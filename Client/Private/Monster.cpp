#include "../Default/PCH.h"
#include "Monster.h"

Monster::Monster(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) {}
Monster::~Monster() {}

HRESULT Monster::Initialize_ProtoType() {
	ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_TRANSFORM] = static_pointer_cast<Transform>(GameInstance::GetInstance().Get_ProtoManager()->Clone_ProtoType((uint32_t)SCENE_TYPE::SCENE_1, COMPONENT_TYPE::COMPONENT_TRANSFORM, nullptr));
	Component_Transform = static_pointer_cast<Transform>(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_TRANSFORM]);

	ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_MODEL] = static_pointer_cast<MeshLoader>(GameInstance::GetInstance().Get_ProtoManager()->Clone_ProtoType((uint32_t)SCENE_TYPE::SCENE_1, COMPONENT_TYPE::COMPONENT_MODEL, nullptr));
	Component_Model = static_pointer_cast<MeshLoader>(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_MODEL]);

	ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_ANIMATOR] = static_pointer_cast<Animator>(GameInstance::GetInstance().Get_ProtoManager()->Clone_ProtoType((uint32_t)SCENE_TYPE::SCENE_1, COMPONENT_TYPE::COMPONENT_ANIMATOR, nullptr));
	Component_Animator = static_pointer_cast<Animator>(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_ANIMATOR]);

	ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_BOUNDBOX] = static_pointer_cast<AABBCollider>(GameInstance::GetInstance().Get_ProtoManager()->Clone_ProtoType((uint32_t)SCENE_TYPE::SCENE_1, COMPONENT_TYPE::COMPONENT_BOUNDBOX, nullptr));
	Component_BoundingBox = static_pointer_cast<AABBCollider>(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_BOUNDBOX]);

	ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_DYNAMIC_SHADER] = static_pointer_cast<Shader>(GameInstance::GetInstance().Get_ProtoManager()->Clone_ProtoType((uint32_t)SCENE_TYPE::SCENE_1, COMPONENT_TYPE::COMPONENT_DYNAMIC_SHADER, nullptr));
	Component_Shader = static_pointer_cast<Shader>(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_DYNAMIC_SHADER]);

	ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_TEXTURE] = static_pointer_cast<TexBuffer>(GameInstance::GetInstance().Get_ProtoManager()->Clone_ProtoType((uint32_t)SCENE_TYPE::SCENE_1, COMPONENT_TYPE::COMPONENT_TEXTURE, nullptr));
	Component_Texture = static_pointer_cast<TexBuffer>(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_TEXTURE]);

	Component_Model->Load_FBXModel(L"../../Resource/Asset/Character/Scar/Scar.fbx", ANIMATION_TYPE::ANIMATION, Component_Animator);

	Component_BoundingBox->Set_ObjectTransform(Component_Transform);
	Component_BoundingBox->Set_ColliderBoxVolume(Component_Model->Get_CoordVertexMin(), Component_Model->Get_CoordVertexMax());

	return S_OK;
}
HRESULT Monster::Initialize(VOID* _ARG) {
	Component_Transform		= static_pointer_cast<Transform>	 (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_TRANSFORM]);
	Component_Model			= static_pointer_cast<MeshLoader>	 (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_MODEL]);
	Component_Animator		= static_pointer_cast<Animator>		 (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_ANIMATOR]);
	Component_BoundingBox	= static_pointer_cast<AABBCollider>	 (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_BOUNDBOX]);
	Component_Shader		= static_pointer_cast<Shader>		 (ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_DYNAMIC_SHADER]);

	Component_BoundingBox->Set_ColliderBoxVolume(Component_Model->Get_CoordVertexMin(), Component_Model->Get_CoordVertexMax());
	Component_BoundingBox->Set_ObjectTransform(Component_Transform);

	Component_Transform->Set_WorldScale(1.f, 1.f, 1.f);
	Component_Transform->Set_WorldPosition(5.f, 0.f, 0.f);
	GameInstance::GetInstance().Get_RayCaster()->Register_Recipient(shared_from_this());

	return S_OK;
}
VOID	Monster::Update(_float _DT) {
	Component_Animator->Update_Animation(_DT);
	GameInstance::GetInstance().Get_RenderManager()->Add_GameObject(RENDER_TYPE::RENDER_NONBLEND, shared_from_this());
}
HRESULT		Monster::Render() {
	XMFLOAT4X4 ViewMatrix, ProjMatrix;
	XMFLOAT4 Position;
	XMStoreFloat4x4(&ViewMatrix, GameInstance::GetInstance().Get_MainCamera()->Get_ViewMatrix());
	XMStoreFloat4x4(&ProjMatrix, GameInstance::GetInstance().Get_MainCamera()->Get_ProjMatrix());
	XMStoreFloat4(&Position, GameInstance::GetInstance().Get_MainCamera()->Get_CameraPosition());

	if (FAILED(Component_Transform->Bine_ShaderResource(Component_Shader, "g_WorldMatrix")))		return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))							return E_FAIL;
	if (FAILED(Component_Shader->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))							return E_FAIL;

	//Component_Model->Bind_TextureResource(Component_Shader, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0);
	Component_Model->Render_Mesh(Component_Shader);

	return S_OK;
}
unique_ptr<Monster>		Monster::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<Monster>(new Monster(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create Monster.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>	Monster::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<Monster>(new Monster(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone Monster.");
		return nullptr;
	}
	return Instance;
}