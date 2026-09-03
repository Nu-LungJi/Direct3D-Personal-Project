#include "../Default/PCH.h"
#include "BehaviorTree_FinalBoss.h"
#include "Monster_FinalBoss.h"
#include "BossEffect.h"

BehaviorTree_FinalBoss::BehaviorTree_FinalBoss(shared_ptr<BTBlackBoard> _BlackBoard) : BlackBoard(_BlackBoard) {}
BehaviorTree_FinalBoss::~BehaviorTree_FinalBoss() {}

HRESULT BehaviorTree_FinalBoss::BehaviorTree_Initialize() {

	return S_OK;
}
VOID	BehaviorTree_FinalBoss::BehaviorTree_Update(CONST FLOAT& _DT) {
	RootNode->Update(_DT);
}
shared_ptr<BehaviorTree_FinalBoss>  BehaviorTree_FinalBoss::Create(shared_ptr<BTBlackBoard> _BlackBoard) {
	auto Instance = shared_ptr<BehaviorTree_FinalBoss>(new BehaviorTree_FinalBoss(_BlackBoard));
	if (FAILED(Instance->BehaviorTree_Initialize())) {
		MSG_BOX("Cannot Create BehaviorTree FinalBoss.");
		return nullptr;
	}
	return Instance;
}

NODESTATE Check_GroggyCondition::Update(const _float _DT) {
	if (*GroggyCall == true) return NODESTATE::SUCCESS;
	return NODESTATE::FAILURE;
}

NODESTATE Play_GroggyAnimation::Update(const _float _DT) {
	if (Component_Animator->Get_CurrentAnimationIndex() != AnimationIndex) {
		Component_Animator->Play_Animation(AnimationIndex);
		Component_Animator->Set_AnimationBlending(true);
		return NODESTATE::SUCCESS;
	}
	return NODESTATE::RUNNING;
}

NODESTATE Check_AttackCondition::Update(const _float _DT) {
	if (Component_Animator->Get_CurrentAnimation()->Get_AnimationState() == ANIMATION_STATE::FINISHED) {
		AttackState = ATTACKSTATE::ONREADY;
		BlackBoard->Set_Value<_bool>("Target Engaged", true);
	}

	if (AttackState == ATTACKSTATE::ONRUNNING) return NODESTATE::FAILURE;

	BlackBoard->Set_Value<ATTACKSTATE>("Attack State", ATTACKSTATE::ONRUNNING);

	return NODESTATE::SUCCESS;
}

NODESTATE Play_AttackAnimation::Update(const _float _DT) {

	_float	 Progress		= Component_Animator->Get_CurrentAnimation()->Get_AnimationProgress();
	uint32_t CurrAnimIndex	= Component_Animator->Get_CurrentAnimationIndex();
	uint32_t* JumpState		= Owner->Get_ObjectOnAir();

	if		(*JumpState == static_cast<uint32_t>(JUMPSTATE::READY) || *JumpState == static_cast<uint32_t>(JUMPSTATE::LAND)) {
		if		(CurrAnimIndex == static_cast<uint32_t>(MONSTER_SCAR::ATTACK01) && Progress >= 0.06f) {
			*JumpState = static_cast<uint32_t>(JUMPSTATE::JUMPING);
		}
		else if (CurrAnimIndex == static_cast<uint32_t>(MONSTER_SCAR::ATTACK03) && Progress >= 0.04f) {
			*JumpState = static_cast<uint32_t>(JUMPSTATE::JUMPING);
		}
		else if (CurrAnimIndex == static_cast<uint32_t>(MONSTER_SCAR::ATTACK04) && Progress >= 0.12f) {
			*JumpState = static_cast<uint32_t>(JUMPSTATE::JUMPING);
		}
		else if (CurrAnimIndex == static_cast<uint32_t>(MONSTER_SCAR::ATTACK11) && Progress >= 0.065f){
			*JumpState = static_cast<uint32_t>(JUMPSTATE::JUMPING);
		}
	}
	else if (*JumpState == static_cast<uint32_t>(JUMPSTATE::JUMPING)) {
		if		(CurrAnimIndex == static_cast<uint32_t>(MONSTER_SCAR::ATTACK01) && Progress >= 0.20f) {
			*JumpState = static_cast<uint32_t>(JUMPSTATE::FALLING);
		}
		else if (CurrAnimIndex == static_cast<uint32_t>(MONSTER_SCAR::ATTACK03) && Progress >= 0.15f) {
			*JumpState = static_cast<uint32_t>(JUMPSTATE::FALLING);
		}
		else if (CurrAnimIndex == static_cast<uint32_t>(MONSTER_SCAR::ATTACK04) && Progress >= 0.40f) {
			*JumpState = static_cast<uint32_t>(JUMPSTATE::FALLING);
		}
		else if (CurrAnimIndex == static_cast<uint32_t>(MONSTER_SCAR::ATTACK11) && Progress >= 0.175f){
			*JumpState = static_cast<uint32_t>(JUMPSTATE::FALLING);
		}
	}
	else if (*JumpState == static_cast<uint32_t>(JUMPSTATE::FALLING)) {
		if		(CurrAnimIndex == static_cast<uint32_t>(MONSTER_SCAR::ATTACK01) && Progress >= 0.35f) {
			*JumpState = static_cast<uint32_t>(JUMPSTATE::LAND);
		}
		else if (CurrAnimIndex == static_cast<uint32_t>(MONSTER_SCAR::ATTACK03) && Progress >= 0.235f){
			*JumpState = static_cast<uint32_t>(JUMPSTATE::LAND);
		}
		else if (CurrAnimIndex == static_cast<uint32_t>(MONSTER_SCAR::ATTACK04) && Progress >= 0.43f) {
			*JumpState = static_cast<uint32_t>(JUMPSTATE::LAND);
		}
		else if (CurrAnimIndex == static_cast<uint32_t>(MONSTER_SCAR::ATTACK11) && Progress >= 0.25f) {
			*JumpState = static_cast<uint32_t>(JUMPSTATE::LAND);
		}
	}

	if (Component_Animator->Get_CurrentAnimation()->Get_AnimationState() == ANIMATION_STATE::ONRUNNING)
		return NODESTATE::RUNNING;

	static bool First = true;

	uint32_t AnimationIndex = 0;
	_float Distance = XMVectorGetX(XMVector3Length(BlackBoard->Get_Value<shared_ptr<Transform>>("PlayerTransform")->Get_WorldPosition() 
		- BlackBoard->Get_Value<shared_ptr<Transform>>("OwnerTransform")->Get_WorldPosition()));

	uint32_t AnimationIndexList[] = {
		static_cast<uint32_t>(MONSTER_SCAR::ATTACK01),
		static_cast<uint32_t>(MONSTER_SCAR::ATTACK03),
		static_cast<uint32_t>(MONSTER_SCAR::ATTACK05),
		static_cast<uint32_t>(MONSTER_SCAR::ATTACK07),
		static_cast<uint32_t>(MONSTER_SCAR::ATTACK08),
		static_cast<uint32_t>(MONSTER_SCAR::ATTACK09),
		static_cast<uint32_t>(MONSTER_SCAR::ATTACK10),
		static_cast<uint32_t>(MONSTER_SCAR::ATTACK11),
		static_cast<uint32_t>(MONSTER_SCAR::ATTACK13)
	};
	static uint32_t Index = 0;
	Component_Animator->Play_Animation(AnimationIndexList[Index++ % 9]);

	return NODESTATE::SUCCESS;
}

