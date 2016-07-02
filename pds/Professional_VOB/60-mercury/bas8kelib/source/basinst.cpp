/*****************************************************************************
   模块名      : 画面复合器(Video Multiplexer)实现
   文件名      : vmpInst.cpp
   创建时间    : 2003年 12月 4日
   实现功能    : include : /include/basinst.h
   作者        : zhangsh
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   03/12/4	   v1.0	      zhangsh	    create
   05/02/27    3.6        libo          加密修改
   2006/03/21  4.0        张宝卿        代码优化
   2009/03/17  1.0(8000E) 顾振华        遗留问题：改变编码码率      
******************************************************************************/
#include "basinst.h"
//#include "eqpcfg.h"
#include "mcuver.h"
//#include "commonlib.h"
//#include "eqpgvari.h"
//#include "mcuconst.h"

//extern CMulPic  * g_pcHardMulPic;                              //画面复合器的封装
/*lint -save -e438 -e526*/
extern TResBrLimitTable g_tResBrLimitTable;
s32 g_nBaslog = 0;
u16 g_wBasLastEvent = 0;
u32 g_dwBasLastFileLine = 0;

//extern CLogManager g_cBasLogMgr;

//extern s8* strofip(u32 dwIp, BOOL32 bBONet);
//extern LPCSTR GetMediaString( u8 byMediaType );
//extern void GetWHViaRes(u8 byRes, u16 &wWidth, u16 &wHeight);

#define REC_LASTCODELINE (g_dwBasLastFileLine = __LINE__)

/*lint -save -e438 -e530*/
/*lint -save -esym(526, __builtin_va_start)*/
/*lint -save -esym(628, __builtin_va_start)*/
// void baslog( s8* pszFmt, ...)
// {
//     s8 achPrintBuf[255];
//     s32  nBufLen = 0;
// 	va_list argptr;
// 	if( g_nBaslog )
// 	{		  
// 		nBufLen = sprintf( achPrintBuf, "[bas]: " ); 
// 		va_start( argptr, pszFmt );
// 		nBufLen += vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
// 		va_end(argptr); 
// 		OspPrintf( TRUE, FALSE, achPrintBuf ); 
// 	}
// }
/*lint -restore*/

const char* BasHardStyleStr(u32 dwHardStyle)
{
    const char* pszStr = "";
    switch ( dwHardStyle )
    {
    case MERGE_M1:  // 一画面
        pszStr = "Single"; 
        break;
        
    case MERGE_M2: // 两画面：左右分
        pszStr = "Lef+Right";  // 两画面：左右分
        break;
        
    case MERGE_M2_1_BR1:  // 两画面: 一大一小
        pszStr = "Big 1+Small 1"; // 两画面： 一大一小（大画面分成9份发送）
        break;
        
    case MERGE_M3_T1: // 三画面
        pszStr = "Triangle"; // 三画面
        break;
        
    case MERGE_M4: // 四画面
        pszStr = "2*2";  // 四画面
        break;
        
    case MERGE_M6_1_5: //六画面
        pszStr = "1+5"; //六画面
        break;
        
    case MERGE_M8_1_7: //八画面
        pszStr = "1+7"; //八画面
        break;

    case MERGE_M9: //九画面
        pszStr = "3*3"; //九画面
        break;
        
    case MERGE_M10_2_R8: //十画面
        pszStr = "2+8"; //十画面：二大八小
        break;

    case MERGE_M10_2_B8:    //十画面
        pszStr = "2Up+8Down"; // 十画面：上二下八
        break;
        
    case MERGE_M13_TL1_12:  //十三画面
        pszStr = "1+12"; //十三画面
        break;
        
    case MERGE_M16: //十六画面
        pszStr = "4*4"; //十六画面
        break;
        
    case MERGE_M4_1_R3://特殊四画面 
        pszStr = "1+3"; //特殊四画面：一大三小（大画面分成4份发送）
        break;
        
    case MERGE_M7_3_BR4: //七画面
        pszStr = "Big 3+Small 4"; //七画面：三大四小（大画面分成4份发送）
        break;

    case MERGE_M6_2_B4: //六画面(两大四小，小画面在下)
        pszStr = "2Up+4Down";
        break;

    case MERGE_M14_TL2_12: //十四画面合成(一大十三小)
        pszStr = "1+13";
        break;
        
    default:
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS,"[BasHardStyleStr] invalid style.%d\n", dwHardStyle);
        pszStr = "Unknown";    // 默认两画面：左右分
        break;
    }
    return pszStr;
}

CBASInst::CBASInst()
{
    //m_pcHardMulPic = NULL;
    //m_bDbVid = FALSE;
	m_byEncNum = 0;
    m_dwLastFUPTick = 0;
	m_byIframeReqCount = 0;
    memset(&m_tPrsTimeSpan, 0, sizeof(TPrsTimeSpan));
    for (u8 byChnlNo = 0; byChnlNo < MAXNUM_VMP_MEMBER; byChnlNo++)
    {
        m_abChnlValid[byChnlNo] = FALSE;
    }
}

/*lint -save -e1551*/
CBASInst::~CBASInst()
{
    if (NULL != m_pcHardMulPic)
    {
        m_pcHardMulPic->Quit();
        delete m_pcHardMulPic;
		m_pcHardMulPic = NULL;
    }
}
/*lint -restore*/
/*lint -save -e715*/
void CBASInst::DaemonInstanceEntry( CMessage* const pcMsg, CApp* pcApp )
{
	if (NULL == pcMsg)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS,"[CBasInst::Daemon]: The received msg's pointer in the msg entry is NULL!");
        return;
    }
	switch (pcMsg->event)
	{
    //上电
	case OSP_POWERON:
		DaemonProInit(pcMsg);
		break;
    //建链
    case EV_BAS_CONNECT_TIMER:
        DaemonProcConnectTimeOut();
        break;
    //注册
	case EV_BAS_REGISTER_TIMER:
        DaemonProcRegisterTimeOut();
        break; 
    //断链
	case OSP_DISCONNECT:
        DaemonProcDisconnect(pcMsg);
        break;
	default:
		 LogPrint( LOG_LVL_ERROR, MID_MCU_BAS,"[CBASInst]Unexcpet Message %u(%s).\n", pcMsg->event, ::OspEventDesc(pcMsg->event));
		break;
	}	
    return;
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
	03/12/4		v1.0		zhangsh			create
====================================================================*/
void CBASInst::InstanceEntry(CMessage *const pMsg)
{
    if (NULL == pMsg)
    {
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS,"[CBASInst::InstanceEntry] pMsg is null!\n");
        return;
    }

    if(EV_BAS_NEEDIFRAME_TIMER != pMsg->event )
    {
       LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[CBASInst]  begin Message %u(%s) received in InstanceEntry\n", pMsg->event, ::OspEventDesc(pMsg->event));
    }
    switch(pMsg->event)
    {
    //--------内部消息------------
    // 断链退出
	case EV_BAS_QUIT:
		ClearCurrentInst();
		break;
	// 注册
    case EV_BAS_REGISTER_TIMER:
        ProRegister();
        break;
	// 注册应答消息
    case MCU_BAS_REG_ACK:
        g_wBasLastEvent = pMsg->event;
        ProcRegAck(pMsg);
        break;
    
    // 注册拒绝消息
    case MCU_BAS_REG_NACK:
        g_wBasLastEvent = pMsg->event;
        ProcRegNack();
        break;
    case TIME_ERRPRINT:
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS,"BAS CMulPic::Initialize failed!\n");
        SetTimer(TIME_ERRPRINT, 15000);
        break;
    case TIME_BAS_FLASHLOG:
        ProcVmpFlashLogTimeOut();
        break;
	
    //--------外部消息------------
    // 启动初始化
    case EV_BAS_INI:
        ProBasInit();
        break;

    // 显示所有的状态和统计信息
    case EV_BAS_SHOW:
        StatusShow();
        break;

	case EV_VMP_SHOWPARAM:
		ParamShow();
		break;

        // 开始/参数改变
    case MCU_BAS_STARTADAPT_REQ:
    case MCU_BAS_SETADAPTPARAM_CMD:
        g_wBasLastEvent = pMsg->event;
        ProChangeAdapt(pMsg);
        break;

    // 停止复合
    case MCU_BAS_STOPADAPT_REQ:
        g_wBasLastEvent = pMsg->event;
        ProcStopAdapt(pMsg);
        break;

    // 设置Qos值
    case MCU_EQP_SETQOS_CMD:
        g_wBasLastEvent = pMsg->event;
        ProcSetQosInfo(pMsg);
        break;
    // 强制关键帧
    case MCU_BAS_FASTUPDATEPIC_CMD:
        g_wBasLastEvent = pMsg->event;
        MsgFastUpdatePicProc(pMsg);
        break;

    case MCU_VMP_CHNNLLOGOPR_CMD:
        MsgChnnlLogOprCmd( pMsg );
        break;

	// 检测是否需要关键帧
    case EV_BAS_NEEDIFRAME_TIMER:
        MsgTimerNeedIFrameProc();
        break;
    
    // 未定义的消息
    default:
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS,"[CBASInst]Unexcpet Message %u(%s).\n", pMsg->event, ::OspEventDesc(pMsg->event));
        break;
    }

	if(EV_BAS_NEEDIFRAME_TIMER != pMsg->event )
    {
		LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[CBASInst] end Message %u(%s) received in InstanceEntry\n", pMsg->event, ::OspEventDesc(pMsg->event));
    }
    REC_LASTCODELINE;
}

/*=============================================================================
函 数 名： ProcVmpFlashLogTimeOut
功    能： 
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/9/16   4.0			周广程                  创建
=============================================================================*/
void CBASInst::ProcVmpFlashLogTimeOut( void )
{
    KillTimer( TIME_BAS_FLASHLOG );
//    g_cBasLogMgr.WriteLogInfo();
//    g_cBasLogMgr.ClearLogTable();
    SetTimer( TIME_BAS_FLASHLOG, BAS_FLASHLOG_TIMEOUT );

    return;
}

/*=============================================================================
  函 数 名： ConvertVcStyle2HardStyle
  功    能： 由会控风格得到底层风格
  算法实现： 
  全局变量： 
  参    数： u8 oldstyle
  返 回 值： u32 
=============================================================================*/
u32 CBASInst::ConvertVcStyle2HardStyle(u8 byOldstyle)
{
    MergeStyle dwStyle = MERGE_M0;
    switch ( byOldstyle )
    {
    case VMP_STYLE_ONE:  // 一画面
        dwStyle = MERGE_M1;  // 单画面
        break;

    case VMP_STYLE_VTWO: // 两画面：左右分
        dwStyle = MERGE_M2;  // 两画面：左右分
        break;

    case VMP_STYLE_HTWO:  // 两画面: 一大一小
        dwStyle = MERGE_M2_1_BR1; // 两画面： 一大一小（大画面分成9份发送）
        break;

    case VMP_STYLE_THREE: // 三画面
        dwStyle = MERGE_M3_T1; // 三画面
        break;

    case VMP_STYLE_FOUR: // 四画面
        dwStyle = MERGE_M4;  // 四画面
        break;

    case VMP_STYLE_SIX: //六画面
        dwStyle = MERGE_M6_1_5; //六画面
        break;

    case VMP_STYLE_EIGHT: //八画面
        dwStyle = MERGE_M8_1_7; //八画面
        break;
    case VMP_STYLE_NINE: //九画面
        dwStyle = MERGE_M9; //九画面
        break;

    case VMP_STYLE_TEN: //十画面
        dwStyle = MERGE_M10_2_R8; //十画面：二大八小
        break;

     case VMP_STYLE_TEN_H:   //特殊十画面，上二下八
         dwStyle = MERGE_M10_2_B8;
         break;

    case VMP_STYLE_THIRTEEN:  //十三画面
        dwStyle = MERGE_M13_TL1_12; //十三画面
        break;

    case VMP_STYLE_SIXTEEN: //十六画面
        dwStyle = MERGE_M16; //十六画面
        break;
        
    case VMP_STYLE_SPECFOUR://特殊四画面 
        dwStyle = MERGE_M4_1_R3; //特殊四画面：一大三小（大画面分成4份发送）
        break;

    case VMP_STYLE_SEVEN: //七画面
        dwStyle = MERGE_M7_3_BR4; //七画面：三大四小（大画面分成4份发送）
        break;

    case VMP_STYLE_SIX_L2UP_S4DOWN: // 特殊六画面（上2下4）
        dwStyle = MERGE_M6_2_B4;
        break;

    case VMP_STYLE_FOURTEEN:    //十四画面合成（2大12小, 2大在上左）
        dwStyle = MERGE_M14_TL2_12;
        break;

    case VMP_STYLE_SIX_DIVIDE:  //六画面（三三等分）
        dwStyle = MERGE_M6;
        break;

    case VMP_STYLE_TEN_M:       //十画面（两个大画面居中）
        dwStyle = MERGE_M10_2_TB8;
        break;

    case VMP_STYLE_THIRTEEN_M:  //十三画面（大画面居中）
        dwStyle = MERGE_M13_1_ROUND12;
        break;

    case VMP_STYLE_FIFTEEN:     //十五画面
        dwStyle = MERGE_M15_T3_12;
        break;

    default:
        LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, "[ConvertVcStyle2HardStyle] invalid style.%d\n", byOldstyle);
        dwStyle = MERGE_M16;    // 默认两画面：左右分
        break;
    }
    return u32(dwStyle);
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
	2010/2/8    8ke 1.0     周嘉麟          修改
