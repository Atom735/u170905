#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>

#include "ft.h"
#include "gl.h"
#include "utils.h"


// безопасный вызов функции OpenGL
#define OPENGL_CALL(exp) \
    { exp; GLenum glErr=glGetError(); if(glErr != GL_NO_ERROR) SDL_Log("expression \"" #exp "\" Error %d(0x%x)", glErr, glErr); }

typedef struct sA5App {
    SDL_Window         *wnd;
    SDL_GLContext       glctx;

    int                 bRun;
    int                 bAnimation;
    int                 iReturn;

    float               f_sW, f_sH, f_mX, f_mY;

    void               *ftLib;
    void               *ftCache;

    struct {
        struct {
            GLuint sp, vs, fs;
            GLint _aPos, _aCol,
                _uMat;
        } s2DC;
        struct {
            GLuint sp, vs, fs;
            GLint _aPos, _aTex,
                _uMat, _uTex;
        } s2DT;
        struct {
            GLuint sp, vs, fs;
            GLint _aPos, _aTex, _aCol,
                _uMat, _uTex;
        } s2DCT;
        struct {
            GLuint sp, vs, fs;
            GLint _aPos, _aTex, _aCol,
                _uPos, _uMat, _uTex, _uTsz;
        } s2DTXT;
        GLuint vbo_text;
        GLuint vbo_mouse;
        GLuint vbo_tt;
        GLuint tex_tt;
    } glfvf;
} A5App, *pA5App;

typedef struct sv2DC {
    GLfloat x, y;
    GLfloat r, g, b, a;
} sv2DC, *psv2DC;

typedef struct sv2DT {
    GLfloat x, y, u, v;
} sv2DT, *psv2DT;

typedef struct sv2DCT {
    GLfloat x, y, u, v;
    GLfloat r, g, b, a;
} sv2DCT, *psv2DCT;

typedef struct sv2DTXT {
    GLshort x, y, u, v;
    union {
        GLuint abgr;
        struct {
            GLubyte r, g, b, a;
        };
    };
} sv2DTXT, *psv2DTXT;

const GLchar
    sz_aPos[] = "v_pos",
    sz_aCol[] = "v_col",
    sz_aTex[] = "v_tex",
    sz_uPos[] = "u_pos",
    sz_uMat[] = "u_mat",
    sz_uTex[] = "u_tex",
    sz_uTsz[] = "u_tsz";

void event(pA5App pApp, SDL_Event *pEvent) {
    // SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "%i (%i.%is) %x(%s)\n", pEvent->common.timestamp, pEvent->common.timestamp/1000, pEvent->common.timestamp%1000, pEvent->type, info_get_sdl_event_type(pEvent->type));
    SDL_Log("%9i EVENT %s", pEvent->common.timestamp, info_get_sdl_event_type(pEvent->type));
    switch(pEvent->type) {
        case SDL_WINDOWEVENT: {
            info_sdl_event_window(&pEvent->window);
            switch(pEvent->window.event) {
                case SDL_WINDOWEVENT_RESIZED: {
                    glViewport(0, 0, pEvent->window.data1, pEvent->window.data2);
                    return;
                }
            }
            return;
        }
        case SDL_QUIT: {
            pApp->bRun = 0;
            return;
        }
    }
}

void drawTxt(pA5App pApp) {
    glUseProgram(pApp->glfvf.s2DTXT.sp);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pApp->glfvf.tex_tt);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glUniform2f(pApp->glfvf.s2DTXT._uPos, pApp->f_mX, pApp->f_mY);

    glBindBuffer(GL_ARRAY_BUFFER, pApp->glfvf.vbo_text);

    char *text = NULL;

    if(SDL_HasClipboardText()) {
        text = SDL_GetClipboardText();
    } else {
        text = SDL_malloc(256);
        text[0] = 'x';
        text[1] = 0;
    }

    int i = 0;
    int X = 0;
    int Y = 0;

    psv2DTXT pv = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);


    char *pT = text;
    while(*pT) {
        if(*pT == '\n') {
            X = 0;
            Y += 32;
            ++pT;
            continue;
        }
        Uint32 iChar = *pT;
        if((*pT & 0x80) && (*pT & 0x40)) {
            if(*pT & 0x20) {
                iChar = ((pT[2]&0x3f)) + ((pT[1]&0x3f)<<6) + ((pT[0]&0x0f)<<12);
                ++pT;
            } else
                iChar = ((pT[1]&0x3f)) + ((pT[0]&0x1f)<<6);
            ++pT;
        }


        psA5Glyph pG = a5ft_get_char(pApp->ftCache, 0, iChar, 32);
        if(!pG) pG = a5ft_get_char(pApp->ftCache, 0, '?', 32);
        if(pG) {
            pv[i*6 + 5].abgr = pv[i*6 + 4].abgr = pv[i*6 + 3].abgr = pv[i*6 + 2].abgr = pv[i*6 + 1].abgr = pv[i*6 + 0].abgr = 0xffffffff;

            pv[i*6+0].x = X+pG->X;
            pv[i*6+0].y = Y+pG->Y;
            pv[i*6+0].u = pG->x;
            pv[i*6+0].v = pG->y;

            pv[i*6+1].x = X+pG->X+pG->w;
            pv[i*6+1].y = Y+pG->Y;
            pv[i*6+1].u = pG->x+pG->w;
            pv[i*6+1].v = pG->y;

            pv[i*6+2].x = X+pG->X;
            pv[i*6+2].y = Y+pG->Y+pG->h;
            pv[i*6+2].u = pG->x;
            pv[i*6+2].v = pG->y+pG->h;

            pv[i*6+3] = pv[i*6+2];
            pv[i*6+4] = pv[i*6+1];

            pv[i*6+5].x = X+pG->X+pG->w;
            pv[i*6+5].y = Y+pG->Y+pG->h;
            pv[i*6+5].u = pG->x+pG->w;
            pv[i*6+5].v = pG->y+pG->h;

            X += pG->A;
            ++i;
        }
        ++pT;
    }
    SDL_free(text);

    glUnmapBuffer(GL_ARRAY_BUFFER);

    glEnableVertexAttribArray(pApp->glfvf.s2DTXT._aPos);
    glEnableVertexAttribArray(pApp->glfvf.s2DTXT._aTex);
    glEnableVertexAttribArray(pApp->glfvf.s2DTXT._aCol);
    glVertexAttribPointer(pApp->glfvf.s2DTXT._aPos, 2, GL_SHORT, GL_FALSE, sizeof(sv2DTXT), &((psv2DTXT)0)->x);
    glVertexAttribPointer(pApp->glfvf.s2DTXT._aTex, 2, GL_SHORT, GL_FALSE, sizeof(sv2DTXT), &((psv2DTXT)0)->u);
    glVertexAttribPointer(pApp->glfvf.s2DTXT._aCol, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(sv2DTXT), &((psv2DTXT)0)->r);
    glDrawArrays(GL_TRIANGLES, 0, 6*i);
    glDisableVertexAttribArray(pApp->glfvf.s2DTXT._aPos);
    glDisableVertexAttribArray(pApp->glfvf.s2DTXT._aTex);
    glDisableVertexAttribArray(pApp->glfvf.s2DTXT._aCol);
}

