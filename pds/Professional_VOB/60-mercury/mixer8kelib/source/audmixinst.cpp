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
   2009/3/31   8000E 1.0   顾振华
******************************************************************************/      
#include "mcustruct.h"
#include "evmcueqp.h"
#include "evmixer.h"
#include "errmixer.h"
#include "audmixinst.h"
#include "codeccommon.h"
#include "boardagent.h"
#include "mcuver.h"
#include "boardagentbasic.h"


CMixerApp g_cMixerApp;
/*lint -save -e526*/
extern CAudioMixer   * g_apcAMixer[MAXNUM_MIXER_GROUP];        //混音组实例

/*=============================================================================
  函 数 名： MixerCallBackProc
  功    能： 底层回调
  算法实现： 
  全局变量： 
  参    数： u8 byExciteChn
             unsigned int nContext
  返 回 值： void 
=============================================================================*/
static void MixerCallBackProc(u8 byExciteChn, unsigned int nContext)
{
	u8  abyMtMmb[MAXNUM_MIXER_DEPTH];
	memset(abyMtMmb, 0, MAXNUM_MIXER_DEPTH);
    byExciteChn++;
    CServMsg cServMsg;
	cServMsg.SetMsgBody(abyMtMmb, MAXNUM_MIXER_DEPTH);
    cServMsg.CatMsgBody((u8*)&byExciteChn, sizeof(u8));
    if (OSP_OK != ::OspPost(MAKEIID(AID_MIXER, (u16)nContext),
							EV_MIXER_ACTIVECHANGE_NOTIF,cServMsg.GetServMsg(),cServMsg.GetServMsgLen()))
    {
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER,"[MixerCallBackProc][Mixer.%d]MixerCallBackProc() OspPost() FAILED!",nContext);
    }
	else
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,"[MixerCallBackProc][Mixer.%d] Active Change to byExciteChn.%d!\n", nContext, byExciteChn);
	}
    return;
}

/*lint -save -e1566*/
//构造
CAudMixInst::CAudMixInst() : m_bNeedPrs(FALSE),
							 m_byRegAckNum(0),
							 m_wMTUSize(0),
							 m_bIsSendRedundancy(FALSE)
							 
{
    m_pcAMixer = NULL;
    m_cConfId.SetNull();
    m_tGrpStatus.Reset();
	m_dwMcuIId = 0;
	memset(m_abyMtId2ChnIdx,0,sizeof(m_abyMtId2ChnIdx));
	memset(&m_tNetRSParam,0,sizeof(m_tNetRSParam));
    ClearMapTable();
}
/*lint -restore*/

//析构
/*lint -save -e1540*/
CAudMixInst::~CAudMixInst()
{
	if (m_pcAMixer != NULL)
	{
		m_pcAMixer = NULL;
	}
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
	LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,"[DaemonInstanceEntry]Daemon:Recv msg %u(%s).\n", pMsg->event, ::OspEventDesc(pMsg->event));
    switch(pMsg->event)
    {
    //混音器上电初始化
    case OSP_POWERON:
        DaemonProcPowerOnMsg(pMsg);
		break;
	//建联定时器
	case EV_MIXER_CONNECT:
		DaemonProcConnetTimerOut();
		break;
	case EV_MIXER_REGISTER:
		DaemonProcRegTimerOut();
		break;
	case MCU_MIXER_REG_ACK:
		DaemonProcRegAck(pMsg,pcApp);
		break;
	case MCU_MIXER_REG_NACK:
		DaemonProcRegNack();
		break;
	case OSP_DISCONNECT:
		DaemonProcOspDisconnect();
		break;

    // 显示混音组状态
    case EV_MIXER_SHOWGROUP:
		DaemonProcMixerStatus();
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
	LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,"[InstanceEntry][Mixer:%d] Recv msg %u(%s).\n", GetInsID(), pMsg->event, ::OspEventDesc(pMsg->event));

    switch(pMsg->event)
    {
	case EV_MIXER_INIT:
		ProcMsgMixerInit(pMsg);
		break;
		//OSP 断链消息
    case OSP_DISCONNECT:
        MsgDisconnectProc();
		break;
		// 注册消息
    case EV_MIXER_REGISTER:      
        ProcRegisterTimeOut();
        break;
		
		// MCU 注册应答消息
    case MCU_MIXER_REG_ACK:
        MsgRegAckProc(pMsg);
        break;
		
		// MCU拒绝本混音器注册
    case MCU_MIXER_REG_NACK:
        MsgRegNackProc(pMsg);
        break;
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
        MsgSetVolumeProc();
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
        MsgSetMixDepthProc();
        break;
	// 开始发送
    case MCU_MIXER_SEND_NOTIFY:
        MsgSetMixSendProc(pMsg);
        break;
	// 语音激励
    case MCU_MIXER_VACKEEPTIME_CMD:
        MsgSetVacKeepTimeProc(pMsg);
        break;
	// 显示混音组状态
    case EV_MIXER_SHOWGROUP:
        MsgGroupShowProc();
		break;
    default:
        break;
    }
}

/*=============================================================================
  函 数 名： DaemonProcPowerOnMsg
  功    能： 上电消息处理
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： void 
=============================================================================*/
void CAudMixInst::DaemonProcPowerOnMsg(CMessage * const pcMsg)
{
	T8keAudioMixerCfg t8keAudioMixerCfg = *(T8keAudioMixerCfg*)pcMsg->content;
	m_tMixerCfg.SetEqpIp(t8keAudioMixerCfg.GetEqpIp());
	if ( OSP_OK != SetTimer(EV_MIXER_CONNECT, 10) ) //立刻启动建链
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_MIXER,"[DaemonProcPowerOnMsg]SetTimer(EV_MIXER_CONNECT) fail!\n");
	}
	else
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,"[DaemonProcPowerOnMsg]SetTimer(EV_MIXER_CONNECT) successfully!\n");
	}
	return;
}

