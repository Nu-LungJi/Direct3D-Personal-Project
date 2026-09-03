#include "../Default/PCH.h"
#include "GameInstance.h"

HitBoxPool::HitBoxPool(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) {}
HitBoxPool::HitBoxPool(CONST HitBoxPool& _PRTOBJ) : GameObject(_PRTOBJ) {}

HitBoxPool::~HitBoxPool() {}

HRESULT		HitBoxPool::Initialize_ProtoType() {
	HitBoxList.reserve(MAX_HITBOXPOOL_SIZE);
	shared_ptr<Scene> CurrentScene = GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene();
	CurrentScene->Add_ProtoType_CurrentScene("PRT_PlayerHitBox", PlayerHitBox::Create(GRPDEV, DEVCTX));

	return S_OK;
}
HRESULT		HitBoxPool::Initialize(VOID* _ARG) {
	shared_ptr<Scene> CurrentScene = GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene();
	for (uint32_t IDX = 0; IDX < MAX_HITBOXPOOL_SIZE; ++IDX) {
		string HitBoxTag = "PlayerHitBox" + to_string(IDX);
		CurrentScene->Add_CloneObject((uint32_t)LAYER_TYPE::LAYER_1, "PRT_PlayerHitBox", HitBoxTag, nullptr);
		HitBoxList.push_back(static_pointer_cast<PlayerHitBox>(CurrentScene->Get_GameObject(HitBoxTag)));
	}
	return S_OK;
}
VOID		HitBoxPool::Update(CONST _float& _DT) {

}
VOID		HitBoxPool::Late_Update(CONST _float& _DT) {

}
HRESULT		HitBoxPool::Render() {

	return S_OK;
}

VOID HitBoxPool::Generate_HitBox(XMVECTOR _Position, XMVECTOR _Rotation, XMVECTOR _StartScale, XMVECTOR _EndScale, _float _Duration, _float _AppearTimeOffset, _float _AfterDurationTime, _float _Damage, _float _DFTTimeOffset, FLOWTYPE _FTYPE) {
	for (auto& HB : HitBoxList) {
		if (HB->IsActivated() == false) {
			HB->Activate_HitBox(_Position, _Rotation, _StartScale, _EndScale, _Duration, _AppearTimeOffset, _AfterDurationTime, _Damage, _DFTTimeOffset, _FTYPE);
			return;
		}
	}
	assert(0);
}


unique_ptr<HitBoxPool>	HitBoxPool::Create(ComPtr<ID3D11Device>	_GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<HitBoxPool>(new HitBoxPool(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create HitBoxPool.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>	HitBoxPool::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<HitBoxPool>(new HitBoxPool(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone HitBoxPool.");
		return nullptr;
	}
	return Instance;
}