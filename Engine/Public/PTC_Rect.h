#pragma once
#include "InstanceBuffer.h"

BEGIN(Engine)

class ENGINE_DLL PTC_Rect : public InstanceBuffer {
private:
	PTC_Rect(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	PTC_Rect(CONST InstanceBuffer& _PRT);
public:
	virtual ~PTC_Rect();

public:
	virtual HRESULT		Initialize_ProtoType() override;
	virtual HRESULT		Initialize(void* _ARG) override;

	virtual HRESULT		Create_VertexBuffer();
	virtual HRESULT		Create_IndexBuffer();
	virtual HRESULT		Create_InstanceBuffer();

	virtual HRESULT		Bind_Resources() override;
	virtual HRESULT		Render_Buffer(uint32_t _InstanceCount)  override;

	static	unique_ptr<PTC_Rect>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual	shared_ptr<Component>	Clone(VOID* _ARG);

private:
	shared_ptr<VTXINS_PARTICLE[]>	ParticleVertexList	= { nullptr };
	shared_ptr<_float[]>			ParticleSpeed		= { nullptr };
	_bool							ParticleLoop		= { false };
};

END