/*====================================================================
	函数  : ProcMsgMixerInit
	功能  : 初始化
	输入  : CMessage * const pcMsg
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CAudMixInst::ProcMsgMixerInit(CMessage* const pMsg)
{
	if (m_pcAMixer == NULL)
	{
		m_pcAMixer = new CAudioMixer;
		if (m_pcAMixer == NULL)
		{
			LogPrint(LOG_LVL_WARNING, MID_MCU_MIXER, "[ProcMsgMixerInit]new CAudioMixer failed,So return!\n");
			return;
		}
	}
    //用全局变量记录混音器指针, zgc, 2009-11-03
    g_apcAMixer[GetInsID()-1] = m_pcAMixer;
	
    m_tGrpStatus.Reset();
    m_cConfId.SetNull();
	if (pMsg == NULL || pMsg->content == NULL) 
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_MIXER, "[ProcMsgMixerInit]pMsg == NULL || pMsg->content == NULL a,So return!\n");
		return;
	}
	CServMsg cServMsg(pMsg->content, pMsg->length);
	u8 *pMixCfgMsg = (u8 *)cServMsg.GetMsgBody();
	if (pMixCfgMsg == NULL) 
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_MIXER, "[ProcMsgMixerInit]pMixCfgMsg == NULL b,So return!\n");
		return;
	}
	m_dwMcuNode = *(u32*)pMixCfgMsg;
	pMixCfgMsg += sizeof(u32);
	if (pMixCfgMsg == NULL) 
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_MIXER, "[ProcMsgMixerInit]pMixCfgMsg == NULL c,So return!\n");
		return;
	}
	u32 dwLocalIp = *(u32*)pMixCfgMsg;
	pMixCfgMsg += sizeof(u32);
	if (pMixCfgMsg == NULL) 
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_MIXER, "[ProcMsgMixerInit]pMixCfgMsg == NULL c,So return!\n");
		return;
	}
	m_tMixerCfg.SetEqpId( *(u8*)pMixCfgMsg );
	pMixCfgMsg += sizeof(u8);
	if (pMixCfgMsg == NULL) 
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_MIXER, "[ProcMsgMixerInit]pMixCfgMsg == NULL d,So return!\n");
		return;
	}
	m_tMixerCfg.SetMaxChnnlInGrp( *(u8*)pMixCfgMsg );
	pMixCfgMsg += sizeof(u8);
	if (pMixCfgMsg == NULL) 
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_MIXER, "[ProcMsgMixerInit]pMixCfgMsg == NULL e,So return!\n");
		return;
	}
	m_tMixerCfg.SetEqpStartPort(*(u16*)pMixCfgMsg);
	pMixCfgMsg += sizeof(u16);
	if (pMixCfgMsg == NULL) 
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_MIXER, "[ProcMsgMixerInit]pMixCfgMsg == NULL e,So return!\n");
		return;
	}
	char achAlias [ MAXLEN_ALIAS + 1 ] = {0};
	strcpy(achAlias,(char *)pMixCfgMsg);
	m_tMixerCfg.SetEqpIp( dwLocalIp );
	m_tMixerCfg.SetConnectIp(dwLocalIp);
	m_tMixerCfg.SetConnectPort(MCU_LISTEN_PORT);
	m_tMixerCfg.SetEqpType( EQP_TYPE_MIXER );
	m_tMixerCfg.SetMcuId( LOCAL_MCUID );
	m_tMixerCfg.SetAlias(achAlias);
	LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MIXER,"[ProcMsgMixerInit]MixerId(%d) MixMemNum(%d) RcvStartPort(%d) MCUIp(%x) Alias(%s)!\n", 
		m_tMixerCfg.GetEqpId(),m_tMixerCfg.GetMaxChnnlInGrp(),m_tMixerCfg.GetEqpStartPort(),m_tMixerCfg.GetConnectIp(),achAlias);
	if (m_pcAMixer->Init(m_tMixerCfg.GetMaxChnnlInGrp()))
	{
		LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MIXER,"[ProcMsgMixerInit][Mixer:%d]Init(%d) Successfully!\n",
			m_tMixerCfg.GetEqpId(), m_tMixerCfg.GetMaxChnnlInGrp());
		if (OSP_OK == SetTimer(EV_MIXER_REGISTER, 10))
		{
			LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MIXER,"[ProcMsgMixerInit]Mixer(%d) SetTimer(EV_MIXER_REGISTER, 10) Successfully!\n",
				m_tMixerCfg.GetEqpId());
		}
		else
		{
			LogPrint( LOG_LVL_WARNING, MID_MCU_MIXER,"[ProcMsgMixerInit]Mixer(%d) SetTimer(EV_MIXER_REGISTER, 10) failed!\n",
				m_tMixerCfg.GetEqpId());
		}
		u8 byChnNo;
		s32 nSendTimes = 0;
		s32 nSendSpan  = 0;
		
		s32 sdwDefault = 0;
		s32 sdwReturnValue = 0;
		s8  achProfileName[64] = {0};
		memset((void*)achProfileName, 0x0, sizeof(achProfileName));
		sprintf(achProfileName, "%s/%s", DIR_CONFIG, "mcueqp.ini");
		BOOL32 bRet = GetRegKeyInt( achProfileName, SECTION_EqpMixer, KEY_QualityLvl, sdwDefault, &sdwReturnValue);
		//--音质优先等级设置 0:关闭 1:中 2:高 默认是:等级0
		if( sdwReturnValue == 1 )  
		{
			nSendTimes = 1;
			nSendSpan = 1;
		}
		else if( sdwReturnValue == 2 )
		{	
			nSendTimes = 2;
			nSendSpan = 1;
		}
		else
		{	
			nSendTimes = 0;
			nSendSpan = 0;
		}
        for (byChnNo=0; byChnNo < m_tMixerCfg.GetMaxChnnlInGrp(); byChnNo++)
        {
            m_pcAMixer->SetAudioResend(byChnNo,nSendTimes,nSendSpan);
        }
	}
	else
	{
		LogPrint( LOG_LVL_WARNING, MID_MCU_MIXER,"[ProcMsgMixerInit][Mixer:%d]Init(mixnum:%d) Failed!\n",
			m_tMixerCfg.GetEqpId(), m_tMixerCfg.GetMaxChnnlInGrp());
	}
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
BOOL32 CAudMixInst::ConnectMcu(u32& dwMcuNode)
{
    BOOL32 bRet = TRUE;
    if (!OspIsValidTcpNode(dwMcuNode))   
    {    
        dwMcuNode = OspConnectTcpNode(htonl(m_tMixerCfg.GetEqpIp()), 
                                      MCU_LISTEN_PORT, 
                                      10, 3, 100 );  

        if (::OspIsValidTcpNode(dwMcuNode))
        {
            ::OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), CInstance::DAEMON);// 由DAEMON实例处理断链消息
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,"[ConnectMcu][Mixer:%d]Connect Mcu Success! Node(%d)\n",GetInsID(),dwMcuNode);
        }
        else 
        {
            LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER, "[ConnectMcu][Mixer:%d]Connect to Mcu(%s) failed,retry after 3s!\n", 
				GetInsID(), strofip(m_tMixerCfg.GetConnectIp()) );
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
void CAudMixInst::ProcRegisterTimeOut(void)
{
    KillTimer( EV_MIXER_REGISTER );
    switch( CurState() )
    {
    case TMixerGrpStatus::IDLE:
        Register();
        break;
    default:
        LogPrint(TRUE, FALSE, "[CAudMixInst::ProcRegisterTimeOut]: EV_MIXER_REGISTER received in wrong state %u!\n", CurState());
        break;
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
void CAudMixInst::Register()
{
    CServMsg cSvrMsg;
    TEqpRegReq tReg;
    tReg.SetEqpId(m_tMixerCfg.GetEqpId());
    tReg.SetEqpType(m_tMixerCfg.GetEqpType());
    tReg.SetEqpAlias( m_tMixerCfg.GetAlias() );
	tReg.SetEqpIpAddr( m_tMixerCfg.GetEqpIp() );
    tReg.SetVersion(DEVVER_MIXER);
    tReg.SetMcuId(LOCAL_MCUID);
    cSvrMsg.SetMsgBody((u8*)&tReg, sizeof(tReg));
    if (OSP_OK == post( MAKEIID(AID_MCU_PERIEQPSSN, m_tMixerCfg.GetEqpId()), MIXER_MCU_REG_REQ,
		cSvrMsg.GetServMsg(), cSvrMsg.GetServMsgLen(), m_dwMcuNode )) 
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,"[Register]Post MIXER_MCU_REG_REQ to Mcu Successfully,Mixer(%d) Type(%d) EqpIp (%s) EqpStartPort(%d)\n",
			m_tMixerCfg.GetEqpId(),m_tMixerCfg.GetEqpType(),strofip(m_tMixerCfg.GetEqpIp()),m_tMixerCfg.GetEqpStartPort());
    }
	else
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,"[Register]Post MIXER_MCU_REG_REQ to Mcu Failed,Mixer(%d) Type(%d) EqpIp (%s) EqpStartPort(%d)\n",
			m_tMixerCfg.GetEqpId(),m_tMixerCfg.GetEqpType(),strofip(m_tMixerCfg.GetEqpIp()),m_tMixerCfg.GetEqpStartPort());
		return;
	}
	if (OSP_OK == SetTimer(EV_MIXER_REGISTER, MIX_REGISTER_TIMEOUT))
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,"[Register]SetTimer(EV_MIXER_REGISTER, MIX_REGISTER_TIMEOUT) Successfully!\n");
    }
	else
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,"[Register]SetTimer(EV_MIXER_REGISTER, MIX_REGISTER_TIMEOUT) Failed!\n");
	}
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
void CAudMixInst::MsgRegAckProc(CMessage* const pMsg)
{
	
    if ( TMixerGrpStatus::IDLE != CurState() )
    {
		LogPrint(LOG_LVL_WARNING, MID_MCU_MIXER,"[MsgRegAckProc]Current state is not IDLE! Error!\n" );
        return;
    }

    NEXTSTATE(TMixerGrpStatus::READY);

    //CAudMixInst* pInst = NULL;
    TPeriEqpStatus tEqpStatus;
    TEqpRegAck* ptRegAck = NULL;
    CServMsg cServMsg(pMsg->content, pMsg->length);
    ptRegAck =(TEqpRegAck*)cServMsg.GetMsgBody();

    const TPrsTimeSpan tPrsTimeSpan = *(TPrsTimeSpan *)(cServMsg.GetMsgBody() + sizeof(TEqpRegAck));
	// 获得MTU size, zgc, 2007-04-02
	u16 wMTUSize = *(u16*)( cServMsg.GetMsgBody() + sizeof(TEqpRegAck) + sizeof(TPrsTimeSpan) );
	wMTUSize = ntohs( wMTUSize );
	//u8 byIsSendRedundancy = 0;//*(u8*)( cServMsg.GetMsgBody() + sizeof(TEqpRegAck) + sizeof(TPrsTimeSpan) + sizeof(u16) );

	// 网络重传参数
	m_tNetRSParam.m_wFirstTimeSpan  = ntohs(tPrsTimeSpan.m_wFirstTimeSpan);
    m_tNetRSParam.m_wSecondTimeSpan = ntohs(tPrsTimeSpan.m_wSecondTimeSpan);
    m_tNetRSParam.m_wThirdTimeSpan  = ntohs(tPrsTimeSpan.m_wThirdTimeSpan);
    m_tNetRSParam.m_wRejectTimeSpan = ntohs(tPrsTimeSpan.m_wRejectTimeSpan);


    LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,"[MsgRegAckProc][Mixer:%d]m_wFirstTimeSpan = %d m_wSecondTimeSpan = %d, m_wThirdTimeSpan = %d, m_wRejectTimeSpan = %d!\n",
            GetInsID(),m_tNetRSParam.m_wFirstTimeSpan,m_tNetRSParam.m_wSecondTimeSpan,
			m_tNetRSParam.m_wThirdTimeSpan,m_tNetRSParam.m_wRejectTimeSpan);
    
    if( m_dwMcuNode == pMsg->srcnode)// 注册成功
    {
        m_tMixerCfg.SetMcuSwitchIp( ptRegAck->GetMcuIpAddr() );
        m_tMixerCfg.SetMcuStartPort( ptRegAck->GetMcuStartPort() );

        m_dwMcuIId = pMsg->srcid;
        m_byRegAckNum++;
        KillTimer(EV_MIXER_REGISTER);
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,"[MsgRegAckProc][Mixer:%d]Register A's Mcu%d  success !\n",GetInsID(), m_tMixerCfg.GetMcuId() );

        tEqpStatus.SetMcuEqp( (u8)m_tMixerCfg.GetMcuId(), m_tMixerCfg.GetEqpId(), m_tMixerCfg.GetEqpType() );
    }

    if(FIRST_REGACK == m_byRegAckNum)  // 防止另外一个注册消息更改状态
    {
		// 设置MTU大小, zgc, 2007-04-02
		m_wMTUSize = wMTUSize;
		//SetVideoSendPacketLen( (s32)g_cMixerApp.m_wMTUSize );
		// 记录是否冗余发送, zgc, 2007-07-25
		m_bIsSendRedundancy = FALSE;
    }

	tEqpStatus.m_tStatus.tMixer.m_byMixOffChnNum = m_tMixerCfg.GetMaxChnnlInGrp();
	m_tGrpStatus.m_byGrpState = TMixerGrpStatus::READY;
	m_tGrpStatus.m_byMixGrpChnNum = m_tMixerCfg.GetMaxChnnlInGrp();
    memcpy((void *)&tEqpStatus.m_tStatus.tMixer.m_atGrpStatus[0],
               (void *)&m_tGrpStatus, sizeof(TMixerGrpStatus));

    tEqpStatus.m_tStatus.tMixer.m_byGrpNum = 1;
    

    tEqpStatus.m_byOnline = (u8)TRUE;

    tEqpStatus.SetAlias(m_tMixerCfg.GetAlias());

    cServMsg.SetMsgBody((u8*)&tEqpStatus, sizeof(tEqpStatus));
    SendMsgToMcu(MIXER_MCU_MIXERSTATUS_NOTIF, cServMsg);
	SendGrpNotif();
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
    if(pMsg->srcnode == m_dwMcuNode)
    {
	    SetTimer(EV_MIXER_REGISTER, MIX_REGISTER_TIMEOUT);
		LogPrint(LOG_LVL_WARNING, MID_MCU_MIXER,"[MsgRegNackProc][Mixer:%d]Mixer registe be refused by A's Mcu%d .\n",GetInsID(), m_tMixerCfg.GetMcuId());
    }

    return;
}

/*====================================================================
函数名      ：MsgDisconnectProc
功能        ：处理断链消息
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2010/01/20  1.0         田志勇          创建
====================================================================*/
void CAudMixInst::MsgDisconnectProc()
{
	NEXTSTATE(TMixerGrpStatus::IDLE);
	m_tGrpStatus.m_byGrpState = TMixerGrpStatus::IDLE;
	SetTimer(EV_MIXER_CONNECT, MIX_CONNETC_TIMEOUT);
	m_tMixerCfg.SetMcuSwitchIp(0);
	m_tMixerCfg.SetMcuStartPort(0);
	m_dwMcuIId = INVALID_INS;
	m_dwMcuNode = INVALID_NODE;
}

