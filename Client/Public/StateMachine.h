#pragma once
#include "Player.h"

class State {
public:
	State(shared_ptr<Player> _Owner) : Owner(_Owner) {};
	virtual ~State() = default;

public:
	virtual VOID	FSM_StateEnter()					= 0;
	virtual VOID	FSM_StateUpdate(CONST _float& _DT)	= 0;
	virtual VOID	FSM_StateExit()						= 0;

protected:
	shared_ptr<Player>	Owner = { nullptr };
	_bool				InitializeFlag = { true };
};

class PlayerStateMachine {
private:
	PlayerStateMachine(shared_ptr<Player> _Owner) : Owner(_Owner) {}
public:
	~PlayerStateMachine() {}

public:
	HRESULT				Initialize_StateMachine();
	HRESULT				Update_StateMachine(const _float& _DT);

	HRESULT				FSM_StateChange(PLAYER_STATE _State);

	State*				FSM_GetCurrentState()			{ return CurrentState;		  }
	State*				FSM_GetPreviousState()			{ return PreviousState;		  }

	PLAYER_STATE		FSM_GetCurrentPlayerState()		{ return CurrentPlayerState;  }
	PLAYER_STATE		FSM_GetPreviousPlayerState()	{ return PreviousPlayerState; }

	HRESULT				FSM_SetOwner(shared_ptr<Player> _Owner);

	static	shared_ptr<PlayerStateMachine> Create(shared_ptr<Player> _Owner);

private:
	shared_ptr<Player>			Owner = { nullptr };
	vector<unique_ptr<State>>	StateList;

	State*			CurrentState		= { nullptr };
	State*			PreviousState		= { nullptr };

	PLAYER_STATE	CurrentPlayerState	= { PLAYER_STATE::STATE_END };
	PLAYER_STATE	PreviousPlayerState = { PLAYER_STATE::STATE_END };
};

class IdleState : public State {
	using State::State;
public:
	virtual VOID	FSM_StateEnter()					override;
	virtual VOID	FSM_StateUpdate(CONST _float& _DT)	override;
	virtual VOID	FSM_StateExit()						override;

private:
	shared_ptr<Animator>			Component_Animator	= { nullptr };
	shared_ptr<PlayerStateMachine>	StateMachine		= { nullptr };

	shared_ptr<Animation>			IdleAnimationList[4];
};
class WalkState : public State {
	using State::State;
public:
	virtual VOID	FSM_StateEnter()					override;
	virtual VOID	FSM_StateUpdate(CONST _float& _DT)	override;
	virtual VOID	FSM_StateExit()						override;

private:
	shared_ptr<Animator>	 Component_Animator		= { nullptr };
	shared_ptr<Transform>	 Component_Transform	= { nullptr };
	shared_ptr<NavMeshAgent> Component_NavMeshAgent	= { nullptr };
	shared_ptr<PlayerStateMachine>	StateMachine	= { nullptr };

	shared_ptr<Transform>	 PlayerCamera_Transform	= { nullptr };

	FMOD::Channel*			 SFX_FootStep	= { nullptr };
	_float					 FootStepTimer	= { 0.f };
};
class RunningState : public State {
	using State::State;
public:
	virtual VOID	FSM_StateEnter()					override;
	virtual VOID	FSM_StateUpdate(CONST _float& _DT)	override;
	virtual VOID	FSM_StateExit()						override;

private:
	shared_ptr<Animator>	Component_Animator		= { nullptr };
	shared_ptr<Transform>	Component_Transform		= { nullptr };
	shared_ptr<NavMeshAgent> Component_NavMeshAgent = { nullptr };
	shared_ptr<PlayerStateMachine>	StateMachine	= { nullptr };

	shared_ptr<Transform>	PlayerCamera_Transform	= { nullptr };
	FMOD::Channel*			SFX_FootStep	= { nullptr };	
	_float					FootStepTimer	= { 0.f };
};
class DashState : public State {
	using State::State;
public:
	virtual VOID	FSM_StateEnter()					override;
	virtual VOID	FSM_StateUpdate(CONST _float& _DT)	override;
	virtual VOID	FSM_StateExit()						override;

private:
	shared_ptr<Animator>	Component_Animator		= { nullptr };
	shared_ptr<Transform>	Component_Transform		= { nullptr };
	shared_ptr<NavMeshAgent> Component_NavMeshAgent = { nullptr };
	shared_ptr<PlayerStateMachine>	StateMachine	= { nullptr };
	uint32_t* ObjectOnAir = { nullptr };
};
class AttackState : public State {
	using State::State;
public:
	virtual VOID	FSM_StateEnter()					override;
	virtual VOID	FSM_StateUpdate(CONST _float& _DT)	override;
	virtual VOID	FSM_StateExit()						override;
private:
	shared_ptr<Animator>	Component_Animator		= { nullptr };
	shared_ptr<Transform>	Component_Transform		= { nullptr };
	shared_ptr<NavMeshAgent> Component_NavMeshAgent = { nullptr };
	shared_ptr<PlayerStateMachine>	StateMachine	= { nullptr };

	shared_ptr<PlayerCamera>		PlayerCam		= { nullptr };

	uint32_t* AttackCombo = { nullptr };
	uint32_t* ObjectOnAir = { nullptr };

	_bool	  EffectFlag[60] = { true };
};
class LandState : public State {
	using State::State;
public:
	virtual VOID	FSM_StateEnter()					override;
	virtual VOID	FSM_StateUpdate(CONST _float& _DT)	override;
	virtual VOID	FSM_StateExit()						override;
private:
	shared_ptr<Animator>	 Component_Animator		= { nullptr };
	shared_ptr<Transform>	 Component_Transform	= { nullptr };
	shared_ptr<NavMeshAgent> Component_NavMeshAgent = { nullptr };

