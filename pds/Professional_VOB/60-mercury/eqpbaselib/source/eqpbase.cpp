/*****************************************************************************
   模块名      : eqpbase
   文件名      : eqpbase.cpp
   相关文件    : 
   文件实现功能: 外设基类实现
   作者        :  周嘉麟
   版本        :  1.0
   日期		   :  2012/02/14
-----------------------------------------------------------------------------
******************************************************************************/
#include "eqpbase.h"
#include "boardagent.h"
#include "kdvmedianet.h"

#ifndef WIN32
	#include "mcuver.h"
#endif



/*====================================================================
函数名      : StrOfIP
功能        ：IP地址转换
算法实现    ：
引用全局变量：
输入参数说明：u32 dwIP	
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
char * StrOfIP( u32 dwIP )
{
    dwIP = htonl(dwIP);
    static char strIP[17];  
    u8 *p = (u8 *)&dwIP;
    sprintf(strIP,"%d.%d.%d.%d%c",p[0],p[1],p[2],p[3],0);
    return strIP;
}
/*====================================================================
函数名      : CChnnlStatusBase
功能        ：CChnnlStatusBase构造
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
CChnnlStatusBase::CChnnlStatusBase():m_byState(EQP_CHNNL_STATUS_INVALID),
									 m_byMediaMode(MODE_NONE),
									 m_wChnIdx(INVALID_CHNLIDX)
{

}

/*====================================================================
函数名      : CChnnlStatusBase
功能        ：CChnnlStatusBase析构
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
CChnnlStatusBase::~CChnnlStatusBase()
{

}

/*====================================================================
函数名      : SetState
功能        ：设置通道状态机
算法实现    ：
引用全局变量：
输入参数说明：const u8 byState 通道状态机
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CChnnlStatusBase::SetState(const u8 byState)
{
	m_byState = byState;
	return TRUE;
}

/*====================================================================
函数名      : GetState
功能        ：获取通道状态机
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：u8 m_byState 通道状态机
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
u8 CChnnlStatusBase::GetState()const
{
	return m_byState;
}

/*====================================================================
函数名      : SetMediaMode
功能        ：设置媒体模式(音视频)
算法实现    ：
引用全局变量：
输入参数说明：const u8 byMediaMode 媒体模式
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CChnnlStatusBase::SetMediaMode(const u8 byMediaMode)
{
	if (MODE_AUDIO != byMediaMode && MODE_VIDEO != byMediaMode && MODE_BOTH != byMediaMode)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, 
								"[CChnnlStatusBase::SetMediaMode] unexpected mode.%d!\n", byMediaMode);
		return;
	}
	m_byMediaMode = byMediaMode;
}

/*====================================================================
函数名      : GetMediaMode
功能        ：获取通道媒体模式
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：u8 m_byMediaMode 媒体模式
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
u8 CChnnlStatusBase::GetMediaMode()const
{
	return m_byMediaMode;
}

/*====================================================================
函数名      : SetChnIdx
功能        ：设置通道索引
算法实现    ：
引用全局变量：
输入参数说明：const u8 wChnIdx 通道索引
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CChnnlStatusBase::SetChnIdx(const u16 wChnIdx)
{
	m_wChnIdx = wChnIdx;
}

/*====================================================================
函数名      : GetChnIdx
功能        ：获取通道索引
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：u16 m_wChnIdx 媒体模式
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
u16 CChnnlStatusBase::GetChnIdx()const
{
	return m_wChnIdx;
}

/*====================================================================
函数名      : CEqpChnnlBase
功能        ：CEqpChnnlBase构造
算法实现    ：
引用全局变量：
输入参数说明：const CEqpBase*const pcEqp			绑定外设
			  CChnnlStatusBase*const pcChnlStatus   实例化通道状态
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
CEqpChnnlBase::CEqpChnnlBase(const CEqpBase*const pcEqp, CChnnlStatusBase*const pcChnlStatus)
	:m_pcEqp(pcEqp), m_pcChnlState(pcChnlStatus)
{

};

/*====================================================================
函数名      : ~CEqpChnnlBase
功能        ：CEqpChnnlBase析构
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
/*lint -save -e1551*/
CEqpChnnlBase::~CEqpChnnlBase()
{

};
/*lint -restore*/
/*====================================================================
函数名      : Init
功能        ：通道基类初始化
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpChnnlBase::Init()
{
	return TRUE;
}

/*====================================================================
函数名      : Destroy
功能        ：销毁
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpChnnlBase::Destroy()
{
	return TRUE;
};

/*====================================================================
函数名      : Stop
功能        : 停止通道工作, 协议接口
算法实现    ：
引用全局变量：
输入参数说明：const u8 byMode   媒体模式(音视频)
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpChnnlBase::Stop(const u8 byMode /* = MODE_BOTH */)
{
	LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP,  "[CEqpChnnlBase::Stop] mode.%d!\n", byMode);
	return TRUE;
}

/*====================================================================
函数名      : Start
功能        : 停止通道工作，协议接口
算法实现    ：
引用全局变量：
输入参数说明：const u8 byMode 媒体模式(音视频)
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpChnnlBase::Start(const u8 byMode /* = MODE_BOTH */)
{
	LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP,  "[CEqpChnnlBase::Start] mode.%d!\n", byMode);
	return TRUE;
}
/*====================================================================
函数名      : OnMessage
功能        ：通道消息入口
算法实现    ：
引用全局变量：
输入参数说明：CMessage* const pcMsg
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpChnnlBase::OnMessage(CMessage* const pcMsg)
{
	if (NULL == pcMsg)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP,  "[CEqpChnnlBase::OnMessage] pcMsg is null!\n");
		return FALSE;
	}
	return TRUE;
}

/*====================================================================
函数名      : PostMsgToMcu
功能        ：发送消息给MCU
算法实现    ：
引用全局变量：
输入参数说明：CServMsg& cMsg  消息体
			  BOOL32 bMcuA    是否发给MCUA	
			  BOOL32 bMcuB    是否发给MCUB
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpChnnlBase::PostMsgToMcu(CServMsg& cMsg, BOOL32 bMcuA /* = TRUE */, BOOL32 bMcuB /* = TRUE */)
{
	if (NULL == m_pcEqp)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpChnnlBase::PostMsgToMcu] m_pcEqp is null!\n");
		return FALSE;
	}

	BOOL32 bRet = FALSE;
	if (bMcuA)
	{
		bRet  = m_pcEqp->PostMsgToMcu(cMsg);
	}
	
	if (bMcuB)
	{
		bRet &= m_pcEqp->PostMsgToMcu(cMsg, FALSE);
	}

	return bRet;
}

/*====================================================================
函数名      : PostMsgToServer
功能        ：发送消息给服务器
算法实现    ：
引用全局变量：
输入参数说明：CServMsg& cMsg
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpChnnlBase::PostMsgToServer(CServMsg& cMsg,const BOOL32 bSendHeadInfo /*= TRUE*/)
{
	if (NULL == m_pcEqp)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpChnnlBase::PostMsgToServer] m_pcEqp is null!\n");
		return FALSE;
	}
	return m_pcEqp->PostMsgToServer(cMsg,bSendHeadInfo);
}

