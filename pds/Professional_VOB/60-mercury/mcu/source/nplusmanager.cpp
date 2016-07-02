/*****************************************************************************
   模块名      : N+1备份管理模块
   文件名      : nplusmanager.cpp
   相关文件    : nplusmanager.h
   文件实现功能: N+1备份管理
   作者        : 许世林
   版本        : V4.0  Copyright(C) 2006-2009 KDCOM, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2006/11/21  4.0         许世林      创建
******************************************************************************/

#include "evmcu.h"
#include "evmcumcs.h"
#include "mcuvc.h"
#include "nplusmanager.h"
#include "mcsssn.h"
//#include "mcuerrcode.h"
#include "evmcumt.h"
#include "mtadpssn.h"

CNPlusApp g_cNPlusApp;

//CNPlusInst
//construct
CNPlusInst::CNPlusInst( void ):m_dwMcuNode(INVALID_NODE), 
                               m_dwMcuIId(0), 
                               m_dwMcuIpAddr(0),
                               m_dwMcuNodeB(INVALID_NODE), 
                               m_dwMcuIIdB(0), 
                               m_dwMcuIpAddrB(0),
                               m_byUsrNum(0),
                               m_wRtdFailTimes(0),
                               m_wRegNackByCapTimes(0)
{
    memset(&m_cUsrGrpsInfo, 0, sizeof(m_cUsrGrpsInfo));
	memset(&m_atConfExData[0], 0,sizeof(m_atConfExData));
	for( u8 byIdx = 0; byIdx <sizeof(m_atNplusParam)/sizeof(m_atNplusParam[0]);byIdx++ )
	{
		m_atNplusParam[byIdx].Clear();
	}
}

//destruct
CNPlusInst::~CNPlusInst( void )
{
}

/*=============================================================================
函 数 名： InstanceEntry
功    能： 备份Server模式下每个实例对应一个mcu
算法实现： 
全局变量： 
参    数： CMessage * const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/21  4.0			许世林                  创建
=============================================================================*/
void CNPlusInst::InstanceEntry( CMessage * const pcMsg )
{
    if ( NULL == pcMsg )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::InstanceEntry] invalid msg body!\n" );
        return;
    }

    switch( pcMsg->event )
    {  
    case OSP_DISCONNECT:
        ProcDisconnect(pcMsg);
        break;    

    case MCU_NPLUS_REG_REQ:
        ProcRegNPlusMcuReq( pcMsg );
        break;

        //保存主mcu发送过来的会议数据
    case MCU_NPLUS_CONFDATAUPDATE_REQ:
    case MCU_NPLUS_CONFINFOUPDATE_REQ:
    case MCU_NPLUS_USRGRPUPDATE_REQ:
    case MCU_NPLUS_USRINFOUPDATE_REQ:
    case MCU_NPLUS_CONFMTUPDATE_REQ:
	case MCU_NPLUS_SMCUINFOUPDATE_REQ:
    case MCU_NPLUS_CHAIRUPDATE_REQ:
    case MCU_NPLUS_SPEAKERUPDATE_REQ:
    case MCU_NPLUS_VMPUPDATE_REQ:
    case MCU_NPLUS_RASINFOUPDATE_REQ:
	case MCU_NPLUS_AUTOMIXUPDATE_REQ:
        ProcMcuDataUpdateReq( pcMsg );
        break;

    case MCU_NPLUS_EQPCAP_NOTIF:
        ProcMcuEqpCapNotif( pcMsg );
        break;

    case MCU_NPLUS_RTD_RSP:
        ProcRtdRsp();
        break;

    case MCU_NPLUS_RTD_TIMER:
        ProcRtdTimeOut();
        break;

        //备份mcu发送到主mcu会议回滚消息
    case VC_NPLUS_MSG_NOTIF:
        ProcConfRollback( pcMsg );
        break;

    case VC_NPLUS_RESET_NOTIF:
        ProcReset( );
        break;

    case MCU_NPLUS_CONFROLLBACK_ACK:
    case MCU_NPLUS_CONFROLLBACK_NACK:
    case MCU_NPLUS_USRROLLBACK_ACK:
    case MCU_NPLUS_USRROLLBACK_NACK:
    case MCU_NPLUS_GRPROLLBACK_ACK:
    case MCU_NPLUS_GRPROLLBACK_NACK:
        break;

    default:
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::InstanceEntry] invalid event(%d): %s!\n", 
            pcMsg->event, OspEventDesc(pcMsg->event));
        break;
    }

    return;
}

/*=============================================================================
函 数 名： InstanceDump
功    能： print
算法实现： 
全局变量： 
参    数： u32 dwParam = 0
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/24  4.0			许世林                  创建
=============================================================================*/
void CNPlusInst::InstanceDump( u32 dwParam )
{
    u8 byIdx = 0;
    u8 byLoop = 0;
    StaticLog("=================NPlusInst: InsID %d================\n", GetInsID());
    StaticLog("McuNode:0x%x McuIId:0x%x McuIp:0x%x dwParam:0x%x\n", m_dwMcuNode, 
              m_dwMcuIId, m_dwMcuIpAddr,dwParam);
    StaticLog("McuNodeB:0x%x McuIIdB:0x%x McuIpB:0x%x dwParam:0x%x\n", m_dwMcuNodeB, 
              m_dwMcuIIdB, m_dwMcuIpAddrB,dwParam);
    
    for (byIdx = 0; byIdx < MAXNUM_ONGO_CONF; byIdx++)
    {
        if (m_atConfData[byIdx].IsNull())
        {
            continue;
        }
        m_atConfData[byIdx].m_tConf.Print();
		m_atConfData[byIdx].m_tConf.GetConfAttrb().Print();
		m_atConfData[byIdx].m_tConf.m_tStatus.Print();
		m_atConfData[byIdx].m_tConf.m_tStatus.GetConfMode().Print();

        StaticLog("\nMtInfo in conf:\n");
        for (byLoop = 0; byLoop < m_atConfData[byIdx].m_byMtNum; byLoop++)
        {   
            TMtInfo *ptInfo = &m_atConfData[byIdx].m_atMtInConf[byLoop];            
            StaticLog("%d: 0x%x(%d)  %d(kbs)\n", byLoop,
                      ptInfo->GetMtAddr().GetIpAddr(),
                      ptInfo->GetMtAddr().GetPort(), ptInfo->GetCallBitrate());
        }

		StaticLog("\nSmcuInfo in conf:\n");
        for (byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++)
        {   
            TSmcuCallnfo tScmuInfo = m_atConfExData[byIdx].m_atSmcuCallInfo[byLoop];
            if(tScmuInfo.m_dwMtAddr.IsNull())
			{
				break;
			}
			else
			{
				StaticLog("%d: 0x%x(%d) E164:%s\n", byLoop,
				tScmuInfo.m_dwMtAddr.GetIpAddr(),
				tScmuInfo.m_dwMtAddr.GetPort(),tScmuInfo.m_achAlias);
			}
        }

        StaticLog("single VmpInfo in conf:\n");
        for (byLoop = 0; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++)
        {

			if(m_atConfData[byIdx].m_tVmpInfo.m_abyMemberType[byLoop]!= VMP_MEMBERTYPE_NULL
				&& m_atConfData[byIdx].m_tVmpInfo.m_abyMemberType[byLoop]!= VMP_MEMBERTYPE_MCSSPEC )
			{
				StaticLog("%d: Membertype(%d)\n", byLoop,
                      m_atConfData[byIdx].m_tVmpInfo.m_abyMemberType[byLoop]);

				continue;
			}

            TMtInfo *ptInfo = &m_atConfData[byIdx].m_tVmpInfo.m_atMtInVmp[byLoop];
            if (ptInfo->IsNull())
            {
                continue;
            }
            StaticLog("%d: 0x%x(%d)  %d(kbs) type(%d)\n", byLoop,
                      ptInfo->GetMtAddr().GetIpAddr(),
                      ptInfo->GetMtAddr().GetPort(), ptInfo->GetCallBitrate(), 
                      m_atConfData[byIdx].m_tVmpInfo.m_abyMemberType[byLoop]);
        }
    }

	for(byIdx = 0; byIdx < MAXNUM_PERIEQP; byIdx ++ )
	{
		if(!m_atNplusParam[byIdx].IsNull())
		{
			StaticLog("===multi--VmpInfo in conf[%d]=============\n",byIdx);
			m_atNplusParam[byIdx].m_tVmpBaiscParam.Print();

			StaticLog("confIdx:%d\n",m_atNplusParam[byIdx].m_byConfIdx);
			for (byLoop = 0; byLoop < MAXNUM_VMP_MEMBER; byLoop++)
			{
				if(m_atNplusParam[byIdx].m_tVmpMemer.m_tVmpChnnlInfo[byLoop].m_byMemberType!= VMP_MEMBERTYPE_NULL
					&& m_atNplusParam[byIdx].m_tVmpMemer.m_tVmpChnnlInfo[byLoop].m_byMemberType!= VMP_MEMBERTYPE_MCSSPEC )
				{
					StaticLog("%d: Membertype(%d)\n", byLoop,
						m_atNplusParam[byIdx].m_tVmpMemer.m_tVmpChnnlInfo[byLoop].m_byMemberType);
					
					continue;
				}

				TMtInfo *ptInfo = &m_atNplusParam[byIdx].m_tVmpMemer.m_tVmpChnnlInfo[byLoop].m_tMtInVmp;
				if (ptInfo->IsNull())
				{
					continue;
				}
				StaticLog("%d: 0x%x(%d)  %d(kbs) type(%d)\n", byLoop,
					ptInfo->GetMtAddr().GetIpAddr(),
					ptInfo->GetMtAddr().GetPort(), ptInfo->GetCallBitrate(), 
					m_atNplusParam[byIdx].m_tVmpMemer.m_tVmpChnnlInfo[byLoop].m_byMemberType);
			}
		}
	}

    m_cUsrGrpsInfo.Print();

    StaticLog("user info:\n");
    for(byIdx = 0; byIdx < m_byUsrNum; byIdx++)
    {
        m_acUsrInfo[byIdx].Print();
    }
    
    return;
}

/*=============================================================================
函 数 名： ClearInst
功    能： 清空实例
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/30  4.0			许世林                  创建
=============================================================================*/
void CNPlusInst::ClearInst( void )
{
    m_dwMcuNode = INVALID_NODE;
    m_dwMcuIId = 0;
    m_dwMcuIpAddr = 0;
    m_dwMcuNodeB = INVALID_NODE;
    m_dwMcuIIdB = 0;
    m_dwMcuIpAddrB = 0;
    m_byUsrNum = 0;
    m_wRtdFailTimes = 0;
    m_wRegNackByCapTimes = 0;
    memset(m_atConfData, 0, sizeof(m_atConfData));
    memset(&m_cUsrGrpsInfo, 0, sizeof(m_cUsrGrpsInfo));
    memset(m_acUsrInfo, 0, sizeof(m_acUsrInfo));
    memset(&m_tRASInfo, 0, sizeof(m_tRASInfo));
    memset(m_atChargeInfo, 0, sizeof(m_atChargeInfo));
	memset(m_atConfExData, 0, sizeof(m_atConfExData));

	for( u8 byIdx = 0; byIdx < sizeof(m_atNplusParam)/sizeof(m_atNplusParam[0]); byIdx++ )
	{
		m_atNplusParam[byIdx].Clear();
	}

    NEXTSTATE(STATE_IDLE);
    return;
}

/*=============================================================================
函 数 名： ProcRegNPlusMcuReq
功    能： 主mcu注册处理函数
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/22  4.0			许世林                  创建
2013/03/18              liaokang              支持主备N+1备双备份
=============================================================================*/
void CNPlusInst::ProcRegNPlusMcuReq( const CMessage * pcMsg )
{
    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcRegNPlusMcuReq]Enter inst.%d's ProcRegNPlusMcuReq!\n",GetInsID());
    
    CServMsg cServMsg(pcMsg->content, pcMsg->length);   
    TNPlusMcuRegInfo *ptRegInfo = (TNPlusMcuRegInfo *)cServMsg.GetMsgBody();
    TNPlusEqpCapEx tActualCapEx;
    TNPlusMcuInfoEx tRemoteMcuInfoEx;
    tRemoteMcuInfoEx.SetMcuEncodingForm(emenCoding_GBK); //默认对端为GBK编码
    if(cServMsg.GetMsgBodyLen() >sizeof(TNPlusMcuRegInfo) + sizeof(u32) + sizeof(u32))
    {
        u16 wUnpackLen = 0;
        BOOL32 bUnkonwInfo = FALSE;
        g_cNPlusApp.UnPackNplusExInfo(tActualCapEx,cServMsg.GetMsgBody()+sizeof(TNPlusMcuRegInfo),wUnpackLen,bUnkonwInfo,&tRemoteMcuInfoEx);
    }
    
    u32 dwMcuIpAddr = ptRegInfo->GetMcuIpAddr();
    u32 dwMcuIId = *(u32 *)(cServMsg.GetMsgBody() + cServMsg.GetMsgBodyLen() - sizeof(u32) - sizeof(u32));
    u32 dwMcuNode = *(u32 *)(cServMsg.GetMsgBody() + cServMsg.GetMsgBodyLen() - sizeof(u32));
    LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[ProcRegNPlusMcuReq] McuIpAddr(0x%x) McuIId(%x) McuNode(%d)\n", dwMcuIpAddr, dwMcuIId, dwMcuNode);

    if ( CurState() == STATE_NORMAL )
    {
        // 防止反复注册
        if( dwMcuNode == m_dwMcuNode || dwMcuNode == m_dwMcuNodeB )
        {
            LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcRegNPlusMcuReq] invalid ins state(%d)(id.%d)\n", CurState(), GetInsID());
            return;
        }    
    }

    BOOL32 bRollBack = FALSE;
    //检查是否为故障恢复mcu 和 本备份mcu是否支持故障恢复
    if (CurState() == STATE_SWITCHED && g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_SWITCH)
    {
        TLocalInfo tLocalInfo;
        if (SUCCESS_AGENT == g_cMcuAgent.GetLocalInfo(&tLocalInfo) && 
            tLocalInfo.GetIsNPlusRollBack() &&
            (m_dwMcuIpAddr == ptRegInfo->GetMcuIpAddr() || m_dwMcuIpAddrB == ptRegInfo->GetMcuIpAddr()))
        {        
            //检查会议恢复所需外设能力，若不满足则拒绝，等待外设能力满足了再进行模式切换
            TNPlusEqpCap tNeedCap,tActualCap;
			TNPlusEqpCapEx tNeedCapEx/*,tActualCapEx*/;
			TBasChnAbility atBasChnAbility[NPLUS_MAX_BASCHN_NUM];
			u8 byBasChnNum = 0;

            for (u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF; byLoop++)
            {
                if (!m_atConfData[byLoop].IsNull())
                {
					TBasChnAbility atTempBasChn[NPLUS_MAX_BASCHN_NUM];
					TNPlusEqpCap tTmpCap;
					TNPlusEqpCapEx tTmpCapEx;

					u8 byConfBasChnNum = 0;
                    g_cNPlusApp.GetEqpCapFromConf(m_atConfData[byLoop].m_tConf,tTmpCap,tTmpCapEx,&atTempBasChn[0],byConfBasChnNum);
                    tNeedCap = tNeedCap + tTmpCap;
					tNeedCapEx = tNeedCapEx + tTmpCapEx;
					memcpy((u8*)(atBasChnAbility+byBasChnNum),(u8*)atTempBasChn,byConfBasChnNum*sizeof(TBasChnAbility));
					byBasChnNum += byConfBasChnNum;
                }
            }
            
            tActualCap = ptRegInfo->GetMcuEqpCap();

            // guzh  [12/13/2006] 检查外设能力，同时要求该MCU上有Mp和MtAdp
			TBasChnAbility atActualBasChnAbility[NPLUS_MAX_BASCHN_NUM];
			u8 byActualBasChnNum = 0;
			GetAllBasChnAbilityArray(tActualCap,tActualCapEx,atActualBasChnAbility,byActualBasChnNum);
			
			u8 byActualVmpNum = tActualCap.m_byVmpNum+tActualCapEx.m_byMPU2BasicVmpNum+tActualCapEx.m_byMPU2EcardBasicVmpNum+tActualCapEx.m_byMPU2EnhancedVmpNum;
			u8 byNeedVmpNum = tNeedCap.m_byVmpNum+tNeedCapEx.m_byMPU2BasicVmpNum+tNeedCapEx.m_byMPU2EcardBasicVmpNum+tNeedCapEx.m_byMPU2EnhancedVmpNum;

			//回滚 vmp只比个数
			if(tActualCap < tNeedCap ||
				tActualCapEx < tNeedCapEx ||
				byActualVmpNum < byNeedVmpNum||
				!IsBasSupportRollBack(atActualBasChnAbility,byActualBasChnNum) ||
                 !tActualCap.HasMp() || 
                 !tActualCap.HasMtAdp() )
            {
                LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcRegNPlusMcuReq] Reg MCU(0x%x) eqp cap is not enough to rollback conf\n", 
                          ptRegInfo->GetMcuIpAddr());

                // guzh [12/13/2006]
                // 如果还没有到达拒绝回滚上限
                // 则断开连接，退出。否则强制回滚，防止备份服务器劫持
                m_wRegNackByCapTimes ++;
                // guzh [12/13/2006] 考虑到可能对端会结束会议后无法开始，暂时忽略
                //if ( m_wRegNackByCapTimes  < NPLUS_MAXNUM_REGNACKBYREMOTECAP)
                LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcRegNPlusMcuReq] Reject m_wRegNackByCapTimes = %d\n", m_wRegNackByCapTimes);
                OspDisconnectTcpNode(pcMsg->srcnode);
                return;           
            }
            
            //模式切换 rollback
            bRollBack = TRUE; 
            
            //清除原先保存的会议数据
            ClearInst();
        }
        else
        {
            LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS, "[ProcRegNPlusMcuReq] mcu not support N+1 rollback.ignore mcu(0x%x) reg\n", 
                     ptRegInfo->GetMcuIpAddr());
            return;
        }        
    } 
  
    if( ( 0 == m_dwMcuIpAddr && 0 == m_dwMcuIpAddrB ) ||   // Idle的inst
          dwMcuIpAddr == m_dwMcuIpAddr)                    // normal或switch的inst
    {
        m_dwMcuIpAddr = dwMcuIpAddr;
        m_dwMcuIId = dwMcuIId;
        m_dwMcuNode = dwMcuNode;
        m_dwMcuIpAddrB = tRemoteMcuInfoEx.GetAnotherMpcIp();
    }    
    else /*if( dwMcuIpAddr == m_dwMcuIpAddrB )*/          // normal或switch的inst
    {
        m_dwMcuIpAddrB = dwMcuIpAddr;
        m_dwMcuIIdB = dwMcuIId;
        m_dwMcuNodeB = dwMcuNode;
        m_dwMcuIpAddr = tRemoteMcuInfoEx.GetAnotherMpcIp();
    }

    m_emMcuEncodingForm = tRemoteMcuInfoEx.GetMcuEncodingForm();

    //ack
    u8 byRollBack = bRollBack ? 1 : 0;

		u8 abyBuf[5];
    	abyBuf[0] = byRollBack;
	u16 wRtdTime = htons(g_cNPlusApp.GetRtdTime());
	u16 wRtdNum  = htons(g_cNPlusApp.GetRtdNum());
	memcpy( &abyBuf[1],&wRtdTime,sizeof(wRtdTime) );
	memcpy( &abyBuf[3],&wRtdNum,sizeof(wRtdNum) );
	
    PostReplyBack(pcMsg->event+1, 0, &abyBuf[0], sizeof(abyBuf), dwMcuIId, dwMcuNode );
    
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS,"[ProcRegNPlusMcuReq] TIME.%d num%d\n",g_cNPlusApp.GetRtdTime(),
		g_cNPlusApp.GetRtdNum() );
    

    //注册断链接收实例
    OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), GetInsID());

    if( STATE_NORMAL != CurState() )
    {
        SetTimer(MCU_NPLUS_RTD_TIMER, g_cNPlusApp.GetRtdTime()*1000);
    }
    
	//  [1/13/2011 chendaiwei]状态机的切换由函数尾部提到此处是为了保证RTD_TIMER被触发的时候,状态始终处在STATE_NORMAL。
	//  如果处在其他状态，就不应该再进行RTD检测。
	NEXTSTATE(STATE_NORMAL);


	// 恢复自身e164号注册gk, 否则主mcu会因duplicate alias而无法呼上终端[11/8/2012 chendaiwei]
	CServMsg cMsg;
	cServMsg.SetConfIdx( 0 );
	cServMsg.SetEventId( MCU_MT_RESTORE_MCUE164_NTF );
	g_cMtAdpSsnApp.SendMsgToMtAdpSsn( g_cMcuVcApp.GetRegGKDriId(), 
                                          MCU_MT_RESTORE_MCUE164_NTF, cMsg );

	//Delay 5 s确保备恢复本级E164号注册的RRQ成功[11/12/2012 chendaiwei]
	if(bRollBack && 
		g_cMcuAgent.GetGkIpAddr() != 0 && 0 != g_cMcuVcApp.GetRegGKDriId())
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS,"[ProcRegNPlusMcuReq] OspDelay 5 second for restore 1 Mode RRQ suceess\n");
		OspDelay(5000);
	}

    //回滚
    if (bRollBack)
    {
        //恢复用户组和用户信息
        OspPost(MAKEIID(AID_MCU_MCSSN, CInstance::DAEMON), NPLUS_VC_DATAUPDATE_NOTIF, NULL, 0, 0,
                MAKEIID(AID_MCU_NPLUSMANAGER, GetInsID())); 

        //恢复会议所有信息
        OspPost(MAKEIID(AID_MCU_VC, CInstance::EACH), NPLUS_VC_DATAUPDATE_NOTIF, NULL, 0, 0,
                MAKEIID(AID_MCU_NPLUSMANAGER, GetInsID()));            
    
        //状态机翻转
        g_cNPlusApp.SetLocalNPlusState(MCU_NPLUS_SLAVE_IDLE);

		//  [1/13/2011 chendaiwei]在回滚发生后,清空SwitchedMcu的IP地址和InstId，避免再次注册的时候误判
		g_cNPlusApp.SetMcuSwitchedInsId(0,0);

    }
	//xliang [8/27/2009] 主mcu向备mcu注册2种场景
    //1, 主的down后,备mcu接管，之后主mcu再次up。这时备mcu进行rollback, 并恢复自身e164号注册gk
	//2, 主备mcu先后重启(只有测试人员会做这种操作) 
	//   或主mcu挂后，备mcu接管后备mcu也挂掉。之后主/备mcu再次先后起来(真实环境极小概率事件)，这时备mcu也要
	//  恢复自身e164号注册gk, 否则主mcu会因duplicate alias而无法呼上终端。
    //NEXTSTATE(STATE_NORMAL);

    return;
}

/*=============================================================================
函 数 名： ProcMcuDataUpdateReq
功    能： 主mcu向N+1 mcu数据同步处理函数
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/23  4.0			许世林                  创建
=============================================================================*/
void CNPlusInst::ProcMcuDataUpdateReq( const CMessage * pcMsg )
{    
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    CConfId cConfId = cServMsg.GetConfId();
    
    switch (pcMsg->event)
    {
        case MCU_NPLUS_CONFDATAUPDATE_REQ:
            {         
                if (sizeof(TNPlusConfData) > cServMsg.GetMsgBodyLen())
				{
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(confdata)\n", cServMsg.GetMsgBodyLen());
                    return;
				}
				else
				{
					TNPlusConfData *ptConfData = (TNPlusConfData *)cServMsg.GetMsgBody();
					
					if (!SetConfData(ptConfData))
					{
						PostReplyBack(pcMsg->event+2);
						LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set conf data failed.insid.%d\n", GetInsID());
						return;
					}

					u16 wUnpackLen = 0;
					if(ptConfData->m_tConf.HasConfExInfo())
					{
						TConfInfoEx tConfEx;

						BOOL32 bExistUnkonwType = FALSE;
						UnPackConfInfoEx(tConfEx,cServMsg.GetMsgBody()+sizeof(TNPlusConfData),wUnpackLen,bExistUnkonwType);
						
						if(bExistUnkonwType)
						{
							PostReplyBack(pcMsg->event+2);
							LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] unknown conf data.insid.%d\n", GetInsID());

							return;
						}
						else
						{
							if(!SetConfExData(ptConfData->m_tConf.GetConfId(),cServMsg.GetMsgBody()+sizeof(TNPlusConfData),wUnpackLen))
							{
								PostReplyBack(pcMsg->event+2);
								LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] setConfExData failed.insid.%d\n", GetInsID());
								
								return;
							}
						}
					}

					if(cServMsg.GetMsgBodyLen()>sizeof(TNPlusConfData)+wUnpackLen)
					{
						CConfId cTmpConId = ptConfData->m_tConf.GetConfId();
						if(!SetConfSmcuCallInfo(cTmpConId,(TSmcuCallnfo*)(cServMsg.GetMsgBody()+sizeof(TNPlusConfData)+wUnpackLen),(u8)MAXNUM_SUB_MCU))
						{
							PostReplyBack(pcMsg->event+2);
							LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] setConfExData<smcuinfo> failed.insid.%d\n", GetInsID());
							
							return;
						}

						if(cServMsg.GetMsgBodyLen() > sizeof(TNPlusConfData)+wUnpackLen +sizeof(TSmcuCallnfo)*MAXNUM_SUB_MCU)
						{
							u16 wPackVmpLen = 0;
							if(!SetConfMultiVmpParam(cTmpConId,(cServMsg.GetMsgBody()+sizeof(TNPlusConfData)+wUnpackLen+sizeof(TSmcuCallnfo)*MAXNUM_SUB_MCU),wPackVmpLen))
							{
								PostReplyBack(pcMsg->event+2);
								LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] SetConfMultiVmpParam failed.insid.%d\n", GetInsID());
								
								return;
							}
						}
					}


				}
            }            
            break;

        case MCU_NPLUS_CONFINFOUPDATE_REQ:
            {
                if (sizeof(u8) + sizeof(TConfInfo) > cServMsg.GetMsgBodyLen())
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(confinfo)\n", cServMsg.GetMsgBodyLen());
                    return;
                }
                
                u8 byMode = *(u8 *)cServMsg.GetMsgBody();
                TConfInfo *ptConf = (TConfInfo *)(cServMsg.GetMsgBody() + sizeof(u8));
				u8 *ptConfInfoEx = NULL;
				if(cServMsg.GetMsgBodyLen()> sizeof(u8)+sizeof(TConfInfo))
				{
					ptConfInfoEx = cServMsg.GetMsgBody() + sizeof(u8) + sizeof(TConfInfo);
				}
				
				BOOL32 bIsStart = FALSE;
				if( NPLUS_CONF_START == byMode )
				{
					bIsStart = TRUE;
				}

                if (!SetConfInfo(ptConf,  bIsStart))
                {
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set conf info failed.insid.%d\n", GetInsID());
                    return;
                }
			
				u16 wUnpackLen = 0;
				if(ptConf->HasConfExInfo() && bIsStart)
				{
					TConfInfoEx tConfEx;

					BOOL32 bExistUnkonwType = FALSE;
					
					UnPackConfInfoEx(tConfEx,ptConfInfoEx,wUnpackLen,bExistUnkonwType);
					
					if(bExistUnkonwType)
					{
						PostReplyBack(pcMsg->event+2);
						LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] unknown conf info.insid.%d\n", GetInsID());
						
						return;
					}
					else
					{
						if(!SetConfExData(ptConf->GetConfId(),ptConfInfoEx,wUnpackLen))
						{
							PostReplyBack(pcMsg->event+2);
							LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] setConfExData failed.insid.%d\n", GetInsID());
							
							return;
						}
					}
					
					if( ptConfInfoEx != NULL)
					{
						ptConfInfoEx+= wUnpackLen;
					}
				}
            }            
            break;

        case MCU_NPLUS_USRGRPUPDATE_REQ:
            {
                if ((sizeof(CUsrGrpsInfo)) != cServMsg.GetMsgBodyLen())
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(usrgrp)\n", cServMsg.GetMsgBodyLen());
                    return;
                }               

                CUsrGrpsInfo *pcGrpInfo = (CUsrGrpsInfo *)cServMsg.GetMsgBody();
                if (!SetUsrGrpInfo(pcGrpInfo))
                {
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set grp info failed.insid.%d\n", GetInsID());
                    return;
                }
            }            
            break;

        case MCU_NPLUS_USRINFOUPDATE_REQ:
            {
                if (0 != cServMsg.GetMsgBodyLen()%sizeof(CExUsrInfo))
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(usrinfo)\n", cServMsg.GetMsgBodyLen());
                    return;
                }
                            
                u8 byUsrNum = cServMsg.GetMsgBodyLen()/sizeof(CExUsrInfo);
                CExUsrInfo *pcUsrInfo = (CExUsrInfo *)cServMsg.GetMsgBody();
                BOOL32 bAdd = (cServMsg.GetCurPktIdx() == 0) ? FALSE : TRUE;
                if (!SetUsrInfo(pcUsrInfo, byUsrNum, bAdd))
                {
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set usr info failed.insid.%d\n", GetInsID());
                    return;
                }
            }            
            break;

        case MCU_NPLUS_CONFMTUPDATE_REQ:
            {
                if (0 != cServMsg.GetMsgBodyLen()%sizeof(TMtInfo))
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(mtinfo)\n", cServMsg.GetMsgBodyLen());
                    return;
                }
              
                TMtInfo *ptMtInfo = (TMtInfo *)cServMsg.GetMsgBody();
                u8 byMtNum = cServMsg.GetMsgBodyLen()/sizeof(TMtInfo);

                if (!SetConfMtInfo(cConfId, ptMtInfo, byMtNum))
                {
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set mt info failed. insid.%d\n", GetInsID());
                    return;
                }
            }            
            break;

        case MCU_NPLUS_SMCUINFOUPDATE_REQ:
            {
                if (0 != cServMsg.GetMsgBodyLen()%sizeof(TSmcuCallnfo))
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(smcuinfo)\n", cServMsg.GetMsgBodyLen());
                    return;
                }
				
                TSmcuCallnfo *ptSmcuCallInfo = (TSmcuCallnfo *)cServMsg.GetMsgBody();
                u8 bySmcuNum = cServMsg.GetMsgBodyLen()/sizeof(TSmcuCallnfo);
				
                if (!SetConfSmcuCallInfo(cConfId, ptSmcuCallInfo, bySmcuNum))
                {
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set smcu call info failed. insid.%d\n", GetInsID());
                    return;
                }
            }            
            break;

        case MCU_NPLUS_CHAIRUPDATE_REQ:
            {
                if (sizeof(TMtAlias) != cServMsg.GetMsgBodyLen())
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(chair)\n", cServMsg.GetMsgBodyLen());
                    return;
                }

                TMtAlias *ptAlias = (TMtAlias *)cServMsg.GetMsgBody();
                if (!SetChairman(cConfId, ptAlias))
                {
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set chair info failed.insid.%d\n", GetInsID());
                    return;
                }
            }            
            break;

        case MCU_NPLUS_SPEAKERUPDATE_REQ:
            {
                if (sizeof(TMtAlias) != cServMsg.GetMsgBodyLen())
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(speaker)\n", cServMsg.GetMsgBodyLen());
                    return;
                }
             
                TMtAlias *ptAlias = (TMtAlias *)cServMsg.GetMsgBody();
                if (!SetSpeaker(cConfId, ptAlias))
                {
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set speaker info failed.insid.%d\n", GetInsID());
                    return;
                }
            }            
            break;

        case MCU_NPLUS_VMPUPDATE_REQ:
            {
                if (sizeof(TNPlusVmpInfo) + sizeof(TVMPParam) > cServMsg.GetMsgBodyLen())
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(vmpinfo)\n", cServMsg.GetMsgBodyLen());
                    return;
                }

                TNPlusVmpInfo *ptVmpInfo = (TNPlusVmpInfo *)cServMsg.GetMsgBody();
                TVMPParam *ptParam = (TVMPParam *)(cServMsg.GetMsgBody() + sizeof(TNPlusVmpInfo));
                if (!SetConfVmpInfo(cConfId, ptVmpInfo, ptParam))
                {
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set vmp info failed.insid.%d\n", GetInsID());
                    return;
                }

				if (sizeof(TNPlusVmpInfo) + sizeof(TVMPParam) < cServMsg.GetMsgBodyLen())
				{
					u16 wPackVmpLen = 0;
					if(!SetConfMultiVmpParam(cConfId,cServMsg.GetMsgBody()+ sizeof(TNPlusVmpInfo)+sizeof(TVMPParam),wPackVmpLen))
					{
						PostReplyBack(pcMsg->event+2);
						LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set multi vmp info failed.insid.%d\n", GetInsID());
						
						return;
					}
				}
            }            
            break;

		//[chendaiwei 2010/09/27]N+1备份处理主发给备的智能混音状态更新
		case MCU_NPLUS_AUTOMIXUPDATE_REQ:
            {
                if (sizeof(BOOL32) != cServMsg.GetMsgBodyLen())
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(automix)\n", cServMsg.GetMsgBodyLen());
                    return;
                }
                BOOL32 bStartAutoMix = *(BOOL32 *)cServMsg.GetMsgBody();
				if ( !bStartAutoMix )
				{
					LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] stop NPlus auto mixing.");
				}
                if (!SetConfAutoMix(cConfId,bStartAutoMix))
                {
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set auto mix failed.insid.%d\n", GetInsID());
                    return;
                }
            }            
            break;
        case MCU_NPLUS_RASINFOUPDATE_REQ:
            {
                if ( sizeof(TRASInfo) != cServMsg.GetMsgBodyLen() ) 
                {
                    PostReplyBack(pcMsg->event+2);
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(rasinfo)\n", cServMsg.GetMsgBodyLen() );
                    return;
                }
                TRASInfo tRASInfo = *(TRASInfo*)cServMsg.GetMsgBody();
                SetRASInfo(&tRASInfo);
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] set RasInfo. Ras EPID Length[%u], EPId Alias[%s], MCUE164[%s]!\n",tRASInfo.GetEPID()->GetIDlength(),tRASInfo.GetEPID()->GetIDAlias(),tRASInfo.GetMcuE164());               

                if ( tRASInfo.GetEPID()->GetIDlength() == 0 )
                {
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] EPID has set to SLAVE mcu Error!\n");
                }
            }
            break;

        default: 
            LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuDataUpdateReq] invalid msg (%s:%d)\n",
                                        OspEventDesc(pcMsg->event), pcMsg->event);
            return;

    }
    //ack
    PostReplyBack(pcMsg->event+1);
    return;
}

