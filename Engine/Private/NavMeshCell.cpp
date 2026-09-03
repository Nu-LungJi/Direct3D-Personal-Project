#include "NavMeshCell.h"

NavMeshCell::NavMeshCell(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GRPDEV(_GRPDEV), DEVCTX(_DEVCTX) {}

HRESULT NavMeshCell::Initialize_NavigationCell(const XMFLOAT3* _CellVertex, int32_t _CellIndex, NAVMESH_TYPE _NTYPE) {

    CellIndex = _CellIndex;

    memcpy(CellVertex, _CellVertex, sizeof(XMFLOAT3) * static_cast<uint32_t>(NAVMESH_VERTEX::VTX_END));
    memset(AdjacentCell_IndexList, -1, sizeof(int32_t) * static_cast<uint32_t>(NAVMESH_LINE::LINE_END));

    uint32_t AB = static_cast<uint32_t>(NAVMESH_LINE::AB);  uint32_t VA = static_cast<uint32_t>(NAVMESH_VERTEX::A);
    uint32_t BC = static_cast<uint32_t>(NAVMESH_LINE::BC);  uint32_t VB = static_cast<uint32_t>(NAVMESH_VERTEX::B);
    uint32_t CA = static_cast<uint32_t>(NAVMESH_LINE::CA);  uint32_t VC = static_cast<uint32_t>(NAVMESH_VERTEX::C);

    XMVECTOR CellLine[static_cast<uint32_t>(NAVMESH_LINE::LINE_END)] = {};

    CellLine[AB] = XMLoadFloat3(&CellVertex[VB]) - XMLoadFloat3(&CellVertex[VA]);
    CellLine[BC] = XMLoadFloat3(&CellVertex[VC]) - XMLoadFloat3(&CellVertex[VB]);
    CellLine[CA] = XMLoadFloat3(&CellVertex[VA]) - XMLoadFloat3(&CellVertex[VC]);

    XMStoreFloat3(&CellNormal[AB], XMVector3Normalize(XMVectorSet(XMVectorGetZ(CellLine[AB]) * -1.f, 0.f, XMVectorGetX(CellLine[AB]), 0.f)));
    XMStoreFloat3(&CellNormal[BC], XMVector3Normalize(XMVectorSet(XMVectorGetZ(CellLine[BC]) * -1.f, 0.f, XMVectorGetX(CellLine[BC]), 0.f)));
    XMStoreFloat3(&CellNormal[CA], XMVector3Normalize(XMVectorSet(XMVectorGetZ(CellLine[CA]) * -1.f, 0.f, XMVectorGetX(CellLine[CA]), 0.f)));

    NavMeshType = _NTYPE;
    if (NavMeshType == NAVMESH_TYPE::NAVMESH_DIAGONAL) {
        XMStoreFloat4(&CellPlane, XMPlaneFromPoints(XMLoadFloat3(&CellVertex[VA]), XMLoadFloat3(&CellVertex[VB]), XMLoadFloat3(&CellVertex[VC])));
    }
    
#ifdef _DEBUG
    CellBuffer = CellBuffer::Create(GRPDEV, DEVCTX, CellVertex);
    if (nullptr == CellBuffer) return E_FAIL;
#endif

    return S_OK;
}

shared_ptr<NavMeshCell> NavMeshCell::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX, const XMFLOAT3* _CellVertex, int32_t _CellIndex, NAVMESH_TYPE _NTYPE) {
    auto Instance = shared_ptr<NavMeshCell>(new NavMeshCell(_GRPDEV, _DEVCTX)); 
    if (FAILED(Instance->Initialize_NavigationCell(_CellVertex, _CellIndex, _NTYPE))) {
        MSG_BOX("Cannot Create NavMeshCell");
        return nullptr;
    }
    return Instance;
}


