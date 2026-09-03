#include "../Default/PCH.h"
#include "GameInstance.h"

PlayerCamera::PlayerCamera(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : Camera(_GRPDEV, _DEVCTX){}
PlayerCamera::PlayerCamera(CONST PlayerCamera& _PRTOBJ) : Camera(_PRTOBJ), MouseControl_Enable(false){}
PlayerCamera::~PlayerCamera()	{}

HRESULT	PlayerCamera::Initialize_ProtoType() {

	return S_OK;
}

HRESULT PlayerCamera::Initialize(VOID* _ARG) {
	GamePlayer			= static_pointer_cast<Player>(GameInstance::GetInstance().Get_GamePlayer());
	PlayerTransform		= static_pointer_cast<Transform>(GamePlayer->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
	CameraPositionBone	= static_pointer_cast<MeshLoader>(GamePlayer->Find_Component(COMPONENT_TYPE::COMPONENT_MODEL))->Get_ModelRootBone();

	XMVECTOR CameraStartPoint	= XMVectorSet(0.04f, 23.f, -0.f, 1.f);
	XMVECTOR CameraGazePoint	= XMVectorSet(-1.f, 0.f, 0.f, 1.f);

	GameInstance::GetInstance().Register_Camera(static_pointer_cast<Camera>(shared_from_this()));
	
	if (FAILED(__super::Initialize(nullptr)))									return E_FAIL;
	if (FAILED(__super::CameraViewSetting(CameraStartPoint, CameraGazePoint)))	return E_FAIL;

	MouseControl_Enable = true;
	CameraDistance = 17.f;

	CameraShake_OriginTime = 0.f;
	CameraShake_Time = 0.f;
	CameraShake_OriginStrength = 0.f;
	CameraShake_Strength = 0.f;

	CameraAngle = { 0.f, 0.f };
	SmoothCameraValue = { 0.f, 0.f };

	TimeScale = GameInstance::GetInstance().Get_TimeManager()->Get_TimeScale();

	return S_OK;
}

VOID	PlayerCamera::Priority_Update(CONST _float& _DT) {
	if (GameInstance::GetInstance().Get_MainCamera() == shared_from_this()) {
		if (*TimeScale < 1.f) {
			TimeScaleStorage = *TimeScale;
			*TimeScale = 1.f;
		}

		if (!CameraFallBack) {
			Camera_Controller(_DT);

			Fixed_MousePointer();
		}
		else {
			Camera_FallBack(_DT);
		}
		Camera_ShakeUpdate(_DT);
		Camera_DropUpdate(_DT);
		Camera_FocusUpdate(_DT);

		Camera::Update_CameraView();

		if (TimeScaleStorage > 0.f) {
			*TimeScale = TimeScaleStorage;
			TimeScaleStorage = 0.f;
		}
	}
}
VOID	PlayerCamera::Update(CONST _float& _DT) {

}
VOID	PlayerCamera::Late_Update(CONST _float& _DT) {
	if (GameInstance::GetInstance().Get_MainCamera() == shared_from_this()) {
		Update_FrustumCulling(_DT);
	}
}
HRESULT	PlayerCamera::Render() {
	return S_OK;
}
VOID	PlayerCamera::Camera_Controller(const _float& _DT) {

	XMFLOAT4 CameraLook;
	XMStoreFloat4(&CameraLook, CameraPositionBone->Get_ComBinedTransform().r[3] + PlayerTransform->Get_WorldPosition());

	if (FirstInitialize) {
		TargetLookY = CameraLook.y;
		FirstInitialize = false;
	}
	else {
		TargetLookY += (CameraLook.y - TargetLookY) * _DT * 6.f;
	}

	// Camera Look
	CameraLook.y = TargetLookY +  12.f * 0.28f;	CameraLook.w = 1.f;
	Component_Transform->LookAt_Target(XMLoadFloat4(&CameraLook));

	_float Camera_RotationX = 0.f, Camera_RotationY = 0.f, MouseSensor = 0.01f;
	ImGuiIO& io = ImGui::GetIO();
	if(MouseControl_Enable == true){
		_float Smoothness = 12.f;

		CameraAngle.x += static_cast<_float>(MOUSE_MOVE(MOUSEMOVESTATE::MMS_X)) * MouseSensor;
		CameraAngle.y += static_cast<_float>(MOUSE_MOVE(MOUSEMOVESTATE::MMS_Y)) * MouseSensor;

		CameraAngle.y = clamp(CameraAngle.y, -8.f, 2.f);

		// 각도 보간
		SmoothCameraValue.x += (CameraAngle.x - SmoothCameraValue.x) * _DT * Smoothness;
		SmoothCameraValue.y += (CameraAngle.y - SmoothCameraValue.y) * _DT * Smoothness;

		XMMATRIX SpringArmRotation	= XMMatrixRotationRollPitchYaw(SmoothCameraValue.y / 10.f, -SmoothCameraValue.x / 10.f, 0.f);
		XMVECTOR SpringArmDirection = XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f), SpringArmRotation);
		// 위치 보간
		XMVECTOR CameraPosition = XMVectorLerp(Component_Transform->Get_WorldPosition(), XMLoadFloat4(&CameraLook) + (SpringArmDirection * CameraDistance), _DT * Smoothness);
		Component_Transform->Set_WorldPosition(CameraPosition);
	}

	if (MOUSE_WHEEL_FRONT && !io.WantCaptureMouse && CameraDistance > 1.f)		CameraDistance -= 0.1f;
	if (MOUSE_WHEEL_BACK  && !io.WantCaptureMouse && CameraDistance < 20.f)		CameraDistance += 0.1f;

	return;
}
VOID PlayerCamera::Fixed_MousePointer() {

	if (KEY_DOWN(DIK_F1))
		MouseControl_Enable ? MouseControl_Enable = FALSE : MouseControl_Enable = TRUE;

	if (MouseControl_Enable) {
		RECT rc{};
		GetWindowRect(GameInstance::GetInstance().Get_GraphicDevice()->Get_EngineDescription().hWnd, &rc);
		SetCursorPos(rc.left + (1280 / 2), rc.top + (720 / 2));
	}
}
VOID PlayerCamera::Camera_FallBack(const _float& _DT) {
	if (CameraFallBack_Initialize) {
		CameraDistance = 3.3f;
		CameraAngle = { 0.f, 0.f };
		CameraFallBack_Initialize = false;
	}
	XMFLOAT4 CameraLook;
	XMStoreFloat4(&CameraLook, CameraPositionBone->Get_ComBinedTransform().r[3] + PlayerTransform->Get_WorldPosition());
		CameraLook.w = 1.f;

	CameraFallBack_Weight += _DT * 1.f;
	if (CameraFallBack_Weight <= 1.f) {
		_float AccerlationWeight = 1.f - (_float)pow((1.f - CameraFallBack_Weight), 3);
		CameraLook.y += 1.2f;
		CameraDistance = 3.3f + (15.f - 3.3f) * AccerlationWeight;
	}
	else if (CameraFallBack_Weight > 1.f && CameraFallBack_Weight <= 1.5f) {
		_float AccerlationWeight = 2.f * (_float)pow((CameraFallBack_Weight - 1.f), 2.f);
		_float CamY = 1.2f + (2.4f - 1.2f) * (CameraFallBack_Weight - 1.f);
		CameraLook.y += CamY;
		CameraDistance = 15.f + (10.f - 15.f) * AccerlationWeight;
	}
	else if (CameraFallBack_Weight > 1.5f && CameraFallBack_Weight <= 2.f) {
		_float AccerlationWeight = 1.f - (_float)pow(1 - (CameraFallBack_Weight - 1.f), 2.f) * 2.f;
		_float CamY = 1.2f + (2.4f - 1.2f) * (CameraFallBack_Weight - 1.f);
		CameraLook.y += CamY;
		CameraDistance = 15.f + (10.f - 15.f) * AccerlationWeight;
	}
	else if (CameraFallBack_Weight > 2.f) {
		CameraDistance = 10.f;
		CameraFallBack_Weight = 0.f;
		CameraFallBack = false;

		CameraAngle = { 0.f, 0.f };
		SmoothCameraValue = { 0.f, 0.f };

		XMFLOAT4 CameraLook;
		XMStoreFloat4(&CameraLook, CameraPositionBone->Get_ComBinedTransform().r[3] + PlayerTransform->Get_WorldPosition());
		TargetLookY = CameraLook.y;

		return;
	}

	Component_Transform->LookAt_Target(XMLoadFloat4(&CameraLook));
	Component_Transform->Set_WorldPosition(XMLoadFloat4(&CameraLook) + CameraDistance * PlayerTransform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK));
}
VOID PlayerCamera::Stop_CameraShake() {
	CameraShake_OriginTime = CameraShake_Time = 0.f;
	CameraShake_OriginStrength = CameraShake_Strength = 0.f;
}
VOID PlayerCamera::Camera_Shake(_float _Duration, _float _Strength) {
	CameraShake_OriginTime = CameraShake_Time = _Duration;
	CameraShake_OriginStrength = CameraShake_Strength = _Strength;
}
VOID PlayerCamera::Camera_ShakeUpdate(const _float& _DT) {
	if (CameraShake_Time >= 0.f) {
		_float RandomPosX = ((float)rand() / RAND_MAX) * 2.f - 1.f;
		_float RandomPosY = ((float)rand() / RAND_MAX) * 2.f - 1.f;
		_float RandomPosZ = ((float)rand() / RAND_MAX) * 2.f - 1.f;

		auto Vec = Component_Transform->Get_WorldPosition();
		Component_Transform->Set_WorldPosition(XMVectorGetX(Vec) + RandomPosX * CameraShake_Strength,
			XMVectorGetY(Vec) + RandomPosY * CameraShake_Strength, XMVectorGetZ(Vec) + RandomPosZ * CameraShake_Strength);

		CameraShake_Time -= _DT;
		CameraShake_Strength = CameraShake_OriginStrength * (CameraShake_Time / CameraShake_OriginTime);
	}
	else if (CameraShake_Time < 0.f || CameraShake_Strength < 0.f) {
		CameraShake_Time = CameraShake_OriginTime = 0.f;
		CameraShake_Strength = CameraShake_OriginStrength = 0.f;
	}
}
VOID PlayerCamera::Stop_CameraDrop() {
	CameraDrop_Time = 0.f;
}
VOID PlayerCamera::Camera_Drop(_float _Duration, XMFLOAT3 _Strength, XMFLOAT3 _CamOffset) {
	CameraDrop_Time = _Duration;
	CameraOffset = _CamOffset;
	Velocity = _Strength;
}

