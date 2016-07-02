/******************************************************************************
模块名  ： BrdWrapper
文件名  ： brdwrapper.h
相关文件： brdwrapperdef.h
文件实现功能：BrdWrapper模块对外提供的结构定义和函数接口头文件，该文件目前主要应用于监
            控设备板级设备驱动。
作者    ：张方明
版本    ：1.0
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
12/27/2006  1.0        张方明      创建
******************************************************************************/
#ifndef __BRD_WRAPPER_H
#define __BRD_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "kdvtype.h"
#include "brdwrapperdef.h"
#include "api_vlynq.h"
#include <syslog.h>

#include <time.h>

/* -------------------------------- 宏定义 ----------------------------------*/

/****************************** 模块的版本号命名规定 *************************
总的结构：mn.mm.ii.cc.tttt
     如  Osp 1.1.7.20040318 表示
模块名称Osp
模块1版本
接口1版本
实现7版本
04年3月18号提交

版本修改记录：
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.1.1.20090413
增加功能：增加BrdGetMasterVideoMode功能接口
修改缺陷：无
提交人：马宜昌
----------------------------------------------------------------------------
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.1.1.20070712
增加功能：增加davinci resizer功能接口
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.1.2.20070921
增加功能：增加ts7210和kdm2820两台设备
修改缺陷：无
提交人：王远涛
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.1.2.20071019
增加功能：增加kdm2100设备
修改缺陷：无
提交人：秦晓辉
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.1.3.20071210
增加功能：增加kdv7810 ts7810设备
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.2.0.20071214
增加功能：增加以太网接口连接状态查询、协商状态查询接口
修改缺陷：无
提交人：张琰
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.2.1.20071226
增加功能：增加以太网接口工作模式设置接口
修改缺陷：无
提交人：张琰
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.2.2.20071229
增加功能：增加kdv7810设备
修改缺陷：无
提交人：秦晓辉
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.2.2.20080107
增加功能：增加选择红外遥控的接口
修改缺陷：无
提交人：王远涛
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.2.2.20080118
增加功能：添加版本号：BIG_ENDIAN在davinci中默认定义了，改成其他名字
修改缺陷：无
提交人：王远涛
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.2.2.20080124
增加功能： 增加TS6210E的设备定义.
修改缺陷：无
提交人：王远涛
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.2.3.20080202
增加功能： 增加HDMI音频查询和设定接口.
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.2.4.20080311
增加功能：增加kdv7810点灯支持
修改缺陷：无
提交人：秦晓辉
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.2.5.20080312
增加功能：更新设备定义
修改缺陷：无
提交人：秦晓辉

----------------------------------------------------------------------------
模块版本：BrdWrapper 1.2.5.20080325
增加功能：non-block update msp430(ir.txt)
修改缺陷：无
提交人：王远涛

----------------------------------------------------------------------------
模块版本：BrdWrapper 1.2.5.20080402
增加功能：增加KDV8000BHD设备定义
修改缺陷：无
提交人：张琰

----------------------------------------------------------------------------
模块版本：BrdWrapper 1.2.5.20080417
增加功能：增加KDV7910 E1支持和KDM2820系统产品支持
修改缺陷：无
提交人：张琰

----------------------------------------------------------------------------
模块版本：BrdWrapper 1.2.6.20080428
增加功能：主从Davinci系统中，增加从系统获取视频输出模式的接口
修改缺陷：无
提交人：王黔川

----------------------------------------------------------------------------
模块版本：BrdWrapper 1.2.6.20080428
增加功能：完善“主从Davinci系统中，增加从系统获取视频输出模式的接口”的代码
修改缺陷：无
提交人：王黔川

----------------------------------------------------------------------------
模块版本：BrdWrapper 1.2.6.20080505
增加功能：新增一些前端设备的定义
修改缺陷：无
提交人：毛一山
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.2.6.20080514
增加功能：无
修改缺陷：修改KDV7810红外单片机升级BUG
提交人：王黔川

--------------------------------------------------------6.20080515
增加功能：新增一个无线告警的接口
修改缺陷：无
提交人：毛一山

----------------------------------------------------------------------------
模块版本：BrdWrapper 1.3.7.20080715
增加功能：新增切换vga/video的接口BrdGetVgaVideoState()
修改缺陷：无
提交人：王远涛

----------------------------------------------------------------------------
模块版本：BrdWrapper 1.3.7.20080801
提交人：王黔川

----------------------------------------------------------------------------
模块版本：BrdWrapper 1.3.7.20080813
增加功能：增加KDV7810告警结构
修改缺陷：无
提交人：王黔川

----------------------------------------------------------------------------
模块版本：BrdWrapper 1.4.7.20080830
增加功能：增加KDV7620的音视频矩阵控制接口
修改缺陷：无
提交人：张方明

----------------------------------------------------------------------------
模块版本：BrdWrapper 1.4.7.20080909
增加功能：E2PROM相关接口重新实现，将nipwrapper中相关接口移植到brdwrapper中；
修改缺陷：无
提交人：张琰
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.4.7.20081120
增加功能：
		  (1)：设置和获取AD5246的灵敏度。只对kdm2210有效
		  (2):  设置硬件子版本号。
		  (3): 设置硬件的productid。只对使用新E2PROM的产品有效
修改缺陷：无
提交人：马建朋

----------------------------------------------------------------------------
模块版本：BrdWrapper 1.4.7.20081125
增加功能：增加TS6610的环路音量调节实现
修改缺陷：无
提交人：王黔川
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.4.7.20081208
增加功能：增加sysinfo中一个获取内存cache大小的字段
修改缺陷：无
提交人：马建朋
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.4.7.20081216
增加功能：修改bcsrFxoCtrl函数，解决环路音量实现带来的与上层业务不兼容问题
修改缺陷：无
提交人：王黔川
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.4.8.20090224
增加功能：增加HDMPU层号识别功能
修改缺陷：无
提交人：王黔川
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.4.9.20090316
增加功能：增加KDV7620的LED灯结构定义
修改缺陷：无
提交人：王黔川
----------------------------------------------------------------------------
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.4.9.20090403
增加功能：修改KDV7620的E1模块号识别缺陷
修改缺陷：无
提交人：王黔川
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.4.9.20090421
增加功能：修改reboot实现解决reboot可能会导致重启失败的问题
修改缺陷：无
提交人：王黔川
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.4.10.20090505
增加功能：修改E2PROM 结构相关宏定义
修改缺陷：无
提交人：蓝天宇
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.4.11.20090507
增加功能：增加VIDIN模块降帧处理和VIDOUT模块升帧处理功能
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.4.11.20090510
增加功能：改makefile，增加NVR2860红外单片机升级功能
修改缺陷：无
提交人：赵卫星
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.4.11.20090518
增加功能：增加KDV7620 H221发送支持
修改缺陷：无
提交人：王黔川
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.4.11.20090908
增加功能：增加KDV7820的LED灯结构定义
修改缺陷：无
提交人：王黔川
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.4.12.20100330
增加功能：KDV7820的接口FPGA增加台标功能
修改缺陷：KDV7820的接口FPGA解决了大部分图像质量问题
提交人：张方明
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.5.12.20100506
增加功能：KDV7820的接口FPGA增加滚动横幅功能
修改缺陷：KDV7820的接口FPGA解决了部分图像质量问题
提交人：张方明
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.5.13.20100608
增加功能：增加VGA输入接口相位滤波调节功能
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.5.14.20100712
增加功能：增加fpga做scale功能,需要第6版接口fpga支持
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.5.15.20100816
增加功能：增加fpga查询横幅滚动次数功能,需要第6版接口fpga支持
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.6.15.20100826
增加功能：增加查询显示设备信息的功能
修改缺陷：无
提交人：张方明
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.6.16.20101221
增加功能：
修改缺陷：解决fpga加载横幅文件时，台标底部显示不全的问题
提交人：杜辉
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.6.17.20101224
增加功能：
修改缺陷：解决fpga叠加横幅时，横幅左端边界与视频层有间隙的问题
提交人：杜辉
----------------------------------------------------------------------------
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.6.18.20110409
增加功能：
修改缺陷：增加LM80信息采集及告警功能
提交人：	王龙波
----------------------------------------------------------------------------
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.6.18.20110414
增加功能：
修改缺陷：增加E1具体告警信息
提交人：	王龙波
----------------------------------------------------------------------------
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.6.18.20110414
增加功能：
修改缺陷：增加终端告警信息
提交人：	王龙波
----------------------------------------------------------------------------
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.6.19.20110718
增加功能：获取内核打印信息
修改缺陷：无
提交人：李洪超
----------------------------------------------------------------------------
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.7.19.20110926
增加功能：数字mic在线升级
修改缺陷：无
提交人：李洪超
----------------------------------------------------------------------------
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.8.19.20111028
增加功能：lm75温度传感器告警接口
修改缺陷：无
提交人：李洪超
----------------------------------------------------------------------------
----------------------------------------------------------------------------
模块版本：BrdWrapper 1.9.19.20111114
增加功能：数字mic版本号查询
修改缺陷：无
提交人：李洪超
----------------------------------------------------------------------------


****************************************************************************/

/*各个板子的版本号定义*/
#define VER_Linux_BrdWrapper         (const s8*)"BrdWrapper 1.6.17.20101224"

#define BRD_ERR(fmt, args...) \
    {printf("[BRD_WRAPPER_ERR]:%s(%d): ",__func__,__LINE__);printf(fmt, ## args); \
     syslog(LOG_ERR,"[BRD_WRAPPER_ERR]:%s(%d): ",__func__,__LINE__);syslog(LOG_ERR,fmt, ## args);}

#define BRD_PRT(fmt, args...) \
    {printf("[BRD_WRAPPER_PRT]: ");printf(fmt, ## args);	\
    syslog(LOG_NOTICE,"[BRD_WRAPPER_PRT]: ");syslog(LOG_NOTICE,fmt, ## args);}


/* ------------------------------- 相关结构体定义 ----------------------------*/

/* 设备的能力集 */
typedef struct {
    u32 byBoardID;   /*设备ID号*/
    u32 byHwVer;     /*硬件版本号*/
    u8 byFpgaVer;   /*FPGA版本号*/
    u8 byBoardType; /*设备类别:0--终端类;1--插板类1(有槽位号和层号);2--插板类2(有槽位号无层号) */
    u8 byEthNum;    /*设备以太网个数*/
    u8 byEth0Name[INTERFACE_NAME_MAX_LEN];/*设备以太网0的名字*/
    u8 byEth1Name[INTERFACE_NAME_MAX_LEN];/*设备以太网0的名字*/
    u8 byEth2Name[INTERFACE_NAME_MAX_LEN];/*设备以太网0的名字*/
    u8 byE1Num;     /*设备E1个数，0代表该设备不支持E1*/
    u8 byV35Num;    /*设备V35个数,0代表该设备不支持V35*/
    u8 byBspNum;    /*设备BSP个数,0代表该设备没有BSP-15媒体芯片*/
    u8 byFanCtrlSupport;    /*设备是否支持风扇控制:0---不支持，1---支持*/
    u8 byRTCType;   /*设备RTC芯片的类型:间相关的宏定义--RTC_TYPE_NONE/RTC_TYPE_DS1337/RTC_TYPE_DS12C887*/
    u8 abyRS232Name[TTY_NAME_MAX_LEN];/*设备RS232串口对应的设备名/MPC板console口对应的设备名*/
    u8 abyRS422Name[TTY_NAME_MAX_LEN];/*设备RS422串口对应的设备名/MPC板SCC2对应的设备名*/
    u8 abyRS485Name[TTY_NAME_MAX_LEN];/*设备RS485串口对应的设备名/MPC板SCC3对应的设备名*/
    u8 abyIrayName[TTY_NAME_MAX_LEN]; /*设备红外串口对应的设备名*/
    u8 abydigmic0RS232Name[TTY_NAME_MAX_LEN];/*数字mic对应的串口0设备名*/
    u8 abydigmic1RS232Name[TTY_NAME_MAX_LEN];/*数字mic对应的串口1设备名*/
    u8 abydigmic2RS232Name[TTY_NAME_MAX_LEN];/*数字mic对应的串口2设备名*/
/*The bottom member support  t300*/
    u8 abyRS422_0_Name[TTY_NAME_MAX_LEN];/*设备RS422串口对应的设备名*/
    u8 abyRS422_1_Name[TTY_NAME_MAX_LEN];/*设备RS422串口对应的设备名*/
    u8 abyRS422_2_Name[TTY_NAME_MAX_LEN];/*设备RS422串口对应的设备名*/
    u8 abyRS232_422_485_0_NAME[TTY_NAME_MAX_LEN];	/*设备RS232/RS422/RS485共用的设备名*/
    u8 abyRS232_422_485_1_NAME[TTY_NAME_MAX_LEN];	/*设备RS232/RS422/RS485共用的设备名*/
    u8 abyRS232_422_485_2_NAME[TTY_NAME_MAX_LEN];	/*设备RS232/RS422/RS485共用的设备名*/
    u8 abyRS232_422_485_3_NAME[TTY_NAME_MAX_LEN];	/*设备RS232/RS422/RS485共用的设备名*/
    u8 abyRS232_422_485_4_NAME[TTY_NAME_MAX_LEN];	/*设备RS232/RS422/RS485共用的设备名*/
    u8 abyRS232_422_485_5_NAME[TTY_NAME_MAX_LEN];	/*设备RS232/RS422/RS485共用的设备名*/
}TBrdCapability;

/* 单板身份结构定义 */
typedef struct{
    u32 byBrdID;                      /* 板子ID号 */
    u8 byBrdLayer;                   /* 板子所在层号，设备不支持时返回0，当作0层处理 */
    u8 byBrdSlot;                    /* 板子所在槽位号，设备不支持时返回0，当作0槽处理;
                                        部分设备有主从之分，用slot来区分：0=主；1=从 */
}
__attribute__ ((packed))
TBrdPosition;

/* 单板信息结构体 */
typedef struct {
    BOOL32 bPlugIn;             /* TRUE =设备在位(主要是查询子卡信息时用，本板永远返回TRUE)；
                                   FALSE=设备不在位(此时后面的信息无效) */
	u32    dwBrdID;             /* 板子ID号 */
	u32    dwHwVer;             /* 硬件版本号 */
	s8     abyDevSerial[STR_NAME_MAX_LEN];    /* 设备序列号，字符串格式 */
	s8     abyProductDate[STR_NAME_MAX_LEN];  /* 设备生产日期，字符串格式 */
	s8     abyFlowCode[STR_NAME_MAX_LEN];     /* 流水线编码，字符串格式 */
	u32    dwEthMacTotal;                     /* 以太网MAC地址的个数 */
	u8     abyMacAddrs[ETH_MAC_MAX_NUM][6];   /* 以太网MAC地址值 */
}TBrdInfo;

/* HDMI音频信息结构体 */
typedef struct {
    u32 dwSampleRate;           /* 采样频率(Hz)，0=无音频或不输出音频; 正常值为:8000,32000,48000,96000 */
    u32 dwSampleBits;           /* 采样位宽：16bit/20bit/24bit/32bit */
	u32 dwChnlNum;              /* 声道个数：1/2/3(2.1)/6(5.1杜比环绕) */
}THdmiAudInfo;

/* 终端KDV8010A以及KDV8000B前面板指示灯结构 */
typedef struct{
    u8 byLedPower;
    u8 byLedALM;
    u8 byLedRun;
    u8 byLedLINK;
    u8 byLedDSP1;
    u8 byLedDSP2;
    u8 byLedDSP3;
    u8 byLedETH1;
    u8 byLedETH2;
    u8 byLedETH3;
    u8 byLedIR;
}TBrdKDV8010LedStateDesc,TBrdMTLedStateDesc;

/* 终端KDV8010C前面板指示灯结构 */
typedef struct{
    u8 byLedPower;
    u8 byLedALM;
    u8 byLedRun;
    u8 byLedLINK;
    u8 byLedDSP1;
    u8 byLedDSP2;
    u8 byLedETH1;
    u8 byLedIR;
}TBrdKDV8010CLedStateDesc;

/* KDM2401前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct{
    u8 byLedRun;
    u8 byLedETH;
}
__attribute__ ((packed))
TBrdKDM2401LedDesc;

/* KDM2402前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct{
    u8 byLedRun;
    u8 byLedSYSLNK;
    u8 byLedHD;     /* 软件无法检测，返回常灭状态 */
    u8 byLedVLED0;
    u8 byLedVLED1;
}
__attribute__ ((packed))
TBrdKDM2402LedDesc;

/*KDM2401ES前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct{
    u8 byLedRun;
    u8 byLedLNK;
    u8 byLedHD;     /* 软件无法检测，返回常灭状态 */
    u8 byLedVLED0;
}
__attribute__ ((packed))
TBrdKDM2401ESLedDesc;

/*KDM2404S前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct{
    u8 byLedRun;
    u8 byLedLNK;
    u8 byLedHD;     /* 软件无法检测，返回常灭状态 */
    u8 byLedVLED0;
    u8 byLedVLED1;
    u8 byLedVLED2;
    u8 byLedVLED3;
}
__attribute__ ((packed))
TBrdKDM2404SLedDesc;

/*KDM2501前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct{
    u8 byLedRun;
    u8 byLedACT;    /* 同LINK灯 */
    u8 byLedIR;     /* 软件无法检测，返回常灭状态 */
}
__attribute__ ((packed))
TBrdKDM2501LedDesc;

/*KDM2820前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct{
    u8 byLedRun;
    u8 byLedETH;    /* 软件无法检测，返回常灭状态 */
    u8 byLedHD;     /* 软件无法检测，返回常灭状态 */
    u8 byLedIR;     /* 软件无法检测，返回常灭状态 */
    u8 byLedALM;
}
__attribute__ ((packed))
TBrdKDM2820LedDesc;

/*V5前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct{
    u8 byLedGreen;
    u8 byLedOrange;
}
__attribute__ ((packed))
TBrdV5LedDesc;

/*TS5210前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct{
    u8 byLedGreen;
    u8 byLedOrange;
}
__attribute__ ((packed))
TBrdTS5210LedDesc;

/*TS7210前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct{
    u8 byLedGreen;
    u8 byLedOrange;
}
__attribute__ ((packed))
TBrdTS7210LedDesc;

/* KDM200_MPU前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct{
    u8 byLedRun;
    u8 byLedLINK;
    u8 byLedALM;
}
__attribute__ ((packed))
TBrdKDM200_MPULedDesc;

/*TS6610前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct{
    u8 byLedGreen;
    u8 byLedOrange;
}
__attribute__ ((packed))
TBrdTS6610LedDesc;

/* KDM2440前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct{
    u8 byLedRun;
}
__attribute__ ((packed))
TBrdKDM2440LedDesc;

/* KDM2100前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct{
    u8 byLedGreen; //FlashMode: on,off,quick,slow
    u8 byLedRed;
    u8 byLedOrange;
}
__attribute__ ((packed))
TBrdKDM2100LedDesc;

/*KDM201C04前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct{
    u8 byLedVLED0;
    u8 byLedVLED1;
    u8 byLedVLED2;
    u8 byLedVLED3;
}
__attribute__ ((packed))
TBrdKDM201C04LedDesc;

/*KDV7810前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct{
    u8 byLedRun;
    u8 byLedLINK;
    u8 byLedALM;
    u8 byLedENC;
    u8 byLedDEC;
}
__attribute__ ((packed))
TBrdKDV7810LedDesc;

typedef struct{
    u8 byLedRun;
    u8 byLedLINK;
    u8 byLedALM;
    u8 byLedPWR;/*只能查询，不能设置*/
    u8 byLedLAN;/*只能查询，不能设置*/
    u8 byLedIR;/*只能查询，不能设置*/
}
__attribute__ ((packed))
TBrdKDV7620LedDesc;


