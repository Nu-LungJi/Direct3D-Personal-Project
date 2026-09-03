#include "StaticMesh.h"
#include "MeshLoader.h"
#include "AABBCollider.h"
#include "GameInstance.h"

StaticMesh::StaticMesh(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : Buffer(_GRPDEV, _DEVCTX){}
StaticMesh::StaticMesh(const StaticMesh& _PRTOBJ) : Buffer(_PRTOBJ), MeshLoader(_PRTOBJ.MeshLoader), MeshName(_PRTOBJ.MeshName),
IndexList(_PRTOBJ.IndexList), TextureList(_PRTOBJ.TextureList), SubMeshCollider(_PRTOBJ.SubMeshCollider) {}
StaticMesh::~StaticMesh() { }

HRESULT StaticMesh::Initialize() {
    TextureList.resize(AI_TEXTURE_TYPE_MAX);
	return S_OK;
}

HRESULT StaticMesh::Bind_TextureResource(shared_ptr<Shader> _Shader, const string& _SRVName, aiTextureType _TexType, uint32_t _TextureIndex) {
    if (TextureList[_TexType].size() == 0 || TextureList[_TexType][_TextureIndex].TextureResource == nullptr) return E_FAIL;
    return _Shader->Bind_ShaderResourceView(_SRVName, TextureList[_TexType][_TextureIndex].TextureResource);
}

HRESULT StaticMesh::UnBind_TextureResource(shared_ptr<Shader> _Shader, const string& _SRVName) {
    return _Shader->UnBind_ShaderResourceView(_SRVName);
}

VOID    StaticMesh::Render_Mesh() {
    uint32_t    VertexSize = sizeof(VTXMESH);
    uint32_t    Offset = 0;
    
    DEVCTX->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &VertexSize, &Offset);
    DEVCTX->IASetIndexBuffer(IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    DEVCTX->IASetPrimitiveTopology(PrimitiveType);
    DEVCTX->PSSetShaderResources(0, 1, TextureList[aiTextureType_DIFFUSE][0].TextureResource.GetAddressOf());
    DEVCTX->DrawIndexed(static_cast<uint32_t>(IndexList.size()), 0, 0);
}
VOID StaticMesh::Render_RawMesh() {
    uint32_t    VertexSize = sizeof(VTXMESH);
    uint32_t    Offset = 0;

    DEVCTX->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &VertexSize, &Offset);
    DEVCTX->IASetIndexBuffer(IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    DEVCTX->IASetPrimitiveTopology(PrimitiveType);
    DEVCTX->DrawIndexed(static_cast<uint32_t>(IndexList.size()), 0, 0);
}
HRESULT StaticMesh::Decompose_MeshData(class MeshLoader* _Loader, const aiScene* _ModelData, aiMesh* _MeshData) {
    
    MeshLoader  = _Loader;

    ModelData   = _ModelData;
    MeshData    = _MeshData;
    MeshName    = MeshData->mName.C_Str();

    IB_IndexFormat = DXGI_FORMAT_R32_UINT;
    PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    MaxCoordVertex = { -9999999.f,-9999999.f ,-9999999.f };
    MinCoordVertex = { +9999999.f,+9999999.f ,+9999999.f };

    TextureList.resize(AI_TEXTURE_TYPE_MAX);

    Decompose_VertexData();
    Decompose_IndexData();
    Decompose_TextureData();

    VB_VertexCount = static_cast<uint32_t>(VertexList.size());
    IB_IndexCount = static_cast<uint32_t>(IndexList.size());

    if (FAILED(Create_VertexBuffer())) {
        MSG_BOX("Cannot Create VertexBuffer.");
        return E_FAIL;
    }
    if (FAILED(Create_IndexBuffer())) {
        MSG_BOX("Cannot Create IndexBuffer.");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT StaticMesh::Decompose_MeshData(class MeshLoader* _Loader, const FB_StaticMesh::StaticMeshData* _MeshData) {
    MeshLoader = _Loader;

    IB_IndexFormat = DXGI_FORMAT_R32_UINT;
    PrimitiveType  = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    TextureList.resize(AI_TEXTURE_TYPE_MAX);
    
    auto FB_VertexList = _MeshData->VertexList();
    uint32_t VertexListSize = FB_VertexList->size();

    VertexList.clear();
    VertexList.resize(VertexListSize);

    for (uint32_t IDX = 0; IDX < VertexListSize; ++IDX) {
        auto Vertex = FB_VertexList->Get(IDX);
        memcpy(&VertexList[IDX].Position, &Vertex->Position(), sizeof(XMFLOAT3));
        memcpy(&VertexList[IDX].TexCoord, &Vertex->TexCoord(), sizeof(XMFLOAT2));
        memcpy(&VertexList[IDX].Normal, &Vertex->Normal(), sizeof(XMFLOAT3));
        memcpy(&VertexList[IDX].Tangent, &Vertex->Tangent(), sizeof(XMFLOAT3));
        memcpy(&VertexList[IDX].BiNormal, &Vertex->BiNormal(), sizeof(XMFLOAT3));
    }

    D3D11_BUFFER_DESC VertexBufferDesc = {};
    VertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    VertexBufferDesc.ByteWidth = static_cast<uint32_t>(sizeof(VTXMESH) * VertexList.size());//FB_VertexList->size();
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA VDataDesc = { };
    VDataDesc.pSysMem = VertexList.data();

    if (FAILED(GRPDEV->CreateBuffer(&VertexBufferDesc, &VDataDesc, VertexBuffer.GetAddressOf()))) {
        MSG_BOX("Cannot Create VertexBuffer");
        return E_FAIL;
    }

    auto FB_IndexList = _MeshData->IndexList();
    IndexList.assign(FB_IndexList->begin(), FB_IndexList->end());

    D3D11_BUFFER_DESC IndexBufferDesc = {};
    IndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    IndexBufferDesc.ByteWidth = static_cast<uint32_t>(sizeof(uint32_t) * IndexList.size());
    IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IndexBufferDesc.CPUAccessFlags = 0;
    IndexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA IDataDesc = {  };
    IDataDesc.pSysMem = IndexList.data();
    
    if (FAILED(GRPDEV->CreateBuffer(&IndexBufferDesc, &IDataDesc, IndexBuffer.GetAddressOf()))) {
        MSG_BOX("Cannot Create IndexBuffer");
        return E_FAIL;
    }

    
    auto FB_TextureList = _MeshData->TextureList();

    for (uint32_t IDX = 0; IDX < FB_TextureList->size(); IDX++) {
        filesystem::path TextureFileName, TextureType, TextureFilePath;

        auto FB_Texture = FB_TextureList->Get(IDX)->TextureFilePath()->c_str();
        filesystem::path FilePath = FB_Texture;
        ComPtr<ID3D11ShaderResourceView>	FB_TextureResource = GameInstance::GetInstance().Get_TextureManager()->Load_Texture(FilePath);

        if (nullptr == FB_TextureResource) {
            assert(0);
        }

        Texture TextureData = { static_cast<aiTextureType>(FB_TextureList->Get(IDX)->TextureType()), "", "" };
        TextureData.TextureResource = FB_TextureResource;

        TextureList[static_cast<uint32_t>(TextureData.TextureType)].push_back(TextureData);
    }
    //if (TextureList[static_cast<uint32_t>(aiTextureType_DIFFUSE)].size() <= 0) {
    //    MSG_BOX("NO_DIFFUSE_TEXTURE");
    //    assert(0);
    //}

    memcpy(&MaxCoordVertex, _MeshData->MaxCoordVertex(), sizeof(XMFLOAT3));
    memcpy(&MinCoordVertex, _MeshData->MinCoordVertex(), sizeof(XMFLOAT3));

    SubMeshCollider = AABBCollider::Create();
    SubMeshCollider->Set_ColliderBoxVolume(XMLoadFloat3(&MaxCoordVertex), XMLoadFloat3(&MinCoordVertex));
    
    MeshLoader->Update_CoordVertex(MaxCoordVertex, MinCoordVertex);
    return S_OK;
}
HRESULT StaticMesh::Decompose_VertexData() {
    for (uint32_t IDX = 0; IDX < MeshData->mNumVertices; ++IDX) {
		VTXMESH Vertex = { MeshData->mVertices[IDX].x, MeshData->mVertices[IDX].y, MeshData->mVertices[IDX].z };

        if (MeshData->HasTextureCoords(0)) {
            Vertex.TexCoord.x = (_float)(MeshData->mTextureCoords[0][IDX].x);
            Vertex.TexCoord.y = (_float)(MeshData->mTextureCoords[0][IDX].y);
        }
        if (MeshData->HasNormals()) { 
            Vertex.Normal = { MeshData->mNormals[IDX].x, MeshData->mNormals[IDX].y, MeshData->mNormals[IDX].z }; 
        }
        if (MeshData->HasTangentsAndBitangents()) {
			Vertex.Tangent = { MeshData->mTangents[IDX].x, MeshData->mTangents[IDX].y, MeshData->mTangents[IDX].z };
            Vertex.BiNormal = { MeshData->mBitangents[IDX].x, MeshData->mBitangents[IDX].y, MeshData->mBitangents[IDX].z };
        }
       
        MaxCoordVertex.x = max(Vertex.Position.x, MaxCoordVertex.x);
        MaxCoordVertex.y = max(Vertex.Position.y, MaxCoordVertex.y);
        MaxCoordVertex.z = max(Vertex.Position.z, MaxCoordVertex.z);

        MinCoordVertex.x = min(Vertex.Position.x, MinCoordVertex.x);
        MinCoordVertex.y = min(Vertex.Position.y, MinCoordVertex.y);
        MinCoordVertex.z = min(Vertex.Position.z, MinCoordVertex.z);

        VertexList.push_back(Vertex);
    }
    SubMeshCollider = AABBCollider::Create();
    SubMeshCollider->Set_ColliderBoxVolume(XMLoadFloat3(&MaxCoordVertex), XMLoadFloat3(&MinCoordVertex));
    MeshLoader->Update_CoordVertex(MaxCoordVertex, MinCoordVertex);
    return S_OK;
}
HRESULT StaticMesh::Decompose_IndexData() {
    for (uint32_t FaceIndex = 0; FaceIndex < MeshData->mNumFaces; ++FaceIndex) {		// Vertex의 인덱스 생성.
        aiFace Face = MeshData->mFaces[FaceIndex];
        for (uint32_t IDX = 0; IDX < Face.mNumIndices; ++IDX)
            IndexList.push_back(Face.mIndices[IDX]);
    }
    return S_OK;
}
HRESULT StaticMesh::Decompose_TextureData() {
    uint32_t    MaterialIndex = MeshData->mMaterialIndex;
    aiMaterial* Material = ModelData->mMaterials[MaterialIndex];       
    // 하나의 모델이 여러 머테리얼을 가진다면, Assimp는 머테리얼 배열을 만들어 저장하고,
	// 각각의 메쉬에게 mMaterialIndex를 지정시켜준다. 따라서, 메쉬가 가진 머테리얼 인덱스를 통해 머테리얼을 얻어올 수 있다.

    for (uint32_t TexTypeIDX = 0; TexTypeIDX < AI_TEXTURE_TYPE_MAX; ++TexTypeIDX) {
        uint32_t TextureCount = Material->GetTextureCount(static_cast<aiTextureType>(TexTypeIDX));
        TextureList[TexTypeIDX].reserve(TextureCount); 
        // 여러개의 텍스쳐가 매핑 될 수 있다. (블렌더의 Mix와 유사) 여러개의 텍스쳐를 블렌딩하여 사용하는 경우.

        for (uint32_t TexIDX = 0; TexIDX < TextureCount; ++TexIDX) {
            aiString FileName;
			Material->GetTexture(static_cast<aiTextureType>(TexTypeIDX), TexIDX, &FileName);
			
			_bool ExistSameTexture = false;
            for (auto& Texture : TextureList[TexTypeIDX]) {
                if (Texture.TextureFileName.c_str() == FileName.C_Str()) {
                    ExistSameTexture = true;
                    break;
                }
            }
            if (!ExistSameTexture) {
                filesystem::path Directory = "../../Resource/Asset/Texture";
                filesystem::path TextureFilePath = Directory / FileName.C_Str();
                Texture MaterialTexture = { static_cast<aiTextureType>(TexTypeIDX), TextureFilePath.string().c_str(), TextureFilePath.filename().string()};
                
                filesystem::path FilePath = TextureFilePath;
                ComPtr<ID3D11ShaderResourceView>	FB_TextureResource = GameInstance::GetInstance().Get_TextureManager()->Load_Texture(FilePath);

				MaterialTexture.TextureResource = FB_TextureResource;
				TextureList[TexTypeIDX].push_back(MaterialTexture);
            }
        }
    }

    return S_OK;
}

HRESULT			StaticMesh::Create_VertexBuffer() {

    VB_VertexSize = sizeof(VTXMESH);

    D3D11_BUFFER_DESC	VBufferDesc = {};

    VBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VBufferDesc.ByteWidth = VB_VertexSize * VB_VertexCount;
    VBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VBufferDesc.StructureByteStride = VB_VertexSize;
    VBufferDesc.CPUAccessFlags = 0;
    VBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA VBufferData = {};
    VBufferData.pSysMem = &(VertexList[0]);

    if (FAILED(GRPDEV->CreateBuffer(&VBufferDesc, &VBufferData, VertexBuffer.GetAddressOf())))   return E_FAIL;

    return S_OK;
}
HRESULT			StaticMesh::Create_IndexBuffer() {
    IB_IndexSize = sizeof(uint32_t);

    D3D11_BUFFER_DESC	IBufferDesc = {};

    IBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    IBufferDesc.ByteWidth = IB_IndexSize * IB_IndexCount;
    IBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IBufferDesc.StructureByteStride = IB_IndexSize;
    IBufferDesc.CPUAccessFlags = 0;
    IBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA IBufferData = {};
    IBufferData.pSysMem = &(IndexList[0]);

    if (FAILED(GRPDEV->CreateBuffer(&IBufferDesc, &IBufferData, IndexBuffer.GetAddressOf())))   return E_FAIL;

    return S_OK;
}


flatbuffers::Offset<FB_StaticMesh::StaticMeshData> StaticMesh::Compose_BinaryData(flatbuffers::FlatBufferBuilder& _Builder) {
    vector<flatbuffers::Offset<FB_StaticMesh::StaticMeshTexture>>	FB_TextureTypeGroup;
    vector<flatbuffers::Offset<flatbuffers::String>>    FilePath;

    for (auto& TexTypeList : TextureList) {
        for (auto& Tex : TexTypeList) {
            filesystem::path TexPath = Tex.TextureFilePath;

            Tex.TextureFileName = TexPath.filename().string();
            Tex.TextureFilePath = TexPath.replace_extension(".dds").string();

            TexPath = Tex.TextureFilePath;

            FB_TextureTypeGroup.push_back(FB_StaticMesh::CreateStaticMeshTexture(_Builder, Tex.TextureType, _Builder.CreateString(TexPath.string())));
        }
    }
    vector<FB_StaticMesh::StaticMeshVertex> FB_Vertices;
    FB_Vertices.reserve(VertexList.size());

    for (const auto& v : VertexList) {
        FB_Utility::Vector3 pos(v.Position.x, v.Position.y, v.Position.z);
        FB_Utility::Vector2 tex(v.TexCoord.x, v.TexCoord.y);
        FB_Utility::Vector3 norm(v.Normal.x, v.Normal.y, v.Normal.z);
        FB_Utility::Vector3 tang(v.Tangent.x, v.Tangent.y, v.Tangent.z);
        FB_Utility::Vector3 binorm(v.BiNormal.x, v.BiNormal.y, v.BiNormal.z);

        // 스키마 순서대로 생성자에 인자를 던져 객체를 만듭니다.
        FB_StaticMesh::StaticMeshVertex fb_v(pos, tex, norm, tang, binorm);

        FB_Vertices.push_back(fb_v);
    }

    auto    FB_TextureList  = _Builder.CreateVector(FB_TextureTypeGroup.data(), FB_TextureTypeGroup.size());
    auto	FB_VertexList   = _Builder.CreateVectorOfStructs(FB_Vertices.data(), FB_Vertices.size());
    auto	FB_IndexList    = _Builder.CreateVector(IndexList.data(), IndexList.size());

    FB_Utility::Vector3 MaxVertex = { MaxCoordVertex.x, MaxCoordVertex.y, MaxCoordVertex.z };
    FB_Utility::Vector3 MinVertex = { MinCoordVertex.x, MinCoordVertex.y, MinCoordVertex.z };

    return CreateStaticMeshData(_Builder, FB_VertexList, FB_IndexList, FB_TextureList, &MaxVertex, &MinVertex);
}

shared_ptr<StaticMesh> StaticMesh::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
    auto Instance = shared_ptr<StaticMesh>(new StaticMesh(_GRPDEV, _DEVCTX));
    if (FAILED(Instance->Initialize())) {
        MSG_BOX("Cannot Create StaticMesh");
        return nullptr;
    }
    return Instance;
}
