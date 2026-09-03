#include "../Default/PCH.h"
#include "PlayerCamera.h"
#include "ActionCamera.h"

HRESULT PlayerStateMachine::Initialize_StateMachine() {
	StateList.resize(static_cast<uint32_t>(PLAYER_STATE::STATE_END));

	StateList[static_cast<uint32_t>(PLAYER_STATE::IDLE)]	 = make_unique<IdleState>(Owner);
	StateList[static_cast<uint32_t>(PLAYER_STATE::WALK)]	 = make_unique<WalkState>(Owner);
	StateList[static_cast<uint32_t>(PLAYER_STATE::RUNNING)]  = make_unique<RunningState>(Owner);
	StateList[static_cast<uint32_t>(PLAYER_STATE::DASH)]	 = make_unique<DashState>(Owner);
	StateList[static_cast<uint32_t>(PLAYER_STATE::ATTACK)]	 = make_unique<AttackState>(Owner);
	StateList[static_cast<uint32_t>(PLAYER_STATE::LAND)]	 = make_unique<LandState>(Owner);
	StateList[static_cast<uint32_t>(PLAYER_STATE::BOOST)]	 = make_unique<BoostState>(Owner);
	StateList[static_cast<uint32_t>(PLAYER_STATE::ULTIMATE)] = make_unique<UltimateState>(Owner);
	StateList[static_cast<uint32_t>(PLAYER_STATE::JUMP)]	 = make_unique<JumpState>(Owner);
	StateList[static_cast<uint32_t>(PLAYER_STATE::SKILL)]	 = make_unique<SkillState>(Owner);
	StateList[static_cast<uint32_t>(PLAYER_STATE::ULTIMATE)] = make_unique<UltimateState>(Owner);
	StateList[static_cast<uint32_t>(PLAYER_STATE::FALLING)]  = make_unique<FallingState>(Owner);
	StateList[static_cast<uint32_t>(PLAYER_STATE::AIRATTACK)] = make_unique<AirAttackState>(Owner);
	StateList[static_cast<uint32_t>(PLAYER_STATE::SLAM)]	 = make_unique<SlamState>(Owner);
	
	return S_OK;
}
HRESULT PlayerStateMachine::Update_StateMachine(const _float& _DT) {
	CurrentState->FSM_StateUpdate(_DT);
	return S_OK;
}
HRESULT PlayerStateMachine::FSM_StateChange(PLAYER_STATE _State) {
	if (nullptr != CurrentState) {
		CurrentState->FSM_StateExit();
		PreviousState = CurrentState;
	}
	CurrentState = StateList[static_cast<uint32_t>(_State)].get();
	CurrentState->FSM_StateEnter();

	PreviousPlayerState = CurrentPlayerState;
	CurrentPlayerState = _State;

	return S_OK;
}
HRESULT PlayerStateMachine::FSM_SetOwner(shared_ptr<Player> _Owner) {
	if (nullptr == _Owner) {
		MSG_BOX("Invalid Owner.");
		return E_FAIL;
	}
	Owner = _Owner;
	return S_OK;
}
shared_ptr<PlayerStateMachine> PlayerStateMachine::Create(shared_ptr<Player> _Owner) {
	auto Instance = shared_ptr<PlayerStateMachine>(new PlayerStateMachine(_Owner));
	if (FAILED(Instance->Initialize_StateMachine())) {
		MSG_BOX("Cannot Clone PlayerStateMachine.");
		return nullptr;
	}
	return Instance;
}
	
VOID IdleState::FSM_StateEnter() {
	if (InitializeFlag) {
		Component_Animator = Owner->Get_AnimatorComponent();
		StateMachine	   = Owner->Get_StateMachine();

		IdleAnimationList[0] = Component_Animator->Get_AnimationByIndex(static_cast<uint32_t>(CHANGLI::STAND_IDLE)) ;
		IdleAnimationList[1] = Component_Animator->Get_AnimationByIndex(static_cast<uint32_t>(CHANGLI::STAND_EXP02));
		IdleAnimationList[2] = Component_Animator->Get_AnimationByIndex(static_cast<uint32_t>(CHANGLI::STAND_EXP03));
		IdleAnimationList[3] = Component_Animator->Get_AnimationByIndex(static_cast<uint32_t>(CHANGLI::STAND_EXP04));

		InitializeFlag = false;
	}

	Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::STAND_IDLE), true);
	Component_Animator->Set_AnimationBlending(true);

	Owner->Set_ObjectOnAir(static_cast<uint32_t>(JUMPSTATE::READY));
	Owner->Set_JumpCount(0);
}
VOID IdleState::FSM_StateUpdate(const _float& _DT) {
	if (KEY_HOLD(DIK_W) || KEY_HOLD(DIK_A) || KEY_HOLD(DIK_S) || KEY_HOLD(DIK_D)) {
		if (KEY_HOLD(DIK_LSHIFT)) {
			StateMachine->FSM_StateChange(PLAYER_STATE::RUNNING); return;
		}
		StateMachine->FSM_StateChange(PLAYER_STATE::WALK);		return;
	}
	if (KEY_HOLD(DIK_SPACE)) {
		StateMachine->FSM_StateChange(PLAYER_STATE::JUMP);		return;
	}
	if (IdleAnimationList[0]->Get_AnimationState() == ANIMATION_STATE::FINISHED) {
		Component_Animator->Play_Animation(IdleAnimationList[rand() % 3 + 1], false);
		Component_Animator->Set_AnimationBlending(true);
	}
	if (IdleAnimationList[1]->Get_AnimationState() == ANIMATION_STATE::FINISHED ||
		IdleAnimationList[2]->Get_AnimationState() == ANIMATION_STATE::FINISHED ||
		IdleAnimationList[3]->Get_AnimationState() == ANIMATION_STATE::FINISHED) {

		Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::STAND_IDLE), true);
		Component_Animator->Set_AnimationBlending(true);
	}
}
VOID IdleState::FSM_StateExit() {

}

VOID WalkState::FSM_StateEnter() {
	if (InitializeFlag) {
		Component_Animator		= Owner->Get_AnimatorComponent();
		Component_Transform		= Owner->Get_TransformComponent();
		Component_NavMeshAgent	= Owner->Get_NavMeshAgentComponent();
		StateMachine			= Owner->Get_StateMachine();

		PlayerCamera_Transform = static_pointer_cast<Transform>(GameInstance::GetInstance().Get_MainCamera()->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));

		InitializeFlag = false;
	}
	Owner->Set_ObjectOnAir(static_cast<uint32_t>(JUMPSTATE::READY));
	Owner->Set_JumpCount(0);

	SFX_FootStep = GameInstance::GetInstance().Get_SoundManager()->Play_Sound(L"SFX_Character_Walk", false, 0.03f);
}
VOID WalkState::FSM_StateUpdate(const _float& _DT) {

	if		(KEY_HOLD(DIK_SPACE)) { StateMachine->FSM_StateChange(PLAYER_STATE::JUMP); return; }
	else if (!KEY_HOLD(DIK_W) && !KEY_HOLD(DIK_A) && !KEY_HOLD(DIK_S) && !KEY_HOLD(DIK_D)){ 
		StateMachine->FSM_StateChange(PLAYER_STATE::IDLE);	return;
	}
	
	XMFLOAT3 InputVector = { 0.f, 0.f, 0.f };
	FLOAT	 MovementSpeed = 2.5f;
	FLOAT	 RotationSpeed = 5.f;

	if (KEY_HOLD(DIK_W)) InputVector.z += 1.f;
	if (KEY_HOLD(DIK_S)) InputVector.z -= 1.f;
	if (KEY_HOLD(DIK_A)) InputVector.x -= 1.f;
	if (KEY_HOLD(DIK_D)) InputVector.x += 1.f;

	XMVECTOR MoveInput = XMLoadFloat3(&InputVector);
	if (XMVector3Equal(MoveInput, XMVectorZero()))	{ StateMachine->FSM_StateChange(PLAYER_STATE::IDLE);	return;	}  // 입력 없는 경우 -> IDLE
	if (KEY_HOLD(DIK_LSHIFT))						{ 
		StateMachine->FSM_StateChange(PLAYER_STATE::RUNNING); return;
	}  // SHIFT 같이 입력하는 경우 -> RUNNING

	XMVECTOR CameraLook  = XMVector3Normalize(XMVectorSetY(PlayerCamera_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK) , 0.f));
	XMVECTOR CameraRight = XMVector3Normalize(XMVectorSetY(PlayerCamera_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT), 0.f));

	if (Component_Animator->Get_CurrentAnimationIndex() != static_cast<uint32_t>(CHANGLI::WALK_FRONT)) {
		Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::WALK_FRONT), true);
		Component_Animator->Set_AnimationBlending(true);
	}
	XMVECTOR MoveDirection	= (InputVector.z * CameraLook) + (InputVector.x * CameraRight);
	XMVECTOR MoveDelta		= MoveDirection * _DT * MovementSpeed;

	XMVECTOR	NextPosition = Component_Transform->Get_WorldPosition() + MoveDelta;
	_float		NextHeight = 0.f, CellHeight = 0.f;
	if (Component_NavMeshAgent->Evaluate_NextPositionCell(NextPosition, &NextHeight, &CellHeight, _DT)) {
		if (CellHeight + 2.f < XMVectorGetY(NextPosition)) {
			Component_Transform->Set_WorldPosition(NextPosition);
			Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::FALL_LOOP), true);
			Component_Animator->Set_AnimationBlending(true);
			StateMachine->FSM_StateChange(PLAYER_STATE::FALLING);
			Owner->Set_JumpCount(1);
			return;
		}
		Component_Transform->Set_WorldPosition(XMVectorSetY(NextPosition, NextHeight));
	}
	
	XMMATRIX TargetRotationMat = XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, 0.f, 0.f), MoveDirection, XMVectorSet(0.f, 1.f, 0.f, 0.f));

	XMVECTOR Det;
	TargetRotationMat = XMMatrixInverse(&Det, TargetRotationMat);

	XMVECTOR TargetQuat = XMQuaternionRotationMatrix(TargetRotationMat);
	TargetQuat = XMQuaternionMultiply(TargetQuat, XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XM_PI));
	XMVECTOR CurrQuat = Component_Transform->Get_WorldRotationQuat();

	XMVECTOR NextQuat = XMQuaternionSlerp(CurrQuat, TargetQuat, _DT * RotationSpeed);
	Component_Transform->Set_WorldRotationQuat(NextQuat);

	////////////////////////////////////////////// FootStep Sound Effect
	FootStepTimer += _DT;
	_float Interval = 0.80f, SoundVolume = 0.03f;
	if (FootStepTimer >= Interval) {
		FootStepTimer = 0.f;
		SFX_FootStep = GameInstance::GetInstance().Get_SoundManager()->Play_Sound(L"SFX_Character_Walk", false, SoundVolume);
	}
	//////////////////////////////////////////////
}
VOID WalkState::FSM_StateExit() {

}