/*=============================================================================
函 数 名： ProcMcuEqpCapNotif
功    能： mcu基本外设能力检查（目前只进行外设个数检查）
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/29  4.0			许世林                  创建
=============================================================================*/
void CNPlusInst::ProcMcuEqpCapNotif( const CMessage * pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    if (sizeof(TNPlusEqpCap) > cServMsg.GetMsgBodyLen())
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuEqpCapNotif] invalid msg body len(%d)\n", cServMsg.GetMsgBodyLen());
        return;
    }

    TNPlusEqpCap tLocalCap = g_cNPlusApp.GetMcuEqpCap();
    TNPlusEqpCap tRegMcuCap = *(TNPlusEqpCap *)cServMsg.GetMsgBody();

	TNPlusEqpCapEx tLocalCapEx = g_cNPlusApp.GetMcuEqpCapEx();
	
	TNPlusEqpCapEx tRegCapEx;
	if(cServMsg.GetMsgBodyLen()>sizeof(TNPlusEqpCap))
	{
		BOOL32 bUnkownInfo = FALSE;
		u16 wUnpackLen = 0;
		g_cNPlusApp.UnPackNplusExInfo(tRegCapEx,cServMsg.GetMsgBody()+sizeof(TNPlusEqpCap),wUnpackLen,bUnkownInfo);
		
		if(bUnkownInfo)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuEqpCapNotif] registing mcu eqp type couldnot be distinguished . disconnect node!\n", 
				cServMsg.GetMsgBodyLen());
			OspDisconnectTcpNode(m_dwMcuNode);
			OspDisconnectTcpNode(m_dwMcuNodeB);
			//  [2/21/2011 chendaiwei]主MCU外设能力大于备MCU，认为备MCU不具备备份主MCU的能力。取消RTD检测，以避免
			//  触发备MCU恢复会议的流程。
			KillTimer(MCU_NPLUS_RTD_TIMER);
			ClearInst();
			
			return;
		}
	}
	
	if(IsLocalEqpCapLowerThanRemote(tLocalCap,tLocalCapEx,tRegMcuCap,tRegCapEx))
	{
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcMcuEqpCapNotif] registing mcu eqpcap is larger then local. disconnect node!\n", 
            cServMsg.GetMsgBodyLen());
        OspDisconnectTcpNode(m_dwMcuNode);
		OspDisconnectTcpNode(m_dwMcuNodeB);
		//  [2/21/2011 chendaiwei]主MCU外设能力大于备MCU，认为备MCU不具备备份主MCU的能力。取消RTD检测，以避免
		//  触发备MCU恢复会议的流程。
		KillTimer(MCU_NPLUS_RTD_TIMER);
		ClearInst();
		
        return;
	}
}

/*=============================================================================
函 数 名： ProcConfRollback
功    能： 发送会议数据给主mcu恢复会议，即模式切换回来
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/30  4.0			许世林                  创建
=============================================================================*/
void CNPlusInst::ProcConfRollback( const CMessage * pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u16 wEvent;

    switch (cServMsg.GetEventId())
    {
    case MCU_NPLUS_CONFDATAUPDATE_REQ:
        wEvent = MCU_NPLUS_CONFROLLBACK_REQ;
        break;

    case MCU_NPLUS_USRINFOUPDATE_REQ:
        wEvent = MCU_NPLUS_USRROLLBACK_REQ;
        break;

    case MCU_NPLUS_USRGRPUPDATE_REQ:
        wEvent = MCU_NPLUS_GRPROLLBACK_REQ;
        break;

    default: 
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[ProcConfRollback] invalid msg(%s) received.\n", OspEventDesc(cServMsg.GetEventId()));
        return;

    }

    CServMsg cMsg;
    cMsg.SetEventId(wEvent);
    cMsg.SetMsgBody(cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
    SndMsg2NplusMcu(cMsg);

    return;
}

/*=============================================================================
函 数 名： ProcReset
功    能： 强制停止服务
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/30  4.0			顾振华                  创建
=============================================================================*/
void CNPlusInst::ProcReset( void )
{
    if ( OspIsValidTcpNode( m_dwMcuNode ) )
    {
        OspDisconnectTcpNode(m_dwMcuNode);
    }

    ClearInst();
}

/*=============================================================================
函 数 名： ProcRtdRsp
功    能： 主mcu 发回的rtd响应处理函数 
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/23  4.0			许世林                  创建
=============================================================================*/
void CNPlusInst::ProcRtdRsp( void )
{
    m_wRtdFailTimes = 0;    
    SetTimer(MCU_NPLUS_RTD_TIMER, g_cNPlusApp.GetRtdTime()*1000);   
    LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[ProcRtdRsp] InsId(%d) mcu(0x%x)/(0x%x).\n", GetInsID(), m_dwMcuIpAddr, m_dwMcuIpAddrB); 
    return;
}

/*=============================================================================
函 数 名： ProcRtdTimeOut
功    能： 检测主mcu 是否正常工作的定时器处理函数
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/23  4.0			许世林                  创建
=============================================================================*/
void CNPlusInst::ProcRtdTimeOut( void )
{
	if( CurState() == STATE_IDLE )
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[ProcRtdTimeOut] invalid ins state(%d)(id.%d)\n", CurState(), GetInsID());
		return;
	}

    LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[ProcRtdTimeOut] Rtd failed %d times!\n", m_wRtdFailTimes);

	m_wRtdFailTimes++;
	
    //需要进行模式切换，恢复此实例对应mcu的会议信息
    if (m_wRtdFailTimes > (g_cNPlusApp.GetRtdNum()) )
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[ProcRtdTimeOut] mode switch and RESTORE mcu(0x%x)/(0x%x) conf.\n", m_dwMcuIpAddr, m_dwMcuIpAddrB);       
        RestoreMcuConf();
        m_dwMcuIId = 0;
        m_dwMcuNode = INVALID_NODE;
        m_dwMcuIIdB = 0;
        m_dwMcuNodeB = INVALID_NODE;
    }
    else
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[ProcRtdTimeOut] rtd time out and retrying mcu(0x%x)/(0x%x).\n", m_dwMcuIpAddr, m_dwMcuIpAddrB); 
        CServMsg cMsg;
        cMsg.SetEventId(MCU_NPLUS_RTD_REQ);
        SndMsg2NplusMcu(cMsg);
        SetTimer(MCU_NPLUS_RTD_TIMER, g_cNPlusApp.GetRtdTime()*1000);
    }    
    return;
}

/*=============================================================================
函 数 名： ProcDisconnect
功    能： osp断链处理函数，暂时不进行模式切换
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/23  4.0			许世林                  创建
2013/03/19              liaokang              支持主备N+1备双备份
=============================================================================*/
void CNPlusInst::ProcDisconnect( const CMessage * pcMsg )
{
    u32 dwNode = *(u32*)pcMsg->content;
    if( dwNode == m_dwMcuNode )
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[ProcDisconnect] MCU(0x%x) Node.%d Disconnected.\n", m_dwMcuIpAddr, m_dwMcuNode);
        m_dwMcuIId = 0;
        m_dwMcuNode = INVALID_NODE;
    } 
    else
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[ProcDisconnect] MCU(0x%x) Node.%d Disconnected.\n", m_dwMcuIpAddrB, m_dwMcuNodeB);
        m_dwMcuIIdB = 0;
        m_dwMcuNodeB = INVALID_NODE;
    }

    return;
}

/*====================================================================
    函数名      : MsgSndPassCheck
    功能        : 消息发送过滤
    算法实现    :
    引用全局变量:
    输入参数说明: 
    返回值说明  :
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人      修改内容
    2013/03/15              liaokang      创建
====================================================================*/
BOOL32 CNPlusInst::MsgSndPassCheck(u16 wEvent)
{
    BOOL32 bRet = TRUE;
    if( !g_cMSSsnApp.JudgeSndMsgPass() &&
        ( MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState() || MCU_NPLUS_MASTER_IDLE == g_cNPlusApp.GetLocalNPlusState() ) )
    {        
        switch(wEvent)
        {            
        case MCU_NPLUS_REG_REQ:
        case MCU_NPLUS_RTD_RSP:
            break;
        default:
            bRet = FALSE;
            break;
        }
    }
    return bRet;
}

/*=============================================================================
函 数 名： PostReplyBack
功    能： 备份mcu应答消息发送
算法实现： 
全局变量： 
参    数：  u16 wEvent
           u8 *const pbyMsg
           u16 wMsgLen
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/22  4.0			许世林                  创建
2013/03/18              liaokang              修改入参
=============================================================================*/
void CNPlusInst::PostReplyBack(u16 wEvent, u16 wErrorCode/* = 0*/, u8 *const pbyMsg/* = NULL*/, 
                               u16 wMsgLen/* = 0*/,u32 dwMcuIId/* = 0*/,u32 dwMcuNode/* = 0*/)
{
    CServMsg cServMsg;
    cServMsg.SetEventId(wEvent);
    cServMsg.SetErrorCode(wErrorCode);
    if (wMsgLen > 0 && NULL != pbyMsg)
    {
        cServMsg.SetMsgBody(pbyMsg, wMsgLen);
    }
    SndMsg2NplusMcu(cServMsg,dwMcuIId,dwMcuNode);
    return;
}

/*====================================================================
    函数名      : SndMsg2NplusMcu
    功能        : Nplus消息发送
    算法实现    :
    引用全局变量:
    输入参数说明: CServMsg &cServMsg
                  u32 dwMcuIId
                  u32 dwMcuNode
    返回值说明  :
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人      修改内容
    2013/03/15              liaokang      创建
====================================================================*/
void CNPlusInst::SndMsg2NplusMcu(CServMsg &cServMsg,u32 dwMcuIId/* = INVALID_NODE*/,u32 dwMcuNode/* = INVALID_NODE*/)
{   
    if( (INVALID_NODE == dwMcuIId && INVALID_NODE != dwMcuNode ) || 
        (INVALID_NODE != dwMcuIId && INVALID_NODE == dwMcuNode)  )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[SndMsg2NplusMcu] invalid param!\n" );
        return;
    } 

    if( !MsgSndPassCheck(cServMsg.GetEventId()) )
    {
        LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "[SndMsg2NplusMcu] Event.%d<%s> MsgSndPassCheck failed!\n",
            cServMsg.GetEventId(), ::OspEventDesc(cServMsg.GetEventId()));
        return;
    } 

    // 指定目的
    if( INVALID_NODE != dwMcuIId && 
        INVALID_NODE != dwMcuNode )
    {
        post(dwMcuIId, cServMsg.GetEventId(), cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), dwMcuNode);
        LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[SndMsg2NplusMcu] send msg.%d<%s> !\n", cServMsg.GetEventId(), ::OspEventDesc(cServMsg.GetEventId()));  
        return;
    } 

    // 非指定目的：double link
    if ( INVALID_NODE != m_dwMcuNode && 
         OspIsValidTcpNode(m_dwMcuNode) ) 
    {
        post(m_dwMcuIId, cServMsg.GetEventId(), cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), m_dwMcuNode);
        LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[SndMsg2NplusMcu] send msg.%d<%s> to Node.%d!\n", cServMsg.GetEventId(), ::OspEventDesc(cServMsg.GetEventId()), m_dwMcuNode);  
    }
    
    if ( INVALID_NODE != m_dwMcuNodeB && 
         OspIsValidTcpNode(m_dwMcuNodeB) )
    {
        post(m_dwMcuIIdB, cServMsg.GetEventId(), cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), m_dwMcuNodeB);
        LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[SndMsg2NplusMcu] send msg.%d<%s> to Node.%d!\n", cServMsg.GetEventId(), ::OspEventDesc(cServMsg.GetEventId()), m_dwMcuNodeB);  
    }

    return;
}

/*=============================================================================
函 数 名： RestoreMcuConf
功    能： 模式切换，恢复此实例对应mcu的会议信息
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/23  4.0			许世林                  创建
=============================================================================*/
void CNPlusInst::RestoreMcuConf( void )
{
    //检查状态机
    if (MCU_NPLUS_SLAVE_IDLE != g_cNPlusApp.GetLocalNPlusState())
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[RestoreMcuConf] invalid NPlusState.%d, restore mcu conf failed.\n",
                  g_cNPlusApp.GetLocalNPlusState());
        
        ClearInst();
        return;
    }

    // 转码
    TranslateEncodingForm();

    // 特殊处理1: N+1 备份注册GK, 备份MCU取代主MCU工作，整体注销主MCU上所有的实
    //            体ID。否则，在主备配相同GK的情况下，备份MCU召开的对应会议将不能实现邀请终端 [12/23/2006-zbq]
	LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"GkIp:%d, GkDriId: %d\n", g_cMcuAgent.GetGkIpAddr(), g_cMcuVcApp.GetRegGKDriId());
    if( g_cMcuAgent.GetGkIpAddr() != 0 && 0 != g_cMcuVcApp.GetRegGKDriId() )
    {
		
        UnRegAllInfoOfSwitchedMcu();
    }

    // 特殊处理2: N+1 备份启用GK计费，备份 MCU取代主MCU工作，需先结束当前正在计
    //            费的会议的计费，并生成话单。备份MCU恢复会议后生成新的后续话单[12/25/2006-zbq]
    if( g_cMcuAgent.GetGkIpAddr() != 0 && 0 != g_cMcuVcApp.GetRegGKDriId() )
    {
        StopConfChargeOfSwitchedMcu();
    }

    //切换状态机，回滚后才恢复状态机
    g_cNPlusApp.SetLocalNPlusState(MCU_NPLUS_SLAVE_SWITCH);
    
    //记录此实例id
    g_cNPlusApp.SetMcuSwitchedInsId( (u8)GetInsID(), m_dwMcuIpAddr );
    
    NEXTSTATE(STATE_SWITCHED);

    u8 byIdx;

    //若当前用户不是admin则断开会控，若为admin则通知mcs更新用户组和用户信息
    for (byIdx = 1; byIdx <= MAXNUM_MCU_MC; byIdx++)
    {
        if (USRGRPID_SADMIN != CMcsSsn::GetUserGroup(byIdx))
        {
            post(MAKEIID(AID_MCU_MCSSN, byIdx), OSP_DISCONNECT);
        }
        else
        {
            post(MAKEIID(AID_MCU_MCSSN, byIdx), MCS_MCU_GETUSERGRP_REQ);
            post(MAKEIID(AID_MCU_MCSSN, byIdx), MCS_MCU_GETUSERLIST_REQ);
        }
    }
        
    //恢复会议信息
    TMtAlias        atAlias[MAXNUM_CONF_MT];
    u16             awMtDialBitRate[MAXNUM_CONF_MT];
    s8              achAliasBuf[SERV_MSG_LEN];
    u16             wAliasBufLen = 0;
    TNPlusConfData  *ptConfData;
    TVmpModule      tVmpMod;
    TMtInfo         *ptMtInfo;

    CServMsg cServMsg;
    cServMsg.SetSrcMtId(CONF_CREATE_NPLUS);
    for(byIdx = 0; byIdx < MAXNUM_ONGO_CONF; byIdx++)
    {
        ptConfData = &m_atConfData[byIdx];

        if (ptConfData->IsNull())
        {
            continue;
        }

		// 自动合成模式下不作成员处理，否则会导致自动画面合成会议的恢复和回滚失败 [01/12/2007-zbq]
        BOOL32 bVmpMod = ptConfData->m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE ? TRUE : FALSE;
        if (bVmpMod)
        {
            tVmpMod.SetVmpParam(ptConfData->m_tConf.m_tStatus.GetVmpParam());

            // 画面合成成员信息只能保存到会议信息里，本结构只保存其他信息 [12/14/2006-zbq]
            for( u8 byIndex = 0; byIndex <  MAXNUM_MPUSVMP_MEMBER; byIndex ++ )
            {
                tVmpMod.m_tVMPParam.ClearVmpMember(byIndex);
            }
        }
		
        for(u8 byLoop = 0; byLoop < ptConfData->m_byMtNum; byLoop++)
        {            
            ptMtInfo = &ptConfData->m_atMtInConf[byLoop];
            atAlias[byLoop].m_AliasType = mtAliasTypeTransportAddress;

			for( u8 bySmcuIdx = 0; bySmcuIdx < MAXNUM_SUB_MCU; bySmcuIdx++ )
			{
				if( m_atConfExData[byIdx].m_atSmcuCallInfo[bySmcuIdx].m_dwMtAddr == ptMtInfo->GetMtAddr())
				{
					atAlias[byLoop].m_AliasType = mtAliasTypeE164;
					atAlias[byLoop].SetE164Alias(m_atConfExData[byIdx].m_atSmcuCallInfo[bySmcuIdx].m_achAlias);
					break;
				}
			}

            atAlias[byLoop].m_tTransportAddr = ptMtInfo->GetMtAddr();
            awMtDialBitRate[byLoop] = /*htons(*/ptMtInfo->GetCallBitrate()/*)*/;

            //更新vmpmod中终端索引
            if (bVmpMod && ptConfData->m_tConf.m_tStatus.GetVMPMode() ==  CONF_VMPMODE_CTRL)
            {
                for (u8 byChnl = 0; byChnl < MAXNUM_MPUSVMP_MEMBER; byChnl++)
                {
					u8 byType = ptConfData->m_tVmpInfo.m_abyMemberType[byChnl];
					//会控指定 设置成员
					if( VMP_MEMBERTYPE_MCSSPEC == byType && ptConfData->m_tVmpInfo.IsMtInVmpMem(*ptMtInfo, byChnl))
					{
						tVmpMod.SetVmpMember(byChnl, byLoop+1, byType);
					}
					//跟随 设置成员193及类型
					else if( byType != VMP_MEMBERTYPE_NULL && byType != VMP_MEMBERTYPE_MCSSPEC)
					{
						tVmpMod.SetVmpMember(byChnl, MAXNUM_CONF_MT+1, byType);
					}
					else
					{
						//noting to do
					}
                } 
            }
        }

		//清空画面合成成员状态 [7/11/2012 chendaiwei]
		TVMPParam tNullVmp;
		memset((void*)&tNullVmp,0,sizeof(tNullVmp));
		ptConfData->m_tConf.m_tStatus.SetVmpParam(tNullVmp);

        PackTMtAliasArray(atAlias, awMtDialBitRate, ptConfData->m_byMtNum, achAliasBuf, wAliasBufLen);

        //去掉tvwall信息，因为tvwall与具体的外设id相关，不能进行恢复
        ptConfData->m_tConf.SetHasTvwallModule(FALSE);
        ptConfData->m_tConf.SetHasVmpModule(bVmpMod);

        ptConfData->m_tConf.m_tStatus.m_tConfMode.SetTakeMode(CONF_TAKEMODE_ONGOING);   
		
		//清空录放像，轮询，轮训点名，录放像,HDU轮询状态 [4/27/2013 chendaiwei]
		ptConfData->m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_NONE);
		ptConfData->m_tConf.m_tStatus.SetRollCallMode(ROLLCALL_MODE_NONE);
		ptConfData->m_tConf.m_tStatus.SetNoRecording();
		ptConfData->m_tConf.m_tStatus.SetNoPlaying();
		THduPollInfo tNullInfo;
		ptConfData->m_tConf.m_tStatus.SetHduPollInfo(tNullInfo);

		//清空会议自动录像属性[8/6/2013 chendaiwei]
		TConfAutoRecAttrb tNullRecAttrb;
		ptConfData->m_tConf.SetAutoRecAttrb(tNullRecAttrb);

        wAliasBufLen = htons(wAliasBufLen);
        //confinfo
        cServMsg.SetMsgBody((u8*)&ptConfData->m_tConf, sizeof(TConfInfo));

        //mt alias
        cServMsg.CatMsgBody((u8 *)&wAliasBufLen, sizeof(wAliasBufLen));
		cServMsg.CatMsgBody((u8 *)achAliasBuf, ntohs(wAliasBufLen));        

        //vmp
        if (bVmpMod)
        {
            cServMsg.CatMsgBody((u8 *)&tVmpMod, sizeof(tVmpMod));
        }

		//扩展能力集勾选信息[1/4/2012 chendaiwei]
		u8 *pTConfExInfo = (u8*)&m_atConfExData[byIdx];
		u16 wBuffLen = ntohs(*(u16*)pTConfExInfo);
		if(wBuffLen != 0)
		{
			cServMsg.CatMsgBody(pTConfExInfo,wBuffLen+sizeof(u16));
		}

		cServMsg.CatMsgBody((u8*)&m_atConfExData[byIdx].m_atSmcuCallInfo[0],sizeof(m_atConfExData[byIdx].m_atSmcuCallInfo));

		TNPlusVmpParam atVmpParam[MAXNUM_PERIEQP];
		u8 byVmpNum = 0;
		GetConfMultiVmpParam(byIdx,atVmpParam,byVmpNum);
		cServMsg.CatMsgBody((u8*)&byVmpNum,sizeof(byVmpNum));
		cServMsg.CatMsgBody((u8*)&atVmpParam[0],sizeof(atVmpParam[0])*byVmpNum);

        //发消息创建会议        
        post(MAKEIID( AID_MCU_VC, CInstance::DAEMON ), MCU_CREATECONF_NPLUS, 
                          cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );			
    }            
    
    return;
}

/*=============================================================================
函 数 名： GetGrpUsrCount
功    能： 获取用户组个数
算法实现： 
全局变量： 
参    数：  u8 byGrpId
           u8 &byMaxNum
           u8 &byNum
返 回 值： BOOL32  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/24  4.0			许世林                  创建
=============================================================================*/
BOOL32  CNPlusInst::GetGrpUsrCount( u8 byGrpId, u8 &byMaxNum, u8 &byNum )
{
    // 如果是超级管理员返回系统值
    if (byGrpId == USRGRPID_SADMIN)
    {        
        byMaxNum = MAXNUM_GRPUSRNUM;
        byNum = m_byUsrNum;
        return TRUE;
    }

    TUsrGrpInfo tGrpInfo;
    if (!m_cUsrGrpsInfo.GetGrpById( byGrpId, tGrpInfo ))
    {
        return FALSE;
    }

    byNum = 0;
    byMaxNum = tGrpInfo.GetMaxUsrNum();  
    for (u8 byLoop = 0; byLoop < m_byUsrNum; byLoop++)
    {        
        if ( byGrpId == m_acUsrInfo[byLoop].GetUsrGrpId() )
        {
            byNum ++;
        }
    }

    return TRUE;
}

/*=============================================================================
函 数 名： GetGrpUserList
功    能： 获取用户组用户列表
算法实现： 
全局变量： 
参    数：  u8 byGrpId
           u8 *pbyBuf
           u8 &byUsrItr
           u8 &byUserNumInPack
返 回 值： BOOL32  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/24  4.0			许世林                  创建
=============================================================================*/
BOOL32  CNPlusInst::GetGrpUserList( u8 byGrpId, u8 *pbyBuf, u8 &byUsrItr, u8 &byUserNumInPack )
{
	if (NULL == pbyBuf)
	{
		return FALSE;
	}

    byUserNumInPack = 0;
	for (; byUserNumInPack < USERNUM_PERPKT && byUsrItr < m_byUsrNum; byUsrItr ++ )
	{
        if ( byGrpId == USRGRPID_SADMIN ||
             byGrpId == m_acUsrInfo[byUsrItr].GetUsrGrpId()  )
        {
			memcpy(pbyBuf + byUserNumInPack * sizeof(CExUsrInfo), &m_acUsrInfo[byUsrItr], sizeof(CExUsrInfo));
			byUserNumInPack++;
        }
	}

	if (0 == byUserNumInPack)
	{
		return FALSE;
	}
    
	return TRUE;
}
    
CExUsrInfo* CNPlusInst::GetUserPtr()
{
    return m_acUsrInfo;
}

CUsrGrpsInfo* CNPlusInst::GetUsrGrpInfo()
{
    return &m_cUsrGrpsInfo;
}

/*=============================================================================
函 数 名： DaemonInstanceEntry
功    能： 备份Server模式下负责实例消息的分发，备份Client模式下demon实例对应本地mcu
算法实现： 
全局变量： 
参    数： CMessage * const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/21  4.0			许世林                  创建
=============================================================================*/
void CNPlusInst::DaemonInstanceEntry( CMessage* const pcMsg, CApp* pcApp )
{
    if ( NULL == pcMsg )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::DaemonInstanceEntry] invalid msg body!\n" );
        return;
    }

    switch( pcMsg->event )
    {
    case OSP_POWERON:
        DaemonPowerOn();
        break;

    case VC_NPLUS_RESET_NOTIF:
    case MCS_MCU_NPLUSRESET_CMD:    // Mcs 请求Reset
        DaemonReset();
        break;

    case MCS_MCU_NPLUSROLLBACK_REQ:
        DaemonMcsRollBack(pcMsg, pcApp);
        break;
        
    case OSP_DISCONNECT:
        DaemonDisconnect();
        break;

    case MCU_NPLUS_REG_REQ:
        DaemonRegNPlusMcuReq( pcMsg, pcApp );
        break;

    case MCU_NPLUS_REG_ACK:
    case MCU_NPLUS_REG_NACK:
        DaemonRegNPlusMcuRsp( pcMsg );
        break;

    case MCU_NPLUS_CONNECT_TIMER:
        DaemonConnectNPlusMcuTimeOut(  );
        break;
    
    case MCU_NPLUS_REG_TIMER:
        DaemonRegNPlusMcuTimeOut();
        break;

        //主mcu数据上报消息
    case VC_NPLUS_MSG_NOTIF:  
        DaemonVcNPlusMsgNotif( pcMsg );
        break;

        //主备rtd消息处理
    case MCU_NPLUS_RTD_REQ:
        DaemonRtdReq();
        break;

    case MCU_NPLUS_CONFINFOUPDATE_ACK:    
    case MCU_NPLUS_USRGRPUPDATE_ACK:
    case MCU_NPLUS_CONFMTUPDATE_ACK:
	case MCU_NPLUS_SMCUINFOUPDATE_ACK:
    case MCU_NPLUS_CHAIRUPDATE_ACK:    
    case MCU_NPLUS_SPEAKERUPDATE_ACK:    
    case MCU_NPLUS_VMPUPDATE_ACK:    
	case MCU_NPLUS_AUTOMIXUPDATE_ACK:
    case MCU_NPLUS_CONFDATAUPDATE_ACK:    
    case MCU_NPLUS_USRINFOUPDATE_ACK:
    case MCU_NPLUS_RASINFOUPDATE_ACK:
        
    case MCU_NPLUS_CONFINFOUPDATE_NACK:
    case MCU_NPLUS_USRGRPUPDATE_NACK:
    case MCU_NPLUS_CONFMTUPDATE_NACK:
	case MCU_NPLUS_SMCUINFOUPDATE_NACK:
    case MCU_NPLUS_CHAIRUPDATE_NACK:
    case MCU_NPLUS_SPEAKERUPDATE_NACK:
    case MCU_NPLUS_VMPUPDATE_NACK:
    case MCU_NPLUS_CONFDATAUPDATE_NACK:
    case MCU_NPLUS_USRINFOUPDATE_NACK:
    case MCU_NPLUS_RASINFOUPDATE_NACK:
        
        DaemonDataUpdateRsp( pcMsg );
        break;

    //会议回滚
    case MCU_NPLUS_CONFROLLBACK_REQ:
        DaemonConfRollbackReq( pcMsg );
        break;
    //用户回滚
    case MCU_NPLUS_USRROLLBACK_REQ:
        DaemonUsrRollbackReq();
        break;
    //用户组回滚
    case MCU_NPLUS_GRPROLLBACK_REQ:
        DaemonGrpRollbackReq();
        break;
        
    default:
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::DaemonInstanceEntry] invalid event(%d): %s!\n", 
            pcMsg->event, OspEventDesc(pcMsg->event));
        break;
    }

    return;
}   

/*=============================================================================
函 数 名： DaemonPowerOn
功    能： 上电初始化
算法实现： 
全局变量： 
参    数： CMessage * pcMsg
返 回 值： void  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/21  4.0			许世林                  创建
=============================================================================*/
void  CNPlusInst::DaemonPowerOn( void )
{    
    //若为N+1备份正常工作模式，则需要向备份模式mcu建链
    if ( MCU_NPLUS_MASTER_IDLE == g_cNPlusApp.GetLocalNPlusState() )
    {
        SetTimer(MCU_NPLUS_CONNECT_TIMER, 10);  //立即进行尝试建链
    }
    return;
}

/*=============================================================================
函 数 名： DaemonReset
功    能： 通知N+1备份服务器重置，停止备份服务，但不断开连接的客户端
算法实现： 
全局变量： 
参    数： CMessage * pcMsg
返 回 值： void  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/12/13  4.0			顾振华                  创建
=============================================================================*/
void  CNPlusInst::DaemonReset( void )
{    
    // 若为N+1备份服务工作模式，
    if ( MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState() )
    {
        // 通知正在工作的实例停止服务
        u8 byInsId = g_cNPlusApp.GetMcuSwitchedInsId();
        post(MAKEIID(AID_MCU_NPLUSMANAGER, byInsId), VC_NPLUS_RESET_NOTIF);

        //结束所有会议
        OspPost(MAKEIID(AID_MCU_VC, CInstance::EACH), MCS_MCU_RELEASECONF_REQ, NULL, 0, 0,
                MAKEIID(AID_MCU_NPLUSMANAGER, GetInsID()));            
    
        //状态机翻转
        g_cNPlusApp.SetLocalNPlusState(MCU_NPLUS_SLAVE_IDLE);        

    }

    return;
}

