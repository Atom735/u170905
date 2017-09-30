#ifndef _H_A5_ERR
#define _H_A5_ERR 170930

enum {
    A5E_OK = 0,
    A5E_SDL_INIT_FAIL,
    A5E_SDL_WND_CREATE,
    A5E_SDL_GLCTX_CREATE,
};

const char *A5ERR(int err);

#endif
