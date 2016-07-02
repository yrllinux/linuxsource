/******************************************************************************
模块名	: nipwrapper
文件名	: nipwrapper.h
功能    : NIPWRAPPER库的头文件
版本	：1.1
******************************************************************************/

#ifndef _NIP_WRAPPER_H
#define _NIP_WRAPPER_H


/*****************************************************************
	Common header files
******************************************************************/

#include "kdvtype.h"
#include "nipwrapperdef.h"
#include "brdwrapper.h"
#include "boarddhcp.h"
#include "boardsntp.h"
#include "atadrvlib.h"
#include "boardwebs.h"

//#ifndef _DAVINCI_
#include "pppoemodule.h"
//#endif

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*相关宏定义*/

/*errno define*/

#define BRD_API_ARGV_ERROR 					101		//无效参数 
#define BRD_API_COMM_ERROR 					102		//通信失败 
#define BRD_API_IF_NOT_EXIST 				103		//接口不存在 
#define BRD_API_IF_ADDR_NOT_EXIST           104     //地址不存在 
#define BRD_API_IF_MALFORMED_ERROR 			105		//无效的地址格式
#define BRD_API_IF_HIGHEST_ADDR_ERROR		106		//最高字节不能为0 or 127
#define	BRD_API_IF_PREFIX_IS_ZERO 			107 	//MASK LEN 不能为0
#define BRD_API_IF_ADDR_IS_MULTI  			108 	//最高字节要小于244
#define BRD_API_IF_CONFILICT_WITH_LOOPBACK  109		//地址与回环地址冲突
#define BRD_API_IF_PREFIX_LEN_ERROR			110 	//MASK LEN 不能为32 or 31
#define BRD_API_IF_PREFIX_ERROR				111 	//MASK不能为网络地址和广播地址
#define BRD_API_IF_CONFLICT_WITH_OTHER		112 	//地址浩渌涌诘刂烦逋煌
#define BRD_API_IF_CONFLICG_ADDR			113		//地址和本接口冲突
#define	BRD_API_IF_NO_MASTER_ADDR 			114		//主地址不存在
#define BRD_API_IF_SET_ADRR_ERROR 			115		//设置ip地址失败
#define BRD_API_IF_ADDR_NOT_FOUND 			116		//找不到地址
#define BRD_API_IF_DELET_SECOND_ADDR_FIRST	116		//在删除主ip之前先删除第二ip
#define BRD_API_IF_DELET_ADDR_ERROR 		117		//删除地址失败
#define BRD_API_IF_NOT_CONF_ADDDR 			119		//不是配置的地址
#define BRD_API_IF_SECOND_REACH_MAX 		120		//第二ip达到最大
#define BRD_API_IF_ADDR_IS_EXIST 			121		//ip已经配置
#define NIP_API_ADD_CFG     1                                  // E1增加配置
#define NIP_API_DEL_CFG     2                                  // E1删除配置


/*add for username length*/
#define API_BRD_USERNAME_LEN 32

#define BRD_IF_UP				1
#define BRD_IF_DOWN				0

/*各个板子的版本号定义*/
#define VER_MTLinuxOsWrapperForKDV8010A  ( const s8 * )"NipWrapperForKDV8010A 40.01.00.02.061031"
#define VER_LinuxOsWrapperForMDSC        ( const s8 * )"NipWrapperForMDSC 40.01.00.01.061026"
#define VER_LinuxOsWrapperForHDSC        ( const s8 * )"NipWrapperForHDSC 40.01.00.01.061026"
#define VER_LinuxOsWrapperForKDV8010C    ( const s8 * )"NipWrapperForKDV8010C 40.01.00.01.061026"
#define VER_LinuxOsWrapperForKDV8010C1    ( const s8 * )"NipWrapperForKDV8010C1 40.01.00.01.061026"
#define VER_LinuxOsWrapperForCRI         ( const s8 * )"NipWrapperForCRI 40.01.00.01.061026"
#define VER_LinuxOsWrapperForMPC         ( const s8 * )"NipWrapperForMPC 40.01.00.00.061026"
#define VER_LinuxOsWrapperForDRI         ( const s8 * )"NipWrapperForDRI 40.01.00.00.061026"
#define VER_LinuxOsWrapperForAPU         ( const s8 * )"NipWrapperForAPU 40.01.00.00.061026"
#define VER_LinuxOsWrapperForKDM2000PLUS         ( const s8 * )"NipWrapperForKDM2000PLUS 40.01.00.00.061026"
#define VER_MTLinuxOsWrapperForKDV8000B  ( const s8 * )"NipWrapperForKDV8000B 40.01.00.00.061026"
#define VER_MTLinuxOsWrapperForMMP       ( const s8 * )"NipWrapperForMMP 40.01.00.00.061026"
#define VER_MTLinuxOsWrapperForDSC       ( const s8 * )"NipWrapperForDSC 40.01.00.00.061026"
#define VER_MTLinuxOsWrapperForKDV8005   ( const s8 * )"NipWrapperForKDV8005 40.01.00.00.061026"
#define VER_MTLinuxOsWrapperForDSI       ( const s8 * )"NipWrapperForDSI 40.01.00.00.061026"
#define VER_MTLinuxOsWrapperForKDV8003   ( const s8 * )"NipWrapperForKDV8003 40.01.00.00.061026"
#define VER_MTLinuxOsWrapperForVAS   ( const s8 * )"NipWrapperForVAS 40.01.00.00.061026"
#define VER_MTLinuxOsWrapperFor16E1   ( const s8 * )"NipWrapperForDRI16 40.01.00.00.061026"
#define VER_MTLinuxOsWrapperForKDM2428   ( const s8 * )"NipWrapperForKDM2428 40.01.00.00.061026"
#define VER_MTLinuxOsWrapperForVRI   ( const s8 * )"NipWrapperForVRI 40.01.00.00.061026"
#define VER_MTLinuxOsWrapperForKDM2402   ( const s8 * )"NipWrapperForKDM2402 40.01.00.00.070117"
#define VER_MTLinuxOsWrapperForKDV7620   ( const s8 * )"NipWrapperForKDV7620 40.01.00.00.090413"
#define VER_MTLinuxOsWrapperForKDV7630   ( const s8 * )"NipWrapperForKDV7630 40.01.00.00.090413"
#define VER_MTLinuxOsWrapperForKDV7810   ( const s8 * )"NipWrapperForKDV7810 40.01.00.00.090413"
#define VER_MTLinuxOsWrapperForKDV7910   ( const s8 * )"NipWrapperForKDV7910 40.01.00.00.090413"
#define VER_MTLinuxOsWrapperForKDV7820   ( const s8 * )"NipWrapperForKDV7820 40.01.00.00.090413"
#define VER_MTLinuxOsWrapperForKDV7920   ( const s8 * )"NipWrapperForKDV7920 40.01.00.00.090413"
#define VER_MTLinuxOsWrapperForH600_H      ( const s8 * )"NipWrapperForH600_H 40.01.00.00.111020"
#define VER_MTLinuxOsWrapperForH600_L       ( const s8 * )"NipWrapperForH600_L 40.01.00.00.11120"
#define VER_MTLinuxOsWrapperForH700           ( const s8 * )"NipWrapperForH700 40.01.00.00.111020"
#define VER_MTLinuxOsWrapperForH800           ( const s8 * )"NipWrapperForH800 40.01.00.00.111020"
#define VER_MTLinuxOsWrapperForH900           ( const s8 * )"NipWrapperForH900 40.01.00.00.111020"


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


/*以太网相关的结构定义*/
/* 以太网参数结构 */
typedef struct{
    u32 dwIpAdrs;/*网络字节序*/
    u32 dwIpMask; /*网络字节序*/
    u8  byMacAdrs[6];
}TBrdEthParam;

/* 以太网mac地址结构*/
typedef struct{
    u8  byMacAdrs[6];
}TBrdEthMac;

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

/* 单个E1路由参数结构 */
typedef struct{
    u32 dwDesIpNet;      /* 目的子网，网络字节序	*/
    u32 dwDesIpMask;     /* 掩码，网络字节序	*/
    u32 dwGwIpAdrs;      /* 网关ip地址，网络字节序*/
    u32 dwRoutePri;		/* 路由优先级(1-255)，默认为0*/
    char dwIfname[32];    /* 本地网络接口名 */
	u32 dwIfunit;         /* 本地网络接口号 */   
}TBrdE1RouteParam;

/*多个路由参数结构 */
typedef struct{
    u32 dwIpRouteNum;    /* 得到的实际路由个数 */
    TBrdIpRouteParam tBrdIpRouteParam[IP_ROUTE_MAX_NUM];
}TBrdAllIpRouteInfo;

