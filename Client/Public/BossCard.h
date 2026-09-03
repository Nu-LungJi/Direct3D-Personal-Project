#pragma once
#include "GameObject.h"

class BossCard : public GameObject {
private:
	BossCard(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	BossCard(CONST BossCard& _PRTOBJ);
public:
	virtual ~BossCard() = default;

	virtual HRESULT			Initialize_ProtoType()			override;
	virtual HRESULT			Initialize(VOID* _ARG)			override;
	virtual VOID			Update(CONST _float& _DT)		override;
	virtual VOID			Late_Update(CONST _float& _DT)	override;
	virtual HRESULT			Render()						override;

public:
	VOID				Enable_Dissolve(PLAY_TYPE _Enable, _float _DelayTime = 0.f, _float _DissolveSpeed = 1.f);

	PLAY_TYPE			Get_DissolveState()						{ return DissolveEnable; }
	VOID				Set_DissolveState(PLAY_TYPE _PTYPE)		{ DissolveEnable = _PTYPE; }

public:
	static	unique_ptr<BossCard>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>	Clone(VOID* _ARG)		override;

private:
	shared_ptr<MeshLoader>				Component_Model		= { nullptr };
	shared_ptr<Transform>				Component_Transform = { nullptr };
	shared_ptr<Collider>				Component_Collider	= { nullptr };
	shared_ptr<Shader>					Component_Shader	= { nullptr }; 
	
	shared_ptr<Player>					GamePlayer = { nullptr };
	shared_ptr<Monster_FinalBoss>		FinalBoss  = { nullptr };

	PLAY_TYPE							DissolveEnable	= { PLAY_TYPE::ONREADY };
	_float								DissolveSpeed	= { 1.f };
	_float								DissolveAmount	= { 0.02f };

	ComPtr<ID3D11ShaderResourceView>	NoiseTexture	= { nullptr };
	_float								EmissiveValue	= { 0.f };
};

