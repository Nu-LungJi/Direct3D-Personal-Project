#pragma once
#include "Engine_Define.h"
#include "Shader.h"

BEGIN(Engine)
class ENGINE_DLL Material_Ad {
private:
	Material_Ad(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	~Material_Ad();

public:
	HRESULT		Initialize(const aiMaterial* _Material, const filesystem::path& _FilePath);
	HRESULT		Bind_ShaderResourceView(shared_ptr<class Shader> _Shader, const string& _SRVName, aiTextureType _TexType, uint32_t _TexIndex);

	static shared_ptr<Material_Ad> Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX, const aiMaterial* _Material, const filesystem::path& _FilePath);

private:
	ComPtr<ID3D11Device>						GRPDEV = nullptr;
	ComPtr<ID3D11DeviceContext>					DEVCTX = nullptr;
	vector<ComPtr<ID3D11ShaderResourceView>>	TextureList[AI_TEXTURE_TYPE_MAX];
};


END