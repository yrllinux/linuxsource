/*****************************************************************************
   模块名      : mpu2lib
   文件名      : mpu2vmp.cpp
   相关文件    : 
   文件实现功能: mpu2lib vmp业务实现
   作者        : 周翼亮
   版本        : V4.7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期       版本        修改人      修改内容
   2011/12/07    4.7         周翼亮      创建
******************************************************************************/
#include "mpu2inst.h"
#include "evmpu2.h"
#include "mcuver.h"
#include "evmcueqp.h"
#include "boardagent.h"
#ifdef _LINUX_
#include "brdwrapper.h"
#endif
BOOL32 g_bPauseSS = FALSE;

/*====================================================================
	函数  : Clear
	功能  : 清除状态，动态内存等
	输入  : CMessage
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpInst::Clear()
{
	//1、delete new的动态内存
    u8 byLoop = 0;
    for ( ; byLoop < MAXNUM_MPU2VMP_MEMBER; byLoop++ )
    {
		if( NULL != m_pcMediaRcv[byLoop] ) 
		{
			delete m_pcMediaRcv[byLoop];
			 m_pcMediaRcv[byLoop] = NULL;
		}
       
    }
    byLoop = 0;
    for (; byLoop < m_byMaxVmpOutNum; byLoop++)
    {
		if ( NULL !=  m_pcMediaSnd[byLoop] ) 
		{
			delete  m_pcMediaSnd[byLoop];
			m_pcMediaSnd[byLoop] = NULL;
		}
        
    }
	//2、清除AddChannel数组
    byLoop = 0;
    for ( ; byLoop < MAXNUM_MPU2VMP_MEMBER; byLoop++ )
    {
        m_bAddVmpChannel[byLoop] = FALSE;
    }
	//3、清除成员变量赋值
    m_dwMcuIId         = 0;        
    m_dwMcuIIdB        = 0;       
    m_byRegAckNum      = 0;     
	m_dwMpcSSrc        = 0;      
    m_dwMcuRcvIp       = 0;      
    m_wMcuRcvStartPort = 0;
    m_byHardStyle      = 0;   
	m_wMTUSize         = 0;	
    m_emMcuEncoding  = emenCoding_GBK;
    memset(m_abChnlIframeReqed,0,sizeof(m_abChnlIframeReqed));
	memset( m_adwLastFUPTick, 0, sizeof(m_adwLastFUPTick));

	memset ( m_bUpdateMediaEncrpyt, 0 , sizeof(m_bUpdateMediaEncrpyt));
	m_cHardMulPic.Destory();
	SetIsSimulateMpu(FALSE);
	SetIsDisplayMbAlias(FALSE);
	m_byMaxVmpOutNum = MAXNUM_MPU2VMP_CHANNEL;//默认是mpu2 vmp 出9路
	m_byValidVmpOutNum = 0;
	if (IsSimulateMpu())
	{
		m_byMaxVmpOutNum = MAXNUM_MPUSVMP_CHANNEL;
		m_byValidVmpOutNum = MAXNUM_MPUSVMP_CHANNEL;
	}

	
	m_bCreateHardMulPic = FALSE;
	m_dwInitVmpMode = 0;
	m_bFastUpdate = FALSE;

       
}
/*====================================================================
	函数  : DaemonProcParamShow
	功能  : Daemon处理打印vmpparam的消息，分发给所有instance
	输入  : CMessage
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期          版本        修改人            修改内容
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpInst::DaemonProcParamShow()
{
	for (u8 byLoop = 0 ; byLoop< g_cMpu2VmpApp.m_byVmpEqpNum ;byLoop++)
	{
		CServMsg cServMsg;
		post(MAKEIID(AID_MPU2,byLoop+1 ),EV_VMP_SHOWPARAM,cServMsg.GetServMsg(),cServMsg.GetServMsgLen());
	}
}
/*====================================================================
	函数  : DaemonProcStatusShow
	功能  : Daemon处理打印status的消息，分发给所有instance
	输入  : CMessage
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期          版本        修改人            修改内容
   2011/12/07		v4.7    	zhouyiliang			create
====================================================================*/
void CMpu2VmpInst::DaemonProcStatusShow()
{
	for (u8 byLoop = 0 ; byLoop< g_cMpu2VmpApp.m_byVmpEqpNum ;byLoop++)
	{
		CServMsg cServMsg;
		post(MAKEIID(AID_MPU2,byLoop+1 ),EV_VMP_DISPLAYALL,cServMsg.GetServMsg(),cServMsg.GetServMsgLen());
	}
}

/*====================================================================
	函数  : DaemonInstanceEntry
	功能  : Daemon消息分发
	输入  : CMessage，pApp
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	11/11/17	v4.7		周翼亮			create
====================================================================*/
void CMpu2VmpInst::DaemonInstanceEntry( CMessage *const pcMsg, CApp* pApp )
{
	if (NULL == pcMsg || NULL == pApp)
	{
		ErrorLog("[DaemonInstanceEntry]wrong message recived. pcMsg is Null or pApp is Null\n ");
		return;
	}
	switch( pcMsg->event ) 
	{
		case EV_VMP_INIT:
			DaemonInit(pcMsg);
			break;
		 // 断链处理
		case OSP_DISCONNECT:
			DaemonProcDisconnect( pcMsg );
			break;
		// 连接
		case EV_VMP_CONNECT_TIMER:
			DaemonProcConnectTimeOut( TRUE );
			break;
		//连接备板
		case EV_VMP_CONNECT_TIMERB:
			DaemonProcConnectTimeOut( FALSE );
			break;
		//打印状态	
		 case EV_VMP_DISPLAYALL:
			DaemonProcStatusShow();
			break;
		//打印参数
 		case EV_VMP_SHOWPARAM:
 			DaemonProcParamShow();
			break;
			default:
			break;
	}
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
	日  期          版本        修改人        修改内容
	11/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpInst::InstanceEntry( CMessage *const pMsg )
{
    if ( NULL == pMsg )
    {
		ErrorLog("[InstanceEntry]wrong message recived. pMsg is Null \n");
        return;
    }

	if ( pMsg->event >=EV_VMP_NEEDIFRAME_TIMER && (pMsg->event< EV_VMP_NEEDIFRAME_TIMER+MAXNUM_MPU2VMP_MEMBER) )
	{
		//检测VMP是否需要关键帧
		MsgTimerNeedIFrameProc(pMsg);
		return;
	}
	else
    {
		KeyLog("Message %u(%s) received in InstanceEntry\n", pMsg->event, ::OspEventDesc(pMsg->event) );
        //KeyLog( "Message %u(%s) received in InstanceEntry\n", pMsg->event, ::OspEventDesc(pMsg->event) );
    }
    switch ( pMsg->event )
    {
    //--------内部消息------------
   
	// 注册
    case EV_VMP_REGISTER_TIMER:
        ProcRegisterTimeOut( TRUE );
        break;
	//备板注册
    case EV_VMP_REGISTER_TIMERB:
        ProcRegisterTimeOut( FALSE );
        break;
	//OSP断链
	case OSP_DISCONNECT:
		Disconnect();
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
	//打印参数
 	case EV_VMP_SHOWPARAM:
 		ParamShow();
		break;
	//更改编码帧率
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
	//更改mcu的接收地址
	case MCU_EQP_MODSENDADDR_CMD:
		MsgModMcuRcvAddrProc( pMsg );
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


    // 更改编码参数
    case MCU_VMP_UPDATAVMPENCRYPTPARAM_REQ:
        MsgUpdateVmpEncryptParamProc( pMsg );
        break;

    // 强制关键帧	//FIXME: 优化成带参数(对应4出)
    case MCU_VMP_FASTUPDATEPIC_CMD:
        MsgFastUpdatePicProc( pMsg );
        break;

		//调整合成成员别名
	case MCU_VMP_CHANGEMEMALIAS_CMD:
		ProcChangeMemAliasCmd(pMsg);
		break;


	// 取主备倒换状态
	case TIME_GET_MSSTATUS:
	case MCU_EQP_GETMSSTATUS_ACK:
		ProcGetMsStatusRsp( pMsg );
		break;
	//打印高级配置信息
	case EV_VMP_SHOWDEBUG:
		ShowVMPDebugInfo();
		break;
	case MCU_VMP_ADDREMOVECHNNL_CMD:
		MsgAddRemoveRcvChnnl(pMsg);
		break;
    // 未定义的消息
    default:
        //log( LOGLVL_EXCEPTION, "[Error]Unexcpet Message %u(%s).\n", pMsg->event, ::OspEventDesc(pMsg->event) );
		ErrorLog("[Error]Unexcpet Message %u(%s).\n", pMsg->event, ::OspEventDesc(pMsg->event) );
        break;
    }
}

/*====================================================================
	函数  : DaemonInit
	功能  : Daemon的初始化
	输入  : CMessage
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期          版本        修改人        修改内容
	11/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpInst::DaemonInit(CMessage* const pcMsg)
{
	//1、参数校验（合法性，错误的配置）
	if (NULL == pcMsg)
	{
		ErrorLog("[DaemonInit]invalid param pcMsg:Null\n");
		return;
	}
	TMpu2Cfg tMpu2Cfg;
	tMpu2Cfg = *(TMpu2Cfg*)pcMsg->content;
	u8 byEqpNum = tMpu2Cfg.GetMpu2EqpNum();
	if (byEqpNum == 0 || byEqpNum > MAXNUM_MPU2_EQPNUM)
	{
		
		ErrorLog("[DaemonInit]Error eqpNum:%d\n",byEqpNum);
		return;
	}
	NEXTSTATE((u32)DAEMON_INIT);
	//2、解析eqp配置发给对应的instance（一个instance对应一个eqp配置）
	BOOL32 bHasConnectIp = FALSE;
	BOOL32 bHasConnectIpB = FALSE;
	for (u8 byLoop = 0; byLoop < byEqpNum; byLoop++)
	{

		TEqpBasicCfg tCfg = tMpu2Cfg.GetMpu2EqpCfg(byLoop);
		KeyLog("[DaemonInit]eqpNo:%d 's config:eqpId:%d,eqpRcvStartPort:%d\n",byLoop,tCfg.byEqpId,tCfg.wRcvStartPort);
		post(MAKEIID(AID_MPU2,byLoop+1),EV_VMP_INIT,(u8*)&tCfg,sizeof(TEqpBasicCfg));
		if (0 != tCfg.dwConnectIP) 
		{
			bHasConnectIp = TRUE;
		}
		if (0 != tCfg.dwConnectIpB) 
		{
			bHasConnectIpB = TRUE;
		}
		
	}
	//平滑发送规则，提升本线程的优先级
#ifdef _LINUX_

	int nRet = SsInit();
	if ( nRet == 0 )
	{
		ErrorLog("SsInit failed!\n");
	}

#endif


	//connect mcu同时注册断链消息
	if (bHasConnectIp)
	{
		 SetTimer( EV_VMP_CONNECT_TIMER, MPU_CONNETC_TIMEOUT );
	}
    if (bHasConnectIpB) 
	{
		SetTimer( EV_VMP_CONNECT_TIMERB, MPU_CONNETC_TIMEOUT);
    }

	

}

/*====================================================================
	函数  : DaemonProcDisconnect
	功能  : Daemon处理dsiconnect消息
	输入  : CMessage
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期          版本        修改人        修改内容
	11/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpInst::DaemonProcDisconnect( CMessage * const pMsg )
{
	//1、参数校验
	if ( NULL == pMsg )  
    {
	    ErrorLog("[DaemonProcDisconnect]disconnect message's pointer is Null\n" );
        return;
    }
	//2、解析断链的node，断开连接
    u32 dwNode = *(u32*)pMsg->content;

    if ( INVALID_NODE != dwNode )
    {
        OspDisconnectTcpNode( dwNode );
    } 
	//3、重起connect的timer连接
    if( dwNode == g_cMpu2VmpApp.m_dwMcuNode ) // 断链
    {
		KeyLog("[DaemonProcDisconnect] McuNode.A disconnect\n" );
        g_cMpu2VmpApp.m_dwMcuNode = INVALID_NODE;
        m_dwMcuIId  = 0;
        SetTimer( EV_VMP_CONNECT_TIMER, MPU_CONNETC_TIMEOUT );
    }
    else if ( dwNode == g_cMpu2VmpApp.m_dwMcuNodeB )
    {
		KeyLog("[DaemonProcDisconnect] McuNode.B disconnect\n" );
        g_cMpu2VmpApp.m_dwMcuNodeB = INVALID_NODE;
        m_dwMcuIIdB  = 0;
        SetTimer( EV_VMP_CONNECT_TIMERB, MPU_CONNETC_TIMEOUT );
    }
	//4、将disconnect消息分发给各个instance，每个外设做对应的disconnect处理
	for (u8 byLoop = 0; byLoop < g_cMpu2VmpApp.m_byVmpEqpNum;byLoop++)
	{

		post(MAKEIID(AID_MPU2,byLoop+1),OSP_DISCONNECT,pMsg,pMsg->length);
	}
	//5、状态转换
	if (INVALID_NODE == g_cMpu2VmpApp.m_dwMcuNode && INVALID_NODE == g_cMpu2VmpApp.m_dwMcuNodeB) 
	{
		NEXTSTATE((u32)DAEMON_IDLE);
	}
	
}



/*====================================================================
	函数  : StatusShow
	功能  : 打印当前编码状态
	输入  : 
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期          版本        修改人        修改内容
	11/12/07		v1.0		zhouyiliang			create
====================================================================*/
void CMpu2VmpInst::StatusShow()
{

    TMulPicStatus tMulPicStatus;
//    TMulPicStatis tMulPicStatis;
    memset(&tMulPicStatus, 0, sizeof(tMulPicStatus));
//    memset(&tMulPicStatis, 0, sizeof(tMulPicStatis));

    m_cHardMulPic.GetStatus(0, tMulPicStatus);
//    m_cHardMulPic.GetStatis(tMulPicStatis);


    StaticLog( "\t===========The state of VMP is below CurEqpId:(%d)===============\n",m_tEqp.GetEqpId());

	StaticLog("The MTU Size is : %d\n", m_wMTUSize );

    StaticLog("  IDLE =%d, NORMAL=%d, RUNNING=%d, VMP Current State =%d, CreateHardMulPic:%s\n",
		IDLE, NORMAL, RUNNING, CurState(),m_bCreateHardMulPic?("SUCCEED"):"FAILED");

    StaticLog( "\t===========The status of VMP is below===============\n");
    StaticLog( "Merge=%d\t Style=%d\t CurChannel=%d\n",
                          tMulPicStatus.bMergeStart,
                          tMulPicStatus.byType,
                          tMulPicStatus.byCurChnNum);

    StaticLog( "\t-------------The Video Encoding Params--------------\n");
	for ( u8 byLoop =0; byLoop < m_byValidVmpOutNum;byLoop++ )
	{
		StaticLog( "\t-------------OutCHNNL:%d--------------\n",byLoop);
		m_cHardMulPic.GetStatus(byLoop, tMulPicStatus);
		StaticLog( "\t EncType=%d\t ComMode=%d\t KeyFrameInter=%d\t MaxQuant=%d\t MinQuant=%d\t\n",
			tMulPicStatus.tVideoEncParam.m_byEncType,
			tMulPicStatus.tVideoEncParam.m_byRcMode,
			tMulPicStatus.tVideoEncParam.m_dwMaxKeyFrameInterval,
			tMulPicStatus.tVideoEncParam.m_byMaxQuant,
			tMulPicStatus.tVideoEncParam.m_byMinQuant);
		
		StaticLog( "BitRate=%d\t FrameRate=%d\t ImageQuality=%d\t VideoWidth=%d\t VideoHeight=%d,profileType=%d\t\n",
			tMulPicStatus.tVideoEncParam.m_wBitRate,
			tMulPicStatus.tVideoEncParam.m_byFrameRate,
			tMulPicStatus.tVideoEncParam.m_byImageQulity,
			tMulPicStatus.tVideoEncParam.m_wVideoWidth,
            tMulPicStatus.tVideoEncParam.m_wVideoHeight,
			 tMulPicStatus.tVideoEncParam.m_dwProfile );
	}
	
    return;
}

/*====================================================================
	函数  : ParamShow
	功能  : 打印当前的编码参数
	输入  : 
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期          版本        修改人        修改内容
	11/12/07		v1.0		zhouyiliang			create
====================================================================*/
void CMpu2VmpInst::ParamShow(void)
{
   if ( CurState() != RUNNING   ) 
   {
		::OspPrintf(TRUE, FALSE, "\t=============CurrentState wrong ,it is :%d=============\t\n",CurState());
		return;
   }

//    if ( IsSimulateMpu() ) 
//    {
// 
// 		StaticLog( "\t=============The Param of VMP is below=============\t\n");
// 		StaticLog(  "\t------------ The first video chnnl ----------------\t\n");
// 		m_tMpuParam[0].Print();
// 		StaticLog( "\n");
// 		StaticLog(  "\t------------ The second video chnnl ----------------\t\n");
// 		m_tMpuParam[1].Print();
// 		StaticLog(  "\t----------------------------------------------------\t\n");
// 		StaticLog(  "\n");
// 		StaticLog(  "\t------------ The third video chnnl -----------------\t\n");
// 		m_tMpuParam[2].Print();
// 		StaticLog(  "\t----------------------------------------------------\t\n\n");
// 		StaticLog(  "\t------------ The fourth video chnnl -----------------\t\n");
// 		m_tMpuParam[3].Print();
// 		StaticLog(  "\t----------------------------------------------------\t\n");
// 		
//    }
//    else
   {
	   StaticLog(  "\t=============The Param of VMP is below CurEqpId:(%d)=============\t\n",m_tEqp.GetEqpId());
	   m_tVmpCommonAttrb.Print();
	   for (u8 byLoop = 0; byLoop < m_byMaxVmpOutNum;byLoop++)
	   {
		   StaticLog( "\t--------Mpu2Vmp outChannel:%d--------\t\n",byLoop);
		   	m_tParam[byLoop].Print();
	   }
	
   }

   StaticLog("==============Follows are the resbitratelimttable(after sort)===============\n");
   g_tResBrLimitTable.printTable();

    StaticLog("==============Follows are the Debug configs===============\n");
   m_tMpuVmpCfg.Print();

	

}

/*====================================================================
	函数  : Init
	功能  : 外设初始化函数
	输入  : CMessage
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期          版本        修改人        修改内容
	11/12/07		v1.0		zhouyiliang			create
====================================================================*/
void CMpu2VmpInst::Init( CMessage *const pMsg )
{
    //1、参数检验 
    if ( NULL == pMsg )
    {
        ErrorLog( "[Init] Init params cannot be NULL!\n" );
        return;
    }
	//2、解析消息体TEqpBasicCfg，直接保存到m_tCfg
   
    memcpy( &m_tCfg, pMsg->content, sizeof(TEqpBasicCfg) );

#ifdef _LINUX_
    if ( m_tCfg.dwLocalIP == m_tCfg.dwConnectIP )
    {
        ErrorLog( "[Init] m_tCfg.dwLocalIP == m_tCfg.dwConnectIP\n" );   
        return;
    }
#endif
	KeyLog("[CMpu2VmpInst::Init]EqpId:%d m_tCfg.dwConnectIP is 0x%X\n",m_tCfg.byEqpId,m_tCfg.dwConnectIP);
	KeyLog("[CMpu2VmpInst::Init]EqpId:%d m_tCfg.dwConnectIPB is 0x%X\n",m_tCfg.byEqpId,m_tCfg.dwConnectIpB);

	//3、媒控层的初始化


	TMultiPicCreate tMPCreate;

	tMPCreate.m_dwDevVersion = (u32)en_MPU2_Board;


	tMPCreate.m_dwMode = VMP_BASIC;
	if ( g_cMpu2VmpApp.m_byWorkMode == TYPE_MPU2VMP_ENHANCED)
	{
		tMPCreate.m_dwMode = VMP_ENHANCED;
	}
	m_dwInitVmpMode = tMPCreate.m_dwMode;
    s32 nRet = m_cHardMulPic.Create( tMPCreate );
	printf("[init]m_cHardMulPic.Create( tMPCreate )ret:%d\n",nRet);
    if ( HARD_MULPIC_OK == nRet )
    {
        KeyLog( "[Init] Create %s Success!\n", m_tCfg.achAlias );
		m_bCreateHardMulPic = TRUE;
    }
    else
    {
        ErrorLog( "[Init] Fail to Create. errcode.%d\n", nRet );
		m_bCreateHardMulPic = FALSE;
        return;
    }

	//4、网络层初始化
    u16 wRet = KdvSocketStartup();
    
    if ( MEDIANET_NO_ERROR != wRet )
    {
        ErrorLog( "[Init] KdvSocketStartup failed, error: %d\n", wRet );
		m_cHardMulPic.Destory();
        return ;
    }
	
	KeyLog("[Init]KdvSocketStartup ok!!!\n");

	//5、init接收对象
    u8 byLoop = 0;
    for ( ; byLoop < MAXNUM_MPU2VMP_MEMBER; byLoop++  )
    {
        m_pcMediaRcv[byLoop] = new(CKdvMediaRcv);
        if ( NULL == m_pcMediaRcv[byLoop] )
        {
            ErrorLog( "new(CKdvMediaRcv) fail!\n" );
			Clear();
            return;
        }
		if (!InitMediaRcv(byLoop))
		{
            ErrorLog( "InitMediaRcv %d fail!\n", byLoop );
			Clear();
		
			return;
		}
		
    }

	//6、new发送对象,设置发送回调
	if ( g_cMpu2VmpApp.m_byWorkMode == TYPE_MPUSVMP ) //如果是模拟mpu，最大4路出
	{
		OspSetPrompt("mpu");
		SetIsSimulateMpu(TRUE);
		m_byMaxVmpOutNum = MAXNUM_MPUSVMP_CHANNEL;
		m_byValidVmpOutNum = MAXNUM_MPUSVMP_CHANNEL;
	}

	byLoop =0;
	for (;byLoop < m_byMaxVmpOutNum;byLoop++)
	{

        m_pcMediaSnd[byLoop] = new(CKdvMediaSnd); 
        if ( NULL == m_pcMediaSnd[byLoop] )
        {
			Clear();
            ErrorLog( " new(CKdvMediaSnd) fail!\n" );
            return;
        }

    
	}
   

   
	//7、读高级配置信息置信息
	sprintf( MPULIB_CFG_FILE, "%s/mcueqp.ini", DIR_CONFIG );
    m_tMpuVmpCfg.ReadDebugValues();

	// 设置加黑边、裁边、非等比拉伸
	m_cHardMulPic.SetEncResizeMode(m_tMpuVmpCfg.GetVidSDMode());
	KeyLog("[Init] SVMP SetEncResizeMode mode is %d\n", m_tMpuVmpCfg.GetVidSDMode());
	printf("[init]end instid:%d\n",GetInsID());

    //[5/17/2013 liaokang] 
    m_cBmpDrawer.LoadFont();
    return;
}

/*====================================================================
	函数  : Disconnect
	功能  : 普通实例处理Osp断链消息
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::Disconnect( )
{

	//1、 与其中一个断链后，向Mcu取主备倒换状态，判断是否成功
	if ( INVALID_NODE != g_cMpu2VmpApp.m_dwMcuNode || INVALID_NODE != g_cMpu2VmpApp.m_dwMcuNodeB )
	{
		if ( OspIsValidTcpNode(g_cMpu2VmpApp.m_dwMcuNode) )
		{
			post( m_dwMcuIId, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cMpu2VmpApp.m_dwMcuNode );           
			KeyLog( "[Disconnect] try GetMsStatusReq. from McuNode.A(%d)\n",g_cMpu2VmpApp.m_dwMcuNode );
		}
		else if ( OspIsValidTcpNode(g_cMpu2VmpApp.m_dwMcuNodeB) )
		{
			post( m_dwMcuIIdB, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cMpu2VmpApp.m_dwMcuNodeB );        
			KeyLog( "[Disconnect] try GetMsStatusReq. from McuNode.B(%d)\n",g_cMpu2VmpApp.m_dwMcuNodeB );
		}

		SetTimer( TIME_GET_MSSTATUS, WAITING_MSSTATUS_TIMEOUT);
		return;
	}
	//2、 两个节点都断，清空状态信息
    if ( INVALID_NODE == g_cMpu2VmpApp.m_dwMcuNode && INVALID_NODE == g_cMpu2VmpApp.m_dwMcuNodeB )
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
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::ClearCurrentInst( void )
{
	switch ( CurState() )
    {
    case NORMAL:
    case RUNNING:
        {
            s32 nRet = m_cHardMulPic.StopMerge();
            KillTimer( EV_VMP_NEEDIFRAME_TIMER );
            KeyLog( "[VMP Stop]:Stop work, the return code: %d!\n", nRet );
			//停止合成编码的码流回调
			for ( u8 byChnNo = 0; byChnNo < m_byValidVmpOutNum; byChnNo++ )
			{
				m_cHardMulPic.SetVidDataCallback( byChnNo, NULL, 0 );
			
			}
        }
        break;
	default:
		break;
    }

    NEXTSTATE((u32) IDLE );

    m_byRegAckNum = 0;
	m_dwMpcSSrc = 0;
	m_bFastUpdate = FALSE;
	
	memset( m_bAddVmpChannel, 0, sizeof(m_bAddVmpChannel) );
	memset( m_bUpdateMediaEncrpyt, 0, sizeof(m_bUpdateMediaEncrpyt));
	memset( m_abChnlIframeReqed, 0, sizeof(m_abChnlIframeReqed) );
	SetIsDisplayMbAlias(FALSE);
	m_cHardMulPic.ChangeMode(m_dwInitVmpMode);

	//清除previous相关的成员
	memset(m_tParamPrevious, 0, sizeof(m_tParamPrevious));
	memset(&m_tPreviousVmpCommonAttrb, 0, sizeof(m_tPreviousVmpCommonAttrb));
    memset(m_tVmpMbAlias, 0, sizeof(m_tVmpMbAlias));

    KeyLog( "[Vmp] Disconnect with A and B board\n");

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
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::DaemonProcConnectTimeOut( BOOL32 bIsConnectA )
{
    BOOL32 bRet = FALSE;
    if( TRUE == bIsConnectA )
    {
        bRet = ConnectMcu( bIsConnectA, g_cMpu2VmpApp.m_dwMcuNode );
        if ( TRUE == bRet )
        {  
           //连接成功，发Register消息给实例，让他们开始注册
			for (u8 byLoop = 0 ; byLoop< g_cMpu2VmpApp.m_byVmpEqpNum ;byLoop++)
			{
				CServMsg cServMsg;
				post(MAKEIID(AID_MPU2,byLoop+1),EV_VMP_REGISTER_TIMER,cServMsg.GetServMsg(),cServMsg.GetServMsgLen());
			}
			//TODO:此时切换状态是否正确，如果connect mcub不成功呢？？？？
			NEXTSTATE((u32)DAEMON_NORMAL);
			
        }
        else
        {
            SetTimer( EV_VMP_CONNECT_TIMER, MPU_CONNETC_TIMEOUT );   
        }
    }
    else //连接备板
    {
        bRet = ConnectMcu( bIsConnectA, g_cMpu2VmpApp.m_dwMcuNodeB );
        if ( TRUE == bRet )
        { 
			//连接成功，发Register消息给实例，让他们开始注册
			for (u8 byLoop = 0 ; byLoop< g_cMpu2VmpApp.m_byVmpEqpNum ;byLoop++)
			{
				CServMsg cServMsg;
				post(MAKEIID(AID_MPU2,byLoop+1),EV_VMP_REGISTER_TIMERB,cServMsg.GetServMsg(),cServMsg.GetServMsgLen());
			}
				//TODO:此时切换状态是否正确？？？？？？
			NEXTSTATE((u32)DAEMON_NORMAL);
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
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
BOOL32 CMpu2VmpInst::ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode )
{
    BOOL32 bRet = TRUE;

	//取连接的node
    if( !OspIsValidTcpNode(dwMcuNode) ) 
    {
#ifdef _8KI_
		dwMcuNode = OspConnectTcpNode(htonl(m_tCfg.dwConnectIP),m_tCfg.wConnectPort,10,3,100);
	
#else
		if(TRUE == bIsConnectA)
        {
            dwMcuNode = BrdGetDstMcuNode();  
        }
        else
        {  
            dwMcuNode = BrdGetDstMcuNodeB(); 
        }
#endif

	    if ( ::OspIsValidTcpNode(dwMcuNode) )
	    {
		    KeyLog( "[ConnectMcu]Connect Mcu %s Success, node: %d!\n", 
                   bIsConnectA ? "A" : "B", dwMcuNode );
		    ::OspNodeDiscCBRegQ( dwMcuNode, GetAppID(), GetInsID() );
	    }
	    else 
	    {
		    ErrorLog( "[ConnectMcu]Connect to Mcu failed, will retry\n" );
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
  参    数： BOOL32 bIsRegiterA
  返 回 值： void 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::ProcRegisterTimeOut( BOOL32 bIsRegiterA )
{
    if( TRUE == bIsRegiterA )
    {
        Register( g_cMpu2VmpApp.m_dwMcuNode );
        SetTimer( EV_VMP_REGISTER_TIMER, MPU_REGISTER_TIMEOUT );  
    }
    else
    {
        Register( g_cMpu2VmpApp.m_dwMcuNodeB );
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
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::Register( /*BOOL32 bIsRegiterA,*/ u32 dwMcuNode )
{
    CServMsg       cSvrMsg;
    TPeriEqpRegReq tReg;

    tReg.SetMcuEqp( (u8)m_tCfg.wMcuId, m_tCfg.byEqpId, m_tCfg.byEqpType );
    tReg.SetPeriEqpIpAddr( htonl(m_tCfg.dwLocalIP) );
	tReg.SetStartPort( m_tCfg.wRcvStartPort );
	if ( IsSimulateMpu() ) 
	{
		tReg.SetVersion( DEVVER_MPU );
	}
	else
	{
		tReg.SetVersion( DEVVER_MPU2 );
	}
    
    
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
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
BOOL CMpu2VmpInst::SendMsgToMcu( u16 wEvent, CServMsg& cServMsg )
{
    KeyLog( "[SendMsgToMcu] Message %u(%s).\n", wEvent, ::OspEventDesc(wEvent) );

    if ( OspIsValidTcpNode(g_cMpu2VmpApp.m_dwMcuNode) )
    {
        post( m_dwMcuIId, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), g_cMpu2VmpApp.m_dwMcuNode );
    }

    if ( OspIsValidTcpNode(g_cMpu2VmpApp.m_dwMcuNodeB) )
    {
        post( m_dwMcuIIdB, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), g_cMpu2VmpApp.m_dwMcuNodeB );
    }

    return TRUE;
}

