#include "LightManager.h"

LightManager::LightManager(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GRPDEV(_GRPDEV), DEVCTX(_DEVCTX) {}

HRESULT LightManager::Generate_Light(LIGHT_TYPE _LTYPE, XMFLOAT4 _Diffuse, XMFLOAT4 _Ambient, XMFLOAT4 _Specular, XMFLOAT4 _Direction, XMFLOAT4 _Position, _float _Range) {
	shared_ptr<Light> LightObject = Light::Create(GRPDEV, DEVCTX);

	LightObject->Set_LightOption(_LTYPE, _Diffuse, _Ambient, _Specular, _Direction, _Position, _Range);

	LightList.push_back(LightObject);

	return S_OK;
}

HRESULT LightManager::Render_LightManager(shared_ptr<class Shader> _Shader, shared_ptr<class RectBuffer> _RenderBuffer) {

	for (auto& LOBJ : LightList) {
		if (LOBJ != nullptr)	LOBJ->Render_Light(_Shader, _RenderBuffer);
	}

	return S_OK;
}
HRESULT LightManager::Reset_LightManager() {
	for (auto& LOBJ : LightList) 
		LOBJ.reset();
	LightList.clear();
	return S_OK;
}
unique_ptr<LightManager>	LightManager::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	return unique_ptr<LightManager>(new LightManager(_GRPDEV, _DEVCTX));
}
