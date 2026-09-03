#pragma once
#include "Scene.h"
class BossScene : public Scene {
private:
	BossScene(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	virtual ~BossScene();

public:
	virtual HRESULT		Initialize()				override;
	virtual VOID		Update(CONST _float& _DT )	override;

	virtual HRESULT		Register_ProtoType()		override;
	virtual HRESULT		Initialize_SceneObject()	override;
	virtual HRESULT		Initialize_MapObject()		override;

	static	shared_ptr<BossScene>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>	Clone(VOID* _ARG) override;

private:
	uint32_t		CurrentCamera;
};

