/*****************************************************************************
   模块名      : 画面复合器(Video Multiplexer)实现
   文件名      : vmpInst.cpp
   创建时间    : 2003年 12月 4日
   实现功能    : include : /include/VMPInst.h
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
#include "vmpinst.h"
//#include "eqpcfg.h"
#include "mcuver.h"
/*lint -save -e537*/
#include "commonlib.h"
/*lint -restore*/
//#include "eqpgvari.h"

extern CMulPic  * g_pcHardMulPic;                              //画面复合器的封装

s32 g_nVmplog = 0;
u16 g_wVmpLastEvent = 0;
u32 g_dwVmpLastFileLine = 0;
extern TResBrLimitTable g_tResBrLimitTable;//分辨率码率限制信息表,在mmp里面定义

//extern CLogManager g_cVmpLogMgr;
// 
// extern "C" s8* strofip(u32 dwIp, BOOL32 bBONet);
// extern "C" LPCSTR GetMediaString( u8 byMediaType );
// extern "C" void GetWHViaRes(u8 byRes, u16 &wWidth, u16 &wHeight);
// extern "C" u8 GetResViaWH( u16 wWidth, u16 wHeight );

#define REC_LASTCODELINE (g_dwVmpLastFileLine = __LINE__)

/*lint -save -esym(530, argptr)*/
/*lint -save -esym(526, __builtin_va_start)*/
/*lint -save -esym(628, __builtin_va_start)*/
void vmplog( s8* pszFmt, ...)
{
    s8 achPrintBuf[255];
    s32  nBufLen = 0;
    va_list argptr;
	if( g_nVmplog )
	{		  
		nBufLen = sprintf( achPrintBuf, "[vmp]: " ); 
		va_start( argptr, pszFmt );
		nBufLen += vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
		va_end(argptr); 
		OspPrintf( TRUE, FALSE, achPrintBuf ); 
	}
    return;
}
/*lint -restore*/

const char* VmpHardStyleStr(u32 dwHardStyle)
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
        OspPrintf( TRUE, FALSE, "[VmpHardStyleStr] invalid style.%d\n", dwHardStyle);
        pszStr = "Unknown";    // 默认两画面：左右分
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP, "[VmpHardStyleStr] invalid style.%d\n", dwHardStyle);
        break;
    }
    return pszStr;
}