/*=============================================================================
函 数 名： DaemonMcsRollBack
功    能： 用户请求回滚
算法实现： 
全局变量： 
参    数： CMessage * pcMsg
返 回 值： void  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/12/15  4.0			顾振华                  创建
=============================================================================*/
void  CNPlusInst::DaemonMcsRollBack( const CMessage * pcMsg, CApp* pcApp  )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->event );
    // 若为N+1备份服务工作模式，
    if ( MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState() )
    {
        // 是否自动回滚
        TLocalInfo tLocalInfo;
        g_cMcuAgent.GetLocalInfo(&tLocalInfo);
        if ( tLocalInfo.GetIsNPlusRollBack() )
        {
            // 默认自动回滚，NACK
            cServMsg.SetErrorCode( ERR_MCU_NPLUS_AUTOROLLBACK );
            CMcsSsn::SendMsgToMcsSsn( cServMsg.GetSrcSsnId(), 
                                          pcMsg->event + 2, 
                                          cServMsg.GetServMsg(),
                                          cServMsg.GetServMsgLen() );
            return;
        }

        // 检查MCU是否已经重新连接        
        u8 byInsId = g_cNPlusApp.GetMcuSwitchedInsId();
        CNPlusInst *pcInst = (CNPlusInst *)pcApp->GetInstance(byInsId);
        if ( pcInst->CurState() == STATE_NORMAL )
        {
            //恢复用户组和用户信息
            OspPost(MAKEIID(AID_MCU_MCSSN, CInstance::DAEMON), NPLUS_VC_DATAUPDATE_NOTIF, NULL, 0, 0,
                    MAKEIID(AID_MCU_NPLUSMANAGER, byInsId)); 

            //恢复会议所有信息
            OspPost(MAKEIID(AID_MCU_VC, CInstance::EACH), NPLUS_VC_DATAUPDATE_NOTIF, NULL, 0, 0,
                    MAKEIID(AID_MCU_NPLUSMANAGER, byInsId));            
    
            //状态机翻转
            g_cNPlusApp.SetLocalNPlusState(MCU_NPLUS_SLAVE_IDLE);    

            pcInst->ClearInst();
        }
        else
        {
            // 备份MCU未连接，NACK
            cServMsg.SetErrorCode( ERR_MCU_NPLUS_BAKCLIENT_NOTREG );
            CMcsSsn::SendMsgToMcsSsn( cServMsg.GetSrcSsnId(), 
                                          pcMsg->event + 2, 
                                          cServMsg.GetServMsg(),
                                          cServMsg.GetServMsgLen() );
            return;            
        }    
    }
}

/*=============================================================================
函 数 名： DaemonDisconnect
功    能： 备份mcu断链处理
算法实现： 
全局变量： 
参    数：  const CMessage * pcMsg
           CApp* pcApp
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/30  4.0			许世林                  创建
=============================================================================*/
void CNPlusInst::DaemonDisconnect( void  )
{
    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonDisconnect] node(%d) disconnect. connecting...\n", m_dwMcuNode);
    KillTimer( MCU_NPLUS_REG_TIMER );    
    ClearInst();
    g_cNPlusApp.SetLocalNPlusState(MCU_NPLUS_MASTER_IDLE);

    SetTimer( MCU_NPLUS_CONNECT_TIMER, NPLUS_CONNECTMCU_TIMEOUT );
    return;
}

/*=============================================================================
函 数 名： DaemonDataUpdateRsp
功    能： 数据更新消息相应处理函数 
算法实现： 目前只处理外设配置不足导致的同步失败
全局变量： 
参    数：  const CMessage * pcMsg
           CApp* pcApp
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/28  4.0			许世林                  创建
=============================================================================*/
void CNPlusInst::DaemonDataUpdateRsp( const CMessage * pcMsg )
{
    if ( pcMsg->event == MCU_NPLUS_CONFINFOUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_VMPUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_CONFDATAUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_CONFMTUPDATE_ACK ||
		 pcMsg->event == MCU_NPLUS_SMCUINFOUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_CHAIRUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_SPEAKERUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_USRINFOUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_USRGRPUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_RASINFOUPDATE_ACK ||
		 pcMsg->event == MCU_NPLUS_AUTOMIXUPDATE_ACK)
    {
        g_cNPlusApp.SetNPlusSynOk(TRUE);
    }
    else if ( pcMsg->event == MCU_NPLUS_CONFINFOUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_VMPUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_CONFDATAUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_CONFMTUPDATE_NACK ||
			  pcMsg->event == MCU_NPLUS_SMCUINFOUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_CHAIRUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_SPEAKERUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_USRINFOUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_USRGRPUPDATE_NACK ||
			  pcMsg->event == MCU_NPLUS_AUTOMIXUPDATE_NACK)
    {
        g_cNPlusApp.SetNPlusSynOk(FALSE);
    }
    return;
}

/*=============================================================================
函 数 名： DaemonConfRollbackReq
功    能： 会议信息回滚
算法实现： 
全局变量： 
参    数：  const CMessage * pcMsg
           CApp* pcApp
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/30  4.0			许世林                  创建
=============================================================================*/
void CNPlusInst::DaemonConfRollbackReq( const CMessage * pcMsg )
{
    if (g_cNPlusApp.GetLocalNPlusState() != MCU_NPLUS_MASTER_CONNECTED)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonConfRollbackReq] invalid N+1 mode state(%d), ignore conf rollback msg\n", 
                  g_cNPlusApp.GetLocalNPlusState());
        return;
    }

    //ack
    PostReplyBack(pcMsg->event+1);

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    //若会议已经通过会议自动恢复召开，先结束，再进行恢复

	u8 *pbyMsgBuf = cServMsg.GetMsgBody();
    TNPlusConfData tConfData = *(TNPlusConfData *)pbyMsgBuf;
	pbyMsgBuf += sizeof(tConfData);

	u8 abyBuffer[CONFINFO_EX_BUFFER_LENGTH] = {0};
	u16 wBufferLen = 0; //扩展数据长度[11/16/2012 chendaiwei]
	
	TNPlusConfExData tConfExData;
	if(tConfData.m_tConf.HasConfExInfo())
	{
		wBufferLen = ntohs(*(u16*)(pbyMsgBuf))+sizeof(u16);
		memcpy(tConfExData.m_byConInfoExBuf,pbyMsgBuf,wBufferLen);

		pbyMsgBuf += wBufferLen;
	}

	if(cServMsg.GetMsgBodyLen()>sizeof(TNPlusConfData)+wBufferLen)
	{
		memcpy(tConfExData.m_atSmcuCallInfo,pbyMsgBuf,sizeof(tConfExData.m_atSmcuCallInfo));

		pbyMsgBuf += sizeof(tConfExData.m_atSmcuCallInfo);
	}
	
	TNPlusVmpParam atVmpParam[MAXNUM_PERIEQP];
	u8 byVmpNum = 0;

	if(cServMsg.GetMsgBodyLen() > sizeof(TNPlusConfData)+wBufferLen +sizeof(TSmcuCallnfo)*MAXNUM_SUB_MCU)
	{
		u16 wPackLen = 0;
		if(!UnPackVmpBufToVmpParam(pbyMsgBuf,atVmpParam,byVmpNum,wPackLen))
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonConfRollbackReq] UnPackVmpBufToVmpParam failed\n");

			return;
		}
	}

    if (tConfData.IsNull())
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonConfRollbackReq] confid is null, ignore conf rollback msg\n");
        return;
    }

    //恢复会议信息
    TMtAlias        atAlias[MAXNUM_CONF_MT];
    u16             awMtDialBitRate[MAXNUM_CONF_MT];
    s8              achAliasBuf[SERV_MSG_LEN];
    u16             wAliasBufLen = 0;
    TVmpModule      tVmpMod;
    TMtInfo         *ptMtInfo;

    cServMsg.SetSrcMtId(CONF_CREATE_NPLUS);    

	// 自动合成模式下不作成员处理，否则会导致自动画面合成会议的恢复和回滚失败 [01/12/2007-zbq]
    BOOL32 bVmpMod = tConfData.m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE ? TRUE : FALSE;
    if (bVmpMod)
    {
        tVmpMod.SetVmpParam(tConfData.m_tConf.m_tStatus.GetVmpParam());

        // 画面合成成员信息只能保存到会议信息里，本结构只保存其他信息 [12/14/2006-zbq]
        for( u8 byIndex = 0; byIndex <  MAXNUM_MPUSVMP_MEMBER; byIndex ++ )
        {
            tVmpMod.m_tVMPParam.ClearVmpMember(byIndex);
        }
    }

	TVmpModuleInfo tVmpModuleInfo;

    for(u8 byLoop = 0; byLoop < tConfData.m_byMtNum; byLoop++)
    {            
        ptMtInfo = &tConfData.m_atMtInConf[byLoop];

        atAlias[byLoop].m_AliasType = mtAliasTypeTransportAddress;
		for( u8 bySmcuIdx = 0; bySmcuIdx < MAXNUM_SUB_MCU; bySmcuIdx++ )
		{
			if( tConfExData.m_atSmcuCallInfo[bySmcuIdx].m_dwMtAddr == ptMtInfo->GetMtAddr())
			{
				atAlias[byLoop].m_AliasType = mtAliasTypeE164;
				atAlias[byLoop].SetE164Alias(tConfExData.m_atSmcuCallInfo[bySmcuIdx].m_achAlias);
				break;
			}
		}

        atAlias[byLoop].m_tTransportAddr = ptMtInfo->GetMtAddr();
        awMtDialBitRate[byLoop] = ptMtInfo->GetCallBitrate();

        //更新vmpmod中终端索引
        if (bVmpMod && tConfData.m_tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_CTRL)
        {
            for (u8 byChnl = 0; byChnl < MAXNUM_MPUSVMP_MEMBER; byChnl++)
            {
				u8 byType = tConfData.m_tVmpInfo.m_abyMemberType[byChnl];

				//会控指定 设置成员
				if( VMP_MEMBERTYPE_MCSSPEC == byType && tConfData.m_tVmpInfo.IsMtInVmpMem(*ptMtInfo, byChnl))
				{
					tVmpMod.SetVmpMember(byChnl, byLoop+1, byType);
				}
				//跟随 设置成员193及类型
				else if( byType != VMP_MEMBERTYPE_NULL && byType != VMP_MEMBERTYPE_MCSSPEC)
				{
					tVmpMod.SetVmpMember(byChnl, MAXNUM_CONF_MT+1, byType);
				}
				else
				{
					//noting to do
				}
			} 
        }
    }

	//清空画面合成成员状态 [7/11/2012 chendaiwei]
	TVMPParam tNullVmp;
	memset((void*)&tNullVmp,0,sizeof(tNullVmp));
	tConfData.m_tConf.m_tStatus.SetVmpParam(tNullVmp);

    PackTMtAliasArray(atAlias, awMtDialBitRate, tConfData.m_byMtNum, achAliasBuf, wAliasBufLen);

    //去掉tvwall信息，因为tvwall与具体的外设id相关，不能进行恢复
    tConfData.m_tConf.SetHasTvwallModule(FALSE);
    tConfData.m_tConf.SetHasVmpModule(bVmpMod);

    tConfData.m_tConf.m_tStatus.m_tConfMode.SetTakeMode(CONF_TAKEMODE_ONGOING);
	
	//清空录放像，轮询，轮训点名，录放像,HDU轮询状态 [4/27/2013 chendaiwei]
	tConfData.m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_NONE);
	tConfData.m_tConf.m_tStatus.SetRollCallMode(ROLLCALL_MODE_NONE);
    tConfData.m_tConf.m_tStatus.SetNoRecording();
    tConfData.m_tConf.m_tStatus.SetNoPlaying();
	THduPollInfo tNullInfo;
	tConfData.m_tConf.m_tStatus.SetHduPollInfo(tNullInfo);

	//清空会议自动录像属性[8/6/2013 chendaiwei]
	TConfAutoRecAttrb tNullRecAttrb;
	tConfData.m_tConf.SetAutoRecAttrb(tNullRecAttrb);

    wAliasBufLen = htons(wAliasBufLen);
    //confinfo
    cServMsg.SetMsgBody((u8*)&tConfData.m_tConf, sizeof(TConfInfo));
	
    //mt alias
    cServMsg.CatMsgBody((u8 *)&wAliasBufLen, sizeof(wAliasBufLen));
	cServMsg.CatMsgBody((u8 *)achAliasBuf, ntohs(wAliasBufLen));        

    //vmp
    if (bVmpMod)
    {
        cServMsg.CatMsgBody((u8 *)&tVmpMod, sizeof(tVmpMod));
    }

	if(tConfData.m_tConf.HasConfExInfo() && wBufferLen > 0)
	{		
		cServMsg.CatMsgBody((u8*)&tConfExData.m_byConInfoExBuf[0],wBufferLen);
	}

	cServMsg.CatMsgBody((u8*)&tConfExData.m_atSmcuCallInfo[0],sizeof(tConfExData.m_atSmcuCallInfo));

	//TODO:待删除
	for(u8 byIdx = 0; byIdx < byVmpNum; byIdx++ )
	{
		atVmpParam[byIdx].m_tVmpBaiscParam.Print();
	}

	cServMsg.CatMsgBody((u8*)&byVmpNum,sizeof(byVmpNum));
	cServMsg.CatMsgBody((u8*)&atVmpParam[0],sizeof(atVmpParam[0])*byVmpNum);

    //发消息创建会议        
    post(MAKEIID( AID_MCU_VC, CInstance::DAEMON ), MCU_CREATECONF_NPLUS, 
                    cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        
    return;
}

/*=============================================================================
函 数 名： DaemonUsrRollbackReq
功    能： 用户信息回滚
算法实现： 
全局变量： 
参    数：  const CMessage * pcMsg
           CApp* pcApp
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/30  4.0			许世林                  创建
=============================================================================*/
void CNPlusInst::DaemonUsrRollbackReq( void )
{
    //不需要作处理
    return;
}

/*=============================================================================
函 数 名： DaemonGrpRollbackReq
功    能： 用户组信息回滚
算法实现： 
全局变量： 
参    数：  const CMessage * pcMsg
           CApp* pcApp
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/30  4.0			许世林                  创建
=============================================================================*/
void CNPlusInst::DaemonGrpRollbackReq( void )
{
    //不需要作处理
    return;
}

/*=============================================================================
函 数 名： DaemonConnectNPlusMcu
功    能： 尝试向备份mcu建链
算法实现： 
全局变量： 
参    数： CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/22  4.0			许世林                  创建
=============================================================================*/
void CNPlusInst::DaemonConnectNPlusMcuTimeOut( void )
{
    if ( !OspIsValidTcpNode(m_dwMcuNode) )
    {
        TLocalInfo tLocalInfo;
        if (SUCCESS_AGENT == g_cMcuAgent.GetLocalInfo(&tLocalInfo))
        {
            m_dwMcuNode = OspConnectTcpNode( htonl(tLocalInfo.GetNPlusMcuIp()), MCU_LISTEN_PORT );
            if ( !OspIsValidTcpNode(m_dwMcuNode) )
            {
                m_dwMcuNode = INVALID_NODE;
			    LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"OspConnectTcpNode MCU(0x%x) Failed!\n", tLocalInfo.GetNPlusMcuIp());
			    SetTimer( MCU_NPLUS_CONNECT_TIMER, NPLUS_CONNECTMCU_TIMEOUT );
			    return;
            }
            OspNodeDiscCBRegQ(m_dwMcuNode, GetAppID(), GetInsID());
        }
        else
        {
            LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonConnectNPlusMcu] mcu agent get localinfo failed.\n");
            return;
        }
    }

    //发起注册请求
    SetTimer( MCU_NPLUS_REG_TIMER, 10 );
    return;
}

/*=============================================================================
函 数 名： DaemonRegNPlusMcu
功    能： 向备份mcu发起注册请求
算法实现： 
全局变量： 
参    数： CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/22  4.0			许世林                  创建
=============================================================================*/
void CNPlusInst::DaemonRegNPlusMcuTimeOut()
{
    if (CurState() != STATE_IDLE)
    {
        return;
    }

    TNPlusMcuRegInfo tRegInfo;
    tRegInfo.SetMcuIpAddr( ntohl(g_cMcuAgent.GetMpcIp()));
    tRegInfo.SetMcuEqpCap( g_cNPlusApp.GetMcuEqpCap() );
    tRegInfo.SetMcuType( g_cNPlusApp.GetMcuType() );

    TNPlusMcuInfoEx tRegInfoEx;
    //[5/7/2013 liaokang] 增加编码方式
#ifdef _UTF8
    tRegInfoEx.SetMcuEncodingForm(emenCoding_Utf8);
#else
    tRegInfoEx.SetMcuEncodingForm(emenCoding_GBK); 
#endif    
    tRegInfoEx.SetCurMSState(g_cMSSsnApp.GetCurMSState());
    // 配置了双链
    if( g_cMSSsnApp.IsDoubleLink() )
    {        
        tRegInfoEx.SetAnotherMpcIp(g_cMSSsnApp.GetAnotherMcuIP());
    }
    
    u8 abyBuffer[NPLUS_PACK_EXINFO_BUF_LEN] = {0};
	u16 wLen = 0;
    g_cNPlusApp.PackNplusExInfo(g_cNPlusApp.GetMcuEqpCapEx(),abyBuffer,wLen,&tRegInfoEx);
    CServMsg cMsg;
    cMsg.SetEventId(MCU_NPLUS_REG_REQ);
	cMsg.SetMsgBody((u8*)&tRegInfo,sizeof(tRegInfo));
	cMsg.CatMsgBody((u8*)&abyBuffer[0],wLen);
    SndMsg2NplusMcu(cMsg,MAKEIID(AID_MCU_NPLUSMANAGER, CInstance::DAEMON),m_dwMcuNode);
    SetTimer( MCU_NPLUS_REG_TIMER, NPLUS_REGMCU_TIMEOUT );
    return;
}

/*=============================================================================
函 数 名： DaemonRegNPlusMcuReq
功    能： 主mcu发给备份mcu的注册消息处理函数
算法实现： 
全局变量： 
参    数： CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/22  4.0			许世林                  创建
=============================================================================*/
void CNPlusInst::DaemonRegNPlusMcuReq( const CMessage * pcMsg, CApp* pcApp )
{
    //本MCU不支持N+1备份 或 N+1备份下本端主用，拒绝其他MCU连上来，以免 假注册成功 或 主主假锁[12/28/2006-zbq]
    if ( MCU_NPLUS_IDLE == g_cNPlusApp.GetLocalNPlusState() ||
        MCU_NPLUS_MASTER_IDLE == g_cNPlusApp.GetLocalNPlusState() ||
        MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState() )
    {
        DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_CONNECT_MASTER );
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    if (sizeof(TNPlusMcuRegInfo) > cServMsg.GetMsgBodyLen())
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonRegNPlusMcuReq] cServMsg.GetMsgBodyLen()=%d, sizeof(TNPlusMcuRegInfo)=%d !\n",
                   cServMsg.GetMsgBodyLen(), sizeof(TNPlusMcuRegInfo));
        DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_MSGLEN_ERR );
        return;
    }

    TNPlusMcuRegInfo *ptRegInfo = (TNPlusMcuRegInfo *)(cServMsg.GetMsgBody());

    //检查备份环境，不允许8000B备份8000 [12/20/2006-zbq]
    if ( ( MCU_TYPE_KDV8000B == g_cNPlusApp.GetMcuType() && 
           MCU_TYPE_KDV8000  == ptRegInfo->GetMcuType() ) ||
         // guzh [3/8/2007] 也不允许8000C备份8000/8000B
         ( MCU_TYPE_KDV8000C == g_cNPlusApp.GetMcuType() &&
           ( MCU_TYPE_KDV8000 == ptRegInfo->GetMcuType() ||
             MCU_TYPE_KDV8000B == ptRegInfo->GetMcuType() ) ) ||
         // guzh [3/8/2007] 不允许8000/8000B 备份 8000C
         ( MCU_TYPE_KDV8000C == ptRegInfo->GetMcuType() &&
           ( MCU_TYPE_KDV8000 == g_cNPlusApp.GetMcuType() ||
             MCU_TYPE_KDV8000B == g_cNPlusApp.GetMcuType() ) ) )
    {
        DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_ENV_UNMATCHED );
        return;
    }

    //检查外设能力
	TNPlusEqpCapEx tRemoteCapEx;
    TNPlusMcuInfoEx tRemoteMcuInfoEx;
    tRemoteMcuInfoEx.SetCurMSState(MCU_MSSTATE_ACTIVE);  //默认对端为主用
    tRemoteMcuInfoEx.SetMcuEncodingForm(emenCoding_GBK); //默认对端为GBK编码
	if(cServMsg.GetMsgBodyLen() > sizeof(TNPlusMcuRegInfo))
	{
		BOOL32 bUnkownInfo = FALSE;
		u16 wUnpackLen = 0;
        g_cNPlusApp.UnPackNplusExInfo(tRemoteCapEx,cServMsg.GetMsgBody() + sizeof(TNPlusMcuRegInfo),wUnpackLen,bUnkownInfo,&tRemoteMcuInfoEx);
		if(bUnkownInfo)
		{
			DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_CAP_ERR );
			return;
		}
	}

    // 不支持gbk编码的mcu备份utf8编码的mcu
#ifndef _UTF8
    if( emenCoding_Utf8 == tRemoteMcuInfoEx.GetMcuEncodingForm() )
    {         
        DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_CHECKENCODING_ERR );
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonRegNPlusMcuReq] Do not support GBK backup Utf8, Nack!\n");
        return;
    }
#endif

    TNPlusEqpCap tLocalCap = g_cNPlusApp.GetMcuEqpCap();
	TNPlusEqpCapEx tLocalCapEx = g_cNPlusApp.GetMcuEqpCapEx();	
	TNPlusEqpCap tRemoteCap = ptRegInfo->GetMcuEqpCap();	
	if(IsLocalEqpCapLowerThanRemote(tLocalCap,tLocalCapEx,tRemoteCap,tRemoteCapEx))
	{
        DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_CAP_ERR );
        return;
	}

    BOOL32 bFind = FALSE;
    // 先查找是否为switch的inst
    CNPlusInst *pcInst = NULL;
    u8 byInsId = g_cNPlusApp.GetMcuSwitchedInsId();
    if( 0 != byInsId )
    {
        pcInst = (CNPlusInst *)pcApp->GetInstance(byInsId);
        if (NULL != pcInst)
        { 

            if ( (pcInst->GetMcuIpAddr() == ptRegInfo->GetMcuIpAddr() && pcInst->GetMcuIpAddrB() == tRemoteMcuInfoEx.GetAnotherMpcIp() ) || 
                (pcInst->GetMcuIpAddrB() == ptRegInfo->GetMcuIpAddr() && pcInst->GetMcuIpAddr() == tRemoteMcuInfoEx.GetAnotherMpcIp() ) )
            {
                if( MCU_MSSTATE_STANDBY == tRemoteMcuInfoEx.GetCurMSState() )
                {
                    // 备用板不能触发回滚,先断链
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonRegNPlusMcuReq] standby board must register after active board in switch state, disconnect node.%d!\n",pcMsg->srcnode);
                    OspDisconnectTcpNode(pcMsg->srcnode);
                    return;
                } 
                else
                {
                    bFind = TRUE;
                }
            } 

        }
    }

    // 再查找是否为Normal的inst
    if( FALSE == bFind )
    {
        for( byInsId = 1; byInsId <= MAXNUM_NPLUS_MCU; byInsId++ )
        {
            pcInst = (CNPlusInst *)pcApp->GetInstance(byInsId);
            if (NULL == pcInst || STATE_NORMAL != pcInst->CurState())
            {
                continue;
            }            

            if ( (pcInst->GetMcuIpAddr() == ptRegInfo->GetMcuIpAddr() && pcInst->GetMcuIpAddrB() == tRemoteMcuInfoEx.GetAnotherMpcIp() ) || 
                 (pcInst->GetMcuIpAddrB() == ptRegInfo->GetMcuIpAddr() && pcInst->GetMcuIpAddr() == tRemoteMcuInfoEx.GetAnotherMpcIp() ) )
            {
                if( pcInst->GetMcuEncodingForm() != tRemoteMcuInfoEx.GetMcuEncodingForm() )
                {
                    // 前后编码方式不一致，NACK
                    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonRegNPlusMcuReq] N mode Mcu Encoding Form is Inconsistent, Nack!\n");
                    DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_CHECKENCODING_ERR );
                    return;
                }

                bFind = TRUE;
                break;
            }
        } 
    }

    // 分配Idle的inst
    if( FALSE == bFind )
    {
        for( byInsId = 1; byInsId <= MAXNUM_NPLUS_MCU; byInsId++ )
        {
            pcInst = (CNPlusInst *)pcApp->GetInstance(byInsId);
            if (NULL != pcInst && STATE_IDLE == pcInst->CurState())
            {
                bFind = TRUE;
                break;
            }
        } 
    }

    if( TRUE == bFind )
    {
        cServMsg.CatMsgBody((u8 *)&pcMsg->srcid, sizeof(pcMsg->srcid));
        cServMsg.CatMsgBody((u8 *)&pcMsg->srcnode, sizeof(pcMsg->srcnode));
        post(MAKEIID(AID_MCU_NPLUSMANAGER, byInsId), pcMsg->event, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[DaemonRegNPlusMcuReq] reg to inst %d!\n", byInsId);        
    }
    else
    {
        DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_OVERLOAD );
        LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[DaemonRegNPlusMcuReq] no enough idle inst !\n");
    }

    return;
}

/*=============================================================================
函 数 名： DaemonRegNack
功    能： 主mcu的注册拒绝统一处理
算法实现： 
全局变量： 
参    数： CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/12/21  4.0			张宝卿                  创建
=============================================================================*/
void CNPlusInst::DaemonRegNack( const CMessage * pcMsg, u8 byReason )
{
    CServMsg cServMsg;
    cServMsg.SetEventId( pcMsg->event + 2 );
    cServMsg.SetErrorCode( byReason );
    SndMsg2NplusMcu(cServMsg,pcMsg->srcid,pcMsg->srcnode);
    OspDisconnectTcpNode(pcMsg->srcnode);

    LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonRegNPlusMcuReq] NPlus reg failed due to reason<%d>, disconnect!\n", byReason);
    return;
}

/*=============================================================================
函 数 名： DaemonRegNPlusMcuRsp
功    能： 备份mcu的注册响应
算法实现： 
全局变量： 
参    数： CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/22  4.0			许世林                  创建
=============================================================================*/
void CNPlusInst::DaemonRegNPlusMcuRsp( const CMessage * pcMsg )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );

    if ( MCU_NPLUS_REG_ACK == pcMsg->event )
    {
        //注册成功后的回滚，先无条件清理本地所有会议。防止该会议在备端已经被结
        //束，但在回滚后本地 仍然存在 的假相发生。只在主端网络环境差时适用。[01/09/2006-zbq]
        u8 byRollBack = *(u8*)cServMsg.GetMsgBody();
        if ( 1 == byRollBack )
        {
            OspPost(MAKEIID(AID_MCU_VC, CInstance::EACH), MCS_MCU_RELEASECONF_REQ,
                           NULL, 0, 0, MAKEIID(AID_MCU_NPLUSMANAGER, GetInsID()));    
        }

		//zjj20130918 主机使用备机断链检测参数，以达到断链同时检测，防止主机备机断链检测状态不一致情况
		if( cServMsg.GetMsgBodyLen() > sizeof(u8) )
		{		
			u16 wRtdTime = *(u16*)(cServMsg.GetMsgBody() + sizeof(u8) );
			u16 wRtdNum  = *(u16*)(cServMsg.GetMsgBody() + + sizeof(u8) + sizeof(u16) );
			wRtdTime     = ntohs(wRtdTime);
			wRtdNum		 = ntohs(wRtdNum);
			OspSetHBParam( m_dwMcuNode, wRtdTime, (u8)wRtdNum );
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "[DaemonRegNPlusMcuRsp] disconnect set %d.%d\n",
					wRtdTime,wRtdNum );
		}

        KillTimer( MCU_NPLUS_REG_TIMER );
        //保存此id，后面即可以直接与备份mcu的相应实例通信
        m_dwMcuIId = pcMsg->srcid;

        g_cNPlusApp.SetLocalNPlusState(MCU_NPLUS_MASTER_CONNECTED);

        NEXTSTATE( STATE_NORMAL);

        //同步会议所有信息
        OspPost(MAKEIID(AID_MCU_VC, CInstance::EACH), NPLUS_VC_DATAUPDATE_NOTIF, NULL, 0, 0,
                MAKEIID(AID_MCU_NPLUSMANAGER, CInstance::DAEMON));

        //同步用户组和用户信息
        OspPost(MAKEIID(AID_MCU_MCSSN, CInstance::DAEMON), NPLUS_VC_DATAUPDATE_NOTIF, NULL, 0, 0,
                MAKEIID(AID_MCU_NPLUSMANAGER, CInstance::DAEMON));

        //同步RAS信息，以使得备板倒换后再次注册前的注销操作成功 [12/18/2006-zbq]
        if ( 0 != g_cMcuAgent.GetGkIpAddr() )
        {
            // 如果本主MCU是晚于备份MCU启动，则很可能本主MCU的RAS信息还有报上来，等待主适配RCF后再作更新
            if ( 0 != g_cMcuVcApp.GetH323EPIDAlias()->GetIDlength() )
            {
                TRASInfo tRASInfo;
                tRASInfo.SetEPID( g_cMcuVcApp.GetH323EPIDAlias() );
                tRASInfo.SetGKID( g_cMcuVcApp.GetH323GKIDAlias() );
                tRASInfo.SetGKIp( ntohl(g_cMcuAgent.GetGkIpAddr()) );
                tRASInfo.SetRRQIp( g_cMcuAgent.GetRRQMtadpIp() );

                // zbq [03/15/2007] 同步RAS信息增加主MCU的E164一项
                TLocalInfo tLocalInfo;
                g_cMcuAgent.GetLocalInfo( &tLocalInfo );
                tRASInfo.SetMcuE164( tLocalInfo.GetE164Num() );
                
                CServMsg cSendServMsg;
                cSendServMsg.SetMsgBody( (u8*)&tRASInfo, sizeof(tRASInfo) );
                cSendServMsg.SetEventId( MCU_NPLUS_RASINFOUPDATE_REQ );

                SndMsg2NplusMcu(cSendServMsg);
            }
            else
            {
                LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[DaemonRegNPlusMcuRsp] Rasinfo invalid, the RRQ mtadp will notify the backup mcu in NO TIME !\n");
            }
        }
    }
    else
    {
        //上报主业务主备注册失败的原因 [12/18/2006-zbq]
        OspPost(MAKEIID(AID_MCU_VC, CInstance::DAEMON), pcMsg->event, NULL, 0);
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonRegNPlusMcuRsp] receive register nack due to Reason.%d.\n", cServMsg.GetErrorCode());
    }
    return;
}

/*=============================================================================
函 数 名： DaemonVcNPlusMsgNotif
功    能： 会议数据通知备份mcu
算法实现： 
全局变量： 
参    数：  const CMessage * pcMsg
           CApp* pcApp
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/22  4.0			许世林                  创建
=============================================================================*/
void CNPlusInst::DaemonVcNPlusMsgNotif( const CMessage * pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    if (CurState() != STATE_NORMAL)
    {
//        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[DaemonVcNPlusMsgNotif] invalid ins state(%d), ignore it(%s)\n", 
//            CurState(), OspEventDesc(cServMsg.GetEventId()));
        return;
    }

    SndMsg2NplusMcu(cServMsg);
    return;
}

/*=============================================================================
函 数 名： DaemonRtdReq
功    能： 主mcu rtd请求处理函数
算法实现： 
全局变量： 
参    数：  const CMessage * pcMsg
           CApp* pcApp
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/12/5  4.0			许世林                  创建
=============================================================================*/
void CNPlusInst::DaemonRtdReq( void )
{
    LogPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"[DaemonRtdReq] receive rtd request.\n");
    PostReplyBack(MCU_NPLUS_RTD_RSP);
    return;
}

/*=============================================================================
函 数 名： SetConfData
功    能： 保存会议所有信息
算法实现： 
全局变量： 
参    数： TNPlusConfData *ptConfData
返 回 值： BOOL32  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/24  4.0			许世林                  创建
=============================================================================*/
BOOL32  CNPlusInst::SetConfData( TNPlusConfData *ptConfData )
{
    if (NULL == ptConfData)
    {
        return FALSE;
    }

    u8 byIdx = GetConfIdxByConfId(ptConfData->m_tConf.GetConfId());
    if (MAXNUM_ONGO_CONF == byIdx)//modify
    {
        byIdx = GetEmptyConfIdx();//new
    }    

    if (MAXNUM_ONGO_CONF == byIdx)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::SetConfData] get conf index failed by cconfid:");
        ptConfData->m_tConf.GetConfId().Print();
        return FALSE;
    }

    memcpy(&m_atConfData[byIdx], ptConfData, sizeof(TNPlusConfData));
    return TRUE;
}

