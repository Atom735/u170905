#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include <Windows.h>

#include <stdlib.h>

#include "log.h"
#include "sm.h"
#include "gl.h"
#include "utilities.h"
#include "ft.h"


CONST LPCTSTR   g_clpszClassName = TEXT("A735 Core Window Class");

HINSTANCE       g_hInstance     = NULL;
HWND            g_hWnd          = NULL;
HDC             g_hDC           = NULL;
HGLRC           g_hRC           = NULL;

BOOL            g_bRun          = TRUE;
BOOL            g_bActive       = TRUE;
BOOL            g_bAnimating    = TRUE;
INT             g_nWidth        = 640;
INT             g_nHeight       = 480;

LARGE_INTEGER   g_liQPF;
LARGE_INTEGER   g_liQPC;
LARGE_INTEGER   g_liQPC_old;
UINT            g_uLastFrame;
FLOAT           g_fLastFrame;
FLOAT           g_f1QPF;

SM              g_smAppState;
SM              g_smOglState;


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

INT     rSOpenGLStart(LPSM lpsm, UINT uMsg, LPVOID ptr);
INT     rSOpenGL_BackGround(LPSM lpsm, UINT uMsg, LPVOID ptr);
INT     rSOpenGL_DrawText(LPSM lpsm, UINT uMsg, LPVOID ptr);


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
    QueryPerformanceFrequency(&g_liQPF); // 3579545
    QueryPerformanceCounter(&g_liQPC_old);
    LOG_rStart();
    LOG_rSetLvl(LOG_V);
    SM_rStart();

    INT o;

    g_hInstance = (HINSTANCE)GetModuleHandle(NULL);


    o = SM_rPush(&g_smAppState, rAppIRegisterClass, NULL);
    if(o) return o;
    o = SM_rPush(&g_smAppState, rAppICreateWindow, NULL);
    if(o) return o;
    o = SM_rPush(&g_smAppState, FT_I, NULL);
    if(o) return o;
    o = SM_rPush(&g_smAppState, rAppIIdle, NULL);
    if(o) return o;


    if((o = SM_rPush(&g_smOglState, rSOpenGL_BackGround, NULL))) {
        SM_rPop(&g_smOglState, NULL);
        return o;
    }
    if((o = SM_rPush(&g_smOglState, rSOpenGLStart, NULL))) {
        SM_rPop(&g_smOglState, NULL);
        return o;
    }
    if((o = SM_rPush(&g_smOglState, rSOpenGL_DrawText, NULL))) {
        SM_rPop(&g_smOglState, NULL);
        return o;
    }



    QueryPerformanceCounter(&g_liQPC);

    g_liQPC.QuadPart -= g_liQPC_old.QuadPart;

    LOG(g_uLogApp, LOG_I, TEXT("QueryPerformance\n\tQPF=%lld\n\tInit: %lld"), g_liQPF.QuadPart, g_liQPC.QuadPart);

    g_liQPC.QuadPart *= 1000LL;
    g_liQPC.QuadPart /= g_liQPF.QuadPart;
    g_f1QPF = 1.f/(FLOAT)g_liQPF.LowPart;

    LOG(g_uLogApp, LOG_I, TEXT("needed Time to Init app: %lldms"), g_liQPC.QuadPart);

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
    if(uMsg == SM_MSG_PUSH) {
    } else if(uMsg == SM_MSG_POP) {
    } else {
        rWndProcPaint(g_hWnd);
        Sleep(1);
    }
    return 0;
}

