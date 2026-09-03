#include "../Default/PCH.h"
#include "MainUI.h"

MainUI::MainUI(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) {}
MainUI::MainUI(CONST MainUI& _PRTOBJ) : GameObject(_PRTOBJ) {}
MainUI::~MainUI()	{}

HRESULT MainUI::Initialize_ProtoType() {
	
	return S_OK;
}

HRESULT MainUI::Initialize(VOID* _ARG) {

	memset(Skill_TimerActivation, false, sizeof(_bool) * static_cast<uint32_t>(SKILL::SKL_END));

	Add_UIObjectBundle();
	Add_FontObjectBundle();

	SPRBatch  = GameInstance::GetInstance().Get_SpriteBatch();
	FontState = GameInstance::GetInstance().Get_CommonState();

	GamePlayer = static_pointer_cast<Player>(GameInstance::GetInstance().Get_GamePlayer());

	POPUPUI = static_pointer_cast<PopUpUI>(GameInstance::GetInstance().Get_SceneManager()->Get_CurrentScene()->Get_GameObject("PopUpUI"));

	return S_OK;
}
VOID	MainUI::Update(CONST _float& _DT) {
	//Update_MouseCursor();
	
	Update_CoolTimer(_DT);
	Update_MapResource();
	Update_FilterUI(_DT);
	for (auto& OBJ : UIObjectList) 
		OBJ->Update(_DT);
}
VOID MainUI::Late_Update(CONST _float& _DT) {
	for (auto& OBJ : UIObjectList) {
		OBJ->Late_Update(_DT);
	}
	for (auto& FOBJ : FontObjectList) {
		FOBJ->Late_Update(_DT);
	}
	GameInstance::GetInstance().Get_RenderManager()->Add_GameObject(RENDER_TYPE::RENDER_UI, shared_from_this());
}
HRESULT MainUI::Render() {
	DEVCTX->GSSetShader(nullptr, nullptr, 0);
	SPRBatch->Begin(DX11::SpriteSortMode_Deferred, FontState->NonPremultiplied());

	for (auto& UOBJ : UIObjectList)
		UOBJ->Render(SPRBatch);

	for (auto& FOBJ : FontObjectList)
		FOBJ->Render(SPRBatch);

	for (auto& SOBJ : FilterObjectList)
		SOBJ->Render(SPRBatch);

	SPRBatch->End();
	
	return S_OK;
}

