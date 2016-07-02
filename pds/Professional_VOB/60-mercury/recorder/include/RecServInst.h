/*****************************************************************************
   模块名      : Recorder
   文件名      : RecServInst.h
   相关文件    : 
   文件实现功能: Recorder　Server的Instance定义
   作者        : 
   版本        : V1.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2010/08/26  1.0         刘 旭         创建
******************************************************************************/


#ifndef RECSERVINST_H
#define RECSERVINST_H

#include "osp.h"
#include "mcustruct.h"
#include "RecServ_Def.h"
#include "StateMgr.h"


class CRecServInst : public CInstance
{
	// 为便于在其它实例中使用CInstance的保护成员NextState，特将自身设为友元
    friend class CRecServInst;
	// CStateMgr可以操作CRecServInst的私有成员
	friend class CStateMgr;
public:
	/** 
	/*	功能: 构造函数
	*/
    CRecServInst();

	/** 
	/*	功能: 析构函数
	*/
	~CRecServInst();
    
public:
	/** 
	/*	功能: 守护实例消息入口
	*/
    void DaemonInstanceEntry( CMessage * const pMsg, CApp* pcApp);

    /** 
	/*	功能: 普通实例消息入口
	*/
	void InstanceEntry( CMessage * const pcMsg );

public:
	/** 
	/*	功能：获取当前的状态
	/*  参数：无
	/*  返回值：当前状态值
	*/
	const u32 GetState() const {return m_pcStateMgr->GetState();}


	/** 
	/*	功能：获取当前的状态
	/*  说明: 为了使新设计的状态模式能正常进行,特此屏蔽了CInstance的NextState
	/*  参数：dwState = 下一个状态的enum值
	/*  返回值：操作结果值,请参照RET_TYPE
	*/
	const u32 NextState(u32 dwState) const {return m_pcStateMgr->ChangeStateTo(dwState);}

	/** 
	/*	功能：将消息发给终端
	/*  参数：
	/*          wEvent   = 消息号
	/*          pContent = 消息内容
	/*          dwLength = 消息内容长度
	/*  返回值：操作结果值,请参照RET_TYPE
	*/
	RET_TYPE PostMsgToMt(const u16 wEvent, const u8* pContent = NULL, const u16 dwLength = 0);
	
	/** 
	/*	功能：将消息发给录像机守护实例
	/*  参数：
	/*          wEvent   = 消息号
	/*          pContent = 消息内容
	/*          dwLength = 消息内容长度
	/*  返回值：操作结果值,请参照RET_TYPE
	*/
    RET_TYPE PostMsgToRecDaemInst(const u16 wEvent, const u8* pContent = NULL, const u16 dwLength = 0);

	/** 
	/*	功    能：将消息发给录像机的录像通道实例
	/*  前置条件: 已经与此通道建立连接,如已经请求录像
	/*  参    数：
	/*          wEvent   = 消息号
	/*          pContent = 消息内容
	/*          dwLength = 消息内容长度
	/*  返回值：操作结果值,请参照RET_TYPE
	*/
	RET_TYPE PostMsgToRecChnInst(const u16 wEvent, const u8* pContent = NULL, const u16 dwLength = 0);

	/** 
	/*	功能：存储录像机节点信息
	/*  参数：
	/*       dwNodeId : 节点号
	/*       dwInstId : mt的Instance号，由MAKEID(appid，instanceid)生成
	/*  返回值：无
	*/
	void SetRecNodeInfo(const TNodeInfo& tNode)
	{
		m_tRecNodeInfo.m_dwAppId = tNode.m_dwAppId;
		m_tRecNodeInfo.m_dwNodeId = tNode.m_dwNodeId;
	}

	/** 
	/*	功能：存储终端节点信息
	/*  参数：
	/*       dwNodeId : 节点号
	/*       dwInstId : mt的Instance号，由MAKEID(appid，instanceid)生成
	/*  返回值：无
	*/
    void SetMTNodeInfo(const TNodeInfo& tNode)
	{
		m_tMTNodeInfo.m_dwAppId = tNode.m_dwAppId;
		m_tMTNodeInfo.m_dwNodeId = tNode.m_dwNodeId;
	}

private:        
    /** 
	/*	功能：守护实例中终端注册响应函数
	*/
    void OnMTRegisterReq(CMessage * const, CApp*);

    /** 
	/*	功能：守护实例中打印录像机服务器状态函数
	*/
    void OnDaemShowStatus(CMessage * const, CApp*);

    /** 
	/*	功能：守护实例中转来自录像机的Nodity消息
	*/
    void OnDaemNodifyEvent(CMessage * const, CApp*);

    /** 
	/*	功能：普通实例终端连接掉线时的响应函数
	*/
    void OnDisconnectMsg(CMessage * const);

private:
    /** 
	/*	功能：守护实例从CApp中获取一个空闲的普通实例
	/*  参数：CApp ：本app实例
	/*  返回值：CInstance* ： 指向本地的某个空闲Instance的指针
	*/
    CRecServInst* GetIdleInst(CApp*);

	/** 
	/*	功能：返回CStateMgr对象指针
	*/
	CStateMgr* GetStateMgr() const {return m_pcStateMgr;}

	/** 
	/*	功能：验证来自终端的请求是否合法
	/*  参数：pcMsg ：CMessage
	/*  返回值：如果此终端已经注册,则True,否则False
	*/
	BOOL32 IsMtReqValid(CMessage * const pcMsg);

	/** 
	/*	功能：验证来自录像机的请求是否合法
	/*  参数：pcMsg ：CMessage
	/*  返回值：合法返回True,否则False
	*/
	BOOL32 IsRecMsgValid(CMessage * const pcMsg);
    
private:
    CServMsg m_cServMsg;				// 保存从Recorder过来的CServMsg信息头信息
    TNodeInfo m_tMTNodeInfo;			// 终端节点信息
	TNodeInfo m_tRecNodeInfo;			// 录像机的节点信息
	CStateMgr* m_pcStateMgr;			// 状态机入口对象

private:
// GETLOCALAPPID宏定义, 获得本实例的全局号
#define GETLOCALAPPID MAKEIID(GetAppID(), GetInsID())
};

// CRecServApp定义
typedef zTemplate< CRecServInst, MAXNUM_RECORDER_CHNNL > CRecServApp;

#endif