VOID RunningState::FSM_StateEnter() {
	if (InitializeFlag) {
		Component_Animator = Owner->Get_AnimatorComponent();
		Component_Transform = Owner->Get_TransformComponent();
		Component_NavMeshAgent = Owner->Get_NavMeshAgentComponent();
		StateMachine = Owner->Get_StateMachine();

		PlayerCamera_Transform = static_pointer_cast<Transform>(GameInstance::GetInstance().Get_MainCamera()->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));

		InitializeFlag = false;
	}
	Owner->Set_ObjectOnAir(static_cast<uint32_t>(JUMPSTATE::READY));
	Owner->Set_JumpCount(0);

	SFX_FootStep = GameInstance::GetInstance().Get_SoundManager()->Play_Sound(L"SFX_Character_Run", false, 0.040f);
}
VOID RunningState::FSM_StateUpdate(CONST _float& _DT) {
	if (KEY_HOLD(DIK_SPACE)) { StateMachine->FSM_StateChange(PLAYER_STATE::JUMP); return; }						// [SPACE] : JUMP
	
	XMFLOAT3 InputVector = { 0.f, 0.f, 0.f };

	if (KEY_HOLD(DIK_W)) InputVector.z += 1.f;
	if (KEY_HOLD(DIK_S)) InputVector.z -= 1.f;
	if (KEY_HOLD(DIK_A)) InputVector.x -= 1.f;
	if (KEY_HOLD(DIK_D)) InputVector.x += 1.f;

	XMVECTOR MoveInput = XMLoadFloat3(&InputVector);
	if (XMVector3Equal(MoveInput, XMVectorZero()))	{ 
		StateMachine->FSM_StateChange(PLAYER_STATE::IDLE);
		return;
	}
	
	if (!KEY_HOLD(DIK_LSHIFT)) {
		StateMachine->FSM_StateChange(PLAYER_STATE::WALK); 
		return;
	}

	FLOAT	 MovementSpeed = 10.f;
	FLOAT	 RotationSpeed = 15.f;

	XMVECTOR CameraLook = XMVector3Normalize(XMVectorSetY(PlayerCamera_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK), 0.f));
	XMVECTOR CameraRight = XMVector3Normalize(XMVectorSetY(PlayerCamera_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT), 0.f));

	if (Component_Animator->Get_CurrentAnimationIndex() != static_cast<uint32_t>(CHANGLI::RUN_FRONT)) {
		Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::RUN_FRONT), true);
		Component_Animator->Set_AnimationBlending(true);
	}

	XMVECTOR MoveDirection = (InputVector.z * CameraLook) + (InputVector.x * CameraRight);
	XMVECTOR MoveDelta = MoveDirection * _DT * MovementSpeed;

	XMVECTOR	NextPosition = Component_Transform->Get_WorldPosition() + MoveDelta;
	_float		NextHeight = 0.f, CellHeight = 0.f;
	if (Component_NavMeshAgent->Evaluate_NextPositionCell(NextPosition, &NextHeight, &CellHeight, _DT)) {
		if (CellHeight + 2.f < XMVectorGetY(NextPosition)) {
			Component_Transform->Set_WorldPosition(NextPosition);
			Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::FALL_LOOP), true);
			Component_Animator->Set_AnimationBlending(true);
			StateMachine->FSM_StateChange(PLAYER_STATE::FALLING);
			Owner->Set_JumpCount(1);
			return;
		}
		Component_Transform->Set_WorldPosition(XMVectorSetY(NextPosition, NextHeight));
	}

	XMMATRIX TargetRotationMat = XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, 0.f, 0.f), MoveDirection, XMVectorSet(0.f, 1.f, 0.f, 0.f));

	XMVECTOR Det;
	TargetRotationMat = XMMatrixInverse(&Det, TargetRotationMat);

	XMVECTOR TargetQuat = XMQuaternionRotationMatrix(TargetRotationMat);
	TargetQuat = XMQuaternionMultiply(TargetQuat, XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XM_PI));
	XMVECTOR CurrQuat = Component_Transform->Get_WorldRotationQuat();

	XMVECTOR NextQuat = XMQuaternionSlerp(CurrQuat, TargetQuat, _DT * RotationSpeed);
	Component_Transform->Set_WorldRotationQuat(NextQuat);

	////////////////////////////////////////////// FootStep Sound Effect
	FootStepTimer += _DT;
	_float Interval = 0.45f, SoundVolume = 0.040f;
	if (FootStepTimer >= Interval) {
		FootStepTimer = 0.f;
		SFX_FootStep = GameInstance::GetInstance().Get_SoundManager()->Play_Sound(L"SFX_Character_Run", false, SoundVolume);
	}
	//////////////////////////////////////////////
}
VOID RunningState::FSM_StateExit() {
	GameInstance::GetInstance().Get_SoundManager()->Stop_Channel(SFX_FootStep);
	FootStepTimer = 0.f;
}
VOID DashState::FSM_StateEnter() {
	if (InitializeFlag) {
		Component_Animator	= Owner->Get_AnimatorComponent();
		Component_Transform = Owner->Get_TransformComponent();
		Component_NavMeshAgent = Owner->Get_NavMeshAgentComponent();
		StateMachine		= Owner->Get_StateMachine();
		ObjectOnAir			= Owner->Get_ObjectOnAir();
		InitializeFlag = false;
	}
	Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::SPRINT_DASH));
	Component_Animator->Set_AnimationBlending(true);
}
VOID DashState::FSM_StateUpdate(CONST _float& _DT) {

	if (*ObjectOnAir == static_cast<uint32_t>(JUMPSTATE::JUMPING) || *ObjectOnAir == static_cast<uint32_t>(JUMPSTATE::FALLING)) {
		XMVECTOR NextPosition = Component_Transform->Get_WorldPosition();
		_float NextHeight = 0.f, CellHeight = 0.f;
		int32_t NextIndex = -1;
		NextPosition = NextPosition - XMVectorSet(0.f, _DT * 3.f, 0.f, 0.f);
		if (Component_NavMeshAgent->Evaluate_NextPositionCell(NextPosition, &NextHeight, &CellHeight, 0.f)) {
			Component_Transform->Set_WorldPosition(NextPosition);
			if (CellHeight >= XMVectorGetY(NextPosition)) {
				Component_Transform->Set_WorldPosition(XMVectorSetY(NextPosition, CellHeight));
				StateMachine->FSM_StateChange(PLAYER_STATE::LAND);
				return;
			}
		}
	}

	if (Component_Animator->Get_CurrentAnimation()->Get_AnimationProgress() >= 0.75f) {
 		if (*ObjectOnAir == static_cast<uint32_t>(JUMPSTATE::JUMPING)
			|| *ObjectOnAir == static_cast<uint32_t>(JUMPSTATE::FALLING)) {
			*ObjectOnAir = static_cast<uint32_t>(JUMPSTATE::FALLING);
			Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::FALL_LOOP), true);
			Component_Animator->Set_AnimationBlending(true);
			StateMachine->FSM_StateChange(PLAYER_STATE::FALLING);
			Owner->Set_JumpCount(1);
			return;
		}

		if (KEY_HOLD(DIK_W) || KEY_HOLD(DIK_A) || KEY_HOLD(DIK_S) || KEY_HOLD(DIK_D)) {
			if (KEY_HOLD(DIK_LSHIFT)) { 
				Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::RUN_FRONT), true);
				Component_Animator->Set_AnimationBlending(true);
				StateMachine->FSM_StateChange(PLAYER_STATE::RUNNING); return; }

			Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::WALK_FRONT), true);
			Component_Animator->Set_AnimationBlending(true);
			StateMachine->FSM_StateChange(PLAYER_STATE::WALK); return;
		}
		StateMachine->FSM_StateChange(PLAYER_STATE::IDLE); return;
	}
}
VOID DashState::FSM_StateExit() {
}
VOID AttackState::FSM_StateEnter() {
	if (InitializeFlag) {
		Component_Animator	= Owner->Get_AnimatorComponent();
		Component_Transform = Owner->Get_TransformComponent();
		Component_NavMeshAgent = Owner->Get_NavMeshAgentComponent();
		StateMachine		= Owner->Get_StateMachine();

		PlayerCam			= static_pointer_cast<PlayerCamera>(GameInstance::GetInstance().Get_MainCamera());

		AttackCombo			= Owner->Get_AttackCombo();
		ObjectOnAir			= Owner->Get_ObjectOnAir();

		InitializeFlag = false;
	}

	if (*ObjectOnAir == static_cast<uint32_t>(JUMPSTATE::LAND)	  || *ObjectOnAir == static_cast<uint32_t>(JUMPSTATE::READY)) {
		if (*AttackCombo > 4)	{ *AttackCombo = 0; }
		*AttackCombo += 1;
	};
	if (*ObjectOnAir == static_cast<uint32_t>(JUMPSTATE::JUMPING) || *ObjectOnAir == static_cast<uint32_t>(JUMPSTATE::FALLING)) {
		if (*AttackCombo <= 5)	{ *AttackCombo = 6; }
		if (*AttackCombo < 13)	{ *AttackCombo += 1; }

		StateMachine->FSM_StateChange(PLAYER_STATE::AIRATTACK);
	}
	for (uint32_t IDX = 0; IDX < 10; ++IDX) EffectFlag[IDX] = true;
}
VOID AttackState::FSM_StateUpdate(CONST _float& _DT) {
	
	if (*Owner->Get_ObjectOnAir() == static_cast<uint32_t>(JUMPSTATE::READY)) {
		if (KEY_HOLD(DIK_W) || KEY_HOLD(DIK_A) || KEY_HOLD(DIK_S) || KEY_HOLD(DIK_D)) {
			if (KEY_HOLD(DIK_LSHIFT)) { 
				Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::RUN_FRONT));
				Component_Animator->Set_AnimationBlending(true);
				StateMachine->FSM_StateChange(PLAYER_STATE::RUNNING); return;
			}
			Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::WALK_FRONT));
			Component_Animator->Set_AnimationBlending(true);
			StateMachine->FSM_StateChange(PLAYER_STATE::WALK); return;
		}
		if (KEY_HOLD(DIK_SPACE)) { 
			Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::JUMP));
			Component_Animator->Set_AnimationBlending(true);
			StateMachine->FSM_StateChange(PLAYER_STATE::JUMP);	return;
		}
	}
	
	if (*AttackCombo >= 1 && *AttackCombo <= 5) {
		uint32_t CurrentAnimationIndex = static_cast<uint32_t>(CHANGLI::ATTACK01) + (*AttackCombo) - 1;
		if (Component_Animator->Get_AnimationState(CurrentAnimationIndex) == ANIMATION_STATE::ONREADY) {
			Component_Animator->Play_Animation(CurrentAnimationIndex);
			Component_Animator->Set_AnimationBlending(true);
		}
		if (Component_Animator->Get_AnimationState(CurrentAnimationIndex) == ANIMATION_STATE::FINISHED) {
			StateMachine->FSM_StateChange(PLAYER_STATE::IDLE); return;
		}
	}
	if (*AttackCombo == 1 && EffectFlag[0] && Component_Animator->Get_CurrentAnimation()->Get_AnimationProgress() >= 0.08f) {
		auto LOutEffect = Owner->Get_EffectProto("EFF_RedCurveTrail_VFlip");
		LOutEffect->Set_EffectOrbitRotation(150.f, -20.f);
		LOutEffect->Set_EffectRotation({ 0.f, 0.f, 0.f });

		LOutEffect->Set_DissolveScrollSpeed({ -1.5f, 0.f });
		LOutEffect->Set_EffectScrollSpeed({ -1.5f, 0.f });

		LOutEffect->Set_DissolveTextureOffset({ 0.2f, -0.2f });
		LOutEffect->Set_EffectTextureOffset({ 0.2f, -0.2f });

		LOutEffect->Set_DissolveStrength(0.02f);
		LOutEffect->Set_EffectScale({ 4.f, 5.f, 5.f });
		LOutEffect->Set_EffectLifeTime(0.5f);

		LOutEffect->Set_DistanceFromPlayer(-1.f);
		LOutEffect->Set_EffectOffset({ 0.f, 2.2f, 0.f });
		Owner->Play_Effect(LOutEffect);

		auto ROutEffect = Owner->Get_EffectProto("EFF_RedCurveTrail_VFlip");
		ROutEffect->Set_EffectOrbitRotation(-30.f, -20.f);
		ROutEffect->Set_EffectRotation({ 0.f, 0.f, 0.f });

		ROutEffect->Set_DissolveScrollSpeed({ -1.5f, 0.f });
		ROutEffect->Set_EffectScrollSpeed({ -1.5f, 0.f });

		ROutEffect->Set_DissolveTextureOffset({ 0.2f, -0.2f });
		ROutEffect->Set_EffectTextureOffset({ 0.2f, -0.2f });

		ROutEffect->Set_DissolveStrength(0.02f);
		ROutEffect->Set_EffectScale({ 4.f, 5.f, 5.f });
		ROutEffect->Set_EffectLifeTime(0.5f);

		ROutEffect->Set_DistanceFromPlayer(-1.f);
		ROutEffect->Set_EffectOffset({ 0.f, 2.2f, 0.f });
		Owner->Play_Effect(ROutEffect);

		EffectFlag[0] = false;
	}
	if (*AttackCombo == 2 && EffectFlag[1] && Component_Animator->Get_CurrentAnimation()->Get_AnimationProgress() >= 0.06f) {
		auto LOutEffect = Owner->Get_EffectProto("EFF_RedFlatTrail");
		LOutEffect->Set_EffectOrbitRotation(170.f, -20.f);
		LOutEffect->Set_EffectRotation({ 0.f, 0.f, 30.f });

		LOutEffect->Set_DissolveScrollSpeed({ -1.5f, 0.f });
		LOutEffect->Set_EffectScrollSpeed({ -1.5f, 0.f });

		LOutEffect->Set_DissolveTextureOffset({ 0.2f, -0.2f });
		LOutEffect->Set_EffectTextureOffset({ 0.2f, -0.2f });

		LOutEffect->Set_DissolveStrength(0.02f);
		LOutEffect->Set_EffectScale({ 5.f, 5.f, 3.f });
		LOutEffect->Set_EffectLifeTime(0.5f);

		LOutEffect->Set_DistanceFromPlayer(1.f);
		LOutEffect->Set_EffectOffset({ 0.f, 2.5f, 0.f });
		Owner->Play_Effect(LOutEffect);

		auto ROutEffect = Owner->Get_EffectProto("EFF_RedCurveTrail_VFlip");
		ROutEffect->Set_EffectOrbitRotation(-30.f, -10.f);
		ROutEffect->Set_EffectRotation({ 0.f, 0.f, 30.f });

		ROutEffect->Set_DissolveScrollSpeed({ -1.5f, 0.f });
		ROutEffect->Set_EffectScrollSpeed({ -1.5f, 0.f });

		ROutEffect->Set_DissolveTextureOffset({ 0.2f, -0.2f });
		ROutEffect->Set_EffectTextureOffset({ 0.2f, -0.2f });

		ROutEffect->Set_DissolveStrength(0.02f);
		ROutEffect->Set_EffectScale({ 4.f, 5.f, 5.f });
		ROutEffect->Set_EffectLifeTime(0.5f);

		ROutEffect->Set_DistanceFromPlayer(-1.f);
		ROutEffect->Set_EffectOffset({ 0.f, 1.5f, 0.f });
		Owner->Play_Effect(ROutEffect);
		EffectFlag[1] = false;
	}
	if (*AttackCombo == 3) {
		_float Progress = Component_Animator->Get_CurrentAnimation()->Get_AnimationProgress();
		if (Progress >= 0.04f && EffectFlag[30]) {
			auto First = Owner->Get_EffectProto("EFF_RedCurveTrail");
			First->Set_EffectOrbitRotation(270.f, 10.f);
			First->Set_EffectRotation({ 0.f, 0.f, 0.f });

			First->Set_DissolveScrollSpeed({ 1.5f, 0.f });
			First->Set_EffectScrollSpeed({ 1.5f, 0.f });

			First->Set_DissolveTextureOffset({ -0.2f, 0.f });
			First->Set_EffectTextureOffset({ -0.2f, 0.f });

			First->Set_DissolveStrength(0.04f);
			First->Set_EffectScale({ 4.2f, 4.2f, 4.2f });
			First->Set_EffectLifeTime(0.5f);

			First->Set_DistanceFromPlayer(0.2f);
			First->Set_EffectOffset({ 0.f, 1.5f, 0.f });
			Owner->Play_Effect(First);

			auto Second = Owner->Get_EffectProto("EFF_PinkSlenderTrail");
			Second->Set_EffectOrbitRotation(250.f, 10.f);
			Second->Set_EffectRotation({ 0.f, 0.f, -5.f });

			Second->Set_DissolveScrollSpeed({ -1.5f, 0.f });
			Second->Set_EffectScrollSpeed({ -1.5f, 0.f });

			Second->Set_DissolveTextureOffset({ 0.2f, 0.f });
			Second->Set_EffectTextureOffset({ 0.2f, 0.f });

			Second->Set_DissolveStrength(0.04f);
			Second->Set_EffectScale({ 5.f, 5.f, 5.f });
			Second->Set_EffectLifeTime(1.4f);

			Second->Set_DistanceFromPlayer(0.4f);
			Second->Set_EffectOffset({ 0.f, 2.f, 0.f });
			Owner->Play_Effect(Second);
			EffectFlag[30] = false;
		}
		if (Progress >= 0.06f && EffectFlag[31]) {
			auto First = Owner->Get_EffectProto("EFF_RedCurveTrail");
			First->Set_EffectOrbitRotation(90.f, -10.f);
			First->Set_EffectRotation({ 0.f, 0.f, 0.f });

			First->Set_DissolveScrollSpeed({ 1.5f, 0.f });
			First->Set_EffectScrollSpeed({ 1.5f, 0.f });

			First->Set_DissolveTextureOffset({ -0.2f, 0.f });
			First->Set_EffectTextureOffset({ -0.2f, 0.f });

			First->Set_DissolveStrength(0.04f);
			First->Set_EffectScale({ 4.2f, 4.2f, 4.2f });
			First->Set_EffectLifeTime(0.5f);

			First->Set_DistanceFromPlayer(0.f);
			First->Set_EffectOffset({ 0.f, 1.5f, 0.f });
			Owner->Play_Effect(First);

			auto Second = Owner->Get_EffectProto("EFF_PinkSlenderTrail");
			Second->Set_EffectOrbitRotation(70.f, -10.f);
			Second->Set_EffectRotation({ 0.f, 0.f, 5.f });

			Second->Set_DissolveScrollSpeed({ -1.5f, 0.f });
			Second->Set_EffectScrollSpeed({ -1.5f, 0.f });

			Second->Set_DissolveTextureOffset({ 0.2f, 0.f });
			Second->Set_EffectTextureOffset({ 0.2f, 0.f });

			Second->Set_DissolveStrength(0.04f);
			Second->Set_EffectScale({ 5.f, 5.f, 5.f });
			Second->Set_EffectLifeTime(1.4f);

			Second->Set_DistanceFromPlayer(0.2f);
			Second->Set_EffectOffset({ 0.f, 2.f, 0.f });
			Owner->Play_Effect(Second);
			EffectFlag[31] = false;
		}
		if (Progress >= 0.10f && EffectFlag[32]) {
			auto First = Owner->Get_EffectProto("EFF_RedCCurveTrail");
			First->Set_EffectOrbitRotation(180.f, 0.f);
			First->Set_EffectRotation({ 0.f, 0.f, -10.f });

			First->Set_DissolveScrollSpeed({ 1.5f, 0.f });
			First->Set_EffectScrollSpeed({ 1.5f, 0.f });

			First->Set_DissolveTextureOffset({ -0.2f, 0.f });
			First->Set_EffectTextureOffset({ -0.2f, 0.f });

			First->Set_DissolveStrength(0.04f);
			First->Set_EffectScale({ 3.5f * 1.5f, 4.f * 1.5f, 4.f * 1.5f });
			First->Set_EffectLifeTime(0.5f);

			First->Set_DistanceFromPlayer(-1.f);
			First->Set_EffectOffset({ 0.f, 2.f, 0.f });
			Owner->Play_Effect(First);

			auto Second = Owner->Get_EffectProto("EFF_PinkCCurveTrail");
			Second->Set_EffectOrbitRotation(180.f, 0.f);
			Second->Set_EffectRotation({ 0.f, 0.f, -10.f });

			Second->Set_DissolveScrollSpeed({ 1.5f, 0.f });
			Second->Set_EffectScrollSpeed({ 1.5f, 0.f });

			Second->Set_DissolveTextureOffset({ -0.2f, 0.f });
			Second->Set_EffectTextureOffset({ -0.2f, 0.f });

			Second->Set_DissolveStrength(0.04f);
			Second->Set_EffectScale({ 3.5f * 1.5f, 4.f * 1.5f, 3.5f * 1.5f });
			Second->Set_EffectLifeTime(0.5f);

			Second->Set_DistanceFromPlayer(-1.f);
			Second->Set_EffectOffset({ 0.f, 2.f, 0.f });
			Owner->Play_Effect(Second);
			EffectFlag[32] = false;
		}
		if (Progress >= 0.14f && EffectFlag[33]) {
			auto First = Owner->Get_EffectProto("EFF_RedCCurveTrail");
			First->Set_EffectOrbitRotation(180.f, 0.f);
			First->Set_EffectRotation({ 0.f, 0.f, -10.f });

			First->Set_DissolveScrollSpeed({ 1.5f, 0.f });
			First->Set_EffectScrollSpeed({ 1.5f, 0.f });

			First->Set_DissolveTextureOffset({ -0.2f, 0.f });
			First->Set_EffectTextureOffset({ -0.2f, 0.f });

			First->Set_DissolveStrength(0.04f);
			First->Set_EffectScale({ 3.5f * 1.65f, 4.f * 1.65f, 4.f * 1.65f });
			First->Set_EffectLifeTime(2.f);

			First->Set_DistanceFromPlayer(-0.75f);
			First->Set_EffectOffset({ 0.f, 2.f, 0.f });
			Owner->Play_Effect(First);

			auto Second = Owner->Get_EffectProto("EFF_PinkCCurveTrail");
			Second->Set_EffectOrbitRotation(180.f, 0.f);
			Second->Set_EffectRotation({ 0.f, 0.f, -10.f });

			Second->Set_DissolveScrollSpeed({ 1.5f, 0.f });
			Second->Set_EffectScrollSpeed({ 1.5f, 0.f });

			Second->Set_DissolveTextureOffset({ -0.2f, 0.f });
			Second->Set_EffectTextureOffset({ -0.2f, 0.f });

			Second->Set_DissolveStrength(0.04f);
			Second->Set_EffectScale({ 3.5f * 1.65f, 4.f * 1.65f, 3.5f * 1.65f });
			Second->Set_EffectLifeTime(2.f);

			Second->Set_DistanceFromPlayer(-0.75f);
			Second->Set_EffectOffset({ 0.f, 2.f, 0.f });
			Owner->Play_Effect(Second);
			EffectFlag[33] = false;
		}
	}
	if (*AttackCombo == 4) {
		_float Progress = Component_Animator->Get_CurrentAnimation()->Get_AnimationProgress();
		if (Progress >= 0.12f && EffectFlag[40]) {
			auto FrontTrailObject = Owner->Get_EffectProto("EFF_Ariel_Ribbon_Feather");
			FrontTrailObject->Set_EffectOrbitRotation(0.f, 0.f);
			FrontTrailObject->Set_EffectRotation({ 0.f, 0.f, 0.f });

			FrontTrailObject->Set_DissolveScrollSpeed({ +0.35f, 0.f });
			FrontTrailObject->Set_EffectScrollSpeed({ +0.35f, 0.f });

			FrontTrailObject->Set_DissolveTextureOffset({ -0.35f, 0.f });
			FrontTrailObject->Set_EffectTextureOffset({ -0.35f, 0.f });

			FrontTrailObject->Set_DissolveStrength(0.02f);
			FrontTrailObject->Set_EffectScale({ 1.f, 1.f, 1.f });
			FrontTrailObject->Set_EffectLifeTime(1.f);

			FrontTrailObject->Set_DistanceFromPlayer(+0.f);
			FrontTrailObject->Set_EffectOffset({ 0.f, -1.f, 0.f });
			Owner->Play_Effect(FrontTrailObject);

			auto ColorAirObject = Owner->Get_EffectProto("EFF_Ariel_Ribbon_ColorAir");
			ColorAirObject->Set_EffectOrbitRotation(0.f, 0.f);
			ColorAirObject->Set_EffectRotation({ 0.f, 0.f, 0.f });

			ColorAirObject->Set_DissolveScrollSpeed({ +0.35f, 0.f });
			ColorAirObject->Set_EffectScrollSpeed({ +0.35f, 0.f });

			ColorAirObject->Set_DissolveTextureOffset({ -0.35f, 0.f });
			ColorAirObject->Set_EffectTextureOffset({ -0.35f, 0.f });

			ColorAirObject->Set_DissolveStrength(0.02f);
			ColorAirObject->Set_EffectScale({ 0.5f, 1.5f, 0.5f });
			ColorAirObject->Set_EffectLifeTime(1.f);

			ColorAirObject->Set_DistanceFromPlayer(+0.f);
			ColorAirObject->Set_EffectOffset({ 0.f, -1.f, 0.f });
			Owner->Play_Effect(ColorAirObject);

			auto AirObject = Owner->Get_EffectProto("EFF_Ariel_Ribbon_Air");
			AirObject->Set_EffectOrbitRotation(0.f, 0.f);
			AirObject->Set_EffectRotation({ 0.f, 0.f, 0.f });

			AirObject->Set_DissolveScrollSpeed({ +0.35f, 0.f });
			AirObject->Set_EffectScrollSpeed({ +0.35f, 0.f });

			AirObject->Set_DissolveTextureOffset({ -0.35f, 0.f });
			AirObject->Set_EffectTextureOffset({ -0.35f, 0.f });

			AirObject->Set_DissolveStrength(0.02f);
			AirObject->Set_EffectScale({ 1.5f, 1.5f, 1.5f });
			AirObject->Set_EffectLifeTime(1.f);

			AirObject->Set_DistanceFromPlayer(+0.f);
			AirObject->Set_EffectOffset({ 0.f, -1.f, 0.f });
			Owner->Play_Effect(AirObject);

			EffectFlag[40] = false;
		}
	}
	if (*AttackCombo == 5) {
		_float Progress = Component_Animator->Get_CurrentAnimation()->Get_AnimationProgress();
		if		(Progress >= 0.08f && EffectFlag[50]) {
			auto FrontTrailObject = Owner->Get_EffectProto("EFF_RedFlatTrail");
			FrontTrailObject->Set_EffectOrbitRotation(-15.f, 15.f);
			FrontTrailObject->Set_EffectRotation({ 180.f, 0.f, 190.f });

			FrontTrailObject->Set_DissolveScrollSpeed({ -1.5f, 0.f });
			FrontTrailObject->Set_EffectScrollSpeed({ -1.5f, 0.f });

			FrontTrailObject->Set_DissolveTextureOffset({ 0.2f, 0.f });
			FrontTrailObject->Set_EffectTextureOffset({ 0.2f, 0.f });

			FrontTrailObject->Set_DissolveStrength(0.02f);
			FrontTrailObject->Set_EffectScale({ 5.f, 4.f, 5.f });
			FrontTrailObject->Set_EffectLifeTime(0.5f);

			FrontTrailObject->Set_DistanceFromPlayer(+1.f);
			FrontTrailObject->Set_EffectOffset({ 0.f, 1.5f, 0.f });
			Owner->Play_Effect(FrontTrailObject);

			auto TrailCoreObject = Owner->Get_EffectProto("EFF_BlueFlatTrail");
			TrailCoreObject->Set_EffectOrbitRotation(-15.f, 15.f);
			TrailCoreObject->Set_EffectRotation({ 180.f, -15.f, 190.f });

			TrailCoreObject->Set_DissolveScrollSpeed({ -1.5f, 0.f });
			TrailCoreObject->Set_EffectScrollSpeed({ -1.5f, 0.f });

			TrailCoreObject->Set_DissolveTextureOffset({ 0.2f, 0.f });
			TrailCoreObject->Set_EffectTextureOffset({ 0.2f, 0.f });

			TrailCoreObject->Set_DissolveStrength(0.02f);
			TrailCoreObject->Set_EffectScale({ 3.f, 2.f, 2.f });
			TrailCoreObject->Set_EffectLifeTime(0.5f);

			TrailCoreObject->Set_DistanceFromPlayer(-1.6f);
			TrailCoreObject->Set_EffectOffset({ 1.5f, 1.3f, 0.f });
			Owner->Play_Effect(TrailCoreObject);

			auto BackTrailObject = Owner->Get_EffectProto("EFF_RedCurveTrail");
			BackTrailObject->Set_EffectOrbitRotation(-15.f, 15.f);
			BackTrailObject->Set_EffectRotation({ -15.f, -20.f, 180.f });

			BackTrailObject->Set_DissolveScrollSpeed({ 1.5f, 0.f });
			BackTrailObject->Set_EffectScrollSpeed({ 1.5f, 0.f });

			BackTrailObject->Set_DissolveTextureOffset({ -0.2f, 0.f });
			BackTrailObject->Set_EffectTextureOffset({ -0.2f, 0.f });

			BackTrailObject->Set_DissolveStrength(0.02f);
			BackTrailObject->Set_EffectScale({ 3.f, 2.f, 3.f });
			BackTrailObject->Set_EffectLifeTime(0.5f);

			BackTrailObject->Set_DistanceFromPlayer(-1.f);
			BackTrailObject->Set_EffectOffset({ 0.f, 1.5f, 0.f });
			Owner->Play_Effect(BackTrailObject);
			EffectFlag[50] = false;
		}
		else if	(Progress >= 0.10f && EffectFlag[51]) {
			auto TrailObject = Owner->Get_EffectProto("EFF_RedFlatTrail");
			TrailObject->Set_EffectOrbitRotation(-30.f, 50.f);
			TrailObject->Set_EffectRotation({ 0.f, 180.f, 270.f });

			TrailObject->Set_DissolveScrollSpeed({ -0.7f, 0.f });
			TrailObject->Set_EffectScrollSpeed({ -0.7f, 0.f });

			TrailObject->Set_DissolveTextureOffset({ 0.2f, 0.f });
			TrailObject->Set_EffectTextureOffset({ 0.2f, 0.f });

			TrailObject->Set_DissolveStrength(0.02f);
			TrailObject->Set_EffectScale({ 4.f, 1.5f, 4.f });
			TrailObject->Set_EffectLifeTime(1.5f);

			TrailObject->Set_DistanceFromPlayer(-4.f);

			Owner->Play_Effect(TrailObject);

			auto CoreObject = Owner->Get_EffectProto("EFF_Slam_PinkFlatTrail");
			CoreObject->Set_EffectOrbitRotation(-30.f, 2.f);
			CoreObject->Set_EffectRotation({ 0.f, 0.f, 0.f });

			CoreObject->Set_DissolveScrollSpeed({ -0.5f, 0.f });
			CoreObject->Set_EffectScrollSpeed({ -0.5f, 0.f });

			CoreObject->Set_DissolveTextureOffset({ 0.2f, 0.f });
			CoreObject->Set_EffectTextureOffset({ 0.2f, 0.f });

			CoreObject->Set_DissolveStrength(0.02f);
			CoreObject->Set_EffectScale({ 1.5f, 1.2f, 1.5f });
			CoreObject->Set_EffectLifeTime(2.5f);

			CoreObject->Set_DistanceFromPlayer(-2.1f);

			Owner->Play_Effect(CoreObject);
			EffectFlag[51] = false;
		}
		else if	(Progress >= 0.12f && EffectFlag[52]) {
			auto TrailObject = Owner->Get_EffectProto("EFF_RedFlatTrail");
			TrailObject->Set_EffectOrbitRotation(0.f, 50.f);
			TrailObject->Set_EffectRotation({ 0.f, 180.f, 270.f });

			TrailObject->Set_DissolveScrollSpeed({ -0.7f, 0.f });
			TrailObject->Set_EffectScrollSpeed({ -0.7f, 0.f });

			TrailObject->Set_DissolveTextureOffset({ 0.2f, 0.f });
			TrailObject->Set_EffectTextureOffset({ 0.2f, 0.f });

			TrailObject->Set_DissolveStrength(0.02f);
			TrailObject->Set_EffectScale({ 4.f, 1.5f, 4.f });
			TrailObject->Set_EffectLifeTime(1.5f);

			TrailObject->Set_DistanceFromPlayer(-4.f);

			Owner->Play_Effect(TrailObject);

			auto CoreObject = Owner->Get_EffectProto("EFF_Slam_PinkFlatTrail");
			CoreObject->Set_EffectOrbitRotation(-0.f, 2.f);
			CoreObject->Set_EffectRotation({ 0.f, 0.f, 0.f });

			CoreObject->Set_DissolveScrollSpeed({ -0.5f, 0.f });
			CoreObject->Set_EffectScrollSpeed({ -0.5f, 0.f });

			CoreObject->Set_DissolveTextureOffset({ 0.2f, 0.f });
			CoreObject->Set_EffectTextureOffset({ 0.2f, 0.f });

			CoreObject->Set_DissolveStrength(0.02f);
			CoreObject->Set_EffectScale({ 1.5f, 1.2f, 1.5f });
			CoreObject->Set_EffectLifeTime(2.5f);

			CoreObject->Set_DistanceFromPlayer(-2.1f);

			Owner->Play_Effect(CoreObject);
		
			EffectFlag[52] = false;
		}
		else if (Progress >= 0.14f && EffectFlag[53]) {
			auto TrailObject = Owner->Get_EffectProto("EFF_RedFlatTrail");
			TrailObject->Set_EffectOrbitRotation(30.f, 50.f);
			TrailObject->Set_EffectRotation({ 0.f, 180.f, 270.f });

			TrailObject->Set_DissolveScrollSpeed({ -0.7f, 0.f });
			TrailObject->Set_EffectScrollSpeed({ -0.7f, 0.f });

			TrailObject->Set_DissolveTextureOffset({ 0.2f, 0.f });
			TrailObject->Set_EffectTextureOffset({ 0.2f, 0.f });

			TrailObject->Set_DissolveStrength(0.02f);
			TrailObject->Set_EffectScale({ 4.f, 1.5f, 4.f });
			TrailObject->Set_EffectLifeTime(1.5f);

			TrailObject->Set_DistanceFromPlayer(-4.f);

			Owner->Play_Effect(TrailObject);

			auto CoreObject = Owner->Get_EffectProto("EFF_Slam_PinkFlatTrail");
			CoreObject->Set_EffectOrbitRotation(30.f, 2.f);
			CoreObject->Set_EffectRotation({ 0.f, 0.f, 0.f });

			CoreObject->Set_DissolveScrollSpeed({ -0.5f, 0.f });
			CoreObject->Set_EffectScrollSpeed({ -0.5f, 0.f });

			CoreObject->Set_DissolveTextureOffset({ 0.2f, 0.f });
			CoreObject->Set_EffectTextureOffset({ 0.2f, 0.f });

			CoreObject->Set_DissolveStrength(0.02f);
			CoreObject->Set_EffectScale({ 1.5f, 1.2f, 1.5f });
			CoreObject->Set_EffectLifeTime(2.5f);

			CoreObject->Set_DistanceFromPlayer(-2.1f);

			Owner->Play_Effect(CoreObject);
		
			EffectFlag[53] = false;
		}
	}
}
VOID AttackState::FSM_StateExit() {
	for (uint32_t IDX = 0; IDX < 60; ++IDX) EffectFlag[IDX] = true;
}

