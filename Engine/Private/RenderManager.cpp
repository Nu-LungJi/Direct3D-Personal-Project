#include "RenderManager.h"
#include "GameInstance.h"
#include "RectBuffer.h"

RenderManager::RenderManager(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX)	: GRPDEV(_GRPDEV), DEVCTX(_DEVCTX){	}
RenderManager::~RenderManager()		{	}

HRESULT RenderManager::Ready_RenderManager() {

	ENGINE_DESC EngineOption = GameInstance::GetInstance().Get_EngineOption();

	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Generate_RenderTargetView(RENDER_TARGET::DIFFUSE, DXGI_FORMAT_R8G8B8A8_UNORM, 
		static_cast<uint32_t>(EngineOption.WindowResolutionX), static_cast<uint32_t>(EngineOption.WindowResolutionY), XMFLOAT4{0.f, 0.f, 0.f, 0.f}))) {
		MSG_BOX("Cannot Generate RenderTargetView : DIFFUSE");
		return E_FAIL;
	}
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Generate_RenderTargetView(RENDER_TARGET::NORMAL, DXGI_FORMAT_R16G16B16A16_UNORM,
		static_cast<uint32_t>(EngineOption.WindowResolutionX), static_cast<uint32_t>(EngineOption.WindowResolutionY), XMFLOAT4{ 0.f, 0.f, 0.f, 1.f }))) {
		MSG_BOX("Cannot Generate RenderTargetView : NORMAL");
		return E_FAIL;
	}
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Generate_RenderTargetView(RENDER_TARGET::SHADE, DXGI_FORMAT_R16G16B16A16_FLOAT,
		static_cast<uint32_t>(EngineOption.WindowResolutionX), static_cast<uint32_t>(EngineOption.WindowResolutionY), XMFLOAT4{ 0.f, 0.f, 0.f, 1.f }))) {
		MSG_BOX("Cannot Generate RenderTargetView : SHADE");
		return E_FAIL;
	}
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Generate_RenderTargetView(RENDER_TARGET::SPECULAR, DXGI_FORMAT_R16G16B16A16_UNORM,
		static_cast<uint32_t>(EngineOption.WindowResolutionX), static_cast<uint32_t>(EngineOption.WindowResolutionY), XMFLOAT4{ 0.f, 0.f, 0.f, 1.f }))) {
		MSG_BOX("Cannot Generate RenderTargetView : SPECULAR");
		return E_FAIL;
	}
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Generate_RenderTargetView(RENDER_TARGET::DEPTH, DXGI_FORMAT_R32G32B32A32_FLOAT,
		static_cast<uint32_t>(EngineOption.WindowResolutionX), static_cast<uint32_t>(EngineOption.WindowResolutionY), XMFLOAT4{ 0.f, 0.f, 0.f, 1.f }))) {
		MSG_BOX("Cannot Generate RenderTargetView : DEPTH");
		return E_FAIL;
	}
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Generate_RenderTargetView(RENDER_TARGET::BRIGHTPASS, DXGI_FORMAT_R16G16B16A16_FLOAT,
		static_cast<uint32_t>(EngineOption.WindowResolutionX) / 2, static_cast<uint32_t>(EngineOption.WindowResolutionY) / 2, XMFLOAT4{ 0.f, 0.f, 0.f, 0.f }))) {
		MSG_BOX("Cannot Generate RenderTargetView : BRIGHTPASS");
		return E_FAIL;
	}
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Generate_RenderTargetView(RENDER_TARGET::VERTICALBLUR, DXGI_FORMAT_R16G16B16A16_FLOAT,
		static_cast<uint32_t>(EngineOption.WindowResolutionX) / 2, static_cast<uint32_t>(EngineOption.WindowResolutionY) / 2, XMFLOAT4{ 0.f, 0.f, 0.f, 1.f }))) {
		MSG_BOX("Cannot Generate RenderTargetView : VBLUR");
		return E_FAIL;
	}
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Generate_RenderTargetView(RENDER_TARGET::HORIZONTALBLUR, DXGI_FORMAT_R16G16B16A16_FLOAT,
		static_cast<uint32_t>(EngineOption.WindowResolutionX) / 2, static_cast<uint32_t>(EngineOption.WindowResolutionY) / 2, XMFLOAT4{ 0.f, 0.f, 0.f, 1.f }))) {
		MSG_BOX("Cannot Generate RenderTargetView : HBLUR");
		return E_FAIL;
	}
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Generate_RenderTargetView(RENDER_TARGET::COMBINDED, DXGI_FORMAT_R16G16B16A16_FLOAT,
		static_cast<uint32_t>(EngineOption.WindowResolutionX), static_cast<uint32_t>(EngineOption.WindowResolutionY), XMFLOAT4{ 0.f, 0.f, 0.f, 0.f }))) {
		MSG_BOX("Cannot Generate RenderTargetView : Combined");
		return E_FAIL;
	}
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Generate_RenderTargetView(RENDER_TARGET::NONLIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT,
		static_cast<uint32_t>(EngineOption.WindowResolutionX), static_cast<uint32_t>(EngineOption.WindowResolutionY), XMFLOAT4{ 0.f, 0.f, 0.f, 0.f }))) {
		MSG_BOX("Cannot Generate RenderTargetView : NONLIGHT");
		return E_FAIL;
	}

	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Generate_RenderTargetView(RENDER_TARGET::SHADOW, DXGI_FORMAT_R32G32B32A32_FLOAT, 8192, 4608, XMFLOAT4{ 1.0f, 1.f, 1.f, 1.f }))) {
		MSG_BOX("Cannot Generate RenderTargetView : Shadow");
		return E_FAIL;
	}

	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Generate_MultiRenderTarget(L"MRT_GameObject", RENDER_TARGET::DIFFUSE))) {
		MSG_BOX("Cannot Generate MultiRenderTargetView : MRT_GameObject<DIFFUSE>");
		return E_FAIL;
	}
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Generate_MultiRenderTarget(L"MRT_GameObject", RENDER_TARGET::NORMAL))) {
		MSG_BOX("Cannot Generate MultiRenderTargetView : MRT_GameObject<NORMAL>");
		return E_FAIL;
	}
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Generate_MultiRenderTarget(L"MRT_GameObject", RENDER_TARGET::DEPTH))) {
		MSG_BOX("Cannot Generate MultiRenderTargetView : MRT_GameObject<DEPTH>");
		return E_FAIL;
	}
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Generate_MultiRenderTarget(L"MRT_LightAccumulation", RENDER_TARGET::SHADE))) {
		MSG_BOX("Cannot Generate MultiRenderTargetView : MRT_LightAccumulation<SHADE>");
		return E_FAIL;
	}
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Generate_MultiRenderTarget(L"MRT_LightAccumulation", RENDER_TARGET::SPECULAR))) {
		MSG_BOX("Cannot Generate MultiRenderTargetView : MRT_LightAccumulation<SPECULAR>");
		return E_FAIL;
	}
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Generate_MultiRenderTarget(L"MRT_NonLight", RENDER_TARGET::NONLIGHT))) {
		MSG_BOX("Cannot Generate MultiRenderTargetView : MRT_NonLight<NONLIGHT>");
		return E_FAIL;
	}
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Generate_MultiRenderTarget(L"MRT_Shadow", RENDER_TARGET::SHADOW))) {
		MSG_BOX("Cannot Generate MultiRenderTargetView : MRT_LightAccumulation<SHADOW>");
		return E_FAIL;
	}
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Generate_MultiRenderTarget(L"MRT_BrightPass", RENDER_TARGET::BRIGHTPASS))) {
		MSG_BOX("Cannot Generate MultiRenderTargetView : MRT_BrightPass<BRIGHTPASS>");
		return E_FAIL;
	}
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Generate_MultiRenderTarget(L"MRT_VerticalBlur", RENDER_TARGET::VERTICALBLUR))) {
		MSG_BOX("Cannot Generate MultiRenderTargetView : MRT_VerticalBlur<VERTICALBLUR>");
		return E_FAIL;
	}
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Generate_MultiRenderTarget(L"MRT_HorizontalBlur", RENDER_TARGET::HORIZONTALBLUR))) {
		MSG_BOX("Cannot Generate MultiRenderTargetView : MRT_HorizontalBlur<HORIZONTALBLUR>");
		return E_FAIL;
	}
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Generate_MultiRenderTarget(L"MRT_Combined", RENDER_TARGET::COMBINDED))) {
		MSG_BOX("Cannot Generate MultiRenderTargetView : MRT_Combined<COMBINDED>");
		return E_FAIL;
	}
	Component_RectBuffer	= RectBuffer::Create(GRPDEV, DEVCTX);
	if (nullptr == Component_RectBuffer) return E_FAIL;
	
	Component_Shader		= Shader::Create(GRPDEV, DEVCTX, L"../../Shader_Deferred.hlsl", VTXTEX::Elements, VTXTEX::ElementsCount);
	if (nullptr == Component_Shader) return E_FAIL;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixScaling(EngineOption.WindowResolutionX, EngineOption.WindowResolutionY, 1.f));
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixOrthographicLH(EngineOption.WindowResolutionX, EngineOption.WindowResolutionY, 0.f, 1.f));

	D3D11_BUFFER_DESC DESC {};

	DESC.Usage					= D3D11_USAGE_DYNAMIC;
	DESC.ByteWidth				= sizeof(XMFLOAT4X4) * 10000;
	DESC.StructureByteStride	= sizeof(XMFLOAT4X4);
	DESC.BindFlags				= D3D11_BIND_VERTEX_BUFFER;
	DESC.CPUAccessFlags			= D3D11_CPU_ACCESS_WRITE;
	DESC.MiscFlags				= 0;

	if (FAILED(GRPDEV->CreateBuffer(&DESC, nullptr, GlobalInstanceBuffer.GetAddressOf()))) return E_FAIL;

