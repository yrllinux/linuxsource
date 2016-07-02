/*****************************************************************************
   模块名      : 重传(Packet Resend)实现
   文件名      : mcuprsinst.cpp
   创建时间    : 2003年 12月 4日
   实现功能    : include : /include/mcuprsinst.h
   作者        : zhangsh
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   04/3/25	   1.0	       zhangsh	   create
   06/03/21    4.0         张宝卿      代码优化  
******************************************************************************/
#include "mcuprsinst.h"
/*lint -save -e766*/
#include "boardagent.h"
#include "mcuver.h"

CMcuPrsApp g_cMcuPrsApp;

s32  g_nprslog = 1;

/*=============================================================================
  函 数 名： prslog
  功    能： 打印
  算法实现： 
  全局变量： 
  参    数： char* fmt
             ...
  返 回 值： void 
=============================================================================*/
/*lint -save -e438 -e530* -e765 -e1055 -e526 -e628*/
// void prslog( s8* pszFmt, ...)
// {
//     s8 achPrintBuf[255];
//     s32  nBufLen = 0;
//     va_list argptr;
//     if( g_nprslog == 1 )
//     {		  
//         nBufLen = sprintf( achPrintBuf, "[Prs]: " ); 
//         va_start( argptr, pszFmt );
//         nBufLen += vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
//         va_end(argptr); 
//         OspPrintf( TRUE, FALSE, achPrintBuf ); 
//     }
// }
/*=============================================================================
  函 数 名： StrOfIPPrt
  功    能： 打印Ip
  算法实现： 
  全局变量： 
  参    数：  u32 dwIP
             u16 wPort
  返 回 值： void 
=============================================================================*/
void StrOfIPPrt( u32 dwIP, u16 wPort )
{
	s8 achIP[18];
	
	u32 l = dwIP;    
	sprintf(achIP,"%d.%d.%d.%d%c",(l>>24)&0xFF,(l>>16)&0xFF,(l>>8)&0xFF,l&0xFF,0);
  	LogPrint(LOG_LVL_KEYSTATUS, MID_PRSEQP_COMMON,"%s", achIP);
	LogPrint(LOG_LVL_KEYSTATUS, MID_PRSEQP_COMMON," : %u\n", wPort);

	return ;
}

/*=============================================================================
  函 数 名： InitNetBuf
  功    能： 初始化NetBuf
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： BOOL 
=============================================================================*/
BOOL CMcuPrsInst::InitNetBuf()
{
	u16 wRet = nbInit();	// netbuf启动
	if (NB_NO_ERROR != wRet)
	{
		LogPrint(LOG_LVL_ERROR, MID_PRSEQP_COMMON,"[Error] Init error, errcode: %u\n", wRet );
		printf( "[Error] Init error, errcode: %u\n", wRet );
        return FALSE;
	}

	for (s16 wLoop = 0; wLoop < MAXNUM_PRS_CHNNL; wLoop++)// 内部保存状态
	{
		tNetBufInfo[wLoop].Init();
		m_tPrsStatus.m_tStatus.tPrs.m_tPerChStatus[wLoop].SetReserved(FALSE);
		m_tPrsStatus.m_tStatus.tPrs.m_tPerChStatus[wLoop].SetIdle();
		m_sCreated[wLoop] = 0;
	}
    return TRUE;

}

CMcuPrsInst::CMcuPrsInst()
{
    for( s32 nIndex = 0; nIndex < MAXNUM_PRS_CHNNL; nIndex ++ )
    {
        m_sCreated[nIndex] = 0;
    }
    memset( &m_tPrsStatus, 0, sizeof(TPeriEqpStatus) );
    memset( &m_tRSParam, 0, sizeof(m_tRSParam) );
}

/*lint -save -e1551*/
CMcuPrsInst::~CMcuPrsInst()
{
	u16 wRetcode = 0;
	for (s16 wLoop = 0; wLoop < MAXNUM_PRS_CHNNL; wLoop++)
	{
		tNetBufInfo[wLoop].Init();
		if (m_tPrsStatus.m_tStatus.tPrs.m_tPerChStatus[wLoop].IsUsed() && m_pNetBuf[wLoop] != NULL)
        {
			m_pNetBuf[wLoop]->FreeNetBuf();
        }
		m_pNetBuf[wLoop] = NULL;
	}

	wRetcode = nbRelease();//释放
	if (NB_NO_ERROR != wRetcode)
	{
		OspPrintf(TRUE, FALSE, "[Prs] Release error, errcode: %u\n", wRetcode);
	}
}
/*lint -restore*/

