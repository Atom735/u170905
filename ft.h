#ifndef _H_FT
#define _H_FT

#include "log.h"
#include "sm.h"

extern PBYTE           FT_A8CACHE;
extern UINT            FT_A8CACHE_SIZE;
extern BOOL            FT_A8CACHE_UPDATE;

typedef struct __FT_GLYPH {
    WORD G, U; // Номер глифа, Unicode
    WORD x, y; // Позиция в текстуре
    CHAR X, Y; // Отступы текстуры при рисовке
    BYTE w, h; // Размеры в текстуре
    BYTE H, F, A, _; // Высота, фонт, отступ след символа, ??? хз чё, не придумал ещё
} FT_GLYPH, *PFT_GLYPH, *LPFT_GLYPH;


INT         FT_I(LPSM lpsm, UINT uMsg, LPVOID ptr);

VOID        FT_rClean();
UINT        FT_rNewFaceA(LPCSTR fName);
UINT        FT_rNewFaceFromMem(LPBYTE pMem, INT size);

LPFT_GLYPH  FT_rNewChar(UINT iFace, UINT iUChar, UINT size);
LPFT_GLYPH  FT_rGetChar(UINT iFace, UINT iUChar, UINT size);

LPFT_GLYPH  FT_rNewGlyph(UINT iFace, UINT iGlyph, UINT size);
LPFT_GLYPH  FT_rGetGlyph(UINT iFace, UINT iGlyph, UINT size);


#endif
