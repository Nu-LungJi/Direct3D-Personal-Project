#pragma once
#include "GameObject.h"

class Player : public GameObject {
private:
	Player(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	virtual ~Player();

public:
	virtual HRESULT		Initialize_ProtoType();
	virtual HRESULT		Initialize(VOID* _ARG);
	virtual VOID		Update(CONST _float& _DT);
	virtual	VOID		Late_Update(CONST _float& _DT);
	virtual HRESULT		Render();

	virtual VOID		On_CollisionEnter(shared_ptr<GameObject> _ColliderOwner) override;
	virtual VOID		On_CollisionStay(shared_ptr<GameObject> _ColliderOwner)  override;
	virtual VOID		On_CollisionExit(shared_ptr<GameObject> _ColliderOwner)  override;

	VOID	Player_Controller(const _float& _DT);

	VOID	Generate_EffectList();

	VOID	HitBox_Controller(const _float& _DT);

	shared_ptr<PlayerEffect>	Get_EffectProto(const string& _EffectTag);
	VOID	Play_Effect(shared_ptr<PlayerEffect> _ProtoEffect);
	shared_ptr<class HitBoxPool>			Get_HitBoxPool()			{ return PlayerHitBoxPool;  }

public:
	shared_ptr<class PlayerStateMachine>	Get_StateMachine()			{ return StateMachine;  }

	VOID							Set_AttackCombo(uint32_t _Combo)	{ AttackCombo = _Combo; }
	uint32_t*						Get_AttackCombo()					{ return &AttackCombo;	}

	VOID							Set_JumpCount(uint32_t _JumpCNT)	{ JumpCount = _JumpCNT; }
	uint32_t*						Get_JumpCount()						{ return &JumpCount;	}

	VOID							Set_RenderFlag(_bool _Flag)			{ RenderFlag = _Flag; }
	_bool*							Get_RenderFlag()					{ return &RenderFlag; }

	shared_ptr<class ActionCamera>	Get_ActionCamera()	{ return PlayerActionCamera; }
	shared_ptr<class PlayerCamera>	Get_PlayerCamera()	{ return PlayerMainCamera;	}

public:
	shared_ptr<Transform>			Get_TransformComponent() { return Component_Transform; }
	shared_ptr<Animator>			Get_AnimatorComponent() { return Component_Animator; }
	shared_ptr<Shader>				Get_ShaderComponent() { return Component_Shader; }
	shared_ptr<MeshLoader>			Get_ModelComponent() { return Component_Model; }
	shared_ptr<Collider>			Get_ColliderComponent() { return Component_Collider; }
	shared_ptr<NavMeshAgent>		Get_NavMeshAgentComponent() { return Component_NavMeshAgent; }
	
	shared_ptr<HitBoxPool>			Get_PlayerHitBoxPool() { return PlayerHitBoxPool; }

public:
	unordered_map<string, shared_ptr<class PlayerEffect>>* Get_EffectProtoList() { return &EffectProtoList; }

public:
	static	unique_ptr<Player>		Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>	Clone(VOID* _ARG);

private:
	shared_ptr<MeshLoader>		Component_Model;
	shared_ptr<Transform>		Component_Transform;
	shared_ptr<Animator>		Component_Animator;
	shared_ptr<Collider>		Component_Collider;
	shared_ptr<Shader>			Component_Shader;
	shared_ptr<NavMeshAgent>	Component_NavMeshAgent;

	shared_ptr<class PlayerStateMachine>	StateMachine		= { nullptr };
	shared_ptr<Bone>						ModelRootBone		= { nullptr };
	shared_ptr<ActionCamera>				PlayerActionCamera	= { nullptr };
	shared_ptr<PlayerCamera>				PlayerMainCamera	= { nullptr };

	ComPtr<ID3D11ShaderResourceView>		MatCapResource = { nullptr };

	shared_ptr<HitBoxPool>					PlayerHitBoxPool = { nullptr };

	uint32_t					AttackCombo = { 0 };
	_float						AttackComboInitTime = { 0.f };
	uint32_t					JumpCount	= { 0 };

	shared_ptr<GameObject>		EffectObject = { nullptr };

	unordered_map<string, shared_ptr<class PlayerEffect>> EffectProtoList;
};

