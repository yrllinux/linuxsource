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
#ifndef __INCKDV8018DrvLibh
#define __INCKDV8018DrvLibh


#ifdef __cplusplus 
extern "C" { 
#endif /* __cplusplus */


#include "time.h"
#include "timers.h"

/* 宏定义 */
#define VER_KDV8018VxNIP  ( const char * )"KDV8018VxNIP 30.01.08.11.041016"

/* 灯指示宏定义 */
#define BRD_LED_ON       1  /* 亮 */
#define BRD_LED_OFF      2  /* 灭 */
#define BRD_LED_QUICK    3  /* 快闪 */
#define BRD_LED_SLOW     4  /* 慢闪 */

/* 底层E1告警信息结构定义 */
typedef struct{
    BOOL bAlarmE1RUA1;     /* E1全1告警 */
    BOOL bAlarmE1RLOS;     /* E1失同步告警 */
    BOOL bAlarmE1RRA;      /* E1远端告警 */
    BOOL bAlarmE1RCL;      /* E1失载波告警 */
}TBrdE1AlarmDesc;

/* 终端前面板指示灯状态数据结构 */
typedef struct{
    UINT8 byLedPower;
    UINT8 byLedALM;
    UINT8 byLedRun; 
    UINT8 byLedLINK;
    UINT8 byLedDSP;
    UINT8 byLedETH1;
    UINT8 byLedETH2;
    UINT8 byLedE1ALM;
    UINT8 byLedIR;
}TBrdSMTLedStateDesc;

STATUS BrdSMTQueryLedState(TBrdSMTLedStateDesc *ptBrdSMTLedState);   /* 闪灯状态获取 */


/* 底层告警信息结构定义 */
typedef struct{
    TBrdE1AlarmDesc tBrdE1Alarm;     /* E1告警 */
}TBrdSMTAlarmAll;

STATUS BrdSMTAlarmStateScan(TBrdSMTAlarmAll *ptBrdSMTAlarmAll);

/* 函数声明 */
/*================================时间模块设置====================================*/
/* 时间结构说明 */
/*采用系统标准tm结构，可参考tornado相关帮助文档
int tm_sec;  seconds after the minute  - [0, 59] 
int tm_min;  minutes after the hour    - [0, 59] 
int tm_hour;  hours after midnight     - [0, 23] 
int tm_mday;  day of the month         - [1, 31] 
int tm_mon;  months since January      - [1-1, 12-1]    注意: 0 = 一月，参数需规范后传入
int tm_year;  years since 1900         - [1980-1900,2079-1900]  注意:参数需规范后传入
int tm_wday;  days since Sunday        - [0, 6]   不支持，输入0
int tm_yday;  days since January 1     - [0, 365] 不支持，输入0 
int tm_isdst;  Daylight Saving Time flag [ 0 ]    不支持，输入0

/* 时间函数声明 */

STATUS BrdTimeGet( struct tm* pGettm );             /* 读取RTC时间 */
STATUS BrdTimeSet( struct tm* pSettm );             /* 设置RTC和系统时间 */ 

STATUS BrdLedFlash(UINT8 byLedID, UINT8 byState);   /* 闪灯控制 */

/*==============================硬件复位=============================*/  
void BrdHwReset(void);
STATUS sysRebootHookAdd(VOIDFUNCPTR rebootHook);    /* 复位注册函数 */

/*==============================nip网络设置部分=============================*/
/* 以太网参数结构 */
typedef struct{
    UINT32 dwIpAdrs;
    UINT32 dwIpMask;
    UINT8  byMacAdrs[6];
}TBrdEthParam;

/* BrdSetEthParam函数中byIpOrMac输入值的宏定义 */
#define Brd_SET_IP_AND_MASK    0x01 /* 设置IP地址和子网掩码 */
#define Brd_SET_MAC_ADDR       0x02 /* 设置MAC地址 */
#define Brd_SET_ETH_ALL_PARAM  0x03 /* 设置以太网的IP地址、子网掩码和MAC地址 */

STATUS BrdSetEthParam(UINT8 byEthId, UINT8 byIpOrMac, TBrdEthParam *ptBrdEthParam); /* 设置以太网参数 */
STATUS BrdGetEthParam(UINT8 byEthId, TBrdEthParam *ptBrdEthParam); /* 获取以太网参数 */
STATUS BrdDelEthParam(UINT8 byEthId);           /* 删除指定的以太网接口 */

STATUS BrdSetDefGateway(UINT32 dwIpAdrs);       /* 设置默认网关ip */
UINT32 BrdGetDefGateway(void);                  /* 获取默认网关ip */
STATUS BrdDelDefGateway(void);                  /* 删除默认网关ip */

UINT32 BrdGetNextHopIpAddr(UINT32 dwDstIpAddr); /* 获取通往指定ip的第一跳路由ip地址 */



/* flash分区信息结构定义 */
#define DISK_MAX_NUMBER     8
typedef struct{
    UINT32  dwDiskBaseAddr;       /* flash盘的基地址 */
    UINT32  dwDiskSize;           /* flash盘的容量，以Byte为单位 */
    char diskNameStr[32];         /* flash盘的名称 */
}TBrdDiskInfoDesc;

typedef struct{
    UINT8  byDiskNum;           /* 实际flash盘的个数 */
    TBrdDiskInfoDesc tBrdDiskInfo[DISK_MAX_NUMBER];
}TBrdAllDiskInfo;

STATUS BrdGetAllDiskInfo(TBrdAllDiskInfo *pBrdAllDiskInfo);
UINT32 BrdGetFullFileName(UINT8 byPutDiskId, char *pInFileName, char *pRtnFileName);


/* vga info */
#define BSP15_VGA_NONE             0       /* vga输入输出功能均没有 */
#define BSP15_VGA_IN               1       /* 只有vga输入功能 */
#define BSP15_VGA_OUT              2       /* 只有vga输出功能 */
#define BSP15_VGA_INOUT            3       /* vga输入输出功能都有 */

UINT8  BrdGetBSP15VGAConf(UINT8 byDevId);        /* 获取指定bsp15的vga配置 */
void   BrdStopVGACap(void);                      /* 停止采集vga(8083)数据(控制fpga) */
void   BrdStartVGACap(UINT8 byMode);             /* 开始采集vga(8083)数据(控制fpga) */
void   BrdSetVGACapMode(UINT8 byMode);           /* 设置fpga中VGA采集的模式(配置fpga) */
void   BrdCloseVGA(void);                        /* 关闭并禁止vga(8083)采集芯片工作 */
void   BrdOpenVGA(void);                         /* 打开并允许vga(8083)采集芯片工作 */



#ifdef __cplusplus 
} 
#endif /* __cplusplus */

#endif   /*INCDriverInith*/
