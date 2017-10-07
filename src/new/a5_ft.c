#include "a5_ft.h"
#include "a5_ut.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include "a5_ut.h"

A5TYPEDEF(struct A5sFT_Sizes,     sFT_Sizes);

struct A5sFT_Sizes {
    A5uint          uFontID;
    A5uint          uFontSize;
};

A5psFT_Lib          A5FT_LibCreate(A5uint uMaxFonts, A5uint uMaxSizes) {
    A5psFT_Lib pLib     = (A5psFT_Lib)SDL_malloc(sizeof(A5sFT_Lib)+sizeof(FT_Face)*uMaxFonts+sizeof(A5sFT_Sizes)*uMaxSizes);
    SDL_memset(pLib, 0, sizeof(A5sFT_Lib)+sizeof(FT_Face)*uMaxFonts+sizeof(A5sFT_Sizes)*uMaxSizes);
    pLib->uObjectId     = A5OID_FT_LIB;
    pLib->uMaxFonts     = uMaxFonts;
    pLib->uMaxSizes     = uMaxSizes;
    FT_Init_FreeType((FT_Library*)&pLib->pFtLib);
    return pLib;
}
A5void              A5FT_LibFree(A5psFT_Lib pLib) {
    FT_Done_FreeType((FT_Library)pLib->pFtLib);
    SDL_free(pLib);
}
A5psFT_Cache        A5FT_CacheCreate(A5psFT_Lib pLib, A5uint uSize) {
    A5psFT_Cache pCache = (A5psFT_Cache)SDL_malloc(uSize*uSize);
    SDL_memset(pCache, 0, uSize*uSize);
    pCache->uObjectId   = A5OID_FT_CACHE;
    pCache->pLib        = pLib;
    pCache->uSize       = uSize;
    pCache->uPenY       = uSize;
    pCache->uFlagUpdate = 1;
    return pCache;
}
A5void              A5FT_CacheClear(A5psFT_Cache pCache) {
    pCache->uGlyphCount = 0;
    pCache->uPenX       = 0;
    pCache->uPenY       = pCache->uSize;
    pCache->uLineHeight = 0;
    pCache->uFlagUpdate = 1;
}
A5void              A5FT_CacheFree(A5psFT_Cache pCache) {
    SDL_free(pCache);
}
A5uint              A5FT_FontNewFromFile(A5psFT_Lib pLib, A5pcstr szFileName) {
    A5pvoid pBuf    = NULL;
    A5uint  uSize   = 0;
    pBuf = A5UT_FileRead(szFileName, &uSize);
    A5uint uFontID = A5FT_FontNewFromMem(pLib, pBuf, uSize);
    if(pBuf) A5UT_FileReadFree(pBuf);
    return uFontID;
}
A5uint              A5FT_FontNewFromMem(A5psFT_Lib pLib, A5pvoid pMem, A5uint uMemSize) {
    FT_Face *pFace = (FT_Face*)(pLib+1);
    for(A5uint i=0; i<pLib->uMaxFonts; ++i) {
        if(!pFace[i]) {
            FT_Error fterror = FT_New_Memory_Face((FT_Library)pLib->pFtLib, pMem, uMemSize, 0, pFace+i);
            if(fterror) {
                SDL_Log("Can't create font FT_Error = (%d)", fterror);
                return 0;
            } else {
                SDL_Log("new [FT_Face = %p (%d/%d)]", pFace[i], i, pLib->uMaxFonts);
                SDL_Log("FaceInfo : %p (%u)\n\tnum_faces: %li\n\tface_index: %li\n\tnum_glyphs: %li\n\tfamily_name: %s\n\tstyle_name: %s\n\tnum_fixed_sizes: %i\n\tnum_charmaps: %i\n\tunits_per_EM: %hu\n\theight: %i",
                    pMem, uMemSize,
                    pFace[i]->num_faces,
                    pFace[i]->face_index,
                    pFace[i]->num_glyphs,
                    pFace[i]->family_name,
                    pFace[i]->style_name,
                    pFace[i]->num_fixed_sizes,
                    pFace[i]->num_charmaps,
                    pFace[i]->units_per_EM,
                    pFace[i]->height);
                return i+1;
            }
        }
    }
    return 0;
}
A5void              A5FT_FontFree(A5psFT_Lib pLib, A5uint uFontID) {
    FT_Face *pFace = (FT_Face*)(pLib+1);
    FT_Done_Face(pFace[uFontID]);
    pFace[uFontID] = NULL;
}
A5uint              A5FT_SizesNew(A5psFT_Lib pLib, A5uint uFontID, A5uint uFontSize) {
    FT_Face *pFace = (FT_Face*)(pLib+1);
    A5psFT_Sizes pSizes = (A5psFT_Sizes)(pFace+pLib->uMaxFonts);
    for(A5uint i=0; i<pLib->uMaxSizes; ++i) {
        if(!pSizes[i].uFontID) {
            pSizes[i].uFontID   = uFontID;
            pSizes[i].uFontSize = uFontSize;
            return i+1;
        }
    }
    return 0;
}
A5void              A5FT_SizesSet(A5psFT_Lib pLib, A5uint uSizesID, A5uint uFontID, A5uint uFontSize) {
    FT_Face *pFace = (FT_Face*)(pLib+1);
    A5psFT_Sizes pSizes = (A5psFT_Sizes)(pFace+pLib->uMaxFonts);
    pSizes[uSizesID-1].uFontID   = uFontID;
    pSizes[uSizesID-1].uFontSize = uFontSize;
}
A5psFT_Glyph        A5FT_GlyphNewCustom(A5psFT_Cache pCache, A5uint uBufWidth, A5uint uBufHeight, A5pvoid pBuf, A5uint uBufPitch) {
    /* Если не можем впихнуть в эту линию */
    if(pCache->uPenX + uBufWidth > pCache->uSize) {
        pCache->uPenX = 0;
        /* Если не можем перейти на новую линию */
        if(pCache->uPenY < pCache->uLineHeight) {
            A5FT_CacheClear(pCache);
            return A5FT_GlyphNewCustom(pCache, uBufWidth, uBufHeight, pBuf, uBufPitch);
        }
        pCache->uPenY -= pCache->uLineHeight;
        pCache->uLineHeight = 0;
    }
    /* Если этот глиф больше то делаем линию под размер глифа */
    if(pCache->uLineHeight < uBufHeight) {
        pCache->uLineHeight = uBufHeight;
    }
    /* Если этот глиф не входит в кеш без нарушения целостности */
    if(pCache->uPenY <= (sizeof(A5sFT_Cache)+sizeof(A5sFT_Glyph)*(pCache->uGlyphCount+1)+1
        )/pCache->uSize+1+pCache->uLineHeight) {
        A5FT_CacheClear(pCache);
        return A5FT_GlyphNewCustom(pCache, uBufWidth, uBufHeight, pBuf, uBufPitch);
    }
    A5puint8 p = (A5puint8)pCache + pCache->uPenX + (pCache->uPenY-uBufHeight)*pCache->uSize;
    for(A5uint uY = 0; uY < uBufHeight; ++uY)
        SDL_memcpy(p+(uY*pCache->uSize), (A5puint8)pBuf+(uY*uBufPitch), uBufWidth);
    ++pCache->uGlyphCount;
    A5psFT_Glyph pa5g = (A5psFT_Glyph)(pCache+1)+pCache->uGlyphCount;
    SDL_memset(pa5g, 0, sizeof(A5sFT_Glyph));
    pa5g->uObjectId     = A5OID_FT_GLYPH;
    pa5g->uCachePosX    = pCache->uPenX;
    pa5g->uCachePosY    = pCache->uPenY-uBufHeight;
    pa5g->uCacheWidth   = uBufWidth;
    pa5g->uCacheHeight  = uBufHeight;
    pCache->uPenX      += uBufWidth;
    pCache->uFlagUpdate = 1;
    return pa5g;
}