VOID PlayerCamera::Camera_DropUpdate(const _float& _DT) {
	if (CameraDrop_Time >= 0.f) {
		XMFLOAT3 Acceleration = (-Stiffness * CameraOffset) - (DampingValue * Velocity);
		Velocity = Velocity + Acceleration * _DT;
		CameraOffset = CameraOffset + Velocity * _DT;

		Component_Transform->Set_WorldPosition(Component_Transform->Get_WorldPosition() + XMLoadFloat3(&CameraOffset));

		CameraDrop_Time -= _DT;
	}
}

VOID PlayerCamera::EaseOut_FOV(_float _Duration, _float _Power, FOVPROGRESS _FTYPE) {
	if		(_FTYPE == FOVPROGRESS::FOV_IN) {
		FOVInTime = _Duration;
	}
	else if (_FTYPE == FOVPROGRESS::FOV_OUT) {
		FOVOutTime = _Duration;
	}
	FOVPower = _Power;
}

VOID PlayerCamera::Camera_FocusUpdate(const _float& _DT) {
	if		(FOVInTime > 0.f && FOVCounter <= FOVInTime) {
		FOVCounter += _DT;
		Set_CameraDistance(CameraDistance - FOVInTime * EASEOUT(FOVCounter / FOVInTime, FOVPower));
	}
	else if (FOVOutTime > 0.f && FOVCounter <= FOVOutTime) {
		FOVCounter += _DT;
		Set_CameraDistance(CameraDistance + FOVOutTime * EASEOUT(FOVCounter / FOVOutTime, FOVPower));
	}
	else {
		FOVInTime  = 0.f;
		FOVOutTime = 0.f;
		FOVCounter = 0.f;
	}
}

VOID PlayerCamera::Update_FrustumCulling(const _float& _DT) {
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
				if (CameraFrustum.Contains(*static_pointer_cast<AABBCollider>(TargetCollider->Get_ColliderBox())->Get_ColliderBox().get()) == DISJOINT) { 
					++iter;
					continue;
				}

				FrustumCulling_SubMesh((*iter));
				(*iter)->Late_Update(_DT);
				++iter;
			}
		}
	}
}
VOID PlayerCamera::FrustumCulling_SubMesh(shared_ptr<GameObject> _GOBJ) {
	shared_ptr<MeshLoader> ParentMesh = static_pointer_cast<MeshLoader>(_GOBJ->Find_Component(COMPONENT_TYPE::COMPONENT_MODEL));

	uint32_t MeshCount = 0;
	ANIMATION_TYPE ATYPE = ParentMesh->Get_AnimationType();
	if (ATYPE == ANIMATION_TYPE::NON_ANIMATION) {
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
unique_ptr<PlayerCamera>	PlayerCamera::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<PlayerCamera>(new PlayerCamera(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create PlayerCamera.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>		PlayerCamera::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<PlayerCamera>(new PlayerCamera(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone PlayerCamera");
		return nullptr;
	}
	return Instance;
}