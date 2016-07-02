/*****************************************************************************
   模块名      : mpulib
   文件名      : mpubas.cpp
   相关文件    : 
   文件实现功能: mpulib bas态分支实现
   作者        : 周文
   版本        : V4.5  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期       版本        修改人      修改内容
   2009/3/14    4.6         张宝卿      注释
******************************************************************************/
#include "mpuinst.h"
//#include "mpustruct.h"
#include "evmpu.h"
#include "mcuver.h"
#include "evmcueqp.h"
#include "baserr.h"
#include "boardagent.h"

/*====================================================================
	函数  : DaemonInstanceEntry
	功能  : Daemon实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
    日  期      版本        修改人      修改内容
    2008/12/04  4.5         周文        创建
====================================================================*/
/*lint -save -e715*/
void CMpuBasInst::DaemonInstanceEntry( CMessage* const pMsg, CApp* pcApp )
{
    switch ( pMsg->event )
    {
    //上电初始化
    case OSP_POWERON:
		DaemonProcPowerOn( pMsg/*, pcApp*/ );
		break;

    // 建链
    case EV_BAS_CONNECT:
        DaemonProcConnectTimeOut( TRUE );
        break;
	// 连接Mcu
    case EV_BAS_CONNECTB:
        DaemonProcConnectTimeOut( FALSE );
        break;

    // 注册消息
    case EV_BAS_REGISTER:  	
        DaemonProcRegisterTimeOut( TRUE );
        break;
	// 注册
    case EV_BAS_REGISTERB:  	
        DaemonProcRegisterTimeOut( FALSE );
        break;

    // MCU 注册应答消息
    case MCU_BAS_REG_ACK:
        DaemonProcMcuRegAck( pMsg/*, pcApp*/ );
        break;

    // MCU拒绝本混音器注册
    case MCU_BAS_REG_NACK:
        DaemonProcMcuRegNack( pMsg );
        break;

    // OSP 断链消息
    case OSP_DISCONNECT:
        DaemonProcOspDisconnect( pMsg/*, pcApp*/ );
        break;

    // 设置Qos值
    case MCU_EQP_SETQOS_CMD:
        //DaemonProcSetQosCmd( pMsg );
        break;

    // 取主备倒换状态
    case EV_BAS_GETMSSTATUS:
    case MCU_EQP_GETMSSTATUS_ACK:
        DeamonProcGetMsStatusRsp(pMsg);
		break;

	case MCU_EQP_MODSENDADDR_CMD:
		DeamonProcModSendIpAddr(pMsg);
		break;

    // 开始适配
    case MCU_HDBAS_STARTADAPT_REQ:
    // 停止适配
    case MCU_HDBAS_STOPADAPT_REQ:
    // 设置适配参数
    case MCU_HDBAS_SETADAPTPARAM_CMD:
    // MCU请求编一个关键帧
    case MCU_BAS_FASTUPDATEPIC_CMD:
        {
            CServMsg cServMsg( pMsg->content, pMsg->length );
            u8 byChnIdx = cServMsg.GetChnIndex();
            u8 byChnId = byChnIdx + 1;
            post( MAKEIID( GetAppID(), byChnId ), pMsg->event, pMsg->content, pMsg->length );
            break;        
        }
	case EV_BAS_SHOWDEBUG:
		{
			DeamonShowBasDebugInfo();
			break;
		}
    default:
        break;
    }
    return;
}
/*lint -restore*/
/*====================================================================
	函数  : InstanceEntry
	功能  : 普通实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
    日  期      版本        修改人      修改内容
    2008/08/04  4.5         顾振华         创建
====================================================================*/
void CMpuBasInst::InstanceEntry( CMessage* const pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    cServMsg.SetEventId(pcMsg->event);
    
    if ( pcMsg->event != EV_BAS_NEEDIFRAME_TIMER &&
         pcMsg->event != EV_BAS_STATUSCHK_TIMER )
    {
        mpulog( MPU_INFO, "[BasInst.%d] Receive msg.%d<%s>\n", GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event) );
    }

    switch ( pcMsg->event )    
    {
    // 初始化通道
    case EV_BAS_INI:
        ProcInitBas();
        break;
    case EV_BAS_QUIT:
        ProcStopBas();        
        break;

	case EV_BAS_SENDCHNSTATUS:
		SendChnNotif();
		break;

    // 开始适配
    case MCU_HDBAS_STARTADAPT_REQ:
        ProcStartAdptReq(cServMsg);
        break;        
        // 停止适配
    case MCU_HDBAS_STOPADAPT_REQ:
        ProcStopAdptReq(cServMsg);
        break;
        
        // 设置适配参数
    case MCU_HDBAS_SETADAPTPARAM_CMD:
        ProcChangeAdptCmd(cServMsg);
        break;
        
        // 请求编一个关键帧
    case MCU_BAS_FASTUPDATEPIC_CMD:
        ProcFastUpdatePicCmd(/*cServMsg*/);
        break;        	
        
        // 检测请求关键帧定时器
    case EV_BAS_NEEDIFRAME_TIMER:
        ProcTimerNeedIFrame();
        break;        
        
        // 显示适配状态    
     case EV_BAS_SHOW:
        StatusShow();
        break;

	 case EV_BAS_MODSENDADDR:
		 ProcModSndAddr();
		 break;

    default:
		log( LOGLVL_EXCEPTION, "[Error]Unexcpet Message %u(%s).\n", pcMsg->event, ::OspEventDesc(pcMsg->event) );
        break;
    }
    return;
}

