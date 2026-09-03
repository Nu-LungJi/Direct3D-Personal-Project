#include "../Default/PCH.h"
#include "GameInstance.h"

BossUI::BossUI(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) {}
BossUI::BossUI(CONST BossUI& _PRTOBJ) : GameObject(_PRTOBJ) {}

HRESULT BossUI::Initialize_ProtoType() {
	

	return S_OK;
}
HRESULT BossUI::Initialize(VOID* _ARG) {

	SPRBatch = GameInstance::GetInstance().Get_SpriteBatch();
	UIState = GameInstance::GetInstance().Get_CommonState();

	Create_FontObject(L"../../Resource/Asset/Font/WutheringWave_Font_ExtraBold.spritefont", L"Lv. 50  ½ºÄ«    ¾Ç¸ùÀÇ À¯·É", { 630.f , 30.f }, 0.42f, { 1.f, 1.f, 1.f }, 1.f);
	Create_FontObject(L"../../Resource/Asset/Font/WutheringWave_Font_ExtraBold.spritefont", L".", { 635.f , 20.f }, 0.72f, { 1.f, 1.f, 1.f }, 1.f);

	SRV1 = GameInstance::GetInstance().Get_TextureManager()->Load_Texture("../../Resource/Asset/MainUI/BossHP_BackBar.png");
	SRV2 = GameInstance::GetInstance().Get_TextureManager()->Load_Texture("../../Resource/Asset/MainUI/BossHP.png");

	uint32_t CurrentSceneIndex = GameInstance::GetInstance().Get_CurrentSceneIndex();

	Component_Shader = Add_Component<Shader>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_SCROLL);
	Component_Transform = Add_Component<Transform>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_TRANSFORM);
	Component_RectBuffer = Add_Component<RectBuffer>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_RECT_BUFFER);

	BackGroundInfo.UIScale = 0.4f;
	BackGroundInfo.UIPosition = { 640.f, 50.f };
	BackGroundInfo.UIRotation = { 0.f };
	BackGroundInfo.UIOrigin = { 0.f, 0.f };
	BackGroundInfo.UIAlpha = 0.f;

	return S_OK;
}
VOID BossUI::Update(const _float& _DT) {

}
VOID BossUI::Late_Update(const _float& _DT) {
	if (Activation) {
		GameInstance::GetInstance().Get_RenderManager()->Add_GameObject(RENDER_TYPE::RENDER_UI, shared_from_this());
	}
}
//HRESULT BossUI::Render() {
//	if (Activation) {
//		DEVCTX->GSSetShader(nullptr, nullptr, 0);
//		SPRBatch->Begin(DX11::SpriteSortMode_Immediate, UIState->NonPremultiplied());
//		for (auto& UOBJ : UIObjectList)
//			UOBJ->Render(SPRBatch);
//
//		for (auto& FOBJ : FontObjectList)
//			FOBJ->Render(SPRBatch);
//
//		SPRBatch->End();
//	}
//
//	return S_OK;
//}
HRESULT BossUI::Render() {
	BackGroundInfo.UIOriginScale = { 1000.f, 40.f };

	XMFLOAT2 UIOiriginScale = { BackGroundInfo.UIOriginScale.x, BackGroundInfo.UIOriginScale.y };
	XMFLOAT2 UIPosition = { BackGroundInfo.UIPosition.x, BackGroundInfo.UIPosition.y };
	_float	 UIScale = BackGroundInfo.UIScale;

	_float FinalWidth = UIOiriginScale.x * UIScale;
	_float FinalHeight = UIOiriginScale.y * UIScale;

	_float CenterOriginTLX = UIPosition.x - (FinalWidth / 2.f);
	_float CenterOriginTLY = UIPosition.y - (FinalHeight / 2.f);

	RECT HPBar_SrcArea = { 0, 0, (LONG)UIOiriginScale.x, (LONG)UIOiriginScale.y };
	RECT HPBar_DestArea = { (LONG)CenterOriginTLX, (LONG)CenterOriginTLY, (LONG)(CenterOriginTLX + FinalWidth), (LONG)(CenterOriginTLY + FinalHeight) };

	SPRBatch->Begin(DX11::SpriteSortMode_Immediate, UIState->NonPremultiplied());

	SPRBatch->Draw(SRV1.Get(), HPBar_DestArea, &HPBar_SrcArea, DirectX::Colors::White);

	for (auto& FOBJ : FontObjectList)
		FOBJ->Render(SPRBatch);

	SPRBatch->End();

	ENGINE_DESC Engine_Option = GameInstance::GetInstance().Get_EngineOption();

	_float SizeX = FinalWidth / (Engine_Option.WindowResolutionX * 0.5f);
	_float SizeY = FinalHeight / (Engine_Option.WindowResolutionY * 0.5f);

	BackGroundInfo.UIOrigin.x = CenterOriginTLX + (FinalWidth * 0.5f);
	BackGroundInfo.UIOrigin.y = CenterOriginTLY + (FinalHeight * 0.5f);

	_float NDC_X = (BackGroundInfo.UIOrigin.x / (Engine_Option.WindowResolutionX * 0.5f)) - 1.0f;
	_float NDC_Y = 1.0f - (BackGroundInfo.UIOrigin.y / (Engine_Option.WindowResolutionY * 0.5f));

	XMStoreFloat4x4(&WorldMat, XMMatrixScaling(SizeX, SizeY, 1.f) * XMMatrixTranslation(NDC_X, NDC_Y, 0.f));
	XMStoreFloat4x4(&ViewMat, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMat, XMMatrixIdentity());

	Component_Shader->Bind_Matrix("g_WorldMatrix", &WorldMat);
	Component_Shader->Bind_Matrix("g_ViewMatrix", &ViewMat);
	Component_Shader->Bind_Matrix("g_ProjMatrix", &ProjMat);

	Component_Shader->Bind_RawValue("CurrentHPPercentage", &CurrentHPPercentage, sizeof(_float));
	Component_Shader->Bind_ShaderResourceView("g_Texture", SRV2);

	Component_Shader->Shader_Begin(0);

	if (FAILED(Component_RectBuffer->Bind_Resources())) return S_OK;
	if (FAILED(Component_RectBuffer->Render_Buffer()))	return S_OK;

	return S_OK;
}
shared_ptr<UIObject>	BossUI::Create_UIObject(filesystem::path _TexRes, _float2 _Position, _float _Scale, _float _OPC, _float _Rotation) {
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
shared_ptr<FontObject>	BossUI::Create_FontObject(const wstring& _FontFilePath, wstring _Text, _float2 _Position, _float _Scale, _float3 _Color, _float _Alpha) {
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

unique_ptr<BossUI>		BossUI::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<BossUI>(new BossUI(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create BossUI.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>	BossUI::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<BossUI>(new BossUI(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone BossUI.");
		return nullptr;
	}
	return Instance;
}