/*====================================================================
	函数  : StartMix
	功能  : 开始混音
	输入  : byAudioType -  语音类型
	输出  : 无
	返回  : 成功返回TRUE否则返回FALSE
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
#if !defined(_8KH_) && !defined(_8KE_)
u8 CAudMixInst::GetAudioMode(const u8 byAudioType, u8 byAudioTrackNum)
{
    u8 byAudMode = 0;
    switch (byAudioType)
    {
    case MEDIA_TYPE_G7221C://G.722.1.C Polycom Siren14
        byAudMode = AUDIO_MODE_G7221;//采样率
        break;
    case MEDIA_TYPE_PCMA://G.711a
        byAudMode = AUDIO_MODE_PCMA;
        break;
    case MEDIA_TYPE_PCMU://G.711u
        byAudMode = AUDIO_MODE_PCMU;
        break;
    case MEDIA_TYPE_G722://G.722
        byAudMode = AUDIO_MODE_G722;
        break;
    case MEDIA_TYPE_G728://G.728
        byAudMode = AUDIO_MODE_G728;
        break;
    case MEDIA_TYPE_G729://G.729
        byAudMode = AUDIO_MODE_G729;
        break;
    case MEDIA_TYPE_G719://G.719
        byAudMode = AUDIO_MODE_G719;
        break;
    case MEDIA_TYPE_MP3://MP3
        byAudMode = AUDIO_MODE_MP3;
        break;
    case MEDIA_TYPE_AACLC://MPEG4 AAL-LC
		if (1 == byAudioTrackNum)
		{
			byAudMode = AUDIO_MODE_AACLC_32_M;
		}
        else if (2 == byAudioTrackNum)
		{
			byAudMode = AUDIO_MODE_AACLC_32;
		}
        break;
    case MEDIA_TYPE_AACLD://MPEG4 AAL-LD
		if (1 == byAudioTrackNum)
		{
			byAudMode = AUDIO_MODE_AACLD_32_M;
		}
		else if (2 == byAudioTrackNum)
		{
			byAudMode = AUDIO_MODE_AACLD_32;
		}
        break;
    default:
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "GetAudioMode MediaType.%d not surportted!!\n", byAudioType);
        return 0;
    }
    return byAudMode;
}
#endif

/*====================================================================
	函数  : StartMix
	功能  : 开始混音
	输入  : byAudioType -  语音类型
	输出  : 无
	返回  : 成功返回TRUE否则返回FALSE
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
BOOL32 CAudMixInst::StartMix(TMediaEncrypt * ptMediaEncrypt, 
                             TDoublePayload * ptDoublePayload,
							 u8 byNChlNum /*= 0*/,
							 TAudioCapInfo *ptAudioCapInfo /*= NULL*/)
{
    //u16 wInst = GetInsID();
    // 开始混音
    BOOL32 bRet = TRUE;
    bRet = m_pcAMixer->StartMixer();
    if (!bRet)
    {
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER,"[StartMix][Mixer:%d]StartMix SetMixerCallBack failed.\n", GetInsID());
        return FALSE;
    }

	// 设置回调
    bRet = m_pcAMixer->SetMixerCallBack(MixerCallBackProc, (u32)GetInsID());
    if (!bRet)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER,"[StartMix][Mixer:%d]StartMix SetMixerCallBack failed.\n", GetInsID());
        return FALSE;
    }

    // 混音深度
    u8 byDepth = MAXNUM_MIXER_DEPTH;
    m_pcAMixer->SetMixerDepth(byDepth);

    TNetAddress tMcuAddr[3];
    
    TNetAddress  tLocalAddr[3];
    
    // N模式输出
