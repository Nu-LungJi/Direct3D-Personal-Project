#include "CellBuffer.h"

CellBuffer::CellBuffer(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : Buffer(_GRPDEV, _DEVCTX) {}
CellBuffer::CellBuffer(CONST CellBuffer& _PRT) : Buffer(_PRT) {}
CellBuffer::~CellBuffer()	{}

HRESULT CellBuffer::Initialize_ProtoType(const XMFLOAT3* _CellVertexList) {
    VB_BufferCount  = 1;
    VB_VertexCount  = 3;
    VB_VertexSize   = sizeof(VTXPOS);

    IB_IndexCount   = 4;
    IB_IndexSize    = sizeof(uint16_t);
    IB_IndexFormat  = DXGI_FORMAT_R16_UINT;
    PrimitiveType   = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    if (FAILED(this->Create_VertexBuffer(_CellVertexList))) {
        MSG_BOX("Cannot Create VertexBuffer.");
        return E_FAIL;
    }
    if (FAILED(this->Create_IndexBuffer())) {
        MSG_BOX("Cannot Create IndexBuffer.");
        return E_FAIL;
    }

    return S_OK;
}
HRESULT CellBuffer::Initialize(void* _ARG) {

    return S_OK;
}
HRESULT CellBuffer::Create_VertexBuffer(const XMFLOAT3* _CellVertexList) {
    D3D11_BUFFER_DESC VBufferDesc = {};
    VBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
    VBufferDesc.ByteWidth           = VB_VertexSize * VB_VertexCount;
    VBufferDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
    VBufferDesc.StructureByteStride = VB_VertexSize;
    VBufferDesc.CPUAccessFlags      = 0;
    VBufferDesc.MiscFlags           = 0;

    VTXPOS* VTXList = new VTXPOS[VB_VertexCount];
    ZeroMemory(VTXList, VB_VertexSize * VB_VertexCount);

    memcpy(VTXList, _CellVertexList, sizeof(XMFLOAT3) * VB_VertexCount);

    D3D11_SUBRESOURCE_DATA VBufferData = {};
    VBufferData.pSysMem = VTXList;

    if (FAILED(GRPDEV->CreateBuffer(&VBufferDesc, &VBufferData, VertexBuffer.GetAddressOf())))   return E_FAIL;

    Safe_Delete_Array(VTXList);

    return S_OK;
}
HRESULT CellBuffer::Create_IndexBuffer() {
    D3D11_BUFFER_DESC IBufferDesc = {};
    IBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
    IBufferDesc.ByteWidth           = IB_IndexSize * IB_IndexCount;
    IBufferDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
    IBufferDesc.StructureByteStride = IB_IndexSize;
    IBufferDesc.CPUAccessFlags      = 0;
    IBufferDesc.MiscFlags           = 0;

    uint16_t* IndexList = new uint16_t[IB_IndexCount];
    ZeroMemory(IndexList, IB_IndexSize * IB_IndexCount);

    IndexList[0] = 0;
    IndexList[1] = 1;
    IndexList[2] = 2;
    IndexList[3] = 0;

    D3D11_SUBRESOURCE_DATA IBufferData = {};
    IBufferData.pSysMem = IndexList;

    if (FAILED(GRPDEV->CreateBuffer(&IBufferDesc, &IBufferData, IndexBuffer.GetAddressOf())))   return E_FAIL;

    Safe_Delete_Array(IndexList);

    return S_OK;
}

unique_ptr<CellBuffer>		CellBuffer::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX, const XMFLOAT3* _CellVertexList) {
    auto Instance = unique_ptr<CellBuffer>(new CellBuffer(_GRPDEV, _DEVCTX));
    if (FAILED(Instance->Initialize_ProtoType(_CellVertexList))) {
        MSG_BOX("Cannot Create CellBuffer.");
        return nullptr;
    }
    return Instance;
}
shared_ptr<Component>	CellBuffer::Clone(VOID* _ARG) {
    auto Instance = shared_ptr<CellBuffer>(new CellBuffer(*this));
    if (FAILED(Instance->Initialize(_ARG))) {
        MSG_BOX("Cannot Clone CellBuffer.");
        return nullptr;
    }
    return Instance;
}