#include "a5_app.h"

#include "a5_err.h"
#include "a5_gl.h"

int main(int argc, char *argv[]) {
    int o=0;
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS))
        return A5E_SDL_INIT_FAIL;
    pA5S_App pApp = NULL;
    A5S_App _App;
    pApp = &_App;
    SDL_memset(pApp, 0, sizeof(A5S_App));
    if((o=A5APP_Init(pApp))) goto P_APP_FREE;
    if((o=A5APP_ResInit(pApp))) goto P_RES_FREE;
    o=A5APP_Loop(pApp);
    P_RES_FREE:
    A5APP_ResFree(pApp);
    P_APP_FREE:
    A5APP_Free(pApp);
    pApp = NULL;
    SDL_Quit();
    return SDL_Log("Exit code %d (%s)", o, A5ERR(o)), o;
}

int A5APP_Init(pA5S_App pApp) {

    pApp->bRun          = 1;
    pApp->bAnimation    = 1;
    pApp->iReturn       = A5E_OK;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);

    if (!(pApp->SDL_wnd = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN))) {
        return A5E_SDL_WND_CREATE;
    }

    if(!(pApp->SDL_glctx = SDL_GL_CreateContext(pApp->SDL_wnd))) {
        return A5E_SDL_GLCTX_CREATE;
    }

    glClearColor(0.3f,0.3f,0.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    SDL_GL_SwapWindow(pApp->SDL_wnd);

    pApp->glExtVersion = A5GL_Init();

    return A5E_OK;
}
void A5APP_Free(pA5S_App pApp) {
    SDL_GL_DeleteContext(pApp->SDL_glctx);
    SDL_DestroyWindow(pApp->SDL_wnd);
    SDL_memset(pApp, 0, sizeof(A5S_App));
}


int  A5APP_Loop(pA5S_App pApp) {
    while(pApp->bRun) {
        while(pApp->bAnimation) {
            while(SDL_PollEvent(&pApp->SDL_event)) {
                A5APP_Evnt(pApp);
                if(!pApp->bRun) return pApp->iReturn;
            }
            A5APP_Idle(pApp);
        }
        if(SDL_WaitEvent(&pApp->SDL_event)) A5APP_Evnt(pApp);
    }
    return pApp->iReturn;
}
int  A5APP_ResInit(pA5S_App pApp) {
    pApp->FT_Lib = A5FT_Init();
    pApp->FT_Cache = A5FT_CacheInit(pApp->FT_Lib, 1<<11, 2);
    A5FT_FontNewFromFile(pApp->FT_Cache, "data/fonts/RobotoSlab-Regular.ttf");

    A5GL_2DTXT_Create(&pApp->gl.s2DTXT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    {
        A5V_2DTXT pBuf[12] = {
            {.x =    0, .y =    0, .u =    0, .v =    0, .r = 0x7f, .g = 0xff, .b = 0x7f, .a = 0x7f},
            {.x =1<<14, .y =    0, .u =1<<14, .v =    0, .r = 0x7f, .g = 0xff, .b = 0x7f, .a = 0x7f},
            {.x =    0, .y =1<<14, .u =    0, .v =1<<14, .r = 0x7f, .g = 0xff, .b = 0x7f, .a = 0x7f},
            {.x =    0, .y =1<<14, .u =    0, .v =1<<14, .r = 0x7f, .g = 0xff, .b = 0x7f, .a = 0x7f},
            {.x =1<<14, .y =    0, .u =1<<14, .v =    0, .r = 0x7f, .g = 0xff, .b = 0x7f, .a = 0x7f},
            {.x =1<<14, .y =1<<14, .u =1<<14, .v =1<<14, .r = 0x7f, .g = 0xff, .b = 0x7f, .a = 0x7f},

            {.x =    0, .y =    0, .u =    0, .v =    0, .r = 0xff, .g = 0x7f, .b = 0xff, .a = 0xff},
            {.x =1<<14, .y =    0, .u =1<<14, .v =    0, .r = 0xff, .g = 0x7f, .b = 0xff, .a = 0xff},
            {.x =    0, .y =1<<14, .u =    0, .v =1<<14, .r = 0xff, .g = 0x7f, .b = 0xff, .a = 0xff},
            {.x =    0, .y =1<<14, .u =    0, .v =1<<14, .r = 0xff, .g = 0x7f, .b = 0xff, .a = 0xff},
            {.x =1<<14, .y =    0, .u =1<<14, .v =    0, .r = 0xff, .g = 0x7f, .b = 0xff, .a = 0xff},
            {.x =1<<14, .y =1<<14, .u =1<<14, .v =1<<14, .r = 0xff, .g = 0x7f, .b = 0xff, .a = 0xff},
        };
        pApp->gl.vbo_DBG_FT_Cache = A5GL_2DTXT_CreateVBO_S(2, pBuf);
        glGenTextures(1, &pApp->gl.tex_DBG_FT_Cache);

        pApp->gl.hsz_DBG_CLIPBOARD = 12;
        pApp->gl.vbo_DBG_CLIPBOARD = A5GL_2DTXT_CreateVBO_D(pApp->gl.vsz_DBG_CLIPBOARD = 1<<14);
    }

    glGenTextures(1, &pApp->gl.tex_FT_Cache);

    return A5E_OK;
}
void A5APP_ResFree(pA5S_App pApp) {

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);

    glDeleteBuffers (1, &pApp->gl.vbo_DBG_CLIPBOARD);
    glDeleteBuffers (1, &pApp->gl.vbo_DBG_FT_Cache);
    glDeleteTextures(1, &pApp->gl.tex_DBG_FT_Cache);
    glDeleteTextures(1, &pApp->gl.tex_FT_Cache);

    A5GL_2DTXT_Release(&pApp->gl.s2DTXT);

    A5FT_CacheFree(pApp->FT_Cache);
    pApp->FT_Cache = NULL;
    A5FT_Free(pApp->FT_Lib);
    pApp->FT_Lib = NULL;
}

