#pragma once
#include "Engine_Define.h"
#include "RenderTarget.h"
BEGIN(Engine)

#define	MAX_RTV_COUNT 8

class ENGINE_DLL RenderTargetManager {
private:
	RenderTargetManager(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	~RenderTargetManager();

public:
	HRESULT		Ready_RenderTargetManager();

	HRESULT		Generate_RenderTargetView(RENDER_TARGET _RTIndex, DXGI_FORMAT _Format, uint32_t _WindowResolutionX, uint32_t _WindowResolutionY, const XMFLOAT4& _ClearColor);
	HRESULT		Generate_MultiRenderTarget(const wstring& _MRTTag, RENDER_TARGET _RTIndex);
	
	HRESULT		Begin_MultiRenderTarget(const wstring& _MRTTag);
	HRESULT		End_MultiRenderTarget();

	HRESULT		Bind_ShaderResourceView(uint32_t _TargetNumb, shared_ptr<class Shader> _Shader, const string& _ShaderVariableName);
	HRESULT		Copy_RenderTarget(uint32_t _TargetNumb, ComPtr<ID3D11Texture2D>& _Texture2D);

	ComPtr<ID3D11RenderTargetView>	Get_RenderTargetView(CONST wstring& _MRTTag);
public:
	static unique_ptr<RenderTargetManager>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);

#ifdef _DEBUG
public:
	HRESULT		Ready_DebugRender(uint32_t _TargetNumb, _float _PosX, _float _PosY, _float _Width, _float _Height);
	HRESULT		Render_DebugRender(CONST wstring& _MRTTag, shared_ptr<class Shader> _Shader, const string& _ShaderVariableName, shared_ptr<class RectBuffer> _RenderBuffer);
#endif

private:
	list<shared_ptr<RenderTarget>>*	Find_MultiRenderTarget(const wstring& _MRTTag);

private:
	ComPtr<ID3D11Device>			GRPDEV			= { nullptr };
	ComPtr<ID3D11DeviceContext>		DEVCTX			= { nullptr };
	ComPtr<ID3D11RenderTargetView>	BackBufferRTV	= { nullptr };
	ComPtr<ID3D11DepthStencilView>	BackBufferDSV	= { nullptr };

private:
	vector<shared_ptr<RenderTarget>>			RenderTargetList;
	unordered_map<wstring, list<shared_ptr<RenderTarget>>>	MultiRenderTargetList;
};
END