CVMPInst::CVMPInst()
{
    m_pcHardMulPic = NULL;
    m_bDbVid = FALSE;
	//m_bIsIframeFirstReq = FALSE;
    m_dwLastFUPTick = 0;
    memset( &m_tPrsTimeSpan, 0, sizeof(TPrsTimeSpan) );
    for (u8 byChnlNo = 0; byChnlNo < MAXNUM_SDVMP_MEMBER; byChnlNo++)
    {
        m_abChnlValid[byChnlNo] = FALSE;
		m_abChnlSetBmp[byChnlNo] = FALSE;
    }

	memset(m_abyChanProfileType,(u8)emBpAttrb,MAXNUM_VMP_ENCNUM);

	
}
/*lint -save -e1551*/
CVMPInst::~CVMPInst()
{
    if ( NULL != m_pcHardMulPic )
    {
        m_pcHardMulPic->Quit();    
        delete m_pcHardMulPic;
    }
    
}
/*lint -restore*/
/*lint -save -e715*/
void CVMPInst::DaemonInstanceEntry( CMessage* const pcMsg, CApp* pcApp )
{
    return;
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
void CVMPInst::InstanceEntry(CMessage *const pMsg)
{
    if (NULL == pMsg)
    {
        return;
    }

	if ( pMsg->event >=EV_VMP_NEEDIFRAME_TIMER && (pMsg->event< EV_VMP_NEEDIFRAME_TIMER+MAXNUM_SDVMP_MEMBER) )
    {
		MsgTimerNeedIFrameProc(pMsg);
		return;
	}
	else
	{
		LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP, "Message %u(%s) received in InstanceEntry\n", pMsg->event, ::OspEventDesc(pMsg->event));
        vmplog("Message %u(%s) received in InstanceEntry\n", pMsg->event, ::OspEventDesc(pMsg->event));
        // g_cVmpLogMgr.InsertLogItem( "Message %u(%s) received in InstanceEntry\n", pMsg->event, ::OspEventDesc(pMsg->event) );
    }
    switch(pMsg->event)
    {
    //--------内部消息------------
    // 断链处理
    case OSP_DISCONNECT:
        Disconnect(pMsg);
        break;
	// 连接
    case EV_VMP_CONNECT_TIMER:
        ProcConnectTimeOut();
        break;
	// 注册
    case EV_VMP_REGISTER_TIMER:
        ProcRegisterTimeOut();
        break;
    case TIME_ERRPRINT:
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP, "Vmp CMulPic::Initialize failed!\n");
        OspPrintf( TRUE, FALSE, "Vmp CMulPic::Initialize failed!\n");
        SetTimer(TIME_ERRPRINT, 15000);
        break;
    case TIME_VMP_FLASHLOG:
        ProcVmpFlashLogTimeOut();
        break;
	
    //--------外部消息------------
    // 启动初始化
    case OSP_POWERON:
        Init(pMsg);
        break;

    // 显示所有的状态和统计信息
    case EV_VMP_DISPLAYALL:
        StatusShow();
        break;

	case EV_VMP_SHOWPARAM:
		ParamShow();
		break;

    // 注册应答消息
    case MCU_VMP_REGISTER_ACK:
        g_wVmpLastEvent = pMsg->event;
        MsgRegAckProc(pMsg);
        break;

    // 注册拒绝消息
    case MCU_VMP_REGISTER_NACK:
        g_wVmpLastEvent = pMsg->event;
        MsgRegNackProc(pMsg);
        break;

        // 开始/参数改变
    case MCU_VMP_STARTVIDMIX_REQ:
    case MCU_VMP_CHANGEVIDMIXPARAM_CMD:
        g_wVmpLastEvent = pMsg->event;
        MsgChangeVidMixParamProc(pMsg);
        break;

    // 停止复合
    case MCU_VMP_STOPVIDMIX_REQ:
        g_wVmpLastEvent = pMsg->event;
        MsgStopVidMixProc(pMsg);
        break;

    // 查询参数
    case MCU_VMP_GETVIDMIXPARAM_REQ:
        g_wVmpLastEvent = pMsg->event;
        MsgGetVidMixParamProc(pMsg);
        break;

    // 设置通道码率
    case MCU_VMP_SETCHANNELBITRATE_REQ:
        g_wVmpLastEvent = pMsg->event;
        MsgSetBitRate(pMsg);
        break;

    // 设置Qos值
    case MCU_EQP_SETQOS_CMD:
        g_wVmpLastEvent = pMsg->event;
        ProcSetQosInfo(pMsg);
        break;

    // 更改编码参数
    case MCU_VMP_UPDATAVMPENCRYPTPARAM_REQ:
        g_wVmpLastEvent = pMsg->event;
        MsgUpdataVmpEncryptParamProc(pMsg);
        break;

    // 强制关键帧
    case MCU_VMP_FASTUPDATEPIC_CMD:
        g_wVmpLastEvent = pMsg->event;
        MsgFastUpdatePicProc(pMsg);
        break;

    case MCU_VMP_CHNNLLOGOPR_CMD:
        MsgChnnlLogOprCmd( pMsg );
        break;

    // 修改复合附属风格(边框、背景)
    case MCU_VMP_SETSTYLE_CMD:
        g_wVmpLastEvent = pMsg->event;
        ProcSetStyleMsg(pMsg);
        break;
	
	//调整合成成员别名
	case MCU_VMP_CHANGEMEMALIAS_CMD:
		ProcChangeMemAliasCmd(pMsg);
		break;

	case MCU_VMP_STARTSTOPCHNNL_CMD:
		ProcStartStopChnnlCmd(pMsg);
		break;
	case MCU_VMP_ADDREMOVECHNNL_CMD:
		MsgAddRemoveRcvChnnl( pMsg );
		break;
//	case MCU_VMP_STARTSTOPALLCHNNL_CMD:
//		ProcStartStopAllChnnlCmd(pMsg);
//		break;
//	case MCU_VMP_CHANGESINGLECHNNL_ENCPARAM_REQ:
//		ProcChangeSingleChnlEncParam(pMsg);
//		break;
    // 未定义的消息
    default:
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP, "[CVMPInst]Unexcpet Message %u(%s).\n", pMsg->event, ::OspEventDesc(pMsg->event));
        OspPrintf(TRUE, FALSE, "[CVMPInst]Unexcpet Message %u(%s).\n", pMsg->event, ::OspEventDesc(pMsg->event));
        break;
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
void CVMPInst::ProcVmpFlashLogTimeOut( void )
{
    KillTimer( TIME_VMP_FLASHLOG );
//     g_cVmpLogMgr.WriteLogInfo();
//     g_cVmpLogMgr.ClearLogTable();
    SetTimer( TIME_VMP_FLASHLOG, VMP_FLASHLOG_TIMEOUT );

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
u32 CVMPInst::ConvertVcStyle2HardStyle(u8 byOldstyle)
{
    u32 dwStyle;
    switch ( byOldstyle )
    {
    case VMP_STYLE_ONE:  // 一画面
        dwStyle = (u32)MERGE_M1;  // 单画面
        break;

    case VMP_STYLE_VTWO: // 两画面：左右分
        dwStyle = (u32)MERGE_M2;  // 两画面：左右分
        break;

    case VMP_STYLE_HTWO:  // 两画面: 一大一小
        dwStyle = (u32)MERGE_M2_1_BR1; // 两画面： 一大一小（大画面分成9份发送）
        break;

    case VMP_STYLE_THREE: // 三画面
        dwStyle = (u32)MERGE_M3_T1; // 三画面
        break;

    case VMP_STYLE_FOUR: // 四画面
        dwStyle = (u32)MERGE_M4;  // 四画面
        break;

    case VMP_STYLE_SIX: //六画面
        dwStyle = (u32)MERGE_M6_1_5; //六画面
        break;

    case VMP_STYLE_EIGHT: //八画面
        dwStyle = (u32)MERGE_M8_1_7; //八画面
        break;
    case VMP_STYLE_NINE: //九画面
        dwStyle = (u32)MERGE_M9; //九画面
        break;

    case VMP_STYLE_TEN: //十画面
        dwStyle = (u32)MERGE_M10_2_R8; //十画面：二大八小
        break;

     case VMP_STYLE_TEN_H:   //特殊十画面，上二下八
         dwStyle = (u32)MERGE_M10_2_B8;
         break;

    case VMP_STYLE_THIRTEEN:  //十三画面
        dwStyle = (u32)MERGE_M13_TL1_12; //十三画面
        break;

    case VMP_STYLE_SIXTEEN: //十六画面
        dwStyle = (u32)MERGE_M16; //十六画面
        break;
        
    case VMP_STYLE_SPECFOUR://特殊四画面 
        dwStyle = (u32)MERGE_M4_1_R3; //特殊四画面：一大三小（大画面分成4份发送）
        break;

    case VMP_STYLE_SEVEN: //七画面
        dwStyle = (u32)MERGE_M7_3_BR4; //七画面：三大四小（大画面分成4份发送）
        break;

    case VMP_STYLE_SIX_L2UP_S4DOWN: // 特殊六画面（上2下4）
        dwStyle = (u32)MERGE_M6_2_B4;
        break;

    case VMP_STYLE_FOURTEEN:    //十四画面合成（2大12小, 2大在上左）
        dwStyle = (u32)MERGE_M14_TL2_12;
        break;

    case VMP_STYLE_SIX_DIVIDE:  //六画面（三三等分）
        dwStyle = (u32)MERGE_M6;
        break;

    case VMP_STYLE_TEN_M:       //十画面（两个大画面居中）
        dwStyle = (u32)MERGE_M10_2_TB8;
        break;

    case VMP_STYLE_THIRTEEN_M:  //十三画面（大画面居中）
        dwStyle = (u32)MERGE_M13_1_ROUND12;
        break;

    case VMP_STYLE_FIFTEEN:     //十五画面
        dwStyle = (u32)MERGE_M15_T3_12;
        break;

    default:
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP, "[ConvertVcStyle2HardStyle] invalid style.%d\n", byOldstyle);
        OspPrintf( TRUE, FALSE, "[ConvertVcStyle2HardStyle] invalid style.%d\n", byOldstyle);
        dwStyle = (u32)MERGE_M16;    // 默认两画面：左右分
        break;
    }
    return dwStyle;
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
void CVMPInst::Init(CMessage *const pMsg)
{
    //从消息中得到信息
    if (NULL == pMsg->content)
    {
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP, "[CVMPInst]Init params cannot be NULL!\n");
        OspPrintf(TRUE, FALSE, "[CVMPInst]Init params cannot be NULL!\n");
        return;
    }

    
    TMulPicParam tInitParam;
    tInitParam.dwTotalMapNum = 16;
    tInitParam.dwType = (u32)MERGE_M16;
    
    m_pcHardMulPic = new CMulPic;
    if ( NULL == m_pcHardMulPic)
    {
        g_cVMPApp.m_bInitOk = FALSE;
        SetTimer(TIME_ERRPRINT, 1000);  //timely print err
        return;
    }

    BOOL32 bInitOk = m_pcHardMulPic->Initialize( tInitParam  );
	LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP, "[CVMPInst::Init] m_pcHardMulPic->Initialize<tInitParam.dwTotalMapNum:%d, tInitParam.dwType:%d> ",tInitParam.dwTotalMapNum, tInitParam.dwType);
	OspPrintf(TRUE, FALSE, "[CVMPInst::Init] m_pcHardMulPic->Initialize<tInitParam.dwTotalMapNum:%d, tInitParam.dwType:%d> ",tInitParam.dwTotalMapNum, tInitParam.dwType);
    if (!bInitOk)
    {
        g_cVMPApp.m_bInitOk = FALSE;
        SetTimer(TIME_ERRPRINT, 1000);  //timely print err
        return;
    }

    // 用全局变量记录, zgc, 2009-11-03
    g_pcHardMulPic = m_pcHardMulPic;

    g_cVMPApp.m_bInitOk = TRUE;

    T8keVmpCfg tVmpCfg;
    memset(&tVmpCfg, 0, sizeof(T8keVmpCfg));

    memcpy(&tVmpCfg, pMsg->content, sizeof(T8keVmpCfg));
	
    m_bDbVid = TRUE;// 编4路码流

	// Mcu信息
    g_cVMPApp.m_tEqpCfg.SetConnectIp( tVmpCfg.GetConnectIp() );
    g_cVMPApp.m_tEqpCfg.SetConnectPort( tVmpCfg.GetConnectPort() );
    g_cVMPApp.m_tEqpCfg.SetEqpType( tVmpCfg.GetEqpType() );
    g_cVMPApp.m_tEqpCfg.SetEqpId( tVmpCfg.GetEqpId() );
    g_cVMPApp.m_tEqpCfg.SetMcuId( tVmpCfg.GetMcuId() );
    g_cVMPApp.m_tEqpCfg.SetAlias( tVmpCfg.GetAlias() );
	g_cVMPApp.m_dwIframeInterval = tVmpCfg.GetIframeInterval();

	LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"Init %s Success!\n", g_cVMPApp.m_tEqpCfg.GetAlias());
    vmplog( "Init %s Success!\n", g_cVMPApp.m_tEqpCfg.GetAlias() );

    if( g_cVMPApp.m_tEqpCfg.IsEmbed() )
    {   
        g_cVMPApp.m_dwMcuNode = 0;
        SetTimer(EV_VMP_REGISTER_TIMER, VMP_REGISTER_TIMEOUT); 
    }
    else if( 0 != g_cVMPApp.m_tEqpCfg.GetConnectIp() ) // 连接Mcu
    {
        SetTimer(EV_VMP_CONNECT_TIMER, VMP_CONNETC_TIMEOUT);
    }


    m_tDbgVal.ReadDebugValues();
	m_pcHardMulPic->SetResizeMode( m_tDbgVal.GetVidEachResizeMode() );
	LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"[Init] SetResizeMode: %d\n", m_tDbgVal.GetVidEachResizeMode());
	printf("[Init] SetResizeMode: %d\n", m_tDbgVal.GetVidEachResizeMode());

    //  [6/9/2013 liaokang]
    m_cBmpDrawer.LoadFont("/opt/mcu/kdvuni.ttf");

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
void CVMPInst::ProcConnectTimeOut()
{
    KillTimer( EV_VMP_CONNECT_TIMER );
    switch( CurState() )
    {
    case IDLE:
        {
            BOOL32 bRet = FALSE;
            bRet = ConnectMcu(g_cVMPApp.m_dwMcuNode);
            if( bRet )
            { 
                SetTimer(EV_VMP_REGISTER_TIMER, VMP_REGISTER_TIMEOUT); 
            }
            else
            {
                SetTimer(EV_VMP_CONNECT_TIMER, VMP_CONNETC_TIMEOUT);
            }
        }
        break;
    default:
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[CVMPInst::ProcConnectTimeOut]: EV_VMP_CONNECT_TIMER received in wrong state %u!\n", CurState());
        OspPrintf(TRUE, FALSE, "[CVMPInst::ProcConnectTimeOut]: EV_VMP_CONNECT_TIMER received in wrong state %u!\n", CurState());
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
BOOL32 CVMPInst::ConnectMcu( u32& dwMcuNode )
{
    BOOL32 bRet = TRUE;

    if( !OspIsValidTcpNode(dwMcuNode))
    {
        dwMcuNode = OspConnectTcpNode(htonl(g_cVMPApp.m_tEqpCfg.GetConnectIp()), g_cVMPApp.m_tEqpCfg.GetConnectPort(), 10, 0, 100 );  

        if (OspIsValidTcpNode(dwMcuNode))
        {
            ::OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), GetInsID());
			LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"[Info] Connect to Mcu Success,node is %u!\n", dwMcuNode);
            vmplog("[Info] Connect to Mcu Success,node is %u!\n", dwMcuNode);
            // 记录本地IP
            g_cVMPApp.m_tEqpCfg.SetEqpIp( ntohl( OspNodeLocalIpGet(dwMcuNode) ) );
        }
        else
        {
            //建链失败
			LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[vmp] Failed to Connect Mcu %s:%d>\n", 
                                    strofip( g_cVMPApp.m_tEqpCfg.GetConnectIp(), FALSE ), g_cVMPApp.m_tEqpCfg.GetConnectPort() );
            OspPrintf(TRUE, FALSE, "[vmp] Failed to Connect Mcu %s:%d>\n", 
                                    strofip( g_cVMPApp.m_tEqpCfg.GetConnectIp(), FALSE ), g_cVMPApp.m_tEqpCfg.GetConnectPort() );
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
void CVMPInst::ProcRegisterTimeOut()
{
    KillTimer( EV_VMP_REGISTER_TIMER );
    switch( CurState() )
    {
    case IDLE:
        Register( g_cVMPApp.m_dwMcuNode );
        SetTimer(EV_VMP_REGISTER_TIMER, VMP_REGISTER_TIMEOUT);
        break;
    default:
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[CVMPInst::ProcRegisterTimeOut]: EV_VMP_REGISTER_TIMER received in wrong state %u!\n", CurState());
        OspPrintf(TRUE, FALSE, "[CVMPInst::ProcRegisterTimeOut]: EV_VMP_REGISTER_TIMER received in wrong state %u!\n", CurState());
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
	日  期      版本        修改人        修改内容
   03/12/4	   v1.0	      zhangsh	    create
====================================================================*/
void CVMPInst::Register(u32 dwMcuNode)
{
    CServMsg cSvrMsg;
    TEqpRegReq tReg;
    memset(&tReg, 0, sizeof(tReg));
    

    tReg.SetEqpIpAddr( g_cVMPApp.m_tEqpCfg.GetEqpIp() );
    tReg.SetEqpType(g_cVMPApp.m_tEqpCfg.GetEqpType());
    tReg.SetEqpAlias(g_cVMPApp.m_tEqpCfg.GetAlias());
    tReg.SetVersion(DEVVER_VMP);
    tReg.SetMcuId( (u8)g_cVMPApp.m_tEqpCfg.GetMcuId() );
        
    cSvrMsg.SetMsgBody((u8*)&tReg, sizeof(tReg));
      
      /*u16 wRet = */post( MAKEIID( AID_MCU_PERIEQPSSN, CInstance::DAEMON ), VMP_MCU_REGISTER_REQ,
             cSvrMsg.GetServMsg(), cSvrMsg.GetServMsgLen(), dwMcuNode );

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
   03/12/4	   v1.0	         zhangsh	    create
   2010/2/8    8ke 1.0       周嘉麟         修改     
====================================================================*/
void CVMPInst::MsgRegAckProc(CMessage* const pMsg)
{    
    vmplog("[MsgRegAckProc] Register OK!\n");
	LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"[MsgRegAckProc] Register OK!\n");
    if (NULL == pMsg->content)
    {
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[MsgRegAckProc] The pointer cannot be Null. (MsgRegAckProc)\n");
        OspPrintf(TRUE, FALSE, "[MsgRegAckProc] The pointer cannot be Null. (MsgRegAckProc)\n");
        return;
    }
    
    if ( IDLE != CurState() )
    {
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[MsgRegAckProc] Current state is not IDLE! Error!\n");
        OspPrintf( TRUE, FALSE, "[MsgRegAckProc] Current state is not IDLE! Error!\n" );
        return;
    }

    NEXTSTATE(NORMAL);

    CServMsg cServMsg(pMsg->content, pMsg->length);
    TEqpRegAck tRegAck = *(TEqpRegAck*)cServMsg.GetMsgBody();

    // 取参数
    g_cVMPApp.m_dwMcuNode = pMsg->srcnode;
    g_cVMPApp.m_dwMcuIId = pMsg->srcid; 
    KillTimer(EV_VMP_REGISTER_TIMER);
    vmplog("register to mcu.A succeed !\n");

    g_cVMPApp.m_tEqpCfg.SetEqpId( tRegAck.GetEqpId() );
    g_cVMPApp.m_tEqpCfg.SetMcuStartPort( tRegAck.GetMcuStartPort() );
    g_cVMPApp.m_tEqpCfg.SetEqpStartPort( tRegAck.GetEqpStartPort() );
    if ( 0 == pMsg->srcnode )
    {
        g_cVMPApp.m_tEqpCfg.SetEqpIp( ntohl( INET_ADDR("127.0.0.1") ) );
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[MsgRegAckProc] pMsg->srcnode = 0!\n");
		OspPrintf(TRUE, FALSE, "[MsgRegAckProc] pMsg->srcnode = 0!\n");
    }
    else
    {
        g_cVMPApp.m_tEqpCfg.SetEqpIp( ntohl( OspNodeLocalIpGet(pMsg->srcnode) ) );
    }

    // mcu侧端口
	u8 byIdx = 0;
	for ( ; byIdx < MAXNUM_VMP_ENCNUM; byIdx++)
	{
		m_tSndAddr[byIdx].dwIp  = htonl(tRegAck.GetMcuIpAddr());  
		m_tSndAddr[byIdx].wPort = tRegAck.GetMcuStartPort() + byIdx*PORTSPAN;
		LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[Info]  Channel<Id:%d> Remote Send IP:%s:%u\n", 
						byIdx, strofip(m_tSndAddr[byIdx].dwIp, TRUE), m_tSndAddr[byIdx].wPort);
		vmplog("[Info]  Channel<Id:%d> Remote Send IP:%s:%u\n", 
						byIdx, strofip(m_tSndAddr[byIdx].dwIp, TRUE), m_tSndAddr[byIdx].wPort);
	}
	byIdx = 0;	
	for ( ; byIdx < MAXNUM_VMP_ENCNUM; byIdx++)
	{
		m_tSndLocalAddr[byIdx].dwIp = 0;
		m_tSndLocalAddr[byIdx].wPort = VMP_8KEOR8KH_LOCALSND_PORT + byIdx*PORTSPAN/*tRegAck.GetEqpStartPort() + (RESV_MCU_STARTPORT + byIdx * PORTSPAN)*//*- (byIdx+1)*PORTSPAN*/;
		LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[Info]  Channel<Id:%d> Local Send IP:%s:%u\n", 
						byIdx, strofip(m_tSndLocalAddr[byIdx].dwIp, TRUE), m_tSndLocalAddr[byIdx].wPort);
		vmplog("[Info]  Channel<Id:%d> Local Send IP:%s:%u\n", 
						byIdx, strofip(m_tSndLocalAddr[byIdx].dwIp, TRUE), m_tSndLocalAddr[byIdx].wPort);
	}
/*
	m_tSndAddr[0].dwIp  = htonl(tRegAck.GetMcuIpAddr());  
    m_tSndAddr[0].wPort = tRegAck.GetMcuStartPort();
    m_tSndAddr[1].dwIp  = htonl(tRegAck.GetMcuIpAddr());  
    m_tSndAddr[1].wPort = tRegAck.GetMcuStartPort()+ PORTSPAN;

    m_tSndLocalAddr[0].dwIp = 0;
    m_tSndLocalAddr[0].wPort = tRegAck.GetEqpStartPort() - PORTSPAN;
    m_tSndLocalAddr[1].dwIp = 0;
    m_tSndLocalAddr[1].wPort = tRegAck.GetEqpStartPort() - 2*PORTSPAN;

    vmplog("[Info] Get Send IP and Port from MCU are Follow:\n");
	byIdx = 0;	
    vmplog("[Info] First Channel Send IP:%s:%u\n",  strofip(m_tSndAddr[0].dwIp, TRUE), m_tSndAddr[0].wPort);
    vmplog("[Info] Second Channel Send IP:%s:%u\n", strofip(m_tSndAddr[1].dwIp, TRUE), m_tSndAddr[1].wPort);
*/	

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


    SendStatusChangeMsg(TRUE, (u8)TVmpStatus::IDLE);     
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
void CVMPInst::MsgRegNackProc(CMessage* const pMsg)
{

    OspPrintf(TRUE, FALSE, "[Info] Register to MCU be refused .retry...\n");
	LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[Info] Register to MCU be refused .retry...\n");

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
BOOL CVMPInst::SendMsgToMcu(u16 wEvent, CServMsg* const pcServMsg)
{
	LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[Send] Message %u(%s).\n", wEvent, ::OspEventDesc(wEvent));
    vmplog("[Send] Message %u(%s).\n", wEvent, ::OspEventDesc(wEvent));
//     g_cVmpLogMgr.InsertLogItem( "[Send] Message %u(%s).\n", wEvent, ::OspEventDesc(wEvent) );

    if(g_cVMPApp.m_bEmbed || OspIsValidTcpNode(g_cVMPApp.m_dwMcuNode))
    {
        post(g_cVMPApp.m_dwMcuIId, wEvent, pcServMsg->GetServMsg(), pcServMsg->GetServMsgLen(), g_cVMPApp.m_dwMcuNode);
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
   03/12/4	   v1.0	      zhangsh	    create
   09/03/06    8000E v1.0	顾振华	      create
====================================================================*/
void CVMPInst::Disconnect(CMessage *const pMsg )
{
//    s32 nRet = 0;
    u32 dwDisNode = *(u32 * )pMsg->content;
    
    if (INVALID_NODE != dwDisNode)
    {
        OspDisconnectTcpNode(dwDisNode);
    }

    if( dwDisNode == g_cVMPApp.m_dwMcuNode )
    {
        g_cVMPApp.FreeStatusData();
        if( 0 != g_cVMPApp.m_tEqpCfg.GetConnectIp() ) // 连接Mcu
        {
            SetTimer(EV_VMP_CONNECT_TIMER, VMP_CONNETC_TIMEOUT);
        }
        else
        {
            SetTimer(EV_VMP_REGISTER_TIMER, VMP_REGISTER_TIMEOUT); 
        }
        LogPrint(LOG_LVL_WARNING,MID_MCU_VMP,"[Vmp] Disconnect with Mcu: %d\n", g_cVMPApp.m_dwMcuNode);
        vmplog("[Vmp] Disconnect with Mcu: %d\n", g_cVMPApp.m_dwMcuNode);
    }
	
    ClearCurrentInst();

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
void CVMPInst::ClearCurrentInst(void)
{
	switch(CurState())//移通道
    {
    //case NORMAL:
    case RUNNING:
        {
            s32 nRet = FALSE;
            if (NULL != m_pcHardMulPic)
            {
                nRet = m_pcHardMulPic->StopMerge();
            }
            for (u8 byChNum = 0;byChNum<MAXNUM_SDVMP_MEMBER;byChNum++)
			{
				if (m_abChnlValid[byChNum] == TRUE)
				{
					KillTimer(EV_VMP_NEEDIFRAME_TIMER+byChNum);
				}
			}
			LogPrint(LOG_LVL_WARNING,MID_MCU_VMP,"[VMP Stop]:Stop work, the return code: %d!\n", nRet);
            vmplog("[VMP Stop]:Stop work, the return code: %d!\n", nRet);
        }
        break;
	default:
		LogPrint(LOG_LVL_WARNING,MID_MCU_VMP,"[VMP Stop]:wrong state,curState:%d!\n", CurState());
		break;
    }

    NEXTSTATE(IDLE);
    g_cVMPApp.m_byRegAckNum = 0;
	memset( m_abChnlValid, 0, sizeof(m_abChnlValid) );
	memset( m_abChnlIframeReqed, 0, sizeof(m_abChnlIframeReqed) );
	memset(m_abChnlSetBmp,0,sizeof(m_abChnlSetBmp));
    memset(m_tVmpMbAlias, 0, sizeof(m_tVmpMbAlias));
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
void CVMPInst::MsgChangeVidMixParamProc(CMessage* const pMsg)
{
    if (NULL == pMsg->content)
    {
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[MsgChangeVidMixParamProc] The pointer cannot be Null\n");
        OspPrintf(TRUE, FALSE, "[MsgChangeVidMixParamProc] The pointer cannot be Null\n");
        return;
    }
    
    switch(CurState())
    {
    case IDLE:        
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[MsgChangeVidMixParamProc] Cannot merge while in state:%d\n", CurState());
        OspPrintf(TRUE, FALSE, "[MsgChangeVidMixParamProc] Cannot merge while in state:%d\n", CurState());
        return ;
    default:
        break;
    }
    
    CServMsg cServMsg(pMsg->content, pMsg->length);
    cServMsg.SetEventId(pMsg->event);    
    g_dwVmpLastFileLine = __LINE__;
    ChangeCP(cServMsg);
    g_dwVmpLastFileLine = __LINE__;
    
    return;
}

/*lint -save -e438 */
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
void CVMPInst::ChangeCP(CServMsg& cServMsg)
{
    // 由于需要先修改状态，所以这里把当前状态记录下来
    u32 byCurState = CurState();
    if ( NORMAL == CurState() )
    {
        NEXTSTATE(RUNNING);
    }
	if ( NULL == m_pcHardMulPic )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"m_pcHardMulPic is NULL\n");
		vmplog("m_pcHardMulPic is NULL\n");
		return;
	}
    u8 byChnIdx = 0;
	BOOL32 bStart = (cServMsg.GetEventId() == MCU_VMP_STARTVIDMIX_REQ);

	BOOL32 bMsgBufIsOK = TRUE;
    //1、 取MCU参数
    u8 * pbyMsgBuf = cServMsg.GetMsgBody();
	u16 wMsgBufLen =0;//当前读到的消息体位置

    C8KEVMPParam atParam[MAXNUM_VMP_ENCNUM];
    memcpy((u8*)atParam, pbyMsgBuf, sizeof(atParam));
    pbyMsgBuf += sizeof(atParam);
	wMsgBufLen += sizeof(atParam);
	
	// 定义最大编码路数
	const u8 byMaxVmpEncNum = sizeof(atParam)/sizeof(atParam[0]);
	// 定义最大合成成员个数
	const u8 byMaxVmpChlMemNum = sizeof(atParam[0].m_atMtMember)/sizeof(atParam[0].m_atMtMember[0]);

	// 判断atParam参数内容
	if (atParam[0].m_byMemberNum > byMaxVmpChlMemNum)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[ChangeCP]:MsgBody GetVmpMemberNum error, MemberNum.%d not correct!\n", atParam[0].m_byMemberNum);
		vmplog("[ChangeCP]:MsgBody GetVmpMemberNum error, MemberNum.%d not correct!\n", atParam[0].m_byMemberNum);
        bMsgBufIsOK = FALSE;
	}

	// 风格切换时，通道大小有变，媒控需要调整解码器停止接收码流，需先停各通道请求关键帧Timer，保证通道ok后请求关键帧
	BOOL32 bKillIframeTimer = FALSE;
	if (!bStart &&
		atParam[0].m_byVMPStyle != m_tCfg.m_tStatus.m_byVMPStyle)
	{
		bKillIframeTimer = TRUE;
		LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[ChangeCP]Change Style, kill EV_VMP_NEEDIFRAME_TIMER!\n");
		vmplog("[ChangeCP]Change Style, kill EV_VMP_NEEDIFRAME_TIMER!\n");
		for (u8 byChNum = 0;byChNum < byMaxVmpChlMemNum;byChNum++)
		{
			KillTimer(EV_VMP_NEEDIFRAME_TIMER+byChNum);
		}
	}

    u8 byNeedPrs = *(u8*)pbyMsgBuf;
    pbyMsgBuf += sizeof(u8);
	wMsgBufLen += sizeof(u8);
	// byNeedPrs判断，0和1以外认为是非法值，消息体内容有问题
	if (byNeedPrs != 0 && byNeedPrs != 1)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[ChangeCP]:MsgBody GetNeedPrs error, NeedPrs.%d not correct!\n", byNeedPrs);
		vmplog("[ChangeCP]:MsgBody GetNeedPrs error, NeedPrs.%d not correct!\n", byNeedPrs);
        bMsgBufIsOK = FALSE;
	}

    TVmpStyleCfgInfo tStyleInfo = *(TVmpStyleCfgInfo*)pbyMsgBuf;
    pbyMsgBuf += sizeof(TVmpStyleCfgInfo);
	wMsgBufLen += sizeof(TVmpStyleCfgInfo);

	// [pengjie 2010/4/24] 空闲通道背景图支持
	TVMPExCfgInfo tVMPExCfgInfo = *(TVMPExCfgInfo *)pbyMsgBuf;
	pbyMsgBuf += sizeof(TVMPExCfgInfo);
	wMsgBufLen += sizeof(TVMPExCfgInfo);
	// 判断VMPExCfgInfo
	if (bMsgBufIsOK)
	{
		if (tVMPExCfgInfo.m_byIdleChlShowMode > VMP_SHOW_USERDEFPIC_MODE)
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[ChangeCP]:MsgBody GetIdleChlShowMode error, IdleChlShowMode.%d not correct!\n", tVMPExCfgInfo.m_byIdleChlShowMode);
			vmplog("[ChangeCP]:MsgBody GetIdleChlShowMode error, IdleChlShowMode.%d not correct!\n", tVMPExCfgInfo.m_byIdleChlShowMode);
			bMsgBufIsOK = FALSE;
		}
		if (tVMPExCfgInfo.m_byIsDisplayMmbAlias != 0 && tVMPExCfgInfo.m_byIsDisplayMmbAlias != 1)
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[ChangeCP]:MsgBody GetIsDisplayMmbAlias error, IsDisplayMmbAlias.%d not correct!\n", tVMPExCfgInfo.m_byIsDisplayMmbAlias);
			vmplog("[ChangeCP]:MsgBody GetIsDisplayMmbAlias error, IsDisplayMmbAlias.%d not correct!\n", tVMPExCfgInfo.m_byIsDisplayMmbAlias);
			bMsgBufIsOK = FALSE;
		}
	}
	
	// 追加解码通道RcvH264DependInMark属性,8kg/8kh/8ki都支持
	u8 abyRcvH264DependInMark[MAXNUM_SDVMP_MEMBER];
	memset(abyRcvH264DependInMark, 0, sizeof(abyRcvH264DependInMark));
	memcpy(abyRcvH264DependInMark,pbyMsgBuf,sizeof(abyRcvH264DependInMark));
	pbyMsgBuf += sizeof(abyRcvH264DependInMark);
	wMsgBufLen += sizeof(abyRcvH264DependInMark);
	// 判断RcvH264DependInMark属性
	if (bMsgBufIsOK)
	{
		for (u8 byChnIdx=0; byChnIdx<sizeof(abyRcvH264DependInMark); byChnIdx++)
		{
			if (abyRcvH264DependInMark[byChnIdx] != 0 && abyRcvH264DependInMark[byChnIdx] != 1)
			{
				LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[ChangeCP]:MsgBody GetRcvH264DependInMark error, Chl.%d RcvH264DependInMark.%d not correct!\n", byChnIdx, abyRcvH264DependInMark[byChnIdx]);
				vmplog("[ChangeCP]:MsgBody GetRcvH264DependInMark error, Chl.%d RcvH264DependInMark.%d not correct!\n", byChnIdx, abyRcvH264DependInMark[byChnIdx]);
				bMsgBufIsOK = FALSE;
				break;
			}
		}
	}

    // Vmp成员别名扩容 [7/4/2013 liaokang]
	if (bStart)
	{
		memset(m_tVmpMbAlias, 0, sizeof(m_tVmpMbAlias));
	}
	else
	{
		for (u8 byIdx=0; byIdx<byMaxVmpChlMemNum;byIdx++)
		{
			if (!(atParam[0].m_atMtMember[byIdx] == m_tParam[0].m_atMtMember[byIdx]))
			{
				memset(&m_tVmpMbAlias[byIdx], 0, sizeof(TVmpMbAlias));
			}
		}
	}
    u8 byChnlNum = *(u8*)pbyMsgBuf;
    pbyMsgBuf ++;
    wMsgBufLen ++;
    u8 byChnlNo = 0;
    u8 byChnlAliasLen = 0;
	// 加保护，对通道个数做判断，不应大于最大合成成员个数
	if (byChnlNum > byMaxVmpChlMemNum)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[ChangeCP]:MsgBody GetChlAlias error, ChnlNum.%d not correct!\n", byChnlNum);
		vmplog("[ChangeCP]:MsgBody GetChlAlias error, ChnlNum.%d not correct!\n", byChnlNum);
		bMsgBufIsOK = FALSE;
	}
	else
	{
		for(u8 byLoop = 0; byLoop < byChnlNum; byLoop++)
		{
			byChnlNo = *(u8*)pbyMsgBuf;
			pbyMsgBuf ++;
			wMsgBufLen ++;
			byChnlAliasLen = *(u8*)pbyMsgBuf;
			pbyMsgBuf ++;
			wMsgBufLen ++;
			if ( byChnlNo < MAXNUM_SDVMP_MEMBER )
			{
				// 开启时，直接设置别名
				if (bStart)
				{
					m_tVmpMbAlias[byChnlNo].SetMbAlias(byChnlAliasLen,(s8*)pbyMsgBuf);
				}
				else
				{
					// 调整时，需判新旧通道成员是否改变，成员未改变时，不更新别名
					if (!(atParam[0].m_atMtMember[byChnlNo] == m_tParam[0].m_atMtMember[byChnlNo]))
					{
						m_tVmpMbAlias[byChnlNo].SetMbAlias(byChnlAliasLen,(s8*)pbyMsgBuf);
					}
				}
			}
			else
			{
				LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[ChangeCP]:MsgBody GetChlAlias error, ChnlNo.%d not correct!\n", byChnlNo);
				vmplog("[ChangeCP]:MsgBody GetChlAlias error, ChnlNo.%d not correct!\n", byChnlNo);
				bMsgBufIsOK = FALSE;
			}
			
			pbyMsgBuf += byChnlAliasLen;
			wMsgBufLen += byChnlAliasLen;
		}
	}

	//zhouyiliang 20110516 8khvmp带上了通道是否active的数组
	u8 abyChnlActive[MAXNUM_VMP_ENCNUM] = {0};
	u8 abyChnlProfileAttrib[MAXNUM_VMP_ENCNUM];
	memset(abyChnlProfileAttrib,(u8)emBpAttrb,sizeof(abyChnlProfileAttrib));
#if defined(_8KH_) || defined(_8KI_)
	// 获得通道是否active
	memcpy(abyChnlActive,pbyMsgBuf,sizeof(abyChnlActive));
	pbyMsgBuf += sizeof(abyChnlActive);
	wMsgBufLen += sizeof(abyChnlActive);

	if (cServMsg.GetMsgBodyLen() > wMsgBufLen )//zhouyiliang 20120327 8kh加上hp/bp属性
	{
		memcpy(abyChnlProfileAttrib,pbyMsgBuf,sizeof(abyChnlProfileAttrib));
		pbyMsgBuf += sizeof(abyChnlProfileAttrib);
		wMsgBufLen += sizeof(abyChnlProfileAttrib);
	}
	else
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[ChangeCP]:MsgBody GetChlAlias error, ChnlNo.%d not correct!\n", byChnlNo);
		vmplog("[ChangeCP]:MsgBody GetChlAlias error, ChnlNo.%d not correct!\n", byChnlNo);
		bMsgBufIsOK = FALSE;
	}
