#include "CollisionManager.h"
#include "GameInstance.h"

CollisionManager::CollisionManager(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GRPDEV(_GRPDEV), DEVCTX(_DEVCTX) {}
CollisionManager::~CollisionManager()	{}

HRESULT CollisionManager::Ready_CollisionManager() {
#ifdef _DEBUG
	BATCH = make_shared<PrimitiveBatch<VertexPositionColor>>(DEVCTX.Get());
	EFFECT = make_shared<BasicEffect>(GRPDEV.Get());
	EFFECT->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t iShaderByteCodeLength = { };
	EFFECT->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(GRPDEV->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &LAYOUT)))
		return E_FAIL;
#endif

	return S_OK;
}

VOID CollisionManager::Update_CollisionManager(CONST _float& _DT) {
	for (auto& Col : ColliderList) 
		Col->Update(_DT);
}
VOID CollisionManager::Late_Update_CollisionManager(const _float& _DT) {
	for (uint32_t SIDX = 0; SIDX < ColliderList.size(); ++SIDX) {
		auto& SRC = ColliderList[SIDX];

		if (SRC->Get_ColliderActiveState() == false) continue;

		for (uint32_t DIDX = SIDX + 1; DIDX < ColliderList.size(); ++DIDX) {
			auto& DST = ColliderList[DIDX];

			if (DST->Get_ColliderActiveState() == false) continue;

			_bool Result = SRC->Evaluate_OnCollision(DST, DST->Get_ColliderType());

			vector<shared_ptr<Collider>>* SRCCollisionList = SRC->Get_ColliderList();

			auto iter = find(SRCCollisionList->begin(), SRCCollisionList->end(), DST);
			_bool OnCollisionBefore = (iter != SRCCollisionList->end());

			if (Result) {
				if (!OnCollisionBefore) {
					SRCCollisionList->push_back(DST);
					DST->Get_ColliderList()->push_back(SRC);

					SRC->Get_ColliderOwner()->On_CollisionEnter(DST->Get_ColliderOwner());
					DST->Get_ColliderOwner()->On_CollisionEnter(SRC->Get_ColliderOwner());
				}
				else {
					SRC->Get_ColliderOwner()->On_CollisionStay(DST->Get_ColliderOwner());
					DST->Get_ColliderOwner()->On_CollisionStay(SRC->Get_ColliderOwner());
				}
			}
			else {
				if (OnCollisionBefore) {
					SRCCollisionList->erase(iter);

					vector<shared_ptr<Collider>>* DSTCollisionList = DST->Get_ColliderList();

					auto DSTiter = find(DSTCollisionList->begin(), DSTCollisionList->end(), SRC);
					if (DSTiter != DSTCollisionList->end()) {
						DSTiter = DSTCollisionList->erase(DSTiter);
					}

					SRC->Get_ColliderOwner()->On_CollisionExit(DST->Get_ColliderOwner());
					DST->Get_ColliderOwner()->On_CollisionExit(SRC->Get_ColliderOwner());
				}
			}
		}
	}
}
#ifdef _DEBUG
VOID CollisionManager::DebugRender_Begin(){

	EFFECT->SetWorld(XMMatrixIdentity());
	EFFECT->SetView(GameInstance::GetInstance().Get_MainCamera()->Get_ViewMatrix());
	EFFECT->SetProjection(GameInstance::GetInstance().Get_MainCamera()->Get_ProjMatrix());

	EFFECT->Apply(DEVCTX.Get());
	DEVCTX->IASetInputLayout(LAYOUT.Get());
	DEVCTX->GSSetShader(nullptr, nullptr, 0);
	BATCH->Begin();
}

VOID CollisionManager::DebugRender_End() {
	BATCH->End();
}
#endif
VOID CollisionManager::UnRegister_Collider(shared_ptr<Collider> _Collider) {
	auto iter = find(ColliderList.begin(), ColliderList.end(), _Collider);
	if (iter != ColliderList.end()) {
		*iter = ColliderList.back();
		ColliderList.pop_back();
	}
}
VOID CollisionManager::Reset_Recipient() {
	ColliderList.clear();
}
unique_ptr<CollisionManager>	CollisionManager::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<CollisionManager>(new CollisionManager(_GRPDEV, _DEVCTX));
	if (Instance->Ready_CollisionManager()) {
		MSG_BOX("Cannot Create CollisionManager.");
		return nullptr;
	}
	return Instance;
}