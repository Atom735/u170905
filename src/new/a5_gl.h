#ifndef _H_A5_GL
#define _H_A5_GL 171007

#include "a5_types.h"
#include "a5_ft.h"

#include <GL/gl.h>
#include <GL/glext.h>

A5TYPEDEF(struct A5sv_2DTXT,     sv_2DTXT);
A5TYPEDEF(struct A5sGL_2DTXT,    sGL_2DTXT);

A5TYPEDEF(struct A5sSETTINGS_TXT,   sSETTINGS_TXT);

A5uint A5GL_Init();

GLuint A5GL_CreateShader(A5pcstr szFileName, GLenum eType);
GLuint A5GL_CreateProgram(GLuint uiVS, GLuint uiFS);
GLuint A5GL_CreateProgramV(GLuint *uiShaders);

struct A5sv_2DTXT {
    GLshort x, y, u, v;
    union {
        GLuint c;
        struct {
            GLubyte r, g, b, a;
        };
    };
};

struct A5sGL_2DTXT {
    GLuint  p, v, f;
    GLint   _aPos, _aTex, _aCol,
            _uPos, _uMat, _uTex, _uTsz;
};

struct A5sSETTINGS_TXT {
    A5pcstr         pzTxt;
    A5psFT_Cache    pCache;
    A5uint          uSizeID;
    A5short         iHeight,iKerning;
    A5short         iPosX, iPosY;
    union {
        GLuint c;
        struct {
            GLubyte r, g, b, a;
        };
    };

};

GLvoid A5GL_2DTXT_Init(A5psGL_2DTXT p);
GLvoid A5GL_2DTXT_Free(A5psGL_2DTXT p);
GLuint A5GL_2DTXT_CreateVBO_Dynamic(GLuint uSize);
GLuint A5GL_2DTXT_CreateVBO_Static(GLuint uSize, A5psv_2DTXT pBuf);
GLvoid A5GL_2DTXT_DrawBegin(A5psGL_2DTXT p, GLuint uScreenWidth, GLuint uScreenHeight);
GLvoid A5GL_2DTXT_DrawSetTexture(A5psGL_2DTXT p, GLuint uTex, GLuint uTexSize);
GLvoid A5GL_2DTXT_DrawSetPos(A5psGL_2DTXT p, GLint iPosX, GLint iPosY);
GLvoid A5GL_2DTXT_DrawVBO(A5psGL_2DTXT p, GLuint uVBO, GLuint uOffset, GLuint uCount);

GLuint A5GL_2DTXT_PrepareVboText(GLuint uVBO, GLuint uOffset, GLuint uCount, A5psSETTINGS_TXT pSettings);














