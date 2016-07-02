#include "apu2eqp.h"



/*====================================================================
函数名      : CApu2Eqp
功能        ：CApu2Eqp构造
算法实现    ：
引用全局变量：
输入参数说明：CEqpMsgSender* const pcMsgSender  消息发送对象绑定
			  CEqpCfg* const       pcCfg        配置绑定
			  const u16            wChnNum      创建通道数
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
CApu2Eqp::CApu2Eqp(CEqpMsgSender* const pcMsgSender, CEqpCfg* const pcCfg, const u16 wChnNum)
	:CEqpBase(pcMsgSender, pcCfg), m_bInited(FALSE)
{
	//为通道指针分配内存
	if(!CEqpBase::SetChnNum(wChnNum))
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "CreatChn failed!\n");
		return;
	}
	
	/*lint -save -esym(429, pcApu2Chnnl)*/
	CApu2Chnnl* pcApu2Chnnl = NULL;
	for (u16 wLoop = 0; wLoop < wChnNum; wLoop++)
	{
		//为通道分配内存
		pcApu2Chnnl = new CApu2Chnnl(this, pcCfg, wLoop);
		PTR_NULL_VOID(pcApu2Chnnl);
		
		//通道指针指向通道
		if(!CEqpBase::InitChn( pcApu2Chnnl, wLoop))
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "CApu2Eqp init chn[%d] failed\n", wLoop);
		}
	}
	/*lint -restore*/
}

