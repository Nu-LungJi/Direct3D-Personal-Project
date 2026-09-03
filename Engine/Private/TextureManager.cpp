#include "TextureManager.h"

TextureManager::TextureManager(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GRPDEV(_GRPDEV), DEVCTX(_DEVCTX) {}
TextureManager::~TextureManager() { Clear_ResourceList(); }

ComPtr<ID3D11ShaderResourceView> TextureManager::Load_Texture(filesystem::path& _FilePath) {
	wstring FilePath = _FilePath.wstring();
	wstring FileName = _FilePath.filename().wstring();

	filesystem::path DDSFilePath = _FilePath.replace_extension(".dds");
	wstring DFilePath = DDSFilePath.wstring();

	ComPtr<ID3D11ShaderResourceView> Resource = nullptr;

	auto iter = ResourceList.find(FileName);
	if (iter != ResourceList.end()) {
		return iter->second;
	}
	else {
		if (filesystem::exists(DDSFilePath)) {
			if (FAILED(CreateDDSTextureFromFile(GRPDEV.Get(), DFilePath.c_str(), nullptr, Resource.GetAddressOf()))) {
				MSG_BOX("Cannot Create DDS Texture File.");
				assert(0);
			}
		}
		else {
			if (FAILED(CreateWICTextureFromFile(GRPDEV.Get(), FilePath.c_str(), nullptr, Resource.GetAddressOf()))) {
				MSG_BOX("Cannot Create Texture File.");
				assert(0);
			}
		}
	}
	
	if (nullptr != Resource) {
		ResourceList[FileName] = Resource;
	}

	return Resource;
}
ComPtr<ID3D11ShaderResourceView> TextureManager::Load_Texture(const string& _FilePath) {
	filesystem::path StringFilePath = _FilePath;

	wstring FilePath = StringFilePath.wstring();
	wstring FileName = StringFilePath.filename().wstring();

	filesystem::path DDSFilePath = StringFilePath.replace_extension(".dds");
	wstring DFilePath = DDSFilePath.wstring();

	ComPtr<ID3D11ShaderResourceView> Resource = nullptr;

	auto iter = ResourceList.find(FileName);
	if (iter != ResourceList.end()) {
		return iter->second;
	}
	else {
		if (filesystem::exists(DDSFilePath)) {
			if (FAILED(CreateDDSTextureFromFile(GRPDEV.Get(), DFilePath.c_str(), nullptr, Resource.GetAddressOf()))) {
				MSG_BOX("Cannot Create DDS Texture File.");
				assert(0);
			}
		}
		else {
			if (FAILED(CreateWICTextureFromFile(GRPDEV.Get(), FilePath.c_str(), nullptr, Resource.GetAddressOf()))) {
				MSG_BOX("Cannot Create Texture File.");
				assert(0);
			}
		}
	}

	if (nullptr != Resource) {
		ResourceList[FileName] = Resource;
	}

	return Resource;
}
ComPtr<ID3D11ShaderResourceView> TextureManager::Load_Texture(const wstring& _FilePath) {
	filesystem::path StringFilePath = _FilePath;

	wstring FilePath = StringFilePath.wstring();
	wstring FileName = StringFilePath.filename().wstring();

	filesystem::path DDSFilePath = StringFilePath.replace_extension(".dds");
	wstring DFilePath = DDSFilePath.wstring();

	ComPtr<ID3D11ShaderResourceView> Resource = nullptr;

	auto iter = ResourceList.find(FileName);
	if (iter != ResourceList.end()) {
		return iter->second;
	}
	else {
		if (filesystem::exists(DDSFilePath)) {
			if (FAILED(CreateDDSTextureFromFile(GRPDEV.Get(), DFilePath.c_str(), nullptr, Resource.GetAddressOf()))) {
				MSG_BOX("Cannot Create DDS Texture File.");
				assert(0);
			}
		}
		else {
			if (FAILED(CreateWICTextureFromFile(GRPDEV.Get(), FilePath.c_str(), nullptr, Resource.GetAddressOf()))) {
				MSG_BOX("Cannot Create Texture File.");
				assert(0);
			}
		}
	}

	if (nullptr != Resource) {
		ResourceList[FileName] = Resource;
	}

	return Resource;
}

VOID TextureManager::Clear_ResourceList() {
	ResourceList.clear();
}
unique_ptr<TextureManager> TextureManager::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	return unique_ptr<TextureManager>(new TextureManager(_GRPDEV, _DEVCTX));
}