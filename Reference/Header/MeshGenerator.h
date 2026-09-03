#pragma once
#include "Engine_Define.h"
#include "GameObject.h"


BEGIN(Engine)
class ENGINE_DLL MeshGenerator {
private:
	MeshGenerator(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	virtual ~MeshGenerator() = default;

public:
	HRESULT      Ready_MeshGenerator();

	HRESULT      Generate_MeshObject(const string& _ButtonLabel, LANDSCAPE _MTYPE);

	VOID      IMGUI_MeshGenerator();
	VOID      IMGUI_SelectFolder();
	VOID      IMGUI_SelectMesh(LANDSCAPE _MTYPE);

	VOID      Generator_Controller();

	VOID	  Set_LayerIndex(LANDSCAPE _MTYPE, uint32_t _Index) { LayerIndexList[(uint32_t)_MTYPE] = _Index; }

	HRESULT      Import_FBXFiles(const string& _FolderPath, LANDSCAPE _MeshType);

	static unique_ptr<MeshGenerator> Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);

private:
	ComPtr<ID3D11Device>         GRPDEV = nullptr;
	ComPtr<ID3D11DeviceContext>  DEVCTX = nullptr;

	shared_ptr<GameObject>       SelectedObject = nullptr;

	vector<string>							Resource_FolderName;
	vector<vector<filesystem::path>>		Resource_FileName;
	string									CurrentOpenFolder;

	vector<string>				 SceneListElement, LayerListElement;

	vector<uint32_t>			 LayerIndexList;

	_bool						 Activate_FolderExplorer, Activate_MeshExplorer;
};
END