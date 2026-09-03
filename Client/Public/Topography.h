#pragma once
#include "GameObject.h"

class Topography : public GameObject {
private:
	Topography(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	Topography(CONST Topography& _PRTOBJ);
public:
	virtual ~Topography() = default;

public:
	virtual HRESULT		Initialize_ProtoType();
	virtual HRESULT		Initialize(VOID* _ARG);
	virtual VOID		Update(CONST _float& _DT);
	virtual VOID		Late_Update(CONST _float& _DT);
	virtual HRESULT		Render();

public:
	static	unique_ptr<Topography>		Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>		Clone(VOID* _ARG);

private:
	shared_ptr<Terrain>			Component_Terrain;
	shared_ptr<Transform>		Component_Transform;
	shared_ptr<Shader>			Component_Shader;
	shared_ptr<NavMeshAgent>	Component_Navigation;

	ComPtr<ID3D11ShaderResourceView> Texture = { nullptr };
};

