#pragma once
#include "Camera.h"

class DynamicCamera : public Camera {
private:
	DynamicCamera(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	DynamicCamera(CONST DynamicCamera& _PRTOBJ);
public:
	virtual ~DynamicCamera();

	virtual HRESULT		Initialize_ProtoType();
	virtual HRESULT		Initialize(VOID* _ARG);
	virtual VOID		Priority_Update(CONST _float& _DT);
	virtual VOID		Update(CONST _float& _DT);
	virtual VOID		Late_Update(CONST _float& _DT);
	virtual HRESULT		Render();

	VOID	Camera_Controller(const _float& _DT);
	VOID	Update_FrustumCulling(const _float& _DT);
	VOID	FrustumCulling_SubMesh(shared_ptr<GameObject> _GOBJ);
	VOID	Fixed_MousePointer();

	static	unique_ptr<DynamicCamera>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>		Clone(VOID* _ARG);

private:
	_bool	MouseControll_Enable;
	XMFLOAT3	Camera_Rotation = {};
	XMFLOAT3	Camera_LookVec = {};

	BoundingFrustum	CameraFrustum;
};

