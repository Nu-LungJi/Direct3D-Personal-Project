#pragma once
#include "Engine_Define.h"
#include "GameObject.h"
#include "MapObject.h"

BEGIN(Engine)
struct InstanceGroup {
	InstanceGroup(shared_ptr<InstanceMesh> _Mesh, shared_ptr<Shader> _Shader, XMMATRIX _Mat)
		: REP_Buffer(_Mesh), REP_Shader(_Shader) { WorldMatrixList.push_back(_Mat);};

	shared_ptr<InstanceMesh>	REP_Buffer;
	shared_ptr<Shader>		REP_Shader;
	vector<XMMATRIX>		WorldMatrixList;
	uint32_t				BufferStartOffset = 0;
};
class ENGINE_DLL RenderManager	{
private:
	RenderManager(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	~RenderManager();

public:
	HRESULT	Ready_RenderManager();
	HRESULT Add_GameObject(RENDER_TYPE _RTYPE, shared_ptr<GameObject> _GOBJ);
	HRESULT Render_ByGroup();

	HRESULT Render_PriorityGroup();
	HRESULT Render_NonBlend();
	HRESULT Render_Light();
	HRESULT Render_Combined();
	HRESULT	Render_NonLight();
	HRESULT Render_AlphaBlend();
	HRESULT	Render_Effect();
	HRESULT Render_UserInterface();

	HRESULT Render_PostProcess();

	HRESULT Render_BrightPass(uint32_t _WindowResolutionX, uint32_t _WindowResolutionY);
	HRESULT Render_VerticalBlur(uint32_t _WindowResolutionX, uint32_t _WindowResolutionY);
	HRESULT Render_HorizontalBlur(uint32_t _WindowResolutionX, uint32_t _WindowResolutionY);

public:
	VOID	Add_InstancedObject(shared_ptr<MapObject> _MOBJ);

	HRESULT	Render_InstancedObject();

	shared_ptr<Shader>& Get_DeferredShader() { return Component_Shader; }

	VOID	Create_DepthStencilTexture();
	ComPtr<ID3D11ShaderResourceView>	Get_DepthTexture() { return DepthTexture.Get(); }
	ID3D11DepthStencilView*		Get_DepthStencilView() { return COMDSV.Get(); }
#ifdef _DEBUG
	HRESULT		Add_DebugComponent(shared_ptr<Component> _CMP);
	HRESULT		Render_RenderTargetView();
#endif

public:
	static	unique_ptr<RenderManager>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);

private:
	ComPtr<ID3D11Device>			GRPDEV = { nullptr };
	ComPtr<ID3D11DeviceContext>		DEVCTX = { nullptr };

	list<shared_ptr<GameObject>>	RenderObjectList[(uint32_t)RENDER_TYPE::RENDER_END];

	shared_ptr<class RectBuffer>	Component_RectBuffer = { nullptr };
	shared_ptr<class Shader>		Component_Shader	 = { nullptr };
	XMFLOAT4X4						WorldMatrix, ViewMatrix, ProjMatrix;

	ComPtr<ID3D11Buffer>						 GlobalInstanceBuffer = { nullptr };
	unordered_map<InstanceMesh*, InstanceGroup>	 MapObjectInstanceList;

	ComPtr<ID3D11Texture2D>			DepthT2D = { nullptr };
	ComPtr<ID3D11DepthStencilView>	COMDSV = { nullptr };
	ComPtr<ID3D11ShaderResourceView>	DepthTexture = { nullptr };

#ifdef _DEBUG
private:
	list<shared_ptr<Component>>		ComponentList;
	_bool							DeferredRenderingFlag = { false };
#endif
};
END