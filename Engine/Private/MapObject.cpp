#include "MapObject.h"
#include "GameInstance.h"

MapObject::MapObject(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) {}
MapObject::MapObject(CONST MapObject& _PRTOBJ) : GameObject(_PRTOBJ){}

HRESULT		MapObject::Initialize_ProtoType(const wstring& _FilePath, uint32_t _LevelIndex) {

	uint32_t		CurrentSceneIndex = GameInstance::GetInstance().Get_SceneOption().CurrentSceneIndex;

	Component_Transform		= Add_Component<Transform>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_TRANSFORM);
	Component_Model			= Add_Component<MeshLoader>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_MODEL);
	Component_Shader		= Add_Component<Shader>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_MESHINSTANCE);
	Component_Collider		= Add_Component<Collider>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_COLLIDER);
	Component_Navigation	= Add_Component<NavMeshAgent>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_NAVIGATION);

 	Component_Model->Load_FBXModel(_FilePath.c_str(), ANIMATION_TYPE::INSTANCED);

	return S_OK;
}
HRESULT		MapObject::Initialize(VOID* _ARG) {
	Component_Transform		= static_pointer_cast<Transform>	(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_TRANSFORM]);
	Component_Model			= static_pointer_cast<MeshLoader>	(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_MODEL]);
	Component_Shader		= static_pointer_cast<Shader>		(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_MESHINSTANCE]);
	Component_Collider		= static_pointer_cast<Collider>		(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_COLLIDER]);
	Component_Navigation	= static_pointer_cast<NavMeshAgent>	(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_NAVIGATION]);

	Component_Collider->Set_AutomaticTransform(Component_Model, Component_Transform, COLLIDER_TYPE::COLLIDER_AABB);
	Component_Collider->Set_ColliderOwner(shared_from_this());

	//GameInstance::GetInstance().Get_CollisionManager()->Register_Collider(Component_Collider);
	GameInstance::GetInstance().Get_RayCaster()->Register_Recipient(shared_from_this());
	return S_OK;
}
VOID MapObject::Update(const _float& _DT) {
	Component_Collider->Update(_DT);
	auto IntancedMeshList = Component_Model->Get_InstanceMeshList();
	for (auto& IM : *IntancedMeshList) {
		IM->Get_BoundingBox()->Update_SubMesh(_DT, Component_Transform->Get_WorldMatrix());
	}
}
VOID MapObject::Late_Update(const _float& _DT) {
	GameInstance::GetInstance().Get_RenderManager()->Add_InstancedObject(static_pointer_cast<MapObject>(shared_from_this()));
}
HRESULT		MapObject::Render() {

	return S_OK;   
}

unique_ptr<MapObject>	MapObject::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX, const wstring& _FilePath, uint32_t _LevelIndex) {
	auto Instance = unique_ptr<MapObject>(new MapObject(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType(_FilePath, _LevelIndex))) {
		MSG_BOX("Cannot Create MapObject.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>	MapObject::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<MapObject>(new MapObject(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone MapObject.");
		return nullptr;
	}
	return Instance;
}