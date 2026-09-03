#pragma once
#include "GameObject.h"
#include "UIObject.h"
class HPBarUI : public GameObject {
private:
	HPBarUI(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	HPBarUI(CONST HPBarUI& _PRTOBJ);
public:
	virtual ~HPBarUI() = default;

public:
	virtual HRESULT		Initialize_ProtoType()			override;
	virtual HRESULT		Initialize(VOID* _ARG)			override;
	virtual VOID		Update(CONST _float& _DT)		override;
	virtual VOID		Late_Update(CONST _float& _DT)	override;
	virtual HRESULT		Render()						override;

	shared_ptr<UIObject> Create_UIObject(filesystem::path _TexRes, _float2 _Position, _float _Scale, _float _OPC, _float _Rotation = 0.f);

	VOID				Set_Position(XMFLOAT2 _Position)	{ BackGroundInfo.UIPosition = _Position; }
	VOID				Set_Scale(_float _Scale)			{ BackGroundInfo.UIScale = _Scale;		 }

	VOID				Set_FontContent(wstring _Content)		{ HPBarFont->Set_FontObjectText(_Content); }

	VOID				Set_FontPosition(XMFLOAT2 _Position)	{ HPBarFont->Set_FontObjectPosition(_Position); }

	VOID				Set_FontScale(_float _Scale)			{ HPBarFont->Set_FontObjectScale(_Scale);  }
	_float				Get_FontScale()							{ return HPBarFont->Get_FontObjectScale(); }

	VOID				Set_CurrentHPPercentage(_float _HPPercentage) { CurrentHPPercentage = _HPPercentage; }

public:
	static  shared_ptr<HPBarUI>		Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>	Clone(VOID* _ARG);

private:
	shared_ptr<SpriteBatch>		SPRBatch			= { nullptr };
	shared_ptr<CommonStates>	UIState			= { nullptr };

	shared_ptr<Transform>		Component_Transform = { nullptr };
	shared_ptr<Shader>			Component_Shader	= { nullptr };
	shared_ptr<TexBuffer>		Component_Texture	= { nullptr };
	shared_ptr<RectBuffer>		Component_RectBuffer = { nullptr };

	vector<shared_ptr<UIObject>>	UIObjectList;
	ComPtr<ID3D11ShaderResourceView> SRV1, SRV2, SRV3; 

	shared_ptr<FontObject>		HPBarFont = { nullptr };

	UIInfo		BackGroundInfo;

	XMFLOAT4X4	WorldMat, ViewMat, ProjMat;

	_bool		RenderFlag = { true };

	_float		CurrentHPPercentage = { 0.f };
};