/*====================================================================
	函数  : MsgRegAckProc 
	功能  : MCU应答注册成功消息处理函数
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::MsgRegAckProc( CMessage* const pMsg )
{
	//1、参数合法性判断
	if ( NULL == pMsg )
    {
        ErrorLog( "[MsgRegAckProc]Recv Reg Ack Msg, but the msg's pointer is Null\n" );
        return;
    }

	//2、解析ACK消息体
	TPeriEqpRegAck* ptRegAck;

	CServMsg cServMsg( pMsg->content, pMsg->length );
	ptRegAck = (TPeriEqpRegAck*)cServMsg.GetMsgBody();
	
    TPrsTimeSpan tPrsTimeSpan = *(TPrsTimeSpan*)(cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck));
	u16 wMTUSize = *(u16*)( cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck) + sizeof(TPrsTimeSpan) );
	wMTUSize = ntohs( wMTUSize );
    //[4/9/2013 liaokang] 编码方式
    u8 byMcuEncoding = emenCoding_GBK;
    if( cServMsg.GetMsgBodyLen() > sizeof(TPeriEqpRegAck) + sizeof(TPrsTimeSpan) + sizeof(u16) )
    {
        byMcuEncoding = *(u8*)( cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck) + sizeof(TPrsTimeSpan) + sizeof(u16) );
    }

	//3、Kill相应的注册timer
    if( pMsg->srcnode == g_cMpu2VmpApp.m_dwMcuNode ) 
    {
	    m_dwMcuIId = pMsg->srcid;
        m_byRegAckNum++;
        KillTimer( EV_VMP_REGISTER_TIMER );

        KeyLog( "[MsgRegAckProc] Regist success to mcu A\n" );
    }
    else if ( pMsg->srcnode == g_cMpu2VmpApp.m_dwMcuNodeB )
    {
        m_dwMcuIIdB = pMsg->srcid;
        m_byRegAckNum++;
        KillTimer( EV_VMP_REGISTER_TIMERB );

        KeyLog( "[MsgRegAckProc] Regist success to mcu B\n" );       
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
            ErrorLog( "[RegAck] MPC SSRC ERROR(%u<-->%u), Disconnect Both Nodes!\n", 
                       m_dwMpcSSrc, ptRegAck->GetMSSsrc() );
            if ( OspIsValidTcpNode(g_cMpu2VmpApp.m_dwMcuNode) )
            {
                OspDisconnectTcpNode(g_cMpu2VmpApp.m_dwMcuNode);
            }
            if ( OspIsValidTcpNode(g_cMpu2VmpApp.m_dwMcuNodeB) )
            {
                OspDisconnectTcpNode(g_cMpu2VmpApp.m_dwMcuNodeB);
            }      
            return;
        }
    }

    //4、 第一次收到注册成功消息
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

            ErrorLog( "[MsgRegAckProc] Found another Mpc IP: %s, try connecting...\n", 
                      ipStr(dwOtherMcuIp) );
        }

        KeyLog( "[MsgRegAckProc]eqpId:%d Local Recv Start Port.%u, Mcu Start Port.%u, Alias.%s\n",
                m_tCfg.byEqpId,m_tCfg.wRcvStartPort, m_wMcuRcvStartPort , m_tCfg.achAlias );

        m_tPrsTimeSpan = tPrsTimeSpan;
        KeyLog( "[MsgRegAckProc] The Vmp Prs span: first: %d, second: %d, three: %d, reject: %d\n", 
                m_tPrsTimeSpan.m_wFirstTimeSpan,
                m_tPrsTimeSpan.m_wSecondTimeSpan,
                m_tPrsTimeSpan.m_wThirdTimeSpan,
                m_tPrsTimeSpan.m_wRejectTimeSpan );

		m_wMTUSize = wMTUSize;
		KeyLog( "[MsgRegAckProc]The Network MTU is : %d\n", m_wMTUSize );

        // zw 20081208 在第一次注册成功之后，将确认后的m_tCfg中的信息（m_tCfg是在PowerOn时传过来并保存的）转存至m_tEqp。
        m_tEqp.SetMcuEqp( (u8)m_tCfg.wMcuId, m_tCfg.byEqpId, m_tCfg.byEqpType );	
	    
        SetMcuEncoding((emenCodingForm)byMcuEncoding);

        NEXTSTATE((u32) NORMAL ); 

    }

	SendStatusChangeMsg((u8)ONLINE,(u8)USESTATE_IDLE);



    return;
}

/*====================================================================
	函数  : MsgRegNackProc
	功能  : MCU 拒绝注册消息处理函数
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::MsgRegNackProc( CMessage* const pMsg )
{
    if ( pMsg->srcnode == g_cMpu2VmpApp.m_dwMcuNode )    
    {
        KeyLog( "VMP registe be refused by A.\n" );
    }
    if ( pMsg->srcnode == g_cMpu2VmpApp.m_dwMcuNodeB )    
    {
        KeyLog( "VMP registe be refused by B.\n" );
    }
    return;
}
/*====================================================================
	函数  : MsgModMcuRcvAddrProc
	功能  : MCU强制要求修改地址
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::MsgModMcuRcvAddrProc( CMessage* const pMsg )
{
	//1、参数合法性校验
    if ( NULL == pMsg )
    {
        ErrorLog( " The pointer cannot be Null (MsgModMcuRcvAddrProc)\n" );
        return;
    }    
	//2、解析消息体，修改后的mcu接收ip
	u32 dwSendIP = *(u32*)pMsg->content;
	m_dwMcuRcvIp = ntohl(dwSendIP);
	//3、修改mcu的接收地址
    if (CurState() == RUNNING)
    {
		for ( u8 byChnNo = 0; byChnNo < m_byMaxVmpOutNum; byChnNo++ )
		{

			SetMediaSndNetParam(byChnNo);
		}

		//[2011/08/26/zhangli]更新接收信息
		for (u8 byChn = 0; byChn < MAXNUM_MPU2VMP_MEMBER; byChn++)
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
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::MsgStartVidMixProc( CMessage * const pMsg )
{
	u32 dwTick = OspTickGet();
	KeyLog("===========[MsgStartVidMixProc] in Tick:%d,tickrate:%d\n",dwTick,OspClkRateGet());
	//0、参数合法性判断
    if ( NULL == pMsg )
    {
        ErrorLog( " The pointer cannot be Null (MsgStartVidMixProc)\n" );
        return;
    }  
	//非NORMAL状态start不进
	if ( CurState() != NORMAL ) 
	{
		ErrorLog( "[MsgStartVidMixProc]Wrong State(not NORMAL),CurrentState:%d\n",CurState() );
		return;
	}
	//2、mpu2 解析消息体 TMPU2CommonAttrb+9* CKDVVMPOutParam
	CServMsg cServMsg( pMsg->content, pMsg->length );
    UnpackVmpWholeOperMsg(cServMsg, TRUE);


	KeyLog( "/*---------------------------------------------------------*/\n\n" );
	KeyLog( "Follows are the params that mcu send to the mpu2vmp:\n\n" );
	s32 nIdx = 0;
	for(nIdx = 0; nIdx < m_byMaxVmpOutNum; nIdx++)
	{
		KeyLog( "CHNNL.%d: Type=%d BitRate=%d VideoHeight:%d VideoWidth:%d,framerate:%d,HP:%d \n",
						   nIdx,
						   m_tParam[nIdx].GetEncType(), m_tParam[nIdx].GetBitRate(), 
						   m_tParam[nIdx].GetVideoHeight(), m_tParam[nIdx].GetVideoWidth(),
						   m_tParam[nIdx].GetFrameRate(),m_tParam[nIdx].GetProfileType() );
	}
	KeyLog( "/*---------------------------------------------------------*/\n\n" );

	s32 nRet = 0;
	//第0路是能力最高的一路，如果小于等于1080 30，则切换到VMP_ADP_1080P30DEC模式
	if ( m_tParam[0].GetVideoWidth() < 1920 || 
			(m_tParam[0].GetVideoWidth() == 1920 && m_tParam[0].GetFrameRate() <= 30) )
	{
		nRet = m_cHardMulPic.ChangeMode(VMP_ADP_1080P30DEC);
		
	}
	
	u8 byNeedPrs = m_tVmpCommonAttrb.m_byNeedPrs;
	m_tStyleInfo = m_tVmpCommonAttrb.m_tVmpStyleCfgInfo;
	TVMPExCfgInfo tVMPExCfgInfo = m_tVmpCommonAttrb.m_tVmpExCfgInfo;

	//3、设置空闲通道显示模式（黑屏，上一帧，自定义图片...）
	nRet = SetIdleChnShowMode(tVMPExCfgInfo.m_byIdleChlShowMode,TRUE);

	SetIsDisplayMbAlias( (tVMPExCfgInfo.m_byIsDisplayMmbAlias == 1) ? TRUE:FALSE);

	//4.部分参数调整,码率作弊，I帧间最大P帧数目，量化参数等
	u8 byValidEncNum = 0;
	for(nIdx = 0; nIdx < m_byMaxVmpOutNum; nIdx++)
	{
		if ( IsEnableBitrateCheat() )
		{
			//原本码率是0的维持码率为0
			if(0 != m_tParam[nIdx].GetBitRate())
			{
				m_tParam[nIdx].SetBitRate( GetBitrateAfterCheated(m_tParam[nIdx].GetBitRate()) );
			}
			
			KeyLog( "New Enc.%d Bitrate: %d\n", nIdx, m_tParam[nIdx].GetBitRate() );
		}

		// 设置视频编码参数(出来的格式)
		// 完成对m_tMpuVmpCfg.m_tVideoEncParam[]的基本参数赋值
		m_tMpuVmpCfg.GetDefaultParam( m_tParam[nIdx].GetEncType(),
									  m_tMpuVmpCfg.m_tVideoEncParam[nIdx],
									  m_tMpuVmpCfg.m_adwMaxSendBand[nIdx] );


		
		// 以下为m_tMpuVmpCfg.m_tVideoEncParam[]其他参数赋值和调整
		TransFromMpu2Param2EncParam( &m_tParam[nIdx], &m_tMpuVmpCfg.m_tVideoEncParam[nIdx] );
		
		if ( m_tMpuVmpCfg.m_adwMaxSendBand[nIdx] == 0 )
		{
			// 发送带宽仍然使用初始码率进行设置
			m_tMpuVmpCfg.m_adwMaxSendBand[nIdx] = (u32)(m_tParam[nIdx].GetBitRate() * 2 + 100);
		}
		
		//mpu2 空参不下，mpu每路都下
		if ( !IsSimulateMpu() &&	
			( m_tMpuVmpCfg.m_tVideoEncParam[nIdx].m_wVideoHeight == 0 || 0 == m_tMpuVmpCfg.m_tVideoEncParam[nIdx].m_wVideoWidth ) )
		{
			continue;
		}
	
		byValidEncNum++;
		// 打印开始的参数
		KeyLog( "Follows are the params we used to start the vmp:\n\n" );
		KeyLog( "/*----------  video channel.%d  -----------*/\n", nIdx );
		m_tMpuVmpCfg.PrintEncParam( nIdx );	
		KeyLog( "/*----------------------------------------*/\n\n" );
	}


	//5. 设置风格
	m_byHardStyle = ConvertVcStyle2HardStyle( m_tVmpCommonAttrb.m_byVmpStyle );
	dwTick = OspTickGet();
	nRet = m_cHardMulPic.SetMulPicType(m_byHardStyle);
	KeyLog("=============[MsgStartVidMixProc] m_cHardMulPic.SetMulPicType Tick:%d\n",OspTickGet() - dwTick);
	if ( HARD_MULPIC_OK != nRet )
	{
		ErrorLog( " SetMulPicType failed, Error code: %u\n", nRet );
		cServMsg.SetErrorCode( 1 );                 
		SendMsgToMcu( pMsg->event + 2, cServMsg );
		return;
	}
	KeyLog( "SetMulPicType to %d.\n", m_byHardStyle);

	//6. 通道整理
	
	u8 byMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle( m_tVmpCommonAttrb.m_byVmpStyle );	
	for (u8 byChnNo = 0; byChnNo < byMaxChnNum; byChnNo++ )
	{
		if ( m_tVmpCommonAttrb.m_atMtMember[byChnNo].IsNull() )
		{
			m_bAddVmpChannel[byChnNo] = FALSE;
		}
		else
		{
			dwTick = OspTickGet();
			nRet = m_cHardMulPic.AddChannel( byChnNo );
			KeyLog("================[MsgStartVidMixProc]after m_cHardMulPic.AddChannel(%d) Tick:%d\n",byChnNo,OspTickGet() - dwTick);
			if ( HARD_MULPIC_OK != nRet )
			{
				mpulog(MPU_CRIT, "[MsgStartVidMixProc] add Chn.%d failed, ret=%u!\n", byChnNo, nRet);
			}
			else
			{
				m_bAddVmpChannel[byChnNo] = TRUE;
		
			
			}
			KeyLog( "Add channel-%u return %u(%u is correct)!\n", byChnNo, nRet, HARD_MULPIC_OK );
			KeyLog( "RealPayLoad: %d, ActivePayload: %d\n",
							   m_tVmpCommonAttrb.m_tDoublePayload[byChnNo].GetRealPayLoad(),
							   m_tVmpCommonAttrb.m_tDoublePayload[byChnNo].GetActivePayload() );
		}
	}
	

	//7. 开始合成
	m_byValidVmpOutNum = byValidEncNum;
	KeyLog("[MsgStartVidMixProc ]m_byValidVmpOutNum:%d\n",m_byValidVmpOutNum);
	TVideoEncParam atOrigEncParam[MAXNUM_MPU2VMP_CHANNEL];
	memcpy(atOrigEncParam,&m_tMpuVmpCfg.m_tVideoEncParam[0],sizeof(atOrigEncParam));
	for (u8 byLoop = 0 ; byLoop < m_byValidVmpOutNum;byLoop++)
	{
		u16 wWidth = m_tParam[byLoop].GetVideoWidth();
		if( wWidth == 704 && MEDIA_TYPE_MP4 == m_tParam[byLoop].GetEncType() )
		{
			wWidth = 720;
		}
		atOrigEncParam[byLoop].m_wVideoWidth = wWidth;
		atOrigEncParam[byLoop].m_wVideoHeight = m_tParam[byLoop].GetVideoHeight();
		atOrigEncParam[byLoop].m_byFrameRate = m_tParam[byLoop].GetFrameRate();
	}


	//先下最大的分辨率，防止start的时候有低码率终端进来先编小分辨率，然后删掉该终端后编大分辨率，媒控不支持
	dwTick = OspTickGet();
	nRet = m_cHardMulPic.SetVideoEncParam(&atOrigEncParam[0], 
										m_byValidVmpOutNum);
	KeyLog("=================[MsgStartVidMixProc] m_cHardMulPic.SetVideoEncParam Tick:%d\n",OspTickGet() - dwTick );
	KeyLog("[MsgStartVidMixProc]byValidEncNum:%d\n",byValidEncNum);
	if ( HARD_MULPIC_OK != nRet )
	{
		ErrorLog( " SetVideoEncParam failed, Error code: %u\n", nRet );
		cServMsg.SetErrorCode( 1 );              
		SendMsgToMcu( pMsg->event + 2, cServMsg );
		return;
	}
	// 将开始码流收发设置提前，startmerge不能保证码率回调上来的时间点，可能会导致错误
	if (!StartNetRcvSnd(byNeedPrs))
	{
		ErrorLog( "StartNetRcvSnd failed!\n");
		cServMsg.SetErrorCode( 1 );                 // FIXME: 错误码上报定义
		SendMsgToMcu( pMsg->event + 2, cServMsg );
		return;
	}
	dwTick = OspTickGet();
	nRet = m_cHardMulPic.StartMerge();
	KeyLog("============[MsgStartVidMixProc] m_cHardMulPic.StartMerge Tick:%d\n",OspTickGet() - dwTick);
	if ( HARD_MULPIC_OK != nRet )
	{
		ErrorLog( " startmerge failed, Error code: %u\n", nRet );
		cServMsg.SetErrorCode( 1 );              
		SendMsgToMcu( pMsg->event + 2, cServMsg );
		return;
	}
	//下实际上需要编码的分辨率
	for ( u8 byChnLoop = 0 ; byChnLoop < m_byValidVmpOutNum;byChnLoop++)
	{
		if ( atOrigEncParam[byChnLoop].m_wVideoWidth != m_tMpuVmpCfg.m_tVideoEncParam[byChnLoop].m_wVideoWidth
			|| atOrigEncParam[byChnLoop].m_wVideoHeight != m_tMpuVmpCfg.m_tVideoEncParam[byChnLoop].m_wVideoHeight 
			|| atOrigEncParam[byChnLoop].m_byFrameRate != m_tMpuVmpCfg.m_tVideoEncParam[byChnLoop].m_byFrameRate )
		{
			dwTick = OspTickGet();
			nRet = m_cHardMulPic.ChangeVideoEncParam(byChnLoop,&m_tMpuVmpCfg.m_tVideoEncParam[byChnLoop]);
			KeyLog("============[MsgStartVidMixProc] m_cHardMulPic.ChangeVideoEncParam(%d) Tick:%d\n",byChnLoop,OspTickGet() -dwTick);
			if ( HARD_MULPIC_OK != nRet )
			{
				ErrorLog("[MsgStartVidMixProc ]chnl:%d,changevideoEncParam failed!\n",byChnLoop);
				cServMsg.SetErrorCode( 1 );              
				SendMsgToMcu( pMsg->event + 2, cServMsg );
				return;
			}
		}
		
	}
	//8. 设置边框背景等
	if( !SetVmpAttachStyle( m_tStyleInfo ) )
	{
		ErrorLog( " Fail to set vmp attach style(MsgStartVidMixProc)\n" );
		cServMsg.SetMsgBody( NULL, 0 );
		cServMsg.SetErrorCode( 1 );          
		SendMsgToMcu( pMsg->event + 2, cServMsg );
		return;
	}
	for (u8 byChNum = 0;byChNum < byMaxChnNum;byChNum++)
	{
		if (m_bAddVmpChannel[byChNum] == TRUE)
		{
			//终端别名显示要放到startmerge之后做
			if ( IsDisplayMbAlias() )
			{
				//DisplayMbAlias(byChNum,m_tVmpCommonAttrb.m_atMtMember[byChNum].GetMbAlias());
                DisplayMbAlias(byChNum,m_tVmpMbAlias[byChNum].GetMbAlias());                
			}
		}
	}
	KeyLog( "StartMerge succeed!\n" );


	//9. 设置平滑发送
	BOOL32 bSetRule = !g_bPauseSS;
	dwTick = OspTickGet();
	SetSmoothSendRule( bSetRule );
	KeyLog("===================[MsgStartVidMixProc] SetSmoothSendRule Tick:%d\n",OspTickGet() -dwTick);


	//11、状态切为RUNNING，同时给mcu回ACK，NOTIFY，通报状态
	NEXTSTATE((u32)RUNNING);

	SendMsgToMcu( pMsg->event + 1, cServMsg );
	SendMsgToMcu( VMP_MCU_STARTVIDMIX_NOTIF, cServMsg );

	m_cConfId = cServMsg.GetConfId();
	SendStatusChangeMsg( (u8)ONLINE, (u8)USESTATE_START ); 
	KeyLog( "Vmp works properly now, cur state.%u\n", CurState() );

	//12、起请求关键帧的timer
	//[nizhijun 2011/03/21]VMP参数改变需要隔1秒检测关键帧请求
	//[nizhijun 2011/06/21]针对每个通道定时请求关键帧？
	memset( m_abChnlIframeReqed, 0, sizeof(m_abChnlIframeReqed) );
	for (u8 byChNumIdx = 0;byChNumIdx < byMaxChnNum;byChNumIdx++)
	{
		if (m_bAddVmpChannel[byChNumIdx] == TRUE)
		{
			SetTimer(EV_VMP_NEEDIFRAME_TIMER+byChNumIdx,CHECK_IFRAME_INTERVAL,byChNumIdx);
		}
	}

	// 针对开启含多个多回传下级场景，会执行一次start+多次change成员处理，耗时较长，导致mcu请求vmp编关键帧延后，终端接收出现蓝屏问题
	// 追加一个标志，在开启时，将此标志置TRUE，若接下来处理的是change而非关键帧请求，则change处理结束时追加对各路编码请求关键帧，再将标志置FALSE，仅有效一次
	// 若接下来收到的是关键帧请求，则标志位置FALSE，直接失效
	m_bFastUpdate = TRUE;

	KeyLog("===========[MsgStartVidMixProc] End Tick:%d\n",OspTickGet());
    return;
}