====================================================================*/
void CBASInst::DaemonProInit(CMessage *const pMsg)
{
    //从消息中得到信息
    if (NULL == pMsg->content)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS,"[CBasInst]Init params cannot be NULL!\n");
        return;
    }
    
	// Mcu信息
	
	T8keBasCfg tBasCfg;
    memset(&tBasCfg, 0, sizeof(tBasCfg));	
    memcpy(&tBasCfg, pMsg->content, sizeof(tBasCfg));

    g_cBasApp.m_tEqpCfg.SetConnectIp( tBasCfg.GetConnectIp() );
    g_cBasApp.m_tEqpCfg.SetConnectPort( tBasCfg.GetConnectPort() );	
	g_cBasApp.m_tEqpCfg.SetMcuId( tBasCfg.GetMcuId() );
    g_cBasApp.m_tEqpCfg.SetEqpType( tBasCfg.GetEqpType() );
    g_cBasApp.m_tEqpCfg.SetEqpId( tBasCfg.GetEqpId() );
    g_cBasApp.m_tEqpCfg.SetAlias( tBasCfg.GetAlias() );
	g_cBasApp.m_dwIframeInterval = tBasCfg.GetIframeInterval();

    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[BASINST]Init %s Success!\n", g_cBasApp.m_tEqpCfg.GetAlias() );
	printf("[BASINST]Init %s Success!\n", g_cBasApp.m_tEqpCfg.GetAlias() );
   
    if( g_cBasApp.m_tEqpCfg.IsEmbed())
    {   
        g_cBasApp.m_dwMcuNode = 0;
        SetTimer(EV_BAS_REGISTER_TIMER, BAS_REGISTER_TIMEOUT); 
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[BASINST]set EV_BAS_REGISTER_TIMER!\n" );
		printf("[BASINST]set EV_BAS_REGISTER_TIMER!\n");
    }
    else if( g_cBasApp.m_tEqpCfg.GetConnectIp()) // 连接Mcu
    {
        SetTimer(EV_BAS_CONNECT_TIMER, BAS_CONNETC_TIMEOUT);
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[BASINST]set EV_BAS_CONNECT_TIMER!\n" );
		printf("[BASINST]set EV_BAS_CONNECT_TIMER!\n");
    }
	
    for (u8 byIns = 1; byIns <= BAS_REG_NUM; byIns++)
    {
		post(MAKEIID(GetAppID(), byIns), EV_BAS_INI, NULL, 0);
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[BASINST]post EV_BAS_INI:%d!\n",byIns);
		printf("[BASINST]post EV_BAS_INI:%d!\n",byIns);
    }
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
void CBASInst::DaemonProcConnectTimeOut()
{
    KillTimer( EV_BAS_CONNECT_TIMER );
    switch( CurState() )
    {
    case IDLE:
        {
            BOOL32 bRet = FALSE;
            bRet = ConnectMcu(g_cBasApp.m_dwMcuNode);
            if( bRet )
            { 
                SetTimer(EV_BAS_REGISTER_TIMER, BAS_REGISTER_TIMEOUT); 
            }
            else
            {
                SetTimer(EV_BAS_CONNECT_TIMER, BAS_CONNETC_TIMEOUT);
            }
        }
        break;
    default:
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[CBASInst::ProcConnectTimeOut]: EV_BAS_CONNECT_TIMER received in wrong state %u!\n", CurState());
        break;
    }
 
    return;
}

/*=============================================================================
  函 数 名： ConnectMcu
  功    能： 
  算法实现： 
  全局变量： 
  参    数： 
             u32& dwMcuNode
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CBASInst::ConnectMcu( u32& dwMcuNode )
{
    BOOL32 bRet = TRUE;

    if( !OspIsValidTcpNode(dwMcuNode))
    {
        dwMcuNode = OspConnectTcpNode(htonl(g_cBasApp.m_tEqpCfg.GetConnectIp()), g_cBasApp.m_tEqpCfg.GetConnectPort(), 10, 0, 100 );  

        if (OspIsValidTcpNode(dwMcuNode))
        {
            ::OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), CBASInst::DAEMON);
            LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[BASINST Info] Connect to Mcu Success,node is %u!\n", dwMcuNode);
			printf("[BASINST Info] Connect to Mcu Success,node is %u!\n", dwMcuNode);
            // 记录本地IP
            g_cBasApp.m_tEqpCfg.SetEqpIp( ntohl( OspNodeLocalIpGet(dwMcuNode) ) );
        }
        else
        {
            //建链失败
            LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[bas] Failed to Connect Mcu %s:%d>\n", 
                                    strofip(g_cBasApp.m_tEqpCfg.GetConnectIp(), FALSE ), g_cBasApp.m_tEqpCfg.GetConnectPort());
			printf("[bas] Failed to Connect Mcu %s:%d>\n", 
                                    strofip(g_cBasApp.m_tEqpCfg.GetConnectIp(), FALSE ), g_cBasApp.m_tEqpCfg.GetConnectPort());
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
  参    数： BOOL32 bIsRegisterA
  返 回 值： void 
=============================================================================*/
void CBASInst::DaemonProcRegisterTimeOut()
{
    KillTimer( EV_BAS_REGISTER_TIMER );
    switch( CurState() )
    {
    case IDLE:
		{
			//Register( g_cBasApp.m_dwMcuNode );
			for (u8 byRegNum = 1; byRegNum <= BAS_REG_NUM; byRegNum++)
			{
	    		post(MAKEIID(GetAppID(), byRegNum), EV_BAS_REGISTER_TIMER, NULL, 0);
				LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[DaemonProcRegisterTimeOut] BAS RegNum :%d \n", byRegNum);
			}
			//这里是否要设置定时器
			//SetTimer(EV_BAS_REGISTER_TIMER, BAS_REGISTER_TIMEOUT);
			break;
		}
    default:
        LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, "[CBASInst::ProcRegisterTimeOut]: EV_BAS_REGISTER_TIMER received in wrong state %u!\n", CurState());
        break;
    }

    return;
}

/*====================================================================
	函数名  ：Register
	功能    ：向MCU注册函数
	输入    ：无
	输出    ：无
	返回值  ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期         版本        修改人        修改内容
   03/01/2010	   v1.0	       	    create
====================================================================*/
void CBASInst::ProRegister()
{
	switch( CurState() )
	{
	case IDLE:
		{
			LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[ProRegister] App:%d, Ins:%d \n", GetAppID(), GetInsID());
			CServMsg cSvrMsg;
			TEqpRegReq tReg;
			memset(&tReg, 0, sizeof(tReg));
			
			
			tReg.SetEqpIpAddr( g_cBasApp.m_tEqpCfg.GetEqpIp() );
			tReg.SetEqpType(g_cBasApp.m_tEqpCfg.GetEqpType());
			tReg.SetEqpAlias(g_cBasApp.m_tEqpCfg.GetAlias());
			tReg.SetVersion(DEVVER_BAS);
			tReg.SetMcuId( (u8)g_cBasApp.m_tEqpCfg.GetMcuId() );
			
			cSvrMsg.SetMsgBody((u8*)&tReg, sizeof(tReg));
			
			post( MAKEIID( AID_MCU_PERIEQPSSN, CInstance::DAEMON ), BAS_MCU_REG_REQ,
             cSvrMsg.GetServMsg(), cSvrMsg.GetServMsgLen(), g_cBasApp.m_dwMcuNode);

			SetTimer(EV_BAS_REGISTER_TIMER, BAS_REGISTER_TIMEOUT); 

			break;
		}
	default:
		LogPrint( LOG_LVL_ERROR, MID_MCU_BAS,"[CBASInst::ProRegister]: EV_BAS_REGISTER_TIMER received in wrong state %u!\n", CurState());
        break;
	}

    return;
}

/*====================================================================
	函数名  ：ProBasInit
	功能    ：初始化
	输入    ：消息
	输出    ：无
	返回值  ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2010/2/8    8ke 1.0       周嘉麟         创建     
====================================================================*/
void CBASInst::ProBasInit()
{
	TMulPicParam tInitParam;
    tInitParam.dwTotalMapNum = 1;
    tInitParam.dwType = u32(MERGE_M1);
    
    m_pcHardMulPic = new CMulPic;
    if ( NULL == m_pcHardMulPic)
    {
		LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, "m_pcHardMulPic is null!\n");
        m_bInitOk = FALSE;
        SetTimer(TIME_ERRPRINT, 1000);  //timely print err
        return;
    }
	
	if ( NULL == m_pcHardMulPic)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_BAS,  "m_pcHardMulPic is null!\n");
	}

	LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_BAS, "before new Initialize!TMulPicParam<dwTotalMapNum:%d, dwType:%d>\n", tInitParam.dwTotalMapNum, tInitParam.dwType);
    BOOL32 bInitOk = m_pcHardMulPic->Initialize( tInitParam  );
	LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_BAS, "after new Initialize!\n");

    if (!bInitOk)
    {
		LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, "[ProBasInit] Ins:%d Initialize failed!\n", GetInsID());
        m_bInitOk = FALSE;
        SetTimer(TIME_ERRPRINT, 1000);  //timely print err
        return;
    }

	//[nizhijun 2013/01/06]由于现在媒控那边默认设置白色边框，故此处针对BAS我们需要再设置下无边框
	TMulPicColor tBGDColor;
	memset(&tBGDColor,0,sizeof(TMulPicColor));
	TDrawFrameColor tSlidelineColor;
	memset( &tSlidelineColor,0,sizeof(TDrawFrameColor) );
	tSlidelineColor.bShow = FALSE;
	bInitOk = m_pcHardMulPic->SetBGDAndSidelineColor(tBGDColor, tBGDColor, &tSlidelineColor);
	if (!bInitOk)
    {
		LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, "[ProBasInit] Ins:%d SetBGDAndSidelineColor failed!\n", GetInsID());
        m_bInitOk = FALSE;
        SetTimer(TIME_ERRPRINT, 1000);  //timely print err
        return;
    }

	//初始化时开启接收对象接收
	bInitOk = m_pcHardMulPic->SetChannelActive(0, TRUE);

	if(!bInitOk)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, "[ProBasInit] Ins:%d SetChannelActive failed!\n", GetInsID());
	}

#ifdef _8KH_
	if (1 == GetInsID())
	{
		m_byChnType = BAS_8KHCHN_MV;
		m_tEqpCfg.SetAlias("BrdBasChn");
	}
	else if(2 == GetInsID())
	{
		m_byChnType = BAS_8KHCHN_DS;
		m_tEqpCfg.SetAlias("DSBasChn");
	}
	else if(3 == GetInsID())
	{
		m_byChnType = BAS_8KHCHN_SEL;
		m_tEqpCfg.SetAlias("SelBasChn");
	}
#elif defined(_8KI_)
	if (1 == GetInsID())
	{
		m_byChnType = BAS_8KICHN0;
		m_tEqpCfg.SetAlias("BrdBasChn");
	}
	else if(2 == GetInsID())
	{
		m_byChnType = BAS_8KICHN1;
		m_tEqpCfg.SetAlias("DSBasChn");
	}
	else if(3 == GetInsID())
	{
		m_byChnType = BAS_8KICHN2;
		m_tEqpCfg.SetAlias("SelBasChn");
	}
#else
	if (1 == GetInsID())
	{
		m_byChnType = BAS_8KECHN_MV;
		m_tEqpCfg.SetAlias("BrdBasChn");
	}
	else if(2 == GetInsID())
	{
		m_byChnType = BAS_8KECHN_DS;
		m_tEqpCfg.SetAlias("DSBasChn");
	}
	else if(3 == GetInsID())
	{
		m_byChnType = BAS_8KECHN_SEL;
		m_tEqpCfg.SetAlias("SelBasChn");
	}
#endif

    // 用全局变量记录, zgc, 2009-11-03
    //g_pcHardMulPic = m_pcHardMulPic;
	
    m_bInitOk = TRUE;	
    //m_bDbVid = TRUE;// 编4路码流
}

/*====================================================================
	函数名  ：MsgRegAckProc
	功能    ：MCU注册应答消息处理函数
	输入    ：消息
	输出    ：无
	返回值  ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期       版本        修改人        修改内容
   03/12/4	     v1.0	         zhangsh	    create
   2010/03/01    8ke 1.0       周嘉麟         修改     
====================================================================*/
void CBASInst::ProcRegAck(CMessage* const pMsg)
{    

    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "[ProcRegAck] Register OK!\n");
	printf("[ProcRegAck] Register OK!\n");

    if (NULL == pMsg->content)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcRegAck] The pointer cannot be Null!\n");
		printf("[ProcRegAck] The pointer cannot be Null!\n");
        return;
    }
    
    if ( IDLE != CurState() )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcRegAck] Current state is not IDLE! Error!\n" );
		printf("[ProcRegAck] Current state is not IDLE! Error!\n" );
        return;
    }

    NEXTSTATE(NORMAL);

    CServMsg cServMsg(pMsg->content, pMsg->length);
    TEqpRegAck tRegAck = *(TEqpRegAck*)cServMsg.GetMsgBody();

    // 取参数
	// 考虑是否移植到instance
	//???
    g_cBasApp.m_dwMcuNode = pMsg->srcnode;
    m_dwMcuIId = pMsg->srcid; 

    KillTimer(EV_BAS_REGISTER_TIMER);
    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "register to mcu.A succeed !\n");
	printf("register to mcu.A succeed !\n");
    
	//每个实例记录各自独立的外设信息
	m_tEqpCfg.SetMcuId(g_cBasApp.m_tEqpCfg.GetMcuId());
	m_tEqpCfg.SetEqpType(g_cBasApp.m_tEqpCfg.GetEqpType());
	m_tEqpCfg.SetEqpId( tRegAck.GetEqpId() );
    m_tEqpCfg.SetMcuStartPort( tRegAck.GetMcuStartPort() );
    m_tEqpCfg.SetEqpStartPort( tRegAck.GetEqpStartPort() );

    if ( 0 == pMsg->srcnode )
    {
        //g_cBasApp.m_tEqpCfg.SetEqpIp( ntohl( INET_ADDR("127.0.0.1") ) );
		m_tEqpCfg.SetEqpIp( ntohl( INET_ADDR("127.0.0.1") ) );
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "[ProcRegAck] bas<Ins:%d> pMsg->srcnode = 0!\n", GetInsID());
    }
    else
    {
        //g_cBasApp.m_tEqpCfg.SetEqpIp( ntohl( OspNodeLocalIpGet(pMsg->srcnode) ) );
		m_tEqpCfg.SetEqpIp( ntohl( OspNodeLocalIpGet(pMsg->srcnode) ) );
    }

    // mcu侧端口
	u8 byIdx = 0;
	for ( ; byIdx < MAXNUM_BAS_ENCNUM; byIdx++)
	{
		m_tSndAddr[byIdx].dwIp  = htonl(tRegAck.GetMcuIpAddr());  
		m_tSndAddr[byIdx].wPort = tRegAck.GetMcuStartPort() + byIdx*PORTSPAN;
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[ProcRegAck]  Channel<Id:%d> Remote Send IP:%s:%u\n", 
						byIdx, strofip(m_tSndAddr[byIdx].dwIp, TRUE), m_tSndAddr[byIdx].wPort);
	}
	

	u8 byLocalOutNum = 0;
	if (1 == GetInsID())
	{
		byLocalOutNum = MAXNUM_BAS_ENCNUM;
	} 
	else if (2 == GetInsID())
	{
		byLocalOutNum = MAXNUM_BAS_ENCNUM -1 ;
	} 
	else if (3 == GetInsID())
	{
	    byLocalOutNum = 1 ;
	}
	byIdx = 0;	
	for ( ; byIdx < byLocalOutNum; byIdx++)
	{
		m_tSndLocalAddr[byIdx].dwIp = 0;
#if defined(_8KH_) || defined(_8KI_)
		m_tSndLocalAddr[byIdx].wPort = BAS_8KE_LOCALSND_PORT + (GetInsID() - 1)*BAS_8KH_PORTSPAN + byIdx*PORTSPAN /*tRegAck.GetEqpStartPort() + (byIdx+1)*PORTSPAN*/;
#else		
		m_tSndLocalAddr[byIdx].wPort = BAS_8KE_LOCALSND_PORT + (GetInsID() - 1)*BAS_8KE_PORTSPAN + byIdx*PORTSPAN /*tRegAck.GetEqpStartPort() + (byIdx+1)*PORTSPAN*/;
#endif
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "[ProcRegAck]  Channel<Id:%d> Local Send IP:%s:%u\n", 
						byIdx, strofip(m_tSndLocalAddr[byIdx].dwIp, TRUE), m_tSndLocalAddr[byIdx].wPort);
	}

	m_tRcvLocalAddr.dwIp   = 0;
	m_tRcvLocalAddr.wPort  = tRegAck.GetEqpStartPort();
	m_tRtcpAddr.dwIp  = htonl(tRegAck.GetMcuIpAddr());
	m_tRtcpAddr.wPort = tRegAck.GetEqpStartPort() + 1;
	m_tLocalSndRtcpAddr.dwIp = 0;
	m_tLocalSndRtcpAddr.wPort =  tRegAck.GetEqpStartPort() + 3;
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "[ProcRegAck] Rcv LocalAddr:<Ip:%s:%u>", m_tRcvLocalAddr.dwIp, m_tRcvLocalAddr.wPort);

	BOOL32 bRetOk = FALSE;
	if (m_pcHardMulPic != NULL)
	{
		bRetOk = m_pcHardMulPic->SetChannelParam(0, m_tRcvLocalAddr, m_tRtcpAddr, m_tLocalSndRtcpAddr);
	}
	if (!bRetOk)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcRegAck] SetChannelParam failed!\n");
	} 
	
	
	m_tPrsTimeSpan = *(TPrsTimeSpan*)(cServMsg.GetMsgBody() + sizeof(TEqpRegAck));
	m_tPrsTimeSpan.m_wFirstTimeSpan  = ntohs(m_tPrsTimeSpan.m_wFirstTimeSpan);
	m_tPrsTimeSpan.m_wSecondTimeSpan = ntohs(m_tPrsTimeSpan.m_wSecondTimeSpan);
	m_tPrsTimeSpan.m_wThirdTimeSpan  = ntohs(m_tPrsTimeSpan.m_wThirdTimeSpan);
	m_tPrsTimeSpan.m_wRejectTimeSpan = ntohs(m_tPrsTimeSpan.m_wRejectTimeSpan);
	m_wMTUSize = *(u16*)( cServMsg.GetMsgBody() + sizeof(TEqpRegAck) + sizeof(TPrsTimeSpan) );
	m_wMTUSize = ntohs( m_wMTUSize );
	//SetVideoSendPacketLen( (s32)m_wMTUSize );
	
    // xsl [8/15/2006] 限制一下最大值，防止底层内存申请失败
	if(m_tPrsTimeSpan.m_wRejectTimeSpan > DEF_LOSETIMESPAN_PRS)
	{
		m_tPrsTimeSpan.m_wRejectTimeSpan = DEF_LOSETIMESPAN_PRS;
	}

