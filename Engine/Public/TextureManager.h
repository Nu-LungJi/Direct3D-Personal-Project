#pragma once
#include "Engine_Define.h"

BEGIN(Engine)
class ENGINE_DLL TextureManager {
private:
	TextureManager(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	~TextureManager();

public:
	ComPtr<ID3D11ShaderResourceView>	Load_Texture(filesystem::path& _FilePath);
	ComPtr<ID3D11ShaderResourceView>	Load_Texture(const string& _FilePath);
	ComPtr<ID3D11ShaderResourceView>	Load_Texture(const wstring& _FilePath);
	VOID	Clear_ResourceList();

	static unique_ptr<TextureManager> Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);

private:
	ComPtr<ID3D11Device>				GRPDEV = { nullptr };
	ComPtr<ID3D11DeviceContext>			DEVCTX = { nullptr };

	unordered_map<wstring, ComPtr<ID3D11ShaderResourceView>>	ResourceList;
};
END