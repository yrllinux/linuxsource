/*****************************************************************************
   模块名      : mpulib
   文件名      : mpubas.cpp
   相关文件    : 
   文件实现功能: mpulib dvmp态分支实现
   作者        : 周文
   版本        : V4.5  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期       版本        修改人      修改内容
   2009/3/14    4.6         张宝卿      逻辑修正，注释及打印整理
******************************************************************************/
#include "mpuinst.h"
//#include "mpustruct.h"
#include "evmpu.h"
#include "mcuver.h"
#include "evmcueqp.h"
#include "boardagent.h"

// #ifdef _LINUX_
// #include "boardwrapper.h"
// #endif

BOOL32 g_bPauseSS = FALSE;

/*====================================================================
	函数  : Clear
	功能  : 消息分发
	输入  : CMessage
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	03/12/4		v1.0		zhangsh			create
====================================================================*/
void CMpuSVmpInst::Clear()
{
    u8 byLoop = 0;
    for ( ; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++ )
    {
        m_pcMediaRcv[byLoop] = NULL;
    }
    byLoop = 0;
    for (; byLoop < MAXNUM_MPUSVMP_CHANNEL; byLoop++)
    {
        m_pcMediaSnd[byLoop] = NULL;
    }
    byLoop = 0;
    for ( ; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++ )
    {
        m_bAddVmpChannel[byLoop] = 0;
    }

    m_dwMcuNode        = 0;       
    m_dwMcuNodeB       = 0;     
    m_dwMcuIId         = 0;        
    m_dwMcuIIdB        = 0;       
    m_byRegAckNum      = 0;     
	m_dwMpcSSrc        = 0;      
    m_dwMcuRcvIp       = 0;      
    m_wMcuRcvStartPort = 0;
    m_byChnConfType    = 0;  
    m_byHardStyle      = 0;   
	m_wMTUSize         = 0;	
    memset(m_byIframeReqCount,0,sizeof(m_byIframeReqCount));
	memset( m_adwLastFUPTick, 0, sizeof(m_adwLastFUPTick));

	memset ( m_bUpdateMediaEncrpyt, 0 , sizeof(m_bUpdateMediaEncrpyt));
       
}

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
	03/12/4		v1.0		zhangsh			create
====================================================================*/
void CMpuSVmpInst::InstanceEntry( CMessage *const pMsg )
{
    if ( NULL == pMsg )
    {
        return;
    }

    if ( EV_VMP_NEEDIFRAME_TIMER != pMsg->event )
    {
        mpulog( MPU_INFO, "Message %u(%s) received in InstanceEntry\n", pMsg->event, ::OspEventDesc(pMsg->event) );
    }
    switch ( pMsg->event )
    {
    //--------内部消息------------
    // 断链处理
    case OSP_DISCONNECT:
        Disconnect( pMsg );
        break;
	// 连接
    case EV_VMP_CONNECT_TIMER:
        ProcConnectTimeOut( TRUE );
        break;

    case EV_VMP_CONNECT_TIMERB:
        ProcConnectTimeOut( FALSE );
        break;
	// 注册
    case EV_VMP_REGISTER_TIMER:
        ProcRegisterTimeOut( TRUE );
        break;

    case EV_VMP_REGISTER_TIMERB:
        ProcRegisterTimeOut( FALSE );
        break;
	
	
    //--------外部消息------------
    // 启动初始化
    case EV_VMP_INIT:
        Init( pMsg );
        break;

     // 显示所有的状态和统计信息
     case EV_VMP_DISPLAYALL:
        StatusShow();
        break;

 	case EV_VMP_SHOWPARAM:
 		ParamShow();
		break;

	case EV_VMP_SETFRAMERATE_CMD:
		ProcSetFrameRateCmd(pMsg);
		break;

    // 注册应答消息
    case MCU_VMP_REGISTER_ACK:
        MsgRegAckProc( pMsg );
        break;

    // 注册拒绝消息
    case MCU_VMP_REGISTER_NACK:
        MsgRegNackProc( pMsg );
        break;

	case MCU_EQP_MODSENDADDR_CMD:
		MsgModSendAddrProc( pMsg );
		break;

    // 开始复合
    case MCU_VMP_STARTVIDMIX_REQ:
        MsgStartVidMixProc( pMsg );
        break;

    // 停止复合
    case MCU_VMP_STOPVIDMIX_REQ:
        MsgStopVidMixProc( pMsg );
        break;

    // 参数改变
    case MCU_VMP_CHANGEVIDMIXPARAM_CMD:
        MsgChangeVidMixParamProc( pMsg );
        break;

    // 设置通道码率
    case MCU_VMP_SETCHANNELBITRATE_REQ:
        MsgSetBitRate( pMsg );
        break;

    // 设置Qos值
//     case MCU_EQP_SETQOS_CMD:
//        ProcSetQosInfo(pMsg);
//        break;

    // 更改编码参数
    case MCU_VMP_UPDATAVMPENCRYPTPARAM_REQ:
        MsgUpdataVmpEncryptParamProc( pMsg );
        break;

    // 强制关键帧	//FIXME: 优化成带参数(对应4出)
    case MCU_VMP_FASTUPDATEPIC_CMD:
        MsgFastUpdatePicProc( pMsg );
        break;
		
		//调整合成成员别名
	case MCU_VMP_CHANGEMEMALIAS_CMD:
		ProcChangeMemAliasCmd(pMsg);
		break;

	// 检测是否需要关键帧
    case EV_VMP_NEEDIFRAME_TIMER:
        MsgTimerNeedIFrameProc();
        break;

	// 取主备倒换状态
	case TIME_GET_MSSTATUS:
	case MCU_EQP_GETMSSTATUS_ACK:
		ProcGetMsStatusRsp( pMsg );
		break;

	case MCU_VMP_ADDREMOVECHNNL_CMD:
		MsgAddRemoveRcvChnnl( pMsg );
		break;
	case EV_VMP_SHOWDEBUG:
		ShowVMPDebugInfo();
		break;
    // 未定义的消息
    default:
        log( LOGLVL_EXCEPTION, "[Error]Unexcpet Message %u(%s).\n", pMsg->event, ::OspEventDesc(pMsg->event) );
        break;
    }
}

/*====================================================================
	函数  : StatusShow
	功能  : 显示当前画面复合器状态信息
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	03/12/4		v1.0		zhangsh			create
====================================================================*/
void CMpuSVmpInst::StatusShow()
{
    TMulPicStatus tMulPicStatus;
//    TMulPicStatis tMulPicStatis;
    memset(&tMulPicStatus, 0, sizeof(tMulPicStatus));
//    memset(&tMulPicStatis, 0, sizeof(tMulPicStatis));

    m_cHardMulPic.GetStatus(0, tMulPicStatus);
//    m_cHardMulPic.GetStatis(tMulPicStatis);

    ::OspPrintf(TRUE, FALSE, "\t===========The state of VMP is below===============\n");
	
	::OspPrintf(TRUE, FALSE, "The MTU Size is : %d\n", m_wMTUSize );

    ::OspPrintf(TRUE, FALSE, "  IDLE =%d, NORMAL=%d, RUNNING=%d, VMP Current State =%d\n",
	                          IDLE, NORMAL, RUNNING, CurState());

    ::OspPrintf(TRUE, FALSE, "\t===========The status of VMP is below===============\n");
    ::OspPrintf(TRUE, FALSE, "Merge=%d\t Style=%d\t CurChannel=%d\n",
                              tMulPicStatus.bMergeStart,
                              tMulPicStatus.byType,
                              tMulPicStatus.byCurChnNum);

    ::OspPrintf(TRUE, FALSE, "\t-------------The Video Encoding Params--------------\n");
    ::OspPrintf(TRUE, FALSE, "\t EncType=%d\t ComMode=%d\t KeyFrameInter=%d\t MaxQuant=%d\t MinQuant=%d\t\n",
                              tMulPicStatus.tVideoEncParam.m_byEncType,
                              tMulPicStatus.tVideoEncParam.m_byRcMode,
                              tMulPicStatus.tVideoEncParam.m_byMaxKeyFrameInterval,
                              tMulPicStatus.tVideoEncParam.m_byMaxQuant,
                              tMulPicStatus.tVideoEncParam.m_byMinQuant);

    ::OspPrintf(TRUE, FALSE, "BitRate=%d\t FrameRate=%d\t ImageQuality=%d\t VideoWidth=%d\t VideoHeight=%d\t\n",
                              tMulPicStatus.tVideoEncParam.m_wBitRate,
                              tMulPicStatus.tVideoEncParam.m_byFrameRate,
                              tMulPicStatus.tVideoEncParam.m_byImageQulity,
                              tMulPicStatus.tVideoEncParam.m_wVideoWidth,
                              tMulPicStatus.tVideoEncParam.m_wVideoHeight);

//     ::OspPrintf(TRUE, FALSE, "\t-------------Network Statics --------------\n");
//     ::OspPrintf(TRUE, FALSE, "SendBitRate=%d\t SendPackNum=%d\t LostPackNum=%d\t\n",
//                               tMulPicStatis.m_dwSendBitRate,
//                               tMulPicStatis.m_dwSendPackNum,
//                               tMulPicStatis.m_dwSendDiscardPackNum);
// 
//     s32 nLoop; 
//     for(nLoop = 0; nLoop < tMulPicStatus.byCurChnNum; nLoop++)
//     {
//         ::OspPrintf(TRUE, FALSE, "Receive Channel(%d) BitRate:%d\t RecvPackNum:%d\t LostPackNum:%d\n",nLoop,
//                                   tMulPicStatis.atMulPicChnStatis[nLoop].m_dwRecvBitRate,
//                                   tMulPicStatis.atMulPicChnStatis[nLoop].m_dwRecvPackNum,
//                                   tMulPicStatis.atMulPicChnStatis[nLoop].m_dwRecvLosePackNum);
//     }
    return;
}

/*=============================================================================
函 数 名： ParamShow
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/3/19   4.0		周广程                  创建
=============================================================================*/
void CMpuSVmpInst::ParamShow(void)
{
	::OspPrintf(TRUE, FALSE, "\t=============The Param of VMP is below=============\t\n");
	for (u8 byLoop = 0; byLoop < MAXNUM_MPUSVMP_CHANNEL;byLoop++)
	{
		::OspPrintf(TRUE, FALSE, "\t------------ The %d video chnnl ----------------\t\n",byLoop);
		::OspPrintf(TRUE, FALSE, "\tVmpStyle:\t%d\n\tEncType:\t%d\n\tBitrate:\t%d\n\tVidWidth:\t%d\n\tVidHeight:\t%d\n\tMemNum:\t\t%d\nFrameRate:\t%d\n",
			m_tParam[byLoop].m_byVMPStyle, m_tParam[byLoop].m_byEncType, m_tParam[byLoop].GetBitRate(), m_tParam[byLoop].GetVideoWidth(), m_tParam[byLoop].GetVideoHeight(), m_tParam[byLoop].m_byMemberNum, m_tParam[byLoop].GetFrameRate());
		
		::OspPrintf(TRUE, FALSE, "\nMem as follows:\n");
		
		for(u8 byIndex = 0; byIndex < MAXNUM_MPUSVMP_MEMBER; byIndex++)
		{
			if( !m_tParam[byLoop].m_atMtMember[byIndex].IsNull() )
			{
				::OspPrintf(TRUE, FALSE, "\tMemIdx[%d]:\n", byIndex);
				::OspPrintf(TRUE, FALSE, "\t\tMcuId:\t\t%d\n\t\tMtId:\t\t%d\n\t\tMemType:\t%d\n\t\tMemStatus:\t%d\n\t\tRealPayLoad:\t%d\n\t\tActPayLoad:\t%d\n\t\tEncrptMode:\t%d\n", 
					m_tParam[byLoop].m_atMtMember[byIndex].GetMcuId(), m_tParam[byLoop].m_atMtMember[byIndex].GetMtId(), 
					m_tParam[byLoop].m_atMtMember[byIndex].GetMemberType(), m_tParam[byLoop].m_atMtMember[byIndex].GetMemStatus(),
					m_tParam[byLoop].m_tDoublePayload[byIndex].GetRealPayLoad(), m_tParam[byLoop].m_tDoublePayload[byIndex].GetActivePayload(),
					m_tParam[byLoop].m_tVideoEncrypt[byIndex].GetEncryptMode());
			}            
		}
        ::OspPrintf(TRUE, FALSE, "\n");
	}
	

// 	m_tParam[0].Print();
// 	::OspPrintf(TRUE, FALSE, "\n");
// 	::OspPrintf(TRUE, FALSE, "\t------------ The second video chnnl ----------------\t\n");
// 	m_tParam[1].Print();
// 	::OspPrintf(TRUE, FALSE, "\t----------------------------------------------------\t\n");
//     ::OspPrintf(TRUE, FALSE, "\n");
// 	::OspPrintf(TRUE, FALSE, "\t------------ The third video chnnl -----------------\t\n");
// 	m_tParam[2].Print();
// 	::OspPrintf(TRUE, FALSE, "\t----------------------------------------------------\t\n\n");
// 	::OspPrintf(TRUE, FALSE, "\t------------ The fourth video chnnl -----------------\t\n");
// 	m_tParam[3].Print();
// 	::OspPrintf(TRUE, FALSE, "\t----------------------------------------------------\t\n");
}

/*====================================================================
	函数  : Init
	功能  : 向MCU注册
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	03/12/4		v1.0		zhangsh			create
====================================================================*/
void CMpuSVmpInst::Init( CMessage *const pMsg )
{
    // 消息体TEqpCfg
    if ( NULL == pMsg->content )
    {
        mpulog( MPU_CRIT, "[Init] Init params cannot be NULL!\n" );
        return;
    }

    // TEqpCfg中有mcuip，port，alias，localip，eqpid，eqptype，localrcvstartport
    // 直接保存到m_tCfg
    memcpy( &m_tCfg, pMsg->content, sizeof(TEqpCfg) );

    s32 nRet = HardMPUInit( (u32)INITMODE_All_VMP );
    if ( HARD_MULPIC_OK == nRet )
    {
        mpulog( MPU_INFO, "[Init] Init %s Success!\n", m_tCfg.achAlias );
    }
    else
    {
        mpulog( MPU_CRIT, "[Init] Fail to inital the Hard Multi-picture.(Init). errcode.%d\n", nRet );
        printf("[Init] Fail to inital the Hard Multi-picture.(Init). errcode.%d\n", nRet );
		return;			
    }

    TMulCreate tMulCreate;
    tMulCreate.m_dwDSPNUM = 4;
    tMulCreate.m_dwStartDspID = 0;
    nRet = m_cHardMulPic.Create( tMulCreate );
    if ( HARD_MULPIC_OK == nRet )
    {
        mpulog( MPU_INFO, "[Init] Create %s Success!\n", m_tCfg.achAlias );
		printf("[Init] Create %s Success!\n", m_tCfg.achAlias);
    }
    else
    {
        mpulog( MPU_CRIT, "[Init] Fail to Create. errcode.%d\n", nRet );
		printf("[Init] Fail to Create. errcode.%d\n", nRet );
        return;
    }

    u16 wRet = KdvSocketStartup();
    
    if ( MEDIANET_NO_ERROR != wRet )
    {
        mpulog( MPU_CRIT, "[Init] KdvSocketStartup failed, error: %d\n", wRet );
		printf("[Init] KdvSocketStartup failed, error: %d\n", wRet);
        return ;
    }
	
	printf("KdvSocketStartup ok!!!\n");

    u8 byLoop = 0;
    for ( ; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++  )
    {
        m_pcMediaRcv[byLoop] = new(CKdvMediaRcv);
        if ( NULL == m_pcMediaRcv[byLoop] )
        {
            mpulog( MPU_CRIT, "new(CKdvMediaRcv) fail!\n" );
            return;
        }
		if (!InitMediaRcv(byLoop))
		{
            mpulog( MPU_CRIT, "InitMediaRcv %d fail!\n", byLoop );			
			return;
		}
		
    }

#ifdef _LINUX_
    if ( m_tCfg.dwLocalIP == m_tCfg.dwConnectIP )
    {
        mpulog( MPU_CRIT, "[Init] m_tCfg.dwLocalIP == m_tCfg.dwConnectIP\n" );   
        return;
    }
#endif

	printf("[CMpuSVmpInst::Init] m_tCfg.dwConnectIP is 0x%X\n",m_tCfg.dwConnectIP);
	printf("[CMpuSVmpInst::Init] m_tCfg.dwConnectIPB is 0x%X\n",m_tCfg.dwConnectIpB);

    if( 0 != m_tCfg.dwConnectIP ) 
    {
        SetTimer( EV_VMP_CONNECT_TIMER, MPU_CONNETC_TIMEOUT );
    }

	if( 0 != m_tCfg.dwConnectIpB )
	{
		SetTimer( EV_VMP_CONNECT_TIMERB, MPU_CONNETC_TIMEOUT);
	}
    sprintf( MPULIB_CFG_FILE, "%s/mcueqp.ini", DIR_CONFIG );
    m_tMpuVmpCfg.ReadDebugValues();

	

	m_cHardMulPic.SetEncResizeMode(m_tMpuVmpCfg.GetVidSDMode());
	printf("[Init] SVMP SetEncResizeMode mode is %d\n", m_tMpuVmpCfg.GetVidSDMode());
    return;
}

