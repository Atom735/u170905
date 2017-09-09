#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include <Windows.h>

#include "gl.h"
#include "utilities.h"

#include <stdlib.h>

// объявим расширения OpenGL
// VAO
PFNGLGENVERTEXARRAYSPROC            glGenVertexArrays           = NULL;
PFNGLDELETEVERTEXARRAYSPROC         glDeleteVertexArrays        = NULL;
PFNGLBINDVERTEXARRAYPROC            glBindVertexArray           = NULL;
// VBO
PFNGLGENBUFFERSPROC                 glGenBuffers                = NULL;
PFNGLDELETEBUFFERSPROC              glDeleteBuffers             = NULL;
PFNGLBINDBUFFERPROC                 glBindBuffer                = NULL;
PFNGLBUFFERDATAPROC                 glBufferData                = NULL;
PFNGLBUFFERSUBDATAPROC              glBufferSubData             = NULL;
PFNGLMAPBUFFERPROC                  glMapBuffer                 = NULL;
PFNGLUNMAPBUFFERPROC                glUnmapBuffer               = NULL;;
// Shaders
PFNGLCREATEPROGRAMPROC              glCreateProgram             = NULL;
PFNGLDELETEPROGRAMPROC              glDeleteProgram             = NULL;
PFNGLLINKPROGRAMPROC                glLinkProgram               = NULL;
PFNGLVALIDATEPROGRAMPROC            glValidateProgram           = NULL;
PFNGLUSEPROGRAMPROC                 glUseProgram                = NULL;
PFNGLGETPROGRAMIVPROC               glGetProgramiv              = NULL;
PFNGLGETPROGRAMINFOLOGPROC          glGetProgramInfoLog         = NULL;
PFNGLCREATESHADERPROC               glCreateShader              = NULL;
PFNGLDELETESHADERPROC               glDeleteShader              = NULL;
PFNGLSHADERSOURCEPROC               glShaderSource              = NULL;
PFNGLCOMPILESHADERPROC              glCompileShader             = NULL;
PFNGLATTACHSHADERPROC               glAttachShader              = NULL;
PFNGLDETACHSHADERPROC               glDetachShader              = NULL;
PFNGLGETSHADERIVPROC                glGetShaderiv               = NULL;
PFNGLGETSHADERINFOLOGPROC           glGetShaderInfoLog          = NULL;
// Shaders attributes
PFNGLGETATTRIBLOCATIONPROC          glGetAttribLocation         = NULL;
PFNGLVERTEXATTRIBPOINTERPROC        glVertexAttribPointer       = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC    glEnableVertexAttribArray   = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC   glDisableVertexAttribArray  = NULL;
// Shaders uniforms
PFNGLGETUNIFORMLOCATIONPROC         glGetUniformLocation        = NULL;
PFNGLUNIFORMMATRIX4FVPROC           glUniformMatrix4fv          = NULL;

// определим указатель на функцию создания расширенного контекста OpenGL
PFNWGLCREATECONTEXTATTRIBSARBPROC   wglCreateContextAttribsARB  = NULL;

// глобальная перменная для хранения ошибки OpenGL
GLenum                               g_OpenGLError = 0;