#ifdef _DEBUG
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Ready_DebugRender(static_cast<uint32_t>(RENDER_TARGET::DIFFUSE) , 160.f, 90.f, 320.f, 180.f)))	return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Ready_DebugRender(static_cast<uint32_t>(RENDER_TARGET::NORMAL)  , 160.f, 270.f, 320.f, 180.f)))	return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Ready_DebugRender(static_cast<uint32_t>(RENDER_TARGET::SHADE)	  , 480.f, 90.f, 320.f, 180.f)))	return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Ready_DebugRender(static_cast<uint32_t>(RENDER_TARGET::SPECULAR), 480.f, 270.f, 320.f, 180.f)))	return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Ready_DebugRender(static_cast<uint32_t>(RENDER_TARGET::BRIGHTPASS), 160.f, 450.f, 320.f, 180.f)))	return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Ready_DebugRender(static_cast<uint32_t>(RENDER_TARGET::COMBINDED), 480.f, 450.f, 320.f, 180.f)))	return E_FAIL;
#endif // _DEBUG
	Create_DepthStencilTexture();
	return S_OK;
}

HRESULT RenderManager::Render_ByGroup() {
	if (FAILED(Render_PriorityGroup()))		return E_FAIL;

	if (FAILED(Render_NonBlend()))			return E_FAIL;

	if (FAILED(Render_Light()))				return E_FAIL;

	if (FAILED(Render_NonLight()))			return E_FAIL;

	if (FAILED(Render_AlphaBlend()))		return E_FAIL;

	if (FAILED(Render_Effect()))			return E_FAIL;

	if (FAILED(Render_Combined()))			return E_FAIL;

	if (FAILED(Render_PostProcess()))		return E_FAIL;

	if (FAILED(Render_UserInterface()))		return E_FAIL;

#ifdef _DEBUG
	if (KEY_DOWN(DIK_F3)) { DeferredRenderingFlag = DeferredRenderingFlag ? false : true; };
	if (DeferredRenderingFlag && FAILED(Render_RenderTargetView()))		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

HRESULT RenderManager::Render_PriorityGroup() {
	// 깊이 기록과 비교를 꺼서 모든 오브젝트를 뒤에서 덮는다.
	for (auto& OBJ : RenderObjectList[(uint32_t)RENDER_TYPE::RENDER_PRIORITY]) {
		if (OBJ == nullptr)	continue;
		OBJ->Render();
	}
	RenderObjectList[(uint32_t)RENDER_TYPE::RENDER_PRIORITY].clear();
	return S_OK;
}
HRESULT RenderManager::Render_AlphaBlend() {
	// 색을 섞는다. 반투명한 픽셀을 그린다는 표현은 맞지않고, 이미 그렸던 픽셀과 색을 섞는다는 표현이 맞다.
	for (auto& OBJ : RenderObjectList[(uint32_t)RENDER_TYPE::RENDER_BLEND]) {
		if (OBJ == nullptr)	continue;
		OBJ->Render();
	}
	RenderObjectList[(uint32_t)RENDER_TYPE::RENDER_BLEND].clear();
	return S_OK;
}
HRESULT RenderManager::Render_NonBlend() {
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Begin_MultiRenderTarget(L"MRT_GameObject"))) return E_FAIL;

	for (auto& OBJ : RenderObjectList[(uint32_t)RENDER_TYPE::RENDER_NONBLEND]) {
		if (OBJ == nullptr)	continue;
		OBJ->Render();
	}
	if (FAILED(Render_InstancedObject()))	return E_FAIL;
	for (auto& OBJ : RenderObjectList[(uint32_t)RENDER_TYPE::RENDER_NONBLEND2]) {
		if (OBJ == nullptr)	continue;
		OBJ->Render();
	}
	RenderObjectList[(uint32_t)RENDER_TYPE::RENDER_NONBLEND].clear();
	RenderObjectList[(uint32_t)RENDER_TYPE::RENDER_NONBLEND2].clear();
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->End_MultiRenderTarget())) return E_FAIL;

	return S_OK;
}
HRESULT RenderManager::Render_Light() {

	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Begin_MultiRenderTarget(L"MRT_LightAccumulation"))) return E_FAIL;

	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Bind_ShaderResourceView(static_cast<uint32_t>(RENDER_TARGET::NORMAL), Component_Shader, "g_NormalTexture"))) return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Bind_ShaderResourceView(static_cast<uint32_t>(RENDER_TARGET::DEPTH), Component_Shader, "g_DepthTexture"))) return E_FAIL;
	
	if (FAILED(Component_Shader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))	return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))		return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))		return E_FAIL;

	if (FAILED(Component_Shader->Bind_Matrix("g_ViewMatrixInverse", GameInstance::GetInstance().Get_MainCamera()->Get_RawInvViewMatrix())))		return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("g_ProjMatrixInverse", GameInstance::GetInstance().Get_MainCamera()->Get_RawInvProjMatrix())))		return E_FAIL;

	if (FAILED(Component_Shader->Bind_RawValue("g_vCamPosition", GameInstance::GetInstance().Get_MainCamera()->Get_RawCameraPosition(), sizeof(XMFLOAT4))))		return E_FAIL;

	if (FAILED(Component_RectBuffer->Bind_Resources())) return E_FAIL;

	if (FAILED(GameInstance::GetInstance().Get_LightManager()->Render_LightManager(Component_Shader, Component_RectBuffer))) return E_FAIL;

	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->End_MultiRenderTarget())) return E_FAIL;

	return S_OK;
}
HRESULT RenderManager::Render_Combined() {
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Begin_MultiRenderTarget(L"MRT_Combined"))) return E_FAIL;

	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Bind_ShaderResourceView(static_cast<uint32_t>(RENDER_TARGET::DIFFUSE), Component_Shader, "g_DiffuseTexture")))	return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Bind_ShaderResourceView(static_cast<uint32_t>(RENDER_TARGET::SHADE), Component_Shader, "g_ShadeTexture")))		return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Bind_ShaderResourceView(static_cast<uint32_t>(RENDER_TARGET::SPECULAR), Component_Shader, "g_SpecularTexture")))	return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Bind_ShaderResourceView(static_cast<uint32_t>(RENDER_TARGET::NONLIGHT), Component_Shader, "g_NonLightTexture")))	return E_FAIL;

	if (FAILED(Component_Shader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))	return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))		return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))		return E_FAIL;

	if (FAILED(Component_Shader->Shader_Begin(static_cast<uint32_t>(DEFERRED_TYPE::COMBINED)))) return E_FAIL;

	if (FAILED(Component_RectBuffer->Bind_Resources()))	return E_FAIL;

	if (FAILED(Component_RectBuffer->Render_Buffer()))	return E_FAIL;

	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->End_MultiRenderTarget())) return E_FAIL;

	return S_OK;
}
HRESULT RenderManager::Render_NonLight() {
	for (auto& OBJ : RenderObjectList[(uint32_t)RENDER_TYPE::RENDER_NONLIGHT]) {
		if (OBJ == nullptr)	continue;
		OBJ->Render();
	}
	RenderObjectList[(uint32_t)RENDER_TYPE::RENDER_NONLIGHT].clear();

	return S_OK;
}
HRESULT RenderManager::Render_Effect() {
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Begin_MultiRenderTarget(L"MRT_NonLight"))) return E_FAIL;

	GameInstance::GetInstance().Get_EffectManager()->Render_EffectManager();
	for (auto& OBJ : RenderObjectList[(uint32_t)RENDER_TYPE::RENDER_EFFECT]) {
		if (OBJ == nullptr)	continue;
		OBJ->Render();
	}
	RenderObjectList[(uint32_t)RENDER_TYPE::RENDER_EFFECT].clear();

	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->End_MultiRenderTarget())) return E_FAIL;

	return S_OK;
}
HRESULT RenderManager::Render_UserInterface() {
	for (auto& OBJ : RenderObjectList[(uint32_t)RENDER_TYPE::RENDER_UI]) {
		if (OBJ == nullptr)	continue;
		OBJ->Render();
	}
	RenderObjectList[(uint32_t)RENDER_TYPE::RENDER_UI].clear();
	return S_OK;
}