#if !defined(_8KH_) && !defined(_8KE_)
	//BOOL32 SetNModeOutput(TNetAddress* ptLocalAddr, TNetAddress* ptDstNetAddr,u32 dwNetAddrNum, u16 wTimeSan, BOOL32 bRepeat = FALSE); 

	u8 byAudioMode = 0;
	u16 wRet = Codec_Success;
	for( u8 byIdx = 0;byIdx < byNChlNum; ++byIdx )
	{
		tMcuAddr[byIdx].dwIp  = htonl(m_tMixerCfg.GetMcuSwitchIp());
		tMcuAddr[byIdx].wPort = m_tMixerCfg.GetMcuStartPort() + m_tMixerCfg.GetMaxChnnlInGrp() * PORTSPAN + byIdx * PORTSPAN + 2; 
		tLocalAddr[byIdx].dwIp  = htonl(m_tMixerCfg.GetEqpIp());
		tLocalAddr[byIdx].wPort = m_tMixerCfg.GetEqpStartPort() + m_tMixerCfg.GetMaxChnnlInGrp() * PORTSPAN + byIdx * PORTSPAN + 2;
		LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,"[Mixer:%d]NMode:chl.%d Local:%s:%d, Mcu:%s:%d\n", GetInsID(), byIdx,
		   strofip(tLocalAddr[byIdx].dwIp, TRUE), tLocalAddr[byIdx].wPort, strofip(tMcuAddr[byIdx].dwIp, TRUE), tMcuAddr[byIdx].wPort);

		byAudioMode = GetAudioMode( ptAudioCapInfo[byIdx].GetAudioMediaType(), ptAudioCapInfo[byIdx].GetAudioTrackNum() );
		wRet = m_pcAMixer->SetAudEncParam( byIdx, ptAudioCapInfo[byIdx].GetAudioMediaType(), byAudioMode, TRUE );
		if(wRet != (u16)Codec_Success)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, 
				"[AddMixMem] Mixer.%d chl.%d SetAudEncParam Index.%d failed because.%d\n", GetInsID(), byIdx, wRet);
		}
	}
	
    bRet = m_pcAMixer->SetNModeOutput(&tLocalAddr[0], &tMcuAddr[0], byNChlNum, 2000, m_bNeedPrs);
#else
	tMcuAddr[0].dwIp  = htonl(m_tMixerCfg.GetMcuSwitchIp());
    tMcuAddr[0].wPort = m_tMixerCfg.GetMcuStartPort() + 2; 
	tLocalAddr[0].dwIp  = htonl(m_tMixerCfg.GetEqpIp());
    tLocalAddr[0].wPort = m_tMixerCfg.GetEqpStartPort() + 2;
	bRet = m_pcAMixer->SetNModeOutput(&tLocalAddr[0], &tMcuAddr[0], 2000, m_bNeedPrs);
	LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,"[Mixer:%d]NMode: Local:%s:%d, Mcu:%s:%d\n", GetInsID(),
		   strofip(tLocalAddr[0].dwIp, TRUE), tLocalAddr[0].wPort, strofip(tMcuAddr[0].dwIp, TRUE), tMcuAddr[0].wPort);

	byNChlNum = 1;
#endif
    if (!bRet)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER,"[StartMix]Mixer.%d call SetNModeOutput failed!\n", m_tGrpStatus.m_byGrpId);
        return FALSE;
    }
    
	//编码参数
    SetEncryptParam(ptMediaEncrypt, ptDoublePayload, byNChlNum, ptAudioCapInfo);

    NEXTSTATE(TMixerGrpStatus::MIXING);
    m_tGrpStatus.m_byGrpState = TMixerGrpStatus::MIXING; 
    m_tGrpStatus.m_byGrpMixDepth = byDepth;

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

    BOOL32 bRet = m_pcAMixer->StopMixer();
    if (!bRet)
    {
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER,"[Mixer:%d] call StopMixerGroup failed!\n",GetInsID());
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
	if (!OspIsValidTcpNode( m_dwMcuNode )) 
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER, "[Mixer:%d]m_dwMcuNode(%d) OspIsValidTcpNode,So Return!\n",GetInsID(), m_dwMcuNode);
		return FALSE;
	}
    if (OSP_OK == post(m_dwMcuIId, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), m_dwMcuNode))
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER, "[Mixer:%d]Send Message %u(%s) to Mcu Successfully!\n",GetInsID(), wEvent, ::OspEventDesc(wEvent) );
    }
    else
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER, "[Mixer:%d]Send Message %u(%s) to Mcu Failed!\n",GetInsID(), wEvent, ::OspEventDesc(wEvent) );
		return FALSE;
    }

    return TRUE;
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
    cServMsg.SetChnIndex( (u8)GetInsID()-1 );
	m_tGrpStatus.m_byGrpId = 0;
    cServMsg.CatMsgBody((u8*)&m_tGrpStatus, sizeof(m_tGrpStatus));
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
    TMixMember *ptMixMmb = (TMixMember*)(pbyMsgBody + sizeof(u8));
	pbyMsgBody += sizeof(u8) *2 + sizeof(TMixMember);

	u8 bySampleRate = *(u8 *)pbyMsgBody;
	pbyMsgBody += sizeof(u8);

	TAudioCapInfo tDecAudioInfo = *(TAudioCapInfo*)pbyMsgBody;
	pbyMsgBody += sizeof(TAudioCapInfo);

	TAudioCapInfo tEncAudioInfo = *(TAudioCapInfo*)pbyMsgBody;
	

    // zbq [03/30/2007] 分配新的通道位置
    u8 byIdleChnIdx = cServMsg.GetChnIndex();

#ifndef _8KI_
	if( 0 == byIdleChnIdx )
	{
		cServMsg.SetErrorCode(ERR_MIXER_CALLDEVICE);
        SendMsgToMcu(MIXER_MCU_ADDMEMBER_NACK,cServMsg);
		LogPrint(LOG_LVL_WARNING, MID_MCU_MIXER,"[AMix] find idle channel fail.\n");
		return;
	}
#endif

    LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,"[Mixer:%d][Add Member] mt id= %d, at channel %d.\n", GetInsID(),
            ptMixMmb->m_tMember.GetMtId(), byIdleChnIdx );

    // zbq [06/14/2007]
    SetMtIdPosInChnIdx( ptMixMmb->m_tMember.GetMtId(), byIdleChnIdx );

    TMixerChannelParam tChannelParam;

    //u16 wLocalRcvPort = m_wEqpRcvStartPort;
    tChannelParam.m_tSrcNetAddr.dwIp    = 0;                                      // 本地接受端
    tChannelParam.m_tSrcNetAddr.wPort   = m_tMixerCfg.GetEqpStartPort() + PORTSPAN*byIdleChnIdx + 2;
    tChannelParam.m_tLocalNetAddr.dwIp  = htonl(m_tMixerCfg.GetEqpIp());      // 本地发送端
    tChannelParam.m_tLocalNetAddr.wPort = m_tMixerCfg.GetEqpStartPort() + PORTSPAN*byIdleChnIdx + 4;
    tChannelParam.m_tDstNetAddr.dwIp    = htonl(ptMixMmb->m_tAddr.GetIpAddr());          // 发送目的端
    tChannelParam.m_tDstNetAddr.wPort   = ptMixMmb->m_tAddr.GetPort();

    if (m_bNeedPrs)
    {
		//[nizhijun 2011/05/24] 混音器接收丢包时，需设置本地绑定地址以及远端rtcp地址
		//本地RTCP发送
		tChannelParam.m_tRtcpLocalAddr.dwIp  = htonl(m_tMixerCfg.GetEqpIp());
		tChannelParam.m_tRtcpLocalAddr.wPort = m_tMixerCfg.GetEqpStartPort() + PORTSPAN*byIdleChnIdx + 6;//目前暂定为39916偏移

		// 远端Rtcp回馈
        tChannelParam.m_tRtcpBackAddr.dwIp  = htonl(ptMixMmb->m_tRtcpBackAddr.GetIpAddr());
        tChannelParam.m_tRtcpBackAddr.wPort = ptMixMmb->m_tRtcpBackAddr.GetPort();
    }
    else
    {
		tChannelParam.m_tRtcpLocalAddr.dwIp = 0;
		tChannelParam.m_tRtcpLocalAddr.wPort = 0;

        tChannelParam.m_tRtcpBackAddr.dwIp  = 0;
        tChannelParam.m_tRtcpBackAddr.wPort = 0;
    }    

#if defined(_8KH_) || defined(_8KE_)
	byIdleChnIdx -= 1;
