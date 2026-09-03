#include "../Default/PCH.h"
#include "GameInstance.h"

LoadingScene::LoadingScene(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : Scene(_GRPDEV, _DEVCTX){}
LoadingScene::~LoadingScene()   {}

HRESULT LoadingScene::Initialize() {


    return S_OK;
}
VOID LoadingScene::Update(const _float& _DT) {

}
HRESULT LoadingScene::Register_ProtoType() {


    return S_OK;
}
HRESULT LoadingScene::Initialize_SceneObject() {


    return S_OK;
}
HRESULT LoadingScene::Initialize_MapObject() {


    return S_OK;
}

LOADING_STATE LoadingScene::Update_LoadingScene(){
    if (LoadingCompleted == LOADING_STATE::COMPLETE && KEY_DOWN(DIK_3)) {
        GameInstance::GetInstance().Get_SceneManager()->Set_CurrentScene(NextSceneIndex);
        NextSceneIndex = 0;
        LoadingCompleted = LOADING_STATE::ONREADY;
        return LOADING_STATE::COMPLETE;
    }

    return LoadingCompleted;
}

shared_ptr<LoadingScene>    LoadingScene::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
    auto	SceneInstance = shared_ptr<LoadingScene>(new LoadingScene(_GRPDEV, _DEVCTX));
    if (FAILED(SceneInstance->Initialize())) {
        MSG_BOX("Failed to Created : LoadingScene");
        return nullptr;
    }
    return SceneInstance;
}
shared_ptr<GameObject>      LoadingScene::Clone(VOID* _ARG) {
    return nullptr;
}