typedef struct{
    u8 byLedRun;
    u8 byLedLINK;
    u8 byLedALM;
    u8 byLedDSP;
    u8 byLedPWR;	/*只能查询，不能设置*/
    u8 byLedLAN1;	/*只能查询，不能设置*/
    u8 byLedLAN2;	/*只能查询，不能设置*/
    u8 byLedIR;		/*只能查询，不能设置*/
}
__attribute__ ((packed))
TBrdKDV7820LedDesc;


/*H600前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct{
    u8 byLedRun;
    u8 byLedALM;
    u8 byLedPWR;
    u8 byLedLAN;
}__attribute__ ((packed))TBrdH600LedDesc;

/*H700前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct brd_h700_led_des
{
    u8 byLedRun;
    u8 byLedALM;
    u8 byLedLINK;
}
__attribute__ ((packed))
TBrdH700LedDesc;

/*H800前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct brd_h800_led_des
{
    u8 byLedRun;
    u8 byLedALM;
    u8 byLedLINK;
}
__attribute__ ((packed))
TBrdH800LedDesc;

/*H900前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct brd_h900_led_des
{
    u8 byLedRun;
    u8 byLedALM;
    u8 byLedLINK;
}
__attribute__ ((packed))
TBrdH900LedDesc;

/*HDMPU前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct{
    u8 byLedRun;
    u8 byLedLINK;
    u8 byLedALM;
    u8 byLedLINK0;
    u8 byLedLINK1;
}
__attribute__ ((packed))
TBrdHDMPULedDesc;


/*HDMPU2前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct{
    u8 byLedRun;
    u8 byLedLINK;
    u8 byLedALM;
    u8 byLedLINK0;
    u8 byLedLINK1;
}
__attribute__ ((packed))
TBrdHDMPU2LedDesc;

/*KDV8000I前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct brd_kdv8000i_led_des
{
    u8 byLedRun;
    u8 byLedALM;
}
__attribute__ ((packed))
TBrdKDV8000ILedDesc;

/*MPC2前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct{
    u8 byLedRun;
    u8 byLedALM;
    u8 byLedMS;
    u8 byLedOUS;
    u8 byLedSYN;
    u8 byLedNMS;
    u8 byLedLINK0;	/*LINK0灯*/
    u8 byLedLINK1;    /*LINK1灯 */
    u8 byLedNORM;    /* 灯板上正常指示灯 */
    u8 byLedNALM;    /* 灯板上一般告警指示灯 */
    u8 byLedSALM;   /* 灯板上严重告警指示灯 */
}
__attribute__ ((packed))
TBrdMPC2LedDesc;

/*CRI2 前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct{
    u8 byLedRun;
    u8 byLedALM;
    u8 byLedLINK0;	/*LINK0灯*/
    u8 byLedLINK1;   /*LINK1灯 */
    u8 byLedMLINK;
    u8 byLedMS;
    u8 byLedFun1;
    u8 byLedFun2;
    u8 byLedFun3;
    u8 byLedFun4;
    u8 byLedFun5;
    u8 byLedFun6;
    u8 byLedFun7;
    u8 byLedFun8;
}
__attribute__ ((packed))
TBrdCRI2LedDesc;

/* 终端DRI2前面板指示灯结构 */
typedef struct{
    u8 byLedRun;     /* 系统运行灯 */
    u8 byLedE1ALM0;  /* E10告警指示灯 */
    u8 byLedE1ALM1;  /* E11告警指示灯 */
    u8 byLedE1ALM2;  /* E12告警指示灯 */
    u8 byLedE1ALM3;  /* E13告警指示灯 */
    u8 byLedE1ALM4;  /* E14告警指示灯 */
    u8 byLedE1ALM5;  /* E15告警指示灯 */
    u8 byLedE1ALM6;  /* E16告警指示灯 */
    u8 byLedE1ALM7;  /* E17告警指示灯 */
    u8 byLedMLINK;   /* 系统建链指示灯 */
    u8 byLedLINK0;	/*LINK0灯*/
    u8 byLedLINK1;    /*LINK1灯 */
}
__attribute__ ((packed))
TBrdDRI2LedDesc;



/*IS3-MPC8313前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
/*数组成员包含1个run灯和26个网口状态灯，其中定义如下：数组元素位置：面板上显示 */
/*不包括5v,12v.  0-6:00-06;  7-13:08-14; 14:run;  15-16:mc0-mc1;  17-24:e0-e7; 25-26:G0-G1 */
#define MPC8313LED_00     0
#define MPC8313LED_06     6
#define MPC8313LED_08     7
#define MPC8313LED_14     13
#define MPC8313LED_RUN     14
#define MPC8313LED_MC0     15
#define MPC8313LED_MC1     16
#define MPC8313LED_E0     17
#define MPC8313LED_E7     24
#define MPC8313LED_G0     25
#define MPC8313LED_G1     26
typedef struct{
 	u8 byLedSwitch[27];
}
__attribute__ ((packed))
TBrdIS3MPC8313LedDesc;

/*IS3-MPC8548前面板指示灯结构，成员变量值如宏定义：指示灯状态 */
typedef struct{
    u8 byLedALM;
    u8 byLedMLINK;    /* MLK */
    u8 byLedLINK;
}
__attribute__ ((packed))
TBrdIS3MPC8548LedDesc;

/* 指示灯状态公共结构 */
typedef struct {
    u32 dwLedNum;                       /* 有效的指示灯的个数 */
    union {
        u8 byLedNo[BRD_LED_NUM_MAX];    /* 所有有效的指示灯状态，超过dwLedNum的灯值为0 */
        TBrdKDV8010CLedStateDesc tBrdKDV8010CLedState;
        TBrdKDV8010LedStateDesc  tBrdKDV8010LedStateDesc;
        TBrdMTLedStateDesc       tBrdMTLedState;
        TBrdKDM2401LedDesc       tBrdKDM2401Led;
        TBrdKDM2402LedDesc       tBrdKDM2402Led;
        TBrdKDM2401ESLedDesc     tBrdKDM2401ESLed;
        TBrdKDM2404SLedDesc      tBrdKDM2404SLed;
        TBrdKDM2501LedDesc       tBrdKDM2501Led;
        TBrdV5LedDesc            tBrdV5Led;
        TBrdTS5210LedDesc        tBrdTS5210Led;
        TBrdTS7210LedDesc        tBrdTS7210Led;
        TBrdKDM200_MPULedDesc    tBrdKDM200_MPULed;
        TBrdTS6610LedDesc        tBrdTS6610Led;
        TBrdKDM2440LedDesc 	     tBrdKDM2440Led;
        TBrdKDM2100LedDesc       tBrdKDM2100Led;
        TBrdKDM201C04LedDesc     tBrdKDM201C04Led;
	 TBrdKDM2820LedDesc       tBrdKDM2820Led;
        TBrdKDV7810LedDesc       tBrdKDV7810Led;
	 TBrdKDV7620LedDesc        tBrdKDV7620Led;
	 TBrdH600LedDesc			tBrdH600Led;
	 TBrdKDV7820LedDesc        tBrdKDV7820Led;
	 TBrdMPC2LedDesc	   tBrdMPC2LedState;
	 TBrdDRI2LedDesc    tBrdDRI2LedState;
	 TBrdHDMPULedDesc tBrdHDMPULedState;
	 TBrdHDMPU2LedDesc tBrdHDMPU2LedState;
	 TBrdCRI2LedDesc   tBrdCRI2LedState;
	 TBrdIS3MPC8313LedDesc  tBrdIS2XMpc8313LedState;
	 TBrdIS3MPC8548LedDesc  tBrdIS2XMpc8548LedState;
	 TBrdH700LedDesc		tBrdH700LedState;
	  TBrdH800LedDesc		tBrdH800LedState;
	 TBrdH900LedDesc		tBrdH900LedState;
	 TBrdKDV8000ILedDesc tBrdKDV8000ILedState;
    } nlunion;
} TBrdLedState;


/* ------------------------- 底层告警相关的结构定义----------------------*/

/* 底层V35告警信息结构定义 */
typedef struct{
    BOOL32 bAlarmV35CDDown;     /* Carrier Detect, 载波检测失败 */
    BOOL32 bAlarmV35CTSDown;    /* Clear To Send, 清除发送失败 */
    BOOL32 bAlarmV35RTSDown;    /* Request To Send, 请求发送失败 */
    BOOL32 bAlarmV35DTRDown;    /* 数据终端未就绪 */
    BOOL32 bAlarmV35DSRDown;    /* 数据未准备好 */
}TBrdV35AlarmDesc;

/* 底层E1告警信息结构定义 */
typedef struct{
    BOOL32 bAlarmE1RUA1;     /* E1全1告警 */
    BOOL32 bAlarmE1RLOS;     /* E1失同步告警 */
    BOOL32 bAlarmE1RRA;      /* E1远端告警 */
    BOOL32 bAlarmE1RCL;      /* E1失载波告警 */
    BOOL32 bAlarmE1RCMF;     /* E1的CRC复帧错误告警 */
    BOOL32 bAlarmE1RMF;      /* E1随路复帧错误告警 */
}TBrdE1AlarmDesc;

/* 机框电源告警数据结构 */
typedef struct{
    BOOL32 bAlarmPowerDC48VLDown;     /* 左电源-48V异常 */
    BOOL32 bAlarmPowerDC48VRDown;     /* 右电源-48V异常 */
    BOOL32 bAlarmPowerDC5VLDown;      /* 左电源+5V异常 */
    BOOL32 bAlarmPowerDC5VRDown;      /* 右电源+5V异常 */
}TBrdMPCAlarmPowerDesc;

/* 机框电源风扇告警数据结构 */
typedef struct{
    BOOL32 bAlarmPowerFanLLDown;      /* 左电源左风扇停转 */
    BOOL32 bAlarmPowerFanLRDown;      /* 左电源右风扇停转 */
    BOOL32 bAlarmPowerFanRLDown;      /* 右电源左风扇停转 */
    BOOL32 bAlarmPowerFanRRDown;      /* 右电源右风扇停转 */
}TBrdMPCAlarmPowerFanDesc;

