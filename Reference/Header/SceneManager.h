#pragma once
#include "Engine_Define.h"
#include "Scene.h"

BEGIN(Engine)

class ENGINE_DLL SceneManager {
private:
	SceneManager();
public:
	~SceneManager();

public:
	HRESULT		Ready_SceneManager(SCENE_DESC _DESC);
	VOID		Priority_Update_SceneManager(CONST _float& _DT);
	VOID		Update_SceneManager(CONST _float& _DT);
	VOID		Late_Update_SceneManager(CONST _float& _DT);
	VOID		Render_SceneManager();

	HRESULT		Register_Scene(shared_ptr<Scene> _NewScene, uint32_t _SceneIndex, uint32_t _MaxLayerCount);
	HRESULT		Change_Scene(uint32_t _SceneNumb);
	HRESULT		Reset_Scene(uint32_t _SceneNumb);

	HRESULT		Add_GameObject(uint32_t _SceneNumb, uint32_t _LayerNumb, shared_ptr<GameObject>	_GOBJ);

	uint32_t	Get_MaxSceneCount() { return MaxSceneCount; }
	uint32_t	Get_MaxLayerCount() { return MaxLayerCount; }

	shared_ptr<Scene>					Get_CurrentScene()		{ return CurrentScene;		}
	HRESULT								Set_CurrentScene(uint32_t _SceneIndex);

	uint32_t							Get_CurrentSceneIndex() { return CurrentSceneIndex; }

	shared_ptr<Scene>					Get_Scene(uint32_t _Index) { return SceneList[_Index]; }

	shared_ptr<Layer>					Get_Layer(uint32_t _SceneNumb, uint32_t _LayerNumb) { return SceneList[_SceneNumb]->Get_Layer(_LayerNumb); }
	vector<shared_ptr<Layer>>			Get_LayerList(uint32_t _SceneNumb) { return SceneList[_SceneNumb]->Get_LayerList(); }

	shared_ptr<GameObject>				Get_GameObject(uint32_t _SceneNumb, string _OBJTAG) { return SceneList[_SceneNumb]->Get_GameObject(_OBJTAG); }
	list<shared_ptr<GameObject>>		Get_GameObjectList(uint32_t _SceneNumb, uint32_t _LayerNumb) { return SceneList[_SceneNumb]->Get_GameObjectList(_LayerNumb); }

	HRESULT								Remove_GameObject(string _OBJTAG);
	HRESULT								Remove_GameObject(shared_ptr<GameObject> _OBJ);

	static	unique_ptr<SceneManager>	Create(SCENE_DESC _DESC);

private:
	uint32_t			CurrentSceneIndex;
	shared_ptr<Scene>	CurrentScene;

	uint32_t			MaxSceneCount;
	uint32_t			MaxLayerCount;

	vector<shared_ptr<Scene>>	SceneList;
};

END