/*====================================================================
	函数  : InstanceEntry
	功能  : 消息分发
	输入  : CMessage
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   04/3/25	   v1.0	      zhangsh	    create
====================================================================*/
void CMcuPrsInst::InstanceEntry( CMessage *const pMsg )
{
    CServMsg cServMsg(pMsg->content ,pMsg->length);

	if (pMsg == NULL)
	{
		LogPrint( LOG_LVL_ERROR, MID_PRSEQP_COMMON ,"[MCU Prs Recv] NULL message received.\n");
		return ;
	}

	if ( EV_MAINTASKSCAN_REQ != pMsg->event )
    {
		LogPrint( LOG_LVL_DETAIL, MID_PRSEQP_COMMON ,"begin [InstanceEntry] Message %u(%s).\n", pMsg->event ,::OspEventDesc( pMsg->event ));
	}

	switch (pMsg->event)
    {
	//--------内部消息----------
        
	//断链处理
	case OSP_DISCONNECT:
		Disconnect(pMsg);
		break;
    // 连接
    case EV_CONNECT_TIMER:
        ProcConnectTimeOut(TRUE);
        break;
    case EV_CONNECT_TIMERB:
        ProcConnectTimeOut(FALSE);
        break;
	// 注册
    case EV_REGISTER_TIMER:
        ProcRegisterTimeOut(TRUE);
        break;
    case EV_REGISTER_TIMERB:
        ProcRegisterTimeOut(FALSE);
        break;

	case EV_MAINTASKSCAN_REQ:
		ProcMainTaskScanReq(pMsg);
		break;

	//--------外部消息------------
	// 启动初始化
	case EV_PRS_INIT:
		Init( pMsg );
		break;

	//MCU 通知会话 重联MPCB 相应实例, zgc, 2007/04/30
	case MCU_EQP_RECONNECTB_CMD:
		ProcReconnectBCmd( pMsg );
		break;

	// 维护信息
	case MCU_EQP_ROUNDTRIP_REQ:
		SendMsgToMcu( EQP_MCU_ROUNDTRIP_ACK, &cServMsg );
		break;

	// 注册应答消息
	case MCU_PRS_REGISTER_ACK:
		MsgRegAckProc( pMsg );
		break;

	// 注册拒绝消息
	case MCU_PRS_REGISTER_NACK:
		MsgRegNackProc( pMsg );
		break;

	// 设置接收源信息
	case MCU_PRS_SETSRC_REQ:
		MsgSetSource( pMsg );
		break;

	// 清除所有的通道
	case MCU_PRS_REMOVEALL_REQ:
		MsgRemvoeAllCh( pMsg);
		break;
	// 主备倒换状态
	case TIME_GET_MSSTATUS:
	case MCU_EQP_GETMSSTATUS_ACK:
		ProcGetMsStatusRsp(pMsg);
		break;

	// 打印状态
	case EV_PRS_STATUS:
		MsgPrintStatus();
		break;
        
	// 未定义的消息
	default:
	    LogPrint( LOG_LVL_ERROR, MID_PRSEQP_COMMON, "[Prs]Unexcpet Message %u(%s).\n", pMsg->event, ::OspEventDesc( pMsg->event ));
		break;
	}

	if ( EV_MAINTASKSCAN_REQ != pMsg->event )
    {
		LogPrint( LOG_LVL_DETAIL, MID_PRSEQP_COMMON ,"end [InstanceEntry] Message %u(%s).\n", pMsg->event ,::OspEventDesc( pMsg->event ));
	}
}

/*====================================================================
	函数  : Init
	功能  : 初始化
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    04/3/25	    v1.0	      zhangsh	    create
====================================================================*/
void CMcuPrsInst::Init(CMessage * const pMsg)
{
    if(NULL == pMsg)
    {
        LogPrint(LOG_LVL_ERROR, MID_PRSEQP_COMMON, "[Prs] The pointer cannot be Null. (Init)\n");
		printf("[Prs] The pointer cannot be Null. (Init)\n");
        return;
    }

    BOOL bFlag = InitNetBuf();
    if(FALSE == bFlag) // 失败保护处理
    {
		LogPrint(LOG_LVL_ERROR, MID_PRSEQP_COMMON, "Fail to Inital the Net Buf\n");
		printf("Fail to Inital the Net Buf\n");
    }

    g_cMcuPrsApp.m_tPrsCfg = *(TPrsCfg *)pMsg->content;
	// 重传参数
	m_tRSParam.m_wFirstTimeSpan = g_cMcuPrsApp.m_tPrsCfg.m_wFirstTimeSpan;
	m_tRSParam.m_wRejectTimeSpan = g_cMcuPrsApp.m_tPrsCfg.m_wRejectTimeSpan;
	m_tRSParam.m_wSecondTimeSpan = g_cMcuPrsApp.m_tPrsCfg.m_wSecondTimeSpan;
	m_tRSParam.m_wThirdTimeSpan = g_cMcuPrsApp.m_tPrsCfg.m_wThirdTimeSpan;

    LogPrint(LOG_LVL_KEYSTATUS, MID_PRSEQP_COMMON, "Init Params We give is follow:\n" );
    LogPrint(LOG_LVL_KEYSTATUS, MID_PRSEQP_COMMON,"Connect McuId is %u IP and port--- \n",g_cMcuPrsApp.m_tPrsCfg.wMcuId);
    StrOfIPPrt(g_cMcuPrsApp.m_tPrsCfg.dwConnectIP,g_cMcuPrsApp.m_tPrsCfg.wConnectPort);
    LogPrint(LOG_LVL_KEYSTATUS, MID_PRSEQP_COMMON,"Mcu prs alias is %s Local Ip and port---",g_cMcuPrsApp.m_tPrsCfg.achAlias);
    StrOfIPPrt(g_cMcuPrsApp.m_tPrsCfg.dwLocalIP,g_cMcuPrsApp.m_tPrsCfg.wRcvStartPort);
    LogPrint(LOG_LVL_KEYSTATUS, MID_PRSEQP_COMMON,"Eqptype %u and EqpId %u\n", g_cMcuPrsApp.m_tPrsCfg.byEqpType,g_cMcuPrsApp.m_tPrsCfg.byEqpId);
    LogPrint(LOG_LVL_KEYSTATUS, MID_PRSEQP_COMMON,"FirstTimeSpan:%u\n", m_tRSParam.m_wFirstTimeSpan);
    LogPrint(LOG_LVL_KEYSTATUS, MID_PRSEQP_COMMON,"RejectTimeSpan:%u\n",m_tRSParam.m_wRejectTimeSpan );
    LogPrint(LOG_LVL_KEYSTATUS, MID_PRSEQP_COMMON,"SecondTimeSpan:%u\n",m_tRSParam.m_wSecondTimeSpan );
    LogPrint(LOG_LVL_KEYSTATUS, MID_PRSEQP_COMMON,"ThirdTimeSpan:%u\n",m_tRSParam.m_wThirdTimeSpan );


	for (s32 nLoop = 0 ; nLoop < MAXNUM_PRS_CHNNL; nLoop++)
	{
		m_pNetBuf[nLoop] = new CNetBuf();
	}
    
	if( g_cMcuPrsApp.m_tPrsCfg.dwLocalIP == g_cMcuPrsApp.m_tPrsCfg.dwConnectIP )
	{
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
		g_cMcuPrsApp.m_bEmbed = FALSE;
#else
		g_cMcuPrsApp.m_bEmbed = TRUE;
        g_cMcuPrsApp.m_dwMcuNode = 0;
        SetTimer(EV_REGISTER_TIMER, PRS_REGISTER_TIMEOUT);
        LogPrint(LOG_LVL_KEYSTATUS, MID_PRSEQP_COMMON, "Prs embed in Mcu 0x%x\n", g_cMcuPrsApp.m_tPrsCfg.dwConnectIP);
		printf("Prs embed in Mcu 0x%x\n", g_cMcuPrsApp.m_tPrsCfg.dwConnectIP);
#endif
	}
	// 保护
    if(g_cMcuPrsApp.m_tPrsCfg.dwLocalIP == g_cMcuPrsApp.m_tPrsCfg.dwConnectIpB)
    {
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
		g_cMcuPrsApp.m_bEmbed = FALSE; 
#else
		g_cMcuPrsApp.m_bEmbed = TRUE;
        g_cMcuPrsApp.m_dwMcuNodeB = 0;
        SetTimer(EV_REGISTER_TIMERB, PRS_REGISTER_TIMEOUT);
		LogPrint(LOG_LVL_KEYSTATUS, MID_PRSEQP_COMMON, "Prs embed in Mcu 0x%x\n", g_cMcuPrsApp.m_tPrsCfg.dwConnectIpB);
		printf("Prs embed in Mcu 0x%x\n", g_cMcuPrsApp.m_tPrsCfg.dwConnectIpB);
#endif
    }

    if(0 != g_cMcuPrsApp.m_tPrsCfg.dwConnectIP && FALSE == g_cMcuPrsApp.m_bEmbed )
    {
        SetTimer(EV_CONNECT_TIMER, PRS_CONNECT_TIMEOUT);
		LogPrint(LOG_LVL_KEYSTATUS, MID_PRSEQP_COMMON, "prs set EV_CONNECT_TIMER\n");
		printf("prs set EV_CONNECT_TIMER\n");
    }

    if( 0 != g_cMcuPrsApp.m_tPrsCfg.dwConnectIpB && FALSE == g_cMcuPrsApp.m_bEmbedB )
    {
        SetTimer(EV_CONNECT_TIMERB, PRS_CONNECT_TIMEOUT);
		LogPrint(LOG_LVL_KEYSTATUS, MID_PRSEQP_COMMON, "prs set EV_CONNECT_TIMERB\n");
		printf("prs set EV_CONNECT_TIMERB\n");
    }
    return;
}

