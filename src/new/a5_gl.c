#include "A5_gl.h"
#include "A5_ut.h"

const GLchar
    sz_aPos[] = "v_pos",
    sz_aCol[] = "v_col",
    sz_aTex[] = "v_tex",
    sz_uPos[] = "u_pos",
    sz_uMat[] = "u_mat",
    sz_uTex[] = "u_tex",
    sz_uTsz[] = "u_tsz";

#define OPENGL_GET_LA(a) \
    if((p->_a##a = glGetAttribLocation(p->p, sz_a##a)) == -1) \
        SDL_Log("Could not bind attribute \"%s\"\n", sz_a##a);

#define OPENGL_GET_LU(a) \
    if((p->_u##a = glGetUniformLocation(p->p, sz_u##a)) == -1) \
        SDL_Log("Could not bind uniform   \"%s\"\n", sz_u##a);



GLuint A5GL_CreateShader(A5pcstr szFileName, GLenum eType) {
    A5puint8 source = A5UT_FileRead(szFileName, NULL);
    if (!source)
        return SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Error opening \"%s\"", szFileName), 0;
    GLuint res = glCreateShader(eType);
    glShaderSource(res, 1, (const GLchar**)&source, NULL);
    A5UT_FileReadFree(source); source = NULL;
    glCompileShader(res);
    GLint compile_ok = GL_FALSE;
    glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
    if (!compile_ok) {
        GLint log_length = 0;
        glGetShaderiv(res, GL_INFO_LOG_LENGTH, &log_length);
        char *log = SDL_stack_alloc(char, log_length);
        glGetShaderInfoLog(res, log_length, NULL, log);
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Error compile \"%s\"\n -  Log: %s", szFileName, log);
        SDL_stack_free(log);
        glDeleteShader(res);
        return 0;
    }
    return res;
}
GLuint A5GL_CreateProgram(GLuint uiVS, GLuint uiFS) {
    GLuint r[3]; r[0] = uiVS; r[1] = uiFS; r[2] = 0;
    return A5GL_CreateProgramV(r);
}
GLuint A5GL_CreateProgramV(GLuint *uiShaders) {
    GLuint program = glCreateProgram();
    for(;*uiShaders;++uiShaders) glAttachShader(program, *uiShaders);
    glLinkProgram(program);
    GLint link_ok = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        GLint log_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
        char* log = SDL_stack_alloc(char, log_length);
        glGetProgramInfoLog(program, log_length, NULL, log);
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Error link program with array shaders\n -  Log: %s", log);
        SDL_stack_free(log);
        glDeleteProgram(program);
        return 0;
    }
    return program;
}


/* Init 2D Colored Text Shader (With Alpha Texture)*/
GLvoid A5GL_2DTXT_Init(A5psGL_2DTXT p) {
    if (!(p->v = A5GL_CreateShader("data/shader/2DTXT.v.glsl", GL_VERTEX_SHADER))) {
        A5GL_2DTXT_Free(p); return; }
    if (!(p->f = A5GL_CreateShader("data/shader/2DTXT.f.glsl", GL_FRAGMENT_SHADER))) {
        A5GL_2DTXT_Free(p); return; }
    if (!(p->p = A5GL_CreateProgram(p->v, p->f))) {
        A5GL_2DTXT_Free(p); return; }
    OPENGL_GET_LA(Pos)
    OPENGL_GET_LA(Tex)
    OPENGL_GET_LA(Col)
    OPENGL_GET_LU(Pos)
    OPENGL_GET_LU(Mat)
    OPENGL_GET_LU(Tex)
    OPENGL_GET_LU(Tsz)
}
GLvoid A5GL_2DTXT_Free(A5psGL_2DTXT p) {
    glDeleteProgram(p->p);
    glDeleteShader (p->v);
    glDeleteShader (p->f);
    SDL_memset(p, 0, sizeof(A5sGL_2DTXT));
}
GLuint A5GL_2DTXT_CreateVBO_Dynamic(GLuint uSize) {
    GLuint o;
    glGenBuffers(1, &o);
    glBindBuffer(GL_ARRAY_BUFFER, o);
    glBufferData(GL_ARRAY_BUFFER, sizeof(A5sv_2DTXT)*uSize*6, NULL, GL_DYNAMIC_DRAW);
    return o;
}
GLuint A5GL_2DTXT_CreateVBO_Static(GLuint uSize, A5psv_2DTXT pBuf) {
    GLuint o;
    glGenBuffers(1, &o);
    glBindBuffer(GL_ARRAY_BUFFER, o);
    glBufferData(GL_ARRAY_BUFFER, sizeof(A5sv_2DTXT)*uSize*6, pBuf, GL_STATIC_DRAW);
    return o;
}
GLvoid A5GL_2DTXT_DrawBegin(A5psGL_2DTXT p, GLuint uScreenWidth, GLuint uScreenHeight) {
    glUseProgram(p->p);
    glUniform1i(p->_uTex, 0);
    glUniform2f(p->_uMat, 2.0f/(GLfloat)uScreenWidth, -2.0f/(GLfloat)uScreenHeight);
}
GLvoid A5GL_2DTXT_DrawSetTexture(A5psGL_2DTXT p, GLuint uTex, GLuint uTexSize) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, uTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glUniform2f(p->_uTsz, 1.0f/(GLfloat)uTexSize, 1.0f/(GLfloat)uTexSize);
}
GLvoid A5GL_2DTXT_DrawSetPos(A5psGL_2DTXT p, GLint iPosX, GLint iPosY) {
    glUniform2f(p->_uPos, (GLfloat)iPosX, (GLfloat)iPosY);
}
GLvoid A5GL_2DTXT_DrawVBO(A5psGL_2DTXT p, GLuint uVBO, GLuint uOffset, GLuint uCount) {
    glBindBuffer(GL_ARRAY_BUFFER, uVBO);
    glEnableVertexAttribArray(p->_aPos);
    glEnableVertexAttribArray(p->_aTex);
    glEnableVertexAttribArray(p->_aCol);
    glVertexAttribPointer(p->_aPos, 2, GL_SHORT, GL_FALSE, sizeof(A5sv_2DTXT), &((A5psv_2DTXT)0)->x);
    glVertexAttribPointer(p->_aTex, 2, GL_SHORT, GL_FALSE, sizeof(A5sv_2DTXT), &((A5psv_2DTXT)0)->u);
    glVertexAttribPointer(p->_aCol, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(A5sv_2DTXT), &((A5psv_2DTXT)0)->r);
    glDrawArrays(GL_TRIANGLES, 6*uOffset, 6*uCount);
    glDisableVertexAttribArray(p->_aPos);
    glDisableVertexAttribArray(p->_aTex);
    glDisableVertexAttribArray(p->_aCol);
}
GLuint A5GL_2DTXT_PrepareVboText(GLuint uVBO, GLuint uOffset, GLuint uCount, A5psSETTINGS_TXT pSettings) {
    glBindBuffer(GL_ARRAY_BUFFER, uVBO);
    A5uint i = 0;
    A5short X = pSettings->iPosX;
    A5short Y = pSettings->iPosY;
    A5psv_2DTXT pv = SDL_stack_alloc(A5V_2DTXT, uCount*6);
    A5pcstr pStr = pzTxt;
    A5uint  uUnicode;
    while(uCount) {
        pStr = A5UT_U8_GetUnicode(pStr, &uUnicode);
        if(!uUnicode) break;
        if(uUnicode == '\n') {
            Y+= pSettings->iHeight;
            X = pSettings->iPosX;
            continue;
        }
        A5psFT_Glyph pG = A5FT_GlyphGet(pSettings->pCache, pSettings->uSizeID, uUnicode);
        if(!pG) pG = A5FT_GlyphNew(pSettings->pCache, pSettings->uSizeID, uUnicode);
        if(!pG) continue;

        if(pG->uCacheWidth && pG->uCacheHeight) {
            pv[i].c = pSettings->c;
            pv[i].x = X+pG->iOffsetX;       pv[i].y = Y+pG->iOffsetY;
            pv[i].u = pG->uCachePosX;       pv[i].v = pG->uCachePosY;
            ++i;                            pv[i]   = pv[i-1];
            pv[i].x+= pG->uCacheWidth;      pv[i].u+= pG->uCacheWidth;
            ++i;                            pv[i]   = pv[i-2];
            pv[i].y+= pG->uCacheHeight;     pv[i].v+= pG->uCacheHeight;
            ++i;                            pv[i]   = pv[i-1];
            ++i;                            pv[i]   = pv[i-3];
            ++i;                            pv[i]   = pv[i-1];
            pv[i].y+= pG->uCacheHeight;     pv[i].v+= pG->uCacheHeight;
            ++i;                            --uCount;
        }
        X += pG->uAdvance + pSettings->iKerning;
    }
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(A5sv_2DTXT)*uOffset*6, sizeof(A5sv_2DTXT)*i, pv);
    SDL_stack_free(pv);
    return i/6;
}









































#define OPENGL_GET_PROC(p,n) \
    ++iM; if ((n = (p)SDL_GL_GetProcAddress(#n))) ++i; else \
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Loading extension '%s' fail\n", #n);
#define OPENGL_STOP_GET_PROC(v) \
    if(i>=iM) o=v; SDL_LogInfo(SDL_LOG_CATEGORY_RENDER, "Loaded %d/%d extension of OpenGL %d.%d\n", \
        i, iM, v>>4, v&0xf);


int A5GL_Init() {
    int i=0, o=0;
    int iM=0;

    OPENGL_GET_PROC(PFNGLDRAWRANGEELEMENTSPROC,                      glDrawRangeElements)
    OPENGL_GET_PROC(PFNGLTEXIMAGE3DPROC,                             glTexImage3D)
    OPENGL_GET_PROC(PFNGLTEXSUBIMAGE3DPROC,                          glTexSubImage3D)
    OPENGL_GET_PROC(PFNGLCOPYTEXSUBIMAGE3DPROC,                      glCopyTexSubImage3D)
    OPENGL_STOP_GET_PROC(0x12)

    OPENGL_GET_PROC(PFNGLACTIVETEXTUREPROC,                          glActiveTexture)
    OPENGL_GET_PROC(PFNGLSAMPLECOVERAGEPROC,                         glSampleCoverage)
    OPENGL_GET_PROC(PFNGLCOMPRESSEDTEXIMAGE3DPROC,                   glCompressedTexImage3D)
    OPENGL_GET_PROC(PFNGLCOMPRESSEDTEXIMAGE2DPROC,                   glCompressedTexImage2D)
    OPENGL_GET_PROC(PFNGLCOMPRESSEDTEXIMAGE1DPROC,                   glCompressedTexImage1D)
    OPENGL_GET_PROC(PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC,                glCompressedTexSubImage3D)
    OPENGL_GET_PROC(PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC,                glCompressedTexSubImage2D)
    OPENGL_GET_PROC(PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC,                glCompressedTexSubImage1D)
    OPENGL_GET_PROC(PFNGLGETCOMPRESSEDTEXIMAGEPROC,                  glGetCompressedTexImage)
    OPENGL_GET_PROC(PFNGLCLIENTACTIVETEXTUREPROC,                    glClientActiveTexture)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD1DPROC,                        glMultiTexCoord1d)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD1DVPROC,                       glMultiTexCoord1dv)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD1FPROC,                        glMultiTexCoord1f)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD1FVPROC,                       glMultiTexCoord1fv)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD1IPROC,                        glMultiTexCoord1i)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD1IVPROC,                       glMultiTexCoord1iv)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD1SPROC,                        glMultiTexCoord1s)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD1SVPROC,                       glMultiTexCoord1sv)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD2DPROC,                        glMultiTexCoord2d)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD2DVPROC,                       glMultiTexCoord2dv)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD2FPROC,                        glMultiTexCoord2f)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD2FVPROC,                       glMultiTexCoord2fv)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD2IPROC,                        glMultiTexCoord2i)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD2IVPROC,                       glMultiTexCoord2iv)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD2SPROC,                        glMultiTexCoord2s)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD2SVPROC,                       glMultiTexCoord2sv)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD3DPROC,                        glMultiTexCoord3d)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD3DVPROC,                       glMultiTexCoord3dv)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD3FPROC,                        glMultiTexCoord3f)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD3FVPROC,                       glMultiTexCoord3fv)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD3IPROC,                        glMultiTexCoord3i)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD3IVPROC,                       glMultiTexCoord3iv)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD3SPROC,                        glMultiTexCoord3s)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD3SVPROC,                       glMultiTexCoord3sv)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD4DPROC,                        glMultiTexCoord4d)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD4DVPROC,                       glMultiTexCoord4dv)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD4FPROC,                        glMultiTexCoord4f)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD4FVPROC,                       glMultiTexCoord4fv)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD4IPROC,                        glMultiTexCoord4i)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD4IVPROC,                       glMultiTexCoord4iv)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD4SPROC,                        glMultiTexCoord4s)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORD4SVPROC,                       glMultiTexCoord4sv)
    OPENGL_GET_PROC(PFNGLLOADTRANSPOSEMATRIXFPROC,                   glLoadTransposeMatrixf)
    OPENGL_GET_PROC(PFNGLLOADTRANSPOSEMATRIXDPROC,                   glLoadTransposeMatrixd)
    OPENGL_GET_PROC(PFNGLMULTTRANSPOSEMATRIXFPROC,                   glMultTransposeMatrixf)
    OPENGL_GET_PROC(PFNGLMULTTRANSPOSEMATRIXDPROC,                   glMultTransposeMatrixd)
    OPENGL_STOP_GET_PROC(0x13)

    OPENGL_GET_PROC(PFNGLBLENDFUNCSEPARATEPROC,                      glBlendFuncSeparate)
    OPENGL_GET_PROC(PFNGLMULTIDRAWARRAYSPROC,                        glMultiDrawArrays)
    OPENGL_GET_PROC(PFNGLMULTIDRAWELEMENTSPROC,                      glMultiDrawElements)
    OPENGL_GET_PROC(PFNGLPOINTPARAMETERFPROC,                        glPointParameterf)
    OPENGL_GET_PROC(PFNGLPOINTPARAMETERFVPROC,                       glPointParameterfv)
    OPENGL_GET_PROC(PFNGLPOINTPARAMETERIPROC,                        glPointParameteri)
    OPENGL_GET_PROC(PFNGLPOINTPARAMETERIVPROC,                       glPointParameteriv)
    OPENGL_GET_PROC(PFNGLFOGCOORDFPROC,                              glFogCoordf)
    OPENGL_GET_PROC(PFNGLFOGCOORDFVPROC,                             glFogCoordfv)
    OPENGL_GET_PROC(PFNGLFOGCOORDDPROC,                              glFogCoordd)
    OPENGL_GET_PROC(PFNGLFOGCOORDDVPROC,                             glFogCoorddv)
    OPENGL_GET_PROC(PFNGLFOGCOORDPOINTERPROC,                        glFogCoordPointer)
    OPENGL_GET_PROC(PFNGLSECONDARYCOLOR3BPROC,                       glSecondaryColor3b)
    OPENGL_GET_PROC(PFNGLSECONDARYCOLOR3BVPROC,                      glSecondaryColor3bv)
    OPENGL_GET_PROC(PFNGLSECONDARYCOLOR3DPROC,                       glSecondaryColor3d)
    OPENGL_GET_PROC(PFNGLSECONDARYCOLOR3DVPROC,                      glSecondaryColor3dv)
    OPENGL_GET_PROC(PFNGLSECONDARYCOLOR3FPROC,                       glSecondaryColor3f)
    OPENGL_GET_PROC(PFNGLSECONDARYCOLOR3FVPROC,                      glSecondaryColor3fv)
    OPENGL_GET_PROC(PFNGLSECONDARYCOLOR3IPROC,                       glSecondaryColor3i)
    OPENGL_GET_PROC(PFNGLSECONDARYCOLOR3IVPROC,                      glSecondaryColor3iv)
    OPENGL_GET_PROC(PFNGLSECONDARYCOLOR3SPROC,                       glSecondaryColor3s)
    OPENGL_GET_PROC(PFNGLSECONDARYCOLOR3SVPROC,                      glSecondaryColor3sv)
    OPENGL_GET_PROC(PFNGLSECONDARYCOLOR3UBPROC,                      glSecondaryColor3ub)
    OPENGL_GET_PROC(PFNGLSECONDARYCOLOR3UBVPROC,                     glSecondaryColor3ubv)
    OPENGL_GET_PROC(PFNGLSECONDARYCOLOR3UIPROC,                      glSecondaryColor3ui)
    OPENGL_GET_PROC(PFNGLSECONDARYCOLOR3UIVPROC,                     glSecondaryColor3uiv)
    OPENGL_GET_PROC(PFNGLSECONDARYCOLOR3USPROC,                      glSecondaryColor3us)
    OPENGL_GET_PROC(PFNGLSECONDARYCOLOR3USVPROC,                     glSecondaryColor3usv)
    OPENGL_GET_PROC(PFNGLSECONDARYCOLORPOINTERPROC,                  glSecondaryColorPointer)
    OPENGL_GET_PROC(PFNGLWINDOWPOS2DPROC,                            glWindowPos2d)
    OPENGL_GET_PROC(PFNGLWINDOWPOS2DVPROC,                           glWindowPos2dv)
    OPENGL_GET_PROC(PFNGLWINDOWPOS2FPROC,                            glWindowPos2f)
    OPENGL_GET_PROC(PFNGLWINDOWPOS2FVPROC,                           glWindowPos2fv)
    OPENGL_GET_PROC(PFNGLWINDOWPOS2IPROC,                            glWindowPos2i)
    OPENGL_GET_PROC(PFNGLWINDOWPOS2IVPROC,                           glWindowPos2iv)
    OPENGL_GET_PROC(PFNGLWINDOWPOS2SPROC,                            glWindowPos2s)
    OPENGL_GET_PROC(PFNGLWINDOWPOS2SVPROC,                           glWindowPos2sv)
    OPENGL_GET_PROC(PFNGLWINDOWPOS3DPROC,                            glWindowPos3d)
    OPENGL_GET_PROC(PFNGLWINDOWPOS3DVPROC,                           glWindowPos3dv)
    OPENGL_GET_PROC(PFNGLWINDOWPOS3FPROC,                            glWindowPos3f)
    OPENGL_GET_PROC(PFNGLWINDOWPOS3FVPROC,                           glWindowPos3fv)
    OPENGL_GET_PROC(PFNGLWINDOWPOS3IPROC,                            glWindowPos3i)
    OPENGL_GET_PROC(PFNGLWINDOWPOS3IVPROC,                           glWindowPos3iv)
    OPENGL_GET_PROC(PFNGLWINDOWPOS3SPROC,                            glWindowPos3s)
    OPENGL_GET_PROC(PFNGLWINDOWPOS3SVPROC,                           glWindowPos3sv)
    OPENGL_GET_PROC(PFNGLBLENDCOLORPROC,                             glBlendColor)
    OPENGL_GET_PROC(PFNGLBLENDEQUATIONPROC,                          glBlendEquation)
    OPENGL_STOP_GET_PROC(0x14)

    OPENGL_GET_PROC(PFNGLGENQUERIESPROC,                             glGenQueries)
    OPENGL_GET_PROC(PFNGLDELETEQUERIESPROC,                          glDeleteQueries)
    OPENGL_GET_PROC(PFNGLISQUERYPROC,                                glIsQuery)
    OPENGL_GET_PROC(PFNGLBEGINQUERYPROC,                             glBeginQuery)
    OPENGL_GET_PROC(PFNGLENDQUERYPROC,                               glEndQuery)
    OPENGL_GET_PROC(PFNGLGETQUERYIVPROC,                             glGetQueryiv)
    OPENGL_GET_PROC(PFNGLGETQUERYOBJECTIVPROC,                       glGetQueryObjectiv)
    OPENGL_GET_PROC(PFNGLGETQUERYOBJECTUIVPROC,                      glGetQueryObjectuiv)
    OPENGL_GET_PROC(PFNGLBINDBUFFERPROC,                             glBindBuffer)
    OPENGL_GET_PROC(PFNGLDELETEBUFFERSPROC,                          glDeleteBuffers)
    OPENGL_GET_PROC(PFNGLGENBUFFERSPROC,                             glGenBuffers)
    OPENGL_GET_PROC(PFNGLISBUFFERPROC,                               glIsBuffer)
    OPENGL_GET_PROC(PFNGLBUFFERDATAPROC,                             glBufferData)
    OPENGL_GET_PROC(PFNGLBUFFERSUBDATAPROC,                          glBufferSubData)
    OPENGL_GET_PROC(PFNGLGETBUFFERSUBDATAPROC,                       glGetBufferSubData)
    OPENGL_GET_PROC(PFNGLMAPBUFFERPROC,                              glMapBuffer)
    OPENGL_GET_PROC(PFNGLUNMAPBUFFERPROC,                            glUnmapBuffer)
    OPENGL_GET_PROC(PFNGLGETBUFFERPARAMETERIVPROC,                   glGetBufferParameteriv)
    OPENGL_GET_PROC(PFNGLGETBUFFERPOINTERVPROC,                      glGetBufferPointerv)
    OPENGL_STOP_GET_PROC(0x15)

    OPENGL_GET_PROC(PFNGLBLENDEQUATIONSEPARATEPROC,                  glBlendEquationSeparate)
    OPENGL_GET_PROC(PFNGLDRAWBUFFERSPROC,                            glDrawBuffers)
    OPENGL_GET_PROC(PFNGLSTENCILOPSEPARATEPROC,                      glStencilOpSeparate)
    OPENGL_GET_PROC(PFNGLSTENCILFUNCSEPARATEPROC,                    glStencilFuncSeparate)
    OPENGL_GET_PROC(PFNGLSTENCILMASKSEPARATEPROC,                    glStencilMaskSeparate)
    OPENGL_GET_PROC(PFNGLATTACHSHADERPROC,                           glAttachShader)
    OPENGL_GET_PROC(PFNGLBINDATTRIBLOCATIONPROC,                     glBindAttribLocation)
    OPENGL_GET_PROC(PFNGLCOMPILESHADERPROC,                          glCompileShader)
    OPENGL_GET_PROC(PFNGLCREATEPROGRAMPROC,                          glCreateProgram)
    OPENGL_GET_PROC(PFNGLCREATESHADERPROC,                           glCreateShader)
    OPENGL_GET_PROC(PFNGLDELETEPROGRAMPROC,                          glDeleteProgram)
    OPENGL_GET_PROC(PFNGLDELETESHADERPROC,                           glDeleteShader)
    OPENGL_GET_PROC(PFNGLDETACHSHADERPROC,                           glDetachShader)
    OPENGL_GET_PROC(PFNGLDISABLEVERTEXATTRIBARRAYPROC,               glDisableVertexAttribArray)
    OPENGL_GET_PROC(PFNGLENABLEVERTEXATTRIBARRAYPROC,                glEnableVertexAttribArray)
    OPENGL_GET_PROC(PFNGLGETACTIVEATTRIBPROC,                        glGetActiveAttrib)
    OPENGL_GET_PROC(PFNGLGETACTIVEUNIFORMPROC,                       glGetActiveUniform)
    OPENGL_GET_PROC(PFNGLGETATTACHEDSHADERSPROC,                     glGetAttachedShaders)
    OPENGL_GET_PROC(PFNGLGETATTRIBLOCATIONPROC,                      glGetAttribLocation)
    OPENGL_GET_PROC(PFNGLGETPROGRAMIVPROC,                           glGetProgramiv)
    OPENGL_GET_PROC(PFNGLGETPROGRAMINFOLOGPROC,                      glGetProgramInfoLog)
    OPENGL_GET_PROC(PFNGLGETSHADERIVPROC,                            glGetShaderiv)
    OPENGL_GET_PROC(PFNGLGETSHADERINFOLOGPROC,                       glGetShaderInfoLog)
    OPENGL_GET_PROC(PFNGLGETSHADERSOURCEPROC,                        glGetShaderSource)
    OPENGL_GET_PROC(PFNGLGETUNIFORMLOCATIONPROC,                     glGetUniformLocation)
    OPENGL_GET_PROC(PFNGLGETUNIFORMFVPROC,                           glGetUniformfv)
    OPENGL_GET_PROC(PFNGLGETUNIFORMIVPROC,                           glGetUniformiv)
    OPENGL_GET_PROC(PFNGLGETVERTEXATTRIBDVPROC,                      glGetVertexAttribdv)
    OPENGL_GET_PROC(PFNGLGETVERTEXATTRIBFVPROC,                      glGetVertexAttribfv)
    OPENGL_GET_PROC(PFNGLGETVERTEXATTRIBIVPROC,                      glGetVertexAttribiv)
    OPENGL_GET_PROC(PFNGLGETVERTEXATTRIBPOINTERVPROC,                glGetVertexAttribPointerv)
    OPENGL_GET_PROC(PFNGLISPROGRAMPROC,                              glIsProgram)
    OPENGL_GET_PROC(PFNGLISSHADERPROC,                               glIsShader)
    OPENGL_GET_PROC(PFNGLLINKPROGRAMPROC,                            glLinkProgram)
    OPENGL_GET_PROC(PFNGLSHADERSOURCEPROC,                           glShaderSource)
    OPENGL_GET_PROC(PFNGLUSEPROGRAMPROC,                             glUseProgram)
    OPENGL_GET_PROC(PFNGLUNIFORM1FPROC,                              glUniform1f)
    OPENGL_GET_PROC(PFNGLUNIFORM2FPROC,                              glUniform2f)
    OPENGL_GET_PROC(PFNGLUNIFORM3FPROC,                              glUniform3f)
    OPENGL_GET_PROC(PFNGLUNIFORM4FPROC,                              glUniform4f)
    OPENGL_GET_PROC(PFNGLUNIFORM1IPROC,                              glUniform1i)
    OPENGL_GET_PROC(PFNGLUNIFORM2IPROC,                              glUniform2i)
    OPENGL_GET_PROC(PFNGLUNIFORM3IPROC,                              glUniform3i)
    OPENGL_GET_PROC(PFNGLUNIFORM4IPROC,                              glUniform4i)
    OPENGL_GET_PROC(PFNGLUNIFORM1FVPROC,                             glUniform1fv)
    OPENGL_GET_PROC(PFNGLUNIFORM2FVPROC,                             glUniform2fv)
    OPENGL_GET_PROC(PFNGLUNIFORM3FVPROC,                             glUniform3fv)
    OPENGL_GET_PROC(PFNGLUNIFORM4FVPROC,                             glUniform4fv)
    OPENGL_GET_PROC(PFNGLUNIFORM1IVPROC,                             glUniform1iv)
    OPENGL_GET_PROC(PFNGLUNIFORM2IVPROC,                             glUniform2iv)
    OPENGL_GET_PROC(PFNGLUNIFORM3IVPROC,                             glUniform3iv)
    OPENGL_GET_PROC(PFNGLUNIFORM4IVPROC,                             glUniform4iv)
    OPENGL_GET_PROC(PFNGLUNIFORMMATRIX2FVPROC,                       glUniformMatrix2fv)
    OPENGL_GET_PROC(PFNGLUNIFORMMATRIX3FVPROC,                       glUniformMatrix3fv)
    OPENGL_GET_PROC(PFNGLUNIFORMMATRIX4FVPROC,                       glUniformMatrix4fv)
    OPENGL_GET_PROC(PFNGLVALIDATEPROGRAMPROC,                        glValidateProgram)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB1DPROC,                         glVertexAttrib1d)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB1DVPROC,                        glVertexAttrib1dv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB1FPROC,                         glVertexAttrib1f)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB1FVPROC,                        glVertexAttrib1fv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB1SPROC,                         glVertexAttrib1s)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB1SVPROC,                        glVertexAttrib1sv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB2DPROC,                         glVertexAttrib2d)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB2DVPROC,                        glVertexAttrib2dv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB2FPROC,                         glVertexAttrib2f)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB2FVPROC,                        glVertexAttrib2fv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB2SPROC,                         glVertexAttrib2s)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB2SVPROC,                        glVertexAttrib2sv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB3DPROC,                         glVertexAttrib3d)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB3DVPROC,                        glVertexAttrib3dv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB3FPROC,                         glVertexAttrib3f)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB3FVPROC,                        glVertexAttrib3fv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB3SPROC,                         glVertexAttrib3s)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB3SVPROC,                        glVertexAttrib3sv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB4NBVPROC,                       glVertexAttrib4Nbv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB4NIVPROC,                       glVertexAttrib4Niv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB4NSVPROC,                       glVertexAttrib4Nsv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB4NUBPROC,                       glVertexAttrib4Nub)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB4NUBVPROC,                      glVertexAttrib4Nubv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB4NUIVPROC,                      glVertexAttrib4Nuiv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB4NUSVPROC,                      glVertexAttrib4Nusv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB4BVPROC,                        glVertexAttrib4bv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB4DPROC,                         glVertexAttrib4d)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB4DVPROC,                        glVertexAttrib4dv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB4FPROC,                         glVertexAttrib4f)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB4FVPROC,                        glVertexAttrib4fv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB4IVPROC,                        glVertexAttrib4iv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB4SPROC,                         glVertexAttrib4s)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB4SVPROC,                        glVertexAttrib4sv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB4UBVPROC,                       glVertexAttrib4ubv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB4UIVPROC,                       glVertexAttrib4uiv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIB4USVPROC,                       glVertexAttrib4usv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBPOINTERPROC,                    glVertexAttribPointer)
    OPENGL_STOP_GET_PROC(0x20)

    OPENGL_GET_PROC(PFNGLUNIFORMMATRIX2X3FVPROC,                     glUniformMatrix2x3fv)
    OPENGL_GET_PROC(PFNGLUNIFORMMATRIX3X2FVPROC,                     glUniformMatrix3x2fv)
    OPENGL_GET_PROC(PFNGLUNIFORMMATRIX2X4FVPROC,                     glUniformMatrix2x4fv)
    OPENGL_GET_PROC(PFNGLUNIFORMMATRIX4X2FVPROC,                     glUniformMatrix4x2fv)
    OPENGL_GET_PROC(PFNGLUNIFORMMATRIX3X4FVPROC,                     glUniformMatrix3x4fv)
    OPENGL_GET_PROC(PFNGLUNIFORMMATRIX4X3FVPROC,                     glUniformMatrix4x3fv)
    OPENGL_STOP_GET_PROC(0x21)

    OPENGL_GET_PROC(PFNGLCOLORMASKIPROC,                             glColorMaski)
    OPENGL_GET_PROC(PFNGLGETBOOLEANI_VPROC,                          glGetBooleani_v)
    OPENGL_GET_PROC(PFNGLGETINTEGERI_VPROC,                          glGetIntegeri_v)
    OPENGL_GET_PROC(PFNGLENABLEIPROC,                                glEnablei)
    OPENGL_GET_PROC(PFNGLDISABLEIPROC,                               glDisablei)
    OPENGL_GET_PROC(PFNGLISENABLEDIPROC,                             glIsEnabledi)
    OPENGL_GET_PROC(PFNGLBEGINTRANSFORMFEEDBACKPROC,                 glBeginTransformFeedback)
    OPENGL_GET_PROC(PFNGLENDTRANSFORMFEEDBACKPROC,                   glEndTransformFeedback)
    OPENGL_GET_PROC(PFNGLBINDBUFFERRANGEPROC,                        glBindBufferRange)
    OPENGL_GET_PROC(PFNGLBINDBUFFERBASEPROC,                         glBindBufferBase)
    OPENGL_GET_PROC(PFNGLTRANSFORMFEEDBACKVARYINGSPROC,              glTransformFeedbackVaryings)
    OPENGL_GET_PROC(PFNGLGETTRANSFORMFEEDBACKVARYINGPROC,            glGetTransformFeedbackVarying)
    OPENGL_GET_PROC(PFNGLCLAMPCOLORPROC,                             glClampColor)
    OPENGL_GET_PROC(PFNGLBEGINCONDITIONALRENDERPROC,                 glBeginConditionalRender)
    OPENGL_GET_PROC(PFNGLENDCONDITIONALRENDERPROC,                   glEndConditionalRender)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBIPOINTERPROC,                   glVertexAttribIPointer)
    OPENGL_GET_PROC(PFNGLGETVERTEXATTRIBIIVPROC,                     glGetVertexAttribIiv)
    OPENGL_GET_PROC(PFNGLGETVERTEXATTRIBIUIVPROC,                    glGetVertexAttribIuiv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBI1IPROC,                        glVertexAttribI1i)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBI2IPROC,                        glVertexAttribI2i)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBI3IPROC,                        glVertexAttribI3i)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBI4IPROC,                        glVertexAttribI4i)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBI1UIPROC,                       glVertexAttribI1ui)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBI2UIPROC,                       glVertexAttribI2ui)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBI3UIPROC,                       glVertexAttribI3ui)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBI4UIPROC,                       glVertexAttribI4ui)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBI1IVPROC,                       glVertexAttribI1iv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBI2IVPROC,                       glVertexAttribI2iv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBI3IVPROC,                       glVertexAttribI3iv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBI4IVPROC,                       glVertexAttribI4iv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBI1UIVPROC,                      glVertexAttribI1uiv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBI2UIVPROC,                      glVertexAttribI2uiv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBI3UIVPROC,                      glVertexAttribI3uiv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBI4UIVPROC,                      glVertexAttribI4uiv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBI4BVPROC,                       glVertexAttribI4bv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBI4SVPROC,                       glVertexAttribI4sv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBI4UBVPROC,                      glVertexAttribI4ubv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBI4USVPROC,                      glVertexAttribI4usv)
    OPENGL_GET_PROC(PFNGLGETUNIFORMUIVPROC,                          glGetUniformuiv)
    OPENGL_GET_PROC(PFNGLBINDFRAGDATALOCATIONPROC,                   glBindFragDataLocation)
    OPENGL_GET_PROC(PFNGLGETFRAGDATALOCATIONPROC,                    glGetFragDataLocation)
    OPENGL_GET_PROC(PFNGLUNIFORM1UIPROC,                             glUniform1ui)
    OPENGL_GET_PROC(PFNGLUNIFORM2UIPROC,                             glUniform2ui)
    OPENGL_GET_PROC(PFNGLUNIFORM3UIPROC,                             glUniform3ui)
    OPENGL_GET_PROC(PFNGLUNIFORM4UIPROC,                             glUniform4ui)
    OPENGL_GET_PROC(PFNGLUNIFORM1UIVPROC,                            glUniform1uiv)
    OPENGL_GET_PROC(PFNGLUNIFORM2UIVPROC,                            glUniform2uiv)
    OPENGL_GET_PROC(PFNGLUNIFORM3UIVPROC,                            glUniform3uiv)
    OPENGL_GET_PROC(PFNGLUNIFORM4UIVPROC,                            glUniform4uiv)
    OPENGL_GET_PROC(PFNGLTEXPARAMETERIIVPROC,                        glTexParameterIiv)
    OPENGL_GET_PROC(PFNGLTEXPARAMETERIUIVPROC,                       glTexParameterIuiv)
    OPENGL_GET_PROC(PFNGLGETTEXPARAMETERIIVPROC,                     glGetTexParameterIiv)
    OPENGL_GET_PROC(PFNGLGETTEXPARAMETERIUIVPROC,                    glGetTexParameterIuiv)
    OPENGL_GET_PROC(PFNGLCLEARBUFFERIVPROC,                          glClearBufferiv)
    OPENGL_GET_PROC(PFNGLCLEARBUFFERUIVPROC,                         glClearBufferuiv)
    OPENGL_GET_PROC(PFNGLCLEARBUFFERFVPROC,                          glClearBufferfv)
    OPENGL_GET_PROC(PFNGLCLEARBUFFERFIPROC,                          glClearBufferfi)
    OPENGL_GET_PROC(PFNGLGETSTRINGIPROC,                             glGetStringi)
    OPENGL_GET_PROC(PFNGLISRENDERBUFFERPROC,                         glIsRenderbuffer)
    OPENGL_GET_PROC(PFNGLBINDRENDERBUFFERPROC,                       glBindRenderbuffer)
    OPENGL_GET_PROC(PFNGLDELETERENDERBUFFERSPROC,                    glDeleteRenderbuffers)
    OPENGL_GET_PROC(PFNGLGENRENDERBUFFERSPROC,                       glGenRenderbuffers)
    OPENGL_GET_PROC(PFNGLRENDERBUFFERSTORAGEPROC,                    glRenderbufferStorage)
    OPENGL_GET_PROC(PFNGLGETRENDERBUFFERPARAMETERIVPROC,             glGetRenderbufferParameteriv)
    OPENGL_GET_PROC(PFNGLISFRAMEBUFFERPROC,                          glIsFramebuffer)
    OPENGL_GET_PROC(PFNGLBINDFRAMEBUFFERPROC,                        glBindFramebuffer)
    OPENGL_GET_PROC(PFNGLDELETEFRAMEBUFFERSPROC,                     glDeleteFramebuffers)
    OPENGL_GET_PROC(PFNGLGENFRAMEBUFFERSPROC,                        glGenFramebuffers)
    OPENGL_GET_PROC(PFNGLCHECKFRAMEBUFFERSTATUSPROC,                 glCheckFramebufferStatus)
    OPENGL_GET_PROC(PFNGLFRAMEBUFFERTEXTURE1DPROC,                   glFramebufferTexture1D)
    OPENGL_GET_PROC(PFNGLFRAMEBUFFERTEXTURE2DPROC,                   glFramebufferTexture2D)
    OPENGL_GET_PROC(PFNGLFRAMEBUFFERTEXTURE3DPROC,                   glFramebufferTexture3D)
    OPENGL_GET_PROC(PFNGLFRAMEBUFFERRENDERBUFFERPROC,                glFramebufferRenderbuffer)
    OPENGL_GET_PROC(PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC,    glGetFramebufferAttachmentParameteriv)
    OPENGL_GET_PROC(PFNGLGENERATEMIPMAPPROC,                         glGenerateMipmap)
    OPENGL_GET_PROC(PFNGLBLITFRAMEBUFFERPROC,                        glBlitFramebuffer)
    OPENGL_GET_PROC(PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC,         glRenderbufferStorageMultisample)
    OPENGL_GET_PROC(PFNGLFRAMEBUFFERTEXTURELAYERPROC,                glFramebufferTextureLayer)
    OPENGL_GET_PROC(PFNGLMAPBUFFERRANGEPROC,                         glMapBufferRange)
    OPENGL_GET_PROC(PFNGLFLUSHMAPPEDBUFFERRANGEPROC,                 glFlushMappedBufferRange)
    OPENGL_GET_PROC(PFNGLBINDVERTEXARRAYPROC,                        glBindVertexArray)
    OPENGL_GET_PROC(PFNGLDELETEVERTEXARRAYSPROC,                     glDeleteVertexArrays)
    OPENGL_GET_PROC(PFNGLGENVERTEXARRAYSPROC,                        glGenVertexArrays)
    OPENGL_GET_PROC(PFNGLISVERTEXARRAYPROC,                          glIsVertexArray)
    OPENGL_STOP_GET_PROC(0x30)

    OPENGL_GET_PROC(PFNGLDRAWARRAYSINSTANCEDPROC,                    glDrawArraysInstanced)
    OPENGL_GET_PROC(PFNGLDRAWELEMENTSINSTANCEDPROC,                  glDrawElementsInstanced)
    OPENGL_GET_PROC(PFNGLTEXBUFFERPROC,                              glTexBuffer)
    OPENGL_GET_PROC(PFNGLPRIMITIVERESTARTINDEXPROC,                  glPrimitiveRestartIndex)
    OPENGL_GET_PROC(PFNGLCOPYBUFFERSUBDATAPROC,                      glCopyBufferSubData)
    OPENGL_GET_PROC(PFNGLGETUNIFORMINDICESPROC,                      glGetUniformIndices)
    OPENGL_GET_PROC(PFNGLGETACTIVEUNIFORMSIVPROC,                    glGetActiveUniformsiv)
    OPENGL_GET_PROC(PFNGLGETACTIVEUNIFORMNAMEPROC,                   glGetActiveUniformName)
    OPENGL_GET_PROC(PFNGLGETUNIFORMBLOCKINDEXPROC,                   glGetUniformBlockIndex)
    OPENGL_GET_PROC(PFNGLGETACTIVEUNIFORMBLOCKIVPROC,                glGetActiveUniformBlockiv)
    OPENGL_GET_PROC(PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC,              glGetActiveUniformBlockName)
    OPENGL_GET_PROC(PFNGLUNIFORMBLOCKBINDINGPROC,                    glUniformBlockBinding)
    OPENGL_STOP_GET_PROC(0x31)

    OPENGL_GET_PROC(PFNGLDRAWELEMENTSBASEVERTEXPROC,                 glDrawElementsBaseVertex)
    OPENGL_GET_PROC(PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC,            glDrawRangeElementsBaseVertex)
    OPENGL_GET_PROC(PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC,        glDrawElementsInstancedBaseVertex)
    OPENGL_GET_PROC(PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC,            glMultiDrawElementsBaseVertex)
    OPENGL_GET_PROC(PFNGLPROVOKINGVERTEXPROC,                        glProvokingVertex)
    OPENGL_GET_PROC(PFNGLFENCESYNCPROC,                              glFenceSync)
    OPENGL_GET_PROC(PFNGLISSYNCPROC,                                 glIsSync)
    OPENGL_GET_PROC(PFNGLDELETESYNCPROC,                             glDeleteSync)
    OPENGL_GET_PROC(PFNGLCLIENTWAITSYNCPROC,                         glClientWaitSync)
    OPENGL_GET_PROC(PFNGLWAITSYNCPROC,                               glWaitSync)
    OPENGL_GET_PROC(PFNGLGETINTEGER64VPROC,                          glGetInteger64v)
    OPENGL_GET_PROC(PFNGLGETSYNCIVPROC,                              glGetSynciv)
    OPENGL_GET_PROC(PFNGLGETINTEGER64I_VPROC,                        glGetInteger64i_v)
    OPENGL_GET_PROC(PFNGLGETBUFFERPARAMETERI64VPROC,                 glGetBufferParameteri64v)
    OPENGL_GET_PROC(PFNGLFRAMEBUFFERTEXTUREPROC,                     glFramebufferTexture)
    OPENGL_GET_PROC(PFNGLTEXIMAGE2DMULTISAMPLEPROC,                  glTexImage2DMultisample)
    OPENGL_GET_PROC(PFNGLTEXIMAGE3DMULTISAMPLEPROC,                  glTexImage3DMultisample)
    OPENGL_GET_PROC(PFNGLGETMULTISAMPLEFVPROC,                       glGetMultisamplefv)
    OPENGL_GET_PROC(PFNGLSAMPLEMASKIPROC,                            glSampleMaski)
    OPENGL_STOP_GET_PROC(0x32)

    OPENGL_GET_PROC(PFNGLBINDFRAGDATALOCATIONINDEXEDPROC,            glBindFragDataLocationIndexed)
    OPENGL_GET_PROC(PFNGLGETFRAGDATAINDEXPROC,                       glGetFragDataIndex)
    OPENGL_GET_PROC(PFNGLGENSAMPLERSPROC,                            glGenSamplers)
    OPENGL_GET_PROC(PFNGLDELETESAMPLERSPROC,                         glDeleteSamplers)
    OPENGL_GET_PROC(PFNGLISSAMPLERPROC,                              glIsSampler)
    OPENGL_GET_PROC(PFNGLBINDSAMPLERPROC,                            glBindSampler)
    OPENGL_GET_PROC(PFNGLSAMPLERPARAMETERIPROC,                      glSamplerParameteri)
    OPENGL_GET_PROC(PFNGLSAMPLERPARAMETERIVPROC,                     glSamplerParameteriv)
    OPENGL_GET_PROC(PFNGLSAMPLERPARAMETERFPROC,                      glSamplerParameterf)
    OPENGL_GET_PROC(PFNGLSAMPLERPARAMETERFVPROC,                     glSamplerParameterfv)
    OPENGL_GET_PROC(PFNGLSAMPLERPARAMETERIIVPROC,                    glSamplerParameterIiv)
    OPENGL_GET_PROC(PFNGLSAMPLERPARAMETERIUIVPROC,                   glSamplerParameterIuiv)
    OPENGL_GET_PROC(PFNGLGETSAMPLERPARAMETERIVPROC,                  glGetSamplerParameteriv)
    OPENGL_GET_PROC(PFNGLGETSAMPLERPARAMETERIIVPROC,                 glGetSamplerParameterIiv)
    OPENGL_GET_PROC(PFNGLGETSAMPLERPARAMETERFVPROC,                  glGetSamplerParameterfv)
    OPENGL_GET_PROC(PFNGLGETSAMPLERPARAMETERIUIVPROC,                glGetSamplerParameterIuiv)
    OPENGL_GET_PROC(PFNGLQUERYCOUNTERPROC,                           glQueryCounter)
    OPENGL_GET_PROC(PFNGLGETQUERYOBJECTI64VPROC,                     glGetQueryObjecti64v)
    OPENGL_GET_PROC(PFNGLGETQUERYOBJECTUI64VPROC,                    glGetQueryObjectui64v)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBDIVISORPROC,                    glVertexAttribDivisor)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBP1UIPROC,                       glVertexAttribP1ui)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBP1UIVPROC,                      glVertexAttribP1uiv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBP2UIPROC,                       glVertexAttribP2ui)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBP2UIVPROC,                      glVertexAttribP2uiv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBP3UIPROC,                       glVertexAttribP3ui)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBP3UIVPROC,                      glVertexAttribP3uiv)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBP4UIPROC,                       glVertexAttribP4ui)
    OPENGL_GET_PROC(PFNGLVERTEXATTRIBP4UIVPROC,                      glVertexAttribP4uiv)
    OPENGL_GET_PROC(PFNGLVERTEXP2UIPROC,                             glVertexP2ui)
    OPENGL_GET_PROC(PFNGLVERTEXP2UIVPROC,                            glVertexP2uiv)
    OPENGL_GET_PROC(PFNGLVERTEXP3UIPROC,                             glVertexP3ui)
    OPENGL_GET_PROC(PFNGLVERTEXP3UIVPROC,                            glVertexP3uiv)
    OPENGL_GET_PROC(PFNGLVERTEXP4UIPROC,                             glVertexP4ui)
    OPENGL_GET_PROC(PFNGLVERTEXP4UIVPROC,                            glVertexP4uiv)
    OPENGL_GET_PROC(PFNGLTEXCOORDP1UIPROC,                           glTexCoordP1ui)
    OPENGL_GET_PROC(PFNGLTEXCOORDP1UIVPROC,                          glTexCoordP1uiv)
    OPENGL_GET_PROC(PFNGLTEXCOORDP2UIPROC,                           glTexCoordP2ui)
    OPENGL_GET_PROC(PFNGLTEXCOORDP2UIVPROC,                          glTexCoordP2uiv)
    OPENGL_GET_PROC(PFNGLTEXCOORDP3UIPROC,                           glTexCoordP3ui)
    OPENGL_GET_PROC(PFNGLTEXCOORDP3UIVPROC,                          glTexCoordP3uiv)
    OPENGL_GET_PROC(PFNGLTEXCOORDP4UIPROC,                           glTexCoordP4ui)
    OPENGL_GET_PROC(PFNGLTEXCOORDP4UIVPROC,                          glTexCoordP4uiv)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORDP1UIPROC,                      glMultiTexCoordP1ui)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORDP1UIVPROC,                     glMultiTexCoordP1uiv)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORDP2UIPROC,                      glMultiTexCoordP2ui)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORDP2UIVPROC,                     glMultiTexCoordP2uiv)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORDP3UIPROC,                      glMultiTexCoordP3ui)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORDP3UIVPROC,                     glMultiTexCoordP3uiv)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORDP4UIPROC,                      glMultiTexCoordP4ui)
    OPENGL_GET_PROC(PFNGLMULTITEXCOORDP4UIVPROC,                     glMultiTexCoordP4uiv)
    OPENGL_GET_PROC(PFNGLNORMALP3UIPROC,                             glNormalP3ui)
    OPENGL_GET_PROC(PFNGLNORMALP3UIVPROC,                            glNormalP3uiv)
    OPENGL_GET_PROC(PFNGLCOLORP3UIPROC,                              glColorP3ui)
    OPENGL_GET_PROC(PFNGLCOLORP3UIVPROC,                             glColorP3uiv)
    OPENGL_GET_PROC(PFNGLCOLORP4UIPROC,                              glColorP4ui)
    OPENGL_GET_PROC(PFNGLCOLORP4UIVPROC,                             glColorP4uiv)
    OPENGL_GET_PROC(PFNGLSECONDARYCOLORP3UIPROC,                     glSecondaryColorP3ui)
    OPENGL_GET_PROC(PFNGLSECONDARYCOLORP3UIVPROC,                    glSecondaryColorP3uiv)
    OPENGL_STOP_GET_PROC(0x33)

    return o;
}

