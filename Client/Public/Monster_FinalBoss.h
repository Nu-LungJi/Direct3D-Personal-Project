#pragma once
#include "GameObject.h"

class Monster_FinalBoss : public GameObject {
private:
	Monster_FinalBoss(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	virtual ~Monster_FinalBoss();

	virtual HRESULT			Initialize_ProtoType();
	virtual HRESULT			Initialize(VOID* _ARG);
	virtual VOID			Update(CONST _float& _DT);
	virtual VOID			Late_Update(CONST _float& _DT);
	virtual HRESULT			Render();

	virtual VOID			On_CollisionEnter(shared_ptr<GameObject> _ColliderOwner) override;
	virtual VOID			On_CollisionStay(shared_ptr<GameObject> _ColliderOwner)  override;
	virtual VOID			On_CollisionExit(shared_ptr<GameObject> _ColliderOwner)	 override;

public:
	VOID					Generate_BehaviorTree();
	unique_ptr<Selector>		Compose_AttackNodeList(unique_ptr<Selector> _CompositeNode);

	shared_ptr<Bone>& Get_ModelRootBone()	{ return ModelRootBone; }

	VOID	Set_HitCall(_bool _Value)		{ HitCall = _Value;		}
	_bool	Get_HitCall()					{ return HitCall;		}

	shared_ptr<class BossEffect>		Get_EffectProto(const string& _EffectTag);
	VOID								Play_Effect(shared_ptr<BossEffect> _ProtoEffect);

	VOID Generate_EffectList();

	VOID				Enable_Dissolve(PLAY_TYPE _Enable, _bool _Inverse, _float _DelayTime = 0.f, _float _DissolveSpeed = 1.f);

	PLAY_TYPE			Get_DissolveState() { return DissolveEnable; }
	VOID				Set_DissolveState(PLAY_TYPE _PTYPE) { DissolveEnable = _PTYPE; }

	_bool			Get_Activation() { return Activation; }
	VOID			Set_Activation(_bool _Active) { Activation = _Active; }

	shared_ptr<Transform>			Get_TransformComponent() { return Component_Transform; }
	shared_ptr<Animator>			Get_AnimatorComponent() { return Component_Animator; }
	shared_ptr<Shader>				Get_ShaderComponent() { return Component_Shader; }
	shared_ptr<MeshLoader>			Get_ModelComponent() { return Component_Model; }
	shared_ptr<Collider>			Get_ColliderComponent() { return Component_Collider; }
	shared_ptr<NavMeshAgent>		Get_NavMeshAgentComponent() { return Component_NavMeshAgent; }

public:
	static	unique_ptr<Monster_FinalBoss>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>			Clone(VOID* _ARG);

private:
	shared_ptr<MeshLoader>		Component_Model;
	shared_ptr<Transform>		Component_Transform;
	shared_ptr<Animator>		Component_Animator;
	shared_ptr<Collider>		Component_Collider;
	shared_ptr<Shader>			Component_Shader;
	shared_ptr<NavMeshAgent>	Component_NavMeshAgent;

	shared_ptr<Bone>					ModelRootBone = { nullptr };
	shared_ptr<BehaviorTree_FinalBoss>	BehaviorTree = { nullptr };
	shared_ptr<BTBlackBoard>			BlackBoard = { nullptr };
	shared_ptr<Player>					GamePlayer = { nullptr };

	_bool								HitCall = { false };
	shared_ptr<class DamageFontUI>		MonsterDamageFont = { nullptr };

	shared_ptr<class BossUI>			HPBar = { nullptr };
	_bool								Visualize_StateUI = { true };

	_bool								GroggyCall = { false };
	uint32_t							AnimIndex = { 0 };

	_float								MaxObjectHP			= { 0.f };
	_float								ObjectHPPercentage	= { 1.f };

	unordered_map<string, shared_ptr<BossEffect>> EffectProtoList;

	PLAY_TYPE							DissolveEnable = { PLAY_TYPE::ONREADY };
	_float								DissolveSpeed  = { 1.f };
	_float								DissolveAmount = { 1.f };
	_bool								DissolvePlayInverse = { true };
	_float DelayTime = { 0.f };


	ComPtr<ID3D11ShaderResourceView> NoiseTexture = { nullptr };

	_bool		Activation = { false };
};