/*=============================================================================
  函 数 名： ProcConnectTimeOut
  功    能： 处理注册超时
  算法实现： 
  全局变量： 
  参    数： BOOL32 bConnectA
  返 回 值： void 
=============================================================================*/
void CMcuPrsInst::ProcConnectTimeOut(BOOL32 bConnectA)
{
    BOOL32 bRet = FALSE;
    if(TRUE == bConnectA)
    {
		KillTimer(EV_CONNECT_TIMER);
        bRet = ConnectMcu(bConnectA, g_cMcuPrsApp.m_dwMcuNode);
        if(TRUE == bRet)
        {
            SetTimer(EV_REGISTER_TIMER, PRS_REGISTER_TIMEOUT);
        }
        else
        {
            SetTimer(EV_CONNECT_TIMER, PRS_CONNECT_TIMEOUT);
        }
    }
    else
    {
		KillTimer(EV_CONNECT_TIMERB);
        bRet = ConnectMcu(bConnectA, g_cMcuPrsApp.m_dwMcuNodeB);
        if(TRUE == bRet)
        {
            SetTimer(EV_REGISTER_TIMERB, PRS_REGISTER_TIMEOUT);
        }
        else
        {
            SetTimer(EV_CONNECT_TIMERB, PRS_CONNECT_TIMEOUT);
        }
    }
    return;
}
/*====================================================================
	函数  : ConnectMcu
	功能  : 与MCU建链
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   04/3/25	   v1.0	      zhangsh	    create
====================================================================*/
BOOL32 CMcuPrsInst::ConnectMcu(BOOL32 bConnectA, u32& dwMcuNode)
{
	LogPrint(LOG_LVL_KEYSTATUS,MID_PRSEQP_COMMON,"[ConnectMcu] bConnectA.%d!\n", bConnectA);

    BOOL32 bRet = TRUE;
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	if(!OspIsValidTcpNode( dwMcuNode ))
    {
		dwMcuNode = OspConnectTcpNode(g_cMcuPrsApp.m_tPrsCfg.dwConnectIP, g_cMcuPrsApp.m_tPrsCfg.wConnectPort, 10, 0, 100 );  
		
        if (OspIsValidTcpNode(dwMcuNode))
        {
            ::OspNodeDiscCBRegQ(dwMcuNode, GetAppID(),GetInsID());
            LogPrint(LOG_LVL_KEYSTATUS,MID_PRSEQP_COMMON,"Connect to Mcu Success,node is %u!\n", dwMcuNode);
        }
        else
        {
            //建链失败
             LogPrint(LOG_LVL_KEYSTATUS,MID_PRSEQP_COMMON,"[prs8ke] Failed to Connect Mcu :%d\n",dwMcuNode);
			bRet = FALSE;
        }
	}
#else
    if(!OspIsValidTcpNode( dwMcuNode ))
    {
        if(TRUE == bConnectA)
        {  
			dwMcuNode = BrdGetDstMcuNode();
		}
        else
        {
			dwMcuNode = BrdGetDstMcuNodeB(); 
		}

	    if (OspIsValidTcpNode( dwMcuNode ))
	    {
		    ::OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), GetInsID());

			LogPrint(LOG_LVL_KEYSTATUS,MID_PRSEQP_COMMON," Connect MCU Success, node: %d!\n", dwMcuNode);
	    }
        else
        {
            bRet = FALSE;
        }
    }
