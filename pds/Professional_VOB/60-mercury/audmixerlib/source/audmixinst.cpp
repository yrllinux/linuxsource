/*****************************************************************************
   模块名      : 混音器
   文件名      : audmixinst.cpp
   创建时间    : 2003年 12月 9日
   实现功能    : 
   作者        : 张明义
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   03/12/09    3.0         zmy         创建
******************************************************************************/      
#include "mcustruct.h"
#include "evmcueqp.h"
#include "evmixer.h"
#include "errmixer.h"
#include "audmixinst.h"
//#include "codeccommon.h"
#include "boardagent.h"
#include "mcuver.h"

CMixerApp g_cMixerApp;

s32  g_nPrintlog = 0;

/*lint -save -e526 -e628*/
/*lint -save -e438  -e530 -e1055*/
void mixlog( s8* pszFmt, ...)
{
    s8 achPrintBuf[512];
    s32  nBufLen = 0;
    va_list argptr;
	if( g_nPrintlog == 1 )
	{		  
		nBufLen = sprintf( achPrintBuf,"[Mixer]:" ); 
		va_start( argptr, pszFmt );
		nBufLen += vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
		va_end(argptr); 
		OspPrintf( TRUE, FALSE, achPrintBuf ); 
	}
    return;
}
/*lint -restore*/
/*=============================================================================
  函 数 名： MixerCallBackProc
  功    能： 底层回调
  算法实现： 
  全局变量： 
  参    数： u8* abyActiveChns
             u8 byExciteChn
             unsigned int nContext
  返 回 值： void 
=============================================================================*/
static void MixerCallBackProc(u8* abyActiveChns, u8 byExciteChn, unsigned int nContext)
{
    CServMsg cServMsg;
    u8 byNum = min(MAXNUM_MIXER_DEPTH, MAX_MIXER_DEPTH);
    cServMsg.SetMsgBody(abyActiveChns, byNum);
    cServMsg.CatMsgBody(&byExciteChn, sizeof(u8));
    ::OspPost(MAKEIID(AID_MIXER, (u16)nContext),
              EV_MIXER_ACTIVECHANGE_NOTIF,
              cServMsg.GetServMsg(),
              cServMsg.GetServMsgLen());
    mixlog("Grp.%d Active Change to byExciteChn!\n", nContext-1, byExciteChn);
    return;
}

//构造
/*lint -save -e1566*/
CAudMixInst::CAudMixInst() : m_nMaxMmbNum(0),
                             m_wLocalRcvPort(0),
                             m_bNeedPrs(FALSE)
{
    m_cConfId.SetNull();
    m_tGrpStatus.Reset();
    ClearMapTable();
}
/*lint -restore*/

//析构
/*lint -save -e1551*/
CAudMixInst::~CAudMixInst()
{
    m_cMixGrp.DestroyMixerGroup();
}
/*lint -restore*/

