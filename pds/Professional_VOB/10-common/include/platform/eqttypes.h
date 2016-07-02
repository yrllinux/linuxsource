/* eqtTypes.h - header file for defining basic data types. */

/* Copyright(C) 2001-2002 KDC, All rights reserved. */ 

/*  
modification history 
-------------------- 
01a,13jan2003,xf  written. 
*/
   

/*
//   SCODE:  Return code
//   INT8:   a signed byte
//   INT16:  a signed 16-bit word
//   INT32:  a signed 32-bit word
//   INT64:  a signed 64-bit word
//   UINT8:  an unsigned byte
//   UINT16: an unsigned 16-bit word
//   UINT32: an unsigned 32-bit word
//   UINT:   synonym for UINT32
//   UINT64: an unsigned 64-bit word
//   BOOL:   boolean
//   BYTE:   an unsigned char
*/

#ifndef __INCeqtTypesh
#define __INCeqtTypesh

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(TYPE_INT8) && !defined(__INCvxTypesh) && !defined(__INCvxTypesOldh)
typedef char INT8, *PINT8;
#define TYPE_INT8
#endif 

#if !defined(TYPE_INT16) && !defined(__INCvxTypesh) && !defined(__INCvxTypesOldh)
typedef short INT16, *PINT16;
#define TYPE_INT16
#endif 

#if !defined(TYPE_INT32) && !defined(__INCvxTypesh) && !defined(__INCvxTypesOldh)
typedef int INT32, *PINT32;
#define TYPE_INT32
#endif 

#if !defined(TYPE_INT64)
#ifdef _MSC_VER
typedef __int64 INT64, *PINT64;
#else 
typedef long long INT64, *PINT64;
#endif 
#define TYPE_INT64
#endif

#if !defined(TYPE_UINT8) && !defined(__INCvxTypesh) && !defined(__INCvxTypesOldh)
typedef unsigned char UINT8, *PUINT8;
#define TYPE_UINT8
#endif 

#if !defined(TYPE_UINT16) && !defined(__INCvxTypesh) && !defined(__INCvxTypesOldh)
typedef unsigned short UINT16, *PUINT16;
#define TYPE_UINT16
#endif 

#if !defined(TYPE_UINT32) && !defined(__INCvxTypesh) && !defined(__INCvxTypesOldh)
typedef unsigned long UINT32, *PUINT32;
#define TYPE_UINT32
#endif 

#if !defined(TYPE_INT) && !defined(__INCvxTypesh) && !defined(__INCvxTypesOldh)
typedef int INT, *PINT;
#define TYPE_INT
#endif 

#if !defined(TYPE_UINT) && !defined(__INCvxTypesh) && !defined(__INCvxTypesOldh)
typedef unsigned int UINT, *PUINT;
#define TYPE_UINT
#endif 


#if !defined(TYPE_UINT64)
#ifdef _MSC_VER
typedef unsigned __int64 UINT64, *PUINT64;
#else 
typedef unsigned long long UINT64, *PUINT64;
#endif
#define TYPE_UINT64
#endif 


#if !defined(TYPE_BOOL) && !defined(__INCvxTypesh) && !defined(__INCvxTypesOldh)
typedef int BOOL, *PBOOL;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define TYPE_BOOL
#endif

#ifndef _MSC_VER 
typedef  unsigned long     DWORD;
#endif

#ifdef _MSC_VER 
#define _BIG_ENDIAN 0
#define _BYTE_ORDER 1
#endif

#ifdef __linux__
#define _BIG_ENDIAN 1
#define _BYTE_ORDER 1
#endif

typedef  unsigned short int     WORD;


#ifdef __linux__
#if !defined(TYPE_CHAR)
typedef unsigned char UCHAR, *PUCHAR;
typedef char CHAR;
#define TYPE_CHAR
#endif /* CHAR */
#endif

#if !defined(TYPE_BYTE)
typedef unsigned char BYTE, *PBYTE;
typedef char SBYTE;
#define TYPE_BYTE
#endif /* BYTE */

#if !defined(TYPE_WORD16)
typedef unsigned short WORD16, *PWORD16;
typedef short SWORD16;
#define TYPE_WORD16
#endif /* WORD16 */

