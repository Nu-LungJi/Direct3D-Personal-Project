#include "../Default/PCH.h"
#include "GameManager.h"
#include "GameInstance.h"

GameManager::GameManager()	: GRPDEV(nullptr), DEVCTX(nullptr) {}
GameManager::~GameManager()	{ GameInstance::GetInstance().Release_Engine(); }

HRESULT GameManager::Ready_GameManager() {
	uint32_t StartSceneIndex = static_cast<uint32_t>(SCENE_TYPE::SCENE_2);

	if (FAILED(Ready_DefaultSetting()))					return E_FAIL;
	//if (FAILED(Ready_LoadingScreen(StartSceneIndex)))	return E_FAIL;
	if (FAILED(Ready_SoundList()))						return E_FAIL;

	//ScreenFilter = Create_FilterObject("../../Resource/Asset/MainUI/EventUI_Filter.png", { 1280.f, 720.f }, 1.f, 0.f);
	//Loading_Screen = Create_UIObject("../../Resource/Asset/MainUI/EventUI_Filter.png", { 1280.f, 720.f }, 1.f, 0.f);
	//Loading_Text = Create_FontObject(L"../../Resource/Asset/Font/WutheringWave_Font_Bold.spritefont", L"", { 1200.f + 22.f, 640.f + 12.f }, 0.45f, { 1.f, 1.f, 1.f }, 1.f);

	return S_OK;
}

