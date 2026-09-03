#include "Shader.h"

Shader::Shader(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : Component(_GRPDEV, _DEVCTX) {}
Shader::Shader(CONST Shader& _PRTOBJ) : Component(_PRTOBJ), EffectObject(_PRTOBJ.EffectObject), InputLayoutList(_PRTOBJ.InputLayoutList),
	TechniquePassesCount(_PRTOBJ.TechniquePassesCount){}
Shader::~Shader()	{}

HRESULT	Shader::Initialize_ProtoType(const _tchar* _FilePath, const D3D11_INPUT_ELEMENT_DESC* _DESC, uint32_t _ElemCount) {
	uint32_t ShaderFlag = {};
#ifdef _DEBUG
	ShaderFlag |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	ShaderFlag |= D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif 
	ComPtr<ID3D10Blob> ErrorMSG = nullptr;

	HRESULT HR = D3DX11CompileEffectFromFile(_FilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, ShaderFlag, 0, GRPDEV.Get(), EffectObject.GetAddressOf(), ErrorMSG.GetAddressOf());
	if (FAILED(HR)) {
		if (ErrorMSG) {
			char* compileErrors = (char*)ErrorMSG->GetBufferPointer();
			MessageBoxA(nullptr, compileErrors, "HLSL 문법 에러 발생", MB_OK);
			ErrorMSG->Release();
		}
		else {
			char msg[256];
			sprintf_s(msg, "HRESULT 에러 코드: 0x%08X", HR);
			MessageBoxA(nullptr, msg, "시스템 에러 (ErrorMSG 없음)", MB_OK);
		}
	}

	ComPtr<ID3DX11EffectTechnique>	TCH = EffectObject->GetTechniqueByIndex(0);

	D3DX11_TECHNIQUE_DESC TDESC = {};

	TCH->GetDesc(&TDESC);

	TechniquePassesCount = TDESC.Passes;

	for (uint32_t IDX = 0; IDX < TechniquePassesCount; ++IDX) {
		ComPtr<ID3D11InputLayout>	InputLayout = nullptr;
		ComPtr<ID3DX11EffectPass>	EffectPass = TCH->GetPassByIndex(IDX);
		D3DX11_PASS_DESC			PDESC = {};

		EffectPass->GetDesc(&PDESC);

		if (FAILED(GRPDEV->CreateInputLayout(_DESC, _ElemCount, PDESC.pIAInputSignature, PDESC.IAInputSignatureSize, InputLayout.GetAddressOf()))) {
			MSG_BOX("Cannot Create InputLayout.");
			return E_FAIL;
		}

		InputLayoutList.push_back(InputLayout);
	}
	return S_OK;
}
HRESULT	Shader::Initialize(VOID* _ARG) {

	return S_OK;
}

HRESULT Shader::Shader_Begin(uint32_t PassIndex) {
	if (PassIndex >= TechniquePassesCount)	return E_FAIL;

	DEVCTX->IASetInputLayout(InputLayoutList[PassIndex].Get());
	// technique11 > (PassIndex)0번 pass를 적용할 것이다.
	if (FAILED(EffectObject->GetTechniqueByIndex(0)->GetPassByIndex(PassIndex)->Apply(0, DEVCTX.Get()))) {
		MSG_BOX("Cannot Apply EffectPass.");		
		return E_FAIL;
	}

	return S_OK;
}
HRESULT Shader::Bind_ShaderResourceView(const string& _ShaderVariableName, ComPtr<ID3D11ShaderResourceView>& _SRV) {
	if (nullptr == EffectObject)	return E_FAIL;

	ID3DX11EffectVariable*	EffectVariable = EffectObject->GetVariableByName(_ShaderVariableName.c_str());
	if (nullptr == EffectVariable)	return E_FAIL;

	ID3DX11EffectShaderResourceVariable*	EffectResourceVariable = EffectVariable->AsShaderResource();
	if (nullptr == EffectResourceVariable)	return E_FAIL;

	return EffectResourceVariable->SetResource(_SRV.Get());
}
HRESULT Shader::UnBind_ShaderResourceView(const string& _ShaderVariableName) {
	if (nullptr == EffectObject)	return E_FAIL;

	ComPtr<ID3DX11EffectVariable>	EffectVariable = EffectObject->GetVariableByName(_ShaderVariableName.c_str());
	if (nullptr == EffectVariable)	return E_FAIL;

	ComPtr<ID3DX11EffectShaderResourceVariable>	EffectResourceVariable = EffectVariable->AsShaderResource();
	if (nullptr == EffectResourceVariable)	return E_FAIL;

	return EffectResourceVariable->SetResource(nullptr);
}
HRESULT Shader::Bind_ShaderResourceViewArray(const string& _ShaderVariableName, vector<ComPtr<ID3D11ShaderResourceView>> _SRVArray) {
	if (nullptr == EffectObject)	return E_FAIL;

	ComPtr<ID3DX11EffectVariable>	EffectVariable = EffectObject->GetVariableByName(_ShaderVariableName.c_str());
	if (nullptr == EffectVariable)	return E_FAIL;

	ComPtr<ID3DX11EffectShaderResourceVariable>	EffectResourceVariable = EffectVariable->AsShaderResource();
	if (nullptr == EffectResourceVariable)	return E_FAIL;
	EffectResourceVariable->SetResourceArray(_SRVArray.data()->GetAddressOf(), 0, static_cast<uint32_t>(_SRVArray.size()));
	
	return S_OK;
}

HRESULT Shader::Bind_Matrix(const string& _ShaderVariableName, const XMFLOAT4X4* _Matrix) {
	if (nullptr == EffectObject)		return E_FAIL;

	ComPtr<ID3DX11EffectVariable>		EffectVariable = EffectObject->GetVariableByName(_ShaderVariableName.c_str());
	if (nullptr == EffectVariable)		return E_FAIL;

	ComPtr<ID3DX11EffectMatrixVariable> MatrixVariable = EffectVariable->AsMatrix();
	if (nullptr == MatrixVariable)		return E_FAIL;

	return MatrixVariable->SetMatrix(reinterpret_cast<const _float*>(_Matrix));
}
HRESULT Shader::Bind_MatrixArray(const string& _ShaderVariableName, const XMFLOAT4X4* _MatrixList, uint32_t _MatrixCount) {
	if (nullptr == EffectObject)		return E_FAIL;

	ComPtr<ID3DX11EffectVariable>		EffectVariable = EffectObject->GetVariableByName(_ShaderVariableName.c_str());
	if (nullptr == EffectVariable)		return E_FAIL;

	ComPtr<ID3DX11EffectMatrixVariable> MatrixVariable = EffectVariable->AsMatrix();
	if (nullptr == MatrixVariable)		return E_FAIL;

	return MatrixVariable->SetMatrixArray(reinterpret_cast<const _float*>(_MatrixList), 0, _MatrixCount);
}
HRESULT	Shader::Bind_RawValue(const string& _ShaderVariableName, const VOID* _Value, uint32_t _ByteLength) {
	if (nullptr == EffectObject)		return E_FAIL;

	ComPtr<ID3DX11EffectVariable>	EffectVariable = EffectObject->GetVariableByName(_ShaderVariableName.c_str());
	if (nullptr == EffectVariable)		return E_FAIL;

	return EffectVariable->SetRawValue(_Value, 0, _ByteLength);
}

HRESULT Shader::Bind_TextureDirect(const string& _ShaderVariableName, filesystem::path _FilePath) {
	wstring FilePath = _FilePath.wstring();
	wstring FileName = _FilePath.filename().wstring();

	filesystem::path DDSFilePath = _FilePath.replace_extension(".dds");
	wstring DFilePath = DDSFilePath.wstring();

	ComPtr<ID3D11ShaderResourceView> Resource = nullptr;
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
	if (FAILED(Bind_ShaderResourceView(_ShaderVariableName, Resource))) {
		MSG_BOX("Cannot Bind ShaderResource.");
		assert(0);
	}
	return S_OK;
}

unique_ptr<Shader>		Shader::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX, const TCHAR* _FilePath, const D3D11_INPUT_ELEMENT_DESC* _DESC, uint32_t _ElemCount) {
	auto Instance = unique_ptr<Shader>(new Shader(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType(_FilePath, _DESC, _ElemCount))) {
		MSG_BOX("Cannot Create Shader");
		return nullptr;
	}
	return Instance;
}
shared_ptr<Component>	Shader::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<Shader>(new Shader(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone Shader");
		return nullptr;
	}
	return Instance;
}