/* MPC单板告警数据结构定义 */
typedef struct{
    BOOL32 bAlarmNetClkLockFailed;     /*  锁相环没有锁住参考时钟 */
    TBrdMPCAlarmPowerDesc tBrdMPCAlarmPower; /* 电源告警 */
    TBrdMPCAlarmPowerFanDesc tBrdMPCAlarmPowerFan; /* 电源风扇告警 */
}TBrdMPCAlarmVeneer;

/* SDH告警结构定义 */
typedef struct{
    BOOL32 bAlarmLOS;         /* 信号丢失 */
    BOOL32 bAlarmLOF;         /* 帧丢失 */
    BOOL32 bAlarmOOF;         /* 帧失步 */
    BOOL32 bAlarmLOM;         /* 复帧丢失 */
    BOOL32 bAlarmAU_LOP;      /* 管理单元指针丢失 */
    BOOL32 bAlarmMS_RDI;      /* 复用段远端缺陷指示 */
    BOOL32 bAlarmMS_AIS;      /* 复用段报警指示 */
    BOOL32 bAlarmHP_RDI;      /* 高阶通道远端缺陷指示 */
    BOOL32 bAlarmHP_AIS;      /* 高阶通道报警指示 */
    BOOL32 bAlarmRS_TIM;      /* 再生段通道踪迹字节不匹配 */
    BOOL32 bAlarmHP_TIM;      /* 阶通道踪迹字节不匹配 */
    BOOL32 bAlarmHP_UNEQ;     /* 高阶通道信号标记字节未装载 */
    BOOL32 bAlarmHP_PLSM;     /* 高阶通道信号标记字节不匹配 */

    BOOL32 bAlarmTU_LOP;      /* 支路单元指针丢失 */
    BOOL32 bAlarmLP_RDI;      /* 低阶通道远端缺陷指示 */
    BOOL32 bAlarmLP_AIS;      /* 低阶通道报警指示 */
    BOOL32 bAlarmLP_TIM;      /* 低阶通道踪迹字节不匹配 */
    BOOL32 bAlarmLP_PLSM;     /* 低阶通道信号标记字节不匹配 */
}TBrdMPCAlarmSDH;

/* MPC告警数据结构定义 */
typedef struct{
    TBrdMPCAlarmVeneer tBrdMPCAlarmVeneer;
    TBrdMPCAlarmSDH    tBrdMPCAlarmSDH;
}TBrdMPCAlarmAll;

typedef struct{
    BOOL32 bAlarmModuleOffLine;     /* 模块离线 */
}TBrdCRIAlarmAll;

/* DRI底层E1告警信息结构定义 */
typedef struct{
    TBrdE1AlarmDesc tBrdE1AlarmDesc[8];
}TBrdDRIAlarmAll;

/* DRI16底层E1告警信息结构定义 */
typedef struct{
    TBrdE1AlarmDesc tBrdE1AlarmDesc[16];
}TBrdDRI16AlarmAll;

/* DSI底层E1告警信息结构定义 */
typedef struct{
    TBrdE1AlarmDesc tBrdE1AlarmDesc[4];
}TBrdDSIAlarmAll;

/* IMT底层告警信息结构定义 */
typedef struct{
    BOOL32 bAlarmDSP1FanStop;     /* 图像编码风扇停转 */
    BOOL32 bAlarmDSP2FanStop;     /* 图像解码风扇停转 */
    BOOL32 bAlarmDSP3FanStop;     /* 音频编解码风扇停转 */
}TBrdIMTAlarmAll;

/* KDV8010A底层告警信息结构定义 */
typedef struct{
    BOOL32 bAlarmEth1LnkDown;     /* 以太网1无连接 */
    BOOL32 bAlarmDSP1FanStop;     /* 图像编码风扇停转 */
    BOOL32 bAlarmDSP2FanStop;     /* 图像解码风扇停转 */
    BOOL32 bAlarmDSP3FanStop;     /* 音频编解码风扇停转 */
    BOOL32 bAlarmSYS1FanStop;     /* 机箱风扇1停转 */
    BOOL32 bAlarmSYS2FanStop;     /* 机箱风扇2停转 */
    TBrdE1AlarmDesc tBrdE1AlarmDesc[4];     /* E1告警 */
    TBrdV35AlarmDesc tBrdV35AlarmDesc;    /* V.35告警 */
}TBrdMTAlarmAll;


/* KDV7810底层告警信息结构定义 */
/*
typedef struct{
    BOOL32 bAlarmHighTemp;
    BOOL32 bAlarmVolatageError;
    BOOL32 bAlarmFan0Stop;
    BOOL32 bAlarmFan1Stop;
}TBrdKDV7810AlarmAll;
*/


/* KDV8018底层告警信息结构定义 */
typedef struct{
    TBrdE1AlarmDesc tBrdE1Alarm;     /* E1告警 */
}TBrdSMTAlarmAll;

/* KDM2400 2500底层告警信息结构定义 */
typedef struct{
    TBrdE1AlarmDesc tBrdE1Alarm;     /* E1告警 */
}TBrdKDM2000AlarmAll;

/* KDM200_MPU底层告警信息结构定义 */
typedef struct{
    BOOL32 bFan0Stop;     /* 风扇0停转 */
    BOOL32 bFan1Stop;     /* 风扇1停转 */
    BOOL32 bFan2Stop;     /* 风扇2停转 */
}TBrdKDM200_MPUAlarm;

typedef struct {
	int temp;
	BOOL32 voltage_alm;
	u32	fan0_speed;
	u32 fan1_speed;
} TBrdSensor;

typedef struct{
	BOOL32 tempAlarm;  //风扇转速超过80°c
	BOOL32 voltageAlarm; //电压范围超过+_10%
	BOOL32 fan0Alarm;   //风扇转速低于2000转
	BOOL32 fan1Alarm;	 //风扇转速低于2000转
	}TBrdSensorAlarm;
/* KDV7820底层告警信息结构定义 */
typedef struct{
    BOOL32 bAlarmHighTemp;     	/* 温度过高(超过80 度) */
    BOOL32 bAlarmVolatageError;   /* 电压告警 */
    BOOL32 bAlarmFan0Stop;     	/* 风扇0 停转 */
    BOOL32 bAlarmFan1Stop;     	/* 风扇1 停转 */
}TBrdKDV7820larmAll;

typedef struct {
	TBrdSensorAlarm tBrdSensorAlarm; //lm75 alarm
}TBrdKDVH600AlarmAll,TBrdKDVH700AlarmAll,TBrdKDVH900AlarmAll;

/* 告警状态公共结构 */
typedef struct {
    u32 dwAlmNum;             /* 实际告警单元个数 */
    union {
        BOOL32 bAlmNo[BRD_ALM_NUM_MAX];
        TBrdMPCAlarmAll tBrdMPCAlarmAll;
        TBrdCRIAlarmAll tBrdCRIAlarmAll;
        TBrdDRIAlarmAll tBrdDRIAlarmAll;
        TBrdDRI16AlarmAll tBrdDRI16AlarmAll;
        TBrdDSIAlarmAll tBrdDSIAlarmAll;
        TBrdIMTAlarmAll tBrdIMTAlarmAll;
        TBrdMTAlarmAll  tBrdMTAlarmAll;
        TBrdSMTAlarmAll tBrdSMTAlarmAll;
        TBrdKDM2000AlarmAll tBrdKDM2000AlarmAll;
        TBrdKDM200_MPUAlarm tKDM200_MPUAlarm;
//	TBrdKDV7810AlarmAll	tKDV7810AlarmAll;
		TBrdKDV7820larmAll tKDV7820Alarm;
        TBrdKDVH900AlarmAll tKDVH900Alarm;
      } nlunion;
}TBrdAlarmState;
typedef struct {
    u32 temp;
    u32 in0_3v3;
    u32 in1_2v5;
    u32 in2_1v8;
    u32 in3_1v8;
    u32 in4_1v2;
    u32 in5_1v2;
    u32 in6_1v0;
    u32 fan0_speed;
    u32 fan1_speed;
} TKdv7820Sensor;
typedef struct {
    u32 temp;
    u32 in0_3v3;
    u32 in1_2v5;
    u32 in2_5v0;
    u32 in3_1v8;
    u32 in4_12v0;
    u32 in5_1v5;
    u32 in6_1v2;
    u32 fan0_speed;
    u32 fan1_speed;
} TKdv7810Sensor;
typedef struct {
    int temp;
    u32 in0;
    u32 in1;
    u32 in2;
    u32 in3;
    u32 in4;
    u32 in5;
    u32 in6;
    u32 fan0_speed;
    u32 fan1_speed;
} TSensor;

typedef struct {
	int temp;
	u32 fan0_speed;
}TKdvH600Sensor;
typedef struct {
	int temp;
   	u32 fan0_speed;
    	u32 fan1_speed;
}TKdvH700Sensor,TKdvH900Sensor;
typedef struct{
	union{
		TSensor sensor;
		TKdv7810Sensor kdv7810sensor;
		TKdv7820Sensor kdv7820sensor;
		//TKdvH600Sensor kdvh600sensor;
		//TKdvH700Sensor kdvh700sensor;
		//TKdvH900Sensor kdvh900sensor;
		}union_t;
}TSensorAll;

/* 老E1模块DS21Q354，底层E1告警信息结构定义 */
typedef struct{
	BOOL32 bAlarmE1RRAOld;      /* E1远端告警 mask:04*/
    BOOL32 bAlarmE1RUA1Old;     /* E1全1告警 mask:08*/
	BOOL32 bAlarmE1RLOSOld;     /* E1失同步告警 mask:01*/
	BOOL32 bAlarmE1RCLOld;      /* E1失载波告警 mask:02*/
}TBrdE1AlarmDetail21354;

/*新E1 模块DS26534，底层E1告警信息结构定义 */
typedef struct{
	BOOL32 bAlarmE1RRAI;	/* E1远端告警mask:08 */
	BOOL32 bAlarmE1RAIS;	 /* E1全1告警 mask:04*/
	BOOL32 bAlarmE1RLOF;    /* E1失同步告警mask:02 */
	BOOL32 bAlarmE1RLOS;	/*E1丢失信号告警mask:01*/
}TBrdE1AlarmDetail26514;

/*新E1和老E1 模块详细告警信息*/
#define MAX__E1_PORT 8
typedef union {
		TBrdE1AlarmDetail21354 tBrdE1AlarmDetail21354;
		TBrdE1AlarmDetail26514 tBrdE1AlarmDetail26514;
		BOOL32 tBrdE1AlarmDetail[4];
		}TBrdE1AlarmDetail;

typedef struct {
	TBrdE1AlarmDetail tBrdE1AlarmDetail[MAX__E1_PORT];	   /* E1告警 */
	BOOL32 E1Exist;					/*E1存在返回1，不存在返回0*/
	BOOL32 E1PhyMod;				/*判断是否是新E1模块，旧模块21354为1，新模块26514为0*/
}TBrdE1AlarmAll;

/*底层告警信息结构定义*/
typedef struct{
	TBrdSensorAlarm tBrdSensorAlarm; //lm80告警信息
	TBrdE1AlarmAll tBrdE1AlarmAll;
}TBrdKDV7810_AlarmAll,TBrdKDV7820AlarmAll;

/*底层告警信息结构定义*/
typedef struct{
	TBrdE1AlarmAll tBrdE1AlarmAll;
}TBrdKDV7620AlarmAll;

typedef struct{
		union{
			BOOL32 bAlmNo[BRD_ALM_NUM_MAX];
			TBrdKDV7810_AlarmAll tBrdKDV7810AlarmAll;
			TBrdKDV7820AlarmAll tBrdKDV7820AlarmAll;
			TBrdKDV7620AlarmAll tBrdKDV7620AlarmAll;
			TBrdKDVH600AlarmAll tBrdKDVH600AlarmAll;
			TBrdKDVH700AlarmAll tBrdKDVH700AlarmAll;
			TBrdKDVH900AlarmAll tBrdKDVH900AlarmAll;
	 		}nlunion;
	}TBrdKDVAlarm;


/*串口控制相关的结构定义*/
/* 485查询数据输入的参数结构 */
typedef struct {
    u32 dwDevType;           /* 485设备类型 */
    u32 dwRcvTimeOut;        /* 等待485设备回应的超时,0取默认值 */
    u8  byDesAddr;           /* 485设备目标地址 */
    u8  byCmd;               /* 操作命令 */
} TRS485InParam;


/* IRAY返回值结构 */
typedef struct{
    u8 byCtrllerType;    /* 遥控器类型 */
    u8 byKeyCode;        /* 键码 */
}TIRAYRtnDataDesc;

typedef struct {
    BOOL32 bDevLnked;         /* 485设备连接状态 */
    union {
        TIRAYRtnDataDesc tIRAYRtnData;
    } nlunion;
} TRS485RtnData;

/*
typedef struct {
    u32 temp; // 1/1000 C
   u32 in0_3v3;	//mv
    u32 in1_2v5;    	//mv
    u32 in2_5v0;	//mv
    u32 in3_1v8;	//mv
    u32 in4_12v0;	//mv
    u32 in5_1v5;	//mv
    u32 in6_1v2;       //mv
    u32 fan0_speed; //rpm
    u32 fan1_speed; //rpm
} TBrdSensor;
*/

typedef struct {
	u32 updays;
	u8 uphours;
	u8 upmins;
	u8 loads_int[3]; /* 1, 5, and 15 minute load averages (int portion)*/
	u8 loads_frac[3]; /* 1, 5, and 15 minute load averages (frac portion) */
	u32 totalram; //k
	u32 freeram; //k
	u32 procs;
	u32 cached; /*cache*/

}TBrdSysInfo;

/* 视频格式结构定义 */
typedef struct{
    u32	    dwWidth;                    /* 视频宽，像素为单位,为0表示无信号 */
    u32	    dwHeight;                   /* 视频高，行为单位,为0表示无信号 */
    BOOL32  bProgressive;               /* 逐行或隔行，TRUE=逐行；FALSE=隔行 */
    u32     dwFrameRate;                /* 帧率，逐行时=场频，隔行时=场频/2，即60i=30P, 为0表示无信号
                                           如果值大于1000，则认为包含小数点后3位，如59940表示59.940Hz
                                           常用的有60/1.001=59.940; 24/1.001=23.976; 30/1.001=29.970 */
    u32     dwColorSpace;               /* 详见brdwrapperdef.h: 视频格式掩码定义 */
} TVidStd;

/* 视频降噪参数结构定义 */
typedef struct{
    u32       dwNsfLevel;                 /* 详见: dwNfLevel宏定义 */
    u32       dwNsfType;                  /* 详见: dwNsfType宏定义 */
	u32       dwNsfEnable;                /* 0-disable 1-enable */    
    TVidStd   vidstd;                     /* 视频格式 */
} TVidNsfParam;

