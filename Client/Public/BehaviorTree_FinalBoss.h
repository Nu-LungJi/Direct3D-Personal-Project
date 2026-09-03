#pragma once

using namespace BehaviorTree;
struct AttackConfig {
	uint32_t				AnimationIndex;
	shared_ptr<Animation>	AttackAnimation;
	uint32_t				Index;
};

class BehaviorTree_FinalBoss {
private:
	BehaviorTree_FinalBoss(shared_ptr<BTBlackBoard> _BlackBoard);
public:
	~BehaviorTree_FinalBoss();

public:
	HRESULT BehaviorTree_Initialize();
	VOID	BehaviorTree_Update(CONST FLOAT& _DT);

	VOID	Set_RootNode(unique_ptr<BTNode>	_RootNode)	{ RootNode = move(_RootNode);	}
	shared_ptr<BTBlackBoard>& Get_BlackBoard()			{ return BlackBoard;			}

public:
	static shared_ptr<BehaviorTree_FinalBoss> Create(shared_ptr<BTBlackBoard>	_BlackBoard);

private:
	unique_ptr<BTNode>			RootNode	= { nullptr };
	shared_ptr<BTBlackBoard>	BlackBoard	= { nullptr };
};

class Check_GroggyCondition : public ConditionNode {
public:
	Check_GroggyCondition(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner, _bool* _GroggyCall) 
		: ConditionNode(_BlackBoard, _Owner), GroggyCall(_GroggyCall){}
	virtual ~Check_GroggyCondition() {};

public:
	virtual NODESTATE Update(const _float _DT) override;

private:
	_bool* GroggyCall = { nullptr };
};
class Play_GroggyAnimation : public ActionNode {
public:
	Play_GroggyAnimation(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner, shared_ptr<Animator> _Animator, uint32_t _Index)
		: ActionNode(_BlackBoard, _Owner), Component_Animator(_Animator), AnimationIndex(_Index) {};
	virtual ~Play_GroggyAnimation() {};

public:
	virtual NODESTATE Update(const _float _DT) override;

private:
	shared_ptr<Animator>	Component_Animator = { nullptr };
	uint32_t				AnimationIndex = { 0 };
};
class Check_AttackCondition : public ConditionNode {
public:
	Check_AttackCondition(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner, shared_ptr<Animator> _Animator)
		: ConditionNode(_BlackBoard, _Owner), Component_Animator(_Animator){};
	virtual ~Check_AttackCondition() {};

public:
	virtual NODESTATE Update(const _float _DT) override;

private:
	shared_ptr<Animator>	Component_Animator = { nullptr };
	ATTACKSTATE				AttackState = { ATTACKSTATE::ONREADY };
};

class Play_AttackAnimation : public ActionNode {
public:
	Play_AttackAnimation(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner, shared_ptr<Animator> _Animator, uint32_t* _Index)
		: ActionNode(_BlackBoard, _Owner), Component_Animator(_Animator), AnimationIndex(_Index) {};
	virtual ~Play_AttackAnimation() {};

public:
	virtual NODESTATE Update(const _float _DT) override;

private:
	shared_ptr<Animator>	Component_Animator	= { nullptr };
	uint32_t*				AnimationIndex		= { nullptr };
	_bool					FirstTick = { true };
};

class LookAt_TargetPosition: public ActionNode {
public:
	LookAt_TargetPosition(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner, shared_ptr<Transform> _Transform, _float _Speed)
		: ActionNode(_BlackBoard, _Owner), Component_Transform(_Transform), RotationSpeed(_Speed){
	};
	virtual ~LookAt_TargetPosition() {};

public:
	virtual NODESTATE Update(const _float _DT) override;

private:
	shared_ptr<Transform>	Component_Transform = { nullptr };
	XMVECTOR				Direction = {};
	_float					RotationSpeed = { 0.f };
};

class Play_AttackEffect : public ActionNode {
public:
	Play_AttackEffect(shared_ptr<BTBlackBoard> _BlackBoard, shared_ptr<GameObject> _Owner, shared_ptr<Animator> _Animator)
		: ActionNode(_BlackBoard, _Owner), Component_Animator(_Animator) { 
		EffectOwner = static_pointer_cast<class Monster_FinalBoss>(_Owner);
		memset(RenderFlag, true, sizeof(_bool) * ((static_cast<uint32_t>(MONSTER_SCAR::ATTACK13) + 1) * 10));
	}
	virtual ~Play_AttackEffect() {};

public:
	virtual NODESTATE Update(const _float _DT) override;

private:
	shared_ptr<Animator>	Component_Animator = { nullptr };
	shared_ptr<class Monster_FinalBoss>	EffectOwner = { nullptr };
	_bool				RenderFlag[(static_cast<uint32_t>(MONSTER_SCAR::ATTACK13) + 1) * 10] = { true };
};
class Execute_HitStop : public ActionNode {
public:
	Execute_HitStop(shared_ptr<BTBlackBoard> _BlackBoard, shared_ptr<GameObject> _Owner, _bool* _HitCall)
		: ActionNode(_BlackBoard, _Owner), HitCall(_HitCall){ }
	virtual ~Execute_HitStop() {};

public:
	virtual NODESTATE Update(const _float _DT) override;
	
private:
	_bool*					HitCall = { nullptr };
};

class BS_Play_DeathAnimation : public ActionNode {
public:
	BS_Play_DeathAnimation(shared_ptr<BTBlackBoard> _BlackBoard, shared_ptr<GameObject> _Owner, shared_ptr<Transform> _Transform, shared_ptr<Animator> _Animator, uint32_t _DeathAnimationIndex)
		: ActionNode(_BlackBoard, _Owner), Component_Transform(_Transform), Component_Animator(_Animator), Death_AnimationIndex(_DeathAnimationIndex) {}
	virtual ~BS_Play_DeathAnimation() {};

public:
	virtual NODESTATE Update(const _float _DT) override;

private:
	shared_ptr<Animator>	Component_Animator		= { nullptr };
	shared_ptr<Transform>	Component_Transform		= { nullptr };
	uint32_t				Death_AnimationIndex	= { 0 };
	_bool					EffectTrigger			= { true };
};
