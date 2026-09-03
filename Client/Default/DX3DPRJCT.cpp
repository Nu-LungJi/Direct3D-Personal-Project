#include "../Default/PCH.h"
#include "framework.h"
#include "DX3DPRJCT.h"

#define MAX_LOADSTRING 100
using namespace Engine;

HWND        hWnd;       HINSTANCE   hInst;
WCHAR       szTitle[MAX_LOADSTRING];
WCHAR       szWindowClass[MAX_LOADSTRING];

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    SetConsoleOutputCP(CP_UTF8);

    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DX3DPRJCT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow)) return FALSE;

    //while (ShowCursor(FALSE) >= 0);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DX3DPRJCT));

    bool MouseFree = FALSE;

    MSG msg = {};
    msg.message = WM_NULL;
    _float DefaultDeltaTime = 0.f, FixedDeltaTime = 0.f;
    unique_ptr<GameManager> GM = GameManager::Create();
    
    while (true) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (WM_QUIT == msg.message) break;

            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        GameInstance::GetInstance().Get_TimeManager()->Update_DefaultTimer();
        DefaultDeltaTime += GameInstance::GetInstance().Get_TimeManager()->Get_DefaultDeltaTime();
        if (DefaultDeltaTime >= 1.f / 60.f) {
            GameInstance::GetInstance().Get_TimeManager()->Update_FixedTimer();
            FixedDeltaTime = GameInstance::GetInstance().Get_TimeManager()->Get_FixedDeltaTime();
            GameInstance::GetInstance().Get_GUIEditor()->Render_Begin_GUIEditor();

            GM->Priority_Update_GameManager(FixedDeltaTime);
            GM->Update_GameManager(FixedDeltaTime);
            GM->LateUpdate_GameManager(FixedDeltaTime);
            GM->Render_GameManager();

            GameInstance::GetInstance().Get_GUIEditor()->Render_GUIEditor();
            GameInstance::GetInstance().Get_GUIEditor()->Render_End_GUIEditor();

            GameInstance::GetInstance().Get_GraphicDevice()->Render_Device();
        }
    }
    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DX3DPRJCT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_CROSS);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(NULL);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
   hInst = hInstance; 
   RECT    rc{ 0,0, static_cast<uint32_t>(WINCX), static_cast<uint32_t>(WINCY) };
   AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
   POINT Resolution = { GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN) };

   hWnd = CreateWindowExW(0, szWindowClass, L"Wuthering Wave", WS_OVERLAPPEDWINDOW,
       (Resolution.x - rc.right) / 2 - 8, (Resolution.y - rc.bottom) / 2 - 19, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd) return FALSE;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(GameInstance::GetInstance().Get_GUIEditor()->PreOccupy_IMGUI(hWnd, message, wParam, lParam)) return true;

    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            MSG_BOX("저장하기");
            //DestroyWindow(hWnd);
            break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
   
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

