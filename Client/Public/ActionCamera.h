#pragma once
#include "Camera.h"

class ActionCamera : public Camera {
private:
	ActionCamera(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	ActionCamera(CONST ActionCamera& _PRTOBJ);
public:
	virtual ~ActionCamera();

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

	VOID	ChangLi_CameraAction(const _float& _DT);
	VOID	Scar_Ultimate_CameraAction(const _float& _DT);

	VOID	Play_ChangLi_CutScene(uint32_t _Value) { Camera_StandByChangLi = true; ChangLi_CutScene = _Value; }

	VOID	Play_Scar_CutScene(uint32_t _Value)		{ Camera_StandByScar = true; Scar_CutScene_Ultimate = _Value; }

	VOID	Reset_ChangLi_CutScene();

	VOID	Set_CameraLook(XMVECTOR _Look) { XMStoreFloat4(&CameraLook, _Look); }
	XMVECTOR Get_CameraLook() { return XMLoadFloat4(&CameraLook); }
	XMFLOAT4* Get_RawCameraLook() { return &CameraLook; }

public:
	static	unique_ptr<ActionCamera>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>		Clone(VOID* _ARG);

private:
	BoundingFrustum			CameraFrustum;

	_bool					MouseControll_Enable;
	shared_ptr<Player>		GamePlayer;
	shared_ptr<Transform>	PlayerTransform;
	shared_ptr<Bone>		CameraPositionBone;
	_float					CameraDistance;

	_bool					Camera_StandByChangLi	= { false };
	_bool					Camera_StandByScar		= { false };

	uint32_t				ChangLi_CutScene		= { 0 };
	uint32_t				Scar_CutScene_Ultimate	= { 0 };

	shared_ptr<Monster_FinalBoss>	FinalBoss		= { nullptr };
	shared_ptr<Transform>	FinalBossTransform		= { nullptr };
	shared_ptr<Bone>		FinalBossRootBone		= { nullptr };

	_float					CameraPointWeight1	= { 0.f };
	_float					CameraPointWeight2	= { 0.f };
	_float2					CameraAngle;
	_float2					SmoothCameraValue;

	XMFLOAT4				CameraLook;
	_float2					CameraCoord;

};

