#include <windows.h>
#include <wininet.h>
#include <tchar.h>
#include <uxtheme.h>

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "uxtheme.lib")

#define IDC_BUTTON 1001
#define IDC_STATIC 1002

BOOL SetSystemProxy(BOOL enable, LPCTSTR proxyServer)
{
    INTERNET_PER_CONN_OPTION_LIST list;
    INTERNET_PER_CONN_OPTION options[2];
    DWORD dwSize = sizeof(list);

    options[0].dwOption = INTERNET_PER_CONN_FLAGS;
    options[0].Value.dwValue = enable ?
        PROXY_TYPE_PROXY | PROXY_TYPE_DIRECT :
        PROXY_TYPE_DIRECT;

    options[1].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
    options[1].Value.pszValue = enable ?
        const_cast<LPTSTR>(proxyServer) :
        const_cast<LPTSTR>(_T(""));

    list.dwSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
    list.pszConnection = NULL;
    list.dwOptionCount = 2;
    list.dwOptionError = 0;
    list.pOptions = options;

    if (!InternetSetOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &list, dwSize))
    {
        MessageBox(NULL, _T("代理设置失败！"), _T("错误"), MB_ICONERROR);
        return FALSE;
    }

    InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);
    InternetSetOption(NULL, INTERNET_OPTION_REFRESH, NULL, 0);
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hButton;
    static HWND hStatic;
    static BOOL proxyEnabled = FALSE;

    switch (message)
    {
    case WM_CREATE:
    {
        hButton = CreateWindow(
            _T("BUTTON"),
            _T("启用代理"),
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_FLAT,
            0, 5, 160, 40,  // 按钮pos
            hWnd,
            (HMENU)IDC_BUTTON,
            ((LPCREATESTRUCT)lParam)->hInstance,
            NULL);

        HFONT hFont = CreateFont(14, 0, 0, 0, FW_LIGHT, FALSE, FALSE, 0, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Segoe UI"));
        SendMessage(hButton, WM_SETFONT, (WPARAM)hFont, TRUE);
        DeleteObject(hFont);

        hStatic = CreateWindow(
            _T("STATIC"),
            _T("power by yuebi"),
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            0, 60, 160, 20,  // 静态文本pos
            hWnd,
            (HMENU)IDC_STATIC,
            ((LPCREATESTRUCT)lParam)->hInstance,
            NULL);

        HFONT hFontStatic = CreateFont(10, 0, 0, 0, FW_THIN, FALSE, FALSE, 0, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Segoe UI"));
        SendMessage(hStatic, WM_SETFONT, (WPARAM)hFontStatic, TRUE);
        DeleteObject(hFontStatic);

        SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(0xFFFFFF));
    }
    break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)GetClassLongPtr(hWnd, GCLP_HBRBACKGROUND));
        EndPaint(hWnd, &ps);
    }
    break;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BUTTON)
        {
            proxyEnabled = !proxyEnabled;
            if (SetSystemProxy(proxyEnabled, _T("10.88.202.71:10001")))
            {
                SetWindowText(hButton, proxyEnabled ? _T("禁用代理") : _T("启用代理"));
            }
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

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    const TCHAR szClassName[] = _T("ProxySwitchClass");

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = szClassName;
    wc.hbrBackground = CreateSolidBrush(0xFFFFFF);
    wc.hCursor = LoadCursor(NULL, IDC_HAND);

    RegisterClass(&wc);

    HWND hWnd = CreateWindow(
        szClassName,
        _T("ヾ(≧▽≦*)o"),
        WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX & ~WS_DLGFRAME & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX & ~WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 175, 115,  
        NULL,
        NULL,
        hInstance,
        NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
