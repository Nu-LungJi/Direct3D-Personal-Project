#pragma once
#include "GameObject.h"
#include "OBBCollider.h"

class PlayerHitBox :public GameObject {
private:
	PlayerHitBox(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	PlayerHitBox(CONST PlayerHitBox& _PRTOBJ);
public:
	virtual ~PlayerHitBox();

public:
	virtual HRESULT		Initialize_ProtoType();
	virtual HRESULT		Initialize(VOID* _ARG);
	virtual VOID		Update(CONST _float& _DT);
	virtual	VOID		Late_Update(CONST _float& _DT);
	virtual HRESULT		Render();

	virtual VOID		On_CollisionEnter(shared_ptr<GameObject> _ColliderOwner) override;
	virtual VOID		On_CollisionStay(shared_ptr<GameObject> _ColliderOwner)  override;
	virtual VOID		On_CollisionExit(shared_ptr<GameObject> _ColliderOwner)  override;

public:
	VOID				Activate_HitBox(XMVECTOR _Position, XMVECTOR _Rotation, XMVECTOR _StartScale, XMVECTOR _EndScale, _float _Duration, _float _AppearTimeOffset, _float _AfterDurationTime, _float _Damage, _float _DFTTimeOffset, FLOWTYPE _FTYPE);
	VOID				DeActivate_HitBox();

	CONST uint32_t&		Get_HitBoxDamage()		{ return HitBoxDamage;		}
	CONST _float&		Get_HitBoxDuration()	{ return HitBoxDuration;	}

	_bool				IsActivated()			{ return HitBoxActivateState;		}

	shared_ptr<GameObject> Find_DamagedObject(const string& _Tag);
public:
	static	 unique_ptr<PlayerHitBox>	Create(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual  shared_ptr<GameObject>		Clone(VOID* _ARG);
private:
	_bool							HitBoxActivateState		= { false };	// ActivateState == true (사용 중임.)
	vector<shared_ptr<GameObject>>	DamagedTargetList;
	shared_ptr<Player>				GamePlayer			= { nullptr };
	shared_ptr<OBBCollider>			HitBoxCollider		= { nullptr };

	shared_ptr<class Transform>		Component_Transform = { nullptr };
	shared_ptr<class Shader>		Component_Shader	= { nullptr };
	shared_ptr<class Collider>		Component_Collider	= { nullptr };

	uint32_t	HitBoxDamage			= { 0 };
	_float		HitBoxDuration				= { 0.f };
	_float		HitBoxMaxDuration			= { 0.f };
	XMFLOAT3	HitBoxAccScale				= { 0.f, 0.f, 0.f };
	_float		HitBoxAppearTimeOffset		= { 0.f };

	_float		HitBox_CurrentScailingTime	= { 0.f };
	_float		HitBox_MaximumScailingTime	= { 0.f };

	XMFLOAT3	HitBoxStartScale			= { 0.f, 0.f, 0.f };
	XMFLOAT3	HitBoxEndScale				= { 0.f, 0.f, 0.f };

	FLOWTYPE	HitBoxFlowType				= { FLOWTYPE::LINEAR };

	unordered_set<shared_ptr<GameObject>>	DamagedObjectList;
	_bool	OverlapInit = { false };

	_bool	DamageFontAppearCheck = { false };
	_float	DamageFontTimeOffset = { 0.f };
	_float  DamageFontTimeAcc = { 0.f };
};