#endif
    return bRet;
}

/*=============================================================================
  函 数 名： ProcRegisterTimeOut
  功    能： 处理注册超时
  算法实现： 
  全局变量： 
  参    数： BOOL32 bRegisterA
  返 回 值： void 
=============================================================================*/
void CMcuPrsInst::ProcRegisterTimeOut(BOOL32 bRegisterA)
{
    if(TRUE == bRegisterA)
    {
        Register(bRegisterA, g_cMcuPrsApp.m_dwMcuNode);
        SetTimer(EV_REGISTER_TIMER, PRS_REGISTER_TIMEOUT);
    }
    else
    {
        Register(bRegisterA, g_cMcuPrsApp.m_dwMcuNodeB);
        SetTimer(EV_REGISTER_TIMERB, PRS_REGISTER_TIMEOUT);
    }
    return;
}

/*=============================================================================
函 数 名： ProcMainTaskScanReq
功    能： 
算法实现： 
全局变量： 
参    数： CMessage* const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/4/7   4.0		周广程                  创建
=============================================================================*/
void CMcuPrsInst::ProcMainTaskScanReq( CMessage* const pcMsg )
{
	post( pcMsg->srcid, EV_MAINTASKSCAN_ACK );
}

/*====================================================================
	函数名  ：Register
	功能    ：向MCU注册函数
	输入    ：无
	输出    ：无
	返回值  ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   04/3/25	   v1.0	      zhangsh	    create
====================================================================*/
void CMcuPrsInst::Register(BOOL32 bRegisterA, u32 dwMcuNode)
{
	CServMsg cSvrMsg;
	TPeriEqpRegReq tReg;
	memset( &tReg, 0, sizeof(tReg) );
	
	tReg.SetEqpId( g_cMcuPrsApp.m_tPrsCfg.byEqpId );

	tReg.SetEqpType( g_cMcuPrsApp.m_tPrsCfg.byEqpType );
	tReg.SetEqpAlias( g_cMcuPrsApp.m_tPrsCfg.achAlias );
	tReg.SetPeriEqpIpAddr( g_cMcuPrsApp.m_tPrsCfg.dwLocalIP );
	tReg.SetStartPort( g_cMcuPrsApp.m_tPrsCfg.wRcvStartPort );
	tReg.SetChnnlNum( MAXNUM_PRS_CHNNL );
    tReg.SetVersion(DEVVER_PRS);
	
    if(TRUE == bRegisterA)
    {
        tReg.SetMcuId( (u8)g_cMcuPrsApp.m_tPrsCfg.wMcuId );
    }
    else
    {
        tReg.SetMcuId( (u8)g_cMcuPrsApp.m_tPrsCfg.wMcuId );
    }
    cSvrMsg.SetMsgBody( (u8*)&tReg ,sizeof(tReg) );
  
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	post(MAKEIID(AID_MCU_PERIEQPSSN, CInstance::DAEMON),
		PRS_MCU_REGISTER_REQ,
		cSvrMsg.GetServMsg(),
		cSvrMsg.GetServMsgLen(),
		dwMcuNode);
#else
	post(MAKEIID(AID_MCU_PERIEQPSSN, g_cMcuPrsApp.m_tPrsCfg.byEqpId),
		PRS_MCU_REGISTER_REQ,
		cSvrMsg.GetServMsg(),
		cSvrMsg.GetServMsgLen(),
		dwMcuNode);
#endif

	return;
}

/*====================================================================
	函数名  ：MsgRegAckProc
	功能    ：MCU注册应答消息处理函数
	输入    ：消息
	输出    ：无
	返回值  ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   04/3/25	   v1.0	      zhangsh	    create
====================================================================*/
void CMcuPrsInst::MsgRegAckProc( CMessage * const pMsg )
{
	if (NULL == pMsg->content)
    {
        LogPrint(LOG_LVL_ERROR,MID_PRSEQP_COMMON, "[Error] The pointer cannot be Null !\n");
        return ;
    }
    
    TPeriEqpRegAck tRegAck;
	memset(&tRegAck, 0, sizeof(TPeriEqpRegAck));

	CServMsg cServMsg(pMsg->content, pMsg->length);
	tRegAck = *(TPeriEqpRegAck*)cServMsg.GetMsgBody();

	if(pMsg->srcnode == g_cMcuPrsApp.m_dwMcuNode)// 注册成功
    {
        g_cMcuPrsApp.m_dwMcuIId = pMsg->srcid;
        g_cMcuPrsApp.m_byRegAckNum++;
        KillTimer(EV_REGISTER_TIMER);
    }
    else if(pMsg->srcnode == g_cMcuPrsApp.m_dwMcuNodeB)
    {
        g_cMcuPrsApp.m_dwMcuIIdB = pMsg->srcid;
        g_cMcuPrsApp.m_byRegAckNum++;
        KillTimer(EV_REGISTER_TIMERB);
    }

	//[nizhijun 2011/02/17]prs 8ke 外置化 参数需要注册后才传递给prs。
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	TPrsTimeSpan tPrsTimeSpan;
	tPrsTimeSpan = *(TPrsTimeSpan*)(cServMsg.GetMsgBody() + sizeof(TEqpRegAck));
	m_tRSParam.m_wFirstTimeSpan  = ntohs(tPrsTimeSpan.m_wFirstTimeSpan);
	m_tRSParam.m_wSecondTimeSpan = ntohs(tPrsTimeSpan.m_wSecondTimeSpan);
	m_tRSParam.m_wThirdTimeSpan  = ntohs(tPrsTimeSpan.m_wThirdTimeSpan);
	m_tRSParam.m_wRejectTimeSpan = ntohs(tPrsTimeSpan.m_wRejectTimeSpan);
	
	LogPrint(LOG_LVL_KEYSTATUS,MID_PRSEQP_COMMON,"FirstTimeSpan:%u\n", m_tRSParam.m_wFirstTimeSpan);
    LogPrint(LOG_LVL_KEYSTATUS,MID_PRSEQP_COMMON,"RejectTimeSpan:%u\n",m_tRSParam.m_wRejectTimeSpan );
    LogPrint(LOG_LVL_KEYSTATUS,MID_PRSEQP_COMMON,"SecondTimeSpan:%u\n",m_tRSParam.m_wSecondTimeSpan );
    LogPrint(LOG_LVL_KEYSTATUS,MID_PRSEQP_COMMON,"ThirdTimeSpan:%u\n",m_tRSParam.m_wThirdTimeSpan );
	
#endif

	// guzh [6/12/2007] 校验会话参数
    if ( g_cMcuPrsApp.m_dwMpcSSrc == 0 )
    {
        g_cMcuPrsApp.m_dwMpcSSrc = tRegAck.GetMSSsrc();
    }
    else
    {
        // 异常情况，断开两个节点
        if ( g_cMcuPrsApp.m_dwMpcSSrc != tRegAck.GetMSSsrc() )
        {
            LogPrint(LOG_LVL_ERROR,MID_PRSEQP_COMMON,"[MsgRegAckProc] MPC SSRC ERROR(%u<-->%u), Disconnect Both Nodes!\n", 
                      g_cMcuPrsApp.m_dwMpcSSrc, tRegAck.GetMSSsrc());
            if ( OspIsValidTcpNode(g_cMcuPrsApp.m_dwMcuNode) )
            {
                OspDisconnectTcpNode(g_cMcuPrsApp.m_dwMcuNode);
            }
            if ( OspIsValidTcpNode(g_cMcuPrsApp.m_dwMcuNodeB) )
            {
                OspDisconnectTcpNode(g_cMcuPrsApp.m_dwMcuNodeB);
            }      
            return;
        }
    }

	if(FIRST_REGACK == g_cMcuPrsApp.m_byRegAckNum) // 因为断链而重新注册，不重新置状态
    {
        g_cMcuPrsApp.m_wLocalPort = tRegAck.GetMcuStartPort();
        m_tPrsStatus.m_byOnline = TRUE;
        NEXTSTATE( NORMAL );
    }
	SendStatusChangeMsg( );

	return;
}