/*====================================================================
	函数名  :UnpackVmpWholeOperMsg
	功能    :解析VMP整体操作消息
	输入    :
	输出    :
	返回值  :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2013/07/03  4.7         liaokang        创建
====================================================================*/
void CMpu2VmpInst::UnpackVmpWholeOperMsg(CServMsg & cServMsg, BOOL32 bStart)
{
    u8* pContent = cServMsg.GetMsgBody();
    u16 wMsgBufLen =0;

    BOOL32 bVmpMbAliasExtend = FALSE;

    //1、判断是否模拟mpu
    if ( IsSimulateMpu() )
    {       
        CKDVNewVMPParam tParam[MAXNUM_MPUSVMP_CHANNEL];    // 四路视频输出通道
        memset( tParam, 0, sizeof(tParam) );
        memcpy( (u8*)tParam, pContent, sizeof(tParam)); 
        pContent += sizeof(tParam);
        wMsgBufLen += sizeof(tParam);
        
        KeyLog( "/*---------simulate mpu param---------------------------------------------------------*/\n\n" );
        KeyLog( "******************************************:\n\n" );
        s32 nIdx = 0;
        for(nIdx = 0; nIdx < m_byMaxVmpOutNum; nIdx++)
        {
            KeyLog( "CHNNL.%d: Type=%d BitRate=%d VideoHeight:%d VideoWidth:%d,framerate:%d \n",
                nIdx,
                tParam[nIdx].m_byEncType, tParam[nIdx].GetBitRate(), 
                tParam[nIdx].GetVideoHeight(), tParam[nIdx].GetVideoWidth(),
                tParam[nIdx].GetFrameRate());
        }
        KeyLog( "/*************************************/\n\n" );        
        
        //2、mpu2 解析消息体 TMPU2CommonAttrb+9* CKDVVMPOutParam
        memcpy(m_tVmpCommonAttrb.m_atMtMember,tParam[0].m_atMtMember,sizeof(TVMPMemberEx)*MAXNUM_MPUSVMP_MEMBER);
        m_tVmpCommonAttrb.m_byMemberNum = tParam[0].m_byMemberNum;
        m_tVmpCommonAttrb.m_byVmpStyle = tParam[0].m_byVMPStyle;
        memcpy(m_tVmpCommonAttrb.m_tDoublePayload ,tParam[0].m_tDoublePayload,sizeof(TDoublePayload)*MAXNUM_MPUSVMP_MEMBER);
        memcpy(m_tVmpCommonAttrb.m_tVideoEncrypt,tParam[0].m_tVideoEncrypt,sizeof(TMediaEncrypt)*MAXNUM_MPUSVMP_MEMBER);

        m_tVmpCommonAttrb.m_byNeedPrs = *(u8*)pContent;
        pContent += sizeof(u8);
        wMsgBufLen += sizeof(u8);

        m_tStyleInfo = *(TVmpStyleCfgInfo*)pContent;
        pContent += sizeof(TVmpStyleCfgInfo);
        wMsgBufLen += sizeof(TVmpStyleCfgInfo);
        m_tVmpCommonAttrb.m_tVmpStyleCfgInfo = m_tStyleInfo;

        //  [1/12/2010 pengjie] Modify 设置空闲通道显示模式（黑屏，上一帧，图片...）
        TVMPExCfgInfo tVMPExCfgInfo;
        if( cServMsg.GetMsgBodyLen() > wMsgBufLen )
        {
            tVMPExCfgInfo = *(TVMPExCfgInfo*)pContent;
            pContent += sizeof(TVMPExCfgInfo);
            wMsgBufLen += sizeof(TVMPExCfgInfo);
        }
        else
        {
            tVMPExCfgInfo.m_byIdleChlShowMode = VMP_SHOW_BLACK_MODE;
        }
        
        if( tVMPExCfgInfo.m_byIdleChlShowMode > VMP_SHOW_USERDEFPIC_MODE )
        {
            tVMPExCfgInfo.m_byIdleChlShowMode = VMP_SHOW_BLACK_MODE; // 默认显示黑屏
        }
        KeyLog( "[TransMpuParam2Mpu2Param] Vmp IdleChlShowMode: %d \n", tVMPExCfgInfo.m_byIdleChlShowMode );
        m_tVmpCommonAttrb.m_tVmpExCfgInfo = tVMPExCfgInfo;
        
#ifdef WIN32//songkun,20110628,加入MPU WIN32 B版编译
        ;
#else
#ifndef _MPUB_256_	//A板
        memset(&tParam[1], 0, sizeof(CKDVNewVMPParam) );
#endif
#endif		
        
        for (u8 byLoop  = 0 ; byLoop  <  m_byMaxVmpOutNum ; byLoop++)
        {
            //检查mpu参数是否有空参，有的话就改成合法的参数，并设置码率为0
            if ( tParam[byLoop].m_byEncType == MEDIA_TYPE_NULL || tParam[byLoop].GetVideoWidth() == 0 || 
                (tParam[byLoop].GetVideoHeight() == 0 ) )
            {
                MpuNullParam2ValidParam(tParam[byLoop],byLoop);
            }
            else if ( tParam[byLoop].GetFrameRate() == 0 )
            {
                MpuFrameRateCheck(tParam[byLoop]);
            }
            //1080 720 60/50编码帧率要降为30/25
            u8 byFrameRate = tParam[byLoop].GetFrameRate();
            if ( byFrameRate >=  50 )
            {
                tParam[byLoop].SetFrameRate(byFrameRate/2);
            }
            m_tParam[byLoop].SetBitRate(tParam[byLoop].GetBitRate());
            m_tParam[byLoop].SetEncType(tParam[byLoop].m_byEncType);
            m_tParam[byLoop].SetFrameRate(tParam[byLoop].GetFrameRate());
            m_tParam[byLoop].SetProfileType(0);
            m_tParam[byLoop].SetVideoHeight(tParam[byLoop].GetVideoHeight());
            m_tParam[byLoop].SetVideoWidth(tParam[byLoop].GetVideoWidth());
        }	      
    }
    else 
    { 
        memcpy((u8*)&m_tVmpCommonAttrb,pContent,sizeof(TMPU2CommonAttrb));
        pContent += sizeof(TMPU2CommonAttrb);
        wMsgBufLen += sizeof(TMPU2CommonAttrb);

        memcpy( (u8*)m_tParam,pContent,MAXNUM_MPU2VMP_CHANNEL*sizeof(CKDVVMPOutMember));
        pContent += (MAXNUM_MPU2VMP_CHANNEL*sizeof(CKDVVMPOutMember));
        wMsgBufLen += (MAXNUM_MPU2VMP_CHANNEL*sizeof(CKDVVMPOutMember));

        if (cServMsg.GetMsgBodyLen() > wMsgBufLen)
        {    
            u8 byTypeNum = *(u8*)pContent;
            pContent += sizeof(u8);
            wMsgBufLen += sizeof(u8);

            u16 wLength = 0;

            while( (byTypeNum-- > 0) && (cServMsg.GetMsgBodyLen() > wMsgBufLen) )
            {
                emVmpCommType etype = (emVmpCommType)(*pContent);
                pContent ++;
                wLength = ntohs( *(u16*)pContent);
                pContent += sizeof(u16);
                switch (etype)
                {
                case emVmpMbAlias:
                    {
                        bVmpMbAliasExtend = TRUE;
						// 开启时，清空所有别名信息，改变成员时，需判断成员
						if (bStart)
						{
							memset(m_tVmpMbAlias, 0, sizeof(m_tVmpMbAlias));
						}
						else
						{
							for (u8 byIdx=0; byIdx<MAXNUM_MPU2VMP_MEMBER;byIdx++)
							{
								if (!(m_tVmpCommonAttrb.m_atMtMember[byIdx] == m_tPreviousVmpCommonAttrb.m_atMtMember[byIdx]))
								{
									memset(&m_tVmpMbAlias[byIdx], 0, sizeof(TVmpMbAlias));
								}
							}
						}

                        u16 wTypeLen = 0;
                        u8 byChnlNum = *(u8*)pContent;
                        pContent ++;
                        wTypeLen ++;
                        u8 byChnlNo = 0;
                        u8 byChnlAliasLen = 0;
                        for(u8 byLoop = 0; byLoop < byChnlNum; byLoop++)
                        {
                            byChnlNo = *(u8*)pContent;
                            pContent ++;
                            wTypeLen ++;
                            byChnlAliasLen = *(u8*)pContent;
                            pContent ++;
                            wTypeLen ++;
                            if ( byChnlNo < MAXNUM_MPU2VMP_MEMBER )
                            {
								// 开启时，直接设置别名
								if (bStart)
								{
									m_tVmpMbAlias[byChnlNo].SetMbAlias(byChnlAliasLen,(s8*)pContent);
								}
								else
								{
									// 调整时，需判新旧通道成员是否改变，成员未改变时，不更新别名
									if (!(m_tVmpCommonAttrb.m_atMtMember[byChnlNo] == m_tPreviousVmpCommonAttrb.m_atMtMember[byChnlNo]))
									{
										m_tVmpMbAlias[byChnlNo].SetMbAlias(byChnlAliasLen,(s8*)pContent);
									}
								}
                            }
                            
                            pContent += byChnlAliasLen;
                            wTypeLen += byChnlAliasLen;
                        }
                        pContent -= wTypeLen;
                    }
                    break;
                default:
                    KeyLog("Unknow Vmp Communication Type %d!\n", etype);
                    break;
                }
                pContent += wLength;
                wMsgBufLen += sizeof(wLength);
            }
        }	
	}

    //别名
    if ( FALSE == bVmpMbAliasExtend )
    {
        u8 byMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle( m_tVmpCommonAttrb.m_byVmpStyle );
        for (u8 byChnNo = 0; byChnNo < byMaxChnNum; byChnNo++ )
        {
            if ( !m_tVmpCommonAttrb.m_atMtMember[byChnNo].IsNull())
            {
                m_tVmpMbAlias[byChnNo].SetMbAlias(strlen(m_tVmpCommonAttrb.m_atMtMember[byChnNo].GetMbAlias()),
                    m_tVmpCommonAttrb.m_atMtMember[byChnNo].GetMbAlias());
            }
        }
    }
}

