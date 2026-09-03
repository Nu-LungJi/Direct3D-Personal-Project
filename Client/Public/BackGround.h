#pragma once
#include "GameObject.h"
#include "Buffer.h"
#include "Shader.h"

class BackGround :public GameObject {
private:
	BackGround(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	BackGround(const BackGround& _PRTOBJ);
public:
	virtual ~BackGround();

public:
	virtual HRESULT		Initialize_ProtoType()		 override;
	virtual HRESULT		Initialize(VOID* _ARG)		 override;
	virtual VOID		Priority_Update(_float	_DT) override;
	virtual VOID		Update(_float _DT)			 override;
	virtual VOID		Late_Update(_float _DT)		 override;
	virtual HRESULT		Render()					 override;

private:
	HRESULT		Ready_Components();
private:
	shared_ptr<Shader>	Component_Shader;
	shared_ptr<Buffer>	Component_Buffer;

public:
	static	unique_ptr<BackGround>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>	Clone(VOID* _ARG);
};