/*====================================================================
	函数名  ：MsgRegNackProc
	功能    ：MCU注册拒绝消息处理函数
	输入    ：消息
	输出    ：无
	返回值  ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   04/3/25	   v1.0	      zhangsh	    create
====================================================================*/
void CMcuPrsInst::MsgRegNackProc(CMessage* const pMsg)
{
	if (NULL == pMsg)
	{
		LogPrint(LOG_LVL_ERROR,MID_PRSEQP_COMMON,"[MsgRegNackProc]pMsg is null!\n" );
	}
	LogPrint(LOG_LVL_ERROR,MID_PRSEQP_COMMON,"Register to MCU be refused .retry...\n" );
	return;
}

/*====================================================================
	函数名  ：SendMsgToMcu
	功能    ：向MCU发送消息
	输入    ：wEvent - 事件
			  
	输出    ：无
	返回值  ：成功返回 TRUE，否则返回FALSE
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   04/3/25	   v1.0	      zhangsh	    create
====================================================================*/
BOOL CMcuPrsInst::SendMsgToMcu(u16 wEvent, CServMsg * const pcServMsg)
{
	LogPrint(LOG_LVL_DETAIL,MID_PRSEQP_COMMON,"prs Send Message %u(%s).\n", wEvent ,::OspEventDesc( wEvent ));
	
    if ( g_cMcuPrsApp.m_bEmbed || OspIsValidTcpNode( g_cMcuPrsApp.m_dwMcuNode ))
	{
        post( g_cMcuPrsApp.m_dwMcuIId, wEvent, pcServMsg->GetServMsg(), pcServMsg->GetServMsgLen(), g_cMcuPrsApp.m_dwMcuNode );
        LogPrint(LOG_LVL_DETAIL,MID_PRSEQP_COMMON,"prs Send message to A Node: %d\n", g_cMcuPrsApp.m_dwMcuNode);
	}

	if(g_cMcuPrsApp.m_bEmbedB || OspIsValidTcpNode(g_cMcuPrsApp.m_dwMcuNodeB))
	{
		post( g_cMcuPrsApp.m_dwMcuIIdB, wEvent, pcServMsg->GetServMsg(), pcServMsg->GetServMsgLen(), g_cMcuPrsApp.m_dwMcuNodeB );
        LogPrint(LOG_LVL_DETAIL,MID_PRSEQP_COMMON,"prs Send message to B Node: %d\n", g_cMcuPrsApp.m_dwMcuNodeB);
	}
    return TRUE;

}

