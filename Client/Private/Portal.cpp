#include "../Default/PCH.h"
#include "GameInstance.h"

Portal::Portal(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) {};
Portal::Portal(CONST Portal& _PRTOBJ) : GameObject(_PRTOBJ) {};

HRESULT Portal::Initialize_ProtoType() {
	uint32_t		CurrentSceneIndex = GameInstance::GetInstance().Get_SceneOption().CurrentSceneIndex;

	Component_Transform		= Add_Component<Transform>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_TRANSFORM);
	Component_Model			= Add_Component<MeshLoader>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_MODEL);
	Component_Shader		= Add_Component<Shader>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_STATIC_SHADER);
	Component_Collider		= Add_Component<Collider>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_COLLIDER);

	Component_Model->Load_FBXModel(L"../../Resource/Asset/MapResource/Portal.fbx", ANIMATION_TYPE::NON_ANIMATION);
	SRV = GameInstance::GetInstance().Get_TextureManager()->Load_Texture("../../Resource/Asset/Texture/T_Com2_Pro_118C_D.dds");
	return S_OK;
}
HRESULT Portal::Initialize(VOID* _ARG) {
	Component_Transform = static_pointer_cast<Transform> (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_TRANSFORM]);
	Component_Model		= static_pointer_cast<MeshLoader>(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_MODEL]);
	Component_Shader	= static_pointer_cast<Shader>	 (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_STATIC_SHADER]);
	Component_Collider	= static_pointer_cast<Collider>	 (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_COLLIDER]);

	Component_Portal_Shader = Shader::Create(GRPDEV, DEVCTX, L"../../Shader_Portal.hlsl", VTXMESH::Elements, VTXMESH::ElementsCount);

	Component_Collider->Set_AutomaticTransform(Component_Model, Component_Transform, COLLIDER_TYPE::COLLIDER_AABB);
	Component_Collider->Set_ColliderOwner(shared_from_this());
	GameInstance::GetInstance().Get_CollisionManager()->Register_Collider(Component_Collider);
	GameInstance::GetInstance().Get_RayCaster()->Register_Recipient(shared_from_this());

	GamePlayer = static_pointer_cast<Player>(GameInstance::GetInstance().Get_GamePlayer());

	XMVECTOR InitialPosition = XMVectorSet(1.36f, 1.2f, -340.f, 1.f);
	Component_Transform->Set_WorldPosition(InitialPosition);
	Component_Transform->Set_WorldScale(1.5f, 1.5f, 1.5f);
	Component_Model->Update_BoundingBox(Component_Transform->Get_WorldMatrix());

	return S_OK;
}

VOID Portal::Update(CONST _float& _DT) {
	Component_Collider->Update(_DT);
	DeltaTime += _DT;

	if (PortalActivatable && PortalAlpha < 1.f) {
		PortalAlpha += _DT / 3.f;
		
	}
	else if (PortalActivatable && !PortalInteractable && PortalAlpha >= 1.f) {
		PortalInteractable = true;
	}
}
VOID Portal::Late_Update(CONST _float& _DT) {
	GameInstance::GetInstance().Get_RenderManager()->Add_GameObject(RENDER_TYPE::RENDER_NONBLEND, shared_from_this());
}

HRESULT Portal::Render() {
	XMFLOAT4X4 ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&ViewMatrix, GameInstance::GetInstance().Get_MainCamera()->Get_ViewMatrix());
	XMStoreFloat4x4(&ProjMatrix, GameInstance::GetInstance().Get_MainCamera()->Get_ProjMatrix());

	vector<shared_ptr<StaticMesh>>* StaticMeshList = Component_Model->Get_StaticMeshList();

	for (uint32_t IDX = 0; IDX < 2; ++IDX) {
		if (IDX == 0) {
			if (FAILED(Component_Transform->Bine_ShaderResource(Component_Shader, "g_WorldMatrix")))		return E_FAIL;
			if (FAILED(Component_Shader->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))							return E_FAIL;
			if (FAILED(Component_Shader->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))							return E_FAIL;

			Component_Shader->Shader_Begin(0);

			(*StaticMeshList)[IDX]->Render_Mesh();
		}
		else {
			if (FAILED(Component_Transform->Bine_ShaderResource(Component_Portal_Shader, "g_WorldMatrix")))	return E_FAIL;
			if (FAILED(Component_Portal_Shader->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))					return E_FAIL;
			if (FAILED(Component_Portal_Shader->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))					return E_FAIL;
			if (FAILED(Component_Portal_Shader->Bind_RawValue("g_Time", &DeltaTime, sizeof(_float))))		return E_FAIL;

			XMFLOAT4	Position;

			XMStoreFloat4(&Position, GameInstance::GetInstance().Get_MainCamera()->Get_CameraPosition());
			if (FAILED(Component_Portal_Shader->Bind_RawValue("g_vCamPosition", &Position, sizeof(XMFLOAT4))))		return E_FAIL;
			if (FAILED(Component_Portal_Shader->Bind_RawValue("g_PortalAlpha", &PortalAlpha, sizeof(_float))))		return E_FAIL;
			Component_Portal_Shader->Bind_ShaderResourceView("g_DiffuseTexture", SRV);
			Component_Portal_Shader->Shader_Begin(0);

			(*StaticMeshList)[IDX]->Render_Mesh();
		}
	}

	return S_OK;
}

VOID Portal::Activate_Portal() {
	PortalActivatable = true;

	shared_ptr<PopUpUI> PUI = static_pointer_cast<PopUpUI>(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Get_GameObject("PopUpUI"));
	PUI->Set_TargetPosition(Component_Transform->Get_WorldPosition());
	PUI->Set_DescriptionText(L"차원문 개방");
}

VOID Portal::Enter_BossDungeon() {
	GameInstance::GetInstance().Get_SceneManager()->Set_CurrentScene(static_cast<uint32_t>(SCENE_TYPE::SCENE_2));
}

unique_ptr<Portal>		Portal::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<Portal>(new Portal(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create Portal.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>	Portal::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<Portal>(new Portal(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone Portal.");
		return nullptr;
	}
	return Instance;
}