#if !defined(TYPE_WORD32)
typedef unsigned long WORD32, *PWORD32;
typedef int SWORD32;
#define TYPE_WORD32
#endif /* WORD32 */

#if !defined(TYPE_bool) && !defined( __cplusplus)
typedef int bool;
#define TYPE_bool
#endif /* bool */


#if !defined(TYPE_n8)
typedef unsigned char n8;
#define TYPE_n8
#endif /* n8 */

#if !defined(TYPE_n16)
typedef unsigned short n16;
#define TYPE_n16
#endif /* n16 */

#if !defined(TYPE_n32)
typedef unsigned int n32;
#define TYPE_n32
#endif /* n32 */

#if !defined(TYPE_n64)
#ifdef _MSC_VER
typedef unsigned __int64 n64;
#else /* !_MSC_VER */
typedef unsigned long long n64;
#endif /* _MSC_VER */
#define TYPE_n64
#endif /* n64 */

#if !defined(TYPE_hmpv_128)
typedef struct { n64 lo, hi; } hmpv_128;
#define TYPE_hmpv_128
#endif /* TYPE_hmpv_128 */

#if !defined(TYPE_hmpv_p132)
typedef struct { n32 rv; hmpv_128 vrv; } hmpv_pl32;
#define TYPE_hmpv_p132
#endif /* TYPE_hmpv_p132 */

#if !defined(TYPE_hmpv_p164)
typedef struct { n64 rv; hmpv_128 vrv; } hmpv_pl64;
#define TYPE_hmpv_p164
#endif /* TYPE_hmpv_p164 */

#define eu8  UINT8
#define eu16 UINT16
#define eu32 UINT32

#if defined(__linux__)
  #if !defined(STATUS)
    #define STATUS int
  #endif
#endif

/*#if !defined(TYPE_n64u)
typedef union {
        n64     n64;

        struct {
                n32 l0, l1;
        } eu32;

        struct {
                long l0, l1;
        } s32;

        struct {
                unsigned short s0, s1, s2, s3;
        } u16;

        struct {
                short s0, s1, s2, s3;
        } s16;

        struct {
                unsigned char c0, c1, c2, c3, c4, c5, c6, c7;
        } u8;

        struct {
            char c0, c1, c2, c3, c4, c5, c6, c7;
        } s8;
} n64u;
#define TYPE_n64u
#endif */

/* 将_BIG_ENDIAN的送据转换成_LITTLE_ENDIAN的宏定义 */
#if _BYTE_ORDER == _BIG_ENDIAN 


#define ltohl(x)	((((x) & 0x000000ff) << 24) | \
			 (((x) & 0x0000ff00) <<  8) | \
			 (((x) & 0x00ff0000) >>  8) | \
			 (((x) & 0xff000000) >> 24))

#define htoll(x)	((((x) & 0x000000ff) << 24) | \
			 (((x) & 0x0000ff00) <<  8) | \
			 (((x) & 0x00ff0000) >>  8) | \
			 (((x) & 0xff000000) >> 24))

#define ltohs(x)	((((x) & 0x00ff) << 8) | \
			 (((x) & 0xff00) >> 8))

#define htols(x)	((((x) & 0x00ff) << 8) | \
			 (((x) & 0xff00) >> 8))

/*#define	LTOHL(x)	ltohl((WORD32)(x))
#define	LTOHS(x)	ltohs((WORD16)(x))
#define	HTOLL(x)	htoll((WORD32)(x))
#define	HTOLS(x)	htols((WORD16)(x))
*/

static inline WORD32 LTOHL(WORD32 x)
{
    return ltohl(x);
}

static inline WORD16 LTOHS(WORD16 x)
{
    return ltohs(x);
}

static inline WORD32 HTOLL(WORD32 x)
{
    return htoll(x);
}

static inline WORD16 HTOLS(WORD16 x)
{
    return htols(x);
}

#else

#define	LTOHL(x)	(WORD32)(x)
#define	LTOHS(x)	(WORD16)(x)
#define	HTOLL(x)	(WORD32)(x)
#define	HTOLS(x)	(WORD16)(x)

#endif	/* _BYTE_ORDER==_LITTLE_ENDIAN */
    

#ifdef __cplusplus
}
#endif

#endif /* __INCeqtTypesh */
