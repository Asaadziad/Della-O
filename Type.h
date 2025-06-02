#pragma once


typedef signed char        S8;
typedef unsigned char      U8;

typedef short              S16;
typedef unsigned short     U16;

typedef long               S32;
typedef unsigned long      U32;

#if defined(__GNUC__) || defined(_MSC_VER)
typedef signed long long   S64;
typedef unsigned long long U64;
#endif

#if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__)
typedef U64 Size_t;
#else
typedef U32 Size_t;
#endif

typedef U8 Bool;