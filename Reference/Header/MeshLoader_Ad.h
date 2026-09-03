#pragma once
#include "Component.h"
#include "Mesh_Ad.h"
#include "Material_Ad.h"
#include "Animation_Ad.h"
#include "Bone_Ad.h"

BEGIN(Engine)
class ENGINE_DLL MeshLoader_Ad : public Component {
private:
	MeshLoader_Ad(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	MeshLoader_Ad(CONST MeshLoader_Ad& _PRTOBJ);
public:
	virtual ~MeshLoader_Ad();

public:
	virtual HRESULT Initialize_ProtoType();
	virtual HRESULT Initialize(VOID* _ARG);

	VOID		Render_Mesh(uint32_t _MeshIndex);

	HRESULT		Play_Animation(_float _DT);
	HRESULT		Bind_BoneMatrix(shared_ptr<class Shader> _Shader, const string& _SRVName, uint32_t _MeshIndex);
	HRESULT		Bind_Material(shared_ptr<class Shader> _Shader, const string& _SRVName, uint32_t _MeshIndex, aiTextureType _MatType, uint32_t _TexIndex);

	HRESULT		Load_FBXModel(const wstring& _FilePath, ANIMATION_TYPE _AnimationType);

public:
	uint32_t	Get_MeshCount() { return MeshCount; }
	int32_t		Get_ModelBoneIndex(const string& _BoneName);

public:
	static	unique_ptr<MeshLoader_Ad>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<Component>		Clone(void* _ARG) override;

private:
	HRESULT		Ready_Mesh();
	HRESULT		Ready_Material();
	HRESULT		Ready_Bone(const aiNode* _Node, int32_t _ParentIndex);
	HRESULT		Ready_Animation();

private:
	const aiScene*		Scene = nullptr;	
	Assimp::Importer	Importer;

	filesystem::path	ModelFile_Directory;
	filesystem::path	ModelFile_Path;
	filesystem::path	ModelFile_Name;

	ANIMATION_TYPE		AnimationType;
private:
	vector<shared_ptr<class Mesh_Ad>>		MeshList;
	uint32_t								MeshCount;

	vector<shared_ptr<class Material_Ad>>	MaterialList;
	uint32_t								MaterialCount;

	vector<shared_ptr<class Bone_Ad>>		BoneList;
	uint32_t								BoneCount;

	vector<shared_ptr<class Animation_Ad>>	AnimationList;
	uint32_t								AnimationCount;
	_bool									AnimationLoop;
	uint32_t								AnimationFrame = {0};
};
END