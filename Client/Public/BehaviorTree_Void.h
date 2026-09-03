#pragma once

using namespace BehaviorTree;

class BehaviorTree_Void {
private:
	BehaviorTree_Void(shared_ptr<BTBlackBoard> _BlackBoard);
public:
	~BehaviorTree_Void();

public:
	HRESULT BehaviorTree_Initialize();
	VOID	BehaviorTree_Update(CONST FLOAT& _DT);

	VOID	Set_RootNode(unique_ptr<BTNode>	_RootNode) { RootNode = move(_RootNode); }
	shared_ptr<BTBlackBoard>& Get_BlackBoard() { return BlackBoard; }

public:
	static shared_ptr<BehaviorTree_Void> Create(shared_ptr<BTBlackBoard> _BlackBoard);

private:
	unique_ptr<BTNode>			RootNode;
	shared_ptr<BTBlackBoard>	BlackBoard;
};

class Attack_AnimationNode : public ConditionNode {
public:
	Attack_AnimationNode(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner, vector<uint32_t>& _IndexList, _float _Speed)
		: ConditionNode(_BlackBoard, _Owner), AnimationIndexList(_IndexList), AnimationSpeed(_Speed) {};
	virtual ~Attack_AnimationNode() {};

public:
	virtual NODESTATE Update(const _float _DT) override;

private:
	vector<uint32_t>	AnimationIndexList;
	_float				AnimationSpeed;
};
class VD_Play_DeathAnimation : public ActionNode {
public:
	VD_Play_DeathAnimation(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner, shared_ptr<Transform> _Transform, shared_ptr<Animator> _Animator, uint32_t _DeathAnimationIndex)
		: ActionNode(_BlackBoard, _Owner), Component_Transform(_Transform), Component_Animator(_Animator), Death_AnimationIndex(_DeathAnimationIndex) {
	};
	virtual ~VD_Play_DeathAnimation() {};

public:
	virtual NODESTATE Update(const _float _DT) override;

private:
	shared_ptr<Animator>	Component_Animator;
	shared_ptr<Transform>	Component_Transform;
	uint32_t				Death_AnimationIndex;
	_bool					EffectTrigger = { true };
};
class VD_ResetToCombatNode : public ActionNode {
public:
	VD_ResetToCombatNode(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner, _float _Speed = 1.f) 
		: ActionNode(_BlackBoard, _Owner), AnimationSpeed(_Speed){};
	virtual ~VD_ResetToCombatNode() {};

public:
	virtual NODESTATE Update(const _float _DT) override;
private:
	_float AnimationSpeed = { 1.f };

};
class Attack_RotateNode : public ActionNode {
public:
	Attack_RotateNode(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner, _float _RotationSpeed)
		: ActionNode(_BlackBoard, _Owner), RotationSpeed(_RotationSpeed) {};
	virtual ~Attack_RotateNode() {};

public:
	virtual NODESTATE Update(const _float _DT) override;
private:
	_float RotationSpeed = { 0.f };
};
class Attack_TranslateNode : public ActionNode {
public:
	Attack_TranslateNode(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner, uint32_t _Index, _float _Speed, _float _Range = 1.5f)
		: ActionNode(_BlackBoard, _Owner), AnimationIndex(_Index), TranslateSpeed(_Speed), AttackableRange(_Range){};
	virtual ~Attack_TranslateNode() {};

public:
	virtual NODESTATE Update(const _float _DT) override;

private:
	uint32_t	AnimationIndex = { 0 };
	_float		TranslateSpeed = { 0.f };
	_float		AttackableRange = { 0.f };
};
class Attack_LerpRoatation : public ActionNode {
public:
	Attack_LerpRoatation(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner)
		: ActionNode(_BlackBoard, _Owner) {
	};
	virtual ~Attack_LerpRoatation() {};

public:
	virtual NODESTATE Update(const _float _DT) override;
};
class VD_RevertNode : public ActionNode {
public:
	VD_RevertNode(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner) : ActionNode(_BlackBoard, _Owner) {};
	virtual ~VD_RevertNode() {};

public:
	virtual NODESTATE Update(const _float _DT) override;
};

class VD_Play_DissolveEffect : public ActionNode {
public:
	VD_Play_DissolveEffect(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner, shared_ptr<class Monster_Void> _EffectOwner) 
		: ActionNode(_BlackBoard, _Owner), EffectOwner(_EffectOwner) {};
	virtual ~VD_Play_DissolveEffect() {};

public:
	virtual NODESTATE Update(const _float _DT) override;

private:
	shared_ptr<Monster_Void> EffectOwner = { nullptr };
}; 