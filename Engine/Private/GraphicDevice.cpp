#include "GraphicDevice.h"
#include "GameInstance.h"
#include "DebugDraw.h"

GraphicDevice::GraphicDevice() : EngineOption{} {}
GraphicDevice::~GraphicDevice() {}

HRESULT GraphicDevice::Ready_GraphicDevice(CONST ENGINE_DESC& _DESC, ComPtr<ID3D11Device>& _GRPDEV, ComPtr<ID3D11DeviceContext>& _DEVCTX) {

	EngineOption = _DESC;

	ZeroMemory(&Viewport, sizeof(Viewport));

	if (FAILED(Create_DeviceAndSwapChain())	)	return E_FAIL;
	if (FAILED(Create_RenderTargetView())	)	return E_FAIL;
	if (FAILED(Create_DepthStencilView())	)	return E_FAIL;
	if (FAILED(Create_RasterizerState())	)	return E_FAIL;
	if (FAILED(Set_Viewport()))					return E_FAIL;

	if (FAILED(Initialize_Graphic())		)	return E_FAIL;

	_GRPDEV = GRPDEV;
	_DEVCTX = DEVCTX;

	return S_OK;
}
HRESULT GraphicDevice::Create_DeviceAndSwapChain() {

	DXGI_SWAP_CHAIN_DESC DESC = { 0 };
	ZeroMemory(&DESC, sizeof(DXGI_SWAP_CHAIN_DESC));
	{
		DESC.BufferDesc.Width					= static_cast<uint32_t>(EngineOption.WindowResolutionX);
		DESC.BufferDesc.Height					= static_cast<uint32_t>(EngineOption.WindowResolutionY);
		DESC.BufferDesc.RefreshRate.Numerator	= 60;
		DESC.BufferDesc.RefreshRate.Denominator = 1;
		DESC.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		DESC.BufferDesc.ScanlineOrdering		= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		DESC.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;

		DESC.SampleDesc.Count					= 1;
		DESC.SampleDesc.Quality					= 0;

		DESC.BufferUsage						= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		DESC.BufferCount						= 1;
		DESC.OutputWindow						= EngineOption.hWnd;
		DESC.Windowed = static_cast<BOOL>(EngineOption.WindowMode);
		DESC.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
	}

	D3D_FEATURE_LEVEL FeatureLevel = {};

	if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG, nullptr, 0, D3D11_SDK_VERSION,
		&DESC, SwapChain.GetAddressOf(), GRPDEV.GetAddressOf(), nullptr, DEVCTX.GetAddressOf())))	return E_FAIL;

	return S_OK;
}
HRESULT GraphicDevice::Create_RenderTargetView() {

	ComPtr<ID3D11Texture2D> BackBufferTexture = nullptr;

	if (FAILED(SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)BackBufferTexture.GetAddressOf())))		return E_FAIL;
	
	if (FAILED(GRPDEV->CreateRenderTargetView(BackBufferTexture.Get(), nullptr, RenderTargetView.GetAddressOf())))	return E_FAIL;

	return S_OK;
}
HRESULT GraphicDevice::Create_DepthStencilView() {
	// 거리에 따른 차폐관리
	// 원래의 깊이 버퍼와 동일하게 생성
	ComPtr<ID3D11Texture2D> DepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TexDESC = { 0 };
	ZeroMemory(&TexDESC, sizeof(TexDESC));

	TexDESC.Width		= static_cast<uint32_t>(EngineOption.WindowResolutionX);
	TexDESC.Height		= static_cast<uint32_t>(EngineOption.WindowResolutionY);
	TexDESC.MipLevels	= 1; // 2^n승의 텍스쳐를 미리 준비하는 것 - 잚 못 사용하면/잘 안 사용하면 메모리 낭비
	TexDESC.ArraySize	= 1;
	TexDESC.Format		= DXGI_FORMAT_D24_UNORM_S8_UINT;

	TexDESC.SampleDesc.Count	= 1;
	TexDESC.SampleDesc.Quality	= 0;

	TexDESC.Usage			= D3D11_USAGE_DEFAULT;
	TexDESC.BindFlags		= D3D11_BIND_DEPTH_STENCIL;
	TexDESC.CPUAccessFlags	= 0;
	TexDESC.MiscFlags		= 0;

	if (FAILED(GRPDEV->CreateTexture2D(&TexDESC, nullptr, DepthStencilTexture.GetAddressOf())))							return E_FAIL;

	if (FAILED(GRPDEV->CreateDepthStencilView(DepthStencilTexture.Get(), nullptr, DepthStencilView.GetAddressOf())))	return E_FAIL;
	
	DEVCTX->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());

	return S_OK;
}
HRESULT GraphicDevice::Create_RasterizerState() {

	D3D11_RASTERIZER_DESC DESC = {};

	DESC.AntialiasedLineEnable = false;
	DESC.CullMode = D3D11_CULL_NONE;
	DESC.DepthBias = 0;
	DESC.DepthBiasClamp = 0.0f;
	DESC.DepthClipEnable = true;
	DESC.FillMode = D3D11_FILL_SOLID;
	DESC.FrontCounterClockwise = false;
	DESC.MultisampleEnable = false;
	DESC.ScissorEnable = false;
	DESC.SlopeScaledDepthBias = 0.0f;

	GRPDEV->CreateRasterizerState(&DESC, &RasterizerState);
	DEVCTX->RSSetState(RasterizerState.Get());
	
	return S_OK;
}
HRESULT GraphicDevice::Set_Viewport() {

	Viewport.TopLeftX	= 0.f;
	Viewport.TopLeftY	= 0.f;
	Viewport.Width		= EngineOption.WindowResolutionX;
	Viewport.Height		= EngineOption.WindowResolutionY;
	Viewport.MinDepth	= 0.f;
	Viewport.MaxDepth	= 1.f;

	DEVCTX->RSSetViewports(1, &Viewport);

	return S_OK;
}
HRESULT GraphicDevice::Clear_RenderTargetView() {
	if (DEVCTX == nullptr) return E_FAIL;
	_float Color[4] = { 0.f, 0.f, 1.f, 1.f };
	// 첫 번째 인자가 렌더 타겟 뷰의 갯수, 최대 8개까지 가능하다.(DX11기준)
	DEVCTX->ClearRenderTargetView(RenderTargetView.Get(), Color);

	return S_OK;
}
HRESULT GraphicDevice::Clear_DepthStencilView() {
	if (DEVCTX == nullptr) return E_FAIL;

	DEVCTX->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}
