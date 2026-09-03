#pragma once
#include "Engine_Define.h"

BEGIN(Engine)
class ENGINE_DLL Utility {
private:
	Utility(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	~Utility() = default;

public:
	HRESULT		Initialize_Utility();

public:

	XMFLOAT2			Convert_WorldPositionToScreen(XMVECTOR _WorldPosition);

public:
	static	unique_ptr<Utility> Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);

private:
	ComPtr<ID3D11Device>				GRPDEV = { nullptr };
	ComPtr<ID3D11DeviceContext>			DEVCTX = { nullptr };
};
END