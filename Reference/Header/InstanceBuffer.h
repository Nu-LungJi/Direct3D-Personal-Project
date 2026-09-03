#pragma once
#include "Buffer.h"

BEGIN(Engine)

class ENGINE_DLL InstanceBuffer : public Buffer {
protected:
	InstanceBuffer(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	InstanceBuffer(CONST InstanceBuffer& _PRT);
public:
	virtual ~InstanceBuffer();

public:
	virtual HRESULT		Initialize_ProtoType() override;
	virtual HRESULT		Initialize(void* _ARG) override;

	virtual HRESULT		Create_VertexBuffer();
	virtual HRESULT		Create_IndexBuffer();
	virtual HRESULT		Create_InstanceBuffer();

	virtual HRESULT		Bind_Resources() override;
	virtual HRESULT		Render_Buffer(uint32_t _InstanceCount);
	virtual HRESULT		BindOffset_Resources(ComPtr<ID3D11Buffer> _GlobalInstanceBuffer, uint32_t _Offset);

	virtual	shared_ptr<Component>	Clone(VOID* _ARG) = 0;

protected:
	ComPtr<ID3D11Buffer>	VertexInstanceBuffer = { nullptr };
	uint32_t				VIB_VertexSize		= { 0 };
	uint32_t				VIB_IndexCount		= { 0 };
	uint32_t				VIB_InstanceCount	= { 0 };
};

END