//     if (m_tPrsTimeSpan.GetRejectTimeSpan() > DEF_LOSETIMESPAN_PRS)
//     {
//         m_tPrsTimeSpan.SetRejectTimeSpan(DEF_LOSETIMESPAN_PRS);
//     }

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "[ProcRegAck]set EV_BAS_NEEDIFRAME_TIMER : %d\n", g_cBasApp.m_dwIframeInterval);
	SetTimer(EV_BAS_NEEDIFRAME_TIMER, g_cBasApp.m_dwIframeInterval);

    SendStatusChangeMsg(TRUE , u8(TBasBaseChnStatus::READY));
    return ;
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
   03/12/4	   v1.0	      zhangsh	    create
====================================================================*/
void CBASInst::ProcRegNack()
{
    LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, "[Info] bas Register to MCU be refused .retry...\n");
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
BOOL CBASInst::SendMsgToMcu(u16 wEvent, CServMsg* const pcServMsg)
{
    LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "[Send] Message %u(%s).\n", wEvent, ::OspEventDesc(wEvent));
//    g_cBasLogMgr.InsertLogItem( "[Send] Message %u(%s).\n", wEvent, ::OspEventDesc(wEvent) );

    if(g_cBasApp.m_bEmbed || OspIsValidTcpNode(g_cBasApp.m_dwMcuNode))
    {
        post(m_dwMcuIId, wEvent, pcServMsg->GetServMsg(), pcServMsg->GetServMsgLen(), g_cBasApp.m_dwMcuNode);
    }

    return TRUE;
}

/*====================================================================
	函数名  ：Disconnec
	功能    ：断链函数
	输入    ：无
	输出    ：无
	返回值  ：无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   03/12/4	   v1.0	      zhangsh	      create
   09/03/06    8000E v1.0	顾振华	      create
====================================================================*/
void CBASInst::DaemonProcDisconnect(CMessage *const pMsg )
{
    u32 dwDisNode = *(u32 * )pMsg->content;
    
    if (INVALID_NODE != dwDisNode)
    {
        OspDisconnectTcpNode(dwDisNode);
    }
	else
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[DaemonProcDisconnect] unexpect disnode: %d\n", dwDisNode);
	}

    if( dwDisNode == g_cBasApp.m_dwMcuNode )
    {
        g_cBasApp.FreeStatusData();
        if( 0 != g_cBasApp.m_tEqpCfg.GetConnectIp() ) // 连接Mcu
        {
            SetTimer(EV_BAS_CONNECT_TIMER, BAS_CONNETC_TIMEOUT);
        }
        else
        {
            SetTimer(EV_BAS_REGISTER_TIMER, BAS_REGISTER_TIMEOUT); 
        }
        
        LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[DaemonProcDisconnect] Disconnect with Mcu: %d\n", g_cBasApp.m_dwMcuNode);
    }
	else
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS,"[DaemonProcDisconnect]  disnode:%d is not equal with m_dwMcuNode:%d\n", dwDisNode, g_cBasApp.m_dwMcuNode);
	}
	
	for (u8 byIns = 1; byIns <= BAS_REG_NUM; byIns++)
	{
		post( MAKEIID(GetAppID(), byIns), EV_BAS_QUIT);
	}
    //ClearCurrentInst();

    return ;
}

/*=============================================================================
  函 数 名： ClearCurrentInst
  功    能： 清空当前实例
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CBASInst::ClearCurrentInst(void)
{
	switch(CurState())//移通道
    {
    //case NORMAL:
    case RUNNING:
        {
            s32 nRet = m_pcHardMulPic->StopMerge();
            KillTimer(EV_BAS_NEEDIFRAME_TIMER);
            LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[ClearCurrentInst]:Stop work, the return code: %d!\n", nRet);
        }
        break;
	default:
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[ClearCurrentInst]:Stop work, CurState(): %d!\n", CurState());	
		}
    }

    NEXTSTATE(IDLE);
    g_cBasApp.m_byRegAckNum = 0;
	m_dwMcuIId = INVALID_INS;

	memset( m_abChnlValid, 0, sizeof(m_abChnlValid) );

	KillTimer( EV_BAS_REGISTER_TIMER );
	return;
}


/*====================================================================
	函数  : MsgChangeVidMixParamProc
	功能  : 改变画面复合参数消息响应
	输入  : 无
	输出  : 无
	返回  : 无
	注    : 编码路数暂时不能改变
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    09/03/06    8000E v1.0	顾振华	      create
====================================================================*/
void CBASInst::ProChangeAdapt(CMessage* const pMsg)
{
    if (NULL == pMsg->content)
    {
        LogPrint( LOG_LVL_ERROR, MID_MCU_BAS,"[ProChangeAdapt] The pointer cannot be Null\n");
        return;
    }
    
    switch(CurState())
    {
    case IDLE:        
        LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, "[ProChangeAdapt] Cannot merge while in state:%d\n", CurState());
        return ;
    default:
        break;
    }
    
    CServMsg cServMsg(pMsg->content, pMsg->length);
    cServMsg.SetEventId(pMsg->event);    
    g_dwBasLastFileLine = __LINE__;
    ChangeCP(cServMsg);
    g_dwBasLastFileLine = __LINE__;
    
    return;
}

void CBASInst::ConvertRealParam(u8 byOutIdx,THDAdaptParam &tBasParam)
{
	THDAdaptParam tTempParam = tBasParam;
    u16 	   wWidth = tBasParam.GetWidth(); 
    u16 	   wHeight= tBasParam.GetHeight();
    u8		   byFrameRate= tBasParam.GetFrameRate();
	if ( g_tResBrLimitTable.GetRealResFrameRatebyBitrate(tBasParam.GetVidType(),wWidth,wHeight,byFrameRate,tBasParam.GetBitrate(),tBasParam.GetProfileType()) )
	{
		tBasParam.SetResolution(wWidth,wHeight);
		tBasParam.SetFrameRate(byFrameRate);
		LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[ConvertRealParam] chnid:%d,outIdx:%d convert width:%d-height:%d framerate:%d to width:%d-height:%d framerate:%d\n",
				 	GetInsID()-1, byOutIdx, tTempParam.GetWidth(),tTempParam.GetHeight(),tTempParam.GetFrameRate(),wWidth,wHeight,byFrameRate
			     );
	}
}

