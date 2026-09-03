#pragma once
#include "InstanceBuffer.h"
#include "Shader.h"
#include "FB_StaticMesh_generated.h"

BEGIN(Engine)
class MeshLoader;
class AABBCollider;
class ENGINE_DLL InstanceMesh : public InstanceBuffer {
private:
	InstanceMesh(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	InstanceMesh(const InstanceMesh& _PRTOBJ);
public:
	virtual ~InstanceMesh();

public:
	virtual HRESULT	Initialize(VOID* _ARG)	override;
	virtual HRESULT	Bind_Resources()		override;
	virtual HRESULT	Render_Buffer(uint32_t _InstanceCount) override;

	VOID			Update_InstanceBuffer(const vector<XMMATRIX>& _InstancedTransform);

	HRESULT			Bind_TextureResource(shared_ptr<Shader> _Shader, const string& _SRVName, aiTextureType _TexType, uint32_t _TextureIndex);
	HRESULT			UnBind_TextureResource(shared_ptr<Shader> _Shader, const string& _SRVName);

public:
	flatbuffers::Offset<FB_StaticMesh::StaticMeshData>	Compose_BinaryData(flatbuffers::FlatBufferBuilder& _Builder);

	HRESULT			Decompose_MeshData(class MeshLoader* _Loader, const aiScene* _ModelData, aiMesh* _MeshData);
	HRESULT			Decompose_MeshData(class MeshLoader* _Loader, const FB_StaticMesh::StaticMeshData* _MeshData);

public:
	vector<Texture>				Get_TextureTypeList(aiTextureType _TexType) { return TextureList[_TexType]; }

	vector<VTXINS>*				Get_VertexList()		{ return &VertexList;		}
	vector<uint32_t>*			Get_IndexList()			{ return &IndexList;		}
	vector<vector<Texture>>*	Get_TextureList()		{ return &TextureList;		}

	shared_ptr<AABBCollider>	Get_BoundingBox()		{ return SubMeshCollider;	}

	XMVECTOR					Get_MaxCoordVertex()	{ return XMLoadFloat3(&MaxCoordVertex); }
	XMVECTOR					Get_MinCoordVertex()	{ return XMLoadFloat3(&MinCoordVertex); }

public:
	static	shared_ptr<InstanceMesh>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual	shared_ptr<Component>		Clone(VOID* _ARG) { return nullptr; }

private:
	virtual HRESULT	Create_VertexBuffer();
	virtual HRESULT	Create_IndexBuffer();
	virtual HRESULT	Create_InstanceBuffer();

	HRESULT			Decompose_VertexData();
	HRESULT			Decompose_IndexData();
	HRESULT			Decompose_TextureData();

private:
	MeshLoader* MeshLoader;
	const aiScene* ModelData;
	aiMesh* MeshData;
	string					MeshName;

	vector<VTXINS>			VertexList;
	vector<uint32_t>		IndexList;
	vector<vector<Texture>>	TextureList;

	XMFLOAT3				MaxCoordVertex;
	XMFLOAT3				MinCoordVertex;

	shared_ptr<AABBCollider> SubMeshCollider;
};
END