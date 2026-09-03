#include "../Default/PCH.h"
#include "GameInstance.h"

ActionCamera_Boss::ActionCamera_Boss(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : Camera(_GRPDEV, _DEVCTX), MouseControll_Enable(FALSE) {}
ActionCamera_Boss::ActionCamera_Boss(CONST ActionCamera_Boss& _PRTOBJ) : Camera(_PRTOBJ), MouseControll_Enable(FALSE) {}
ActionCamera_Boss::~ActionCamera_Boss() { }

HRESULT	ActionCamera_Boss::Initialize_ProtoType() {

	return S_OK;
}
HRESULT ActionCamera_Boss::Initialize(VOID* _ARG) {
	XMVECTOR CameraStartPoint	= XMVectorSet(0.04f, 23.f, -0.f, 1.f);
	XMVECTOR CameraGazePoint	= XMVectorSet(-1.f, 0.f, 0.f, 1.f);

	GameInstance::GetInstance().Register_Camera(static_pointer_cast<Camera>(shared_from_this()));

	if (FAILED(__super::Initialize(nullptr)))									return E_FAIL;
	if (FAILED(__super::CameraViewSetting(CameraStartPoint, CameraGazePoint)))	return E_FAIL;
	MouseControll_Enable = true;

	return S_OK;
}
VOID	ActionCamera_Boss::Priority_Update(CONST _float& _DT) {
	if (GameInstance::GetInstance().Get_MainCamera() == shared_from_this()) {
		Camera_Controller(_DT);
		Update_CutScene(_DT);

		Fixed_MousePointer();
		Update_CameraView();
	}
}
VOID		ActionCamera_Boss::Update(CONST _float& _DT) {

}
VOID		ActionCamera_Boss::Late_Update(CONST _float& _DT) {
	if (GameInstance::GetInstance().Get_MainCamera() == shared_from_this()) 
		Update_FrustumCulling(_DT);
}
HRESULT	ActionCamera_Boss::Render() {

	return S_OK;
}
VOID	ActionCamera_Boss::Camera_Controller(const _float& _DT) {
	_float Camera_RotationSpeed = 0.f, Camera_MoveSpeed = 0.5f, MouseSensor = 2.5f;
	ImGuiIO& io = ImGui::GetIO();

	if ((Camera_RotationSpeed = static_cast<_float>(MOUSE_MOVE(MOUSEMOVESTATE::MMS_X))) && MouseControll_Enable) {
		Component_Transform->Append_WorldRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), Camera_RotationSpeed / 10.f, MouseSensor);
	}
	if ((Camera_RotationSpeed = static_cast<_float>(MOUSE_MOVE(MOUSEMOVESTATE::MMS_Y))) && MouseControll_Enable) {
		Component_Transform->Append_WorldRotation(Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT), Camera_RotationSpeed / 10.f, MouseSensor);
	}
	if (KEY_DOWN(DIK_0)) {
		Component_Transform->Append_WorldRotation(Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK), 0.25f, 1);
	}
	if (KEY_DOWN(DIK_9)) {
		Component_Transform->Append_WorldRotation(Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK), -0.25f, 1);
	}
	if (KEY_HOLD(DIK_W) || GameInstance::GetInstance().Get_InputManager()->Get_LThumbStickAngleY() > 0.1f)		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::FORWARD, Camera_MoveSpeed, _DT);
	if (KEY_HOLD(DIK_S) || GameInstance::GetInstance().Get_InputManager()->Get_LThumbStickAngleY() < -0.1f)		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::BACKWARD, Camera_MoveSpeed, _DT);
	if (KEY_HOLD(DIK_A) || GameInstance::GetInstance().Get_InputManager()->Get_LThumbStickAngleX() < -0.1f)		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::LEFT, Camera_MoveSpeed, _DT);
	if (KEY_HOLD(DIK_D) || GameInstance::GetInstance().Get_InputManager()->Get_LThumbStickAngleX() > 0.1f)		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::RIGHT, Camera_MoveSpeed, _DT);
	if (KEY_HOLD(DIK_SPACE))	Component_Transform->Append_WorldPosition(DIRECTION_TYPE::UP, Camera_MoveSpeed, _DT);
	if (KEY_HOLD(DIK_LCONTROL)
		&& KEY_HOLD(DIK_SPACE)) Component_Transform->Append_WorldPosition(DIRECTION_TYPE::DOWN, Camera_MoveSpeed, _DT);

	if (MOUSE_WHEEL_FRONT && !io.WantCaptureMouse)		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::FORWARD, Camera_MoveSpeed * 3.f, _DT);
	if (MOUSE_WHEEL_BACK && !io.WantCaptureMouse)		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::BACKWARD, Camera_MoveSpeed * 3.f, _DT);

	if (KEY_HOLD(DIK_UP))		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::FORWARD, 0.01f, _DT);
	if (KEY_HOLD(DIK_DOWN))		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::BACKWARD, 0.01f, _DT);
	if (KEY_HOLD(DIK_LEFT))		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::LEFT, 0.01f, _DT);
	if (KEY_HOLD(DIK_RIGHT))	Component_Transform->Append_WorldPosition(DIRECTION_TYPE::RIGHT, 0.01f, _DT);
	if (KEY_HOLD(DIK_LSHIFT) && KEY_HOLD(DIK_UP))		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::UP, 0.01f, _DT);
	if (KEY_HOLD(DIK_LSHIFT) && KEY_HOLD(DIK_DOWN))		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::DOWN, 0.01f, _DT);

	return;
}
VOID	ActionCamera_Boss::Update_CutScene(const float& _DT) {
	if (CutScene_Enable) {
		
		if (nullptr == FinalBoss) {
			FinalBoss = static_pointer_cast<Monster_FinalBoss>(GameInstance::GetInstance().Get_SceneManager()->Get_GameObject(GameInstance::GetInstance().Get_CurrentSceneIndex(), "Monster_FinalBoss"));
			FinalBossTransform = static_pointer_cast<Transform>(FinalBoss->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
			FinalBossAnimator = static_pointer_cast<Animator>(FinalBoss->Find_Component(COMPONENT_TYPE::COMPONENT_ANIMATOR));
		}

		if (CutScenePhase == 0 && FinalBossAnimator->Get_CurrentAnimation()->Get_AnimationProgress() >= 0.26f) {
			CutScenePhase += 1;
			SmoothStepWeight = 0.f;
		}
		if (CutScenePhase == 1 && FinalBossAnimator->Get_CurrentAnimation()->Get_AnimationProgress() >= 0.355f) {
			CutScenePhase += 1;
			SmoothStepWeight = 0.f;
		}

		if (CutScenePhase == 0 && SmoothStepWeight < 1.f) {
			SmoothStepWeight += _DT * 0.5f;
			XMVECTOR ResultPosition = XMVectorSetW(XMVectorAdd(FinalBossTransform->Get_WorldPosition(), XMVectorSet(-9.246f, 0.319f, 8.495f, 1.f)), 1.f);
			XMVECTOR StartPosition = XMVectorSetW(XMVectorAdd(FinalBossTransform->Get_WorldPosition(), XMVectorSet(-6.630f, 12.564f, -8.338f, 1.f)), 1.f);
			XMVECTOR EndPosition = ResultPosition;

			ResultPosition = XMVectorLerp(StartPosition, EndPosition, EASEOUT(SmoothStepWeight, 2.f));

			Component_Transform->Set_WorldPosition(ResultPosition);
			Component_Transform->LookAt_Target(Component_Transform->Get_WorldPosition() + XMVectorSet(0.291f, 0.592f, -0.75f, 0.f));
		}
		if (CutScenePhase == 1 && SmoothStepWeight < 1.f) {
			SmoothStepWeight += _DT;
			XMVECTOR StartPosition = XMVectorSetW(XMVectorAdd(FinalBossTransform->Get_WorldPosition(), XMVectorSet(-1.422f, 7.298f, -4.74f, 1.f)), 1.f);
			XMVECTOR StartRotation = XMVectorSet(28.86f, 3.99f, -10.239f, 0.f);
			XMVECTOR StartLookVec = XMVectorSet(-0.017f, -0.486f, 0.873f, 0.f);

			XMVECTOR EndPosition = XMVectorSetW(XMVectorAdd(FinalBossTransform->Get_WorldPosition(), XMVectorSet(0.506f, 6.642f, -3.75f, 1.f)), 1.f);
			XMVECTOR EndRotation = XMVectorSet(28.86f, 3.99f, -10.239f, 0.f);
			XMVECTOR EndLookVec = XMVectorSet(-0.462f, -0.315f, 0.828f, 0.f);

			XMVECTOR ResultPosition = XMVectorLerp(StartPosition, EndPosition, SMOOTHSTEP(SmoothStepWeight));
			XMVECTOR ResultRotation = XMVectorLerp(StartRotation, EndRotation, SMOOTHSTEP(SmoothStepWeight));
			XMVECTOR ResultLookVec	= XMVectorLerp(StartLookVec, EndLookVec, SMOOTHSTEP(SmoothStepWeight));

			Component_Transform->Set_WorldRotation(ResultRotation);
			Component_Transform->Set_WorldPosition(ResultPosition);
			Component_Transform->LookAt_Target(Component_Transform->Get_WorldPosition() + ResultLookVec);
		}
		if (CutScenePhase == 2 && SmoothStepWeight < 1.f) {
			SmoothStepWeight += _DT;

			XMVECTOR StartPosition	= XMVectorSetW(XMVectorAdd(FinalBossTransform->Get_WorldPosition(), XMVectorSet(0.506f, 6.642f, -3.75f, 1.f)), 1.f);
			XMVECTOR StartRotation	= XMVectorSet(28.86f, 3.99f, -10.239f, 0.f);
			XMVECTOR StartLookVec	= XMVectorSet(-0.462f, -0.315f, 0.828f, 0.f);

			XMVECTOR EndPosition = XMVectorSetW(XMVectorAdd(FinalBossTransform->Get_WorldPosition(), XMVectorSet(-0.748f, 0.993f, -48.364f, 1.f)), 1.f);
			XMVECTOR EndRotation = XMVectorSet(28.86f, 3.99f, -10.239f, 0.f);
			XMVECTOR EndLookVec  = XMVectorSet(0.029f, 0.428f, 0.903f, 0.f);

			XMVECTOR ResultPosition = XMVectorLerp(StartPosition, EndPosition, EASEOUT(SmoothStepWeight, 4.f));
			XMVECTOR ResultRotation = XMVectorLerp(StartRotation, EndRotation, EASEOUT(SmoothStepWeight, 4.f));
			XMVECTOR ResultLookVec = XMVectorLerp(StartLookVec, EndLookVec, EASEOUT(SmoothStepWeight, 4.f));

			Component_Transform->Set_WorldRotation(ResultRotation);
			Component_Transform->Set_WorldPosition(ResultPosition);
			Component_Transform->LookAt_Target(Component_Transform->Get_WorldPosition() + ResultLookVec);

			// 폭발 시 도리도리 추가하기
		}
	}
}
VOID	ActionCamera_Boss::Fixed_MousePointer() {
	//if (KEY_DOWN(DIK_F1))
	//	MouseControll_Enable ? MouseControll_Enable = FALSE : MouseControll_Enable = TRUE;
	//
	//if (MouseControll_Enable) {
		RECT rc{};
		GetWindowRect(GameInstance::GetInstance().Get_GraphicDevice()->Get_EngineDescription().hWnd, &rc);
		SetCursorPos(rc.left + (1280 / 2), rc.top + (720 / 2));
	//}
}

VOID	ActionCamera_Boss::Update_FrustumCulling(const _float& _DT) {
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
VOID	ActionCamera_Boss::FrustumCulling_SubMesh(shared_ptr<GameObject> _GOBJ) {
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

unique_ptr<ActionCamera_Boss>	ActionCamera_Boss::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<ActionCamera_Boss>(new ActionCamera_Boss(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create ActionCamera_Boss.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>			ActionCamera_Boss::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<ActionCamera_Boss>(new ActionCamera_Boss(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone ActionCamera_Boss");
		return nullptr;
	}
	return Instance;
}