/* OSD参数格式结构定义 */
typedef struct{
    TVidStd	    tVid0Std;               /* 视频0层制式 */
    TVidStd	    tOsd0Std;               /* OSD0层制式，其图像宽、高和逐行或隔行必须和Vid层相同 */
    u32         dwTransVal;             /* 全局透明度，范围: 0-0xff, 为0时为全透，OSD层不参与合成，Vid层直接透传 */
    u32         dwBackGroundY;          /* 全透的背景色Y分量值，范围: 0x10-0xf0, OSD层每个点的Y值等于背景色的话
                                           不参与合成，Vid层直接透传该点的数据 */
} TVidOsdParam;

typedef struct{
    u32 chip_id;
    u32 vp_id;
    TVidStd tVidStd;
}TVidIoConfigParam;

/* Resize参数格式结构定义 */
typedef struct{
    u32         dwIntfIn;               /* Resizer的源接口选择 */
    u32         dwMode;                 /* Resizer的缩放模式选择 */
    TVidStd	    tVidInStd;              /* 进入Resizer的视频制式 */
    TVidStd	    tVidOutStd;             /* Resizer之后的视频制式*/

    u32         dwScaledX;              /* 缩放后画面相对底图左边右移n个象素，整个图像右移 !注意：必须是偶数!不能超出底图范围 */
    u32         dwScaledY;              /* 缩放后画面相对底图顶部下移n行，整个图像下移，注意：不能超出底图范围 */
    u32         dwScaledWidth;          /* 填0表示缩放后画面宽和输出制式一致,非0表示缩放后图像的宽度 !注意：必须是偶数!不能超出底图范围 */
    u32         dwScaledHight;          /* 填0表示缩放后画面高和输出制式一致,非0表示缩放后图像的高度 注意：不能超出底图范围 */

    u8          *pbyCoef;               /* Resizer的滤波系数数据指针，为NULL表示使用默认值 */
    u32         dwCoefLen;              /* Resizer的滤波系数数据字节数，为0表示使用默认值 */
} TVidResizeParam;

/* 输出口连接的TV信息结构定义 */
typedef struct{
    u32       dwEdidAvailable;        /* 所连接的TV的EDID信息是否可用，1 = 可用，0 = 不可用 */
    s8        edid_ver[2];            /* EDID版本: 对于版本1.3，则edid_ver[0]=1, edid_ver[1]=3 */
    u32       dwTVIntf;               /* TV连接的视频接口类型，如: 视频输出接口类型宏定义
                                           返回VIDOUT_SHUT_DOWN表示无连接, 其他值为有效接口类型，
                                           同时可以根据这里来判断所连接的视频接口为模拟还是数字 */
    s8        chName[16];             /* TV名称 */
    s8        chManufacturer[16];     /* 生产厂商 */
    s8        product_code[2];        /* 产品代码，格式: 比如对于f036，则product_code[0]=0xf0, product_code[1]=0x36 */
    s32       serial_num;             /* 序列号， 格式: 32位有符号整形，(可选，当以字符串的形式来表示的时候，存放在另外一个地方) */
    s8        serial_num_str[16];     /* 以字符串形式表示的序列号，例如:"DCT3C14R27MM" */
    s32       production_date[2];     /* 生产日期: 年份，第几个星期。 对于2011年第18个星期，则production_date[0]=2011,production_date[1]=18 */
    s8        screen_size[2];         /* TV的屏幕大小，单位厘米，格式: 长 x 宽  ==  screen_size[0] x screen_size[1] */
    s8        gamma;                  /* TV的gamma值，格式:  gamma = (真实的伽马值 x 100) – 100， 比如 120 = (2.2 x 100) - 100 */
    
    s8        feature;                /* 所支持的一些特性，按比特来划分:
                                           bit[7]: 1 = Standby supported, 0 = not supported  
                                           bit[6]: 1 = Suspend supported, 0 = not supported 
                                           bit[5]: 1 = Active Off supported, 0 = not supported 
                                           bit[4:3]: Bit 4    Bit 3
                                                     1         1         Undefined 
                                                     1         0         Non-RGB Display 
                                                     0         1         RGB Display   
                                                     0         0         Monochrome Display
                                           bit[2]: 1 = sRGB supported, 0 = not supported 
                                           bit[1]: 1 = preferred timing is indicated in first 
                                                          detailed timing block (required),  
                                                   0 = not indicated (not allowed) 
                                           bit[0]: 1 = GTF supported, 0 = not supported */
                                           
    s8        range_limit[5];         /* range_limit[0] = Min Vertical Rate in Hz
                                           range_limit[1] = Max Vertical Rate in Hz
                                           range_limit[2] = Min Horizontal Rate in kHz 
                                           range_limit[3] = Max Horizontal Rate in kHz
                                           range_limit[4] = Max Supported pixel clock rate in MHz/10 */
    
    u32       dwNumVidSupport;        /* 支持的视频制式的个数， 来自于对显示设备EDID信息的分析，
                                           通过XXXX-XXX-XXX命令来获取具体的制式的视频制式，第一个视频制式
                                           是推荐的分辨率*/
    u32       dwAudioSupport;         /* EDID里面对音频的支持情况，等于0表示不支持音频 */
} TVidOutTVInfo;


/* 视频复用输出结构定义 */
typedef struct{
    u32	    dwVDPort;                   /* 数字视频输出口: 0=vp1 1=vp2... */
    u32	    dwMuxChnlNum;               /* 几路视频复用，传0的话默认为1路 */
    u32	    dwMuxMode;                  /* 视频复用方式，0=pix Mux; 1=line Mux */
    u32	    dwMuxChnlMask;              /* 视频输入通道掩码: D[3-0]=chnl1 D[7-4]=chnl2 ... */
} TVidInMuxOutput;

/* TitleGen图片数据格式结构定义 */
typedef struct{
    u32	    dwPicId;                    /* 图片数据索引，从0开始编号 */
    u32	    dwPicWidth;                 /* 图片数据窗口宽，像素为单位 */
    u32	    dwPicHeight;                /* 图片数据窗口高，行为单位 */
    u32	    dwPicFmt;                   /* 图片数据格式，0默认为UYVY的16位YUV422格式,1=YV16格式 */
    void    *pbyPicData;                /* 图片数据在用户空间的地址 */
    u32     dwPicSize;                  /* 图片数据的字节数 */
} TVidTitlePicLoad;


typedef struct{
    u32       dwIntfIn;                   /* TitleGen的源接口选择 */
    TVidStd   tVidStd;                    /* 视频层制式 */

    u32	    dwOsdPosX;                  /* OSD窗口X方向坐标，像素为单位 */
    u32	    dwOsdPosY;                  /* OSD窗口Y方向坐标，行为单位 */
    u32	    dwOsdWidth;                 /* OSD窗口宽，像素为单位 */
    u32	    dwOsdHeight;                /* OSD窗口高，行为单位 */

    u32         dwTransVal;                 /* 全局透明度，范围: 0-0xff, 为0时为全透，OSD层不参与合成，Vid层直接透传 */
    u32       dwBackGroundY;              /* 全透的背景色Y分量值，范围: 0x10-0xf0, OSD层每个点的Y值等于背景色的话
                                            不参与合成，Vid层直接透传该点的数据 */

    u32	      dwDrawPicId;                /* 显示指定索引的图片，前提是指定索引的图片已经上传过 */
    u32	      dwDrawMode;                /* 图片显示模式，0=静止; 1=从右向左滚动叠加OSD */
    u32       dwDrawTimes;                /* dwDrawMode!=0时有效：循环显示整个OSD图片的次数 */

    /* movement's step in hor direction */
    u32       dwDrawMvStep;
    /* movement's step in ver direction */
    u32       dwPauseLines;
    /* a loop consist of  N windows, the interval time between the (N-1) windows */
    u32       dwDrawMvDelay;
    /* a loop consist of  N windows, the interval time between the (N-1)  and N  window*/
    u32       dwPauseInterval;


    /* contain displayed window's num per loop  */
    u32       disp_win_num;
    /* the interval time between displayed window */
    u32       loop_osd_intval;
} TVidTitlePicDraw;



/* TitleGen图片显示状态结构定义 */
typedef struct{
    u32	    dwPicId;                    /* 图片数据索引，从0开始编号 */
    u32     dwDrawedTimes;              /* dwDrawMode!=0时有效：指示已经循环显示过整个OSD图片的次数，当设置无限次滚动时无效 */
} TVidTitleStat;

/* 音频格式结构定义 */
typedef struct{
    u32 dwSampleRate;                   /* 采样频率(Hz)，0=无音频或不输出音频; 正常值为:8000,32000,48000,96000 */
    u32 dwSampleBits;                   /* 采样位宽：16bit/20bit/24bit/32bit */
	u32 dwChnlNum;                      /* 声道个数：1/2/3(2.1)/6(5.1杜比环绕) */
} TAudStd;

/* E2PROM 参数结构体 */
typedef struct{
    u32 dwHardwareVersion;
    u32 dwDeviceCode;
    u32 dwProductId;
    u32 dwHWSubVersion;
    u32 dwProductDate;
    s8  chDeviceSerial[11];
    s8  chFlowCode[7];
}TBrdE2PromInfo;

/*数字mic结构体*/
typedef struct {
	int magicnum;						/*表示升级功能0x646d7675*/
	int len;							/*版本的长度*/
	int crc;							/*版本数据的crc校验码*/
	int ex;								/*目前未用，作为扩展*/
}dig_mic;

/*lm75温度传感器告警接口*/
typedef struct{
	u32 temp_alarm;
}tempalarm;
/*lm75传感器结构体*/
typedef struct {
	int temp;
	u32 temp_os;
	u32 temp_hyst;
}lm75;

/* 结构声明：
  link:  0-fail;    1-pass
  duplex: 0-half;   1-full
  speed: 10-10Mbps; 100-100Mbps; 1000-1000Mbps)
*/
typedef struct{
    u8 Link;                      /* 以太网接口连接状态 */
    u8 AutoNeg;                   /* 以太网接口协商，该状态不上传,只是为了兼容其他设备 */
    u8 Duplex;                    /* 以太网接口双工 */
    u32 Speed;                     /* 以太网接口速率 */
}TBrdEthInfo;
#define SW_ETH_MAXNUM 				26

struct SWEthInfoRaw
{
	TBrdEthInfo eth_info[SW_ETH_MAXNUM];
} ;

#define INSIDE_ETH_MAXNUM 		16
#define FRONT_PANE_ETH_MAXNUM 	2
#define BACK_PANE_ETH_MAXNUM 	8

/*交换芯片提供的网络接口分为三类：分别是内部网络接口，前面板网络接口（下面的2个接口，最上面的为IS3-8548的网络接口），背板网络接口 */
struct SWEthInfo
{
	TBrdEthInfo inside_eth_info[INSIDE_ETH_MAXNUM];
	TBrdEthInfo front_eth_info[FRONT_PANE_ETH_MAXNUM];
	TBrdEthInfo back_eth_info[BACK_PANE_ETH_MAXNUM];
};

/*增加boardinfo，cpuinfo和meminfo结构体体来查询板子信息*/
#define SIZE_WORD 32
#define SIZE_DWORD 64
#define SIZE_BYTE 8
/*系统proc文件信息，/proc/boardinfo,/proc/meminfo,/proc/cpuinfo*/
typedef struct {
	u8 boardName[SIZE_WORD];/*board name*/
	u8 cpuType[SIZE_WORD];/*cpu*/
	u8 bootVer[SIZE_DWORD];/*boot version*/
	u8 kernelVer[SIZE_DWORD];/*kernel version*/
	u32 memSize;/*memory size*/
	u32 flashSize;/*flash size*/
	u32 cpuFreq;/*cpu frequency*/
	u32 memFreq;/*memory frequency*/
	u32 CPLDVer;/*CPLD version*/
	u32 FPGAVer;/*FPGA version*/
	
	u8 cpuName[SIZE_WORD];/*Processor*/
	float cpuBogoMIPS;/*BogoMIPS*/
	u8 cpuFeatures[SIZE_DWORD];/*Features*/
	u8  cpuImplementer[SIZE_BYTE];/*CPU implementer*/
	u32 cpuArch;/*CPU architecture*/
	u8 cpuVariant[SIZE_BYTE];/*CPU variant*/
	u8 cpuPart[SIZE_BYTE];/*CPU part*/
	u32 cpuRevision;/*CPU revision*/

	u8 brdHardware[SIZE_WORD];/*Hardware*/
	u32 brdRevision;/*Revision*/
	u8 brdSerial[SIZE_WORD];/*Serial*/

	u32 sysMemToltal;/*MemTotal*/
	u32 sysMemFree;/*MemFree*/
	u32 sysMemBuffers;/*Buffers*/
	u32 sysMemCached;/*Cached*/
}TBrdSysProcInfo;

typedef struct {
	u32 dwIpAdrs;/*ip地址，网络字节序*/
	u32 dwIpMask; /*子网掩码，网络字节序*/
	u8  byMacAdrs[6];/*MAC地址，网络字节序*/

	u8 brdDhcpStatus;/*DHCP状态，1为启用，0为不启用*/
	u32 brdDefGateway;/*默认网关*/
}TBrdNetInfo;

typedef struct {
    u32 byBoardID;   /*设备ID号*/
    u32 byHwVer;     /*硬件版本号*/
    u8 byFpgaVer;   /*FPGA版本号*/
    u8 byBoardType; /*设备类别:0--终端类;1--插板类1(有槽位号和层号);2--插板类2(有槽位号无层号) */
    u8 byEthNum;    /*设备以太网个数*/
    u8 byEth0Name[INTERFACE_NAME_MAX_LEN];/*设备以太网0的名字*/
    u8 byEth1Name[INTERFACE_NAME_MAX_LEN];/*设备以太网0的名字*/
    u8 byEth2Name[INTERFACE_NAME_MAX_LEN];/*设备以太网0的名字*/
    u8 byE1Num;     /*设备E1个数，0代表该设备不支持E1*/
    u8 byV35Num;    /*设备V35个数,0代表该设备不支持V35*/
    u8 byBspNum;    /*设备BSP个数,0代表该设备没有BSP-15媒体芯片*/
    u8 byFanCtrlSupport;    /*设备是否支持风扇控制:0---不支持，1---支持*/
    u8 byRTCType;   /*设备RTC芯片的类型:间相关的宏定义--RTC_TYPE_NONE/RTC_TYPE_DS1337/RTC_TYPE_DS12C887*/
    u8 abyRS232Name[TTY_NAME_MAX_LEN];/*设备RS232串口对应的设备名/MPC板console口对应的设备名*/
    u8 abyRS422Name[TTY_NAME_MAX_LEN];/*设备RS422串口对应的设备名/MPC板SCC2对应的设备名*/
    u8 abyRS485Name[TTY_NAME_MAX_LEN];/*设备RS485串口对应的设备名/MPC板SCC3对应的设备名*/
    u8 abyIrayName[TTY_NAME_MAX_LEN]; /*设备红外串口对应的设备名*/
    s32 brdSubCardStat; /*是否有子卡*/
}TBrdHwInfo;