/*====================================================================
	函数  : ChangeCP
	功能  : 改变画面复合参数
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    03/12/4	   v1.0	      zhangsh	      create
	2010/2/8   8ke 1.0    周嘉麟		  修改
====================================================================*/
void CBASInst::ChangeCP(CServMsg& cServMsg)
{
    // 由于需要先修改状态，所以这里把当前状态记录下来
    u8 byCurState = CurState();
    if ( NORMAL == CurState() )
    {	
        NEXTSTATE(RUNNING);
    }
	LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[ChangeCP] Ins<%d>'s Curstate is :%d!\n ", GetInsID(), CurState());

    u8 byChnIdx = 0;
	
	if(cServMsg.GetMsgBodyLen() <  MAXNUM_BAS_ENCNUM * sizeof(THDAdaptParam)+ sizeof(TMediaEncrypt) + sizeof(TDoublePayload))
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_BAS,"msgbody len(%d) error ,return!\n", cServMsg.GetMsgBodyLen());
		return;
	}
    //取适配参数
	TMediaEncrypt  tMediaEncryptVideo;
	TDoublePayload tDoublePayloadVideo;
	u8 byNeedPrs = 0;
    THDAdaptParam atHdAdaptParam[MAXNUM_BAS_ENCNUM];
	C8KEVMPParam   atParam[MAXNUM_BAS_ENCNUM];
	memset(atHdAdaptParam, 0, MAXNUM_BAS_ENCNUM * sizeof(THDAdaptParam));

	tMediaEncryptVideo = *(TMediaEncrypt*)( cServMsg.GetMsgBody()  + MAXNUM_BAS_ENCNUM * sizeof(THDAdaptParam) );
 	s32 nLen = MAXLEN_KEY;
	u8  abyKey[MAXLEN_KEY];
	memset( abyKey, 0, MAXLEN_KEY );
 	tMediaEncryptVideo.GetEncryptKey(abyKey, &nLen);
	LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[ChangeCP] Encrypt<Type:%d, Key:%s>\n", tMediaEncryptVideo.GetEncryptMode(), abyKey);

	for (u8 byOut = 0; byOut <MAXNUM_BAS_ENCNUM; byOut++)
	{
		atHdAdaptParam[byOut] = *(THDAdaptParam*)(cServMsg.GetMsgBody() + byOut*sizeof(THDAdaptParam));
		g_cBasApp.ConvertToRealFR(atHdAdaptParam[byOut]);
		ConvertRealParam(byOut,atHdAdaptParam[byOut]);

		//[nizhijun 2010/9/11] 目前mcu采用标准4CIF(704* 576), 而对于MPEG4,采取D1(720 * 576)
		if (MEDIA_TYPE_MP4 == atHdAdaptParam[byOut].GetVidType()&&
			704 == atHdAdaptParam[byOut].GetWidth() &&
			576 == atHdAdaptParam[byOut].GetHeight())
		{
			atHdAdaptParam[byOut].SetResolution(720, 576);
		}

		atParam[byOut].m_wVideoWidth  = atHdAdaptParam[byOut].GetWidth();
		atParam[byOut].m_wVideoHeight = atHdAdaptParam[byOut].GetHeight();
		atParam[byOut].m_wBitRate     = atHdAdaptParam[byOut].GetBitrate();
		atParam[byOut].m_byEncType    = atHdAdaptParam[byOut].GetVidType();
		atParam[byOut].m_byFrameRate  = atHdAdaptParam[byOut].GetFrameRate();
		atParam[byOut].m_tVideoEncrypt[0] = tMediaEncryptVideo;
		atParam[byOut].m_byVMPStyle   = VMP_STYLE_ONE; //适配默一画面
		atParam[byOut].m_byMemberNum  = 1;			   //成员个数1
		LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_BAS,"%d->out: Width:%d, Height:%d, Bitrate:%d, FrameRate:%d, EncType:%d\n",
					byOut, atParam[byOut].m_wVideoWidth, atParam[byOut].m_wVideoHeight,
					atParam[byOut].m_wBitRate, atParam[byOut].m_byFrameRate, atParam[byOut].m_byEncType);
	}	

	//是否需要prs 1.需要 0.不需要
	byNeedPrs = atHdAdaptParam[0].IsNeedbyPrs();
    
	
    tDoublePayloadVideo= *(TDoublePayload*)( cServMsg.GetMsgBody() + MAXNUM_BAS_ENCNUM * sizeof(THDAdaptParam) + sizeof(TMediaEncrypt) );

	u8 byIsRcv264DependInMark = *(u8*)(cServMsg.GetMsgBody() + MAXNUM_BAS_ENCNUM * sizeof(THDAdaptParam) + sizeof(TMediaEncrypt) + sizeof(TDoublePayload));


	//适配模式下，设置默认色调是否妥当？
    TVmpStyleCfgInfo tStyleInfo;
	tStyleInfo.ResetDefaultColor();

    CConfId cConfId = cServMsg.GetConfId();

    cServMsg.SetMsgBody();
    
   
    C8KEVMPParam atSetParam[MAXNUM_BAS_ENCNUM]; // 实际设置下去的编码参数
    BOOL32 abIsValid[MAXNUM_BAS_ENCNUM];        // 编码参数合法性记录
    s8 anEncChnOfParam[MAXNUM_BAS_ENCNUM];      // 记录上层传来的编码参数最终对应的画面合成器编码通道索引号
    s8 anParamOfEncChn[MAXNUM_BAS_ENCNUM];      // 记录画面合成器编码通道最终对应的上层传来的编码参数索引号
    for ( byChnIdx = 0; byChnIdx < MAXNUM_BAS_ENCNUM; byChnIdx++ )
    {
        atSetParam[byChnIdx].m_byEncType = MEDIA_TYPE_NULL;
        atSetParam[byChnIdx].m_wBitRate = 0;
        atSetParam[byChnIdx].m_wVideoHeight = 0;
        atSetParam[byChnIdx].m_wVideoWidth = 0;
		atSetParam[byChnIdx].m_byVMPStyle = 0;
		atSetParam[byChnIdx].m_byFrameRate = 0;

        abIsValid[byChnIdx] = FALSE;

        anEncChnOfParam[byChnIdx] = -1;
        anParamOfEncChn[byChnIdx] = -1;
    }

    // 对MCU传来的参数进行有效性整理
    u8 byValidParamNum = 0; // 合法参数数量
    u8 bySetIdx = 0;
    for ( byChnIdx = 0; byChnIdx < MAXNUM_BAS_ENCNUM; byChnIdx++ )
    {
		//编码参数不合法过滤
        if ( MEDIA_TYPE_NULL == atParam[byChnIdx].m_byEncType || 0 == atParam[byChnIdx].m_byEncType )
        {
            continue;
        }

		atSetParam[byChnIdx] = atParam[byChnIdx];
		abIsValid[byChnIdx] = TRUE;;
		bySetIdx++;
        byValidParamNum++;
    }

	//没有合法参数则nack
    if ( 0 == byValidParamNum )
    {
		TEqp tBas;
		tBas.SetMcuEqp(m_tEqpCfg.GetMcuId(), m_tEqpCfg.GetEqpId(), m_tEqpCfg.GetEqpType());
		cServMsg.SetMsgBody((u8*)&tBas, sizeof(tBas));
        LogPrint( LOG_LVL_ERROR, MID_MCU_BAS,"[ChangeCP]: bas Encode param error, no valid param!\n");
		if( MCU_BAS_STARTADAPT_REQ == cServMsg.GetEventId() )
		{
			SendMsgToMcu(cServMsg.GetEventId()+2, &cServMsg);
		}
        return;
    }

    // 打印设置到底层的适配参数
    LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[basinst]/*---------------------------------------------------------*/byIsRcv264DependInMark.%d\n\n",byIsRcv264DependInMark);
    LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[basinst]Following are the params that will set to Media:\n\n");	
    for ( byChnIdx = 0; byChnIdx < MAXNUM_BAS_ENCNUM; byChnIdx++ )
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[basinst]Ch%d: VmpStyle=%d EncType=%s BitRate=%u Video: %u*%u  FrameRate:%d\n", byChnIdx,
            atSetParam[byChnIdx].m_byVMPStyle,GetMediaString(atSetParam[byChnIdx].m_byEncType), 
			atSetParam[byChnIdx].m_wBitRate, atSetParam[byChnIdx].m_wVideoWidth, atSetParam[byChnIdx].m_wVideoHeight, atSetParam[byChnIdx].m_byFrameRate);
    }
    LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[basinst]/*---------------------------------------------------------*/\n\n");
        
	//对于非有效参数的默认码率填适配参数中的最大码率，
	u16 wAdpMaxBR = 0;
	for(byChnIdx = 0; byChnIdx <  MAXNUM_BAS_ENCNUM; byChnIdx++)
	{
		if(abIsValid[byChnIdx] && atSetParam[byChnIdx].m_wBitRate > wAdpMaxBR)
		{
			wAdpMaxBR = atSetParam[byChnIdx].m_wBitRate;
		}
	}
    // 视频编码参数格式转换
    TVidEncParam atEncParam[MAXNUM_BAS_ENCNUM];
    memset( atEncParam, 0, sizeof(atEncParam) );
    for ( byChnIdx = 0; byChnIdx < MAXNUM_BAS_ENCNUM; byChnIdx++ )
    {
		BOOL32 bIsSupportHp = FALSE;
#if defined(_8KH_) || defined(_8KI_)
		if ( atHdAdaptParam[byChnIdx].GetProfileType() == emHpAttrb )
		{
			bIsSupportHp = TRUE;
		}
#endif

        if ( abIsValid[byChnIdx] )
        {
            Trans2EncParam( &atSetParam[byChnIdx], &atEncParam[byChnIdx],bIsSupportHp);
        }
        else
        {
            C8KEVMPParam tDefaultParam;
            tDefaultParam.m_byEncType = MEDIA_TYPE_H264;
            tDefaultParam.m_wBitRate = wAdpMaxBR/*256*/;
            u16 wWidth = 0;
            u16 wHeight = 0;
            GetWHViaRes( VIDEO_FORMAT_CIF, wWidth, wHeight );
            tDefaultParam.m_wVideoWidth = wWidth;
            tDefaultParam.m_wVideoHeight = wHeight;
			
            Trans2EncParam( &tDefaultParam, &atEncParam[byChnIdx],bIsSupportHp);
        }
    }  
    
    // 获得当前合成器实际状态
    TMulPicStatus tMulPicStatus;
    if ( !m_pcHardMulPic->GetStatus(tMulPicStatus) )
    {
		TEqp tBas;
		tBas.SetMcuEqp(m_tEqpCfg.GetMcuId(), m_tEqpCfg.GetEqpId(), m_tEqpCfg.GetEqpType());
		cServMsg.SetMsgBody((u8*)&tBas, sizeof(tBas));
        LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, "[ChangeCP] bas Get bas current MulPicStatus failed!\n" );
		if( MCU_BAS_STARTADAPT_REQ == cServMsg.GetEventId() )
		{
			SendMsgToMcu(cServMsg.GetEventId()+2, &cServMsg);
		}
        return;
    }
	
	s32 nEncNum = byValidParamNum;
  
	BOOL32 bRetOk = TRUE;  
    // 开始合成
    if ( !tMulPicStatus.bMergeStart )
    {
		
        REC_LASTCODELINE;
        bRetOk &= m_pcHardMulPic->StartMerge( atEncParam, m_tSndAddr, m_tSndLocalAddr, nEncNum ); 
        if ( !bRetOk )
        {
            LogPrint( LOG_LVL_ERROR, MID_MCU_BAS,"[ChangeCP] bas Startmerge failed\n");
        }
		m_pcHardMulPic->SetChannelActive(0, TRUE);
        REC_LASTCODELINE;  
    }
    else    //或切换编码
    {
		LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_BAS, "[changecp] bas bMergeStart is open!\n");

        // 调用底层接口，设置编码参数
        for ( byChnIdx = 0; byChnIdx < nEncNum; byChnIdx++ )
        {
            if ( abIsValid[byChnIdx] )
            {
                REC_LASTCODELINE;
                TVidEncParam tSetEncParam = atEncParam[byChnIdx];  // 底层会修改参数中的码率，所以需要使用临时变量作为函数传入参数
                bRetOk &= m_pcHardMulPic->SetEncParam(byChnIdx, &tSetEncParam);
                if ( !bRetOk )
                {
                    LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[BASINST] EncParam%d set failed\n", byChnIdx);
                }
                REC_LASTCODELINE; 
            }
        }
    }

	if( !m_pcHardMulPic->SetRcvInMarker( 0, byIsRcv264DependInMark ) )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_BAS,"[ChangeCP] SetRcvInMarker failed\n");
	}

	 // 记录编码参数和编码通道之间的对应关系
	for ( byChnIdx = 0; byChnIdx < nEncNum; byChnIdx++ )
	{
		if ( abIsValid[byChnIdx] )
		{
			anEncChnOfParam[byChnIdx] = byChnIdx;
			anParamOfEncChn[byChnIdx] = byChnIdx;
		}
    }

    // 第2路编码保持开启
    //bRetOk &= m_pcHardMulPic->SetEncActive(1, bDbVid);
    // 动态判断两路编码是否需要active，zgc，2009-08-20
    REC_LASTCODELINE;
    for ( byChnIdx = 0; byChnIdx < nEncNum; byChnIdx++ )
    {
        bRetOk &= m_pcHardMulPic->SetEncActive( byChnIdx, ( -1 != anParamOfEncChn[byChnIdx] ? TRUE : FALSE ) );
        LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"Set enc chn %d active.%s\n", byChnIdx, ( -1 != anParamOfEncChn[byChnIdx] ? "TRUE" : "FALSE" ) );
    }
    REC_LASTCODELINE;
        
    // 修改画面合成风格
    u32  dwStyle = ConvertVcStyle2HardStyle( atSetParam[0].m_byVMPStyle );  
    if ( atSetParam[0].m_byVMPStyle != m_tCfg.m_tStatus.m_byVMPStyle )// 与原有风格不同
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[ChangeCP]: Change Style from %d to %u!\n", m_tCfg.m_tStatus.m_byVMPStyle, atSetParam[0].m_byVMPStyle);

        // 改变类型
        REC_LASTCODELINE; 
        bRetOk &= m_pcHardMulPic->SetMulPicType(dwStyle);
        if (!bRetOk)
        {
            LogPrint(LOG_LVL_ERROR, MID_MCU_BAS,"[bas] Fail to SetMulPicType\n"); 
        }
        else
        {
            LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[bas] Set hard mulpic type %d\n", dwStyle );
        }
        REC_LASTCODELINE;            
    }

    
    // 设置编码加密参数
    u8  abyKeyBuf[MAXLEN_KEY];   
    s32 nKeyLen = 0;
    u8  abyOldKeyBuf[MAXLEN_KEY];    
    s32 nOldKeyLen = 0;

    TMediaEncrypt tEncKey;
    for ( byChnIdx = 0; byChnIdx < nEncNum; byChnIdx++ )
    {
        if ( abIsValid[byChnIdx] )
        {
            nKeyLen = 0;
            nOldKeyLen = 0;
            memset( abyKeyBuf, 0, MAXLEN_KEY );
            memset( abyOldKeyBuf, 0, MAXLEN_KEY );

            bRetOk &= GetEncryptParam( &atSetParam[byChnIdx], &tEncKey );
            tEncKey.GetEncryptKey( abyKeyBuf, &nKeyLen );
            m_tEncEncrypt[anEncChnOfParam[byChnIdx]].GetEncryptKey( abyOldKeyBuf, &nOldKeyLen );

            if ( tEncKey.GetEncryptMode() != m_tEncEncrypt[anEncChnOfParam[byChnIdx]].GetEncryptMode() ||
                 nKeyLen != nOldKeyLen ||
                 0 != strncmp( (s8*)abyKeyBuf, (s8*)abyOldKeyBuf, nKeyLen ) )
            {
                bRetOk &= SetEncryptParam(anEncChnOfParam[byChnIdx], &tEncKey, atSetParam[byChnIdx].m_byEncType);
                if (!bRetOk)
                {
                   	LogPrint( LOG_LVL_ERROR, MID_MCU_BAS,"[bas] Fail to set encrypt key of EncChn%d\n", anEncChnOfParam[byChnIdx] ); 
                }
                else
                {
                    LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"Set encrypt key of EncChn%d success\n", anEncChnOfParam[byChnIdx] );
                    // 记录新的编码密钥
                    m_tEncEncrypt[anEncChnOfParam[byChnIdx]] = tEncKey;
                }
            }
        }
    }
// 	memset( abyKeyBuf, 0, MAXLEN_KEY );
// 	nKeyLen = 0;
// 	atSetParam[0].m_tVideoEncrypt[0].GetEncryptKey(abyKeyBuf, &nKeyLen);
// 	OspPrintf(TRUE, FALSE, "before SetDecryptParam <DecMode:%d, Key:%s>!\n", atSetParam[0].m_tVideoEncrypt[0].GetEncryptMode(), abyKeyBuf);
	if (!SetDecryptParam(0, &atSetParam[0].m_tVideoEncrypt[0], &tDoublePayloadVideo))
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_BAS,"[SetDecryptParam] bas failed!\n");
	}
	
    