/*====================================================================
	函数  : DaemonProcOspDisconnect
	功能  : Osp断链消息处理函数
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2008/12/09  4.5         周文          创建
====================================================================*/
void CMpuSVmpInst::Disconnect( CMessage* const pMsg )
{
    if ( NULL == pMsg )  
    {
        mpulog( MPU_CRIT, "message's pointer is Null\n" );
        return;
    }

    u32 dwNode = *(u32*)pMsg->content;

    if ( INVALID_NODE != dwNode )
    {
        OspDisconnectTcpNode( dwNode );
    } 
    if( dwNode == m_dwMcuNode ) // 断链
    {
        mpulog( MPU_INFO, "[Disconnect] McuNode.A disconnect\n" );
        m_dwMcuNode = 0;
        m_dwMcuIId  = 0;
        SetTimer( EV_VMP_CONNECT_TIMER, MPU_CONNETC_TIMEOUT );
    }
    else if ( dwNode == m_dwMcuNodeB )
    {
        mpulog( MPU_INFO, "[Disconnect] McuNode.B disconnect\n" );
        m_dwMcuNodeB = 0;
        m_dwMcuIIdB  = 0;
        SetTimer( EV_VMP_CONNECT_TIMERB, MPU_CONNETC_TIMEOUT );
    }
	// 与其中一个断链后，向Mcu取主备倒换状态，判断是否成功
	if ( INVALID_NODE != m_dwMcuNode || INVALID_NODE != m_dwMcuNodeB )
	{
		if ( OspIsValidTcpNode(m_dwMcuNode) )
		{
			post( m_dwMcuIId, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, m_dwMcuNode );           
			mpulog( MPU_INFO, "[Disconnect] try GetMsStatusReq. from McuNode.A(%d)\n",m_dwMcuNode );
		}
		else if ( OspIsValidTcpNode(m_dwMcuNodeB) )
		{
			post( m_dwMcuIIdB, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, m_dwMcuNodeB );        
			mpulog( MPU_INFO, "[Disconnect] try GetMsStatusReq. from McuNode.B(%d)\n",m_dwMcuNodeB );
		}

		SetTimer( TIME_GET_MSSTATUS, WAITING_MSSTATUS_TIMEOUT);
		return;
	}
	// 两个节点都断，清空状态信息
    if ( INVALID_NODE == m_dwMcuNode && INVALID_NODE == m_dwMcuNodeB )
    {
        ClearCurrentInst();
    }
    return;
}

/*=============================================================================
  函 数 名： ClearCurrentInst
  功    能： 清空当前实例
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CMpuSVmpInst::ClearCurrentInst( void )
{
	switch ( CurState() )//移通道
    {
    case NORMAL:
    case RUNNING:
        {
            s32 nRet = m_cHardMulPic.StopMerge();
            KillTimer( EV_VMP_NEEDIFRAME_TIMER );
            mpulog( MPU_INFO, "[VMP Stop]:Stop work, the return code: %d!\n", nRet );
        }
        break;
	default:
		break;
    }

    NEXTSTATE( IDLE );

    m_byRegAckNum = 0;
	m_dwMpcSSrc = 0;
	
	memset( m_bAddVmpChannel, 0, sizeof(m_bAddVmpChannel) );

	memset( m_bUpdateMediaEncrpyt, 0, sizeof(m_bUpdateMediaEncrpyt));
	memset(m_tParamPrevious,0,sizeof(m_tParamPrevious));
    mpulog(MPU_INFO, "[Vmp] Disconnect with A and B board\n");

	return;
}

/*=============================================================================
  函 数 名： DaemonProcConnectTimeOut
  功    能： 处理注册超时
  算法实现： 
  全局变量： 
  参    数： BOOL32 bIsConnectA
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人      修改内容
  2008/12/04  4.5         周文        创建
=============================================================================*/
void CMpuSVmpInst::ProcConnectTimeOut( BOOL32 bIsConnectA )
{
    BOOL32 bRet = FALSE;
    if( TRUE == bIsConnectA )
    {
        bRet = ConnectMcu( bIsConnectA, m_dwMcuNode );
        if ( TRUE == bRet )
        {  
            SetTimer( EV_VMP_REGISTER_TIMER, MPU_REGISTER_TIMEOUT ); 
        }
        else
        {
            SetTimer( EV_VMP_CONNECT_TIMER, MPU_CONNETC_TIMEOUT );   
        }
    }
    else
    {
        bRet = ConnectMcu( bIsConnectA, m_dwMcuNodeB );
        if ( TRUE == bRet )
        { 
            SetTimer( EV_VMP_REGISTER_TIMERB, MPU_REGISTER_TIMEOUT ); 
        }
        else
        {
            SetTimer( EV_VMP_CONNECT_TIMERB, MPU_CONNETC_TIMEOUT );   
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
  日  期      版本        修改人      修改内容
  2008/08/04  4.5         顾振华         创建
====================================================================*/
BOOL32 CMpuSVmpInst::ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode )
{
    BOOL32 bRet = TRUE;

    // 内嵌时被过滤
    if( !OspIsValidTcpNode(dwMcuNode) ) 
    {
        if( TRUE == bIsConnectA )
        {
            // 貌似去OspConnect，其实没有连是去取。
            dwMcuNode = BrdGetDstMcuNode();  
        }
        else
        {  
            dwMcuNode = BrdGetDstMcuNodeB(); 
        }

	    if ( ::OspIsValidTcpNode(dwMcuNode) )
	    {
		    mpulog( MPU_CRIT, "[ConnectMcu]Connect Mcu %s Success, node: %d!\n", 
                   bIsConnectA ? "A" : "B", dwMcuNode );
		    ::OspNodeDiscCBRegQ( dwMcuNode, GetAppID(), GetInsID() );
	    }
	    else 
	    {
		    mpulog( MPU_CRIT, "[ConnectMcu]Connect to Mcu failed, will retry\n" );
            bRet = FALSE;
	    }
    }
    return bRet;
}

/*=============================================================================
  函 数 名： DaemonProcRegisterTimeOut
  功    能： 处理注册超时
  算法实现： 
  全局变量： 
  参    数： BOOL32 bIsRegiterA
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人      修改内容
  2008/12/04  4.5         周文        创建
=============================================================================*/
void CMpuSVmpInst::ProcRegisterTimeOut( BOOL32 bIsRegiterA )
{
    if( TRUE == bIsRegiterA )
    {
        Register( m_dwMcuNode );
        SetTimer( EV_VMP_REGISTER_TIMER, MPU_REGISTER_TIMEOUT );  
    }
    else
    {
        Register( m_dwMcuNodeB );
		SetTimer( EV_VMP_REGISTER_TIMERB, MPU_REGISTER_TIMEOUT ); 
    }
    return;
}

/*==================================================================
	函数  : Register
	功能  : 向MCU注册
	输入  : 无
	输出  : 无
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人      修改内容
  2008/12/04  4.5         周文        创建
====================================================================*/
void CMpuSVmpInst::Register( /*BOOL32 bIsRegiterA,*/ u32 dwMcuNode )
{
    CServMsg       cSvrMsg;
    TPeriEqpRegReq tReg;

    tReg.SetMcuEqp( (u8)m_tCfg.wMcuId, m_tCfg.byEqpId, m_tCfg.byEqpType );
    tReg.SetPeriEqpIpAddr( htonl(m_tCfg.dwLocalIP) );
	tReg.SetStartPort( m_tCfg.wRcvStartPort );
    tReg.SetVersion( DEVVER_MPU );
    
    cSvrMsg.SetMsgBody( (u8*)&tReg, sizeof(tReg) );
        
    post( MAKEIID(AID_MCU_PERIEQPSSN, m_tCfg.byEqpId),
            VMP_MCU_REGISTER_REQ,
            cSvrMsg.GetServMsg(),
            cSvrMsg.GetServMsgLen(),
            dwMcuNode );

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
   03/12/4	   v1.0	      zhangsh	    create
====================================================================*/
BOOL CMpuSVmpInst::SendMsgToMcu( u16 wEvent, CServMsg& cServMsg )
{
    mpulog( MPU_INFO, "[SendMsgToMcu] Message %u(%s).\n", wEvent, ::OspEventDesc(wEvent) );

    if ( OspIsValidTcpNode(m_dwMcuNode) )
    {
        post( m_dwMcuIId, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), m_dwMcuNode );
    }

    if ( OspIsValidTcpNode(m_dwMcuNodeB) )
    {
        post( m_dwMcuIIdB, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), m_dwMcuNodeB );
    }

    return TRUE;
}

/*====================================================================
	函数  : DaemonProcMcuRegAck 
	功能  : MCU应答注册消息处理函数
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人      修改内容
  2008/12/04  4.5         周文        创建
====================================================================*/
void CMpuSVmpInst::MsgRegAckProc( CMessage* const pMsg )
{
	if ( NULL == pMsg )
    {
        mpulog( MPU_CRIT, "[MsgRegAckProc]Recv Reg Ack Msg, but the msg's pointer is Null\n" );
        return;
    }

	TPeriEqpRegAck* ptRegAck;

	CServMsg cServMsg( pMsg->content, pMsg->length );
	ptRegAck = (TPeriEqpRegAck*)cServMsg.GetMsgBody();
	
    TPrsTimeSpan tPrsTimeSpan = *(TPrsTimeSpan*)(cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck));
	u16 wMTUSize = *(u16*)( cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck) + sizeof(TPrsTimeSpan) );
	wMTUSize = ntohs( wMTUSize );
	
    if( pMsg->srcnode == m_dwMcuNode ) 
    {
	    m_dwMcuIId = pMsg->srcid;
        m_byRegAckNum++;
        KillTimer( EV_VMP_REGISTER_TIMER );

        mpulog( MPU_INFO, "[MsgRegAckProc] Regist success to mcu A\n" );
    }
    else if ( pMsg->srcnode == m_dwMcuNodeB )
    {
        m_dwMcuIIdB = pMsg->srcid;
        m_byRegAckNum++;
        KillTimer( EV_VMP_REGISTER_TIMERB );

        mpulog( MPU_INFO, "[MsgRegAckProc] Regist success to mcu B\n" );       
    }

	// guzh [6/12/2007] 校验会话参数
    if ( m_dwMpcSSrc == 0 )
    {
        m_dwMpcSSrc = ptRegAck->GetMSSsrc();
    }
    else
    {
        // 异常情况，断开两个节点
        if ( m_dwMpcSSrc != ptRegAck->GetMSSsrc() )
        {
            mpulog( MPU_CRIT, "[RegAck] MPC SSRC ERROR(%u<-->%u), Disconnect Both Nodes!\n", 
                       m_dwMpcSSrc, ptRegAck->GetMSSsrc() );
            if ( OspIsValidTcpNode(m_dwMcuNode) )
            {
                OspDisconnectTcpNode(m_dwMcuNode);
            }
            if ( OspIsValidTcpNode(m_dwMcuNodeB) )
            {
                OspDisconnectTcpNode(m_dwMcuNodeB);
            }      
            return;
        }
    }

    // 第一次收到注册成功消息
    if( FIRST_REGACK == m_byRegAckNum ) 
    {
        m_dwMcuRcvIp       = ptRegAck->GetMcuIpAddr();
	    m_wMcuRcvStartPort = ptRegAck->GetMcuStartPort();

        // 由于启动时没有获取，这里获取另外一个MCU，如果获取则去连接
        u32 dwOtherMcuIp = htonl( ptRegAck->GetAnotherMpcIp() );
        if ( m_tCfg.dwConnectIpB == 0 && dwOtherMcuIp != 0 )
        {
            m_tCfg.dwConnectIpB = dwOtherMcuIp;
            SetTimer( EV_VMP_CONNECT_TIMERB, MPU_CONNETC_TIMEOUT );

            mpulog( MPU_CRIT, "[MsgRegAckProc] Found another Mpc IP: %s, try connecting...\n", 
                      ipStr(dwOtherMcuIp) );
        }

        mpulog( MPU_INFO, "[MsgRegAckProc] Local Recv Start Port.%u, Mcu Start Port.%u, Alias.%s\n",
                m_tCfg.wRcvStartPort, m_wMcuRcvStartPort , m_tCfg.achAlias );

        m_tPrsTimeSpan = tPrsTimeSpan;
        mpulog( MPU_INFO, "[MsgRegAckProc] The Vmp Prs span: first: %d, second: %d, three: %d, reject: %d\n", 
                m_tPrsTimeSpan.m_wFirstTimeSpan,
                m_tPrsTimeSpan.m_wSecondTimeSpan,
                m_tPrsTimeSpan.m_wThirdTimeSpan,
                m_tPrsTimeSpan.m_wRejectTimeSpan );

		m_wMTUSize = wMTUSize;
		mpulog( MPU_INFO, "[MsgRegAckProc]The Network MTU is : %d\n", m_wMTUSize );

        // zw 20081208 在第一次注册成功之后，将确认后的m_tCfg中的信息（m_tCfg是在PowerOn时传过来并保存的）转存至m_tEqp。
        m_tEqp.SetMcuEqp( (u8)m_tCfg.wMcuId, m_tCfg.byEqpId, m_tCfg.byEqpType );	
	
        NEXTSTATE( NORMAL ); 
       	mpulog(MPU_INFO, "[MsgRegAckProc]set EV_VMP_NEEDIFRAME_TIMER : %d\n", m_tMpuVmpCfg.GetIframeInterval());
        SetTimer(EV_VMP_NEEDIFRAME_TIMER, m_tMpuVmpCfg.GetIframeInterval());
    }

    TPeriEqpStatus tEqpStatus;
    tEqpStatus.SetMcuEqp( (u8)m_tCfg.wMcuId, m_tCfg.byEqpId, m_tCfg.byEqpType );
    tEqpStatus.SetAlias(m_tCfg.achAlias);
    tEqpStatus.m_byOnline = 1;
    tEqpStatus.m_tStatus.tVmp.m_bySubType = MPU_SVMP;

//songkun,20110628,加入MPU WIN32 B版编译
#ifdef WIN32
    tEqpStatus.m_tStatus.tVmp.m_byBoardVer = MPU_BOARD_B256;
#else
    #ifndef _MPUB_256_ 
        tEqpStatus.m_tStatus.tVmp.m_byBoardVer = MPU_BOARD_A128;
    #else
        tEqpStatus.m_tStatus.tVmp.m_byBoardVer = MPU_BOARD_B256;
    #endif
#endif
	
	mpulog(MPU_INFO, "[MsgRegAckProc] mpu boardVer is %u\n", tEqpStatus.m_tStatus.tVmp.m_byBoardVer);
    tEqpStatus.m_tStatus.tVmp.m_byChlNum = 20/*MAXNUM_MPUSVMP_CHN*/;
    cServMsg.SetMsgBody( (u8*)&tEqpStatus, sizeof(tEqpStatus) );
	SendMsgToMcu( VMP_MCU_VMPSTATUS_NOTIF, cServMsg );

    return;
}

