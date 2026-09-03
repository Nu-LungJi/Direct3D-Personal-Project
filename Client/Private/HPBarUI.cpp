#include "../Default/PCH.h"
#include "GameInstance.h"

HPBarUI::HPBarUI(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GameObject(_GRPDEV, _DEVCTX) { }
HPBarUI::HPBarUI(const HPBarUI& _PRTOBJ) : GameObject(_PRTOBJ) { }


HRESULT HPBarUI::Initialize_ProtoType() {
	SPRBatch = GameInstance::GetInstance().Get_SpriteBatch();
	UIState = GameInstance::GetInstance().Get_CommonState();
	
	uint32_t CurrentSceneIndex = GameInstance::GetInstance().Get_CurrentSceneIndex();

	SRV1 = GameInstance::GetInstance().Get_TextureManager()->Load_Texture("../../Resource/Asset/MainUI/Monster_BackBar.png");
	SRV2 = GameInstance::GetInstance().Get_TextureManager()->Load_Texture("../../Resource/Asset/MainUI/Monster_FrontBar.png");

	Component_Shader		= Add_Component<Shader>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_SCROLL);
	Component_Transform		= Add_Component<Transform>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_TRANSFORM);
	Component_RectBuffer	= Add_Component<RectBuffer>(CurrentSceneIndex, COMPONENT_TYPE::COMPONENT_RECT_BUFFER);

	HPBarFont = static_pointer_cast<FontObject>(GameInstance::GetInstance().Get_ProtoManager()->Clone_ProtoType(GameInstance::GetInstance().Get_CurrentSceneIndex()
		, "PRT_FontObject", "LevelFont_0", nullptr));
	HPBarFont->Create_FontObject(L"../../Resource/Asset/Font/WutheringWave_Font_Bold.spritefont", L"Lv 81", { 0.f, 0.f }, 0.5f, { 1.f, 1.f, 1.f }, 1.f);

	return S_OK;
}

HRESULT HPBarUI::Initialize(VOID* _ARG) {
	Component_Transform		= static_pointer_cast<Transform>	 (ComponentList[static_cast<uint32_t>(COMPONENT_TYPE::COMPONENT_TRANSFORM)]);
	Component_RectBuffer	= static_pointer_cast<RectBuffer>	 (ComponentList[static_cast<uint32_t>(COMPONENT_TYPE::COMPONENT_RECT_BUFFER)]);
	Component_Shader		= static_pointer_cast<Shader>		 (ComponentList[static_cast<uint32_t>(COMPONENT_TYPE::COMPONENT_SCROLL)]);

	return S_OK;
}

VOID HPBarUI::Update(const _float& _DT) {

}

VOID HPBarUI::Late_Update(const _float& _DT) {
	GameInstance::GetInstance().Get_RenderManager()->Add_GameObject(RENDER_TYPE::RENDER_UI, shared_from_this());
}

shared_ptr<UIObject> HPBarUI::Create_UIObject(filesystem::path _TexRes, _float2 _Position, _float _Scale, _float _OPC, _float _Rotation) {
	uint32_t CurrentSceneIndex = GameInstance::GetInstance().Get_CurrentSceneIndex();
	string	NewTag = _TexRes.filename().string();

	shared_ptr<UIObject> CLNOBJ = static_pointer_cast<UIObject>(GameInstance::GetInstance().Get_ProtoManager()->Clone_ProtoType(CurrentSceneIndex, "PRT_UIObject", NewTag, nullptr));

	if (FAILED(CLNOBJ->Create_UIObject(_TexRes, _Position, _Scale, _OPC, _Rotation))) {
		MSG_BOX("Cannot Ready UIObject TextureState.");
		return nullptr;
	}

	UIObjectList.push_back(CLNOBJ);

	return UIObjectList.back();
}

HRESULT HPBarUI::Render() {
	BackGroundInfo.UIOriginScale = { 700.f, 58.f };

	XMFLOAT2 UIOiriginScale = { BackGroundInfo.UIOriginScale.x, BackGroundInfo.UIOriginScale.y };
	XMFLOAT2 UIPosition = { BackGroundInfo.UIPosition.x, BackGroundInfo.UIPosition.y };
	_float	 UIScale = BackGroundInfo.UIScale;

	_float FinalWidth = UIOiriginScale.x * UIScale;
	_float FinalHeight = UIOiriginScale.y * UIScale;

	_float CenterOriginTLX = UIPosition.x - (FinalWidth / 2.f);
	_float CenterOriginTLY = UIPosition.y - (FinalHeight / 2.f);

	RECT HPBar_SrcArea = { 0, 0, (LONG)UIOiriginScale.x, (LONG)UIOiriginScale.y };
	RECT HPBar_DestArea = { (LONG)CenterOriginTLX, (LONG)CenterOriginTLY, (LONG)(CenterOriginTLX + FinalWidth), (LONG)(CenterOriginTLY + FinalHeight) };

	DEVCTX->GSSetShader(nullptr, nullptr, 0);
	SPRBatch->Begin(DirectX::SpriteSortMode_Immediate, UIState->NonPremultiplied());
	SPRBatch->Draw(SRV1.Get(), HPBar_DestArea, &HPBar_SrcArea, DirectX::Colors::White);
	HPBarFont->Render(SPRBatch);
	//SPRBatch->Draw(SRV1.Get(), bgDestRect, &bgSrcRect, DirectX::Colors::White);
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

shared_ptr<HPBarUI>		HPBarUI::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = shared_ptr<HPBarUI>(new HPBarUI(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Initialize_ProtoType())) {
		MSG_BOX("Cannot Create HPBarUI.");
		return nullptr;
	}
	return Instance;
}
shared_ptr<GameObject>	HPBarUI::Clone(VOID* _ARG) {
	auto Instance = shared_ptr<HPBarUI>(new HPBarUI(*this));
	if (FAILED(Instance->Initialize(_ARG))) {
		MSG_BOX("Cannot Clone HPBarUI.");
		return nullptr;
	}
	return Instance;
}