/*====================================================================
函数名      : SetChnIdx
功能        ：绑定通道索引
算法实现    ：
引用全局变量：
输入参数说明：const u16 wChnIdx 通道索引
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CEqpChnnlBase::SetChnIdx(const u16 wChnIdx)
{
	if( !m_pcEqp || m_pcEqp->GetChnNum() <= wChnIdx )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpChnnlBase]SetChnIdx failed, m_pcEqp is null or wIdx.%d invalid\n", wChnIdx);
		return;
	}
	
	if (m_pcChnlState)
	{
		m_pcChnlState->SetChnIdx(wChnIdx);
	}else
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpChnnlBase]SetChnIdx failed, m_pcChnlState is null\n");
	}
}

/*====================================================================
函数名      : GetChnIdx
功能        ：绑定通道索引
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：u16 通道索引
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
u16 CEqpChnnlBase::GetChnIdx()const
{
	if (NULL == m_pcChnlState)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpChnnlBase]GetChnIdx failed, m_pcChnlState is null\n");
		return INVALID_CHNLIDX;
	}

	return m_pcChnlState->GetChnIdx();
}

/*====================================================================
函数名      : SetState
功能        ：设置通道实状态
算法实现    ：
引用全局变量：
输入参数说明：const u8 byState 通道状态
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpChnnlBase::SetState(const u8 byState)
{
    if (NULL == m_pcChnlState)
    {
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpChnnlBase::SetState] m_pcChnlState is null!\n");
		return FALSE;
    }
	return m_pcChnlState->SetState(byState);
}

/*====================================================================
函数名      : GetState
功能        ：获取通道实状态
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：u8			   通道状态
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
u8 CEqpChnnlBase::GetState()const
{
	if (NULL == m_pcChnlState)
    {
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpChnnlBase::GetState] m_pcChnlState is null!\n");
		return EQP_CHNNL_STATUS_INVALID;
    }
	return m_pcChnlState->GetState();
}

/*====================================================================
函数名      : GetInsID
功能        ：获取实例号
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：u8	通道状态
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
u16 CEqpChnnlBase::GetInsID()const
{
	if (NULL == m_pcEqp)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpChnnlBase::GetInsID] m_pcEqp is null!\n");
		return INVALID_INS;
	}
	return m_pcEqp->GetInsID();
}

/*====================================================================
函数名      : SetTimer
功能        ：设置定时器
算法实现    ：
引用全局变量：
输入参数说明：const u32 dwTimerId     事件号
			  const u32 dwMillionSecs 定时间隔
			  const u32 dwParam       携带参数
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CEqpChnnlBase::SetTimer(const u32 dwTimerId, const u32 dwMillionSecs, const u32 dwParam /* = 0 */ )
{
	if (NULL == m_pcEqp)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpChnnlBase::SetTimer] m_pcEqp is null!\n");
		return;
	}
	
	m_pcEqp->SetTimer(dwTimerId, dwMillionSecs, dwParam);
}

