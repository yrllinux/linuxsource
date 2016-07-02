/* rvstdio.h - This file replaces the ANSI stdio.h for portability. */
/************************************************************************
        Copyright (c) 2001 RADVISION Inc. and RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Inc. and RADVISION Ltd.. No part of this document may be
reproduced in any form whatsoever without written prior approval by
RADVISION Inc. or RADVISION Ltd..

RADVISION Inc. and RADVISION Ltd. reserve the right to revise this
publication and make changes without obligation to notify any person of
such revisions or changes.
***********************************************************************/

#if !defined(RV_STDIO_H)
#define RV_STDIO_H

#include "rvccore.h"
#include "rvansi.h"
#include "rverror.h"

#ifdef __cplusplus
extern "C" {
#endif


#if !defined(RV_STDIO_TYPE) || ((RV_STDIO_TYPE != RV_STDIO_ANSI) && \
    (RV_STDIO_TYPE != RV_STDIO_OSE_DEBUG) && \
    (RV_STDIO_TYPE != RV_STDIO_STUB) && \
    (RV_STDIO_TYPE != RV_STDIO_WINCE_DEBUG))
#error RV_STDIO_TYPE not set properly
#endif

/********************************************************************************************
 * RvStdioInit - Initializes the stdio module.
 *
 * Must be called once (and only once) before any other functions in the module are called.
 *
 * INPUT   : none
 * OUTPUT  : none
 * RETURN  : Always RV_OK
 */
RvStatus Rv2StdioInit(void);

/********************************************************************************************
 * RvStdioEnd - De-initializes the stdio module.
 *
 * INPUT   : none
 * OUTPUT  : none
 * RETURN  : Always RV_OK
 */
RvStatus Rv2StdioEnd(void);

#if (RV_STDIO_TYPE == RV_STDIO_ANSI) || \
    (RV_STDIO_TYPE == RV_STDIO_OSE_DEBUG) || \
    (RV_STDIO_TYPE == RV_STDIO_WINCE_DEBUG)

#include <stdio.h>

/* Some of the OS's must have ctype.h for isspace(), tolower(), isdigit(), etc. */
#include <ctype.h>


#if (RV_OS_TYPE != RV_OS_TYPE_WINCE) && \
    (RV_OS_TYPE != RV_OS_TYPE_VXWORKS) && \
    (RV_OS_TYPE != RV_OS_TYPE_PSOS) && \
    (RV_OS_TYPE != RV_OS_TYPE_OSE) && \
    (RV_OS_TYPE != RV_OS_TYPE_NUCLEUS) && \
    (RV_OS_TYPE != RV_OS_TYPE_SYMBIAN) && \
	!((RV_OS_TYPE == RV_OS_TYPE_MOPI) && !defined(_WIN32))
/* nucleus, VxWorks, pSOS, WinCE & OSE keeps 'qsort()' in stdlib.h */
/* MOPI platform doesn't have 'search.h' */
#include <search.h>     /* for 'qsort()' */
#endif

#if (RV_OS_TYPE == RV_OS_TYPE_TRU64) || \
    (RV_OS_TYPE == RV_OS_TYPE_LINUX) || \
    (RV_OS_TYPE == RV_OS_TYPE_UNIXWARE)
#include <string.h>
#include <stdlib.h> /* TRU64 keeps qsort() in stdlib.h */
#endif



/* Data Types */

/* FILE - represents a ANSI stdio FILE type for streams. */
#define RvFILE     FILE

/* RvFpos_t - represents a ANSI stdio fpos_t type for file position */
#define RvFpos_t   fpos_t
#define RvL_tmpnam L_tmpnam

/* Standard Streams */
#define RvStderr stderr
#define RvStdout stdout
#define RvStdin  stdin

/* Constants */
#define RvEOF    EOF