//     for ( byChnNo = 0; byChnNo < atSetParam[0].m_byMemberNum; byChnNo++)
//     {        
//         // 原存在而现在不存在则先删除
//         if (m_abChnlValid[byChnNo] && atSetParam[0].m_atMtMember[byChnNo].IsNull())
//         {
//             REC_LASTCODELINE;          
//             if (!m_pcHardMulPic->SetChannelActive(byChnNo, FALSE) )
//             {
//                 bRetOk = FALSE;
//                 OspPrintf(TRUE, FALSE, "[vmp] Fail to SetChannelActive, chn.%d\n", byChnNo); 
//             }
//             else
//             {
//                 baslog("[ChangeCP]Remove channel %d.\n", byChnNo);
//                 m_pcHardMulPic->ClearChannelCache( byChnNo );
//             }
//             REC_LASTCODELINE;      
//         }
//         // 原先不存在但现在存在
//         else if ( !atSetParam[0].m_atMtMember[byChnNo].IsNull() )
//         {
//             nKeyLen = 0;
//             nOldKeyLen = 0;
//             memset( abyKeyBuf, 0, MAXLEN_KEY );
//             memset( abyOldKeyBuf, 0, MAXLEN_KEY );
// 
//             atSetParam[0].m_tVideoEncrypt[byChnNo].GetEncryptKey( abyKeyBuf, &nKeyLen );
//             m_tParam[0].m_tVideoEncrypt[byChnNo].GetEncryptKey( abyOldKeyBuf, &nOldKeyLen );           
// 
//             if ( !m_abChnlValid[byChnNo] || ( atSetParam[0].m_tVideoEncrypt[byChnNo].GetEncryptMode() != m_tParam[0].m_tVideoEncrypt[byChnNo].GetEncryptMode() ||
//                                               nKeyLen != nOldKeyLen ||
//                                               0 != strncmp( (s8*)abyKeyBuf, (s8*)abyOldKeyBuf, nKeyLen ) ) )
//             {
// 				OspPrintf(TRUE, FALSE, "[ChangeCp] SetDecryptParam <Key:%s>\n", abyKeyBuf);
//                 if (!SetDecryptParam(byChnNo, &atSetParam[0].m_tVideoEncrypt[byChnNo], &atSetParam[0].m_tDoublePayload[byChnNo]))
//                 {
//                     bRetOk = FALSE;
//                     OspPrintf(TRUE, FALSE, "[vmp] Fail to SetDecryptParam mmb.%d\n", byChnNo); 
//                 }
// /*                
//                 tNetAddr.wPort = m_tEqpCfg.GetEqpStartPort();
//                 tNetRtcpAddr.wPort = GetSendRtcpPort(byChnNo);
// 				OspPrintf(TRUE, FALSE, "[ChangeCP] Ins:%d Local rcv Port:%d!\n", GetInsID(), tNetAddr.wPort);
// 
//                 REC_LASTCODELINE;
//                 bRetOk &= m_pcHardMulPic->SetChannelParam(byChnNo, tNetAddr, tNetRtcpAddr);
// 
//                 REC_LASTCODELINE;
//                 bRetOk &= m_pcHardMulPic->SetChannelActive(byChnNo, TRUE);
//                 REC_LASTCODELINE;
//                
//                 baslog("[ChangeCP]Add channel %d for mt%d at port %d, pt: %d/%d, ret=%d\n", 
//                         byChnNo, 
//                         atSetParam[0].m_atMtMember[byChnNo].GetMtId(), 
//                         tNetAddr.wPort,
//                         atSetParam[0].m_tDoublePayload[byChnNo].GetRealPayLoad(),
//                         atSetParam[0].m_tDoublePayload[byChnNo].GetActivePayload(),
//                         bRetOk);
// 				*/
//             }  
//         }                          
//     }   
    
    if (byCurState == NORMAL)
    {
        // 设置丢包重传参数，仅在开始时设置一次
        TNetRSParam tNetRSParam;    
        if (byNeedPrs)
        {
			tNetRSParam.m_wFirstTimeSpan = m_tPrsTimeSpan.m_wFirstTimeSpan;
            tNetRSParam.m_wSecondTimeSpan = m_tPrsTimeSpan.m_wSecondTimeSpan;
            tNetRSParam.m_wThirdTimeSpan = m_tPrsTimeSpan.m_wThirdTimeSpan;
            tNetRSParam.m_wRejectTimeSpan = m_tPrsTimeSpan.m_wRejectTimeSpan;

            REC_LASTCODELINE;
            m_pcHardMulPic->SetNetSendFeedbackVideoParam(2000, TRUE);
            REC_LASTCODELINE;
            m_pcHardMulPic->SetNetRecvFeedbackVideoParam(tNetRSParam, TRUE);
            REC_LASTCODELINE;
        }
        else
        {
            tNetRSParam.m_wFirstTimeSpan = 0;
            tNetRSParam.m_wSecondTimeSpan = 0;
            tNetRSParam.m_wThirdTimeSpan = 0;
            tNetRSParam.m_wRejectTimeSpan = 0;
            
            REC_LASTCODELINE;
            m_pcHardMulPic->SetNetSendFeedbackVideoParam(2000, FALSE);
            REC_LASTCODELINE;
            m_pcHardMulPic->SetNetRecvFeedbackVideoParam(tNetRSParam, FALSE);
            REC_LASTCODELINE;
        }     
        LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[ChangeCP]SetNetSendFeedbackVideoParam Prs=%d, FirstTimeSpan=%d\n", m_byNeedPrs, tNetRSParam.m_wFirstTimeSpan);
    }

    
    if ( !bRetOk )
    {
		TEqp tBas;
		tBas.SetMcuEqp(m_tEqpCfg.GetMcuId(), m_tEqpCfg.GetEqpId(), m_tEqpCfg.GetEqpType());
		cServMsg.SetMsgBody((u8*)&tBas, sizeof(tBas));
        LogPrint( LOG_LVL_ERROR, MID_MCU_BAS,"[Info] bas ChangeCP failed\n");
		if( MCU_BAS_STARTADAPT_REQ == cServMsg.GetEventId() )
		{
			SendMsgToMcu(cServMsg.GetEventId()+2, &cServMsg);
		}
        return;
    }

	u8 byChnNo = 0;
    for ( byChnNo = 0; byChnNo < atSetParam[0].m_byMemberNum; byChnNo++)
    {
        m_abChnlValid[byChnNo] = atSetParam[0].m_atMtMember[byChnNo].IsNull() ? FALSE : TRUE;
    }
	
	// [pengjie 2010/5/13] 出错打印
	if( m_abChnlValid[0] == FALSE )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS,"[ChangeCP] Set m_abChnlValid[0] == FALSE !\n" );
	}

    memcpy( m_tParam, atSetParam, MAXNUM_BAS_ENCNUM * sizeof(C8KEVMPParam));
    m_byNeedPrs = byNeedPrs;
    m_tStyleInfo = tStyleInfo;
//    m_tCapSupportEx = tCapSupportEx;
    m_cConfId = cConfId;
    //m_bDbVid = ( MAXNUM_BAS_ENCNUM == byValidParamNum ) ? TRUE : FALSE;

	m_byEncNum = nEncNum;	//保存当前需要编码的路数
    m_tCfg.m_tStatus.m_byVMPStyle = atSetParam[0].m_byVMPStyle;
    m_tCfg.m_tStatus.m_byMemberNum = atSetParam[0].m_byMemberNum;
    
    TEqp tBas;
	tBas.SetMcuEqp(m_tEqpCfg.GetMcuId(), m_tEqpCfg.GetEqpId(), m_tEqpCfg.GetEqpType());
	cServMsg.SetMsgBody((u8*)&tBas, sizeof(tBas));
	if( MCU_BAS_STARTADAPT_REQ == cServMsg.GetEventId() )
	{
		SendMsgToMcu(cServMsg.GetEventId()+1, &cServMsg);
	}
    
	//[nizhijun 2011/03/21]拖发言人/切换发言他人时BAS关键帧定时检测还是设置为1秒来进行检测
	KillTimer(EV_BAS_NEEDIFRAME_TIMER);	
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[ChangeCP] basparam changed,kill timer:%d, reset EV_BAS_NEEDIFRAME_TIMER:%d\n",
		g_cBasApp.m_dwIframeInterval, CHECK_IFRAME_INTERVAL);
	m_byIframeReqCount = 0;
	SetTimer(EV_BAS_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL);

    // 取得当前新设置的合成状态及统计
//     m_pcHardMulPic->GetStatus( tMulPicStatus );
//     TMulPicStatis tMulPicStatis;
//     m_pcHardMulPic->GetStatis( tMulPicStatis );

    // VMP实际编码情况构建消息体
//     u8 byEncNum = MAXNUM_BAS_ENCNUM; 
//     TVideoStreamCap atVmpOutPutCap[MAXNUM_BAS_ENCNUM];
//     for ( byChnIdx = 0; byChnIdx < MAXNUM_BAS_ENCNUM; byChnIdx++ )
//     {
//         if ( -1 != anParamOfEncChn[byChnIdx] )
//         {
//             atVmpOutPutCap[byChnIdx].SetMediaType( tMulPicStatus.atVidEncParam[byChnIdx].m_byEncType );
//             atVmpOutPutCap[byChnIdx].SetResolution( GetResViaWH( tMulPicStatus.atVidEncParam[byChnIdx].m_wEncVideoWidth, tMulPicStatus.atVidEncParam[byChnIdx].m_wEncVideoHeight ) );
//             atVmpOutPutCap[byChnIdx].SetMaxBitRate( atEncParam[anParamOfEncChn[byChnIdx]].m_dwBitRate );   // 这里用设置值，因为底层会对设置的码率打折扣
//             atVmpOutPutCap[byChnIdx].SetUserDefFrameRate( 25 );    // 固定25帧
//         }
//     }
   
//    cServMsg.SetMsgBody( (u8*)&byEncNum, sizeof(byEncNum) );
//    cServMsg.CatMsgBody( (u8*)atVmpOutPutCap, sizeof(atVmpOutPutCap) );
//    SendMsgToMcu(BAS_MCU_CHNNLSTATUS_NOTIF, &cServMsg);  
    
//    SendStatusChangeMsg(TRUE, TVmpStatus::CPING);
    LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[ChangeCP] Ins<%d>'s Curstate is :%d!\n ", GetInsID(), CurState());
    return;
}

/*=============================================================================
函 数 名： GetEncryptParam
功    能： 
算法实现： 
全局变量： 
参    数： [IN] C8KEVMPParam *ptParam
           [OUT] TMediaEncrypt *ptVideoEncrypt
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/7/3   4.0			周广程                  创建
=============================================================================*/
BOOL32 CBASInst::GetEncryptParam( C8KEVMPParam *ptParam, TMediaEncrypt *ptVideoEncrypt )
{
    if ( NULL == ptParam || NULL == ptVideoEncrypt )
    {
        LogPrint( LOG_LVL_ERROR, MID_MCU_BAS,"[GetEncryptParam] Error! Point as param is null!\n" );
        return FALSE;
    }
    for ( u8 byEncIdx = 0; byEncIdx < ptParam->m_byMemberNum; byEncIdx++ )
    {
        if ( ptParam->m_tVideoEncrypt[byEncIdx].GetEncryptMode() != CONF_ENCRYPTMODE_NONE )
        {
            *ptVideoEncrypt = ptParam->m_tVideoEncrypt[byEncIdx];
            break;
        }
    }
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
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CBASInst::SetEncryptParam(u8 byChnNo, TMediaEncrypt *ptVideoEncrypt, u8 byMeidaType)
{
    if( NULL == ptVideoEncrypt )
	{
		return FALSE;
	}

    u8  abyKeyBuf[MAXLEN_KEY];
	memset( abyKeyBuf, 0, MAXLEN_KEY );

    s32 nKeyLen = 0;

    u8 byActivePt = VmpGetActivePayload(byMeidaType);
 
    u8 byEncryptMode = ptVideoEncrypt->GetEncryptMode();

    BOOL32 bRet = TRUE;
    if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
    {              
        LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[Info]Encode encrypt key is NULL\n");

        REC_LASTCODELINE;  
        bRet &= m_pcHardMulPic->SetEncryptKey(byChnNo, NULL, 0, 0);
        REC_LASTCODELINE; 
    }
    else  if (CONF_ENCRYPTMODE_DES == byEncryptMode || CONF_ENCRYPTMODE_AES == byEncryptMode) // 加密情况下
    {
        ptVideoEncrypt->GetEncryptKey( abyKeyBuf, &nKeyLen );
        byEncryptMode = (CONF_ENCRYPTMODE_DES == byEncryptMode) ? DES_ENCRYPT_MODE : AES_ENCRYPT_MODE; // 上下层宏定义转换
            
        LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[Info]Encode encrypt Mode: %u KeyLen: %u Actove PT: %u Key:%s \n",
                        ptVideoEncrypt->GetEncryptMode(), nKeyLen, byActivePt, abyKeyBuf);

        REC_LASTCODELINE; 
        bRet &= m_pcHardMulPic->SetEncryptKey(byChnNo, (s8*)abyKeyBuf, (u16)nKeyLen, byEncryptMode);
        REC_LASTCODELINE; 

        /*
        vmplog("[Info]Key is : ");
        for (u8 byKeyLoop = 0; byKeyLoop < nKeyLen; byKeyLoop++) // 调试用
        {
            vmplog("%x ", abyKeyBuf[byKeyLoop]);
        }
        vmplog("\n");
        */
    }
    return bRet;
}

/*=============================================================================
  函 数 名： SetDecryptParam
  功    能： 设置解码参数
  算法实现： 
  全局变量： 
  参    数： u8 byChnNo
             TMediaEncrypt *  ptVideoEncrypt
             TDoublePayload * ptDoublePayload
  返 回 值： void 
=============================================================================*/
BOOL32 CBASInst::SetDecryptParam(u8 byChnNo, TMediaEncrypt *ptVideoEncrypt, TDoublePayload *ptDoublePayload)
{
    if( NULL == ptVideoEncrypt || NULL == ptDoublePayload )
    {
        return FALSE;
    }

    u8 byEncryptMode;
    u8 abyKeyBuf[MAXLEN_KEY];
    s32 byKenLen = 0;
    memset(abyKeyBuf, 0, MAXLEN_KEY);

    BOOL32 bRet = TRUE;
    REC_LASTCODELINE; 
    if ( NULL != ptDoublePayload && 0 == ptDoublePayload->GetActivePayload() )
    {
        ptDoublePayload->SetActivePayload( ptDoublePayload->GetRealPayLoad() );
    }
    bRet &= m_pcHardMulPic->SetVideoActivePT(byChnNo, ptDoublePayload->GetActivePayload(), ptDoublePayload->GetRealPayLoad());

    REC_LASTCODELINE; 
    byEncryptMode = ptVideoEncrypt->GetEncryptMode();
    if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
    {        
        LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[Info]Decrypt key to NULL!\n");
        REC_LASTCODELINE; 
        bRet &= m_pcHardMulPic->SetDecryptKey(byChnNo, NULL, 0, 0);
        REC_LASTCODELINE; 
    }
    else  if (CONF_ENCRYPTMODE_DES == byEncryptMode || CONF_ENCRYPTMODE_AES == byEncryptMode)
    {
        byEncryptMode = (CONF_ENCRYPTMODE_DES == byEncryptMode) ? DES_ENCRYPT_MODE : AES_ENCRYPT_MODE;

        ptVideoEncrypt->GetEncryptKey(abyKeyBuf, &byKenLen);
        REC_LASTCODELINE; 
        bRet &= m_pcHardMulPic->SetDecryptKey(byChnNo, (s8*)abyKeyBuf, (u16)byKenLen, byEncryptMode);
        REC_LASTCODELINE; 

        LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[Info] Decrypt Mode: %u KeyLen: %u Active PT: %u\n",
                ptVideoEncrypt->GetEncryptMode(), 
                byKenLen, 
                ptDoublePayload->GetActivePayload());

        /*
        vmplog("Key is : ");
        for (u8 byKeyLoop=0; byKeyLoop<byKenLen; byKeyLoop++)
        {
            vmplog("%x ", abyKeyBuf[byKeyLoop]);
        }
       vmplog("\n");
       */
    }
    return bRet;
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
void CBASInst::ProcStopAdapt(CMessage * const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);
     LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[ProcStopAdapt] Ins<%d>'s Curstate is :%d!\n ", GetInsID(), CurState());
    switch(CurState())
    {
    case RUNNING:
        break;
    default:
        LogPrint( LOG_LVL_ERROR, MID_MCU_BAS,"[ProcStopAdapt] Ins<%d> Error State:%d\n", GetInsID(), CurState());
        return;
    }
    cServMsg.SetMsgBody();
    SendMsgToMcu(BAS_MCU_STOPADAPT_ACK, &cServMsg);
  
	memset(m_tParam, 0, sizeof(m_tParam));	
    m_tCfg.Clear();

    // 停止复合
    s32 nRetCode = m_pcHardMulPic->StopMerge();
    if ( nRetCode )
    {
        cServMsg.SetErrorCode(0);
    }
    else
    {
         LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"Call stopmerge() error code= %d\n", nRetCode );
         cServMsg.SetErrorCode(1);
    }

    memset( m_tParam, 0, sizeof(m_tParam) );
    m_tStyleInfo.Clear();
    memset( &m_tCapSupportEx, 0, sizeof(TCapSupportEx) );


    for ( u8 byChnIdx = 0; byChnIdx < MAXNUM_BAS_ENCNUM; byChnIdx++ )
    {
        m_pcHardMulPic->SetEncryptKey(byChnIdx, NULL, 0, 0);
        m_tEncEncrypt[byChnIdx].Reset();
    }

    for (u8 byChnNo = 0; byChnNo < m_tCfg.m_tStatus.m_byMemberNum; byChnNo++)
    {        
        m_pcHardMulPic->SetDecryptKey( byChnNo, NULL, 0, 0 );
    }
    
    LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"we clear EncryptKey DecryptKey EncryptPT ActivePT Info success!\n");


    m_byNeedPrs = FALSE;

    m_cConfId.SetNull();
    //m_bDbVid = FALSE;
	m_byEncNum = 0;
    m_tCfg.m_tStatus.m_byVMPStyle = VMP_STYLE_NONE;
    m_tCfg.m_tStatus.m_byMemberNum = 0;
    memset( m_abChnlValid, FALSE, sizeof(m_abChnlValid) );

	LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[ProcStopAdapt] Set m_abChnlValid[0] == FALSE !\n" );
    
    /*
    // 设丢包重传参数
    TNetRSParam tNetRSParam;
    tNetRSParam.m_wFirstTimeSpan = 0;
    tNetRSParam.m_wSecondTimeSpan = 0;
    tNetRSParam.m_wThirdTimeSpan = 0;
    tNetRSParam.m_wRejectTimeSpan = 0;
    m_pcHardMulPic->SetNetSendFeedbackVideoParam(2000, FALSE);
    m_pcHardMulPic->SetNetRecvFeedbackVideoParam(tNetRSParam, FALSE);
    */

    //cServMsg.SetMsgBody();
    //SendMsgToMcu(VMP_MCU_STOPVIDMIX_NOTIF, &cServMsg);

	//停止适配，主控侧已经更新BAS状态为ready，故此处没必要再发送
  //  SendStatusChangeMsg(TRUE, u8(THDBasChnStatus::READY));

    NEXTSTATE(NORMAL);
    return ;
} 