#endif
    
	//BOOL32 bRet = m_pcAMixer->DeleteMixerChannel(byIdleChnIdx - 1);
	
    BOOL32 bRet = m_pcAMixer->AddMixerChannel(byIdleChnIdx, &tChannelParam);
    
	if ( bRet )
    {
		
#if !defined(_8KH_) && !defined(_8KE_) && !defined(WIN32)
		u8 byAudioMode = GetAudioMode( tDecAudioInfo.GetAudioMediaType(), tDecAudioInfo.GetAudioTrackNum() );

		if( !m_pcAMixer->SetAudioDecParam( byIdleChnIdx, tDecAudioInfo.GetAudioMediaType(), byAudioMode) )
		{
			cServMsg.SetErrorCode(ERR_MIXER_CALLDEVICE);
			SendMsgToMcu(MIXER_MCU_ADDMEMBER_NACK,cServMsg);
			LogPrint(LOG_LVL_WARNING, MID_MCU_MIXER,"[AMix] Grp.%d ChanIdx.%d SetAudioDecParam failed!\n", m_tGrpStatus.m_byGrpId, byIdleChnIdx);
		}

		byAudioMode = GetAudioMode( tEncAudioInfo.GetAudioMediaType(), tEncAudioInfo.GetAudioTrackNum() );
		if( !m_pcAMixer->SetAudEncParam( byIdleChnIdx, tEncAudioInfo.GetAudioMediaType(), byAudioMode) )
		{
			cServMsg.SetErrorCode(ERR_MIXER_CALLDEVICE);
			SendMsgToMcu(MIXER_MCU_ADDMEMBER_NACK,cServMsg);
			LogPrint(LOG_LVL_WARNING, MID_MCU_MIXER,"[AMix] Grp.%d ChanIdx.%d SetAudEncParam failed!\n", m_tGrpStatus.m_byGrpId, byIdleChnIdx);
		}
#endif

        if ( m_bNeedPrs )
        {			
			//解码接收：混音器接收丢包，向终端发RTCP申述包 
			m_pcAMixer->SetFeedbackParam(byIdleChnIdx, m_tNetRSParam, TRUE); 
			LogPrint(TRUE, FALSE, "[MsgAddMixMemberProc] SetFeedbackParam!\n");
        }

		//[2011/05/26] 编码发送为适应底层，故是否需要丢包，让底层去感知
		//编码发送：混音接收终端丢包后，向混音器请求，混音器需要设置定时时间
		m_pcAMixer->SetSndFeedbackParam(byIdleChnIdx, 2000, m_bNeedPrs);

        LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,"[Mixer:%d]ChanIdx = %d  LocalRecvPort: %d LocalSendPort: %d SendDst: %s:%d\n",GetInsID(),
                byIdleChnIdx,
                tChannelParam.m_tSrcNetAddr.wPort,tChannelParam.m_tLocalNetAddr.wPort,
                strofip(tChannelParam.m_tDstNetAddr.dwIp, TRUE), tChannelParam.m_tDstNetAddr.wPort);
     
        cServMsg.SetErrorCode(0);
        SendMsgToMcu(MIXER_MCU_ADDMEMBER_ACK, cServMsg);

        SendGrpNotif();
    }
    else
    {
        cServMsg.SetErrorCode(ERR_MIXER_CALLDEVICE);
        SendMsgToMcu(MIXER_MCU_ADDMEMBER_NACK,cServMsg);
		LogPrint(LOG_LVL_WARNING, MID_MCU_MIXER,"[AMix] Grp.%d ChanIdx.%d SetChannelParam failed!\n", m_tGrpStatus.m_byGrpId, byIdleChnIdx);
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

    LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,"[Mixer:%d]%d call SetExciteKeepTime to %u\n", byGrpId, dwKeepTime);
    BOOL32 bRet = m_pcAMixer->SetExciteKeepTime(dwKeepTime);
    if (!bRet)
    {
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER,"[AMix] Grp.%d SetExciteKeepTime to %u failed!\n", byGrpId, dwKeepTime);
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

    BOOL32 bNotSend = (1 == bySend)? FALSE: TRUE;  //NOTICE：反义
    LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,"[Mixer:%d]%d %s mixer encode and send.\n",GetInsID(), byGrpId, bNotSend ? "STOP" : "START");
    BOOL32 bRet = m_pcAMixer->StopMixerSnd(bNotSend);
    if (!bRet)
    {
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER,"[AMix] Grp.%d set mixer send failed!\n",byGrpId);
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
void CAudMixInst::MsgSetMixDepthProc()
{
    /*
    CServMsg cServMsg(pMsg->content,pMsg->length);
    u8 * pbyMsgBody = cServMsg.GetMsgBody();

    u8 byGrpId     = *pbyMsgBody;
    u8 byDepth  = *(pbyMsgBody + sizeof(u8));

    BOOL32 bRet = m_pcAMixer->SetMixerDepth(byDepth);
    if (!bRet)
    {
        cServMsg.SetErrorCode(ERR_MIXER_CALLDEVICE);
        SendMsgToMcu(MIXER_MCU_SETMIXDEPTH_NACK, cServMsg);
        LogPrint(TRUE, FALSE, "[AMix] Grp.%d set depth %d failed!\n", byGrpId, byDepth);
        return;
    }

    m_tGrpStatus.m_byGrpMixDepth = byDepth;
    cServMsg.SetErrorCode(0);
    SendMsgToMcu(MIXER_MCU_SETMIXDEPTH_ACK, cServMsg);

    SendGrpNotif();
    */
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

	u8 byMtId = ptMixMmb->m_tMember.GetMtId();
    u8 byChnIdx = cServMsg.GetChnIndex();

	LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,"[Mixer:%d][Remove Member] mt id= %d at channel %d.\n",GetInsID(), byMtId, byChnIdx );

    // zbq [06/14/2007]
    SetMtIdPosInChnIdx( ptMixMmb->m_tMember.GetMtId(), byChnIdx, TRUE );

#if defined(_8KE_) || defined(_8KH_)
	byChnIdx -= 1;
#endif

    BOOL32 bRet = m_pcAMixer->DeleteMixerChannel(byChnIdx);
    if (!bRet)
    {
        cServMsg.SetErrorCode(ERR_MIXER_CALLDEVICE);
        SendMsgToMcu(MIXER_MCU_REMOVEMEMBER_NACK, cServMsg);
		LogPrint(LOG_LVL_WARNING, MID_MCU_MIXER,"[AMix] Grp.%d DeleteMixerChannel failed!\n", byGrpId);
        return;
    }

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
void CAudMixInst::SetEncryptParam(TMediaEncrypt * ptMediaEncrypt, TDoublePayload * ptDoublePayload,
										u8 byNModeChlNum,
										TAudioCapInfo *ptAudioCapInfo )
{
    u8  abyKeyBuf[MAXLEN_KEY];
	memset(abyKeyBuf, 0, sizeof(abyKeyBuf));
    s32 wKeySize;
    

    u8 byEncryptMode = ptMediaEncrypt->GetEncryptMode();

    LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,"[Mixer:%d][SetEncryptParam] RealPT.%d, ActivePT.%d, DecryptMode.%d\n",GetInsID(),
            ptDoublePayload->GetRealPayLoad(), 
            ptDoublePayload->GetActivePayload(), ptMediaEncrypt->GetEncryptMode());

    u8 byActivePt = ptDoublePayload->GetActivePayload();
    u8 byRealPt   =  ptDoublePayload->GetRealPayLoad();

	u8 byChnNo = 0;
    if (byEncryptMode == CONF_ENCRYPTMODE_NONE)// 不需加密
    {       
        
        for (byChnNo=0; byChnNo < m_tMixerCfg.GetMaxChnnlInGrp(); byChnNo++)
        {
            m_pcAMixer->SetEncryptKey(byChnNo, (s8*)NULL, 0, 0);// 编码密字符串

            m_pcAMixer->SetAudioActivePT(byChnNo, byActivePt, byRealPt);// 动态载荷值
            m_pcAMixer->SetDecryptKey(byChnNo, (s8*)NULL, 0, 0);// 解码密字符串
#if !defined(_8KH_) && !defined(_8KE_)
			m_pcAMixer->SetActivePT( byChnNo,byActivePt );//设置发送动态载荷
#endif
        }

        // xsl [10/25/2006] n 模式只设置编码参数
        
		for( u8 byIdx = 0;byIdx < byNModeChlNum; ++byIdx )
		{
#if defined(_8KH_) || defined(_8KE_)
			byChnNo = m_tMixerCfg.GetMaxChnnlInGrp();
			m_pcAMixer->SetEncryptKey( byChnNo+byIdx, (s8*)NULL, 0, 0 );// 编码密字符串			
#else
			m_pcAMixer->SetEncryptKey(byIdx, (s8*)NULL, 0, 0, TRUE);// 编码密字符串
			m_pcAMixer->SetActivePT( byIdx,ptAudioCapInfo[byIdx].GetActivePayLoad(),TRUE);//设置发送动态载荷
#endif
		}
        
    }
    else if (byEncryptMode==CONF_ENCRYPTMODE_DES || byEncryptMode==CONF_ENCRYPTMODE_AES) // 需加密
    {
        ptMediaEncrypt->GetEncryptKey(abyKeyBuf, &wKeySize);

        if (CONF_ENCRYPTMODE_DES == byEncryptMode)
        {
            byEncryptMode = DES_ENCRYPT_MODE;
        }
        else
        {
            byEncryptMode = AES_ENCRYPT_MODE;
        }

        
        for (byChnNo=0; byChnNo < m_tMixerCfg.GetMaxChnnlInGrp(); byChnNo++)
        {
            m_pcAMixer->SetEncryptKey(byChnNo, (s8*)abyKeyBuf, (u16)wKeySize, byEncryptMode);

            m_pcAMixer->SetAudioActivePT(byChnNo, byActivePt, byRealPt);
            m_pcAMixer->SetDecryptKey(byChnNo, (s8*)abyKeyBuf, (u16)wKeySize, byEncryptMode);
#if !defined(_8KH_) && !defined(_8KE_)
			m_pcAMixer->SetActivePT( byChnNo,byActivePt);//设置发送动态载荷
#endif
        }

        
		for( u8 byIdx = 0;byIdx < byNModeChlNum; ++byIdx )
		{
#if defined(_8KH_) || defined(_8KE_)
			byChnNo = m_tMixerCfg.GetMaxChnnlInGrp();
			m_pcAMixer->SetEncryptKey(byChnNo+byIdx, (s8*)abyKeyBuf, (u16)wKeySize, byEncryptMode );			
#else
			m_pcAMixer->SetEncryptKey(byIdx, (s8*)abyKeyBuf, (u16)wKeySize, byEncryptMode, TRUE);
			m_pcAMixer->SetActivePT( byIdx,ptAudioCapInfo[byIdx].GetActivePayLoad(),TRUE);//设置发送动态载荷
#endif
		}        
    }
    else
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,"Not supported encrypt mode!");
    }
