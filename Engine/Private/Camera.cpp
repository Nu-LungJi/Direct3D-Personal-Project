#include "Camera.h"
#include "GameInstance.h"

Camera::Camera(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) {}
Camera::Camera(CONST Camera& _PRTOBJ) : GameObject(_PRTOBJ){}

HRESULT	Camera::Initialize_ProtoType() {

	return S_OK;
}
HRESULT Camera::Initialize(VOID* _ARG) {
	uint32_t SceneIndex = GameInstance::GetInstance().Get_CurrentSceneIndex();

	ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_TRANSFORM] = static_pointer_cast<Transform>(GameInstance::GetInstance().Get_ProtoManager()->Clone_ProtoType(SceneIndex, COMPONENT_TYPE::COMPONENT_TRANSFORM, nullptr));
	Component_Transform = static_pointer_cast<Transform>(ComponentList[(uint32_t)COMPONENT_TYPE::COMPONENT_TRANSFORM]);

	XMStoreFloat4x4(&ViewMatrix	  , XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix	  , XMMatrixIdentity());
	XMStoreFloat4x4(&InvViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&InvProjMatrix, XMMatrixIdentity());

	return S_OK;
}
VOID	Camera::Priority_Update(CONST _float& _DT) {

}
VOID	Camera::Update(CONST _float& _DT) {

}
VOID	Camera::Late_Update(CONST _float& _DT) {

}
HRESULT	Camera::Render() {
	return S_OK;
}

HRESULT Camera::CameraViewSetting(XMVECTOR _StartPos, XMVECTOR _LookVec, _float _FOV, _float _Near, _float _Far) {

	ENGINE_DESC EngineOption = GameInstance::GetInstance().Get_EngineOption();
	Near = _Near;	Far = _Far;		FOV = XMConvertToRadians(_FOV); 
	Aspect = EngineOption.WindowResolutionX / EngineOption.WindowResolutionY;

	Component_Transform->Set_WorldTransform(VECTOR_TYPE::VECTOR_POS, _StartPos);
	Component_Transform->LookAt_Target(_LookVec);

	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(FOV, EngineOption.WindowResolutionX / EngineOption.WindowResolutionY, Near, Far));

	Update_CameraView();

	return S_OK;
}
VOID Camera::Update_CameraView() {
	XMStoreFloat4x4(&ViewMatrix, XMMatrixInverse(nullptr, Component_Transform->Get_WorldMatrix()));

	XMStoreFloat4x4(&InvViewMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&ViewMatrix)));
	XMStoreFloat4x4(&InvProjMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&ProjMatrix)));

	memcpy(&CameraPosition, &InvViewMatrix._41, sizeof(CameraPosition));
}
VOID Camera::Update_InverseMatrix() {
	XMStoreFloat4x4(&InvViewMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&ViewMatrix)));
	XMStoreFloat4x4(&InvProjMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&ProjMatrix)));

	memcpy(&CameraPosition, &InvViewMatrix._41, sizeof(CameraPosition));
}
VOID Camera::Set_CameraViewMatrix(XMMATRIX _ViewMat) {
	XMStoreFloat4x4(&ViewMatrix, _ViewMat);
}
VOID Camera::Set_CameraProjMatrix(XMMATRIX _ProjMat) {
	XMStoreFloat4x4(&ProjMatrix, _ProjMat);
}
unique_ptr<Camera>		Camera::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<Camera>(new Camera(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create Camera.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>	Camera::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<Camera>(new Camera(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone Camera");
		return nullptr;
	}
	return Instance;
}