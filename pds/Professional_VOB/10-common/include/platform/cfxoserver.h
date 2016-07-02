/*****************************************************************************
  模块名      : 
  文件名      : CFxoServer.h
  相关文件    : 
  文件实现功能: 
  作者        : 张飞
  版本        : V1.0  Copyright(C) 1997-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2007/1/19  1.0         张飞      创建
******************************************************************************/

#ifndef INCLUDE_CFXOSERVER_H_HEADER_INCLUDED_BA50DB82
#define INCLUDE_CFXOSERVER_H_HEADER_INCLUDED_BA50DB82

#include "kdvtype.h"
#include "osp.h"


#ifdef _LINUX_
#define THREAD_VAL void*
#define THREAD_RTN NULL
#else
#define THREAD_VAL void
#define THREAD_RTN 
#endif

const u32 dwMaxPhoneLen = 32;

enum EMFxoAnswerMode { 
    // 拒绝来电		
    emFxoReject, 
	// 提示来电
	emFxoManu 
};
	
enum EMFxoState { 
	// 振铃过程中
	emFxoAlerting, 
	// 通话线路建立
	emFxoConnected, 
	// 话挂断
	emFxoDisconnected,
	//取机失败
	emFxoError
};
		
typedef void (*TFxoCallIncoming)(void* pParam,u32 dwContext);
typedef void (*TFxoCallBack)(EMFxoState emState,void* pParam,u32 dwContext);
		

class CFxoServer
{

public:
	CFxoServer();
	// 启动FXO服务程序
	// TDTMFCallBack  pProperty 需要注册的属性回调函数
	// dwProcessContext    属性回调上下文环境
	// TDTMFCallIncoming 需要注册的来电回调函数
	// dwIncomingContext   来电回调上下文
	BOOL FxoInit(TFxoCallBack pfCallBack, u32 dwProcessContext, TFxoCallIncoming pfCallIncoming, u32 dwIncomingContext);
	virtual ~CFxoServer();
public:
	// 主动发起一个FXO呼叫
	// const s8* const szPhoneNumber 电话号码字符串
	void MakeCall(const s8* const szPhoneNumber);

	// 拒绝呼入
	void RejectCall();
	
	// 在呼叫中拨号（分机等）
	// const s8* const szPhoneNumber 电话号码字符串
	void DailInCall(const s8* const szPhoneNumber);
	

	// 摘起话筒接受一个FXO通话
	void AcceptCall();
	// 挂断一个一个FXO通话
	void Hungup();
	// 设置FXO应答模式
	void FxoSetAnswerMode(EMFxoAnswerMode emMode);
protected:
	enum EMFxoRunState { 
		// FXO处于空闲状态
		emFxoSFree, 
		// FXO忙
		emFxoSBusy, 
		// FXO正在建立呼叫
		emFxoSMakeCall, 
		// FXO处于接听状态
		emFxoSAcceptCall, 
		// FXO处于接听状态
		emFxoSRejectCall, 
		// FXO处于挂断状态
		emFxoSHangup 
	};
	// 检测线程
	static THREAD_VAL Excute(void* pParam);
	// 状态翻转控制
	BOOL Do();
	// 检测振铃是否结束：
	// TRUE，结束；
	// FALSE，还没有结束
	BOOL RingIsEnd(BOOL bCleanRing);
protected:
	// 在呼叫中拨号（分机等）
	// const s8* const szPhoneNumber 电话号码字符串
	BOOL DialInCall();

	// 创建一个FXO连接
	BOOL Pickup2Call();
	//接受一个FXO呼叫
	BOOL Accept();
	//挂断一个FXO呼叫
	BOOL Deny();
	//拒绝一个FXO呼叫
	BOOL Reject();
protected:	
	// FXO运行状态
	volatile EMFxoRunState m_emFxoRunState;	
	//FXO接听模式
	volatile EMFxoAnswerMode  m_emFxoAnswerMode;
	// 电话号码
	s8 m_achPhoneNumber[dwMaxPhoneLen+1];
	//信号量保护，确保状态值正确
	SEMHANDLE m_hMutex;
private:
	// FXO状态回调函数
	TFxoCallBack m_pfCallBack;	
	// FXO有呼叫到来回调函数
	TFxoCallIncoming m_pfCallIncoming;	
	// FXO有呼叫来回调函数上下文
	u32 m_dwCallIncomingContext;	
	// FXO状态回调函数上下文
	u32 m_dwCallBackContext;
private:
	//转换状态值
	const s8* str_run_state(EMFxoRunState state);	
};


//一下两个函数属于微调函数，API性质的
void SetCallIncomingInterval(u32 dwSecond);
void SetFxoThreadSleepInterval(u32 dwMacroSecond);

BOOL  GetFxoServerVersion(s8* szVerbuf, u8& dwLen);

		
#endif /* INCLUDE_CFXOSERVER_H_HEADER_INCLUDED_BA50DB82 */
