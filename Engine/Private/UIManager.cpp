#include "UIManager.h"
#include "GameInstance.h"
#include "InputManager.h"

UIManager::UIManager(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) :GRPDEV(_GRPDEV), DEVCTX(_DEVCTX) {}
UIManager::~UIManager() {}

HRESULT UIManager::Ready_UIManager() {

    //Create_UISprite(UI_LAYER::MAIN_UI, L"../../Resource/EmptyDump.png"            , make_shared<SpriteAttribute>(SpriteAttribute{ L"None"      , UI_TYPE::EXTRA, {0.00f, 0.00f}, {0.0f, 0.0f}  , 0 }));
    //Create_UISprite(UI_LAYER::MAIN_UI, L"../../Resource/T_IconA_currency_1_UI.png", make_shared<SpriteAttribute>(SpriteAttribute{ L"Main Icon" , UI_TYPE::EXTRA, {100.f, 100.f}, {0.5f, 0.5f}  , 1 }));
    //Create_UISprite(UI_LAYER::MAIN_UI, L"../../Resource/T_IconA_jzmy_01_UI.png"   , make_shared<SpriteAttribute>(SpriteAttribute{ L"Sub Icon"  , UI_TYPE::EXTRA, {100.f, 500.f}, {0.5f, 0.5f}  , 1 }));
    //Create_UISprite(UI_LAYER::MAIN_UI, L"../../Resource/CursorNor.png"            , make_shared<SpriteAttribute>(SpriteAttribute{ L"Cursor"    , UI_TYPE::EXTRA, {0.00f, 0.00f}, {0.5f, 0.5f}  , 1 }));   
    //
    //Selected_Object = Get_SpriteAttribute(L"None");
    //Cursor_Object   = Get_SpriteAttribute(L"Cursor");
    //Dump_Object     = Selected_Object;


	return S_OK;
}
VOID    UIManager::Update_UIManager(CONST FLOAT& _DT) {

}
VOID    UIManager::LateUpdate_UIManager(CONST FLOAT& _DT) {
    POINT mousePoint{ 0, 0 };
    GetCursorPos(&mousePoint);
    ScreenToClient(hWnd, &mousePoint);

    //Cursor_Object(int)mousePoint.x +  9.f;
    //Cursor_Object(int)mousePoint.y + 10.f;
}
VOID    UIManager::Render_UIManager() {

}
VOID    UIManager::Create_UISprite(uint32_t _ULAYER, wstring _FileName, shared_ptr<UIObject> _SPRATT) {
    
}

unique_ptr<UIManager>	UIManager::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
    auto UM = unique_ptr<UIManager>(new UIManager(_GRPDEV, _DEVCTX));
    if (FAILED(UM->Ready_UIManager()))
        MSG_BOX("Cannot Create UIManager.");

    return UM;
}