/*=============================================================================
函 数 名： SetConfExData
功    能： 保存会议扩展信息
算法实现： 
全局变量： 
参    数： [IN]CConfId cConfId
		   [IN]u8 *pbybuff
		   [IN]u16 wBufLen
返 回 值： BOOL32  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2012/1/10   4.0			陈代伟                  创建
=============================================================================*/
BOOL32  CNPlusInst::SetConfExData(CConfId cConfId, u8 *pbybuff,u16 wBufLen )
{
    if (NULL == pbybuff || wBufLen> CONFINFO_EX_BUFFER_LENGTH)
    {
        return FALSE;
    }

    u8 byIdx = GetConfIdxByConfId(cConfId);
    if (MAXNUM_ONGO_CONF == byIdx)
    {
        return FALSE;
    }
    else
	{
		memcpy((u8*)&m_atConfExData[byIdx],pbybuff,wBufLen);
		return TRUE;
	}
}

/*=============================================================================
函 数 名： SetConfInfo
功    能： 保存会议信息
算法实现： 
全局变量： 
参    数：  TConfInfo *ptConfInfo
返 回 值： BOOL32  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/24  4.0			许世林                  创建
=============================================================================*/
BOOL32  CNPlusInst::SetConfInfo( TConfInfo *ptConfInfo,BOOL32 bStart )
{
    if (NULL == ptConfInfo)
    {
        return FALSE;
    }

    u8 byIdx = GetConfIdxByConfId(ptConfInfo->GetConfId());
    if (MAXNUM_ONGO_CONF == byIdx)
    {
        byIdx = GetEmptyConfIdx();//new     
    }

    if (MAXNUM_ONGO_CONF == byIdx)
    {        
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::SetConfInfo] get conf index failed by cconfid:");
        ptConfInfo->GetConfId().Print();
        return FALSE;
    }

    if(bStart)
    {
        TConfInfo *ptConf = &m_atConfData[byIdx].m_tConf;
        TMtAlias tChair = ptConf->GetChairAlias();
        TMtAlias tSpeaker = ptConf->GetSpeakerAlias();
        memcpy(ptConf, ptConfInfo, sizeof(TConfInfo));
        ptConf->SetChairAlias(tChair);
        ptConf->SetSpeakerAlias(tSpeaker);
    }
    else
    {
		//1.清空多vmp参数
		ClearVmpParamByConfIdx(byIdx);
		
		//2.清空ex参数
		m_atConfExData[byIdx].Clear();

        m_atConfData[byIdx].Clear();
    }
    
    return TRUE;
}

/*=============================================================================
函 数 名： SetConfAutoMix
功    能： 设置会议是否支持智能混音
算法实现： 
全局变量： 
参    数： CConfId  tConfId会议号结构  BOOL32 bStart TRUE表示开启智能混音 FALSE表示
		   停止智能混音 
返 回 值： BOOL32  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010/09/26  			陈代伟                  创建
=============================================================================*/
BOOL32  CNPlusInst::SetConfAutoMix( CConfId &cConfId, BOOL32 bStart )
{
    u8 byIdx = GetConfIdxByConfId(cConfId);

    if (MAXNUM_ONGO_CONF == byIdx)
    {        
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::SetConfAutoMix] get conf index failed by cconfid:");
        cConfId.Print();
        return FALSE;
    }

	m_atConfData[byIdx].m_tConf.m_tStatus.SetAutoMixing(bStart);
    
    return TRUE;
}
/*=============================================================================
函 数 名： SetConfMtInfo
功    能： 保存会议终端信息
算法实现： 
全局变量： 
参    数：  CConfId *pcConfId
           TMtInfo *ptMtInfo
           u8 byMtNum
返 回 值： BOOL32  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/24  4.0			许世林                  创建
=============================================================================*/
BOOL32  CNPlusInst::SetConfMtInfo( CConfId &cConfId, TMtInfo *ptMtInfo, u8 byMtNum )
{
    if (NULL == ptMtInfo)
    {
        return FALSE;
    }

    u8 byIdx = GetConfIdxByConfId(cConfId);
    if (MAXNUM_ONGO_CONF == byIdx)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::SetConfMtInfo] get conf index failed by cconfid:");
        cConfId.Print();
        return FALSE;
    }

    memcpy(m_atConfData[byIdx].m_atMtInConf, ptMtInfo, byMtNum*sizeof(TMtInfo));
    m_atConfData[byIdx].m_byMtNum = byMtNum;
    return TRUE;
}

/*=============================================================================
函 数 名： SetConfSmcuCallInfo
功    能： 保存下级MCU会议呼叫信息
算法实现： 
全局变量： 
参    数：  CConfId *pcConfId
           TSmcuCallnfo *ptSmcuCallInfo
           u8 bySmcuNum
返 回 值： BOOL32  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2012/11/16  4.7			chendaiwei            create
=============================================================================*/
BOOL32  CNPlusInst::SetConfSmcuCallInfo( CConfId &cConfId, TSmcuCallnfo *ptSmcuCallInfo, u8 bySmcuNum )
{
    if (NULL == ptSmcuCallInfo)
    {
        return FALSE;
    }

    u8 byIdx = GetConfIdxByConfId(cConfId);
    if (MAXNUM_ONGO_CONF == byIdx)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::SetConfSmcuCallInfo] get conf index failed by cconfid:");
        cConfId.Print();
        return FALSE;
    }

	memset((u8*)&m_atConfExData[byIdx].m_atSmcuCallInfo[0],0,sizeof(m_atConfExData[byIdx].m_atSmcuCallInfo));
    memcpy(m_atConfExData[byIdx].m_atSmcuCallInfo, ptSmcuCallInfo, bySmcuNum*sizeof(TSmcuCallnfo));
    return TRUE;
}

BOOL32 CNPlusInst::UnPackVmpBufToVmpParam( u8 *ptNplusVmpInfoBuf, TNPlusVmpParam *ptVmpParam, u8 &byVmpNum, u16 &wPackbufLen)
{
	if( ptNplusVmpInfoBuf == NULL || ptVmpParam == NULL )
	{
		return FALSE;
	}

	byVmpNum = *ptNplusVmpInfoBuf;
	wPackbufLen = 0;

	ptNplusVmpInfoBuf++;
	wPackbufLen++;
	
	for( u8 byVmpNumIdx = 0; byVmpNumIdx < byVmpNum; byVmpNumIdx++ )
	{
		memcpy(&ptVmpParam[byVmpNumIdx].m_tVmpBaiscParam,(TVmpBasicParam*)ptNplusVmpInfoBuf,sizeof(TVmpBasicParam));
		ptNplusVmpInfoBuf += sizeof(TVmpBasicParam);
		wPackbufLen += sizeof(TVmpBasicParam);
				
		u8 byChnlNum = *ptNplusVmpInfoBuf;
		ptNplusVmpInfoBuf++;
		wPackbufLen++;
		
		memcpy(&ptVmpParam[byVmpNumIdx].m_tVmpMemer.m_tVmpChnnlInfo[0],ptNplusVmpInfoBuf,sizeof(TNPlusVmpChnlMember)*byChnlNum);
		ptNplusVmpInfoBuf += sizeof(TNPlusVmpChnlMember)*byChnlNum;
		wPackbufLen += sizeof(TNPlusVmpChnlMember)*byChnlNum;
	}
	
	return TRUE;
}

void CNPlusInst::ClearVmpParamByConfIdx ( u8 byConIdx )
{
	for( u8 byEqpidx = 0; byEqpidx < MAXNUM_PERIEQP; byEqpidx++ )
	{
		if(m_atNplusParam[byEqpidx].m_byConfIdx == byConIdx)
		{
			m_atNplusParam[byEqpidx].Clear();
		}
	}
}

/*=============================================================================
函 数 名： SetConfMultiVmpParam
功    能： 保存会议多vmp参数信息
算法实现： 
全局变量： 
参    数：  CConfId *pcConfId
           u8 *ptNplusVmpInfoBuf多vmp参数内存首地址
		   u16 &wPackBufLen buffer长度
返 回 值： BOOL32  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2013/03/16  4.7			chendaiwei            create
=============================================================================*/
BOOL32  CNPlusInst::SetConfMultiVmpParam( CConfId &cConfId, u8 *ptNplusVmpInfoBuf, u16 &wPackbufLen)
{
    if (NULL == ptNplusVmpInfoBuf)
    {
        return FALSE;
    }

    u8 byIdx = GetConfIdxByConfId(cConfId);
    if (MAXNUM_ONGO_CONF == byIdx)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::SetConfVmpParam] get conf index failed by cconfid:");
        cConfId.Print();
        return FALSE;
    }

	TNPlusVmpParam atVmpParam[MAXNUM_PERIEQP];
	wPackbufLen = 0;
	u8 byVmpNum = 0;
	u8 byVmpNumIdx = 0;

	if(!UnPackVmpBufToVmpParam(ptNplusVmpInfoBuf,atVmpParam,byVmpNum,wPackbufLen))
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::UnPackVmpBufToVmpParam] failed!\n");

		return FALSE;
	}

	ClearVmpParamByConfIdx(byIdx);

	for( u8 byEqpIdx = 0; byEqpIdx < MAXNUM_PERIEQP; byEqpIdx++ )
	{
		if(m_atNplusParam[byEqpIdx].IsNull() && byVmpNumIdx < byVmpNum)
		{
			m_atNplusParam[byEqpIdx] = atVmpParam[byVmpNumIdx];
			m_atNplusParam[byEqpIdx].m_byConfIdx = byIdx;
			byVmpNumIdx++;
		}
	}
	
    return TRUE;
}


BOOL32 CNPlusInst::GetConfMultiVmpParam( u8 byConfIdx, TNPlusVmpParam *ptParam, u8 &byVmpNum)
{
    if (NULL == ptParam)
    {
        return FALSE;
    }
	
    if (MAXNUM_ONGO_CONF == byConfIdx)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::GetConfMultiVmpParam] conf index %d invalid\n",byConfIdx);

        return FALSE;
    }

	byVmpNum = 0;

	for( u8 byEqpidx = 0; byEqpidx < MAXNUM_PERIEQP; byEqpidx++ )
	{
		if(m_atNplusParam[byEqpidx].m_byConfIdx == byConfIdx)
		{
			ptParam[byVmpNum] = m_atNplusParam[byEqpidx];
			byVmpNum++;
		}
	}

	return TRUE;
}

/*=============================================================================
函 数 名： SetConfVmpInfo
功    能： 保存会议vmp信息
算法实现： 
全局变量： 
参    数：  CConfId *pcConfId
           TMtInfo *ptMtInfo
           u8 byMtNum
返 回 值： BOOL32  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/24  4.0			许世林                  创建
=============================================================================*/
BOOL32  CNPlusInst::SetConfVmpInfo( CConfId &cConfId, TNPlusVmpInfo *ptVmpInfo, TVMPParam *ptParam )
{
    if (NULL == ptVmpInfo || NULL == ptParam)
    {
        return FALSE;
    }

    u8 byIdx = GetConfIdxByConfId(cConfId);
    if (MAXNUM_ONGO_CONF == byIdx)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::SetConfVmpInfo] get conf index failed by cconfid:");
        cConfId.Print();
        return FALSE;
    }   

    memcpy(&m_atConfData[byIdx].m_tVmpInfo, ptVmpInfo, sizeof(TNPlusVmpInfo));

    m_atConfData[byIdx].m_tConf.m_tStatus.SetVmpParam(*ptParam);
    return TRUE;
}

/*=============================================================================
函 数 名： SetChairman
功    能： 保存会议主席信息
算法实现： 
全局变量： 
参    数：  CConfId *pcConfId
           TMtInfo *ptMtAlias
返 回 值： BOOL32  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/24  4.0			许世林                  创建
=============================================================================*/
BOOL32  CNPlusInst::SetChairman( CConfId &cConfId, TMtAlias *ptMtAlias )
{
    if (NULL == ptMtAlias)
    {
        return FALSE;
    }

    u8 byIdx = GetConfIdxByConfId(cConfId);
    if (MAXNUM_ONGO_CONF == byIdx)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::SetChairman] get conf index failed by cconfid:");
        cConfId.Print();
        return FALSE;
    }

    m_atConfData[byIdx].m_tConf.SetChairAlias(*ptMtAlias);
    return TRUE;
}

/*=============================================================================
函 数 名： SetSpeaker
功    能： 保存会议发言人信息
算法实现： 
全局变量： 
参    数：  CConfId *pcConfId
           TMtInfo *ptMtAlias
返 回 值： BOOL32  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/24  4.0			许世林                  创建
=============================================================================*/
BOOL32  CNPlusInst::SetSpeaker( CConfId &cConfId, TMtAlias *ptMtAlias )
{
    if (NULL == ptMtAlias)
    {
        return FALSE;
    }

    u8 byIdx = GetConfIdxByConfId(cConfId);
    if (MAXNUM_ONGO_CONF == byIdx)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::SetSpeaker] get conf index failed by cconfid:");
        cConfId.Print();
        return FALSE;
    }

    m_atConfData[byIdx].m_tConf.SetSpeakerAlias(*ptMtAlias);
    return TRUE;
}

/*=============================================================================
函 数 名： SetUsrGrpInfo
功    能： 保存用户组信息
算法实现： 
全局变量： 
参    数： CUsrGrpsInfo *pcUsrGrpInfo
返 回 值： BOOL32  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/24  4.0			许世林                  创建
=============================================================================*/
BOOL32  CNPlusInst::SetUsrGrpInfo( CUsrGrpsInfo *pcUsrGrpInfo )
{
    if (NULL == pcUsrGrpInfo)
    {
        return FALSE;
    }
    memcpy(&m_cUsrGrpsInfo, pcUsrGrpInfo, sizeof(CUsrGrpsInfo));
    return TRUE;
}

/*=============================================================================
函 数 名： SetUsrInfo
功    能： 保存用户信息
算法实现： 
全局变量： 
参    数：  CExUsrInfo *pcUsrInfo
           u8 byNum
返 回 值： BOOL32  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/24  4.0			许世林                  创建
=============================================================================*/
BOOL32  CNPlusInst::SetUsrInfo( CExUsrInfo *pcUsrInfo, u8 byNum, BOOL32 bAdd )
{
    if (NULL == pcUsrInfo)
    {
        return FALSE;
    }
    
    if (bAdd)
    {
        if (byNum + m_byUsrNum > MAXNUM_USRNUM)
        {
            LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[SetUsrInfo] user packet error.\n");
            return FALSE;
        }
        memcpy(&m_acUsrInfo[m_byUsrNum], pcUsrInfo, sizeof(CExUsrInfo)*byNum);
        m_byUsrNum += byNum;
    }
    else
    {
        if (byNum > MAXNUM_USRNUM)
        {
            LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[SetUsrInfo] user packet error.\n");
            return FALSE;
        }
        memcpy(m_acUsrInfo, pcUsrInfo, sizeof(CExUsrInfo)*byNum);
        m_byUsrNum = byNum;
    }    
    return TRUE;
}

/*=============================================================================
函 数 名： GetConfIdxByConfId
功    能： 根据CConfId获取会议信息索引(0 - MAXNUM_ONGO_CONF-1)
算法实现： 
全局变量： 
参    数： CConfId *pcConfId
返 回 值： u8  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/24  4.0			许世林                  创建
=============================================================================*/
u8 CNPlusInst::GetConfIdxByConfId( const CConfId &cConfId )
{    
    if (cConfId.IsNull())
    {
        return MAXNUM_ONGO_CONF;
    }

    for (u8 byIdx = 0; byIdx < MAXNUM_ONGO_CONF; byIdx++)
    {
        if (m_atConfData[byIdx].m_tConf.GetConfId() == cConfId)
        {
            return byIdx;
        }
    }
    
    return MAXNUM_ONGO_CONF;
}

/*=============================================================================
函 数 名： GetEmptyConfIdx
功    能： 获取会议信息索引
算法实现： 
全局变量： 
参    数： void
返 回 值： u8  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/12/5  4.0			许世林                  创建
=============================================================================*/
u8  CNPlusInst::GetEmptyConfIdx( void )
{
    for (u8 byIdx = 0; byIdx < MAXNUM_ONGO_CONF; byIdx++)
    {
        if (m_atConfData[byIdx].m_tConf.GetConfId().IsNull())
        {
            return byIdx;
        }
    }

    return MAXNUM_ONGO_CONF;
}

/*=============================================================================
    函 数 名： UnRegAllInfoOfSwitchedMcu
    功    能： 如果该备份MCU配置了和主MCU相同的GK，则该备份MCU的主适配板将伪装
               成主MCU的主适配板 对 主MCU上所有注册到 GK上的信息 发起注销。
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/12/18  4.0			张宝卿                  创建
=============================================================================*/
void CNPlusInst::UnRegAllInfoOfSwitchedMcu()
{
    TRASInfo *ptRASInfo = GetRASInfo();
	if(ptRASInfo != NULL)
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "[UnRegAllInfoOfSwitchedMcu] Get RasInfo. Ras EPID Length[%u], EPId Alias[%s], MCUE164[%s]!\n",ptRASInfo->GetEPID()->GetIDlength(),ptRASInfo->GetEPID()->GetIDAlias(),ptRASInfo->GetMcuE164());               
	}

    if (ptRASInfo == NULL || 0 == ptRASInfo->GetEPID()->GetIDlength() ) 
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[UnRegAllInfoOfSwitchedMcu] the EPID of MASTER mcu hasn't got yet !\n" );
        return;
    }
    CServMsg cServMsg;
    cServMsg.SetConfIdx( 0 );
    cServMsg.SetEventId( MCU_MT_UNREGGK_NPLUS_CMD );
    cServMsg.SetMsgBody( (u8*)ptRASInfo, sizeof(TRASInfo) );

    g_cMtAdpSsnApp.SendMsgToMtAdpSsn( g_cMcuVcApp.GetRegGKDriId(), 
                                      MCU_MT_UNREGGK_NPLUS_CMD, cServMsg );

    return;
}

/*=============================================================================
    函 数 名： StopConfChargeOfSwitchedMcu
    功    能： 如果该备份MCU配置了和主MCU相同的GK，则该备份MCU的主适配板将伪装
               成主MCU的主适配板 对 主MCU上所有在GK上计费的会议进行取消计费操作，以发起新的计费
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/12/25  4.0			张宝卿                  创建
=============================================================================*/
void CNPlusInst::StopConfChargeOfSwitchedMcu()
{
    for( u8 byConfIdx = 0; byConfIdx < MAXNUM_ONGO_CONF; byConfIdx  ++ )
    {
        if ( !m_atConfData[byConfIdx].IsNull() &&
             m_atConfData[byConfIdx].m_tConf.IsSupportGkCharge() )
        {
            TConfChargeInfo tChargeInfo;
            tChargeInfo.SetGKUsrName(m_atConfData[byConfIdx].m_tConf.GetConfAttrbEx().GetGKUsrName());
            tChargeInfo.SetGKPwd(m_atConfData[byConfIdx].m_tConf.GetConfAttrbEx().GetGKPwd());

            CServMsg cServMsg;
            cServMsg.SetEventId( MCU_MT_CONF_STOPCHARGE_CMD );
            cServMsg.SetMsgBody( (u8*)&tChargeInfo, sizeof(TConfChargeInfo) );
            cServMsg.CatMsgBody( (u8*)&m_atConfData[byConfIdx].m_tSsnId, sizeof(TAcctSessionId) );
            
            g_cMtAdpSsnApp.SendMsgToMtAdpSsn( g_cMcuVcApp.GetRegGKDriId(), 
                                              MCU_MT_CONF_STOPCHARGE_CMD, cServMsg );
        }
    }
}

/*=============================================================================
    函 数 名： IsLocalEqpCapLowerThanRemote
    功    能： 比较本地外设能力是否小于远端外设能力集
    算法实现： 
    全局变量： 
    参    数： [IN]TNPlusEqpCap &tLocalCap本地外设能力集
               [IN] TNPlusEqpCapEx &tlocalCapEx本地外设扩展能力集
               [IN]TNPlusEqpCapEx &tRemoteCap远端外设能力集
			   [IN]TNPlusEqpCapEx &tRemoteCapEx远端外设扩展能力集
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2011/12/29  4.0			chendaiwei                  创建
=============================================================================*/
BOOL32 CNPlusInst::IsLocalEqpCapLowerThanRemote(TNPlusEqpCap &tLocalCap, TNPlusEqpCapEx &tlocalCapEx, TNPlusEqpCap &tRemoteCap, TNPlusEqpCapEx &tRemoteCapEx)
{	
	//1.基本能力比较
	if( tLocalCap < tRemoteCap)
	{
		return TRUE;
	}

	//2.扩展能力比较
	if(tlocalCapEx < tRemoteCapEx)
	{
		return TRUE;
	}

	//3.vmp能力比较
	if( IsVmpLowerThanOther(tLocalCap,tRemoteCap,tlocalCapEx,tRemoteCapEx))
	{
		return TRUE;
	}
	
	//4. BAS能力比较
	TBasChnAbility tlocalBasChn[NPLUS_MAX_BASCHN_NUM];
	TBasChnAbility tRemoteBasChn[NPLUS_MAX_BASCHN_NUM];
	TBasChnAbility *ptLocalBasChn = tlocalBasChn;
	TBasChnAbility *ptRemoteBasChn = tRemoteBasChn;
	u8 byLocalBasChnNum = 0;
	u8 byRemoteBasChnNum = 0;

	GetVideoBasChnAbilityArray(tLocalCap,tlocalCapEx,ptLocalBasChn,byLocalBasChnNum);
	GetVideoBasChnAbilityArray(tRemoteCap,tRemoteCapEx,ptRemoteBasChn,byRemoteBasChnNum);
	
	//视频BAS通道能力集比较
	if(IsBasChnLowerThanOther(ptLocalBasChn,ptRemoteBasChn,byLocalBasChnNum,byRemoteBasChnNum))
	{
		return TRUE;
	}

	memset(tlocalBasChn,0,sizeof(tlocalBasChn));
	memset(tRemoteBasChn,0,sizeof(tRemoteBasChn));
	byLocalBasChnNum = 0;
	byRemoteBasChnNum = 0;
	GetAudioBasChnAbilityArray(tLocalCap,tlocalCapEx,ptLocalBasChn,byLocalBasChnNum);
	GetAudioBasChnAbilityArray(tRemoteCap,tRemoteCapEx,ptRemoteBasChn,byRemoteBasChnNum);

	//音频BAS通道能力比较
	if(IsBasChnLowerThanOther(ptLocalBasChn,ptRemoteBasChn,byLocalBasChnNum,byRemoteBasChnNum,FALSE))
	{
		return TRUE;
	}

	return FALSE;
}

BOOL32 CNPlusInst::IsVmpLowerThanOther( TNPlusEqpCap tLocalCap, TNPlusEqpCap tOtherCap, TNPlusEqpCapEx tLocalCapEx, TNPlusEqpCapEx tOtherCapEx)
{

	if(tLocalCapEx.m_byMPU2EnhancedVmpNum < tOtherCapEx.m_byMPU2EnhancedVmpNum ||
		(tLocalCapEx.m_byMPU2BasicVmpNum + tLocalCapEx.m_byMPU2EcardBasicVmpNum + tLocalCapEx.m_byMPU2EnhancedVmpNum) < (tOtherCapEx.m_byMPU2BasicVmpNum + tOtherCapEx.m_byMPU2EcardBasicVmpNum + tOtherCapEx.m_byMPU2EnhancedVmpNum) ||
			(tLocalCapEx.m_byMPU2BasicVmpNum + tLocalCapEx.m_byMPU2EnhancedVmpNum) < (tOtherCapEx.m_byMPU2BasicVmpNum + tOtherCapEx.m_byMPU2EnhancedVmpNum) 	)
	{
		return TRUE;
	}
	
	//计算本地多余mpu2 vmp
	u8 byMPU2VmpNum = tLocalCapEx.m_byMPU2BasicVmpNum + tLocalCapEx.m_byMPU2EcardBasicVmpNum + tLocalCapEx.m_byMPU2EnhancedVmpNum -(tOtherCapEx.m_byMPU2BasicVmpNum + tOtherCapEx.m_byMPU2EcardBasicVmpNum + tOtherCapEx.m_byMPU2EnhancedVmpNum);
	for( u8 byIdx = 0; byIdx < byMPU2VmpNum; byIdx++ )
	{
		tLocalCap.m_byVmpNum ++;
		tLocalCap.m_abyVMPChnNum[tLocalCap.m_byVmpNum-1] = MAXNUM_MPU2VMP_MEMBER;
	}

	if(tLocalCap.m_byVmpNum < tOtherCap.m_byVmpNum
		|| tLocalCap.IsLocalVMPCapInferior(tOtherCap))
	{
		return TRUE;
	}

	return FALSE;
}

/*=============================================================================
    函 数 名： IsBasChnLowerThanOther
    功    能： 比较两个Bas通道能力集大小
    算法实现： 
    全局变量： 
    参    数： [IN]TBasChnAbility *ptBasChn Bas通道能力集数组指针
			   [IN]TBasChnAbility *ptOtherBasChn 另一组Bas通道能力集数组指针
               [IN] u16 wChNum Bas通道数
               [IN]u16 wOtherChnNum 另一组BAs通道数
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2011/12/29  4.0			chendaiwei                  创建
=============================================================================*/
BOOL32 CNPlusInst::IsBasChnLowerThanOther(TBasChnAbility *ptBasChn,TBasChnAbility *ptOtherBasChn,u8 byChNum, u8 byOtherChnNum, BOOL32 bVideoBas)
{
	if(ptBasChn == NULL || ptOtherBasChn == NULL)
	{
        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[IsBasChnLowerThanOther] ptLocalBasChn == NULL || ptRemoteBasChn == NULL.\n");
		
		return TRUE;
	}

	if( bVideoBas )
	{
		//1. BAs通道总数比较
		if(byChNum < byOtherChnNum)
		{
			LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS, "[IsBasChnLowerThanOther]BAs Chnnl Num <local:%d,remote:%d>",byChNum,byOtherChnNum);

			return TRUE;
		}
		
		//2. BAS解码能力比较
		u8 byCanBeUsedChnNum = 0;

		u8 byOtherDecHPChnNum = 0;
		u8 byOtherDecHD60ChnNum = 0;
		u8 byOtherDecHD30ChnNum  = 0;
		u8 byOtherDecSDChnNum = 0;
		for(u16 byIndex = 0; byIndex <byOtherChnNum; byIndex++)
		{
			switch (ptOtherBasChn[byIndex].m_byDecAbility)
			{
			case emDecHP:
				byOtherDecHPChnNum++;
				break;
			case emDecHD60:
				byOtherDecHD60ChnNum++;
				break;
			case emDecHD30:
				byOtherDecHD30ChnNum++;
				break;
			case emDecSD:
				byOtherDecSDChnNum ++;
				break;
			default:
				break;
			}
		}

		u8 byDecHPChnNum = 0;
		u8 byDecHD60ChnNum = 0;
		u8 byDecHD30ChnNum  = 0;
		u8 byDecSDChnNum = 0;
		for(u16 wIdx = 0; wIdx <byChNum; wIdx++)
		{
			switch (ptBasChn[wIdx].m_byDecAbility)
			{
			case emDecHP:
				byDecHPChnNum++;
				break;
			case emDecHD60:
				byDecHD60ChnNum++;
				break;
			case emDecHD30:
				byDecHD30ChnNum++;
				break;
			case emDecSD:
				byDecSDChnNum ++;
				break;
			default:
				break;
			}
		}

		//解HP
		if( byDecHPChnNum < byOtherDecHPChnNum )
		{
			LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS, "[IsBasChnLowerThanOther]Dec HP Ability<local:%d,remote:%d>",byDecHPChnNum,byOtherDecHPChnNum);
			return TRUE;
		}
		else
		{
			byCanBeUsedChnNum = byDecHPChnNum - byOtherDecHPChnNum;
		}
		
		//解高清60帧
		if(byDecHD60ChnNum < byOtherDecHD60ChnNum)
		{
			if( byDecHD60ChnNum + byCanBeUsedChnNum < byOtherDecHD60ChnNum )
			{
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS, "[IsBasChnLowerThanOther]Dec HD 60Fps Ability<local:%d,remote:%d>",byDecHD60ChnNum,byOtherDecHD60ChnNum);
				
				return TRUE;
			}
			else
			{
				byCanBeUsedChnNum = byCanBeUsedChnNum - (byOtherDecHD60ChnNum - byDecHD60ChnNum);
			}
		}
		else
		{
			byCanBeUsedChnNum = byCanBeUsedChnNum + (byDecHD60ChnNum - byOtherDecHD60ChnNum);
		}
		
		//解高清30帧
		if(byDecHD30ChnNum < byOtherDecHD30ChnNum)
		{
			if( byDecHD30ChnNum + byCanBeUsedChnNum < byOtherDecHD30ChnNum )
			{
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS, "[IsBasChnLowerThanOther]Dec HD 30Fps Ability<local:%d,remote:%d>",byDecHD30ChnNum,byOtherDecHD30ChnNum);
				
				return TRUE;
			}
			else
			{
				byCanBeUsedChnNum = byCanBeUsedChnNum - (byOtherDecHD30ChnNum - byDecHD30ChnNum);
			}
		}
		else
		{
			byCanBeUsedChnNum = byCanBeUsedChnNum + (byDecHD30ChnNum - byOtherDecHD30ChnNum);
		}
		
		//解标清
		if(byDecSDChnNum < byOtherDecSDChnNum)
		{

			if( byDecSDChnNum + byCanBeUsedChnNum < byOtherDecSDChnNum )
			{
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS, "[IsBasChnLowerThanOther]Dec SD Ability<local:%d,remote:%d>",byDecSDChnNum,byOtherDecSDChnNum);
				
				return TRUE;
			}
		}	

		//升序排列
		for( u8 bySortIdx = 0; bySortIdx < byChNum - 1; bySortIdx++ )
		{
			for(u8 byInnerIdx = 0; byInnerIdx <byChNum - bySortIdx -1; byInnerIdx ++)
			{
				if(ptBasChn[byInnerIdx].m_emChnType > ptBasChn[byInnerIdx+1].m_emChnType)
				{
					TBasChnAbility tTemp = ptBasChn[byInnerIdx];
					ptBasChn[byInnerIdx] = ptBasChn[byInnerIdx+1];
					ptBasChn[byInnerIdx+1] = tTemp;
				}
			}
		}

		for( u8 byIdex = 0; byIdex < byOtherChnNum - 1; byIdex++ )
		{
			for(u8 byInnerIdx = 0; byInnerIdx <byOtherChnNum - byIdex -1; byInnerIdx ++)
			{
				if(ptOtherBasChn[byInnerIdx].m_emChnType > ptOtherBasChn[byInnerIdx+1].m_emChnType)
				{
					TBasChnAbility tTemp = ptOtherBasChn[byInnerIdx];
					ptOtherBasChn[byInnerIdx] = ptOtherBasChn[byInnerIdx+1];
					ptOtherBasChn[byInnerIdx+1] = tTemp;
				}
			}
		}

		//3.解码能力比较
		u16 byOtherIdx = 0;
		u16 byIdx = 0;

		
		while( byOtherIdx < byOtherChnNum && byIdx < byChNum)
		{
			//local某通道解码能力足够
			if(ptOtherBasChn[byOtherIdx].m_byDecAbility <= ptBasChn[byIdx].m_byDecAbility && !ptBasChn[byIdx].m_byIsOccupy)
			{
				//Remote通道有60帧的编码能力，local通道是mpu_h,重新描述local 的mpu_h通道，以能出60帧描述
				if(ptOtherBasChn[byOtherIdx].Has60FpsEncAbility() && ptBasChn[byIdx].m_emChnType == emMPU_HChn)
				{
					ptBasChn[byIdx] = g_cNPlusApp.GetBasChnAbilityByBasChnType(emMPU_H60Chn);
				}
				
				u32 dwOtherBasEncAbility[6] = {0};
				u8 byOtherEncChnIdx = 0;
				u8 byEncChnIdx = 0;

				memcpy((u8*)dwOtherBasEncAbility,(u8*)&ptOtherBasChn[byOtherIdx].m_dwAbility[0],ptOtherBasChn[byOtherIdx].m_byEncNum*sizeof(ptOtherBasChn[byOtherIdx].m_dwAbility[0]));
				
				//遍历比较Remote某通道的N出，查找大于等于N出能力的一个或多个local通道
				while(byOtherEncChnIdx < ptOtherBasChn[byOtherIdx].m_byEncNum && byEncChnIdx < ptBasChn[byIdx].m_byEncNum)
				{
					dwOtherBasEncAbility[byOtherEncChnIdx] = ~(ptBasChn[byIdx].m_dwAbility[byEncChnIdx])&dwOtherBasEncAbility[byOtherEncChnIdx];
					
					//local某出能备Remote某出
					if(dwOtherBasEncAbility[byOtherEncChnIdx] == 0)
					{
						byEncChnIdx++;
						byOtherEncChnIdx++;
					}
					//local某出不能备Remote某出，跳到local下一出
					else
					{
						byEncChnIdx++;
					}

					//Remote通道ptRemoteBasChn[byRemoteIdx]能够被备，跳出循环
					if(byOtherEncChnIdx == ptOtherBasChn[byOtherIdx].m_byEncNum)
					{
						byOtherIdx++;
						ptBasChn[byIdx].m_byIsOccupy = 1; //占用
						byIdx ++;

						break;
					}
					//该local通道已被占用，仍然不足Remote 某Bas通道能力，尝试下一个local bas 通道
					else if(byEncChnIdx == ptBasChn[byIdx].m_byEncNum)
					{
						ptBasChn[byIdx].m_byIsOccupy = 1; //占用
						byIdx ++;
						byEncChnIdx = 0;
					}
				}
			}
			else
			{
				byIdx++; //local解码能力不足，找下一个
			}
			
			//local通道已占用完，Remote还有local无法备份的通道，local Bas Chn Ability < Remote Bas Chn Ability
			if(byIdx == byChNum && byOtherIdx != byOtherChnNum)
			{
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS, "[IsBasChnLowerThanOther]BAs Chnnl Dec ability is not enough!\n");
				return TRUE;
			}
		}

		return FALSE;
	}
	else
	{
		//目前音频BAS只有APU2，新音频BAS调整比较策略[3/30/2013 chendaiwei]
		if(byChNum < byOtherChnNum)
		{
			LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS, "[IsBasChnLowerThanOther]Audio BAs Chnnl Num <local:%d,remote:%d>",byChNum,byOtherChnNum);

			return TRUE;
		}

		//升序排列
		for( u8 bySortIdx = 0; bySortIdx < byChNum - 1; bySortIdx++ )
		{
			for(u8 byInnerIdx = 0; byInnerIdx <byChNum - bySortIdx -1; byInnerIdx ++)
			{
				if(ptBasChn[byInnerIdx].m_emChnType > ptBasChn[byInnerIdx+1].m_emChnType)
				{
					TBasChnAbility tTemp = ptBasChn[byInnerIdx];
					ptBasChn[byInnerIdx] = ptBasChn[byInnerIdx+1];
					ptBasChn[byInnerIdx+1] = tTemp;
				}
			}
		}

		for( u8 byIdex = 0; byIdex < byOtherChnNum - 1; byIdex++ )
		{
			for(u8 byInnerIdx = 0; byInnerIdx <byOtherChnNum - byIdex -1; byInnerIdx ++)
			{
				if(ptOtherBasChn[byInnerIdx].m_emChnType > ptOtherBasChn[byInnerIdx+1].m_emChnType)
				{
					TBasChnAbility tTemp = ptOtherBasChn[byInnerIdx];
					ptOtherBasChn[byInnerIdx] = ptOtherBasChn[byInnerIdx+1];
					ptOtherBasChn[byInnerIdx+1] = tTemp;
				}
			}
		}

		u8 byAudioBasIdx = 0;
		u8 byOthterAudioBasIdx = 0;
		while (byOthterAudioBasIdx < byOtherChnNum )
		{
			if(ptBasChn[byAudioBasIdx].m_byEncNum < ptOtherBasChn[byOthterAudioBasIdx].m_byEncNum)
			{
				byAudioBasIdx++;
				byOthterAudioBasIdx++;

				if( byAudioBasIdx == byChNum)
				{
					LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS, "[IsBasChnLowerThanOther]Audio BAs Chnnl capbility local<other\n");
					return TRUE;
				}
			}
			else
			{
				byOthterAudioBasIdx++;
			}
		}

		return FALSE;
	}
}

