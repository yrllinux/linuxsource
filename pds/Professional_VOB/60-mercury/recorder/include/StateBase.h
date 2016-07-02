/*****************************************************************************
   模块名      : Recorder
   文件名      : StateBase.h
   相关文件    : 
   文件实现功能: Recorder　Server的Base状态定义
   作者        : 
   版本        : V1.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2010/08/26  1.0         刘 旭         创建
******************************************************************************/


#ifndef STATEBASE_H
#define STATEBASE_H

#include "StateMgr.h"

enum ERecState
{
	E_STATE_REC_IDLE  = 0,					// 空闲
	E_STATE_REC_ONREG,						// 注册处理状态
	E_STATE_REC_READY,						// 录像准备好
	E_STATE_RECORDING,						// 正在录像
	E_STATE_REC_PAUSE,						// 录像暂停状态

	E_STATE_REC_INVALID  = 0xFFFF,			// 非法状态
};

class CStateBase
{    
public:
	// 构造函数
    CStateBase(CStateMgr* pMgr, u32 dwState = 0) : m_pcStateMgr(pMgr), m_dwState(dwState) {}
    // 析构函数
	~CStateBase(){}
    
public:
    // 进入时做类的初始化处理,或者进入时开始这个状态的动作处理
    virtual BOOL32 Enter(){return FALSE;}
    // 离开这个状态时的善后处理,比如释放申请的资源等
    virtual BOOL32 Exit(){return FALSE;}
    // 接收消息处理
    virtual RET_TYPE OnMessage(CMessage *const);
    
public:
    // 获得状态
    const u32 GetState(){return m_dwState;}
    
protected:
	/**
	/* 终端请求录像的响应函数
	*/
	RET_TYPE OnMtStartRecReq(CMessage *const);

	/**
	/* 终端请求暂停录像的响应函数
	*/
	RET_TYPE OnMtPauseRecReq(CMessage *const);

	/**
	/* 终端请求停止录像的响应函数
	*/
	RET_TYPE OnMtStopRecReq(CMessage *const);

	/**
	/* 终端请求回复录像的响应函数
	*/
	RET_TYPE OnMtResumeRecReq(CMessage *const);

    // 对StateMgr的封装,切换状态
    RET_TYPE ChangeStateTo(const u32 &dwState);    
    
    // 获取StateMgr
    CStateMgr * const GetStateMgr(){return m_pcStateMgr;}
	
	// 录像机对来源于复终端的请求回复Ack时的响应函数
	RET_TYPE OnMtReqAck(CMessage *const);

	// 录像机对来源于复终端的请求回复nack时的响应函数
	RET_TYPE OnMtReqNack(CMessage *const);

	// 消息发到了错误的状态中时的处理函数
	RET_TYPE OnErrorMsg(CMessage *const);

	// 回复终端Error
	RET_TYPE ReplyMtError(const u16 wEvent, const u16 wErr);

	// 输出当前状态信息
	RET_TYPE OnNodifyEvent(CMessage *const);

private:
	// 输出当前状态信息
	RET_TYPE OnShowStatus(CMessage *const);

	const s8* GetStateStr(const u32 &dwState);

private:
    CStateMgr * const m_pcStateMgr;					// 状态机管理者类
    u32 m_dwState;									// 自身状态值
};


#endif