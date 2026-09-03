#pragma once
#include "ParentCollider.h"
#include "Transform.h"
#include "Bone.h"

BEGIN(Engine)
class GameObject;
class MeshLoader;
class ENGINE_DLL OBBCollider : public ParentCollider {
private:
	OBBCollider();
public:
	virtual ~OBBCollider() {};

public:
	virtual HRESULT		Initialize_ProtoType()		override;
	virtual HRESULT		Initialize(void* _ARG)		override;
	virtual	VOID		Update(CONST FLOAT& _DT)	override;

#ifdef _DEBUG
	virtual VOID		Render_DebugLine(shared_ptr<PrimitiveBatch<VertexPositionColor>> _BATCH, XMVECTOR _COLOR) override;
#endif

public:
	virtual VOID		Set_AutomaticTransform(shared_ptr<MeshLoader> _MSLCMP, shared_ptr<Transform> _TRSCMP) override;
	virtual _bool		Check_OnCollision(shared_ptr<ParentCollider> _Col, COLLIDER_TYPE _CTYPE) override;

	VOID				Set_ColliderBoxVolume(XMVECTOR _MAX, XMVECTOR _MIN, XMFLOAT4 _RotQuat = { 0.f, 0.f, 0.f, 1.f });

	virtual VOID		Register_TransformComponent(shared_ptr<Transform> _TRSCMP) { Component_Transform = _TRSCMP; }

	shared_ptr<BoundingOrientedBox>	Get_ColliderBox() { return ColliderBox; }

	static	shared_ptr<OBBCollider>	Create();

private:
	shared_ptr<BoundingOrientedBox>		ColliderBox = { nullptr };
	shared_ptr<BoundingOrientedBox>		OriginalBox = { nullptr };

	shared_ptr<Transform>				Component_Transform = { nullptr };

	XMVECTOR							Collider_Offset		= {};
};
END