/*====================================================================
	函数  : DaemonProcMcuRegNack
	功能  : MCU 拒绝注册消息处理函数(目前的策略不是被拒后继续注册)
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人      修改内容
  2008/12/09  4.5         周文        创建
====================================================================*/
void CMpuSVmpInst::MsgRegNackProc( CMessage* const pMsg )
{
    if ( pMsg->srcnode == m_dwMcuNode )    
    {
        mpulog( MPU_INFO, "Bas registe be refused by A.\n" );
    }
    if ( pMsg->srcnode == m_dwMcuNodeB )    
    {
        mpulog( MPU_INFO, "Bas registe be refused by B.\n" );
    }
    return;
}
/*====================================================================
	函数  : MsgModSendAddrProc
	功能  : MCU强制要求修改发送地址
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人      修改内容
  2008/12/09  4.5         付秀华        创建
====================================================================*/
void CMpuSVmpInst::MsgModSendAddrProc( CMessage* const pMsg )
{
    if ( NULL == pMsg->content )
    {
        mpulog( MPU_CRIT, " The pointer cannot be Null (MsgStartVidMixProc)\n" );
        return;
    }    

	u32 dwSendIP = *(u32*)pMsg->content;
	m_dwMcuRcvIp = ntohl(dwSendIP);
    if (CurState() == RUNNING)
    {
		for ( u8 byChnNo = 0; byChnNo < MAXNUM_MPUSVMP_CHANNEL; byChnNo++ )
		{
			//20110322_tzy Bug00049975 PDS-VCS:组呼会议，外设绑定的转发板备份功能有异常
			InitMediaSnd( byChnNo );
//[2010/12/31 zhushz]平滑发送接口封装
//			if (!g_bPauseSS)
//			{
//				SetSmoothSendRule( m_dwMcuRcvIp,  m_wMcuRcvStartPort + byChnNo * PORTSPAN,
//								   m_tParam[byChnNo].GetBitRate() );
//			}
		}

		//[2011/08/26/zhangli]更新接收信息
		for (u8 byChn = 0; byChn < MAXNUM_MPUSVMP_MEMBER; byChn++)
		{
			SetMediaRcvNetParam( byChn);
		}
		BOOL32 bSetRule = !g_bPauseSS;
		SetSmoothSendRule( bSetRule );
    }	
}
/*====================================================================
	函数名  :MsgStartVidMixProc
	功能    :开始画面复合消息处理函数
	输入    :消息
	输出    :无
	返回值  :无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	03/12/4		v1.0		zhangsh			create
====================================================================*/
void CMpuSVmpInst::MsgStartVidMixProc( CMessage * const pMsg )
{
    if ( NULL == pMsg->content )
    {
        mpulog( MPU_CRIT, " The pointer cannot be Null (MsgStartVidMixProc)\n" );
        return;
    }    

    switch ( CurState() )
    {
    case NORMAL:
		{
//     #ifndef WIN32
// 			BrdLedStatusSet( LED_SYS_LINK, BRD_LED_ON );// 将MLINK灯点亮
// 	#endif	
			CServMsg cServMsg( pMsg->content, pMsg->length );

			CKDVNewVMPParam tParam[MAXNUM_MPUSVMP_CHANNEL];    // 四路视频输出通道
			memset( tParam, 0, MAXNUM_MPUSVMP_CHANNEL * sizeof(CKDVNewVMPParam) );
			memcpy( (u8*)tParam, cServMsg.GetMsgBody(), MAXNUM_MPUSVMP_CHANNEL * sizeof(CKDVNewVMPParam) );
			
            //保存参数
			memcpy( m_tParam, tParam, MAXNUM_MPUSVMP_CHANNEL * sizeof(CKDVNewVMPParam) );

            #ifdef WIN32//songkun,20110628,加入MPU WIN32 B版编译
                ;
            #else
                #ifndef _MPUB_256_	//A板
                    memset(&m_tParam[1], 0, sizeof(CKDVNewVMPParam) );
                    //memset(&m_tParam[3], 0, sizeof(CKDVNewVMPParam) );
		        #endif
		#endif
			
            mpulog( MPU_INFO, "/*---------------------------------------------------------*/\n\n" );
			mpulog( MPU_INFO, "Follows are the params that mcu send to the svmp:\n\n" );
			s32 nIdx = 0;
			for(nIdx = 0; nIdx < MAXNUM_MPUSVMP_CHANNEL; nIdx++)
			{
				mpulog( MPU_INFO, "CHNNL.%d: Type=%d BitRate=%d VideoHeight:%d VideoWidth:%d \n",
					               nIdx,
					               tParam[nIdx].m_byEncType, tParam[nIdx].GetBitRate(), 
					               tParam[nIdx].GetVideoHeight(), tParam[nIdx].GetVideoWidth() );
			}
			mpulog( MPU_INFO, "/*---------------------------------------------------------*/\n\n" );


			u8 byNeedPrs = *(cServMsg.GetMsgBody() + MAXNUM_MPUSVMP_CHANNEL * sizeof(CKDVNewVMPParam));
			
            m_tStyleInfo = *(TVmpStyleCfgInfo*)( cServMsg.GetMsgBody() + 
                                                 MAXNUM_MPUSVMP_CHANNEL * sizeof(CKDVNewVMPParam) +
                                                 sizeof(u8) );
            
		
			// End Modify

            //2.部分参数调整
			for(nIdx = 0; nIdx < MAXNUM_MPUSVMP_CHANNEL; nIdx++)
			{
			
				if ( m_tMpuVmpCfg.IsEnableBitrateCheat() )
				{
					//原本码率是0的维持码率为0
					if(0 != m_tParam[nIdx].GetBitRate())
					{
						m_tParam[nIdx].SetBitRate(m_tMpuVmpCfg.GetDecRateDebug(m_tParam[nIdx].GetBitRate()));
					}
					
					mpulog( MPU_INFO, "New Enc.%d Bitrate: %d\n", nIdx, m_tParam[nIdx].GetBitRate() );
				}
				// 设置视频编码参数(出来的格式)
				// 完成对m_tMpuVmpCfg.m_tVideoEncParam[]的基本参数赋值
				m_tMpuVmpCfg.GetDefaultParam( m_tParam[nIdx].m_byEncType,
                                              m_tMpuVmpCfg.m_tVideoEncParam[nIdx],
                                              m_tMpuVmpCfg.m_adwMaxSendBand[nIdx] );
			
				// 以下为m_tMpuVmpCfg.m_tVideoEncParam[]其他参数赋值和调整
				Trans2EncParam(nIdx, &m_tParam[nIdx], &m_tMpuVmpCfg.m_tVideoEncParam[nIdx] );
				
			

				if ( m_tMpuVmpCfg.m_adwMaxSendBand[nIdx] == 0 )
				{
					// 发送带宽仍然使用初始码率进行设置
					m_tMpuVmpCfg.m_adwMaxSendBand[nIdx] = (u32)(m_tParam[nIdx].GetBitRate() * 2 + 100);
				}

			

				// 打印开始的参数
				mpulog( MPU_INFO, "Follows are the params we used to start the vmp:\n\n" );
				mpulog( MPU_INFO, "/*----------  video channel.%d  -----------*/\n", nIdx );
				m_tMpuVmpCfg.PrintEncParam( nIdx );	
				mpulog( MPU_INFO, "/*----------------------------------------*/\n\n" );
			}


			//3. 设置风格
			m_byHardStyle = ConvertVcStyle2HardStyle(m_tParam[0].m_byVMPStyle);
			m_cHardMulPic.SetMulPicType(m_byHardStyle);
			mpulog( MPU_INFO, "SetMulPicType to %d.\n", m_byHardStyle);

            //4. 开始合成
			TVideoEncParam atOrigEncParam[MAXNUM_MPUSVMP_CHANNEL];
			memcpy(atOrigEncParam,&m_tMpuVmpCfg.m_tVideoEncParam[0],sizeof(atOrigEncParam));
			for (u8 byLoop = 0 ; byLoop < MAXNUM_MPUSVMP_CHANNEL;byLoop++)
			{
				atOrigEncParam[byLoop].m_wVideoWidth = m_tParam[byLoop].GetVideoWidth();
				atOrigEncParam[byLoop].m_wVideoHeight = m_tParam[byLoop].GetVideoHeight();
				atOrigEncParam[byLoop].m_byFrameRate = m_tParam[byLoop].GetFrameRate();
			}
			
			//先下最大的分辨率，防止start的时候有低码率终端进来先编小分辨率，然后删掉该终端后编大分辨率，媒控不支持
			s32 nRet = m_cHardMulPic.StartMerge(&atOrigEncParam[0], 
                                              MAXNUM_MPUSVMP_CHANNEL);
			//下实际上需要编码的分辨率
			for ( nIdx = 0 ; nIdx < MAXNUM_MPUSVMP_CHANNEL;nIdx++)
			{
				if ( atOrigEncParam[nIdx].m_wVideoWidth != m_tMpuVmpCfg.m_tVideoEncParam[nIdx].m_wVideoWidth
					 || atOrigEncParam[nIdx].m_wVideoHeight != m_tMpuVmpCfg.m_tVideoEncParam[nIdx].m_wVideoHeight 
					|| atOrigEncParam[nIdx].m_byFrameRate != m_tMpuVmpCfg.m_tVideoEncParam[nIdx].m_byFrameRate )
				{
					m_cHardMulPic.SetEncChnlParam(&m_tMpuVmpCfg.m_tVideoEncParam[nIdx],nIdx);
				}
				
			}
			if ( HARD_MULPIC_OK != nRet )
			{
                mpulog( MPU_CRIT, " startmerge failed, Error code: %u\n", nRet );
                cServMsg.SetErrorCode( 1 );                 // FIXME: 错误码上报定义
                SendMsgToMcu( pMsg->event + 2, cServMsg );
                return;
            }

			//1. 设置边框背景等
			if( !SetVmpAttachStyle( m_tStyleInfo ) )
			{
				mpulog( MPU_CRIT, " Fail to set vmp attach style(MsgStartVidMixProc)\n" );
				cServMsg.SetMsgBody( NULL, 0 );
				cServMsg.SetErrorCode( 1 );          // FIXME: 错误码定义
				SendMsgToMcu( pMsg->event + 2, cServMsg );
				return;
			}

            //5. 通道整理
            mpulog( MPU_INFO, "StartMerge succeed!\n" );

            u8 byMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle(m_tParam[0].m_byVMPStyle);
            u8 byChnNo = 0;
            for ( byChnNo = 0; byChnNo < byMaxChnNum; byChnNo++ )
            {
                if ( m_tParam[0].m_atMtMember[byChnNo].IsNull() )
                {
                    nRet = m_cHardMulPic.RemoveChannel( byChnNo );
                    if ( HARD_MULPIC_OK == nRet)
                    {
                        m_bAddVmpChannel[byChnNo] = FALSE;
                    }
                    mpulog( MPU_INFO, "[MsgStartVidMixProc] Remove channel-%u again, ret.%u(%u is correct)!\n",  byChnNo, nRet, HARD_MULPIC_OK );
                }
                else
                {
                    nRet = m_cHardMulPic.AddChannel( byChnNo ); 
                    if ( HARD_MULPIC_OK != nRet )
                    {
                        mpulog(MPU_CRIT, "[MsgStartVidMixProc] add Chn.%d failed, ret=%u!\n", byChnNo, nRet);
                    }
                    else
                    {
                        m_bAddVmpChannel[byChnNo] = TRUE;
                    }
                    mpulog( MPU_INFO, "Add channel-%u return %u(%u is correct)!\n", byChnNo, nRet, HARD_MULPIC_OK );
                    mpulog( MPU_INFO, "RealPayLoad: %d, ActivePayload: %d\n",
                                       m_tParam[0].m_tDoublePayload[byChnNo].GetRealPayLoad(),
                                       m_tParam[0].m_tDoublePayload[byChnNo].GetActivePayload() );
                }
            }
			
            //6. 平滑发送
			//[2010/12/31 zhushz]平滑发送接口封装
//            if (!g_bPauseSS)
//            {				
//                for(u8 byLoop = 0; byLoop < MAXNUM_MPUSVMP_CHANNEL; byLoop ++)
//                {
//
//					//	[nizhijun 2010/11/20] 平滑发送开启前，是否有必要做clear，待确认
//					//ClearSmoothSendRule( m_dwMcuRcvIp, m_wMcuRcvStartPort + byLoop * PORTSPAN);
//                    SetSmoothSendRule( m_dwMcuRcvIp,
//                                       m_wMcuRcvStartPort + byLoop * PORTSPAN,
//                                       m_tParam[byLoop].GetBitRate() );
//                }				
//             }
			BOOL32 bSetRule = !g_bPauseSS;
			SetSmoothSendRule( bSetRule );

            //7. 开始码流收发
            if (!StartNetRcvSnd(byNeedPrs))
            {
                mpulog( MPU_CRIT, "StartNetRcvSnd failed!\n");
                cServMsg.SetErrorCode( 1 );                 // FIXME: 错误码上报定义
                SendMsgToMcu( pMsg->event + 2, cServMsg );
                return;
            }


			//  [1/12/2010 pengjie] Modify 设置空闲通道显示模式（黑屏，上一帧，图片...）
			TVMPExCfgInfo tVMPExCfgInfo;
			if( cServMsg.GetMsgBodyLen() > (MAXNUM_MPUSVMP_CHANNEL * sizeof(CKDVNewVMPParam) +
				sizeof(u8)) + sizeof(TVmpStyleCfgInfo) )
			{
				tVMPExCfgInfo = *(TVMPExCfgInfo *)( cServMsg.GetMsgBody() + 
					MAXNUM_MPUSVMP_CHANNEL * sizeof(CKDVNewVMPParam) + sizeof(u8) + sizeof(TVmpStyleCfgInfo) ); 	
			}
			else
			{
				tVMPExCfgInfo.m_byIdleChlShowMode = VMP_SHOW_BLACK_MODE;
			}
			
			if( tVMPExCfgInfo.m_byIdleChlShowMode > VMP_SHOW_USERDEFPIC_MODE )
			{
				tVMPExCfgInfo.m_byIdleChlShowMode = VMP_SHOW_BLACK_MODE; // 默认显示黑屏
			}
			mpulog( MPU_INFO, "[MsgStartVidMixProc] Set Vmp IdleChlShowMode: %d \n", tVMPExCfgInfo.m_byIdleChlShowMode );
			
			s32 nReturn = 0;
			// 给底层设置空闲通道显示模式
			if( VMP_SHOW_BLACK_MODE == tVMPExCfgInfo.m_byIdleChlShowMode )
			{
				nReturn = m_cHardMulPic.SetNoStreamBak((u32)NoStrm_PlyBlk);
			}
			else if( VMP_SHOW_LASTFRAME_MODE == tVMPExCfgInfo.m_byIdleChlShowMode )
			{
				nReturn = m_cHardMulPic.SetNoStreamBak((u32)NoStrm_PlyLst);
			}
			else if( VMP_SHOW_DEFPIC_MODE == tVMPExCfgInfo.m_byIdleChlShowMode )
			{
				nReturn = m_cHardMulPic.SetNoStreamBak((u32)NoStrm_PlyBmp_dft);
			}
			else // 用户自定义图片
			{
				nReturn = m_cHardMulPic.SetNoStreamBak((u32)NoStrm_PlyBmp_usr);
			}
			mpulog( MPU_INFO, "[MsgStartVidMixProc][SetNoStreamBak] return %d \n", nReturn );

			for (nIdx = 0; nIdx < MAXNUM_MPUSVMP_MEMBER; nIdx++)
			{
				//20111108 zjl modify 支持vmp成员显示自定义图片
				for (u8 byIndex = 0; byIndex < MAXNUM_VMPDEFINE_PIC; byIndex++)
				{
					if(!tParam[0].m_atMtMember[nIdx].IsNull() &&
						strlen(tParam[0].m_atMtMember[nIdx].GetMbAlias()) > 0 &&
						0 == strcmp(tParam[0].m_atMtMember[nIdx].GetMbAlias(), 
						m_tMpuVmpCfg.GetVmpMemAlias(byIndex)))
					{
						//调用接口
						mpulog( MPU_INFO, "[MsgStartVidMixProc]index.%d -> alias.%s -> pic.%s!\n", 
							nIdx, 
							m_tMpuVmpCfg.GetVmpMemAlias(byIndex), 
							m_tMpuVmpCfg.GetVmpMemRoute(byIndex));
						
						nReturn = m_cHardMulPic.SetNoStreamBak((u32)NoStrm_PlyBmp_usr, 
							(u8*)m_tMpuVmpCfg.GetVmpMemRoute(byIndex), 
							nIdx);
						if ( (s32)Codec_Success != nReturn )
						{
							OspPrintf(TRUE, FALSE, "[MsgStartVidMixProc] Index.%d SetNoStreamBak usrbmp failed(ErrCode.%d)!\n", nIdx, nReturn);
						}
						break;
					}
				}			
				//
			}

			NEXTSTATE(RUNNING);

			SendMsgToMcu( pMsg->event + 1, cServMsg );
			SendMsgToMcu( VMP_MCU_STARTVIDMIX_NOTIF, cServMsg );
            
            m_cConfId = cServMsg.GetConfId();
			SendStatusChangeMsg( 1, 1/*, 0*//*always*/ ); 
			mpulog( MPU_INFO, "Vmp works properly now, cur state.%u\n", CurState() );

			//[nizhijun 2011/03/21]VMP参数改变需要隔1秒检测关键帧请求
			KillTimer( EV_VMP_NEEDIFRAME_TIMER );
			mpulog(MPU_INFO, "[MsgStartVidMixProc]reset EV_VMP_NEEDIFRAME_TIMER from %d to: %d\n", 
				m_tMpuVmpCfg.GetIframeInterval(), CHECK_IFRAME_INTERVAL);			           
            //songkun,20110622,画面合成Iframe请求TimerOut策略调整
            for ( byChnNo = 0; byChnNo < byMaxChnNum; byChnNo++)
            {
                if( m_bAddVmpChannel[byChnNo] )
                {
                    m_byIframeReqCount[byChnNo] = 0;                 
                }        
            }
           	SetTimer( EV_VMP_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL );   // 定时查询适配组是否需要关键帧
		}

        break;

    default:
        mpulog( MPU_CRIT, "[MsgStartVidMixProc] Cannot start merge while in state:%d\n", CurState() );
    }
    return;
}

/*====================================================================
	函数名  :MsgStopVidMixProc
	功能    :停止复合器处理函数
	输入    :CMessage
	输出    :
	返回值  :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	03/12/4	    v1.0		zhangsh			create
====================================================================*/
void CMpuSVmpInst::MsgStopVidMixProc( CMessage * const pMsg )
{
    CServMsg cServMsg( pMsg->content, pMsg->length );

    switch ( CurState() )
    {
    case RUNNING:
        break;
    default:
        mpulog( MPU_CRIT, "[MsgStopVidMixProc] Wrong State:%d\n", CurState());
        return;
    }

    u8 byChnNo = 0;
    
    //0. 停止合成编码的码流回调
    for ( byChnNo = 0; byChnNo < MAXNUM_MPUSVMP_CHANNEL; byChnNo++ )
    {
        m_cHardMulPic.SetVidDataCallback( byChnNo, NULL, 0 );
    }

    //1. 停止发收
    StopNetRcvSnd();

    //2、停止平滑
	//[2010/12/31 zhushz]平滑发送接口封装
	//[nizhijun 2010/11/20] m_dwMcuRcvIp m_wMcuRcvStartPort已经是主机序，不需要在这边转了
//    ClearSmoothSendRule( m_dwMcuRcvIp, m_wMcuRcvStartPort );
//    ClearSmoothSendRule( m_dwMcuRcvIp, m_wMcuRcvStartPort  + PORTSPAN );
//    ClearSmoothSendRule( m_dwMcuRcvIp, m_wMcuRcvStartPort  + 2 * PORTSPAN );
//	  ClearSmoothSendRule( m_dwMcuRcvIp, m_wMcuRcvStartPort  + 3 * PORTSPAN );
	SetSmoothSendRule(FALSE);
	
#ifdef _LINUX_
	BOOL32 bRet = CloseSmoothSending();
	printf("close smooth result: %u\n", bRet);
#endif

    //3、清空通道
    for ( byChnNo = 0; byChnNo < MAXNUM_MPUSVMP_MEMBER; byChnNo++ )
    {
        m_cHardMulPic.RemoveChannel( byChnNo );
    }

    //4、停止复合
    s32 nRetCode = m_cHardMulPic.StopMerge();
    mpulog( MPU_INFO, "Call stopmerge() error code= %d (0 is ok)\n", nRetCode );
    if ( HARD_MULPIC_OK == nRetCode )
    {
        cServMsg.SetErrorCode(0);
    }
    else
    {
        cServMsg.SetErrorCode(1);
    }

	

    SendMsgToMcu( VMP_MCU_STOPVIDMIX_NOTIF, cServMsg );
    SendStatusChangeMsg( 1, 0/*, 0*//*always*/ );
	memset(m_tParamPrevious,0,sizeof(m_tParamPrevious));
    NEXTSTATE(NORMAL);

//     #ifndef WIN32
// 	    BrdLedStatusSet( LED_SYS_LINK, BRD_LED_OFF );// 将MLINK灯灭
// 	#endif

    return ;
} 