INT     rWndProcCreate(HWND hWnd) {
    PIXELFORMATDESCRIPTOR pfd;
    INT iFormat;

    // получим дескриптор контекста окна
    g_hDC = GetDC(hWnd);
    if(!g_hDC) {
        LOG(g_uLogApp, LOG_F, TEXT("GetDC FAIL!\n\terr=%p"), GetLastError());
        return -1;
    }

    INT iFormatMax = DescribePixelFormat(g_hDC, 1, sizeof(pfd), &pfd);
    LOG(g_uLogApp, LOG_I, TEXT("PFD Count = %d"), iFormatMax);
    for(iFormat = 1; iFormat <= iFormatMax; ++iFormat) {
        iFormatMax = DescribePixelFormat(g_hDC, iFormat, sizeof(pfd), &pfd);

        LOG(g_uLogApp, LOG_I, TEXT("PFD  [%d/%d]\n"
            "  nSize =         %d/%d\n"
            "  nVersion =      %d\n"
            "  cColorBits =    %d R%d G%d B%d A%d\n"
            "  cAccumBits =    %d R%d G%d B%d A%d\n"
            "  cDepthBits =    %d\n"
            "  cStencilBits =  %d\n"
            "  cAuxBuffers =   %d\n"
            "  dwVisibleMask = %p\n"
            "  shifts =        R%d G%d B%d A%d%hs"),
            iFormat, iFormatMax,
            pfd.nSize, sizeof(pfd),
            pfd.nVersion,
            pfd.cColorBits, pfd.cRedBits, pfd.cGreenBits, pfd.cBlueBits, pfd.cAlphaBits,
            pfd.cAccumBits, pfd.cAccumRedBits, pfd.cAccumGreenBits, pfd.cAccumBlueBits, pfd.cAccumAlphaBits,
            pfd.cDepthBits,
            pfd.cStencilBits,
            pfd.cAuxBuffers,
            pfd.dwVisibleMask,
            pfd.cRedShift, pfd.cGreenShift, pfd.cBlueShift, pfd.cAlphaShift,
            GL_INFOSPIXELFLAGS(pfd.dwFlags));
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

    // покажем окно на экране
    // SetForegroundWindow(hWnd);
    // SetFocus(hWnd);
    // UpdateWindow(hWnd);

    if(!(g_hRC = GL_rCreateCtx(g_hDC))) {
        LOG(g_uLogApp, LOG_F, TEXT("Create OpenGL ctx FAIL!"));
        return -1;
    }
    if(GL_rInitExtensions()) {
        LOG(g_uLogApp, LOG_F, TEXT("Init OpenGL extensions FAIL!"));
        return -1;
    }

    return 0;
}
INT     rWndProcDestroy(HWND hWnd) {

    SM_rPopAll(&g_smOglState, NULL);

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
    QueryPerformanceCounter(&g_liQPC_old);
    if(g_bAnimating) {
        OPENGL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        SM_rProcessAll(&g_smOglState, 0, NULL);
        SwapBuffers(g_hDC);
    }
    ValidateRect(hWnd, NULL);
    QueryPerformanceCounter(&g_liQPC);
    g_uLastFrame = g_liQPC.LowPart - g_liQPC_old.LowPart;
    g_fLastFrame = g_f1QPF * (FLOAT)g_uLastFrame;
    return 0;
}

INT     rSOpenGL_BackGround(LPSM lpsm, UINT uMsg, LPVOID ptr) {
    static GLuint iSP = 0;
    static GLuint iVS = 0;
    static GLuint iFS = 0;
    static GLuint iVAO = 0;
    static GLuint iVBO = 0;

    if(uMsg == SM_MSG_PUSH) {
        struct {
            FLOAT x, y;
            FLOAT r, g, b, a;
        } m[6];

        m[1].x = m[4].x = m[5].x = m[0].y = m[1].y = m[4].y =  1.0f;
        m[0].x = m[2].x = m[3].x = m[2].y = m[3].y = m[5].y = -1.0f;


        m[0].r = m[1].r = m[2].r = m[3].r = m[4].r = m[5].r = 1.0f;
        m[0].a = m[1].a = m[2].a = m[3].a = m[4].a = m[5].a = 1.0f;

        m[0].g = (FLOAT)0xFD/(FLOAT)0xFF;
        m[0].b = (FLOAT)0xE7/(FLOAT)0xFF;

        m[1].g = m[4].g = (FLOAT)0xD7/(FLOAT)0xFF;
        m[1].b = m[4].b = (FLOAT)0x40/(FLOAT)0xFF;

        m[2].g = m[3].g = (FLOAT)0xD1/(FLOAT)0xFF;
        m[2].b = m[3].b = (FLOAT)0x80/(FLOAT)0xFF;

        m[5].g = (FLOAT)0xCC/(FLOAT)0xFF;
        m[5].b = (FLOAT)0xBC/(FLOAT)0xFF;

        // FFFDE7
        // FFD740
        // FFD180
        // FFCCBC


        if(!(iVS = GL_rCreateShaderFromFileA(GL_VERTEX_SHADER, "data/2dRGBA.v.glsl"))) {
            return -1;
        }
        if(!(iFS = GL_rCreateShaderFromFileA(GL_FRAGMENT_SHADER, "data/2dRGBA.f.glsl"))) {
            return -1;
        }
        GLuint u[3]; u[0] = iVS; u[1] = iFS; u[2] = 0;
        if(!(iSP = GL_rCreateProgramm(u))) {
            return -1;
        }
        OPENGL_CALL(glUseProgram(iSP));
        // создадим и используем Vertex Array Object (VAO)
        OPENGL_CALL(glGenVertexArrays(1, &iVAO));
        OPENGL_CALL(glBindVertexArray(iVAO));
        // создадим и используем Vertex Buffer Object (VBO)
        OPENGL_CALL(glGenBuffers(1, &iVBO));
        OPENGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, iVBO));
        // заполним VBO данными треугольника
        OPENGL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(FLOAT)*6*6, m, GL_STATIC_DRAW));
        GLint iGLSL_pos, iGLSL_color;
        // получим позицию атрибута 'in_pos' из шейдера
        OPENGL_CALL(iGLSL_pos = glGetAttribLocation(iSP, "in_pos"));
        if (iGLSL_pos != -1) {
            // назначим на атрибут параметры доступа к VBO
            OPENGL_CALL(glVertexAttribPointer(iGLSL_pos, 2, GL_FLOAT, GL_FALSE, sizeof(FLOAT)*6, (const GLvoid*)0));
            // разрешим использование атрибута
            OPENGL_CALL(glEnableVertexAttribArray(iGLSL_pos));
        }
        // получим позицию атрибута 'in_color' из шейдера
        iGLSL_color = glGetAttribLocation(iSP, "in_color");
        if (iGLSL_color != -1)  {
            // назначим на атрибут параметры доступа к VBO
            OPENGL_CALL(glVertexAttribPointer(iGLSL_color, 4, GL_FLOAT, GL_FALSE, sizeof(FLOAT)*6, (const GLvoid*)(sizeof(FLOAT)*2)));
            // разрешим использование атрибута
            OPENGL_CALL(glEnableVertexAttribArray(iGLSL_color));
        }

    } else if(uMsg == SM_MSG_POP) {
        OPENGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
        OPENGL_CALL(glDeleteBuffers(1, &iVBO));
        OPENGL_CALL(glBindVertexArray(0));
        OPENGL_CALL(glDeleteVertexArrays(1, &iVAO));
        OPENGL_CALL(glUseProgram(0));
        OPENGL_CALL(glDeleteProgram(iSP));
        OPENGL_CALL(glDeleteShader(iVS));
        OPENGL_CALL(glDeleteShader(iFS));
    } else {
        OPENGL_CALL(glUseProgram(iSP));
        OPENGL_CALL(glBindVertexArray(iVAO));
        OPENGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, iVBO));
        OPENGL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));
    }
    return 0;
}

