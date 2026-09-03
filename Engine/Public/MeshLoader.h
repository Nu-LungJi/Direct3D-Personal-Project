#pragma once
#include "Component.h"
#include "Animator.h"
#include "Shader.h"
#include "StaticMesh.h"
#include "DynamicMesh.h"
#include "InstanceMesh.h"

#include "CommonStruct_generated.h"
#include "FB_MeshLoader_generated.h"
#include "FB_Animator_generated.h"
#include "FB_Model_generated.h"

BEGIN(Engine)
class ENGINE_DLL MeshLoader : public Component {
private:
	MeshLoader(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	MeshLoader(CONST MeshLoader& _PRTOBJ);

public:
	virtual ~MeshLoader();

public:
	virtual HRESULT	Initialize_ProtoType();
	virtual HRESULT	Initialize(VOID* _ARG);

	VOID			Update_BoundingBox(XMMATRIX _ParentMatrix);

	VOID			Render_Mesh(shared_ptr<Shader> _Shader, XMMATRIX _WorldMatrix = XMMatrixIdentity(), uint32_t _ShaderPass = 0);

	HRESULT			Compose_BinaryData(filesystem::path& _SaveFilePath);
	vector<uint8_t>	Compose_EmbaddedTextures(Texture* _Tex);
	HRESULT			Decompose_BinaryData(filesystem::path& _SaveFilePath);
	HRESULT			Decompose_MeshLoaderBinaryData(const FB_MeshLoader::MeshLoader* _MeshLoader);
	HRESULT			Decompose_BoneBinaryData(const flatbuffers::Vector<::flatbuffers::Offset<FB_Bone::BoneData>>* _BoneList);

	HRESULT			Decompose_Hierarchy(aiNode* _Node);
	HRESULT			Decompose_NodeTree(aiNode* _Node, XMMATRIX _ParentMatrix);

	HRESULT			Bind_TextureResource(shared_ptr<Shader> _Shader, const string& _SRVName, aiTextureType _TexType, uint32_t _TextureIndex);
	HRESULT			Bind_BoneResource(shared_ptr<Shader> _Shader, const string& _SRVName, XMMATRIX _WorldMatrix);

	HRESULT			Decompose_MeshData(aiNode* _Node, XMMATRIX _ParentMatrix);
	HRESULT			Decompose_ModelBoneData(const aiNode* _Node, int32_t _ParentIndex);


	HRESULT			Load_FBXModel(const wstring& _FilePath, ANIMATION_TYPE _AnimationType, shared_ptr<Animator> _Animator = nullptr);

public:
	filesystem::path			Get_Directory()			{ return FBXFile_Directory; }
	filesystem::path			Get_Path()				{ return FBXFile_Path;		}
	filesystem::path			Get_Name()				{ return FBXFile_Name;		}

	vector<aiAnimation*>&		Get_AnimationList()		{ return AnimationList;		}
	ANIMATION_TYPE				Get_AnimationType()		{ return AnimationType;		}

	VOID						Update_CoordVertex(XMFLOAT3 _MAX, XMFLOAT3 _MIN);

	XMVECTOR					Get_CoordVertexMax()	{ return XMLoadFloat3(&MaxCoordVertex); }
	XMVECTOR					Get_CoordVertexMin()	{ return XMLoadFloat3(&MinCoordVertex); }

	int32_t						Get_ModelBoneIndex(const string& _BoneName);
	vector<shared_ptr<Bone>>&	Get_ModelBoneList()		{ return ModelBoneList;		}

	shared_ptr<Bone>			Find_ModelBoone(const string& _BoneName);

	shared_ptr<Bone>&			Get_ModelRootBone()		{ return ModelRootBone;		}
	shared_ptr<Bone>&			Get_ModelPelvisBone()	{ return ModelPelvisBone;	}

	vector<shared_ptr<StaticMesh>>*		Get_StaticMeshList()	{ return &StaticMeshList;   }
	vector<shared_ptr<DynamicMesh>>*	Get_DynamicMeshList()	{ return &DynamicMeshList;  }
	vector<shared_ptr<InstanceMesh>>*	Get_InstanceMeshList()  { return &InstanceMeshList; }

	VOID			Set_SubMeshRenderFlag(uint32_t _Index, _bool _Value) { RenderFlagList.size() > _Index ? RenderFlagList[_Index] = _Value : NULL; }
	_bool			Get_SubMeshRenderFlag(uint32_t _Index)				 { return RenderFlagList[_Index];	}
	vector<_bool>*	Get_RenderFlagList()								 { return &RenderFlagList; }

	BoundingBox		Get_SubMeshBoundingBox(uint32_t _Index) { return SubMeshBoundingBoxList[_Index]; }

public:
	static	unique_ptr<MeshLoader>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<Component>	Clone(VOID* _ARG);

	shared_ptr<Animator>&		Get_Animator() { return Component_Animator; }

private:
	HRESULT						Create_AnimaionList();

	HRESULT						Check_BinaryExist(filesystem::path& _FilePath);

private:
	vector<shared_ptr<StaticMesh>>	 StaticMeshList;
	vector<shared_ptr<DynamicMesh>>  DynamicMeshList;
	vector<shared_ptr<InstanceMesh>> InstanceMeshList;

	vector<shared_ptr<Bone>>		ModelBoneList;

	const aiScene*					Scene;
	Assimp::Importer				Importer;

	shared_ptr<Animator>			Component_Animator;
	vector<aiAnimation*>			AnimationList;
	ANIMATION_TYPE					AnimationType;

	XMFLOAT3						MaxCoordVertex, MinCoordVertex;

	filesystem::path				FBXFile_Directory;
	filesystem::path				FBXFile_Path;
	filesystem::path				FBXFile_Name;

	_bool							ReferencedByBinary = false;

	shared_ptr<Bone>				ModelRootBone	= { nullptr };
	shared_ptr<Bone>				ModelPelvisBone = { nullptr };

	vector<_bool>					RenderFlagList;
	vector<BoundingBox>				SubMeshBoundingBoxList;
};

END