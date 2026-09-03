#include "Mesh_Ad.h"

Mesh_Ad::Mesh_Ad(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : Buffer(_GRPDEV, _DEVCTX){}
Mesh_Ad::~Mesh_Ad(){ }

HRESULT Mesh_Ad::Initialize(ANIMATION_TYPE _ANIMTYPE, MeshLoader_Ad* _MeshLoader, const aiMesh* _Mesh) {
	
	RootBoneName	= _Mesh->mName.C_Str();
	Mesh = _Mesh;
	MeshLoader		= _MeshLoader;
	MaterialIndex	= _Mesh->mMaterialIndex;

	VB_BufferCount	= 1;
	VB_VertexCount	= _Mesh->mNumVertices;
	VB_VertexSize	= (_ANIMTYPE == ANIMATION_TYPE::ANIMATION) ? sizeof(VTXANIMMESH) : sizeof(VTXMESH);

	IB_IndexCount	= _Mesh->mNumFaces * 3;
	IB_IndexSize	= sizeof(uint32_t);
	IB_IndexFormat	= DXGI_FORMAT_R32_UINT;

	PrimitiveType	= D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	if		(_ANIMTYPE == ANIMATION_TYPE::ANIMATION) {
		if (FAILED(Create_Dynamic_VertexBuffer())) {
			MSG_BOX("Cannot Create Dynamic VertexBuffer");
			return E_FAIL;
		}
	}
	else if (_ANIMTYPE == ANIMATION_TYPE::NON_ANIMATION) {
		if (FAILED(Create_Static_VertexBuffer())) {
			MSG_BOX("Cannot Create Static VertexBuffer");
			return E_FAIL;
		}
	}
	if (FAILED(Create_IndexBuffer())) {
		MSG_BOX("Cannot Create IndexBuffer");
		return E_FAIL;
	}
	return S_OK;
}
HRESULT Mesh_Ad::Create_Static_VertexBuffer() {
	D3D11_BUFFER_DESC VertexBufferDesc = {  };
	VertexBufferDesc.Usage		= D3D11_USAGE_DEFAULT;
	VertexBufferDesc.ByteWidth	= static_cast<uint32_t>(sizeof(VTXMESH)) * VB_VertexCount;
	VertexBufferDesc.BindFlags	= D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.StructureByteStride = static_cast<uint32_t>(sizeof(VTXMESH));	
	VertexBufferDesc.CPUAccessFlags = 0;
	VertexBufferDesc.MiscFlags	= 0;

	VTXMESH* VertexList = new VTXMESH[VB_VertexCount];
	ZeroMemory(VertexList, sizeof(VTXMESH) * VB_VertexCount);

	for(uint32_t IDX = 0; IDX < VB_VertexCount; ++IDX) {
		memcpy(&VertexList[IDX].Position, &Mesh->mVertices[IDX], sizeof(_float3));
		memcpy(&VertexList[IDX].Normal	, &Mesh->mNormals[IDX], sizeof(_float3));
		memcpy(&VertexList[IDX].Tangent , &Mesh->mTangents[IDX], sizeof(_float3));
		memcpy(&VertexList[IDX].BiNormal, &Mesh->mBitangents[IDX], sizeof(_float3));
		memcpy(&VertexList[IDX].TexCoord, &Mesh->mTextureCoords[0][IDX], sizeof(_float2));
	}	

	D3D11_SUBRESOURCE_DATA		DataDesc = {};
	DataDesc.pSysMem = VertexList;

	if (FAILED(GRPDEV->CreateBuffer(&VertexBufferDesc, &DataDesc, VertexBuffer.GetAddressOf()))) {
		MSG_BOX("Cannot Create VertexBuffer");
		return E_FAIL;
	}

	Safe_Delete_Array(VertexList);
	return S_OK;
}
HRESULT Mesh_Ad::Create_Dynamic_VertexBuffer() {
	D3D11_BUFFER_DESC VertexBufferDesc = {  };
	VertexBufferDesc.Usage		= D3D11_USAGE_DEFAULT;
	VertexBufferDesc.ByteWidth	= static_cast<uint32_t>(sizeof(VTXANIMMESH)) * VB_VertexCount;
	VertexBufferDesc.BindFlags	= D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.StructureByteStride = static_cast<uint32_t>(sizeof(VTXANIMMESH));
	VertexBufferDesc.CPUAccessFlags = 0;
	VertexBufferDesc.MiscFlags	= 0;

	VTXANIMMESH* VertexList = new VTXANIMMESH[VB_VertexCount];
	ZeroMemory(VertexList, sizeof(VTXANIMMESH) * VB_VertexCount);

	for (uint32_t IDX = 0; IDX < VB_VertexCount; ++IDX) {
		memcpy(&VertexList[IDX].Position, &Mesh->mVertices[IDX], sizeof(_float3));
		memcpy(&VertexList[IDX].Normal, &Mesh->mNormals[IDX], sizeof(_float3));
		memcpy(&VertexList[IDX].Tangent, &Mesh->mTangents[IDX], sizeof(_float3));
		memcpy(&VertexList[IDX].BiNormal, &Mesh->mBitangents[IDX], sizeof(_float3));
		memcpy(&VertexList[IDX].TexCoord, &Mesh->mTextureCoords[0][IDX], sizeof(_float2));
	}

	Ready_BoneList(VertexList);

	D3D11_SUBRESOURCE_DATA DataDesc = {};
	DataDesc.pSysMem = VertexList;

	if (FAILED(GRPDEV->CreateBuffer(&VertexBufferDesc, &DataDesc, VertexBuffer.GetAddressOf()))) {
		MSG_BOX("Cannot Create VertexBuffer");
		return E_FAIL;
	}

	Safe_Delete_Array(VertexList);
	return S_OK;
}
HRESULT Mesh_Ad::Create_IndexBuffer() {
	D3D11_BUFFER_DESC IndexBufferDesc = { };
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.ByteWidth = static_cast<uint32_t>(sizeof(uint32_t)) * IB_IndexCount;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;
	IndexBufferDesc.StructureByteStride = static_cast<uint32_t>(sizeof(uint32_t));

	uint32_t* IndexList = new uint32_t[IB_IndexCount];
	ZeroMemory(IndexList, sizeof(uint32_t) * IB_IndexCount);

	uint32_t IndexCount = 0;

	for (uint32_t IDX = 0; IDX < Mesh->mNumFaces; IDX++) {
		IndexList[IndexCount++] = Mesh->mFaces[IDX].mIndices[0];
		IndexList[IndexCount++] = Mesh->mFaces[IDX].mIndices[1];
		IndexList[IndexCount++] = Mesh->mFaces[IDX].mIndices[2];
	}

	D3D11_SUBRESOURCE_DATA DataDesc = {};
	DataDesc.pSysMem = IndexList;

	if (FAILED(GRPDEV->CreateBuffer(&IndexBufferDesc, &DataDesc, IndexBuffer.GetAddressOf()))) {
		MSG_BOX("Cannot Create IndexBuffer");
		return E_FAIL;
	}

	Safe_Delete_Array(IndexList);
	return S_OK;
}

VOID Mesh_Ad::Ready_BoneList(VTXANIMMESH* _VertexList) {
	BoneCount = Mesh->mNumBones;
	BoneMatrixList.resize(BoneCount);
	BoneOffsetMatrixList.reserve(BoneCount);

	for (uint32_t BoneIDX = 0; BoneIDX < BoneCount; ++BoneIDX) {
		aiBone* BoneData = Mesh->mBones[BoneIDX];

		_float4x4 OffsetMatrix;
		memcpy(&OffsetMatrix, &BoneData->mOffsetMatrix, sizeof(_float4x4));

		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		BoneOffsetMatrixList.push_back(OffsetMatrix);
		
		int32_t BoneIndex = MeshLoader->Get_ModelBoneIndex(BoneData->mName.C_Str());
		if (-1 == BoneIndex) return;

		BoneIndexList.push_back(BoneIndex);

		for (uint32_t VC = 0; VC < BoneData->mNumWeights; ++VC) {
			if (0 == _VertexList[BoneData->mWeights[VC].mVertexId].BlendIndex.x) {
				_VertexList[BoneData->mWeights[VC].mVertexId].BlendIndex.x = BoneIDX;
				_VertexList[BoneData->mWeights[VC].mVertexId].BlendWeight.x = BoneData->mWeights[VC].mWeight;
			}
			else if (0 == _VertexList[BoneData->mWeights[VC].mVertexId].BlendIndex.y) {
				_VertexList[BoneData->mWeights[VC].mVertexId].BlendIndex.y = BoneIDX;
				_VertexList[BoneData->mWeights[VC].mVertexId].BlendWeight.y = BoneData->mWeights[VC].mWeight;
			}
			else if (0 == _VertexList[BoneData->mWeights[VC].mVertexId].BlendIndex.z) {
				_VertexList[BoneData->mWeights[VC].mVertexId].BlendIndex.z = BoneIDX;
				_VertexList[BoneData->mWeights[VC].mVertexId].BlendWeight.z = BoneData->mWeights[VC].mWeight;
			}
			else if (0 == _VertexList[BoneData->mWeights[VC].mVertexId].BlendIndex.w) {
				_VertexList[BoneData->mWeights[VC].mVertexId].BlendIndex.w = BoneIDX;
				_VertexList[BoneData->mWeights[VC].mVertexId].BlendWeight.w = BoneData->mWeights[VC].mWeight;
			}
		}
	}
	if (0 == BoneCount) {
		BoneCount = 1;
		int32_t BoneIndex = -1;
		BoneIndex = MeshLoader->Get_ModelBoneIndex(RootBoneName);

		if (-1 == BoneIndex) return;
		_float4x4 OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		BoneIndexList.push_back(BoneIndex);
		BoneOffsetMatrixList.push_back(OffsetMatrix);
		BoneMatrixList.resize(BoneIndex);
	}
}

HRESULT Mesh_Ad::Bind_BoneMatrix(const vector<shared_ptr<class Bone_Ad>>& BoneList, shared_ptr<Shader> _Shader, const string& _SRVName) {
	ZeroMemory(&BoneMatrixList.front(), sizeof(_float4x4) * BoneMatrixList.size());
	
	for (uint32_t IDX = 0; IDX < BoneCount; ++IDX) {
		XMStoreFloat4x4(&BoneMatrixList[IDX], XMLoadFloat4x4(&BoneOffsetMatrixList[IDX]) * BoneList[BoneIndexList[IDX]]->Get_CombinedBoneMatrix());
	}
	if (!BoneMatrixList.empty()) {
		if(FAILED(_Shader->Bind_MatrixArray(_SRVName, BoneMatrixList.data(), BoneCount))) {
			MSG_BOX("Cannot Bind Bone Matrix");
			return E_FAIL;
		}
	}
	return S_OK;
}

shared_ptr<Mesh_Ad> Mesh_Ad::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX, ANIMATION_TYPE _ANIMTYPE, class MeshLoader_Ad* _MeshLoader, const aiMesh* _Mesh) {
	auto Instance = shared_ptr<Mesh_Ad>(new Mesh_Ad(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize(_ANIMTYPE, _MeshLoader, _Mesh))) {
		MSG_BOX("Cannot Create Mesh");
		return nullptr;
	}
	return Instance;
}
shared_ptr<Component>		Mesh_Ad::Clone(void* _ARG) {
	return nullptr;
}