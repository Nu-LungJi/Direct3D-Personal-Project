#pragma once
#include "Buffer.h"

BEGIN(Engine)
class ENGINE_DLL Terrain : public Buffer {
	struct WaterContant {
		XMMATRIX InvViewProj;
		XMFLOAT4 ScreenParam;
		XMFLOAT3 WorldSpaceCameraPos;
		_float Time;
	};
private:
	Terrain(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	Terrain(CONST Terrain& _PRT);
public:
	virtual ~Terrain();

public:
	virtual HRESULT		Initialize_ProtoType();
	virtual HRESULT		Initialize(void* _ARG);

	virtual HRESULT		Create_VertexBuffer();
	virtual HRESULT		Create_IndexBuffer();
	virtual HRESULT		Render_Buffer();

	virtual VOID		Update(const _float& _DT) override;

	VOID				Create_TerrainVertices(uint32_t _CountX, uint32_t _CountZ);

	static	unique_ptr<Terrain>		Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual	shared_ptr<Component>	Clone(VOID* _ARG);

private:
	uint32_t VTXCNTX = {};
	uint32_t VTXCNTZ = {};
};
END