#include "InstanceBuffer.h"

InstanceBuffer::InstanceBuffer(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : Buffer(_GRPDEV, _DEVCTX)	{ }
InstanceBuffer::InstanceBuffer(const InstanceBuffer& _PRT) : Buffer(_PRT)	{ }

InstanceBuffer::~InstanceBuffer()	{}

HRESULT InstanceBuffer::Initialize_ProtoType() {
	return S_OK;
}
HRESULT InstanceBuffer::Initialize(void* _ARG) {
	return S_OK;
}

HRESULT	InstanceBuffer::Create_VertexBuffer() {
	return S_OK;
}
HRESULT	InstanceBuffer::Create_IndexBuffer() {
	return S_OK;
}
HRESULT InstanceBuffer::Create_InstanceBuffer() {
	return S_OK;
}

HRESULT InstanceBuffer::Bind_Resources() {
	ID3D11Buffer* BufferList[] = {
		VertexBuffer.Get(), VertexInstanceBuffer.Get()
	};

	uint32_t VertexSizeList[] = {
		VB_VertexSize, VIB_VertexSize
	};

	uint32_t VertexOffsetList[] = {
		0, 0
	};

	DEVCTX->IASetVertexBuffers(0, VB_BufferCount, BufferList, VertexSizeList, VertexOffsetList);
	DEVCTX->IASetIndexBuffer(IndexBuffer.Get(), IB_IndexFormat, 0);
	DEVCTX->IASetPrimitiveTopology(PrimitiveType);

	return S_OK;
}

HRESULT InstanceBuffer::BindOffset_Resources(ComPtr<ID3D11Buffer> _GlobalInstanceBuffer, uint32_t _Offset) {
	ID3D11Buffer* vertexBuffers[2] = { VertexBuffer.Get(), _GlobalInstanceBuffer.Get() };

	uint32_t strides[2] = { sizeof(VTXINS), sizeof(XMFLOAT4X4) };

	uint32_t offsets[2] = {
		0,
		_Offset * sizeof(XMFLOAT4X4) 
	};

	DEVCTX->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);
	DEVCTX->IASetIndexBuffer(IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	return S_OK;
}
HRESULT InstanceBuffer::Render_Buffer(uint32_t _InstanceCount) {
	DEVCTX->DrawIndexedInstanced(VIB_IndexCount, _InstanceCount, 0, 0, 0);

	return S_OK;
}

