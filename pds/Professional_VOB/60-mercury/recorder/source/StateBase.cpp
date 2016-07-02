/*****************************************************************************
   模块名      : Recorder
   文件名      : StateBase.cpp
   相关文件    : StateMgr.cpp, State.cpp等
   文件实现功能: Recorder Server的状态机Base部实现
   作者        : 
   版本        : V1.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2010/08/26  1.0         刘 旭        创建
******************************************************************************/

#include "StateBase.h"

/*===================================================================
  函 数 名： OnMessage
  功    能： 状态机Base部消息接收函数
  算法实现： (1). 子状态不处理的消息将会由CStateBase::OnMessage处理
  全局变量： 
  参    数： pcMsg   : CMessage
  返 回 值： RET_TYPE,请参照RET_TYPE定义
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CStateBase::OnMessage(CMessage *const pcMsg)
{
	// 暂时没消息处理
	switch (pcMsg->event)
	{
	case EV_RECSERV_SHOW_STATUS:
		return OnShowStatus(pcMsg);

	case REC_MCU_EXCPT_NOTIF:
	case REC_MCU_RECSTATUS_NOTIF:
		return OnNodifyEvent(pcMsg);

	default:
		return RET_PASSED;
	}	
}


/*===================================================================
  函 数 名： OnMtReqAck
  功    能： 录像机对来源于复终端的请求回复Ack时的响应函数
  算法实现： (1). 先判断此消息是否是录像机的合法消息
			 (2). 从消息中去掉CServMsg的消息头
			 (3). 将消息发给终端
  全局变量： 
  参    数： pcMsg   : CMessage
  返 回 值： RET_TYPE,请参照RET_TYPE定义
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CStateBase::OnMtReqAck(CMessage *const pcMsg)
{
	if (!(GetStateMgr()->IsRecMsgValid(pcMsg)))
	{
		RecServLevelLog(RECSERV_LEVEL_LOG_WARNING, "CRecServInst received un-known msg from recorder\n");
		return RET_FAILED;
	}

	CServMsg cServMsg( pcMsg->content ,pcMsg->length);

	return GetStateMgr()->PostMsgToMt(pcMsg->event, cServMsg.GetMsgBody(), cServMsg.GetServMsgLen());
}


/*===================================================================
  函 数 名： OnMtReqNack
  功    能： 录像机对来源于复终端的请求回复nack时的响应函数
  算法实现： (1). 先判断此消息是否是录像机的合法消息
			 (2). 从消息中提取错误号并形成TErrorInfo
			 (3). 将TErrorInfo内容发给终端
  全局变量： 
  参    数： pcMsg   : CMessage
  返 回 值： RET_TYPE,请参照RET_TYPE定义
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CStateBase::OnMtReqNack(CMessage *const pcMsg)
{
	if (!(GetStateMgr()->IsRecMsgValid(pcMsg)))
	{
		RecServLevelLog(RECSERV_LEVEL_LOG_WARNING, "CRecServInst received un-known msg from recorder\n");
		return RET_FAILED;
	}

	CServMsg cServMsg( pcMsg->content ,pcMsg->length);
	return ReplyMtError(pcMsg->event, cServMsg.GetErrorCode());
}

/*===================================================================
  函 数 名： ReplyMtError
  功    能： Nack时回复终端Error
  算法实现：
  全局变量： 
  参    数： 
              wEvent  : Nack事件号
				wErr  : Error号
  返 回 值： RET_TYPE,请参照RET_TYPE定义
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CStateBase::ReplyMtError(const u16 wEvent, const u16 wErr)
{
	// 输出错误信息
	RecServLevelLog(RECSERV_LEVEL_LOG_COMMON, "[ReplyMtError]Error Event[%d, %d] occured in State[%d]\n", wEvent, wErr, GetState());

	// 设置错误信息
	TErrorInfo tErro = {0};
	tErro.m_wErrCode = wErr;
	return GetStateMgr()->PostMsgToMt(wEvent, (u8 *)&tErro, sizeof(tErro));
}

/*===================================================================
  函 数 名： OnErrorMsg
  功    能： 录像机对来源于复终端的请求回复nack时的响应函数
  算法实现： 在录像机服务器中一般不进行状态转换
  全局变量： 
  参    数： pcMsg   : CMessage
  返 回 值： RET_TYPE,请参照RET_TYPE定义
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CStateBase::OnErrorMsg(CMessage *const pcMsg)
{
	// 输出错误信息
	RecServLevelLog(RECSERV_LEVEL_LOG_WARNING, "State[%d] received error msg[%d]", GetState(), pcMsg->event);
	return RET_DONE;
}

/*===================================================================
  函 数 名： ChangeStateTo
  功    能： 状态切换函数
  算法实现：
  全局变量： 
  参    数： dwState   : 新状态的Enum值
  返 回 值： RET_TYPE,请参照RET_TYPE定义
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CStateBase::ChangeStateTo(const u32 &dwState)
{
	if (m_pcStateMgr)
	{
		return m_pcStateMgr->ChangeStateTo(dwState);
	}
	
	RecServLevelLog(RECSERV_LEVEL_LOG_ERROR, "m_pcStateMgr is NULL, please check\n");
	return RET_FAILED;
}

/*===================================================================
  函 数 名： OnMtStartRecReq
  功    能： 终端请求录像的响应函数
  算法实现： 基本逻辑如下:
			 (1).此终端与已注册终端是否匹配.不匹配则nack
			 (2).将消息发给录像机,录像机申请空闲录像通道
			 (3).等待录像机回复
  全局变量： 
  参    数： pcMsg : osp的CMessage
  返 回 值： void 
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CStateBase::OnMtStartRecReq(CMessage *const pcMsg)
{
	// 验证消息来源是否合法
	if( !(GetStateMgr()->IsMtReqValid(pcMsg)) )
	{
		// 输出错误信息
		RecServLevelLog(RECSERV_LEVEL_LOG_WARNING, "CRecServInst received un-authored request\n");
		
		// 回复Nack
		TErrorInfo tError = {0};
		tError.m_wErrCode = ERR_REC_SERVER_INVALID;
		OspPost(pcMsg->srcid, pcMsg->event + 2, &tError, sizeof(tError), pcMsg->srcnode);

		return RET_FAILED;
	}

	// 向录像机请求录像
	GetStateMgr()->PostMsgToRecDaemInst(pcMsg->event, pcMsg->content, pcMsg->length);

	return RET_DONE;
}

/*===================================================================
  函 数 名： OnMtPauseRecReq
  功    能： 终端请求暂停录像的响应函数
  算法实现： 
  全局变量： 
  参    数： pcMsg : osp的CMessage
  返 回 值： RET_TYPE 
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CStateBase::OnMtPauseRecReq(CMessage *const pcMsg)
{
	// 验证消息来源是否合法
	if( !(GetStateMgr()->IsMtReqValid(pcMsg)) )
	{
		RecServLevelLog(RECSERV_LEVEL_LOG_WARNING, "Mt request to Pause Rec failed, for invalid request\n");
		TErrorInfo tError = {0};
		tError.m_wErrCode = ERR_REC_SERVER_INVALID;
		OspPost(pcMsg->srcid, pcMsg->event + 2, &tError, sizeof(tError), pcMsg->srcnode);
		
		return RET_FAILED;
	}
	
	// 向录像机请求暂停录像
	return GetStateMgr()->PostMsgToRecChnInst(pcMsg->event, pcMsg->content, pcMsg->length);
}

/*===================================================================
  函 数 名： OnMtStopRecReq
  功    能： 终端请求停止录像的响应函数
  算法实现： 
  全局变量： 
  参    数： pcMsg : osp的CMessage
  返 回 值： RET_TYPE 
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CStateBase::OnMtStopRecReq(CMessage *const pcMsg)
{
	// 验证消息来源是否合法
	if( !(GetStateMgr()->IsMtReqValid(pcMsg)) )
	{
		RecServLevelLog(RECSERV_LEVEL_LOG_WARNING, "Mt request to Stop Rec failed, for invalid request\n");
		TErrorInfo tError = {0};
		tError.m_wErrCode = ERR_REC_SERVER_INVALID;
		OspPost(pcMsg->srcid, pcMsg->event + 2, &tError, sizeof(tError), pcMsg->srcnode);
		
		return RET_FAILED;
	}
	
	// 向录像机请求停止录像
	return GetStateMgr()->PostMsgToRecChnInst(pcMsg->event, pcMsg->content, pcMsg->length);
}

/*===================================================================
  函 数 名： OnMtResumeRecReq
  功    能： 终端请求回复录像的响应函数
  算法实现： 
  全局变量： 
  参    数： pcMsg : osp的CMessage
  返 回 值： RET_TYPE 
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CStateBase::OnMtResumeRecReq(CMessage *const pcMsg)
{
	// 验证消息来源是否合法
	if( !(GetStateMgr()->IsMtReqValid(pcMsg)) )
	{
		RecServLevelLog(RECSERV_LEVEL_LOG_WARNING, "Mt request to Stop Rec failed, for invalid request\n");
		TErrorInfo tError = {0};
		tError.m_wErrCode = ERR_REC_SERVER_INVALID;
		OspPost(pcMsg->srcid, pcMsg->event + 2, &tError, sizeof(tError), pcMsg->srcnode);
		
		return RET_FAILED;
	}
	// 向录像机请求恢复录像
	return GetStateMgr()->PostMsgToRecChnInst(pcMsg->event, pcMsg->content, pcMsg->length);
}

/*===================================================================
  函 数 名： OnMtResumeRecReq
  功    能： 终端请求回复录像的响应函数
  算法实现： 
  全局变量： 
  参    数： pcMsg : osp的CMessage
  返 回 值： RET_TYPE 
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CStateBase::OnShowStatus(CMessage *const pcMsg)
{
	RecServLog("CRecServInst[%2d] is On %s\n", GETINS(pcMsg->dstid), GetStateStr(GetState()));
	return RET_DONE;
}

/*===================================================================
  函 数 名： OnNodifyEvent
  功    能： 将Nofity转发给终端
  算法实现： 
  全局变量： 
  参    数： pcMsg : osp的CMessage
  返 回 值： RET_TYPE 
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/09/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CStateBase::OnNodifyEvent(CMessage *const pcMsg)
{
	// 状态不对,回复Pass
	if (E_STATE_REC_IDLE == GetState())
	{
		return RET_PASSED;
	}

	CServMsg cServMsg( pcMsg->content ,pcMsg->length);
	return GetStateMgr()->PostMsgToMt(pcMsg->event, cServMsg.GetMsgBody(), cServMsg.GetServMsgLen());
}


/*===================================================================
  函 数 名： GetStateStr
  功    能： 根据状态值返回对应的字符串描述
  算法实现： 
  全局变量： 
  参    数： dwState : 状态值,参照ERecState
  返 回 值： 对应的字符串描述 
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
const s8* CStateBase::GetStateStr(const u32 &dwState)
{
	switch (dwState)
	{
	case E_STATE_REC_IDLE:
		return "Idle Status";
	case E_STATE_REC_ONREG:
		return "Register Status";
	case E_STATE_REC_READY:
		return "Ready Status";
	case E_STATE_RECORDING:
		return "Recording Status";
	case E_STATE_REC_PAUSE:
		return "Pause Status";
	default:
		return "Invalid Status";
	}
}