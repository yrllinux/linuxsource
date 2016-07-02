/*****************************************************************************
   模块名      : MP
   文件名      : mpinst.cpp
   相关文件    : mp.h
   文件实现功能: MP的业务逻辑实现
   作者        : 胡昌威
   版本        : V4.0  Copyright(C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期       版本        修改人          修改内容
   2007/09/24   4.0         张宝卿          创建
******************************************************************************

  说明：为增强代码可读性，广播交换逻辑优化新增部分的逻辑和缩写特作说明。
  
    1. 原交换策略: MT1/EQP1->MP(M/dump)->MT2/EQP2. 
       需处理两步: (1) 在 MT2/EQP2 的码流转出对应端口建立dump
                   (2) 建立点到点交换 MT1/EQP1->MP(M)->MT2/EQP2
    
    2. 新交换策略: MT1/EQP1->MP(M)->MP(Src1/Src2)->MP(T/dump)->MT2/EQP2. 
       需处理五步: (1) 在 MT1/EQP1 的码流转入端口, 即M端建立dump(基于多MP桥交换考虑)
                   (2) 在 MT2/EQP2 的码流转出端口, 即T端(即59000)建立dump
                   (3) 建立点到点交换     MT1/EQP1  ->MP(M/dump)->MP(Src1/2)
                   (4) 建立多点到一点交换 MP(M/dump)->MP(Src1/2)->MP(T/dump)
                   (5) 建立点到点交换     MP(Src1/2)->MP(T/dump)->MT2/EQP2
      
    3. 新的交换策略只用于第一路视频码流。音频和双流沿用原来的交换策略.
        
                                                                04/26/2007 zbq
******************************************************************************/
#include "mp.h"
#include "boardagent.h"
#include "mcuver.h"

//CMpInst *g_pMpInst;
static u8       g_byPageNum       = 0;

extern CNetTrfcStat g_cNetTrfcStat;
extern BOOL32       g_bPrintMpNetStat;

//构造

CMpInst::CMpInst()
{
	u16 wRet = 0;
	KdvSocketStartup();
	for (u8 byMediaIdx = 0 ;byMediaIdx < MEDIA_TYPE_NULL; byMediaIdx ++ )
	{
		//一种meidatype只用一个发送对象
		m_ppSnd[byMediaIdx] = NULL;
		if (  g_cMpApp.IsAudioType(byMediaIdx) )
		{
			m_ppSnd[byMediaIdx] = new CKdvMediaSnd;
			if (NULL == m_ppSnd[byMediaIdx])
			{
				MpLog(LOG_ERR,"[ProcSendSelfMutePackTimer]new kdvMediaSnd error\n");
				continue;
			}
			wRet = m_ppSnd[byMediaIdx]->Create(MAX_AUDIO_FRAME_SIZE,64<<10,25,byMediaIdx);
			if (wRet != 0)//0为正确
			{
				MpLog(LOG_ERR,"[ProcSendSelfMutePackTimer]meidatype:%d,Create kdvMediaSnd error\n",byMediaIdx);
				continue;
			}
		}

	}
	
	memset(m_achBuf,0,sizeof(m_achBuf));
}

//析构
CMpInst::~CMpInst()
{
}

/*====================================================================
    函数名      ：InstanceEntry
    功能        ：实例消息处理入口函数，必须override
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
====================================================================*/
void CMpInst::InstanceEntry(CMessage * const pcMsg)
{
	if (NULL == pcMsg)
	{
		MpLog(LOG_ERR, "[InstanceEntry] pcMsg is null!\n");
	}
    return;
}

/*====================================================================
    函数名      :DaemonInstanceEntry
    功能        :Daemon实例处理函数入口
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg,  传入的消息
				  CApp* pApp ,传入应用指针
    返回值说明  ：
====================================================================*/
void CMpInst::DaemonInstanceEntry(CMessage * const pcMsg, CApp* pcApp)
{
	if (NULL == pcApp)
	{
		MpLog(LOG_ERR, "[DaemonInstanceEntry] pcApp is null!\n");
		return;
	}

    //MP与mcu间消息id大于30000，不会重复
    if ( pcMsg->event >  WAITIFRAME_START_TIMER && 
         pcMsg->event <= WAITIFRAME_START_TIMER+(MAX_CONFIDX*(MAXNUM_CONF_MT+POINT_NUM)) )
    {
        DaemonProcWaitingIFrameTimer(pcMsg);
        return;
    }


    //g_pMpInst = this;
  
    switch(pcMsg->event) 
    {	 
    case OSP_POWERON:
        ProcPowerOn();
        break;
    case OSP_DISCONNECT:
        ProcDisconnect(pcMsg);      
        break;
        
    case EV_MP_CONNECT_MCU:
        ProcConnectTimeOut(TRUE);
        break;
    case EV_MP_CONNECT_MCUB:
        ProcConnectTimeOut(FALSE);
        break;

    case EV_MP_REGISTER_MCU:
        ProcRegisterTimeOut(TRUE);
        break;
    case EV_MP_REGISTER_MCUB:
        ProcRegisterTimeOut(FALSE);
        break;

    case MCU_MP_REG_ACK:
    case MCU_MP_REG_NACK:
        ProcMcuRegisterRsp(pcMsg);
        break;

	//处理Mcu的增加交换请求
	case MCU_MP_ADDSWITCH_REQ:
        ProcAddSwitchReq(pcMsg);              
        break;
	//处理Mcu的移除交换请求
    case MCU_MP_REMOVESWITCH_REQ:
        ProcRemoveSwitchReq(pcMsg);           
        break;

    //处理Mcu的增加多点到一点交换请求
    case MCU_MP_ADDMULTITOONESWITCH_REQ:		
        ProcAddMulToOneSwitchReq(pcMsg);    
        break;
	//处理Mcu的移除多点到一点交换请求
    case MCU_MP_REMOVEMULTITOONESWITCH_REQ:	
        ProcRemoveMulToOneSwitchReq(pcMsg); 
        break;
    //处理Mcu的停止多点到一点交换请求
    case MCU_MP_STOPMULTITOONESWITCH_REQ:		
        ProcStopMulToOneSwitchReq(pcMsg);  
        break;

	//处理Mcu的添加只接收交换的请求
    case MCU_MP_ADDRECVONLYSWITCH_REQ:
        ProcAddRecvOnlySwitchReq(pcMsg);      
        break;
    //处理Mcu的移除只接收交换的请求
    case MCU_MP_REMOVERECVONLYSWITCH_REQ:
        ProcRemoveRecvOnlySwitchReq(pcMsg);   
        break;


    //处理Mcu的添加广播源的请求 或 强行切换源命令
    case MCU_MP_ADDBRDSRCSWITCH_REQ:
        ProcAddBroadSwitchSrcReq(pcMsg);
        break;
    //处理Mcu的删除广播目标的请求
    case MCU_MP_REMOVEBRDSRCSWITCH_REQ:
        ProcRemoveBroadSwitchSrcReq(pcMsg);
        break;
    //处理Mcu的添加广播目标的请求
    case MCU_MP_ADDBRDDSTSWITCH_REQ:
        ProcAddBroadSwitchDstReq(pcMsg);
        break;

    case EV_MP_CHGSRCTOT_TIMER:
        ProcChangeSrcTimeout();
        break;
        
    case EV_MP_DEAL_CALLBACK_NTF:
        ProcCallBackSwitchChgNtf(pcMsg);
        break;

    // 取主备倒换状态
    case EV_MP_GETMSSTATUS_TIEMR:
    case MCU_MP_GETMSSTATUS_ACK:
        ProcGetMsStatusRsp(pcMsg);
        break;

    //处理Mcu的得到状态请求
    case MCU_MP_GETSWITCHSTATUS_REQ:
        ProcGetStatusReq(pcMsg);              
        break;
        
    //处理Mcu结束会议通知
    case MCU_MP_RELEASECONF_NOTIFY:
        ProcReleaseConfNotify(pcMsg);         
        break;

	//处理MCU的变更接收交换ssrc请求
    case MCU_MP_SETRECVSWITCHSSRC_REQ:
        // 统一用归一化处理
        break;
	//归一化处理
    case MCU_MP_CONFUNIFORMMODE_NOTIFY:
        ProcConfUniformModeNotify(pcMsg);     
        break;
        
    // 统计值
    case EV_MP_NETSTAT:
        ProcNetStat();
        break;
	// 定时对MP的转发流量进行监测，超限则上报会控
	case EV_MP_FLUXOVERRUN:
		ProcFluxOverRun();
		break;
	// 对发送超限通知进行过滤，5秒内只能发送一个
	case EV_MP_SENDFLUXOVERRUNNOTIFY:
		ProcSendFluxOverrunNotify(pcMsg);
		break;
    case EV_SWITCHTABLE_PRTNEXTPAGE_TEMER:
        ShowSwitch();
        break;

    // For unit test
    case EV_MP_ADDTABLE:
        ProcAddTable(pcMsg);
        break;
    case EV_MP_CLEARTABLE:                         
        g_cMpApp.ClearSwitchTable( TRUE );
        break; 

	case MCU_MP_ADDSENDSELFMUTEPACK_CMD:
		ProcAddSendSelfMutePackCmd(pcMsg);

	case MCU_MP_DELSENDSELFMUTEPACK_CMD:
		ProcDelSendSelfMutePackCmd(pcMsg);
		break;

    case MCU_MP_PINHOLE_NOTIFY:         //H460打洞处理逻辑         
        ProcMcuMpPinHoleNotify(pcMsg);
        break;
        
    case MCU_MP_STOPPINHOLE_NOTIFY:     //H460补洞处理逻辑 
        PorcMcuMpStopPinHoleNotify(pcMsg);
		break;

    default:
        MpLog( LOG_CRIT, "unexpected msg.%d<%s> received in DaemonInstanceEntry !\n", 
                                          pcMsg->event, OspEventDesc(pcMsg->event) );
        break;
    }
    return;
}

/*=============================================================================
    函 数 名： ProcPowerOn
    功    能： 实例初始化
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： void 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2006/2/7  4.0			许世林                  创建
=============================================================================*/
void CMpInst::ProcPowerOn()
{
    SetTimer( EV_MP_NETSTAT, NETFRFCSTAT_TIMESPAN*1000 );
	SetTimer( EV_MP_FLUXOVERRUN, 10 );
    return;
}

/*=============================================================================
  函 数 名： ProcConnectTimeOut
  功    能： 处理连接超时
  算法实现： 
  全局变量： 
  参    数： BOOL32 bIsConnectA
  返 回 值： void 
=============================================================================*/
void CMpInst::ProcConnectTimeOut(BOOL32 bIsConnectA)
{
    BOOL32 bRet = FALSE;
    if( bIsConnectA )
    {
        bRet = ConnectToMcu(bIsConnectA, g_cMpApp.m_dwMcuNode);
        if(TRUE == bRet)
        {
            SetTimer(EV_MP_REGISTER_MCU, MP_REGISTER_TMEROUT);
        }
        else
        {
            SetTimer(EV_MP_CONNECT_MCU, MP_CONNECT_TIMEOUT);
        }
    }
    else
    {
        bRet = ConnectToMcu(bIsConnectA, g_cMpApp.m_dwMcuNodeB);
        if(TRUE == bRet)
        {
            SetTimer(EV_MP_REGISTER_MCUB, MP_REGISTER_TMEROUT);
        }
        else
        {
            SetTimer(EV_MP_CONNECT_MCUB, MP_CONNECT_TIMEOUT);
        }
    }
    return;
}

/*=============================================================================
  函 数 名： ConnectToMcu
  功    能： 连接Mcu
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
=============================================================================*/
BOOL32 CMpInst::ConnectToMcu(BOOL32 bIsConnectA, u32& dwMcuNode)
{
    BOOL32 bRet = TRUE;
    if(!OspIsValidTcpNode(dwMcuNode))
    {
        if(TRUE == bIsConnectA)
        { 
            dwMcuNode = BrdGetDstMcuNode(); 
        }
        else
        { 
            dwMcuNode = BrdGetDstMcuNodeB(); 
        }

        if(OspIsValidTcpNode(dwMcuNode))
        {
            MpLog( LOG_INFO, "[ConnectToMcu] mp connect to mcu A success!\n" );
            OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), GetInsID());
            
            g_cMpApp.m_byConnectNum++;
            if(1 == g_cMpApp.m_byConnectNum) // 以防多次修改状态
            {
                NextState(STATE_IDLE);
            }
        }
        else
        {
            MpLog( LOG_CRIT, "[MpConnectToMcu] Fail to connect to mcu!\n");
            bRet = FALSE;
        }
    }
    return bRet;
}

/*=============================================================================
  函 数 名： ProcRegisterTimeOut
  功    能： 处理注册超时
  算法实现： 
  全局变量： 
  参    数： BOOL32 bIsRegister
  返 回 值： void 
=============================================================================*/
void CMpInst::ProcRegisterTimeOut(BOOL32 bIsRegisterA)
{
    if(bIsRegisterA)
    {
        RegisterToMcu(g_cMpApp.m_dwMcuNode);
        SetTimer(EV_MP_REGISTER_MCU, MP_REGISTER_TMEROUT);
    }
    else
    {
        RegisterToMcu(g_cMpApp.m_dwMcuNodeB);
        SetTimer(EV_MP_REGISTER_MCUB, MP_REGISTER_TMEROUT);
    }
    return;
}

/*=============================================================================
  函 数 名： RegisterToMcu
  功    能： 向Mcu注册
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
=============================================================================*/
void CMpInst::RegisterToMcu(u32 dwMcuNode)
{
    TMp tmp;
    tmp.SetMcuId(g_cMpApp.m_byMcuId);
    tmp.SetMpId(g_cMpApp.m_byMpId);
    tmp.SetIpAddr(g_cMpApp.m_dwIpAddr);
    tmp.SetAlias(g_cMpApp.m_abyMpAlias);
    tmp.SetAttachMode(g_cMpApp.m_byAttachMode);
	tmp.SetVersion(DEVVER_MP);

    CServMsg cRegMsg;
    cRegMsg.SetMcuId(g_cMpApp.m_byMcuId);
    cRegMsg.SetNoSrc();
    cRegMsg.SetMsgBody((u8 *)&tmp, sizeof(tmp));

    post( MAKEIID(AID_MCU_MPSSN, g_cMpApp.m_byMpId), MP_MCU_REG_REQ, 
          cRegMsg.GetServMsg(), cRegMsg.GetServMsgLen(), dwMcuNode );
        
    MpLog( LOG_INFO, "[RegisterToMcu] Send register infomation to Mcu !\n");
    return;
}

/*=============================================================================
  函 数 名： ProcDisconnect
  功    能： 断链处理
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
=============================================================================*/
void CMpInst::ProcDisconnect(CMessage * const pcMsg)
{
    u32 dwDisNode = *(u32*)pcMsg->content; // 断链节点号

    if ( INVALID_NODE != dwDisNode )
    {
        OspDisconnectTcpNode( dwDisNode );
    }
    if(dwDisNode == g_cMpApp.m_dwMcuNode)
    {
        g_cMpApp.FreeStatusA();
        if( 0 != g_cMpApp.m_dwMcuIpAddr && !g_cMpApp.m_bEmbedA )
        { 
            SetTimer(EV_MP_CONNECT_MCU, MP_CONNECT_TIMEOUT); 
        }
        else
        { 
            SetTimer(EV_MP_REGISTER_MCU, MP_REGISTER_TMEROUT); 
        }
    }
    else if(dwDisNode == g_cMpApp.m_dwMcuNodeB)
    {
        g_cMpApp.FreeStatusB();
        if( 0 != g_cMpApp.m_dwMcuIpAddrB && !g_cMpApp.m_bEmbedB )
        { 
            SetTimer(EV_MP_CONNECT_MCUB, MP_CONNECT_TIMEOUT); 
        }
        else
        { 
            SetTimer(EV_MP_REGISTER_MCUB, MP_REGISTER_TMEROUT); 
        }
    }
    
	// 与其中一个断链后，向Mcu取主备倒换状态，判断是否成功
	if (INVALID_NODE != g_cMpApp.m_dwMcuNode || INVALID_NODE != g_cMpApp.m_dwMcuNodeB)
	{
		if (OspIsValidTcpNode(g_cMpApp.m_dwMcuNode))
		{
			post( g_cMpApp.m_dwMcuIId, MP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cMpApp.m_dwMcuNode );     
        
			MpLog( LOG_CRIT, "[MsgDisconnectProc] GetMsStatusReq. McuNode.A\n");
		}
		else if (OspIsValidTcpNode(g_cMpApp.m_dwMcuNodeB))// 发消息取状态
		{
			post( g_cMpApp.m_dwMcuIIdB, MP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cMpApp.m_dwMcuNodeB );        

			MpLog( LOG_CRIT, "[DaemonProcMcuDisconnect] GetMsStatusReq. McuNode.B\n");
		}

		SetTimer(EV_MP_GETMSSTATUS_TIEMR, WAITING_MSSTATUS_TIMEOUT);
		return;
	}
	
    if( INVALID_NODE == g_cMpApp.m_dwMcuNode && INVALID_NODE == g_cMpApp.m_dwMcuNodeB )
    {
        ClearInstStatus();
    }
    return;
}

