#include "TimeManager.h"
#include "GameInstance.h"

TimeManager::TimeManager(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GRPDEV(_GRPDEV), DEVCTX(_DEVCTX) {
    
}
TimeManager::~TimeManager() {}

HRESULT TimeManager::Ready_TimeManager() {
    ZeroMemory(&Default_FrameTime, sizeof(LARGE_INTEGER));
    ZeroMemory(&Default_FixTime, sizeof(LARGE_INTEGER));
    ZeroMemory(&Default_LastTime, sizeof(LARGE_INTEGER));
    ZeroMemory(&Default_CpuTick, sizeof(LARGE_INTEGER));

    ZeroMemory(&Fixed_FrameTime, sizeof(LARGE_INTEGER));
    ZeroMemory(&Fixed_FixTime, sizeof(LARGE_INTEGER));
    ZeroMemory(&Fixed_LastTime, sizeof(LARGE_INTEGER));
    ZeroMemory(&Fixed_CpuTick, sizeof(LARGE_INTEGER));

    QueryPerformanceCounter(&Default_FrameTime);
    QueryPerformanceCounter(&Default_LastTime);
    QueryPerformanceCounter(&Default_FixTime);
    QueryPerformanceFrequency(&Default_CpuTick);

    QueryPerformanceCounter(&Fixed_FrameTime);
    QueryPerformanceCounter(&Fixed_LastTime);
    QueryPerformanceCounter(&Fixed_FixTime);
    QueryPerformanceFrequency(&Fixed_CpuTick);

    Default_fTimeDelta = 0.f;
    Fixed_fTimeDelta = 0.f;

    FrameTimeAccumulation = 0.f;
    TimeAccumulation = 0.f;

    hWnd = GameInstance::GetInstance().Get_EngineOption().hWnd;

    return S_OK;
}
VOID TimeManager::Update_DefaultTimer() {
    QueryPerformanceCounter(&Default_FrameTime);			// 1500

    if (Default_FrameTime.QuadPart - Default_FixTime.QuadPart >= Default_CpuTick.QuadPart)
    {
        QueryPerformanceFrequency(&Default_CpuTick);
        Default_FixTime = Default_FrameTime;
    }

    Default_fTimeDelta = (Default_FrameTime.QuadPart - Default_LastTime.QuadPart) / (_float)Default_CpuTick.QuadPart;

    Default_LastTime = Default_FrameTime;
}
VOID TimeManager::Update_FixedTimer() {
    QueryPerformanceCounter(&Fixed_FrameTime);			// 1500

    if (Fixed_FrameTime.QuadPart - Fixed_FixTime.QuadPart >= Fixed_CpuTick.QuadPart)
    {
        QueryPerformanceFrequency(&Fixed_CpuTick);
        Fixed_FixTime = Fixed_FrameTime;
    }

    Fixed_fTimeDelta = (Fixed_FrameTime.QuadPart - Fixed_LastTime.QuadPart) / (_float)Fixed_CpuTick.QuadPart;

    Fixed_LastTime = Fixed_FrameTime;
    
    if (HS_TimeLeft > 0.f) {
        HS_TimeLeft -= Fixed_fTimeDelta;
    }
    else {
        TimeScale = 1.f;
        HS_TimeLeft = 0.f;
    }
}


VOID TimeManager::Execute_HitStop(_float _Time, _float _Scale) {
    HS_TimeLeft = _Time;
    TimeScale = _Scale;
}

unique_ptr<TimeManager>	TimeManager::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
    auto Instance = unique_ptr<TimeManager>(new TimeManager(_GRPDEV, _DEVCTX));
    if (FAILED(Instance->Ready_TimeManager()))
        MSG_BOX("Cannot Create TimeManager.");

    return Instance;
}