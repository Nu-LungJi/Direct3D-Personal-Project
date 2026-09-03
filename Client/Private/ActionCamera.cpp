#include "../Default/PCH.h"
#include "GameInstance.h"
#include "PlayerCamera.h"

ActionCamera::ActionCamera(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : Camera(_GRPDEV, _DEVCTX) {}
ActionCamera::ActionCamera(CONST ActionCamera& _PRTOBJ) : Camera(_PRTOBJ), MouseControll_Enable(false) {}
ActionCamera::~ActionCamera() {}


HRESULT	ActionCamera::Initialize_ProtoType() {

	return S_OK;
}

HRESULT ActionCamera::Initialize(VOID* _ARG) {
	GamePlayer			= static_pointer_cast<Player>(GameInstance::GetInstance().Get_GamePlayer());
	PlayerTransform		= static_pointer_cast<Transform>(GamePlayer->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
	CameraPositionBone	= static_pointer_cast<MeshLoader>(GamePlayer->Find_Component(COMPONENT_TYPE::COMPONENT_MODEL))->Get_ModelPelvisBone();

	XMVECTOR CameraStartPoint = XMVectorSet(0.04f, 23.f, -0.f, 1.f);
	XMVECTOR CameraGazePoint  = XMVectorSet(-1.f, 0.f, 0.f, 1.f);

	GameInstance::GetInstance().Register_Camera(static_pointer_cast<Camera>(shared_from_this()));

	if (FAILED(__super::Initialize(nullptr)))											return E_FAIL;
	if (FAILED(__super::CameraViewSetting(CameraStartPoint, CameraGazePoint, 10.f)))	return E_FAIL;

	MouseControll_Enable = true;
	CameraDistance		= 3.98f;
	CameraAngle			= { 23.82f, 1.59f };
	SmoothCameraValue	= { 0.f, 0.f };
	CameraCoord			= { -0.05f, 1.3f };

	Component_Transform->Set_WorldPosition(0.f, 0.f, 0.f);
	CameraLook = { 0.f, 0.f, 0.f, 0.f };
	return S_OK;
}

VOID	ActionCamera::Priority_Update(CONST _float& _DT) {
	if (GameInstance::GetInstance().Get_MainCamera() == shared_from_this()) {
		if (Camera_StandByChangLi) {
			ChangLi_CameraAction(_DT);
		}
		if (Camera_StandByScar) {
			Scar_Ultimate_CameraAction(_DT);
		}
		Camera_Controller(_DT);
		
		Fixed_MousePointer();
		
		Camera::Update_CameraView();
	}
}
VOID	ActionCamera::Update(CONST _float& _DT) {

}
VOID	ActionCamera::Late_Update(CONST _float& _DT) {
	if (GameInstance::GetInstance().Get_MainCamera() == shared_from_this()) 
		Update_FrustumCulling(_DT);
}
HRESULT	ActionCamera::Render() {
	return S_OK;
}

VOID	ActionCamera::Update_FrustumCulling(const _float& _DT) {
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
				if ((*iter) == shared_from_this()) { ++iter; continue; };

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
VOID	ActionCamera::FrustumCulling_SubMesh(shared_ptr<GameObject> _GOBJ) {
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

VOID	ActionCamera::Camera_Controller(const _float& _DT) {

	XMStoreFloat4(&CameraLook, CameraPositionBone->Get_ComBinedTransform().r[3] + PlayerTransform->Get_WorldPosition());

	CameraLook.x += CameraCoord.x; CameraLook.y += CameraCoord.y;	CameraLook.w = 1.f;
	Component_Transform->LookAt_Target(XMLoadFloat4(&CameraLook));
	
	_float Camera_RotationX = 0.f, Camera_RotationY = 0.f, MouseSensor = 0.01f;
	ImGuiIO& io = ImGui::GetIO();
	if (MouseControll_Enable == true) {
		_float Smoothness = 12.f;
	
		SmoothCameraValue.x += (CameraAngle.x - SmoothCameraValue.x) * _DT * Smoothness;
		SmoothCameraValue.y += (CameraAngle.y - SmoothCameraValue.y) * _DT * Smoothness;
	
		XMMATRIX SpringArmRotation = XMMatrixRotationRollPitchYaw(SmoothCameraValue.y / 10.f, -SmoothCameraValue.x / 10.f, 0.f);
		XMVECTOR SpringArmDirection = XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f), SpringArmRotation);
	
		XMVECTOR CameraPosition = XMVectorLerp(Component_Transform->Get_WorldPosition(), XMLoadFloat4(&CameraLook) + (SpringArmDirection * CameraDistance), _DT * Smoothness);
		Component_Transform->Set_WorldPosition(CameraPosition);
	}
}
VOID ActionCamera::Fixed_MousePointer() {
	if (KEY_DOWN(DIK_F1))
		MouseControll_Enable ? MouseControll_Enable = FALSE : MouseControll_Enable = TRUE;

	if (MouseControll_Enable) {
		RECT rc{};
		GetWindowRect(GameInstance::GetInstance().Get_GraphicDevice()->Get_EngineDescription().hWnd, &rc);
		SetCursorPos(rc.left + (1280 / 2), rc.top + (720 / 2));
	}
}
VOID ActionCamera::ChangLi_CameraAction(const _float& _DT) {

	XMVECTOR PlayerForward	= PlayerTransform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK);
	XMVECTOR WorldForward	= XMVectorSet(0.f, 0.f, 1.f, 0.f);

	XMVECTOR CrossResult	= XMVector3Cross(PlayerForward, WorldForward);

	_float DegreeRotation	= XMVectorGetY(PlayerTransform->Get_WorldRotation());
	_float RotationOffset	= 0.f;

	if (XMVectorGetY(CrossResult) > 0) { RotationOffset = +XMConvertToRadians(DegreeRotation); }
	else { RotationOffset = -XMConvertToRadians(DegreeRotation); }

	XMStoreFloat4(&CameraLook, CameraPositionBone->Get_ComBinedTransform().r[3] + PlayerTransform->Get_WorldPosition());

	if (ChangLi_CutScene == 1) {
		CameraPointWeight1 += _DT * 0.8f;
		if (CameraPointWeight1 <= 1.f) {
			CameraAngle.x = 23.82f + (33.58f - 23.82f) * CameraPointWeight1 + RotationOffset * 10.f;
			CameraAngle.y = 1.59f + (2.40f - 1.59f) * CameraPointWeight1;

			CameraDistance = 3.98f + (5.084f - 3.98f) * CameraPointWeight1;

			CameraCoord.x = -0.05f + (0.0625f - (-0.05f)) * CameraPointWeight1;
			CameraCoord.y = -0.01f + (-0.165f - -0.01f) * CameraPointWeight1 + 1.31f;
		}
	}
	if (ChangLi_CutScene == 2) {
		CameraPointWeight2 += _DT * 3.5f;
		if (CameraPointWeight2 <= 1.f) {
			_float AccerlationWeight = (_float)pow(CameraPointWeight2, 4);
			CameraAngle.y = 2.40f + (0.73f - 2.40f) * AccerlationWeight;

			CameraDistance = 5.084f + (60.5f - 5.084f) * AccerlationWeight;

			CameraCoord.y = -0.165f + (-2.425f - -0.165f) * AccerlationWeight * CameraPointWeight2 + 1.31f;
		}
	}
}

VOID ActionCamera::Scar_Ultimate_CameraAction(const _float& _DT) {

	if (nullptr == FinalBoss) {
		FinalBoss = static_pointer_cast<Monster_FinalBoss>(GameInstance::GetInstance().Get_SceneManager()
			->Get_GameObject(GameInstance::GetInstance().Get_SceneOption().CurrentSceneIndex, "Monster_FinalBoss"));
		FinalBossTransform = static_pointer_cast<Transform>(FinalBoss->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
		FinalBossRootBone = static_pointer_cast<MeshLoader>(FinalBoss->Find_Component(COMPONENT_TYPE::COMPONENT_MODEL))->Get_ModelPelvisBone();
	}

	XMVECTOR ScarTransform = FinalBossTransform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK);
	XMVECTOR WorldForward = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	XMVECTOR CrossResult = XMVector3Cross(ScarTransform, WorldForward);

	_float DegreeRotation = XMVectorGetY(FinalBossTransform->Get_WorldRotation());
	_float RotationOffset = 0.f;  
	if (XMVectorGetY(CrossResult) > 0) { RotationOffset = +XMConvertToRadians(DegreeRotation); }
	else { RotationOffset = -XMConvertToRadians(DegreeRotation); }
	XMStoreFloat4(&CameraLook, FinalBossRootBone->Get_ComBinedTransform().r[3] + FinalBossTransform->Get_WorldPosition());
	if (Scar_CutScene_Ultimate == 1) {
		CameraPointWeight1 += _DT * 0.8f;
		if (CameraPointWeight1 <= 1.f) {
			CameraAngle.x = 23.82f + (33.58f - 23.82f) * CameraPointWeight1 + RotationOffset * 10.f;
			CameraAngle.y = 1.59f + (2.40f - 1.59f) * CameraPointWeight1;

			CameraDistance = 3.98f + (5.084f - 3.98f) * CameraPointWeight1;

			CameraCoord.x = -0.05f + (0.0625f - (-0.05f)) * CameraPointWeight1;
			CameraCoord.y = -0.01f + (-0.165f - -0.01f) * CameraPointWeight1 + 1.31f;
		}
	}
	if (Scar_CutScene_Ultimate == 2) {
		CameraPointWeight2 += _DT * 3.5f;
		if (CameraPointWeight2 <= 1.f) {
			_float AccerlationWeight = (_float)pow(CameraPointWeight2, 4);
			CameraAngle.y = 2.40f + (0.73f - 2.40f) * AccerlationWeight;

			CameraDistance = 5.084f + (60.5f - 5.084f) * AccerlationWeight;

			CameraCoord.y = -0.165f + (-2.425f - -0.165f) * AccerlationWeight * CameraPointWeight2 + 1.31f;
		}
	}
}

VOID ActionCamera::Reset_ChangLi_CutScene() {
	ChangLi_CutScene = 0;
	CameraPointWeight1 = CameraPointWeight2 = 0.f;

	CameraDistance		= 3.98f;
	CameraAngle			= { 23.82f, 1.59f };
	SmoothCameraValue	= { 0.f, 0.f };
	CameraCoord			= { -0.05f, 1.3f };

	Camera_StandByChangLi = false;
}

unique_ptr<ActionCamera>	ActionCamera::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<ActionCamera>(new ActionCamera(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create ActionCamera.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>		ActionCamera::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<ActionCamera>(new ActionCamera(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone ActionCamera");
		return nullptr;
	}
	return Instance;
}