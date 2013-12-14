#include <windows.h>
#include <gl/gL.h>
#include "../../game/GLGame.h"
#include "../../game/GLUtils.h"

enum {
    kMenuNewGame = 9001,
    kMenuPauseGame = 9002,
    kMenuEndGame = 9003,
    kMenuExit = 9004
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
    void onRender();
    void onResize(UINT width, UINT height);
    void onMenu(WORD cmd);
    void onKey(DWORD key, bool down);
};

AppController::AppController()
{
}

AppController::~AppController()
{
}

bool AppController::init(HINSTANCE hInstance)
{
    // Register the window class
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
    win = CreateWindow(winClass.lpszClassName, L"Glypha III", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, x, y, w, h, NULL, NULL, hInstance, this);
    if (win == NULL) {
        return false;
    }
    
    // Setup OpenGL
    hDC = GetDC(win);
    if (hDC == NULL) {
        return false;
    }
    static PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
    };
    int pixelFormat = ChoosePixelFormat(hDC, &pfd);
    if (pixelFormat == 0 || SetPixelFormat(hDC, pixelFormat, &pfd) == FALSE) {
        return false;
    }
    hRC = wglCreateContext(hDC);
    if (hRC == NULL || wglMakeCurrent(hDC, hRC) == FALSE) {
        return false;
    }

    // Create menubar
    HMENU mainMenu = CreateMenu();
    HMENU subMenu = CreatePopupMenu();
    AppendMenu(subMenu, MF_STRING, kMenuNewGame, L"&New Game");
    AppendMenu(subMenu, MF_STRING | MF_GRAYED, kMenuPauseGame, L"&Pause Game");
    AppendMenu(subMenu, MF_STRING | MF_GRAYED, kMenuEndGame, L"&End Game");
    AppendMenu(subMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(subMenu, MF_STRING, kMenuExit, L"E&xit");
    AppendMenu(mainMenu, MF_STRING | MF_POPUP, (UINT_PTR)subMenu, L"&Game");
    subMenu = CreatePopupMenu();
    AppendMenu(subMenu, MF_STRING | MF_GRAYED, kMenuNewGame, L"&Help");
    AppendMenu(subMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(subMenu, MF_STRING | MF_GRAYED, kMenuNewGame, L"High &Scores");
    AppendMenu(subMenu, MF_STRING | MF_GRAYED, kMenuPauseGame, L"&Reset Scores\u2026");
    AppendMenu(subMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(subMenu, MF_STRING | MF_GRAYED, kMenuExit, L"N&o Sound");
    AppendMenu(subMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(subMenu, MF_STRING | MF_GRAYED, kMenuExit, L"&About Glypha III\u2026");
    AppendMenu(mainMenu, MF_STRING | MF_POPUP, (UINT_PTR)subMenu, L"&Options");
    SetMenu(win, mainMenu);

    // Readjust the window so the client size matches our desired size
    RECT rcClient, rcWindow;
    POINT ptDiff;
    (void)GetClientRect(win, &rcClient);
    (void)GetWindowRect(win, &rcWindow);
    ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
    ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
    (void)MoveWindow(win, rcWindow.left, rcWindow.top, w + ptDiff.x, h + ptDiff.y, TRUE);

    // Start the timer
    if (SetTimer(win, 1, 1000/30, NULL) == 0) {
        return false;
    }

    // Show the window
    (void)ShowWindow(win, SW_SHOWNORMAL);
    (void)UpdateWindow(win);

    return true;
}

void AppController::run()
{
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        (void)TranslateMessage(&msg);
        (void)DispatchMessageW(&msg);
    }
}

void AppController::onKey(DWORD key, bool down)
{
    GLGameKey gameKey;
    switch (key) {
    case VK_SPACE: gameKey = kGLGameKeySpacebar; break;
    case VK_DOWN: gameKey = kGLGameKeyDownArrow; break;
    case VK_LEFT: gameKey = kGLGameKeyLeftArrow; break;
    case VK_RIGHT: gameKey = kGLGameKeyRightArrow; break;
    case 'A': gameKey = kGLGameKeyA; break;
    case 'S': gameKey = kGLGameKeyS; break;
    case VK_OEM_1: gameKey = kGLGameKeyColon; break;
    case VK_OEM_7: gameKey = kGLGameKeyQuote; break;
    default:
	    return;
    }
    if (down) {
	    game.handleKeyDownEvent(gameKey);
    } else {
	    game.handleKeyUpEvent(gameKey);
    }
}

LRESULT CALLBACK AppController::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE) {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        AppController *appController = (AppController *)pcs->lpCreateParams;
        (void)SetWindowLongPtrW(hwnd, GWLP_USERDATA, PtrToUlong(appController));
        result = 1;
    } else {
        AppController *appController = reinterpret_cast<AppController *>(static_cast<LONG_PTR>(GetWindowLongPtrW(hwnd, GWLP_USERDATA)));
        bool wasHandled = false;

        if (appController != NULL) {
            switch (message) {
            case WM_SIZE:
                appController->onResize(LOWORD(lParam), HIWORD(lParam));
                result = 0;
                wasHandled = true;
                break;

            case WM_DISPLAYCHANGE:
            case WM_TIMER:
                (void)InvalidateRect(hwnd, NULL, TRUE);
                result = 0;
                wasHandled = true;
                break;

            case WM_PAINT:
                appController->onRender();
                (void)ValidateRect(hwnd, NULL);
                result = 0;
                wasHandled = true;
                break;

            case WM_DESTROY:
                PostQuitMessage(0);
                result = 1;
                wasHandled = true;
                break;

            case WM_COMMAND:
                appController->onMenu(LOWORD(wParam));
                result = 1;
                wasHandled = true;
                break;

            case WM_KEYDOWN:
            case WM_KEYUP:
	            appController->onKey(wParam, message == WM_KEYDOWN);
	            result = 0;
	            wasHandled = true;
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

void AppController::onMenu(WORD cmd)
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
        (void)MessageBoxW(NULL, L"Failed to initialize.", NULL, MB_OK | MB_ICONERROR);
        return 0;
    }
    appController.run();
    return 0;
}
