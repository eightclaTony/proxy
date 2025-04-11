#include <windows.h>
#include <wininet.h>
#include <tchar.h>
#include <uxtheme.h>

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "uxtheme.lib")

#define IDC_BUTTON 1001
#define IDC_STATIC 1002
#define IDC_COMBOBOX 1003
#define IDC_STATIC_DESC 1004

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
    static HWND hCombo;
    static HWND hDesc;
    static HFONT hFont;
    static BOOL proxyEnabled = FALSE;

    switch (message)
    {
    case WM_CREATE:
        {
            // 创建字体
            hFont = CreateFont(
                20,                    // 字体高度
                0,
                0,
                0,
                FW_NORMAL,
                FALSE,
                FALSE,
                FALSE,
                DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY,
                DEFAULT_PITCH | FF_SWISS,
                _T("Microsoft YaHei")
            );

            // 创建静态文本控件
            HWND hStaticText = CreateWindow(
                _T("STATIC"), _T("将网络连接至："),
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                30, 15, 120, 30,  // 文本控件的位置和大小
                hWnd, NULL,
                ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            SendMessage(hStaticText, WM_SETFONT, (WPARAM)hFont, TRUE);

            // 创建组合框
            hCombo = CreateWindow(_T("COMBOBOX"), NULL,
                WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
                150, 15, 100, 90,  // 调整组合框的位置以适应新添加的文本控件
                hWnd, (HMENU)IDC_COMBOBOX,
                ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            SendMessage(hCombo, WM_SETFONT, (WPARAM)hFont, TRUE);

            // 添加端口选项
            SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)_T("线路一"));
            SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)_T("线路二"));
            SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)_T("线路三"));
            SendMessage(hCombo, CB_SETCURSEL, 0, 0); // 默认选中第一个
            hButton = CreateWindow(
                _T("BUTTON"),
                _T("连接"),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_FLAT,
                35, 55, 207, 40,  // 按钮pos
                hWnd,
                (HMENU)IDC_BUTTON,
                ((LPCREATESTRUCT)lParam)->hInstance,
                NULL);
            SendMessage(hButton, WM_SETFONT, (WPARAM)hFont, TRUE);

            hDesc = CreateWindow(
                _T("STATIC"), _T("此连接较为稳定，速度适中"),
                WS_CHILD | WS_VISIBLE | SS_CENTER,
                0, 105, 280, 40,
                hWnd, (HMENU)IDC_STATIC_DESC,
                ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            SendMessage(hDesc, WM_SETFONT, (WPARAM)hFont, TRUE);
        }
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // 填充窗口背景
        FillRect(hdc, &ps.rcPaint, (HBRUSH)GetClassLongPtr(hWnd, GCLP_HBRBACKGROUND));

        // 确保所有子控件在此时更新
        UpdateWindow(hCombo);
        UpdateWindow(hButton);
        UpdateWindow(hDesc);
        UpdateWindow(hStatic);

        EndPaint(hWnd, &ps);
    }
    break;


case WM_COMMAND:
    if (LOWORD(wParam) == IDC_BUTTON)
    {
        TCHAR szPort[6] = { 0 };
        TCHAR szProxy[32] = _T("10.88.202.71:");

        // 获取当前选中的端口
        int nIndex = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
        SendMessage(hCombo, CB_GETLBTEXT, nIndex, (LPARAM)szPort);

        if (_tcscmp(szPort, _T("线路一")) == 0) {
            _tcscpy_s(szPort, _T("10001"));
        }
        else if (_tcscmp(szPort, _T("线路二")) == 0) {
            _tcscpy_s(szPort, _T("10002"));
        }
        else if (_tcscmp(szPort, _T("线路三")) == 0) {
            _tcscpy_s(szPort, _T("10003"));
        }

        _tcscat_s(szProxy, szPort); // 拼接完整代理地址

        proxyEnabled = !proxyEnabled;
        if (SetSystemProxy(proxyEnabled, szProxy))
        {
            SetWindowText(hButton, proxyEnabled ? _T("断开") : _T("连接"));

            if (proxyEnabled) {
                ShellExecute(hWnd, _T("open"), _T("https://eightclatony.github.io/proxy/"), NULL, NULL, SW_SHOWNORMAL);
            }
        }
    }
    else if (LOWORD(wParam) == IDC_COMBOBOX && HIWORD(wParam) == CBN_SELCHANGE) {
        TCHAR szPort[6] = { 0 };
        int nIndex = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
        SendMessage(hCombo, CB_GETLBTEXT, nIndex, (LPARAM)szPort);

        if (_tcscmp(szPort, _T("线路一")) == 0) {
            SetWindowText(hDesc, _T("此连接较为稳定，速度适中"));
        }
        else if (_tcscmp(szPort, _T("线路二")) == 0) {
            SetWindowText(hDesc, _T("此连接拥有外网访问权限"));
        }
        else if (_tcscmp(szPort, _T("线路三")) == 0) {
            SetWindowText(hDesc, _T("此速度极快的代理，但有时不可用"));
        }
    }
    break;

    case WM_SETCURSOR:
        if (LOWORD(lParam) == HTCLIENT && ((HWND)wParam) == hStatic) {
            SetCursor(LoadCursor(NULL, IDC_HAND));
            return TRUE;
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
        CW_USEDEFAULT, CW_USEDEFAULT, 295, 165,
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