NODESTATE LookAt_TargetPosition::Update(const _float _DT) {
	shared_ptr<Transform>	OwnerTransform  = BlackBoard->Get_Value<shared_ptr<Transform>>("OwnerTransform"	);
	shared_ptr<Animator>	OwnerAnimator   = BlackBoard->Get_Value<shared_ptr<Animator>> ("OwnerAnimator"	);
	shared_ptr<Transform>	TargetTransform = BlackBoard->Get_Value<shared_ptr<Transform>>("PlayerTransform");

	if (BlackBoard->Get_Value<_bool>("Target Engaged") == true) {
		Direction = XMVector3Normalize(XMVectorSetY(OwnerTransform->Get_WorldPosition() - TargetTransform->Get_WorldPosition(), 0.f));
		BlackBoard->Set_Value<_bool>("Target Engaged", false);
	}

	XMVECTOR UpVec = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMVECTOR RightVec = XMVector3Normalize(XMVector3Cross(UpVec, Direction));
	UpVec = XMVector3Cross(Direction, RightVec);

	XMMATRIX TargetMat = XMMatrixIdentity();
	TargetMat.r[0] = RightVec; TargetMat.r[1] = UpVec; TargetMat.r[2] = Direction;
	XMVECTOR TargetQuat = XMQuaternionRotationMatrix(TargetMat);

	XMVECTOR NextFrameQuat = XMQuaternionNormalize(XMQuaternionSlerp(OwnerTransform->Get_WorldRotationQuat(), TargetQuat, _DT * RotationSpeed));
	OwnerTransform->Set_WorldRotationQuat(NextFrameQuat);

	XMVECTOR vMyLook = XMVector3Normalize(OwnerTransform->Get_WorldTransform(VECTOR_TYPE::VECTOR_LOOK));
	if (XMVectorGetX(XMVector3Dot(vMyLook, Direction)) > 0.995f) {
		OwnerAnimator->Set_MovementAmount(BlackBoard->Get_Value<_float>("MovementAmount"), 1.f, BlackBoard->Get_Value<_float>("MovementAmount"));
		return NODESTATE::FAILURE;
	}
	return NODESTATE::FAILURE;
}

