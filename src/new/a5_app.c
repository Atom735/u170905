#include "a5_app.h"

A5sApp      _App;

int main(int argc, char *argv[]) {
    A5uint o;
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS))
        return 0;
    A5psApp pApp;
    pApp = &_App;
    SDL_memset(pApp, 0, sizeof(A5sApp));


    if((o=A5APP_Init(pApp))) goto P_APP_FREE;
    if((o=A5APP_ResInit(pApp))) goto P_RES_FREE;
    o=A5APP_Loop(pApp);
    P_RES_FREE:
    A5APP_ResFree(pApp);
    P_APP_FREE:
    A5APP_Free(pApp);
    SDL_Quit();
    return SDL_Log("Exit code %d", o), o;
}

A5uint  A5APP_Init(A5psApp pApp) {
    pApp->uObjectId     = A5OID_APP;

    pApp->bRun          = 1;
    pApp->bAnimation    = 1;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);

    if (!(pApp->SDL_Window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN))) {
        return 1;
    }

    if(!(pApp->SDL_GLContext = SDL_GL_CreateContext(pApp->SDL_Window))) {
        return 2;
    }

    glClearColor(0.3f,0.3f,0.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    SDL_GL_SwapWindow(pApp->SDL_Window);

    return 0;
}
A5void  A5APP_Free(A5psApp pApp) {
    SDL_GL_DeleteContext(pApp->SDL_GLContext);
    SDL_DestroyWindow(pApp->SDL_Window);
    SDL_memset(pApp, 0, sizeof(A5sApp));
}

