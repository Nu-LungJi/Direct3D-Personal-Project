#include "../Default/PCH.h"
#include "BehaviorTree_Void.h"

BehaviorTree_Void::BehaviorTree_Void(shared_ptr<BTBlackBoard> _BlackBoard) : BlackBoard(_BlackBoard) {}
BehaviorTree_Void::~BehaviorTree_Void() {}

HRESULT BehaviorTree_Void::BehaviorTree_Initialize() {

	return S_OK;
}
VOID	BehaviorTree_Void::BehaviorTree_Update(CONST FLOAT& _DT) {
	RootNode->Update(_DT);
}
shared_ptr<BehaviorTree_Void>  BehaviorTree_Void::Create(shared_ptr<BTBlackBoard>	_BlackBoard) {
	auto Instance = shared_ptr<BehaviorTree_Void>(new BehaviorTree_Void(_BlackBoard));
	if (FAILED(Instance->BehaviorTree_Initialize())) {
		MSG_BOX("Cannot Create BehaviorTree Void.");
		return nullptr;
	}
	return Instance;
}

NODESTATE Attack_AnimationNode::Update(const _float _DT) {
	shared_ptr<Animator> Component_Animator = BlackBoard->Get_Value<shared_ptr<Animator>>("OwnerAnimator");
	shared_ptr<Transform> Component_Transform = BlackBoard->Get_Value<shared_ptr<Transform>>("OwnerTransform");
	if (!BlackBoard->Has_Key("Random Attack Enable")) { BlackBoard->Set_Value<_float>("Random Attack Enable", true); }
	if (BlackBoard->Get_Value<_float>("Random Attack Enable")) {
		BlackBoard->Set_Value<uint32_t>("AttackAnimIndex", rand() % AnimationIndexList.size());
		BlackBoard->Set_Value<_float>("Random Attack Enable", false);
	}

	auto AnimationState = Component_Animator->Get_AnimationState(BlackBoard->Get_Value<uint32_t>("AttackAnimIndex"));
	auto CurrentAnimation = Component_Animator->Get_CurrentAnimation();

	if (AnimationState == ANIMATION_STATE::ONREADY) {
		BlackBoard->Set_Value<_bool>("Attack State", true);
		Component_Animator->Play_Animation(BlackBoard->Get_Value<uint32_t>("AttackAnimIndex"), false, AnimationSpeed);
		Component_Animator->Set_AnimationBlending(true);
	}
	else if (AnimationState == ANIMATION_STATE::FINISHED) {
		BlackBoard->Set_Value<_bool>("Attack State", false);
		BlackBoard->Set_Value<_float>("Random Attack Enable", true);
		BlackBoard->Set_Value<_bool>("Chased Target", false);
		return NODESTATE::SUCCESS;
	}

	return NODESTATE::RUNNING;
}
NODESTATE VD_Play_DeathAnimation::Update(const _float _DT) {
	if (Component_Animator->Get_CurrentAnimationIndex() != Death_AnimationIndex) {
		Component_Animator->Play_Animation(Death_AnimationIndex);
	}
	if (Component_Animator->Get_CurrentAnimation()->Get_AnimationProgress() >= 0.8f && EffectTrigger) {
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
NODESTATE VD_RevertNode::Update(const _float _DT) {
	if (BlackBoard->Get_Value<_bool>("StandByStance")) { return NODESTATE::SUCCESS; }

	shared_ptr<Animator>	Component_Animator = BlackBoard->Get_Value<shared_ptr<Animator>>("OwnerAnimator");
	shared_ptr<Animation>	StandBy_Animation = Component_Animator->Get_AnimationByIndex(static_cast<uint32_t>(MONSTER_VOID::STANDBY));

	if (Component_Animator->Get_CurrentAnimationIndex() != static_cast<uint32_t>(MONSTER_VOID::WAKEUP) &&
		Component_Animator->Get_CurrentAnimationIndex() != static_cast<uint32_t>(MONSTER_VOID::STANDBY)) {
		Component_Animator->Play_Animation(static_cast<uint32_t>(MONSTER_VOID::STANDBY), true, 1.f);
		Component_Animator->Set_AnimationBlending(true);

		return NODESTATE::SUCCESS;
	}

	if (Component_Animator->Get_CurrentAnimationIndex() == static_cast<uint32_t>(MONSTER_VOID::STANDBY) && 
		Component_Animator->Get_CurrentAnimation()->Get_AnimationProgress() >= 0.5f && 
		Component_Animator->Get_AnimationLoop() != ANIMATION_LOOP::PAUSED) {
		Component_Animator->Stop_Animation();
		return NODESTATE::FAILURE;
	}
	
	return NODESTATE::RUNNING;
}
NODESTATE VD_ResetToCombatNode::Update(const _float _DT) {
	shared_ptr<Animator> Component_Animator = BlackBoard->Get_Value<shared_ptr<Animator>>("OwnerAnimator");
	if (BlackBoard->Get_Value<_bool>("Return Completed")) {
		BlackBoard->Set_Value<_bool>("CombatStance", true);
		BlackBoard->Set_Value<_bool>("StandByStance", false);
		BlackBoard->Set_Value<_bool>("Return Completed", false);
		BlackBoard->Set_Value<_bool>("Chased Target", false);
		if (Component_Animator->Get_CurrentAnimationIndex() == static_cast<uint32_t>(MONSTER_VOID::STANDBY)) {
			Component_Animator->Resume_PlayBack(ANIMATION_LOOP::PLAY_ONCE);
			Component_Animator->Get_CurrentAnimation()->Set_AnimationSpeed(AnimationSpeed);
		}
	}

	if (Component_Animator->Get_AnimationByIndex(static_cast<uint32_t>(MONSTER_VOID::STANDBY))->Get_AnimationState() == ANIMATION_STATE::ONRUNNING) {
		return NODESTATE::RUNNING;
	}

	return NODESTATE::SUCCESS;
}

NODESTATE Attack_TranslateNode::Update(const _float _DT) {
	if (BlackBoard->Get_Value<_bool>("Attack State"))		{ return NODESTATE::SUCCESS; }
	if (BlackBoard->Get_Value<_bool>("Return Completed"))	{ return NODESTATE::SUCCESS; }

	shared_ptr<Animator>	Component_Animator  = BlackBoard->Get_Value<shared_ptr<Animator>> ("OwnerAnimator");
	shared_ptr<Transform>	Component_Transform = BlackBoard->Get_Value<shared_ptr<Transform>>("OwnerTransform");
	shared_ptr<Transform>	PlayerTransform		= BlackBoard->Get_Value<shared_ptr<Transform>>("PlayerTransform");

	XMVECTOR CurrentPos	= Component_Transform->Get_WorldPosition();
	XMVECTOR TargetPos  = PlayerTransform->Get_WorldPosition();

	XMVECTOR Distance = XMVectorSetY(TargetPos - CurrentPos, 0.f);
	_float	 DistanceSQRT = XMVector3LengthSq(Distance).m128_f32[0];

	_bool FirstAnimation = Component_Animator->Get_CurrentAnimationIndex() != AnimationIndex;
	if (FirstAnimation || Component_Animator->Get_AnimationState(AnimationIndex) == ANIMATION_STATE::FINISHED) {
		if (FirstAnimation) Component_Animator->Set_AnimationBlending(true);
		Component_Animator->Play_Animation(AnimationIndex, false, TranslateSpeed);
	}

	return NODESTATE::SUCCESS;
}

NODESTATE Attack_RotateNode::Update(const _float _DT) {
	if (BlackBoard->Get_Value<_bool>("Attack State")) { return NODESTATE::SUCCESS; }
	if (BlackBoard->Get_Value<_bool>("Rotate_CenterPoint")) { return NODESTATE::SUCCESS; }

	shared_ptr<Transform> OwnerTransform	= BlackBoard->Get_Value<shared_ptr<Transform>>("OwnerTransform");
	shared_ptr<Animator>  OwnerAnimator		= BlackBoard->Get_Value<shared_ptr<Animator>>("OwnerAnimator");
	shared_ptr<Transform> PlayerTransform	= BlackBoard->Get_Value<shared_ptr<Transform>>("PlayerTransform");

	XMVECTOR TargetDirection = XMVector3Normalize(XMVectorSetY(OwnerTransform->Get_WorldPosition() - PlayerTransform->Get_WorldPosition(), 0.f));

	XMVECTOR UpVec = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMVECTOR RightVec = XMVector3Normalize(XMVector3Cross(UpVec, TargetDirection));
	UpVec = XMVector3Cross(TargetDirection, RightVec);

	XMMATRIX TargetMat = XMMatrixIdentity();
	TargetMat.r[0] = RightVec; TargetMat.r[1] = UpVec; TargetMat.r[2] = TargetDirection;
	XMVECTOR TargetQuat = XMQuaternionRotationMatrix(TargetMat);

	XMVECTOR LookDirection = XMVector3Normalize(OwnerTransform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK));
	_float	 SightAnglePercentage = XMVectorGetX(XMVector3Dot(LookDirection, TargetDirection));
	_float	 FinalRotationSpeed = RotationSpeed;
	if (SightAnglePercentage > 0.80f) {
		FinalRotationSpeed *= 5.f;
	}

	XMVECTOR NextFrameQuat = XMQuaternionNormalize(XMQuaternionSlerp(OwnerTransform->Get_WorldRotationQuat(), TargetQuat, _DT * FinalRotationSpeed));
	OwnerTransform->Set_WorldRotationQuat(NextFrameQuat);

	if (SightAnglePercentage > 0.995f) {
		return NODESTATE::SUCCESS;
	}

	return NODESTATE::RUNNING;
}

NODESTATE Attack_LerpRoatation::Update(const _float _DT) {
	return NODESTATE::FAILURE;
}
NODESTATE VD_Play_DissolveEffect::Update(const _float _DT) {
	PLAY_TYPE PTYPE = EffectOwner->Get_DissolveState();
	if (PTYPE == PLAY_TYPE::ONREADY) {
		EffectOwner->Enable_Dissolve(PLAY_TYPE::ONSTART, 0.5f);
	}
	else if (PTYPE == PLAY_TYPE::FINISHED) {
		EffectOwner->Set_DissolveState(PLAY_TYPE::ONREADY);
		return NODESTATE::SUCCESS;
	}
	return NODESTATE::RUNNING;
}