#pragma once
#include "Engine_Define.h"
#include "NavMeshAgent.h"
#include "NavMeshCell.h"
#include "StaticMesh.h"
#include "FB_NavigationVertex_generated.h"
#include "Camera.h"
#include "Shader.h"

BEGIN(Engine)
class ENGINE_DLL Navigator {
private:
	Navigator(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	virtual ~Navigator() = default;

public:
	HRESULT		Initialize_Navigator();

	HRESULT		Save_NavigationData(filesystem::path& _FilePath);

	NavigationResult		Generate_NavMeshRay();
	HRESULT					Eliminate_NavMeshCell();
	shared_ptr<GameObject>	RayCast_StaticMesh();

	VOID					Reset_Navigator();

	VOID					Set_ControlLock(_bool _Lock)	{ Control_Lock = _Lock; }
	VOID					Set_ControlLock()				{ Control_Lock ? Control_Lock = false : Control_Lock = true; }

	_bool					Get_ControlLock()				{ return Control_Lock; }

	VOID					Register_Cell(shared_ptr<NavMeshCell> _Cell, NAVMESH_TYPE _NTYPE) { 
		NavMeshCellList[static_cast<uint32_t>(_NTYPE)].push_back(_Cell);
		NavMeshCellList[static_cast<uint32_t>(_NTYPE)].back()->Set_NavMeshType(_NTYPE);
	}

	shared_ptr<GameObject>	Get_CastedObject()				{ return CastedObject; }
	VOID					Set_CastedObject(shared_ptr<GameObject> _GOBJ) { CastedObject = _GOBJ; }

	VOID					Set_CurrentNavMeshType(NAVMESH_TYPE _Type) { CurrentNavMeshType = _Type; }	

	HRESULT					Select_NextRayCastedObject();
#ifdef _DEBUG
	VOID		Render_NavMeshCell();

	VOID		Render_HorizontalCell();
	VOID		Render_VerticalCell();
	VOID		Render_DiagonalCell();
	VOID		Render_BlockCell();
#endif
	static		unique_ptr<Navigator> Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);

private:
	XMFLOAT3	Compute_ViewRayDirection(XMMATRIX _ProjectionMat);

	NavigationResult	Compute_StaticMeshTriangle(XMVECTOR _RayOrigin, XMVECTOR _RayDirection, shared_ptr<GameObject> _CastedObject);
	VOID		Compute_SubMeshTriangle(XMVECTOR _RayOrigin, XMVECTOR _RayDirection, NavigationResult* Result, shared_ptr<StaticMesh> _StaticMesh, uint32_t _SubMeshIndex);
	VOID		Compute_SubMeshTriangle(XMVECTOR _RayOrigin, XMVECTOR _RayDirection, NavigationResult* Result, shared_ptr<InstanceMesh> _StaticMesh, uint32_t _SubMeshIndex);

	VOID		Convert_WorldCoordinate(NavigationResult* _Result, shared_ptr<GameObject> _CastedObject);


private:
	ComPtr<ID3D11Device>				GRPDEV				= { nullptr };
	ComPtr<ID3D11DeviceContext>			DEVCTX				= { nullptr };

	shared_ptr<Camera>					MainCamera			= { nullptr };
	shared_ptr<GameObject>				CopiedObject		= { nullptr };

	XMVECTOR							WorldRayOrigin		= {};
	XMVECTOR							WorldRayDirection	= {};

	XMVECTOR							LocalRayOrigin		= {};
	XMVECTOR							LocalRayDirection	= {};

	map<_float, shared_ptr<GameObject>>	CastedObjectList;

	list<shared_ptr<NavMeshCell>>		NavMeshCellList[static_cast<uint32_t>(NAVMESH_TYPE::NAVMESH_END)];
	shared_ptr<GameObject>				CastedObject		= { nullptr };

	_bool								Control_Lock		= { false };

	NAVMESH_TYPE						CurrentNavMeshType  = { NAVMESH_TYPE::NAVMESH_HORIZONTAL };

#ifdef _DEBUG
private:
	shared_ptr<Shader>	Component_HorizontalCellShader = { nullptr };
	shared_ptr<Shader>	Component_VerticalCellShader = { nullptr };
	shared_ptr<Shader>	Component_DiagonalCellShader = { nullptr };
	shared_ptr<Shader>	Component_BlockCellShader = { nullptr };
#endif
};
END