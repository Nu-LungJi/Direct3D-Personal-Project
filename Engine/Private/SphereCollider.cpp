#include "SphereCollider.h"

SphereCollider::SphereCollider() : ParentCollider() {}

HRESULT SphereCollider::Initialize_ProtoType() {


    return S_OK;
}
HRESULT SphereCollider::Initialize(void* _ARG) {


    return S_OK;
}

VOID SphereCollider::Update(const _float& _DT)
{
    return VOID();
}

VOID SphereCollider::Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> _BATCH, XMVECTOR _COLOR) {
    DX::Draw(_BATCH.get(), *ColliderBox.get(), _COLOR);
}

VOID SphereCollider::Set_AutomaticTransform(shared_ptr<MeshLoader> _MeshLoader, shared_ptr<Transform> _Transform) {
    
}

shared_ptr<SphereCollider>	SphereCollider::Create() {
    auto Instance = shared_ptr<SphereCollider>(new SphereCollider());
    if (FAILED(Instance->Initialize_ProtoType()))
        MSG_BOX("Cannot Create SphereCollider.");
	return Instance;
}
