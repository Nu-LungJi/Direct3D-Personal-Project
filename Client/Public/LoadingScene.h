#pragma once
#include "Scene.h"
class LoadingScene : public Scene {
private:
	LoadingScene(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	virtual ~LoadingScene();

public:
	virtual HRESULT		Initialize()				override;
	virtual VOID		Update(CONST _float& _DT)	override;

	virtual HRESULT		Register_ProtoType()		override;
	virtual HRESULT		Initialize_SceneObject()	override;
	virtual HRESULT		Initialize_MapObject()		override;

	LOADING_STATE		Update_LoadingScene();

	static	shared_ptr<LoadingScene>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>		Clone(VOID* _ARG) override;

	VOID			Set_LoadingState(LOADING_STATE _State)	{ LoadingCompleted = _State; }
	LOADING_STATE	Get_LoadingState()						{ return LoadingCompleted;	 }

	VOID			Set_NextSceneIndex(uint32_t _Index)		{ NextSceneIndex = _Index;	 }
	uint32_t		Get_NextSceneIndex()					{ return NextSceneIndex;	 }

private:
	uint32_t		CurrentCamera;


	LOADING_STATE	LoadingCompleted = { LOADING_STATE::ONREADY };
	uint32_t		NextSceneIndex = { 0 };
};

