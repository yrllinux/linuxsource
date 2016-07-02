#ifndef WINBRD_H
#define WINBRD_H

#include "osp.h"
#include "kdvtype.h"
#ifdef ERROR
#undef ERROR
#endif
#include "boardwrapperdef.h"
#include "kdvsys.h"

#define	FILENAME_CONFIG				(s8*)"brdcfgdebug.ini"

#define SECTION_BoardConfig			(s8*)"BoardConfig"
#define KEY_Layer					(s8*)"Layer"
#define KEY_Slot					(s8*)"Slot"
#define KEY_Type					(s8*)"Type"
#define KEY_BoardIpAddr				(s8*)"BoardIpAddr"

#define SECTION_IpConfig			(s8*)"IpConfig"
#define ENTRYPART_ETHERNET			(s8*)"Eth"
#define ENTRYPART_IPADDR			(s8*)"IpAddr"
#define ENTRYPART_IPNUM				(s8*)"IpNum"
#define	ENTRYPART_MAC				(s8*)"Mac"

#define SECTION_IpRoute				(s8*)"IpRoute"
#define KEY_IpRouteNum				(s8*)"IpRouteNum"
#define ENTRYPART_IpRoute			(s8*)"IpRoute"				

#define BOARDHWVER					(u8)9
#define BOARDFPGAVER				(u8)4
#define BSP15SPEED					(u8)393
#define OSVERSION					(s8*)"windows"

#define MCUTYPE_UNKOWN              (u8)0
#define MCUTYPE_MCU                 (u8)1
#define MCUTYPE_MINIMCU             (u8)2

#ifdef __cplusplus 
extern "C" 
#endif /* __cplusplus */

#include "kdvtype.h"
#include "boardwrapperdef.h"
#include "boarddhcp.h"
#include "boardsntp.h"
#include <time.h>

/*相关宏定义*/

#define BRD_KDV7810                 0x7b   /* KDV7810视频终端 */
#define BRD_KDV7910                 0x7c   /* KDV7910视频终端 */
#define BRD_KDV7820                 0x7d   /* KDV7820视频终端 */
#define BRD_KDV7920                 0x7e   /* KDV7920视频终端 */
#define BRD_HWID_KDV7820            0x0021 /* KDV7820视频终端 */


/*各个板子的版本号定义*/
#define VER_MTLinuxOsWrapperForKDV8010A  ( const s8 * )"WinBoardWrapperForKDV8010A 40.01.00.02.070413"
#define VER_LinuxOsWrapperForMDSC        ( const s8 * )"WinBoardWrapperForMDSC 40.01.00.01.070413"
#define VER_LinuxOsWrapperForHDSC        ( const s8 * )"WinBoardWrapperForHDSC 40.01.00.01.070413"   
#define VER_LinuxOsWrapperForKDV8010C    ( const s8 * )"WinBoardWrapperForKDV8010C 40.01.00.01.070413" 
#define VER_LinuxOsWrapperForKDV8010C1    ( const s8 * )"WinBoardWrapperForKDV8010C1 40.01.00.01.070413"     
#define VER_LinuxOsWrapperForCRI         ( const s8 * )"WinBoardWrapperForCRI 40.01.00.01.070413" 
#define VER_LinuxOsWrapperForMPC         ( const s8 * )"WinBoardWrapperForMPC 40.01.00.00.070413" 
#define VER_LinuxOsWrapperForDRI         ( const s8 * )"WinBoardWrapperForDRI 40.01.00.00.070413" 
#define VER_LinuxOsWrapperForAPU         ( const s8 * )"WinBoardWrapperForAPU 40.01.00.00.070413" 
#define VER_LinuxOsWrapperForKDM2000PLUS         ( const s8 * )"BWinoardWrapperForKDM2000PLUS 40.01.00.00.070413" 
#define VER_MTLinuxOsWrapperForKDV8000B  ( const s8 * )"WinBoardWrapperForKDV8000B 40.01.00.00.070413"
#define VER_MTLinuxOsWrapperForKDV8000C  ( const s8 * )"WinBoardWrapperForKDV8000C 40.01.00.00.070413"
#define VER_MTLinuxOsWrapperForMMP       ( const s8 * )"WinBoardWrapperForMMP 40.01.00.00.070413"
#define VER_MTLinuxOsWrapperForDSC       ( const s8 * )"WinBoardWrapperForDSC 40.01.00.00.070413"
#define VER_MTLinuxOsWrapperForKDV8005   ( const s8 * )"WinBoardWrapperForKDV8005 40.01.00.00.070413"
#define VER_MTLinuxOsWrapperForDSI       ( const s8 * )"WinBoardWrapperForDSI 40.01.00.00.070413"
#define VER_MTLinuxOsWrapperForKDV8003   ( const s8 * )"WinBoardWrapperForKDV8003 40.01.00.00.070413"
#define VER_MTLinuxOsWrapperForVAS   ( const s8 * )"WinBoardWrapperForVAS 40.01.00.00.070413"
#define VER_MTLinuxOsWrapperFor16E1   ( const s8 * )"WinBoardWrapperForDRI16 40.01.00.00.070413"
#define VER_MTLinuxOsWrapperForKDM2428   ( const s8 * )"WinBoardWrapperForKDM2428 40.01.00.00.070413"
#define VER_MTLinuxOsWrapperForVRI   ( const s8 * )"WinBoardWrapperForVRI 40.01.00.00.070413"


