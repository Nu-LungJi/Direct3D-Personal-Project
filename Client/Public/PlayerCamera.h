#pragma once
#include "Camera.h"

class PlayerCamera : public Camera {
private:
	PlayerCamera(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	PlayerCamera(CONST PlayerCamera& _PRTOBJ);
public:
	virtual ~PlayerCamera();

	virtual HRESULT		Initialize_ProtoType();
	virtual HRESULT		Initialize(VOID* _ARG);
	virtual VOID		Priority_Update(CONST _float& _DT);
	virtual VOID		Update(CONST _float& _DT);
	virtual VOID		Late_Update(CONST _float& _DT);
	virtual HRESULT		Render();

	VOID	Update_FrustumCulling(const _float& _DT);
	VOID	FrustumCulling_SubMesh(shared_ptr<GameObject> _GOBJ);

	VOID	Camera_Controller(const _float& _DT);
	VOID	Fixed_MousePointer();
	VOID	Camera_FallBack(const _float& _DT);

	VOID	Stop_CameraShake();
	VOID	Camera_Shake(_float _Duration, _float _Strength);
	VOID	Camera_ShakeUpdate(const _float& _DT);

	VOID	Stop_CameraDrop();
	VOID	Camera_Drop(_float _Duration, XMFLOAT3 _Strength, XMFLOAT3 _CamOffset);
	VOID	Camera_DropUpdate(const _float& _DT);

	VOID	Set_CameraDistance(_float _Distance) { CameraDistance = _Distance;	}
	_float	Get_CameraDistance()				 { return CameraDistance;		}

	VOID	EaseOut_FOV(_float _Duration, _float _Power, FOVPROGRESS _FTYPE);
	VOID	Camera_FocusUpdate(CONST _float& _DT);

	_bool	IsCameraShaking() { return CameraShake_Time >= 0.002f ? true : false; }
	_bool	IsCameraDroping() { return CameraDrop_Time >= 0.002f ? true : false; }

	VOID	Set_CameraFallBack(_bool _Value)	{ CameraFallBack = _Value;	}
	_bool	Get_CameraFallBack()				{ return CameraFallBack;	}

	_bool	Get_MouseControlState() { return MouseControl_Enable; }

	static	unique_ptr<PlayerCamera>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>		Clone(VOID* _ARG);

private:
	_bool					MouseControl_Enable;
	shared_ptr<Player>		GamePlayer;
	shared_ptr<Transform>	PlayerTransform;
	shared_ptr<Bone>		CameraPositionBone;
	_float					CameraDistance;

	BoundingFrustum			CameraFrustum;

	_float					CameraShake_OriginTime;
	_float					CameraShake_Time;
	_float					CameraShake_OriginStrength;
	_float					CameraShake_Strength;

	_float					CameraDrop_Time;

	_float					Stiffness = { 400.f };
	_float					DampingValue = { 40.f };
	XMFLOAT3				Velocity = { 0.f, 0.f, 0.f };
	XMFLOAT3				CameraOffset = { 0.f, 0.f, 0.f };

	_float2					CameraAngle;
	_float2					SmoothCameraValue;

	_bool					CameraFallBack = { false };
	_bool					CameraFallBack_Initialize = { true };
	_float					CameraFallBack_Weight = { 0.f };

	_float TargetLookY = 0.f;
	_bool	FirstInitialize = { true };

	_float*					TimeScale = { nullptr };
	_float					TimeScaleStorage = { 0.f };

	_float					FOVInTime = { 0.f }, FOVOutTime = { 0.f }, FOVPower = { 0.f };
	_float					FOVCounter = { 0.f };
};