#else
	//8kg不带则默认为1
	memset(abyChnlActive,1,sizeof(abyChnlActive));
#endif
	// 判断abyChnlActive数组与abyChnlProfileAttrib数组
	if (bMsgBufIsOK)
	{
		u8 byChlId = 0;
		for (byChlId=0; byChlId<sizeof(abyChnlActive); byChlId++)
		{
			// 通道Active参数不可以大于2
			if (abyChnlActive[byChlId] > emVmpOutChnlNotChange)
			{
				LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[ChangeCP]:MsgBody GetChnlActive error, Chnl.%d Active.%d not correct!\n",byChlId, abyChnlActive[byChlId]);
				vmplog("[ChangeCP]:MsgBody GetChnlActive error, Chnl.%d Active.%d not correct!\n",byChlId, abyChnlActive[byChlId]);
				bMsgBufIsOK = FALSE;
				break;
			}
		}
		for (byChlId=0; byChlId<sizeof(abyChnlProfileAttrib); byChlId++)
		{
			// 通道BP HP属性check 以外的为非法值
			if (abyChnlProfileAttrib[byChlId] != emBpAttrb && abyChnlProfileAttrib[byChlId] != emHpAttrb)
			{
				LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[ChangeCP]:MsgBody GetChnlProfileAttrib error, Chnl.%d ProfileAttrib.%d not correct!\n",byChlId, abyChnlProfileAttrib[byChlId]);
				vmplog("[ChangeCP]:MsgBody GetChnlProfileAttrib error, Chnl.%d ProfileAttrib.%d not correct!\n",byChlId, abyChnlProfileAttrib[byChlId]);
				bMsgBufIsOK = FALSE;
				break;
			}
		}
	}

