#ifndef _H_A5_FT
#define _H_A5_FT 170930

typedef void *A5S_FTLib;
typedef void *A5S_FTCache;

typedef struct tagA5S_Glyph {
    /* ID Глифа */
    unsigned int    iGlyphID;
    /* Номер символа в юникоде */
    unsigned int    iUnicode;
    /* Номер используемого шрифта */
    unsigned int    iFontID;
    /* Высота глифа */
    unsigned int    iHeight;
    /* Ширина глифа */
    unsigned int    iAdvance;
    /* Позиция в текстуре */
    unsigned int    iTexX, iTexY;
    /* Размеры в текстуре */
    unsigned int    iTexW, iTexH;
    /* Отступы для текстуры */
    int             iOffX, iOffY;
} A5S_Glyph, *pA5S_Glyph;

typedef struct tagA5S_TextSettings {
    /* Указатель на UTF-8 строку */
    char           *szText;
    /* Указатель на UTF-32 строку */
    unsigned int   *szText32;
    /* Указатель на кеш */
    A5S_FTCache     pCache;
    /* Номер используемого шрифта */
    unsigned int    iFontID;
    /* Высота глифа */
    unsigned int    iHeight;
    /* Максимальная иширна строки */
    unsigned int    iWidth;
    /* Высота строки */
    int             iLineHeight;
    /* кёрнинг */
    int             iKerning;
    /* ширина табуляция */
    int             iTab;
    /* Позиция в буффере */
    int             iX, iY;
    /* Цвета в буффере */
    unsigned char   R, G, B, A;
} A5S_TextSettings, *pA5S_TextSettings;



A5S_FTLib   A5FT_Init();
void        A5FT_Free(A5S_FTLib pLib);

A5S_FTCache     A5FT_CacheInit(A5S_FTLib pLib, unsigned int uSize, unsigned int uMaxFonts);
void            A5FT_CacheFree(A5S_FTCache pCache);
void            A5FT_CacheClear(A5S_FTCache pCache);
unsigned int    A5FT_CacheUpdate(A5S_FTCache pCache, unsigned int iUpdate);
unsigned int    A5FT_CacheGetSize(A5S_FTCache pCache);

unsigned int    A5FT_FontNewFromFile(A5S_FTCache pCache, const char *szFileName);
unsigned int    A5FT_FontNewFromMem(A5S_FTCache pCache, void *pMem, unsigned int uSize);
void            A5FT_FontDel(A5S_FTCache pCache, unsigned int iFontID);

void            A5FT_GlyphClear(A5S_FTCache pCache);
pA5S_Glyph      A5FT_GlyphNew(A5S_FTCache pCache, unsigned int iFontID, unsigned int iGlyphID, unsigned int iHeight);
pA5S_Glyph      A5FT_GlyphGet(A5S_FTCache pCache, unsigned int iFontID, unsigned int iGlyphID, unsigned int iHeight);
pA5S_Glyph      A5FT_CharGet(A5S_FTCache pCache, unsigned int iFontID, unsigned int iUnicode, unsigned int iHeight);


#endif