typedef struct {
	TBrdE2PromInfo brdE2PromInfo;
	TBrdHwInfo brdHwInfo;
	TBrdPosition brdBrdPosition;
	TBrdSysProcInfo brdSysProcInfo;
	TBrdNetInfo brdNetInfo[ETH_MAC_MAX_NUM];
}TBrdAllInfo;
/* ----------------------------- 函数接口定义 -------------------------------*/

/*====================================================================
    函数名      : BrdWrapperInit
    功能        ：本模块的初始化函数，应用必须首先调用该函数完成本模块的初始
                  化工作，否则以下某些函数会无法正常工作，该函数可以多次调用。
    输入参数说明：无;
    返回值说明  ：错误返回ERROR；成功返回OK
---------------------------------------------------------------------*/
STATUS BrdWrapperInit(void);

/*====================================================================
    函数名      : BrdQryCapability
    功能        ：查询设备能力集，之前必须先BrdWrapperInit初始化。
    输入参数说明：无
    返回值说明  ：错误返回NULL；
                成功返回本模块维护的设备能力集信息的全局结构变量的地址，用户
                不要对其写入操作，否则会破坏里面的信息
---------------------------------------------------------------------*/
TBrdCapability * BrdQryCapability(void);

/*====================================================================
    函数名      : BrdGetBrdWrapperVer
    功能        ：本模块版本号查询。
    输入参数说明：pchVer： 给定的存放版本信息的buf指针
                 dwBufLen：给定buf的长度
                 pdwVerLen：版本的实际字符串长度，该长度返回之前必须和dwBufLen
                           比较是否溢出，正常时才对pchVer赋值！
    返回值说明  ：无。如果实际字符串长度大于dwBufLen，pdwVerLen赋值为0
---------------------------------------------------------------------*/
void  BrdGetBrdWrapperVer(s8 *pchVer, u32 dwBufLen, u32 *pdwVerLen);

/*====================================================================
    函数名      : BrdGetBoardID
    功能        ：设备身份ID查询。
    输入参数说明：无;
    返回值说明  ：如：单板种类ID号宏定义
---------------------------------------------------------------------*/
u32  BrdGetBoardID(void);

/*====================================================================
    函数名      : BrdGetMasterVideoMode
    功能        ：Get Mater Video Mode on KDM2860
    输入参数说明：NULL
    返回值说明  ： mode=1 VGA mode
   		   mode=0 HDMI mode
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/4/9            1.0            zhaoweixing        add BrdGetMasterVideoMode()
====================================================================*/
int BrdGetMasterVideoMode(void);

/*====================================================================
    函数名      : BrdGetAlarmMuteStatus
    功能        ：Get Alarm Mute Status on KDM2860
    输入参数说明：NULL
    返回值说明  :		1: Alarm mute
    				    2: NO mute
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/5/5            1.0            mayichang       创建
====================================================================*/
int BrdGetAlarmMuteStatus(void);

/*================================
函数名：BrdGetFuncID
功能：设备功能ID查询
算法实现：（可选项）
引用全局变量：
输入参数说明：  无
返回值说明： 单板功能ID
==================================*/
u8  BrdGetFuncID(void);

/*====================================================================
函数名      : BrdExtModuleIdentify
功能        ：终端外挂模块识别函数
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：无
返回值说明  ：外挂模块ID号：
            0为E1模块，1为4E1模块，2为V35DTE模块，3为V35DCE模块，4为网桥模块，5为2E1模块,
            6为KDV8000B模块, 0xc为DSC模块,目前MDSC也是这个ID号，下一版的MDSC将改变为0xd,0xe为HDSC模块，
            0xf 没有外挂模块
====================================================================*/
u8   BrdExtModuleIdentify(void);


/*====================================================================
    函数名      : BrdQueryHWVersion
    功能        ：硬件版本号查询。
    输入参数说明：无;
    返回值说明  ：硬件版本号,0~0xff
---------------------------------------------------------------------*/
u8  BrdQueryHWVersion (void);

/*====================================================================
    函数名      : BrdQueryFPGAVersion
    功能        ：FPGA或EPLD版本号查询。
    输入参数说明：无;
    返回值说明  ：FPGA或EPLD版本号,0~0xff
---------------------------------------------------------------------*/
u8  BrdQueryFPGAVersion (void);

/*====================================================================
    函数名      : BrdLedStatusSet
    功能        ：设置灯的状态
    算法实现    ：（可选项）
    引用全局变量：无
    输入参数说明：byLedID:灯的ID号（见brdwrapperdef.h中相关的定义）
                byState:灯的状态（见brdwrapperdef.h中相关的定义）
    返回值说明  ：成功返回OK,失败返回ERROR
--------------------------------------------------------------------*/
STATUS BrdLedStatusSet(u8 byLedID, u8 byState);

/*====================================================================
    函数名      : BrdQueryLedState
    功能        ：指示灯闪灯模式查询
    输入参数说明：ptBrdLedState：存放灯的状态的结构体指针
    返回值说明  ：成功返回OK,失败返回ERROR
---------------------------------------------------------------------*/
STATUS BrdQueryLedState(TBrdLedState *ptBrdLedState);

/*====================================================================
    函数名      : SysOpenWdGuard
    功能        ：打开系统守卫函数
    输入参数说明： dwNoticeTimeout：通知消息时间，如果应用程序超过这个时间
                  还没有调用SysNoticeWdGuard喂狗，系统将复位。时间单位为秒。
				 如果设置为0，则认为应用程序不需要喂狗，由系统自动喂狗。
    返回值说明  ：成功返回OK,失败返回ERROR
---------------------------------------------------------------------*/
STATUS SysOpenWdGuard(u32 dwNoticeTimeout);

/*====================================================================
    函数名      : SysCloseWdGuard
    功能        ：关闭系统守卫函数，由系统负责喂狗
    输入参数说明：无
    返回值说明  ：成功返回OK,失败返回ERROR
---------------------------------------------------------------------*/
STATUS SysCloseWdGuard(void);

/*====================================================================
    函数名      : SysNoticeWdGuard
    功能        ：应用程序通知操作系统守卫运行。当打开系统守卫后，应用程序就
                 必须在注册所设定的时间内调用这个接口，否则系统将复位。
    输入参数说明：无
    返回值说明  ：成功返回OK,失败返回ERROR
---------------------------------------------------------------------*/
STATUS SysNoticeWdGuard(void);

/*====================================================================
    函数名      : SysWdGuardIsOpened
    功能        ：获取系统守卫状态。
    输入参数说明：无
    返回值说明  ：打开返回TRUE；关闭返回FALSE
---------------------------------------------------------------------*/
BOOL32 SysWdGuardIsOpened(void);

/*====================================================================
    函数名      : BrdGetAlarmInput
    功能        ：查询现场告警状态。
    输入参数说明：byPort: 告警输入端口号(KDM2402有2个告警输入，0-1)
                 pbyState: 存放告警输入值状态的指针（值定义：0:低电平；1:高电平）
    返回值说明  ：错误返回ERROR；成功返回OK
---------------------------------------------------------------------*/
STATUS BrdGetAlarmInput(u8 byPort, u8* pbyState);

/*====================================================================
    函数名      : BrdSetAlarmOutput
    功能        ：设置现场告警
    输入参数说明：byPort: 告警输出端口号(KDM2402有1个告警输入，0)
                 byState: 0:断开告警输出线路;1:导通告警输出线路
    返回值说明  ：错误返回ERROR；成功返回OK
---------------------------------------------------------------------*/
STATUS BrdSetAlarmOutput(u8 byPort, u8  byState);


/*====================================================================
    函数名      : SysRebootEnable
    功能        ：允许系统出现exception时自动复位,调试用
    输入参数说明：无
    返回值说明  ：无
--------------------------------------------------------------------*/
void  SysRebootEnable (void);

/*====================================================================
    函数名      : SysRebootDisable
    功能        ：禁止系统出现exception时自动复位,调试用
    输入参数说明：无
    返回值说明  ：无
--------------------------------------------------------------------*/
void  SysRebootDisable(void);

/*====================================================================
    函数名      : BrdHwReset
    功能        ：系统硬件复位,调试用
    输入参数说明：无
    返回值说明  ：无
--------------------------------------------------------------------*/
void  BrdHwReset(void);

/*====================================================================
    函数名      : BrdFxoCtrl
    功能        ：控制指定的FXO设备执行进行相应的操作
    输入参数说明：byDevID:  扩展用，目前填充0。
                 dwCmd： 控制命令ID：
                    #define FXO_SET_HANGUP       0x00  挂机
                    #define FXO_SET_PICKUP2CALL  0x01  摘机拨号
                    #define FXO_SET_TALK         0x02  通话
                    #define FXO_SET_RCV_RING     0x03  接收铃音
                    #define FXO_EN_REMOTE_SPEAK  0x04  允许对端电话用户发言
                    #define FXO_DIS_REMOTE_SPEAK 0x05  禁止对端电话用户发言
                    #define FXO_EN_REMOTE_LISTEN 0x06  允许对端电话用户听到会场发言
                    #define FXO_DIS_REMOTE_LISTEN 0x07 禁止对端电话用户听到会场发言
                    #define FXO_GET_STATE        0x80  查询FXO当前状态
                    #define FXO_SET_RING_VOLUME_IN   0x09		 环路音量输入(AD->环路)
                    #define FXO_SET_RING_VOLUME_OUT   0x0a		  环路音量输入(环路->AD)
                 pArgs：参数，和dwCmd相关，目前用到的情况如下，其他情况用户填NULL：
                    1)	FXO_GET_STATE
                    查工作状态时用户需要传入一个u32类型变量的指针，本接口写入
                    当前FXO工作状态情况，返回值定义如下：
                    #define FXO_STATE_RING       0x00   振铃
                    #define FXO_STATE_PICKUP     0x01   摘机
                    #define FXO_STATE_HANGUP     0x02   挂机
                    2)	FXO_SET_RING_VOLUME_IN          环路音量输入(AD->环路)
                    		允许值: 1，2，3，4 ，值越大，音量越大
                    3)  FXO_SET_RING_VOLUME_OUT          环路音量输入(环路->AD)
												允许值: 1，2，3，4 ，值越大，音量越大
    返回值说明  ：成功返回OK,失败返回ERROR
---------------------------------------------------------------------*/
STATUS BrdFxoCtrl(u8 byDevID, u32 dwCmd, void *pArgs);

/*====================================================================
    函数名      : BrdQueryPosition
    功能        ：板身份(ID、层号、槽位)查询
    输入参数说明：ptBrdPosition： 由该函数返回板身份信息的结构指针,用户分配
    返回值说明  ：错误返回ERROR；成功返回OK和板身份信息，具体如TBrdPosition结构描述
--------------------------------------------------------------------*/
STATUS BrdQueryPosition(TBrdPosition *ptBrdPosition);

/*====================================================================
    函数名      : BrdWakeup
    功能        ：唤醒设备
    输入参数说明：无
    返回值说明  ：成功返回OK ；失败返回ERROR
--------------------------------------------------------------------*/
STATUS  BrdWakeup(void);

/*====================================================================
    函数名      : BrdSleep
    功能        ：休眠设备
    输入参数说明：无
    返回值说明  ：成功返回OK ；失败返回ERROR
--------------------------------------------------------------------*/
STATUS  BrdSleep (void);

/*====================================================================
    函数名      : BrdSetSpeakerMode
    功能        ：设置扬声器通断模式
    输入参数说明：byMode: 0=关闭；1=打开
    返回值说明  ：成功返回OK ；失败返回ERROR
--------------------------------------------------------------------*/
STATUS  BrdSetSpeakerMode (u8 byMode);

/*====================================================================
    函数名      : BrdAlarmStateScan
    功能        ：底层告警信息扫描
    算法实现    ：（可选项）
    引用全局变量：无
    输入参数说明：ptBrdAlarmState:返回的告警信息
    返回值说明  ：成功返回OK ；失败返回ERROR
====================================================================*/
STATUS BrdAlarmStateScan(TBrdAlarmState *ptBrdAlarmState);

/*====================================================================
    函数名      : BrdGetWirelessInput
    功能        ：获取无线遥控命令。
    输入参数说明：pbyState:存放无线遥控命令的指针
    返回值说明  ：错误返回ERROR；成功返回OK
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/05/14    1.0         毛一山        创建
====================================================================*/
STATUS BrdGetWirelessInput(u8* pbyState);

/*====================================================================
    函数名      : BrdResetSubCard
    功能        ：复位指定的子板卡，需要100ms时间后返回
    输入参数说明：bySubCardID: 子卡编号，MPU最多支持7个子卡，范围: 0~6
    返回值说明  ：成功返回OK ；失败返回ERROR
--------------------------------------------------------------------*/
STATUS BrdResetSubCard(u8 bySubCardID);

/*====================================================================
    函数名      : BrdShutDownSubCard
    功能        ：打开关闭指定的子板卡，上电后默认所有子卡处于上电状态
    输入参数说明：bySubCardID: 子卡编号，MPU最多支持7个子卡，范围: 0~6
                 bShutDown: TRUE=关电/FALSE=上电
    返回值说明  ：成功返回OK ；失败返回ERROR
--------------------------------------------------------------------*/
STATUS BrdShutDownSubCard(u8 bySubCardID, BOOL32 bShutDown);

/*====================================================================
    函数名      : BrdGetSubCardInfo
    功能        ：查询子卡信息，目前MPU支持该功能。
    输入参数说明：dwSubCardID: 子卡的ID号，和单板有关:
                              MPU: 0~(MPU_SUBCARD_NUM-1)
                 ptBrdInfo: 存放子卡信息，用户分配buf传入指针
    返回值说明  ：成功返回OK,失败返回ERROR
--------------------------------------------------------------------*/
STATUS BrdGetSubCardInfo(u32 dwSubCardID, TBrdInfo *ptBrdInfo);

/*====================================================================
    函数名      : BrdSelConsole
    功能        ：切换外部的console接口到指定的cpu上
    输入参数说明：byCpuId: 在TS5210 TS6610 TS7210上0=master;1=slave
    返回值说明  ：成功返回OK ；失败返回ERROR
--------------------------------------------------------------------*/
STATUS BrdSelConsole(u8 byCpuId);

/*====================================================================
    函数名      : MspUpdate
    功能        ：烧录MSP单片机程序
                  注意：本接口使用了红外串口，因此调用该接口时用户最好不要
                  打开红外串口。
    输入参数说明：byDevID: 设备ID，从0开始编号
                 pbyFileName: 单片机程序文件名
    返回值说明  ：成功返回OK,失败返回ERROR
--------------------------------------------------------------------*/
STATUS MspUpdate(u8 byDevID, s8 *pbyFileName);

/*====================================================================
    函数名      : MspStm8Update
    功能        ：烧录红外单片机程序
                  注意：本接口使用了红外串口，因此调用该接口时用户最好不要
                  打开红外串口。
    输入参数说明：byDevID: 设备ID，从0开始编号
                 pbyFileName: 单片机程序文件名
    返回值说明  ：成功返回OK,失败返回ERROR
--------------------------------------------------------------------*/
STATUS MspStm8Update(u8 byDevID, s8 *pbyFileName);