INT     rSOpenGL_DrawText(LPSM lpsm, UINT uMsg, LPVOID ptr) {
    static GLuint iSP = 0;
    static GLuint iVS = 0;
    static GLuint iFS = 0;
    static GLuint iVAO = 0;
    static GLuint iVBO = 0;
    static GLuint iTEX = 0;

    if(uMsg == SM_MSG_PUSH) {
        struct {
            FLOAT x, y, u, v;
            FLOAT r, g, b, a;
        } m[6];

        m[1].x = m[4].x = m[5].x = m[0].y = m[1].y = m[4].y =  16.0f;
        m[0].x = m[2].x = m[3].x = m[2].y = m[3].y = m[5].y = 528.0f;


        m[0].r = m[1].r = m[2].r = m[3].r = m[4].r = m[5].r = 1.0f;
        m[0].g = m[1].g = m[2].g = m[3].g = m[4].g = m[5].g = 1.0f;
        m[0].b = m[1].b = m[2].b = m[3].b = m[4].b = m[5].b = 1.0f;
        m[0].a = m[1].a = m[2].a = m[3].a = m[4].a = m[5].a = 1.0f;

        // FFFDE7
        // FFD740
        // FFD180
        // FFCCBC




        if(!(iVS = GL_rCreateShaderFromFileA(GL_VERTEX_SHADER, "data/2dTexRGBA.v.glsl"))) {
            return -1;
        }
        if(!(iFS = GL_rCreateShaderFromFileA(GL_FRAGMENT_SHADER, "data/2dTexRGBA.f.glsl"))) {
            return -1;
        }
        GLuint u[3]; u[0] = iVS; u[1] = iFS; u[2] = 0;
        if(!(iSP = GL_rCreateProgramm(u))) {
            return -1;
        }

        OPENGL_CALL(glGenTextures(1, &iTEX));
        OPENGL_CALL(glBindTexture(GL_TEXTURE_2D, iTEX));
        OPENGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        OPENGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        OPENGL_CALL(glUseProgram(iSP));
        // создадим и используем Vertex Array Object (VAO)
        OPENGL_CALL(glGenVertexArrays(1, &iVAO));
        OPENGL_CALL(glBindVertexArray(iVAO));
        // создадим и используем Vertex Buffer Object (VBO)
        OPENGL_CALL(glGenBuffers(1, &iVBO));
        OPENGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, iVBO));
        // заполним VBO данными треугольника
        OPENGL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(FLOAT)*6*8, m, GL_STATIC_DRAW));
        GLint iGLSL_in_pos, iGLSL_in_tex, iGLSL_in_color;
        GLint iGLSL_u_tex0, iGLSL_u_VP;

        OPENGL_CALL(iGLSL_in_pos = glGetAttribLocation(iSP, "in_pos"));
        if (iGLSL_in_pos != -1) {
            OPENGL_CALL(glVertexAttribPointer(iGLSL_in_pos, 2, GL_FLOAT, GL_FALSE, sizeof(FLOAT)*8, (const GLvoid*)0));
            OPENGL_CALL(glEnableVertexAttribArray(iGLSL_in_pos));
        }
        OPENGL_CALL(iGLSL_in_tex = glGetAttribLocation(iSP, "in_texcoord"));
        if (iGLSL_in_tex != -1) {
            OPENGL_CALL(glVertexAttribPointer(iGLSL_in_tex, 2, GL_FLOAT, GL_FALSE, sizeof(FLOAT)*8, (const GLvoid*)(sizeof(FLOAT)*2)));
            OPENGL_CALL(glEnableVertexAttribArray(iGLSL_in_tex));
        }
        OPENGL_CALL(iGLSL_in_color = glGetAttribLocation(iSP, "in_color"));
        if (iGLSL_in_color != -1)  {
            OPENGL_CALL(glVertexAttribPointer(iGLSL_in_color, 4, GL_FLOAT, GL_FALSE, sizeof(FLOAT)*6, (const GLvoid*)(sizeof(FLOAT)*4)));
            OPENGL_CALL(glEnableVertexAttribArray(iGLSL_in_color));
        }
        OPENGL_CALL(iGLSL_u_tex0 = glGetUniformLocation(iSP, "u_tex0"));
        if (iGLSL_u_tex0 != -1)  {
        }
        OPENGL_CALL(iGLSL_u_VP = glGetUniformLocation(iSP, "u_VP"));
        if (iGLSL_u_VP != -1)  {
        }

    } else if(uMsg == SM_MSG_POP) {
        OPENGL_CALL(glDeleteTextures(1, &iTEX));
        OPENGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
        OPENGL_CALL(glDeleteBuffers(1, &iVBO));
        OPENGL_CALL(glBindVertexArray(0));
        OPENGL_CALL(glDeleteVertexArrays(1, &iVAO));
        OPENGL_CALL(glUseProgram(0));
        OPENGL_CALL(glDeleteProgram(iSP));
        OPENGL_CALL(glDeleteShader(iVS));
        OPENGL_CALL(glDeleteShader(iFS));
    } else {
        OPENGL_CALL(glUseProgram(iSP));
        OPENGL_CALL(glActiveTexture(GL_TEXTURE0));
        // OPENGL_CALL(glUniform1i(uniform_mytexture, /*GL_TEXTURE*/0));
        OPENGL_CALL(glBindTexture(GL_TEXTURE_2D, iTEX));
        OPENGL_CALL(glBindVertexArray(iVAO));

        OPENGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, iVBO));
        OPENGL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));
    }
    return 0;
}

