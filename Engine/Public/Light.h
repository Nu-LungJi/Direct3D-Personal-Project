#pragma once
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL Light {
private:
	Light(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	~Light() = default;

public:
	HRESULT		Initialize_Light();
	HRESULT		Set_LightOption(LIGHT_TYPE _LTYPE, XMFLOAT4 _Diffuse, XMFLOAT4 _Ambient, XMFLOAT4 _Specular, XMFLOAT4 _Direction, XMFLOAT4 _Position, _float _Range);
	HRESULT		Render_Light(shared_ptr<class Shader> _Shader, shared_ptr<class RectBuffer> _RenderBuffer);

public:
	static shared_ptr<Light>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);

private:
	ComPtr<ID3D11Device>			GRPDEV = { nullptr };
	ComPtr<ID3D11DeviceContext>		DEVCTX = { nullptr };

	LIGHT_TYPE		LightType	= {		};
	XMFLOAT4		Diffuse		= {		};
	XMFLOAT4		Ambient		= {		};
	XMFLOAT4		Specular	= {		};

	XMFLOAT4		Direction	= {		};
	XMFLOAT4		Position	= {		};
	_float			Range		= { 0.f };
};
END
