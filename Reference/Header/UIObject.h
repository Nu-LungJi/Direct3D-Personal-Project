#pragma once
#include "GameObject.h"
#include "Transform.h"
#include "Buffer.h"
#include "Texture.h"

BEGIN(Engine)

struct UIInfo {
	UIInfo() {};
	UIInfo(XMFLOAT2 _Pos, _float _Sca, _float _Alpha = 1.f, _float _Rotation = 0.f) : UIPosition{ _Pos }, UIScale(_Sca), UIAlpha(_Alpha), UIRotation(_Rotation){}

	XMFLOAT2 UIPosition;
	_float	 UIScale;
	XMFLOAT2 UIOriginScale;
	_float	 UIAlpha;
	XMFLOAT2 UIOrigin = { 0.f, 0.f };
	_float	 UIRotation = 0.f;
};

class ENGINE_DLL UIObject : public GameObject {
protected:
	UIObject(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	UIObject(CONST UIObject& _PRTOBJ);
public:
	virtual ~UIObject();

public:
	virtual HRESULT		Initialize(VOID* _ARG)			override;
	virtual VOID		Update(CONST _float& _DT)		override;
	virtual VOID		Late_Update(CONST _float& _DT)	override;
	HRESULT				Render(shared_ptr<SpriteBatch> _BATCH);

	HRESULT Bind_Texture(const wstring& _TEX);
	HRESULT Create_UIObject(const wstring& _TEX, _float2 _Position, _float _Scale, _float _OPC, _float _Rotation = 0.f);

public:
	XMFLOAT2			Get_Position() { return UINFO->UIPosition; }
	VOID				Set_Position(XMFLOAT2 _Pos) { UINFO->UIPosition = _Pos; }

	_float				Get_Scale() { return UINFO->UIScale; }
	VOID				Set_Scale(_float _Sca) { UINFO->UIScale = _Sca; }

	_float				Get_Alpha() { return UINFO->UIAlpha; }
	VOID				Set_Alpha(_float _Alp) { UINFO->UIAlpha = _Alp; }

	_bool				Get_Visibility()			{ return Visibility; }
	VOID				Set_Visibility(_bool _VSB)	{ Visibility = _VSB; }

	_float				Get_Rotation()				{ return UINFO->UIRotation; }
	VOID				Set_Rotation(_float _Rot)	{ UINFO->UIRotation = _Rot; }

	ComPtr<ID3D11ShaderResourceView>& Get_SRV() { return SRV; }
	shared_ptr<UIInfo>	Get_UIInfo() { return UINFO; }

public:
	static	unique_ptr<UIObject>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>	Clone(VOID* _ARG);

private:
	ComPtr<ID3D11ShaderResourceView>    SRV = { nullptr };
	shared_ptr<UIInfo>  UINFO = { nullptr };
	_bool				Visibility = { true };
};
END