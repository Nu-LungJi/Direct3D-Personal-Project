#include "Navigator.h"
#include "GameInstance.h"
#include "AABBCollider.h"

Navigator::Navigator(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GRPDEV(_GRPDEV), DEVCTX(_DEVCTX) {}

HRESULT Navigator::Initialize_Navigator() {

#ifdef _DEBUG
    Component_HorizontalCellShader = Shader::Create(GRPDEV, DEVCTX, L"../../Shader_NormalCell.hlsl", VTXPOS::Elements, VTXPOS::ElementsCount);
    if (nullptr == Component_HorizontalCellShader) return E_FAIL;
    _float4 NColor = { 0.64f, 0.286f, 0.643f, 0.8f };
    Component_HorizontalCellShader->Bind_RawValue("g_vColor", &NColor, sizeof(XMFLOAT4));

    Component_VerticalCellShader = Shader::Create(GRPDEV, DEVCTX, L"../../Shader_NormalCell.hlsl", VTXPOS::Elements, VTXPOS::ElementsCount);
    if (nullptr == Component_VerticalCellShader) return E_FAIL;
    _float4 VColor = { 0.615f, 0.784f, 0.098f, 0.8f };
    Component_VerticalCellShader->Bind_RawValue("g_vColor", &VColor, sizeof(XMFLOAT4));

    Component_DiagonalCellShader = Shader::Create(GRPDEV, DEVCTX, L"../../Shader_NormalCell.hlsl", VTXPOS::Elements, VTXPOS::ElementsCount);
    if (nullptr == Component_DiagonalCellShader) return E_FAIL;
    _float4 DColor = { 3.f / 255.f, 160.f / 255.f, 1.f, 0.8f };
    Component_DiagonalCellShader->Bind_RawValue("g_vColor", &DColor, sizeof(XMFLOAT4));

    Component_BlockCellShader = Shader::Create(GRPDEV, DEVCTX, L"../../Shader_NormalCell.hlsl", VTXPOS::Elements, VTXPOS::ElementsCount);
    if (nullptr == Component_BlockCellShader) return E_FAIL;
    _float4 BColor = { 1.f, 0.f, 0.f, 0.8f };
    Component_BlockCellShader->Bind_RawValue("g_vColor", &BColor, sizeof(XMFLOAT4));
#endif 

    return S_OK;
}
HRESULT Navigator::Save_NavigationData(filesystem::path& _FilePath) {

    time_t timer = time(nullptr); tm now;
    localtime_s(&now, &timer);

    wstring FilePath = _FilePath.wstring() + L"_" + to_wstring(now.tm_mday + 1) + to_wstring(now.tm_hour + 1)
        + to_wstring(now.tm_min + 1) + to_wstring(now.tm_sec + 1) + L".bin";

    DWORD DWByte = {};

    HANDLE FileHandle = CreateFile(FilePath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (0 == FileHandle)
        return E_FAIL;
    for (uint32_t IDX = 0; IDX < static_cast<uint32_t>(NAVMESH_TYPE::NAVMESH_END); ++IDX) {
        for (auto& Cell : NavMeshCellList[IDX]) {
            XMFLOAT3 VTXA, VTXB, VTXC;

            XMStoreFloat3(&VTXA, Cell->Get_CellVertex(NAVMESH_VERTEX::A));
            XMStoreFloat3(&VTXB, Cell->Get_CellVertex(NAVMESH_VERTEX::B));
            XMStoreFloat3(&VTXC, Cell->Get_CellVertex(NAVMESH_VERTEX::C));
            XMFLOAT3 Vertices[3] = { VTXA, VTXB, VTXC };

            WriteFile(FileHandle, Vertices, sizeof(XMFLOAT3) * 3, &DWByte, nullptr);

            uint32_t CellType = static_cast<uint32_t>(Cell->Get_NavMeshType());
            WriteFile(FileHandle, &CellType, sizeof(uint32_t), &DWByte, nullptr);
        }
    }
    

    CloseHandle(FileHandle);

    return S_OK;
}

NavigationResult Navigator::Generate_NavMeshRay() {
    if (nullptr == MainCamera)	MainCamera = GameInstance::GetInstance().Get_MainCamera();
    
    XMMATRIX    ProjectionMatrix    = MainCamera->Get_ProjMatrix();
    XMMATRIX    InvViewMatrix       = MainCamera->Get_InvViewMatrix();

    XMFLOAT3    NormalizedCoord     = Compute_ViewRayDirection(ProjectionMatrix);

    XMVECTOR    RayOrigin           = XMVectorSet(0.f, 0.f, 0.f, 1.f);
    XMVECTOR    RayDirection        = XMVectorSet(NormalizedCoord.x, NormalizedCoord.y, 1.f, 0.f);

    WorldRayOrigin                  = XMVector3TransformCoord(RayOrigin, InvViewMatrix);
    WorldRayDirection               = XMVector3TransformNormal(RayDirection, InvViewMatrix);
    WorldRayDirection               = XMVector3Normalize(WorldRayDirection);
    
    CastedObject = RayCast_StaticMesh();
    
    if (Control_Lock && nullptr != CastedObject) {
        NavigationResult NavRayResult = Compute_StaticMeshTriangle(LocalRayOrigin, LocalRayDirection, CastedObject);

        XMFLOAT3 NavMeshPoints[3] = { NavRayResult.Vertex00, NavRayResult.Vertex01, NavRayResult.Vertex02 };
        
        if (NavRayResult.Vertex00.x < -10000.f || 
              (XMVector3Equal(XMLoadFloat3(&NavRayResult.Vertex00), XMLoadFloat3(&NavRayResult.Vertex01))
            && XMVector3Equal(XMLoadFloat3(&NavRayResult.Vertex00), XMLoadFloat3(&NavRayResult.Vertex02)) 
            && XMVector3Equal(XMLoadFloat3(&NavRayResult.Vertex01), XMLoadFloat3(&NavRayResult.Vertex02)))) return NavigationResult();
        for (uint32_t IDX = 0; IDX < static_cast<uint32_t>(NAVMESH_TYPE::NAVMESH_END); ++IDX) {
            for (auto& Cell : NavMeshCellList[IDX]) {
                uint32_t ExactCount = 0;
                for (uint32_t IDX = 0; IDX < 3; ++IDX) {
                    if (XMVector3Equal(Cell->Get_CellVertex(NAVMESH_VERTEX::A), XMLoadFloat3(&NavMeshPoints[IDX]))) ExactCount++;
                    if (XMVector3Equal(Cell->Get_CellVertex(NAVMESH_VERTEX::B), XMLoadFloat3(&NavMeshPoints[IDX]))) ExactCount++;
                    if (XMVector3Equal(Cell->Get_CellVertex(NAVMESH_VERTEX::C), XMLoadFloat3(&NavMeshPoints[IDX]))) ExactCount++; 
                }
                if (ExactCount == 3) return NavRayResult;
            }
        }
        NavMeshCellList[static_cast<uint32_t>(CurrentNavMeshType)].push_back(NavMeshCell::Create(GRPDEV, DEVCTX, NavMeshPoints,
            static_cast<uint32_t>(NavMeshCellList[static_cast<uint32_t>(CurrentNavMeshType)].size()), CurrentNavMeshType));

        return NavRayResult;
    }
    return NavigationResult();
}

HRESULT Navigator::Eliminate_NavMeshCell() {
    if (nullptr == MainCamera)	MainCamera = GameInstance::GetInstance().Get_MainCamera();

    XMMATRIX    ProjectionMatrix   = MainCamera->Get_ProjMatrix();
    XMMATRIX    InvViewMatrix      = MainCamera->Get_InvViewMatrix();

    XMFLOAT3    NormalizedCoord    = Compute_ViewRayDirection(ProjectionMatrix);

    XMVECTOR    RayOrigin          = XMVectorSet(0.f, 0.f, 0.f, 0.f);
    XMVECTOR    RayDirection       = XMVectorSet(NormalizedCoord.x, NormalizedCoord.y, 1.f, 0.f);

    WorldRayOrigin                 = XMVector3TransformCoord(RayOrigin, InvViewMatrix);
    WorldRayDirection              = XMVector3TransformNormal(RayDirection, InvViewMatrix);
    WorldRayDirection              = XMVector3Normalize(WorldRayDirection);

    CastedObject = RayCast_StaticMesh();
    
    if (Control_Lock && nullptr != CastedObject) {
        NavigationResult NavRayResult = Compute_StaticMeshTriangle(LocalRayOrigin, LocalRayDirection, CastedObject);

        XMFLOAT3 NavMeshPoints[3] = { NavRayResult.Vertex00, NavRayResult.Vertex01, NavRayResult.Vertex02 };
        for (uint32_t IDX = 0; IDX < static_cast<uint32_t>(NAVMESH_TYPE::NAVMESH_END); ++IDX) {
            for (auto iter = NavMeshCellList[IDX].begin(); iter != NavMeshCellList[IDX].end(); iter++) {
                uint32_t ExactCount = 0;
                for (uint32_t IDX = 0; IDX < 3; ++IDX) {
                    if (XMVector3Equal((*iter)->Get_CellVertex(NAVMESH_VERTEX::A), XMLoadFloat3(&NavMeshPoints[IDX]))) ExactCount++;
                    if (XMVector3Equal((*iter)->Get_CellVertex(NAVMESH_VERTEX::B), XMLoadFloat3(&NavMeshPoints[IDX]))) ExactCount++;
                    if (XMVector3Equal((*iter)->Get_CellVertex(NAVMESH_VERTEX::C), XMLoadFloat3(&NavMeshPoints[IDX]))) ExactCount++; 
                }
                if (ExactCount == 3) {
                    shared_ptr<NavMeshCell> Target = (*iter);
                    iter = NavMeshCellList[IDX].erase(iter);
                    Target.reset();

                    return S_OK;
                }
            }
        }
        return E_FAIL;
    }

    return S_OK;
}

shared_ptr<GameObject> Navigator::RayCast_StaticMesh() {
    _float MinimumDistance = FLT_MAX;
    CastedObjectList.clear();
    auto RecipientList = GameInstance::GetInstance().Get_RayCaster()->Get_RecipientList();

    for (auto& Recipient : *RecipientList) {
        shared_ptr<Transform> TransformCMP = static_pointer_cast<Transform>(Recipient->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
        if (nullptr == TransformCMP)	continue;
        XMMATRIX InvWorld   = XMMatrixInverse(0, TransformCMP->Get_WorldMatrix());

        LocalRayOrigin      = XMVector3TransformCoord(WorldRayOrigin, InvWorld);
        LocalRayDirection   = XMVector3TransformNormal(WorldRayDirection, InvWorld);
        LocalRayDirection   = XMVector3Normalize(LocalRayDirection);

        if (Control_Lock)    break;

        _float Distance = 0.f;
        shared_ptr<ParentCollider> ColliderBox = static_pointer_cast<Collider>(Recipient->Find_Component(COMPONENT_TYPE::COMPONENT_COLLIDER))->Get_ColliderBox();

        BoundingBox BBox = *static_pointer_cast<AABBCollider>(ColliderBox)->Get_ColliderBox().get();
        XMStoreFloat3(&BBox.Center, XMVector3TransformCoord(XMLoadFloat3(&BBox.Center), InvWorld));
        XMStoreFloat3(&BBox.Extents, XMVector3TransformNormal(XMLoadFloat3(&BBox.Extents), InvWorld));
        if (BBox.Intersects(LocalRayOrigin, LocalRayDirection, Distance)) {
            CastedObjectList.insert({ Distance, Recipient });
            if (Distance < MinimumDistance) {
                MinimumDistance = Distance;
                CastedObject = Recipient;
            }
        }
    }
    if (nullptr == CastedObject) return nullptr;

    shared_ptr<Transform>	TransformCMP = static_pointer_cast<Transform>(CastedObject->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
    XMMATRIX InvWorld = XMMatrixInverse(0, TransformCMP->Get_WorldMatrix());
    LocalRayOrigin      = XMVector3TransformCoord(WorldRayOrigin, InvWorld);
    LocalRayDirection   = XMVector3TransformNormal(WorldRayDirection, InvWorld);
    LocalRayDirection   = XMVector3Normalize(LocalRayDirection);

    return CastedObject;
}
VOID Navigator::Reset_Navigator() {
    for (auto& CellList : NavMeshCellList) {
        for (auto& Cell : CellList) {
            if (nullptr != Cell) Cell.reset();
        }
    }
}
#ifdef _DEBUG
VOID Navigator::Render_NavMeshCell() {
    Render_HorizontalCell();
    Render_VerticalCell();
    Render_DiagonalCell();
    Render_BlockCell();
}
VOID Navigator::Render_HorizontalCell() {
    XMFLOAT4X4 WorldMatrix = {};
    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

    if (FAILED(Component_HorizontalCellShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))    return;
    if (FAILED(Component_HorizontalCellShader->Bind_Matrix("g_ViewMatrix", GameInstance::GetInstance().Get_MainCamera()->Get_RawViewMatrix())))    return;
    if (FAILED(Component_HorizontalCellShader->Bind_Matrix("g_ProjMatrix", GameInstance::GetInstance().Get_MainCamera()->Get_RawProjMatrix())))    return;

    Component_HorizontalCellShader->Shader_Begin(0);

    for (auto& Cell : NavMeshCellList[static_cast<uint32_t>(NAVMESH_TYPE::NAVMESH_HORIZONTAL)]) {
        if (nullptr != Cell) Cell->Render_NavigationCell();
    }
}
VOID Navigator::Render_VerticalCell() {
    XMFLOAT4X4 WorldMatrix = {};
    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

    if (FAILED(Component_VerticalCellShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))    return;
    if (FAILED(Component_VerticalCellShader->Bind_Matrix("g_ViewMatrix", GameInstance::GetInstance().Get_MainCamera()->Get_RawViewMatrix())))    return;
    if (FAILED(Component_VerticalCellShader->Bind_Matrix("g_ProjMatrix", GameInstance::GetInstance().Get_MainCamera()->Get_RawProjMatrix())))    return;

    Component_VerticalCellShader->Shader_Begin(0);

    for (auto& Cell : NavMeshCellList[static_cast<uint32_t>(NAVMESH_TYPE::NAVMESH_VERTICAL)]) {
        if (nullptr != Cell) Cell->Render_NavigationCell();
    }
}
VOID Navigator::Render_DiagonalCell() {
    XMFLOAT4X4 WorldMatrix = {};
    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

    if (FAILED(Component_DiagonalCellShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))    return;
    if (FAILED(Component_DiagonalCellShader->Bind_Matrix("g_ViewMatrix", GameInstance::GetInstance().Get_MainCamera()->Get_RawViewMatrix())))    return;
    if (FAILED(Component_DiagonalCellShader->Bind_Matrix("g_ProjMatrix", GameInstance::GetInstance().Get_MainCamera()->Get_RawProjMatrix())))    return;

    Component_DiagonalCellShader->Shader_Begin(0);

    for (auto& Cell : NavMeshCellList[static_cast<uint32_t>(NAVMESH_TYPE::NAVMESH_DIAGONAL)]) {
        if (nullptr != Cell) Cell->Render_NavigationCell();
    }
}
VOID Navigator::Render_BlockCell() {
    XMFLOAT4X4 WorldMatrix = {};
    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

    if (FAILED(Component_BlockCellShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))    return;
    if (FAILED(Component_BlockCellShader->Bind_Matrix("g_ViewMatrix", GameInstance::GetInstance().Get_MainCamera()->Get_RawViewMatrix())))    return;
    if (FAILED(Component_BlockCellShader->Bind_Matrix("g_ProjMatrix", GameInstance::GetInstance().Get_MainCamera()->Get_RawProjMatrix())))    return;

    Component_BlockCellShader->Shader_Begin(0);

    for (auto& Cell : NavMeshCellList[static_cast<uint32_t>(NAVMESH_TYPE::NAVMESH_BLOCK)]) {
        if (nullptr != Cell) Cell->Render_NavigationCell();
    }
}
#endif
unique_ptr<Navigator> Navigator::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
    auto NG = unique_ptr<Navigator>(new Navigator(_GRPDEV, _DEVCTX));
    if (FAILED(NG->Initialize_Navigator()))
        MSG_BOX("Cannot Create Navigator.");

    return NG;
}

XMFLOAT3 Navigator::Compute_ViewRayDirection(XMMATRIX _ProjectionMat) {
    if (nullptr == MainCamera)	MainCamera = GameInstance::GetInstance().Get_MainCamera();

    const ENGINE_DESC& EngineOption = GameInstance::GetInstance().Get_GraphicDevice()->Get_EngineDescription();

    POINT ptMouse;
    GetCursorPos(&ptMouse);
    ScreenToClient(EngineOption.hWnd, &ptMouse);

    XMMATRIX ProjectionMatrix = MainCamera->Get_ProjMatrix();
    XMMATRIX ViewMatrix = MainCamera->Get_ViewMatrix();
    XMMATRIX InvViewMatrix = MainCamera->Get_InvViewMatrix();

    // 스크린 좌표 (0, 0) ~ (1280, 720) >> NDC 좌표 (-1, -1) ~ (1, 1) 변환 >> ViewSpace 좌표 변환
    // ViewSpace : 카메라는 원점(0, 0, 0), Z축을 바라보게 만드는 과정
    RECT rc{};
    HWND hWnd = EngineOption.hWnd;
    GetWindowRect(hWnd, &rc);

    _float NDCX = ((2.f * ptMouse.x) / EngineOption.WindowResolutionX - 1.f) / ProjectionMatrix.r[0].m128_f32[0];
    _float NDCY = ((-2.f * ptMouse.y) / EngineOption.WindowResolutionY + 1.f) / ProjectionMatrix.r[1].m128_f32[1];

    return XMFLOAT3(NDCX, NDCY, 1.f);
}

VOID Navigator::Compute_SubMeshTriangle(XMVECTOR _RayOrigin, XMVECTOR _RayDirection, NavigationResult* _Result, shared_ptr<StaticMesh> _StaticMesh, uint32_t _SubMeshIndex) {

    const auto& VertexList = *_StaticMesh->Get_VertexList();
    const auto& IndexList = *_StaticMesh->Get_IndexList();

    for (uint32_t VIDX = 0; VIDX < _StaticMesh->Get_IndexList()->size(); VIDX += 3) {
        XMVECTOR Vertex00 = XMLoadFloat3(&VertexList[IndexList[VIDX + 0]].Position);
        XMVECTOR Vertex01 = XMLoadFloat3(&VertexList[IndexList[VIDX + 1]].Position);
        XMVECTOR Vertex02 = XMLoadFloat3(&VertexList[IndexList[VIDX + 2]].Position);

        _float TriangleDistance = 0.f;
        if (DirectX::TriangleTests::Intersects(_RayOrigin, _RayDirection, Vertex00, Vertex01, Vertex02, TriangleDistance)) {
            if (TriangleDistance < _Result->Distance) {
                _Result->Distance = TriangleDistance;
                _Result->NavMeshFaceIndex = static_cast<uint32_t>(VIDX / 3);
                _Result->SubMeshIndex = static_cast<int32_t>(_SubMeshIndex);

                XMStoreFloat3(&_Result->Vertex00, Vertex00);
                XMStoreFloat3(&_Result->Vertex01, Vertex01);
                XMStoreFloat3(&_Result->Vertex02, Vertex02);
            }
        }
    }
}
VOID Navigator::Compute_SubMeshTriangle(XMVECTOR _RayOrigin, XMVECTOR _RayDirection, NavigationResult* _Result, shared_ptr<InstanceMesh> _InstanceMesh, uint32_t _SubMeshIndex) {
    const auto& VertexList = *_InstanceMesh->Get_VertexList();
    const auto& IndexList = *_InstanceMesh->Get_IndexList();

    for (uint32_t VIDX = 0; VIDX < _InstanceMesh->Get_IndexList()->size(); VIDX += 3) {
        XMVECTOR Vertex00 = XMLoadFloat3(&VertexList[IndexList[VIDX + 0]].Position);
        XMVECTOR Vertex01 = XMLoadFloat3(&VertexList[IndexList[VIDX + 1]].Position);
        XMVECTOR Vertex02 = XMLoadFloat3(&VertexList[IndexList[VIDX + 2]].Position);

        _float TriangleDistance = 0.f;
        if (DirectX::TriangleTests::Intersects(_RayOrigin, _RayDirection, Vertex00, Vertex01, Vertex02, TriangleDistance)) {
            if (TriangleDistance < _Result->Distance) {
                _Result->Distance = TriangleDistance;
                _Result->NavMeshFaceIndex = static_cast<uint32_t>(VIDX / 3);
                _Result->SubMeshIndex = static_cast<int32_t>(_SubMeshIndex);

                XMStoreFloat3(&_Result->Vertex00, Vertex00);
                XMStoreFloat3(&_Result->Vertex01, Vertex01);
                XMStoreFloat3(&_Result->Vertex02, Vertex02);
            }
        }
    }
}
NavigationResult Navigator::Compute_StaticMeshTriangle(XMVECTOR _RayOrigin, XMVECTOR _RayDirection, shared_ptr<GameObject> _CastedObject) {
    shared_ptr<MeshLoader> Component_Model = static_pointer_cast<MeshLoader>(_CastedObject->Find_Component(COMPONENT_TYPE::COMPONENT_MODEL));
    vector<shared_ptr<InstanceMesh>>* InstanceMeshList = Component_Model->Get_InstanceMeshList();

    NavigationResult Result;

    for (uint32_t SIDX = 0; SIDX < InstanceMeshList->size(); ++SIDX) {
        shared_ptr<InstanceMesh> SM = (*InstanceMeshList)[SIDX];
        shared_ptr<AABBCollider> Collider = SM->Get_BoundingBox();

        _float SubMeshBoxDistance = 0.f;
        if (Collider->Get_ColliderBox()->Intersects(_RayOrigin, _RayDirection, SubMeshBoxDistance)) { continue; }

        Compute_SubMeshTriangle(_RayOrigin, _RayDirection, &Result, SM, SIDX);
    }
    Convert_WorldCoordinate(&Result, CastedObject);

    return Result;
}
VOID Navigator::Convert_WorldCoordinate(NavigationResult* _Result, shared_ptr<GameObject> _CastedObject) {
    if (_Result->Distance < FLT_MAX) {
        shared_ptr<Transform> Component_Transform = static_pointer_cast<Transform>(_CastedObject->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));

        if (Component_Transform) {
            XMMATRIX WorldMatrix = Component_Transform->Get_WorldMatrix();
            XMStoreFloat3(&_Result->Vertex00, XMVector3TransformCoord(XMLoadFloat3(&_Result->Vertex00), WorldMatrix));
            XMStoreFloat3(&_Result->Vertex01, XMVector3TransformCoord(XMLoadFloat3(&_Result->Vertex01), WorldMatrix));
            XMStoreFloat3(&_Result->Vertex02, XMVector3TransformCoord(XMLoadFloat3(&_Result->Vertex02), WorldMatrix));
        }
    }
}
HRESULT Navigator::Select_NextRayCastedObject() {

    auto iterator = CastedObjectList.begin();
    for (; iterator != CastedObjectList.end(); ++iterator) {
        if (iterator->second == CastedObject) break;
    }

    if (iterator == CastedObjectList.end()) return E_FAIL;

    iterator = next(iterator);
    if (iterator == CastedObjectList.end())	iterator = CastedObjectList.begin();
    CastedObject = iterator->second;

    return S_OK;
}