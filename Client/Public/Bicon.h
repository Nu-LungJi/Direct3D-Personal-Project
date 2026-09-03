#pragma once
#include "GameObject.h"
class Bicon : public GameObject {
private:
	Bicon(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	Bicon(CONST Bicon& _PRTOBJ);
public:
	virtual ~Bicon() = default;

public:
	virtual HRESULT		Initialize_ProtoType()			override;
	virtual HRESULT		Initialize(VOID* _ARG)			override;
	virtual VOID		Update(CONST _float& _DT)		override;
	virtual VOID		Late_Update(CONST _float& _DT)	override;
	virtual HRESULT		Render()						override;

public:
	static	unique_ptr<Bicon>		Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>	Clone(VOID* _ARG);

private:
	shared_ptr<MeshLoader>		Component_Model = { nullptr };

	shared_ptr<Transform>		Component_Transform = { nullptr };
	shared_ptr<Shader>			Component_Shader = { nullptr };
	shared_ptr<Collider>		Component_Collider = { nullptr };

	shared_ptr<Player>			GamePlayer = { nullptr };
};

