#ifndef _CHE_DEFINE_
#define _CHE_DEFINE_

typedef void	HE_VOID;

//Pointer to any type.
typedef void *  HE_LPVOID;
 
//Pointer to any constant type. 
typedef void const *  HE_LPCVOID;

//Byte (8 bits).
typedef unsigned char  HE_BYTE;

//Pointer to a HE_BYTE.
typedef unsigned char *  HE_LPBYTE;

//Pointer to a constant HE_BYTE.
typedef unsigned char const *  HE_LPCBYTE;
 
//Short integer (16 bits).
typedef short  HE_SHORT;
 
//16-bit unsigned integer.
typedef unsigned short  HE_WORD;
 
//Pointer to a DWORD.
typedef unsigned short *  HE_LPWORD;
 
//32-bit signed integer.
typedef int  HE_INT32;

typedef unsigned int HE_UINT32;
 
//32-bit unsigned integer. 
typedef unsigned long  HE_DWORD;

//Pointer to a DWORD. 
typedef unsigned long *  HE_LPDWORD;

//Boolean variable (should be TRUE or FALSE).
typedef bool HE_BOOL;
 
//8-bit Windows (ANSI) character. 
typedef char  HE_CHAR;

//Pointer to 8-bit Windows (ANSI) characters.
typedef char *  HE_LPSTR;
 
//Pointer to constant 8-bit Windows (ANSI) characters. 
typedef char const *  HE_LPCSTR;

#ifdef _MSC_VER
	#ifndef _WCHAR_T_DEFINED
	typedef unsigned short wchar_t;
	#endif
#endif

//16-bit Unicode character.
typedef wchar_t  HE_WCHAR;

//Pointer to 16-bit Unicode characters.
typedef wchar_t *  HE_LPWSTR;
 
//Pointer to constant 16-bit Unicode characters.
typedef wchar_t const *  HE_LPCWSTR;

typedef long HE_LONG;

//Signed 64-bit integer. 
typedef long int HE_INT64;

//Unsigned 64-bit integer.
typedef unsigned long HE_UINT64;

//Signed integral type for pointer precision.
typedef int  HE_INTPTR;

//Unsigned integral type for pointer precision.
typedef unsigned int HE_UINTPTR;

//32-bit floating-point number.
typedef float  HE_FLOAT;

typedef double HE_DOUBLE;

#define FALSE 0
#define TRUE 1

#ifndef NULL
#define NULL 0
#endif

struct HE_RECT
{
	HE_DWORD left;
	HE_DWORD top;
	HE_DWORD width;
	HE_DWORD height;
};

#define HE_ABS abs

#endif
