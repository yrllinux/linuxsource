/*****************************************************************************
模块名      : all board
文件名      : brdDrvLib.h
相关文件    : 
文件实现功能:  所有硬件os使用的所有宏定义和部分公共函数的声明.
作者        : 张方明
版本        : V1.0  Copyright(C) 2003-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2003/8/19   01a         张方明      创建	
******************************************************************************/
#ifndef __INCBrdDrvLibh
#define __INCBrdDrvLibh


#ifdef __cplusplus 
extern "C" { 
#endif /* __cplusplus */

#ifndef WIN32 
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
*/
#include "time.h"
#include "timers.h"
#endif

/* +++++++++++++++++++++++++++++ 所有设备公共部分 ++++++++++++++++++++++++++++++++++*/
/* ============================= 公共宏定义 =====================================*/
/* OS版本信息 */
#define VER_MCUDRVLIB           ( const char * )"MCUDrvLib 40.01.01.01.050226"
#define VER_CRIDRVLIB           ( const char * )"CRIDrvLib 40.01.01.01.050318"
#define VER_APUVxNIP            ( const char * )"APUVxNIP 40.01.03.02.060412"
#define VER_MCUVXNIP            ( const char * )"MCUVXNIP 40.01.05.05.060412"
#define VER_CRIVxNIP            ( const char * )"CRIVxNIP 40.01.04.06.060925"
#define VER_DRIVxNIP            ( const char * )"DRIVxNIP 40.01.04.04.060610"
#define VER_DSIVxNIP            ( const char * )"DSIVxNIP 40.01.03.03.060610"
#define VER_KDV8000BVxNIP       ( const char * )"KDV8000BVxNIP 40.01.02.02.060412"
#define VER_MMPVxNIP            ( const char * )"MMPVxNIP  40.01.04.03.060412"
#define VER_VASVxNIP            ( const char * )"VASVxNIP 40.01.02.02.060412"
#define VER_MTVxNIPforKDV8010A  ( const char * )"MTVxNIPforKDV8010A 40.01.11.06.070806"
#define VER_KDV8018VxNIP        ( const char * )"KDV8018VxNIP 36.01.00.00.050226"
#define VER_IMTVxNIP            ( const char * )"IMTVxNIP 40.01.02.02.060412"
#define VER_KDV8005VxNIP        ( const char * )"KDV8005VxNIP 40.01.02.02.060412"
#define VER_KDV8010CVxNIP       ( const char * )"KDV8010CVxNIP 40.01.08.03.070309"
#define VER_VRIVxNIP            ( const char * )"VRIVxNIP 40.01.02.02.060812"
#define VER_DRI16VxNIP       	( const char * )"DRI16VxNIP 40.01.02.02.060412"
#define VER_16E1VxNIP       	( const char * )"16E1VxNIP 40.01.05.04.060610"
#define VER_KDV8003VxNIP       	( const char * )"KDV8003VxNIP 40.01.03.02.060510"
#define VER_KDM2417VxNIP       	( const char * )"KDM2417VxNIP 40.01.08.03.060412"
#define VER_KDV8008VxNIP       	( const char * )"KDV8008VxNIP 40.01.04.01.060302"
#define VER_MDSCVxNIP       	( const char * )"MDSCVxNIP 40.01.01.01.050824"
#define VER_KDM2000VxNIP       	( const char * )"KDM2000VxNIP 40.01.05.04.070202"
#define VER_KDM2428VxNIP       	( const char * )"KDM2428VxNIP 40.01.05.05.070809"

/* 单板种类ID号宏定义 */  
#define DSL8000_BRD_MPC             0      /* MPC板 */
#define DSL8000_BRD_DTI             1      /* DTI板 */
#define DSL8000_BRD_DIC             2      /* DIC板 */
#define DSL8000_BRD_CRI             3      /* CRI板 */
#define DSL8000_BRD_VAS             4      /* VAS板 */
#define DSL8000_BRD_MMP             5      /* MMP板 */
#define DSL8000_BRD_DRI             6      /* DRI板 */
#define DSL8000_BRD_IMT             7      /* IMT板 */
#define DSL8000_BRD_APU             8      /* APU板 */
#define DSL8000_BRD_DSI             9      /* DSI板 */
#define DSL8000_BRD_VPU             10     /* VPU板 */
#define DSL8000_BRD_DEC5            11     /* DEC5板 */
#define DSL8000_BRD_VRI             12     /* VRI板 */
#define KDV8000B_MODULE             13     /* KDV8000B模块 */
#define  KDV8005_BOARD              14     /* KDV8005板 */
#define DSL8000_BRD_DRI16           15     /* DRI16板 */
#define DSL8000_BRD_MDSC          0x10     /* MDSC板 */
#define DSL8000_BRD_16E1          0x11     /* 16E1板 */
#define KDV8003_BOARD             0x12     /* KDV8003板 */

#define KDM2560_BOARD             0x31     /* KDM2560 */
#define KDV2400_BOARD               50     /* 2400终端 */
#define KDV2500_BOARD               51     /* 2500B终端 */
#define KDM2418_BOARD               52     /* 2418终端 */
#define KDM2518_BOARD               53     /* 2518终端 */
#define KDM2417_BOARD               54     /* 2417终端 */
#define KDM2428_BOARD               56     /* 2428终端 */

#define KDV8010_BOARD               100    /* KDV8010终端 */
#define KDV8018_BOARD               101    /* KDV8018终端 */
#define KDV8010A_BOARD              102    /* KDV8010A终端 */
#define KDV8010B_BOARD              103    /* KDV8010B终端 */
#define KDV8010C_BOARD              104    /* KDV8010C终端 */
#define KDV8010D_BOARD              105    /* KDV8010D终端 */
#define KDV8010E_BOARD              106    /* KDV8010E终端 */
#define KDV8000B_BOARD              107    /* KDV8000B终端 */
#define KDV8010A_10_BOARD           108    /* 第十版KDV8010A终端 */
#define KDV8008_BOARD               109    /* KDV8008终端 */
#define KDV8010A_PLUS_BOARD         110    /* 第十一版KDV8010A终端 */

/* BrdLedStatusSet函数中指示灯闪烁状态 */
#define BRD_LED_ON                  1      /* 亮 */
#define BRD_LED_OFF                 2      /* 灭 */
#define BRD_LED_QUICK               3      /* 快闪 */
#define BRD_LED_SLOW                4      /* 慢闪 */

/* BrdLedStatusSet 函数中指示灯控制ID */
#define LED_E1_ID_BASE               0xd0
#define LED_E1_ID(ix)                (LED_E1_ID_BASE+ix)      /* 模块上0号E1告警灯*/
#define LED_E1_ID0                   LED_E1_ID(0)/*0号E1的告警灯*/
#define LED_E1_ID1                   LED_E1_ID(1)/*1号E1的告警灯*/
#define LED_E1_ID2                   LED_E1_ID(2)/*2号E1的告警灯*/
#define LED_E1_ID3                   LED_E1_ID(3)/*3号E1的告警灯*/
#define LED_E1_ID4                   LED_E1_ID(4)/*4号E1的告警灯*/
#define LED_E1_ID5                   LED_E1_ID(5)/*5号E1的告警灯*/
#define LED_E1_ID6                   LED_E1_ID(6)/*6号E1的告警灯*/
#define LED_E1_ID7                   LED_E1_ID(7)/*7号E1的告警灯*/
#define LED_E1_ID8                   LED_E1_ID(8)/*8号E1的告警灯*/
#define LED_E1_ID9                   LED_E1_ID(9)/*9号E1的告警灯*/
#define LED_E1_ID10                  LED_E1_ID(10)/*10号E1的告警灯*/
#define LED_E1_ID11                  LED_E1_ID(11)/*11号E1的告警灯*/
#define LED_E1_ID12                  LED_E1_ID(12)/*12号E1的告警灯*/
#define LED_E1_ID13                  LED_E1_ID(13)/*13号E1的告警灯*/
#define LED_E1_ID14                  LED_E1_ID(14)/*14号E1的告警灯*/
#define LED_E1_ID15                  LED_E1_ID(15)/*15号E1的告警灯*/

#define LED_SYS_ALARM                0xe0      /* 系统告警灯，所有板子有效 */
#define LED_SYS_LINK                 0xe1      /* 系统连接灯，所有板子有效 */
#define LED_BOARD_LED_NORM           0xe2      /* 灯板上正常指示灯，仅对KDV8000机框灯板有效 */
#define LED_BOARD_LED_NALM           0xe3      /* 灯板上一般告警指示灯，仅对KDV8000机框灯板有效 */
#define LED_BOARD_LED_SALM           0xe4      /* 灯板上严重告警指示灯，仅对KDV8000机框灯板有效 */
#define LED_CDMA_LED      	         0xe5      /* KDM2417上CDMA灯 */
#define LED_WLAN_LED      	         0xe6      /* KDM2417上WLAN灯 */
#define LED_DISK_LED      	         0xe7      /* KDM2417上存储盘指示灯 */
#define LED_MPC_OUS                  0xe8      /* mpc板的ous离线灯，数据同步用，同步后点灭 */

#define LED_BSP15_ID_BASE            0xf0
#define LED_BSP15_ID(ix)             (LED_BSP15_ID_BASE+ix)      /* bsp15指示灯 */
#define LED_BSP15_ID0                LED_BSP15_ID(0)      /* 0号bsp15指示灯 */
#define LED_BSP15_ID1                LED_BSP15_ID(1)      /* 1号bsp15指示灯 */
#define LED_BSP15_ID2                LED_BSP15_ID(2)      /* 2号bsp15指示灯 */
#define LED_BSP15_ID3                LED_BSP15_ID(3)      /* 3号bsp15指示灯 */
#define LED_BSP15_ID4                LED_BSP15_ID(4)      /* 4号bsp15指示灯 */
#define LED_BSP15_ID5                LED_BSP15_ID(5)      /* 5号bsp15指示灯 */


/* BrdSetEthParam函数中byIpOrMac输入值的宏定义 */
#define Brd_SET_IP_AND_MASK         1      /* 设置IP地址和子网掩码 */
#define Brd_SET_MAC_ADDR            2      /* 设置MAC地址 */
#define Brd_SET_ETH_ALL_PARAM       3      /* 设置以太网的IP地址、子网掩码和MAC地址 */

/* ip地址配置状态 */
#define IP_SET_AND_UP	   1  /* address set and up */
#define IP_SET_AND_DOWN	   2  /* address set and down */
#define IP_NOT_SET	       3  /* address not set */

#define IP_ROUTE_MAX_NUM   64 /* 最大支持的路由个数 */
#define KDV8005_V35__MAX_NUM   1 /* 最大支持的V35接口数 */
#define VRI_V35_MAX_NUM   	   6 /* 最大支持的V35接口数 */

/* SysSetAutoRunFile函数中用户程序启动参数设置 */
#define LOCAL_UNCOMPRESS_FILE       0      /* 本地未压缩的文件，一般以.out结尾 */ 
#define LOCAL_COMPRESS_FILE         1      /* 本地压缩的文件，一般以.z结尾 */ 
#define REMOTE_UNCOMPRESS_FILE      2      /* 远端未压缩的文件，一般以.out结尾 */ 
#define REMOTE_COMPRESS_FILE        3      /* 远端压缩的文件，一般以.z结尾 */ 
#define LOCAL_ZIP_FILE              4      /* 本地zip压缩包中的文件，一般以.out结尾 */ 

/* BrdSetWatchdogMode函数中看门狗宏定义 */
#define WATCHDOG_USE_CLK            0x00   /* 时钟硬件喂狗 */
#define WATCHDOG_USE_SOFT           0x01   /* 软件喂狗 */
#define WATCHDOG_STOP               0x02   /* 停止喂狗 */

/* flash分区信息宏定义 */
#define DISK_MAX_NUMBER             8      /* 最大支持的存储设备数量 */

/* BrdSetFanState函数中风扇运行状态宏定义 */
#define BRD_FAN_RUN                  0      /* 风扇转动 */
#define BRD_FAN_STOP                 1      /* 风扇停转 */

/* BrdFanIsStopped函数中风扇ID限制宏定义 */
#define BRD_FAN_MAX_NUM              8      /* 风扇最大个数 */

/* BrdOpenSerial函数打开串口类型宏定义 */
#define BRD_SERIAL_RS232                 0
#define BRD_SERIAL_RS422                 1
#define BRD_SERIAL_RS485                 2
#define BRD_SERIAL_INFRARED              3



/* E1线路NIP封装接口相关宏定义 */
/*---------------------------------------------------------------------------*/
#define E1_SINGLE_LINK_CHAN_MAX_NUM 32 /* 最大支持的E1单链路serial同步接口个数 */
#define E1_MULTI_LINK_CHAN_MAX_NUM   8 /* 最大支持的E1多链路捆绑连接通道(即virtual虚拟模板接口)个数 */

/* dwProtocolType参数宏定义 */
#define INTERFACE_PROTOCOL_PPP       0 /* 接口协议PPP */
#define INTERFACE_PROTOCOL_HDLC      1 /* 接口协议HDLC */

/* dwAuthenticationType参数宏定义 */
#define MP_AUTHENTICATION_NONE      0  /* 不设置验证方法 */
#define MP_AUTHENTICATION_PAP       1  /* PPP链接的验证方法PAP */
#define MP_AUTHENTICATION_CHAP      2  /* PPP链接的验证方法CHAP */

/* dwFragMinPackageLen 参数范围宏定义 */
#define MP_FRAG_MIN_PACKAGE_LEN_MIN_VAL  20
#define MP_FRAG_MIN_PACKAGE_LEN_MAX_VAL  1500

#define MP_STRING_MAX_LEN            32  /* 该模块通用字符串长度 */

/* dwSerialId 参数范围宏定义 */
#define E1_SERIAL_ID_MIN_VAL             16
#define E1_SERIAL_ID_MAX_VAL             62

/* dwE1ChanGroupId 参数范围宏定义 */
#define E1_CHAN_GROUP_ID_MIN_VAL         0
#define E1_CHAN_GROUP_ID_MAX_VAL         7

/* dwFragMinPackageLen dwSerialId dwE1ChanGroupId 参数自动设置宏定义 */
#define E1_PARAM_AUTO_CONFIG            0xffffffff

/* dwEchoInterval dwEchoMaxRetry 参数最大值宏定义 */
#define E1_SERIAL_ECHO_MAX_VAL           256

/* 返回值及错误码定义 */
#define E1_RETURN_OK                    0                     /* 操作成功 */
#define E1_ERRCODE_BASE                 10                    /* 错误码基值 */
#define E1_ERR_UNKNOWN                  (E1_ERRCODE_BASE+0)   /* 未知错误 */
#define E1_ERR_PARAM_EXCEPTION          (E1_ERRCODE_BASE+1)   /* 参数异常 */
#define E1_ERR_SERIAL_ID_INVALID        (E1_ERRCODE_BASE+2)   /* SERIAL同步接口号无效 */
#define E1_ERR_E1_ID_INVALID            (E1_ERRCODE_BASE+3)   /* E1接口号无效 */
#define E1_ERR_E1_TSMASK_INVALID        (E1_ERRCODE_BASE+4)   /* E1时隙掩码无效 */
#define E1_ERR_E1_CHANGROUP_ID_INVALID  (E1_ERRCODE_BASE+5)   /* E1时隙分配所在的编组号无效 */
#define E1_ERR_SERIAL_PROTOCOL_TYPE_INVALID  (E1_ERRCODE_BASE+6) /* 接口协议封装类型无效 */
#define E1_ERR_SERIAL_ECHO_INTERVAL_INVALID  (E1_ERRCODE_BASE+7) /* 同步接口的echo请求时间间隔无效 */
#define E1_ERR_SERIAL_ECHO_MAXRETRY_INVALID  (E1_ERRCODE_BASE+8) /* 同步接口的echo最大重发次数无效 */
#define E1_ERR_STRING_LEN_OVERFLOW           (E1_ERRCODE_BASE+9) /* 字符串长度溢出 */
#define E1_ERR_NIP_OPT_ERR                   (E1_ERRCODE_BASE+10)/* nip接口调用失败 */
#define E1_ERR_CHAN_NOT_CONF                 (E1_ERRCODE_BASE+11)/* 指定的通道号未打开使用 */
#define E1_ERR_CHAN_CONFLICT                 (E1_ERRCODE_BASE+12)/* 通道号冲突，指定的通道号已被使用 */
#define E1_ERR_MP_AUTHEN_TYPE_INVALID        (E1_ERRCODE_BASE+13)/* PPP链接的验证方法类型无效 */
#define E1_ERR_MP_FRAG_MIN_PACKAGE_LEN_INVALID (E1_ERRCODE_BASE+14)/* 最小分片包长无效 */


/*---------------------------------------------------------------------------*/


#ifndef WIN32 
/* =============================== 公共数据结构 ================================*/
/* 单板身份结构定义 */
typedef struct{
    UINT8 byBrdID;                      /* 板子ID号 */
    UINT8 byBrdLayer;                   /* 板子所在层号 */
    UINT8 byBrdSlot;                    /* 板子所在槽位号 */
}TBrdPosition;

/* 以太网参数结构 */
typedef struct{
    UINT32 dwIpAdrs;
    UINT32 dwIpMask;
    UINT8  byMacAdrs[6];
}TBrdEthParam;


/* 路由参数结构 */
typedef struct{
    UINT32 dwDesIpNet;      /* 目的子网	*/
    UINT32 dwDesIpMask;     /* 掩码		*/
    UINT32 dwGwIpAdrs;      /* 网关ip地址 */
    UINT32 dwRoutePri;		/* 路由优先级，0为默认 */
}TBrdIpRouteParam;

typedef struct{
    UINT32 dwIpRouteNum;    /* 得到的实际路由个数 */
    TBrdIpRouteParam tBrdIpRouteParam[IP_ROUTE_MAX_NUM];
}TBrdAllIpRouteInfo;


/*--------------------E1线路NIP封装接口相关结构定义---------------------------*/
/* E1线路单个serial接口参数结构 */
typedef struct{
    UINT32 dwUsedFlag;                  /* 当前serial同步接口使用标志,置1使用，下面参数有效；0不使用，下面参数无效 */
    UINT32 dwSerialId;                  /* serial同步接口编号，e1链路从16开始，有效范围16~62,不可冲突 */
    UINT32 dwE1Id;                      /* 该serial同步接口对应的e1编号，可配置范围0~E1_MAXNUM-1, E1_MAXNUM取决于相应硬件支持的e1个数,如kdv8010终端 #define BRD_MT_E1_MAXNUM 4 */
    UINT32 dwE1TSMask;                  /* E1时隙分配说明,详见下 */
    UINT32 dwE1ChanGroupId;             /* E1时隙分配所在的编组号，可配置范围0~7,如果不拆分一个E1使用的话填0，如果拆分的话必须对每个时隙段进行依次编号 */
    UINT32 dwProtocolType;		/* 接口协议封装类型,如果是单E1连接可以指定PPP/HDLC，如果是多E1捆绑连接必须是PPP协议 */
    UINT32 dwEchoInterval;		/* 对应serial同步接口的echo请求时间间隔，秒为单位，有效范围0-256，填默认值2即可 */
    UINT32 dwEchoMaxRetry;		/* 对应serial同步接口的echo最大重发次数，有效范围0-256，填默认值2即可 */
}TBrdE1SerialInfo;
/********************************************************************************* 
* dwE1TSMask时隙分配补充说明：
*    时晰分配的掩码，由Ts31 Ts30...Ts0构成32位掩码，TSx为1时表示使用该时隙，为0时不使用
* Ts0作为同步信号，用户不可使用, 0x0、0x1都是非法的参数，有效范围：0x2~0xfffffffe；
* 例: 使用TS1~TS5 5个时隙时dwE1TSMask = 0x0000003e,注意:Ts0=0; 此时带宽= 64x5 = 320kbit
      使用TS1~TS3,TS30~31 5个时隙时dwE1TSMask = 0xc000000e,注意:Ts0=0; 此时带宽= 64x5 = 320kbit
**********************************************************************************/

/* 单E1链路连接通道参数结构 */
typedef struct{
    UINT32 dwUsedFlag;                  /* 当前E1单链路连接通道使用标志,置1使用，下面参数有效；0不使用，下面参数无效 */
    UINT32 dwIpAdrs;                    /* ip地址，网络序 */
    UINT32 dwIpMask;                    /* 掩码，网络序 */
    TBrdE1SerialInfo tE1SerialInfo;     /* 相应serial同步接口链路参数 */
}TBrdE1SingleLinkChanInfo;

/* 多E1链路捆绑连接通道参数结构 */
typedef struct{
    UINT32 dwUsedFlag;                      /* 当前E1多链路捆绑连接通道使用标志,置1使用，下面参数有效；0不使用，下面参数无效 */
    UINT32 dwIpAdrs;                        /* ip地址，网络序 */
    UINT32 dwIpMask;                        /* 掩码，网络序 */
    UINT32 dwAuthenticationType;            /* PPP链接的验证方法PAP/CHAP,默认填MP_AUTHENTICATION_PAP */
    UINT32 dwFragMinPackageLen;             /* 最小分片包长，字节为单位，范围20~1500，默认填20 */
    char   chSvrUsrName[MP_STRING_MAX_LEN]; /* 服务端用户名称，用来验证对端的数据 */
    char   chSvrUsrPwd[MP_STRING_MAX_LEN];  /* 服务端用户密码，用来验证对端的数据 */
    char   chSentUsrName[MP_STRING_MAX_LEN];/* 客户端用户名称，用来被对端验证 */
    char   chSentUsrPwd[MP_STRING_MAX_LEN]; /* 客户端用户密码，用来被对端验证 */
    TBrdE1SerialInfo tE1SerialInfo[E1_SINGLE_LINK_CHAN_MAX_NUM];/* 每个被捆绑的serial同步接口链路参数 */
}TBrdE1MultiLinkChanInfo;
/*---------------------------------------------------------------------------*/


/* flash分区信息结构定义 */
typedef struct{
    UINT32  dwDiskBaseAddr;       /* flash盘的基地址 */
    UINT32  dwDiskSize;           /* flash盘的容量，以Byte为单位 */
    char diskNameStr[32];         /* flash盘的名称 */
}TBrdDiskInfoDesc;

typedef struct{
    UINT8  byDiskNum;           /* 实际flash盘的个数 */
    TBrdDiskInfoDesc tBrdDiskInfo[DISK_MAX_NUMBER];
}TBrdAllDiskInfo;

/* 底层V35告警信息结构定义 */
typedef struct{
    BOOL bAlarmV35CDDown;     /* Carrier Detect, 载波检测失败 */
    BOOL bAlarmV35CTSDown;    /* Clear To Send, 清除发送失败 */
    BOOL bAlarmV35RTSDown;    /* Request To Send, 请求发送失败 */
    BOOL bAlarmV35DTRDown;    /* 数据终端未就绪 */
    BOOL bAlarmV35DSRDown;    /* 数据未准备好 */
}TBrdV35AlarmDesc;

/* 底层E1告警信息结构定义 */
typedef struct{
    BOOL bAlarmE1RUA1;     /* E1全1告警 */
    BOOL bAlarmE1RLOS;     /* E1失同步告警 */
    BOOL bAlarmE1RRA;      /* E1远端告警 */
    BOOL bAlarmE1RCL;      /* E1失载波告警 */
    BOOL bAlarmE1RCMF;     /* E1的CRC复帧错误告警 */
    BOOL bAlarmE1RMF;      /* E1随路复帧错误告警 */
}TBrdE1AlarmDesc;


/* MPC板指示灯结构 */
typedef struct{
    UINT8 byLedRUN;     /* 系统运行灯 */
    UINT8 byLedDSP;     /* DSP运行灯 */
    UINT8 byLedALM;     /* 系统告警灯 */
    UINT8 byLedMS;      /* 主备用指示灯 */
    UINT8 byLedOUS;     /* ous指示灯 */
    UINT8 byLedSYN;     /* 同步指示灯 */
    UINT8 byLedLNK0;    /* 以太网0连接灯 */
    UINT8 byLedLNK1;    /* 以太网1连接灯 */
    UINT8 byLedNMS;     /* nms指示灯 */
    UINT8 byLedNORM;    /* 灯板上正常指示灯 */
    UINT8 byLedNALM;    /* 灯板上一般告警指示灯 */
    UINT8 byLedSALM ;   /* 灯板上严重告警指示灯 */
}TBrdMPCLedStateDesc;


/* CRI指示灯结构 */
typedef struct{
    UINT8 byLedRUN;      /* 系统运行灯 */
    UINT8 byLedMRUN;     /* VOIP/DDN模块运行灯 */
    UINT8 byLedSPD0;     /* 网口0速率指示灯 */
    UINT8 byLedLNK0;     /* 网口0联机指示灯 */
    UINT8 byLedACT0;     /* 网口0数据收发有效指示灯 */
    UINT8 byLedSPD1;     /* 网口1速率指示灯 */
    UINT8 byLedLNK1;     /* 以太网1连接灯 */
    UINT8 byLedACT1;     /* 网口1数据收发有效指示灯 */
    UINT8 byLedWERR;     /* 广义网数据错误 */
    UINT8 byLedMLNK;     /* 与MPC联机指示灯 */
}TBrdCRILedStateDesc;

/* DRI指示灯结构 */
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

/* DRI16指示灯结构 *//*wjh:add*/
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
    UINT8 byLedE1_ALM8;
    UINT8 byLedE1_ALM9;
    UINT8 byLedE1_ALM10;
    UINT8 byLedE1_ALM11;
    UINT8 byLedE1_ALM12;
    UINT8 byLedE1_ALM13;
    UINT8 byLedE1_ALM14;
    UINT8 byLedE1_ALM15;
    UINT8 byLedMLINK;
    UINT8 byLedLNK0;
    UINT8 byLedLNK1;
}TBrdDRI16LedStateDesc;

