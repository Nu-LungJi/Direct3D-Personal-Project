#include "../Default/PCH.h"
#include "BehaviorTree_Knight.h"

BehaviorTree_Knight::BehaviorTree_Knight(shared_ptr<BTBlackBoard> _BlackBoard)	: BlackBoard(_BlackBoard){}
BehaviorTree_Knight::~BehaviorTree_Knight() {}

HRESULT BehaviorTree_Knight::BehaviorTree_Initialize() {

	return S_OK;
}
VOID	BehaviorTree_Knight::BehaviorTree_Update(CONST FLOAT& _DT) {
	RootNode->Update(_DT);
}
shared_ptr<BehaviorTree_Knight>  BehaviorTree_Knight::Create(shared_ptr<BTBlackBoard> _BlackBoard) {
	auto Instance = shared_ptr<BehaviorTree_Knight>(new BehaviorTree_Knight(_BlackBoard));
	if (FAILED(Instance->BehaviorTree_Initialize())) {
		MSG_BOX("Cannot Create BehaviorTree_Knight.");
		return nullptr;
	}
	return Instance;
}

/// Death State : <CheckHP> -> <DeathAnimation, Dissolve Shader, Disable Collider, etc...> -> <Destroy Object>
NODESTATE Check_DeathCondition::Update(const _float _DT) {
	if (Owner->Get_HP() > 0.f) 
		return NODESTATE::FAILURE;
	return NODESTATE::SUCCESS;
}
NODESTATE Play_DeathAnimation::Update(const _float _DT) {
	if (Component_Animator->Get_CurrentAnimationIndex() != Death_AnimationIndex) {
		Component_Animator->Play_Animation(Death_AnimationIndex);
	}
	if (EffectTrigger && Component_Animator->Get_CurrentAnimation()->Get_AnimationProgress() >= 0.9f) {
		XMFLOAT3 Pos, Rot;
		XMStoreFloat3(&Pos, Component_Transform->Get_WorldPosition());
		XMStoreFloat3(&Rot, Component_Transform->Get_WorldRotation());

		
		GameInstance::GetInstance().Get_EffectManager()->Play_Effect("Void_Disappear_Effect", Pos, Rot * -1, { 2.f, 2.f, 2.f }, 30.f);

		EffectTrigger = false;
	}
	if (Component_Animator->Get_CurrentAnimation()->Get_AnimationState() == ANIMATION_STATE::FINISHED) {
		return NODESTATE::SUCCESS;
	}

	return NODESTATE::RUNNING;
	// 애니메이션 실행, Dissolve 셰이더 활성화, 콜라이더 비활성화, 
}
NODESTATE Destroy_OwnerActor::Update(const _float _DT) {
	Owner->Set_DeadState(true);
	return NODESTATE::SUCCESS;
}

/// Combat State : <Patrol> -> <CheckDistance To Target> -> <BattleStance : Animation> -> <MoveToTarget>
NODESTATE Check_TargetDistance::Update(const _float _DT) {
	if (BlackBoard->Get_Value<_bool>("Attack State")) { return NODESTATE::SUCCESS; }
	XMVECTOR DistanceVecSQRT = XMVector3LengthSq(BlackBoard->Get_Value<XMVECTOR>("Center Position") - TargetTransform->Get_WorldPosition());
	_float DistanceSQRT = XMVectorGetX(DistanceVecSQRT) + XMVectorGetZ(DistanceVecSQRT);

	if (DistanceSQRT < pow(Distance, 5.f)) { 
		return NODESTATE::SUCCESS;
	}
	
	return NODESTATE::FAILURE;
}

NODESTATE Rotate_CenterPoint::Update(const _float _DT) {
	if (BlackBoard->Get_Value<_bool>("Rotate_CenterPoint")) { return NODESTATE::SUCCESS; }
	
	shared_ptr<Transform> OwnerTransform = BlackBoard->Get_Value<shared_ptr<Transform>>("OwnerTransform");
	shared_ptr<Animator> OwnerAnimator	 = BlackBoard->Get_Value<shared_ptr<Animator>>("OwnerAnimator");

	XMVECTOR Direction = XMVector3Normalize(XMVectorSetY(OwnerTransform->Get_WorldPosition() - BlackBoard->Get_Value<XMVECTOR>("Center Position"), 0.f));

	if (XMVector3LengthSq(Direction).m128_f32[0] < 0.0001f) {
		return NODESTATE::SUCCESS;
	}

	XMVECTOR UpVec = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMVECTOR RightVec = XMVector3Normalize(XMVector3Cross(UpVec, Direction));
	UpVec = XMVector3Normalize(XMVector3Cross(Direction, RightVec));

	XMMATRIX TargetMat = XMMatrixIdentity();
	TargetMat.r[0] = RightVec; TargetMat.r[1] = UpVec; TargetMat.r[2] = Direction;
	XMVECTOR TargetQuat = XMQuaternionRotationMatrix(TargetMat);

	XMVECTOR NextFrameQuat = XMQuaternionNormalize(XMQuaternionSlerp(OwnerTransform->Get_WorldRotationQuat(), TargetQuat, _DT * RotationSpeed));
	OwnerTransform->Set_WorldRotationQuat(NextFrameQuat);

	XMVECTOR vMyLook = XMVector3Normalize(OwnerTransform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK));
	if (XMVectorGetX(XMVector3Dot(vMyLook, Direction)) > 0.99f) {
		OwnerAnimator->Set_MovementAmount(BlackBoard->Get_Value<_float>("MovementAmount"));
		return NODESTATE::SUCCESS;
	}
	return NODESTATE::SUCCESS;
}

