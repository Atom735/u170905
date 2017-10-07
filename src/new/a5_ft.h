#ifndef _H_A5_FT
#define _H_A5_FT 171007

#include "a5_types.h"

A5TYPEDEF(struct A5sFT_Lib,     sFT_Lib);
A5TYPEDEF(struct A5sFT_Cache,   sFT_Cache);
A5TYPEDEF(struct A5sFT_Glyph,   sFT_Glyph);

A5psFT_Lib          A5FT_LibCreate(A5uint uMaxFonts, A5uint uMaxSizes);
A5void              A5FT_LibFree(A5psFT_Lib pLib);

A5psFT_Cache        A5FT_CacheCreate(A5psFT_Lib pLib, A5uint uSize);
A5void              A5FT_CacheClear(A5psFT_Cache pCache);
A5void              A5FT_CacheFree(A5psFT_Cache pCache);

A5uint              A5FT_FontNewFromFile(A5psFT_Lib pLib, A5pcstr szFileName);
A5uint              A5FT_FontNewFromMem(A5psFT_Lib pLib, A5pvoid pMem, A5uint uMemSize);
A5void              A5FT_FontFree(A5psFT_Lib pLib, A5uint uFontID);

A5uint              A5FT_SizesNew(A5psFT_Lib pLib, A5uint uFontID, A5uint uFontSize);
A5void              A5FT_SizesSet(A5psFT_Lib pLib, A5uint uSizesID, A5uint uFontID, A5uint uFontSize);

A5psFT_Glyph        A5FT_GlyphNewCustom(A5psFT_Cache pCache, A5uint uBufWidth, A5uint uBufHeight, A5pvoid pBuf, A5uint uBufPitch);
A5psFT_Glyph        A5FT_GlyphNew(A5psFT_Cache pCache, A5uint uSizeID, A5uint uUnicode);
A5psFT_Glyph        A5FT_GlyphGet(A5psFT_Cache pCache, A5uint uSizeID, A5uint uUnicode);

struct A5sFT_Lib {
    A5uint          uObjectId;
    A5pvoid         pFtLib;
    A5uint          uMaxFonts;
    A5uint          uMaxSizes;
};

struct A5sFT_Cache {
    A5uint          uObjectId;
    A5psFT_Lib      pLib;
    A5uint          uSize;
    A5uint          uGlyphCount;
    A5uint          uPenX;
    A5uint          uPenY;
    A5uint          uLineHeight;
    A5uint          uFlagUpdate;
};

struct A5sFT_Glyph {
    A5uint          uObjectId;
    A5uint          uUnicode;
    A5uint          uSizeID;
    A5uint          uAdvance;
    A5uint          uCachePosX;
    A5uint          uCachePosY;
    A5uint          uCacheWidth;
    A5uint          uCacheHeight;
    A5int           iOffsetX;
    A5int           iOffsetY;
};


#endif /* _H_A5_FT */
