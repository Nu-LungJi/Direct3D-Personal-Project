#pragma once
#include "Layer.h"

BEGIN(Engine)
class ENGINE_DLL Scene	{
protected:
	Scene(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	virtual ~Scene();

public:
	virtual HRESULT Initialize();
	virtual VOID	Priority_Update(CONST _float& _DT);
	virtual VOID	Update(CONST _float& _DT);
	virtual VOID	Late_Update(CONST _float& _DT);
	virtual VOID	Render();

	virtual HRESULT	Register_ProtoType();
	virtual HRESULT Initialize_SceneObject();
	virtual HRESULT	Initialize_MapObject();

	virtual shared_ptr<GameObject>	Clone(VOID* _ARG) = 0;

public:
	HRESULT	Add_ProtoType_CurrentScene(COMPONENT_TYPE _CMPTYPE, unique_ptr<Component> _CMP);
	HRESULT Add_ProtoType_CurrentScene(const string& _PRTTAG, unique_ptr<GameObject> _OBJ);

	HRESULT Add_CloneObject(uint32_t _LayerNumb, const string& _ORGOBJ, const string& _OBJTAG, void* _ARG);

	HRESULT	Register_Layer(uint32_t _MaxLayerCount);
	HRESULT	Clear_Layer();

	HRESULT Add_GameObject(uint32_t _LayerNumb, shared_ptr<GameObject> _GOBJ);

	shared_ptr<Layer>				Get_Layer(uint32_t _LayerNumb)	{ return LayerList[_LayerNumb]; }
	vector<shared_ptr<Layer>>		Get_LayerList()					{ return LayerList;				}

	shared_ptr<GameObject>			Get_GameObject(string _OBJTAG);
	list<shared_ptr<GameObject>>	Get_GameObjectList(uint32_t _LayerNumb);

protected:
	ComPtr<ID3D11Device>			GRPDEV = { nullptr };
	ComPtr<ID3D11DeviceContext>		DEVCTX = { nullptr };
	vector<shared_ptr<Layer>>		LayerList;
};
END