/*====================================================================
函数名      : KillTimer
功能        ：取消定时器
算法实现    ：
引用全局变量：
输入参数说明：u32 dwTimerId     事件号
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CEqpChnnlBase::KillTimer(u32 dwTimerId)
{
	if (NULL == m_pcEqp)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpChnnlBase::KillTimer] m_pcEqp is null!\n");
		return;
	}
	m_pcEqp->KillTimer(dwTimerId);
}
/*====================================================================
函数名      : CEqpBase
功能        ：CEqpBase构造
算法实现    ：
引用全局变量：
输入参数说明：CEqpMsgSender*const pcMsgSender 消息发送对象绑定
              CEqpCfg*const       pcCfg       配置绑定
	
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
CEqpBase::CEqpBase(CEqpMsgSender*const pcMsgSender, CEqpCfg*const pcCfg)
	:m_pcMsgSender(pcMsgSender), 
	 m_pcEqpCfg(pcCfg),
	 m_wChnNum(0),
	 m_ppcChnl(NULL),
	 m_eEqpStat(E_EQP_INVALID) 
{
}

/*====================================================================
函数名      : CEqpBase
功能        ：CEqpBase析构
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
/*lint -save -e1551*/
/*lint -save -e1579*/
CEqpBase::~CEqpBase()
{
	CEqpBase::Destroy();
}
/*lint -restore*/
/*lint -restore*/
/*====================================================================
函数名      : Init
功能		: 外设基本初始化
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpBase::Init()
{
	for ( u16 wLoop = 0; wLoop < m_wChnNum; wLoop++ )
	{
		if (NULL == m_ppcChnl)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::Init] m_ppcChnl is null!\n");
			return FALSE;	
		}

		if (NULL == m_ppcChnl[wLoop])
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::Init] m_ppcChnl[%d] is null!\n", wLoop);
			return FALSE;	
		}

		m_ppcChnl[wLoop]->Init();
	}
	
	SetEqpState(E_EQP_OFFLINE); 
	return TRUE;
}

/*====================================================================
函数名      : Destroy
功能        ：销毁
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpBase::Destroy()
{
	DestroyChn();
	DestroyCfg();
	SetEqpState(E_EQP_INVALID); 	
	return TRUE;
}

/*====================================================================
函数名      : DestroyChn
功能        ：销毁通道
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpBase::DestroyChn()
{
	//销毁子类申请的空间
	for ( u16 wLoop = 0; wLoop < m_wChnNum; wLoop++ )
	{
		if (m_ppcChnl && m_ppcChnl[wLoop])
		{
			m_ppcChnl[wLoop]->Destroy();
		}
	}
	
	//销毁通道指针
	if (NULL != m_ppcChnl)
	{
		delete []m_ppcChnl;
		m_ppcChnl = NULL;
	}

	m_wChnNum = 0;

	return TRUE;
}

/*====================================================================
函数名      : DestroyCfg
功能        ：销毁配置
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpBase::DestroyCfg()
{
	//销毁配置
	if(NULL == m_pcEqpCfg)
	{
		delete m_pcEqpCfg;
		m_pcEqpCfg = NULL;
	}	
	return TRUE;
}

/*====================================================================
函数名      : OnMessage
功能        ：外设基类消息处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage*const pcMsg
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpBase::OnMessage(CMessage*const pcMsg)
{
	if (NULL == pcMsg)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::OnMessage] pcMsg is null!\n");
		return FALSE;
	}

	LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[CEqpBase::OnMessage]:Recv msg %u(%s).\n", 
							pcMsg->event, ::OspEventDesc(pcMsg->event));

	switch (pcMsg->event)
	{
	case EV_EQP_CONNECT_TIMER:
    case EV_EQP_CONNECTB_TIMER:
		OnConnectMcuTimer(pcMsg);
		 break;	

	case EV_EQP_REGISTER_TIMER:
	case EV_EQP_REGISTERB_TIMER:
		OnRegisterMcuTimer(pcMsg);
		 break;

	case MCU_EQP_QOS_CMD:
		OnSetQosCmd(pcMsg);
		 break;

	case OSP_DISCONNECT:
		OnDisConnect(pcMsg);
		 break;

	case MCU_EQP_GETMSSTAT_ACK:
	case EV_EQP_GETMSSTATUS_TIMER:
		OnGetMsStatusAck(pcMsg);
		 break;

	case EV_EQPSERVER_CONNECT_TIMER:
		OnConnectServerTimer();
		break;

	case EV_EQPSERVER_REGISTER_TIMER:
		OnRegisterServerTimer();
		break;

	case SERVER_EQP_REGISTER_ACK:
		OnRegisterServerAck(pcMsg);
		break;

	case SERVER_EQP_REGISTER_NACK:
		OnRegisterServerNack(pcMsg);
		break;

	default:
		{
			CServMsg cServMsg(pcMsg->content, pcMsg->length);
			if (cServMsg.GetChnIndex() >= m_wChnNum )
			{
				LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::OnMessage] ChnIndex.%d is larger than ChnNum.%d!\n", 
					cServMsg.GetChnIndex(), m_wChnNum);
				return FALSE;
			}

			if (NULL == m_ppcChnl || NULL == m_ppcChnl[cServMsg.GetChnIndex()])
			{
				LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::OnMessage] m_ppcChnl is null or m_ppcChnl[%d]!\n", cServMsg.GetChnIndex());
				return FALSE;
			}

			m_ppcChnl[cServMsg.GetChnIndex()]->OnMessage(pcMsg);
		}
		break;
	}
	return TRUE;
}

/*====================================================================
函数名      : OnConnectMcuTimer
功能        ：外设连接处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage*const pcMsg
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpBase::OnConnectMcuTimer(CMessage* const pcMsg)
{	
	if (NULL == pcMsg)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::OnConnectMcuTimer] pcMsg is null!\n");
		return FALSE;
	}
		
	if (NULL == pcMsg->content)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::OnConnectMcuTimer] pcMsg->content is null!\n");
		return FALSE;
	}
	
	if (NULL == m_pcEqpCfg)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::OnConnectMcuTimer] pcCfg is null!\n");
		return FALSE;
	}

	//检测节点A
	if (FALSE == IsValidTcpNode(m_pcEqpCfg->m_dwMcuNodeA))
	{
		m_pcEqpCfg->m_dwMcuNodeA = BrdGetDstMcuNode();
	}
	//检测节点B
	if (FALSE == IsValidTcpNode(m_pcEqpCfg->m_dwMcuNodeB))
	{
		m_pcEqpCfg->m_dwMcuNodeB = BrdGetDstMcuNodeB();
	}

	BOOL32 bIsConnectA = TRUE;

	//连接McuA或者McuB
	bIsConnectA = *(u32*)pcMsg->content ? FALSE : TRUE;

	u32 dwConnectNode = INVALID_NODE;
	if (bIsConnectA)
	{
		dwConnectNode = m_pcEqpCfg->m_dwMcuNodeA;
	}
	else
	{
		dwConnectNode = m_pcEqpCfg->m_dwMcuNodeB;
	}
	
	BOOL32 bMsgSenderNull = NULL == m_pcMsgSender? TRUE : FALSE; 

	if (dwConnectNode == INVALID_NODE || NULL == m_pcMsgSender
			|| !m_pcMsgSender->IsValidTcpNode(dwConnectNode) || OSP_OK != m_pcMsgSender->DisConRegister(dwConnectNode))
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_EQP, "[CEqpBase::OnConnectMcuTimer]Connect Mcu %s failed, node: %d, pSend is null[%d]!\n", 
												bIsConnectA ? "A" : "B", dwConnectNode, bMsgSenderNull);
		
		if (bIsConnectA)
		{
			SetTimer(EV_EQP_CONNECT_TIMER,  EQP_CONNECT_TIMEOUT, 0);
		}
		else
		{
			SetTimer(EV_EQP_CONNECTB_TIMER, EQP_CONNECT_TIMEOUT, 1);
		}	
		return FALSE;
	}
	
	if (bIsConnectA)
	{
		SetTimer(EV_EQP_REGISTER_TIMER, EQP_REGISTER_TIMEOUT, 0 );
	}
	else
	{
		SetTimer(EV_EQP_REGISTERB_TIMER, EQP_REGISTER_TIMEOUT, 1 );
	}
	
	return TRUE;
}


/*====================================================================
函数名      : OnRegisterMcuTimer
功能        ：外设注册处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage*const pcMsg
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpBase::OnRegisterMcuTimer(CMessage* const pcMsg)
{
	if (NULL == pcMsg)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::OnRegisterMcuTimer] pcMsg is null!\n");
		return FALSE;
	}

	if (NULL == pcMsg->content)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::OnRegisterMcuTimer] pcMsg->content is null!\n");
		return FALSE;
	}
	
	if (NULL == m_pcEqpCfg)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::OnRegisterMcuTimer] m_pcEqpCfg is null!\n");
		return FALSE;
	}

	TPeriEqpRegReq tReg;
	
	tReg.SetMcuEqp((u8)m_pcEqpCfg->wMcuId, (u8)m_pcEqpCfg->byEqpId, (u8)m_pcEqpCfg->byEqpType);
    tReg.SetPeriEqpIpAddr(m_pcEqpCfg->dwLocalIP);
	tReg.SetChnnlNum((u8)GetChnNum());
	tReg.SetVersion(m_pcEqpCfg->m_wEqpVer);
	tReg.SetStartPort(m_pcEqpCfg->wRcvStartPort);
    tReg.SetHDEqp(IsHdEqp());
	
	CServMsg  CMsg;
    CMsg.SetMsgBody((u8*)&tReg, sizeof(tReg));
	CMsg.SetEventId(EQP_MCU_REG_REQ);
	
	BOOL32 bMcuA = *(u32*)pcMsg->content ? FALSE : TRUE; 

    if(bMcuA)
    {
		PostMsgToMcu(CMsg, TRUE, TRUE);
        SetTimer(EV_EQP_REGISTER_TIMER, EQP_REGISTER_TIMEOUT, 0);
    }
    else
    {
		PostMsgToMcu(CMsg, FALSE, TRUE);
		SetTimer(EV_EQP_REGISTERB_TIMER, EQP_REGISTER_TIMEOUT, 1);
    }
	
	return TRUE;
}


/*====================================================================
函数名      : OnRegisterMcuAck
功能        ：注册Mcu成功处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage*const pcMsg
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpBase::OnRegisterMcuAck(CMessage* const pcMsg)
{
	if (NULL == pcMsg)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::OnRegisterMcuAck][error] POINTER.pcMsg Is NULL!\n");
		return FALSE;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	TPeriEqpRegAck *ptRegAck = NULL;
	ptRegAck = (TPeriEqpRegAck*)cServMsg.GetMsgBody();
	
	if (NULL == ptRegAck)
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[CEqpBase::OnRegisterMcuAck][error] CONTENT.RegAck Is NULL!\n");
		return FALSE;
	}

	if (NULL == m_pcEqpCfg)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::OnRegisterMcuAck][error] POINTER.m_pcEqpCfg Is NULL!\n");
		return FALSE;
	}

	if (m_pcEqpCfg->m_dwMcuNodeA == pcMsg->srcnode)
	{
		m_pcEqpCfg->m_dwMcuIIdA = pcMsg->srcid;
		m_pcEqpCfg->m_byRegAckNum ++;

		KillTimer(EV_EQP_REGISTER_TIMER);                
        LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[CEqpBase::OnRegisterMcuAck] Register Success To MCUA\n");
	}
	else if (m_pcEqpCfg->m_dwMcuNodeB == pcMsg->srcnode)
	{
		m_pcEqpCfg->m_dwMcuIIdB = pcMsg->srcid;
		m_pcEqpCfg->m_byRegAckNum ++;

		KillTimer(EV_EQP_REGISTERB_TIMER);                
        LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[CEqpBase::OnRegisterMcuAck] Register Success To MCUB\n");
	}

	if ( m_pcEqpCfg->m_dwMpcSSrc == 0 )
    {
        m_pcEqpCfg->m_dwMpcSSrc = ptRegAck->GetMSSsrc();
    }
    else
    {
		if (NULL == m_pcMsgSender) 
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_EQP,"[CEqpBase::OnRegisterMcuAck]NULL == m_pcMsgSender!\n");
			return FALSE;
		}
        if (m_pcEqpCfg->m_dwMpcSSrc != ptRegAck->GetMSSsrc())
        {
            LogPrint(LOG_LVL_ERROR, MID_MCU_EQP,"[CEqpBase::OnRegisterMcuAck] MPC SSRC ERROR(%u<-->%u), Disconnect Both Nodes!\n", 
												 m_pcEqpCfg->m_dwMpcSSrc, ptRegAck->GetMSSsrc());
			
            if ( m_pcMsgSender->IsValidTcpNode(m_pcEqpCfg->m_dwMcuNodeA) )
            {
                m_pcMsgSender->DisconnectTcpNode(m_pcEqpCfg->m_dwMcuNodeA);
            }
            if ( m_pcMsgSender->IsValidTcpNode(m_pcEqpCfg->m_dwMcuNodeB) )
            {
                m_pcMsgSender->DisconnectTcpNode(m_pcEqpCfg->m_dwMcuNodeB);
            }
			return FALSE;
        }
    }

	if (FIRST_REGACK == m_pcEqpCfg->m_byRegAckNum)
	{
		m_pcEqpCfg->m_wMcuRcvStartPort = ptRegAck->GetMcuStartPort();
		m_pcEqpCfg->m_dwMcuRcvIp       = ptRegAck->GetMcuIpAddr();

		u32 dwOtherMcuIp = htonl( ptRegAck->GetAnotherMpcIp() );
        if ( m_pcEqpCfg->dwConnectIpB == 0 && dwOtherMcuIp != 0 )
        {
            m_pcEqpCfg->dwConnectIpB = dwOtherMcuIp;
            SetTimer( EV_EQP_CONNECTB_TIMER, EQP_CONNECT_TIMEOUT);
			
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[CEqpBase::OnRegisterMcuAck] Find another Mpc Ip:%s, try connecting...\n",
													  StrOfIP(dwOtherMcuIp));
        }

		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[CEqpBase::OnRegisterMcuAck] MPC rcvIP[%s], rcvPORT[%d]\n",
												  StrOfIP(m_pcEqpCfg->m_dwMcuRcvIp), m_pcEqpCfg->m_wMcuRcvStartPort);
		
		TPrsTimeSpan *ptPrsTime = NULL;
		ptPrsTime = (TPrsTimeSpan*)(cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck));
		m_pcEqpCfg->m_tPrsTimeSpan = *ptPrsTime;

		LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[CEqpBase::OnRegisterMcuAck] PRS span: FIRST[%d], SECOND[%d], THREE[%d], REJECT[%d]\n", 
													m_pcEqpCfg->m_tPrsTimeSpan.m_wFirstTimeSpan,
													m_pcEqpCfg->m_tPrsTimeSpan.m_wSecondTimeSpan,
													m_pcEqpCfg->m_tPrsTimeSpan.m_wThirdTimeSpan,
													m_pcEqpCfg->m_tPrsTimeSpan.m_wRejectTimeSpan );
	}

	SetEqpState(E_EQP_ONLINE);	
	return TRUE;
}

/*====================================================================
函数名      : OnRegisterMcuNack
功能        ：注册Mcu失败处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage*const pcMsg
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpBase::OnRegisterMcuNack(CMessage* const pcMsg)
{	
	if (NULL == m_pcEqpCfg)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::OnRegisterMcuNack] m_pcEqpCfg is null!\n");
		return FALSE;
	}

	if ( pcMsg->srcnode == m_pcEqpCfg->m_dwMcuNodeA)    
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::OnRegisterMcuNack] EqpIns.%d register be refused by McuA!\n", GetInsID());
    }
    if ( pcMsg->srcnode == m_pcEqpCfg->m_dwMcuNodeB)    
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::OnRegisterMcuNack] EqpIns.%d register be refused by McuB!\n", GetInsID());
    }
	return TRUE;
}

/*====================================================================
函数名      : OnDisConnect
功能        ：断链处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage*const pcMsg
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpBase::OnDisConnect(CMessage* const pcMsg)
{
	u32 dwNode = *(u32*)pcMsg->content;
	if (INVALID_NODE != dwNode)
    {
		DisconnectTcpNode(dwNode);
    }
	
	if (NULL == m_pcEqpCfg)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::OnDisConnect] EqpIns.%d m_pcEqpCfg is null!\n", GetInsID());
		return FALSE;
	}

	if(dwNode == m_pcEqpCfg->m_dwMcuNodeA)
	{
		m_pcEqpCfg->m_dwMcuNodeA = INVALID_NODE;
		m_pcEqpCfg->m_dwMcuIIdA  = INVALID_INS;
		SetTimer(EV_EQP_CONNECT_TIMER, EQP_CONNECT_TIMEOUT, 0);
	}
	else if (dwNode == m_pcEqpCfg->m_dwMcuNodeB)
	{
		m_pcEqpCfg->m_dwMcuNodeB = INVALID_NODE;
		m_pcEqpCfg->m_dwMcuIIdB  = INVALID_INS;
		SetTimer(EV_EQP_CONNECTB_TIMER, EQP_CONNECT_TIMEOUT, 1);
	}
	else if (dwNode == m_pcEqpCfg->m_dwServerNode)
	{
		m_pcEqpCfg->m_dwServerNode = INVALID_NODE;
		m_pcEqpCfg->m_dwServerIId  = INVALID_INS;
		SetTimer(EV_EQPSERVER_CONNECT_TIMER, EQP_CONNECT_TIMEOUT);
	}

	if (INVALID_NODE != m_pcEqpCfg->m_dwMcuNodeA || INVALID_NODE != m_pcEqpCfg->m_dwMcuNodeB)
    {
        if (IsValidTcpNode(m_pcEqpCfg->m_dwMcuNodeA))
        {
			CServMsg cMsg;
			cMsg.SetEventId(EQP_MCU_GETMSSTAT_REQ);
			PostMsgToMcu(cMsg, TRUE, FALSE);
            LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[CEqpBase::OnDisConnect] EqpIns.%d GetMsStatusReq. McuNode.A\n", GetInsID());
        }
        else if (IsValidTcpNode(m_pcEqpCfg->m_dwMcuNodeB))
        {
            CServMsg cMsg;
			cMsg.SetEventId(EQP_MCU_GETMSSTAT_REQ);
			PostMsgToMcu(cMsg, FALSE, TRUE);
            LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[CEqpBase::OnDisConnect] EqpIns.%d GetMsStatusReq. McuNode.B\n", GetInsID());
        }
        SetTimer(EV_EQP_GETMSSTATUS_TIMER, EQP_GETMSSTATUS_TIMEOUT);
    }

	if (INVALID_NODE == m_pcEqpCfg->m_dwMcuNodeA 
			&& INVALID_NODE == m_pcEqpCfg->m_dwMcuNodeB 
			&& INVALID_NODE == m_pcEqpCfg->m_dwServerNode)
	{
		for (u16 wLoop = 0; wLoop < GetChnNum() && m_ppcChnl && m_ppcChnl[wLoop]; wLoop++ )
		{
			m_ppcChnl[wLoop]->Stop();
		}

		m_pcEqpCfg->m_dwMpcSSrc   = 0;
		m_pcEqpCfg->m_byRegAckNum = 0;
		SetEqpState(E_EQP_OFFLINE);

		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[CEqpBase::OnDisConnect] change eqp State to offline!\n");
	}
	return TRUE;
}

/*====================================================================
函数名      : OnGetMsStatusAck
功能        ： 主备状态获取成功处理 
	           主备状态获取超时处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage*const pcMsg
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpBase::OnGetMsStatusAck(CMessage* const pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
	TMcuMsStatus *ptMsStatus = (TMcuMsStatus *)cServMsg.GetMsgBody();
    if( MCU_EQP_GETMSSTAT_ACK == pcMsg->event )
    {      
        KillTimer(EV_EQP_GETMSSTATUS_TIMER);
        LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[CEqpBase::OnGetMsStatusAck] EqpIns.%d receive msg MCU_EQP_GETMSSTAT_ACK. IsMsSwitchOK :%d\n", 
												  GetInsID(), ptMsStatus->IsMsSwitchOK());
    }

	if (!ptMsStatus->IsMsSwitchOK())
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[CEqpBase::OnGetMsStatusAck] EqpIns.%d !ptMsStatus->IsMsSwitchOK()!\n", GetInsID());	
	
		for (u16 wLoop = 0; wLoop < GetChnNum() && m_ppcChnl && m_ppcChnl[wLoop]; wLoop++ )
		{
			m_ppcChnl[wLoop]->Stop();
		}

		if (NULL == m_pcEqpCfg)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::OnGetMsStatusAck] EqpIns.%d m_pcEqpCfg is null!\n", GetInsID());
			return FALSE;
		}

		m_pcEqpCfg->m_dwMpcSSrc   = 0;
		m_pcEqpCfg->m_byRegAckNum = 0;

		if (IsValidTcpNode(m_pcEqpCfg->m_dwMcuNodeA))
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP,  "[CEqpBase::OnGetMsStatusAck] EqpIns.%d DisconnectTcpNode A!\n", GetInsID());	
			DisconnectTcpNode(m_pcEqpCfg->m_dwMcuNodeA);
		}

		if (IsValidTcpNode(m_pcEqpCfg->m_dwMcuNodeB))
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP,  "[CEqpBase::OnGetMsStatusAck] EqpIns.%d DisconnectTcpNode B!\n", GetInsID());	
			DisconnectTcpNode(m_pcEqpCfg->m_dwMcuNodeB);
		}

		if( INVALID_NODE == m_pcEqpCfg->m_dwMcuNodeA)
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[CEqpBase::OnGetMsStatusAck] EqpIns.%d SetTimer EV_EQP_CONNECT_TIMER!\n", GetInsID());	
			SetTimer(EV_EQP_CONNECT_TIMER, EQP_CONNECT_TIMEOUT, 0);
		}

		if( INVALID_NODE == m_pcEqpCfg->m_dwMcuNodeB)
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[CEqpBase::OnGetMsStatusAck] EqpIns.%d SetTimer EV_EQP_CONNECTB_TIMER!\n", GetInsID());
			SetTimer(EV_EQP_CONNECTB_TIMER, EQP_CONNECT_TIMEOUT, 1);
		}
	}
	return TRUE;
}

/*====================================================================
函数名      : OnSetQosCmd
功能        ：设置Qos处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage *const pcMsg
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpBase::OnSetQosCmd(CMessage *const pcMsg)
{
	if(NULL == pcMsg)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::OnSetQosCmd] Mixer.%d pcMsg is Null\n", GetInsID());
        return FALSE;
    }
	
    TMcuQosCfgInfo  tQosInfo;
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    tQosInfo = *(TMcuQosCfgInfo*)cServMsg.GetMsgBody();
    
    u8 byQosType = tQosInfo.GetQosType();
    u8 byAudioValue = tQosInfo.GetAudLevel();
    u8 byVideoValue = tQosInfo.GetVidLevel();
    u8 byDataValue = tQosInfo.GetDataLevel();
    u8 byIpPriorValue = tQosInfo.GetIpServiceType();
	
    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[CEqpBase::OnSetQosCmd]Mixer.%d Type: %d, Aud= %d, Vid= %d, Data= %d, IpPrior= %d\n", 
												GetInsID(), byQosType, byAudioValue, byVideoValue, byDataValue, byIpPriorValue);

	if (NULL == m_pcEqpCfg)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::OnSetQosCmd]Mixer.%d's Cfg is null!\n", GetInsID());
		return FALSE;
	}
	m_pcEqpCfg->m_tQosInfo = tQosInfo;

    if(QOSTYPE_IP_PRIORITY == byQosType)
    {
        byAudioValue = (byAudioValue << 5);
        byVideoValue = (byVideoValue << 5);
        byDataValue = (byDataValue << 5);
        ComplexQos(byAudioValue, byIpPriorValue);
        ComplexQos(byVideoValue, byIpPriorValue);
        ComplexQos(byDataValue, byIpPriorValue);
    }
    else
    {
        byAudioValue = (byAudioValue << 2);
        byVideoValue = (byVideoValue << 2);
        byDataValue = (byDataValue << 2);
    }
	
    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[CEqpBase::OnSetQosCmd] Mixer.%d Aud= %d, Vid= %d, Data= %d\n", 
												GetInsID(), byAudioValue, byVideoValue, byDataValue);
	
    kdvSetMediaTOS((s32)byAudioValue, QOS_AUDIO);
    kdvSetMediaTOS((s32)byVideoValue, QOS_VIDEO);
    kdvSetMediaTOS((s32)byDataValue, QOS_DATA);
	return TRUE;
}
/*====================================================================
函数名      : OnModifyMcuRcvIp
功能        ：修改发送地址
算法实现    ：
引用全局变量：
输入参数说明：CMessage *const pcMsg
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpBase::OnModifyMcuRcvIp(CMessage* const pcMsg)
{
	if(NULL == pcMsg)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::OnModifyMcuRcvIp] pcMsg is Null\n");
        return FALSE;
    }
	//修改保存的远端接收IP地址
	u32 dwSendIP = *(u32*)pcMsg->content;

	if(m_pcEqpCfg != NULL)
	{
		m_pcEqpCfg->m_dwMcuRcvIp = ntohl(dwSendIP);
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[CEqpBase::OnModifyMcuRcvIp] Modify McuRcvIp.%s!\n", StrOfIP(m_pcEqpCfg->m_dwMcuRcvIp));
	}

	return TRUE;
}

/*====================================================================
函数名      ：ComplexQos
功能        ：求复合Qos值
算法实现    ：
引用全局变量：
输入参数说明：u8& byValue
			  u8  byPrior
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2010/01/20  1.0         周嘉麟          创建
====================================================================*/
void CEqpBase::ComplexQos(u8& byValue, u8 byPrior)
{
    u8 byLBit = ((byPrior & 0x08) >> 3);
    u8 byRBit = ((byPrior & 0x04) >> 1);
    u8 byTBit = ((byPrior & 0x02)<<1);
    u8 byDBit = ((byPrior & 0x01)<<3);
    
    byValue = byValue + ((byDBit + byTBit + byRBit + byLBit)*2);
    return;
}
/*====================================================================
函数名      : IsHdEqp
功能        ：是否是高清外设
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpBase::IsHdEqp()const
{
	return FALSE;
}

/*====================================================================
函数名      : PostMsgToMcu
功能        ：向mcu发消息
算法实现    ：
引用全局变量：
输入参数说明：CServMsg& cMsg             消息体
			  const BOOL32 bMcuA         是否发给McuA
			  const BOOL32 bSndToEqpSSn  是否发给EqpSSn
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32	CEqpBase::PostMsgToMcu( CServMsg& cMsg, const BOOL32 bMcuA /* = TRUE */, 
							    const BOOL32 bSndToEqpSSn /* = FALSE */ )const
{
	if (NULL == m_pcMsgSender)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::PostMsgToMcu]m_pcMsgSender is null\n");
		return FALSE;
	}

	if (NULL == m_pcEqpCfg)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::PostMsgToMcu] pcEqpCfg is null!\n");
		return FALSE;
	}

	BOOL32     bRet = FALSE;
	u32    dwDstIId = 0;

	if ( bMcuA && m_pcMsgSender->IsValidTcpNode(m_pcEqpCfg->m_dwMcuNodeA) )
	{
		
		if (bSndToEqpSSn)
		{
			dwDstIId = MAKEIID(AID_MCU_PERIEQPSSN, (u8)(m_pcEqpCfg->byEqpId));
		}
		else
		{
			dwDstIId = m_pcEqpCfg->m_dwMcuIIdA;
		}
		m_pcMsgSender->PostEqpMsg(m_pcEqpCfg->m_dwMcuNodeA, dwDstIId, cMsg);
		bRet = TRUE;
	}
	
	if ( !bMcuA && m_pcMsgSender->IsValidTcpNode(m_pcEqpCfg->m_dwMcuNodeB) )
	{
		if (bSndToEqpSSn)
		{
			dwDstIId = MAKEIID(AID_MCU_PERIEQPSSN, (u8)(m_pcEqpCfg->byEqpId));
		}
		else
		{
			dwDstIId = m_pcEqpCfg->m_dwMcuIIdB;
		}
		m_pcMsgSender->PostEqpMsg(m_pcEqpCfg->m_dwMcuNodeB, dwDstIId, cMsg);
		bRet = TRUE;
	}
	
	return bRet;
}