LOADING_STATE GameManager::Check_LoadingState() {
	shared_ptr<LoadingScene> LScene = static_pointer_cast<LoadingScene>(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene());
	return LScene->Update_LoadingScene();
}
VOID GameManager::Priority_Update_GameManager(CONST FLOAT& _DT) {
	GameInstance::GetInstance().Priority_Update_Engine(_DT);
}
VOID GameManager::Update_GameManager(CONST FLOAT& _DT) {
	GameInstance::GetInstance().Update_Engine(_DT);
}
VOID GameManager::LateUpdate_GameManager(CONST FLOAT& _DT) {
	GameInstance::GetInstance().LateUpdate_Engine(_DT);
}
VOID	GameManager::Render_GameManager() {
	GameInstance::GetInstance().Render_Engine();
}
HRESULT GameManager::Ready_DefaultSetting() {

	ENGINE_DESC EDESC = {};
	EDESC.hInst = hInst;			 EDESC.hWnd = hWnd;					EDESC.WindowMode = WINMODE::WMODE_WIN;
	EDESC.WindowResolutionX = WINCX; EDESC.WindowResolutionY = WINCY;	EDESC.MaxSceneCount = static_cast<uint32_t>(SCENE_TYPE::SCENE_END);
	
	SCENE_DESC	SDESC = {};
	SDESC.MaxSceneCount = static_cast<uint32_t>(SCENE_TYPE::SCENE_END);	SDESC.MaxLayerCount = static_cast<uint32_t>(LAYER_TYPE::LAYER_END);

	if (FAILED(GameInstance::GetInstance().Initialize_GameInstance(EDESC, SDESC, GRPDEV, DEVCTX)))							return E_FAIL;
	
	if (FAILED(Register_AllScene()))	return E_FAIL;

	GameInstance::GetInstance().Get_EffectManager()->Load_EffectBundle("../../Resource/Asset/Effect/");

	//GameInstance::GetInstance().Get_EffectManager()->Play_Effect(0, { 1.2f, 47.f, -61.45f }, { 5.f, 5.f, 5.f }, 10.f);

	SetCursor(NULL);

	return S_OK;
}
UINT32 APIENTRY Thread_Main(VOID* _ARG) {
	auto GM = static_cast<GameManager*>(_ARG);
	if (FAILED(GM->Loading_GameManger()))	return 1;

	return 0;
}
HRESULT GameManager::Ready_LoadingScreen(uint32_t _StartScene){
	InitializeCriticalSection(&CRTSCT);

	SCENETYPE = _StartScene;

	Handle = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);
	if (0 == Handle)
		return E_FAIL;

	return S_OK;
}
HRESULT GameManager::Ready_SoundList(){
	if (FAILED(GameInstance::GetInstance().Get_SoundManager()->Load_SoundBundle("../../Resource/Sound/Sound_Loop", true))) return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Get_SoundManager()->Load_SoundBundle("../../Resource/Sound/Sound_SFX", false))) return E_FAIL;

	return S_OK;
}
HRESULT GameManager::Register_AllScene() {
	//if (FAILED(GameInstance::GetInstance().Get_SceneManager()->Register_Scene(LoadingScene::Create(GRPDEV, DEVCTX), (uint32_t)SCENE_TYPE::SCENE_0, (uint32_t)LAYER_TYPE::LAYER_END)))	return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Get_SceneManager()->Register_Scene(MainScene::Create(GRPDEV, DEVCTX), (uint32_t)SCENE_TYPE::SCENE_1, (uint32_t)LAYER_TYPE::LAYER_END)))		return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Get_SceneManager()->Register_Scene(BossScene::Create(GRPDEV, DEVCTX), (uint32_t)SCENE_TYPE::SCENE_2, (uint32_t)LAYER_TYPE::LAYER_END)))		return E_FAIL;

	if (FAILED(GameInstance::GetInstance().Get_SceneManager()->Set_CurrentScene((uint32_t)SCENE_TYPE::SCENE_2)))						return E_FAIL;

	if (FAILED(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Register_ProtoType()))								return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Initialize_SceneObject()))							return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Initialize_MapObject()))								return E_FAIL;
	return S_OK;
}
HRESULT GameManager::Loading_MainScene(){
	if (FAILED(GameInstance::GetInstance().Get_SceneManager()->Get_Scene(static_cast<uint32_t>(SCENE_TYPE::SCENE_1))->Register_ProtoType()))		return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Get_SceneManager()->Get_Scene(static_cast<uint32_t>(SCENE_TYPE::SCENE_1))->Initialize_SceneObject()))	return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Get_SceneManager()->Get_Scene(static_cast<uint32_t>(SCENE_TYPE::SCENE_1))->Initialize_MapObject()))		return E_FAIL;
	
	shared_ptr<LoadingScene> LScene = static_pointer_cast<LoadingScene>(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene());
	LScene->Set_LoadingState(LOADING_STATE::COMPLETE);
	LScene->Set_NextSceneIndex(static_cast<uint32_t>(SCENE_TYPE::SCENE_1));
}
HRESULT GameManager::Loading_BossScene(){

	if (FAILED(GameInstance::GetInstance().Get_SceneManager()->Get_Scene(static_cast<uint32_t>(SCENE_TYPE::SCENE_2))->Register_ProtoType()))		return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Get_SceneManager()->Get_Scene(static_cast<uint32_t>(SCENE_TYPE::SCENE_2))->Initialize_SceneObject()))	return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Get_SceneManager()->Get_Scene(static_cast<uint32_t>(SCENE_TYPE::SCENE_2))->Initialize_MapObject()))		return E_FAIL;
	
	shared_ptr<LoadingScene> LScene = static_pointer_cast<LoadingScene>(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene());
	LScene->Set_LoadingState(LOADING_STATE::COMPLETE);
	LScene->Set_NextSceneIndex(static_cast<uint32_t>(SCENE_TYPE::SCENE_2));
}
HRESULT GameManager::Loading_GameManger(){
	EnterCriticalSection(&CRTSCT);
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	shared_ptr<LoadingScene> LScene = static_pointer_cast<LoadingScene>(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene());
	LScene->Set_LoadingState(LOADING_STATE::ONLOADING);

	if (SCENETYPE == static_cast<uint32_t>(SCENE_TYPE::SCENE_1)) {
		if (FAILED(Loading_MainScene()))		return 1;
	}
	if (SCENETYPE == static_cast<uint32_t>(SCENE_TYPE::SCENE_2)) {
		if (FAILED(Loading_BossScene()))		return 1;
	}

	CoUninitialize();
	LeaveCriticalSection(&CRTSCT);

	return S_OK;
}
unique_ptr<GameManager> GameManager::Create() {
	unique_ptr<GameManager> GM = unique_ptr<GameManager>(new GameManager());
	if (FAILED(GM->Ready_GameManager())) {
		MSG_BOX("Cannot Create GameManager.");
		GM.reset();
	}
	return GM;
}
shared_ptr<FontObject> GameManager::Create_FontObject(const wstring& _FontFilePath, wstring _Text, _float2 _Position, _float _Scale, _float3 _Color, _float _Alpha) {
	uint32_t CurrentSceneIndex = GameInstance::GetInstance().Get_CurrentSceneIndex();
	string	NewTag = "";
	shared_ptr<FontObject>  FOBJ = static_pointer_cast<FontObject>(GameInstance::GetInstance().Get_ProtoManager()->Clone_ProtoType(CurrentSceneIndex, "PRT_FontObject", NewTag, nullptr));

	if (nullptr == FOBJ) {
		MSG_BOX("Cannot Create FontObject");
		assert(0);
	}

	FOBJ->Create_FontObject(_FontFilePath, _Text, _Position, _Scale, _Color, _Alpha);

	FontObjectList.push_back(FOBJ);

	return FontObjectList.back();
}
shared_ptr<UIObject>   GameManager::Create_UIObject(filesystem::path _TexRes, _float2 _Position, _float _Scale, _float _OPC, _float _Rotation) {
	uint32_t CurrentSceneIndex = GameInstance::GetInstance().Get_CurrentSceneIndex();
	string	NewTag = _TexRes.filename().string();

	shared_ptr<UIObject> CLNOBJ = static_pointer_cast<UIObject>(GameInstance::GetInstance().Get_ProtoManager()->Clone_ProtoType(CurrentSceneIndex, "PRT_UIObject", NewTag, nullptr));

	if (FAILED(CLNOBJ->Create_UIObject(_TexRes, _Position, _Scale, _OPC, _Rotation))) {
		MSG_BOX("Cannot Ready UIObject TextureState.");
		return nullptr;
	}

	UIObjectList.push_back(CLNOBJ);
	GameInstance::GetInstance().Get_GUIEditor()->Register_UIObject(CLNOBJ);

	return UIObjectList.back();
}
shared_ptr<UIObject>   GameManager::Create_FilterObject(filesystem::path _TexRes, _float2 _Position, _float _Scale, _float _OPC, _float _Rotation) {
	uint32_t CurrentSceneIndex = GameInstance::GetInstance().Get_CurrentSceneIndex();
	string	NewTag = _TexRes.filename().string();

	shared_ptr<UIObject> CLNOBJ = static_pointer_cast<UIObject>(GameInstance::GetInstance().Get_ProtoManager()->Clone_ProtoType(CurrentSceneIndex, "PRT_UIObject", NewTag, nullptr));

	if (FAILED(CLNOBJ->Create_UIObject(_TexRes, _Position, _Scale, _OPC, _Rotation))) {
		MSG_BOX("Cannot Ready UIObject TextureState.");
		return nullptr;
	}

	FilterObjectList.push_back(CLNOBJ);
	GameInstance::GetInstance().Get_GUIEditor()->Register_UIObject(CLNOBJ);

	return FilterObjectList.back();
}