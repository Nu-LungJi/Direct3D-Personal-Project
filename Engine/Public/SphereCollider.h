#pragma once
#include "ParentCollider.h"

BEGIN(Engine)
class ENGINE_DLL SphereCollider : public ParentCollider {
class MeshLoader;
class Transform;
private:
	SphereCollider();
public:
	virtual ~SphereCollider() {};

public:
	virtual HRESULT		Initialize_ProtoType()		override;
	virtual HRESULT		Initialize(void* _ARG)		override;
	virtual VOID		Update(const _float& _DT)	override;
	virtual	VOID		Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> _BATCH, XMVECTOR _COLOR)		override;

public:
	virtual VOID		Set_AutomaticTransform(shared_ptr<MeshLoader> _MeshLoader, shared_ptr<Transform> _Transform);

	shared_ptr<BoundingOrientedBox>	Get_ColliderBox() { return ColliderBox; }

	static	shared_ptr<SphereCollider>	Create();

private:
	shared_ptr<BoundingOrientedBox>		ColliderBox = { nullptr };
	shared_ptr<BoundingOrientedBox>		OriginalBox = { nullptr };

	shared_ptr<Transform>		Component_Transform = { nullptr };
	shared_ptr<Bone>			ModelCoreBone		= { nullptr };

	XMVECTOR					Collider_Offset = {};
};
END