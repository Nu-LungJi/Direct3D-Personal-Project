#pragma once
#include "GameObject.h"

class MainUI : public GameObject {
private:
	MainUI(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	MainUI(CONST MainUI& _PRTOBJ);
public:
	virtual ~MainUI();

	enum class SKILL { SKL_T, SKL_E, SKL_R, SKL_Q, SKL_EXTRA, SKL_END };

public:
	virtual HRESULT	Initialize_ProtoType()			override;
	virtual HRESULT	Initialize(VOID* _ARG)			override;
	virtual VOID	Update(CONST _float& _DT)		override;
	virtual VOID	Late_Update(CONST _float& _DT)	override;
	virtual HRESULT	Render()						override;

	HRESULT		Add_UIObjectBundle();
	HRESULT		Add_FontObjectBundle();

	shared_ptr<FontObject>			Create_FontObject(const wstring& _FontFilePath, wstring _Text, _float2 _Position, _float _Scale, _float3 _Color, _float _Alpha);
	shared_ptr<UIObject>			Create_UIObject(filesystem::path _TexRes, _float2 _Position, _float _Scale, _float _OPC, _float _Rotation = 0.f);
	shared_ptr<UIObject>			Create_FilterObject(filesystem::path _TexRes, _float2 _Position, _float _Scale, _float _OPC, _float _Rotation = 0.f);

	HRESULT		Create_MouseCursor();
	VOID		Update_MouseCursor();

	VOID		Update_MapResource();

	VOID		Update_CoolTimer(CONST _float& _DT);

	VOID		Activate_CoolTimer(SKILL _SKL, _float _Time);

	VOID		Enable_FadeOut(CONST _float& _Time) { FadeOutTime = _Time; CurrentTime = 0.f; }
	VOID		Enable_FadeIn(CONST _float& _Time)  { CurrentTime = FadeInTime = _Time; }

	VOID		Update_FilterUI(CONST _float& _DT);

	VOID		Update_SceneTransition(CONST _float& _DT);

public:
	static	unique_ptr<MainUI>		Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>	Clone(VOID* _ARG) override;

private:
	ENGINE_DESC EngineOption = {};

	vector<shared_ptr<UIObject>>	UIObjectList;
	vector<shared_ptr<FontObject>>	FontObjectList;
	vector<shared_ptr<UIObject>>	FilterObjectList;

	shared_ptr<UIObject>		MouseCursor = { nullptr };
	POINT						MouseCursorOffset = { 0, 0 };

	shared_ptr<SpriteBatch>		SPRBatch = { nullptr };
	shared_ptr<CommonStates>	FontState = { nullptr };

	_bool					Skill_TimerActivation[static_cast<uint32_t>(SKILL::SKL_END)];
	_float					Skill_Time[static_cast<uint32_t>(SKILL::SKL_END)];
	shared_ptr<UIObject>	Skill_Icon[static_cast<uint32_t>(SKILL::SKL_END)];
	shared_ptr<FontObject>	Skill_Text[static_cast<uint32_t>(SKILL::SKL_END)];

	shared_ptr<UIObject>	MapArrow = { nullptr };

	shared_ptr<UIObject>	Energy_NormalBar = { nullptr };
	shared_ptr<UIObject>	Energy_BoostBar = { nullptr };
	shared_ptr<UIObject>	Energy_UnitBundle[4] = { nullptr };

	shared_ptr<Player>	GamePlayer = { nullptr };

	shared_ptr<PopUpUI>	POPUPUI = { nullptr };

	shared_ptr<UIObject>	Filter_FadeOut = { nullptr };
	_float					FadeOutTime = { 0.f }, FadeInTime = { 0.f }, CurrentTime = { 0.f };

};