/* Standard char operations for old WinCE versions */
#if (RV_OS_TYPE == RV_OS_TYPE_WINCE) && (RV_OS_VERSION == RV_OS_WINCE_2_11)
void CharToWChar(RvChar CharIn, WCHAR * wCharOut);
/********************************************************************************************
 * isupper - checks if input character is an uper case letter
 *
 * INPUT   : CharIn		- character to be checked
 * OUTPUT  : none
 * RETURN  : returns a non-zero value if CharIn is an uppercase character 
 */
RVCOREAPI 
RvInt RVCALLCONV __cdecl isupper(
	IN RvInt CharIn);


/********************************************************************************************
 * islower - checks if input character is an lower case letter
 *
 * INPUT   : CharIn		- character to be checked
 * OUTPUT  : none
 * RETURN  : returns a non-zero value if CharInc is a lowercase character  
 */
RVCOREAPI 
RvInt RVCALLCONV __cdecl islower(
	IN RvInt CharIn);


/********************************************************************************************
 * isspace - checks if input character is a white-space character
 *
 * INPUT   : CharIn		- character to be checked
 * OUTPUT  : none
 * RETURN  : returns a non-zero value if CharIn is a white-space character   
 */
RVCOREAPI 
RvInt RVCALLCONV __cdecl isspace(
	IN RvInt CharIn);


/********************************************************************************************
 * isdigit - checks if input character is a digit character
 *
 * INPUT   : CharIn		- character to be checked
 * OUTPUT  : none
 * RETURN  : returns a non-zero value if CharIn is a digit character   
 */
RVCOREAPI 
RvInt RVCALLCONV __cdecl isdigit(
	IN RvInt CharIn);

/********************************************************************************************
 * isalnum - checks if input character is a alphanumeric character
 *
 * INPUT   : CharIn		- character to be checked
 * OUTPUT  : none
 * RETURN  : returns a non-zero value if CharIn is a alphanumeric character   
 */
RVCOREAPI 
RvInt RVCALLCONV __cdecl isalnum(
	IN RvInt CharIn);

/********************************************************************************************
 * strspn - Find the first substring
 *
 * INPUT   : s1		- Null-terminated string to search 
 *			 s2		- Null-terminated character set 
 * OUTPUT  : none
 * RETURN  : an integer value specifying the length of the substring in s1   
 */
RVCOREAPI 
RvSize_t RVCALLCONV __cdecl strspn(
	IN const RvChar * s1, 
	IN const RvChar * s2);

#endif

/* Sorting functions */
#define RvQsort qsort

/* File Operation */
#if (RV_OS_TYPE != RV_OS_TYPE_NUCLEUS) && (RV_OS_TYPE != RV_OS_TYPE_PSOS)
#define RvFopen   fopen
#define RvFreopen freopen
#define RvFlush   flush
#define RvFclose  fclose
#define RvRemove  remove
#define RvRename  rename
#define RvTmpfile tmpfile
#define RvTmpnam  tmpnam
#define RvSetvbuf setvbuf
#define RvSetbuf  setbuf
#else
#define RvFopen(a, b) NULL
#define RvFclose(a) RvEOF
#endif

/* Formatted Output */
#define Rv2Vprintf vprintf
#define Rv2Vfprintf vfprintf
#define Rv2Fprintf fprintf
#if (RV_STDIO_TYPE == RV_STDIO_OSE_DEBUG)
#include <dbgprintf.h>
#define Rv2Printf dbgprintf
#elif (RV_STDIO_TYPE == RV_STDIO_WINCE_DEBUG)

/********************************************************************************************
 * RvOutputDebugPrintf - De-initializes the stdio module.
 *
 * INPUT   : format		- printf style format
 *			 ...		- printf style parameters that match format
 * OUTPUT  : none
 * RETURN  : lenght of string that was printed
 */
RVCOREAPI 
RvInt RVCALLCONV Rv2OutputDebugPrintf(
	IN const RvChar		*format,
	IN					...);

