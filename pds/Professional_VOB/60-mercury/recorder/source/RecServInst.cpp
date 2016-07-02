/*****************************************************************************
   模块名      : Recorder
   文件名      : RecServInst.cpp
   相关文件    : RecApp.cpp
   文件实现功能: Recorder Server实现
   作者        : 
   版本        : V1.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2010/08/26  1.0         刘 旭        创建
******************************************************************************/

#include "RecServInst.h"
#include "ConfAgent.h"
//#include "mcuver.h"
#include "ChnInst.h"
#include "StateBase.h"


// 自定义全局变量, 录像机服务器的App定义
CRecServApp g_RecServApp;


/*===================================================================
  函 数 名： CRecServInst
  功    能： CRecServInst的构造函数
  算法实现： 
  全局变量： 
  参    数： 无
  返 回 值： 无
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
CRecServInst::CRecServInst() : m_pcStateMgr(NULL)
{
	m_pcStateMgr = new CStateMgr(this);
	memset(&m_cServMsg, 0, sizeof(m_cServMsg));
	memset(&m_tMTNodeInfo, 0, sizeof(m_tMTNodeInfo));
	memset(&m_tRecNodeInfo, 0, sizeof(m_tRecNodeInfo));
}

/*===================================================================
  函 数 名： ~CRecServInst
  功    能： CRecServInst的析造函数
  算法实现： (1).释放申请资源
  全局变量： 
  参    数： 无
  返 回 值： 无
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
CRecServInst::~CRecServInst()
{
	if (m_pcStateMgr)
	{
		delete m_pcStateMgr;
		m_pcStateMgr = NULL;
	}
}


/*===================================================================
  函 数 名： DaemonInstanceEntry
  功    能： CRecServInst的守护实例入口函数
  算法实现： 
  全局变量： 
  参    数： pcMsg = 消息; pcApp = CApp
  返 回 值： 无
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
void CRecServInst::DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp)
{
    switch (pcMsg->event)
    {
	// 终端注册请求
    case MT_RECSERV_REG_REQ:
        OnMTRegisterReq(pcMsg, pcApp);
        break;

	case REC_MCU_EXCPT_NOTIF:
	case REC_MCU_RECSTATUS_NOTIF:
		OnDaemNodifyEvent(pcMsg, pcApp);
		break;

	case EV_RECSERV_SHOW_STATUS:
		OnDaemShowStatus(pcMsg, pcApp);
		break;

	// 录像机上电消息
	case EV_REC_POWERON:
		break;
     
    default:
        break;
    }
}

/*===================================================================
  函 数 名： InstanceEntry
  功    能： CRecServInst的普通实例入口函数
  算法实现： 
  全局变量： 
  参    数： pcMsg = 消息; pcApp = CApp
  返 回 值： 无
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
void CRecServInst::InstanceEntry(CMessage * const pcMsg)
{
    const u16 wEventId = pcMsg->event;

    switch (wEventId)
    {
	// 终端掉线通知
    case OSP_DISCONNECT:
        OnDisconnectMsg(pcMsg);
        break;

    default:
		if (m_pcStateMgr)
		{
			m_pcStateMgr->OnMessage(pcMsg);
		}
        break;
    }
}

/*===================================================================
  函 数 名： OnMTRegisterReq
  功    能： 终端请求注册时CRecServInst将为之分配一个空闲的instance
  算法实现： 
  全局变量： 
  参    数： pcMsg = 消息; pcApp = CApp
  返 回 值： 无
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
void CRecServInst::OnMTRegisterReq(CMessage * const pMsg, CApp* pcApp)
{
	// 获取一个CRecServInst的空闲实例
    CRecServInst *pInstance = (CRecServInst *)GetIdleInst(pcApp);

	// 获取空闲实例失败
    if (!pInstance)
    {
		RecServLog("RecServer is busy now, refused the register from (%d, %d)\n", pMsg->srcnode, pMsg->srcid);

        // 通知终端服务器正忙
        TErrorInfo tErro;
        tErro.m_wErrCode = ERR_REC_SERVER_BUSY;

        OspPost(pMsg->srcid, 
            RECSERV_MT_REG_NACK,
            &tErro,
            sizeof(tErro),
            pMsg->srcnode
        );

        return;
    }

    // 注册断链
    OspSetHBParam(pMsg->srcnode, 20, 20);
	// 断链的处理方为将要接收此次注册的CRecServInst
    OspNodeDiscCBReg(pMsg->srcnode, pInstance->GetAppID(), pInstance->GetInsID());

	// 状态置换
    pInstance->NextState(E_STATE_REC_ONREG);
	
	// 保存终端节点信息
	TNodeInfo tMtNode = {0};
	tMtNode.m_dwAppId = pMsg->srcid;
	tMtNode.m_dwNodeId = pMsg->srcnode;
    pInstance->SetMTNodeInfo(tMtNode);

	// 将终端注册消息转给具体的CRecServInst来处理
    OspPost(MAKEIID(pInstance->GetAppID(), pInstance->GetInsID()), 
        pMsg->event,
        pMsg->content,
        pMsg->length);
}

/*===================================================================
  函 数 名： OnDaemNodifyEvent
  功    能： 守护实例中转来自录像机的Nodity消息
  算法实现： 
  全局变量： 
  参    数： pcMsg = 消息; pcApp = CApp
  返 回 值： 无
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
void CRecServInst::OnDaemNodifyEvent(CMessage * const pMsg, CApp* pcApp)
{
    CRecServInst* pInstance = NULL;
    for (u16 iCounter = 1; iCounter <= pcApp->GetInstanceNumber(); iCounter++)
    {
        pInstance = (CRecServInst*)pcApp->GetInstance(iCounter);
        ASSERT(pInstance != NULL);
        
        // pInstance的状态时Idle, 且不是专为Recorder注册用的那个instance
        if ( pInstance && E_STATE_REC_IDLE != pInstance->GetState())
        {
			// 将终端注册消息转给具体的CRecServInst来处理
			OspPost(MAKEIID(pInstance->GetAppID(), pInstance->GetInsID()), pMsg->event, pMsg->content, pMsg->length);
        }
    }
}

/*===================================================================
  函 数 名： OnDaemShowStatus
  功    能： 打印录像机服务器的状态信息
  算法实现： 
  全局变量： 
  参    数： pcMsg = 消息; pcApp = CApp
  返 回 值： 无
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
void CRecServInst::OnDaemShowStatus(CMessage * const pMsg, CApp* pcApp)
{
	// 遍历pApp, 找到一个空闲的Instance.
	RecServLog("\n=====================================================================\n");
	RecServLog("\nThe status of CRecServInst are shown below:\n");
    CInstance* pInstance = NULL;
    for (u16 iCounter = 1; pcApp && iCounter <= pcApp->GetInstanceNumber(); iCounter++)
    {
        pInstance = pcApp->GetInstance(iCounter);
        if (!pInstance) break;
        
		// 将终端注册消息转给具体的CRecServInst来处理
		OspPost(MAKEIID(pInstance->GetAppID(), pInstance->GetInsID()), pMsg->event);
    }
}


/*===================================================================
  函 数 名： OnDisconnectMsg
  功    能： 录像机服务器检测到终端掉线时的处理函数
  算法实现： 
  全局变量： 
  参    数： pcMsg
  返 回 值： 无
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
void CRecServInst::OnDisconnectMsg(CMessage * const pcMsg)
{
	// 判断消息内容是否合法
    if ( NULL == pcMsg->content )
    {
        RecServLog("[CRecServInst][Error]Ins%d received error msg\n", GetInsID());
        return;
    }
   
	// 判断消息内容是否正确
    u32 dwNodeId = *(u32*)pcMsg->content;
	if (dwNodeId == m_tMTNodeInfo.m_dwNodeId)
    {
		// 主动断开
        OspDisconnectTcpNode(dwNodeId);
        OspPrintf(TRUE, FALSE, "[CRecServInst][Error]Node%d disconnect, then reset!\n", m_tMTNodeInfo.m_dwNodeId);

		// 通知录像机终止录像/放像，释放通道
        PostMsgToRecChnInst(EV_MT_DISCONNECTED, NULL, 0);

		// 清空m_cServMsg的消息头
        memset(&m_cServMsg, 0, sizeof(m_cServMsg));		
		// 初始化状态
        NextState(E_STATE_REC_IDLE);
    }
}


/*===================================================================
  函 数 名： GetIdleInst
  功    能： 从pcApp中获取一个空闲实例
  算法实现： 
  全局变量： 
  参    数： pcApp
  返 回 值： CInstance*对象
---------------------------------------------------------------------
  修改纪录:
  日   期       版本    修改人    修改内容
  2010/08/26    1.0     刘 旭      创建
====================================================================*/
CRecServInst* CRecServInst::GetIdleInst(CApp* pcApp) 
{
    // 遍历pApp, 找到一个空闲的Instance.
    CRecServInst* pInstance = NULL;
    for (u16 iCounter = 1; iCounter <= pcApp->GetInstanceNumber(); iCounter++)
    {
        pInstance = (CRecServInst *)pcApp->GetInstance(iCounter);
        if(pInstance == NULL) break;
        
        // pInstance的状态时Idle, 且不是专为Recorder注册用的那个instance
        if (E_STATE_REC_IDLE == pInstance->GetState())
        {
             return pInstance;
        }
    }

    return NULL;
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
RET_TYPE CRecServInst::PostMsgToMt(const u16 wEvent, const u8* pContent, const u16 dwLength)
{
	RecServLevelLog(RECSERV_LEVEL_LOG_BELOW, 
		"CRecServInst[%d] post %s to mt[%d]\n", 
		GetInsID(), 
		OspEventDesc(wEvent), 
		m_tMTNodeInfo.m_dwNodeId);

	OspPost(m_tMTNodeInfo.m_dwAppId, 
		wEvent, 
		pContent, 
		dwLength, 
		m_tMTNodeInfo.m_dwNodeId, 
		GETLOCALAPPID);
	
	return RET_DONE;
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
RET_TYPE CRecServInst::PostMsgToRecDaemInst(const u16 wEvent, const u8* pContent, const u16 dwLength)
{
	memset(&m_cServMsg, 0, sizeof(m_cServMsg));
	m_cServMsg.SetMsgBody(const_cast<u8 *>(pContent), dwLength);

	OspPost(MAKEIID(AID_RECORDER, CInstance::DAEMON), 
		wEvent, 
		m_cServMsg.GetServMsg(), 
		m_cServMsg.GetServMsgLen(), 
		m_tRecNodeInfo.m_dwNodeId, 
		GETLOCALAPPID);
	
	return RET_DONE;
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
RET_TYPE CRecServInst::PostMsgToRecChnInst(const u16 wEvent, const u8* pContent, const u16 dwLength)
{
	memset(&m_cServMsg, 0, sizeof(m_cServMsg));
	m_cServMsg.SetMsgBody(const_cast<u8 *>(pContent), dwLength);

	OspPost(m_tRecNodeInfo.m_dwAppId, 
		wEvent, 
		m_cServMsg.GetServMsg(), 
		m_cServMsg.GetServMsgLen(), 
		m_tRecNodeInfo.m_dwNodeId, 
		GETLOCALAPPID);
	
	return RET_DONE;
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
BOOL32 CRecServInst::IsMtReqValid(CMessage * const pcMsg)
{
	// 验证消息来源是否合法
	if (m_tMTNodeInfo.m_dwNodeId != pcMsg->srcnode
		|| m_tMTNodeInfo.m_dwAppId != pcMsg->srcid)
	{
		RecServLog("The Mt[%d, %d] != MyMt[%d, %d], it's invalid!\n", pcMsg->srcnode, pcMsg->srcid, m_tMTNodeInfo.m_dwNodeId, m_tMTNodeInfo.m_dwAppId);
		return FALSE;
	}
	
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
BOOL32 CRecServInst::IsRecMsgValid(CMessage * const pcMsg)
{
	// 如果是Daemon实例的消息, 回复TRUE
	if (pcMsg->srcnode == m_tRecNodeInfo.m_dwNodeId && DAEMON == GETINS(pcMsg->srcid))
	{
		return TRUE;
	}

	// 非Daemon实例消息,需要判断
	if (pcMsg->srcnode != m_tRecNodeInfo.m_dwNodeId || pcMsg->srcid != m_tRecNodeInfo.m_dwAppId)
	{
		RecServLog("The Chanel[%d, %d] != MyChn[%d, %d], it's invalid!\n", pcMsg->srcnode, pcMsg->srcid, m_tRecNodeInfo.m_dwNodeId, m_tRecNodeInfo.m_dwAppId);
		return FALSE;
	}
	
	return TRUE;
}

