#ifndef _H_A5_TYPES
#define _H_A5_TYPES 171007

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>

#define A5TYPEDEF(a, b) typedef a A5##b, *A5p##b, **A5pp##b

A5TYPEDEF(void,             void);

A5TYPEDEF(signed char,      char);
A5TYPEDEF(signed short,     short);
A5TYPEDEF(signed int,       int);
A5TYPEDEF(signed long,      long);
A5TYPEDEF(signed long long, dlong);

A5TYPEDEF(signed char,      uchar);
A5TYPEDEF(signed short,     ushort);
A5TYPEDEF(signed int,       uint);
A5TYPEDEF(signed long,      ulong);
A5TYPEDEF(signed long long, udlong);

A5TYPEDEF(int8_t,           int8);
A5TYPEDEF(int16_t,          int16);
A5TYPEDEF(int32_t,          int32);
A5TYPEDEF(int64_t,          int64);

A5TYPEDEF(uint8_t,          uint8);
A5TYPEDEF(uint16_t,         uint16);
A5TYPEDEF(uint32_t,         uint32);
A5TYPEDEF(uint64_t,         uint64);

A5TYPEDEF(A5uint,           bool);

A5TYPEDEF(char,             str);
A5TYPEDEF(const char,       cstr);

enum {
    A5OID_APP = 0x1,
    A5OID_GL_EXT,
    A5OID_FT_LIB,
    A5OID_FT_CACHE,
    A5OID_FT_GLYPH,

    A5OIDEND
};


#endif /* _H_A5_TYPES */
