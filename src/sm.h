#ifndef _H_SM
#define _H_SM

#include <WinDef.h>

#define SM_MAX_DEPTH 254

typedef struct sSM SM, *PSM, *LPSM;

typedef INT (*LPRSM)(LPSM, UINT, LPVOID);

struct sSM {
    UINT uDepth;
    UINT uMaxSize;
    LPRSM alpFunc[SM_MAX_DEPTH];
};

enum {
    SM_MSG_NULL = 0,
    SM_MSG_PUSH,
    SM_MSG_POP,
    SM_MSG_USER,
};

VOID    SM_rStart();

LPSM    SM_rNew();
VOID    SM_rDel(LPSM lpSM);

INT     SM_rPush(LPSM lpSM, LPRSM lpFunc, LPVOID lpData);
INT     SM_rPop(LPSM lpSM, LPVOID lpData);
VOID    SM_rPopAll(LPSM lpSM, LPVOID lpData);

INT     SM_rProcess(LPSM lpSM, UINT uMsg, LPVOID lpData);
VOID    SM_rProcessAll(LPSM lpSM, UINT uMsg, LPVOID lpData);
VOID    SM_rProcessAllETS(LPSM lpSM, UINT uMsg, LPVOID lpData);

#endif
