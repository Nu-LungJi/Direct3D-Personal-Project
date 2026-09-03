#include "../Default/PCH.h"
#include "GameInstance.h"

DynamicCamera::DynamicCamera(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : Camera(_GRPDEV, _DEVCTX), MouseControll_Enable(FALSE){}
DynamicCamera::DynamicCamera(CONST DynamicCamera& _PRTOBJ) : Camera(_PRTOBJ), MouseControll_Enable(FALSE) {}
DynamicCamera::~DynamicCamera(){}

HRESULT	DynamicCamera::Initialize_ProtoType() {

	return S_OK;
}

HRESULT DynamicCamera::Initialize(VOID* _ARG) {
	XMVECTOR CameraStartPoint	= XMVectorSet(12.5f, 44.f, -190.f, 1.f);
	XMVECTOR CameraGazePoint	= XMVectorSet(-1.f,  0.f,  0.f, 1.f);

	GameInstance::GetInstance().Register_Camera(static_pointer_cast<Camera>(shared_from_this()));

	if (FAILED(__super::Initialize(nullptr)))									return E_FAIL;
	if (FAILED(__super::CameraViewSetting(CameraStartPoint, CameraGazePoint)))	return E_FAIL;
	if (FAILED(GameInstance::GetInstance().Set_MainCamera(0)))					return E_FAIL;
	MouseControll_Enable = true;
	
	return S_OK;
}

VOID	DynamicCamera::Priority_Update(CONST _float& _DT) {
	if (GameInstance::GetInstance().Get_MainCamera() == shared_from_this()) {
		Camera_Controller(_DT);
		Fixed_MousePointer();
		Camera::Update_CameraView();
	}
}
VOID	DynamicCamera::Update(CONST _float& _DT) {
	
}
VOID	DynamicCamera::Late_Update(CONST _float& _DT) {
	if (GameInstance::GetInstance().Get_MainCamera() == shared_from_this()) {
		Update_FrustumCulling(_DT);
	}
}
#ifdef _DEBUG
HRESULT		DynamicCamera::Render() {
	
	GameInstance::GetInstance().Get_CollisionManager()->DebugRender_Begin();
	auto PB = GameInstance::GetInstance().Get_CollisionManager()->Get_PrimitiveBatch();
	DX::Draw(PB.get(), CameraFrustum);
	GameInstance::GetInstance().Get_CollisionManager()->DebugRender_End();

	return S_OK;
}
#endif
VOID	DynamicCamera::Camera_Controller(const _float& _DT) {
	_float Camera_RotationSpeed = 0.f, Camera_MoveSpeed = 1.5f, MouseSensor = 2.5f;
	ImGuiIO& io = ImGui::GetIO();
	
	if ((Camera_RotationSpeed = static_cast<_float>(MOUSE_MOVE(MOUSEMOVESTATE::MMS_X))) && MouseControll_Enable) {
		Component_Transform->Append_WorldRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), Camera_RotationSpeed / 10.f, MouseSensor);
	}
	if ((Camera_RotationSpeed = static_cast<_float>(MOUSE_MOVE(MOUSEMOVESTATE::MMS_Y))) && MouseControll_Enable) {
		Component_Transform->Append_WorldRotation(Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT), Camera_RotationSpeed / 10.f, MouseSensor);
	}
	if (KEY_HOLD(DIK_W) || GameInstance::GetInstance().Get_InputManager()->Get_LThumbStickAngleY() > 0.1f )		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::FORWARD	, Camera_MoveSpeed, _DT);
	if (KEY_HOLD(DIK_S) || GameInstance::GetInstance().Get_InputManager()->Get_LThumbStickAngleY() < -0.1f)		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::BACKWARD	, Camera_MoveSpeed, _DT);
	if (KEY_HOLD(DIK_A) || GameInstance::GetInstance().Get_InputManager()->Get_LThumbStickAngleX() < -0.1f)		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::LEFT, Camera_MoveSpeed, _DT);
	if (KEY_HOLD(DIK_D) || GameInstance::GetInstance().Get_InputManager()->Get_LThumbStickAngleX() > 0.1f)		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::RIGHT		, Camera_MoveSpeed, _DT);
	if (KEY_HOLD(DIK_SPACE))	Component_Transform->Append_WorldPosition(DIRECTION_TYPE::UP		, Camera_MoveSpeed, _DT);
	if (KEY_HOLD(DIK_LCONTROL) 
		&& KEY_HOLD(DIK_SPACE)) Component_Transform->Append_WorldPosition(DIRECTION_TYPE::DOWN		, Camera_MoveSpeed, _DT);

	if (MOUSE_WHEEL_FRONT && !io.WantCaptureMouse)		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::FORWARD, Camera_MoveSpeed * 3.f, _DT);
	if (MOUSE_WHEEL_BACK  && !io.WantCaptureMouse)		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::BACKWARD, Camera_MoveSpeed * 3.f, _DT);

	if (KEY_HOLD(DIK_UP))		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::FORWARD, 0.01f, _DT);
	if (KEY_HOLD(DIK_DOWN))		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::BACKWARD, 0.01f, _DT);
	if (KEY_HOLD(DIK_LEFT))		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::LEFT, 0.01f, _DT);
	if (KEY_HOLD(DIK_RIGHT))	Component_Transform->Append_WorldPosition(DIRECTION_TYPE::RIGHT, 0.01f, _DT);
	if (KEY_HOLD(DIK_LSHIFT) && KEY_HOLD(DIK_UP))		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::UP, 0.01f, _DT);
	if (KEY_HOLD(DIK_LSHIFT) && KEY_HOLD(DIK_DOWN))		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::DOWN, 0.01f, _DT);

	return;
}
VOID DynamicCamera::Update_FrustumCulling(const _float& _DT) {
	BoundingFrustum::CreateFromMatrix(CameraFrustum, XMLoadFloat4x4(&ProjMatrix));

	CameraFrustum.Transform(CameraFrustum, XMLoadFloat4x4(&InvViewMatrix));

	auto LayerList = GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Get_LayerList();
	for (auto& Layer : LayerList) {
		auto GameObjectList = Layer->Get_GameObjectList();

		for (auto iter = GameObjectList.begin(); iter != GameObjectList.end();) {
			if ((*iter)->Get_DeadState()) {
				Layer->Remove_GameObject(*iter);
				iter = GameObjectList.erase(iter);
			}
			else {
				if ((*iter) == shared_from_this()) { ++iter; continue; }

				shared_ptr<Collider> TargetCollider = static_pointer_cast<Collider>((*iter)->Find_Component(COMPONENT_TYPE::COMPONENT_COLLIDER));
				if (nullptr == TargetCollider || TargetCollider->Get_ColliderType() == COLLIDER_TYPE::COLLIDER_OBB) {
					(*iter)->Late_Update(_DT);
					{ ++iter; continue; }
				}
				if (CameraFrustum.Contains(*static_pointer_cast<AABBCollider>(TargetCollider->Get_ColliderBox())->Get_ColliderBox().get()) == DISJOINT) { ++iter; continue; }

				FrustumCulling_SubMesh((*iter));
				(*iter)->Late_Update(_DT);
				++iter;
			}
		}
	}
}
VOID DynamicCamera::FrustumCulling_SubMesh(shared_ptr<GameObject> _GOBJ) {
	shared_ptr<MeshLoader> ParentMesh = static_pointer_cast<MeshLoader>(_GOBJ->Find_Component(COMPONENT_TYPE::COMPONENT_MODEL));

	uint32_t MeshCount = 0;
	ANIMATION_TYPE ATYPE = ParentMesh->Get_AnimationType();
	if		(ATYPE == ANIMATION_TYPE::NON_ANIMATION) {
		vector<shared_ptr<StaticMesh>>* SubMeshList = ParentMesh->Get_StaticMeshList();
		MeshCount = static_cast<uint32_t>(SubMeshList->size());
	}
	else if (ATYPE == ANIMATION_TYPE::INSTANCED) {
		vector<shared_ptr<InstanceMesh>>* SubMeshList = ParentMesh->Get_InstanceMeshList();
		MeshCount = static_cast<uint32_t>(SubMeshList->size());
	}

	for (uint32_t IDX = 0; IDX < MeshCount; ++IDX) {
  		if (CameraFrustum.Contains(ParentMesh->Get_SubMeshBoundingBox(IDX)) == DISJOINT) {
			ParentMesh->Set_SubMeshRenderFlag(IDX, false);
			continue;
		}
		ParentMesh->Set_SubMeshRenderFlag(IDX, true);
	}
}
VOID DynamicCamera::Fixed_MousePointer() {
	if (KEY_DOWN(DIK_F1)) 
		MouseControll_Enable ? MouseControll_Enable = FALSE : MouseControll_Enable = TRUE;
	
	if (MouseControll_Enable) {
		RECT rc{};
		GetWindowRect(GameInstance::GetInstance().Get_GraphicDevice()->Get_EngineDescription().hWnd, &rc);
		SetCursorPos(rc.left + (1280 / 2), rc.top + (720 / 2));
	}
}
unique_ptr<DynamicCamera>	DynamicCamera::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<DynamicCamera>(new DynamicCamera(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create DynamicCamera.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>		DynamicCamera::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<DynamicCamera>(new DynamicCamera(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone DynamicCamera");
		return nullptr;
	}
	return Instance;
}

//카메라 매니저 - 여러 카메라를 만들어서, 메인 카메라, UI 카메라, 연출 카메라, 어떤 카메라 활성화할지 관리해준다.