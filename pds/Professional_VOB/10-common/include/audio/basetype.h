/**                                                                            
*** Copyright (c) 2002-2003 Keda Communication Co.LTD.                         
**/

// Name    : Basetype.h 


#ifndef BASETYPE_HEADFILE
#define BASETYPE_HEADFILE

/*We can define the following flags to optimize our code

  OPTIMIZE_FOR:

  not defined--> ANSI C
  0--> ANSI C, no float
  1--> Intel MMX
  2--> Intel SSE
  3--> Intel SSE2
  100--> Equator MAP-CA

*/

/* basic types: according to C99 <stdint.h>, <stdbool.h>
*/
#ifndef NULL
  #ifdef  __cplusplus
    #define NULL    0
  #else
    #define NULL    ((void *)0)
  #endif
#endif

typedef char int8_t; 
typedef unsigned char uint8_t;
typedef short int16_t; 
typedef unsigned short uint16_t; 
typedef long int32_t;
typedef unsigned long uint32_t;

#ifdef WIN32
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t; 
#else
typedef long long int64_t; 
typedef unsigned long long uint64_t;
#endif

#if !defined(TYPE_SCODE)
typedef int SCODE, *PSCODE;
#define TYPE_SCODE
#endif /* SCODE */

#if !defined(TYPE_INT8) && !defined(__INCvxTypesh)
typedef char INT8, *PINT8;
#define TYPE_INT8
#endif /* INT8 */

#if !defined(TYPE_INT16) && !defined(__INCvxTypesh)
typedef short INT16, *PINT16;
#define TYPE_INT16
#endif /* INT16 */

#if !defined(TYPE_INT32) && !defined(__INCvxTypesh)
typedef int INT32, *PINT32;
#define TYPE_INT32
#endif /* INT32 */

#if !defined(TYPE_INT64)
#ifdef _MSC_VER
typedef __int64 INT64, *PINT64;
#else /* !_MSC_VER */
typedef long long INT64, *PINT64;
#endif /* _MSC_VER */
#define TYPE_INT64
#endif /* INT64 */

#if !defined(TYPE_UINT8) && !defined(__INCvxTypesh)
typedef unsigned char UINT8, *PUINT8;
#define TYPE_UINT8
#endif /* UINT8 */

#if !defined(TYPE_UINT16) && !defined(__INCvxTypesh)
typedef unsigned short UINT16, *PUINT16;
#define TYPE_UINT16
#endif /* UINT16 */

#if !defined(TYPE_UINT32) && !defined(__INCvxTypesh)
typedef unsigned int UINT32, *PUINT32;
#define TYPE_UINT32
#endif /* UINT32 */

#if !defined(TYPE_INT) && !defined(__INCvxTypesh)
typedef int INT, *PINT;
#define TYPE_INT
#endif /* INT */

#if !defined(TYPE_UINT) && !defined(__INCvxTypesh)
typedef unsigned int UINT, *PUINT;
#define TYPE_UINT
#endif /* UINT */

#if !defined(TYPE_UINT64)
#ifdef _MSC_VER
typedef unsigned __int64 UINT64, *PUINT64;
#else /* !_MSC_VER */
typedef unsigned long long UINT64, *PUINT64;
#endif /* _MSC_VER */
#define TYPE_UINT64
#endif /* UINT64 */

#if !defined(TYPE_BOOL) && !defined(__INCvxTypesh)
typedef int BOOL, *PBOOL;
#define TRUE 1
#define FALSE 0
#define TYPE_BOOL
#endif /* BOOL */

#if !defined(TYPE_BYTE)
typedef unsigned char BYTE, *PBYTE;
#define TYPE_BYTE
#endif /* BYTE */

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

//the below DWORD and WORD define by cxz for easy use BMP bmp define.
#if !defined(TYPE_DWORD)
typedef unsigned long       DWORD;
#define TYPE_DWORD
#endif

#if !defined(TYPE_WORD)
typedef unsigned short      WORD;
#define TYPE_WORD
#endif

#if !defined(TYPE_LONG)
typedef long LONG;
#define TYPE_LONG
#endif

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

#if !defined(TYPE_n64u)
typedef union {
        n64     n64;

        struct {
                n32 l0, l1;
        } u32;

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
            /* Note that char is signed on HMPV and i386
             */
            char c0, c1, c2, c3, c4, c5, c6, c7;
        } s8;
} n64u;
#define TYPE_n64u
#endif /* TYPE_n64u */

#ifndef VOLATILE
#define	VOLATILE volatile_noncached
#endif

#define ETI_IFPRED() if_pred
#define ETI_IFPROB(x) if_prob(x)
#define ETI_UNROLLAMOUNT(x) unroll_amount(x)
#define ETI_ENDTRACE() hmpv_endtrace()
#define RESTRICT restrict
#define VOLATILE volatile_noncached

#ifdef	DEBUG
#define	FIRM_WARE_PRINTF(x) kprintf(x)
#else
#define	FIRM_WARE_PRINTF(x)
#endif

#define		FLOAT					float

#endif //BASETYPE_HEADFILE
