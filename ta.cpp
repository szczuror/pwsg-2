#include "framework.h"
#include "miauapi.h"
#include <cmath> // Do funkcji trygonometrycznych (sin, cos, atan2)

#define MAX_LOADSTRING 100
#define NUM_DIGITS 10

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// --- ZMIENNE DO TARCZY TELEFONU ---
HWND hDigits[NUM_DIGITS];
WNDPROC OldButtonProc;
double currentAngleOffset = 0.0;
bool bDragging = false;
double lastMouseAngle = 0.0;
const int dialCenterX = 250;
const int dialCenterY = 250;
const int dialRadius = 120;
// ----------------------------------

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// Funkcja aktualizująca pozycje cyfr na ekranie
void UpdateDigitPositions(HWND hWndParent) {
    for (int i = 0; i < NUM_DIGITS; i++) {
        // Cyfry od 1 do 9, potem 0 ułożone wzdłuż okręgu
        double baseAngle = i * (2.0 * 3.14159 / NUM_DIGITS);
        double angle = baseAngle + currentAngleOffset;

        // Środek tarczy + przesunięcie kątowe - 20 (połowa szerokości/wysokości przycisku 40x40)
        int x = dialCenterX + (int)(dialRadius * cos(angle)) - 20;
        int y = dialCenterY + (int)(dialRadius * sin(angle)) - 20;

        // Proste ustawienie pozycji okna w czasie rzeczywistym
        SetWindowPos(hDigits[i], nullptr, x, y, 40, 40, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS);
    }
}

// Subclassing - Przechwytujemy zdarzenia myszy dla cyfr (przycisków)
LRESULT CALLBACK DigitSubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_LBUTTONDOWN:
    {
        bDragging = true;
        POINT pt;
        GetCursorPos(&pt);
        HWND hParent = GetParent(hWnd);
        ScreenToClient(hParent, &pt);
        // Obliczamy początkowy kąt kliknięcia
        lastMouseAngle = atan2(pt.y - dialCenterY, pt.x - dialCenterX);

        SetCapture(hWnd); // Trzymamy mysz uwięzioną w tym oknie
        return 0;
    }
    case WM_MOUSEMOVE:
    {
        if (bDragging) {
            POINT pt;
            GetCursorPos(&pt);
            HWND hParent = GetParent(hWnd);
            ScreenToClient(hParent, &pt);

            // Obliczamy nowy kąt po przesunięciu
            double newAngle = atan2(pt.y - dialCenterY, pt.x - dialCenterX);
            double delta = newAngle - lastMouseAngle;

            // Obsługa przeskoku kąta przy -Pi/Pi (uniknięcie "wariowania" tarczy)
            if (delta > 3.14159) delta -= 2.0 * 3.14159;
            if (delta < -3.14159) delta += 2.0 * 3.14159;

            currentAngleOffset += delta;
            lastMouseAngle = newAngle;

            UpdateDigitPositions(hParent);
        }
        return 0;
    }
    case WM_LBUTTONUP:
    {
        bDragging = false;
        ReleaseCapture(); // Uwalniamy mysz
        return 0;
    }
    }
    // Domyślna obsługa dla wszystkiego innego
    return CallWindowProc(OldButtonProc, hWnd, msg, wParam, lParam);
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MIAUAPI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MIAUAPI));
    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MIAUAPI));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MIAUAPI);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, 0, 600, 600, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        // 1. Tworzymy cyfry
        const wchar_t* labels[10] = { L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L"0" };
        for (int i = 0; i < NUM_DIGITS; i++) {
            hDigits[i] = CreateWindowW(L"STATIC", labels[i],
                WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_CENTER | SS_CENTERIMAGE | WS_BORDER,
                0, 0, 40, 40,
                hWnd, (HMENU)(UINT_PTR)(100 + i), hInst, nullptr);

            // 2. Podmieniamy procedurę okna, aby wyłapać ruchy myszy na poszczególnych "cyferkach"
            if (i == 0) {
                OldButtonProc = (WNDPROC)SetWindowLongPtr(hDigits[i], GWLP_WNDPROC, (LONG_PTR)DigitSubclassProc);
            }
            else {
                SetWindowLongPtr(hDigits[i], GWLP_WNDPROC, (LONG_PTR)DigitSubclassProc);
            }
        }

        // 3. Ustawiamy cyfry na startowych miejscach
        UpdateDigitPositions(hWnd);
    }
    break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
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
