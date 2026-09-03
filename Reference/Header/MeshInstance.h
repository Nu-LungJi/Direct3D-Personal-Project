#pragma once
#include "InstanceBuffer.h"

BEGIN(Engine)

class ENGINE_DLL MeshInstance : public InstanceBuffer {
private:
	MeshInstance(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	MeshInstance(CONST InstanceBuffer& _PRT);
public:
	virtual ~MeshInstance();

public:
	virtual HRESULT		Initialize_ProtoType(vector<VTXINS>* _VertexList, vector<uint32_t>* _IndexList);
	virtual HRESULT		Initialize(void* _ARG)  override;

	virtual HRESULT		Create_VertexBuffer()	override;
	virtual HRESULT		Create_IndexBuffer()	override;
	virtual HRESULT		Create_InstanceBuffer()	override;

	virtual HRESULT		Bind_Resources()		override;
	virtual HRESULT		Render_Buffer(uint32_t _InstanceCount) override;

public:
	VOID	Update_InstanceBuffer(const vector<XMMATRIX>& _InstanceTransform);

	static	unique_ptr<MeshInstance> Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX, vector<VTXINS>* _VertexList, vector<uint32_t>* _IndexList);
	virtual	shared_ptr<Component>	 Clone(VOID* _ARG) override;

private:
	vector<VTXINS>*				VertexList;
	vector<uint32_t>*				IndexList;
	D3D11_BUFFER_DESC				ISBufferDesc = {};
};

END