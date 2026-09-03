#include "RectBuffer.h"

RectBuffer::RectBuffer(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : Buffer(_GRPDEV, _DEVCTX) {}
RectBuffer::RectBuffer(CONST RectBuffer& _PRTOBJ) : Buffer(_PRTOBJ) {}
RectBuffer::~RectBuffer() {}

HRESULT	RectBuffer::Initialize_ProtoType() {

	VB_BufferCount = 1;
	VB_VertexCount = 4;
	VB_VertexSize = sizeof(VTXTEX);

	IB_IndexCount = 6;
	IB_IndexSize = sizeof(uint16_t);
	IB_IndexFormat = DXGI_FORMAT_R16_UINT;

	PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

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
HRESULT	RectBuffer::Initialize(VOID* _ARG) {

	return S_OK;
}

HRESULT	RectBuffer::Create_VertexBuffer() {
	D3D11_BUFFER_DESC	VBufferDesc = {};
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
HRESULT	RectBuffer::Create_IndexBuffer() {
	D3D11_BUFFER_DESC				IBufferDesc = {};
	IBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	IBufferDesc.ByteWidth			= IB_IndexSize * IB_IndexCount;
	IBufferDesc.BindFlags			= D3D11_BIND_INDEX_BUFFER;
	IBufferDesc.StructureByteStride = IB_IndexSize;
	IBufferDesc.CPUAccessFlags		= 0;
	IBufferDesc.MiscFlags			= 0;

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

unique_ptr<RectBuffer>	RectBuffer::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<RectBuffer>(new RectBuffer(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create RectBuffer.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<Component>	RectBuffer::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<RectBuffer>(new RectBuffer(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone RectBuffer.");
		return nullptr;
	}
	return Instance;
}