NODESTATE Play_AttackEffect::Update(const _float _DT) {
	uint32_t		AnimIndex	= Component_Animator->Get_CurrentAnimationIndex();
	_float			Progress	= Component_Animator->Get_CurrentAnimation()->Get_AnimationProgress();
	ANIMATION_STATE AnimState	= Component_Animator->Get_CurrentAnimation()->Get_AnimationState();
	shared_ptr<PlayerCamera> PlayerCam = static_pointer_cast<PlayerCamera>(GameInstance::GetInstance().Get_MainCamera());
	
	if (AnimIndex == static_cast<uint32_t>(MONSTER_SCAR::ATTACK01)) {
		if (RenderFlag[1] && Progress >= 0.200f) {
			shared_ptr<BossEffect> Charge_Outer, Charge_Inner;
			if (RenderFlag[0]) {
				{
					Charge_Outer = EffectOwner->Get_EffectProto("EFF_CirclePlane_Charge_v2");
					Charge_Outer->Set_EffectOwner(EffectOwner);
					Charge_Outer->Set_EffectOrbitRotation(0.f, 0.f);
					Charge_Outer->Set_EffectRotation({ 0.f, 0.f, 180.f });
					Charge_Outer->Set_EffectAlpha(0.65f);

					Charge_Outer->Set_DissolveScrollSpeed({ 0.5f, 1.5f });
					Charge_Outer->Set_EffectScrollSpeed({ 0.5f, 1.5f });

					Charge_Outer->Set_DissolveTextureOffset({ -0.2f, 0.f });
					Charge_Outer->Set_EffectTextureOffset({ -0.2f, 0.f });

					Charge_Outer->Set_DissolveStrength(1.5f);
					Charge_Outer->Set_EffectScale({ 20.f, 20.f, 20.f });
					Charge_Outer->Set_EffectLifeTime(1.5f);

					Charge_Outer->Set_DistanceFromPlayer(0.f);
					Charge_Outer->Set_EffectOffset({ 0.f, 13.f, -5.f });
					Charge_Outer->Set_DissolveEdgeColor({ 0.255f * 1.5f, 0.157f * 1.5f, 0.941f * 1.5f });
					EffectOwner->Play_Effect(Charge_Outer);

				}
				RenderFlag[0] = false;
			}
			if (nullptr != Charge_Outer)
				Charge_Outer->Update_EffectTransform();
			if (nullptr != Charge_Inner)
				Charge_Inner->Update_EffectTransform();

			if (Progress >= 0.25f) 
				RenderFlag[1] = false;
		}
		if (RenderFlag[2] && Progress >= 0.325f) {
			auto SwordTrail = EffectOwner->Get_EffectProto("EFF_HalfTrail");
			SwordTrail->Set_EffectOwner(EffectOwner);
			SwordTrail->Set_EffectOrbitRotation(90.f, 85.f);
			SwordTrail->Set_EffectRotation({ 0.f, 45.f, 0.f });

			SwordTrail->Set_DissolveScrollSpeed({ 1.5f, 0.f });
			SwordTrail->Set_EffectScrollSpeed({ 1.5f, 0.f });

			SwordTrail->Set_DissolveTextureOffset({ -0.2f, 0.f });
			SwordTrail->Set_EffectTextureOffset({ -0.2f, 0.f });

			SwordTrail->Set_DissolveStrength(0.8f);
			SwordTrail->Set_EffectScale({ 3.5f, 3.5f, 3.5f });
			SwordTrail->Set_EffectLifeTime(0.5f);

			SwordTrail->Set_DistanceFromPlayer(3.f);
			SwordTrail->Set_EffectOffset({ 0.f, 5.f, -3.f });
			SwordTrail->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });
			EffectOwner->Play_Effect(SwordTrail);

			RenderFlag[2] = false;
		}
		if (RenderFlag[3] && Progress >= 0.330f) {
			PlayerCam->Camera_Drop(0.25f, { 0.f, -0.5f, 0.f }, { 0.f, -0.125f, 0.f });
			PlayerCam->Camera_Shake(0.25f, 0.6f);
			RenderFlag[3] = false;
		}
		if (RenderFlag[4] && Progress >= 0.550f) {
			PlayerCam->Camera_Shake(2.f, 0.4f);
			RenderFlag[4] = false;
		}
	}
	if (AnimIndex == static_cast<uint32_t>(MONSTER_SCAR::ATTACK03)) {
		if (RenderFlag[30] && Progress >= 0.500f) {
			auto SwordTrail = EffectOwner->Get_EffectProto("EFF_CircleTrail");
			SwordTrail->Set_EffectOwner(EffectOwner);
			SwordTrail->Set_EffectOrbitRotation(0.f, 0.f);
			SwordTrail->Set_EffectRotation({ 0.f, -5.f, -15.f });

			SwordTrail->Set_DissolveScrollSpeed({ -4.5f, 1.0f });
			SwordTrail->Set_EffectScrollSpeed({ -4.5f, 1.0f });

			SwordTrail->Set_DissolveTextureOffset({ 1.f, 0.f });
			SwordTrail->Set_EffectTextureOffset({ 1.f, 0.f });

			SwordTrail->Set_DissolveStrength(1.5f);
			SwordTrail->Set_EffectScale({ 15.f, 15.f, 15.f });
			SwordTrail->Set_EffectLifeTime(0.75f);

			SwordTrail->Set_DistanceFromPlayer(0.f);
			SwordTrail->Set_EffectOffset({ 0.f, 5.f, -3.f });
			SwordTrail->Set_DissolveEdgeColor({ 0.255f * 1.5f, 0.157f * 1.5f, 0.941f * 1.5f });
			EffectOwner->Play_Effect(SwordTrail);

			RenderFlag[30] = false;
		}
		if (RenderFlag[31] && Progress >= 0.800f) {
			auto CardBomb = EffectOwner->Get_EffectProto("EFF_CardBomb");
			CardBomb->Set_EffectOwner(EffectOwner);
			CardBomb->Set_EffectOrbitRotation(60.f, 0.f);
			CardBomb->Set_EffectRotation({ 0.f, 0.f, 0.f });

			CardBomb->Set_DissolveScrollSpeed({ 2.5f, 0.0f });
			CardBomb->Set_EffectScrollSpeed({ 2.5f, 0.0f });

			CardBomb->Set_DissolveTextureOffset({ -1.f, 0.f });
			CardBomb->Set_EffectTextureOffset({ -1.f, 0.f });

			CardBomb->Set_DissolveStrength(1.f);
			CardBomb->Set_EffectScale({ 5.f, 5.f, 5.f });
			CardBomb->Set_EffectLifeTime(0.25f);

			CardBomb->Set_DistanceFromPlayer(-15.f);
			CardBomb->Set_EffectOffset({ 0.f, 5.f, -3.f });
			CardBomb->Set_DissolveEdgeColor({ 0.255f * 1.5f, 0.157f * 1.5f, 0.941f * 1.5f });

			CardBomb->Set_BillBoardOption(true);
			EffectOwner->Play_Effect(CardBomb);

			RenderFlag[31] = false;
		}
		if (RenderFlag[32] && Progress >= 0.815f) {
			auto CardBomb = EffectOwner->Get_EffectProto("EFF_CardBomb");
			CardBomb->Set_EffectOwner(EffectOwner);
			CardBomb->Set_EffectOrbitRotation(30.f, 0.f);
			CardBomb->Set_EffectRotation({ 0.f, 0.f, 0.f });

			CardBomb->Set_DissolveScrollSpeed({ 2.5f, 0.0f });
			CardBomb->Set_EffectScrollSpeed({ 2.5f, 0.0f });

			CardBomb->Set_DissolveTextureOffset({ -0.2f, 0.f });
			CardBomb->Set_EffectTextureOffset({ -0.2f, 0.f });

			CardBomb->Set_DissolveStrength(1.f);
			CardBomb->Set_EffectScale({ 5.f, 5.f, 5.f });
			CardBomb->Set_EffectLifeTime(0.25f);

			CardBomb->Set_DistanceFromPlayer(-15.f);
			CardBomb->Set_EffectOffset({ 0.f, 5.f, -3.f });
			CardBomb->Set_DissolveEdgeColor({ 0.255f * 1.5f, 0.157f * 1.5f, 0.941f * 1.5f });

			CardBomb->Set_BillBoardOption(1);
			EffectOwner->Play_Effect(CardBomb);

			RenderFlag[32] = false;
		}
		if (RenderFlag[33] && Progress >= 0.830f) {
			auto CardBomb = EffectOwner->Get_EffectProto("EFF_CardBomb");
			CardBomb->Set_EffectOwner(EffectOwner);
			CardBomb->Set_EffectOrbitRotation(0.f, 0.f);
			CardBomb->Set_EffectRotation({ 0.f, 0.f, 0.f });

			CardBomb->Set_DissolveScrollSpeed({ 2.5f, 0.0f });
			CardBomb->Set_EffectScrollSpeed({ 2.5f, 0.0f });

			CardBomb->Set_DissolveTextureOffset({ -0.2f, 0.f });
			CardBomb->Set_EffectTextureOffset({ -0.2f, 0.f });

			CardBomb->Set_DissolveStrength(1.f);
			CardBomb->Set_EffectScale({ 5.f, 5.f, 5.f });
			CardBomb->Set_EffectLifeTime(0.25f);

			CardBomb->Set_DistanceFromPlayer(-15.f);
			CardBomb->Set_EffectOffset({ 0.f, 5.f, -3.f });
			CardBomb->Set_DissolveEdgeColor({ 0.255f * 1.5f, 0.157f * 1.5f, 0.941f * 1.5f });

			CardBomb->Set_BillBoardOption(1);
			EffectOwner->Play_Effect(CardBomb);

			RenderFlag[33] = false;
		}
		if (RenderFlag[34] && Progress >= 0.845f) {
			auto CardBomb = EffectOwner->Get_EffectProto("EFF_CardBomb");
			CardBomb->Set_EffectOwner(EffectOwner);
			CardBomb->Set_EffectOrbitRotation(-30.f, 0.f);
			CardBomb->Set_EffectRotation({ 0.f, 0.f, 0.f });

			CardBomb->Set_DissolveScrollSpeed({ 2.5f, 0.0f });
			CardBomb->Set_EffectScrollSpeed({ 2.5f, 0.0f });

			CardBomb->Set_DissolveTextureOffset({ -0.2f, 0.f });
			CardBomb->Set_EffectTextureOffset({ -0.2f, 0.f });

			CardBomb->Set_DissolveStrength(1.f);
			CardBomb->Set_EffectScale({ 5.f, 5.f, 5.f });
			CardBomb->Set_EffectLifeTime(0.25f);

			CardBomb->Set_DistanceFromPlayer(-15.f);
			CardBomb->Set_EffectOffset({ 0.f, 5.f, -3.f });
			CardBomb->Set_DissolveEdgeColor({ 0.255f * 1.5f, 0.157f * 1.5f, 0.941f * 1.5f });

			CardBomb->Set_BillBoardOption(1);
			EffectOwner->Play_Effect(CardBomb);

			RenderFlag[34] = false;
		}
		if (RenderFlag[35] && Progress >= 0.860f) {
			auto CardBomb = EffectOwner->Get_EffectProto("EFF_CardBomb");
			CardBomb->Set_EffectOwner(EffectOwner);
			CardBomb->Set_EffectOrbitRotation(-60.f, 0.f);
			CardBomb->Set_EffectRotation({ 0.f, 0.f, 0.f });

			CardBomb->Set_DissolveScrollSpeed({ 2.5f, 0.0f });
			CardBomb->Set_EffectScrollSpeed({ 2.5f, 0.0f });

			CardBomb->Set_DissolveTextureOffset({ -0.2f, 0.f });
			CardBomb->Set_EffectTextureOffset({ -0.2f, 0.f });

			CardBomb->Set_DissolveStrength(1.f);
			CardBomb->Set_EffectScale({ 5.f, 5.f, 5.f });
			CardBomb->Set_EffectLifeTime(0.25f);

			CardBomb->Set_DistanceFromPlayer(-15.f);
			CardBomb->Set_EffectOffset({ 1.f, 0.f, -3.f });
			CardBomb->Set_DissolveEdgeColor({ 0.255f * 1.5f, 0.157f * 1.5f, 0.941f * 1.5f });

			CardBomb->Set_BillBoardOption(1);
			EffectOwner->Play_Effect(CardBomb);

			RenderFlag[35] = false;
		}
	}

	if (AnimIndex == static_cast<uint32_t>(MONSTER_SCAR::ATTACK05)) {
		if (RenderFlag[50] && Progress >= 0.290f) {
			{
				auto CatchSlamEffect = EffectOwner->Get_EffectProto("EFF_CatchSlam_DecalWave");
				CatchSlamEffect->Set_EffectOwner(EffectOwner);
				CatchSlamEffect->Set_EffectOrbitRotation(0.f, 0.f);
				CatchSlamEffect->Set_EffectRotation({ 0.f, 0.f, 0.f });

				CatchSlamEffect->Set_DissolveScrollSpeed({ 0.f, -2.5f });
				CatchSlamEffect->Set_EffectScrollSpeed({ 0.f, -3.5f });

				CatchSlamEffect->Set_DissolveTextureOffset({ 0.f, 0.f });
				CatchSlamEffect->Set_EffectTextureOffset({ 0.f, 0.f });

				CatchSlamEffect->Set_DissolveStrength(0.1f);
				CatchSlamEffect->Set_EffectScale({ 7.f, 7.f, 7.f });
				CatchSlamEffect->Set_EffectLifeTime(1.f);

				CatchSlamEffect->Set_EffectFadeOutValue(0.4f);

				CatchSlamEffect->Set_DistanceFromPlayer(0.f);
				CatchSlamEffect->Set_EffectOffset({ 0.25f, 0.2f, -6.f });
				CatchSlamEffect->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });
				EffectOwner->Play_Effect(CatchSlamEffect);
			}
			{
				auto SlamWave = EffectOwner->Get_EffectProto("EFF_CatchSlam_UpWave");

				SlamWave->Set_EffectOwner(EffectOwner);
				SlamWave->Set_EffectOrbitRotation(0.f, 0.f);
				SlamWave->Set_EffectRotation({ 0.f, 0.f, 0.f });

				SlamWave->Set_DissolveScrollSpeed({ 0.f, 2.5f });
				SlamWave->Set_EffectScrollSpeed({ 0.f, 3.5f });
				SlamWave->Set_EffectAlpha(0.75f);

				SlamWave->Set_DissolveTextureOffset({ 0.f, 0.f });
				SlamWave->Set_EffectTextureOffset({ 0.f, 0.f });

				SlamWave->Set_DissolveStrength(0.1f);
				SlamWave->Set_EffectScale({ 3.f, 5.f, 3.f });
				SlamWave->Set_EffectLifeTime(1.f);

				SlamWave->Set_EffectFadeOutValue(0.4f);
				SlamWave->Set_DistanceFromPlayer(0.f);
				SlamWave->Set_EffectOffset({ 0.25f, 0.2f, -6.f });
				SlamWave->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });
				EffectOwner->Play_Effect(SlamWave);
			}
			
			PlayerCam->Camera_Shake(0.1f, 0.25f);
			RenderFlag[50] = false;
		}
		if (RenderFlag[51] && Progress >= 0.382f) {
			{
				auto CatchSlamEffect = EffectOwner->Get_EffectProto("EFF_CatchSlam_DecalWave");
				CatchSlamEffect->Set_EffectOwner(EffectOwner);
				CatchSlamEffect->Set_EffectOrbitRotation(0.f, 0.f);
				CatchSlamEffect->Set_EffectRotation({ 0.f, 0.f, 0.f });

				CatchSlamEffect->Set_DissolveScrollSpeed({ 0.f, -2.5f });
				CatchSlamEffect->Set_EffectScrollSpeed({ 0.f, -3.5f });

				CatchSlamEffect->Set_DissolveTextureOffset({ 0.f, 0.f });
				CatchSlamEffect->Set_EffectTextureOffset({ 0.f, 0.f });

				CatchSlamEffect->Set_DissolveStrength(0.1f);
				CatchSlamEffect->Set_EffectScale({ 7.f, 7.f, 7.f });
				CatchSlamEffect->Set_EffectLifeTime(1.f);

				CatchSlamEffect->Set_EffectFadeOutValue(0.4f);

				CatchSlamEffect->Set_DistanceFromPlayer(0.f);
				CatchSlamEffect->Set_EffectOffset({ -5.5f, 0.2f, 1.5f });
				CatchSlamEffect->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });
				EffectOwner->Play_Effect(CatchSlamEffect);
			}
			{
				auto SlamWave = EffectOwner->Get_EffectProto("EFF_CatchSlam_UpWave");

				SlamWave->Set_EffectOwner(EffectOwner);
				SlamWave->Set_EffectOrbitRotation(0.f, 0.f);
				SlamWave->Set_EffectRotation({ 0.f, 0.f, 0.f });

				SlamWave->Set_DissolveScrollSpeed({ 0.f, 2.5f });
				SlamWave->Set_EffectScrollSpeed({ 0.f, 3.5f });
				SlamWave->Set_EffectAlpha(0.75f);

				SlamWave->Set_DissolveTextureOffset({ 0.f, 0.f });
				SlamWave->Set_EffectTextureOffset({ 0.f, 0.f });

				SlamWave->Set_DissolveStrength(0.1f);
				SlamWave->Set_EffectScale({ 3.f, 5.f, 3.f });
				SlamWave->Set_EffectLifeTime(1.f);

				SlamWave->Set_EffectFadeOutValue(0.4f);
				SlamWave->Set_DistanceFromPlayer(0.f);
				SlamWave->Set_EffectOffset({ -5.5f, 0.2f, 1.5f });
				SlamWave->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });
				EffectOwner->Play_Effect(SlamWave);
			}
			PlayerCam->Camera_Shake(0.1f, 0.25f);
			RenderFlag[51] = false;
		}
		if (RenderFlag[52] && Progress >= 0.465f) {
			{
				auto CatchSlamEffect = EffectOwner->Get_EffectProto("EFF_CatchSlam_DecalWave");
				CatchSlamEffect->Set_EffectOwner(EffectOwner);
				CatchSlamEffect->Set_EffectOrbitRotation(0.f, 0.f);
				CatchSlamEffect->Set_EffectRotation({ 0.f, 0.f, 0.f });

				CatchSlamEffect->Set_DissolveScrollSpeed({ 0.f, -2.5f });
				CatchSlamEffect->Set_EffectScrollSpeed({ 0.f, -3.5f });

				CatchSlamEffect->Set_DissolveTextureOffset({ 0.f, 0.f });
				CatchSlamEffect->Set_EffectTextureOffset({ 0.f, 0.f });

				CatchSlamEffect->Set_DissolveStrength(0.1f);
				CatchSlamEffect->Set_EffectScale({ 7.f, 7.f, 7.f });
				CatchSlamEffect->Set_EffectLifeTime(1.f);

				CatchSlamEffect->Set_EffectFadeOutValue(0.4f);
				CatchSlamEffect->Set_DistanceFromPlayer(0.f);
				CatchSlamEffect->Set_EffectOffset({ -4.5f, 0.2f, 0.5f });
				CatchSlamEffect->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });
				EffectOwner->Play_Effect(CatchSlamEffect);
			}
			{
				auto SlamWave = EffectOwner->Get_EffectProto("EFF_CatchSlam_UpWave");

				SlamWave->Set_EffectOwner(EffectOwner);
				SlamWave->Set_EffectOrbitRotation(0.f, 0.f);
				SlamWave->Set_EffectRotation({ 0.f, 0.f, 0.f });

				SlamWave->Set_DissolveScrollSpeed({ 0.f, 2.5f });
				SlamWave->Set_EffectScrollSpeed({ 0.f, 3.5f });
				SlamWave->Set_EffectAlpha(0.75f);

				SlamWave->Set_DissolveTextureOffset({ 0.f, 0.f });
				SlamWave->Set_EffectTextureOffset({ 0.f, 0.f });

				SlamWave->Set_DissolveStrength(0.1f);
				SlamWave->Set_EffectScale({ 3.f, 5.f, 3.f });
				SlamWave->Set_EffectLifeTime(1.f);

				SlamWave->Set_EffectFadeOutValue(0.4f);
				SlamWave->Set_DistanceFromPlayer(0.f);
				SlamWave->Set_EffectOffset({ -4.5f, 0.2f, 0.5f });
				SlamWave->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });
				EffectOwner->Play_Effect(SlamWave);
			}
			PlayerCam->Camera_Shake(0.1f, 0.25f);
			RenderFlag[52] = false;
		}
		if (RenderFlag[53] && Progress >= 0.670f) {
			{
				auto CatchSlamEffect = EffectOwner->Get_EffectProto("EFF_CatchSlam_DecalWave");
				CatchSlamEffect->Set_EffectOwner(EffectOwner);
				CatchSlamEffect->Set_EffectOrbitRotation(0.f, 0.f);
				CatchSlamEffect->Set_EffectRotation({ 0.f, 0.f, 0.f });

				CatchSlamEffect->Set_DissolveScrollSpeed({ 0.f, -2.5f });
				CatchSlamEffect->Set_EffectScrollSpeed({ 0.f, -3.5f });

				CatchSlamEffect->Set_DissolveTextureOffset({ 0.f, 0.f });
				CatchSlamEffect->Set_EffectTextureOffset({ 0.f, 0.f });

				CatchSlamEffect->Set_DissolveStrength(0.1f);
				CatchSlamEffect->Set_EffectScale({ 7.f, 7.f, 7.f });
				CatchSlamEffect->Set_EffectLifeTime(1.f);

				CatchSlamEffect->Set_EffectFadeOutValue(0.6f);

				CatchSlamEffect->Set_DistanceFromPlayer(0.f);
				CatchSlamEffect->Set_EffectOffset({ -5.5f, 0.2f, 0.f });
				CatchSlamEffect->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });
				EffectOwner->Play_Effect(CatchSlamEffect);
			}
			{
				auto SlamWave = EffectOwner->Get_EffectProto("EFF_CatchSlam_UpWave");

				SlamWave->Set_EffectOwner(EffectOwner);
				SlamWave->Set_EffectOrbitRotation(0.f, 0.f);
				SlamWave->Set_EffectRotation({ 0.f, 0.f, 0.f });

				SlamWave->Set_DissolveScrollSpeed({ 0.f, 2.5f });
				SlamWave->Set_EffectScrollSpeed({ 0.f, 3.5f });
				SlamWave->Set_EffectAlpha(0.75f);

				SlamWave->Set_DissolveTextureOffset({ 0.f, 0.f });
				SlamWave->Set_EffectTextureOffset({ 0.f, 0.f });

				SlamWave->Set_DissolveStrength(0.1f);
				SlamWave->Set_EffectScale({ 3.f, 5.f, 3.f });
				SlamWave->Set_EffectLifeTime(1.f);

				SlamWave->Set_EffectFadeOutValue(0.4f);
				SlamWave->Set_DistanceFromPlayer(0.f);
				SlamWave->Set_EffectOffset({ -5.5f, 0.2f, 0.f });
				SlamWave->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });
				EffectOwner->Play_Effect(SlamWave);
			}
			PlayerCam->Camera_Shake(0.1f, 0.25f);
			RenderFlag[53] = false;
		}
		if (RenderFlag[54] && Progress >= 0.760f) {
			{
				auto CatchSlamEffect = EffectOwner->Get_EffectProto("EFF_CatchSlam_DecalWave");
				CatchSlamEffect->Set_EffectOwner(EffectOwner);
				CatchSlamEffect->Set_EffectOrbitRotation(0.f, 0.f);
				CatchSlamEffect->Set_EffectRotation({ 0.f, 0.f, 0.f });

				CatchSlamEffect->Set_DissolveScrollSpeed({ 0.f, -2.5f });
				CatchSlamEffect->Set_EffectScrollSpeed({ 0.f, -3.5f });

				CatchSlamEffect->Set_DissolveTextureOffset({ 0.f, 0.f });
				CatchSlamEffect->Set_EffectTextureOffset({ 0.f, 0.f });

				CatchSlamEffect->Set_DissolveStrength(0.1f);
				CatchSlamEffect->Set_EffectScale({ 7.f, 7.f, 7.f });
				CatchSlamEffect->Set_EffectLifeTime(1.f);

				CatchSlamEffect->Set_EffectFadeOutValue(0.4f);

				CatchSlamEffect->Set_DistanceFromPlayer(0.f);
				CatchSlamEffect->Set_EffectOffset({ 0.25f, 0.2f, -6.f });
				CatchSlamEffect->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });
				EffectOwner->Play_Effect(CatchSlamEffect);
			}
			{
				auto SlamWave = EffectOwner->Get_EffectProto("EFF_CatchSlam_UpWave");

				SlamWave->Set_EffectOwner(EffectOwner);
				SlamWave->Set_EffectOrbitRotation(0.f, 0.f);
				SlamWave->Set_EffectRotation({ 0.f, 0.f, 0.f });

				SlamWave->Set_DissolveScrollSpeed({ 0.f, 2.5f });
				SlamWave->Set_EffectScrollSpeed({ 0.f, 3.5f });
				SlamWave->Set_EffectAlpha(0.75f);

				SlamWave->Set_DissolveTextureOffset({ 0.f, 0.f });
				SlamWave->Set_EffectTextureOffset({ 0.f, 0.f });

				SlamWave->Set_DissolveStrength(0.1f);
				SlamWave->Set_EffectScale({ 3.f, 5.f, 3.f });
				SlamWave->Set_EffectLifeTime(1.f);

				SlamWave->Set_EffectFadeOutValue(0.4f);
				SlamWave->Set_DistanceFromPlayer(0.f);
				SlamWave->Set_EffectOffset({ 0.25f, 0.2f, -6.f });
				SlamWave->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });
				EffectOwner->Play_Effect(SlamWave);
			}
			PlayerCam->Camera_Shake(0.1f, 0.25f);
			RenderFlag[54] = false;
		}
	}

	if (AnimIndex == static_cast<uint32_t>(MONSTER_SCAR::ATTACK07)) {
		if (RenderFlag[70] && Progress >= 0.36f) {
			auto SwordTrail = EffectOwner->Get_EffectProto("EFF_CircleTrail");

			SwordTrail->Set_EffectOwner(EffectOwner);
			SwordTrail->Set_EffectOrbitRotation(0.f, 0.f);
			SwordTrail->Set_EffectRotation({ 0.f, -30.f, -1.f });

			SwordTrail->Set_DissolveScrollSpeed({ -4.f, 0.f });
			SwordTrail->Set_EffectScrollSpeed({ -4.f, 0.f });

			SwordTrail->Set_DissolveTextureOffset({ 0.75f, 0.f });
			SwordTrail->Set_EffectTextureOffset({ 0.75f, 0.f });

			SwordTrail->Set_DissolveStrength(0.5f);
			SwordTrail->Set_EffectScale({ 12.f, 12.f, 12.f });
			SwordTrail->Set_EffectLifeTime(0.75f);

			SwordTrail->Set_EffectFadeOutValue(0.4f);
			SwordTrail->Set_DistanceFromPlayer(-6.f);
			SwordTrail->Set_EffectOffset({ 0.f, 2.f, 0.f });
			SwordTrail->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });

			EffectOwner->Play_Effect(SwordTrail);
			RenderFlag[70] = false;
		}
	}
	if (AnimIndex == static_cast<uint32_t>(MONSTER_SCAR::ATTACK08)) {
		if (RenderFlag[80] && Progress >= 0.36f) {
			auto SwordTrail = EffectOwner->Get_EffectProto("EFF_CircleTrail");

			SwordTrail->Set_EffectOwner(EffectOwner);
			SwordTrail->Set_EffectOrbitRotation(0.f, 0.f);
			SwordTrail->Set_EffectRotation({ 0.f, -30.f, -1.f });

			SwordTrail->Set_DissolveScrollSpeed({ -5.f, 0.f });
			SwordTrail->Set_EffectScrollSpeed({ -5.f, 0.f });

			SwordTrail->Set_DissolveTextureOffset({ 0.75f, 0.f });
			SwordTrail->Set_EffectTextureOffset({ 0.75f, 0.f });

			SwordTrail->Set_DissolveStrength(0.5f);
			SwordTrail->Set_EffectScale({ 12.f, 12.f, 12.f });
			SwordTrail->Set_EffectLifeTime(0.5f);

			SwordTrail->Set_EffectFadeOutValue(0.4f);
			SwordTrail->Set_DistanceFromPlayer(-6.f);
			SwordTrail->Set_EffectOffset({ 0.f, 2.f, 0.f });
			SwordTrail->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });

			EffectOwner->Play_Effect(SwordTrail);
			RenderFlag[80] = false;
		}
	}
	if (AnimIndex == static_cast<uint32_t>(MONSTER_SCAR::ATTACK09)) {
		static shared_ptr<BossEffect> Trail01 = { nullptr }, Trail02 = { nullptr }, Trail03 = { nullptr };
		if (RenderFlag[90] && Progress >= 0.25f && Trail01 == nullptr) {
			Trail01 = EffectOwner->Get_EffectProto("EFF_TrailUp_V2");

			Trail01->Set_EffectOwner(EffectOwner);
			Trail01->Set_EffectOrbitRotation(0.f, 0.f);
			Trail01->Set_EffectRotation({ 0.f, 0.f, 0.f });

			Trail01->Set_EmissiveOption({50.f / 255.f , 35.f / 255.f, 100.f / 255.f }, 5.f);

			Trail01->Set_DissolveScrollSpeed({ 0.f, -1.5f });
			Trail01->Set_EffectScrollSpeed({ 0.f, -1.5f });
			Trail01->Set_ShaderPassNumber(1);

			Trail01->Set_DissolveTextureOffset({ 0.f, -1.f });
			Trail01->Set_EffectTextureOffset({ 0.f, -1.f });

			Trail01->Set_DissolveStrength(0.f);
			Trail01->Set_EffectScale({ 20.f, 30.f, 20.f });
			Trail01->Set_EffectLifeTime(1.f);

			Trail01->Set_EffectFadeOutValue(0.4f);
			Trail01->Set_DistanceFromPlayer(0.f);
			Trail01->Set_EffectOffset({ 3.f, 0.f, -6.f });
			Trail01->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });
			Trail01->Set_EffectLoop(true);

			EffectOwner->Play_Effect(Trail01);
			RenderFlag[90] = false;
		}
		if (RenderFlag[91] && Progress >= 0.25f && Trail02 == nullptr) {
			Trail02 = EffectOwner->Get_EffectProto("EFF_TrailUp_V2");

			Trail02->Set_EffectOwner(EffectOwner);
			Trail02->Set_EffectOrbitRotation(0.f, 0.f);
			Trail02->Set_EffectRotation({ 0.f, 90.f, 0.f });

			Trail02->Set_EmissiveOption({ 50.f / 255.f , 35.f / 255.f, 100.f / 255.f }, 5.f);

			Trail02->Set_DissolveScrollSpeed({ 0.f, -1.f });
			Trail02->Set_EffectScrollSpeed({ 0.f, -1.f });
			Trail02->Set_ShaderPassNumber(1);

			Trail02->Set_DissolveTextureOffset({ 0.f, 1.f });
			Trail02->Set_EffectTextureOffset({ 0.f, 1.f });

			Trail02->Set_DissolveStrength(0.f);
			Trail02->Set_EffectScale({ 20.f, 30.f, 20.f });
			Trail02->Set_EffectLifeTime(1.f);

			Trail02->Set_EffectFadeOutValue(0.4f);
			Trail02->Set_DistanceFromPlayer(0.f);
			Trail02->Set_EffectOffset({ 3.f, 0.f, -6.f });
			Trail02->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });
			Trail02->Set_EffectLoop(true);

			EffectOwner->Play_Effect(Trail02);
			RenderFlag[91] = false;
		}
		if (RenderFlag[92] && Progress >= 0.25f && Trail03 == nullptr) {
			Trail03 = EffectOwner->Get_EffectProto("EFF_TrailUp_V2");

			Trail03->Set_EffectOwner(EffectOwner);
			Trail03->Set_EffectOrbitRotation(0.f, 0.f);
			Trail03->Set_EffectRotation({ 0.f, 45.f, 0.f });

			Trail03->Set_EmissiveOption({ 50.f / 255.f , 35.f / 255.f, 100.f / 255.f }, 5.f);

			Trail03->Set_DissolveScrollSpeed({ 0.f, -1.5f });
			Trail03->Set_EffectScrollSpeed({ 0.f, -1.5f });
			Trail03->Set_ShaderPassNumber(1);

			Trail03->Set_DissolveTextureOffset({ 0.f, -1.f });
			Trail03->Set_EffectTextureOffset({ 0.f, -1.f });

			Trail03->Set_DissolveStrength(0.f);
			Trail03->Set_EffectScale({ 20.f, 30.f, 20.f });
			Trail03->Set_EffectLifeTime(1.f);

			Trail03->Set_EffectFadeOutValue(0.4f);
			Trail03->Set_DistanceFromPlayer(0.f);
			Trail03->Set_EffectOffset({ 3.f, 0.f, -6.f });
			Trail03->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });
			Trail03->Set_EffectLoop(true);

			EffectOwner->Play_Effect(Trail03);
			RenderFlag[92] = false;
		}
		
		if (RenderFlag[93] && Progress >= 0.45f) {
			Trail01->Set_ShaderPassNumber(0);
			Trail01->Set_TimeAccumulation(0.f);
			Trail01->Set_TimeProgress(0.f);
			Trail01->Set_EffectLoop(false);

			Trail02->Set_ShaderPassNumber(0);
			Trail02->Set_TimeAccumulation(0.f);
			Trail02->Set_TimeProgress(0.f);
			Trail02->Set_EffectLoop(false);

			Trail03->Set_ShaderPassNumber(0);
			Trail03->Set_TimeAccumulation(0.f);
			Trail03->Set_TimeProgress(0.f);
			Trail03->Set_EffectLoop(false);

			Trail01 = { nullptr }; Trail02 = { nullptr }; Trail03 = { nullptr };
			RenderFlag[93] = false;
		}

		if (RenderFlag[94] && Progress >= 0.685f) {
			auto SwordTrail = EffectOwner->Get_EffectProto("EFF_CircleTrail_V2");

			SwordTrail->Set_EffectOwner(EffectOwner);
			SwordTrail->Set_EffectOrbitRotation(0.f, 0.f);
			SwordTrail->Set_EffectRotation({ 15.f, -30.f, 60.f });

			SwordTrail->Set_DissolveScrollSpeed({ -1.5f, 0.f });
			SwordTrail->Set_EffectScrollSpeed({ -1.5f, 0.f });

			SwordTrail->Set_DissolveTextureOffset({ 0.5f, 0.f });
			SwordTrail->Set_EffectTextureOffset({ 0.5f, 0.f });

			SwordTrail->Set_DissolveStrength(0.2f);
			SwordTrail->Set_EffectScale({ 7.f, 7.f, 7.f });
			SwordTrail->Set_EffectLifeTime(1.f);

			SwordTrail->Set_EffectFadeOutValue(0.1f);
			SwordTrail->Set_DistanceFromPlayer(0.f);
			SwordTrail->Set_EffectOffset({ -3.f, 5.f, 0.f });
			SwordTrail->Set_DissolveEdgeColor({ 0.255f, 0.157f, 0.941f });

			EffectOwner->Play_Effect(SwordTrail);
			RenderFlag[94] = false;
		}
	}
	if (AnimIndex == static_cast<uint32_t>(MONSTER_SCAR::ATTACK11)) {
		//if (RenderFlag[100] && Progress >= 0.185f) {
			
			//RenderFlag[100] = false;
		//}
	}
	if (AnimIndex == static_cast<uint32_t>(MONSTER_SCAR::ATTACK13)) {
		if (RenderFlag[120] && Progress >= 0.4f) {
			EffectOwner->Enable_Dissolve(PLAY_TYPE::ONSTART, false, 0.f, 1.5f);
			RenderFlag[120] = false;
		}
		if (RenderFlag[121] && Progress >= 0.9f) {
			XMVECTOR NextPosition = EffectOwner->Get_TransformComponent()->Get_WorldPosition() + XMVectorSet(RANDOM(20.f, -20.f), 0.f, RANDOM(20.f, -20.f), 0.f);
			EffectOwner->Get_TransformComponent()->Set_WorldPosition(NextPosition);
			EffectOwner->Enable_Dissolve(PLAY_TYPE::ONSTART, true, 0.f, 1.5f);
			RenderFlag[121] = false;
		}
	}
	for (uint32_t IDX = 0; IDX < (static_cast<uint32_t>(MONSTER_SCAR::ATTACK13) + 1) * 10; ++IDX) {
		if (Component_Animator->Get_AnimationByIndex(IDX / 10)->Get_AnimationState() == ANIMATION_STATE::FINISHED) {
			for (uint32_t AIDX = IDX; AIDX < IDX + 10; ++AIDX) {
				RenderFlag[AIDX] = true;
			}
		}
	}
	
	return NODESTATE::FAILURE;
}