/*====================================================================
	函数名  ：Disconnect
	功能    ：断链函数
	输入    ：无
	输出    ：无
	返回值  ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   04/3/25	   v1.0	      zhangsh	    create
====================================================================*/
void CMcuPrsInst::Disconnect(CMessage *const pMsg)
{

    u32 dwNode = *(u32*)pMsg->content;
    
    if (INVALID_NODE != dwNode)
    {
        OspDisconnectTcpNode(dwNode);
    }
    if(dwNode == g_cMcuPrsApp.m_dwMcuNode)
    {
        g_cMcuPrsApp.FreeStatusDataA();
        SetTimer(EV_CONNECT_TIMER, PRS_CONNECT_TIMEOUT);
    }
    else if(dwNode == g_cMcuPrsApp.m_dwMcuNodeB)
    {
        g_cMcuPrsApp.FreeStatusDataB();
        SetTimer(EV_CONNECT_TIMERB, PRS_CONNECT_TIMEOUT);
    }

	if( INVALID_NODE != g_cMcuPrsApp.m_dwMcuNode || INVALID_NODE != g_cMcuPrsApp.m_dwMcuNodeB )
	{
		if (OspIsValidTcpNode(g_cMcuPrsApp.m_dwMcuNode))
		{
			// 是否考虑延时一会待Mcu稳定后再发
			post( g_cMcuPrsApp.m_dwMcuIId, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cMcuPrsApp.m_dwMcuNode );     
			LogPrint( LOG_LVL_KEYSTATUS, MID_PRSEQP_COMMON ,"[MsgDisconnectProc] GetMsStatusReq. McuNode.A\n");
		}
		else if (OspIsValidTcpNode(g_cMcuPrsApp.m_dwMcuNodeB))
		{
			post( g_cMcuPrsApp.m_dwMcuIIdB, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cMcuPrsApp.m_dwMcuNodeB );        
			LogPrint( LOG_LVL_KEYSTATUS, MID_PRSEQP_COMMON ,"[MsgDisconnectProc] GetMsStatusReq. McuNode.B\n");
		}
		// 等待指定时间
		SetTimer(TIME_GET_MSSTATUS, WAITING_MSSTATUS_TIMEOUT);
		return;
	}

	// 同时断链
    if(INVALID_NODE == g_cMcuPrsApp.m_dwMcuNode && INVALID_NODE == g_cMcuPrsApp.m_dwMcuNodeB)
    {
	    ClearAllChannelStatus();
    }
	return ;
}

