#include "GameInstance.h"

#include "GraphicDevice.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "SoundManager.h"
#include "CollisionManager.h"
#include "UIManager.h"
#include "GUIEditor.h"

GameInstance::GameInstance()	{}
GameInstance::~GameInstance()	{}

HRESULT GameInstance::Initialize_GameInstance(CONST ENGINE_DESC& _EDESC, CONST SCENE_DESC& _SDESC, ComPtr<ID3D11Device>& _GRPDEV, ComPtr<ID3D11DeviceContext>& _DEVCTX) {

	EngIneOption = _EDESC;
	SceneOption	 = _SDESC;

	Initialize_Manager(_EDESC, _SDESC, _GRPDEV, _DEVCTX);
	Initialize_Editor (_EDESC, _SDESC, _GRPDEV, _DEVCTX);

	GV_SpriteBatch = make_shared<SpriteBatch>(_DEVCTX.Get());
	GV_CommonState = make_shared<CommonStates>(_GRPDEV.Get());

	return S_OK;
}
VOID GameInstance::Priority_Update_Engine(CONST FLOAT& _DT) {
	Engine_SceneManager->Priority_Update_SceneManager(_DT);
}
VOID GameInstance::Update_Engine(CONST FLOAT& _DT) {
	Engine_InputManager->Update_InputManager();

	Engine_EffectManager->Update_EffectManager(_DT);

	Engine_SceneManager->Update_SceneManager(_DT);

	Engine_CollisionManager->Update_CollisionManager(_DT);

	Engine_Tool_MeshGenerator->Generator_Controller();

	Engine_UIManager->Update_UIManager(_DT);

	Engine_SoundManager->Update_SoundManager();
}
VOID GameInstance::LateUpdate_Engine(CONST FLOAT& _DT) {
	//Engine_SceneManager->Late_Update_SceneManager(_DT);
	Engine_CollisionManager->Late_Update_CollisionManager(_DT);
	if (nullptr != MainCamera)	MainCamera->Late_Update(0.f);
}
VOID GameInstance::Render_Engine() {
	Engine_GraphicDevice->Clear_RenderTargetView();
	Engine_GraphicDevice->Clear_DepthStencilView();

	Engine_RenderManager->Render_ByGroup();
#ifdef _DEBUG
	//Engine_Tool_Navigator->Render_NavMeshCell();
#endif
	Engine_Tool_RayCaster->Render();
	Engine_UIManager->Render_UIManager();

	//MainCamera->Render();
}
VOID GameInstance::Clear_Resource(uint32_t _Level) {

	Engine_ProtoManager->Clear_ProtoType(_Level);
}

VOID GameInstance::Release_Engine() {

	Engine_Tool_ThreadPool.reset();
	Engine_Tool_GUIEditor.reset();
	Engine_Tool_MapEditor.reset();
	Engine_Tool_RayCaster.reset();
	Engine_Tool_MeshGenerator.reset();
	Engine_Tool_Navigator.reset();
	Engine_Tool_Utility.reset();

	Engine_UIManager.reset();
	Engine_InputManager.reset();
	Engine_TimeManager.reset();
	Engine_ProtoManager.reset();
	Engine_SceneManager.reset();
	Engine_RenderManager.reset();
	Engine_EffectManager.reset();
	Engine_TextureManager.reset();
	Engine_CollisionManager.reset();
	Engine_RenderTargetManager.reset();
	Engine_LightManager.reset();
	Engine_SoundManager.reset();

	Engine_GraphicDevice->Release_GRPDEV();
	Engine_GraphicDevice.reset();
}

VOID GameInstance::Initialize_Manager(CONST ENGINE_DESC& _EDESC, CONST SCENE_DESC& _SDESC, ComPtr<ID3D11Device>& _GRPDEV, ComPtr<ID3D11DeviceContext>& _DEVCTX) {
	
	Engine_GraphicDevice	= GraphicDevice	::Create(_GRPDEV, _DEVCTX, _EDESC);
	
	Engine_TimeManager		= TimeManager	::Create(_GRPDEV, _DEVCTX);

	Engine_InputManager		= InputManager	::Create(_GRPDEV, _DEVCTX);

	Engine_UIManager		= UIManager		::Create(_GRPDEV, _DEVCTX);

	Engine_SceneManager		= SceneManager	::Create(_SDESC);

	Engine_ProtoManager		= ProtoManager	::Create(_EDESC.MaxSceneCount);

	Engine_RenderTargetManager = RenderTargetManager::Create(_GRPDEV, _DEVCTX);

	Engine_RenderManager	= RenderManager	::Create(_GRPDEV, _DEVCTX);

	Engine_EffectManager	= EffectManager	::Create(_GRPDEV, _DEVCTX);

	Engine_TextureManager	= TextureManager::Create(_GRPDEV, _DEVCTX);

	Engine_CollisionManager = CollisionManager::Create(_GRPDEV, _DEVCTX);

	Engine_LightManager		= LightManager::Create(_GRPDEV, _DEVCTX);

	Engine_SoundManager		= SoundManager::Create(_GRPDEV, _DEVCTX);
}
VOID GameInstance::Initialize_Editor(CONST ENGINE_DESC& _EDESC, CONST SCENE_DESC& _SDESC, ComPtr<ID3D11Device>& _GRPDEV, ComPtr<ID3D11DeviceContext>& _DEVCTX)	{
	
	Engine_Tool_ThreadPool		= ThreadPool	::Create();
	
	Engine_Tool_GUIEditor		= GUIEditor		::Create(_GRPDEV, _DEVCTX);
	
	Engine_Tool_MapEditor		= MapEditor		::Create(_GRPDEV, _DEVCTX);
	
	Engine_Tool_RayCaster		= RayCaster		::Create(_GRPDEV, _DEVCTX);

	Engine_Tool_MeshGenerator	= MeshGenerator	::Create(_GRPDEV, _DEVCTX);

	Engine_Tool_Navigator		= Navigator		::Create(_GRPDEV, _DEVCTX);

	Engine_Tool_Utility			= Utility		::Create(_GRPDEV, _DEVCTX);
}

HRESULT GameInstance::Register_Camera(shared_ptr<Camera> _CAM){
	CameraList.push_back(_CAM); 
	SceneOption.CurrentCameraCount += 1; 

	return S_OK; 
}

HRESULT GameInstance::Set_GamePlayer(shared_ptr<GameObject> _PLY) {
	GamePlayer = _PLY;
	return S_OK;
}
