#pragma once
#include "Component.h"
#include "NavMeshCell.h"
#include "Shader.h"

BEGIN(Engine)
class Transform;
class ENGINE_DLL NavMeshAgent : public Component {
private:
	NavMeshAgent(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	NavMeshAgent(CONST NavMeshAgent& _PRTOBJ);
public:
	virtual ~NavMeshAgent() = default;

public:
	virtual HRESULT	Initialize_ProtoType()		override;
	virtual HRESULT	Initialize(VOID* _ARG)		override;

	_bool			Evaluate_NextPositionCell(XMVECTOR _Position, _float* _OutHeight, _float* _CurrCellHeight, const _float& _DT);

	HRESULT			Evaluate_AdjacentCell();

	XMVECTOR		SetUp_OnNavigation(XMVECTOR _Position);
	VOID			Set_CurrentCellIndex(int32_t _IDX) { CurrentCellIndex = _IDX; }

	HRESULT			Load_NavigationData_FromCurrentFile(const string& _Directory);
	HRESULT			Load_NavigationData(const string& _Directory);
	VOID			Register_CellDataToNavigator();
	int32_t			Find_NewUnderCell(XMVECTOR _Position);

	VOID			Copy_CellList(shared_ptr<NavMeshAgent> _Agent);

	shared_ptr<NavMeshCell>	Get_CellByIndex(uint32_t _IDX);

	VOID			Register_TransformComponent(shared_ptr<Transform> _TRS) { Component_Transform = _TRS; }

	_bool			RayCast_NextPosition(XMVECTOR _Position, _float* _OutHeight, int32_t* _OutIndex);

	VOID			Set_CurrentUnderCellHeight(_float _Height) { CurrentUnderCellHeight = _Height; }
	_float			Get_CurrentUnderCellHeight() { return CurrentUnderCellHeight; }

public:
	virtual shared_ptr<Component>		Clone(VOID* _ARG);
	static  unique_ptr<NavMeshAgent>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);

	VOID	Add_Cell(shared_ptr<NavMeshCell> _Cell) { CellList.push_back(_Cell); }
	shared_ptr<NavMeshCell>	Get_CurrentCell() { return CellList[CurrentCellIndex]; }

private:
	int32_t								CurrentCellIndex = { 0 };

	vector<shared_ptr<NavMeshCell>>		CellList;

	_bool								Enable_Navigation = { true };
	shared_ptr<Transform>				Component_Transform = { nullptr };

	_float		Height = { 0.f };
	_bool		FirstFrameInit = { true };

	_float		CurrentUnderCellHeight = { 0.f };
};
END