NODESTATE Play_StagingAnimation::Update(const _float _DT) {
	if (BlackBoard->Get_Value<_bool>("Awake Completed")) { return NODESTATE::SUCCESS; }

	shared_ptr<Animator>	Component_Animator = BlackBoard->Get_Value<shared_ptr<Animator>>("OwnerAnimator");
	shared_ptr<Animation>	WakeUp_Animation = Component_Animator->Get_AnimationByIndex(WakeUpAnimation);

	if (WakeUp_Animation->Get_AnimationState() == ANIMATION_STATE::FINISHED) {
		BlackBoard->Set_Value<_bool>("Awake Completed", true);
		return NODESTATE::SUCCESS;
	}
	else if (Component_Animator->Get_CurrentAnimationIndex() == WakeUpAnimation && 
		Component_Animator->Get_CurrentAnimation()->Get_AnimationState() == ANIMATION_STATE::ONREADY && 
		!BlackBoard->Get_Value<_bool>("Awake Completed")) {
		Component_Animator->Play_Animation(WakeUpAnimation, false, AnimationSpeed);
	}

	return NODESTATE::RUNNING;
} 

NODESTATE Play_CombatStateAnimation::Update(const _float _DT) {
	if (BlackBoard->Get_Value<_bool>("CombatStance")) { return NODESTATE::SUCCESS; }

	shared_ptr<Animator>	Component_Animator = BlackBoard->Get_Value<shared_ptr<Animator>>("OwnerAnimator");
	shared_ptr<Animation>	WakeUp_Animation = Component_Animator->Get_AnimationByIndex(BattleStateAnimation);

	if (WakeUp_Animation->Get_AnimationState() == ANIMATION_STATE::FINISHED) {
		BlackBoard->Set_Value<_bool>("CombatStance", true);
		return NODESTATE::SUCCESS;
	}
	else if (WakeUp_Animation->Get_AnimationState() == ANIMATION_STATE::ONREADY && !BlackBoard->Get_Value<_bool>("CombatStance Completed")) {
		Component_Animator->Play_Animation(BattleStateAnimation, false, 1.5f);
		Component_Animator->Set_AnimationBlending(true);
	}

	return NODESTATE::RUNNING;
}
NODESTATE Move_CenterPoint::Update(const _float _DT) {
	if (BlackBoard->Get_Value<_bool>("Return Completed")) { return NODESTATE::SUCCESS; }

	shared_ptr<Animator>	Component_Animator	= BlackBoard->Get_Value<shared_ptr<Animator>>("OwnerAnimator");
	shared_ptr<Transform>	Component_Transform = BlackBoard->Get_Value<shared_ptr<Transform>>("OwnerTransform");

	XMVECTOR CurrentPos		= Component_Transform->Get_WorldPosition();
	XMVECTOR CenterPos		= BlackBoard->Get_Value<XMVECTOR>("Center Position");

	XMVECTOR Distance		= XMVectorSetY(CenterPos - CurrentPos, 0.f);
	_float	 DistanceSQRT	= XMVector3LengthSq(Distance).m128_f32[0];

	if (DistanceSQRT < 8.f) {
		BlackBoard->Set_Value<_bool>("Return Completed", true);
		return NODESTATE::SUCCESS;
	}

	_bool FirstAnimation = Component_Animator->Get_CurrentAnimationIndex() != MoveAnimationIndex;
	if (FirstAnimation || Component_Animator->Get_AnimationState(MoveAnimationIndex) == ANIMATION_STATE::FINISHED) {
		if (FirstAnimation) Component_Animator->Set_AnimationBlending(true);
		Component_Animator->Play_Animation(MoveAnimationIndex, false, 2.5f);
	}

	return NODESTATE::RUNNING;
}

