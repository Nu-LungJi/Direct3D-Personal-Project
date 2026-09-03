#include "RenderTargetManager.h"

RenderTargetManager::RenderTargetManager(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GRPDEV(_GRPDEV), DEVCTX(_DEVCTX) { }
RenderTargetManager::~RenderTargetManager() { }

HRESULT RenderTargetManager::Ready_RenderTargetManager() {
	uint32_t InitializeSize = static_cast<uint32_t>(RENDER_TARGET::RT_END);

	RenderTargetList.reserve(InitializeSize);
	RenderTargetList.resize(InitializeSize);

	return S_OK;
}

HRESULT RenderTargetManager::Generate_RenderTargetView(RENDER_TARGET _RTIndex, DXGI_FORMAT _Format, uint32_t _WindowResolutionX, uint32_t _WindowResolutionY, const XMFLOAT4& _ClearColor) {
	uint32_t TargetIndex = static_cast<uint32_t>(_RTIndex);

	if (RenderTargetList.size() < TargetIndex + 1 || nullptr != RenderTargetList[TargetIndex]) return E_FAIL;

	RenderTargetList[TargetIndex] = RenderTarget::Create(GRPDEV, DEVCTX, _Format, _WindowResolutionX, _WindowResolutionY, _ClearColor);
	if (nullptr == RenderTargetList[TargetIndex]) return E_FAIL;

	return S_OK;
}

HRESULT RenderTargetManager::Generate_MultiRenderTarget(const wstring& _MRTTag, RENDER_TARGET _RTIndex) {
	auto RT = RenderTargetList[static_cast<uint32_t>(_RTIndex)];

	auto RTList = Find_MultiRenderTarget(_MRTTag);
	if (nullptr == RTList) {
		list<shared_ptr<RenderTarget>> NewRenderTargetList;

		NewRenderTargetList.push_back(RT);

		MultiRenderTargetList.emplace(_MRTTag, NewRenderTargetList);
	}
	else 
		(*RTList).push_back(RT);
	
	return S_OK;
}

HRESULT RenderTargetManager::Begin_MultiRenderTarget(const wstring& _MRTTag) {

	list<shared_ptr<RenderTarget>>* MRTList = Find_MultiRenderTarget(_MRTTag);

	if (nullptr == MRTList) return E_FAIL;

	DEVCTX->OMGetRenderTargets(1, &BackBufferRTV, &BackBufferDSV);

	ComPtr<ID3D11RenderTargetView>	RTVList[MAX_RTV_COUNT] = {};
	uint32_t NumbRenderTarget = 0;

	for (auto& RT : *MRTList) {
		RT->Clear_RenderTargetView();
		RTVList[NumbRenderTarget++] = RT->Get_RenderTargetView();
	}

	DEVCTX->OMSetRenderTargets(NumbRenderTarget, RTVList[0].GetAddressOf(), BackBufferDSV.Get());

	return S_OK;
}

HRESULT RenderTargetManager::End_MultiRenderTarget() {
	ComPtr<ID3D11RenderTargetView>	RTList[MAX_RTV_COUNT] = { BackBufferRTV };

	DEVCTX->OMSetRenderTargets(1, RTList[0].GetAddressOf(), BackBufferDSV.Get());

	ID3D11ShaderResourceView* nullSRVs[16] = { nullptr };
	DEVCTX->PSSetShaderResources(0, 16, nullSRVs);

	return S_OK;
}

HRESULT RenderTargetManager::Bind_ShaderResourceView(uint32_t _TargetNumb, shared_ptr<class Shader> _Shader, const string& _ShaderVariableName) {
	shared_ptr<RenderTarget>	RT = RenderTargetList[_TargetNumb];
	if (nullptr == RT) return E_FAIL;

	return RT->Bind_ShaderResourceView(_Shader, _ShaderVariableName);
}
HRESULT RenderTargetManager::Copy_RenderTarget(uint32_t _TargetNumb, ComPtr<ID3D11Texture2D>& _Texture2D) {
	shared_ptr<RenderTarget>	RT = RenderTargetList[_TargetNumb];
	if (nullptr == RT) return E_FAIL;

	return RT->Copy_RenderTarget(_Texture2D);
}
list<shared_ptr<RenderTarget>>* RenderTargetManager::Find_MultiRenderTarget(const wstring& _MRTTag) {
	auto iter = MultiRenderTargetList.find(_MRTTag);
	return iter == MultiRenderTargetList.end() ? nullptr : &(iter->second);
}

#ifdef _DEBUG
HRESULT RenderTargetManager::Ready_DebugRender(uint32_t _TargetNumb, _float _PosX, _float _PosY, _float _Width, _float _Height) {
	shared_ptr<RenderTarget>	RT = RenderTargetList[_TargetNumb];
	if (nullptr == RT) return E_FAIL;

	return RT->Ready_DebugRender(_PosX, _PosY, _Width, _Height);
}
HRESULT RenderTargetManager::Render_DebugRender(const wstring& _MRTTag, shared_ptr<class Shader> _Shader, const string& _ShaderVariableName, shared_ptr<class RectBuffer> _RenderBuffer) {
	auto MRTList = Find_MultiRenderTarget(_MRTTag);
	if (nullptr == MRTList) return E_FAIL;

	for (auto& RT : *MRTList) {
		if (nullptr != RT) {
			if (RT->Render_DebugRender(_Shader, _ShaderVariableName, _RenderBuffer)) {
				MSG_BOX("Cannot Render RenderTarget.");
				return E_FAIL;
			}
		}
	}

	return S_OK;
}
#endif

unique_ptr<RenderTargetManager>	RenderTargetManager::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<RenderTargetManager>(new RenderTargetManager(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Ready_RenderTargetManager())) {
		MSG_BOX("Cannot Create RenderTargetManager");
		return nullptr;
	}
	return Instance;
}