#include "Light.h"
#include "GameInstance.h"
#include "RectBuffer.h"

Light::Light(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GRPDEV(_GRPDEV), DEVCTX(_DEVCTX) {};

HRESULT Light::Initialize_Light() {


    return S_OK;
}
HRESULT Light::Set_LightOption(LIGHT_TYPE _LTYPE, XMFLOAT4 _Diffuse, XMFLOAT4 _Ambient, XMFLOAT4 _Specular, XMFLOAT4 _Direction, XMFLOAT4 _Position, _float _Range) {
    
    LightType   = _LTYPE;
    Diffuse     = _Diffuse;
    Ambient     = _Ambient;
    Specular    = _Specular;

    Direction   = _Direction;
    Position    = _Position;
    Range       = _Range;

    return S_OK;
}
HRESULT Light::Render_Light(shared_ptr<class Shader> _Shader, shared_ptr<class RectBuffer> _RenderBuffer) {

    uint32_t PassNumb = 0;

    if      (LightType == LIGHT_TYPE::POINT) {
        if (FAILED(_Shader->Bind_RawValue("g_vLightPos", &Position, sizeof(XMFLOAT4))))  return E_FAIL;
        if (FAILED(_Shader->Bind_RawValue("g_fLightRange", &Range, sizeof(_float))))     return E_FAIL;

        PassNumb = static_cast<uint32_t>(DEFERRED_TYPE::POINT);
    }
    else if (LightType == LIGHT_TYPE::DIRECTIONAL) {
        if (FAILED(_Shader->Bind_RawValue("g_vLightDir", &Direction, sizeof(XMFLOAT4)))) return E_FAIL;
        PassNumb = static_cast<uint32_t>(DEFERRED_TYPE::DIRECTIONAL);
    }
    else return E_FAIL;
    
    if (FAILED(_Shader->Bind_RawValue("g_vLightDiffuse", &Diffuse, sizeof(XMFLOAT4))))   return E_FAIL;
    if (FAILED(_Shader->Bind_RawValue("g_vLightAmbient", &Ambient, sizeof(XMFLOAT4))))   return E_FAIL;
    if (FAILED(_Shader->Bind_RawValue("g_vLightSpecular", &Specular, sizeof(XMFLOAT4)))) return E_FAIL;

    if (FAILED(_Shader->Shader_Begin(PassNumb))) return E_FAIL;
    if (FAILED(_RenderBuffer->Render_Buffer())) return E_FAIL;

    return S_OK;
}

shared_ptr<Light>	Light::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
    auto Instance = shared_ptr<Light>(new Light(_GRPDEV, _DEVCTX));
    if (FAILED(Instance->Initialize_Light())) {
        MSG_BOX("Cannot Create Light");
        return nullptr;
    }
    return Instance;
}