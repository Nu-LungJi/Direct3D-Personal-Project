#pragma once
#include "GameObject.h"

BEGIN(Engine)
class ENGINE_DLL FontObject : public GameObject {
private:
	FontObject(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	FontObject(CONST FontObject& _PRTOBJ);
public:
	virtual ~FontObject() = default;

public:
	HRESULT		Render(shared_ptr<SpriteBatch> _BATCH);

	HRESULT		Create_FontObject(const wstring& _FileName, wstring _Text, _float2 _Position, _float _Scale, _float3 _Color, _float _Alpha);

	VOID		Set_FontObjectPosition(const XMFLOAT2& _Pos) { FontPosition = _Pos; }
	XMFLOAT2	Get_FontObjectPosition()					 { return FontPosition; }

	VOID		Set_FontObjectScale(const _float& _Sca)		{ FontScale = _Sca; }
	_float		Get_FontObjectScale()						{ return FontScale; }

	VOID		Set_FontObjectText(const wstring& _Text)	{ FontText = _Text;   }

	VOID		Set_FontObjectAlpha(_float _Alpha)			{ FontAlpha = _Alpha; }
	_float		Get_FontObjectAlpha()						{ return FontAlpha; }


	_bool		Get_Visibility()							{ return Visibility; }
	VOID		Set_Visibility(_bool _VSB)					{ Visibility = _VSB; }

	VOID		Set_FontObjectColor(XMFLOAT3 _Color)		{ FontColor = _Color; }

public:
	static		unique_ptr<FontObject>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual		shared_ptr<GameObject>	Clone(VOID* _ARG) override;


private:
	shared_ptr<SpriteFont>		FontOBJ		= { nullptr };

	wstring		FontText = { L"" };
	XMFLOAT2	FontPosition;
	XMFLOAT2	FontOriginScale;
	_float		FontScale;
	XMFLOAT3	FontColor;
	_float		FontAlpha = { 0.f };
	XMFLOAT2	FontOrigin = { 0.f, 0.f };
	_bool		Visibility = { true };
};
END