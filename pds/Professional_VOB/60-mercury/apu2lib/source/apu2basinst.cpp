#include "apu2basinst.h"

/*====================================================================
函数名      ：CApu2BasInst
功能        ：CApu2BasInst构造
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013.03/28	2.0			国大卫		  创建
====================================================================*/
CApu2BasInst::CApu2BasInst():m_pcEqp(NULL)
{
	
}

/*====================================================================
函数名      ：~CApu2BasInst
功能        ：CApu2BasInst析构
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/28  2.0         国大卫        创建
====================================================================*/
CApu2BasInst::~CApu2BasInst()
{
	SAFE_DELETE(m_pcEqp);
}

/*====================================================================
函数名      ：DaemonInstanceEntry
功能        ：
算法实现    ：
引用全局变量：
输入参数说明：CMessage * const pcMsg
		      CApp*            pcApp
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/28	2.0			国大卫		  创建
====================================================================*/
void CApu2BasInst::DaemonInstanceEntry( CMessage * const pcMsg, CApp* pcApp )
{
	PTR_NULL_VOID(pcMsg);
	PTR_NULL_VOID(pcApp);

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"BAS: [CApu2BasInst::DaemonInstanceEntry] Received MSG %u(%s).\n", pcMsg->event, ::OspEventDesc(pcMsg->event));

	switch(pcMsg->event)
	{
	case EV_BAS_INIT:
		DaemonProcInit(pcMsg);
		break;

	case EV_BAS_SHOWBAS:
	case EV_BAS_SHOWCHINFO:
		DaemonProcPrint(pcMsg);
		break;

	default:
		break;
	}
}

/*====================================================================
函数名      ：DaemonProcInit
功能        ：Daemon实例根据配置投递给CommonInst创建对象
算法实现    ：
引用全局变量：
输入参数说明：CMessage * const pcMsg 
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/28	2.0			国大卫		  创建
====================================================================*/
void CApu2BasInst::DaemonProcInit(CMessage * const pcMsg)
{
	PTR_NULL_VOID(pcMsg);
	
	TApu2EqpCfg* ptApu2Cfg = (TApu2EqpCfg*)pcMsg->content;
	
	for (u8 byIdx = 0; byIdx < ptApu2Cfg->m_byBasNum ; byIdx++ )
    {
		post(MAKEIID(GetAppID(),byIdx+1), EV_BAS_INIT, (u8*)&(ptApu2Cfg->m_acBasCfg[byIdx]), sizeof(CApu2BasCfg));
    }
}

/*====================================================================
函数名      ：InstanceEntry
功能        ：
算法实现    ：
引用全局变量：
输入参数说明：CMessage * const pcMsg
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/28  2.0         国大卫        创建
====================================================================*/
void CApu2BasInst::InstanceEntry( CMessage * const pcMsg )
{
	PTR_NULL_VOID(pcMsg);
	
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"BAS: [CApu2BasInst::InstanceEntry] BAS.%d Received MSG [%u](%s).\n", GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event));

	if (EV_BAS_INIT == pcMsg->event)
	{
		ProcInit(pcMsg);
	}
	else
	{
		//外-->内
		TransMcuMsgToEqpBaseMsg(pcMsg);

		PTR_NULL_VOID(m_pcEqp);
		m_pcEqp->OnMessage(pcMsg);
	}
}

