#pragma once
#include "GameObject.h"

class EventUI : public GameObject {
private:
	EventUI(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	EventUI(CONST EventUI& _PRTOBJ);
public:
	virtual ~EventUI() = default;

public:
	virtual HRESULT		Initialize_ProtoType()			override;
	virtual HRESULT		Initialize(VOID* _ARG)			override;
	virtual VOID		Update(CONST _float& _DT)		override;
	virtual VOID		Late_Update(CONST _float& _DT)	override;
	virtual HRESULT		Render()						override;

	shared_ptr<UIObject>	Create_UIObject(filesystem::path _TexRes, _float2 _Position, _float _Scale, _float _OPC, _float _Rotation);
	shared_ptr<FontObject>	Create_FontObject(const wstring& _FontFilePath, wstring _Text, _float2 _Position, _float _Scale, _float3 _Color, _float _Alpha);

	VOID				Set_EventActivation(_bool _Value)	{ EventActivation = _Value; }
	_bool				Get_EventActivation()				{ return EventActivation;	}

	VOID				Update_ItemPopUp(CONST _float& _DT);

public:
	static  unique_ptr<EventUI>		Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>	Clone(VOID* _ARG);

private:
	shared_ptr<SpriteBatch>		SPRBatch = { nullptr };
	shared_ptr<CommonStates>	UIState = { nullptr };

	shared_ptr<Transform>		Component_Transform = { nullptr };
	shared_ptr<Shader>			Component_Shader = { nullptr };
	shared_ptr<TexBuffer>		Component_Texture = { nullptr };
	shared_ptr<RectBuffer>		Component_RectBuffer = { nullptr };

	vector<shared_ptr<UIObject>>	UIObjectList;
	vector<shared_ptr<FontObject>>	FontObjectList;

	shared_ptr<class PopUpUI>		POPUPUI = { nullptr };


	_float TimeAccumulation = { 0.f }, TotalTimeAccumulation = { 0.f };

	shared_ptr<UIObject>	IPU_Core = { nullptr }, IPU_MessageBack = { nullptr };
	shared_ptr<FontObject>	IPU_Text = { nullptr };

	_bool					EventActivation = { false };
};

