#include "Texture.h"

TexBuffer::TexBuffer(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : Buffer(_GRPDEV, _DEVCTX){}
TexBuffer::TexBuffer(CONST TexBuffer& _PRTOBJ) : Buffer(_PRTOBJ), TextureList(_PRTOBJ.TextureList), TextureCount(_PRTOBJ.TextureCount) {}
TexBuffer::~TexBuffer(){
    for (auto& TEX : TextureList)
        TEX.Reset();
}

HRESULT	TexBuffer::Initialize_ProtoType(){

    uint32_t BasicTextureCount = 5;

    TextureList.reserve(BasicTextureCount);

    VB_BufferCount  = 1;
    VB_VertexCount  = 4;
    VB_VertexSize   = sizeof(VTXTEX);

    IB_IndexCount   = 6;
    IB_IndexSize    = sizeof(uint16_t);
    IB_IndexFormat  = DXGI_FORMAT_R16_UINT;
    PrimitiveType   = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    if (FAILED(this->Create_VertexBuffer())) {
        MSG_BOX("Cannot Create VertexBuffer.");
        return E_FAIL;
    }
    if (FAILED(this->Create_IndexBuffer())) {
        MSG_BOX("Cannot Create IndexBuffer.");
        return E_FAIL;
    }

    return S_OK;
}
HRESULT	TexBuffer::Initialize(VOID* _ARG){

	return S_OK;
}
HRESULT TexBuffer::Create_VertexBuffer() {
    D3D11_BUFFER_DESC VBufferDesc = {};
    VBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VBufferDesc.ByteWidth = VB_VertexSize * VB_VertexCount;
    VBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VBufferDesc.StructureByteStride = VB_VertexSize;
    VBufferDesc.CPUAccessFlags = 0;
    VBufferDesc.MiscFlags = 0;

    VTXTEX* VTXList = new VTXTEX[VB_VertexCount];
    ZeroMemory(VTXList, VB_VertexSize * VB_VertexCount);

    VTXList[0].Position = _float3(-0.5f, 0.5f, 0.f);
    VTXList[0].TexCoord = _float2(0.f, 0.f);

    VTXList[1].Position = _float3(0.5f, 0.5f, 0.f);
    VTXList[1].TexCoord = _float2(1.f, 0.f);

    VTXList[2].Position = _float3(0.5f, -0.5f, 0.f);
    VTXList[2].TexCoord = _float2(1.f, 1.f);

    VTXList[3].Position = _float3(-0.5f, -0.5f, 0.f);
    VTXList[3].TexCoord = _float2(0.f, 1.f);

    D3D11_SUBRESOURCE_DATA VBufferData = {};
    VBufferData.pSysMem = VTXList;

    if (FAILED(GRPDEV->CreateBuffer(&VBufferDesc, &VBufferData, VertexBuffer.GetAddressOf())))   return E_FAIL;

    Safe_Delete_Array(VTXList);

    return S_OK;
}
HRESULT TexBuffer::Create_IndexBuffer() {
    D3D11_BUFFER_DESC IBufferDesc = {};
    IBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    IBufferDesc.ByteWidth = IB_IndexSize * IB_IndexCount;
    IBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IBufferDesc.StructureByteStride = IB_IndexSize;
    IBufferDesc.CPUAccessFlags = 0;
    IBufferDesc.MiscFlags = 0;

    uint16_t* IndexList = new uint16_t[IB_IndexCount];
    ZeroMemory(IndexList, IB_IndexSize * IB_IndexCount);

    IndexList[0] = 0;
    IndexList[1] = 1;
    IndexList[2] = 2;

    IndexList[3] = 0;
    IndexList[4] = 2;
    IndexList[5] = 3;

    D3D11_SUBRESOURCE_DATA IBufferData = {};
    IBufferData.pSysMem = IndexList;

    if (FAILED(GRPDEV->CreateBuffer(&IBufferDesc, &IBufferData, IndexBuffer.GetAddressOf())))   return E_FAIL;

    Safe_Delete_Array(IndexList);

    return S_OK;
}

HRESULT TexBuffer::Load_Texture(const wstring& _FilePath, uint32_t _AlwaysZero){
    ComPtr<ID3D11ShaderResourceView>    SRV = nullptr;

    filesystem::path DDSFilePath = _FilePath;
    DDSFilePath.replace_extension(".dds");

    if (filesystem::exists(DDSFilePath)) {
        if (FAILED(CreateDDSTextureFromFile(GRPDEV.Get(), DDSFilePath.c_str(), nullptr, SRV.GetAddressOf()))) {
            MSG_BOX("Cannot Create DDS Texture File.");
            return E_FAIL;
        }
    }
    else {
        if (FAILED(CreateWICTextureFromFile(GRPDEV.Get(), _FilePath.c_str(), nullptr, SRV.GetAddressOf()))) {
            MSG_BOX("Cannot Create Texture File.");
            return E_FAIL;
        }
    }

    TextureList.push_back(SRV);

	TextureCount = static_cast<uint32_t>(TextureList.size());

    return S_OK;
}

HRESULT TexBuffer::Load_Texture(ComPtr<ID3D11ShaderResourceView> _Resource) {
    TextureList.push_back(_Resource);
    return S_OK;
}

HRESULT TexBuffer::Bind_ShaderResource(shared_ptr<Shader> _CMPShader, const string& _SVName, uint32_t _TextureIndex) {

    if (_TextureIndex >= TextureList.size())   return E_FAIL;

    return _CMPShader->Bind_ShaderResourceView(_SVName, TextureList[_TextureIndex]);
}

HRESULT TexBuffer::Get_TextureSize(uint32_t _TextureNumb, _float& _Width, _float& _Height) {
    ComPtr<ID3D11Resource> OriginalResource = nullptr;
    TextureList[_TextureNumb]->GetResource(OriginalResource.GetAddressOf());

    ComPtr<ID3D11Texture2D> TextureResource = nullptr;
    if (FAILED(OriginalResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)TextureResource.GetAddressOf()))) {
        MSG_BOX("Cannot Get TextureSize");
        return E_FAIL;
    }
    D3D11_TEXTURE2D_DESC TEXDESC = {};
    TextureResource->GetDesc(&TEXDESC);

    _Width  = static_cast<_float>(TEXDESC.Width);
	_Height = static_cast<_float>(TEXDESC.Height);

    TextureResource.Reset();
    OriginalResource.Reset();

    return S_OK;
}

ComPtr<ID3D11ShaderResourceView> TexBuffer::Get_Texture(uint32_t _TexIndex) {
    if (_TexIndex >= TextureList.size()) {
        MSG_BOX("Invalid TextureList Index.");
        return nullptr;
    }
    return TextureList[_TexIndex];
}

unique_ptr<TexBuffer>	TexBuffer::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
    auto Instance = unique_ptr<TexBuffer>(new TexBuffer(_GRPDEV, _DEVCTX));
    if (FAILED(Instance->Initialize_ProtoType()))
        MSG_BOX("Cannot Create TexBuffer.");
    
    return Instance;
}
shared_ptr<Component>	TexBuffer::Clone(VOID* _ARG) {
    auto Instance = shared_ptr<TexBuffer>(new TexBuffer(*this));
    if (FAILED(Instance->Initialize(_ARG)))
        MSG_BOX("Cannot Clone TexBuffer.");

    return Instance;
}