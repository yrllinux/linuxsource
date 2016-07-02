/*****************************************************************************
   模块名      : Recorder
   文件名      : State.cpp
   相关文件    : StateMgr.cpp, StateBase.cpp等
   文件实现功能: Recorder Server的状态机实现
   作者        : 
   版本        : V1.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2010/08/26  1.0         刘 旭        创建
******************************************************************************/

#include "State.h"
#include "ChnInst.h"

/*===================================================================
  函 数 名： Enter
  功    能： CIdleState进入后的初始化处理
  算法实现： 
  全局变量： 
  参    数： 无
  返 回 值： 返回True
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
BOOL32 CIdleState::Enter()
{
	// 状态清0
	TNodeInfo tNodeInfo = {0};
	GetStateMgr()->SetRecNodeInfo(tNodeInfo);
	GetStateMgr()->SetMTNodeInfo(tNodeInfo);
	
	return TRUE;
}


/*===================================================================
  函 数 名： OnMessage
  功    能： COnRegisterState的消息接收函数
  算法实现： 
  全局变量： 
  参    数： pcMsg : osp的CMessage
  返 回 值： RET_TYPE 
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE COnRegisterState::OnMessage(CMessage *const pcMsg)
{
	switch (pcMsg->event)
	{
	case MT_RECSERV_REG_REQ:
		return OnMtRegReq(pcMsg);

	case RECSERV_MT_REG_ACK:
		return OnMtRegReqAck(pcMsg);

	case RECSERV_MT_REG_NACK:
		return OnMtRegReqNack(pcMsg);
		
	default:
		return CStateBase::OnMessage(pcMsg);
	}
}


/*===================================================================
  函 数 名： OnMtRegReq
  功    能： COnRegisterState状态时响应终端注册请求的函数
  算法实现： 
  全局变量： 
  参    数： pcMsg : osp的CMessage
  返 回 值： RET_TYPE 
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE COnRegisterState::OnMtRegReq(CMessage *const pcMsg)
{
	// 验证消息体是否合法
    if (sizeof(TMtRegReq) > pcMsg->length)
    {
		// 输出警告信息
		RecServLevelLog(RECSERV_LEVEL_LOG_WARNING, "Register infomation is invalid\n");

		// 回复Nack
		TErrorInfo tError = {0};
		tError.m_wErrCode = ERR_REC_SERVER_ERR;
		GetStateMgr()->PostMsgToMt(RECSERV_MT_REG_NACK, (u8 *)&tError, sizeof(tError));

		ChangeStateTo(E_STATE_REC_IDLE);
        return RET_FAILED;
    }
	
	// 由录像机处理注册
	if(RET_DONE != GetStateMgr()->PostMsgToRecDaemInst(pcMsg->event, pcMsg->content, pcMsg->length))
	{
		// 向录像机发送消息失败,回复Nack
		TErrorInfo tError = {0};
		tError.m_wErrCode = ERR_REC_SERVER_ERR;
		GetStateMgr()->PostMsgToMt(pcMsg->event + 2, (u8 *)&tError, sizeof(tError));
		ChangeStateTo(E_STATE_REC_IDLE);
	}

	return RET_DONE;
}

/*===================================================================
  函 数 名： OnMtRegReqAck
  功    能： COnRegisterState状态时录像机回复注册Ack的响应函数
  算法实现： 
  全局变量： 
  参    数： pcMsg : osp的CMessage
  返 回 值： RET_TYPE 
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE COnRegisterState::OnMtRegReqAck(CMessage *const pcMsg)
{
	if(RET_DONE != OnMtReqAck(pcMsg))
	{
		ChangeStateTo(E_STATE_REC_IDLE);
		return RET_FAILED;
	}

	return ChangeStateTo(E_STATE_REC_READY);
}


/*===================================================================
  函 数 名： OnMtRegReqNack
  功    能： COnRegisterState状态时录像机回复注册Nack的响应函数
  算法实现： 
  全局变量： 
  参    数： pcMsg : osp的CMessage
  返 回 值： RET_TYPE 
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE COnRegisterState::OnMtRegReqNack(CMessage *const pcMsg)
{
	if(RET_DONE != OnMtReqNack(pcMsg))
	{
		ChangeStateTo(E_STATE_REC_IDLE);
		return RET_FAILED;
	}

	return ChangeStateTo(E_STATE_REC_IDLE);
}

/*===================================================================
  函 数 名： Enter
  功    能： CRecReadyState状态时入口处理函数
  算法实现： (1). 将录像通道信息清0
  全局变量： 
  参    数： 无
  返 回 值： 返回True 
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
BOOL32 CRecReadyState::Enter()
{
	// 录像通道状态清0
	TNodeInfo tRecNode = {0};
	GetStateMgr()->SetRecNodeInfo(tRecNode);

	return TRUE;
}

/*===================================================================
  函 数 名： OnMessage
  功    能： CRecReadyState状态时的消息接收函数
  算法实现： 
  全局变量： 
  参    数： pcMsg : osp的CMessage
  返 回 值： RET_TYPE 
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CRecReadyState::OnMessage(CMessage *const pcMsg)
{
	switch (pcMsg->event)
	{
	case MCU_REC_STARTREC_REQ:
		return OnMtStartRecReq(pcMsg);

	case REC_MCU_STARTREC_ACK:
		return OnMtStartRecAck(pcMsg);

	case REC_MCU_STARTREC_NACK:
		return OnMtReqNack(pcMsg);

	case MCU_REC_STOPREC_REQ:
		return ReplyMtError(REC_MCU_STOPREC_NACK, ERR_REC_CURDOING);

	case MCU_REC_RESUMEREC_REQ:
		return ReplyMtError(REC_MCU_RESUMEREC_NACK, ERR_REC_NOTRECORD);

	case MCU_REC_PAUSEREC_REQ:
		return ReplyMtError(REC_MCU_PAUSEREC_NACK, ERR_REC_NOTRECORD);
		
	default:
		return CStateBase::OnMessage(pcMsg);
	}
}


/*===================================================================
  函 数 名： OnMtStartRecAck
  功    能： CRecReadyState状态时录像机回复开始录像Ack时的响应函数
  算法实现： 
  全局变量： 
  参    数： pcMsg : osp的CMessage
  返 回 值： RET_TYPE 
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CRecReadyState::OnMtStartRecAck(CMessage *const pcMsg)
{
	// 保存录像机通道的节点信息
	TNodeInfo tRecNode = {0};
	tRecNode.m_dwAppId = pcMsg->srcid;
	tRecNode.m_dwNodeId = pcMsg->srcnode;
	GetStateMgr()->SetRecNodeInfo(tRecNode);

	// 给终端回复Ack
	OnMtReqAck(pcMsg);
	
	// 切换状态到录像状态
	return ChangeStateTo(E_STATE_RECORDING);
}


/*===================================================================
  函 数 名： OnMessage
  功    能： CRecordingState状态时的消息接收函数
  算法实现： 
  全局变量： 
  参    数： pcMsg : osp的CMessage
  返 回 值： RET_TYPE 
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CRecordingState::OnMessage(CMessage *const pcMsg)
{
	switch (pcMsg->event)
	{
	case MCU_REC_STARTREC_REQ:
		return OnMtStartRecReq(pcMsg);
	
	case MCU_REC_PAUSEREC_REQ:
		return OnMtPauseRecReq(pcMsg);
		
	case REC_MCU_PAUSEREC_ACK:
		OnMtReqAck(pcMsg);
		return ChangeStateTo(E_STATE_REC_PAUSE);
		
	case REC_MCU_PAUSEREC_NACK:
		return OnMtReqNack(pcMsg);
		
	case MCU_REC_STOPREC_REQ:
		return OnMtStopRecReq(pcMsg);
		
	case REC_MCU_STOPREC_ACK:
		OnMtReqAck(pcMsg);
		return ChangeStateTo(E_STATE_REC_READY);
		
	case REC_MCU_STOPREC_NACK:
		return OnMtReqNack(pcMsg);
	
	case MCU_REC_RESUMEREC_REQ:
		// 状态不对,回复Nack
		return ReplyMtError(REC_MCU_RESUMEREC_NACK, ERR_REC_CURDOING);
	
	case REC_MCU_NEEDIFRAME_CMD:
		return OnRecNeedIFrameCmd(pcMsg);
	
	case REC_MCU_RECORDPROG_NOTIF:
		return OnNodifyEvent(pcMsg);
		
	default:
		return CStateBase::OnMessage(pcMsg);
	}
}


/*===================================================================
  函 数 名： OnMtStartRecReq
  功    能： CRecordingState状态时的终端请求录像的响应函数
  算法实现： 
  全局变量： 
  参    数： pcMsg : osp的CMessage
  返 回 值： RET_TYPE 
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CRecordingState::OnMtStartRecReq(CMessage *const pcMsg)
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
	return ReplyMtError(REC_MCU_STARTREC_NACK, ERR_REC_CURDOING);
}


/*===================================================================
  函 数 名： OnRecNeedIFrameCmd
  功    能： CRecordingState状态时的录像机请求关键帧的响应函数
  算法实现： 
  全局变量： 
  参    数： pcMsg : osp的CMessage
  返 回 值： RET_TYPE 
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CRecordingState::OnRecNeedIFrameCmd(CMessage *const pcMsg)
{
	// 验证消息来源是否合法
	if( !(GetStateMgr()->IsRecMsgValid(pcMsg)) )
	{
		RecServLevelLog(RECSERV_LEVEL_LOG_WARNING, "Rec request to get IFrame failed, for invalid request\n");
		return RET_FAILED;
	}

	// 向录像机请求停止录像
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	return GetStateMgr()->PostMsgToMt(pcMsg->event, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
}


/*===================================================================
  函 数 名： OnMessage
  功    能： CRecPauseState状态时的消息接收函数
  算法实现： 
  全局变量： 
  参    数： pcMsg : osp的CMessage
  返 回 值： RET_TYPE 
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CRecPauseState::OnMessage(CMessage *const pcMsg)
{
	switch (pcMsg->event)
	{
	case MCU_REC_STARTREC_REQ:
		return OnMtStartRecReq(pcMsg);
	
	case MCU_REC_RESUMEREC_REQ:
		m_bIsStartRecReq = FALSE;
		return OnMtResumeRecReq(pcMsg);
		
	case REC_MCU_RESUMEREC_ACK:
		return OnResumeRecReqAck(pcMsg);
		
	case REC_MCU_RESUMEREC_NACK:
		return OnResumeRecReqNack(pcMsg);
		
	case MCU_REC_STOPREC_REQ:
		return OnMtStopRecReq(pcMsg);
	
	case MCU_REC_PAUSEREC_REQ:
		return ReplyMtError(REC_MCU_PAUSEREC_NACK, ERR_REC_CURDOING);
	
	default:
		return CStateBase::OnMessage(pcMsg);
	}
}


/*===================================================================
  函 数 名： OnResumeRecReqAck
  功    能： CRecPauseState状态时向录像机请求恢复录像后录像机回复Ack时的处理
  算法实现： 
  全局变量： 
  参    数： pcMsg : osp的CMessage
  返 回 值： RET_TYPE 
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CRecPauseState::OnResumeRecReqAck(CMessage *const pcMsg)
{
	// 如果录像机请求了开始录像,则回复REC_MCU_RESUMEREC_ACK
	if (m_bIsStartRecReq)
	{
		GetStateMgr()->PostMsgToMt(REC_MCU_STARTREC_ACK);
	}else
	{
		if(RET_DONE != OnMtReqAck(pcMsg))
		{
			return RET_FAILED;
		}
	}

	// 向录像机请求恢复录像
	return ChangeStateTo(E_STATE_RECORDING);
}

/*===================================================================
  函 数 名： OnResumeRecReqNack
  功    能： CRecPauseState状态时向录像机请求恢复录像后录像机回复nack时的处理
  算法实现： 
  全局变量： 
  参    数： pcMsg : osp的CMessage
  返 回 值： RET_TYPE 
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CRecPauseState::OnResumeRecReqNack(CMessage *const pcMsg)
{
	// 如果录像机请求了开始录像,则回复REC_MCU_RESUMEREC_NACK
	if (m_bIsStartRecReq)
	{
		ReplyMtError(REC_MCU_STARTREC_NACK, ERR_REC_ACTFAIL);
	}else
	{
		if(RET_DONE != OnMtReqNack(pcMsg))
		{
			return RET_FAILED;
		}
	}

	// 向录像机请求恢复录像
	return RET_DONE;
}


/*===================================================================
  函 数 名： OnMtStartRecReq
  功    能： CRecPauseState状态时终端请求录像的响应函数
  算法实现： 
  全局变量： 
  参    数： pcMsg : osp的CMessage
  返 回 值： RET_TYPE 
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CRecPauseState::OnMtStartRecReq(CMessage *const pcMsg)
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
	
	// 向录像机请求暂停录像
	m_bIsStartRecReq = TRUE;
	return GetStateMgr()->PostMsgToRecChnInst(MCU_REC_RESUMEREC_REQ);
}