/*相关结构体定义*/

/*SysSetUsrAppParam()函数用到的结构体*/

/* 应用配置信息结构体*/
typedef struct{
    u8 abyAppName[APP_NAME_MAX_LEN+1];  /* 应用程序文件名 */
    u8 byAppType;                       /* 应用程序类型,如:LOCAL_UNCOMPRESS_FILE等 */
}TAppInf;

typedef struct{
    u32 dwAppNum; /* 应用程序的个数 */
    TAppInf tAppInf[APP_MAX_NUM];
}TAppLoadInf;


/*单板身份结构定义*/
/* 单板身份结构定义 */
typedef struct{
    u8 byBrdID;                      /* 板子ID号 */
    u8 byBrdLayer;                   /* 板子所在层号 */
    u8 byBrdSlot;                    /* 板子所在槽位号 */
}TBrdPosition;

/*以太网相关的结构定义*/
/* 以太网参数结构 */
typedef struct{
    u32 dwIpAdrs;/*网络字节序*/
    u32 dwIpMask; /*网络字节序*/
    u8  byMacAdrs[6];
}TBrdEthParam;

/* 以太网参数结构 */
typedef struct{
    u32 dwIpNum;/*有效的IP地址数*/
    TBrdEthParam atBrdEthParam[IP_ADDR_MAX_NUM];/*存放IP地址等信息的数组*/
}TBrdEthParamAll;

/* 单个路由参数结构 */
typedef struct{
    u32 dwDesIpNet;      /* 目的子网，网络字节序	*/
    u32 dwDesIpMask;     /* 掩码，网络字节序	*/
    u32 dwGwIpAdrs;      /* 网关ip地址，网络字节序*/
    u32 dwRoutePri;		/* 路由优先级(1-255)，默认为0*/
}TBrdIpRouteParam;

/*多个路由参数结构 */
typedef struct{
    u32 dwIpRouteNum;    /* 得到的实际路由个数 */
    TBrdIpRouteParam tBrdIpRouteParam[IP_ROUTE_MAX_NUM];
}TBrdAllIpRouteInfo;

/* 以太网平滑发送忙状态信息 */
typedef struct{
    u8 byEthId;
    BOOL32  bSndBusy;
}TBrdEthSndBusyParam;

/*以太网平滑发送忙回调函数类型定义*/
typedef void ( *TEthSndBusyCallBack )(u32 dwUsrArg, TBrdEthSndBusyParam *ptParam);

/*注册设备重启回调函数的回调函数定义---VxWorks上该函数类型是系统定义的，在Linux上为了兼容VxWorks而特地定义该函数类型*/
#ifdef _LINUX_
typedef void 		(*VOIDFUNCPTR) (void);	/* pfunction returning void */
#endif

