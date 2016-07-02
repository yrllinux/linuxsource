/*****************************************************************************
   模块名      : Recorder
   文件名      : State.h
   相关文件    : 
   文件实现功能: Recorder　Server的子状态定义
   作者        : 
   版本        : V1.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2010/08/26  1.0         刘 旭         创建
******************************************************************************/

#ifndef RECSTATE_H
#define RECSTATE_H

#include "StateBase.h"


class CIdleState : public CStateBase
{
public:
	/**
	/* 构造函数
	*/
    CIdleState(CStateMgr* pMgr) : CStateBase(pMgr, E_STATE_REC_IDLE){}

public:
	/**
	/* 进入时做类的初始化处理,或者进入时开始这个状态的动作处理
	*/
    virtual BOOL32 Enter();
};

class COnRegisterState : public CStateBase
{
public:
	/**
	/* 构造函数
	*/
    COnRegisterState(CStateMgr* pMgr) : CStateBase(pMgr, E_STATE_REC_ONREG){}

public:
	/**
	/* 接收消息处理
	*/
    virtual RET_TYPE OnMessage(CMessage *const);
	
private:
	/**
	/* COnRegisterState状态时响应终端注册请求的函数
	*/
	RET_TYPE OnMtRegReq(CMessage *const);

	/**
	/* 录像机回复注册Ack的响应函数
	*/
	RET_TYPE OnMtRegReqAck(CMessage *const);

	/**
	/* 录像机回复注册Ack的响应函数
	*/
	RET_TYPE OnMtRegReqNack(CMessage *const);
};


class CRecReadyState : public CStateBase
{
public:
	/**
	/* 构造函数
	*/
    CRecReadyState(CStateMgr* pMgr) : CStateBase(pMgr, E_STATE_REC_READY){}

public:
    // 进入时做类的初始化处理,或者进入时开始这个状态的动作处理
    virtual BOOL32 Enter();

    // 接收消息处理
    virtual RET_TYPE OnMessage(CMessage *const);

private:
	/**
	/* 录像机回复开始录像Ack时的响应函数
	*/	
	RET_TYPE OnMtStartRecAck(CMessage *const);
};


class CRecordingState : public CStateBase
{
public:
	/**
	/* 构造函数
	*/
    CRecordingState(CStateMgr* pMgr) : CStateBase(pMgr, E_STATE_RECORDING){}

public:
	/**
	/* 接收消息处理
	*/
    virtual RET_TYPE OnMessage(CMessage *const);
	
private:
	/**
	/* 终端请求录像
	*/
	RET_TYPE OnMtStartRecReq(CMessage *const);
	/**
	/* 录像机请求关键帧的响应函数
	*/
	RET_TYPE OnRecNeedIFrameCmd(CMessage *const);
};


class CRecPauseState : public CStateBase
{
public:
	/**
	/* 构造函数
	*/
    CRecPauseState(CStateMgr* pMgr) : CStateBase(pMgr, E_STATE_REC_PAUSE){}

public:
	/**
	/* 接收消息处理
	*/
    virtual RET_TYPE OnMessage(CMessage *const);
	
private:
	/**
	/* 终端请求录像
	*/
	RET_TYPE OnMtStartRecReq(CMessage *const);

	/**
	/* 录像机对恢复录像恢复Ack
	*/
	RET_TYPE OnResumeRecReqAck(CMessage *const);

	/**
	/* 录像机对恢复录像恢复Nack
	*/
	RET_TYPE OnResumeRecReqNack(CMessage *const);

private:
	/**
	/* 功    能: 终端是否在此状态下请求了开始录像的标志.
	/* 详细说明: 由于在此状态下终端如果请求录像,录像机服务器会向录像机发送录像恢复的命令
	/*  之后录像机会回复恢复录像的Ack或Nack.这时需要向终端回复开始录像的Ack或Nack
	*/
	BOOL32 m_bIsStartRecReq;		
};


#endif