#ifndef _H_A5_APP
#define _H_A5_APP 170930

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>

#include <GL/gl.h>
#include <GL/glext.h>


#include "a5_err.h"

typedef struct tagA5S_App {
    /* Хендл окна */
    SDL_Window         *SDL_wnd;
    /* Контекст устройства OpenGL */
    SDL_GLContext       SDL_glctx;
    /* Последнее событие */
    SDL_Event           SDL_event;

    /* Флаг работы приложения */
    int                 bRun;
    /* Флаг анмимации */
    int                 bAnimation;
    /* Возвращаемое значение */
    int                 iReturn;

} A5S_App, *pA5S_App;

int  A5APP_Init(pA5S_App pApp);
void A5APP_Exit(pA5S_App pApp);
int  A5APP_Loop(pA5S_App pApp);
void A5APP_Evnt(pA5S_App pApp);
void A5APP_Idle(pA5S_App pApp);
int  A5APP_ResInit(pA5S_App pApp);
void A5APP_ResExit(pA5S_App pApp);


#endif