/*====================================================================
函数名      : ~CApu2Eqp
功能        ：CApu2Eqp析构
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
CApu2Eqp::~CApu2Eqp()
{
	CApu2Eqp::Destroy();
}
/*lint -restore*/
/*====================================================================
函数名      : Init
功能        ：Apu2外设初始化
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CApu2Eqp::Init()
{
	if (m_bInited)
	{
		return TRUE;
	}
	
	//初始化socket
	u16 wRet = KdvSocketStartup();
    
    if ( MEDIANET_NO_ERROR != wRet )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[CApu2Eqp::Init] KdvSocketStartup failed!\n");
        return FALSE;
    }

	//外设基类初始化
	if(!CEqpBase::Init())
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[CApu2Eqp::Init] CEqpBase::Init error!\n");
		return FALSE;
	}
	
	const CEqpCfg* pcEqpCfg = CEqpBase::GetEqpCfg();
	PTR_NULL_RETURN(pcEqpCfg, FALSE);

	// 定时建链
    if( 0 != pcEqpCfg->dwConnectIP )
    {
        SetTimer(EV_EQP_CONNECT_TIMER,  MIX_CONNETC_TIMEOUT, 0 );
    }

	// 定时建链
    if( 0 != pcEqpCfg->dwConnectIpB )
    {
        SetTimer(EV_EQP_CONNECTB_TIMER, MIX_CONNETC_TIMEOUT, 1 );
    }

	m_bInited = TRUE;
	return TRUE;
}

/*====================================================================
函数名      : Destroy
功能        ：外设销毁
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CApu2Eqp::Destroy()
{
	//关闭socket
	KdvSocketCleanup();

	//基类销毁
	CEqpBase::Destroy();
	m_bInited = FALSE;
	return TRUE;
}
/*====================================================================
函数名      : OnMessage
功能        ：Apu2外设消息处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage* const pcMsg
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CApu2Eqp::OnMessage(CMessage* const pcMsg)
{
	PTR_NULL_RETURN(pcMsg, FALSE);

	//[20130131 guodawei]
	LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER, 
		"MIXER.%d (CApu2Eqp) Received msg [%u](%s).\n", GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event));

	switch (pcMsg->event)
	{
	case MCU_EQP_REG_ACK:
		OnRegisterMcuAck(pcMsg);
	  	 break;

	case MCU_EQP_REG_NACK:
	    OnRegisterMcuNack(pcMsg);
	 	 break;

	case EV_MIXER_SHOWMIX:
	case EV_MIXER_SHOWCHINFO:
	case EV_MIXER_SHOWSTATE:
		OnMixPrint(pcMsg->event);
		break;

	case MCU_EQP_QOS_CMD:
		OnSetQosCmd(pcMsg);
		 break;
		
	case MCU_EQP_MODIFYADDR_CMD:
		OnModifyMcuRcvIp(pcMsg);
		break;

	default:
		{
			if (pcMsg->event != EV_EQP_CONNECT_TIMER &&
				pcMsg->event != EV_EQP_CONNECTB_TIMER &&
				pcMsg->event != EV_EQP_REGISTER_TIMER &&
				pcMsg->event != EV_EQP_REGISTERB_TIMER &&
				pcMsg->event != OSP_DISCONNECT)
			{
				CServMsg cServMsg(pcMsg->content, pcMsg->length);
				if (pcMsg->event == MCU_MIXER_ADDMEMBER_REQ)
				{
					//消息体最后追加通道号(u8)
					u8 byChnIdx = cServMsg.GetChnIndex();
					cServMsg.CatMsgBody((u8*)&byChnIdx, sizeof(u8));
					pcMsg->length += sizeof(u8);
				}
				cServMsg.SetChnIndex(0);
				pcMsg->content = cServMsg.GetServMsg();
			}
			CEqpBase::OnMessage(pcMsg);
		}
		break;
	}
	return TRUE;
}

/*====================================================================
函数名      : OnRegisterMcuAck
功能        ：注册Mcu成功处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage *const pcMsg
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CApu2Eqp::OnRegisterMcuAck(CMessage *const pcMsg)
{
	PTR_NULL_RETURN(pcMsg, FALSE);

	if (!CEqpBase::OnRegisterMcuAck(pcMsg))
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[CApu2Eqp::OnRegisterMcuAck] CEqpBase::OnRegisterMcuAck failed!\n");
		return FALSE;
	}

	CApu2MixerCfg* pcCfg = (CApu2MixerCfg*)GetEqpCfg();
	PTR_NULL_RETURN(pcCfg, FALSE);

	//通道状态翻转成ready
	u16 wChnNum = CEqpBase::GetChnNum();
	CApu2Chnnl* pcChnnl = NULL;
	for (u16 wIdx = 0; wIdx < wChnNum; wIdx++)
	{
		pcChnnl = GetChnnl(wIdx);
		PTR_NULL_RETURN(pcChnnl, FALSE);
		if (pcChnnl->GetState() == (u8)CApu2ChnStatus::IDLE)
		{
			pcChnnl->SetState((u8)CApu2ChnStatus::READY);
		}
	}

	//状态通告
	CServMsg cMsg;
	TPeriEqpStatus tEqpStatus;
    tEqpStatus.SetMcuEqp(LOCAL_MCUID, pcCfg->byEqpId, pcCfg->byEqpType);
    tEqpStatus.m_byOnline = TRUE;
	tEqpStatus.m_tStatus.tMixer.m_atGrpStatus[0].m_byGrpState = (u8)TMixerGrpStatus::READY;
    tEqpStatus.m_tStatus.tMixer.m_byGrpNum = 1 ;
    tEqpStatus.m_tStatus.tMixer.m_byMixOffChnNum = 0;
	tEqpStatus.m_tStatus.tMixer.m_atGrpStatus[0].m_byMixGrpChnNum = pcCfg->m_byMemberNum;
	tEqpStatus.m_tStatus.tMixer.m_byMixOffChnNum = pcCfg->m_byRcvOff;
    tEqpStatus.SetAlias(pcCfg->achAlias);
    cMsg.SetMsgBody((u8*)&tEqpStatus, sizeof(tEqpStatus));
	cMsg.SetEventId(MIXER_MCU_MIXERSTATUS_NOTIF);

	if (pcMsg->srcnode == pcCfg->m_dwMcuNodeA)
	{
		PostMsgToMcu(cMsg);
	}
	else if(pcMsg->srcnode == pcCfg->m_dwMcuNodeB)
	{	
		PostMsgToMcu(cMsg, FALSE);
	}
	
	return TRUE;
}

/*====================================================================
函数名      : OnRegisterMcuNack
功能        ：注册Mcu失败处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage *const pcMsg
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CApu2Eqp::OnRegisterMcuNack(CMessage *const pcMsg)
{
	PTR_NULL_RETURN(pcMsg, FALSE);

	return CEqpBase::OnRegisterMcuNack(pcMsg);
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
2023/03/08  2.0         国大卫          创建
====================================================================*/
BOOL32 CApu2Eqp::OnModifyMcuRcvIp(CMessage* const pcMsg)
{
	PTR_NULL_RETURN(pcMsg, FALSE);

	//修改保存的发送IP地址
	CEqpBase::OnModifyMcuRcvIp(pcMsg);
	CApu2Chnnl* pMixerChn = NULL;
	//重新取出该值
	CApu2MixerCfg* pMixerCfg = (CApu2MixerCfg*)(GetEqpCfg());
	for (u16 wIndex = 0; wIndex < GetChnNum(); wIndex++)
	{
		pMixerChn = GetChnnl(wIndex);
		PTR_NULL_CONTINUE(pMixerChn);
		//修改发送对象
		switch(pMixerChn->GetState())
		{
		case CApu2ChnStatus::MIXING:
		case CApu2ChnStatus::MIXING_VAC:
			{
				for (u8 byLoop = 0; byLoop < MAXNUM_NMODECHN; byLoop++)
				{
					TAudDecEncInfo *tAudInfo = (TAudDecEncInfo *)pMixerChn->GetChnlInfo(pMixerCfg->m_byMemberNum + byLoop);
					if (tAudInfo->byEncAudType == MEDIA_TYPE_NULL)
					{
						continue;
					}
					pMixerChn->SetSndObjectPara(pMixerCfg->m_dwMcuRcvIp, 
						pMixerCfg->m_wMcuRcvStartPort + (pMixerCfg->m_byIsSimuApu? 0 :pMixerCfg->m_byMemberNum + byLoop) * PORTSPAN + 2, 
						pMixerCfg->m_byMemberNum + byLoop);
					LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[CApu2Eqp::OnModifyMcuRcvIp] Mixer.%d ResetSndObjectPara on Chnl.%d\n", 
						GetInsID(), pMixerCfg->m_byMemberNum + byLoop);
				}
			}
			break;
		default:
			break;
		}
	}
	return TRUE;
}

