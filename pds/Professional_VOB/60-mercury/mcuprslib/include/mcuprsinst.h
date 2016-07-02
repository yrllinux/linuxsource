/*****************************************************************************
   模块名      : 画面复合器( Video Multiplexer)
   文件名      : McuPrsInst.h
   创建时间    : 2003年 12月 4日
   实现功能    : McuPrsInst.cpp
   作者        : zhangsh
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   03/12/4	   v1.0	       zhangsh	   create
   06/03/21    4.0         张宝卿      代码优化 
******************************************************************************/
#ifndef _MCUPRSINST_H_
#define _MCUPRSINST_H_

#include "osp.h"
#include "evmcueqp.h"
#include "mcustruct.h"
#include "mcuconst.h"
#include "netbuf.h"
#include "eqpcfg.h"
#include "mcuprs.h"



#define   PRS_CONNECT_TIMEOUT           (u16)(3*1000)   // connect time out(s)
#define   PRS_REGISTER_TIMEOUT          (u16)(4*1000)   // register time out(s)
#define   FIRST_REGACK                  (u8)1           // 第一次收到注册Ack

struct NetBufInfo
{
    NetBufInfo()
    {
        Init();
    }
	TNetLocalAddr local;
	TNetAddr	  addr;
	void Init()
	{
		memset( &local, 0, sizeof(TNetLocalAddr) );
		memset( &addr,  0, sizeof(TNetAddr) );
	}
	void EmptyLocal()
	{
		memset( &local, 0, sizeof(TNetLocalAddr) );
	}
	void EmptyFeed()
	{
		memset( &addr, 0, sizeof(TNetAddr) );
	}
};
typedef NetBufInfo TNetBufInfo;

class CMcuPrsInst : public CInstance
{
	enum //实例状态
	{
		IDLE   = 0,
		NORMAL = 1,
	};
public:
	CMcuPrsInst();
	virtual ~CMcuPrsInst();

private:
	TPeriEqpStatus  m_tPrsStatus;    //状态
	TRSParam        m_tRSParam;      //创建参数
	TNetBufInfo tNetBufInfo[MAXNUM_PRS_CHNNL];

private:
	//-----------自身状态----------------
	CNetBuf*   m_pNetBuf[MAXNUM_PRS_CHNNL];         //封装
	u8		   m_sCreated[MAXNUM_PRS_CHNNL];        //保存是否已经创建
private:
	//-------------消息响应----------------
	void InstanceEntry( CMessage *const pcMsg );    //消息入口
	void Init( CMessage *const pcMsg );             //初始化
	void MsgRegAckProc( CMessage* const pcMsg );    //确认
	void MsgRegNackProc( CMessage* const pcMsg );   //拒绝
	void MsgSetSource( CMessage* const pcMsg );     //设置检测源
	void MsgPrintStatus();   //打印通道状态
	void MsgRemvoeAllCh( CMessage* const pcMsg );   //清空所有通道
	void SendStatusChangeMsg();                     //报告状态

	void ProcReconnectBCmd( CMessage* const pcMsg ); //MCU 通知会话 重联MPCB 相应实例, zgc, 2007/04/30

	void ProcMainTaskScanReq( CMessage* const pcMsg ); //prs guard发来的探察请求消息处理，zgc, 2008-04-07

    BOOL InitNetBuf();
private:
	//-------------过程------------------
    BOOL32 ConnectMcu( BOOL32 bConnectA, u32& dwMcuNode ); //处理连接MCU
    void   Disconnect( CMessage *const pcMsg );            //处理断链
    void   ProcConnectTimeOut( BOOL32 bConnectA );         //处理连接超时
    void   Register( BOOL32 bRegisterA, u32 dwMcuNode );   //处理向MCU注册
    void   ProcRegisterTimeOut( BOOL32 bRegisterA );       //处理向MCU注册超时
	void   ProcGetMsStatusRsp( CMessage* const pcMsg );    //处理取主备倒换消息
    void   ClearAllChannelStatus( void );                  //清空所有通道状态
	BOOL32 SendMsgToMcu( u16 wEvent, CServMsg* const pcServMsg ); //发送消息给MCU
};

class CPrsCfg
{
public:
    CPrsCfg();
    virtual ~CPrsCfg();
public:    
    u32     m_dwMcuNode;        //连接的MCU.A 节点号
    u32     m_dwMcuNodeB;       //连接的MCU.B 节点号
    u32     m_dwMcuIId;         //与实例通讯的MCU.A 的全局ID
    u32     m_dwMcuIIdB;        //与实例通讯的MCU.B 的全局ID
    BOOL32  m_bEmbed;           //内嵌于MCU.A板
    BOOL32  m_bEmbedB;          //内嵌于MCU.B板
    u8      m_byRegAckNum;      //第一次收到注册ACK
	u32     m_dwMpcSSrc;        // guzh [6/12/2007] 业务侧会话校验值 
    u16     m_wLocalPort;
    TPrsCfg m_tPrsCfg;          //创建参数
public:
    void FreeStatusDataA( void );
    void FreeStatusDataB( void );
};

void PrsAPIEnableInLinux();

typedef zTemplate< CMcuPrsInst, 1, CPrsCfg > CMcuPrsApp;
extern CMcuPrsApp g_cMcuPrsApp;

#endif
