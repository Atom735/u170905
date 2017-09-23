#include "ft.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <SDL2/SDL_log.h>

#include "utils.h"

int a5ft_init(void **pLib) {
    FT_Error fterror = FT_Init_FreeType((FT_Library*)pLib);
    if(fterror) {
        SDL_Log("FT_Init_FreeType FAIL! %d", fterror);
        return -1;
    } else {
        SDL_Log("FT_Init_FreeType\n\tftlib=%p", *pLib);
    }
    return 0;
}

void a5ft_free(void  *pLib) {
    SDL_Log("FT_Done_FreeType\n\tftlib=%p", pLib);
    FT_Done_FreeType(pLib);
}

typedef struct sA5FtCacheHead{
    FT_Library pLib;
    Uint16 uSize, uF;
    Uint32 uG;
    Uint16 uX, uY, uUpdate, uH;
} sA5FtCacheHead, *psA5FtCacheHead;

void a5ft_cache_init (void *pLib, void **pCache, int size, int maxFonts) {
    if(*pCache) a5ft_cache_free(*pCache);
    *pCache = SDL_malloc(size*size);

    SDL_memset(*pCache, 0, size*size);
    psA5FtCacheHead pHead = (psA5FtCacheHead)(*pCache);
    pHead->pLib = (FT_Library)pLib;
    pHead->uSize = size;
    pHead->uF = maxFonts;
    pHead->uY = size;
    SDL_Log("FT New cahce\n\tftlib=%p ftcache=%p(%d^2)[%d]", pHead->pLib, pHead, pHead->uSize, pHead->uF);
}
void a5ft_cache_free (void *pCache) {
    a5ft_cache_clear(pCache);
    SDL_free(pCache);
}
void a5ft_cache_clear(void *pCache) {
    psA5FtCacheHead pHead = (psA5FtCacheHead)pCache;
    FT_Face *pFace = (FT_Face*)(pHead+1);
    SDL_Log("FT Clear cahce\n\tftlib=%p ftcache=%p(%d^2)[%d]", pHead->pLib, pHead, pHead->uSize, pHead->uF);
    for(Uint16 i=0; i<pHead->uF; ++i) {
        if(!pFace[i]) {
            FT_Done_Face(pFace[i]);
        }
    }
    sA5FtCacheHead buf = *pHead;
    SDL_memset(pCache, 0, buf.uSize * buf.uSize);
    *pHead = buf;
}

int  a5ft_cache_update(void *pCache, int i) {
    psA5FtCacheHead pHead = (psA5FtCacheHead)pCache;
    if(i<0) return pHead->uH;
    int o = pHead->uH;
    pHead->uH = i;
    return o;
}

