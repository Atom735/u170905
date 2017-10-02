#ifndef _H_A5_FT
#define _H_A5_FT 171002

#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_log.h>

typedef void            FTvoid,     *FTpvoid;
typedef unsigned int    FTuint,     *FTpuint;
typedef int             FTint,      *FTpint;
typedef char            FTchar,     *FTpchar;
typedef FTpchar         FTstr,      *FTpstr;
typedef const FTstr     FTcstr,     *FTpcstr;

/*A5FT*RU*A5FT_System
    ****************************
        Система A5FT
    ****************************
*/
    typedef FTpvoid         pA5FT_Lib;
    /*A5FT*RU*
        Инициализация системы A5FT
        +In:    NULL
        +Out:   Указатель на систему A5FT (NULL - в случае ошибки)
    */
    pA5FT_Lib       A5FT_Init();
    /*A5FT*RU*
        Освобождение системы A5FT
        +In:    +0: Указатель на систему A5FT
        +Out:   NULL
    */
    FTvoid          A5FT_Free(pA5FT_Lib pLib);

/*A5FT*RU*A5FT_Cache
    ****************************
        Кеш A5FT
    ****************************
*/
    /*A5FT*RU*
        Структура начала кеша A5FT
        +pLib:              Указатель на систему A5FT
        +uCacheSize:        Размер стороны кеша
        +uMaxFonts:         Максимальное количество загруженных шрифтов
        +uGlyphCount:       Количество глифов
        +uPenX:             Позиция пера в кеше
        +uPenY:             Позиция пера в кеше
        +uLineHeight:       Высота заполняемой линии кеша
        +uFlagUpdate:       Флаг обновления (увеличивается при каждом изменении кеша)
        +uFlagOptimize:     Флаг состояния оптимизации
    */
    typedef struct tagA5S_FT_Cache {
        FTpvoid         pLib;
        FTuint          uCacheSize;
        FTuint          uMaxFonts;
        FTuint          uGlyphCount;
        FTuint          uPenX;
        FTuint          uPenY;
        FTuint          uLineHeight;
        FTuint          uFlagUpdate;
        FTuint          uFlagOptimize;
    } *pA5FT_Cache;
    /*A5FT*RU*
        Создание кеша под глифы (символы)
        +In:    +0: Указатель на систему A5FT
                +1: Размер стороны кеша
                +2: Максимальное количество загруженных шрифтов (0 - для кастомного кеша)
        +Out:   Указатель на кеш A5FT (NULL - в случае ошибки)
    */
    pA5FT_Cache     A5FT_CacheInit(pA5FT_Lib pLib, FTuint uSize, FTuint uMaxFonts);
    /*A5FT*RU*
        Освобождение кеша
        +In:     +0: Указатель на кеш A5FT
        +Out:    NULL
    */
    FTvoid          A5FT_CacheFree(pA5FT_Cache pCache);
    /*A5FT*RU*
        Полное очищение кеша (высвобождает все шрифты и глифы)
        +In:    +0: Указатель на кеш A5FT
        +Out:   NULL
    */
    FTvoid          A5FT_CacheClear(pA5FT_Cache pCache);
    /*A5FT*RU*
        Оптимизация кеша A5FT
        +In:    +0: Указатель на кеш A5FT
                +1: Степень оптимизации
        +Out:   NULL
    */
    FTvoid          A5FT_CacheOptimize(pA5FT_Cache pCache, FTuint uFlagOptimize);
    /*A5FT*RU*
        Получть дебаг кеш кеша A5FT
        +In:    +0: Указатель на кеш A5FT
                +1: Тип отладочной информации
                +2: Указатель на пустой массив размера кеша куда запишем данные
        +Out:   Указатель на заполненый массив который получили в аргументе
    */
    FTpvoid         A5FT_CacheDebugGet(pA5FT_Cache pCache, FTuint uFlagDebug, FTpvoid pBuf);

/*A5FT*RU*A5FT_Font
    ****************************
        Шрифты A5FT
    ****************************
*/
    /*A5FT*RU*
        Загрузка шрифта из файла
        +In:    +0: Указатель на кеш A5FT
                +1: (u8) Путь к файлу
        +Out:   Номер шрифта в кеше (0 - в случае ошибки)
    */
    FTuint          A5FT_FontNewFromFile(pA5FT_Cache pCache, FTcstr szFileName);
    /*A5FT*RU*
        Загрузка шрифта из участка памяти
        +In:    +0: Указатель на кеш A5FT
                +1: Указатель на участок памяти
                +2: Размер памяти
        +Out:   Номер шрифта в кеше (0 - в случае ошибки)
    */
    FTuint          A5FT_FontNewFromMem(pA5FT_Cache pCache, FTpvoid pMem, FTuint uSize);
    /*A5FT*RU*
        Удаление шрифта из кеша
        +In:    +0: Указатель на кеш A5FT
                +1: Номер шрифта в кеше
                +2: Размер памяти
        +Out:   NULL
    */
    FTvoid          A5FT_FontDelete(pA5FT_Cache pCache, FTuint uFontID);

