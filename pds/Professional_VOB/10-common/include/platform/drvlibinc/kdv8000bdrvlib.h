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
#ifndef __INCKdv8000bDrvLibh
#define __INCKdv8000bDrvLibh


#ifdef __cplusplus 
extern "C" { 
#endif /* __cplusplus */

#include "time.h"
#include "timers.h"

/* 宏定义 */
#define VER_KDV8000BVxNIP  ( const char * )"KDV8000BVxNIP 30.01.02.04.041016"


/* 指示灯状态数据结构 */
typedef struct{
    UINT8 byLedRun; 
    UINT8 byLedLNK0;
    UINT8 byLedLNK1;
}TBrdKDV8000BLedStateDesc;


/* 函数声明 */
STATUS BrdTimeGet( struct tm* pGettm );        /* 读取系统时间 */
STATUS BrdTimeSet( struct tm* pSettm );        /* 设置系统时间 */ 

STATUS BrdKDV8000BQueryLedState(TBrdKDV8000BLedStateDesc *ptBrdKDV8000BLedState);  /* 闪灯模式查询 */




#ifdef __cplusplus 
} 
#endif /* __cplusplus */

#endif   /* INCKdv8000bDrvLibh */
