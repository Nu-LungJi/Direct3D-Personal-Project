#pragma once
#include "Engine_Define.h"
#include "MapObject.h"

#define	AUTO_SAVETIME_INTERVAL	1.f

BEGIN(Engine)
class ENGINE_DLL MapEditor{
private:
	MapEditor(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	~MapEditor();

public:
	HRESULT		Initialize_MapEditor();

	HRESULT		Save_WorldMap_ObjectList(const string& _Directory, uint32_t _SceneIndex, uint32_t _LayerIndex);
	HRESULT		Load_WorldMap_ObjectList(const string& _Directory, uint32_t _SceneIndex, uint32_t _LayerIndex);

	HRESULT		Load_LatestFile(const string& _Directory, string& _Filepath);

	VOID		Create_MapObject(uint32_t _SceneIndex, uint32_t _LayerIndex, const string& _CloneTag);

	list<shared_ptr<MapObject>>		Get_WorldMap_ObjectList() { return WorldMap_ObjectList; }

	static unique_ptr<MapEditor>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	
private:
	ComPtr<ID3D11Device>			GRPDEV = { nullptr };
	ComPtr<ID3D11DeviceContext>		DEVCTX = { nullptr };

	list<shared_ptr<MapObject>>		WorldMap_ObjectList;

	shared_ptr<MapObject>			Selected_MapObject;
	uint32_t						MOBJIndex;

	_bool							Enable_AutoSave;
	_float							SaveAccTime;
	HWND							hWnd;
};
END