void animation(pA5App pApp) {
    if(a5ft_cache_update(pApp->ftCache, 0)) {
        glBindTexture(GL_TEXTURE_2D, pApp->glfvf.tex_tt);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, pApp->ftCache);
    }

    static GLfloat r = 0.f;
    static GLfloat g = 0.f;
    static GLfloat b = 0.f;
    glClearColor((r>1.f?2.f-r:r)*0.33f,(g>1.f?2.f-g:g)*0.33f,(b>1.f?2.f-b:b)*0.33f,1);
    r += 0.003f;
    g += 0.0039f;
    b += 0.0047f;
    if(r>2.f) r=0.f;
    if(g>2.f) g=0.f;
    if(b>2.f) b=0.f;
    glClear(GL_COLOR_BUFFER_BIT);

    int ds_w, ds_h;
    SDL_GL_GetDrawableSize(pApp->wnd, &ds_w, &ds_h);
    float f_w = (float)ds_w;
    float f_h = (float)ds_h;

    int ms_x, ms_y;
    SDL_GetMouseState(&ms_x, &ms_y);
    ms_y = ds_h-ms_y;
    float f_x = (float)ms_x;
    float f_y = (float)ms_y;
    pApp->f_mX = f_x;
    pApp->f_mY = f_h-f_y-1.0f;
    pApp->f_sW = f_w;
    pApp->f_sH = f_h;
    {
        glUseProgram(pApp->glfvf.s2DC.sp);

        glUniform2f(pApp->glfvf.s2DC._uMat, 2.0f/f_w, 2.0f/f_h);

        glBindBuffer(GL_ARRAY_BUFFER, pApp->glfvf.vbo_mouse);
        psv2DC pv = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        for(int i=0; i<6; ++i) {
            pv[i].x = f_x; pv[i].y = f_y;
            pv[i].a = pv[i].b = pv[i].g = pv[i].r = 1.0f;
        }
        pv[1].x -= 8.0f; pv[1].y += 10.0f; pv[2].y += 13.0f;
        pv[4].x = f_x + 100.0f*SDL_cosf(f_x*0.01f);
        pv[4].y = f_y + 100.0f*SDL_sinf(f_x*0.01f);
        pv[5].x = f_x + 100.0f*SDL_cosf(f_y*0.01f);
        pv[5].y = f_y + 100.0f*SDL_sinf(f_y*0.01f);
        glUnmapBuffer(GL_ARRAY_BUFFER);

        glEnableVertexAttribArray(pApp->glfvf.s2DC._aPos);
        glEnableVertexAttribArray(pApp->glfvf.s2DC._aCol);
        glVertexAttribPointer(pApp->glfvf.s2DC._aPos, 2, GL_FLOAT, GL_FALSE, sizeof(sv2DC), &((psv2DC)0)->x);
        glVertexAttribPointer(pApp->glfvf.s2DC._aCol, 4, GL_FLOAT, GL_FALSE, sizeof(sv2DC), &((psv2DC)0)->r);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(pApp->glfvf.s2DC._aPos);
        glDisableVertexAttribArray(pApp->glfvf.s2DC._aCol);
    }
    {
        glUseProgram(pApp->glfvf.s2DTXT.sp);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pApp->glfvf.tex_tt);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glUniform1i(pApp->glfvf.s2DTXT._uTex, 0);

        glUniform2f(pApp->glfvf.s2DTXT._uMat, 2.0f/f_w, -2.0f/f_h);
        glUniform2f(pApp->glfvf.s2DTXT._uPos, 0.0f, 0.0f);
        glUniform2f(pApp->glfvf.s2DTXT._uTsz, 1.0f/512.0f, 1.0f/512.0f);

        glBindBuffer(GL_ARRAY_BUFFER, pApp->glfvf.vbo_tt);


        glEnableVertexAttribArray(pApp->glfvf.s2DTXT._aPos);
        glEnableVertexAttribArray(pApp->glfvf.s2DTXT._aTex);
        glEnableVertexAttribArray(pApp->glfvf.s2DTXT._aCol);
        glVertexAttribPointer(pApp->glfvf.s2DTXT._aPos, 2, GL_SHORT, GL_FALSE, sizeof(sv2DTXT), &((psv2DTXT)0)->x);
        glVertexAttribPointer(pApp->glfvf.s2DTXT._aTex, 2, GL_SHORT, GL_FALSE, sizeof(sv2DTXT), &((psv2DTXT)0)->u);
        glVertexAttribPointer(pApp->glfvf.s2DTXT._aCol, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(sv2DTXT), &((psv2DTXT)0)->r);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(pApp->glfvf.s2DTXT._aPos);
        glDisableVertexAttribArray(pApp->glfvf.s2DTXT._aTex);
        glDisableVertexAttribArray(pApp->glfvf.s2DTXT._aCol);
    }
    drawTxt(pApp);
    SDL_GL_SwapWindow(pApp->wnd);
}