/*A5FT*RU*A5FT_Font
    ****************************
        Глифы A5FT
    ****************************
*/
    /*A5FT*RU*
        Структура глифа A5FT
        +uFontID:           Номер шрифта в кеше (0 - для собственного глифа)
        +uHeight:           Высота глифа
        +uUnicode:          Юникод глифа
        +uGlyphID:          Номер глифа в шрифте
        +uAdvance:          Ширина глифа
        +uCachePosX:        Позиция в кеше
        +uCachePosY:        Позиция в кеше
        +uCacheWidth:       Ширина в кеше
        +uCacheHeight:      Высота в кеше
        +iTexOffsetX:       Отступ текстуры
        +iTexOffsetY:       Отступ текстуры
    */
    typedef struct tagA5S_FT_Glyph {
        FTuint          uFontID;
        FTuint          uHeight;
        FTuint          uUnicode;
        FTuint          uGlyphID;
        FTuint          uAdvance;
        FTuint          uCachePosX;
        FTuint          uCachePosY;
        FTuint          uCacheWidth;
        FTuint          uCacheHeight;
        FTint           iTexOffsetX;
        FTint           iTexOffsetY;
    } *pA5FT_Glyph;
    /*A5FT*RU*
        Сброс всех глифов
        +In:    +0: Указатель на кеш A5FT
        +Out:   NULL
    */
    FTvoid          A5FT_GlyphClear(pA5FT_Cache pCache);
    /*A5FT*RU*
        Создать новый собственный глиф из участка памяти
        +In:    +0: Указатель на кеш A5FT
                +1: Ширина данных в памяти
                +2: Высота данных в памяти
                +3: Указатель на память
                +4: Ширина самой памяти
        +Out:   Указатель на глиф (NULL - в случае ошибки)
    */
    pA5FT_Glyph     A5FT_GlyphNewCustom(pA5FT_Cache pCache, FTuint uBufWidth, FTuint uBufHeight, FTpvoid pBuf, FTuint uBufPitch);
    /*A5FT*RU*
        Создать новый глиф по номеру глифа из загруженого шрифта
        +In:    +0: Указатель на кеш A5FT
                +1: Номер шрифта в кеше
                +2: Высота глифа
                +3: Номер глифа в шрифте
        +Out:   Указатель на глиф (NULL - в случае ошибки)
    */
    pA5FT_Glyph     A5FT_GlyphNew(pA5FT_Cache pCache, FTuint uFontID, FTuint uHeight, FTuint uGlyphID);
    /*A5FT*RU*
        Создать новый глиф по юникоду из загруженого шрифта
        +In:    +0: Указатель на кеш A5FT
                +1: Номер шрифта в кеше
                +2: Высота глифа
                +3: Юникод глифа
        +Out:   Указатель на глиф (NULL - в случае ошибки)
    */
    pA5FT_Glyph     A5FT_GlyphNewUnicode(pA5FT_Cache pCache, FTuint uFontID, FTuint uHeight, FTuint uUnicode);
    /*A5FT*RU*
        Найти собственный глиф
        +In:    +0: Указатель на кеш A5FT
                +1: Номер глифа
        +Out:   Указатель на глиф (NULL - в случае ошибки или его отсутсвия)
    */
    pA5FT_Glyph     A5FT_GlyphGetCustom(pA5FT_Cache pCache, FTuint uGlyphID);
    /*A5FT*RU*
        Найти глиф по номеру глифа из загруженого шрифта
        +In:    +0: Указатель на кеш A5FT
                +1: Номер шрифта в кеше
                +2: Высота глифа
                +3: Номер глифа в шрифте
        +Out:   Указатель на глиф (NULL - в случае ошибки или его отсутсвия)
    */
    pA5FT_Glyph     A5FT_GlyphGet(pA5FT_Cache pCache, FTuint uFontID, FTuint uHeight, FTuint uGlyphID);
    /*A5FT*RU*
        Найти глиф по юникоду из загруженого шрифта
        +In:    +0: Указатель на кеш A5FT
                +1: Номер шрифта в кеше
                +2: Высота глифа
                +3: Юникод глифа
        +Out:   Указатель на глиф (NULL - в случае ошибки или его отсутсвия)
    */
    pA5FT_Glyph     A5FT_GlyphGetUnicode(pA5FT_Cache pCache, FTuint uFontID, FTuint uHeight, FTuint uUnicode);

#endif
