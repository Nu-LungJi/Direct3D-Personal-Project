#include "RenderTarget.h"
#include "GameInstance.h"

RenderTarget::RenderTarget(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GRPDEV(_GRPDEV), DEVCTX(_DEVCTX) {}

HRESULT RenderTarget::Initialize_RenderTarget(DXGI_FORMAT _Format, uint32_t _WindowResolutionX, uint32_t _WindowResolutionY, const XMFLOAT4& _ClearColor) {

	ENGINE_DESC EDESC = GameInstance::GetInstance().Get_EngineOption();

	D3D11_TEXTURE2D_DESC DESC = {};

	DESC.Width				= _WindowResolutionX;
	DESC.Height				= _WindowResolutionY;
	DESC.MipLevels			= 1;
	DESC.ArraySize			= 1;
	DESC.Format				= _Format;

	DESC.SampleDesc.Count	= 1;
	DESC.SampleDesc.Quality = 0;

	DESC.Usage				= D3D11_USAGE_DEFAULT;
	DESC.BindFlags			= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	DESC.CPUAccessFlags		= 0;
	DESC.MiscFlags			= 0;

	if (FAILED(GRPDEV->CreateTexture2D(&DESC, nullptr, COMTEX.GetAddressOf()))) {
		MSG_BOX("Cannot Create Texture2D");
		return E_FAIL;
	}
	if (FAILED(GRPDEV->CreateRenderTargetView(COMTEX.Get(), nullptr, COMRTV.GetAddressOf()))) {
		MSG_BOX("Cannot Create RenderTargetView");
		return E_FAIL;
	}
	if (FAILED(GRPDEV->CreateShaderResourceView(COMTEX.Get(), nullptr, COMSRV.GetAddressOf()))) {
		MSG_BOX("Cannot Create ShaderResourceView");
		return E_FAIL;
	}

	ClearColor[0] = _ClearColor.x;
	ClearColor[1] = _ClearColor.y;
	ClearColor[2] = _ClearColor.z;
	ClearColor[3] = _ClearColor.w;

    return S_OK;
}
VOID	 RenderTarget::Clear_RenderTargetView() {
	DEVCTX->ClearRenderTargetView(COMRTV.Get(), ClearColor);
}
HRESULT RenderTarget::Bind_ShaderResourceView(shared_ptr<class Shader> _Shader, const string& _ShaderVariableName) {
	return _Shader->Bind_ShaderResourceView(_ShaderVariableName, COMSRV);
}
HRESULT RenderTarget::Copy_RenderTarget(ComPtr<ID3D11Texture2D>& _Texture2D) {
	DEVCTX->CopyResource(_Texture2D.Get(), COMTEX.Get());
	return S_OK;
}
#ifdef  _DEBUG
HRESULT RenderTarget::Ready_DebugRender(_float _PosX, _float _PosY, _float _Width, _float _Height) {
	
	ENGINE_DESC EngineOption = GameInstance::GetInstance().Get_EngineOption();
	
	XMStoreFloat4x4(&WorldMatrix, XMMatrixScaling(_Width, _Height, 1.f) * XMMatrixTranslation(_PosX - EngineOption.WindowResolutionX * 0.5f, EngineOption.WindowResolutionY * 0.5f -_PosY, 0.f));

	return S_OK;
}
HRESULT RenderTarget::Render_DebugRender(shared_ptr<class Shader> _Shader, const string& _ShaderVariableName, shared_ptr<class RectBuffer> _RenderBuffer) {
	
	if (FAILED(_Shader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))				return E_FAIL;

	if (FAILED(_Shader->Bind_ShaderResourceView(_ShaderVariableName, COMSRV)))		return E_FAIL;

	if (FAILED(_Shader->Shader_Begin(static_cast<uint32_t>(DEFERRED_TYPE::DEBUG))))	return E_FAIL;

	if (FAILED(_RenderBuffer->Render_Buffer()))	return E_FAIL;

	return S_OK;
}
#endif //  _DEBUG
shared_ptr<RenderTarget> RenderTarget::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX, DXGI_FORMAT _Format, uint32_t _WindowResolutionX, uint32_t _WindowResolutionY, const XMFLOAT4& _ClearColor) {
    auto Instance = shared_ptr<RenderTarget>(new RenderTarget(_GRPDEV, _DEVCTX));
    if (FAILED(Instance->Initialize_RenderTarget(_Format, _WindowResolutionX, _WindowResolutionY, _ClearColor)))
        MSG_BOX("Cannot Create RenderTarget.");

    return Instance;
}
