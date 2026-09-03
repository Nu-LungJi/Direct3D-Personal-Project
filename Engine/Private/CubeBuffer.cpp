#include "CubeBuffer.h"

CubeBuffer::CubeBuffer(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : Buffer(_GRPDEV, _DEVCTX) {}
CubeBuffer::CubeBuffer(CONST CubeBuffer& _PRTOBJ) : Buffer(_PRTOBJ) {}
CubeBuffer::~CubeBuffer() {}

HRESULT	CubeBuffer::Initialize_ProtoType()	{

	VB_BufferCount	= 1;
	VB_VertexCount	= 8;
	VB_VertexSize	= sizeof(VTXCUBE);

	IB_IndexCount	= 36;
	IB_IndexSize	= sizeof(uint16_t);
	IB_IndexFormat	= DXGI_FORMAT_R16_UINT;

	PrimitiveType	= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

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
HRESULT	CubeBuffer::Initialize(VOID* _ARG)	{

	return S_OK;
}

HRESULT	CubeBuffer::Create_VertexBuffer()	{
	D3D11_BUFFER_DESC	VBufferDesc = {};
	VBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	VBufferDesc.ByteWidth			= VB_VertexSize * VB_VertexCount;
	VBufferDesc.BindFlags			= D3D11_BIND_VERTEX_BUFFER;
	VBufferDesc.StructureByteStride = VB_VertexSize;
	VBufferDesc.CPUAccessFlags		= 0;
	VBufferDesc.MiscFlags			= 0;

	VTXCUBE* VTXList = new VTXCUBE[VB_VertexCount];
	ZeroMemory(VTXList, VB_VertexSize * VB_VertexCount);

	VTXList[0].Position = _float3(-0.5f, +0.5f, -0.5f);
	VTXList[0].TexCoord = VTXList[0].Position;

	VTXList[1].Position = _float3(+0.5f, +0.5f, -0.5f);
	VTXList[1].TexCoord = VTXList[1].Position;

	VTXList[2].Position = _float3(+0.5f, -0.5f, -0.5f);
	VTXList[2].TexCoord = VTXList[2].Position;

	VTXList[3].Position = _float3(-0.5f, -0.5f, -0.5f);
	VTXList[3].TexCoord = VTXList[3].Position;

	VTXList[4].Position = _float3(-0.5f, +0.5f, +0.5f);
	VTXList[4].TexCoord = VTXList[4].Position;

	VTXList[5].Position = _float3(+0.5f, +0.5f, +0.5f);
	VTXList[5].TexCoord = VTXList[5].Position;

	VTXList[6].Position = _float3(+0.5f, -0.5f, +0.5f);
	VTXList[6].TexCoord = VTXList[6].Position;

	VTXList[7].Position = _float3(-0.5f, -0.5f, +0.5f);
	VTXList[7].TexCoord = VTXList[7].Position;

	D3D11_SUBRESOURCE_DATA VBufferData = {};
	VBufferData.pSysMem = VTXList;

	if (FAILED(GRPDEV->CreateBuffer(&VBufferDesc, &VBufferData, VertexBuffer.GetAddressOf())))   return E_FAIL;

	Safe_Delete_Array(VTXList);

	return S_OK;
}
HRESULT	CubeBuffer::Create_IndexBuffer ()	{
	D3D11_BUFFER_DESC				IBufferDesc = {};
	IBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	IBufferDesc.ByteWidth			= IB_IndexSize * IB_IndexCount;
	IBufferDesc.BindFlags			= D3D11_BIND_INDEX_BUFFER;
	IBufferDesc.StructureByteStride = IB_IndexSize;
	IBufferDesc.CPUAccessFlags		= 0;
	IBufferDesc.MiscFlags			= 0;

	uint16_t* IndexList = new uint16_t[IB_IndexCount];
	ZeroMemory(IndexList, IB_IndexSize * IB_IndexCount);

	IndexList[0] = 0; IndexList[1] = 1; IndexList[2] = 2;
	IndexList[3] = 0; IndexList[4] = 2; IndexList[5] = 3;

	IndexList[6] = 5; IndexList[7] = 4; IndexList[8] = 7;
	IndexList[9] = 5; IndexList[10] = 7; IndexList[11] = 6;	// Y+

	IndexList[12] = 4; IndexList[13] = 0; IndexList[14] = 3;
	IndexList[15] = 4; IndexList[16] = 3; IndexList[17] = 7;

	IndexList[18] = 1; IndexList[19] = 5; IndexList[20] = 6;
	IndexList[21] = 1; IndexList[22] = 6; IndexList[23] = 2;

	IndexList[24] = 4; IndexList[25] = 5; IndexList[26] = 1;
	IndexList[27] = 4; IndexList[28] = 1; IndexList[29] = 0;

	IndexList[30] = 3; IndexList[31] = 2; IndexList[32] = 6;
	IndexList[33] = 3; IndexList[34] = 6; IndexList[35] = 7;


	D3D11_SUBRESOURCE_DATA IBufferData = {};
	IBufferData.pSysMem = IndexList;

	if (FAILED(GRPDEV->CreateBuffer(&IBufferDesc, &IBufferData, IndexBuffer.GetAddressOf())))   return E_FAIL;
	
	Safe_Delete_Array(IndexList);

	return S_OK;
}

unique_ptr<CubeBuffer>	CubeBuffer::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<CubeBuffer>(new CubeBuffer(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create CubeBuffer.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<Component>	CubeBuffer::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<CubeBuffer>(new CubeBuffer(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone CubeBuffer.");
		return nullptr;
	}
	return Instance;
}