/*====================================================================
    函数名      : IRDisplayTest
    功能        ：IRDISPLAY红外单片机显示测试程序
    算法实现    ：（可选项）
    引用全局变量：无
    输入参数说明：无
    返回值说明  ：成功返回IRDISPLAY_OK；失败返回系统定义的错误码
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/12/10    1.0         戈志明        创建
====================================================================*/
STATUS IRDisplayTest(char ver[]);
/*====================================================================
    函数名      : SysLoadFpga
    功能        : 装载FPGA数据
    输入参数说明 : bForceLoad: TRUE=不管当前fpga是否有程序强制重新装载
                            FALSE=如果已经加载过程序则跳过加载，节省时间
                 pbyFileName:
    返回值说明  : OK = 成功，ERROR = 失败
--------------------------------------------------------------------*/
STATUS SysLoadFpga(BOOL32 bForceLoad, u8 *pbyFileName);

/*====================================================================
    函数名      : FpgaProgramOpen
    功能        : 准备开始fpga编程
    输入参数说明 : 无
    返回值说明  : OK = 成功，ERROR = 失败
--------------------------------------------------------------------*/
STATUS FpgaProgramOpen(void);

/*====================================================================
    函数名      : FpgaProgramWrite
    功能        : 向fpga写入一段数据，初次调用前必须先调用BcsrProgramFpgaOpen，
                  可多次调用，直到写完所有数据
    输入参数说明 : pbyData:数据指针；dwSize:数据大小
    返回值说明  : OK = 成功，ERROR = 失败
--------------------------------------------------------------------*/
STATUS FpgaProgramWrite(u8 *pbyData, u32 dwSize);

/*====================================================================
    函数名      : BcsrProgramFpgaClose
    功能        : 结束fpga编程，同时启动fpga
    输入参数说明 : 无
    返回值说明  : OK = 成功，ERROR = 失败
--------------------------------------------------------------------*/
STATUS FpgaProgramClose(void);

/*====================================================================
    函数名      : BrdDetectedHaltSignal
    功能        ：查询是否检测到系统挂起信号，一般用按键来触发
    输入参数说明：无
    返回值说明  ：FALSE=正常；TRUE=检测到系统挂起信号
--------------------------------------------------------------------*/
BOOL32 BrdDetectedHaltSignal(void);

/*====================================================================
    函数名      : BrdDetectedWakeupSignal
    功能        ：查询是否检测到系统唤醒信号，一般用按键来触发
    输入参数说明：无
    返回值说明  ：FALSE=正常；TRUE=检测到系统唤醒信号
--------------------------------------------------------------------*/
BOOL32 BrdDetectedWakeupSignal(void);

/*====================================================================
    函数名      : BrdDetectedDefaultSignal
    功能        ：查询是否检测到系统恢复缺省配置信号，一般用按键来触发
    输入参数说明：无
    返回值说明  ：FALSE=正常；TRUE=检测到系统恢复缺省配置信号
--------------------------------------------------------------------*/
BOOL32 BrdDetectedDefaultSignal(void);

/*====================================================================
    函数名      : BrdGetEthLinkStat
    功能        ：获取以太网接口连接状态
    输入参数说明：u8 byEthId 以太网接口号；
                  u8 *Link   获取的以太网接口连接状态；
                             0-link down, 1-link up
    返回值说明  ：OK = 成功，ERROR = 失败
====================================================================*/
STATUS  BrdGetEthLinkStat(u8 byEthId, u8 *Link);

/*====================================================================
    函数名      : BrdGetEthNegStat
    功能        ：获取以太网接口连接状态
    输入参数说明：u8 byEthId 以太网接口号；
                  u8 *AutoNeg 以太网协商状态，1-自协商，0-强制
                  u8 *Duplex  以太网双工状态，1-全双工，0-半双工
                  u32 *Speed  以太网接口速率Mbps，10/100/1000
    返回值说明  ：OK = 成功，ERROR = 失败
====================================================================*/
STATUS  BrdGetEthNegStat(u8 byEthId, u8 *AutoNeg, u8 *Duplex, u32 *Speed);

/*====================================================================
    函数名      : BrdSetEthNego
    功能        ：获取以太网接口连接状态
    输入参数说明：u8 byEthId 以太网接口号；
                  u8 Duplex  以太网双工状态，1-全双工，0-半双工, 2-自协商
                  u32 Speed  以太网接口速率Mbps，10/100/1000，0-自协商
    返回值说明  ：OK = 成功，ERROR = 失败
====================================================================*/
STATUS  BrdSetEthNego(u8 byEthId, u8 Duplex, u32 Speed);

/*====================================================================
    函数名      : BrdGetHdmiRxAudInfo
    功能        ：获取指定HDMI接收器接口的音频信息，如采样率、数据位宽
    输入参数说明： dwDevId: HDMI索引，0开始编号；
                  ptInfo:  指向HDMI接口的音频信息,用户分配，驱动填写
    返回值说明  ：OK = 成功，ERROR = 失败
====================================================================*/
STATUS  BrdGetHdmiRxAudInfo(u32 dwDevId, THdmiAudInfo *ptInfo);

/*====================================================================
    函数名      : BrdSetHdmiTxAudInfo
    功能        ：设置指定HDMI发送器接口的音频参数，如采样率、数据位宽
    输入参数说明： dwDevId: HDMI索引，0开始编号；
                  ptInfo:  指向HDMI接口的音频信息,用户给定
    返回值说明  ：OK = 成功，ERROR = 失败
====================================================================*/
STATUS  BrdSetHdmiTxAudInfo(u32 dwDevId, THdmiAudInfo *ptInfo);

/*====================================================================
函数名      : BrdTimeGet
功能        ：取系统时间（不会从RTC中获取时间）
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：ptGettm：存放返回时间的结构指针；tm为系统的数据结构
返回值说明  ：错误返回ERROR；成功返回OK
说明：业务程序对本接口返回的tm结构中的年和月必须进行了处理之后才能传给用户，
具体处理要求是：tm中的年要加上1900；tm中的月要将加上1
====================================================================*/
STATUS BrdTimeGet( struct tm* ptTime);


/*====================================================================
函数名      : BrdTimeSet
功能        ：设置系统时间，如果有RTC则同步设置
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：ptGettm：存放要设置的时间的结构指针；tm为系统的数据结构
返回值说明  ：错误返回ERROR；成功返回OK
说明：业务程序对tm结构中的年和月必须进行了处理之后才能传给该接口，
具体处理要求是：tm中的年要将用户设置的数值减去1900；tm中的月要将用户设置的数值减去1
====================================================================*/
STATUS BrdTimeSet( struct tm* ptTime );

/*====================================================================
    函数名      : BcsrTi81xxEdma
    功能        ：Ti81xx Edma
    输入参数说明：srcPhyAddr: 源物理地址
    			  dstPhyAddr: 目的物理地址
    			  size: 数据大小
    返回值说明  ：错误返回ERROR；成功返回OK
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/03/11    1.0        周新星        创建
====================================================================*/
STATUS BcsrTi81xxEdma(u32 srcPhyAddr, u32 dstPhyAddr, u32 size);

/*====================================================================
    函数名      : BcsrTi81xxGetReservedMem
    功能        ：获取物理空间
    输入参数说明：dwPhyAddr: 物理地址
    			  dwVirAddr: 虚拟地址
    			  dwLen: 数据大小
    返回值说明  ：错误返回ERROR；成功返回OK
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/03/11    1.0        周新星        创建
====================================================================*/
u32 BcsrTi81xxGetReservedMem(u32 *dwPhyAddr, u32 *dwVirAddr, u32 dwLen);

/*====================================================================
    函数名      : BcsrTi81xxEdmaFill2D
    功能        ：Ti81xx Edma
    输入参数说明：srcPhyAddr: 源物理地址
    			  dstPhyAddr: 目的物理地址
    			  width:	  2D区域的宽
    			  height:	  2D区域的高
    			  sync:	  	  同步长度
    返回值说明  ：错误返回ERROR；成功返回OK
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/05/20    1.0        周新星        创建
====================================================================*/
STATUS BcsrTi81xxEdmaFill2D(u32 srcPhyAddr, u32 dstPhyAddr, u32 width, u32 height, u32 sync);

/*====================================================================
    函数名      : BcsrTi81xxEdma2DFill
    功能        ：Ti81xx Edma 2D Fill
    输入参数说明：pixPhyAddr: 源物理地址
    			  dstPhyAddr: 目的物理地址
    			  width:	  2D区域的宽
    			  height:	  2D区域的高
    			  pitch:	  目标同步长度
    			  bpp:		  像素字节数
    返回值说明  ：0，或者错误号
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/07/04    1.0        周新星        创建
====================================================================*/
u32 BcsrTi81xxEdma2DFill(u32 pixPhyAddr, u32 dstPhyAddr, u32 width, u32 height, u32 pitch, u32 bpp);

/*================================
函数名：BrdSetRtcToSysClock
功能：将rtc时间设为系统时间
算法实现：（可选项）
引用全局变量：
输入参数说明：
返回值说明： 成功返回OK，否则返回ERROR
==================================*/
STATUS BrdSetRtcToSysClock(void);

/*================================
函数名：BrdSetSysToRtcClock
功能：将系统时间设为rtc时间
算法实现：（可选项）
引用全局变量：
输入参数说明：
返回值说明： 成功返回OK，否则返回ERROR
==================================*/
STATUS BrdSetSysToRtcClock(void);


/*================================
函数名：BrdOpenSerial
功能：提供给业务层打开Rs232 Rs422 Rs485红外串口
算法实现：（可选项）
引用全局变量：
输入参数说明：byPort串口端口号(t300的串口数量比其他设备多，采用另一套端口号)
              #define SERIAL_RS232                 0
              #define SERIAL_RS422                 1
              #define SERIAL_RS485                 2            H600控制SE600也采用该端口
              #define BRD_SERIAL_INFRARED          3
T300对应的端口号:
#define SERIAL_RS232                 	0
#define BRD_SERIAL_RS422_0	   4
#define BRD_SERIAL_RS422_1            5
#define BRD_SERIAL_RS422_2            6
#define BRD_SERIAL_232_422_485_0	7
#define BRD_SERIAL_232_422_485_1      8
#define BRD_SERIAL_232_422_485_2      9
#define BRD_SERIAL_232_422_485_3      10
#define BRD_SERIAL_232_422_485_4      11
#define BRD_SERIAL_232_422_485_5      12
返回值说明： 打开串口的句柄
==================================*/
s32 BrdOpenSerial(u8 byPort);


/*================================
函数名：BrdCloseSerial
功能：提供给业务层关闭Rs232 Rs422 Rs485红外串口
算法实现：（可选项）
引用全局变量：
输入参数说明：  nFd：串口文件描述符，由BrdOpenSerial函数返回
返回值说明： 成功返回0，失败返回ERROR
==================================*/
s32 BrdCloseSerial(s32 nFd);


/*================================
函数名：BrdReadSerial
功能：提供给业务层读取Rs232 Rs422 Rs485串口
算法实现：（可选项）
引用全局变量：
输入参数说明：  nFd：串口文件描述符，由BrdOpenSerial函数返回
                pbyBuf：存放数据的缓冲区指针
                nMaxbytes：要读取的数据的最大字节数
返回值说明： 成功返回读取数据的字节数，范围1~ nMaxbytes，0表示没有数据，失败返回
            ERROR
==================================*/
s32  BrdReadSerial(s32 nFd, s8  *pbyBuf, s32 nMaxbytes);


/*================================
函数名：BrdWriteSerial
功能：提供给业务层写Rs232 Rs422 Rs485红外串口
算法实现：（可选项）
引用全局变量：
输入参数说明：  nFd：串口文件描述符，由BrdOpenSerial函数返回
                pbyBuf：要写入的数据的缓冲区指针
                nBytes：要写入数据的字节数
返回值说明： 成功返回写入数据的字节数，且等于nBytes，描述符无效返回ERROR，写入值
             不等于nBytes表示写入发生错误

==================================*/
s32 BrdWriteSerial (s32 nFd, s8  *pbyBuf, s32 nBytes);


/*================================
函数名：BrdIoctlSerial
功能：提供给业务层控制Rs232 Rs422 Rs485内置红外串口，同ioctl函数
算法实现：（可选项）
引用全局变量：
输入参数说明：  nFd：串口文件描述符，由BrdOpenSerial函数返回
                nFunction：操作功能码，见相关的串口操作宏定义
                nArg：参数
返回值说明： 成功返回OK，描述符无效或失败返回ERROR
==================================*/
s32 BrdIoctlSerial (s32 nFd, s32 nFunction, s32 nArg);


/*================================
函数名：BrdRs485QueryData
功能：Rs485查询数据，先发查询帧，后接收对端数据
算法实现：（可选项）
引用全局变量：
输入参数说明：  nFd：串口文件描述符，由BrdOpenSerial函数返回
		        ptRS485InParam：存放输入参数的结构指针；
		        ptRS485RtnData: 存放返回信息的结构指针。
返回值说明：  485状态相关宏定义
目前库里已不支持该接口
==================================*/
s32  BrdRs485QueryData (s32 nFd, TRS485InParam *ptRS485InParam,TRS485RtnData *ptRS485RtnData);


/*================================
函数名：BrdRs485TransSnd
功能：Rs485透明发送
算法实现：（可选项）
引用全局变量：
输入参数说明：  nFd：串口文件描述符，由BrdOpenSerial函数返回
                pbyMsg：待发送的数据；
                dwMsgLen: 数据长度
返回值说明： 485状态相关宏定义
目前库里已不支持该接口
==================================*/
s32  BrdRs485TransSnd (s32 nFd, u8 *pbyMsg, u32 dwMsgLen);


/*================================
函数名：BrdGetE1MaxNum
功能：查询设备最大可配置的e1个数
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： 最大配置的e1个数
==================================*/
u8 BrdGetE1MaxNum(void);


/*================================
函数名：BrdSetE1RelayLoopMode
功能：目前仅对DRI、DSI、KDV8010有效，主要设置E1链路继电器环回（正常、内环、外环）
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ID：E1链路的ID号，最大值和相关设备相关，具体如各单板最大E1个数宏定义;
            byMode：继电器工作模式设置，定义如下：
            #define BRD_RELAY_MODE_NORMAL     0    继电器连通
            #define BRD_RELAY_MODE_LOCLOOP    1    继电器自环
            #define BRD_RELAY_MODE_OUTLOOP    2    继电器外环
            说明：
            1：对DRI来说，只有正常、内环设置；
            2：对DSI来说，只有正常、外环设置；
            3：对MT来说，只有正常、内环设置，且和所插的模块相关。
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdSetE1RelayLoopMode(u8 byE1ID,u8 byMode);


/*================================
函数名：BrdQueryE1Imp
功能：目前仅对DRI、DSI、KDV8010有效，主要获取指定e1阻抗值
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ID：E1链路的ID号，最大值和相关设备相关，具体如各单板最大E1个数宏定义
返回值说明： 错误返回0xff；成功返回阻抗值宏定义
==================================*/
u8  BrdQueryE1Imp(u8 byE1ID);


