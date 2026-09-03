#include "NavMeshAgent.h"
#include "GameInstance.h"
#include "Transform.h"

NavMeshAgent::NavMeshAgent(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : Component(_GRPDEV, _DEVCTX) {}
NavMeshAgent::NavMeshAgent(CONST NavMeshAgent& _PRTOBJ) : Component(_PRTOBJ), CellList(_PRTOBJ.CellList){}

HRESULT NavMeshAgent::Initialize_ProtoType() {

    return S_OK;
}
HRESULT NavMeshAgent::Initialize(VOID* _ARG) {

    return S_OK;
}
_bool NavMeshAgent::Evaluate_NextPositionCell(XMVECTOR _EvaluatePosition, _float* _OutHeight, _float* _CurrCellHeight, const _float& _DT) {
    
	_float  ClosestHeight = 0.f;
	int32_t ClosestCellIndex = -1;

    if (RayCast_NextPosition(_EvaluatePosition, &ClosestHeight, &ClosestCellIndex)) {
        NAVMESH_TYPE NavMeshType = CellList[ClosestCellIndex]->Get_NavMeshType();
        if (NavMeshType == NAVMESH_TYPE::NAVMESH_BLOCK) {
			return false;     // BLOCK Cell : 이동 불가
        }
        _float CurrentPlayerY = XMVectorGetY(Component_Transform->Get_WorldPosition());
		_float HeightInterval = ClosestHeight - CurrentPlayerY;
        if (NavMeshType == NAVMESH_TYPE::NAVMESH_VERTICAL && HeightInterval > 1.f) {
            return false;     // VERTICAL Cell : 벽 높이 높은 경우 이동 불가
        }
        if (NavMeshType == NAVMESH_TYPE::NAVMESH_HORIZONTAL && HeightInterval > 1.3f) {
			return false;     // HORIZONTAL Cell : 높이 차이 큰 경우 이동 불가
        }
		CurrentCellIndex = ClosestCellIndex;
        _float Ratio = _DT * 12.f > 1.f ? 1.f : _DT * 12.f;
        _float SmoothedHeight = CurrentPlayerY + (ClosestHeight - CurrentPlayerY) * Ratio;

        *_OutHeight = SmoothedHeight;
        *_CurrCellHeight = ClosestHeight;

        return true;
    }

    return false;
}
HRESULT NavMeshAgent::Load_NavigationData_FromCurrentFile(const string& _Directory) {

    DWORD       dwByte = {};
    HANDLE      NavigationFile = {};

    filesystem::path Directory = _Directory; // 탐색할 경로
    filesystem::path latest_file;
    filesystem::file_time_type max_time;

    wstring FilePath = L"";

    try {
        if (!filesystem::exists(Directory) || !filesystem::is_directory(Directory)) {
            return E_FAIL;
        }

        for (const auto& entry : filesystem::directory_iterator(Directory)) {
            if (filesystem::is_regular_file(entry) && entry.path().extension() == ".bin") {

                auto current_time = filesystem::last_write_time(entry);
                if (latest_file.empty() || current_time > max_time) {
                    max_time = current_time;
                    latest_file = entry.path();
                }
            }
        }
    }
    catch (_bool i) {
        i = false;
    }
    FilePath = latest_file.wstring();

    if (filesystem::exists(FilePath)) {
        NavigationFile = CreateFile(FilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (0 == NavigationFile)
            return E_FAIL;

        while (true)
        {
            _float3     CellVertex[static_cast<uint32_t>(NAVMESH_VERTEX::VTX_END)] = {};

			uint32_t    CellType = { 1 };

            DWORD dwVertexSize = sizeof(_float3) * static_cast<uint32_t>(NAVMESH_VERTEX::VTX_END);
            ReadFile(NavigationFile, CellVertex, dwVertexSize, &dwByte, nullptr);
            
            if (0 == dwByte)  break;

            ReadFile(NavigationFile, &CellType, sizeof(uint32_t), &dwByte, nullptr);

            auto        pCell = NavMeshCell::Create(GRPDEV, DEVCTX, CellVertex, static_cast<uint32_t>(CellList.size()), static_cast<NAVMESH_TYPE>(CellType));
            if (nullptr == pCell)
                return E_FAIL;
            CellList.push_back(pCell);
        }

        CloseHandle(NavigationFile);

        if (FAILED(Evaluate_AdjacentCell())) {
            MSG_BOX("Cannot Specify Adjacent Cell.");
            return E_FAIL;
        }

        Register_CellDataToNavigator();
    }
    
    return S_OK;
}
HRESULT NavMeshAgent::Load_NavigationData(const string& _FilePath) {
    DWORD       dwByte = {};
    HANDLE      NavigationFile = {};

    filesystem::path FilePath = _FilePath;

    if (filesystem::exists(FilePath)) {
        NavigationFile = CreateFile(FilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (0 == NavigationFile)
            return E_FAIL;

        while (true)
        {
            _float3     CellVertex[static_cast<uint32_t>(NAVMESH_VERTEX::VTX_END)] = {};

            uint32_t    CellType = { 1 };

            DWORD dwVertexSize = sizeof(_float3) * static_cast<uint32_t>(NAVMESH_VERTEX::VTX_END);
            ReadFile(NavigationFile, CellVertex, dwVertexSize, &dwByte, nullptr);

            if (0 == dwByte)  break;

            ReadFile(NavigationFile, &CellType, sizeof(uint32_t), &dwByte, nullptr);
           

            auto        pCell = NavMeshCell::Create(GRPDEV, DEVCTX, CellVertex, static_cast<uint32_t>(CellList.size()), static_cast<NAVMESH_TYPE>(CellType));
            if (nullptr == pCell)
                return E_FAIL;
            CellList.push_back(pCell);
        }

        CloseHandle(NavigationFile);

        if (FAILED(Evaluate_AdjacentCell())) {
            MSG_BOX("Cannot Specify Adjacent Cell.");
            return E_FAIL;
        }

        Register_CellDataToNavigator();
    }
    return S_OK;
}
VOID NavMeshAgent::Register_CellDataToNavigator() {
    for (auto& Cell : CellList) {
        if(Cell != nullptr)        GameInstance::GetInstance().Get_Navigator()->Register_Cell(Cell, Cell->Get_NavMeshType());
    }
}
int32_t NavMeshAgent::Find_NewUnderCell(XMVECTOR _Position) {
    int32_t CellCount = static_cast<int32_t>(CellList.size());

    for (int32_t IDX = 0; IDX < CellCount; ++IDX) {
        if (CellList[IDX]->Check_ObjectOnCell_XZ(_Position)) {
            _float CellHeight    = CellList[IDX]->Compute_CellHeight(_Position);
            _float CurrentHeight = XMVectorGetY(_Position);

            if (fabsf(CellHeight - CurrentHeight) < 1.f)
                return IDX;
        }
    }
    return -1;
}
VOID NavMeshAgent::Copy_CellList(shared_ptr<NavMeshAgent> _Agent){
    for (auto& Cell : CellList) 
        _Agent->Add_Cell(Cell);
}
shared_ptr<NavMeshCell> NavMeshAgent::Get_CellByIndex(uint32_t _IDX) {
    for (auto& Cell : CellList) {
        if (Cell->Get_CellIndex() == _IDX)        return Cell;
    }
    return nullptr;
}

HRESULT NavMeshAgent::Evaluate_AdjacentCell() {
    for (auto& SRC : CellList) {
        for (auto& DST : CellList) {
            if (SRC == DST) continue;

            if      (DST->Compare_EqualVertex(SRC->Get_CellVertex(NAVMESH_VERTEX::A), SRC->Get_CellVertex(NAVMESH_VERTEX::B))) {
                SRC->Set_AdjacentCellIndex(NAVMESH_LINE::AB, DST->Get_CellIndex());                // SRC - DST : AB면 동일
            }
            else if (DST->Compare_EqualVertex(SRC->Get_CellVertex(NAVMESH_VERTEX::B), SRC->Get_CellVertex(NAVMESH_VERTEX::C))) {
                SRC->Set_AdjacentCellIndex(NAVMESH_LINE::BC, DST->Get_CellIndex());                // SRC - DST : BC면 동일
            }
            else if (DST->Compare_EqualVertex(SRC->Get_CellVertex(NAVMESH_VERTEX::C), SRC->Get_CellVertex(NAVMESH_VERTEX::A))) {
                SRC->Set_AdjacentCellIndex(NAVMESH_LINE::CA, DST->Get_CellIndex());                // SRC - DST : CA면 동일
            }
        }
    }
    return S_OK;
}

XMVECTOR NavMeshAgent::SetUp_OnNavigation(XMVECTOR _Position) {
    float ComputedValue = CellList[CurrentCellIndex]->Compute_CellHeight(_Position);
    XMVECTOR ReturnValue = XMVectorSetY(_Position, ComputedValue);
    return ReturnValue;
}

_bool NavMeshAgent::RayCast_NextPosition(XMVECTOR _Position, _float* _OutHeight, int32_t* _OutIndex) {
	_float      ClosestDistance = -FLT_MAX;
	int32_t 	ClosestCellIndex = { -1 };
    _float      RayOrigin = XMVectorGetY(_Position) + 1.2f;
    
    for (int32_t IDX = 0; IDX < CellList.size(); ++IDX) {
        if (CellList[IDX]->Check_ObjectOnCell_XZ(_Position)) {
            _float CellHeight = CellList[IDX]->Compute_CellHeight(_Position);
		    
            if (CellHeight <= RayOrigin && CellHeight > ClosestDistance) {
				ClosestDistance = CellHeight;
				ClosestCellIndex = IDX;
            }
        }
    }

    if (ClosestCellIndex != -1) {
		*_OutHeight = ClosestDistance;
		*_OutIndex  = ClosestCellIndex;
        return true;
    }
    return false;
}
unique_ptr<NavMeshAgent>	NavMeshAgent::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
    auto Instance = unique_ptr<NavMeshAgent>(new NavMeshAgent(_GRPDEV, _DEVCTX));
    if (FAILED(Instance->Initialize_ProtoType())) {
        MSG_BOX("Cannot Create NavMeshAgent");
        return nullptr;
    }
    return Instance;
}
shared_ptr<Component>	    NavMeshAgent::Clone(VOID* _ARG) {
    auto Instance = shared_ptr<NavMeshAgent>(new NavMeshAgent(*this));
    if (FAILED(Instance->Initialize(_ARG))) {
        MSG_BOX("Cannot Clone NavMeshAgent");
        return nullptr;
    }
    return Instance;
}