/*====================================================================
函数名      : GetChnnl
功能        : 获取通道
算法实现    ：
引用全局变量：
输入参数说明：const u16 wChnIdx 通道索引
返回值说明  ：CApu2Chnnl*       实例化通道
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
CApu2Chnnl* CApu2Eqp::GetChnnl( const u16 wChnIdx ) 
{
	return (CApu2Chnnl*)GetChnl(wChnIdx); 
}

/*====================================================================
函数名      : OnMixPrint
功能        ：混音器打印信息
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
20130613	2.0			国大卫			创建
====================================================================*/
BOOL32 CApu2Eqp::OnMixPrint(u16 wMsgID)
{
	CApu2Chnnl* pMixerChn = NULL;
	for (u16 wIndex = 0; wIndex < GetChnNum(); wIndex++)
	{
		pMixerChn = GetChnnl(wIndex);
		PTR_NULL_CONTINUE(pMixerChn);
		CApu2MixerCfg* pMixerCfg = (CApu2MixerCfg*)(GetEqpCfg());
		PTR_NULL_RETURN(pMixerCfg, FALSE);

		switch (wMsgID)
		{
		case EV_MIXER_SHOWCHINFO:
			StaticLog("\n******************* MIXER.[%d]'s ChnlNetInfo *******************\n", pMixerChn->GetInsID());
			pMixerChn->PrintChnlInfo();
			break;
		case EV_MIXER_SHOWSTATE:
			StaticLog("\n******************* MIXER.[%d]'s MediaInfo *******************\n", pMixerChn->GetInsID());
			pMixerChn->PrintMixState();
			break;
		case EV_MIXER_SHOWMIX:
			StaticLog("\n******************* MIXER.[%d]'s ConfigInfo *******************\n", pMixerChn->GetInsID());
			pMixerCfg->Print();
			StaticLog("\n******************* MIXER.[%d]'s WorkParam ********************\n", pMixerChn->GetInsID());
			pMixerChn->Print();
		default:
			break;
		}
		StaticLog("\n***************************************************************\n");
	}
	return TRUE;
}
/*====================================================================
函数名      : GetChnnl
功能        : 获取通道
算法实现    ：
引用全局变量：
输入参数说明：const u16 wChnIdx 通道索引
返回值说明  ：CApu2Chnnl*       实例化通道
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
CApu2Chnnl* CTestEqp::GetChnnl( const u16 wChnIdx ) 
{
	return (CApu2Chnnl*)GetChnl(wChnIdx); 
}
/*====================================================================
函数名      : OnMixPrint
功能        ：混音器打印信息
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
20130613	2.0			国大卫			创建
====================================================================*/
BOOL32 CTestEqp::OnMixPrint(u16 wMsgID)
{
	CApu2Chnnl* pMixerChn = NULL;
	for (u16 wIndex = 0; wIndex < GetChnNum(); wIndex++)
	{
		pMixerChn = GetChnnl(wIndex);
		PTR_NULL_CONTINUE(pMixerChn);
		CApu2MixerCfg* pMixerCfg = (CApu2MixerCfg*)(GetEqpCfg());
		PTR_NULL_RETURN(pMixerCfg, FALSE);

		switch (wMsgID)
		{
		case EV_MIXER_SHOWCHINFO:
			StaticLog("\n******************* MIXER.[%d]'s ChnlNetInfo *******************\n", pMixerChn->GetInsID());
			pMixerChn->PrintChnlInfo();
			break;
		case EV_MIXER_SHOWSTATE:
			StaticLog("\n******************* MIXER.[%d]'s MediaInfo *******************\n", pMixerChn->GetInsID());
			pMixerChn->PrintMixState();
			break;
		case EV_MIXER_SHOWMIX:
			StaticLog("\n******************* MIXER.[%d]'s ConfigInfo *******************\n", pMixerChn->GetInsID());
			pMixerCfg->Print();
			StaticLog("\n******************* MIXER.[%d]'s WorkParam ********************\n", pMixerChn->GetInsID());
			pMixerChn->Print();
		default:
			break;
		}
		StaticLog("\n***************************************************************\n");
	}
	return TRUE;
}