/*=============================================================================
    函 数 名： GetAllBasChnAbilityArray
    功    能： 获取本地和远端bas通道能力集的数组描述
    算法实现： 
    全局变量： 
    参    数： [IN]TNPlusEqpCap tCap 本地外设能力集
			   [IN]TNPlusEqpCapEx tCapEx 外设扩展能力集
			   [OUT]TBasChnAbility *ptBasChn 通道能力集首地址
			   [OUT]u16 &wBasChnNum BAs通道数
    返 回 值： void
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2011/12/29  4.0			chendaiwei                  创建
=============================================================================*/
void CNPlusInst::GetAllBasChnAbilityArray(TNPlusEqpCap tCap, TNPlusEqpCapEx tCapEx,TBasChnAbility *ptBasChn, u8 &byBasChnNum)
{
	byBasChnNum = 0;
	u8 byAudioBasNum = 0;
	u8 byVideoBasNum = 0;

	if(ptBasChn == NULL)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[GetAllBasChnAbilityArray] || ptLocalBasChn == NULL, error!\n");
		return;
	}

	GetVideoBasChnAbilityArray(tCap,tCapEx,ptBasChn,byVideoBasNum);
	GetAudioBasChnAbilityArray(tCap,tCapEx,(ptBasChn+byVideoBasNum),byAudioBasNum);

	byBasChnNum = byVideoBasNum + byAudioBasNum;
	
	return;
}


void CNPlusInst::GetVideoBasChnAbilityArray(TNPlusEqpCap tCap, TNPlusEqpCapEx tCapEx,TBasChnAbility *ptBasChn, u8 &byBasChnNum)
{
	byBasChnNum = 0;
	
	if(ptBasChn == NULL)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[GetAllBasChnAbilityArray] || ptLocalBasChn == NULL, error!\n");
		return;
	}
	
	//获取local BAS Chnnel
	u8 byChnNUm = 0;
	
	TBasChnAbility tVpuBasChn = g_cNPlusApp.GetBasChnAbilityByBasChnType(emVpuChn);
	for(u8 byIdx = 0; byIdx < tCap.m_byBasVidChnlNum; byIdx++)
	{
		*ptBasChn = tVpuBasChn;
		ptBasChn++;
		byBasChnNum++;
	}
	
	GetBasChnAbilityArrayByEqpType(TYPE_MAU_H263PLUS,tCap.m_byMAUH263pNum,ptBasChn,byChnNUm);
	ptBasChn = ptBasChn + byChnNUm;
	byBasChnNum = byBasChnNum + byChnNUm;
	
	GetBasChnAbilityArrayByEqpType(TYPE_MAU_NORMAL,tCap.m_byMAUNum,ptBasChn,byChnNUm);
	ptBasChn = ptBasChn + byChnNUm;
	byBasChnNum = byBasChnNum + byChnNUm;
	
	GetBasChnAbilityArrayByEqpType(TYPE_MPU,tCap.m_byMpuBasNum,ptBasChn,byChnNUm);
	ptBasChn = ptBasChn + byChnNUm;
	byBasChnNum = byBasChnNum + byChnNUm;
	
	GetBasChnAbilityArrayByEqpType(TYPE_8KE_BAS,tCapEx.m_by8000GBasNum,ptBasChn,byChnNUm);
	ptBasChn = ptBasChn + byChnNUm;
	byBasChnNum = byBasChnNum + byChnNUm;
	
	GetBasChnAbilityArrayByEqpType(TYPE_8KH_BAS,tCapEx.m_by8000HBasNum,ptBasChn,byChnNUm);
	ptBasChn = ptBasChn + byChnNUm;
	byBasChnNum = byBasChnNum + byChnNUm;
	
	GetBasChnAbilityArrayByEqpType(TYPE_MPU_H,tCap.m_byBap2BasNum,ptBasChn,byChnNUm);
	ptBasChn = ptBasChn + byChnNUm;
	byBasChnNum = byBasChnNum + byChnNUm;
	
	GetBasChnAbilityArrayByEqpType(TYPE_MPU2_ENHANCED,tCapEx.m_byMPU2EnhancedBasNum,ptBasChn,byChnNUm);
	ptBasChn = ptBasChn + byChnNUm;
	byBasChnNum = byBasChnNum + byChnNUm;
	
	GetBasChnAbilityArrayByEqpType(TYPE_MPU2_BASIC,tCapEx.m_byMPU2BasicBasNum,ptBasChn,byChnNUm);
	ptBasChn = ptBasChn + byChnNUm;
	byBasChnNum = byBasChnNum + byChnNUm;

	GetBasChnAbilityArrayByEqpType(TYPE_8KI_VID_BAS,tCapEx.m_by8000IVidBasNum,ptBasChn,byChnNUm);
	ptBasChn = ptBasChn + byChnNUm;
	byBasChnNum = byBasChnNum + byChnNUm;
	
	return;
}

void CNPlusInst::GetAudioBasChnAbilityArray(TNPlusEqpCap tCap, TNPlusEqpCapEx tCapEx,TBasChnAbility *ptBasChn, u8 &byBasChnNum)
{
	byBasChnNum = 0;
	
	if(ptBasChn == NULL)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[GetAllBasChnAbilityArray] || ptLocalBasChn == NULL, error!\n");
		return;
	}
	
	//获取local BAS Chnnel
	u8 byChnNUm = 0;
	
	GetBasChnAbilityArrayByEqpType(TYPE_APU2_BAS,tCapEx.m_byAPU2BasNum,ptBasChn,byChnNUm);
	ptBasChn = ptBasChn + byChnNUm;
	byBasChnNum = byBasChnNum + byChnNUm;

	GetBasChnAbilityArrayByEqpType(TYPE_8KI_AUD_BAS,tCapEx.m_by8000IAudBasNum,ptBasChn,byChnNUm);
	ptBasChn = ptBasChn + byChnNUm;
	byBasChnNum = byBasChnNum + byChnNUm;

	return;
}


/*=============================================================================
    函 数 名： GetBasChnAbilityArrayByEqpType
    功    能： 通过BAS外设类型和个数获取Bas能力描述数组
    算法实现： 
    全局变量： 
    参    数： [IN]u8 byEqpType Bas外设类型
               [IN]u8 byEqpNum  Bas外设个数
			   [IN][OUT]TBasChnAbility *ptBasChnnlArry Bas能力描述数组首地址
			   [IN][OUT]u8 &byArraySize Bas能力描述数组大小
    返 回 值： void
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2011/12/29  4.0			chendaiwei                  创建
=============================================================================*/
void CNPlusInst::GetBasChnAbilityArrayByEqpType( u8 byEqpType, u8 byEqpNum, TBasChnAbility *ptBasChnnlArry, u8 &byArraySize)
{	
	if( ptBasChnnlArry == NULL)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[CNPlusInst::GetBasChnAbilityArrayByEqpType] ptBasChnnlArry == NULL!\n");

		return;
	}
	
	byArraySize = 0;

	switch(byEqpType)
	{
	case TYPE_MAU_NORMAL:
		{
			TBasChnAbility tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(emMAUNormal);
			byArraySize = byEqpNum;
			for(u8 byCount = 0; byCount < byEqpNum; byCount++)
			{
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
			}
		}
		break;

	case TYPE_MAU_H263PLUS:
		{
			TBasChnAbility tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(emMAUH263PlusChn);
			byArraySize = byEqpNum;
			for(u8 byCount = 0; byCount < byEqpNum; byCount++)
			{
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
			}
		}
		break;

	case TYPE_MPU:
		{	
			TBasChnAbility tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(emMPUChn);
			byArraySize = byEqpNum*4; //4通道
			for(u8 byCount = 0; byCount < byArraySize; byCount++)
			{
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
			}
		}
		break;

	case TYPE_8KE_BAS:
		{
			for(u8 byCount = 0; byCount < byEqpNum/3; byCount ++)
			{
				TBasChnAbility tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(em8KGBrdChn);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
				
				tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(em8KGSelChn);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
				
				tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(em8KGDSChn);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
			}
			byArraySize = byEqpNum/3*3;
		}
		break;

	case TYPE_8KH_BAS:
		{
			for(u8 byCount = 0; byCount < byEqpNum/3; byCount ++)
			{
				TBasChnAbility tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(em8KHBrdChn);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
				
				tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(em8KHSelChn);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
				
				tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(em8KHDSChn);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
			}
			byArraySize = byEqpNum/3*3;
		}
		break;
	case TYPE_MPU_H:
		{
			TBasChnAbility tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(emMPU_HChn);
			byArraySize = byEqpNum*2; //4通道
			for(u8 byCount = 0; byCount < byArraySize; byCount++)
			{
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
			}
		}
		break;
	case TYPE_MPU2_BASIC:
		{
			for(u8 byCount = 0; byCount < byEqpNum; byCount ++)
			{
				TBasChnAbility tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(emMPU2BasicBrdChn);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;

				tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(emMPU2BasicDsChn);		
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
				
				tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(emMPU2BasicSelChn);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
			}
			byArraySize = byEqpNum*4;
		}

		break;
	case TYPE_MPU2_ENHANCED:
		{
			for(u8 byCount = 0; byCount < byEqpNum; byCount ++)
			{
				TBasChnAbility tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(emMPU2EnhancedBrdChn);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;

				tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(emMPU2EnhancedDsChn);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
				
				tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(emMPU2EnhancedSelChn);
				for( u8 byInnerCount = 0; byInnerCount < 5 ; byInnerCount++ )
				{
					memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
					ptBasChnnlArry ++;
				}
			}
			byArraySize = byEqpNum*7;
		}

		break;

	case TYPE_APU2_BAS:
		{
			TBasChnAbility tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(emAPU2BasChn);
			byArraySize = byEqpNum*MAXNUM_APU2_BASCHN;
			for(u8 byCount = 0; byCount < byArraySize; byCount++)
			{
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
			}
		}

		break;

	case TYPE_8KI_VID_BAS:
		{
			for(u8 byCount = 0; byCount < byEqpNum/3; byCount ++)
			{
				TBasChnAbility tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(em8KIVidBasChn0);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
				
				tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(em8KIVidBasChn1);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
				
				tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(em8KIVidBasChn2);
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
			}

			byArraySize = byEqpNum/3*3;
		}

		break;

	case TYPE_8KI_AUD_BAS:
		{
			TBasChnAbility tBas = g_cNPlusApp.GetBasChnAbilityByBasChnType(em8KIAudBasChn);
			byArraySize = byEqpNum*MAXNUM_8KI_AUD_BASCHN;
			for(u8 byCount = 0; byCount < byArraySize; byCount++)
			{
				memcpy((u8*)ptBasChnnlArry,(u8*)&tBas,sizeof(TBasChnAbility));
				ptBasChnnlArry ++;
			}
		}
		
		break;
		
	default:
		LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[CNPlusInst::GetBasChnAbilityArrayByEqpType] unexpected eqp type!\n");
		break;
	}

	return;
}


//CNPlusData
//construct
CNPlusData::CNPlusData( void ) : m_emLocalNPlusState(MCU_NPLUS_IDLE),
                                 m_byNPlusSwitchInsId(0),
                                 m_dwNPlusSwitchMcuIp(0),
                                 m_wRtdTime(0),
                                 m_wRtdNum(0),
                                 m_byNPlusSynOk(FALSE),
                                 m_byLocalMcuType(0)
{
}

//destruct
CNPlusData::~CNPlusData( void )
{
}

/*=============================================================================
函 数 名： GetNPlusSwitchInsId
功    能： 获取发送切换的实例id
算法实现： 
全局变量： 
参    数： void
返 回 值： u8 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/24  4.0			许世林                  创建
=============================================================================*/
u8 CNPlusData::GetMcuSwitchedInsId( void )
{
    return m_byNPlusSwitchInsId;
}

/*=============================================================================
函 数 名： SetNPlusSwitchInsId
功    能： 记录发生切换的实例id
算法实现： 
全局变量： 
参    数： u8 byInsId
返 回 值： u8 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/24  4.0			许世林                  创建
=============================================================================*/
void CNPlusData::SetMcuSwitchedInsId( u8 byInsId, u32 dwIpAddr )
{
    m_byNPlusSwitchInsId = byInsId;
    m_dwNPlusSwitchMcuIp = dwIpAddr;
}

/*=============================================================================
函 数 名： GetMcuSwitchedIp
功    能： 获取发送切换的mcu ip
算法实现： 
全局变量： 
参    数： void
返 回 值： u32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/28  4.0			许世林                  创建
=============================================================================*/
u32 CNPlusData::GetMcuSwitchedIp( void )
{
    return m_dwNPlusSwitchMcuIp;
}

/*=============================================================================
函 数 名： GetLocalNPlusState
功    能： 获取本地mcu N+1模式的状态机
算法实现： 
全局变量： 
参    数： void
返 回 值： ENPlusState 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/21  4.0			许世林                  创建
=============================================================================*/
ENPlusState CNPlusData::GetLocalNPlusState( void )
{
    return m_emLocalNPlusState;    
}

/*=============================================================================
函 数 名： SetLocalNPlusState
功    能： 设置本地 mcu N+1状态机(目前仅用于模式切换时状态机改变)
算法实现： 
全局变量： 
参    数： ENPlusState emState
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/21  4.0			许世林                  创建
=============================================================================*/
void CNPlusData::SetLocalNPlusState( ENPlusState emState )
{
    m_emLocalNPlusState = emState;
}

/*=============================================================================
函 数 名： GetRtdTime
功    能： rtd 时间间隔
算法实现： 
全局变量： 
参    数： void
返 回 值： u16 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/22  4.0			许世林                  创建
=============================================================================*/
u16 CNPlusData::GetRtdTime( void )
{
    return m_wRtdTime;
}

/*=============================================================================
函 数 名： GetRtdNum
功    能： rtd 次数
算法实现： 
全局变量： 
参    数： void
返 回 值： u16 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/22  4.0			许世林                  创建
=============================================================================*/
u16 CNPlusData::GetRtdNum( void )
{
    return m_wRtdNum;
}

/*=============================================================================
函 数 名： InitNPlusState
功    能： 初始化本地mcu工作状态机(需要在mcu代理初始化后使用)
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/21  4.0			许世林                  创建
=============================================================================*/
BOOL32 CNPlusData::InitNPlusState( void )
{
    TLocalInfo tLocalInfo;
    BOOL32 bNPlusMode = FALSE;
    if (SUCCESS_AGENT == g_cMcuAgent.GetLocalInfo(&tLocalInfo))
    {
        m_wRtdTime = tLocalInfo.GetNPlusRtdTime();
        m_wRtdNum = tLocalInfo.GetNPlusRtdNum();
        if (tLocalInfo.IsNPlusMode())
        {
            bNPlusMode = TRUE;
            if (tLocalInfo.IsNPlusBackupMode())
            {
                m_emLocalNPlusState = MCU_NPLUS_SLAVE_IDLE;

                // m_byNPlusSynOk字段对备MCU没有意义，此处始终置之为TRUE [12/20/2006-zbq]
                m_byNPlusSynOk = TRUE;
            }
            else if (0 != tLocalInfo.GetNPlusMcuIp())
            {
                m_emLocalNPlusState = MCU_NPLUS_MASTER_IDLE;
            }
        }

        m_byLocalMcuType = GetMcuPdtType();
    }
    return bNPlusMode;
}

/*=============================================================================
函 数 名： GetMcuEqpCap
功    能： 获取本地mcu外设能力
算法实现： 
全局变量： 
参    数： void
返 回 值： TNPlusEqpCap  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/29  4.0			许世林                  创建
2006/12/13  4.0         顾振华                  增加Mp和MtAdp
=============================================================================*/
TNPlusEqpCap    CNPlusData::GetMcuEqpCap( void )
{
    TNPlusEqpCap tEqpCap;
    TPeriEqpStatus tStatus;
    for (u8 byEqpId = 1; byEqpId <= MAXNUM_MCU_PERIEQP; byEqpId++)
    {
        if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus) && 1 == tStatus.m_byOnline)
        {
			//[chendaiwei 2010/09/27]增加混音器能力的判断
			if (byEqpId <= MIXERID_MAX)
			{
				tEqpCap.m_byMixerNum ++;
			}
            else if (byEqpId <= BASID_MAX)
            {
/*
#ifdef _8KE_
			 if (TYPE_8KE_BAS == tStatus.m_tStatus.tHdBas.GetEqpType())
			 {
				tEqpCap.m_byBasVidChnlNum++;
				LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[GetMcuEqpCap]m_byBasVidChnlNum is: %d\n", tEqpCap.m_byBasVidChnlNum);
			 }			 
#endif*/

#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)	
                if (!g_cMcuAgent.IsEqpBasHD(byEqpId))
                {
                    for (u8 byLoop = 0; byLoop < tStatus.m_tStatus.tBas.byChnNum && byLoop < MAXNUM_BAS_CHNNL; byLoop++)
                    {
                        if (BAS_CHAN_AUDIO == tStatus.m_tStatus.tBas.tChnnl[byLoop].GetChannelType())
                        {
                            tEqpCap.m_byBasAudChnlNum++;
                        }
                        else if (BAS_CHAN_VIDEO == tStatus.m_tStatus.tBas.tChnnl[byLoop].GetChannelType())
                        {
                            tEqpCap.m_byBasVidChnlNum++;
                        }
                    } 
                }
                else
                {
                    if (TYPE_MAU_NORMAL == tStatus.m_tStatus.tHdBas.GetEqpType())
                    {
						//  [1/13/2011 chendaiwei]需要通过tStatus.m_byOnline和EqpType两个条件来判断是否外设
						//  能力需要加1，TYPE_MAU_NORMAL默认值是0，也就是说实际上EqpType还没有被设置,但初始化值
						//  就是0，导致误判。所以此处增加一个状态机作为辅助的判断条件
						if(tStatus.m_tStatus.tHdBas.tStatus.tMauBas.GetVidChnStatus()->GetStatus() != TBasBaseChnStatus::IDLE)
						{
							tEqpCap.m_byMAUNum++;
						}

						LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS,"m_byMAUNum == %u\n",tEqpCap.m_byMAUNum);
                    }
                    else if (TYPE_MAU_H263PLUS == tStatus.m_tStatus.tHdBas.GetEqpType())
                    {
                        tEqpCap.m_byMAUH263pNum ++;
                    }
                    else if (TYPE_MPU == tStatus.m_tStatus.tHdBas.GetEqpType())
                    {
                        tEqpCap.m_byMpuBasNum ++;
                    }
					else if(TYPE_MPU_H == tStatus.m_tStatus.tHdBas.GetEqpType())
					{
						tEqpCap.m_byBap2BasNum ++;
					}
                    else if( TYPE_MPU2_ENHANCED != tStatus.m_tStatus.tHdBas.GetEqpType() && TYPE_MPU2_BASIC != tStatus.m_tStatus.tHdBas.GetEqpType())
                    {
                        LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusData::GetMcuEqpCap] unexpected mau type.%d\n",
                                                tStatus.m_tStatus.tHdBas.GetEqpType());
                    }
                }
#endif
            }
            else if (byEqpId <= VMPID_MAX)
            {
                // 8000B 画面数为4，即MAP数为2的情况下，仍具有16通道的合成能力 [12/29/2006-zbq]
                if ( MCU_TYPE_KDV8000B == GetMcuType() ||
                    MCU_TYPE_KDV8000C == GetMcuType() )
                {
                    if ( 4 == tStatus.m_tStatus.tVmp.m_byChlNum )
                    {
                        tStatus.m_tStatus.tVmp.m_byChlNum = 16;
                    }
                }
				
				if(tStatus.m_tStatus.tVmp.m_bySubType !=MPU2_VMP_BASIC && tStatus.m_tStatus.tVmp.m_bySubType != MPU2_VMP_ENHACED)
				{
					tEqpCap.m_byVmpNum++;
					// VMP能力具体到MAP数 [12/28/2006-zbq]
					tEqpCap.m_abyVMPChnNum[tEqpCap.m_byVmpNum-1] = tStatus.m_tStatus.tVmp.m_byChlNum;
				}
            }            
            else if (byEqpId <= PRSID_MAX)
            {
                tEqpCap.m_byPrsChnlNum += MAXNUM_PRS_CHNNL;
            }
        }
    }

    // guzh [12/13/2006] 读取Mp和MtAdp
    tEqpCap.m_byMtAdpNum = g_cMcuVcApp.GetMtAdpNum(PROTOCOL_TYPE_H323);
    tEqpCap.m_byMpNum = g_cMcuVcApp.GetMpNum();

    //GK Charge [12/18/2006-zbq]
    tEqpCap.SetIsGKCharge(g_cMcuAgent.GetIsGKCharge());
    
    //DCS [12/20/2006-zbq]
    TPeriDcsStatus tDcsStatus;
    for( u8 byDcsId = 1; byDcsId <= MAXNUM_MCU_DCS; byDcsId ++ )
    {
        if ( g_cMcuVcApp.GetPeriDcsStatus(byDcsId, &tDcsStatus) && 1 == tDcsStatus.m_byOnline )
        {
            tEqpCap.m_byDCSNum ++;
        }
    }

    return tEqpCap;
}

/*=============================================================================
函 数 名： GetMcuEqpCapEX
功    能： 获取本地mcu外设扩展能力
算法实现： 
全局变量： 
参    数： void
返 回 值： TNPlusEqpCapEx  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/29  4.0			许世林                  创建
2006/12/13  4.0         顾振华                  增加Mp和MtAdp
=============================================================================*/
TNPlusEqpCapEx    CNPlusData::GetMcuEqpCapEx( void )
{
    TNPlusEqpCapEx tEqpCapEx;
    TPeriEqpStatus tStatus;
    for (u8 byEqpId = 1; byEqpId <= MAXNUM_MCU_PERIEQP; byEqpId++)
    {
        if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tStatus) && 1 == tStatus.m_byOnline)
        {
			if (byEqpId <= MIXERID_MAX)
			{	
				u8 byMixerType = UNKONW_MIXER;
				if(g_cMcuAgent.GetMixerSubTypeByEqpId(byEqpId,byMixerType))
				{
					if(byMixerType == APU2_MIXER)
					{
						tEqpCapEx.m_byAPU2MixerNum++;
					}
					else if ( byMixerType  == MIXER_8KI)
					{
						tEqpCapEx.m_by8000IMixerNum++;
					}
				}
			}
            else if (byEqpId <= BASID_MAX)
            {
#ifdef _8KE_
				if (TYPE_8KE_BAS == tStatus.m_tStatus.tHdBas.GetEqpType())
				{
					tEqpCapEx.m_by8000GBasNum++;
				}			 
#endif

#ifdef _8KH_
				//8000H的宏和外设类型待定义[1/10/2012 chendaiwei]
				if (TYPE_8KH_BAS == tStatus.m_tStatus.tHdBas.GetEqpType())
				{
					tEqpCapEx.m_by8000HBasNum++;
				}			 
#endif

#ifdef _8KI_
				if (TYPE_8KI_VID_BAS == tStatus.m_tStatus.tHdBas.GetEqpType())
				{
					tEqpCapEx.m_by8000IVidBasNum++;
				}	
				if ( TYPE_8KI_AUD_BAS == tStatus.m_tStatus.tAudBas.GetEqpType())
				{
					tEqpCapEx.m_by8000IAudBasNum++;
				}
#endif
				if (g_cMcuAgent.IsEqpBasHD(byEqpId))
				{
					if(TYPE_MPU2_BASIC == tStatus.m_tStatus.tHdBas.GetEqpType())
					{
						tEqpCapEx.m_byMPU2BasicBasNum ++;
					}
					else if(TYPE_MPU2_ENHANCED == tStatus.m_tStatus.tHdBas.GetEqpType())
					{
						tEqpCapEx.m_byMPU2EnhancedBasNum++;
					}
				} 

				//TODO:IsEqpBasAud加入 TYPE_8KI_AUD_BAS [7/29/2013 chendaiwei]
				if(g_cMcuAgent.IsEqpBasAud(byEqpId))
				{
					if( TYPE_APU2_BAS == tStatus.m_tStatus.tAudBas.GetEqpType())
					{	
						tEqpCapEx.m_byAPU2BasNum++;
					}
					 
				}
			}
            else if (byEqpId <= VMPID_MAX)
            {
				if(tStatus.m_tStatus.tVmp.m_bySubType == MPU2_VMP_BASIC)
				{
					if(g_cMcuAgent.GetMPU2TypeByVmpEqpId(byEqpId) == BRD_TYPE_MPU2)
					{
						tEqpCapEx.m_byMPU2BasicVmpNum++;
					}
					else if(g_cMcuAgent.GetMPU2TypeByVmpEqpId(byEqpId) == BRD_TYPE_MPU2ECARD)
					{
						tEqpCapEx.m_byMPU2EcardBasicVmpNum++;
					}
				}
				else if(tStatus.m_tStatus.tVmp.m_bySubType == MPU2_VMP_ENHACED)
				{
					tEqpCapEx.m_byMPU2EnhancedVmpNum++;
				}
            }            
        }
    }

    return tEqpCapEx;
}

