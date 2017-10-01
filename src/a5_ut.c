#include "a5_app.h"
#include "a5_ut.h"

Uint8* A5UT_FileRead(const char *szFileName, int *pSize) {
    SDL_RWops *rw = SDL_RWFromFile(szFileName, "rb");
    if(!rw) return NULL;
    Sint64 res_size = SDL_RWsize(rw);
    Uint8 *res = (Uint8*)SDL_malloc(res_size + 1);

    Sint64 nb_read_total = 0, nb_read = 1;
    Uint8 *buf = res;
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
    if (pSize)  *pSize = nb_read_total;
    return res;
}

const char *A5UT_GetUtf8(const char *szText, unsigned int *iChar) {
    unsigned int o = 0;
    if(*szText & 0x80) {
        if(*szText & 0x40) {
            /* Начало символа в 2 байта и более */
            if(*szText & 0x20) {
                /* Начало символа в 3 байта и более */
                if(*szText & 0x10) {
                    /* Начало символа в 4 байта и более */
                    if(*szText & 0x08) {
                        /* Начало символа в 5 байт и более */
                        if(*szText & 0x05) {
                            /* Символ в 6 байт */
                            o|= (unsigned int)(*szText & 0x01)<<30;
                            ++szText;
                            o|= (unsigned int)(*szText & 0x3f)<<24;
                            ++szText;
                            o|= (unsigned int)(*szText & 0x3f)<<18;
                            ++szText;
                            o|= (unsigned int)(*szText & 0x3f)<<12;
                            ++szText;
                            o|= (unsigned int)(*szText & 0x3f)<<6;
                            ++szText;
                            o|= (unsigned int)(*szText & 0x3f)<<6;
                            ++szText;
                        } else {
                            /* Символ в 5 байт */
                            o|= (unsigned int)(*szText & 0x03)<<24;
                            ++szText;
                            o|= (unsigned int)(*szText & 0x3f)<<18;
                            ++szText;
                            o|= (unsigned int)(*szText & 0x3f)<<12;
                            ++szText;
                            o|= (unsigned int)(*szText & 0x3f)<<6;
                            ++szText;
                            o|= (unsigned int)(*szText & 0x3f)<<6;
                            ++szText;
                        }
                    } else {
                        /* Символ в 4 байта */
                        o|= (unsigned int)(*szText & 0x07)<<18;
                        ++szText;
                        o|= (unsigned int)(*szText & 0x3f)<<12;
                        ++szText;
                        o|= (unsigned int)(*szText & 0x3f)<<6;
                        ++szText;
                        o|= (unsigned int)(*szText & 0x3f)<<6;
                        ++szText;
                    }
                } else {
                    /* Символ в 3 байта */
                    o|= (unsigned int)(*szText & 0x0f)<<12;
                    ++szText;
                    o|= (unsigned int)(*szText & 0x3f)<<6;
                    ++szText;
                    o|= (unsigned int)(*szText & 0x3f);
                    ++szText;
                }
            } else {
                /* Символ в 2 байта */
                o|= (unsigned int)(*szText & 0x1f)<<6;
                ++szText;
                o|= (unsigned int)(*szText & 0x3f);
                ++szText;
            }
        } else {
            /* Неправильный символ */
            o = '?';
            ++szText;
        }
    } else {
        /* Обычный ASCII символ */
        o = *szText;
        ++szText;
    }
    if(iChar) *iChar=o;
    return szText;
}