/*====================================================================
	函数  : DaemonInstanceEntry
	功能  : Daemon实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CAudMixInst::DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp)
{
    u8 byGrpId;
    u8* pbyMsgBody;

    CServMsg cServMsg(pMsg->content, pMsg->length);

    pbyMsgBody = (u8 *)cServMsg.GetMsgBody();

    mixlog("Daemon:Recv msg %u(%s).\n", pMsg->event, ::OspEventDesc(pMsg->event));
    switch(pMsg->event)
    {
    //断链检测:
    case MCU_EQP_ROUNDTRIP_REQ:
        SendMsgToMcu(EQP_MCU_ROUNDTRIP_ACK ,cServMsg);
        break;

    //混音器上电初始化
    case EV_MIXER_INIT:
        DaemonProcInitalMsg(pMsg, pcApp);
		break;

    // 建链
    case EV_MIXER_CONNECT:
        ProcConnectMcuTimeOut(TRUE);
        break;

    case EV_MIXER_CONNECTB:
        ProcConnectMcuTimeOut(FALSE);
        break;

    // 注册消息
    case EV_MIXER_REGISTER:  	
        ProcRegisterTimeOut(TRUE);
        break;

    case EV_MIXER_REGISTERB:  	
        ProcRegisterTimeOut(FALSE);
        break;

    // MCU 注册应答消息
    case MCU_MIXER_REG_ACK:
        MsgRegAckProc(pMsg, pcApp);
        break;

    // MCU拒绝本混音器注册
    case MCU_MIXER_REG_NACK:
        MsgRegNackProc(pMsg);
        break;
    // 设置Qos值
    case MCU_EQP_SETQOS_CMD:
        ProcSetQosInfo(pMsg);
        break;

    //OSP 断链消息
    case OSP_DISCONNECT:
        MsgDisconnectProc(pMsg, pcApp);
        break;

    case MCU_MIXER_STARTMIX_REQ:            // 开始混音组消息
    case MCU_MIXER_STOPMIX_REQ:             // 停止混音组消息
    case MCU_MIXER_ADDMEMBER_REQ:           // 增加,删除成员请求消息
    case MCU_MIXER_REMOVEMEMBER_REQ:    
    case MCU_MIXER_SETCHNNLVOL_CMD:         // 设置音量
    case MCU_MIXER_FORCEACTIVE_REQ:         // 强制混音请求
    case MCU_MIXER_CANCELFORCEACTIVE_REQ:   // 取消强制混音请求
    case MCU_MIXER_SETMIXDEPTH_REQ:         // 设置混音深度请求消息
    case MCU_MIXER_SEND_NOTIFY:             // 是否要发送码流通知
    case MCU_MIXER_VACKEEPTIME_CMD:         // 语音激励延时保护时长消息
    case MCU_MIXER_SETMEMBER_CMD:           // 指定混音成员
    case MCU_MIXER_CHANGEMIXDELAY_CMD:
        {
            byGrpId = *pbyMsgBody + 1; 
            // 混音组ID无效
            if (byGrpId > g_cMixerApp.m_tMixerCfg.byMaxMixGroupCount)
            {
                cServMsg.SetErrorCode(ERR_MIXER_GRPINVALID);
                SendMsgToMcu(pMsg->event + 2, cServMsg);
                log(LOGLVL_EXCEPTION, "[AMix] Invalid group ID(%d)\n", byGrpId);
                return;
            }
            // 将消息分发到指定的组
			post(MAKEIID(GetAppID(), byGrpId), pMsg->event, pMsg->content, pMsg->length);
        }
        break;

    // 显示混音组状态
    case EV_MIXER_SHOWGROUP:
		ShowAllMixGroupStatus(pcApp);
		break;

	case MCU_EQP_MODSENDADDR_CMD:
		DeamonProcModSendIpAddr(pMsg);
		break;

    case EV_MIXER_SHOWMT2CHNNL:
        ShowAllMixGrpChnStatus(pcApp);
        break;

	// 取主备倒换状态应答或超时
	case EV_MIXER_MCU_GETMSSTATUS:
	case MCU_EQP_GETMSSTATUS_ACK:
		DaemonProcGetMsStatusRsp(pMsg,pcApp);
		break;

    // 显示各组状态
    case EV_MIXER_SHOWMEMBER:
        break;

    // 退出混音器
    case EV_MIXER_EXIT:
        break;
		
	// Mcu 通知外设重连 MPCB 板, zgc, 2007/04/30	
	case MCU_EQP_RECONNECTB_CMD: 
		DaemonProcReconnectBCmd( pMsg);
		break;

	//test, zgc, 2007/04/24
	case MCU_MIXER_STOPEQP_CMD:
		DaemonProcStopEqpCmd(pcApp );
		break;
	case MCU_MIXER_STARTEQP_CMD:
		DaemonProcStartEqpCmd( pMsg, pcApp );
		break;
    ////////////////////////////////////
    // 测试代理
    //获取外设状态消息(同步消息)
    case EV_MIXER_TEST_GETEQPSTATUS:
        break;

    default:
        break;
    }
}

/*====================================================================
	函数  : InstanceEntry
	功能  : 普通实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CAudMixInst::InstanceEntry(CMessage * const pMsg)
{
    mixlog("Grp.%d Recv msg %u(%s).\n", GetInsID()-1, pMsg->event, ::OspEventDesc(pMsg->event));

    switch(pMsg->event)
    {
    // 增加混音成员
    case MCU_MIXER_ADDMEMBER_REQ:
        MsgAddMixMemberProc(pMsg);
        break;

    // 删除混音成员
    case MCU_MIXER_REMOVEMEMBER_REQ:
        MsgRemoveMixMemberPorc(pMsg);
        break;

    // 开始混音
    case MCU_MIXER_STARTMIX_REQ:
        MsgStartMixProc(pMsg);
        break;

    // 停止混音
    case MCU_MIXER_STOPMIX_REQ:
        MsgStopMixProc(pMsg);
        break;

    // 设置音量
    case MCU_MIXER_SETCHNNLVOL_CMD:
        MsgSetVolumeProc(pMsg);
        break;

    // 强制混音请求
    case MCU_MIXER_FORCEACTIVE_REQ:
        MsgForceActiveProc(pMsg);
        break;

    // 取消强制混音请求
    case MCU_MIXER_CANCELFORCEACTIVE_REQ:
        MsgCancelForceActiveProc(pMsg);
        break;

    // 激活成员变化
    case EV_MIXER_ACTIVECHANGE_NOTIF:
        MsgActiveMmbChangeProc(pMsg);
        break;
        
    // 混音延时
    case MCU_MIXER_CHANGEMIXDELAY_CMD:
        MsgChangeMixDelay(pMsg);
        break;
	// 设置混音深度
    case MCU_MIXER_SETMIXDEPTH_REQ:
        MsgSetMixDepthProc(pMsg);
        break;
	// 设置混音成员
    case MCU_MIXER_SETMEMBER_CMD:
        MsgSetMixChannelProc();
        break;
	// 
    case MCU_MIXER_SEND_NOTIFY:
        MsgSetMixSendProc(pMsg);
        break;
	// 语音激励
    case MCU_MIXER_VACKEEPTIME_CMD:
        MsgSetVacKeepTimeProc(pMsg);
        break;
        
    // 设置Qos值
    case MCU_EQP_SETQOS_CMD:
        ProcSetQosInfo(pMsg);
        break;

	case EV_MIXER_MODSENDADDR:
		ProcModSndAddr();
		 break;	

    default:
        break;
    }
}

/*=============================================================================
  函 数 名： DaemonProcInitalMsg
  功    能： 处理初始化消息
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
             CApp* pcApp
  返 回 值： void 
=============================================================================*/
void CAudMixInst::DaemonProcInitalMsg(CMessage* const pMsg, CApp* pcApp)
{
	g_cMixerApp.m_tMixerCfg = *(TAudioMixerCfg*)pMsg->content; // 先设置配置信息
    if (!Init(pcApp)) // 初始化
    {
        OspPrintf(TRUE, FALSE, "Daemon:Init Mixer fail!\n");
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
void CAudMixInst::DeamonProcModSendIpAddr( CMessage* const pMsg )
{
    if ( NULL == pMsg->content )
    {
        mixlog(" The pointer cannot be Null (DeamonProcModSendIpAddr)\n" );
        return;
    } 
	
	u32 dwSendIP = *(u32*)pMsg->content;
	g_cMixerApp.m_dwMcuRcvIp = ntohl(dwSendIP);
	
    for (u8 byLoop = 1; byLoop <= MAXNUM_MIXER_GROUP; byLoop++ )
    {
        post( MAKEIID(GetAppID(), byLoop), EV_MIXER_MODSENDADDR);
    }	
	
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
void CAudMixInst::ProcModSndAddr()
{
    TNetAddress tMcuAddr;
    tMcuAddr.dwIp  = g_cMixerApp.m_dwMcuRcvIp;
    tMcuAddr.wPort = g_cMixerApp.m_wMcuRcvStartPort + MAXPORTSPAN_MIXER_GROUP*m_tGrpStatus.m_byGrpId + 2; //??
    TNetAddress  tLocalAddr;
    tLocalAddr.dwIp  = g_cMixerApp.m_tMixerCfg.dwLocalIP;
    tLocalAddr.wPort = m_wLocalRcvPort + 2;
	
    m_cMixGrp.SetNModeOutput(&tLocalAddr, &tMcuAddr);// N模式输出
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
====================================================================*/
BOOL32 CAudMixInst::Init(CApp* pcApp)
{
    u16 wLoop;
    u16 wFailIdx;
    CAudMixInst* pInst = NULL;

    wFailIdx = 0;       //创建混音组失败位置 
    for(wLoop = 1; wLoop <= g_cMixerApp.m_tMixerCfg.byMaxMixGroupCount; wLoop++)
    {
        pInst = (CAudMixInst*)pcApp->GetInstance(wLoop);
        if (!pInst->InitMixGroup())
        {
            wFailIdx = wLoop;
        }
    }

    if (wFailIdx > 0) // 有创建失败的混音组
    {
        for(wLoop = 1; wLoop < wFailIdx; wLoop++)
        {
			pInst = (CAudMixInst*)pcApp->GetInstance(wLoop);
		    if (MIXER_OPERATE_SUCCESS != pInst->DestroyeMixGroup())  // 对称操作
			{
                log(LOGLVL_EXCEPTION, "[AMix] Destory Mixer Group. %d failed.\n",wLoop);
			}
		}
        return FALSE;
    }
	// 处理内嵌
    if(g_cMixerApp.m_tMixerCfg.dwConnectIP == g_cMixerApp.m_tMixerCfg.dwLocalIP)
    {
        g_cMixerApp.m_bEmbed = TRUE;
        g_cMixerApp.m_dwMcuNode = 0;
        SetTimer(EV_MIXER_REGISTER, MIX_REGISTER_TIMEOUT);// 如果嵌入，直接注册
    }

    if(g_cMixerApp.m_tMixerCfg.dwConnectIpB == g_cMixerApp.m_tMixerCfg.dwLocalIP)
    {
        g_cMixerApp.m_bEmbedB = TRUE;
        g_cMixerApp.m_dwMcuNodeB = 0;
        SetTimer(EV_MIXER_REGISTERB, MIX_REGISTER_TIMEOUT);
    }
	// 连接Mcu
    if(0 != g_cMixerApp.m_tMixerCfg.dwConnectIP && FALSE == g_cMixerApp.m_bEmbed)  // connect to mcu a
    { 
        SetTimer(EV_MIXER_CONNECT, MIX_CONNETC_TIMEOUT); 
    }

    if( 0 != g_cMixerApp.m_tMixerCfg.dwConnectIpB && FALSE == g_cMixerApp.m_bEmbedB) // connect to mcu b
    { 
        SetTimer(EV_MIXER_CONNECTB, MIX_CONNETC_TIMEOUT); 
    }

    return TRUE;
}

/*=============================================================================
  函 数 名： ProcConnectMcuTimeOut
  功    能： 处理连接超时
  算法实现： 
  全局变量： 
  参    数： BOOL32 bIsConnectA
  返 回 值： void 
=============================================================================*/
void CAudMixInst::ProcConnectMcuTimeOut(BOOL32 bIsConnectA)
{
    BOOL32 bRet = FALSE;
    if(TRUE == bIsConnectA)
    {
        bRet = ConnectMcu(bIsConnectA, g_cMixerApp.m_dwMcuNode);
        if(TRUE == bRet)
        { 
            SetTimer(EV_MIXER_REGISTER, MIX_REGISTER_TIMEOUT); 
        }
        else
        {
            SetTimer(EV_MIXER_CONNECT, MIX_CONNETC_TIMEOUT);    //定时建链
        }
    }
    else
    {
        bRet = ConnectMcu(bIsConnectA, g_cMixerApp.m_dwMcuNodeB);
        if(TRUE == bRet)
        {  
            SetTimer(EV_MIXER_REGISTERB, MIX_REGISTER_TIMEOUT); 
        }
        else
        {
            SetTimer(EV_MIXER_CONNECTB, MIX_CONNETC_TIMEOUT);    //定时建链
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
====================================================================*/
BOOL32 CAudMixInst::ConnectMcu(BOOL32 bIsConnectA, u32& dwMcuNode)
{
    BOOL32 bRet = TRUE;
    if (!OspIsValidTcpNode(dwMcuNode))   
    {   
        if(TRUE == bIsConnectA)
        {   
            dwMcuNode = BrdGetDstMcuNode();  
        }
        else
        {    
            dwMcuNode = BrdGetDstMcuNodeB(); 
        }

        if (::OspIsValidTcpNode(dwMcuNode))
        {
            mixlog("Connect Mcu Success!");
            ::OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), CInstance::DAEMON);// 由DAEMON实例处理断链消息
        }
        else 
        {
            mixlog("Connect to Mcu%d failed,retry after 3s!\n", g_cMixerApp.m_tMixerCfg.wMcuId);
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
void CAudMixInst::ProcRegisterTimeOut(BOOL32 bIsRegisterA)
{
    if(TRUE == bIsRegisterA)
    {
        Register(bIsRegisterA, g_cMixerApp.m_dwMcuNode);
        SetTimer(EV_MIXER_REGISTER, MIX_REGISTER_TIMEOUT);
    }
    else
    {
        Register(bIsRegisterA, g_cMixerApp.m_dwMcuNodeB);
        SetTimer(EV_MIXER_REGISTERB, MIX_REGISTER_TIMEOUT);
    }
    return;
}

/*====================================================================
	函数  : Register
	功能  : 向MCU注册
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CAudMixInst::Register(BOOL32 bIsRegisterA, u32& dwMcuNode)
{
    CServMsg cSvrMsg;
    TPeriEqpRegReq tReg;

    u8 nMaxChannlNum = g_cMixerApp.m_tMixerCfg.byMaxMixGroupCount;

    tReg.SetEqpId(g_cMixerApp.m_tMixerCfg.byEqpId);
    tReg.SetEqpType(g_cMixerApp.m_tMixerCfg.byEqpType);
    tReg.SetEqpAlias(g_cMixerApp.m_tMixerCfg.achAlias);
    tReg.SetChnnlNum(nMaxChannlNum);
    tReg.SetPeriEqpIpAddr(g_cMixerApp.m_tMixerCfg.dwLocalIP);
    tReg.SetStartPort(g_cMixerApp.m_tMixerCfg.wRcvStartPort);
    tReg.SetVersion(DEVVER_MIXER);

    if(TRUE == bIsRegisterA) // 以后区分
    {
        tReg.SetMcuId((u8)g_cMixerApp.m_tMixerCfg.wMcuId);
    }
    else
    {
        tReg.SetMcuId((u8)g_cMixerApp.m_tMixerCfg.wMcuId);
    }

    cSvrMsg.SetMsgBody((u8*)&tReg, sizeof(tReg));
    post( MAKEIID(AID_MCU_PERIEQPSSN, g_cMixerApp.m_tMixerCfg.byEqpId), MIXER_MCU_REG_REQ,
          cSvrMsg.GetServMsg(), cSvrMsg.GetServMsgLen(), dwMcuNode );

    mixlog("Register to Mcu%d ....\n", g_cMixerApp.m_tMixerCfg.wMcuId);
    mixlog("Ip: %0x, IpB: %0x\n", g_cMixerApp.m_tMixerCfg.dwConnectIP, g_cMixerApp.m_tMixerCfg.dwConnectIpB);
    mixlog("A's node: %d, B's node: %d\n", g_cMixerApp.m_dwMcuNode, g_cMixerApp.m_dwMcuNodeB);

    return;
}

/*====================================================================
	函数  : MsgRegAckProc 
	功能  : MCU应答混音器注册消息处理函数
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CAudMixInst::MsgRegAckProc(CMessage* const pMsg, CApp* pcApp)
{
    CAudMixInst* pInst = NULL;
    TPeriEqpStatus tEqpStatus;
    TPeriEqpRegAck* ptRegAck = NULL;
    u16 wLoop = 0;
    CServMsg cServMsg(pMsg->content, pMsg->length);
    ptRegAck =(TPeriEqpRegAck*)cServMsg.GetMsgBody();

    TPrsTimeSpan tPrsTimeSpan = *(TPrsTimeSpan *)(cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck));
	// 获得MTU size, zgc, 2007-04-02
	u16 wMTUSize = *(u16*)( cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck) + sizeof(TPrsTimeSpan) );
	wMTUSize = ntohs( wMTUSize );
	u8 byIsSendRedundancy = *(u8*)( cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck) + sizeof(TPrsTimeSpan) + sizeof(u16) );

	// 网络重传参数
    g_cMixerApp.m_tNetRSParam.m_wFirstTimeSpan  = tPrsTimeSpan.m_wFirstTimeSpan;
    g_cMixerApp.m_tNetRSParam.m_wSecondTimeSpan = tPrsTimeSpan.m_wSecondTimeSpan;
    g_cMixerApp.m_tNetRSParam.m_wThirdTimeSpan  = tPrsTimeSpan.m_wThirdTimeSpan;
    g_cMixerApp.m_tNetRSParam.m_wRejectTimeSpan = tPrsTimeSpan.m_wRejectTimeSpan;

    mixlog("[m_tNetRsParam]m_wFirstTimeSpan = %d m_wSecondTimeSpan = %d, m_wThirdTimeSpan = %d, m_wRejectTimeSpan = %d!\n",
                        g_cMixerApp.m_tNetRSParam.m_wFirstTimeSpan,
                        g_cMixerApp.m_tNetRSParam.m_wSecondTimeSpan,
                        g_cMixerApp.m_tNetRSParam.m_wThirdTimeSpan,
                        g_cMixerApp.m_tNetRSParam.m_wRejectTimeSpan);
	mixlog( "[RegAck] The net MTU size is : %d\n", wMTUSize );
	mixlog( "[RegAck] IsSendRedundancy : %d\n", byIsSendRedundancy );
    
    if( g_cMixerApp.m_dwMcuNode == pMsg->srcnode)// 注册成功
    {
        g_cMixerApp.m_dwMcuRcvIp       = ptRegAck->GetMcuIpAddr();
        g_cMixerApp.m_wMcuRcvStartPort = ptRegAck->GetMcuStartPort();
        g_cMixerApp.m_dwMcuIId = pMsg->srcid;
        g_cMixerApp.m_byRegAckNum++;
        KillTimer(EV_MIXER_REGISTER);
        mixlog("Register A's Mcu%d  success !\n", g_cMixerApp.m_tMixerCfg.wMcuId);

        tEqpStatus.SetMcuEqp((u8)g_cMixerApp.m_tMixerCfg.wMcuId, g_cMixerApp.m_tMixerCfg.byEqpId, g_cMixerApp.m_tMixerCfg.byEqpType);
    }
    else if( g_cMixerApp.m_dwMcuNodeB == pMsg->srcnode )
    {
        g_cMixerApp.m_dwMcuRcvIpB = ptRegAck->GetMcuIpAddr();
        g_cMixerApp.m_wMcuRcvStartPortB = ptRegAck->GetMcuStartPort();
        g_cMixerApp.m_dwMcuIIdB = pMsg->srcid;
        g_cMixerApp.m_byRegAckNum ++;
        KillTimer(EV_MIXER_REGISTERB);
        mixlog("Register B's Mcu%d  success !\n", g_cMixerApp.m_tMixerCfg.wMcuId);

        tEqpStatus.SetMcuEqp((u8)g_cMixerApp.m_tMixerCfg.wMcuId, g_cMixerApp.m_tMixerCfg.byEqpId, g_cMixerApp.m_tMixerCfg.byEqpType);

    }

	// guzh [6/12/2007] 校验会话参数
    if ( g_cMixerApp.m_dwMpcSSrc == 0 )
    {
        g_cMixerApp.m_dwMpcSSrc = ptRegAck->GetMSSsrc();
    }
    else
    {
        // 异常情况，断开两个节点
        if ( g_cMixerApp.m_dwMpcSSrc != ptRegAck->GetMSSsrc() )
        {
            OspPrintf(TRUE, FALSE, "[MsgRegAckProc] MPC SSRC ERROR(%u<-->%u), Disconnect Both Nodes!\n", 
                      g_cMixerApp.m_dwMpcSSrc, ptRegAck->GetMSSsrc());
            if ( OspIsValidTcpNode(g_cMixerApp.m_dwMcuNode) )
            {
                OspDisconnectTcpNode(g_cMixerApp.m_dwMcuNode);
            }
            if ( OspIsValidTcpNode(g_cMixerApp.m_dwMcuNodeB) )
            {
                OspDisconnectTcpNode(g_cMixerApp.m_dwMcuNodeB);
            }      
            return;
        }
    }

    if(FIRST_REGACK == g_cMixerApp.m_byRegAckNum)  // 防止另外一个注册消息更改状态
    {
		// 设置MTU大小, zgc, 2007-04-02
		g_cMixerApp.m_wMTUSize = wMTUSize;
		//SetVideoSendPacketLen( (s32)g_cMixerApp.m_wMTUSize );
		// 记录是否冗余发送, zgc, 2007-07-25
		// 已取消，为保持兼容，暂保留, 置FALSE, zgc, 2007-12-25
		//g_cMixerApp.m_bIsSendRedundancy = ( 1 == byIsSendRedundancy ) ? TRUE : FALSE;
		g_cMixerApp.m_bIsSendRedundancy = FALSE;

        NEXTSTATE((s32)NORMAL);  //DAEMON 实例进入 NORMAL 状态
    }

    for(wLoop = 0; wLoop < g_cMixerApp.m_tMixerCfg.byMaxMixGroupCount; wLoop++)
    {
        pInst = (CAudMixInst*)pcApp->GetInstance(wLoop+1);
        memcpy(&tEqpStatus.m_tStatus.tMixer.m_atGrpStatus[wLoop],
               &pInst->m_tGrpStatus, sizeof(TMixerGrpStatus));
    }

    tEqpStatus.m_tStatus.tMixer.m_byGrpNum = g_cMixerApp.m_tMixerCfg.byMaxMixGroupCount;

    tEqpStatus.m_byOnline = (u8)TRUE;

    tEqpStatus.SetAlias(g_cMixerApp.m_tMixerCfg.achAlias);

    cServMsg.SetMsgBody((u8*)&tEqpStatus, sizeof(tEqpStatus));

    //zbq[04/11/2008] 注册成功后的状态通知导致的混音恢复时时序很重要. 即使主备也只通知一次.
    if ( g_cMixerApp.m_dwMcuNode == pMsg->srcnode )
    {
        SendMsgToMcuA(MIXER_MCU_MIXERSTATUS_NOTIF, cServMsg);
    }
    else if (  g_cMixerApp.m_dwMcuNodeB == pMsg->srcnode )
    {
        SendMsgToMcuB(MIXER_MCU_MIXERSTATUS_NOTIF, cServMsg);
    }

    for(wLoop = 0; wLoop < g_cMixerApp.m_tMixerCfg.byMaxMixGroupCount; wLoop++)
    {
        pInst = (CAudMixInst*)pcApp->GetInstance(wLoop+1);
        pInst->SendGrpNotif();
    }
    return;
}

/*====================================================================
	函数  : MsgRegNackProc
	功能  : MCU 拒绝混音器注册消息处理函数
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CAudMixInst::MsgRegNackProc(CMessage* const pMsg)
{
    if(pMsg->srcnode == g_cMixerApp.m_dwMcuNode)
    {
        mixlog("Mixer registe be refused by A's Mcu%d .\n", g_cMixerApp.m_tMixerCfg.wMcuId);
    }

    if(pMsg->srcnode == g_cMixerApp.m_dwMcuNodeB)
    {
        mixlog("Mixer registe be refused by B's Mcu%d .\n", g_cMixerApp.m_tMixerCfg.wMcuId);
    }
    return;
}


/*====================================================================
	函数  : MsgDisconnectProc
	功能  : Osp断链消息处理函数
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CAudMixInst::MsgDisconnectProc(CMessage* const pMsg, CApp* pcApp)
{
    u32 dwNode = *(u32*)pMsg->content;

    if (INVALID_NODE != dwNode)
    {
        OspDisconnectTcpNode(dwNode);
    } 
    if(dwNode == g_cMixerApp.m_dwMcuNode)
    {
        g_cMixerApp.FreeStatusDataA();
        SetTimer(EV_MIXER_CONNECT, MIX_CONNETC_TIMEOUT);
    }
    else if(dwNode == g_cMixerApp.m_dwMcuNodeB)
    {
        g_cMixerApp.FreeStatusDataB();
        SetTimer(EV_MIXER_CONNECTB, MIX_CONNETC_TIMEOUT);
    }
	
    // 与其中一个断链后，向Mcu取主备倒换状态，判断是否成功
	if (INVALID_NODE != g_cMixerApp.m_dwMcuNode || INVALID_NODE != g_cMixerApp.m_dwMcuNodeB)
	{
		if (OspIsValidTcpNode(g_cMixerApp.m_dwMcuNode))
		{
			// 是否考虑延时一会待Mcu稳定后再发
			post( g_cMixerApp.m_dwMcuIId, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cMixerApp.m_dwMcuNode );     
			OspPrintf(TRUE, FALSE, "[MsgDisconnectProc] GetMsStatusReq. McuNode.A\n");
		}
		else if (OspIsValidTcpNode(g_cMixerApp.m_dwMcuNodeB))
		{
			post( g_cMixerApp.m_dwMcuIIdB, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cMixerApp.m_dwMcuNodeB );        
			OspPrintf(TRUE, FALSE, "[MsgDisconnectProc] GetMsStatusReq. McuNode.B\n");
		}
		// 等待指定时间
		SetTimer(EV_MIXER_MCU_GETMSSTATUS, WAITING_MSSTATUS_TIMEOUT);
		return;
	}

    if(INVALID_NODE == g_cMixerApp.m_dwMcuNode && INVALID_NODE == g_cMixerApp.m_dwMcuNodeB)
    {
        StopAllMixGroup(pcApp);
        NEXTSTATE((s32)IDLE);                      //  DAEMON 实例进入空闲状态
        g_cMixerApp.m_byRegAckNum = 0;
		g_cMixerApp.m_dwMpcSSrc = 0;
    }
    return;
}

/*=============================================================================
  函 数 名： DaemonProcGetMsStatusRsp
  功    能： 处理取主备倒换状态
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
=============================================================================*/
void CAudMixInst::DaemonProcGetMsStatusRsp(CMessage* const pMsg, CApp* pcApp)
{
	BOOL bSwitchOk = FALSE;
	CServMsg cServMsg(pMsg->content, pMsg->length);
	if( MCU_EQP_GETMSSTATUS_ACK == pMsg->event )
	{
		TMcuMsStatus *ptMsStatus = (TMcuMsStatus *)cServMsg.GetMsgBody();
        
        KillTimer(EV_MIXER_MCU_GETMSSTATUS);
        mixlog("[DeamonProcGetMsStatus]. receive msg MCU_EQP_GETMSSTATUS_ACK. IsMsSwitchOK :%d\n", 
                  ptMsStatus->IsMsSwitchOK());

        if(ptMsStatus->IsMsSwitchOK())         //倒换成功
        {
            bSwitchOk = TRUE;
        }
	}
	// 倒换失败或者超时
	if( !bSwitchOk )
	{
		//20110914 zjl 超时后先清状态 再断链 再连接
		mixlog("[DeamonProcGetMsStatus]StopAllMixGroup!\n");
		StopAllMixGroup(pcApp);
		NEXTSTATE((s32)IDLE);                      //DAEMON 实例进入空闲状态
        g_cMixerApp.m_byRegAckNum = 0;
		g_cMixerApp.m_dwMpcSSrc = 0;

		if ( OspIsValidTcpNode(g_cMixerApp.m_dwMcuNode ))
		{
			mixlog("[DeamonProcGetMsStatus] OspDisconnectTcpNode A!\n");
			OspDisconnectTcpNode(g_cMixerApp.m_dwMcuNode );
		}
		if ( OspIsValidTcpNode(g_cMixerApp.m_dwMcuNodeB))
		{
			mixlog("[DeamonProcGetMsStatus] OspDisconnectTcpNode B!\n");
			OspDisconnectTcpNode(g_cMixerApp.m_dwMcuNodeB);
		}

		if( INVALID_NODE == g_cMixerApp.m_dwMcuNode )
		{
			mixlog("[DeamonProcGetMsStatus] EV_MIXER_CONNECT!\n");
			SetTimer(EV_MIXER_CONNECT, MIX_CONNETC_TIMEOUT);
		}
		if( INVALID_NODE == g_cMixerApp.m_dwMcuNodeB )
		{
			mixlog("[DeamonProcGetMsStatus] EV_MIXER_CONNECTB!\n");
			SetTimer(EV_MIXER_CONNECTB, MIX_CONNETC_TIMEOUT);
		}
	}
	return;
}

/*=============================================================================
函 数 名： DaemonProcReconnectBCmd
功    能： 
算法实现： 
全局变量： 
参    数：  CMessage * const pcMsg
           CApp* pcApp
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/4/30   4.0			周广程                  创建
=============================================================================*/
void CAudMixInst::DaemonProcReconnectBCmd( CMessage * const pcMsg)
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u32 dwMpcBIp = *(u32*)cServMsg.GetMsgBody();
	dwMpcBIp = ntohl(dwMpcBIp);
	g_cMixerApp.m_tMixerCfg.dwConnectIpB = dwMpcBIp;
	
	// 如果已连接, 先断开
	if( OspIsValidTcpNode( g_cMixerApp.m_dwMcuNodeB ) )
	{
		OspDisconnectTcpNode( g_cMixerApp.m_dwMcuNodeB );
	}

	g_cMixerApp.FreeStatusDataB();
	if( 0 != g_cMixerApp.m_tMixerCfg.dwConnectIpB && !g_cMixerApp.m_bEmbedB) // connect to mcu b
    { 
        SetTimer(EV_MIXER_CONNECTB, MIX_CONNETC_TIMEOUT); 
    }
	else
	{
		SetTimer(EV_MIXER_REGISTERB, MIX_REGISTER_TIMEOUT);
	}
}

/*=============================================================================
函 数 名： DaemonProcStopEqpCmd
功    能： 
算法实现： 
全局变量： 
参    数：  CMessage * const pcMsg
           CApp* pcApp
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/4/24   4.0			周广程                  创建
=============================================================================*/
/*lint -save -e527*/
void CAudMixInst::DaemonProcStopEqpCmd(CApp* pcApp )
{
	return;
	StopAllMixGroup( pcApp );
	OspDelay( 10 );
	CAudMixInst *pInst = NULL;
	for(u8 byLoop = 1; byLoop <= g_cMixerApp.m_tMixerCfg.byMaxMixGroupCount; byLoop++)
	{
		pInst = (CAudMixInst*)pcApp->GetInstance(byLoop);
		if (MIXER_OPERATE_SUCCESS != pInst->DestroyeMixGroup())  
		{
		   OspPrintf( TRUE, FALSE, "[AMix] Destory Mixer Group. %d failed.\n",byLoop);
		}
		//pInst->SendGrpNotif();
	}
	g_cMixerApp.m_byRegAckNum = 0;
	//NEXTSTATE(IDLE);                      //  DAEMON 实例进入空闲状态
}
/*lint -restore*/

/*=============================================================================
函 数 名： DaemonProcStartEqpCmd
功    能： 
算法实现： 
全局变量： 
参    数：  CMessage * const pcMsg
           CApp* pcApp
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/4/26   4.0			周广程                  创建
=============================================================================*/
void CAudMixInst::DaemonProcStartEqpCmd( CMessage * const pcMsg, CApp* pcApp )
{
	if( 0 == g_cMixerApp.m_tMixerCfg.byEqpId )
	{
		OspPrintf( TRUE, FALSE, "Daemon: Mixer eqpid is %d\n", g_cMixerApp.m_tMixerCfg.byEqpId );
		return;
	}
	pcMsg->content = (u8*)&g_cMixerApp.m_tMixerCfg;
	if (!Init(pcApp)) // 初始化
    {
        OspPrintf(TRUE, FALSE, "Daemon:Init Mixer fail!\n");
    }
	return;
}

/*=============================================================================
  函 数 名： StopAllMixGroup
  功    能： 停止所有混音组
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CAudMixInst::StopAllMixGroup(CApp* pcApp)
{
	CAudMixInst* pcInst = NULL;
	u8 byGrpId = 0;
	CServMsg cServMsg;
	for(u16 wLoop = 0; wLoop < g_cMixerApp.m_tMixerCfg.byMaxMixGroupCount; wLoop++)// 向各组发送停止消息
    {
        byGrpId = (u8)(wLoop+1);
        pcInst  = (CAudMixInst*)pcApp->GetInstance(byGrpId);

        cServMsg.SetMsgBody(&byGrpId ,sizeof(byGrpId));
        cServMsg.SetConfId(pcInst->m_cConfId);
		// 停止混音
        post(MAKEIID(GetAppID(), byGrpId), MCU_MIXER_STOPMIX_REQ, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
    }
	return;
}

/*====================================================================
	函数  : InitMixGroup
	功能  : 初始化混音组
	输入  : 无
	输出  : 无
	返回  : 成功返回TRUE，否则FALSE;
	注    : 将混音组所有成员初始为未使用
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
BOOL32 CAudMixInst::InitMixGroup()
{
    u16 wInst = GetInsID();
    TMixerMap tMixMap;         //MAP信息

    m_tGrpStatus.Reset();
    m_tGrpStatus.m_byGrpId    = (u8)wInst-1;
    m_tGrpStatus.m_byGrpState = (u8)TMixerGrpStatus::IDLE;

    m_wLocalRcvPort = g_cMixerApp.m_tMixerCfg.wRcvStartPort+ MAXPORTSPAN_MIXER_GROUP*m_tGrpStatus.m_byGrpId;

    if (wInst <= g_cMixerApp.m_tMixerCfg.byMaxMixGroupCount)
    {
        NEXTSTATE((s32)TMixerGrpStatus::READY);
        m_tGrpStatus.m_byGrpState = (u8)TMixerGrpStatus::READY; // 该通道为可用通道
    }
	// map信息
    tMixMap.m_dwCoreSpd = g_cMixerApp.m_tMixerCfg.m_atMap[wInst-1].dwCoreSpd ;
    tMixMap.m_dwMemSpd  = g_cMixerApp.m_tMixerCfg.m_atMap[wInst-1].dwMemSpd;
    tMixMap.m_dwMemSize = g_cMixerApp.m_tMixerCfg.m_atMap[wInst-1].dwMemSize;
    tMixMap.m_byMapId   = g_cMixerApp.m_tMixerCfg.m_atMap[wInst-1].byMapId;
    tMixMap.m_byPort    = (u8)g_cMixerApp.m_tMixerCfg.m_atMap[wInst-1].dwPort;

    BOOL32 bRet = TRUE;
	// 创建混音组
    u16 wRet = m_cMixGrp.CreateMixerGroup(&tMixMap, g_cMixerApp.m_tMixerCfg.byMaxChannelInGroup);
    if (MIXER_OPERATE_SUCCESS != wRet)
    {
        log(LOGLVL_EXCEPTION, "[AMix] Grp.%d call \"CreateMixerGroup\" failed.\n", GetInsID());
        bRet = FALSE;
    }
    else
    {
        m_cConfId.SetNull();

		u32 dwStartRcvBufs = 0;
		u32 dwFastRcvBufs = 0;
		
		BOOL32 bResult = ReadConfigFileApu( dwStartRcvBufs, dwFastRcvBufs );
		if( bResult )
		{
			m_cMixGrp.SetMixerRcvBufs( dwStartRcvBufs, dwFastRcvBufs );

			mixlog("[InitMixGroup] SetMixerRcvBufs dwStartRcvBufs= %d, dwFastRcvBufs= %d.", dwStartRcvBufs, dwFastRcvBufs);
		}
		else
		{
			OspPrintf(TRUE, FALSE, "[InitMixGroup] the apudebug.ini is not exist.\n");
		}

    }
	
    return bRet;
}

/*====================================================================
	函数  : StartMix
	功能  : 开始混音
	输入  : byAudioType -  语音类型
	        byDepth     -   混音深度
	输出  : 无
	返回  : 成功返回TRUE否则返回FALSE
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
BOOL32 CAudMixInst::StartMix(u8 byAudType, u8 byDepth, u8 byAll,
                             TMediaEncrypt * ptMediaEncrypt, 
                             TDoublePayload * ptDoublePayload, u8 byFECType)
{
    u16 wInst = GetInsID();
    if (wInst > g_cMixerApp.m_tMixerCfg.byMaxMixGroupCount)
    {
        log(LOGLVL_EXCEPTION, "[AMix] Grp.%d not exist.\n", GetInsID()-1);
        return FALSE;
    }
	// 一块Map一个混音组
    u8 byMapNum = (u8)g_cMixerApp.m_tMixerCfg.wMAPCount;
    if (wInst > byMapNum)
    {
        OspPrintf(TRUE, FALSE, "This audio mixer only alloc %d map,yet inst %d want start mix!!\n", byMapNum, wInst);
        return FALSE;
    }
	// 设置回调
    u16 wRet = m_cMixGrp.SetMixerCallBack(MixerCallBackProc, (u32)wInst);
    if (MIXER_OPERATE_SUCCESS != wRet)
    {
        OspPrintf(TRUE, FALSE, "[AMix] Grp.%d call \"SetMixerCallBack\" failed.\n", GetInsID());
        return FALSE;
    }

    mixlog("Create Grp.%d success.\n", GetInsID()-1);
	// 开始混音
    u16 wOpRet = m_cMixGrp.StartMixerGroup(byAll);
    if (MIXER_OPERATE_SUCCESS != wOpRet)
    {
        OspPrintf(TRUE, FALSE, "[AMix] Grp.%d call StartMixerGroup failed(%d)!\n", GetInsID()-1, wOpRet);
        return FALSE;
    }

	// 这里应考虑同一混音器存在多个混音组的情况，NMode目标端口加以区分 ??
    TNetAddress tMcuAddr;
    tMcuAddr.dwIp  = g_cMixerApp.m_dwMcuRcvIp;
    tMcuAddr.wPort = g_cMixerApp.m_wMcuRcvStartPort + MAXPORTSPAN_MIXER_GROUP*m_tGrpStatus.m_byGrpId + 2; //??
    TNetAddress  tLocalAddr;
    tLocalAddr.dwIp  = g_cMixerApp.m_tMixerCfg.dwLocalIP;
    tLocalAddr.wPort = m_wLocalRcvPort + 2;

	mixlog("[Mix] NMode: LocalIp:%0x, Port:%d McuIp:%0x, McuPort:%d\n", 
		   tLocalAddr.dwIp, tLocalAddr.wPort, tMcuAddr.dwIp, tMcuAddr.wPort);

    wRet = m_cMixGrp.SetNModeOutput(&tLocalAddr, &tMcuAddr);// N模式输出
    if (MIXER_OPERATE_SUCCESS != wRet)
    {
        OspPrintf(TRUE, FALSE, "[AMix] Grp.%d call SetNModeOutput failed(%d)!\n", GetInsID()-1, wRet);
        return FALSE;
    }
    
    if(m_bNeedPrs)
    {
        wRet = m_cMixGrp.SetNModeNetFeedbackParam(TRUE, 500);
        if (MIXER_OPERATE_SUCCESS != wRet)
        {
            OspPrintf(TRUE, FALSE, "[AMix] Grp.%d call SetNModeNetFeedbackParam failed(%d)!\n", GetInsID()-1, wRet);
            return FALSE;
        }
    }
	// 混音深度
    wOpRet = m_cMixGrp.SetMixerDepth(byDepth);
    if (MIXER_OPERATE_SUCCESS != wOpRet)
    {
        OspPrintf(TRUE, FALSE, "[AMix] Grp.%d call SetMixerDepth failed(%d)!\n", GetInsID()-1, wOpRet);
        return FALSE;
    }

    TMixerParam tPara;
    memset(&tPara, 0, sizeof(tPara));
    tPara.dwType = byAudType;
    tPara.dwVolume = 255;

    mixlog("Grp.%d SetMixerParam type = %d.\n", GetInsID()-1, tPara.dwType);

	//混音参数
    wOpRet = m_cMixGrp.SetMixerParam(&tPara);
    if (MIXER_OPERATE_SUCCESS != wOpRet)
    {
        OspPrintf(TRUE, FALSE, "[AMix] Grp.%d call SetMixerParam failed(%d)!\n", GetInsID()-1, wOpRet);
        return FALSE;
    }

	//编码参数
    SetEncryptParam(ptMediaEncrypt, ptDoublePayload);

    //zbq [10/15/2007] FEC底层支持
    if ( FECTYPE_NONE != byFECType )
    {
        // 音频的FEC底层接口尚未提供
    }

	//设置混音冗余发送；如果FEC，则优先发送采用FEC，保证互斥
	if ( g_cMixerApp.m_bIsSendRedundancy && FECTYPE_NONE == byFECType )
	{
		for (u8 byChnNo=0; byChnNo < g_cMixerApp.m_tMixerCfg.byMaxChannelInGroup; byChnNo++)
		{
           u16 wReturn = m_cMixGrp.SetNetAudioResend(byChnNo, 1, 1);//冗余发送
		   if (CODEC_NO_ERROR != wReturn)
            {
                OspPrintf(TRUE, FALSE, "[AMix] SetNetAudioResend failed err=%d!\n", wReturn );
            }
		}
	}

    NEXTSTATE((s32)TMixerGrpStatus::MIXING);
    m_tGrpStatus.m_byGrpState = (u8)TMixerGrpStatus::MIXING; // 该通道为可用通道

    return TRUE;
}

/*====================================================================
	函数  : StopMix
	功能  : 停止混音
	输入  : 无
	输出  : 无
	返回  : 成功返回TRUE否则返回FALSE
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
BOOL32 CAudMixInst::StopMix()
{
    // 去除对应关系
    ClearMapTable();

	// 调用底层接口
    u16 wRet = m_cMixGrp.StopMixerGroup();
    if (MIXER_OPERATE_SUCCESS != wRet)
    {
        mixlog("Grp.%d call StopMixerGroup failed(%d)!\n", GetInsID(), wRet);
		return FALSE;
    }
    return TRUE;
}

/*====================================================================
	函数  : DestroyeMixGroup()
	功能  : 将混音器释放
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
BOOL32 CAudMixInst::DestroyeMixGroup()
{
    m_cMixGrp.DestroyMixerGroup();
    m_cConfId.SetNull();
    m_tGrpStatus.Reset();
	ClearMapTable();
    return TRUE;
}

/*====================================================================
	函数  : SendMsgToMcu
	功能  : 向MCU发送消息
	输入  : wEvent - 发送事件
	        cServMsg - 业务消息
	输出  : 无
	返回  : 成功返回TRUE否则FALSE
	注    : 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
BOOL32 CAudMixInst::SendMsgToMcu(u16 wEvent, CServMsg const &cServMsg)
{
    BOOL32 bRet = FALSE;
    bRet = SendMsgToMcuA(wEvent, cServMsg);
    bRet &= SendMsgToMcuB(wEvent, cServMsg);
    return bRet;
}

/*====================================================================
	函数  : SendMsgToMcuA
	功能  : 向MCU.A发送消息
	输入  : wEvent - 发送事件
	        cServMsg - 业务消息
	输出  : 无
	返回  : 成功返回TRUE否则FALSE
	注    : 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
BOOL32 CAudMixInst::SendMsgToMcuA(u16 wEvent, CServMsg const &cServMsg)
{
    BOOL bRet = TRUE;
    if (g_cMixerApp.m_bEmbed || OspIsValidTcpNode( g_cMixerApp.m_dwMcuNode ))
    {
        post(g_cMixerApp.m_dwMcuIId, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), g_cMixerApp.m_dwMcuNode);
        mixlog("Send Message %u(%s) to A's Mcu%d\n", wEvent, ::OspEventDesc(wEvent), g_cMixerApp.m_tMixerCfg.wMcuId);
    }
    else
    {
        mixlog("Send Message failed %u(%s) ,since disconnected with MCU%d .\n",
            wEvent, ::OspEventDesc(wEvent), g_cMixerApp.m_tMixerCfg.wMcuId);
        bRet = FALSE;
    }
    return bRet;
}

/*====================================================================
	函数  : SendMsgToMcu
	功能  : 向MCU.B发送消息
	输入  : wEvent - 发送事件
	        cServMsg - 业务消息
	输出  : 无
	返回  : 成功返回TRUE否则FALSE
	注    : 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
BOOL32 CAudMixInst::SendMsgToMcuB(u16 wEvent, CServMsg const &cServMsg)
{
    BOOL bRet = TRUE;
    if( g_cMixerApp.m_bEmbedB || OspIsValidTcpNode(g_cMixerApp.m_dwMcuNodeB))
    {
        post(g_cMixerApp.m_dwMcuIIdB, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), g_cMixerApp.m_dwMcuNodeB);
        mixlog("Send Message %u(%s) to B's Mcu%d\n", wEvent, ::OspEventDesc(wEvent), g_cMixerApp.m_tMixerCfg.wMcuId);
    }
    else
    {
        mixlog("Send Message failed %u(%s) ,since disconnected with MCU%d .\n",
            wEvent, ::OspEventDesc(wEvent), g_cMixerApp.m_tMixerCfg.wMcuId);
        bRet = FALSE;
    }
    return bRet;
}

/*====================================================================
	函数  : 混音组状态上报
	功能  : SendGrpNotif
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CAudMixInst::SendGrpNotif()
{
    CServMsg cServMsg;
    cServMsg.SetConfId(m_cConfId);
    cServMsg.SetMsgBody((u8*)&m_tGrpStatus, sizeof(m_tGrpStatus));
    SendMsgToMcu(MIXER_MCU_GRPSTATUS_NOTIF, cServMsg); 
    return;
}

/*====================================================================
	函数  : MsgAddMixMemberProc
	功能  : 增加混音成员(每次增加一个终端)
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CAudMixInst::MsgAddMixMemberProc(CMessage* const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);
    u8 *pbyMsgBody = cServMsg.GetMsgBody();
    u8  byGrpId  = *pbyMsgBody;
    TMixMember *ptMixMmb = (TMixMember*)(pbyMsgBody + sizeof(u8));
	// 会议号保护
    if (!(m_cConfId == cServMsg.GetConfId()))
    {
        char achCurConfId[64]  = {0};
        char achStopConfId[64] = {0};

        m_cConfId.GetConfIdString(achCurConfId, sizeof(achCurConfId));
        cServMsg.GetConfId().GetConfIdString(achStopConfId, sizeof(achStopConfId));

        cServMsg.SetErrorCode(ERR_MIXER_ERRCONFID);
        SendMsgToMcu(MIXER_MCU_ADDMEMBER_NACK, cServMsg);
        mixlog("Grp.%d Serving %s refuse confId(%s) stop req.\n", GetInsID(), achCurConfId, achStopConfId);
        return;
    }

    // zbq [03/30/2007] 分配新的通道位置
    u8 byIdleChnIdx = cServMsg.GetChnIndex();

	if( 0 == byIdleChnIdx )
	{
		cServMsg.SetErrorCode(ERR_MIXER_CALLDEVICE);
        SendMsgToMcu(MIXER_MCU_ADDMEMBER_NACK,cServMsg);
        OspPrintf(TRUE, FALSE, "[AMix] find idle channel fail.\n");
		return;
	}
    OspPrintf(TRUE, FALSE, "[Add Mem] mt id= %d. idle channel= %d.\n", 
                        ptMixMmb->m_tMember.GetMtId(), byIdleChnIdx );

    // zbq [06/14/2007]
    SetMtIdPosInChnIdx( ptMixMmb->m_tMember.GetMtId(), byIdleChnIdx );

    TMixerChannel tMixChn;
    tMixChn.m_byChannelIndex      = byIdleChnIdx;
    // guzh [5/12/2007] 本地接收地址填 0
    //tMixChn.m_tSrcNetAddr.dwIp    = g_cMixerApp.m_tMixerCfg.dwLocalIP;      // 本地接受端
    tMixChn.m_tSrcNetAddr.dwIp    = 0;                                      // 本地接受端
    tMixChn.m_tSrcNetAddr.wPort   = m_wLocalRcvPort + PORTSPAN*byIdleChnIdx + 2;
    tMixChn.m_tLocalNetAddr.dwIp  = g_cMixerApp.m_tMixerCfg.dwLocalIP;      // 本地发送端
    tMixChn.m_tLocalNetAddr.wPort = m_wLocalRcvPort + PORTSPAN*byIdleChnIdx + 4;
    tMixChn.m_tDstNetAddr.dwIp    = ptMixMmb->m_tAddr.GetIpAddr();          // 发送目的端
    tMixChn.m_tDstNetAddr.wPort   = ptMixMmb->m_tAddr.GetPort();

    if (m_bNeedPrs)
    {
        tMixChn.m_tRtcpBackAddr.dwIp  = ptMixMmb->m_tRtcpBackAddr.GetIpAddr();// Rtcp回馈
        tMixChn.m_tRtcpBackAddr.wPort = ptMixMmb->m_tRtcpBackAddr.GetPort();
    }
    else
    {
        tMixChn.m_tRtcpBackAddr.dwIp  = 0;
        tMixChn.m_tRtcpBackAddr.wPort = 0;
    }
    
    u16 wRet = m_cMixGrp.AddMixerChannel(&tMixChn);
    if (MIXER_OPERATE_SUCCESS == wRet)
    {
        if ( m_bNeedPrs )
        {
            wRet = m_cMixGrp.SetMixerNetFeedbackParam(byIdleChnIdx,  TRUE, g_cMixerApp.m_tNetRSParam, TRUE, 500); // 设置混音重传
        }

        mixlog("Index = %d  Src %0x@%d   Local: %0x@%d Dst %0x@%d  Ret=%d!\n",
                    tMixChn.m_byChannelIndex,
                    tMixChn.m_tSrcNetAddr.dwIp, tMixChn.m_tSrcNetAddr.wPort,
                    tMixChn.m_tLocalNetAddr.dwIp, tMixChn.m_tLocalNetAddr.wPort,
                    tMixChn.m_tDstNetAddr.dwIp, tMixChn.m_tDstNetAddr.wPort, wRet);
        
        m_tGrpStatus.AddMmb(byIdleChnIdx);
        cServMsg.SetErrorCode(0);
        SendMsgToMcu(MIXER_MCU_ADDMEMBER_ACK, cServMsg);

        SendGrpNotif();
    }
    else
    {
        cServMsg.SetErrorCode(ERR_MIXER_CALLDEVICE);
        SendMsgToMcu(MIXER_MCU_ADDMEMBER_NACK,cServMsg);
        OspPrintf(TRUE, FALSE, "[AMix] Grp.%d call AddMixerChannel failed(%d)!\n", byGrpId, wRet);
    }

    return;
}

/*====================================================================
	函数  : MsgSetVacKeepTimeProc
	功能  : 设置混音器语音激励延时保护时长消息(当某个终端被激励后，
	        如果没有音频，则强制保护指定时间长度让其被激励)
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CAudMixInst::MsgSetVacKeepTimeProc( CMessage * const pMsg )
{
    CServMsg cServMsg(pMsg->content,pMsg->length);

    u8 *pbyMsgBody = (u8*)cServMsg.GetMsgBody();
    u8 byGrpId    = *pbyMsgBody;
    u32 dwKeepTime = ntohl(*(u32*)(pbyMsgBody + sizeof(u8)));

    mixlog("Grp.%d call SetExciteKeepTime. para=%d \n", byGrpId, dwKeepTime);
    u16 wRet = m_cMixGrp.SetExciteKeepTime(dwKeepTime);
    if (MIXER_OPERATE_SUCCESS != wRet)
    {
        OspPrintf(TRUE, FALSE, "[AMix] Grp.%d SetExciteKeepTime failed! para=%d.\n", byGrpId, dwKeepTime);
    }

    return;
}

/*====================================================================
	函数  : MsgSetMixSendProc
	功能  : 设置混音器是否发送码流
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CAudMixInst::MsgSetMixSendProc(CMessage *const pMsg)
{
 
    CServMsg cServMsg(pMsg->content, pMsg->length);

    u8 *pbyMsgBody = (u8*)cServMsg.GetMsgBody();

    u8 byGrpId    = *pbyMsgBody;
    u8 bySend     = *(pbyMsgBody + sizeof(u8));

    BOOL32 bSend = (1 == bySend)? FALSE: TRUE;
    mixlog("Grp.%d call stop mixer send. para=%d \n", byGrpId, bSend);
    u16 wRet = m_cMixGrp.StopMixerSnd(bSend);
    if (MIXER_OPERATE_SUCCESS != wRet)
    {
        OspPrintf(TRUE, FALSE, "[AMix] Grp.%d set mixer send failed! Send=%d , wRet = %d.\n",
                               byGrpId, bSend, wRet);
    }
    return;
}

/*====================================================================
	函数  : MsgSetMixDepthProc
	功能  : 设置混音深度(最多参与混音的成员数目)
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CAudMixInst::MsgSetMixDepthProc(CMessage *const pMsg)
{
    // byGrpId = (u8)GetInsID();

    CServMsg cServMsg(pMsg->content,pMsg->length);
    u8 * pbyMsgBody = cServMsg.GetMsgBody();

    u8 byGrpId     = *pbyMsgBody;
    u8 byDepth  = *(pbyMsgBody + sizeof(u8));

    if(!(m_cConfId == cServMsg.GetConfId()))
    {
        s8 achCurConfId[64] ={0};
        s8 achStopConfId[64] ={0};

        m_cConfId.GetConfIdString(achCurConfId, sizeof(achCurConfId));
        cServMsg.GetConfId().GetConfIdString(achStopConfId, sizeof(achStopConfId));

        cServMsg.SetErrorCode(ERR_MIXER_ERRCONFID);
        SendMsgToMcu(MIXER_MCU_SETMIXDEPTH_NACK, cServMsg);
        mixlog("Grp.%d Serving %s refuse confId(%s) stop req.\n", GetInsID(), achCurConfId, achStopConfId);
        return;
    }

    u16 wRet = m_cMixGrp.SetMixerDepth(byDepth);
    if (MIXER_OPERATE_SUCCESS != wRet)
    {
        cServMsg.SetErrorCode(ERR_MIXER_CALLDEVICE);
        SendMsgToMcu(MIXER_MCU_SETMIXDEPTH_NACK, cServMsg);
        OspPrintf(TRUE, FALSE, "[AMix] Grp.%d set depth %d failed!\n", byGrpId, byDepth);
        return;
    }
    m_tGrpStatus.m_byGrpMixDepth = byDepth;
    cServMsg.SetErrorCode(0);
    SendMsgToMcu(MIXER_MCU_SETMIXDEPTH_ACK, cServMsg);

    SendGrpNotif();
}

/*====================================================================
	函数  : MsgSetMixChannelProc
	功能  : 设置混音成员
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CAudMixInst::MsgSetMixChannelProc()
{
/*    u8 byGrpId = (u8)GetInsID();

    CServMsg cServMsg(pMsg->content, pMsg->length);
    u8 *pbyMsgBody = cServMsg.GetMsgBody()+1;

    if (m_cConfId == cServMsg.GetConfId())
    {
        m_cMixGrp.SetMixChnStatus(pbyMsgBody);
    }*/

    OspPrintf(TRUE, FALSE, "[MsgSetMixChannelProc] invalid operation msg !!!\n");
    return;
}

/*====================================================================
	函数  : MsgRemoveMixMemberPorc
	功能  : 删除混音成员
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CAudMixInst::MsgRemoveMixMemberPorc(CMessage *const pMsg)
{
    CServMsg cServMsg(pMsg->content,pMsg->length);
    u8 *pbyMsgBody = cServMsg.GetMsgBody();

    u8 byGrpId     = *pbyMsgBody;
    TMixMember * ptMixMmb    = (TMixMember*)(pbyMsgBody + sizeof(u8));

    if (!(m_cConfId == cServMsg.GetConfId())) // 保护
    {
        s8 achCurConfId[64] ={0};
        s8 achStopConfId[64] ={0};

        m_cConfId.GetConfIdString(achCurConfId, sizeof(achCurConfId));
        cServMsg.GetConfId().GetConfIdString(achStopConfId, sizeof(achStopConfId));

        cServMsg.SetErrorCode(ERR_MIXER_ERRCONFID);
        SendMsgToMcu(MIXER_MCU_REMOVEMEMBER_NACK, cServMsg);
        mixlog("Grp.%d Serving %s refuse confId(%s) remove req.\n", byGrpId, achCurConfId, achStopConfId);
        return;
    }

	u8 byMtId = ptMixMmb->m_tMember.GetMtId();
    u8 byIdleChnIdx = cServMsg.GetChnIndex();
    if( (0 == byIdleChnIdx) || (VALUE_INVALID == byIdleChnIdx) )
    {
        cServMsg.SetErrorCode(ERR_MIXER_ERRCONFID);
        SendMsgToMcu(MIXER_MCU_REMOVEMEMBER_NACK, cServMsg);
        OspPrintf(TRUE, FALSE, "[Amix]Error Mt id %d.\n", byMtId );
        return;
    }
	OspPrintf(TRUE, FALSE, "[Remove Mem] mt id= %d. idle channel= %d.\n", byMtId, byIdleChnIdx );

    // zbq [06/14/2007]
    SetMtIdPosInChnIdx( ptMixMmb->m_tMember.GetMtId(), byIdleChnIdx, TRUE );

    u16 wRet = m_cMixGrp.DeleteMixerChannel(byIdleChnIdx);
    if (MIXER_OPERATE_SUCCESS != wRet)
    {
        cServMsg.SetErrorCode(ERR_MIXER_CALLDEVICE);
        SendMsgToMcu(MIXER_MCU_REMOVEMEMBER_NACK, cServMsg);
        OspPrintf(TRUE, FALSE, "[AMix] Grp.%d call DeleteMixerChannel failed(%d)!\n", byGrpId, wRet);
        return;
    }
    m_tGrpStatus.RemoveMmb(byIdleChnIdx);
    cServMsg.SetErrorCode(0);
    SendMsgToMcu(MIXER_MCU_REMOVEMEMBER_ACK, cServMsg);

    SendGrpNotif();
	return;
}

/*====================================================================
	函数  : SetEncryptParam
	功能  : 设置加密参数
	输入  : 
	输出  : 无
	返回  : 无
	注    : 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    05/01/29    3.6          libo            创建
====================================================================*/
void CAudMixInst::SetEncryptParam(TMediaEncrypt * ptMediaEncrypt, TDoublePayload * ptDoublePayload)
{
    u16 wRet;
    u8  abyKeyBuf[MAXLEN_KEY];
	memset(abyKeyBuf, 0, sizeof(abyKeyBuf));
    s32 wKeySize;
    u8  byDecryptMode;

    byDecryptMode = ptMediaEncrypt->GetEncryptMode();

    mixlog("[SetEncryptParam] RealPT.%d, ActivePT.%d, DecryptMode.%d\n",
            ptDoublePayload->GetRealPayLoad(), 
            ptDoublePayload->GetActivePayload(), ptMediaEncrypt->GetEncryptMode());

    if (byDecryptMode == CONF_ENCRYPTMODE_NONE)// 不需加密
    {
        // zbq [10/13/2007] FEC支持
        u8 byAPayload = 0;
        if ( MEDIA_TYPE_FEC == ptDoublePayload->GetActivePayload() )
        {
            byAPayload = MEDIA_TYPE_FEC;
        }
        u8 byChnNo;
        for (byChnNo=0; byChnNo < g_cMixerApp.m_tMixerCfg.byMaxChannelInGroup; byChnNo++)
        {
            wRet = m_cMixGrp.SetAudEncryptPT(byChnNo, 0);//编码载荷值
            if (CODEC_NO_ERROR != wRet)
            {
                OspPrintf(TRUE, FALSE, "[AMix] SetAudEncryptPT failed err=%d!\n", wRet );
                return;
            }		

            wRet = m_cMixGrp.SetAudEncryptKey(byChnNo, (s8*)NULL, 0, 0);// 编码密字符串
            if (CODEC_NO_ERROR != wRet)
            {
                OspPrintf(TRUE, FALSE, "[AMix] SetAudEncryptKey failed err=%d!\n", wRet );
                return;
            }

            wRet = m_cMixGrp.SetAudioActivePT(byChnNo, byAPayload, byAPayload);// 动态载荷值
            if (MIXER_OPERATE_SUCCESS != wRet)
            {
                OspPrintf(TRUE, FALSE, "[AMix] Grp.%d call \"SetAudioActivePT\" failed.\n", GetInsID());
                return;
            }

            wRet = m_cMixGrp.SetAudDecryptKey(byChnNo, (s8*)NULL, 0, 0);// 解码密字符串		
            if (CODEC_NO_ERROR != wRet)
            {
                OspPrintf(TRUE, FALSE, "[AMix] SetAudDecryptKey failed err=%d!\n", wRet );
                return;
            }
        }

        // xsl [10/25/2006] n 模式只设置编码参数
        byChnNo = g_cMixerApp.m_tMixerCfg.byMaxChannelInGroup;
        wRet = m_cMixGrp.SetAudEncryptPT(byChnNo, byAPayload);//编码载荷值
        if (CODEC_NO_ERROR != wRet)
        {
            OspPrintf(TRUE, FALSE, "[AMix] SetAudEncryptPT failed err=%d!\n", wRet );
            return;
        }		

        wRet = m_cMixGrp.SetAudEncryptKey(byChnNo, (s8*)NULL, 0, 0);// 编码密字符串
        if (CODEC_NO_ERROR != wRet)
        {
            OspPrintf(TRUE, FALSE, "[AMix] SetAudEncryptKey failed err=%d!\n", wRet );
            return;
        }
    }
    else if (byDecryptMode==CONF_ENCRYPTMODE_DES || byDecryptMode==CONF_ENCRYPTMODE_AES) // 需加密
    {
        ptMediaEncrypt->GetEncryptKey(abyKeyBuf, &wKeySize);

        if (CONF_ENCRYPTMODE_DES == byDecryptMode)
        {
            byDecryptMode = DES_ENCRYPT_MODE;
        }
        else
        {
            byDecryptMode = AES_ENCRYPT_MODE;
        }

        // zbq [10/13/2007] FEC支持
        u8 byRealPT = ptDoublePayload->GetRealPayLoad();
        u8 byActivePT = ptDoublePayload->GetActivePayload();
        if ( MEDIA_TYPE_FEC == byActivePT )
        {
            byRealPT = MEDIA_TYPE_FEC;
        }

        u8 byChnNo;
        for (byChnNo=0; byChnNo < g_cMixerApp.m_tMixerCfg.byMaxChannelInGroup; byChnNo++)
        {
            wRet = m_cMixGrp.SetAudEncryptKey(byChnNo, (s8*)abyKeyBuf, (u16)wKeySize, byDecryptMode);
            if (CODEC_NO_ERROR != wRet)
            {
                OspPrintf(TRUE, FALSE, "[AMix] SetAudEncryptKey failed err=%d!\n", wRet);
                return;
            }

            wRet = m_cMixGrp.SetAudEncryptPT(byChnNo, ptDoublePayload->GetActivePayload());
            if (CODEC_NO_ERROR != wRet)
            {
                OspPrintf(TRUE, FALSE, "[AMix] SetAudEncryptPT failed err=%d!\n", wRet);
                return;
            }

            wRet = m_cMixGrp.SetAudioActivePT(byChnNo, byActivePT, byRealPT);
            if (MIXER_OPERATE_SUCCESS != wRet)
            {
                OspPrintf(TRUE, FALSE, "[AMix] Grp.%d call \"SetAudioActivePT\" failed.\n", GetInsID());
                return;
            }
			// 解码密字符串
            wRet = m_cMixGrp.SetAudDecryptKey(byChnNo, (s8*)abyKeyBuf, (u16)wKeySize, byDecryptMode);
            if (CODEC_NO_ERROR != wRet)
            {
                OspPrintf(TRUE, FALSE, "[AMix] SetAudDecryptKey failed err=%d!\n", wRet);
                return;
            }
        }

        // xsl [10/25/2006] n 模式只设置编码参数
        byChnNo = g_cMixerApp.m_tMixerCfg.byMaxChannelInGroup;
        wRet = m_cMixGrp.SetAudEncryptKey(byChnNo, (s8*)abyKeyBuf, (u16)wKeySize, byDecryptMode);
        if (CODEC_NO_ERROR != wRet)
        {
            OspPrintf(TRUE, FALSE, "[AMix] SetAudEncryptKey failed err=%d!\n", wRet);
            return;
        }

        wRet = m_cMixGrp.SetAudEncryptPT(byChnNo, byActivePT);
        if (CODEC_NO_ERROR != wRet)
        {
            OspPrintf(TRUE, FALSE, "[AMix] SetAudEncryptPT failed err=%d!\n", wRet);
            return;
        }
    }
    else
    {
        mixlog("Not supported encrypt mode!");
    }

    mixlog("Decrypt mode is %d, key is %2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d\n", 
                    byDecryptMode, abyKeyBuf[0], abyKeyBuf[1], abyKeyBuf[2], abyKeyBuf[3], abyKeyBuf[4], abyKeyBuf[5], 
                    abyKeyBuf[6], abyKeyBuf[7], abyKeyBuf[8], abyKeyBuf[9], abyKeyBuf[10], abyKeyBuf[11], 
                    abyKeyBuf[12], abyKeyBuf[13], abyKeyBuf[14], abyKeyBuf[15] );
	return;
}

/*====================================================================
	函数  : MsgStartMixProc
	功能  : 创建混音组并开始混音
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	05/01/29    3.6          libo            加密
====================================================================*/
void CAudMixInst::MsgStartMixProc( CMessage* const pMsg )
{
  
    CServMsg cServMsg(pMsg->content, pMsg->length);

    TMixerStart tMixerStart = *(TMixerStart*)cServMsg.GetMsgBody();	
    TDoublePayload tDoublePayload = *(TDoublePayload*)(cServMsg.GetMsgBody() + sizeof(TMixerStart));
	// MCU前向纠错, zgc, 2007-09-27
	TCapSupportEx tCapSupportEx = *(TCapSupportEx*)(cServMsg.GetMsgBody() + sizeof(TMixerStart) + sizeof(TDoublePayload));
	if( g_nPrintlog == 1 )
	{
		tCapSupportEx.Print();
	}

    u8 byDepth = tMixerStart.GetMixDepth();
	mixlog("[MsgStartMixProc]Mix Depth: %d\n", byDepth );
    u8 byType  = tMixerStart.GetAudioMode();
    u8 byMixAll = tMixerStart.IsAllMix();//是否全体混音

    TMediaEncrypt tMediaEncrypt = tMixerStart.GetAudioEncrypt();

    m_bNeedPrs = tMixerStart.IsNeedByPrs();
    switch(CurState())
    {
    //该混音组未被使用
    case TMixerGrpStatus::READY:
        {
			BOOL bRet = TRUE;
			if (!StartMix(byType, byDepth, byMixAll, &tMediaEncrypt, &tDoublePayload, tCapSupportEx.GetAudioFECType()))
			{
				cServMsg.SetErrorCode(ERR_MIXER_CALLDEVICE);
				SendMsgToMcu(MIXER_MCU_STARTMIX_NACK, cServMsg);
				bRet = FALSE;
			}
			if( bRet )
			{
				m_cConfId =	cServMsg.GetConfId();

				cServMsg.SetErrorCode(0);
				SendMsgToMcu(MIXER_MCU_STARTMIX_ACK, cServMsg);

				NEXTSTATE((s32)TMixerGrpStatus::MIXING);
				m_tGrpStatus.m_byGrpState    = (u8)TMixerGrpStatus::MIXING;
				m_tGrpStatus.m_byGrpMixDepth = byDepth;
				SendGrpNotif();
			}
        }
        break;

    case TMixerGrpStatus::MIXING: //当前已开始混音,应拒绝
    case TMixerGrpStatus::IDLE:	  //该混组尚未创建
        cServMsg.SetErrorCode(ERR_MIXER_MIXING);
        SendMsgToMcu(MIXER_MCU_STARTMIX_NACK, cServMsg);
        mixlog("Grp.%d at MIXING state recv stat mixing cmd.\n", GetInsID(), CurState());
        break;
    default:
        mixlog("Grp.%d at undefine state (%d).\n", GetInsID(), CurState());
        break;
    }
	return;
}

/*====================================================================
	函数  : MsgStopMixProc
	功能  : 停止混音
	输入  : 无
	输出  : 无
	返回  : 无
	注    : 停止混音消息,除停止混音外
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CAudMixInst::MsgStopMixProc(CMessage* const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);

    if (!(m_cConfId == cServMsg.GetConfId())) // 保护
    {
        s8 achCurConfId[64] ={0};
        s8 achStopConfId[64] ={0};

        m_cConfId.GetConfIdString(achCurConfId, sizeof(achCurConfId));
        cServMsg.GetConfId().GetConfIdString(achStopConfId, sizeof(achStopConfId));

        cServMsg.SetErrorCode(ERR_MIXER_ERRCONFID);
        SendMsgToMcu(MIXER_MCU_STOPMIX_NACK, cServMsg);
        mixlog("Grp.%d Serving %s refuse confId(%s) stop req.\n", GetInsID(), achCurConfId, achStopConfId);
        return;
    }

    switch(CurState())
    {
    // 暂停或停止状态
    case TMixerGrpStatus::MIXING:
        {		
			if ( StopMix() )
			{				
				SendMsgToMcu(MIXER_MCU_STOPMIX_ACK, cServMsg);
				m_tGrpStatus.m_byGrpState    = (u8)TMixerGrpStatus::READY ;
				m_tGrpStatus.m_byGrpMixDepth = 0;
				NEXTSTATE((s32)TMixerGrpStatus::READY);//进入停止状态
				SendGrpNotif();
				m_cConfId.SetNull();//不为任何会议服务				
			}
			else
			{
				cServMsg.SetErrorCode(ERR_MIXER_CALLDEVICE);
				SendMsgToMcu(MIXER_MCU_STOPMIX_NACK, cServMsg);
			}
        }
        break;

    // 该组并未混音
    case TMixerGrpStatus::READY:
        cServMsg.SetErrorCode(ERR_MIXER_NOTMIXING);
        SendMsgToMcu(MIXER_MCU_STOPMIX_NACK, cServMsg);
        mixlog("Grp.%d at READY state recv Stop Mix cmd.\n", GetInsID()-1);
        break;

    // 该混组尚未创建
    case TMixerGrpStatus::IDLE:
        mixlog("Grp.%d at IDLE state recv Stop Mix cmd.\n", GetInsID()-1);
        break;

    default:
        mixlog("Grp.%d at undefine state (%d).\n", GetInsID()-1, CurState());
        break;
    }
}

/*====================================================================
	函数  : MsgSetVolumeProc
	功能  : 设置音量
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CAudMixInst::MsgSetVolumeProc(CMessage* const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);

    u8 *pbyMsgBody = (u8*)cServMsg.GetMsgBody();
    u8 byGrpId   = *pbyMsgBody;
    TMixMember* ptMixMmb  = (TMixMember*)(pbyMsgBody + sizeof(u8));

    u8 byMtId = ptMixMmb->m_tMember.GetMtId();
    u8 byChnlIdx = cServMsg.GetChnIndex();
	if( 0 == byChnlIdx )
	{
		OspPrintf(TRUE, FALSE, "[AMix] Error Mt id= %d.\n", ptMixMmb->m_tMember.GetMtId() );
		return;
	}
    OspPrintf(TRUE, FALSE, "[MsgSetVolumeProc] mt id= %d. idle channel= %d.\n", byMtId, byChnlIdx );

    u8 byVol  = ptMixMmb->m_byVolume;
    u16 wRet = m_cMixGrp.SetMixerChannelVolume( byChnlIdx, byVol );
    if (MIXER_OPERATE_SUCCESS != wRet)
    {
        OspPrintf(TRUE, FALSE, "[AMix] Grp.%d set mt.%d volume(%d) failed! (%d).\n", byGrpId, byChnlIdx, byVol, wRet);
        return;
    }

    cServMsg.SetMsgBody((u8*)ptMixMmb, sizeof(TMixMember));
    SendMsgToMcu(MIXER_MCU_CHNNLVOL_NOTIF, cServMsg);
	return;
}

/*====================================================================
	函数  : MsgForceActiveProc
	功能  : 强制终端激活，当所有混音通道(若为10个)都被占用时，
		    可以强制指定某个通道(第11个)为强制通道，下次混音时，
			如果该终端有音频，则优先加入混音组，参与混音
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CAudMixInst::MsgForceActiveProc(CMessage* const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);
    u8* pbyMsgBody = cServMsg.GetMsgBody();

    TMixMember* ptMixMmb = (TMixMember*)(pbyMsgBody + sizeof(u8));

    u8 byMtId = ptMixMmb->m_tMember.GetMtId();
	u8 byChnlIdx = cServMsg.GetChnIndex();
    if( 0 == byChnlIdx )
    {
        OspPrintf(TRUE, FALSE, "[AMix] Error Mt id= %d.\n", byChnlIdx );
        return;
    }    
	OspPrintf(TRUE, FALSE, "[MsgForceActiveProc] mt id= %d. idle channel= %d.\n", byMtId, byChnlIdx );

    u16 wRet = m_cMixGrp.SetForceAudMix( byChnlIdx );
    if (CODEC_NO_ERROR != wRet)
    {
        cServMsg.SetErrorCode(ERR_MIXER_CALLDEVICE);
        SendMsgToMcu(MIXER_MCU_FORCEACTIVE_NACK, cServMsg);
    }
    else
    {
        cServMsg.SetErrorCode(0);
        SendMsgToMcu(MIXER_MCU_FORCEACTIVE_ACK, cServMsg);
    }
    return;	
}

/*====================================================================
	函数  : MsgCanelForceActiveProc
	功能  : 取消强制终端激活
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CAudMixInst::MsgCancelForceActiveProc(CMessage* const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);
    u16 wRet = m_cMixGrp.CancelForceAudMix();
    if (CODEC_NO_ERROR != wRet)
    {
        cServMsg.SetErrorCode(ERR_MIXER_CALLDEVICE);
        SendMsgToMcu(MIXER_MCU_CANCELFORCEACTIVE_NACK, cServMsg);
        return;		
    }

    cServMsg.SetErrorCode(0);
    SendMsgToMcu(MIXER_MCU_CANCELFORCEACTIVE_ACK, cServMsg);
    return;	
}

/*=============================================================================
  函 数 名： MsgChangeMixDelay
  功    能： 设置混音延时
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
=============================================================================*/
void CAudMixInst::MsgChangeMixDelay(CMessage* const pMsg)
{
    if( NULL != pMsg )
    {
        CServMsg cMixDelayMsg(pMsg->content, pMsg->length);
        u8* pbyMsgBody = cMixDelayMsg.GetMsgBody();
        u16 wTimeDelay = *(u16*)(pbyMsgBody+sizeof(u8));  // 毫秒

        mixlog("Proc MsgChangeMixDelay. Time: %d\n", wTimeDelay);
        // 设置混音延时
        u16 wRet = m_cMixGrp.SetMixerDelay((u32)wTimeDelay);
        if(CODEC_NO_ERROR != wRet)
        {
            mixlog("Set MixerDelay Error. ret code: %d\n", wRet);
        }
    }
    return;
}

/*====================================================================
	函数  : MsgActiveMmbChangeProc
	功能  : 激活成员变化
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CAudMixInst::MsgActiveMmbChangeProc(CMessage * const pMsg)
{
    u8* pbyCurActiveMmb;
    u8  abyMmb[MAXNUM_MIXER_DEPTH];
    u8  abyMtMmb[MAXNUM_MIXER_DEPTH];
    // guzh [11/30/2007] 准备和硬件数据结构宏分离
    u8 byNum = min(MAXNUM_MIXER_DEPTH, MAX_MIXER_DEPTH);

    CServMsg cServMsg(pMsg->content, pMsg->length);
    pbyCurActiveMmb = (u8*)cServMsg.GetMsgBody();

    memset(abyMmb, 0, MAXNUM_MIXER_DEPTH);
    memcpy(abyMmb, pbyCurActiveMmb, byNum);

    // xsl [9/20/2006] 转换为对应对终端id上报mcu
    memset(abyMtMmb, 0, MAXNUM_MIXER_DEPTH);
    for(u8 byChnnl = 0; byChnnl < byNum; byChnnl++)
    {
        u8 byTmpIdx = abyMmb[byChnnl];
        if (byTmpIdx > 0 && byTmpIdx <= MAX_CHANNEL_NUM)
        {
            abyMtMmb[byChnnl] = m_abyMtId2ChnIdx[byTmpIdx];
        }
    }
    u8 byActChnnl = *(pbyCurActiveMmb+byNum);
    u8 byActMtId = 0;
    if (byActChnnl > 0 && byActChnnl <= MAX_CHANNEL_NUM)
    {
        byActMtId = m_abyMtId2ChnIdx[byActChnnl];
    }

    cServMsg.SetConfId(m_cConfId);
    cServMsg.SetErrorCode(0);
    cServMsg.SetMsgBody(abyMtMmb, MAXNUM_MIXER_DEPTH);
    cServMsg.CatMsgBody(&byActMtId, sizeof(u8));
    SendMsgToMcu(MIXER_MCU_ACTIVEMMBCHANGE_NOTIF, cServMsg);

    m_tGrpStatus.UpdateActiveMmb(abyMmb);
    SendGrpNotif();

    if (g_nPrintlog)
    {
        OspPrintf(TRUE, FALSE, "Current Active Mmember:\n");
        for(s32 nLop = 0; nLop < byNum; nLop++)
        {
            OspPrintf(TRUE, FALSE, " %d(MtId.%d)", abyMmb[nLop], abyMtMmb[nLop]);
        }
        OspPrintf(TRUE, FALSE, "\n");
        OspPrintf(TRUE, FALSE, "Excite Channel No : %d(MtId.%d)\n", byActChnnl, byActMtId);
    }    

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
void CAudMixInst::ProcSetQosInfo(CMessage* const pMsg)
{
    if(NULL == pMsg)
    {
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

	mixlog("Type: %d, Aud= %d, Vid= %d, Data= %d, IpPrior= %d\n", 
		    byQosType, byAudioValue, byVideoValue, byDataValue, byIpPriorValue);

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
		
	mixlog("\nAud= %d, Vid= %d, Data= %d\n", 
					byAudioValue, byVideoValue, byDataValue);

    SetMediaTOS((s32)byAudioValue, QOS_AUDIO);
    SetMediaTOS((s32)byVideoValue, QOS_VIDEO);
    SetMediaTOS((s32)byDataValue, QOS_DATA);

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
void CAudMixInst::ComplexQos(u8& byValue, u8 byPrior)
{
    u8 byLBit = ((byPrior & 0x08) >> 3);
    u8 byRBit = ((byPrior & 0x04) >> 1);
    u8 byTBit = ((byPrior & 0x02)<<1);
    u8 byDBit = ((byPrior & 0x01)<<3);
    
    byValue = byValue + ((byDBit + byTBit + byRBit + byLBit)*2);
    return;
}

/*=============================================================================
    函 数 名： ClearMapTable
    功    能： 情况映射表
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： void 
=============================================================================*/
void CAudMixInst::ClearMapTable( void )
{
	memset( m_abyMtId2ChnIdx, 0, sizeof(m_abyMtId2ChnIdx) );
}

/*====================================================================
 函数名      ：SetMtId2ChnIdx
 功能        ：增加或删除映射表项
 算法实现    ：
 引用全局变量：
 输入参数说明：u8     byMtId
 BOOL32 bRemove
 返回值说明  ：u8: pos
 ----------------------------------------------------------------------
 修改记录    ：
 日  期      版本        修改人        修改内容
 07/06/14    4.0         张宝卿        创建
====================================================================*/
void CAudMixInst::SetMtIdPosInChnIdx( u8 byMtId, u8 byChanIdx, BOOL32 bRemove )
{
    if ( byMtId == 0 || byChanIdx == 0 )
    {
        OspPrintf( TRUE, FALSE, "[SetMtId2ChnIdx] param err: byMtId.%d, byChanIdx.%d \n", byMtId, byChanIdx );
        return;
    }
    
    if ( !bRemove )
    {
        BOOL32 bExist = FALSE;
        
        u8 byPos = 0;
        
        for( byPos = 1; byPos < MAXNUM_MIXER_CHNNL+1; byPos++ )
        {
            if ( byMtId == m_abyMtId2ChnIdx[byPos] && byPos == byChanIdx )
            {
                bExist = TRUE;
                mixlog( "[SetMtId2ChnIdx] Mt.%d already exist at byPos.%d !\n", byMtId, byPos );
                break;
            }
        }
        if ( !bExist )
        {
            m_abyMtId2ChnIdx[byChanIdx] = byMtId;
            mixlog( "[SetMtId2ChnIdx] Mt.%d set succeed, byPos.%d !\n", byMtId, byChanIdx );
        }
    }
    else
    {
        BOOL32 bExist = FALSE;
        u8 byPos = 1;
        for( ; byPos < MAXNUM_MIXER_CHNNL+1; byPos++ )
        {
            if ( byMtId == m_abyMtId2ChnIdx[byPos] && byPos == byChanIdx )
            {
                m_abyMtId2ChnIdx[byPos] = 0;
                bExist = TRUE;
                mixlog( "[SetMtId2ChnIdx] Mt.%d remve m_abyMixMtId succeed, byPos.%d !\n", byMtId, byPos );
                break;
            }
        }
        if ( !bExist )
        {
            OspPrintf( TRUE, FALSE, "[SetMtId2ChnIdx] Mt.%d remve m_abyMixMtId unexist, ignore it !\n", byMtId );
        }
    }
    return;
}

///////////////////////////////////////////////////////////
//                                                       //
//                    调试函数                           //
//                                                       //
///////////////////////////////////////////////////////////
/*=============================================================================
  函 数 名： ShowAllMixGroupStatus
  功    能： 显示所有混音通道状态
  算法实现： 
  全局变量： 
  参    数： CApp* pcApp
  返 回 值： void 
=============================================================================*/
void CAudMixInst::ShowAllMixGroupStatus(CApp* pcApp)
{
	// 打印MTU, zgc, 2007-04-06
	OspPrintf(TRUE, FALSE, "The MTU Size is : %d\n", g_cMixerApp.m_wMTUSize );

    OspPrintf(TRUE, FALSE, "\n\t Mixer Group Status (%d Groups )\n", g_cMixerApp.m_tMixerCfg.byMaxMixGroupCount);
    for(u8 byGrpId = 1; byGrpId <= g_cMixerApp.m_tMixerCfg.byMaxMixGroupCount; byGrpId++)
    {
        CAudMixInst* pInst;
        pInst = (CAudMixInst*)pcApp->GetInstance(byGrpId);
        pInst->MsgGroupShowProc();
    }
    OspPrintf( TRUE ,FALSE ,"\n------------------------------------\n");
	return;
}

/*=============================================================================
  函 数 名： MsgGroupShowProc
  功    能： 显示混音组信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CAudMixInst::MsgGroupShowProc()
{
    u8 byIdx;
    u8 byMmbNum;
    u8 abyActiveMmb[MAXNUM_MIXER_DEPTH];
    //u8 abyMixMmb[256];
    TMixerGroupStatus tMixGrpStatus;
    TMixerChannelStatis tMixerChannelStatis;
    static char status[3][7] = {"  IDLE", " READY", "MIXING"};

    OspPrintf(TRUE, FALSE, "\n----------------    Grp.%d    ----------------\n", m_tGrpStatus.m_byGrpId + 1);
    OspPrintf(TRUE, FALSE, "Status : %s           Depth  : %d\n\n", status[m_tGrpStatus.m_byGrpState], m_tGrpStatus.m_byGrpMixDepth);

    memset(&tMixGrpStatus, 0, sizeof(TMixerGroupStatus));
    m_cMixGrp.GetMixerGroupStatus(tMixGrpStatus);
    OspPrintf(TRUE, FALSE, "MIXER START?(1:ON 0:OFF)		ChnlNum		  N-Mode SndIP\n");
    OspPrintf(TRUE, FALSE, "     %d                            %d         0x%-8x:%-5d\n\n",
                            tMixGrpStatus.m_bMixerStart,
                            tMixGrpStatus.m_byCurChannelNum,
                            ntohl(tMixGrpStatus.m_tNModeDstAddr.dwIp),    //N模式的输出IP地址
                            ntohs(tMixGrpStatus.m_tNModeDstAddr.wPort));  //N模式的输出端口

    OspPrintf(TRUE, FALSE, "                                                  _________Rcv________            ____________Snd__________\n");
    OspPrintf(TRUE, FALSE, "                                                 |                     |          |                          |\n");
    OspPrintf(TRUE, FALSE, "ChID   MtId      SrcIP          ObjIP      BitRate  RcvPackNum  RcvLosePackNum  BitRate  SndPackNum   SndLosePackNum\n");
    for(byIdx=0; byIdx < tMixGrpStatus.m_byCurChannelNum; byIdx++)
    {
        if (byIdx >= MAX_CHANNEL_NUM )
            break;
        
        //打印统计信息
        u16 wRet = 0;
        memset(&tMixerChannelStatis, 0, sizeof(TMixerChannelStatis));
        wRet = m_cMixGrp.GetMixerChannelStatis(byIdx+1, tMixerChannelStatis);
        if (wRet != 0)
        {
            OspPrintf(TRUE, FALSE, "m_cMixGrp.GetMixerChannelStatis(%d, tMixerChannelStatis) = %d\n", byIdx+1, wRet);
        }

        if (tMixGrpStatus.m_atChannel[byIdx].m_tSrcNetAddr.dwIp != 0 ||
            tMixGrpStatus.m_atChannel[byIdx].m_tDstNetAddr.dwIp != 0 ||
            tMixerChannelStatis.m_dwRecvPackNum != 0 ||
            tMixerChannelStatis.m_dwSendPackNum != 0)
        {
            OspPrintf(TRUE, FALSE, "  %d    %d    0x%-8x:%-5d 0x%-8x:%-5d  %4d  %8d %8d %4d  %8d %8d\n",
                                    tMixGrpStatus.m_atChannel[byIdx].m_byChannelIndex,
                                    m_abyMtId2ChnIdx[byIdx+1],
                                    ntohl(tMixGrpStatus.m_atChannel[byIdx].m_tSrcNetAddr.dwIp),
                                    ntohs(tMixGrpStatus.m_atChannel[byIdx].m_tSrcNetAddr.wPort),                                    
                                    ntohl(tMixGrpStatus.m_atChannel[byIdx].m_tDstNetAddr.dwIp),
                                    ntohs(tMixGrpStatus.m_atChannel[byIdx].m_tDstNetAddr.wPort),
                                    tMixerChannelStatis.m_dwRecvBitRate,
                                    tMixerChannelStatis.m_dwRecvPackNum,
                                    tMixerChannelStatis.m_dwRecvLosePackNum,
                                    tMixerChannelStatis.m_dwSendBitRate,
                                    tMixerChannelStatis.m_dwSendPackNum,
                                    tMixerChannelStatis.m_dwMixerDiscardPackNum);
        }
    }

    OspPrintf(TRUE, FALSE, "\nActive Member:\n");
    byMmbNum = m_tGrpStatus.GetActiveMmb(abyActiveMmb, MAXNUM_MIXER_DEPTH);
    for(byIdx=0; byIdx<byMmbNum; byIdx++)
    {
        OspPrintf(TRUE, FALSE, " %3d", abyActiveMmb[byIdx]);
        if (((byIdx+1)%10)==0)
        {
            OspPrintf(TRUE, FALSE, "\n");
        }
    }
    OspPrintf(TRUE, FALSE, "\n");
}

/*=============================================================================
  函 数 名： ShowAllMixGrpChnStatus
  功    能： 显示所有混音通道和终端的对应状态
  算法实现： 
  全局变量： 
  参    数： CApp* pcApp
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  07/03/30    4.0         张宝卿         创建
=============================================================================*/
void CAudMixInst::ShowAllMixGrpChnStatus( CApp* pcApp )
{
    OspPrintf(TRUE, FALSE, "\n\t Mixer Group Channel Status (%d Groups )\n", g_cMixerApp.m_tMixerCfg.byMaxMixGroupCount);
    for(u8 byGrpId = 1; byGrpId <= g_cMixerApp.m_tMixerCfg.byMaxMixGroupCount; byGrpId++)
    {
        CAudMixInst* pInst;
        pInst = (CAudMixInst*)pcApp->GetInstance(byGrpId);
        pInst->MsgGrpChnShowProc();
    }
    OspPrintf( TRUE ,FALSE ,"\n------------------------------------\n");
    return;
}


/*=============================================================================
  函 数 名： MsgGroupShowProc
  功    能： 显示混音组信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  07/03/30    4.0         张宝卿         创建
=============================================================================*/
void CAudMixInst::MsgGrpChnShowProc()
{
    OspPrintf(TRUE, FALSE, "\n----------------    Grp.%d    ----------------\n", m_tGrpStatus.m_byGrpId + 1);
    OspPrintf( TRUE, FALSE, "Total Mix Info: \n" );
    for( u8 byPos = 1; byPos < MAXNUM_MIXER_CHNNL+1; byPos++ )
    {
        if ( 0 != m_abyMtId2ChnIdx[byPos] )
        {
            OspPrintf( TRUE, FALSE, "Pos.%d: MtId = %d \n", byPos, m_abyMtId2ChnIdx[byPos] );
        }
    }
    return;
}

/*lint -save -e1566*/
CMixerCfg::CMixerCfg()
{
    m_dwMcuRcvIp  = 0;
    m_dwMcuRcvIpB = 0;
    m_bEmbed      = FALSE;
    m_bEmbedB     = FALSE;
    m_wMcuRcvStartPort  = 0;
    m_wMcuRcvStartPortB = 0;
    m_byRegAckNum       = 0;
	m_dwMpcSSrc   = 0;
    FreeStatusDataA();
    FreeStatusDataB();
    memset( &m_tNetRSParam, 0, sizeof(TNetRSParam) );
    memset( &m_tMixerCfg,   0, sizeof(TAudioMixerCfg) );
}
/*lint -restore*/

CMixerCfg::~CMixerCfg()
{
}

/*=============================================================================
  函 数 名： g_cMixerApp.FreeStatusDataA
  功    能： 清除状态参数
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CMixerCfg::FreeStatusDataA(void)
{
    m_dwMcuNode = INVALID_NODE;
    m_dwMcuIId  = INVALID_INS;
    return;
}

/*=============================================================================
  函 数 名： g_cMixerApp.FreeStatusDataB
  功    能： 清除状态参数
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CMixerCfg::FreeStatusDataB(void)
{
    m_dwMcuNodeB = INVALID_NODE;
    m_dwMcuIIdB  = INVALID_INS;
    return;
}

/*=============================================================================
  函 数 名： ReadConfigFileApu
  功    能： 读配置文件，取缓冲区参数
  算法实现： 
  全局变量： 
  参    数：  u32& dwStartRcvBufs
             u32& dwFastRcvBufs
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CAudMixInst::ReadConfigFileApu( u32& dwStartRcvBufs, u32& dwFastRcvBufs )
{
	s8	achCfgName[KDV_MAX_PATH] = "";
    sprintf(achCfgName, "%s/apudebug.ini", DIR_CONFIG);
    
	s8  achSectionName[] = "mixparam";
	FILE *pR = fopen( achCfgName, "r" );
	if( NULL == pR )
	{
		OspPrintf( TRUE, FALSE, "the %s is not exist.\n", achCfgName );
		return FALSE;
	}
	
	fclose( pR );
	
	s32 nValue = 0;
	if ( ::GetRegKeyInt(achCfgName, achSectionName, "StartRcvBufs", 0, &nValue) )
	{
		dwStartRcvBufs = (u32)nValue;
	}
	else
	{
		dwStartRcvBufs = 1;
	}

	if ( ::GetRegKeyInt(achCfgName, achSectionName, "FastRcvBufs", 0, &nValue) )
	{
		dwFastRcvBufs = (u32)nValue;
	}
	else
	{
		dwFastRcvBufs = 8;
	}
	return TRUE;
}
