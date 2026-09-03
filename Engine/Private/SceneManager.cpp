#include "SceneManager.h"
#include "GameInstance.h"
SceneManager::SceneManager() : CurrentScene(nullptr), MaxSceneCount(0) {}
SceneManager::~SceneManager() {}

HRESULT SceneManager::Ready_SceneManager(SCENE_DESC _DESC) {
	MaxSceneCount = _DESC.MaxSceneCount;
	MaxLayerCount = _DESC.MaxLayerCount;

	SceneList.resize(MaxSceneCount);

	return S_OK;
}
VOID SceneManager::Priority_Update_SceneManager(const _float& _DT) {
	CurrentScene->Priority_Update(_DT);
}
VOID SceneManager::Update_SceneManager(const _float& _DT) {
	CurrentScene->Update(_DT);
}
VOID SceneManager::Late_Update_SceneManager(const _float& _DT) {
	CurrentScene->Late_Update(_DT);
}
VOID SceneManager::Render_SceneManager() {
	CurrentScene->Render();
}
HRESULT SceneManager::Register_Scene(shared_ptr<Scene> _NewScene, uint32_t _SceneIndex, uint32_t _MaxLayerCount) {
	SceneList[_SceneIndex] = _NewScene;
	SceneList[_SceneIndex]->Register_Layer(_MaxLayerCount);

	return S_OK;
}
HRESULT SceneManager::Change_Scene(uint32_t _SceneNumb) {
	SceneList[CurrentSceneIndex]->Clear_Layer();
	SceneList[CurrentSceneIndex].reset();

	GameInstance::GetInstance().Get_ProtoManager()->Clear_ProtoType(CurrentSceneIndex);

	CurrentSceneIndex = _SceneNumb;
	CurrentScene = SceneList[_SceneNumb];

	return S_OK;
}

HRESULT SceneManager::Reset_Scene(uint32_t _SceneNumb) {
	SceneList[_SceneNumb]->Clear_Layer();
	SceneList[_SceneNumb].reset();
	GameInstance::GetInstance().Get_ProtoManager()->Clear_ProtoType(_SceneNumb);

	return S_OK;
}

HRESULT SceneManager::Add_GameObject(uint32_t _SceneNumb, uint32_t _LayerNumb, shared_ptr<GameObject> _GOBJ) {
	if ( nullptr == SceneList[_SceneNumb])										return E_FAIL;
	if ( nullptr == _GOBJ )														return E_FAIL;

	if (FAILED(SceneList[_SceneNumb]->Add_GameObject(_LayerNumb, _GOBJ)))		return E_FAIL;

	return S_OK;
}

HRESULT SceneManager::Set_CurrentScene(uint32_t _SceneIndex) {

	if (SceneList.size() <= 0)	return E_FAIL;

	CurrentSceneIndex = _SceneIndex;
	CurrentScene = SceneList[_SceneIndex];

	// 현재 씬은 SceneList에서 꺼내와서 CurrentScene으로 따로 놓아둔다.
	// 씬 전환 시에 현재 씬은 CurrentSceneIndex 값 이용해서 다시 벡터에 넘겨주고,
	// 전환할 씬 인덱스 번호를 꺼내온다. // 씬리스트는 전부 Create가 되어있다고 가정하고 진행 한다.

	GameInstance::GetInstance().Set_CurrentSceneIndex(CurrentSceneIndex);

	return S_OK;
}

HRESULT SceneManager::Remove_GameObject(string _OBJTAG) {
	vector<shared_ptr<Layer>> LayerList = Get_CurrentScene()->Get_LayerList();
	for (auto& LYR : LayerList){
		if (!FAILED(LYR->Remove_GameObject(_OBJTAG))) return S_OK;
	}

	return E_FAIL;
}

HRESULT SceneManager::Remove_GameObject(shared_ptr<GameObject> _OBJ) {
	vector<shared_ptr<Layer>> LayerList = Get_CurrentScene()->Get_LayerList();
	for (auto& LYR : LayerList) {
		if (!FAILED(LYR->Remove_GameObject(_OBJ))) return S_OK;
	}

	return E_FAIL;
}

unique_ptr<SceneManager>	SceneManager::Create(SCENE_DESC _DESC) {
	auto	SM = unique_ptr<SceneManager>(new SceneManager);
	if (FAILED(SM->Ready_SceneManager(_DESC))) {
		MSG_BOX("Cannot Create SceneManager.");
		return nullptr;
	}
	return SM;
}