/*多个E1路由参数结构 */
typedef struct{
    u32 dwIpRouteNum;    /* 得到的实际路由个数 */
    TBrdE1RouteParam tBrdE1RouteParam[IP_ROUTE_MAX_NUM];
}TBrdAllE1RouteInfo;

/* 以太网平滑发送忙状态信息 */
typedef struct{
    BOOL32  bSndBusy;
    u8 byEthId;
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

typedef struct 
{
	int ifStatus;	// 1 up 0 down
	char ifName[16]; //接口名
}TBrdIfStatusCB;
/*interface status callback definition*/
typedef int(*BrdIfStatusCallBack) (TBrdIfStatusCB  *pMsg);
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
    u32  dwTimeSlotMask;     /* 见下面的时晰参数使用说明*/
    u16  wBufLength;         /* 消息队列中每个buf的长度,范围如上面的宏定义，且必须是8的整数倍 */
}TE1TransChanParam;

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
}TE1TransMsgParam;

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
    TBrdDiskInfoDesc atBrdDiskInfo[DISK_MAX_NUMBER];
    u8  byDiskNum;           /* 实际flash盘的个数 */
}TBrdAllDiskInfo;

/*BrdPing接口传入参数结构体*/
typedef struct{
    s32 nPktSize;/*要ping的数据报的大小，以字节为单位*/
    s32 nTimeOut;/*ping的超时值，以秒为单位*/
    s32 nTtl;/*TTL值*/
}TPingOpt;


/*BrdGetDeviceInfo 接口传入参数结构体*/
typedef struct{
    u32 dwMemSize;/*物理内存大小，以字节为单位*/
    u32 dwFlashSize;/*FLash大小，以字节为单位*/
    u32 dwCpuFreq;/*CPU的主频*/
    u32 dwCpmFreq;/*CPM的主频*/
    u32 dwBusFreq;/*总线的主频*/
    u32 dwEthNum;/*以太网的个数*/
    u32 dwE1Num;/*设备当前插的E1的个数*/
    u32 dwV35Num;/*设备当前插的V35的个数*/
    s8  achCPUInfo[CPU_TYPE_MAX_LEN];/*CPU类型*/
    s8  achUBootInfo[UBOOT_VERSION_MAX_LEN];/*u-boot版本信息*/
    s8  achKernelInfo[KERNEL_VERSION_MAX_LEN];/*内核操作系统的版本信息*/
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
    BOOL32  bUseDefaultConf;                  /* 是否使用默认的配置分读分区信息时返回0，无效数据 */
    TOnePartnParam tOnePartitionParam[PARTITION_MAX_NUM];    /* 每个分区信息 */
    u32 dwRamdiskSize;                   /* RAMDISK 的大小,字节为单位 */
}TFlPartnParam;

/*存放用户名和密码的结构体*/
typedef struct
{
    s8 user_name[API_BRD_USERNAME_LEN + 1];
    s8 password[API_BRD_USERNAME_LEN + 1];
}TUserPass;

/*函数声明*/

/*公共函数接口定义*/

/*获取板子状态的接口*/

/*================================
函数名：BrdInit
功能：本模块的初始化函数
算法实现：（可选项）
引用全局变量：
输入参数说明：  无
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdInit(void);

/*================================
函数名：BrdEthPrintEnable
功能：设置是否以太网状态打印有效
算法实现：（可选项）
引用全局变量：
输入参数说明：
返回值说明： 无。
==================================*/
void BrdEthPrintEnable(BOOL32 bEnable);

/*================================
函数名：BrdQueryOsVer
功能：提供给业务层调用的os版本号查询
算法实现：（可选项）
引用全局变量：
输入参数说明：  pchVer： 获取版本的字符串指针
                dwBufLen：该字符串的长度
                pdwVerLen：获取版本的实际字符串长度，该长度返回之前必须和dwBufLen比较是否溢出，正常时才对pVer赋值！

返回值说明： 无。如果实际字符串长度大于dwBufLen，pVerLen赋值为0
==================================*/
void  BrdQueryNipVersion(s8 *pchVer, u32 dwBufLen);

/*================================
函数名：BrdQueryOsVer
功能：提供给业务层调用的os版本号查询
算法实现：（可选项）
引用全局变量：
输入参数说明：  pchVer： 获取版本的字符串指针
                dwBufLen：该字符串的长度
                pdwVerLen：获取版本的实际字符串长度，该长度返回之前必须和dwBufLen比较是否溢出，正常时才对pVer赋值！

返回值说明： 无。如果实际字符串长度大于dwBufLen，pVerLen赋值为0
==================================*/
void  BrdQueryOsVer(s8 *pchVer, u32 dwBufLen,u32 *pdwVerLen);

/*================================
函数名：BrdQueryNipState
功能：Nip状态查询
算法实现：（可选项）
引用全局变量：
输入参数说明： 无
返回值说明： 成功返回OK,失败返回ERROR
==================================*/
s32  BrdQueryNipState(void);

/*====================================================================
函数名      : BrdCopyFile
功能        ：拷贝文件
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  pchSrcFile：源文件，必须是全路经
                pchDesFile：目标文件，必须是全路经
返回值说明  ：成功返回OK ；失败返回ERROR
====================================================================*/
STATUS BrdCopyFile (s8 *pchSrcFile, s8 *pchDesFile);

/*====================================================================
函数名      : BrdBackupFile
功能        ：备份文件
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  pchSrcFile：源文件，必须是全路经
                pchDesFile：目标文件，必须是全路经
返回值说明  ：成功返回OK ；失败返回ERROR
====================================================================*/
STATUS BrdBackupFile (s8 *pchSrcFile, s8 *pchDesFile);

/*====================================================================
函数名      : BrdRestoreFile
功能        ：还原文件，如果目标文件存在且只读，将其强行覆盖
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  pchSrcFile：源文件，必须是全路经
                pchDesFile：目标文件，必须是全路经
返回值说明  ：成功返回OK ；失败返回ERROR
====================================================================*/
STATUS BrdRestoreFile(s8 *pchSrcFile, s8 *pchDesFile);

/*====================================================================
函数名      : BrdGetMemInfo
功能        ：查询当前内存使用信息
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  pdwByteFree: 存放当前空闲的内存大小数据的指针，字节为单位
                pdwByteAlloc: 存放当前已分配的内存大小数据的指针，字节为单位
返回值说明  ：成功返回OK ；失败返回ERROR
====================================================================*/
STATUS BrdGetMemInfo(u32 *pdwByteFree, u32 *pdwByteAlloc);

/*====================================================================
函数名      : SysGetIdlePercent
功能        ：查询当前cpu的空闲百分比
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  无
返回值说明  ：当前cpu的空闲百分比
====================================================================*/
u8 SysGetIdlePercent(void);


/*====================================================================
函数名      : BrdGetDeviceInfo
功能        ：查询当前设备的CPU类型、主频、u-boot和内核版本信息的接口
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  ptBrdDeviceInfo:指向TBrdDeviceInfo结构体的指针
返回值说明  ：OK/ERROR
====================================================================*/
STATUS BrdGetDeviceInfo (TBrdDeviceInfo* ptBrdDeviceInfo);

/*====================================================================
函数名      : BrdAddUser
功能        ：增加系统用户
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  pchUserName：要增加的登陆用户名
		        pchPassword：要增加的用户的登录密码
                tUserType：要增加的用户类型，见相关的结构体定义
返回值说明  ：OK/ERROR
====================================================================*/
STATUS BrdAddUser (s8* pchUserName,s8* pchPassword, TUserType tUserType);

/*====================================================================
函数名      : BrdDelUser
功能        ：删除系统用户
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  pchUserName：要删除的登陆用户名
		        pchPassword：要删除的用户的登录密码
                tUserType：要删除的用户类型，见相关的结构体定义
返回值说明  ：OK/ERROR
====================================================================*/
STATUS BrdDelUser (s8* pchUserName,s8* pchPassword, TUserType tUserType);

/*====================================================================
函数名      : BrdGetUser
功能        ：获取用户名和密码
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  tUserPass:存放用户名密码的结构体数组
                dwArrayLen: 数组长度
                tUserType：用户类型，见相关的结构体定义
返回值说明  ：OK/ERROR
====================================================================*/
STATUS BrdGetUser(TUserPass tUserPass[], s32 dwArrayLen, TUserType tUserType);

/*====================================================================
函数名      : BrdUpdateAppFile
功能        ：将用户指定的单个文件更新到APP区(/usr/bin目录下)的接口
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  pchSrcFile：要更新的源文件名，包含绝对路径，不支持“./”等相对路径符号
		        pchDstFile：要更新的目的文件名，包含绝对路径，不支持“./”等相对路径符号
返回值说明  ：OK/ERROR
示例：BrdUpdateAppFile（”/ramdisk/mp8000b”,”/usr/bin/mp8000b”）
	将/ramdisk/目录下的mp8000b程序更新到只读的APP区(/usr/bin)并命名为mp8000b
====================================================================*/
STATUS BrdUpdateAppFile (s8* pchSrcFile,s8* pchDstFile);