int resources_init(pA5App pApp) {

    a5ft_init(&pApp->ftLib);
    a5ft_cache_init(pApp->ftLib, &pApp->ftCache, 512, 4);
    a5ft_new_font_from_file(pApp->ftCache, "data/fonts/RobotoSlab-Regular.ttf");
    for(int j = 4; j<16; ++j)
    for (int i = 0x20; i <= 0x7f; ++i){
        a5ft_new_char(pApp->ftCache, 0, i, j*4);
    }

    GLint link_ok = GL_FALSE;

    /* Init 2D Colored Shader */
    {
        if (!(pApp->glfvf.s2DC.vs = create_shader("data/shader/2DC.v.glsl", GL_VERTEX_SHADER))) {
            print_log(pApp->glfvf.s2DC.vs);
            return 0x0e01;
        }
        if (!(pApp->glfvf.s2DC.fs = create_shader("data/shader/2DC.f.glsl", GL_FRAGMENT_SHADER))) {
            print_log(pApp->glfvf.s2DC.fs);
            return 0x0e02;
        }
        pApp->glfvf.s2DC.sp = glCreateProgram();
        glAttachShader(pApp->glfvf.s2DC.sp, pApp->glfvf.s2DC.vs);
        glAttachShader(pApp->glfvf.s2DC.sp, pApp->glfvf.s2DC.fs);
        glLinkProgram (pApp->glfvf.s2DC.sp);
        glGetProgramiv(pApp->glfvf.s2DC.sp, GL_LINK_STATUS, &link_ok);
        if (!link_ok) {
            print_log(pApp->glfvf.s2DC.sp);
            return 0x0e0f;
        }
        if ((pApp->glfvf.s2DC._aPos = glGetAttribLocation(pApp->glfvf.s2DC.sp, sz_aPos)) == -1) {
            SDL_Log("Could not bind attribute \"%s\"\n", sz_aPos);
            return 0x0e08;
        }
        if ((pApp->glfvf.s2DC._aCol = glGetAttribLocation(pApp->glfvf.s2DC.sp, sz_aCol)) == -1) {
            SDL_Log("Could not bind attribute \"%s\"\n", sz_aCol);
            return 0x0e09;
        }
        if ((pApp->glfvf.s2DC._uMat = glGetUniformLocation(pApp->glfvf.s2DC.sp, sz_uMat)) == -1) {
            SDL_Log("Could not bind uniform   \"%s\"\n", sz_uMat);
            return 0x0e0a;
        }
        link_ok = GL_FALSE;
    }
    /* Init 2D Textured Shader */
    {

        if (!(pApp->glfvf.s2DT.vs = create_shader("data/shader/2DT.v.glsl", GL_VERTEX_SHADER))) {
            print_log(pApp->glfvf.s2DT.vs);
            return 0x0e11;
        }
        if (!(pApp->glfvf.s2DT.fs = create_shader("data/shader/2DT.f.glsl", GL_FRAGMENT_SHADER))) {
            print_log(pApp->glfvf.s2DT.fs);
            return 0x0e12;
        }
        pApp->glfvf.s2DT.sp = glCreateProgram();
        glAttachShader(pApp->glfvf.s2DT.sp, pApp->glfvf.s2DT.vs);
        glAttachShader(pApp->glfvf.s2DT.sp, pApp->glfvf.s2DT.fs);
        glLinkProgram (pApp->glfvf.s2DT.sp);
        glGetProgramiv(pApp->glfvf.s2DT.sp, GL_LINK_STATUS, &link_ok);
        if (!link_ok) {
            print_log(pApp->glfvf.s2DT.sp);
            return 0x0e1f;
        }
        if ((pApp->glfvf.s2DT._aPos = glGetAttribLocation(pApp->glfvf.s2DT.sp, sz_aPos)) == -1) {
            SDL_Log("Could not bind attribute \"%s\"\n", sz_aPos);
            return 0x0e18;
        }
        if ((pApp->glfvf.s2DT._aTex = glGetAttribLocation(pApp->glfvf.s2DT.sp, sz_aTex)) == -1) {
            SDL_Log("Could not bind attribute \"%s\"\n", sz_aTex);
            return 0x0e19;
        }
        if ((pApp->glfvf.s2DT._uMat = glGetUniformLocation(pApp->glfvf.s2DT.sp, sz_uMat)) == -1) {
            SDL_Log("Could not bind uniform   \"%s\"\n", sz_uMat);
            return 0x0e1a;
        }
        if ((pApp->glfvf.s2DT._uTex = glGetUniformLocation(pApp->glfvf.s2DT.sp, sz_uTex)) == -1) {
            SDL_Log("Could not bind uniform   \"%s\"\n", sz_uTex);
            return 0x0e1b;
        }
        link_ok = GL_FALSE;
    }
    /* Init 2D Colored Textured Shader */
    {
        if (!(pApp->glfvf.s2DCT.vs = create_shader("data/shader/2DCT.v.glsl", GL_VERTEX_SHADER))) {
            print_log(pApp->glfvf.s2DCT.vs);
            return 0x0e21;
        }
        if (!(pApp->glfvf.s2DCT.fs = create_shader("data/shader/2DCT.f.glsl", GL_FRAGMENT_SHADER))) {
            print_log(pApp->glfvf.s2DCT.fs);
            return 0x0e22;
        }
        pApp->glfvf.s2DCT.sp = glCreateProgram();
        glAttachShader(pApp->glfvf.s2DCT.sp, pApp->glfvf.s2DCT.vs);
        glAttachShader(pApp->glfvf.s2DCT.sp, pApp->glfvf.s2DCT.fs);
        glLinkProgram (pApp->glfvf.s2DCT.sp);
        glGetProgramiv(pApp->glfvf.s2DCT.sp, GL_LINK_STATUS, &link_ok);
        if (!link_ok) {
            print_log(pApp->glfvf.s2DCT.sp);
            return 0x0e2f;
        }
        if ((pApp->glfvf.s2DCT._aPos = glGetAttribLocation(pApp->glfvf.s2DCT.sp, sz_aPos)) == -1) {
            SDL_Log("Could not bind attribute \"%s\"\n", sz_aPos);
            return 0x0e28;
        }
        if ((pApp->glfvf.s2DCT._aTex = glGetAttribLocation(pApp->glfvf.s2DCT.sp, sz_aTex)) == -1) {
            SDL_Log("Could not bind attribute \"%s\"\n", sz_aTex);
            return 0x0e29;
        }
        if ((pApp->glfvf.s2DCT._aCol = glGetAttribLocation(pApp->glfvf.s2DCT.sp, sz_aCol)) == -1) {
            SDL_Log("Could not bind attribute \"%s\"\n", sz_aCol);
            return 0x0e2a;
        }
        if ((pApp->glfvf.s2DCT._uMat = glGetUniformLocation(pApp->glfvf.s2DCT.sp, sz_uMat)) == -1) {
            SDL_Log("Could not bind uniform   \"%s\"\n", sz_uMat);
            return 0x0e2b;
        }
        if ((pApp->glfvf.s2DCT._uTex = glGetUniformLocation(pApp->glfvf.s2DCT.sp, sz_uTex)) == -1) {
            SDL_Log("Could not bind uniform   \"%s\"\n", sz_uTex);
            return 0x0e2c;
        }
        link_ok = GL_FALSE;
    }
    /* Init 2D Colored Text Shader (With Alpha Texture)*/
    {
        if (!(pApp->glfvf.s2DTXT.vs = create_shader("data/shader/2DTXT.v.glsl", GL_VERTEX_SHADER))) {
            print_log(pApp->glfvf.s2DTXT.vs);
            return 0x0e31;
        }
        if (!(pApp->glfvf.s2DTXT.fs = create_shader("data/shader/2DTXT.f.glsl", GL_FRAGMENT_SHADER))) {
            print_log(pApp->glfvf.s2DTXT.fs);
            return 0x0e32;
        }
        pApp->glfvf.s2DTXT.sp = glCreateProgram();
        glAttachShader(pApp->glfvf.s2DTXT.sp, pApp->glfvf.s2DTXT.vs);
        glAttachShader(pApp->glfvf.s2DTXT.sp, pApp->glfvf.s2DTXT.fs);
        glLinkProgram (pApp->glfvf.s2DTXT.sp);
        glGetProgramiv(pApp->glfvf.s2DTXT.sp, GL_LINK_STATUS, &link_ok);
        if (!link_ok) {
            print_log(pApp->glfvf.s2DTXT.sp);
            return 0x0e3f;
        }
        if ((pApp->glfvf.s2DTXT._aPos = glGetAttribLocation(pApp->glfvf.s2DTXT.sp, sz_aPos)) == -1) {
            SDL_Log("Could not bind attribute \"%s\"\n", sz_aPos);
            return 0x0e38;
        }
        if ((pApp->glfvf.s2DTXT._aTex = glGetAttribLocation(pApp->glfvf.s2DTXT.sp, sz_aTex)) == -1) {
            SDL_Log("Could not bind attribute \"%s\"\n", sz_aTex);
            return 0x0e39;
        }
        if ((pApp->glfvf.s2DTXT._aCol = glGetAttribLocation(pApp->glfvf.s2DTXT.sp, sz_aCol)) == -1) {
            SDL_Log("Could not bind attribute \"%s\"\n", sz_aCol);
            return 0x0e3a;
        }
        if ((pApp->glfvf.s2DTXT._uPos = glGetUniformLocation(pApp->glfvf.s2DTXT.sp, sz_uPos)) == -1) {
            SDL_Log("Could not bind uniform   \"%s\"\n", sz_uPos);
            return 0x0e3b;
        }
        if ((pApp->glfvf.s2DTXT._uMat = glGetUniformLocation(pApp->glfvf.s2DTXT.sp, sz_uMat)) == -1) {
            SDL_Log("Could not bind uniform   \"%s\"\n", sz_uMat);
            return 0x0e3c;
        }
        if ((pApp->glfvf.s2DTXT._uTex = glGetUniformLocation(pApp->glfvf.s2DTXT.sp, sz_uTex)) == -1) {
            SDL_Log("Could not bind uniform   \"%s\"\n", sz_uTex);
            return 0x0e3d;
        }
        if ((pApp->glfvf.s2DTXT._uTsz = glGetUniformLocation(pApp->glfvf.s2DTXT.sp, sz_uTsz)) == -1) {
            SDL_Log("Could not bind uniform   \"%s\"\n", sz_uTsz);
            return 0x0e3e;
        }
        link_ok = GL_FALSE;
    }


    glGenBuffers(1, &pApp->glfvf.vbo_mouse);
    glBindBuffer(GL_ARRAY_BUFFER, pApp->glfvf.vbo_mouse);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sv2DC)*6, NULL, GL_DYNAMIC_DRAW);

    {
        glGenBuffers(1, &pApp->glfvf.vbo_tt);
        glBindBuffer(GL_ARRAY_BUFFER, pApp->glfvf.vbo_tt);
        sv2DTXT pv[6];
        pv[0].x = 0;    pv[0].y = 0;
        pv[0].u = 0;    pv[0].v = 0;
        pv[1].x = 2048; pv[1].y = 0;
        pv[1].u = 2048; pv[1].v = 0;
        pv[2].x = 0;    pv[2].y = 2048;
        pv[2].u = 0;    pv[2].v = 2048;
        pv[3]   = pv[2];pv[4]   = pv[1];
        pv[5].x = 2048; pv[5].y = 2048;
        pv[5].u = 2048; pv[5].v = 2048;
        for(int i=0;i<6;++i) pv[i].abgr = 0xff1f3f7f;
        glBufferData(GL_ARRAY_BUFFER, sizeof(sv2DTXT)*6, pv, GL_STATIC_DRAW);

        glGenTextures(1, &pApp->glfvf.tex_tt);
    }

    {
        glGenBuffers(1, &pApp->glfvf.vbo_text);
        glBindBuffer(GL_ARRAY_BUFFER, pApp->glfvf.vbo_text);
        glBufferData(GL_ARRAY_BUFFER, sizeof(sv2DTXT)*1024*6, NULL, GL_DYNAMIC_DRAW);
    }


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return 0;
}

