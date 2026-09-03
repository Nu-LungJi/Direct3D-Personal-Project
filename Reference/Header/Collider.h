#pragma once
#include "Component.h"
#include "MeshLoader.h"
#include "Transform.h"	
#include "ParentCollider.h"

BEGIN(Engine)

class ENGINE_DLL Collider : public Component {
protected:
	Collider(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	Collider(CONST Collider& _PRT);

public:
	virtual ~Collider();

public:
	virtual HRESULT		Initialize_ProtoType();
	virtual HRESULT		Initialize(VOID* _ARG);

	virtual VOID		Update(CONST _float& _DT);

#ifdef _DEBUG
	VOID							Render_DebugLine(shared_ptr<PrimitiveBatch<VertexPositionColor>> _BATCH, XMVECTOR _COLOR);
#endif
	VOID							Set_AutomaticTransform(shared_ptr<MeshLoader> _MeshLoader, shared_ptr<Transform> _Transform, COLLIDER_TYPE _ARG);

	shared_ptr<ParentCollider>		Create_ColliderBox(COLLIDER_TYPE _ARG);

	_bool							Evaluate_OnCollision(shared_ptr<Collider> _Col, COLLIDER_TYPE _CTYPE);

	VOID							Register_TransformComponent(shared_ptr<Transform> _TRSCMP);

public:
	shared_ptr<ParentCollider>		Get_ColliderBox()									{ return ColliderBox;		}
	COLLIDER_TYPE					Get_ColliderType()									{ return ColliderType;		}

	VOID							Set_ColliderOwner(shared_ptr<GameObject> _Owner)	{ ColliderOwner = _Owner;	}
	shared_ptr<GameObject>			Get_ColliderOwner()									{ return ColliderOwner;		}

	VOID							Set_ColliderActiveState(_bool _Value)				{ ColliderActive = _Value;	}
	_bool							Get_ColliderActiveState()							{ return ColliderActive;	}

	vector<shared_ptr<Collider>>*	Get_ColliderList()									{ return &ColliderList;		}

public:
	static	unique_ptr<Collider>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual	shared_ptr<Component>	Clone(VOID* _ARG);

private:
	shared_ptr<ParentCollider>				ColliderBox;
	COLLIDER_TYPE							ColliderType;
	_bool									ColliderActive = { true };

	vector<shared_ptr<Collider>>			ColliderList;
	shared_ptr<GameObject>					ColliderOwner = { nullptr };
};

END