/*====================================================================
函数名      ：ProcInit
功能        ：单外设初始化
算法实现    ：
引用全局变量：
输入参数说明：CMessage * const pcMsg 
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/29	2.0			国大卫		  创建
====================================================================*/
void CApu2BasInst::ProcInit(CMessage * const pcMsg)
{
	PTR_NULL_VOID(pcMsg);
	
	//适配器配置
	CApu2BasCfg* pApu2BasCfg = (CApu2BasCfg*)(pcMsg->content);
	PTR_NULL_VOID(pApu2BasCfg);

	//实例化Apu2对象
	/*lint -save -e429*/
	if (NULL == m_pcEqp)
	{	
		CApu2BasCfg* pcCfg = new CApu2BasCfg();
		PTR_NULL_VOID(pcCfg);

		//保存配置
		pcCfg->Copy(pApu2BasCfg);
		pcCfg->m_wEqpVer = DEVVER_APU2;

		//初始化1个通道对象
		m_pcEqp = new CApu2BasEqp((CEqpMsgSender*)this, pcCfg, MAXNUM_BASCHN);

		if (NULL == m_pcEqp)
		{
			SAFE_DELETE(pcCfg);
			LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "BAS: [CApu2BasInst::ProcInit][error] CreatEqp failed!\n");
			return;
		}
	}
	/*lint -restore*/

	//外设初始化
	m_pcEqp->Init();

	return;
}

/*====================================================================
函数名      : PostEqpMsg
功能        ：向目的发消息
算法实现    ：
引用全局变量：
输入参数说明：const u32 dwMcuNode    Tcp结点
			  const u32 dwDstIId     目的IID
			  CServMsg& cMsg	     消息体
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/29	2.0			国大卫		  创建
====================================================================*/
void CApu2BasInst::PostEqpMsg( const u32 dwMcuNode, const u32 dwDstIId, CServMsg& cMsg,const BOOL32 bIsSendHeadInfo /*= TRUE*/ )
{
	//内-->外
	TransEqpBaseMsgToMcuMsg(cMsg);

	if (bIsSendHeadInfo) 
	{
		post(dwDstIId, cMsg.GetEventId(), cMsg.GetServMsg(), cMsg.GetServMsgLen(), dwMcuNode);
	}
	else
	{
		post(dwDstIId, cMsg.GetEventId(), cMsg.GetMsgBody(), cMsg.GetMsgBodyLen(), dwMcuNode);
	}
	
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "BAS: [CApu2BasInst::PostEqpMsg] BAS.%d Post MSG [%u](%s)(%d).\n", 
							CInstance::GetInsID(), cMsg.GetEventId(), ::OspEventDesc(cMsg.GetEventId()),bIsSendHeadInfo);
}	

/*====================================================================
函数名      ：SetTimer
功能        ：Osp设置定时器
算法实现    ：
引用全局变量：
输入参数说明：u32  dwTimerId       事件号
              long nMilliSeconds   时间间隔
			  u32  dwPara		   携带参数
返回值说明  ：int
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/29	2.0			国大卫		  创建
====================================================================*/
int CApu2BasInst::SetTimer( u32 dwTimerId, long nMilliSeconds, u32 dwPara )
{
	return CInstance::SetTimer(dwTimerId, nMilliSeconds, dwPara);
}

/*====================================================================
函数名      ：KillTimer
功能        ：Osp停止定时器
算法实现    ：
引用全局变量：
输入参数说明：u32 dwTimerId 事件号
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/29	2.0			国大卫		  创建
====================================================================*/
int CApu2BasInst::KillTimer(u32 dwTimerId)
{
	return CInstance::KillTimer(dwTimerId);
}

/*====================================================================
函数名      ：DisConRegister
功能        ：注册断链
算法实现    ：
引用全局变量：
输入参数说明：u32 dwMcuNode  Tcp结点
返回值说明  ：int
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/29	2.0			国大卫		  创建
====================================================================*/
int CApu2BasInst::DisConRegister( u32 dwMcuNode )
{
	return ::OspNodeDiscCBRegQ(dwMcuNode, CInstance::GetAppID(), CInstance::GetInsID());
}

/*====================================================================
函数名      ：IsValidTcpNode
功能        ：校验Tcp结点有效性
算法实现    ：
引用全局变量：
输入参数说明：const u32 dwTcpNode Tcp结点
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/29	2.0			国大卫		  创建
====================================================================*/
BOOL32 CApu2BasInst::IsValidTcpNode(const u32 dwTcpNode)
{
	return OspIsValidTcpNode(dwTcpNode);
}

