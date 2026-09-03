#pragma once
#include "Buffer.h"
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CubeBuffer : public Buffer {
private:
	CubeBuffer(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	CubeBuffer(CONST CubeBuffer& _PRTOBJ);
public:
	virtual ~CubeBuffer();

public:
	virtual HRESULT	Initialize_ProtoType();
	virtual HRESULT	Initialize(VOID* _ARG);

	virtual HRESULT	Create_VertexBuffer();
	virtual HRESULT	Create_IndexBuffer();

	static	unique_ptr<CubeBuffer>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual	shared_ptr<Component>	Clone(VOID* _ARG);
};

END