/*=============================================================================
  函 数 名： MsgUpdataVmpEncryptParamProc
  功    能： 更改加密参数
  算法实现： 
  全局变量： 
  参    数： CMessage * const pMsg
  返 回 值： void 
=============================================================================*/
void CBASInst::MsgUpdataVmpEncryptParamProc(CMessage * const pMsg)
{
    if( NULL == pMsg )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_BAS,"[MsgUpdataVmpEncryptParamProc] Error input parameter.\n");
		return;
	}

	u8  byChannelNo = 0;
    TMediaEncrypt  tVideoEncrypt;
    TDoublePayload tDoublePayload;

    CServMsg cServMsg(pMsg->content, pMsg->length);

    byChannelNo    = *(u8 *)(cServMsg.GetMsgBody());
    tVideoEncrypt  = *(TMediaEncrypt *)(cServMsg.GetMsgBody() + sizeof(u8));
    tDoublePayload = *(TDoublePayload *)(cServMsg.GetMsgBody() + sizeof(u8) + sizeof(TMediaEncrypt));


    LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[UpdateChannel] byChannelNo: %d, ActivePT: %d, RealPT: %d\n", 
            byChannelNo,
            tDoublePayload.GetActivePayload(),
            tDoublePayload.GetRealPayLoad());    

    SetDecryptParam(byChannelNo, &tVideoEncrypt, &tDoublePayload);

	// [pengjie 2010/5/13] 出错打印
	LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "[MsgUpdataVmpEncryptParamProc] Get m_abChnlValid[%d] == %d !\n", m_abChnlValid[byChannelNo] );
	LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "[MsgUpdataVmpEncryptParamProc] Get m_abChnlValid[0] == %d !\n", m_abChnlValid[0] );
	// End

    if ( !m_abChnlValid[byChannelNo] )
    {
        TNetAddress tNetAddr;
        TNetAddress tNetRtcpAddr;
        TNetAddress tLocalSndRtcpAddr;
        tNetAddr.dwIp = 0;
        tNetRtcpAddr.dwIp = m_tSndAddr[0].dwIp;
        tNetAddr.wPort = g_cBasApp.m_tEqpCfg.GetEqpStartPort() + byChannelNo*PORTSPAN;     
		tLocalSndRtcpAddr.dwIp = 0;
		tLocalSndRtcpAddr.wPort = 0;
        tNetRtcpAddr.wPort = GetSendRtcpPort(byChannelNo);         
        /*s32 nRet = */m_pcHardMulPic->SetChannelParam(byChannelNo, tNetAddr, tNetRtcpAddr, tLocalSndRtcpAddr);
        /*nRet &= */m_pcHardMulPic->SetChannelActive(byChannelNo, TRUE);

        m_abChnlValid[byChannelNo] = TRUE;
    }

    cServMsg.SetMsgBody();
    SendMsgToMcu(VMP_MCU_CHANGESTATUS_NOTIF, &cServMsg);

	return;
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
/*
void CBASInst::MsgSetBitRate(CMessage* const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);
    u8 byChalNum = cServMsg.GetChnIndex();
    u16 wBitrate;

    cServMsg.GetMsgBody((u8*)&wBitrate, sizeof(u16));
    wBitrate = ntohs(wBitrate);

    baslog("[Info]now we set the encode-%u bitrate to %u!\n", byChalNum, wBitrate);

    if ( byChalNum > 0 && byChalNum <= MAXNUM_BAS_ENCNUM )
    {
        m_pcHardMulPic->ChangeEncBitRate(byChalNum-1, wBitrate);
        m_tParam[byChalNum-1].m_wBitRate = wBitrate; 
    }
    
//     if (1 == byChalNum) // 是否双发送
//     {
//         m_pcHardMulPic->ChangeEncBitRate(0, wBitrate);
// 		m_tParam[0].m_wBitRate = wBitrate; //保存新码率, zgc, 2007-10-09
//     }
//     if (2 == byChalNum && m_bDbVid)
//     {
//         m_pcHardMulPic->ChangeEncBitRate(1, wBitrate);
// 		m_tParam[1].m_wBitRate = wBitrate; //保存新码率, zgc, 2007-10-09
//     }

    SendMsgToMcu(VMP_MCU_SETCHANNELBITRATE_ACK, &cServMsg);
}
*/
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
void CBASInst::MsgTimerNeedIFrameProc(void)
{
    KillTimer(EV_BAS_NEEDIFRAME_TIMER);

    CServMsg cServMsg;
    cServMsg.SetConfId(m_cConfId);
    TVidRecvStatis tDecStatis;

    //songkun,20110627,Iframe请求TimerOut策略调整                    
    if( 0 == m_byIframeReqCount)
    {
        u8 byChnNo = 0;//目前Bas成员个数1
        memset(&tDecStatis, 0, sizeof(TVidRecvStatis));
        m_pcHardMulPic->GetVidRecvStatis(byChnNo, tDecStatis);
	    LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[MsgTimerNeedIFrameProc] m_bRequestKeyFrame :%d!\n", tDecStatis.m_bRequestKeyFrame);
        //只有BAS运行的编解码的时候，才发关键帧请求
		if (tDecStatis.m_bRequestKeyFrame && CurState() == RUNNING)
        {
            //用户可配
	        m_byIframeReqCount = (u8)(g_cBasApp.m_dwIframeInterval / CHECK_IFRAME_INTERVAL);;
            cServMsg.SetChnIndex(byChnNo);
            SendMsgToMcu(BAS_MCU_NEEDIFRAME_CMD, &cServMsg);
            LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[MsgTimerNeedIFrameProc]bas channel:%d request iframe,next time is %u !!\n", byChnNo,m_byIframeReqCount);
        }	
    }
    if ( 0 != m_byIframeReqCount)
    {
       --m_byIframeReqCount;                                    
    }
    SetTimer(EV_BAS_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL);
    LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[MsgTimerNeedIFrameProc]bas set EV_BAS_NEEDIFRAME_TIMER: %d !\n", CHECK_IFRAME_INTERVAL);
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
====================================================================*/
void CBASInst::MsgFastUpdatePicProc(CMessage * const pMsg)
{
	CServMsg cServMsg( pMsg->content, pMsg->length );
	u8 byChnnlId = cServMsg.GetChnIndex();

    u32 dwTimeInterval = (u32)(OspClkRateGet()*0.5);  
    u32 dwCurTick = OspTickGet();	
    if( dwCurTick - m_dwLastFUPTick > dwTimeInterval)
    {		
        m_dwLastFUPTick = dwCurTick;
		if (m_byEncNum > 0)
		{	
			for(u8 byOutId = 0; byOutId < m_byEncNum; byOutId++)
			{
				m_pcHardMulPic->SetFastUpdata(byOutId);
			}
		}
		else
		{
			 LogPrint(LOG_LVL_ERROR, MID_MCU_BAS,"bas[MsgFastUpdatePicProc] m_byEncNum:%d <= 0!\n", m_byEncNum);
		}
        LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"bas[MsgFastUpdatePicProc]m_pcHardMulPic->SetFastUpdata(%d)!\n", byChnnlId);
    }
	return;
}

/*=============================================================================
函 数 名： MsgChnnlLogOprCmd
功    能： 
算法实现： 
全局变量： 
参    数： CMessage * const pMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/9/9   4.0			周广程                  创建
=============================================================================*/
void CBASInst::MsgChnnlLogOprCmd( CMessage * const pMsg )
{
//     CServMsg cServMsg( pMsg->content, pMsg->length );
// 
//     u8 byIsAddLog = *(u8*)cServMsg.GetMsgBody();
//     BOOL32 IsDisplayMmbAlias = ( 1 == byIsAddLog ) ? TRUE : FALSE;
// 
//     for ( u8 byChnNo = 0; byChnNo < m_tParam[0].m_byMemberNum; byChnNo++)
//     {
//         if ( !m_tParam[0].m_atMtMember[byChnNo].IsNull() )
//         {
//             if ( IsDisplayMmbAlias )
//             {
//                 REC_LASTCODELINE;
//                 m_pcHardMulPic->SetLogo( byChnNo, (s8*)m_tParam[0].m_atMtMember[byChnNo].GetMbAlias(), strlen(m_tParam[0].m_atMtMember[byChnNo].GetMbAlias())+1 );
//                 LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"Mmb.%d alias is %s\n", byChnNo, m_tParam[0].m_atMtMember[byChnNo].GetMbAlias() );
//                 REC_LASTCODELINE;
//             }
//             REC_LASTCODELINE;
//             m_pcHardMulPic->SetLogoActive( byChnNo, IsDisplayMmbAlias ); 
//             REC_LASTCODELINE;
// 
//         }       
//     }
}

