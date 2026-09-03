#include "../Default/PCH.h"
#include "GameInstance.h"

Ocean::Ocean(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) {}
Ocean::Ocean(CONST Ocean& _PRTOBJ) : GameObject(_PRTOBJ) {}


HRESULT		Ocean::Initialize_ProtoType() {
	uint32_t		CurrentSceneIndex = GameInstance::GetInstance().Get_SceneOption().CurrentSceneIndex;

	Component_Transform = Add_Component<Transform>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_TRANSFORM);
	Component_Terrain = Add_Component<Terrain>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_TERRAIN);

	return S_OK;
}
HRESULT		Ocean::Initialize(VOID* _ARG) {
	Component_Transform = static_pointer_cast<Transform>(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_TRANSFORM]);
	Component_Terrain = static_pointer_cast<Terrain>(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_TERRAIN]);
	Component_OceanShader = Shader::Create(GRPDEV, DEVCTX, L"../../Shader_Ocean2.hlsl", VTXTEX::Elements, VTXTEX::ElementsCount);

	//Component_Transform->Set_WorldPosition(-10000.f, -1.f, -10000.f);
	Component_Transform->Set_WorldScale(500.f, 1.f, 500.f);

	Generate_ReflectTexture();

	ColorRampTexture = GameInstance::GetInstance().Get_TextureManager()->Load_Texture(L"../../Resource/Asset/Texture/T_Color_27010.png");
	OceanFoamTexture = GameInstance::GetInstance().Get_TextureManager()->Load_Texture(L"../../Resource/Asset/Texture/T4_Com_Wat_10A_D.png");
	OceanNoiseTexture = GameInstance::GetInstance().Get_TextureManager()->Load_Texture(L"../../Resource/Asset/Texture/PerlinNoise.png");

	OceanWaveDataA = { 0.9f + 1.f, 0.2f + 1.f, 0.f, 500.f };
	OceanWaveDataB = { 0.5f + 1.f, 0.8f + 1.f, 0.f, 200.f };

	return S_OK;
}
VOID		Ocean::Update(CONST _float& _DT) {
	TimeAccumulation += _DT;
}
VOID		Ocean::Late_Update(CONST _float& _DT) {
	XMFLOAT4 MainCameraPosition = *GameInstance::GetInstance().Get_MainCamera()->Get_RawCameraPosition();
	Component_Transform->Set_WorldPosition(MainCameraPosition.x, -1.f, MainCameraPosition.z);
	GameInstance::GetInstance().Get_RenderManager()->Add_GameObject(RENDER_TYPE::RENDER_NONBLEND, shared_from_this());
}
HRESULT		Ocean::Render() {

	shared_ptr<Camera> MainCamera = GameInstance::GetInstance().Get_MainCamera();

	Render_ReflectionTexture(MainCamera);

	XMFLOAT4X4 ViewMatrix, ProjMatrix;
	XMFLOAT4 Position;

	XMStoreFloat4x4(&ViewMatrix, MainCamera->Get_ViewMatrix());
	XMStoreFloat4x4(&ProjMatrix, MainCamera->Get_ProjMatrix());
	XMStoreFloat4(&Position, MainCamera->Get_CameraPosition());

	if (FAILED(Component_Transform->Bine_ShaderResource(Component_OceanShader, "g_WorldMatrix")))				return E_FAIL;
	if (FAILED(Component_OceanShader->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))								return E_FAIL;
	if (FAILED(Component_OceanShader->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))								return E_FAIL;

	XMFLOAT4X4 RViewMat;
	XMStoreFloat4x4(&RViewMat, ReflectionViewMatrix);
	if (FAILED(Component_OceanShader->Bind_Matrix("ReflectViewMatrix", &RViewMat)))	return E_FAIL;

	if (FAILED(Component_OceanShader->Bind_RawValue("OceanWaveDataA", &OceanWaveDataA, sizeof(XMFLOAT4))))		return E_FAIL;
	if (FAILED(Component_OceanShader->Bind_RawValue("OceanWaveDataB", &OceanWaveDataB, sizeof(XMFLOAT4))))		return E_FAIL;

	if (FAILED(Component_OceanShader->Bind_RawValue("TimeAccumulation", &TimeAccumulation, sizeof(_float))))	return E_FAIL;
	if (FAILED(Component_OceanShader->Bind_RawValue("CameraPosition", &Position, sizeof(XMFLOAT3))))			return E_FAIL;

	if (FAILED(Component_OceanShader->Bind_ShaderResourceView("RampTexture", ColorRampTexture)))				return E_FAIL;
	if (FAILED(Component_OceanShader->Bind_ShaderResourceView("FoamTexture", OceanFoamTexture)))				return E_FAIL;
	if (FAILED(Component_OceanShader->Bind_ShaderResourceView("ReflectionTexture", ReflectionTexture)))			return E_FAIL;

	//ID3D11RenderTargetView* pNullRTV[1] = { nullptr };
	//DEVCTX->OMSetRenderTargets(1, pNullRTV, nullptr);
	//
	//DEVCTX->OMSetRenderTargets(1, BackBufferRTV.GetAddressOf(), COMDSV.Get());
	//auto RenderMgrDepthTex = GameInstance::GetInstance().Get_RenderManager()->Get_DepthTexture();
	//if (FAILED(Component_OceanShader->Bind_ShaderResourceView("DepthTexture", RenderMgrDepthTex))) return E_FAIL;

	if (FAILED(Component_OceanShader->Bind_ShaderResourceView("NoiseTexture", OceanNoiseTexture)))				return E_FAIL;
	if (FAILED(Component_OceanShader->Shader_Begin(0)))															return E_FAIL;
	if (FAILED(Component_Terrain->Bind_Resources()))															return E_FAIL;
	if (FAILED(Component_Terrain->Render_Buffer()))																return E_FAIL;

	//ID3D11ShaderResourceView* pUnbindSRVs[16] = { nullptr };
	//DEVCTX->PSSetShaderResources(0, 16, pUnbindSRVs);
	//
	//auto BackDSV = GameInstance::GetInstance().Get_RenderManager()->Get_DepthStencilView();
	//DEVCTX->OMSetRenderTargets(1, BackBufferRTV.GetAddressOf(), BackBufferDSV.Get());

	return S_OK;
}

