#pragma once
#include "Scene.h"

class MainScene : public Scene {
private:
	MainScene(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	virtual ~MainScene();

public:
	virtual HRESULT		Initialize();
	virtual VOID		Update(CONST _float& _DT);

	virtual HRESULT		Register_ProtoType();
	virtual HRESULT		Initialize_SceneObject();
	virtual HRESULT		Initialize_MapObject();

	static	shared_ptr<MainScene>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>	Clone(VOID* _ARG);

private:
	uint32_t		CurrentCamera = { 0 };
};

