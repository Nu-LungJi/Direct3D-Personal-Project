#include "DynamicMesh.h"
#include "MeshLoader.h"
#include "GameInstance.h"

DynamicMesh::DynamicMesh(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : Buffer(_GRPDEV, _DEVCTX) {}
DynamicMesh::DynamicMesh(const DynamicMesh& _PRTOBJ) : Buffer(_PRTOBJ), MeshLoader(_PRTOBJ.MeshLoader), MeshName(_PRTOBJ.MeshName),
IndexList(_PRTOBJ.IndexList), TextureList(_PRTOBJ.TextureList), BoneTransformedMatrixList(_PRTOBJ.BoneTransformedMatrixList),
BoneOffsetMatrixList(_PRTOBJ.BoneOffsetMatrixList), BoneIndexList(_PRTOBJ.BoneIndexList){}
DynamicMesh::~DynamicMesh() {}

HRESULT DynamicMesh::Initialize() {

    TextureList.resize(AI_TEXTURE_TYPE_MAX);

    return S_OK;
}
HRESULT DynamicMesh::Bind_TextureResource(shared_ptr<Shader> _Shader, const string& _SRVName, aiTextureType _TexType, uint32_t _TextureIndex) {
    if (TextureList[_TexType].size() == 0 || TextureList[_TexType][_TextureIndex].TextureResource == nullptr)  return E_FAIL;
    
    return _Shader->Bind_ShaderResourceView(_SRVName, TextureList[_TexType][_TextureIndex].TextureResource);
}

HRESULT DynamicMesh::UnBind_TextureResource(shared_ptr<Shader> _Shader, const string& _SRVName) {
    return _Shader->UnBind_ShaderResourceView(_SRVName);
}

HRESULT DynamicMesh::Bind_BoneMatrix(shared_ptr<Shader> _Shader, const string& _SRVName, vector<shared_ptr<Bone>>& _ModelBoneList, XMMATRIX _WorldMatrix) {
    ZeroMemory(&BoneTransformedMatrixList.front(), sizeof(_float4x4) * BoneTransformedMatrixList.size());

    uint32_t BoneCount = 0;
    if (nullptr != MeshData) { BoneCount = MeshData->mNumBones; }
    else                     { BoneCount = static_cast<uint32_t>(BoneTransformedMatrixList.size()); }

    for (uint32_t IDX = 0; IDX < BoneCount; ++IDX) {
        XMStoreFloat4x4(&BoneTransformedMatrixList[IDX], XMMatrixMultiply(XMMatrixMultiply(XMLoadFloat4x4(&BoneOffsetMatrixList[IDX]), _ModelBoneList[BoneIndexList[IDX]]->Get_ComBinedTransform()), _WorldMatrix));
    }
    
    if (!BoneTransformedMatrixList.empty()) {
        if (FAILED(_Shader->Bind_MatrixArray(_SRVName, &BoneTransformedMatrixList.front(), BoneCount))) {
            MSG_BOX("Cannot Bind Bone Matrix");
            return E_FAIL;
        }
    }
    
    return S_OK;
}

VOID    DynamicMesh::Render_Mesh() {
    uint32_t    VertexSize = sizeof(VTXANIMMESH);
    uint32_t    Offset = 0;

    DEVCTX->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &VertexSize, &Offset);
    DEVCTX->IASetIndexBuffer(IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    DEVCTX->IASetPrimitiveTopology(PrimitiveType);
    DEVCTX->PSSetShaderResources(0, 1, TextureList[aiTextureType_DIFFUSE][0].TextureResource.GetAddressOf());
    DEVCTX->DrawIndexed(static_cast<uint32_t>(IndexList.size()), 0, 0);
}
HRESULT DynamicMesh::Decompose_MeshData(class MeshLoader* _Loader, const aiScene* _ModelData, aiMesh* _MeshData) {
    MeshLoader = _Loader;
    ModelData = _ModelData;
    MeshData = _MeshData;
    MeshName = _MeshData->mName.C_Str();

    MaxCoordVertex = { -9999999.f,-9999999.f ,-9999999.f };
    MinCoordVertex = { +9999999.f,+9999999.f ,+9999999.f };

    PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    TextureList.resize(AI_TEXTURE_TYPE_MAX);

    Decompose_VertexData();
    Decompose_IndexData();
    Decompose_TextureData();
    Decompose_BoneData();

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

HRESULT DynamicMesh::Decompose_MeshData(class MeshLoader* _Loader, const FB_DynamicMesh::DynamicMeshData* _MeshData) {
    MeshLoader = _Loader;

    IB_IndexFormat = DXGI_FORMAT_R32_UINT;
    PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    TextureList.resize(AI_TEXTURE_TYPE_MAX);

    auto FB_VertexList = _MeshData->VertexList();

    D3D11_BUFFER_DESC VertexBufferDesc = {};
    VertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    VertexBufferDesc.ByteWidth = sizeof(VTXANIMMESH) * FB_VertexList->size();
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA VDataDesc = { };
    VDataDesc.pSysMem = FB_VertexList->Data();

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
    IDataDesc.pSysMem = &IndexList.front();

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

    auto FB_BoneLocalTransformList = _MeshData->BoneLocalMatrixList();
    auto FB_BoneOffsetMatrixList = _MeshData->BoneOffsetMatrixList();
    auto FB_BoneIndexList = _MeshData->BoneIndexList();

    BoneTransformedMatrixList.resize(FB_BoneLocalTransformList->size());
    BoneOffsetMatrixList.resize(FB_BoneOffsetMatrixList->size());

    for (uint32_t IDX = 0; IDX < FB_BoneOffsetMatrixList->size(); ++IDX) {
        memcpy(&BoneOffsetMatrixList[IDX], FB_BoneOffsetMatrixList->Get(IDX), sizeof(FB_Utility::Matrix4x4));
    }
    BoneIndexList.assign(FB_BoneIndexList->begin(), FB_BoneIndexList->end());

    return S_OK;
}
HRESULT DynamicMesh::Decompose_VertexData() {
    for (uint32_t IDX = 0; IDX < MeshData->mNumVertices; ++IDX) {
        VTXANIMMESH Vertex = { MeshData->mVertices[IDX].x, MeshData->mVertices[IDX].y, MeshData->mVertices[IDX].z };

        if (MeshData->HasNormals()) {
            Vertex.Normal = { MeshData->mNormals[IDX].x, MeshData->mNormals[IDX].y, MeshData->mNormals[IDX].z };
        }
        if (MeshData->HasTangentsAndBitangents()) {
            Vertex.Tangent = { MeshData->mTangents[IDX].x, MeshData->mTangents[IDX].y, MeshData->mTangents[IDX].z };
            Vertex.BiNormal = { MeshData->mBitangents[IDX].x, MeshData->mBitangents[IDX].y, MeshData->mBitangents[IDX].z };
        }
        if (MeshData->HasTextureCoords(0)) {
            Vertex.TexCoord.x = (_float)(MeshData->mTextureCoords[0][IDX].x);
            Vertex.TexCoord.y = (_float)(MeshData->mTextureCoords[0][IDX].y);
        }
        MaxCoordVertex.x = max(Vertex.Position.x, MaxCoordVertex.x);
        MaxCoordVertex.y = max(Vertex.Position.y, MaxCoordVertex.y);
        MaxCoordVertex.z = max(Vertex.Position.z, MaxCoordVertex.z);

        MinCoordVertex.x = min(Vertex.Position.x, MinCoordVertex.x);
        MinCoordVertex.y = min(Vertex.Position.y, MinCoordVertex.y);
        MinCoordVertex.z = min(Vertex.Position.z, MinCoordVertex.z);

        VertexList.push_back(Vertex);
    }

    MeshLoader->Update_CoordVertex(MaxCoordVertex, MinCoordVertex);
    return S_OK;
}
HRESULT DynamicMesh::Decompose_IndexData() {
    for (uint32_t FaceIndex = 0; FaceIndex < MeshData->mNumFaces; ++FaceIndex) {		// Vertex의 인덱스 생성.
        aiFace Face = MeshData->mFaces[FaceIndex];

        for (uint32_t IDX = 0; IDX < Face.mNumIndices; ++IDX)
            IndexList.push_back(Face.mIndices[IDX]);
    }
    return S_OK;
}
HRESULT DynamicMesh::Decompose_TextureData() {
    uint32_t    MaterialIndex = MeshData->mMaterialIndex;
    aiMaterial* Material = ModelData->mMaterials[MaterialIndex];
    // 하나의 모델이 여러 머테리얼을 가진다면, Assimp는 머테리얼 배열을 만들어 저장하고,
    // 각각의 메쉬에게 mMaterialIndex를 지정시켜준다. 따라서, 메쉬가 가진 머테리얼 인덱스를 통해 머테리얼을 얻어올 수 있다.

    for (uint32_t TexTypeIDX = 0; TexTypeIDX < AI_TEXTURE_TYPE_MAX; ++TexTypeIDX) {
        uint32_t TextureCount = Material->GetTextureCount(static_cast<aiTextureType>(TexTypeIDX));
        if (TextureCount == 0) continue;
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
                Texture MaterialTexture = { static_cast<aiTextureType>(TexTypeIDX), TextureFilePath.string().c_str(), TextureFilePath.filename().string() };

                filesystem::path FilePath = TextureFilePath;
                ComPtr<ID3D11ShaderResourceView>	FB_TextureResource = GameInstance::GetInstance().Get_TextureManager()->Load_Texture(FilePath);

                if (nullptr == FB_TextureResource) {
                    assert(0);
                }
                MaterialTexture.TextureResource = FB_TextureResource;
                TextureList[TexTypeIDX].push_back(MaterialTexture);
            }
        }
    }

    return S_OK;
}