NODESTATE Play_StandByAnimation::Update(const _float _DT) {
	if (BlackBoard->Get_Value<_bool>("StandByStance"))			{ return NODESTATE::SUCCESS; }
	shared_ptr<Animator>	Component_Animator = BlackBoard->Get_Value<shared_ptr<Animator>>("OwnerAnimator");

	shared_ptr<Animation> StandBy_Start_Animation = Component_Animator->Get_AnimationByIndex(static_cast<uint32_t>(MONSTER_KNIGHT::STANDBY_START));
	shared_ptr<Animation> StandBy_Loop_Animation  = Component_Animator->Get_AnimationByIndex(static_cast<uint32_t>(MONSTER_KNIGHT::STANDBY_LOOP));

	if (Component_Animator->Get_CurrentAnimationIndex() != static_cast<uint32_t>(MONSTER_KNIGHT::STANDBY_START)) {
		Component_Animator->Play_Animation(static_cast<uint32_t>(MONSTER_KNIGHT::STANDBY_START), true, 1.5f);
		Component_Animator->Set_AnimationBlending(true);

		return NODESTATE::SUCCESS;
	}
	if (StandBy_Start_Animation->Get_AnimationState() == ANIMATION_STATE::FINISHED) {
		BlackBoard->Set_Value<_bool>("CombatStance", false);
		BlackBoard->Set_Value<_bool>("StandByStance", true);
		Component_Animator->Play_Animation(static_cast<uint32_t>(MONSTER_KNIGHT::STANDBY_LOOP));
	}

	return NODESTATE::RUNNING;
}

NODESTATE Convert_CombatState::Update(const _float _DT) {
	shared_ptr<Animator> Component_Animator = BlackBoard->Get_Value<shared_ptr<Animator>>("OwnerAnimator");
	if (BlackBoard->Get_Value<_bool>("Return Completed")) {
		BlackBoard->Set_Value<_bool>("CombatStance", true);
		BlackBoard->Set_Value<_bool>("StandByStance", false);
		BlackBoard->Set_Value<_bool>("Return Completed", false);
		BlackBoard->Set_Value<_bool>("Chased Target", false);
	}

	return NODESTATE::SUCCESS;
}

NODESTATE Play_AttackedAnimation::Update(const _float _DT) {
	if (*HitCall == true) {
		*HitCall = false;

		//Component_Animator->Get_CurrentAnimation()->Set_AnimationState(ANIMATION_STATE::FINISHED);
		Component_Animator->Set_MovementAmount(0.f);
		
		Component_Animator->Play_Animation(AnimationIndex, false, AnimationSpeed);
		Component_Animator->Set_AnimationBlending(true);

		HitStopTrigger = true;

		return NODESTATE::RUNNING;
	}
	else if (Component_Animator->Get_CurrentAnimationIndex() != AnimationIndex){
		return NODESTATE::SUCCESS;
	}

	if (HitStopTrigger && Component_Animator->Get_CurrentAnimation()->Get_AnimationProgress() > 0.2f) {
		GameInstance::GetInstance().Get_TimeManager()->Execute_HitStop(0.1f, 0.1f);
		HitStopTrigger = false;
	}
	if (Component_Animator->Get_CurrentAnimation()->Get_AnimationState() == ANIMATION_STATE::FINISHED) {
		BlackBoard->Set_Value("Chased Target", false);
		BlackBoard->Set_Value<_float>("Random Attack Enable", true);
		Component_Animator->Set_MovementAmount(BlackBoard->Get_Value<_float>("MovementAmount"));

		return NODESTATE::SUCCESS;
	}
	return NODESTATE::RUNNING;
}

NODESTATE Check_FirstDetect::Update(const _float _DT)  {
	if (BlackBoard->Has_Key("First Detect State") == false) { BlackBoard->Set_Value<_bool>("First Detect State", true); }
	_bool IsFirstDetect = BlackBoard->Get_Value<_bool>("First Detect State");
	if		(IsFirstDetect == true ) return NODESTATE::SUCCESS;
	else							 return NODESTATE::FAILURE;
}

NODESTATE Disable_FirstDetect::Update(const _float _DT) {
	BlackBoard->Set_Value<_bool>("First Detect State", false);
	return NODESTATE::SUCCESS;
}

NODESTATE Play_ConvertAnimation::Update(const _float _DT) {
	if (Component_Animator->Get_CurrentAnimationIndex() != AnimationIndex && BlackBoard->Get_Value<_bool>("Return Completed"))
		Component_Animator->Play_Animation(AnimationIndex, false, AnimationSpeed);
	if (Component_Animator->Get_AnimationByIndex(AnimationIndex)->Get_AnimationState() == ANIMATION_STATE::ONRUNNING) {
		return NODESTATE::RUNNING;
	}
	return NODESTATE::SUCCESS;
}

NODESTATE Play_DissolveEffect::Update(const _float _DT){
	PLAY_TYPE PTYPE = EffectOwner->Get_DissolveState();
	if		(PTYPE == PLAY_TYPE::ONREADY) {
		EffectOwner->Enable_Dissolve(PLAY_TYPE::ONSTART, 0.5f);
	}
	else if (PTYPE == PLAY_TYPE::FINISHED){
		EffectOwner->Set_DissolveState(PLAY_TYPE::ONREADY);
		return NODESTATE::SUCCESS;
	}
	
	return NODESTATE::RUNNING;
}