VOID LandState::FSM_StateEnter() {
	if (InitializeFlag) {
		Component_Animator	= Owner->Get_AnimatorComponent();
		Component_Transform = Owner->Get_TransformComponent();
		Component_NavMeshAgent = Owner->Get_NavMeshAgentComponent();

		StateMachine		= Owner->Get_StateMachine();
		AttackCombo			= Owner->Get_AttackCombo();

		PlayerCamera_Transform = static_pointer_cast<Transform>(GameInstance::GetInstance().Get_MainCamera()->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));

		JumpCount = Owner->Get_JumpCount();

		InitializeFlag = false;
	}
	if (*JumpCount == 1) {
		Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::LAND_LIGHT), false, 1.5f);
		Component_Animator->Set_AnimationBlending(true);
	}
	if (*JumpCount == 2 || *JumpCount == 3 || *JumpCount == 999) {
		shared_ptr<PlayerCamera> PlayerCam = static_pointer_cast<PlayerCamera>(GameInstance::GetInstance().Get_MainCamera());
		Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::LAND_HEAVY), false, 2.f);
		PlayerCam->Camera_Drop(0.125f, { 0.f, -0.5f, 0.f }, { 0.f, -0.125f, 0.f });
		PlayerCam->Camera_Shake(0.125f, 0.2f);
	}
	Owner->Set_ObjectOnAir(static_cast<uint32_t>(JUMPSTATE::LAND));
	*JumpCount = 0;
}
VOID LandState::FSM_StateUpdate(CONST _float& _DT) {
	if		(MOUSE_LBUTTON) { StateMachine->FSM_StateChange(PLAYER_STATE::ATTACK); return; }		// 공격 시 -> 모션 캔슬 + ATTACK

	if (Component_Animator->Get_CurrentAnimationIndex() == static_cast<uint32_t>(CHANGLI::LAND_LIGHT) &&
		Component_Animator->Get_AnimationByIndex(static_cast<uint32_t>(CHANGLI::LAND_LIGHT))->Get_AnimationProgress() <= 0.1f) {
		XMFLOAT3 InputVector = { 0.f, 0.f, 0.f };

		if (KEY_HOLD(DIK_W)) InputVector.z += 1.f;
		if (KEY_HOLD(DIK_S)) InputVector.z -= 1.f;
		if (KEY_HOLD(DIK_A)) InputVector.x -= 1.f;
		if (KEY_HOLD(DIK_D)) InputVector.x += 1.f;

		XMVECTOR MoveInput = XMLoadFloat3(&InputVector);
		if (!XMVector3Equal(MoveInput, XMVectorZero())) {
			FLOAT	 MovementSpeed = 10.f;

			XMVECTOR CameraLook = XMVector3Normalize(XMVectorSetY(PlayerCamera_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK), 0.f));
			XMVECTOR CameraRight = XMVector3Normalize(XMVectorSetY(PlayerCamera_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT), 0.f));

			XMVECTOR MoveDirection = (InputVector.z * CameraLook) + (InputVector.x * CameraRight);
			XMVECTOR MoveDelta = MoveDirection * _DT * MovementSpeed;

			XMVECTOR	NextPosition = Component_Transform->Get_WorldPosition() + MoveDelta;
			_float		NextHeight = 0.f, CellHeight = 0.f;

			if (Component_NavMeshAgent->Evaluate_NextPositionCell(NextPosition, &NextHeight, &CellHeight, _DT)) {
				Component_Transform->Set_WorldPosition(NextPosition);
			}
		}
	}

	if (Component_Animator->Get_AnimationByIndex(static_cast<uint32_t>(CHANGLI::LAND_LIGHT))->Get_AnimationProgress() >= 0.3f
		|| Component_Animator->Get_AnimationByIndex(static_cast<uint32_t>(CHANGLI::LAND_HEAVY))->Get_AnimationProgress() >= 0.45f) {
		Owner->Set_ObjectOnAir(static_cast<uint32_t>(JUMPSTATE::READY));
		if (KEY_HOLD(DIK_W) || KEY_HOLD(DIK_A) || KEY_HOLD(DIK_D) || KEY_HOLD(DIK_S)) {
			if (KEY_HOLD(DIK_LSHIFT)) {
				Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::RUN_FRONT), true);
				Component_Animator->Set_AnimationBlending(true);
				StateMachine->FSM_StateChange(PLAYER_STATE::RUNNING); return;
			}
			Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::WALK_FRONT), true);
			Component_Animator->Set_AnimationBlending(true);
			StateMachine->FSM_StateChange(PLAYER_STATE::WALK); return;
		}
		StateMachine->FSM_StateChange(PLAYER_STATE::IDLE); return;
	}

	if (*AttackCombo == 5 && Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::LAND_LIGHT)) == ANIMATION_STATE::FINISHED) {
		StateMachine->FSM_StateChange(PLAYER_STATE::IDLE); return;
	}
}
VOID LandState::FSM_StateExit() {
	Owner->Set_ObjectOnAir(static_cast<uint32_t>(JUMPSTATE::READY));
}

