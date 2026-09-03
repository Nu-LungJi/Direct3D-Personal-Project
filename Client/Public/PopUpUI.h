#pragma once
#include "GameObject.h"

class PopUpUI : public GameObject {
private:
	PopUpUI(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	PopUpUI(CONST PopUpUI& _PRTOBJ);
public:
	virtual ~PopUpUI();

public:
	virtual HRESULT		Initialize_ProtoType()			override;
	virtual HRESULT		Initialize(VOID* _ARG)			override;
	virtual VOID		Update(CONST _float& _DT)		override;
	virtual VOID		Late_Update(CONST _float& _DT)	override;
	virtual HRESULT		Render()						override;

	shared_ptr<UIObject>	Create_UIObject(filesystem::path _TexRes, _float2 _Position, _float _Scale, _float _OPC, _float _Rotation);
	shared_ptr<FontObject>	Create_FontObject(const wstring& _FontFilePath, wstring _Text, _float2 _Position, _float _Scale, _float3 _Color, _float _Alpha);

	VOID	PopUp_InteractionUI(CONST _float& _DT);
	VOID	PopDown_InteractionUI(CONST _float& _DT);

	VOID	Set_TargetPosition(XMVECTOR _Position)		{ TargetPosition = _Position; }
	VOID	Set_DescriptionText(const wstring& _Text)	{ POPUP_DescFont->Set_FontObjectText(_Text); }

	VOID	Update_MissionSequence_Scene_01();

public:
	uint32_t	Get_GuideDistance() { return GuideDistance; }
public:
	static	unique_ptr<PopUpUI>		Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>	Clone(VOID* _ARG)	override;

private:
	shared_ptr<SpriteBatch>		SPRBatch				= { nullptr };
	shared_ptr<CommonStates>	FontState				= { nullptr };

	shared_ptr<UIObject>		POPUP_NCBackGround		= { nullptr };
	shared_ptr<UIObject>		POPUP_CBackGround		= { nullptr };

	shared_ptr<UIObject>		POPUP_NCMarkIcon		= { nullptr };
	shared_ptr<UIObject>		POPUP_CMarkIcon			= { nullptr };

	shared_ptr<UIObject>		POPUP_KeyIcon			= { nullptr };
	shared_ptr<FontObject>		POPUP_DescFont			= { nullptr };

	shared_ptr<UIObject>		POPUP_GuideBackGround	= { nullptr };
	shared_ptr<UIObject>		POPUP_GuideIcon			= { nullptr };
	shared_ptr<FontObject>		POPUP_GuideDistance		= { nullptr };

	shared_ptr<Player>			GamePlayer				= { nullptr };
	uint32_t					GuideDistance			= { 0 };

	XMVECTOR					TargetPosition = {};
	XMVECTOR					CurrentGuideIconPosition = {};

	_bool						MissionSequence[4] = { false };
	shared_ptr<FontObject>			Quest_Text[2];
	vector<shared_ptr<UIObject>>	UIObjectList;
	vector<shared_ptr<FontObject>>	FontObjectList;

	_float	TimeAccumulation = { 0.f };
	_bool	InRanged = { false };
	shared_ptr<EventUI>			EventUIObject = { nullptr };
};