NODESTATE Execute_HitStop::Update(const _float _DT){
	if (*HitCall == true) {
		GameInstance::GetInstance().Get_TimeManager()->Execute_HitStop(0.15f, 0.1f);
		*HitCall = false;
	}
	return NODESTATE::SUCCESS;
}

NODESTATE BS_Play_DeathAnimation::Update(const _float _DT){
	if (Component_Animator->Get_CurrentAnimationIndex() != Death_AnimationIndex) {
		Component_Animator->Play_Animation(Death_AnimationIndex);
	}
	if (EffectTrigger && Component_Animator->Get_CurrentAnimation()->Get_AnimationProgress() >= 0.7f) {
		static_pointer_cast<Monster_FinalBoss>(Owner)->Enable_Dissolve(PLAY_TYPE::ONSTART, false, 0.f, 2.f);
	}
	if (EffectTrigger && Component_Animator->Get_CurrentAnimation()->Get_AnimationProgress() >= 0.8f) {
		XMFLOAT3 Pos, Rot;
		XMStoreFloat3(&Pos, Component_Transform->Get_WorldPosition());
		XMStoreFloat3(&Rot, Component_Transform->Get_WorldRotation());

		GameInstance::GetInstance().Get_EffectManager()->Play_Effect("Void_Disappear_Effect", Pos, Rot, { 6.f, 6.f, 6.f }, 60.f);

		EffectTrigger = false;
	}
	if (Component_Animator->Get_CurrentAnimation()->Get_AnimationState() == ANIMATION_STATE::FINISHED) {
		return NODESTATE::SUCCESS;
	}

	return NODESTATE::RUNNING;
}
