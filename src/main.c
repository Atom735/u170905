#include <WinDef.h>

#include "log.h"

INT main(INT argc, LPSTR argv[]) {
    LOG_rStart();
    LOG_rSetLvl(LOG_V);

    UINT uLogTestEng = LOG_rNewCategory(TEXT("TEST Cat"));
    UINT uLogTestRus = LOG_rNewCategory(TEXT("Тест кат"));

    LOG(uLogTestEng, LOG_VERBOSE, TEXT("Verbose %d"), 0x1);
    LOG(uLogTestEng, LOG_DEBUG, TEXT("Debug info %d"), 0x2);
    LOG(uLogTestEng, LOG_INFO, TEXT("Information %d"), 0x3);
    LOG(uLogTestEng, LOG_WARN, TEXT("Warning %d"), 0x4);
    LOG(uLogTestEng, LOG_ERROR, TEXT("Error %d"), 0x5);
    LOG(uLogTestEng, LOG_FATAL, TEXT("Fatal error %d"), 0x6);

    LOG(uLogTestRus, LOG_VERBOSE, TEXT("Подробная информация %d"), 0x7);
    LOG(uLogTestRus, LOG_DEBUG, TEXT("Отладочная инфа %d"), 0x8);
    LOG(uLogTestRus, LOG_INFO, TEXT("Полезная инфа %d"), 0x9);
    LOG(uLogTestRus, LOG_WARN, TEXT("Предупреждение %d"), 0xa);
    LOG(uLogTestRus, LOG_ERROR, TEXT("Ошибка %d"), 0xb);
    LOG(uLogTestRus, LOG_FATAL, TEXT("Фатальная ошибка %d"), 0xc);

    LOG(uLogTestEng, LOG_INFO, TEXT("Information multiline\nIts secpnd line\nThird line"));

    LOG(uLogTestRus, LOG_INFO, TEXT("Сообщение в несколько линий\nВторая линия\nТретья линия"));


    LOG_rFin();
    return 0;
}