/*=============================================================================
  函 数 名： ClearInstStatus
  功    能： 清空实例
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CMpInst::ClearInstStatus(void)
{
    g_cMpApp.ClearSwitchTable( TRUE );
    g_cMpApp.m_dwMpcSSrc = 0;

    ::dsRemoveAll(g_cMpApp.m_nInitUdpHandle);
    MpLog( LOG_INFO, "[ClearInstStatus] Disconnect to mcu success !\n");
	g_cMpApp.m_byRegAckNum = 0;
    NextState(STATE_IDLE);
	return;
}

/*=============================================================================
  函 数 名： ProcGetMsStatusRsp
  功    能： 处理取主备倒换状态
  算法实现： 
  全局变量： 
  参    数： CMessage* const pcMsg
  返 回 值： void 
=============================================================================*/
void CMpInst::ProcGetMsStatusRsp(CMessage* const pcMsg)
{
	BOOL bSwitchOk = FALSE;
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	if( MCU_MP_GETMSSTATUS_ACK == pcMsg->event )
	{
		TMcuMsStatus *ptMsStatus = (TMcuMsStatus *)cServMsg.GetMsgBody();
        
        KillTimer(EV_MP_GETMSSTATUS_TIEMR);
        MpLog( LOG_CRIT, "[ProcGetMsStatusRsp]. receive msg MCU_MP_GETMSSTATUS_ACK. IsMsSwitchOK :%d\n", 
                          ptMsStatus->IsMsSwitchOK());

        if(ptMsStatus->IsMsSwitchOK())
        {
            bSwitchOk = TRUE;
        }
	}

	// 倒换失败或者超时
	if( !bSwitchOk )
	{
		//20110914 zjl 主备倒换失败 先清状态  再断链 再连接
		ClearInstStatus();
		MpLog(LOG_CRIT, "[ProcGetMsStatusRsp]ClearInstStatus!\n");

		if (OspIsValidTcpNode(g_cMpApp.m_dwMcuNode))
		{
			MpLog(LOG_CRIT, "[ProcGetMsStatusRsp] OspDisconnectTcpNode A!\n");	
			OspDisconnectTcpNode(g_cMpApp.m_dwMcuNode);
		}
		if (OspIsValidTcpNode(g_cMpApp.m_dwMcuNodeB))
		{
			MpLog(LOG_CRIT, "[ProcGetMsStatusRsp] OspDisconnectTcpNode B!\n");	
			OspDisconnectTcpNode(g_cMpApp.m_dwMcuNodeB);
		}

		if( INVALID_NODE == g_cMpApp.m_dwMcuNode)// 有可能产生重连，在Connect里面过滤
		{
			if( 0 != g_cMpApp.m_dwMcuIpAddr && !g_cMpApp.m_bEmbedA )
			{ 
				MpLog(LOG_CRIT, "[ProcGetMsStatusRsp] EV_MP_CONNECT_MCU!\n");
                SetTimer(EV_MP_CONNECT_MCU, MP_CONNECT_TIMEOUT);
            }
			else
			{ 
				SetTimer(EV_MP_REGISTER_MCU, MP_REGISTER_TMEROUT);
			}
		}
		if( INVALID_NODE == g_cMpApp.m_dwMcuNodeB)
		{
			if( 0 != g_cMpApp.m_dwMcuIpAddrB && !g_cMpApp.m_bEmbedB )
			{
				MpLog(LOG_CRIT, "[ProcGetMsStatusRsp] EV_MP_CONNECT_MCUB!\n");
                SetTimer(EV_MP_CONNECT_MCUB, MP_CONNECT_TIMEOUT);
            }
			else
			{ 
				SetTimer(EV_MP_REGISTER_MCUB, MP_REGISTER_TMEROUT);
			}
		}
		MpLog( LOG_CRIT, "[ProcGetMsStatusRsp] Fail to get MS status or time out\n");
	}
	return;
}

/*=============================================================================
  函 数 名： ProcMcuRegisterRsp
  功    能： 注册响应
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
=============================================================================*/
void CMpInst::ProcMcuRegisterRsp(CMessage * const pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    switch( pcMsg->event )
    {
    case MCU_MP_REG_ACK:
		{
			if(pcMsg->srcnode == g_cMpApp.m_dwMcuNode)
			{
				g_cMpApp.m_dwMcuIId  = pcMsg->srcid;
	            g_cMpApp.m_wRecvMtDataPort = *(u16*)(cServMsg.GetMsgBody() + sizeof(u32));
                g_cMpApp.m_byRegAckNum++;
			    KillTimer(EV_MP_REGISTER_MCU);
				MpLog(LOG_INFO, "[ProcMcuRegisterRsp] Register to MCU A success !\n");
	        }
		    else if(pcMsg->srcnode == g_cMpApp.m_dwMcuNodeB)
			{
				g_cMpApp.m_dwMcuIIdB  = pcMsg->srcid;
	            g_cMpApp.m_wRecvMtDataPortB = *(u16*)(cServMsg.GetMsgBody() + sizeof(u32));
                g_cMpApp.m_byRegAckNum++;
			    KillTimer(EV_MP_REGISTER_MCUB);
				MpLog(LOG_INFO, "[ProcMcuRegisterRsp] Register to MCU B success !\n");
			}

		    u32 dwMpcSSrc = ntohl(*(u32*)cServMsg.GetMsgBody());
		    // guzh [6/12/2007] 校验会话参数
			if ( g_cMpApp.m_dwMpcSSrc == 0 )
			{
				g_cMpApp.m_dwMpcSSrc = dwMpcSSrc;
			}
			else
			{
				// 异常情况，断开两个节点
				if ( g_cMpApp.m_dwMpcSSrc != dwMpcSSrc )
				{
					OspPrintf(TRUE, FALSE, "[MsgRegAckProc] MPC SSRC ERROR(%u<-->%u), Disconnect Both Nodes!\n", 
						      g_cMpApp.m_dwMpcSSrc, dwMpcSSrc);
					if ( OspIsValidTcpNode(g_cMpApp.m_dwMcuNode) )
					{
						OspDisconnectTcpNode(g_cMpApp.m_dwMcuNode);
					}
					if ( OspIsValidTcpNode(g_cMpApp.m_dwMcuNodeB) )
					{
						OspDisconnectTcpNode(g_cMpApp.m_dwMcuNodeB);
					}      
					return;
				}
			}

        
            if (1 == g_cMpApp.m_byRegAckNum)  // 以防多次更改状态
		    {
			    // guzh [1/11/2007] 记录是否要做MP码流限制
                g_cMpApp.m_bIsRestrictFlux = ( 1 == *(u8*)( cServMsg.GetMsgBody() + sizeof(u32) + sizeof(u16) ) )? TRUE : FALSE;
	            u16	wMaxCap = *(u16*)( cServMsg.GetMsgBody() + sizeof(u32) + sizeof(u16) + sizeof(u8) );
		        wMaxCap = ntohs(wMaxCap);
			    g_cNetTrfcStat.m_nMaxCap = ((s32)wMaxCap) << 10;
            
	            NextState(STATE_NORMAL);
		    }

            //保存打洞时间间隔
            if( cServMsg.GetMsgBodyLen() >= sizeof(u32) + sizeof(u16) + sizeof(u8) + sizeof(u16) + sizeof(u32) )
            {
                u32 dwPinHoleTime = *(u32*)( cServMsg.GetMsgBody() + sizeof(u32) + sizeof(u16) + sizeof(u8) + sizeof(u16) );
                g_cMpApp.SetPinHoleInterval(ntohl(dwPinHoleTime));
			    MpLog(LOG_CRIT, "[ProcMcuRegisterRsp] MCU_MP_REG_ACK dwPinHoleTime(%d) !!!\n", dwPinHoleTime);
            }

			//保存哑音包发送包数和时间间隔
            if( cServMsg.GetMsgBodyLen() >= sizeof(u32) + sizeof(u16) + sizeof(u8) + sizeof(u16) + sizeof(u32) +2*sizeof(u8))
            {
                u8 byMutePackNum = *( cServMsg.GetMsgBody() + sizeof(u32) + sizeof(u16) + sizeof(u8) + sizeof(u16) +sizeof(u32));
				u8 byMutePackSendInterval = *( cServMsg.GetMsgBody() + sizeof(u32) + sizeof(u16) + sizeof(u8) + sizeof(u16) +sizeof(u32)+sizeof(u8));
                g_cMpApp.SetMutePackNum(byMutePackNum);
				g_cMpApp.SetMutePackInterval(byMutePackSendInterval);

				MpLog(LOG_CRIT, "[ProcMcuRegisterRsp] MCU_MP_REG_ACK byMutePackNum(%d) byMutePackSendInterval(%d) !!!\n", byMutePackNum,byMutePackSendInterval);
            }
            
		}
    	break;

    case MCU_MP_REG_NACK:
        {
            u32 dwMcuNode = pcMsg->srcnode;
            if(dwMcuNode == g_cMpApp.m_dwMcuNode)
            {
                MpLog(LOG_CRIT, "[ProcMcuRegisterRsp] Receive mcu A's Register Nack\n");
            }
            else if(dwMcuNode == g_cMpApp.m_dwMcuNodeB)
            {
                MpLog(LOG_CRIT, "[ProcMcuRegisterRsp] Receive mcu B's Register Nack\n");
            }
        }
    	break;

    default:
        MpLog(LOG_CRIT, "unexpected msg.%d<%s> received in RegisterRsp!\n", 
                               pcMsg->event, OspEventDesc(pcMsg->event) );
        break;
    }
    return;
}

/*=============================================================================
    函 数 名： DaemonProcWaitingIFrameTimer
    功    能： 等待I帧超时处理
    算法实现： 
    全局变量： 
    参    数： CMessage * const pcMsg
    返 回 值： void 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2006/5/9  4.0			许世林                  创建
=============================================================================*/
void CMpInst::DaemonProcWaitingIFrameTimer(CMessage * const pcMsg)
{
    u32 wMtId = *(u32 *)pcMsg->content;
    u8 byConfNo = (u8)((pcMsg->event-wMtId-WAITIFRAME_START_TIMER)/(MAXNUM_CONF_MT+POINT_NUM));
    if (wMtId > MAXNUM_CONF_MT+POINT_NUM || byConfNo >= MAXNUM_MCU_CONF)
    {
        MpLog( LOG_CRIT, "[DaemonProcWaitingIFrameTimer] invalid confno :%d or mtid :%d\n", byConfNo, wMtId );
        return;
    }    
    
    MpLog( LOG_INFO, "[DaemonProcWaitingIFrameTimer] received timerid :%d wMtId :%d confno :%d\n", 
                     pcMsg->event, wMtId, byConfNo );
    
    g_cMpApp.m_pptWaitIFrmData[byConfNo][wMtId-1].bWaitingIFrame  = FALSE;   
    
    return;
}

/*=============================================================================
  函 数 名： ProcAddSwitchReq
  功    能： MCU->MP 增加交换表
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/20    4.0			张宝卿                增加广播交换逻辑和非归一重整回调
  20100505      4.6         pengjie               批量建交换支持
=============================================================================*/
void CMpInst::ProcAddSwitchReq(CMessage * const pcMsg)
{
    if(NULL == pcMsg)
    {
        MpLog( LOG_CRIT, "[ProcAddSwitchReq] param err: pcMsg.0x%x !\n", pcMsg );
        return;
    }

    CServMsg cRevMsg(pcMsg->content, pcMsg->length);
    u8  byConfIdx = cRevMsg.GetConfIdx() - 1;

    if( byConfIdx >= MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[ProcAddSwitchReq] ConfIdx:%d error !\n", byConfIdx + 1 );
        SendMsgToMcu( pcMsg->event + 2 );
        return;
    }
    
	//限制MP转发流量处理, zgc, 07/01/11
    RestrictFluxOfMp( pcMsg );

	u16 wMsgBodyLen = cRevMsg.GetMsgBodyLen();
    if (wMsgBodyLen % (sizeof(TSwitchChannelExt) + sizeof(u8) * 3) != 0)
    {
        MpLog( LOG_CRIT, "[AddSwitchDstReq] msg body len.%d err, ignore it!\n", wMsgBodyLen );
        SendMsg2Mcu( pcMsg->event + 2, cRevMsg );
        return;
    }
    
    u8 byDstMtNum = wMsgBodyLen / (sizeof(TSwitchChannelExt) + sizeof(u8) * 3);
	
    CServMsg cReplyMsg;
    cReplyMsg.SetSrcDriId( g_cMpApp.m_byMpId );
    cReplyMsg.SetConfIdx( byConfIdx + 1 );
    
    u16 wOffSet = 0;
	
    // 逐个增加目标地址交换
	cRevMsg.SetSrcDriId(g_cMpApp.m_byMpId);
    u8 byDstId = 0;
    for( ; byDstId < byDstMtNum; byDstId ++ )
    {
        TSwitchChannelExt tSwitchChannel = *(TSwitchChannelExt*)(cRevMsg.GetMsgBody() + wOffSet);
        u8  bySwitchChannelMode = *(u8*)(cRevMsg.GetMsgBody() + sizeof(TSwitchChannelExt) + wOffSet);
        u8  byUniformPayload    = *(u8*)(cRevMsg.GetMsgBody() + sizeof(TSwitchChannelExt) + 1 + wOffSet);
        //u8  byIsSwitchRtcp      = *(u8*)(cRevMsg.GetMsgBody() + sizeof(TSwitchChannel) + 2 + wOffSet);
		
        wOffSet += sizeof(TSwitchChannelExt) + 3;
		
        MpLog( LOG_INFO, "[AddSwitchDstReq] byChnMode.%d, byUniformload.%d !\n", 
			bySwitchChannelMode, byUniformPayload );
		
		u32 dwRevIp     = tSwitchChannel.GetRcvIP();
		u16 wRevPort    = tSwitchChannel.GetRcvPort();
		u32 dwRevBindIp = tSwitchChannel.GetRcvBindIP();
		
		u32 dwDstIp     = tSwitchChannel.GetDstIP();
		u16 wDstPort    = tSwitchChannel.GetDstPort();
		u32 dwSndBindIp = tSwitchChannel.GetSndBindIP();
		
		s8 achRevIp[18] = {0};
		strofip(dwRevIp, achRevIp);    
		
		// zbq [04/26/2007] 判断是否为已经存在的交换，是则不重复建立
		BOOL32 bSwitchExist = FALSE;
		
		TSwitchChannel tTmpChan;
		if ( g_cMpApp.SearchSwitchTable( byConfIdx, dwDstIp, wDstPort, tTmpChan) )
		{
			if ( dwRevIp  == tTmpChan.GetRcvIP() &&
				wRevPort == tTmpChan.GetRcvPort() )
			{
				bSwitchExist = TRUE;
			}
		}
		if ( bSwitchExist ) 
		{
			//SendMsg2Mcu( pcMsg->event+1, cRevMsg );
			MpLog( LOG_WARN, "[ProcAddSwitchReq] switch: %s(%d)-->%s(%d) exist, reply success!\n", 
				achRevIp, wRevPort, strofip(dwDstIp), wDstPort ); 
			continue;
		}
		
		u32 dwResult1 = DSOK;
		u32 dwResult2 = DSOK;
		
		dwResult1 = ::dsAdd( g_cMpApp.m_nInitUdpHandle, 
			htonl(dwRevIp), wRevPort, htonl(dwRevBindIp), 
			htonl(dwDstIp), wDstPort, htonl(dwSndBindIp) );
		// guzh [10/30/2007] 该交换没有存在意义
		/*
		if (0 != byIsSwitchRtcp)
		{
        dwResult2 = ::dsAdd( g_cMpApp.m_nInitUdpHandle, 
		htonl(dwRevIp), wRevPort+1, htonl(dwRevBindIp),
		htonl(dwDstIp), wDstPort+1, htonl(dwSndBindIp) );
		}
		*/
		
		if( DSOK == dwResult1 && DSOK == dwResult2 )
		{
			BOOL32 bFlag = g_cMpApp.AddSwitchTable(byConfIdx, &tSwitchChannel, SWITCH_ONE2ONE);
			if( bFlag )
			{
				if(NULL == g_cMpApp.m_pptFilterData)
				{
					MpLog( LOG_CRIT, "[ProcAddSwitchReq] error: m_ppSend = NULL\n" );
					continue;
				}
				
				::dsSetSendCallback( g_cMpApp.m_nInitUdpHandle, 
					htonl(dwRevIp), wRevPort,
					0, 0, MpSendFilterCallBackProc );
				
				u16 wChanNo = g_cMpApp.GetChanNoByDst(byConfIdx, dwDstIp, wDstPort, 0);
				
				if ( wChanNo != MAX_SWITCH_CHANNEL )
				{
					// 回调数据整理，回调设置
					TSendFilterAppData * ptSendFilterAppData;
					
					if( CONF_UNIFORMMODE_VALID == g_cMpApp.GetConfUniMode(byConfIdx) )
					{
						// xsl [5/11/2006]等关键帧处理
						u32 dwMtId = tSwitchChannel.GetSrcMt().GetMtId();                    
						if (g_cMpApp.m_bIsWaitingIFrame && wRevPort % PORTSPAN == 0)
						{                		
							g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].bWaitingIFrame = g_cMpApp.m_bIsWaitingIFrame;
							g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].dwFstTimeStamp = 0;
							g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].dwSrcIp  = dwRevIp;
							g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].wSrcPort = wRevPort;
							g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].byConfNo = byConfIdx;
							g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].byMtId   = (u8)dwMtId;
							SetTimer(WAITIFRAME_START_TIMER+byConfIdx*(MAXNUM_CONF_MT+POINT_NUM) + dwMtId, 5000, dwMtId);
							MpLog( LOG_INFO, "[ProcAddSwitchReq] SetTimer waiting I Frame, byconfno :%d, wchannelno :%d, dwDstIp :0x%x, dwDstPort :%d, MtId:%d\n",
								byConfIdx, wChanNo, dwDstIp, wDstPort, dwMtId);
						}      
						
						if (SWITCHCHANNEL_UNIFORMMODE_NONE == bySwitchChannelMode)
						{
							g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].bUniform = FALSE;
						}
						else
						{
							g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].bUniform = TRUE;
							//ip端口作假
							if (tSwitchChannel.GetMapIp() == 0 || tSwitchChannel.GetMapPort() == 0)
							{
								MpLog(LOG_INFO,"[ProcAddSwitchReq] no mappedip or port!\n");
							}
							else//设置了作假的ip和端口
							{
								MpLog(LOG_INFO,"[ProcAddSwitchReq]dstip:%d,port:%d ,MappedIp:%d,MappedPort:%d",dwDstIp,wDstPort,tSwitchChannel.GetMapIp(),tSwitchChannel.GetMapPort());
								u32 dwRet = dsSpecifyDstFwdSrc( g_cMpApp.m_nInitUdpHandle, htonl(dwRevIp),wRevPort,
									htonl(dwDstIp), wDstPort, 
									htonl(tSwitchChannel.GetMapIp()), tSwitchChannel.GetMapPort());
								if (DSOK != dwRet)
								{
									
									MpLog( LOG_CRIT, "[ProcAddSwitchReq] dsSpecifyDstFwdSrc failed!\n");
								}
							}
						}
						g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].byPayload  = byUniformPayload;
						
						g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].byConfNo   = byConfIdx;
						g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].wChannelNo = wChanNo;
						ptSendFilterAppData = &g_cMpApp.m_pptFilterData[byConfIdx][wChanNo];
						
						MpLog(LOG_INFO, "[ProcAddSwitchReq] Add wChanNo = %d\n", wChanNo);
					}
					else
					{
						g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].byConfNo   = byConfIdx;
						ptSendFilterAppData = &g_cMpApp.m_pptFilterData[byConfIdx][wChanNo];
						MpLog(LOG_INFO, "[ProcAddSwitchReq] Add wChanNo = %d ( NoUni )\n", wChanNo);                        
					}
					
					::dsSetAppDataForSend( g_cMpApp.m_nInitUdpHandle, 
						htonl(dwRevIp), wRevPort,
						0, 0, htonl(dwDstIp), wDstPort,
						(void *)ptSendFilterAppData );
				}
				else
				{
					MpLog( LOG_CRIT, "[ProcAddSwitchReq] no pos for this switchSrc!\n" );
					return;
				}
				
				//SendMsg2Mcu( pcMsg->event+1, cRevMsg );
				//回的消息体中不需要mapip和port				
				cReplyMsg.CatMsgBody( (u8*)&tSwitchChannel, sizeof(TSwitchChannel) );
				MpLog( LOG_INFO, "[ProcAddSwitchReq] dsAdd: %s(%d)-->%s(%d) success!\n", 
					achRevIp, wRevPort, strofip(dwDstIp), wDstPort);
			}
			else
			{
				MpLog( LOG_CRIT, "[ProcAddSwitchReq] Add table %s(%d)-->%s(%d) failed \n",
					achRevIp, wRevPort, strofip(dwDstIp), wDstPort );
				::dsRemove(g_cMpApp.m_nInitUdpHandle, htonl(dwRevIp), wRevPort);
				::dsRemove(g_cMpApp.m_nInitUdpHandle, htonl(dwRevIp), wRevPort+1);
				SendMsg2Mcu( pcMsg->event+2, cRevMsg );
			}
		}
		else
		{
			SendMsg2Mcu( pcMsg->event+2, cRevMsg );
			MpLog( LOG_CRIT, "[ProcAddSwitchReq] dsAdd %s(%d)-->%s(%d) Fail, err code: %d, %d\n",
				achRevIp, wRevPort, strofip(dwDstIp), wDstPort, dwResult1, dwResult2);
		}
	}

	// 所有增加成功的交换通知MCU记录
    if ( cReplyMsg.GetMsgBodyLen() > 0 ) 
    {
        SendMsg2Mcu( pcMsg->event + 1, cReplyMsg );
    }

    return;
}