HRESULT MainUI::Add_UIObjectBundle() {
	EngineOption = GameInstance::GetInstance().Get_GraphicDevice()->Get_EngineDescription();
	_float2		WindowCenterPos = { EngineOption.WindowResolutionX / 2, EngineOption.WindowResolutionY / 2 }; 

	{	// Add UIObject
		Create_UIObject("../../Resource/Asset/MainUI/Character_RoleHaed.png", { 1251.f, 236.f }, 0.25f, 1.f);
		Create_UIObject("../../Resource/Asset/MainUI/KEY_R.png", { 1240.f, 710.f }, 0.45f, 1.f);
		Create_UIObject("../../Resource/Asset/MainUI/KEY_Q.png", { 1160.f, 710.f }, 0.45f, 1.f);
		Create_UIObject("../../Resource/Asset/MainUI/KEY_E.png", { 1080.f, 710.f }, 0.45f, 1.f);
		Create_UIObject("../../Resource/Asset/MainUI/KEY_T.png", { 1000.f, 710.f }, 0.45f, 1.f);
		Create_UIObject("../../Resource/Asset/MainUI/MOUSE.png", { 917.f , 700.f }, 0.45f, 1.f);

		Create_UIObject("../../Resource/Asset/MainUI/Common_Icon01.png", { 905.f , 51.f }, 0.14f, 1.f);
		Create_UIObject("../../Resource/Asset/MainUI/Common_Icon02.png", { 980.f , 49.f }, 0.125f, 1.f);
		Create_UIObject("../../Resource/Asset/MainUI/Common_Icon03.png", { 1055.f, 50.f }, 0.14f, 1.f);
		Create_UIObject("../../Resource/Asset/MainUI/Common_Icon04.png", { 1125.f, 52.f }, 0.14f, 1.f);
		Create_UIObject("../../Resource/Asset/MainUI/Common_Icon05.png", { 1200.f, 54.f }, 0.40f, 1.f);
		Create_UIObject("../../Resource/Asset/MainUI/Common_Icon06.png", { 1255.f, 50.f }, 0.40f, 1.f);

		Create_UIObject("../../Resource/Asset/MainUI/MAP_Circle.png", { 149.f, 145.f }, 0.5f, 1.f);
		MapArrow = Create_UIObject("../../Resource/Asset/MainUI/MAP_Arrow.png ", { 101.f, 96.f }, 0.4f, 1.f);
		Create_UIObject("../../Resource/Asset/MainUI/MAP_Icon01.png", { 40.f, 165.f }, 0.5f, 1.f);

		Create_UIObject("../../Resource/Asset/MainUI/QUE_Icon.png", { 50.f, 220.f }, 0.35f, 1.f);

		Create_UIObject("../../Resource/Asset/MainUI/SkillIcon_Circle.png", { 1265.f, 690.f }, 1.f, 1.f);
		Create_UIObject("../../Resource/Asset/MainUI/SkillIcon_Circle.png", { 1185.f, 690.f }, 1.f, 1.f);
		Create_UIObject("../../Resource/Asset/MainUI/SkillIcon_Circle.png", { 1105.f, 690.f }, 1.f, 1.f);
		Create_UIObject("../../Resource/Asset/MainUI/SkillIcon_Circle.png", { 1025.f, 690.f }, 1.f, 1.f );
		Create_UIObject("../../Resource/Asset/MainUI/SkillIcon_Circle.png", { 930.f, 685.f }, 0.7f, 1.f );

		Skill_Icon[0] = Create_UIObject("../../Resource/Asset/MainUI/SkillIcon_T.png", { 1018.f, 686.f }, 0.50f, 1.f);
		Skill_Icon[1] = Create_UIObject("../../Resource/Asset/MainUI/SkillIcon_E.png", { 1096.f, 684.f }, 0.45f, 1.f);
		Skill_Icon[2] = Create_UIObject("../../Resource/Asset/MainUI/SkillIcon_Q.png", { 1176.f, 681.f }, 0.45f, 1.f);
		Skill_Icon[3] = Create_UIObject("../../Resource/Asset/MainUI/SkillIcon_R.png", { 1256.f, 681.f }, 0.45f, 1.f);
		Create_UIObject("../../Resource/Asset/MainUI/SkillIcon_Mouse.png", { 928.f, 683.f }, 0.4f, 1.f);

		Energy_BoostBar = Create_UIObject("../../Resource/Asset/MainUI/Energy_BoostBar.png", { 775.f, 670.f }, 0.6f, 1.f);
		Create_UIObject("../../Resource/Asset/MainUI/PlayerBackBar.png", { 760.f, 695.f }, 0.7f, 0.8f);
		Create_UIObject("../../Resource/Asset/MainUI/PlayerFrontBar.png", { 760.f, 695.f }, 0.7f, 0.8f);

		Create_MouseCursor();
	}

	uint32_t CurrentSceneIndex = GameInstance::GetInstance().Get_CurrentSceneIndex();

	for (auto& OBJ : UIObjectList)
		GameInstance::GetInstance().Get_SceneManager()->Add_GameObject(CurrentSceneIndex, (uint32_t)LAYER_TYPE::LAYER_UI, OBJ);

	return S_OK;
}

HRESULT MainUI::Add_FontObjectBundle() {

	Skill_Text[0] = Create_FontObject(L"../../Resource/Asset/Font/WutheringWave_Font_Bold.spritefont", L"0.0", { 1200.f + 22.f, 640.f + 12.f }, 0.45f, { 1.f, 1.f, 1.f }, 1.f);
	Skill_Text[1] = Create_FontObject(L"../../Resource/Asset/Font/WutheringWave_Font_Bold.spritefont", L"0.0", { 1120.f + 22.f, 640.f + 12.f }, 0.45f, { 1.f, 1.f, 1.f }, 1.f);
	Skill_Text[2] = Create_FontObject(L"../../Resource/Asset/Font/WutheringWave_Font_Bold.spritefont", L"0.0", { 1040.f + 22.f, 640.f + 12.f }, 0.45f, { 1.f, 1.f, 1.f }, 1.f);
	Skill_Text[3] = Create_FontObject(L"../../Resource/Asset/Font/WutheringWave_Font_Bold.spritefont", L"0.0", { 960.f  + 22.f, 640.f + 12.f }, 0.45f, { 1.f, 1.f, 1.f }, 1.f);
	Create_FontObject(L"../../Resource/Asset/Font/WutheringWave_Font_Bold.spritefont", L"6168 / 6168", { 635.f, 684.f }, 0.3f, { 0.f, 0.f, 0.f }, 1.f);
	Create_FontObject(L"../../Resource/Asset/Font/WutheringWave_Font_Bold.spritefont", L"Lv. 50", { 785.f, 685.f }, 0.4f, { 1.f, 1.f, 1.f }, 1.f);

	return S_OK;
}

