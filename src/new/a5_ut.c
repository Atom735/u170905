#include "a5_ut.h"

A5puint8    A5UT_FileRead(A5pcstr szFileName, A5puint puSize) {
    SDL_RWops *rw = SDL_RWFromFile(szFileName, "rb");
    if(!rw) return NULL;
    A5int64 res_size = SDL_RWsize(rw);
    A5puint8 res = (A5puint8)SDL_malloc(res_size + 1);
    A5int64 nb_read_total = 0, nb_read = 1;
    A5puint8 buf = res;
    while((nb_read_total < res_size) && (nb_read != 0)) {
        nb_read = SDL_RWread(rw, buf, 1, (res_size - nb_read_total));
        nb_read_total += nb_read;
        buf += nb_read;
    }
    SDL_RWclose(rw);
    if (nb_read_total != res_size) {
        SDL_free(res);
        return NULL;
    }
    res[nb_read_total] = '\0';
    if (puSize)  *puSize = nb_read_total;
    return res;
}
A5void      A5UT_FileReadFree(A5puint8 pBuf) {
    SDL_free(pBuf);
}

A5pcstr     A5UT_U8_GetUnicode(A5pcstr szText, A5puint puUnicode) {
    if(!(*szText & 0x80)) {
        /* Точно ASCII символ */
        if(puUnicode) *puUnicode=(A5uint)*szText;
        return szText+1;
    }
    if(!(*szText & 0x40)) {
        /* Точно не первый символ */
        if(puUnicode) *puUnicode=(A5uint)'?';
        return szText+1;
    }
    if(!(*szText & 0x20)) {
        /* Символ в 2 байта */
        if(puUnicode) *puUnicode=((A5uint)(szText[0] & 0x1f)<<6)|((A5uint)(szText[1] & 0x3f));
        return szText+2;
    }
    if(!(*szText & 0x10)) {
        /* Символ в 3 байта */
        if(puUnicode) *puUnicode=((A5uint)(szText[0] & 0x0f)<<12)|((A5uint)(szText[1] & 0x3f)<<6)|((A5uint)(szText[2] & 0x3f));
        return szText+3;
    }
    if(!(*szText & 0x08)) {
        /* Символ в 4 байта */
        if(puUnicode) *puUnicode=((A5uint)(szText[0] & 0x07)<<18)|((A5uint)(szText[1] & 0x3f)<<12)|((A5uint)(szText[2] & 0x3f)<<6)|((A5uint)(szText[3] & 0x3f));
        return szText+4;
    }
    if(!(*szText & 0x04)) {
        /* Символ в 5 байт */
        if(puUnicode) *puUnicode=((A5uint)(szText[0] & 0x03)<<24)|((A5uint)(szText[1] & 0x3f)<<18)|((A5uint)(szText[2] & 0x3f)<<12)|((A5uint)(szText[3] & 0x3f)<<6)|((A5uint)(szText[4] & 0x3f));
        return szText+5;
    }
    if(!(*szText & 0x04)) {
        /* Символ в 6 байт */
        if(puUnicode) *puUnicode=((A5uint)(szText[0] & 0x01)<<30)|((A5uint)(szText[1] & 0x3f)<<24)|((A5uint)(szText[2] & 0x3f)<<18)|((A5uint)(szText[3] & 0x3f)<<12)|((A5uint)(szText[4] & 0x3f)<<6)|((A5uint)(szText[5] & 0x3f));
        return szText+6;
    }
    return szText;
}


A5uint      A5UT_U8_GetLenght(A5pcstr szText) {
    A5uint len = 0;
    for(;*szText;++len) {
        if(!(*szText & 0x80)) ++szText;
            /* Точно ASCII символ */
        else if(!(*szText & 0x40)) ++szText;
            /* Точно не первый символ */
        else if(!(*szText & 0x20)) szText+=2;
            /* Символ в 2 байта */
        else if(!(*szText & 0x10)) szText+=3;
            /* Символ в 3 байта */
        else if(!(*szText & 0x08)) szText+=4;
            /* Символ в 4 байта */
        else if(!(*szText & 0x04)) szText+=5;
            /* Символ в 5 байт */
        else if(!(*szText & 0x04)) szText+=6;
            /* Символ в 6 байт */
    }
    return len;
}