/*=============================================================================
    函 数 名： PackNplusExInfo
    功    能： Pack数据，存储到pbyOutbuf中
    算法实现： 
    全局变量： 
    参    数： [IN]  TNPlusEqpCapEx &tEqpCapEx 待pack数据
               [OUT] u8 *pbyOutbuf pack后数据存储位置
               [OUT] u16 &wPackExLen Pack后的数据长度 主机序
               [IN]  TNPlusMcuInfoEx *ptMcuInfoEx 待pack数据
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2011/12/29  4.0			chendaiwei              创建
    2013/03/13              liaokang                支持TNPlusMcuInfoEx
=============================================================================*/
void CNPlusData::PackNplusExInfo(const TNPlusEqpCapEx &tEqpCapEx,                                    
                                 u8 * pbyOutbuf, 
                                 u16 &wPackExLen,
                                 const TNPlusMcuInfoEx *ptMcuInfoEx/* = NULL*/)
{
	//扩展信息解析  u16扩展信息总长度 + 
    //              信息类型A（u8） + 信息类型A字节数（标识A数据大小,u16） + 信息类型A数据 + 
    //              信息类型B（u8）......
    wPackExLen = 0;
	u8 *pbyConfExSizeBuf = pbyOutbuf;
	u8 *pbyConfExBuf = pbyConfExSizeBuf + 2;

	//APU2
	if(tEqpCapEx.m_byAPU2MixerNum !=0)
	{
		*pbyConfExBuf = (u8)emAPU2MixerNum;
		pbyConfExBuf++;
		wPackExLen++;

		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);

		*pbyConfExBuf = tEqpCapEx.m_byAPU2MixerNum;
		pbyConfExBuf++;
		wPackExLen++;
	}

	//MPU2 BAS BAsic
	if(tEqpCapEx.m_byMPU2BasicBasNum !=0)
	{
		*pbyConfExBuf = (u8)emMPU2BasicBasNum;
		pbyConfExBuf++;
		wPackExLen++;

		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);

		*pbyConfExBuf = tEqpCapEx.m_byMPU2BasicBasNum;
		pbyConfExBuf++;
		wPackExLen++;
	}

	//MPU2 Ecard Bas enhanced
	if(tEqpCapEx.m_byMPU2EnhancedBasNum !=0)
	{
		*pbyConfExBuf = (u8)emMPU2EnhancedBasNum;
		pbyConfExBuf++;
		wPackExLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);

		*pbyConfExBuf = tEqpCapEx.m_byMPU2EnhancedBasNum;
		pbyConfExBuf++;
		wPackExLen++;
	}

	//MPU2 vmp basic
	if(tEqpCapEx.m_byMPU2BasicVmpNum !=0)
	{
		*pbyConfExBuf = (u8)emMPU2BasicVmpNum;
		pbyConfExBuf++;
		wPackExLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);

		*pbyConfExBuf = tEqpCapEx.m_byMPU2BasicVmpNum;
		pbyConfExBuf++;
		wPackExLen++;
	}

	//MPU2ECard vmp basic
	if(tEqpCapEx.m_byMPU2EcardBasicVmpNum !=0)
	{
		*pbyConfExBuf = (u8)emMPU2EcardBasicVmpNum;
		pbyConfExBuf++;
		wPackExLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);

		*pbyConfExBuf = tEqpCapEx.m_byMPU2EcardBasicVmpNum;
		pbyConfExBuf++;
		wPackExLen++;
	}

	//MPU2ECard vmp enhanced
	if(tEqpCapEx.m_byMPU2EnhancedVmpNum !=0)
	{
		*pbyConfExBuf = (u8)emMPU2EnhancedVmpNum;
		pbyConfExBuf++;
		wPackExLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);

		*pbyConfExBuf = tEqpCapEx.m_byMPU2EnhancedVmpNum;
		pbyConfExBuf++;
		wPackExLen++;
	}

	//8000G bas
	if(tEqpCapEx.m_by8000GBasNum !=0)
	{
		*pbyConfExBuf = (u8)em8000GBasNum;
		pbyConfExBuf++;
		wPackExLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);
		
		*pbyConfExBuf = tEqpCapEx.m_by8000GBasNum;
		pbyConfExBuf++;
		wPackExLen++;
	}

	//8000H bas
	if(tEqpCapEx.m_by8000HBasNum !=0)
	{
		*pbyConfExBuf = (u8)em8000HBasNum;
		pbyConfExBuf++;
		wPackExLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);
		
		*pbyConfExBuf = tEqpCapEx.m_by8000HBasNum;
		pbyConfExBuf++;
		wPackExLen++;
	}

	//apu2 bas
	if(tEqpCapEx.m_byAPU2BasNum !=0)
	{
		*pbyConfExBuf = (u8)emAPU2BasNum;
		pbyConfExBuf++;
		wPackExLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);
		
		*pbyConfExBuf = tEqpCapEx.m_byAPU2BasNum;
		pbyConfExBuf++;
		wPackExLen++;
	}

	//8000i vid bas
	if(tEqpCapEx.m_by8000IVidBasNum !=0)
	{
		*pbyConfExBuf = (u8)em8000IVidBasNum;
		pbyConfExBuf++;
		wPackExLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);
		
		*pbyConfExBuf = tEqpCapEx.m_by8000IVidBasNum;
		pbyConfExBuf++;
		wPackExLen++;
	}

	//8000i aud bas
	if(tEqpCapEx.m_by8000IAudBasNum !=0)
	{
		*pbyConfExBuf = (u8)em8000IAudBasNum;
		pbyConfExBuf++;
		wPackExLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);
		
		*pbyConfExBuf = tEqpCapEx.m_by8000IAudBasNum;
		pbyConfExBuf++;
		wPackExLen++;
	}

	//8000i mixer
	if(tEqpCapEx.m_by8000IMixerNum !=0)
	{
		*pbyConfExBuf = (u8)em8000IMixerNum;
		pbyConfExBuf++;
		wPackExLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackExLen = wPackExLen + sizeof(u16);
		
		*pbyConfExBuf = tEqpCapEx.m_by8000IMixerNum;
		pbyConfExBuf++;
		wPackExLen++;
	}


    //MCU信息扩展
    if( NULL != ptMcuInfoEx /*&& !ptMcuInfoEx->IsNull()*/)
    {
        *pbyConfExBuf = (u8)emNplusMcuInfoEx;
        pbyConfExBuf++;
        wPackExLen++;
        
        *(u16*)pbyConfExBuf = htons(sizeof(TNPlusMcuInfoEx));
        pbyConfExBuf = pbyConfExBuf + sizeof(u16);
        wPackExLen = wPackExLen + sizeof(u16);
        
        memcpy(pbyConfExBuf,ptMcuInfoEx,sizeof(TNPlusMcuInfoEx));
        pbyConfExBuf = pbyConfExBuf + sizeof(TNPlusMcuInfoEx);
        wPackExLen = wPackExLen + sizeof(TNPlusMcuInfoEx);
	}

	*(u16*)pbyConfExSizeBuf = htons(wPackExLen);
	
	wPackExLen += sizeof(u16);
	
	return;
}

/*=============================================================================
    函 数 名： UnPackNplusExInfo
    功    能： Unpack数据
    算法实现： 
    全局变量： 
    参    数： [OUT]  TNPlusEqpCapEx &tNplusEqpCap 保存unpack后的数据
               [IN]   const u8 *pbyBuf 待unpack的数据
               [OUT]  u16 &wUnPackExLen unPack后的数据缓冲长度 主机序
			   [OUT]  BOOL32 &bExistUnknowInfo 存在本MCU无法解析的信息，置为TRUE
               [OUT]  TNPlusMcuInfoEx *ptMcuInfoEx 保存unpack后的数据
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2011/12/29  4.0			chendaiwei              创建
    2013/03/13              liaokang                支持TNPlusMcuInfoEx
=============================================================================*/
void CNPlusData::UnPackNplusExInfo(TNPlusEqpCapEx & tNplusEqpCap, 
                                   const u8 *pbyInBuf, 
                                   u16 &wUnPackExLen,
                                   BOOL32 &bExistUnknowInfo, 
                                   TNPlusMcuInfoEx *ptMcuInfoEx/* = NULL*/)
{
    //扩展信息解析  u16扩展信息总长度 + 
    //              信息类型A（u8） + 信息类型A字节数（标识A数据大小,u16） + 信息类型A数据 + 
    //              信息类型B（u8）......
    u16 wNplusInfoExSize = ntohs( *(u16*)pbyInBuf);
	wUnPackExLen = wNplusInfoExSize + sizeof(u16);
	pbyInBuf = pbyInBuf + sizeof(u16);
	bExistUnknowInfo = FALSE;

	s32 nNplusInfoExSize = wNplusInfoExSize;

	while( nNplusInfoExSize > 0)
	{
		u16 wLength = 0;
		emNPlusCommunicateType etype = (emNPlusCommunicateType)(*pbyInBuf);
		pbyInBuf ++;
		
		switch(etype)
		{
		case emAPU2MixerNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_byAPU2MixerNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;
		case emMPU2BasicBasNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_byMPU2BasicBasNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;
		case emMPU2EnhancedBasNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_byMPU2EnhancedBasNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;
		case emMPU2BasicVmpNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_byMPU2BasicVmpNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;
		case emMPU2EcardBasicVmpNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_byMPU2EcardBasicVmpNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;
		case emMPU2EnhancedVmpNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_byMPU2EnhancedVmpNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;
		case em8000GBasNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_by8000GBasNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;
		case em8000HBasNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_by8000HBasNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;
		case emAPU2BasNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_byAPU2BasNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;

		case em8000IAudBasNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_by8000IAudBasNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;

		case em8000IVidBasNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_by8000IVidBasNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;

		case em8000IMixerNum:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tNplusEqpCap.m_by8000IMixerNum = *pbyInBuf;
			pbyInBuf = pbyInBuf + wLength;
			
			break;

        case emNplusMcuInfoEx:
            if( NULL != ptMcuInfoEx )
            {
                wLength = ntohs( *(u16*)pbyInBuf);
                pbyInBuf = pbyInBuf + sizeof(u16);
                memcpy(ptMcuInfoEx,pbyInBuf,min(wLength, sizeof(TNPlusMcuInfoEx)));
                pbyInBuf = pbyInBuf + wLength;
            }
            else
            {
                wLength = ntohs( *(u16*)pbyInBuf);
                pbyInBuf = pbyInBuf + sizeof(u16) +wLength;
                bExistUnknowInfo = TRUE;
                LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[UnPackNplusExInfo] ptMcuInfoEx is Null, wrong!\n");
            }
            
            break;
		default:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16) +wLength;
			bExistUnknowInfo = TRUE;
			
			break;
		}
		
		nNplusInfoExSize = nNplusInfoExSize - (s32)sizeof(u8) - (s32)sizeof(u16) - (s32)wLength;
	}

	return;
}

/*=============================================================================
函 数 名： GetEqpCapFromConf
功    能： 从会议信息获取所需mcu能力
算法实现： 
全局变量： 
参    数： TConfInfo &tConf
返 回 值： TNPlusEqpCap  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/30  4.0			许世林                  创建
=============================================================================*/
void CNPlusData::GetEqpCapFromConf( const TConfInfo &tConf, TNPlusEqpCap &tEqpCap, TNPlusEqpCapEx &tCapEx, TBasChnAbility *ptBasAbility, u8& byBasChnNum)
{

	//初始化
	byBasChnNum = 0;

	//[chendaiwei 2010/09/28]增加混音器能力
	if ( tConf.m_tStatus.IsMixing())
	{
		tEqpCap.m_byMixerNum ++;
	}

	//TBD [1/13/2012 chendaiwei] 针对apu2的处理逻辑.本会议使用APU2，但APU2不是必须的，则tCapEx.m_byAPU2Num不计数

	u8 byConfIdx = MIN_CONFIDX;
    //vmp
	for ( byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++)
	{
		CMcuVcInst * pcVcInst = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
		if (NULL == pcVcInst)
		{
			continue;
		}
		
		if (0 == memcmp(pcVcInst->m_tConf.GetConfE164(), tConf.GetConfE164(), MAXLEN_E164))
		{
			TPeriEqpStatus tPeriEqpStatus; 
			u8 byVmpSubType;
			for (u8 byVmpId = VMPID_MIN; byVmpId <= VMPID_MAX; byVmpId++)
			{
				g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tPeriEqpStatus );
				// 跳过非该会议的vmp，或未真正开启的vmp
				if (tPeriEqpStatus.GetConfIdx() != byConfIdx || TVmpStatus::START != tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
				{
					continue;
				}
				byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
				if(byVmpSubType == MPU2_VMP_BASIC)
				{
					if(g_cMcuAgent.GetMPU2TypeByVmpEqpId(byVmpId) == BRD_TYPE_MPU2)
					{
						tCapEx.m_byMPU2BasicVmpNum++;
					}
					else if(g_cMcuAgent.GetMPU2TypeByVmpEqpId(byVmpId) == BRD_TYPE_MPU2ECARD)
					{
						tCapEx.m_byMPU2EcardBasicVmpNum++;
					}
				}
				else if(byVmpSubType == MPU2_VMP_ENHACED)
				{
					tCapEx.m_byMPU2EnhancedVmpNum++;
				}
				else
				{
					tEqpCap.m_byVmpNum++;
				}
			}
		}
	}

    //bas
    if (tConf.GetConfAttrb().IsUseAdapter())
    {
		//遍历vc所有会议，取ConfId相等的出来，进行广播Bas占用情况的统计
		for ( byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++)
		{
			CMcuVcInst * pcVcInst = g_cMcuVcApp.GetConfInstHandle(byConfIdx);
			if (NULL == pcVcInst)
			{
				continue;
			}
			if (0 == memcmp(pcVcInst->m_tConf.GetConfE164(), tConf.GetConfE164(), MAXLEN_E164))
			{
				CBasChn *apcBrdBasChn[MAXNUM_PERIEQP]={NULL};
				TPeriEqpStatus tBasStatus;
				u8 byChnNum = 0;
				if(pcVcInst->GetBasAllBrdChn(byChnNum, apcBrdBasChn) && byChnNum > 0)
				{
					for (u8 byIdx = 0; byIdx < byChnNum; byIdx++)
					{
						if ( apcBrdBasChn[byIdx] == NULL )
						{
							continue;
						}

						if(g_cMcuVcApp.GetPeriEqpStatus(apcBrdBasChn[byIdx]->GetBas().GetEqpId(), &tBasStatus))
						{
#ifdef _8KE_
							if (TYPE_8KE_BAS == tBasStatus.m_tStatus.tHdBas.GetEqpType())
							{
								if(tBasStatus.m_tStatus.tHdBas.tStatus.t8keBas.GetVidChnStatus(0)->GetType() == BAS_8KECHN_MV)
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(em8KGBrdChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								else if(tBasStatus.m_tStatus.tHdBas.tStatus.t8keBas.GetVidChnStatus(0)->GetType() == BAS_8KECHN_DS)
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(em8KGDSChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								else if(tBasStatus.m_tStatus.tHdBas.tStatus.t8keBas.GetVidChnStatus(0)->GetType() == BAS_8KECHN_SEL)
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(em8KGSelChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								
							}						
#elif defined(_8KH_)
							if (TYPE_8KH_BAS == tBasStatus.m_tStatus.tHdBas.GetEqpType())
							{
								if(tBasStatus.m_tStatus.tHdBas.tStatus.t8khBas.GetVidChnStatus(0)->GetType() == BAS_8KHCHN_MV)
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(em8KHBrdChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								else if(tBasStatus.m_tStatus.tHdBas.tStatus.t8khBas.GetVidChnStatus(0)->GetType() == BAS_8KHCHN_DS)
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(em8KHDSChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								else if(tBasStatus.m_tStatus.tHdBas.tStatus.t8khBas.GetVidChnStatus(0)->GetType() == BAS_8KHCHN_SEL)
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(em8KHSelChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								
							}
#elif defined(_8KI_)
							if (TYPE_8KI_VID_BAS == tBasStatus.m_tStatus.tHdBas.GetEqpType())
							{
								if(tBasStatus.m_tStatus.tHdBas.tStatus.t8kiBas.GetVidChnStatus(0)->GetType() == BAS_8KICHN0)
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(em8KIVidBasChn0);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								else if(tBasStatus.m_tStatus.tHdBas.tStatus.t8kiBas.GetVidChnStatus(0)->GetType() == BAS_8KICHN1)
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(em8KIVidBasChn1);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								else if(tBasStatus.m_tStatus.tHdBas.tStatus.t8kiBas.GetVidChnStatus(0)->GetType() == BAS_8KICHN2)
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(em8KIVidBasChn2);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								
							}

							//TODO:IsEqpBasAud待修改
							//if( g_cMcuAgent.IsEqpBasAud(apcBrdBasChn[byIdx]->GetBas().GetEqpId()))
							{
								if ( TYPE_8KI_AUD_BAS == tBasStatus.m_tStatus.tAudBas.GetEqpType())
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(em8KIAudBasChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
							}
#else
							if( g_cMcuAgent.IsEqpBasAud(apcBrdBasChn[byIdx]->GetBas().GetEqpId()))
							{
								if ( TYPE_APU2_BAS == tBasStatus.m_tStatus.tAudBas.GetEqpType())
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(emAPU2BasChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
							}
							else if (!g_cMcuAgent.IsEqpBasHD(apcBrdBasChn[byIdx]->GetBas().GetEqpId()))
							{
								if (BAS_CHAN_AUDIO == tBasStatus.m_tStatus.tBas.tChnnl[apcBrdBasChn[byIdx]->GetChnId()].GetChannelType())
								{
									tEqpCap.m_byBasAudChnlNum++;
								}
								else if (BAS_CHAN_VIDEO == tBasStatus.m_tStatus.tBas.tChnnl[apcBrdBasChn[byIdx]->GetChnId()].GetChannelType())
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(emVpuChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
							}
							else
							{
								if (TYPE_MAU_NORMAL == tBasStatus.m_tStatus.tHdBas.GetEqpType())
								{			
									*ptBasAbility = GetBasChnAbilityByBasChnType(emMAUNormal);
									ptBasAbility ++;
									byBasChnNum ++;								
								}
								else if (TYPE_MAU_H263PLUS == tBasStatus.m_tStatus.tHdBas.GetEqpType())
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(emMAUH263PlusChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								else if (TYPE_MPU == tBasStatus.m_tStatus.tHdBas.GetEqpType())
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(emMPUChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								else if (TYPE_MPU_H == tBasStatus.m_tStatus.tHdBas.GetEqpType())
								{
									*ptBasAbility = GetBasChnAbilityByBasChnType(emMPU_HChn);
									ptBasAbility ++;
									byBasChnNum ++;
								}
								else if(TYPE_MPU2_BASIC == tBasStatus.m_tStatus.tHdBas.GetEqpType())
								{
									//BRD chnnl
									if(apcBrdBasChn[byIdx]->GetChnId() == 0 || apcBrdBasChn[byIdx]->GetChnId() == 1)
									{
										*ptBasAbility = GetBasChnAbilityByBasChnType(emMPU2BasicBrdChn);
									}
									//Sel chnnl
									else
									{
										*ptBasAbility = GetBasChnAbilityByBasChnType(emMPU2BasicSelChn);
									}
		
									ptBasAbility ++;
									byBasChnNum ++;
								}
								else if(TYPE_MPU2_ENHANCED == tBasStatus.m_tStatus.tHdBas.GetEqpType())
								{
									//BRD chnnl
									if(apcBrdBasChn[byIdx]->GetChnId() == 0 || apcBrdBasChn[byIdx]->GetChnId() == 1)
									{
										*ptBasAbility = GetBasChnAbilityByBasChnType(emMPU2EnhancedBrdChn);
									}
									//Sel chnnl
									else
									{
										*ptBasAbility = GetBasChnAbilityByBasChnType(emMPU2EnhancedSelChn);
									}
									
									ptBasAbility ++;
									byBasChnNum ++;
								}
								else
								{
									LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[GetEqpCapFromConf] ConIdx.%d unexpected  type.%d\n",
															pcVcInst->m_byConfIdx, tBasStatus.m_tStatus.tHdBas.GetEqpType());
								}
							}
#endif
						}
						else
						{
							LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[GetEqpCapFromConf] ConfIdx.%d GetPeriEqpStatus BasId.%d failed!\n",
													pcVcInst->m_byConfIdx, apcBrdBasChn[byIdx]->GetBas().GetEqpId());
						}				
					}
					LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_NPLUS, "[GetEqpCapFromConf] ConfIdx.%d Ocuppy %d BasChn<VpuVid.%d, VpuAud.%d, Mau.%d, MauH263.%d, Mpu-4.%d, Mpu-2.%d>!\n", 
												pcVcInst->m_byConfIdx,
												byChnNum,
												tEqpCap.m_byBasVidChnlNum, 
												tEqpCap.m_byBasAudChnlNum,
												tEqpCap.m_byMAUNum,
												tEqpCap.m_byMAUH263pNum,
												tEqpCap.m_byMpuBasNum,
												tEqpCap.m_byBap2BasNum);

				}
				else
				{
					LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[GetEqpCapFromConf] ConfIdx.%d GetBasAllBrdChn failed!\n", pcVcInst->m_byConfIdx);
				}
				break;
			}
		}
// #ifdef _8KE_
// 		u8 byNVChn = 0;
// 		u8 byDSChn = 0;
// 
// 		tEqpCap.m_byBasVidChnlNum = GetBasChnNumNeededByConf(tConf, byNVChn, byDSChn);
// #else
//         if (!IsHDConf(tConf))
//         {
//             if (tConf.GetSecAudioMediaType() != MEDIA_TYPE_NULL)
//             {
//                 tEqpCap.m_byBasAudChnlNum++;
//             }
//             else if (tConf.GetSecVideoMediaType() != MEDIA_TYPE_NULL)
//             {
//                 tEqpCap.m_byBasVidChnlNum++;
//             }
//             
//             if (tConf.GetSecBitRate() != 0)
//             {
//                 tEqpCap.m_byBasVidChnlNum++;
//             }
//         }
//         else
//         {
//             u8 byNVChn = 0;
//             u8 byDSChn = 0;
//             u8 byH263pChn = 0;
//             u8 byVGAChn = 0;
// 
//             u8 byMpuNum = 0;
//             g_cMcuVcApp.GetMpuNum(byMpuNum);
// 
//             //有mpu就认为是纯mpu备份，暂不支持mpu和mau的交叉备份
//             if (byMpuNum > 0)
//             {
//                 //CBasMgr cBasMgr;
//                 //cBasMgr.GetNeededMpu(tConf, byNVChn, byDSChn);
// 				GetMpuChnNumNeededByConf(tConf, byNVChn, byDSChn);
//                 tEqpCap.m_byMpuBasNum += (byNVChn + byDSChn)/MAXNUM_MPU_CHN + ((byNVChn + byDSChn)%MAXNUM_MPU_CHN ? 1 : 0);
//             }
//             else
//             {
//                 //CBasMgr cBasMgr;
//                 //cBasMgr.GetNeededMau(tConf, byNVChn, byH263pChn, byVGAChn);
//                 GetMauChnNumNeededByConf(tConf, byNVChn, byH263pChn, byVGAChn);
//                 tEqpCap.m_byMAUNum += byNVChn;
//                 tEqpCap.m_byMAUH263pNum += byH263pChn;
//             }
//         }
// #endif
    }

	TConfAttrb tTempConfAttrb = tConf.GetConfAttrb();
    //prs
    if (tTempConfAttrb.IsResendLosePack())
    {
        tEqpCap.m_byPrsChnlNum++;
    }
    //GK Charge [12/18/2006-zbq]
    tEqpCap.SetIsGKCharge(tConf.m_tStatus.IsGkCharge());
    
    return;
}

/*=============================================================================
函 数 名： SetNPlusSynOk
功    能： 是否数据备份成功
算法实现： 
全局变量： 
参    数： BOOL32 bSynOk
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/28  4.0			许世林                  创建
=============================================================================*/
void CNPlusData::SetNPlusSynOk( BOOL32 bSynOk )
{
    m_byNPlusSynOk = bSynOk ? 1 : 0;
}

/*=============================================================================
函 数 名： GetNPlusSynOk
功    能： 是否数据备份成功
算法实现： 
全局变量： 
参    数： void
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/28  4.0			许世林                  创建
=============================================================================*/
BOOL32 CNPlusData::GetNPlusSynOk( void )
{
    return (1 == m_byNPlusSynOk ? TRUE : FALSE);
}

/*=============================================================================
函 数 名： SetMcuType
功    能： 设置本端MCU的类新
算法实现： 
全局变量： 
参    数： u8 byType
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/12/20  4.0			张宝卿                  创建
=============================================================================*/
void CNPlusData::SetMcuType( u8 byType )
{
    m_byLocalMcuType = byType;
    return;
}

/*=============================================================================
函 数 名： GetMcuType
功    能： 获取本端MCU的类新
算法实现： 
全局变量： 
参    数： void
返 回 值： u8 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/12/20  4.0			张宝卿                  创建
=============================================================================*/
u8 CNPlusData::GetMcuType( void )
{
    return m_byLocalMcuType;
}

/*=============================================================================
函 数 名： PostMsgToNPlusDaemon
功    能： Mcu 业务其他应用发送消息到N+1 Deamon
算法实现： 
全局变量： 
参    数： CServMsg &cServMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/22  4.0			许世林                  创建
=============================================================================*/
void CNPlusData::PostMsgToNPlusDaemon( u16 wEvent, u8 *const pbyMsg, u16 wMsgLen )
{
    if ( m_emLocalNPlusState != MCU_NPLUS_IDLE )
    {
        OspPost(MAKEIID(AID_MCU_NPLUSMANAGER, CInstance::DAEMON), wEvent, pbyMsg, wMsgLen);
    }    
}

/*=============================================================================
    函 数 名： GetBasChnAbilityByBasChnType
    功    能： 通过BAS通道类型获取Bas能力描述
    算法实现： 
    全局变量： 
    参    数： [IN]u8 byBasChnType Bas通道类型
    返 回 值： TBasChnAbility 通道能力描述
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2011/12/29  4.0			chendaiwei                  创建
=============================================================================*/
TBasChnAbility CNPlusData::GetBasChnAbilityByBasChnType( emBasChnType byBasChnType)
{	
	TBasChnAbility tBasChn;
	
	switch(byBasChnType)
	{
	case emVpuChn:
		tBasChn.m_emChnType = emVpuChn;
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,emOther);
		tBasChn.SetDecAbility(emDecSD);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 1;
		break;

	case em8KHSelChn:
		tBasChn.m_emChnType = em8KHSelChn;
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,emOther);
		tBasChn.SetDecAbility(emDecSD);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 1;
		break;

	case emMAUH263PlusChn:
		tBasChn.m_emChnType = emMAUH263PlusChn;
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,emOther);
		tBasChn.SetEncAbility(0,emh263Plus);
		tBasChn.SetDecAbility(emDecHD30);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 1;
		break;

	case em8KHDSChn:
		tBasChn.m_emChnType = em8KHDSChn;
		tBasChn.SetEncAbility(0,emXgaBP);
		tBasChn.SetEncAbility(0,emh263Plus);
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(1,emXgaBP);
		tBasChn.SetEncAbility(1,emh263Plus);
		tBasChn.SetDecAbility(emDecHD30);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 2;
		break;

	case em8KGDSChn:
		tBasChn.m_emChnType = em8KGDSChn;
		tBasChn.SetEncAbility(0,emXgaBP);
		tBasChn.SetEncAbility(0,emh263Plus);
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(1,emXgaBP);
		tBasChn.SetEncAbility(1,emh263Plus);
		tBasChn.SetDecAbility(emDecHD30);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 2;
		break;

	case emMAUNormal:
		tBasChn.m_emChnType = emMAUNormal;
		tBasChn.SetEncAbility(0,emXgaBP);
		tBasChn.SetEncAbility(0,emSxga30BP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,em10802530BP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(1,emcifBP);
		tBasChn.SetEncAbility(1,emOther);
		tBasChn.SetEncAbility(1,emh263Plus);
		tBasChn.SetDecAbility(emDecHD30);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 2;
		break;

	case emMPUChn:
		tBasChn.m_emChnType = emMPUChn;
		tBasChn.SetEncAbility(0,emXgaBP);
		tBasChn.SetEncAbility(0,emSxga30BP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,em10802530BP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(1,emcifBP);
		tBasChn.SetEncAbility(1,emOther);
		tBasChn.SetEncAbility(1,emh263Plus);
		tBasChn.SetDecAbility(emDecHD30);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 2;
		break;

	case emMPU2BasicSelChn:
		tBasChn.m_emChnType = emMPU2BasicSelChn;
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(0,em7202530HP);
		tBasChn.SetEncAbility(0,em7205060BP);
		tBasChn.SetEncAbility(0,em7205060HP);
		tBasChn.SetEncAbility(0,emXgaBP);
		tBasChn.SetEncAbility(0,emXgaHP);
		tBasChn.SetEncAbility(0,emSxga30BP);
		tBasChn.SetEncAbility(0,emSxga30HP);
		tBasChn.SetEncAbility(0,emUxga60BP);
		tBasChn.SetEncAbility(0,emUxga60HP);
		tBasChn.SetEncAbility(0,em10802530BP);
		tBasChn.SetEncAbility(0,em10802530HP);
		tBasChn.SetEncAbility(0,em10805060BP);
		tBasChn.SetEncAbility(1,emOther);
		tBasChn.SetEncAbility(1,emh263Plus);
		tBasChn.SetEncAbility(2,emOther);
		tBasChn.SetEncAbility(2,emh263Plus);
		tBasChn.SetDecAbility(emDecHP);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 3;
		break;
		
	case emMPU2BasicDsChn:
		tBasChn.m_emChnType = emMPU2BasicDsChn;
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(0,em7202530HP);
		tBasChn.SetEncAbility(0,em7205060BP);
		tBasChn.SetEncAbility(0,em7205060HP);
		tBasChn.SetEncAbility(0,em10802530BP);
		tBasChn.SetEncAbility(0,em10802530HP);
		tBasChn.SetEncAbility(0,em10805060BP);
		tBasChn.SetEncAbility(0,em10805060HP);

		tBasChn.SetEncAbility(1,emcifBP);
		tBasChn.SetEncAbility(1,em4cifBP);
		tBasChn.SetEncAbility(1,em7202530BP);
		tBasChn.SetEncAbility(1,em7202530HP);
		tBasChn.SetEncAbility(1,em7205060BP);
		tBasChn.SetEncAbility(1,em7205060HP);
		tBasChn.SetEncAbility(1,em10802530BP);
		tBasChn.SetEncAbility(1,em10802530HP);

		tBasChn.SetEncAbility(2,emcifBP);
		tBasChn.SetEncAbility(2,em4cifBP);
		tBasChn.SetEncAbility(2,em7202530BP);
		tBasChn.SetEncAbility(2,em7202530HP);
		
		tBasChn.SetEncAbility(3,emcifBP);
		tBasChn.SetEncAbility(3,em4cifBP);
		
		tBasChn.SetEncAbility(4,emcifBP);
		tBasChn.SetEncAbility(4,emOther);
		tBasChn.SetEncAbility(4,emh263Plus);
		
		tBasChn.SetEncAbility(5,emcifBP);
		tBasChn.SetEncAbility(5,emOther);
		tBasChn.SetEncAbility(5,emh263Plus);
		
		tBasChn.SetDecAbility(emDecHP);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 6;
		break;

	case emMPU2EnhancedSelChn:
		tBasChn.m_emChnType = emMPU2EnhancedSelChn;
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(0,em7202530HP);
		tBasChn.SetEncAbility(0,em7205060BP);
		tBasChn.SetEncAbility(0,em7205060HP);
		tBasChn.SetEncAbility(0,emXgaBP);
		tBasChn.SetEncAbility(0,emXgaHP);
		tBasChn.SetEncAbility(0,emSxga30BP);
		tBasChn.SetEncAbility(0,emSxga30HP);
		tBasChn.SetEncAbility(0,emUxga60BP);
		tBasChn.SetEncAbility(0,emUxga60HP);
		tBasChn.SetEncAbility(0,em10802530BP);
		tBasChn.SetEncAbility(0,em10802530HP);
		tBasChn.SetEncAbility(0,em10805060BP);
		tBasChn.SetEncAbility(0,em10805060HP);
		tBasChn.SetEncAbility(1,emOther);
		tBasChn.SetEncAbility(1,emh263Plus);
		tBasChn.SetEncAbility(2,emOther);
		tBasChn.SetEncAbility(2,emh263Plus);
		tBasChn.SetDecAbility(emDecHP);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 3;
		break;
		
	case em8KGSelChn:
		tBasChn.m_emChnType = em8KGSelChn;
		tBasChn.SetEncAbility(0,emOther);
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetDecAbility(emDecSD);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 1;
		break;

	case em8KGBrdChn:
		tBasChn.m_emChnType = em8KGBrdChn;
		tBasChn.SetEncAbility(0,emOther);
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(1,emOther);
		tBasChn.SetEncAbility(1,emcifBP);
		tBasChn.SetEncAbility(1,em4cifBP);
		tBasChn.SetEncAbility(2,emOther);
		tBasChn.SetEncAbility(2,emcifBP);
		tBasChn.SetEncAbility(3,emOther);
		tBasChn.SetDecAbility(emDecHD30);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 4;
		break;
	
	case em8KHBrdChn:
		tBasChn.m_emChnType = em8KHBrdChn;
		tBasChn.SetEncAbility(0,emOther);
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(0,em10802530BP);
		tBasChn.SetEncAbility(1,emOther);
		tBasChn.SetEncAbility(1,emcifBP);
		tBasChn.SetEncAbility(1,em4cifBP);
		tBasChn.SetEncAbility(1,em7202530BP);
		tBasChn.SetEncAbility(2,emOther);
		tBasChn.SetEncAbility(2,emcifBP);
		tBasChn.SetEncAbility(2,em4cifBP);
		tBasChn.SetEncAbility(3,emOther);
		tBasChn.SetEncAbility(3,emcifBP);
		tBasChn.SetEncAbility(4,emOther);
		tBasChn.SetDecAbility(emDecHD60);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 5;
		break;

	case emMPU_HChn:
		tBasChn.m_emChnType = emMPU_HChn;
		tBasChn.SetEncAbility(0,emXgaBP);
		tBasChn.SetEncAbility(0,emSxga30BP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(0,em10802530BP);
		
		tBasChn.SetEncAbility(1,em7202530BP);
		tBasChn.SetEncAbility(2,em4cifBP);
		tBasChn.SetEncAbility(3,emcifBP);
		
		tBasChn.SetEncAbility(4,emcifBP);
		tBasChn.SetEncAbility(4,em4cifBP);
		tBasChn.SetEncAbility(4,emOther);
		tBasChn.SetEncAbility(4,emh263Plus);
		
		tBasChn.SetEncAbility(5,emcifBP);
		tBasChn.SetEncAbility(5,em4cifBP);
		tBasChn.SetEncAbility(5,emOther);
		tBasChn.SetEncAbility(5,emh263Plus);
		tBasChn.SetDecAbility(emDecHD60);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 6;
		break;

	case emMPU_H60Chn:
		tBasChn.m_emChnType = emMPU_H60Chn;
		tBasChn.SetEncAbility(0,em7205060BP);
		tBasChn.SetEncAbility(0,em10805060BP);
		
		tBasChn.SetEncAbility(1,emcifBP);
		tBasChn.SetEncAbility(1,em4cifBP);
		tBasChn.SetEncAbility(1,emOther);
		tBasChn.SetEncAbility(1,emh263Plus);
		
		tBasChn.SetEncAbility(2,emcifBP);
		tBasChn.SetEncAbility(2,em4cifBP);
		tBasChn.SetEncAbility(2,emOther);
		tBasChn.SetEncAbility(2,emh263Plus);
		tBasChn.SetDecAbility(emDecHD60);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 3;
		break;

	case emMPU2BasicBrdChn:
		tBasChn.m_emChnType = emMPU2BasicBrdChn;
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(0,em7202530HP);
		tBasChn.SetEncAbility(0,em7205060BP);
		tBasChn.SetEncAbility(0,em7205060HP);
		tBasChn.SetEncAbility(0,em10802530BP);
		tBasChn.SetEncAbility(0,em10802530HP);
		tBasChn.SetEncAbility(0,em10805060BP);
		tBasChn.SetEncAbility(0,em10805060HP);

		tBasChn.SetEncAbility(1,emcifBP);
		tBasChn.SetEncAbility(1,em4cifBP);
		tBasChn.SetEncAbility(1,em7202530BP);
		tBasChn.SetEncAbility(1,em7202530HP);
		tBasChn.SetEncAbility(1,em7205060BP);
		tBasChn.SetEncAbility(1,em7205060HP);
		tBasChn.SetEncAbility(1,em10802530BP);
		tBasChn.SetEncAbility(1,em10802530HP);

		tBasChn.SetEncAbility(2,emcifBP);
		tBasChn.SetEncAbility(2,em4cifBP);
		tBasChn.SetEncAbility(2,em7202530BP);
		tBasChn.SetEncAbility(2,em7202530HP);

		tBasChn.SetEncAbility(3,emcifBP);
		tBasChn.SetEncAbility(3,em4cifBP);

		tBasChn.SetEncAbility(4,emcifBP);
		tBasChn.SetEncAbility(4,emOther);
		tBasChn.SetEncAbility(4,emh263Plus);

		tBasChn.SetEncAbility(5,emcifBP);
		tBasChn.SetEncAbility(5,emOther);
		tBasChn.SetEncAbility(5,emh263Plus);

		tBasChn.SetDecAbility(emDecHP);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 6;
		break;

	case emMPU2EnhancedBrdChn:
		tBasChn.m_emChnType = emMPU2EnhancedBrdChn;
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(0,em7202530HP);
		tBasChn.SetEncAbility(0,em7205060BP);
		tBasChn.SetEncAbility(0,em7205060HP);
		tBasChn.SetEncAbility(0,em10802530BP);
		tBasChn.SetEncAbility(0,em10802530HP);
		tBasChn.SetEncAbility(0,em10805060BP);
		tBasChn.SetEncAbility(0,em10805060HP);
		
		tBasChn.SetEncAbility(1,emcifBP);
		tBasChn.SetEncAbility(1,em4cifBP);
		tBasChn.SetEncAbility(1,em7202530BP);
		tBasChn.SetEncAbility(1,em7202530HP);
		tBasChn.SetEncAbility(1,em7205060BP);
		tBasChn.SetEncAbility(1,em7205060HP);
		tBasChn.SetEncAbility(1,em10802530BP);
		tBasChn.SetEncAbility(1,em10802530HP);
		tBasChn.SetEncAbility(1,em10805060BP);
		tBasChn.SetEncAbility(1,em10805060HP);

		tBasChn.SetEncAbility(2,emcifBP);
		tBasChn.SetEncAbility(2,em4cifBP);
		tBasChn.SetEncAbility(2,em7202530BP);
		tBasChn.SetEncAbility(2,em7202530HP);
		
		tBasChn.SetEncAbility(3,emcifBP);
		tBasChn.SetEncAbility(3,em4cifBP);
		
		tBasChn.SetEncAbility(4,emcifBP);
		tBasChn.SetEncAbility(4,emOther);
		tBasChn.SetEncAbility(4,emh263Plus);
		
		tBasChn.SetEncAbility(5,emcifBP);
		tBasChn.SetEncAbility(5,emOther);
		tBasChn.SetEncAbility(5,emh263Plus);
		
		tBasChn.SetDecAbility(emDecHP);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 6;
		break;
	case emMPU2EnhancedDsChn:
		tBasChn.m_emChnType = emMPU2EnhancedBrdChn;
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(0,em7202530HP);
		tBasChn.SetEncAbility(0,em7205060BP);
		tBasChn.SetEncAbility(0,em7205060HP);
		tBasChn.SetEncAbility(0,em10802530BP);
		tBasChn.SetEncAbility(0,em10802530HP);
		tBasChn.SetEncAbility(0,em10805060BP);
		tBasChn.SetEncAbility(0,em10805060HP);
		
		tBasChn.SetEncAbility(1,emcifBP);
		tBasChn.SetEncAbility(1,em4cifBP);
		tBasChn.SetEncAbility(1,em7202530BP);
		tBasChn.SetEncAbility(1,em7202530HP);
		tBasChn.SetEncAbility(1,em7205060BP);
		tBasChn.SetEncAbility(1,em7205060HP);
		tBasChn.SetEncAbility(1,em10802530BP);
		tBasChn.SetEncAbility(1,em10802530HP);
		tBasChn.SetEncAbility(1,em10805060BP);
		tBasChn.SetEncAbility(1,em10805060HP);
		
		tBasChn.SetEncAbility(2,emcifBP);
		tBasChn.SetEncAbility(2,em4cifBP);
		tBasChn.SetEncAbility(2,em7202530BP);
		tBasChn.SetEncAbility(2,em7202530HP);
		
		tBasChn.SetEncAbility(3,emcifBP);
		tBasChn.SetEncAbility(3,em4cifBP);
		
		tBasChn.SetEncAbility(4,emcifBP);
		tBasChn.SetEncAbility(4,emOther);
		tBasChn.SetEncAbility(4,emh263Plus);
		
		tBasChn.SetEncAbility(5,emcifBP);
		tBasChn.SetEncAbility(5,emOther);
		tBasChn.SetEncAbility(5,emh263Plus);
		
		tBasChn.SetDecAbility(emDecHP);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 6;
		break;

	case emAPU2BasChn:
		tBasChn.m_emChnType = emAPU2BasChn;
		tBasChn.m_byBasMediaType = MODE_AUDIO;
		tBasChn.m_byEncNum = MAXNUM_APU2BAS_OUTCHN;
		break;

	//TODO:能力集描述[7/29/2013 chendaiwei]
	case em8KIVidBasChn0:
		tBasChn.m_emChnType = em8KIVidBasChn0;
		tBasChn.SetEncAbility(0,emOther);
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(0,em7202530HP);
		tBasChn.SetEncAbility(0,em7205060BP);
		tBasChn.SetEncAbility(0,em7205060HP);
		tBasChn.SetEncAbility(0,em10802530BP);
		tBasChn.SetEncAbility(0,em10802530HP);
		
		tBasChn.SetEncAbility(1,emOther);
		tBasChn.SetEncAbility(1,emcifBP);
		tBasChn.SetEncAbility(1,em4cifBP);
		tBasChn.SetEncAbility(1,em7202530BP);
		tBasChn.SetEncAbility(1,em7202530HP);		

		tBasChn.SetEncAbility(2,emOther);	
		tBasChn.SetEncAbility(2,emcifBP);
		tBasChn.SetEncAbility(2,em4cifBP);

		tBasChn.SetEncAbility(3,emcifBP);
		tBasChn.SetEncAbility(3,emOther);
		
		tBasChn.SetEncAbility(4,emOther);
		
		tBasChn.SetDecAbility(emDecHP);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 5;

		break;

	case em8KIVidBasChn1:
		tBasChn.m_emChnType = em8KIVidBasChn1;
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);
		tBasChn.SetEncAbility(0,emXgaBP);
		tBasChn.SetEncAbility(0,em7202530BP);
		tBasChn.SetEncAbility(0,emSxga20BP);

		//tBasChn.SetEncAbility(1,emcifBP);
		//tBasChn.SetEncAbility(1,emXgaBP);
		
		tBasChn.SetEncAbility(1,emXgaBP);
		tBasChn.SetEncAbility(1,emh263Plus);	
		//tBasChn.SetEncAbility(2,emXgaBP);
		
		tBasChn.SetDecAbility(emDecHP);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 2;
		break;

	case em8KIVidBasChn2:
		tBasChn.m_emChnType = em8KIVidBasChn2;
		tBasChn.SetEncAbility(0,emOther);
		tBasChn.SetEncAbility(0,emcifBP);
		tBasChn.SetEncAbility(0,em4cifBP);		

		tBasChn.SetDecAbility(emDecHP);
		tBasChn.m_byBasMediaType = MODE_VIDEO;
		tBasChn.m_byEncNum = 1;
		break;

	case em8KIAudBasChn:
		tBasChn.m_emChnType = em8KIAudBasChn;
		tBasChn.m_byBasMediaType = MODE_AUDIO;
		tBasChn.m_byEncNum = MAXNUM_8KIAUDBAS_OUTPUT;
		break;

	default:
		LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[CNPlusInst::GetBasChnAbilityByBasChnType]unexpected Bas Chn type:%d",byBasChnType);
		break;
	}

	return tBasChn;
}

/*=============================================================================
函 数 名： IsSupportRollBack
功    能：  是否支持回滚会议
算法实现： 
全局变量： 
参    数： u8  *pabyChnType           记录所需通道的类型的数组
		   u8 byChnNum				  通道个数			
		   TConfInfoEx *aptConfInfoEx 记录所有会议额外信息的数组
		   u8 byConfNum				   会议个数
返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2012/04/18  4.7			倪志俊                 创建
=============================================================================*/
//TODO:预占函数待修改
BOOL32 CNPlusInst::IsBasSupportRollBack(TBasChnAbility *patBasChnAbility,u8 byChnNum)
{
	if ( NULL == patBasChnAbility )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[IsBasSupportRollBack]pabyChnType is NULL!\n");
		return FALSE;
	}

	if ( byChnNum > MAXNUM_MCU_BAS_CHN )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[IsBasSupportRollBack]byChnNum=%d more than maxvalue!\n",byChnNum);
		return FALSE;
	}
	
	//1.获得BAS通道
	CNPlusBasChnListMgr cChnListMgr;
	COldBasChn	acOldBasChn[MAXNUM_MCU_BAS_CHN];
	CMpu2BasChn acMpu2BasChn[MAXNUM_MCU_BAS_CHN];
	CApu2BasChn acApu2BasChn[MAXNUM_MCU_BAS_CHN];
	C8KIAudBasChn  ac8KIAudBasChn[MAXNUM_MCU_BAS_CHN];
	u8		byRealChnNum = 0;
	BOOL32	bIsChnFrom8KH = FALSE;
	BOOL32  bIsChnFrom8KG = FALSE;
	BOOL32  bIsChnFrom8KI = FALSE;
	for ( u8 byIdx = 0; byIdx<byChnNum; byIdx++ )
	{
		switch (patBasChnAbility[byIdx].m_emChnType)
		{
		case emVpuChn:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(ADAPT_TYPE_AUD,TRUE,TRUE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emVpuChn\n");
			}
			break;
		case emMAUNormal:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(TYPE_MAU_NORMAL,TRUE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emMAUNormal\n");
			}
			break;
		case emMAUH263PlusChn:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(TYPE_MAU_H263PLUS,TRUE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emMAUH263PlusChn\n");
			}
			break;
		case em8KGBrdChn:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(BAS_8KECHN_MV,FALSE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				bIsChnFrom8KG = TRUE;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:em8KGBrdChn\n");
			}
			break;
		case em8KGDSChn:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(BAS_8KECHN_DS,FALSE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				bIsChnFrom8KG = TRUE;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:em8KGDSChn\n");
			}
			break;
		case em8KGSelChn:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(BAS_8KECHN_SEL,FALSE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				bIsChnFrom8KG = TRUE;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:em8KGSelChn\n");
			}
			break;
		case em8KHBrdChn:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(BAS_8KHCHN_MV,FALSE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				bIsChnFrom8KH = TRUE;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:em8KHBrdChn\n");
			}
			break;
		case em8KHDSChn:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(BAS_8KHCHN_DS,FALSE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				bIsChnFrom8KH = TRUE;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:em8KHDSChn\n");
			}
			break;
		case em8KHSelChn:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(BAS_8KHCHN_SEL,FALSE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;	
				bIsChnFrom8KH = TRUE;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:em8KHSelChn\n");
			}
			break;
		case em8KIVidBasChn2:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(BAS_8KICHN2,FALSE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;	
				bIsChnFrom8KI = TRUE;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:em8KIVidBasChn2\n");
			}
			break;
		case em8KIVidBasChn1:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(BAS_8KICHN1,FALSE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;	
				bIsChnFrom8KI = TRUE;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:em8KIVidBasChn1\n");
			}
			break;
		case em8KIVidBasChn0:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(BAS_8KICHN0,FALSE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;	
				bIsChnFrom8KI = TRUE;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:em8KIVidBasChn0\n");
			}
			break;
		case emMPU_H60Chn:
		case emMPU_HChn:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(TYPE_MPU_H,TRUE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emMPU_H60Chn/emMPU_HChn\n");
			}
			break;
		case emMPUChn:
			{
				acOldBasChn[byRealChnNum].NPlusSetAttr(TYPE_MPU,TRUE);
				acOldBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acOldBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emMPUChn\n");
			}
			break;
		case emMPU2BasicBrdChn:
			{
				acMpu2BasChn[byRealChnNum].SetMpu2BasAttr(0,TYPE_MPU2_BASIC);
				acMpu2BasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acMpu2BasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emMPU2BasicBrdChn\n");
			}
			break;
		case emMPU2BasicDsChn:
			{
				acMpu2BasChn[byRealChnNum].SetMpu2BasAttr(1,TYPE_MPU2_BASIC);
				acMpu2BasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acMpu2BasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emMPU2BasicDsChn\n");
			}
			break;
		case emMPU2BasicSelChn:
			{
				acMpu2BasChn[byRealChnNum].SetMpu2BasAttr(2,TYPE_MPU2_BASIC);
				acMpu2BasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acMpu2BasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emMPU2BasicSelChn\n");
			}
			break;
		case emMPU2EnhancedBrdChn:
			{
				acMpu2BasChn[byRealChnNum].SetMpu2BasAttr(0,TYPE_MPU2_ENHANCED);
				acMpu2BasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acMpu2BasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emMPU2EnhancedBrdChn\n");
			}
			break;
		case emMPU2EnhancedDsChn:
			{
				acMpu2BasChn[byRealChnNum].SetMpu2BasAttr(1,TYPE_MPU2_ENHANCED);
				acMpu2BasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acMpu2BasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emMPU2EnhancedDsChn\n");
			}
			break;
		case emMPU2EnhancedSelChn:
			{
				acMpu2BasChn[byRealChnNum].SetMpu2BasAttr(2,TYPE_MPU2_ENHANCED);
				acMpu2BasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acMpu2BasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;	
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emMPU2EnhancedSelChn\n");
			}
			break;
		case emAPU2BasChn:
			{
				acApu2BasChn[byRealChnNum].SetApu2BasAttr();
				acApu2BasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&acApu2BasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:emApu2BasChn\n");
			}
			break;
		case em8KIAudBasChn:
			{
				ac8KIAudBasChn[byRealChnNum].Set8KIAudBasAttr();
				ac8KIAudBasChn[byRealChnNum].SetIsOnLine(TRUE);
				cChnListMgr.InsertBasChn(&ac8KIAudBasChn[byRealChnNum],byRealChnNum);
				byRealChnNum++;
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]add chn:em8KIAudBasChn\n");
			}
			break;

		default:
			{
				LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[IsBasSupportRollBack]pabyChnType[byIdx]:%d-%d is illegal!\n",patBasChnAbility[byIdx].m_emChnType,byIdx);
				return FALSE;
			}
		}
	}
	
	//通道排序
	cChnListMgr.SetRealChnNum(byRealChnNum);
	cChnListMgr.SortBasChn();
	LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_NPLUS,"[IsBasSupportRollBack]byRealChnNum:%d\n",byRealChnNum);

	//2.开始预占
	TNeedVidAdaptData atReqResource[emMODE_END];	//主流视频、双流
	TNeedAudAdaptData tReqAudResource;				//音频所需能力
	TConfInfoEx tTempConfEx;
	u16 wLength = 0;
	BOOL32 bExistUnknownType = FALSE;
	for ( u8 byConfIdx = 0; byConfIdx < MAXNUM_ONGO_CONF; byConfIdx++ )
	{
		if ( m_atConfData[byConfIdx].IsNull() )
		{
			continue;
		}

		if( !m_atConfData[byConfIdx].m_tConf.GetConfAttrb().IsUseAdapter() )
		{
			continue;
		}

		TConfInfoEx tConfAttrbExInfo = GetConfInfoExFromConfAttrb(m_atConfData[byConfIdx].m_tConf);

		if(!m_atConfData[byConfIdx].m_tConf.HasConfExInfo())
		{
			tTempConfEx = tConfAttrbExInfo;
		}
		else
		{
			UnPackConfInfoEx(tTempConfEx,m_atConfExData[byConfIdx].m_byConInfoExBuf,wLength,bExistUnknownType);
			TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];
			u8   byAudioCapNum = tTempConfEx.GetAudioTypeDesc(atAudioTypeDesc);
			if (byAudioCapNum > 0)//会议扩展信息中有音频能力信息则需要用扩展信息中去覆盖
			{
				tConfAttrbExInfo.SetAudioTypeDesc(atAudioTypeDesc,byAudioCapNum);
			}
			tTempConfEx.AddCapExInfo(tConfAttrbExInfo);
		}
		
		memset( &atReqResource,0,sizeof(atReqResource) );
		memset( &tReqAudResource,0,sizeof(tReqAudResource) );

		if ( !GetBasCapConfNeedForNPlus(m_atConfData[byConfIdx].m_tConf, tTempConfEx, atReqResource,&tReqAudResource, bIsChnFrom8KH,bIsChnFrom8KG,bIsChnFrom8KI) )
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[IsBasSupportRollBack]GetBasCapConfNeed failed\n");
			return FALSE;
		}

		if ( !cChnListMgr.CheckBasEnoughForReq(atReqResource, &tReqAudResource) )
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[IsBasSupportRollBack]left baschns don't support conf:%s\n",m_atConfData[byConfIdx].m_tConf.GetConfName());
			return FALSE;
		}
	}

	return TRUE;
}

