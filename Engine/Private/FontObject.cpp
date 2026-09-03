#include "FontObject.h"
#include "GameInstance.h"

FontObject::FontObject(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) {}
FontObject::FontObject(CONST FontObject& _PRTOBJ) : GameObject(_PRTOBJ) {}

HRESULT FontObject::Render(shared_ptr<SpriteBatch> _BATCH) {
	if (Visibility == false) return E_FAIL;
	FontOBJ->DrawString(_BATCH.get(), FontText.c_str(), FontPosition,
		XMVectorSetW(XMLoadFloat3(&FontColor), FontAlpha), 0.f, FontOrigin, FontScale);

    return S_OK; 
}
HRESULT FontObject::Create_FontObject(const wstring& _FileName, wstring _Text, _float2 _Position, _float _Scale, _float3 _Color, _float _Alpha) {
	FontOBJ = make_shared<SpriteFont>(GRPDEV.Get(), _FileName.c_str());
	
	FontText = _Text; FontPosition = _Position; FontScale = _Scale; FontColor = _Color; FontAlpha = _Alpha;

	XMStoreFloat2(&FontOriginScale, FontOBJ->MeasureString(FontText.c_str()));
	
	XMVECTOR HalfScale = XMVectorScale(XMLoadFloat2(&FontOriginScale), 0.5f);
	XMStoreFloat2(&FontOrigin, HalfScale);
	
	return S_OK;  
}
unique_ptr<FontObject>	FontObject::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<FontObject>(new FontObject(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create FontObject.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>  FontObject::Clone(VOID*_ARG) {
	auto Instance = shared_ptr<FontObject>(new FontObject(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone FontObject.");
		return nullptr;
	}
	return Instance;
}
