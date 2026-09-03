#include "UIObject.h"
#include "GameInstance.h"

UIObject::UIObject(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) {}
UIObject::UIObject(CONST UIObject& _PRTOBJ) : GameObject(_PRTOBJ) {}
UIObject::~UIObject() {}

HRESULT UIObject::Initialize(VOID* _ARG) {

    

    return S_OK;
}
VOID UIObject::Update(CONST _float& _DT) {


}
VOID UIObject::Late_Update(CONST _float& _DT) {
    
}
HRESULT UIObject::Render(shared_ptr<SpriteBatch> _BATCH) {
    if (Visibility == false) return E_FAIL;
    RECT DrawSpace = { 
        static_cast<LONG>(UINFO->UIPosition.x - (UINFO->UIOriginScale.x) * UINFO->UIScale / 2.f),
        static_cast<LONG>(UINFO->UIPosition.y - (UINFO->UIOriginScale.y) * UINFO->UIScale / 2.f),
        static_cast<LONG>(UINFO->UIPosition.x + (UINFO->UIOriginScale.x) * UINFO->UIScale / 2.f),
        static_cast<LONG>(UINFO->UIPosition.y + (UINFO->UIOriginScale.y) * UINFO->UIScale / 2.f)
    };
    _BATCH->Draw(SRV.Get(), DrawSpace, nullptr, XMVectorSet(1.f, 1.f, 1.f, UINFO->UIAlpha), UINFO->UIRotation, UINFO->UIOrigin);

    return S_OK;
}
HRESULT UIObject::Bind_Texture(const wstring& _TEX) {

    filesystem::path AssetPath = _TEX;
    filesystem::path DDSAssetPath = AssetPath;
    DDSAssetPath.replace_extension(".dds");

    if (filesystem::exists(DDSAssetPath)) {
        if (FAILED(CreateDDSTextureFromFile(GRPDEV.Get(), DDSAssetPath.wstring().c_str(), nullptr, SRV.GetAddressOf()))) {
            MSG_BOX("Cannot Create DDS Texture File.");
            assert(0);
        }
    }
    else {
        if (FAILED(CreateWICTextureFromFile(GRPDEV.Get(), AssetPath.wstring().c_str(), nullptr, SRV.GetAddressOf()))) {
            MSG_BOX("Cannot Create WIC Texture File.");
            assert(0);
        }
    }

    D3D11_TEXTURE2D_DESC DESC;
    ComPtr<ID3D11Resource> RS; ComPtr<ID3D11Texture2D> Tex;

    SRV->GetResource(RS.GetAddressOf());
    RS->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&Tex);
    Tex->GetDesc(&DESC);

    UINFO->UIOriginScale = { static_cast<_float>(DESC.Width), static_cast<_float>(DESC.Height) };
    UINFO->UIOrigin = { UINFO->UIOriginScale.x / 2.f, UINFO->UIOriginScale.y / 2.f };

    return S_OK;
}
HRESULT UIObject::Create_UIObject(const wstring& _TEX, _float2 _Position, _float _Scale, _float _OPC, _float _Rotation) {
    UINFO = make_shared<UIInfo>(UIInfo(_Position, _Scale, _OPC, _Rotation));

    Bind_Texture(_TEX);

    return S_OK;
}
unique_ptr<UIObject>	UIObject::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX){
    auto Instance = unique_ptr<UIObject>(new UIObject(_GRPDEV, _DEVCTX));
    if (FAILED(Instance->Initialize_ProtoType())) {
        MSG_BOX("Cannot Create UIObject.");
        return nullptr;
    }
    return Instance;
}
shared_ptr<GameObject>	UIObject::Clone(VOID* _ARG) {
    auto Instance = shared_ptr<UIObject>(new UIObject(*this));
    if (FAILED(Instance->Initialize(_ARG))) {
        MSG_BOX("Cannot Clone UIObject.");
        return nullptr;
    }
    return Instance;
}