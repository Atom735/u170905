#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include <Windows.h>

#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>

#include "log.h"

UINT             g_uLogApp = 0;
UINT             g_uLogOGL = 0;
UINT             g_uLogSM = 0;
UINT             g_uLogFT = 0;

BOOL    LOG_bRun                 = FALSE;
HANDLE  LOG_hFile                = INVALID_HANDLE_VALUE;
LPCTSTR LOG_alpctszCategories[LOG_MAX_CATEGORIES];
BOOL    LOG_abCategories[LOG_MAX_CATEGORIES];
UINT    LOG_uLvl                 = LOG_I;
TCHAR   LOG_bufOut[LOG_MAX_MESAGELEN];
UINT    LOG_bufOutSz             = 0;


VOID    LOG_rStart() {
    LOG_hFile = LOG_rOtherStart(TEXT("LOG.txt"));
    LOG_bRun = (LOG_hFile != INVALID_HANDLE_VALUE);
    LOG_alpctszCategories[0] = TEXT("NULL");
    LOG_abCategories[0] = FALSE;

    g_uLogApp = LOG_rNewCategory(TEXT("App"));
    g_uLogOGL = LOG_rNewCategory(TEXT("OpenGL"));
    g_uLogSM = LOG_rNewCategory(TEXT("StateMan"));
    g_uLogFT = LOG_rNewCategory(TEXT("FreeType"));

    LOG(0, LOG_INFO, TEXT("Log start"));
}
VOID    LOG_rFin() {
    LOG(0, LOG_INFO, TEXT("Log end"));
    if(LOG_hFile != INVALID_HANDLE_VALUE)
    LOG_rOtherFin(LOG_hFile);
}
UINT    LOG_rSetLvl(UINT uLvl) {
    UINT o = LOG_uLvl;
    LOG_uLvl = uLvl;
    return o;
}
UINT    LOG_rNewCategory(LPCTSTR lpctszCategory) {
    for(UINT i=0; i<LOG_MAX_CATEGORIES; ++i) {
        if(!LOG_alpctszCategories[i]) {
            LOG_alpctszCategories[i] = lpctszCategory;
            return i;
        }
    }
    return 0;
}
LPCTSTR LOG_rGetCategoryStr(UINT uCategory) {
    if(uCategory >= LOG_MAX_CATEGORIES) return NULL;
    return LOG_alpctszCategories[uCategory];
}
LPCTSTR LOG_rSetCategoryStr(UINT uCategory, LPCTSTR lpctszCategory) {
    if(uCategory >=  LOG_MAX_CATEGORIES) return NULL;
    LPCTSTR o = LOG_alpctszCategories[uCategory];
    LOG_alpctszCategories[uCategory] = lpctszCategory;
    return o;
}
BOOL    LOG_rSetCategory(UINT uCategory, BOOL bOff) {
    if(uCategory >= LOG_MAX_CATEGORIES) return FALSE;
    BOOL o = LOG_abCategories[uCategory];
    LOG_abCategories[uCategory] = bOff;
    return o;
}
LPCTSTR LOG(UINT uCategory, UINT uLvl, LPCTSTR lpctszFmt, ...) {
    if(uCategory >= LOG_MAX_CATEGORIES) return NULL;
    if(LOG_abCategories[uCategory]) return NULL;
    if(uLvl < LOG_uLvl) return NULL;
    if(!LOG_bRun) LOG_rStart();

    TCHAR buf[LOG_MAX_MESAGELEN];
    DWORD _;
    LOG_bufOutSz = 0;
    va_list args;
    va_start (args, lpctszFmt);
    _vstprintf(buf, lpctszFmt, args);
    va_end (args);

    LPTSTR lpOut = LOG_bufOut;
    switch(uLvl) {

        case LOG_V: *lpOut = 'V'; break;
        case LOG_D: *lpOut = 'D'; break;
        case LOG_I: *lpOut = 'I'; break;
        case LOG_W: *lpOut = 'W'; break;
        case LOG_E: *lpOut = 'E'; break;
        case LOG_F: *lpOut = 'F'; break;
        default:    *lpOut = ' '; break;
    } ++lpOut; *lpOut = '/'; LOG_bufOutSz += 2; ++lpOut;

    if(LOG_alpctszCategories[uCategory]) {
        INT i = 0;
        for(;LOG_alpctszCategories[uCategory][i]; ++i) {
            *lpOut = LOG_alpctszCategories[uCategory][i]; ++lpOut; ++LOG_bufOutSz;
        }
        for(;i<LOG_MESAGETAB;++i){
            *lpOut = ' '; ++lpOut; ++LOG_bufOutSz;
        }
    } else {
        for(INT i=0;i<LOG_MESAGETAB;++i){
            *lpOut = ' '; ++lpOut; ++LOG_bufOutSz;
        }
    }
    *lpOut = ':'; ++lpOut; ++LOG_bufOutSz;
    *lpOut = ' '; ++lpOut; ++LOG_bufOutSz;

    for(LPCTSTR lpDst = buf; *lpDst; ++lpDst) {
        *lpOut = *lpDst; ++lpOut; ++LOG_bufOutSz;
        if(*lpDst == '\n') {
            switch(uLvl) {
                case LOG_V: *lpOut = 'V'; break;
                case LOG_D: *lpOut = 'D'; break;
                case LOG_I: *lpOut = 'I'; break;
                case LOG_W: *lpOut = 'W'; break;
                case LOG_E: *lpOut = 'E'; break;
                case LOG_F: *lpOut = 'F'; break;
                default:    *lpOut = ' '; break;
            } ++lpOut; *lpOut = '/'; LOG_bufOutSz += 2; ++lpOut;
            if(LOG_alpctszCategories[uCategory]) {
                INT i = 0;
                for(;LOG_alpctszCategories[uCategory][i]; ++i) {
                    *lpOut = LOG_alpctszCategories[uCategory][i]; ++lpOut; ++LOG_bufOutSz;
                }
                for(;i<LOG_MESAGETAB;++i){
                    *lpOut = ' '; ++lpOut; ++LOG_bufOutSz;
                }
            } else {
                for(INT i=0;i<LOG_MESAGETAB;++i){
                    *lpOut = ' '; ++lpOut; ++LOG_bufOutSz;
                }
            }
            *lpOut = ':'; ++lpOut; ++LOG_bufOutSz;
            *lpOut = ' '; ++lpOut; ++LOG_bufOutSz;
        }
    }
    *lpOut = '\n';  ++lpOut; ++LOG_bufOutSz;
    *lpOut = '\n';  ++lpOut; ++LOG_bufOutSz;
    *lpOut = 0;

    WriteFile(LOG_hFile, LOG_bufOut, LOG_bufOutSz*sizeof(TCHAR), &_, NULL);
    return LOG_bufOut;
}
HANDLE  LOG_rOtherStart(LPCTSTR lpctszFileName) {
    #ifdef _UNICODE
        HANDLE o = CreateFile(lpctszFileName, GENERIC_WRITE, FILE_SHARE_READ,
        NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        DWORD _;
        TCHAR buf[2];
        buf[0] = 0xFEFF;
        if(o != INVALID_HANDLE_VALUE)
            WriteFile(o, buf, 2, &_, NULL);
        return o;
    #else
        return CreateFile(lpctszFileName, GENERIC_WRITE, FILE_SHARE_READ,
        NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    #endif
}
VOID    LOG_rOtherFin(HANDLE hFile) {
    CloseHandle(hFile);
}
LPCTSTR LOG_rOtherLog(HANDLE hFile, UINT uCategory, UINT uLvl, LPCTSTR lpctszFmt, ...) {
    if(uCategory >= LOG_MAX_CATEGORIES) return NULL;
    if(LOG_abCategories[uCategory]) return NULL;
    if(uLvl < LOG_uLvl) return NULL;

    TCHAR buf[LOG_MAX_MESAGELEN];
    DWORD _;
    LOG_bufOutSz = 0;
    va_list args;
    va_start (args, lpctszFmt);
    _vstprintf(buf, lpctszFmt, args);
    va_end (args);

    LPTSTR lpOut = LOG_bufOut;
    switch(uLvl) {

        case LOG_V: *lpOut = 'V'; break;
        case LOG_D: *lpOut = 'D'; break;
        case LOG_I: *lpOut = 'I'; break;
        case LOG_W: *lpOut = 'W'; break;
        case LOG_E: *lpOut = 'E'; break;
        case LOG_F: *lpOut = 'F'; break;
        default:    *lpOut = ' '; break;
    } ++lpOut; *lpOut = '/'; LOG_bufOutSz += 2; ++lpOut;

    if(LOG_alpctszCategories[uCategory]) {
        INT i = 0;
        for(;LOG_alpctszCategories[uCategory][i]; ++i) {
            *lpOut = LOG_alpctszCategories[uCategory][i]; ++lpOut; ++LOG_bufOutSz;
        }
        for(;i<LOG_MESAGETAB;++i){
            *lpOut = ' '; ++lpOut; ++LOG_bufOutSz;
        }
    } else {
        for(INT i=0;i<LOG_MESAGETAB;++i){
            *lpOut = ' '; ++lpOut; ++LOG_bufOutSz;
        }
    }
    *lpOut = ':'; ++lpOut; ++LOG_bufOutSz;
    *lpOut = ' '; ++lpOut; ++LOG_bufOutSz;

    for(LPCTSTR lpDst = buf; *lpDst; ++lpDst) {
        *lpOut = *lpDst; ++lpOut; ++LOG_bufOutSz;
        if(*lpDst == '\n') {
            switch(uLvl) {
                case LOG_V: *lpOut = 'V'; break;
                case LOG_D: *lpOut = 'D'; break;
                case LOG_I: *lpOut = 'I'; break;
                case LOG_W: *lpOut = 'W'; break;
                case LOG_E: *lpOut = 'E'; break;
                case LOG_F: *lpOut = 'F'; break;
                default:    *lpOut = ' '; break;
            } ++lpOut; *lpOut = '/'; LOG_bufOutSz += 2; ++lpOut;
            if(LOG_alpctszCategories[uCategory]) {
                INT i = 0;
                for(;LOG_alpctszCategories[uCategory][i]; ++i) {
                    *lpOut = LOG_alpctszCategories[uCategory][i]; ++lpOut; ++LOG_bufOutSz;
                }
                for(;i<LOG_MESAGETAB;++i){
                    *lpOut = ' '; ++lpOut; ++LOG_bufOutSz;
                }
            } else {
                for(INT i=0;i<LOG_MESAGETAB;++i){
                    *lpOut = ' '; ++lpOut; ++LOG_bufOutSz;
                }
            }
            *lpOut = ':'; ++lpOut; ++LOG_bufOutSz;
            *lpOut = ' '; ++lpOut; ++LOG_bufOutSz;
        }
    }
    *lpOut = '\n';  ++lpOut; ++LOG_bufOutSz;
    *lpOut = '\n';  ++lpOut; ++LOG_bufOutSz;
    *lpOut = 0;

    if(hFile) WriteFile(hFile, LOG_bufOut, LOG_bufOutSz*sizeof(TCHAR), &_, NULL);
    return LOG_bufOut;
}
LPCTSTR LOG_rGetBuf() {
    return LOG_bufOut;
}
UINT    LOG_rGetBufSize() {
    return LOG_bufOutSz;
}