/*====================================================================
函数名      : BrdUpdateAppImage
功能        ：将用户指定的使用mkfs.jffs2制作的包含所有上层业务程序的Image文件更新到APP区(/usr/bin目录下)的接口
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  pImageFile：要更新的Image文件名，包含绝对路径，不支持“./”等相对路径符号
返回值说明  ：OK/ERROR
示例：BrdUpdateAppImage（”/ramdisk/mdsc.jffs2”）
	将/ramdisk/目录下使用mkfs.jff2制作的的包含mdsc上所有的业务程序、配置文件的mdsc.jffs2镜像文件更新到只读的APP区(/usr/bin)
注意：调用完该函数后，设备必须重启新更新APP分区才能生效
====================================================================*/
STATUS BrdUpdateAppImage (s8* pImageFile);

#if 0 /*Linux已经不再提供该函数*/
/*====================================================================
函数名      : SysGetOsVer
功能        ：获取OS版本类型，因为可能在同一块板子上运行不同的OS，主要用于GK应用
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：无
返回值说明  ：宏定义：
		    #define OS_CRI_FOR_MP       0
            #define OS_CRI_FOR_GK       1
====================================================================*/
u8 SysGetOsVer(void);
#endif

/*================================
函数名：BrdGetEthParam
功能：读取以太网参数，含ip、mask、mac
算法实现：（可选项）
引用全局变量：
输入参数说明：  u8 byEthId --- 以太网ID号（0~x）;
                TBrdEthParam *ptBrdEthParam --- 存放以太网信息的结构指针。
返回值说明： 出错返回ERROR；成功返回OK。
==================================*/
STATUS BrdGetEthParam(u8 byEthId, TBrdEthParam *ptBrdEthParam);

/*================================
函数名：BrdGetEthMac
功能：读取以太网mac地址
算法实现：（可选项）
引用全局变量：
输入参数说明：  u8 byEthId --- 以太网ID号（0~x）;
                TBrdEthMac *ptBrdEthMac  --- 存放以太网信息的结构指针。
返回值说明： 出错返回ERROR；成功返回OK。
==================================*/
STATUS BrdGetEthMac(u8 byEthId, TBrdEthMac *ptBrdEthMac);

/*================================
函数名：BrdGetEthParam
功能：获取一个网口上所有的以太网参数，多个IP地址的情况
算法实现：（可选项）
引用全局变量：
输入参数说明：  u8 byEthId --- 以太网ID号（0~x）;
                BrdEthParamAll *ptBrdEthParamAll --- 存放以太网信息的结构指针。
返回值说明： 出错返回ERROR；成功返回OK。
==================================*/
STATUS BrdGetEthParamAll(u8 byEthId, TBrdEthParamAll *ptBrdEthParamAll);

/*================================
函数名：BrdGetEthParamSecIP
功能：获取一个网口上所有第二IP地址
算法实现：（可选项）
引用全局变量：
输入参数说明：  u8 byEthId --- 以太网ID号（0~x）;
                BrdEthParamAll *ptBrdEthParamAll --- 存放以太网信息的结构指针，此结构是
                用于返回第二IP地址的，返回的地址个数由dwIpNum指定，结构中的
                atBrdEthParam数组成员只由IP 和Mask有效, Mac 地址没有效
返回值说明： 出错返回ERROR；成功返回OK。
==================================*/
STATUS BrdGetEthParamSecIP(u8 byEthId, TBrdEthParamAll *ptBrdEthParamAll);

/*================================
函数名：BrdSetEthParam
功能：设置以太网参数
算法实现：（可选项）
引用全局变量：
输入参数说明：  byEthId：以太网的编号（0 ~ X）X视不同板子而定；
                byIpOrMac：为设置命令选择(见BoardWrapper.h中相关的宏定义),可以
                                     设置主IP,第二IP和Mac.
                                       Brd_SET_IP_AND_MASK         1      设置IP地址和子网掩码
                                       Brd_SET_MAC_ADDR            2       设置MAC地址
                                       Brd_SET_ETH_ALL_PARAM       3       设置以太网的IP地址、子网掩码和MAC地址
                                       Brd_SET_ETH_SEC_IP         4   设置以太网第二IP 地址
                ptBrdEthParam：指向存放设定值的TbrdEthParam数据结构的指针
返回值说明： 出错返回ERROR；成功返回OK。
==================================*/
STATUS BrdSetEthParam(u8 byEthId, u8 byIpOrMac, TBrdEthParam *ptBrdEthParam);

/*================================
函数名：BrdDelEthParamSecIP
功能：删除第二IP地址
算法实现：（可选项）
引用全局变量：
输入参数说明：  byEthId：以太网的编号（0 ~ X）X视不同板子而定；
                ptBrdEthParam：指向存放设定值的TbrdEthParam数据结构的指针
                                           结构中只有IP和 Mask有效.
返回值说明： 出错返回ERROR；成功返回OK。
==================================*/
STATUS BrdDelEthParamSecIP(u8 byEthId, TBrdEthParam *ptBrdEthParam);


/*================================
函数名：BrdDelEthParamIPAndMask
功能：删除all IP地址
算法实现：（可选项）
引用全局变量：
输入参数说明：  byEthId：以太网的编号（0 ~ X）X视不同板子而定；               
返回值说明： 出错返回ERROR；成功返回OK。
==================================*/
STATUS BrdDelEthParamIPAndMask(u8 byEthId);


/*================================
函数名：BrdDelEthParam
功能：删除指定的以太网接口
算法实现：（可选项）
引用全局变量：
输入参数说明：  byEthId：以太网的编号（0 ~ X）X视不同板子而定；
返回值说明： 出错返回ERROR；成功返回OK。
注意：在Linux下，一个网口可以配置多个IP地址，该接口会删除该接口上的所有IP地址。
==================================*/
STATUS BrdDelEthParam(u8 byEthId);

/*================================
函数名：BrdSetDefGateway
功能：设置默认网关ip
算法实现：（可选项）
引用全局变量：
输入参数说明：  dwIpAdrs：默认网关ip地址，长整型，网络序
返回值说明： 出错返回ERROR；成功返回OK。
==================================*/
STATUS BrdSetDefGateway(u32 dwIpAdrs);

/*================================
函数名：BrdGetDefGateway
功能：获取默认网关ip
算法实现：（可选项）
引用全局变量：
输入参数说明： 无
返回值说明： 错误返回0；成功返回长整型的ip地址，网络序
==================================*/
u32 BrdGetDefGateway(void);

/*================================
函数名：BrdDelDefGateway
功能：删除默认网关ip
算法实现：（可选项）
引用全局变量：
输入参数说明： 无
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdDelDefGateway(void);

/*================================
函数名：BrdGetNextHopIpAddr
功能：获取通往指定ip的第一跳路由ip地址
算法实现：（可选项）
引用全局变量：
输入参数说明：dwDstIpAddr：目的ip地址，长整型，网络序
		    dwDstMask：目的掩码，长整型，网络序
返回值说明： 错误返回0；成功返回第一跳路由ip地址，长整型
==================================*/
u32 BrdGetNextHopIpAddr(u32 dwDstIpAddr,u32 dwDstMask);

/*================================
函数名：BrdAddOneIpRoute
功能：增加一条路
算法实现：（可选项）
引用全局变量：
输入参数说明：ptBrdIpRouteParam：指向存放路由信息的TBrdIpRouteParam结构指针
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdAddOneIpRoute(TBrdIpRouteParam *ptBrdIpRouteParam);

/*================================
函数名：BrdDelOneIpRoute
功能：删除一条路
算法实现：（可选项）
引用全局变量：
输入参数说明：ptBrdIpRouteParam：指向存放路由信息的TBrdIpRouteParam结构指针
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdDelOneIpRoute(TBrdIpRouteParam *ptBrdIpRouteParam);

/*================================
函数名：BrdGetAllIpRoute
功能：读取所有路由信息
算法实现：（可选项）
引用全局变量：
输入参数说明：ptBrdAllIpRouteInfo：指向存放返回路由信息的TBrdAllIpRouteInfo结构指针
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdGetAllIpRoute(TBrdAllIpRouteInfo *ptBrdAllIpRouteInfo);

/*================================
函数名：BrdChkOneIpStatus
功能：检查指定ip在配置中的状态
算法实现：（可选项）
引用全局变量：
输入参数说明：dwIpAdrs：待检查的ip地址，网络序
返回值说明： 返回指定ip在配置中的状态：
#define IP_SET_AND_UP	        1  ---address set and up
#define IP_SET_AND_DOWN	        2  ---address set and down
#define IP_NOT_SET	            3  ---address not set
==================================*/
u32 BrdChkOneIpStatus(u32 dwIpAdrs);