_bool NavMeshCell::Check_ObjectOnCell(XMVECTOR _CurrentPosition, int32_t* _AdjacentCellIndex) {

    for (uint32_t IDX = 0; IDX < static_cast<uint32_t>(NAVMESH_LINE::LINE_END); ++IDX) {
        XMVECTOR CellVertexToPos = XMVector3Normalize(XMVectorSetW(_CurrentPosition - XMLoadFloat3(&CellVertex[IDX]), 0.f));       // CellVertexToPos : 셀에 있는 정점에서 현재 위치까지의 방향

        if (0 < XMVectorGetX(XMVector3Dot(CellVertexToPos, XMLoadFloat3(&CellNormal[IDX])))) {                                      // XMVectorGetX(result) : 내적 결과 Scalar값
            // 내적을 하게 되면, -90 ~ 90 범위의 θ가 나오는데, cosθ는 양수이기 때문에, 현재 위치(_CurrentPosition)와 삼각형 라인의 외곽으로 직교하는 벡터(CellNormal)가 
            // -90 ~ 90 범위라면 _CurrentPosition이 삼각형의 외곽에 있는 것이고, (모든 셀의 정점들에 대해서 검사한 결과)그보다 큰 범위 라면 cosθ가 음수라서, 삼각형 안에 있다고 할 수 있다(TRUE).
            // 그렇다면 지금 이 if문으로 들어왔다는 것은, "_CurrentPosition이 셀 <외부>에 위치하고 있다."라는 설명이 된다.
            *_AdjacentCellIndex = AdjacentCell_IndexList[IDX];
            // 현재 내가 밟고 있었던 CellList[CurrentCellIndex]의 외부에 있다는 것은 이웃으로 이동한 것이고, 평가된 CellNormal방향으로 나갔다는 것이므로,
            // 나간 방향에 있는 이웃셀의 인덱스를 던져주고 FALSE를 반환한다.
            return false;
        }
    }
    return true;
}

_bool NavMeshCell::Check_ObjectOnCell_XZ(XMVECTOR _CurrentPosition) {
    XMVECTOR A = XMLoadFloat3(&CellVertex[static_cast<uint32_t>(NAVMESH_VERTEX::A)]);
    XMVECTOR B = XMLoadFloat3(&CellVertex[static_cast<uint32_t>(NAVMESH_VERTEX::B)]);
    XMVECTOR C = XMLoadFloat3(&CellVertex[static_cast<uint32_t>(NAVMESH_VERTEX::C)]);

    XMVECTOR AB = XMVectorSet(XMVectorGetX(B) - XMVectorGetX(A), 0.f, XMVectorGetZ(B) - XMVectorGetZ(A), 0.f);
    XMVECTOR BC = XMVectorSet(XMVectorGetX(C) - XMVectorGetX(B), 0.f, XMVectorGetZ(C) - XMVectorGetZ(B), 0.f);
    XMVECTOR CA = XMVectorSet(XMVectorGetX(A) - XMVectorGetX(C), 0.f, XMVectorGetZ(A) - XMVectorGetZ(C), 0.f);

    XMVECTOR AP = XMVectorSet(XMVectorGetX(_CurrentPosition) - XMVectorGetX(A), 0.f, XMVectorGetZ(_CurrentPosition) - XMVectorGetZ(A), 0.f);
    XMVECTOR BP = XMVectorSet(XMVectorGetX(_CurrentPosition) - XMVectorGetX(B), 0.f, XMVectorGetZ(_CurrentPosition) - XMVectorGetZ(B), 0.f);
    XMVECTOR CP = XMVectorSet(XMVectorGetX(_CurrentPosition) - XMVectorGetX(C), 0.f, XMVectorGetZ(_CurrentPosition) - XMVectorGetZ(C), 0.f);

    XMVECTOR CrossAB = XMVector3Cross(AB, AP);
    XMVECTOR CrossBC = XMVector3Cross(BC, BP);
    XMVECTOR CrossCA = XMVector3Cross(CA, CP);

    float SignAB = XMVectorGetY(CrossAB);
    float SignBC = XMVectorGetY(CrossBC);
    float SignCA = XMVectorGetY(CrossCA);

    const float EPSILON = 0.0001f;

    if ((SignAB >= -EPSILON && SignBC >= -EPSILON && SignCA >=-EPSILON) ||
        (SignAB <=  EPSILON && SignBC <=  EPSILON && SignCA <= EPSILON)) {
        return true;
    }
    return false;
}

