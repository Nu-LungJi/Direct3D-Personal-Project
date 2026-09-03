#pragma once
#include "GameObject.h"

class Portal : public GameObject {
private:
	Portal(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	Portal(CONST Portal& _PRTOBJ);
public:
	virtual ~Portal() = default;

public:
	virtual HRESULT		Initialize_ProtoType()			override;
	virtual HRESULT		Initialize(VOID* _ARG)			override;
	virtual VOID		Update(CONST _float& _DT)		override;
	virtual VOID		Late_Update(CONST _float& _DT)	override;
	virtual HRESULT		Render()						override;

	VOID				Activate_Portal();
	VOID				Enter_BossDungeon();

public:
	static	unique_ptr<Portal>		Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>	Clone(VOID* _ARG);

private:
	shared_ptr<MeshLoader>		Component_Model		= { nullptr };
	shared_ptr<Transform>		Component_Transform = { nullptr };
	shared_ptr<Shader>			Component_Shader	= { nullptr };
	shared_ptr<Collider>		Component_Collider	= { nullptr };
	shared_ptr<Shader>			Component_Portal_Shader = { nullptr };
	shared_ptr<Player>			GamePlayer = { nullptr };

	_float DeltaTime = { 0.f };
	_float PortalAlpha = { 0.f };
	_float PortalActivatable = { false }, PortalInteractable = { false };
	ComPtr<ID3D11ShaderResourceView> SRV;
};