/*================================
函数名：BrdGetIfStatus
功能：获取if 状态
算法实现：（可选项）
引用全局变量：
输入参数说明：byEthId : 接口编号
返回值说明： 返回指定接口的状态

#define IF_UP			0
#define IF_DOWN		1
==================================*/
STATUS BrdGetIfStatus(u32 byEthId);
/*====================================================================
函数名：BrdSysReboot
功能：重新启动系统
算法实现：（可选项）
引用全局变量：
输入参数说明：
返回值说明：
====================================================================*/
void BrdSysReboot();

/*================================
函数名：BrdSaveNipConfig
功能：保存nip的网络配置文件
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdSaveNipConfig(void);

/*================================
函数名：BrdIpConflictCallBackReg
功能：注册ip地址冲突时回调函数,当设备的IP地址和外部设备冲突时，会调用该函数通知业务程序
算法实现：（可选项）
引用全局变量：
输入参数说明：ptFunc：执行业务程序注册的回调函数指针
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdIpConflictCallBackReg (TIpConflictCallBack  ptFunc);

/*================================
函数名：BrdIpOnceConflicted
功能：查询系统是否发生过ip地址冲突
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： TRUE or FALSE
==================================*/
BOOL32 BrdIpOnceConflicted (void);

/*================================
函数名：BrdPing
功能：以太网ping接口
算法实现：（可选项）
引用全局变量：
输入参数说明：pchDestIP:Ping的目的IP地址
              ptPingOpt：Ping的参数结构体指针
              nUserID:用户ID标志，用户区分不同的用户调用
              ptCallBackFunc:Ping结果的回调函数
返回值说明： TRUE--传入的参数合法，该函数调用成功，但是否ping通需要用户注册的回调函数来判断
             FALSE--传入的参数非法，该函数调用失败
==================================*/
BOOL32 BrdPing(s8* pchDestIP,TPingOpt* ptPingOpt,s32 nUserID,TPingCallBack ptCallBackFunc);


/*RawFlash相关的函数声明*/

/*================================
函数名： BrdRawFlashIsUsed
功能：查询是否采用rawflash管理
算法实现：（可选项）
引用全局变量：无
输入参数说明：  无
返回值说明： TRUE:使用/FALSE:不使用
==================================*/
BOOL32   BrdRawFlashIsUsed(void);

/*================================
函数名：BrdGetFullRamDiskFileName
功能：转换相对路径的文件名为ramdisk中带绝对路径的文件名
算法实现：（可选项）
引用全局变量：无
输入参数说明： pInFileName:相对路径的文件名,如"webfiles/doc/www.html";
               pRtnFileName:存放返回绝对路径文件名的地址指针。
               上面的例子会输出:"/ramdisk/webfiles/doc/www.html"
返回值说明： 带绝对路径文件名的长度。
==================================*/
u32 BrdGetFullRamDiskFileName(s8 *pInFileName, s8 *pRtnFileName);

/*================================
函数名：BrdFpUnzipFile
功能：解压复合分区中指定的文件，并输出到指定的文件。
算法实现：（可选项）
引用全局变量：无
输入参数说明： pUnzipFileName:待解压的文件名，可含相对路径，如"/webfiles/doc/aaa.html"。
               pOuputFileName:存放解压后数据的文件名，目前必须解压到/ramdisk/中，如"/ramdisk/bbb.html"
返回值说明  ：OK/ERROR。
==================================*/
STATUS BrdFpUnzipFile(s8 *pUnzipFileName, s8 *pOuputFileName);

/*================================
函数名：BrdFpUnzipFileIsExist
功能：查询复合分区压缩包中指定的压缩文件是否存在。
算法实现：（可选项）
引用全局变量：无
输入参数说明： pUnzipFileName:待解压的文件名
返回值说明： TRUE/FALSE
==================================*/
BOOL32   BrdFpUnzipFileIsExist(s8 *pUnzipFileName);

/*================================
函数名：BrdFpPartition
功能：分区。
算法实现：（可选项）
引用全局变量：无
输入参数说明： ptParam：指向TFlPartnParam结构缓冲区的指针。
返回值说明： OK/ERROR。
==================================*/
STATUS BrdFpPartition(TFlPartnParam *ptParam);

/*================================
函数名：BrdFpGetFPtnInfo
功能：读分区信息
算法实现：（可选项）
引用全局变量：无
输入参数说明： ptParam：指向TFlPartnParam结构缓冲区的指针。
返回值说明： OK/ERROR。
==================================*/
STATUS BrdFpGetFPtnInfo(TFlPartnParam *ptParam);

/*================================
函数名：BrdFpGetExecDataInfo
功能：读取特殊可执行程序的类型、地址和大小
算法实现：（可选项）
引用全局变量：无
输入参数说明： pdwExecAdrs:存放特殊可执行数据相对地址的缓冲区的指针；
                pdwExeCodeType:存放特殊可执行数据类型的缓冲区的指针。
返回值说明： -1出错/其它值为特殊可执行数据的大小。
==================================*/
s32  BrdFpGetExecDataInfo(u32 *pdwExecAdrs, u32 *pdwExeCodeType);

/*================================
函数名：BrdFpReadExecData
功能：读取特殊可执行程序数据
算法实现：（可选项）
引用全局变量：无
输入参数说明： pbyDesBuf:存放特殊可执行数据的缓冲区的指针;
              dwLen: 要读取的数据的长度。
返回值说明： -1出错/其它值为读取特殊可执行程序数据的长度。
==================================*/
s32  BrdFpReadExecData(u8 *pbyDesBuf, u32 dwLen);

/*================================
函数名：BrdFpUpdateExecData
功能：更新特殊可执行程序数据
算法实现：（可选项）
引用全局变量：无
输入参数说明： pFile:FPGA文件名字
返回值说明： OK/ERROR。
==================================*/
STATUS BrdFpUpdateExecData(s8* pFile);

/*================================
函数名：BrdFpWriteExecData
功能：擦除ExeCode特殊可执行程序数据
算法实现：（可选项）
引用全局变量：无
输入参数说明： 无
返回值说明： OK/ERROR。
==================================*/
STATUS BrdFpEraseExeCode(void);

/*================================
函数名：BrdFpUpdateAuxData
功能：更新IOS分区中的数据(IOS分区中的数据组成：kernel+ramdisk，其中，kernel和ramdisk是路由组发布的--update.linux)
算法实现：（可选项）
引用全局变量：无
输入参数说明： pFile:update.linux文件名字
返回值说明： OK/ERROR。
==================================*/
STATUS BrdFpUpdateAuxData(s8* pFile);

/*================================
函数名      : BrdFpEraseAuxData
功能        ：读取用户数据分区的个数。
算法实现    ：（可选项）
引用全局变量：
输入参数说明：无。
返回值说明  ：用户分区的个数
==================================*/
u8  BrdFpGetUsrFpnNum (void);

/*================================
函数名      : BrdFpWriteDataToUsrFpn
功能        ：写指定缓冲区数据到指定的用户数据分区
算法实现    ：（可选项）
引用全局变量：
输入参数说明：byIndex：第几个用户分区（从0开始）
              pbyData：要写入的数据指针
              dwLen：要写入数据的长度
返回值说明  ：OK/ERROR。
==================================*/
STATUS BrdFpWriteDataToUsrFpn(u8 byIndex, u8 *pbyData, u32 dwLen);

/*================================
函数名      : AtaApiMountPartion
功能        ：挂载磁盘分区---Linux上新增的磁盘操作接口
算法实现    ：（可选项）
引用全局变量：
输入参数说明：pchPartionName:要挂载的磁盘分区名称，如"/dev/hda0"
              pMntPath:要挂载的目的路径，如"/ramdisk/hda0"
返回值说明  ：错误返回如ata驱动模块错误码描述错误码；成功, 返回ATA_SUCCESS.
==================================*/
s32 AtaApiMountPartion(s8* pchPartionName,s8* pMntPath);

/*================================
函数名      : BrdNipErase
功能        ：擦除NIP配置信息
算法实现    ：擦除配置；请在console模式下调用该函数；
              telnet上调用该函数会导致telnet无法正常工作
              （由于nip中IP地址被擦除后，telnet无法正常工作）；
引用全局变量：
输入参数说明：无
返回值说明  ：错误返回ERROR；成功返回OK
==================================*/
STATUS BrdNipErase(void);

/*================================
函数名      : BrdGetUpdateInfo
功能        ：获取版本冗余信息
算法实现    ：所有板卡上都支持
引用全局变量：
输入参数说明：无
返回值说明  ：错误返回ERROR；成功返回OK
==================================*/
STATUS BrdGetUpdateInfo(s8* pchFilename, s8* pchInfo);

