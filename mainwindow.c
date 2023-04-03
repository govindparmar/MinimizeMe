#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <strsafe.h>
#include <sal.h>

#pragma comment(lib, "comctl32.lib")

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif


WCHAR g_szClassName[] = L"CoordReportWnd";

LRESULT CALLBACK WindowProc(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam);
BOOL CALLBACK EnumChildProc(_In_ HWND hWnd, _In_ LPARAM lParam);

ATOM RegisterWCEX(_In_ HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    ZeroMemory(&wcex, sizeof(WNDCLASSEXW));

    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.lpfnWndProc = WindowProc;
    wcex.hInstance = hInstance;
    wcex.lpszClassName = g_szClassName;
    wcex.hIcon =
    wcex.hIconSm = LoadIconW(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH) COLOR_WINDOW;

    return RegisterClassExW(&wcex);
}

BOOL CALLBACK EnumChildProc(_In_ HWND hWnd, _In_ LPARAM lParam)
{
    HFONT hfDefault = *(HFONT *)lParam;
    SendMessageW(hWnd, WM_SETFONT, (WPARAM) hfDefault, 0);
    return TRUE;
}

INT APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ INT nShowCmd)
{
    HWND hWnd;
    MSG Msg;
    NONCLIENTMETRICSW ncm;
    HFONT hfDefault;

    ZeroMemory(&ncm, sizeof(NONCLIENTMETRICSW));
    ncm.cbSize = sizeof(NONCLIENTMETRICSW);
    SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW), &ncm, FALSE);
    hfDefault = CreateFontIndirectW(&ncm.lfMessageFont);

    if (!RegisterWCEX(hInstance))
    {
        MessageBoxW(0, L"Window registration failed!", L"Error", MB_ICONSTOP);
        return -1;
    }

    hWnd = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW, g_szClassName, L"Minimize Me", WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX, 100, 100, 400, 250, NULL, NULL, GetModuleHandleW(NULL), NULL);
    ShowWindow(hWnd, SW_SHOW);
    EnumChildWindows(hWnd, EnumChildProc, (LPARAM) &hfDefault);
    UpdateWindow(hWnd);

    while (GetMessageW(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessageW(&Msg);
    }
    return (INT) Msg.wParam;
}

VOID WINAPI OnClose(_In_ HWND hWnd)
{
    DestroyWindow(hWnd);
}

BOOL WINAPI OnCreate(_In_ HWND hWnd, _In_ LPCREATESTRUCTW lpCS)
{
    HINSTANCE hInstance = lpCS->hInstance;
    HWND hStatic;
    INITCOMMONCONTROLSEX iccx;

    iccx.dwICC = ICC_STANDARD_CLASSES;
    iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
    InitCommonControlsEx(&iccx);

    hStatic = CreateWindowW(L"Static", L"", WS_VISIBLE | WS_CHILD, 10, 10, 180, 20, hWnd, NULL, hInstance, NULL);

    return TRUE;
}

VOID WINAPI OnDestroy(_In_ HWND hWnd)
{
    PostQuitMessage(ERROR_SUCCESS);
}

VOID WINAPI OnWindowPosChanged(_In_ HWND hWnd, _In_ CONST LPWINDOWPOS lpWP)
{
    if (lpWP->x < 0 || lpWP->y < 0)
    {
        WCHAR stTxt[64];
        HWND hStatic = FindWindowExW(hWnd, NULL, L"Static", NULL);
        StringCchPrintfW(stTxt, 64, L"(%d, %d)", lpWP->x, lpWP->y);
        SetWindowTextW(hStatic, stTxt);
    }
}

VOID WINAPI OnPaint(_In_ HWND hWnd)
{
    PAINTSTRUCT ps;

    BeginPaint(hWnd, &ps);
    EndPaint(hWnd, &ps);

    return;
}

LRESULT CALLBACK WindowProc(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    switch (Msg)
    {
        HANDLE_MSG(hWnd, WM_CLOSE, OnClose);
        HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
        HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
        HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
        HANDLE_MSG(hWnd, WM_WINDOWPOSCHANGED, OnWindowPosChanged);
    default:
        return DefWindowProcW(hWnd, Msg, wParam, lParam);
    }
    return 0;
}