int  a5ft_new_font_from_file(void *pCache, const char *fName) {
    int size = 0;
    char* pBuf = NULL;
    pBuf = file_read(fName, &size);
    int o = a5ft_new_font_from_mem(pCache, pBuf, size);
    if(pBuf) SDL_free(pBuf);
    return o;
}
int  a5ft_new_font_from_mem (void *pCache, void *pMem, int size) {
    psA5FtCacheHead pHead = (psA5FtCacheHead)pCache;
    FT_Face *pFace = (FT_Face*)(pHead+1);

    for(Uint16 i=0; i<pHead->uF; ++i) {
        if(!pFace[i]) {

            FT_Error fterror = FT_New_Memory_Face(pHead->pLib, pMem, size, 0, pFace+i);
            if(fterror) {
                SDL_Log("FT_New_Memory_Face FAIL! %d", fterror);
                return -1;
            } else {
                SDL_Log("FT_New_Memory_Face\n\tftlib=%p ftface=(%d/%d)%p", pHead->pLib, i, pHead->uF, pFace[i]);
                SDL_Log("NEW FACE: %p(%d)\n\tnum_faces: %li\n\tface_index: %li\n\tnum_glyphs: %li\n\tfamily_name: %s\n\tstyle_name: %s\n\tnum_fixed_sizes: %i\n\tnum_charmaps: %i\n\tunits_per_EM: %hu\n\theight: %i",
                    pMem, size,
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
    SDL_Log("Haven't space for new face");
    return -1;
}
void a5ft_del_font          (void *pCache, int iFont) {
    if(iFont < 0) return;
    psA5FtCacheHead pHead = (psA5FtCacheHead)pCache;
    if(iFont >= pHead->uF) return;
    FT_Face *pFace = (FT_Face*)(pHead+1);
    if(!pFace[iFont]) return;
    FT_Done_Face(pFace[iFont]);
    pFace[iFont] = NULL;
}

psA5Glyph a5ft_new_glyph(void *pCache, int iFont, int iGlyph, int H) {
    if(iFont < 0) return NULL;
    if(iGlyph < 0) return NULL;
    if(H > 0xff) return NULL;
    psA5FtCacheHead pHead = (psA5FtCacheHead)pCache;
    if(iFont >= pHead->uF) return NULL;
    FT_Face *pFace = (FT_Face*)(pHead+1);
    if(!pFace[iFont]) return NULL;

    FT_Set_Pixel_Sizes(pFace[iFont], 0, H);
    FT_Load_Glyph(pFace[iFont], iGlyph, FT_LOAD_DEFAULT);
    FT_GlyphSlot pG = pFace[iFont]->glyph;
    FT_Render_Glyph(pG, FT_RENDER_MODE_NORMAL);

    if(pHead->uX + pG->bitmap.width  >= pHead->uSize) {
        pHead->uX = 0;
        if(pHead->uY < pHead->uH) return NULL;
        pHead->uY -= pHead->uH;
        pHead->uH = 0;
    }
    if(pHead->uH < pG->bitmap.rows) {
        pHead->uH = pG->bitmap.rows;
    }
    if(pHead->uY <= (sizeof(sA5FtCacheHead)+sizeof(FT_Face)*pHead->uF+sizeof(sA5Glyph)*(pHead->uG+1)+1)/pHead->uSize+1+pHead->uH) return NULL;
    Uint8 * p = (Uint8*)pCache + pHead->uX + (pHead->uY-pG->bitmap.rows)*pHead->uSize;

    for(int iy = 0; iy < pG->bitmap.rows; ++iy) {
        SDL_memcpy(p+(iy*pHead->uSize), pG->bitmap.buffer+(iy*pG->bitmap.pitch), pG->bitmap.width);
    }
    ++pHead->uG;
    psA5Glyph pa5g = (psA5Glyph)(pFace+pHead->uF)+pHead->uG;
    pa5g->G = iGlyph;
    pa5g->U = 0;
    pa5g->x = pHead->uX;
    pa5g->y = pHead->uY-pG->bitmap.rows;
    pa5g->w = pG->bitmap.width;
    pa5g->h = pG->bitmap.rows;
    pa5g->X = pG->bitmap_left;
    pa5g->Y = -pG->bitmap_top;
    pa5g->H = H;
    pa5g->F = iFont;
    pa5g->A = pG->advance.x >> 6;
    pa5g->_ = 0;

    pHead->uX += pG->bitmap.width;
    ++pHead->uUpdate;
    return pa5g;
}
psA5Glyph a5ft_get_glyph(void *pCache, int iFont, int iGlyph, int H) {
    if(iFont < 0) return NULL;
    if(iGlyph < 0) return NULL;
    if(H > 0xff) return NULL;
    psA5FtCacheHead pHead = (psA5FtCacheHead)pCache;
    if(iFont >= pHead->uF) return NULL;
    FT_Face *pFace = (FT_Face*)(pHead+1);
    psA5Glyph pa5g = (psA5Glyph)(pFace+pHead->uF);
    for(Uint32 i=0; i<pHead->uG; ++i)
        if((pa5g[i].F == iFont)&&(pa5g[i].H == H)&&(pa5g[i].G == iGlyph)) return pa5g+i;
    return a5ft_new_glyph(pCache, iFont, iGlyph, H);
}

psA5Glyph a5ft_new_char(void *pCache, int iFont, int iChar, int H) {
    psA5Glyph pa5g = a5ft_new_glyph(pCache, iFont, FT_Get_Char_Index(((FT_Face*)(((psA5FtCacheHead)pCache)+1))[iFont], iChar), H);
    if(pa5g) pa5g->U = iChar;
    return pa5g;
}
psA5Glyph a5ft_get_char(void *pCache, int iFont, int iChar, int H) {
    psA5Glyph pa5g = a5ft_get_glyph(pCache, iFont, FT_Get_Char_Index(((FT_Face*)(((psA5FtCacheHead)pCache)+1))[iFont], iChar), H);
    if(pa5g) pa5g->U = iChar;
    return pa5g;
}
