#pragma once
#include "Engine_Define.h"
#include "Shader.h"
#include "Bone_Ad.h"
#include "Material_Ad.h"
#include "Buffer.h"

BEGIN(Engine)
class MeshLoader_Ad;
class ENGINE_DLL Mesh_Ad : public Buffer{
private:
	Mesh_Ad(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	virtual ~Mesh_Ad();
public:
	HRESULT	Initialize(ANIMATION_TYPE _ANIMTYPE, MeshLoader_Ad* _MeshLoader, const aiMesh* _Mesh);
	HRESULT Bind_BoneMatrix(const vector<shared_ptr<class Bone_Ad>>& BoneList, shared_ptr<class Shader> _Shader, const string& _SRVName);

	int32_t Get_MaterialIndex() { return MaterialIndex; }

	static  shared_ptr<Mesh_Ad>   Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX, ANIMATION_TYPE _ANIMTYPE, MeshLoader_Ad* _MeshLoader, const aiMesh* _Mesh);
	virtual shared_ptr<Component> Clone(void* _ARG);

private:
	HRESULT Create_Static_VertexBuffer();
	HRESULT Create_Dynamic_VertexBuffer();
	HRESULT Create_IndexBuffer();

	VOID	Ready_BoneList(VTXANIMMESH* _VertexList);

private:
	MeshLoader_Ad*				MeshLoader	= nullptr;
	const aiMesh*				Mesh		= nullptr;	

	string						RootBoneName;
	vector<uint32_t>			BoneIndexList;
	vector<_float4x4>			BoneMatrixList;
	vector<_float4x4>			BoneOffsetMatrixList;
	uint32_t					BoneCount;
	int32_t						MaterialIndex;
};

END