/*====================================================================
函数名      : PostMsgToServer
功能        ：向生产测试服务器发消息
算法实现    ：
引用全局变量：
输入参数说明：CServMsg& cMsg
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpBase::PostMsgToServer(CServMsg& cMsg,const BOOL32 bSendHeadInfo /* = FALSE */)const
{
	if (NULL == m_pcMsgSender)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::PostMsgToServer]m_pcMsgSender is null\n");
		return FALSE;
	}
	
	if (NULL == m_pcEqpCfg)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::PostMsgToMcu] pcEqpCfg is null!\n");
		return FALSE;
	}

	BOOL32  bRet = FALSE;

	if (m_pcMsgSender->IsValidTcpNode(m_pcEqpCfg->m_dwServerNode))
	{
		if (INVALID_INS == m_pcEqpCfg->m_dwServerIId)
		{
			m_pcMsgSender->PostEqpMsg(m_pcEqpCfg->m_dwServerNode, MAKEIID(SERVER_APPID, 1), cMsg,bSendHeadInfo);	
		}
		else
		{
			m_pcMsgSender->PostEqpMsg(m_pcEqpCfg->m_dwServerNode, m_pcEqpCfg->m_dwServerIId, cMsg,bSendHeadInfo);	
		}	
		bRet = TRUE;
	}

	return bRet;
}
/*====================================================================
函数名      : SetChnNum
功能        ：为外设通道分配地址
算法实现    ：
引用全局变量：
输入参数说明：const u16 wChnNum 创建几个通道
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpBase::SetChnNum(const u16 wChnNum)
{
	if ( 0 == wChnNum )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::CreatChn] wChnNum = 0!\n");
		return FALSE;
	}
	
	//销毁通道
	DestroyChn();

	//分配指针地址
	m_ppcChnl = new CEqpChnnlBase*[wChnNum];
	if (NULL == m_ppcChnl)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase] m_ppcChnl new failed!\n");
		return FALSE;
	}
	
	memset(m_ppcChnl, 0, sizeof(CEqpChnnlBase*)* wChnNum);
	m_wChnNum = wChnNum;
	
	return TRUE;
}

/*====================================================================
函数名      : InitChn
功能        : 绑定通道对象和索引
算法实现    ：
引用全局变量：
输入参数说明：CEqpChnnlBase* pChn 实例化指针对象
			  const u16 wChnIdx   通道索引
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpBase::InitChn(CEqpChnnlBase* pChn, const u16 wChnIdx)
{
	if ( !pChn || !m_ppcChnl || wChnIdx >= m_wChnNum )
	{
		return FALSE;
	}
	
	m_ppcChnl[wChnIdx] = pChn;

	return TRUE;
}
/*====================================================================
函数名      : GetChnNum
功能        ：获取外设通道数
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：u16 
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
u16 CEqpBase::GetChnNum()const
{
	return m_wChnNum;
}

/*====================================================================
函数名      : GetChnl
功能        ：获取通道
算法实现    ：
引用全局变量：
输入参数说明：const u16 wIndex  通道索引
返回值说明  ：CEqpChnnlBase*    通道指针 
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
CEqpChnnlBase* CEqpBase::GetChnl(const u16 wIndex)
{	
	if (wIndex >= m_wChnNum)
	{
		return NULL;
	}

	if (NULL != m_ppcChnl && NULL != m_ppcChnl[wIndex])
	{
		return m_ppcChnl[wIndex];
	}

	return NULL;
}

/*====================================================================
函数名      : SetEqpState
功能        ：设置外设状态机
算法实现    ：
引用全局变量：
输入参数说明：const EEqpState eState  外设状态
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CEqpBase::SetEqpState(const EEqpState eState)
{
	m_eEqpStat = eState;
}

/*====================================================================
函数名      : GetEqpState
功能        ：获取外设状态机
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ： EEqpState
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
CEqpBase::EEqpState CEqpBase::GetEqpState()const
{
 	return m_eEqpStat;
}

/*====================================================================
函数名      : SetTimer
功能        ：设置定时器
算法实现    ：
引用全局变量：
输入参数说明：const u32 dwTimerId        事件号
			  const u32 dwMillionSecs    时间间隔
			  const u32 dwParam			 携带参数
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CEqpBase::SetTimer( const u32 dwTimerId, const u32 dwMillionSecs, const u32 dwParam /* = 0 */ )const
{
	if (NULL == m_pcMsgSender)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::SetTimer] m_pcMsgSender is null\n");
		return;
	}

	m_pcMsgSender->SetTimer(dwTimerId, dwMillionSecs, dwParam);
}

