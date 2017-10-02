#ifndef _H_A5_APP
#define _H_A5_APP 170930

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>

#include "a5_ft.h"
#include "a5_gl.h"

typedef struct tagA5S_App {
    /* Хендл окна */
    SDL_Window         *SDL_wnd;
    /* Контекст устройства OpenGL */
    SDL_GLContext       SDL_glctx;
    int                 glExtVersion;
    /* Последнее событие */
    SDL_Event           SDL_event;
    /* Библиотека FT */
    pA5FT_Lib           FT_Lib;
    /* Кеш FT */
    pA5FT_Cache         FT_Cache;

    /* Флаг работы приложения */
    int                 bRun;
    /* Флаг анмимации */
    int                 bAnimation;
    /* Возвращаемое значение */
    int                 iReturn;

    /* Дебаг состояние */
    int                 iDbgState;



    struct {
        A5GLV_2DTXT     s2DTXT;
        GLuint          vbo_DBG_FT_Cache;
        GLuint          tex_DBG_FT_Cache;
        GLuint          uib_DBG_FT_Cache;
        GLuint          tex_FT_Cache;

        GLuint          hsz_DBG_CLIPBOARD;
        GLuint          vsz_DBG_CLIPBOARD;
        GLuint          vbo_DBG_CLIPBOARD;
    } gl;

} A5S_App, *pA5S_App;

int  A5APP_Init(pA5S_App pApp);
void A5APP_Free(pA5S_App pApp);
int  A5APP_Loop(pA5S_App pApp);
void A5APP_Evnt(pA5S_App pApp);
void A5APP_Idle(pA5S_App pApp);
int  A5APP_ResInit(pA5S_App pApp);
void A5APP_ResFree(pA5S_App pApp);


#endif
