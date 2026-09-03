#pragma once
#include "Engine_Define.h"
#include <EffekSeer/Effekseer.h>
#include <EffekSeer/EffekseerRendererDX11.h>

BEGIN(Engine)
class ENGINE_DLL EffectManager {
private:
	EffectManager(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	~EffectManager();

public:
	HRESULT	Ready_EffectManager();
	VOID	Update_EffectManager(const _float& _DT);
	VOID	Render_EffectManager();

	HRESULT	Load_EffectBundle(const filesystem::path& _FolderPath);

	unordered_map<string, Effekseer::EffectRef>* Get_EffectList() { return &EffectProtoList; }

	VOID	Play_Effect(string _EffectName, XMFLOAT3 _Position, XMFLOAT3 _Rotation, XMFLOAT3 _Scale, _float _Speed);
	VOID	Stop_Effect() { /* 필요하면 구현*/ };

	VOID	Release_EffectManager();

public:
	static	unique_ptr<EffectManager>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);

private:
	ComPtr<ID3D11Device>				GRPDEV		= { nullptr };
	ComPtr<ID3D11DeviceContext>			DEVCTX		= { nullptr };

	Effekseer::ManagerRef				EffekSeerManager	= { nullptr };
	EffekseerRendererDX11::RendererRef	EffekSeerRenderer	= { nullptr };

	unordered_map<string, Effekseer::EffectRef>		EffectProtoList;
	Effekseer::Handle					EffectHandle;

	atomic<_bool>						Render_Flag		= { false };
	atomic<_bool>						EffectPlay_Flag = { false };
};
END