/*=============================================================================
函 数 名： ProcReconnectBCmd
功    能： 
算法实现： 
全局变量： 
参    数： CMessage* const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/4/30   4.0			周广程                  创建
=============================================================================*/
void CMcuPrsInst::ProcReconnectBCmd( CMessage* const pcMsg )
{
	if( pcMsg == NULL )
	{
		return;
	}
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u32 dwMpcBIp = *(u32*)cServMsg.GetMsgBody();
	dwMpcBIp = ntohl(dwMpcBIp);
	g_cMcuPrsApp.m_tPrsCfg.dwConnectIpB = dwMpcBIp;
	

	// 如果已连接, 先断开
	if( OspIsValidTcpNode( g_cMcuPrsApp.m_dwMcuNodeB ) )
	{
		OspDisconnectTcpNode( g_cMcuPrsApp.m_dwMcuNodeB );
	}

	g_cMcuPrsApp.FreeStatusDataB();
    if( 0 != g_cMcuPrsApp.m_tPrsCfg.dwConnectIpB && FALSE == g_cMcuPrsApp.m_bEmbedB )
    {
        SetTimer(EV_CONNECT_TIMERB, PRS_CONNECT_TIMEOUT);
    }
	else
	{
		SetTimer(EV_REGISTER_TIMERB, PRS_REGISTER_TIMEOUT);
	}
}
/*=============================================================================
  函 数 名： ClearAllChannelStatus
  功    能： 清空所有通道状态
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CMcuPrsInst::ClearAllChannelStatus(void)
{
	// 初始化所有通道
    s32 nChNum = 0;
	for ( nChNum = 0; nChNum < MAXNUM_PRS_CHNNL; nChNum++)
	{
		tNetBufInfo[nChNum].Init();
		if (m_tPrsStatus.m_tStatus.tPrs.m_tPerChStatus[nChNum].IsUsed() && m_pNetBuf[nChNum] != NULL )
		{
			m_pNetBuf[nChNum]->SetLocalAddr(NULL);
			m_pNetBuf[nChNum]->SetChannelLocalAddr(NULL);
		}
	}
	// 清空保留的状态
	for ( nChNum = 0 ;nChNum < MAXNUM_PRS_CHNNL; nChNum++)
	{
		m_tPrsStatus.m_byOnline = FALSE;
	    m_tPrsStatus.m_tStatus.tPrs.m_tPerChStatus[nChNum].SetReserved(FALSE);
		m_tPrsStatus.m_tStatus.tPrs.m_tPerChStatus[nChNum].SetIdle();
	}
    NEXTSTATE( IDLE );
    g_cMcuPrsApp.m_byRegAckNum = 0;
	g_cMcuPrsApp.m_dwMpcSSrc = 0;
	return;
}

/*=============================================================================
  函 数 名： ProcGetMsStatusRsp
  功    能： 处理取主备倒换消息
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
=============================================================================*/
void CMcuPrsInst::ProcGetMsStatusRsp(CMessage* const pMsg)
{
	BOOL bSwitchOk = FALSE;
	CServMsg cServMsg(pMsg->content, pMsg->length);
	if( MCU_EQP_GETMSSTATUS_ACK == pMsg->event )
	{
		TMcuMsStatus *ptMsStatus = (TMcuMsStatus *)cServMsg.GetMsgBody();
        
        KillTimer(TIME_GET_MSSTATUS);
        LogPrint(LOG_LVL_KEYSTATUS,MID_PRSEQP_COMMON,"[ProcGetMsStatusRsp]. receive msg MCU_EQP_GETMSSTATUS_ACK. IsMsSwitchOK :%d\n", 
                  ptMsStatus->IsMsSwitchOK());

        if(ptMsStatus->IsMsSwitchOK()) // 倒换成功
        {
            bSwitchOk = TRUE;
        }
	}
	// 倒换失败或者超时
	if( !bSwitchOk )
	{
		//20110914 zjl 主备倒换失败 先清状态  再断链 再连接
		ClearAllChannelStatus();
		LogPrint(LOG_LVL_DETAIL,MID_PRSEQP_COMMON,"[ProcGetMsStatusRsp]ClearAllChannelStatus!\n");

		if (OspIsValidTcpNode(g_cMcuPrsApp.m_dwMcuNode))
		{
			LogPrint(LOG_LVL_DETAIL,MID_PRSEQP_COMMON,"[ProcGetMsStatusRsp] OspDisconnectTcpNode A!\n");	
			OspDisconnectTcpNode(g_cMcuPrsApp.m_dwMcuNode);
		}
		if (OspIsValidTcpNode(g_cMcuPrsApp.m_dwMcuNodeB))
		{
			LogPrint(LOG_LVL_DETAIL,MID_PRSEQP_COMMON,"[ProcGetMsStatusRsp] OspDisconnectTcpNode B!\n");	
			OspDisconnectTcpNode(g_cMcuPrsApp.m_dwMcuNodeB);
		}

		if( INVALID_NODE == g_cMcuPrsApp.m_dwMcuNode )
		{
			LogPrint(LOG_LVL_DETAIL,MID_PRSEQP_COMMON,"[ProcGetMsStatusRsp] EV_CONNECT_TIMER!\n");	
			SetTimer(EV_CONNECT_TIMER, PRS_CONNECT_TIMEOUT);
		}
		if( INVALID_NODE == g_cMcuPrsApp.m_dwMcuNodeB )
		{
			LogPrint(LOG_LVL_DETAIL,MID_PRSEQP_COMMON,"[ProcGetMsStatusRsp] EV_CONNECT_TIMERB!\n");	
			SetTimer(EV_CONNECT_TIMERB, PRS_CONNECT_TIMEOUT);
		}
	}
	return;
}
/*====================================================================
	函数名  :MsgSetSource
	功能    :设置重传源
	输入    :消息
	输出    :无
	返回值  :无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   04/3/25	   v1.0	      zhangsh	    create
====================================================================*/
void CMcuPrsInst::MsgSetSource( CMessage * const pMsg )
{

	u16 wRetcode;
	TPrsParam tParam;
	TNetLocalAddr tNetLocalAddr;
	TNetAddr tNetAddr;
	CServMsg cServMsg( pMsg->content, pMsg->length );

	u8 byPos = cServMsg.GetChnIndex();
	memset( &tParam, 0, sizeof(tParam) );
	memset( &tNetLocalAddr, 0, sizeof(TNetLocalAddr) );
	memset( &tNetAddr, 0, sizeof(TNetAddr) );
	if ( byPos >= MAXNUM_PRS_CHNNL )
	{
		LogPrint(LOG_LVL_ERROR,MID_PRSEQP_COMMON,"[MCU PRS]:Current Channel Index Error:%u\n", byPos );
		return ;
	}

	//state check
	switch (CurState())
	{
	case NORMAL:
		break;
	default:
		LogPrint(LOG_LVL_ERROR,MID_PRSEQP_COMMON,"[MCU PRS]Error State:%d\n", CurState() );
		return;
	}

	//tNetAddr.m_dwIP = g_cMcuPrsApp.m_tPrsCfg.dwLocalIP;
	//tNetAddr.m_wPort = g_cMcuPrsApp.m_tPrsCfg.wRcvStartPort + 4*byPos + 2;
	cServMsg.GetMsgBody((u8 *)&tParam, sizeof(tParam));
    tNetAddr.m_dwIP = g_cMcuPrsApp.m_tPrsCfg.dwLocalIP;
	tNetAddr.m_wPort = tParam.m_tLocalAddr.GetPort() + 2;
	memcpy(&tNetBufInfo[byPos].addr, &tNetAddr, sizeof(TNetAddr));

	tNetLocalAddr.m_tLocalAddr.m_dwIP = htonl(tParam.m_tLocalAddr.GetIpAddr());  // 本地接受Rtp包端口
	tNetLocalAddr.m_tLocalAddr.m_wPort = tParam.m_tLocalAddr.GetPort();
	tNetLocalAddr.m_tRtcpBackAddr.m_dwIP = htonl(tParam.m_tRemoteAddr.GetIpAddr()); // 发送Rtcp包端口
	tNetLocalAddr.m_tRtcpBackAddr.m_wPort = tParam.m_tRemoteAddr.GetPort();
	
	LogPrint(LOG_LVL_DETAIL,MID_PRSEQP_COMMON,"tNetAddr.m_wPort = %d\n", tNetAddr.m_wPort);
	LogPrint(LOG_LVL_DETAIL,MID_PRSEQP_COMMON,"Channel-%u prs source change!\n RTP---", byPos);
	StrOfIPPrt(tParam.m_tLocalAddr.GetIpAddr(), tParam.m_tLocalAddr.GetPort());
	LogPrint(LOG_LVL_DETAIL,MID_PRSEQP_COMMON,"RTCP---");
	StrOfIPPrt(tParam.m_tRemoteAddr.GetIpAddr(),tParam.m_tRemoteAddr.GetPort());

	// 未创建成功，拒绝
	if (0 == m_sCreated[byPos])
	{
		if (NULL == m_pNetBuf[byPos])
        {
			m_pNetBuf[byPos] = new CNetBuf;
        }
		wRetcode = m_pNetBuf[byPos]->CreateNetBuf(m_tRSParam, g_cMcuPrsApp.m_tPrsCfg.wRcvStartPort - 4*byPos -4);
		if ( NB_NO_ERROR != wRetcode)
		{
		   // 该重传结构没有创建

			LogPrint(LOG_LVL_ERROR,MID_PRSEQP_COMMON,"The NetBuf%u Create Error,ErrCode=%u!\n",byPos ,wRetcode);

			SendMsgToMcu(PRS_MCU_SETSRC_NACK, &cServMsg);
			return ;
		}
		else
		{
			//添加了一个，本地存储加一个
			m_sCreated[byPos] = 1;
		}
	}

	//创建成功
	if (tParam.m_tRemoteAddr.m_dwIpAddr == 0)
    {
		m_pNetBuf[byPos]->SetLocalAddr(NULL);
    }
	else
    {
		m_pNetBuf[byPos]->SetLocalAddr( &tNetLocalAddr );
    }

	m_pNetBuf[byPos]->SetChannelLocalAddr( &tNetAddr );  // 接受Rtcp包的地址
	memcpy( &tNetBufInfo[byPos].local, &tNetLocalAddr, sizeof(TNetLocalAddr) );
	SendMsgToMcu( PRS_MCU_SETSRC_ACK, &cServMsg );

    m_tPrsStatus.m_tStatus.tPrs.m_tPerChStatus[byPos].SetUsed();
	SendStatusChangeMsg();
}

