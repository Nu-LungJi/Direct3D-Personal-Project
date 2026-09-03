#pragma once
#include "GameObject.h"
#include "UIObject.h"

class BossUI : public GameObject {
private:
	BossUI(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	BossUI(CONST BossUI& _PRTOBJ);
public:
	virtual ~BossUI() = default;

public:
	virtual HRESULT		Initialize_ProtoType()			override;
	virtual HRESULT		Initialize(VOID* _ARG)			override;
	virtual VOID		Update(CONST _float& _DT)		override;
	virtual VOID		Late_Update(CONST _float& _DT)	override;
	virtual HRESULT		Render()						override;

	shared_ptr<UIObject>	Create_UIObject(filesystem::path _TexRes, _float2 _Position, _float _Scale, _float _OPC, _float _Rotation);
	shared_ptr<FontObject>	Create_FontObject(const wstring& _FontFilePath, wstring _Text, _float2 _Position, _float _Scale, _float3 _Color, _float _Alpha);

	VOID				Set_CurrentHPPercentage(_float _HPPercentage) { CurrentHPPercentage = _HPPercentage; }

public:
	static  unique_ptr<BossUI>		Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>	Clone(VOID* _ARG);

private:
	shared_ptr<SpriteBatch>			SPRBatch			= { nullptr };
	shared_ptr<CommonStates>		UIState				= { nullptr };

	shared_ptr<Transform>			Component_Transform		= { nullptr };
	shared_ptr<Shader>				Component_Shader		= { nullptr };
	shared_ptr<TexBuffer>			Component_Texture		= { nullptr };
	shared_ptr<RectBuffer>			Component_RectBuffer	= { nullptr };

	vector<shared_ptr<UIObject>>	UIObjectList;
	vector<shared_ptr<FontObject>>	FontObjectList;

	_bool							Activation = { true };

	XMFLOAT4X4	WorldMat, ViewMat, ProjMat;

	_float							CurrentHPPercentage = { 1.f };
	ComPtr<ID3D11ShaderResourceView> SRV1, SRV2, SRV3;
	UIInfo							BackGroundInfo;
};

