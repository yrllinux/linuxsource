 /*****************************************************************************
   模块名      : mcu
   文件名      : mcuinttest.h
   相关文件    : 
   文件实现功能: Eqp集成测试接口说明
   作者        : zhangsh
   版本        : V3.5  Copyright(C) 2001-2004 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2004/09/11  3.5         zhangsh       创建
******************************************************************************/
#ifndef __MCUEQPINT_H_
#define __MCUEQPINT_H_
#include "kdvtype.h"
#include "osp.h"
#include "codeccommon.h"
#include "mcustruct.h"
#include "eqpcfg.h"
#ifdef WIN32
    #pragma comment( lib, "ws2_32.lib" ) 
    #pragma pack( push )
    #pragma pack( 1 )
    #define window( x )	x
#else
    #include <netinet/in.h>
    #define window( x )
#endif

#define MIXLISTEN_PORT 2031
#define BASLISTEN_PORT 2032
#define TWLISTEN_PORT 2033
#define PRSLISTEN_PORT 2034
#define VMPLISTEN_PORT 2035
struct TKDVVMPParam
{
	u8          m_byVMPStyle;      //画面合成风格,参见mcuconst.h中画面合成风格定义
	u8    	    m_byEncType;       //图像编码类型
	u16         m_wBitRate;        //编码比特率(Kbps)
	u16   	    m_wVideoWidth;     //图像宽度(default:720)
	u16   		m_wVideoHeight;    //图像高度(default:576)
	u8      	m_byMemberNum;     //参加视频复合的成员数量
	TMt        	m_atMtMember[16];  //视频复合成员，按照复合方式的逻辑编号顺序
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TVmpWorkData
{
	u16 m_wLocalPort;//起始端口号,由配置文件给出
	BOOL32 m_bConnected; //链路状态
	TNetAddress m_tSndAddr[2];   //合成后输出地址
	BOOL32  bDbVid;	//是否双视频	
	u8      channelnum;
	TKDVVMPParam tParam[2];	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TBasWorkData
{
	//各实例公用数据成员
	static  unsigned long       m_dwMcuRcvIP;//MCU 接收地址
	static  unsigned short        m_wMcuRcvStartPort;//MCU 接收起始端口号
	static  unsigned char        m_byChnNum;//通道个数
	static  TEqpCfg     m_tCfg;//适配器配置
	static  TEqp        m_tEqp;//本设备
	static  u32         m_bEmbedMcu; //是否内嵌在MCU中
	CConfId             m_cConfId;    // 混音组服务的会议ID
	unsigned short                m_wLocalBindPort; //本地绑定起始端口号    
	unsigned short                m_wLocalRcvPort;  //本地接收起始端口号
	unsigned char                m_byGrpIdx; //组
	unsigned char                m_byChnIdx; //通道索引
	unsigned char                m_bySN;
	TBasChnStatus       m_tChnStatus;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TPrsWorkData
{
	u16      m_wLocalPort;//起始端口号,由配置文件给出
	int       m_byMcuNode; //连接的MCUNode号
	BOOL     m_bConnected; //链路状态
	TPrsCfg m_tPrsCfg;      //创建参数
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TTwWorkData
{
	u32        m_dwMcuIId ;// 通信mcu表示
    u32        m_dwMcuNode;// 与MCU通信节点号
	u8          m_byTvNum;  // 电视墙画面数
	TEqpCfg     m_tCfg;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TEqpWorkData
{
	union
	{
		TTwWorkData tTw;
		TPrsWorkData tPrs;
		TBasWorkData tBas;
		TVmpWorkData tVmp;
	}m_tData;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

class CMcuEqpIntTester/**/
{	
private:
	u32  m_dwMcuNode;
public:
    CMcuEqpIntTester();
    virtual ~CMcuEqpIntTester();
public:
	
	//建立Tester与指定Vmp的连接
	virtual BOOL ConnectEqp( u32 dwVmpIp, //Mcu Ip地址
					 u16 wPort    //Mcu 侦听端口
					 );

	//断开Tester与vmp间的连接
	virtual BOOL DisconnectEqp();

	//获取
	/*
	*	#define     AID_MIXER						62	//混音器
	#define     AID_TVWALL						63	//电视墙
	#define     AID_RECORDER					64	//录像机
	#define     AID_DCS							65	//数据会议服务器
	#define     AID_BAS							66	//码率适配服务器
	#define     AID_MP							67	//MP
	#define	    AID_VMP			1个实例			68	//画面合成器
	#define     AID_PRS							69  //包重发
	 */
	virtual BOOL GetEqpData(u16 wApp,u16 ins,TEqpWorkData &data);
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

#ifdef WIN32
#pragma pack( pop )
#endif

#endif