/*====================================================================
	函数名  :MpuNullParam2ValidParam
	功能    :mpu空参数变为有效参数,但是码率要设置为0
	输入    :
	输出    :
	返回值  :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2013/01/18  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::MpuNullParam2ValidParam(CKDVNewVMPParam& tInOutMpuParam, u8 byChnIdx)
{
	if ( byChnIdx >= MAXNUM_MPUSVMP_CHANNEL)
	{
		ErrorLog("[MpuNullParam2ValidParam]invalid chidx:%d\n",byChnIdx);
		return;
	}
	//mpu 0:1080,1:720,2:cif(other),3(4cif)
	u8 byVideoFormat[]={VIDEO_FORMAT_HD1080,VIDEO_FORMAT_HD720,VIDEO_FORMAT_CIF,VIDEO_FORMAT_4CIF};
	u16 witdth = 0,wheight = 0 ;
	GetWHViaRes(byVideoFormat[byChnIdx],witdth,wheight);
	tInOutMpuParam.SetVideoWidth(witdth);
	tInOutMpuParam.SetVideoHeight(wheight);
	switch (byChnIdx)
	{
	case 0:
		tInOutMpuParam.m_byEncType = MEDIA_TYPE_H264;
		tInOutMpuParam.SetBitRate(0);
		tInOutMpuParam.SetFrameRate(30);
		break;
	case 1:
		tInOutMpuParam.m_byEncType = MEDIA_TYPE_H264;
		tInOutMpuParam.SetBitRate(0);
		tInOutMpuParam.SetFrameRate(30);
		break;
	case 2:
		tInOutMpuParam.m_byEncType = MEDIA_TYPE_H264;
		tInOutMpuParam.SetBitRate(0);
		tInOutMpuParam.SetFrameRate(25);
		break;
	case 3:
		tInOutMpuParam.m_byEncType = MEDIA_TYPE_H264;
		tInOutMpuParam.SetBitRate(0);
		tInOutMpuParam.SetFrameRate(25);
		break;
	default:
		break;
	}
	
}

/*====================================================================
	函数名  :MpuFrameRateCheck
	功能    :mpu framerate下0的话，帮它填上
	输入    :
	输出    :
	返回值  :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2013/01/18  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::MpuFrameRateCheck(CKDVNewVMPParam& tInOutMpuParam)
{
	if (tInOutMpuParam.GetFrameRate() != 0)
	{
		return;
	}
	switch (tInOutMpuParam.GetVideoWidth())
	{
	case 1920:
		tInOutMpuParam.SetFrameRate(30);
		break;
	case 1280:
		tInOutMpuParam.SetFrameRate(30);
		break;
	case 352:
		tInOutMpuParam.SetFrameRate(25);
		break;
	case 704:
	case 720:
			tInOutMpuParam.SetFrameRate(25);
		break;
	default:
		break;
	}
	
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
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::MsgStopVidMixProc( CMessage * const pMsg )
{
    CServMsg cServMsg( pMsg->content, pMsg->length );

    switch ( CurState() )
    {
    case RUNNING:
        break;
    default:
		SendMsgToMcu( pMsg->event + 2, cServMsg );
        ErrorLog( "[MsgStopVidMixProc] Wrong State:%d\n", CurState());
        return;
    }

    u8 byChnNo = 0;
	//0. 停止合成编码的码流回调
    for ( byChnNo = 0; byChnNo < m_byValidVmpOutNum; byChnNo++ )
    {
        m_cHardMulPic.SetVidDataCallback( byChnNo, NULL, 0 );
    }
    //1. 停止发收
    StopNetRcvSnd();

 


	//3、停止复合
    s32 nRetCode = m_cHardMulPic.StopMerge();
	KeyLog( "Call stopmerge() error code= %d (0 is ok)\n", nRetCode );
    if ( HARD_MULPIC_OK == nRetCode )
    {
        cServMsg.SetErrorCode(0);
    }
    else
    {
        cServMsg.SetErrorCode(1);
    }
	//4、清空通道
	u8 byMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle(m_tVmpCommonAttrb.m_byVmpStyle);;

	
    for ( byChnNo = 0; byChnNo < byMaxChnNum; byChnNo++ )
    {
        m_cHardMulPic.RemoveChannel( byChnNo );
		if (IsDisplayMbAlias())
		{
			m_cHardMulPic.StopAddIcon(byChnNo);
			KeyLog("[MsgStopVidMixProc]Stop AddIcon chnl:%d\n",byChnNo);
		}
		KeyLog("[MsgStopVidMixProc] remove chnl:%d\n",byChnNo);
    } 

	//5、给mcu回响应
	SendMsgToMcu( pMsg->event + 1, cServMsg );
    SendMsgToMcu( VMP_MCU_STOPVIDMIX_NOTIF, cServMsg );
    SendStatusChangeMsg( (u8)ONLINE, (u8)USESTATE_IDLE );

	memset(m_abChnlIframeReqed,0,sizeof(m_abChnlIframeReqed));
	memset( m_bAddVmpChannel, 0, sizeof(m_bAddVmpChannel) );
    for (u8 byChn =0; byChn < MAXNUM_MPU2VMP_MEMBER; byChn++ )
	{
		KillTimer(EV_VMP_NEEDIFRAME_TIMER+byChn);
    }
	SetIsDisplayMbAlias(FALSE);

	   //2、停止平滑
	//[2010/12/31 zhushz]平滑发送接口封装
	SetSmoothSendRule(FALSE);
	
	//将vmp的模式修改回初始模式
	m_cHardMulPic.ChangeMode(m_dwInitVmpMode);
	
	//清除previous相关的成员
	memset(m_tParamPrevious, 0, sizeof(m_tParamPrevious));
	memset(&m_tPreviousVmpCommonAttrb, 0, sizeof(m_tPreviousVmpCommonAttrb));
    memset(m_tVmpMbAlias, 0, sizeof(m_tVmpMbAlias));
	
#ifdef _LINUX_
	BOOL32 bRet = CloseSmoothSending();
	KeyLog("close smooth result: %u\n", bRet);
#endif
	//状态切变为noraml
    NEXTSTATE((u32)NORMAL);

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
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::MsgChangeVidMixParamProc( CMessage* const pMsg )
{
	//0、参数的合法性判断
	if ( NULL == pMsg )
    {
        ErrorLog( " The pointer cannot be Null (MsgChangeVidMixParamProc)\n" );
        return;
    }    
	if ( CurState() != RUNNING ) 
	{
		 ErrorLog( "[MsgChangeVidMixParamProc] Wrong State:%d !\n", CurState() );
		return;
	}

	//1、保存上一次参数
	memset(m_tParamPrevious, 0, sizeof(m_tParamPrevious));
	memcpy(m_tParamPrevious, m_tParam, sizeof(m_tParam));
	memset(&m_tPreviousVmpCommonAttrb,0,sizeof(TMPU2CommonAttrb));
	memcpy(&m_tPreviousVmpCommonAttrb,&m_tVmpCommonAttrb,sizeof(TMPU2CommonAttrb));
	CServMsg cServMsg( pMsg->content, pMsg->length ); 
    UnpackVmpWholeOperMsg(cServMsg, FALSE);

    BOOL32 bCheckRet = FALSE;
    
    m_tStyleInfo = m_tVmpCommonAttrb.m_tVmpStyleCfgInfo;
    
    //3、风格和边框切变

	//设置画面合成风格
	bCheckRet = ChangeVMPStyle();
	if ( bCheckRet )
	{
		cServMsg.SetErrorCode( 0 ); // ok
	}
	else
	{
		 ErrorLog( " Failed to set ChangeVMPStyle (MsgChangeVidMixParamProc)\n" );
		cServMsg.SetMsgBody( NULL, 0 );
		cServMsg.SetErrorCode( 1 );
		SendMsgToMcu( pMsg->event + 2, cServMsg );
		return;
	}

	//设置边框和颜色
	BOOL32 bRet = SetVmpAttachStyle( m_tStyleInfo );
	if ( !bRet )
	{
		ErrorLog( " Failed to set vmp attach style (MsgChangeVidMixParamProc)\n" );
		cServMsg.SetMsgBody( NULL, 0 );
		cServMsg.SetErrorCode( 1 );
		SendMsgToMcu( pMsg->event + 2, cServMsg );
		return;
	}

	SetIdleChnShowMode(m_tVmpCommonAttrb.m_tVmpExCfgInfo.m_byIdleChlShowMode,FALSE);

    //4、通道和对应的net接收 切变
	BOOL32 bStyleChange = ( m_tPreviousVmpCommonAttrb.m_byVmpStyle != m_tVmpCommonAttrb.m_byVmpStyle );
    ChangeChnAndNetRcv( bStyleChange );
	
	//5、改变码率
	for(u8 byChnnlId = 0; byChnnlId < MAXNUM_MPU2VMP_CHANNEL; byChnnlId ++)
	{  
		//码率作弊，调整码率
		if ( IsEnableBitrateCheat() )
		{   
			m_tParam[byChnnlId].SetBitRate( GetBitrateAfterCheated(m_tParam[byChnnlId].GetBitRate()) );
			KeyLog( "New Enc Bitrate(chl.%d): %d\n", byChnnlId, m_tParam[byChnnlId].GetBitRate() );	
		}
		
		if( m_tParam[byChnnlId].GetBitRate() != m_tParamPrevious[byChnnlId].GetBitRate() )
		{
			//change br
			m_cHardMulPic.ChangeBitRate( byChnnlId, m_tParam[byChnnlId].GetBitRate() );
			//根据码率，读码率分辨率限制信息表，得到新的分辨率和帧率，看是否和老的相同，不相同，重新下参
			u16 wVideoOldWith = m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId].m_wVideoWidth;
			u16 wVideoOldHeight = m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId].m_wVideoHeight;
			u8 byOldFramerate = m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId].m_byFrameRate;
			m_tMpuVmpCfg.GetDefaultParam( m_tParam[byChnnlId].GetEncType(),
				m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId],
				m_tMpuVmpCfg.m_adwMaxSendBand[byChnnlId] );
			TransFromMpu2Param2EncParam(&m_tParam[byChnnlId],&m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId]);
			if (	m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId].m_wVideoWidth != wVideoOldWith || 
					m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId].m_wVideoHeight != wVideoOldHeight || 
					byOldFramerate != m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId].m_byFrameRate
					)
			{
				m_cHardMulPic.ChangeVideoEncParam( byChnnlId,&m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId] );
//				m_cHardMulPic.SetEncChnlParam( &m_tMpuVmpCfg.m_tVideoEncParam[byChnnlId], byChnnlId );
				mpulog( MPU_INFO,"[MsgSetBitRate] set encchan:%d new encparam:\n",byChnnlId);
				m_tMpuVmpCfg.PrintEncParam( byChnnlId );	
			}
		
		}
    }   



	//[2010/12/31 zhushz]平滑发送接口封装
	//6、设置平滑
	BOOL32 bSetRule = !g_bPauseSS;
	SetSmoothSendRule( bSetRule );


	//7、回ACK，回Notify，报状态
	SendMsgToMcu( pMsg->event + 1, cServMsg );
    SendMsgToMcu( VMP_MCU_CHANGESTATUS_NOTIF, cServMsg );
    KeyLog( "[[MsgChangeVidMixParamProc]]change param success,vmp work according to new params!\n" );
    
    SendStatusChangeMsg( (u8)ONLINE, (u8)USESTATE_START );
	
	// 追加请求关键帧处理,针对含多个下级多回传终端的开启合成，第一个change增加编码通道关键帧请求
	if (m_bFastUpdate)
	{
		s32 nRet;
		m_bFastUpdate = FALSE;// 仅有效1次
		for (u8 byIdx=0; byIdx<MAXNUM_MPU2VMP_CHANNEL; byIdx++)
		{
			if (byIdx == m_byValidVmpOutNum)
			{
				break;
			}
			nRet = m_cHardMulPic.SetFastUpdata( byIdx );
			if (nRet != HARD_MULPIC_OK)
			{
				KeyLog("[MsgChangeVidMixParamProc]m_cHardMulPic.SetFastUpdata(%u) failed!\n",byIdx );
			}
			else
			{
				KeyLog("[MsgChangeVidMixParamProc]m_cHardMulPic.SetFastUpdata(%u)! succeed\n",byIdx );
			}
		}
	}

    //8、起请求关键帧的timer
	//[nizhijun 2011/06/21]针对每个通道定时请求关键帧
	memset( m_abChnlIframeReqed, 0, sizeof(m_abChnlIframeReqed) );
	u8 byMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle(m_tVmpCommonAttrb.m_byVmpStyle);
	for (u8 byChNum = 0;byChNum < byMaxChnNum; byChNum++)
	{
		if (m_bAddVmpChannel[byChNum] == TRUE)
		{
			KillTimer(EV_VMP_NEEDIFRAME_TIMER+byChNum);
			SetTimer(EV_VMP_NEEDIFRAME_TIMER+byChNum,CHECK_IFRAME_INTERVAL,byChNum);
		}
	}
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
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::MsgSetBitRate( CMessage* const pMsg )
{
	//1、参数合法性校验
	if (NULL == pMsg)
	{
		ErrorLog("[MsgSetBitRate]Invalid pMsg（NULL）\n");
		return;
	}
	//2、解析消息体，通道号和要设置的码率
    CServMsg cServMsg( pMsg->content, pMsg->length );
    u8 byChalNum = cServMsg.GetChnIndex();
    u16 wBitrate = 0;


    cServMsg.GetMsgBody( (u8*)&wBitrate, sizeof(u16) ) ;
    wBitrate = ntohs(wBitrate);

    KeyLog( "[MsgSetBitRate]now we set the encode-%u bitrate to %u!\n", byChalNum, wBitrate );

	//3、码率作弊调整
    // guzh [10/18/2007] 支持码率作弊
	u16 wCheatBit = wBitrate;
    if ( IsEnableBitrateCheat() )
    {
        wCheatBit = GetBitrateAfterCheated( wBitrate );
    }   
	
    m_cHardMulPic.ChangeBitRate( byChalNum, wCheatBit );
	


	//4、保存通道的码率
// 	if ( IsSimulateMpu() )
// 	{
// 		m_tMpuParam[byChalNum].SetBitRate(wCheatBit);
// 		if( m_tMpuParam[byChalNum].GetBitRate() != m_tMpuParamPrevious[byChalNum].GetBitRate() )
// 		{
// 			m_tMpuParamPrevious[byChalNum].SetBitRate(wCheatBit);
// 			//根据码率，读码率分辨率限制信息表，得到新的分辨率和帧率，看是否和老的相同，不相同，重新下参
// 			u16 wVideoOldWith = m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_wVideoWidth;
// 			u16 wVideoOldHeight = m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_wVideoHeight;
// 			u8 byOldFramerate = m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_byFrameRate;
// 			m_tMpuVmpCfg.GetDefaultParam( m_tMpuParam[byChalNum].m_byEncType,
// 				m_tMpuVmpCfg.m_tVideoEncParam[byChalNum],
// 				m_tMpuVmpCfg.m_adwMaxSendBand[byChalNum] );
// 			TransFromMpuParam2EncParam(&m_tMpuParam[byChalNum],&m_tMpuVmpCfg.m_tVideoEncParam[byChalNum]);
// 			if ( m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_wVideoWidth != wVideoOldWith 
// 					|| m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_wVideoHeight != wVideoOldHeight 
// 					|| m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_byFrameRate != byOldFramerate)
// 			{
// 				m_cHardMulPic.ChangeVideoEncParam( byChalNum,&m_tMpuVmpCfg.m_tVideoEncParam[byChalNum] );
// //				m_cHardMulPic.SetEncChnlParam( &m_tMpuVmpCfg.m_tVideoEncParam[byChalNum], byChalNum );
// 				mpulog( MPU_INFO,"[MsgSetBitRate] set encchan:%d new encparam:\n",byChalNum);
// 				m_tMpuVmpCfg.PrintEncParam( byChalNum );	
// 			}
// 			
// 		}
// 	}
// 	else
	{
		m_tParam[byChalNum].SetBitRate(wCheatBit);
		if( m_tParam[byChalNum].GetBitRate() != m_tParamPrevious[byChalNum].GetBitRate() )
		{
			m_tParamPrevious[byChalNum].SetBitRate(wCheatBit);
			
			//根据码率，读码率分辨率限制信息表，得到新的分辨率和帧率，看是否和老的相同，不相同，重新下参
			u16 wVideoOldWith = m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_wVideoWidth;
			u16 wVideoOldHeight = m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_wVideoHeight;
			u8 byOldFramerate = m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_byFrameRate;
			m_tMpuVmpCfg.GetDefaultParam( m_tParam[byChalNum].GetEncType(),
				m_tMpuVmpCfg.m_tVideoEncParam[byChalNum],
				m_tMpuVmpCfg.m_adwMaxSendBand[byChalNum] );
			TransFromMpu2Param2EncParam(&m_tParam[byChalNum],&m_tMpuVmpCfg.m_tVideoEncParam[byChalNum]);
			if (	m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_wVideoWidth != wVideoOldWith || 
					m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_wVideoHeight != wVideoOldHeight ||
					m_tMpuVmpCfg.m_tVideoEncParam[byChalNum].m_byFrameRate != byOldFramerate)
			{
				m_cHardMulPic.ChangeVideoEncParam( byChalNum,&m_tMpuVmpCfg.m_tVideoEncParam[byChalNum] );
				mpulog( MPU_INFO,"[MsgSetBitRate] set encchan:%d new encparam:\n",byChalNum);
				m_tMpuVmpCfg.PrintEncParam( byChalNum );	
			}
		}
	}

	
	//[2010/12/31 zhushz]平滑发送接口封装
	BOOL32 bSetRule = !g_bPauseSS;
	SetSmoothSendRule( bSetRule );



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
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::SendStatusChangeMsg( u8 byOnline, u8 byState/*, u8 byStyle */)
{
    CServMsg cServMsg;
    TPeriEqpStatus tInitStatus;
    memset( &tInitStatus,0,sizeof(TPeriEqpStatus) );

    tInitStatus.m_byOnline = byOnline;
    tInitStatus.SetMcuEqp( (u8)m_tEqp.GetMcuId(), m_tEqp.GetEqpId(), m_tEqp.GetEqpType() );
    tInitStatus.SetAlias( m_tCfg.achAlias );

	//boardver，mpu2的话插子卡和不插子卡上报，mpu：A板B板
	if ( IsSimulateMpu() )  //模拟mpu
	{
		  tInitStatus.m_tStatus.tVmp.m_tVMPParam.SetVMPStyle( m_tVmpCommonAttrb.m_byVmpStyle );
		  tInitStatus.m_tStatus.tVmp.m_byChlNum = MAXNUM_MPUSVMP_MEMBER;
		#ifdef WIN32//songkun,20110628,加入MPU WIN32 B版编译
			tInitStatus.m_tStatus.tVmp.m_byBoardVer = MPU_BOARD_B256;
		#else    
			#ifndef _MPUB_256_ 
				tInitStatus.m_tStatus.tVmp.m_byBoardVer = MPU_BOARD_A128;
			#else
				tInitStatus.m_tStatus.tVmp.m_byBoardVer = MPU_BOARD_B256;
			#endif
		#endif
	}
	else //mpu2
	{
		 tInitStatus.m_tStatus.tVmp.m_tVMPParam.SetVMPStyle( m_tVmpCommonAttrb.m_byVmpStyle );
		 tInitStatus.m_tStatus.tVmp.m_byChlNum = MAXNUM_MPU2VMP_MEMBER;
		  BOOL32 bHasEcard = FALSE;
#ifdef _LINUX_
		  if ( BrdMpu2HasEcard() == 1)
		  {
			  bHasEcard = TRUE;
		  }
#else //win32
		  if ( g_cMpu2VmpApp.m_byWorkMode == TYPE_MPU2VMP_ENHANCED ||
				(g_cMpu2VmpApp.m_byWorkMode == TYPE_MPU2VMP_BASIC && g_cMpu2VmpApp.m_byVmpEqpNum == 2)
			  )
		  {
			  bHasEcard = TRUE;
		  }
#endif
		if ( bHasEcard ) 
		{
			tInitStatus.m_tStatus.tVmp.m_byBoardVer = MPU2_BOARD_ECARD;
		}
		else      //不插子卡
		{
			tInitStatus.m_tStatus.tVmp.m_byBoardVer = MPU2_BOARD;
		}
	}
  
    tInitStatus.m_tStatus.tVmp.m_byUseState = byState;

    KeyLog( "Before: alias: %s EqpId: %u EqpType: %u McuId: %u\n",
                                m_tCfg.achAlias, 
                                m_tEqp.GetEqpId(), 
                                m_tEqp.GetEqpType(), 
                                m_tEqp.GetMcuId() );
    KeyLog( "After: alias: %s EqpId: %u EqpType: %u McuId: %u\n",
                                m_tCfg.achAlias, 
                                tInitStatus.GetEqpId(), 
                                tInitStatus.GetEqpType(), 
                                tInitStatus.GetMcuId() );

   

	//取vmp的subtype
	u8 bySubType = GetSubTypeByWorkMode( g_cMpu2VmpApp.m_byWorkMode );
	tInitStatus.m_tStatus.tVmp.m_bySubType = bySubType; 
	
	KeyLog( "[SendStatusChangeMsg] mpu boardVer is %u\n", tInitStatus.m_tStatus.tVmp.m_byBoardVer);

    tInitStatus.SetAlias(m_tCfg.achAlias);
    cServMsg.SetMsgBody( (u8*)&(tInitStatus), sizeof(TPeriEqpStatus) );
    SendMsgToMcu( VMP_MCU_VMPSTATUS_NOTIF, cServMsg );
    return;
}