/*================================
函数名：BrdGetE1AlmState
功能：获取指定E1线路的告警状态
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ID：E1链路的ID号，最大值和相关设备相关，具体如各单板最大E1个数宏定义
返回值说明： 返回0为正常，非0有告警
==================================*/
u8  BrdGetE1AlmState(u8 byE1Id);


/*================================
函数名：E1TransGetMaxE1Num
功能：获取设备实际支持的E1个数
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： 错误返回0并打印出错信息；成功, 返回设备实际支持的E1个数
==================================*/
u8 E1TransGetMaxE1Num(void);

/*================================
函数名      : BrdGetSensorStat
功能        ：查询当前系统感应器的状态
算法实现    ：
引用全局变量：
输入参数说明：sensor存放返回信息的结构指针。
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdGetSensorStat(lm75 * sensor);


/*================================
函数名      : BrdMicAdjustIsSupport
功能        ：查询当前mic调节是否支持
算法实现    ：
引用全局变量：
输入参数说明：无
返回值说明  ：错误返回FALSE；成功返回TRUE
==================================*/
BOOL32   BrdMicAdjustIsSupport(void);

/*================================
函数名      : BrdMicVolumeSet
功能        ：设置mic输入音量
算法实现    ：
引用全局变量：
输入参数说明：byTapPosition:音阶，范围：0-127,对音量为正逻辑
返回值说明  ：错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMicVolumeSet(u8  byTapPosition);

/*================================
函数名      : BrdMicVolumeGet
功能        ：查询当前mic输入音量
算法实现    ：
引用全局变量：
输入参数说明：无
返回值说明  ：当前mic输入音量(0-127)/0xff=错误
==================================*/
u8 BrdMicVolumeGet(void);

/*================================
函数名      : BrdGetSysInfo
功能        ：查询当前系统信息
算法实现    ：
引用全局变量：
输入参数说明：SysInfo存放返回信息的结构指针。
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdGetSysInfo(TBrdSysInfo *SysInfo);


/*================================
函数名      : BrdSetIrPin
功能        ：查询设置使用哪个红外遥控
算法实现    ：
引用全局变量：
输入参数说明：pin从0到7
返回值说明  ：错误返回ERROR；成功返回OK
==================================*/
STATUS BrdSetIrPin(u8 pin);


/*================================
函数名      : BrdSlaveGetOutputMode
功能        ：从系统获取视频输出模式
算法实现    ：
引用全局变量：
输入参数说明：无
返回值说明  ：  1 表示N 制；0 表示P 制；
					-1 表示主系统调用此函数?					其他表示错误
==================================*/
STATUS BrdSlaveGetOutputMode(void);


/*================================
函数名      : BrdGetVgaVideoState
功能        ：
算法实现    ：
引用全局变量：
输入参数说明：无
返回值说明  ：
==================================*/
s32 BrdGetVgaVideoState(void);

/* --------------------------------------视频输入输出适配模块----------------------------------- */

/*====================================================================
    函数名      : VidInApiMapIntfToVP
    功能        :映射指定VP口对应的物理输入接口
    输入参数说明 : dwCapChipId: 取值如宏定义: 视频采集芯片索引, 用于区别多个采集芯片
                   dwVPId: 视频采集口索引，0～ 用于区别相同采集芯片上不同的VP口
                   dwInterface: 对应的视频接口名称，
                                取值如:视频输入输出接口类型宏定义;
                                如果需要对输入口降帧处理应或上VID_DIV2宏
    返回值说明  : 错误返回ERROR；成功返回OK
---------------------------------------------------------------------*/
s32 VidInApiMapIntfToVP (u32 dwCapChipId, u32 dwVPId, u32 dwInterface);

/*====================================================================
    函数名      : VidInApiCtrl
    功能        ：视频采集芯片控制函数
    输入参数说明 :dwInterface: 对应的视频接口名称，如：视频输入输出接口类型宏定义
                               如果需要对输入口降帧处理时在设置输入接口制式
                               cmd=VID_CTRL_SET_STD时应或上VID_DIV2宏
		         cmd：操作命令，见: 视频输入输出设备控制类型宏定义；arg：参数
		         cmd = VID_CTRL_SET_STD/VID_CTRL_GET_STD时arg传(TVidStd*)指针
		         cmd = 其他，传递(int *)指针
    返回值说明  : 错误返回ERROR；成功返回OK
--------------------------------------------------------------------*/
s32 VidInApiCtrl (u32 dwInterface, s32 cmd, void *arg);

/*====================================================================
    函数名      : VidOutApiSelVidOutSrc
    功能        : 选择指定视频输出口对应的视频源接口
    输出参数说明 :dwOutIntf: 视频输出接口，取值如：视频输入输出接口类型宏定义
                             如果需要对输出口升帧处理应或上VID_DBL2宏
                 dwSrcIntf:  视频源的接口，取值如：视频输入输出接口类型宏定义
    返回值说明  : 错误返回ERROR；成功返回OK
---------------------------------------------------------------------*/
s32 VidOutApiSelVidOutSrc (u32 dwOutIntf, u32 dwSrcIntf);

/*====================================================================
    函数名      : VidOutApiCtrl
    功能        ：视频播放芯片控制函数
    输出参数说明 :dwInterface: 对应的视频接口名称，如:视频输入输出接口类型宏定义
                               如果需要对输出口升帧处理在设置输出接口制式
                               cmd=VID_CTRL_SET_STD时应或上VID_DBL2宏
		         cmd：操作命令，见: 视频输入输出设备控制类型宏定义；arg：参数
		         cmd = VID_CTRL_SET_STD/VID_CTRL_GET_STD时arg传(TVidStd*)指针
		         cmd = 其他，传递(int *)指针
    返回值说明  : 错误返回ERROR；成功返回OK
--------------------------------------------------------------------*/
s32 VidOutApiCtrl (u32 dwInterface, s32 cmd, void *arg);


/* --------------------------------------音频输入输出适配模块----------------------------------- */

/*====================================================================
    函数名      : AudInApiCtrl
    功能        ：音频采集芯片控制函数
    输入参数说明 :dwInterface: 对应的音频接口名称，如：音频输入输出接口类型宏定义
		         cmd：操作命令，见: 音频输入输出设备控制类型宏定义宏定义；arg：参数
    返回值说明  : 错误返回ERROR；成功返回OK
--------------------------------------------------------------------*/
s32 AudInApiCtrl (u32 dwInterface, s32 cmd, void *arg);

/*====================================================================
    函数名      : AudOutApiCtrl
    功能        ：音频播放芯片控制函数
    输入参数说明 :dwInterface: 对应的音频接口名称，如：音频输入输出接口类型宏定义
		         cmd：操作命令，见: 音频输入输出设备控制类型宏定义宏定义；arg：参数
    返回值说明  : 错误返回ERROR；成功返回OK
----------------------------------------------------------------------*/
s32 AudOutApiCtrl (u32 dwInterface, s32 cmd, void *args);


/*****************************************************************************
 ********************  E2PROM 控制模块 ***************************************
 ****************************************************************************/

/*====================================================================
函数名      : BrdGetE2PromInfo
功能        ：查询当前设备E2Prom信息的接口
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  ptBrdE2PromInfo:指向TBrdE2PromInfo结构体的指针
返回值说明  ：OK/ERROR
====================================================================*/
STATUS BrdGetE2PromInfo (TBrdE2PromInfo* ptBrdE2PromInfo);


/*================================
函数名：BrdClearE2promTestFlag
功能：清除eeprom测试位
算法实现：（可选项）
引用全局变量：
输入参数说明：
返回值说明： 成功返回OK，否则返回ERROR
==================================*/
STATUS BrdClearE2promTestFlag(void);

/*================================
函数名：BrdSetE2promTestFlag
功能：清除eeprom测试位
算法实现：（可选项）
引用全局变量：
输入参数说明：
返回值说明： 成功返回OK，否则返回ERROR
==================================*/
STATUS BrdSetE2promTestFlag(void);

/*================================
函数名：BrdGetE2promTestFlag
功能：检测eeprom测试位
算法实现：（可选项）
引用全局变量：
输入参数说明：
返回值说明： 需要测试返回TRUE，否则返回FALSE
==================================*/
BOOL32 BrdGetE2promTestFlag(void);

/*================================
函数名：BrdGetE2promIpAddr
功能：获得测试期间ip地址
算法实现：（可选项）
引用全局变量：
输入参数说明：dwIpAddr:回调返回的IP地址网络字节序
返回值说明： 成功返回OK，否则返回ERROR
==================================*/
STATUS BrdGetE2promIpAddr(u32* pdwIpAddr);


/*================================
函数名：BrdTestEEPromFunc
功能：验证eeprom的读写功能，对全部容量的eeprom进行数据写入、数据读取并校验。
算法实现：（可选项）
引用全局变量：
输入参数说明：
返回值说明： 成功返回OK，否则返回ERROR
==================================*/
STATUS BrdTestEEPromFunc(void);

/*================================
函数名：BrdEEPromGetPal
功能：获取是pal还是ntsc制式。
算法实现：（可选项）
引用全局变量：
输入参数说明：
返回值说明： pal返回OK，ntsc返回ERROR
==================================*/
STATUS BrdEEPromGetPal(void);

/*================================
函数名：BrdEEPromSetPal
功能：设置是pal还是ntsc制式。
算法实现：（可选项）
引用全局变量：
输入参数说明：set值为0表示pal制，为1表示ntsc制
返回值说明： 成功返回OK，失败返回ERROR
==================================*/
s32 BrdEEPromSetPal(s32 set);

/*================================
函数名：BrdEEPromGetOutputMode
功能：设置是pal还是ntsc制式。
算法实现：（可选项）
引用全局变量：
返回值说明：
			返回值为2表示输出为P制；4为N制，6为VGA；0表示无输出；
			#define EEPROM_LOG_OUTPUT_NULL	0x00
			#define EEPROM_LOG_OUTPUT_PAL	0x02
			#define EEPROM_LOG_OUTPUT_NTSC	0x04
			#define EEPROM_LOG_OUTPUT_VGA	0x06
==================================*/
STATUS BrdEEPromGetOutputMode(void);


/*================================
函数名：BrdEEPromSetOutputMode
功能：设置是pal还是ntsc制式。
算法实现：（可选项）
引用全局变量：
输入参数说明：set值为：
				#define EEPROM_LOG_OUTPUT_NULL	0x00
				#define EEPROM_LOG_OUTPUT_PAL	0x02
				#define EEPROM_LOG_OUTPUT_NTSC	0x04
				#define EEPROM_LOG_OUTPUT_VGA	0x06

返回值说明： 成功返回OK，失败返回ERROR
==================================*/
s32 BrdEEPromSetOutputMode(s32 set);

/*=========================================
函数名：BrdAd5246GetSensitivity
功能：  get sensititvity from ad5246.
算法实现：（可选项）
引用全局变量：
返回值说明：
			0---127:valid
			other:invalid
==========================================*/
s32 BrdAd5246GetSensitivity(void);

/*=========================================
函数名：BrdAd5246SetSensitivity
功能：  get sensititvity from ad5246.
算法实现：（可选项）
引用全局变量：
输入参数说明：value值为0--127 valid;
					    other invalid

返回值说明： 成功返回OK，失败返回ERROR
==========================================*/
STATUS BrdAd5246SetSensitivity(int  value);
/*=========================================
函数名：BrdEEPromSetHwSubVersion
功能： set hardware sub version in e2prom
算法实现：（可选项）
引用全局变量：
输入参数说明：hwsubver: hardware sub version

返回值说明： 成功返回OK，失败返回ERROR
==========================================*/
STATUS BrdEEPromSetHwSubVersion(unsigned short hwsubver);

/*=========================================
函数名：BrdEEPRromSetProductid
功能： set product id in e2prom
算法实现：（可选项）
引用全局变量：
输入参数说明：pid:product id

返回值说明： 成功返回OK，失败返回ERROR
==========================================*/
STATUS BrdEEPRromSetProductid(unsigned int pid);

/* =====================================
函数名：BrdEEPromSetUserData
功能！用户自定义数据设置
算法实现：（可选项）
引用全局变量：
参数说明
				pUserData: 指向用户自定义数据
				length: 数据长度
返回值说明  ：OK/ERROR
=====================================*/
STATUS BrdEEPromSetUserData(void *pUserData,u32 length);

/* =====================================
函数名：BrdEEPromGetUserData
功能：获取用户自定义数据
算法实现：（可选项）
引用全局变量：
返回值说明：
				获取失败返回NULL
				成功则返回void *
=====================================*/
void *BrdEEPromGetUserData(void);

/*================================
函数名      : BrdFpWriteDataToUsrFpn
功能        ：读取指定分区内指定段指定长度的有效数据到缓冲区
算法实现    ：（可选项）
引用全局变量：
输入参数说明：byIndex：第几个用户分区（从0开始）
              pbyData：要读出的数据指针
              dwLen：要读出数据的长度
返回值说明  ：实际读取的数据长度
==================================*/
s32  BrdFpReadDataFromUsrFpn(u8 bySection, u8 *pbyDesBuf, u32 dwLen);

/*================================
函数名      : BrdMPCQueryAnotherMPCState
功能        ：对端主处理机板是否在位查询
算法实现    ：（可选项）
引用全局变量：
输入参数说明：无
返回值说明  ：宏定义：
		  #define BRD_MPC_OUTOF_POSITION      ((u8)0)   //对端主处理机板不在位
          #define BRD_MPC_IN_POSITION               ((u8)1)   //对端主处理机板在位
==================================*/
u8 BrdMPCQueryAnotherMPCState(void);

/*================================
函数名      : BrdMPCQueryLocalMSState
功能        ：当前主处理机板主备用状态查询
算法实现    ：（可选项）
引用全局变量：
输入参数说明：无
返回值说明  ：宏定义：
		  #define BRD_MPC_RUN_MASTER      ((u8)0)   // 当前主处理机板主用
          #define BRD_MPC_RUN_SLAVE               ((u8)1)   // 当前主处理机板备用
==================================*/
u8 BrdMPCQueryLocalMSState (void);

/*================================
函数名      : BrdMPCSetLocalMSState
功能        ：设置当前主处理机板主备用状态
算法实现    ：（可选项）
引用全局变量：
输入参数说明：byState:
              #define BRD_MPC_RUN_MASTER          0      // 当前主处理机板主用
              #define BRD_MPC_RUN_SLAVE           1      // 当前主处理机板备用
返回值说明  ：OK/ERROR
==================================*/
STATUS BrdMPCSetLocalMSState (u8 byState);