A5uint  A5APP_Loop(A5psApp pApp) {
    while(pApp->bRun) {
        while(pApp->bAnimation) {
            while(SDL_PollEvent(&pApp->SDL_Event)) {
                A5APP_Evnt(pApp);
                if(!pApp->bRun) return pApp->uReturn;
            }
            A5APP_Idle(pApp);
        }
        if(SDL_WaitEvent(&pApp->SDL_Event)) A5APP_Evnt(pApp);
    }
    return pApp->uReturn;
}
A5uint  A5APP_ResInit(A5psApp pApp) {

    pApp->FT_Lib = A5FT_LibCreate(2, 2);
    A5FT_FontNewFromFile(pApp->FT_Lib, "data/fonts/RobotoSlab-Regular.ttf");
    A5FT_SizesSet(pApp->FT_Lib, 1, 1, 16);
    pApp->FT_Cache = A5FT_CacheCreate(pApp->FT_Lib, 1024);

    for(A5uint i=0x20; i<0x7f; ++i) A5FT_GlyphNew(pApp->FT_Cache, 1, i);

    A5uint8 pBuf[256*256];
    for(A5uint y = 0; y < 256; ++y) {
        for(A5uint x = 0; x < 256; ++x) {
            A5uint r = x*x+y*y;
            pBuf[x+y*256] = r;
        }
    }
    A5FT_GlyphNewCustom(pApp->FT_Cache, 256, 256, pBuf, 256);
    for(A5uint y = 0; y < 256; ++y) {
        for(A5uint x = 0; x < 256; ++x) {
            A5uint r = x*x+y*y;
            pBuf[x+y*256] = (A5uint8)(SDL_sqrtf((float)r)*0.5f);
        }
    }
    A5FT_GlyphNewCustom(pApp->FT_Cache, 256, 256, pBuf, 256);
    for(A5uint y = 0; y < 256; ++y) {
        for(A5uint x = 0; x < 256; ++x) {
            pBuf[x+y*256] = x|y;
        }
    }
    A5FT_GlyphNewCustom(pApp->FT_Cache, 256, 256, pBuf, 256);
    for(A5uint y = 0; y < 256; ++y) {
        for(A5uint x = 0; x < 256; ++x) {
            pBuf[x+y*256] = x&y;
        }
    }
    A5FT_GlyphNewCustom(pApp->FT_Cache, 256, 256, pBuf, 256);
    for(A5uint y = 0; y < 256; ++y) {
        for(A5uint x = 0; x < 256; ++x) {
            pBuf[x+y*256] = x*y;
        }
    }
    A5FT_GlyphNewCustom(pApp->FT_Cache, 256, 256, pBuf, 256);
    for(A5uint y = 0; y < 256; ++y) {
        for(A5uint x = 0; x < 256; ++x) {
            pBuf[x+y*256] = x%(y+1);
        }
    }
    A5FT_GlyphNewCustom(pApp->FT_Cache, 256, 256, pBuf, 256);
    for(A5uint y = 0; y < 256; ++y) {
        for(A5uint x = 0; x < 256; ++x) {
            pBuf[x+y*256] = (A5uint8)(SDL_sinf((float)x*0.1f)*(float)y);
        }
    }
    A5FT_GlyphNewCustom(pApp->FT_Cache, 256, 256, pBuf, 256);
    for(A5uint y = 0; y < 256; ++y) {
        for(A5uint x = 0; x < 256; ++x) {
            float fx = (float)x-127.5f;
            float fy = (float)y-127.5f;
            float r = SDL_sqrtf(fx*fx+fy*fy);
            if(r<10.f)
                pBuf[x+y*256] = (A5uint8)((10.f-r)*25.5f);
            else if((r>=100.f) && (r<110.f))
                pBuf[x+y*256] = (A5uint8)((r-100.f)*25.5f);
            else if((r>=110.f) && (r<120.f))
                pBuf[x+y*256] = (A5uint8)((120.f-r)*25.5f);
            else
                pBuf[x+y*256] = 0;
        }
    }
    A5FT_GlyphNewCustom(pApp->FT_Cache, 256, 256, pBuf, 256);
    for(A5uint y = 0; y < 256; ++y) {
        for(A5uint x = 0; x < 256; ++x) {
            float fx = (float)x-127.5f;
            float fy = (float)y-127.5f;
            float r = SDL_sqrtf(fx*fx+fy*fy);
            if(r<120.f)
                pBuf[x+y*256] = (A5uint8)(r*2.125f);
            else
                pBuf[x+y*256] = 0;
        }
    }
    A5FT_GlyphNewCustom(pApp->FT_Cache, 256, 256, pBuf, 256);
    A5uint rr = 4;
    for(A5uint y = 0; y < 256; ++y) {
        for(A5uint x = 0; x < 256; ++x) {
            rr+=x*3+x*y+y+(y*7+x*x+y*y*9)/3;
            pBuf[x+y*256] = rr;
        }
    }
    A5FT_GlyphNewCustom(pApp->FT_Cache, 256, 256, pBuf, 256);
    for(A5uint y = 0; y < 256; ++y) {
        for(A5uint x = 0; x < 256; ++x) {
            float fx = (float)x-127.5f;
            float fy = (float)y-127.5f;
            float fs = SDL_sinf(fx*0.1f)*50.f-fy;
            if(fs < 0.0f) fs = -fs;
            if(fs < 50.f)
                pBuf[x+y*256] = (A5uint8)(255.f-fs*5.1f);
            else
                pBuf[x+y*256] = 0;
        }
    }
    A5FT_GlyphNewCustom(pApp->FT_Cache, 256, 256, pBuf, 256);
    for(A5uint y = 0; y < 256; ++y) {
        for(A5uint x = 0; x < 256; ++x) {
            pBuf[x+y*256] = x/(y+1);
        }
    }
    A5FT_GlyphNewCustom(pApp->FT_Cache, 256, 256, pBuf, 256);

    pApp->uGLExtVersion = A5GL_Init();
    A5GL_2DTXT_Init(&pApp->gls_2DTXT);
    {
        A5sv_2DTXT pBuf[12] = {
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
        pApp->glu_VBO_2DTXT_DBG = A5GL_2DTXT_CreateVBO_Static(2, pBuf);
        glGenTextures(1, &pApp->glu_TEX_2DTXT_DBG);
        pApp->glu_VBO_TEXT = A5GL_2DTXT_CreateVBO_Dynamic(4096);
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return 0;
}
A5void  A5APP_ResFree(A5psApp pApp) {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);

    glDeleteBuffers (1, &pApp->glu_VBO_TEXT);
    glDeleteBuffers (1, &pApp->glu_VBO_2DTXT_DBG);
    glDeleteTextures(1, &pApp->glu_TEX_2DTXT_DBG);

    A5GL_2DTXT_Free(&pApp->gls_2DTXT);

    A5FT_CacheFree(pApp->FT_Cache);
    A5FT_LibFree(pApp->FT_Lib);
}

A5void  A5APP_Evnt(A5psApp pApp) {
    switch(pApp->SDL_Event.type) {
        case SDL_WINDOWEVENT:
            switch(pApp->SDL_Event.window.event)
                case SDL_WINDOWEVENT_RESIZED:
                    if(pApp->SDL_GLContext)
                        glViewport(0, 0, pApp->SDL_Event.window.data1, pApp->SDL_Event.window.data2);
                    pApp->bAnimation = 1;
                    return;
            return;
        case SDL_QUIT:
            pApp->bRun = 0;
            return;
    }
}
A5void  A5APP_Idle(A5psApp pApp) {
    A5int ds_w, ds_h;
    SDL_GL_GetDrawableSize(pApp->SDL_Window, &ds_w, &ds_h);
    A5int ms_x, ms_y;
    SDL_GetMouseState(&ms_x, &ms_y);
    static Uint64 u64_OldCounter = 0;
    static Uint64 u64_NewCounter = 0;
    static A5uint uCounts = 0;

    float FF=0.f;
    float FPS=0.f;

    A5uint Freq;
    Freq = (A5uint)SDL_GetPerformanceFrequency();
    uCounts = (A5uint)(u64_NewCounter-u64_OldCounter);
    FF = (float)uCounts / (float)Freq;
    if(uCounts) FPS = (float)Freq / (float)uCounts;

    u64_OldCounter = SDL_GetPerformanceCounter();

    glClearColor(0.3f,0.3f,0.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if(pApp->FT_Cache->uFlagUpdate) {
        glBindTexture(GL_TEXTURE_2D, pApp->glu_TEX_2DTXT_DBG);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1024, 1024, 0, GL_RED, GL_UNSIGNED_BYTE, pApp->FT_Cache);
        pApp->FT_Cache->uFlagUpdate = 0;
    }

    A5GL_2DTXT_DrawBegin(&pApp->gls_2DTXT, ds_w, ds_h);
    A5GL_2DTXT_DrawSetTexture(&pApp->gls_2DTXT, pApp->glu_TEX_2DTXT_DBG, 2048);
    A5GL_2DTXT_DrawSetPos(&pApp->gls_2DTXT, 32-ms_x*8, 32-ms_y*8);
    A5GL_2DTXT_DrawVBO(&pApp->gls_2DTXT, pApp->glu_VBO_2DTXT_DBG, 0, 1);

    A5sSETTINGS_TXT Settings;
    char str[4096];
    Settings.pzTxt = str;
    Settings.pCache = pApp->FT_Cache;
    Settings.uSizeID = 1;
    Settings.iHeight = 18;
    Settings.iKerning = 0;
    Settings.iPosX = 512;
    Settings.iPosY = 256;
    Settings.r = 0xff;
    Settings.g = 0xff;
    Settings.b = 0xff;
    Settings.a = 0x7f;

    char *pCB = SDL_GetClipboardText();

    SDL_snprintf(str, 4096, u8"Количество тиков со старта: [%u]\nВремя последнего кадра: [%.5f]\nFPS: [%.5f]\nА тут твой буффер обмена:\n%s", SDL_GetTicks(), FF, FPS, pCB);

    if(pCB) SDL_free(pCB);

    GLuint txtSz = 0;
    txtSz += A5GL_2DTXT_PrepareVboText(pApp->glu_VBO_TEXT, txtSz, 4096-txtSz, &Settings);


    A5GL_2DTXT_DrawSetPos(&pApp->gls_2DTXT, 32-ms_x*2, 32-ms_y*2);
    A5GL_2DTXT_DrawSetTexture(&pApp->gls_2DTXT, pApp->glu_TEX_2DTXT_DBG, 1024);
    A5GL_2DTXT_DrawVBO(&pApp->gls_2DTXT, pApp->glu_VBO_TEXT, 0, txtSz);

    SDL_GL_SwapWindow(pApp->SDL_Window);

    u64_NewCounter = SDL_GetPerformanceCounter();
}