A5psFT_Glyph        A5FT_GlyphNew(A5psFT_Cache pCache, A5uint uSizeID, A5uint uUnicode) {
    FT_Face *pFace = (FT_Face*)(pCache->pLib+1);
    A5psFT_Sizes pSizes = (A5psFT_Sizes)(pFace+pCache->pLib->uMaxFonts);
    FT_Face pTFace = pFace[pSizes[uSizeID-1].uFontID-1];
    A5uint uGlyphID = FT_Get_Char_Index(pTFace, uUnicode);
    FT_Set_Pixel_Sizes(pTFace, 0, pSizes[uSizeID-1].uFontSize);
    FT_Load_Glyph(pTFace, uGlyphID, FT_LOAD_DEFAULT);
    FT_GlyphSlot pG = pTFace->glyph;
    FT_Render_Glyph(pG, FT_RENDER_MODE_NORMAL);
    A5psFT_Glyph pa5g = A5FT_GlyphNewCustom(pCache, pG->bitmap.width, pG->bitmap.rows, pG->bitmap.buffer, pG->bitmap.pitch);
    if(!pa5g) return NULL;
    pa5g->uUnicode      = uUnicode;
    pa5g->uSizeID       = uSizeID;
    pa5g->uAdvance      = pG->advance.x >> 6;
    pa5g->iOffsetX      = pG->bitmap_left;
    pa5g->iOffsetY      =-pG->bitmap_top;
    return pa5g;
}
A5psFT_Glyph        A5FT_GlyphGet(A5psFT_Cache pCache, A5uint uSizeID, A5uint uUnicode) {
    A5psFT_Glyph pa5g = (A5psFT_Glyph)(pCache+1);
    for (A5uint i = 0; i < pCache->uGlyphCount; ++i)
            if((pa5g[i].uSizeID == uSizeID) && (pa5g[i].uUnicode == uUnicode))
                return pa5g+i;
    return NULL;
}