shared_ptr<FontObject> MainUI::Create_FontObject(const wstring& _FontFilePath, wstring _Text, _float2 _Position, _float _Scale, _float3 _Color, _float _Alpha) {
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

shared_ptr<UIObject> MainUI::Create_UIObject(filesystem::path _TexRes, _float2 _Position, _float _Scale, _float _OPC, _float _Rotation) {
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
shared_ptr<UIObject> MainUI::Create_FilterObject(filesystem::path _TexRes, _float2 _Position, _float _Scale, _float _OPC, _float _Rotation){
	uint32_t CurrentSceneIndex = GameInstance::GetInstance().Get_CurrentSceneIndex();
	string	NewTag = _TexRes.filename().string();

	shared_ptr<UIObject> CLNOBJ = static_pointer_cast<UIObject>(GameInstance::GetInstance().Get_ProtoManager()->Clone_ProtoType(CurrentSceneIndex, "PRT_UIObject", NewTag, nullptr));

	if (FAILED(CLNOBJ->Create_UIObject(_TexRes, _Position, _Scale, _OPC, _Rotation))) {
		MSG_BOX("Cannot Ready UIObject TextureState.");
		return nullptr;
	}

	FilterObjectList.push_back(CLNOBJ);
	GameInstance::GetInstance().Get_GUIEditor()->Register_UIObject(CLNOBJ);

	return FilterObjectList.back();
	
}
HRESULT MainUI::Create_MouseCursor() {
	if (nullptr == Create_UIObject("../../Resource/Asset/MainUI/CursorNor.png", { 0.f, 0.f }, 0.4f, 1.f)) {
		MSG_BOX("Invalid MouseCursor Texture.");
		return E_FAIL;
	}
	MouseCursor = UIObjectList.back();
	MouseCursorOffset = { 12, 15 };

	return S_OK;
}
VOID MainUI::Update_MouseCursor() {
	POINT MousePointer{ 0, 0 };
	GetCursorPos(&MousePointer);
	ScreenToClient(hWnd, &MousePointer);

	MousePointer = { MousePointer.x + MouseCursorOffset.x, MousePointer.y + MouseCursorOffset.y };

	static_pointer_cast<Transform>(MouseCursor->Find_Component(COMPONENT_TYPE::COMPONENT_TRANSFORM))->Set_WorldTransform(VECTOR_TYPE::VECTOR_POS,
		XMVectorSet(MousePointer.x - EngineOption.WindowResolutionX * 0.5f, EngineOption.WindowResolutionY * 0.5f - MousePointer.y, 0.f, 1.f));
}

VOID MainUI::Update_MapResource() {
	XMVECTOR PlayerRotQuat = GamePlayer->Get_TransformComponent()->Get_WorldRotationQuat();

	XMFLOAT4X4 PlayerRotMat;
	XMStoreFloat4x4(&PlayerRotMat, XMMatrixRotationQuaternion(PlayerRotQuat));

	_float PlayerRotYaw = atan2(PlayerRotMat._31, PlayerRotMat._33);
	
	MapArrow->Set_Rotation(PlayerRotYaw);
}

VOID MainUI::Activate_CoolTimer(SKILL _SKL, _float _Time) {
	Skill_Time[static_cast<uint32_t>(_SKL)] = _Time;
	Skill_TimerActivation[static_cast<uint32_t>(_SKL)] = true;
}

VOID MainUI::Update_CoolTimer(CONST _float& _DT) {
	for (uint32_t IDX = 0; IDX < static_cast<uint32_t>(SKILL::SKL_END); ++IDX) {
		if (Skill_TimerActivation[IDX]) {
			Skill_Time[IDX] -= _DT;
			wstringstream wss;
			wss << fixed << setprecision(1) << trunc(Skill_Time[IDX] * 10) / 10;

			Skill_Text[IDX]->Set_FontObjectText(wss.str());

			if (Skill_Time[IDX] <= 0.f) {
				Skill_Text[IDX]->Set_FontObjectText(L"");
				Skill_Time[IDX] = 0.1f;
				Skill_TimerActivation[IDX] = false;
			}
		}
	}
}

VOID MainUI::Update_FilterUI(const _float& _DT) {
	if (FadeOutTime > 0.f) {
		CurrentTime += _DT;
		Filter_FadeOut->Set_Alpha(CurrentTime / FadeOutTime);
		if (CurrentTime > FadeOutTime) { CurrentTime = 0.f; FadeOutTime = 0.f; }
	}
	if (FadeInTime > 0.f) {
		CurrentTime -= _DT;
		Filter_FadeOut->Set_Alpha(CurrentTime / FadeInTime);
		if (CurrentTime <= 0.f) { CurrentTime = 0.f; FadeInTime = 0.f; }
	}
}
unique_ptr<MainUI>		MainUI::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto	Instance = unique_ptr<MainUI>(new MainUI(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create MainUI.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>  MainUI::Clone(VOID* _ARG) {
	auto	Instance = unique_ptr<MainUI>(new MainUI(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone MainUI.");
		return nullptr;
	}
	return Instance;
}