HRESULT DynamicMesh::Decompose_BoneData() {
    uint32_t BoneCount = MeshData->mNumBones;
    BoneTransformedMatrixList.resize(BoneCount);
    BoneOffsetMatrixList.reserve(BoneCount);

    for (uint32_t BoneIDX = 0; BoneIDX < MeshData->mNumBones; ++BoneIDX) {
        aiBone* BoneData = MeshData->mBones[BoneIDX];
        string   BoneName = BoneData->mName.C_Str();
        uint32_t BoneCount = MeshData->mNumBones;

        aiVertexWeight* BoneWeight = BoneData->mWeights;           // 본에 의해 영향을 받는 정점들의 배열
        uint32_t        BoneWeightCount = BoneData->mNumWeights;        // 위 배열의 size()

        XMFLOAT4X4 OffsetMatrix;
        memcpy(&OffsetMatrix, &BoneData->mOffsetMatrix, sizeof(XMFLOAT4X4));
        XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

        BoneOffsetMatrixList.push_back(OffsetMatrix);

        int32_t BoneIndex = MeshLoader->Get_ModelBoneIndex(BoneName);
        if (-1 == BoneIndex) return E_FAIL;
        BoneIndexList.push_back(BoneIndex);

        for (size_t WeightIndex = 0; WeightIndex < BoneWeightCount; ++WeightIndex) {
            int32_t VertexID = BoneWeight[WeightIndex].mVertexId;				// 본에 의해 영향을 받는 정점들의 ID
            _float  VertexWeight = BoneWeight[WeightIndex].mWeight;

            for (size_t BoneIndex = 0; BoneIndex < MAX_BONE_INFLUENCE; ++BoneIndex) {
                if (VertexList[VertexID].BlendIndex[BoneIndex] <= 0) {
                    VertexList[VertexID].BlendIndex[BoneIndex]  = BoneIDX;
                    VertexList[VertexID].BlendWeight[BoneIndex] = VertexWeight;
                    break;
                }
            }
        }
    }
    if (0 == BoneCount) {       // RootNode인 경우
        BoneCount = 1;
        int32_t BoneIndex = -1;
        BoneIndex = MeshLoader->Get_ModelBoneIndex(MeshName);

        if (-1 == BoneIndex) return E_FAIL;
        _float4x4 OffsetMatrix;
        XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

        BoneIndexList.push_back(BoneIndex);
        BoneOffsetMatrixList.push_back(OffsetMatrix);
        BoneTransformedMatrixList.resize(BoneIndex);
    }
    return S_OK;
}
HRESULT			DynamicMesh::Create_VertexBuffer() {
    D3D11_BUFFER_DESC VertexBufferDesc = {  };
    VertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    VertexBufferDesc.ByteWidth = static_cast<uint32_t>(sizeof(VTXANIMMESH) * VertexList.size());
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA DataDesc = {};
    DataDesc.pSysMem = &VertexList.front();

    if (FAILED(GRPDEV->CreateBuffer(&VertexBufferDesc, &DataDesc, VertexBuffer.GetAddressOf()))) {
        MSG_BOX("Cannot Create VertexBuffer");
        return E_FAIL;
    }
    return S_OK;
}
HRESULT			DynamicMesh::Create_IndexBuffer() {
    D3D11_BUFFER_DESC IndexBufferDesc = { };
    IndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    IndexBufferDesc.ByteWidth = static_cast<uint32_t>(sizeof(uint32_t) * IndexList.size());
    IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IndexBufferDesc.CPUAccessFlags = 0;
    IndexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA DataDesc = {};
    DataDesc.pSysMem = &IndexList.front();

    if (FAILED(GRPDEV->CreateBuffer(&IndexBufferDesc, &DataDesc, IndexBuffer.GetAddressOf()))) {
        MSG_BOX("Cannot Create IndexBuffer");
        return E_FAIL;
    }
    return S_OK;
}