/*====================================================================
	函数  : GetSubTypeByWorkMode
	功能  : 通过workmode取得相应的vmpsubtype
	输入  : u8 byWorkMode
	输出  : u8 subtype
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
u8 CMpu2VmpInst::GetSubTypeByWorkMode(u8 byWorkMode)
{
	u8 bySubType = MPU2_VMP_BASIC;
	switch( byWorkMode ) 
	{
	case TYPE_MPUSVMP:
		bySubType = MPU_SVMP;
		break;
	case TYPE_MPU2VMP_BASIC:
		bySubType = MPU2_VMP_BASIC;
		break;
	case TYPE_MPU2VMP_ENHANCED:
		bySubType = MPU2_VMP_ENHACED;
		break;
	default:
		break;
	}
	return bySubType;
}

/*=============================================================================
  函 数 名： MsgUpdateVmpEncryptParamProc
  功    能： 更改加密参数
  算法实现： 
  全局变量： 
  参    数： CMessage * const pMsg
  返 回 值： void 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::MsgUpdateVmpEncryptParamProc( CMessage * const pMsg )
{ 
	//1、参数合法性校验
    if( NULL == pMsg )
	{
		ErrorLog( "[MsgUpdataVmpEncryptParamProc] Error input parameter.\n" );
		return;
	}
	//2、解析消息体
	u8  byChannelNo = 0;
    TMediaEncrypt  tVideoEncrypt;
    TDoublePayload tDoublePayload;

    CServMsg cServMsg( pMsg->content, pMsg->length );

    byChannelNo    = *(u8 *)( cServMsg.GetMsgBody() );
    tVideoEncrypt  = *(TMediaEncrypt *)( cServMsg.GetMsgBody() + sizeof(u8) );
    tDoublePayload = *(TDoublePayload *)( cServMsg.GetMsgBody() + sizeof(u8) + sizeof(TMediaEncrypt) );

    KeyLog( "byChannelNo: %d, ActivePayLoad: %d, RealPayLoad: %d\n", 
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

        KeyLog( "[Info]Channel-%u add return value is %u(correct is %u),the Ip is %u Port is %u!\n",
                                   byChannelNo, nRet, HARD_MULPIC_OK );

    }

	//3、更新payload
// 	if ( IsSimulateMpu() )
// 	{
// 		m_tMpuParamPrevious[0].m_tDoublePayload[byChannelNo].SetActivePayload(tDoublePayload.GetActivePayload());
// 		m_tMpuParamPrevious[0].m_tDoublePayload[byChannelNo].SetRealPayLoad(tDoublePayload.GetRealPayLoad());
// 
// 	}
// 	else
	{
		m_tPreviousVmpCommonAttrb.m_tDoublePayload[byChannelNo].SetActivePayload(tDoublePayload.GetActivePayload());
		m_tPreviousVmpCommonAttrb.m_tDoublePayload[byChannelNo].SetRealPayLoad(tDoublePayload.GetRealPayLoad());

	}

	//4、重设所有编码参数的加密参数
	// [7/31/2010 xliang] 重新设置编码参数
	for(u8 byEncChnl = 0; byEncChnl < m_byMaxVmpOutNum; byEncChnl++)
	{
		if(!m_bUpdateMediaEncrpyt[byEncChnl])
		{
			u8 byEncType = m_tParam[byEncChnl].GetEncType();
// 			if ( IsSimulateMpu() ) 
// 			{
// 				byEncType = m_tMpuParam[byEncChnl].m_byEncType;
// 			}
			SetEncryptParam( byEncChnl, &tVideoEncrypt, byEncType );
			m_bUpdateMediaEncrpyt[byEncChnl] = TRUE;
		}
	}


    
	cServMsg.SetMsgBody( NULL, 0 );
    SendMsgToMcu( VMP_MCU_CHANGESTATUS_NOTIF, cServMsg );

    SendStatusChangeMsg( (u8)ONLINE, (u8)USESTATE_START );

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
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::MsgFastUpdatePicProc( CMessage * const pMsg )
{
	//1、参数合法性校验
	if ( NULL == pMsg ) 
	{
		ErrorLog("[MsgFastUpdatePicProc]Null pointer CMessage\n");
		return;
	}
	//2、解析消息体，是请求哪个编码通道的关键帧
    CServMsg cServMsg( pMsg->content, pMsg->length );
	u8 byChnnlId = cServMsg.GetChnIndex();
	//3、1s内的重复请求过滤
	u32 dwTimeInterval = 1 * OspClkRateGet();
    u32 dwCurTick = OspTickGet();	
    if( dwCurTick - m_adwLastFUPTick[byChnnlId] > dwTimeInterval 
		)
    {
        m_adwLastFUPTick[byChnnlId] = dwCurTick;

		s32 nRet = m_cHardMulPic.SetFastUpdata( byChnnlId );
		if (nRet != HARD_MULPIC_OK)
		{
			ErrorLog("[MsgFastUpdatePicProc]m_cHardMulPic.SetFastUpdata(%u) failed!\n",byChnnlId );
		}
		else
		{
			KeyLog( "[MsgFastUpdatePicProc]m_cHardMulPic.SetFastUpdata(%u)! succeed\n",byChnnlId );
		}
        
    }
	
	// 开启后，先收到关键帧请求，单独请求关键帧处理失效
	m_bFastUpdate = FALSE;

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
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::MsgTimerNeedIFrameProc(  CMessage * const pMsg )
{
	//1、参数合法性校验
	if ( NULL == pMsg ) 
	{
		ErrorLog("[MsgTimerNeedIFrameProc]Null pointer CMessage\n");
		return;
	}
	//2、非RUNNING 状态不发请求关键帧（无意义）
	KillTimer(pMsg->event);
	if ( CurState() != RUNNING ) 
	{
		ErrorLog("[MsgTimerNeedIFrameProc]wrong state:%d\n",CurState());
		return;
	}
	//3、给mcu发请求关键帧消息
	u8 byChnNo = *(u8 *)pMsg->content;
	if ( byChnNo < MAXNUM_MPU2VMP_MEMBER )
	{
		CServMsg cServMsg;
		cServMsg.SetConfId( m_cConfId );
		TMulPicChnStatis tDecStatis;
		if(m_bAddVmpChannel[byChnNo])
		{
			memset(&tDecStatis, 0, sizeof(TMulPicChnStatis));
			m_cHardMulPic.GetDecStatis( byChnNo, tDecStatis );
			if ( tDecStatis.m_bWaitKeyFrame	)
			{			
				m_abChnlIframeReqed[byChnNo]=TRUE;
				cServMsg.SetChnIndex(byChnNo);
				SendMsgToMcu(VMP_MCU_NEEDIFRAME_CMD, cServMsg);
				KeyLog("[MsgTimerNeedIFrameProc]vmp channel:%d request iframe!!\n", byChnNo);
				KeyLog("[MsgTimerNeedIFrameProc]vmp channel:%d  set EV_VMP_NEEDIFRAME_TIMER: %d !\n", byChnNo, m_tMpuVmpCfg.GetIframeInterval());
				SetTimer(pMsg->event, m_tMpuVmpCfg.GetIframeInterval(),byChnNo);
			}
			else
			{
				//配置文件的定时未检测到，则根据标志位判断是否走配置文件 
				if ( m_abChnlIframeReqed[byChnNo]==TRUE )
				{
				
					DetailLog( "[MsgTimerNeedIFrameProc]vmp channel:%d  set EV_VMP_NEEDIFRAME_TIMER: %d !\n", byChnNo, m_tMpuVmpCfg.GetIframeInterval());
					SetTimer(pMsg->event,m_tMpuVmpCfg.GetIframeInterval(),byChnNo);
				}
				else
				{
				
					DetailLog( "[MsgTimerNeedIFrameProc]vmp channel:%d  set EV_VMP_NEEDIFRAME_TIMER: %d !\n", byChnNo, CHECK_IFRAME_INTERVAL);
					SetTimer(pMsg->event, CHECK_IFRAME_INTERVAL,byChnNo);
				}			
			}
		}
	}

	
}

/*=============================================================================
  函 数 名： ProcGetMsStatusRsp
  功    能： 处理取主备倒换状态
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::ProcGetMsStatusRsp( CMessage* const pMsg )
{
	//1、参数合法性校验
	if ( NULL == pMsg ) 
	{
		ErrorLog("[ProcGetMsStatusRsp]Null pointer CMessage\n");
		return;
	}
	//2、取主备倒换状态
	BOOL bSwitchOk = FALSE;
	CServMsg cServMsg( pMsg->content, pMsg->length );
	if( MCU_EQP_GETMSSTATUS_ACK == pMsg->event ) // Mcu响应消息
	{
		TMcuMsStatus *ptMsStatus = (TMcuMsStatus *)cServMsg.GetMsgBody();
        
        KillTimer( TIME_GET_MSSTATUS );
        KeyLog( "[ProcGetMsStatus]. receive msg MCU_EQP_GETMSSTATUS_ACK. IsMsSwitchOK :%d\n", 
                  ptMsStatus->IsMsSwitchOK() );

        if ( ptMsStatus->IsMsSwitchOK() ) // 倒换成功
        {
            bSwitchOk = TRUE;
        }
	}
	//3、 倒换失败或者超时
	if( !bSwitchOk )
	{
		ClearCurrentInst();
		KeyLog( "[ProcGetMsStatus] ClearCurrentInst!\n");

		if ( OspIsValidTcpNode(g_cMpu2VmpApp.m_dwMcuNode))
		{
			KeyLog( "[ProcGetMsStatus] OspDisconnectTcpNode A!\n");
			OspDisconnectTcpNode(g_cMpu2VmpApp.m_dwMcuNode );
		}
		if ( OspIsValidTcpNode(g_cMpu2VmpApp.m_dwMcuNodeB))
		{
			KeyLog( "[ProcGetMsStatus] OspDisconnectTcpNode B!\n");
			OspDisconnectTcpNode(g_cMpu2VmpApp.m_dwMcuNodeB );
		}

		if ( INVALID_NODE == g_cMpu2VmpApp.m_dwMcuNode )
		{
			KeyLog( "[ProcGetMsStatus] EV_VMP_CONNECT_TIMER A!\n");
			//SetTimer( EV_VMP_CONNECT_TIMER, MPU_CONNETC_TIMEOUT );
			post(MAKEIID(AID_MPU2,CInstance::DAEMON),EV_VMP_CONNECT_TIMER);

		}
		if ( INVALID_NODE == g_cMpu2VmpApp.m_dwMcuNodeB )
		{
			KeyLog( "[ProcGetMsStatus] EV_VMP_CONNECT_TIMER B!\n");
			//SetTimer( EV_VMP_CONNECT_TIMERB, MPU_CONNETC_TIMEOUT );
			post(MAKEIID(AID_MPU2,CInstance::DAEMON),EV_VMP_CONNECT_TIMERB);

		}
	}
	return;
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
void CMpu2VmpInst::ProcChangeMemAliasCmd( CMessage* const pMsg )
{
	//1、参数合法性校验
	if ( NULL == pMsg ) 
	{
		ErrorLog("[ProcChangeMemAliasCmd]Null pointer CMessage\n");
		return;
	}
	if ( CurState() != RUNNING ) 
	{
		ErrorLog( "[ProcChangeMemAliasCmd] Wrong State:%d !\n", CurState() );
		return;
	}
	//2、判断用户是否配置了要显示终端别名，没有配置则收到这个消息也不处理
	CServMsg cServMsg( pMsg->content, pMsg->length );
	u8 byChnNo = cServMsg.GetChnIndex();
	
	// 获取别名与通道，支持别名扩容，支持多通道同时更新
	s8 *pchAlias;
	u16 wMsgLen = 0;
	u8 byIndex = 0;
	s32 nReturn = Codec_Success;
	do 
	{
		pchAlias = (s8*)(cServMsg.GetMsgBody() + wMsgLen);
		if (strlen(pchAlias) > 0)
		{
			wMsgLen += MAXLEN_ALIAS;//别名长度
			byChnNo = *(cServMsg.GetMsgBody() + wMsgLen);
			wMsgLen++;
			if (byChnNo >= MAXNUM_MPU2VMP_MEMBER)
			{
				// 通道号校验
				break;
			}
			// 保存通道终端别名
			m_tVmpMbAlias[byChnNo].SetMbAlias(strlen(pchAlias), pchAlias);
			
			if ( TRUE == IsDisplayMbAlias() )
			{
				DisplayMbAlias(byChnNo,pchAlias);
			}
			
			//看是否有自定义图片要对应
			for (byIndex = 0; byIndex < MAXNUM_VMPDEFINE_PIC; byIndex++)
			{
				if(!m_tVmpCommonAttrb.m_atMtMember[byChnNo].IsNull() &&
					strlen(pchAlias) > 0 &&
					0 == strcmp(pchAlias, m_tMpuVmpCfg.GetVmpMemAlias(byIndex)))
				{
					//调用接口
					KeyLog( "[ProcChangeMemAliasCmd]index.%d -> alias.%s -> pic.%s!\n", 
						byChnNo, 
						m_tMpuVmpCfg.GetVmpMemAlias(byIndex), 
						m_tMpuVmpCfg.GetVmpMemRoute(byIndex));
					
					nReturn = SetNoStreamBakByBmpPath((u32)EN_PLAY_BMP_USR,m_tMpuVmpCfg.GetVmpMemRoute(byIndex),byChnNo);
					if ( (s32)Codec_Success != nReturn )
					{
						ErrorLog( "[ProcChangeMemAliasCmd] Index.%d SetNoStreamBak usrbmp failed(ErrCode.%d)!\n", byChnNo, nReturn);
					}
					break;
				}
			}
		}
		else
		{
			wMsgLen += MAXLEN_ALIAS;//别名长度			
			wMsgLen++;
		}
	} while (cServMsg.GetMsgBodyLen() > wMsgLen);

	return;
}

/*==============================================================================
函数名    :  MsgAddRemoveRcvChnnl
功能      :  增加/删除某接收通道
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2012/11/19  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::MsgAddRemoveRcvChnnl(CMessage * const pMsg)
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
		if (IsDisplayMbAlias())
		{
			nRet = m_cHardMulPic.StopAddIcon(byChnNo);
			KeyLog("[MsgAddRemoveRcvChnnl]Stop AddIcon chnl:%d,ret:%d\n",byChnNo,nRet);
		}
		if ( HARD_MULPIC_OK != nRet )
		{
			ErrorLog( "[MsgAddRemoveRcvChnnl] Remove channel.%d error, ret.%d(correct is %u)\n", byChnNo, nRet, HARD_MULPIC_OK);
			return;
		}
		else
		{
			KeyLog("[MsgAddRemoveRcvChnnl]RemoveChannel:%d successfully \n",byChnNo);
			m_bAddVmpChannel[byChnNo] = FALSE;
// 			if (IsSimulateMpu())
// 			{
// 				m_tMpuParam[0].m_atMtMember[byChnNo].SetNull();
// 				m_tMpuParam[0].m_tDoublePayload[byChnNo].Reset();
// 				
// 			}
// 			else
			{
				m_tVmpCommonAttrb.m_atMtMember[byChnNo].SetNull();
				m_tVmpCommonAttrb.m_tDoublePayload[byChnNo].Reset();
				memset(&m_tVmpMbAlias[byChnNo], 0, sizeof(m_tVmpMbAlias[byChnNo]));
				//设置边框和颜色
				if ( !SetVmpAttachStyle( m_tStyleInfo ) )
				{
					ErrorLog( " Failed to set vmp attach style (MsgAddRemoveRcvChnnl)\n" );
					return;
				}
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
			ErrorLog(  "[MsgAddRemoveRcvChnnl] AddChannel.%u error, ret.%u(correct is %u)\n", byChnNo, nRet, HARD_MULPIC_OK );
			return;
		}
		else
		{    
			KeyLog("[MsgAddRemoveRcvChnnl]addchnnel:%d successfully \n",byChnNo);
			m_bAddVmpChannel[byChnNo] = TRUE;
			//设置对应通道的解码参数
			StartMediaRcv(byChnNo);
		}
	}
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
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
BOOL CMpu2VmpInst::ChangeVMPStyle()
{
    s32 nRet = 0;

    //1、获取新的风格
	u8 byStyle = ConvertVcStyle2HardStyle( m_tVmpCommonAttrb.m_byVmpStyle );
// 	if ( IsSimulateMpu() ) //模拟mpu
// 	{
// 		byStyle = ConvertVcStyle2HardStyle( m_tMpuParam[0].m_byVMPStyle );
// 	}
	
    KeyLog( "[ChangeVMPStyle] OldHardStyle: %d, NewHardStyle: %d\n",m_byHardStyle, byStyle  );

    //2、风格切变
    if ( m_byHardStyle != byStyle )
    {
        //先停止旧风格下的通道接收
        u8 byOldStyle = ConvertHardStyle2VcStyle(m_byHardStyle);
        u8 byOldMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle(byOldStyle);

		TVMPMemberEx tLoopMember, tLoopPreviousMember;
        for (u8 byChnNo = 0; byChnNo < byOldMaxChnNum; byChnNo ++)
        {
			// 新旧成员不一致时才需要先停再开
			tLoopMember = m_tVmpCommonAttrb.m_atMtMember[byChnNo];
			tLoopPreviousMember = m_tPreviousVmpCommonAttrb.m_atMtMember[byChnNo];
			if (tLoopMember == tLoopPreviousMember)
			{
				continue;
			}
            m_pcMediaRcv[byChnNo]->StopRcv();
        }
        //切风格
        nRet = m_cHardMulPic.SetMulPicType( byStyle );
        if ( HARD_MULPIC_OK != nRet )
        {
            ErrorLog( " Fail to call: SetMulPicType, Error code: %d\n", nRet );
            return FALSE;
        }

        m_byHardStyle = byStyle;
    }

    return TRUE;
}




/*=============================================================================
  函 数 名： ConvertHardStyle2VcStyle
  功    能： 由底层风格得到会控风格
  算法实现： 
  全局变量： 
  参    数： u8 byHardStyle
  返 回 值： u8 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
u8 CMpu2VmpInst::ConvertHardStyle2VcStyle(u8 byHardStyle)
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

	case MULPIC_TYPE_THREE_2IBR:	   //三画面，2个小的在底部靠右
		byStyle = (u8)VMP_STYLE_THREE_2BOTTOMRIGHT; 
		break;

	case MULPIC_TYPE_THREE_2IB :	   //三画面，2个小的在底部两边
		byStyle = (u8)VMP_STYLE_THREE_2BOTTOM2SIDE; 
		break;

	case MULPIC_TYPE_THREE_2IBL :	   //三画面，2个小的在底部靠左
		byStyle = (u8)VMP_STYLE_THREE_2BOTTOMLEFT; 
		break;

	case MULPIC_TYPE_VTHREE :	    //垂直等大3画面
		byStyle = (u8)VMP_STYLE_THREE_VERTICAL; 
		break;

	case MULPIC_TYPE_FOUR_3IB :	  //四画面，3个在底部中央
		byStyle = (u8)VMP_STYLE_FOUR_3BOTTOMMIDDLE; 
		break;
		
	case MULPIC_TYPE_TEN_MID_LR :	  //十画面，中间两个，上下各4个
		byStyle = (u8)VMP_STYLE_TEN_M; 
		break;

	case MULPIC_TYPE_THIRTEEN_MID :	  //十三画面，中间一个大的，环绕中间型
		byStyle = (u8)VMP_STYLE_THIRTEEN_M; 
		break;
	case MULPIC_TYPE_TWENTYFIVE : //二十五画面
		byStyle = (u8)VMP_STYLE_TWENTYFIVE;
		break;
        
    default:
        byStyle = VMP_STYLE_VTWO;    // 默认两画面：左右分
        break;
    }
    return byStyle;
}

/*=============================================================================
  函 数 名： SetVmpAttachStyle
  功    能： 设置复合风格
  算法实现： 
  全局变量： 
  参    数： TVmpStyleCfgInfo
  返 回 值： void 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
BOOL32 CMpu2VmpInst::SetVmpAttachStyle( TVmpStyleCfgInfo&  tStyleInfo )
{


	//1、得到stylecfginfo里面图像背景色，对应主席，发言人，观众的边框颜色
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

	//2、根据各个接收通道的成员的属性，设置该通道对应的边框颜色
    TMulPicColor atSlidelineColor[/*MAXNUM_MPUSVMP_MEMBER*/MULPIC_MAX_CHNS];
    memset( atSlidelineColor, 0, sizeof(atSlidelineColor) );
	
	s32 nRet = 0;
    u8 byLoop;
    for ( byLoop = 0; byLoop < MAXNUM_MPU2VMP_MEMBER; byLoop++ )
    {

		TVMPMemberEx tLoopMember ;
		tLoopMember.SetNull();
// 		if ( IsSimulateMpu() ) //模拟mpu
// 		{
// 			tLoopMember = m_tMpuParam[0].m_atMtMember[byLoop];
// 		}
// 		else
		{
			tLoopMember  = m_tVmpCommonAttrb.m_atMtMember[byLoop];
		}

        if (  tLoopMember.IsNull() )//fanchao要求，无终端通道也要设置为观众的颜色
        {
			atSlidelineColor[byLoop].RColor = 246;
            atSlidelineColor[byLoop].GColor = 246;
            atSlidelineColor[byLoop].BColor = 246;
            continue;
        }
		
        if ( MT_STATUS_AUDIENCE == tLoopMember.GetMemStatus() )
        {
            atSlidelineColor[byLoop].RColor = byRColorAudience;
            atSlidelineColor[byLoop].GColor = byGColorAudience;
            atSlidelineColor[byLoop].BColor = byBColorAudience;
			
            KeyLog( "The channel: %d, The Audience Color Value: %0x\n", byLoop, dwAudienceColor);
        }

        else if ( MT_STATUS_SPEAKER == tLoopMember.GetMemStatus() )
        {
            atSlidelineColor[byLoop].RColor = byRColorSpeaker;
            atSlidelineColor[byLoop].GColor = byGColorSpeaker;
            atSlidelineColor[byLoop].BColor = byBColorSpeaker;
            KeyLog( "The channel: %d, The Speaker  Color Value: %0x\n", byLoop, dwSpeakerColor);
        }
        else if ( MT_STATUS_CHAIRMAN == tLoopMember.GetMemStatus() )
        {
            atSlidelineColor[byLoop].RColor = byRColorChair;
            atSlidelineColor[byLoop].GColor = byGColorChair;
            atSlidelineColor[byLoop].BColor = byBColorChair;
            KeyLog( "The channel: %d, The Chairman Color Value: %0x\n", byLoop, dwChairColor);
        }

        if (m_tStyleInfo.GetIsRimEnabled())
        {
            atSlidelineColor[byLoop].byDefault = 1;
        }
		else
		{
			atSlidelineColor[byLoop].byDefault = 0;
		}

		OspPrintf(TRUE,FALSE,"[SetVipChnl]--%d\n",atSlidelineColor[byLoop].byDefault);
		nRet = m_cHardMulPic.SetVipChnl(atSlidelineColor[byLoop], byLoop);
		if ( HARD_MULPIC_OK != nRet )
		{
			ErrorLog( " Fail to call the SetVipChnl. (ProcSetVmpAttachStyle)\n" );
			return FALSE;
		}
    }
    KeyLog( "The Background Color Value: 0x%x\n", tStyleInfo.GetBackgroundColor() );
    KeyLog( "The RGB Color of Chairman : R<%d> G<%d> B<%d> \n", byRColorChair,    byGColorChair,    byBColorChair );
    KeyLog( "The RGB Color of Speaker  : R<%d> G<%d> B<%d> \n", byRColorSpeaker,  byGColorSpeaker,  byBColorSpeaker );
    KeyLog( "The RGB Color of Audience : R<%d> G<%d> B<%d> \n", byRColorAudience, byGColorAudience, byBColorAudience );
    KeyLog( "The RGB Color of Backgroud: R<%d> G<%d> B<%d> \n", tBGDColor.RColor, tBGDColor.GColor, tBGDColor.BColor );
	
    

	//3、画面合成风格背景加边框
	//fanchao要求mpu2调用SetBGDAndSidelineColor的时候都填TRUE