INT     rSOpenGLStart(LPSM lpsm, UINT uMsg, LPVOID ptr) {
    static GLuint iGL_ShaderProgram = 0;
    static GLuint iGL_VertexShader = 0;
    static GLuint iGL_FragmentShader = 0;
    static GLuint iGL_VAO = 0, iGL_VBO = 0;

    // количество вершин в нашей геометрии, у нас простой треугольник
    static const int MESH_VERTEX_COUNT = 3;

    // размер одной вершины меша в байтах - 6 float на позицию и на цвет вершины
    static const int VERTEX_SIZE = 6 * sizeof(float);

    // смещения данных внутри вершины
    static const int VERTEX_POSITION_OFFSET = 0;
    static const int VERTEX_COLOR_OFFSET    = 2 * sizeof(float);
    // подготовим данные для вывода треугольника, всего 3 вершины


    if(uMsg == SM_MSG_PUSH) {
        FLOAT triangleMesh[3 * 6] = {
            -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f,
             0.0f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f,
             0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 1.0f,
        };

        OPENGL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        OPENGL_CALL(glClearDepth(1.0f));

        if(!(iGL_VertexShader = GL_rCreateShaderFromFileA(GL_VERTEX_SHADER, "0.vs"))) {
            return -1;
        }
        if(!(iGL_FragmentShader = GL_rCreateShaderFromFileA(GL_FRAGMENT_SHADER, "0.fs"))) {
            return -1;
        }
        GLuint _ush[3];
        _ush[0] = iGL_VertexShader;
        _ush[1] = iGL_FragmentShader;
        _ush[2] = 0;
        if(!(iGL_ShaderProgram = GL_rCreateProgramm(_ush))) {
            return -1;
        }

        OPENGL_CALL(glUseProgram(iGL_ShaderProgram));

        // создадим и используем Vertex Array Object (VAO)
        OPENGL_CALL(glGenVertexArrays(1, &iGL_VAO));
        OPENGL_CALL(glBindVertexArray(iGL_VAO));

        // создадим и используем Vertex Buffer Object (VBO)
        OPENGL_CALL(glGenBuffers(1, &iGL_VBO));
        OPENGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, iGL_VBO));


        // заполним VBO данными треугольника
        OPENGL_CALL(glBufferData(GL_ARRAY_BUFFER, MESH_VERTEX_COUNT * VERTEX_SIZE,
            triangleMesh, GL_STATIC_DRAW));

        GLint positionLocation, colorLocation;

        // получим позицию атрибута 'position' из шейдера
        positionLocation = glGetAttribLocation(iGL_ShaderProgram, "position");
        if (positionLocation != -1) {
            // назначим на атрибут параметры доступа к VBO
            OPENGL_CALL(glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE,
                VERTEX_SIZE, (const GLvoid*)VERTEX_POSITION_OFFSET));
            // разрешим использование атрибута
            OPENGL_CALL(glEnableVertexAttribArray(positionLocation));
        }

        // получим позицию атрибута 'color' из шейдера
        colorLocation = glGetAttribLocation(iGL_ShaderProgram, "color");
        if (colorLocation != -1)  {
            // назначим на атрибут параметры доступа к VBO
            OPENGL_CALL(glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE,
                VERTEX_SIZE, (const GLvoid*)VERTEX_COLOR_OFFSET));
            // разрешим использование атрибута
            OPENGL_CALL(glEnableVertexAttribArray(colorLocation));
        }

    } else if(uMsg == SM_MSG_POP) {
        OPENGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
        OPENGL_CALL(glDeleteBuffers(1, &iGL_VBO));

        OPENGL_CALL(glBindVertexArray(0));
        OPENGL_CALL(glDeleteVertexArrays(1, &iGL_VAO));

        OPENGL_CALL(glUseProgram(0));
        OPENGL_CALL(glDeleteProgram(iGL_ShaderProgram));

        OPENGL_CALL(glDeleteShader(iGL_VertexShader));
        OPENGL_CALL(glDeleteShader(iGL_FragmentShader));
    } else {
        OPENGL_CALL(glUseProgram(iGL_ShaderProgram));
        OPENGL_CALL(glBindVertexArray(iGL_VAO));

        OPENGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, iGL_VBO));
        // OPENGL_CALL(glBufferData(GL_ARRAY_BUFFER, MESH_VERTEX_COUNT * VERTEX_SIZE, NULL, GL_STATIC_DRAW));

        PFLOAT pF = NULL;

        OPENGL_CALL(pF = (PFLOAT)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

        static FLOAT ff = 0.f;
        for(int i = 0; i < MESH_VERTEX_COUNT; i++) {

            pF[i*6 + 0] = (((FLOAT)(((UINT)(ff*100.f))%501))-250.f)*0.001f;
            pF[i*6 + 1] = -(((FLOAT)(((UINT)(ff*100.f))%501))-250.f)*0.001f;
            pF[i*6 + 2] = pF[i*6 + 3] = pF[i*6 + 4] = ff;
            pF[i*6 + 5] = 1.f;
        }

        pF[6] = -0.5f;
        pF[7] = -0.5f;
        pF[12] = 0.5f;
        pF[13] = -0.5f;

        glUnmapBuffer(GL_ARRAY_BUFFER);
        ff += 0.01f;

        OPENGL_CALL(glDrawArrays(GL_TRIANGLES, 0, MESH_VERTEX_COUNT));
        OPENGL_CHECK_FOR_ERRORS();
    }
    return 0;
}



