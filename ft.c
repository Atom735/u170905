#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include <Windows.h>

#include "ft.h"
#include "gl.h"

#include <ft2build.h>
#include FT_FREETYPE_H

PBYTE           FT_A8CACHE          = NULL;
UINT            FT_A8CACHE_SIZE     = 2048;
UINT            FT_A8CACHE_X        = 0;
UINT            FT_A8CACHE_Y        = 2048;
UINT            FT_A8CACHE_FACE_MAX = 16;
UINT            FT_A8CACHE_H        = 1;
BOOL            FT_A8CACHE_UPDATE   = 0;
FT_Library      FT_LIB              = NULL;

INT     FT_I(LPSM lpsm, UINT uMsg, LPVOID ptr) {
    if(uMsg == SM_MSG_PUSH) {
        FT_Error fterror = FT_Init_FreeType(&FT_LIB);
        if(fterror) {
            LOG(g_uLogFT, LOG_F, TEXT("FT_Init_FreeType FAIL! %d"), fterror);
            return -1;
        } else {
            LOG(g_uLogFT, LOG_D, TEXT("FT_Init_FreeType OK!\n\tftlib=%p"), FT_LIB);
        }
        FT_A8CACHE = (PBYTE)VirtualAlloc(NULL, FT_A8CACHE_SIZE*FT_A8CACHE_SIZE, MEM_COMMIT, PAGE_READWRITE);

        FT_rNewFaceA("data/RobotoSlab-Regular.ttf");
    } else if(uMsg == SM_MSG_POP) {
        FT_rClean();
        VirtualFree(FT_A8CACHE, 0, MEM_RELEASE);
        if(FT_LIB) {
            FT_Done_FreeType(FT_LIB);
            FT_LIB = NULL;
            LOG(g_uLogFT, LOG_D, TEXT("FT_Done_FreeType OK!"));
        } else {
            LOG(g_uLogFT, LOG_W, TEXT("FT_Done_FreeType miss\nFTLIB is Null"));
        }
        return 0;
    }
    return 0;
}

VOID    FT_rClean() {
    if(!FT_A8CACHE) return;
    FT_Face *pFace = (FT_Face*)(FT_A8CACHE+sizeof(DWORD));
    for(INT i=0; i<FT_A8CACHE_FACE_MAX; ++i) {
        if(pFace[i]) {
            FT_Error fterror = FT_Done_Face(pFace[i]);
            if(fterror) {
                LOG(g_uLogFT, LOG_F, TEXT("FT_Done_Face FAIL! %d\n\tftface=%p"), fterror, pFace[i]);
            } else {
                LOG(g_uLogFT, LOG_D, TEXT("FT_Done_Face OK!\n\tftface=%p"), pFace[i]);
            }
        }
    }
    ZeroMemory(FT_A8CACHE, FT_A8CACHE_SIZE*FT_A8CACHE_SIZE);
    FT_A8CACHE_X = 0;
    FT_A8CACHE_Y = FT_A8CACHE_SIZE;
    FT_A8CACHE_H = 1;
    ++FT_A8CACHE_UPDATE;
}