HRESULT RenderManager::Render_PostProcess() {
	ENGINE_DESC EngineOption = GameInstance::GetInstance().Get_EngineOption();

	uint32_t WX = static_cast<uint32_t>(EngineOption.WindowResolutionX), WY = static_cast<uint32_t>(EngineOption.WindowResolutionY);

	Render_BrightPass(WX, WY);
	Render_HorizontalBlur(WX, WY);
	Render_VerticalBlur(WX, WY);

	D3D11_VIEWPORT origViewport = { 0.0f, 0.0f, (float)EngineOption.WindowResolutionX, (float)EngineOption.WindowResolutionY, 0.0f, 1.0f };
	DEVCTX->RSSetViewports(1, &origViewport);
	
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Bind_ShaderResourceView(static_cast<uint32_t>(RENDER_TARGET::COMBINDED), Component_Shader, "g_CombinedTexture"))) return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Bind_ShaderResourceView(static_cast<uint32_t>(RENDER_TARGET::VERTICALBLUR), Component_Shader, "g_BlurVTexture"))) return E_FAIL;
	
	const float fIntensity = 0.5f;
	Component_Shader->Bind_RawValue("g_fBloomIntensity", &fIntensity, sizeof(float));

	if (FAILED(Component_Shader->Shader_Begin(static_cast<uint32_t>(DEFERRED_TYPE::BLOOM)))) return E_FAIL;
	if (FAILED(Component_RectBuffer->Bind_Resources()))	return E_FAIL;
	if (FAILED(Component_RectBuffer->Render_Buffer()))	return E_FAIL;
	
	return S_OK;
}