// получть адрес функции из драйвера
#define OPENGL_GET_PROC(p,n) \
    if (!(n = (p)wglGetProcAddress(#n))) { LOG(g_uLogOGL, LOG_E, TEXT("Loading extension '%s' fail (%d)"), #n, GetLastError()); return -1;  }

INT     GL_rInitExtensions() {
    // VAO
    OPENGL_GET_PROC(PFNGLGENVERTEXARRAYSPROC,           glGenVertexArrays);
    OPENGL_GET_PROC(PFNGLDELETEVERTEXARRAYSPROC,        glDeleteVertexArrays);
    OPENGL_GET_PROC(PFNGLBINDVERTEXARRAYPROC,           glBindVertexArray);
    // VBO
    OPENGL_GET_PROC(PFNGLGENBUFFERSPROC,                glGenBuffers);
    OPENGL_GET_PROC(PFNGLDELETEBUFFERSPROC,             glDeleteBuffers);
    OPENGL_GET_PROC(PFNGLBINDBUFFERPROC,                glBindBuffer);
    OPENGL_GET_PROC(PFNGLBUFFERDATAPROC,                glBufferData);
    OPENGL_GET_PROC(PFNGLBUFFERSUBDATAPROC,             glBufferSubData);
    OPENGL_GET_PROC(PFNGLMAPBUFFERPROC,                 glMapBuffer);
    OPENGL_GET_PROC(PFNGLUNMAPBUFFERPROC,               glUnmapBuffer);
    // Shaders
    OPENGL_GET_PROC(PFNGLCREATEPROGRAMPROC,             glCreateProgram);
    OPENGL_GET_PROC(PFNGLDELETEPROGRAMPROC,             glDeleteProgram);
    OPENGL_GET_PROC(PFNGLLINKPROGRAMPROC,               glLinkProgram);
    OPENGL_GET_PROC(PFNGLVALIDATEPROGRAMPROC,           glValidateProgram);
    OPENGL_GET_PROC(PFNGLUSEPROGRAMPROC,                glUseProgram);
    OPENGL_GET_PROC(PFNGLGETPROGRAMIVPROC,              glGetProgramiv);
    OPENGL_GET_PROC(PFNGLGETPROGRAMINFOLOGPROC,         glGetProgramInfoLog);
    OPENGL_GET_PROC(PFNGLCREATESHADERPROC,              glCreateShader);
    OPENGL_GET_PROC(PFNGLDELETESHADERPROC,              glDeleteShader);
    OPENGL_GET_PROC(PFNGLSHADERSOURCEPROC,              glShaderSource);
    OPENGL_GET_PROC(PFNGLCOMPILESHADERPROC,             glCompileShader);
    OPENGL_GET_PROC(PFNGLATTACHSHADERPROC,              glAttachShader);
    OPENGL_GET_PROC(PFNGLDETACHSHADERPROC,              glDetachShader);
    OPENGL_GET_PROC(PFNGLGETSHADERIVPROC,               glGetShaderiv);
    OPENGL_GET_PROC(PFNGLGETSHADERINFOLOGPROC,          glGetShaderInfoLog);
    // Shaders attributes
    OPENGL_GET_PROC(PFNGLGETATTRIBLOCATIONPROC,         glGetAttribLocation);
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBPOINTERPROC,       glVertexAttribPointer);
    OPENGL_GET_PROC(PFNGLENABLEVERTEXATTRIBARRAYPROC,   glEnableVertexAttribArray);
    OPENGL_GET_PROC(PFNGLDISABLEVERTEXATTRIBARRAYPROC,  glDisableVertexAttribArray);
    // Shaders uniforms
    OPENGL_GET_PROC(PFNGLGETUNIFORMLOCATIONPROC,        glGetUniformLocation);
    OPENGL_GET_PROC(PFNGLUNIFORMMATRIX4FVPROC,          glUniformMatrix4fv);
    return 0;
}

HGLRC GL_rCreateCtx(HDC hDC) {

    // создадим временный контекст рендеринга он нужен для получения функции wglCreateContextAttribsARB
    HGLRC hRCTemp = wglCreateContext(hDC);
    if (!hRCTemp || !wglMakeCurrent(hDC, hRCTemp)){
        LOG(g_uLogApp, LOG_F, TEXT("Сreating temp render context FAIL!\n\terr=%p"), GetLastError());
        return NULL;
    }

    // получим адрес функции установки атрибутов контекста рендеринга
    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)(wglGetProcAddress("wglCreateContextAttribsARB"));

    // временный контекст OpenGL нам больше не нужен
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRCTemp);
    hRCTemp = NULL;

    if(!wglCreateContextAttribsARB) {
        LOG(g_uLogApp, LOG_F, TEXT("wglCreateContextAttribsARB FAIL!\n\terr=%p"), GetLastError());
        return NULL;
    }

    // укажем атрибуты для создания расширенного контекста OpenGL
    INT attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    // создадим расширенный контекст с поддержкой OpenGL 3
    HGLRC hRC = wglCreateContextAttribsARB(hDC, 0, attribs);
    if (!hRC || !wglMakeCurrent(hDC, hRC)) {
        LOG(g_uLogApp, LOG_F, TEXT("Creating render context FAIL!\n\terr=%p"), GetLastError());
        return NULL;
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

    return hRC;
}


LPCSTR GL_INFOSPIXELFLAGS(DWORD dwFlags) {

    if(!dwFlags) return "\n 0 NULL";
    static CHAR buf[LOG_MAX_MESAGELEN/2];
    buf[0] = 0;
    if(dwFlags & PFD_DOUBLEBUFFER) strcat(buf, "\n + DOUBLEBUFFER");
    if(dwFlags & PFD_STEREO) strcat(buf, "\n + STEREO");
    if(dwFlags & PFD_DRAW_TO_WINDOW) strcat(buf, "\n + DRAW_TO_WINDOW");
    if(dwFlags & PFD_DRAW_TO_BITMAP) strcat(buf, "\n + DRAW_TO_BITMAP");
    if(dwFlags & PFD_SUPPORT_GDI) strcat(buf, "\n + SUPPORT_GDI");
    if(dwFlags & PFD_SUPPORT_OPENGL) strcat(buf, "\n + SUPPORT_OPENGL");
    if(dwFlags & PFD_GENERIC_FORMAT) strcat(buf, "\n + GENERIC_FORMAT");
    if(dwFlags & PFD_NEED_PALETTE) strcat(buf, "\n + NEED_PALETTE");
    if(dwFlags & PFD_NEED_SYSTEM_PALETTE) strcat(buf, "\n + NEED_SYSTEM_PALETTE");
    if(dwFlags & PFD_SWAP_EXCHANGE) strcat(buf, "\n + SWAP_EXCHANGE");
    if(dwFlags & PFD_SWAP_COPY) strcat(buf, "\n + SWAP_COPY");
    if(dwFlags & PFD_SWAP_LAYER_BUFFERS) strcat(buf, "\n + SWAP_LAYER_BUFFERS");
    if(dwFlags & PFD_GENERIC_ACCELERATED) strcat(buf, "\n + GENERIC_ACCELERATED");
    if(dwFlags & PFD_DEPTH_DONTCARE) strcat(buf, "\n + DEPTH_DONTCARE");
    if(dwFlags & PFD_DOUBLEBUFFER_DONTCARE) strcat(buf, "\n + DOUBLEBUFFER_DONTCARE");
    if(dwFlags & PFD_STEREO_DONTCARE) strcat(buf, "\n + STEREO_DONTCARE");

    return buf;
}


GLuint  GL_rCreateShader(GLenum shaderType, PBYTE pSrc, INT sz) {
    GLuint o;

    OPENGL_CALL(o = glCreateShader(shaderType));
    OPENGL_CALL(glShaderSource(o, 1, (const GLchar**)&pSrc, (const GLint*)&sz));
    OPENGL_CALL(glCompileShader(o));

    GLint status;
    OPENGL_CALL(glGetShaderiv(o, GL_COMPILE_STATUS, &status));
    if (status != GL_TRUE) {
        GLint length;
        GLchar buffer[1024];
        OPENGL_CALL(glGetShaderInfoLog(o, 1024, &length, buffer));
        LOG(g_uLogOGL, LOG_E, TEXT("Shader: %hs"), buffer);
        OPENGL_CALL(glDeleteShader(o));
        return 0;
    }
    LOG(g_uLogOGL, LOG_D, TEXT("Shader created: %p(%d)"), o, o);
    return o;
}

GLuint  GL_rCreateShaderFromFileA(GLenum shaderType, LPCSTR lpszFileName) {
    PBYTE   pBuf    = NULL;
    UINT    uSz     = 0;
    if (rLoadFileToMem(lpszFileName, &pBuf, &uSz)) {
        if(pBuf) free(pBuf);
        pBuf = NULL;
        return 0;
    }
    GLuint o = GL_rCreateShader(shaderType, pBuf, uSz);
    if(pBuf) free(pBuf);
    pBuf = NULL;
    return o;
}

GLuint  GL_rCreateProgramm(GLuint *pShaders) {
    GLuint o;
    OPENGL_CALL(o = glCreateProgram());
    for(INT i=0; pShaders[i]; ++i) {
        OPENGL_CALL(glAttachShader(o, pShaders[i]));
    }
    OPENGL_CALL(glLinkProgram(o));
    GLint status;
    OPENGL_CALL(glGetProgramiv(o, GL_LINK_STATUS, &status));
    if (status != GL_TRUE) {
        GLint length;
        GLchar buffer[1024];
        OPENGL_CALL(glGetProgramInfoLog(o, 1024, &length, buffer));
        LOG(g_uLogOGL, LOG_E, TEXT("Shader program: %hs"), buffer);
        OPENGL_CALL(glDeleteProgram(o));
        return 0;
    }
    OPENGL_CALL(glValidateProgram(o));
    OPENGL_CALL(glGetProgramiv(o, GL_VALIDATE_STATUS, &status));
    if (status != GL_TRUE) {
        GLint length;
        GLchar buffer[1024];
        OPENGL_CALL(glGetProgramInfoLog(o, 1024, &length, buffer));
        LOG(g_uLogOGL, LOG_E, TEXT("Shader program: %hs"), buffer);
        OPENGL_CALL(glDeleteProgram(o));
        return 0;
    }
    LOG(g_uLogOGL, LOG_D, TEXT("Shader program created: %p(%d)"), o, o);
    return o;
}
