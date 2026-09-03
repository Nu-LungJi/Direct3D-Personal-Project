#include "Transform.h"

Transform::Transform(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : Component(_GRPDEV, _DEVCTX)		{	}
Transform::Transform(CONST Transform& _PRTOBJ) : Component(_PRTOBJ) {}
Transform::~Transform() {}

HRESULT	 Transform::Initialize_ProtoType() {

	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	return S_OK;
}
HRESULT	 Transform::Initialize(VOID* _ARG) {

	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

unique_ptr<Transform>	Transform::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<Transform>(new Transform(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create Transform");
		return nullptr;
	}
	return Instance;
}
shared_ptr<Component>	Transform::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<Transform>(new Transform(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone Transform");
		return nullptr;
	}
	return Instance;
}

XMVECTOR Transform::Get_WorldTransform(VECTOR_TYPE _TRS) {
	return XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&WorldMatrix.m[static_cast<uint32_t>(_TRS)][0]));
}
XMVECTOR Transform::Get_WorldPosition() {
	return XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&WorldMatrix.m[static_cast<uint32_t>(VECTOR_TYPE::VECTOR_POS)][0]));
}
XMVECTOR Transform::Get_WorldRotation() {
	XMVECTOR RVec = XMVector3Normalize(Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT));
	XMVECTOR UVec = XMVector3Normalize(Get_WorldTransform(VECTOR_TYPE::VECTOR_UP));
	XMVECTOR LVec = XMVector3Normalize(Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK));

	XMMATRIX RotationMat = XMMatrixIdentity();
	RotationMat.r[0] = RVec;	RotationMat.r[1] = UVec;	RotationMat.r[2] = LVec;

	_float Pitch = 0.f, Yaw = 0.f, Roll = 0.f;
	Pitch = asinf(-XMVectorGetZ(RotationMat.r[1]));

	if (cosf(Pitch) > 0.0001f) {
		Yaw  = atan2f(XMVectorGetZ(RotationMat.r[0]), XMVectorGetZ(RotationMat.r[2]));
		Roll = atan2f(XMVectorGetX(RotationMat.r[1]), XMVectorGetY(RotationMat.r[1]));
	}
	else {		// ZimberLock
		Yaw  = atan2f(-XMVectorGetX(RotationMat.r[2]), XMVectorGetX(RotationMat.r[0]));
		Roll = 0.f;
	}

	if (WorldRotation.x > 180) { Pitch = 360 - fabsf(XMConvertToDegrees(Pitch)); }
	else { Pitch = fabsf(XMConvertToDegrees(Pitch)); }
	if (WorldRotation.y > 180) { Yaw = 360 - fabsf(XMConvertToDegrees(Yaw)); }
	else { Yaw = fabsf(XMConvertToDegrees(Yaw)); }
	if (WorldRotation.z > 180) { Roll = 360 - fabsf(XMConvertToDegrees(Roll)); }
	else { Roll = fabsf(XMConvertToDegrees(Roll)); }

	XMVECTOR ReturnVec = XMVectorSet(Pitch, Yaw, Roll, 0.f);

	return ReturnVec;
}
XMVECTOR Transform::Get_WorldScale() {
	XMVECTOR	ReturnVec = XMVectorSet(
		XMVectorGetX(XMVector3Length(Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT))),
		XMVectorGetX(XMVector3Length(Get_WorldTransform(VECTOR_TYPE::VECTOR_UP	 ))),
		XMVectorGetX(XMVector3Length(Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK ))),
		1.f
	);
	return ReturnVec;
}

XMVECTOR Transform::Get_WorldRotationQuat() {
	XMVECTOR RVec = XMVector3Normalize(Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT));
	XMVECTOR UVec = XMVector3Normalize(Get_WorldTransform(VECTOR_TYPE::VECTOR_UP));
	XMVECTOR LVec = XMVector3Normalize(Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK));

	XMMATRIX RotationMat = XMMatrixIdentity();
	RotationMat.r[0] = RVec;
	RotationMat.r[1] = UVec;
	RotationMat.r[2] = LVec;

	return XMQuaternionRotationMatrix(RotationMat);
}
VOID Transform::Set_WorldRotationQuat(XMVECTOR _Quat) {
	XMVECTOR WorldScale = Get_WorldScale();

	XMMATRIX RotationMat = XMMatrixRotationQuaternion(_Quat);

	Set_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT, RotationMat.r[0] * XMVectorGetX(WorldScale));
	Set_WorldTransform(VECTOR_TYPE::VECTOR_UP, RotationMat.r[1] * XMVectorGetY(WorldScale));
	Set_WorldTransform(VECTOR_TYPE::VECTOR_LOOK, RotationMat.r[2] * XMVectorGetZ(WorldScale));
}

