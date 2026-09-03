#pragma once
#include "Engine_Define.h"
#include "CellBuffer.h"

BEGIN(Engine)
class ENGINE_DLL NavigationCell {
private:
	NavigationCell(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	~NavigationCell() = default;

public:
	HRESULT Initialize_NavigationCell(const XMFLOAT3* _CellVertex, int32_t _CellIndex);

	static shared_ptr<NavigationCell>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX, const XMFLOAT3* _CellVertex, int32_t _CellIndex);

private:
	ComPtr<ID3D11Device>			GRPDEV = { nullptr };
	ComPtr<ID3D11DeviceContext>		DEVCTX = { nullptr };

	int32_t				CellIndex = { -1 };
	XMFLOAT3			CellVertex[static_cast<uint32_t>(NAVMESH_VERTEX::VTX_END)] = {};
	XMFLOAT3			CellNormal[static_cast<uint32_t>(NAVMESH_LINE::LINE_END)]  = {};

#ifdef _DEBUG
public:
	HRESULT Render_NavigationCell();

private:
	unique_ptr<CellBuffer>		CellBuffer = { nullptr };
#endif
};
END