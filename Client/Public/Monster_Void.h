#pragma once
#include "GameObject.h"
#include "BehaviorTree_Void.h"
#include "HPBarUI.h"

class Monster_Void : public GameObject {
private:
	Monster_Void(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	virtual ~Monster_Void();

	virtual HRESULT			Initialize_ProtoType();
	virtual HRESULT			Initialize(VOID* _ARG);
	virtual VOID			Update(CONST _float& _DT);
	virtual	VOID			Late_Update(CONST _float& _DT);
	virtual HRESULT			Render();

	virtual VOID		On_CollisionEnter(shared_ptr<GameObject> _ColliderOwner)	override;
	virtual VOID		On_CollisionStay(shared_ptr<GameObject> _ColliderOwner)		override;
	virtual VOID		On_CollisionExit(shared_ptr<GameObject> _ColliderOwner)		override;

public:
	VOID				Generate_BehaviorTree();

	VOID				Update_StateUI(CONST _float& _DT);

	VOID				Set_HitCall(_bool _Value)			{ HitCall = _Value; }
	_bool				Get_HitCall()						{ return HitCall;	}

	VOID				Enable_Dissolve(PLAY_TYPE _Enable, _float _DissolveSpeed = 1.f) { DissolveEnable = _Enable; DissolveSpeed = _DissolveSpeed; }
	
	PLAY_TYPE			Get_DissolveState() { return DissolveEnable; }
	VOID				Set_DissolveState(PLAY_TYPE _PTYPE) { DissolveEnable = _PTYPE; }

public:
	static	unique_ptr<Monster_Void> Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>	 Clone(VOID* _ARG);

private:
	shared_ptr<MeshLoader>		Component_Model;
	shared_ptr<Transform>		Component_Transform;
	shared_ptr<Animator>		Component_Animator;
	shared_ptr<Collider>		Component_Collider;
	shared_ptr<Shader>			Component_Shader;
	shared_ptr<NavMeshAgent>	Component_NavMeshAgent;

	shared_ptr<Bone>					ModelRootBone	= { nullptr };
	shared_ptr<BehaviorTree_Void>		BehaviorTree	= { nullptr };
	shared_ptr<BTBlackBoard>			BlackBoard		= { nullptr };
	shared_ptr<Player>					GamePlayer		= { nullptr };

	_bool								Visualize_StateUI	= { false };

	_bool								HitCall				= { false };
	shared_ptr<class DamageFontUI>		MonsterDamageFont	= { nullptr };

	shared_ptr<HPBarUI>					HPBar				= { nullptr };
	_float								MaxObjectHP			= { 0.f };
	_float								ObjectHPPercentage	= { 1.f };

	PLAY_TYPE							DissolveEnable = { PLAY_TYPE::ONREADY };
	_float								DissolveSpeed = { 0.f };
	_float								DissolveAmount = { 0.f };

	ComPtr<ID3D11ShaderResourceView>	NoiseTexture = { nullptr };
};