/* DSI指示灯结构 */
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
}TBrdDSILedStateDesc;

/* IMT指示灯结构 */
typedef struct{
    UINT8 byLedRun; 
    UINT8 byLedDSP1;
    UINT8 byLedDSP2;
    UINT8 byLedDSP3;
    UINT8 byLedSYSALM;
    UINT8 byLedMLINK;
    UINT8 byLedLNK0;
    UINT8 byLedLNK1;
}TBrdIMTLedStateDesc;

/* KDV8000B模块DSC指示灯结构 */
typedef struct{
    UINT8 byLedRun; 
    UINT8 byLedLNK0;
    UINT8 byLedLNK1;
}TBrdDSCLedStateDesc;

/* MMP指示灯结构 */
typedef struct{
    UINT8 byLedRUN; 
    UINT8 byLedDSPRUN1;
    UINT8 byLedDSPRUN2;
    UINT8 byLedDSPRUN3;
    UINT8 byLedDSPRUN4;
    UINT8 byLedDSPRUN5;
    UINT8 byLedALM;
    UINT8 byLedLNK;
    UINT8 byLedETH0;
    UINT8 byLedETH1;
}TBrdMMPLedStateDesc;

/* VAS指示灯结构 */
typedef struct{
    UINT8 byLedRUN; 
    UINT8 byLedALM;
    UINT8 byLedMLNK;
    UINT8 byLedETHLNK;
}TBrdVASLedStateDesc;

