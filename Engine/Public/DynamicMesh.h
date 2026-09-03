#pragma once
#include "Buffer.h"
#include "Shader.h"
#include "Bone.h"
#include "FB_DynamicMesh_generated.h"

BEGIN(Engine)
class MeshLoader;
class ENGINE_DLL DynamicMesh : public Buffer {
private:
	DynamicMesh(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	DynamicMesh(const DynamicMesh& _PRTOBJ);
public:
	virtual ~DynamicMesh();

public:
	HRESULT			Initialize();

	HRESULT			Bind_TextureResource(shared_ptr<Shader> _Shader, const string& _SRVName, aiTextureType _TexType, uint32_t _TextureIndex);
	HRESULT			UnBind_TextureResource(shared_ptr<Shader> _Shader, const string& _SRVName);

	HRESULT			Bind_BoneMatrix(shared_ptr<Shader> _Shader, const string& _SRVName, vector<shared_ptr<Bone>>& _ModelBoneList, XMMATRIX _WorldMatrix);

	VOID			Render_Mesh();

	HRESULT			Decompose_MeshData(class MeshLoader* _Loader, const aiScene* _ModelData, aiMesh* _MeshData);
	HRESULT			Decompose_MeshData(class MeshLoader* _Loader, const FB_DynamicMesh::DynamicMeshData* _MeshData);

	HRESULT			Decompose_VertexData();
	HRESULT			Decompose_IndexData();
	HRESULT			Decompose_TextureData();
	HRESULT			Decompose_BoneData();

	vector<Texture>		Get_TextureTypeList(aiTextureType _TexType) { return TextureList[_TexType]; }

	flatbuffers::Offset<FB_DynamicMesh::DynamicMeshData>		Compose_BinaryData(flatbuffers::FlatBufferBuilder& _Builder);

	static	shared_ptr<DynamicMesh>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual	shared_ptr<Component>	Clone(VOID* _ARG) { return nullptr; }

private:
	virtual HRESULT	Create_VertexBuffer();
	virtual HRESULT	Create_IndexBuffer();

private:
	MeshLoader*				MeshLoader;
	const aiScene*			ModelData;
	aiMesh*					MeshData = nullptr;
	string					MeshName;

	vector<VTXANIMMESH>		VertexList;
	vector<uint32_t>		IndexList;
	vector<vector<Texture>>	TextureList;
	//vector<Bone>			BoneList;

	vector<XMFLOAT4X4>		BoneTransformedMatrixList;
	vector<XMFLOAT4X4>		BoneOffsetMatrixList;
	vector<uint32_t>		BoneIndexList;

	XMFLOAT3				MaxCoordVertex;
	XMFLOAT3				MinCoordVertex;
};
END
