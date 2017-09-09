#ifndef _H_LOG
#define _H_LOG

#include <WinDef.h>
#include <Assert.h>
#define ASSERT(x) assert(x)

#define LOG_MAX_CATEGORIES       32
#define LOG_MAX_MESAGELEN        4096
#define LOG_MESAGETAB            8


extern UINT             g_uLogApp;
extern UINT             g_uLogOGL;

enum {
    LOG_V = 0,
    LOG_D,
    LOG_I,
    LOG_W,
    LOG_E,
    LOG_F,
    LOG_S,
};
#define LOG_VERBOSE     LOG_V
#define LOG_DEBUG       LOG_D
#define LOG_INFO        LOG_I
#define LOG_WARN        LOG_W
#define LOG_ERROR       LOG_E
#define LOG_FATAL       LOG_F
#define LOG_SILENT      LOG_S

VOID    LOG_rStart();
VOID    LOG_rFin();
UINT    LOG_rSetLvl(UINT uLvl);

UINT    LOG_rNewCategory(LPCTSTR lpszCategory);
LPCTSTR LOG_rGetCategoryStr(UINT uCategory);
LPCTSTR LOG_rSetCategoryStr(UINT uCategory, LPCTSTR lpszCategory);
BOOL    LOG_rSetCategory(UINT uCategory, BOOL bOff);

LPCTSTR LOG(UINT uCategory, UINT uLvl, LPCTSTR lpszFmt, ...);

HANDLE  LOG_rOtherStart(LPCTSTR lpszFileName);
VOID    LOG_rOtherFin(HANDLE hFile);
LPCTSTR LOG_rOtherLog(HANDLE hFile, UINT uCategory, UINT uLvl, LPCTSTR lpszFmt, ...);

LPCTSTR LOG_rGetBuf();
UINT    LOG_rGetBufSize();

#endif
