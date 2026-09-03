#include "../Default/PCH.h"
#include "GameInstance.h"

Bicon::Bicon(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) {};
Bicon::Bicon(CONST Bicon& _PRTOBJ) : GameObject(_PRTOBJ) {};

HRESULT Bicon::Initialize_ProtoType() {
	uint32_t		CurrentSceneIndex = GameInstance::GetInstance().Get_SceneOption().CurrentSceneIndex;

	Component_Transform = Add_Component<Transform>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_TRANSFORM);
	Component_Model		= Add_Component<MeshLoader>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_MODEL);
	Component_Shader	= Add_Component<Shader>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_STATIC_SHADER);
	Component_Collider	= Add_Component<Collider>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_COLLIDER);

	Component_Model->Load_FBXModel(L"../../Resource/Asset/MapResource/Bicon.fbx", ANIMATION_TYPE::NON_ANIMATION);

	return S_OK;
}
HRESULT Bicon::Initialize(VOID* _ARG) {
	Component_Transform = static_pointer_cast<Transform> (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_TRANSFORM]);
	Component_Model = static_pointer_cast<MeshLoader>(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_MODEL]);
	Component_Shader = static_pointer_cast<Shader>	 (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_STATIC_SHADER]);
	Component_Collider = static_pointer_cast<Collider>	 (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_COLLIDER]);

	Component_Collider->Set_AutomaticTransform(Component_Model, Component_Transform, COLLIDER_TYPE::COLLIDER_AABB);
	Component_Collider->Set_ColliderOwner(shared_from_this());
	//GameInstance::GetInstance().Get_CollisionManager()->Register_Collider(Component_Collider);
	//GameInstance::GetInstance().Get_RayCaster()->Register_Recipient(shared_from_this());

	GamePlayer = static_pointer_cast<Player>(GameInstance::GetInstance().Get_GamePlayer());

	XMVECTOR InitialPosition = XMVectorSet(0.f, 11.2f, -60.f, 1.f);
	Component_Transform->Set_WorldPosition(InitialPosition);

	Component_Model->Update_BoundingBox(Component_Transform->Get_WorldMatrix());

	return S_OK;
}

VOID Bicon::Update(CONST _float& _DT) {
	Component_Collider->Update(_DT);
}
VOID Bicon::Late_Update(CONST _float& _DT) {
	GameInstance::GetInstance().Get_RenderManager()->Add_GameObject(RENDER_TYPE::RENDER_NONBLEND, shared_from_this());
}

HRESULT Bicon::Render() {
	XMFLOAT4X4 ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&ViewMatrix, GameInstance::GetInstance().Get_MainCamera()->Get_ViewMatrix());
	XMStoreFloat4x4(&ProjMatrix, GameInstance::GetInstance().Get_MainCamera()->Get_ProjMatrix());

	_float One = 1.f;
	_float4 Four = { 1.f, 1.f , 1.f ,1.f };
	if (FAILED(Component_Transform->Bine_ShaderResource(Component_Shader, "g_WorldMatrix")))		return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))							return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))							return E_FAIL;
	if (FAILED(Component_Shader->Bind_RawValue("g_EmissiveIntensity", &One, sizeof(_float))))		return E_FAIL;
	if (FAILED(Component_Shader->Bind_RawValue("g_EmissiveDiffuse", &Four, sizeof(_float4))))		return E_FAIL;

	Component_Model->Render_Mesh(Component_Shader);
	
	return S_OK;
}

unique_ptr<Bicon>		Bicon::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<Bicon>(new Bicon(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create Bicon.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>	Bicon::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<Bicon>(new Bicon(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone Bicon.");
		return nullptr;
	}
	return Instance;
}