/*====================================================================
	函数  : MsgGetVidMixParamProc
	功能  : MCU查询画面复合参数消息响应
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	03/12/4		v1.0		zhangsh			create
====================================================================*/
/*
void CBASInst::MsgGetVidMixParamProc(CMessage* const pMsg)
{
    return;
}
*/
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
void CBASInst::StatusShow()
{
    
    TMulPicStatus tMulPicStatus;
    TMulPicStatis tMulPicStatis;
    TVidRecvStatis tRecvStatis;
    TVidSendStatis tSendStatis;
    memset(&tMulPicStatus, 0, sizeof(tMulPicStatus));
    memset(&tMulPicStatis, 0, sizeof(tMulPicStatis));
    memset(&tRecvStatis, 0, sizeof(tRecvStatis));
    memset(&tSendStatis, 0, sizeof(tSendStatis));

    m_pcHardMulPic->GetStatus(tMulPicStatus);
    ::OspPrintf(TRUE, FALSE, "\t===================The param passed by mcu===================\n\n");
    ::OspPrintf(TRUE, FALSE, "Following are the params that mcu send to the bas:\n\n");

	for (u8 byIdx = 0; byIdx < MAXNUM_BAS_ENCNUM; byIdx++)
	{	
		::OspPrintf(TRUE, FALSE, "Chn%d: EncType=%s BitRate=%u Video: %u*%u \n",
					byIdx, 
					GetMediaString(m_tParam[byIdx].m_byEncType), m_tParam[byIdx].m_wBitRate, 
                    m_tParam[byIdx].m_wVideoWidth, m_tParam[byIdx].m_wVideoHeight);
	}
    
    
    ::OspPrintf(TRUE, FALSE, "\t============================================================\n\n");

    ::OspPrintf(TRUE, FALSE, "\t===========The state of Bas is below===============\n");

    ::OspPrintf(TRUE, FALSE, "Codec Init OK? %s\n", m_bInitOk ? "YES" : "NO" );
	
	::OspPrintf(TRUE, FALSE, "The MTU Size is : %d\n", m_wMTUSize );

    ::OspPrintf(TRUE, FALSE, "  IDLE =%d, NORMAL=%d, RUNNING=%d, bas Current State =%d\n",
	                          IDLE, NORMAL, RUNNING, CurState());

    ::OspPrintf(TRUE, FALSE, "\t===========The status of VMP is below===============\n");
    ::OspPrintf(TRUE, FALSE, "CPing=%s\t Style=%s(%d)\t ChannelNum=%d(Real=%d), EncNum=%d(Real=%d)\n",
                              tMulPicStatus.bMergeStart ? "Y" : "N",
                              BasHardStyleStr(tMulPicStatus.dwMergeType), tMulPicStatus.dwMergeType,
                              tMulPicStatus.byChnNum,
                              m_tParam[0].m_byMemberNum,
                              tMulPicStatus.byEncNum);
                              /*m_bDbVid ? 2 : 1*/
    s32 nLoop = 0;
    for (nLoop =0; nLoop < tMulPicStatus.byChnNum; nLoop++)
    {    
        ::OspPrintf(TRUE, FALSE, "Receive Channel(%2d) Valid: %d  Port:%d WaitIFCountDown: %2d\n",
                    nLoop, m_abChnlValid[nLoop], tMulPicStatus.atNetRcvAddr[nLoop].wPort,m_byIframeReqCount);
    }

    ::OspPrintf(TRUE, FALSE, "\t-------------The Video Encoding Params--------------\n");
    for (nLoop =0; nLoop < tMulPicStatus.byEncNum; nLoop++)
    {
        ::OspPrintf(TRUE, FALSE, "Chnl.%2d\t EncType=%d\t KeyFrameIntv=%d\t MaxQuant=%d\t MinQuant=%d\t\n",
                                  nLoop,
                                  tMulPicStatus.atVidEncParam[nLoop].m_byEncType,
                                  tMulPicStatus.atVidEncParam[nLoop].m_dwMaxKeyFrameInterval,
                                  tMulPicStatus.atVidEncParam[nLoop].m_byMaxQuant,
                                  tMulPicStatus.atVidEncParam[nLoop].m_byMinQuant);

        ::OspPrintf(TRUE, FALSE, "BitRate=%d\t VideoSize=%d*%d\t\n",
                                  tMulPicStatus.atVidEncParam[nLoop].m_dwBitRate,                                  
                                  tMulPicStatus.atVidEncParam[nLoop].m_wEncVideoWidth,
                                  tMulPicStatus.atVidEncParam[nLoop].m_wEncVideoHeight);

        ::OspPrintf(TRUE, FALSE, "Send Ip Address:%s:%d\n", 
                    strofip(tMulPicStatus.atNetSndAddr[nLoop].dwIp, TRUE), tMulPicStatus.atNetSndAddr[nLoop].wPort);
    }


    ::OspPrintf(TRUE, FALSE, "\t-------------Recv Network Statics --------------\n");

    for(nLoop = 0; nLoop < tMulPicStatus.byChnNum; nLoop++)
    {
        m_pcHardMulPic->GetVidRecvStatis(nLoop, tRecvStatis);
        ::OspPrintf(TRUE, FALSE, "Recv Chl(%2d) BitRate:%4u FrameRate:%2d RecvPack:%6d LostPack:%4d VideoSize:%d*%d ReqIFrame:%d \n",
                                  nLoop,
                                  tRecvStatis.m_dwVideoBitRate,
                                  tRecvStatis.m_dwVideoFrameRate,
                                  tRecvStatis.m_dwVideoRecvFrame,
                                  tRecvStatis.m_dwVideoLoseFrame,
                                  tRecvStatis.m_dwDecdWidth,
                                  tRecvStatis.m_dwDecdHeight,
                                  tRecvStatis.m_bRequestKeyFrame,
                                  tRecvStatis.m_dwFullLose,
                                  tRecvStatis.m_dwDecFailCount
                   );
    }

    ::OspPrintf(TRUE, FALSE, "\t-------------Send Network Statics --------------\n");

    for (nLoop =0; nLoop < tMulPicStatus.byEncNum; nLoop++)
    {
        m_pcHardMulPic->GetVidSendStatis(nLoop, tSendStatis);
        ::OspPrintf(TRUE, FALSE, "Send Chl(%2d) BitRate=%4u FrameRate=%d SendFail=%4d EncFail=%4d\n",
                    nLoop,
                    tSendStatis.m_dwSendBitRate,
                    tSendStatis.m_byVideoFrameRate,
                    tSendStatis.m_dwSendFailCount,
                    tSendStatis.m_dwEncFailCount);
    }

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
2008/3/19   4.0		    周广程                  创建
2010/2/8    4.6         
=============================================================================*/
void CBASInst::ParamShow(void)
{
	::OspPrintf(TRUE, FALSE, "\t===========================The VC Param of BAS is below==========================\t\n");
	for (u8 byIdx = 0; byIdx < MAXNUM_BAS_ENCNUM; byIdx++)
	{
		::OspPrintf(TRUE, FALSE, "\t----------------------------- chnnl %d -----------------------------\t\n", byIdx+1);
		m_tParam[byIdx].Print();
	    ::OspPrintf(TRUE, FALSE, "\n");
		if (byIdx == MAXNUM_BAS_ENCNUM - 1)
		{
			::OspPrintf(TRUE, FALSE, "\t================================================================================\t\n");
		}
	}
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
void CBASInst::SendStatusChangeMsg( u8 byOnline, u8 byState)
{
    CServMsg cServMsg;
    TPeriEqpStatus tBasStatus;
    memset(&tBasStatus,0,sizeof(TPeriEqpStatus));

    tBasStatus.m_byOnline = byOnline;
    tBasStatus.SetMcuEqp((u8)m_tEqpCfg.GetMcuId(), m_tEqpCfg.GetEqpId(), m_tEqpCfg.GetEqpType());
	tBasStatus.m_tStatus.tHdBas.tStatus.t8keBas.GetVidChnStatus(0)->SetStatus(byState);
	
    tBasStatus.SetAlias(m_tEqpCfg.GetAlias());
    
    cServMsg.SetMsgBody((u8*)&(tBasStatus), sizeof(TPeriEqpStatus));
	cServMsg.CatMsgBody((u8*)&m_byChnType,sizeof(m_byChnType));
    SendMsgToMcu(BAS_MCU_BASSTATUS_NOTIF, &cServMsg);
    return;
}

/*=============================================================================
  函 数 名： ProcSetQosInfo
  功    能： 设置Qos值
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
=============================================================================*/
void CBASInst::ProcSetQosInfo(CMessage* const pMsg)
{
    if(NULL == pMsg)
    {
        OspPrintf(TRUE, FALSE, "[ProcSetQosInfo] ProcSetQosInfo infomation is Null\n");
        return;
    }
	return;
// 
//     TMcuQosCfgInfo  tQosInfo;
//     CServMsg cServMsg(pMsg->content, pMsg->length);
//     tQosInfo = *(TMcuQosCfgInfo*)cServMsg.GetMsgBody();
// 
//     u8 byQosType = tQosInfo.GetQosType();
//     u8 byAudioValue = tQosInfo.GetAudLevel();
//     u8 byVideoValue = tQosInfo.GetVidLevel();
//     u8 byDataValue = tQosInfo.GetDataLevel();
//     u8 byIpPriorValue = tQosInfo.GetIpServiceType();
//     if(QOSTYPE_IP_PRIORITY == byQosType)
//     {
//         byAudioValue = (byAudioValue << 5);
//         byVideoValue = (byVideoValue << 5);
//         byDataValue = (byDataValue << 5);
//         ComplexQos(byAudioValue, byIpPriorValue);
//         ComplexQos(byVideoValue, byIpPriorValue);
//         ComplexQos(byDataValue, byIpPriorValue);
//     }
//     else
//     {
//         byAudioValue = (byAudioValue << 2);
//         byVideoValue = (byVideoValue << 2);
//         byDataValue = (byDataValue << 2);
//     }
// 
// //     s32 nRet = SetMediaTOS((s32)byAudioValue, QOS_AUDIO);
// //     nRet = SetMediaTOS((s32)byVideoValue, QOS_VIDEO);
// //     nRet = SetMediaTOS((s32)byDataValue, QOS_DATA);
// 
//     return;
}

/*=============================================================================
  函 数 名： ComplexQos
  功    能： 求复合Qos值
  算法实现： 
  全局变量： 
  参    数： u8& byValue
             u8 byPrior
  返 回 值： void 
=============================================================================*/
void CBASInst::ComplexQos(u8& byValue, u8 byPrior)
{
    u8 byLBit = ((byPrior & 0x08) >> 3);
    u8 byRBit = ((byPrior & 0x04) >> 1);
    u8 byTBit = ((byPrior & 0x02)<<1);
    u8 byDBit = ((byPrior & 0x01)<<3);
    
    byValue = byValue + ((byDBit + byTBit + byRBit + byLBit)*2);
    return;
}

/*=============================================================================
  函 数 名： ProcSetStyleMsg
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
=============================================================================*/
void CBASInst::ProcSetStyleMsg(CMessage* const pMsg)
{
    TVmpStyleCfgInfo  tStyleInfo;
    CServMsg cStyleInfo(pMsg->content, pMsg->length);

    cStyleInfo.GetMsgBody((u8*)&tStyleInfo, sizeof(TVmpStyleCfgInfo));
    BOOL32 bRet = SetVmpAttachStyle(tStyleInfo, m_tParam[0]);
    if(FALSE == bRet)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS,"[Error] set BAS attach style failed !\n");
        return;
    }

    // 确认成功后再保存，zgc，2009-07-09
    memcpy((u8*)&m_tStyleInfo, (void*)&tStyleInfo, sizeof(TVmpStyleCfgInfo));

    return;

}

/*=============================================================================
函 数 名： SetVmpAttachStyle
功    能： 
算法实现： 
全局变量： 
参    数： TVmpStyleCfgInfo&  tStyleInfo
           C8KEVMPParam & tParam
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/7/9   4.0			周广程                  创建
=============================================================================*/
BOOL32 CBASInst::SetVmpAttachStyle(TVmpStyleCfgInfo&  tStyleInfo, C8KEVMPParam & tParam )
{
    // 这里不保存，由调用该函数的地方在返回成功后再保存，zgc，2009-07-09
    //memcpy((u8*)&m_tStyleInfo, (void*)&tStyleInfo, sizeof(TVmpStyleCfgInfo));
    
    TMulPicColor tBGDColor;
    GetRGBColor(tStyleInfo.GetBackgroundColor(), tBGDColor.RColor,
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

    GetRGBColor(dwChairColor,    byRColorChair,    byGColorChair,    byBColorChair);
    GetRGBColor(dwSpeakerColor,  byRColorSpeaker,  byGColorSpeaker,  byBColorSpeaker);
    GetRGBColor(dwAudienceColor, byRColorAudience, byGColorAudience, byBColorAudience);

    TDrawFrameColor atSlidelineColor[PIC_MERGE_NUM_MAX];
    memset(atSlidelineColor, 0, sizeof(atSlidelineColor));

    for(u8 byLoop = 0; byLoop < tParam.m_byMemberNum; byLoop++)
    {
        if ( !tStyleInfo.GetIsRimEnabled() )
        {
            atSlidelineColor[byLoop].tMulPicColor.RColor = tBGDColor.RColor;
            atSlidelineColor[byLoop].tMulPicColor.GColor = tBGDColor.GColor;
            atSlidelineColor[byLoop].tMulPicColor.BColor = tBGDColor.BColor;
            atSlidelineColor[byLoop].bShow  = FALSE;

            continue;
        }

        atSlidelineColor[byLoop].bShow  = TRUE;
        if(MT_STATUS_CHAIRMAN == tParam.m_atMtMember[byLoop].GetMemStatus())
        {
            atSlidelineColor[byLoop].tMulPicColor.RColor = byRColorChair;
            atSlidelineColor[byLoop].tMulPicColor.GColor = byGColorChair;
            atSlidelineColor[byLoop].tMulPicColor.BColor = byBColorChair;
			LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"The channel: %d, The Chairman Color Value: %0x\n", byLoop, dwChairColor);
        }
        if(MT_STATUS_SPEAKER == tParam.m_atMtMember[byLoop].GetMemStatus())
        {
            atSlidelineColor[byLoop].tMulPicColor.RColor = byRColorSpeaker;
            atSlidelineColor[byLoop].tMulPicColor.GColor = byGColorSpeaker;
            atSlidelineColor[byLoop].tMulPicColor.BColor = byBColorSpeaker;
            LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"The channel: %d, The Speaker  Color Value: %0x\n", byLoop, dwSpeakerColor);
        }
        if(MT_STATUS_AUDIENCE == tParam.m_atMtMember[byLoop].GetMemStatus())
        {
            atSlidelineColor[byLoop].tMulPicColor.RColor = byRColorAudience;
            atSlidelineColor[byLoop].tMulPicColor.GColor = byGColorAudience;
            atSlidelineColor[byLoop].tMulPicColor.BColor = byBColorAudience;
            LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"The channel: %d, The Audience Color Value: %0x\n", byLoop, dwAudienceColor);
        }


    }
    LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"The Background Color Value: 0x%x\n", tStyleInfo.GetBackgroundColor() );
    LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"The RGB Color of Chairman : R<%d> G<%d> B<%d> \n", byRColorChair,    byGColorChair,    byBColorChair );
    LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"The RGB Color of Speaker  : R<%d> G<%d> B<%d> \n", byRColorSpeaker,  byGColorSpeaker,  byBColorSpeaker );
    LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"The RGB Color of Audience : R<%d> G<%d> B<%d> \n", byRColorAudience, byGColorAudience, byBColorAudience );
    LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"The RGB Color of Backgroud: R<%d> G<%d> B<%d> \n", tBGDColor.RColor, tBGDColor.GColor, tBGDColor.BColor );
	
    s32 nRet = 0;

	//画面合成风格加边框
    REC_LASTCODELINE;
    nRet = m_pcHardMulPic->SetBGDAndSidelineColor(tBGDColor, tBGDColor, atSlidelineColor);
    REC_LASTCODELINE;

    if(!nRet)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[BAS] Fail to call the SetBGDAndSidelineColor.\n");
        //return FALSE; //Always return TRUE
    }
    else
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"Set BAS Scheme:%d with RimEnabled:%d successfully !\n", tStyleInfo.GetSchemeId(), tStyleInfo.GetIsRimEnabled());
    }   	
    
	return TRUE;
}

/*=============================================================================
  函 数 名： GetRColor
  功    能： 取Red值
  算法实现： 
  全局变量： 
  参    数： u32 dwColorValue
  返 回 值： u8 
=============================================================================*/
u8 CBASInst::GetRColor(u32 dwColorValue)
{
    u32 dwRetValue = dwColorValue & 0x000000ff;
    return (u8)dwRetValue;
}

/*=============================================================================
  函 数 名： GetGColor
  功    能： 取Green值
  算法实现： 
  全局变量： 
  参    数： u32 dwColorValue
  返 回 值： u8 
=============================================================================*/
u8 CBASInst::GetGColor(u32 dwColorValue)
{
    u32 dwRetValue = ((dwColorValue & 0x0000ff00) >> 8);
    return (u8)dwRetValue;
}

