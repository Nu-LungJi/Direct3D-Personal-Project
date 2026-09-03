#pragma once
#include "Component.h"
#include "Buffer.h"
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL TexBuffer : public Buffer {
	// 밉맵은 텍스쳐를 반씩 줄이면서 1대1(2의 n승)텍스쳐를 메모리에 배열형태로 저장한다.
	// DX11은 자동으로 원본 텍스쳐를 2의 n승으로 조정하지만, 밉맵은 자동으로 생성하지 않는다.(DX9에서는 자동으로 생성.)
private:
	TexBuffer(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	TexBuffer(CONST TexBuffer& _PRTOBJ);
public:
	virtual ~TexBuffer();

public:
	virtual HRESULT	Initialize_ProtoType();
	virtual HRESULT	Initialize(VOID* _ARG);

	virtual HRESULT	Create_VertexBuffer();
	virtual HRESULT	Create_IndexBuffer();

	HRESULT	Load_Texture(const wstring& _FilePath, uint32_t _AlwaysZero);
	HRESULT	Load_Texture(ComPtr<ID3D11ShaderResourceView> _Resource);

	HRESULT	Bind_ShaderResource(shared_ptr<Shader> _CMPShader, const string& _SVName, uint32_t _TextureIndex);

	HRESULT	Get_TextureSize(uint32_t _TextureNumb, _float& _Width, _float& _Height);
	ComPtr<ID3D11ShaderResourceView>	Get_Texture(uint32_t _TexIndex);

	static	unique_ptr<TexBuffer>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual	shared_ptr<Component>	Clone(VOID* _ARG);

private:
	uint32_t									TextureCount = {};
	vector<ComPtr<ID3D11ShaderResourceView>>	TextureList;
};

END