#define Rv2Printf Rv2OutputDebugPrintf
#else
#define Rv2Printf printf
#define Rv2OutputDebugPrintf( a ) OutputDebugString((LPCTSTR)a)
#endif

/* Formatted Input */
#define Rv2Fscanf fscanf

/* Character Input ans Output Functions */
#define Rv2Fgetc fgetc
#define Rv2Fgets fgets
#define Rv2Fputc fputc
#define Rv2Fputs fputs
#define Rv2Getc getc
#define Rv2Getchar getchar
#define Rv2Gets gets
#define Rv2Putc putc
#define Rv2Putchar putchar
#define Rv2Puts puts
#define Rv2Ungetc ungetc

/* Direct Input and Output Functions */
#define Rv2Fread  fread
#define Rv2Fwrite fwrite

/* File Positioning Functions */
#if (RV_OS_TYPE != RV_OS_TYPE_NUCLEUS) && (RV_OS_TYPE != RV_OS_TYPE_PSOS)
#define Rv2Fseek   fseek
#define Rv2Ftell   ftell
#define Rv2Rewind  rewind
#define Rv2Fgetpos fgetpos
#define Rv2Fsetpos fsetpos
#else
RvInt Rv2Fseek(RvFILE *stream, long offset, RvInt origin);
#define Rv2Ftell(a)       -1L
#endif

/* Error Functions */
#define Rv2Clearerror clearerror
#define Rv2Feof       feof
#define Rv2Ferror     ferror
#define Rv2Perror     perror

#elif (RV_STDIO_TYPE == RV_STDIO_STUB)
#include <stdarg.h>

/* Data Types */
#define RvFILE    RvInt
#define RvFpos_t  RvInt

/* Standard Streams */
#define RvStderr (RvFILE*)0
#define RvStdout (RvFILE*)1
#define RvStdin  (RvFILE*)2

/* Constants */
#define RvEOF -1
#define RvL_tmpnam 32

/* Sorting functions */
#define RvQsort qsort

/* File Operation */
RvFILE* RvFopen(const RvChar* filename, const RvChar* mode);
RvFILE* RvFreopen(const RvChar* filename, const RvChar* mode, RvFILE* stream);
RvInt RvFlush(RvFILE* stream);
RvInt RvFclose(RvFILE* stream);
RvInt RvRemove(const RvChar* filename);
RvInt RvRename(const RvChar* oldname, const RvChar* newname);
RvFILE* RvTmpfile(void);
RvChar* RvTmpnam(RvChar s[RvL_tmpnam]);
RvInt RvSetvbuf(RvFILE stream, RvChar* buf, RvInt mode, RvSize_t size);
void RvSetbuf(RvFILE stream, RvChar* buf);

/* Formatted Output */

/********************************************************************************************
 * RvPrintf - This function implements an ANSI printf style function.
 *
 * INPUT   : format	- A fprintf style format string.
 *			 ...	- A variable argument list.
 * OUTPUT  : none
 * RETURN  : The number of characters written, or negative if an error occured.   
 */
RVCOREAPI 
RvInt RVCALLCONV Rv2Printf(
	IN const RvChar	*format, 
	INT				...);

/********************************************************************************************
 * RvFprintf - This function implements an ANSI fprintf style function.
 *
 * INPUT   : stream	- The stream to which to write 
 *			 format	- A fprintf style format string.
 *			 ...	- A variable argument list.
 * OUTPUT  : none
 * RETURN  : The number of characters written, or negative if an error occured.   
 */
RVCOREAPI RvInt RVCALLCONV Rv2Fprintf(
	IN RvFILE			* stream, 
	IN const RvChar		*format, 
	IN					...);

/********************************************************************************************
 * RvVprintf - This function implements an ANSI vprintf style function.
 *
 * INPUT   : format	- A fprintf style format string.
 *			 ...	- A variable argument list.
 * OUTPUT  : none
 * RETURN  : The number of characters written, or negative if an error occured.   
 */