/*IP地址冲突回调函数类型定义*/
typedef void (*TIpConflictCallBack)(u8 *pbyMacAddr, u32 dwIpAdrs);

/*Ping回调函数类型定义*/
/*参数说明：
    nResult：ping结果:0--成功；1 ---超时；2--出错
    nTtl：ping收到ICMP回包的ttl，该ttl值可以反应出该ping包经过的跳数
    nTripTime：ping收到ICMP回包的往返时间，单位为微妙（us）
    nUserID：ping包携带的用户数据，可用来区分不同的用户
    nErrType：收到ICMP回包的类型，由于类型较多，暂时没有定义出来，需要时可以查找对照
    nErrCode：收到ICMP回包的代码，由于类型较多，暂时没有定义出来，需要时可以查找对照
*/
typedef s32 (*TPingCallBack)(s32 nResult,s32 nTtl,s32 nTripTime,s32 nUserID,s32 nErrType,s32 nErrCode);


/*E1相关的结构定义*/
/* E1线路单个serial接口参数结构 */
typedef struct{
    u32 dwUsedFlag;                  /* 当前serial同步接口使用标志,置1使用，下面参数有效；0不使用，下面参数无效 */
    u32 dwSerialId;                  /* serial同步接口编号，e1链路从16开始，有效范围16~62,不可冲突 */
    u32 dwE1Id;                      /* 该serial同步接口对应的e1编号，可配置范围0~E1_MAXNUM-1, E1_MAXNUM取决于相应硬件支持的e1个数,如kdv8010终端 #define BRD_MT_E1_MAXNUM 4 */
    u32 dwE1TSMask;                  /* E1时隙分配说明,详见下 */
    u32 dwE1ChanGroupId;             /* E1时隙分配所在的编组号，可配置范围0~7,如果不拆分一个E1使用的话填0，如果拆分的话必须对每个时隙段进行依次编号 */
    u32 dwProtocolType;		/* 接口协议封装类型,如果是单E1连接可以指定PPP/HDLC，如果是多E1捆绑连接必须是PPP协议 */
    u32 dwEchoInterval;		/* 对应serial同步接口的echo请求时间间隔，秒为单位，有效范围0-256，填默认值2即可 */
    u32 dwEchoMaxRetry;		/* 对应serial同步接口的echo最大重发次数，有效范围0-256，填默认值2即可 */
}TBrdE1SerialInfo;
/******************************************************************************
* dwE1TSMask时隙分配补充说明：
*    时晰分配的掩码，由Ts31 Ts30...Ts0构成32位掩码，TSx为1时表示使用该时隙，为0时不使用
* Ts0作为同步信号，用户不可使用, 0x0、0x1都是非法的参数，有效范围：0x2~0xfffffffe；
* 例: 使用TS1~TS5 5个时隙时dwE1TSMask = 0x0000003e,注意:Ts0=0; 此时带宽= 64x5 = 320kbit
      使用TS1~TS3,TS30~31 5个时隙时dwE1TSMask = 0xc000000e,注意:Ts0=0; 此时带宽= 64x5 = 320kbit
******************************************************************************/

/* 单E1链路连接通道参数结构 */
typedef struct{
    u32 dwUsedFlag;                  /* 当前E1单链路连接通道使用标志,置1使用，下面参数有效；0不使用，下面参数无效 */
    u32 dwIpAdrs;                    /* ip地址，网络序 */
    u32 dwIpMask;                    /* 掩码，网络序 */
    TBrdE1SerialInfo tE1SerialInfo;     /* 相应serial同步接口链路参数 */
}TBrdE1SingleLinkChanInfo;