/*=============================================================================
  函 数 名： ProcRemoveSwitchReq
  功    能： MCU->MP 移除交换表
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/27    4.0			张宝卿                增加广播交换逻辑下删除 T->MT2/EQP2 交换的保护策略
  20110505      4.6         pengjie               批量拆交换支持
=============================================================================*/
void CMpInst::ProcRemoveSwitchReq(CMessage * const pcMsg)
{
    if( NULL == pcMsg )
    {
        MpLog( LOG_CRIT, "[ProcRemoveSwitchReq] The pointer cannot be Null.\n");
        return;
    }

    CServMsg cRevMsg(pcMsg->content, pcMsg->length);
    u8  byConfIdx = cRevMsg.GetConfIdx() - 1;
    u32 dwResult1 = DSOK, dwResult2 = DSOK;
	cRevMsg.SetSrcDriId(g_cMpApp.m_byMpId);

    if(byConfIdx >= MAX_CONFIDX)
    {
        MpLog( LOG_CRIT, "[ProcRemoveSwitchReq] ConfIdx.%d error !\n", byConfIdx+1 );
        SendMsg2Mcu( pcMsg->event+2, cRevMsg );
        return;
    }

	u16 wMsgBodyLen = cRevMsg.GetMsgBodyLen();
    //if (wMsgBodyLen % ((sizeof(TSwitchChannel) + sizeof(u8)) != 0))
	if (wMsgBodyLen % (sizeof(TSwitchChannel) + sizeof(u8)) != 0)
    {
        MpLog( LOG_CRIT, "[ProcRemoveSwitchReq] msg body len.%d err, ignore it!\n", wMsgBodyLen );
        SendMsg2Mcu( pcMsg->event + 2, cRevMsg );
        return;
    }
    
    u8 byDstMtNum = wMsgBodyLen / (sizeof(TSwitchChannel) + sizeof(u8));

	MpLog( LOG_INFO, "[ProcRemoveSwitchReq] byDstMtNum:%d!\n", byDstMtNum);

    CServMsg cReplyMsg;
    cReplyMsg.SetSrcDriId( g_cMpApp.m_byMpId );
    cReplyMsg.SetConfIdx( byConfIdx + 1 );
    
    u16 wOffSet = 0;
	u8 byDstId = 0;
    for( ; byDstId < byDstMtNum; byDstId ++ )
    {
		TSwitchChannel tSwitchChannel = *(TSwitchChannel*)( cRevMsg.GetMsgBody() + wOffSet);
		u8  byIsSwitchRtcp = *(u8 *)(cRevMsg.GetMsgBody() + sizeof(TSwitchChannel) + wOffSet);
		u32 dwDstIp  = tSwitchChannel.GetDstIP();
		u16 wDstPort = tSwitchChannel.GetDstPort();

		MpLog(LOG_INFO, "[ProcRemoveSwitchReq] dwDstIp:%s, wDstPort:%d!\n", strofip(dwDstIp), wDstPort);

		wOffSet += sizeof(TSwitchChannel) + sizeof(u8);

		dwResult1 = ::dsRemove(g_cMpApp.m_nInitUdpHandle, htonl(dwDstIp), wDstPort);
		if (0 != byIsSwitchRtcp)
		{
			dwResult2 = ::dsRemove(g_cMpApp.m_nInitUdpHandle, htonl(dwDstIp), wDstPort+1);
		}
  
		if(DSOK == dwResult1 && DSOK == dwResult2)
		{
			MpLog( LOG_INFO, "[ProcRemoveSwitchReq] dsRemove:%s(%d) success !\n", 
				strofip(dwDstIp), wDstPort );
			
			BOOL32 bFlag = g_cMpApp.RemoveSwitchTable(byConfIdx, &tSwitchChannel, SWITCH_ONE2ONE);
			if( bFlag )
			{
				// zbq [04/28/2007] 不论是否归一重整，都清Filter
				u16 wChanNo = g_cMpApp.GetChanNoByDst(byConfIdx, dwDstIp, wDstPort, 1);
				if ( wChanNo != MAX_SWITCH_CHANNEL )
				{
					g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].bRemoved = TRUE;
					if (g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].bUniform == TRUE)
					{
						SendSelfMutePack(dwDstIp,wDstPort);
					}
				}
				else
				{
					MpLog( LOG_CRIT, "[ProcRemoveSwitchReq] chan:%d unexist !\n", wChanNo );
				}
			}
			else
			{
				// LOG_ERR: 刚启动Mp时switch table 为空
				MpLog( LOG_ERR, "[ProcRemoveSwitchReq] remove <%s@%d> from table failed.\n",
					strofip(dwDstIp), wDstPort );
			}
			cReplyMsg.CatMsgBody( (u8 *)&tSwitchChannel, sizeof(tSwitchChannel) );
			//SendMsg2Mcu( pcMsg->event+1, cReplyMsg );
		}
		else
		{
			SendMsg2Mcu( pcMsg->event+2, cRevMsg );
			MpLog( LOG_CRIT, "[ProcRemoveSwitchReq] dsRemove<%s@%d> failed !\n",
				strofip(dwDstIp), wDstPort );
		}

	}

	// 拆除成功的交换项，通知mcu
	if( cReplyMsg.GetMsgBodyLen() > 0 )
	{
		SendMsg2Mcu( pcMsg->event+1, cReplyMsg );
	}

    return;
}

/*=============================================================================
  函 数 名： ProcAddMulToOneSwitchReq
  功    能： 增加多点到一点交换
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/28    4.0			张宝卿                代码整理
=============================================================================*/
void CMpInst::ProcAddMulToOneSwitchReq(CMessage * const pcMsg)
{
    if(NULL == pcMsg)
    {
        MpLog( LOG_CRIT, "[AddMulToOneSwitch] The pointer cannot be Null.\n");
        return;
    }

    CServMsg cRevMsg(pcMsg->content, pcMsg->length);
    u8 byConfIdx = cRevMsg.GetConfIdx()-1;
    if(byConfIdx >= MAX_CONFIDX)
    {
        MpLog( LOG_CRIT, "[AddMulToOneSwitch] ConfIdx:%d error !\n", byConfIdx+1 );
        SendMsg2Mcu( pcMsg->event+2, cRevMsg );
        return;
    }

    //限制MP转发流量处理, zgc, 07/01/11
    RestrictFluxOfMp( pcMsg );
    
	////////////////////////////////////////
	u16 wMsgBodyLen = cRevMsg.GetMsgBodyLen();
    if (wMsgBodyLen % (sizeof(TSwitchChannel) + sizeof(u32) + sizeof(u16)) != 0)
    {
        MpLog( LOG_CRIT, "[AddMulToOneSwitch] msg body len.%d err, ignore it!\n", wMsgBodyLen );
        SendMsg2Mcu( pcMsg->event + 2, cRevMsg );
        return;
    }
    
    u8 byDstMtNum = wMsgBodyLen / (sizeof(TSwitchChannel) + sizeof(u32) + sizeof(u16));

	OspPrintf(TRUE, FALSE, "[AddMulToOneSwitch] byDstMtNum:%d!\n", byDstMtNum);

	CServMsg cReplyMsg;
    cReplyMsg.SetSrcDriId( g_cMpApp.m_byMpId );
    cReplyMsg.SetConfIdx( byConfIdx + 1 );

	cRevMsg.SetSrcDriId(g_cMpApp.m_byMpId);
	u16 wOffSet = 0;
    u8  byDstId = 0;
    for( ; byDstId < byDstMtNum; byDstId ++ )
    {
        TSwitchChannel tSwitchChannel = *(TSwitchChannel*)(cRevMsg.GetMsgBody() + wOffSet);

		u32 dwRevIp     = tSwitchChannel.GetRcvIP();
		u16 wRevPort    = tSwitchChannel.GetRcvPort();
		u32 dwRcvBindIP = tSwitchChannel.GetRcvBindIP();
		u32 dwDstIp     = tSwitchChannel.GetDstIP();
		u16 wDstPort    = tSwitchChannel.GetDstPort();
		u32 dwSndBindIP = tSwitchChannel.GetSndBindIP();
		u32 dwMapIpAddr = *(u32*)(cRevMsg.GetMsgBody() + sizeof(TSwitchChannel) + wOffSet);
        u16 wMapPort    = *(u16*)(cRevMsg.GetMsgBody() + sizeof(TSwitchChannel) + sizeof(u32) + wOffSet);
	
		wOffSet += sizeof(TSwitchChannel) + sizeof(u32) + sizeof(u16);
#ifdef NEW_DS
    // add many to one
		u32 dwResult = ::dsAddManyToOne( g_cMpApp.m_nInitUdpHandle, 
										 htonl(dwRevIp), wRevPort, htonl(dwRcvBindIP),
										 htonl(dwDstIp), wDstPort, htonl(dwSndBindIP) );

		//Judge result and response
		cRevMsg.SetSrcDriId(g_cMpApp.m_byMpId);
		if (DSOK == dwResult)
		{
			//SendMsg2Mcu( pcMsg->event+1, cRevMsg );

			cReplyMsg.CatMsgBody( (u8*)&tSwitchChannel, sizeof(tSwitchChannel) );
			
			MpLog( LOG_INFO, "[AddMulToOneSwitch] dsAddManyToOne 0x%x(%d)--->0x%x(%d)\n", 
								dwRevIp, wRevPort, ntohl(dwMapIpAddr), ntohs(wMapPort) );

			//将 MT.RTCP -> PRS 时，考虑到防火墙支撑，需将交换包的源ip、port映射为 MT.RTP，以便PRS重传
			if ( 0 != wMapPort )
			{
				if ( DSOK != dsSpecifyFwdSrc( g_cMpApp.m_nInitUdpHandle, 
											  htonl(dwRevIp), wRevPort, 
											  dwMapIpAddr, ntohs(wMapPort) ) )
				{
					MpLog( LOG_CRIT, "[AddMulToOneSwitch] dsSpecifyFwdSrc: 0x%x(%d)--->0x%x(%d) Err !\n", 
									  dwRevIp, wRevPort, ntohl(dwMapIpAddr), ntohs(wMapPort));
				}
			}

			// zbq [07/19/2007] RTCP交换不再记入交换表
			/*
			BOOL32 bFlag = g_cMpApp.AddSwitchTable( byConfIdx, &tSwitchChannel, SWITCH_MUL2ONE );
			if( bFlag )
			{
				SendMsg2Mcu( pcMsg->event+1, cRevMsg );
			}
			else
			{
				::dsRemoveManyToOne( g_cMpApp.m_nInitUdpHandle,
									 htonl(dwRevIp), wRevPort, htonl(dwDstIp), wDstPort );
				SendMsg2Mcu( pcMsg->event + 2, cRevMsg );
				MpLog( LOG_CRIT, "[AddMulToOneSwitch] Add 0x%x(%d)--->0x%x(%d) to table failed !\n",
								  dwRevIp, wRevPort, dwDstIp, wDstPort);            
			}*/

		}
		else
		{
			//SendMsg2Mcu( pcMsg->event+2, cRevMsg );
			MpLog( LOG_CRIT, "[AddMulToOneSwitch] dsAddManyToOne 0x%x(%d)--->0x%x(%d) failed!\n", 
							  dwRevIp, wRevPort, dwDstIp, wDstPort );
		}
	}
    if ( cReplyMsg.GetMsgBodyLen() > 0 ) 
    {
        SendMsg2Mcu( pcMsg->event + 1, cReplyMsg );
    }

#endif
    return;
}

/*=============================================================================
  函 数 名： ProcRemoveMulToOneSwitchReq
  功    能： 移除多点到一点交换
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/28    4.0			张宝卿                代码整理
=============================================================================*/
void CMpInst::ProcRemoveMulToOneSwitchReq(CMessage * const pcMsg)
{
    if(NULL == pcMsg)
    {
        MpLog( LOG_CRIT, "[RemoveMulToOneSwitch] The pointer cannot be Null.\n");
        return;
    }

    CServMsg cRevMsg(pcMsg->content, pcMsg->length);
    u8 byConfIdx = cRevMsg.GetConfIdx()-1;

	if( byConfIdx >= MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[RemoveMulToOneSwitch] ConfIdx.%d error !\n", byConfIdx+1 );
        SendMsg2Mcu( pcMsg->event+2, cRevMsg );
        return;
    }

	////////////////////////////////////////
	u16 wMsgBodyLen = cRevMsg.GetMsgBodyLen();
    if ((wMsgBodyLen % sizeof(TSwitchChannel)) != 0)
    {
        MpLog( LOG_CRIT, "[RemoveMulToOneSwitch] msg body len.%d err, ignore it!\n", wMsgBodyLen );
        SendMsg2Mcu( pcMsg->event + 2, cRevMsg );
        return;
    }
    
    u8 byDstMtNum = wMsgBodyLen / (sizeof(TSwitchChannel));
	
	OspPrintf(TRUE, FALSE, "[RemoveMulToOneSwitch] byDstMtNum:%d!\n", byDstMtNum);
	
	CServMsg cReplyMsg;
    cReplyMsg.SetSrcDriId( g_cMpApp.m_byMpId );
    cReplyMsg.SetConfIdx( byConfIdx + 1 );

	u16 wOffSet = 0;
    u8  byDstId = 0;
    for( ; byDstId < byDstMtNum; byDstId ++ )
    {
        TSwitchChannel tSwitchChannel = *(TSwitchChannel*)(cRevMsg.GetMsgBody() + wOffSet);
		u32 dwRevIp  = tSwitchChannel.GetRcvIP();
		u16 wRevPort = tSwitchChannel.GetRcvPort();
		u32 dwDstIp  = tSwitchChannel.GetDstIP();
	    u16 wDstPort = tSwitchChannel.GetDstPort();
		u32 dwResult = ::dsRemoveManyToOne( g_cMpApp.m_nInitUdpHandle, 
											htonl(dwRevIp), wRevPort, 
											htonl(dwDstIp), wDstPort );
		wOffSet += sizeof(TSwitchChannel);

		cRevMsg.SetSrcDriId(g_cMpApp.m_byMpId);
		if(DSOK == dwResult)
		{
			cReplyMsg.CatMsgBody( (u8*)&tSwitchChannel, sizeof(tSwitchChannel));
			//SendMsg2Mcu( pcMsg->event+1, cRevMsg );
			MpLog( LOG_INFO, "[RemoveMulToOneSwitch] dsRemoveManyToOne <0x%x@%d>-><0x%x@%d> Success !\n",
							  htonl(dwRevIp), wRevPort, htonl(dwDstIp), wDstPort );

			// zbq [07/19/2007] RTCP交换不再记入交换表
			/*
			BOOL32 bFlag = g_cMpApp.RemoveSwitchTable(byConfIdx, &tSwitchChannel, SWITCH_MUL2ONE);
			if( bFlag )
			{
				SendMsg2Mcu( pcMsg->event+1, cRevMsg );
			}
			else
			{
				SendMsg2Mcu( pcMsg->event+2, cRevMsg );
				MpLog( LOG_CRIT, "[RemoveMulToOneSwitch] <0x%x@%d>-><0x%x@%d> unexist !\n",
								  htonl(dwRevIp), wRevPort, htonl(dwDstIp), wDstPort );
			}*/
		}
		else
		{
			//SendMsg2Mcu( pcMsg->event+2, cRevMsg );
			MpLog( LOG_ERR, "[RemoveMulToOneSwitch] dsRemoveManyToOne <0x%x@%d>-><0x%x@%d Fail !\n",
							  htonl(dwRevIp), wRevPort, htonl(dwDstIp), wDstPort );
		}
	}

	if ( cReplyMsg.GetMsgBodyLen() > 0 ) 
    {
        SendMsg2Mcu( pcMsg->event + 1, cReplyMsg );
    }
    
    return;
}

/*=============================================================================
  函 数 名： ProcStopMulToOneSwitchReq
  功    能： 停止多点到一点交换
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/28    4.0			张宝卿                代码整理
=============================================================================*/
void CMpInst::ProcStopMulToOneSwitchReq(CMessage * const pcMsg)
{
    if( NULL == pcMsg )
    {
        MpLog( LOG_CRIT, "[StopMulToOneSwitch] The pointer cannot be Null.\n");
        return;
    }
    
    CServMsg cRevMsg(pcMsg->content, pcMsg->length);
    u8 byConfIdx = cRevMsg.GetConfIdx()-1;

    if( byConfIdx > MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[StopMulToOneSwitch] ConfIdx:%d error\n", byConfIdx+1 );
        SendMsg2Mcu( pcMsg->event+2, cRevMsg );
        return;
    }

    TSwitchChannel tSwitchChannel = *(TSwitchChannel *)cRevMsg.GetMsgBody();
	u32 dwDstIp  = tSwitchChannel.GetDstIP();
	u16 wDstPort = tSwitchChannel.GetDstPort();
    
    u32 dwResult = ::dsRemoveAllManyToOne( g_cMpApp.m_nInitUdpHandle, 
                                           htonl(dwDstIp), wDstPort );
    
    cRevMsg.SetSrcDriId(g_cMpApp.m_byMpId);
    if(DSOK == dwResult)
    {
        SendMsg2Mcu( pcMsg->event + 1, cRevMsg );
        MpLog( LOG_INFO, "[StopMulToOneSwitch] dsRemoveAllManyToOne: %s(%d) success!\n", strofip(dwDstIp), wDstPort );
        
        // zbq [07/19/2007] RTCP交换不再记入交换表
        /*
        BOOL32 bFlag = g_cMpApp.RemoveSwitchTable( byConfIdx, &tSwitchChannel, SWITCH_MUL2ONE, TRUE );
        if( bFlag )
        {
            SendMsg2Mcu( pcMsg->event + 1, cRevMsg );
        }
        else
        {
            SendMsg2Mcu( pcMsg->event + 2, cRevMsg );
            MpLog( LOG_CRIT, "[StopMulToOneSwitch] switch:%s(%d) unexist!\n", strofip(dwDstIp), wDstPort );
        }*/
    }
    else
    {
        SendMsg2Mcu( pcMsg->event + 2, cRevMsg );
        MpLog( LOG_CRIT, "[StopMulToOneSwitch] dsRemoveAllManyToOne:%s(%d) failed!\n", strofip(dwDstIp), wDstPort );
    }
    return;
}

