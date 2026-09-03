#pragma once
#include "Engine_Define.h"
#include "CellBuffer.h"

BEGIN(Engine)
class ENGINE_DLL NavMeshCell {
private:
	NavMeshCell(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	~NavMeshCell() = default;

public:
	HRESULT Initialize_NavigationCell(const XMFLOAT3* _CellVertex, int32_t _CellIndex, NAVMESH_TYPE _NTYPE);

	static shared_ptr<NavMeshCell>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX, const XMFLOAT3* _CellVertex, int32_t _CellIndex, NAVMESH_TYPE _NTYPE);

public:
	_bool				Compare_EqualVertex(XMVECTOR _SRC, XMVECTOR _DST);
	_float				Compute_CellHeight(XMVECTOR _CurrentPosition);
	
public:
	_bool				Check_ObjectOnCell(XMVECTOR _CurrentPosition, int32_t* _AdjacentCellIndex);
	_bool				Check_ObjectOnCell_XZ(XMVECTOR _CurrentPosition);

public:
	XMVECTOR			Get_CellVertex(NAVMESH_VERTEX _CVTX)							{ return XMLoadFloat3(&CellVertex[static_cast<uint32_t>(_CVTX)]);	 }
	XMFLOAT3			Get_RawCellVertex(NAVMESH_VERTEX _CVTX)						{ return CellVertex[static_cast<uint32_t>(_CVTX)];					 }
	int32_t*			Get_AdjacentCell_IndexList()								{ return AdjacentCell_IndexList;									 }
	
	VOID				Set_AdjacentCellIndex(NAVMESH_LINE _CLIN, int32_t _CellIndex)	{ AdjacentCell_IndexList[static_cast<uint32_t>(_CLIN)] = _CellIndex; }
	int32_t				Get_AdjacentCellIndex(NAVMESH_LINE _CLIN)						{ return AdjacentCell_IndexList[static_cast<uint32_t>(_CLIN)];		 }

	int32_t				Get_CellIndex()												{ return CellIndex; }

	VOID				Set_UniqueVertexIndex(uint32_t _VIDX, int32_t _UIDX)		{ UniqueVertex_IndexList[_VIDX] = _UIDX; }
	int32_t				Get_UniqueVertexIndex(uint32_t _VIDX)						{ return UniqueVertex_IndexList[_VIDX];  }

	VOID				Set_NavMeshType(NAVMESH_TYPE _NTYPE)						{ NavMeshType = _NTYPE; }
	NAVMESH_TYPE		Get_NavMeshType()											{ return NavMeshType;   }

private:
	ComPtr<ID3D11Device>			GRPDEV = { nullptr };
	ComPtr<ID3D11DeviceContext>		DEVCTX = { nullptr };

	int32_t				CellIndex = { -1 };
	XMFLOAT3			CellVertex[static_cast<uint32_t>(NAVMESH_VERTEX::VTX_END)] = {};
	XMFLOAT3			CellNormal[static_cast<uint32_t>(NAVMESH_LINE::LINE_END)]  = {};

	int32_t				AdjacentCell_IndexList[static_cast<uint32_t>(NAVMESH_LINE::LINE_END)];
	XMFLOAT4			CellPlane = {};

	int32_t				UniqueVertex_IndexList[static_cast<uint32_t>(NAVMESH_LINE::LINE_END)];

	NAVMESH_TYPE		NavMeshType;

#ifdef _DEBUG
public:
	HRESULT Render_NavigationCell();

private:
	unique_ptr<CellBuffer>		CellBuffer = { nullptr };
#endif
};
END