/*====================================================================
    函数名      : TranslateEncodingForm
    功能        : NPlus内部转码
    算法实现    :
    引用全局变量:
    输入参数说明: 
    返回值说明  :
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人      修改内容
    2013/05/08              liaokang      创建
====================================================================*/
void CNPlusInst::TranslateEncodingForm( void )
{
#ifdef _UTF8
    if( emenCoding_GBK != m_emMcuEncodingForm )
    {
        return;
    }

    u16 wLoop = 0;
    //s8 achTemp[2*MAX_CHARLENGTH] = {0};
    //1、转用户组信息 m_cUsrGrpsInfo
    TransEncodingOfUsrGrps(m_cUsrGrpsInfo, TRANSENCODING_GBK_UTF8);

    //2、转用户信息 m_acUsrInfo[MAXNUM_USRNUM]
    for ( wLoop = 0; wLoop < MAXNUM_USRNUM; wLoop++)
    {
        TransEncodingOfUsrInfo(m_acUsrInfo[wLoop], TRANSENCODING_GBK_UTF8);
    }

    for ( wLoop = 0; wLoop < MAXNUM_ONGO_CONF; wLoop++)
    {  
        //3、转mcu上会议信息m_atConfData[MAXNUM_ONGO_CONF]
        TransEncodingOfNPlusConfData(m_atConfData[wLoop], TRANSENCODING_GBK_UTF8);

        //4、会议扩展信息m_atConfExData[MAXNUM_ONGO_CONF]不需要转码
        //4.1 m_byConInfoExBuf不需要转码
        //4.2 m_atSmcuCallInfo.m_achAlias为164别名，不需要转码
    }

    //5、m_tRASInfo不需要转码，其别名是通过协议接口取到的，就是ucs2
    //6、m_atChargeInfo[MAXNUM_ONGO_CONF]不需要转码，没有用到，相关信息在会议信息中有
    //7、m_atNplusParam[MAX_PRIEQP_NUM]不需要转码

#endif
}

/*=============================================================================
函 数 名： InsertBasChn
功    能： 添加通道
算法实现： 
全局变量： 
参    数： CBasChn *pBasChn,
		   u8 byIdx
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2012/04/18  4.7			倪志俊                 创建
=============================================================================*/
void CNPlusBasChnListMgr::InsertBasChn(CBasChn *pBasChn,u8 byIdx)
{
	if ( NULL == pBasChn )
	{
		return;
	}
	m_apcBasChn[byIdx] = pBasChn; 	
}

/*=============================================================================
函 数 名： SortBasChn
功    能： 对通道进行排序
算法实现： 
全局变量： 
参    数：
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2012/04/18  4.7			倪志俊                 创建
=============================================================================*/
void CNPlusBasChnListMgr::SortBasChn()
{
	//将BAS通道按照能力从小到大排列依次
	//升序排列
	CBasChn *pcTempBasChn;
	for( u8 bySortIdx = 0; bySortIdx < m_byRealBasChNum - 1; bySortIdx++ )
	{
		for(u8 byInnerIdx = 0; byInnerIdx <m_byRealBasChNum - bySortIdx -1; byInnerIdx ++)
		{
			if( m_apcBasChn[byInnerIdx]->GetMaxDecAbility() > m_apcBasChn[byInnerIdx+1]->GetMaxDecAbility() )
			{	
				pcTempBasChn= m_apcBasChn[byInnerIdx];
				m_apcBasChn[byInnerIdx] = m_apcBasChn[byInnerIdx+1];
				m_apcBasChn[byInnerIdx+1] = pcTempBasChn;
			}
			else if ( m_apcBasChn[byInnerIdx]->GetMaxDecAbility() == m_apcBasChn[byInnerIdx+1]->GetMaxDecAbility() )
			{
				if ( m_apcBasChn[byInnerIdx]->GetMaxEncAbility() > m_apcBasChn[byInnerIdx+1]->GetMaxEncAbility() )
				{
					pcTempBasChn= m_apcBasChn[byInnerIdx];
					m_apcBasChn[byInnerIdx] = m_apcBasChn[byInnerIdx+1];
					m_apcBasChn[byInnerIdx+1] = pcTempBasChn;
				}
			}
		}
	}
	
}

/*=============================================================================
函 数 名： GetOneIdleSuitableBasChn
功    能： 获得一个适合的通道
算法实现： 
全局变量： 
参    数：TNeedBasResData &tLeftReqResData 剩余的能力集

返 回 值： CBasChn* 返回获得通道 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2012/04/18  4.7			倪志俊                 创建
=============================================================================*/
CBasChn* CNPlusBasChnListMgr::GetOneIdleSuitableBasChn(TNeedVidAdaptData &tLeftReqResData)
{	
	CBasChn *pSubSuitableChn = NULL;		//记录次优的BAS通道
	TNeedVidAdaptData tTempLeftData;				//记录次优通道使用后，剩余能力
	TNeedVidAdaptData tTempNeedData;				//临时保存BAS所需资源数据
	memcpy( &tTempNeedData, &tLeftReqResData, sizeof(TNeedVidAdaptData) );
	
	CBasChn *pTmpBasChn = NULL;
	TVideoStreamCap atSimCap[MAX_CONF_BAS_ADAPT_NUM]; 
	u8 byOccupiedNum=0;
	for ( u8 byIdx = 0; byIdx < m_byRealBasChNum; byIdx++ )
	{
		pTmpBasChn = m_apcBasChn[byIdx];
		if ( NULL == pTmpBasChn )
		{
			continue;
		}

		if ( pTmpBasChn->IsBasChnSupportNeeds(tLeftReqResData,atSimCap) )
		{	
			//根据已占去的编码路数，对ptOccpiedResData多余的元素进行初始化
			byOccupiedNum = tTempNeedData.m_byRealEncNum - tLeftReqResData.m_byRealEncNum;
			if ( byOccupiedNum>0 && byOccupiedNum < MAX_CONF_BAS_ADAPT_NUM )
			{
				for ( u8 byLoop =byOccupiedNum; byLoop<MAX_CONF_BAS_ADAPT_NUM; byLoop++ )
				{
					atSimCap[byLoop].Clear();
				}
			}
			//找能力最优的通道，找不到先记录该通道，以便找不到时可以使用次优通道
			if (tLeftReqResData.m_byRealEncNum ==0 )
			{
				pTmpBasChn->SetIsReserved(TRUE);
				pTmpBasChn->SetMediaMode(tLeftReqResData.m_byMediaMode);
				return pTmpBasChn;		//返回最优通道
			}
			else
			{
				pSubSuitableChn = pTmpBasChn;	//记录次优通道
				memcpy( &tTempLeftData,&tLeftReqResData,   sizeof(TNeedVidAdaptData) );	//记录次优剩余数据
				memcpy( &tLeftReqResData, &tTempNeedData, sizeof(TNeedVidAdaptData) );	//恢复数据继续查找
			}
			
		}
	}

	//判断是否有次优通道可使用
	if ( pSubSuitableChn != NULL)
	{
		pSubSuitableChn->SetIsReserved(TRUE);		   //预占
		pSubSuitableChn->SetMediaMode(tLeftReqResData.m_byMediaMode);
		memcpy( &tLeftReqResData, &tTempLeftData, sizeof(TNeedVidAdaptData) );	//返回剩余数据
		return pSubSuitableChn;			//返回通道
	}
	
	memcpy( &tLeftReqResData, &tTempNeedData, sizeof(TNeedVidAdaptData) );	//恢复数据
	return NULL;
}

/*=============================================================================
函 数 名： GetOneIdelSuitableAudBasChn
功    能： 获得一个适合的通道
算法实现： 
全局变量： 
参    数：TNeedAudAdaptData &tLeftReqResData 剩余的能力集

返 回 值： CBasChn* 返回获得通道 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2013/03/29  4.7			倪志俊                 创建
=============================================================================*/
CBasChn* CNPlusBasChnListMgr::GetOneIdelSuitableAudBasChn(TNeedAudAdaptData &tLeftReqResData)
{	
	CBasChn *pSubSuitableChn = NULL;		//记录次优的BAS通道
	TNeedAudAdaptData tTempLeftData;				//记录次优通道使用后，剩余能力
	TNeedAudAdaptData tTempNeedData;				//临时保存BAS所需资源数据
	memcpy( &tTempNeedData, &tLeftReqResData, sizeof(TNeedAudAdaptData) );
	
	CBasChn *pTmpBasChn = NULL;
	TAudioTypeDesc atSimCap[MAXNUM_CONF_AUDIOTYPE];  
	for ( u8 byIdx = 0; byIdx < m_byRealBasChNum; byIdx++ )
	{
		pTmpBasChn = m_apcBasChn[byIdx];
		if ( NULL == pTmpBasChn )
		{
			continue;
		}

		if ( pTmpBasChn->IsBasChnSupportAudNeeds(tLeftReqResData,atSimCap,FALSE) )
		{	
			//找能力最优的通道，找不到先记录该通道，以便找不到时可以使用次优通道
			if (tLeftReqResData.m_byNeedAudEncNum ==0 )
			{
				pTmpBasChn->SetIsReserved(TRUE);
				pTmpBasChn->SetMediaMode(MODE_AUDIO);
				return pTmpBasChn;		//返回最优通道
			}
			else
			{
				pSubSuitableChn = pTmpBasChn;	//记录次优通道
				memcpy( &tTempLeftData,&tLeftReqResData,   sizeof(TNeedAudAdaptData) );	//记录次优剩余数据
				memcpy( &tLeftReqResData, &tTempNeedData, sizeof(TNeedAudAdaptData) );	//恢复数据继续查找
			}
			
		}
	}

	//判断是否有次优通道可使用
	if ( pSubSuitableChn != NULL)
	{
		pSubSuitableChn->SetIsReserved(TRUE);		   //预占
		pSubSuitableChn->SetMediaMode(MODE_AUDIO);
		memcpy( &tLeftReqResData, &tTempLeftData, sizeof(TNeedAudAdaptData) );	//返回剩余数据
		return pSubSuitableChn;			//返回通道
	}
	
	memcpy( &tLeftReqResData, &tTempNeedData, sizeof(TNeedAudAdaptData) );	//恢复数据
	return NULL;
}

/*=============================================================================
函 数 名： GetBasChnsForBrd
功    能： 获得通道
算法实现： 
全局变量： 
参    数：TNeedBasResData &tLeftReqResData 会议所需能力集描述,视频，音频，双流

返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2012/04/18  4.7			倪志俊                 创建
=============================================================================*/
BOOL32 CNPlusBasChnListMgr::GetBasChnsForBrd(TNeedVidAdaptData *ptReqResData)
{
	//编码路数为0，说明不需要适配
	if ( ptReqResData->m_byRealEncNum == 0 )
	{
		return TRUE;
	}
	
	if ( NULL == ptReqResData )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[CNPlusBasChnListMgr::GetBasChnsForBrd]ptReqResData is NULL\n");
		return FALSE;
	}
	
	TNeedVidAdaptData	tTempLeftNeedData;
	memcpy( &tTempLeftNeedData, ptReqResData, sizeof(TNeedVidAdaptData) );
	CBasChn *pCBasChn = NULL;	
	while(tTempLeftNeedData.m_byRealEncNum)
	{
		pCBasChn = GetOneIdleSuitableBasChn(tTempLeftNeedData);
		if ( NULL == pCBasChn )
		{
			return FALSE;
		}
	}
	return TRUE;
}

/*=============================================================================
函 数 名： GetAudBasChnsForBrd
功    能： 获得通道
算法实现： 
全局变量： 
参    数：TNeedAudAdaptData &ptReqAudResource 会议所需能力集描述,音频

返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2013/03/29  4.7			倪志俊                 创建
=============================================================================*/
BOOL32 CNPlusBasChnListMgr::GetAudBasChnsForBrd(TNeedAudAdaptData *ptReqAudResource)
{
	if ( NULL == ptReqAudResource )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[CNPlusBasChnListMgr::GetBasChnsForBrd]ptReqAudResource is NULL\n");
		return FALSE;
	}

	//编码路数为0，说明不需要适配
	if ( ptReqAudResource->m_byNeedAudEncNum == 0 )
	{
		return TRUE;
	}
	

	
	TNeedAudAdaptData	tTempLeftNeedData;
	memcpy( &tTempLeftNeedData, ptReqAudResource, sizeof(TNeedAudAdaptData) );
	CBasChn *pCBasChn = NULL;	
	while(tTempLeftNeedData.m_byNeedAudEncNum)
	{
		pCBasChn = GetOneIdelSuitableAudBasChn(tTempLeftNeedData);
		if ( NULL == pCBasChn )
		{
			return FALSE;
		}
	}
	return TRUE;
}

/*=============================================================================
函 数 名： CheckBasEnoughForReq
功    能： 检查是否有足够的通道召开会议
算法实现： 
全局变量： 
参    数：TNeedBasResData &tLeftReqResData 会议所需能力集描述,视频，音频，双流

返 回 值： BOOL32 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2012/04/18  4.7			倪志俊                 创建
=============================================================================*/
BOOL32 CNPlusBasChnListMgr::CheckBasEnoughForReq(TNeedVidAdaptData *ptReqResData,TNeedAudAdaptData *ptReqAudResource)
{
	if ( NULL == ptReqResData || NULL == ptReqAudResource)
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[CNPlusBasChnListMgr::CheckBasEnoughForReq]ptReqResData or pReqAudResource is NULL\n");
		return FALSE;
	}

	BOOL32	bIsOccupyOk = FALSE;
	//先预占音频
	if ( GetAudBasChnsForBrd(ptReqAudResource) )
	{
		bIsOccupyOk = TRUE;
	}
	else
	{
		return FALSE;
	}

	//再预占视频
	u8		byOccupyNum = 0;		//记录已经预占的通道个数
	for ( u8 byIdx = 0; byIdx < emMODE_END; byIdx++ )
	{
		u8 byTempNum = 0;
		if ( GetBasChnsForBrd( &ptReqResData[byIdx] ) 
			)
		{ 
			bIsOccupyOk = TRUE;
		}
		else
		{
			bIsOccupyOk = FALSE;
			break;
		}
	}
	
	
	//预占不成功
	if (!bIsOccupyOk)
	{
		return FALSE;
	}

	return TRUE;
}