void resources_free(pA5App pApp) {

    a5ft_cache_free(pApp->ftCache);
    pApp->ftCache = NULL;
    a5ft_free(pApp->ftLib);
    pApp->ftLib = NULL;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);

    glDeleteProgram(pApp->glfvf.s2DTXT.sp);
    glDeleteShader (pApp->glfvf.s2DTXT.vs);
    glDeleteShader (pApp->glfvf.s2DTXT.fs);

    glDeleteProgram(pApp->glfvf.s2DCT.sp);
    glDeleteShader (pApp->glfvf.s2DCT.vs);
    glDeleteShader (pApp->glfvf.s2DCT.fs);

    glDeleteProgram(pApp->glfvf.s2DT.sp);
    glDeleteShader (pApp->glfvf.s2DT.vs);
    glDeleteShader (pApp->glfvf.s2DT.fs);

    glDeleteProgram(pApp->glfvf.s2DC.sp);
    glDeleteShader (pApp->glfvf.s2DC.vs);
    glDeleteShader (pApp->glfvf.s2DC.fs);

    glDeleteBuffers (1, &pApp->glfvf.vbo_mouse);
    glDeleteTextures(1, &pApp->glfvf.tex_tt);
    glDeleteBuffers (1, &pApp->glfvf.vbo_tt);

}