#if defined(_8KH_) || defined(_8KE_)
	m_pcAMixer->SetActivePT(byActivePt);
#endif
	
    /*
    LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,"Decrypt mode is %d, key is %2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d\n", 
                    byEncryptMode, abyKeyBuf[0], abyKeyBuf[1], abyKeyBuf[2], abyKeyBuf[3], abyKeyBuf[4], abyKeyBuf[5], 
                    abyKeyBuf[6], abyKeyBuf[7], abyKeyBuf[8], abyKeyBuf[9], abyKeyBuf[10], abyKeyBuf[11], 
                    abyKeyBuf[12], abyKeyBuf[13], abyKeyBuf[14], abyKeyBuf[15] );
    */
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

    T8KEMixerStart tMixerStart = *(T8KEMixerStart*)cServMsg.GetMsgBody();	
    TDoublePayload tDoublePayload = *(TDoublePayload*)(cServMsg.GetMsgBody() + sizeof(T8KEMixerStart));

    u8 byDepth = tMixerStart.GetMixDepth();

    TMediaEncrypt tMediaEncrypt = tMixerStart.GetAudioEncrypt();

    m_bNeedPrs = tMixerStart.IsNeedByPrs();

	tMixerStart.Print();
	tDoublePayload.Print();
#ifdef _8KH_ 
	u8 byAudioTrackNum = *(u8*)(cServMsg.GetMsgBody() + sizeof(T8KEMixerStart) + sizeof(TDoublePayload) + sizeof(TCapSupportEx));
#endif

#ifdef _8KI_
	TAudioCapInfo tAudioCapInfo[3];
	u8 byNChnlNum = *(u8*)(cServMsg.GetMsgBody() + sizeof(T8KEMixerStart) + sizeof(TDoublePayload) + sizeof(TCapSupportEx));
	u8 *lpMsgBody = cServMsg.GetMsgBody() + sizeof(T8KEMixerStart) + sizeof(TDoublePayload) + sizeof(TCapSupportEx) + sizeof(u8);
	if ( byNChnlNum > 0 && byNChnlNum <= 3 )
	{
		for (u8 byIdx = 0; byIdx < byNChnlNum; byIdx++)
		{
			//N路编码通道
			tAudioCapInfo[byIdx] = *(TAudioCapInfo *)lpMsgBody;
			lpMsgBody += sizeof(TAudioCapInfo);			
		}
	}
	else
	{
		cServMsg.SetErrorCode(ERR_MIXER_CALLDEVICE);
		SendMsgToMcu(MIXER_MCU_STARTMIX_NACK, cServMsg);
	}
#endif

    switch(CurState())
    {
    //该混音组未被使用
    case TMixerGrpStatus::READY:
        {
#ifdef _8KI_
			if (!StartMix(&tMediaEncrypt, &tDoublePayload,byNChnlNum,&tAudioCapInfo[0]))
#else
			if (!StartMix(&tMediaEncrypt, &tDoublePayload))
#endif
			{
				cServMsg.SetErrorCode(ERR_MIXER_CALLDEVICE);
				SendMsgToMcu(MIXER_MCU_STARTMIX_NACK, cServMsg);
                return;
			}
#ifdef _8KH_ 
			if (!m_pcAMixer->SetAACChannel(byAudioTrackNum))
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_MIXER,"[StartMix][Mixer:%d]SetAACChannel(%d) failed.\n", GetInsID(),byAudioTrackNum);
			}
			else
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,"[StartMix][Mixer:%d]SetAACChannel(%d) successed.\n", GetInsID(),byAudioTrackNum);
			}
#endif
			m_cConfId =	cServMsg.GetConfId();

			cServMsg.SetErrorCode(0);
            cServMsg.SetMsgBody(&byDepth, sizeof(u8));
			SendMsgToMcu(MIXER_MCU_STARTMIX_ACK, cServMsg);			
			SendGrpNotif();
        }
        break;

    case TMixerGrpStatus::MIXING: //当前已开始混音,应拒绝
        cServMsg.SetErrorCode(ERR_MIXER_MIXING);
        SendMsgToMcu(MIXER_MCU_STARTMIX_NACK, cServMsg);
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,"Grp.%d at MIXING state recv start mix cmd.\n", GetInsID());
        break;
    case TMixerGrpStatus::IDLE:	  //该混组尚未创建
        cServMsg.SetErrorCode(ERR_MIXER_MIXING);
        SendMsgToMcu(MIXER_MCU_STARTMIX_NACK, cServMsg);
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,"Grp.%d at IDLE state recv start mix cmd.\n", GetInsID());
        break;
    default:
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,"Grp.%d at undefine state (%d).\n", GetInsID(), CurState());
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

    switch(CurState())
    {
    // 暂停或停止状态
    case TMixerGrpStatus::MIXING:
    // 该组并未混音，同步状态
    case TMixerGrpStatus::READY:    
        {		
			if ( StopMix() )
			{				
				SendMsgToMcu(MIXER_MCU_STOPMIX_ACK, cServMsg);				
			}
			else
			{
				cServMsg.SetErrorCode(ERR_MIXER_CALLDEVICE);
				SendMsgToMcu(MIXER_MCU_STOPMIX_NACK, cServMsg);
			}

            m_tGrpStatus.m_byGrpState    = TMixerGrpStatus::READY ;
            m_tGrpStatus.m_byGrpMixDepth = 0;
            m_cConfId.SetNull();
            NEXTSTATE(TMixerGrpStatus::READY);//进入停止状态
			LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,"[MsgStopMixProc] Grp.%d change to status:(%d).\n", GetInsID()-1, CurState());
            SendGrpNotif();
        }
        break;
    case TMixerGrpStatus::IDLE:
    default:
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,"Grp.%d at undefine state (%d).\n", GetInsID()-1, CurState());
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
void CAudMixInst::MsgSetVolumeProc()
{
    /*
    CServMsg cServMsg(pMsg->content, pMsg->length);

    u8 *pbyMsgBody = (u8*)cServMsg.GetMsgBody();
    u8 byGrpId   = *pbyMsgBody;
    TMixMember* ptMixMmb  = (TMixMember*)(pbyMsgBody + sizeof(u8));

    u8 byMtId = ptMixMmb->m_tMember.GetMtId();
    u8 byChnlIdx = cServMsg.GetChnIndex();
	if( 0 == byChnlIdx )
	{
		LogPrint(TRUE, FALSE, "[AMix] Error Mt id= %d.\n", ptMixMmb->m_tMember.GetMtId() );
		return;
	}
    LogPrint(TRUE, FALSE, "[MsgSetVolumeProc] mt id= %d. idle channel= %d.\n", byMtId, byChnlIdx );

    u8 byVol  = ptMixMmb->m_byVolume;
//    u16 wRet = m_pcAMixer->SetMixerChannelVolume( byChnlIdx, byVol );
//     if (TRUE != wRet)
//     {
//         LogPrint(TRUE, FALSE, "[AMix] Grp.%d set mt.%d volume(%d) failed! (%d).\n", byGrpId, byChnlIdx, byVol, wRet);
//         return;
//     }

    cServMsg.SetMsgBody((u8*)ptMixMmb, sizeof(TMixMember));
    SendMsgToMcu(MIXER_MCU_CHNNLVOL_NOTIF, cServMsg);
	return;
    */
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
    
	LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,"[Mixer:%d][ForceActive] mt id= %d at channel= %d.\n",GetInsID(), byMtId, byChnlIdx );

    BOOL32 bRet = m_pcAMixer->SetForceAudMix( byChnlIdx-1 );
    if ( !bRet )
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
    BOOL32 bRet = m_pcAMixer->CancelForceAudMix();
    if (!bRet)
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
    CServMsg cMixDelayMsg(pMsg->content, pMsg->length);
    u8* pbyMsgBody = cMixDelayMsg.GetMsgBody();
    u8 byGrpId = *pbyMsgBody;
    u16 wTimeDelay = *(u16*)(pbyMsgBody+sizeof(u8));  // 毫秒
    
    LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,"[Mixer:%d]Proc MsgChangeMixDelay. Time: %d\n",byGrpId, wTimeDelay);
    // 设置混音延时
    BOOL32 bRet = m_pcAMixer->SetMixerDelay((u32)wTimeDelay);
    if (!bRet)
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,"Set MixerDelay Error\n");
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER,"Set MixerDelay Error\n");
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
    2009/03/26  8000E 1.0   顾振华        修改消息体