//     TCapSupportEx tCapSupportEx = *(TCapSupportEx*)pbyMsgBuf;
//     pbyMsgBuf += sizeof(TCapSupportEx);

    CConfId cConfId = cServMsg.GetConfId();

    cServMsg.SetMsgBody();
    
    //2、 对MCU传来的参数进行字节序整理
    for ( byChnIdx = 0; byChnIdx < MAXNUM_VMP_ENCNUM; byChnIdx++ )
    {
        atParam[byChnIdx].m_wBitRate = ntohs(atParam[byChnIdx].m_wBitRate);
        atParam[byChnIdx].m_wVideoWidth = ntohs(atParam[byChnIdx].m_wVideoWidth);
        atParam[byChnIdx].m_wVideoHeight = ntohs(atParam[byChnIdx].m_wVideoHeight);

        if ( 0 == atParam[byChnIdx].m_wBitRate )
        {
            atParam[byChnIdx].m_wBitRate = 256;
        }

		//mpeg 4cif = (720 * 576), not (704 * 576)
		if( MEDIA_TYPE_MP4 == atParam[byChnIdx].m_byEncType && 704 == atParam[byChnIdx].m_wVideoWidth )
		{
			atParam[byChnIdx].m_wVideoWidth = 720;
		}
    }

	if (!bMsgBufIsOK)
	{
        SendMsgToMcu(cServMsg.GetEventId()+2, &cServMsg);
		// 恢复关键帧Timer，若之前被kill掉了
		if (bKillIframeTimer)
		{
			SetNeedIFrameTimer4VmpChl();
		}
        return;
	}
   
    C8KEVMPParam atSetParam[MAXNUM_VMP_ENCNUM]; // 实际设置下去的编码参数
    BOOL32 abIsValid[MAXNUM_VMP_ENCNUM];        // 编码参数合法性记录
    s8 anExistChnOfParam[MAXNUM_VMP_ENCNUM];    // 记录画面合成器当前编码参数和上层参数一致的编码通道索引号
    s8 anEncChnOfParam[MAXNUM_VMP_ENCNUM];      // 记录上层传来的编码参数最终对应的画面合成器编码通道索引号
    s8 anParamOfEncChn[MAXNUM_VMP_ENCNUM];      // 记录画面合成器编码通道最终对应的上层传来的编码参数索引号
	u8 abySetProfileType[MAXNUM_VMP_ENCNUM];    // 记录每路编码的hp/bp属性
    for ( byChnIdx = 0; byChnIdx < byMaxVmpEncNum; byChnIdx++ )
    {
        atSetParam[byChnIdx].m_byEncType = MEDIA_TYPE_NULL;
        atSetParam[byChnIdx].m_wBitRate = 0;
        atSetParam[byChnIdx].m_wVideoHeight = 0;
        atSetParam[byChnIdx].m_wVideoWidth = 0;
		atSetParam[byChnIdx].m_byVMPStyle = 0;

        abIsValid[byChnIdx] = FALSE;

        anExistChnOfParam[byChnIdx] = -1;
        anEncChnOfParam[byChnIdx] = -1;
        anParamOfEncChn[byChnIdx] = -1;

		abySetProfileType[byChnIdx] = (u8)emBpAttrb;
    }

    //3、 对MCU传来的参数进行有效性整理
    u8 byValidParamNum = 0; // 合法参数数量
    u8 bySetIdx = 0;
    for ( byChnIdx = 0; byChnIdx < byMaxVmpEncNum; byChnIdx++ )
    {
		BOOL32 bNullParam = FALSE;
        if ( MEDIA_TYPE_NULL == atParam[byChnIdx].m_byEncType || 0 == atParam[byChnIdx].m_byEncType )
        {
            // 编码参数不合法
			if(atParam[byChnIdx].m_byVMPStyle != VMP_STYLE_NONE  )
			{
				atSetParam[byChnIdx].m_byVMPStyle = atParam[byChnIdx].m_byVMPStyle;
			}
			bNullParam = TRUE;
//            continue;
        }
        u8 byTmpIdx = 0;
        for ( byTmpIdx = 0; byTmpIdx < bySetIdx; byTmpIdx++ )
        {
            if (!bNullParam && atSetParam[byTmpIdx].m_byEncType == atParam[byChnIdx].m_byEncType &&
                atSetParam[byTmpIdx].m_wBitRate == atParam[byChnIdx].m_wBitRate &&
                atSetParam[byTmpIdx].m_wVideoWidth == atParam[byChnIdx].m_wVideoWidth &&
                atSetParam[byTmpIdx].m_wVideoHeight == atParam[byChnIdx].m_wVideoHeight &&
				abySetProfileType[byTmpIdx] == abyChnlProfileAttrib[byChnIdx] &&
				atSetParam[byTmpIdx].m_byFrameRate == atParam[byChnIdx].m_byFrameRate
				)
            {
                break;
            }
        }

        if ( byTmpIdx < bySetIdx )
        {
            // 不重复记录相同的编码参数
            continue;
        }
        atSetParam[bySetIdx] = atParam[byChnIdx];
		abySetProfileType[bySetIdx] = abyChnlProfileAttrib[byChnIdx];
        abIsValid[bySetIdx] = TRUE;
        bySetIdx++;
        byValidParamNum++;
    }
    if ( 0 == byValidParamNum )
    {
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[ChangeCP]: Encode param error, no valid param!\n");
        OspPrintf(TRUE, FALSE, "[ChangeCP]: Encode param error, no valid param!\n");
        SendMsgToMcu(cServMsg.GetEventId()+2, &cServMsg);
		// 恢复关键帧Timer，若之前被kill掉了
		if (bKillIframeTimer)
		{
			SetNeedIFrameTimer4VmpChl();
		}
        return;
    }

    // 打印 MCU 传入的参数, zgc, 2008-03-19
	LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"/*---------------------------------------------------------*\n\n");
	LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"Following are the params that mcu send to the vmp:\n\n");
    vmplog("/*---------------------------------------------------------*\n\n");
    vmplog("Following are the params that mcu send to the vmp:\n\n");
	for (byChnIdx = 0; byChnIdx < byMaxVmpChlMemNum; byChnIdx++)
	{
		LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"InCh%d: RcvH264DependInMark=%d \n", byChnIdx, abyRcvH264DependInMark[byChnIdx]);
	}
    for ( byChnIdx = 0; byChnIdx < byMaxVmpEncNum; byChnIdx++ )
    {
		LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"Ch%d: VmpStyle=%d EncType=%s BitRate=%u Video: %u*%u profiletype:%d ChnlActive:%d fps:%d\n", byChnIdx,
            atSetParam[byChnIdx].m_byVMPStyle,GetMediaString(atSetParam[byChnIdx].m_byEncType), 
			atSetParam[byChnIdx].m_wBitRate, 
			atSetParam[byChnIdx].m_wVideoWidth, atSetParam[byChnIdx].m_wVideoHeight,
			abySetProfileType[byChnIdx], abyChnlActive[byChnIdx],atSetParam[byChnIdx].m_byFrameRate );

        vmplog("Ch%d: VmpStyle=%d EncType=%s BitRate=%u Video: %u*%u profiletype:%d ChnlActive:%d fps:%d\n", byChnIdx,
            atSetParam[byChnIdx].m_byVMPStyle,GetMediaString(atSetParam[byChnIdx].m_byEncType), 
			atSetParam[byChnIdx].m_wBitRate, atSetParam[byChnIdx].m_wVideoWidth, atSetParam[byChnIdx].m_wVideoHeight,
			abySetProfileType[byChnIdx], abyChnlActive[byChnIdx],atSetParam[byChnIdx].m_byFrameRate );
    }
	LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"/*---------------------------------------------------------*\n\n");
    vmplog("/*---------------------------------------------------------*\n\n");

	// [pengjie 2010/4/24] 空闲通道显示背景图片支持
	if( tVMPExCfgInfo.m_byIdleChlShowMode > VMP_SHOW_USERDEFPIC_MODE )
	{
		LogPrint(LOG_LVL_WARNING,MID_MCU_VMP,"[ChangeCP] Mcu set IdleChlShowMode: %d But it's invalid so Reset Mode: 0 !\n",
			tVMPExCfgInfo.m_byIdleChlShowMode );
		vmplog( "[ChangeCP] Mcu set IdleChlShowMode: %d But it's invalid so Reset Mode: 0 !\n",
			tVMPExCfgInfo.m_byIdleChlShowMode );
		tVMPExCfgInfo.m_byIdleChlShowMode = VMP_SHOW_BLACK_MODE;
	}
	else
	{
		LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[ChangeCP] Mcu set IdleChlShowMode: %d !\n", tVMPExCfgInfo.m_byIdleChlShowMode );
		vmplog( "[ChangeCP] Mcu set IdleChlShowMode: %d !\n", tVMPExCfgInfo.m_byIdleChlShowMode );
	}

	s32 nReturn = 0;
	// 给底层设置空闲通道显示模式
	u32 dwNoStreamBak = (u32)NoStrm_PlyBlk;
	if( VMP_SHOW_BLACK_MODE == tVMPExCfgInfo.m_byIdleChlShowMode )
	{
		dwNoStreamBak = (u32)NoStrm_PlyBlk;
	}
	else if(  VMP_SHOW_LASTFRAME_MODE == tVMPExCfgInfo.m_byIdleChlShowMode )
	{
		dwNoStreamBak = (u32)NoStrm_PlyLst;
	}
	else if(  VMP_SHOW_DEFPIC_MODE == tVMPExCfgInfo.m_byIdleChlShowMode )
	{
		dwNoStreamBak = (u32)NoStrm_PlyBmp_dft;
	}
	else  // 用户自定义图片
	{
		dwNoStreamBak = (u32)NoStrm_PlyBmp_usr;
		
	}

	nReturn = m_pcHardMulPic->SetNoStreamBak( dwNoStreamBak );
	
	for (u8 nIdx = 0; nIdx < byMaxVmpChlMemNum; nIdx++)
	{
		if ( m_abChnlSetBmp[nIdx])//已经设置过通道图片了，判断通道终端是否变了
		{
			if ( atSetParam[0].m_atMtMember[nIdx].GetMcuId() ==  m_tParam[0].m_atMtMember[nIdx].GetMcuId() && 
				atSetParam[0].m_atMtMember[nIdx].GetMtId() ==  m_tParam[0].m_atMtMember[nIdx].GetMtId() )
			{
				continue;
			}
			else
			{
				//先清掉图片
				nReturn = m_pcHardMulPic->SetNoStreamBak((u32)NoStrm_PlyBmp_usr,NULL, nIdx);
				m_abChnlSetBmp[nIdx] = FALSE;
				if ( (s32)TRUE != nReturn )
				{
					LogPrint(LOG_LVL_ERROR,MID_MCU_VMP, "[ChangeCP] Index.%d SetNoStreamBak clear usrbmp failed(ErrCode.%d)!\n", nIdx, nReturn);
					vmplog("[ChangeCP] Index.%d SetNoStreamBak usrbmp failed(ErrCode.%d)!\n", nIdx, nReturn);
				}
				else
				{
					LogPrint(LOG_LVL_ERROR,MID_MCU_VMP, "[ChangeCP] Index.%d SetNoStreamBak clear usrbmp succeed!\n", nIdx);
					vmplog("[ChangeCP] Index.%d SetNoStreamBak clear usrbmp succeed!\n", nIdx);
				}
			}
		}

		//没设置过或者设置了但是通道终端变了，遍历是否配了要设图片	
		//20111108 zjl modify 支持vmp成员显示自定义图片
		for (u8 byIndex = 0; byIndex < MAXNUM_VMPDEFINE_PIC; byIndex++)
		{
			if(!atSetParam[0].m_atMtMember[nIdx].IsNull() &&
				strlen(m_tVmpMbAlias[nIdx].GetMbAlias()) > 0 && 
				0 == strcmp(m_tVmpMbAlias[nIdx].GetMbAlias(),m_tDbgVal.GetVmpMemAlias(byIndex)) 
				)
			{
				//调用接口
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"[ChangeCP]index.%d -> alias.%s -> pic.%s!\n", nIdx, 
					m_tDbgVal.GetVmpMemAlias(byIndex), 
					m_tDbgVal.GetVmpMemRoute(byIndex));
				vmplog("[ChangeCP]index.%d -> alias.%s -> pic.%s!\n", 
					nIdx, 
					m_tDbgVal.GetVmpMemAlias(byIndex), 
					m_tDbgVal.GetVmpMemRoute(byIndex));

				s8 szVmpMemRoute[255]=  {0};
				memcpy(szVmpMemRoute,m_tDbgVal.GetVmpMemRoute(byIndex),255);
				nReturn = m_pcHardMulPic->SetNoStreamBak((u32)NoStrm_PlyBmp_usr,szVmpMemRoute,nIdx );
				if ( (s32)TRUE != nReturn )
				{
					LogPrint(LOG_LVL_ERROR,MID_MCU_VMP, "[ChangeCP] Index.%d SetNoStreamBak usrbmp failed(ErrCode.%d)!\n", nIdx, nReturn);
					vmplog("[ChangeCP] Index.%d SetNoStreamBak usrbmp failed(ErrCode.%d)!\n", nIdx, nReturn);
				}
				m_abChnlSetBmp[nIdx] = TRUE;
				
				break;
			}
		}
		
	
	}

	vmplog( "[ChangeCP] Set IdleChlShowMode return %d \n", nReturn );
	// End
				
    // 是否显示成员别名, zgc, 2009-07-24
    BOOL32 IsDisplayMmbAlias = ( 1 == atSetParam[0].m_byIsDisplayMmbAlias ) ? TRUE : FALSE;
	LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"IsDisplayMmbAlias = %d\n", atSetParam[0].m_byIsDisplayMmbAlias  );
    vmplog( "IsDisplayMmbAlias = %d\n", atSetParam[0].m_byIsDisplayMmbAlias );

    //4、 视频编码参数格式转换，转换为媒控层需要的参数
    TVidEncParam atEncParam[MAXNUM_VMP_ENCNUM];
    memset( atEncParam, 0, sizeof(atEncParam) );
    for ( byChnIdx = 0; byChnIdx < byMaxVmpEncNum; byChnIdx++ )
    {
        if ( abIsValid[byChnIdx] )
        {
            Trans2EncParam( &atSetParam[byChnIdx], &atEncParam[byChnIdx] );
			if ( abySetProfileType[byChnIdx] == (u8)emHpAttrb  )
			{
				atEncParam[byChnIdx].m_byEncQuality = HPMULTE_PRIO;//3
			}
		
			
        }
        else
        {
            C8KEVMPParam tDefaultParam;
            tDefaultParam.m_byEncType = MEDIA_TYPE_H264;
            tDefaultParam.m_wBitRate = 256;
            u16 wWidth = 0;
            u16 wHeight = 0;
            GetWHViaRes( VIDEO_FORMAT_CIF, wWidth, wHeight );
            tDefaultParam.m_wVideoWidth = wWidth;
            tDefaultParam.m_wVideoHeight = wHeight;
            
            Trans2EncParam( &tDefaultParam, &atEncParam[byChnIdx] );
        }
		
		LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"EncParam tp.%d h.%d w.%d bitrate.%d frmrate.%d-%d\n", 
			atEncParam[byChnIdx].m_byEncType,atEncParam[byChnIdx].m_wEncVideoHeight,atEncParam[byChnIdx].m_wEncVideoWidth,
			atEncParam[byChnIdx].m_dwBitRate,atEncParam[byChnIdx].m_byMinFrameRate,atEncParam[byChnIdx].m_byMaxFrameRate);


		vmplog( "EncParam tp.%d h.%d w.%d bitrate.%d frmrate.%d-%d\n", 
			atEncParam[byChnIdx].m_byEncType,atEncParam[byChnIdx].m_wEncVideoHeight,atEncParam[byChnIdx].m_wEncVideoWidth,
			atEncParam[byChnIdx].m_dwBitRate,atEncParam[byChnIdx].m_byMinFrameRate,atEncParam[byChnIdx].m_byMaxFrameRate);
    }  
    
    // 获得当前合成器实际状态
    TMulPicStatus tMulPicStatus;
    if ( !m_pcHardMulPic->GetStatus(tMulPicStatus) )
    {
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[ChangeCP] Get vmp current MulPicStatus failed!\n"  );
        OspPrintf( TRUE, FALSE, "[ChangeCP] Get vmp current MulPicStatus failed!\n" );
        SendMsgToMcu(cServMsg.GetEventId()+2, &cServMsg);
		// 恢复关键帧Timer，若之前被kill掉了
		if (bKillIframeTimer)
		{
			SetNeedIFrameTimer4VmpChl();
		}
        return;
    }

    BOOL32 bRetOk = TRUE;   
    //5、 开始合成
    if ( !tMulPicStatus.bMergeStart )
    {
        s32 nEncNum = byValidParamNum/*MAXNUM_VMP_ENCNUM*/;    //固定开始4路编码，具体通过下面设置编码参数
		
		for(u8 bylp = 0; bylp < byMaxVmpEncNum; bylp ++)
		{
			LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"m_tSndAddr[%d]-port: %d\n", bylp, m_tSndAddr[bylp].wPort );
			OspPrintf(TRUE, FALSE, "m_tSndAddr[%d]-port: %d\n", bylp, m_tSndAddr[bylp].wPort);
		}
		
        REC_LASTCODELINE;
		//防止start的时候先下小的，change的时候变大了，媒控底层出问题
		TVidEncParam atOrigEncParam[MAXNUM_VMP_ENCNUM];
		memcpy(atOrigEncParam,atEncParam,sizeof(atOrigEncParam));
		/*for ( byChnIdx = 0; byChnIdx < byMaxVmpEncNum; byChnIdx++)
		{
			atOrigEncParam[byChnIdx].m_wEncVideoWidth = atSetParam[byChnIdx].GetVideoWidth();
			atOrigEncParam[byChnIdx].m_wEncVideoHeight = atSetParam[byChnIdx].GetVideoHeight();
		}*/
        bRetOk &= m_pcHardMulPic->StartMerge( atOrigEncParam, m_tSndAddr, m_tSndLocalAddr, nEncNum ); 
        if ( !bRetOk )
        {
			LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[ChangeCP] Startmerge failed\n" );
            OspPrintf(TRUE, FALSE, "[ChangeCP] Startmerge failed\n");
        }
		//如果有因码率原因导致分辨率下小的，下小
		for ( byChnIdx = 0; byChnIdx < nEncNum && byChnIdx < byMaxVmpEncNum; byChnIdx++)
		{
			if ( atOrigEncParam[byChnIdx].m_wEncVideoWidth != atEncParam[byChnIdx].m_wEncVideoWidth ||
				atOrigEncParam[byChnIdx].m_wEncVideoHeight != atEncParam[byChnIdx].m_wEncVideoHeight )
			{
				bRetOk &= m_pcHardMulPic->SetEncParam(byChnIdx,&atEncParam[byChnIdx]);
				if (!bRetOk)
				{
					LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[ChangeCP]chnl:%d SetEncParam failed\n" ,byChnIdx);
					OspPrintf(TRUE, FALSE, "[ChangeCP]chnl:%d SetEncParam failed\n" ,byChnIdx);
				}
			}
			
		}
        REC_LASTCODELINE;  
        
        // 记录编码参数和编码通道之间的对应关系
        for ( byChnIdx = 0; byChnIdx < byValidParamNum && byChnIdx < byMaxVmpEncNum; byChnIdx++ )
        {
            if ( abIsValid[byChnIdx] )
            {
                anEncChnOfParam[byChnIdx] = byChnIdx;
                anParamOfEncChn[byChnIdx] = byChnIdx;
            }
        }
    }
    else //5、或切换编码
    {
        // 确定画面合成器当前编码参数和上层参数一致的编码通道
        for ( byChnIdx = 0; byChnIdx < byValidParamNum && byChnIdx < byMaxVmpEncNum; byChnIdx++ )
        {
            if ( abIsValid[byChnIdx] )
            {
                anExistChnOfParam[byChnIdx] = GetEncChnIdxByParam( &atEncParam[byChnIdx] );
            }
        }

        // 填写所有能找到一致编码通道的参数的相关索引
        for ( byChnIdx = 0; byChnIdx < byValidParamNum && byChnIdx < byMaxVmpEncNum; byChnIdx++ )
        {
            if ( abIsValid[byChnIdx] /*&& -1 != anExistChnOfParam[byChnIdx]*/ )
            {
                anEncChnOfParam[byChnIdx] = byChnIdx/*anExistChnOfParam[byChnIdx]*/;
                anParamOfEncChn[byChnIdx/*anEncChnOfParam[byChnIdx]*/] = byChnIdx;
            }
        }


        // 调用底层接口，设置编码参数
        for ( byChnIdx = 0; byChnIdx < byValidParamNum && byChnIdx < byMaxVmpEncNum; byChnIdx++ )
        {
            if ( abIsValid[byChnIdx] && -1 == anExistChnOfParam[byChnIdx] )
            {
				LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"EncParam%d set to enc chn%d!\n", byChnIdx, anEncChnOfParam[byChnIdx]  );
                vmplog( "EncParam%d set to enc chn%d!\n", byChnIdx, anEncChnOfParam[byChnIdx] );
                REC_LASTCODELINE;
                TVidEncParam tSetEncParam = atEncParam[byChnIdx];  // 底层会修改参数中的码率，所以需要使用临时变量作为函数传入参数
                bRetOk &= m_pcHardMulPic->SetEncParam(anEncChnOfParam[byChnIdx], &tSetEncParam);
                if ( !bRetOk )
                {
					LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[vmp] EncParam%d set failed\n", byChnIdx);
                    OspPrintf(TRUE, FALSE, "[vmp] EncParam%d set failed\n", byChnIdx);
                }
                REC_LASTCODELINE; 
            }
        }
    }


    //6、设置编码通道Active属性
    REC_LASTCODELINE;
    for ( byChnIdx = 0; byChnIdx < byValidParamNum && byChnIdx < byMaxVmpEncNum; byChnIdx++ )
    {
		BOOL32 bActive = ( -1 != anParamOfEncChn[byChnIdx] ? TRUE : FALSE );
		//bActive = FALSE; //win32版测试用
		if ( abyChnlActive[byChnIdx] == (u8)emVmpOutChnlInactive ) 
		{
			bActive = FALSE ;
		}
		
        bRetOk &= m_pcHardMulPic->SetEncActive( byChnIdx,  bActive);
		LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"Set enc chn %d active.%s\n", byChnIdx, (bActive ? "TRUE" : "FALSE" ));
        vmplog( "Set enc chn %d active.%s\n", byChnIdx, (bActive ? "TRUE" : "FALSE" ) );
    }
    REC_LASTCODELINE;
        
    //7、 修改画面合成风格
    u32  dwStyle = ConvertVcStyle2HardStyle( atSetParam[0].m_byVMPStyle );  
    if ( atSetParam[0].m_byVMPStyle != m_tCfg.m_tStatus.m_byVMPStyle )// 与原有风格不同
    {
		LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[ChangeCP]: Change Style from %d to %u!\n", m_tCfg.m_tStatus.m_byVMPStyle, atSetParam[0].m_byVMPStyle);
        vmplog("[ChangeCP]: Change Style from %d to %u!\n", m_tCfg.m_tStatus.m_byVMPStyle, atSetParam[0].m_byVMPStyle);

        // 改变类型
        REC_LASTCODELINE; 
        bRetOk &= m_pcHardMulPic->SetMulPicType(dwStyle);
        if (!bRetOk)
        {
			LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[vmp] Fail to SetMulPicType\n");
            OspPrintf(TRUE, FALSE, "[vmp] Fail to SetMulPicType\n"); 
        }
        else
        {
			LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"Set hard mulpic type %d\n", dwStyle );
            vmplog( "Set hard mulpic type %d\n", dwStyle );
        }
        REC_LASTCODELINE;            
    }

    
    //8、 设置编码加密参数
    u8  abyKeyBuf[MAXLEN_KEY];   
    s32 nKeyLen = 0;
    u8  abyOldKeyBuf[MAXLEN_KEY];    
    s32 nOldKeyLen = 0;

    TMediaEncrypt tEncKey;
    for ( byChnIdx = 0; byChnIdx < byValidParamNum && byChnIdx < byMaxVmpEncNum; byChnIdx++ )
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
					LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[vmp] Fail to set encrypt key of EncChn%d\n", anEncChnOfParam[byChnIdx]  );
                    OspPrintf(TRUE, FALSE, "[vmp] Fail to set encrypt key of EncChn%d\n", anEncChnOfParam[byChnIdx] ); 
                }
                else
                {
					LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"Set encrypt key of EncChn%d success\n", anEncChnOfParam[byChnIdx] );
                    vmplog( "Set encrypt key of EncChn%d success\n", anEncChnOfParam[byChnIdx] );
                    // 记录新的编码密钥
                    m_tEncEncrypt[anEncChnOfParam[byChnIdx]] = tEncKey;
                }
            }
        }
    }

    TNetAddress tNetAddr;
    TNetAddress tNetRtcpAddr;
	TNetAddress tLocalReqRtcpAddr;
    tNetAddr.dwIp = 0;
    tNetRtcpAddr.dwIp = m_tSndAddr[0].dwIp;
	tLocalReqRtcpAddr.dwIp = 0;
    u8 byChnNo = 0;
    for ( byChnNo = 0; byChnNo < atSetParam[0].m_byMemberNum && byChnNo < byMaxVmpChlMemNum; byChnNo++)
    {        
        // 原存在而现在不存在则先删除
        if (m_abChnlValid[byChnNo] && atSetParam[0].m_atMtMember[byChnNo].IsNull())
        {
            REC_LASTCODELINE;          
            if (!m_pcHardMulPic->SetChannelActive(byChnNo, FALSE) )
            {
                bRetOk = FALSE;
				LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[vmp] Fail to SetChannelActive, chn.%d\n", byChnNo );
                OspPrintf(TRUE, FALSE, "[vmp] Fail to SetChannelActive, chn.%d\n", byChnNo); 
            }
            else
            {
                LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[ChangeCP]Remove channel %d.\n", byChnNo);
				vmplog("[ChangeCP]Remove channel %d.\n", byChnNo);
                m_pcHardMulPic->ClearChannelCache( byChnNo );
            }
            REC_LASTCODELINE;      
        }
        // 原先不存在但现在存在
        else if ( !atSetParam[0].m_atMtMember[byChnNo].IsNull() )
        {
            nKeyLen = 0;
            nOldKeyLen = 0;
            memset( abyKeyBuf, 0, MAXLEN_KEY );
            memset( abyOldKeyBuf, 0, MAXLEN_KEY );

            atSetParam[0].m_tVideoEncrypt[byChnNo].GetEncryptKey( abyKeyBuf, &nKeyLen );
            m_tParam[0].m_tVideoEncrypt[byChnNo].GetEncryptKey( abyOldKeyBuf, &nOldKeyLen );           

            if ( !m_abChnlValid[byChnNo] || 
					( atSetParam[0].m_tVideoEncrypt[byChnNo].GetEncryptMode() != m_tParam[0].m_tVideoEncrypt[byChnNo].GetEncryptMode() ||
                       nKeyLen != nOldKeyLen || 0 != strncmp( (s8*)abyKeyBuf, (s8*)abyOldKeyBuf, nKeyLen ) 
					) ||
					( atSetParam[0].m_tDoublePayload[byChnNo].GetRealPayLoad() !=  m_tParam[0].m_tDoublePayload[byChnNo].GetRealPayLoad() ||
						atSetParam[0].m_tDoublePayload[byChnNo].GetActivePayload() !=  m_tParam[0].m_tDoublePayload[byChnNo].GetActivePayload() 
					)
				)
            {
                if (!SetDecryptParam(byChnNo, &atSetParam[0].m_tVideoEncrypt[byChnNo], &atSetParam[0].m_tDoublePayload[byChnNo]))
                {
                    bRetOk = FALSE;
					LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[vmp] Fail to SetDecryptParam mmb.%d\n", byChnNo);
                    OspPrintf(TRUE, FALSE, "[vmp] Fail to SetDecryptParam mmb.%d\n", byChnNo); 
                }
                
                tNetAddr.wPort = g_cVMPApp.m_tEqpCfg.GetEqpStartPort() + byChnNo*PORTSPAN;
                tNetRtcpAddr.wPort = GetSendRtcpPort(byChnNo);
				//本地发送rtcp端口
				tLocalReqRtcpAddr.wPort = g_cVMPApp.m_tEqpCfg.GetEqpStartPort() + byChnNo*PORTSPAN + 1;

                REC_LASTCODELINE;
                bRetOk &= m_pcHardMulPic->SetChannelParam(byChnNo, tNetAddr, tNetRtcpAddr, tLocalReqRtcpAddr);
                REC_LASTCODELINE;
                bRetOk &= m_pcHardMulPic->SetChannelActive(byChnNo, TRUE);
                REC_LASTCODELINE;
                
				LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[ChangeCP]Add channel %d for mt%d at port %d, pt: %d/%d, ret=%d\n", 
                        byChnNo, 
                        atSetParam[0].m_atMtMember[byChnNo].GetMtId(), 
                        tNetAddr.wPort,
                        atSetParam[0].m_tDoublePayload[byChnNo].GetRealPayLoad(),
                        atSetParam[0].m_tDoublePayload[byChnNo].GetActivePayload(),
                        bRetOk);
                vmplog("[ChangeCP]Add channel %d for mt%d at port %d, pt: %d/%d, ret=%d\n", 
                        byChnNo, 
                        atSetParam[0].m_atMtMember[byChnNo].GetMtId(), 
                        tNetAddr.wPort,
                        atSetParam[0].m_tDoublePayload[byChnNo].GetRealPayLoad(),
                        atSetParam[0].m_tDoublePayload[byChnNo].GetActivePayload(),
                        bRetOk);
            }  
        }                          
    }

    //10、 设置logo,RcvInMarker
    for ( byChnNo = 0; byChnNo < atSetParam[0].m_byMemberNum && byChnNo < byMaxVmpChlMemNum; byChnNo++)
    {
        if ( !atSetParam[0].m_atMtMember[byChnNo].IsNull() )
        {
            REC_LASTCODELINE;
            SetLogo( byChnNo, IsDisplayMmbAlias, (s8*)m_tVmpMbAlias[byChnNo].GetMbAlias() );
            REC_LASTCODELINE;
			// 设置RcvInMarker，给协议用
			LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"Mmb.%d SetRcvInMarker.%s\n", byChnNo, abyRcvH264DependInMark[byChnNo] );
			bRetOk &= m_pcHardMulPic->SetRcvInMarker( byChnNo, abyRcvH264DependInMark[byChnNo] );
            REC_LASTCODELINE;
        }
        else
        {
            REC_LASTCODELINE;
            SetLogo( byChnNo, FALSE );
            REC_LASTCODELINE;
			// 设置RcvInMarker，给协议用
			bRetOk &= m_pcHardMulPic->SetRcvInMarker( byChnNo, FALSE );
            REC_LASTCODELINE;
        }        
    }
	// [4/16/2010 xliang] 清不必要的logo
	for( byChnNo = atSetParam[0].m_byMemberNum; byChnNo < byMaxVmpChlMemNum; byChnNo ++ )
	{
        SetLogo( byChnNo, FALSE );
	}

    //11、 设置边框和颜色
    bRetOk &= SetVmpAttachStyle(tStyleInfo, atSetParam[0]);    
    
    if (byCurState == NORMAL)
    {
        //12、 设置丢包重传参数，仅在开始时设置一次
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
		LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[ChangeCP]SetNetSendFeedbackVideoParam Prs=%d, FirstTimeSpan=%d\n", m_byNeedPrs, tNetRSParam.m_wFirstTimeSpan);
        vmplog("[ChangeCP]SetNetSendFeedbackVideoParam Prs=%d, FirstTimeSpan=%d\n", m_byNeedPrs, tNetRSParam.m_wFirstTimeSpan);
    }

    
    if ( !bRetOk )
    {
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[Info] ChangeCP failed\n");
        OspPrintf(TRUE, FALSE, "[Info] ChangeCP failed\n");
        SendMsgToMcu(cServMsg.GetEventId()+2, &cServMsg);
		// 恢复关键帧Timer，若之前被kill掉了
		if (bKillIframeTimer)
		{
			SetNeedIFrameTimer4VmpChl();
		}
        return;
    }

    for ( byChnNo = 0; byChnNo < byMaxVmpChlMemNum; byChnNo++)
    {
		if ( byChnNo >= atSetParam[0].m_byMemberNum )
		{
			m_abChnlValid[byChnNo] = FALSE;
		}
		else
		{
			m_abChnlValid[byChnNo] = atSetParam[0].m_atMtMember[byChnNo].IsNull() ? FALSE : TRUE;
		}
        
    }

    memcpy( m_tParam, atSetParam, MAXNUM_VMP_ENCNUM * sizeof(C8KEVMPParam));
	memcpy( m_abyChanProfileType ,abySetProfileType,MAXNUM_VMP_ENCNUM);

    m_byNeedPrs = byNeedPrs;
    m_tStyleInfo = tStyleInfo;
