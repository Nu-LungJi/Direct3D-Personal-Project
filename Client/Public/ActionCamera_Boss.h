#pragma once
#include "Camera.h"

class ActionCamera_Boss : public Camera {
private:
	ActionCamera_Boss(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	ActionCamera_Boss(CONST ActionCamera_Boss& _PRTOBJ);
public:
	virtual ~ActionCamera_Boss();

	virtual HRESULT		Initialize_ProtoType();
	virtual HRESULT		Initialize(VOID* _ARG);
	virtual VOID		Priority_Update(CONST _float& _DT);
	virtual VOID		Update(CONST _float& _DT);
	virtual VOID		Late_Update(CONST _float& _DT);
	virtual HRESULT		Render();

	static	unique_ptr<ActionCamera_Boss>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>			Clone(VOID* _ARG);
	VOID	Fixed_MousePointer();

	VOID	Camera_Controller(const _float& _DT);

	VOID	Update_FrustumCulling(const _float& _DT);
	VOID	FrustumCulling_SubMesh(shared_ptr<GameObject> _GOBJ);

	VOID	Update_CutScene(const float& _DT);

	VOID	Activate_CutScene() { CutScene_Enable = true; }
private:
	shared_ptr<Monster_FinalBoss>	FinalBoss	= { nullptr };
	shared_ptr<Transform>	FinalBossTransform	= { nullptr };
	shared_ptr<Bone>		FinalBossRootBone	= { nullptr };
	shared_ptr<Animator>	FinalBossAnimator	= { nullptr };

	_float2					CameraAngle;
	_float2					SmoothCameraValue;

	XMFLOAT4				CameraLook;
	_float3					CameraCoord;
	_float					CameraDistance;

	_bool					MouseControll_Enable = true;
	XMFLOAT3	Camera_Rotation		= {};
	XMFLOAT3	Camera_LookVec		= {};

	_bool		CutScene_Enable		= { false };
	_float		SmoothStepWeight	= { 0.f };
	uint32_t	CutScenePhase		= { 0 };

	BoundingFrustum	CameraFrustum;
};