/*================================
函数名      : BrdCheckUpdatePackage
功能        ：验证要升级的文件
算法实现    ：所有板卡上都支持
引用全局变量：
输入参数说明：
返回值说明  ：错误返回错误号
                  PID检测失败：CHK_PID_FAILURE
		  其他错误   ：ERROR

	      成功返回OK
==================================*/
int BrdCheckUpdatePackage(s8* pchFilename);

/*================================
函数名      : BrdMSSystemSwitch
功能        ：切换主从系统
算法实现    ：所有板卡上都支持
引用全局变量：
输入参数说明：
返回值说明  ：错误返回ERROR；成功返回OK
==================================*/

STATUS BrdMSSystemSwitch(void);

/*================================
函数名      : BrdSysUpdate
功能        ：升级系统
算法实现    ：仅达芬奇板卡上支持
引用全局变量：
输入参数说明：
返回值说明  ：SYSTEM_UPDATE_SUCCESS                系统升级成功
              SYSTEM_VERSION_ERROR                 系统版本校验错误
              SYSTEM_UPDATEVERSION_COMPARE_ERROR   系统升级比对校验错误
==================================*/
u8 BrdSysUpdate(s8* pchFilename);

/*================================
函数名      : BrdAllSysUpdate
功能        ：升级系统
算法实现    ：所有的板卡上都支持
引用全局变量：
输入参数说明：pchFilename : 升级的文件名
              upboot：1:update boot
                      0: don't update boot
返回值说明  ：0: update ok
 * 	     -1: error
 *	      1: app update error
 *	      2: os update error
 *	      3: boot update error
 *	      4: set update flag error
==================================*/
s8 BrdAllSysUpdate(s8* pchFilename, s32 upboot);

/*================================
函数名      : BrdSetSysRunSuccess
功能        ：设置升级成功标识
算法实现    ：所有板卡上都支持
引用全局变量：
输入参数说明：
返回值说明  ：NO_UPDATE_OPERATION       	0  正常启动，无升级操作
              UPDATE_VERSION_ROLLBACK	    1  升级失败，版本回滚。
              UPDATE_VERSION_SUCCESS		2  升级成功，应用新版本成功
              SET_UPDATE_FLAG_FAILED		-1 设置失败
==================================*/
s8 BrdSetSysRunSuccess(void);


/*硬件诊断接口封装函数*/

/*================================
函数名：BrdSetMinUnitCheckable
功能：设置可检测最小数据块大小
算法实现：（可选项）
引用全局变量：
输入参数说明：只能输入以下宏定义的最小大小类型，其他类型返回FALSE，推荐最小以64K字节为单位
              MINSIZE_64K_BYTE  ------   64K
             MINSIZE_128K_BYTE ------   128K
              MINSIZE_512K_BYTE ------   512K
              MINSIZE_1024K_BYTE------   1024K
返回值说明： 成功返回OK，否则返回ERROR
==================================*/
STATUS BrdSetMinUnitCheckable(u8 byUnitSizeType);

/*================================
函数名：BrdGetWriteDataState
功能：获取写入数据完成情况
算法实现：（可选项）
引用全局变量：
输入参数说明：dwTotalLen:待写入文件长度或数据总长度, dwCompleteLen:已写入flash数据长度
返回值说明： 成功返回OK，否则返回ERROR
==================================*/
STATUS BrdGetWriteDataState(u32* pdwTotalLen, u32* pdwCompleteLen);

typedef u32 (*FLASHFUNC)(u32*, u32*);
/*================================
函数名：BrdTestFlashStable
功能：测试flash读写稳定性   按照指定路径写入指定大小的文件，随后读取数据并判断数据是否为预期
	  值，函数返回前将删除测试文件
算法实现：（可选项）
引用全局变量：
输入参数说明：pchFlashPath: 待操作flash分区路径, dwFileSize: 待测试文件大小,
              dwRepeatTimes：反复读写flash次数,pvCallbackEntry:上层传给底层调用的函数指针
返回值说明： 成功返回TRUE，否则返回FALSE
==================================*/
STATUS BrdTestFlashStable(s8* pchFlashPath,u32 dwFileSize, u32 dwRepeatTimes, FLASHFUNC pvCallbackEntry);

typedef struct{
	u32 dwBoudRate;  /*串口波特率，有效范围2400～115200，配置前请确定硬件设计支持该波特率。*/
	u32 dwDataBits;  /*数据位，有效值为5、6、7、8*/
	u32 dwStopBits;  /*停止位，有效值为1、2*/
	u32 dwParity;    /*串口校验模式，0-.-no, 1---odd, 2---even, 3----space, 4----mark 等校验类型。
						配置前请确定硬件设计支持该校验模式。*/
}TSerialAttribte;

/*================================
函数名：BrdTestUartFun
功能：按照指定属性配置串口，并发送/接收100个字节的数据。
	  发送数据为指定文件中，接收到的数据将存放在/usr/recv文件中，
	  供测试人员与对端设备比较收发数据是否一致。
算法实现：（可选项）
引用全局变量：
输入参数说明：byPort串口端口号
              #define SERIAL_RS232                 0
              #define SERIAL_RS422                 1
              #define SERIAL_RS485                 2
              #define BRD_SERIAL_INFRARED          3
			  tSerialAttribte :串口的属性设置
			  pchFileWholeName:待发送的测试文件名 如：/usr/send
返回值说明： 成功返回OK，否则返回ERROR
==================================*/
STATUS BrdTestUartFun(u8 byPort,TSerialAttribte tSerialAttribte,s8* pchFileWholeName);

/*================================
函数名：BrdChip2Start
功能：通过VLYNQ、UART2启动CHIP2，判断CHIP2是否启动正常。
算法实现：（可选项）
引用全局变量：
输入参数说明：
返回值说明： 成功返回OK，否则返回ERROR
==================================*/
STATUS BrdChip2Start(void);

/*================================
函数名：BrdTestLedFunc
功能：通过指定的GPIO或EPLD使所有存在led依次点亮
算法实现：（可选项）
引用全局变量：
输入参数说明：dwTime:点亮持续时间，以秒为单位
返回值说明： 成功返回OK，否则返回ERROR
==================================*/
STATUS BrdTestLedFunc(u32 dwTime);

/*================================
函数名：BrdTestRelayFunc
功能：控制GPIO输出来控制继电器开闭
算法实现：（可选项）
引用全局变量：
输入参数说明：
返回值说明： 成功返回OK，否则返回ERROR
==================================*/
STATUS BrdTestRelayFunc();

/*================================
函数名：BrdTestEpldFunc
功能：从EPLD读取/写入数据信息
算法实现：（可选项）
引用全局变量：
输入参数说明：dwCmd		：epld操作命令，WRITE_CMD：0，READ_CMD：1
			  dwOffset	：待操作的epld地址
			  dwValue	：写入/读出的数据

返回值说明： 成功返回OK，否则返回ERROR
==================================*/
STATUS BrdTestEpldFunc(u32 dwCmd, u32 dwOffset, s8* pchValue);

/*================================
函数名：BrdTestWatchdoFunc
功能：利用看门狗来控制设备复位
算法实现：（可选项）
引用全局变量：
输入参数说明：
返回值说明： 成功返回OK，否则返回ERROR
==================================*/
STATUS BrdTestWatchdoFunc();

/*申明一些空函数头头文件,以兼容以往BoardWrapper的头文件*/
/*================================
函数名：BrdGetBSP15Speed
功能：获取BSP-15的主频
算法实现：（可选项）
引用全局变量：
输入参数说明： byDevID:BSP-15的ID号(从0开始）
返回值说明： 成功返回BSP-15的主频,失败返回ERROR
==================================*/
u8 BrdGetBSP15Speed(u8 byDevID);

/*================================
函数名：BrdGetBSP15SdramSize
功能：获取BSP-15的SDRAM的大小
算法实现：（可选项）
引用全局变量：
输入参数说明： byDevID:BSP-15的ID号(从0开始）
返回值说明： 成功返回BSP-15SDRAM的大小,失败返回ERROR
==================================*/
u8 BrdGetBSP15SdramSize(u8 byDevID);

/*================================
函数名：SysRebootHookAdd
功能：复位回调注册函数
算法实现：（可选项）
引用全局变量：
输入参数说明：  指向VOIDFUNCPTR类型的函数指针
返回值说明： 成功返回OK ；失败返回ERROR
==================================*/
STATUS SysRebootHookAdd(VOIDFUNCPTR ptRebootHookFunc);