/* 终端KDV8010前面板指示灯结构 */
typedef struct{
    UINT8 byLedPower;
    UINT8 byLedALM;
    UINT8 byLedRun; 
    UINT8 byLedLINK;
    UINT8 byLedDSP1;
    UINT8 byLedDSP2;
    UINT8 byLedDSP3;
    UINT8 byLedETH1;
    UINT8 byLedETH2;
    UINT8 byLedETH3;
    UINT8 byLedIR;
}TBrdMTLedStateDesc;


/* 终端KDV8018前面板指示灯状态数据结构 */
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

/* 终端KDV8010C前面板指示灯结构 */
typedef struct{
    UINT8 byLedPower;
    UINT8 byLedALM;
    UINT8 byLedRun; 
    UINT8 byLedLINK;
    UINT8 byLedDSP1;
    UINT8 byLedDSP2;
    UINT8 byLedETH1;
    UINT8 byLedIR;
}TBrdKDV8010CLedStateDesc;

/* 终端KDV8010A以及KDV8000B前面板指示灯结构 */
typedef struct{
    UINT8 byLedPower;
    UINT8 byLedALM;
    UINT8 byLedRun; 
    UINT8 byLedLINK;
    UINT8 byLedDSP1;
    UINT8 byLedDSP2;
    UINT8 byLedDSP3;
    UINT8 byLedETH1;
    UINT8 byLedIR;
}TBrdKDV8010LedStateDesc;

/* KDV8005前面板指示灯结构 */
typedef struct{
    UINT8 byLedRun; 
    UINT8 byLedLINK;
    UINT8 byLedETH0;
    UINT8 byLedETH1;
    UINT8 byLedV35;
}TBrdKDV8005LedStateDesc;

/* KDV8003前面板指示灯结构 */
typedef struct{
    UINT8 byLedRun; 
    UINT8 byLedLINK;
    UINT8 byLedETH0;
    UINT8 byLedE1Alm;
}TBrdKDV8003LedStateDesc;

/* KDM2417前面板指示灯结构 */
typedef struct{
    UINT8 byLedPower;
    UINT8 byLedALM;
    UINT8 byLedRun; 
    UINT8 byLedCDMA;
    UINT8 byLedWLAN;
    UINT8 byLedETH;
    UINT8 byLedDISK;
}TBrdKDM2417LedStateDesc;

/* KDV8008前面板指示灯结构 */
typedef struct{
    UINT8 byLedRun;
    UINT8 byLedALM;
    UINT8 byLedETH0;
    UINT8 byLedETH1;
    UINT8 byLedSW;
}TBrdKDV8008LedStateDesc;