RVCOREAPI RvInt RVCALLCONV Rv2Vprintf(
	IN const RvChar	*format,
	IN va_list		arg);

/********************************************************************************************
 * RvVfprintf - This function implements an ANSI vfprintf style function.
 *
 * INPUT   : stream - The stream to which to write
 *			 format - A vfprintf style format string.
 *			 arg	- A variable argument list.
 * OUTPUT  : none
 * RETURN  : The number of characters written, or negative if an error occured.
 */
RVCOREAPI RvInt RVCALLCONV Rv2Vfprintf(
	IN RvFILE		*stream, 
	IN const RvChar	*format,
	IN va_list		arg);

/* Formatted Input */
/********************************************************************************************
 * RvFscanf - This function implements an ANSI fscanf style function.
 *
 * INPUT   : stream	- The stream from which to read input.
 *			 format	- A sscanf style format string.
 *			 ...	- A variable argument list.
 * OUTPUT  : none
 * RETURN  : The number input items converted, or RvEOF if an error occured.   
 */
RvInt Rv2Fscanf(
	IN RvFILE		*stream, 
	IN const RvChar	*format,
	INT				...);

/********************************************************************************************
 * RvScanf - This function implements an ANSI scanf style function.
 *
 * INPUT   : format	- A sscanf style format string.
 *			 ...	- A variable argument list.
 * OUTPUT  : none
 * RETURN  : The number input items converted, or RvEOF if an error occured.   
 */
RvInt Rv2Scanf(
	IN const RvChar	*format,
	INT				...);

/* Character Input ans Output Functions */
RvInt Rv2Fgetc(RvFILE* stream);
RvChar* Rv2Fgets(RvChar* s, RvInt n, RvFILE* stream);
RvInt Rv2Fputc(RvInt c, RvFILE* stream);
RvInt Rv2Fputs(const RvChar* s, RvFILE* stream);
RvInt Rv2Getc(RvFILE* stream);
RvInt Rv2Getchar(void);
RvChar* Rv2Gets(RvChar* s);
RvInt Rv2Putc(RvInt c, RvFILE* stream);
RvInt Rv2Putchar(RvInt c);
RvInt Rv2Puts(const RvChar* s, RvFILE* stream);
RvInt Rv2Ungetc(RvInt c, RvFILE* stream);

/* Direct Input and Output Functions */
RvSize_t Rv2Fread(void* ptr, RvSize_t size, RvSize_t nobj, RvFILE* stream);
RvSize_t Rv2Fwrite(const void* ptr, RvSize_t size, RvSize_t nobj, RvFILE* stream);

/* File Positioning Functions */
RvInt Rv2Fseek(RvFILE* stream, long offset, RvInt origin);
long Rv2Ftell(RvFILE* stream);
void Rv2Rewind(RvFILE* stream);
RvInt Rv2Fgetpos(RvFILE* stream, RvFpos_t* ptr);
RvInt Rv2Fsetpos(RvFILE* stream, const RvFpos_t* ptr);

/* Error Functions */
void Rv2Clearerror(RvFILE* stream);
RvInt Rv2Feof(RvFILE* stream);
RvInt Rv2Ferror(RvFILE* stream);
void Rv2Perror(const RvChar* s);

#endif

/********************************************************************************************
 * RvBsearch - Binary search function
 *
 * INPUT   : key
 *			 base
 *			 numOfElements
 *			 elementSize
 *			 compareFunc
 * OUTPUT  : none
 * RETURN  : pointer to found element or NULL
 */
RVCOREAPI void* RVCALLCONV Rv2Bsearch(
    IN const void*     key,
    IN const void*     base,
    IN RvSize_t        numOfElements,
    IN RvSize_t        elementSize,
    IN RvInt (*compareFunc)(const void* key, const void* ));

#ifdef __cplusplus
}
#endif

#endif  /* Include guard */

