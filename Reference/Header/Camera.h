#pragma once
#include "Engine_Define.h"
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)
class ENGINE_DLL Camera : public GameObject{
protected:
	Camera(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	Camera(CONST Camera& _PRTOBJ);
public:
	virtual ~Camera() = default;

public:
	virtual HRESULT		Initialize_ProtoType();
	virtual HRESULT		Initialize(VOID* _ARG);
	virtual VOID		Priority_Update(CONST _float& _DT);
	virtual VOID		Update(CONST _float& _DT);
	virtual VOID		Late_Update(CONST _float& _DT);
	virtual HRESULT		Render();

	static  unique_ptr<Camera>		Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>	Clone(VOID* _ARG);

	HRESULT		CameraViewSetting(XMVECTOR _StartPos, XMVECTOR _LookVec, _float _FOV = 60.f, _float _Near = 0.01f, _float _Far = 1000.f);

	VOID		Update_CameraView();
	VOID		Update_InverseMatrix();

	XMMATRIX	Get_ViewMatrix()		{ return XMLoadFloat4x4(&ViewMatrix);	 }
	XMMATRIX	Get_ProjMatrix()		{ return XMLoadFloat4x4(&ProjMatrix);	 }
	XMFLOAT4X4*	Get_RawViewMatrix()		{ return &ViewMatrix; }
	XMFLOAT4X4*	Get_RawProjMatrix()		{ return &ProjMatrix; }
	XMMATRIX	Get_InvViewMatrix()		{ return XMLoadFloat4x4(&InvViewMatrix); }
	XMMATRIX	Get_InvProjMatrix()		{ return XMLoadFloat4x4(&InvProjMatrix); }
	XMFLOAT4X4*	Get_RawInvViewMatrix()	{ return &InvViewMatrix; }
	XMFLOAT4X4*	Get_RawInvProjMatrix()	{ return &InvProjMatrix; }

	XMVECTOR	Get_CameraPosition()	{ return XMLoadFloat4(&CameraPosition);  }
	XMFLOAT4*	Get_RawCameraPosition() { return &CameraPosition; }

	VOID		Set_CameraViewMatrix(XMMATRIX _ViewMat);
	VOID		Set_CameraProjMatrix(XMMATRIX _ProjMat);

protected:
	shared_ptr<Transform>	Component_Transform;

	XMFLOAT4X4	ViewMatrix, ProjMatrix, InvViewMatrix, InvProjMatrix;

	XMFLOAT4	CameraPosition;

	_float		Near, Far, FOV, Aspect;
};
END