#pragma once
#include "GameObject.h"

class Ocean : public GameObject {
private:
	Ocean(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	Ocean(CONST Ocean& _PRTOBJ);
public:
	virtual ~Ocean() = default;

public:
	virtual HRESULT		Initialize_ProtoType();
	virtual HRESULT		Initialize(VOID* _ARG);
	virtual VOID		Update(CONST _float& _DT);
	virtual VOID		Late_Update(CONST _float& _DT);
	virtual HRESULT		Render();

	HRESULT				Generate_ReflectTexture();
	VOID				Render_ReflectionTexture(shared_ptr<Camera> _MainCamera);

public:
	static	unique_ptr<Ocean>		Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>	Clone(VOID* _ARG);

private:
	shared_ptr<Transform>		        Component_Transform;
    shared_ptr<Terrain>			        Component_Terrain;
	shared_ptr<Shader>			        Component_OceanShader;

    ComPtr<ID3D11ShaderResourceView>	ReflectionTexture   = { nullptr };
    ComPtr<ID3D11ShaderResourceView>	DepthTexture        = { nullptr };

    ComPtr<ID3D11ShaderResourceView>	ColorRampTexture	= { nullptr };
    ComPtr<ID3D11ShaderResourceView>	OceanFoamTexture	= { nullptr };
    ComPtr<ID3D11ShaderResourceView>    OceanNoiseTexture	= { nullptr };

    ComPtr<ID3D11Texture2D>				ReflectT2D			= { nullptr };
    ComPtr<ID3D11RenderTargetView>		ReflectRTV			= { nullptr };
    ComPtr<ID3D11DepthStencilView>		COMDSV				= { nullptr };

	XMFLOAT4 OceanWaveDataA = { }; 
	XMFLOAT4 OceanWaveDataB = { };

	XMMATRIX ReflectionViewMatrix = { };

    _float	 TimeAccumulation = { 0.f };

	ComPtr<ID3D11RenderTargetView> BackBufferRTV = { nullptr };
	ComPtr<ID3D11DepthStencilView> BackBufferDSV = { nullptr };
};

