/*****************************************************************************
模块名      : mcu
文件名      : mcuDrvLibh.h
相关文件    : drvMpcControl.c
文件实现功能:  mcu使用的所有宏定义和部分公共函数的声明.
作者        : 张方明
版本        : V1.0  Copyright(C) 2003-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2003/8/19   01a         张方明      创建	
******************************************************************************/
#ifndef __INCmcuDrvLibh
#define __INCmcuDrvLibh


#ifdef __cplusplus 
extern "C" { 
#endif /* __cplusplus */

#ifdef IS_DSL8000_BRD_MPC
#include "DrvLibInc/mpc/drvMpcAlarm.h"
#include "DrvLibInc/mpc/drvMpcStateQuery.h"
#include "DrvLibInc/mpc/drvMpcControl.h"
#endif

#ifdef IS_DSL8000_BRD_CRI
#include "DrvLibInc/cri/drvCriAlarm.h"
#include "DrvLibInc/cri/drvCriStateQuery.h"
#include "DrvLibInc/cri/drvCriControl.h"
#endif

#ifdef IS_DSL8000_BRD_DTI
#include "E:/software/implement_h323/Driver/DrvDTI_860/DtiDrvLib/include/drvDtiAlarm.h"
#include "E:/software/implement_h323/Driver/DrvDTI_860/DtiDrvLib/include/drvDtiStateQuery.h"
#include "E:/software/implement_h323/Driver/DrvDTI_860/DtiDrvLib/include/drvDtiControl.h"
#include "E:/software/implement_h323/Driver/DrvDTI_860/DtiDrvLib/include/drvDtiInit.h"
#endif

#ifdef IS_DSL8000_BRD_DIC
#include "E:/software/implement_h323/Driver/DrvDIC_860/DicDrvLib/include/drvDicAlarm.h"
#include "E:/software/implement_h323/Driver/DrvDIC_860/DicDrvLib/include/drvDicStateQuery.h"
#include "E:/software/implement_h323/Driver/DrvDIC_860/DicDrvLib/include/drvDicControl.h"
#include "E:/software/implement_h323/Driver/DrvDIC_860/DicDrvLib/include/drvDicInit.h"
#endif

#ifdef IS_DSL8000_BRD_VAS
#include "DrvLibInc/vasDrvLib.h"
#endif

#ifdef IS_DSL8000_BRD_MMP
#include "DrvLibInc/mmpDrvLib.h"
#endif

#ifdef IS_DSL8000_BRD_DRI
#include "DrvLibInc/driDrvLib.h"
#endif

#ifdef IS_DSL8000_BRD_IMT
#include "DrvLibInc/imtDrvLib.h"
#endif

#ifdef IS_DSL8000_BRD_APU
#include "DrvLibInc/apuDrvLib.h"
#endif

#ifdef IS_DSL8000_BRD_DSI
#include "DrvLibInc/dsiDrvLib.h"
#endif

#ifdef IS_KDV8000B_BOARD
#include "DrvLibInc/kdv8000bDrvLib.h"
#endif


/* +++++++++++++++++++++++++++++ 宏定义 +++++++++++++++++++++++++++++++++++++++*/
/* =========================== 公共宏定义 =====================================*/
/*  -> 单板种类ID号宏定义 */  
#define  DSL8000_BRD_MPC      0            /* MPC板 */
#define  DSL8000_BRD_DTI      1            /* DTI板 */
#define  DSL8000_BRD_DIC      2            /* DIC板 */
#define  DSL8000_BRD_CRI      3            /* CRI板 */
#define  DSL8000_BRD_VAS      4            /* VAS板 */
#define  DSL8000_BRD_MMP      5            /* MMP板 */
#define  DSL8000_BRD_DRI      6            /* DRI板 */
#define  DSL8000_BRD_IMT      7            /* IMT板 */
#define  DSL8000_BRD_APU      8            /* APU板 */
#define  DSL8000_BRD_DSI      9            /* DSI板 */
#define  DSL8000_BRD_VPU      10           /* VPU板 */
#define  DSL8000_BRD_DEC5     11           /* DEC5板 */
#define  DSL8000_BRD_VRI      12           /* VRI板 */
#define  KDV8000B_BOARD       13           /* KDV8000B板 */
#define  DSL8000_BRD_MPU      66           /* MPU板 */  //与brdwrapperdef.h里的值保持一致 
#define  DSL8000_BRD_EBAP     67           /* EBAP板 */
#define  DSL8000_BRD_EVPU     68           /* EVPU板 */
#define  DSL8000_BRD_HDU      70           /* HDU板 */

#define BRD_LED_ON                    1             /* 亮 */
#define BRD_LED_OFF                   2             /* 灭 */
#define BRD_LED_QUICK                 3             /* 快闪 */
#define BRD_LED_SLOW                  4             /* 慢闪 */

#define E1_75_OHM   0              /* E1模块阻抗为75  ohm */
#define E1_120_OHM  1              /* E1模块阻抗为120 ohm */
#define E1_ERR_OHM  0xff           /* E1模块阻抗无法识别 */

/* BrdSetEthParam函数中byIpOrMac输入值的宏定义 */
#define Brd_SET_IP_AND_MASK    0x01 /* 设置IP地址和子网掩码 */
#define Brd_SET_MAC_ADDR       0x02 /* 设置MAC地址 */
#define Brd_SET_ETH_ALL_PARAM  0x03 /* 设置以太网的IP地址、子网掩码和MAC地址 */

/* 用户程序启动参数设置 */
#define UNCOMPRESS_FILE     0
#define COMPRESS_FILE       1

#define OS_CRI_FOR_MP       0
#define OS_CRI_FOR_GK       1

/* flash分区信息结构定义 */
#define DISK_MAX_NUMBER     8

/* ============================ MPC板相关的宏定义 ============================ */
/* -> 单板灯宏定义 */
#define BRD_MPC_ALARM_LED                 1          /* 系统告警灯 */
#define BRD_MPC_SYN_LED                   2          /* 网同步指示灯 */
#define BRD_MPC_OLAM_LED                  3          /* 光模块告警灯 */

/* 灯板宏定义 */
#define LED_BOARD_LED_NORM          0x01   /* 灯板上正常指示灯 */
#define LED_BOARD_LED_NALM          0x02   /* 灯板上一般告警指示灯 */
#define LED_BOARD_LED_SALM          0x04   /* 灯板上严重告警指示灯 */

#define LED_BOARD_LED_ON            0x01   /* 灯板指示灯亮 */
#define LED_BOARD_LED_OFF           0x00   /* 灯板指示灯灭 */

#define LED_BOARD_SPK_ON            0x01   /* 开启扬声器 */
#define LED_BOARD_SPK_OFF           0x00   /* 关闭扬声器 */

/* 看门狗宏定义 */
#define WATCHDOG_USE_CLK            0x00   /* 时钟硬件喂狗 */
#define WATCHDOG_USE_SOFT           0x01   /* 软件喂狗 */
#define WATCHDOG_STOP               0x02   /* 停止喂狗 */

/* MPC 状态查询宏定义 */
#define BRD_MPC_IN_POSITION         0      /* 对端主处理机板在位  */
#define BRD_MPC_OUTOF_POSITION      1      /* 对端主处理机板不在位 */

#define BRD_MPC_RUN_MASTER          0      /* 当前主处理机板主用 */
#define BRD_MPC_RUN_SLAVE           1      /* 当前主处理机板备用  */

#define BRD_MPC_SDHMODULE_NONE      0x07   /* 当前主处理机板没有插模块 */
#define BRD_MPC_SDHMODULE_SOI1      0x00   /* 当前主处理机板插模块SOI1 */
#define BRD_MPC_SDHMODULE_SOI4      0x01   /* 当前主处理机板插模块SOI4  */

#define SYNC_MODE_FREERUN           0      /* 自由振荡，对于主MCU应设置为该模式，所有下级线路时钟与该MCU同步 */
#define SYNC_MODE_TRACK_SDH8K       1      /* 跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的8K时钟 */
#define SYNC_MODE_TRACK_SDH2M       2      /* 跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的2M时钟 */
#define SYNC_MODE_TRACK_DT2M        4      /* 跟踪模式，作为下级MCU应设置为该模式，跟踪DT来的2M时钟 */
#define SYNC_MODE_UNKNOWN           0xff   /* 未知或错误的模式 */

#define SYNC_CLOCK_UNLOCK                0          /* 跟踪模式时时钟未锁定 */
#define SYNC_CLOCK_LOCK                  1          /* 跟踪模式时时钟锁定成功 */

/* ============================ CRI板相关的宏定义 ============================ */
#define BRD_CRI_MODULE_OFF          0
#define BRD_CRI_MODULE_ON           1

#define BRD_CRI_MLNK_LED                  0          /* CRI板mlink灯的编号 */

/* ============================ VAS板相关的宏定义 ============================ */
#define  VAS_MATRIX_16X16           0        /* 16X16的矩阵板 */
#define  VAS_MATRIX_08X08           8        /*   8X8的矩阵板 */

#define  VAS_MATRIX_OUTPUT_DISABLE  0xfb        /* 禁止音视频矩阵某输出端口 */
#define  VAS_MATRIX_OUTPUT_ENABLE   0xfc        /* 启用音视频矩阵某输出端口 */

#define BRD_VAS_MLINK_LED                    2    /* VAS板mlink灯的编号 */

/* ============================ DRI板相关的宏定义 ============================ */
#define BRD_DRI_CLK_OUTPUT_DISABLE   0   /* 高阻禁止DRI板上时钟信号输出到MPC板上 */
#define BRD_DRI_CLK_OUTPUT_ENABLE    1   /* 允许DRI板上时钟信号输出到MPC板上 */

#define BRD_DRI_RELAY_LOOP_DISABLE   0  /* 继电器连通 */
#define BRD_DRI_RELAY_LOOP_ENABLE    1  /* 继电器自环 */

#define BRD_DRI_E1CHAIN_DISABLE      0  /* 禁用某条E1链路 */
#define BRD_DRI_E1CHAIN_ENABLE       1  /* 使用某条E1链路 */

#define BRD_DRI_E1_MAXNUM            8  /* DRI共有8条E1链路 */

#define BRD_DRI_MLINK_LED                    9   /* dri板mlink灯的编号 */

/* ============================ MMP板相关的宏定义 ============================ */
#define BRD_MMP_MLINK_LED                    7   /* MMP板mlink灯的编号 */

/*==============================IMT板相关的宏定义====================================*/
#define IMT_SYSALM_LED   1  /* 系统告警灯，有告警时可点该灯 */
#define IMT_LINK_LED     5  /* 系统连接灯，和MCU建链后可点该灯 */


/* ============================ DSI板相关的宏定义 ============================ */
#define BRD_DSI_CLK_OUTPUT_DISABLE   0   /* 高阻禁止DSI板上时钟信号输出到MPC板上 */
#define BRD_DSI_CLK_OUTPUT_ENABLE    1   /* 允许DSI板上时钟信号输出到MPC板上 */

#define BRD_DSI_RELAY_SWITCH_IN      1  /* 继电器将背板进来的指定E1切换到8260 */
#define BRD_DSI_RELAY_SWITCH_OUT     0  /* 继电器将背板进来的指定E1切换到背板相应的E1输出 */

#define BRD_DSI_E1_MAXNUM            4  /* DSI共有4条E1链路 */

#define BRD_DSI_MLINK_LED            9   /* dsi板mlink灯的编号 */


/* vga info */
#define BSP15_VGA_NONE             0       /* vga输入输出功能均没有 */
#define BSP15_VGA_IN               1       /* 只有vga输入功能 */
#define BSP15_VGA_OUT              2       /* 只有vga输出功能 */
#define BSP15_VGA_INOUT            3       /* vga输入输出功能都有 */


#ifndef WIN32 
/* (2) 公共数据结构 */
/*  a. 单板身份结构定义 */
typedef struct{
    UINT8 byBrdID;                      /* 板子ID号 */
    UINT8 byBrdLayer;                   /* 板子所在层号 */
    UINT8 byBrdSlot;                    /* 板子所在槽位号 */
}TBrdPosition;

/*  b. 以太网参数结构 */
typedef struct{
    UINT32 dwIpAdrs;
    UINT32 dwIpMask;
    UINT8  byMacAdrs[6];
}TBrdEthParam;

/* 盘信息 */
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
STATUS BrdRestoreFile(char *pSrcFile, char *pDesFile);/* 还原文件 */
STATUS BrdBackupFile(char *pSrcFile, char *pDesFile); /* 备份文件 */


UINT8  BrdGetBSP15VGAConf(UINT8 byDevId);        /* 获取指定bsp15的vga配置 */
void   BrdStopVGACap(void);                      /* 停止采集vga(8083)数据(控制fpga) */
void   BrdStartVGACap(UINT8 byMode);             /* 开始采集vga(8083)数据(控制fpga) */
void   BrdSetVGACapMode(UINT8 byMode);           /* 设置fpga中VGA采集的模式(配置fpga) */
void   BrdCloseVGA(void);                        /* 关闭并禁止vga(8083)采集芯片工作 */
void   BrdOpenVGA(void);                         /* 打开并允许vga(8083)采集芯片工作 */


/* (3) 公共函数声明 */
STATUS BrdQueryPosition(TBrdPosition *pTBrdPosition);   /* 板身份(ID、层号、槽位)查询 */
STATUS BrdLedFlash(UINT8 byLedID, UINT8 byState);       /* 面板点灯控制 */
STATUS BrdLedStatusSet(UINT8 byLedID, UINT8 byState);   /* 面板点灯控制 */
UINT8 BrdQueryHWVersion(void);                          /* 硬件版本号查询 */
UINT8 BrdQueryFPGAVersion(void);                        /* FPGA版本号查询 */
STATUS BrdDrvLibInit(void);
STATUS sysRebootHookAdd(VOIDFUNCPTR rebootHook);    /* 复位注册函数 */
void BrdHwReset(void);                              /* 系统硬件复位 */
void SysRebootDisable(void);                        /* 禁止系统出现excption时自动复位 */
void SysRebootEnable(void);                         /* 允许系统出现excption时自动复位 */

STATUS BrdSetEthParam(UINT8 byEthId, UINT8 byIpOrMac, TBrdEthParam *ptBrdEthParam); /* 设置以太网参数 */
STATUS BrdGetEthParam(UINT8 byEthId, TBrdEthParam *ptBrdEthParam);                  /* 获取以太网参数 */
STATUS BrdDelEthParam(UINT8 byEthId);           /* 删除指定的以太网接口 */

STATUS BrdSetDefGateway(UINT32 dwIpAdrs);       /* 设置默认网关ip */
UINT32 BrdGetDefGateway(void);                  /* 获取默认网关ip */
STATUS BrdDelDefGateway(void);                  /* 删除默认网关ip */

UINT32 BrdGetNextHopIpAddr(UINT32 dwDstIpAddr); /* 获取通往指定ip的第一跳路由ip地址 */

STATUS SysSetUsrAppParam(char *pFileName, UINT8 byFileType, char *pAutoRunFunc);/* 用户程序启动参数设置 */

UINT8 SysGetOsVer(void);        /* 获取OS版本类型，因为可能在同一块板子上运行不同的OS */
#endif  /* END #ifndef WIN32 */



#ifdef __cplusplus 
} 
#endif /* __cplusplus */

#endif   /*INCDriverInith*/