/* 多E1链路捆绑连接通道参数结构 */
typedef struct{
    u32 dwUsedFlag;                      /* 当前E1多链路捆绑连接通道使用标志,置1使用，下面参数有效；0不使用，下面参数无效 */
    u32 dwIpAdrs;                        /* ip地址，网络序 */
    u32 dwIpMask;                        /* 掩码，网络序 */
    u32 dwAuthenticationType;            /* PPP链接的验证方法PAP/CHAP,默认填MP_AUTHENTICATION_PAP */
    u32 dwFragMinPackageLen;             /* 最小分片包长，字节为单位，范围20~1500，默认填20 */
    s8  chSvrUsrName[MP_STRING_MAX_LEN]; /* 服务端用户名称，用来验证对端的数据 */
    s8  chSvrUsrPwd[MP_STRING_MAX_LEN];  /* 服务端用户密码，用来验证对端的数据 */
    s8  chSentUsrName[MP_STRING_MAX_LEN];/* 客户端用户名称，用来被对端验证 */
    s8  chSentUsrPwd[MP_STRING_MAX_LEN]; /* 客户端用户密码，用来被对端验证 */
    TBrdE1SerialInfo tE1SerialInfo[E1_SINGLE_LINK_CHAN_MAX_NUM];/* 每个被捆绑的serial同步接口链路参数 */
}TBrdE1MultiLinkChanInfo;

/* 创建一个通道需要传入的参数结构定义 */
typedef struct{
    u32  dwBufNum;           /* 消息队列中buf的总数 */
    u16  wBufLength;         /* 消息队列中每个buf的长度,范围如上面的宏定义，且必须是8的整数倍 */
    u32  dwTimeSlotMask;     /* 见下面的时晰参数使用说明*/
} TE1TransChanParam;

/********************************************************************************* 
* dwTimeSlotMask 时晰参数使用说明：
*    时晰分配的掩码，由Ts31 Ts30...Ts0构成32位掩码，TSx为1时表示使用该时隙，为0时不使用
* Ts0作为同步信号，用户不可使用，Ts16可作为数据或信令，在本模块中该时隙视用户需求通过设置
* 时隙掩码选择是否使用，时隙分配建议从低到高连续分配时隙，不要间隔，否则可能容易引起通信系
* 统不稳定。
* 例: 使用5个时隙时dwTimeSlotMask = 0x0000003e,此时带宽= 64x5 = 320kbit。 注意:Ts0=0; 
**********************************************************************************/


/* 用户传入的发送数据参数结构定义 */
typedef struct{
    u8* pbyMsg;                  /* 用户消息指针 */
    u32 dwMsgLen;                /* 用户消息长度 */
} TE1TransMsgParam;

/* 通道数据收发统计结构定义 */
typedef struct{
    u32 dwRxOnePacketLen;        /* 接收到的单个包的长度 */
    u32 dwRxPackets;             /* 从E1线路上收到的包数 */
    u32 dwRxErrPackets;          /* 收到错误的包数 */
    u32 dwRxDropPackets;         /* 传给上层用户过程中丢掉的包数 */
    u32 dwRxSucSndPackets;       /* 成功传给上层用户的包数 */
    u32 dwTxSndPackets;          /* 上层要发送的包数 */
    u32 dwTxSucSndPackets;       /* 上层成功发送掉的包数 */
    u32 dwTxSucSndPacketLen;     /* 上层成功发送掉的总包长，字节为单位 */
}TE1TransChanStat;

/* 回调函数类型定义 */
typedef void ( * TE1MsgCallBack )(u8 byE1ChanID, u32 dwState);

/*Flash相关的结构定义*/
/* flash分区信息结构定义 */
typedef struct{
    u32  dwDiskSize;        /* flash盘的容量，以Byte为单位 */
    s8   achDiskNameStr[32];         /* flash盘的名称 */
}TBrdDiskInfoDesc;

typedef struct{
    u8  byDiskNum;           /* 实际flash盘的个数 */
    TBrdDiskInfoDesc atBrdDiskInfo[DISK_MAX_NUMBER];
}TBrdAllDiskInfo;

/*底层告警相关的结构定义*/
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

/* KDV8018底层告警信息结构定义 */
typedef struct{
    TBrdE1AlarmDesc tBrdE1Alarm;     /* E1告警 */
}TBrdSMTAlarmAll;

/* KDM2400 2500底层告警信息结构定义 */
typedef struct{
    TBrdE1AlarmDesc tBrdE1Alarm;     /* E1告警 */
}TBrdKDM2000AlarmAll;


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
    } nlunion;
} TBrdAlarmState;