/*=============================================================================
  函 数 名： GetBColor
  功    能： 取Blue值
  算法实现： 
  全局变量： 
  参    数： u32 dwColorValue
  返 回 值： u8 
=============================================================================*/
u8 CBASInst::GetBColor(u32 dwColorValue)
{
    u32 dwRetValue = ((dwColorValue & 0x00ff0000) >> 16);
    return (u8)dwRetValue;
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
void CBASInst::GetRGBColor(u32 dwColor, u8& byRColor, u8& byGColor, u8& byBColor)
{
    byRColor = (u8)dwColor & 0x000000ff;
    byGColor = (u8)((dwColor & 0x0000ff00) >> 8);
    byBColor = (u8)((dwColor & 0x00ff0000) >> 16);
    return;
}


/*=============================================================================
函 数 名： GetSendRtcpPort
功    能： 获取通道rtcp端口（主机序）
算法实现： 画面合成接收码流通道，mp侧RTCP端口分配(分配原则2...9, 因为0和1为MP发送端口)
全局变量： 
参    数： u8 byChanNo
返 回 值： u16 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/6/3  4.0			许世林                  创建
=============================================================================*/
u16 CBASInst::GetSendRtcpPort(u8 byChanNo)
{
    if (byChanNo >= 8)
    {
		OspPrintf(TRUE, FALSE, "[GetSendRtcpPort] Chn:%d's RtcpPort is :%d!\n", byChanNo, m_tSndAddr[0].wPort + 4 + byChanNo);
        return (m_tSndAddr[0].wPort + 4 + byChanNo); 
    }
    else
    {
		OspPrintf(TRUE, FALSE, "[GetSendRtcpPort] Chn:%d's RtcpPort is :%d!\n", byChanNo, m_tSndAddr[0].wPort + 2 + byChanNo);
        return (m_tSndAddr[0].wPort + 2 + byChanNo); 
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
void CBASInst::SetSmoothSendRule(u32 dwDestIp, u16 wPort, u32 dwRate)
{
    dwRate = dwRate >> 3;   // bit 2 byte
    
    u32 dwPeak = 0;

    dwPeak = dwRate + dwRate / 5; //add 20%, default
    LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[SetSmoothSendRule] Setting ssrule for 0x%x:%d, rate/peak=%u/%u KByte.\n",
              dwDestIp, wPort, dwRate, dwPeak );
/*
#ifdef _LINUX_
    s32 nRet = SetSSRule(dwDestIp, wPort, dwRate, dwPeak, 2); // default 2 seconds
    if ( 0 == nRet )
    {
        vmplog( "[SetSmoothSendRule] Set rule failed. SSErrno=%d\n", SSErrno );
    }
#endif
*/
}

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
void CBASInst::ClearSmoothSendRule(u32 dwDestIp, u16 wPort)
{
   LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[ClearSmoothSendRule] Unsetting ssrule for 0x%x:%d.\n",
            dwDestIp, wPort );

/*
#ifdef _LINUX_
    s32 nRet = UnsetSSRule(dwDestIp, wPort);
    if ( 0 == nRet )
    {
        vmplog( "[ClearSmoothSendRule] Unset rule failed. SSErrno=%d\n", SSErrno );
    }
#endif
*/
}

/*=============================================================================
函 数 名： StopSmoothSend
功    能： 停止平滑发送
算法实现： 
全局变量： SSErrno
参    数： 
返 回 值： void
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/03/12  4.0			顾振华                  创建
=============================================================================*/
void CBASInst::StopSmoothSend()
{
    LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[StopSmoothSend] Stopping SmoothSend\n" );

/*
#ifdef _LINUX_
    s32 nRet = CloseSmoothSending();
    if ( 0 == nRet )
    {
        vmplog( "[ClearSmoothSendRule] CloseSmoothSending failed. SSErrno=%d\n", SSErrno );
    }
#endif 
*/

}

/*=============================================================================
    函 数 名： GetVmpChlNumByStyle
    功    能： 根据画面合成风格获得合成通道数
    算法实现： 
    全局变量： 
    参    数： u8 byVMPStyle
    返 回 值： u8
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    04/03/31    3.0         胡昌威                创建
=============================================================================*/
u8 CBASInst::GetVmpChlNumByStyle( u8 byVMPStyle )
{
    u8   byMaxMemNum = 0;
    
    switch( byVMPStyle ) 
    {
    case VMP_STYLE_ONE:
        byMaxMemNum = 1;
        break;
    case VMP_STYLE_VTWO:
    case VMP_STYLE_HTWO:
        byMaxMemNum = 2;
        break;
    case VMP_STYLE_THREE:
        byMaxMemNum = 3;
        break;
    case VMP_STYLE_FOUR:
    case VMP_STYLE_SPECFOUR:
        byMaxMemNum = 4;
        break;
    case VMP_STYLE_SIX:
        byMaxMemNum = 6;
        break;
    case VMP_STYLE_EIGHT:
        byMaxMemNum = 8;
        break;
    case VMP_STYLE_NINE:
        byMaxMemNum = 9;
        break;
    case VMP_STYLE_TEN:
        byMaxMemNum = 10;
        break;
    case VMP_STYLE_THIRTEEN:
        byMaxMemNum = 13;
        break;
    case VMP_STYLE_SIXTEEN:
        byMaxMemNum = 16;
        break;       
    case VMP_STYLE_SEVEN:
        byMaxMemNum = 7;
        break;
    default:
        break;
    }
    
    return byMaxMemNum;
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
u8 CBASInst::VmpGetActivePayload(u8 byRealPayloadType)
{
    u8 byAPayload = MEDIA_TYPE_NULL;
    u8 byFECType  = FECTYPE_NONE;
    
    /*
    switch(byRealPayloadType)
    {
    case MEDIA_TYPE_PCMA:
    case MEDIA_TYPE_PCMU:
    case MEDIA_TYPE_G721:
    case MEDIA_TYPE_G722:
    case MEDIA_TYPE_G7231:
    case MEDIA_TYPE_G728:
    case MEDIA_TYPE_G729:
    case MEDIA_TYPE_MP3:
    case MEDIA_TYPE_G7221C:
    case MEDIA_TYPE_G7221:
    case MEDIA_TYPE_ADPCM:
        break;
        
    case MEDIA_TYPE_H261:
    case MEDIA_TYPE_H262:
    case MEDIA_TYPE_H263:
    case MEDIA_TYPE_MP4:
    case MEDIA_TYPE_H263PLUS:
    case MEDIA_TYPE_H264:

        byFECType = m_tCapSupportEx.GetVideoFECType();
        break;
        
    default:
        OspPrintf( TRUE, FALSE, "[GetActivePayload] unexpected media type.%d, ignore it\n", byRealPayloadType );
        break;
    }
    */
    if ( FECTYPE_NONE != byFECType )
    {
        byAPayload = MEDIA_TYPE_FEC;
    }
    else
    {
        switch(byRealPayloadType)
        {
        case MEDIA_TYPE_PCMA:   byAPayload = ACTIVE_TYPE_PCMA;  break;
        case MEDIA_TYPE_PCMU:   byAPayload = ACTIVE_TYPE_PCMU;  break;
        case MEDIA_TYPE_G721:   byAPayload = ACTIVE_TYPE_G721;  break;
        case MEDIA_TYPE_G722:   byAPayload = ACTIVE_TYPE_G722;  break;
        case MEDIA_TYPE_G7231:  byAPayload = ACTIVE_TYPE_G7231; break;
        case MEDIA_TYPE_G728:   byAPayload = ACTIVE_TYPE_G728;  break;
        case MEDIA_TYPE_G729:   byAPayload = ACTIVE_TYPE_G729;  break;
        case MEDIA_TYPE_H261:   byAPayload = ACTIVE_TYPE_H261;  break;
        case MEDIA_TYPE_H262:   byAPayload = ACTIVE_TYPE_H262;  break;
        case MEDIA_TYPE_H263:   byAPayload = ACTIVE_TYPE_H263;  break;
        //case MEDIA_TYPE_ADPCM:  byAPayload = bNewActivePT ? ACTIVE_TYPE_ADPCM : byRealPayloadType;  break;
        //case MEDIA_TYPE_G7221C: byAPayload = bNewActivePT ? ACTIVE_TYPE_G7221C : byRealPayloadType; break;
        default:                byAPayload = byRealPayloadType; break;
        }        
    }
    return byAPayload;
}

/*=============================================================================
函 数 名： IsSameEncParam
功    能： 
算法实现： 
全局变量： 
参    数：  TVidEncParam * ptEncParam1
           TVidEncParam * ptEncParam2
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/7/3   4.0			周广程                  创建
=============================================================================*/
BOOL32 CBASInst::IsSameEncParam( TVidEncParam * ptEncParam1, TVidEncParam * ptEncParam2 )
{
    if ( NULL == ptEncParam1 || NULL == ptEncParam2 )
    {
        LogPrint( LOG_LVL_ERROR, MID_MCU_BAS,"[IsSameEncParam] Param error! ptEncParam1 = 0x%x, ptEncParam2 = 0x%x!\n", ptEncParam1, ptEncParam2 );
        return FALSE;
    }
    if ( 0 == memcmp( ptEncParam1, ptEncParam2, sizeof(TVidEncParam) ) )
    {
        return TRUE;
    }
    return FALSE;
}

/*=============================================================================
函 数 名： Trans2EncParam
功    能： 将MCU发来的编码参数转换成VMP本地的结构
算法实现： 
全局变量： 
参    数： const C8KEVMPParam * ptVmpParam
           TVidEncParam * ptEncparam
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/8/18   4.0			周广程                  创建
=============================================================================*/
void CBASInst::Trans2EncParam( const C8KEVMPParam * ptVmpParam, TVidEncParam * ptEncparam,BOOL32 bIsSupportHp /*= FALSE*/)
{
    if ( NULL != ptVmpParam && NULL != ptEncparam )
    {
		ptEncparam->m_wEncVideoWidth = ptVmpParam->m_wVideoWidth;
		ptEncparam->m_wEncVideoHeight = ptVmpParam->m_wVideoHeight;
		ptEncparam->m_byEncType = ptVmpParam->m_byEncType;
		ptEncparam->m_dwBitRate = ptVmpParam->m_wBitRate;
		
		//取帧率 最大帧率为mcu设置编码的帧率
		//       最小帧率填实际占率的三分之二
		ptEncparam->m_byMaxFrameRate = ptVmpParam->m_byFrameRate;
		ptEncparam->m_byMinFrameRate = ptVmpParam->m_byFrameRate - (ptVmpParam->m_byFrameRate /3);
		
		TVidEncParam tTmpEncParam = *ptEncparam;
		m_tCfg.GetDefaultParam( ptEncparam->m_byEncType, tTmpEncParam ); 
		*ptEncparam = tTmpEncParam;
		if ( bIsSupportHp )
		{
			ptEncparam->m_byEncQuality = HPMULTE_PRIO;
		}
    }
	else
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_BAS,"[Trans2EncParam] Param error! ptVmpParam = 0x%x, ptEncParam = 0x%x!\n", ptVmpParam, ptEncparam );
	}
    
}
/*=============================================================================
函 数 名： GetEncChnIdxByParam
功    能： 获得正在以输入的编码参数进行编码的合成通道，如果没有则返回-1
算法实现： 
全局变量： 
参    数： TVidEncParam * ptEncParam
返 回 值： s8 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/8/18   4.0			周广程                  创建
=============================================================================*/
s8 CBASInst::GetEncChnIdxByParam( TVidEncParam * ptEncParam )
{
    if ( NULL == ptEncParam )
    {
        LogPrint( LOG_LVL_ERROR, MID_MCU_BAS,"[GetEncChnIdxByParam] Param is error! ptEncParam = 0x%x!\n", ptEncParam );
        return -1;
    }
    // 获得当前合成器实际状态
    TMulPicStatus tMulPicStatus;
    if ( !m_pcHardMulPic->GetStatus(tMulPicStatus) )
    {
        LogPrint( LOG_LVL_ERROR, MID_MCU_BAS,"[GetEncChnIdxByParam] Get vmp current MulPicStatus failed!\n" );
        return -1;
    }

    // 因为底层实际设置时会减掉部分码率，因此比较的时候要对上层码率进行修正
    TVidEncParam tTmpEncParam = *ptEncParam;
    tTmpEncParam.m_dwBitRate = m_pcHardMulPic->RecalculateBitRate( tTmpEncParam.m_dwBitRate );

    u8 byEncChnIdx = 0;
    for ( byEncChnIdx = 0; byEncChnIdx < tMulPicStatus.byEncNum; byEncChnIdx++ )
    {
        if ( IsSameEncParam( &tTmpEncParam, &tMulPicStatus.atVidEncParam[byEncChnIdx] ) )
        {
            return byEncChnIdx;
        }
    }
    return -1;
}

CBASCfg::CBASCfg()
{
    m_dwMcuNode = INVALID_NODE;   
    m_dwMcuIId  = INVALID_INS;
    m_bEmbed  = FALSE;
    m_byRegAckNum = 0;
}

CBASCfg::~CBASCfg()
{

}

/*=============================================================================
  函 数 名： FreeStatusData
  功    能： 清空状态信息
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CBASCfg::FreeStatusData(void)
{
    m_dwMcuNode = INVALID_NODE;   
    m_dwMcuIId  = INVALID_INS;
}


void CBASInst::SendChnNotif()
{

}

void CBASCfg::ConvertToRealFR(THDAdaptParam& tAdptParm)
{
	if (tAdptParm.GetVidType() != MEDIA_TYPE_H264)
	{
		u8 byFrameRate = tAdptParm.GetFrameRate();
		switch(byFrameRate)
		{
		case VIDEO_FPS_2997_1:
			byFrameRate = 30;
			break;
		case VIDEO_FPS_25:
			byFrameRate = 25;
			break;
		case VIDEO_FPS_2997_2:
			byFrameRate = 15;
			break;
		case VIDEO_FPS_2997_3:
			byFrameRate = 10;
			break;
		case VIDEO_FPS_2997_4:
		case VIDEO_FPS_2997_5:
			byFrameRate = 6;
			break;
		case VIDEO_FPS_2997_6:
			byFrameRate = 5;
			break;
		case VIDEO_FPS_2997_30:
			byFrameRate = 1;
			break;
		case VIDEO_FPS_2997_7P5:
			byFrameRate = 4;
			break;
		case VIDEO_FPS_2997_10:
			byFrameRate = 3;
			break;
		case VIDEO_FPS_2997_15:
			byFrameRate = 2;
			break;
		default:
			LogPrint( LOG_LVL_ERROR, MID_MCU_BAS,"[ConvertToRealFR] undef non264 framerate%d\n", byFrameRate);
			break;
		}
		
		tAdptParm.SetFrameRate(byFrameRate);
	}
}
//END OF FILE