_bool NavMeshCell::Compare_EqualVertex(XMVECTOR _SRC, XMVECTOR _DST) {      // 두 삼각형의 한 면이 서로 만나는 경우, TRUE
    if (XMVector3Equal(XMLoadFloat3(&CellVertex[static_cast<uint32_t>(NAVMESH_VERTEX::A)]), _SRC)) {
        if      (XMVector3Equal(XMLoadFloat3(&CellVertex[static_cast<uint32_t>(NAVMESH_VERTEX::B)]), _DST))  return true;
        else if (XMVector3Equal(XMLoadFloat3(&CellVertex[static_cast<uint32_t>(NAVMESH_VERTEX::C)]), _DST))  return true;
    }
    if (XMVector3Equal(XMLoadFloat3(&CellVertex[static_cast<uint32_t>(NAVMESH_VERTEX::B)]), _SRC)) {
         if      (XMVector3Equal(XMLoadFloat3(&CellVertex[static_cast<uint32_t>(NAVMESH_VERTEX::C)]), _DST)) return true;
         else if (XMVector3Equal(XMLoadFloat3(&CellVertex[static_cast<uint32_t>(NAVMESH_VERTEX::A)]), _DST)) return true;
    }
    if (XMVector3Equal(XMLoadFloat3(&CellVertex[static_cast<uint32_t>(NAVMESH_VERTEX::C)]), _SRC)) {
        if      (XMVector3Equal(XMLoadFloat3(&CellVertex[static_cast<uint32_t>(NAVMESH_VERTEX::A)]), _DST))  return true;
        else if (XMVector3Equal(XMLoadFloat3(&CellVertex[static_cast<uint32_t>(NAVMESH_VERTEX::B)]), _DST))  return true;
    }

    return false;
}

_float NavMeshCell::Compute_CellHeight(XMVECTOR _CurrentPosition) {
    if      (NavMeshType == NAVMESH_TYPE::NAVMESH_HORIZONTAL) {
		return CellVertex[static_cast<uint32_t>(NAVMESH_VERTEX::A)].y;     // 수평 셀인 경우, 모든 Vertex의 높이가 같다고 가정하고 Vertex A의 높이를 반환
    }
    else if (NavMeshType == NAVMESH_TYPE::NAVMESH_VERTICAL) {           // 수직 셀인 경우, 모든 버텍스 높이의 평균을 반환
        return (CellVertex[static_cast<uint32_t>(NAVMESH_VERTEX::A)].y + 
                CellVertex[static_cast<uint32_t>(NAVMESH_VERTEX::B)].y + 
                CellVertex[static_cast<uint32_t>(NAVMESH_VERTEX::C)].y) / 3.f;
    }
    else if (NavMeshType == NAVMESH_TYPE::NAVMESH_DIAGONAL) {           // 경사 셀인 경우, 평면의 방정식
        return ((-CellPlane.x * XMVectorGetX(_CurrentPosition)) + (-CellPlane.z * XMVectorGetZ(_CurrentPosition)) - CellPlane.w) / CellPlane.y;
    }
    else if (NavMeshType == NAVMESH_TYPE::NAVMESH_BLOCK) {
        return CellVertex[static_cast<uint32_t>(NAVMESH_VERTEX::A)].y;
    }
    
    MSG_BOX("Invalid CellType.");
    assert(0);

    return 0.f;
}

#ifdef _DEBUG
HRESULT NavMeshCell::Render_NavigationCell() {
    
    CellBuffer->Bind_Resources();
    CellBuffer->Render_Buffer();

    return S_OK;
}
#endif    