//    m_tCapSupportEx = tCapSupportEx;
    m_cConfId = cConfId;
    m_bDbVid = ( 2 == byValidParamNum ) ? TRUE : FALSE;
    m_tCfg.m_tStatus.m_byVMPStyle = atSetParam[0].m_byVMPStyle;
    m_tCfg.m_tStatus.m_byMemberNum = atSetParam[0].m_byMemberNum;
    
    SendMsgToMcu(cServMsg.GetEventId()+1, &cServMsg);   
    
    // 取得当前新设置的合成状态及统计
    m_pcHardMulPic->GetStatus( tMulPicStatus );
    TMulPicStatis tMulPicStatis;
    m_pcHardMulPic->GetStatis( tMulPicStatis );

    // VMP实际编码情况构建消息体
    u8 byEncNum = MAXNUM_VMP_ENCNUM; 
    TVideoStreamCap atVmpOutPutCap[MAXNUM_VMP_ENCNUM];
    for ( byChnIdx = 0; byChnIdx < byMaxVmpEncNum; byChnIdx++ )
    {
        if ( -1 != anParamOfEncChn[byChnIdx] )
        {
            atVmpOutPutCap[byChnIdx].SetMediaType( tMulPicStatus.atVidEncParam[byChnIdx].m_byEncType );
            atVmpOutPutCap[byChnIdx].SetResolution( GetResViaWH( tMulPicStatus.atVidEncParam[byChnIdx].m_wEncVideoWidth, tMulPicStatus.atVidEncParam[byChnIdx].m_wEncVideoHeight ) );
            atVmpOutPutCap[byChnIdx].SetMaxBitRate( (u16)atEncParam[anParamOfEncChn[byChnIdx]].m_dwBitRate );   // 这里用设置值，因为底层会对设置的码率打折扣
            atVmpOutPutCap[byChnIdx].SetUserDefFrameRate( tMulPicStatus.atVidEncParam[byChnIdx].m_byMinFrameRate );    // 固定25帧
        }
    }

    cServMsg.SetMsgBody( (u8*)&byEncNum, sizeof(byEncNum) );
    cServMsg.CatMsgBody( (u8*)atVmpOutPutCap, sizeof(atVmpOutPutCap) );

    if ( cServMsg.GetEventId() == MCU_VMP_STARTVIDMIX_REQ )
    {        
        SendMsgToMcu(VMP_MCU_STARTVIDMIX_NOTIF, &cServMsg);    
    }
    else
    {
        SendMsgToMcu(VMP_MCU_CHANGESTATUS_NOTIF, &cServMsg); 
    }      
    
    SendStatusChangeMsg(TRUE, (u8)TVmpStatus::START);


	//[nizhijun 2011/06/21]针对每个通道定时请求关键帧
	SetNeedIFrameTimer4VmpChl();
	

    return;
}