/*=============================================================================
  函 数 名： ProcAddRecvOnlySwitchReq
  功    能： 增加只接受交换
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/27    4.0			张宝卿                dump不记入交换表
=============================================================================*/
void CMpInst::ProcAddRecvOnlySwitchReq(CMessage * const pcMsg)
{
    if(NULL == pcMsg)
    {
        MpLog( LOG_CRIT, "[AddRecvOnlySwitch] The pointer cannot be Null.\n" );
        return;
    }

    CServMsg cRevMsg(pcMsg->content, pcMsg->length);
    u8 byConfIdx = cRevMsg.GetConfIdx()-1;
    if(byConfIdx >= MAX_CONFIDX)
    {
        MpLog( LOG_CRIT, "[AddRecvOnlySwitch] ConfIdx:%d error !\n", byConfIdx+1 );
        SendMsg2Mcu( pcMsg->event + 2, cRevMsg );
        return;
    }

    TSwitchChannel tSwitchChannel = *(TSwitchChannel*)cRevMsg.GetMsgBody();
	u8 byReverseG7221c = 0;
	if (cRevMsg.GetMsgBodyLen() > sizeof(tSwitchChannel))
	{
		byReverseG7221c = *(cRevMsg.GetMsgBody() + sizeof(TSwitchChannel));//获得是否需要对g7221.c音频格式码流内容奇偶对换
	}

    u32 dwRevIp     = tSwitchChannel.GetRcvIP();
    u16 wRevPort    = tSwitchChannel.GetRcvPort();

    BOOL32 bPtMt    = FALSE;
    BOOL32 bPtFirst = FALSE;
    u8     byMtNum  = 0;

    // zbq [04/02/2007] 终端视频端口统一从59000上接收. 即建立T的DUMP规则
    if ( g_cMpApp.IsPointMt(wRevPort) ) 
    {
        wRevPort = CONFBRD_STARTPORT + byConfIdx * PORTSPAN;
        tSwitchChannel.SetRcvPort(wRevPort);

        // zbq [05/13/2007] 记录本MP负责转发的终端数
        byMtNum = g_cMpApp.GetMtNumNeedT(byConfIdx);
        if ( 0 == byMtNum )
        {
            bPtFirst = TRUE;
        }
        byMtNum++;
        g_cMpApp.SetMtNumNeedT( byConfIdx, byMtNum );

        bPtMt = TRUE;
    }

    if ( bPtMt && !bPtFirst )
    {
        MpLog( LOG_INFO, "[AddRecvOnlySwitch] T's dump already exist for.%d, ignore !\n", byMtNum-- );
        return;
    }

    u32 dwRevBindIp = tSwitchChannel.GetRcvBindIP();
    u32 dwResult = ::dsAddDump( g_cMpApp.m_nInitUdpHandle, 
                                htonl(dwRevIp), wRevPort, htonl(dwRevBindIp) );

    cRevMsg.SetSrcDriId(g_cMpApp.m_byMpId);
    
    s8 achStrIp[18] = {0};
    strofip(dwRevIp, achStrIp);

    if( DSOK == dwResult )
    {
		MpLog( LOG_INFO,"dwRcvIp:%d,wRcvPort:%d,rtcpbackIP:%d,rtcpbackPort:%d\n",dwRevIp,wRevPort,tSwitchChannel.GetDstIP(),tSwitchChannel.GetDstPort());
		if (tSwitchChannel.GetDstIP() != 0 && tSwitchChannel.GetDstPort() != 0)
		{
			dwResult = dsStartRtcpRR(g_cMpApp.m_nInitUdpHandle,htonl(dwRevIp), wRevPort,htonl(tSwitchChannel.GetDstIP()),tSwitchChannel.GetDstPort(),g_cMpApp.m_byRtcpBackTimerInterval);
			MpLog( LOG_INFO,"==========[dsStartRtcpRR]ret:%d,dwRcvIp:%d,wRcvPort:%d,rtcpbackIP:%d,rtcpbackPort:%d\n",dwResult,dwRevIp,wRevPort,tSwitchChannel.GetDstIP(),tSwitchChannel.GetDstPort());
		}

        // zbq [04/27/2007] dump不记入交换表
        if ( CONF_UNIFORMMODE_VALID != g_cMpApp.GetConfUniMode(byConfIdx) )
        {
            ::dsSetSendCallback( g_cMpApp.m_nInitUdpHandle, 
                                 htonl(dwRevIp), wRevPort, 0, 0, NULL );
        }
        else
        {
			// 更新g7221.c音频格式码流内容奇偶对换列表
			if (byReverseG7221c == TRUE)
			{
				g_cMpApp.AddReverseG7221cPort(wRevPort);
			}
			else
			{
				g_cMpApp.RemoveReverseG7221cPort(wRevPort);
			}

            ::dsSetSendCallback( g_cMpApp.m_nInitUdpHandle, 
                                 htonl(dwRevIp), wRevPort, 0, 0, MpSendFilterCallBackProc );
        }

        MpLog( LOG_INFO, "[AddRecvOnlySwitch] dsAddDump: %s(%d) success !\n", achStrIp, wRevPort);
        SendMsg2Mcu( pcMsg->event + 1, cRevMsg );
    }
    else
    {
        MpLog( LOG_CRIT, "[AddRecvOnlySwitch] dsAddDump: %s(%d) failed !\n", achStrIp, wRevPort);
        SendMsg2Mcu( pcMsg->event + 2, cRevMsg );
    }

    return;
}

/*=============================================================================
  函 数 名： ProcRemoveRecvOnlySwitchReq
  功    能： 移除只接收交换
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/27    4.0			张宝卿                dump不记入交换表
=============================================================================*/
void CMpInst::ProcRemoveRecvOnlySwitchReq(CMessage * const pcMsg)
{
    if( NULL == pcMsg )
    {
        MpLog( LOG_CRIT, "[RemoveRecvOnlySwitch] The pointer cannot be Null.\n");
        return;
    }

    CServMsg cRevMsg(pcMsg->content, pcMsg->length);
    u8 byConfIdx = cRevMsg.GetConfIdx()-1;
    if(byConfIdx >= MAX_CONFIDX)
    {
        MpLog( LOG_CRIT, "[RemoveRecvOnlySwitch] ConfIdx:%d error\n", byConfIdx+1);
        SendMsg2Mcu( pcMsg->event + 2, cRevMsg );
        return;
    }

    TSwitchChannel tSwitchChannel = *(TSwitchChannel*)cRevMsg.GetMsgBody();
    u32 dwRevIp  = tSwitchChannel.GetRcvIP();
    u16 wRevPort = tSwitchChannel.GetRcvPort();
    
    cRevMsg.SetSrcDriId(g_cMpApp.m_byMpId);

	// 从g7221.c音频格式码流内容奇偶对换表清除
	g_cMpApp.RemoveReverseG7221cPort(wRevPort);

    // zbq [04/27/2007] 拆终端dump，即直接拆 T的dump
    if ( g_cMpApp.IsPointMt( wRevPort ) )
    {
        wRevPort = CONFBRD_STARTPORT + byConfIdx * PORTSPAN;
        tSwitchChannel.SetRcvPort( wRevPort );

        u8 byMtNum = g_cMpApp.GetMtNumNeedT(byConfIdx);
        byMtNum --;
        g_cMpApp.SetMtNumNeedT( byConfIdx, byMtNum );

        if ( g_cMpApp.IsTInUseStill(byConfIdx) || byMtNum > 0 )
        {
            MpLog( LOG_WARN, "[RemoveRecvOnlySwitch] T still In Use, No RemoveDump !\n" );
            return;
        }
    }
    
    u32 dwResult = dsRemoveDump(g_cMpApp.m_nInitUdpHandle, htonl(dwRevIp), wRevPort);
    if( DSOK == dwResult )
    {
        // zbq [04/27/2007] dump不记入交换表
        SendMsg2Mcu( pcMsg->event + 1, cRevMsg );
        MpLog( LOG_INFO, "[RemoveRecvOnlySwitch] dsRemoveDump<%s@%d> Success !\n",
                                                     strofip(dwRevIp), wRevPort );
    }
    else
    {
        SendMsg2Mcu( pcMsg->event + 2, cRevMsg );
        MpLog( LOG_CRIT, "[RemoveRecvOnlySwitch] dsRemoveDump<%s@%d> Failed !\n",
                                                    strofip(dwRevIp), wRevPort );
    }
    return;
}


/*=============================================================================
  函 数 名： ProcAddBroadSwitchSrcReq
  功    能： 增加 广播源 交换
  算法实现： 目前只支持视频做这样的处理
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  07/03/22    4.0         张宝卿        创建
  07/04/20    4.0         张宝卿        增加非归一重整的回调支持
=============================================================================*/
void CMpInst::ProcAddBroadSwitchSrcReq( CMessage * const pcMsg )
{
    CServMsg cRevMsg( pcMsg->content, pcMsg->length );

    u8  byConfIdx = cRevMsg.GetConfIdx() - 1;

    if ( byConfIdx >= MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[AddBroadSwitchSrcReq] ConfIdx:%d error \n", byConfIdx+1);
        SendMsgToMcu( pcMsg->event + 2 );
        return;
    }
    
    //限制MP转发流量处理
    RestrictFluxOfMp( pcMsg );

    TSwitchChannel tSwitchChannel = *(TSwitchChannel*)cRevMsg.GetMsgBody();
    u8             byChangeSrcNow = *(u8*)( cRevMsg.GetMsgBody() + sizeof(TSwitchChannel) );

    u32 dwRevIp     = tSwitchChannel.GetRcvIP();

    // zbq [04/26/2007] 针对多MP增加源，本接收地址可能不为本地地址，刷新之
	//8000H/G/I不需要修改RecvIP[5/25/2012 chendaiwei]
#if !defined(_8KH_) && !defined(_8KE_) && !defined(_8KI_)	
    if ( dwRevIp != g_cMpApp.m_dwIpAddr ) 
    {
        dwRevIp = g_cMpApp.m_dwIpAddr;
        tSwitchChannel.SetRcvIP(dwRevIp);
    }
#endif

    // zbq [06/13/2007] 同一个源重复建交换，直接ACK
    if ( g_cMpApp.IsSrcSwitchExist(byConfIdx, &tSwitchChannel) )
    {		
        CServMsg cServMsg;
        cServMsg.SetServMsg(cRevMsg.GetServMsg(), SERV_MSGHEAD_LEN);
        cServMsg.SetSrcDriId(g_cMpApp.m_byMpId);	
        
        //SendMsg2Mcu( pcMsg->event + 1, cServMsg );

        s8 achSrcIp[17] = {0};
        s8 achRcvIp[17] = {0};
        strcpy( achSrcIp, strofip(tSwitchChannel.GetSrcIp()) );
        strcpy( achRcvIp, strofip(tSwitchChannel.GetRcvIP()) );
        
        MpLog( LOG_INFO, "[AddBroadSwitchSrcReq] SrcSwitch Src<%s@%d>->Rcv<%s@%d> exist already \n",
                          achSrcIp, tSwitchChannel.GetRcvPort(),
                          achRcvIp, tSwitchChannel.GetRcvPort() );

		// 这里需要清另一个点的状态和通道, zgc, 2008-04-15
		u8 bySwitchedPt = 0;
		if ( PS_SWITCHED == g_cMpApp.GetPtState(byConfIdx, 1) )
		{
			bySwitchedPt = 1;
		}
		else if ( PS_SWITCHED == g_cMpApp.GetPtState(byConfIdx, 2) )
		{
			bySwitchedPt = 2;
		}
		if ( bySwitchedPt == 0 )
		{
			MpLog( LOG_CRIT, "[AddBroadSwitchSrcReq] find switched Pt failed, cur<%d,%d> impossible !\n",
					g_cMpApp.GetPtState(byConfIdx, 1), g_cMpApp.GetPtState(byConfIdx, 2) );
            SendMsg2Mcu( pcMsg->event + 2, cRevMsg );
            return;
		}
		
		if ( PS_GETSRC == g_cMpApp.GetPtState( byConfIdx, bySwitchedPt, TRUE ) )
		{
			g_cMpApp.SetPtState( byConfIdx, bySwitchedPt, PS_IDLE, TRUE );
			g_cMpApp.ProceedSrcChange( byConfIdx, bySwitchedPt );
		}
		else if ( PS_SWITCHED == g_cMpApp.GetPtState( byConfIdx, bySwitchedPt, TRUE ) )
		{
			MpLog( LOG_CRIT, "[AddBroadSwitchSrcReq] Have 2 switched pt, cur<%d,%d> impossible !\n",
					g_cMpApp.GetPtState(byConfIdx, 1), g_cMpApp.GetPtState(byConfIdx, 2) );
			SendMsg2Mcu( pcMsg->event + 2, cRevMsg );
			return;	
		}
		
		SendMsg2Mcu( pcMsg->event + 1, cServMsg );
		return;
    }
    else
    {
        s8 achSrcIp[16];
        s8 achRcvIp[16];
        strcpy( achSrcIp, strofip(tSwitchChannel.GetSrcIp()) );
        strcpy( achRcvIp, strofip(tSwitchChannel.GetRcvIP()) );
        
        MpLog( LOG_INFO, "[AddBroadSwitchSrcReq] SrcSwitch Src<%s@%d>->Rcv<%s@%d> add normally \n",
                          achSrcIp, tSwitchChannel.GetRcvPort(),
                          achRcvIp, tSwitchChannel.GetRcvPort() );
    }

    u16 wRevPort    = tSwitchChannel.GetRcvPort();
    //u32 dwRevBindIp = tSwitchChannel.GetRcvBindIP();

    u32 dwDstIp     = tSwitchChannel.GetDstIP();
    u16 wDstPort    = tSwitchChannel.GetDstPort();
    u32 dwSndBindIp = tSwitchChannel.GetSndBindIP();

    // zbq [04/11/2007] 默认支持归一重整，增加目标交换的时候过滤
    u8  byUniformPayload    = INVALID_PAYLOAD;
	// fxh[091120]对于广播源的转发点，默认不能进行归一重整，否则会导致（如画面合成）丢包重传失效
    u8 bySwitchChannelMode = /*SWITCHCHANNEL_UNIFORMMODE_VALID*/SWITCHCHANNEL_UNIFORMMODE_NONE;

    // 获取当前空闲的中转源和空闲的跳转间隔，建交换
    u8  byPointIdle = g_cMpApp.GetPtIdle( byConfIdx );
    if ( 0 == byPointIdle )
    {
        MpLog( LOG_ERR, "[AddBroadSwitchSrcReq] find idle Pt failed, replace GETSRC Pt!\n" );

        // zbq [04/29/2007] 没有空闲SRC点，直接替换当前的SRC点
        BOOL bGETSRCPtExist = FALSE;

        u8 byPID = 1;
        for( ; byPID <= POINT_NUM; byPID ++ )
        {
            if ( PS_GETSRC == g_cMpApp.GetPtState(byConfIdx, byPID) )
            {
                // 当前SRC点取消等待，通知MCU删交换e
                g_cMpApp.ProceedSrcChange( byConfIdx, byPID, FALSE );

                //当前会议等待时间清零，重新等待记时
                g_cMpApp.SetChgDelay( byConfIdx, 0 );

                bGETSRCPtExist = TRUE;
                byPointIdle = byPID;

                MpLog( LOG_ERR, "[AddBroadSwitchSrcReq] Point.%d has been robbed !\n", byPID );

                break;
            }
        }
        if ( !bGETSRCPtExist )
        {
            MpLog( LOG_CRIT, "[AddBroadSwitchSrcReq] find getSrc Pt failed, cur<%d,%d> impossible !\n",
                              g_cMpApp.GetPtState(byConfIdx, 1), g_cMpApp.GetPtState(byConfIdx, 2) );
            SendMsg2Mcu( pcMsg->event + 2, cRevMsg );
            return;
        }
    }

    g_cMpApp.SetPtState( byConfIdx, byPointIdle, PS_GETSRC );

	//8kh/8kg 内部切变的ip用本地ip
#if !defined(_8KH_) && !defined(_8KE_) && !defined(_8KI_)	
   tSwitchChannel.SetDstIP( dwRevIp );
#else
     tSwitchChannel.SetDstIP(g_cMpApp.m_dwIpAddr);
	
#endif
    
    tSwitchChannel.SetDstPort( CONFBRD_STARTPORT + byConfIdx * PORTSPAN + byPointIdle * POINT_NUM );
    
    dwDstIp  = tSwitchChannel.GetDstIP();
    wDstPort = tSwitchChannel.GetDstPort();

    MpLog( LOG_INFO, "[AddBroadSwitchSrcReq] Add Src.0x%x to Point.%d !\n",
                                  tSwitchChannel.GetSrcIp(), byPointIdle );

    u32 dwRet1 = DSOK;
    u32 dwRet2 = DSOK;
    u32 dwRet3 = DSOK;


    // 1. 建立：M 的DUMP规则，基于桥码流畅通考虑
    dwRet1 = ::dsAddDump( g_cMpApp.m_nInitUdpHandle,
                          htonl(dwRevIp), wRevPort, htonl(dwSndBindIp) );
    if ( DSOK != dwRet1 ) 
    {
        ::dsRemoveDump( g_cMpApp.m_nInitUdpHandle, htonl(dwRevIp), wRevPort );
        MpLog( LOG_CRIT, "[AddBroadSwitchSrcReq] dsAddDump %s(%d) failed!\n", strofip(dwRevIp), wRevPort );
        return;
    }
    else
    {
        MpLog( LOG_INFO, "[AddBroadSwitchSrcReq] dsAddDump %s(%d) succeed!\n", strofip(dwRevIp), wRevPort );

        if ( CONF_UNIFORMMODE_VALID != g_cMpApp.GetConfUniMode(byConfIdx) )
        {
            ::dsSetSendCallback( g_cMpApp.m_nInitUdpHandle, 
                                 htonl(dwRevIp), wRevPort, 0, 0, NULL );
        }
        else
        {
            ::dsSetSendCallback( g_cMpApp.m_nInitUdpHandle, 
                                 htonl(dwRevIp), wRevPort, 0, 0, MpSendFilterCallBackProc );
        }        
    }
    
    // 2. 建立：MT -> MT(mp) -> S1/S2
    dwRet2 = ::dsAdd( g_cMpApp.m_nInitUdpHandle, 
                      htonl(dwRevIp), wRevPort, htonl(dwSndBindIp), 
                      htonl(dwDstIp), wDstPort, htonl(dwSndBindIp) );
    
    // guzh [10/30/2007] 该交换没有存在意义
    /*
    // 3. 建立：MT -> MT(mp) -> S1/S2，默认RTCP畅通
    dwRet3 = ::dsAdd( g_cMpApp.m_nInitUdpHandle, 
                      htonl(dwRevIp), wRevPort + 1, htonl(dwRevBindIp),
                      htonl(dwDstIp), wDstPort + 1, htonl(dwSndBindIp) );
    */

    cRevMsg.SetSrcDriId(g_cMpApp.m_byMpId);

    s8 achRcvIp[18] = {0};
    strofip(dwRevIp, achRcvIp);
    
    if( DSOK == dwRet2 && DSOK == dwRet3 )
    {
        BOOL32 bFlag = g_cMpApp.AddSwitchTable( byConfIdx, &tSwitchChannel, SWITCH_ONE2ONE );
        if( bFlag )
        {
            if(NULL == g_cMpApp.m_pptFilterData)
            {
                MpLog( LOG_CRIT, "[AddBroadSwitchSrcReq] err: NULL == m_pptFilterData\n");
                g_cMpApp.SetPtState( byConfIdx, byPointIdle, PS_IDLE );
                return;
            }

            // 回调注册
            ::dsSetSendCallback( g_cMpApp.m_nInitUdpHandle, 
                                 htonl(dwRevIp), wRevPort,
                                 0, 0, MpSendFilterCallBackProc );

            u16 wChanNo = g_cMpApp.GetChanNoByDst( byConfIdx, dwDstIp, wDstPort, 0 );
            if ( wChanNo != MAX_SWITCH_CHANNEL )
            {
                // 回调自定义数据整理、回调设置
                TSendFilterAppData * ptSendFilterAppData;

                if( CONF_UNIFORMMODE_VALID == g_cMpApp.GetConfUniMode(byConfIdx) )
                {
                    // xsl [5/11/2006]等关键帧处理
                    u32 dwMtId = tSwitchChannel.GetSrcMt().GetMtId();                    
                    if (g_cMpApp.m_bIsWaitingIFrame && wRevPort % PORTSPAN == 0)
                    {
                        g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].bWaitingIFrame = g_cMpApp.m_bIsWaitingIFrame;
                        g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].dwFstTimeStamp = 0;
                        g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].dwSrcIp  = dwRevIp;
                        g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].wSrcPort = wRevPort;
                        g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].byConfNo = byConfIdx;
                        g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].byMtId   = (u8)dwMtId;
                        SetTimer(WAITIFRAME_START_TIMER+byConfIdx*(MAXNUM_CONF_MT+POINT_NUM) + dwMtId, 5000, dwMtId);
                        MpLog( LOG_INFO, "[AddBroadSwitchSrcReq] SetTimer waiting I Frame, byconfno :%d, wchannelno :%d, dwDstIp :0x%x, dwDstPort :%d, MtId:%d\n",
                                          byConfIdx, wChanNo, dwDstIp, wDstPort, dwMtId );
                    }

                    if (SWITCHCHANNEL_UNIFORMMODE_NONE == bySwitchChannelMode)
                    { 
                        g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].bUniform = FALSE;
                    }
                    else
                    {
                        g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].bUniform = TRUE;
                    }
                    g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].byPayload  = byUniformPayload;
                    g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].byConfNo   = byConfIdx;
                    g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].wChannelNo = wChanNo;
                    ptSendFilterAppData = &g_cMpApp.m_pptFilterData[byConfIdx][wChanNo];

                    MpLog( LOG_INFO, "[AddBroadSwitchSrcReq] add switch: wChanNo = %d\n", wChanNo );
                }
                else
                {
                    g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].byConfNo   = byConfIdx;
                    ptSendFilterAppData = &g_cMpApp.m_pptFilterData[byConfIdx][wChanNo];
                    MpLog( LOG_INFO, "[AddBroadSwitchSrcReq] add switch: wChanNo = %d ( NoUni )\n", wChanNo );
                }
                
                
                ::dsSetAppDataForSend( g_cMpApp.m_nInitUdpHandle, 
                                       htonl(dwRevIp), wRevPort,
                                       0, 0, htonl(dwDstIp), wDstPort,
                                       (void *)ptSendFilterAppData );
            }
            else
            {
                MpLog( LOG_CRIT, "[AddBroadSwitchSrcReq] no pos for this switchSrc!\n" );
                return;
            }
            
            SendMsg2Mcu( pcMsg->event + 1, cRevMsg );
            MpLog( LOG_INFO, "[AddBroadSwitchSrcReq] dsAdd %s(%d)-->%s(%d) success !\n",
                              achRcvIp, wRevPort, strofip(tSwitchChannel.GetDstIP()), tSwitchChannel.GetDstPort() );
        }
        else
        {
            ::dsRemove( g_cMpApp.m_nInitUdpHandle, htonl(dwRevIp), wRevPort );
            ::dsRemove( g_cMpApp.m_nInitUdpHandle, htonl(dwRevIp), wRevPort+1 );
            SendMsg2Mcu( pcMsg->event + 2, cRevMsg );
            g_cMpApp.SetPtState( byConfIdx, byPointIdle, PS_IDLE );
            MpLog( LOG_CRIT, "[AddBroadSwitchSrcReq] Fail to add switch table !\n" );
        }
    }
    else
    {
        if ( DSOK == dwRet2 )
        {
            ::dsRemove( g_cMpApp.m_nInitUdpHandle, htonl(dwRevIp), wRevPort );
        }
        if ( DSOK == dwRet3 ) 
        {
            ::dsRemove( g_cMpApp.m_nInitUdpHandle, htonl(dwRevIp), wRevPort+1 );
        }
        SendMsg2Mcu( pcMsg->event + 2, cRevMsg );
        g_cMpApp.SetPtState( byConfIdx, byPointIdle, PS_IDLE );
        MpLog( LOG_CRIT, "[AddBroadSwitchSrcReq] dsAdd %s(%d)-->%s(%d) failed, reason<%d,%d>\n",
                          achRcvIp, wRevPort, 
                          strofip(tSwitchChannel.GetDstIP()), tSwitchChannel.GetDstPort(), dwRet2, dwRet3 );
    }

    // 4. 相关的切换处理
    u8 bySwitchNum = 0;
    for( u8 byIdx = 1; byIdx <= POINT_NUM; byIdx ++ )
    {
        if ( PS_SWITCHED == g_cMpApp.GetPtState(byConfIdx, byIdx) ) 
        {
            bySwitchNum ++;
        }
    }
    // 当前无源，强制切换
    if ( 0 == bySwitchNum && 0 == byChangeSrcNow )
    {
        byChangeSrcNow = 1;
        MpLog( LOG_CRIT, "[AddBroadSwitchSrcReq] No src now, change forcely !\n" );
    }

    BOOL32 bRet = FALSE;
	//8kh/8kg 内部切变的ip用本地ip