/* KDM2000PLUS前面板指示灯结构 */
typedef struct{
    UINT8 byLedRun; 
    UINT8 byLedALM; 
    UINT8 byLedLINK;
    UINT8 byLedDSP;
    UINT8 byLedE1Alm;
    UINT8 byLedETH1;
    UINT8 byLedSPD1;
    UINT8 byLedETH2;
}TBrdKDM2000LedStateDesc;

/* KDM2428前面板指示灯结构 */
typedef struct{
    UINT8 byLedPower;
    UINT8 byLedALM;
    UINT8 byLedRun; 
    UINT8 byLedLNK;
    UINT8 byLedETH;
    UINT8 byLedDISK;
}TBrdKDM2428LedStateDesc;

/* 指示灯状态公共结构 */
#define BRD_LED_NUM_MAX     32      /* 最多灯的个数 */
typedef struct {
    UINT32 dwLedNum;                /* 实际指示灯的个数 */
    union {
        UINT8 byLedNo[BRD_LED_NUM_MAX];
        TBrdMPCLedStateDesc tBrdMPCLedState;
        TBrdCRILedStateDesc tBrdCRILedState;
        TBrdDRILedStateDesc tBrdDRILedState;
        TBrdDRI16LedStateDesc tBrdDRI16LedState;
        TBrdDSILedStateDesc tBrdDSILedState;
        TBrdIMTLedStateDesc tBrdIMTLedState;
        TBrdDSCLedStateDesc tBrdDSCLedStateDesc;
        TBrdMMPLedStateDesc tBrdMMPLedState;
        TBrdVASLedStateDesc tBrdVASLedState;
        TBrdMTLedStateDesc  tBrdMTLedState;
        TBrdSMTLedStateDesc tBrdSMTLedState;
        TBrdKDV8010CLedStateDesc tBrdKDV8010CLedState;
        TBrdKDV8010LedStateDesc tBrdKDV8010LedStateDesc;
        TBrdKDV8005LedStateDesc tBrdKDV8005LedState;
        TBrdKDV8003LedStateDesc tBrdKDV8003LedState;
        TBrdKDM2417LedStateDesc tBrdKDM2417LedState;
        TBrdKDV8008LedStateDesc tBrdKDV8008LedState;
        TBrdKDM2000LedStateDesc tBrdKDM2000LedState;
        TBrdKDM2428LedStateDesc tBrdKDM2428LedState;
    } nlunion;
} TBrdLedState;


/* 机框电源告警数据结构 */
typedef struct{
    BOOL bAlarmPowerDC48VLDown;     /* 左电源-48V异常 */
    BOOL bAlarmPowerDC48VRDown;     /* 右电源-48V异常 */
    BOOL bAlarmPowerDC5VLDown;      /* 左电源+5V异常 */
    BOOL bAlarmPowerDC5VRDown;      /* 右电源+5V异常 */
}TBrdMPCAlarmPowerDesc;

/* 机框电源风扇告警数据结构 */
typedef struct{
    BOOL bAlarmPowerFanLLDown;      /* 左电源左风扇停转 */
    BOOL bAlarmPowerFanLRDown;      /* 左电源右风扇停转 */
    BOOL bAlarmPowerFanRLDown;      /* 右电源左风扇停转 */
    BOOL bAlarmPowerFanRRDown;      /* 右电源右风扇停转 */
}TBrdMPCAlarmPowerFanDesc;

/* MPC单板告警数据结构定义 */
typedef struct{
    BOOL bAlarmNetClkLockFailed;     /*  锁相环没有锁住参考时钟 */
    TBrdMPCAlarmPowerDesc tBrdMPCAlarmPower; /* 电源告警 */
    TBrdMPCAlarmPowerFanDesc tBrdMPCAlarmPowerFan; /* 电源风扇告警 */
}TBrdMPCAlarmVeneer;

/* SDH告警结构定义 */
typedef struct{
    BOOL bAlarmLOS;         /* 信号丢失 */
    BOOL bAlarmLOF;         /* 帧丢失 */
    BOOL bAlarmOOF;         /* 帧失步 */
    BOOL bAlarmLOM;         /* 复帧丢失 */
    BOOL bAlarmAU_LOP;      /* 管理单元指针丢失 */
    BOOL bAlarmMS_RDI;      /* 复用段远端缺陷指示 */
    BOOL bAlarmMS_AIS;      /* 复用段报警指示 */
    BOOL bAlarmHP_RDI;      /* 高阶通道远端缺陷指示 */
    BOOL bAlarmHP_AIS;      /* 高阶通道报警指示 */
    BOOL bAlarmRS_TIM;      /* 再生段通道踪迹字节不匹配 */
    BOOL bAlarmHP_TIM;      /* 阶通道踪迹字节不匹配 */
    BOOL bAlarmHP_UNEQ;     /* 高阶通道信号标记字节未装载 */
    BOOL bAlarmHP_PLSM;     /* 高阶通道信号标记字节不匹配 */

    BOOL bAlarmTU_LOP;      /* 支路单元指针丢失 */
    BOOL bAlarmLP_RDI;      /* 低阶通道远端缺陷指示 */
    BOOL bAlarmLP_AIS;      /* 低阶通道报警指示 */
    BOOL bAlarmLP_TIM;      /* 低阶通道踪迹字节不匹配 */
    BOOL bAlarmLP_PLSM;     /* 低阶通道信号标记字节不匹配 */
}TBrdMPCAlarmSDH;


/* MPC告警数据结构定义 */
typedef struct{
    TBrdMPCAlarmVeneer tBrdMPCAlarmVeneer;
    TBrdMPCAlarmSDH    tBrdMPCAlarmSDH;
}TBrdMPCAlarmAll;

/* CRI告警数据结构 */
typedef struct{
    BOOL bAlarmModuleOffLine;     /* 模块离线,以后需要删除 */
}TBrdCRIAlarmVeneer;

typedef struct{
    BOOL bAlarmModuleOffLine;     /* 模块离线 */
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
    BOOL bAlarmDSP1FanStop;     /* 图像编码风扇停转 */
    BOOL bAlarmDSP2FanStop;     /* 图像解码风扇停转 */
    BOOL bAlarmDSP3FanStop;     /* 音频编解码风扇停转 */
}TBrdIMTAlarmAll;


/* KDV8010底层告警信息结构定义 */
typedef struct{
    BOOL bAlarmEth1LnkDown;     /* 以太网1无连接 */
    BOOL bAlarmEth2LnkDown;     /* 以太网2无连接 */
    BOOL bAlarmEth3LnkDown;     /* 以太网3无连接 */

    BOOL bAlarmDSP1FanStop;     /* 图像编码风扇停转 */
    BOOL bAlarmDSP2FanStop;     /* 图像解码风扇停转 */
    BOOL bAlarmDSP3FanStop;     /* 音频编解码风扇停转 */
    BOOL bAlarmSYS1FanStop;     /* 机箱风扇1停转 */
    BOOL bAlarmSYS2FanStop;     /* 机箱风扇2停转 */

    TBrdE1AlarmDesc tBrdE1AlarmDesc[4];     /* E1告警 */
    TBrdV35AlarmDesc tBrdV35AlarmDesc;    /* V.35告警 */
}TBrdMTAlarmAll;


/* KDV8018底层告警信息结构定义 */
typedef struct{
    TBrdE1AlarmDesc tBrdE1Alarm;     /* E1告警 */
}TBrdSMTAlarmAll;

/* KDM2400 2500底层告警信息结构定义 */
typedef struct{
    TBrdE1AlarmDesc tBrdE1Alarm;     /* E1告警 */
}TBrdKDM2000AlarmAll;


/* 告警状态公共结构 */
#define BRD_ALM_NUM_MAX     256  /* 最大告警单元个数 */
typedef struct {
    UINT32 dwAlmNum;             /* 实际告警单元个数 */
    union {
        BOOL bAlmNo[BRD_ALM_NUM_MAX];
        TBrdMPCAlarmAll tBrdMPCAlarmAll;
        TBrdCRIAlarmAll tBrdCRIAlarmAll;
        TBrdDRIAlarmAll tBrdDRIAlarmAll;
        TBrdDRI16AlarmAll tBrdDRI16AlarmAll;
        TBrdDSIAlarmAll tBrdDSIAlarmAll;
        TBrdIMTAlarmAll tBrdIMTAlarmAll;
        TBrdMTAlarmAll  tBrdMTAlarmAll;
        TBrdSMTAlarmAll tBrdSMTAlarmAll;
        TBrdKDM2000AlarmAll tBrdKDM2000AlarmAll;
    } nlunion;
} TBrdAlarmState;

/* 以太网发送忙状态信息 */
typedef struct{
    UINT8 byEthId;
    BOOL  bSndBusy;
}TBrdEthSndBusyParam;

typedef void ( *TEthSndBusyCallBack )(UINT32 dwUsrArg, TBrdEthSndBusyParam *ptParam);

typedef void ( *TIpConflictCallBack )(UINT8 *pbyMacAddr, UINT32 dwIpAdrs);

/* 485查询数据输入的参数结构 */
typedef struct {
    UINT32 dwDevType;           /* 485设备类型 */
    UINT32 dwRcvTimeOut;        /* 等待485设备回应的超时,0取默认值 */
    UINT8  byDesAddr;           /* 485设备目标地址 */
    UINT8  byCmd;               /* 操作命令 */
} TRS485InParam;


/* IRAY返回值结构 */
typedef struct{
    UINT8 byCtrllerType;    /* 遥控器类型 */
    UINT8 byKeyCode;        /* 键码 */
}TIRAYRtnDataDesc;

typedef struct {
    BOOL bDevLnked;         /* 485设备连接状态 */
    union {
        TIRAYRtnDataDesc tIRAYRtnData;
    } nlunion;
} TRS485RtnData;


