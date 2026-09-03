#include "Buffer.h"
#include "GameInstance.h"

Buffer::Buffer(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX)   : Component(_GRPDEV, _DEVCTX) { }
Buffer::Buffer(const Buffer& _PRT) : Component(_PRT), VertexBuffer(_PRT.VertexBuffer), IndexBuffer(_PRT.IndexBuffer),
        VB_BufferCount(_PRT.VB_BufferCount), VB_VertexCount(_PRT.VB_VertexCount), VB_VertexSize(_PRT.VB_VertexSize),
        IB_IndexFormat(_PRT.IB_IndexFormat), PrimitiveType(_PRT.PrimitiveType),
        IB_IndexCount(_PRT.IB_IndexCount), IB_IndexSize(_PRT.IB_IndexSize) { }
Buffer::~Buffer()   { }

HRESULT	Buffer::Initialize_ProtoType() {
    
   // 사각형을 기본으로 설정
   VB_BufferCount  = 1;
   
   VB_VertexCount  = 4;
   VB_VertexSize   = sizeof(VTXNORTEX);
   
   IB_IndexCount   = 6;
   IB_IndexSize    = sizeof(uint32_t);
   IB_IndexFormat  = DXGI_FORMAT_R32_UINT;
   
   PrimitiveType   = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
   
   if (FAILED(Create_VertexBuffer()))  return E_FAIL;
   if (FAILED(Create_IndexBuffer()))   return E_FAIL;

    return S_OK;
}
HRESULT	Buffer::Initialize(void* _ARG) {
    // 사각형을 기본으로 설정
    VB_BufferCount = 1;

    VB_VertexCount = 4;
    VB_VertexSize = sizeof(VTXNORTEX);

    IB_IndexCount = 6;
    IB_IndexSize = sizeof(uint32_t);
    IB_IndexFormat = DXGI_FORMAT_R32_UINT;

    PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    if (FAILED(Create_VertexBuffer()))  return E_FAIL;
    if (FAILED(Create_IndexBuffer()))   return E_FAIL;
    return S_OK;
}

HRESULT Buffer::Bind_Resources() {

    ComPtr<ID3D11Buffer> VertexBufferResources[] = { VertexBuffer };
        // 하나의 버텍스 버퍼에 모든 버텍스를 때려박아서 렌더하는 방식도 있지만,
        // 여러 버텍스 버퍼를 만들어서(분할해서) 렌더할 수도 있다.
        // IASetVertexBuffers 함수의 2번째 인자가 버퍼의 갯수, 3번째 인자가 해당 버퍼들을 모아둔 배열.
    uint32_t VertexStride[] = { VB_VertexSize };
    uint32_t VertexOffset[] = { 0 };

    DEVCTX->IASetVertexBuffers(0, VB_BufferCount, VertexBufferResources[0].GetAddressOf(), VertexStride, VertexOffset);
    DEVCTX->IASetIndexBuffer(IndexBuffer.Get(), IB_IndexFormat, 0);
    DEVCTX->IASetPrimitiveTopology(PrimitiveType);

    return S_OK;
}
HRESULT Buffer::Render_Buffer() {
    if (nullptr == DEVCTX)   return E_FAIL;

    DEVCTX->DrawIndexed(IB_IndexCount, 0, 0);

    return S_OK;
}

HRESULT		Buffer::Create_VertexBuffer() {
    D3D11_BUFFER_DESC	VBufferDesc = {};
    VBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VBufferDesc.ByteWidth = VB_VertexSize * VB_VertexCount;
    VBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VBufferDesc.StructureByteStride = VB_VertexSize;
    VBufferDesc.CPUAccessFlags = 0;
    VBufferDesc.MiscFlags = 0;

    unique_ptr<VTXNORTEX[]> VTXList = unique_ptr<VTXNORTEX[]>(new VTXNORTEX[VB_VertexCount]);
    ZeroMemory(VTXList.get(), VB_VertexSize * VB_VertexCount);

    VTXList[0].Position = _float3(-0.5f, 0.5f, 0.f);
    VTXList[0].TexCoord = _float2(0.f, 0.f);

    VTXList[1].Position = _float3(0.5f, 0.5f, 0.f);
    VTXList[1].TexCoord = _float2(1.f, 0.f);

    VTXList[2].Position = _float3(0.5f, -0.5f, 0.f);
    VTXList[2].TexCoord = _float2(1.f, 1.f);

    VTXList[3].Position = _float3(-0.5f, -0.5f, 0.f);
    VTXList[3].TexCoord = _float2(0.f, 1.f);

    D3D11_SUBRESOURCE_DATA VBufferData = {};
    VBufferData.pSysMem = VTXList.get();

    if (FAILED(GRPDEV->CreateBuffer(&VBufferDesc, &VBufferData, VertexBuffer.GetAddressOf())))   return E_FAIL;

    return S_OK;
}
HRESULT		Buffer::Create_IndexBuffer() {
    D3D11_BUFFER_DESC				IBufferDesc = {};
    IBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
    IBufferDesc.ByteWidth           = IB_IndexSize * IB_IndexCount;
    IBufferDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
    IBufferDesc.StructureByteStride = IB_IndexSize;
    IBufferDesc.CPUAccessFlags      = 0;
    IBufferDesc.MiscFlags           = 0;

    uint32_t IndexCounter = 0;
    unique_ptr<uint32_t[]> INDEX = unique_ptr<uint32_t[]>(new uint32_t[IB_IndexCount]);
    ZeroMemory(INDEX.get(), IB_IndexSize * IB_IndexCount);
    
    INDEX[0] = 0;
    INDEX[1] = 1;
    INDEX[2] = 2;

    INDEX[3] = 0;
    INDEX[4] = 2;
    INDEX[5] = 3;

    D3D11_SUBRESOURCE_DATA IBufferData = {};
    IBufferData.pSysMem = INDEX.get();

    if (FAILED(GRPDEV->CreateBuffer(&IBufferDesc, &IBufferData, IndexBuffer.GetAddressOf())))   return E_FAIL;

    return S_OK;
}

unique_ptr<Buffer>	    Buffer::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
    auto Instance = unique_ptr<Buffer>(new Buffer(_GRPDEV, _DEVCTX));
    if (FAILED(Instance->Initialize_ProtoType())){
        MSG_BOX("Cannot Create Buffer.");
        return nullptr;
    }
    return Instance;
}
shared_ptr<Component>	Buffer::Clone(VOID* _ARG) {
    auto Instance = shared_ptr<Buffer>(new Buffer(*this));
    if (FAILED(Instance->Initialize(_ARG))) {
        MSG_BOX("Cannot Clone Buffer.");
        return nullptr;
    }
    return Instance;
}