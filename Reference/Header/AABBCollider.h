#pragma once
#include "ParentCollider.h"
#include "Transform.h"
#include "Bone.h"

BEGIN(Engine)
class GameObject;
class MeshLoader;
class ENGINE_DLL AABBCollider : public ParentCollider {
private:
	AABBCollider();
public:
	virtual ~AABBCollider();

public:
	virtual	HRESULT	Initialize_ProtoType()		override;
	virtual	HRESULT	Initialize(VOID* _ARG)		override;
	virtual	VOID	Update(CONST FLOAT& _DT)	override;
#ifdef _DEBUG
	virtual	VOID	Render_DebugLine(shared_ptr<PrimitiveBatch<VertexPositionColor>> _BATCH, XMVECTOR _COLOR)	override;
#endif
	virtual VOID	Set_AutomaticTransform(shared_ptr<MeshLoader> _MSLCMP, shared_ptr<Transform> _TRSCMP)		override;

	virtual _bool	Check_OnCollision(shared_ptr<ParentCollider> _Col, COLLIDER_TYPE _CTYPE)					override;

	VOID			Set_ColliderBoxVolume(XMVECTOR _MAX, XMVECTOR _MIN);

	virtual VOID		Register_TransformComponent(shared_ptr<Transform> _TRSCMP) { Component_Transform = _TRSCMP; }
public:
	shared_ptr<BoundingBox>	Get_ColliderBox()									{ return ColliderBox;			 }
	XMFLOAT3				Get_ColliderCenter()								{ return OriginalBox->Center;	 }

	VOID					Set_ColliderOffset(XMVECTOR _Offset)				{ Collider_Offset = _Offset;	 }

	VOID	Update_SubMesh(const _float& _DT, XMMATRIX _WorldMatrix);
public:
	static		shared_ptr<AABBCollider>	Create();

private:
	shared_ptr<BoundingBox>			ColliderBox			= { nullptr };
	shared_ptr<BoundingBox>			OriginalBox			= { nullptr };

	shared_ptr<Transform>			Component_Transform = { nullptr };

	XMVECTOR						Collider_Offset = {};
	_bool							StaticMeshFlag = { false };
};
END