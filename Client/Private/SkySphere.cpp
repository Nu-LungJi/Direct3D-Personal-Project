#include "../Default/PCH.h"
#include "GameInstance.h"

SkySphere::SkySphere(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX){}
SkySphere::SkySphere(CONST SkySphere& _PRTOBJ) : GameObject(_PRTOBJ) {}

HRESULT SkySphere::Initialize_ProtoType() {
	uint32_t		CurrentSceneIndex = GameInstance::GetInstance().Get_SceneOption().CurrentSceneIndex;

	Component_Transform	 = Add_Component<Transform>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_TRANSFORM);
	Component_Shader	 = Add_Component<Shader>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_SKYSPHERE_SHADER);
	Component_Model		 = Add_Component<MeshLoader>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_MODEL);
	Component_Texture	 = Add_Component<TexBuffer>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_TEXTURE);
	
	if		(CurrentSceneIndex == static_cast<uint32_t>(SCENE_TYPE::SCENE_1))
		Component_Model->Load_FBXModel(L"../../Resource/Asset/SkySphere/SkySphere.fbx", ANIMATION_TYPE::NON_ANIMATION);
	else if (CurrentSceneIndex == static_cast<uint32_t>(SCENE_TYPE::SCENE_2))
		Component_Model->Load_FBXModel(L"../../Resource/Asset/SkySphere/BossSkyBox.fbx", ANIMATION_TYPE::NON_ANIMATION);
	return S_OK;
}
HRESULT	SkySphere::Initialize(VOID* _ARG) {
	uint32_t		CurrentSceneIndex = GameInstance::GetInstance().Get_SceneOption().CurrentSceneIndex;

	Component_Transform  = static_pointer_cast<Transform> (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_TRANSFORM]			);
	Component_Shader     = static_pointer_cast<Shader>	  (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_SKYSPHERE_SHADER]	);
	Component_Model		 = static_pointer_cast<MeshLoader>(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_MODEL]);
	Component_Texture	 = static_pointer_cast<TexBuffer> (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_TEXTURE]);

	if		(CurrentSceneIndex == static_cast<uint32_t>(SCENE_TYPE::SCENE_1))
		Component_Transform->Set_WorldRotation(-90.f, 0.f, 0.f);
	else if (CurrentSceneIndex == static_cast<uint32_t>(SCENE_TYPE::SCENE_2))
		Component_Transform->Set_WorldRotation(0.f, 0.f, 0.f);

	Component_Transform->Set_WorldScale(0.8f, 0.8f, 0.8f);
	Component_Model->Set_SubMeshRenderFlag(0, true);

	if (CurrentSceneIndex == static_cast<uint32_t>(SCENE_TYPE::SCENE_2)) {
		auto InstanceShader = static_pointer_cast<Shader>(GameInstance::GetInstance().Get_ProtoManager()->Clone_ProtoType(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_MESHINSTANCE, nullptr));
		_float	 MapBrightness		= 0.25f;
		XMFLOAT4 MapLightDiffuse	= { 1.f, 0.55f, 0.55f, 0.3f };

		InstanceShader->Bind_RawValue("g_fBrightness", &MapBrightness, sizeof(_float));
		Component_Transform->Set_WorldRotation(0.f, 0.f, 0.f);

		if (nullptr == NoiseTexture)
			NoiseTexture = GameInstance::GetInstance().Get_TextureManager()->Load_Texture(L"../../Resource/Asset/Texture/T_Noise_500002.png");
	}
	
	return S_OK;
}
VOID	SkySphere::Update(CONST _float& _DT) {
	uint32_t		CurrentSceneIndex = GameInstance::GetInstance().Get_SceneOption().CurrentSceneIndex;

	shared_ptr<Transform> TRS = static_pointer_cast<Transform>(GameInstance::GetInstance().Get_MainCamera()->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
	if (CurrentSceneIndex == static_cast<uint32_t>(SCENE_TYPE::SCENE_1)) {
		Component_Transform->Set_WorldPosition(TRS->Get_WorldPosition() - XMVectorSet(0.f, 2.f, 0.f, 0.f));
	}
	if (CurrentSceneIndex == static_cast<uint32_t>(SCENE_TYPE::SCENE_2)) {
		Component_Transform->Set_WorldPosition(TRS->Get_WorldPosition() - XMVectorSet(0.f, 0.f, 0.f, 0.f));
	}

	TimeAccumulation += _DT;
}
VOID	SkySphere::Late_Update(CONST _float& _DT) {
	GameInstance::GetInstance().Get_RenderManager()->Add_GameObject(RENDER_TYPE::RENDER_NONBLEND, shared_from_this());
}
HRESULT	SkySphere::Render() {
	XMFLOAT4X4 ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&ViewMatrix, GameInstance::GetInstance().Get_MainCamera()->Get_ViewMatrix());
	XMStoreFloat4x4(&ProjMatrix, GameInstance::GetInstance().Get_MainCamera()->Get_ProjMatrix());

	if (FAILED(Component_Transform->Bine_ShaderResource(Component_Shader, "WorldMatrix")))							return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("ViewMatrix", &ViewMatrix)))											return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("ProjMatrix", &ProjMatrix)))											return E_FAIL;

	//if (FAILED(Component_Shader->Bind_ShaderResourceView("SkySphere_BackGround", BackgroundTexture)))	return E_FAIL;
	if (GameInstance::GetInstance().Get_CurrentSceneIndex() == static_cast<uint32_t>(SCENE_TYPE::SCENE_2)) {
		_float DisrtortionStrength = 0.02f;
		if (FAILED(Component_Shader->Bind_ShaderResourceView("g_NoiseTexture", NoiseTexture)))						return E_FAIL;
		if (FAILED(Component_Shader->Bind_RawValue("TimeAccumulation", &TimeAccumulation, sizeof(_float))))			return E_FAIL;
		if (FAILED(Component_Shader->Bind_RawValue("DisrtortionStrength", &DisrtortionStrength, sizeof(_float))))	return E_FAIL;
	}
	Component_Model->Render_Mesh(Component_Shader, XMMatrixIdentity(), GameInstance::GetInstance().Get_CurrentSceneIndex());

	return S_OK;
}

unique_ptr<SkySphere>		SkySphere::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<SkySphere>(new SkySphere(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create SkySphere.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>	SkySphere::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<SkySphere>(new SkySphere(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone SkySphere.");
		return nullptr;
	}
	return Instance;
}