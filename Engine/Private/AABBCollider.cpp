#include "AABBCollider.h"
#include "OBBCollider.h"

AABBCollider::AABBCollider() : ParentCollider() {}
AABBCollider::~AABBCollider() {}

HRESULT	AABBCollider::Initialize_ProtoType() {
	Collider_Offset = XMVectorSet( 0.f, 0.f, 0.f, 0.f );

	return S_OK;
}
HRESULT AABBCollider::Initialize(VOID* _ARG) {

	return S_OK;
}

VOID AABBCollider::Update(const FLOAT& _DT) {
	//if (StaticMeshFlag) return;

	if (Component_Transform != nullptr) {
		XMMATRIX TransformMatrix = Component_Transform->Get_WorldMatrix();

		if (XMVector3LengthSq(Collider_Offset).m128_f32[0] > 0) {
			TransformMatrix.r[3] = XMVectorAdd(TransformMatrix.r[3], Collider_Offset);
		}
		BoundingBox AABB;
		OriginalBox->Transform(AABB, TransformMatrix);

		*ColliderBox.get() = AABB;

		//StaticMeshFlag = true;
	}
}
VOID AABBCollider::Update_SubMesh(const FLOAT& _DT, XMMATRIX _WorldMatrix) {

		BoundingBox AABB;
		XMMATRIX LocalMatrix = XMMatrixTranslation(Center.x, Center.y, Center.z);
		
		OriginalBox->Transform(AABB, LocalMatrix * _WorldMatrix);

		*ColliderBox.get() = AABB;
}
VOID	AABBCollider::Render_DebugLine(shared_ptr<PrimitiveBatch<VertexPositionColor>> _BATCH, XMVECTOR _COLOR) {
	DX::Draw(_BATCH.get(), *ColliderBox.get(), _COLOR);
}

VOID AABBCollider::Set_AutomaticTransform(shared_ptr<MeshLoader> _MSLCMP, shared_ptr<Transform> _TRSCMP) {
	Component_Transform = _TRSCMP;

	XMFLOAT3 Center, Extents;
	XMStoreFloat3(&Center, (_MSLCMP->Get_CoordVertexMax() + _MSLCMP->Get_CoordVertexMin()) / 2.f);
	XMStoreFloat3(&Extents, (_MSLCMP->Get_CoordVertexMax() - _MSLCMP->Get_CoordVertexMin()) / 2.f);

	OriginalBox = make_unique<BoundingBox>(Center, Extents);
	ColliderBox = make_unique<BoundingBox>(*OriginalBox);
}

VOID AABBCollider::Set_ColliderBoxVolume(XMVECTOR _MAX, XMVECTOR _MIN) {
	XMFLOAT3 Center, Extents;
	XMStoreFloat3(&Center, (_MAX + _MIN) / 2.f);
	XMStoreFloat3(&Extents, (_MAX - _MIN) / 2.f);

	OriginalBox = make_unique<BoundingBox>(Center, Extents);
	ColliderBox = make_unique<BoundingBox>(*OriginalBox);
}


_bool AABBCollider::Check_OnCollision(shared_ptr<ParentCollider> _Col, COLLIDER_TYPE _CTYPE) {
	_bool Result = false;

	if (_CTYPE == COLLIDER_TYPE::COLLIDER_AABB) 
		Result = ColliderBox->Intersects(*static_pointer_cast<AABBCollider>(_Col)->Get_ColliderBox().get());
	if (_CTYPE == COLLIDER_TYPE::COLLIDER_OBB)
		Result = ColliderBox->Intersects(*static_pointer_cast<OBBCollider>(_Col)->Get_ColliderBox().get());

	return Result;
}

shared_ptr<AABBCollider>	AABBCollider::Create() {
	auto Instance = shared_ptr<AABBCollider>(new AABBCollider());
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create AABBCollider");
		return nullptr;
	}
	return Instance;
}