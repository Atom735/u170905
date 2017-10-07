#ifndef _H_A5_UT
#define _H_A5_UT 171007

#include "a5_types.h"


A5puint8    A5UT_FileRead(A5pcstr szFileName, A5puint puSize);
A5void      A5UT_FileReadFree(A5puint8 pBuf);
A5pcstr     A5UT_U8_GetUnicode(A5pcstr szText, A5puint puUnicode);
A5uint      A5UT_U8_GetLenght(A5pcstr szText);

#endif /* _H_A5_UT */
