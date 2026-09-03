#pragma once
#include "Engine_define.h"
#include "MeshLoader.h"
#include "Transform.h"
#include "DebugDraw.h"

BEGIN(Engine)
class ENGINE_DLL ParentCollider abstract {
protected:
	ParentCollider() {};
public:
	virtual ~ParentCollider() = default;

public:
	virtual HRESULT		Initialize_ProtoType()		{ return S_OK; }
	virtual HRESULT		Initialize(VOID* _ARG)		{ return S_OK; }
	virtual VOID		Update(CONST _float& _DT)	{}

#ifdef _DEBUG
	virtual	VOID		Render_DebugLine(shared_ptr<PrimitiveBatch<VertexPositionColor>> _BATCH, XMVECTOR _COLOR)		{}
#endif

public:
	virtual VOID		Set_AutomaticTransform(shared_ptr<MeshLoader> _MeshLoader, shared_ptr<Transform> _Transform) {}

	virtual _bool		Check_OnCollision(shared_ptr<ParentCollider> _Col, COLLIDER_TYPE _CTYPE) = 0;

	virtual VOID		Register_TransformComponent(shared_ptr<Transform> _TRSCMP) {}
protected:
	XMFLOAT3	Center = {};
};

END