#if !defined(_8KH_) && !defined(_8KE_) && !defined(_8KI_)	

#else
	tSwitchChannel.SetRcvIP(g_cMpApp.m_dwIpAddr);
	
#endif
    if ( 1 == byChangeSrcNow )
    {
        // 马上建立交换，并切换到当前源
        bRet = SwitchSrcToT( byConfIdx, byPointIdle,
                             tSwitchChannel, 
                             bySwitchChannelMode, byUniformPayload, TRUE );
        if ( !bRet )
        {
            MpLog( LOG_CRIT, "[AddBroadSwitchSrcReq] SwitchSrcToT failed, bNow.%d !\n", byChangeSrcNow );
            SendMsg2Mcu( pcMsg->event + 2, cRevMsg );
            g_cMpApp.SetPtState( byConfIdx, byPointIdle, PS_IDLE );
        }
    }
    else
    {
        // 先建立交换，丢掉关键帧到来前的码流包；关键帧来了再切换到当前源；超时强切。
        bRet = SwitchSrcToT( byConfIdx, byPointIdle,
                             tSwitchChannel, 
                             bySwitchChannelMode, byUniformPayload );
        if ( !bRet )
        {
            MpLog( LOG_CRIT, "[AddBroadSwitchSrcReq] SwitchSrcToT failed !\n" );
            SendMsg2Mcu( pcMsg->event + 2, cRevMsg );
            g_cMpApp.SetPtState( byConfIdx, byPointIdle, PS_IDLE );
        }
        
        MpLog( LOG_INFO, "[AddBroadSwitchSrcReq] Change to cur src until IFrame !\n" );
    }
}

/*=============================================================================
    函 数 名： SwitchSrcToT
    功    能： MP 内部 切换源节点到中转T节点
    算法实现： 
    全局变量： 
    参    数： u8             byConfIdx :
               u8             bySrcPoint: 新建源节点号
               TSwitchChannel tSrcSwitch: 源节点当前的交换信息
               u8             bySwitchChannelMode:
               u8             byUniformPayload   :
               BOOL32         bSwitchNow         : 是否马上刷节点切换
    返 回 值： BOOL32
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2007/04/03  4.0			张宝卿                  创建
    2007/04/20  4.0         张宝卿                  增加非归一重整的回调支持
=============================================================================*/
BOOL32 CMpInst::SwitchSrcToT( u8 byConfIdx, u8 bySrcPoint,
                              TSwitchChannel &tSrcSwitch, 
                              u8 bySwitchChannelMode, 
                              u8 byUniformPayload, BOOL32 bSwitchNow )
{
    if ( byConfIdx > MAX_CONFIDX || bySrcPoint == 0 || bySrcPoint > POINT_NUM ) 
    {
        MpLog( LOG_CRIT, "[SwitchSrcToT] param err: ConfIdx.%d, SrcPt.%d !\n", byConfIdx, bySrcPoint );
        return FALSE;
    }


    // 中转过渡规则构造
    u32 dwSrcIp = tSrcSwitch.GetRcvIP();
    u32 dwRcvIp = tSrcSwitch.GetRcvIP();
    u32 dwDstIp = tSrcSwitch.GetRcvIP();
    u32 dwSndBindIp = tSrcSwitch.GetSndBindIP();
    u16 wRcvPort = CONFBRD_STARTPORT + byConfIdx * PORTSPAN + bySrcPoint * POINT_NUM;
    u16 wDstPort = CONFBRD_STARTPORT + byConfIdx * PORTSPAN;

    TSwitchChannel tSwitch;
    tSwitch.SetSrcIp( dwSrcIp );
    tSwitch.SetRcvIP( dwRcvIp );
    tSwitch.SetDstIP( dwDstIp );
    tSwitch.SetSndBindIP( dwSndBindIp );
    tSwitch.SetRcvPort( wRcvPort );
    tSwitch.SetDstPort( wDstPort );

    //u16 wSrcPort = MT_MCU_STARTPORT + tSrcSwitch.GetSrcMt().GetMtId() * PORTSPAN;

    u32 dwRet1 = DSOK;
    u32 dwRet2 = DSOK;

    // 1. 建立：MT(mp) -> S1/S2 -> T
    dwRet1 = ::dsAddManyToOne( g_cMpApp.m_nInitUdpHandle,
                               htonl(dwRcvIp), wRcvPort, dwSndBindIp, 
                               htonl(dwDstIp), wDstPort, dwSndBindIp );
    
    // guzh [10/30/2007] 该交换没有存在意义
    /*
    // 2. 建立：MT(mp) -> S1/S2 -> T，默认RTCP畅通
    dwRet2 = ::dsAddManyToOne( g_cMpApp.m_nInitUdpHandle,
                               htonl(dwRcvIp), wRcvPort + 1, dwSndBindIp, 
                               htonl(dwDstIp), wDstPort + 1, dwSndBindIp );
    */
    
    s8 achRcvIp[18] = {0};
    strofip(dwRcvIp, achRcvIp);
    
    if ( DSOK == dwRet1 || DSOK == dwRet2 ) 
    {
        // 中转过渡规则也加入交换表(在交换表中唯一)，并直接支持归一重整
        BOOL32 bFlag = g_cMpApp.AddSwitchTable( byConfIdx, &tSwitch, SWITCH_MUL2ONE );
        if( bFlag )
        {
            if(NULL == g_cMpApp.m_pptFilterData)
            {
                MpLog( LOG_CRIT, "[SwitchSrcToT] err: NULL == m_pptFilterData\n");
                return FALSE;
            }
            
            ::dsSetSendCallback( g_cMpApp.m_nInitUdpHandle, 
                                 htonl(tSwitch.GetRcvIP()), tSwitch.GetRcvPort(),
                                 0, 0, MpSendFilterCallBackProc );
            
            u16 wChanNo = g_cMpApp.GetChanNoByDst( byConfIdx, tSwitch.GetDstIP(), 
                                             tSwitch.GetDstPort(), 0, wRcvPort );
            
            MpLog( LOG_INFO, "[SwitchSrcToT] GetChanNoByDst.wRcvPort.%d, ChanNo.%d !\n", wRcvPort, wChanNo );

            if ( wChanNo != MAX_SWITCH_CHANNEL )
            {
                // 回调数据整理，回调设置
                TSendFilterAppData * ptSendFilterAppData;

                if( CONF_UNIFORMMODE_VALID == g_cMpApp.GetConfUniMode(byConfIdx) )
                {
                    // xsl [5/11/2006]等关键帧处理
                    u32 dwMtId = MAXNUM_CONF_MT + bySrcPoint;
                    if (g_cMpApp.m_bIsWaitingIFrame && wRcvPort % PORTSPAN == 0)
                    {                		
                        g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].bWaitingIFrame = g_cMpApp.m_bIsWaitingIFrame;
                        g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].dwFstTimeStamp = 0;
                        g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].dwSrcIp  = dwRcvIp;
                        g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].wSrcPort = wRcvPort;
                        g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].byConfNo = byConfIdx;
                        g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].byMtId   = (u8)dwMtId;
                        SetTimer(WAITIFRAME_START_TIMER+byConfIdx*(MAXNUM_CONF_MT+POINT_NUM) + dwMtId, 5000, dwMtId);
                        MpLog( LOG_INFO, "[SwitchSrcToT] SetTimer waiting I Frame, byconfno :%d, wchannelno :%d, dwDstIp :0x%x, dwDstPort :%d, MtId:%d\n",
                                          byConfIdx, wChanNo, dwDstIp, wDstPort, dwMtId );
                    }
                    if (SWITCHCHANNEL_UNIFORMMODE_NONE == bySwitchChannelMode)
                    {
                        g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].bUniform = FALSE;
                    }
                    else
                    {
                        g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].bUniform = TRUE;
                    }
                    g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].byPayload  = byUniformPayload;
                
                    g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].byConfNo   = byConfIdx;
                    g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].wChannelNo = wChanNo;
                    ptSendFilterAppData = &g_cMpApp.m_pptFilterData[byConfIdx][wChanNo];

                    MpLog( LOG_INFO, "[SwitchSrcToT] add switch: wChanNo = %d\n", wChanNo );
                }
                else
                {
                    g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].byConfNo   = byConfIdx;
                    ptSendFilterAppData = &g_cMpApp.m_pptFilterData[byConfIdx][wChanNo];
                    MpLog( LOG_INFO, "[SwitchSrcToT] add switch: wChanNo = %d ( NoUni )\n", wChanNo );   
                }
                ::dsSetAppDataForSend( g_cMpApp.m_nInitUdpHandle, 
                                       htonl(dwRcvIp), wRcvPort,
                                       0, 0, htonl(dwDstIp), wDstPort, 
                                       (void *)ptSendFilterAppData );
            }
            else
            {
                MpLog( LOG_CRIT, "[SwitchSrcToT] no pos for this src to T!\n" );
                return FALSE;
            }

            MpLog( LOG_INFO, "[SwitchSrcToT] dsAddManyToOne: %s(%d)-->%s(%d) success!\n",
                                        achRcvIp, wRcvPort, strofip(dwDstIp), wDstPort );
        }
        else
        {
            ::dsRemoveManyToOne( g_cMpApp.m_nInitUdpHandle, 
                                 htonl(dwRcvIp), wRcvPort,
                                 htonl(dwDstIp), wDstPort );
            ::dsRemoveManyToOne( g_cMpApp.m_nInitUdpHandle, 
                                 htonl(dwRcvIp), wRcvPort + 1,
                                 htonl(dwDstIp), wDstPort + 1 );
            MpLog( LOG_CRIT, "[SwitchSrcToT] Fail to add switch table !\n" );
            return FALSE;
        }
    }
    else
    {
        if ( DSOK != dwRet1 )
        {
            ::dsRemoveManyToOne( g_cMpApp.m_nInitUdpHandle, 
                                 htonl(dwRcvIp), wRcvPort,
                                 htonl(dwDstIp), wDstPort );
        }
        if ( DSOK != dwRet2 ) 
        {
            ::dsRemoveManyToOne( g_cMpApp.m_nInitUdpHandle, 
                                 htonl(dwRcvIp), wRcvPort + 1,
                                 htonl(dwDstIp), wDstPort + 1 );
        }
        MpLog( LOG_CRIT, "[SwitchSrcToT] dsAddManyToOne: %s(%d)-->%s(%d) failed, reason<%d,%d>!\n",
                          achRcvIp, wRcvPort, strofip(dwDstIp), wDstPort, dwRet1, dwRet2 );
        return FALSE;
    }

    // 3. 刷新内部源点状态，并通知MCU切换前的源终端, 移除 MT->M->S1/S2 的交换
    if ( bSwitchNow )
    {
        g_cMpApp.SetPtState( byConfIdx, bySrcPoint, PS_SWITCHED );

        if ( PS_IDLE != g_cMpApp.GetPtState( byConfIdx, bySrcPoint, TRUE ) )
        {
            g_cMpApp.SetPtState( byConfIdx, bySrcPoint, PS_IDLE, TRUE );
            g_cMpApp.ProceedSrcChange( byConfIdx, bySrcPoint );
        }
    }
    else
    {
        // 等待关键帧来了作切换，关键帧不来，3s超时切换
        SetTimer( EV_MP_CHGSRCTOT_TIMER, MP_CHGSRCTOT_CHECK_TIMEOUT );
    }

    return TRUE;
}