/*指示灯控制相关的结构定义*/
/* MPC板指示灯结构 */
typedef struct{
    u8 byLedRUN;     /* 系统运行灯 */
    u8 byLedDSP;     /* DSP运行灯 */
    u8 byLedALM;     /* 系统告警灯 */
    u8 byLedMS;      /* 主备用指示灯 */
    u8 byLedOUS;     /* ous指示灯 */
    u8 byLedSYN;     /* 同步指示灯 */
    u8 byLedLNK0;    /* 以太网0连接灯 */
    u8 byLedLNK1;    /* 以太网1连接灯 */
    u8 byLedNMS;     /* nms指示灯 */
    u8 byLedNORM;    /* 灯板上正常指示灯 */
    u8 byLedNALM;    /* 灯板上一般告警指示灯 */
    u8 byLedSALM ;   /* 灯板上严重告警指示灯 */
}TBrdMPCLedStateDesc;

/* CRI指示灯结构 */
typedef struct{
    u8 byLedRUN;      /* 系统运行灯 */
    u8 byLedMRUN;     /* VOIP/DDN模块运行灯 */
    u8 byLedSPD0;     /* 网口0速率指示灯 */
    u8 byLedLNK0;     /* 网口0联机指示灯 */
    u8 byLedACT0;     /* 网口0数据收发有效指示灯 */
    u8 byLedSPD1;     /* 网口1速率指示灯 */
    u8 byLedLNK1;     /* 以太网1连接灯 */
    u8 byLedACT1;     /* 网口1数据收发有效指示灯 */
    u8 byLedWERR;     /* 广义网数据错误 */
    u8 byLedMLNK;     /* 与MPC联机指示灯 */
}TBrdCRILedStateDesc;

/* DRI指示灯结构 */
typedef struct{
    u8 byLedRun; 
    u8 byLedE1_ALM0;
    u8 byLedE1_ALM1;
    u8 byLedE1_ALM2;
    u8 byLedE1_ALM3;
    u8 byLedE1_ALM4;
    u8 byLedE1_ALM5;
    u8 byLedE1_ALM6;
    u8 byLedE1_ALM7;
    u8 byLedMLINK;
    u8 byLedLNK0;
    u8 byLedLNK1;
}TBrdDRILedStateDesc;

/* DRI16指示灯结构 */
typedef struct{
    u8 byLedRun; 
    u8 byLedE1_ALM0;
    u8 byLedE1_ALM1;
    u8 byLedE1_ALM2;
    u8 byLedE1_ALM3;
    u8 byLedE1_ALM4;
    u8 byLedE1_ALM5;
    u8 byLedE1_ALM6;
    u8 byLedE1_ALM7;
    u8 byLedE1_ALM8;
    u8 byLedE1_ALM9;
    u8 byLedE1_ALM10;
    u8 byLedE1_ALM11;
    u8 byLedE1_ALM12;
    u8 byLedE1_ALM13;
    u8 byLedE1_ALM14;
    u8 byLedE1_ALM15;
    u8 byLedMLINK;
    u8 byLedLNK0;
    u8 byLedLNK1;
}TBrdDRI16LedStateDesc;

/* DSI指示灯结构 */
typedef struct{
    u8 byLedRun; 
    u8 byLedE1_ALM0;
    u8 byLedE1_ALM1;
    u8 byLedE1_ALM2;
    u8 byLedE1_ALM3;
    u8 byLedE1_ALM4;
    u8 byLedE1_ALM5;
    u8 byLedE1_ALM6;
    u8 byLedE1_ALM7;
    u8 byLedMLINK;
    u8 byLedLNK0;
    u8 byLedLNK1;
}TBrdDSILedStateDesc;

/* IMT指示灯结构 */
typedef struct{
    u8 byLedRun; 
    u8 byLedDSP1;
    u8 byLedDSP2;
    u8 byLedDSP3;
    u8 byLedSYSALM;
    u8 byLedMLINK;
    u8 byLedLNK0;
    u8 byLedLNK1;
}TBrdIMTLedStateDesc;

