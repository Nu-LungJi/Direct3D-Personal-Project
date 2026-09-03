#include "../Default/PCH.h"
#include "GameInstance.h"

PopUpUI::PopUpUI(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) {}
PopUpUI::PopUpUI(CONST PopUpUI& _PRTOBJ) : GameObject(_PRTOBJ) {}
PopUpUI::~PopUpUI() {}

HRESULT PopUpUI::Initialize_ProtoType() {
	
	return S_OK;
}
HRESULT PopUpUI::Initialize(VOID* _ARG) {

	SPRBatch = GameInstance::GetInstance().Get_SpriteBatch();
	FontState = GameInstance::GetInstance().Get_CommonState();

	POPUP_GuideBackGround	= Create_UIObject(L"../../Resource/Asset/MainUI/Guide_BackCircle.png"	, { 640.f + 75.f, 360.f + 75.f }, 0.25f, 0.7f, 0.f);
	POPUP_GuideIcon			= Create_UIObject(L"../../Resource/Asset/MainUI/Guide_MarkIcon.png"	, { 640.f + 50.f, 360.f + 50.f }, 0.50f, 0.7f, 0.f);
	POPUP_GuideDistance		= Create_FontObject(L"../../Resource/Asset/Font/WutheringWave_Font_ExtraBold.spritefont", L"0M", { 640.f, 360.f }, 0.4f, { 1.f, 1.f, 1.f }, 0.7f);

	POPUP_NCBackGround	= Create_UIObject(L"../../Resource/Asset/MainUI/POPUP_BackBar_NonComplete.png"	, { 1100.f, 450.f + 100.f }, 0.4f, 0.f, 0.f);
	POPUP_CBackGround	= Create_UIObject(L"../../Resource/Asset/MainUI/POPUP_BackBar_Complete.png"		, { 1100.f, 450.f + 100.f }, 0.4f, 0.f, 0.f);

	POPUP_NCMarkIcon	= Create_UIObject(L"../../Resource/Asset/MainUI/POPUP_MarkIcon_NonComplete.png"	, { 812.f, 442.f + 100.f }, 0.28f, 0.f, 0.f);
	POPUP_CMarkIcon		= Create_UIObject(L"../../Resource/Asset/MainUI/POPUP_MarkIcon_Complete.png"	, { 812.f, 442.f + 100.f }, 0.28f, 0.f, 0.f);

	POPUP_KeyIcon		= Create_UIObject(L"../../Resource/Asset/MainUI/KEY_F.png"						, { 770.f, 443.f + 100.f }, 0.45f, 0.f, 0.f);
	
	POPUP_DescFont = Create_FontObject(L"../../Resource/Asset/Font/WutheringWave_Font_Bold.spritefont", L"비콘 활성화", { 865.f, 200.f  }, 0.4f, { 1.f, 1.f, 1.f }, 0.f);
	
	Quest_Text[0] = Create_FontObject(L"../../Resource/Asset/Font/WutheringWave_Font_ExtraBold.spritefont", L"중앙 비콘 활성화 시키기", { 150.f, 198.f }, 0.5f, { 1.f, 1.f, 1.f }, 1.f);
	Quest_Text[1] = Create_FontObject(L"../../Resource/Asset/Font/WutheringWave_Font_Bold.spritefont", L"", { 45.f, 225.f }, 0.45f, { 1.f, 0.785f, 0.f }, 1.f);

	if		(GameInstance::GetInstance().Get_CurrentSceneIndex() == (uint32_t)SCENE_TYPE::SCENE_1) {
		shared_ptr<GameObject> BiconObject = GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Get_GameObject("Bicon");
		TargetPosition = static_pointer_cast<Transform>(BiconObject->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_WorldPosition();
	}
	else if (GameInstance::GetInstance().Get_CurrentSceneIndex() == (uint32_t)SCENE_TYPE::SCENE_2) {
		Quest_Text[0]->Set_FontObjectText(L"스카 격파.");
		Quest_Text[1]->Set_FontObjectText(L"");

		POPUP_DescFont->Set_FontObjectText(L"카드 열어보기.");

		shared_ptr<GameObject> BossCard = GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Get_GameObject("BossCard");
		TargetPosition = static_pointer_cast<Transform>(BossCard->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_WorldPosition();
	}
	
	EventUIObject = static_pointer_cast<EventUI>(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Get_GameObject("EventUI"));
	GamePlayer = static_pointer_cast<Player>(GameInstance::GetInstance().Get_GamePlayer());

	return S_OK;
}
VOID PopUpUI::Update(CONST _float& _DT) {
	XMFLOAT2 ScreenPosition = GameInstance::GetInstance().Get_Utility()->Convert_WorldPositionToScreen(TargetPosition + XMVectorSet(0.f, 1.f, 0.f, 0.f));

	GuideDistance = static_cast<uint32_t>(XMVectorGetX(XMVector3Length(XMVectorSubtract(TargetPosition, GamePlayer->Get_TransformComponent()->Get_WorldPosition()))));

	if (GuideDistance > 7) {
		PopDown_InteractionUI(_DT);
		if		(ScreenPosition.x <= 200.f)	 ScreenPosition.x = 200.f;
		else if (ScreenPosition.x >= 1080.f) ScreenPosition.x = 1080.f;
		if		(ScreenPosition.y <= 100.f)	 ScreenPosition.y = 100.f;
		else if (ScreenPosition.y >= 620.f)	 ScreenPosition.y = 620.f;
		
		POPUP_GuideBackGround->Set_Position(ScreenPosition + XMVectorSet(75.f * 0.25f, 75.f * 0.25f, 0.f, 0.f));
		POPUP_GuideIcon->Set_Position(ScreenPosition + XMVectorSet(50.f * 0.5f, 50.f * 0.5f, 0.f, 0.f));

		POPUP_GuideDistance->Set_FontObjectText(to_wstring(GuideDistance) + L"M");
		POPUP_GuideDistance->Set_FontObjectPosition(ScreenPosition - XMVectorSet(10.f, 50.f * 0.5f, 0.f, 0.f));
	}
	else {
		PopUp_InteractionUI(_DT);
		if		(GameInstance::GetInstance().Get_CurrentSceneIndex() == (uint32_t)SCENE_TYPE::SCENE_1) {
			Update_MissionSequence_Scene_01();
		}
		else if (GameInstance::GetInstance().Get_CurrentSceneIndex() == (uint32_t)SCENE_TYPE::SCENE_2) {
			if (!MissionSequence[0] && KEY_DOWN(DIK_F)) {
				static_pointer_cast<BossCard>(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Get_GameObject("BossCard"))->Enable_Dissolve(PLAY_TYPE::ONSTART, 1.f, 1.f);

				Quest_Text[0]->Set_FontObjectText(L"스카 격파.");
				Quest_Text[1]->Set_FontObjectText(L"");

				POPUP_GuideBackGround->Set_Visibility(false);
				POPUP_GuideIcon->Set_Visibility(false);
				POPUP_GuideDistance->Set_Visibility(false);

				TargetPosition = XMVectorSet( 0.f, 99999.f, 0.f, 0.f );
			}
		}

		
	}
	if (GameInstance::GetInstance().Get_CurrentSceneIndex() == (uint32_t)SCENE_TYPE::SCENE_1) {
		Quest_Text[1]->Set_FontObjectText(L"아래 " + to_wstring(GuideDistance) + L"M");
	}
	if (GameInstance::GetInstance().Get_CurrentSceneIndex() == (uint32_t)SCENE_TYPE::SCENE_2) {
		Quest_Text[1]->Set_FontObjectText(L"");
	}
}
VOID PopUpUI::Late_Update(CONST _float& _DT) {
	if (EventUIObject->Get_EventActivation() == false) {
		GameInstance::GetInstance().Get_RenderManager()->Add_GameObject(RENDER_TYPE::RENDER_UI, shared_from_this());
	}
}
HRESULT PopUpUI::Render() {
	//if (GuideDistance > 160) {
		DEVCTX->GSSetShader(nullptr, nullptr, 0);
		SPRBatch->Begin(DX11::SpriteSortMode_Immediate, FontState->NonPremultiplied());
		for (auto& UOBJ : UIObjectList)
			UOBJ->Render(SPRBatch);

		for (auto& FOBJ : FontObjectList)
			FOBJ->Render(SPRBatch);

		SPRBatch->End();
	//}

	return S_OK;
}
shared_ptr<UIObject>	PopUpUI::Create_UIObject(filesystem::path _TexRes, _float2 _Position, _float _Scale, _float _OPC, _float _Rotation) {
	uint32_t CurrentSceneIndex = GameInstance::GetInstance().Get_CurrentSceneIndex();
	string	NewTag = _TexRes.filename().string();

	shared_ptr<UIObject> CLNOBJ = static_pointer_cast<UIObject>(GameInstance::GetInstance().Get_ProtoManager()->Clone_ProtoType(CurrentSceneIndex, "PRT_UIObject", NewTag, nullptr));

	if (FAILED(CLNOBJ->Create_UIObject(_TexRes, _Position, _Scale, _OPC, _Rotation))) {
		MSG_BOX("Cannot Ready UIObject TextureState.");
		return nullptr;
	}

	UIObjectList.push_back(CLNOBJ);
	GameInstance::GetInstance().Get_GUIEditor()->Register_UIObject(CLNOBJ);

	return UIObjectList.back();
}
shared_ptr<FontObject>	PopUpUI::Create_FontObject(const wstring& _FontFilePath, wstring _Text, _float2 _Position, _float _Scale, _float3 _Color, _float _Alpha) {
	uint32_t CurrentSceneIndex = GameInstance::GetInstance().Get_CurrentSceneIndex();
	string	NewTag = "";
	shared_ptr<FontObject>  FOBJ = static_pointer_cast<FontObject>(GameInstance::GetInstance().Get_ProtoManager()->Clone_ProtoType(CurrentSceneIndex, "PRT_FontObject", NewTag, nullptr));

	if (nullptr == FOBJ) {
		MSG_BOX("Cannot Create FontObject");
		assert(0);
	}

	FOBJ->Create_FontObject(_FontFilePath, _Text, _Position, _Scale, _Color, _Alpha);

	FontObjectList.push_back(FOBJ);

	return FontObjectList.back();
}
VOID PopUpUI::PopUp_InteractionUI(const _float& _DT) {
	_float Duration = 0.2f;
	if (InRanged == true) { 
		InRanged = false; 
		TimeAccumulation = 0.f;
	
		POPUP_GuideBackGround->Set_Alpha(0.f);
		POPUP_GuideIcon->Set_Alpha(0.f);
		POPUP_GuideDistance->Set_FontObjectAlpha(0.f);
	}

	TimeAccumulation += _DT;
	if (TimeAccumulation > Duration) TimeAccumulation = Duration;

	_float Weight = EASEOUT(TimeAccumulation / Duration, 3.f);

	_float MaxAlpha = 0.9f;
	_float CurrentAlpha = Weight * MaxAlpha;

	POPUP_NCBackGround->Set_Alpha(CurrentAlpha);
	POPUP_NCMarkIcon->Set_Alpha(CurrentAlpha);
	POPUP_KeyIcon->Set_Alpha(CurrentAlpha);
	POPUP_DescFont->Set_FontObjectAlpha(CurrentAlpha);

	if (POPUP_NCBackGround->Get_Position().y >= 450.f) {
		_float CurrentY = LERP(550.f, 450.f, Weight);

		POPUP_NCBackGround->Set_Position({ POPUP_NCBackGround->Get_Position().x , CurrentY });
		POPUP_NCMarkIcon->Set_Position({ POPUP_NCMarkIcon->Get_Position().x , CurrentY - 8.f });
		POPUP_KeyIcon->Set_Position({ POPUP_KeyIcon->Get_Position().x , CurrentY - 7.f });
		POPUP_DescFont->Set_FontObjectPosition({ POPUP_DescFont->Get_FontObjectPosition().x , CurrentY - 22.f });
	}
}

VOID PopUpUI::PopDown_InteractionUI(const _float& _DT) {
	_float Duration = 0.2f;
	if (InRanged == false) { 
		InRanged = true; 
		TimeAccumulation = 0.f;
	
		POPUP_GuideBackGround->Set_Alpha(0.7f);
		POPUP_GuideIcon->Set_Alpha(0.7f);
		POPUP_GuideDistance->Set_FontObjectAlpha(0.7f);
	}

	TimeAccumulation += _DT;
	if (TimeAccumulation > Duration) TimeAccumulation = Duration;

	_float Weight = EASEOUT(TimeAccumulation / Duration, 3.f);

	_float CurrentAlpha = 0.9f * (1.f - Weight);

	POPUP_NCBackGround->Set_Alpha(CurrentAlpha);
	POPUP_NCMarkIcon->Set_Alpha(CurrentAlpha);
	POPUP_KeyIcon->Set_Alpha(CurrentAlpha);
	POPUP_DescFont->Set_FontObjectAlpha(CurrentAlpha);

	if (POPUP_NCBackGround->Get_Position().y <= 550.f) {
		_float CurrentY = LERP(450.f, 550.f, Weight);

		POPUP_NCBackGround->Set_Position({ POPUP_NCBackGround->Get_Position().x , CurrentY });
		POPUP_NCMarkIcon->Set_Position({ POPUP_NCMarkIcon->Get_Position().x , CurrentY - 8.f });
		POPUP_KeyIcon->Set_Position({ POPUP_KeyIcon->Get_Position().x , CurrentY - 7.f });
		POPUP_DescFont->Set_FontObjectPosition({ POPUP_DescFont->Get_FontObjectPosition().x , CurrentY - 22.f });
	}
}

VOID PopUpUI::Update_MissionSequence_Scene_01(){
	if (!MissionSequence[0] && KEY_DOWN(DIK_F)) {		// 미니게임
		EventUIObject->Set_EventActivation(false);
		auto RChestTRS = static_pointer_cast<Transform>(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Get_GameObject("RewardChest")->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM));
		TargetPosition = RChestTRS->Get_WorldPosition();

		Quest_Text[0]->Set_FontObjectText(L"차원문 열쇠 획득하기.");
		POPUP_DescFont->Set_FontObjectText(L"보상얻기");
		MissionSequence[0] = true;
	}
	else if (!MissionSequence[1] && KEY_DOWN(DIK_F)) {		// 상자 열기
		if (EventUIObject->Get_EventActivation() == false)	EventUIObject->Set_EventActivation(true);
		static_pointer_cast<RewardChest>(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Get_GameObject("RewardChest"))->Open_RewardBox();
		static_pointer_cast<RewardChest>(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Get_GameObject("RewardChest"))->Enable_Dissolve(PLAY_TYPE::ONSTART, 1.f, 1.f);
		TargetPosition = static_pointer_cast<Transform>(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Get_GameObject("Portal")->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Get_WorldPosition();

		Quest_Text[0]->Set_FontObjectText(L"차원문 개방하기.");
		POPUP_DescFont->Set_FontObjectText(L"차원문 개방");
		MissionSequence[1] = true;
	}
	else if (!MissionSequence[2] && KEY_DOWN(DIK_F)) {		// 차원문 열기
		static_pointer_cast<Portal>(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Get_GameObject("Portal"))->Activate_Portal();
		Quest_Text[0]->Set_FontObjectText(L"보스 던전 입장하기.");
		POPUP_DescFont->Set_FontObjectText(L"보스 던전 입장");
		MissionSequence[2] = true;
	}
	else if (!MissionSequence[3] && KEY_DOWN(DIK_F)) {		// 차원문 입장
		static_pointer_cast<Portal>(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Get_GameObject("Portal"))->Enter_BossDungeon();
		POPUP_DescFont->Set_FontObjectText(L"");
		MissionSequence[3] = true;
	}
}

unique_ptr<PopUpUI>	PopUpUI::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto	Instance = unique_ptr<PopUpUI>(new PopUpUI(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create PopUpUI.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>	PopUpUI::Clone(VOID* _ARG) {
	auto	Instance = unique_ptr<PopUpUI>(new PopUpUI(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone PopUpUI.");
		return nullptr;
	}
	return Instance;
}