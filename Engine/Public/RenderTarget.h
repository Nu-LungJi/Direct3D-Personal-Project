#pragma once
#include "Engine_Define.h"

BEGIN(Engine)
class ENGINE_DLL RenderTarget {
private:
	RenderTarget(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	~RenderTarget() = default;

public:
	HRESULT Initialize_RenderTarget(DXGI_FORMAT _Format, uint32_t _WindowResolutionX, uint32_t _WindowResolutionY, const XMFLOAT4& _ClearColor);

	ComPtr<ID3D11RenderTargetView>	Get_RenderTargetView() { return COMRTV; }

	VOID	Clear_RenderTargetView();

public:
	HRESULT		Bind_ShaderResourceView(shared_ptr<class Shader> _Shader, const string& _ShaderVariableName);
	HRESULT		Copy_RenderTarget(ComPtr<ID3D11Texture2D>& _Texture2D);

#ifdef  _DEBUG
public:
	HRESULT		Ready_DebugRender(_float _PosX, _float _PosY, _float _Width, _float _Height);
	HRESULT		Render_DebugRender(shared_ptr<class Shader> _Shader, const string& _ShaderVariableName, shared_ptr<class RectBuffer> _RenderBuffer);
#endif //  _DEBUG
	static shared_ptr<RenderTarget>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX, DXGI_FORMAT _Format, uint32_t _WindowResolutionX, uint32_t _WindowResolutionY, const XMFLOAT4& _ClearColor = { 0.f, 0.f, 0.f, 1.f });

private:
	ComPtr<ID3D11Device>				GRPDEV = { nullptr };
	ComPtr<ID3D11DeviceContext>			DEVCTX = { nullptr };

	ComPtr<ID3D11Texture2D>				COMTEX = { nullptr };
	ComPtr<ID3D11RenderTargetView>		COMRTV = { nullptr };
	ComPtr<ID3D11ShaderResourceView>	COMSRV = { nullptr };

	_float	ClearColor[4] = {};
#ifdef  _DEBUG
	XMFLOAT4X4		WorldMatrix = {};
#endif //  _DEBUG
};
END