#ifndef _H_GL
#define _H_GL

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include <Windows.h>

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/wglext.h>

#include "log.h"


// объявим расширения OpenGL
// VAO
extern PFNGLGENVERTEXARRAYSPROC            glGenVertexArrays;
extern PFNGLDELETEVERTEXARRAYSPROC         glDeleteVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC            glBindVertexArray;
// VBO
extern PFNGLGENBUFFERSPROC                 glGenBuffers;
extern PFNGLDELETEBUFFERSPROC              glDeleteBuffers;
extern PFNGLBINDBUFFERPROC                 glBindBuffer;
extern PFNGLBUFFERDATAPROC                 glBufferData;
extern PFNGLBUFFERSUBDATAPROC              glBufferSubData;
extern PFNGLMAPBUFFERPROC                  glMapBuffer;
extern PFNGLUNMAPBUFFERPROC                glUnmapBuffer;
// Shaders
extern PFNGLCREATEPROGRAMPROC              glCreateProgram;
extern PFNGLDELETEPROGRAMPROC              glDeleteProgram;
extern PFNGLLINKPROGRAMPROC                glLinkProgram;
extern PFNGLVALIDATEPROGRAMPROC            glValidateProgram;
extern PFNGLUSEPROGRAMPROC                 glUseProgram;
extern PFNGLGETPROGRAMIVPROC               glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC          glGetProgramInfoLog;
extern PFNGLCREATESHADERPROC               glCreateShader;
extern PFNGLDELETESHADERPROC               glDeleteShader;
extern PFNGLSHADERSOURCEPROC               glShaderSource;
extern PFNGLCOMPILESHADERPROC              glCompileShader;
extern PFNGLATTACHSHADERPROC               glAttachShader;
extern PFNGLDETACHSHADERPROC               glDetachShader;
extern PFNGLGETSHADERIVPROC                glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC           glGetShaderInfoLog;
// Shaders attributes
extern PFNGLGETATTRIBLOCATIONPROC          glGetAttribLocation;
extern PFNGLVERTEXATTRIBPOINTERPROC        glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC    glEnableVertexAttribArray;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC   glDisableVertexAttribArray;
// Shaders uniforms
extern PFNGLGETUNIFORMLOCATIONPROC         glGetUniformLocation;
extern PFNGLUNIFORMMATRIX4FVPROC           glUniformMatrix4fv;

// определим указатель на функцию создания расширенного контекста OpenGL
extern PFNWGLCREATECONTEXTATTRIBSARBPROC   wglCreateContextAttribsARB;



// глобальная перменная для хранения ошибки OpenGL
extern GLenum                               g_OpenGLError;
// проверка на ошибки OpenGL
#define OPENGL_CHECK_FOR_ERRORS() \
    if((g_OpenGLError = glGetError()) != GL_NO_ERROR) LOG(g_uLogOGL, LOG_E, TEXT("Error %d(0x%x)"), (INT)g_OpenGLError, (INT)g_OpenGLError);
// безопасный вызов функции OpenGL
#define OPENGL_CALL(exp) \
    { exp; if((g_OpenGLError = glGetError()) != GL_NO_ERROR) LOG(g_uLogOGL, LOG_E, TEXT("expression \"" #exp "\" Error %d(0x%x)"), (INT)g_OpenGLError, (INT)g_OpenGLError); }

HGLRC   GL_rCreateCtx(HDC hDC);
INT     GL_rInitExtensions();

GLuint  GL_rCreateShader(GLenum shaderType, PBYTE pSrc, INT sz);
GLuint  GL_rCreateShaderFromFileA(GLenum shaderType, LPCSTR lpszFileName);
GLuint  GL_rCreateProgramm(GLuint *pShaders);

LPCSTR GL_INFOSPIXELFLAGS(DWORD dwFlags);


#endif