/*=============================================================================
  函 数 名： DaemonProcPowerOn
  功    能： 初始化处理
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
             CApp* pcApp
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人      修改内容
  2008/12/04  4.5         周文        创建
=============================================================================*/
void CMpuBasInst::DaemonProcPowerOn( const CMessage* const pcMsg/*, CApp* pcApp*/ )
{
    if ( pcMsg->length == sizeof(TMpuCfg) )
    {
        memcpy( (u8*)(&g_cMpuBasApp.m_tCfg), pcMsg->content, sizeof(TEqpCfg) );
    }
    //g_cMpuBasApp.m_byWorkMode = TYPE_MPUBAS;
	
    // 码率作弊
    sprintf( MPULIB_CFG_FILE, "%s/mcueqp.ini", DIR_CONFIG );
    g_cMpuBasApp.ReadDebugValues();    

    u16 wRet = KdvSocketStartup();
    
    if ( MEDIANET_NO_ERROR != wRet )
    {
        mpulog( MPU_CRIT, "KdvSocketStartup failed, error: %d\n", wRet );
        printf( "KdvSocketStartup failed, error: %d\n", wRet );
        return ;
    }

#ifdef _LINUX_
	if (TYPE_MPUBAS == g_cMpuBasApp.m_byWorkMode)
	{
		wRet = HardMPUInit( (u32)INITMODE_HDBAS );
	}
	else if(TYPE_MPUBAS_H == g_cMpuBasApp.m_byWorkMode)
	{
		wRet = HardMPUInit( (u32)INITMODE_HD2BAS );
	}
	else
	{
		printf("[DaemonProcPowerOn] unexpected Mpu WorkMode:%d!\n", g_cMpuBasApp.m_byWorkMode);
	}
    
    if ( (u16)Codec_Success != wRet )
    {
        mpulog( MPU_CRIT, "HardMPUInit failed, error: %d\n", wRet );
        printf( "HardMPUInit failed, error: %d\n", wRet );
        return ;
    }
#endif	  
    
    // 开始向MCU注册
    if ( TRUE == g_cMpuBasApp.m_bEmbedMcu )
    {
        mpulog( MPU_INFO, "[DaemonProcPowerOn] bas embed in Mcu A\n" );
        g_cMpuBasApp.m_dwMcuNode = 0;                              //内嵌时，将节点号置为0就可以
        SetTimer( EV_BAS_REGISTER, MPU_REGISTER_TIMEOUT );   //直接注册
    }
    
    if( TRUE == g_cMpuBasApp.m_bEmbedMcuB )
    {
        mpulog( MPU_INFO, "[DaemonProcPowerOn] bas embed in Mcu B\n" );
        g_cMpuBasApp.m_dwMcuNodeB = 0;
        SetTimer( EV_BAS_REGISTERB, MPU_REGISTER_TIMEOUT );  //直接注册
    } 
    
    if( 0 != g_cMpuBasApp.m_tCfg.dwConnectIP && FALSE == g_cMpuBasApp.m_bEmbedMcu )
    {
        SetTimer( EV_BAS_CONNECT, MPU_CONNETC_TIMEOUT );     //3s后再建链
    }
    if( 0 != g_cMpuBasApp.m_tCfg.dwConnectIpB && FALSE == g_cMpuBasApp.m_bEmbedMcuB )
    {
        SetTimer( EV_BAS_CONNECTB, MPU_CONNETC_TIMEOUT );    //3s后再建链
    }

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
void CMpuBasInst::DaemonProcConnectTimeOut( BOOL32 bIsConnectA )
{
    BOOL32 bRet = FALSE;
    if( TRUE == bIsConnectA )
    {
        bRet = ConnectMcu( bIsConnectA, g_cMpuBasApp.m_dwMcuNode );
        if ( TRUE == bRet )
        {  
            SetTimer( EV_BAS_REGISTER, MPU_REGISTER_TIMEOUT ); 
        }
        else
        {
            SetTimer( EV_BAS_CONNECT, MPU_CONNETC_TIMEOUT );   //3s后再建链
        }
    }
    else
    {
        bRet = ConnectMcu( bIsConnectA, g_cMpuBasApp.m_dwMcuNodeB );
        if ( TRUE == bRet )
        { 
            SetTimer( EV_BAS_REGISTERB, MPU_REGISTER_TIMEOUT ); 
        }
        else
        {
            SetTimer( EV_BAS_CONNECTB, MPU_CONNETC_TIMEOUT );   //3s后再建链
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
BOOL32 CMpuBasInst::ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode )
{
    BOOL32 bRet = TRUE;
    if( !OspIsValidTcpNode(dwMcuNode) ) // 过滤不合法节点
    {
        if(TRUE == bIsConnectA)
        {
            // zw 20081208 貌似去OspConnect，其实没有连是去取。
            dwMcuNode = BrdGetDstMcuNode();  
        }
        else
        {  
            dwMcuNode = BrdGetDstMcuNodeB(); 
        }

	    if ( ::OspIsValidTcpNode(dwMcuNode) )
	    {
			printf("[ConnectMcu]Connect Mcu %s Success, node: %d!\n", 
                   bIsConnectA ? "A" : "B", dwMcuNode);

		    mpulog(MPU_CRIT, "Connect Mcu %s Success, node: %d!\n", 
                   bIsConnectA ? "A" : "B", dwMcuNode);
		    ::OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), GetInsID());// 断链消息处理
	    }
	    else 
	    {
			printf("[ConnectMcu]Connect Mcu %s failed, node: %d\n", 
                   bIsConnectA ? "A" : "B", dwMcuNode);
		    mpulog(MPU_CRIT, "Connect to Mcu failed, will retry\n");
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
void CMpuBasInst::DaemonProcRegisterTimeOut( BOOL32 bIsRegiterA )
{
    if( TRUE == bIsRegiterA )
    {
		printf("[DaemonProcRegisterTimeOut] send BAS_MCU_REG_REQ to NodeA.%d\n", g_cMpuBasApp.m_dwMcuNode);
        Register( /*bIsRegiterA,*/ g_cMpuBasApp.m_dwMcuNode );
        SetTimer( EV_BAS_REGISTER, MPU_REGISTER_TIMEOUT );  //直接注册
    }
    else
    {
		printf("[DaemonProcRegisterTimeOut] send BAS_MCU_REG_REQ to NodeB.%d\n",g_cMpuBasApp.m_dwMcuNodeB);
        Register(/* bIsRegiterA,*/ g_cMpuBasApp.m_dwMcuNodeB );
		SetTimer( EV_BAS_REGISTERB, MPU_REGISTER_TIMEOUT );  //直接注册
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
void CMpuBasInst::Register(/* BOOL32 bIsRegiterA,*/ u32 dwMcuNode )
{
    CServMsg       cSvrMsg;
    TPeriEqpRegReq tReg;

    tReg.SetMcuEqp( (u8)g_cMpuBasApp.m_tCfg.wMcuId, g_cMpuBasApp.m_tCfg.byEqpId, g_cMpuBasApp.m_tCfg.byEqpType );
    tReg.SetPeriEqpIpAddr( htonl(g_cMpuBasApp.m_tCfg.dwLocalIP) );
    tReg.SetVersion( DEVVER_MPU );
    tReg.SetStartPort( g_cMpuBasApp.m_tCfg.wRcvStartPort );
	tReg.SetEqpAlias( g_cMpuBasApp.m_tCfg.achAlias );
	// fxh bas别名
    tReg.SetHDEqp( TRUE );

    cSvrMsg.SetMsgBody( (u8*)&tReg, sizeof(tReg) );
        
    post( MAKEIID(AID_MCU_PERIEQPSSN, g_cMpuBasApp.m_tCfg.byEqpId),
            BAS_MCU_REG_REQ,
            cSvrMsg.GetServMsg(),
            cSvrMsg.GetServMsgLen(),
            dwMcuNode );

	return;
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
void CMpuBasInst::DaemonProcMcuRegAck( const CMessage* const pMsg/*, CApp* pcApp*/ )
{
	if( NULL == pMsg )
    {
        mpulog( MPU_CRIT, "Recv Reg Ack Msg, but the msg's pointer is Null\n" );
        return;
    }

	TPeriEqpRegAck* ptRegAck;

	CServMsg cServMsg( pMsg->content, pMsg->length );
	u8* pMsgBody = cServMsg.GetMsgBody();

	ptRegAck = (TPeriEqpRegAck*)pMsgBody;
	pMsgBody += sizeof(TPeriEqpRegAck);
	OspPrintf(TRUE, FALSE, "[DaemonProcMcuRegAck] 挂载转发板Ip:%s!\n", StrOfIP(ptRegAck->GetMcuIpAddr()));
    TPrsTimeSpan tPrsTimeSpan = *(TPrsTimeSpan*)pMsgBody;
	pMsgBody += sizeof(TPrsTimeSpan);

	u16 wMTUSize = *(u16*)pMsgBody;
	pMsgBody += sizeof(u16);
	wMTUSize = ntohs( wMTUSize );

	TEqpBasHDCfgInfo tBasConfig = *(TEqpBasHDCfgInfo*)pMsgBody;

    if( pMsg->srcnode == g_cMpuBasApp.m_dwMcuNode ) // 注册应答
    {
	    g_cMpuBasApp.m_dwMcuIId = pMsg->srcid;
        g_cMpuBasApp.m_byRegAckNum++;
        KillTimer(EV_BAS_REGISTER);// 关闭定时器

        mpulog( MPU_INFO, "[RegAck] Regist success to mcu A\n" );
    }
    else if ( pMsg->srcnode == g_cMpuBasApp.m_dwMcuNodeB )
    {
        g_cMpuBasApp.m_dwMcuIIdB = pMsg->srcid;
        g_cMpuBasApp.m_byRegAckNum++;

        KillTimer(EV_BAS_REGISTERB);

        mpulog( MPU_INFO, "[RegAck] Regist success to mcu B\n" );       
    }

	// guzh [6/12/2007] 校验会话参数
    if ( g_cMpuBasApp.m_dwMpcSSrc == 0 )
    {
        g_cMpuBasApp.m_dwMpcSSrc = ptRegAck->GetMSSsrc();
    }
    else
    {
        // 异常情况，断开两个节点
        if ( g_cMpuBasApp.m_dwMpcSSrc != ptRegAck->GetMSSsrc() )
        {
            mpulog( MPU_CRIT, "[RegAck] MPC SSRC ERROR(%u<-->%u), Disconnect Both Nodes!\n", 
                      g_cMpuBasApp.m_dwMpcSSrc, ptRegAck->GetMSSsrc());
            if ( OspIsValidTcpNode(g_cMpuBasApp.m_dwMcuNode) )
            {
                OspDisconnectTcpNode(g_cMpuBasApp.m_dwMcuNode);
            }
            if ( OspIsValidTcpNode(g_cMpuBasApp.m_dwMcuNodeB) )
            {
                OspDisconnectTcpNode(g_cMpuBasApp.m_dwMcuNodeB);
            }      
            return;
        }
    }
    
	u8 byInitChnlNum = 0;
	if (!GetBasInitChnlNumAcd2WorkMode(g_cMpuBasApp.m_byWorkMode, byInitChnlNum))
	{
		OspPrintf(TRUE,  FALSE, "[DaemonProcMcuRegAck] GetBasInitChnlNumAcd2WorkMode failed!\n");
		return;
	}
    OspPrintf(TRUE, FALSE, "[RegAck] mpu init ChnNum:%d\n", byInitChnlNum);

    if( FIRST_REGACK == g_cMpuBasApp.m_byRegAckNum ) // 第一次收到注册成功消息
    {
        g_cMpuBasApp.m_dwMcuRcvIp       = ptRegAck->GetMcuIpAddr();
	    g_cMpuBasApp.m_wMcuRcvStartPort = ptRegAck->GetMcuStartPort();

        // 由于启动时没有获取，这里获取另外一个MCU，如果获取则去连接
        u32 dwOtherMcuIp = htonl( ptRegAck->GetAnotherMpcIp() );
        if ( g_cMpuBasApp.m_tCfg.dwConnectIpB == 0 && dwOtherMcuIp != 0 )
        {
            g_cMpuBasApp.m_tCfg.dwConnectIpB = dwOtherMcuIp;
            SetTimer( EV_BAS_CONNECTB, MPU_CONNETC_TIMEOUT );

            mpulog( MPU_CRIT, "[RegAck] Found another Mpc IP: %s, try connecting...\n", 
                      StrOfIP(dwOtherMcuIp) );
        }

		memcpy(g_cMpuBasApp.m_tCfg.achAlias, tBasConfig.GetAlias(), MAXLEN_EQP_ALIAS);
        mpulog( MPU_INFO, "[RegAck] Local Recv Start Port.%u, Mcu Start Port.%u, Alias.%s\n",
               g_cMpuBasApp.m_tCfg.wRcvStartPort, g_cMpuBasApp.m_wMcuRcvStartPort , g_cMpuBasApp.m_tCfg.achAlias );

        g_cMpuBasApp.m_tPrsTimeSpan = tPrsTimeSpan;
        mpulog( MPU_INFO, "[RegAck] The Bas Prs span: first: %d, second: %d, three: %d, reject: %d\n", 
                g_cMpuBasApp.m_tPrsTimeSpan.m_wFirstTimeSpan,
                g_cMpuBasApp.m_tPrsTimeSpan.m_wSecondTimeSpan,
                g_cMpuBasApp.m_tPrsTimeSpan.m_wThirdTimeSpan,
                g_cMpuBasApp.m_tPrsTimeSpan.m_wRejectTimeSpan );

		g_cMpuBasApp.m_wMTUSize = wMTUSize;
		mpulog( MPU_INFO, "[RegAck]The Network MTU is : %d\n", g_cMpuBasApp.m_wMTUSize );

        // zw 20081208 在第一次注册成功之后，将确认后的m_tCfg中的信息（m_tCfg是在PowerOn时传过来并保存的）转存至m_tEqp。
        g_cMpuBasApp.m_tEqp.SetMcuEqp( (u8)g_cMpuBasApp.m_tCfg.wMcuId, g_cMpuBasApp.m_tCfg.byEqpId, g_cMpuBasApp.m_tCfg.byEqpType );	

        // DAEMON 实例进入 NORMAL 状态，它没有IDLE和READY状态，没有起到什么作用。
        NEXTSTATE( (u32)NORMAL ); 

        u8 byLoop;
		//根据mpubas启动模式来决定初始化几个通道
		
        for ( byLoop = 1; byLoop <= byInitChnlNum/*MAXNUM_MPUBAS_CHANNEL*/; byLoop++ )
        {
            post( MAKEIID(GetAppID(), byLoop), EV_BAS_INI );
        }
    }

    TPeriEqpStatus tEqpStatus;
    tEqpStatus.SetMcuEqp( (u8)g_cMpuBasApp.m_tCfg.wMcuId, 
                          g_cMpuBasApp.m_tCfg.byEqpId,
                          g_cMpuBasApp.m_tCfg.byEqpType );
    tEqpStatus.m_byOnline = 1;
    tEqpStatus.SetAlias( g_cMpuBasApp.m_tCfg.achAlias );

	//根据bas启动模式在mcu侧由TYPE_MPU和TYPE_MPU_H区分
	if (TYPE_MPUBAS == g_cMpuBasApp.m_byWorkMode)
	{
		tEqpStatus.m_tStatus.tHdBas.SetEqpType(TYPE_MPU);
	}
	else if (TYPE_MPUBAS_H == g_cMpuBasApp.m_byWorkMode)
	{
		tEqpStatus.m_tStatus.tHdBas.SetEqpType(TYPE_MPU_H);
	}
	else
	{
		OspPrintf(TRUE, FALSE, "[DaemonProcMcuRegAck] unexpected mpu workmode:%d!\n", g_cMpuBasApp.m_byWorkMode);
	}
    
    cServMsg.SetMsgBody( (u8*)&tEqpStatus, sizeof(tEqpStatus) );

	SendMsgToMcu( BAS_MCU_BASSTATUS_NOTIF, cServMsg );
	
    for (u8 byChnlIdx = 1; byChnlIdx <= byInitChnlNum/*MAXNUM_MPUBAS_CHANNEL*/; byChnlIdx++ )
    {
		post( MAKEIID(GetAppID(), byChnlIdx), EV_BAS_SENDCHNSTATUS);
    }
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
void CMpuBasInst::DaemonProcMcuRegNack( CMessage* const pMsg ) const
{
    if ( pMsg->srcnode == g_cMpuBasApp.m_dwMcuNode )    
    {
        mpulog( MPU_CRIT, "Bas register be refused by A.\n" );
    }
    if ( pMsg->srcnode == g_cMpuBasApp.m_dwMcuNodeB )    
    {
        mpulog( MPU_CRIT, "Bas register be refused by B.\n" );
    }
    return;
}

/*=============================================================================
  函 数 名： DeamonProcGetMsStatus
  功    能： 处理取主备倒换状态
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人      修改内容
  2008/08/04  4.5         顾振华         创建
=============================================================================*/
void CMpuBasInst::DeamonProcGetMsStatusRsp( CMessage* const pMsg )
{
	BOOL bSwitchOk = FALSE;
	CServMsg cServMsg( pMsg->content, pMsg->length );
	if( MCU_EQP_GETMSSTATUS_ACK == pMsg->event )
	{
		TMcuMsStatus *ptMsStatus = (TMcuMsStatus*)cServMsg.GetMsgBody();
        
        KillTimer( EV_BAS_GETMSSTATUS );
        mpulog( MPU_INFO, "[DeamonProcGetMsStatus]. receive msg MCU_EQP_GETMSSTATUS_ACK. IsMsSwitchOK :%d\n", 
                ptMsStatus->IsMsSwitchOK() );

        if ( ptMsStatus->IsMsSwitchOK() ) // 倒换成功
        {
            bSwitchOk = TRUE;
        }
	}

	// 倒换失败或者超时
	if ( !bSwitchOk )
	{
		//20110914 zjl 超时后先清状态 再断链 再连接
		ClearInstStatus();
		mpulog(MPU_INFO, "[DeamonProcGetMsStatusRsp] ClearCurrentInst!\n");

		if ( OspIsValidTcpNode(g_cMpuBasApp.m_dwMcuNode))
		{
			mpulog(MPU_INFO, "[DeamonProcGetMsStatusRsp] OspDisconnectTcpNode A!\n");
			OspDisconnectTcpNode(g_cMpuBasApp.m_dwMcuNode);
		}
		if ( OspIsValidTcpNode(g_cMpuBasApp.m_dwMcuNodeB))
		{
			mpulog(MPU_INFO, "[DeamonProcGetMsStatusRsp] OspDisconnectTcpNode B!\n");
			OspDisconnectTcpNode(g_cMpuBasApp.m_dwMcuNodeB);
		}

		if ( INVALID_NODE == g_cMpuBasApp.m_dwMcuNode )// 有可能产生重连，在Connect里面过滤
		{
			mpulog(MPU_INFO, "[DeamonProcGetMsStatusRsp] EV_BAS_CONNECT!\n");
			SetTimer( EV_BAS_CONNECT, MPU_CONNETC_TIMEOUT );
		}
		if ( INVALID_NODE == g_cMpuBasApp.m_dwMcuNodeB )
		{
			mpulog(MPU_INFO, "[DeamonProcGetMsStatusRsp] EV_BAS_CONNECTB!\n");
			SetTimer( EV_BAS_CONNECTB, MPU_CONNETC_TIMEOUT );
		}
	}
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
void CMpuBasInst::DaemonProcOspDisconnect( CMessage* const pMsg/*, CApp* pcApp*/ )
{
    if ( NULL == pMsg )  
    {
        mpulog( MPU_CRIT, "[DaemonProcOspDisconnect] message's pointer is Null\n" );
        return;
    }

    u32 dwNode = *(u32*)pMsg->content;

    if ( INVALID_NODE != dwNode )
    {
        OspDisconnectTcpNode( dwNode );
    } 
    if( dwNode == g_cMpuBasApp.m_dwMcuNode ) // 断链
    {
        mpulog( MPU_INFO, "[DaemonProcOspDisconnect] McuNode.A disconnect\n" );
        g_cMpuBasApp.FreeStatusDataA();
        SetTimer( EV_BAS_CONNECT, MPU_CONNETC_TIMEOUT );
    }
    else if ( dwNode == g_cMpuBasApp.m_dwMcuNodeB )
    {
        mpulog( MPU_INFO, "[DaemonProcOspDisconnect] McuNode.B disconnect\n" );
        g_cMpuBasApp.FreeStatusDataB();
        SetTimer( EV_BAS_CONNECTB, MPU_CONNETC_TIMEOUT );
    }
	// 与其中一个断链后，向Mcu取主备倒换状态，判断是否成功
	if ( INVALID_NODE != g_cMpuBasApp.m_dwMcuNode || INVALID_NODE != g_cMpuBasApp.m_dwMcuNodeB )
	{
		if ( OspIsValidTcpNode(g_cMpuBasApp.m_dwMcuNode) )
		{
			post( g_cMpuBasApp.m_dwMcuIId, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cMpuBasApp.m_dwMcuNode );           
			mpulog( MPU_INFO, "[DaemonProcOspDisconnect] GetMsStatusReq. McuNode.A\n" );
		}
		else if ( OspIsValidTcpNode(g_cMpuBasApp.m_dwMcuNodeB) )
		{
			post( g_cMpuBasApp.m_dwMcuIIdB, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cMpuBasApp.m_dwMcuNodeB );        
			mpulog( MPU_INFO, "[DaemonProcMcuDisconnect] GetMsStatusReq. McuNode.B\n" );
		}

		SetTimer( EV_BAS_GETMSSTATUS, WAITING_MSSTATUS_TIMEOUT );
		return;
	}
	// 两个节点都断，清空状态信息
    if ( INVALID_NODE == g_cMpuBasApp.m_dwMcuNode && INVALID_NODE == g_cMpuBasApp.m_dwMcuNodeB )
    {
        ClearInstStatus();
    }
    return;
}
/*====================================================================
	函数  : DeamonProcModSendIpAddr
	功能  : 处理MCU强制命令修改发送Ip地址
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2008/12/09  4.5         付秀华          创建
====================================================================*/
void CMpuBasInst::DeamonProcModSendIpAddr( CMessage* const pMsg )
{
    if ( NULL == pMsg->content )
    {
        mpulog( MPU_CRIT, " The pointer cannot be Null (MsgStartVidMixProc)\n" );
        return;
    } 

	u32 dwSendIP = *(u32*)pMsg->content;
	g_cMpuBasApp.m_dwMcuRcvIp = ntohl(dwSendIP);
	u8 byInitNum = 0;
	if(!GetBasInitChnlNumAcd2WorkMode(g_cMpuBasApp.m_byWorkMode, byInitNum))
	{
		OspPrintf(TRUE, FALSE, "[DeamonProcModSendIpAddr] GetBasInitChnlNumAcd2WorkMode failed!\n");
		return;
	}

    for (u8 byLoop = 1; byLoop <= byInitNum/*MAXNUM_MPUBAS_CHANNEL*/; byLoop++ )
    {
        post( MAKEIID(GetAppID(), byLoop), EV_BAS_MODSENDADDR);
    }	

}

/*====================================================================
	函数  : DeamonShowBasDebugInfo
	功能  : 显示BAS的debug配置信息
	输入  : 
	输出  : 无
	返回  : 无
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人      修改内容
  2010/11/30  4.6         倪志俊        创建
====================================================================*/
void CMpuBasInst::DeamonShowBasDebugInfo()
{
	g_cMpuBasApp.m_tDebugVal.Print();	
}

/*====================================================================
	函数  : ProcInitBas
	功能  : 
	输入  : 
	输出  : 无
	返回  : 无
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人      修改内容
  2008/12/09  4.5         周文        创建
====================================================================*/
void CMpuBasInst::ProcInitBas()
{
    BOOL bRet = TRUE;

	u8 byInitNum = 0;
	if(!GetBasOutChnlNumAcd2WorkMode(g_cMpuBasApp.m_byWorkMode, byInitNum))
	{
		printf("[DaemonProcRegAck] GetBasOutChnlNumAcd2WorkMode failed!");
		OspPrintf(TRUE, FALSE, "[DaemonProcRegAck] GetBasOutChnlNumAcd2WorkMode failed!\n");
		return;
	}
	
	u16 wDstPort = g_cMpuBasApp.m_wMcuRcvStartPort + ( GetInsID() - 1 ) * PORTSPAN * byInitNum/*2*/;
    if ( m_pAdpCroup == NULL )
    {
        // start new       
        m_pAdpCroup = new CMpuBasAdpGroup;
/*lint -save -e734*/
        u16 wRecvPort = g_cMpuBasApp.m_tCfg.wRcvStartPort + ( GetInsID() - 1 ) * PORTSPAN * byInitNum/*2*/;	
		printf("[ProcInitBas] Chn.%d , RcvPort:%u, DstPort:%u!\n", GetInsID()-1, wRecvPort, wDstPort);
		OspPrintf(TRUE, FALSE, "[ProcInitBas] Chn.%d , RcvPort:%u, DstPort:%u!\n", GetInsID()-1, wRecvPort, wDstPort);

        bRet = m_pAdpCroup->Create( /*MAX_VIDEO_FRAME_SIZE, */
                                    wRecvPort + 1,
                                    wRecvPort,
                                    g_cMpuBasApp.m_dwMcuRcvIp,
                                    wDstPort,
								    (u32)this,
                                    GetInsID()-1 );
/*lint -restore*/
        // new frm queue
        m_ptFrmQueue = new TRtpPkQueue;

        if (NULL != m_ptFrmQueue)
        {
            m_ptFrmQueue->Init(g_cMpuBasApp.GetAudioDeferNum());
        }

		//  [11/9/2009 pengjie] 设置加黑边裁边	
		printf( "[MPUdebug][ProcInitBas][SetResizeMode] Mode = %d \n", g_cMpuBasApp.GetVidEncHWMode() );
		if( (u16)Codec_Success != m_pAdpCroup->m_cAptGrp.SetResizeMode( (u32)g_cMpuBasApp.GetVidEncHWMode() ) )
		{
			printf( "[MPUdebug][ProcInitBas][SetResizeMode] Mode = %d Failed\n", g_cMpuBasApp.GetVidEncHWMode() );
		}

	}
	else
	{
		m_pAdpCroup->ResetDestPort( wDstPort);
		m_pAdpCroup->ModNetSndIpAddr(g_cMpuBasApp.m_dwMcuRcvIp);
		OspPrintf(TRUE, FALSE, "[ProcInitBas] set new DstIp:%s,new DstPort:%u!\n",StrOfIP(g_cMpuBasApp.m_dwMcuRcvIp), wDstPort);
	}

    if (bRet)
    {
        NextState( (u32)READY );
       	OspPrintf(TRUE, FALSE, "[ProcInitBas]set EV_BAS_NEEDIFRAME_TIMER : %d\n", g_cMpuBasApp.GetIframeInterval());
        SetTimer( EV_BAS_NEEDIFRAME_TIMER, g_cMpuBasApp.GetIframeInterval() );   // 定时查询适配组是否需要关键帧
    }
  
    SendChnNotif();    
  
    return;
}

/*====================================================================
	函数  : ProcStopAdptReq
	功能  : 停止适配
	输入  : 
	输出  : 无
	返回  : 无
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人      修改内容
  2008/12/09  4.5         周文        创建
====================================================================*/
void CMpuBasInst::ProcStopBas()
{
    KillTimer( EV_BAS_NEEDIFRAME_TIMER );

    NextState( (u32)IDLE );
    //清空外设状态
	memset(&m_tChnInfo.m_tChnStatus, 0, sizeof(THDBasVidChnStatus));
    SendChnNotif();
}

/*====================================================================
	函数  : SendChnNotif
	功能  : 向MCU发送通道状态通知
	输入  : 
	输出  : 无
	返回  : 成功返回TRUE否则FALSE
	注    : 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人      修改内容
  2008/12/09  4.5         周文        创建
====================================================================*/
void CMpuBasInst::SendChnNotif()
{
	u8 byChnIdx = GetInsID() - 1;

    CServMsg cServMsg;
	cServMsg.SetConfId( m_tChnInfo.m_cChnConfId );
  
    m_tChnInfo.m_tChnStatus.SetEqp( g_cMpuBasApp.m_tEqp );
    m_tChnInfo.m_tChnStatus.SetChnIdx( byChnIdx );
    m_tChnInfo.m_tChnStatus.SetStatus( (u8)CurState() );
    
    u8 byChnType = HDBAS_CHNTYPE_MPU;
    cServMsg.SetMsgBody((u8*)&byChnType, sizeof(u8));
    cServMsg.CatMsgBody((u8*)&m_tChnInfo.m_tChnStatus, sizeof(m_tChnInfo.m_tChnStatus));
    
    mpulog( MPU_INFO, "[SendChnNotif] Inst.%d Channel.%u state.%u\n",
               GetInsID(),
               byChnIdx,
               CurState() );
    
	SendMsgToMcu( HDBAS_MCU_CHNNLSTATUS_NOTIF, cServMsg );
    return;
}
/*====================================================================
	函数  : SendMsgToMcu
	功能  : 向MCU发送消息
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人      修改内容
  2008/08/04  4.5         顾振华         创建
====================================================================*/
void CMpuBasInst::SendMsgToMcu( u16 wEvent, CServMsg& cServMsg )
{
    if ( GetInsID() != CInstance::DAEMON )
    {
        cServMsg.SetChnIndex( (u8)GetInsID() - 1 );
    }
    
    if ( g_cMpuBasApp.m_bEmbedMcu || OspIsValidTcpNode(g_cMpuBasApp.m_dwMcuNode) ) 
	{
		post( g_cMpuBasApp.m_dwMcuIId, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), g_cMpuBasApp.m_dwMcuNode );
		mpulog( MPU_INFO, "Send Message %u(%s) to Mcu A\n",
                wEvent, ::OspEventDesc(wEvent) );
		printf( "Send Message %u(%s) to Mcu A\n",
                wEvent, ::OspEventDesc(wEvent) );
	}
	else
	{
		mpulog( MPU_CRIT, "SendMsgToMcuA (event:%s) error \n", ::OspEventDesc( wEvent ));
		printf("SendMsgToMcuA (event:%s) error \n", ::OspEventDesc( wEvent ));
	}

    if ( g_cMpuBasApp.m_bEmbedMcuB || OspIsValidTcpNode(g_cMpuBasApp.m_dwMcuNodeB) )
    {
	    post( g_cMpuBasApp.m_dwMcuIIdB, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), g_cMpuBasApp.m_dwMcuNodeB );
		mpulog( MPU_INFO, "Send Message %u(%s) to Mcu B\n",
                wEvent, ::OspEventDesc(wEvent) );
		printf( "Send Message %u(%s) to Mcu B\n",
                wEvent, ::OspEventDesc(wEvent) );
    }
    else
    {
		mpulog( MPU_CRIT, "SendMsgToMcuB (event:%s) error \n", ::OspEventDesc( wEvent ));
		printf("SendMsgToMcuA (event:%s) error \n", ::OspEventDesc( wEvent ));
    }

    return ;
}

/*====================================================================
    函数  : ProcStartAdptReq
    功能  : 开始适配处理
    输入  : 
    输出  : 无
    返回  : 无
    注    : 
----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人      修改内容
  2008/12/09  4.5         周文        创建
====================================================================*/
void CMpuBasInst::ProcStartAdptReq( CServMsg& cServMsg )
{   
    u8 byChnIdx = (u8)GetInsID() - 1;

    mpulog( MPU_INFO, "[ProcStartAdptReq] Start adp, The channel is %d\n", byChnIdx );

	if( NULL == m_pAdpCroup )
	{
        cServMsg.SetErrorCode( ERR_BAS_OPMAP );
		cServMsg.SetMsgBody( (u8*)&g_cMpuBasApp.m_tEqp, sizeof(g_cMpuBasApp.m_tEqp) );
		SendMsgToMcu( cServMsg.GetEventId() + 2, cServMsg);
		mpulog( MPU_CRIT, "[ProcStartAdptReq] Channel.%d create failed !\n", GetInsID() );	
		return;
	}
	
	if (m_pAdpCroup->IsStart())
	{
	//	m_pAdpCroup->m_cAptGrp.ClearVideo();
	}

    TMediaEncrypt  tMediaEncryptVideo;
	TDoublePayload tDoublePayloadVideo;
	
	//根据模式解析参数输出个数
	u8 byOutPutNum = 0;
	if (TYPE_MPUBAS == g_cMpuBasApp.m_byWorkMode)
	{
		byOutPutNum = MAXNUM_VOUTPUT;
	}
	else if(TYPE_MPUBAS_H == g_cMpuBasApp.m_byWorkMode)
	{
		byOutPutNum = MAXNUM_MPU_H_VOUTPUT;
	}
	else
	{
		OspPrintf(TRUE, FALSE, "[ProcStartAdptReq] unexpected mpu workmode:%d!\n", g_cMpuBasApp.m_byWorkMode);
		return;
	}
	mpulog( MPU_INFO, "[ProcStartAdptReq] BasOutNum is %d\n", byOutPutNum);

	THDAdaptParam  atParm[MAXNUM_MPU_H_VOUTPUT];
	for (u8 byOutIdx = 0; byOutIdx < byOutPutNum; byOutIdx++)
	{
		atParm[byOutIdx] = *(THDAdaptParam*)(cServMsg.GetMsgBody() + byOutIdx * sizeof(THDAdaptParam));
	}

	//zjl
    //THDAdaptParam  atParm[2];
	//atParm[0] = *(THDAdaptParam*)cServMsg.GetMsgBody();
	//atParm[1] = *(THDAdaptParam*)( cServMsg.GetMsgBody() + sizeof(THDAdaptParam) );

	for (u8 byIdx = 0; byIdx < byOutPutNum; byIdx++)
	{
		ConvertToRealFR(atParm[byIdx]);
		ConverToRealParam(GetInsID()-1,byIdx,atParm[byIdx]);
	}

    
    if ( g_cMpuBasApp.IsEnableCheat())
    {
		for (u8 byOutId = 0; byOutId < byOutPutNum; byOutId++)
		{
			if (MEDIA_TYPE_NULL == atParm[byOutId].GetVidType() ||
				0 == atParm[byOutId].GetVidType())
			{  
				continue;
			}
			u16 wBitrate = atParm[byOutId].GetBitrate();
			wBitrate = g_cMpuBasApp.GetDecRateDebug( wBitrate );
			atParm[byOutId].SetBitRate( wBitrate );
		}
		/*zjl
        u16 wBitrate = atParm[0].GetBitrate();
        wBitrate = g_cMpuBasApp.GetDecRateDebug( wBitrate );
        atParm[0].SetBitRate( wBitrate );

        wBitrate = atParm[1].GetBitrate();
        wBitrate = g_cMpuBasApp.GetDecRateDebug( wBitrate );
        atParm[1].SetBitRate( wBitrate );
		*/
    }

    tMediaEncryptVideo = *(TMediaEncrypt*)( cServMsg.GetMsgBody()  + byOutPutNum * sizeof(THDAdaptParam) );
    tDoublePayloadVideo= *(TDoublePayload*)( cServMsg.GetMsgBody() + byOutPutNum * sizeof(THDAdaptParam) + sizeof(TMediaEncrypt) );
	
    u8 byAudDecType = MEDIA_TYPE_NULL;
    if (cServMsg.GetMsgBodyLen() > sizeof(THDAdaptParam) * byOutPutNum + sizeof(TMediaEncrypt) + sizeof(TDoublePayload))
    {
        byAudDecType = *(cServMsg.GetMsgBody() + byOutPutNum * sizeof(THDAdaptParam) + sizeof(TMediaEncrypt)+ sizeof(TDoublePayload));
    }
	
	switch( CurState() )
	{
	case READY:  
	case NORMAL: //重入临时支持
		{
            //u16 wRet = 0;
			BOOL32 bCreatSendObj = FALSE;
			TVideoEncParam atVidEncPar[MAXNUM_MPU_H_VOUTPUT];


			// fxh 目前mcu采用标准4CIF(704* 576), 而对于MPEG4,采取D1(720 * 576)
			for(u8 byOutChnlIdx = 0; byOutChnlIdx < byOutPutNum; byOutChnlIdx++)
			{
				if (MEDIA_TYPE_MP4 == atParm[byOutChnlIdx].GetVidType() &&
					704 == atParm[byOutChnlIdx].GetWidth() &&
					576 == atParm[byOutChnlIdx].GetHeight())
				{
					atParm[byOutChnlIdx].SetResolution(720, 576);
				}

			    g_cMpuBasApp.GetDefaultParam( atParm[byOutChnlIdx].GetVidType(), atVidEncPar[byOutChnlIdx] );

				//设置第一路编码参数
				atVidEncPar[byOutChnlIdx].m_wVideoHeight = atParm[byOutChnlIdx].GetHeight();
				atVidEncPar[byOutChnlIdx].m_wVideoWidth  = atParm[byOutChnlIdx].GetWidth();
				atVidEncPar[byOutChnlIdx].m_wBitRate     = atParm[byOutChnlIdx].GetBitrate(); 
				atVidEncPar[byOutChnlIdx].m_byFrameRate  = atParm[byOutChnlIdx].GetFrameRate();

				//zbq[09/04/2008] FIXME: 临时修正，稍后调整到MCU
				//依然保留？
	// 			if ( atVidEncPar[0].m_wVideoWidth == 720 &&
	// 				 atVidEncPar[0].m_wVideoHeight == 576 )
	// 			{
	// 				atVidEncPar[0].m_wVideoWidth = 704;
	// 			}


				//zjl[20100531]暂时放开
// 				if ( atVidEncPar[byOutChnlIdx].m_byFrameRate == 30 )
// 				{
// 					atVidEncPar[byOutChnlIdx].m_byFrameRate = 25;
// 				}

				//zbq[10/10/2008] 根据分辨率调整最大关键帧 间隔
				if ( (atVidEncPar[byOutChnlIdx].m_wVideoWidth == 1280 &&
					  atVidEncPar[byOutChnlIdx].m_wVideoHeight == 720) ||
					  (atVidEncPar[byOutChnlIdx].m_wVideoWidth == 1920 &&
					   atVidEncPar[byOutChnlIdx].m_wVideoHeight == 1088) ||
					   (atVidEncPar[byOutChnlIdx].m_wVideoWidth == 1920 &&
					   atVidEncPar[byOutChnlIdx].m_wVideoHeight == 544))
				{
					atVidEncPar[byOutChnlIdx].m_byMaxKeyFrameInterval = 3000;
				}
				else if( (atVidEncPar[byOutChnlIdx].m_wVideoWidth == 704 &&
						  atVidEncPar[byOutChnlIdx].m_wVideoHeight == 576) ||
						 (atVidEncPar[byOutChnlIdx].m_wVideoWidth == 720 &&
						  atVidEncPar[byOutChnlIdx].m_wVideoHeight == 576) )
				{
					atVidEncPar[byOutChnlIdx].m_byMaxKeyFrameInterval = 1000;
				}
				else if ( atVidEncPar[byOutChnlIdx].m_wVideoWidth == 352 &&
						  atVidEncPar[byOutChnlIdx].m_wVideoHeight == 288 )
				{
					atVidEncPar[byOutChnlIdx].m_byMaxKeyFrameInterval = 500;
				}

				//调整量化参数
				if (MEDIA_TYPE_H264 == atParm[byOutChnlIdx].GetVidType())
				{
					//CIF以上给51－10
					if (atParm[byOutChnlIdx].GetHeight() > 288)
					{
						atVidEncPar[byOutChnlIdx].m_byMaxQuant = 51;
						atVidEncPar[byOutChnlIdx].m_byMinQuant = 10;
					}
					else
					{
						atVidEncPar[byOutChnlIdx].m_byMaxQuant = 45;
						atVidEncPar[byOutChnlIdx].m_byMinQuant = 20;
					}
				}
				else
				{
					atVidEncPar[byOutChnlIdx].m_byMaxQuant = 31;
					atVidEncPar[byOutChnlIdx].m_byMinQuant = 3;
				}

				mpulog( MPU_INFO, "Start Video Enc Param: %u, W*H: %dx%d, Bitrate.%dKbps, FrmRate.%d, MaxKeyFrmInt.%d\n",
								   atParm[byOutChnlIdx].GetVidType(), 
								   atVidEncPar[byOutChnlIdx].m_wVideoWidth, 
								   atVidEncPar[byOutChnlIdx].m_wVideoHeight,
								   atVidEncPar[byOutChnlIdx].m_wBitRate,
								   atVidEncPar[byOutChnlIdx].m_byFrameRate,
								   atVidEncPar[byOutChnlIdx].m_byMaxKeyFrameInterval );
			}

			for (u8 byPriOut = 0; byPriOut < byOutPutNum;  byPriOut++)
			{
				if (atParm[byPriOut].GetVidType() != 
					m_tChnInfo.m_tChnStatus.GetOutputVidParam(byPriOut)->GetVidType())
				{
					bCreatSendObj = TRUE;
					OspPrintf(TRUE, FALSE, "[ProcStartAdptReq] CreatSendObj again!\n");
					break;
				}
			}

			if ( TRUE == m_pAdpCroup->SetVideoEncParam(atVidEncPar, byChnIdx, byOutPutNum, bCreatSendObj))
			{
				// 记录通道状态
				for (u8 byOut = 0; byOut < byOutPutNum;  byOut++)
				{
					THDAdaptParam* ptAdpParam = m_tChnInfo.m_tChnStatus.GetOutputVidParam(byOut);
					if (NULL != ptAdpParam)
					{
						ptAdpParam->SetAudType( atParm[byOut].GetAudType() );
						ptAdpParam->SetVidType( atParm[byOut].GetVidType() );
						ptAdpParam->SetBitRate( atParm[byOut].GetBitrate() );
						ptAdpParam->SetResolution( atParm[byOut].GetWidth(), atParm[byOut].GetHeight() );
						ptAdpParam->SetFrameRate( atParm[byOut].GetFrameRate() );
					}
				}
//zjl
// 	    		THDAdaptParam* ptAdpParam = m_tChnInfo.m_tChnStatus.GetOutputVidParam(0);
// 
// 	    		if ( NULL != ptAdpParam )
// 				{
// 		    		ptAdpParam->SetAudType( atParm[0].GetAudType() );
// 		    		ptAdpParam->SetVidType( atParm[0].GetVidType() );
// 		    		ptAdpParam->SetBitRate( atParm[0].GetBitrate() );
// 			    	ptAdpParam->SetResolution( atParm[0].GetWidth(), atParm[0].GetHeight() );
// 					ptAdpParam->SetFrameRate( atParm[0].GetFrameRate() );
// 				}
// 
//                 THDAdaptParam* ptAdpParam1 = m_tChnInfo.m_tChnStatus.GetOutputVidParam(1);
//                 
//                 if ( NULL != ptAdpParam1 )
//                 {
//                     ptAdpParam1->SetAudType( atParm[1].GetAudType() );
//                     ptAdpParam1->SetVidType( atParm[1].GetVidType() );
//                     ptAdpParam1->SetBitRate( atParm[1].GetBitrate() );
//                     ptAdpParam1->SetResolution( atParm[1].GetWidth(), atParm[1].GetHeight() );
// 					ptAdpParam1->SetFrameRate( atParm[1].GetFrameRate() );
// 				}
			}

            //音频中转的Net参数设置
			//mpu_h音频暂时无需处理
            if(!m_pAdpCroup->SetAudioParam( byAudDecType, byChnIdx ) )
			{
				OspPrintf(TRUE, FALSE, "SetAudioParam failed!\n");
			}

			OspPrintf(TRUE, FALSE, "ActivePayLoad:%u, RealPayLoad:%u!\n", tDoublePayloadVideo.GetActivePayload(), tDoublePayloadVideo.GetRealPayLoad());
           
			u8 byEncryptMode = tMediaEncryptVideo.GetEncryptMode();
			u8 abySndActPt[MAXNUM_MPU_H_VOUTPUT];
			u8 byLoop = 0;
            if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
            {
                mpulog( MPU_CRIT, "CONF_ENCRYPTMODE_NONE == byEncryptMode\n" );
				for(;byLoop < byOutPutNum; byLoop ++)
				{
					abySndActPt[byLoop] = atParm[byLoop].GetVidActiveType();
					if(atParm[byLoop].GetVidType() == MEDIA_TYPE_H263 || atParm[byLoop].GetVidType() == MEDIA_TYPE_MP4 )
					{
						abySndActPt[byLoop] = 0;
					}
					OspPrintf(TRUE, FALSE, "abySndActPt[%d] is: %u!\n", byLoop, abySndActPt[byLoop]);
				}
				//接口调整传入指针abySndActPt和有效输出通道数byOutPutNum
                m_pAdpCroup->SetKeyandPT( NULL, 0, 0,
										  abySndActPt, byOutPutNum,
                                          tDoublePayloadVideo.GetActivePayload(), 
										  tDoublePayloadVideo.GetRealPayLoad());

				/*mpulog( MPU_INFO, "abySndActPt[0] is: %u, abySndActPt[1] is: %u\n",abySndActPt[0], abySndActPt[1] );*/
//                 m_pAdpCroup->SetKeyandPT( NULL, 0, 0,
//                                           tDoublePayloadVideo.GetActivePayload(), tDoublePayloadVideo.GetRealPayLoad(),
//                                           abySndActPt[0], abySndActPt[1] );
            }
            else if ( CONF_ENCRYPTMODE_DES == byEncryptMode || CONF_ENCRYPTMODE_AES == byEncryptMode )
            {
                u8 abyKeyBuf[MAXLEN_KEY];
                s32 byKenLen = 0;
                tMediaEncryptVideo.GetEncryptKey( abyKeyBuf, &byKenLen );
                if ( CONF_ENCRYPTMODE_DES == byEncryptMode )
                {
                    byEncryptMode = DES_ENCRYPT_MODE;
                }
                else
                {
                    byEncryptMode = AES_ENCRYPT_MODE;
                }
				//由于SetKeyandPT接口调整，这里临时组织参数
				memset(abySndActPt, 0, sizeof(abySndActPt));
				for (byLoop = 0; byLoop < byOutPutNum; byLoop++)
				{
					abySndActPt[byLoop] = atParm[byLoop].GetVidActiveType();
				}
                m_pAdpCroup->SetKeyandPT( abyKeyBuf, byKenLen, byEncryptMode, 
										  abySndActPt, byOutPutNum,
                                          tDoublePayloadVideo.GetActivePayload(), 
										  tDoublePayloadVideo.GetRealPayLoad());
			
                                         
//                 m_pAdpCroup->SetKeyandPT( abyKeyBuf, byKenLen, byEncryptMode, 
//                                           tDoublePayloadVideo.GetActivePayload(), tDoublePayloadVideo.GetRealPayLoad(),
//                                           atParm[0].GetVidActiveType(), atParm[1].GetVidActiveType() );
            }

			//设置加密 接口统一所有发送对象的密钥设置
			if(!m_pAdpCroup->SetVidEncryptKey( tMediaEncryptVideo, byOutPutNum) )
			{
				mpulog(MPU_CRIT, "[ProcStartAdptReq] SetVidEncryptKey failed\n");
			}

			//设置加密 暂无用
			//m_pAdpCroup->SetSecVidEncryptKey( tMediaEncryptVideo );

			//设置动态载荷
			if ( !m_pAdpCroup->SetVideoActivePT( tDoublePayloadVideo.GetActivePayload(), tDoublePayloadVideo.GetRealPayLoad() ) )
			{
				mpulog(MPU_CRIT, "[ProcStartAdptReq] SetVideoActivePT failed\n");
			}

            //保存ConfId
			m_tChnInfo.m_cChnConfId = cServMsg.GetConfId();

			// 设丢包重传参数
		
			BOOL32 bIsNeedPrs = FALSE;
			for (u8 byEncIdx = 0; byEncIdx < byOutPutNum; byEncIdx++)
			{
				if (atParm[byEncIdx].IsNeedbyPrs())
				{
					bIsNeedPrs = TRUE;
					break;
				}
			}

			TRSParam tNetRSParam;
			OspPrintf(TRUE, FALSE, "[ProcStartAdptReq] IsNeedPrs:%d!\n", bIsNeedPrs);
            if ( bIsNeedPrs/*atParm[0].IsNeedbyPrs()*/ )		//是否需要包重传？ 1: 重传, 0: 不重传
			{		
				mpulog( MPU_INFO, "[ProcStartAdptReq] Bas is needed by Prs!\n" );
                tNetRSParam.m_wFirstTimeSpan  = g_cMpuBasApp.m_tPrsTimeSpan.m_wFirstTimeSpan;
				tNetRSParam.m_wSecondTimeSpan = g_cMpuBasApp.m_tPrsTimeSpan.m_wSecondTimeSpan;
				tNetRSParam.m_wThirdTimeSpan  = g_cMpuBasApp.m_tPrsTimeSpan.m_wThirdTimeSpan;
				tNetRSParam.m_wRejectTimeSpan = g_cMpuBasApp.m_tPrsTimeSpan.m_wRejectTimeSpan;
				//zjl 统一接口SetNetSendFeedbackVideoParam设置所有发送通道的重传参数
				if( !m_pAdpCroup->SetNetSendFeedbackVideoParam( 2000, byOutPutNum, TRUE ))
				{
					mpulog( MPU_CRIT, "[ProcStartAdptReq]SetNetSendFeedbackVideoParam failed\n ");
				}
				if ( !m_pAdpCroup->SetNetRecvFeedbackVideoParam( tNetRSParam, TRUE ) )
				{
					mpulog( MPU_CRIT, "[ProcStartAdptReq]SetNetRecvFeedbackVideoParam failed\n ");
				}
			}
			else
			{
				mpulog( MPU_INFO, "Bas is not needed by Prs!\n" );
                tNetRSParam.m_wFirstTimeSpan  = 0;
				tNetRSParam.m_wSecondTimeSpan = 0;
				tNetRSParam.m_wThirdTimeSpan  = 0;
				tNetRSParam.m_wRejectTimeSpan = 0;
				if (!m_pAdpCroup->SetNetSendFeedbackVideoParam( 2000, byOutPutNum, FALSE ))
				{
					mpulog( MPU_CRIT, "[ProcStartAdptReq]SetNetSendFeedbackVideoParam failed\n ");
				}
				//m_pAdpCroup->SetNetSecSendFeedbackVideoParam( 2000, FALSE );
				if (!m_pAdpCroup->SetNetRecvFeedbackVideoParam( tNetRSParam, FALSE ))
				{
					mpulog( MPU_CRIT, "[ProcStartAdptReq]SetNetRecvFeedbackVideoParam failed\n ");

				}
			}

			//临时兼容重入
			if( !m_pAdpCroup->IsStart() )
			{
				BOOL bStartOk;
                bStartOk = m_pAdpCroup->StartAdapter( TRUE );
				if ( !bStartOk )
				{
					cServMsg.SetErrorCode( ERR_BAS_OPMAP );
					cServMsg.SetMsgBody( (u8*)&g_cMpuBasApp.m_tEqp, sizeof(g_cMpuBasApp.m_tEqp) );
					SendMsgToMcu( cServMsg.GetEventId() + 2, cServMsg );
					return;
				}
			}

            // 平滑发送
			// [pengjie 2010/12/30] 平滑逻辑整理
			m_pAdpCroup->SetSmoothSendRule( g_cMpuBasApp.m_bIsUseSmoothSend );
			
            cServMsg.SetMsgBody( (u8*)&g_cMpuBasApp.m_tEqp, sizeof(g_cMpuBasApp.m_tEqp) );
            SendMsgToMcu( cServMsg.GetEventId() + 1, cServMsg );
            
            NextState( (u32)NORMAL );            
            SendChnNotif();

			
			//[nizhijun 2011/03/21] BAS参数改变需要隔1秒检测关键帧请求
			KillTimer( EV_BAS_NEEDIFRAME_TIMER );
			mpulog(MPU_INFO, "[ProcStartAdptReq]reset EV_BAS_NEEDIFRAME_TIMER from %d to: %d\n", 
				g_cMpuBasApp.GetIframeInterval(), CHECK_IFRAME_INTERVAL);
			m_byIframeReqCount = 0;
			SetTimer( EV_BAS_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL );   // 定时查询适配组是否需要关键帧
		}
		break;

	default:
		cServMsg.SetErrorCode( ERR_BAS_CHNOTREAD );
		cServMsg.SetMsgBody( (u8*)&g_cMpuBasApp.m_tEqp, sizeof(g_cMpuBasApp.m_tEqp) );
		SendMsgToMcu( cServMsg.GetEventId() + 2, cServMsg );
		mpulog( MPU_CRIT, "the chn.%d in unknown state !\n", byChnIdx );
		break;
	}

    return;
}

/*====================================================================
	函数  : ProcChangeAdptCmd
	功能  : 调整适配参数
	输入  : 
	输出  : 无
	返回  : 无
	注释  : 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人      修改内容
  2008/12/09  4.5         周文        创建
====================================================================*/
void CMpuBasInst::ProcChangeAdptCmd( CServMsg& cServMsg )
{
    u8 byChnIdx = (u8)GetInsID() - 1;
	
	if ( NULL == m_pAdpCroup )
    {
		mpulog( MPU_CRIT, "[ProcChangeAdptCmd] Channel.%d create failed !\n", byChnIdx );	
		return;
	}

	if (m_pAdpCroup->IsStart())
	{
	//	m_pAdpCroup->m_cAptGrp.ClearVideo();
	}

    TMediaEncrypt tMediaEncryptVideo;
	TDoublePayload tDoublePayloadVideo;

    mpulog( MPU_INFO, "[ProcChangeAdptCmd] Adjust adp param, The channel is: %d\n", byChnIdx );

	//根据模式解析参数输出个数
	u8 byOutPutNum = 0;
	if (TYPE_MPUBAS == g_cMpuBasApp.m_byWorkMode)
	{
		byOutPutNum = MAXNUM_VOUTPUT;
	}
	else if(TYPE_MPUBAS_H == g_cMpuBasApp.m_byWorkMode)
	{
		byOutPutNum = MAXNUM_MPU_H_VOUTPUT;
	}
	else
	{
		OspPrintf(TRUE, FALSE, "[ProcChangeAdptCmd] unexpected mpu workmode:%d!\n", g_cMpuBasApp.m_byWorkMode);
		return;
	}
	mpulog( MPU_INFO, "[ProcStartAdptReq] BasOutNum is %d\n", byOutPutNum);

    THDAdaptParam  atParm[MAXNUM_MPU_H_VOUTPUT];
	for (u8 byOutIdx = 0; byOutIdx < byOutPutNum; byOutIdx++)
	{
		atParm[byOutIdx] = *(THDAdaptParam*)(cServMsg.GetMsgBody() + byOutIdx * sizeof(THDAdaptParam));
	}

// 	atParm[0]  = *(THDAdaptParam*)cServMsg.GetMsgBody();
// 	atParm[1] = *(THDAdaptParam*)( cServMsg.GetMsgBody() + sizeof(THDAdaptParam) );

	for (u8 byIdx = 0; byIdx < byOutPutNum; byIdx++)
	{
		ConvertToRealFR(atParm[byIdx]);
		ConverToRealParam(GetInsID()-1,byIdx,atParm[byIdx]);
	}
	
    tMediaEncryptVideo = *(TMediaEncrypt*)(cServMsg.GetMsgBody()  + byOutPutNum * sizeof(THDAdaptParam));
    tDoublePayloadVideo= *(TDoublePayload*)(cServMsg.GetMsgBody() + byOutPutNum * sizeof(THDAdaptParam) + sizeof(TMediaEncrypt));

    u8 byAudDecType = MEDIA_TYPE_NULL;
    if (cServMsg.GetMsgBodyLen() > sizeof(THDAdaptParam) * byOutPutNum + sizeof(TMediaEncrypt) + sizeof(TDoublePayload))
    {
        byAudDecType = *(cServMsg.GetMsgBody() + byOutPutNum * sizeof(THDAdaptParam) + sizeof(TMediaEncrypt)+ sizeof(TDoublePayload));
    }


    //支持码率作弊
    if ( g_cMpuBasApp.IsEnableCheat() )
    {
		for (u8 byOutId = 0; byOutId < byOutPutNum; byOutId++)
		{
			if (MEDIA_TYPE_NULL == atParm[byOutId].GetVidType() ||
				0 == atParm[byOutId].GetVidType())
			{  
				continue;
			}
			u16 wBitrate = atParm[byOutId].GetBitrate();
			wBitrate = g_cMpuBasApp.GetDecRateDebug( wBitrate );
			atParm[byOutId].SetBitRate( wBitrate );
		}
//         u16 wBitrate = atParm[0].GetBitrate();
//         wBitrate = g_cMpuBasApp.GetDecRateDebug( wBitrate );
//         atParm[0].SetBitRate(wBitrate);
// 
//         wBitrate = atParm[1].GetBitrate();
//         wBitrate = g_cMpuBasApp.GetDecRateDebug( wBitrate );
//         atParm[1].SetBitRate(wBitrate);
    }
    
	switch( CurState() ) 
	{
	case NORMAL:
		{
			BOOL32 bCreatSendObj = FALSE;
            TVideoEncParam atVidParam[MAXNUM_MPU_H_VOUTPUT];
            memset(&atVidParam[0], 0, sizeof(TVideoEncParam));
 
			// fxh 目前mcu采用标准4CIF(704* 576), 而对于MPEG4,采取D1(720 * 576)
			for(u8 byOutChnlIdx = 0; byOutChnlIdx < byOutPutNum; byOutChnlIdx++)
			{
				if (MEDIA_TYPE_MP4 == atParm[byOutChnlIdx].GetVidType() &&
					704 == atParm[byOutChnlIdx].GetWidth() &&
					576 == atParm[byOutChnlIdx].GetHeight())
				{
					atParm[byOutChnlIdx].SetResolution(720, 576);
				}	
				
			    g_cMpuBasApp.GetDefaultParam( atParm[byOutChnlIdx].GetVidType(), atVidParam[byOutChnlIdx] );

				//设置第一路编码参数
				atVidParam[byOutChnlIdx].m_wVideoHeight = atParm[byOutChnlIdx].GetHeight();
				atVidParam[byOutChnlIdx].m_wVideoWidth  = atParm[byOutChnlIdx].GetWidth();
				atVidParam[byOutChnlIdx].m_wBitRate     = atParm[byOutChnlIdx].GetBitrate(); 
				atVidParam[byOutChnlIdx].m_byFrameRate  = atParm[byOutChnlIdx].GetFrameRate();

				//zbq[09/04/2008] FIXME: 临时修正，稍后调整到MCU
				//依然保留？
	// 			if ( atVidEncPar[0].m_wVideoWidth == 720 &&
	// 				 atVidEncPar[0].m_wVideoHeight == 576 )
	// 			{
	// 				atVidEncPar[0].m_wVideoWidth = 704;
	// 			}


				if ( atVidParam[byOutChnlIdx].m_byFrameRate == 30 )
				{
					atVidParam[byOutChnlIdx].m_byFrameRate = 25;
				}

				//zbq[10/10/2008] 根据分辨率调整最大关键帧 间隔
				if ( atVidParam[byOutChnlIdx].m_wVideoWidth == 1280 &&
					 atVidParam[byOutChnlIdx].m_wVideoHeight == 720 )
				{
					atVidParam[byOutChnlIdx].m_byMaxKeyFrameInterval = 3000;
				}
				else if( (atVidParam[byOutChnlIdx].m_wVideoWidth == 704 &&
						  atVidParam[byOutChnlIdx].m_wVideoHeight == 576) ||
						 (atVidParam[byOutChnlIdx].m_wVideoWidth == 720 &&
						  atVidParam[byOutChnlIdx].m_wVideoHeight == 576) )
				{
					atVidParam[byOutChnlIdx].m_byMaxKeyFrameInterval = 1000;
				}
				else if ( atVidParam[byOutChnlIdx].m_wVideoWidth == 352 &&
						  atVidParam[byOutChnlIdx].m_wVideoHeight == 288 )
				{
					atVidParam[byOutChnlIdx].m_byMaxKeyFrameInterval = 500;
				}

				//调整量化参数
				if (MEDIA_TYPE_H264 == atParm[byOutChnlIdx].GetVidType())
				{
					//CIF以上给51－10
					if (atParm[byOutChnlIdx].GetHeight() > 288)
					{
						atVidParam[byOutChnlIdx].m_byMaxQuant = 51;
						atVidParam[byOutChnlIdx].m_byMinQuant = 10;
					}
					else
					{
						atVidParam[byOutChnlIdx].m_byMaxQuant = 45;
						atVidParam[byOutChnlIdx].m_byMinQuant = 20;
					}
				}
				else
				{
					atVidParam[byOutChnlIdx].m_byMaxQuant = 31;
					atVidParam[byOutChnlIdx].m_byMinQuant = 3;
				}

				mpulog( MPU_INFO, "[ProcChangeAdptCmd]Change Video Enc Param: %u, W*H: %dx%d, Bitrate.%dKbps, FrmRate.%d, MaxKeyFrmInt.%d\n",
								   atParm[byOutChnlIdx].GetVidType(), 
								   atVidParam[byOutChnlIdx].m_wVideoWidth, 
								   atVidParam[byOutChnlIdx].m_wVideoHeight,
								   atVidParam[byOutChnlIdx].m_wBitRate,
								   atVidParam[byOutChnlIdx].m_byFrameRate,
								   atVidParam[byOutChnlIdx].m_byMaxKeyFrameInterval );
			}            

            
            {
				for (u8 byPriOut = 0; byPriOut < byOutPutNum;  byPriOut++)
				{
					if (atParm[byPriOut].GetVidType() != 
												m_tChnInfo.m_tChnStatus.GetOutputVidParam(byPriOut)->GetVidType())
					{
						bCreatSendObj = TRUE;
						OspPrintf(TRUE, FALSE, "[ProcChangeAdptCmd] CreatSendObj again!\n");
						break;
					}
				}

                //zbq[10/09/2008] 容错考虑
                BOOL bRet = m_pAdpCroup->SetVideoEncParam( atVidParam, byChnIdx, byOutPutNum, bCreatSendObj/*!(m_pAdpCroup->IsStart())*/ );  
                if ( TRUE != bRet )
			    {
				    mpulog( MPU_INFO, "Change Video Enc Param failed!(%d)\n", bRet );
                    return;
			    }

                //音频中转的Net参数设置
                if (!m_pAdpCroup->SetAudioParam( byAudDecType, byChnIdx ))
				{
					mpulog( MPU_CRIT, "[ProcChangeAdptCmd]SetAudioParam failed\n");
				}

				// 记录通道状态
	    		//THDAdaptParam* ptAdpParam = m_tChnInfo.m_tChnStatus.GetOutputVidParam(0);
				
				// 记录通道状态
				for (u8 byOut = 0; byOut < byOutPutNum;  byOut++)
				{
					THDAdaptParam* ptAdpParam = m_tChnInfo.m_tChnStatus.GetOutputVidParam(byOut);
					if (NULL != ptAdpParam)
					{
						ptAdpParam->SetAudType( atParm[byOut].GetAudType() );
						ptAdpParam->SetVidType( atParm[byOut].GetVidType() );
						ptAdpParam->SetBitRate( atParm[byOut].GetBitrate() );
						ptAdpParam->SetResolution( atParm[byOut].GetWidth(), atParm[byOut].GetHeight() );
						ptAdpParam->SetFrameRate( atParm[byOut].GetFrameRate() );
					}
				}

// 	    		if ( NULL != ptAdpParam )
// 				{
// 		    		ptAdpParam->SetAudType( atParm[0].GetAudType() );
// 		    		ptAdpParam->SetVidType( atParm[0].GetVidType() );
// 		    		ptAdpParam->SetBitRate( atParm[0].GetBitrate() );
// 			    	ptAdpParam->SetResolution( atParm[0].GetWidth(), atParm[0].GetHeight() );
// 					ptAdpParam->SetFrameRate( atParm[0].GetFrameRate() );
// 				}
// 
//                 THDAdaptParam* ptAdpParam1 = m_tChnInfo.m_tChnStatus.GetOutputVidParam(1);
//                 
//                 if ( NULL != ptAdpParam1 )
//                 {
//                     ptAdpParam1->SetAudType( atParm[1].GetAudType() );
//                     ptAdpParam1->SetVidType( atParm[1].GetVidType() );
//                     ptAdpParam1->SetBitRate( atParm[1].GetBitrate() );
//                     ptAdpParam1->SetResolution( atParm[1].GetWidth(), atParm[1].GetHeight() );
// 					ptAdpParam1->SetFrameRate( atParm[1].GetFrameRate() );
// 				}

				OspPrintf(TRUE, FALSE, "ActivePayLoad:%d, RealPayLoad:%d!\n", tDoublePayloadVideo.GetActivePayload(), tDoublePayloadVideo.GetRealPayLoad());

				u8 byEncryptMode = tMediaEncryptVideo.GetEncryptMode();
				u8 abySndActPt[MAXNUM_MPU_H_VOUTPUT];
				u8 byLoop = 0;
				if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
				{
					mpulog( MPU_CRIT, "CONF_ENCRYPTMODE_NONE == byEncryptMode\n" );
					for(;byLoop < byOutPutNum; byLoop ++)
					{
						abySndActPt[byLoop] = atParm[byLoop].GetVidActiveType();
						if(atParm[byLoop].GetVidType() == MEDIA_TYPE_H263 || atParm[byLoop].GetVidType() == MEDIA_TYPE_MP4 )
						{
							abySndActPt[byLoop] = 0;
						}
						OspPrintf(TRUE, FALSE, "abySndActPt[%d] is: %u\n", byLoop, abySndActPt[byLoop]);
					}
					m_pAdpCroup->SetKeyandPT( NULL, 0, 0,
											  abySndActPt, byOutPutNum,
											  tDoublePayloadVideo.GetActivePayload(), tDoublePayloadVideo.GetRealPayLoad());


// 					m_pAdpCroup->SetKeyandPT( NULL, 0, 0,
// 											  tDoublePayloadVideo.GetActivePayload(), tDoublePayloadVideo.GetRealPayLoad(),
// 											  abySndActPt[0], abySndActPt[1] );
				}
				else if ( CONF_ENCRYPTMODE_DES == byEncryptMode || CONF_ENCRYPTMODE_AES == byEncryptMode )
				{
					u8 abyKeyBuf[MAXLEN_KEY];
					s32 byKenLen = 0;
					tMediaEncryptVideo.GetEncryptKey( abyKeyBuf, &byKenLen );
					if ( CONF_ENCRYPTMODE_DES == byEncryptMode )
					{
						byEncryptMode = DES_ENCRYPT_MODE;
					}
					else
					{
						byEncryptMode = AES_ENCRYPT_MODE;
					}
					//由于SetKeyandPT接口调整，这里临时组织参数
					memset(abySndActPt, 0, sizeof(abySndActPt));
					for (byLoop = 0; byLoop < byOutPutNum; byLoop++)
					{
						abySndActPt[byLoop] = atParm[byLoop].GetVidActiveType();
					}
					m_pAdpCroup->SetKeyandPT( abyKeyBuf, byKenLen, byEncryptMode, 
											  abySndActPt, byOutPutNum,
											  tDoublePayloadVideo.GetActivePayload(), 
										      tDoublePayloadVideo.GetRealPayLoad());

// 					m_pAdpCroup->SetKeyandPT( abyKeyBuf, byKenLen, byEncryptMode, 
// 											  tDoublePayloadVideo.GetActivePayload(), tDoublePayloadVideo.GetRealPayLoad(),
// 											  atParm[0].GetVidActiveType(), atParm[1].GetVidActiveType() );
				}

				//设置加密
				if( !m_pAdpCroup->SetVidEncryptKey( tMediaEncryptVideo, byOutPutNum) )
				{
					mpulog(MPU_CRIT, "[ProcChangeAdptCmd] SetVidEncryptKey failed\n");
				}
               
                //设置加密
                //m_pAdpCroup->SetSecVidEncryptKey( tMediaEncryptVideo );
                
                //设置动态载荷
                if (m_pAdpCroup->SetVideoActivePT( tDoublePayloadVideo.GetActivePayload(), tDoublePayloadVideo.GetRealPayLoad() ) )
				{
					mpulog(MPU_CRIT, "[ProcChangeAdptCmd] SetVideoActivePT failed\n");
				}

				if ( m_pAdpCroup->IsStart() == FALSE )
				{
					if( !m_pAdpCroup->StartAdapter( TRUE ) )
					{
						mpulog(MPU_CRIT, "[ProcChangeAdptCmd] StartAdapter failed\n");
						return;
					}
				}

                // 重新设置重传
				BOOL32 bIsNeedPrs = FALSE;
				for (u8 byEncIdx = 0; byEncIdx < byOutPutNum; byEncIdx++)
				{
					if (atParm[byEncIdx].IsNeedbyPrs())
					{
						bIsNeedPrs = TRUE;
						break;
					}
				}

                TRSParam tNetRSParam;
				OspPrintf(TRUE, FALSE, "[ProcChangeAdptCmd] IsNeedPrs:%d!\n", bIsNeedPrs);
                if ( bIsNeedPrs/*atParm[0].IsNeedbyPrs()*/ )		//是否需要包重传？ 1: 重传, 0: 不重传
                {		
                    mpulog( MPU_INFO, "Bas is needed by Prs!\n" );
                    tNetRSParam.m_wFirstTimeSpan  = g_cMpuBasApp.m_tPrsTimeSpan.m_wFirstTimeSpan;
                    tNetRSParam.m_wSecondTimeSpan = g_cMpuBasApp.m_tPrsTimeSpan.m_wSecondTimeSpan;
                    tNetRSParam.m_wThirdTimeSpan  = g_cMpuBasApp.m_tPrsTimeSpan.m_wThirdTimeSpan;
                    tNetRSParam.m_wRejectTimeSpan = g_cMpuBasApp.m_tPrsTimeSpan.m_wRejectTimeSpan;
                    if( !m_pAdpCroup->SetNetSendFeedbackVideoParam( 2000, byOutPutNum, TRUE ) )
					{
						mpulog(MPU_CRIT, "[ProcChangeAdptCmd]SetNetSendFeedbackVideoParam failed\n");
					}
                    //m_pAdpCroup->SetNetSecSendFeedbackVideoParam( 2000, TRUE );
                    if( !m_pAdpCroup->SetNetRecvFeedbackVideoParam( tNetRSParam, TRUE ) )
					{
						mpulog(MPU_CRIT, "[ProcChangeAdptCmd]SetNetRecvFeedbackVideoParam failed\n");
					}
                }
                else
                {
                    mpulog( MPU_INFO, "Bas is not needed by Prs!\n" );
                    tNetRSParam.m_wFirstTimeSpan  = 0;
                    tNetRSParam.m_wSecondTimeSpan = 0;
                    tNetRSParam.m_wThirdTimeSpan  = 0;
                    tNetRSParam.m_wRejectTimeSpan = 0;
                    if (!m_pAdpCroup->SetNetSendFeedbackVideoParam( 2000, byOutPutNum, FALSE ))
					{
						mpulog(MPU_CRIT, "[ProcChangeAdptCmd]SetNetSendFeedbackVideoParam failed\n");
					}
                    //m_pAdpCroup->SetNetSecSendFeedbackVideoParam( 2000, FALSE );
                    if (!m_pAdpCroup->SetNetRecvFeedbackVideoParam( tNetRSParam, FALSE ))
					{
						mpulog(MPU_CRIT, "[ProcChangeAdptCmd]SetNetRecvFeedbackVideoParam failed\n");
					}
			    }
	
				// 平滑发送
				// [pengjie 2010/12/30] 平滑逻辑整理
				m_pAdpCroup->SetSmoothSendRule( g_cMpuBasApp.m_bIsUseSmoothSend );
            }   
			
            cServMsg.SetMsgBody( (u8*)&g_cMpuBasApp.m_tEqp, sizeof(g_cMpuBasApp.m_tEqp) );

			//CMD msg no reply
            //SendMsgToMcu(cServMsg.GetEventId() + 2, cServMsg);

            //change不需要发状态上告，因为已经是running
            //SendChnNotif();

			//[nizhijun 2011/03/21] BAS参数改变需要隔1秒检测关键帧请求
			KillTimer( EV_BAS_NEEDIFRAME_TIMER );
			mpulog(MPU_INFO, "[ProcChangeAdptCmd]reset EV_BAS_NEEDIFRAME_TIMER from %d to: %d\n", 
				g_cMpuBasApp.GetIframeInterval(), CHECK_IFRAME_INTERVAL);
			m_byIframeReqCount = 0;
			SetTimer( EV_BAS_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL );   // 定时查询适配组是否需要关键帧
		}
		break;

	default:
        cServMsg.SetErrorCode( ERR_BAS_CHNOTREAD );
        cServMsg.SetMsgBody( (u8*)&g_cMpuBasApp.m_tEqp,sizeof(g_cMpuBasApp.m_tEqp) );

		//CMD msg no reply
        //SendMsgToMcu(cServMsg.GetEventId() + 2, cServMsg);
        mpulog(MPU_CRIT, "the chn.%d is not runing ,refuse change adap req\n", byChnIdx );		
		break;
	}

    return;
}

/*====================================================================
	函数  : ProcStopAdptReq
	功能  : 停止适配
	输入  : 
	输出  : 无
	返回  : 无
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人      修改内容
  2008/12/09  4.5         周文        创建
====================================================================*/
void CMpuBasInst::ProcStopAdptReq( CServMsg& cServMsg )
{
    u8 byChnIdx = (u8)GetInsID() - 1;
    mpulog( MPU_INFO, "[ProcStopAdptReq] Stop adp, The channel is %d\n", byChnIdx );

    BOOL wRet;
    CConfId    m_cTmpConfId = cServMsg.GetConfId();

	if ( !(m_tChnInfo.m_cChnConfId == m_cTmpConfId) )
	{
		cServMsg.SetErrorCode( ERR_BAS_OPMAP );
		SendMsgToMcu( cServMsg.GetEventId() + 2, cServMsg );
		mpulog( MPU_CRIT, "[ProcStopAdptReq] Channel.%d unmatch conf id in stop adpt req\n", byChnIdx );
		return;
	}

	switch( CurState() )
	{
	case NORMAL:
        wRet = m_pAdpCroup->StopAdapter();
		
		// [pengjie 2010/12/29] 停止适配时清除当前的平滑发送规则
		m_pAdpCroup->SetSmoothSendRule( FALSE );
		
		//清空音频缓存队列
		if (NULL != m_ptFrmQueue)
		{
			m_ptFrmQueue->Clear();
		}		
        if( TRUE != wRet )
        {	
			cServMsg.SetErrorCode( ERR_BAS_OPMAP );
			SendMsgToMcu( cServMsg.GetEventId() + 2, cServMsg );
			mpulog( MPU_CRIT, "Remove channel %d failed when stop adpt, error: %d\n",
                        byChnIdx, wRet );
			return;				           
        }        		
		memset(&m_tChnInfo.m_tChnStatus, 0, sizeof(THDBasVidChnStatus));
		cServMsg.SetMsgBody( (u8*)&g_cMpuBasApp.m_tEqp, sizeof(g_cMpuBasApp.m_tEqp) );
		SendMsgToMcu( cServMsg.GetEventId() + 1, cServMsg );
        NextState( (u32)READY );
        
        SendChnNotif();
		break;

    case READY:
        {
            cServMsg.SetErrorCode( ERR_BAS_CHREADY );
            cServMsg.SetMsgBody( (u8*)&g_cMpuBasApp.m_tEqp,sizeof(g_cMpuBasApp.m_tEqp) );
            SendMsgToMcu( cServMsg.GetEventId() + 2, cServMsg );
            //return;
        }
		break;

	default:
		cServMsg.SetErrorCode( ERR_BAS_CHNOTREAD );
		cServMsg.SetMsgBody( (u8*)&g_cMpuBasApp.m_tEqp, sizeof(g_cMpuBasApp.m_tEqp) );
		SendMsgToMcu( cServMsg.GetEventId() + 2, cServMsg );
		break;
	}

    return;
}

/*====================================================================
	函数  : ProcFastUpdatePicCmd
	功能  : 
	输入  : 
	输出  : 无
	返回  : 无
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人      修改内容
  2008/12/09  4.5         周文        创建
====================================================================*/
void CMpuBasInst::ProcFastUpdatePicCmd( /*CServMsg& cServMsg*/ )
{
	printf("[ProcFastUpdatePicCmd for Chn.%d !\n", GetInsID() - 1 );
    mpulog(MPU_INFO, "[ProcFastUpdatePicCmd for Chn.%d !\n", GetInsID() - 1 );

    //u8 byChnIdx = cServMsg.GetChnIndex();
    u32 dwTimeInterval = OspClkRateGet();
    u32 dwLastFUPTick = m_tChnInfo.m_dwLastFUPTick;
    
	//根据模式解析参数输出个数
	u8 byOutPutNum = 0;
	if (TYPE_MPUBAS == g_cMpuBasApp.m_byWorkMode)
	{
		byOutPutNum = MAXNUM_VOUTPUT;
	}
	else if(TYPE_MPUBAS_H == g_cMpuBasApp.m_byWorkMode)
	{
		byOutPutNum = MAXNUM_MPU_H_VOUTPUT;
	}
	else
	{
		OspPrintf(TRUE, FALSE, "[ProcStartAdptReq] unexpected mpu workmode:%d!\n", g_cMpuBasApp.m_byWorkMode);
		return;
	}

    u32 dwCurTick = OspTickGet();	
    if ( dwCurTick - dwLastFUPTick > dwTimeInterval )
    {
        m_tChnInfo.m_dwLastFUPTick = dwCurTick;
        
		if ( NULL != m_pAdpCroup )
		{			
			// 目前请求关键帧采取同时请求两路
			for (u8 byIdx = 0; byIdx < byOutPutNum; byIdx++)
			{
				if (!m_pAdpCroup->SetFastUpdata(byIdx))
				{
					mpulog(MPU_CRIT, "SetFastUpdata(%u) failed \n", byIdx);
				}
			}
//     		m_pAdpCroup->SetFastUpdata(0);
// 			m_pAdpCroup->SetFastUpdata(1);
		}
		else
		{
			printf("m_pAdpGroup for Chnnl.%d cannot set fastupdate due to its not ready\n!", GetInsID()-1 );
			mpulog(MPU_CRIT, "m_pAdpGroup for Chnnl.%d cannot set fastupdate due to its not ready\n!", GetInsID()-1 );
		}

    }
    else
    {
		printf("[ProcFastUpdatePicCmd tick intval is %d - %d !\n", dwCurTick , dwLastFUPTick );
        mpulog(MPU_INFO, "[ProcFastUpdatePicCmd tick intval is %d - %d !\n", dwCurTick , dwLastFUPTick );
    }

    return;
}

/*====================================================================
	函数  : ProcTimerNeedIFrame
	功能  : 
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
    日  期         版本        修改人       修改内容
    2008/12/09     4.5         周文         创建
====================================================================*/
void CMpuBasInst::ProcTimerNeedIFrame()
{
    CServMsg cServMsg;    
    TAdapterChannelStatis tAdapterChannelStatis;    
    memset( &tAdapterChannelStatis, 0, sizeof(TAdapterChannelStatis) );
    static u8 byPrintCount = 1;

	KillTimer(EV_BAS_NEEDIFRAME_TIMER);
    mpulog(MPU_DETAIL, "[ProcTimerNeedIFrame] Chn.%d IFrm check timer!\n", GetInsID()-1);

    if( byPrintCount > 20)
    {
        byPrintCount = 1;
    }
	if ( m_pAdpCroup && m_pAdpCroup->IsStart() )
	{      
        //songkun,20110630,Iframe请求TimerOut策略调整  
        if ( 0 == m_byIframeReqCount )
		{
            if (!m_pAdpCroup->GetVideoChannelStatis( 0, tAdapterChannelStatis ))
            {
                mpulog(MPU_CRIT, "[ProcTimerNeedIFrame] GetVideoChannelStatis failed!\n");
            }
            if( tAdapterChannelStatis.m_bVidCompellingIFrm )
            {
                cServMsg.SetConfId( m_tChnInfo.m_cChnConfId );
                cServMsg.SetChnIndex( GetInsID()-1 );
                cServMsg.SetSrcSsnId( g_cMpuBasApp.m_tCfg.byEqpId );
                SendMsgToMcu( BAS_MCU_NEEDIFRAME_CMD, cServMsg );
                m_byIframeReqCount = (u8)(g_cMpuBasApp.GetIframeInterval() / CHECK_IFRAME_INTERVAL);
    		    mpulog( MPU_INFO, "[ProcTimerNeedIFrame]Bas channel:%d request iframe!!,next time is %u\n", GetInsID() - 1,m_byIframeReqCount);
            }
            else
            {
                mpulog( MPU_INFO, "[ProcTimerNeedIFrame]No iframe request for bas channel :%d\n", GetInsID() - 1);
            }
        }        
	}
    if ( 0 != m_byIframeReqCount)
    {
        --m_byIframeReqCount;                                    
    }
    mpulog(MPU_INFO, "[ProcTimerNeedIFrame]set EV_BAS_NEEDIFRAME_TIMER : %d\n", CHECK_IFRAME_INTERVAL);
    SetTimer( EV_BAS_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL);   // 定时查询适配组是否需要关键帧

    return;
}
/*=============================================================================
函 数 名： ClearInstStatus
功    能： 清空本实例保存的状态信息
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2008/12/09     4.5      周文        创建
=============================================================================*/
void CMpuBasInst::ClearInstStatus( void )
{
    //CServMsg cServMsg;
    // 先停止适配
    u8 byInitNum = 0;
    if(!GetBasInitChnlNumAcd2WorkMode(g_cMpuBasApp.m_byWorkMode, byInitNum))
    {
        OspPrintf(TRUE, FALSE, "[ClearInstStatus] GetBasInitChnlNumAcd2WorkMode failed!\n");
        return;
    }	
    
    u8 byLoop;
    for ( byLoop = 1; byLoop <= byInitNum/*MAXNUM_MPUBAS_CHANNEL*/; byLoop++ )
    {
        post( MAKEIID(GetAppID(), byLoop), EV_BAS_QUIT );
    }
    
    NEXTSTATE( (u32)IDLE );   // DAEMON 实例进入空闲状态
    g_cMpuBasApp.m_byRegAckNum = 0;
    g_cMpuBasApp.m_dwMpcSSrc = 0;
    return;
}

/*====================================================================
	函数  : ProcModSndAddr
	功能  : 
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
    日  期         版本        修改人       修改内容
    2008/12/09     4.5         付秀华         创建
====================================================================*/
void CMpuBasInst::ProcModSndAddr()
{
	if (m_pAdpCroup != NULL)
	{
		m_pAdpCroup->ModNetSndIpAddr(g_cMpuBasApp.m_dwMcuRcvIp);
	}

}
/*=============================================================================
  函 数 名： StatusShow
  功    能： 显示当前通道状态
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人      修改内容
  2008/12/09     4.5      周文        创建
=============================================================================*/
void CMpuBasInst::StatusShow( void )
{
    if ( GetInsID() != CInstance::DAEMON)
    {
        if(m_pAdpCroup)
        {
            m_pAdpCroup->ShowChnInfo();
        }
        else
        {
            OspPrintf(TRUE, FALSE, "m_pAdpCroup is NULL\n");
        }
    }
    
    //OspPrintf( TRUE, FALSE, "[StatusShow] g_cBasApp.m_byMauType.%d\n", g_cBasApp.GetType());
    OspPrintf(TRUE, FALSE, "[StatusShow]CMpuBasInst:%d status:%d\n",GetInsID(),CurState());
    OspPrintf( TRUE, FALSE, "[StatusShow] server for conf:");

    m_tChnInfo.m_cChnConfId.Print();
    
    m_ptFrmQueue->Print();
    return;
}

BOOL32 CMpuBasInst::GetBasInitChnlNumAcd2WorkMode(u8 byWorkMode, u8 &byInitChnlNum)
{
	byInitChnlNum = 0;
	switch (byWorkMode)
	{
	case TYPE_MPUBAS:
		byInitChnlNum = MAXNUM_MPUBAS_CHANNEL;
		break;
	case TYPE_MPUBAS_H:
		byInitChnlNum = MAXNUM_MPUBAS_H_CHANNEL;
		break;
	default:
		OspPrintf(TRUE, FALSE, "[GetBasInitChnlNumAcd2WorkMode] unexpected mpu workmode:%d!\n", byWorkMode);
		break;
	}
	return 0 != byInitChnlNum ;
}

BOOL32 CMpuBasInst::GetBasOutChnlNumAcd2WorkMode(u8 byWorkMode, u8 &byOutChnlNum)
{
	byOutChnlNum = 0;
	switch (byWorkMode)
	{
	case TYPE_MPUBAS:
		byOutChnlNum = MAXNUM_VOUTPUT;
		break;
	case TYPE_MPUBAS_H:
		byOutChnlNum = MAXNUM_MPU_H_VOUTPUT;
		break;
	default:
		OspPrintf(TRUE, FALSE, "[GetBasOutChnlNumAcd2WorkMode] unexpected mpu workmode:%d!\n", byWorkMode);
		break;
	}
	return 0 != byOutChnlNum ;
}
//END OF FILE