/* GL_VERSION_1_2 */
PFNGLDRAWRANGEELEMENTSPROC                      glDrawRangeElements                     = NULL;
PFNGLTEXIMAGE3DPROC                             glTexImage3D                            = NULL;
PFNGLTEXSUBIMAGE3DPROC                          glTexSubImage3D                         = NULL;
PFNGLCOPYTEXSUBIMAGE3DPROC                      glCopyTexSubImage3D                     = NULL;
/* GL_VERSION_1_3 */
PFNGLACTIVETEXTUREPROC                          glActiveTexture                         = NULL;
PFNGLSAMPLECOVERAGEPROC                         glSampleCoverage                        = NULL;
PFNGLCOMPRESSEDTEXIMAGE3DPROC                   glCompressedTexImage3D                  = NULL;
PFNGLCOMPRESSEDTEXIMAGE2DPROC                   glCompressedTexImage2D                  = NULL;
PFNGLCOMPRESSEDTEXIMAGE1DPROC                   glCompressedTexImage1D                  = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC                glCompressedTexSubImage3D               = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC                glCompressedTexSubImage2D               = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC                glCompressedTexSubImage1D               = NULL;
PFNGLGETCOMPRESSEDTEXIMAGEPROC                  glGetCompressedTexImage                 = NULL;
PFNGLCLIENTACTIVETEXTUREPROC                    glClientActiveTexture                   = NULL;
PFNGLMULTITEXCOORD1DPROC                        glMultiTexCoord1d                       = NULL;
PFNGLMULTITEXCOORD1DVPROC                       glMultiTexCoord1dv                      = NULL;
PFNGLMULTITEXCOORD1FPROC                        glMultiTexCoord1f                       = NULL;
PFNGLMULTITEXCOORD1FVPROC                       glMultiTexCoord1fv                      = NULL;
PFNGLMULTITEXCOORD1IPROC                        glMultiTexCoord1i                       = NULL;
PFNGLMULTITEXCOORD1IVPROC                       glMultiTexCoord1iv                      = NULL;
PFNGLMULTITEXCOORD1SPROC                        glMultiTexCoord1s                       = NULL;
PFNGLMULTITEXCOORD1SVPROC                       glMultiTexCoord1sv                      = NULL;
PFNGLMULTITEXCOORD2DPROC                        glMultiTexCoord2d                       = NULL;
PFNGLMULTITEXCOORD2DVPROC                       glMultiTexCoord2dv                      = NULL;
PFNGLMULTITEXCOORD2FPROC                        glMultiTexCoord2f                       = NULL;
PFNGLMULTITEXCOORD2FVPROC                       glMultiTexCoord2fv                      = NULL;
PFNGLMULTITEXCOORD2IPROC                        glMultiTexCoord2i                       = NULL;
PFNGLMULTITEXCOORD2IVPROC                       glMultiTexCoord2iv                      = NULL;
PFNGLMULTITEXCOORD2SPROC                        glMultiTexCoord2s                       = NULL;
PFNGLMULTITEXCOORD2SVPROC                       glMultiTexCoord2sv                      = NULL;
PFNGLMULTITEXCOORD3DPROC                        glMultiTexCoord3d                       = NULL;
PFNGLMULTITEXCOORD3DVPROC                       glMultiTexCoord3dv                      = NULL;
PFNGLMULTITEXCOORD3FPROC                        glMultiTexCoord3f                       = NULL;
PFNGLMULTITEXCOORD3FVPROC                       glMultiTexCoord3fv                      = NULL;
PFNGLMULTITEXCOORD3IPROC                        glMultiTexCoord3i                       = NULL;
PFNGLMULTITEXCOORD3IVPROC                       glMultiTexCoord3iv                      = NULL;
PFNGLMULTITEXCOORD3SPROC                        glMultiTexCoord3s                       = NULL;
PFNGLMULTITEXCOORD3SVPROC                       glMultiTexCoord3sv                      = NULL;
PFNGLMULTITEXCOORD4DPROC                        glMultiTexCoord4d                       = NULL;
PFNGLMULTITEXCOORD4DVPROC                       glMultiTexCoord4dv                      = NULL;
PFNGLMULTITEXCOORD4FPROC                        glMultiTexCoord4f                       = NULL;
PFNGLMULTITEXCOORD4FVPROC                       glMultiTexCoord4fv                      = NULL;
PFNGLMULTITEXCOORD4IPROC                        glMultiTexCoord4i                       = NULL;
PFNGLMULTITEXCOORD4IVPROC                       glMultiTexCoord4iv                      = NULL;
PFNGLMULTITEXCOORD4SPROC                        glMultiTexCoord4s                       = NULL;
PFNGLMULTITEXCOORD4SVPROC                       glMultiTexCoord4sv                      = NULL;
PFNGLLOADTRANSPOSEMATRIXFPROC                   glLoadTransposeMatrixf                  = NULL;
PFNGLLOADTRANSPOSEMATRIXDPROC                   glLoadTransposeMatrixd                  = NULL;
PFNGLMULTTRANSPOSEMATRIXFPROC                   glMultTransposeMatrixf                  = NULL;
PFNGLMULTTRANSPOSEMATRIXDPROC                   glMultTransposeMatrixd                  = NULL;
/* GL_VERSION_1_4 */
PFNGLBLENDFUNCSEPARATEPROC                      glBlendFuncSeparate                     = NULL;
PFNGLMULTIDRAWARRAYSPROC                        glMultiDrawArrays                       = NULL;
PFNGLMULTIDRAWELEMENTSPROC                      glMultiDrawElements                     = NULL;
PFNGLPOINTPARAMETERFPROC                        glPointParameterf                       = NULL;
PFNGLPOINTPARAMETERFVPROC                       glPointParameterfv                      = NULL;
PFNGLPOINTPARAMETERIPROC                        glPointParameteri                       = NULL;
PFNGLPOINTPARAMETERIVPROC                       glPointParameteriv                      = NULL;
PFNGLFOGCOORDFPROC                              glFogCoordf                             = NULL;
PFNGLFOGCOORDFVPROC                             glFogCoordfv                            = NULL;
PFNGLFOGCOORDDPROC                              glFogCoordd                             = NULL;
PFNGLFOGCOORDDVPROC                             glFogCoorddv                            = NULL;
PFNGLFOGCOORDPOINTERPROC                        glFogCoordPointer                       = NULL;
PFNGLSECONDARYCOLOR3BPROC                       glSecondaryColor3b                      = NULL;
PFNGLSECONDARYCOLOR3BVPROC                      glSecondaryColor3bv                     = NULL;
PFNGLSECONDARYCOLOR3DPROC                       glSecondaryColor3d                      = NULL;
PFNGLSECONDARYCOLOR3DVPROC                      glSecondaryColor3dv                     = NULL;
PFNGLSECONDARYCOLOR3FPROC                       glSecondaryColor3f                      = NULL;
PFNGLSECONDARYCOLOR3FVPROC                      glSecondaryColor3fv                     = NULL;
PFNGLSECONDARYCOLOR3IPROC                       glSecondaryColor3i                      = NULL;
PFNGLSECONDARYCOLOR3IVPROC                      glSecondaryColor3iv                     = NULL;
PFNGLSECONDARYCOLOR3SPROC                       glSecondaryColor3s                      = NULL;
PFNGLSECONDARYCOLOR3SVPROC                      glSecondaryColor3sv                     = NULL;
PFNGLSECONDARYCOLOR3UBPROC                      glSecondaryColor3ub                     = NULL;
PFNGLSECONDARYCOLOR3UBVPROC                     glSecondaryColor3ubv                    = NULL;
PFNGLSECONDARYCOLOR3UIPROC                      glSecondaryColor3ui                     = NULL;
PFNGLSECONDARYCOLOR3UIVPROC                     glSecondaryColor3uiv                    = NULL;
PFNGLSECONDARYCOLOR3USPROC                      glSecondaryColor3us                     = NULL;
PFNGLSECONDARYCOLOR3USVPROC                     glSecondaryColor3usv                    = NULL;
PFNGLSECONDARYCOLORPOINTERPROC                  glSecondaryColorPointer                 = NULL;
PFNGLWINDOWPOS2DPROC                            glWindowPos2d                           = NULL;
PFNGLWINDOWPOS2DVPROC                           glWindowPos2dv                          = NULL;
PFNGLWINDOWPOS2FPROC                            glWindowPos2f                           = NULL;
PFNGLWINDOWPOS2FVPROC                           glWindowPos2fv                          = NULL;
PFNGLWINDOWPOS2IPROC                            glWindowPos2i                           = NULL;
PFNGLWINDOWPOS2IVPROC                           glWindowPos2iv                          = NULL;
PFNGLWINDOWPOS2SPROC                            glWindowPos2s                           = NULL;
PFNGLWINDOWPOS2SVPROC                           glWindowPos2sv                          = NULL;
PFNGLWINDOWPOS3DPROC                            glWindowPos3d                           = NULL;
PFNGLWINDOWPOS3DVPROC                           glWindowPos3dv                          = NULL;
PFNGLWINDOWPOS3FPROC                            glWindowPos3f                           = NULL;
PFNGLWINDOWPOS3FVPROC                           glWindowPos3fv                          = NULL;
PFNGLWINDOWPOS3IPROC                            glWindowPos3i                           = NULL;
PFNGLWINDOWPOS3IVPROC                           glWindowPos3iv                          = NULL;
PFNGLWINDOWPOS3SPROC                            glWindowPos3s                           = NULL;
PFNGLWINDOWPOS3SVPROC                           glWindowPos3sv                          = NULL;
PFNGLBLENDCOLORPROC                             glBlendColor                            = NULL;
PFNGLBLENDEQUATIONPROC                          glBlendEquation                         = NULL;
/* GL_VERSION_1_5 */
PFNGLGENQUERIESPROC                             glGenQueries                            = NULL;
PFNGLDELETEQUERIESPROC                          glDeleteQueries                         = NULL;
PFNGLISQUERYPROC                                glIsQuery                               = NULL;
PFNGLBEGINQUERYPROC                             glBeginQuery                            = NULL;
PFNGLENDQUERYPROC                               glEndQuery                              = NULL;
PFNGLGETQUERYIVPROC                             glGetQueryiv                            = NULL;
PFNGLGETQUERYOBJECTIVPROC                       glGetQueryObjectiv                      = NULL;
PFNGLGETQUERYOBJECTUIVPROC                      glGetQueryObjectuiv                     = NULL;
PFNGLBINDBUFFERPROC                             glBindBuffer                            = NULL;
PFNGLDELETEBUFFERSPROC                          glDeleteBuffers                         = NULL;
PFNGLGENBUFFERSPROC                             glGenBuffers                            = NULL;
PFNGLISBUFFERPROC                               glIsBuffer                              = NULL;
PFNGLBUFFERDATAPROC                             glBufferData                            = NULL;
PFNGLBUFFERSUBDATAPROC                          glBufferSubData                         = NULL;
PFNGLGETBUFFERSUBDATAPROC                       glGetBufferSubData                      = NULL;
PFNGLMAPBUFFERPROC                              glMapBuffer                             = NULL;
PFNGLUNMAPBUFFERPROC                            glUnmapBuffer                           = NULL;
PFNGLGETBUFFERPARAMETERIVPROC                   glGetBufferParameteriv                  = NULL;
PFNGLGETBUFFERPOINTERVPROC                      glGetBufferPointerv                     = NULL;
/* GL_VERSION_2_0 */
PFNGLBLENDEQUATIONSEPARATEPROC                  glBlendEquationSeparate                 = NULL;
PFNGLDRAWBUFFERSPROC                            glDrawBuffers                           = NULL;
PFNGLSTENCILOPSEPARATEPROC                      glStencilOpSeparate                     = NULL;
PFNGLSTENCILFUNCSEPARATEPROC                    glStencilFuncSeparate                   = NULL;
PFNGLSTENCILMASKSEPARATEPROC                    glStencilMaskSeparate                   = NULL;
PFNGLATTACHSHADERPROC                           glAttachShader                          = NULL;
PFNGLBINDATTRIBLOCATIONPROC                     glBindAttribLocation                    = NULL;
PFNGLCOMPILESHADERPROC                          glCompileShader                         = NULL;
PFNGLCREATEPROGRAMPROC                          glCreateProgram                         = NULL;
PFNGLCREATESHADERPROC                           glCreateShader                          = NULL;
PFNGLDELETEPROGRAMPROC                          glDeleteProgram                         = NULL;
PFNGLDELETESHADERPROC                           glDeleteShader                          = NULL;
PFNGLDETACHSHADERPROC                           glDetachShader                          = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC               glDisableVertexAttribArray              = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC                glEnableVertexAttribArray               = NULL;
PFNGLGETACTIVEATTRIBPROC                        glGetActiveAttrib                       = NULL;
PFNGLGETACTIVEUNIFORMPROC                       glGetActiveUniform                      = NULL;
PFNGLGETATTACHEDSHADERSPROC                     glGetAttachedShaders                    = NULL;
PFNGLGETATTRIBLOCATIONPROC                      glGetAttribLocation                     = NULL;
PFNGLGETPROGRAMIVPROC                           glGetProgramiv                          = NULL;
PFNGLGETPROGRAMINFOLOGPROC                      glGetProgramInfoLog                     = NULL;
PFNGLGETSHADERIVPROC                            glGetShaderiv                           = NULL;
PFNGLGETSHADERINFOLOGPROC                       glGetShaderInfoLog                      = NULL;
PFNGLGETSHADERSOURCEPROC                        glGetShaderSource                       = NULL;
PFNGLGETUNIFORMLOCATIONPROC                     glGetUniformLocation                    = NULL;
PFNGLGETUNIFORMFVPROC                           glGetUniformfv                          = NULL;
PFNGLGETUNIFORMIVPROC                           glGetUniformiv                          = NULL;
PFNGLGETVERTEXATTRIBDVPROC                      glGetVertexAttribdv                     = NULL;
PFNGLGETVERTEXATTRIBFVPROC                      glGetVertexAttribfv                     = NULL;
PFNGLGETVERTEXATTRIBIVPROC                      glGetVertexAttribiv                     = NULL;
PFNGLGETVERTEXATTRIBPOINTERVPROC                glGetVertexAttribPointerv               = NULL;
PFNGLISPROGRAMPROC                              glIsProgram                             = NULL;
PFNGLISSHADERPROC                               glIsShader                              = NULL;
PFNGLLINKPROGRAMPROC                            glLinkProgram                           = NULL;
PFNGLSHADERSOURCEPROC                           glShaderSource                          = NULL;
PFNGLUSEPROGRAMPROC                             glUseProgram                            = NULL;
PFNGLUNIFORM1FPROC                              glUniform1f                             = NULL;
PFNGLUNIFORM2FPROC                              glUniform2f                             = NULL;
PFNGLUNIFORM3FPROC                              glUniform3f                             = NULL;
PFNGLUNIFORM4FPROC                              glUniform4f                             = NULL;
PFNGLUNIFORM1IPROC                              glUniform1i                             = NULL;
PFNGLUNIFORM2IPROC                              glUniform2i                             = NULL;
PFNGLUNIFORM3IPROC                              glUniform3i                             = NULL;
PFNGLUNIFORM4IPROC                              glUniform4i                             = NULL;
PFNGLUNIFORM1FVPROC                             glUniform1fv                            = NULL;
PFNGLUNIFORM2FVPROC                             glUniform2fv                            = NULL;
PFNGLUNIFORM3FVPROC                             glUniform3fv                            = NULL;
PFNGLUNIFORM4FVPROC                             glUniform4fv                            = NULL;
PFNGLUNIFORM1IVPROC                             glUniform1iv                            = NULL;
PFNGLUNIFORM2IVPROC                             glUniform2iv                            = NULL;
PFNGLUNIFORM3IVPROC                             glUniform3iv                            = NULL;
PFNGLUNIFORM4IVPROC                             glUniform4iv                            = NULL;
PFNGLUNIFORMMATRIX2FVPROC                       glUniformMatrix2fv                      = NULL;
PFNGLUNIFORMMATRIX3FVPROC                       glUniformMatrix3fv                      = NULL;
PFNGLUNIFORMMATRIX4FVPROC                       glUniformMatrix4fv                      = NULL;
PFNGLVALIDATEPROGRAMPROC                        glValidateProgram                       = NULL;
PFNGLVERTEXATTRIB1DPROC                         glVertexAttrib1d                        = NULL;
PFNGLVERTEXATTRIB1DVPROC                        glVertexAttrib1dv                       = NULL;
PFNGLVERTEXATTRIB1FPROC                         glVertexAttrib1f                        = NULL;
PFNGLVERTEXATTRIB1FVPROC                        glVertexAttrib1fv                       = NULL;
PFNGLVERTEXATTRIB1SPROC                         glVertexAttrib1s                        = NULL;
PFNGLVERTEXATTRIB1SVPROC                        glVertexAttrib1sv                       = NULL;
PFNGLVERTEXATTRIB2DPROC                         glVertexAttrib2d                        = NULL;
PFNGLVERTEXATTRIB2DVPROC                        glVertexAttrib2dv                       = NULL;
PFNGLVERTEXATTRIB2FPROC                         glVertexAttrib2f                        = NULL;
PFNGLVERTEXATTRIB2FVPROC                        glVertexAttrib2fv                       = NULL;
PFNGLVERTEXATTRIB2SPROC                         glVertexAttrib2s                        = NULL;
PFNGLVERTEXATTRIB2SVPROC                        glVertexAttrib2sv                       = NULL;
PFNGLVERTEXATTRIB3DPROC                         glVertexAttrib3d                        = NULL;
PFNGLVERTEXATTRIB3DVPROC                        glVertexAttrib3dv                       = NULL;
PFNGLVERTEXATTRIB3FPROC                         glVertexAttrib3f                        = NULL;
PFNGLVERTEXATTRIB3FVPROC                        glVertexAttrib3fv                       = NULL;
PFNGLVERTEXATTRIB3SPROC                         glVertexAttrib3s                        = NULL;
PFNGLVERTEXATTRIB3SVPROC                        glVertexAttrib3sv                       = NULL;
PFNGLVERTEXATTRIB4NBVPROC                       glVertexAttrib4Nbv                      = NULL;
PFNGLVERTEXATTRIB4NIVPROC                       glVertexAttrib4Niv                      = NULL;
PFNGLVERTEXATTRIB4NSVPROC                       glVertexAttrib4Nsv                      = NULL;
PFNGLVERTEXATTRIB4NUBPROC                       glVertexAttrib4Nub                      = NULL;
PFNGLVERTEXATTRIB4NUBVPROC                      glVertexAttrib4Nubv                     = NULL;
PFNGLVERTEXATTRIB4NUIVPROC                      glVertexAttrib4Nuiv                     = NULL;
PFNGLVERTEXATTRIB4NUSVPROC                      glVertexAttrib4Nusv                     = NULL;
PFNGLVERTEXATTRIB4BVPROC                        glVertexAttrib4bv                       = NULL;
PFNGLVERTEXATTRIB4DPROC                         glVertexAttrib4d                        = NULL;
PFNGLVERTEXATTRIB4DVPROC                        glVertexAttrib4dv                       = NULL;
PFNGLVERTEXATTRIB4FPROC                         glVertexAttrib4f                        = NULL;
PFNGLVERTEXATTRIB4FVPROC                        glVertexAttrib4fv                       = NULL;
PFNGLVERTEXATTRIB4IVPROC                        glVertexAttrib4iv                       = NULL;
PFNGLVERTEXATTRIB4SPROC                         glVertexAttrib4s                        = NULL;
PFNGLVERTEXATTRIB4SVPROC                        glVertexAttrib4sv                       = NULL;
PFNGLVERTEXATTRIB4UBVPROC                       glVertexAttrib4ubv                      = NULL;
PFNGLVERTEXATTRIB4UIVPROC                       glVertexAttrib4uiv                      = NULL;
PFNGLVERTEXATTRIB4USVPROC                       glVertexAttrib4usv                      = NULL;
PFNGLVERTEXATTRIBPOINTERPROC                    glVertexAttribPointer                   = NULL;
/* GL_VERSION_2_1 */
PFNGLUNIFORMMATRIX2X3FVPROC                     glUniformMatrix2x3fv                    = NULL;
PFNGLUNIFORMMATRIX3X2FVPROC                     glUniformMatrix3x2fv                    = NULL;
PFNGLUNIFORMMATRIX2X4FVPROC                     glUniformMatrix2x4fv                    = NULL;
PFNGLUNIFORMMATRIX4X2FVPROC                     glUniformMatrix4x2fv                    = NULL;
PFNGLUNIFORMMATRIX3X4FVPROC                     glUniformMatrix3x4fv                    = NULL;
PFNGLUNIFORMMATRIX4X3FVPROC                     glUniformMatrix4x3fv                    = NULL;
/* GL_VERSION_3_0 */
PFNGLCOLORMASKIPROC                             glColorMaski                            = NULL;
PFNGLGETBOOLEANI_VPROC                          glGetBooleani_v                         = NULL;
PFNGLGETINTEGERI_VPROC                          glGetIntegeri_v                         = NULL;
PFNGLENABLEIPROC                                glEnablei                               = NULL;
PFNGLDISABLEIPROC                               glDisablei                              = NULL;
PFNGLISENABLEDIPROC                             glIsEnabledi                            = NULL;
PFNGLBEGINTRANSFORMFEEDBACKPROC                 glBeginTransformFeedback                = NULL;
PFNGLENDTRANSFORMFEEDBACKPROC                   glEndTransformFeedback                  = NULL;
PFNGLBINDBUFFERRANGEPROC                        glBindBufferRange                       = NULL;
PFNGLBINDBUFFERBASEPROC                         glBindBufferBase                        = NULL;
PFNGLTRANSFORMFEEDBACKVARYINGSPROC              glTransformFeedbackVaryings             = NULL;
PFNGLGETTRANSFORMFEEDBACKVARYINGPROC            glGetTransformFeedbackVarying           = NULL;
PFNGLCLAMPCOLORPROC                             glClampColor                            = NULL;
PFNGLBEGINCONDITIONALRENDERPROC                 glBeginConditionalRender                = NULL;
PFNGLENDCONDITIONALRENDERPROC                   glEndConditionalRender                  = NULL;
PFNGLVERTEXATTRIBIPOINTERPROC                   glVertexAttribIPointer                  = NULL;
PFNGLGETVERTEXATTRIBIIVPROC                     glGetVertexAttribIiv                    = NULL;
PFNGLGETVERTEXATTRIBIUIVPROC                    glGetVertexAttribIuiv                   = NULL;
PFNGLVERTEXATTRIBI1IPROC                        glVertexAttribI1i                       = NULL;
PFNGLVERTEXATTRIBI2IPROC                        glVertexAttribI2i                       = NULL;
PFNGLVERTEXATTRIBI3IPROC                        glVertexAttribI3i                       = NULL;
PFNGLVERTEXATTRIBI4IPROC                        glVertexAttribI4i                       = NULL;
PFNGLVERTEXATTRIBI1UIPROC                       glVertexAttribI1ui                      = NULL;
PFNGLVERTEXATTRIBI2UIPROC                       glVertexAttribI2ui                      = NULL;
PFNGLVERTEXATTRIBI3UIPROC                       glVertexAttribI3ui                      = NULL;
PFNGLVERTEXATTRIBI4UIPROC                       glVertexAttribI4ui                      = NULL;
PFNGLVERTEXATTRIBI1IVPROC                       glVertexAttribI1iv                      = NULL;
PFNGLVERTEXATTRIBI2IVPROC                       glVertexAttribI2iv                      = NULL;
PFNGLVERTEXATTRIBI3IVPROC                       glVertexAttribI3iv                      = NULL;
PFNGLVERTEXATTRIBI4IVPROC                       glVertexAttribI4iv                      = NULL;
PFNGLVERTEXATTRIBI1UIVPROC                      glVertexAttribI1uiv                     = NULL;
PFNGLVERTEXATTRIBI2UIVPROC                      glVertexAttribI2uiv                     = NULL;
PFNGLVERTEXATTRIBI3UIVPROC                      glVertexAttribI3uiv                     = NULL;
PFNGLVERTEXATTRIBI4UIVPROC                      glVertexAttribI4uiv                     = NULL;
PFNGLVERTEXATTRIBI4BVPROC                       glVertexAttribI4bv                      = NULL;
PFNGLVERTEXATTRIBI4SVPROC                       glVertexAttribI4sv                      = NULL;
PFNGLVERTEXATTRIBI4UBVPROC                      glVertexAttribI4ubv                     = NULL;
PFNGLVERTEXATTRIBI4USVPROC                      glVertexAttribI4usv                     = NULL;
PFNGLGETUNIFORMUIVPROC                          glGetUniformuiv                         = NULL;
PFNGLBINDFRAGDATALOCATIONPROC                   glBindFragDataLocation                  = NULL;
PFNGLGETFRAGDATALOCATIONPROC                    glGetFragDataLocation                   = NULL;
PFNGLUNIFORM1UIPROC                             glUniform1ui                            = NULL;
PFNGLUNIFORM2UIPROC                             glUniform2ui                            = NULL;
PFNGLUNIFORM3UIPROC                             glUniform3ui                            = NULL;
PFNGLUNIFORM4UIPROC                             glUniform4ui                            = NULL;
PFNGLUNIFORM1UIVPROC                            glUniform1uiv                           = NULL;
PFNGLUNIFORM2UIVPROC                            glUniform2uiv                           = NULL;
PFNGLUNIFORM3UIVPROC                            glUniform3uiv                           = NULL;
PFNGLUNIFORM4UIVPROC                            glUniform4uiv                           = NULL;
PFNGLTEXPARAMETERIIVPROC                        glTexParameterIiv                       = NULL;
PFNGLTEXPARAMETERIUIVPROC                       glTexParameterIuiv                      = NULL;
PFNGLGETTEXPARAMETERIIVPROC                     glGetTexParameterIiv                    = NULL;
PFNGLGETTEXPARAMETERIUIVPROC                    glGetTexParameterIuiv                   = NULL;
PFNGLCLEARBUFFERIVPROC                          glClearBufferiv                         = NULL;
PFNGLCLEARBUFFERUIVPROC                         glClearBufferuiv                        = NULL;
PFNGLCLEARBUFFERFVPROC                          glClearBufferfv                         = NULL;
PFNGLCLEARBUFFERFIPROC                          glClearBufferfi                         = NULL;
PFNGLGETSTRINGIPROC                             glGetStringi                            = NULL;
PFNGLISRENDERBUFFERPROC                         glIsRenderbuffer                        = NULL;
PFNGLBINDRENDERBUFFERPROC                       glBindRenderbuffer                      = NULL;
PFNGLDELETERENDERBUFFERSPROC                    glDeleteRenderbuffers                   = NULL;
PFNGLGENRENDERBUFFERSPROC                       glGenRenderbuffers                      = NULL;
PFNGLRENDERBUFFERSTORAGEPROC                    glRenderbufferStorage                   = NULL;
PFNGLGETRENDERBUFFERPARAMETERIVPROC             glGetRenderbufferParameteriv            = NULL;
PFNGLISFRAMEBUFFERPROC                          glIsFramebuffer                         = NULL;
PFNGLBINDFRAMEBUFFERPROC                        glBindFramebuffer                       = NULL;
PFNGLDELETEFRAMEBUFFERSPROC                     glDeleteFramebuffers                    = NULL;
PFNGLGENFRAMEBUFFERSPROC                        glGenFramebuffers                       = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSPROC                 glCheckFramebufferStatus                = NULL;
PFNGLFRAMEBUFFERTEXTURE1DPROC                   glFramebufferTexture1D                  = NULL;
PFNGLFRAMEBUFFERTEXTURE2DPROC                   glFramebufferTexture2D                  = NULL;
PFNGLFRAMEBUFFERTEXTURE3DPROC                   glFramebufferTexture3D                  = NULL;
PFNGLFRAMEBUFFERRENDERBUFFERPROC                glFramebufferRenderbuffer               = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC    glGetFramebufferAttachmentParameteriv   = NULL;
PFNGLGENERATEMIPMAPPROC                         glGenerateMipmap                        = NULL;
PFNGLBLITFRAMEBUFFERPROC                        glBlitFramebuffer                       = NULL;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC         glRenderbufferStorageMultisample        = NULL;
PFNGLFRAMEBUFFERTEXTURELAYERPROC                glFramebufferTextureLayer               = NULL;
PFNGLMAPBUFFERRANGEPROC                         glMapBufferRange                        = NULL;
PFNGLFLUSHMAPPEDBUFFERRANGEPROC                 glFlushMappedBufferRange                = NULL;
PFNGLBINDVERTEXARRAYPROC                        glBindVertexArray                       = NULL;
PFNGLDELETEVERTEXARRAYSPROC                     glDeleteVertexArrays                    = NULL;
PFNGLGENVERTEXARRAYSPROC                        glGenVertexArrays                       = NULL;
PFNGLISVERTEXARRAYPROC                          glIsVertexArray                         = NULL;
/* GL_VERSION_3_1 */
PFNGLDRAWARRAYSINSTANCEDPROC                    glDrawArraysInstanced                   = NULL;
PFNGLDRAWELEMENTSINSTANCEDPROC                  glDrawElementsInstanced                 = NULL;
PFNGLTEXBUFFERPROC                              glTexBuffer                             = NULL;
PFNGLPRIMITIVERESTARTINDEXPROC                  glPrimitiveRestartIndex                 = NULL;
PFNGLCOPYBUFFERSUBDATAPROC                      glCopyBufferSubData                     = NULL;
PFNGLGETUNIFORMINDICESPROC                      glGetUniformIndices                     = NULL;
PFNGLGETACTIVEUNIFORMSIVPROC                    glGetActiveUniformsiv                   = NULL;
PFNGLGETACTIVEUNIFORMNAMEPROC                   glGetActiveUniformName                  = NULL;
PFNGLGETUNIFORMBLOCKINDEXPROC                   glGetUniformBlockIndex                  = NULL;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC                glGetActiveUniformBlockiv               = NULL;
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC              glGetActiveUniformBlockName             = NULL;
PFNGLUNIFORMBLOCKBINDINGPROC                    glUniformBlockBinding                   = NULL;
/* GL_VERSION_3_2 */
PFNGLDRAWELEMENTSBASEVERTEXPROC                 glDrawElementsBaseVertex                = NULL;
PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC            glDrawRangeElementsBaseVertex           = NULL;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC        glDrawElementsInstancedBaseVertex       = NULL;
PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC            glMultiDrawElementsBaseVertex           = NULL;
PFNGLPROVOKINGVERTEXPROC                        glProvokingVertex                       = NULL;
PFNGLFENCESYNCPROC                              glFenceSync                             = NULL;
PFNGLISSYNCPROC                                 glIsSync                                = NULL;
PFNGLDELETESYNCPROC                             glDeleteSync                            = NULL;
PFNGLCLIENTWAITSYNCPROC                         glClientWaitSync                        = NULL;
PFNGLWAITSYNCPROC                               glWaitSync                              = NULL;
PFNGLGETINTEGER64VPROC                          glGetInteger64v                         = NULL;
PFNGLGETSYNCIVPROC                              glGetSynciv                             = NULL;
PFNGLGETINTEGER64I_VPROC                        glGetInteger64i_v                       = NULL;
PFNGLGETBUFFERPARAMETERI64VPROC                 glGetBufferParameteri64v                = NULL;
PFNGLFRAMEBUFFERTEXTUREPROC                     glFramebufferTexture                    = NULL;
PFNGLTEXIMAGE2DMULTISAMPLEPROC                  glTexImage2DMultisample                 = NULL;
PFNGLTEXIMAGE3DMULTISAMPLEPROC                  glTexImage3DMultisample                 = NULL;
PFNGLGETMULTISAMPLEFVPROC                       glGetMultisamplefv                      = NULL;
PFNGLSAMPLEMASKIPROC                            glSampleMaski                           = NULL;
/* GL_VERSION_3_3 */
PFNGLBINDFRAGDATALOCATIONINDEXEDPROC            glBindFragDataLocationIndexed           = NULL;
PFNGLGETFRAGDATAINDEXPROC                       glGetFragDataIndex                      = NULL;
PFNGLGENSAMPLERSPROC                            glGenSamplers                           = NULL;
PFNGLDELETESAMPLERSPROC                         glDeleteSamplers                        = NULL;
PFNGLISSAMPLERPROC                              glIsSampler                             = NULL;
PFNGLBINDSAMPLERPROC                            glBindSampler                           = NULL;
PFNGLSAMPLERPARAMETERIPROC                      glSamplerParameteri                     = NULL;
PFNGLSAMPLERPARAMETERIVPROC                     glSamplerParameteriv                    = NULL;
PFNGLSAMPLERPARAMETERFPROC                      glSamplerParameterf                     = NULL;
PFNGLSAMPLERPARAMETERFVPROC                     glSamplerParameterfv                    = NULL;
PFNGLSAMPLERPARAMETERIIVPROC                    glSamplerParameterIiv                   = NULL;
PFNGLSAMPLERPARAMETERIUIVPROC                   glSamplerParameterIuiv                  = NULL;
PFNGLGETSAMPLERPARAMETERIVPROC                  glGetSamplerParameteriv                 = NULL;
PFNGLGETSAMPLERPARAMETERIIVPROC                 glGetSamplerParameterIiv                = NULL;
PFNGLGETSAMPLERPARAMETERFVPROC                  glGetSamplerParameterfv                 = NULL;
PFNGLGETSAMPLERPARAMETERIUIVPROC                glGetSamplerParameterIuiv               = NULL;
PFNGLQUERYCOUNTERPROC                           glQueryCounter                          = NULL;
PFNGLGETQUERYOBJECTI64VPROC                     glGetQueryObjecti64v                    = NULL;
PFNGLGETQUERYOBJECTUI64VPROC                    glGetQueryObjectui64v                   = NULL;
PFNGLVERTEXATTRIBDIVISORPROC                    glVertexAttribDivisor                   = NULL;
PFNGLVERTEXATTRIBP1UIPROC                       glVertexAttribP1ui                      = NULL;
PFNGLVERTEXATTRIBP1UIVPROC                      glVertexAttribP1uiv                     = NULL;
PFNGLVERTEXATTRIBP2UIPROC                       glVertexAttribP2ui                      = NULL;
PFNGLVERTEXATTRIBP2UIVPROC                      glVertexAttribP2uiv                     = NULL;
PFNGLVERTEXATTRIBP3UIPROC                       glVertexAttribP3ui                      = NULL;
PFNGLVERTEXATTRIBP3UIVPROC                      glVertexAttribP3uiv                     = NULL;
PFNGLVERTEXATTRIBP4UIPROC                       glVertexAttribP4ui                      = NULL;
PFNGLVERTEXATTRIBP4UIVPROC                      glVertexAttribP4uiv                     = NULL;
PFNGLVERTEXP2UIPROC                             glVertexP2ui                            = NULL;
PFNGLVERTEXP2UIVPROC                            glVertexP2uiv                           = NULL;
PFNGLVERTEXP3UIPROC                             glVertexP3ui                            = NULL;
PFNGLVERTEXP3UIVPROC                            glVertexP3uiv                           = NULL;
PFNGLVERTEXP4UIPROC                             glVertexP4ui                            = NULL;
PFNGLVERTEXP4UIVPROC                            glVertexP4uiv                           = NULL;
PFNGLTEXCOORDP1UIPROC                           glTexCoordP1ui                          = NULL;
PFNGLTEXCOORDP1UIVPROC                          glTexCoordP1uiv                         = NULL;
PFNGLTEXCOORDP2UIPROC                           glTexCoordP2ui                          = NULL;
PFNGLTEXCOORDP2UIVPROC                          glTexCoordP2uiv                         = NULL;
PFNGLTEXCOORDP3UIPROC                           glTexCoordP3ui                          = NULL;
PFNGLTEXCOORDP3UIVPROC                          glTexCoordP3uiv                         = NULL;
PFNGLTEXCOORDP4UIPROC                           glTexCoordP4ui                          = NULL;
PFNGLTEXCOORDP4UIVPROC                          glTexCoordP4uiv                         = NULL;
PFNGLMULTITEXCOORDP1UIPROC                      glMultiTexCoordP1ui                     = NULL;
PFNGLMULTITEXCOORDP1UIVPROC                     glMultiTexCoordP1uiv                    = NULL;
PFNGLMULTITEXCOORDP2UIPROC                      glMultiTexCoordP2ui                     = NULL;
PFNGLMULTITEXCOORDP2UIVPROC                     glMultiTexCoordP2uiv                    = NULL;
PFNGLMULTITEXCOORDP3UIPROC                      glMultiTexCoordP3ui                     = NULL;
PFNGLMULTITEXCOORDP3UIVPROC                     glMultiTexCoordP3uiv                    = NULL;
PFNGLMULTITEXCOORDP4UIPROC                      glMultiTexCoordP4ui                     = NULL;
PFNGLMULTITEXCOORDP4UIVPROC                     glMultiTexCoordP4uiv                    = NULL;
PFNGLNORMALP3UIPROC                             glNormalP3ui                            = NULL;
PFNGLNORMALP3UIVPROC                            glNormalP3uiv                           = NULL;
PFNGLCOLORP3UIPROC                              glColorP3ui                             = NULL;
PFNGLCOLORP3UIVPROC                             glColorP3uiv                            = NULL;
PFNGLCOLORP4UIPROC                              glColorP4ui                             = NULL;
PFNGLCOLORP4UIVPROC                             glColorP4uiv                            = NULL;
PFNGLSECONDARYCOLORP3UIPROC                     glSecondaryColorP3ui                    = NULL;
PFNGLSECONDARYCOLORP3UIVPROC                    glSecondaryColorP3uiv                   = NULL;