VOID JumpState::FSM_StateEnter() {
	if (InitializeFlag) {
		Component_Animator	= Owner->Get_AnimatorComponent();
		Component_Transform = Owner->Get_TransformComponent();
		Component_NavMeshAgent = Owner->Get_NavMeshAgentComponent();
		StateMachine		= Owner->Get_StateMachine();

		JumpCount = Owner->Get_JumpCount();
		ObjectOnAir = Owner->Get_ObjectOnAir();

		PlayerCamera_Transform = static_pointer_cast<Transform>(GameInstance::GetInstance().Get_MainCamera()->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));

		InitializeFlag = false;
	}

	Component_Animator->Get_AnimationByIndex(static_cast<uint32_t>(CHANGLI::LAND_LIGHT))->Set_CurrentTime(0.f);
	Component_Animator->Get_AnimationByIndex(static_cast<uint32_t>(CHANGLI::LAND_HEAVY))->Set_CurrentTime(0.f);

	Component_Animator->Get_AnimationByIndex(static_cast<uint32_t>(CHANGLI::JUMP))->Set_CurrentTime(0.f);
	Component_Animator->Get_AnimationByIndex(static_cast<uint32_t>(CHANGLI::JUMP_SECOND_FRONT))->Set_CurrentTime(0.f);
	Component_Animator->Get_AnimationByIndex(static_cast<uint32_t>(CHANGLI::JUMP_SECOND_BACK))->Set_CurrentTime(0.f);

	if		(*JumpCount == 1) {
		Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::JUMP));
		Component_Animator->Set_AnimationBlending(true);
	}
	else if (*JumpCount == 2) { Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::JUMP_SECOND_FRONT)); }
	else if (*JumpCount == 3) { Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::JUMP_SECOND_BACK));  }

	*ObjectOnAir = static_cast<uint32_t>(JUMPSTATE::JUMPING);
}
VOID JumpState::FSM_StateUpdate(CONST _float& _DT) {
	_float OutHeight = 0.f, CellHeight = 0.f;
	

	XMFLOAT3 InputVector = { 0.f, 0.f, 0.f };

	if (KEY_HOLD(DIK_W)) InputVector.z += 1.f;
	if (KEY_HOLD(DIK_S)) InputVector.z -= 1.f;
	if (KEY_HOLD(DIK_A)) InputVector.x -= 1.f;
	if (KEY_HOLD(DIK_D)) InputVector.x += 1.f;

	XMVECTOR MoveInput = XMLoadFloat3(&InputVector);
	if (!XMVector3Equal(MoveInput, XMVectorZero())) {
		FLOAT	 MovementSpeed = 10.f;

		XMVECTOR CameraLook = XMVector3Normalize(XMVectorSetY(PlayerCamera_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK), 0.f));
		XMVECTOR CameraRight = XMVector3Normalize(XMVectorSetY(PlayerCamera_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT), 0.f));

		XMVECTOR MoveDirection = (InputVector.z * CameraLook) + (InputVector.x * CameraRight);
		XMVECTOR MoveDelta = MoveDirection * _DT * MovementSpeed;

		XMVECTOR	NextPosition = Component_Transform->Get_WorldPosition() + MoveDelta;
		_float		NextHeight = 0.f, CellHeight = 0.f;

		if (Component_NavMeshAgent->Evaluate_NextPositionCell(NextPosition, &NextHeight, &CellHeight, _DT)) {
			Component_Transform->Set_WorldPosition(NextPosition);
		}
	}
	
	
	if ((*ObjectOnAir == static_cast<uint32_t>(JUMPSTATE::JUMPING) && *ObjectOnAir != static_cast<uint32_t>(JUMPSTATE::FALLING)) &&
		(Component_Animator->Get_AnimationByIndex(static_cast<uint32_t>(CHANGLI::JUMP_SECOND_FRONT))->Get_AnimationProgress() > 0.4f ||
		Component_Animator->Get_AnimationByIndex(static_cast<uint32_t>(CHANGLI::JUMP_SECOND_BACK))->Get_AnimationProgress() > 0.4f ||
		Component_Animator->Get_AnimationByIndex(static_cast<uint32_t>(CHANGLI::JUMP))->Get_AnimationProgress() > 0.4f)) {
		*ObjectOnAir = static_cast<uint32_t>(JUMPSTATE::FALLING);
	}
	if (Component_Animator->Get_AnimationByIndex(static_cast<uint32_t>(CHANGLI::JUMP))->Get_AnimationProgress() >= 0.8f ||
		Component_Animator->Get_AnimationByIndex(static_cast<uint32_t>(CHANGLI::JUMP_SECOND_FRONT))->Get_AnimationProgress() >= 0.6f ||
		Component_Animator->Get_AnimationByIndex(static_cast<uint32_t>(CHANGLI::JUMP_SECOND_BACK))->Get_AnimationProgress() >= 0.6f) {
		StateMachine->FSM_StateChange(PLAYER_STATE::FALLING);
	}
  }
