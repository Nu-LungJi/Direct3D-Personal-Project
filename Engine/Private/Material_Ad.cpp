#include "Material_Ad.h"
Material_Ad::Material_Ad(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GRPDEV(_GRPDEV), DEVCTX(_DEVCTX) {}
Material_Ad::~Material_Ad() {}

HRESULT Material_Ad::Initialize(const aiMaterial* _Material, const filesystem::path& _FilePath) {
	for (uint32_t TexTypeIDX = 0; TexTypeIDX < AI_TEXTURE_TYPE_MAX; ++TexTypeIDX) {

		uint32_t TextureCount = _Material->GetTextureCount(static_cast<aiTextureType>(TexTypeIDX));
		TextureList[TexTypeIDX].reserve(TextureCount);

		for (uint32_t TexIDX = 0; TexIDX < TextureCount; ++TexIDX) {
			aiString TexturePath;
			if (FAILED(_Material->GetTexture(static_cast<aiTextureType>(TexTypeIDX), TexIDX, &TexturePath))) {
				MSG_BOX("Cannot Get Texture Path");
				return E_FAIL;
			}
			filesystem::path FullTexturePath = _FilePath.parent_path() / TexturePath.C_Str();
			ComPtr<ID3D11ShaderResourceView> SRV = nullptr;
			if (_FilePath.extension() == ".dds") {
				if(FAILED(CreateDDSTextureFromFile(GRPDEV.Get(), FullTexturePath.wstring().c_str(), nullptr, &SRV))) {
					MSG_BOX("Cannot Create Shader Resource View From File");
					return E_FAIL;
				}
			}
			else {
				if (FAILED(CreateWICTextureFromFile(GRPDEV.Get(), FullTexturePath.wstring().c_str(), nullptr, &SRV))) {
					MSG_BOX("Cannot Create Shader Resource View From File");
					return E_FAIL;
				}
			}
			TextureList[TexTypeIDX].push_back(SRV);
		}
	}
	return S_OK;
}
HRESULT Material_Ad::Bind_ShaderResourceView(shared_ptr<class Shader> _Shader, const string& _SRVName, aiTextureType _TexType, uint32_t _TexIndex) {
	return _Shader->Bind_ShaderResourceView(_SRVName, TextureList[_TexType][_TexIndex]);
}
shared_ptr<Material_Ad> Material_Ad::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX, const aiMaterial* _Material, const filesystem::path& _FilePath) {
	auto Instance = shared_ptr<Material_Ad>(new Material_Ad(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize(_Material, _FilePath))) {
		MSG_BOX("Cannot Create ModelLoader");
		return nullptr;
	}
	return Instance;
}
