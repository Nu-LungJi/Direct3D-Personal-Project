#pragma once
#include "Engine_Define.h"
#include "GameObject.h"
#include "AABBCollider.h"
#include "Camera.h"

BEGIN(Engine)
class ENGINE_DLL RayCaster {
private:
	RayCaster(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	virtual ~RayCaster() = default;

public:
	HRESULT						Register_Recipient(shared_ptr<GameObject> _GOBJ);
	HRESULT						UnRegister_Recipient(shared_ptr<GameObject> _GOBJ);

	shared_ptr<GameObject>		RayCast_Collider();
	shared_ptr<GameObject>		Compute_RayCastObject();

	shared_ptr<GameObject>		Find_Recipient(const string& _OBJTAG);

	shared_ptr<GameObject>		Get_CastedObject() { return CastedObject; }
	HRESULT						Set_CastedObject(shared_ptr<GameObject> _GOBJ);

	HRESULT						Delete_RayCastObject();
	HRESULT						Select_NextRayCastedObject();

	XMVECTOR					Get_LocalRayOrigin()		{ return LocalRayOrigin;	}
	XMVECTOR					Get_LocalRayDirection()		{ return LocalRayDirection; }

	list<shared_ptr<GameObject>>* Get_RecipientList()		{ return &RecipientList; }

	VOID						Reset_RayCasterRecipient();

	VOID						Set_ControlLock() { Control_Lock ? Control_Lock = false : Control_Lock = true; }
#ifdef _DEBUG
	VOID		Render();
#endif
	static		unique_ptr<RayCaster> Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);

private:
	ComPtr<ID3D11Device>				GRPDEV				= nullptr;
	ComPtr<ID3D11DeviceContext>			DEVCTX				= nullptr;
									 
	shared_ptr<Camera>					MainCamera			= nullptr;
	shared_ptr<GameObject>				CastedObject		= nullptr;
	shared_ptr<GameObject>				CopiedObject		= nullptr;

	ENGINE_DESC							EngineOption		= {};
									 
	XMVECTOR							WorldRayOrigin		= {};
	XMVECTOR							WorldRayDirection	= {};

	XMVECTOR							LocalRayOrigin		= {};
	XMVECTOR							LocalRayDirection	= {};

	map<_float, shared_ptr<GameObject>>	CastedObjectList;

	list<shared_ptr<GameObject>>		RecipientList;

	_bool								Render_RayCastLine = true;

	_bool								Control_Lock = { false };
};

END