UINT        FT_rNewFaceA(LPCSTR fName) {
    FT_Face *pFace = (FT_Face*)(FT_A8CACHE+sizeof(DWORD));
    for(INT i=0; i<FT_A8CACHE_FACE_MAX; ++i) {
        if(!pFace[i]) {
            FT_Error fterror = FT_New_Face(FT_LIB, fName, 0, pFace+i);
            if(fterror) {
                LOG(g_uLogFT, LOG_F, TEXT("FT_New_Face FAIL! %d"), fterror);
                return -1;
            } else {
                LOG(g_uLogFT, LOG_D, TEXT("FT_New_Face OK!\n\tftface=%p"), pFace[i]);
                LOG(g_uLogFT, LOG_I, TEXT("NEW FACE: %hs\n\tnum_faces: %i\n\tface_index: %i\n\tnum_glyphs: %i\n\tfamily_name: %hs\n\tstyle_name: %hs\n\tnum_fixed_sizes: %i\n\tnum_charmaps: %i\n\tunits_per_EM: %hu\n\theight: %i"), fName, pFace[i]->num_faces, pFace[i]->face_index, pFace[i]->num_glyphs, pFace[i]->family_name, pFace[i]->style_name, pFace[i]->num_fixed_sizes, pFace[i]->num_charmaps, pFace[i]->units_per_EM, pFace[i]->height);
                return i;
            }
        }
    }
    LOG(g_uLogFT, LOG_F, TEXT("FTCacheNewFaceA FAIL!\n\tHaven't space for new face"));
    return -1;
}
UINT        FT_rNewFaceFromMem(LPBYTE pMem, INT size) {
    FT_Face *pFace = (FT_Face*)(FT_A8CACHE+sizeof(DWORD));
    for(INT i=0; i<FT_A8CACHE_FACE_MAX; ++i) {
        if(!pFace[i]) {

            FT_Error fterror = FT_New_Memory_Face(FT_LIB, pMem, size, 0, pFace+i);
            if(fterror) {
                LOG(g_uLogFT, LOG_F, TEXT("FT_New_Memory_Face FAIL! %d"), fterror);
                return -1;
            } else {
                LOG(g_uLogFT, LOG_D, TEXT("FT_New_Memory_Face OK!\n\tftface=%p"), pFace[i]);
                LOG(g_uLogFT, LOG_I, TEXT("NEW FACE: %p(%d)\n\tnum_faces: %i\n\tface_index: %i\n\tnum_glyphs: %i\n\tfamily_name: %hs\n\tstyle_name: %hs\n\tnum_fixed_sizes: %i\n\tnum_charmaps: %i\n\tunits_per_EM: %hu\n\theight: %i"), pMem, size, pFace[i]->num_faces, pFace[i]->face_index, pFace[i]->num_glyphs, pFace[i]->family_name, pFace[i]->style_name, pFace[i]->num_fixed_sizes, pFace[i]->num_charmaps, pFace[i]->units_per_EM, pFace[i]->height);
                return i;
            }
        }
    }
    LOG(g_uLogFT, LOG_F, TEXT("FTCacheNewFaceA FAIL!\n\tHaven't space for new face"));
    return -1;
}