/*====================================================================
函数名      : DisconnectTcpNode
功能        ：断开一个Tcp结点连接
算法实现    ：
引用全局变量：
输入参数说明：const u32 dwTcpNode Tcp结点
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/29	2.0			国大卫		  创建
====================================================================*/
BOOL32 CApu2BasInst::DisconnectTcpNode(const u32 dwTcpNode)
{
	return OspDisconnectTcpNode(dwTcpNode);
}

/*====================================================================
函数名      ：ConnectTcpNode
功能        ：创建TCP连接
算法实现    ：
引用全局变量：
输入参数说明：const u32 dwConnectIP  连接远端IP
			  const u16 wConnectPort 连接远端端口
返回值说明  ：int
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/29	2.0			国大卫		  创建
====================================================================*/
int CApu2BasInst::ConnectTcpNode(const u32 dwConnectIP, const u16 wConnectPort)
{
	return OspConnectTcpNode(dwConnectIP, wConnectPort);
}

/*====================================================================
函数名      ：GetInsID
功能        ：Osp获取实例号
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：u16  Instance实例号
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/29	2.0			国大卫		  创建
====================================================================*/
u16 CApu2BasInst::GetInsID()
{
	return CInstance::GetInsID();
}

/*====================================================================
函数名      ：TransMcuMsgToEqpBaseMsg
功能        ：将mcu与外设消息转换成EqpBase消息
算法实现    ：
引用全局变量：
输入参数说明：CMessage * const pcMsg
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/29	2.0			国大卫		  修改
====================================================================*/
void CApu2BasInst::TransMcuMsgToEqpBaseMsg(CMessage * const pcMsg)
{
	PTR_NULL_VOID(pcMsg);
	
	switch (pcMsg->event)
	{
	case MCU_BAS_REG_ACK:
		pcMsg->event = MCU_EQP_REG_ACK;
		 break;

	case MCU_BAS_REG_NACK:
		pcMsg->event = MCU_EQP_REG_NACK;
	 	 break;
		 
	case MCU_EQP_GETMSSTATUS_ACK:
		pcMsg->event = MCU_EQP_GETMSSTAT_ACK;
		 break;

	case MCU_EQP_GETMSSTATUS_NACK:
		pcMsg->event = MCU_EQP_GETMSSTAT_NACK;
	 	 break;

	case MCU_EQP_MODSENDADDR_CMD:
		pcMsg->event = MCU_EQP_MODIFYADDR_CMD;
		break;

	default:
		break;
	}
	return;
}

/*====================================================================
函数名      ：TransEqpBaseMsgToMcuMsg
功能        ：EqpBase消息转换成mcu与外设通信消息
算法实现    ：
引用全局变量：
输入参数说明：CMessage * const pcMsg
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/29	2.0			国大卫		  创建
====================================================================*/
void CApu2BasInst::TransEqpBaseMsgToMcuMsg(CServMsg& cMsg)
{
	switch(cMsg.GetEventId())
	{
	case EQP_MCU_REG_REQ:
		{
			cMsg.SetEventId(BAS_MCU_REG_REQ);
			break;
		}
	case EQP_MCU_GETMSSTAT_REQ:
		{
			cMsg.SetEventId(EQP_MCU_GETMSSTATUS_REQ);
			break;
		}

	default:
		break;
	}
	return;
}

/*====================================================================
函数名      ：DaemonProcPrint
功能        ：通用打印发送
算法实现    ：
引用全局变量：
输入参数说明：CMessage * const pcMsg
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/29	2.0			国大卫		  创建
====================================================================*/
void CApu2BasInst::DaemonProcPrint(CMessage * const pcMsg)
{
	PTR_NULL_VOID(pcMsg);

	u8 byInsID = *(u8*)(pcMsg->content);
	if (byInsID > MAXNUM_APU2_BAS)
	{
		return;
	}
	u8 byPrtIns = (byInsID == 0)? MAXNUM_APU2_BAS : byInsID;
	u8 byIns = (byInsID == 0)? 1 : byInsID;

	for (byIns; byIns <= byPrtIns; byIns++)
	{
		post(MAKEIID(GetAppID(), byIns), pcMsg->event);
	}
}
