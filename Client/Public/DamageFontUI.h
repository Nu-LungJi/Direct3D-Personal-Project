#pragma once
#include "GameObject.h"

struct DamageFont {
	_float		CurrentLifeTime  = { 0.f };
	_float		MaxLifeTime		 = { 1.2f };

	_bool		RenderActivation = { false };

	XMFLOAT2	ScreenOffset;
	_float		FontScale;
	_float		FontAlpha;
	XMFLOAT3	WorldPosition;

	uint32_t	DamageValue		 = { 0 };
	wstring		DamageString	 = {};
	XMFLOAT2	ScreenPosition;
};

class DamageFontUI : public GameObject {
private:
	DamageFontUI(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	DamageFontUI(CONST DamageFontUI& _PRTOBJ);
public:
	virtual ~DamageFontUI();

public:
	virtual HRESULT		Initialize_ProtoType()			override;
	virtual HRESULT		Initialize(VOID* _ARG)			override;
	virtual VOID		Update(CONST _float& _DT)		override;
	virtual VOID		Late_Update(CONST _float& _DT)	override;
	virtual HRESULT		Render()						override;

public:
	VOID	Generate_DamageFont(XMVECTOR _WorldPos, uint32_t _Damage);

public:
	static	unique_ptr<DamageFontUI>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>		Clone(VOID* _ARG) override;

private:
	shared_ptr<UIObject>					OriginalTexture[10];
	vector<shared_ptr<DamageFont>>			DamageFontTexturePool;
	unordered_set<shared_ptr<DamageFont>>	RenderDamageFont;

	shared_ptr<SpriteBatch>		SPRBatch = { nullptr };
	shared_ptr<CommonStates>	FontState = { nullptr };

	uint32_t	LastAllocatedIndex = { 0 };
};

