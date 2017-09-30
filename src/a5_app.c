
#include "a5_app.h"

int main(int argc, char *argv[]) {
    int o=0;
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS))
        return A5E_SDL_INIT_FAIL;
    pA5S_App pApp = NULL;
    A5S_App _App;
    pApp = &_App;
    SDL_memset(pApp, 0, sizeof(A5S_App));
    if((o=A5APP_Init(pApp))) goto P_APPEXIT;
    if((o=A5APP_ResInit(pApp))) goto P_RESEXIT;
    o=A5APP_Loop(pApp);
    P_RESEXIT:
    A5APP_ResExit(pApp);
    P_APPEXIT:
    A5APP_Exit(pApp);
    pApp = NULL;
    SDL_Quit();
    return SDL_Log("Exit code %d (%s)", o, A5ERR(o)), o;
}

int A5APP_Init(pA5S_App pApp) {

    pApp->bRun          = 1;
    pApp->bAnimation    = 0;
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

    return A5E_OK;
}
void A5APP_Exit(pA5S_App pApp) {
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
    return A5E_OK;
}
void A5APP_ResExit(pA5S_App pApp) {
}

void A5APP_Evnt(pA5S_App pApp) {
}
void A5APP_Idle(pA5S_App pApp) {
    SDL_GL_SwapWindow(pApp->SDL_wnd);
}