HRESULT GraphicDevice::Release_GRPDEV() {
	DEVCTX->ClearState();
	DEVCTX->Flush();

	RenderTargetView.Reset();
	SwapChain.Reset(); 
	
	DepthStencilView.Reset();

	RasterizerState.Reset();
	SamplerState.Reset();

	DEVCTX.Reset();
	GRPDEV.Reset();

	return S_OK;
}
HRESULT GraphicDevice::Initialize_Graphic() {

	D3D11_SAMPLER_DESC SDESC = {  };
	ZeroMemory(&SDESC, sizeof(SDESC));
	SDESC.Filter = D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
	SDESC.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SDESC.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SDESC.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SDESC.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SDESC.MinLOD = 0;
	SDESC.MaxLOD = D3D11_FLOAT32_MAX;

	if (FAILED(GRPDEV->CreateSamplerState(&SDESC, SamplerState.GetAddressOf()))) {
		MSG_BOX("Cannot Create SamplerState.");
		return E_FAIL;
	}
		
	return S_OK;
}
VOID	GraphicDevice::Render_Device() {
	DEVCTX->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());
	// IMGUI 화면 날아가는 거 방지용
	SwapChain->Present(0, 0);
}
unique_ptr<GraphicDevice> GraphicDevice::Create(ComPtr<ID3D11Device>& _GRPDEV, ComPtr<ID3D11DeviceContext>& _DEVCTX, const ENGINE_DESC& _DESC) {
	auto Instance = unique_ptr<GraphicDevice>(new GraphicDevice());
	if (FAILED(Instance->Ready_GraphicDevice(_DESC, _GRPDEV, _DEVCTX))) {
		MSG_BOX("Cannot Create GraphicDevice.");
		return nullptr;
	}
	return Instance;
}