#pragma once
#include "Engine_Define.h"

BEGIN(Engine)
class GameObject;
class ENGINE_DLL Component : public enable_shared_from_this<Component>{
protected:
	Component(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	Component(CONST Component& _PRTOBJ);
public:
	virtual ~Component();

public:
	virtual HRESULT	Initialize_ProtoType() { return S_OK; };
	virtual HRESULT	Initialize(VOID* _ARG) { return S_OK; };

	virtual	VOID	Update(const _float& _DT)	{}
	virtual VOID	Render()					{}

	virtual VOID			Set_ComponentType(COMPONENT_TYPE _CTYPE) { COMTYPE = _CTYPE; }
	virtual COMPONENT_TYPE	Get_ComponentType()						 { return COMTYPE;	 }

	virtual shared_ptr<GameObject>	Get_ComponentOwner()			 { return Component_Owner; }

public:
	virtual shared_ptr<Component>	Clone(VOID* _ARG) = 0;

protected:
	ComPtr<ID3D11Device>			GRPDEV = { nullptr };
	ComPtr<ID3D11DeviceContext>		DEVCTX = { nullptr };
	COMPONENT_TYPE					COMTYPE;

	shared_ptr<GameObject>			Component_Owner;
};

END