/* =============================== 公共函数声明 ================================*/
STATUS BrdQueryPosition(TBrdPosition *pTBrdPosition);  /* 板身份(ID、层号、槽位)查询 */
UINT8  BrdGetBoardID(void);                            /* 设备身份ID查询 */
UINT8  BrdGetFuncID(void);                             /* 设备功能ID查询 */
UINT8  BrdQueryHWVersion(void);                        /* 硬件版本号查询 */
UINT8  BrdQueryFPGAVersion(void);                      /* FPGA版本号查询 */
void   BrdQueryOsVer(char *pVer, UINT32 dwBufLen, UINT32 *pVerLen);/* os版本号查询 */
STATUS BrdTimeGet( struct tm* pGettm );                /* 读取RTC时间 */
STATUS BrdTimeSet( struct tm* pSettm );                /* 设置RTC和系统时间 */ 
STATUS BrdLedStatusSet(UINT8 byLedID, UINT8 byState);  /* 面板点灯控制 */
STATUS BrdSetEthParam(UINT8 byEthId, UINT8 byIpOrMac, TBrdEthParam *ptBrdEthParam); /* 设置以太网参数 */
STATUS BrdGetEthParam(UINT8 byEthId, TBrdEthParam *ptBrdEthParam);                  /* 获取以太网参数 */
STATUS BrdDelEthParam(UINT8 byEthId);                  /* 删除指定的以太网接口 */
STATUS BrdSetDefGateway(UINT32 dwIpAdrs);              /* 设置默认网关ip */
UINT32 BrdGetDefGateway(void);                         /* 获取默认网关ip */
STATUS BrdDelDefGateway(void);                         /* 删除默认网关ip */
UINT32 BrdGetNextHopIpAddr(UINT32 dwDstIpAddr);        /* 获取通往指定ip的第一跳路由ip地址 */
STATUS SysRebootHookAdd(VOIDFUNCPTR rebootHook);       /* 复位注册函数 */
void   BrdHwReset(void);                               /* 系统硬件复位 */
void   SysRebootDisable(void);                         /* 禁止系统出现excption时自动复位 */
void   SysRebootEnable(void);                          /* 允许系统出现excption时自动复位 */
STATUS SysOpenWdGuard(UINT32 dwNoticeTimeout);         /* 打开系统守卫 */
STATUS SysCloseWdGuard(void);                          /* 关闭系统守卫 */
STATUS SysNoticeWdGuard(void);                         /* 激活一次系统守卫 */
BOOL   SysWdGuardIsOpened(void);                       /* 查询系统守卫是否打开 */

UINT32 BrdGetSdramMaxSize(void);                       /* 取板子内存最大值,以MByte为单位 */
void   BrdSetWatchdogMode(UINT8 byMode);               /* 设置看门狗工作模式 */
void   BrdFeedDog(void);                               /* 喂狗函数,1.6秒内至少调一次 */
STATUS BrdQueryLedState(TBrdLedState *ptBrdLedState);  /* 闪灯模式查询 */
STATUS BrdAlarmStateScan(TBrdAlarmState *ptBrdAlarmState);  /* 底层告警信息扫描 */
UINT32 BrdGetRouteWayBandwidth(void);                  /* 读取线路带宽值，以Kbps为单位 */
STATUS SysSetAutoRunFile( char *pFileName, UINT8 byFileType, char *pAutoRunFunc,
                          char *pFtpSvrIp, char *pFtpUsr, char *pFtpPassWord ); /* 新的用户程序启动参数设置 */

STATUS BrdGetAllDiskInfo(TBrdAllDiskInfo *pBrdAllDiskInfo); /* 获取flash分区信息 */
UINT32 BrdGetFullFileName(UINT8 byPutDiskId, char *pInFileName, char *pRtnFileName); /* 文件名绝对路径化 */
STATUS BrdWakeup(void);      /* 设备唤醒 */
STATUS BrdSleep(void);       /* 设备休眠 */
STATUS BrdRestoreFile(char *pSrcFile, char *pDesFile);/* 还原文件 */
STATUS BrdBackupFile(char *pSrcFile, char *pDesFile); /* 备份文件 */
STATUS BrdSetFanState(UINT8 byRunState);              /* 设置系统风扇运行状态 */
BOOL   BrdFanIsStopped(UINT8 byFanId);                /* 查询指定风扇是否停转 */

STATUS BrdSetV35Param(UINT32 dwDevId, UINT32 dwIpAdrs, UINT32 dwIpMask);    /* 设置v35参数 */
STATUS BrdGetV35Param(UINT32 dwDevId, UINT32 *pdwIpAdrs, UINT32 *pdwIpMask);/* 读取v35接口参数 */
STATUS BrdDelV35Param(UINT32 dwDevId);                           /* 删除v35接口 */
STATUS BrdAddOneIpRoute(TBrdIpRouteParam *ptBrdIpRouteParam);    /* 增加一条路由 */
STATUS BrdDelOneIpRoute(TBrdIpRouteParam *ptBrdIpRouteParam);    /* 删除一条路由 */
STATUS BrdGetAllIpRoute(TBrdAllIpRouteInfo *ptBrdAllIpRouteInfo);/* 读取所有路由信息 */
STATUS BrdSaveNipConfig(void);/* 保存nip的网络配置文件 */
STATUS BrdGetMemInfo(UINT32 *pdwByteFree, UINT32 *pdwByteAlloc);/* 查询当前内存使用信息 */
/*--------------------------------------------------------------------------*/

/*--------------------E1线路NIP封装接口相关结构定义---------------------------*/
STATUS BrdOpenE1SingleLinkChan(UINT32 dwChanID, TBrdE1SingleLinkChanInfo *ptChanParam);  /* 创建一个E1单链路连接通道，dwChanID范围0~E1_SINGLE_LINK_CHAN_MAX_NUM-1 */
STATUS BrdOpenE1MultiLinkChan(UINT32 dwChanID, TBrdE1MultiLinkChanInfo *ptChanParam);    /* 创建一个E1多链路捆绑连接通道，dwChanID范围0~E1_MULTI_LINK_CHAN_MAX_NUM-1 */
STATUS BrdCloseE1SingleLinkChan(UINT32 dwChanID); /* 关闭指定的E1单链路连接通道，dwChanID范围0~dwSingleLinkChanNum-1 */
STATUS BrdCloseE1MultiLinkChan(UINT32 dwChanID);  /* 关闭指定的E1多链路捆绑连接通道，dwChanID范围0~dwMultiLinkChanNum-1 */
STATUS BrdGetE1SingleLinkChanInfo(UINT32 dwChanID, TBrdE1SingleLinkChanInfo *ptChanInfo);/* 读取指定的E1单链路连接通道信息，dwChanID范围0~dwSingleLinkChanNum-1 */
STATUS BrdGetE1MultiLinkChanInfo(UINT32 dwChanID, TBrdE1MultiLinkChanInfo *ptChanInfo);  /* 读取指定的E1多链路捆绑连接通道信息，dwChanID范围0~dwSingleLinkChanNum-1 */
/*--------------------------------------------------------------------------*/

UINT32 BrdChkOneIpStatus(UINT32 dwIpAdrs);      /* 检查指定ip在配置中的状态 */
BOOL   BrdLineIsLoop(UINT32 *pdwIpAddr);        /* 检查当前E1线路自环状态 */
void   BrdEthSndBusyCallBackReg(UINT32 dwUsrArg, TEthSndBusyCallBack ptEthSndBusyCallBack);
BOOL   BrdEthSndUseTimer(void); /* 查询是否使用定时发送以太网数据 */
STATUS BrdSetEthBautRate(UINT8 byUnit, UINT32 dwBautRate, UINT32 dwSmoothRate);
STATUS BrdMoveEthBautRate(UINT8 byUnit, UINT32 dwBautRate);
STATUS BrdAddEthBautRate(UINT8 byUnit, UINT32 dwBautRate);
STATUS BrdSetEthSpeedLimit(UINT8 byUnit, UINT32 dwLstBautRate,UINT32 dwHstBautRate);
UINT8  BrdGetEthActLnkStat(UINT8 byEthID);

STATUS BrdGetAlarmInput(UINT8 byPort, UINT8* pbyState);/* 获取现场告警(KDM2400/2500专用) */
STATUS BrdSetAlarmOutput(UINT8 byPort, UINT8  byState);/* 输出现场告警(KDM2400/2500专用) */
INT32  BrdOpenSerial(UINT8 byPort);/* 提供给业务层3.0版本的打开Rs232 Rs422 Rs485串口  */ 
INT32  BrdCloseSerial(INT32 nFd);
INT32  BrdReadSerial(INT32 nFd, INT8 *pbyBuf, INT32 nMaxbytes);
INT32  BrdWriteSerial (INT32 nFd, INT8 *pbyBuf, INT32 nBytes);
INT32  BrdIoctlSerial (INT32 nFd, INT32 nFunction, INT32 nArg);
INT32  BrdRs485TransSnd(INT32 nFd, UINT8 *pbyMsg, UINT32 dwMsgLen);
INT32  BrdRs485QueryData(INT32 nFd, TRS485InParam *ptRS485InParam, TRS485RtnData *ptRS485RtnData);

BOOL BrdGetEthMacAddrFromRom(UINT8 byEthId, UINT8 *pbyMacAddr);/* 从ROM中获取指定以太网的MAC地址 */

STATUS BrdIpConflictCallBackReg (TIpConflictCallBack ptFunc); /* 注册ip地址冲突时回调函数 */
BOOL   BrdIpOnceConflicted (void);    /* 查询系统是否发生过ip地址冲突 */

INT32  BrdGetPowerVoltage(UINT32 dwSampleTimes); /* 获取当前电压值，以毫伏为单位 */
STATUS BrdShutoffPower(void);                   /* 关闭电源供应 */
UINT8  BrdGetSwitchSel(void);         /* 获取开关选择的状态, 目前用于kdv8008双流盒 */
UINT8  SysGetIdlePercent(void);       /* 查询当前cpu的空闲百分比 */
#endif  /* end #ifndef WIN32 */


/* ===========所有带BSP15芯片的单板公共的数据结构、宏定义以及函数接口=========== */
/* BSP15图像采集端口宏定义 */
#define BSP15_VCAPTURE_FROM_NONE   0       /* video capture from port NULL */
#define BSP15_VCAPTURE_FROM_PORTA  1       /*                    port B    */
#define BSP15_VCAPTURE_FROM_PORTB  2       /*                    port B    */
#define BSP15_VCAPTURE_FROM_PORTAB 3       /*                    port AB   */

