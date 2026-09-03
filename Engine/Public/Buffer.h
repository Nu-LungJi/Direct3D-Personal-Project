#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL Buffer : public Component {
protected:
	Buffer(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	Buffer(CONST Buffer& _PRT);
public:
	virtual ~Buffer();

public:
	virtual HRESULT		Initialize_ProtoType();
	virtual HRESULT		Initialize(void* _ARG);

	virtual HRESULT		Create_VertexBuffer();
	virtual HRESULT		Create_IndexBuffer();

	virtual HRESULT		Bind_Resources();
	virtual HRESULT		Render_Buffer();

	static	unique_ptr<Buffer>		Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual	shared_ptr<Component>	Clone(VOID* _ARG);

protected:
	ComPtr<ID3D11Buffer>	VertexBuffer	= { nullptr };
	ComPtr<ID3D11Buffer>	IndexBuffer		= { nullptr };

	uint32_t				VB_BufferCount	= {};
	uint32_t				VB_VertexCount	= {};
	uint32_t				VB_VertexSize	= {};

	uint32_t				IB_IndexCount	= {};
	uint32_t				IB_IndexSize	= {};
	DXGI_FORMAT				IB_IndexFormat	= {};

	D3D_PRIMITIVE_TOPOLOGY	PrimitiveType	= {};
};

END