VOID JumpState::FSM_StateExit() {
	Owner->Set_ObjectOnAir(static_cast<uint32_t>(JUMPSTATE::FALLING));
}

VOID SkillState::FSM_StateEnter() {
	if (InitializeFlag) {
		Component_Animator		= Owner->Get_AnimatorComponent();
		Component_Transform		= Owner->Get_TransformComponent();
		Component_NavMeshAgent	= Owner->Get_NavMeshAgentComponent();

		StateMachine			= Owner->Get_StateMachine();
		PlayerCam				= static_pointer_cast<PlayerCamera>(GameInstance::GetInstance().Get_CameraByIndex(1));
		
		InitializeFlag = false;
	}
	Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::SKILL_START));
	Owner->Set_ObjectOnAir(true);
	Component_Animator->Set_AnimationBlending(true);
	HitBoxFlag = true;
}
VOID SkillState::FSM_StateUpdate(CONST _float& _DT) {
	if (*Owner->Get_ObjectOnAir() == false) {
		if (KEY_HOLD(DIK_W) || KEY_HOLD(DIK_A) || KEY_HOLD(DIK_S) || KEY_HOLD(DIK_D)) {
			if (KEY_HOLD(DIK_LSHIFT)) { StateMachine->FSM_StateChange(PLAYER_STATE::RUNNING); return; }
			StateMachine->FSM_StateChange(PLAYER_STATE::WALK); return;
		}
		if (KEY_HOLD(DIK_SPACE)) { StateMachine->FSM_StateChange(PLAYER_STATE::JUMP);	return; }
	}

	if (Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::SKILL_START)) == ANIMATION_STATE::ONRUNNING) {
		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::UP, 0.05f, _DT);
	}

	if (Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::SKILL_START)) == ANIMATION_STATE::FINISHED) {
		Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::SKILL_LOOP), true, 1.f);
		Component_Animator->Set_AnimationBlending(true);
	}
	if (Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::SKILL_LOOP)) == ANIMATION_STATE::ONRUNNING) {
		_float	 NextHeight = 0.f, CellHeight = 0.f;
		XMVECTOR NextPosition = Component_Transform->Get_WorldPosition() - XMVector3Normalize(Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_UP)) * 1.f;
		Component_NavMeshAgent->Evaluate_NextPositionCell(NextPosition, &NextHeight, &CellHeight, _DT);
		if (NextHeight < XMVectorGetY(NextPosition)) {
			Component_Transform->Set_WorldPosition(NextPosition);
		}
		else {
			Component_Transform->Set_WorldPosition(XMVectorSetY(NextPosition, NextHeight));
			Component_Animator->Set_AnimationState(static_cast<uint32_t>(CHANGLI::SKILL_LOOP), ANIMATION_STATE::FINISHED);

			Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::SKILL_END));
			Component_Animator->Set_AnimationBlending(true);
			Owner->Set_ObjectOnAir(false);

			PlayerCam->Camera_Drop(0.25f, { 0.f, -0.25f, 0.f }, { 0.f, -0.25f, 0.f });
			PlayerCam->Camera_Shake(0.35f, 0.35f);
		}
	}
	if (Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::SKILL_END)) == ANIMATION_STATE::FINISHED) {
		StateMachine->FSM_StateChange(PLAYER_STATE::IDLE); return;
	}
	if (HitBoxFlag) {

		XMVECTOR FSca = XMVectorSet(0.2f, 0.5f, 0.2f, 1.f);
		XMVECTOR ESca = XMVectorSet(0.2f, 3.5f, 0.2f, 1.f);
		for (uint32_t IDX = 0; IDX < 10; ++IDX) {
			XMVECTOR Pos = Component_Transform->Get_WorldPosition()
				- (Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK) * RANDOM(3.f, 1.f))
				+ (Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT) * RANDOM(1.f, -1.f))
				+ (Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_UP) * (_float)IDX * 0.75f);
			_float RotateValue = RANDOM(30.f, -30.f);
			RotateValue >= 0 ? RotateValue += 30.f : RotateValue -= 30.f;

			XMVECTOR TrailRot = XMVectorSet(XMConvertToRadians(180.f), 0.f, XMConvertToRadians(RotateValue), 0.f);

			Owner->Get_PlayerHitBoxPool()->Generate_HitBox(Pos, TrailRot, FSca, ESca, 0.1f * (IDX + 1), 0.f, 0.75f, RANDOM(9999, 1000), 0.f, FLOWTYPE::SMOOTHSTEP);
		}
		XMVECTOR FSca2 = XMVectorSet(1.f, 0.5f, 1.f, 1.f);
		XMVECTOR ESca2 = XMVectorSet(1.f, 11.5f, 1.f, 1.f);
		XMVECTOR ESca3 = XMVectorSet(5.f, 0.5f, 5.f, 1.f);
		Owner->Get_PlayerHitBoxPool()->Generate_HitBox(Component_Transform->Get_WorldPosition() + XMVectorSet(0.f, 10.f, 0.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 0.f)
			, FSca2, ESca2, 0.15f, 1.6f, 0.3f, RANDOM(9999, 1000), 0.f, FLOWTYPE::SMOOTHSTEP);

		XMVECTOR SlamPos = Component_Transform->Get_WorldPosition() + XMVectorSet(0.f, -2.f, 0.f, 0.f) 
			+ Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT) * 2.0f
			+ Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK)	 * 1.5f;
		Owner->Get_PlayerHitBoxPool()->Generate_HitBox(SlamPos, XMVectorSet(0.f, 0.f, 0.f, 0.f)
			, ESca3, ESca3, 0.2f, 1.8f, 0.3f, RANDOM(9999, 1000), 0.f, FLOWTYPE::SMOOTHSTEP);


		HitBoxFlag = false;
	}

	if (EffectFlag[0]) {
		_float Progress = Component_Animator->Get_CurrentAnimation()->Get_AnimationProgress();
		XMFLOAT3 AverageScale = { 1.f * 0.75f, 1.f * 0.75f, 0.75f * 0.75f };
		XMFLOAT2 ScrollSpeed = { -1.5f, 0.f };
		XMFLOAT2 TexOffset = { 1.f, 0.f };
		_float   AngleZ = -60.f;

		if (EffectFlag[1] && Progress >= 0.15f) {
			
			_float AngleX = RANDOM(55.f, 10.f), AngleY = RANDOM(280.f, 260.f);
			_float OffsetX = RANDOM(6.5f, 3.5f) + 6.f, OffsetY = RANDOM(15.f, 12.f), OffsetZ = RANDOM(1.5f, -1.5f);
			{
				auto BackEffect = Owner->Get_EffectProto("EFF_Skill_BackTrail");
				BackEffect->Set_EffectOrbitRotation(0.f, 0.f);
				BackEffect->Set_EffectRotation({ AngleX, AngleY, AngleZ });
				 
				BackEffect->Set_DissolveScrollSpeed(ScrollSpeed);
				BackEffect->Set_EffectScrollSpeed(ScrollSpeed);

				BackEffect->Set_DissolveTextureOffset(TexOffset);
				BackEffect->Set_EffectTextureOffset(TexOffset);
				BackEffect->Set_EmissiveOption({ 1.f ,0.2f, 0.2f }, 0.25f);

				BackEffect->Set_DissolveStrength(0.02f);
				BackEffect->Set_EffectScale(AverageScale);
				BackEffect->Set_EffectLifeTime(6.f);

				BackEffect->Set_DistanceFromPlayer(-1.f);
				BackEffect->Set_EffectOffset({ OffsetX, OffsetY, OffsetZ });

				BackEffect->Set_EffectOption(1);
				Owner->Play_Effect(BackEffect);
			}
			{
				auto FrontEffect = Owner->Get_EffectProto("EFF_Skill_FrontTrail");
				FrontEffect->Set_EffectOrbitRotation(0.f, 0.f);
				FrontEffect->Set_EffectRotation({ AngleX, AngleY, AngleZ });

				FrontEffect->Set_DissolveScrollSpeed(ScrollSpeed);
				FrontEffect->Set_EffectScrollSpeed(ScrollSpeed);

				FrontEffect->Set_DissolveTextureOffset(TexOffset);
				FrontEffect->Set_EffectTextureOffset(TexOffset);
				FrontEffect->Set_EmissiveOption({ 0.4f ,0.4f, 1.f }, 0.05f);

				FrontEffect->Set_DissolveStrength(0.02f);
				FrontEffect->Set_EffectScale(AverageScale);
				FrontEffect->Set_EffectLifeTime(6.f);

				FrontEffect->Set_DistanceFromPlayer(-1.f);
				FrontEffect->Set_EffectOffset({ OffsetX, OffsetY, OffsetZ });
				FrontEffect->Set_EffectOption(1);

				Owner->Play_Effect(FrontEffect);
			}
			EffectFlag[1] = false;
		}
		if (EffectFlag[2] && Progress >= 0.20f) {
			_float AngleX = RANDOM(55.f, 10.f), AngleY = RANDOM(100.f, 80.f);
			_float OffsetX = RANDOM(3.f, 0.f) - 9.f, OffsetY = RANDOM(15.f, 12.f), OffsetZ = RANDOM(1.5f, -1.5f);
			
			{
				auto BackEffect = Owner->Get_EffectProto("EFF_Skill_BackTrail");
				BackEffect->Set_EffectOrbitRotation(0.f, 0.f);
				BackEffect->Set_EffectRotation({ AngleX, AngleY, AngleZ });

				BackEffect->Set_DissolveScrollSpeed(ScrollSpeed);
				BackEffect->Set_EffectScrollSpeed(ScrollSpeed);

				BackEffect->Set_DissolveTextureOffset(TexOffset);
				BackEffect->Set_EffectTextureOffset(TexOffset);
				BackEffect->Set_EmissiveOption({ 1.f ,0.2f, 0.2f }, 0.25f);

				BackEffect->Set_DissolveStrength(0.02f);
				BackEffect->Set_EffectScale(AverageScale);
				BackEffect->Set_EffectLifeTime(6.f);

				BackEffect->Set_DistanceFromPlayer(-1.f);
				BackEffect->Set_EffectOffset({ OffsetX, OffsetY, OffsetZ });

				BackEffect->Set_EffectOption(1);
				Owner->Play_Effect(BackEffect);
			}
			{
				auto FrontEffect = Owner->Get_EffectProto("EFF_Skill_FrontTrail");
				FrontEffect->Set_EffectOrbitRotation(0.f, 0.f);
				FrontEffect->Set_EffectRotation({ AngleX, AngleY, AngleZ });

				FrontEffect->Set_DissolveScrollSpeed(ScrollSpeed);
				FrontEffect->Set_EffectScrollSpeed(ScrollSpeed);

				FrontEffect->Set_DissolveTextureOffset(TexOffset);
				FrontEffect->Set_EffectTextureOffset(TexOffset);
				FrontEffect->Set_EmissiveOption({ 0.4f ,0.4f, 1.f }, 0.05f);

				FrontEffect->Set_DissolveStrength(0.02f);
				FrontEffect->Set_EffectScale(AverageScale);
				FrontEffect->Set_EffectLifeTime(6.f);

				FrontEffect->Set_DistanceFromPlayer(-1.f);
				FrontEffect->Set_EffectOffset({ OffsetX, OffsetY, OffsetZ });

				FrontEffect->Set_EffectOption(1);
				Owner->Play_Effect(FrontEffect);
			}
			EffectFlag[2] = false;
		}
		if (EffectFlag[3] && Progress >= 0.25f) {
			_float AngleX = RANDOM(55.f, 10.f), AngleY = RANDOM(280.f, 260.f);
			_float OffsetX = RANDOM(3.f, 0.f) + 6.f, OffsetY = RANDOM(15.f, 12.f), OffsetZ = RANDOM(1.5f, -1.5f);

			{
				auto BackEffect = Owner->Get_EffectProto("EFF_Skill_BackTrail");
				BackEffect->Set_EffectOrbitRotation(0.f, 0.f);
				BackEffect->Set_EffectRotation({ AngleX, AngleY, AngleZ });

				BackEffect->Set_DissolveScrollSpeed(ScrollSpeed);
				BackEffect->Set_EffectScrollSpeed(ScrollSpeed);

				BackEffect->Set_DissolveTextureOffset(TexOffset);
				BackEffect->Set_EffectTextureOffset(TexOffset);
				BackEffect->Set_EmissiveOption({ 1.f ,0.2f, 0.2f }, 0.25f);

				BackEffect->Set_DissolveStrength(0.02f);
				BackEffect->Set_EffectScale(AverageScale);
				BackEffect->Set_EffectLifeTime(6.f);

				BackEffect->Set_DistanceFromPlayer(-1.f);
				BackEffect->Set_EffectOffset({ OffsetX, OffsetY, OffsetZ });

				BackEffect->Set_EffectOption(1);
				Owner->Play_Effect(BackEffect);
			}
			{
				auto FrontEffect = Owner->Get_EffectProto("EFF_Skill_FrontTrail");
				FrontEffect->Set_EffectOrbitRotation(0.f, 0.f);
				FrontEffect->Set_EffectRotation({ AngleX, AngleY, AngleZ });

				FrontEffect->Set_DissolveScrollSpeed(ScrollSpeed);
				FrontEffect->Set_EffectScrollSpeed(ScrollSpeed);

				FrontEffect->Set_DissolveTextureOffset(TexOffset);
				FrontEffect->Set_EffectTextureOffset(TexOffset);
				FrontEffect->Set_EmissiveOption({ 0.4f ,0.4f, 1.f }, 0.05f);

				FrontEffect->Set_DissolveStrength(0.02f);
				FrontEffect->Set_EffectScale(AverageScale);
				FrontEffect->Set_EffectLifeTime(6.f);

				FrontEffect->Set_DistanceFromPlayer(-1.f);
				FrontEffect->Set_EffectOffset({ OffsetX, OffsetY, OffsetZ });

				FrontEffect->Set_EffectOption(1);
				Owner->Play_Effect(FrontEffect);
			}
			EffectFlag[3] = false;
		}
		if (EffectFlag[4] && Progress >= 0.30f) {
			_float AngleX = RANDOM(55.f, 10.f), AngleY = RANDOM(100.f, 80.f);
			_float OffsetX = RANDOM(3.f, 0.f) - 9.f, OffsetY = RANDOM(15.f, 12.f), OffsetZ = RANDOM(1.5f, -1.5f);

			{
				auto BackEffect = Owner->Get_EffectProto("EFF_Skill_BackTrail");
				BackEffect->Set_EffectOrbitRotation(0.f, 0.f);
				BackEffect->Set_EffectRotation({ AngleX, AngleY, AngleZ });

				BackEffect->Set_DissolveScrollSpeed(ScrollSpeed);
				BackEffect->Set_EffectScrollSpeed(ScrollSpeed);

				BackEffect->Set_DissolveTextureOffset(TexOffset);
				BackEffect->Set_EffectTextureOffset(TexOffset);
				BackEffect->Set_EmissiveOption({ 1.f ,0.2f, 0.2f }, 0.25f);

				BackEffect->Set_DissolveStrength(0.02f);
				BackEffect->Set_EffectScale(AverageScale);
				BackEffect->Set_EffectLifeTime(6.f);

				BackEffect->Set_DistanceFromPlayer(-1.f);
				BackEffect->Set_EffectOffset({ OffsetX, OffsetY, OffsetZ });

				BackEffect->Set_EffectOption(1);
				Owner->Play_Effect(BackEffect);
			}
			{
				auto FrontEffect = Owner->Get_EffectProto("EFF_Skill_FrontTrail");
				FrontEffect->Set_EffectOrbitRotation(0.f, 0.f);
				FrontEffect->Set_EffectRotation({ AngleX, AngleY, AngleZ });

				FrontEffect->Set_DissolveScrollSpeed(ScrollSpeed);
				FrontEffect->Set_EffectScrollSpeed(ScrollSpeed);

				FrontEffect->Set_DissolveTextureOffset(TexOffset);
				FrontEffect->Set_EffectTextureOffset(TexOffset);
				FrontEffect->Set_EmissiveOption({ 0.4f ,0.4f, 1.f }, 0.05f);

				FrontEffect->Set_DissolveStrength(0.02f);
				FrontEffect->Set_EffectScale(AverageScale);
				FrontEffect->Set_EffectLifeTime(6.f);

				FrontEffect->Set_DistanceFromPlayer(-1.f);
				FrontEffect->Set_EffectOffset({ OffsetX, OffsetY, OffsetZ });

				FrontEffect->Set_EffectOption(1);
				Owner->Play_Effect(FrontEffect);
			}
			EffectFlag[4] = false;
		}
		if (EffectFlag[5] && Progress >= 0.35f) {
			_float AngleX = RANDOM(55.f, 10.f), AngleY = RANDOM(280.f, 260.f);
			_float OffsetX = RANDOM(3.f, 0.f) + 6.f, OffsetY = RANDOM(15.f, 12.f), OffsetZ = RANDOM(1.5f, -1.5f);
			{
				auto BackEffect = Owner->Get_EffectProto("EFF_Skill_BackTrail");
				BackEffect->Set_EffectOrbitRotation(0.f, 0.f);
				BackEffect->Set_EffectRotation({ AngleX, AngleY, AngleZ });

				BackEffect->Set_DissolveScrollSpeed(ScrollSpeed);
				BackEffect->Set_EffectScrollSpeed(ScrollSpeed);

				BackEffect->Set_DissolveTextureOffset(TexOffset);
				BackEffect->Set_EffectTextureOffset(TexOffset);
				BackEffect->Set_EmissiveOption({ 1.f ,0.2f, 0.2f }, 0.25f);

				BackEffect->Set_DissolveStrength(0.02f);
				BackEffect->Set_EffectScale(AverageScale);
				BackEffect->Set_EffectLifeTime(6.f);

				BackEffect->Set_DistanceFromPlayer(-1.f);
				BackEffect->Set_EffectOffset({ OffsetX, OffsetY, OffsetZ });

				BackEffect->Set_EffectOption(1);
				Owner->Play_Effect(BackEffect);
			}
			{
				auto FrontEffect = Owner->Get_EffectProto("EFF_Skill_FrontTrail");
				FrontEffect->Set_EffectOrbitRotation(0.f, 0.f);
				FrontEffect->Set_EffectRotation({ AngleX, AngleY, AngleZ });

				FrontEffect->Set_DissolveScrollSpeed(ScrollSpeed);
				FrontEffect->Set_EffectScrollSpeed(ScrollSpeed);

				FrontEffect->Set_DissolveTextureOffset(TexOffset);
				FrontEffect->Set_EffectTextureOffset(TexOffset);
				FrontEffect->Set_EmissiveOption({ 0.4f ,0.4f, 1.f }, 0.05f);

				FrontEffect->Set_DissolveStrength(0.02f);
				FrontEffect->Set_EffectScale(AverageScale);
				FrontEffect->Set_EffectLifeTime(6.f);

				FrontEffect->Set_DistanceFromPlayer(-1.f);
				FrontEffect->Set_EffectOffset({ OffsetX, OffsetY, OffsetZ });

				FrontEffect->Set_EffectOption(1);
				Owner->Play_Effect(FrontEffect);
			}
			EffectFlag[5] = false;
		}
		if (EffectFlag[6] && Progress >= 0.40f) {
			_float AngleX = RANDOM(55.f, 10.f), AngleY = RANDOM(100.f, 80.f);
			_float OffsetX = RANDOM(3.f, 0.f) - 9.f, OffsetY = RANDOM(15.f, 12.f), OffsetZ = RANDOM(1.5f, -1.5f);

			{
				auto BackEffect = Owner->Get_EffectProto("EFF_Skill_BackTrail");
				BackEffect->Set_EffectOrbitRotation(0.f, 0.f);
				BackEffect->Set_EffectRotation({ AngleX, AngleY, AngleZ });

				BackEffect->Set_DissolveScrollSpeed(ScrollSpeed);
				BackEffect->Set_EffectScrollSpeed(ScrollSpeed);

				BackEffect->Set_DissolveTextureOffset(TexOffset);
				BackEffect->Set_EffectTextureOffset(TexOffset);
				BackEffect->Set_EmissiveOption({ 1.f ,0.2f, 0.2f }, 0.25f);

				BackEffect->Set_DissolveStrength(0.02f);
				BackEffect->Set_EffectScale(AverageScale);
				BackEffect->Set_EffectLifeTime(6.f);

				BackEffect->Set_DistanceFromPlayer(-1.f);
				BackEffect->Set_EffectOffset({ OffsetX, OffsetY, OffsetZ });

				BackEffect->Set_EffectOption(1);
				Owner->Play_Effect(BackEffect);
			}
			{
				auto FrontEffect = Owner->Get_EffectProto("EFF_Skill_FrontTrail");
				FrontEffect->Set_EffectOrbitRotation(0.f, 0.f);
				FrontEffect->Set_EffectRotation({ AngleX, AngleY, AngleZ });

				FrontEffect->Set_DissolveScrollSpeed(ScrollSpeed);
				FrontEffect->Set_EffectScrollSpeed(ScrollSpeed);

				FrontEffect->Set_DissolveTextureOffset(TexOffset);
				FrontEffect->Set_EffectTextureOffset(TexOffset);
				FrontEffect->Set_EmissiveOption({ 1.f ,0.2f, 0.2f }, 0.05f);

				FrontEffect->Set_DissolveStrength(0.02f);
				FrontEffect->Set_EffectScale(AverageScale);
				FrontEffect->Set_EffectLifeTime(6.f);

				FrontEffect->Set_DistanceFromPlayer(-1.f);
				FrontEffect->Set_EffectOffset({ OffsetX, OffsetY, OffsetZ });

				FrontEffect->Set_EffectOption(1);
				Owner->Play_Effect(FrontEffect);
			}

			EffectFlag[6] = false;
		}
		if (EffectFlag[7] && Progress >= 0.45f) {
			_float AngleX = RANDOM(55.f, 10.f), AngleY = RANDOM(280.f, 260.f);
			_float OffsetX = RANDOM(3.f, 0.f) + 6.f, OffsetY = RANDOM(15.f, 12.f), OffsetZ = RANDOM(1.5f, -1.5f);
			{
				auto BackEffect = Owner->Get_EffectProto("EFF_Skill_BackTrail");
				BackEffect->Set_EffectOrbitRotation(0.f, 0.f);
				BackEffect->Set_EffectRotation({ AngleX, AngleY, AngleZ });

				BackEffect->Set_DissolveScrollSpeed(ScrollSpeed);
				BackEffect->Set_EffectScrollSpeed(ScrollSpeed);

				BackEffect->Set_DissolveTextureOffset(TexOffset);
				BackEffect->Set_EffectTextureOffset(TexOffset);
				BackEffect->Set_EmissiveOption({ 1.f ,0.2f, 0.2f }, 0.25f);

				BackEffect->Set_DissolveStrength(0.02f);
				BackEffect->Set_EffectScale(AverageScale);
				BackEffect->Set_EffectLifeTime(6.f);

				BackEffect->Set_DistanceFromPlayer(-1.f);
				BackEffect->Set_EffectOffset({ OffsetX, OffsetY, OffsetZ });

				BackEffect->Set_EffectOption(1);
				Owner->Play_Effect(BackEffect);
			}
			{
				auto FrontEffect = Owner->Get_EffectProto("EFF_Skill_FrontTrail");
				FrontEffect->Set_EffectOrbitRotation(0.f, 0.f);
				FrontEffect->Set_EffectRotation({ AngleX, AngleY, AngleZ });

				FrontEffect->Set_DissolveScrollSpeed(ScrollSpeed);
				FrontEffect->Set_EffectScrollSpeed(ScrollSpeed);

				FrontEffect->Set_DissolveTextureOffset(TexOffset);
				FrontEffect->Set_EffectTextureOffset(TexOffset);
				FrontEffect->Set_EmissiveOption({ 1.f ,0.2f, 0.2f }, 0.05f);

				FrontEffect->Set_DissolveStrength(0.02f);
				FrontEffect->Set_EffectScale(AverageScale);
				FrontEffect->Set_EffectLifeTime(6.f);

				FrontEffect->Set_DistanceFromPlayer(-1.f);
				FrontEffect->Set_EffectOffset({ OffsetX, OffsetY, OffsetZ });

				FrontEffect->Set_EffectOption(1);
				Owner->Play_Effect(FrontEffect);
			}

			EffectFlag[7] = false;
		}
		if (EffectFlag[8] && Progress >= 0.55f) {
			auto BackEffect = Owner->Get_EffectProto("EFF_Skill_Charging");
			BackEffect->Set_EffectOrbitRotation(0.f, 0.f);
			BackEffect->Set_EffectRotation({ 0.f, 0.f, 0.f });

			BackEffect->Set_DissolveScrollSpeed({ -3.f, 0.f });
			BackEffect->Set_EffectScrollSpeed({ -3.f, 0.f });
			BackEffect->Set_EmissiveOption({ 1.f ,0.f, 0.f }, 0.25f);

			BackEffect->Set_DissolveTextureOffset({ 1.f, 0.f });
			BackEffect->Set_EffectTextureOffset({ 1.f, 0.f });

			BackEffect->Set_DissolveStrength(0.02f);
			BackEffect->Set_EffectScale({ 5.f, 5.f, 5.f });
			BackEffect->Set_EffectLifeTime(2.f);

			BackEffect->Set_DistanceFromPlayer(0.f);
			BackEffect->Set_EffectOffset({ 0.f, 3.5f, 0.f });
			Owner->Play_Effect(BackEffect);

			EffectFlag[8] = false;
		}
	}
}
VOID SkillState::FSM_StateExit() {

	for (uint32_t IDX = 0; IDX < 10; IDX++) 
		EffectFlag[IDX] = true;
	
	HitBoxFlag = true;
}

