#include <windows.h>
#include <gl/gL.h>
#include "../../game/GLGame.h"

enum {
    kMenuNewGame = 9001,
    kMenuExit =    9002
};

class AppController {
public:
    AppController();
    ~AppController();
    bool init(HINSTANCE hInstance);
    void run();
private:
    HINSTANCE hInstance;
    HWND win;
    GLGame game;

    HGLRC hRC;
    HDC hDC;  

    static LRESULT CALLBACK AppController::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    bool createRenderingObjects();
    void discardRenderingResources();
    void onRender();
    void onResize(UINT width, UINT height);
    void onCommand(WORD cmd);
};

AppController::AppController()
{
}

AppController::~AppController()
{
}

void ClientResize(HWND hWnd, int nWidth, int nHeight)
{
    RECT rcClient, rcWindow;
    POINT ptDiff;
    GetClientRect(hWnd, &rcClient);
    GetWindowRect(hWnd, &rcWindow);
    ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
    ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
    MoveWindow(hWnd,rcWindow.left, rcWindow.top, nWidth + ptDiff.x, nHeight + ptDiff.y, TRUE);
}

bool AppController::init(HINSTANCE hInstance)
{
    // Register the window class.
    WNDCLASSEX winClass;
    ZeroMemory(&winClass, sizeof(WNDCLASSEX));
    winClass.cbSize = sizeof(winClass);
    winClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    winClass.lpfnWndProc = AppController::WndProc;
    winClass.cbWndExtra = sizeof(LONG_PTR);
    winClass.hInstance = hInstance;
    winClass.hCursor = LoadCursor(NULL, IDI_APPLICATION);
    winClass.lpszClassName = L"MainWindow";

    if (RegisterClassEx(&winClass) == 0) {
        return false;
    }

    // Create the window centered
    int w = 640, h = 460;
    int x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
    int y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;
    win = CreateWindow(L"MainWindow", L"Glypha III", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, x, y, w, h, NULL, NULL, hInstance, this);
    if (win == NULL) {
        return false;
    }
    ClientResize(win, w, h + GetSystemMetrics(SM_CYMENU));
    
    // Create the menus
    HMENU mainMenu = CreateMenu();
    HMENU subMenu = CreatePopupMenu();
    AppendMenu(subMenu, MF_STRING, kMenuNewGame, L"&New Game");
    AppendMenu(subMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(subMenu, MF_STRING, kMenuExit, L"E&xit");
    AppendMenu(mainMenu, MF_STRING | MF_POPUP, (UINT_PTR)subMenu, L"&File");
    SetMenu(win, mainMenu);

    hDC = GetDC(win);
    if (hDC == NULL) {
        return false;
    }

    static  PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW |
        PFD_SUPPORT_OPENGL |
        PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        16,
        0,
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };
    int PixelFormat = ChoosePixelFormat(hDC, &pfd);
    if (PixelFormat == 0) {
        return false;
    }

    if (SetPixelFormat(hDC,PixelFormat,&pfd) == FALSE) {
        return false;
    }

    hRC = wglCreateContext(hDC);
    if (hRC == NULL) {
        return false;
    }

    if (wglMakeCurrent(hDC, hRC) == FALSE) {
        return false;
    }

    if (SetTimer(win, 1, 1000/30, NULL) == 0) {
        return false;
    }

    (void)ShowWindow(win, SW_SHOWNORMAL);
    (void)UpdateWindow(win);

    return true;
}

void AppController::run()
{
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

LRESULT CALLBACK AppController::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE) {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        AppController *pDemoApp = (AppController *)pcs->lpCreateParams;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, PtrToUlong(pDemoApp));
        result = 1;
    } else {
        AppController *pDemoApp = reinterpret_cast<AppController *>(static_cast<LONG_PTR>(GetWindowLongPtrW(hwnd, GWLP_USERDATA)));
        bool wasHandled = false;

        if (pDemoApp != NULL) {
            switch (message) {
            case WM_SIZE:
                pDemoApp->onResize(LOWORD(lParam), HIWORD(lParam));
                result = 0;
                wasHandled = true;
                break;

            case WM_DISPLAYCHANGE:
            case WM_TIMER:
                InvalidateRect(hwnd, NULL, TRUE);
                result = 0;
                wasHandled = true;
                break;

            case WM_PAINT:
                pDemoApp->onRender();
                ValidateRect(hwnd, NULL);
                result = 0;
                wasHandled = true;
                break;

            case WM_DESTROY:
                PostQuitMessage(0);
                result = 1;
                wasHandled = true;
                break;

            case WM_COMMAND:
                pDemoApp->onCommand(LOWORD(wParam));
                break;
            }
        }

        if (wasHandled == false) {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}

void AppController::onRender()
{
    game.draw();
    (void)SwapBuffers(hDC);
}

void AppController::onResize(UINT width, UINT height)
{
    game.renderer()->resize(width, height);
}

void AppController::onCommand(WORD cmd)
{
    switch(cmd) {
    case kMenuNewGame:
        game.newGame();
        break;
    case kMenuExit:
        PostMessage(win, WM_CLOSE, 0, 0);
        break;
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    AppController appController;
    if (appController.init(hInstance) == false) {
        return 0;
    }
    appController.run();
    return 0;
}