HRESULT RenderManager::Render_BrightPass(uint32_t _WindowResolutionX, uint32_t _WindowResolutionY) {

	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Begin_MultiRenderTarget(L"MRT_BrightPass"))) return E_FAIL;

	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Bind_ShaderResourceView(static_cast<uint32_t>(RENDER_TARGET::COMBINDED), Component_Shader, "g_CombinedTexture"))) return E_FAIL;
	
	D3D11_VIEWPORT blurViewport = { 0.0f, 0.0f, (float)(_WindowResolutionX / 2), (float)(_WindowResolutionY / 2), 0.0f, 1.0f };
	DEVCTX->RSSetViewports(1, &blurViewport);

	if (FAILED(Component_Shader->Shader_Begin(static_cast<uint32_t>(DEFERRED_TYPE::PUREBR)))) return E_FAIL;
	if (FAILED(Component_RectBuffer->Bind_Resources()))	return E_FAIL;
	if (FAILED(Component_RectBuffer->Render_Buffer())) return E_FAIL;

	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->End_MultiRenderTarget())) return E_FAIL;

	return S_OK;
}
HRESULT RenderManager::Render_HorizontalBlur(uint32_t _WindowResolutionX, uint32_t _WindowResolutionY) {
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Begin_MultiRenderTarget(L"MRT_HorizontalBlur"))) return E_FAIL;

	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Bind_ShaderResourceView(static_cast<uint32_t>(RENDER_TARGET::BRIGHTPASS), Component_Shader, "g_BlurHTexture"))) return E_FAIL;

	XMFLOAT2 horizontalOffset = XMFLOAT2(1.0f / (_WindowResolutionX / 2), 0.0f);
	Component_Shader->Bind_RawValue("g_vTexelOffset", &horizontalOffset, sizeof(XMFLOAT2));

	D3D11_VIEWPORT blurViewport = { 0.0f, 0.0f, (float)(_WindowResolutionX / 2), (float)(_WindowResolutionY / 2), 0.0f, 1.0f };
	DEVCTX->RSSetViewports(1, &blurViewport);

	if (FAILED(Component_Shader->Shader_Begin(static_cast<uint32_t>(DEFERRED_TYPE::BLUR)))) return E_FAIL;
	if (FAILED(Component_RectBuffer->Bind_Resources()))	return E_FAIL;
	if (FAILED(Component_RectBuffer->Render_Buffer())) return E_FAIL;

	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->End_MultiRenderTarget())) return E_FAIL;

	return S_OK;
}
HRESULT RenderManager::Render_VerticalBlur(uint32_t _WindowResolutionX, uint32_t _WindowResolutionY) {
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Begin_MultiRenderTarget(L"MRT_VerticalBlur"))) return E_FAIL;

	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Bind_ShaderResourceView(static_cast<uint32_t>(RENDER_TARGET::HORIZONTALBLUR), Component_Shader, "g_BlurVTexture"))) return E_FAIL;

	XMFLOAT2 verticalOffset = XMFLOAT2(0.0f, 1.0f / (_WindowResolutionY / 2));
	Component_Shader->Bind_RawValue("g_vTexelOffset", &verticalOffset, sizeof(XMFLOAT2));

	D3D11_VIEWPORT blurViewport = { 0.0f, 0.0f, (float)(_WindowResolutionX / 2), (float)(_WindowResolutionY / 2), 0.0f, 1.0f };
	DEVCTX->RSSetViewports(1, &blurViewport);

	if (FAILED(Component_Shader->Shader_Begin(static_cast<uint32_t>(DEFERRED_TYPE::BLUR)))) return E_FAIL;
	if (FAILED(Component_RectBuffer->Bind_Resources()))	return E_FAIL;
	if (FAILED(Component_RectBuffer->Render_Buffer())) return E_FAIL;

	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->End_MultiRenderTarget())) return E_FAIL;

	return S_OK;
}

