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
    if(pLib)    FT_LOG_Debug("FT_Lib = %p", pLib);
    else        FT_LOG_Error("%s", "can't create FT_Lib");
    return pLib;
}

FTvoid          A5FT_Free(pA5FT_Lib pLib) {
    FT_LOG_Debug("FT_Lib = %p", pLib);
    FT_Done_FreeType(pLib);
}

/*

A5S_FTLib   A5FT_Init() {
    FT_Library pLib;
    FT_Init_FreeType(&pLib);
    if(pLib) SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "FT New      : FT_Lib = %p", pLib);
    else SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "can't create FT_Lib");
    return pLib;
}
void        A5FT_Free(A5S_FTLib pLib) {
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "FT Del      : FT_Lib = %p", pLib);
    FT_Done_FreeType(pLib);
}

A5S_FTCache     A5FT_CacheInit(A5S_FTLib pLib, unsigned int uSize, unsigned int uMaxFonts) {
    pA5S_FtCacheHead pHead = (pA5S_FtCacheHead)SDL_malloc(uSize*uSize*2);
    SDL_memset(pHead, 0, uSize*uSize);
    pHead->pLib = (FT_Library)pLib;
    pHead->uSize = uSize;
    pHead->uMaxFonts = uMaxFonts;
    pHead->uY = uSize;



    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "CacheNew    : FT_Lib = %p FT_Cache = %p (%ux%u)[%u]", pHead->pLib, pHead, pHead->uSize, pHead->uSize, pHead->uMaxFonts);

    Uint8 *pDbg = (Uint8*)pHead+(pHead->uSize*pHead->uSize);
    SDL_memset(pDbg, 0, uSize*uSize);
    SDL_memset(pDbg, 0xff, sizeof(A5S_FtCacheHead));

    return pHead;
}
void            A5FT_CacheFree(A5S_FTCache pCache) {
    A5FT_CacheClear(pCache);
    SDL_free(pCache);
}
void            A5FT_CacheClear(A5S_FTCache pCache) {
    pA5S_FtCacheHead pHead = (pA5S_FtCacheHead)pCache;
    FT_Face *pFace = (FT_Face*)(pHead+1);
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "CacheClear  : FT_Lib = %p FT_Cache = %p (%ux%u)[%u]", pHead->pLib, pHead, pHead->uSize, pHead->uSize, pHead->uMaxFonts);
    for(unsigned int i=0; i<pHead->uMaxFonts; ++i) {
        if(pFace[i]) {
            FT_Done_Face(pFace[i]);
        }
    }
    SDL_memset(pHead+1, 0, (pHead->uSize * pHead->uSize) - sizeof(A5S_FtCacheHead));
    pHead->uGlyphCount = 0;
    pHead->uX = 0;
    pHead->uY = pHead->uSize;
    pHead->uH = 0;
    pHead->uUpdate = 1;

    Uint8 *pDbg = (Uint8*)pHead+(pHead->uSize*pHead->uSize);
    SDL_memset(pDbg, 0, (pHead->uSize*pHead->uSize));
    SDL_memset(pDbg, 0xff, sizeof(A5S_FtCacheHead));
}
unsigned int    A5FT_CacheUpdate(A5S_FTCache pCache, unsigned int iUpdate) {
    pA5S_FtCacheHead pHead = (pA5S_FtCacheHead)pCache;
    unsigned int o = pHead->uUpdate;
    pHead->uUpdate = iUpdate;
    return o;
}
unsigned int    A5FT_CacheGetSize(A5S_FTCache pCache) {
    pA5S_FtCacheHead pHead = (pA5S_FtCacheHead)pCache;
    return pHead->uSize;
}

unsigned int    A5FT_FontNewFromFile(A5S_FTCache pCache, const char *szFileName) {
    int size = 0;
    Uint8* pBuf = NULL;
    pBuf = A5UT_FileRead(szFileName, &size);
    int o = A5FT_FontNewFromMem(pCache, pBuf, size);
    if(pBuf) SDL_free(pBuf);
    return o;
}
unsigned int    A5FT_FontNewFromMem(A5S_FTCache pCache, void *pMem, unsigned int uSize) {
    pA5S_FtCacheHead pHead = (pA5S_FtCacheHead)pCache;
    FT_Face *pFace = (FT_Face*)(pHead+1);
    for(unsigned int i=0; i<pHead->uMaxFonts; ++i) {
        if(!pFace[i]) {
            FT_Error fterror = FT_New_Memory_Face(pHead->pLib, pMem, uSize, 0, pFace+i);
            if(fterror) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "can't create FT_Face");
                return pHead->uMaxFonts;
            } else {
                SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "FaceNew     : FT_Lib = %p FT_Cache = %p (%ux%u)[%u]  FT_Face = %p (%d/%d)", pHead->pLib, pHead, pHead->uSize, pHead->uSize, pHead->uMaxFonts, pFace[i], i, pHead->uMaxFonts);
                SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "FaceNewInfo : %p (%u)\n\tnum_faces: %li\n\tface_index: %li\n\tnum_glyphs: %li\n\tfamily_name: %s\n\tstyle_name: %s\n\tnum_fixed_sizes: %i\n\tnum_charmaps: %i\n\tunits_per_EM: %hu\n\theight: %i",
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

                Uint8 *pDbg = (Uint8*)pHead+(pHead->uSize*pHead->uSize);
                SDL_memset(pDbg+sizeof(A5S_FtCacheHead)+sizeof(FT_Face)*i, 0xff, sizeof(FT_Face));

                return i;
            }
        }
    }
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Haven't space for new FT_Face");
    return pHead->uMaxFonts;
}
void            A5FT_FontDel(A5S_FTCache pCache, unsigned int iFontID) {
    pA5S_FtCacheHead pHead = (pA5S_FtCacheHead)pCache;
    if(iFontID >= pHead->uMaxFonts) return;
    FT_Face *pFace = (FT_Face*)(pHead+1);
    if(!pFace[iFontID]) return;
    FT_Done_Face(pFace[iFontID]);
    pFace[iFontID] = NULL;

    Uint8 *pDbg = (Uint8*)pHead+(pHead->uSize*pHead->uSize);
    SDL_memset(pDbg+sizeof(A5S_FtCacheHead)+sizeof(FT_Face)*iFontID, 0x00, sizeof(FT_Face));
}

void            A5FT_GlyphClear(A5S_FTCache pCache) {
    pA5S_FtCacheHead pHead = (pA5S_FtCacheHead)pCache;
    pHead->uGlyphCount = 0;
    pHead->uX = 0;
    pHead->uY = pHead->uSize;
    pHead->uH = 0;
    pHead->uUpdate = 1;

    Uint8 *pDbg = (Uint8*)pHead+(pHead->uSize*pHead->uSize);
    unsigned int szDbg = sizeof(A5S_FtCacheHead)+sizeof(FT_Face)*pHead->uMaxFonts;
    SDL_memset(pDbg+szDbg, 0x00, (pHead->uSize*pHead->uSize)-szDbg);
}

pA5S_Glyph      A5FT_GlyphNew(A5S_FTCache pCache, unsigned int iFontID, unsigned int iGlyphID, unsigned int iHeight) {
    pA5S_FtCacheHead pHead = (pA5S_FtCacheHead)pCache;
    if(iFontID >= pHead->uMaxFonts) return NULL;
    FT_Face *pFace = (FT_Face*)(pHead+1);
    if(!pFace[iFontID]) return NULL;

    FT_Set_Pixel_Sizes(pFace[iFontID], 0, iHeight);
    FT_Load_Glyph(pFace[iFontID], iGlyphID, FT_LOAD_DEFAULT);
    FT_GlyphSlot pG = pFace[iFontID]->glyph;
    FT_Render_Glyph(pG, FT_RENDER_MODE_NORMAL);

    if(pHead->uX + pG->bitmap.width  >= pHead->uSize) {
        pHead->uX = 0;
        if(pHead->uY < pHead->uH) {
            A5FT_GlyphClear(pCache);
            return A5FT_GlyphNew(pCache, iFontID, iGlyphID, iHeight);
        }
        pHead->uY -= pHead->uH;
        pHead->uH = 0;
    }
    if(pHead->uH < pG->bitmap.rows) {
        pHead->uH = pG->bitmap.rows;
    }
    if(pHead->uY <= (sizeof(A5S_FtCacheHead)+sizeof(FT_Face)*pHead->uMaxFonts+sizeof(A5S_Glyph)*(pHead->uGlyphCount+1)+1)/pHead->uSize+1+pHead->uH) {
        A5FT_GlyphClear(pCache);
        return A5FT_GlyphNew(pCache, iFontID, iGlyphID, iHeight);
    }
    Uint8 * p = (Uint8*)pCache + pHead->uX + (pHead->uY-pG->bitmap.rows)*pHead->uSize;

    for(int iy = 0; iy < pG->bitmap.rows; ++iy) {
        SDL_memcpy(p+(iy*pHead->uSize), pG->bitmap.buffer+(iy*pG->bitmap.pitch), pG->bitmap.width);

        Uint8 *pDbg = (Uint8*)p+(iy*pHead->uSize)+pHead->uSize*pHead->uSize;
        if((iy==0) || (iy+1==pG->bitmap.rows)) {
            SDL_memset(pDbg, 0xff, pG->bitmap.width);
        } else {
            SDL_memset(pDbg, 0x7f, pG->bitmap.width);
        } pDbg[0] = 0xff; pDbg[pG->bitmap.width-1] = 0xff;
    }
    ++pHead->uGlyphCount;
    pA5S_Glyph pa5g = (pA5S_Glyph)(pFace+pHead->uMaxFonts)+pHead->uGlyphCount;
    pa5g->iGlyphID  = iGlyphID;
    pa5g->iUnicode  = 0;
    pa5g->iTexX     = pHead->uX;
    pa5g->iTexY     = pHead->uY-pG->bitmap.rows;
    pa5g->iTexW     = pG->bitmap.width;
    pa5g->iTexH     = pG->bitmap.rows;
    pa5g->iOffX     = pG->bitmap_left;
    pa5g->iOffY     = -pG->bitmap_top;
    pa5g->iHeight   = iHeight;
    pa5g->iFontID   = iFontID;
    pa5g->iAdvance  = pG->advance.x >> 6;

    Uint8 *pDbg = (Uint8*)pa5g+pHead->uSize*pHead->uSize;
    SDL_memset(pDbg, 0x7f, sizeof(A5S_Glyph));
    pDbg[0] = 0xff; pDbg[sizeof(A5S_Glyph)-1] = 0xff;

    pHead->uX += pG->bitmap.width;
    pHead->uUpdate = 1;
    return pa5g;
}
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
