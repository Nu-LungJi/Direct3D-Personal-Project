#pragma once
#include "Component.h"
#include "Shader.h"
#include "NavMeshAgent.h"

BEGIN(Engine)
class ENGINE_DLL Transform : public Component {
private:
	Transform(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	Transform(CONST Transform& _PRTOBJ);
public:
	virtual ~Transform();

public:
	virtual		HRESULT	Initialize_ProtoType();
	virtual		HRESULT	Initialize(VOID* _ARG);

	static		unique_ptr<Transform>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual		shared_ptr<Component>	Clone(VOID* _ARG);

public:
	XMVECTOR	Get_WorldTransform(VECTOR_TYPE _TRS);
	VOID		Set_WorldTransform(VECTOR_TYPE _TRS, XMVECTOR _StoreVec);

	XMMATRIX	Get_WorldMatrix()				{ return XMLoadFloat4x4(&WorldMatrix); }
	VOID		Set_WorldMatrix(XMMATRIX _Mat)	{ XMStoreFloat4x4(&WorldMatrix, _Mat); }

	XMVECTOR	Get_WorldPosition();
	XMVECTOR	Get_WorldRotation();
	XMVECTOR	Get_WorldScale();

	XMVECTOR	Get_WorldRotationQuat();
	VOID		Set_WorldRotationQuat(XMVECTOR _Quat);

	HRESULT		Bine_ShaderResource(shared_ptr<Shader> _CMPShader, CONST string& _ShaderVariableName);

public:
	// 초기화 후, 지정된 위치로 이동.
	VOID		Set_WorldScale	 (_float _SX, _float _SY, _float _SZ);
	VOID		Set_WorldPosition(_float _PX, _float _PY, _float _PZ);
	VOID		Set_WorldRotation(_float _RX, _float _RY, _float _RZ);

	// 초기화 후, 지정된 위치로 이동.
	VOID		Set_WorldScale	 (XMVECTOR _VEC);
	VOID		Set_WorldPosition(XMVECTOR _VEC);
	VOID		Set_WorldRotation(XMVECTOR _VEC);

	// 현재 상태에서 X, Y, Z 값으로 배율
	VOID		Multiply_WorldScale(_float _SX, _float _SY, _float _SZ);

	// 현재 상태에서 DeltaTime에 따라 누적. (Scale은 Multiply_WorldScale로 사용)
	VOID		Append_WorldPosition(DIRECTION_TYPE _AXIS, _float _SpeedVec, _float _DT);
	VOID		Append_WorldRotation(XMVECTOR _AXIS, _float _SpeedAgl, _float _DT);

	VOID		LookAt_Target(XMVECTOR _TARGET);
	VOID		Chase_Target(XMVECTOR _TARGET, _float _Speed, _float _DT, _float _MaxChase = 0.f);

private:
	XMFLOAT4X4	WorldMatrix		= { };
	XMFLOAT4	WorldPosition	= { };
	XMFLOAT4	WorldRotation	= { };
	XMFLOAT4	WorldScale		= { };
};

END