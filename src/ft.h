#ifndef _a5_ft_h
#define _a5_ft_h

#include <SDL2/SDL_stdinc.h>

typedef struct sA5Glyph {
    Uint16 G, U; // Номер глифа, Unicode
    Uint16 x, y; // Позиция в текстуре
    Sint8  X, Y; // Отступы текстуры при рисовке
    Uint8  w, h; // Размеры в текстуре
    Uint8  H, F; // Высота, номер фонта
    Uint8  A, _; // Отступ след символа, ??? хз чё, не придумал ещё
} sA5Glyph, *psA5Glyph;

int  a5ft_init(void **pLib);
void a5ft_free(void  *pLib);

void a5ft_cache_init (void *pLib, void **pCache, int size, int maxFonts);
void a5ft_cache_free (void *pCache);
void a5ft_cache_clear(void *pCache);
int  a5ft_cache_update(void *pCache, int i);

int  a5ft_new_font_from_file(void *pCache, const char *fName);
int  a5ft_new_font_from_mem (void *pCache, void *pMem, int size);
void a5ft_del_font          (void *pCache, int iFont);

psA5Glyph a5ft_new_glyph(void *pCache, int iFont, int iGlyph, int H);
psA5Glyph a5ft_get_glyph(void *pCache, int iFont, int iGlyph, int H);

psA5Glyph a5ft_new_char(void *pCache, int iFont, int iChar, int H);
psA5Glyph a5ft_get_char(void *pCache, int iFont, int iChar, int H);


#endif