/*====================================================================
	函数  : MsgChangeVidMixParamProc
	功能  : 改变画面复合参数消息响应
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    03/12/4	    v1.0	    zhangsh	      create
====================================================================*/
void CMpuSVmpInst::MsgChangeVidMixParamProc( CMessage* const pMsg )
{
    BOOL bCheckRet;
    CServMsg cServMsg( pMsg->content, pMsg->length );
    CKDVNewVMPParam tParam[MAXNUM_MPUSVMP_CHANNEL];
    memset(&tParam[0], 0, sizeof(tParam));
 
    switch ( CurState() )
    {
    case RUNNING:
        break;
    default:
        mpulog( MPU_CRIT, "[MsgChangeVidMixParamProc] Wrong State:%d !\n", CurState() );
        return;
    }

	memcpy( (u8*)tParam, cServMsg.GetMsgBody(), MAXNUM_MPUSVMP_CHANNEL * sizeof(CKDVNewVMPParam) );

	u8 byNewStyleChnNum = tParam[0].m_byMemberNum;
    u8 byOldStyleChnNum = m_tParam[0].m_byMemberNum;
    
    //上一次参数
    memset(m_tParamPrevious, 0, sizeof(m_tParamPrevious));
    memcpy(m_tParamPrevious, m_tParam, sizeof(m_tParam));

    //新的合成参数
    memcpy(m_tParam, tParam, MAXNUM_MPUSVMP_CHANNEL * sizeof(CKDVNewVMPParam) );

    //不会设给底层（没有StartMerge），但在平滑时会用到。
    if ( m_tMpuVmpCfg.IsEnableBitrateCheat() )
    {   
		u8 byChnnlId = 0;
		for(; byChnnlId < MAXNUM_MPUSVMP_CHANNEL; byChnnlId ++)
		{
			m_tParam[byChnnlId].SetBitRate(m_tMpuVmpCfg.GetDecRateDebug(m_tParam[byChnnlId].GetBitRate()));
			mpulog( MPU_INFO, "New Enc Bitrate(chl.%d): %d\n", byChnnlId, m_tParam[byChnnlId].GetBitRate() );
			
			
		}
    }   

    //u8 byNeedPrs = *( cServMsg.GetMsgBody() + MAXNUM_MPUSVMP_CHANNEL * sizeof(CKDVNewVMPParam) );
    
    m_tStyleInfo = *(TVmpStyleCfgInfo*)( cServMsg.GetMsgBody() + 
                                         MAXNUM_MPUSVMP_CHANNEL * sizeof(CKDVNewVMPParam) + 
                                         sizeof(u8) );
    
    //1、风格和边框切变
    if ( byNewStyleChnNum >= byOldStyleChnNum )
    {
        mpulog( MPU_INFO, "NewStyleChnNum >= OldStyleChnNum\n" );

        
        //[2].设置画面合成风格
        bCheckRet = ChangeVMPStyle();
        if ( bCheckRet )
        {
            cServMsg.SetErrorCode( 0 ); // ok
        }
        else
        {
            cServMsg.SetErrorCode( 1 ); // error
        }

		//[1].设置边框和颜色
        BOOL32 bRet = SetVmpAttachStyle( m_tStyleInfo );
        if ( !bRet )
        {
            mpulog( MPU_CRIT, " Fail to set vmp attach style (MsgChangeVidMixParamProc)\n" );
            cServMsg.SetMsgBody( NULL, 0 );
            cServMsg.SetErrorCode( 1 );
            SendMsgToMcu( pMsg->event + 2, cServMsg );
            return;
        }
        else
        {
            mpulog( MPU_INFO, "now we get new params from mcu, change according to it!\n" );
            SendMsgToMcu( pMsg->event + 1, cServMsg );
        }
    }
    else
    {
        mpulog( MPU_INFO, "NewStyleChnNum < OldStyleChnNum\n" );

        //[2].设置画面合成风格
        bCheckRet = ChangeVMPStyle();
        if (bCheckRet)
        {
            cServMsg.SetErrorCode( 0 ); // ok
        }
        else
        {
            cServMsg.SetErrorCode( 1 ); // error
        }
        //[1].设置边框和颜色
        BOOL32 bRet = SetVmpAttachStyle( m_tStyleInfo );
        if (!bRet)
        {
            mpulog( MPU_CRIT, "[Info]Fail to set vmp attach style (MsgChangeVidMixParamProc)\n" );
            cServMsg.SetMsgBody( NULL, 0 );
            cServMsg.SetErrorCode( 1 );
            SendMsgToMcu( pMsg->event + 2, cServMsg );
            return;
        }
        else
        {
            mpulog( MPU_INFO, "[MsgChangeVidMixParamProc]now we get new params from mcu, change according to it!\n" );
            SendMsgToMcu( pMsg->event + 1, cServMsg );
        }
    }

	//20111108 zjl modify 支持vmp成员显示自定义图片
	
	TVMPExCfgInfo tVMPExCfgInfo;
	if( cServMsg.GetMsgBodyLen() > (MAXNUM_MPUSVMP_CHANNEL * sizeof(CKDVNewVMPParam) +
		sizeof(u8)) + sizeof(TVmpStyleCfgInfo) )
	{
		tVMPExCfgInfo = *(TVMPExCfgInfo *)( cServMsg.GetMsgBody() + 
			MAXNUM_MPUSVMP_CHANNEL * sizeof(CKDVNewVMPParam) + sizeof(u8) + sizeof(TVmpStyleCfgInfo) ); 	
	}
	else
	{
		tVMPExCfgInfo.m_byIdleChlShowMode = VMP_SHOW_BLACK_MODE;
	}
	
	s32 nReturn = 0;
	BOOL32 bSetBmp = FALSE;
	for (u8 byIdx = 0; byIdx < MAXNUM_MPUSVMP_MEMBER; byIdx++)
	{	
		bSetBmp = FALSE;
		if( !tParam[0].m_atMtMember[byIdx].IsNull())
		{
			for (u8 byIndex = 0; byIndex < MAXNUM_VMPDEFINE_PIC; byIndex++)
			{
				
				if ( strlen(tParam[0].m_atMtMember[byIdx].GetMbAlias()) >0 && 
					0 == strcmp(tParam[0].m_atMtMember[byIdx].GetMbAlias(), m_tMpuVmpCfg.GetVmpMemAlias(byIndex)))
				{
					//调用接口
					OspPrintf(TRUE, FALSE, "[MsgChangeVidMixParamProc]index.%d -> alias.%s -> pic.%s!\n", 
						byIdx, 
						m_tMpuVmpCfg.GetVmpMemAlias(byIndex), 
						m_tMpuVmpCfg.GetVmpMemRoute(byIndex));
					
					nReturn = m_cHardMulPic.SetNoStreamBak((u32)NoStrm_PlyBmp_usr, 
						(u8*)m_tMpuVmpCfg.GetVmpMemRoute(byIndex), 
						byIdx);
					if ((s32)Codec_Success != nReturn)
					{
						OspPrintf(TRUE, FALSE, "[MsgChangeVidMixParamProc] SetNoStreamBak bmp .%d failed(ErrCode.%d)!\n", byIdx, nReturn);
					}
					else
					{
						bSetBmp = TRUE;
					}
					break;
				}
			}
		}
		if ( tParam[0].m_atMtMember[byIdx] == m_tParamPrevious[0].m_atMtMember[byIdx] || tParam[0].m_atMtMember[byIdx].IsNull() && m_tParamPrevious[0].m_atMtMember[byIdx].IsNull() )
		{
			mpulog(MPU_INFO,"[MsgChangeVidMixParamProc]Idx :%d,previous and now member is same mt,no need to reset nostreambak,contiue.\n",byIdx);
			continue;
		}
		if (!bSetBmp )
		{
			// 给底层设置空闲通道显示模式
			if( VMP_SHOW_BLACK_MODE == tVMPExCfgInfo.m_byIdleChlShowMode )
			{
				nReturn = m_cHardMulPic.SetNoStreamBak((u32)NoStrm_PlyBlk, NULL, byIdx);
			}
			else if( VMP_SHOW_LASTFRAME_MODE == tVMPExCfgInfo.m_byIdleChlShowMode )
			{
				nReturn = m_cHardMulPic.SetNoStreamBak((u32)NoStrm_PlyLst, NULL, byIdx);
			}
			else if( VMP_SHOW_DEFPIC_MODE == tVMPExCfgInfo.m_byIdleChlShowMode )
			{
				nReturn = m_cHardMulPic.SetNoStreamBak((u32)NoStrm_PlyBmp_dft, NULL, byIdx);
			}
			else
			{
				nReturn = m_cHardMulPic.SetNoStreamBak((u32)NoStrm_PlyBmp_usr, NULL, byIdx);
			}
		
			if ((s32)Codec_Success != nReturn)
			{
				OspPrintf(TRUE, FALSE, "[MsgChangeVidMixParamProc] Index.%d SetNoStreamBak .%d failed(ErrCode.%d)!\n", byIdx, tVMPExCfgInfo.m_byIdleChlShowMode, nReturn);
			}
			else
			{
				OspPrintf(TRUE, FALSE, "[MsgChangeVidMixParamProc] Index.%d SetNoStreamBak .%d success!\n", byIdx, tVMPExCfgInfo.m_byIdleChlShowMode);
			}
		}	
	}

    //3、通道和对应的net接收 切变
	BOOL32 bChangeStyle = (m_tParamPrevious[0].m_byVMPStyle != m_tParam[0].m_byVMPStyle);
    ChangeChnAndNetRcv( bChangeStyle );
	
	//4、改变码率
	for( u8 byChnnlId = 0; byChnnlId < MAXNUM_MPUSVMP_CHANNEL; byChnnlId ++)
	{
		if( m_tParam[byChnnlId].GetBitRate() != m_tParamPrevious[byChnnlId].GetBitRate() )
		{
			//change br
			m_cHardMulPic.ChangeBitRate( byChnnlId, m_tParam[byChnnlId].GetBitRate() );
			//根据码率，读码率分辨率限制信息表，得到新的分辨率和帧率，看是否和老的相同，不相同，重新下参
			u16 wVideoOldWith = m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId].m_wVideoWidth;
			u16 wVideoOldHeight = m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId].m_wVideoHeight;
			u8 byOldFramerate = m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId].m_byFrameRate;
			m_tMpuVmpCfg.GetDefaultParam( m_tParam[byChnnlId].m_byEncType,
				m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId],
				m_tMpuVmpCfg.m_adwMaxSendBand[byChnnlId] );
			Trans2EncParam(byChnnlId,&m_tParam[byChnnlId],&m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId]);
			if ( 	m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId].m_wVideoWidth != wVideoOldWith || 
					m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId].m_wVideoHeight != wVideoOldHeight ||
					m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId].m_byFrameRate != byOldFramerate
				)
			{
				m_cHardMulPic.SetEncChnlParam( &m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId], byChnnlId );
				mpulog( MPU_INFO,"[MsgSetBitRate] set encchan:%d new encparam:\n",byChnnlId);
				m_tMpuVmpCfg.PrintEncParam( byChnnlId );	
			}
		
			
	
			
		}
	}

	//[2010/12/31 zhushz]平滑发送接口封装
	//5、设置平滑
	BOOL32 bSetRule = !g_bPauseSS;
	SetSmoothSendRule( bSetRule );
    
    //4、平滑发送切变(含ChangeBitRate)
//    if (!g_bPauseSS)
//    {
//		u8 byChnnlId = 0;
//		for(; byChnnlId < MAXNUM_MPUSVMP_CHANNEL; byChnnlId ++)
//		{
			//No need to call SetSmoothSendRule if biterate has no change at all 
//			if( m_tParam[byChnnlId].GetBitRate() != m_tParamPrevious[byChnnlId].GetBitRate() )
//			{
//				//change br
//				m_cHardMulPic.ChangeBitRate( byChnnlId, m_tParam[byChnnlId].GetBitRate() );
				
			//	[nizhijun 2010/11/20] 平滑发送已开启，这时候不需要停止发送后再设置平滑
			//	ClearSmoothSendRule( m_dwMcuRcvIp, m_wMcuRcvStartPort + byChnnlId * PORTSPAN);

//				SetSmoothSendRule(  m_dwMcuRcvIp, m_wMcuRcvStartPort + byChnnlId * PORTSPAN, m_tParam[byChnnlId].GetBitRate());
// 			}
// 		}
		/*
		// xliang [7/27/2009] No need to call SetSmoothSendRule if biterate has no change at all 
		// or previous  value is larger
		if( m_tParam[0].GetBitRate() > m_tParamPrevious[0].GetBitRate() )
		{
		#ifdef _LINUX_
			BOOL32 bRet = CloseSmoothSending();
			printf("close smooth result: %u\n", bRet);
		#endif
			u8 byChnnlId = 0;
			for(; byChnnlId < MAXNUM_MPUSVMP_CHANNEL; byChnnlId ++)
			{
				//ClearSmoothSendRule( m_dwMcuRcvIp, m_wMcuRcvStartPort + byChnnlId * PORTSPAN);
				SetSmoothSendRule( m_dwMcuRcvIp, m_wMcuRcvStartPort + byChnnlId * PORTSPAN, m_tParam[byChnnlId].GetBitRate());
			}			
		}*/
//     }

    SendMsgToMcu( VMP_MCU_CHANGESTATUS_NOTIF, cServMsg );
    mpulog( MPU_INFO, "[[MsgChangeVidMixParamProc]]change param success,vmp work according to new params!\n" );
    
    SendStatusChangeMsg( 1, 1/*, 0*//*always*/ );

	//[nizhijun 2011/03/21]VMP参数改变需要隔1秒检测关键帧请求
	KillTimer( EV_VMP_NEEDIFRAME_TIMER );
	mpulog(MPU_INFO, "[MsgChangeVidMixParamProc]reset EV_VMP_NEEDIFRAME_TIMER from %d to: %d\n", 
		m_tMpuVmpCfg.GetIframeInterval(), CHECK_IFRAME_INTERVAL);
	
    
    //songkun,20110622,画面合成Iframe请求TimerOut策略调整
    u8 byMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle(m_tParam[0].m_byVMPStyle);
    u8 byChnNo = 0;
    for ( byChnNo = 0; byChnNo < byMaxChnNum; byChnNo++)
    {
        if( m_bAddVmpChannel[byChnNo] )
        {   
            m_byIframeReqCount[byChnNo] = 0;          
        }        
    }
  	SetTimer( EV_VMP_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL );   // 定时查询适配组是否需要关键帧
    return ;
}

