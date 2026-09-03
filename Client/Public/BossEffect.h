#pragma once
#include "GameObject.h"

class BossEffect : public GameObject {
private:
	BossEffect(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	BossEffect(CONST BossEffect& _PRTOBJ);
public:
	virtual ~BossEffect() = default;

public:
	virtual HRESULT		Initialize_ProtoType()			override;
	virtual HRESULT		Initialize(VOID* _ARG)			override;
	virtual VOID		Update(CONST _float& _DT)		override;
	virtual VOID		Late_Update(CONST _float& _DT)	override;
	virtual HRESULT		Render()						override;

	VOID				Update_TimeProgress(CONST _float& _DT);
	VOID 				Update_EffectTransform();
	VOID				Update_SpecialEffect(CONST _float& _DT);
public:
	VOID	Allocate_EffectTexture(CONST wstring& _FilePath);
	VOID	Allocate_DissolveTexture(CONST wstring& _FilePath);
	VOID	Allocate_NoiseTexture(CONST wstring& _FilePath);

	VOID	Set_EffectLifeTime(const _float& _Time) { MaxLifeTime = _Time; }
	VOID	Set_EffectAlpha(const _float& _Alpha) { EffectAlpha = _Alpha; }

	VOID	Set_EmissiveOption(XMFLOAT3 _Color, _float _Instensity) { EffectEmissiveColor = _Color;  EffectEmissiveIntensity = _Instensity; }

	VOID	Set_EffectScrollSpeed(CONST XMFLOAT2& _Speed) { EffectScrollSpeed = _Speed; }
	VOID	Set_DissolveScrollSpeed(CONST XMFLOAT2& _Speed) { DissolveScrollSpeed = _Speed; }

	VOID	Set_EffectTextureOffset(CONST XMFLOAT2& _Offset) { EffectTextureOffset = _Offset; }
	VOID	Set_DissolveTextureOffset(CONST XMFLOAT2& _Offset) { DissolveTextureOffset = _Offset; }

	VOID	Set_EffectFadeOutValue(const _float& _Value) { EffectFadeOutValue = _Value; }

	VOID	Set_DissolveStrength(CONST _float& _Str) { DissolveStrength = _Str; }
	VOID	Set_DissolveEdgeWidth(CONST _float& _Width) { DissolveEdgeWidth = _Width; }
	VOID	Set_DissolveEdgeColor(CONST XMFLOAT3& _Color) { DissolveEdgeColor = _Color; }

	VOID	Set_EffectLoop(_bool _Loop) { EffectLoop = _Loop; }

	_float Get_EffectLifeTime() { return MaxLifeTime; }
	_float Get_EffectAlpha() { return EffectAlpha; }
	_float3 Get_EffectEmissiveColor() { return EffectEmissiveColor; }
	_float Get_EffectEmissiveIntensity() { return EffectEmissiveIntensity; }

	_float2 Get_EffectScrollSpeed() { return EffectScrollSpeed; }
	_float2 Get_DissolveScrollSpeed() { return DissolveScrollSpeed; }

	_float2 Get_EffectTextureOffset() { return EffectTextureOffset; }
	_float2 Get_DissolveTextureOffset() { return DissolveTextureOffset; }

	_float Get_DissolveStrength() { return DissolveStrength; }
	_float Get_DissolveEdgeWidth() { return DissolveStrength; }
	_float3 Get_DissolveEdgeColor() { return DissolveEdgeColor; }

	_float	Get_EffectFadeOutValue() { return EffectFadeOutValue; }

	_bool	Get_EffectLoop() { return EffectLoop; }

	VOID	Load_FBXModel(CONST wstring& _FilePath);

	VOID	Set_DistanceFromPlayer(CONST _float& _Distance) { Distance = _Distance; }

	VOID	Set_EffectOrbitRotation(CONST _float& _HRZ, CONST _float& _VTC) { EffectOrbitRotation = { _HRZ, _VTC }; }

	VOID	Set_EffectOffset(CONST XMFLOAT3& _Offset)		{ EffectOffset = _Offset;		}
	VOID	Set_EffectScale(CONST XMFLOAT3& _Scale)			{ EffectScale = _Scale;			}
	VOID	Set_EffectRotation(CONST XMFLOAT3& _Rotation)	{ EffectRotation = _Rotation;	}

	XMFLOAT3	Get_EffectScale() { return EffectScale; }
	XMFLOAT3	Get_EffectRotation() { return EffectRotation; }

	_float		Get_DistanceFromPlayer() { return Distance; }
	_float2		Get_EffectOrbitRotation() { return EffectOrbitRotation; }

	XMFLOAT3	Get_EffectOffset() { return EffectOffset; }

	VOID		Set_EffectOwner(shared_ptr<Monster_FinalBoss> _Boss) { EffectOwner = _Boss; }

	VOID		Set_BillBoardOption(_bool _Enable)		 { BillBoardEnable = _Enable;  }

	VOID		Set_ShaderPassNumber(uint32_t _PassNumb) { ShaderPassNumb = _PassNumb; }

	VOID		Set_NoiseScrollSpeed(CONST XMFLOAT2& _Speed)	{ NoiseScrollSpeed = _Speed; }
	VOID		Set_TimeAccumulation(const _float& _Time)		{ TimeAccumulation = _Time;  }
	VOID		Set_TimeProgress(const _float& _Progress)		{ TimeProgress = _Progress;  }
public :
	static  unique_ptr<BossEffect>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	virtual shared_ptr<GameObject>	Clone(VOID* _ARG);

private:
	shared_ptr<MeshLoader>				Component_Model		= { nullptr };
	shared_ptr<Transform>				Component_Transform = { nullptr };
	shared_ptr<Shader>					Component_Shader	= { nullptr };

	shared_ptr<Player>					GamePlayer			= { nullptr };
	shared_ptr<Monster_FinalBoss>		EffectOwner			= { nullptr };

	ComPtr<ID3D11ShaderResourceView>	EffectTexture		= { nullptr };
	ComPtr<ID3D11ShaderResourceView>	DissolveTexture		= { nullptr };
	ComPtr<ID3D11ShaderResourceView>	NoiseTexture		= { nullptr };

	XMFLOAT2     EffectScrollSpeed			= { 0.f, 0.f };
	_float       EffectAlpha				= { 1.f };

	XMFLOAT3     EffectEmissiveColor		= { 0.f, 0.f, 0.f };
	_float       EffectEmissiveIntensity	= { 0.f };

	XMFLOAT2     DissolveScrollSpeed		= { 0.f, 0.f };
	XMFLOAT2     NoiseScrollSpeed			= { 0.f, 0.f };

	_float       DissolveStrength			= { 0.f };
	_float       DissolveEdgeWidth			= { 0.f };
	XMFLOAT3     DissolveEdgeColor			= { 1.f, 1.f, 1.f };

	_float       TimeProgress				= { 0.f };
	_float       TimeAccumulation			= { 0.f };

	XMFLOAT2     EffectTextureOffset		= { 0.f, 0.f };
	XMFLOAT2     DissolveTextureOffset		= { 0.f, 0.f };

	_float       EffectFadeOutValue			= { 0.f };

	_float		 MaxLifeTime				= { 1.f };

	_bool		 ModelRenderFlag			= { false };
	_bool		 EffectLoop					= { false };

	_float		 Distance					= { 0.f };
	_float2		 EffectOrbitRotation		= { 0.f, 0.f };

	XMFLOAT3	 EffectScale				= { 1.f, 1.f, 1.f };
	XMFLOAT3	 EffectRotation				= { 1.f, 1.f, 1.f };
	XMFLOAT3	 EffectOffset				= { 0.f, 0.f, 0.f };

	_bool		 BillBoardEnable			= { false };

	uint32_t	 ShaderPassNumb				= { 0 };
};

