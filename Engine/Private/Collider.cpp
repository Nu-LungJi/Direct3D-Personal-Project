#include "Collider.h"
#include "GameInstance.h"
#include "AABBCollider.h"
#include "OBBCollider.h"

Collider::Collider(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : Component(_GRPDEV, _DEVCTX){}
Collider::Collider(CONST Collider& _PRT) : Component(_PRT), ColliderBox(_PRT.ColliderBox), ColliderType(_PRT.ColliderType), ColliderOwner(_PRT.ColliderOwner){}
Collider::~Collider() {}

HRESULT	Collider::Initialize_ProtoType() {


	return S_OK;
}
HRESULT Collider::Initialize(void* _ARG) {
	
	return S_OK;
	
}
VOID	Collider::Update(const _float& _DT) {
	ColliderBox->Update(_DT);
}
#ifdef _DEBUG
VOID	Collider::Render_DebugLine(shared_ptr<PrimitiveBatch<VertexPositionColor>> _BATCH, XMVECTOR _COLOR) {
	ColliderBox->Render_DebugLine(_BATCH, _COLOR);
}
#endif

VOID	Collider::Set_AutomaticTransform(shared_ptr<MeshLoader> _MeshLoader, shared_ptr<Transform> _Transform, COLLIDER_TYPE _ARG) {
	Create_ColliderBox(_ARG);
	ColliderBox->Set_AutomaticTransform(_MeshLoader, _Transform);
}

shared_ptr<ParentCollider> Collider::Create_ColliderBox(COLLIDER_TYPE _ARG) {
	if (nullptr == ColliderBox) {
		ColliderType = _ARG;

		if (_ARG == COLLIDER_TYPE::COLLIDER_AABB)		{ ColliderBox = AABBCollider::Create(); }
		else if (_ARG == COLLIDER_TYPE::COLLIDER_OBB)	{ ColliderBox = OBBCollider::Create(); }
		//else if (_ARG == COLLIDER_TYPE::COLLIDER_SPHERE) { ColliderBox = SphereCollider::Create(); }
	}

	return ColliderBox;
}

_bool Collider::Evaluate_OnCollision(shared_ptr<Collider> _Col, COLLIDER_TYPE _CTYPE) {
	return ColliderBox->Check_OnCollision(_Col->Get_ColliderBox(), _CTYPE);
}
VOID Collider::Register_TransformComponent(shared_ptr<Transform> _TRSCMP) {
	ColliderBox->Register_TransformComponent(_TRSCMP);
}
unique_ptr<Collider>	Collider::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<Collider>(new Collider(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create Collider");
		return nullptr;
	}
	return Instance;
}
shared_ptr<Component>	Collider::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<Collider>(new Collider(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone Collider");
		return nullptr;
	}
	return Instance;
}