====================================================================*/
void CAudMixInst::MsgActiveMmbChangeProc(CMessage * const pMsg)
{
    CServMsg cServMsg(pMsg->content, pMsg->length);

    u8 byActChnnl = *((u8*)cServMsg.GetMsgBody() + MAXNUM_MIXER_DEPTH * sizeof(u8));
    u8 byActMtId = 0;
    if (byActChnnl > 0 && byActChnnl <= m_tMixerCfg.GetMaxChnnlInGrp())
    {
#ifdef _8KI_
		byActChnnl -= 1;
#endif
        byActMtId = m_abyMtId2ChnIdx[byActChnnl];
    }

    cServMsg.SetConfId(m_cConfId);
	u8  abyMtMmb[MAXNUM_MIXER_DEPTH];
	memset(abyMtMmb, 0, MAXNUM_MIXER_DEPTH);
	cServMsg.SetMsgBody(abyMtMmb, MAXNUM_MIXER_DEPTH);
    cServMsg.CatMsgBody(&byActMtId, sizeof(u8));
    LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,"[Mixer:%d][MsgActiveMmbChangeProc] Excite member channel.%d, mtid.%d\n",GetInsID(), byActChnnl, byActMtId);
    SendMsgToMcu(MIXER_MCU_ACTIVEMMBCHANGE_NOTIF, cServMsg);

    SendGrpNotif();

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
#ifndef _8KI_
    if ( byMtId == 0 || byChanIdx == 0 )
    {
        LogPrint(LOG_LVL_WARNING, MID_MCU_MIXER, "[SetMtId2ChnIdx] param err: byMtId.%d, byChanIdx.%d \n", byMtId, byChanIdx );
        return;
    }
#endif
    
    if ( !bRemove )
    {
        BOOL32 bExist = FALSE;
        
        u8 byPos = 0;
#ifndef _8KI_
        for( byPos = 1; byPos < MAXNUM_MIXER_CHNNL+1; byPos++ )
#else
		for( byPos = 0; byPos < MAXNUM_MIXER_CHNNL; byPos++ )
#endif
        {
            if ( byMtId == m_abyMtId2ChnIdx[byPos] && byPos == byChanIdx )
            {
                bExist = TRUE;
                LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER, "[SetMtId2ChnIdx] Mt.%d already exist at byPos.%d !\n", byMtId, byPos );
                break;
            }
        }
        if ( !bExist )
        {
            m_abyMtId2ChnIdx[byChanIdx] = byMtId;
            LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER, "[SetMtId2ChnIdx] Mt.%d set succeed, byPos.%d !\n", byMtId, byChanIdx );
        }
    }
    else
    {
        BOOL32 bExist = FALSE;
#ifndef _8KI_
        u8 byPos = 1;
        for( ; byPos < MAXNUM_MIXER_CHNNL+1; byPos++ )
#else
		u8 byPos = 0;
        for( ; byPos < MAXNUM_MIXER_CHNNL; byPos++ )
#endif
        {
            if ( byMtId == m_abyMtId2ChnIdx[byPos] && byPos == byChanIdx )
            {
                m_abyMtId2ChnIdx[byPos] = 0;
                bExist = TRUE;
                LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER, "[SetMtId2ChnIdx] Mt.%d remve m_abyMixMtId succeed, byPos.%d !\n", byMtId, byPos );
                break;
            }
        }
        if ( !bExist )
        {
            LogPrint( TRUE, FALSE, "[SetMtId2ChnIdx] Mt.%d remve m_abyMixMtId unexist, ignore it !\n", byMtId );
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
  函 数 名： DaemonProcMixerStatus
  功    能： 显示所有混音状态
  算法实现： 
  全局变量： 
  参    数： CApp* pcApp
  返 回 值： void 
=============================================================================*/
void CAudMixInst::DaemonProcMixerStatus()
{
	LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "\n----------------DAEMON:MIXERNUM(%d)  McuNode(%d)----------------\n",m_tMixerCfg.GetMaxMixGrpCount(),m_dwMcuNode);
	for(u8 byMixerIdx = 0 ; byMixerIdx < m_tMixerCfg.GetMaxMixGrpCount();byMixerIdx++)
	{
		post(MAKEIID(GetAppID(), byMixerIdx + 1), EV_MIXER_SHOWGROUP);
	}
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
	if (m_tGrpStatus.m_byGrpState == TMixerGrpStatus::IDLE)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "\n----------------    Mixer   (%d) STATUS:IDLE----------------\n", GetInsID());
		return;
	}
    u8 byIdx;
	static char status[3][7] = {"  IDLE", " READY", "MIXING"};
    LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "\n----------------    Mixer(EQPID:%d Alias:%s)   (InstId:%d) (McuNode:%d)----------------\n",
		m_tMixerCfg.GetEqpId(),m_tMixerCfg.GetAlias(), GetInsID(),m_dwMcuNode);
    LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "Status : %s   MaxMixMem : %d LocalRcvStartPort: %d Nmode(Ip:%s,Port:%d)\n\n", 
		status[m_tGrpStatus.m_byGrpState],m_tMixerCfg.GetMaxChnnlInGrp(),
		m_tMixerCfg.GetEqpStartPort(),strofip(m_tMixerCfg.GetMcuSwitchIp()),m_tMixerCfg.GetMcuStartPort());
    TMixerGroupStatus tMixGrpStatus;
    TMixerChannelStatis tMixerChannelStatis;

    memset(&tMixGrpStatus, 0, sizeof(TMixerGroupStatus));
    m_pcAMixer->GetMixStatus(&tMixGrpStatus);
	LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER,"混音是否开始(1:开始 0:停止)  混音组的通道数  N模式的输出地址\n");
	
	LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "     %d                            %d ",
							tMixGrpStatus.m_bMixerStart,
                            tMixGrpStatus.m_byCurChannelNum );
#if !defined(_8KH_) && !defined(_8KE_)
	for( byIdx = 0;byIdx<AUDIO_NMODE_MAX_CHNL;++byIdx )
	{		
		        	LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "                                      0x%-8x:%-5d\n\n",                            
                            ntohl(tMixGrpStatus.m_tNModeDstAddr[byIdx].dwIp),    //N模式的输出IP地址
							tMixGrpStatus.m_tNModeDstAddr[byIdx].wPort);         //N模式的输出端口
	}