/*====================================================================
函数名      : BrdSetWatchdogMode
功能        ：设置看门狗工作模式
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：byMode:如下看门狗宏定义
                #define WATCHDOG_USE_CLK            0x00   时钟硬件喂狗
                #define WATCHDOG_USE_SOFT           0x01    软件喂狗
                #define WATCHDOG_STOP                0x02   停止喂狗，设备会立即重启
返回值说明  ：无
说明：由于在VxWorks上由业务程序喂狗频繁出现问题，因此，在Linux上做了调整，喂狗这一部分
统一调用上面提供的系统守卫的一套函数接口，该接口在Linux上不再提供。
====================================================================*/
void   BrdSetWatchdogMode(u8 byMode);

/*====================================================================
函数名      : BrdFeedDog
功能        ：喂狗函数,1.6秒内至少调一次，否则系统会重启，调用该函数的任务的优先级必须很高，建议为最高优先级
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：无
返回值说明  ：无
说明：由于在VxWorks上由业务程序喂狗频繁出现问题，因此，在Linux上做了调整，喂狗这一部分
统一调用上面提供的系统守卫的一套函数接口，该接口在Linux上不再提供。
====================================================================*/
void   BrdFeedDog(void);


/*====================================================================
函数名      : BrdSetFanState
功能        ：设置系统风扇运行状态
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：byRunState：模式
                #define BRD_FAN_RUN                  0       风扇转动
                #define BRD_FAN_STOP                 1       风扇停转
返回值说明  ：成功返回OK ；失败返回ERROR
====================================================================*/
STATUS BrdSetFanState(u8 byRunState);

/*====================================================================
函数名      : BrdFanIsStopped
功能        ：查询指定风扇是否停转
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：无
返回值说明  ：TRUE=风扇停转 / FALSE=风扇正常
====================================================================*/
BOOL32   BrdFanIsStopped(u8 byFanId);

/*====================================================================
函数名      : BrdVideoMatrixSet
功能        ：视频交换矩阵设置
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：注：选择的端口范围视矩阵板类型而定，16x16矩阵输入输出端口取值范围：1~16
            由于硬件面积无法全部使用，只使用了15个）；8x8取值范围：1~8。
            videoInSelect = 视频输入源选择： 1~16/8输入端口， 0xfb = 关相应的输出；
            0xfc = 开相应的输出，默认所有的输出口都是打开的，如果业务程序关闭了某
            个输出口，则必须先打开这个输出口，然后在设置交换（分两步做）
            videoOutSelect = 视频输出端口选择：1~16/8视频输出接口
            说明：在KDV8010A上，设备面板上的6个输入/数出口对应的输入/输出端口号是：2~7
返回值说明  ：无
====================================================================*/
void BrdVideoMatrixSet(u8 byInSelect, u8 byOutSelect);

/*====================================================================
函数名      : BrdGetSdramMaxSize
功能        ：取板子内存最大值,以MByte为单位
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：无
返回值说明  ：内存最大值,以MByte为单位
====================================================================*/
u32 BrdGetSdramMaxSize(void);

/*====================================================================
函数名      : BrdGetAllDiskInfo
功能        ：获取flash分区信息
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：ptBrdAllDiskInfo ：指向TbrdAllDiskInfo结构的指针
返回值说明  ：错误返回ERROR；成功返回OK
====================================================================*/
STATUS BrdGetAllDiskInfo(TBrdAllDiskInfo *ptBrdAllDiskInfo);

/*====================================================================
函数名      : BrdGetFullFileName
功能        ：转换文件名为带全路径的文件名
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  byPutDiskId:盘的Id；
                pchInFileName:输入的文件名 ，不带盘符；
                pchRtnFileName:返回的加上Id号盘符全路径的文件名
返回值说明  ：返回的加上Id号盘符的文件名的长度，以字节为单位，0为错误
====================================================================*/
u32 BrdGetFullFileName(u8 byPutDiskId, s8 *pchInFileName, s8 *pchRtnFileName);

/*====================================================================
函数名      : SysSetUsrAppParam
功能        ：单个用户程序启动参数设置，为了兼容vx上的接口
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  pbyFileName:应用文件名
                byFileType :有两套宏定义，含义相同，为了兼容以前vx上
                #define LOCAL_UNCOMPRESS_FILE   0 本地未压缩的文件
                #define LOCAL_COMPRESS_FILE  1 本地压缩的文件
                #define REMOTE_UNCOMPRESS_FILE 2 远端未压缩的文件
                #define REMOTE_COMPRESS_FILE  3 远端压缩的文件
                #define LOCAL_ZIP_FILE  4 本地zip压缩包中的文件
                pAutoRunFunc:为了兼容以前vx上接口而设，本函数无效
返回值说明  ：成功返回OK ；失败返回ERROR
====================================================================*/
STATUS SysSetUsrAppParam(s8 *pbyFileName, u8 byFileType, s8 *pAutoRunFunc);

/*====================================================================
函数名      : SysSetMultiAppParam
功能        ：多个用户程序启动参数设置，Linux上新增接口
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：  ptAppLoadInf:指向应用配置信息的数据结构的指针
返回值说明  ：成功返回OK ；失败返回ERROR
====================================================================*/
STATUS SysSetMultiAppParam(TAppLoadInf *ptAppLoadInf);

/*================================
函数名：BrdGetBSP15CapturePort
功能：BSP15图像采集端口查询
算法实现：（可选项）
引用全局变量：
输入参数说明： byBSP15ID:BSP-15的ID号(从0开始）
返回值说明： BSP15图像采集端口宏定义
==================================*/
u8  BrdGetBSP15CapturePort(u8 byBSP15ID);


/*================================
函数名：BrdGetSAA7114OutPort
功能：BSP15图像输出接口查询
算法实现：（可选项）
引用全局变量：
输入参数说明： byBSP15ID:BSP-15的ID号(从0开始）
返回值说明： SAA7114图像数据输出端口宏定义
==================================*/
u8  BrdGetSAA7114OutPort(u8 byBSP15ID);

/*================================
函数名：BrdGetCaptureChipType
功能：BSP15图像采集芯片查询
算法实现：（可选项）
引用全局变量：
输入参数说明： byBSP15ID:BSP-15的ID号(从0开始）
返回值说明： 图像采集芯片型号
==================================*/
u8  BrdGetCaptureChipType(u8 byBSP15ID);

/*================================
函数名：BrdGetAudCapChipType
功能：提供给codec上层查询BSP15音频采集芯片的类型
算法实现：（可选项）
引用全局变量：
输入参数说明： byBSP15ID:BSP-15的ID号(从0开始）
返回值说明： 音频采集芯片型号
==================================*/
u8  BrdGetAudCapChipType(u8 byBSP15ID);

/*================================
函数名：BrdBsp15GpdpIsUsed
功能：查询哪些BSP15的gpdp端口的互相连接
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： bsp15使用gpdp互联状况的掩码，每个bit位代表一个bsp15，0=不使用/1=使用，
低位为0号map， 最大支持32个map互联
==================================*/
u32 BrdBsp15GpdpIsUsed(void);

/*================================
函数名：BrdGetBSP15VGAConf
功能：获取指定bsp15的vga配置
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： byDevId：bsp15的设备号
低位为0号map， 见VGA配置宏定义
==================================*/
u8  BrdGetBSP15VGAConf(u8 byDevId);

/*================================
函数名：BrdStopVGACap
功能：停止采集vga(8083)数据(控制fpga)
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： 无
==================================*/
void BrdStopVGACap(void);

/*================================
函数名：BrdStartVGACap
功能：开始采集vga(8083)数据(控制fpga)
算法实现：（可选项）
引用全局变量：
输入参数说明：byMode:健上面的VGA采集模式宏定义
返回值说明： 无
==================================*/
void BrdStartVGACap(u8 byMode);

/*================================
函数名：BrdSetVGACapMode
功能：开始采集vga(8083)数据(控制fpga)
算法实现：（可选项）
引用全局变量：
输入参数说明：byMode:健上面的VGA采集模式宏定义
返回值说明： 无
==================================*/
void BrdSetVGACapMode (u8 byMode);

/*================================
函数名：BrdCloseVGA
功能：关闭并禁止vga(8083)采集芯片工作
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： 无
==================================*/
void BrdCloseVGA(void);

/*================================
函数名：BrdOpenVGA
功能：打开并开始vga(8083)采集芯片工作
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： 无
==================================*/
void BrdOpenVGA(void);

/*================================
函数名：BrdSetBsp15GPIOMode
功能：GPIO总线切换控制信号,仅对第11版kdv8010a有效
算法实现：（可选项）
引用全局变量：
输入参数说明：byMode:模式，见相关的宏定义
返回值说明： 无
==================================*/
void BrdSetBsp15GPIOMode(u8 byMode);

/*================================
函数名：BrdMapDevOpenPreInit
功能：在map启动时对相关硬件的初始化
算法实现：（可选项）
引用全局变量：
输入参数说明：byBSP15ID：bsp15的设备号
返回值说明：无
==================================*/
void BrdMapDevOpenPreInit(u8 byBSP15ID);