/*====================================================================
函数名        GetBasCapConfNeed
功能        ：获取会议编解码能力集
算法实现    ：
引用全局变量：
输入参数说明：[in]const TConfInfo &tConfInfo		  会议信息
			  [in]const TConfInfoEx &tConfInfoEx,     会议勾选额外信息
			  [out]TNeedBasResData *ptMVReqResource   返回编解码信息
			  [in] u8  byMediaNum					  媒体类型个数							
			  
返回值说明  ：成功获得编解码信息返回TRUE,失败返回FALSE
					
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2011/12/26  4.7			倪志俊		   创建
====================================================================*/
BOOL32 GetBasCapConfNeedForNPlus(const TConfInfo &tConfInfo,const TConfInfoEx &tConfInfoEx, 
									 TNeedVidAdaptData *ptReqResource, TNeedAudAdaptData *pReqAudResource, 
									 BOOL32 bIsChn8KH/* = FALSE*/,BOOL32 bIsChn8KE/* = FALSE*/,
									 BOOL32 bIsChn8KI/* = FALSE*/)
{	
	if ( ptReqResource == NULL || pReqAudResource == NULL )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[GetBasCapConfNeedForNPlus]ptReqResource or pReqAudResource is NULL!\n");
		return FALSE;
	}

	//1.获得会议解码能力(获得主流视频与双流能力解码能力，音频解码能力最后获得)
	u8 byIdx = 0;
	for ( byIdx = emMODE_VIDEO; byIdx < (emMODE_END); byIdx++ )
	{
		if ( byIdx == emMODE_VIDEO && tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_H261 )
		{
			ptReqResource[byIdx].m_byDecNeedRes = (u8)1 << CBasChn::emDecH261;
		}
		else if (tConfInfo.GetProfileAttrb() == emHpAttrb)
		{
			//8kh双流通道支持解HP，但不支持解高帧率，所以这里针对HP做特殊处理，记录解码能力emDecHighFps
			if ( bIsChn8KH )
			{
				if ( byIdx == emMODE_SECVIDEO && IsDSFR50OR60(tConfInfo))
				{
					ptReqResource[byIdx].m_byDecNeedRes = (u8)1 <<CBasChn::emDecHighFps;
				}
				else
				{
					ptReqResource[byIdx].m_byDecNeedRes = (u8)1 <<CBasChn::emDecHp;
				}
			}
			else
			{
				ptReqResource[byIdx].m_byDecNeedRes = (u8)1 <<CBasChn::emDecHp;
			}
		}
		else if ( tConfInfo.GetProfileAttrb() == emBpAttrb )
		{
			BOOL32 bHighFps = FALSE;
			if ( byIdx == emMODE_VIDEO )
			{
				bHighFps= IsConfFR50OR60(tConfInfo);
			}
			else if ( byIdx == emMODE_SECVIDEO )
			{
				bHighFps= IsDSFR50OR60(tConfInfo);
			}

			BOOL32 bMainMediaH264   =  tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_H264;
			BOOL32 bSecMediaH264	=  tConfInfo.GetSecVideoMediaType() != MEDIA_TYPE_NULL &&
									   tConfInfo.GetSecVideoMediaType() == MEDIA_TYPE_H264;
			if(bHighFps)
			{
				//双流的解码能力做下特殊处理，为了兼容以前的规格
				//以前：高帧率会议 双流同主视频 才认为需要highfps解码能力的通道，否则其他情况只占hd解码能力的即可
				//现在：现在没有同主视频的适配，故认为只要双流主格式不是50/60的话，解码能力可修改成hd
				if ( byIdx ==emMODE_SECVIDEO &&
					tConfInfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetUserDefFrameRate() <= 30)
				{
					ptReqResource[byIdx].m_byDecNeedRes= (u8)1 <<CBasChn::emDecHD;
				}
				else
				{
					ptReqResource[byIdx].m_byDecNeedRes= (u8)1 <<CBasChn::emDecHighFps;
				}
			}
			else if((IsResGE(tConfInfo.GetMainVideoFormat(), VIDEO_FORMAT_4CIF) && bMainMediaH264) ||
				(IsResGE(tConfInfo.GetSecVideoFormat(), VIDEO_FORMAT_4CIF) && bSecMediaH264))
			{
				ptReqResource[byIdx].m_byDecNeedRes = (u8)1 <<CBasChn::emDecHD;
			}
			else
			{
				ptReqResource[byIdx].m_byDecNeedRes = (u8)1 <<CBasChn::emDecSD;
			}
		}
		else
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_NPLUS, "[GetBasCapConfNeedForNPlus]tConfInfo.GetProfileAttrb() failed!\n");
			return FALSE;
		}
	}

	//2. 获得主流视频所需编码能力
	u8	byStartIdx = 0;		//设置编码idx的起始位置记录
	ptReqResource[emMODE_VIDEO].m_byMediaMode = MODE_VIDEO;

	//预留主格式
	if ( IsConfNeedReserveMainCap(tConfInfo,tConfInfoEx) )
	{
		//8000H 主格式1080 60，由于能力限制，需要将帧率减半作假
		TVideoStreamCap tTempVideoCap = tConfInfo.GetMainSimCapSet().GetVideoCap();
		if (bIsChn8KH)
		{
			if ( tConfInfo.GetProfileAttrb() == emHpAttrb )
			{	
				// HP的话能力填 720 30 HP
				tTempVideoCap.SetResolution(VIDEO_FORMAT_HD720);
				tTempVideoCap.SetH264ProfileAttrb(emHpAttrb);
				if (  tTempVideoCap.GetUserDefFrameRate() == 60 ||
					tTempVideoCap.GetUserDefFrameRate() == 50
					)
				{
					tTempVideoCap.SetUserDefFrameRate( tTempVideoCap.GetUserDefFrameRate()/2 );
				}				
			}
			ptReqResource[emMODE_VIDEO].SetVieoSimCapSet( byStartIdx, tTempVideoCap );
		}
		else if( bIsChn8KI )
		{
			if ( tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
				tConfInfo.GetMainVideoFormat() == VIDEO_FORMAT_HD1080 &&
				(tConfInfo.GetMainVidUsrDefFPS() > 30 ) 
				)
			{
				tTempVideoCap.SetUserDefFrameRate( tTempVideoCap.GetUserDefFrameRate()/2 );
			
			}			
			ptReqResource[emMODE_VIDEO].SetVieoSimCapSet( byStartIdx, tTempVideoCap );						
		}
		else
		{
			ptReqResource[emMODE_VIDEO].SetVieoSimCapSet( byStartIdx, tTempVideoCap );
		}	
		byStartIdx++;
		LogPrint(LOG_LVL_DETAIL,MID_MCU_NPLUS,"[GetBasCapConfNeedForNPlus]reserve MainVideoCap:mediatype:%d-res:%d-profile:%d-userframe:%d-biterate:%d !\n",
				tTempVideoCap.GetMediaType(), 
				tTempVideoCap.GetResolution(),
				tTempVideoCap.GetH264ProfileAttrb(),
				tTempVideoCap.GetUserDefFrameRate(),
				tTempVideoCap.GetMaxBitRate()
					);	
	}

	//获得主流模板勾选
	TVideoStreamCap tVideoCap[MAX_CONF_CAP_EX_NUM];
	u8 byCapNUm = MAX_CONF_CAP_EX_NUM;
	if ( tConfInfoEx.GetMainStreamCapEx(tVideoCap,byCapNUm) )
	{
		for ( byIdx = 0; byIdx<byCapNUm; byIdx++)
		{
			if ( tVideoCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL )
			{
				continue;
			}
			//8000H由于能力限制，如果勾选的是720 60的话，需要将帧率减半
			if (bIsChn8KH)
			{
				//勾选有HP过滤掉，因为主格式预留逻辑中已经强行预留720 30 HP,此处无需预留
				if ( tVideoCap[byIdx].GetH264ProfileAttrb() == emHpAttrb )
				{
					continue;
				}
			}
			else if( bIsChn8KI )
			{
				if ( tVideoCap[byIdx].GetMediaType()== MEDIA_TYPE_H264 &&
					( tVideoCap[byIdx].GetResolution() == VIDEO_FORMAT_HD1080 || 
						tVideoCap[byIdx].GetResolution() == VIDEO_FORMAT_HD720 ) &&
					tVideoCap[byIdx].GetUserDefFrameRate() > 30 
					)
				{
					tVideoCap[byIdx].SetUserDefFrameRate( tVideoCap[byIdx].GetUserDefFrameRate()/2 );					
				}
				
				ptReqResource[emMODE_VIDEO].SetVieoSimCapSet( byStartIdx,tVideoCap[byIdx] );				
			}			
			else
			{
				//设置勾选能力
				ptReqResource[emMODE_VIDEO].SetVieoSimCapSet(byStartIdx,tVideoCap[byIdx]);
			}
			byStartIdx++;	
			LogPrint(LOG_LVL_DETAIL,MID_MCU_NPLUS,"[GetBasCapConfNeedForNPlus]reserve video cap:mediatype:%d-res:%d-profile:%d-userframe:%d-biterate:%d !\n",
				tVideoCap[byIdx].GetMediaType(), 
				tVideoCap[byIdx].GetResolution(),
				tVideoCap[byIdx].GetH264ProfileAttrb(),
				tVideoCap[byIdx].GetUserDefFrameRate(),
				tVideoCap[byIdx].GetMaxBitRate()
					);	
		}
	}
	else
	{
		LogPrint(LOG_LVL_ERROR,MID_MCU_BAS,"[GetBasCapConfNeedForNPlus]tConfInfoEx.GetMainStreamCapEx failed!\n");
		return FALSE;
	}

	//记录编码路数
	ptReqResource[emMODE_VIDEO].m_byRealEncNum = byStartIdx;

	//最后预留辅格式	
	if ( tConfInfo.GetSecVideoMediaType()!= MEDIA_TYPE_NULL )
	{
		ptReqResource[emMODE_VIDEO].SetVieoSimCapSet( byStartIdx, tConfInfo.GetSecSimCapSet().GetVideoCap() );
		byStartIdx++;
		ptReqResource[emMODE_VIDEO].m_byRealEncNum = byStartIdx;
		LogPrint(LOG_LVL_DETAIL,MID_MCU_NPLUS,"[GetBasCapConfNeedForNPlus]reserve SecVideoCap:mediatype:%d-res:%d-profile:%d-userframe:%d-biterate:%d !\n",
			tConfInfo.GetSecSimCapSet().GetVideoMediaType(), 
			tConfInfo.GetSecSimCapSet().GetVideoResolution(),
			tConfInfo.GetSecSimCapSet().GetVideoProfileType(),
			tConfInfo.GetSecSimCapSet().GetUserDefFrameRate(),
			tConfInfo.GetSecSimCapSet().GetVideoMaxBitRate()
			);	
	}

	//如果同时存在720 30fps 和720 60fps 则去除该路720 60fps的idx
	if( !bIsChn8KI )
	{
		if ( bIsChn8KH )
		{
			ptReqResource[emMODE_VIDEO].Filter720P60FPSOnlyFor8KH();
		}
		else
		{
			ptReqResource[emMODE_VIDEO].Filter720P60FPS();
		}
	}	
	
	//3.获得双流所需编码能力
	byStartIdx = 0;
	ptReqResource[emMODE_SECVIDEO].m_byMediaMode = MODE_SECVIDEO;
	//预留主格式
	if ( IsConfNeedReserveDSMainCap(tConfInfo,tConfInfoEx) )
	{
		if ( IsConfDualEqMV(tConfInfo) )//同主视频双速会议预留主流主格式
		{
			ptReqResource[emMODE_SECVIDEO].SetVieoSimCapSet(byStartIdx, tConfInfo.GetMainSimCapSet().GetVideoCap());	
			if ( bIsChn8KE || bIsChn8KH  )
			{
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetResolution(VIDEO_FORMAT_CIF);
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetUserDefFrameRate(25);
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetH264ProfileAttrb(emBpAttrb);
			}
			
			if( bIsChn8KI )
			{
				if( ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetResolution() >= VIDEO_FORMAT_HD720 )
				{
					u8 byFrameRate = ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetUserDefFrameRate();
					if( byFrameRate >= 50 )
					{
						byFrameRate = byFrameRate / 2;
					}
					ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetResolution(VIDEO_FORMAT_HD720);
					ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetUserDefFrameRate(byFrameRate);
					ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetH264ProfileAttrb(emBpAttrb);
				}				
			}

			LogPrint(LOG_LVL_DETAIL,MID_MCU_BAS,"[GetBasCapConfNeed]reserve DSDualMainCap:mediatype:%d-res:%d-profile:%d-userframe:%d-biterate:%d !\n",
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetMediaType(),
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetResolution(),
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetH264ProfileAttrb(),
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetUserDefFrameRate(),
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetMaxBitRate()
				);
			byStartIdx++;
		}
		else
		{
			ptReqResource[emMODE_SECVIDEO].SetVieoSimCapSet(byStartIdx, tConfInfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap());
			if ( ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetResolution() == VIDEO_FORMAT_VGA ||
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetResolution() == VIDEO_FORMAT_SVGA
				)
			{
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetResolution(VIDEO_FORMAT_XGA);
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetUserDefFrameRate(5);
			}
			
			if ( bIsChn8KE || bIsChn8KH )
			{
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetResolution(VIDEO_FORMAT_XGA);
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetUserDefFrameRate(5);
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetH264ProfileAttrb(emBpAttrb);
			}

			if ( bIsChn8KI )
			{
				if( ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetMediaType() == MEDIA_TYPE_H264 )
				{
					emProfileAttrb emProfile = emHpAttrb;
					if( IsConfHasBpCapOrBpExCap(tConfInfo,tConfInfoEx) )
					{
						emProfile = emBpAttrb;
					}
					
					if( Is8KINeedDsAdaptH264SXGA(tConfInfo) )
					{
						ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetResolution(VIDEO_FORMAT_SXGA);
						ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetUserDefFrameRate(20);					
						ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetH264ProfileAttrb(emProfile);						
					}
					else
					{
						if( tConfInfoEx.IsDSSupportCapEx( VIDEO_FORMAT_XGA,5,emBpAttrb ) )
						{						
							ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetResolution(VIDEO_FORMAT_XGA);
							ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetUserDefFrameRate(5);
							ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetH264ProfileAttrb(emBpAttrb);						
						}
						else if(  tConfInfoEx.IsDSSupportCapEx( VIDEO_FORMAT_XGA,5,emHpAttrb ) )
						{
							ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetResolution(VIDEO_FORMAT_XGA);
							ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetUserDefFrameRate(5);
							ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetH264ProfileAttrb(emHpAttrb);
						}
					}		
				}				
			}

			LogPrint(LOG_LVL_DETAIL,MID_MCU_BAS,"[GetBasCapConfNeed]reserve DSMainCap:mediatype:%d-res:%d-profile:%d-userframe:%d-biterate:%d !\n",
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetMediaType(),
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetResolution(),
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetH264ProfileAttrb(),
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetUserDefFrameRate(),
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].GetMaxBitRate()
				);	
			byStartIdx++;
		}
	}

	//获得双流模板勾选
	if ( !bIsChn8KE && !bIsChn8KH && !bIsChn8KI )
	{
		TVideoStreamCap tSecVideoCap[MAX_CONF_CAP_EX_NUM];
		u8 byDSCapNum = MAX_CONF_CAP_EX_NUM;
		if ( tConfInfoEx.GetDoubleStreamCapEx(tSecVideoCap,byDSCapNum) )
		{
			for ( byIdx = 0; byIdx<byDSCapNum; byIdx++)
			{
				if ( tSecVideoCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL )
				{
					continue;
				}
				//设置勾选能力
				ptReqResource[emMODE_SECVIDEO].SetVieoSimCapSet(byStartIdx,tSecVideoCap[byIdx]);
				byStartIdx++;
				
				LogPrint(LOG_LVL_DETAIL,MID_MCU_NPLUS,"[GetBasCapConfNeedForNPlus]reserve ds video cap:mediatype:%d-res:%d-profile:%d-userframe:%d-biterate:%d !\n",
					tSecVideoCap[byIdx].GetMediaType(), 
					tSecVideoCap[byIdx].GetResolution(),
					tSecVideoCap[byIdx].GetH264ProfileAttrb(),
					tSecVideoCap[byIdx].GetUserDefFrameRate(),
					tSecVideoCap[byIdx].GetMaxBitRate()
					);	
			}
		}
		else
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_NPLUS,"[GetBasCapConfNeedForNPlus]tConfInfoEx.GetDoubleStreamCapEx failed!\n");
			return FALSE;
		}		
		//记录编码路数
		ptReqResource[emMODE_SECVIDEO].m_byRealEncNum = byStartIdx;
	}

	//双双流预留 辅格式
	BOOL32  bDoubleDual= IsConfDoubleDual(tConfInfo);
	if ( bDoubleDual && tConfInfo.GetDStreamUsrDefFPS() >= 5 )
	{
		//辅格式双流预留
		ptReqResource[emMODE_SECVIDEO].SetVieoSimCapSet(byStartIdx,tConfInfo.GetCapSupportEx().GetSecDSVideoCap());
		//H263+使用宏定义，在BAS外设处会根据宏调整成实际帧率，目前都是5帧,
		ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetFrameRate(VIDEO_FPS_2997_6);
		//H263+ 分辨率此处调整成XGA
		ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetResolution(VIDEO_FORMAT_XGA);
		byStartIdx++;
		ptReqResource[emMODE_SECVIDEO].m_byRealEncNum = byStartIdx;
		LogPrint(LOG_LVL_DETAIL,MID_MCU_NPLUS,"[GetBasCapConfNeedForNPlus]reserve DoubleDual\n");
	}
	else if( bIsChn8KI )
	{
		if( Is8KINeedDsAdaptH264SXGA(tConfInfo) )
		{
			if( tConfInfoEx.IsDSSupportCapEx( VIDEO_FORMAT_XGA,5,emBpAttrb ) )
			{
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetMediaType( MEDIA_TYPE_H264 );
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetResolution(VIDEO_FORMAT_XGA);
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetUserDefFrameRate(5);
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetH264ProfileAttrb(emBpAttrb);
				byStartIdx++;
				ptReqResource[emMODE_SECVIDEO].m_byRealEncNum = byStartIdx;
			}
			else if( tConfInfoEx.IsDSSupportCapEx( VIDEO_FORMAT_XGA,5,emHpAttrb ) )
			{
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetMediaType( MEDIA_TYPE_H264 );
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetResolution(VIDEO_FORMAT_XGA);
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetUserDefFrameRate(5);
				ptReqResource[emMODE_SECVIDEO].m_atSimCapSet[byStartIdx].SetH264ProfileAttrb(emHpAttrb);
				byStartIdx++;
				ptReqResource[emMODE_SECVIDEO].m_byRealEncNum = byStartIdx;
			}
		}
	}

	//如果同时存在720 30fps 和720 60fps 则去除该路720 60fps的idx
	ptReqResource[emMODE_SECVIDEO].Filter720P60FPS();
	
	//4.音频适配获得
	u8 byAudTypeNum = 0;
	TAudioTypeDesc atAudTypeDesc[MAXNUM_CONF_AUDIOTYPE];
	byAudTypeNum = tConfInfoEx.GetAudioTypeDesc(atAudTypeDesc);
	if ( byAudTypeNum > 1 ) //音频能力集个数大于1，说明需要音频适配
	{
		memcpy(pReqAudResource->m_atAudTypeDesc,atAudTypeDesc,sizeof(atAudTypeDesc));
		pReqAudResource->m_byNeedAudEncNum = byAudTypeNum-1;		
		for ( u8 byLoop=0 ; byLoop<byAudTypeNum; byLoop++ )
		{
			LogPrint(LOG_LVL_DETAIL,MID_MCU_BAS,"[GetBasCapConfNeedForNPlus]conf all audiotype:mediatype-%d-tracknum-%d\n",atAudTypeDesc[byLoop].GetAudioMediaType(), atAudTypeDesc[byLoop].GetAudioTrackNum() );	
		}
	}
	else
	{
		pReqAudResource->m_byNeedAudEncNum = 0;
	}

	return TRUE;
}

/*====================================================================
    函数名      : TransEncodingOfUsrGrps
    功能        : 用户组 转码
    算法实现    :
    引用全局变量:
    输入参数说明: 
    返回值说明  :
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人      修改内容
    2013/05/09              liaokang      创建
====================================================================*/
inline void TransEncodingOfUsrGrps(CUsrGrpsInfo &cUsrGrpsInfo, u8 byTransEncodingType)
{
    u16 wLoop = 0;
    s8 achTmp[2*MAX_CHARLENGTH] = {0};     
    switch( byTransEncodingType )
    {
    case TRANSENCODING_UTF8_GBK:
        {
            for ( wLoop = 0; wLoop < MAXNUM_USRGRP; wLoop++)
            {
                if ( cUsrGrpsInfo.m_atInfo[wLoop].IsFree() )
                {
                    continue;
                }
                
                memset(achTmp, 0, sizeof(achTmp));
                utf8_to_gb2312(cUsrGrpsInfo.m_atInfo[wLoop].GetUsrGrpName(), achTmp, MAX_CHARLENGTH-1);
                cUsrGrpsInfo.m_atInfo[wLoop].SetUsrGrpName(achTmp);        
                
                memset(achTmp, 0, sizeof(achTmp));
                utf8_to_gb2312(cUsrGrpsInfo.m_atInfo[wLoop].GetUsrGrpDesc(), achTmp, 2*MAX_CHARLENGTH-1);
                cUsrGrpsInfo.m_atInfo[wLoop].SetUsrGrpDesc(achTmp);
            }
        }
        break;
    case TRANSENCODING_GBK_UTF8:
        {
            for ( wLoop = 0; wLoop < MAXNUM_USRGRP; wLoop++)
            {
                if ( cUsrGrpsInfo.m_atInfo[wLoop].IsFree() )
                {
                    continue;
                }
                
                memset(achTmp, 0, sizeof(achTmp));
                gb2312_to_utf8(cUsrGrpsInfo.m_atInfo[wLoop].GetUsrGrpName(), achTmp, MAX_CHARLENGTH-1);
                cUsrGrpsInfo.m_atInfo[wLoop].SetUsrGrpName(achTmp);        
                
                memset(achTmp, 0, sizeof(achTmp));
                gb2312_to_utf8(cUsrGrpsInfo.m_atInfo[wLoop].GetUsrGrpDesc(), achTmp, 2*MAX_CHARLENGTH-1);
                cUsrGrpsInfo.m_atInfo[wLoop].SetUsrGrpDesc(achTmp);
            }
        }
        break;
    default :
        break;
    } 
}

/*====================================================================
    函数名      : TransEncodingOfUsrInfo
    功能        : 用户信息 转码
    算法实现    :
    引用全局变量:
    输入参数说明: 
    返回值说明  :
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人      修改内容
    2013/05/09              liaokang      创建
====================================================================*/
inline void TransEncodingOfUsrInfo(CExUsrInfo &cExUsrInfo, u8 byTransEncodingType)
{
    if ( cExUsrInfo.IsEqualName("admin"))
    {
        return; // Mcu在使用的时候会把discription的首字节当 组ID 用， [pengguofeng 4/25/2013]
        //对于0~127之间的没问题，但是对于255这样的就会被当成非法字母，被强行转了
    }

    s8 achTmp[2*MAX_CHARLENGTH] = {0};     
    switch( byTransEncodingType )
    {
    case TRANSENCODING_UTF8_GBK:
        {
            memset(achTmp, 0, sizeof(achTmp));
            utf8_to_gb2312(cExUsrInfo.name, achTmp, MAX_CHARLENGTH-1);
            cExUsrInfo.SetName(achTmp);
            
            memset(achTmp, 0, sizeof(achTmp));
            utf8_to_gb2312(cExUsrInfo.fullname, achTmp, MAX_CHARLENGTH-1);
            cExUsrInfo.SetFullName(achTmp);
            
            memset(achTmp, 0, sizeof(achTmp));
            utf8_to_gb2312(cExUsrInfo.discription, achTmp, 2*MAX_CHARLENGTH-1);
            cExUsrInfo.SetDiscription(achTmp);            
        } 
        break;
    case TRANSENCODING_GBK_UTF8:
        {
            memset(achTmp, 0, sizeof(achTmp));
            gb2312_to_utf8(cExUsrInfo.name, achTmp, MAX_CHARLENGTH-1);
            cExUsrInfo.SetName(achTmp);
            
            memset(achTmp, 0, sizeof(achTmp));
            gb2312_to_utf8(cExUsrInfo.fullname, achTmp, MAX_CHARLENGTH-1);
            cExUsrInfo.SetFullName(achTmp);
            
            memset(achTmp, 0, sizeof(achTmp));
            gb2312_to_utf8(cExUsrInfo.discription, achTmp, 2*MAX_CHARLENGTH-1);
            cExUsrInfo.SetDiscription(achTmp);
        } 
        break;
    default :
        break;
    }
}
/*====================================================================
    函数名      : TransEncodingOfNPlusConfData
    功能        : NPlusConfData转码
    算法实现    :
    引用全局变量:
    输入参数说明: 
    返回值说明  :
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人      修改内容
    2013/05/09              liaokang      创建
====================================================================*/
inline void TransEncodingOfNPlusConfData(TNPlusConfData &tConfData, u8 byTransEncodingType)
{
    if ( tConfData.IsNull() )
    {
        return;
    } 

    u16 wLoop = 0;
    s8 achTmp[MAXLEN_ALIAS+1] = {0};     
    switch( byTransEncodingType )
    {
    case TRANSENCODING_UTF8_GBK:
        {
            //转mcu上会议信息
            //m_tConf        
            //会议名m_achConfName[MAXLEN_CONFNAME+1]
            memset(achTmp, 0, sizeof(achTmp));
            utf8_to_gb2312(tConfData.m_tConf.GetConfName(), achTmp, MAXLEN_CONFNAME);
            tConfData.m_tConf.SetConfName(achTmp);        
            //会议密码m_achConfPwd[MAXLEN_PWD+1]
            memset(achTmp, 0, sizeof(achTmp));
            utf8_to_gb2312(tConfData.m_tConf.GetConfPwd(), achTmp, MAXLEN_PWD);
            tConfData.m_tConf.SetConfPwd(achTmp);
            //会议中主席的别名m_tChairAlias
            memset(achTmp, 0, sizeof(achTmp));
            TMtAlias tMtAlias;
            tMtAlias = tConfData.m_tConf.GetChairAlias();
            utf8_to_gb2312(tMtAlias.m_achAlias, achTmp, MAXLEN_ALIAS-1);
            memcpy(tMtAlias.m_achAlias, achTmp, MAXLEN_ALIAS-1);
            tConfData.m_tConf.SetChairAlias(tMtAlias);
            //会议中发言人的别名m_tSpeakerAlias
            memset(achTmp, 0, sizeof(achTmp));
            tMtAlias.SetNull();
            tMtAlias = tConfData.m_tConf.GetSpeakerAlias();
            utf8_to_gb2312(tMtAlias.m_achAlias, achTmp, MAXLEN_ALIAS-1);
            memcpy(tMtAlias.m_achAlias, achTmp, MAXLEN_ALIAS-1);
            tConfData.m_tConf.SetSpeakerAlias(tMtAlias);
            //m_tMediaKey
            memset(achTmp, 0, sizeof(achTmp));
            TMediaEncrypt tMediaKey = tConfData.m_tConf.GetMediaKey();
            s8  abyEncKey[MAXLEN_KEY] = {0};
            s32 nKey = 0;
            u32 dwKey = 0;
            //DES加密
            if( CONF_ENCRYPTMODE_DES == tMediaKey.GetEncryptMode() )
            {
                nKey = LEN_DES;
                tMediaKey.GetEncryptKey((u8*)abyEncKey, &nKey);
                dwKey = utf8_to_gb2312(abyEncKey, achTmp, MAXLEN_KEY-1);
                tMediaKey.SetEncryptKey((u8*)achTmp, dwKey);
                tConfData.m_tConf.SetMediaKey(tMediaKey); 
            }
            //AES加密
            else if( CONF_ENCRYPTMODE_AES == tMediaKey.GetEncryptMode() )
            {
                nKey = LEN_AES;
                tMediaKey.GetEncryptKey((u8*)abyEncKey, &nKey);
                dwKey = utf8_to_gb2312(abyEncKey, achTmp, MAXLEN_KEY-1);
                tMediaKey.SetEncryptKey((u8*)achTmp, dwKey);
                tConfData.m_tConf.SetMediaKey(tMediaKey); 
            }
        }
        break;
    case TRANSENCODING_GBK_UTF8:
        {
            //转mcu上会议信息
            //m_tConf        
            //会议名m_achConfName[MAXLEN_CONFNAME+1]
            memset(achTmp, 0, sizeof(achTmp));
            gb2312_to_utf8(tConfData.m_tConf.GetConfName(), achTmp, MAXLEN_CONFNAME);
            tConfData.m_tConf.SetConfName(achTmp);        
            //会议密码m_achConfPwd[MAXLEN_PWD+1]
            memset(achTmp, 0, sizeof(achTmp));
            gb2312_to_utf8(tConfData.m_tConf.GetConfPwd(), achTmp, MAXLEN_PWD);
            tConfData.m_tConf.SetConfPwd(achTmp);
            //会议中主席的别名m_tChairAlias
            memset(achTmp, 0, sizeof(achTmp));
            TMtAlias tMtAlias;
            tMtAlias = tConfData.m_tConf.GetChairAlias();
            gb2312_to_utf8(tMtAlias.m_achAlias, achTmp, MAXLEN_ALIAS-1);
            memcpy(tMtAlias.m_achAlias, achTmp, MAXLEN_ALIAS-1);
            tConfData.m_tConf.SetChairAlias(tMtAlias);
            //会议中发言人的别名m_tSpeakerAlias
            memset(achTmp, 0, sizeof(achTmp));
            tMtAlias.SetNull();
            tMtAlias = tConfData.m_tConf.GetSpeakerAlias();
            gb2312_to_utf8(tMtAlias.m_achAlias, achTmp, MAXLEN_ALIAS-1);
            memcpy(tMtAlias.m_achAlias, achTmp, MAXLEN_ALIAS-1);
            tConfData.m_tConf.SetSpeakerAlias(tMtAlias);
            //m_tMediaKey
            memset(achTmp, 0, sizeof(achTmp));
            TMediaEncrypt tMediaKey = tConfData.m_tConf.GetMediaKey();
            s8  abyEncKey[MAXLEN_KEY] = {0};
            s32 nKey = 0;
            u32 dwKey = 0;
            //DES加密
            if( CONF_ENCRYPTMODE_DES == tMediaKey.GetEncryptMode() )
            {
                nKey = LEN_DES;
                tMediaKey.GetEncryptKey((u8*)abyEncKey, &nKey);
                dwKey = gb2312_to_utf8(abyEncKey, achTmp, MAXLEN_KEY-1);
                tMediaKey.SetEncryptKey((u8*)achTmp, dwKey);
                tConfData.m_tConf.SetMediaKey(tMediaKey); 
            }
            //AES加密
            else if( CONF_ENCRYPTMODE_AES == tMediaKey.GetEncryptMode() )
            {
                nKey = LEN_AES;
                tMediaKey.GetEncryptKey((u8*)abyEncKey, &nKey);
                dwKey = gb2312_to_utf8(abyEncKey, achTmp, MAXLEN_KEY-1);
                tMediaKey.SetEncryptKey((u8*)achTmp, dwKey);
                tConfData.m_tConf.SetMediaKey(tMediaKey); 
            }
        } 
        break;
    default :
        break;
    }    
}

//END OF FILE
