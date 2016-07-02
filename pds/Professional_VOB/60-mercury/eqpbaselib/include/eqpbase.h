/*****************************************************************************
   模块名      : eqpbase
   文件名      : eqpbase.h
   相关文件    : 
   文件实现功能: 外设基类抽象
   作者        :  周嘉麟
   版本        :  1.0
   日期		   :  2012/02/14
-----------------------------------------------------------------------------
******************************************************************************/

#ifndef _EQP_BASE_H_
#define _EQP_BASE_H_

#include "kdvtype.h"
#include "osp.h"
#include "eqpcfg.h"
#include "mcustruct.h"
#include "eveqpbase.h"
#include "eqpautotest.h"
#include "eqpservertest.h"


#ifdef _LINUX_
#include "nipwrapper.h"
#include "nipwrapperdef.h"
#include "brdwrapperdef.h"
#endif

#ifdef _LINUX12_
#include "brdwrapper.h"
#endif

#define EQP_CONNECT_TIMEOUT            (u16)3*1000     // 连接超时值3s
#define EQP_REGISTER_TIMEOUT           (u16)5*1000     // 注册超时值5s
#define EQP_GETMSSTATUS_TIMEOUT        (u16)8*1000     // 获取主备状态超时值8s
#define FIRST_REGACK                   (u8)1           // 第一次收到注册Ack

char * StrOfIP( u32 dwIP );

/*类前置声明*/
class CEqpBase;

	/*******************************************************************/
	/*				CChnnlStatusBase外设通道状态基类定义	    	   */
	/*******************************************************************/
	#define  	EQP_CHNNL_STATUS_INVALID  0xff			// 通道初始状态
	#define     INVALID_CHNLIDX           0xffff		

class CChnnlStatusBase
{
public:
	CChnnlStatusBase();
	virtual~CChnnlStatusBase();
public:

	//设置通道状态
	BOOL32 SetState(const u8 byState);
	//获取通道状态
	u8     GetState()const;
	//设置媒体模式(音视频)
	void   SetMediaMode(const u8 byMediaMode);
	//获取媒体模式(音视频)
	u8     GetMediaMode()const;
	//设置通道索引
	void   SetChnIdx(const u16 wChnIdx); 	
	//获取通道索引
	u16    GetChnIdx()const;

private:
	u8  m_byState;	    //状态机
	u8  m_byMediaMode;	//音频或视频
	u16 m_wChnIdx;      //通道索引

};

	/**********************************************************************/
	/*					CEqpChnnlBase外设通道基类定义					  */
	/**********************************************************************/

class CEqpChnnlBase
{
public:
	CEqpChnnlBase(const CEqpBase*const pcEqp, CChnnlStatusBase*const pcChnlStatus);
	virtual~CEqpChnnlBase();
public:

	/*******************************消息入口***********************************/
	virtual BOOL32 OnMessage(CMessage* const pcMsg);
	
	/*******************************协议接口***********************************/
	virtual BOOL32 Init();
	virtual BOOL32 Destroy();
	virtual BOOL32 Start(const u8 byMode = MODE_BOTH);
	virtual BOOL32 Stop(const u8 byMode = MODE_BOTH);

	/******************************基本操作接口********************************/
	//发送消息给mcu
	BOOL32  PostMsgToMcu(CServMsg& cMsg, BOOL32 bMcuA = TRUE, BOOL32 bMcuB = TRUE);
	//发送消息给服务器
	BOOL32  PostMsgToServer(CServMsg& cMsg,const BOOL32 bSendHeadInfo = TRUE);
	//设置通道索引
	void	SetChnIdx(const u16 wChnIdx);
	//获取通道索引
	u16     GetChnIdx()const;
	//设置通道状态
	BOOL32  SetState(const u8 byState);
	//获取通道状态
	u8      GetState()const;
	//获取InsId
	u16     GetInsID()const;
	//设置定时器
	void    SetTimer(const u32 dwTimerId, const u32 dwMillionSecs, const u32 dwParam = 0 );
	//取消定时器
	void	KillTimer(u32 dwTimerId);
private:
	const CEqpBase *const m_pcEqp;         //Eqp回调指针
	CChnnlStatusBase*const m_pcChnlState ; //通道基类状态，需实例化
};

	/************************************************************************/
	/*						CEqpCfg外设配置类定义						    */
	/************************************************************************/
class CEqpCfg:public TEqpCfg
{
public:
	CEqpCfg();
	virtual~CEqpCfg();
public:
	void Print();
public:
	u32 m_dwMcuNodeA;				//连接McuA结点号
	u32 m_dwMcuIIdA;				//与McuA通信IID
	u32 m_dwMcuNodeB;				//连接McuB结点号
	u32 m_dwMcuIIdB;				//与McuB通信IID
	u32 m_dwServerNode;             //生产测试结点号
    u32 m_dwServerIId;				//生产测试IID
	u32 m_dwMpcSSrc;				//唯一会话源
	u32 m_dwMcuRcvIp;               //Mcu为外设分配的转发板IP
	u16 m_wMcuRcvStartPort;         //Mcu为外设分配的转发板端口
	u16 m_wEqpVer;					//外设版本
	u8  m_byRegAckNum;				//注册成次数
	TPrsTimeSpan m_tPrsTimeSpan;    //重传参数
	TMcuQosCfgInfo m_tQosInfo;      
};

	/************************************************************************/
	/*							消息发送对象定义			                */
	/************************************************************************/