/*lint -restore*/
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
BOOL32 CVMPInst::GetEncryptParam( C8KEVMPParam *ptParam, TMediaEncrypt *ptVideoEncrypt )
{
    if ( NULL == ptParam || NULL == ptVideoEncrypt )
    {
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[GetEncryptParam] Error! Point as param is null!\n" );
        OspPrintf( TRUE, FALSE, "[GetEncryptParam] Error! Point as param is null!\n" );
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
BOOL32 CVMPInst::SetEncryptParam(u8 byChnNo, TMediaEncrypt *ptVideoEncrypt, u8 byMeidaType)
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
		LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[Info]Encode encrypt key is NULL\n");
        vmplog("[Info]Encode encrypt key is NULL\n");

        REC_LASTCODELINE;  
        bRet &= m_pcHardMulPic->SetEncryptKey(byChnNo, NULL, 0, 0);
        REC_LASTCODELINE; 
    }
    else  if (CONF_ENCRYPTMODE_DES == byEncryptMode || CONF_ENCRYPTMODE_AES == byEncryptMode) // 加密情况下
    {
        ptVideoEncrypt->GetEncryptKey( abyKeyBuf, &nKeyLen );
        byEncryptMode = (CONF_ENCRYPTMODE_DES == byEncryptMode) ? DES_ENCRYPT_MODE : AES_ENCRYPT_MODE; // 上下层宏定义转换
            
		LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[Info]Encode encrypt Mode: %u KeyLen: %u Actove PT: %u \n",
                        ptVideoEncrypt->GetEncryptMode(), nKeyLen, byActivePt);
        vmplog("[Info]Encode encrypt Mode: %u KeyLen: %u Actove PT: %u \n",
                        ptVideoEncrypt->GetEncryptMode(), nKeyLen, byActivePt);

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
BOOL32 CVMPInst::SetDecryptParam(u8 byChnNo, TMediaEncrypt *ptVideoEncrypt, TDoublePayload *ptDoublePayload)
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

    if ( NULL != m_pcHardMulPic )
    {
        bRet &= m_pcHardMulPic->SetVideoActivePT(byChnNo, ptDoublePayload->GetActivePayload(), ptDoublePayload->GetRealPayLoad());
    }
    else
    {
        bRet &= FALSE;
        vmplog("[SetDecryptParam]SetVideoActivePT failed because m_pcHardMulPic is NULL!\n");
		return bRet;
    }

    REC_LASTCODELINE; 
    byEncryptMode = ptVideoEncrypt->GetEncryptMode();
    if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
    {        
		LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"[Info]Decrypt key to NULL!\n");
        vmplog("[Info]Decrypt key to NULL!\n");
        REC_LASTCODELINE;
        bRet &= m_pcHardMulPic->SetDecryptKey(byChnNo, NULL, 0, 0);
        REC_LASTCODELINE; 
    }
    else  if ( CONF_ENCRYPTMODE_DES == byEncryptMode || CONF_ENCRYPTMODE_AES == byEncryptMode )
    {
        byEncryptMode = (CONF_ENCRYPTMODE_DES == byEncryptMode) ? DES_ENCRYPT_MODE : AES_ENCRYPT_MODE;

        ptVideoEncrypt->GetEncryptKey(abyKeyBuf, &byKenLen);
        REC_LASTCODELINE;
        bRet &= m_pcHardMulPic->SetDecryptKey(byChnNo, (s8*)abyKeyBuf, (u16)byKenLen, byEncryptMode);
        REC_LASTCODELINE; 

		LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"[Info] Decrypt Mode: %u KeyLen: %u Active PT: %u\n",
                ptVideoEncrypt->GetEncryptMode(), 
                byKenLen, 
                ptDoublePayload->GetActivePayload());
        vmplog("[Info] Decrypt Mode: %u KeyLen: %u Active PT: %u\n",
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
void CVMPInst::MsgStopVidMixProc(CMessage * const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);

    switch(CurState())
    {
    case RUNNING:
        break;
    default:
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[Vmp Error]Error State:%d\n", CurState());
        OspPrintf( TRUE, FALSE, "[Vmp Error]Error State:%d\n", CurState());
        return;
    }

    cServMsg.SetMsgBody();
    SendMsgToMcu(VMP_MCU_STOPVIDMIX_ACK, &cServMsg);
    memset(m_tParam, 0, MAXNUM_VMP_ENCNUM * sizeof(C8KEVMPParam));
    m_tCfg.Clear();

    // 停止复合
    s32 nRetCode = m_pcHardMulPic->StopMerge();

    if ( nRetCode )
    {
        cServMsg.SetErrorCode(0);
    }
    else
    {
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"Call stopmerge() error code= %d\n", nRetCode );
         vmplog("Call stopmerge() error code= %d\n", nRetCode );
         cServMsg.SetErrorCode(1);
    }

    memset( m_tParam, 0, MAXNUM_VMP_ENCNUM * sizeof(C8KEVMPParam) );
    m_tStyleInfo.Clear();
    memset( &m_tCapSupportEx, 0, sizeof(TCapSupportEx) );


    for ( u8 byChnIdx = 0; byChnIdx < MAXNUM_VMP_ENCNUM; byChnIdx++ )
    {
        m_pcHardMulPic->SetEncryptKey(byChnIdx, NULL, 0, 0);
        m_tEncEncrypt[byChnIdx].Reset();
    }

    for (u8 byChnNo = 0; byChnNo < m_tCfg.m_tStatus.m_byMemberNum; byChnNo++)
    {        
        m_pcHardMulPic->SetDecryptKey( byChnNo, NULL, 0, 0 );
    }
    
    vmplog("we clear EncryptKey DecryptKey EncryptPT ActivePT Info success!\n");
	LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"we clear EncryptKey DecryptKey EncryptPT ActivePT Info success!\n");

    m_byNeedPrs = FALSE;

    m_cConfId.SetNull();
    m_bDbVid = FALSE;
    m_tCfg.m_tStatus.m_byVMPStyle = VMP_STYLE_NONE;
    m_tCfg.m_tStatus.m_byMemberNum = 0;
    memset( m_abChnlValid, FALSE, sizeof(m_abChnlValid) );
	memset( m_abChnlSetBmp, 0, sizeof(m_abChnlSetBmp) );	
    memset( m_abChnlIframeReqed, 0, sizeof(m_abChnlIframeReqed) );
    memset(m_tVmpMbAlias, 0, sizeof(m_tVmpMbAlias));
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

    cServMsg.SetMsgBody();
    SendMsgToMcu(VMP_MCU_STOPVIDMIX_NOTIF, &cServMsg);

    SendStatusChangeMsg(TRUE, (u8)TVmpStatus::IDLE);

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
void CVMPInst::MsgUpdataVmpEncryptParamProc(CMessage * const pMsg)
{
    if( NULL == pMsg )
	{
		OspPrintf(TRUE, FALSE, "[MsgUpdataVmpEncryptParamProc] Error input parameter.\n");
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP, "[MsgUpdataVmpEncryptParamProc] Error input parameter.\n");
		return;
	}

	u8  byChannelNo = 0;
    TMediaEncrypt  tVideoEncrypt;
    TDoublePayload tDoublePayload;
	u8 byRcvH264DependInMark = 0;  //用于8ke/h/i vmp按特殊方式解码设置（针对华为终端）

    CServMsg cServMsg(pMsg->content, pMsg->length);

    byChannelNo    = *(u8 *)(cServMsg.GetMsgBody());
    tVideoEncrypt  = *(TMediaEncrypt *)(cServMsg.GetMsgBody() + sizeof(u8));
    tDoublePayload = *(TDoublePayload *)(cServMsg.GetMsgBody() + sizeof(u8) + sizeof(TMediaEncrypt));
	byRcvH264DependInMark = *(u8 *)(cServMsg.GetMsgBody() + sizeof(u8) + sizeof(TMediaEncrypt) + sizeof(TDoublePayload));

	LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[UpdateChannel] byChannelNo: %d, ActivePT: %d, RealPT: %d, RcvH264DependInMark: %d\n", 
            byChannelNo,
            tDoublePayload.GetActivePayload(),
            tDoublePayload.GetRealPayLoad(),
			byRcvH264DependInMark);
    vmplog("[UpdateChannel] byChannelNo: %d, ActivePT: %d, RealPT: %d, RcvH264DependInMark: %d\n", 
            byChannelNo,
            tDoublePayload.GetActivePayload(),
            tDoublePayload.GetRealPayLoad(),
			byRcvH264DependInMark);   

	// 参数check，参数有问题则不做处理
	{
		BOOL32 bMsgBuffIsOK = TRUE;
		if (byChannelNo >= MAXNUM_SDVMP_MEMBER)
		{
			OspPrintf(TRUE, FALSE, "[MsgUpdataVmpEncryptParamProc] Error MsgBuff. ChannelNo.%d.\n", byChannelNo);
			LogPrint(LOG_LVL_ERROR,MID_MCU_VMP, "[MsgUpdataVmpEncryptParamProc] Error MsgBuff. ChannelNo.%d.\n", byChannelNo);
			bMsgBuffIsOK = FALSE;
		}
		if (byRcvH264DependInMark != 0 && byRcvH264DependInMark != 1)
		{
			OspPrintf(TRUE, FALSE, "[MsgUpdataVmpEncryptParamProc] Error MsgBuff. RcvH264DependInMark.%d.\n", byRcvH264DependInMark);
			LogPrint(LOG_LVL_ERROR,MID_MCU_VMP, "[MsgUpdataVmpEncryptParamProc] Error MsgBuff. RcvH264DependInMark.%d.\n", byRcvH264DependInMark);
			bMsgBuffIsOK = FALSE;
		}
		if (!bMsgBuffIsOK)
		{
			return;
		}
	}

    SetDecryptParam(byChannelNo, &tVideoEncrypt, &tDoublePayload);
	for( u8 byId = 0; byId < MAXNUM_VMP_ENCNUM; byId ++)
	{
		LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[MsgUpdataVmpEncryptParamProc] setEncryptParam(chnlId.%u, encType.%u)\n", 
			byId,
			m_tParam[byId].m_byEncType);
		vmplog("[MsgUpdataVmpEncryptParamProc] setEncryptParam(chnlId.%u, encType.%u)\n", 
			byId,
			m_tParam[byId].m_byEncType);
		SetEncryptParam(byId, &tVideoEncrypt, m_tParam[byId].m_byEncType);
	}

    if ( !m_abChnlValid[byChannelNo] )
    {
        TNetAddress tNetAddr;
        TNetAddress tNetRtcpAddr;
        TNetAddress tLocalSndRtcpAddr;
        tNetAddr.dwIp = 0;
        tNetRtcpAddr.dwIp = m_tSndAddr[0].dwIp;
		tLocalSndRtcpAddr.dwIp = 0;
        tNetAddr.wPort = g_cVMPApp.m_tEqpCfg.GetEqpStartPort() + byChannelNo*PORTSPAN;        
        tNetRtcpAddr.wPort = GetSendRtcpPort(byChannelNo);     
		tLocalSndRtcpAddr.wPort = g_cVMPApp.m_tEqpCfg.GetEqpStartPort() + byChannelNo*PORTSPAN + 1;
		
        /*s32 nRet = */m_pcHardMulPic->SetChannelParam(byChannelNo, tNetAddr, tNetRtcpAddr, tLocalSndRtcpAddr);
        /*nRet &= */m_pcHardMulPic->SetChannelActive(byChannelNo, TRUE);

        m_abChnlValid[byChannelNo] = TRUE;

    }
	
	if (TRUE == byRcvH264DependInMark)
	{
		m_pcHardMulPic->SetRcvInMarker( byChannelNo, TRUE );
	}
	else
	{
		m_pcHardMulPic->SetRcvInMarker( byChannelNo, FALSE );
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
void CVMPInst::MsgSetBitRate(CMessage* const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);
    u8 byChalNum = cServMsg.GetChnIndex();
    u16 wBitrate;

    cServMsg.GetMsgBody((u8*)&wBitrate, sizeof(u16));
    wBitrate = ntohs(wBitrate);

	LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[Info]now we set the encode-%u bitrate to %u!\n", byChalNum, wBitrate);
    vmplog("[Info]now we set the encode-%u bitrate to %u!\n", byChalNum, wBitrate);
    
    if ( NULL != m_pcHardMulPic  && byChalNum < MAXNUM_VMP_ENCNUM )
    {
		u16 wOldBitrate = m_tParam[byChalNum].GetBitRate();
		if ( wOldBitrate != wBitrate )
		{
			m_pcHardMulPic->ChangeEncBitRate(byChalNum, wBitrate);
			m_tParam[byChalNum].m_wBitRate = wBitrate; 
			TVidEncParam tEncParam;
			memset( &tEncParam, 0, sizeof(tEncParam) );
			Trans2EncParam( &m_tParam[byChalNum], &tEncParam );
			m_pcHardMulPic->SetEncParam(byChalNum,&tEncParam);
			LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"[Info]now we set the encode-%u encparam:width:%d,height:%d,framerate:%d!\n", byChalNum,tEncParam.m_wEncVideoWidth,
				tEncParam.m_wEncVideoHeight,tEncParam.m_byMaxFrameRate);
			vmplog("[Info]now we set the encode-%u encparam:width:%d,height:%d,framerate:%d!\n", byChalNum,tEncParam.m_wEncVideoWidth,
				tEncParam.m_wEncVideoHeight,tEncParam.m_byMaxFrameRate);
			
			
		}
     
				
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

/*====================================================================
	函数  : SetNeedIFrameTimer4VmpChl
	功能  : 针对每个通道定时请求关键帧
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
    日  期         版本        修改人       修改内容
    08/08/19       v4.7      yanghuaizhi     create
====================================================================*/
void CVMPInst::SetNeedIFrameTimer4VmpChl()
{
	// 更新关键帧请求为未发过，对有成员的通道设置关键帧请求Timer
	memset( m_abChnlIframeReqed, 0, sizeof(m_abChnlIframeReqed) );
	for (u8 byChNum = 0;byChNum < MAXNUM_SDVMP_MEMBER;byChNum++)
	{
		if (m_abChnlValid[byChNum] == TRUE)
		{
			SetTimer(EV_VMP_NEEDIFRAME_TIMER+byChNum,CHECK_IFRAME_INTERVAL,byChNum);
		}
	}
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
void CVMPInst::MsgTimerNeedIFrameProc(CMessage * const pMsg)
{
	KillTimer(pMsg->event);
	u8 byChnNo = *(u8 *)pMsg->content;

	if ( byChnNo < MAXNUM_SDVMP_MEMBER)
	{
		CServMsg cServMsg;
		cServMsg.SetConfId(m_cConfId);
		TVidRecvStatis tDecStatis;
		if(m_abChnlValid[byChnNo] && NULL != m_pcHardMulPic)
		{
			memset(&tDecStatis, 0, sizeof(TVidRecvStatis));
			m_pcHardMulPic->GetVidRecvStatis(byChnNo, tDecStatis);
			if (tDecStatis.m_bRequestKeyFrame)
			{			
				m_abChnlIframeReqed[byChnNo]=TRUE;
				cServMsg.SetChnIndex(byChnNo);
				SendMsgToMcu(VMP_MCU_NEEDIFRAME_CMD, &cServMsg);
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"[MsgTimerNeedIFrameProc]vmp channel:%d request iframe!!\n", byChnNo);
				vmplog("[MsgTimerNeedIFrameProc]vmp channel:%d request iframe!!\n", byChnNo);
				LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[MsgTimerNeedIFrameProc]cfg,vmp channel:%d set EV_VMP_NEEDIFRAME_TIMER: %d !\n", byChnNo, g_cVMPApp.m_dwIframeInterval);
				vmplog("[MsgTimerNeedIFrameProc]cfg,vmp channel:%d  set EV_VMP_NEEDIFRAME_TIMER: %d !\n", byChnNo, g_cVMPApp.m_dwIframeInterval);
				SetTimer(pMsg->event, g_cVMPApp.m_dwIframeInterval,byChnNo);
			}
			else
			{
				//配置文件的定时未检测到，则根据标志位判断是否走配置文件 
				if ( m_abChnlIframeReqed[byChnNo]==TRUE )
				{
					LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[MsgTimerNeedIFrameProc]cfg,vmp channel:%d set EV_VMP_NEEDIFRAME_TIMER: %d !\n", byChnNo, g_cVMPApp.m_dwIframeInterval);
					vmplog("[MsgTimerNeedIFrameProc]cfg,vmp channel:%d  set EV_VMP_NEEDIFRAME_TIMER: %d !\n", byChnNo, g_cVMPApp.m_dwIframeInterval);
					SetTimer(pMsg->event, g_cVMPApp.m_dwIframeInterval,byChnNo);
				}
				else
				{
					LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[MsgTimerNeedIFrameProc]vmp channel:%d set EV_VMP_NEEDIFRAME_TIMER: %d !\n", byChnNo, CHECK_IFRAME_INTERVAL);
					vmplog("[MsgTimerNeedIFrameProc]vmp channel:%d  set EV_VMP_NEEDIFRAME_TIMER: %d !\n", byChnNo, CHECK_IFRAME_INTERVAL);
					SetTimer(pMsg->event, CHECK_IFRAME_INTERVAL,byChnNo);
				}			
			}
		
		} 
	}
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
void CVMPInst::MsgFastUpdatePicProc(CMessage * const pMsg)
{
	CServMsg cServMsg( pMsg->content, pMsg->length );
	u8 byChnnlId = cServMsg.GetChnIndex();
	// 参数判断，不正确不做处理
	if (byChnnlId >= MAXNUM_VMP_ENCNUM)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP, "[MsgFastUpdatePicProc]MsgBuff Err, byChnlId.%d!\n", byChnnlId);
		vmplog("[MsgFastUpdatePicProc]MsgBuff Err, byChnlId.%d!\n", byChnnlId);
		return;
	}

    u32 dwTimeInterval = (u32)(OspClkRateGet()*0.5);
    
    u32 dwCurTick = OspTickGet();	
    if( dwCurTick - m_dwLastFUPTick > dwTimeInterval)
    {		
        m_dwLastFUPTick = dwCurTick;
        if ( NULL != m_pcHardMulPic)
        {
            m_pcHardMulPic->SetFastUpdata(byChnnlId);
            vmplog("[MsgFastUpdatePicProc]m_pcHardMulPic->SetFastUpdata(%d)!\n", byChnnlId);
        }
        else
        {
            vmplog("[MsgFastUpdatePicProc]m_pcHardMulPic is NULL!\n");
        }
		
		/*
        m_pcHardMulPic->SetFastUpdata(0);
        if (m_bDbVid)
        {
            m_pcHardMulPic->SetFastUpdata(1);
        }
        */
        
    }
	return;
}