VOID BoostState::FSM_StateEnter() {
	if (InitializeFlag) {
		Component_Animator		= Owner->Get_AnimatorComponent();
		Component_Transform		= Owner->Get_TransformComponent();
		Component_NavMeshAgent	= Owner->Get_NavMeshAgentComponent();

		StateMachine			= Owner->Get_StateMachine();
		PlayerCam = static_pointer_cast<PlayerCamera>(GameInstance::GetInstance().Get_CameraByIndex(1));

		InitializeFlag = false;
	}
	if (Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::ATTACK_BOOST)) == ANIMATION_STATE::ONREADY) {
		Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::ATTACK_BOOST));
		Component_Animator->Set_AnimationBlending(true);
	}
	FOVInFlag = true; FOVOutFlag = true;
	HitBoxFlag = true; EffectFlag = true;
}
VOID BoostState::FSM_StateUpdate(CONST _float& _DT) {
	_float Progress = Component_Animator->Get_CurrentAnimation()->Get_AnimationProgress();

	if (Progress >= 0.75f) {
		if (KEY_HOLD(DIK_W) || KEY_HOLD(DIK_A) || KEY_HOLD(DIK_S) || KEY_HOLD(DIK_D)) {
			if (KEY_HOLD(DIK_LSHIFT)) { StateMachine->FSM_StateChange(PLAYER_STATE::RUNNING); return; }
			StateMachine->FSM_StateChange(PLAYER_STATE::WALK); return;
		}
		if (KEY_HOLD(DIK_SPACE)) { StateMachine->FSM_StateChange(PLAYER_STATE::JUMP); return; }
	}

	if (FOVOutFlag && Progress >= 0.15f) {
		PlayerCam->Camera_Shake(0.05f, 0.15f);
		PlayerCam->EaseOut_FOV(0.2f, 2.f, FOVPROGRESS::FOV_OUT);
		FOVOutFlag = false;
	}
	if (FOVInFlag && Progress >= 0.8f) {
		PlayerCam->EaseOut_FOV(0.2f, 2.f, FOVPROGRESS::FOV_IN);
		FOVInFlag = false;
	}
	if (Progress >= 0.9f) {
		StateMachine->FSM_StateChange(PLAYER_STATE::IDLE); return;
	}
	if (EffectFlag && Progress >= 0.15f) {
		for (uint32_t IDX = 1; IDX < 12; ++IDX) {
			//if (IDX == ) continue;
			auto CoreEffect1 = Owner->Get_EffectProto("EFF_Boost_CoreFeather_v2");		
			auto InnerEffect1 = Owner->Get_EffectProto("EFF_Boost_InnerFlame_v2");
			auto OuterEffect1 = Owner->Get_EffectProto("EFF_Boost_OuterFlame_v2");

			CoreEffect1 ->Set_EffectOrbitRotation(30.f * IDX, 0.f);
			InnerEffect1->Set_EffectOrbitRotation(30.f * IDX, 0.f);
			OuterEffect1->Set_EffectOrbitRotation(30.f * IDX, 0.f);

			CoreEffect1 ->Set_EffectRotation({ 0.f, -90.f, 0.f});
			InnerEffect1->Set_EffectRotation({ 0.f, -90.f, 0.f});
			OuterEffect1->Set_EffectRotation({ 0.f, -90.f, 0.f});

			_float RandomValue = RANDOM(0.5f, 0.3f);
			CoreEffect1 ->Set_DissolveScrollSpeed({ -0.5f - RandomValue, 0.f });
			InnerEffect1->Set_DissolveScrollSpeed({ -0.5f - RandomValue, 0.f });
			OuterEffect1->Set_DissolveScrollSpeed({ -0.5f - RandomValue, 0.f });

			CoreEffect1 ->Set_EffectScrollSpeed({ -0.5f - RandomValue, 0.f });
			InnerEffect1->Set_EffectScrollSpeed({ -0.5f - RandomValue, 0.f });
			OuterEffect1->Set_EffectScrollSpeed({ -0.5f - RandomValue, 0.f });

			CoreEffect1 ->Set_DissolveTextureOffset({ 0.4f, 0.f });
			InnerEffect1->Set_DissolveTextureOffset({ 0.4f, 0.f });
			OuterEffect1->Set_DissolveTextureOffset({ 0.4f, 0.f });
			
			CoreEffect1 ->Set_EffectTextureOffset({ 0.6f, 0.f });
			InnerEffect1->Set_EffectTextureOffset({ 0.6f, 0.f });
			OuterEffect1->Set_EffectTextureOffset({ 0.6f, 0.f });

			CoreEffect1->Set_DissolveStrength (0.02f);
			InnerEffect1->Set_DissolveStrength(0.02f);
			OuterEffect1->Set_DissolveStrength(0.02f);

			CoreEffect1->Set_EffectScale ({ RandomValue + 8.f, RandomValue + 8.f, RandomValue + 8.f });
			InnerEffect1->Set_EffectScale({ RandomValue + 8.f, RandomValue + 8.f, RandomValue + 8.f });
			OuterEffect1->Set_EffectScale({ RandomValue + 8.f, RandomValue + 8.f, RandomValue + 8.f });
			
			CoreEffect1->Set_EffectLifeTime (2.f + RandomValue / 2.f);
			InnerEffect1->Set_EffectLifeTime(2.f + RandomValue / 2.f);
			OuterEffect1->Set_EffectLifeTime(2.f + RandomValue / 2.f);
			
			Owner->Play_Effect(CoreEffect1);
			Owner->Play_Effect(InnerEffect1);
			Owner->Play_Effect(OuterEffect1);
		}
		EffectFlag = false;
	}
	if (HitBoxFlag) {
		_float NextHeight = 0.f, CellHeight = 0.f;
		XMVECTOR PosA = Component_Transform->Get_WorldPosition() + XMVectorSet(0.f, 0.5f, 0.f, 0.f);
		Component_NavMeshAgent->Evaluate_NextPositionCell(PosA, &NextHeight, &CellHeight, _DT);

		XMVECTOR FSca1 = XMVectorSet(0.3f, 0.3f, +0.5f, 1.f);
		XMVECTOR ESca1 = XMVectorSet(0.3f, 0.3f, +7.5f, 1.f);

		XMVECTOR FSca2 = XMVectorSet(1.f, 0.5f, 0.3f, 1.f);
		XMVECTOR ESca2 = XMVectorSet(1.f, 5.0f, 0.3f, 1.f);
		
		for (uint32_t IDX = 0; IDX < 12; ++IDX) {
			XMVECTOR TrailRot = XMVectorSet(0.f, XMConvertToRadians(+180.f - 30.f * IDX), 0.f, 0.f);
			XMVECTOR FlameRot = XMVectorSet(XMConvertToRadians(180.f), XMConvertToRadians(+180.f - 30.f * IDX), 0.f, 0.f);

			XMVECTOR RotatedVector = XMVector3Rotate(-Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK), XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(+180.f - 30.f * IDX)));

			XMVECTOR PosB = Component_Transform->Get_WorldPosition() + XMVector3Normalize(RotatedVector) * 8.f + XMVectorSet(0.f, 0.5f, 0.f, 0.f);
			
			Owner->Get_PlayerHitBoxPool()->Generate_HitBox(XMVectorSetY(PosA, CellHeight - 1.5f), TrailRot, FSca1, ESca1, 0.05f * (IDX + 1), 0.f, 1.5f, RANDOM(9999, 1000), 0.f, FLOWTYPE::SMOOTHSTEP);
			Owner->Get_PlayerHitBoxPool()->Generate_HitBox(XMVectorSetY(PosB, CellHeight - 1.5f), FlameRot, FSca2, ESca2, 0.35f, 1.05f, 0.25f, RANDOM(9999, 1000), 0.f, FLOWTYPE::SMOOTHSTEP);
		}
		HitBoxFlag = false;
	}
}
VOID BoostState::FSM_StateExit() {
	FOVInFlag = true; FOVOutFlag = true;
	HitBoxFlag = true; EffectFlag = true;
}

