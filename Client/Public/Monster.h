#pragma once
#include "GameObject.h"
#include "BehaviorTree.h"

class Monster : public GameObject {
private:
	Monster(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	virtual ~Monster();

	virtual HRESULT			Initialize_ProtoType();
	virtual HRESULT			Initialize(VOID* _ARG);
	virtual VOID			Update(_float _DT);
	virtual HRESULT			Render();

public:
	static	unique_ptr<Monster>		Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>	Clone(VOID* _ARG);

private:
	shared_ptr<Buffer>			Component_Buffer;
	shared_ptr<MeshLoader>		Component_Model;
	shared_ptr<Transform>		Component_Transform;
	shared_ptr<Animator>		Component_Animator;
	shared_ptr<AABBCollider>	Component_BoundingBox;
	shared_ptr<Shader>			Component_Shader;
	shared_ptr<TexBuffer>		Component_Texture;
};