void CVMPInst::ProcChangeMemAliasCmd(CMessage * const pMsg)
{
    if (NULL == m_pcHardMulPic)
    {
        vmplog( "[ProcChangeMemAliasCmd]m_pcHardMulPic is NULL\n");
        return;
    }

	CServMsg cServMsg( pMsg->content, pMsg->length );
	u8 byChnNo = cServMsg.GetChnIndex();

	BOOL32 bNeedDisplayAlias = ( 1 == m_tParam[0].m_byIsDisplayMmbAlias )? TRUE: FALSE;
	
	// 获取别名与通道，支持别名扩容，支持多通道同时更新
	s8 *pchAlias;
	u16 wMsgLen = 0;
	do 
	{
		pchAlias = (s8*)(cServMsg.GetMsgBody() + wMsgLen);
		if (strlen(pchAlias) > 0)
		{
			wMsgLen += MAXLEN_ALIAS;//别名长度
			byChnNo = *(cServMsg.GetMsgBody() + wMsgLen);
			wMsgLen++;
			if (byChnNo >= MAXNUM_SDVMP_MEMBER)
			{
				// 通道号校验
				break;
			}
			// 保存通道终端别名
			m_tVmpMbAlias[byChnNo].SetMbAlias(strlen(pchAlias), pchAlias);
			
			LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP, "[ProcChangeMemAliasCmd]Mmb.%d alias is %s bNeedDisplayAlias%d\n", 
				byChnNo, pchAlias, bNeedDisplayAlias );
			vmplog( "[ProcChangeMemAliasCmd]Mmb.%d alias is %s bNeedDisplayAlias%d\n", byChnNo, pchAlias, bNeedDisplayAlias );
			
			SetLogo( byChnNo, bNeedDisplayAlias, pchAlias );
		}
		else
		{
			wMsgLen += MAXLEN_ALIAS;//别名长度
			wMsgLen++;
		}
	} while (cServMsg.GetMsgBodyLen() > wMsgLen);
	
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
void CVMPInst::MsgChnnlLogOprCmd( CMessage * const pMsg )
{
    if ( NULL == m_pcHardMulPic )
    {
        vmplog( "[MsgChnnlLogOprCmd] m_pcHardMulPic is NULL\n" );
        return;
    }

    CServMsg cServMsg( pMsg->content, pMsg->length );

    u8 byIsAddLog = *(u8*)cServMsg.GetMsgBody();
    BOOL32 IsDisplayMmbAlias = ( 1 == byIsAddLog ) ? TRUE : FALSE;

    BOOL32 bRetOk = FALSE;
    for ( u8 byChnNo = 0; byChnNo < m_tParam[0].m_byMemberNum; byChnNo++)
    {
        if ( !m_tParam[0].m_atMtMember[byChnNo].IsNull() )
        {
            REC_LASTCODELINE;
            SetLogo( byChnNo, IsDisplayMmbAlias, (s8*)m_tVmpMbAlias[byChnNo].GetMbAlias() );
            REC_LASTCODELINE;
        }       
    }
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
void CVMPInst::MsgGetVidMixParamProc(CMessage* const pMsg)
{
    return;
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
void CVMPInst::StatusShow()
{
    if ( NULL == m_pcHardMulPic )
    {
        ::OspPrintf( TRUE, FALSE, "m_pcHardMulPic is NULL, Can not get status!\n");
        return;
    }

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
    ::OspPrintf(TRUE, FALSE, "Following are the params that mcu send to the vmp:\n\n");

	for (u8 byIdx = 0; byIdx < MAXNUM_VMP_ENCNUM; byIdx++)
	{	
		::OspPrintf(TRUE, FALSE, "Chn%d: EncType=%s BitRate=%u Video: %u*%u \n",
					byIdx, 
					GetMediaString(m_tParam[byIdx].m_byEncType), m_tParam[byIdx].m_wBitRate, 
                    m_tParam[byIdx].m_wVideoWidth, m_tParam[byIdx].m_wVideoHeight);
	}
    
    
    ::OspPrintf(TRUE, FALSE, "\t============================================================\n\n");

    ::OspPrintf(TRUE, FALSE, "\t===========The state of VMP is below===============\n");

    ::OspPrintf(TRUE, FALSE, "Codec Init OK? %s\n", g_cVMPApp.m_bInitOk ? "YES" : "NO" );
	
	::OspPrintf(TRUE, FALSE, "The MTU Size is : %d\n", m_wMTUSize );

    ::OspPrintf(TRUE, FALSE, "  IDLE =%d, NORMAL=%d, RUNNING=%d, VMP Current State =%d\n",
	                          IDLE, NORMAL, RUNNING, CurState());

    ::OspPrintf(TRUE, FALSE, "\t===========The status of VMP is below===============\n");
    ::OspPrintf(TRUE, FALSE, "CPing=%s\t Style=%s(%d)\t ChannelNum=%d(Real=%d), EncNum=%d(Real=%d)\n",
                              tMulPicStatus.bMergeStart ? "Y" : "N",
                              VmpHardStyleStr(tMulPicStatus.dwMergeType), tMulPicStatus.dwMergeType,
                              tMulPicStatus.byChnNum,
                              m_tParam[0].m_byMemberNum,
                              tMulPicStatus.byEncNum,
                              m_bDbVid ? 2 : 1);
    s32 nLoop = 0;
    for (nLoop =0; nLoop < tMulPicStatus.byChnNum; nLoop++)
    {    
        ::OspPrintf(TRUE, FALSE, "Receive Channel(%2d) Valid: %d  Port:%d\n",
                    nLoop, m_abChnlValid[nLoop], tMulPicStatus.atNetRcvAddr[nLoop].wPort);
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
void CVMPInst::ParamShow(void)
{
	::OspPrintf(TRUE, FALSE, "\t===========================The VC Param of VMP is below==========================\t\n");
	for (u8 byIdx = 0; byIdx < MAXNUM_VMP_ENCNUM; byIdx++)
	{
		::OspPrintf(TRUE, FALSE, "\t----------------------------- chnnl %d -----------------------------\t\n", byIdx+1);
		m_tParam[byIdx].Print();
		OspPrintf(TRUE,FALSE,"ProfileType:%s",(m_abyChanProfileType[byIdx] == (u8)emBpAttrb)?"BP":"HP");
	    ::OspPrintf(TRUE, FALSE, "\n");
		if (byIdx == MAXNUM_VMP_ENCNUM - 1)
		{
			::OspPrintf(TRUE, FALSE, "\t================================================================================\t\n");
		}
	}
	::OspPrintf(TRUE, FALSE, "\t===========================Debug info==========================\t\n");
	m_tDbgVal.Print();
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
void CVMPInst::SendStatusChangeMsg( u8 byOnline, u8 byState )
{
    CServMsg cServMsg;
    TPeriEqpStatus tInitStatus;
    memset(&tInitStatus,0,sizeof(TPeriEqpStatus));

    tInitStatus.m_byOnline = byOnline;
    tInitStatus.SetMcuEqp((u8)g_cVMPApp.m_tEqpCfg.GetMcuId(), g_cVMPApp.m_tEqpCfg.GetEqpId(), g_cVMPApp.m_tEqpCfg.GetEqpType());
    tInitStatus.SetAlias(g_cVMPApp.m_tEqpCfg.GetAlias());
    tInitStatus.m_tStatus.tVmp.m_byUseState = byState;

    cServMsg.SetMsgBody((u8*)&(tInitStatus), sizeof(TPeriEqpStatus));
    SendMsgToMcu(VMP_MCU_VMPSTATUS_NOTIF, &cServMsg);
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
void CVMPInst::ProcSetQosInfo(CMessage* const pMsg)
{
    if(NULL == pMsg)
    {
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[Mixer] The Mix's Qos infomation is Null\n");
        OspPrintf(TRUE, FALSE, "[Mixer] The Mix's Qos infomation is Null\n");
        return;
    }

    TMcuQosCfgInfo  tQosInfo;
    CServMsg cServMsg(pMsg->content, pMsg->length);
    tQosInfo = *(TMcuQosCfgInfo*)cServMsg.GetMsgBody();

    u8 byQosType = tQosInfo.GetQosType();
    u8 byAudioValue = tQosInfo.GetAudLevel();
    u8 byVideoValue = tQosInfo.GetVidLevel();
    u8 byDataValue = tQosInfo.GetDataLevel();
    u8 byIpPriorValue = tQosInfo.GetIpServiceType();
    if(QOSTYPE_IP_PRIORITY == byQosType)
    {
        byAudioValue = (byAudioValue << 5);
        byVideoValue = (byVideoValue << 5);
        byDataValue = (byDataValue << 5);
        ComplexQos(byAudioValue, byIpPriorValue);
        ComplexQos(byVideoValue, byIpPriorValue);
        ComplexQos(byDataValue, byIpPriorValue);
    }
//     else
//     {
//         byAudioValue = (byAudioValue << 2);
//         byVideoValue = (byVideoValue << 2);
//         byDataValue = (byDataValue << 2);
//     }

//     s32 nRet = SetMediaTOS((s32)byAudioValue, QOS_AUDIO);
//     nRet = SetMediaTOS((s32)byVideoValue, QOS_VIDEO);
//     nRet = SetMediaTOS((s32)byDataValue, QOS_DATA);

    return;
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
void CVMPInst::ComplexQos(u8& byValue, u8 byPrior)
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
void CVMPInst::ProcSetStyleMsg(CMessage* const pMsg)
{
    TVmpStyleCfgInfo  tStyleInfo;
    CServMsg cStyleInfo(pMsg->content, pMsg->length);

    cStyleInfo.GetMsgBody((u8*)&tStyleInfo, sizeof(TVmpStyleCfgInfo));
    BOOL32 bRet = SetVmpAttachStyle(tStyleInfo, m_tParam[0]);
    if(FALSE == bRet)
    {
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP, "[Error] set vmp attach style failed !\n");
        vmplog( "[Error] set vmp attach style failed !\n");
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
BOOL32 CVMPInst::SetVmpAttachStyle(TVmpStyleCfgInfo&  tStyleInfo, C8KEVMPParam & tParam )
{
    // 这里不保存，由调用该函数的地方在返回成功后再保存，zgc，2009-07-09
    //memcpy((u8*)&m_tStyleInfo, (void*)&tStyleInfo, sizeof(TVmpStyleCfgInfo));
    
    TMulPicColor tBGDColor;
	TMulPicColor tBGFrameColor;
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
	// 不管是否使用边框，背景边框色统一用白色，不用背景色（边框色分背景边框色与前景边框色）
	// 前景边框色在无码流时无法显示，无码流通道无法显示为白色边框色
	//tBGFrameColor = tBGDColor; 
	tBGFrameColor.RColor = 0xff;
	tBGFrameColor.GColor = 0xff;
	tBGFrameColor.BColor = 0xff;
    TDrawFrameColor atSlidelineColor[PIC_MERGE_NUM_MAX];
    memset(atSlidelineColor, 0, sizeof(atSlidelineColor));

    for(u8 byLoop = 0; byLoop < tParam.m_byMemberNum; byLoop++)
    {
        if ( !tStyleInfo.GetIsRimEnabled() )
        {
			//标准边框颜色（无边框）现在改为默认的白色
//             atSlidelineColor[byLoop].tMulPicColor.RColor = 0xff;
//             atSlidelineColor[byLoop].tMulPicColor.GColor = 0xff;
//             atSlidelineColor[byLoop].tMulPicColor.BColor = 0xff;
            atSlidelineColor[byLoop].bShow  = FALSE;
            continue;
        }
		//空闲通道背景颜色用默认的白色
		if ( tParam.m_atMtMember[byLoop].IsNull() )
		{
			atSlidelineColor[byLoop].tMulPicColor.RColor = 0xff;
            atSlidelineColor[byLoop].tMulPicColor.GColor = 0xff;
            atSlidelineColor[byLoop].tMulPicColor.BColor = 0xff;
			vmplog("The channel: %d, The Idle Color set default rgb 0xff\n", byLoop);
			continue;
		}

        atSlidelineColor[byLoop].bShow  = TRUE;
        if(MT_STATUS_CHAIRMAN == tParam.m_atMtMember[byLoop].GetMemStatus())
        {
            atSlidelineColor[byLoop].tMulPicColor.RColor = byRColorChair;
            atSlidelineColor[byLoop].tMulPicColor.GColor = byGColorChair;
            atSlidelineColor[byLoop].tMulPicColor.BColor = byBColorChair;
            vmplog("The channel: %d, The Chairman Color Value: %0x\n", byLoop, dwChairColor);
        }
        if(MT_STATUS_SPEAKER == tParam.m_atMtMember[byLoop].GetMemStatus())
        {
            atSlidelineColor[byLoop].tMulPicColor.RColor = byRColorSpeaker;
            atSlidelineColor[byLoop].tMulPicColor.GColor = byGColorSpeaker;
            atSlidelineColor[byLoop].tMulPicColor.BColor = byBColorSpeaker;
            vmplog("The channel: %d, The Speaker  Color Value: %0x\n", byLoop, dwSpeakerColor);
        }
        if(MT_STATUS_AUDIENCE == tParam.m_atMtMember[byLoop].GetMemStatus())
        {
            atSlidelineColor[byLoop].tMulPicColor.RColor = byRColorAudience;
            atSlidelineColor[byLoop].tMulPicColor.GColor = byGColorAudience;
            atSlidelineColor[byLoop].tMulPicColor.BColor = byBColorAudience;
            vmplog("The channel: %d, The Audience Color Value: %0x\n", byLoop, dwAudienceColor);
        }


    }
	LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP, "The Background Color Value: 0x%x\n", tStyleInfo.GetBackgroundColor() );
	LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP, "The RGB Color of Chairman : R<%d> G<%d> B<%d> \n", byRColorChair,    byGColorChair,    byBColorChair );
	LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP, "The RGB Color of Speaker  : R<%d> G<%d> B<%d> \n", byRColorSpeaker,  byGColorSpeaker,  byBColorSpeaker );
	LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP, "The RGB Color of Audience : R<%d> G<%d> B<%d> \n", byRColorAudience, byGColorAudience, byBColorAudience );
	LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP, "The RGB Color of Backgroud: R<%d> G<%d> B<%d> \n", tBGDColor.RColor, tBGDColor.GColor, tBGDColor.BColor );
	LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP, "The RGB Color of Backgroud: R<%d> G<%d> B<%d> \n", tBGFrameColor.RColor, tBGFrameColor.GColor, tBGFrameColor.BColor );
    vmplog("The Background Color Value: 0x%x\n", tStyleInfo.GetBackgroundColor() );
    vmplog("The RGB Color of Chairman : R<%d> G<%d> B<%d> \n", byRColorChair,    byGColorChair,    byBColorChair );
    vmplog("The RGB Color of Speaker  : R<%d> G<%d> B<%d> \n", byRColorSpeaker,  byGColorSpeaker,  byBColorSpeaker );
    vmplog("The RGB Color of Audience : R<%d> G<%d> B<%d> \n", byRColorAudience, byGColorAudience, byBColorAudience );
    vmplog("The RGB Color of Backgroud: R<%d> G<%d> B<%d> \n", tBGDColor.RColor, tBGDColor.GColor, tBGDColor.BColor );
	vmplog("The RGB Color of BackgroudFrame: R<%d> G<%d> B<%d> \n", tBGFrameColor.RColor, tBGFrameColor.GColor, tBGFrameColor.BColor );
	
    s32 nRet = 0;

	//画面合成风格加边框
    REC_LASTCODELINE;
    nRet = m_pcHardMulPic->SetBGDAndSidelineColor(tBGDColor, tBGFrameColor, atSlidelineColor);
    REC_LASTCODELINE;

    if(!nRet)
    {
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[vmp] Fail to call the SetBGDAndSidelineColor.\n");
        OspPrintf(TRUE, FALSE, "[vmp] Fail to call the SetBGDAndSidelineColor.\n");
        //return FALSE; //Always return TRUE
    }
    else
    {
		LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"Set VMP Scheme:%d with RimEnabled:%d successfully !\n", tStyleInfo.GetSchemeId(), tStyleInfo.GetIsRimEnabled());
        vmplog("Set VMP Scheme:%d with RimEnabled:%d successfully !\n", tStyleInfo.GetSchemeId(), tStyleInfo.GetIsRimEnabled());
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
u8 CVMPInst::GetRColor(u32 dwColorValue)
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
u8 CVMPInst::GetGColor(u32 dwColorValue)
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
u8 CVMPInst::GetBColor(u32 dwColorValue)
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
void CVMPInst::GetRGBColor(u32 dwColor, u8& byRColor, u8& byGColor, u8& byBColor)
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
u16 CVMPInst::GetSendRtcpPort(u8 byChanNo)
{
    if (byChanNo >= 8)
    {
        return (m_tSndAddr[0].wPort + 4 + byChanNo); 
    }
    else
    {
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
void CVMPInst::SetSmoothSendRule(u32 dwDestIp, u16 wPort, u32 dwRate)
{
    dwRate = dwRate >> 3;   // bit 2 byte
    
    u32 dwPeak = 0;

    dwPeak = dwRate + dwRate / 5; //add 20%, default
	LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"[SetSmoothSendRule] Setting ssrule for 0x%x:%d, rate/peak=%u/%u KByte.\n",
              dwDestIp, wPort, dwRate, dwPeak );
    vmplog( "[SetSmoothSendRule] Setting ssrule for 0x%x:%d, rate/peak=%u/%u KByte.\n",
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
void CVMPInst::ClearSmoothSendRule(u32 dwDestIp, u16 wPort)
{
	LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"[ClearSmoothSendRule] Unsetting ssrule for 0x%x:%d.\n",
            dwDestIp, wPort );
    vmplog( "[ClearSmoothSendRule] Unsetting ssrule for 0x%x:%d.\n",
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
void CVMPInst::StopSmoothSend()
{
	LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"[StopSmoothSend] Stopping SmoothSend\n" );
    vmplog( "[StopSmoothSend] Stopping SmoothSend\n" );

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
u8 CVMPInst::GetVmpChlNumByStyle( u8 byVMPStyle )
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
	case VMP_STYLE_FIFTEEN:
        byMaxMemNum = 15;
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
u8 CVMPInst::VmpGetActivePayload(u8 byRealPayloadType)
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
BOOL32 CVMPInst::IsSameEncParam( TVidEncParam * ptEncParam1, TVidEncParam * ptEncParam2 )
{
    if ( NULL == ptEncParam1 || NULL == ptEncParam2 )
    {
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[IsSameEncParam] Param error! ptEncParam1 = 0x%x, ptEncParam2 = 0x%x!\n", ptEncParam1, ptEncParam2 );
        OspPrintf( TRUE, FALSE, "[IsSameEncParam] Param error! ptEncParam1 = 0x%x, ptEncParam2 = 0x%x!\n", ptEncParam1, ptEncParam2 );
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
参    数： const CKDVVMPParam * ptVmpParam
           TVidEncParam * ptEncparam
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/8/18   4.0			周广程                  创建
=============================================================================*/
void CVMPInst::Trans2EncParam(  C8KEVMPParam * ptVmpParam, TVidEncParam * ptEncparam )
{
    if ( NULL == ptVmpParam || NULL == ptEncparam )
    {
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[Trans2EncParam] Param error! ptVmpParam = 0x%x, ptEncParam = 0x%x!\n", ptVmpParam, ptEncparam );
        OspPrintf( TRUE, FALSE, "[Trans2EncParam] Param error! ptVmpParam = 0x%x, ptEncParam = 0x%x!\n", ptVmpParam, ptEncparam );
        return;
    }
	//根据码率调整分辨率和帧率
	u8 byRes = GetResViaWH(ptVmpParam->m_wVideoWidth, ptVmpParam->m_wVideoHeight);
	u8 byFramerate = ptVmpParam->m_byFrameRate;
	BOOL32 bGetRet = g_tResBrLimitTable.GetRealResFrameRatebyBitrate(ptVmpParam->m_byEncType,byRes,
		byFramerate,ptVmpParam->m_wBitRate);
	//查表找对应码率的分辨率，帧率不成功
	if (!bGetRet)
	{
		OspPrintf(TRUE,FALSE,"[Trans2EncParam]ERROR: GetRealResFrameratebyBitrate return FALSE\n");
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[Trans2EncParam]ERROR: GetRealResFrameratebyBitrate return FALSE\n");
	}
	else//成功，打印
	{
		LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"[Trans2EncParam]GetRealResFrameRatebyBitrate:byRes:%d,framerate:%d\n",byRes,byFramerate);
	}

	u16 wWidth = 0;
	u16 wHeight = 0;
	GetWHViaRes(byRes,wWidth,wHeight);
	//mp4 4cif要下720 576而不能用704 576
	if( VIDEO_FORMAT_4CIF == byRes && MEDIA_TYPE_MP4 == ptVmpParam->m_byEncType )
	{
		wWidth = 720;
		wHeight = 576;
	}
    ptEncparam->m_wEncVideoWidth = wWidth;
    ptEncparam->m_wEncVideoHeight = wHeight;
    ptEncparam->m_byEncType = ptVmpParam->m_byEncType;
    ptEncparam->m_dwBitRate = ptVmpParam->m_wBitRate;
	//20110411 zhouyiliang 增加最大帧率和最小帧率赋值
	ptEncparam->m_byMaxFrameRate = byFramerate;
	ptEncparam->m_byMinFrameRate = byFramerate;

    TVidEncParam tTmpEncParam = *ptEncparam;
	
	m_tCfg.GetDefaultParam( ptEncparam->m_byEncType, tTmpEncParam );
	// [8/20/2010 xliang] special modify
// 	if( ptVmpParam->m_byEncType == MEDIA_TYPE_H264 )
// 	{
// 		if(ptVmpParam->m_wVideoWidth == 1280 && ptVmpParam->m_wVideoHeight == 720 )
// 		{
// 			tTmpEncParam.m_dwMaxKeyFrameInterval = 3000;
// 		}
// 		else if (ptVmpParam->m_wVideoWidth == 704 && ptVmpParam->m_wVideoHeight == 576)
// 		{
// 			tTmpEncParam.m_dwMaxKeyFrameInterval = 1000;
// 		}
// 	}
	

    *ptEncparam = tTmpEncParam;

    return;
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
s8 CVMPInst::GetEncChnIdxByParam( TVidEncParam * ptEncParam )
{
    if ( NULL == ptEncParam )
    {
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[GetEncChnIdxByParam] Param is error! ptEncParam = 0x%x!\n", ptEncParam );
        OspPrintf( TRUE, FALSE, "[GetEncChnIdxByParam] Param is error! ptEncParam = 0x%x!\n", ptEncParam );
        return -1;
    }
    // 获得当前合成器实际状态
    TMulPicStatus tMulPicStatus;
    if ( m_pcHardMulPic == NULL || !m_pcHardMulPic->GetStatus(tMulPicStatus) )
    {
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[GetEncChnIdxByParam] Get vmp current MulPicStatus failed!\n" );
        OspPrintf( TRUE, FALSE, "[GetEncChnIdxByParam] Get vmp current MulPicStatus failed!\n" );
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

/*=============================================================================
函 数 名： SetOutputChnnlActive
功    能： 停，开单通道处理函数
算法实现： 
全局变量： 
参    数： u8 byOutChnlIdx：要停、开的输出通道号
			BOOL32 bActive ：TRUE 开输出通道
							 FALSE 停输出通道
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2011/3/31   4.0			周翼亮                  创建
=============================================================================*/
BOOL32 CVMPInst::SetOutputChnnlActive(u8 byOutChnlIdx,BOOL32 bActive)
{

	if ( NULL == m_pcHardMulPic || byOutChnlIdx >= MAXNUM_VMP_ENCNUM )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[SetOutputChnnlActive] invalid OutChnl:%d or m_pcHardMulPic =NULL\n ",byOutChnlIdx );
		vmplog("[SetOutputChnnlActive] invalid OutChnl:%d or m_pcHardMulPic =NULL\n ",byOutChnlIdx );
		return FALSE;
	}

	return m_pcHardMulPic->SetEncActive(byOutChnlIdx,bActive);
}

/*=============================================================================
函 数 名： ProcStartStopChnnlCmd
功    能： 处理mcu发过来的停，开单通道消息
算法实现： 
全局变量： 
参    数： CMessage mcu发过来的消息体
返 回 值： void
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2011/3/31   4.0			周翼亮                  创建
=============================================================================*/
void CVMPInst::ProcStartStopChnnlCmd( CMessage * const pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	u8 abyActive[MAXNUM_VMP_ENCNUM] = {0};
	memcpy(abyActive,cServMsg.GetMsgBody(),MAXNUM_VMP_ENCNUM);
	
	for (u8 byLoop = 0; byLoop < MAXNUM_VMP_ENCNUM; byLoop++)
	{
		
		if ( (abyActive[byLoop] != (u8)emVmpOutChnlInactive) && (abyActive[byLoop] != (u8)emVmpOutChnlActive)) 
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[ProcStartStopChnnlCmd] chanel:%d keep status ,continue\n",byLoop);
			continue;
		}
		BOOL32 bActive = (abyActive[byLoop] == (u8)emVmpOutChnlInactive)?FALSE:TRUE;
		if ( !SetOutputChnnlActive(byLoop,bActive) ) 
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[ProcStartStopChnnlCmd] SetOutputChnnlActive return FALSE\n");
			vmplog("[ProcStartStopChnnlCmd] SetOutputChnnlActive return FALSE\n");
		}
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
void CVMPInst::MsgAddRemoveRcvChnnl(CMessage * const pMsg)
{
	CServMsg cServMsg( pMsg->content, pMsg->length );
	u8 byChnNo = cServMsg.GetChnIndex();
//	u8 byNoUse = *cServMsg.GetMsgBody();
	u8 byAdd = *(cServMsg.GetMsgBody() + 1);
	
	// 参数判断，若参数不正确，则不做处理
	if (byChnNo >= MAXNUM_SDVMP_MEMBER)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[MsgAddRemoveRcvChnnl] MsgBuff Err, ChnNo.%d\n", byChnNo );
        OspPrintf(TRUE, FALSE, "[MsgAddRemoveRcvChnnl] MsgBuff Err, ChnNo.%d\n", byChnNo ); 
		return;
	}
	if (byAdd !=0 && byAdd != 1)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[MsgAddRemoveRcvChnnl] MsgBuff Err, byAdd.%d\n", byAdd );
        OspPrintf(TRUE, FALSE, "[MsgAddRemoveRcvChnnl] MsgBuff Err, byAdd.%d\n", byAdd ); 
		return;
	}

	if (byAdd == 0)
	{
		if (m_abChnlValid[byChnNo] && NULL != m_pcHardMulPic )
        {
            REC_LASTCODELINE; 
			// 关通道
            if (!m_pcHardMulPic->SetChannelActive(byChnNo, FALSE) )
            {
				LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[vmp] Fail to SetChannelActive, chn.%d\n", byChnNo );
                OspPrintf(TRUE, FALSE, "[vmp] Fail to SetChannelActive, chn.%d\n", byChnNo); 
            }
            else
            {
                LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[ChangeCP]Remove channel %d.\n", byChnNo);
				vmplog("[ChangeCP]Remove channel %d.\n", byChnNo);
                m_pcHardMulPic->ClearChannelCache( byChnNo );
				m_abChnlValid[byChnNo] = FALSE;
				//设置边框和颜色
				m_tParam[0].m_atMtMember[byChnNo].SetNull();
				m_tParam[0].m_tDoublePayload[byChnNo].Reset();
				if ( !SetVmpAttachStyle( m_tStyleInfo, m_tParam[0]) )
				{
					LogPrint(LOG_LVL_ERROR,MID_MCU_VMP," Failed to set vmp attach style (MsgAddRemoveRcvChnnl)\n");
					return;
				}
            }
            REC_LASTCODELINE;      
			//更新别名，刷台标
			memset(&m_tVmpMbAlias[byChnNo], 0, sizeof(m_tVmpMbAlias[byChnNo]));
			SetLogo(byChnNo, FALSE);
            REC_LASTCODELINE;
        }
	}
	else
	{
		//暂时不支持8kh/8ke ADD
	}
}

/*====================================================================
    函数名      : SetLogo
    功能        : 对媒控SetLogo及SetLogoActive的封装
    算法实现    :
    引用全局变量:
    输入参数说明: 
    返回值说明  :
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人      修改内容
    2013/06/09              liaokang      创建
====================================================================*/
void CVMPInst::SetLogo(u8 byChnNo, BOOL32 bDisplay ,s8 *pszBuf)
{
    if( byChnNo >= MAXNUM_SDVMP_MEMBER  )
    {     
        LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[SetLogo] Param error! byChnnl:%d\n", byChnNo );
        vmplog("[SetLogo] Param error! byChnnl:%d\n", byChnNo );        
        return;
    }

    BOOL32 bRet = FALSE;
    BOOL32 bLogoActive = FALSE;    
    if ( TRUE == bDisplay )
    { 
        const s8 *pchAlias = "(none)";
        if( NULL != pszBuf && 
            0 != strlen(pszBuf) &&
            0 != strcmp(pchAlias, pszBuf) )
        {            
            LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"Mmb.%d alias is %s, aliaslen.%d\n", byChnNo, pszBuf, strlen(pszBuf) );
            vmplog( "Mmb.%d alias is %s, aliaslen.%d\n", byChnNo, pszBuf, strlen(pszBuf) );
            
            TDrawParam tDrawParam;
            tDrawParam.SetText(pszBuf);
            tDrawParam.SetFontSize(FONT_SIZE16);
            tDrawParam.SetDrawMode(USER_DEFINED_SIZE_MODE);
            tDrawParam.SetPicHeight(28);
            tDrawParam.SetPicWidth(600);
            TPic tPic;
            bRet = m_cBmpDrawer.DrawBmp(tPic, tDrawParam);
            if (!bRet)
            {
                LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[SetLogo] DrawBmp error, byChnnl:%d\n", byChnNo );
                vmplog( "[SetLogo] DrawBmp error, byChnnl:%d\n", byChnNo );
            }
            else
            {
                LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"[SetLogo] Chnl:%d,drawbmp succeed,width=%d,height=%d\n", byChnNo, tPic.GetPicWidth(),tPic.GetPicHeight());
                vmplog( "[SetLogo] Chnl:%d,drawbmp succeed,width=%d,height=%d\n", byChnNo, tPic.GetPicWidth(), tPic.GetPicHeight());            
                
                bRet = m_pcHardMulPic->SetLogo( byChnNo, (u8*)tPic.GetPic(), tPic.GetPicSize(), tPic.GetMinTextPicWidth() );
                if(bRet)
                {
                    bLogoActive = TRUE;
                }        
                LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"[SetLogo] Chnnl:%d m_pcHardMulPic->SetLogo = %d\n", byChnNo, bRet);
                vmplog("[SetLogo] Chnnl:%d m_pcHardMulPic->SetLogo = %d\n", byChnNo, bRet);
            }
        }
    } 

    bRet = m_pcHardMulPic->SetLogoActive( byChnNo, bLogoActive );
    LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_VMP,"[SetLogo] Chnnl:%d bDisplay:%d LogoActive:%d m_pcHardMulPic->SetLogoActive %d\n", byChnNo, bDisplay, bLogoActive, bRet);
    vmplog("[SetLogo] Chnnl:%d bDisplay:%d LogoActive:%d m_pcHardMulPic->SetLogoActive %d\n", byChnNo, bDisplay, bLogoActive, bRet); 
    return;
}

//
///*=============================================================================
//函 数 名： ProcStartStopAllChnnlCmd
//功    能： 处理mcu发过来的停，开所有通道消息
//算法实现： 
//全局变量： 
//参    数： CMessage mcu发过来的消息体
//返 回 值： void
//----------------------------------------------------------------------
//修改记录    ：
//日  期		版本		修改人		走读人    修改内容
//2011/4/11   4.0			周翼亮                  创建
//=============================================================================*/
//void CVMPInst::ProcStartStopAllChnnlCmd( CMessage * const pcMsg )
//{
//	CServMsg cServMsg(pcMsg->content, pcMsg->length);
//	u8 byActive = *(u8*)cServMsg.GetMsgBody();
//	BOOL32 bActive = (byActive == 0)?FALSE:TRUE;
//	for (u8 byLoop = 0; byLoop < MAXNUM_VMP_ENCNUM; byLoop++)
//	{
//		if ( !SetOutputChnnlActive(byLoop,bActive) ) 
//		{
//			LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[ProcStartStopChnnlCmd] SetOutputChnnlActive return FALSE\n");
//			vmplog("[ProcStartStopChnnlCmd] SetOutputChnnlActive return FALSE\n");
//		}
//	}
//	
//	
//}



//=============================================================================
//函 数 名： ProcChangeSingleChnlEncParam
//功    能： 处理mcu发过来修改单独编码通道参数的消息
//算法实现： 
//全局变量： 
//参    数： CMessage mcu发过来的消息体
//返 回 值： void
//----------------------------------------------------------------------
//修改记录    ：
//日  期		版本		修改人		走读人    修改内容
//2011/06/29   4.0			周翼亮                  创建
//=============================================================================void CVMPInst::ProcChangeSingleChnlEncParam(CMessage* const pcMsg)
//{
//	CServMsg cServMsg(pcMsg->content, pcMsg->length);
//	
//	u8 * pbyMsgBuf = cServMsg.GetMsgBody();
//	u8 byChnnlNum = *(u8*)pbyMsgBuf;
//	pbyMsgBuf += sizeof(u8);
//	C8KEVMPParam cVmpParam;
//	memcpy(&cVmpParam,pbyMsgBuf,sizeof(C8KEVMPParam));
//	cVmpParam.m_wBitRate = ntohs(cVmpParam.m_wBitRate);
//    cVmpParam.m_wVideoWidth = ntohs(cVmpParam.m_wVideoWidth);
//    cVmpParam.m_wVideoHeight = ntohs(cVmpParam.m_wVideoHeight);
//
//    if ( 0 == cVmpParam.m_wBitRate )
//    {
//        cVmpParam.m_wBitRate = 256;
//    }
//	
//	//mpeg 4cif = (720 * 576), not (704 * 576)
//	if( MEDIA_TYPE_MP4 == cVmpParam.m_byEncType && 704 == cVmpParam.m_wVideoWidth )
//	{
//		cVmpParam.m_wVideoWidth = 720;
//	}
//
//
//		
//    // 视频编码参数格式转换
//    TVidEncParam atEncParam;
//    memset( &atEncParam, 0, sizeof(TVidEncParam) );
//    Trans2EncParam( &cVmpParam, &atEncParam );
//        
//    // 获得当前合成器实际状态
//    TMulPicStatus tMulPicStatus;
//    if ( !m_pcHardMulPic->GetStatus(tMulPicStatus) )
//    {
//		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[ProcChangeSingleChnlEncParam] Get vmp current MulPicStatus failed!\n"  );
//        OspPrintf( TRUE, FALSE, "[ProcChangeSingleChnlEncParam] Get vmp current MulPicStatus failed!\n" );
//        SendMsgToMcu(cServMsg.GetEventId()+2, &cServMsg);
//        return;
//    }
//
//    BOOL32 bRetOk = TRUE;   
//    // 还没开始合成，设置单独某一通道req不予理会
//    if ( !tMulPicStatus.bMergeStart )
//    {
//      	LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[ProcChangeSingleChnlEncParam] vmp current MulPicStatus mergestart:FALSE!\n"  );
//        OspPrintf( TRUE, FALSE, "[ProcChangeSingleChnlEncParam] vmp current MulPicStatus mergestart:FALSE!\n"  );
//        SendMsgToMcu(cServMsg.GetEventId()+2, &cServMsg);
//        return;
//    }
//    else    //切换编码
//    {
//        // 确定画面合成器当前编码参数和上层参数一致的编码通道
//
//        s8 byExistChnl  =  GetEncChnIdxByParam( &atEncParam );
//		if (byExistChnl == -1) //没有找到和要设置的通道参数完全一样的编码通道
//		{
//			LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[ProcChangeSingleChnlEncParam]EncParam set to enc chn%d!\n",  byChnnlNum );
//			vmplog( "[ProcChangeSingleChnlEncParam]EncParam set to enc chn%d!\n",  byChnnlNum );
//			REC_LASTCODELINE;
//			bRetOk &= m_pcHardMulPic->SetEncParam(byChnnlNum, &atEncParam);
//			if ( !bRetOk )
//			{
//				LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[ProcChangeSingleChnlEncParam] EncParam set failed\n");
//				OspPrintf(TRUE, FALSE, "[ProcChangeSingleChnlEncParam] EncParam set failed\n");
//			}
//			REC_LASTCODELINE; 
//		}
//    }
//
//	if (!bRetOk) 
//	{
//		LogPrint(LOG_LVL_ERROR,MID_MCU_VMP,"[ProcChangeSingleChnlEncParam] bRetOk false, nack it!\n"  );
//        SendMsgToMcu(cServMsg.GetEventId()+2, &cServMsg);
//		return;
//	}
//	SendMsgToMcu(cServMsg.GetEventId()+1, &cServMsg);
//
//}
CVMPCfg::CVMPCfg()
{
//    FreeStatusData();
    m_dwMcuNode = INVALID_NODE;   
    m_dwMcuIId  = INVALID_INS;

    m_bEmbed  = FALSE;
    m_byRegAckNum = 0;
}

CVMPCfg::~CVMPCfg()
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
void CVMPCfg::FreeStatusData(void)
{
    m_dwMcuNode = INVALID_NODE;   
    m_dwMcuIId  = INVALID_INS;
    return;
}

//END OF FILE