/* BSP15图像采集芯片BrdGetCaptureChipType返回值宏定义 */
#define CAPTURE_FROM_NONE          0       /* video or audio capture from chip none */
#define CAPTURE_FROM_SAA7114       1       /*                    SAA7114    */
#define CAPTURE_FROM_SAA7113       2       /*                    SAA7113    */
#define CAPTURE_FROM_AL240         3       /*                    AL240    */

/* BSP15声音采集芯片BrdGetAudCapChipType返回值宏定义 */
#define CAPTURE_AUDIO_FROM_NONE    0       /* audio capture from chip none  */
#define CAPTURE_AUDIO_FROM_5331    1       /* audio capture from chip 5331  */
#define CAPTURE_AUDIO_FROM_AIC23B  2       /*                        AIC23B */
#define CAPTURE_AUDIO_FROM_CS5340  3       /*                        CS5340 */

/* SAA7114图像数据输出端口宏定义 */
#define SAA7114_OUTPORT_NONE       0       /* SAA7114 vedio output port NULL */
#define SAA7114_OUTPORT_X          1       /*                      port X    */
#define SAA7114_OUTPORT_I          2       /*                      port I    */
#define SAA7114_OUTPORT_XI         3       /*                      port XI   */

/* VGA输入输出配置的宏定义 */
#define BSP15_VGA_NONE             0       /* vga输入输出功能均没有 */
#define BSP15_VGA_IN               1       /* 只有vga输入功能 */
#define BSP15_VGA_OUT              2       /* 只有vga输出功能 */
#define BSP15_VGA_INOUT            3       /* vga输入输出功能都有 */

/* VGA输入模式宏定义 */
#define VGAMODE_SVGA_60HZ  0               /* 分辨率800x600  刷新率60Hz */
#define VGAMODE_SVGA_75HZ  1               /* 分辨率800x600  刷新率75Hz */
#define VGAMODE_SVGA_85HZ  2               /* 分辨率800x600  刷新率85Hz */
#define VGAMODE_VGA_60HZ   3               /* 分辨率640x480  刷新率60Hz */
#define VGAMODE_VGA_75HZ   4               /* 分辨率640x480  刷新率75Hz */
#define VGAMODE_VGA_85HZ   5               /* 分辨率640x480  刷新率85Hz */
#define VGAMODE_XGA_60HZ   6               /* 分辨率1024x768 刷新率60Hz */
#define VGAMODE_XGA_75HZ   7               /* 分辨率1024x768 刷新率75Hz */

/* 硬件第11版kdv8010a专用的gpio模式，BrdSetBsp15GPIOMode用 */
#define BSP_GPIO_MODE_1TO2  (0x00)  /* 编码BSP15的视频
                        输出口连接到解码BSP15的GPIO口，同时VGA编解码BSP15的视频
                        输出口连接到SAA7121的视频输入口,仅对第11版kdv8010a有效 */
#define BSP_GPIO_MODE_3TO2  (0x01)  /* VGA编解码BSP15的
                        视频输出口连接到解码BSP15的GPIO口 */

#ifndef WIN32
UINT8  BrdGetBSP15CapturePort(UINT8 byBSP15ID);  /* BSP15图像采集端口查询 */
UINT8  BrdGetSAA7114OutPort(UINT8 byBSP15ID);    /* SAA7114图像输出端口查询 */
UINT8  BrdGetCaptureChipType(UINT8 byBSP15ID);   /* BSP15图像采集芯片查询 */
UINT8  BrdGetAudCapChipType(UINT8 byBSP15ID);    /* BSP15音频采集芯片查询 */
UINT32 BrdBsp15GpdpIsUsed(void);                 /* 查询哪些BSP15的gpdp端口互相连接 */
void   BrdSetBsp15GPIOMode(UINT8 byMode);        /* GPIO总线切换控制信号,仅对第11版kdv8010a有效 */
UINT8  BrdGetBSP15Speed(UINT8 byDevId);          /* 取指定bsp15的内核和内存频率 */
UINT8  BrdGetBSP15SdramSize(UINT8 byDevId);      /* 取指定bsp15的内存大小 */   
UINT8  BrdFastLoadEqtFileIsEn(void);             /* 是否允许快速装载BSP15的可执行文件 */
UINT8  BrdBsp15LedFlashMode(void);               /* 提供给cci层设置BSP15的点灯的选择依据 */
UINT8  BrdGetBSP15VGAConf(UINT8 byDevId);        /* 获取指定bsp15的vga配置 */
void   BrdStopVGACap(void);                      /* 停止采集vga(8083)数据(控制fpga) */
void   BrdStartVGACap(UINT8 byMode);             /* 开始采集vga(8083)数据(控制fpga) */
void   BrdSetVGACapMode(UINT8 byMode);           /* 设置fpga中VGA采集的模式(配置fpga) */
void   BrdCloseVGA(void);                        /* 关闭并禁止vga(8083)采集芯片工作 */
void   BrdOpenVGA(void);                         /* 打开并允许vga(8083)采集芯片工作 */
void   BrdMapDevOpenPreInit(UINT8 byBSP15ID);    /* 在map启动时对相关硬件的初始化 */
void   BrdMapDevClosePreInit(UINT8 byBSP15ID);   /* 在map关闭时对相关硬件的初始化 */

#endif /* end #ifndef WIN32 */

/*================ 所有带E1的单板公共的数据结构、宏定义以及函数接口=======================*/
/* E1阻抗值 */
#define E1_75_OHM                   0      /* E1模块阻抗为75  ohm */
#define E1_120_OHM                  1      /* E1模块阻抗为120 ohm */
#define E1_ERR_OHM                  0xff   /* E1模块阻抗无法识别 */

/* 网同步时钟信号输出控制 */
#define E1_CLK_OUTPUT_DISABLE       0     /* 高阻禁止单板上参考时钟信号输出到MPC板上 */
#define E1_CLK_OUTPUT_ENABLE        1     /* 允许单板板上参考时钟信号输出到MPC板上 */

/* 继电器工作模式设置 */
#define BRD_RELAY_MODE_NORMAL        0     /* 继电器连通 */
#define BRD_RELAY_MODE_LOCLOOP       1     /* 继电器自环 */
#define BRD_RELAY_MODE_OUTLOOP       2     /* 继电器外环 */

/* 各单板最大E1个数 */
#define BRD_E1_MAXNUM                16    /* 一个设备最多有16个E1物理接口 */
#define BRD_DRI_E1_MAXNUM            8     /* DRI共有8条E1链路 */
#define BRD_DSI_E1_MAXNUM            4     /* DSI共有4条E1链路 */
#define BRD_MT_E1_MAXNUM             4     /* KDV8010共有4条E1链路 */
#define BRD_DRI16_E1_MAXNUM          16    /* DRI16共有16条E1链路 */
#define BRD_16E1_E1_MAXNUM           16    /* 16E1共有16条E1链路 */
#define BRD_KDV8003_E1_MAXNUM        1     /* kdv8003共有1条E1链路 */
#define BRD_KDM2000_E1_MAXNUM        1     /* kdm2000最多有1条E1链路 */

#ifndef WIN32
UINT8  BrdGetE1MaxNum(void);                              /* 查询设备最大可配置的e1个数 */
STATUS BrdSetE1SyncClkOutputState (UINT8 mode);           /* 设置E1线路时钟是否输出给MPC板进行同步 */
STATUS BrdSetE1RelayLoopMode(UINT8 byE1ID, UINT8 mode);   /* E1链路设置继电器环回（内环、外环） */
STATUS BrdSelectE1NetSyncClk(UINT8 byE1ID);               /* 网同步参考时钟输出选择 */
UINT8  BrdQueryE1Imp(UINT8 byE1ID);                       /* 获取e1阻抗值 */
UINT8  BrdGetE1AlmState(UINT8 byE1Id);                    /* 获取指定E1线路的告警状态，返回0为正常，非0有告警 */
#endif /* end #ifndef WIN32 */


/*============ 所有带音视频矩阵的单板公共的数据结构、宏定义以及函数接口============*/
#define  MATRIX_OUTPUT_DISABLE  0xfb        /* 禁止音视频矩阵某输出端口 */
#define  MATRIX_OUTPUT_ENABLE   0xfc        /* 启用音视频矩阵某输出端口 */

#ifndef WIN32
void  BrdVideoMatrixSet(UINT8 vedioInSelect, UINT8 vedioOutSelect); /* 设置视频矩阵 */
void  BrdAudioMatrixSet(UINT8 audioInSelect, UINT8 audioOutSelect);  /* 设置音频矩阵 */
#endif /* end #ifndef WIN32 */


/* ============================ MPC板相关的宏定义 ============================ */
/* 对端主处理机板是否在位宏定义 */
#define BRD_MPC_IN_POSITION         0      /* 对端主处理机板在位  */
#define BRD_MPC_OUTOF_POSITION      1      /* 对端主处理机板不在位 */

/* 当前主处理机板主备用状态 */
#define BRD_MPC_RUN_MASTER          0      /* 当前主处理机板主用 */
#define BRD_MPC_RUN_SLAVE           1      /* 当前主处理机板备用  */

/* 光模块种类 */
#define BRD_MPC_SDHMODULE_NONE      0x07   /* 当前主处理机板没有插模块 */
#define BRD_MPC_SDHMODULE_SOI1      0x00   /* 当前主处理机板插模块SOI1 */
#define BRD_MPC_SDHMODULE_SOI4      0x01   /* 当前主处理机板插模块SOI4  */

/* 锁相环工作模式 */
#define SYNC_MODE_FREERUN           0      /* 自由振荡，对于主MCU应设置为该模式，所有下级线路时钟与该MCU同步 */
#define SYNC_MODE_TRACK_SDH8K       1      /* 跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的8K时钟 */
#define SYNC_MODE_TRACK_SDH2M       2      /* 跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的2M时钟 */
#define SYNC_MODE_TRACK_DT2M        4      /* 跟踪模式，作为下级MCU应设置为该模式，跟踪DT来的2M时钟 */
#define SYNC_MODE_UNKNOWN           0xff   /* 未知或错误的模式 */