class CEqpMsgSender
{
public:
	/*******************************协议接口*********************************/
	//给外部发送消息函数
	virtual void   PostEqpMsg( const u32 dwMcuNode, const u32 dwDstId, CServMsg& cMsg,const BOOL32 bSendHeadInfo = TRUE) = 0;	
	// 设置定时
	virtual int    SetTimer( u32 dwTimerId, long nMilliSeconds, u32 dwPara = 0 ) = 0;	
	// 取消定时
	virtual int    KillTimer(u32 dwTimerId) = 0;	
	// 断链注册
	virtual int	   DisConRegister( u32 dwMcuNode ) = 0;	
	// Tcp结点有效性校验
	virtual BOOL32 IsValidTcpNode(const u32 dwTcpNode) = 0;
	// 断开一个Tcp结点
	virtual BOOL32 DisconnectTcpNode(const u32 dwTcpNode) = 0;
	// 创建TCP连接
	virtual int    ConnectTcpNode(const u32 dwConnectIP, const u16 wConnectPort) = 0;
	// 获取实例号
	virtual u16    GetInsID() = 0;
};

	/************************************************************************/
	/*							CEqpBase类定义								*/
	/************************************************************************/
class CEqpBase
{
public:
	enum EEqpState{
		E_EQP_INVALID = 0,					// 未初始化
			E_EQP_OFFLINE,					// 未注册
			E_EQP_ONLINE					// 已经注册
	};

public:
	CEqpBase(CEqpMsgSender*const pcMsgSender, CEqpCfg*const pcCfg);
	virtual~CEqpBase();
public:

	/***********************消息入口************************/
	virtual BOOL32 OnMessage(CMessage* const pcMsg);

	/***********************协议接口************************/
	//初始化
	virtual BOOL32 Init();
	//销毁
	virtual BOOL32 Destroy();
	//连接处理
	virtual BOOL32 OnConnectMcuTimer(CMessage* const pcMsg);
	//注册Mcu处理
	virtual BOOL32 OnRegisterMcuTimer(CMessage* const pcMsg);
	//注册Mcu成功处理
	virtual BOOL32 OnRegisterMcuAck(CMessage* const pcMsg);
	//注册Mcu失败处理
	virtual BOOL32 OnRegisterMcuNack(CMessage* const pcMsg);
	//断链处理
	virtual BOOL32 OnDisConnect(CMessage* const pcMsg);
	//主备状态获取成功处理 /主备状态获取超时处理
	virtual BOOL32 OnGetMsStatusAck(CMessage* const pcMsg);
	//生产测试连接服务器定时处理
	virtual BOOL32 OnConnectServerTimer();
	//生产测试注册服务器定时处理
	virtual BOOL32 OnRegisterServerTimer();
	//外设注册生产测试服务器成功处理
	virtual BOOL32 OnRegisterServerAck(CMessage* const pcMsg);
	//外设注册生产测试服务器失败处理
	virtual BOOL32 OnRegisterServerNack(CMessage* const pcMsg);
	//设置Qos处理
	virtual BOOL32 OnSetQosCmd(CMessage *const pcMsg);
	//修改发送地址
	virtual BOOL32 OnModifyMcuRcvIp(CMessage* const pcMsg);

	/***********************业务接口************************/
	//向mcu发消息
	BOOL32	  PostMsgToMcu( CServMsg& cMsg, const BOOL32 bMcuA = TRUE, const BOOL32 bSndToEqpSSn = FALSE )const;
	//向生产测试服务器发消息
	BOOL32	  PostMsgToServer( CServMsg& cMsg,const BOOL32 bSendHeadInfo = TRUE)const;
	//求Qos复合值
	void      ComplexQos(u8& byValue, u8 byPrior);
	//是否高清外设
	virtual BOOL32 IsHdEqp( )const;
	//设置通道数及为通道分配地址
	BOOL32	  SetChnNum(const u16 wChnNum);
	//获取通道数
	u16       GetChnNum()const;
	//获取通道
	CEqpChnnlBase* GetChnl(const u16 wIndex);
	//通道地址实例化内容
	BOOL32    InitChn(CEqpChnnlBase* pChn, const u16 wChnIdx);
	//设置Eqp状态
	void      SetEqpState(const EEqpState eState);
	//获取Eqp状态
	EEqpState GetEqpState()const;
	//设置定时
	void      SetTimer(const u32 dwTimerId, const u32 dwMillionSecs, const u32 dwParam = 0 )const;
	//取消定时
	void	  KillTimer(u32 dwTimerId) const;
	//获取配置
	const CEqpCfg*  GetEqpCfg()const;
	//获取实例号
	u16       GetInsID()const;
	//校验Tcp结点有效性
	BOOL32    IsValidTcpNode(const u32 dwTcpNode)const;
	//断开一个Tcp结点连接
	BOOL32    DisconnectTcpNode(const u32 dwTcpNode)const;
	//创建TCP连接
	int       ConnectTcpNode(const u32 dwConnectIP, const u16 wConnectPort);
	//获取设备信息
	void      SetDeviceInfo(CDeviceInfo &cDeviceInfo);
private:
	//销毁通道
	BOOL32    DestroyChn();
	//销毁配置
    BOOL32    DestroyCfg();

private:
	CEqpMsgSender*const m_pcMsgSender;      //消息发送对象
	CEqpCfg*			m_pcEqpCfg;			//基本配置，需实例化
	u16					m_wChnNum;		    //外设通道数
	CEqpChnnlBase**		m_ppcChnl;			//外设通道
	EEqpState		    m_eEqpStat;			//外设状态机
};
#endif