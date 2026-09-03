#pragma once
#include "Engine_Define.h"

BEGIN(Engine)
class ENGINE_DLL Mesh {
	Mesh(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX,
		vector<shared_ptr<Vertex>>& _VL, vector<shared_ptr<uint32_t>>& _IL, vector<shared_ptr<Texture>>& _TL);


private:
	HRESULT			Create_VertexBuffer();
	HRESULT			Create_IndexBuffer();

private:
	ComPtr<ID3D11Buffer>			VertexBuffer, IndexBuffer;
};
END