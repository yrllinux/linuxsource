/*****************************************************************************
模块名      : mcu
文件名      : vasDrvLib.h
相关文件    : drvMpcControl.c
文件实现功能:  内部使用的一些宏定义和函数声明.
作者        : 张方明
版本        : V1.0  Copyright(C) 2003-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2003/8/19   01a         张方明      创建	
******************************************************************************/
#ifndef __INCVasDrvLibh
#define __INCVasDrvLibh


#ifdef __cplusplus 
extern "C" { 
#endif /* __cplusplus */

#include "time.h"
#include "timers.h"

/* 宏定义 */
#define VER_VASVxNIP  ( const char * )"VASVxNIP 30.01.07.07.041016"

/* 指示灯状态数据结构 */
typedef struct{
    UINT8 byLedRUN; 
    UINT8 byLedALM;
    UINT8 byLedMLNK;
    UINT8 byLedETHLNK;
}TBrdVASLedStateDesc;

/* 函数声明 */
STATUS BrdTimeGet( struct tm* pGettm );        /* 读取RTC时间 */
STATUS BrdTimeSet( struct tm* pSettm );        /* 设置RTC和系统时间 */ 

UINT8 BrdGetMatrixType(void);                    /* 矩阵板类型查询 */
void  BrdVideoMatrixSet(UINT8 vedioInSelect, UINT8 vedioOutSelect); /* 视频矩阵设置 */
void  BrdAudioMatrixSet(UINT8 audioInSelect, UINT8 audioOutSelect); /* 音频矩阵设置 */
STATUS BrdVASQueryLedState(TBrdVASLedStateDesc *ptBrdVASLedStateDesc);

#ifdef __cplusplus 
} 
#endif /* __cplusplus */

#endif   /*INCDriverInith*/
