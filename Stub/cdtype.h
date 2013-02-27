/*==========================================================================*
|                  			WIN32 DATA TYPE DEFINE.			         		|
| UNIT: cdtype.h                                                            |
| DESP: 仿WIN32数据类型                                                     |
| AUTH: JOHN.LEE															|
| DATE: 2011-04-20															|
|---------------------------------------------------------------------------|
| INFO: Copyright (c) 2006, AZ. 											|
|		All rights reserved.        										|
*==========================================================================*/

#ifndef __CDTYPE_H_INCLUDE_BE749EFD_92D1_4444_ADE3_52116B834618__
#define __CDTYPE_H_INCLUDE_BE749EFD_92D1_4444_ADE3_52116B834618__

#pragma once
#include <sys/types.h>


#define CONST               const

#if !defined(VOID_TYPEDEFINED)
typedef void				VOID;
typedef VOID            	*PVOID;
typedef CONST VOID      	*CPVOID;
typedef VOID * CONST        PCVOID;
#endif

#if !defined(SIZE_T)
typedef size_t SIZE_T;
#endif

/*native define*/
#if !defined(_NATIVE_DEFINE_)
#define _NATIVE_DEFINE_

#if !defined(INT_TYPEDEFINED)
typedef int                 INT;
typedef INT            		*PINT;
#endif

#if !defined(UINT_TYPEDEFINED)
typedef unsigned int        UINT;
typedef UINT        		*PUINT;
#endif

#if !defined(LONG_TYPEDEFINED)
typedef long				LONG;
typedef LONG            	*PLONG;
#endif

#if !defined(ULONG_TYPEDEFINED)
typedef unsigned long 		ULONG;
typedef ULONG 				*PULONG;
#endif

#if !defined(DWORD_TYPEDEFINED)
typedef ULONG      			DWORD;
typedef DWORD          		*PDWORD;
#endif

#if !defined(SHORT_TYPEDEFINED)
typedef short				SHORT;
typedef SHORT            	*PSHORT;
#endif

#if !defined(USHORT_TYPEDEFINED)
typedef unsigned short 		USHORT;
typedef USHORT 				*PUSHORT;
#endif

#if !defined(WORD_TYPEDEFINED)
typedef USHORT      		WORD;
typedef WORD           		*PWORD;
#endif

#if !defined(FLOAT_TYPEDEFINED)
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
#endif

#if !defined(DOUBLE_TYPEDEFINED)
typedef double              DOUBLE;
typedef DOUBLE              *PDOUBLE;
#endif

#if !defined(CHAR_TYPEDEFINED)
typedef char				CHAR;
typedef char				*PCHAR;
typedef CHAR				*PSTR;
typedef CONST CHAR			*CPSTR;
typedef CHAR * CONST        PCSTR;
#endif

#if !defined(UCHAR_TYPEDEFINED)
typedef unsigned char 		UCHAR;
typedef UCHAR 				*PUCHAR;
#endif

#if !defined(BYTE_TYPEDEFINED)
typedef UCHAR       		BYTE;
typedef BYTE				*PBYTE;
typedef CONST BYTE			*CPBYTE;
typedef BYTE * CONST        PCBYTE;
#endif

#if !defined(BOOL_TYPEDEFINED)
typedef unsigned            BOOL;
typedef BOOL           		*PBOOL;
#endif

// #if !defined(TEXT)
// typedef PBYTE				TEXT;
// #endif

#endif /*#if !defined(_NATIVE_DEFINE_)*/


/*设置宏禁止类型扩展*/
#if !defined(NO_TYPE_EXTEND)

typedef long long				__INT64;
typedef unsigned long long		__UINT64;

typedef int					    __INT32;
typedef unsigned int			__UINT32;

typedef short int			    __INT16;
typedef unsigned short int		__UINT16;

typedef char			        __INT8;
typedef unsigned char		    __UINT8;


#if !defined(LINT_TYPEDEFINED)
typedef __INT64            LINT;
typedef LINT           		*PLINT;
#endif

#if !defined(ULINT_TYPEDEFINED)
typedef __UINT64    		ULINT;
typedef ULINT           	*PULINT;
#endif


#if !defined(_64BIT_INT_DECLARE_)

/*64位强行声明*/
typedef __INT64            		INT64;
typedef INT64            		*PINT64;

typedef __UINT64    			UINT64;
typedef UINT64        			*PUINT64;

typedef INT64					LONG64;
typedef LONG64            		*PLONG64;

typedef UINT64					ULONG64;
typedef ULONG64 				*PULONG64;

typedef ULONG64      			DWORD64;
typedef DWORD64        			*PDWORD64;

#endif


#if !defined(_32BIT_INT_DECLARE_)
/*32位强行声明*/
typedef __INT32            	INT32;
typedef INT32            	*PINT32;

typedef __UINT32    		UINT32;
typedef UINT32        		*PUINT32;

typedef INT32				LONG32;
typedef LONG32            	*PLONG32;

typedef UINT32				ULONG32;
typedef ULONG32 			*PULONG32;

typedef ULONG32      		DWORD32;
typedef DWORD32        		*PDWORD32;

#endif

#endif /*#if !defined(NO_TYPE_EXTEND)*/



#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#ifndef ISTRUE
#define ISTRUE(a)			(a > 0)
#endif

#ifndef ISFALSE
#define ISFALSE(a)			(a < 1)
#endif

#ifndef ISZERO
#define ISZERO(a)			(a == 0)
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL    			0x00000000
#define INVALIDPOINTER		0xFFFFFFFF
#else
#define NULL    			((void *)0x00000000)
#define INVALIDPOINTER		((void *)0xFFFFFFFF);
#endif
#endif


#ifndef ISNULL
#define ISNULL(a) ((void *)a == NULL)
#endif

#ifndef ASSERT
#define ASSERT(a) ((void *)a > NULL)
#endif

#ifndef ISEMPTY
#define ISEMPTY(a) ((void *)a==NULL)
#endif


#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))


#endif /* __CDTYPE_H_INCLUDE_BE749EFD_92D1_4444_ADE3_52116B834618__ */
