//
// IT24 SISTEMAS S.A.
// General Type Definitions
//
// Definicion general de tipos
//
// Tarea        Fecha           Autor   Observaciones
// (Inicial)    1998.06.22      mdc     Base
// (Inicial)    1999.05.25      mdc     PCHAR
// (Inicial)    2002.03.04      mdc     WCHAR
//

#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_

#include "alias.h"

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#ifndef FALSE
#define FALSE               (0)
#endif
#ifndef TRUE
#define TRUE                (1)
#endif

// BASETYPES 

typedef int                 INT;
typedef char				CHAR;
typedef short				SHORT;
typedef long				LONG;
typedef unsigned int        UINT;
typedef unsigned long		ULONG;
typedef unsigned short		USHORT;
typedef unsigned char		UCHAR;
typedef int                 *PINT;
typedef unsigned int        *PUINT;
typedef ULONG				*PULONG;
typedef USHORT				*PUSHORT;
typedef UCHAR				*PUCHAR;
typedef char				*PSZ;
typedef char				*PSTR;
typedef CHAR				*PCHAR;

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
typedef BOOL                *PBOOL;
typedef BYTE                *PBYTE;
typedef WORD                *PWORD;
typedef DWORD               *PDWORD;
typedef const void          *PCVOID;
typedef const CHAR			*LPCSTR, *PCSTR;
typedef CHAR				*LPSTR, *PSTR;
typedef BYTE                *LPBYTE;
typedef SHORT				*PSHORT;  
#ifndef VOID
typedef void                VOID;
typedef void                *PVOID;
#endif // VOID

//
// SQL portable types for C 
//
typedef unsigned char           UCHAR;
typedef signed char             SCHAR;
typedef SCHAR                   SQLSCHAR;
typedef long int                SDWORD;
typedef short int               SWORD;
typedef unsigned long int       UDWORD;
typedef unsigned short int      UWORD;
typedef UDWORD                  SQLUINTEGER;

//
// HANDLES
//
#ifndef _DECLARE_HANDLE_DEFINED
#define _DECLARE_HANDLE_DEFINED
#define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
#endif // _DECLARE_HANDLE_DEFINED


typedef void			*HANDLE;
typedef HANDLE			*PHANDLE;
typedef void*           HSTMT;
typedef int				HFILE;
#include <Windows.h>
#ifndef _WINDEF_
//#ifndef _HWND_T_DEFINED
//#define _HWND_T_DEFINED
DECLARE_HANDLE            (HWND);
//#endif // _HWND_T_DEFINED
#endif // _WINDEF_ 

//
// UNICODE (Wide Character) types
//
#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

#ifndef _MAC
typedef wchar_t WCHAR;    // wc,   16-bit UNICODE character
#else
// some Macintosh compilers don't define wchar_t in a convenient location, or define it as a char
typedef unsigned short WCHAR;    // wc,   16-bit UNICODE character
#endif

typedef WCHAR *PWCHAR;
typedef WCHAR *LPWCH, *PWCH;
typedef CONST WCHAR *LPCWCH, *PCWCH;
typedef WCHAR *NWPSTR;
typedef WCHAR *LPWSTR, *PWSTR;

typedef CONST WCHAR *LPCWSTR, *PCWSTR;

// 
// BOOLEAN
//
#ifndef _BOOLEAN_T_
#define _BOOLEAN_T_
typedef enum {is_false=0,is_true=1} boolean_t;
#endif /* _BOOLEAN_T_ */

//
// MIN y MAX
//
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//
// GUID
//
#ifndef GUID_ST_DEFINED
#define GUID_ST_DEFINED

typedef struct GUID_tag {          // size is 16
    DWORD  Data1;
    WORD   Data2;
    WORD   Data3;
    BYTE   Data4[8];
} stGUID;

#ifndef GUID_DEFINED
typedef GUID_tag GUID;          // size is 16
#define GUID_DEFINED
#endif
#endif // !GUID_DEFINED

//
// SYSTEM TIME
//
#ifndef SYSTEMTIME_DEFINED
#define SYSTEMTIME_DEFINED

typedef struct SYSTEMTIME_tag {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} stSYSTEMTIME;
#endif // SYSTEMTIME_DEFINED


#endif // end-ifdef-TYPEDEFS_H

