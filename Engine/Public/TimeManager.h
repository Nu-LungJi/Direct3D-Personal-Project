#pragma once

#include "Engine_Define.h"
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL TimeManager  {
private:
	TimeManager(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	virtual ~TimeManager();

public:
	HRESULT		Ready_TimeManager();
	VOID		Update_DefaultTimer();
	VOID		Update_FixedTimer();

	_float		Get_DefaultDeltaTime()	const	{ return Default_fTimeDelta; }
	_float		Get_FixedDeltaTime()	const	{ return Fixed_fTimeDelta * TimeScale; }

	_float*		Get_TimeScale()					{ return &TimeScale; }
	VOID		Set_TimeScale(_float _Scale)	{ TimeScale = _Scale; }

	VOID		Execute_HitStop(_float _Time, _float _Scale);

	static unique_ptr<TimeManager>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);

private:
	ComPtr<ID3D11Device>			GRPDEV = { nullptr };
	ComPtr<ID3D11DeviceContext>		DEVCTX = { nullptr };
	HWND							hWnd;

	LARGE_INTEGER		Default_FrameTime = {};
	LARGE_INTEGER		Default_FixTime = {};
	LARGE_INTEGER		Default_LastTime = {};
	LARGE_INTEGER		Default_CpuTick = {};

	_float				Default_fTimeDelta = {};

	LARGE_INTEGER		Fixed_FrameTime = {};
	LARGE_INTEGER		Fixed_FixTime = {};
	LARGE_INTEGER		Fixed_LastTime = {};
	LARGE_INTEGER		Fixed_CpuTick = {};

	_float				Fixed_fTimeDelta = {};

	_float				FrameTimeAccumulation = {};
	_float				TimeAccumulation = {};
	uint16_t			FrameCount = {};

	_float				TimeScale = { 1.f };
	_float				HS_TimeLeft = { 0.f };

};

END