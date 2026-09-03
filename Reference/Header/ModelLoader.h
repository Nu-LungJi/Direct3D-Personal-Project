#pragma once
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL ModelLoader {
private:
	ModelLoader();
public:
	HRESULT	Load_FBXModel(const string& _FilePath);

	HRESULT	Initialize();

	const aiScene*	Get_AIScene() { return Scene.get(); }

	static unique_ptr<ModelLoader>		Create();

private:
	unique_ptr<const aiScene>	 Scene		= { nullptr };
	unique_ptr<Assimp::Importer> Importer	= { nullptr };
};

END