// 	if ( m_tStyleInfo.GetIsRimEnabled() )
// 	{
		u32 dwTick = OspTickGet();
 		nRet = m_cHardMulPic.SetBGDAndSidelineColor( tBGDColor, atSlidelineColor, TRUE );
		KeyLog("============= m_cHardMulPic.SetBGDAndSidelineColor Tick:%d\n",OspTickGet()-dwTick);
//  	}
//  	else
//  	{
// 		u32 dwTick = OspTickGet();
//  		nRet = m_cHardMulPic.SetBGDAndSidelineColor( tBGDColor, atSlidelineColor, FALSE );
// 		KeyLog("============= m_cHardMulPic.SetBGDAndSidelineColor Tick:%d\n",OspTickGet() -dwTick);
//  	}
    
    
	KeyLog( "Set VMP Scheme:%d with RimEnabled:%d successfully !\n", m_tStyleInfo.GetSchemeId(), m_tStyleInfo.GetIsRimEnabled() );
    
	return (HARD_MULPIC_OK == nRet);
}

/*=============================================================================
  函 数 名： GetRGBColor
  功    能： 取得颜色对应的R,G,B值
  算法实现： 
  全局变量： 
  参    数： u32 dwColor
             u8& byRColor
             u8& byGColor
             u8& byBColor
  返 回 值： void 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::GetRGBColor( u32 dwColor, u8& byRColor, u8& byGColor, u8& byBColor )
{
    byRColor = (u8)dwColor & 0x000000ff;
    byGColor = (u8)( (dwColor & 0x0000ff00) >> 8 );
    byBColor = (u8)( (dwColor & 0x00ff0000) >> 16 );
    return;
}

/*=============================================================================
    函 数 名： VmpGetActivePayload
    功    能： 通过RealPayload取对应的ActivePayload
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： u8
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
u8 CMpu2VmpInst::VmpGetActivePayload( u8 byRealPayloadType )
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
	功能    :开始网络收发
	算法实现： 
    全局变量： 
    参    数： u8 byNeedPrs:是否需要丢包重传
    返 回 值： BOOL
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
BOOL CMpu2VmpInst::StartNetRcvSnd(u8 byNeedPrs)
{

	//1. 编码发送相关
    u8 byChnNo = 0;
	//u8 byActPt = 0;

	// media encrypt modify
	TMediaEncrypt tMediaEncrypt;
	s32 nKeyLen = 0;
	u8  abyKeyBuf[MAXLEN_KEY] = {0};
	for (byChnNo = 0; byChnNo < MAXNUM_MPU2VMP_MEMBER; byChnNo ++)
	{
// 		if ( IsSimulateMpu() ) //模拟mpu
// 		{
// 			if ( CONF_ENCRYPTMODE_NONE == m_tMpuParam[0].m_tVideoEncrypt[byChnNo].GetEncryptMode() )
// 			{
// 				continue;
// 			}
// 			m_tMpuParam[0].m_tVideoEncrypt[byChnNo].GetEncryptKey( abyKeyBuf, &nKeyLen );
// 			tMediaEncrypt.SetEncryptMode(m_tMpuParam[0].m_tVideoEncrypt[byChnNo].GetEncryptMode());
// 			tMediaEncrypt.SetEncryptKey( abyKeyBuf, nKeyLen );
// 			break;
// 		}
// 		else
		{
			if ( CONF_ENCRYPTMODE_NONE == m_tVmpCommonAttrb.m_tVideoEncrypt[byChnNo].GetEncryptMode() )
			{
				continue;
			}
			m_tVmpCommonAttrb.m_tVideoEncrypt[byChnNo].GetEncryptKey( abyKeyBuf, &nKeyLen );
			tMediaEncrypt.SetEncryptMode(m_tVmpCommonAttrb.m_tVideoEncrypt[byChnNo].GetEncryptMode());
			tMediaEncrypt.SetEncryptKey( abyKeyBuf, nKeyLen );
			break;
		}
		
	}

    for ( byChnNo = 0; byChnNo < m_byValidVmpOutNum; byChnNo++ )
    {

        InitMediaSnd( byChnNo );
		u8 byEncType = m_tParam[byChnNo].GetEncType();
// 		if ( IsSimulateMpu() ) 
// 		{
// 			byEncType = m_tMpuParam[byChnNo].m_byEncType;
// 		}
// 		
        SetEncryptParam( byChnNo, &tMediaEncrypt, byEncType );
		if (NULL != m_pcMediaSnd[byChnNo])
		{
			u32 dwTick = OspTickGet();
			m_cHardMulPic.SetVidDataCallback( byChnNo, VMPCALLBACK, m_pcMediaSnd[byChnNo] );
			KeyLog("==============[StartNetRcvSnd]after m_cHardMulPic.SetVidDataCallback(%d) Tick:%d\n",byChnNo,OspTickGet() - dwTick);
		}
		
       
    }

    //发送重传设置
    SetPrs(byNeedPrs);


    //2. 解码接收相关
	for (byChnNo = 0; byChnNo < MAXNUM_MPU2VMP_MEMBER; byChnNo ++)
	{
        //MediaRcv 对象设置网络参数
        SetDecryptParam( byChnNo, &tMediaEncrypt/*, &m_tParam[0].m_tDoublePayload[byChnNo]*/ );
		SetMediaRcvNetParam( byChnNo);
	}

	u8 byMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle(m_tVmpCommonAttrb.m_byVmpStyle);
