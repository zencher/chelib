#ifndef _CHELIB_BASE_DEFINE_
#define _CHELIB_BASE_DEFINE_

#include <stdint.h>

typedef uint8_t BYTE;

typedef uint8_t * PBYTE;

typedef uint8_t const * PCBYTE;

typedef int8_t int8;

typedef uint8_t uint8;

typedef int16_t int16;

typedef uint16_t uint16;
 
typedef int32_t  int32;
 
typedef uint32_t uint32;

typedef int64_t int64;

typedef uint64_t uint64;

#ifdef _MSC_VER
	#ifndef _WCHAR_T_DEFINED
	typedef unsigned short wchar_t;
	#endif
#endif

typedef float FLOAT;

typedef double DOUBLE;

typedef uint32 ARGB;

#endif