/*====================================================================
	函数  : MsgSetBitRate
	功能  : 改变发送码率
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	03/12/4		v1.0		zhangsh			create
====================================================================*/
void CMpuSVmpInst::MsgSetBitRate( CMessage* const pMsg )
{
    CServMsg cServMsg( pMsg->content, pMsg->length );
    u8 byChalNum = cServMsg.GetChnIndex();
    u16 wBitrate = 0;
	u16 wCheatBit = 0;

    cServMsg.GetMsgBody( (u8*)&wBitrate, sizeof(u16) ) ;
    wBitrate = ntohs(wBitrate);

    mpulog( MPU_INFO, "[MsgSetBitRate]now we set the encode-%u bitrate to %u!\n", byChalNum, wBitrate );

    // guzh [10/18/2007] 支持码率作弊
    if ( m_tMpuVmpCfg.IsEnableBitrateCheat() )
    {
        wCheatBit = m_tMpuVmpCfg.GetDecRateDebug( wBitrate );
    }   
	
    m_cHardMulPic.ChangeBitRate( byChalNum, wCheatBit );
	//u8 byLoop = 0;
	
	m_tParam[byChalNum].SetBitRate(wCheatBit);
	//[2010/12/31 zhushz]平滑发送接口封装
//	if( !g_bPauseSS)
//	{
		//No need to call SetSmoothSendRule if biterate has no change at all 
//		if( m_tParam[byChalNum].GetBitRate() != m_tParamPrevious[byChalNum].GetBitRate() )
//		{
//			//	[nizhijun 2010/11/20 平滑发送已开启，这时候不需要停止发送后再设置平滑
//			//ClearSmoothSendRule( m_dwMcuRcvIp, m_wMcuRcvStartPort + byChalNum * PORTSPAN);
//
//			//[2010/12/31 zhushz]平滑发送接口封装
//			SetSmoothSendRule( byChalNum);
//			// [9/30/2010 xliang] refresh ParamPrevious
// 			m_tParamPrevious[byChalNum].SetBitRate(wCheatBit);
// 		}
//	}

	if( m_tParam[byChalNum].GetBitRate() != m_tParamPrevious[byChalNum].GetBitRate() )
	{
		m_tParamPrevious[byChalNum].SetBitRate(wCheatBit);
		//根据码率，读码率分辨率限制信息表，得到新的分辨率和帧率，看是否和老的相同，不相同，重新下参
		u16 wVideoOldWith = m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_wVideoHeight;
		u16 wVideoOldHeight = m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_wVideoHeight;
		u8 byOldFrameRate = m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_byFrameRate;
		m_tMpuVmpCfg.GetDefaultParam( m_tParam[byChalNum].m_byEncType,
			m_tMpuVmpCfg.m_tVideoEncParam[byChalNum],
			m_tMpuVmpCfg.m_adwMaxSendBand[byChalNum] );
		Trans2EncParam(byChalNum,&m_tParam[byChalNum],&m_tMpuVmpCfg.m_tVideoEncParam[byChalNum]);
		if ( 	m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_wVideoWidth != wVideoOldWith || 	
				m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_wVideoHeight != wVideoOldHeight ||
				m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_byFrameRate != byOldFrameRate
				)
		{
			m_cHardMulPic.SetEncChnlParam( &m_tMpuVmpCfg.m_tVideoEncParam[byChalNum], byChalNum );
			mpulog( MPU_INFO,"[MsgSetBitRate] set encchan:%d new encparam:\n",byChalNum);
			m_tMpuVmpCfg.PrintEncParam( byChalNum );	
		}
		
	}
	BOOL32 bSetRule = !g_bPauseSS;
	SetSmoothSendRule( bSetRule );

	/*
	for(; byLoop < MAXNUM_MPUSVMP_CHANNEL; byLoop ++)
	{
		//m_tParam[byLoop].m_wBitRate = wCheatBit;
		m_tParam[byLoop].SetBitRate(wCheatBit);
	}
    if (!g_bPauseSS)
    {
        // 平滑发送
		// xliang [7/27/2009] No need to call SetSmoothSendRule if biterate has no change at all 
		// or previous  value is larger
		if( m_tParam[0].GetBitRate() > m_tParamPrevious[0].GetBitRate() )
		{
		#ifdef _LINUX_
			BOOL32 bRet = CloseSmoothSending();
			printf("[MsgSetBitRate]close smooth result: %u\n", bRet);
		#endif
			u8 byChnnlId = 0;
			for(; byChnnlId < MAXNUM_MPUSVMP_CHANNEL; byChnnlId ++)
			{
				
				//ClearSmoothSendRule( m_dwMcuRcvIp, m_wMcuRcvStartPort + byChnnlId * PORTSPAN);
				SetSmoothSendRule( m_dwMcuRcvIp, m_wMcuRcvStartPort + byChnnlId * PORTSPAN, m_tParam[byChnnlId].GetBitRate());
			}
		}

    }
	*/

    SendMsgToMcu( VMP_MCU_SETCHANNELBITRATE_ACK, cServMsg );
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
	03/12/4		v1.0		zhangsh			create
====================================================================*/
void CMpuSVmpInst::SendStatusChangeMsg( u8 byOnline, u8 byState/*, u8 byStyle */)
{
    CServMsg cServMsg;
    TPeriEqpStatus tInitStatus;
    memset( &tInitStatus,0,sizeof(TPeriEqpStatus) );

    tInitStatus.m_byOnline = byOnline;
    tInitStatus.SetMcuEqp( m_tEqp.GetMcuId(), m_tEqp.GetEqpId(), m_tEqp.GetEqpType() );
    tInitStatus.SetAlias( m_tCfg.achAlias );
    tInitStatus.m_tStatus.tVmp.m_tVMPParam.SetVMPStyle( m_tParam[0].m_byVMPStyle );
    tInitStatus.m_tStatus.tVmp.m_byUseState = byState;

    mpulog( MPU_INFO, "Before: alias: %s EqpId: %u EqpType: %u McuId: %u\n",
                                m_tCfg.achAlias, 
                                m_tEqp.GetEqpId(), 
                                m_tEqp.GetEqpType(), 
                                m_tEqp.GetMcuId() );
    mpulog( MPU_INFO, "After: alias: %s EqpId: %u EqpType: %u McuId: %u\n",
                                m_tCfg.achAlias, 
                                tInitStatus.GetEqpId(), 
                                tInitStatus.GetEqpType(), 
                                tInitStatus.GetMcuId() );

    tInitStatus.m_tStatus.tVmp.m_byChlNum = MAXNUM_MPUSVMP_MEMBER;
	tInitStatus.m_tStatus.tVmp.m_bySubType = MPU_SVMP; 
#ifdef WIN32//songkun,20110628,加入MPU WIN32 B版编译
	tInitStatus.m_tStatus.tVmp.m_byBoardVer = MPU_BOARD_B256;
#else    
    #ifndef _MPUB_256_ 
	    tInitStatus.m_tStatus.tVmp.m_byBoardVer = MPU_BOARD_A128;
    #else
	    tInitStatus.m_tStatus.tVmp.m_byBoardVer = MPU_BOARD_B256;
    #endif
#endif

	mpulog(MPU_INFO, "[SendStatusChangeMsg] mpu boardVer is %u\n", tInitStatus.m_tStatus.tVmp.m_byBoardVer);

    tInitStatus.SetAlias(m_tCfg.achAlias);
    cServMsg.SetMsgBody( (u8*)&(tInitStatus), sizeof(TPeriEqpStatus) );
    SendMsgToMcu( VMP_MCU_VMPSTATUS_NOTIF, cServMsg );
    return;
}

/*=============================================================================
  函 数 名： MsgUpdataVmpEncryptParamProc
  功    能： 更改加密参数
  算法实现： 
  全局变量： 
  参    数： CMessage * const pMsg
  返 回 值： void 
=============================================================================*/
void CMpuSVmpInst::MsgUpdataVmpEncryptParamProc( CMessage * const pMsg )
{ 
    if( NULL == pMsg )
	{
		mpulog( MPU_CRIT, "[MsgUpdataVmpEncryptParamProc] Error input parameter.\n" );
		return;
	}

	u8  byChannelNo = 0;
    TMediaEncrypt  tVideoEncrypt;
    TDoublePayload tDoublePayload;

    CServMsg cServMsg( pMsg->content, pMsg->length );

    byChannelNo    = *(u8 *)( cServMsg.GetMsgBody() );
    tVideoEncrypt  = *(TMediaEncrypt *)( cServMsg.GetMsgBody() + sizeof(u8) );
    tDoublePayload = *(TDoublePayload *)( cServMsg.GetMsgBody() + sizeof(u8) + sizeof(TMediaEncrypt) );

    mpulog( MPU_INFO, "byChannelNo: %d, ActivePayLoad: %d, RealPayLoad: %d\n", 
                                                        byChannelNo,
                                                        tDoublePayload.GetActivePayload(),
                                                        tDoublePayload.GetRealPayLoad() );

    SetDecryptParam( byChannelNo, &tVideoEncrypt/*, &tDoublePayload*/ );
    if ( !m_bAddVmpChannel[byChannelNo] )
    {      
        s32 nRet;
        nRet = m_cHardMulPic.AddChannel( byChannelNo );
        if ( HARD_MULPIC_OK == nRet )
        {
            m_bAddVmpChannel[byChannelNo] = TRUE;
        }

        mpulog( MPU_INFO, "[Info]Channel-%u add return value is %u(correct is %u),the Ip is %u Port is %u!\n",
                                   byChannelNo, nRet, HARD_MULPIC_OK );

    }

	//3、更新payload

	m_tParamPrevious[0].m_tDoublePayload[byChannelNo].SetActivePayload(tDoublePayload.GetActivePayload());
	m_tParamPrevious[0].m_tDoublePayload[byChannelNo].SetRealPayLoad(tDoublePayload.GetRealPayLoad());
	


	// [7/31/2010 xliang] 重新设置编码参数
	for(u8 byEncChnl = 0; byEncChnl < MAXNUM_MPUSVMP_CHANNEL; byEncChnl++)
	{
		if(!m_bUpdateMediaEncrpyt[byEncChnl])
		{
			SetEncryptParam( byEncChnl, &tVideoEncrypt, m_tParam[byEncChnl].m_byEncType );
			m_bUpdateMediaEncrpyt[byEncChnl] = TRUE;
		}
	}

	
    
	cServMsg.SetMsgBody( NULL, 0 );
    SendMsgToMcu( VMP_MCU_CHANGESTATUS_NOTIF, cServMsg );

    SendStatusChangeMsg( 1, 1/*, 0*//*always*/ );

	return;
}

/*====================================================================
	函数  : MsgFastUpdatePicProc
	功能  : 
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
    日  期         版本        修改人       修改内容
    05/06/23       v3.6       libo        create
	09/06/12       v4.6		  xl			分通道
====================================================================*/
void CMpuSVmpInst::MsgFastUpdatePicProc( CMessage * const pMsg )
{
    CServMsg cServMsg( pMsg->content, pMsg->length );
	u8 byChnnlId = cServMsg.GetChnIndex();

	u32 dwTimeInterval = 1 * OspClkRateGet();

    u32 dwCurTick = OspTickGet();	
    if( dwCurTick - m_adwLastFUPTick[byChnnlId] > dwTimeInterval 
		)
    {
        m_adwLastFUPTick[byChnnlId] = dwCurTick;

		m_cHardMulPic.SetFastUpdata( byChnnlId );
//         m_cHardMulPic.SetFastUpdata( 0 );
//         m_cHardMulPic.SetFastUpdata( 1 );
//         m_cHardMulPic.SetFastUpdata( 2 );
	
        mpulog( MPU_INFO, "[MsgFastUpdatePicProc]m_cHardMulPic.SetFastUpdata(%u)!\n",byChnnlId );
    }
	return;
}

/*====================================================================
	函数  : MsgTimerNeedIFrameProc
	功能  : 
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
    日  期         版本        修改人       修改内容
    05/06/15       v3.6       libo        create
====================================================================*/
void CMpuSVmpInst::MsgTimerNeedIFrameProc( void )
{
	KillTimer(EV_VMP_NEEDIFRAME_TIMER);

    CServMsg cServMsg;
    cServMsg.SetConfId( m_cConfId );
    TMulPicChnStatis tDecStatis;
    //只查询当前风格下的最大通道数
    u8 byMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle(m_tParam[0].m_byVMPStyle);
    //songkun,20110622,画面合成Iframe请求TimerOut策略调整 
    for (u8 byChnNo = 0; byChnNo < byMaxChnNum; byChnNo++)
    {        
        if(m_bAddVmpChannel[byChnNo])
        {
            if ( 0 == m_byIframeReqCount[byChnNo])
            {
                memset(&tDecStatis, 0, sizeof(tDecStatis));
                m_cHardMulPic.GetDecStatis( byChnNo, tDecStatis );
                if (tDecStatis.m_bWaitKeyFrame 
				&& CurState() == RUNNING )//VMP不使用时不必狂请求关键帧了 
                {
                    //用户可配
                    m_byIframeReqCount[byChnNo] = (u8)(m_tMpuVmpCfg.GetIframeInterval() / CHECK_IFRAME_INTERVAL);
                    cServMsg.SetChnIndex(byChnNo);
                    SendMsgToMcu(VMP_MCU_NEEDIFRAME_CMD, cServMsg);
                    mpulog(MPU_INFO,"[MsgTimerNeedIFrameProc] vmp channel:(%d) request iframe，next time is %u !!\n", byChnNo,m_byIframeReqCount[byChnNo]);
                }
                else
                {
                    mpulogall("[MsgTimerNeedIFrameProc]vmp channel:%d NOT NEED request iframe!!\n", byChnNo);
                }
            }
            if ( 0 != m_byIframeReqCount[byChnNo])
            {
                --m_byIframeReqCount[byChnNo];                                    
            }
        }
        else
        {   
            mpulogall("[MsgTimerNeedIFrameProc]vmp channel:%d NOT Add!!\n", byChnNo);
        }
    }	
    SetTimer(EV_VMP_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL);
    mpulog(MPU_INFO,"[MsgTimerNeedIFrameProc]vmp set EV_VMP_NEEDIFRAME_TIMER: %d !\n", CHECK_IFRAME_INTERVAL);

   	//mpulog(MPU_INFO, "[MsgTimerNeedIFrameProc]set EV_VMP_NEEDIFRAME_TIMER : %d\n", m_tMpuVmpCfg.GetIframeInterval());
    //SetTimer(EV_VMP_NEEDIFRAME_TIMER, m_tMpuVmpCfg.GetIframeInterval());

	return;
}

/*=============================================================================
  函 数 名： ProcGetMsStatusRsp
  功    能： 处理取主备倒换状态
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
=============================================================================*/
void CMpuSVmpInst::ProcGetMsStatusRsp( CMessage* const pMsg )
{
	BOOL bSwitchOk = FALSE;
	CServMsg cServMsg( pMsg->content, pMsg->length );
	if( MCU_EQP_GETMSSTATUS_ACK == pMsg->event ) // Mcu响应消息
	{
		TMcuMsStatus *ptMsStatus = (TMcuMsStatus *)cServMsg.GetMsgBody();
        
        KillTimer( TIME_GET_MSSTATUS );
        mpulog( MPU_INFO, "[ProcGetMsStatus]. receive msg MCU_EQP_GETMSSTATUS_ACK. IsMsSwitchOK :%d\n", 
                  ptMsStatus->IsMsSwitchOK() );

        if ( ptMsStatus->IsMsSwitchOK() ) // 倒换成功
        {
            bSwitchOk = TRUE;
        }
	}
	// 倒换失败或者超时
	if( !bSwitchOk )
	{
		ClearCurrentInst();
		mpulog(MPU_INFO, "[ProcGetMsStatus] ClearCurrentInst!\n");

		if ( OspIsValidTcpNode(m_dwMcuNode))
		{
			mpulog(MPU_INFO, "[ProcGetMsStatus] OspDisconnectTcpNode A!\n");
			OspDisconnectTcpNode(m_dwMcuNode );
		}
		if ( OspIsValidTcpNode(m_dwMcuNodeB))
		{
			mpulog(MPU_INFO, "[ProcGetMsStatus] OspDisconnectTcpNode B!\n");
			OspDisconnectTcpNode(m_dwMcuNodeB );
		}

		if ( INVALID_NODE == m_dwMcuNode )
		{
			mpulog(MPU_INFO, "[ProcGetMsStatus] EV_VMP_CONNECT_TIMER A!\n");
			SetTimer( EV_VMP_CONNECT_TIMER, MPU_CONNETC_TIMEOUT );
		}
		if ( INVALID_NODE == m_dwMcuNodeB )
		{
			mpulog(MPU_INFO, "[ProcGetMsStatus] EV_VMP_CONNECT_TIMER B!\n");
			SetTimer( EV_VMP_CONNECT_TIMERB, MPU_CONNETC_TIMEOUT );
		}
	}
	return;
}

/*====================================================================
	函数  : ChangeVMPStyle
	功能  : 改变画面复合参数(改变了风格)消息响应
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    03/12/4	    v1.0	    zhangsh	      create
    09/01/14    4.6         周文          从标清移植过来
    09/03/14    4.6         张宝卿        重新整理起停和通道增删逻辑
====================================================================*/
BOOL CMpuSVmpInst::ChangeVMPStyle()
{
    s32 nRet = 0;
    //BOOL bChangeStyle = FALSE;

    u8 byChnNo = 0;
    //u8 byMaxChnNum = 0;

    //切换风格
    u8 byStyle = ConvertVcStyle2HardStyle( m_tParam[0].m_byVMPStyle );
    mpulog( MPU_INFO, "[ChangeVMPStyle] OldHardStyle: %d, NewHardStyle: %d\n", byStyle, m_byHardStyle );

//    BOOL32 bVMPStyleChged = FALSE;

    //风格切变
    if ( m_byHardStyle != byStyle )
    {
        //先停止旧风格下的通道接收
        u8 byOldStyle = ConvertHardStyle2VcStyle(m_byHardStyle);
        u8 byOldMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle(byOldStyle);

        for (byChnNo = 0; byChnNo < byOldMaxChnNum; byChnNo ++)
        {
            m_pcMediaRcv[byChnNo]->StopRcv();
        }
        //切风格
        nRet = m_cHardMulPic.SetMulPicType( byStyle );
        if ( HARD_MULPIC_OK != nRet )
        {
            mpulog( MPU_CRIT, " Fail to call: SetMulPicType, Error code: %d\n", nRet );
            return FALSE;
        }

        m_byHardStyle = byStyle;
//        bVMPStyleChged = TRUE;
    }

    return TRUE;
}

/*=============================================================================
  函 数 名： ConvertVcStyle2HardStyle
  功    能： 由会控风格得到底层风格
  算法实现： 
  全局变量： 
  参    数： u8 oldstyle
  返 回 值： u8 
=============================================================================*/
u8 CMpuSVmpInst::ConvertVcStyle2HardStyle( u8 byOldstyle )
{
    u8 byStyle;
    switch ( byOldstyle )
    {
    case VMP_STYLE_ONE:  // 一画面
        byStyle = (u8)MULPIC_TYPE_ONE;  // 单画面
        break;

    case VMP_STYLE_VTWO: // 两画面：左右分
        byStyle = (u8)MULPIC_TYPE_VTWO;  // 两画面：左右分
        break;

    case VMP_STYLE_HTWO:  // 两画面: 一大一小
        byStyle = (u8)MULPIC_TYPE_ITWO; // 两画面： 一大一小（大画面分成9份发送）
        break;

    case VMP_STYLE_THREE: // 三画面
        byStyle = (u8)MULPIC_TYPE_THREE; // 三画面
        break;

    case VMP_STYLE_FOUR: // 四画面
        byStyle = (u8)MULPIC_TYPE_FOUR;  // 四画面
        break;

    case VMP_STYLE_SIX: //六画面
        byStyle = (u8)MULPIC_TYPE_SIX; //六画面
        break;

    case VMP_STYLE_EIGHT: //八画面
        byStyle = (u8)MULPIC_TYPE_EIGHT; //八画面
        break;
    case VMP_STYLE_NINE: //九画面
        byStyle = (u8)MULPIC_TYPE_NINE; //九画面
        break;

    case VMP_STYLE_TEN: //十画面
        byStyle = (u8)MULPIC_TYPE_TEN; //十画面：二大八小（大画面分成4份发送）
        break;

    case VMP_STYLE_THIRTEEN:  //十三画面
        byStyle = (u8)MULPIC_TYPE_THIRTEEN; //十三画面
        break;

    case VMP_STYLE_SIXTEEN: //十六画面
        byStyle = (u8)MULPIC_TYPE_SIXTEEN; //十六画面
        break;

    case VMP_STYLE_TWENTY: //二十画面
        byStyle = (u8)MULPIC_TYPE_TWENTY;
        break;
        
    case VMP_STYLE_SPECFOUR://特殊四画面 
        byStyle = (u8)MULPIC_TYPE_SFOUR; //特殊四画面：一大三小（大画面分成4份发送）
        break;

    case VMP_STYLE_SEVEN: //七画面
        byStyle = (u8)MULPIC_TYPE_SEVEN; //七画面：三大四小（大画面分成4份发送）
        break;

    default:
        byStyle = (u8)MULPIC_TYPE_VTWO;    // 默认两画面：左右分
        break;
    }
    return byStyle;
}


/*=============================================================================
  函 数 名： ConvertHardStyle2VcStyle
  功    能： 由底层风格得到会控风格
  算法实现： 
  全局变量： 
  参    数： u8 byHardStyle
  返 回 值： u8 
=============================================================================*/
u8 CMpuSVmpInst::ConvertHardStyle2VcStyle(u8 byHardStyle)
{
    u8 byStyle = 0;
    switch ( byHardStyle )
    {
    case MULPIC_TYPE_ONE:  // 一画面
        byStyle = VMP_STYLE_ONE;  // 单画面
        break;
        
    case MULPIC_TYPE_VTWO: // 两画面：左右分
        byStyle = VMP_STYLE_VTWO;  // 两画面：左右分
        break;
        
    case MULPIC_TYPE_ITWO:  // 两画面: 一大一小
        byStyle = VMP_STYLE_HTWO; // 两画面： 一大一小（大画面分成9份发送）
        break;
        
    case MULPIC_TYPE_THREE: // 三画面
        byStyle = VMP_STYLE_THREE; // 三画面
        break;
        
    case MULPIC_TYPE_FOUR: // 四画面
        byStyle = VMP_STYLE_FOUR;  // 四画面
        break;
        
    case MULPIC_TYPE_SIX: //六画面
        byStyle = VMP_STYLE_SIX; //六画面
        break;
        
    case MULPIC_TYPE_EIGHT: //八画面
        byStyle = VMP_STYLE_EIGHT; //八画面
        break;

    case MULPIC_TYPE_NINE: //九画面
        byStyle = VMP_STYLE_NINE; //九画面
        break;
        
    case MULPIC_TYPE_TEN: //十画面
        byStyle = VMP_STYLE_TEN; //十画面：二大八小（大画面分成4份发送）
        break;
        
    case MULPIC_TYPE_THIRTEEN:  //十三画面
        byStyle = VMP_STYLE_THIRTEEN; //十三画面
        break;
        
    case MULPIC_TYPE_SIXTEEN: //十六画面
        byStyle = VMP_STYLE_SIXTEEN; //十六画面
        break;
        
    case MULPIC_TYPE_TWENTY: //二十画面
        byStyle = VMP_STYLE_TWENTY;
        break;
        
    case MULPIC_TYPE_SFOUR://特殊四画面 
        byStyle = VMP_STYLE_SPECFOUR; //特殊四画面：一大三小（大画面分成4份发送）
        break;
        
    case MULPIC_TYPE_SEVEN: //七画面
        byStyle = VMP_STYLE_SEVEN; //七画面：三大四小（大画面分成4份发送）
        break;
        
    default:
        byStyle = VMP_STYLE_VTWO;    // 默认两画面：左右分
        break;
    }
    return byStyle;
}

/*=============================================================================
  函 数 名： ProcSetVmpAttachStyle
  功    能： 设置复合风格
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
=============================================================================*/
BOOL32 CMpuSVmpInst::SetVmpAttachStyle( TVmpStyleCfgInfo&  tStyleInfo )
{
//	return TRUE;

	memcpy( (u8*)&m_tStyleInfo, (void*)&tStyleInfo, sizeof(TVmpStyleCfgInfo) );
    
    TMulPicColor tBGDColor;
    GetRGBColor( tStyleInfo.GetBackgroundColor(), tBGDColor.RColor,
                                                 tBGDColor.GColor,
                                                 tBGDColor.BColor );
    
    u32 dwChairColor    = tStyleInfo.GetChairFrameColor();
    u32 dwSpeakerColor  = tStyleInfo.GetSpeakerFrameColor();
    u32 dwAudienceColor = tStyleInfo.GetFrameColor();

    u8 byRColorChair = 0;
    u8 byGColorChair = 0;
    u8 byBColorChair = 0;

    u8 byRColorAudience = 0;
    u8 byGColorAudience = 0;
    u8 byBColorAudience = 0;

    u8 byRColorSpeaker = 0;
    u8 byGColorSpeaker = 0;
    u8 byBColorSpeaker = 0;

    GetRGBColor( dwChairColor,    byRColorChair,    byGColorChair,    byBColorChair );
    GetRGBColor( dwSpeakerColor,  byRColorSpeaker,  byGColorSpeaker,  byBColorSpeaker );
    GetRGBColor( dwAudienceColor, byRColorAudience, byGColorAudience, byBColorAudience );

    TMulPicColor atSlidelineColor[/*MAXNUM_MPUSVMP_MEMBER*/MULPIC_MAX_CHNS];
    memset( atSlidelineColor, 0, sizeof(atSlidelineColor) );
	
	s32 nRet = 0;
    u8 byLoop;
    for ( byLoop = 0; byLoop < MULPIC_MAX_CHNS; byLoop++ )
    {

		
        if ( byLoop >= 20 
             || m_tParam[0].m_atMtMember[byLoop].IsNull()
           )
        {
            continue;
        }
		
        if ( MT_STATUS_AUDIENCE == m_tParam[0].m_atMtMember[byLoop].GetMemStatus() )
        {
            atSlidelineColor[byLoop].RColor = byRColorAudience;
            atSlidelineColor[byLoop].GColor = byGColorAudience;
            atSlidelineColor[byLoop].BColor = byBColorAudience;
			
            mpulog( MPU_INFO, "The channel: %d, The Audience Color Value: %0x\n", byLoop, dwAudienceColor);
        }

        else if ( MT_STATUS_SPEAKER == m_tParam[0].m_atMtMember[byLoop].GetMemStatus() )
        {
            atSlidelineColor[byLoop].RColor = byRColorSpeaker;
            atSlidelineColor[byLoop].GColor = byGColorSpeaker;
            atSlidelineColor[byLoop].BColor = byBColorSpeaker;
            mpulog( MPU_INFO, "The channel: %d, The Speaker  Color Value: %0x\n", byLoop, dwSpeakerColor);
        }
        else if ( MT_STATUS_CHAIRMAN == m_tParam[0].m_atMtMember[byLoop].GetMemStatus() )
        {
            atSlidelineColor[byLoop].RColor = byRColorChair;
            atSlidelineColor[byLoop].GColor = byGColorChair;
            atSlidelineColor[byLoop].BColor = byBColorChair;
            mpulog( MPU_INFO, "The channel: %d, The Chairman Color Value: %0x\n", byLoop, dwChairColor);
        }

        if (m_tStyleInfo.GetIsRimEnabled())
        {
            atSlidelineColor[byLoop].byDefualt = 1;
        }
		else
		{
			atSlidelineColor[byLoop].byDefualt = 0;
		}

		OspPrintf(TRUE,FALSE,"[SetVipChnl]--%d\n",atSlidelineColor[byLoop].byDefualt);
		nRet = m_cHardMulPic.SetVipChnl(atSlidelineColor[byLoop], byLoop);
		if ( HARD_MULPIC_OK != nRet )
		{
			mpulog( MPU_CRIT, " Fail to call the SetVipChnl. (ProcSetVmpAttachStyle)\n" );
			return FALSE;
		}
    }
    mpulog( MPU_INFO, "The Background Color Value: 0x%x\n", tStyleInfo.GetBackgroundColor() );
    mpulog( MPU_INFO, "The RGB Color of Chairman : R<%d> G<%d> B<%d> \n", byRColorChair,    byGColorChair,    byBColorChair );
    mpulog( MPU_INFO, "The RGB Color of Speaker  : R<%d> G<%d> B<%d> \n", byRColorSpeaker,  byGColorSpeaker,  byBColorSpeaker );
    mpulog( MPU_INFO, "The RGB Color of Audience : R<%d> G<%d> B<%d> \n", byRColorAudience, byGColorAudience, byBColorAudience );
    mpulog( MPU_INFO, "The RGB Color of Backgroud: R<%d> G<%d> B<%d> \n", tBGDColor.RColor, tBGDColor.GColor, tBGDColor.BColor );
	
   
	//画面合成风格加边框
	if ( m_tStyleInfo.GetIsRimEnabled() )
	{
		nRet = m_cHardMulPic.SetBGDAndSidelineColor( tBGDColor, atSlidelineColor, TRUE );
	}
	else
	{
		nRet = m_cHardMulPic.SetBGDAndSidelineColor( tBGDColor, atSlidelineColor, FALSE );
	}

    
    
	mpulog( MPU_INFO, "Set VMP Scheme:%d with RimEnabled:%d successfully !\n", m_tStyleInfo.GetSchemeId(), m_tStyleInfo.GetIsRimEnabled() );
    
	return (HARD_MULPIC_OK == nRet);
}

/*=============================================================================
  函 数 名： GetRGBColor
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u32 dwColor
             u8& byRColor
             u8& byGColor
             u8& byBColor
  返 回 值： void 
=============================================================================*/
void CMpuSVmpInst::GetRGBColor( u32 dwColor, u8& byRColor, u8& byGColor, u8& byBColor )
{
    byRColor = (u8)dwColor & 0x000000ff;
    byGColor = (u8)( (dwColor & 0x0000ff00) >> 8 );
    byBColor = (u8)( (dwColor & 0x00ff0000) >> 16 );
    return;
}

/*=============================================================================
    函 数 名： VmpGetActivePayload
    功    能： 
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： u8
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    07/10/18    4.0         张宝卿                创建
=============================================================================*/
u8 CMpuSVmpInst::VmpGetActivePayload( u8 byRealPayloadType )
{
    u8 byAPayload = MEDIA_TYPE_NULL;

    switch ( byRealPayloadType )
    {
    case MEDIA_TYPE_PCMA:   byAPayload = ACTIVE_TYPE_PCMA;  break;
    case MEDIA_TYPE_PCMU:   byAPayload = ACTIVE_TYPE_PCMU;  break;
    case MEDIA_TYPE_G721:   byAPayload = ACTIVE_TYPE_G721;  break;
    case MEDIA_TYPE_G722:   byAPayload = ACTIVE_TYPE_G722;  break;
    case MEDIA_TYPE_G7231:  byAPayload = ACTIVE_TYPE_G7231; break;
    case MEDIA_TYPE_G728:   byAPayload = ACTIVE_TYPE_G728;  break;
    case MEDIA_TYPE_G729:   byAPayload = ACTIVE_TYPE_G729;  break;
    case MEDIA_TYPE_G7221:  byAPayload = ACTIVE_TYPE_G7221; break;
    case MEDIA_TYPE_G7221C: byAPayload = ACTIVE_TYPE_G7221C;break;
    case MEDIA_TYPE_ADPCM:  byAPayload = ACTIVE_TYPE_ADPCM; break;
    case MEDIA_TYPE_H261:   byAPayload = ACTIVE_TYPE_H261;  break;
    case MEDIA_TYPE_H262:   byAPayload = ACTIVE_TYPE_H262;  break;
    case MEDIA_TYPE_H263:   byAPayload = ACTIVE_TYPE_H263;  break;
    //case MEDIA_TYPE_ADPCM:  byAPayload = bNewActivePT ? ACTIVE_TYPE_ADPCM : byRealPayloadType;  break;
    //case MEDIA_TYPE_G7221C: byAPayload = bNewActivePT ? ACTIVE_TYPE_G7221C : byRealPayloadType; break;
    default:                byAPayload = byRealPayloadType; break;
    }        

    return byAPayload;
}

/*====================================================================
	函数名  :StartNetRcvSnd
	功能    :开始画面复合器处理函数
	输入    :消息
	输出    :无
	返回值  :无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	09/07/21	v4.6		张宝卿			create
====================================================================*/
BOOL CMpuSVmpInst::StartNetRcvSnd(u8 byNeedPrs)
{
    //s32 nRet = 0;
    u8  byErrNum = 0; 

    mpulog( MPU_INFO, "[StartNetRcvSnd] open chns.%u as Style.%u, m_byEncType.%d\n",
                       m_tParam[0].m_byMemberNum,
                       m_tParam[0].m_byVMPStyle, m_tParam[0].m_byEncType);
   
	//1. 编码发送相关
    u8 byChnNo = 0;
	//u8 byActPt = 0;

	// media encrypt modify
	TMediaEncrypt tMediaEncrypt;
	s32 nKeyLen = 0;
	u8  abyKeyBuf[MAXLEN_KEY] = {0};
	for (byChnNo = 0; byChnNo < MAXNUM_MPUSVMP_MEMBER; byChnNo ++)
	{
		if ( CONF_ENCRYPTMODE_NONE == m_tParam[0].m_tVideoEncrypt[byChnNo].GetEncryptMode() )
		{
			continue;
		}
		m_tParam[0].m_tVideoEncrypt[byChnNo].GetEncryptKey( abyKeyBuf, &nKeyLen );
		tMediaEncrypt.SetEncryptMode(m_tParam[0].m_tVideoEncrypt[byChnNo].GetEncryptMode());
		tMediaEncrypt.SetEncryptKey( abyKeyBuf, nKeyLen );
		break;
	}

    for ( byChnNo = 0; byChnNo < MAXNUM_MPUSVMP_CHANNEL; byChnNo++ )
    {
        InitMediaSnd( byChnNo );
        SetEncryptParam( byChnNo, &tMediaEncrypt, m_tParam[byChnNo].m_byEncType );
        m_cHardMulPic.SetVidDataCallback( byChnNo, VMPCALLBACK, m_pcMediaSnd[byChnNo] );
    }

    //发送重传设置
    SetPrs(byNeedPrs);


    //2. 解码接收相关
	for (byChnNo = 0; byChnNo < MAXNUM_MPUSVMP_MEMBER; byChnNo ++)
	{
        //MediaRcv 对象设置网络参数
        SetDecryptParam( byChnNo, &tMediaEncrypt/*, &m_tParam[0].m_tDoublePayload[byChnNo]*/ );
		SetMediaRcvNetParam( byChnNo);
	}

    u8 byMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle(m_tParam[0].m_byVMPStyle);

    for ( byChnNo = 0; byChnNo < byMaxChnNum/*m_tParam[0].m_byMemberNum*/; byChnNo++ )
    {
        if ( !m_tParam[0].m_atMtMember[byChnNo].IsNull() )
        { 
            //设置对应通道的解码参数
            StartMediaRcv(byChnNo);
        }
        else
        {
			m_pcMediaRcv[byChnNo]->StopRcv();
			mpulog( MPU_INFO, "%d StopRcv!\n", byChnNo);
			printf("%d StopRcv!\n", byChnNo);
        }
    }

    return ( 0 == byErrNum );
}

/*====================================================================
	函数名  :ChangeChnAndNetRcv
	功能    :切变net收发 和 通道
	输入    :消息
	输出    :无
	返回值  :无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	09/07/21	v4.6		张宝卿			create
====================================================================*/
BOOL CMpuSVmpInst::ChangeChnAndNetRcv(/*u8 byNeedPrs,*/ BOOL32 bStyleChged)
{
    s32 nRet = 0;
//    s32 nErrNum = 0;

    u8 byChnNo = 0;

    //1、切变编码参数（统一由Start处理）
    /*
    u8 byActPt = 0;

    for ( byChnNo = 0; byChnNo < MAXNUM_MPUSVMP_CHANNEL; byChnNo++ )
    {
        if(m_tParam[0].m_atMtMember[byChnNo] == m_tParamPrevious[0].m_atMtMember[byChnNo])
        {
            mpulog(MPU_INFO, "[SetPrs] chn.%d eq to previous one, check next directly!\n", byChnNo);
            continue;
	    }
        if( m_tParam[byChnNo].m_byEncType == MEDIA_TYPE_H263
            || m_tParam[byChnNo].m_byEncType == MEDIA_TYPE_MP4)
        {
            byActPt = 0;
        }
        else
        {
            byActPt = VmpGetActivePayload(m_tParam[byChnNo].m_byEncType);
        }
        SetEncryptParam( byChnNo, &m_tParam[byChnNo].m_tVideoEncrypt[0], byActPt );
    }*/
    
    //2、整理Prs（统一由Start处理）
    //SetPrs(byNeedPrs);
    
    //3、整理通道，重新开始接收
    u8 byMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle(m_tParam[0].m_byVMPStyle);
    for( byChnNo = 0; byChnNo < byMaxChnNum; byChnNo++ )
    {
        if (m_tParam[0].m_atMtMember[byChnNo].IsNull())
        {
            nRet = m_cHardMulPic.RemoveChannel( byChnNo ); 
            mpulog(MPU_INFO, "[ChangeVMPStyle] Remove channel.%d, ret.%d(correct is %u)\n", byChnNo, nRet, HARD_MULPIC_OK);
            
            m_bAddVmpChannel[byChnNo] = FALSE;
        }
        else
        {
            // zbq[05/14/2009] 非风格切变，对于已经存在的通道不再执行本操作
            if (//!bStyleChged
				// [pengjie 2010/2/5] 如果原来某个通道有终端的话，不用再Addchnl
                //&& m_tParam[0].m_atMtMember[byChnNo] == m_tParamPrevious[0].m_atMtMember[byChnNo]
				!m_tParamPrevious[0].m_atMtMember[byChnNo].IsNull()
                && m_tParamPrevious[0].m_tDoublePayload[byChnNo].GetRealPayLoad() != 0	// xliang [5/18/2009] 适应模板启自动画面合成的情况
                )
            {
				// [pengjie 2010/2/9] 设置对应通道的解码参数
				if( !(m_tParam[0].m_atMtMember[byChnNo] == m_tParamPrevious[0].m_atMtMember[byChnNo]) || bStyleChged )
				{	
					mpulog(MPU_INFO, "[ChangeChnAndNetRcv] chn.%d, StartMediaRcv because of need StyleChged or ChnnlMember change!\n", byChnNo);
					StartMediaRcv(byChnNo);
				}
				// End 

                mpulog(MPU_INFO, "[ChangeVMPStyle] chn.%d eq to previous one, check next directly!\n", byChnNo);
                continue;
            }
            
            // 添加通道
            nRet = m_cHardMulPic.AddChannel( byChnNo );
            if ( HARD_MULPIC_OK != nRet )
            {
//                nErrNum++;
            }
            else
            {        
                m_bAddVmpChannel[byChnNo] = TRUE;
            }
            mpulog( MPU_INFO, "[ChangeVMPStyle] AddChannel.%u, ret.%u(correct is %u)\n", byChnNo, nRet, HARD_MULPIC_OK );
            
            //设置对应通道的解码参数
            StartMediaRcv(byChnNo);
        }
    }

    return TRUE;
}

/*====================================================================
	函数名  :StopNetRcvSnd
	功能    :停止Net收发
	输入    :消息
	输出    :无
	返回值  :无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	09/07/21	v4.6		张宝卿			create
====================================================================*/
BOOL CMpuSVmpInst::StopNetRcvSnd()
{
    u8 byChnNo = 0;
    
    for ( ; byChnNo < MAXNUM_MPUSVMP_MEMBER; byChnNo++ )
    {
        //清空动态载荷和加密
        TMediaEncrypt tVideoEncrypt;
        memset( &tVideoEncrypt, 0, sizeof(TMediaEncrypt) );
        TDoublePayload tDoublePayload;
        memset( &tDoublePayload, 0, sizeof(TDoublePayload) );
        SetDecryptParam( byChnNo, &tVideoEncrypt/*, &tDoublePayload*/ );
        
        //停止接收
        m_pcMediaRcv[byChnNo]->StopRcv();
    }
    
    //清空解码密钥
    for ( byChnNo = 0; byChnNo < MAXNUM_MPUSVMP_CHANNEL; byChnNo++ )
    {
        TMediaEncrypt tVideoEncrypt;
        memset( &tVideoEncrypt, 0, sizeof(TMediaEncrypt) );
        SetEncryptParam( byChnNo, &tVideoEncrypt, 0 );
		m_bUpdateMediaEncrpyt[byChnNo] = FALSE;
    }

    //清空PRS
    SetPrs(0);

    return TRUE;
}

/*=============================================================================
  函 数 名： SetEncryptParam
  功    能： 设置加密参数（开始合成时设置）
  算法实现： 
  全局变量： 
  参    数： u8 byChnNo        ：通道号
             TMediaEncrypt     ：加密参数
             u8 byDoublePayload：编码载荷
  返 回 值： void 
=============================================================================*/
void CMpuSVmpInst::SetEncryptParam( u8 byChnNo, TMediaEncrypt * ptVideoEncrypt, u8 byRealPT )
{
    if( NULL == ptVideoEncrypt )
	{
		return;
	}

    u8 byDoublePayload = VmpGetActivePayload(byRealPT);

    u8  abyKeyBuf[MAXLEN_KEY];
	memset( abyKeyBuf, 0, MAXLEN_KEY );
    s32 nKeyLen = 0;

    u8 byEncryptMode = ptVideoEncrypt->GetEncryptMode();
    if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
    {
        m_pcMediaSnd[byChnNo]->SetEncryptKey( NULL, 0, 0 );

        if (MEDIA_TYPE_H264 == byRealPT)
        {
            m_pcMediaSnd[byChnNo]->SetActivePT(byDoublePayload);
        }
        else
        {
            m_pcMediaSnd[byChnNo]->SetActivePT(0);
        }

		mpulog( MPU_INFO, "[SetEncryptParam]OutChannel.%u: no encrypt, so set key to NULL, set ActivePt.%u\n",
			byChnNo, byDoublePayload);
 
    }
    else if ( CONF_ENCRYPTMODE_DES == byEncryptMode || CONF_ENCRYPTMODE_AES == byEncryptMode ) // 加密情况下
    {
        if ( CONF_ENCRYPTMODE_DES == byEncryptMode ) // 上下层宏定义转换
        {
            byEncryptMode = DES_ENCRYPT_MODE;
        }
        else
        {
            byEncryptMode = AES_ENCRYPT_MODE;
        }
        ptVideoEncrypt->GetEncryptKey( abyKeyBuf, &nKeyLen );

        m_pcMediaSnd[byChnNo]->SetEncryptKey( (s8*)abyKeyBuf, (u16)nKeyLen, byEncryptMode );
        m_pcMediaSnd[byChnNo]->SetActivePT( byDoublePayload );

        mpulog( MPU_INFO, "Mode: %u KeyLen: %u PT: %u \n",
                ptVideoEncrypt->GetEncryptMode(), 
                nKeyLen, 
                byDoublePayload );
    }
    return;
}

/*=============================================================================
  函 数 名： InitMediaSnd
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 byChnNo        ：通道号
  返 回 值： void 
=============================================================================*/
void CMpuSVmpInst::InitMediaSnd( u8 byChnNo )
{
    u8 byFrameRate = m_tMpuVmpCfg.m_tVideoEncParam[byChnNo].m_byFrameRate;
    u8 byMediaType = m_tMpuVmpCfg.m_tVideoEncParam[byChnNo].m_byEncType;
    u32 dwNetBand  = m_tMpuVmpCfg.m_adwMaxSendBand[byChnNo];

    // 适应MCU的群组优化调整，作参数保护
    if ( byFrameRate > 60 )
    {
        byFrameRate = 30;
    }

    if ( dwNetBand * 1024 > 8000000 )
    {
        if ( m_tParam[0].GetBitRate() <= 8128 )
        {
			dwNetBand = m_tMpuVmpCfg.m_tVideoEncParam[byChnNo].m_wBitRate * 1024;
        }
        else
        {
            dwNetBand = 0;
        }
    }
    else
    {
        dwNetBand = dwNetBand * 1024;
    }
    mpulog( MPU_INFO, "[InitMediaSnd]m_cMediaSnd[%d]: dwNetBand = %d\n",byChnNo, dwNetBand);

    if ( NULL == m_pcMediaSnd[byChnNo] )
    {
        m_pcMediaSnd[byChnNo] = new(CKdvMediaSnd); 
        if ( NULL == m_pcMediaSnd[byChnNo] )
        {
			delete m_pcMediaSnd[byChnNo];  //  pengjie[8/7/2009]      
            m_pcMediaSnd[byChnNo] = NULL;
            mpulog( MPU_CRIT, " new(CKdvMediaSnd) fail!\n" );
            return;
        }
    }
    
    s32 wRet = m_pcMediaSnd[byChnNo]->Create( MAX_VIDEO_FRAME_SIZE,
		    	                      dwNetBand,
			                          byFrameRate,
			    				      byMediaType );

    if ( MEDIANET_NO_ERROR != wRet )
    {
        mpulog( MPU_CRIT, "[InitMediaSnd] m_cMediaSnd[%d].Create fail, Error code is:%d, NetBand.%d, FrmRate.%d, MediaType.%d\n", byChnNo, wRet, dwNetBand, byFrameRate, byMediaType );
    	return;
    }
    else
    {
        mpulog( MPU_INFO, "[InitMediaSnd] m_cMediaSnd[%d].Create as NetBand.%d, FrmRate.%d, MediaType.%d\n", byChnNo, dwNetBand, byFrameRate, byMediaType );
    }
	
	BOOL32 bHDFlag = TRUE;
	if(byChnNo == 2)	//h264 CIF, other 不置HD Flag; 其他h264的4cif，720，1080均置HD Flag
	{
		bHDFlag = FALSE;
	}
	else	
	{
		bHDFlag = TRUE;
	}
	m_pcMediaSnd[byChnNo]->SetHDFlag( bHDFlag );
	mpulog( MPU_INFO, "[InitMediaSnd] m_cMediaSnd[%d].SetHDFlag: %d\n", byChnNo, bHDFlag );


    TNetSndParam tNetSndPar;
    memset( &tNetSndPar, 0x00, sizeof(tNetSndPar) );

    tNetSndPar.m_byNum  = 1;
    tNetSndPar.m_tLocalNet.m_wRTPPort       = PORT_SNDBIND + byChnNo * 2;
    tNetSndPar.m_tLocalNet.m_wRTCPPort      = m_wMcuRcvStartPort + byChnNo * PORTSPAN + 1;/*PORT_SNDBIND + byChnNo * 2 + 1*/
    tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr  = htonl(m_dwMcuRcvIp);
    tNetSndPar.m_tRemoteNet[0].m_wRTPPort   = m_wMcuRcvStartPort + byChnNo * PORTSPAN;
    tNetSndPar.m_tRemoteNet[0].m_dwRTCPAddr = htonl(m_dwMcuRcvIp);
    tNetSndPar.m_tRemoteNet[0].m_wRTCPPort  = m_wMcuRcvStartPort + byChnNo * PORTSPAN + 1;

    mpulog( MPU_INFO, "[InitMediaSnd] Snd[%d]: RTPAddr(%s)\n", byChnNo, StrOfIP(m_dwMcuRcvIp));

    wRet = m_pcMediaSnd[byChnNo]->SetNetSndParam( tNetSndPar );
    if ( MEDIANET_NO_ERROR != wRet )
	{
    	mpulog( MPU_CRIT, "[InitMediaSnd] m_cMediaSnd[%d].SetNetSndParam fail, Error code is: %d \n", byChnNo, wRet );
       	return;
	}

    return;
}

/*=============================================================================
  函 数 名： SetMediaRcvNetParam
  功    能： 设置Medianet Receive对象网络参数
  算法实现： 
  全局变量： 
  参    数： u8 byChnNo        ：通道号
  返 回 值： BOOL 
=============================================================================*/
BOOL CMpuSVmpInst::SetMediaRcvNetParam( u8 byChnNo)
{
	TLocalNetParam tlocalNetParm;
    memset(&tlocalNetParm, 0, sizeof(TLocalNetParam));
    tlocalNetParm.m_tLocalNet.m_wRTPPort  = m_tCfg.wRcvStartPort + byChnNo * PORTSPAN;
    tlocalNetParm.m_tLocalNet.m_wRTCPPort = m_tCfg.wRcvStartPort + 1 + byChnNo * PORTSPAN;
    tlocalNetParm.m_dwRtcpBackAddr        = htonl(m_dwMcuRcvIp);
    //tlocalNetParm.m_wRtcpBackPort         = m_tCfg.wRcvStartPort + 1 + byChnNo * PORTSPAN;
	// xliang [5/6/2009] 分配原则 ChnNo		Port(该MPU绑定的MP板上rtcp端口)
	//							  0~7	===> 2~9 (即39002,...,39009)
	//							  8~15	===> 12~19 (即39012,...,39019)
	//							  16~20	===> 22~25
	u16 wRtcpBackPort;
	if (byChnNo >= 16)
	{
		wRtcpBackPort = m_wMcuRcvStartPort + 6 + byChnNo;
	}
	else if (byChnNo >= 8)
	{
		wRtcpBackPort = m_wMcuRcvStartPort + 4 + byChnNo;
	}
	else
	{
		wRtcpBackPort = m_wMcuRcvStartPort + 2 + byChnNo;
    }
	
	tlocalNetParm.m_wRtcpBackPort = wRtcpBackPort;//远端的rtcp port
	
    mpulog( MPU_INFO, "m_cMediaRcv[%d]:\n", byChnNo );
    mpulog( MPU_INFO, "\t tlocalNetParm.m_tLocalNet.m_wRTPPort[%d]:\n", tlocalNetParm.m_tLocalNet.m_wRTPPort );
    mpulog( MPU_INFO, "\t tlocalNetParm.m_tLocalNet.m_wRTCPPort[%d]:\n", tlocalNetParm.m_tLocalNet.m_wRTCPPort );
    mpulog( MPU_INFO, "\t tlocalNetParm.m_dwRtcpBackAddr[%s]:\n", StrOfIP(tlocalNetParm.m_dwRtcpBackAddr) );
    mpulog( MPU_INFO, "\t tlocalNetParm.m_wRtcpBackPort[%d]:\n", tlocalNetParm.m_wRtcpBackPort );
	
	s32 nRet = 0;
    nRet = m_pcMediaRcv[byChnNo]->SetNetRcvLocalParam( tlocalNetParm );
    if( MEDIANET_NO_ERROR !=  nRet )
    {
        mpulog( MPU_CRIT, " m_cMediaRcv[%d].SetNetRcvLocalParam fail, Error code is: %d \n", byChnNo, nRet );        
        return FALSE;
	}
    else
    {
        mpulog( MPU_INFO, "m_cMediaRcv[%d].SetNetRcvLocalParam succeed!\n", byChnNo );
    }
	return TRUE;
}
/*=============================================================================
  函 数 名： InitMediaRcv
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 byChnNo        ：通道号
  返 回 值： BOOL 
=============================================================================*/
BOOL CMpuSVmpInst::InitMediaRcv( u8 byChnNo )
{
    g_cMpuSVmpApp.m_atMpuSVmpRcvCB[byChnNo].m_byChnnlId = byChnNo;
    g_cMpuSVmpApp.m_atMpuSVmpRcvCB[byChnNo].m_pThis     = this;

    s32 wRet = m_pcMediaRcv[byChnNo]->Create( MAX_VIDEO_FRAME_SIZE,
			                                  CBMpuSVmpRecvFrame,   
				    		                  (u32)&g_cMpuSVmpApp.m_atMpuSVmpRcvCB[byChnNo] );

    if( MEDIANET_NO_ERROR !=  wRet )
	{
		mpulog( MPU_CRIT, " m_cMediaRcv[%d].Create fail, Error code is: %d \n", byChnNo, wRet );
		return FALSE;
	}
    else
    {
        mpulog( MPU_INFO, "m_cMediaRcv[%d].Create succeed!\n", byChnNo );
    }

    m_pcMediaRcv[byChnNo]->SetHDFlag( TRUE );
/*
    // 这个端口可能有误 
    TLocalNetParam tlocalNetParm;
    memset(&tlocalNetParm, 0, sizeof(TLocalNetParam));
    tlocalNetParm.m_tLocalNet.m_wRTPPort  = m_tCfg.wRcvStartPort + byChnNo * PORTSPAN;
    tlocalNetParm.m_tLocalNet.m_wRTCPPort = m_tCfg.wRcvStartPort + 1 + byChnNo * PORTSPAN;
    tlocalNetParm.m_dwRtcpBackAddr        = m_dwMcuRcvIp;
    //tlocalNetParm.m_wRtcpBackPort         = m_tCfg.wRcvStartPort + 1 + byChnNo * PORTSPAN;
	// xliang [5/6/2009] 分配原则 ChnNo		Port(该MPU绑定的MP板上rtcp端口)
	//							  0~7	===> 2~9 (即39002,...,39009)
	//							  8~15	===> 12~19 (即39012,...,39019)
	//							  16~20	===> 22~25
	u16 wRtcpBackPort;
	if (byChnNo >= 16)
	{
		wRtcpBackPort = m_wMcuRcvStartPort + 6 + byChnNo;
	}
	if (byChnNo >= 8)
	{
		wRtcpBackPort = m_wMcuRcvStartPort + 4 + byChnNo;
	}
	else
	{
		wRtcpBackPort = m_wMcuRcvStartPort + 2 + byChnNo;
    }

	tlocalNetParm.m_wRtcpBackPort = wRtcpBackPort;//远端的rtcp port

    mpulog( MPU_INFO, "m_cMediaRcv[%d]:\n", byChnNo );
    mpulog( MPU_INFO, "\t tlocalNetParm.m_tLocalNet.m_wRTPPort[%d]:\n", tlocalNetParm.m_tLocalNet.m_wRTPPort );
    mpulog( MPU_INFO, "\t tlocalNetParm.m_tLocalNet.m_wRTCPPort[%d]:\n", tlocalNetParm.m_tLocalNet.m_wRTCPPort );
    mpulog( MPU_INFO, "\t tlocalNetParm.m_dwRtcpBackAddr[%s]:\n", StrOfIP(tlocalNetParm.m_dwRtcpBackAddr) );
    mpulog( MPU_INFO, "\t tlocalNetParm.m_wRtcpBackPort[%d]:\n", tlocalNetParm.m_wRtcpBackPort );
        
    wRet = m_pcMediaRcv[byChnNo]->SetNetRcvLocalParam( tlocalNetParm );
    if( MEDIANET_NO_ERROR !=  wRet )
    {
        mpulog( MPU_CRIT, " m_cMediaRcv[%d].SetNetRcvLocalParam fail, Error code is: %d \n", byChnNo, wRet );        
        return FALSE;
	}
    else
    {
        mpulog( MPU_INFO, "m_cMediaRcv[%d].SetNetRcvLocalParam succeed!\n", byChnNo );
    }
*/
    return TRUE;
}
/*=============================================================================
  函 数 名： StartMediaRcv
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 byChnNo        ：通道号
  返 回 值： void 
=============================================================================*/
void CMpuSVmpInst::StartMediaRcv(u8 byChnNo)
{
	if (byChnNo >= MAXNUM_MPUSVMP_MEMBER)
	{
		OspPrintf(TRUE, FALSE, "[StartMediaRcv] wrong input param byChnNo = %d\n", byChnNo);
		printf("[StartMediaRcv] wrong input param byChnNo = %d\n", byChnNo);
        return;
	}

	if (NULL == m_pcMediaRcv[byChnNo])
	{
		OspPrintf(TRUE, FALSE, "[StartMediaRcv] m_pcMediaRcv[%d] still null\n", byChnNo);
		printf("[StartMediaRcv] m_pcMediaRcv[%d] still null\n", byChnNo);
		return;
	}

    //接收的HDFlag依赖于开启的设置，此处不重置
    /*
	// xliang [5/31/2009] set HD flag
	if (MEDIA_TYPE_H263 == m_tParam[0].m_tDoublePayload[byChnNo].GetRealPayLoad()
		|| MEDIA_TYPE_MP4 == m_tParam[0].m_tDoublePayload[byChnNo].GetRealPayLoad() )
	{
		m_pcMediaRcv[byChnNo]->SetHDFlag( FALSE );
	}
	else
	{
		m_pcMediaRcv[byChnNo]->SetHDFlag( TRUE );
	}*/
    m_pcMediaRcv[byChnNo]->StartRcv();

    return;
}

/*=============================================================================
  函 数 名： SetDecryptParam
  功    能： 设置解码参数
  算法实现： 
  全局变量： 
  参    数： u8 byChnNo
             TMediaEncrypt *  ptVideoEncrypt
             TDoublePayload * ptDoublePayload
             BOOL32 bSetDPOnly: 是否只设置动态载荷
  返 回 值： void 
=============================================================================*/
void CMpuSVmpInst::SetDecryptParam(u8 byChnNo,
                                   TMediaEncrypt *  ptVideoEncrypt/*,TDoublePayload * ptDoublePayload*/)
{
    u8 byEncryptMode;
    u8 abyKeyBuf[MAXLEN_KEY];
    s32 byKenLen = 0;
    memset(abyKeyBuf, 0, MAXLEN_KEY);

    byEncryptMode = ptVideoEncrypt->GetEncryptMode();
    if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
    {
         m_pcMediaRcv[byChnNo]->SetDecryptKey( NULL, 0, 0 );

        //u8 byRealPayload = ptDoublePayload->GetRealPayLoad();
        //u8 byActivePayload = ptDoublePayload->GetActivePayload();
        //m_pcMediaRcv[byChnNo]->SetActivePT( byActivePayload, byRealPayload );
     
         //2009-7-22 为防止后续动态载荷切换引起meidanet紊乱，统一设置载荷为 MEDIA_TYPE_H264
        u16 wRet = m_pcMediaRcv[byChnNo]->SetActivePT( MEDIA_TYPE_H264, MEDIA_TYPE_H264 );
		mpulog( MPU_INFO, "Set key to NULL, PT(real): %u, SetActivePt result is:%d \n", MEDIA_TYPE_H264, wRet );
    }
    else if ( CONF_ENCRYPTMODE_DES == byEncryptMode || CONF_ENCRYPTMODE_AES == byEncryptMode )
    {
        if ( CONF_ENCRYPTMODE_DES == byEncryptMode )
        {
            byEncryptMode = DES_ENCRYPT_MODE;
        }
        else
        {
            byEncryptMode = AES_ENCRYPT_MODE;
        }

        //u8 byRealPayload = ptDoublePayload->GetRealPayLoad();
        //u8 byActivePayload = ptDoublePayload->GetActivePayload();
        //m_pcMediaRcv[byChnNo]->SetActivePT( byActivePayload, byRealPayload );

        //2009-7-22 为防止后续动态载荷切换引起meidanet紊乱，统一设置载荷为 MEDIA_TYPE_H264
        m_pcMediaRcv[byChnNo]->SetActivePT( MEDIA_TYPE_H264, MEDIA_TYPE_H264 );

        ptVideoEncrypt->GetEncryptKey(abyKeyBuf, &byKenLen);
        m_pcMediaRcv[byChnNo]->SetDecryptKey( (s8*)abyKeyBuf, (u16)byKenLen, byEncryptMode );

        mpulog( MPU_INFO, "Mode: %u KeyLen: %u PT: %u\n",
                ptVideoEncrypt->GetEncryptMode(), byKenLen, MEDIA_TYPE_H264 );
    }
    return;
}

/*=============================================================================
 函 数 名： SetPrs
 功    能： 设置Prs
 算法实现： setPrs在开启VMP时只调一次，覆盖所有的通道
 全局变量： 
 参    数： 
 返 回 值： 
 ----------------------------------------------------------------------
 修改记录    ： 
 日  期		版本		修改人		走读人    修改内容
 2007/03/12  4.0		顾振华                  创建
 2009/07/21  4.6        张宝卿                  重置所有的通道
=============================================================================*/
void CMpuSVmpInst::SetPrs( u8 byNeedPrs )
{
	TRSParam tNetRSParam;
    if ( byNeedPrs )		//是否需要包重传？ 1: 重传, 0: 不重传
	{		
		mpulog( MPU_INFO, "[SetPrs] needed Prs!\n" );
        tNetRSParam.m_wFirstTimeSpan  = m_tPrsTimeSpan.m_wFirstTimeSpan;
		tNetRSParam.m_wSecondTimeSpan = m_tPrsTimeSpan.m_wSecondTimeSpan;
		tNetRSParam.m_wThirdTimeSpan  = m_tPrsTimeSpan.m_wThirdTimeSpan;
		tNetRSParam.m_wRejectTimeSpan = m_tPrsTimeSpan.m_wRejectTimeSpan;
		
        s32	wRet;
        u8 byIndex = 0;
        for ( byIndex = 0; byIndex < MAXNUM_MPUSVMP_CHANNEL; byIndex++ )
        {
            wRet = m_pcMediaSnd[byIndex]->ResetRSFlag( 2000, TRUE );
            if( MEDIANET_NO_ERROR != wRet )
            {
	            mpulog( MPU_CRIT, "m_cMediaSnd[%d].ResetRSFlag fail,Error code is:%d\n", byIndex, wRet );
	            continue;
            }
        }
        for ( byIndex = 0; byIndex < MAXNUM_MPUSVMP_MEMBER; byIndex++ )
        {
            /*
			if(m_tParam[0].m_atMtMember[byIndex] == m_tParamPrevious[0].m_atMtMember[byIndex])
			{
				mpulog(MPU_INFO, "[SetPrs] chn.%d eq to previous one, check next directly!\n", byIndex);
				continue;
			}*/
            GetRSParamByPos(byIndex, tNetRSParam);
            wRet = m_pcMediaRcv[byIndex]->ResetRSFlag( tNetRSParam, TRUE );
	        if( MEDIANET_NO_ERROR != wRet )
	        {
		        mpulog( MPU_CRIT, "m_cMediaRcv[%d].ResetRSFlag fail,Error code is:%d\n", byIndex, wRet );
		        return;
	        }
            else
            {
                mpulog( MPU_INFO, "m_cMediaRcv[%d].ResetRSParam<F:%d, S.%d, T.%d, R.%d>\n",
									byIndex,
                                    tNetRSParam.m_wFirstTimeSpan,
                                    tNetRSParam.m_wSecondTimeSpan,
                                    tNetRSParam.m_wThirdTimeSpan,
                                    tNetRSParam.m_wRejectTimeSpan);
            }
        }
	}
	else
	{
		mpulog( MPU_INFO, "[SetPrs] not need Prs!\n" );
		return;
	}
}

/*=============================================================================
函 数 名： SetSmoothSendRule
功    能： 设置平滑发送规则
算法实现： 
全局变量： SSErrno
参    数： 均为主机序
返 回 值： void
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/03/12  4.0			顾振华                  创建
=============================================================================*/
//void CMpuSVmpInst::SetSmoothSendRule( u32 dwDestIp, u16 wPort, u32 dwRate )
//{
//	//还原打折前码率，平滑发送参数不能取编码码率
//	dwRate = m_tMpuVmpCfg.GetOrigRate(dwRate);
//    
//	dwRate = dwRate >> 3;   
//    u32 dwPeak = dwRate + dwRate / 5; // add 20%
//    mpulog( MPU_CRIT, "[SetSmoothSendRule] Setting ssrule for 0x%x:%d, rate/peak=%u/%u KByte.\n",
//		dwDestIp, wPort, dwRate, dwPeak );
//
//#ifdef _LINUX_
//     s32 nRet = SetSSRule(dwDestIp, wPort, dwRate, dwPeak, 8); // default 2 seconds
//     if ( 0 == nRet )
//     {
//         mpulog(MPU_CRIT, "[SetSmoothSendRule] Set rule failed. SSErrno=%d\n", SSErrno );
//    }
//#endif
//}

/*lint -save -e715*/
/*=============================================================================
函 数 名： SetSmoothSendRule
功    能： 设置平滑发送规则
算法实现： 
全局变量： 
参    数： BOOL32 bSetFlag TRUE:SetSSRule(设置);	FALSE:UnsetSSRule(清除)
返 回 值： void
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010/12/31   			朱胜泽                  创建
=============================================================================*/
void CMpuSVmpInst::SetSmoothSendRule(BOOL32 bSetFlag)
{
#ifdef _LINUX_    
	s32 nRet = 0;
	
	if (bSetFlag)
	{//设置平滑规则

		for (u8 byChannlNO = 0; byChannlNO < MAXNUM_MPUSVMP_CHANNEL; byChannlNO++)
		{
			//还原打折前码率，平滑发送参数不能取编码码率
			u32 dwRate = m_tParam[byChannlNO].GetBitRate();	
			dwRate = m_tMpuVmpCfg.GetOrigRate(dwRate);    
			dwRate = dwRate >> 3;
			//峰值为120%
			u32 dwPeak = dwRate + dwRate / 5; 
			mpulog( MPU_CRIT, "[SetSmoothSendRule] Setting ssrule for 0x%x:%d, rate/peak=%u/%u KByte.\n",
				m_dwMcuRcvIp, m_wMcuRcvStartPort + byChannlNO * PORTSPAN, dwRate, dwPeak );
			nRet = SetSSRule(m_dwMcuRcvIp, m_wMcuRcvStartPort + byChannlNO * PORTSPAN, dwRate, dwPeak, 8);	

			if ( 0 == nRet )
			{
				mpulog(MPU_CRIT, "[SetSmoothSendRule] Set rule failed. SSErrno=%d. \n", SSErrno);
			}
		}		
	}
	else
	{//清除平滑规则
		for (u8 byChannlNO = 0; byChannlNO < MAXNUM_MPUSVMP_CHANNEL; byChannlNO++)
		{
			mpulog( MPU_CRIT, "[SetSmoothSendRule] Unsetting ssrule for 0x%x:%d.\n",
				m_dwMcuRcvIp, m_wMcuRcvStartPort + byChannlNO * PORTSPAN );
			nRet = UnsetSSRule( m_dwMcuRcvIp, m_wMcuRcvStartPort + byChannlNO * PORTSPAN );	
			if ( 0 == nRet )
			{
				mpulog(MPU_CRIT, "[SetSmoothSendRule] Unset rule failed. SSErrno=%d. \n", SSErrno);
            }
		}		
	}
	

#endif
	return;
}
/*lint -restore*/

/*=============================================================================
函 数 名： ClearSmoothSendRule
功    能： 取消设置平滑发送规则
算法实现： 
全局变量： SSErrno
参    数： 均为主机序
返 回 值： void
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/03/12  4.0			顾振华                  创建
=============================================================================*/
//void CMpuSVmpInst::ClearSmoothSendRule( u32 dwDestIp, u16 wPort )
//{
//    mpulog( MPU_CRIT, "[ClearSmoothSendRule] Unsetting ssrule for 0x%x:%d.\n",
//            dwDestIp, wPort );
//
//#ifdef _LINUX_
//     s32 nRet = UnsetSSRule( dwDestIp, wPort );
//     if ( 0 == nRet )
//     {
//         mpulog(MPU_CRIT, "[ClearSmoothSendRule] Unset rule failed. SSErrno=%d\n", SSErrno );
//    }
//#endif
//}


void CMpuSVmpInst::ProcSetFrameRateCmd( CMessage * const pcMsg )
{
	u8 byFrameRate = *(pcMsg->content);
	for( u8 byLoop = 0; byLoop < MAXNUM_MPUSVMP_CHANNEL; byLoop ++)
	{
		m_tMpuVmpCfg.m_tVideoEncParam[byLoop].m_byFrameRate = byFrameRate;
	}
	
	s32 nRet = m_cHardMulPic.StartMerge(&m_tMpuVmpCfg.m_tVideoEncParam[0], 
                                                MAXNUM_MPUSVMP_CHANNEL);

	if ( HARD_MULPIC_OK != nRet )
	{
		OspPrintf(TRUE, FALSE, "Set Vmp FrameRate to %d failed!\n", byFrameRate);
	}
	else
	{
		OspPrintf(TRUE, FALSE, "Set Vmp FrameRate to %d successfully!\n", byFrameRate);
	}
}

/*==============================================================================
函数名    :  MsgAddRemoveRcvChnnl
功能      :  增加/删除某接收通道
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010/09/15   4.6           薛亮								create
==============================================================================*/
void CMpuSVmpInst::MsgAddRemoveRcvChnnl(CMessage * const pMsg)
{
	CServMsg cServMsg( pMsg->content, pMsg->length );
	u8 byChnNo = cServMsg.GetChnIndex();
//	u8 byNoUse = *cServMsg.GetMsgBody();
	u8 byAdd = *(cServMsg.GetMsgBody()+1);
	s32 nRet = FALSE;
	if (byAdd == 0)
	{
		if(m_bAddVmpChannel[byChnNo] == FALSE)
		{
			return;
		}
		nRet = m_cHardMulPic.RemoveChannel( byChnNo );
		if ( HARD_MULPIC_OK != nRet )
		{
			mpulog(MPU_INFO, "[MsgAddRemoveRcvChnnl] Remove channel.%d error, ret.%d(correct is %u)\n", byChnNo, nRet, HARD_MULPIC_OK);
			return;
		}
		else
		{
			m_bAddVmpChannel[byChnNo] = FALSE;
			m_tParam[0].m_atMtMember[byChnNo].SetNull();
			m_tParam[0].m_tDoublePayload[byChnNo].Reset();
			//设置边框和颜色
			if ( !SetVmpAttachStyle( m_tStyleInfo ) )
			{
				mpulog(MPU_CRIT,"[MsgAddRemoveRcvChnnl] Failed to set vmp attach style!");
				return;
			}
		}
	}
	else
	{
		if( m_bAddVmpChannel[byChnNo] == TRUE )
		{
			return;
		}
		nRet = m_cHardMulPic.AddChannel( byChnNo );
		if ( HARD_MULPIC_OK != nRet )
		{
			mpulog( MPU_CRIT, "[MsgAddRemoveRcvChnnl] AddChannel.%u error, ret.%u(correct is %u)\n", byChnNo, nRet, HARD_MULPIC_OK );
			return;
		}
		else
		{        
			m_bAddVmpChannel[byChnNo] = TRUE;
			//设置对应通道的解码参数
			StartMediaRcv(byChnNo);
		}
	}
}

void CMpuSVmpInst::ShowVMPDebugInfo()
{
	m_tMpuVmpCfg.Print();
}

void CMpuSVmpInst::Trans2EncParam(u8 byEncChnIdx,CKDVNewVMPParam* ptKdvVmpParam, TVideoEncParam* ptVideEncParam)
{
	if (NULL == ptVideEncParam || NULL == ptKdvVmpParam)
	{
		mpulog(MPU_CRIT,"[Trans2EncParam] error param (null)!");
		return;
	}
	
	// 参数赋值和调整
	
	ptVideEncParam->m_byEncType    = ptKdvVmpParam->m_byEncType;
	
	ptVideEncParam->m_wBitRate     = ptKdvVmpParam->GetBitRate();
	
	
	u8 byFrameRate =  ptKdvVmpParam->GetFrameRate();
	u8 byRes = GetResViaWH(ptKdvVmpParam->GetVideoWidth(), ptKdvVmpParam->GetVideoHeight());
	//查分辨率码率限制信息表，调整分辨率和帧率
		//分辨率码率表是按照打折前的码率来调的
	u16 wOrigBitrate = m_tMpuVmpCfg.GetOrigRate(ptKdvVmpParam->GetBitRate());
	BOOL32 bGetRet = g_tResBrLimitTable.GetRealResFrameRatebyBitrate(ptKdvVmpParam->m_byEncType,byRes,
		byFrameRate,wOrigBitrate);
	if (!bGetRet)
	{
		OspPrintf(TRUE,FALSE,"[MsgStartVidMixProc]ERROR: GetRealResFrameratebyBitrate return FALSE\n");
	}
	u16 wVideoWith = 0;
	u16 wVideoHeight = 0;
	GetWHViaRes(byRes,wVideoWith,wVideoHeight);
	if( VIDEO_FORMAT_4CIF == byRes && MEDIA_TYPE_MP4 == ptKdvVmpParam->m_byEncType )
	{
		wVideoWith = 720;
		wVideoHeight = 576;
	}
	
	//mpu的下层限制，第0路如果是1080或720，第1路不能是cif。
	if ( 1 == byEncChnIdx && 
			( m_tMpuVmpCfg.m_tVideoEncParam[0].m_wVideoWidth == 1920  || 
				m_tMpuVmpCfg.m_tVideoEncParam[0].m_wVideoWidth == 1280  
  			) &&
			wVideoWith == 352
		)
	{
		wVideoWith = (MEDIA_TYPE_MP4 == byRes) ? 720: 704;
		wVideoHeight = 576 ;
	}
				


	ptVideEncParam->m_wVideoHeight = wVideoHeight; 
	ptVideEncParam->m_wVideoWidth  = wVideoWith;
	
	//帧率调整
	ptVideEncParam->m_byFrameRate = 0;
	if( ptKdvVmpParam->GetFrameRate() != 0 )
	{
		mpulog( MPU_INFO, "Set vmp frame rate to %d\n", byFrameRate);
		
		ptVideEncParam->m_byFrameRate = byFrameRate;
	}
	
	
	if ( (ptVideEncParam->m_wVideoWidth == 1920 &&
		ptVideEncParam->m_wVideoHeight == 1088) ||
		(ptVideEncParam->m_wVideoWidth == 1920 &&
		ptVideEncParam->m_wVideoHeight == 1080) )
	{
		ptVideEncParam->m_byMaxKeyFrameInterval = 3000;
	}
	else if ( ptVideEncParam->m_wVideoWidth == 1280 &&
		ptVideEncParam->m_wVideoHeight == 720 )
	{
		ptVideEncParam->m_byMaxKeyFrameInterval = 3000;
	}
	else if ( (ptVideEncParam->m_wVideoWidth == 720 ||
		ptVideEncParam->m_wVideoWidth == 704) &&
		ptVideEncParam->m_wVideoHeight == 576 )
	{
		ptVideEncParam->m_byMaxKeyFrameInterval = 1000;
	}
	else if ( ptVideEncParam->m_wVideoWidth == 352 &&
		ptVideEncParam->m_wVideoHeight == 288 )
	{
		ptVideEncParam->m_byMaxKeyFrameInterval = 500;
	}
	
	// xliang [5/26/2009] 量化参数调整
	if(ptVideEncParam->m_byEncType == MEDIA_TYPE_H264 )
	{
		ptVideEncParam->m_byMaxQuant = 50;
		ptVideEncParam->m_byMinQuant = 15;
	}
	else//h263 ,mp4
	{
		mpulog( MPU_INFO, "set h263/mp4 Quant and MaxKeyFrameInterval!\n");
		ptVideEncParam->m_byMaxQuant = 30;
		ptVideEncParam->m_byMinQuant = 3;
		ptVideEncParam->m_byMaxKeyFrameInterval = 300;
	}

}


/*=============================================================================
  函 数 名： ProcChangeMemAliasCmd
  功    能： 改变成员显示的别名
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2012/06/06  4.7         周翼亮          创建
====================================================================*/
void CMpuSVmpInst::ProcChangeMemAliasCmd( CMessage* const pMsg )
{
	//1、参数合法性校验
	if ( NULL == pMsg ) 
	{
		mpulog( MPU_CRIT,"[ProcChangeMemAliasCmd]Null pointer CMessage\n");
		return;
	}
	if ( CurState() != RUNNING ) 
	{
		mpulog( MPU_CRIT, "[ProcChangeMemAliasCmd] Wrong State:%d !\n", CurState() );
		return;
	}

	CServMsg cServMsg( pMsg->content, pMsg->length );
	u8 byChnNo = cServMsg.GetChnIndex();
	s8 *pchAlias = (s8*)cServMsg.GetMsgBody();

	//看是否有自定义图片对应要设置
	for (u8 byIndex = 0; byIndex < MAXNUM_VMPDEFINE_PIC; byIndex++)
	{
		if(!m_tParam[0].m_atMtMember[byChnNo].IsNull() &&
			strlen(pchAlias) > 0 &&
			0 == strcmp(pchAlias, m_tMpuVmpCfg.GetVmpMemAlias(byIndex)))
		{
			//调用接口
			mpulog( MPU_INFO, "[ProcChangeMemAliasCmd]index.%d -> alias.%s -> pic.%s!\n", 
				byChnNo, 
				m_tMpuVmpCfg.GetVmpMemAlias(byIndex), 
				m_tMpuVmpCfg.GetVmpMemRoute(byIndex));
			
			s32 nReturn = m_cHardMulPic.SetNoStreamBak((u32)NoStrm_PlyBmp_usr, 
				(u8*)m_tMpuVmpCfg.GetVmpMemRoute(byIndex), 
				byChnNo);
			if ( (s32)Codec_Success != nReturn )
			{
				OspPrintf(TRUE, FALSE, "[ProcChangeMemAliasCmd] Index.%d SetNoStreamBak usrbmp failed(ErrCode.%d)!\n", byChnNo, nReturn);
			}
			break;
		}
	}			


}


//END OF FILE