// 	if ( IsSimulateMpu() )
// 	{
// 		 byMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle(m_tMpuParam[0].m_byVMPStyle);
// 	}
   
    for ( byChnNo = 0; byChnNo < byMaxChnNum; byChnNo++ )
    {
		TVMPMemberEx tLoopMember;
		tLoopMember.SetNull();
// 		if ( IsSimulateMpu() ) 
// 		{
// 			tLoopMember = m_tMpuParam[0].m_atMtMember[byChnNo];
// 		}
// 		else
		{
			tLoopMember = m_tVmpCommonAttrb.m_atMtMember[byChnNo];
		}
        if ( !tLoopMember.IsNull() )
        { 
            //设置对应通道的解码参数
            StartMediaRcv(byChnNo);
        }
        else
        {
			m_pcMediaRcv[byChnNo]->StopRcv();
			KeyLog( "%d StopRcv!\n", byChnNo);
        }
    }

    return TRUE;
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
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
BOOL CMpu2VmpInst::ChangeChnAndNetRcv(/*u8 byNeedPrs,*/ BOOL32 bStyleChged)
{
    s32 nRet = 0;
    
    //1、整理通道，重新开始接收
    u8 byMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle(m_tVmpCommonAttrb.m_byVmpStyle);
// 	if ( IsSimulateMpu() )//模拟mpu
// 	{
// 		byMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle(m_tMpuParam[0].m_byVMPStyle);
// 	}

    for(u8 byChnNo = 0; byChnNo < MAXNUM_MPU2VMP_MEMBER; byChnNo++ )
    {
		if ( byChnNo >= byMaxChnNum )
		{
			m_bAddVmpChannel[byChnNo] = FALSE;
			continue;
		}
		TVMPMemberEx tLoopMember, tLoopPreviousMember;
		tLoopMember.SetNull();
		tLoopPreviousMember.SetNull();
		TDoublePayload tLoopPreDoublePayload;
// 		if ( IsSimulateMpu() )
// 		{
// 			tLoopMember = m_tMpuParam[0].m_atMtMember[byChnNo];
// 			tLoopPreviousMember = m_tMpuParamPrevious[0].m_atMtMember[byChnNo];
// 			tLoopPreDoublePayload = m_tMpuParamPrevious[0].m_tDoublePayload[byChnNo];
// 		}
// 		else
		{
			tLoopMember = m_tVmpCommonAttrb.m_atMtMember[byChnNo];
			tLoopPreviousMember = m_tPreviousVmpCommonAttrb.m_atMtMember[byChnNo];
			tLoopPreDoublePayload = m_tPreviousVmpCommonAttrb.m_tDoublePayload[byChnNo];
		}
		//通道成员为null，Remove
        if ( tLoopMember.IsNull() )
        {
			if ( TRUE == m_bAddVmpChannel[byChnNo] )
			{
				nRet = m_cHardMulPic.RemoveChannel( byChnNo ); 
				KeyLog( "[ChangeChnAndNetRcv] Remove channel.%d, ret.%d(correct is %u)\n", byChnNo, nRet, HARD_MULPIC_OK);
				if (IsDisplayMbAlias())
				{
					nRet = m_cHardMulPic.StopAddIcon(byChnNo);
					KeyLog("[ChangeChnAndNetRcv]Stop AddIcon chnl:%d,ret:%d\n",byChnNo,nRet);
				}
			}
            
            m_bAddVmpChannel[byChnNo] = FALSE;
        }
        else
        {
            // zbq[05/14/2009] 非风格切变，对于已经存在的通道不再执行本操作
            if ( !tLoopPreviousMember.IsNull()
                 && tLoopPreDoublePayload.GetRealPayLoad() != MEDIA_TYPE_NULL	// xliang [5/18/2009] 适应模板启自动画面合成的情况
                )
            {
                // 统一字体大小为22号 [8/5/2013 liaokang]
                // 刷别名，防止风格切换而台标的字体大小有问题 [7/8/2013 liaokang]
                //m_cHardMulPic.StopAddIcon(byChnNo);
                //DisplayMbAlias(byChnNo,m_tVmpMbAlias[byChnNo].GetMbAlias());

				// [pengjie 2010/2/9] 设置对应通道的解码参数，mpu2切风格时前后通道成员相等时不先停再开
				if( !(tLoopMember == tLoopPreviousMember)/* || bStyleChged */)
				{	
                    DisplayMbAlias(byChnNo,m_tVmpMbAlias[byChnNo].GetMbAlias());
					//DisplayMbAlias(byChnNo,m_tVmpMbAlias[byChnNo].GetMbAlias());
					KeyLog( "[ChangeChnAndNetRcv] chn.%d, StartMediaRcv because of need StyleChged or ChnnlMember change!\n", byChnNo);
					StartMediaRcv(byChnNo);
					
				}
				// End 

                KeyLog( "[ChangeChnAndNetRcv] chn.%d eq to previous one, check next directly!\n", byChnNo);
                continue;
            }
            
            // 添加通道
            nRet = m_cHardMulPic.AddChannel( byChnNo );
            if ( HARD_MULPIC_OK != nRet )
            {
				ErrorLog("[ChangeChnAndNetRcv]AddChannel:%d failed.\n",byChnNo);
				m_bAddVmpChannel[byChnNo] = FALSE;
				continue;
            }
            else
            {        
                m_bAddVmpChannel[byChnNo] = TRUE;
				if ( IsDisplayMbAlias() )
				{
					DisplayMbAlias(byChnNo,m_tVmpMbAlias[byChnNo].GetMbAlias());
				}
				
            }
            KeyLog( "[ChangeChnAndNetRcv] AddChannel.%u, ret.%u(correct is %u)\n", byChnNo, nRet, HARD_MULPIC_OK );
            
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
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
BOOL CMpu2VmpInst::StopNetRcvSnd()
{
    u8 byChnNo = 0;
    //1、停止20路收
    for ( ; byChnNo < MAXNUM_MPU2VMP_MEMBER; byChnNo++ )
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
    
    //2、更新编码密钥（清空）
    for ( byChnNo = 0; byChnNo < m_byMaxVmpOutNum; byChnNo++ )
    {
        TMediaEncrypt tVideoEncrypt;
        memset( &tVideoEncrypt, 0, sizeof(TMediaEncrypt) );
        SetEncryptParam( byChnNo, &tVideoEncrypt, 0 );
		m_bUpdateMediaEncrpyt[byChnNo] = FALSE;

    }

    //清空PRS
    SetPrs(0);////////////Prs清空、、、、、

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
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::SetEncryptParam( u8 byChnNo, TMediaEncrypt * ptVideoEncrypt, u8 byRealPT )
{
	//1、参数合法性校验
    if( NULL == ptVideoEncrypt )
	{
		ErrorLog("[SetEncryptParam]ERROR:chnl:%d null pointer\n");
		return;
	}
	//2、设置发送对象的加密参数
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

		KeyLog( "[SetEncryptParam]OutChannel.%u: no encrypt, so set key to NULL, set ActivePt.%u\n",
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

        KeyLog( "Mode: %u KeyLen: %u PT: %u \n",
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
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::InitMediaSnd( u8 byChnNo )
{
	//
    u8 byFrameRate = m_tMpuVmpCfg.m_tVideoEncParam[byChnNo].m_byFrameRate;
    u8 byMediaType = m_tMpuVmpCfg.m_tVideoEncParam[byChnNo].m_byEncType;
    u32 dwNetBand  = m_tMpuVmpCfg.m_adwMaxSendBand[byChnNo];

//    // 适应MCU的群组优化调整，作参数保护
//    if ( byFrameRate > 60 )
//    {
//        byFrameRate = 30;
//    }

	//1、最大网络发送带宽调整
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
    KeyLog( "[InitMediaSnd]m_cMediaSnd[%d]: dwNetBand = %d\n",byChnNo, dwNetBand);


    //2、Create 发送对象
    s32 wRet = m_pcMediaSnd[byChnNo]->Create( MAX_VIDEO_FRAME_SIZE,
		    	                      dwNetBand,
			                          byFrameRate,
			    				      byMediaType );

    if ( MEDIANET_NO_ERROR != wRet )
    {
        ErrorLog( "[InitMediaSnd] m_cMediaSnd[%d].Create fail, Error code is:%d, NetBand.%d, FrmRate.%d, MediaType.%d\n", byChnNo, wRet, dwNetBand, byFrameRate, byMediaType );
    	return;
    }
    else
    {
        KeyLog( "[InitMediaSnd] m_cMediaSnd[%d].Create as NetBand.%d, FrmRate.%d, MediaType.%d\n", byChnNo, dwNetBand, byFrameRate, byMediaType );
    }
	
	//3、设置HDFlag
	BOOL32 bHDFlag = TRUE;

	//h264 CIF, other 不置HD Flag; 其他h264的4cif，720，1080均置HD Flag
	if (GetEncCifChnNum() == byChnNo ||  GetEncOtherChnNum() == byChnNo) 
	{
		bHDFlag = FALSE;
	}
	m_pcMediaSnd[byChnNo]->SetHDFlag( bHDFlag );
	KeyLog( "[InitMediaSnd] m_cMediaSnd[%d].SetHDFlag: %d\n", byChnNo, bHDFlag );

	//4、设置发送通道的netparam
    SetMediaSndNetParam(byChnNo);

    return;
}

/*=============================================================================
  函 数 名： SetMediaRcvNetParam
  功    能： 设置Medianet Receive对象网络参数
  算法实现： 
  全局变量： 
  参    数： u8 byChnNo        ：通道号
  返 回 值： BOOL 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
BOOL CMpu2VmpInst::SetMediaRcvNetParam( u8 byChnNo)
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
	//16~23	===> 22~29	
	//24~25 ====> 32~33	
	u16 wRtcpBackPort;
	if (byChnNo >= 24)
	{
		wRtcpBackPort = m_wMcuRcvStartPort+8+byChnNo;
	}
	else if (byChnNo >= 16)
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
	
    KeyLog( "m_cMediaRcv[%d]:\n", byChnNo );
    KeyLog( "\t tlocalNetParm.m_tLocalNet.m_wRTPPort[%d]:\n", tlocalNetParm.m_tLocalNet.m_wRTPPort );
    KeyLog( "\t tlocalNetParm.m_tLocalNet.m_wRTCPPort[%d]:\n", tlocalNetParm.m_tLocalNet.m_wRTCPPort );
    KeyLog( "\t tlocalNetParm.m_dwRtcpBackAddr[%s]:\n", StrOfIP(tlocalNetParm.m_dwRtcpBackAddr) );
    KeyLog( "\t tlocalNetParm.m_wRtcpBackPort[%d]:\n", tlocalNetParm.m_wRtcpBackPort );
	
	s32 nRet = 0;
    nRet = m_pcMediaRcv[byChnNo]->SetNetRcvLocalParam( tlocalNetParm );
    if( MEDIANET_NO_ERROR !=  nRet )
    {
        ErrorLog( " m_cMediaRcv[%d].SetNetRcvLocalParam fail, Error code is: %d \n", byChnNo, nRet );        
        return FALSE;
	}
    else
    {
        KeyLog( "m_cMediaRcv[%d].SetNetRcvLocalParam succeed!\n", byChnNo );
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
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
BOOL32 CMpu2VmpInst::InitMediaRcv( u8 byChnNo )
{
	//1、校验参数，设置接收回调
	u16 byInstIdx = GetInsID();
	if ( byInstIdx > MAXNUM_MPU2_EQPNUM || byInstIdx < 1 || byChnNo >= MAXNUM_MPU2VMP_MEMBER) 
	{
		ErrorLog("[InitMediaRcv]Invalid param,InstID:%d,byChnNo:%d\n",byInstIdx,byChnNo);
		return FALSE;
	}
    g_cMpu2VmpApp.m_atMpu2VmpRcvCB[byInstIdx -1][byChnNo].m_byChnnlId = byChnNo;
    g_cMpu2VmpApp.m_atMpu2VmpRcvCB[byInstIdx -1][byChnNo].m_pHardMulPic  = &m_cHardMulPic;
	//2、create
    s32 wRet = m_pcMediaRcv[byChnNo]->Create( MAX_VIDEO_FRAME_SIZE,
			                                  CBMpuSVmpRecvFrame,   
				    		                  (u32)&g_cMpu2VmpApp.m_atMpu2VmpRcvCB[byInstIdx -1][byChnNo] );

    if( MEDIANET_NO_ERROR !=  wRet )
	{
		ErrorLog( " m_cMediaRcv[%d].Create fail, Error code is: %d \n", byChnNo, wRet );
		return FALSE;
	}
    else
    {
        KeyLog( "m_cMediaRcv[%d].Create succeed!\n", byChnNo );
    }

	//媒控：medianet接收对象不要调用SetHdFlag接口或者设置参数为false。
	//如果设置为TRUE，net组帧时会加一个2K的头，这个头是不需要的，只会增加负担。
    m_pcMediaRcv[byChnNo]->SetHDFlag( FALSE );

    return TRUE;
}
/*=============================================================================
  函 数 名： StartMediaRcv
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 byChnNo        ：通道号
  返 回 值： void 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::StartMediaRcv(u8 byChnNo)
{
	if (byChnNo >= MAXNUM_MPU2VMP_MEMBER)
	{
		ErrorLog( "[StartMediaRcv] wrong input param byChnNo = %d\n", byChnNo);
//		printf("[StartMediaRcv] wrong input param byChnNo = %d\n", byChnNo);
        return;
	}

	if (NULL == m_pcMediaRcv[byChnNo])
	{
		ErrorLog( "[StartMediaRcv] m_pcMediaRcv[%d] still null\n", byChnNo);
//		printf("[StartMediaRcv] m_pcMediaRcv[%d] still null\n", byChnNo);
		return;
	}

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
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::SetDecryptParam(u8 byChnNo,
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
		KeyLog( "Set key to NULL, PT(real): %u, SetActivePt result is:%d \n", MEDIA_TYPE_H264, wRet );
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

        KeyLog( "Mode: %u KeyLen: %u PT: %u\n",
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
	日  期      版本        修改人        修改内容
    2011/12/07  4.7         周翼亮          创建
====================================================================*/
void CMpu2VmpInst::SetPrs( u8 byNeedPrs )
{
	TRSParam tNetRSParam;
    if ( byNeedPrs )		//是否需要包重传？ 1: 重传, 0: 不重传
	{		
		KeyLog( "[SetPrs] needed Prs!\n" );
		//regack发过来的timespan是网络序,转主机序传
        tNetRSParam.m_wFirstTimeSpan  = ntohs(m_tPrsTimeSpan.m_wFirstTimeSpan);
		tNetRSParam.m_wSecondTimeSpan = ntohs(m_tPrsTimeSpan.m_wSecondTimeSpan);
		tNetRSParam.m_wThirdTimeSpan  = ntohs(m_tPrsTimeSpan.m_wThirdTimeSpan);
		tNetRSParam.m_wRejectTimeSpan = ntohs(m_tPrsTimeSpan.m_wRejectTimeSpan);
		
        s32	wRet;
        u8 byIndex = 0;
        for ( byIndex = 0; byIndex < m_byMaxVmpOutNum; byIndex++ )
        {
            wRet = m_pcMediaSnd[byIndex]->ResetRSFlag( 2000, TRUE );
            if( MEDIANET_NO_ERROR != wRet )
            {
	            ErrorLog( "m_cMediaSnd[%d].ResetRSFlag fail,Error code is:%d\n", byIndex, wRet );
	            continue;
            }
        }
        for ( byIndex = 0; byIndex < MAXNUM_MPU2VMP_MEMBER; byIndex++ )
        {
         
			//TODO:mpu2是否和mpu一样？？？？？？？
            GetRSParamByPos(byIndex, tNetRSParam);

            wRet = m_pcMediaRcv[byIndex]->ResetRSFlag( tNetRSParam, TRUE );
	        if( MEDIANET_NO_ERROR != wRet )
	        {
		        ErrorLog( "m_cMediaRcv[%d].ResetRSFlag fail,Error code is:%d\n", byIndex, wRet );
		        return;
	        }
            else
            {
                KeyLog( "m_cMediaRcv[%d].ResetRSParam<F:%d, S.%d, T.%d, R.%d>\n",
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
		KeyLog( "[SetPrs] not need Prs!\n" );
		return;
	}
}



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
void CMpu2VmpInst::SetSmoothSendRule(BOOL32 bSetFlag)
{
#ifdef _LINUX_    
	s32 nRet = 0;
	
	if (bSetFlag)
	{
		//设置平滑规则
		for (u8 byChannlNO = 0; byChannlNO < m_byValidVmpOutNum; byChannlNO++)
		{
			//还原打折前码率，平滑发送参数不能取编码码率
			u32 dwRate 	= m_tParam[byChannlNO].GetBitRate();
			u32 dwPrevRate = m_tParamPrevious[byChannlNO].GetBitRate(); 
//  			if ( IsSimulateMpu() )
//  			{
//  				dwRate = m_tMpuParam[byChannlNO].GetBitRate();
//  				dwPrevRate = m_tMpuParamPrevious[byChannlNO].GetBitRate();
//  			}
 			if (dwRate == dwPrevRate)
			{
 				KeyLog("[SetSmoothSendRule] previous and now param bitrate the same ,no need setssrule !\n");
 				continue;
 			}

			dwRate = m_tMpuVmpCfg.GetOrigRate(dwRate);    
			dwRate = dwRate >> 3;
			//峰值为120%
			u32 dwPeak = dwRate + dwRate / 5; 
			KeyLog( "[SetSmoothSendRule] Setting ssrule for 0x%x:%d, rate/peak=%u/%u KByte.\n",
				m_dwMcuRcvIp, m_wMcuRcvStartPort + byChannlNO * PORTSPAN, dwRate, dwPeak );
			//nRet = SetSSRule(m_dwMcuRcvIp, m_wMcuRcvStartPort + byChannlNO * PORTSPAN, dwRate, dwPeak, 8);	
			nRet = BatchAddSSRule(m_dwMcuRcvIp, m_wMcuRcvStartPort + byChannlNO * PORTSPAN, dwRate, dwPeak, 8);
			if ( 0 == nRet )
			{
				ErrorLog( "[SetSmoothSendRule]BatchAddSSRule failed. SSErrno=%d. \n", SSErrno);
			}
		}	
		nRet = BatchExecSSRule();
		if (0 == nRet)
		{
			ErrorLog( "[SetSmoothSendRule]BatchExecSSRule failed. SSErrno=%d. \n", SSErrno);
		}
	}
	else
	{
		//清除平滑规则
		for (u8 byChannlNO = 0; byChannlNO < m_byValidVmpOutNum; byChannlNO++)
		{
			KeyLog( "[SetSmoothSendRule] Unsetting ssrule for 0x%x:%d.\n",
				m_dwMcuRcvIp, m_wMcuRcvStartPort + byChannlNO * PORTSPAN );
			//nRet = UnsetSSRule( m_dwMcuRcvIp, m_wMcuRcvStartPort + byChannlNO * PORTSPAN );	
			nRet = BatchAddUnsetSSRule( m_dwMcuRcvIp, m_wMcuRcvStartPort + byChannlNO * PORTSPAN );	
			if ( 0 == nRet )
			{
				ErrorLog( "[SetSmoothSendRule] BatchAddUnsetSSRule  failed. SSErrno=%d. \n", SSErrno);
            }
		}
		nRet = BatchExecUnsetSSRule();
		if (0 == nRet)
		{
			ErrorLog( "[SetSmoothSendRule]BatchExecUnsetSSRule failed. SSErrno=%d. \n", SSErrno);
		}
	}
	

#endif
	return;
}
/*lint -restore*/



/*=============================================================================
函 数 名： ProcSetFrameRateCmd
功    能： 设置编码帧率
算法实现： 
全局变量： 
参    数：CMessage
返 回 值： void
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2011/12/07  			周翼亮                  创建
=============================================================================*/
void CMpu2VmpInst::ProcSetFrameRateCmd( CMessage * const pcMsg )
{
	u8 byFrameRate = *(pcMsg->content);
	for( u8 byLoop = 0; byLoop < m_byMaxVmpOutNum; byLoop ++)
	{
		m_tMpuVmpCfg.m_tVideoEncParam[byLoop].m_byFrameRate =  byFrameRate;
	}
	
	s32 nRet = m_cHardMulPic.StartMerge(&m_tMpuVmpCfg.m_tVideoEncParam[0], 
                                                m_byMaxVmpOutNum);

	if ( HARD_MULPIC_OK != nRet )
	{
		ErrorLog("Set Vmp FrameRate to %d failed!\n", byFrameRate);
	}
	else
	{
		KeyLog( "Set Vmp FrameRate to %d successfully!\n", byFrameRate);
	}
}

/*====================================================================
    函数名      : SetMcuCodingForm
    功能        : 存储mcu编码方式
    算法实现    :
    引用全局变量:
    输入参数说明: 
    返回值说明  :
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人      修改内容
    2013/04/08              liaokang      创建
====================================================================*/
void CMpu2VmpInst::SetMcuEncoding(emenCodingForm emMcuEncoding)
{
    m_emMcuEncoding = emMcuEncoding;
}

/*====================================================================
    函数名      : GetMcuCodingForm
    功能        : 获取MCU编码方式
    算法实现    :
    引用全局变量:
    输入参数说明: 
    返回值说明  : 
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人      修改内容
    2013/04/08              liaokang      创建
====================================================================*/
emenCodingForm CMpu2VmpInst::GetMcuEncoding()
{
    return m_emMcuEncoding;
}

/*==============================================================================
函数名    :  ShowVMPDebugInfo
功能      :  打印高级配置信息
算法实现  :  
参数说明  :  
返回值说明:  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2011/12/07  			周翼亮                  创建
=============================================================================*/
void CMpu2VmpInst::ShowVMPDebugInfo()
{
	m_tMpuVmpCfg.Print();
}


/*==============================================================================
函数名    :  IsEnableBitrateCheat
功能      :  是否配置码率作弊
算法实现  :  
参数说明  :  
返回值说明:  TRUE：配置了码率作弊，FALSE：没有配置
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2011/12/07  			周翼亮                  创建
=============================================================================*/
BOOL32 CMpu2VmpInst::IsEnableBitrateCheat()const
{
	return m_tMpuVmpCfg.IsEnableBitrateCheat();
}

/*==============================================================================
函数名    :  SetNoStreamBakByBmpPath
功能      :  设置某一通道无码流显示
算法实现  :  
参数说明  :  dwBakType：为媒控用的模式，传给媒控，pBmpPath：对应图片的路径，
			dwChnlId:设置的通道号
返回值说明:  s32 媒控设置空闲通道显示模式的返回值
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2011/12/1   4.7           zhouyiliang						create
==============================================================================*/
s32 CMpu2VmpInst::SetNoStreamBakByBmpPath(u32 dwBakType,const s8* pBmpPath,u32 dwChnlId /* = MULPIC_MAX_CHNS */)
{
	s32 nReturn = 0;
	FILE* pBmp = NULL;
	u8* pBuf = NULL;//媒控没限制长度，只限制了宽高，用new
	u32 dwBmpLen = 0;
	switch (dwBakType)
	{
	case EN_PLAY_BLACK:
	case EN_PLAY_LAST:
		break;
	case EN_PLAY_BMP:
	case EN_PLAY_BMP_USR:
		if ( pBmpPath != NULL && strlen(pBmpPath) != 0 )
		{
			pBmp = fopen(pBmpPath,"r");
			if (NULL != pBmp)
			{
				fseek(pBmp,0,SEEK_END);
				dwBmpLen = ftell(pBmp);
				fseek(pBmp,0,SEEK_SET);
				pBuf = new u8[dwBmpLen];
				if (NULL != pBuf)
				{
					fread(pBuf,1,dwBmpLen,pBmp);
				}
			}
		}
		break;
	default:
		ErrorLog("[SetNoStreamBakByBmpPath]Invalid baktype:%d",dwBakType);
		break;
	}



	KeyLog("[SetNoStreamBakByBmpPath]dwBakType:%d,pBmpPath:%s,dwChnId:%d\n",dwBakType,pBmpPath,dwChnlId);
	nReturn = m_cHardMulPic.SetNoStreamBak( dwBakType ,pBuf,dwBmpLen,dwChnlId);
	if (NULL != pBmp)
	{
		fclose(pBmp);
	}
	if (NULL != pBuf)
	{
		delete[] pBuf;
	}
	return nReturn;
}

/*==============================================================================
函数名    :  SetIdleChnShowMode
功能      :  设置空闲通道显示模式
算法实现  :  
参数说明  :  u8:byShowMode,mcu下参过来的显示模式，需要转换为媒控用的模式，传给媒控
返回值说明:  s32 媒控设置空闲通道显示模式的返回值
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2011/12/1   4.7           zhouyiliang						create
==============================================================================*/
s32 CMpu2VmpInst::SetIdleChnShowMode(u8 byShowMode,BOOL32 bStart)
{

	if( byShowMode > VMP_SHOW_USERDEFPIC_MODE )
	{
		byShowMode = VMP_SHOW_BLACK_MODE; // 默认显示黑屏
	}
	KeyLog( "[SetIdleChnShowMode] Set Vmp IdleChlShowMode: %d \n", byShowMode );
	
	s32 nReturn = 0;
	s8 szBmpFilePath[255] = {0};
	if (bStart)//Start的时候先设置统一模式，然后设置别名对应的通道
	{
		// 给底层设置空闲通道显示模式
		u32 dwHardShowMode = (u32)EN_PLAY_BLACK;
		switch( byShowMode )
		{
			case VMP_SHOW_BLACK_MODE://VMP空闲通道显示为黑色，默认值，不用再赋值
				break;
			case VMP_SHOW_LASTFRAME_MODE://VMP空闲通道显示最后一帧
				dwHardShowMode = (u32)EN_PLAY_LAST;
				break;
			case VMP_SHOW_DEFPIC_MODE://VMP空闲通道显示默认图片
				dwHardShowMode = (u32)EN_PLAY_BMP;
				sprintf(szBmpFilePath,"%s",DEFAULT_BMP__PATH);
				break;
			case VMP_SHOW_USERDEFPIC_MODE://VMP空闲通道显示用户自定义图片
				
				dwHardShowMode = (u32)EN_PLAY_BMP_USR;
				sprintf(szBmpFilePath,"%s",USR_BMP_PATH);
				break;
			default:		
			break;
		}
		nReturn = SetNoStreamBakByBmpPath(dwHardShowMode,szBmpFilePath);
		if ( (s32)Codec_Success != nReturn )
		{
			ErrorLog( "[SetIdleChnShowMode] start SetNoStreamBakByBmpPath mode:%d failed(ErrCode.%d)!\n",byShowMode, nReturn);
		}
	
		for (u8 nIdx = 0; nIdx < MAXNUM_MPU2VMP_MEMBER; nIdx++)
		{
			//20111108 zjl modify 支持vmp成员显示自定义图片
			for (u8 byIndex = 0; byIndex < MAXNUM_VMPDEFINE_PIC; byIndex++)
			{
				if(!m_tVmpCommonAttrb.m_atMtMember[nIdx].IsNull() &&
					strlen(m_tVmpMbAlias[nIdx].GetMbAlias()) > 0 &&
					0 == strcmp(m_tVmpMbAlias[nIdx].GetMbAlias(), 
					m_tMpuVmpCfg.GetVmpMemAlias(byIndex)))
				{
					//调用接口
					KeyLog( "[SetIdleChnShowMode]index.%d -> alias.%s -> pic.%s!\n", 
						nIdx, 
						m_tMpuVmpCfg.GetVmpMemAlias(byIndex), 
						m_tMpuVmpCfg.GetVmpMemRoute(byIndex));

					nReturn = SetNoStreamBakByBmpPath((u32)EN_PLAY_BMP_USR,m_tMpuVmpCfg.GetVmpMemRoute(byIndex),nIdx);
					if ( (s32)Codec_Success != nReturn )
					{
						ErrorLog( "[SetIdleChnShowMode] Index.%d SetNoStreamBak usrbmp failed(ErrCode.%d)!\n", nIdx, nReturn);
					}
					break;
				}
			}			
			//
		}

	}
	else //change的时候要先判断设置了图片的通道，然后再按现有模式设置有改变的通道
	{
		BOOL32 bSetBmp = FALSE;
		for (u8 byIdx = 0; byIdx < MAXNUM_MPU2VMP_MEMBER; byIdx++)
		{	
			bSetBmp = FALSE;
			if( !m_tVmpCommonAttrb.m_atMtMember[byIdx].IsNull())
			{
				for (u8 byIndex = 0; byIndex < MAXNUM_VMPDEFINE_PIC; byIndex++)
				{
					// 此处的byIdx与byIndex是不同的，修改时注意
					if ( strlen(m_tVmpMbAlias[byIdx].GetMbAlias()) >0 && 
						0 == strcmp(m_tVmpMbAlias[byIdx].GetMbAlias(), m_tMpuVmpCfg.GetVmpMemAlias(byIndex)))
					{
						//调用接口
						KeyLog( "[MsgChangeVidMixParamProc]index.%d -> alias.%s -> pic.%s!\n", 
							byIdx, 
							m_tMpuVmpCfg.GetVmpMemAlias(byIndex), 
							m_tMpuVmpCfg.GetVmpMemRoute(byIndex));
						
						nReturn = SetNoStreamBakByBmpPath((u32)EN_PLAY_BMP_USR,m_tMpuVmpCfg.GetVmpMemRoute(byIndex),byIdx);
						if ((s32)Codec_Success != nReturn)
						{
							ErrorLog( "[MsgChangeVidMixParamProc] SetNoStreamBak bmp .%d failed(ErrCode.%d)!\n", byIdx, nReturn);
						}
						else
						{
							bSetBmp = TRUE;
						}
						break;
					}
				}
			}
			if ( m_tVmpCommonAttrb.m_atMtMember[byIdx] == m_tPreviousVmpCommonAttrb.m_atMtMember[byIdx] || 
				m_tVmpCommonAttrb.m_atMtMember[byIdx].IsNull() && m_tPreviousVmpCommonAttrb.m_atMtMember[byIdx].IsNull() )
			{
				KeyLog("[MsgChangeVidMixParamProc]Idx :%d,previous and now member is same mt,no need to reset nostreambak,contiue.\n",byIdx);
				continue;
			}
			if (!bSetBmp )
			{
				u32 dwHardShowMode = (u32)EN_PLAY_BLACK;
				switch( byShowMode )
				{
					case VMP_SHOW_BLACK_MODE://VMP空闲通道显示为黑色，默认值，不用再赋值
						break;
					case VMP_SHOW_LASTFRAME_MODE://VMP空闲通道显示最后一帧
						dwHardShowMode = (u32)EN_PLAY_LAST;
						break;
					case VMP_SHOW_DEFPIC_MODE://VMP空闲通道显示默认图片
						dwHardShowMode = (u32)EN_PLAY_BMP;
						sprintf(szBmpFilePath,"%s",DEFAULT_BMP__PATH);
						break;
					case VMP_SHOW_USERDEFPIC_MODE://VMP空闲通道显示用户自定义图片
						
						dwHardShowMode = (u32)EN_PLAY_BMP_USR;
						sprintf(szBmpFilePath,"%s",USR_BMP_PATH);
						break;
					default:		
					break;
				}
			
				nReturn = SetNoStreamBakByBmpPath(dwHardShowMode,szBmpFilePath,byIdx);
			
				if ((s32)Codec_Success != nReturn)
				{
					ErrorLog( "[MsgChangeVidMixParamProc] Index.%d SetNoStreamBak .%d failed(ErrCode.%d)!\n", byIdx, byShowMode, nReturn);
				}
				else
				{
					KeyLog( "[MsgChangeVidMixParamProc] Index.%d SetNoStreamBak .%d success!\n", byIdx, byShowMode);
				}
			}	
		}
	}
	
	KeyLog( "[SetIdleChnShowMode][SetNoStreamBak] return %d \n", nReturn );

	return nReturn;


}


/*==============================================================================
函数名    :  SetMediaSndNetParam
功能      :  设置发送对象的网络参数
算法实现  :  
参数说明  :  u8：byChnNo 设置的对象index
返回值说明:  TRUE：设置成功，FALSE：设置失败
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2011/12/07  			周翼亮                  创建
=============================================================================*/
BOOL32 CMpu2VmpInst::SetMediaSndNetParam(u8 byChnNo)
{
	 TNetSndParam tNetSndPar;
    memset( &tNetSndPar, 0x00, sizeof(tNetSndPar) );
	
    tNetSndPar.m_byNum  = 1;           
#ifdef _8KI_
	tNetSndPar.m_tLocalNet.m_wRTPPort       = VMP_8KEOR8KH_LOCALSND_PORT +  byChnNo * PORTSPAN; 
#else
	tNetSndPar.m_tLocalNet.m_wRTPPort       = PORT_SNDBIND + (GetInsID() -1) * PORTSPAN*2 + byChnNo * 2; //2*9 =18 间隔20刚好
#endif
    
    tNetSndPar.m_tLocalNet.m_wRTCPPort      = m_wMcuRcvStartPort + byChnNo * PORTSPAN + 1;/*PORT_SNDBIND + byChnNo * 2 + 1*/
    tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr  = htonl(m_dwMcuRcvIp);
    tNetSndPar.m_tRemoteNet[0].m_wRTPPort   = m_wMcuRcvStartPort + byChnNo * PORTSPAN;
    tNetSndPar.m_tRemoteNet[0].m_dwRTCPAddr = htonl(m_dwMcuRcvIp);
    tNetSndPar.m_tRemoteNet[0].m_wRTCPPort  = m_wMcuRcvStartPort + byChnNo * PORTSPAN + 1;

    KeyLog( "[InitMediaSnd] Snd[%d]: RTPAddr(%s)\n", byChnNo, StrOfIP(m_dwMcuRcvIp));

    u16 wRet = m_pcMediaSnd[byChnNo]->SetNetSndParam( tNetSndPar );
    if ( MEDIANET_NO_ERROR != wRet )
	{
    	ErrorLog( "[SetMediaSndNetParam] m_cMediaSnd[%d].SetNetSndParam fail, Error code is: %d \n", byChnNo, wRet );
       	return FALSE;
	}
	return TRUE;
}


/*==============================================================================
函数名    :  GetEncCifChnNum
功能      :  获取当前编cif码流的通道
算法实现  :  
参数说明  :  
返回值说明:  u8：编cif码流的通道号，没有出cif的通道时返回：0xFF
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2011/12/07  			周翼亮                  创建
=============================================================================*/
u8 CMpu2VmpInst::GetEncCifChnNum()
{
	u8 byCifChn = 0xFF;
	//对于mpu2，cif不固定，mpu是固定在第3路(chnnum:2)出cif/other
	if (!IsSimulateMpu())
	{
		for (u8 byLoop  = 0 ; byLoop < MAXNUM_MPU2VMP_CHANNEL; byLoop++) 
		{
			if ( m_tParam[byLoop].GetEncType() == MEDIA_TYPE_H264 &&
				 m_tParam[byLoop].GetVideoWidth() == 352 && m_tParam[byLoop].GetVideoHeight() == 288 )
			{
				byCifChn = byLoop;
				break;
			}
		}
	}
	else
	{
		byCifChn = 2;
	}


	return byCifChn;
}


/*==============================================================================
函数名    :  GetEncOtherChnNum
功能      :  获取当前编other码流的通道
算法实现  :  
参数说明  :  
返回值说明:  u8：编cif码流的通道号，没有出cif的通道时返回：0xFF
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2011/12/07  			周翼亮                  创建
=============================================================================*/
u8 CMpu2VmpInst::GetEncOtherChnNum()
{
	u8 byOhterChn = 0xFF;
	//对于mpu2，other不固定，mpu是固定在第3路(chnnum:2)出cif/other
	if (!IsSimulateMpu())
	{
		for (u8 byLoop  = 0 ; byLoop < MAXNUM_MPU2VMP_CHANNEL; byLoop++) 
		{
			if ( m_tParam[byLoop].GetEncType() != MEDIA_TYPE_H264  )
			{
				byOhterChn = byLoop;
				break;
			}
		}
	}
	else
	{
		byOhterChn = 2;
	}
	return byOhterChn;
}

/*==============================================================================
函数名    :  TransFromMpuParam2EncParam
功能      :  将mpu的param转换为encparam
算法实现  :  
参数说明  :  
返回值说明:  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2011/12/07  			周翼亮                  创建
=============================================================================*/
void CMpu2VmpInst::TransFromMpuParam2EncParam(CKDVNewVMPParam* ptKdvVmpParam, TVideoEncParam* ptVideEncParam)
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
	//码率是原始的码率
	u16 wOrigBitrate = m_tMpuVmpCfg.GetOrigRate(  ptKdvVmpParam->GetBitRate() );
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
		ptVideEncParam->m_dwMaxKeyFrameInterval = 3000;
	}
	else if ( ptVideEncParam->m_wVideoWidth == 1280 &&
		ptVideEncParam->m_wVideoHeight == 720 )
	{
		ptVideEncParam->m_dwMaxKeyFrameInterval = 3000;
	}
	else if ( (ptVideEncParam->m_wVideoWidth == 720 ||
		ptVideEncParam->m_wVideoWidth == 704) &&
		ptVideEncParam->m_wVideoHeight == 576 )
	{
		ptVideEncParam->m_dwMaxKeyFrameInterval = 1000;
	}
	else if ( ptVideEncParam->m_wVideoWidth == 352 &&
		ptVideEncParam->m_wVideoHeight == 288 )
	{
		ptVideEncParam->m_dwMaxKeyFrameInterval = 500;
	}
	
	// xliang [5/26/2009] 量化参数调整
	if(ptVideEncParam->m_byEncType == MEDIA_TYPE_H264 )
	{
		ptVideEncParam->m_byMaxQuant = 45;
		ptVideEncParam->m_byMinQuant = 20;
	}
	else//h263 ,mp4
	{
		mpulog( MPU_INFO, "set h263/mp4 Quant and MaxKeyFrameInterval!\n");
		ptVideEncParam->m_byMaxQuant = 30;
		ptVideEncParam->m_byMinQuant = 3;
		ptVideEncParam->m_dwMaxKeyFrameInterval = 300;
	}
	
}

/*==============================================================================
函数名    :  TransFromMpuParam2EncParam
功能      :  将mpu2的param转换为encparam
算法实现  :  
参数说明  :  
返回值说明:  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2011/12/07  			周翼亮                  创建
=============================================================================*/
void CMpu2VmpInst::TransFromMpu2Param2EncParam(CKDVVMPOutMember* ptKdvVmpParam,TVideoEncParam* ptVideEncParam)
{
	if (NULL == ptVideEncParam || NULL == ptKdvVmpParam)
	{
		ErrorLog("[TransFromMpu2Param2EncParam] error param (null)!");
		return;
	}
	
	// 参数赋值和调整
	
	ptVideEncParam->m_byEncType    = ptKdvVmpParam->GetEncType();
	ptVideEncParam->m_wBitRate     = ptKdvVmpParam->GetBitRate();
	
	
	u8 byFrameRate =  ptKdvVmpParam->GetFrameRate();
	u8 byRes = GetResViaWH(ptKdvVmpParam->GetVideoWidth(), ptKdvVmpParam->GetVideoHeight());

	//查分辨率码率限制信息表，调整分辨率和帧率
	//码率是原始码率
	u16 wOrigBitrate = m_tMpuVmpCfg.GetOrigRate(  ptKdvVmpParam->GetBitRate() );
	KeyLog("[TransFromMpu2Param2EncParam]Before GetRealResFrameRatebyBitrate \n:width:%d,height:%d,framerate:%d,wOrigBitrate:%d,ProfileType:%d\n",
		ptKdvVmpParam->GetVideoWidth(),ptKdvVmpParam->GetVideoHeight(),ptKdvVmpParam->GetFrameRate(),wOrigBitrate,ptKdvVmpParam->GetProfileType());
	BOOL32 bGetRet = g_tResBrLimitTable.GetRealResFrameRatebyBitrate( ptKdvVmpParam->GetEncType(),byRes,
		byFrameRate,wOrigBitrate,ptKdvVmpParam->GetProfileType());
	if (!bGetRet)
	{
		OspPrintf(TRUE,FALSE,"[TransFromMpu2Param2EncParam]ERROR: GetRealResFrameratebyBitrate return FALSE\n");
		ErrorLog("[TransFromMpu2Param2EncParam]ERROR: GetRealResFrameratebyBitrate return FALSE\n");
	}
	u16 wVideoWith = 0;
	u16 wVideoHeight = 0;
	GetWHViaRes(byRes,wVideoWith,wVideoHeight);
	if( VIDEO_FORMAT_4CIF == byRes && MEDIA_TYPE_MP4 == ptKdvVmpParam->GetEncType() )
	{
		wVideoWith = 720;
		wVideoHeight = 576;
	}
	
	KeyLog("[TransFromMpu2Param2EncParam]After GetRealResFrameRatebyBitrate \n:width:%d,height:%d,framerate:%d,ProfileType:%d\n",
		wVideoWith,wVideoHeight,byFrameRate,ptKdvVmpParam->GetProfileType());
	ptVideEncParam->m_wVideoHeight = wVideoHeight; 
	ptVideEncParam->m_wVideoWidth  = wVideoWith;
	ptVideEncParam->m_dwProfile = ptKdvVmpParam->GetProfileType();
	//帧率调整
	ptVideEncParam->m_byFrameRate = 0;
	if( ptKdvVmpParam->GetFrameRate() != 0 )
	{
		KeyLog("Set vmp frame rate to %d\n", byFrameRate);
		ptVideEncParam->m_byFrameRate = byFrameRate;
	}
	
	
	
	
	if ( (ptVideEncParam->m_wVideoWidth == 1920 &&
		ptVideEncParam->m_wVideoHeight == 1088) ||
		(ptVideEncParam->m_wVideoWidth == 1920 &&
		ptVideEncParam->m_wVideoHeight == 1080) )
	{
		ptVideEncParam->m_dwMaxKeyFrameInterval = 3000;
	}
	else if ( ptVideEncParam->m_wVideoWidth == 1280 &&
		ptVideEncParam->m_wVideoHeight == 720 )
	{
		ptVideEncParam->m_dwMaxKeyFrameInterval = 3000;
	}
	else if ( (ptVideEncParam->m_wVideoWidth == 720 ||
		ptVideEncParam->m_wVideoWidth == 704) &&
		ptVideEncParam->m_wVideoHeight == 576 )
	{
		ptVideEncParam->m_dwMaxKeyFrameInterval = 1000;
	}
	else if ( ptVideEncParam->m_wVideoWidth == 352 &&
		ptVideEncParam->m_wVideoHeight == 288 )
	{
		ptVideEncParam->m_dwMaxKeyFrameInterval = 500;
	}
	
	// xliang [5/26/2009] 量化参数调整
	if(ptVideEncParam->m_byEncType == MEDIA_TYPE_H264 )
	{
		ptVideEncParam->m_byMaxQuant = 45;
		ptVideEncParam->m_byMinQuant = 20;
	}
	else//h263 ,mp4
	{
		mpulog( MPU_INFO, "set h263/mp4 Quant and MaxKeyFrameInterval!\n");
		ptVideEncParam->m_byMaxQuant = 30;
		ptVideEncParam->m_byMinQuant = 3;
		ptVideEncParam->m_dwMaxKeyFrameInterval = 300;
	}
	
}
/*==============================================================================
函数名    :  DisplayMbAlias
功能      :  显示终端别名
算法实现  :  
参数说明  :  
返回值说明:  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2012/06/06  			周翼亮                  创建
=============================================================================*/
void CMpu2VmpInst::DisplayMbAlias(u8 const byChnnl,const s8* pMemberAlias)
{
	//param check 
	if ( NULL == pMemberAlias || byChnnl >= MAXNUM_MPU2VMP_MEMBER)
	{
		ErrorLog("[DisplayMbAlias]Error:pMemberAlias is Null Pointer byChnl:%d.\n",byChnnl);
		return;
	}
	if ( FALSE == IsDisplayMbAlias() )
	{
		ErrorLog("[DisplayMbAlias]Current NotDisplayMbAlias\n");
		return;
	}

	s8 szAlias[MAXLEN_ALIAS]={0};
	memcpy( szAlias,pMemberAlias, min(strlen(pMemberAlias), MAXLEN_ALIAS-1) );
	
	// 若别名长度为0，则不显示别名
	if (0 == strlen(szAlias))
	{
		ErrorLog("[DisplayMbAlias] Chl:%d Alias length is 0.\n", byChnnl);
		m_cHardMulPic.StopAddIcon(byChnnl);
		return;
	}

	//获取该通道的大小
	u8 byStyle = VMP_STYLE_NONE;
    byStyle = m_tVmpCommonAttrb.m_byVmpStyle;
	if (VMP_STYLE_NONE == byStyle || byStyle >= VMPSTYLE_NUMBER)
	{
		ErrorLog("[DisplayMbAlias] error style:%d\n",byStyle);
		return;
	}
	u8 byChnRes = m_tMpuVmpCfg.m_aVMPStyleChnlRes[byStyle][byChnnl];
	u16 wChnlWidth = 0;
	u16 wChnlHeight = 0 ;
	GetWHViaRes(byChnRes,wChnlWidth,wChnlHeight);
	if ( 0 == wChnlWidth  || 0 == wChnlHeight )
	{
		ErrorLog("[DisplayMbAlias] GetWHViaRes error, byChnRes:%d\n",byChnRes);
		return;
	}
    KeyLog("[DisplayMbAlias] Style=%d,Chnl=%d,ChnRes=%d,ChnWidth=%d,ChnHeight=%d\n",byStyle,byChnnl,byChnRes,wChnlWidth,wChnlHeight);
	
    //获取drawbmp后别名的宽高等信息
	TDrawParam tDrawParam;
	tDrawParam.SetText(szAlias);

    // 统一字体大小为22号 [8/5/2013 liaokang]
    tDrawParam.SetFontSize(22);
    // 根据通道设置字体大小 [7/8/2013 liaokang] 
    // 通道宽度：1920、1280、960、640、480、352 
    // StartAddIcon中媒控限定了大小600*50, 距离底部10个像素预留,因此限定为600*4
    // 16个汉字，30号字体占500*40   24号字体占376*32，16号字体占256*24
//     if ( wChnlWidth >=1280 )
//     {
//         tDrawParam.SetFontSize(FONT_SIZE32);
//     }
//     else if ( wChnlWidth > 480 && wChnlWidth < 1280 )
//     {
//         tDrawParam.SetFontSize(FONT_SIZE24);
//     }
//     else
//     {
//         tDrawParam.SetFontSize(FONT_SIZE16);
//     }

    tDrawParam.SetTextEncode((u8)GetMcuEncoding());
	TPic tPic;
	BOOL32 bDrawBmp = m_cBmpDrawer.DrawBmp(tPic, tDrawParam);
	if (!bDrawBmp)
	{
		ErrorLog("[DisplayMbAlias] DrawBmp return error, byChnnl:%d\n",byChnnl);
		return;
	}
	KeyLog("[DisplayMbAlias] Chnl:%d,drawbmp succeed,width=%d,height=%d\n",byChnnl,tPic.GetPicWidth(),tPic.GetPicHeight());
	u32 dwXpos = 0;
	u32 dwYpos = 0;
	if ( wChnlWidth < tPic.GetPicWidth() || wChnlHeight < (tPic.GetPicHeight() + 10) )
	{
		ErrorLog("[DisplayMbAlias] byChnnl:%d drawbmp return pic height or width is bigger than channel height or width\n",byChnnl);
		return;
	}
	dwXpos = (wChnlWidth - tPic.GetPicWidth())/2;
	dwYpos = wChnlHeight - tPic.GetPicHeight() - 10;//距离底部10个像素预留
	TBackBGDColor tBackBGDColor;
	memset(&tBackBGDColor,0,sizeof(TBackBGDColor));

	u8 abyPicBuf[MAX_BMPFILE_SIZE] = {0};
	memcpy(abyPicBuf,tPic.GetPic(),tPic.GetPicSize() );
	s32 nRet = m_cHardMulPic.StartAddIcon(byChnnl, abyPicBuf , tPic.GetPicSize(), dwXpos, dwYpos,  
		tPic.GetPicWidth(),tPic.GetPicHeight(),  tBackBGDColor,  0);
	KeyLog("[DisplayMbAlias] Chnl:%d,displayAlias:%s,nRet:%d(CORRECT:%d)\n",byChnnl,szAlias,nRet,HARD_MULPIC_OK);
}
//END OF FILE