/*====================================================================
函数名      : KillTimer
功能        ：取消定时器
算法实现    ：
引用全局变量：
输入参数说明：u32 dwTimerId  事件号
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CEqpBase::KillTimer(u32 dwTimerId)const
{
	if (NULL == m_pcMsgSender)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::KillTimer] m_pcMsgSender is null\n");
		return;
	}
	
	m_pcMsgSender->KillTimer(dwTimerId);
}

/*====================================================================
函数名      : GetEqpCfg
功能        ：获取外设配置信息
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
const CEqpCfg* CEqpBase::GetEqpCfg()const 
{
	return m_pcEqpCfg;
}

/*====================================================================
函数名      : GetInsID
功能        ：获取实例号
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：u16  Instance实例号
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
u16 CEqpBase::GetInsID()const
{
	if (NULL == m_pcMsgSender)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::GetInsID] m_pcMsgSender is null!\n");
		return INVALID_INS;
	}
	return m_pcMsgSender->GetInsID();
}

/*====================================================================
函数名      : IsValidTcpNode
功能        ：校验Tcp结点有效性
算法实现    ：
引用全局变量：
输入参数说明：const u32 dwTcpNode  Tcp结点
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpBase::IsValidTcpNode(const u32 dwTcpNode)const
{
	if (NULL == m_pcMsgSender)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::IsValidTcpNode] m_pcMsgSender is null!\n");
		return FALSE;
	}
	return m_pcMsgSender->IsValidTcpNode(dwTcpNode);
}

/*====================================================================
函数名      : DisconnectTcpNode
功能        ：断开一个Tcp结点连接
算法实现    ：
引用全局变量：
输入参数说明：const u32 dwTcpNode  Tcp结点
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpBase::DisconnectTcpNode(const u32 dwTcpNode)const
{
	if (NULL == m_pcMsgSender)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::DisconnectTcpNode] m_pcMsgSender is null!\n");
		return FALSE;
	}
	return m_pcMsgSender->DisconnectTcpNode(dwTcpNode);
}

/*====================================================================
函数名      : ConnectTcpNode
功能        ：创建TCP连接
算法实现    ：
引用全局变量：
输入参数说明：const u32 dwConnectIP  连接远端IP
			  const u16 wConnectPort 连接远端端口
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
int CEqpBase::ConnectTcpNode(const u32 dwConnectIP, const u16 wConnectPort)
{
	if (NULL == m_pcMsgSender)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::ConnectTcpNode] m_pcMsgSender is null!\n");
		return FALSE;
	}
	return m_pcMsgSender->ConnectTcpNode(dwConnectIP, wConnectPort);
}

/*====================================================================
函数名      : OnConnectServerTimer
功能        ：生产测试连接服务器定时处理
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpBase::OnConnectServerTimer()
{
	if (NULL == m_pcEqpCfg)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::OnConnectServerTimer] m_pcEqpCfg is null!\n");
		return FALSE;
	}

	u32 dwServerIp  = m_pcEqpCfg->dwConnectIP;
	u16 wServerPort = m_pcEqpCfg->wConnectPort;
	
	if (!IsValidTcpNode(m_pcEqpCfg->m_dwServerNode))
	{
		m_pcEqpCfg->m_dwServerNode = ConnectTcpNode(htonl(dwServerIp), wServerPort);
	}

	BOOL32 bMsgSenderNull = NULL == m_pcMsgSender? TRUE : FALSE;
	if (m_pcEqpCfg->m_dwServerNode == INVALID_NODE || NULL == m_pcMsgSender
		|| !m_pcMsgSender->IsValidTcpNode(m_pcEqpCfg->m_dwServerNode)  || OSP_OK != m_pcMsgSender->DisConRegister(m_pcEqpCfg->m_dwServerNode))
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_EQP, "[CEqpBase::OnConnectServerTimer]Connect Server %s failed, node: %d, pSend is null[%d]!\n", 
												StrOfIP(dwServerIp), m_pcEqpCfg->m_dwServerNode, bMsgSenderNull);
		
		SetTimer(EV_EQPSERVER_CONNECT_TIMER, EQP_CONNECT_TIMEOUT);
		return FALSE;
	}
	
    SetTimer(EV_EQPSERVER_REGISTER_TIMER, EQP_REGISTER_TIMEOUT);

	return TRUE;
}

/*====================================================================
函数名      : OnRegisterServerTimer
功能        ：生产测试注册服务器定时处理
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpBase::OnRegisterServerTimer()
{
	u8 abyTemp[sizeof(u8)*32 + sizeof(CLoginRequest) + sizeof(CDeviceInfo)];
    memset(abyTemp, 0, sizeof(abyTemp));
    CLoginRequest cLoginReq;
    cLoginReq.Empty();
    cLoginReq.SetName("admin");
    cLoginReq.SetPassword("admin");
    memcpy(abyTemp + sizeof(u8)*32, &cLoginReq, sizeof(CLoginRequest));
    
    CDeviceInfo cDeviceinfo;
	memset( &cDeviceinfo, 0x0, sizeof( cDeviceinfo ) );
    SetDeviceInfo(cDeviceinfo);
    memcpy(abyTemp + sizeof(u8)*32 + sizeof(CLoginRequest), &cDeviceinfo, sizeof(CDeviceInfo));
	
	CServMsg cMsg;
	cMsg.SetMsgBody((u8*)abyTemp, sizeof(u8) * 32  + sizeof(CLoginRequest) + sizeof(CDeviceInfo));
	cMsg.SetEventId(EQP_SERVER_REGISTER_REQ);
	PostMsgToServer(cMsg,FALSE);

	SetTimer( EV_EQPSERVER_REGISTER_TIMER, EQP_REGISTER_TIMEOUT);
	return TRUE;
}

/*====================================================================
函数名      : OnRegisterServerAck
功能        ：外设注册生产测试服务器成功处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage*const pcMsg
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpBase::OnRegisterServerAck(CMessage* const pcMsg)
{
	KillTimer(EV_EQPSERVER_REGISTER_TIMER);

	if (NULL == m_pcEqpCfg)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::OnRegisterServerAck] m_pcEqpCfg is null!\n");
		return FALSE;
	}

	m_pcEqpCfg->m_dwServerIId = pcMsg->srcid;

	return TRUE;
}

/*====================================================================
函数名      : OnRegisterServerNack
功能        ：外设注册生产测试服务器失败处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage*const pcMsg
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CEqpBase::OnRegisterServerNack(CMessage* const pcMsg)
{
	if (NULL == pcMsg)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::OnRegisterServerNack] pcMsg is null!\n");
		return FALSE;
	}
	LogPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[CEqpBase::OnRegisterServerNack]!\n");
	return TRUE;
}
/*====================================================================
函数名      : SetDeviceInfo
功能        ：获取设备信息
算法实现    ：
引用全局变量：
输入参数说明：CDeviceInfo &cDeviceInfo 返回的设备信息
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
/*lint -save -e715*/
void CEqpBase::SetDeviceInfo(CDeviceInfo &cDeviceInfo)
{
#ifndef WIN32
	s8 gs_VersionBuf[128] = {0};
    {
        strcpy(gs_VersionBuf, KDV_MCU_PREFIX);
		
        s8 achMon[16] = {0};
        u32 byDay = 0;
        u32 byMonth = 0;
        u32 wYear = 0;
        s8 achFullDate[24] = {0};
		
        s8 achDate[32] = {0};
        sprintf(achDate, "%s", __DATE__);
        StrUpper(achDate);
		
        sscanf(achDate, "%s %d %d", achMon, &byDay, &wYear );
		
        if ( 0 == strcmp( achMon, "JAN") )		 
            byMonth = 1;
        else if ( 0 == strcmp( achMon, "FEB") )
            byMonth = 2;
        else if ( 0 == strcmp( achMon, "MAR") )
            byMonth = 3;
        else if ( 0 == strcmp( achMon, "APR") )		 
            byMonth = 4;
        else if ( 0 == strcmp( achMon, "MAY") )
            byMonth = 5;
        else if ( 0 == strcmp( achMon, "JUN") )
            byMonth = 6;
        else if ( 0 == strcmp( achMon, "JUL") )
            byMonth = 7;
        else if ( 0 == strcmp( achMon, "AUG") )
            byMonth = 8;
        else if ( 0 == strcmp( achMon, "SEP") )		 
            byMonth = 9;
        else if ( 0 == strcmp( achMon, "OCT") )
            byMonth = 10;
        else if ( 0 == strcmp( achMon, "NOV") )
            byMonth = 11;
        else if ( 0 == strcmp( achMon, "DEC") )
            byMonth = 12;
        else
            byMonth = 0;
		
        if ( byMonth != 0 )
        {
            sprintf(achFullDate, "%04d%02d%02d", wYear, byMonth, byDay);
            sprintf(gs_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
        }
        else
        {
            sprintf(gs_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
        }
    }

    s8* pSoftVer = (s8*)gs_VersionBuf;
    cDeviceInfo.setSoftVersion( pSoftVer );

    //MAC, IP, SubMask
	s8 achMac[VERSION_LEN];
	memset( achMac, 0x0, sizeof(achMac) );
	u8 byEthId = 0;
	u8 byState = 0;
	//判断用前(外/0)网口还是后(内/1)网口，默认用外网口
	for(; byEthId < 2; byEthId ++)
	{
		if( ERROR == BrdGetEthLinkStat(byEthId, &byState) )
		{
			printf("[SetDeviceInfo] Call BrdGetEthLinkStat(%u) ERROR!\n", byEthId);
			break;
		}
		if( byState == 1 )
		{
			break;
		}
	}
	
	TBrdEthParam tBrdEthParam;
	memset( &tBrdEthParam, 0x0, sizeof(tBrdEthParam) );
    BrdGetEthParam( byEthId, &tBrdEthParam );
    
	u32 dwIp = tBrdEthParam.dwIpAdrs;
    u32 dwMask = tBrdEthParam.dwIpMask; 
	sprintf(achMac,
        "%02X-%02X-%02X-%02X-%02X-%02X",
        tBrdEthParam.byMacAdrs[0],
        tBrdEthParam.byMacAdrs[1],
        tBrdEthParam.byMacAdrs[2],
        tBrdEthParam.byMacAdrs[3],
        tBrdEthParam.byMacAdrs[4],
        tBrdEthParam.byMacAdrs[5]
        ); 
	cDeviceInfo.setMac( achMac );
    cDeviceInfo.setIp( dwIp );
    cDeviceInfo.setSubMask( dwMask ); 
	
	// hw version, serialNo
	TBrdE2PromInfo tBrdE2PromInfo;
	s8 achSerial[12] = { 0 };
	s8 achHWversion[VERSION_LEN];
	
	memset( achHWversion, 0x0, sizeof(achHWversion) );
	memset( &tBrdE2PromInfo, 0x0, sizeof(tBrdE2PromInfo) );
    BrdGetE2PromInfo( &tBrdE2PromInfo );
	
    sprintf( achHWversion, "%d", tBrdE2PromInfo.dwHardwareVersion );
    memcpy( achSerial, tBrdE2PromInfo.chDeviceSerial, sizeof(tBrdE2PromInfo.chDeviceSerial) );
	cDeviceInfo.setHardVersion( achHWversion );
    cDeviceInfo.setSerial( achSerial ); 
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "achMac=[%s]achHWversion=[%s]\n", achMac, achHWversion);

#endif
	return;
}
/*lint -restore*/
/*====================================================================
函数名      : CEqpCfg
功能        ：CEqpCfg构造
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
CEqpCfg::CEqpCfg():TEqpCfg(),
				   m_dwMcuNodeA(INVALID_NODE),
				   m_dwMcuIIdA(INVALID_INS),
				   m_dwMcuNodeB(INVALID_NODE),
				   m_dwMcuIIdB(INVALID_INS),
				   m_dwServerNode(INVALID_NODE),				   
				   m_dwServerIId(INVALID_INS),
				   m_dwMpcSSrc(0),
				   m_dwMcuRcvIp(0),
				   m_wMcuRcvStartPort(0),
				   m_wEqpVer(0xffff),
				   m_byRegAckNum(0)			   
{
	memset(&m_tPrsTimeSpan, 0, sizeof(m_tPrsTimeSpan));
	memset(&m_tQosInfo, 0, sizeof(m_tQosInfo));
}

/*====================================================================
函数名      : CEqpCfg
功能        ：CEqpCfg析构
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
CEqpCfg::~CEqpCfg()
{

}
/*====================================================================
函数名      : Print
功能        ：打印外设基本配置
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CEqpCfg::Print()
{
	StaticLog("Local:\n");
	StaticLog("\t IP			: %s\n", StrOfIP(dwLocalIP));
	StaticLog("\t EqpType		: %d\n", byEqpType);
	StaticLog("\t EqpId			: %d\n", byEqpId);
	StaticLog("\t EqpAlias		: %s\n", achAlias);

	StaticLog("\t ConnectIPA		: %s\n", StrOfIP(dwConnectIP));
	StaticLog("\t ConnectPortA		: %d\n", wConnectPort);
	StaticLog("\t RcvStartPortA		: %d\n", wRcvStartPort);

	StaticLog("\t ConnectIPB		: %s\n", StrOfIP(dwConnectIpB));
	StaticLog("\t ConnectPortB		: %d\n", wConnectPortB);
	StaticLog("\t RcvStartPortB		: %d\n", wRcvStartPortB);
	
	StaticLog("\nMcu:\n");
	StaticLog("\t McuNodeA		: %d\n",  m_dwMcuNodeA);
	StaticLog("\t McuIIdA		: %d\n",  m_dwMcuIIdA);
	StaticLog("\t McuNodeB		: %d\n",  m_dwMcuNodeB);
	StaticLog("\t McuIIdB		: %d\n",  m_dwMcuIIdB);
	StaticLog("\t ServerNode		: %d\n",  m_dwServerNode);
	StaticLog("\t ServerIId		: %d\n",  m_dwServerIId);
	StaticLog("\t McuSSrc		: %d\n",  m_dwMpcSSrc);
	StaticLog("\t McuRcvIp		: %s\n",  StrOfIP(m_dwMcuRcvIp));
	StaticLog("\t McuStartPort		: %d\n",  m_wMcuRcvStartPort);
	StaticLog("\t EqpVer			: %d\n",  m_wEqpVer);
	StaticLog("\t RegAckNum		: %d\n",  m_byRegAckNum);

	StaticLog("\nPrs:\n");
	StaticLog("\t FirstSpan		: %d\n", m_tPrsTimeSpan.m_wFirstTimeSpan);
	StaticLog("\t SecondSpan		: %d\n", m_tPrsTimeSpan.m_wSecondTimeSpan);
	StaticLog("\t ThirdSpan		: %d\n", m_tPrsTimeSpan.m_wThirdTimeSpan);
	StaticLog("\t RejectSpan		: %d\n", m_tPrsTimeSpan.m_wRejectTimeSpan);
	return;
}