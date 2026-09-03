#include "../Default/PCH.h"
#include "GameInstance.h"
DamageFontUI::DamageFontUI(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) { }
DamageFontUI::DamageFontUI(const DamageFontUI& _PRTOBJ) : GameObject(_PRTOBJ), SPRBatch(_PRTOBJ.SPRBatch), FontState(_PRTOBJ.FontState)
, DamageFontTexturePool(_PRTOBJ.DamageFontTexturePool), RenderDamageFont(_PRTOBJ.RenderDamageFont){
	for (uint32_t IDX = 0; IDX < 10; IDX++) OriginalTexture[IDX] = _PRTOBJ.OriginalTexture[IDX];
}

DamageFontUI::~DamageFontUI() { }

HRESULT DamageFontUI::Initialize_ProtoType() {
	SPRBatch = GameInstance::GetInstance().Get_SpriteBatch();
	FontState = GameInstance::GetInstance().Get_CommonState();

	uint32_t CurrentSceneIndex = GameInstance::GetInstance().Get_CurrentSceneIndex();

	for (uint32_t IDX = 0; IDX < 10; IDX++) {
		string DMGTag = "DMGFNT_NUM:" + to_string(IDX);
		wstring FilePath = L"../../Resource/Asset/MainUI/DMF_" + to_wstring(IDX) + L".png";
		OriginalTexture[IDX] = static_pointer_cast<UIObject>(GameInstance::GetInstance().Get_ProtoManager()->Clone_ProtoType(CurrentSceneIndex, "PRT_UIObject", DMGTag, nullptr));
		OriginalTexture[IDX]->Create_UIObject(FilePath, {640.f, 360.f}, 0.75f, 1.f);
	}
	return S_OK;
}
HRESULT DamageFontUI::Initialize(VOID* _ARG) {
	uint32_t PoolSize = 100;

	DamageFontTexturePool.reserve(PoolSize);
	for (uint32_t IDX = 0; IDX < PoolSize; IDX++) {
		DamageFontTexturePool.push_back(shared_ptr<DamageFont>(new DamageFont()));
	}
	return S_OK;
}
VOID DamageFontUI::Update(const _float& _DT) {

	for (auto iter = RenderDamageFont.begin(); iter != RenderDamageFont.end();) {
		auto& FOBJ = *iter;

		if (FOBJ->MaxLifeTime > FOBJ->CurrentLifeTime) {
			FOBJ->CurrentLifeTime += _DT;
			++iter;
		}
		else {
			FOBJ->CurrentLifeTime = 0.f;
			FOBJ->RenderActivation	= false;

			iter = RenderDamageFont.erase(iter);
		}
	}
}
VOID DamageFontUI::Late_Update(const _float& _DT) {
	GameInstance::GetInstance().Get_RenderManager()->Add_GameObject(RENDER_TYPE::RENDER_UI, shared_from_this());
}
HRESULT DamageFontUI::Render() {
	_float DMGSpacing = 18.f;
	DEVCTX->GSSetShader(nullptr, nullptr, 0);
	SPRBatch->Begin(DX11::SpriteSortMode_Deferred, FontState->NonPremultiplied());

	for (auto& FOBJ : RenderDamageFont) {
		if (FOBJ->RenderActivation == false) continue;

		wstring DamageString = FOBJ->DamageString;
		_float FrontTextPosX = FOBJ->ScreenPosition.x + FOBJ->ScreenOffset.x - (DamageString.size() * DMGSpacing / 2.f);

		_float ScaleAlphaDownSpeed = 15.f, ScaleDownClamp = 0.8f, AlphaDownClamp = 0.85f;
		_float TextAppearInterval = 0.05f, TextDisappearTime = 0.6f, AlphaDisappearSpeed = 7.f;

		for (uint32_t IDX = 0; IDX < DamageString.size(); ++IDX) {
			if (FOBJ->CurrentLifeTime < TextAppearInterval * IDX) continue;

			uint32_t Numb = DamageString[IDX] - L'0';

			_float IndividualLifeTime = FOBJ->CurrentLifeTime - TextAppearInterval * IDX;

			_float IndividualScale  = max(FOBJ->FontScale - (IndividualLifeTime * ScaleAlphaDownSpeed), ScaleDownClamp);
			if (IndividualLifeTime < 0.15f) {
				IndividualScale = 1.5f - powf(1.5f - IndividualLifeTime / 0.15f, 3.f);
			}
			
			_float IndividualAlpha  = min(FOBJ->FontAlpha + (IndividualLifeTime * ScaleAlphaDownSpeed), AlphaDownClamp);
			_float IndividualOffset = 0.f;

			_float DisappearSinkTime = TextDisappearTime + TextAppearInterval * (DamageString.size() - IDX);
			if (IndividualLifeTime >= DisappearSinkTime) {
				_float FadeAge = IndividualLifeTime - DisappearSinkTime;

				IndividualAlpha  = max(0.f, AlphaDownClamp - (FadeAge * AlphaDisappearSpeed));
				IndividualOffset = -(FadeAge * 200.f);
			}

			XMFLOAT3 TargetDrawPosition;
			
			TargetDrawPosition.z = Numb == 1 ? 1.2f : 1.f;
			TargetDrawPosition = { FrontTextPosX + DMGSpacing / TargetDrawPosition.z, FOBJ->ScreenPosition.y + FOBJ->ScreenOffset.y + IndividualOffset, TargetDrawPosition.z };

			FrontTextPosX = TargetDrawPosition.x;

			OriginalTexture[Numb]->Set_Position({ TargetDrawPosition.x, TargetDrawPosition.y });
			OriginalTexture[Numb]->Set_Scale(IndividualScale);
			OriginalTexture[Numb]->Set_Alpha(IndividualAlpha);
			OriginalTexture[Numb]->Render(SPRBatch);
		}
	}

	SPRBatch->End();

	return S_OK;
}
VOID DamageFontUI::Generate_DamageFont(XMVECTOR _WorldPos, uint32_t _Damage){
	shared_ptr<DamageFont>	TargetPoolObject = nullptr;
	uint32_t PoolSize = static_cast<uint32_t>(DamageFontTexturePool.size());

	for (uint32_t IDX = 0; IDX < PoolSize; ++IDX) {
		uint32_t CircularIndex = (LastAllocatedIndex + 1 + IDX) % PoolSize;

		if (DamageFontTexturePool[CircularIndex]->RenderActivation == false) {
			DamageFontTexturePool[CircularIndex]->RenderActivation = true;
			TargetPoolObject = DamageFontTexturePool[CircularIndex];
			LastAllocatedIndex = CircularIndex;
			break;
		}
	}

	if (nullptr == TargetPoolObject) return;

	XMFLOAT2 MonsterScreenPos = GameInstance::GetInstance().Get_Utility()->Convert_WorldPositionToScreen(_WorldPos + XMVectorSet(0.f, 2.f, 0.f, 0.f));

	TargetPoolObject->RenderActivation	= true;
	TargetPoolObject->ScreenPosition	= MonsterScreenPos;
	TargetPoolObject->DamageValue		= _Damage;
	TargetPoolObject->CurrentLifeTime	= 0.f;
	TargetPoolObject->FontScale			= 2.5f;
	TargetPoolObject->FontAlpha			= 0.f;
	TargetPoolObject->DamageString		= to_wstring(_Damage);

	_float MinRadiusOffset = 50.f, MaxRadiusOffset = 100.f;
	_float RandomAngle = XMConvertToRadians(static_cast<_float>(rand() % 360));
	_float RandomDistance = MinRadiusOffset + static_cast<_float>(rand() % static_cast<int32_t>(MaxRadiusOffset - MinRadiusOffset));

	TargetPoolObject->ScreenOffset = XMFLOAT2(cosf(RandomAngle) * RandomDistance, sinf(RandomAngle) * RandomDistance);

	RenderDamageFont.insert(TargetPoolObject);
}
unique_ptr<DamageFontUI> DamageFontUI::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto	Instance = unique_ptr<DamageFontUI>(new DamageFontUI(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create DamageFontUI.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>	 DamageFontUI::Clone(VOID* _ARG) {
	auto	Instance = shared_ptr<DamageFontUI>(new DamageFontUI(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone DamageFontUI.");
		return nullptr;
	}
	return Instance;
}
