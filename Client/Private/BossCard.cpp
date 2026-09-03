#include "../Default/PCH.h"
#include "GameInstance.h"

BossCard::BossCard(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) {};
BossCard::BossCard(CONST BossCard& _PRTOBJ) : GameObject(_PRTOBJ) {};

HRESULT BossCard::Initialize_ProtoType(){
	int32_t		CurrentSceneIndex = GameInstance::GetInstance().Get_SceneOption().CurrentSceneIndex;

	Component_Model		= Add_Component<MeshLoader>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_MODEL);
	Component_Transform = Add_Component<Transform>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_TRANSFORM);
	Component_Shader	= Add_Component<Shader>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_STATIC_SHADER);
	Component_Collider	= Add_Component<Collider>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_COLLIDER);

	Component_Model->Load_FBXModel(L"../../Resource/Asset/Effect/BossEffect/BossCard.fbx", ANIMATION_TYPE::NON_ANIMATION, nullptr);

	return S_OK;
}
HRESULT BossCard::Initialize(VOID* _ARG) {
	Component_Transform = static_pointer_cast<Transform> (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_TRANSFORM]);
	Component_Shader	= static_pointer_cast<Shader>	 (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_STATIC_SHADER]);
	Component_Collider	= static_pointer_cast<Collider>	 (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_COLLIDER]);
	Component_Model		= static_pointer_cast<MeshLoader>	 (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_MODEL]);
	
	Component_Collider->Set_AutomaticTransform(Component_Model, Component_Transform, COLLIDER_TYPE::COLLIDER_AABB);
	
	Component_Model->Update_BoundingBox(Component_Transform->Get_WorldMatrix());

	GamePlayer = static_pointer_cast<Player>(GameInstance::GetInstance().Get_GamePlayer());
	FinalBoss = static_pointer_cast<Monster_FinalBoss>(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Get_GameObject("Monster_FinalBoss"));
	Component_Transform->Set_WorldPosition(-8.f, 16.f, -265.f);

	NoiseTexture = GameInstance::GetInstance().Get_TextureManager()->Load_Texture(L"../../Resource/Asset/Effect/BossEffect/T_Tile_300192.png");

	EmissiveValue = 1.f;

	return S_OK;
}
VOID BossCard::Update(const _float& _DT) {
	Component_Collider->Update(_DT);

	if (DissolveEnable == PLAY_TYPE::ONSTART) {
		DissolveEnable = PLAY_TYPE::ONPROGRESS;
	}
	else if (DissolveEnable == PLAY_TYPE::ONPROGRESS) {
		if (DissolveAmount >= 1.f) {
			DissolveAmount = 1.f;
			DissolveEnable = PLAY_TYPE::FINISHED;
			FinalBoss->Enable_Dissolve(PLAY_TYPE::ONSTART, true, 1.f);
			FinalBoss->Set_Activation(true);
		}
		DissolveAmount += _DT * DissolveSpeed;
	}
}
VOID BossCard::Late_Update(const _float& _DT) {
	GameInstance::GetInstance().Get_RenderManager()->Add_GameObject(RENDER_TYPE::RENDER_NONBLEND, shared_from_this());
	Component_Model->Set_SubMeshRenderFlag(0, true);
}
HRESULT BossCard::Render() {

	XMFLOAT4X4 ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&ViewMatrix, GameInstance::GetInstance().Get_MainCamera()->Get_ViewMatrix());
	XMStoreFloat4x4(&ProjMatrix, GameInstance::GetInstance().Get_MainCamera()->Get_ProjMatrix());

	if (FAILED(Component_Transform->Bine_ShaderResource(Component_Shader, "g_WorldMatrix")))				return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))									return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))									return E_FAIL;

	XMFLOAT4 DissolveColor = { 40.f / 255.f, 0.f, 220.f / 255.f, 1.f };
	XMFLOAT4 EmissiveColor = { 1.f, 1.f, 1.f, 1.f };
	if (FAILED(Component_Shader->Bind_RawValue("g_EmissiveDiffuse", &EmissiveColor, sizeof(XMFLOAT4))))		return E_FAIL;
	if (FAILED(Component_Shader->Bind_RawValue("g_EmissiveIntensity", &EmissiveValue, sizeof(_float))))		return E_FAIL;
	if (DissolveAmount > 0.f && DissolveAmount <= 1.f) {
		if (FAILED(Component_Shader->Bind_ShaderResourceView("g_NoiseTexture", NoiseTexture)))				return E_FAIL;
		if (FAILED(Component_Shader->Bind_RawValue("DissolveColor", &DissolveColor, sizeof(XMFLOAT4))))		return E_FAIL;
		if (FAILED(Component_Shader->Bind_RawValue("DissolveAmount", &DissolveAmount, sizeof(_float))))		return E_FAIL;
	}

	Component_Model->Render_Mesh(Component_Shader, XMMatrixIdentity(), 2);
	
	return S_OK;
}

VOID	BossCard::Enable_Dissolve(PLAY_TYPE _Enable, _float _DelayTime, _float _DissolveSpeed) {
	DissolveEnable = _Enable;
	DissolveAmount = -_DelayTime;
	DissolveSpeed = _DissolveSpeed;
}

unique_ptr<BossCard>	BossCard::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<BossCard>(new BossCard(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create BossCard.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>	BossCard::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<BossCard>(new BossCard(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone BossCard.");
		return nullptr;
	}
	return Instance;
}