VOID UltimateState::FSM_StateEnter() {
	if (InitializeFlag) {
		Component_Animator = Owner->Get_AnimatorComponent();
		Component_Transform = Owner->Get_TransformComponent();
		Component_NavMeshAgent = Owner->Get_NavMeshAgentComponent();
		PlayerCam = static_pointer_cast<PlayerCamera>(GameInstance::GetInstance().Get_CameraByIndex(1));
		ActionCam = static_pointer_cast<ActionCamera>(GameInstance::GetInstance().Get_CameraByIndex(2));

		InitializeFlag = false;
	}
	ActionCam->Reset_ChangLi_CutScene();
	
	if (Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::ATTACK_ULTIMATE)) == ANIMATION_STATE::ONREADY) {
		Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::ATTACK_ULTIMATE));
		GameInstance::GetInstance().Set_MainCamera(2);
		ActionCam->Play_ChangLi_CutScene(1);
	}
	HitBoxFlag = true; EffectFlag = true;
}
VOID UltimateState::FSM_StateUpdate(CONST _float& _DT) {
	
	if (Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::ATTACK_ULTIMATE)) == ANIMATION_STATE::FINISHED) {
		Owner->Get_StateMachine()->FSM_StateChange(PLAYER_STATE::IDLE); return;
	}
	_float Progress = Component_Animator->Get_CurrentAnimation()->Get_AnimationProgress();

	if		(Progress >= 0.15f && Progress <= 0.16f) {
		ActionCam->Play_ChangLi_CutScene(2);
	}
	else if (Progress >= 0.325f && Progress <= 0.33f) {
		GameInstance::GetInstance().Set_MainCamera(1);
		PlayerCam->Set_CameraFallBack(true);
	}
	else if (Progress <= 0.341f && Progress >= 0.34f && !PlayerCam->IsCameraDroping() ) {
		PlayerCam->Camera_Shake(0.5f, 0.25f);
		PlayerCam->Camera_Drop(0.5f, { 0.f, -0.5f, 0.f }, { 0.f, -0.2f, 0.f });
		Component_Animator->Get_CurrentAnimation()->Set_AnimationSpeed(2.5f);
	}
	if (Progress > 0.35f && HitBoxFlag) {
		_float NextHeight = 0.f, CellHeight = 0.f;
		XMVECTOR Pos = Component_Transform->Get_WorldPosition() + XMVectorSet(0.f, 0.5f, 0.f, 0.f);
		XMVECTOR Rot = XMVectorSet(XMConvertToRadians(90.f), 0.f, 0.f, 0.f);
		Component_NavMeshAgent->Evaluate_NextPositionCell(Pos, &NextHeight, &CellHeight, _DT);

		XMVECTOR FSca = XMVectorSet(7.5f, 7.5f, +0.5f, 1.f);
		XMVECTOR ESca = XMVectorSet(7.5f, 7.5f, +0.5f, 1.f);

		Owner->Get_PlayerHitBoxPool()->Generate_HitBox(XMVectorSetY(Pos, CellHeight - 1.5f), Rot, FSca, ESca, 0.2f, 0.f, 0.2f, RANDOM(15000, 9999), 0.2f, FLOWTYPE::SMOOTHSTEP);
		HitBoxFlag = false;
	}
	if (EffectFlag && Progress >= 0.35f) {
		for (uint32_t IDX = 1; IDX < 12; ++IDX) {
			//if (IDX == 9) continue;
			auto CoreEffect1 = Owner->Get_EffectProto("EFF_Boost_CoreFeather_v1");
			auto InnerEffect1 = Owner->Get_EffectProto("EFF_Boost_InnerFlame_v1");
			auto OuterEffect1 = Owner->Get_EffectProto("EFF_Boost_OuterFlame_v1");

			CoreEffect1->Set_EffectOrbitRotation(30.f * IDX, 0.f);
			InnerEffect1->Set_EffectOrbitRotation(30.f * IDX, 0.f);
			OuterEffect1->Set_EffectOrbitRotation(30.f * IDX, 0.f);

			CoreEffect1->Set_EffectRotation({ 0.f, -90.f, 0.f });
			InnerEffect1->Set_EffectRotation({ 0.f, -90.f, 0.f });
			OuterEffect1->Set_EffectRotation({ 0.f, -90.f, 0.f });

			_float RandomValue = RANDOM(0.5f, 0.3f);
			CoreEffect1->Set_DissolveScrollSpeed({ -0.5f - RandomValue, 0.f });
			InnerEffect1->Set_DissolveScrollSpeed({ -0.5f - RandomValue, 0.f });
			OuterEffect1->Set_DissolveScrollSpeed({ -0.5f - RandomValue, 0.f });

			CoreEffect1->Set_EffectScrollSpeed({ -0.5f - RandomValue, 0.f });
			InnerEffect1->Set_EffectScrollSpeed({ -0.5f - RandomValue, 0.f });
			OuterEffect1->Set_EffectScrollSpeed({ -0.5f - RandomValue, 0.f });

			CoreEffect1->Set_DissolveTextureOffset({ 0.4f, 0.f });
			InnerEffect1->Set_DissolveTextureOffset({ 0.4f, 0.f });
			OuterEffect1->Set_DissolveTextureOffset({ 0.4f, 0.f });

			CoreEffect1->Set_EffectTextureOffset({ 0.6f, 0.f });
			InnerEffect1->Set_EffectTextureOffset({ 0.6f, 0.f });
			OuterEffect1->Set_EffectTextureOffset({ 0.6f, 0.f });

			CoreEffect1->Set_DissolveStrength(0.02f);
			InnerEffect1->Set_DissolveStrength(0.02f);
			OuterEffect1->Set_DissolveStrength(0.02f);

			CoreEffect1->Set_EffectScale({ RandomValue + 8.f, RandomValue + 8.f, RandomValue + 8.f });
			InnerEffect1->Set_EffectScale({ RandomValue + 8.f, RandomValue + 8.f, RandomValue + 8.f });
			OuterEffect1->Set_EffectScale({ RandomValue + 8.f, RandomValue + 8.f, RandomValue + 8.f });

			CoreEffect1->Set_EffectLifeTime(2.f + RandomValue / 2.f);
			InnerEffect1->Set_EffectLifeTime(2.f + RandomValue / 2.f);
			OuterEffect1->Set_EffectLifeTime(2.f + RandomValue / 2.f);

			Owner->Play_Effect(CoreEffect1);
			Owner->Play_Effect(InnerEffect1);
			Owner->Play_Effect(OuterEffect1);
		}
		EffectFlag = false;
	}
}
VOID UltimateState::FSM_StateExit() {
	HitBoxFlag = true; EffectFlag = true;
}

