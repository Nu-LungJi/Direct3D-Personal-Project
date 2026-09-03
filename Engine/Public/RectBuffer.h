#pragma once
#include "Buffer.h"
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL RectBuffer : public Buffer {
private:
	RectBuffer(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	RectBuffer(CONST RectBuffer& _PRTOBJ);
public:
	virtual ~RectBuffer();

public:
	virtual HRESULT	Initialize_ProtoType();
	virtual HRESULT	Initialize(VOID* _ARG);

	virtual HRESULT	Create_VertexBuffer();
	virtual HRESULT	Create_IndexBuffer();

	static	unique_ptr<RectBuffer>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual	shared_ptr<Component>	Clone(VOID* _ARG);
};

END