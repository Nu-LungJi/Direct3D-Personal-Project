#include "Terrain.h"
#include "GameInstance.h"

Terrain::Terrain(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : Buffer(_GRPDEV, _DEVCTX) {}
Terrain::Terrain(CONST Terrain& _PRT) : Buffer(_PRT), VTXCNTX(_PRT.VTXCNTX), VTXCNTZ(_PRT.VTXCNTZ)  {}
Terrain::~Terrain() {}

HRESULT	Terrain::Initialize_ProtoType(){
	//VTXCNTX = 2048; VTXCNTZ = 2048;
	VTXCNTX = 256; VTXCNTZ = 256;
	Create_TerrainVertices(VTXCNTX, VTXCNTZ);

	if (FAILED(Create_VertexBuffer()))  return E_FAIL;
	if (FAILED(Create_IndexBuffer()))   return E_FAIL;

	return S_OK;
}
HRESULT	Terrain::Initialize(void* _ARG){

	return S_OK;
}

HRESULT	Terrain::Create_VertexBuffer(){
	D3D11_BUFFER_DESC	VBufferDesc = {};
	VBufferDesc.Usage					= D3D11_USAGE_DEFAULT;
	VBufferDesc.ByteWidth				= VB_VertexSize * VB_VertexCount;
	VBufferDesc.BindFlags				= D3D11_BIND_VERTEX_BUFFER;
	VBufferDesc.StructureByteStride		= VB_VertexSize;
	VBufferDesc.CPUAccessFlags			= 0;
	VBufferDesc.MiscFlags				= 0;

	_float DistancePerVertex = 1.f;
	_float StartX = -(VTXCNTX * DistancePerVertex) * 0.5f;
	_float StartZ = -(VTXCNTZ * DistancePerVertex) * 0.5f;

	unique_ptr<VTXTEX[]> VTXList = unique_ptr<VTXTEX[]>(new VTXTEX[VB_VertexCount]);
	ZeroMemory(VTXList.get(), VB_VertexSize * VB_VertexCount);

	for (uint32_t ZIDX = 0; ZIDX < VTXCNTZ; ZIDX++) {
		for (uint32_t XIDX = 0; XIDX < VTXCNTX; XIDX++) {
			uint32_t UINDEX = ZIDX * VTXCNTX + XIDX;
			VTXList[UINDEX].Position = XMFLOAT3(StartX + (_float)XIDX * DistancePerVertex, 0.f, StartZ + (_float)ZIDX * DistancePerVertex);
			VTXList[UINDEX].TexCoord = XMFLOAT2(XIDX / (VTXCNTX - 1), ZIDX / (VTXCNTZ - 1));
		}
	}
	
	D3D11_SUBRESOURCE_DATA VBufferData = {};
	VBufferData.pSysMem = VTXList.get();

	if (FAILED(GRPDEV->CreateBuffer(&VBufferDesc, &VBufferData, VertexBuffer.GetAddressOf())))   return E_FAIL;

	return S_OK;
}
HRESULT	Terrain::Create_IndexBuffer(){

	D3D11_BUFFER_DESC				IBufferDesc = {};
	IBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	IBufferDesc.ByteWidth			= IB_IndexSize * IB_IndexCount;
	IBufferDesc.BindFlags			= D3D11_BIND_INDEX_BUFFER;
	IBufferDesc.StructureByteStride = IB_IndexSize;
	IBufferDesc.CPUAccessFlags		= 0;
	IBufferDesc.MiscFlags			= 0;

	uint32_t IndexCounter = 0;
	unique_ptr<uint32_t[]> INDEX = unique_ptr<uint32_t[]>(new uint32_t[IB_IndexCount]);
	ZeroMemory(INDEX.get(), IB_IndexSize * IB_IndexCount);
	for (uint32_t ZIDX = 0; ZIDX < VTXCNTZ - 1 ; ZIDX++) {
		for (uint32_t XIDX = 0; XIDX < VTXCNTX - 1 ; XIDX++) {

			uint32_t UINDEX = ZIDX * VTXCNTX + XIDX;

			INDEX[IndexCounter++] = UINDEX;
			INDEX[IndexCounter++] = UINDEX + VTXCNTX;
			INDEX[IndexCounter++] = UINDEX + VTXCNTX + 1;

			INDEX[IndexCounter++] = UINDEX;
			INDEX[IndexCounter++] = UINDEX + VTXCNTX + 1;
			INDEX[IndexCounter++] = UINDEX + 1;
		}
	}

	D3D11_SUBRESOURCE_DATA IBufferData = {};
	IBufferData.pSysMem = INDEX.get();

	if (FAILED(GRPDEV->CreateBuffer(&IBufferDesc, &IBufferData, IndexBuffer.GetAddressOf())))   return E_FAIL;

	return S_OK;
}
VOID	Terrain::Update(const _float& _DT) {
	
}
HRESULT Terrain::Render_Buffer() {
	if(FAILED(Buffer::Render_Buffer()))	return E_FAIL;

	return S_OK;
}
VOID	Terrain::Create_TerrainVertices(uint32_t _CountX, uint32_t _CountZ) {
	VB_BufferCount = 1;
	VB_VertexCount = VTXCNTX * VTXCNTZ * 2;
	VB_VertexSize = sizeof(VTXTEX);

	IB_IndexCount = (VTXCNTZ - 1) * (VTXCNTX - 1) * 6;
	IB_IndexSize = sizeof(uint32_t);
	IB_IndexFormat = DXGI_FORMAT_R32_UINT;

	PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}
unique_ptr<Terrain>		Terrain::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX){
    auto Instance = unique_ptr<Terrain>(new Terrain(_GRPDEV, _DEVCTX));
    if (FAILED(Instance->Initialize_ProtoType()))
        MSG_BOX("Cannot Create Terrain.");

    return Instance;
}
shared_ptr<Component>	Terrain::Clone(VOID* _ARG){
    auto Instance = shared_ptr<Terrain>(new Terrain(*this));
    if (FAILED(Instance->Initialize(_ARG)))
        MSG_BOX("Cannot Clone Terrain.");

    return Instance;
}