flatbuffers::Offset<FB_DynamicMesh::DynamicMeshData>	DynamicMesh::Compose_BinaryData(flatbuffers::FlatBufferBuilder& _Builder) {
    vector<flatbuffers::Offset<FB_DynamicMesh::DynamicMeshTexture>>	FB_TextureVector;

    vector<FB_Utility::Matrix4x4>   FB_BoneMatrixVector;
    vector<FB_Utility::Matrix4x4>   FB_BoneOffsetMatrixVector;
    flatbuffers::Offset<flatbuffers::String>    FilePath;

    auto	FB_VertexList = _Builder.CreateVectorOfStructs(reinterpret_cast<const FB_DynamicMesh::DynamicMeshVertex*>(VertexList.data()), VertexList.size());
    auto	FB_IndexList = _Builder.CreateVector(IndexList.data(), IndexList.size());
    for (auto& TexTypeList : TextureList) {
        for (auto& Tex : TexTypeList) {

            filesystem::path TexPath = Tex.TextureFilePath;

            Tex.TextureFileName = TexPath.filename().string();
            Tex.TextureFilePath = TexPath.replace_extension(".dds").string();

            TexPath = Tex.TextureFilePath;

            auto	FB_Texture = FB_DynamicMesh::CreateDynamicMeshTexture(_Builder, Tex.TextureType, _Builder.CreateString(TexPath.string()));
            FB_TextureVector.push_back(FB_Texture);
        }
    }
    auto FB_TexTypeList = _Builder.CreateVector(FB_TextureVector.data(), FB_TextureVector.size());
    for (auto& Matrix : BoneTransformedMatrixList) {
        FB_BoneMatrixVector.push_back(Matrix);
    }
    for (auto& Matrix : BoneOffsetMatrixList) {
        FB_BoneOffsetMatrixVector.push_back(Matrix);
    }
    auto FB_BoneMatrixList = _Builder.CreateVectorOfStructs(FB_BoneMatrixVector);
    auto FB_BoneOffsetMatrixList = _Builder.CreateVectorOfStructs(FB_BoneOffsetMatrixVector);

    auto FB_BoneIndexList = _Builder.CreateVector(BoneIndexList.data(), BoneIndexList.size());

    return FB_DynamicMesh::CreateDynamicMeshData(_Builder, FB_VertexList, FB_IndexList, FB_TexTypeList,
        FB_BoneMatrixList, FB_BoneOffsetMatrixList, FB_BoneIndexList);
}

shared_ptr<DynamicMesh> DynamicMesh::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
    auto Instance = shared_ptr<DynamicMesh>(new DynamicMesh(_GRPDEV, _DEVCTX));
    if (FAILED(Instance->Initialize())) {
        MSG_BOX("Cannot Create DynamicMesh");
        return nullptr;
    }
    return Instance;
}