VOID RenderManager::Add_InstancedObject(shared_ptr<MapObject> _MOBJ) {
	if (nullptr == _MOBJ) return;

	shared_ptr<Shader>		Component_Shader = _MOBJ->Get_ShaderComponent();
	shared_ptr<MeshLoader>	Component_Model  = _MOBJ->Get_ModelComponent();

	vector<shared_ptr<InstanceMesh>>* SubMeshList = Component_Model->Get_InstanceMeshList();
	vector<_bool>* RenderFlagList = Component_Model->Get_RenderFlagList();

	XMMATRIX WorldMatrix = _MOBJ->Get_TransformComponent()->Get_WorldMatrix();

	for (uint32_t IDX = 0; IDX < SubMeshList->size(); IDX++) {
		if ((*RenderFlagList)[IDX] == false) continue;
		shared_ptr<InstanceMesh> SM = (*SubMeshList)[IDX];

		auto iter = MapObjectInstanceList.find(SM.get());
		if (iter == MapObjectInstanceList.end()) {
			MapObjectInstanceList.insert({ SM.get(), InstanceGroup{SM, Component_Shader, WorldMatrix}});
		}
		else {
			iter->second.WorldMatrixList.push_back(WorldMatrix);
		}
	}
}
HRESULT RenderManager::Render_InstancedObject() {
	shared_ptr<Camera> MainCamera = GameInstance::GetInstance().Get_MainCamera();

	D3D11_MAPPED_SUBRESOURCE MRES;
	if (FAILED(DEVCTX->Map(GlobalInstanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MRES))) return E_FAIL;

	XMFLOAT4X4* pBufferData = reinterpret_cast<XMFLOAT4X4*>(MRES.pData);
	uint32_t CurrentOffset = 0;

	for (auto& Pair : MapObjectInstanceList) {
		InstanceGroup& Group = Pair.second;
		uint32_t GroupSize = static_cast<uint32_t>(Group.WorldMatrixList.size());
		if (0 == GroupSize) continue;

		memcpy(&pBufferData[CurrentOffset], Group.WorldMatrixList.data(), sizeof(XMFLOAT4X4) * GroupSize);

		Group.BufferStartOffset = CurrentOffset;
		CurrentOffset += GroupSize;
	}
	DEVCTX->Unmap(GlobalInstanceBuffer.Get(), 0);

	for (auto& InstancePair : MapObjectInstanceList) {
		InstanceGroup& Group = InstancePair.second;
		uint32_t GroupMatrixSize = static_cast<uint32_t>(Group.WorldMatrixList.size());
		if (0 == GroupMatrixSize) continue;

		shared_ptr<InstanceMesh> GroupStaticMesh = Group.REP_Buffer;
		shared_ptr<Shader>		 GroupShader	 = Group.REP_Shader;

		GroupShader->Bind_Matrix("ViewMatrix", MainCamera->Get_RawViewMatrix());
		GroupShader->Bind_Matrix("ProjMatrix", MainCamera->Get_RawProjMatrix());

		GroupShader->Bind_RawValue("g_vCamPosition", MainCamera->Get_RawCameraPosition(), sizeof(XMFLOAT4));

		XMFLOAT4X4 LocalMatrix;
		XMStoreFloat4x4(&LocalMatrix, XMMatrixIdentity());
		GroupShader->Bind_Matrix("LocalMatrix", &LocalMatrix);
		GroupStaticMesh->Bind_TextureResource(GroupShader, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0);
		GroupStaticMesh->Bind_TextureResource(GroupShader, "g_NormalTexture", aiTextureType_NORMALS, 0);

		if (GroupStaticMesh->Get_TextureTypeList(aiTextureType_EMISSIVE).size() >= 1) {
			GroupStaticMesh->Bind_TextureResource(GroupShader, "g_EmissiveTexture", aiTextureType_EMISSIVE, 0);
		}
		else {
			GroupStaticMesh->UnBind_TextureResource(GroupShader, "g_EmissiveTexture");
		}

		uint32_t SceneIndex = GameInstance::GetInstance().Get_SceneOption().CurrentSceneIndex;

		if (FAILED(GroupShader->Shader_Begin(SceneIndex))) continue;

		GroupStaticMesh->BindOffset_Resources(GlobalInstanceBuffer, Group.BufferStartOffset);

		GroupStaticMesh->Render_Buffer(GroupMatrixSize);

		Group.WorldMatrixList.clear();
	}
	return S_OK;
}
#ifdef _DEBUG
VOID RenderManager::Create_DepthStencilTexture() {

	ENGINE_DESC EngineOption = GameInstance::GetInstance().Get_EngineOption();

	D3D11_TEXTURE2D_DESC DDESC = {};

	DDESC.Width = static_cast<uint32_t>(EngineOption.WindowResolutionX);
	DDESC.Height = static_cast<uint32_t>(EngineOption.WindowResolutionY);
	DDESC.MipLevels = 1;
	DDESC.ArraySize = 1;
	DDESC.Format = DXGI_FORMAT_R24G8_TYPELESS;
	DDESC.SampleDesc.Count = 1;
	DDESC.SampleDesc.Quality = 0;
	DDESC.Usage = D3D11_USAGE_DEFAULT;
	DDESC.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(GRPDEV->CreateTexture2D(&DDESC, nullptr, DepthT2D.GetAddressOf()))) {
		MSG_BOX("Cannot Create Texture2D");
		return;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDESC = {};
	DSVDESC.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DSVDESC.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSVDESC.Texture2D.MipSlice = 0;

	if (FAILED(GRPDEV->CreateDepthStencilView(DepthT2D.Get(), &DSVDESC, COMDSV.GetAddressOf()))) {
		MSG_BOX("Cannot Create DepthStencilView");
		return;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDESC = {};
	SRVDESC.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	SRVDESC.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDESC.Texture2D.MostDetailedMip = 0;
	SRVDESC.Texture2D.MipLevels = 1;

	if (FAILED(GRPDEV->CreateShaderResourceView(DepthT2D.Get(), &SRVDESC, DepthTexture.GetAddressOf()))) {
		MSG_BOX("Cannot Create ShaderResourceView");
		return;
	}

	ComPtr<ID3D11RenderTargetView> BackBufferRTV = nullptr;
	DEVCTX->OMSetRenderTargets(1.f, BackBufferRTV.GetAddressOf(), COMDSV.Get());
}
HRESULT RenderManager::Add_DebugComponent(shared_ptr<Component> _CMP) {

	ComponentList.push_back(_CMP);
	return S_OK;
}
HRESULT RenderManager::Render_RenderTargetView() {

	for (auto& CMP : ComponentList) {
		CMP->Render();
	}

	ComponentList.clear();

	XMFLOAT4X4 WorldMatrix = {};
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	if (FAILED(Component_Shader->Bind_Matrix("g_ViewMatrix" , &ViewMatrix))) return E_FAIL;;
	if (FAILED(Component_Shader->Bind_Matrix("g_ProjMatrix" , &ProjMatrix))) return E_FAIL;;

	if (FAILED(Component_RectBuffer->Bind_Resources()))	return E_FAIL;

	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Render_DebugRender(L"MRT_GameObject", Component_Shader, "g_Texture", Component_RectBuffer)))		  return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Render_DebugRender(L"MRT_LightAccumulation", Component_Shader, "g_Texture", Component_RectBuffer))) return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Render_DebugRender(L"MRT_BrightPass", Component_Shader, "g_Texture", Component_RectBuffer))) return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Render_DebugRender(L"MRT_Combined", Component_Shader, "g_Texture", Component_RectBuffer))) return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Get_RenderTargetManager()->Render_DebugRender(L"MRT_NonLight", Component_Shader, "g_Texture", Component_RectBuffer))) return E_FAIL;

	return S_OK;
}
#endif

HRESULT RenderManager::Add_GameObject(RENDER_TYPE _RTYPE, shared_ptr<GameObject> _GOBJ) {

	if (_GOBJ == nullptr)	return E_FAIL;
	RenderObjectList[(uint32_t)_RTYPE].push_back(_GOBJ);

	return S_OK;
}
unique_ptr<RenderManager>	RenderManager::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<RenderManager>(new RenderManager(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Ready_RenderManager())) {
		MSG_BOX("Cannot Create RenderManager");
		return nullptr;
	}
	return Instance;
}