void A5APP_Evnt(pA5S_App pApp) {
    switch(pApp->SDL_event.type) {
        case SDL_KEYDOWN: {
            switch((unsigned int)pApp->SDL_event.key.keysym.scancode) {
                case SDL_SCANCODE_1: {
                    if(pApp->iDbgState == 1) pApp->iDbgState = 0;
                    else pApp->iDbgState = 1;
                    return;
                }
                case SDL_SCANCODE_2: {
                    if(pApp->iDbgState == 2) pApp->iDbgState = 0;
                    else pApp->iDbgState = 2;
                    return;
                }
                case SDL_SCANCODE_0: {
                    pApp->iDbgState = 0;
                    return;
                }
                case SDL_SCANCODE_MINUS: {
                    if(pApp->iDbgState == 2) --pApp->gl.hsz_DBG_CLIPBOARD;
                    if(pApp->gl.hsz_DBG_CLIPBOARD < 4) pApp->gl.hsz_DBG_CLIPBOARD=4;
                    return;
                }
                case SDL_SCANCODE_EQUALS: {
                    if(pApp->iDbgState == 2) ++pApp->gl.hsz_DBG_CLIPBOARD;
                    return;
                }
            }
            return;
        }
        case SDL_WINDOWEVENT: {
            switch(pApp->SDL_event.window.event) {
                case SDL_WINDOWEVENT_RESIZED: {
                    if(pApp->SDL_glctx) {
                        glViewport(0, 0, pApp->SDL_event.window.data1, pApp->SDL_event.window.data2);
                    }
                    pApp->bAnimation = 1;
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
void A5APP_Idle(pA5S_App pApp) {

    int ds_w, ds_h;
    SDL_GL_GetDrawableSize(pApp->SDL_wnd, &ds_w, &ds_h);
    int ms_x, ms_y;
    SDL_GetMouseState(&ms_x, &ms_y);

    if(A5FT_CacheUpdate(pApp->FT_Cache, 0)) {
        glBindTexture(GL_TEXTURE_2D, pApp->gl.tex_FT_Cache);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, A5FT_CacheGetSize(pApp->FT_Cache), A5FT_CacheGetSize(pApp->FT_Cache), 0, GL_RED, GL_UNSIGNED_BYTE, pApp->FT_Cache);
        glBindTexture(GL_TEXTURE_2D, pApp->gl.tex_DBG_FT_Cache);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, A5FT_CacheGetSize(pApp->FT_Cache), A5FT_CacheGetSize(pApp->FT_Cache), 0, GL_RED, GL_UNSIGNED_BYTE, (GLubyte*)pApp->FT_Cache+A5FT_CacheGetSize(pApp->FT_Cache)*A5FT_CacheGetSize(pApp->FT_Cache));

    }

    glClearColor(0.3f,0.3f,0.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if(pApp->iDbgState == 1) {
        glClearColor(0.3f,0.3f,0.3f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        A5GL_2DTXT_DrawBegin(&pApp->gl.s2DTXT, ds_w, ds_h);
        A5GL_2DTXT_DrawSetTex(&pApp->gl.s2DTXT, pApp->gl.tex_DBG_FT_Cache, A5FT_CacheGetSize(pApp->FT_Cache));
        A5GL_2DTXT_DrawVbo(&pApp->gl.s2DTXT, 8-ms_x*8, 8-ms_y*8, pApp->gl.vbo_DBG_FT_Cache, 1, 1);
        A5GL_2DTXT_DrawSetTex(&pApp->gl.s2DTXT, pApp->gl.tex_FT_Cache, A5FT_CacheGetSize(pApp->FT_Cache));
        A5GL_2DTXT_DrawVbo(&pApp->gl.s2DTXT, 8-ms_x*8, 8-ms_y*8, pApp->gl.vbo_DBG_FT_Cache, 0, 1);
    } else if(pApp->iDbgState == 2) {
        glClearColor(0.3f,0.3f,0.3f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        A5GL_2DTXT_DrawBegin(&pApp->gl.s2DTXT, ds_w, ds_h);
        A5GL_2DTXT_DrawSetTex(&pApp->gl.s2DTXT, pApp->gl.tex_FT_Cache, A5FT_CacheGetSize(pApp->FT_Cache));

        A5S_TextSettings ts;
        GLuint uCount = 0;
        SDL_memset(&ts, 0, sizeof(A5S_TextSettings));
        if(SDL_HasClipboardText()) {
            ts.szText = SDL_GetClipboardText();
            uCount = pApp->gl.vsz_DBG_CLIPBOARD;
        } else {
            ts.szText = SDL_malloc(32);
            uCount = 11;
            SDL_memcpy(ts.szText, "No Clipboard", 13);
        }

        ts.pCache = pApp->FT_Cache;
        ts.iHeight = pApp->gl.hsz_DBG_CLIPBOARD;
        ts.R = 0xef;
        ts.G = 0xff;
        ts.B = 0xdf;
        ts.A = 0xff;

        GLuint strSz = A5GL_2DTXT_PrepareVboText(pApp->gl.vbo_DBG_CLIPBOARD, 0, uCount, &ts);
        SDL_free(ts.szText);
        A5GL_2DTXT_DrawVbo(&pApp->gl.s2DTXT, 8-ms_x*8, 8-ms_y*4+ts.iHeight, pApp->gl.vbo_DBG_CLIPBOARD, 0, strSz);
    }

    SDL_GL_SwapWindow(pApp->SDL_wnd);
    // pApp->bAnimation = 0;

}

