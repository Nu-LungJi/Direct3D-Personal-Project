#pragma once
#include "Engine_Define.h"
#include "Light.h"

BEGIN(Engine)

class ENGINE_DLL LightManager {
private:
	LightManager(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	~LightManager() = default;

public:
	HRESULT		Generate_Light(LIGHT_TYPE _LTYPE, XMFLOAT4 _Diffuse, XMFLOAT4 _Ambient, XMFLOAT4 _Specular, XMFLOAT4 _Direction, XMFLOAT4 _Position, _float _Range);
	HRESULT		Render_LightManager(shared_ptr<class Shader> _Shader, shared_ptr<class RectBuffer> _RenderBuffer);

	HRESULT		Reset_LightManager();

public:
	static unique_ptr<LightManager>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);

private:
	ComPtr<ID3D11Device>			GRPDEV = { nullptr };
	ComPtr<ID3D11DeviceContext>		DEVCTX = { nullptr };

	list<shared_ptr<Light>>			LightList;
};
END