/*

NOTICE:
This document contains information that is proprietary to RADVISION LTD..
No part of this publication may be reproduced in any form whatsoever without
written prior approval by RADVISION LTD..

RADVISION LTD. reserves the right to revise this publication and make changes
without obligation to notify any person of such revisions or changes.

*/

/************************************************************************
 *
 *
 *              Windows OS
 *
 *
 ************************************************************************/

#ifndef __RVDEFWIN_H
#define __RVDEFWIN_H

#ifdef __cplusplus
extern "C" {
#endif



#include <memory.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/************************************************************************
 * Function charecteristics definitions
 ************************************************************************/
#define CDECL
#define RVAPI       __declspec(dllexport)

/*del by eric 2003-1-16
#define RVCALLCONV  __stdcall
#define CALLCONVC   __cdecl
*/

#define RVCALLCONV
#define CALLCONVC




/************************************************************************
 * Type definitions
 ************************************************************************/
typedef unsigned long   UINTPTR;
typedef   signed long   INTPTR;

typedef unsigned long   ULONG;
typedef unsigned long   DWORD;
typedef unsigned char   UCHAR;
typedef unsigned short  u_short;

typedef unsigned int    UINT32;
typedef unsigned short  UINT16;
typedef unsigned char   UINT8;
typedef signed   int    INT32;
typedef signed   short  INT16;
typedef signed   char   INT8;

typedef unsigned int    UINT;
typedef signed   int    INT;

typedef unsigned char   BYTE;

#ifndef TRUE
typedef unsigned  BOOL;
#endif



/************************************************************************
 * Alignment macro
 ************************************************************************/
#define RV_ALIGN(numBytes) (((numBytes + 3) / 4) * 4)


/* todo: comment */
/*#define CALL_TIME*/
#ifdef CALL_TIME
typedef struct
{
    __int64 time;
    int     number;
    int     param;
    int     len;
    char    funct[20];
} callTimeStructT;

typedef struct
{
    callTimeStructT callTimeStruct[2000];
    int             callTimeNum;
} callTimeT;

__int64 callTimeGetTime() ;
__int64  callTimeGetFr();
extern callTimeT callTime;

#endif






/************************************************************************
 * Warnings: We disable some of the warnings, as they are a little
 * useless
 * 4706 - assignment within conditional expression (level 4)
 * 4514 - unreferenced inline/local function has been removed (level 4)
 * 4310 - cast truncates constant value (level 3)
 * 4127 - conditional expression is constant (level 4)
 * 4206 - nonstandard extension used : translation unit is empty (level 4)
 * 4711 - function selected for automatic inline expansion
 ************************************************************************/
#pragma warning (disable : 4706 4514 4310 4127 4206 4711)



#ifdef __cplusplus
}
#endif

#endif  /* __RVDEFWIN_H */