/*=============================================================================
  函 数 名： ProcChangeSrcTimeout
  功    能： 超时源切换检查
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  07/04/03    4.0         张宝卿        创建
=============================================================================*/
void CMpInst::ProcChangeSrcTimeout( void )
{
    BOOL32 bMpCheckStill = FALSE;

    u8 byConfIdx = 0;
    for( ; byConfIdx < MAX_CONFIDX; byConfIdx ++ )
    {
        BOOL32 bConfCheckStill = FALSE;

        u8 byPointId = 1;
        for( ; byPointId <= POINT_NUM; byPointId ++ )
        {
            if ( PS_GETSRC == g_cMpApp.GetPtState(byConfIdx, byPointId) ) 
            {
                if ( PS_SWITCHED == g_cMpApp.GetPtState(byConfIdx, byPointId, TRUE) ) 
                {
                    u16 wChgDelay = g_cMpApp.GetChgDelay(byConfIdx);
                    if (!g_cMpApp.m_bCancelWaitIFrameNoUni && 
                        wChgDelay < MP_CHGSRCTOT_TIMEOUT )
                    {
                        wChgDelay += MP_CHGSRCTOT_CHECK_TIMEOUT;
                        g_cMpApp.SetChgDelay(byConfIdx, wChgDelay);

                        MpLog( LOG_INFO, "[ChangeSrcTimeout] ConfIdx.%d's Pt.%d delay.%d \n",
                                          byConfIdx, byPointId, wChgDelay );
                    }
                    else
                    {
                        // 超时强切 或 自然过滤
                        g_cMpApp.SetChgDelay(byConfIdx, 0);
                        g_cMpApp.SetPtState(byConfIdx, byPointId, PS_SWITCHED);
                        g_cMpApp.SetPtState(byConfIdx, byPointId, PS_IDLE, TRUE);
                        
                        // 通知mcu旧发言人拆源交换，拆旧的中转交换
                        g_cMpApp.ProceedSrcChange( byConfIdx, byPointId );

                        if (!g_cMpApp.m_bCancelWaitIFrameNoUni)
                        {
                            MpLog( LOG_CRIT, "[ChangeSrcTimeout] ConfIdx.%d's Pt.%d chg to T forcely!\n",
                                              byConfIdx, byPointId );
                        }
                        else
                        {
                            MpLog( LOG_CRIT, "[ChangeSrcTimeout] ConfIdx.%d's Pt.%d chg to T naturely due to debug!\n",
                                              byConfIdx, byPointId );
                        }
                        break;
                    }

                    bMpCheckStill   = TRUE;
                    bConfCheckStill = TRUE;
                }
                else
                {
                    MpLog( LOG_CRIT, "[ChangeSrcTimeout] unexpected point state<%d,%d> \n",
                                      g_cMpApp.GetPtState(byConfIdx, byPointId),
                                      g_cMpApp.GetPtState(byConfIdx, byPointId, TRUE) );
                }
            }
        }
        if ( !bConfCheckStill )
        {
            g_cMpApp.ClearChgDelay( byConfIdx );
        }
    }

    if ( !bMpCheckStill )
    {
        KillTimer( EV_MP_CHGSRCTOT_TIMER );
    }
    else
    {
        SetTimer( EV_MP_CHGSRCTOT_TIMER, MP_CHGSRCTOT_CHECK_TIMEOUT );
    }
    return;
}


/*=============================================================================
  函 数 名： ProcRemoveBroadSwitchSrcReq
  功    能： 移除 广播源 交换
  算法实现： 目前只支持视频做这样的处理
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  07/03/23    4.0         张宝卿        创建
=============================================================================*/
void CMpInst::ProcRemoveBroadSwitchSrcReq(CMessage * const pcMsg)
{
    CServMsg cRevMsg( pcMsg->content, pcMsg->length );
    u8  byConfIdx = cRevMsg.GetConfIdx() - 1;
    
    if ( byConfIdx >= MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[RemoveSrcReq] ConfIdx:%d error \n", byConfIdx+1 );
        SendMsgToMcu( pcMsg->event + 2 );
        return;
    }
    
    TSwitchChannel tSwitchChannel = *(TSwitchChannel*)cRevMsg.GetMsgBody();
    u32 dwSrcIp  = tSwitchChannel.GetSrcIp();
    u32 dwRcvIp  = tSwitchChannel.GetRcvIP();
    u32 dwDstIp  = tSwitchChannel.GetRcvIP();

    // zbq [04/26/2007] 针对多MP删除源，本接收地址可能不为本地地址，刷新之
	//8000H/G/I不需要修改RecvIP[5/25/2012 chendaiwei]
#if !defined(_8KH_) && !defined(_8KE_) && !defined(_8KI_)	
    if ( dwRcvIp != g_cMpApp.m_dwIpAddr ) 
    {
        dwRcvIp = g_cMpApp.m_dwIpAddr;
        dwDstIp = g_cMpApp.m_dwIpAddr;
        tSwitchChannel.SetRcvIP(dwRcvIp);
        tSwitchChannel.SetDstIP(dwDstIp);
    }
#endif

    u16 wDstPort = tSwitchChannel.GetDstPort();
    u16 wRcvPort = tSwitchChannel.GetRcvPort();
    

    // 两个源中转，哪个不空移哪个。两个都不空，两个都移。
    u8 byPID = 1;
    for( ; byPID <= POINT_NUM; byPID ++ )
    {
        if ( PS_IDLE != g_cMpApp.GetPtState( byConfIdx, byPID ) )
        {
            wDstPort = CONFBRD_STARTPORT + byConfIdx * PORTSPAN + byPID * POINT_NUM;
            tSwitchChannel.SetDstPort(wDstPort);
        }
        else
        {
            continue;
        }

        g_cMpApp.SetPtState( byConfIdx, byPID, PS_IDLE );
        
        // zbq [05/25/2007] 移源的时候存在若为双源，手动清除 非当前界面源
        TSwitchChannel tSwitchSrc;
        if ( !g_cMpApp.SearchSwitchTable(byConfIdx, dwDstIp, wDstPort, tSwitchSrc) )
        {
            MpLog( LOG_CRIT, "[RemoveSrcReq] switch <%s@%d> to be remove unexist\n",
                              strofip(dwDstIp), wDstPort );
            continue;
        }
        if ( tSwitchChannel.GetSrcIp() != tSwitchSrc.GetSrcIp() )
        {
            g_cMpApp.ProceedSrcChange( byConfIdx, byPID, FALSE );
            continue;
        }
        

        u32 dwRet1 = DSOK;
        u32 dwRet2 = DSOK;
        
        dwRet1 = ::dsRemove(g_cMpApp.m_nInitUdpHandle, htonl(dwDstIp), wDstPort);
        dwRet2 = ::dsRemove(g_cMpApp.m_nInitUdpHandle, htonl(dwDstIp), wDstPort+1);

        cRevMsg.SetSrcDriId(g_cMpApp.m_byMpId);    
        if( DSOK == dwRet1 && DSOK == dwRet2 )
        {
            s8 achSrcIp[16];
            s8 achRcvIp[16];
            s8 achDstIp[16];
            strcpy( achSrcIp, strofip(dwSrcIp) );
            strcpy( achRcvIp, strofip(dwRcvIp) );
            strcpy( achDstIp, strofip(dwDstIp) );

            MpLog( LOG_INFO, "[RemoveSrcReq] dsRemove <%s@%d>-><%s@%d>-><%s@%d> Ok !\n", 
                              achSrcIp, wRcvPort,
                              achRcvIp, wRcvPort,
                              achDstIp, wDstPort );
            
            BOOL32 bFlag = g_cMpApp.RemoveSwitchTableSrc(byConfIdx, &tSwitchChannel);
            if( bFlag )
            {
                if( CONF_UNIFORMMODE_VALID == g_cMpApp.GetConfUniMode(byConfIdx) )
                {
                    u16 wChanNo = g_cMpApp.GetChanNoByDst(byConfIdx, dwDstIp, wDstPort, 1);
                    if (wChanNo != MAX_SWITCH_CHANNEL)
                    {
                        g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].bRemoved = TRUE;
                    }
                    else
                    {
                        MpLog(LOG_CRIT, "[RemoveSrcReq] ChnNo:%d unexist !\n", wChanNo);
                    }
                }
            }
            else
            {
                MpLog( LOG_CRIT, "[RemoveSrcReq] Remove <%s@%d> from table failed !\n",
                                                          strofip(dwSrcIp), wRcvPort );
            }
            SendMsg2Mcu( pcMsg->event+1, cRevMsg );
        }
        else
        {
            SendMsg2Mcu( pcMsg->event + 2, cRevMsg );
            MpLog( LOG_CRIT, "[RemoveSrcReq] dsRemove <%s@%d> failed !\n", 
                                             strofip(dwSrcIp), wRcvPort );
        }

        // 移除 M -> S1/S2 -> T 的交换
        u32 dwRet3 = DSOK;
        dwRet3 = ::dsRemoveManyToOne( g_cMpApp.m_nInitUdpHandle,
                                      htonl(dwRcvIp), wDstPort,
                                      htonl(dwRcvIp), CONFBRD_STARTPORT + byConfIdx * PORTSPAN );
        TSwitchChannel tSrc2TChan;
        tSrc2TChan.SetRcvIP( dwRcvIp );
        tSrc2TChan.SetRcvPort( wDstPort );
        tSrc2TChan.SetDstIP( dwRcvIp );
        tSrc2TChan.SetDstPort( CONFBRD_STARTPORT + byConfIdx * PORTSPAN );
        if ( DSOK == dwRet3 )
        {
            MpLog( LOG_INFO, "[RemoveSrcReq] dsRemoveManyToOne rcv<0x%x:%d>, dst<0x%x:%d> !\n",
                        dwRcvIp, wDstPort, dwRcvIp, CONFBRD_STARTPORT + byConfIdx * PORTSPAN );

            BOOL32 bFlag = g_cMpApp.RemoveSwitchTable(byConfIdx, &tSrc2TChan, SWITCH_MUL2ONE);
            if( bFlag )
            {
                if( CONF_UNIFORMMODE_VALID == g_cMpApp.GetConfUniMode(byConfIdx) )
                {
                    u16 wChanNo = g_cMpApp.GetChanNoByDst( byConfIdx, 
                                                     tSrc2TChan.GetDstIP(), 
                                                     tSrc2TChan.GetDstPort(), 1, 
                                                     tSrc2TChan.GetRcvPort() );
                    
                    MpLog( LOG_INFO, "[RemoveSrcReq] GetChanNoByDst.wRcvPort.%d !\n", wRcvPort );

                    if (wChanNo != MAX_SWITCH_CHANNEL)
                    {
                        g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].bRemoved = TRUE;
                    }
                    else
                    {
                        MpLog(LOG_CRIT, "[RemoveSrcReq] ChnNo:%d unexist!\n", wChanNo);
                    }
                }
            }
            else
            {
                MpLog( LOG_CRIT, "[RemoveSrcReq] RemoveSwitchTable failed !\n");
            }
        }
        else
        {
            MpLog( LOG_CRIT, "[RemoveSrcReq] dsRemoveManyToOne <0x%x:%d>--><0x%x:%d> !\n",
                              dwRcvIp, wDstPort, dwRcvIp, CONFBRD_STARTPORT + byConfIdx * PORTSPAN );
        }
    }
    
    // 移除 MT->M->S1/S2 的交换
    g_cMpApp.NtfMcuSrcSwitchChg( byConfIdx, tSwitchChannel );
    
    
    // 移除 M 的Dump
    u32 dwRet4 = ::dsRemoveDump( g_cMpApp.m_nInitUdpHandle, 
                                 htonl(tSwitchChannel.GetRcvIP()), tSwitchChannel.GetRcvPort() );
    if ( DSOK != dwRet4 )
    {
        // FIXME: 这个基于桥交换的dump为什么会移除失败
		// to FIXME: ip应填成网络序
        MpLog( LOG_ERR, "[RemoveSrcReq] dsRemoveDump <%s@%d> failed !\n",
                          strofip(tSwitchChannel.GetRcvIP()), tSwitchChannel.GetRcvPort());
    }

    return;
}

/*=============================================================================
函 数 名： ProcAddSendSelfMutePackCmd
功    能： 增加给自己发哑音包表项
算法实现： 
全局变量： 
参    数：CServMsg &cServMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2012/09/26  4.7			周翼亮                  创建
=============================================================================*/
void CMpInst::ProcAddSendSelfMutePackCmd(CMessage* const pcMsg)
{
	if (NULL == pcMsg)
	{
		MpLog(LOG_ERR,"[ProcAddSendSelfMutePackCmd]ERROR:pcMsg is Null!\n");
		return;
	}
	CServMsg cServMsg(pcMsg->content,pcMsg->length);
	TSendMutePack tSendMutePack = *(TSendMutePack*)(cServMsg.GetMsgBody());
	
	g_cMpApp.AddSendMutePack(tSendMutePack);
}

/*=============================================================================
函 数 名： ProcDelSendSelfMutePackCmd
功    能： 删除一条给自己发哑音包表项
算法实现： 
全局变量： 
参    数： 
CServMsg &cServMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2012/09/26  4.7			周翼亮                  创建
=============================================================================*/
void CMpInst::ProcDelSendSelfMutePackCmd(CMessage* const pcMsg)
{
	if (NULL == pcMsg)
	{
		MpLog(LOG_ERR,"[ProcDelSendSelfMutePackCmd]ERROR:pcMsg is Null!\n");
		return;
	}
	CServMsg cServMsg(pcMsg->content,pcMsg->length);
	TMt tDelMt = *(TMt*)(cServMsg.GetMsgBody());
	g_cMpApp.DelSendMutePackByConfIdx(tDelMt.GetConfIdx(),tDelMt.GetMtId());
	
}

/*=============================================================================
  函 数 名： ProcMcuMpPinHoleNotify
  功    能： 打洞消息处理
  算法实现： H460打洞
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  2013/05/09  4.7.2		  朱胜泽			创建
=============================================================================*/ 
void CMpInst::ProcMcuMpPinHoleNotify(CMessage* const pcMsg)
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TGwPinholeInfo tInfo;
	PinHoleInfo    tPinHoleInfp;
	u16 wIndex = 0;

	// 本地地址
	tInfo.m_dwLocalIP  = *(u32 *)(cServMsg.GetMsgBody());
	wIndex += sizeof(u32);

	// 本地端口
	tInfo.m_wLocalPort = *(u16 *)(cServMsg.GetMsgBody() + wIndex);
	wIndex += sizeof(u16);
	
	// 远端地址
	tInfo.m_dwDestIP   = *(u32 *)(cServMsg.GetMsgBody() + wIndex);
	wIndex += sizeof(u32);

	// 远端端口
	tInfo.m_wDestPort  = *(u16 *)(cServMsg.GetMsgBody() + wIndex);
	wIndex += sizeof(u16);

	// RTP打洞类型
	u8 byPinHoleType   = *(u8 *)(cServMsg.GetMsgBody() + wIndex);
	wIndex += sizeof(u8);

	// 动态载荷
	tInfo.m_byPayLoad  = *(u8 *)(cServMsg.GetMsgBody() + wIndex);
    
	//保存打洞信息
	tPinHoleInfp.m_wLocalPort  = tInfo.m_wLocalPort;
	tPinHoleInfp.m_dwRemoteIp  = tInfo.m_dwDestIP;
	tPinHoleInfp.m_wRemotePort = tInfo.m_wDestPort;
    tPinHoleInfp.m_emType      = (EMGwPinholeType)byPinHoleType;
    tPinHoleInfp.m_byPlayLoad  = tInfo.m_byPayLoad;
    
    for (u16 wLoop=0; wLoop<MAX_SWITCH_CHANNEL; wLoop++)
    {
        PinHoleInfo ptPinHoleInfp = g_cMpApp.GetPinHoleItem(wLoop);
        if (0 == ptPinHoleInfp.m_wLocalPort)
        {
            g_cMpApp.SetPinHoleTable(tPinHoleInfp, wLoop);
            break;
        }
    }

    MpLog( LOG_DETAIL, "[ProcMcuMpPinHoleNotify] LocalAddr<0x%x:%d>  RemoteAddr<0x%x:%d>  PinHoleType<%d> PayLoad<%d> ConfIdx:%d \n",
		tInfo.m_dwLocalIP, tInfo.m_wLocalPort, tInfo.m_dwDestIP, tInfo.m_wDestPort, byPinHoleType, tInfo.m_byPayLoad, cServMsg.GetConfIdx() );
    
    // 调用打洞接口
    KdvTSAddPinhole(tInfo, g_cMpApp.GetPinHoleInterval(), tPinHoleInfp.m_emType);    
}

/*=============================================================================
  函 数 名： PorcMcuMpStopPinHoleNotify
  功    能： 补洞消息处理
  算法实现： 目前只支持视频做这样的处理
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  2013/05/10  4.7.2		 zhushengze			创建
=============================================================================*/
void CMpInst::PorcMcuMpStopPinHoleNotify(CMessage* const pcMsg)
{
	CServMsg     cServMsg( pcMsg->content, pcMsg->length );

	u16 wIndex = 0;

	// 补洞的数量
	u8  byPinHoleNum   = *(u8 *)(cServMsg.GetMsgBody());
	wIndex += sizeof(u8);

    for ( u8 byHoleIndex = 0; byHoleIndex < byPinHoleNum; byHoleIndex++ )
    {
        // 本地接收端口
        u16 wLocalRcvPort  = *(u16 *)(cServMsg.GetMsgBody() + wIndex);
        wIndex += sizeof(u16);
        
        // 远端接收地址
        u32 dwRemoteIpAddr = *(u32 *)(cServMsg.GetMsgBody() + wIndex);
        wIndex += sizeof(u32);
        
        // 远端接收端口
        u16 wRemotePort    = *(u16 *)(cServMsg.GetMsgBody() + wIndex);
        wIndex += sizeof(u16);
        
        PinHoleInfo ptPinHoleInfp; 
        PinHoleInfo tNullPinHoleInfp;
        memset(&tNullPinHoleInfp, 0, sizeof(PinHoleInfo));
                
        for ( u16 wLoop=0; wLoop<MAX_SWITCH_CHANNEL; wLoop++ )
        {
            //目前底层补洞只需要本地端口，为保证信息的统一，这里也不校验远端信息
            //远端信息为日后扩展保留，存在一个本地端口对应多个远端地址的情况
            ptPinHoleInfp = g_cMpApp.GetPinHoleItem(wLoop);
            if ( wLocalRcvPort == ptPinHoleInfp.m_wLocalPort //&& 
                /* dwRemoteIpAddr == ptPinHoleInfp.m_dwRemoteIp &&*/
                 /*wRemotePort == ptPinHoleInfp.m_wRemotePort*/)
            {
                g_cMpApp.SetPinHoleTable(tNullPinHoleInfp, wLoop);
                //break;
            }
        }
                
        MpLog( LOG_CRIT, "[PorcMcuMpStopPinHoleNotify] LocalPort<%d> RemoteAddr<0x%x:%d> ConfIdx:%d\n",
            wLocalRcvPort, dwRemoteIpAddr, wRemotePort, cServMsg.GetConfIdx() );
        
        //补洞
	    KdvTSDelPinhole(wLocalRcvPort);
    }
}