VOID Transform::Set_WorldTransform(VECTOR_TYPE _TRS, XMVECTOR _StoreVec) {
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&WorldMatrix.m[static_cast<uint32_t>(_TRS)][0]), _StoreVec);
}

HRESULT  Transform::Bine_ShaderResource(shared_ptr<Shader> _CMPShader, const string& _ShaderVariableName) {
	return _CMPShader->Bind_Matrix(_ShaderVariableName, &WorldMatrix);
}

VOID Transform::Set_WorldScale	 (_float _SX, _float _SY, _float _SZ) {
	WorldScale = { _SX, _SY, _SZ, 0.f };

	Set_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT, XMVector3Normalize(Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT)) * _SX);
	Set_WorldTransform(VECTOR_TYPE::VECTOR_UP	, XMVector3Normalize(Get_WorldTransform(VECTOR_TYPE::VECTOR_UP))	* _SY);
	Set_WorldTransform(VECTOR_TYPE::VECTOR_LOOK	, XMVector3Normalize(Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK))	* _SZ);
}
VOID Transform::Set_WorldPosition(_float _PX, _float _PY, _float _PZ) {
	XMVECTOR	SaveVector = XMVectorSet(_PX, _PY, _PZ, 1.f);

	WorldPosition = { _PX, _PY, _PZ, 1.f };

	Set_WorldTransform(VECTOR_TYPE::VECTOR_POS, SaveVector);
	
}
VOID Transform::Set_WorldRotation(_float _RX, _float _RY, _float _RZ) {
	WorldRotation = { _RX, _RY, _RZ, 0.f };

	XMVECTOR WorldScale		= Get_WorldScale();
	XMFLOAT3 ScaleVector	= { XMVectorGetX(WorldScale),XMVectorGetY(WorldScale),XMVectorGetZ(WorldScale) };

	XMVECTOR	StateVec[]	= { 
		{ 1.f * ScaleVector.x, 0.f, 0.f, 0.f },
		{ 0.f, 1.f * ScaleVector.y, 0.f, 0.f },
		{ 0.f, 0.f, 1.f * ScaleVector.z, 0.f }
	};

	XMVECTOR	RPYRotationVec = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(_RX), XMConvertToRadians(_RY), XMConvertToRadians(_RZ));

	XMMATRIX	QuaternionMat = XMMatrixRotationQuaternion(RPYRotationVec);

	Set_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT, XMVector3TransformNormal(StateVec[(uint32_t)VECTOR_TYPE::VECTOR_RIGHT], QuaternionMat));
	Set_WorldTransform(VECTOR_TYPE::VECTOR_UP	, XMVector3TransformNormal(StateVec[(uint32_t)VECTOR_TYPE::VECTOR_UP]	, QuaternionMat));
	Set_WorldTransform(VECTOR_TYPE::VECTOR_LOOK	, XMVector3TransformNormal(StateVec[(uint32_t)VECTOR_TYPE::VECTOR_LOOK]	, QuaternionMat));
}

VOID Transform::Set_WorldScale	 (XMVECTOR _VEC) {
	XMStoreFloat4(&WorldScale, _VEC);

	Set_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT, XMVector3Normalize(Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT)) * XMVectorGetX(_VEC));
	Set_WorldTransform(VECTOR_TYPE::VECTOR_UP, XMVector3Normalize(Get_WorldTransform(VECTOR_TYPE::VECTOR_UP)) * XMVectorGetY(_VEC));
	Set_WorldTransform(VECTOR_TYPE::VECTOR_LOOK, XMVector3Normalize(Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK)) * XMVectorGetZ(_VEC));
}
VOID Transform::Set_WorldPosition(XMVECTOR _VEC) {
	XMStoreFloat4(&WorldPosition, _VEC);

	Set_WorldTransform(VECTOR_TYPE::VECTOR_POS, _VEC);
}
VOID Transform::Set_WorldRotation(XMVECTOR _VEC) {
	WorldRotation = { XMVectorGetX(_VEC), XMVectorGetY(_VEC), XMVectorGetZ(_VEC), 0.f};

	XMVECTOR WorldScale = Get_WorldScale();
	XMFLOAT3 ScaleVector = { XMVectorGetX(WorldScale),XMVectorGetY(WorldScale),XMVectorGetZ(WorldScale) };

	XMVECTOR	StateVec[] = {
		{ 1.f * ScaleVector.x, 0.f, 0.f, 0.f },
		{ 0.f, 1.f * ScaleVector.y, 0.f, 0.f },
		{ 0.f, 0.f, 1.f * ScaleVector.z, 0.f }
	};

	XMVECTOR	RPYRotationVec = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(XMVectorGetX(_VEC)), XMConvertToRadians(XMVectorGetY(_VEC)), XMConvertToRadians(XMVectorGetZ(_VEC)));

	XMMATRIX	QuaternionMat = XMMatrixRotationQuaternion(RPYRotationVec);

	Set_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT, XMVector3TransformNormal(StateVec[(uint32_t)VECTOR_TYPE::VECTOR_RIGHT], QuaternionMat));
	Set_WorldTransform(VECTOR_TYPE::VECTOR_UP, XMVector3TransformNormal(StateVec[(uint32_t)VECTOR_TYPE::VECTOR_UP], QuaternionMat));
	Set_WorldTransform(VECTOR_TYPE::VECTOR_LOOK, XMVector3TransformNormal(StateVec[(uint32_t)VECTOR_TYPE::VECTOR_LOOK], QuaternionMat));
}

