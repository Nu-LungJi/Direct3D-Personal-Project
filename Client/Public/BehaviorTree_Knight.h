#pragma once

using namespace BehaviorTree;

class BehaviorTree_Knight {
private:
	BehaviorTree_Knight(shared_ptr<BTBlackBoard> _BlackBoard);
public:
	~BehaviorTree_Knight();

public:
	HRESULT BehaviorTree_Initialize();
	VOID	BehaviorTree_Update(CONST FLOAT& _DT);

	VOID	Set_RootNode(unique_ptr<BTNode>	_RootNode)	{ RootNode = move(_RootNode);	}
	shared_ptr<BTBlackBoard>& Get_BlackBoard()			{ return BlackBoard;			}

public:
	static shared_ptr<BehaviorTree_Knight> Create(shared_ptr<BTBlackBoard>	_BlackBoard);

private:
	unique_ptr<BTNode>			RootNode;
	shared_ptr<BTBlackBoard>	BlackBoard;
};

/// Check Owner State (Health)
class Check_DeathCondition : public ConditionNode {
public:
	Check_DeathCondition(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner) : ConditionNode(_BlackBoard, _Owner) {};
	virtual ~Check_DeathCondition() {};

public:
	virtual NODESTATE Update(const _float _DT) override;
};
class Play_DeathAnimation : public ActionNode {
public:
	Play_DeathAnimation(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner, shared_ptr<Transform> _Transform, shared_ptr<Animator> _Animator, uint32_t _DeathAnimationIndex)
		: ActionNode(_BlackBoard, _Owner), Component_Transform(_Transform), Component_Animator(_Animator), Death_AnimationIndex(_DeathAnimationIndex){};
	virtual ~Play_DeathAnimation() {};

public:
	virtual NODESTATE Update(const _float _DT) override;

private:
	shared_ptr<Animator>	Component_Animator;
	shared_ptr<Transform>	Component_Transform;
	uint32_t				Death_AnimationIndex;
	_bool					EffectTrigger = { true };
};
class Destroy_OwnerActor : public ActionNode {
public:
	Destroy_OwnerActor(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner) : ActionNode(_BlackBoard, _Owner) {};
	virtual ~Destroy_OwnerActor() {};

public:
	virtual NODESTATE Update(const _float _DT) override;
};
class Play_DissolveEffect : public ActionNode {
public:
	Play_DissolveEffect(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner, shared_ptr<class Monster_Knight> _Knight)
		: ActionNode(_BlackBoard, _Owner), EffectOwner(_Knight){
	};
	virtual ~Play_DissolveEffect() {};

public:
	virtual NODESTATE Update(const _float _DT) override;

private:
	shared_ptr<Monster_Knight> EffectOwner = { nullptr };
};
/// Combat Node 

class Check_TargetDistance : public ConditionNode {
public:
	Check_TargetDistance(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner, shared_ptr<Transform>	_TargetTRS, _float _Distance)
		: ConditionNode(_BlackBoard, _Owner), TargetTransform(_TargetTRS), Distance(_Distance) {
		OwnerTransform = BlackBoard->Get_Value<shared_ptr<Transform>>("OwnerTransform");
	};
	virtual ~Check_TargetDistance() {};

public:
	virtual NODESTATE Update(const _float _DT) override;

private:
	_float					Distance;
	shared_ptr<Transform>	TargetTransform;
	shared_ptr<Transform>	OwnerTransform;
};

class Rotate_CenterPoint : public ActionNode {
public:
	Rotate_CenterPoint(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner, _float _RotationSpeed)
		: ActionNode(_BlackBoard, _Owner), RotationSpeed(_RotationSpeed) {
	};
	virtual ~Rotate_CenterPoint() {};

public:
	virtual NODESTATE Update(const _float _DT) override;
private:
	_float RotationSpeed;
};