/*=============================================================================
  函 数 名： ProcAddBroadSwitchDstReq
  功    能： 添加 广播目标 交换
  算法实现： 目前只支持视频做这样的处理
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  07/03/23    4.0         张宝卿        创建
  07/04/20    4.0         张宝卿        增加非归一重整的回调支持
=============================================================================*/
void CMpInst::ProcAddBroadSwitchDstReq(CMessage * const pcMsg)
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );
    
    u8  byConfIdx = cServMsg.GetConfIdx() - 1;
    
    if ( byConfIdx >= MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[AddSwitchDstReq] ConfIdx:%d error \n", byConfIdx+1 );
        SendMsgToMcu( pcMsg->event + 2 );
        return;
    }
    
    //限制MP转发流量处理
    RestrictFluxOfMp( pcMsg );

    cServMsg.SetSrcDriId(g_cMpApp.m_byMpId);

    u16 wMsgBodyLen = cServMsg.GetMsgBodyLen();
    if (wMsgBodyLen % (sizeof(TSwitchChannelExt) + sizeof(u8) * 3) != 0)
    {
        MpLog( LOG_CRIT, "[AddSwitchDstReq] msg body len.%d err, ignore it!\n", wMsgBodyLen );
        SendMsg2Mcu( pcMsg->event + 2, cServMsg );
        return;
    }
    
    u8 byDstMtNum = wMsgBodyLen / (sizeof(TSwitchChannelExt) + sizeof(u8) * 3);

    CServMsg cReplyMsg;
    cReplyMsg.SetSrcDriId( g_cMpApp.m_byMpId );
    cReplyMsg.SetConfIdx( byConfIdx + 1 );
    
    u16 wOffSet = 0;

    // 逐个增加目标地址交换
    u8 byDstId = 0;
    for( ; byDstId < byDstMtNum; byDstId ++ )
    {
        TSwitchChannelExt tSwitchChannel = *(TSwitchChannelExt*)(cServMsg.GetMsgBody() + wOffSet);
        u8  bySwitchChannelMode = *(u8*)(cServMsg.GetMsgBody() + sizeof(TSwitchChannelExt) + wOffSet);
        u8  byUniformPayload    = *(u8*)(cServMsg.GetMsgBody() + sizeof(TSwitchChannelExt) + 1 + wOffSet);
        //u8  byIsSwitchRtcp      = *(u8*)(cServMsg.GetMsgBody() + sizeof(TSwitchChannelExt) + 2 + wOffSet);
     
        wOffSet += sizeof(TSwitchChannelExt) + 3;

        MpLog( LOG_INFO, "[AddSwitchDstReq] byChnMode.%d, byUniformload.%d !\n", 
                                        bySwitchChannelMode, byUniformPayload );

        u32 dwRevIp     = tSwitchChannel.GetRcvIP();
        u16 wRevPort    = tSwitchChannel.GetRcvPort();
        u32 dwRevBindIp = tSwitchChannel.GetRcvBindIP();

        u32 dwDstIp     = tSwitchChannel.GetDstIP();
        u16 wDstPort    = tSwitchChannel.GetDstPort();
        u32 dwSndBindIp = tSwitchChannel.GetSndBindIP();

		MpLog(LOG_INFO,"Param List: dwRevIP: %x wRecvPort:%x, dwRevBindIp:%x,dwDstIp:%x,wDstPort:%d, dwSndBindIp:%x\n",
			dwRevIp,wRevPort,dwRevBindIp,dwDstIp,wDstPort,dwSndBindIp);

        // zbq [04/23/2007] 过滤上层业务重复发同一个增加目标交换的请求
        TSwitchChannel tTmpChan;
        if ( g_cMpApp.SearchSwitchTable(byConfIdx, dwDstIp, wDstPort, tTmpChan) )
        {
            // zbq [04/28/2007] 只过滤重复广播交换，普通交换仍然重新建立
			if ( g_cMpApp.m_dwIpAddr == tTmpChan.GetRcvIP() && CONFBRD_STARTPORT + byConfIdx * PORTSPAN == tTmpChan.GetRcvPort() )
			{
	            // zgc, 2008-05-27, 修改为新的广播源
                if ( !(tTmpChan.GetSrcMt() == tSwitchChannel.GetSrcMt()) )
                {
                    MpLog( LOG_INFO, "[AddSwitchDstReq] old src<type.%d,id%d>-->new src<type.%d,id%d>\n",
                                    tTmpChan.GetSrcMt().GetType(), tTmpChan.GetSrcMt().GetMtId(),
                                    tSwitchChannel.GetSrcMt().GetType(), tSwitchChannel.GetSrcMt().GetMtId() );
                    TMt tNewSrc = tSwitchChannel.GetSrcMt();
                    tTmpChan.SetSrcMt( tNewSrc );
                    g_cMpApp.AddSwitchTable( byConfIdx, &tTmpChan, SWITCH_ONE2ONE );
                }
                MpLog( LOG_WARN, "[AddSwitchDstReq] dst<0x%x@%d> exist already !\n",
                                                                dwDstIp, wDstPort );
                continue;
            }
            else
            {
                MpLog( LOG_INFO, "[AddSwitchDstReq] Rcv<0x%x@%d>->Dst<0x%x@%d> exist as normal switch, replace it !\n",
                                  tTmpChan.GetRcvIP(), tTmpChan.GetRcvPort(), 
                                  dwDstIp, wDstPort );
            }
        }

        //u32 dwSrcIp     = dwRevIp;
        tSwitchChannel.SetSrcIp(dwRevIp);

        dwRevIp         = dwRevBindIp;
        wRevPort        = CONFBRD_STARTPORT + byConfIdx * PORTSPAN;

        tSwitchChannel.SetRcvIP( dwRevIp );
        tSwitchChannel.SetRcvPort( wRevPort );

        u32 dwResult1 = DSOK;
        u32 dwResult2 = DSOK;
		MpLog(LOG_INFO,"[dsAdd]Param:dwRevIP%x wRevPort%d dwRevBindIP:%x dwDstIp:%x wDstPort:%d dwSndBindIp:%x\n",
			dwRevIp,wRevPort,dwRevBindIp,dwDstIp,wDstPort,dwSndBindIp);

        dwResult1 = ::dsAdd( g_cMpApp.m_nInitUdpHandle, 
                              htonl(dwRevIp), wRevPort, 
                              htonl(dwRevBindIp), 
                              htonl(dwDstIp), wDstPort,
                              htonl(dwSndBindIp) );
    
        // guzh [10/30/2007] 该交换没有存在意义
        /*
        if ( byIsSwitchRtcp ) 
        {
            dwResult2 = ::dsAdd( g_cMpApp.m_nInitUdpHandle, 
                                  htonl(dwRevIp), wRevPort + 1,
                                  htonl(dwRevBindIp),
                                  htonl(dwDstIp), wDstPort + 1,
                                  htonl(dwSndBindIp) );
        }
        */

        cServMsg.SetSrcDriId(g_cMpApp.m_byMpId);

        if( DSOK == dwResult1 && DSOK == dwResult2 )
        {
            BOOL32 bFlag = g_cMpApp.AddSwitchTable( byConfIdx, &tSwitchChannel, SWITCH_ONE2ONE );
            if( bFlag )
            {
                if(NULL == g_cMpApp.m_pptFilterData)
                {
                    MpLog( LOG_CRIT, "[AddSwitchDstReq] err: NULL == m_pptData\n");
                    return;
                }

                ::dsSetSendCallback( g_cMpApp.m_nInitUdpHandle, 
                                     htonl(dwRevIp), wRevPort,
                                     0, 0, MpSendFilterCallBackProc );

                u16 wChanNo = g_cMpApp.GetChanNoByDst( byConfIdx, dwDstIp, wDstPort, 0 );
                if ( wChanNo != MAX_SWITCH_CHANNEL )
                {
                    // 回调数据整理，回调设置
                    TSendFilterAppData * ptSendFilterAppData;

                    if( CONF_UNIFORMMODE_VALID == g_cMpApp.GetConfUniMode(byConfIdx) )
                    {
                        // xsl [5/11/2006]等关键帧处理
                        u32 dwMtId = tSwitchChannel.GetSrcMt().GetMtId();                    
                        if (g_cMpApp.m_bIsWaitingIFrame && wRevPort % PORTSPAN == 0)
                        {                		
                            g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].bWaitingIFrame = g_cMpApp.m_bIsWaitingIFrame;
                            g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].dwFstTimeStamp = 0;
                            g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].dwSrcIp  = dwRevIp;
                            g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].wSrcPort = wRevPort;
                            g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].byConfNo = byConfIdx;
                            g_cMpApp.m_pptWaitIFrmData[byConfIdx][dwMtId-1].byMtId   = (u8)dwMtId;
                            SetTimer(WAITIFRAME_START_TIMER+byConfIdx*(MAXNUM_CONF_MT+POINT_NUM) + dwMtId, 5000, dwMtId);
                            MpLog( LOG_INFO, "[AddSwitchDstReq] SetTimer waiting I Frame, byconfno :%d, wchannelno :%d, dwDstIp :0x%x, dwDstPort :%d, MtId:%d\n",
                                              byConfIdx, wChanNo, dwDstIp, wDstPort, dwMtId );
                        }
                        if (SWITCHCHANNEL_UNIFORMMODE_NONE == bySwitchChannelMode)
                        {
                            g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].bUniform = FALSE;
                        }
                        else
                        {
                            g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].bUniform = TRUE;
							//端口作假
							if (tSwitchChannel.GetMapIp() == 0 || tSwitchChannel.GetMapPort() == 0)
							{
								MpLog(LOG_INFO,"[ProcAddSwitchReq] no mappedip or port!\n");
							}
							else//设置了作假的ip和端口
							{
								MpLog(LOG_INFO,"[ProcAddBroadSwitchDstReq]dstip:%d,port:%d ,MappedIp:%d,MappedPort:%d",dwDstIp,wDstPort,tSwitchChannel.GetMapIp(),tSwitchChannel.GetMapPort());
								u32 dwRet = dsSpecifyDstFwdSrc( g_cMpApp.m_nInitUdpHandle, htonl(dwRevIp),wRevPort,
									htonl(dwDstIp), wDstPort, 
									htonl(tSwitchChannel.GetMapIp()), tSwitchChannel.GetMapPort());
								if (DSOK != dwRet)
								{
									
									MpLog( LOG_CRIT, "[ProcAddBroadSwitchDstReq] dsSpecifyDstFwdSrc failed!\n");
								}
							}
                        }
                        g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].byPayload  = byUniformPayload;
                        g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].byConfNo   = byConfIdx;
                        g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].wChannelNo = wChanNo;
                        ptSendFilterAppData = &g_cMpApp.m_pptFilterData[byConfIdx][wChanNo];

                        MpLog( LOG_INFO, "[AddSwitchDstReq] add switch: wChanNo = %d\n", wChanNo );
                    }
					else
                    {
						g_cMpApp.m_pptFilterData[byConfIdx][wChanNo].byConfNo   = byConfIdx;
						ptSendFilterAppData = &g_cMpApp.m_pptFilterData[byConfIdx][wChanNo];
						MpLog( LOG_INFO, "[AddSwitchDstReq] add switch: wChanNo = %d ( NoUni )\n", wChanNo );
                        
                    }
					::dsSetAppDataForSend(g_cMpApp.m_nInitUdpHandle, 
						htonl(dwRevIp), wRevPort,
						0, 0, htonl(dwDstIp), wDstPort,
						(void *)ptSendFilterAppData );
                }
                else
                {
                    MpLog( LOG_CRIT, "[AddSwitchDstReq] no pos for this switchDst!\n" );
                    return;
                }

                cReplyMsg.CatMsgBody( (u8*)&tSwitchChannel, sizeof(TSwitchChannel) );
                s8 achRcvIp[18] = { 0 };
                strofip(dwRevIp, achRcvIp);
                MpLog( LOG_INFO, "[AddSwitchDstReq] Add switch: %s(%d)--->%s(%d) success !\n",
                                  achRcvIp, wRevPort, 
                                  strofip(tSwitchChannel.GetDstIP()), tSwitchChannel.GetDstPort() );
            }
            else
            {
                MpLog( LOG_CRIT, "[AddSwitchDstReq] Fail add table !\n" );
                ::dsRemove(g_cMpApp.m_nInitUdpHandle, htonl(dwRevIp), wRevPort);
                ::dsRemove(g_cMpApp.m_nInitUdpHandle, htonl(dwRevIp), wRevPort+1);
                SendMsg2Mcu(pcMsg->event + 2, cServMsg );
            }
        }
        else
        {
            SendMsg2Mcu( pcMsg->event + 2, cServMsg );
            MpLog( LOG_CRIT, "[AddSwitchDstReq] dsAdd %s(%d) Failure, reason<%d,%d>!\n", 
                              strofip(tSwitchChannel.GetDstIP()), tSwitchChannel.GetDstPort(), 
                              dwResult1, dwResult2 );
        }
    }

    // 所有增加成功的交换通知MCU记录
    if ( cReplyMsg.GetMsgBodyLen() > 0 ) 
    {
        SendMsg2Mcu( pcMsg->event + 1, cReplyMsg );
    }
    
    return;
}


/*=============================================================================
  函 数 名： ProcConfUniformModeNotify
  功    能： 处理归一化
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
=============================================================================*/
void CMpInst::ProcConfUniformModeNotify(CMessage * const pcMsg)
{
    CServMsg cRevMsg(pcMsg->content, pcMsg->length);
	u8 byConfIdx = cRevMsg.GetConfIdx() - 1;

    if (byConfIdx >= MAX_CONFIDX)
    {
        MpLog( LOG_CRIT, "[UniformModeNotify] ConfIdx:%d error\n", byConfIdx);
        return;
    }

	u8 byConfMode = *(u8 *)cRevMsg.GetMsgBody();
    g_cMpApp.SetConfUniMode( byConfIdx, byConfMode );
    MpLog( LOG_INFO, "[UniformModeNotify] ConfUniform.%d, byConfIdx:%d \n", byConfMode, byConfIdx);
    
    return;
}

/*=============================================================================
 函 数 名： ProcGetStatusReq
 功    能： 
 算法实现： 
 全局变量： 
 参    数： CMessage * const pcMsg
 返 回 值： void 
=============================================================================*/
void CMpInst::ProcGetStatusReq(CMessage * const pcMsg)
{
	if (NULL == pcMsg)
	{
		MpLog(LOG_ERR, "[ProcGetStatusReq] pcMsg is null!\n");
	}
    SendMsgToMcu(MP_MCU_GETSWITCHSTATUS_ACK);
    return;
}

/*=============================================================================
  函 数 名： ProcReleaseConfNotify
  功    能： 结束会议
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  07/04/13    4.0         张宝卿        代码优化
=============================================================================*/
void CMpInst::ProcReleaseConfNotify(CMessage * const pcMsg)
{
    if( NULL == pcMsg )
    {
        MpLog( LOG_CRIT, "[ReleaseConfNotify] param err: pcMsg.0x%x\n", pcMsg );
        return;
    }
    
    CServMsg cRevMsg(pcMsg->content, pcMsg->length);
    u8 byConfIdx = cRevMsg.GetConfIdx() - 1;
    if(byConfIdx >= MAX_CONFIDX)
    {
        MpLog( LOG_CRIT, "[ReleaseConfNotify] ConfIdx:%d error \n", byConfIdx+1 );
        SendMsg2Mcu( pcMsg->event + 2, cRevMsg );
        return;
    }
    
	s8 achRcvIp[18] = {0};

	// 强制进行T点的dump清除
	::dsRemoveDump(g_cMpApp.m_nInitUdpHandle, 
		           htonl(g_cMpApp.m_dwIpAddr), CONFBRD_STARTPORT + byConfIdx * PORTSPAN);
	
    // 以防清除的不够干净
    u16 wTableId = 0;
    for( ; wTableId < MAX_SWITCH_CHANNEL; wTableId ++ )
    {
        TSwitchChannel tSwitchChannel;
        if ( !g_cMpApp.GetSwitchChan(byConfIdx, wTableId, &tSwitchChannel) )
        {
            MpLog( LOG_CRIT, "[ReleaseConfNotify] get switch<%d, %d> failed \n", byConfIdx, wTableId );
            continue;
        }

        if ( tSwitchChannel.GetDstIP() != 0 )
        {
            //Get addr			
            u32 dwDstIp  = tSwitchChannel.GetDstIP();
            u16 wDstPort = tSwitchChannel.GetDstPort();
            
            u32 dwRet1 = DSOK;

			//S->T交换为多到一，应调用dsRemoveManyToOne，zgc, 2007-11-02
			u32 dwRcvIp = tSwitchChannel.GetRcvIP();
			u16 wRcvPort = tSwitchChannel.GetRcvPort();
			if ( dwRcvIp == g_cMpApp.m_dwIpAddr
				&& dwDstIp == g_cMpApp.m_dwIpAddr
				&& g_cMpApp.IsPortSrcToT(byConfIdx,wRcvPort) 
				&& g_cMpApp.IsPointT(wDstPort) )
			{
				dwRet1 = ::dsRemoveManyToOne(g_cMpApp.m_nInitUdpHandle,
											htonl(dwRcvIp), wRcvPort, htonl(dwDstIp), wDstPort);
				// S->T无RTCP交换
			}
			else
            {
				dwRet1 = ::dsRemove(g_cMpApp.m_nInitUdpHandle, htonl(dwDstIp), wDstPort);
			}

			memset( achRcvIp, 0, sizeof(achRcvIp) );
			strofip(dwRcvIp, achRcvIp);
			if ( DSOK != dwRet1)
			{
				MpLog( LOG_CRIT, "[ReleaseConfNotify] Remove switch Src<%s@%d>->Rcv<%s@%d> failed <%d>\n", 
													achRcvIp, wRcvPort,
													strofip(dwDstIp), wDstPort,
													dwRet1 );
			}
        }
    }
    // 此处要再清一次表
    g_cMpApp.ClearSwitchTable( FALSE, byConfIdx );
    
    // zbq [04/13/2007] 结束会议清节点状态
    u8 byPID = 1;
    for ( ; byPID <= POINT_NUM; byPID ++ )
    {
        g_cMpApp.SetPtState( byConfIdx, byPID, PS_IDLE );
    }
    g_cMpApp.SetConfUniMode( byConfIdx, 0xff );
    g_cMpApp.MpResetSendFilterParam(byConfIdx);

    return;
}

/*=============================================================================
  函 数 名： ProcCallBackSwitchChgNtf
  功    能： 回调导致的切换的后续处理
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  07/04/30    4.0         张宝卿        创建
=============================================================================*/
void CMpInst::ProcCallBackSwitchChgNtf( CMessage* const pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u8 byConfIdx = cServMsg.GetConfIdx() - 1;
    u8 byPID     = *(u8*)cServMsg.GetMsgBody();

    g_cMpApp.ProceedSrcChange( byConfIdx, byPID );
    
    return;
}

/*=============================================================================
  函 数 名： ProcAddTable
  功    能： 增加交互表：测试用
  算法实现： 
  全局变量： 
  参    数： CMessage* const pcMsg
  返 回 值： void 
=============================================================================*/
void CMpInst::ProcAddTable(CMessage* const pcMsg)
{
    CServMsg cRevMsg(pcMsg->content, pcMsg->length);
    u8  byConfIdx = cRevMsg.GetConfIdx() - 1;
    BOOL32 bResult = FALSE;

    TSwitchChannel tSwitchChannel = *(TSwitchChannel*)cRevMsg.GetMsgBody();
    bResult = g_cMpApp.AddSwitchTable(byConfIdx, &tSwitchChannel, SWITCH_ONE2ONE);

    if( bResult )
    {
        SendMsg2Mcu( pcMsg->event+1, cRevMsg );
    }
    else
    {
        SendMsg2Mcu( pcMsg->event+2, cRevMsg );
    }
    return;
}   

