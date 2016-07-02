/* eqtBrdLib.h - Equator Board API Header */ 
 
/* Copyright(C) 2001-2002 KDC, All rights reserved. */ 
 
/*  
modification history 
-------------------- 
01a,13jan2003,xf  written. 
*/

/*
This file contains some constants and routine declarations used by upper level.
*/

#ifndef __INCeqtBrdLibh
#define __INCeqtBrdLibh

#ifdef __cplusplus 
extern "C" { 
#endif /* __cplusplus */

#include "eqttypes.h"

typedef void* HBoard;                    /* handle to an ETI board */


/* 与Equator板卡相关的宏定义 */
#define ETI_BOARD_DEFAULT   0x0

#define MAX_BRD_NUM         (BYTE)16    /* 最大支持的Equator板卡的数量 */

/* Return codes */
#define ETI_BOARD_SUCCESS  0
#define ETI_BOARD_FAIL    -1

/* video capture from port A/B/NONE */
#define BSP15_VCAPTURE_FROM_NONE   0
#define BSP15_VCAPTURE_FROM_PORTA  1
#define BSP15_VCAPTURE_FROM_PORTB  2
#define BSP15_VCAPTURE_FROM_PORTAB 3

/* SAA7114 vedio output port X/I */
#define SAA7114_OUTPORT_NONE       0
#define SAA7114_OUTPORT_X          1
#define SAA7114_OUTPORT_I          2
#define SAA7114_OUTPORT_XI         3

/* 
 * function prototypes 
 */
HBoard    EqtBrdOpen(BYTE byBrdNum, WORD32 dwCoreSpd, WORD32 dwMemSpd, WORD32 dwMemSize);
SWORD32   EqtGetMemSize (HBoard Handle);
SWORD32   EqtSetMemSize (HBoard Handle, WORD32 dwSize);
SWORD32   EqtBrdLoadFile(HBoard Handle, WORD32 dwOffSet, BYTE* pbyFilename);
SWORD32   EqtAppRun (HBoard Handle, WORD32 dwOffset);
SWORD32   EqtBrdClose (HBoard Handle);
SWORD32   EqtBrdWriteSdram (HBoard Handle, WORD32 dwOffset, const BYTE *pbyData, WORD32 dwLen);
SWORD32   EqtBrdReadSdram (HBoard Handle, WORD32 dwOffset, BYTE *pbyData, WORD32 dwLen);
SBYTE     EqtGetBrdNo (HBoard hBoard);
SBYTE     EqtFindBrd (BYTE byBusNo, BYTE byDevNo, BYTE byFuncNo);

HBoard     EqtGetHBrd (BYTE byBrdNo);
UINT8     BrdGetBSP15CapturePort(UINT8 byBSP15ID);
UINT8     BrdGetSAA7114OutPort(UINT8 byBSP15ID);
UINT32    BrdBsp15GpdpIsUsed(void);

#ifdef __cplusplus 
} 
#endif /* __cplusplus */

#endif /* __INCeqtBrdLibh */