/*================================
函数名      : BrdMPCQuerySDHType
功能        ：光模块种类查询
算法实现    ：（可选项）
引用全局变量：
输入参数说明：无
返回值说明  ：宏定义：
		#define BRD_MPC_SDHMODULE_NONE      ((u8)0x07)   // 当前主处理机板没有插模块
        #define BRD_MPC_SDHMODULE_SOI1      ((u8)0x00)   // 当前主处理机板插模块SOI1
        #define BRD_MPC_SDHMODULE_SOI4      ((u8)0x01)   // 当前主处理机板插模块SOI4
==================================*/
u8 BrdMPCQuerySDHType (void);

/*================================
函数名      : BrdMPCQueryNetSyncMode
功能        ：锁相环工作模式查询
算法实现    ：（可选项）
引用全局变量：
输入参数说明：无
返回值说明  ：宏定义：
		#define SYNC_MODE_FREERUN           ((u8)0)   // 自由振荡，对于MCU应设置为该模式，所有下级线路时钟与该MCU同步
        #define SYNC_MODE_TRACK_SDH8K       ((u8)1)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的8K时钟
        #define SYNC_MODE_TRACK_SDH2M       ((u8)2)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的2M时钟
        #define SYNC_MODE_TRACK_DT2M        ((u8)4)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪DT来的2M时钟
        #define SYNC_MODE_UNKNOWN           ((u8)0xff)// 未知或错误的模式
==================================*/
u8 BrdMPCQueryNetSyncMode (void);

/*================================
函数名      : BrdMPCSetNetSyncMode
功能        ：锁相环网同步模式选择设置，不支持SDH部分
算法实现    ：（可选项）
引用全局变量：
输入参数说明：byMode：锁相环网同步模式，有如下定义：
        #define SYNC_MODE_FREERUN           ((u8)0)   // 自由振荡，对于主MCU应设置为该模式，所有下级线路时钟与该MCU同步
        #define SYNC_MODE_TRACK_SDH8K       ((u8)1)   //跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的8K时钟
        #define SYNC_MODE_TRACK_SDH2M       ((u8)2)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的2M时钟
        #define SYNC_MODE_TRACK_DT2M        ((u8)4)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪DT来的2M时钟
返回值说明  ：错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMPCSetNetSyncMode(u8 byMode);

/*================================
函数名      : BrdMPCSetAllNetSyncMode
功能        ：锁相环网同步模式选择设置,增加sdh部分
算法实现    ：（可选项）
引用全局变量：
输入参数说明：byMode：锁相环网同步模式，有如下定义：
        #define SYNC_MODE_FREERUN           ((u8)0)   // 自由振荡，对于主MCU应设置为该模式，所有下级线路时钟与该MCU同步
        #define SYNC_MODE_TRACK_SDH8K       ((u8)1)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的8K时钟
        #define SYNC_MODE_TRACK_SDH2M       ((u8)2)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的2M时钟
        #define SYNC_MODE_TRACK_DT2M        ((u8)4)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪DT来的2M时钟
        dwSdhE1Id：对于同步模式为SYNC_MODE_TRACK_SDH8K和SYNC_MODE_TRACK_SDH2M时必须指定sdh对应的e1号，如果是SOI-1模
        块范围为241-301；如果是SIO-4模块时范围为241-484
返回值说明  ：错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMPCSetAllNetSyncMode(u8 byMode, u32 dwSdhE1Id);

/*================================
函数名      : BrdMPCResetSDH
功能        ：光模块复位
算法实现    ：（可选项）
引用全局变量：
输入参数说明：无
返回值说明  ：错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMPCResetSDH (void);

/*================================
函数名      : BrdMPCReSetAnotherMPC
功能        ：复位对端主处理机
算法实现    ：（可选项）
引用全局变量：
输入参数说明：无
返回值说明  ：错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMPCReSetAnotherMPC (void);

/*================================
函数名      : BrdMPCOppReSetDisable
功能        ：禁止对端主处理机复位本主处理机
算法实现    ：（可选项）
引用全局变量：
输入参数说明：无
返回值说明  ：错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMPCOppReSetDisable (void);

/*================================
函数名      : BrdMPCOppReSetEnable
功能        ：允许对端主处理机复位本主处理机
算法实现    ：（可选项）
引用全局变量：
输入参数说明：无
返回值说明  ：错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMPCOppReSetEnable (void);

/*================================
函数名      : BrdMPCLedBoardSpeakerSet
功能        ：设置灯板上扬声器蜂鸣的状态
算法实现    ：（可选项）
引用全局变量：
输入参数说明：byState：扬声器状态，如下宏定义
        #define LED_BOARD_SPK_ON                ((u8)0x01)   //开启扬声器
        #define LED_BOARD_SPK_OFF               ((u8)0x00)   // 关闭扬声器
返回值说明  ：错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMPCLedBoardSpeakerSet(u8 byState);

/*================================
函数名：BrdSetE1SyncClkOutputState
功能：目前仅对DRI、DSI有效，主要设置从E1线路上提取出的网同步参考时钟是否输出给MPC板进行同步，
      同一时刻绝对禁止不同的板子同时输出时钟，否则MPC无法同步
算法实现：（可选项）
引用全局变量：
输入参数说明：byMode：网同步参考时钟输出模式，定义如下：
                #define E1_ CLK_OUTPUT_DISABLE   ((u8)0)   高阻禁止板上时钟信号输出到MPC板上
                #define E1_ CLK_OUTPUT_ENABLE    ((u8)1)   允许板上时钟信号输出到MPC板上
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdSetE1SyncClkOutputState (u8 byMode);

/*================================
函数名：BrdSelectE1NetSyncClk
功能：目前仅对DRI、DSI有效，主要设置网同步参考时钟输出选择。
注意：该功能必须在BrdSetE1SyncClkOutputState设置了同步参考时钟输出允许的条件下才生效
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ID：E1链路的ID号，最大值和相关设备相关，具体如各单板最大E1个数宏定义
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdSelectE1NetSyncClk(u8 byE1ID);

/*================================
函数名：BrdSelectE1NetSyncClk
功能：目前仅对DRI、DSI有效，主要设置网同步参考时钟输出选择。
注意：该功能必须在BrdSetE1SyncClkOutputState设置了同步参考时钟输出允许的条件下才生效
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ID：E1链路的ID号，最大值和相关设备相关，具体如各单板最大E1个数宏定义
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdSelectE1NetSyncClk(u8 byE1ID);

/*================================
函数名：BrdMPCFanBrdSetFanSpeed
功能：电源板电压值AD
算法实现：（可选项）
引用全局变量：
输入参数说明：byState: 0~100对应速度百分比（应在10%以上）
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMPCFanBrdSetFanSpeed(u8 bySpeed);

/*================================
函数名：MPCFanBrdSetSpeaker
功能：风扇板Speaker控制
算法实现：（可选项）
引用全局变量：
输入参数说明：byState :Bit0，1：响；0，不响；
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMPCFanBrdSetSpeaker(u8 byState);

/*================================
函数名：BrdMPCFanBrdGetVersion
功能：获取版本号消息
算法实现：（可选项）
引用全局变量：
输入参数说明：	id :  	0对应监控板；1对应风扇板
					    version: 返回版本号
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMPCFanBrdGetVersion(u8 id, u16* version);

/*================================
函数名：BrdMPCLedBrdGetVoltage
功能：电源板电压值AD
算法实现：（可选项）
引用全局变量：
输入参数说明：dbyVol5:  12V AD电压=高字节.低字节（V，精度0.1V）
					    dbyVol12: 5V AD电压=高字节.低字节（V，精度0.1V）
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMPCLedBrdGetVoltage(u16* dbyVol5, u16* dbyVol12);

/*================================
函数名：BrdMPCLedBrdTempCheck
功能：电源板温度采集
算法实现：（可选项）
引用全局变量：
输入参数说明：pbyPowerTempRst：返回状态
					   0x02位表示右电源状态，0，温度正常；1，温度异常；
					   0x01位表示左电源状态，0，温度正常；1，温度异常；

返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMPCLedBrdTempCheck(u8* pbyPowerTempRst);

/*================================
函数名：BrdMPCFanBrdGetFanSpeed
功能：电源板电压值AD
算法实现：（可选项）
引用全局变量：
输入参数说明：byState : 0~7对应8个风扇ID
					   *speed: 返回速度指针
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMPCFanBrdGetFanSpeed(u8 byState, u16* Speed);

/*================================
函数名：MPCFanBrdSetFanSpeed
功能：电源板电压值AD
算法实现：（可选项）
引用全局变量：
输入参数说明：bySpeed: 0~100对应速度百分比（应在10%以上）
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS MPCFanBrdSetFanSpeed(u8 bySpeed);
/*================================
函数名      : BrdGetSensor
功能        ：查询传感器告警   :电压告警，温度转速信息
算法实现    ：（可选项）
引用全局变量：g_tBrdCapability
输入参数说明：sensor存放返回信息的结构指针TSensorAll
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdGetSensor(TBrdSensor *sensor);
/*====================================================================
函数名      : BcsrE1AllAlmDetail
功能        ：判断设备所有的E1接口的告警信息
算法实现    ：（可选项）
引用全局变量：无
输入参数说明： TBrdE1AlarmDetail ，新E1和老E1 模块详细告警信息

返回值说明： 错误返回ERROR；成功返回OK
====================================================================*/
STATUS AllE1AlmDetail(TBrdE1AlarmAll *e1AlmArray);

/*================================
函数名      : BrdGetSensorAlarm
功能        ：查询设备感应器的告警状态，电压，温度，风扇转速告警
算法实现    ：
引用全局变量：无
输入参数说明：TSensorAll，sensor告警信息结构指针
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdGetSensorAlarm(TBrdSensorAlarm *sensorAlarm);
/*====================================================================
函数名      : BrdGetAlarm
功能        ：获取终端所有告警信息，E1、电压、温度、风扇转速
算法实现：（可选项）
引用全局变量：g_tBrdCapability
输入参数说明：TBrdKDVAlarm，终端告警信息结构体指针
返回值说明  ：错误返回ERROR；成功返回OK
====================================================================*/
STATUS BrdMtGetAlarm(TBrdKDVAlarm *Alarm);

/*====================================================================
函数名      : get_kernel_log
功能        ：读取内核打印信息。
输入参数说明：buf缓冲区指针；size为buf大小；pathname为可选项,为要写入的文件。
返回值说明  ：pathname为NULL，成功返回ret；pathname不为NULL，成功返回0.
====================================================================*/
STATUS get_kernel_log(char *buf, int size,char *pathname);

/*====================================================================
函数名      : update_dig_mic
功能        ：数字mic在线升级
输入参数说明: buff为数据缓冲区,len为版本长度,uart_num为数字mic串口设备号（0 or 1）
返回值说明  ：升级成功返回OK，失败返回ERROR
====================================================================*/
STATUS update_dig_mic(char* buff, int len, int uart_num);

/*===================================================================
函数名      : get_lm75temp_alarm
功能        ：获取lm75温度传感器的温度告警状态
输入参数说明：sensoralarm 为传入的tempalarm结构体指针
返回值说明  ：成功返回OK，失败返回ERROR
====================================================================*/
STATUS get_lm75temp_alarm(tempalarm *sensoralarm);

/*====================================================================
函数名      : get_digmic_ver
功能        ：数字mic版本号查询
输入参数说明: ver_buf为接收版本号的指针,uart_num为数字mic串口号(0 or 1)
返回值说明  ：查询成功返回OK，失败返回ERROR
====================================================================*/
int get_digmic_ver(char *ver_buf, int uart_num);

/*================================
函数名      : BrdSwitchPort
功能        ：H600用于切换UART2的功能
算法实现    ：
引用全局变量：
输入参数说明：控制摄像头，0；摄像头升级，1；数字mic升级，2.
==================================*/
s32 BrdSwitchPort(u8 mode);


/*================================
函数名      : BrdSwitchPort
功能        ：MPU2查询是否有子卡
算法实现    ：
引用全局变量：
输入参数说明：无
==================================*/
s32 BrdMpu2HasEcard(void);

/*================================
函数名      : MspLpc2368Update
功能        ：h600 Msp Lpc2368 update
算法实现    ：
引用全局变量：
输入参数说明：无
==================================*/
STATUS MspLpc2368Update(s8 *pbyFileName);

/*===================================================================
函数名      : SwGetEthNego
功能        ：获取交换芯片的网络状态
输入参数说明：sw_eth_info 为传入的SWEthInfo结构体指针
返回值说明  ：成功返回OK，失败返回ERROR
====================================================================*/
STATUS SwGetEthNego(struct SWEthInfo *sw_eth_info );

/*===================================================================
函数名      : DaughterBoardClockConfig
功能        ：配置子板时钟,只供 生产测试阶段调用，目前只支持APU2 KDV8000I
输入参数说明：无
返回值说明  ：成功返回OK，失败返回ERROR
====================================================================*/
STATUS DaughterBoardClockConfig(void);

/*====================================================================
函数名      : BcsrLogicIn
功能        ：获取逻辑口状态
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：byPort,对应的逻辑口(0-7)
返回值说明：对应的逻辑口状态(0/1),0xff则返回失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
     12/11/21    1.0         王龙波          创建
====================================================================*/
u8 BcsrLogicIn(u8 byPort);

/*====================================================================
函数名      : BcsrLogicOut
功能        ：设定逻辑口的状态
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：byPort,对应的逻辑口(0-7);byState,对应的逻辑口状态(0/1)
返回值说明： 0为正常，-1为异常
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    12/11/21    1.0         王龙波          创建
====================================================================*/
STATUS BcsrLogicOut(u8 byPort, u8 byState);

/*================================
函数名：BrdTestUsb
功能：将Nip Usb 测试函数
算法实现： 1.自动检索当前存在的usb设备。2.自动mount 所有usb设备的第一个分区，如失败将返回error
           3.mount成功后，将向指定目录中写文件并检测文件是否正确，并删除测试文件。如检查文件出错将删除文件并返回error
		   4.umount usb设备分区
引用全局变量：
输入参数说明：
返回值说明： 成功返回0，否则返回-1
==================================*/
STATUS BrdTestUsb(void);
/*====================================================================
函数名      : MicIndicate
功能        ：mic指示灯状态设定函数
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：byPort,对应的mic编号，(0-7);byState,对应的指示灯状态(0,熄灭；/1，点亮)
返回值说明： 0为正常，-1为异常
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    12/11/21    1.0         王龙波          创建
====================================================================*/
STATUS MicIndicate(u8 byPort, u8 byState);


/*====================================================================
 函数名      : BrdGetAllInfo
 功能        ：obtain board information                                                                                
 算法实现    ：（可选项）    
 输入参数说明：ptBrdAllInfo是板子包含硬件，网络，系统信息指针，获取板子所有的信息
 返回值说明  ：错误返回ERROR；成功返回OK                                                                               
 ----------------------------------------------------------------------                                                 
 修改记录    ：日  期      版本        修改人        修改内容
               05/27/13    1.0          张专          创建                                                
 ====================================================================*/                                                 
STATUS BrdGetAllInfo(TBrdAllInfo *ptBrdAllInfo);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BRD_WRAPPER_H */

