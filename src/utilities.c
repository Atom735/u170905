#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include <Windows.h>

#include "utilities.h"


#include <stdlib.h>
#include <stdio.h>

INT rLoadFileToMem(LPCSTR lpszFileName, PBYTE *buf, PUINT size) {
    ASSERT(lpszFileName);
    ASSERT(buf);
    ASSERT(size);

    FILE *input;
    UINT fileSize, readed;

    if ((input = fopen(lpszFileName, "rb")) == NULL) {
        LOG(0, LOG_E, TEXT("ERROR Opening file '%hs'"), lpszFileName);
        return -1;
    }

    fseek(input, 0, SEEK_END);
    fileSize = (UINT)ftell(input);
    rewind(input);

    if (!fileSize)  {
        LOG(0, LOG_E, TEXT("ERROR Empty file '%hs'"), lpszFileName);
        fclose(input);
        return -1;
    }
    *buf = (PBYTE)malloc(fileSize);
    ASSERT(*buf);
    readed = fread(*buf, 1, fileSize, input);

    fclose(input);

    if (readed != fileSize)  {
        LOG(0, LOG_E, TEXT("ERROR Reading file '%hs'"), lpszFileName);
        free(*buf);
        *buf = NULL;
        return -1;
    }

    *size = fileSize;

    return 0;
}
