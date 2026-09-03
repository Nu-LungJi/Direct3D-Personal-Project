#include "OBBCollider.h"
#include "GameInstance.h"

OBBCollider::OBBCollider() : ParentCollider() {}

HRESULT OBBCollider::Initialize_ProtoType() {
    Collider_Offset = XMVectorSet(0.f, 0.f, 0.f, 0.f);

    return S_OK;
}
HRESULT OBBCollider::Initialize(void* _ARG) {


    return S_OK;
}

VOID OBBCollider::Update(const FLOAT& _DT) {
	if (Component_Transform != nullptr) {
		XMMATRIX TransformMatrix = Component_Transform->Get_WorldMatrix();

		if (XMVector3LengthSq(Collider_Offset).m128_f32[0] > 0) {
			TransformMatrix.r[3] = XMVectorAdd(TransformMatrix.r[3], Collider_Offset);
		}
		BoundingOrientedBox OBB;
		OriginalBox->Transform(OBB, TransformMatrix);

		*ColliderBox.get() = OBB;
	}
}
#ifdef _DEBUG
VOID OBBCollider::Render_DebugLine(shared_ptr<PrimitiveBatch<VertexPositionColor>> _BATCH, XMVECTOR _COLOR) {
	DX::Draw(_BATCH.get(), *ColliderBox.get(), _COLOR);
}
#endif
VOID	OBBCollider::Set_AutomaticTransform(shared_ptr<MeshLoader> _MSLCMP, shared_ptr<Transform> _TRSCMP) {
    Component_Transform = _TRSCMP;

    XMFLOAT3 Center, Extents;
    XMStoreFloat3(&Center , (_MSLCMP->Get_CoordVertexMax() + _MSLCMP->Get_CoordVertexMin()) / 2.f);
    XMStoreFloat3(&Extents, (_MSLCMP->Get_CoordVertexMax() - _MSLCMP->Get_CoordVertexMin()) / 2.f);

	XMFLOAT4 RotationQuat = { 0.f, 0.f, 0.f, 1.f };
	
    OriginalBox = make_shared<BoundingOrientedBox>(Center, Extents, RotationQuat);
	ColliderBox = make_shared<BoundingOrientedBox>(*OriginalBox);
}

VOID OBBCollider::Set_ColliderBoxVolume(XMVECTOR _MAX, XMVECTOR _MIN, XMFLOAT4 _RotQuat) {
	XMFLOAT3 Center, Extents;
	XMStoreFloat3(&Center, (_MAX + _MIN) / 2.f);
	XMStoreFloat3(&Extents, (_MAX - _MIN) / 2.f);

	OriginalBox = make_shared<BoundingOrientedBox>(Center, Extents, _RotQuat);
	ColliderBox = make_shared<BoundingOrientedBox>(*OriginalBox);
}
_bool OBBCollider::Check_OnCollision(shared_ptr<ParentCollider> _Col, COLLIDER_TYPE _CTYPE) {
	_bool Result = false;

	if (_CTYPE == COLLIDER_TYPE::COLLIDER_AABB)
		Result = ColliderBox->Intersects(*static_pointer_cast<AABBCollider>(_Col)->Get_ColliderBox().get());
	if (_CTYPE == COLLIDER_TYPE::COLLIDER_OBB)
		Result = ColliderBox->Intersects(*static_pointer_cast<OBBCollider>(_Col)->Get_ColliderBox().get());

	return Result;
}

shared_ptr<OBBCollider>	 OBBCollider::Create() {
    auto Instance = shared_ptr<OBBCollider>(new OBBCollider());
    if (FAILED(Instance->Initialize_ProtoType())) {
        MSG_BOX("Cannot Create OBBCollider.");
        return nullptr;
    }
	return Instance;
}