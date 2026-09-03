#pragma once
#include "Engine_Define.h"
#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CollisionManager {
private:
	CollisionManager(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	virtual ~CollisionManager();

public:
	HRESULT Ready_CollisionManager();
	VOID	Update_CollisionManager(CONST _float& _DT);
	VOID	Late_Update_CollisionManager(CONST _float& _DT);

#ifdef _DEBUG
	VOID	DebugRender_Begin();
	VOID	DebugRender_End();

	shared_ptr<PrimitiveBatch<VertexPositionColor>>	Get_PrimitiveBatch() { return BATCH; }
#endif
public:
	VOID	Register_Collider(shared_ptr<Collider> _Collider) { ColliderList.push_back(_Collider); }
	VOID	UnRegister_Collider(shared_ptr<Collider> _Collider);

	VOID	Reset_Recipient();
public:
	static	unique_ptr<CollisionManager>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);

private:
	ComPtr<ID3D11Device>					GRPDEV = { nullptr };
	ComPtr<ID3D11DeviceContext>				DEVCTX = { nullptr };

	vector<shared_ptr<Collider>>						ColliderList;

	shared_ptr<PrimitiveBatch<VertexPositionColor>>		BATCH	= { nullptr };
	shared_ptr<BasicEffect>								EFFECT	= { nullptr };
	shared_ptr<CommonStates>							STATE	= { nullptr };
	ComPtr<ID3D11InputLayout>							LAYOUT	= { nullptr };
};

END