/* KDV8000机框扬声器开关宏定义 */
#define LED_BOARD_SPK_OFF           0x00   /* 关闭扬声器 */
#define LED_BOARD_SPK_ON            0x01   /* 开启扬声器 */

/* ============================ CRI板相关的宏定义 ============================ */
#define OS_CRI_FOR_MP               0
#define OS_CRI_FOR_GK               1

/* ============================ VAS板相关的宏定义 ============================ */
#define  VAS_MATRIX_16X16           0        /* 16X16的矩阵板 */
#define  VAS_MATRIX_08X08           8        /*   8X8的矩阵板 */

/* ============================ DRI板相关的宏定义 ============================ */

/* ============================ MMP板相关的宏定义 ============================ */

/*==============================IMT板相关的宏定义====================================*/

/* ============================ DSI板相关的宏定义 ============================ */

/* ============================ KDV8000B板相关的宏定义 ============================ */

/*==============================KDV8010板相关的宏定义====================================*/
/* 模块宏定义 */
#define KDV8010A_MODULE_E1          0        /* 外挂模块为: 单E1模块   */
#define KDV8010A_MODULE_4E1         1        /* 外挂模块为: 4E1模块    */
#define KDV8010A_MODULE_V35DTE      2        /* 外挂模块为: V35DTE模块 */
#define KDV8010A_MODULE_V35DCE      3        /* 外挂模块为: V35DCE模块 */
#define KDV8010A_MODULE_BRIDGE      4        /* 外挂模块为: 网桥模块   */
#define KDV8010A_MODULE_2E1         5        /* 外挂模块为: 2E1模块    */
#define KDV8010A_MODULE_KDV8000B    6        /* 外挂模块为: KDV8000B模块    */
#define KDV8010A_MODULE_DSC         0xc      /* 外挂模块为: DSC(同KDV8000B)模块 */
#define KDV8010A_NO_MODULE          0xf      /* 没有外挂模块  */

/* 485状态相关宏定义 */
#define RS485_SUCCESS                     0    /* 操作成功 */
#define RS485_NOT_OPENED                  1    /* RS485串口没有打开 */
#define RS485_NOT_CONNECTED               2    /* RS485设备没有在线 */
#define RS485_SND_TIMEOUT                 3    /* RS485发送操作超时 */
#define RS485_RCV_TIMEOUT                 4    /* RS485接收操作超时 */
#define RS485_RCV_LENERR                  5    /* RS485接收的数据长度错 */
#define RS485_RCV_ERRDATA                 6    /* RS485接收的错包 */
#define RS485_ERROR                      -1    /* 操作错误 */


#define RS485_SET_BAUDRATE      0   /* 设置485串口的波特率 */
#define RS485_GET_BAUDRATE      1   /* 查询485串口的波特率 */
#define RS485_SET_STOPBIT       2   /* 设置485串口的停止位 */
#define RS485_GET_STOPBIT       3   /* 查询485串口的停止位 */
#define RS485_SET_SND_TIMEOUT   4   /* 设置485串口操作的发送超时值 */
#define RS485_GET_SND_TIMEOUT   5   /* 查询485串口操作的发送超时值 */
#define RS485_SET_PARITY        6   /* 设置串口的奇偶校验位 */
#define RS485_GET_PARITY        7   /* 查询串口的奇偶校验位 */

#define RS485_PARITY_NONE       0   /* 串口无奇偶校验位 */
#define RS485_PARITY_ODD        1   /* 串口奇校验 */
#define RS485_PARITY_EVEN       2   /* 串口偶校验 */
#define RS485_PARITY_MARK       3   /* 串口标记校验 */
#define RS485_PARITY_SPACE      4   /* 串口空格校验 */

/*串口相关宏定义*/
#define SIO_SET_BAUDRATE      0x2000   /* 设置串口的波特率 */
#define SIO_GET_BAUDRATE      0x2001   /* 查询串口的波特率 */
#define SIO_SET_STOPBIT       0x2002   /* 设置串口的停止位 */
#define SIO_GET_STOPBIT       0x2003   /* 查询串口的停止位 */
#define SIO_SET_DATABIT       0x2004   /* 设置串口的数据位 */
#define SIO_GET_DATABIT       0x2005   /* 查询串口的数据位 */
#define SIO_SET_PARITY        0x2006   /* 设置串口的奇偶校验位 */
#define SIO_GET_PARITY        0x2007   /* 查询串口的奇偶校验位 */
#define SIO_485_SET_SND_TIMEOUT 0x2008   /* 设置485串口操作的发送超时值 */
#define SIO_485_GET_SND_TIMEOUT 0x2009   /* 查询485串口操作的发送超时值 */

#define SIO_PARITY_NONE       0   /* 串口无奇偶校验位 */
#define SIO_PARITY_ODD        1   /* 串口奇校验 */
#define SIO_PARITY_EVEN       2   /* 串口偶校验 */
#define SIO_PARITY_MARK       3   /* 串口标记校验 */
#define SIO_PARITY_SPACE      4   /* 串口空格校验 */

#define SIO_STOPBIT_1         1   /* 串口1位停止位 */
#define SIO_STOPBIT_1_5       15  /* 串口1.5位停止位 */
#define SIO_STOPBIT_2         2   /* 串口2位停止位 */

/* =485设备类型= */
#define RS485DEV_IRAY           0   /* IRAY设备 */

/* =红外遥控器类型= */
#define IRAY_CTRL_BLACK         0   /* 黑色旧版的遥控器 */
#define IRAY_CTRL_SILVERY       1   /* 银白色新版的遥控器 */
#define IRAY_CTRL_UNKNOWN    0xff   /* 未知的遥控器 */

#define IRAY_RCV_NOKEY       0xfe   /* iray没有收到遥控器信息 */

#define IRAY_ADDR0           0x02   /* IRAY设备地址1 */
#define IRAY_ADDR1           0x7f   /* IRAY设备地址2 */
#define HOST_485DEV_ADDR     0x01   /* 主设备地址 */

#define IRAY_CMD_QRY_LNKSTATE   0   /* 查询IRAY设备是否在线 */
#define IRAY_CMD_QRY_KEYCODE    1   /* 查询IRAY设备接收的键码 */

/*==============================KDV8008板相关的宏定义====================================*/
#define DISPLAY_REMOTE_VGA  1   /* VGA选择信号。"1"远端VGA */
#define DISPLAY_LOCAL_VGA   0   /* VGA选择信号。"0"近端VGA */

#ifndef WIN32


/* MPC板的函数声明 */
UINT8  BrdMPCQueryAnotherMPCState(void);               /* 对端主处理机板是否在位查询 */
UINT8  BrdMPCQueryLocalMSState(void);                  /* 当前主处理机板主备用状态查询 */
STATUS BrdMPCSetLocalMSState(UINT8 byState);           /* 设置当前主处理机板主备用状态 */
UINT8  BrdMPCQuerySDHType(void);                       /* 光模块种类查询 */
UINT8  BrdMPCQueryNetSyncMode(void);                   /* 锁相环工作模式查询 */
STATUS BrdMPCSetNetSyncMode(UINT8 mode);               /* 锁相环网同步模式选择设置 */
STATUS BrdMPCSetAllNetSyncMode(UINT8 mode, UINT32 dwSdhE1Id);/* 锁相环网同步模式选择设置,增加sdh部分 */
STATUS BrdMPCResetSDH(void);                           /* 光模块复位 */
STATUS BrdMPCReSetAnotherMPC(void);                    /* 复位对端主处理机 */
STATUS BrdMPCOppReSetDisable (void);                   /* 禁止对端主处理机复位本主处理机 */
STATUS BrdMPCOppReSetEnable (void);                    /* 允许对端主处理机复位本主处理机 */
STATUS BrdMPCLedBoardSpeakerSet(UINT8 state);          /* 设置KDV8000机框上的扬声器 */

/* CRI板的函数声明 */
UINT8  SysGetOsVer(void);            /* 获取OS版本类型，因为可能在同一块板子上运行不同的OS */

/* DRI板函数声明 */

/* DSI板函数声明 */

/* IMT板函数声明 */

/* VAS板函数声明 */
UINT8 BrdGetMatrixType(void);                                       /* 矩阵板类型查询 */

/* KDV8010板函数声明 */
UINT8  BrdExtModuleIdentify(void);       /* 外挂模块识别函数 */

INT32  Rs485Open(UINT8 byComPort);
STATUS Rs485Close(UINT8 byComPort);
STATUS Rs485ioctl(UINT8 byComPort, INT32 function, INT32 arg);
INT32  Rs485QueryData(UINT8 byComPort, TRS485InParam *ptRS485InParam, TRS485RtnData *ptRS485RtnData);
INT32  Rs485TransSnd(UINT8 byComPort, UINT8 *pbyMsg, UINT32 dwMsgLen);

BOOL   BrdMicAdjustIsSupport(void);/* 查询当前mic调节是否支持 */
STATUS BrdMicVolumeSet
(
    UINT8  byTapPosition   /* 音阶，范围：0-127,对音量为正逻辑 */
);/* 设置mic输入音量 */

UINT8  BrdMicVolumeGet(void);/* 查询当前mic输入音量 */

#endif /* #ifndef WIN32 */



/**********************************************************************************/
/* ==================== 为了兼容3.0以前版本暂且保留，以后需要删除 ====================*/
#define BRD_CRI_MODULE_OFF          0
#define BRD_CRI_MODULE_ON           1
#define BRD_CRI_MLNK_LED            0          /* CRI板mlink灯的编号 */

#define BRD_VAS_MLINK_LED           2          /* VAS板mlink灯的编号 */
#define VAS_MATRIX_OUTPUT_DISABLE  0xfb        /* 禁止音视频矩阵某输出端口 */
#define VAS_MATRIX_OUTPUT_ENABLE   0xfc        /* 启用音视频矩阵某输出端口 */

