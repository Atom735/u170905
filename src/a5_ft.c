#include "a5_ft.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include "a5_ut.h"

#define FT_LOG_CATEGORY         SDL_LOG_CATEGORY_APPLICATION
#define FT_LOG_Debug(a, ...)    SDL_LogDebug(FT_LOG_CATEGORY, "%24s: " a, __func__, __VA_ARGS__)
#define FT_LOG_Error(a, ...)    SDL_LogError(FT_LOG_CATEGORY, "%24s: " a, __func__, __VA_ARGS__)

pA5FT_Lib       A5FT_Init() {
    FT_Library pLib;
    FT_Init_FreeType(&pLib);
    if(pLib)    FT_LOG_Debug("new [FT_Lib = %p]", pLib);
    else        FT_LOG_Error("%s", "can't create [FT_Lib]");
    return pLib;
}
FTvoid          A5FT_Free(pA5FT_Lib pLib) {
    if(!pLib) {
        FT_LOG_Error("%s", "can't delete [FT_Lib = NULL]");
        return;
    }
    FT_LOG_Debug("del [FT_Lib = %p]", pLib);
    FT_Done_FreeType(pLib);
}

pA5FT_Cache     A5FT_CacheInit(pA5FT_Lib pLib, FTuint uSize, FTuint uMaxFonts){
    if(!pLib) {
        FT_LOG_Error("%s", "can't create [FT_Cache] with [FT_Lib = NULL]");
        return NULL;
    }
    if(!uSize) {
        FT_LOG_Error("%s", "can't create [FT_Cache] with [uSize = 0]");
        return NULL;
    }
    if((uSize*uSize) < (sizeof(struct tagA5S_FT_Cache)+(sizeof(FT_Face)*(uMaxFonts+1)))) {
        FT_LOG_Error("can't create [FT_Cache] because [uSize = %d] is very small, i need more than [%d] bytes of space, but you give me [%d] bytes", uSize, (sizeof(struct tagA5S_FT_Cache)+sizeof(FT_Face)*(uMaxFonts+1)), uSize*uSize);
        return NULL;
    }
    pA5FT_Cache pCache = (pA5FT_Cache)SDL_malloc(uSize*uSize);
    if(!pCache) {
        FT_LOG_Error("can't create [FT_Cache] ([FT_Lib = %p] [uSize = %d])", pLib, uSize);
        return NULL;
    }
    SDL_memset(pCache, 0, uSize*uSize);
    pCache->pLib        = pLib;
    pCache->uCacheSize  = uSize;
    pCache->uMaxFonts   = uMaxFonts;
    pCache->uPenY       = uSize;
    pCache->uFlagUpdate = 1;
    FT_LOG_Debug("new [FT_Cache = %p (%d,%d)] by [FT_Lib = %p]", pCache, pCache->uCacheSize, pCache->uMaxFonts, pCache->pLib);
    return pCache;
}
FTvoid          A5FT_CacheFree(pA5FT_Cache pCache) {
    if(!pCache) {
        FT_LOG_Error("%s", "can't del [FT_Cache = NULL]");
        return;
    }
    FT_LOG_Debug("del [FT_Cache = %p (%d,%d)] by [FT_Lib = %p]", pCache, pCache->uCacheSize, pCache->uMaxFonts, pCache->pLib);
    A5FT_CacheClear(pCache);
    SDL_free(pCache);
}
FTvoid          A5FT_CacheClear(pA5FT_Cache pCache) {
    if(!pCache) {
        FT_LOG_Error("%s", "can't clr [FT_Cache = NULL]");
        return;
    }
    FT_LOG_Debug("clr [FT_Cache = %p (%d,%d)] by [FT_Lib = %p]", pCache, pCache->uCacheSize, pCache->uMaxFonts, pCache->pLib);
    FT_Face *pFace = (FT_Face*)(pCache+1);
    for(FTuint i=1; i<=pCache->uMaxFonts; ++i)
        if(pFace[i])
            A5FT_FontDelete(pCache, i);
    // SDL_memset(pCache+1, 0, (pCache->uCacheSize*pCache->uCacheSize)-sizeof(struct tagA5S_FT_Cache));
    pCache->uGlyphCount = 0;
    pCache->uPenX       = 0;
    pCache->uPenY       = pCache->uCacheSize;
    pCache->uLineHeight = 0;
    pCache->uFlagUpdate = 1;
    pCache->uFlagOptimize=0;
}
FTvoid          A5FT_CacheOptimize(pA5FT_Cache pCache, FTuint uFlagOptimize) {
    if(!pCache) {
        FT_LOG_Error("%s", "can't opz [FT_Cache = NULL]");
        return;
    }
    FT_LOG_Debug("opz [FT_Cache = %p (%d,%d)] by [FT_Lib = %p]", pCache, pCache->uCacheSize, pCache->uMaxFonts, pCache->pLib);
    // TODO
    FT_LOG_Error("%s", "TODO");
}
FTpvoid         A5FT_CacheDebugGet(pA5FT_Cache pCache, FTuint uFlagDebug, FTpvoid pBuf) {
    if(!pCache) {
        FT_LOG_Error("%s", "can't get debug by [FT_Cache = NULL]");
        return NULL;
    }
    if(!pBuf) {
        FT_LOG_Error("can't get debug by [FT_Cache = %p (%d,%d)] to [pBuf = NULL]", pCache, pCache->uCacheSize, pCache->uMaxFonts);
        return NULL;
    }
    // TODO
    FT_LOG_Error("%s", "TODO");
    return NULL;
}
FTuint          A5FT_FontNewFromFile(pA5FT_Cache pCache, FTcstr szFileName) {
    if(!pCache) {
        FT_LOG_Error("%s", "can't create font in [FT_Cache = NULL]");
        return 0;
    }
    if(!szFileName) {
        FT_LOG_Error("can't create font in [FT_Cache = %p (%d,%d)] by [szFileName = NULL]", pCache, pCache->uCacheSize, pCache->uMaxFonts);
        return 0;
    }

    FTuint  _uSize = 0;
    FTpvoid _pBuf = NULL;
    _pBuf = A5UT_FileRead(szFileName, &_uSize);
    FT_LOG_Debug("create font in [FT_Cache = %p (%d,%d)] by [szFileName = \"%s\"] [_pBuf = %p (%d)]", pCache, pCache->uCacheSize, pCache->uMaxFonts, szFileName, _pBuf, _uSize);
    FTuint _uFontID = A5FT_FontNewFromMem(pCache, _pBuf, _uSize);
    if(_pBuf) SDL_free(_pBuf);
    return _uFontID;
}
FTuint          A5FT_FontNewFromMem(pA5FT_Cache pCache, FTpvoid pMem, FTuint uSize) {
    if(!pCache) {
        FT_LOG_Error("%s", "can't create font in [FT_Cache = NULL]");
        return 0;
    }
    if(!pMem) {
        FT_LOG_Error("can't create font in [FT_Cache = %p (%d,%d)] by [pMem = NULL]", pCache, pCache->uCacheSize, pCache->uMaxFonts);
        return 0;
    }
    if(!uSize) {
        FT_LOG_Error("can't create font in [FT_Cache = %p (%d,%d)] by [pMem = %p] with [uSize = 0]", pCache, pCache->uCacheSize, pCache->uMaxFonts, pMem);
        return 0;
    }
    if(!pCache->pLib) {
        FT_LOG_Error("can't create font in [FT_Cache = %p (%d,%d)] with [FT_Lib = NULL]", pCache, pCache->uCacheSize, pCache->uMaxFonts);
        return 0;
    }
    FT_Face *pFace = (FT_Face*)(pCache+1);
    for(FTuint i=1; i<=pCache->uMaxFonts; ++i) {
        if(!pFace[i]) {
            FT_Error fterror = FT_New_Memory_Face((FT_Library)pCache->pLib, pMem, uSize, 0, pFace+i);
            if(fterror) {
                FT_LOG_Error("can't create font in [FT_Cache = %p (%d,%d)] with freetype problem (%d)", pCache, pCache->uCacheSize, pCache->uMaxFonts, fterror);
                return 0;
            } else {
                FT_LOG_Debug("new [FT_Face = %p (%d/%d)] in [FT_Cache = %p (%d,%d)]", pFace[i], i, pCache->uMaxFonts, pCache, pCache->uCacheSize, pCache->uMaxFonts);
                FT_LOG_Debug("FaceInfo : %p (%u)\n\tnum_faces: %li\n\tface_index: %li\n\tnum_glyphs: %li\n\tfamily_name: %s\n\tstyle_name: %s\n\tnum_fixed_sizes: %i\n\tnum_charmaps: %i\n\tunits_per_EM: %hu\n\theight: %i",
                    pMem, uSize,
                    pFace[i]->num_faces,
                    pFace[i]->face_index,
                    pFace[i]->num_glyphs,
                    pFace[i]->family_name,
                    pFace[i]->style_name,
                    pFace[i]->num_fixed_sizes,
                    pFace[i]->num_charmaps,
                    pFace[i]->units_per_EM,
                    pFace[i]->height);
                return i;
            }
        }
    }
    FT_LOG_Error("can't create font in [FT_Cache = %p (%d,%d)] because haven't space", pCache, pCache->uCacheSize, pCache->uMaxFonts);
    return 0;
}
FTvoid          A5FT_FontDelete(pA5FT_Cache pCache, FTuint uFontID) {
    if(!pCache) {
        FT_LOG_Error("%s", "can't delete font in [FT_Cache = NULL]");
        return;
    }
    if((!uFontID) || (uFontID > pCache->uMaxFonts)) {
        FT_LOG_Error("can't delete font in [FT_Cache = %p (%d,%d)] because [uFontID = %d] invalidate", pCache, pCache->uCacheSize, pCache->uMaxFonts, uFontID);
        return;
    }
    FT_Face *pFace = (FT_Face*)(pCache+1);
    if(!pFace[uFontID]) {
        FT_LOG_Error("can't delete font in [FT_Cache = %p (%d,%d)] because [uFontID = %d] is empty", pCache, pCache->uCacheSize, pCache->uMaxFonts, uFontID);
        return;
    }
    FT_LOG_Debug("del [FT_Face = %p (%d/%d)] in [FT_Cache = %p (%d,%d)]", pFace[uFontID], uFontID, pCache->uMaxFonts, pCache, pCache->uCacheSize, pCache->uMaxFonts);
    FT_Done_Face(pFace[uFontID]);
    pFace[uFontID] = NULL;
}
FTvoid          A5FT_GlyphClear(pA5FT_Cache pCache) {
    if(!pCache) {
        FT_LOG_Error("%s", "[FT_Cache = NULL]");
        return;
    }
    FT_LOG_Debug("[FT_Cache = %p (%d,%d)]", pCache, pCache->uCacheSize, pCache->uMaxFonts);
    pCache->uGlyphCount = 0;
    pCache->uPenX       = 0;
    pCache->uPenY       = pCache->uCacheSize;
    pCache->uLineHeight = 0;
    pCache->uFlagUpdate = 1;
    pCache->uFlagOptimize=0;
}
pA5FT_Glyph     A5FT_GlyphNewCustom(pA5FT_Cache pCache, FTuint uBufWidth, FTuint uBufHeight, FTpvoid pBuf, FTuint uBufPitch) {
    if(!pCache) {
        FT_LOG_Error("%s", "[FT_Cache = NULL]");
        return NULL;
    }

    /* Если не можем впихнуть в эту линию */
    if(pCache->uPenX + uBufWidth >= pCache->uCacheSize) {
        pCache->uPenX = 0;
        /* Если не можем перейти на новую линию */
        if(pCache->uPenY < pCache->uLineHeight) {
            A5FT_GlyphClear(pCache);
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
    if(pCache->uPenY <=
        (sizeof(struct tagA5S_FT_Cache)+
            sizeof(FT_Face)*(pCache->uMaxFonts+1)+
            sizeof(struct tagA5S_FT_Glyph)*(pCache->uGlyphCount+1)
            +1
        )/pCache->uCacheSize+1+pCache->uLineHeight) {
        A5FT_GlyphClear(pCache);
        return A5FT_GlyphNewCustom(pCache, uBufWidth, uBufHeight, pBuf, uBufPitch);
    }
    Uint8 *p = (Uint8*)pCache + pCache->uPenX + (pCache->uPenY-uBufHeight)*pCache->uCacheSize;
    for(FTuint uY = 0; uY < uBufHeight; ++uY)
        SDL_memcpy(p+(uY*pCache->uCacheSize), (Uint8*)pBuf+(uY*uBufPitch), uBufWidth);
    ++pCache->uGlyphCount;
    pA5FT_Glyph pa5g = (pA5FT_Glyph)((FT_Face*)(pCache+1)+pCache->uMaxFonts+1)+pCache->uGlyphCount;
    SDL_memset(pa5g, 0, sizeof(struct tagA5S_FT_Glyph));
    pa5g->uCachePosX    = pCache->uPenX;
    pa5g->uCachePosY    = pCache->uPenY-uBufHeight;
    pa5g->uCacheWidth   = uBufWidth;
    pa5g->uCacheHeight  = uBufHeight;
    pCache->uPenX      += uBufWidth;
    pCache->uFlagUpdate = 1;
    pCache->uFlagOptimize=0;
    return pa5g;
}
pA5FT_Glyph     A5FT_GlyphNew(pA5FT_Cache pCache, FTuint uFontID, FTuint uHeight, FTuint uGlyphID) {
    if(!pCache) {
        FT_LOG_Error("%s", "[FT_Cache = NULL]");
        return NULL;
    }
    if(!uHeight) {
        FT_LOG_Error("%s", "[uHeight = 0]");
        return NULL;
    }
    if((!uFontID) || (uFontID > pCache->uMaxFonts)) {
        FT_LOG_Error("[uFontID = %d/%d]", uFontID, pCache->uMaxFonts);
        return NULL;
    }
    FT_Face *pFace = (FT_Face*)(pCache+1);
    if(!pFace[uFontID]) {
        FT_LOG_Error("[uFontID = %d/%d] [FT_Face = NULL]", uFontID, pCache->uMaxFonts);
        return NULL;
    }

    FT_Set_Pixel_Sizes(pFace[uFontID], 0, uHeight);
    FT_Load_Glyph(pFace[uFontID], uGlyphID, FT_LOAD_DEFAULT);
    FT_GlyphSlot pG = pFace[uFontID]->glyph;
    FT_Render_Glyph(pG, FT_RENDER_MODE_NORMAL);

    pA5FT_Glyph pa5g = A5FT_GlyphNewCustom(pCache, pG->bitmap.width, pG->bitmap.rows, pG->bitmap.buffer, pG->bitmap.pitch);
    if(!pa5g) return NULL;

    pa5g->uFontID       = uFontID;
    pa5g->uHeight       = uHeight;
    pa5g->uGlyphID      = uGlyphID;
    pa5g->uAdvance      = pG->advance.x >> 6;
    pa5g->iTexOffsetX   = pG->bitmap_left;
    pa5g->iTexOffsetY   =-pG->bitmap_top;

    return pa5g;
}
pA5FT_Glyph     A5FT_GlyphNewUnicode(pA5FT_Cache pCache, FTuint uFontID, FTuint uHeight, FTuint uUnicode) {
    if(!pCache) {
        FT_LOG_Error("%s", "[FT_Cache = NULL]");
        return NULL;
    }
    if(!uHeight) {
        FT_LOG_Error("%s", "[uHeight = 0]");
        return NULL;
    }
    if((!uFontID) || (uFontID > pCache->uMaxFonts)) {
        FT_LOG_Error("[uFontID = %d/%d]", uFontID, pCache->uMaxFonts);
        return NULL;
    }
    FT_Face *pFace = (FT_Face*)(pCache+1);
    if(!pFace[uFontID]) {
        FT_LOG_Error("[uFontID = %d/%d] [FT_Face = NULL]", uFontID, pCache->uMaxFonts);
        return NULL;
    }
    pA5FT_Glyph pa5g = A5FT_GlyphNew(pCache, uFontID, uHeight, FT_Get_Char_Index(pFace[uFontID], uUnicode));
    if(pa5g) pa5g->uUnicode = uUnicode;
    return pa5g;
}
pA5FT_Glyph     A5FT_GlyphGetCustom(pA5FT_Cache pCache, FTuint uGlyphID) {
    if(!pCache) {
        FT_LOG_Error("%s", "[FT_Cache = NULL]");
        return NULL;
    }
    pA5FT_Glyph pa5g = (pA5FT_Glyph)((FT_Face*)(pCache+1)+pCache->uMaxFonts+1);
    /* Если без оптимизации */
    if(!pCache->uFlagOptimize) {
        for (FTuint i = 0; i < pCache->uGlyphCount; ++i)
            if((!pa5g[i].uFontID) &&
                (pa5g[i].uGlyphID == uGlyphID))
                return pa5g+i;
    }
    // TODO
    return NULL;
}
pA5FT_Glyph     A5FT_GlyphGet(pA5FT_Cache pCache, FTuint uFontID, FTuint uHeight, FTuint uGlyphID) {
    if(!pCache) {
        FT_LOG_Error("%s", "[FT_Cache = NULL]");
        return NULL;
    }
    pA5FT_Glyph pa5g = (pA5FT_Glyph)((FT_Face*)(pCache+1)+pCache->uMaxFonts+1);
    /* Если без оптимизации */
    if(!pCache->uFlagOptimize) {
        for (FTuint i = 0; i < pCache->uGlyphCount; ++i)
            if( (pa5g[i].uFontID  == uFontID) &&
                (pa5g[i].uHeight  == uHeight) &&
                (pa5g[i].uGlyphID == uGlyphID))
                return pa5g+i;
    }
    // TODO
    return NULL;
}
pA5FT_Glyph     A5FT_GlyphGetUnicode(pA5FT_Cache pCache, FTuint uFontID, FTuint uHeight, FTuint uUnicode) {
        if(!pCache) {
        FT_LOG_Error("%s", "[FT_Cache = NULL]");
        return NULL;
    }
    pA5FT_Glyph pa5g = (pA5FT_Glyph)((FT_Face*)(pCache+1)+pCache->uMaxFonts+1);
    /* Если без оптимизации */
    if(!pCache->uFlagOptimize) {
        for (FTuint i = 0; i < pCache->uGlyphCount; ++i)
            if( (pa5g[i].uFontID  == uFontID) &&
                (pa5g[i].uHeight  == uHeight) &&
                (pa5g[i].uUnicode == uUnicode))
                return pa5g+i;
    }
    // TODO
    return NULL;
}
/*

pA5S_Glyph      A5FT_GlyphGet(A5S_FTCache pCache, unsigned int iFontID, unsigned int iGlyphID, unsigned int iHeight) {
    pA5S_FtCacheHead pHead = (pA5S_FtCacheHead)pCache;
    if(iFontID >= pHead->uMaxFonts) return NULL;
    FT_Face *pFace = (FT_Face*)(pHead+1);
    if(!pFace[iFontID]) return NULL;

    pA5S_Glyph pa5g = (pA5S_Glyph)(pFace+pHead->uMaxFonts);
    for(unsigned int i=0; i<pHead->uGlyphCount; ++i)
        if((pa5g[i].iFontID == iFontID)&&(pa5g[i].iHeight == iHeight)&&(pa5g[i].iGlyphID == iGlyphID)) return pa5g+i;
    return A5FT_GlyphNew(pCache, iFontID, iGlyphID, iHeight);
}

pA5S_Glyph      A5FT_CharGet(A5S_FTCache pCache, unsigned int iFontID, unsigned int iUnicode, unsigned int iHeight) {
    pA5S_FtCacheHead pHead = (pA5S_FtCacheHead)pCache;
    if(iFontID >= pHead->uMaxFonts) return NULL;
    FT_Face *pFace = (FT_Face*)(pHead+1);
    if(!pFace[iFontID]) return NULL;
    pA5S_Glyph pa5g = A5FT_GlyphGet(pCache, iFontID, FT_Get_Char_Index(pFace[iFontID], iUnicode), iHeight);
    if(pa5g) pa5g->iUnicode = iUnicode;
    return pa5g;
}

*/