/*================================
函数名：BrdMapDevClosePreInit
功能：在map关闭时对相关硬件的复位等操作
算法实现：（可选项）
引用全局变量：
输入参数说明：byBSP15ID：bsp15的设备号
返回值说明：无
==================================*/
void BrdMapDevClosePreInit(u8 byBSP15ID);

/*====================================================================
函数名      : BrdAudioMatrixSet
功能        ：音频交换矩阵设置
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：注：byAudioInSelect = 输入源选择：0~6 = 7个输入接口，0号输入硬件内部
          已接至解码后的图像，对外屏蔽。0xb = 关相应的输出；0xc = 开相应的输出；
          byAudioOutSelect = 视频输出端口选择：0~6 = 7个视频输出接口，
          0号输出硬件内部，已接至编码数据源，对外屏蔽。其余保留。
返回值说明  ：无
====================================================================*/
void  BrdAudioMatrixSet(u8 byAudioInSelect, u8 byAdioOutSelect);

/*====================================================================
函数名      : BrdGetMatrixType
功能        ：矩阵板类型查询
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：无
返回值说明  ：矩阵板类型，具体如矩阵板型号宏定义
====================================================================*/
u8 BrdGetMatrixType(void);

/*====================================================================
函数名      : BrdGetPowerVoltage
功能        ：获取当前电压值，以毫伏为单位
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：dwSampleTimes 采样次数，0表示使用默认值8
返回值说明  ：成功返回电压值，以毫伏为单位；无效返回ERROR
====================================================================*/
s32 BrdGetPowerVoltage(u32 dwSampleTimes);


/*====================================================================
函数名      : BrdShutoffPower
功能        ：关闭电源供应
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：无
返回值说明  ：成功返回OK；失败返回ERROR
====================================================================*/
STATUS BrdShutoffPower(void);

/*====================================================================
函数名      : BrdGetSwitchSel
功能        ：获取开关选择的状态
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：无
返回值说明  ：0 or 1
====================================================================*/
u8 BrdGetSwitchSel(void);

/*================================
函数名：BrdGetRouteWayBandwidth
功能：读取俄E1捆绑线路的带宽值，以Kbps为单位
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： 线路带宽值，以Kbps为单位
==================================*/
u32 BrdGetRouteWayBandwidth(void);

/*================================
函数名：BrdGetEthActLnkStat
功能：真实的以太网连接状态检测
算法实现：（可选项）
引用全局变量：
输入参数说明：byEthID：要查询的以太网索引号
返回值说明： 0=down/1=up
==================================*/
u8 BrdGetEthActLnkStat(u8 byEthID);


/*================================
函数名：BrdSetV35Param
功能：设置v35接口参数
算法实现：（可选项）
引用全局变量：
输入参数说明：  dwDevId：v35接口的编号（0 ~ X）X视不同板子而定；
                dwIpAdrs：为ip地址
                dwIpMask：为掩码地址
返回值说明： 出错返回ERROR；成功返回OK。
==================================*/
STATUS BrdSetV35Param(u32 dwDevId, u32 dwIpAdrs, u32 dwIpMask);

/*================================
函数名：BrdGetV35Param
功能：获取v35接口参数
算法实现：（可选项）
引用全局变量：
输入参数说明：  dwDevId：v35接口的编号（0 ~ X）X视不同板子而定；
pdwIpAdrs：指向存放返回ip地址的指针
pdwIpMask: 指向存放返回掩码地址的指针
返回值说明： 出错返回ERROR；成功返回OK。
==================================*/
STATUS BrdGetV35Param(u32 dwDevId, u32 *pdwIpAdrs, u32 *pdwIpMask);

/*================================
函数名：BrdDelV35Param
功能：删除指定的v35接口地址
算法实现：（可选项）
引用全局变量：
输入参数说明：  dwDevId：v35接口的编号（0 ~ X）X视不同板子而定
返回值说明： 出错返回ERROR；成功返回OK。
==================================*/
STATUS BrdDelV35Param(u32 dwDevId);

/*================================
函数名：BrdLineIsLoop
功能：检查当前E1线路自环状态(如果有多个E1线路自环，则找到第一个自环的E1线路并返回IP地址)
算法实现：（可选项）
引用全局变量：
输入参数说明：  pdwIpAddr：存放接口ip地址的指针，仅在返回值为TRUE时有效
返回值说明： 自环则返回TRUE ；没有自环则返回FALSE
==================================*/
BOOL32   BrdLineIsLoop(u32 *pdwIpAddr);

/*================================
函数名：E1TransChanOpen
功能：打开一个E1透明传输通道，初始化相应的mcc_channel
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ChanID：打开的通道号,必须小于 E1_TRANS_CHAN_MAX_NUM
		      ptChanParam：通道的参数设置
              ptE1MsgCallBack：注册的回调函数
返回值说明： E1_TRANS_FAILURE/E1_TRANS_SUCCESS
==================================*/
s32 E1TransChanOpen( u8 byE1ChanID,TE1TransChanParam *ptChanParam,TE1MsgCallBack  ptE1MsgCallBack );

/*================================
函数名：E1TransChanClose
功能：关闭mcc控制器一个E1透明传输通道
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ChanID：打开的通道号,必须小于 E1_TRANS_CHAN_MAX_NUM
返回值说明： E1_TRANS_FAILURE/E1_TRANS_SUCCESS
==================================*/
s32 E1TransChanClose( u8 byE1ChanID);

/*================================
函数名：E1TransChanMsgSnd
功能：拷贝模式发送数据包
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ChanID:设备号
		      tMsgParam:消息数据信息指针
返回值说明： 见E1透明传输部分的返回值宏定义
==================================*/
s32 E1TransChanMsgSnd(u8 byE1ChanID, TE1TransMsgParam *ptMsgParam);

/*================================
函数名：E1TransTxPacketNumGet
功能：读取指定通道发送缓冲区队列中待发送的数据包的个数
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ChanID:设备号
返回值说明： 出错返回E1_TRANS_FAILURE；成功返回队列中待发送的数据包的个数
==================================*/
s32 E1TransTxPacketNumGet(u8 byE1ChanID);

/*================================
函数名：E1TransBufQHeadPtrInc
功能：移动读指针
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ChanID:设备号
              dwOffset：地址偏移
返回值说明： 见E1透明传输部分的返回值宏定义
==================================*/
s32 E1TransBufQHeadPtrInc(u8 byE1ChanID, u32 dwOffset);

/*================================
函数名：E1TransBufMsgCopy
功能：从中读取指定长度的数据到指定缓冲区，不移动读指针
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ChanID:设备号
              pbyDstBuf:读出数据保存区
              dwSize:期望读出长度
返回值说明： 错误返回0；成功返回读出的字节数
==================================*/
s32 E1TransBufMsgCopy(u8 byE1ChanID, u8 *pbyDstBuf, u32 dwSize);

/*================================
函数名：E1TransBufMsgLenGet
功能：查询当前可读数据的长度
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ChanID:设备号
返回值说明： 错误返回0；成功返回可读数据长度
==================================*/
s32 E1TransBufMsgLenGet(u8 byE1ChanID);

/*================================
函数名：E1TransBufMsgByteRead
功能：读取当前读指针开始指定偏移的一个字符，不移动读指针
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ChanID:设备号
              dwOffset:地址偏移
              pbyRtnByte:存字节数据的指针
返回值说明： 见E1透明传输部分的返回值宏定义
==================================*/
s32 E1TransBufMsgByteRead(u8 byE1ChanID, u32 dwOffset, u8 *pbyRtnByte);

/*================================
函数名：E1TransChanLocalLoopSet
功能：将指定通道物理线路自环
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ChanID:设备号
              bIsLoop：是否将该链路设置为自环模式
返回值说明： 见E1透明传输部分的返回值宏定义
==================================*/
s32 E1TransChanLocalLoopSet(u8 byE1ChanID, BOOL32 bIsLoop);

/*================================
函数名：E1TransChanInfoGet
功能：获取指定通道数据收发的统计信息
算法实现：（可选项）
引用全局变量：
输入参数说明：byE1ChanID:设备号
              ptInfo:存统计信息的指针
返回值说明： 见E1透明传输部分的返回值宏定义
==================================*/
s32 E1TransChanInfoGet(u8 byE1ChanID, TE1TransChanStat *ptInfo);

/*================================
函数名：BrdOpenE1SingleLinkChan
功能：创建一个E1单链路连接通道
算法实现：（可选项）
引用全局变量：
输入参数说明：dwChanID: 单链路连接通道号，范围0~ E1_SINGLE_LINK_CHAN_MAX_NUM -1，该ID具有唯一性，用来标识每个单链路连接通道，不可冲突。一个通道不可重复打开，必须先关闭
              ptChanParam:单链路连接通道参数结构指针
返回值说明： E1_RETURN_OK/ERRCODE
==================================*/
STATUS BrdOpenE1SingleLinkChan(u32 dwChanID, TBrdE1SingleLinkChanInfo *ptChanParam);