/*====================================================================
函数名      : CTestEqp
功能        ：CTestEqp构造
算法实现    ：
引用全局变量：
输入参数说明：CEqpMsgSender* const pcMsgSender  消息发送对象绑定
			  CEqpCfg* const       pcCfg		配置
			  const u16            wChnNum      创建通道数
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
CTestEqp::CTestEqp(CEqpMsgSender* const pcMsgSender, CEqpCfg* const pcCfg, const u16 wChnNum)
	:CEqpBase(pcMsgSender, pcCfg), m_bInited(FALSE)
{
	//为通道指针分配内存
	if(!CEqpBase::SetChnNum(wChnNum))
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "CreatChn failed!\n");
		return;
	}
	
	CApu2Chnnl* pcApu2Chnnl = NULL;
	for (u16 wLoop = 0; wLoop < wChnNum; wLoop++)
	{
		//为通道分配内存
		pcApu2Chnnl = new CApu2Chnnl(this, pcCfg, wLoop);
		PTR_NULL_VOID(pcApu2Chnnl);
		
		//通道指针指向通道
		if(!CEqpBase::InitChn( pcApu2Chnnl, wLoop))
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "CApu2Eqp init chn[%d] failed\n", wLoop);
		}
		printf("[PRODUCTTEST] CApu2Eqp init chn[%d] success!\n", wLoop);
	}
}

/*====================================================================
函数名      : ~CTestEqp
功能        ：CTestEqp析构
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
CTestEqp::~CTestEqp()
{
	CTestEqp::Destroy();
}
/*lint -restore*/
/*====================================================================
函数名      : Destroy
功能        ：外设销毁
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CTestEqp::Destroy()
{
	//关闭socket
	KdvSocketCleanup();

	//基类销毁
	CEqpBase::Destroy();
	m_bInited = FALSE;
	return TRUE;
}

/*====================================================================
函数名      : OnMessage
功能        ：TestEqp外设消息处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage* const pcMsg
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CTestEqp::OnMessage(CMessage* const pcMsg)
{
	PTR_NULL_RETURN(pcMsg, FALSE);

	switch (pcMsg->event)
	{
	case SERVER_EQP_REGISTER_ACK:
		OnRegisterServerAck(pcMsg);
		 break;

	case SERVER_EQP_REGISTER_NACK:
		OnRegisterServerNack(pcMsg);
		 break;

	case EV_MIXER_SHOWMIX:
	case EV_MIXER_SHOWCHINFO:	
	case EV_MIXER_SHOWSTATE:
		OnMixPrint(pcMsg->event);
		break;

	case EV_EQPSERVER_REGISTER_TIMER:
		OnRegisterServerTimer();
		 break;
	default:
		{
			if (pcMsg->event != EV_EQPSERVER_CONNECT_TIMER &&
				pcMsg->event != OSP_DISCONNECT &&
				pcMsg->event != TIMER_TEST_LED)
			{
				CServMsg cServMsg(pcMsg->content, pcMsg->length);
				cServMsg.SetChnIndex(0);
				pcMsg->content = cServMsg.GetServMsg();
			}
			CEqpBase::OnMessage(pcMsg);
		}
		break;
	}
	return TRUE;
}

/*====================================================================
函数名      : Init
功能        ：TestEqp外设初始化
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CTestEqp::Init()
{
	if (m_bInited)
	{
		return TRUE;
	}
	
	//初始化socket
	u16 wRet = KdvSocketStartup();
    
    if ( MEDIANET_NO_ERROR != wRet )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[CTestEqp::Init] KdvSocketStartup failed!\n");
        return FALSE;
    }

	//外设基类初始化
	if(!CEqpBase::Init())
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[CTestEqp::Init] CEqpBase::Init error!\n");
		return FALSE;
	}

    SetTimer(EV_EQPSERVER_CONNECT_TIMER,  MIX_CONNETC_TIMEOUT, 0 );

	m_bInited = TRUE;
	CEqpBase::SetEqpState(E_EQP_OFFLINE);
	//[20130131 guodawei]
	printf("[PRODUCTTEST]CTestEqp Init success!\n");

	return TRUE;
}

/*====================================================================
函数名      : OnRegisterServerAck
功能        ：注册界面服务器成功处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage *const pcMsg
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CTestEqp::OnRegisterServerAck(CMessage *const pcMsg)
{
	PTR_NULL_RETURN(pcMsg, FALSE);

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[CTestEqp::OnRegisterServerAck]!\n");
	
	if(!CEqpBase::OnRegisterServerAck(pcMsg))
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[CTestEqp::OnRegisterServerAck] [CEqpBase::OnRegisterServerAck] failed!\n");
		return FALSE;
	}
	//[20130131 guodawei]
	printf("[PRODUCTTEST]Apu2 registered success!\n");

	//通道状态翻转成ready
	u16 wChnNum = CEqpBase::GetChnNum();
	CApu2Chnnl* pcChnnl = NULL;
	for (u16 wIdx = 0; wIdx < wChnNum; wIdx++)
	{
		pcChnnl = GetChnnl(wIdx);
		PTR_NULL_RETURN(pcChnnl, FALSE);
		pcChnnl->SetState((u8)CApu2ChnStatus::READY);
	}

	return TRUE;
}

/*====================================================================
函数名      : OnRegisterServerNack
功能        ：注册界面服务器失败处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage *const pcMsg
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CTestEqp::OnRegisterServerNack(CMessage *const pcMsg)
{
	PTR_NULL_RETURN(pcMsg, FALSE);

	//[20130131 guodawei]
	printf("[PRODUCTTEST]Apu2 registered failed!\n");

	SetTimer( EV_EQPSERVER_REGISTER_TIMER, EQP_REGISTER_TIMEOUT);
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
2012/05/03  1.0         田志勇          创建
====================================================================*/
BOOL32 CTestEqp::OnRegisterServerTimer()
{
	//[20130131 guodawei]
	printf("[PRODUCTTEST]Apu2 is registering To testserver now!\n");

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[OnRegisterServerTimer]Reg To Server\n");
	u8 abyTemp[sizeof(u8)*32 + sizeof(CLoginRequest) + sizeof(CDeviceInfo)];
    memset(abyTemp, 0, sizeof(abyTemp));
	abyTemp[0] = APU2_MIXER;
#ifdef _8KI_
	abyTemp[1] = 1;
#endif
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
