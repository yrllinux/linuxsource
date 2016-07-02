/*****************************************************************************
   模块名      : Recorder
   文件名      : StateMgr.h
   相关文件    : 
   文件实现功能: Recorder　Server的StateMgr定义
   作者        : 
   版本        : V1.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2010/08/26  1.0         刘 旭         创建
******************************************************************************/

#ifndef STATEMGR_H
#define STATEMGR_H

#include "osp.h"
#include "kdvtype.h"
#include "RecServ_Def.h"

// 类型前置声明
class CStateBase;
class CRecServInst;

class CStateMgr
{
public:
	/** 
	/* 功能: 构造函数
	*/
	CStateMgr(CRecServInst * const pInst);

	/** 
	/* 功能: 析造函数
	*/
	~CStateMgr();

public:
	/** 
	/* 功能: 初始化函数
	*/
	void Initialize(){}

	/** 
	/* 功能: 消息入口函数
	*/
	RET_TYPE OnMessage(CMessage *const);

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
	/*	功能：获取当前的状态
	/*  说明: 为了使新设计的状态模式能正常进行,特此屏蔽了CInstance的NextState
	/*  参数：dwState = 下一个状态的enum值
	/*  返回值：操作结果值,请参照RET_TYPE
	*/	
	RET_TYPE ChangeStateTo(const u32 &);

	/** 
	/*	功能：取得状态机当前状态
	/*  参数：无
	/*  返回值：当前状态值
	*/
    const u32 GetState();

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


	/** 
	/*	功能：存储录像机节点信息
	/*  参数：
	/*       dwNodeId : 节点号
	/*       dwInstId : mt的Instance号，由MAKEID(appid，instanceid)生成
	/*  返回值：无
	*/
	void SetRecNodeInfo(const TNodeInfo& tNode);

	/** 
	/*	功能：存储终端节点信息
	/*  参数：
	/*       dwNodeId : 节点号
	/*       dwInstId : mt的Instance号，由MAKEID(appid，instanceid)生成
	/*  返回值：无
	*/
    void SetMTNodeInfo(const TNodeInfo& tNode);

public:
	/*	功能：获取CRecServInst实例
	/*  参数：无
	/*  返回值：CRecServInst实例指针
	*/	
	CRecServInst* GetOwnInst(){	return m_pServInst; }
	
private:
	CRecServInst*  m_pServInst;					// CRecServInst实例指针
	CStateBase* m_pcState;						// 状态指针
};

#endif