LPFT_GLYPH  FT_rNewGlyph(UINT iFace, UINT iGlyph, UINT size) {
    FT_Face *pFace = (FT_Face*)(FT_A8CACHE+sizeof(DWORD));
    if(!pFace[iFace]) {
        LOG(g_uLogFT, LOG_F, TEXT("FTCacheNewGlyph FAIL!\n\tidface=%d can't find"), iFace);
        return NULL;
    }
    if(size > 255) {
        LOG(g_uLogFT, LOG_F, TEXT("FTCacheNewGlyph FAIL!\n\tsize=%d is greater than one byte"), size);
        return NULL;
    }
    FT_Set_Pixel_Sizes(pFace[iFace], 0, size);
    FT_Error fterror = FT_Load_Glyph(pFace[iFace], iGlyph, FT_LOAD_DEFAULT);
    if(fterror) {
        LOG(g_uLogFT, LOG_E, TEXT("FTCacheNewGlyph FAIL!\n\t%i/%i: FT_Load_Glyph FAIL! %d"), iGlyph, pFace[iFace]->num_glyphs, fterror);
        return NULL;
    }
    FT_GlyphSlot pG = pFace[iFace]->glyph;
    fterror = FT_Render_Glyph(pG, FT_RENDER_MODE_NORMAL);
    if(fterror) {
        LOG(g_uLogFT, LOG_E, TEXT("FTCacheNewGlyph FAIL!\n\t%i/%i: FT_Render_Glyph FAIL! %d"), iGlyph, pFace[iFace]->num_glyphs, fterror);
        return NULL;
    }

    if(FT_A8CACHE_X + pG->bitmap.width  >= FT_A8CACHE_SIZE) {
        FT_A8CACHE_X = 0;
        FT_A8CACHE_Y -= FT_A8CACHE_H;
        FT_A8CACHE_H = 1;
    }
    if(FT_A8CACHE_H < pG->bitmap.rows) {
        FT_A8CACHE_H = pG->bitmap.rows;
    }
    if(FT_A8CACHE_Y <= ((sizeof(DWORD)+(sizeof(FT_Face)*FT_A8CACHE_FACE_MAX)+(sizeof(FT_GLYPH)*(*((PDWORD)FT_A8CACHE)+1)))/FT_A8CACHE_SIZE)+1+FT_A8CACHE_H) {

        LOG(g_uLogFT, LOG_F, TEXT("FTCacheNewGlyph FAIL!\n\tHaven't space for new glyph"));
        return NULL;
    }

    PBYTE p = FT_A8CACHE+FT_A8CACHE_X+(FT_A8CACHE_Y-pG->bitmap.rows)*FT_A8CACHE_SIZE;

    for(INT iy = 0; iy < pG->bitmap.rows; ++iy) {
        CopyMemory(p+(iy*FT_A8CACHE_SIZE),
            pG->bitmap.buffer+(iy*pG->bitmap.pitch), pG->bitmap.width);
    }
    ++*((PDWORD)FT_A8CACHE);
    LPFT_GLYPH lpa7glyph = (LPFT_GLYPH)(FT_A8CACHE+(sizeof(DWORD)+(sizeof(FT_Face)*FT_A8CACHE_FACE_MAX)+(sizeof(FT_GLYPH)*(*(PDWORD)FT_A8CACHE))));
    lpa7glyph->G = iGlyph;
    lpa7glyph->x = FT_A8CACHE_X;
    lpa7glyph->y = FT_A8CACHE_Y-pG->bitmap.rows;
    lpa7glyph->w = pG->bitmap.width;
    lpa7glyph->h = pG->bitmap.rows;
    lpa7glyph->X = pG->bitmap_left;
    lpa7glyph->Y = pG->bitmap_top;
    lpa7glyph->H = size;
    lpa7glyph->F = iFace;
    lpa7glyph->A = pG->advance.x >> 6;
    lpa7glyph->_ = 0;

    FT_A8CACHE_X += pG->bitmap.width;
    ++FT_A8CACHE_UPDATE;
    return lpa7glyph;
}

LPFT_GLYPH  FT_rGetGlyph(UINT iFace, UINT iGlyph, UINT size) {
    DWORD dw = *((PDWORD)FT_A8CACHE);
    LPFT_GLYPH lpa7glyph = (LPFT_GLYPH)(FT_A8CACHE+(sizeof(DWORD)+(sizeof(FT_Face)*FT_A8CACHE_FACE_MAX)));
    for(DWORD i=0; i<dw; ++i) {
        if((lpa7glyph[i].F == iFace) && (lpa7glyph[i].G == iGlyph)  && (lpa7glyph[i].H == size)) {
            return lpa7glyph+i;
        }
    }
    return FT_rNewGlyph(iFace, iGlyph, size);
}

LPFT_GLYPH  FT_rNewChar(UINT iFace, UINT iUChar, UINT size) {
    FT_Face *pFace = (FT_Face*)(FT_A8CACHE+sizeof(DWORD));
    LPFT_GLYPH o = FT_rNewGlyph(iFace, FT_Get_Char_Index(pFace[iFace], iUChar), size);
    o->U = iUChar;
    return o;
}

LPFT_GLYPH  FT_rGetChar(UINT iFace, UINT iUChar, UINT size) {
    DWORD dw = *((PDWORD)FT_A8CACHE);
    LPFT_GLYPH lpa7glyph = (LPFT_GLYPH)(FT_A8CACHE+(sizeof(DWORD)+(sizeof(FT_Face)*FT_A8CACHE_FACE_MAX)));
    for(DWORD i=0; i<dw; ++i) {
        if((lpa7glyph[i].F == iFace) && (lpa7glyph[i].U == iUChar)  && (lpa7glyph[i].H == size)) {
            return lpa7glyph+i;
        }
    }
    return FT_rNewChar(iFace, iUChar, size);
}