HRESULT Ocean::Generate_ReflectTexture() {
	D3D11_TEXTURE2D_DESC DESC = {};
	ENGINE_DESC EngineOption = GameInstance::GetInstance().Get_EngineOption();

	DESC.Width  = static_cast<uint32_t>(EngineOption.WindowResolutionX);
	DESC.Height = static_cast<uint32_t>(EngineOption.WindowResolutionY);
	DESC.MipLevels = 1;
	DESC.ArraySize = 1;
	DESC.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	DESC.SampleDesc.Count = 1;
	DESC.SampleDesc.Quality = 0;
	DESC.Usage = D3D11_USAGE_DEFAULT;
	DESC.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(GRPDEV->CreateTexture2D(&DESC, nullptr, ReflectT2D.GetAddressOf()))) {
		MSG_BOX("Cannot Create Texture2D");
		return E_FAIL;
	}
	if (FAILED(GRPDEV->CreateRenderTargetView(ReflectT2D.Get(), nullptr, ReflectRTV.GetAddressOf()))) {
		MSG_BOX("Cannot Create RenderTargetView");
		return E_FAIL;
	}
	if (FAILED(GRPDEV->CreateShaderResourceView(ReflectT2D.Get(), nullptr, ReflectionTexture.GetAddressOf()))) {
		MSG_BOX("Cannot Create ShaderResourceView");
		return E_FAIL;
	}

	return S_OK;
}

VOID Ocean::Render_ReflectionTexture(shared_ptr<Camera> _MainCamera) {
	BackBufferRTV = { nullptr };
	BackBufferDSV = { nullptr };
	DEVCTX->OMGetRenderTargets(1, BackBufferRTV.GetAddressOf(), BackBufferDSV.GetAddressOf());

	XMVECTOR OriginalPosition = _MainCamera->Get_CameraPosition();
	XMMATRIX OriginalViewMat = _MainCamera->Get_ViewMatrix();
	XMVECTOR OriginalLook = XMMatrixInverse(nullptr, OriginalViewMat).r[2];

	float OceanHeight = XMVectorGetY(Component_Transform->Get_WorldPosition());

	XMFLOAT4 CamPosition = {}, CamLook = {};
	XMStoreFloat4(&CamPosition, OriginalPosition);
	XMStoreFloat4(&CamLook, OriginalLook);

	XMVECTOR ReflectPos = XMVectorSet(CamPosition.x, (OceanHeight * 2.f) - CamPosition.y, CamPosition.z, 1.f);
	XMVECTOR ReflectLook = XMVector3Normalize(XMVectorSet(CamLook.x, 0.f, CamLook.z, 0.f));
	XMVECTOR ReflectTarget = ReflectPos + ReflectLook;
	XMVECTOR ReflectUp = XMVectorSet(0.f, -1.f, 0.f, 0.f);

	ReflectionViewMatrix = XMMatrixLookAtLH(ReflectPos, ReflectTarget, ReflectUp);

	float ClearColor[4] = { 0.f, 0.f, 0.f, 1.f };
	DEVCTX->ClearRenderTargetView(ReflectRTV.Get(), ClearColor);
	DEVCTX->OMSetRenderTargets(1, ReflectRTV.GetAddressOf(), nullptr);

	_MainCamera->Set_CameraViewMatrix(ReflectionViewMatrix);
	_MainCamera->Update_InverseMatrix();

	auto SKSP = GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Get_GameObject("SkySphere");
	if (SKSP) {
		auto SKSPTRS = static_pointer_cast<Transform>(SKSP->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
		XMVECTOR SKSPPOS = SKSPTRS->Get_WorldPosition();

		SKSPTRS->Set_WorldPosition(ReflectPos);
		SKSP->Render();
		SKSPTRS->Set_WorldPosition(SKSPPOS);
	}

	_MainCamera->Set_CameraViewMatrix(OriginalViewMat);
	_MainCamera->Update_CameraView();

	DEVCTX->OMSetRenderTargets(1, BackBufferRTV.GetAddressOf(), BackBufferDSV.Get());
}

unique_ptr<Ocean>		Ocean::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<Ocean>(new Ocean(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create Ocean.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>	Ocean::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<Ocean>(new Ocean(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone Ocean");
		return nullptr;
	}
	return Instance;
}
