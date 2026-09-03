#pragma once
#include "GameObject.h"

class SkySphere : public GameObject {
private:
	SkySphere(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	SkySphere(CONST SkySphere& _PRTOBJ);
public:
	virtual ~SkySphere() = default;

public:
	virtual HRESULT		Initialize_ProtoType() override;
	virtual HRESULT		Initialize(VOID* _ARG) ;
	virtual VOID		Update(CONST _float& _DT);
	virtual VOID		Late_Update(CONST _float& _DT);
	virtual HRESULT		Render();

public:
	static	unique_ptr<SkySphere>		Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>	Clone(VOID* _ARG);

private:
	shared_ptr<Transform>	Component_Transform  = { nullptr };
	shared_ptr<Shader>		Component_Shader	 = { nullptr };
	shared_ptr<MeshLoader>	Component_Model		 = { nullptr };
	shared_ptr<TexBuffer>	Component_Texture	 = { nullptr };

	ComPtr<ID3D11ShaderResourceView>	BackgroundTexture = { nullptr };
	ComPtr<ID3D11ShaderResourceView>	NoiseTexture  = { nullptr };

	_float TimeAccumulation = { 0.f };
};

