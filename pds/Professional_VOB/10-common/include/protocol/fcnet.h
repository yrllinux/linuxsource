#ifndef _FCNET_H_
#define _FCNET_H_

#include "kdvtype.h"

#define			INTERFCNET_VERION		"INTERFCNET.40.0.0.01.071224"

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
注：本模块涉及的所有ip 均为u32网络序
	所有码率:单位bit/s
	所有时间单位:millisecond
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*
错误码
*/
//正确返回值
#define		INTERFCNET_OK					(u16)0
#define		INTERFCNET_ERROR_BASE			(u16)16500
//ip无效
#define		INTERFCNET_INVALID_IP			INTERFCNET_ERROR_BASE + 1
//创建信号量错误
#define		INTERFCNET_SEM_CREATE			INTERFCNET_ERROR_BASE + 2
//内存不足
#define		INTERFCNET_NO_MEMORY			INTERFCNET_ERROR_BASE + 3
//流控模块初始化失败
#define		INTERFCNET_DCB_START			INTERFCNET_ERROR_BASE + 4
//流控模块退出失败
#define		INTERFCNET_DCB_STOP				INTERFCNET_ERROR_BASE + 5
//通道组满
#define		INTERFCNET_CHANNEL_FULL			INTERFCNET_ERROR_BASE + 6
//本模块没有初始化
#define		INTERFCNET_NOT_INIT				INTERFCNET_ERROR_BASE + 7
//通道组加入流控模块失败
#define		INTERFCNET_ADD_FC_FAIL			INTERFCNET_ERROR_BASE + 8
//从流控模块删除通道组失败
#define		INTERFCNET_DEL_FC_FAIL			INTERFCNET_ERROR_BASE + 9
//触发通道组流控计算失败
#define		INTERFCNET_TRIGGER_FC_FAIL		INTERFCNET_ERROR_BASE + 10
//带宽检测失败
#define		INTERFCNET_BAND_DETECT_FAIL		INTERFCNET_ERROR_BASE + 11
//
#define		INTERFCNET_NULL_POINT		INTERFCNET_ERROR_BASE + 12
//////////////////////////////////////////////////////////////////////////

//默认通道数
#define			INTERFCNET_DEFAULT_CHANNEL_NUM		(u32)256
//默认fc定时器超时值（毫秒）
#define			INTERFCNET_DEFAULT_TIMEOUT_MILLIS	(u32)5000

/**/
//fc定时器回调时,fcnet构建的统计命令(请求)
#define			FCNET_STAIS_TYPE_REQ	1
//fc定时器回调时,fcnet构建的统计命令的返回信息(回应)
#define			FCNET_STAIS_TYPE_ACK	2

/**/
//流控计算时,用最大值,最小值,平均值
typedef enum
{
		STATIS_USE_VALUE_AVERAGE	=	0,		//取平均值
		STATIS_USE_VALUE_MAX		=	1,		//取最大值
		STATIS_USE_VALUE_MIN		=	2		//取最小值
}TStatisValueType;


//固定字段，fcnet以u8形式传给medianet，medianet已u8形式原样传回fcnet
typedef struct tagStatisFixField
{
	u64 m_dwTimestamp;				//本通道的本次统计包的时间戳
	u32 m_dwSrcRtpIp;					//源ip
	u16 m_wSrcRtpPort;					//源port
	u16 m_wTargetRtcpPort;				//目的port?????
	u32 m_dwTargetRtcpIp;				//目的ip，标识所属的通道组	
	u32 m_dwFcTimeOutTimeStamp;			//fc超时的时间戳
	u64	m_qwSendOutTimeStamp;			//
	u64 m_qwSendInTimeStamp;			//
}TStatisFixField;

//变化字段，当medianet已u8形式传回fcnet数据时，
//必须在固定字段后这个结构的字段顺序在固定字段后添加数据
typedef struct tagStatisVaryField
{
	u32 m_dwRecvPackNum;//本次的收包数
	u32 m_dwExptPackNum;//本次的期望包数
	u32 m_dwRecvdBytes;//本次的收到的字节数
	u32	m_dwReserved;			//
	u64	m_qwRtcpInTimeStamp;	//
	u64 m_qwRtcpOutTimeStamp;	//
}TStatisVaryField;

//单个mediasnd对象的每次fc定时器超时统计信息
typedef struct tagPackStatis
{
	u8 m_byCommandType;				//
	u8 m_byAlignField;				//????
	u16 m_wAlignField;				//????
	u64 m_dwRtt;					//本通道的延迟
	double m_llLostFraction;		//丢包率
	u32 m_dwCompareField;			//用于正确性判断,目前没用
	TStatisFixField m_tStatisFixField;
	TStatisVaryField m_tStatisVaryField;
}TPackStatis;



/*===============================
typedef void (*BDNOTICECB)(u32 dwIp, u32 dwBandWidth, u32 dwContext);
//流控动态带宽通知回调
u32 dwip: 目的ip，标识通道组
u32 dwBd：通道组的建议带宽
u32 dwContext：用户数据
================================*/
typedef void (*BDNOTICECB)(u32 dwIp, u32 dwBandWidth, u32 dwContext);

/*===============================
typedef void(*BDDETECTCB)(u32 dwTargetIp, u32 dwBandWidth, u32 dwContext);
//带宽检测通知回调
u32 dwip: 目的ip，标识通道组
u32 dwBd：通道组的建议带宽
u32 dwContext：用户数据
================================*/
typedef void(*BDDETECTCB)(u32 dwTargetIp, u32 dwBandWidth, u32 dwContext);


