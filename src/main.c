#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include <Windows.h>

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/wglext.h>

#include "log.h"
#include "sm.h"

// глобальная перменная для хранения ошибки OpenGL
GLenum g_OpenGLError;

// проверка на ошибки OpenGL
#define OPENGL_CHECK_FOR_ERRORS() \
    if((g_OpenGLError = glGetError()) != GL_NO_ERROR) LOG(g_uLogOGL, LOG_E, TEXT("Error %d(0x%x)"), (INT)g_OpenGLError, (INT)g_OpenGLError);

// безопасный вызов функции OpenGL
#define OPENGL_CALL(exp) \
    { exp; if((g_OpenGLError = glGetError()) != GL_NO_ERROR) LOG(g_uLogOGL, LOG_E, TEXT("expression \"" #exp "\" Error %d(0x%x)"), (INT)g_OpenGLError, (INT)g_OpenGLError); }


CONST LPCTSTR   g_clpszClassName = TEXT("A735 Core Window Class");
UINT            g_uLogOGL       = 0;
UINT            g_uLogApp       = 0;

HINSTANCE       g_hInstance     = NULL;
HWND            g_hWnd          = NULL;
HDC             g_hDC           = NULL;
HGLRC           g_hRC           = NULL;

BOOL            g_bRun          = TRUE;
BOOL            g_bActive       = TRUE;
BOOL            g_bAnimating    = TRUE;
INT             g_nWidth        = 640;
INT             g_nHeight       = 480;

SM              g_smAppState;

// определим указатель на функцию создания расширенного контекста OpenGL
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;

// обработчик сообщений окна
LRESULT CALLBACK rWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT     rAppInit();
VOID    rAppDeInit();
INT     rAppLoop();

INT     rAppIRegisterClass(LPSM lpsm, UINT uMsg, LPVOID ptr);
INT     rAppICreateWindow(LPSM lpsm, UINT uMsg, LPVOID ptr);
INT     rAppIIdle(LPSM lpsm, UINT uMsg, LPVOID ptr);

INT     rWndProcCreate(HWND hWnd);
INT     rWndProcDestroy(HWND hWnd);
INT     rWndProcSize(HWND hWnd);
INT     rWndProcPaint(HWND hWnd);

INT main(INT argc, LPSTR argv[]) {
    INT o = rAppInit();
    if(o) return o;
    o = rAppLoop();
    rAppDeInit();
    return o;
}

LRESULT CALLBACK rWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE)
                g_bRun = FALSE;
            return 0;

        case WM_PAINT:
            return rWndProcPaint(hWnd);

        case WM_ERASEBKGND:
            return 0;

        case WM_SIZE:
            if(wParam != SIZE_MINIMIZED) {
                g_nWidth = (INT)LOWORD(lParam);  // ширина рабочей области
                g_nHeight = (INT)HIWORD(lParam); // высота рабочей области

                return rWndProcSize(hWnd);
            }
            break;

        case WM_SETFOCUS:
            g_bActive = TRUE;
            return 0;

        case WM_KILLFOCUS:
            g_bActive = FALSE;
            return 0;

        case WM_ACTIVATE:
            g_bActive = (LOWORD(wParam) == WA_INACTIVE);
            return 0;

        case WM_CLOSE:
            g_bRun = g_bActive = FALSE;
            PostQuitMessage(0);
            return 0;

        case WM_CREATE:
            return rWndProcCreate(hWnd);

        case WM_DESTROY:
            return rWndProcDestroy(hWnd);
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


INT     rAppInit() {
    LOG_rStart();
    LOG_rSetLvl(LOG_V);
    SM_rStart();

    INT o;

    g_uLogApp = LOG_rNewCategory(TEXT("App"));
    g_uLogOGL = LOG_rNewCategory(TEXT("OpenGL"));
    g_hInstance = (HINSTANCE)GetModuleHandle(NULL);

    o = SM_rPush(&g_smAppState, rAppIRegisterClass, NULL);
    if(o) return o;
    o = SM_rPush(&g_smAppState, rAppICreateWindow, NULL);
    if(o) return o;
    o = SM_rPush(&g_smAppState, rAppIIdle, NULL);
    if(o) return o;

    return 0;
}
VOID    rAppDeInit() {

    SM_rPopAll(&g_smAppState, NULL);

    LOG_rFin();
}
INT     rAppLoop() {
    MSG msg;

    while (g_bRun) {
        // обработаем сообщения из очереди сообщений
        while (PeekMessage(&msg, g_hWnd, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                g_bRun = 0;
                break;
            }
            // TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if(g_bRun && g_bActive) g_bRun = !SM_rProcess(&g_smAppState, 0, NULL);
        Sleep(1);
    }
    return msg.wParam;
}

INT     rAppIRegisterClass(LPSM lpsm, UINT uMsg, LPVOID ptr) {
    if(uMsg == SM_MSG_PUSH) {
        WNDCLASSEX wcx;
        memset(&wcx, 0, sizeof(wcx));
        wcx.cbSize        = sizeof(wcx);
        wcx.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wcx.lpfnWndProc   = (WNDPROC)(rWndProc);
        wcx.hInstance     = g_hInstance;
        wcx.lpszClassName = g_clpszClassName;
        wcx.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
        wcx.hCursor       = LoadCursor(NULL, IDC_ARROW);

        ATOM o = RegisterClassEx(&wcx);
        if(o) {
            LOG(g_uLogApp, LOG_D, TEXT("RegisterClassEx OK!\n\tatom=%p"), o);
        } else {
            LOG(g_uLogApp, LOG_F, TEXT("RegisterClassEx FAIL!\n\terr=%p"), GetLastError());
            return -1;
        }
    } else if(uMsg == SM_MSG_POP) {
        UnregisterClass(g_clpszClassName, g_hInstance);
    }
    return 0;
}
INT     rAppICreateWindow(LPSM lpsm, UINT uMsg, LPVOID ptr) {
    if(uMsg == SM_MSG_PUSH) {
        RECT rc;
        DWORD  dwStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX | WS_VISIBLE,
            dwExStyle = WS_EX_APPWINDOW;
        // выровняем окно по центру экрана
        INT x = (GetSystemMetrics(SM_CXSCREEN) - g_nWidth ) / 2;
        INT y = (GetSystemMetrics(SM_CYSCREEN) - g_nHeight) / 2;
        rc.left   = x;
        rc.right  = x + g_nWidth;
        rc.top    = y;
        rc.bottom = y + g_nHeight;
        // подгоним размер окна под стили
        AdjustWindowRectEx (&rc, dwStyle, FALSE, dwExStyle);
        // создаем окно
        g_hWnd = CreateWindowEx(dwExStyle, g_clpszClassName, TEXT("Empty title"), dwStyle, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, g_hInstance, NULL);
        if (g_hWnd) {
            LOG(g_uLogApp, LOG_D, TEXT("CreateWindowEx OK!\n\thwnd=%p"), g_hWnd);
        } else {
            LOG(g_uLogApp, LOG_F, TEXT("CreateWindowEx FAIL!\n\terr=%p"), GetLastError());
            return -1;
        }
    } else if(uMsg == SM_MSG_POP) {
        // удаляем окно
        if (g_hWnd) {
            DestroyWindow(g_hWnd);
            LOG(g_uLogApp, LOG_D, TEXT("DestroyWindow OK!\n\thwnd=%p"), g_hWnd);
            g_hWnd = NULL;
        } else {
            LOG(g_uLogApp, LOG_W, TEXT("DestroyWindow miss!\n\thwnd=%p"), g_hWnd);
        }
    }
    return 0;
}
INT     rAppIIdle(LPSM lpsm, UINT uMsg, LPVOID ptr) {
    rWndProcPaint(g_hWnd);
    return 0;
}

INT     rWndProcCreate(HWND hWnd) {
    PIXELFORMATDESCRIPTOR pfd;
    INT iFormat;
    HGLRC hRCTemp;
    // укажем атрибуты для создания расширенного контекста OpenGL
    INT attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };
    // получим дескриптор контекста окна
    g_hDC = GetDC(hWnd);
    if(!g_hDC) {
        LOG(g_uLogApp, LOG_F, TEXT("GetDC FAIL!\n\terr=%p"), GetLastError());
        return -1;
    }
    // описание формата пикселей
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize      = sizeof(pfd);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;

    // запросим формат пикселей, ближайший к описанному выше
    iFormat = ChoosePixelFormat(g_hDC, &pfd);
    if(!iFormat || !SetPixelFormat(g_hDC, iFormat, &pfd)) {
        LOG(g_uLogApp, LOG_F, TEXT("Setting pixel format FAIL!\n\terr=%p"), GetLastError());
        return -1;
    }

    // создадим временный контекст рендеринга он нужен для получения функции wglCreateContextAttribsARB
    hRCTemp = wglCreateContext(g_hDC);
    if (!hRCTemp || !wglMakeCurrent(g_hDC, hRCTemp)){
        LOG(g_uLogApp, LOG_F, TEXT("Сreating temp render context FAIL!\n\terr=%p"), GetLastError());
        return -1;
    }

    // получим адрес функции установки атрибутов контекста рендеринга
    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)(wglGetProcAddress("wglCreateContextAttribsARB"));

    // временный контекст OpenGL нам больше не нужен
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRCTemp);

    if(!wglCreateContextAttribsARB) {
        LOG(g_uLogApp, LOG_F, TEXT("wglCreateContextAttribsARB FAIL!\n\terr=%p"), GetLastError());
        return -1;
    }

    // создадим расширенный контекст с поддержкой OpenGL 3
    g_hRC = wglCreateContextAttribsARB(g_hDC, 0, attribs);
    if (!g_hRC || !wglMakeCurrent(g_hDC, g_hRC)) {
        LOG(g_uLogApp, LOG_F, TEXT("Creating render context FAIL!\n\terr=%p"), GetLastError());
        return -1;
    }

    // выведем в лог немного информации о контексте OpenGL
    int major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    LOG(g_uLogOGL, LOG_I, TEXT("OpenGL render context information:\n"
        "  Renderer       : %hs\n"
        "  Vendor         : %hs\n"
        "  Version        : %hs\n"
        "  GLSL version   : %hs\n"
        "  OpenGL version : %d.%d"),
        (LPCSTR)glGetString(GL_RENDERER),
        (LPCSTR)glGetString(GL_VENDOR),
        (LPCSTR)glGetString(GL_VERSION),
        (LPCSTR)glGetString(GL_SHADING_LANGUAGE_VERSION),
        major, minor);

    // покажем окно на экране
    // SetForegroundWindow(hWnd);
    // SetFocus(hWnd);
    // UpdateWindow(hWnd);

    // устанавливаем вьюпорт на все окно
    OPENGL_CALL(glViewport(0, 0, g_nWidth, g_nHeight));

    OPENGL_CHECK_FOR_ERRORS();
    return 0;
}
INT     rWndProcDestroy(HWND hWnd) {
    // удаляем контекст рендеринга
    if (g_hRC) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(g_hRC);
        LOG(g_uLogApp, LOG_D, TEXT("wglDeleteContext OK!\n\thglrc=%p"), g_hRC);
        g_hRC = NULL;
    } else {
        LOG(g_uLogApp, LOG_W, TEXT("wglDeleteContext miss!\n\thglrc=%p"), g_hRC);
    }

    // освобождаем контекст окна
    if (g_hDC) {
        ReleaseDC(hWnd, g_hDC);
        LOG(g_uLogApp, LOG_D, TEXT("ReleaseDC OK!\n\thwnd=%p hdc=%p"), hWnd, g_hDC);
        g_hDC = NULL;
    } else {
        LOG(g_uLogApp, LOG_W, TEXT("ReleaseDC miss!\n\thwnd=%p hdc=%p"), hWnd, g_hDC);
    }
    return 0;
}


INT     rWndProcSize(HWND hWnd) {
    OPENGL_CALL(glViewport(0, 0, g_nWidth, g_nHeight));

    OPENGL_CHECK_FOR_ERRORS();
    return 0;
}
INT     rWndProcPaint(HWND hWnd) {
    if(g_bAnimating) {
        OPENGL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    }

    SwapBuffers(g_hDC);
    ValidateRect(hWnd, NULL);
    return 0;
}