/*====================================================================
函数名      : BrdCloseE1SingleLinkChan
功能        ：关闭指定的E1单链路连接通道
算法实现    ：。
引用全局变量：无
输入参数说明：dwChanID: 单链路连接通道号，范围0~ E1_SINGLE_LINK_CHAN_MAX_NUM -1，
            该ID具有唯一性，用来标识每个单链路连接通道，不可冲突。
            该通道必须是已经打开成功的，如果未打开则返回E1_ERR_CHAN_NOT_CONF;
返回值说明  ：E1_RETURN_OK/ERRCODE。
====================================================================*/
STATUS BrdCloseE1SingleLinkChan(u32 dwChanID);

/*====================================================================
函数名      : BrdGetE1SingleLinkChanInfo
功能        ：读取指定的E1单链路连接通道信息
算法实现    ：调用路由组提供的nip模块的接口。
引用全局变量：无
输入参数说明：dwChanID: 单链路连接通道号，范围0~ E1_SINGLE_LINK_CHAN_MAX_NUM -1，
            该ID具有唯一性，用来标识每个单链路连接通道，不可冲突。
            该通道必须是已经打开成功的，如果未打开则返回E1_ERR_CHAN_NOT_CONF;
                ptChanInfo:单链路连接通道参数结构指针.
返回值说明  ：E1_RETURN_OK/ERRCODE。
====================================================================*/
STATUS BrdGetE1SingleLinkChanInfo(u32 dwChanID, TBrdE1SingleLinkChanInfo *ptChanInfo);

/*====================================================================
函数名      : BrdOpenE1MultiLinkChan
功能        ：创建一个E1多链路捆绑连接通道
算法实现    ：调用路由组提供的nip模块的接口。
引用全局变量：无
输入参数说明：dwChanID: 多链路捆绑连接通道号，范围0~ dwMultiLinkChanNum -1，
            该ID具有唯一性，用来标识每个单链路连接通道，不可冲突。一个通道不可重复打开，
            必须先关闭;
             ptChanParam:多链路捆绑连接通道参数结构指针.
返回值说明  ：E1_RETURN_OK/ERRCODE。
====================================================================*/
STATUS BrdOpenE1MultiLinkChan(u32 dwChanID, TBrdE1MultiLinkChanInfo *ptChanParam);

/*====================================================================
函数名      : BrdCloseE1MultiLinkChan
功能        ：关闭指定的E1多链路捆绑连接通道
算法实现    ：。
引用全局变量：无
输入参数说明：dwChanID: 多链路捆绑连接通道号，范围0~ dwMultiLinkChanNum -1，
            该ID具有唯一性，用来标识每个多链路捆绑连接通道，不可冲突。
            该通道必须是已经打开成功的，如果未打开则返回E1_ERR_CHAN_NOT_CONF;
返回值说明  ：E1_RETURN_OK/ERRCODE。
====================================================================*/
STATUS BrdCloseE1MultiLinkChan(u32 dwChanID);

/*====================================================================
函数名      : BrdGetE1MultiLinkChanInfo
功能        ：读取指定的E1多链路捆绑连接通道信息
算法实现    ：调用路由组提供的nip模块的接口。
引用全局变量：无
输入参数说明：dwChanID: 多链路捆绑连接通道号，范围0~ dwMultiLinkChanNum -1，
            该ID具有唯一性，用来标识每个单链路连接通道，不可冲突。
            该通道必须是已经打开成功的，如果未打开则返回E1_ERR_CHAN_NOT_CONF;
            ptChanInfo:多链路捆绑连接通道参数结构指针.
返回值说明  ：E1_RETURN_OK/ERRCODE。
====================================================================*/
STATUS BrdGetE1MultiLinkChanInfo(u32 dwChanID, TBrdE1MultiLinkChanInfo *ptChanInfo);


/*================================
函数名      : BrdFastLoadEqtFileIsEn
功能        ：是否允许bsp15快速启动
算法实现    ：（可选项）
引用全局变量：
输入参数说明：无
返回值说明  ：0=禁止快速启动bsp15，从flash启动，速度较慢;
              1=允许快速启动bsp15，从内存镜像启动，速度较快，但占用较多内存;
==================================*/
u8 BrdFastLoadEqtFileIsEn(void);

/*================================
函数名      : BrdNipWrapperGetVersion
功能        ：获取nipwrapper的版本号
算法实现    ：（可选项）
引用全局变量：
输入参数说明：version长度为NIPWRAPPER_VERSION_LEN的字符串
返回值说明  ：NULL 或版本号
==================================*/
LPSTR BrdNipWrapperGetVersion(LPSTR version);

/*================================
函数名：BrdDelE1IntfConf
功能：删除一个E1接口
算法实现：（可选项）
引用全局变量：
输入参数说明：dwE1Id: E1 接口号
返回值说明： E1_RETURN_OK/ERRCODE
==================================*/
STATUS BrdDelE1IntfConf(u32 dwE1Id);

/*================================
函数名：BrdChangFtpPortRange
功能：设置ftp server在PASV模式下传输数据使用端口范围
算法实现：（可选项）
引用全局变量：
输入参数说明：min: 最小port值
					    max: 最大port值
返回值说明：  OK/ERROR
==================================*/
STATUS BrdSetFtpPortRange(u32 dwmin, u32 dwmax);
/*================================
函数名：BrdInterfaceShutdown
功能：关闭所选接口
算法实现：（可选项）
引用全局变量：
输入参数说明：ifname:网口类型名("ethernet","fastethernet","gigabitEthernet","serial","loopback","virtual")
					  ifunit :网口序号				

返回值说明： OK/ERROR
==================================*/
STATUS BrdInterfaceShutdown(char *ifname,int ifunit);

/*================================
函数名：BrdInterfaceNoShutdown
功能：打开所选接口
算法实现：（可选项）
引用全局变量：
输入参数说明：ifname:网口类型名("ethernet","fastethernet","gigabitEthernet","serial","loopback","virtual")
					  ifunit :网口序号				

返回值说明： OK/ERROR
==================================*/
STATUS BrdInterfaceNoShutdown(char *ifname,int ifunit);

/*================================
函数名：BrdGetE1Bandwidth
功能：获取指定E1接口带宽
算法实现：（可选项）
引用全局变量：无
输入参数说明：ifname: 指定接口类型 ifunit:指定接口号
返回值说明： 成功:dwBandWidth/失败:0
==================================*/
u32 BrdGetE1Bandwidth(char *ifname , int ifunit);

/*================================
函数名：BrdGetAllE1Route
功能：读
算法实现：（可选项）
引用全局变量：
输入参数说明：ptBrdAllE1RouteInfo：指向存放返回路由信息的TBrdAllE1RouteInfo结构指针
返回值说明： 错误返回ERROR；成功返回OK
==================================*/
STATUS BrdGetAllE1Route(TBrdAllE1RouteInfo *ptBrdAllE1RouteInfo);

/*================================
函数名：BrdE1TransH221Start
功能：开始E1H221发送
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： 错误返回ERROR；成功, 返回OK
==================================*/
STATUS BrdE1H221Start(void);

/*================================
函数名：BrdE1TransH221Stop
功能：停止E1H221发送
算法实现：（可选项）
引用全局变量：
输入参数说明：无
返回值说明： 错误返回ERROR；成功, 返回OK
==================================*/
STATUS BrdE1H221Stop(void);

/*================================
函数名：BrdSetE1H221Inf
功能：设置E1H221的接口号
算法实现：（可选项）
引用全局变量：
输入参数说明：进行透传的E1接口号
返回值说明： 错误返回ERROR；成功, 返回OK
==================================*/
STATUS BrdSetE1H221Inf(int e1);

/*================================
函数名：BrdUnsetE1H221Inf
功能：清除E1H221的接口号
算法实现：（可选项）
引用全局变量：
输入参数说明：进行透传的E1接口号
返回值说明： 错误返回ERROR；成功, 返回OK
==================================*/
STATUS BrdUnsetE1H221Inf(int e1);

/*================================
函数名：BrdSetE1Clock
功能：添加或删除E1的时钟模式，dce或者dte
算法实现：（可选项）
引用全局变量：
输入参数说明：e1为E1接口号
              clock为("dce"或"dte")
              cmd为NIP_API_ADD_CFG，即1，则添加配置
              cmd为NIP_API_DEL_CFG，即2，则删除配置
返回值说明： 错误返回ERROR；成功, 返回OK
==================================*/
STATUS BrdSetE1Clock(int e1,char *clock,int cmd);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*_NIP_WRAPPER_H*/