/* KDV8000B模块DSC指示灯结构 */
typedef struct{
    u8 byLedRun; 
    u8 byLedLNK0;
    u8 byLedLNK1;
    u8 byLedALM;
}TBrdDSCLedStateDesc;

/* MMP指示灯结构 */
typedef struct{
    u8 byLedRUN; 
    u8 byLedDSPRUN1;
    u8 byLedDSPRUN2;
    u8 byLedDSPRUN3;
    u8 byLedDSPRUN4;
    u8 byLedDSPRUN5;
    u8 byLedALM;
    u8 byLedLNK;
    u8 byLedETH0;
    u8 byLedETH1;
}TBrdMMPLedStateDesc;

/* VAS指示灯结构 */
typedef struct{
    u8 byLedRUN; 
    u8 byLedALM;
    u8 byLedMLNK;
    u8 byLedETHLNK;
}TBrdVASLedStateDesc;

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

/* 终端KDV8010C1前面板指示灯结构 */
typedef struct{
    u8 byLedPower;
    u8 byLedALM;
    u8 byLedRun; 
    u8 byLedLINK;
    u8 byLedDSP1;
    u8 byLedDSP2;
    u8 byLedETH1;
    u8 byLedIR;
}TBrdKDV8010C1LedStateDesc;

/* KDV8005前面板指示灯结构 */
typedef struct{
    u8 byLedRun; 
    u8 byLedLINK;
    u8 byLedETH0;
    u8 byLedETH1;
    u8 byLedV35;
}TBrdKDV8005LedStateDesc;

/* KDV8003前面板指示灯结构 */
typedef struct{
    u8 byLedRun; 
    u8 byLedLINK;
    u8 byLedETH0;
    u8 byLedE1Alm;
}TBrdKDV8003LedStateDesc;

/* KDM2417前面板指示灯结构 */
typedef struct{
    u8 byLedPower;
    u8 byLedALM;
    u8 byLedRun; 
    u8 byLedCDMA;
    u8 byLedWLAN;
    u8 byLedETH;
    u8 byLedDISK;
}TBrdKDM2417LedStateDesc;

/* KDV8008前面板指示灯结构 */
typedef struct{
    u8 byLedALM;
    u8 byLedRun;
    u8 byLedETH;
}TBrdKDV8008LedStateDesc;

/* KDM2000PLUS前面板指示灯结构 */
typedef struct{
    u8 byLedRun; 
    u8 byLedALM; 
    u8 byLedLINK;
    u8 byLedDSP;
    u8 byLedE1Alm;
    u8 byLedETH1;
    u8 byLedSPD1;
    u8 byLedETH2;
}TBrdKDM2000LedStateDesc;

/* KDM2428前面板指示灯结构 */
typedef struct{
    u8 byLedPower;
    u8 byLedALM;
    u8 byLedRun; 
    u8 byLedLNK;
    u8 byLedETH;
    u8 byLedDISK;
}TBrdKDM2428LedStateDesc;

/* MDSC前面板指示灯结构 */
typedef struct{
    u8 byLedRun; 
    u8 byLedALM; 
    u8 byLedETH1;
    u8 byLedETH2;
}TBrdMDSLedStateDesc;

/* HDSC前面板指示灯结构 */
typedef struct{
    u8 byLedRun; 
    u8 byLedLos; 
    u8 byLedETH1;
    u8 byLedETH2;
}TBrdHDSLedStateDesc;

/* 终端VRI前面板指示灯结构 */
typedef struct{
    u8 byLedRUN;     /* 系统运行灯 */
    u8 byLedV35ALM0;  /* E10告警指示灯 */
    u8 byLedV35ALM1;  /* E11告警指示灯 */
    u8 byLedV35ALM2;  /* E12告警指示灯 */
    u8 byLedV35ALM3;  /* E13告警指示灯 */
    u8 byLedV35ALM4;  /* E14告警指示灯 */
    u8 byLedV35ALM5;  /* E15告警指示灯 */
    u8 byLedMLINK;   /* 系统建链指示灯 */
    u8 byLedLNK0;    /* Eth0的连接指示灯 */
}TVRILedStateDesc;


