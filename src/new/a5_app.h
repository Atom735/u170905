#ifndef _H_A5_APP
#define _H_A5_APP 171007

#include "a5_types.h"

#include <SDL2/SDL.h>
#include "a5_ft.h"
#include "a5_gl.h"

A5TYPEDEF(struct A5sApp,        sApp);

A5uint  A5APP_Init(A5psApp pApp);
A5void  A5APP_Free(A5psApp pApp);
A5uint  A5APP_Loop(A5psApp pApp);
A5void  A5APP_Evnt(A5psApp pApp);
A5void  A5APP_Idle(A5psApp pApp);
A5uint  A5APP_ResInit(A5psApp pApp);
A5void  A5APP_ResFree(A5psApp pApp);

struct A5sApp {
    A5uint              uObjectId;
    SDL_Window         *SDL_Window;
    SDL_GLContext       SDL_GLContext;
    SDL_Event           SDL_Event;
    A5uint              uGLExtVersion;
    A5psFT_Lib          FT_Lib;
    A5psFT_Cache        FT_Cache;

    A5bool              bRun;
    A5bool              bAnimation;
    A5uint              uReturn;

    A5sGL_2DTXT         gls_2DTXT;
    GLuint              glu_VBO_2DTXT_DBG;
    GLuint              glu_VBO_TEXT;
    GLuint              glu_TEX_2DTXT_DBG;
};


#endif /* _H_A5_APP */
