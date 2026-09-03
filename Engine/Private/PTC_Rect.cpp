#include "PTC_Rect.h"

PTC_Rect::PTC_Rect(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : InstanceBuffer(_GRPDEV, _DEVCTX) { }
PTC_Rect::PTC_Rect(const InstanceBuffer& _PRT) : InstanceBuffer(_PRT) { }

PTC_Rect::~PTC_Rect() { }

HRESULT PTC_Rect::Initialize_ProtoType() {

	// 사각형을 기본으로 설정
	VB_BufferCount = 2;

	VB_VertexCount = 4;
	VB_VertexSize = sizeof(VTXTEX);

	IB_IndexCount = 6;
	IB_IndexSize = sizeof(uint32_t);
	IB_IndexFormat = DXGI_FORMAT_R32_UINT;

	PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	VIB_IndexCount = IB_IndexCount;
	VIB_VertexSize = sizeof(VTXINS_PARTICLE);
	VIB_InstanceCount = 100;

	if (FAILED(Create_VertexBuffer()))		return E_FAIL;
	if (FAILED(Create_IndexBuffer()))		return E_FAIL;	
	if (FAILED(Create_InstanceBuffer()))	return E_FAIL;
	
	return S_OK;
}
HRESULT PTC_Rect::Initialize(void* _ARG) {

	return S_OK;
}
HRESULT	PTC_Rect::Create_VertexBuffer() {
	D3D11_BUFFER_DESC	VBufferDesc = {};

	VBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VBufferDesc.ByteWidth = VB_VertexSize * VB_VertexCount;
	VBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBufferDesc.StructureByteStride = VB_VertexSize;
	VBufferDesc.CPUAccessFlags = 0;
	VBufferDesc.MiscFlags = 0;

	unique_ptr<VTXTEX[]> VTXList = unique_ptr<VTXTEX[]>(new VTXTEX[VB_VertexCount]);
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
HRESULT	PTC_Rect::Create_IndexBuffer() {
	D3D11_BUFFER_DESC				IBufferDesc = {};
	IBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IBufferDesc.ByteWidth = IB_IndexSize * IB_IndexCount;
	IBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBufferDesc.StructureByteStride = IB_IndexSize;
	IBufferDesc.CPUAccessFlags = 0;
	IBufferDesc.MiscFlags = 0;

	uint16_t IndexCounter = 0;
	unique_ptr<uint16_t[]> INDEX = unique_ptr<uint16_t[]>(new uint16_t[IB_IndexCount]);
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
HRESULT	PTC_Rect::Create_InstanceBuffer() {
	D3D11_BUFFER_DESC				ISBufferDesc = {};
	ISBufferDesc.ByteWidth		= VIB_InstanceCount * VIB_VertexSize;
	ISBufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
	ISBufferDesc.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
	ISBufferDesc.StructureByteStride = VIB_VertexSize;
	ISBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ISBufferDesc.MiscFlags		= 0;

	ParticleVertexList	= shared_ptr<VTXINS_PARTICLE[]>(new VTXINS_PARTICLE[VIB_InstanceCount]);
	ParticleSpeed		= shared_ptr<_float[]>(new _float[VIB_InstanceCount]);
	ParticleLoop		= false;

	XMFLOAT3 ParticleRange = _float3(1.f, 0.2f, 1.f);

	for (size_t i = 0; i < VIB_InstanceCount; i++)
	{
		_float      ParticleScale = RANDOM(0.2f, 0.5f);
		ParticleSpeed[i] = RANDOM(1.f, 4.f);

		ParticleVertexList[i].RightVec	= _float4(ParticleScale, 0.f, 0.f, 0.f);
		ParticleVertexList[i].UpVec		= _float4(0.f, ParticleScale, 0.f, 0.f);
		ParticleVertexList[i].LookVec	= _float4(0.f, 0.f, ParticleScale, 0.f);
		ParticleVertexList[i].PosVec	= _float4(
			RANDOM((-ParticleRange.x) * 0.5f, (ParticleRange.x) * 0.5f),
			RANDOM((-ParticleRange.y) * 0.5f, (ParticleRange.y) * 0.5f),
			RANDOM((-ParticleRange.z) * 0.5f, (ParticleRange.z) * 0.5f),
			1.f);

		ParticleVertexList[i].LifeTime = _float2(RANDOM(0.3f, 0.7f), 0.f);
	}
	D3D11_SUBRESOURCE_DATA          InstanceInitialData{};
	InstanceInitialData.pSysMem = ParticleVertexList.get();

	if (FAILED(GRPDEV->CreateBuffer(&ISBufferDesc, &InstanceInitialData, &VertexInstanceBuffer)))
		return E_FAIL;

	return S_OK;
}

HRESULT PTC_Rect::Bind_Resources() {
	InstanceBuffer::Bind_Resources();
	return S_OK;
}
HRESULT PTC_Rect::Render_Buffer(uint32_t _InstanceCount) {
	if (_InstanceCount > 0) {
		InstanceBuffer::Render_Buffer(_InstanceCount);
	}
	else {
		InstanceBuffer::Render_Buffer(VIB_InstanceCount);
	}
	
	return S_OK;
}
shared_ptr<Component>	PTC_Rect::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<PTC_Rect>(new PTC_Rect(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone PTC_Rect.");
		return nullptr;
	}
	return Instance;
}
