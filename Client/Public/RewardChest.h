#pragma once
#include "GameObject.h"

class RewardChest : public GameObject {
private:
	RewardChest(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	RewardChest(CONST RewardChest& _PRTOBJ);
public:
	virtual ~RewardChest() = default;

public:
	virtual HRESULT		Initialize_ProtoType()			override;
	virtual HRESULT		Initialize(VOID* _ARG)			override;
	virtual VOID		Update(CONST _float& _DT)		override;
	virtual VOID		Late_Update(CONST _float& _DT)	override;
	virtual HRESULT		Render()						override;

	VOID	Open_RewardBox();

	VOID				Enable_Dissolve(PLAY_TYPE _Enable, _float _DelayTime = 0.f, _float _DissolveSpeed = 1.f) {
		DissolveAmount = -_DelayTime; DissolveEnable = _Enable; DissolveSpeed = _DissolveSpeed;
	}

	PLAY_TYPE			Get_DissolveState() { return DissolveEnable; }
	VOID				Set_DissolveState(PLAY_TYPE _PTYPE) { DissolveEnable = _PTYPE; }

public:
	static	unique_ptr<RewardChest>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>	Clone(VOID* _ARG);

private:
	shared_ptr<MeshLoader>		Component_ChestLid_Model = { nullptr };
	shared_ptr<MeshLoader>		Component_Container_Model = { nullptr };
	shared_ptr<Transform>		Component_Transform = { nullptr };
	shared_ptr<Shader>			Component_Shader = { nullptr };
	shared_ptr<Collider>		Component_Collider = { nullptr };

	shared_ptr<Player>			GamePlayer = { nullptr };

	XMFLOAT3					ChestScale = {}, ChestRotation = {}, ChestPosition = {};

	XMMATRIX					LidMatrix, ContainerMatrix;

	_bool						Interactable = { false };
	_float						EmissiveValue = { 0.f };

	PLAY_TYPE							DissolveEnable	= { PLAY_TYPE::ONREADY };
	_float								DissolveSpeed	= { 1.f };
	_float								DissolveAmount	= { 0.02f };

	ComPtr<ID3D11ShaderResourceView> NoiseTexture = { nullptr };
};

