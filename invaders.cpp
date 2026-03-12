// invaders.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "invaders.h"
#include <vector>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

HWND hPlayer = nullptr;
HWND hEnemy = nullptr; 
int enemyStartX = 0;
int enemyDirection = 1;
std::vector<HWND> bullets;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_INVADERS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_INVADERS));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_INVADERS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = CreateSolidBrush(RGB(255, 255, 255));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_INVADERS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    WNDCLASSEXW wcexPlayer = wcex;
    wcexPlayer.lpszClassName = L"PlayerClass";
    wcexPlayer.hbrBackground = CreateSolidBrush(RGB(255, 0, 0));
    wcexPlayer.hIcon = nullptr;
    wcexPlayer.hIconSm = nullptr;
    wcexPlayer.lpfnWndProc = DefWindowProc;
    RegisterClassExW(&wcexPlayer);

    WNDCLASSEXW wcexEnemy = wcex;
    wcexEnemy.lpszClassName = L"EnemyClass";
    wcexEnemy.hbrBackground = CreateSolidBrush(RGB(70, 70, 255));
    wcexEnemy.hIcon = nullptr;
    wcexEnemy.hIconSm = nullptr;
    wcexEnemy.lpfnWndProc = DefWindowProc;
    RegisterClassExW(&wcexEnemy);

    WNDCLASSEXW wcexBullet = wcex;
    wcexBullet.lpszClassName = L"BulletClass";
    wcexBullet.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    wcexBullet.hIcon = nullptr;
    wcexBullet.hIconSm = nullptr;
    wcexBullet.lpfnWndProc = DefWindowProc;
    RegisterClassExW(&wcexBullet);

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
//BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
//{
//   hInst = hInstance; // Store instance handle in our global variable
//
//   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
//      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
//
//   if (!hWnd)
//   {
//      return FALSE;
//   }
//
//   ShowWindow(hWnd, nCmdShow);
//   UpdateWindow(hWnd);
//
//   return TRUE;
//}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int windowWidth = 800;
    int windowHeight = 600;

    int xPos = (screenWidth - windowWidth) / 2;
    int yPos = (screenHeight - windowHeight) / 2;

    DWORD dwStyle = (WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX) | WS_CLIPCHILDREN;

    DWORD dwExStyle = WS_EX_TOPMOST | WS_EX_LAYERED;

    HWND hWnd = CreateWindowExW(dwExStyle, szWindowClass, szTitle, dwStyle,
        xPos, yPos, windowWidth, windowHeight, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        int playerX = (800 - 50) / 2;
        int playerY = 500;

        hPlayer = CreateWindowW(L"PlayerClass", nullptr,
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
            playerX, playerY, 50, 50,
            hWnd, nullptr, hInst, nullptr);

        int enemyX = (800 - 50) / 2;
        int enemyY = 50;

        hEnemy = CreateWindowW(L"EnemyClass", nullptr,
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
            enemyX, enemyY, 50, 40,
            hWnd, nullptr, hInst, nullptr);

        enemyStartX = enemyX;
        SetTimer(hWnd, 1, 30, nullptr);
        SetTimer(hWnd, 2, 50, nullptr);
    }
    break;
    case WM_TIMER:
    {
        if (wParam == 1 && hEnemy != nullptr)
        {
            RECT rcEnemy;

            GetWindowRect(hEnemy, &rcEnemy);

            POINT pt[2];
            pt[0].x = rcEnemy.left;
            pt[0].y = rcEnemy.top;
            pt[1].x = rcEnemy.right;
            pt[1].y = rcEnemy.bottom;
            MapWindowPoints(HWND_DESKTOP, hWnd, pt, 2);

            int currentX = pt[0].x;
            int enemyY = pt[0].y;
            int enemyWidth = pt[1].x - pt[0].x;
            int enemyHeight = pt[1].y - pt[0].y;

            int step = 2;

            currentX += step * enemyDirection;

            if (currentX >= enemyStartX + 25)
            {
                currentX = enemyStartX + 25; 
                enemyDirection = -1;
            }
            else if (currentX <= enemyStartX - 25)
            {
                currentX = enemyStartX - 25; 
                enemyDirection = 1;    
            }

            MoveWindow(hEnemy, currentX, enemyY, enemyWidth, enemyHeight, TRUE);
        }
        if (wParam == 2) {
            for (auto it = bullets.begin(); it != bullets.end(); )
            {
                HWND hB = *it;
                RECT rcBullet;
                GetWindowRect(hB, &rcBullet);

                POINT pt[2];
                pt[0].x = rcBullet.left; pt[0].y = rcBullet.top;
                pt[1].x = rcBullet.right; pt[1].y = rcBullet.bottom;
                MapWindowPoints(HWND_DESKTOP, hWnd, pt, 2);

                int bX = pt[0].x;
                int bY = pt[0].y;
                int bW = pt[1].x - pt[0].x;
                int bH = pt[1].y - pt[0].y;

                bY -= 15;

                if (bY + bH < 0)
                {
                    DestroyWindow(hB);
                    it = bullets.erase(it);  
                }
                else
                {
                    MoveWindow(hB, bX, bY, bW, bH, TRUE);
                    ++it;
                }
            }
        }
    }
    break;
    case WM_KEYDOWN:
    {
        if (wParam == VK_LEFT || wParam == VK_RIGHT)
        {
            RECT rcPlayer, rcClient;

            GetWindowRect(hPlayer, &rcPlayer);

            GetClientRect(hWnd, &rcClient);

            POINT pt[2];
            pt[0].x = rcPlayer.left;
            pt[0].y = rcPlayer.top;
            pt[1].x = rcPlayer.right;
            pt[1].y = rcPlayer.bottom;
            MapWindowPoints(HWND_DESKTOP, hWnd, pt, 2);

            int playerX = pt[0].x;
            int playerY = pt[0].y;
            int playerWidth = pt[1].x - pt[0].x;
            int playerHeight = pt[1].y - pt[0].y;

            int step = 15;

            if (wParam == VK_LEFT)
            {
                playerX -= step;
                if (playerX < 10)
                {
                    playerX = 10;
                }
            }
            else if (wParam == VK_RIGHT)
            {
                playerX += step;
                if (playerX + playerWidth > rcClient.right - 10)
                {
                    playerX = rcClient.right - 10 - playerWidth;
                }
            }

            MoveWindow(hPlayer, playerX, playerY, playerWidth, playerHeight, TRUE);
        }
        if (wParam == VK_SPACE)
        {
            RECT rcPlayer;
            GetWindowRect(hPlayer, &rcPlayer);

            POINT pt[2];
            pt[0].x = rcPlayer.left; pt[0].y = rcPlayer.top;
            pt[1].x = rcPlayer.right; pt[1].y = rcPlayer.bottom;
            MapWindowPoints(HWND_DESKTOP, hWnd, pt, 2);

            int playerX = pt[0].x;
            int playerY = pt[0].y;
            int playerWidth = pt[1].x - pt[0].x;

            int bulletWidth = 5;
            int bulletHeight = 15;

            int bulletX = playerX + (playerWidth / 2) - (bulletWidth / 2);
            int bulletY = playerY - bulletHeight;

            HWND hBullet = CreateWindowW(L"BulletClass", nullptr,
                WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                bulletX, bulletY, bulletWidth, bulletHeight,
                hWnd, nullptr, hInst, nullptr);

            if (hBullet)
            {
                bullets.push_back(hBullet);
            }
        }
    }
    break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
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
    case WM_ACTIVATE:
    {
        if (LOWORD(wParam) == WA_INACTIVE)
        {
            SetLayeredWindowAttributes(hWnd, 0, 102, LWA_ALPHA);
        }
        else
        {
            SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
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

// Message handler for about box.
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
