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
#ifndef __INCDriDrvLibh
#define __INCDriDrvLibh


#ifdef __cplusplus 
extern "C" { 
#endif /* __cplusplus */

#include "time.h"
#include "timers.h"

/* 宏定义 */
#define VER_DRIVxNIP  ( const char * )"DRIVxNIP 30.01.07.08.041016"


/* 底层E1告警信息结构定义 */
typedef struct{
    BOOL bAlarmE1RUA1;     /* E1全1告警 */
    BOOL bAlarmE1RLOS;     /* E1失同步告警 */
    BOOL bAlarmE1RRA;      /* E1远端告警 */
    BOOL bAlarmE1RCL;      /* E1失载波告警 */
    BOOL bAlarmE1RCMF;     /* E1的CRC复帧错误告警 */
    BOOL bAlarmE1RMF;      /* E1随路复帧错误告警 */
}TBrdE1AlarmDesc;

typedef struct{
    TBrdE1AlarmDesc tBrdE1AlarmDesc[8];
}TBrdAllE1Alarm;

/* 指示灯状态数据结构 */
typedef struct{
    UINT8 byLedRun; 
    UINT8 byLedE1_ALM0;
    UINT8 byLedE1_ALM1;
    UINT8 byLedE1_ALM2;
    UINT8 byLedE1_ALM3;
    UINT8 byLedE1_ALM4;
    UINT8 byLedE1_ALM5;
    UINT8 byLedE1_ALM6;
    UINT8 byLedE1_ALM7;
    UINT8 byLedMLINK;
    UINT8 byLedLNK0;
    UINT8 byLedLNK1;
}TBrdDRILedStateDesc;


/* 函数声明 */
STATUS BrdTimeGet( struct tm* pGettm );        /* 读取系统时间 */
STATUS BrdTimeSet( struct tm* pSettm );        /* 设置系统时间 */ 

STATUS BrdDRIAlarmE1StateScan(TBrdAllE1Alarm *ptBrdAllE1Alarm); /* DRI链路告警函数声明 */

STATUS BrdDRISetE1ChainMode(UINT8 byE1ID, UINT8 mode);          /* E1链路使用开关设置 */
STATUS BrdDRISetRelayLoop(UINT8 byE1ID, UINT8 mode);            /* E1链路设置继电器环回（内环） */
STATUS BrdDRISelectNetSyncClk(UINT8 byE1ID);                    /* DRI上网同步参考时钟输出选择 */
STATUS BrdDRISetNetSyncMode(UINT8 mode);                        /* DRI上网同步时钟输出开关 */
UINT8 BrdQueryE1Imp(UINT8 byE1ID);                              /* 取e12阻抗值 */

STATUS BrdDRIQueryLedState(TBrdDRILedStateDesc *ptBrdDRILedState);  /* 闪灯模式查询 */




#ifdef __cplusplus 
} 
#endif /* __cplusplus */

#endif   /*INCDriverInith*/