int loop(pA5App pApp) {
    SDL_Event e;
    while(pApp->bRun) {
        while(pApp->bAnimation) {
            while(SDL_PollEvent(&e)) {
                event(pApp, &e);
                if(!pApp->bRun) return pApp->iReturn;
            }
            animation(pApp);
        }
        if(SDL_WaitEvent(&e)) event(pApp, &e);
    }
    return pApp->iReturn;
}

int main(int argc, char *argv[]) {
    int o=0;
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
    if(SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO|SDL_INIT_EVENTS)) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 0xfe01;
    } else {
        A5App App;
        SDL_zero(App);
        App.bRun = 1;
        App.bAnimation = 1;

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);

        if (!(App.wnd = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN))) {
            SDL_Log("Could not create window: %s\n", SDL_GetError());
            SDL_Quit();
            return 0xfe02;
        }

        if(!(App.glctx = SDL_GL_CreateContext(App.wnd))) {
            SDL_Log("Could not create OpenGL context: %s\n", SDL_GetError());
            SDL_DestroyWindow(App.wnd);
            SDL_Quit();
            return 0xfe03;
        }

        A5_GLEXT_Init();

        glClearColor(0.3f,0.3f,0.3f,1);
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(App.wnd);

        {
            int err;
            if((err = resources_init(&App))) {
                App.bRun = 0;
                App.iReturn = 0xfe04;
                SDL_Log("Resources init fail %x\n", err);
                return 0xfe04;
            }
        }


        o=loop(&App);

        resources_free(&App);

        SDL_GL_DeleteContext(App.glctx);
        SDL_DestroyWindow(App.wnd);
    }
    SDL_Quit();
    return o;
}