VOID Transform::Multiply_WorldScale(_float _SX, _float _SY, _float _SZ) {
	Set_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT, Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT) * _SX);
	Set_WorldTransform(VECTOR_TYPE::VECTOR_UP	, Get_WorldTransform(VECTOR_TYPE::VECTOR_UP)	* _SY);
	Set_WorldTransform(VECTOR_TYPE::VECTOR_LOOK	, Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK)	* _SZ);

}
VOID Transform::Append_WorldPosition(DIRECTION_TYPE _AXIS, _float _SpeedVec, _float _DT) {
	XMVECTOR Position = Get_WorldTransform(VECTOR_TYPE::VECTOR_POS);

	if		(_AXIS == DIRECTION_TYPE::FORWARD) { Position += XMVector3Normalize(Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK)) * _SpeedVec; }
	else if (_AXIS == DIRECTION_TYPE::BACKWARD) { Position -= XMVector3Normalize(Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK)) * _SpeedVec; }
	else if (_AXIS == DIRECTION_TYPE::RIGHT) { Position += XMVector3Normalize(Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT)) * _SpeedVec; }
	else if (_AXIS == DIRECTION_TYPE::LEFT) { Position -= XMVector3Normalize(Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT)) * _SpeedVec; }
	else if (_AXIS == DIRECTION_TYPE::UP) { Position += XMVector3Normalize(Get_WorldTransform(VECTOR_TYPE::VECTOR_UP)) * _SpeedVec; }
	else if (_AXIS == DIRECTION_TYPE::DOWN) { Position -= XMVector3Normalize(Get_WorldTransform(VECTOR_TYPE::VECTOR_UP)) * _SpeedVec; }
	
	Set_WorldTransform(VECTOR_TYPE::VECTOR_POS, Position);
}
VOID Transform::Append_WorldRotation(XMVECTOR _AXIS, _float _SpeedAgl, _float _DT) {
	XMVECTOR	StateVec[]	= {
		Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT),
		Get_WorldTransform(VECTOR_TYPE::VECTOR_UP),
		Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK)
	};

	XMMATRIX	RotationMat = XMMatrixRotationAxis(_AXIS, XMConvertToRadians(_SpeedAgl) * _DT);

	Set_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT, XMVector3TransformNormal(StateVec[(uint32_t)VECTOR_TYPE::VECTOR_RIGHT], RotationMat));
	Set_WorldTransform(VECTOR_TYPE::VECTOR_UP	, XMVector3TransformNormal(StateVec[(uint32_t)VECTOR_TYPE::VECTOR_UP]	, RotationMat));
	Set_WorldTransform(VECTOR_TYPE::VECTOR_LOOK	, XMVector3TransformNormal(StateVec[(uint32_t)VECTOR_TYPE::VECTOR_LOOK]	, RotationMat));
}
VOID Transform::LookAt_Target(XMVECTOR _TARGET) {
	XMVECTOR	LookVec = _TARGET - Get_WorldTransform(VECTOR_TYPE::VECTOR_POS);
	XMVECTOR	RightVec = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), LookVec);
	XMVECTOR	UpVec = XMVector3Cross(LookVec, RightVec);

	XMVECTOR	ScaleVec = Get_WorldScale();

	Set_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT, XMVector3Normalize(RightVec)	* XMVectorGetX(ScaleVec));
	Set_WorldTransform(VECTOR_TYPE::VECTOR_UP, XMVector3Normalize(UpVec)		* XMVectorGetY(ScaleVec));
	Set_WorldTransform(VECTOR_TYPE::VECTOR_LOOK, XMVector3Normalize(LookVec)	* XMVectorGetZ(ScaleVec));
}
VOID Transform::Chase_Target(XMVECTOR _TARGET, _float _Speed, _float _DT, _float _MaxChase) {
	XMVECTOR	Position = Get_WorldTransform(VECTOR_TYPE::VECTOR_POS);
	XMVECTOR	MoveDirection = _TARGET - Position;

	_float Distance = XMVectorGetX(XMVector3Length(MoveDirection));

	if (Distance > _MaxChase)	Position += XMVector3Normalize(MoveDirection) * _DT * _Speed;

	Set_WorldTransform(VECTOR_TYPE::VECTOR_POS, Position);
}