#define BRD_DRI_CLK_OUTPUT_DISABLE   0   /* 高阻禁止DRI板上时钟信号输出到MPC板上 */
#define BRD_DRI_CLK_OUTPUT_ENABLE    1   /* 允许DRI板上时钟信号输出到MPC板上 */

#define BRD_DRI_RELAY_LOOP_DISABLE   0  /* 继电器连通 */
#define BRD_DRI_RELAY_LOOP_ENABLE    1  /* 继电器自环 */

#define BRD_DRI_E1CHAIN_DISABLE      0  /* 禁用某条E1链路 */
#define BRD_DRI_E1CHAIN_ENABLE       1  /* 使用某条E1链路 */

#define BRD_MMP_MLINK_LED                    7   /* MMP板mlink灯的编号 */

#define BRD_DSI_CLK_OUTPUT_DISABLE   0   /* 高阻禁止DSI板上时钟信号输出到MPC板上 */
#define BRD_DSI_CLK_OUTPUT_ENABLE    1   /* 允许DSI板上时钟信号输出到MPC板上 */
#define BRD_DSI_RELAY_SWITCH_IN      1  /* 继电器将背板进来的指定E1切换到8260 */
#define BRD_DSI_RELAY_SWITCH_OUT     0  /* 继电器将背板进来的指定E1切换到背板相应的E1输出 */
#define BRD_DSI_MLINK_LED            9   /* dsi板mlink灯的编号 */

#define UNCOMPRESS_FILE             0      /* 本地未压缩的文件，一般以.out结尾,兼容老函数 */ 
#define COMPRESS_FILE               1      /* 本地压缩的文件，一般以.z结尾,兼容老函数 */ 

/* 网络设备参数netEquipUnit宏定义 */
#define NETEQUIP_IS_ETH1    1    /* 以太网1： 对应8265的FCC2 或 x86上的启动网卡 */
#define NETEQUIP_IS_ETH2    2    /* 以太网2： 对应8265的FCC1 或 x86上的网关网卡*/
#define NETEQUIP_IS_ETH3    3    /* 以太网3： 对应8265的SCC1 */
#define NETEQUIP_IS_IPOH    4    /* IPOH  ：  对应8265的MCC1 */

#define BOARD_VERSION         ((UINT8)0x00)
#define EPLD_VERSION          ((UINT8)0x01)

/* 视频源检测宏定义 */  
#define VIDEOIN_CHKERROR   -1   /* 视频编码芯片MAP[1]没有创建并正常运行 */
#define VIDEOIN_OFF         0   /* 被选进编码的视频源无信号 */
#define VIDEOIN_ON          1   /* 被选进编码的视频源有信号 */


#define IMT_DSP1FAN      0         /* 图像编码风扇 */
#define IMT_DSP2FAN      1         /* 图像解码风扇 */
#define IMT_DSP3FAN      2         /* 音频编解码风扇 */


/* 待机宏定义 */
#define KDV8010A_WAKEUP       0         /* 唤醒系统 */
#define KDV8010A_SLEEP        1         /* 休眠系统 */

/* 视频切换矩阵模块设置 */
#define KDV8010_MATRIX_OUTPUT_DISABLE      ((UINT8)0xfb)        /* 禁止音视频矩阵某输出端口 */
#define KDV8010_MATRIX_OUTPUT_ENABLE       ((UINT8)0xfc)        /* 启用音视频矩阵某输出端口 */


/* 为了兼容3.0以前版本暂且保留，以后需要删除 */
#define KDV8010A_E1_75   0              /* E1模块阻抗为75  ohm */
#define KDV8010A_E1_120  0x40           /* E1模块阻抗为120 ohm */

/*==============================网同步=============================*/  
/* 同步模式宏定义 */
#define SYNC_MODE_TRACK_2M_SLAVE  4   /* 跟踪模式，作为下级MCU应设置为该模式，跟踪DT来的从2M时钟 */

#define SYNC_CLOCK_UNLOCK                0          /* 跟踪模式时时钟未锁定 */
#define SYNC_CLOCK_LOCK                  1          /* 跟踪模式时时钟锁定成功 */

#ifndef WIN32

/* 底层E1告警信息结构定义 */
typedef struct{
    BOOL bAlarmE1RUA1;     /* E1全1告警 */
    BOOL bAlarmE1RLOS;     /* E1失同步告警 */
    BOOL bAlarmE1RRA;      /* E1远端告警 */
    BOOL bAlarmE1RCL;      /* E1失载波告警 */
}TBrdE1AlarmDescOld;

typedef struct{
    TBrdE1AlarmDesc tBrdE1AlarmDesc[8]; 
}TBrdAllE1Alarm;


/* 底层告警信息结构定义 */
typedef struct{
    BOOL bAlarmEth1LnkDown;     /* 以太网1无连接 */
    BOOL bAlarmEth2LnkDown;     /* 以太网2无连接 */
    BOOL bAlarmEth3LnkDown;     /* 以太网3无连接 */

    BOOL bAlarmDSP1FanStop;     /* 图像编码风扇停转 */
    BOOL bAlarmDSP2FanStop;     /* 图像解码风扇停转 */
    BOOL bAlarmDSP3FanStop;     /* 音频编解码风扇停转 */
    BOOL bAlarmSYS1FanStop;     /* 机箱风扇1停转 */
    BOOL bAlarmSYS2FanStop;     /* 机箱风扇2停转 */

    TBrdE1AlarmDescOld tBrdE1AlarmDesc[4];     /* E1告警 */

    TBrdV35AlarmDesc tBrdV35AlarmDesc;    /* V.35告警 */
}TBrdMTAlarmAllOld;


/* 底层告警信息结构定义 */
typedef struct{
    TBrdE1AlarmDescOld tBrdE1Alarm;     /* E1告警 */
}TBrdSMTAlarmAllOld;


STATUS BrdDrvLibInit(void);
STATUS BrdLedFlash(UINT8 byLedID, UINT8 byState);      /* 面板点灯控制 */
STATUS BrdMPCAlarmVeneerStateScan(TBrdMPCAlarmVeneer *pTBrdMPCAlarmVeneer); /* MPC单板告警函数声明 */
STATUS BrdMPCAlarmSDHStateScan(TBrdMPCAlarmSDH *pTBrdMPCAlarmSDH);          /* MPC SDH告警函数声明 */
STATUS BrdMPCQueryLedState(TBrdMPCLedStateDesc *ptBrdMPCLedState);  /* 闪灯模式查询 */
void   BrdMpcWatchdogMode(UINT8 data);
void   BrdMpcFeedDog(void);
STATUS BrdMPCLedBoardLedSet(UINT8 LedID, UINT8 state); /* 设置KDV8000机框上的指示灯 */
STATUS BrdCRIAlarmVeneerStateScan(TBrdCRIAlarmVeneer *pTBrdCRIAlarmVeneer); /* CRI 单板告警函数声明 */
STATUS BrdCRIQueryLedState(TBrdCRILedStateDesc *ptBrdCRILedState);  /* 闪灯模式查询 */
STATUS BrdDRISetE1ChainMode(UINT8 byE1ID, UINT8 mode);          /* E1链路使用开关设置 */
STATUS BrdDRISetRelayLoop(UINT8 byE1ID, UINT8 mode);            /* E1链路设置继电器环回（内环） */
STATUS BrdDRISelectNetSyncClk(UINT8 byE1ID);                    /* DRI上网同步参考时钟输出选择 */
STATUS BrdDRISetNetSyncMode(UINT8 mode);                        /* DRI上网同步时钟输出开关 */
STATUS BrdDRIQueryLedState(TBrdDRILedStateDesc *ptBrdDRILedState);  /* 闪灯模式查询 */
STATUS BrdDRIAlarmE1StateScan(TBrdAllE1Alarm *ptBrdAllE1Alarm); /* DRI链路告警函数声明 */
STATUS BrdDSISetRelayMode(UINT8 byE1ID, UINT8 mode);            /* E1链路设置继电器模式 */
STATUS BrdDSISelectNetSyncClk(UINT8 byE1ID);                    /* DSI上网同步参考时钟输出选择 */
STATUS BrdDSISetNetSyncMode(UINT8 mode);                        /* DSI上网同步时钟输出开关 */
STATUS BrdDSIAlarmE1StateScan(TBrdAllE1Alarm *ptBrdAllE1Alarm); /* DSI链路告警函数声明 */
STATUS BrdDSIQueryLedState(TBrdDSILedStateDesc *ptBrdDSILedState);  /* 闪灯模式查询 */
STATUS BrdIMTQueryLedState(TBrdIMTLedStateDesc *ptBrdIMTLedState);
STATUS BrdKDV8000BQueryLedState(TBrdDSCLedStateDesc *ptBrdKDV8000BLedState);  /* 闪灯模式查询 */
STATUS BrdMMPQueryLedState(TBrdMMPLedStateDesc *ptBrdMMPLedStateDesc);
STATUS BrdVASQueryLedState(TBrdVASLedStateDesc *ptBrdVASLedStateDesc);
STATUS BrdRunStatSet(UINT8 state);                               /* 设备休眠设置 */
STATUS BrdMTAlarmStateScan(TBrdMTAlarmAllOld *ptBrdMTAlarmAll);  /* 底层告警信息扫描 */
STATUS BrdMTQueryLedState(TBrdMTLedStateDesc *ptBrdMTLedState);   /* 闪灯状态获取 */
STATUS BrdVersionGet(UINT8 module, UINT8 *pVersion);            /* 底层硬件版本信息 */  
STATUS BrdSMTQueryLedState(TBrdSMTLedStateDesc *ptBrdSMTLedState);   /* 闪灯状态获取 */
STATUS BrdSMTAlarmStateScan(TBrdSMTAlarmAllOld *ptBrdSMTAlarmAll);
STATUS sysRebootHookAdd(VOIDFUNCPTR rebootHook);
STATUS SysSetUsrAppParam(char *pFileName, UINT8 byFileType, char *pAutoRunFunc);/* 用户程序启动参数设置 */


#endif  /* END #ifndef WIN32 */



#ifdef __cplusplus 
} 
#endif /* __cplusplus */

#endif   /*INCDriverInith*/
