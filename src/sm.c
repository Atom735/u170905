#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include <Windows.h>
#include <malloc.h>

#include "sm.h"
#include "log.h"

UINT SM_uLog = 0;

VOID    SM_rStart() {
    if(!SM_uLog) SM_uLog = LOG_rNewCategory(TEXT("SM"));
}

LPSM    SM_rNew() {
    LPSM o = (LPSM)malloc(sizeof(SM));
    o->uDepth = 0;
    o->uMaxSize = SM_MAX_DEPTH;
    LOG(SM_uLog, LOG_DEBUG, TEXT("New mem %p"), o);
    return o;
}
VOID    SM_rDel(LPSM lpSM) {
    LOG(SM_uLog, LOG_DEBUG, TEXT("Del mem %p"), lpSM);
    free(lpSM);
}

INT     SM_rPush(LPSM lpSM, LPRSM lpFunc, LPVOID lpData) {
    LOG(SM_uLog, LOG_DEBUG, TEXT("Try to push to mem %p (%d/%d), func %p, with data %p"), lpSM, lpSM->uDepth, SM_MAX_DEPTH, lpFunc, lpData);

    if(lpSM->uDepth >= SM_MAX_DEPTH) {
            LOG(SM_uLog, LOG_ERROR, TEXT("Error! SM in mem %p, is full!"), lpSM);
        return 0;
    }
    lpSM->alpFunc[lpSM->uDepth] = lpFunc;
    ++lpSM->uDepth;
    INT o = lpFunc(lpSM, SM_MSG_PUSH, lpData);
    LOG(SM_uLog, LOG_DEBUG, TEXT("push is OK with %d"), o);
    return o;
}
INT     SM_rPop(LPSM lpSM, LPVOID lpData) {
    LOG(SM_uLog, LOG_DEBUG, TEXT("Try pop in mem %p (%d/%d), with data %p"), lpSM, lpSM->uDepth, SM_MAX_DEPTH, lpData);
    if(lpSM->uDepth == 0) {
        LOG(SM_uLog, LOG_ERROR, TEXT("Error! SM in mem %p, is empty!"), lpSM);
        return 0;
    }
    --lpSM->uDepth;
    if(!lpSM->alpFunc[lpSM->uDepth]) {
        LOG(SM_uLog, LOG_FATAL, TEXT("Error! SM func in mem %p (%d/%d), is empty!"), lpSM, lpSM->uDepth, SM_MAX_DEPTH);
        return 0;
    }
    INT o = lpSM->alpFunc[lpSM->uDepth](lpSM, SM_MSG_POP, lpData);
    LOG(SM_uLog, LOG_DEBUG, TEXT("pop is OK with %d"), o);
    return o;
}
VOID    SM_rPopAll(LPSM lpSM, LPVOID lpData) {
    LOG(SM_uLog, LOG_DEBUG, TEXT("Try popAll in mem %p (%d/%d), with data %p"), lpSM, lpSM->uDepth, SM_MAX_DEPTH, lpData);
    if(lpSM->uDepth == 0) {
        LOG(SM_uLog, LOG_ERROR, TEXT("Error! SM in mem %p, is empty!"), lpSM);
        return;
    }
    while(lpSM->uDepth) {
        --lpSM->uDepth;
        if(!lpSM->alpFunc[lpSM->uDepth]) {
            LOG(SM_uLog, LOG_FATAL, TEXT("Error! SM func in mem %p (%d/%d), is empty!"), lpSM, lpSM->uDepth, SM_MAX_DEPTH);
            continue;
        }
        INT o = lpSM->alpFunc[lpSM->uDepth](lpSM, SM_MSG_POP, lpData);
        LOG(SM_uLog, LOG_DEBUG, TEXT("popAll(%d/%d) is OK with %d"), lpSM->uDepth, SM_MAX_DEPTH, o);
    }
}

INT     SM_rProcess(LPSM lpSM, UINT uMsg, LPVOID lpData) {
    LOG(SM_uLog, LOG_VERBOSE, TEXT("Try process in mem %p (%d/%d), with data %p"), lpSM, lpSM->uDepth, SM_MAX_DEPTH, lpData);
    if(lpSM->uDepth == 0) {
        LOG(SM_uLog, LOG_ERROR, TEXT("Error! SM in mem %p, is empty!"), lpSM);
        return 0;
    }
    if(!lpSM->alpFunc[lpSM->uDepth-1]) {
        LOG(SM_uLog, LOG_FATAL, TEXT("Error! SM func in mem %p (%d/%d), is empty!"), lpSM, lpSM->uDepth, SM_MAX_DEPTH);
        return 0;
    }
    INT o = lpSM->alpFunc[lpSM->uDepth-1](lpSM, uMsg, lpData);
    LOG(SM_uLog, LOG_VERBOSE, TEXT("process is OK with %d"), o);
    return o;
}
VOID    SM_rProcessAll(LPSM lpSM, UINT uMsg, LPVOID lpData) {
    LOG(SM_uLog, LOG_VERBOSE, TEXT("Try processAll in mem %p (%d/%d), with data %p"), lpSM, lpSM->uDepth, SM_MAX_DEPTH, lpData);
    if(lpSM->uDepth == 0) {
        LOG(SM_uLog, LOG_ERROR, TEXT("Error! SM in mem %p, is empty!"), lpSM);
        return;
    }
    for(UINT i=0; i<lpSM->uDepth; ++i) {
        if(!lpSM->alpFunc[i]) {
            LOG(SM_uLog, LOG_FATAL, TEXT("Error! SM func in mem %p (%d/%d), is empty!"), lpSM, i, lpSM->uDepth);
            continue;
        }
        INT o = lpSM->alpFunc[i](lpSM, uMsg, lpData);
        LOG(SM_uLog, LOG_VERBOSE, TEXT("processAll(%d/%d) is OK with %d"), i, lpSM->uDepth, o);
    }
}
VOID    SM_rProcessAllETS(LPSM lpSM, UINT uMsg, LPVOID lpData) {
    LOG(SM_uLog, LOG_VERBOSE, TEXT("Try processAllETS in mem %p (%d/%d), with data %p"), lpSM, lpSM->uDepth, SM_MAX_DEPTH, lpData);
    if(lpSM->uDepth == 0) {
            LOG(SM_uLog, LOG_ERROR, TEXT("Error! SM in mem %p, is empty!"), lpSM);
        return;
    }
    for(UINT i=lpSM->uDepth-1; i<lpSM->uDepth;--i) {
        if(!lpSM->alpFunc[i]) {
            LOG(SM_uLog, LOG_FATAL, TEXT("Error! SM func in mem %p (%d/%d), is empty!"), lpSM, i, lpSM->uDepth);
            continue;
        }
        INT o = lpSM->alpFunc[i](lpSM, uMsg, lpData);
        LOG(SM_uLog, LOG_VERBOSE, TEXT("processAllETS(%d/%d) is OK with %d"), i, lpSM->uDepth, o);
        if(!i) return;
    }
}
