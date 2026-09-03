#include "Scene.h"
#include "GameInstance.h"
Scene::Scene(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) 
	: GRPDEV(_GRPDEV), DEVCTX(_DEVCTX)	{ }
Scene::~Scene()							{ }

HRESULT Scene::Initialize() {
	return S_OK;
}
VOID	Scene::Priority_Update(const _float& _DT) {
	for (auto& LYR : LayerList) 
		if (LYR != nullptr) { LYR->Priority_Update(_DT); }
}
VOID	Scene::Update(CONST _float& _DT) {
	for (auto& LYR : LayerList) 
		if (LYR != nullptr) { LYR->Update(_DT); }
}
VOID	Scene::Late_Update(const _float& _DT) {
	for (auto& LYR : LayerList)
		if (LYR != nullptr) { LYR->Late_Update(_DT); }
}
VOID	Scene::Render() {

}

HRESULT Scene::Register_ProtoType() {
	return S_OK;
}
HRESULT Scene::Initialize_SceneObject() {
	return S_OK;
}
HRESULT	Scene::Initialize_MapObject() {
	return S_OK;
}
HRESULT Scene::Register_Layer(uint32_t _MaxLayerCount) {
	for (uint32_t IDX = 0; IDX < _MaxLayerCount; ++IDX) 
		LayerList.push_back(Layer::Create());
	
	return S_OK;
}

HRESULT Scene::Clear_Layer() {
	for (auto& LYR : LayerList) 
		LYR->Clear_GameObjectList();
	LayerList.clear();

	return S_OK;
}
HRESULT Scene::Add_GameObject(uint32_t _LayerNumb, shared_ptr<GameObject> _GOBJ) {
	if (FAILED(LayerList[_LayerNumb]->Add_GameObject(_GOBJ)) || nullptr == _GOBJ) return E_FAIL;
	return S_OK;
}
HRESULT Scene::Add_ProtoType_CurrentScene(COMPONENT_TYPE _CMPTYPE, unique_ptr<Component> _CMP){
	uint32_t CurrentSceneIndex = GameInstance::GetInstance().Get_CurrentSceneIndex();

	if (FAILED(GameInstance::GetInstance().Get_ProtoManager()->Add_ProtoType(CurrentSceneIndex, _CMPTYPE, move(_CMP)))) {
		MSG_BOX("Cannot Add Component ProtoType");
		return E_FAIL;
	}
	return S_OK;
}
HRESULT Scene::Add_ProtoType_CurrentScene(const string& _PRTTAG, unique_ptr<GameObject> _OBJ) {
	uint32_t CurrentSceneIndex = GameInstance::GetInstance().Get_CurrentSceneIndex();

	if (FAILED(GameInstance::GetInstance().Get_ProtoManager()->Add_ProtoType(CurrentSceneIndex, _PRTTAG, move(_OBJ)))) {
		MSG_BOX("Cannot Add GameObject ProtoType");
		return E_FAIL;
	}
	return S_OK;
}
HRESULT Scene::Add_CloneObject(uint32_t _LayerNumb, const string& _ORGOBJ, const string& _OBJTAG, void* _ARG) {
	uint32_t CurrentSceneIndex = GameInstance::GetInstance().Get_CurrentSceneIndex();
	shared_ptr<GameObject> CLNOBJ = GameInstance::GetInstance().Get_ProtoManager()->Clone_ProtoType(CurrentSceneIndex, _ORGOBJ.c_str(), _OBJTAG.c_str(), _ARG);
	
	if (nullptr == CLNOBJ)							return E_FAIL;
	if (FAILED(Add_GameObject(_LayerNumb, CLNOBJ)))	return E_FAIL;
	
	return S_OK;
}
shared_ptr<GameObject>			 Scene::Get_GameObject(string _OBJTAG) {
	for (auto& LYR : LayerList) {
		shared_ptr<GameObject> GOBJ = LYR->Get_GameObject(_OBJTAG);
		if (nullptr != GOBJ) return GOBJ;
	}
	return nullptr;
}
list<shared_ptr<GameObject>>	 Scene::Get_GameObjectList(uint32_t _LayerNumb) {
	return LayerList[_LayerNumb]->Get_GameObjectList();
}