/* GL_VERSION_1_2 */
extern PFNGLDRAWRANGEELEMENTSPROC                      glDrawRangeElements;
extern PFNGLTEXIMAGE3DPROC                             glTexImage3D;
extern PFNGLTEXSUBIMAGE3DPROC                          glTexSubImage3D;
extern PFNGLCOPYTEXSUBIMAGE3DPROC                      glCopyTexSubImage3D;
/* GL_VERSION_1_3 */
extern PFNGLACTIVETEXTUREPROC                          glActiveTexture;
extern PFNGLSAMPLECOVERAGEPROC                         glSampleCoverage;
extern PFNGLCOMPRESSEDTEXIMAGE3DPROC                   glCompressedTexImage3D;
extern PFNGLCOMPRESSEDTEXIMAGE2DPROC                   glCompressedTexImage2D;
extern PFNGLCOMPRESSEDTEXIMAGE1DPROC                   glCompressedTexImage1D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC                glCompressedTexSubImage3D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC                glCompressedTexSubImage2D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC                glCompressedTexSubImage1D;
extern PFNGLGETCOMPRESSEDTEXIMAGEPROC                  glGetCompressedTexImage;
extern PFNGLCLIENTACTIVETEXTUREPROC                    glClientActiveTexture;
extern PFNGLMULTITEXCOORD1DPROC                        glMultiTexCoord1d;
extern PFNGLMULTITEXCOORD1DVPROC                       glMultiTexCoord1dv;
extern PFNGLMULTITEXCOORD1FPROC                        glMultiTexCoord1f;
extern PFNGLMULTITEXCOORD1FVPROC                       glMultiTexCoord1fv;
extern PFNGLMULTITEXCOORD1IPROC                        glMultiTexCoord1i;
extern PFNGLMULTITEXCOORD1IVPROC                       glMultiTexCoord1iv;
extern PFNGLMULTITEXCOORD1SPROC                        glMultiTexCoord1s;
extern PFNGLMULTITEXCOORD1SVPROC                       glMultiTexCoord1sv;
extern PFNGLMULTITEXCOORD2DPROC                        glMultiTexCoord2d;
extern PFNGLMULTITEXCOORD2DVPROC                       glMultiTexCoord2dv;
extern PFNGLMULTITEXCOORD2FPROC                        glMultiTexCoord2f;
extern PFNGLMULTITEXCOORD2FVPROC                       glMultiTexCoord2fv;
extern PFNGLMULTITEXCOORD2IPROC                        glMultiTexCoord2i;
extern PFNGLMULTITEXCOORD2IVPROC                       glMultiTexCoord2iv;
extern PFNGLMULTITEXCOORD2SPROC                        glMultiTexCoord2s;
extern PFNGLMULTITEXCOORD2SVPROC                       glMultiTexCoord2sv;
extern PFNGLMULTITEXCOORD3DPROC                        glMultiTexCoord3d;
extern PFNGLMULTITEXCOORD3DVPROC                       glMultiTexCoord3dv;
extern PFNGLMULTITEXCOORD3FPROC                        glMultiTexCoord3f;
extern PFNGLMULTITEXCOORD3FVPROC                       glMultiTexCoord3fv;
extern PFNGLMULTITEXCOORD3IPROC                        glMultiTexCoord3i;
extern PFNGLMULTITEXCOORD3IVPROC                       glMultiTexCoord3iv;
extern PFNGLMULTITEXCOORD3SPROC                        glMultiTexCoord3s;
extern PFNGLMULTITEXCOORD3SVPROC                       glMultiTexCoord3sv;
extern PFNGLMULTITEXCOORD4DPROC                        glMultiTexCoord4d;
extern PFNGLMULTITEXCOORD4DVPROC                       glMultiTexCoord4dv;
extern PFNGLMULTITEXCOORD4FPROC                        glMultiTexCoord4f;
extern PFNGLMULTITEXCOORD4FVPROC                       glMultiTexCoord4fv;
extern PFNGLMULTITEXCOORD4IPROC                        glMultiTexCoord4i;
extern PFNGLMULTITEXCOORD4IVPROC                       glMultiTexCoord4iv;
extern PFNGLMULTITEXCOORD4SPROC                        glMultiTexCoord4s;
extern PFNGLMULTITEXCOORD4SVPROC                       glMultiTexCoord4sv;
extern PFNGLLOADTRANSPOSEMATRIXFPROC                   glLoadTransposeMatrixf;
extern PFNGLLOADTRANSPOSEMATRIXDPROC                   glLoadTransposeMatrixd;
extern PFNGLMULTTRANSPOSEMATRIXFPROC                   glMultTransposeMatrixf;
extern PFNGLMULTTRANSPOSEMATRIXDPROC                   glMultTransposeMatrixd;
/* GL_VERSION_1_4 */
extern PFNGLBLENDFUNCSEPARATEPROC                      glBlendFuncSeparate;
extern PFNGLMULTIDRAWARRAYSPROC                        glMultiDrawArrays;
extern PFNGLMULTIDRAWELEMENTSPROC                      glMultiDrawElements;
extern PFNGLPOINTPARAMETERFPROC                        glPointParameterf;
extern PFNGLPOINTPARAMETERFVPROC                       glPointParameterfv;
extern PFNGLPOINTPARAMETERIPROC                        glPointParameteri;
extern PFNGLPOINTPARAMETERIVPROC                       glPointParameteriv;
extern PFNGLFOGCOORDFPROC                              glFogCoordf;
extern PFNGLFOGCOORDFVPROC                             glFogCoordfv;
extern PFNGLFOGCOORDDPROC                              glFogCoordd;
extern PFNGLFOGCOORDDVPROC                             glFogCoorddv;
extern PFNGLFOGCOORDPOINTERPROC                        glFogCoordPointer;
extern PFNGLSECONDARYCOLOR3BPROC                       glSecondaryColor3b;
extern PFNGLSECONDARYCOLOR3BVPROC                      glSecondaryColor3bv;
extern PFNGLSECONDARYCOLOR3DPROC                       glSecondaryColor3d;
extern PFNGLSECONDARYCOLOR3DVPROC                      glSecondaryColor3dv;
extern PFNGLSECONDARYCOLOR3FPROC                       glSecondaryColor3f;
extern PFNGLSECONDARYCOLOR3FVPROC                      glSecondaryColor3fv;
extern PFNGLSECONDARYCOLOR3IPROC                       glSecondaryColor3i;
extern PFNGLSECONDARYCOLOR3IVPROC                      glSecondaryColor3iv;
extern PFNGLSECONDARYCOLOR3SPROC                       glSecondaryColor3s;
extern PFNGLSECONDARYCOLOR3SVPROC                      glSecondaryColor3sv;
extern PFNGLSECONDARYCOLOR3UBPROC                      glSecondaryColor3ub;
extern PFNGLSECONDARYCOLOR3UBVPROC                     glSecondaryColor3ubv;
extern PFNGLSECONDARYCOLOR3UIPROC                      glSecondaryColor3ui;
extern PFNGLSECONDARYCOLOR3UIVPROC                     glSecondaryColor3uiv;
extern PFNGLSECONDARYCOLOR3USPROC                      glSecondaryColor3us;
extern PFNGLSECONDARYCOLOR3USVPROC                     glSecondaryColor3usv;
extern PFNGLSECONDARYCOLORPOINTERPROC                  glSecondaryColorPointer;
extern PFNGLWINDOWPOS2DPROC                            glWindowPos2d;
extern PFNGLWINDOWPOS2DVPROC                           glWindowPos2dv;
extern PFNGLWINDOWPOS2FPROC                            glWindowPos2f;
extern PFNGLWINDOWPOS2FVPROC                           glWindowPos2fv;
extern PFNGLWINDOWPOS2IPROC                            glWindowPos2i;
extern PFNGLWINDOWPOS2IVPROC                           glWindowPos2iv;
extern PFNGLWINDOWPOS2SPROC                            glWindowPos2s;
extern PFNGLWINDOWPOS2SVPROC                           glWindowPos2sv;
extern PFNGLWINDOWPOS3DPROC                            glWindowPos3d;
extern PFNGLWINDOWPOS3DVPROC                           glWindowPos3dv;
extern PFNGLWINDOWPOS3FPROC                            glWindowPos3f;
extern PFNGLWINDOWPOS3FVPROC                           glWindowPos3fv;
extern PFNGLWINDOWPOS3IPROC                            glWindowPos3i;
extern PFNGLWINDOWPOS3IVPROC                           glWindowPos3iv;
extern PFNGLWINDOWPOS3SPROC                            glWindowPos3s;
extern PFNGLWINDOWPOS3SVPROC                           glWindowPos3sv;
extern PFNGLBLENDCOLORPROC                             glBlendColor;
extern PFNGLBLENDEQUATIONPROC                          glBlendEquation;
/* GL_VERSION_1_5 */
extern PFNGLGENQUERIESPROC                             glGenQueries;
extern PFNGLDELETEQUERIESPROC                          glDeleteQueries;
extern PFNGLISQUERYPROC                                glIsQuery;
extern PFNGLBEGINQUERYPROC                             glBeginQuery;
extern PFNGLENDQUERYPROC                               glEndQuery;
extern PFNGLGETQUERYIVPROC                             glGetQueryiv;
extern PFNGLGETQUERYOBJECTIVPROC                       glGetQueryObjectiv;
extern PFNGLGETQUERYOBJECTUIVPROC                      glGetQueryObjectuiv;
extern PFNGLBINDBUFFERPROC                             glBindBuffer;
extern PFNGLDELETEBUFFERSPROC                          glDeleteBuffers;
extern PFNGLGENBUFFERSPROC                             glGenBuffers;
extern PFNGLISBUFFERPROC                               glIsBuffer;
extern PFNGLBUFFERDATAPROC                             glBufferData;
extern PFNGLBUFFERSUBDATAPROC                          glBufferSubData;
extern PFNGLGETBUFFERSUBDATAPROC                       glGetBufferSubData;
extern PFNGLMAPBUFFERPROC                              glMapBuffer;
extern PFNGLUNMAPBUFFERPROC                            glUnmapBuffer;
extern PFNGLGETBUFFERPARAMETERIVPROC                   glGetBufferParameteriv;
extern PFNGLGETBUFFERPOINTERVPROC                      glGetBufferPointerv;
/* GL_VERSION_2_0 */
extern PFNGLBLENDEQUATIONSEPARATEPROC                  glBlendEquationSeparate;
extern PFNGLDRAWBUFFERSPROC                            glDrawBuffers;
extern PFNGLSTENCILOPSEPARATEPROC                      glStencilOpSeparate;
extern PFNGLSTENCILFUNCSEPARATEPROC                    glStencilFuncSeparate;
extern PFNGLSTENCILMASKSEPARATEPROC                    glStencilMaskSeparate;
extern PFNGLATTACHSHADERPROC                           glAttachShader;
extern PFNGLBINDATTRIBLOCATIONPROC                     glBindAttribLocation;
extern PFNGLCOMPILESHADERPROC                          glCompileShader;
extern PFNGLCREATEPROGRAMPROC                          glCreateProgram;
extern PFNGLCREATESHADERPROC                           glCreateShader;
extern PFNGLDELETEPROGRAMPROC                          glDeleteProgram;
extern PFNGLDELETESHADERPROC                           glDeleteShader;
extern PFNGLDETACHSHADERPROC                           glDetachShader;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC               glDisableVertexAttribArray;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC                glEnableVertexAttribArray;
extern PFNGLGETACTIVEATTRIBPROC                        glGetActiveAttrib;
extern PFNGLGETACTIVEUNIFORMPROC                       glGetActiveUniform;
extern PFNGLGETATTACHEDSHADERSPROC                     glGetAttachedShaders;
extern PFNGLGETATTRIBLOCATIONPROC                      glGetAttribLocation;
extern PFNGLGETPROGRAMIVPROC                           glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC                      glGetProgramInfoLog;
extern PFNGLGETSHADERIVPROC                            glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC                       glGetShaderInfoLog;
extern PFNGLGETSHADERSOURCEPROC                        glGetShaderSource;
extern PFNGLGETUNIFORMLOCATIONPROC                     glGetUniformLocation;
extern PFNGLGETUNIFORMFVPROC                           glGetUniformfv;
extern PFNGLGETUNIFORMIVPROC                           glGetUniformiv;
extern PFNGLGETVERTEXATTRIBDVPROC                      glGetVertexAttribdv;
extern PFNGLGETVERTEXATTRIBFVPROC                      glGetVertexAttribfv;
extern PFNGLGETVERTEXATTRIBIVPROC                      glGetVertexAttribiv;
extern PFNGLGETVERTEXATTRIBPOINTERVPROC                glGetVertexAttribPointerv;
extern PFNGLISPROGRAMPROC                              glIsProgram;
extern PFNGLISSHADERPROC                               glIsShader;
extern PFNGLLINKPROGRAMPROC                            glLinkProgram;
extern PFNGLSHADERSOURCEPROC                           glShaderSource;
extern PFNGLUSEPROGRAMPROC                             glUseProgram;
extern PFNGLUNIFORM1FPROC                              glUniform1f;
extern PFNGLUNIFORM2FPROC                              glUniform2f;
extern PFNGLUNIFORM3FPROC                              glUniform3f;
extern PFNGLUNIFORM4FPROC                              glUniform4f;
extern PFNGLUNIFORM1IPROC                              glUniform1i;
extern PFNGLUNIFORM2IPROC                              glUniform2i;
extern PFNGLUNIFORM3IPROC                              glUniform3i;
extern PFNGLUNIFORM4IPROC                              glUniform4i;
extern PFNGLUNIFORM1FVPROC                             glUniform1fv;
extern PFNGLUNIFORM2FVPROC                             glUniform2fv;
extern PFNGLUNIFORM3FVPROC                             glUniform3fv;
extern PFNGLUNIFORM4FVPROC                             glUniform4fv;
extern PFNGLUNIFORM1IVPROC                             glUniform1iv;
extern PFNGLUNIFORM2IVPROC                             glUniform2iv;
extern PFNGLUNIFORM3IVPROC                             glUniform3iv;
extern PFNGLUNIFORM4IVPROC                             glUniform4iv;
extern PFNGLUNIFORMMATRIX2FVPROC                       glUniformMatrix2fv;
extern PFNGLUNIFORMMATRIX3FVPROC                       glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC                       glUniformMatrix4fv;
extern PFNGLVALIDATEPROGRAMPROC                        glValidateProgram;
extern PFNGLVERTEXATTRIB1DPROC                         glVertexAttrib1d;
extern PFNGLVERTEXATTRIB1DVPROC                        glVertexAttrib1dv;
extern PFNGLVERTEXATTRIB1FPROC                         glVertexAttrib1f;
extern PFNGLVERTEXATTRIB1FVPROC                        glVertexAttrib1fv;
extern PFNGLVERTEXATTRIB1SPROC                         glVertexAttrib1s;
extern PFNGLVERTEXATTRIB1SVPROC                        glVertexAttrib1sv;
extern PFNGLVERTEXATTRIB2DPROC                         glVertexAttrib2d;
extern PFNGLVERTEXATTRIB2DVPROC                        glVertexAttrib2dv;
extern PFNGLVERTEXATTRIB2FPROC                         glVertexAttrib2f;
extern PFNGLVERTEXATTRIB2FVPROC                        glVertexAttrib2fv;
extern PFNGLVERTEXATTRIB2SPROC                         glVertexAttrib2s;
extern PFNGLVERTEXATTRIB2SVPROC                        glVertexAttrib2sv;
extern PFNGLVERTEXATTRIB3DPROC                         glVertexAttrib3d;
extern PFNGLVERTEXATTRIB3DVPROC                        glVertexAttrib3dv;
extern PFNGLVERTEXATTRIB3FPROC                         glVertexAttrib3f;
extern PFNGLVERTEXATTRIB3FVPROC                        glVertexAttrib3fv;
extern PFNGLVERTEXATTRIB3SPROC                         glVertexAttrib3s;
extern PFNGLVERTEXATTRIB3SVPROC                        glVertexAttrib3sv;
extern PFNGLVERTEXATTRIB4NBVPROC                       glVertexAttrib4Nbv;
extern PFNGLVERTEXATTRIB4NIVPROC                       glVertexAttrib4Niv;
extern PFNGLVERTEXATTRIB4NSVPROC                       glVertexAttrib4Nsv;
extern PFNGLVERTEXATTRIB4NUBPROC                       glVertexAttrib4Nub;
extern PFNGLVERTEXATTRIB4NUBVPROC                      glVertexAttrib4Nubv;
extern PFNGLVERTEXATTRIB4NUIVPROC                      glVertexAttrib4Nuiv;
extern PFNGLVERTEXATTRIB4NUSVPROC                      glVertexAttrib4Nusv;
extern PFNGLVERTEXATTRIB4BVPROC                        glVertexAttrib4bv;
extern PFNGLVERTEXATTRIB4DPROC                         glVertexAttrib4d;
extern PFNGLVERTEXATTRIB4DVPROC                        glVertexAttrib4dv;
extern PFNGLVERTEXATTRIB4FPROC                         glVertexAttrib4f;
extern PFNGLVERTEXATTRIB4FVPROC                        glVertexAttrib4fv;
extern PFNGLVERTEXATTRIB4IVPROC                        glVertexAttrib4iv;
extern PFNGLVERTEXATTRIB4SPROC                         glVertexAttrib4s;
extern PFNGLVERTEXATTRIB4SVPROC                        glVertexAttrib4sv;
extern PFNGLVERTEXATTRIB4UBVPROC                       glVertexAttrib4ubv;
extern PFNGLVERTEXATTRIB4UIVPROC                       glVertexAttrib4uiv;
extern PFNGLVERTEXATTRIB4USVPROC                       glVertexAttrib4usv;
extern PFNGLVERTEXATTRIBPOINTERPROC                    glVertexAttribPointer;
/* GL_VERSION_2_1 */
extern PFNGLUNIFORMMATRIX2X3FVPROC                     glUniformMatrix2x3fv;
extern PFNGLUNIFORMMATRIX3X2FVPROC                     glUniformMatrix3x2fv;
extern PFNGLUNIFORMMATRIX2X4FVPROC                     glUniformMatrix2x4fv;
extern PFNGLUNIFORMMATRIX4X2FVPROC                     glUniformMatrix4x2fv;
extern PFNGLUNIFORMMATRIX3X4FVPROC                     glUniformMatrix3x4fv;
extern PFNGLUNIFORMMATRIX4X3FVPROC                     glUniformMatrix4x3fv;
/* GL_VERSION_3_0 */
extern PFNGLCOLORMASKIPROC                             glColorMaski;
extern PFNGLGETBOOLEANI_VPROC                          glGetBooleani_v;
extern PFNGLGETINTEGERI_VPROC                          glGetIntegeri_v;
extern PFNGLENABLEIPROC                                glEnablei;
extern PFNGLDISABLEIPROC                               glDisablei;
extern PFNGLISENABLEDIPROC                             glIsEnabledi;
extern PFNGLBEGINTRANSFORMFEEDBACKPROC                 glBeginTransformFeedback;
extern PFNGLENDTRANSFORMFEEDBACKPROC                   glEndTransformFeedback;
extern PFNGLBINDBUFFERRANGEPROC                        glBindBufferRange;
extern PFNGLBINDBUFFERBASEPROC                         glBindBufferBase;
extern PFNGLTRANSFORMFEEDBACKVARYINGSPROC              glTransformFeedbackVaryings;
extern PFNGLGETTRANSFORMFEEDBACKVARYINGPROC            glGetTransformFeedbackVarying;
extern PFNGLCLAMPCOLORPROC                             glClampColor;
extern PFNGLBEGINCONDITIONALRENDERPROC                 glBeginConditionalRender;
extern PFNGLENDCONDITIONALRENDERPROC                   glEndConditionalRender;
extern PFNGLVERTEXATTRIBIPOINTERPROC                   glVertexAttribIPointer;
extern PFNGLGETVERTEXATTRIBIIVPROC                     glGetVertexAttribIiv;
extern PFNGLGETVERTEXATTRIBIUIVPROC                    glGetVertexAttribIuiv;
extern PFNGLVERTEXATTRIBI1IPROC                        glVertexAttribI1i;
extern PFNGLVERTEXATTRIBI2IPROC                        glVertexAttribI2i;
extern PFNGLVERTEXATTRIBI3IPROC                        glVertexAttribI3i;
extern PFNGLVERTEXATTRIBI4IPROC                        glVertexAttribI4i;
extern PFNGLVERTEXATTRIBI1UIPROC                       glVertexAttribI1ui;
extern PFNGLVERTEXATTRIBI2UIPROC                       glVertexAttribI2ui;
extern PFNGLVERTEXATTRIBI3UIPROC                       glVertexAttribI3ui;
extern PFNGLVERTEXATTRIBI4UIPROC                       glVertexAttribI4ui;
extern PFNGLVERTEXATTRIBI1IVPROC                       glVertexAttribI1iv;
extern PFNGLVERTEXATTRIBI2IVPROC                       glVertexAttribI2iv;
extern PFNGLVERTEXATTRIBI3IVPROC                       glVertexAttribI3iv;
extern PFNGLVERTEXATTRIBI4IVPROC                       glVertexAttribI4iv;
extern PFNGLVERTEXATTRIBI1UIVPROC                      glVertexAttribI1uiv;
extern PFNGLVERTEXATTRIBI2UIVPROC                      glVertexAttribI2uiv;
extern PFNGLVERTEXATTRIBI3UIVPROC                      glVertexAttribI3uiv;
extern PFNGLVERTEXATTRIBI4UIVPROC                      glVertexAttribI4uiv;
extern PFNGLVERTEXATTRIBI4BVPROC                       glVertexAttribI4bv;
extern PFNGLVERTEXATTRIBI4SVPROC                       glVertexAttribI4sv;
extern PFNGLVERTEXATTRIBI4UBVPROC                      glVertexAttribI4ubv;
extern PFNGLVERTEXATTRIBI4USVPROC                      glVertexAttribI4usv;
extern PFNGLGETUNIFORMUIVPROC                          glGetUniformuiv;
extern PFNGLBINDFRAGDATALOCATIONPROC                   glBindFragDataLocation;
extern PFNGLGETFRAGDATALOCATIONPROC                    glGetFragDataLocation;
extern PFNGLUNIFORM1UIPROC                             glUniform1ui;
extern PFNGLUNIFORM2UIPROC                             glUniform2ui;
extern PFNGLUNIFORM3UIPROC                             glUniform3ui;
extern PFNGLUNIFORM4UIPROC                             glUniform4ui;
extern PFNGLUNIFORM1UIVPROC                            glUniform1uiv;
extern PFNGLUNIFORM2UIVPROC                            glUniform2uiv;
extern PFNGLUNIFORM3UIVPROC                            glUniform3uiv;
extern PFNGLUNIFORM4UIVPROC                            glUniform4uiv;
extern PFNGLTEXPARAMETERIIVPROC                        glTexParameterIiv;
extern PFNGLTEXPARAMETERIUIVPROC                       glTexParameterIuiv;
extern PFNGLGETTEXPARAMETERIIVPROC                     glGetTexParameterIiv;
extern PFNGLGETTEXPARAMETERIUIVPROC                    glGetTexParameterIuiv;
extern PFNGLCLEARBUFFERIVPROC                          glClearBufferiv;
extern PFNGLCLEARBUFFERUIVPROC                         glClearBufferuiv;
extern PFNGLCLEARBUFFERFVPROC                          glClearBufferfv;
extern PFNGLCLEARBUFFERFIPROC                          glClearBufferfi;
extern PFNGLGETSTRINGIPROC                             glGetStringi;
extern PFNGLISRENDERBUFFERPROC                         glIsRenderbuffer;
extern PFNGLBINDRENDERBUFFERPROC                       glBindRenderbuffer;
extern PFNGLDELETERENDERBUFFERSPROC                    glDeleteRenderbuffers;
extern PFNGLGENRENDERBUFFERSPROC                       glGenRenderbuffers;
extern PFNGLRENDERBUFFERSTORAGEPROC                    glRenderbufferStorage;
extern PFNGLGETRENDERBUFFERPARAMETERIVPROC             glGetRenderbufferParameteriv;
extern PFNGLISFRAMEBUFFERPROC                          glIsFramebuffer;
extern PFNGLBINDFRAMEBUFFERPROC                        glBindFramebuffer;
extern PFNGLDELETEFRAMEBUFFERSPROC                     glDeleteFramebuffers;
extern PFNGLGENFRAMEBUFFERSPROC                        glGenFramebuffers;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC                 glCheckFramebufferStatus;
extern PFNGLFRAMEBUFFERTEXTURE1DPROC                   glFramebufferTexture1D;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC                   glFramebufferTexture2D;
extern PFNGLFRAMEBUFFERTEXTURE3DPROC                   glFramebufferTexture3D;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC                glFramebufferRenderbuffer;
extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC    glGetFramebufferAttachmentParameteriv;
extern PFNGLGENERATEMIPMAPPROC                         glGenerateMipmap;
extern PFNGLBLITFRAMEBUFFERPROC                        glBlitFramebuffer;
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC         glRenderbufferStorageMultisample;
extern PFNGLFRAMEBUFFERTEXTURELAYERPROC                glFramebufferTextureLayer;
extern PFNGLMAPBUFFERRANGEPROC                         glMapBufferRange;
extern PFNGLFLUSHMAPPEDBUFFERRANGEPROC                 glFlushMappedBufferRange;
extern PFNGLBINDVERTEXARRAYPROC                        glBindVertexArray;
extern PFNGLDELETEVERTEXARRAYSPROC                     glDeleteVertexArrays;
extern PFNGLGENVERTEXARRAYSPROC                        glGenVertexArrays;
extern PFNGLISVERTEXARRAYPROC                          glIsVertexArray;
/* GL_VERSION_3_1 */
extern PFNGLDRAWARRAYSINSTANCEDPROC                    glDrawArraysInstanced;
extern PFNGLDRAWELEMENTSINSTANCEDPROC                  glDrawElementsInstanced;
extern PFNGLTEXBUFFERPROC                              glTexBuffer;
extern PFNGLPRIMITIVERESTARTINDEXPROC                  glPrimitiveRestartIndex;
extern PFNGLCOPYBUFFERSUBDATAPROC                      glCopyBufferSubData;
extern PFNGLGETUNIFORMINDICESPROC                      glGetUniformIndices;
extern PFNGLGETACTIVEUNIFORMSIVPROC                    glGetActiveUniformsiv;
extern PFNGLGETACTIVEUNIFORMNAMEPROC                   glGetActiveUniformName;
extern PFNGLGETUNIFORMBLOCKINDEXPROC                   glGetUniformBlockIndex;
extern PFNGLGETACTIVEUNIFORMBLOCKIVPROC                glGetActiveUniformBlockiv;
extern PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC              glGetActiveUniformBlockName;
extern PFNGLUNIFORMBLOCKBINDINGPROC                    glUniformBlockBinding;
/* GL_VERSION_3_2 */
extern PFNGLDRAWELEMENTSBASEVERTEXPROC                 glDrawElementsBaseVertex;
extern PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC            glDrawRangeElementsBaseVertex;
extern PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC        glDrawElementsInstancedBaseVertex;
extern PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC            glMultiDrawElementsBaseVertex;
extern PFNGLPROVOKINGVERTEXPROC                        glProvokingVertex;
extern PFNGLFENCESYNCPROC                              glFenceSync;
extern PFNGLISSYNCPROC                                 glIsSync;
extern PFNGLDELETESYNCPROC                             glDeleteSync;
extern PFNGLCLIENTWAITSYNCPROC                         glClientWaitSync;
extern PFNGLWAITSYNCPROC                               glWaitSync;
extern PFNGLGETINTEGER64VPROC                          glGetInteger64v;
extern PFNGLGETSYNCIVPROC                              glGetSynciv;
extern PFNGLGETINTEGER64I_VPROC                        glGetInteger64i_v;
extern PFNGLGETBUFFERPARAMETERI64VPROC                 glGetBufferParameteri64v;
extern PFNGLFRAMEBUFFERTEXTUREPROC                     glFramebufferTexture;
extern PFNGLTEXIMAGE2DMULTISAMPLEPROC                  glTexImage2DMultisample;
extern PFNGLTEXIMAGE3DMULTISAMPLEPROC                  glTexImage3DMultisample;
extern PFNGLGETMULTISAMPLEFVPROC                       glGetMultisamplefv;
extern PFNGLSAMPLEMASKIPROC                            glSampleMaski;
/* GL_VERSION_3_3 */
extern PFNGLBINDFRAGDATALOCATIONINDEXEDPROC            glBindFragDataLocationIndexed;
extern PFNGLGETFRAGDATAINDEXPROC                       glGetFragDataIndex;
extern PFNGLGENSAMPLERSPROC                            glGenSamplers;
extern PFNGLDELETESAMPLERSPROC                         glDeleteSamplers;
extern PFNGLISSAMPLERPROC                              glIsSampler;
extern PFNGLBINDSAMPLERPROC                            glBindSampler;
extern PFNGLSAMPLERPARAMETERIPROC                      glSamplerParameteri;
extern PFNGLSAMPLERPARAMETERIVPROC                     glSamplerParameteriv;
extern PFNGLSAMPLERPARAMETERFPROC                      glSamplerParameterf;
extern PFNGLSAMPLERPARAMETERFVPROC                     glSamplerParameterfv;
extern PFNGLSAMPLERPARAMETERIIVPROC                    glSamplerParameterIiv;
extern PFNGLSAMPLERPARAMETERIUIVPROC                   glSamplerParameterIuiv;
extern PFNGLGETSAMPLERPARAMETERIVPROC                  glGetSamplerParameteriv;
extern PFNGLGETSAMPLERPARAMETERIIVPROC                 glGetSamplerParameterIiv;
extern PFNGLGETSAMPLERPARAMETERFVPROC                  glGetSamplerParameterfv;
extern PFNGLGETSAMPLERPARAMETERIUIVPROC                glGetSamplerParameterIuiv;
extern PFNGLQUERYCOUNTERPROC                           glQueryCounter;
extern PFNGLGETQUERYOBJECTI64VPROC                     glGetQueryObjecti64v;
extern PFNGLGETQUERYOBJECTUI64VPROC                    glGetQueryObjectui64v;
extern PFNGLVERTEXATTRIBDIVISORPROC                    glVertexAttribDivisor;
extern PFNGLVERTEXATTRIBP1UIPROC                       glVertexAttribP1ui;
extern PFNGLVERTEXATTRIBP1UIVPROC                      glVertexAttribP1uiv;
extern PFNGLVERTEXATTRIBP2UIPROC                       glVertexAttribP2ui;
extern PFNGLVERTEXATTRIBP2UIVPROC                      glVertexAttribP2uiv;
extern PFNGLVERTEXATTRIBP3UIPROC                       glVertexAttribP3ui;
extern PFNGLVERTEXATTRIBP3UIVPROC                      glVertexAttribP3uiv;
extern PFNGLVERTEXATTRIBP4UIPROC                       glVertexAttribP4ui;
extern PFNGLVERTEXATTRIBP4UIVPROC                      glVertexAttribP4uiv;
extern PFNGLVERTEXP2UIPROC                             glVertexP2ui;
extern PFNGLVERTEXP2UIVPROC                            glVertexP2uiv;
extern PFNGLVERTEXP3UIPROC                             glVertexP3ui;
extern PFNGLVERTEXP3UIVPROC                            glVertexP3uiv;
extern PFNGLVERTEXP4UIPROC                             glVertexP4ui;
extern PFNGLVERTEXP4UIVPROC                            glVertexP4uiv;
extern PFNGLTEXCOORDP1UIPROC                           glTexCoordP1ui;
extern PFNGLTEXCOORDP1UIVPROC                          glTexCoordP1uiv;
extern PFNGLTEXCOORDP2UIPROC                           glTexCoordP2ui;
extern PFNGLTEXCOORDP2UIVPROC                          glTexCoordP2uiv;
extern PFNGLTEXCOORDP3UIPROC                           glTexCoordP3ui;
extern PFNGLTEXCOORDP3UIVPROC                          glTexCoordP3uiv;
extern PFNGLTEXCOORDP4UIPROC                           glTexCoordP4ui;
extern PFNGLTEXCOORDP4UIVPROC                          glTexCoordP4uiv;
extern PFNGLMULTITEXCOORDP1UIPROC                      glMultiTexCoordP1ui;
extern PFNGLMULTITEXCOORDP1UIVPROC                     glMultiTexCoordP1uiv;
extern PFNGLMULTITEXCOORDP2UIPROC                      glMultiTexCoordP2ui;
extern PFNGLMULTITEXCOORDP2UIVPROC                     glMultiTexCoordP2uiv;
extern PFNGLMULTITEXCOORDP3UIPROC                      glMultiTexCoordP3ui;
extern PFNGLMULTITEXCOORDP3UIVPROC                     glMultiTexCoordP3uiv;
extern PFNGLMULTITEXCOORDP4UIPROC                      glMultiTexCoordP4ui;
extern PFNGLMULTITEXCOORDP4UIVPROC                     glMultiTexCoordP4uiv;
extern PFNGLNORMALP3UIPROC                             glNormalP3ui;
extern PFNGLNORMALP3UIVPROC                            glNormalP3uiv;
extern PFNGLCOLORP3UIPROC                              glColorP3ui;
extern PFNGLCOLORP3UIVPROC                             glColorP3uiv;
extern PFNGLCOLORP4UIPROC                              glColorP4ui;
extern PFNGLCOLORP4UIVPROC                             glColorP4uiv;
extern PFNGLSECONDARYCOLORP3UIPROC                     glSecondaryColorP3ui;
extern PFNGLSECONDARYCOLORP3UIVPROC                    glSecondaryColorP3uiv;


#endif /* _H_A5_GL */
