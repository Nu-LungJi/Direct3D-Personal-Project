#pragma once

#include "Engine_Define.h"
#include "GameObject.h"
#include "Camera.h"

BEGIN(Engine)

class ENGINE_DLL GraphicDevice {
private:
	GraphicDevice();
public:
	virtual ~GraphicDevice();
public:
	HRESULT Ready_GraphicDevice(CONST ENGINE_DESC& _DESC, ComPtr<ID3D11Device>& _GRPDEV, ComPtr<ID3D11DeviceContext>& _DEVCTX);

	HRESULT	Create_DeviceAndSwapChain();
	HRESULT	Create_RenderTargetView();
	HRESULT Create_DepthStencilView();
	HRESULT Create_RasterizerState();
	HRESULT	Set_Viewport();

	HRESULT Clear_RenderTargetView();
	HRESULT Clear_DepthStencilView();

	HRESULT Release_GRPDEV();

	HRESULT	Initialize_Graphic();

public:
	ComPtr<ID3D11Device>			Get_GraphicDevice()		{ return GRPDEV; }
	ComPtr<ID3D11DeviceContext>		Get_DeviceContext()		{ return DEVCTX; }

	ComPtr<ID3D11DepthStencilView>	Get_DepthStencilView()	{ return DepthStencilView;}
	ComPtr<ID3D11RasterizerState>	Get_RasterizerState()	{ return RasterizerState; }

	ComPtr<IDXGISwapChain>			Get_SwapChain()			{ return SwapChain; }
	ComPtr<ID3D11RenderTargetView>	Get_RenderTargetView()	{ return RenderTargetView; }

	VOID	Render_Device();
	
	CONST ENGINE_DESC				Get_EngineDescription() { return EngineOption; }

	static unique_ptr<GraphicDevice>	Create(ComPtr<ID3D11Device>& _GRPDEV, ComPtr<ID3D11DeviceContext>& _DEVCTX, const ENGINE_DESC& _DESC);

private:
	ComPtr<ID3D11Device>			GRPDEV					= { nullptr };
	ComPtr<ID3D11DeviceContext>		DEVCTX					= { nullptr };

	ComPtr<IDXGISwapChain>			SwapChain				= { nullptr };

	ComPtr<ID3D11RenderTargetView>	RenderTargetView		= { nullptr };
	ComPtr<ID3D11DepthStencilView>	DepthStencilView		= { nullptr };
	ComPtr<ID3D11RasterizerState>	RasterizerState			= { nullptr };
	ComPtr<ID3D11SamplerState>		SamplerState			= { nullptr };

	D3D11_VIEWPORT					Viewport		= {};

	ENGINE_DESC						EngineOption	= {};
};

END