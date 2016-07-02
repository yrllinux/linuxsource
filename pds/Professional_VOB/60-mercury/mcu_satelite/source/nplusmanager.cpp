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
#include "mcuerrcode.h"
#include "evmcumt.h"
#include "mtadpssn.h"

CNPlusApp g_cNPlusApp;

//CNPlusInst
//construct
CNPlusInst::CNPlusInst( void ):m_dwMcuNode(INVALID_NODE), 
                               m_dwMcuIId(0), 
                               m_dwMcuIpAddr(0),
                               m_wRtdFailTimes(0),
                               m_byUsrNum(0),
                               m_wRegNackByCapTimes(0)
{
    memset(&m_cUsrGrpsInfo, 0, sizeof(m_cUsrGrpsInfo));
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
        OspPrintf( TRUE, FALSE, "[CNPlusInst::InstanceEntry] invalid msg body!\n" );
        return;
    }

    switch( pcMsg->event )
    {  
    case OSP_DISCONNECT:
        ProcDisconnect( pcMsg );
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
    case MCU_NPLUS_CHAIRUPDATE_REQ:
    case MCU_NPLUS_SPEAKERUPDATE_REQ:
    case MCU_NPLUS_VMPUPDATE_REQ:
    case MCU_NPLUS_RASINFOUPDATE_REQ:
        ProcMcuDataUpdateReq( pcMsg );
        break;

    case MCU_NPLUS_EQPCAP_NOTIF:
        ProcMcuEqpCapNotif( pcMsg );
        break;

    case MCU_NPLUS_RTD_RSP:
        ProcRtdRsp( pcMsg );
        break;

    case MCU_NPLUS_RTD_TIMER:
        ProcRtdTimeOut( pcMsg );
        break;

        //备份mcu发送到主mcu会议回滚消息
    case VC_NPLUS_MSG_NOTIF:
        ProcConfRollback( pcMsg );
        break;

    case VC_NPLUS_RESET_NOTIF:
        ProcReset( pcMsg );
        break;

    case MCU_NPLUS_CONFROLLBACK_ACK:
    case MCU_NPLUS_CONFROLLBACK_NACK:
    case MCU_NPLUS_USRROLLBACK_ACK:
    case MCU_NPLUS_USRROLLBACK_NACK:
    case MCU_NPLUS_GRPROLLBACK_ACK:
    case MCU_NPLUS_GRPROLLBACK_NACK:
        break;

    default:
        OspPrintf( TRUE, FALSE, "[CNPlusInst::InstanceEntry] invalid event(%d): %s!\n", 
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
    OspPrintf(TRUE, FALSE, "=================NPlusInst: InsID %d================\n", GetInsID());
    OspPrintf(TRUE, FALSE, "McuNode:0x%x McuIId:0x%x McuIp:0x%x\n", m_dwMcuNode, 
              m_dwMcuIId, m_dwMcuIpAddr);
    
    for (byIdx = 0; byIdx < MAXNUM_ONGO_CONF; byIdx++)
    {
        if (m_atConfData[byIdx].IsNull())
        {
            continue;
        }
        m_atConfData[byIdx].m_tConf.Print();

        OspPrintf(TRUE, FALSE, "\nMtInfo in conf:\n");
        for (byLoop = 0; byLoop < m_atConfData[byIdx].m_byMtNum; byLoop++)
        {   
            TMtInfo *ptInfo = &m_atConfData[byIdx].m_atMtInConf[byLoop];            
            OspPrintf(TRUE, FALSE, "%d: 0x%x(%d)  %d(kbs)\n", byLoop,
                      ptInfo->GetMtAddr().GetIpAddr(),
                      ptInfo->GetMtAddr().GetPort(), ptInfo->GetCallBitrate());
        }

        OspPrintf(TRUE, FALSE, "VmpInfo in conf:\n");
        for (byLoop = 0; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++)
        {
            TMtInfo *ptInfo = &m_atConfData[byIdx].m_tVmpInfo.m_atMtInVmp[byLoop];
            if (ptInfo->IsNull())
            {
                continue;
            }
            OspPrintf(TRUE, FALSE, "%d: 0x%x(%d)  %d(kbs) type(%d)\n", byLoop,
                      ptInfo->GetMtAddr().GetIpAddr(),
                      ptInfo->GetMtAddr().GetPort(), ptInfo->GetCallBitrate(), 
                      m_atConfData[byIdx].m_tVmpInfo.m_abyMemberType[byLoop]);
        }
    }

    m_cUsrGrpsInfo.Print();

    OspPrintf(TRUE, FALSE, "user info:\n");
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
    m_dwMcuNode = 0;
    m_dwMcuIId = 0;
    m_dwMcuIpAddr = 0;
    m_byUsrNum = 0;
    m_wRtdFailTimes = 0;
    m_wRegNackByCapTimes = 0;
    memset(m_atConfData, 0, sizeof(m_atConfData));
    memset(&m_cUsrGrpsInfo, 0, sizeof(m_cUsrGrpsInfo));
    memset(m_acUsrInfo, 0, sizeof(m_acUsrInfo));
    memset(&m_tRASInfo, 0, sizeof(m_tRASInfo));
    memset(&m_atChargeInfo, 0, sizeof(m_atChargeInfo));

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
=============================================================================*/
void CNPlusInst::ProcRegNPlusMcuReq( const CMessage * pcMsg )
{
    if (CurState() == STATE_NORMAL)
    {
        OspPrintf(TRUE, FALSE, "[ProcRegNPlusMcuReq] invalid ins state(%d)(id.%d)\n", CurState(), GetInsID());
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);   
    TNPlusMcuRegInfo *ptRegInfo = (TNPlusMcuRegInfo *)cServMsg.GetMsgBody();
    BOOL32 bRollBack = FALSE;
    
    //检查是否为故障恢复mcu 和 本备份mcu是否支持故障恢复
    if (CurState() == STATE_SWITCHED && g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_SWITCH)
    {
        TLocalInfo tLocalInfo;
        if (SUCCESS_AGENT == g_cMcuAgent.GetLocalInfo(&tLocalInfo) && 
            tLocalInfo.GetIsNPlusRollBack() &&
            m_dwMcuIpAddr == ptRegInfo->GetMcuIpAddr())
        {        
            //检查会议恢复所需外设能力，若不满足则拒绝，等待外设能力满足了再进行模式切换
            TNPlusEqpCap tNeedCap, tTmpCap;
            for (u8 byLoop = 0; byLoop < MAXNUM_ONGO_CONF; byLoop++)
            {
                if (!m_atConfData[byLoop].IsNull())
                {
                    tTmpCap = g_cNPlusApp.GetEqpCapFromConf(m_atConfData[byLoop].m_tConf);
                    tNeedCap = tNeedCap + tTmpCap;
                }
            }
            
            tTmpCap = ptRegInfo->GetMcuEqpCap();
            // guzh  [12/13/2006] 检查外设能力，同时要求该MCU上有Mp和MtAdp
            if ( tTmpCap < tNeedCap || 
                 !tTmpCap.HasMp() || 
                 !tTmpCap.HasMtAdp() )
            {
                OspPrintf(TRUE, FALSE, "[ProcRegNPlusMcuReq] Reg MCU(0x%x) eqp cap is not enough to rollback conf\n", 
                          ptRegInfo->GetMcuIpAddr());

                // guzh [12/13/2006]
                // 如果还没有到达拒绝回滚上限
                // 则断开连接，退出。否则强制回滚，防止备份服务器劫持
                m_wRegNackByCapTimes ++;
                // guzh [12/13/2006] 考虑到可能对端会结束会议后无法开始，暂时忽略
                //if ( m_wRegNackByCapTimes  < NPLUS_MAXNUM_REGNACKBYREMOTECAP)
                if (true)
                {                   
                    OspPrintf(TRUE, FALSE, "[ProcRegNPlusMcuReq] Reject m_wRegNackByCapTimes = %d\n", m_wRegNackByCapTimes);
                    OspDisconnectTcpNode(pcMsg->srcnode);
                    return;
                }                
            }
            
            //模式切换 rollback
            bRollBack = TRUE; 
            
            //清除原先保存的会议数据
            ClearInst();
        }
        else
        {
            NPlusLog("[ProcRegNPlusMcuReq] mcu not support N+1 rollback.ignore mcu(0x%x) reg\n", 
                     ptRegInfo->GetMcuIpAddr());
            return;
        }        
    }    

    m_dwMcuIpAddr = ptRegInfo->GetMcuIpAddr();
    m_dwMcuIId = *(u32 *)(cServMsg.GetMsgBody() + sizeof(TNPlusMcuRegInfo));
    m_dwMcuNode = *(u32 *)(cServMsg.GetMsgBody() + sizeof(TNPlusMcuRegInfo) + sizeof(pcMsg->srcid));
    NPlusLog("[ProcRegNPlusMcuReq] McuIpAddr(0x%x) McuIId(%x) McuNode(%d)\n", 
        m_dwMcuIpAddr, m_dwMcuIId, m_dwMcuNode);

    //ack
    u8 byRollBack = bRollBack ? 1 : 0;
    PostReplyBack(pcMsg->event+1, 0, &byRollBack, sizeof(u8) );

    //注册断链接收实例
    OspNodeDiscCBRegQ(m_dwMcuNode, GetAppID(), GetInsID());

    //启动rtd
    SetTimer(MCU_NPLUS_RTD_TIMER, g_cNPlusApp.GetRtdTime()*1000);

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

        // zbq [03/15/2007] 恢复本MCU的注册E164号
        CServMsg cServMsg;
        cServMsg.SetConfIdx( 0 );
        cServMsg.SetEventId( MCU_MT_RESTORE_MCUE164_NTF );
        g_cMtAdpSsnApp.SendMsgToMtAdpSsn( g_cMcuVcApp.GetRegGKDriId(), 
                                          MCU_MT_RESTORE_MCUE164_NTF, cServMsg );
    }
    
    NEXTSTATE(STATE_NORMAL);

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
                if (sizeof(TNPlusConfData) != cServMsg.GetMsgBodyLen())
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(confdata)\n", cServMsg.GetMsgBodyLen());
                    return;
                }                

                TNPlusConfData *ptConfData = (TNPlusConfData *)cServMsg.GetMsgBody();

                if (!SetConfData(ptConfData))
                {
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] set conf data failed.insid.%d\n", GetInsID());
                    return;
                }                
            }            
            break;

        case MCU_NPLUS_CONFINFOUPDATE_REQ:
            {
                if ((sizeof(u8) + sizeof(TConfInfo)) != cServMsg.GetMsgBodyLen())
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(confinfo)\n", cServMsg.GetMsgBodyLen());
                    return;
                }
                
                u8 byMode = *(u8 *)cServMsg.GetMsgBody();
                TConfInfo *ptConf = (TConfInfo *)(cServMsg.GetMsgBody() + sizeof(u8));
                if (!SetConfInfo(ptConf, NPLUS_CONF_START == byMode))
                {
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] set conf info failed.insid.%d\n", GetInsID());
                    return;
                }               
            }            
            break;

        case MCU_NPLUS_USRGRPUPDATE_REQ:
            {
                if ((sizeof(CUsrGrpsInfo)) != cServMsg.GetMsgBodyLen())
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(usrgrp)\n", cServMsg.GetMsgBodyLen());
                    return;
                }               

                CUsrGrpsInfo *pcGrpInfo = (CUsrGrpsInfo *)cServMsg.GetMsgBody();
                if (!SetUsrGrpInfo(pcGrpInfo))
                {
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] set grp info failed.insid.%d\n", GetInsID());
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
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(usrinfo)\n", cServMsg.GetMsgBodyLen());
                    return;
                }
                            
                u8 byUsrNum = cServMsg.GetMsgBodyLen()/sizeof(CExUsrInfo);
                CExUsrInfo *pcUsrInfo = (CExUsrInfo *)cServMsg.GetMsgBody();
                BOOL32 bAdd = (cServMsg.GetCurPktIdx() == 0) ? FALSE : TRUE;
                if (!SetUsrInfo(pcUsrInfo, byUsrNum, bAdd))
                {
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] set usr info failed.insid.%d\n", GetInsID());
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
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(mtinfo)\n", cServMsg.GetMsgBodyLen());
                    return;
                }
              
                TMtInfo *ptMtInfo = (TMtInfo *)cServMsg.GetMsgBody();
                u8 byMtNum = cServMsg.GetMsgBodyLen()/sizeof(TMtInfo);

                if (!SetConfMtInfo(cConfId, ptMtInfo, byMtNum))
                {
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] set mt info failed. insid.%d\n", GetInsID());
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
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(chair)\n", cServMsg.GetMsgBodyLen());
                    return;
                }

                TMtAlias *ptAlias = (TMtAlias *)cServMsg.GetMsgBody();
                if (!SetChairman(cConfId, ptAlias))
                {
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] set chair info failed.insid.%d\n", GetInsID());
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
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(speaker)\n", cServMsg.GetMsgBodyLen());
                    return;
                }
             
                TMtAlias *ptAlias = (TMtAlias *)cServMsg.GetMsgBody();
                if (!SetSpeaker(cConfId, ptAlias))
                {
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] set speaker info failed.insid.%d\n", GetInsID());
                    return;
                }
            }            
            break;

        case MCU_NPLUS_VMPUPDATE_REQ:
            {
                if (sizeof(TNPlusVmpInfo) + sizeof(TVMPParam) != cServMsg.GetMsgBodyLen())
                {
                    //nack
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(vmpinfo)\n", cServMsg.GetMsgBodyLen());
                    return;
                }

                TNPlusVmpInfo *ptVmpInfo = (TNPlusVmpInfo *)cServMsg.GetMsgBody();
                TVMPParam *ptParam = (TVMPParam *)(cServMsg.GetMsgBody() + sizeof(TNPlusVmpInfo));
                if (!SetConfVmpInfo(cConfId, ptVmpInfo, ptParam))
                {
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] set vmp info failed.insid.%d\n", GetInsID());
                    return;
                }
            }            
            break;

        case MCU_NPLUS_RASINFOUPDATE_REQ:
            {
                if ( sizeof(TRASInfo) != cServMsg.GetMsgBodyLen() ) 
                {
                    PostReplyBack(pcMsg->event+2);
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] invalid msg body len(%d). nack(rasinfo)\n", cServMsg.GetMsgBodyLen() );
                    return;
                }
                TRASInfo tRASInfo = *(TRASInfo*)cServMsg.GetMsgBody();
                SetRASInfo(&tRASInfo);
                
                if ( tRASInfo.GetEPID()->GetIDlength() == 0 )
                {
                    OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] EPID has set to SLAVE mcu Error!\n");
                }
            }
            break;

        default: 
            OspPrintf(TRUE, FALSE, "[ProcMcuDataUpdateReq] invalid msg (%s:%d)\n",
                                        OspEventDesc(pcMsg->event), pcMsg->event);
            return;
            break;
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
    if (sizeof(TNPlusEqpCap) != cServMsg.GetMsgBodyLen())
    {
        OspPrintf(TRUE, FALSE, "[ProcMcuEqpCapNotif] invalid msg body len(%d)\n", cServMsg.GetMsgBodyLen());
        return;
    }

    TNPlusEqpCap tLocalCap = g_cNPlusApp.GetMcuEqpCap();
    TNPlusEqpCap tRegMcuCap = *(TNPlusEqpCap *)cServMsg.GetMsgBody();
    if (tLocalCap < tRegMcuCap)
    {
        OspPrintf(TRUE, FALSE, "[ProcMcuEqpCapNotif] registing mcu eqpcap is larger then local. disconnect node(%d)\n", 
            cServMsg.GetMsgBodyLen(), m_dwMcuNode);
        OspDisconnectTcpNode(m_dwMcuNode);
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
        OspPrintf(TRUE, FALSE, "[ProcConfRollback] invalid msg(%s) received.\n", OspEventDesc(cServMsg.GetEventId()));
        return;
        break;
    }

    PostMsgToNPlusMcuDaemon(wEvent, cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
    
    NPlusLog("[ProcConfRollback] send msg %s to master mcu(0x%x).\n", OspEventDesc(wEvent), m_dwMcuIpAddr);  
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
void CNPlusInst::ProcReset( const CMessage * pcMsg )
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
void CNPlusInst::ProcRtdRsp( const CMessage * pcMsg )
{
    m_wRtdFailTimes = 0;    
    SetTimer(MCU_NPLUS_RTD_TIMER, g_cNPlusApp.GetRtdTime()*1000);   
    NPlusLog("[ProcRtdRsp] InsId(%d) mcu(0x%x).\n", GetInsID(), m_dwMcuIpAddr); 
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
void CNPlusInst::ProcRtdTimeOut( const CMessage * pcMsg )
{        
    m_wRtdFailTimes++;

    //需要进行模式切换，恢复此实例对应mcu的会议信息
    if (m_wRtdFailTimes >= g_cNPlusApp.GetRtdNum())
    {
        NPlusLog("[ProcRtdTimeOut] mode switch and RESTORE mcu(0x%x) conf.\n", m_dwMcuIpAddr);       
        RestoreMcuConf();        
    }
    else
    {
        NPlusLog("[ProcRtdTimeOut] rtd time out and retrying mcu(0x%x).\n", m_dwMcuIpAddr); 
        PostMsgToNPlusMcuDaemon(MCU_NPLUS_RTD_REQ);
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
=============================================================================*/
void CNPlusInst::ProcDisconnect( const CMessage * pcMsg )
{
    NPlusLog("[ProcDisconnect] OSP Disconnected MCU(0x%x).\n", m_dwMcuIpAddr);
    return;
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
=============================================================================*/
void CNPlusInst::PostReplyBack( u16 wEvent, u16 wErrorCode, u8 *const pbyMsg, u16 wMsgLen )
{
    CServMsg cServMsg;
    cServMsg.SetEventId(wEvent);
    cServMsg.SetErrorCode(wErrorCode);
    if (wMsgLen > 0 && NULL != pbyMsg)
    {
        cServMsg.SetMsgBody(pbyMsg, wMsgLen);
    }
    post(m_dwMcuIId, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), m_dwMcuNode);
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
        OspPrintf(TRUE, FALSE, "[RestoreMcuConf] invalid NPlusState.%d, restore mcu conf failed.\n",
                  g_cNPlusApp.GetLocalNPlusState());
        
        ClearInst();
        return;
    }

    // 特殊处理1: N+1 备份注册GK, 备份MCU取代主MCU工作，整体注销主MCU上所有的实
    //            体ID。否则，在主备配相同GK的情况下，备份MCU召开的对应会议将不能实现邀请终端 [12/23/2006-zbq]
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
        BOOL32 bVmpMod = ptConfData->m_tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_CTRL ? TRUE : FALSE;
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
            atAlias[byLoop].m_tTransportAddr = ptMtInfo->GetMtAddr();
            awMtDialBitRate[byLoop] = htons(ptMtInfo->GetCallBitrate());

            //更新vmpmod中终端索引
            if (bVmpMod)
            {
                for (u8 byChnl = 0; byChnl < MAXNUM_MPUSVMP_MEMBER; byChnl++)
                {
                    if (ptConfData->m_tVmpInfo.IsMtInVmpMem(*ptMtInfo, byChnl))
                    {
                        u8 byType = ptConfData->m_tVmpInfo.GetVmpMemType(*ptMtInfo, byChnl);
                        tVmpMod.SetVmpMember(byChnl, byLoop+1, byType);
                    }    
                }                            
            }
        }

        PackTMtAliasArray(atAlias, awMtDialBitRate, ptConfData->m_byMtNum, achAliasBuf, wAliasBufLen);

        //去掉tvwall信息，因为tvwall与具体的外设id相关，不能进行恢复
        ptConfData->m_tConf.SetHasTvwallModule(FALSE);
        ptConfData->m_tConf.SetHasVmpModule(bVmpMod);

        ptConfData->m_tConf.m_tStatus.m_tConfMode.SetTakeMode(CONF_TAKEMODE_ONGOING);        
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
    CExUsrInfo cUsrInfo;
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
        OspPrintf( TRUE, FALSE, "[CNPlusInst::DaemonInstanceEntry] invalid msg body!\n" );
        return;
    }

    switch( pcMsg->event )
    {
    case OSP_POWERON:
        DaemonPowerOn( pcMsg, pcApp );
        break;

    case VC_NPLUS_RESET_NOTIF:
    case MCS_MCU_NPLUSRESET_CMD:    // Mcs 请求Reset
        DaemonReset( pcMsg, pcApp );
        break;

    case MCS_MCU_NPLUSROLLBACK_REQ:
        DaemonMcsRollBack(pcMsg, pcApp);
        break;
        
    case OSP_DISCONNECT:
        DaemonDisconnect( pcMsg, pcApp );
        break;

    case MCU_NPLUS_REG_REQ:
        DaemonRegNPlusMcuReq( pcMsg, pcApp );
        break;

    case MCU_NPLUS_REG_ACK:
    case MCU_NPLUS_REG_NACK:
        DaemonRegNPlusMcuRsp( pcMsg, pcApp );
        break;

    case MCU_NPLUS_CONNECT_TIMER:
        DaemonConnectNPlusMcuTimeOut( pcMsg );
        break;
    
    case MCU_NPLUS_REG_TIMER:
        DaemonRegNPlusMcuTimeOut( pcMsg );
        break;

        //主mcu数据上报消息
    case VC_NPLUS_MSG_NOTIF:  
        DaemonVcNPlusMsgNotif( pcMsg, pcApp );
        break;

        //主备rtd消息处理
    case MCU_NPLUS_RTD_REQ:
        DaemonRtdReq( pcMsg, pcApp );
        break;

    case MCU_NPLUS_CONFINFOUPDATE_ACK:    
    case MCU_NPLUS_USRGRPUPDATE_ACK:
    case MCU_NPLUS_CONFMTUPDATE_ACK:    
    case MCU_NPLUS_CHAIRUPDATE_ACK:    
    case MCU_NPLUS_SPEAKERUPDATE_ACK:    
    case MCU_NPLUS_VMPUPDATE_ACK:    
    case MCU_NPLUS_CONFDATAUPDATE_ACK:    
    case MCU_NPLUS_USRINFOUPDATE_ACK:
    case MCU_NPLUS_RASINFOUPDATE_ACK:
        
    case MCU_NPLUS_CONFINFOUPDATE_NACK:
    case MCU_NPLUS_USRGRPUPDATE_NACK:
    case MCU_NPLUS_CONFMTUPDATE_NACK:
    case MCU_NPLUS_CHAIRUPDATE_NACK:
    case MCU_NPLUS_SPEAKERUPDATE_NACK:
    case MCU_NPLUS_VMPUPDATE_NACK:
    case MCU_NPLUS_CONFDATAUPDATE_NACK:
    case MCU_NPLUS_USRINFOUPDATE_NACK:
    case MCU_NPLUS_RASINFOUPDATE_NACK:
        
        DaemonDataUpdateRsp( pcMsg, pcApp );
        break;

    //会议回滚
    case MCU_NPLUS_CONFROLLBACK_REQ:
        DaemonConfRollbackReq( pcMsg, pcApp );
        break;
    //用户回滚
    case MCU_NPLUS_USRROLLBACK_REQ:
        DaemonUsrRollbackReq( pcMsg, pcApp );
        break;
    //用户组回滚
    case MCU_NPLUS_GRPROLLBACK_REQ:
        DaemonGrpRollbackReq( pcMsg, pcApp );
        break;
        
    default:
        OspPrintf( TRUE, FALSE, "[CNPlusInst::DaemonInstanceEntry] invalid event(%d): %s!\n", 
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
void  CNPlusInst::DaemonPowerOn( const CMessage * pcMsg, CApp* pcApp  )
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
void  CNPlusInst::DaemonReset( const CMessage * pcMsg, CApp* pcApp  )
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
            g_cMcsSsnApp.SendMsgToMcsSsn( cServMsg.GetSrcSsnId(), 
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
void CNPlusInst::DaemonDisconnect( const CMessage * pcMsg, CApp* pcApp  )
{
    OspPrintf(TRUE, FALSE, "[DaemonDisconnect] node(%d) disconnect. connecting...\n", m_dwMcuNode);
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
void CNPlusInst::DaemonDataUpdateRsp( const CMessage * pcMsg, CApp* pcApp )
{
    if ( pcMsg->event == MCU_NPLUS_CONFINFOUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_VMPUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_CONFDATAUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_CONFMTUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_CHAIRUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_SPEAKERUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_USRINFOUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_USRGRPUPDATE_ACK ||
         pcMsg->event == MCU_NPLUS_RASINFOUPDATE_ACK )
    {
        g_cNPlusApp.SetNPlusSynOk(TRUE);
    }
    else if ( pcMsg->event == MCU_NPLUS_CONFINFOUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_VMPUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_CONFDATAUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_CONFMTUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_CHAIRUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_SPEAKERUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_USRINFOUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_USRGRPUPDATE_NACK ||
              pcMsg->event == MCU_NPLUS_RASINFOUPDATE_NACK )
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
void CNPlusInst::DaemonConfRollbackReq( const CMessage * pcMsg, CApp* pcApp )
{
    if (g_cNPlusApp.GetLocalNPlusState() != MCU_NPLUS_MASTER_CONNECTED)
    {
        OspPrintf(TRUE, FALSE, "[DaemonConfRollbackReq] invalid N+1 mode state(%d), ignore conf rollback msg\n", 
                  g_cNPlusApp.GetLocalNPlusState());
        return;
    }

    //ack
    PostReplyBack(pcMsg->event+1);

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    //若会议已经通过会议自动恢复召开，先结束，再进行恢复
    TNPlusConfData tConfData = *(TNPlusConfData *)cServMsg.GetMsgBody();
    if (tConfData.IsNull())
    {
        OspPrintf(TRUE, FALSE, "[DaemonConfRollbackReq] confid is null, ignore conf rollback msg\n");
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
    BOOL32 bVmpMod = tConfData.m_tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_CTRL ? TRUE : FALSE;
    if (bVmpMod)
    {
        tVmpMod.SetVmpParam(tConfData.m_tConf.m_tStatus.GetVmpParam());

        // 画面合成成员信息只能保存到会议信息里，本结构只保存其他信息 [12/14/2006-zbq]
        for( u8 byIndex = 0; byIndex <  MAXNUM_MPUSVMP_MEMBER; byIndex ++ )
        {
            tVmpMod.m_tVMPParam.ClearVmpMember(byIndex);
        }
    }

    for(u8 byLoop = 0; byLoop < tConfData.m_byMtNum; byLoop++)
    {            
        ptMtInfo = &tConfData.m_atMtInConf[byLoop];
        atAlias[byLoop].m_AliasType = mtAliasTypeTransportAddress;
        atAlias[byLoop].m_tTransportAddr = ptMtInfo->GetMtAddr();
        awMtDialBitRate[byLoop] = htons(ptMtInfo->GetCallBitrate());

        //更新vmpmod中终端索引
        if (bVmpMod)
        {
            for (u8 byChnl = 0; byChnl < MAXNUM_MPUSVMP_MEMBER; byChnl++)
            {
                if (tConfData.m_tVmpInfo.IsMtInVmpMem(*ptMtInfo, byChnl))
                {
                    u8 byType = tConfData.m_tVmpInfo.GetVmpMemType(*ptMtInfo, byChnl);
                    tVmpMod.SetVmpMember(byChnl, byLoop+1, byType);
                }    
            }               
        }
    }

    PackTMtAliasArray(atAlias, awMtDialBitRate, tConfData.m_byMtNum, achAliasBuf, wAliasBufLen);

    //去掉tvwall信息，因为tvwall与具体的外设id相关，不能进行恢复
    tConfData.m_tConf.SetHasTvwallModule(FALSE);
    tConfData.m_tConf.SetHasVmpModule(bVmpMod);

    tConfData.m_tConf.m_tStatus.m_tConfMode.SetTakeMode(CONF_TAKEMODE_ONGOING);
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
void CNPlusInst::DaemonUsrRollbackReq( const CMessage * pcMsg, CApp* pcApp )
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
void CNPlusInst::DaemonGrpRollbackReq( const CMessage * pcMsg, CApp* pcApp )
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
void CNPlusInst::DaemonConnectNPlusMcuTimeOut( const CMessage * pcMsg )
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
			    NPlusLog("OspConnectTcpNode MCU(0x%x) Failed!\n", tLocalInfo.GetNPlusMcuIp());
			    SetTimer( MCU_NPLUS_CONNECT_TIMER, NPLUS_CONNECTMCU_TIMEOUT );
			    return;
            }
            OspNodeDiscCBRegQ(m_dwMcuNode, GetAppID(), GetInsID());
        }
        else
        {
            OspPrintf(TRUE, FALSE, "[DaemonConnectNPlusMcu] mcu agent get localinfo failed.\n");
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
void CNPlusInst::DaemonRegNPlusMcuTimeOut( const CMessage * pcMsg )
{
    if (CurState() != STATE_IDLE)
    {
        return;
    }

    TNPlusMcuRegInfo tRegInfo;
    u32 dwIp = ntohl(g_cMcuAgent.GetMpcIp());
    tRegInfo.SetMcuIpAddr( dwIp );
    tRegInfo.SetMcuEqpCap( g_cNPlusApp.GetMcuEqpCap() );

    // N+1 备份环境约束: 8000B不允许备份8000，其他8种备份情况都允许 [12/20/2006-zbq]
    tRegInfo.SetMcuType( g_cNPlusApp.GetMcuType() );

    PostMsgToNPlusMcuDaemon( MCU_NPLUS_REG_REQ, (u8 *)&tRegInfo, sizeof(tRegInfo) );

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
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    if (sizeof(TNPlusMcuRegInfo) != cServMsg.GetMsgBodyLen())
    {
        OspPrintf( TRUE, FALSE, "[DaemonRegNPlusMcuReq] cServMsg.GetMsgBodyLen()=%d, sizeof(TNPlusMcuRegInfo)=%d !\n",
                   cServMsg.GetMsgBodyLen(), sizeof(TNPlusMcuRegInfo));
        DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_MSGLEN_ERR );
        return;
    }

    TNPlusMcuRegInfo *ptRegInfo = (TNPlusMcuRegInfo *)(cServMsg.GetMsgBody());

    //本MCU不支持N+1备份 或 N+1备份下本端主用，拒绝其他MCU连上来，以免 假注册成功 或 主主假锁[12/28/2006-zbq]
    if ( MCU_NPLUS_IDLE == g_cNPlusApp.GetLocalNPlusState() ||
         MCU_NPLUS_MASTER_IDLE == g_cNPlusApp.GetLocalNPlusState() ||
         MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState() )
    {
        DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_CONNECT_MASTER );
        return;
    }

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
    TNPlusEqpCap tLocalCap = g_cNPlusApp.GetMcuEqpCap();
    if (tLocalCap < ptRegInfo->GetMcuEqpCap())
    {
        DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_CAP_ERR );
        return;
    }

    u8 byInsId = 1;
    if (g_cNPlusApp.GetMcuSwitchedIp() == ptRegInfo->GetMcuIpAddr())
    {
        byInsId = g_cNPlusApp.GetMcuSwitchedInsId();
    }
    else
    {
        for( ; byInsId <= MAXNUM_NPLUS_MCU; byInsId++ )
        {
            CNPlusInst *pcInst = (CNPlusInst *)pcApp->GetInstance(byInsId);
            if (NULL != pcInst && pcInst->CurState() == STATE_IDLE)
            {
                break;
            }
        }    
    }    

    if (byInsId <= MAXNUM_NPLUS_MCU)
    {
        cServMsg.CatMsgBody((u8 *)&pcMsg->srcid, sizeof(pcMsg->srcid));
        cServMsg.CatMsgBody((u8 *)&pcMsg->srcnode, sizeof(pcMsg->srcnode));
        post(MAKEIID(AID_MCU_NPLUSMANAGER, byInsId), pcMsg->event, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
    }
    else
    {
        DaemonRegNack( pcMsg, NPLUS_BAKCLIENT_OVERLOAD );
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
    DaemonReplyNack( pcMsg, cServMsg );
    OspDisconnectTcpNode(m_dwMcuNode);

    OspPrintf(TRUE, FALSE, "[DaemonRegNPlusMcuReq] NPlus reg failed due to reason<%d>.\n", byReason);
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
void CNPlusInst::DaemonRegNPlusMcuRsp( const CMessage * pcMsg, CApp* pcApp )
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
                
                CServMsg cServMsg;
                cServMsg.SetMsgBody( (u8*)&tRASInfo, sizeof(tRASInfo) );
                cServMsg.SetEventId( MCU_NPLUS_RASINFOUPDATE_REQ );

                PostMsgToNPlusMcuInst( cServMsg );
            }
            else
            {
                NPlusLog("[DaemonRegNPlusMcuRsp] Rasinfo invalid, the RRQ mtadp will notify the backup mcu in NO TIME !\n");
            }
        }
    }
    else
    {
        //上报主业务主备注册失败的原因 [12/18/2006-zbq]
        OspPost(MAKEIID(AID_MCU_VC, CInstance::DAEMON), pcMsg->event, NULL, 0);
        OspPrintf(TRUE, FALSE, "[DaemonRegNPlusMcuRsp] receive register nack due to Reason.%d.\n", cServMsg.GetErrorCode());
    }
    return;
}

/*=============================================================================
函 数 名： DaemonReplyNack
功    能： 备份mcu Daemon实例的发送nack给主mcu
算法实现： 
全局变量： 
参    数：  const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/22  4.0			许世林                  创建
=============================================================================*/
void CNPlusInst::DaemonReplyNack( const CMessage * pcMsg, CServMsg &cServMsg )
{
    post(pcMsg->srcid, pcMsg->event+2, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), pcMsg->srcnode);
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
void CNPlusInst::DaemonVcNPlusMsgNotif( const CMessage * pcMsg, CApp* pcApp )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    if (CurState() != STATE_NORMAL)
    {
//        OspPrintf(TRUE, FALSE, "[DaemonVcNPlusMsgNotif] invalid ins state(%d), ignore it(%s)\n", 
//            CurState(), OspEventDesc(cServMsg.GetEventId()));
        return;
    }

    PostMsgToNPlusMcuInst(cServMsg);
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
void CNPlusInst::DaemonRtdReq( const CMessage * pcMsg, CApp* pcApp )
{
    NPlusLog("[DaemonRtdReq] receive rtd request.\n");
    PostReplyBack(MCU_NPLUS_RTD_RSP);
    return;
}

/*=============================================================================
函 数 名： PostMsgToNPlusMcuDaemon
功    能： 主mcu daemon发送消息到备份mcu daemon实例
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
=============================================================================*/
void CNPlusInst::PostMsgToNPlusMcuDaemon( u16 wEvent, u8 *const pbyMsg, u16 wMsgLen )
{
    CServMsg cServMsg;
    cServMsg.SetEventId(wEvent);
    if (wMsgLen > 0 && NULL != pbyMsg)
    {
        cServMsg.SetMsgBody(pbyMsg, wMsgLen);
    }    
    post(MAKEIID(AID_MCU_NPLUSMANAGER, CInstance::DAEMON), 
        wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), m_dwMcuNode);
    return;
}

/*=============================================================================
函 数 名： PostMsgToNPlusMcuInst
功    能： 主mcu daemon发送消息给备份mcu相应实例
算法实现： 
全局变量： 
参    数：  
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/22  4.0			许世林                  创建
=============================================================================*/
void CNPlusInst::PostMsgToNPlusMcuInst( CServMsg &cServMsg )
{    
    post(m_dwMcuIId, cServMsg.GetEventId(), cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), m_dwMcuNode);
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
        OspPrintf(TRUE, FALSE, "[CNPlusInst::SetConfData] get conf index failed by cconfid:");
        ptConfData->m_tConf.GetConfId().Print();
        return FALSE;
    }

    memcpy(&m_atConfData[byIdx], ptConfData, sizeof(TNPlusConfData));
    return TRUE;
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
BOOL32  CNPlusInst::SetConfInfo( TConfInfo *ptConfInfo, BOOL32 bStart )
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
        OspPrintf(TRUE, FALSE, "[CNPlusInst::SetConfInfo] get conf index failed by cconfid:");
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
        m_atConfData[byIdx].Clear();
    }
    
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
        OspPrintf(TRUE, FALSE, "[CNPlusInst::SetConfMtInfo] get conf index failed by cconfid:");
        cConfId.Print();
        return FALSE;
    }

    memcpy(m_atConfData[byIdx].m_atMtInConf, ptMtInfo, byMtNum*sizeof(TMtInfo));
    m_atConfData[byIdx].m_byMtNum = byMtNum;
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
        OspPrintf(TRUE, FALSE, "[CNPlusInst::SetConfVmpInfo] get conf index failed by cconfid:");
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
        OspPrintf(TRUE, FALSE, "[CNPlusInst::SetChairman] get conf index failed by cconfid:");
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
        OspPrintf(TRUE, FALSE, "[CNPlusInst::SetSpeaker] get conf index failed by cconfid:");
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
            OspPrintf(TRUE, FALSE, "[SetUsrInfo] user packet error.\n");
            return FALSE;
        }
        memcpy(&m_acUsrInfo[m_byUsrNum], pcUsrInfo, sizeof(CExUsrInfo)*byNum);
        m_byUsrNum += byNum;
    }
    else
    {
        if (byNum > MAXNUM_USRNUM)
        {
            OspPrintf(TRUE, FALSE, "[SetUsrInfo] user packet error.\n");
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

    if ( 0 == ptRASInfo->GetEPID()->GetIDlength() ) 
    {
        OspPrintf( TRUE, FALSE, "[UnRegAllInfoOfSwitchedMcu] the EPID of MASTER mcu hasn't got yet !\n" );
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
            if (byEqpId <= BASID_MAX)
            {
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
                        tEqpCap.m_byMAUNum++;
                    }
                    else if (TYPE_MAU_H263PLUS == tStatus.m_tStatus.tHdBas.GetEqpType())
                    {
                        tEqpCap.m_byMAUH263pNum ++;
                    }
                    else if (TYPE_MPU == tStatus.m_tStatus.tHdBas.GetEqpType())
                    {
                        tEqpCap.m_byMpuBasNum ++;
                    }
                    else
                    {
                        OspPrintf(TRUE, FALSE, "[CNPlusData::GetMcuEqpCap] unexpected mau type.%d\n",
                                                tStatus.m_tStatus.tHdBas.GetEqpType());
                    }
                }
               
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
                tEqpCap.m_byVmpNum++;

                // VMP能力具体到MAP数 [12/28/2006-zbq]
                tEqpCap.m_abyVMPChnNum[tEqpCap.m_byVmpNum-1] = tStatus.m_tStatus.tVmp.m_byChlNum;
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
TNPlusEqpCap    CNPlusData::GetEqpCapFromConf( TConfInfo &tConf )
{
    TNPlusEqpCap tEqpCap;
    //vmp
	//自动画面合成需要VMP能力支持 [01/13/2006-zbq]
    if ( tConf.GetConfAttrb().IsHasVmpModule() || 
	     CONF_VMPMODE_AUTO == tConf.m_tStatus.GetVMPMode() )
    {
        tEqpCap.m_byVmpNum++;
    }
    //bas
    if (tConf.GetConfAttrb().IsUseAdapter())
    {
        if (!IsHDConf(tConf))
        {
            if (tConf.GetSecAudioMediaType() != MEDIA_TYPE_NULL)
            {
                tEqpCap.m_byBasAudChnlNum++;
            }
            else if (tConf.GetSecVideoMediaType() != MEDIA_TYPE_NULL)
            {
                tEqpCap.m_byBasVidChnlNum++;
            }
            
            if (tConf.GetSecBitRate() != 0)
            {
                tEqpCap.m_byBasVidChnlNum++;
            }
        }
        else
        {
            u8 byNVChn = 0;
            u8 byDSChn = 0;
            u8 byH263pChn = 0;
            u8 byVGAChn = 0;
            
            u8 byMpuNum = 0;
            g_cMcuVcApp.GetMpuNum(byMpuNum);

            //有mpu就认为是纯mpu备份，暂不支持mpu和mau的交叉备份
            if (byMpuNum > 0)
            {
                CBasMgr cBasMgr;
                cBasMgr.GetNeededMpu(tConf, byNVChn, byDSChn);
                tEqpCap.m_byMpuBasNum += (byNVChn + byDSChn)/MAXNUM_MPU_CHN + 1;
            }
            else
            {
                CBasMgr cBasMgr;
                cBasMgr.GetNeededMau(tConf, byNVChn, byH263pChn, byVGAChn);
                
                tEqpCap.m_byMAUNum += byNVChn;
                tEqpCap.m_byMAUH263pNum += byH263pChn;
            }
        }
    }
    //prs
    if (tConf.GetConfAttrb().IsResendLosePack())
    {
        tEqpCap.m_byPrsChnlNum++;
    }
    //GK Charge [12/18/2006-zbq]
    tEqpCap.SetIsGKCharge(tConf.m_tStatus.IsGkCharge());
    
    return tEqpCap;
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

//END OF FILE