	shared_ptr<PlayerStateMachine> StateMachine		= { nullptr };
	shared_ptr<Transform>	 PlayerCamera_Transform = { nullptr };

	uint32_t* JumpCount								= { nullptr };
	uint32_t* AttackCombo							= { nullptr };
};
class BoostState : public State {
	using State::State;
public:
	virtual VOID	FSM_StateEnter()					override;
	virtual VOID	FSM_StateUpdate(CONST _float& _DT)	override;
	virtual VOID	FSM_StateExit()						override;
private:
	shared_ptr<Animator>	 Component_Animator		= { nullptr };
	shared_ptr<Transform>	 Component_Transform	= { nullptr };
	shared_ptr<NavMeshAgent> Component_NavMeshAgent = { nullptr };
	shared_ptr<PlayerStateMachine> StateMachine		= { nullptr };
	shared_ptr<PlayerCamera> PlayerCam				= { nullptr };

	_bool					 FOVOutFlag = { true };
	_bool					 FOVInFlag	= { true };

	_bool					 HitBoxFlag = { true };
	_bool					 EffectFlag = { true };
};
class JumpState : public State {
	using State::State;
public:
	virtual VOID	FSM_StateEnter()					override;
	virtual VOID	FSM_StateUpdate(CONST _float& _DT)	override;
	virtual VOID	FSM_StateExit()						override;
private:
	shared_ptr<Animator>	 Component_Animator		= { nullptr };
	shared_ptr<Transform>	 Component_Transform	= { nullptr };
	shared_ptr<NavMeshAgent> Component_NavMeshAgent = { nullptr };
	shared_ptr<PlayerStateMachine> StateMachine		= { nullptr };
	shared_ptr<Transform>	 PlayerCamera_Transform = { nullptr };

	uint32_t*	JumpCount	= { nullptr };
	uint32_t*	ObjectOnAir = { nullptr };
};
class SkillState : public State {
	using State::State;
public:
	virtual VOID	FSM_StateEnter()					override;
	virtual VOID	FSM_StateUpdate(CONST _float& _DT)	override;
	virtual VOID	FSM_StateExit()						override;
private:
	shared_ptr<Animator>	 Component_Animator		= { nullptr };
	shared_ptr<Transform>	 Component_Transform	= { nullptr };
	shared_ptr<NavMeshAgent> Component_NavMeshAgent = { nullptr };
	shared_ptr<PlayerStateMachine> StateMachine		= { nullptr };
	shared_ptr<PlayerCamera> PlayerCam				= { nullptr };

	_bool					 HitBoxFlag = { true };
	_bool					 EffectFlag[10] = { true };
};
class UltimateState : public State {
	using State::State;
public:
	virtual VOID	FSM_StateEnter()					override;
	virtual VOID	FSM_StateUpdate(CONST _float& _DT)	override;
	virtual VOID	FSM_StateExit()						override;

private:
	shared_ptr<Animator>	 Component_Animator		= { nullptr };
	shared_ptr<Transform>	 Component_Transform	= { nullptr };
	shared_ptr<NavMeshAgent> Component_NavMeshAgent = { nullptr };
	shared_ptr<PlayerCamera> PlayerCam				= { nullptr };
	shared_ptr<ActionCamera> ActionCam				= { nullptr };

	_bool					 HitBoxFlag				= { true };
	_bool					 EffectFlag				= { true };
};
class FallingState : public State {
	using State::State;
public:
	virtual VOID	FSM_StateEnter()					override;
	virtual VOID	FSM_StateUpdate(CONST _float& _DT)	override;
	virtual VOID	FSM_StateExit()						override;

private:
	shared_ptr<Animator>	 Component_Animator		= { nullptr };
	shared_ptr<Transform>	 Component_Transform	= { nullptr };
	shared_ptr<NavMeshAgent> Component_NavMeshAgent = { nullptr };
	shared_ptr<PlayerStateMachine> StateMachine		= { nullptr };
	shared_ptr<Transform>	 PlayerCamera_Transform = { nullptr };

	uint32_t* JumpCount = { nullptr };
	uint32_t* ObjectOnAir = { nullptr };
	_float	  JumpGravity = { 0.f };
};

class AirAttackState : public State {
	using State::State;
public:
	virtual VOID	FSM_StateEnter()					override;
	virtual VOID	FSM_StateUpdate(CONST _float& _DT)	override;
	virtual VOID	FSM_StateExit()						override;

private:
	shared_ptr<Animator>	 Component_Animator		= { nullptr };
	shared_ptr<Transform>	 Component_Transform	= { nullptr };
	shared_ptr<NavMeshAgent> Component_NavMeshAgent = { nullptr };
	shared_ptr<PlayerStateMachine> StateMachine		= { nullptr };

	uint32_t* JumpCount		= { nullptr };
	uint32_t* AttackCombo	= { nullptr };
};

class SlamState : public State {
	using State::State;
public:
	virtual VOID	FSM_StateEnter()					override;
	virtual VOID	FSM_StateUpdate(CONST _float& _DT)	override;
	virtual VOID	FSM_StateExit()						override;

private:
	shared_ptr<Animator>	 Component_Animator		= { nullptr };
	shared_ptr<Transform>	 Component_Transform	= { nullptr };
	shared_ptr<NavMeshAgent> Component_NavMeshAgent = { nullptr };
	shared_ptr<PlayerStateMachine> StateMachine		= { nullptr };

	uint32_t* JumpCount								= { nullptr };
	uint32_t* AttackCombo							= { nullptr };

	shared_ptr<PlayerCamera>		PlayerCam		= { nullptr };

	uint32_t SlamStateTempo = 0;
};