/*====================================================================
	函数  : MsgRemvoeAllCh
	功能  : 拆除所有
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   04/3/25	   v1.0	      zhangsh	    create
====================================================================*/
void CMcuPrsInst::MsgRemvoeAllCh( CMessage * const pMsg )
{
	CServMsg cServMsg( pMsg->content ,pMsg->length );
	u8 byPos = cServMsg.GetChnIndex();

	LogPrint(LOG_LVL_KEYSTATUS,MID_PRSEQP_COMMON,"Chl-%u prs remove all!\n", byPos );


	if (!m_tPrsStatus.m_tStatus.tPrs.m_tPerChStatus[byPos].IsUsed())
	{
		//该重传结构没有创建,拒绝
		LogPrint(LOG_LVL_ERROR,MID_PRSEQP_COMMON,"Chl-%u does not use, nack!\n", byPos );

		SendMsgToMcu( PRS_MCU_REMOVEALL_NACK, &cServMsg );
		return ;
	}

	u16 wRet = m_pNetBuf[byPos]->FreeNetBuf(); // 释放
	m_sCreated[byPos] = 0;
	if ( NB_NO_ERROR != wRet)
	{

		LogPrint(LOG_LVL_ERROR,MID_PRSEQP_COMMON,"[Error]: Remove Resend Channel Failed:%u, return code: %d\n", byPos, wRet );

		SendMsgToMcu( PRS_MCU_REMOVEALL_NACK, &cServMsg );
		return ;
	}

    tNetBufInfo[byPos].Init();    /*add by libo*/
	SendMsgToMcu( PRS_MCU_REMOVEALL_ACK, &cServMsg );

	m_tPrsStatus.m_tStatus.tPrs.m_tPerChStatus[byPos].SetIdle();
	SendStatusChangeMsg();

	return ;
}

/*====================================================================
	函数  : SendStatusChangeMsg
	功能  : 状态改变后通知MCU
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   04/3/25	   v1.0	      zhangsh	    create
====================================================================*/
void CMcuPrsInst::SendStatusChangeMsg()
{
	CServMsg cServMsg;

	m_tPrsStatus.SetAlias( g_cMcuPrsApp.m_tPrsCfg.achAlias );
	// m_tPrsStatus.SetEqpId( g_cMcuPrsApp.m_tPrsCfg.byEqpId );
	// m_tPrsStatus.SetEqpType( g_cMcuPrsApp.m_tPrsCfg.byEqpType );
	// m_tPrsStatus.SetMcu( (u8)g_cMcuPrsApp.m_tPrsCfg.wMcuId );
    m_tPrsStatus.SetMcuEqp((u8)g_cMcuPrsApp.m_tPrsCfg.wMcuId, g_cMcuPrsApp.m_tPrsCfg.byEqpId, g_cMcuPrsApp.m_tPrsCfg.byEqpType);
        
	cServMsg.SetMsgBody( (u8 *)&(m_tPrsStatus), sizeof(TPeriEqpStatus) );

	SendMsgToMcu( PRS_MCU_PRSSTATUS_NOTIF, &cServMsg );

	return ;
}

/*====================================================================
	函数  : MsgPrintStatus
	功能  : 打印通道状态
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   04/3/25	   v1.0	      zhangsh	    create
====================================================================*/
void CMcuPrsInst::MsgPrintStatus()
{
    OspPrintf(TRUE, FALSE, "The group is follow(max group is %u):\n", MAXNUM_PRS_CHNNL);
    for ( s32 nLoop = 0; nLoop < MAXNUM_PRS_CHNNL; nLoop++ )
    {
        OspPrintf( TRUE, FALSE, "Group-%u  ", nLoop);
        OspPrintf( TRUE, FALSE, "Receive RTP from ");
        StrOfIPPrt( ntohl(tNetBufInfo[nLoop].local.m_tLocalAddr.m_dwIP),tNetBufInfo[nLoop].local.m_tLocalAddr.m_wPort );
        OspPrintf( TRUE, FALSE, "   Send RTCP to ");
        StrOfIPPrt( ntohl(tNetBufInfo[nLoop].local.m_tRtcpBackAddr.m_dwIP), tNetBufInfo[nLoop].local.m_tRtcpBackAddr.m_wPort );
        OspPrintf( TRUE, FALSE, "\n");
        OspPrintf( TRUE, FALSE, "Receive Feedback RTCP from ");
        StrOfIPPrt( ntohl(tNetBufInfo[nLoop].addr.m_dwIP), tNetBufInfo[nLoop].addr.m_wPort );
        OspPrintf( TRUE, FALSE, "\n");
    }
}

CPrsCfg::CPrsCfg()
{
    g_cMcuPrsApp.FreeStatusDataA();
    g_cMcuPrsApp.FreeStatusDataB();
    m_bEmbed  = FALSE; 
    m_bEmbedB = FALSE;
    m_byRegAckNum = 0;
	m_dwMpcSSrc = 0;
    m_wLocalPort = 0;
    memset( &m_tPrsCfg, 0, sizeof(TPrsCfg) );
}

CPrsCfg::~CPrsCfg()
{
}

/*=============================================================================
  函 数 名： FreeStatusDataA
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CPrsCfg::FreeStatusDataA()
{
    m_dwMcuNode = INVALID_NODE;
    m_dwMcuIId  = INVALID_INS;
    return;
}

/*=============================================================================
  函 数 名： FreeStatusDataB
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CPrsCfg::FreeStatusDataB()
{
    m_dwMcuNodeB = INVALID_NODE;
    m_dwMcuIIdB  = INVALID_INS;
    return;
}
