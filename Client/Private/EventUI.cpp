#include "../Default/PCH.h"
#include "GameInstance.h"

EventUI::EventUI(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) {}
EventUI::EventUI(CONST EventUI& _PRTOBJ) : GameObject(_PRTOBJ) {}

HRESULT EventUI::Initialize_ProtoType() {


	return S_OK;
}
HRESULT EventUI::Initialize(VOID* _ARG) {

	SPRBatch = GameInstance::GetInstance().Get_SpriteBatch();
	UIState = GameInstance::GetInstance().Get_CommonState();

	//Create_UIObject(L"../../Resource/Asset/MainUI/EventUI_Filter.png", { 1280.f, 720.f }, 1.f, 0.8f, 0.f);
	//Create_FontObject(L"../../Resource/Asset/Font/WutheringWave_Font_Bold.spritefont", L"¡ÿ∫Ò ¡ﬂ¿‘¥œ¥Ÿ.", { 640.f , 360.f  }, 0.8f, { 1.f, 1.f, 1.f }, 1.f);

	IPU_Core = Create_UIObject(L"../../Resource/Asset/MainUI/ItemPopUp.png", { 670.f, 400.f }, 0.3f, 0.f, 0.f);
	IPU_MessageBack = Create_UIObject(L"../../Resource/Asset/MainUI/ItemPopUpMessage.png", { 668.f, 330.f }, 0.35f, 0.f, 0.f);

	IPU_Text = Create_FontObject(L"../../Resource/Asset/Font/WutheringWave_Font_Bold.spritefont", L"»πµÊ", { 640.f , 316.f }, 0.4f, { 1.f, 1.f, 1.f }, 0.f);

	return S_OK;
}
VOID EventUI::Update(const _float& _DT) {
	if (nullptr == POPUPUI) {
		POPUPUI = static_pointer_cast<PopUpUI>(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Get_GameObject("PopUpUI"));
	}

	if (EventActivation) {
		Update_ItemPopUp(_DT);
	}
	
}
VOID EventUI::Late_Update(const _float& _DT) {
	if (EventActivation) {
		GameInstance::GetInstance().Get_RenderManager()->Add_GameObject(RENDER_TYPE::RENDER_UI, shared_from_this());
	}
}
HRESULT EventUI::Render() {
	if (EventActivation) {
		DEVCTX->GSSetShader(nullptr, nullptr, 0);
		SPRBatch->Begin(DX11::SpriteSortMode_Immediate, UIState->NonPremultiplied());
		for (auto& UOBJ : UIObjectList)
			UOBJ->Render(SPRBatch);

		for (auto& FOBJ : FontObjectList)
			FOBJ->Render(SPRBatch);

		SPRBatch->End();
	}
	
	return S_OK;
}
VOID EventUI::Update_ItemPopUp(const _float& _DT) {
	_float DelayTime = 0.25f, PopUpTime = 0.4f, Duration = 1.5f, PopDownTime = 0.6f;

	TotalTimeAccumulation += _DT;

	if (TotalTimeAccumulation <= DelayTime + PopUpTime) {
		_float Weight = EASEOUT(TotalTimeAccumulation / (DelayTime + PopUpTime), 3.f);

		_float MaxAlpha = 0.9f;
		_float CurrentAlpha = Weight * MaxAlpha;

		IPU_Core->Set_Alpha(CurrentAlpha);
		IPU_MessageBack->Set_Alpha(CurrentAlpha);
		IPU_Text->Set_FontObjectAlpha(CurrentAlpha);

		if (IPU_Core->Get_Position().y >= 400.f) {
			_float CurrentY = LERP(500.f, 400.f, Weight);

			IPU_Core->Set_Position({ IPU_Core->Get_Position().x , CurrentY });
			IPU_MessageBack->Set_Position({ IPU_MessageBack->Get_Position().x , CurrentY - 70.f });
			IPU_Text->Set_FontObjectPosition({ IPU_Text->Get_FontObjectPosition().x , CurrentY - 80.f });
		}
	}
	else if (TotalTimeAccumulation <= DelayTime + PopUpTime + Duration) {

	}
	else if (TotalTimeAccumulation <= DelayTime + PopUpTime + Duration + PopDownTime) {
		_float CurrentTime = TotalTimeAccumulation - (DelayTime + PopUpTime + Duration);
		_float Weight = SMOOTHSTEP(CurrentTime / PopDownTime);

		_float MaxAlpha = 1.f;
		_float CurrentAlpha = 1.f - Weight * MaxAlpha;

		IPU_Core->Set_Alpha(CurrentAlpha);
		IPU_MessageBack->Set_Alpha(CurrentAlpha);
		IPU_Text->Set_FontObjectAlpha(CurrentAlpha);

		if (IPU_Core->Get_Position().y <= 550.f) {
			_float CurrentY = LERP(400.f, 550.f, Weight);

			IPU_Core->Set_Position({ IPU_Core->Get_Position().x , CurrentY });
			IPU_MessageBack->Set_Position({ IPU_MessageBack->Get_Position().x , CurrentY - 70.f });
			IPU_Text->Set_FontObjectPosition({ IPU_Text->Get_FontObjectPosition().x , CurrentY - 80.f });
		}
	}
	else {
		EventActivation = false;
		TotalTimeAccumulation = 0.f;
	}
}

shared_ptr<UIObject>	EventUI::Create_UIObject(filesystem::path _TexRes, _float2 _Position, _float _Scale, _float _OPC, _float _Rotation) {
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
shared_ptr<FontObject>	EventUI::Create_FontObject(const wstring& _FontFilePath, wstring _Text, _float2 _Position, _float _Scale, _float3 _Color, _float _Alpha) {
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

unique_ptr<EventUI>		EventUI::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<EventUI>(new EventUI(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create EventUI.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>	EventUI::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<EventUI>(new EventUI(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone EventUI.");
		return nullptr;
	}
	return Instance;
}