/*=============================================================================
  函 数 名： ShowSwitch
  功    能： 显示交换信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/20    4.0			张宝卿                代码整理、移植
=============================================================================*/
void CMpInst::ShowSwitch()
{
    u32 dwSrcIp, dwDisIp, dwRevIp, dwDstIp;
    u16 wRevPort, wDstPort;
    TSwitchChannel tSwitchChannel;

    OspPrintf( TRUE, FALSE,"\n-----------------------------switch info-----------------------------\n");

    if(NULL == g_cMpApp.m_pptSwitchChannel)
    {
        MpLog( LOG_CRIT, "[ShowSwitch] NULL == m_pptSwitchChannel\n");
        return;
    }

    g_byPageNum++;

    OspPrintf( TRUE, FALSE, "Page.%d\n", g_byPageNum );

    u16     wSwitchNum = 0;
    BOOL32  bPrtNextPage = FALSE;

    for(int nLp = MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE-1; nLp >= 0 ; nLp-- )
    {
        for(int nLp1 = MAX_SWITCH_CHANNEL-1; nLp1 >= 0; nLp1--)
        {
            tSwitchChannel = g_cMpApp.m_pptSwitchChannel[nLp][nLp1];
            if(!tSwitchChannel.IsNull())
            {
                u8 byConfIdx = 0;
                u8 byPointId = 0;
                
                wSwitchNum ++;

                if ( wSwitchNum <= (g_byPageNum-1) * MAX_SWITCH_PERPAGE )
                {
                    continue;
                }
                if ( wSwitchNum > g_byPageNum * MAX_SWITCH_PERPAGE )
                {
                    wSwitchNum --;
                    bPrtNextPage = TRUE;
                    SetTimer( EV_SWITCHTABLE_PRTNEXTPAGE_TEMER, 100 );
                    break;
                }

                dwSrcIp = tSwitchChannel.GetSrcIp();
                dwDisIp = tSwitchChannel.GetDisIp();
                dwRevIp = tSwitchChannel.GetRcvIP();
                dwDstIp = tSwitchChannel.GetDstIP();

                wRevPort = tSwitchChannel.GetRcvPort();
                wDstPort = tSwitchChannel.GetDstPort();
            
                u16 wSrcPort  = 0;

                if ( g_cMpApp.IsPointT(wRevPort) )
                {
                    byConfIdx = (wRevPort - CONFBRD_STARTPORT) / PORTSPAN;

                    for( u8 byPId = 1; byPId <= POINT_NUM; byPId ++ )
                    {
                        if ( PS_SWITCHED == g_cMpApp.GetPtState(byConfIdx, byPId) ) 
                        {
                            wSrcPort = CONFBRD_STARTPORT + byConfIdx * PORTSPAN + byPId * POINT_NUM;
                            break;
                        }                        
                    }
                    OspPrintf(TRUE, FALSE, "\nConf%d %d  %s:%d", nLp+1, nLp1, strofip(dwSrcIp), wSrcPort);
                }
                else if ( g_cMpApp.IsPointSrc(wRevPort, &byConfIdx, &byPointId) )
                {
                    TSwitchChannel tSwitchSrc;
                    g_cMpApp.SearchSwitchTable( byConfIdx, dwDstIp, wRevPort, tSwitchSrc );
                    wSrcPort = tSwitchSrc.GetRcvPort();
                    
                    OspPrintf(TRUE, FALSE, "\nConf%d %d  %s:%d", nLp+1, nLp1, strofip(dwSrcIp), wSrcPort);
                }
                else
                {
                    OspPrintf(TRUE, FALSE, "\nConf%d %d  %s:%d", nLp+1, nLp1, strofip(dwSrcIp), wRevPort);
                }

                if(dwDisIp > 0)
                {
                     OspPrintf(TRUE, FALSE, "--> %s: %d", strofip( dwDisIp ), wRevPort);
                }
                OspPrintf(TRUE, FALSE, "--> %s: %d", strofip( dwRevIp ), wRevPort);
		        OspPrintf(TRUE, FALSE, "--> %s: %d", strofip( dwDstIp ), wDstPort);
            }
        }
    }
    if ( wSwitchNum > 0 )
    {
        OspPrintf( TRUE, FALSE, "\n" );
    }
    if ( g_byPageNum > 0 )
    {
        OspPrintf(TRUE, FALSE, "\nPage.%d end, Switch Num: %d, TotalSwitchNum: %d\n", 
                                g_byPageNum, wSwitchNum-(g_byPageNum-1)*MAX_SWITCH_PERPAGE, wSwitchNum);
    }

    if ( !bPrtNextPage )
    {
        OspPrintf(TRUE,FALSE,"\n---------------------------switch info end---------------------------\n");
        g_byPageNum = 0;
    }

    return;
}

/*=============================================================================
    函 数 名： ProcNetStat
    功    能： 网络流量统计定时器
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： void 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2006/12/30  4.0			许世林                  创建
=============================================================================*/
void CMpInst::ProcNetStat()
{
    s64 lnRecv = ::dsGetRecvBytesCount();
    s64 lnSend = ::dsGetSendBytesCount();

    g_cNetTrfcStat.AppendLatestStat( lnRecv, lnSend );

    if ( g_bPrintMpNetStat )
        g_cNetTrfcStat.DumpLatestStat();    
    
    NotifyMcuFluxStatus();

    SetTimer( EV_MP_NETSTAT, NETFRFCSTAT_TIMESPAN*1000 );
        
    return;
}

/*=============================================================================
    函 数 名： ProcFluxOverRun
    功    能： 网络转发能力超限监测定时器
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： void 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2007/01/12  4.0			周广程                  创建
=============================================================================*/
void CMpInst::ProcFluxOverRun()
{
	CServMsg cServMsg;
    cServMsg.SetSrcDriId( g_cMpApp.m_byMpId );

	if ( g_cNetTrfcStat.IsFluxOverrun( NETFRFCSTAT_TIMESPAN ) )
	{
		if( g_cMpApp.m_bIsRestrictFlux )
		{
			MpLog( LOG_ERR, "[ProcFluxOverRun] Mp is Overrun! Notify MCU!\n");
            NotifyMcuFluxStatus();
		}
	}
	SetTimer( EV_MP_FLUXOVERRUN, NETFRFCSTAT_TIMESPAN*1000*12);
	return;
}

/*=============================================================================
    函 数 名： RestrictFluxOfMp
    功    能： 限制MP转发流量处理
    算法实现： 
    全局变量： 
    参    数：CMessage* const pcMsg 
    返 回 值：void 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2007/03/21  4.0			张宝卿                创建
=============================================================================*/
void CMpInst::RestrictFluxOfMp(CMessage* const pcMsg)
{
    CServMsg cRevMsg(pcMsg->content, pcMsg->length);
    
    // 限制MP转发流量处理, zgc 07/01/11
    if( g_cMpApp.m_bIsRestrictFlux &&  g_cNetTrfcStat.IsNetTrfcOverrun() )
    {
        MpLog( LOG_WARN, "[RestrictFluxOfMp]Error: MP is running over... \n");
        SendMsg2Mcu( pcMsg->event + 2, cRevMsg );
        
        //过滤超限通知，5秒内只允许发送一个超限通知, zgc, 07/01/15
        if( g_cMpApp.m_bIsSendOverrunNtf )
        {
            g_cMpApp.m_bIsSendOverrunNtf = FALSE;
            SetTimer( EV_MP_SENDFLUXOVERRUNNOTIFY, NETFRFCSTAT_TIMESPAN * 1000 );
            MpLog( LOG_WARN, "[RestrictFluxOfMp]Mp is Overrun!\n");
            SetTimer( EV_MP_FLUXOVERRUN, 10 );
        }
        
        NotifyMcuFluxStatus();
    }

    return;
}

/*=============================================================================
    函 数 名： NotifyMcuFluxStatus
    功    能： 当前的码流实际负载情况通知MCU
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： void 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2007/02/11  4.0			张宝卿                 创建
=============================================================================*/
void CMpInst::NotifyMcuFluxStatus()
{
    s32 nSend = 0;
    s32 nRecv = 0;
    g_cNetTrfcStat.GetAvgNetBurden( NETFRFCSTAT_TIMESPAN, nSend, nRecv);
    
	/*lint -save -e702*/
    u16 wBandUsed = (u16)((nSend + nRecv) >> 10);     // Mbps
	/*lint -restore*/
    wBandUsed = htons(wBandUsed);
    CServMsg cServMsg;
    cServMsg.SetEventId(MP_MCU_FLUXSTATUS_NOTIFY);
    cServMsg.SetSrcDriId(g_cMpApp.m_byMpId);
    cServMsg.SetMsgBody((u8*)&wBandUsed, sizeof(u16));
    SendMsg2Mcu( cServMsg.GetEventId(), cServMsg );

    return;
}

/*=============================================================================
    函 数 名： ProcSendFluxOverrunNotify
    功    能： 能力超限通知过滤定时器
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： void 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2007/01/12  4.0			周广程                  创建
=============================================================================*/
void CMpInst::ProcSendFluxOverrunNotify(CMessage* const pcMsg)
{
	if (NULL == pcMsg)
	{
		MpLog(LOG_ERR, "[ProcSendFluxOverrunNotify] pcMsg is null!\n");
	}
	g_cMpApp.m_bIsSendOverrunNtf = TRUE;
}

/*=============================================================================
  函 数 名： SendMsgToMcu
  功    能： 发消息给Mcu
  算法实现： 
  全局变量： 
  参    数： u16  wEvent
             u8 * const pbyMsg
             u16  wLen
  返 回 值： void 
=============================================================================*/
void CMpInst::SendMsgToMcu(u16 wEvent, u8 * const pbyMsg, u16 wLen)
{
    if(OspIsValidTcpNode(g_cMpApp.m_dwMcuNode) || g_cMpApp.m_bEmbedA)
    {
        post(g_cMpApp.m_dwMcuIId, wEvent, pbyMsg, wLen, g_cMpApp.m_dwMcuNode);
    }

    if(OspIsValidTcpNode(g_cMpApp.m_dwMcuNodeB)  || g_cMpApp.m_bEmbedB)
    {
        post(g_cMpApp.m_dwMcuIIdB, wEvent, pbyMsg, wLen, g_cMpApp.m_dwMcuNodeB);
    }

    return;
}

/*=============================================================================
  函 数 名： SendMsg2Mcu
  功    能： 发消息给Mcu
  算法实现： 
  全局变量： 
  参    数： u16      wEvent
             CServMsg &cServMsg
  返 回 值： void 
=============================================================================*/
void CMpInst::SendMsg2Mcu( u16 wEvent, CServMsg &cServMsg )
{
    cServMsg.SetSrcDriId(g_cMpApp.m_byMpId);
    SendMsgToMcu(wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
}


/*=============================================================================
  函 数 名： SendSelfMutePack
  功    能： 给自己发哑音包
  算法实现： 
  全局变量： 
  参    数： u32 dwNeedDstIp：要发往的目的ip
			 u16 wNeedDstPort：要发往的目的port
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2013/05/22  4.7			周翼亮                  创建
=============================================================================*/
void CMpInst::SendSelfMutePack(u32 dwNeedDstIp,u16 wNeedDstPort)
{
	u16 wRet = 0;
	TNetSndParam tSndParam;
	memset(&tSndParam,0,sizeof(tSndParam));
	tSndParam.m_byNum = 1;
	//send mute pack to need send dstip and port
	for (u16 wLoop = 0 ; wLoop < g_cMpApp.m_wNeedSendMutePackNum ; wLoop++ )
	{
		if (g_cMpApp.m_ptSendMutePackTable == NULL)
		{
			return;
		}
		if ( g_cMpApp.m_ptSendMutePackTable[wLoop].IsNull())
		{
			continue;
		}

		//发哑音包
		u8 byConfIdx = g_cMpApp.m_ptSendMutePackTable[wLoop].GetConfIdx() ;
		u32 dwDstIp = g_cMpApp.m_ptSendMutePackTable[wLoop].GetDstIp();
		u16 wDstPort = g_cMpApp.m_ptSendMutePackTable[wLoop].GetDstPort();

		if ( dwDstIp != dwNeedDstIp ||  wNeedDstPort != wDstPort)
		{
			continue;
		}
		
		u8 byMediaType = g_cMpApp.m_ptSendMutePackTable[wLoop].GetPayloadType();
		for ( u8 byMutePackIdx = 0; byMutePackIdx < g_cMpApp.GetMutePackNum(); byMutePackIdx ++)
		{
			u8 byAudioTrackNum = g_cMpApp.m_ptSendMutePackTable[wLoop].GetAudioTrackNum();
			MpLog(LOG_INFO,"[SendSelfMutePack]Send mute pack to confidx:%d, Ip:0x%x,port:%d,payloadType:%d\n",
				byConfIdx,dwDstIp,wDstPort,byMediaType	);
			if (NULL == m_ppSnd[byMediaType])
			{
				MpLog(LOG_ERR,"[SendSelfMutePack]pSnd[] is Null!\n");
				continue;
			}
			u32 dwSSrc = 0;
			u32 dwTimeStamp = 0;
			u16 wSeqNum = 0;
			u8 byConfNoInMp = byConfIdx - 1;
			u16 wChanNo = g_cMpApp.GetChanNoByDst(byConfNoInMp , dwDstIp, wDstPort, 0);
			if (wChanNo != MAX_SWITCH_CHANNEL)//找到
			{
				if ( g_cMpApp.m_pptFilterParam[byConfNoInMp][wChanNo].dwModifySSRC == 0 )
				{
					u16 wTempSeqNum     = g_cMpApp.m_pptFilterData[byConfNoInMp][wChanNo].wSeqNum;
					u32 dwTempTimeStamp = g_cMpApp.m_pptFilterData[byConfNoInMp][wChanNo].dwTimeStamp;
					u32 dwTempSSRC      = g_cMpApp.m_pptFilterData[byConfNoInMp][wChanNo].dwSSRC;
					
					g_cMpApp.m_pptFilterParam[byConfNoInMp][wChanNo].wModifySeqNum     = wTempSeqNum;
					g_cMpApp.m_pptFilterParam[byConfNoInMp][wChanNo].dwModifyTimeStamp = dwTempTimeStamp;
					g_cMpApp.m_pptFilterParam[byConfNoInMp][wChanNo].dwModifySSRC      = dwTempSSRC;
				}
				else
				{
					g_cMpApp.m_pptFilterParam[byConfNoInMp][wChanNo].wModifySeqNum     +=  1;
					
					if( 0 != g_cMpApp.m_pptFilterParam[byConfNoInMp][wChanNo].dwTimeStampNum )
					{
						g_cMpApp.m_pptFilterParam[byConfNoInMp][wChanNo].dwModifyTimeStamp += g_cMpApp.m_pptFilterParam[byConfNoInMp][wChanNo].dwTimeStampNum;
					}
					else
					{
						g_cMpApp.m_pptFilterParam[byConfNoInMp][wChanNo].dwModifyTimeStamp += DEFAULT_TIMESTAMP_INTERVAL;
					}
				}
				//取归一重整保存的ssrc
				dwSSrc = g_cMpApp.m_pptFilterParam[byConfNoInMp][wChanNo].dwModifySSRC;
				wSeqNum = g_cMpApp.m_pptFilterParam[byConfNoInMp][wChanNo].wModifySeqNum;
				dwTimeStamp = g_cMpApp.m_pptFilterParam[byConfNoInMp][wChanNo].dwModifyTimeStamp;
				
				
				MpLog(LOG_INFO,"[SendSelfMutePack]Ip:0x%x,port:%d,payloadType:%d dwTimeStampNum.%d\n",
					dwDstIp,wDstPort,byMediaType,g_cMpApp.m_pptFilterParam[byConfNoInMp][wChanNo].dwTimeStampNum );
				
			}	
			
			tSndParam.m_tLocalNet.m_wRTPPort = 9998;
			tSndParam.m_tLocalNet.m_wRTCPPort = 9999;
			tSndParam.m_tRemoteNet[0].m_dwRTPAddr = htonl(dwDstIp);
			tSndParam.m_tRemoteNet[0].m_wRTPPort = wDstPort;
			tSndParam.m_tRemoteNet[0].m_dwRTCPAddr = htonl(dwDstIp);
			tSndParam.m_tRemoteNet[0].m_wRTCPPort  = wDstPort +1;
			
			wRet = m_ppSnd[byMediaType]->SetNetSndParam(tSndParam);
			if (wRet != 0)
			{
				MpLog(LOG_ERR,"[SendSelfMutePack]mediatype:%d,SetNetSndparam error!,wRet:%d\n",byMediaType,wRet);
				continue;
			}
			
			TRtpPack tPack;
			//tPack.m_byMark = 1;
			tPack.m_byExtence = 0;
			tPack.m_byPayload = byMediaType;
			tPack.m_dwSSRC = dwSSrc;			
			tPack.m_dwTimeStamp = dwTimeStamp;
			
			
			tPack.m_wSequence = wSeqNum;
			
			
			u32 dwFrameLen = 0;
			//u8 achFrameData[MAX_AUDIO_FRAME_SIZE] = {0};
			memset(m_achBuf,0,sizeof(m_achBuf));
			if ( !GetMuteFrameData(byMediaType,dwFrameLen,m_achBuf,byAudioTrackNum) )
			{
				MpLog(LOG_ERR,"[SendSelfMutePack]mediatype:%d,GetMuteFrameData error!\n",byMediaType);
				continue;
			}
			tPack.m_nRealSize = dwFrameLen;
			tPack.m_pRealData = m_achBuf;
			MpLog(LOG_INFO,"[SendSelfMutePack]ssrc:0x%x,timestamp:%d,seq:%d\n",dwSSrc,dwTimeStamp,wSeqNum);
			wRet = m_ppSnd[byMediaType]->Send(&tPack);
			
			if (wRet != 0)
			{
				MpLog(LOG_ERR,"[SendSelfMutePack]mediatype:%d,send error!,wRet:%d\n",byMediaType,wRet);
			}

			OspDelay(g_cMpApp.GetMutePackInterval());
			MpLog( LOG_INFO, "[ProcRemoveSwitchReq] OspDelay %d.ms, the %d th pack sent success!\n",g_cMpApp.GetMutePackInterval(),byMutePackIdx+1);
		}
		
		if(m_ppSnd[byMediaType] != NULL)
		{
			wRet = m_ppSnd[byMediaType]->RemoveNetSndLocalParam();
			if (wRet != 0)
			{
				MpLog(LOG_ERR,"[SendSelfMutePack]RemoveNetSndLocalParam error!,wRet:%d\n",wRet);
			}
		}
	}
}


//END OF FILE