class Play_StagingAnimation : public ActionNode {
public:
	Play_StagingAnimation(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner, uint32_t _WakeUpAnim, _float _Speed = 1.f)
		: ActionNode(_BlackBoard, _Owner), WakeUpAnimation(_WakeUpAnim), AnimationSpeed(_Speed){};
	virtual ~Play_StagingAnimation() {};

public:
	virtual NODESTATE Update(const _float _DT) override;
private:
	uint32_t WakeUpAnimation;
	_float	 AnimationSpeed;
};

class Check_FirstDetect : public ConditionNode {
public:
	Check_FirstDetect(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner) : ConditionNode(_BlackBoard, _Owner) {};
	virtual ~Check_FirstDetect() {};

public:
	virtual NODESTATE Update(const _float _DT) override;
};
class Disable_FirstDetect : public ConditionNode {
public:
	Disable_FirstDetect(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner) : ConditionNode(_BlackBoard, _Owner) {};
	virtual ~Disable_FirstDetect() {};

public:
	virtual NODESTATE Update(const _float _DT) override;
};

class Play_CombatStateAnimation : public ActionNode {
public:
	Play_CombatStateAnimation(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner, uint32_t _BSAnim)
		: ActionNode(_BlackBoard, _Owner), BattleStateAnimation(_BSAnim) {};
	virtual ~Play_CombatStateAnimation() {};

public:
	virtual NODESTATE Update(const _float _DT) override;
private:
	uint32_t BattleStateAnimation;
};
class Move_CenterPoint : public ActionNode {
public:
	Move_CenterPoint(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner, uint32_t _MoveAnimIndex)
		: ActionNode(_BlackBoard, _Owner), MoveAnimationIndex(_MoveAnimIndex) {};
	virtual ~Move_CenterPoint() {};

public:
	virtual NODESTATE Update(const _float _DT) override;

private:
	uint32_t MoveAnimationIndex = { 0 };
};
class Play_StandByAnimation : public ActionNode {
public:
	Play_StandByAnimation(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner) : ActionNode(_BlackBoard, _Owner) {};
	virtual ~Play_StandByAnimation() {};

public:
	virtual NODESTATE Update(const _float _DT) override;
};
class Convert_CombatState : public ConditionNode {
public:
	Convert_CombatState(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner) : ConditionNode(_BlackBoard, _Owner){};
	virtual ~Convert_CombatState() {};

public:
	virtual NODESTATE Update(const _float _DT) override;
};
class Play_ConvertAnimation : public ActionNode {
public:
	Play_ConvertAnimation(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner, shared_ptr<Animator> _Animator, uint32_t _Index, _float _Speed = 1.f) 
		: ActionNode(_BlackBoard, _Owner), Component_Animator(_Animator), AnimationIndex(_Index), AnimationSpeed(_Speed){};
	virtual ~Play_ConvertAnimation() {};

public:
	virtual NODESTATE Update(const _float _DT) override;
private:
	shared_ptr<Animator> Component_Animator;
	uint32_t			 AnimationIndex = { 0 };
	_float				 AnimationSpeed = { 1.f };
};

class Play_AttackedAnimation : public ConditionNode {
public:
	Play_AttackedAnimation(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner, shared_ptr<Animator> _Animator, shared_ptr<Transform> _Transform, _bool* _HitCall, uint32_t _AnimIndex, _float _Speed)
		: ConditionNode(_BlackBoard, _Owner), Component_Animator(_Animator), Component_Transform(_Transform), HitCall(_HitCall), AnimationIndex(_AnimIndex), AnimationSpeed(_Speed){ };
	virtual ~Play_AttackedAnimation() {};

public:
	virtual NODESTATE Update(const _float _DT) override;
private:
	shared_ptr<Animator>	Component_Animator	= { nullptr };
	shared_ptr<Transform>	Component_Transform = { nullptr };
	_bool*					HitCall				= { nullptr };
	_float					AnimationSpeed		= { 0.f };
	uint32_t				AnimationIndex		= { 0 };
	_bool					HitStopTrigger		= { false };
};