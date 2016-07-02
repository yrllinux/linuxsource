/*****************************************************************************
   模块名      : Recorder
   文件名      : StateMgr.cpp
   相关文件    : State.cpp, State.cpp, RecServInst.cpp等
   文件实现功能: Recorder Server的状态机实现
   作者        : 
   版本        : V1.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2010/08/26  1.0         刘 旭        创建
******************************************************************************/

#include "StateMgr.h"
#include "StateBase.h"
#include "State.h"
#include "RecServInst.h"


/*===================================================================
  函 数 名： CStateMgr
  功    能： CStateMgr的构造函数
  算法实现： 
  全局变量： 
  参    数： pInst : CRecServInst的实例指针
  返 回 值：
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
CStateMgr::CStateMgr(CRecServInst * const pInst) : m_pServInst(pInst), m_pcState(NULL)
{
	m_pcState = new CIdleState(this);

	// 判断初始化是否成功
	if (!m_pcState || !m_pServInst)
	{
		RecServLog("[Error]CStateMgr initial failed!\n");
	}
}


/*===================================================================
  函 数 名： CStateMgr
  功    能： CStateMgr的析造函数
  算法实现： 
  全局变量： 
  参    数：
  返 回 值：
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
CStateMgr::~CStateMgr()
{
	if (m_pcState)
	{
		m_pcState->Exit();
		delete m_pcState;
		m_pcState = NULL;
	}
}


/*===================================================================
  函 数 名： CStateMgr
  功    能： CStateMgr的状态切换函数
  算法实现： 
  全局变量： 
  参    数：dwState : 新状态的enum值
  返 回 值：RET_TYPE,请参照RET_TYPE定义
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CStateMgr::ChangeStateTo(const u32 &dwState)
{
    if (m_pcState)
    {
		// 如果要切换的状态是当前状态,直接返回false
        if (m_pcState->GetState() == dwState)
        {
            return RET_PASSED;
        }else			// 否则先退出旧状态
        {
            m_pcState->Exit();
            delete m_pcState;
            m_pcState = NULL;
        }
    }
	
	// 根据状态值动态创建m_pcState
    switch (dwState)
    {
	case E_STATE_REC_IDLE:							// 空闲
		m_pcState = new CIdleState(this);
		break;
	case E_STATE_REC_ONREG:							// 正在准备接受注册
		m_pcState = new COnRegisterState(this);
		break;
	case E_STATE_REC_READY:							// 录像准备好
		m_pcState = new CRecReadyState(this);
		break;
	case E_STATE_RECORDING:							// 正在录像
		m_pcState = new CRecordingState(this);
		break;
	case E_STATE_REC_PAUSE:							// 录像暂停状态
		m_pcState = new CRecPauseState(this);
		break;
    default:
        break;
    }
	
	// 验证m_pcState是否创建成功
    if (!m_pcState)
    {
        RecServLevelLog(RECSERV_LEVEL_LOG_WARNING, "ChangeStateTo to state %d failed, please check the value\n ", dwState);
        return RET_FAILED;
    }
	
	// 状态开始
    m_pcState->Enter();
    return RET_DONE;
}

/*===================================================================
  函 数 名： OnMessage
  功    能： CStateMgr的消息入口函数
  算法实现： 
  全局变量： 
  参    数：dwState : CMessage*
  返 回 值：RET_TYPE,请参照RET_TYPE定义
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CStateMgr::OnMessage(CMessage *const pcMsg)
{
    if (m_pcState)
    {
        return m_pcState->OnMessage(pcMsg);
    }
    
    RecServLevelLog(RECSERV_LEVEL_LOG_ERROR, "CStateMgr hasn't been initial success!\n");
    return RET_FAILED;
}

/*===================================================================
  函 数 名： PostMsgToMt
  功    能： 将消息发给已经注册的终端
  算法实现： 
  全局变量： 
  参    数： 
			wEvent   : 事件号
			pContent : 事件内容
			dwLength : 内容长度
  返 回 值： RET_TYPE,请参照RET_TYPE
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CStateMgr::PostMsgToMt(const u16 wEvent, const u8* pContent, const u16 dwLength)
{
	if (GetOwnInst())
	{
		return GetOwnInst()->PostMsgToMt(wEvent, pContent, dwLength);
	}

	// instance为空,显示错误
	RecServLevelLog(RECSERV_LEVEL_LOG_ERROR, "CStateMgr::m_pServInst is Null, Initialize failed!\n");
	return RET_FAILED;
}

/*===================================================================
  函 数 名： PostMsgToRecDaemInst
  功    能： 将消息发给录像机的守护实例
  算法实现： 
  全局变量： 
  参    数： 
			wEvent   : 事件号
			pContent : 事件内容
			dwLength : 内容长度
  返 回 值： RET_TYPE,请参照RET_TYPE
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CStateMgr::PostMsgToRecDaemInst(const u16 wEvent, const u8* pContent, const u16 dwLength)
{
	if (GetOwnInst())
	{
		return GetOwnInst()->PostMsgToRecDaemInst(wEvent, pContent, dwLength);
	}

	// instance为空,显示错误	
	RecServLevelLog(RECSERV_LEVEL_LOG_ERROR, "CStateMgr::m_pServInst is Null, Initialize failed!\n");
	return RET_FAILED;
}

/*===================================================================
  函 数 名： PostMsgToRecChnInst
  功    能： 将消息发给录像机的录像通道
  条    件:  此通道正在录像
  算法实现： 
  全局变量： 
  参    数： 
			wEvent   : 事件号
			pContent : 事件内容
			dwLength : 内容长度
  返 回 值： RET_TYPE,请参照RET_TYPE
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
RET_TYPE CStateMgr::PostMsgToRecChnInst(const u16 wEvent, const u8* pContent, const u16 dwLength)
{
	if (GetOwnInst())
	{
		return GetOwnInst()->PostMsgToRecChnInst(wEvent, pContent, dwLength);
	}
	
	// instance为空,显示错误
	RecServLevelLog(RECSERV_LEVEL_LOG_ERROR, "CStateMgr::m_pServInst is Null, Initialize failed!\n");
	return RET_FAILED;
}

/*===================================================================
  函 数 名： IsMtReqValid
  功    能： 验证终端的请求是否合法
  条    件:  此终端已经注册
  算法实现： (1). 如果消息源与已注册终端信息不符合,则此消息为非法消息
  全局变量： 
  参    数： pcMsg   : CMessage
  返 回 值： 合法返回True,否则False
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
BOOL32 CStateMgr::IsMtReqValid(CMessage * const pcMsg)
{
	if (GetOwnInst())
	{
		return GetOwnInst()->IsMtReqValid(pcMsg);
	}
	
	// instance为空,显示错误
	RecServLevelLog(RECSERV_LEVEL_LOG_ERROR, "CStateMgr::m_pServInst is Null, Initialize failed!\n");
	return TRUE;
}

/*===================================================================
  函 数 名： IsRecMsgValid
  功    能： 验证录像机的消息是否合法
  算法实现： (1). 如果消息来自于录像机的守护实例,则合法
			 (2). 如果非守护实例,则需要验证此消息源是否与已经记录的录像通道信息相符合
  全局变量： 
  参    数： pcMsg   : CMessage
  返 回 值： 合法返回True,否则False
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
BOOL32 CStateMgr::IsRecMsgValid(CMessage * const pcMsg)
{
	if (GetOwnInst())
	{
		return GetOwnInst()->IsRecMsgValid(pcMsg);
	}
	
	// instance为空,显示错误	
	RecServLevelLog(RECSERV_LEVEL_LOG_ERROR, "CStateMgr::m_pServInst is Null, Initialize failed!\n");
	return TRUE;
}

/** 
/*	功能：获取状态机当前状态
/*  参数：
/*  返回值：当前状态的enum值
*/
const u32 CStateMgr::GetState()
{
	if (m_pcState)
	{
		return m_pcState->GetState();
	}else
	{
		return E_STATE_REC_INVALID;
	}
}

/** 
/*	功能：存储录像机节点信息
/*  参数：
/*       dwNodeId : 节点号
/*       dwInstId : mt的Instance号，由MAKEID(appid，instanceid)生成
/*  返回值：无
*/
void CStateMgr::SetRecNodeInfo(const TNodeInfo& tNode)
{
	if (GetOwnInst())
	{
		GetOwnInst()->SetRecNodeInfo(tNode);
		return;
	}

	// instance为空,显示错误	
	RecServLevelLog(RECSERV_LEVEL_LOG_ERROR, "CStateMgr::m_pServInst is Null, Initialize failed!\n");
}

/** 
/*	功能：存储终端节点信息
/*  参数：
/*       dwNodeId : 节点号
/*       dwInstId : mt的Instance号，由MAKEID(appid，instanceid)生成
/*  返回值：无
*/
void CStateMgr::SetMTNodeInfo(const TNodeInfo& tNode)
{
	if (GetOwnInst())
	{
		GetOwnInst()->SetMTNodeInfo(tNode);
		return;
	}

	// instance为空,显示错误	
	RecServLevelLog(RECSERV_LEVEL_LOG_ERROR, "CStateMgr::m_pServInst is Null, Initialize failed!\n");
}
