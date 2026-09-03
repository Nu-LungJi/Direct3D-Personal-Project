#pragma once
#include "Buffer.h"

BEGIN(Engine)
class ENGINE_DLL CellBuffer : public Buffer {
private:
	CellBuffer(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	CellBuffer(CONST CellBuffer& _PRT);
public:
	virtual ~CellBuffer();

public:
	virtual HRESULT		Initialize_ProtoType(const XMFLOAT3* _CellVertexList);
	virtual HRESULT		Initialize(void* _ARG)		override;

	virtual HRESULT		Create_VertexBuffer(const XMFLOAT3* _CellVertexList);
	virtual HRESULT		Create_IndexBuffer()		override;

	static	unique_ptr<CellBuffer>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX, const XMFLOAT3* _CellVertexList);
	virtual	shared_ptr<Component>	Clone(VOID* _ARG);
};
END