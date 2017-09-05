#include <WinDef.h>

#include "log.h"

#include <Windows.h>

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/wglext.h>

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

// определим указатель на функцию создания расширенного контекста OpenGL
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;


// обработчик сообщений окна
LRESULT CALLBACK rWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT main(INT argc, LPSTR argv[]) {
    LOG_rStart();
    LOG_rSetLvl(LOG_V);
    g_uLogApp = LOG_rNewCategory(TEXT("App"));
    g_uLogOGL = LOG_rNewCategory(TEXT("OpenGL"));

    g_hInstance = (HINSTANCE)GetModuleHandle(NULL);

    { // Регестрируем класс основного окна и создаём его
        WNDCLASSEX wcx;
        memset(&wcx, 0, sizeof(wcx));
        wcx.cbSize        = sizeof(wcx);
        wcx.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wcx.lpfnWndProc   = (WNDPROC)(rWindowProc);
        wcx.hInstance     = g_hInstance;
        wcx.lpszClassName = g_clpszClassName;
        wcx.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
        wcx.hCursor       = LoadCursor(NULL, IDC_ARROW);

        ATOM o = RegisterClassEx(&wcx);
        if(o) {
            LOG(g_uLogApp, LOG_D, TEXT("RegisterClassEx OK!\n\tatom=%p"), o);
        } else {
            LOG(g_uLogApp, LOG_F, TEXT("RegisterClassEx FAIL!\n\terr=%p"), GetLastError());
            LOG_rFin();
            return 0;
        }
    }

    { // Создание окна
        RECT rc;
        DWORD  dwStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
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
        g_hWnd = CreateWindowEx(dwExStyle, g_clpszClassName, TEXT("Empty title"), dwStyle, rc.left, rc.top,
            rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, g_hInstance, NULL);
        if (!g_hWnd) {
            LOG(g_uLogApp, LOG_F, TEXT("CreateWindowEx FAIL!\n\terr=%p"), GetLastError());
            UnregisterClass(g_clpszClassName, g_hInstance);
            LOG_rFin();
            return 0;
        }
        SetCursorPos(x + g_nWidth / 2, y + g_nHeight / 2);
    }

    { // Создание контекста рендеринга OpenGL
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
        g_hDC = GetDC(g_hWnd);
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
            "  Renderer       : %s\n"
            "  Vendor         : %s\n"
            "  Version        : %s\n"
            "  GLSL version   : %s\n"
            "  OpenGL version : %d.%d\n"),
            (LPCSTR)glGetString(GL_RENDERER),
            (LPCSTR)glGetString(GL_VENDOR),
            (LPCSTR)glGetString(GL_VERSION),
            (LPCSTR)glGetString(GL_SHADING_LANGUAGE_VERSION),
            major, minor);

        // покажем окно на экране
        ShowWindow(g_hWnd, SW_SHOW);
        SetForegroundWindow(g_hWnd);
        SetFocus(g_hWnd);
        UpdateWindow(g_hWnd);

        // устанавливаем вьюпорт на все окно
        OPENGL_CALL(glViewport(0, 0, g_nWidth, g_nHeight));

        OPENGL_CHECK_FOR_ERRORS();
    }

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

        // если окно в рабочем режиме и активно
        if (g_bRun && g_bActive && g_bAnimating) {
            OPENGL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            SwapBuffers(g_hDC);
        }

        Sleep(2);
    }

    {
        // удаляем контекст рендеринга
        if (g_hRC) {
            wglMakeCurrent(NULL, NULL);
            wglDeleteContext(g_hRC);
        }

        // освобождаем контекст окна
        if (g_hDC)
            ReleaseDC(g_hWnd, g_hDC);

        // удаляем окно
        if (g_hWnd)
            DestroyWindow(g_hWnd);

        UnregisterClass(g_clpszClassName, g_hInstance);
    }

    LOG_rFin();
    return 0;
}

LRESULT CALLBACK rWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE)
                g_bRun = FALSE;
            return 0;

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

        case WM_SYSCOMMAND:
            switch (wParam & 0xFFF0) {
                case SC_KEYMENU:
                    return 0;
            }
            break;

        case WM_ERASEBKGND:
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

