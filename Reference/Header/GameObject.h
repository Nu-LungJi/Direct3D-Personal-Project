#pragma once
#include "Component.h"

BEGIN(Engine)
class GameInstance;
class Collider;
class ENGINE_DLL GameObject : public enable_shared_from_this<GameObject> {
protected:
	GameObject(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	GameObject(CONST GameObject& _PRTOBJ);
public:
	virtual ~GameObject() = default;

public:
	virtual HRESULT		Initialize_ProtoType()					{		return S_OK;	  }
	virtual HRESULT		Initialize(VOID* _ARG)					{		return S_OK;	  }
	virtual VOID		Priority_Update(CONST _float& _DT)		{						  }
	virtual VOID		Update(CONST _float& _DT)				{						  }
	virtual VOID		Late_Update(CONST _float& _DT)			{						  }
	virtual HRESULT		Render()								{		return S_OK;	  }

	virtual VOID		On_CollisionEnter(shared_ptr<GameObject> _ColliderOwner) {}
	virtual VOID		On_CollisionStay (shared_ptr<GameObject> _ColliderOwner) {}
	virtual VOID		On_CollisionExit (shared_ptr<GameObject> _ColliderOwner) {}

public:
	string				Get_ObjectTag()							{ return OBJTAG;		  }
	VOID				Set_ObjectTag(string _OBJTAG)			{ OBJTAG = _OBJTAG;		  }
																						  
	string				Get_AssetPath()							{ return AssetPath;		  }
	VOID				Set_AssetPath(const string& _AssetPath)	{ AssetPath = _AssetPath; }

	_bool				Get_DeadState()							{ return OBJDEAD;		  }
	VOID				Set_DeadState(_bool _OBJDEAD)			{ OBJDEAD = _OBJDEAD;	  }

	_float				Get_HP()								{ return OBJHP;			  }
	VOID				Set_HP(_float _OBJHP)					{ OBJHP = _OBJHP;		  }

	VOID				Set_ObjectOnAir(uint32_t _OnAir)		{ ObjectOnAir = _OnAir;		}
	uint32_t*			Get_ObjectOnAir()						{ return &ObjectOnAir;		}

	_bool				Get_RenderFlagState()				{ return RenderFlag; }
	VOID				Set_RenderFlagState(_bool _RFLAG) { RenderFlag = _RFLAG; }

	shared_ptr<Component> Find_Component(COMPONENT_TYPE _CTYPE);

public:
	template<typename T>
	shared_ptr<T> Add_Component(uint32_t _SceneIndex, COMPONENT_TYPE _CTYPE, VOID* _ARG = nullptr) {
		shared_ptr<T>	CMP = static_pointer_cast<T>(GameInstance::GetInstance().Get_ProtoManager()->Clone_ProtoType(_SceneIndex, _CTYPE, _ARG));
		ComponentList[(uint32_t)_CTYPE] = CMP;

		return CMP;
	}
	
public:
	virtual shared_ptr<GameObject>	Clone(VOID* _ARG) = 0;

protected:
	ComPtr<ID3D11Device>			GRPDEV = { nullptr };
	ComPtr<ID3D11DeviceContext>		DEVCTX = { nullptr };

	vector<shared_ptr<Component>>	ComponentList;

	_float							OBJHP		= { 0.f };
	_bool							OBJDEAD		= { false };
	uint32_t						ObjectOnAir = { 0 };
	string							AssetPath	= {};
	string							OBJTAG		= {};

	_bool							RenderFlag = { false };
};

END