#else
					LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "                                      0x%-8x:%-5d\n\n",                            
                            ntohl(tMixGrpStatus.m_tNModeDstAddr.dwIp),    //N模式的输出IP地址
							tMixGrpStatus.m_tNModeDstAddr.wPort);         //N模式的输出端口
#endif
    

    LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "                                                 _______接收________      _______发送_______\n");
    LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "                                                 |                 |      |                |\n");
    LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "通道号   MtId      源地址          目的地址      比特率  收包数   丢包数  比特率  发包数   丢包数\n");
	for (byIdx=0; byIdx < MAXNUM_MIXER_CHNNL; byIdx++)
    {
#if !defined(_8KH_) && !defined(_8KE_)
		if (m_abyMtId2ChnIdx[byIdx] !=0)
#else
		if (m_abyMtId2ChnIdx[byIdx + 1] !=0)
#endif
		{
			memset(&tMixerChannelStatis, 0, sizeof(TMixerChannelStatis));
			m_pcAMixer->GetMixStatis( byIdx , &tMixerChannelStatis);
			
			if (tMixGrpStatus.m_atChannel[byIdx].m_tSrcNetAddr.dwIp != 0 ||
				tMixGrpStatus.m_atChannel[byIdx].m_tDstNetAddr.dwIp != 0 ||
				tMixerChannelStatis.m_dwRecvPackNum != 0 ||
				tMixerChannelStatis.m_dwSendPackNum != 0)
			{
				LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "  %d    %d    0x%-8x:%-5d 0x%-8x:%-5d  %4d  %8d %8d %4d  %8d %8d\n",
					byIdx,
#if !defined(_8KH_) && !defined(_8KE_)
					m_abyMtId2ChnIdx[byIdx],
#else
					m_abyMtId2ChnIdx[byIdx+1],
#endif
					tMixGrpStatus.m_atChannel[byIdx].m_tSrcNetAddr.dwIp,
					tMixGrpStatus.m_atChannel[byIdx].m_tSrcNetAddr.wPort,                                    
					tMixGrpStatus.m_atChannel[byIdx].m_tDstNetAddr.dwIp,
					tMixGrpStatus.m_atChannel[byIdx].m_tDstNetAddr.wPort,
					tMixerChannelStatis.m_dwRecvBitRate,
					tMixerChannelStatis.m_dwRecvPackNum,
					tMixerChannelStatis.m_dwRecvLosePackNum,
					tMixerChannelStatis.m_dwSendBitRate,
					tMixerChannelStatis.m_dwSendPackNum,
					tMixerChannelStatis.m_dwMixerDiscardPackNum);
			}
		}
    }

    u8 abyActiveMmb[MAXNUM_MIXER_DEPTH];
    u8 byMmbNum = m_tGrpStatus.GetActiveMmb(abyActiveMmb, MAXNUM_MIXER_DEPTH);
	if (byMmbNum > 0)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "\nActive Member:");
	} 
    for(byIdx=0; byIdx < byMmbNum; byIdx++)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER," %3d", abyActiveMmb[byIdx]);
        if (((byIdx+1)%10)==0)
        {
            LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "\n");
        }
    }
    LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER,"\n");
}
/*====================================================================
函数名      : DaemonProcConnetTimerOut
功能        ：建链定时器超时处理
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/04/25  1.0         田志勇          创建
====================================================================*/
void CAudMixInst::DaemonProcConnetTimerOut()
{
	if(ConnectMcu( m_dwMcuNode))
	{ 
		SetTimer(EV_MIXER_REGISTER, 10); 
	}
	else
	{
		SetTimer(EV_MIXER_CONNECT, MIX_CONNETC_TIMEOUT);    //定时建链
	}
}
/*====================================================================
函数名      : DaemonProcRegTimerOut
功能        ：注册定时器超时处理
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/04/25  1.0         田志勇          创建
====================================================================*/
void CAudMixInst::DaemonProcRegTimerOut()
{
	CServMsg cSvrMsg;
	TEqpRegReq tReg;
	tReg.SetEqpType( EQP_TYPE_MIXER );
	tReg.SetVersion( DEVVER_MIXER );
	cSvrMsg.SetMsgBody((u8*)&tReg, sizeof(tReg));
	post( MAKEIID(AID_MCU_PERIEQPSSN, CInstance::DAEMON), MIXER_MCU_REG_REQ,
		cSvrMsg.GetServMsg(), cSvrMsg.GetServMsgLen(), m_dwMcuNode );
	SetTimer(EV_MIXER_REGISTER,MIX_REGISTER_TIMEOUT);
}
/*====================================================================
函数名      : DaemonProcRegAck
功能        ：注册成功消息处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage * const pcMsg
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/04/25  1.0         田志勇          创建
====================================================================*/
void CAudMixInst::DaemonProcRegAck(CMessage * const pcMsg, CApp* pcApp)
{
	KillTimer(EV_MIXER_REGISTER);
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	char achAlias [ MAXLEN_ALIAS + 1 ] = {0};
	u8 byMixerNum = cServMsg.GetMsgBodyLen() / (sizeof(u8) + sizeof(u8) + sizeof(u16) + sizeof(achAlias));
	m_tMixerCfg.SetMaxMixGrpCount(byMixerNum);
	u8 *pMsg = (u8 *)cServMsg.GetMsgBody();
	CServMsg cMixerCfgToEqp;
	u8 byMixerId = 0;
	u8 byMixMemNum = 0;
	u16 wEqpRcvStartPort = 0;
	u32 dwLocalIp = m_tMixerCfg.GetEqpIp();
	for(u8 byMixerIdx = 0 ;byMixerIdx < byMixerNum;byMixerIdx++)
	{
		byMixerId = *(u8*)pMsg;
		pMsg += sizeof(u8);
		byMixMemNum = *(u8*)pMsg;
		pMsg += sizeof(u8);
		wEqpRcvStartPort = *(u16*)pMsg;
		pMsg += sizeof(u16);
		strcpy(achAlias,(char *)pMsg);
		pMsg += sizeof(achAlias);
		cMixerCfgToEqp.SetMsgBody( (u8*)&m_dwMcuNode, sizeof(u32) );
		cMixerCfgToEqp.CatMsgBody( (u8*)&dwLocalIp, sizeof(u32) );
		cMixerCfgToEqp.CatMsgBody( (u8*)&byMixerId, sizeof(u8) );
		cMixerCfgToEqp.CatMsgBody( (u8*)&byMixMemNum, sizeof(u8) );
		cMixerCfgToEqp.CatMsgBody( (u8*)&wEqpRcvStartPort, sizeof(u16) );
		cMixerCfgToEqp.CatMsgBody( (u8*)&achAlias[0], sizeof(achAlias) );
		post(MAKEIID(GetAppID(), byMixerIdx + 1), EV_MIXER_INIT,cMixerCfgToEqp.GetServMsg(),cMixerCfgToEqp.GetServMsgLen());
		LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MIXER,"[DaemonProcRegAck]m_dwMcuNode(%d) LocalIp(%x) MixerId(%d) MixMemNum(%d) RcvStartPort(%d) Aliax(%s) AppId(%d)!\n", 
			m_dwMcuNode,dwLocalIp,byMixerId,byMixMemNum,wEqpRcvStartPort,achAlias,pcApp != NULL ? pcApp->appId : 0);
	}
}
/*====================================================================
函数名      : DaemonProcRegNack
功能        ：注册失败消息处理
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/04/25  1.0         田志勇          创建
====================================================================*/
void CAudMixInst::DaemonProcRegNack()
{
	SetTimer(EV_MIXER_REGISTER,MIX_REGISTER_TIMEOUT);
}
/*====================================================================
函数名      : DaemonProcOspDisconnect
功能        ：断链消息处理
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/04/25  1.0         田志勇          创建
====================================================================*/
void CAudMixInst::DaemonProcOspDisconnect()
{
	m_dwMcuNode = INVALID_NODE;
	SetTimer(EV_MIXER_CONNECT,MIX_CONNETC_TIMEOUT);
	for(u8 byMixerIdx = 0 ; byMixerIdx < m_tMixerCfg.GetMaxMixGrpCount();byMixerIdx++)
	{
		post(MAKEIID(GetAppID(), byMixerIdx + 1), OSP_DISCONNECT);
	}
}