/* 指示灯状态公共结构 */
typedef struct {
    u32 dwLedNum;                /* 实际指示灯的个数 */
    union {
        u8 byLedNo[BRD_LED_NUM_MAX];
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
        TBrdKDV8010CLedStateDesc tBrdKDV8010CLedState;
        TBrdKDV8010C1LedStateDesc tBrdKDV8010C1LedState;
        TBrdKDV8010LedStateDesc tBrdKDV8010LedStateDesc;
        TBrdKDV8005LedStateDesc tBrdKDV8005LedState;
        TBrdKDV8003LedStateDesc tBrdKDV8003LedState;
        TBrdKDM2417LedStateDesc tBrdKDM2417LedState;
        TBrdKDV8008LedStateDesc tBrdKDV8008LedState;
        TBrdKDM2000LedStateDesc tBrdKDM2000LedState;
        TBrdKDM2428LedStateDesc tBrdKDM2428LedStateDesc;
        TBrdMDSLedStateDesc     tBrdMDSLedStateDesc;
        TBrdHDSLedStateDesc     tBrdHDSLedStateDesc;
        TVRILedStateDesc        tVRILedStateDesc;
    } nlunion;
} TBrdLedState;

// 网口状态公共结构
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
// IS2.2 8313 网口状态
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

/*BrdPing接口传入参数结构体*/
typedef struct{
    s32 nPktSize;/*要ping的数据报的大小，以字节为单位*/
    s32 nTimeOut;/*ping的超时值，以秒为单位*/
    s32 nTtl;/*TTL值*/
}TPingOpt;

/*BrdGetDeviceInfo 接口传入参数结构体*/
typedef struct{
    s8  achCPUInfo[CPU_TYPE_MAX_LEN];/*CPU类型*/
    s8  achUBootInfo[UBOOT_VERSION_MAX_LEN];/*u-boot版本信息*/
    s8  achKernelInfo[KERNEL_VERSION_MAX_LEN];/*内核操作系统的版本信息*/
    u32 dwMemSize;/*物理内存大小，以字节为单位*/
    u32 dwFlashSize;/*FLash大小，以字节为单位*/
    u32 dwCpuFreq;/*CPU的主频*/
    u32 dwCpmFreq;/*CPM的主频*/
    u32 dwBusFreq;/*总线的主频*/
    u32 dwEthNum;/*以太网的个数*/
    u32 dwE1Num;/*设备当前插的E1的个数*/
    u32 dwV35Num;/*设备当前插的V35的个数*/    
}TBrdDeviceInfo;

/*BrdAddUser接口传入的用户类型*/
typedef enum UserType
{
  USER_TYPE_CONSOLE = 0,     /*Console用户*/
  USER_TYPE_CONSOLE_SUPER,     /*Console高级用户*/
  USER_TYPE_FTP,         /*FTP 用户*/
  USER_TYPE_TELNET,      /*TELNET用户*/
  USER_TYPE_TELNET_SUPER,      /*TELNET 高级用户*/
  USER_TYPE_PPP,         /*PPP链路用户*/
  USER_TYPE_HTTP,	  /*HTTP用户*/
  USER_TYPE_MAX,         /*用户类型的最大值*/
}TUserType;

/*RawFlash相关的结构体定义*/

/* 每个分区参数结构定义 */
typedef struct{
    u32 dwPartitionType;                 /* 分区类型 */
    u32 dwPartitionSize;                 /* 该分区容量,字节为单位 */
}TOnePartnParam;

/* 分区参数结构定义 */
typedef struct{
    BOOL32  bUseDefaultConf;                  /* 是否使用默认的配置分区,如果为TRUE，使用每个板子的默认配置来分区,在读分区信息时返回0，无效数据 */
    TOnePartnParam tOnePartitionParam[PARTITION_MAX_NUM];    /* 每个分区信息 */
    u32 dwRamdiskSize;                   /* RAMDISK 的大小,字节为单位 */
}TFlPartnParam;

#endif