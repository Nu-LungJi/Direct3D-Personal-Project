#pragma once
#include "Engine_Define.h"

#include "GraphicDevice.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "SoundManager.h"
#include "CollisionManager.h"
#include "ProtoManager.h"
#include "UIManager.h"
#include "RenderManager.h"
#include "EffectManager.h"
#include "TextureManager.h"
#include "RenderTargetManager.h"
#include "LightManager.h"

#include "ThreadPool.h"
#include "GUIEditor.h"
#include "MapEditor.h"
#include "RayCaster.h"
#include "MeshGenerator.h"
#include "Navigator.h"
#include "Utility.h"

#include "Camera.h"

BEGIN(Engine)

class ENGINE_DLL GameInstance {
	COMPONENT_SINGLETON(GameInstance)
private:
	GameInstance();
public:
	~GameInstance();

public:
	HRESULT Initialize_GameInstance(CONST ENGINE_DESC& _EDESC, CONST SCENE_DESC& _SDESC, ComPtr<ID3D11Device>& _GRPDEV, ComPtr<ID3D11DeviceContext>& _DEVCTX);

	VOID	Priority_Update_Engine(CONST FLOAT& _DT);
	VOID	Update_Engine(CONST FLOAT& _DT);
	VOID	LateUpdate_Engine(CONST FLOAT& _DT);	
	VOID	Render_Engine();
	VOID	Clear_Resource(uint32_t _Level);
	VOID	Release_Engine();

	VOID	Initialize_Manager(CONST ENGINE_DESC& _EDESC, CONST SCENE_DESC& _SDESC, ComPtr<ID3D11Device>& _GRPDEV, ComPtr<ID3D11DeviceContext>& _DEVCTX);
	VOID	Initialize_Editor(CONST ENGINE_DESC& _EDESC, CONST SCENE_DESC& _SDESC, ComPtr<ID3D11Device>& _GRPDEV, ComPtr<ID3D11DeviceContext>& _DEVCTX);

public:
	//////////////////////////////////////// Manager ////////////////////////////////////////
	GraphicDevice		*Get_GraphicDevice()	 { return Engine_GraphicDevice		.get();}
	TimeManager			*Get_TimeManager()		 { return Engine_TimeManager		.get();}
	InputManager		*Get_InputManager()		 { return Engine_InputManager		.get();}
	SoundManager		*Get_SoundManager()		 { return Engine_SoundManager		.get();}
	SceneManager		*Get_SceneManager()		 { return Engine_SceneManager		.get();}
	CollisionManager	*Get_CollisionManager()	 { return Engine_CollisionManager	.get();}
	UIManager			*Get_UIManager()		 { return Engine_UIManager			.get();}
	ProtoManager		*Get_ProtoManager()		 { return Engine_ProtoManager		.get();}
	RenderManager		*Get_RenderManager()	 { return Engine_RenderManager		.get();}
	EffectManager		*Get_EffectManager()	 { return Engine_EffectManager		.get();}
	TextureManager		*Get_TextureManager()	 { return Engine_TextureManager		.get();}
	RenderTargetManager *Get_RenderTargetManager(){ return Engine_RenderTargetManager.get();}
	LightManager		*Get_LightManager()		 { return Engine_LightManager		.get();}
	//////////////////////////////////////// Editor ////////////////////////////////////////
	ThreadPool			*Get_ThreadPool()		 { return Engine_Tool_ThreadPool	.get();}
	GUIEditor			*Get_GUIEditor()		 { return Engine_Tool_GUIEditor		.get();}
	MapEditor			*Get_MapEditor()		 { return Engine_Tool_MapEditor		.get();}
	RayCaster			*Get_RayCaster()		 { return Engine_Tool_RayCaster		.get();}
	MeshGenerator		*Get_MeshGenerator()	 { return Engine_Tool_MeshGenerator	.get();}
	Navigator			*Get_Navigator()		 { return Engine_Tool_Navigator		.get();}
	Utility				*Get_Utility()			 { return Engine_Tool_Utility		.get();}
	///////////////////////////////////////// Data /////////////////////////////////////////
	HRESULT					Register_Camera(shared_ptr<Camera> _CAM);

	shared_ptr<Camera>		Get_MainCamera()							{ return MainCamera;   }
	shared_ptr<Camera>		Get_CameraByIndex(uint32_t _CamIDX)			{ return CameraList[_CamIDX]; }
	HRESULT					Set_MainCamera(uint32_t _CamIDX)			{ MainCamera = CameraList[_CamIDX]; return S_OK; }

	shared_ptr<GameObject>	Get_GamePlayer()							{ return GamePlayer; }
	HRESULT					Set_GamePlayer(shared_ptr<GameObject> _PLY);

	CONST ENGINE_DESC&		Get_EngineOption()							{ return EngIneOption;   }
	CONST SCENE_DESC&		Get_SceneOption()							{ return SceneOption;    }

	shared_ptr<SpriteBatch> Get_SpriteBatch()							{ return GV_SpriteBatch; }
	shared_ptr<CommonStates>Get_CommonState()							{ return GV_CommonState; }

	uint32_t				Get_CurrentSceneIndex()						{ return SceneOption.CurrentSceneIndex; }
	VOID					Set_CurrentSceneIndex(uint32_t _CSI)		{ SceneOption.CurrentSceneIndex = _CSI; }

private:
	unique_ptr<GraphicDevice>		Engine_GraphicDevice		= { nullptr };
	unique_ptr<TimeManager>			Engine_TimeManager			= { nullptr };
	unique_ptr<InputManager>		Engine_InputManager			= { nullptr };
	unique_ptr<SoundManager>		Engine_SoundManager			= { nullptr };
	unique_ptr<SceneManager>		Engine_SceneManager			= { nullptr };
	unique_ptr<CollisionManager>	Engine_CollisionManager		= { nullptr };
	unique_ptr<UIManager>			Engine_UIManager			= { nullptr };
	unique_ptr<ProtoManager>		Engine_ProtoManager			= { nullptr };
	unique_ptr<RenderManager>		Engine_RenderManager		= { nullptr };
	unique_ptr<EffectManager>		Engine_EffectManager		= { nullptr };
	unique_ptr<TextureManager>		Engine_TextureManager		= { nullptr };
	unique_ptr<RenderTargetManager>	Engine_RenderTargetManager  = { nullptr };
	unique_ptr<LightManager>		Engine_LightManager			= { nullptr };

	unique_ptr<ThreadPool>			Engine_Tool_ThreadPool		= { nullptr };
	unique_ptr<GUIEditor>			Engine_Tool_GUIEditor		= { nullptr };
	unique_ptr<MapEditor>			Engine_Tool_MapEditor		= { nullptr };
	unique_ptr<RayCaster>			Engine_Tool_RayCaster		= { nullptr };
	unique_ptr<MeshGenerator>		Engine_Tool_MeshGenerator	= { nullptr };
	unique_ptr<Navigator>			Engine_Tool_Navigator		= { nullptr };
	unique_ptr<Utility>				Engine_Tool_Utility			= { nullptr };

	vector<shared_ptr<Camera>>		CameraList		= {};
	shared_ptr<Camera>				MainCamera		= { nullptr };
	shared_ptr<GameObject>			GamePlayer		= { nullptr };

	ENGINE_DESC						EngIneOption	= {};
	SCENE_DESC						SceneOption		= {};

	shared_ptr<SpriteBatch>			GV_SpriteBatch = { nullptr };
	shared_ptr<CommonStates>		GV_CommonState = { nullptr };

};
END
