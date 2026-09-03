#pragma once
#include "InstanceBuffer.h"

BEGIN(Engine)

class ENGINE_DLL Particle : public InstanceBuffer {
private:
	Particle(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	Particle(CONST InstanceBuffer& _PRT);
public:
	virtual ~Particle();

public:
	virtual HRESULT		Initialize_ProtoType() override;
	virtual HRESULT		Initialize(void* _ARG) override;

	virtual HRESULT		Create_VertexBuffer();
	virtual HRESULT		Create_IndexBuffer();

	virtual HRESULT		Bind_Resources() override;
	virtual HRESULT		Render_Buffer()  override;

	static	unique_ptr<Particle>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual	shared_ptr<Component>	Clone(VOID* _ARG);
};

END