/*
与上层接口
*/
/*==================================
u16 InitFcNet(u16 wCtrlPort, u16 wStartDataPort, u16 wDataPortNum, u32 dwTotalGrpNum=0 )
描述：           在上层用户要求启动时注册、加载，并在本地所有接口的指定端口上
//                  侦听带宽检测请求(信令)，如不指定侦听端口，则在缺省的端口上侦听；
//                  其次，本模块将根据指定起始端口号和端口数，在后续可能的检测发起中
//                  选取指定范围内可用的端口进行检测流量接收(数据)；
//                  第三，本模块也将启动全局负载平衡子模块、获取各接口的物理带宽
//                  并设置相关参数，申请必要的资源
//初始化
//必须在创建mediasnd之前调用
u16 wCtrlPort:控制信令监听port，此为tcp连接
u16 wStartDataPort：检测的数据port,此为udp
u16 wDataPortNum：同时可能有多个检测，此为连续可用的数据port数
u16 wInnerStartPort:模块内部使用的端口可用范围起始值
u16 wInnerPortNum:模块内部使用端口额外可用数量，0表示只有起始值可用，
					1表示可用端口范围为[start_inner_port, start_inner_port+1]，依此类推
u32 dwTotalGrpNum:通道组数，=0则取默认值:INTERFCNET_DEFAULT_CHANNEL_NUM
====================================*/
u16 InitFcNet(u16 wCtrlPort, u16 wStartDataPort, u16 wDataPortNum, u16 wInnerStartPort, u16 wInnerPortNum, u32 dwTotalGrpNum=0 );
u16 QuitFcNet();

/*=================================
u16 RegisterChnlGrp(u32 dwChnlGrpIp, BDNOTICECB pBdNoticeCb, u32 dwContext, u32 dwFcTimeInterval=0);
//按指定时间间隔对去往指定对端的通道组启动/重置自动流量控制
u32 dwIp：目的ip
u32 dwFcTimeInterval：流控时间间隔,=0则取默认值INTERFCNET_DEFAULT_TIMEOUT_MILLIS;
						以毫秒计，合理取值范围为<100, 600000>，为较精确地跟踪码流实际收发情况，
						该值建议设置范围为<500, 5000>
BDNOTICECB pBdNoticeCb：带宽通知回调函数
u32 dwContext：用户数据
==================================*/
u16 RegisterChnlGrp(u32 dwChnlGrpIp, BDNOTICECB pBdNoticeCb, u32 dwContext, u32 dwFcTimeInterval=0);
//关闭对去往指定对端的通道组的流量控制
u16 UnregisterChnlGrp(u32 dwChnlGrpIp);


/*========================================================
u16 DcbDetect(u32 dwTargetIp, u32 dwInitBandWidth, u32 dwContext, BDDETECTCB pBdDetectCb);
//检测从本端到指定地址的全程可用带宽
u32 dwTargetIp:目标ip
u16 wPort:对端检测侦听端口
u32 dwInitBandWidth:检测时初始建议带宽,作用为有利于减少检测次数
u32 dwContext:用于数据
BDDETECTCB pBdDetectCb:带宽检测通知回调
=========================================================*/
u16 DcbDetect(u32 dwTargetIp, u16 wPort, u32 dwInitBandWidth, u32 dwContext, BDDETECTCB pBdDetectCb);
/*
//设置全局默认码率
u16 DcbIfDefRateSet(int, int);//dcb_if_def_rate_set(int, int);
//设置接口可用带宽
u16 DcbIfAvlBwSet(int, int);//dcb_if_avl_bw_set(int, int);
//流控算法可替换定制
u16 DcbFcAlgSet(const char*, unsigned int, ...);//dcb_fc_alg_set(const char*, unsigned int, ...);
//负载均衡算法可替换定制
u16 DcbLbAlgSet(unsigned int, ...);//dcb_lb_alg_set(unsigned int, ...);
//获取指定通道组的负载量及全局负载比
u16 DcbLoadblcGet(const char*, struct load_proportion*);//dcb_loadblc_get(const char*, struct load_proportion*);
//设置指定通道组的负载量及全局负载比
u16	DcbLoadblcSet(const char*, struct load_proportion*);//dcb_loadblc_set(const char*, struct load_proportion*);
*/

/*
//与medianet接口
*/
/*==================================
u16 RegisterChannel(u32 dwChalGrpIp, u16 wTargetRtcpPort, u32 dwSrcRtpIp, u16 wSrcRtpPort, const CKdvMediaSnd* pcMediaSnd);
//向连接件注册mediasnd对象
u32 dwIp：目的ip,标识所在的通道组
const CKdvMediaSnd* pcMediaSnd：通道
===================================*/
u16 RegisterChannel(u32 dwChalGrpIp, u16 wTargetRtcpPort, u32 dwSrcRtpIp, u16 wSrcRtpPort, void* pcMediaSnd);
u16 UnregisterChannel(u32 dwChnlGrpIp, u16 wTargetRtcpPort, u32 dwSrcRtpIp, u16 wSrcRtpPort, void* pcMediaSnd);

/*=======================================================
u16 NoticePackStatis(const TPackStatis& tPackStatis, const CKdvMediaSnd* pcMediaSnd, u32 dwChalGrpIp);
//单个mediasnd对象收到rtcp回应包后向连接件通知统计信息
const TPackStatis& tPackStatis：统计信息结构
const CKdvMediaSnd* pcMediaSnd：通道
u32 dwIp：通道组ip
========================================================*/
u16 NoticePackStatis( u8* pBuf, u16 wBufLen);



API void fcnetver();

#endif