VOID FallingState::FSM_StateEnter() {
	if (InitializeFlag) {
		Component_Animator	= Owner->Get_AnimatorComponent();
		Component_Transform = Owner->Get_TransformComponent();
		Component_NavMeshAgent = Owner->Get_NavMeshAgentComponent();
		StateMachine		= Owner->Get_StateMachine();
		PlayerCamera_Transform = static_pointer_cast<Transform>(GameInstance::GetInstance().Get_MainCamera()->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));

		JumpCount			= Owner->Get_JumpCount();
		ObjectOnAir			= Owner->Get_ObjectOnAir();

		InitializeFlag = false;
	}
	JumpGravity = 0.25f;
	*ObjectOnAir = static_cast<uint32_t>(JUMPSTATE::FALLING);
}
VOID FallingState::FSM_StateUpdate(const _float& _DT) {

	JumpGravity += _DT * 0.5f;

	if (*ObjectOnAir == static_cast<uint32_t>(JUMPSTATE::LAND)) {
		StateMachine->FSM_StateChange(PLAYER_STATE::LAND); 
		return;
	}

	XMFLOAT3 InputVector = { 0.f, 0.f, 0.f };

	if (KEY_HOLD(DIK_W)) InputVector.z += 1.f;
	if (KEY_HOLD(DIK_S)) InputVector.z -= 1.f;
	if (KEY_HOLD(DIK_A)) InputVector.x -= 1.f;
	if (KEY_HOLD(DIK_D)) InputVector.x += 1.f;

	XMVECTOR MoveInput = XMLoadFloat3(&InputVector);
	if (!XMVector3Equal(MoveInput, XMVectorZero())) {
		FLOAT	 MovementSpeed = 10.f;

		XMVECTOR CameraLook = XMVector3Normalize(XMVectorSetY(PlayerCamera_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK), 0.f));
		XMVECTOR CameraRight = XMVector3Normalize(XMVectorSetY(PlayerCamera_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_RIGHT), 0.f));

		XMVECTOR MoveDirection = (InputVector.z * CameraLook) + (InputVector.x * CameraRight);
		XMVECTOR MoveDelta = MoveDirection * _DT * MovementSpeed;

		XMVECTOR	NextPosition = Component_Transform->Get_WorldPosition() + MoveDelta;
		_float		NextHeight = 0.f, CellHeight = 0.f;

		if (Component_NavMeshAgent->Evaluate_NextPositionCell(NextPosition, &NextHeight, &CellHeight, _DT)) {
			Component_Transform->Set_WorldPosition(NextPosition);
		}
	}

	if ((Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::FALL_LOOP)) == ANIMATION_STATE::ONREADY ||
		Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::FALL_LOOP))  == ANIMATION_STATE::FINISHED) &&
		*ObjectOnAir == static_cast<uint32_t>(JUMPSTATE::FALLING)) {
		Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::FALL_LOOP), true);
		Component_Animator->Set_AnimationBlending(true);
	}
	if (Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::FALL_LOOP)) == ANIMATION_STATE::ONRUNNING && *ObjectOnAir == static_cast<uint32_t>(JUMPSTATE::FALLING)) {
		Component_Transform->Append_WorldPosition(DIRECTION_TYPE::DOWN, SMOOTHSTEP(JumpGravity), _DT);
	}
	if ((*ObjectOnAir == static_cast<uint32_t>(JUMPSTATE::JUMPING) || *ObjectOnAir == static_cast<uint32_t>(JUMPSTATE::FALLING)) && MOUSE_LBUTTON) {
		StateMachine->FSM_StateChange(PLAYER_STATE::AIRATTACK);
	}
}
VOID FallingState::FSM_StateExit()
{
	JumpGravity = 0.f;
}

VOID AirAttackState::FSM_StateEnter() {
	if (InitializeFlag) {
		Component_Animator		= Owner->Get_AnimatorComponent();
		Component_Transform		= Owner->Get_TransformComponent();
		Component_NavMeshAgent	= Owner->Get_NavMeshAgentComponent();
		StateMachine			= Owner->Get_StateMachine();

		JumpCount	= Owner->Get_JumpCount();
		AttackCombo = Owner->Get_AttackCombo();

		InitializeFlag = false;
	}
	Owner->Set_ObjectOnAir(true);
}
VOID AirAttackState::FSM_StateUpdate(const _float& _DT)
{
	if (*Owner->Get_ObjectOnAir() == static_cast<uint32_t>(JUMPSTATE::JUMPING) || *Owner->Get_ObjectOnAir() == static_cast<uint32_t>(JUMPSTATE::FALLING)) {
		if (Component_Animator->Get_CurrentAnimation()->Get_AnimationProgress() >= 0.8) {
			if (KEY_HOLD(DIK_W) || KEY_HOLD(DIK_A) || KEY_HOLD(DIK_S) || KEY_HOLD(DIK_D)) {
				Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::FALL_LOOP));
				Component_Animator->Set_AnimationBlending(true);
				StateMachine->FSM_StateChange(PLAYER_STATE::FALLING);
				return;
			}
		}
		if (*AttackCombo >= 6 && KEY_HOLD(DIK_S)) {
			Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::ATTACK_SLAM_START));
			Component_Animator->Set_AnimationBlending(true);
			StateMachine->FSM_StateChange(PLAYER_STATE::SLAM);
		}
		else if (*AttackCombo == 7 && Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::AIR_ATTACK01)) == ANIMATION_STATE::ONREADY) {
			Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::AIR_ATTACK01));
			Component_Animator->Set_AnimationBlending(true);
		}
		else if (*AttackCombo == 8 && Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::AIR_ATTACK02)) == ANIMATION_STATE::ONREADY) {
			Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::AIR_ATTACK02));
			Component_Animator->Set_AnimationBlending(true);
		}
		else if (*AttackCombo == 9 && Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::AIR_ATTACK03)) == ANIMATION_STATE::ONREADY) {
			Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::AIR_ATTACK03));
			Component_Animator->Set_AnimationBlending(true);
		}
		else if (*AttackCombo == 10 && Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::AIR_ATTACK04)) == ANIMATION_STATE::ONREADY) {
			Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::AIR_ATTACK04));
			Component_Animator->Set_AnimationBlending(true);
		}
		else if (*AttackCombo == 11 && Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::AIR_ATTACK05)) == ANIMATION_STATE::ONREADY) {
			Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::AIR_ATTACK05));
			Component_Animator->Set_AnimationBlending(true);
		}
		else if (*AttackCombo >= 12 ) {
			StateMachine->FSM_StateChange(PLAYER_STATE::SLAM);
			return;
		}
		
		if (Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::AIR_ATTACK01)) == ANIMATION_STATE::FINISHED ||
			Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::AIR_ATTACK02)) == ANIMATION_STATE::FINISHED ||
			Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::AIR_ATTACK03)) == ANIMATION_STATE::FINISHED ||
			Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::AIR_ATTACK04)) == ANIMATION_STATE::FINISHED ||
			Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::AIR_ATTACK05)) == ANIMATION_STATE::FINISHED) {
			Owner->Set_ObjectOnAir(static_cast<uint32_t>(JUMPSTATE::FALLING));
			Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::FALL_LOOP));
			Component_Animator->Set_AnimationBlending(true);
			StateMachine->FSM_StateChange(PLAYER_STATE::FALLING);
		}
	}
	else {
		StateMachine->FSM_StateChange(PLAYER_STATE::IDLE);
	}
}
VOID AirAttackState::FSM_StateExit() {

}

VOID SlamState::FSM_StateEnter() {
	if (InitializeFlag) {
		Component_Animator = Owner->Get_AnimatorComponent();
		Component_Transform = Owner->Get_TransformComponent();
		Component_NavMeshAgent = Owner->Get_NavMeshAgentComponent();
		StateMachine = Owner->Get_StateMachine();

		JumpCount = Owner->Get_JumpCount();
		AttackCombo = Owner->Get_AttackCombo();
		PlayerCam = static_pointer_cast<PlayerCamera>(GameInstance::GetInstance().Get_MainCamera());

		InitializeFlag = false;
	}
	if (SlamStateTempo == 2) {
		SlamStateTempo = 0;
	}
}
VOID SlamState::FSM_StateUpdate(const _float& _DT)
{
	if (SlamStateTempo == 0) {
		if (Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::ATTACK_SLAM_START)) == ANIMATION_STATE::ONREADY) {
			Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::ATTACK_SLAM_START));
			Component_Animator->Set_AnimationBlending(true);
		}
		if (Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::ATTACK_SLAM_START)) == ANIMATION_STATE::FINISHED) {
			SlamStateTempo = 1;
		}
	}
	if (SlamStateTempo == 1) {
		if (Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::ATTACK_SLAM_LOOP)) == ANIMATION_STATE::ONREADY) {
			Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::ATTACK_SLAM_LOOP), true, 2.1f);
			Component_Animator->Set_AnimationBlending(true);
		}
		if (Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::ATTACK_SLAM_LOOP)) == ANIMATION_STATE::ONRUNNING) {
			_float	 NextHeight = 0.f;
			int32_t CellIndex = -1;
			XMVECTOR NextPosition = Component_Transform->Get_WorldPosition() - XMVector3Normalize(Component_Transform->Get_WorldTransform(VECTOR_TYPE::VECTOR_UP)) * 0.25f;
			Component_NavMeshAgent->RayCast_NextPosition(NextPosition, &NextHeight, &CellIndex);
			if (NextHeight + 0.25f < XMVectorGetY(NextPosition)) {
				Component_Transform->Set_WorldPosition(NextPosition);
			}
			else {
				Component_Transform->Set_WorldPosition(XMVectorSetY(NextPosition, NextHeight));
				Component_Animator->Set_AnimationState(static_cast<uint32_t>(CHANGLI::ATTACK_SLAM_LOOP), ANIMATION_STATE::FINISHED);
				SlamStateTempo = 2;
			}
		}
	}
	if (SlamStateTempo == 2) {
		if (Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::ATTACK_SLAM_END)) == ANIMATION_STATE::ONREADY) {
			Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::ATTACK_SLAM_END));
			Component_Animator->Set_AnimationBlending(true);
		}
		if (Component_Animator->Get_AnimationState(static_cast<uint32_t>(CHANGLI::ATTACK_SLAM_END)) == ANIMATION_STATE::ONRUNNING) {
			_float Progress = Component_Animator->Get_CurrentAnimation()->Get_AnimationProgress();

			Owner->Set_ObjectOnAir(static_cast<uint32_t>(JUMPSTATE::READY));
			Owner->Set_JumpCount(0);
			*AttackCombo = 0;

			if (Progress >= 0.35f) {
				if (KEY_HOLD(DIK_W) || KEY_HOLD(DIK_A) || KEY_HOLD(DIK_D) || KEY_HOLD(DIK_S)) {
					if (KEY_HOLD(DIK_LSHIFT)) {
						Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::RUN_FRONT), true);
						Component_Animator->Set_AnimationBlending(true);
						StateMachine->FSM_StateChange(PLAYER_STATE::RUNNING); return;
					}
					Component_Animator->Play_Animation(static_cast<uint32_t>(CHANGLI::WALK_FRONT), true);
					Component_Animator->Set_AnimationBlending(true);
					StateMachine->FSM_StateChange(PLAYER_STATE::WALK); return;
				}
			}
			if (Progress >= 0.85f) {
				StateMachine->FSM_StateChange(PLAYER_STATE::IDLE);
				*AttackCombo = 0;
				Owner->Set_ObjectOnAir(static_cast<uint32_t>(JUMPSTATE::READY));
				Owner->Set_JumpCount(0);
				return;
			}
			if (Progress >= 0.025f && Progress <= 0.05f && !PlayerCam->IsCameraDroping() && !PlayerCam->IsCameraShaking()) {
				PlayerCam->Camera_Drop(0.25f, { 0.f, -0.3f, 0.f }, { 0.f, -0.125f, 0.f });
				PlayerCam->Camera_Shake(0.25f, 0.5f);
			}
		}
	}
}
VOID SlamState::FSM_StateExit()
{
	SlamStateTempo = 0;
}
