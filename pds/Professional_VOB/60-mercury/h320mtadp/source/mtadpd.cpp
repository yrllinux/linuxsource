/*****************************************************************************
模块名      : h320终端适配模块
文件名      : MtAdpd.cpp
相关文件    : MtAdpInst.h
文件实现功能: Daemon实例模块
作者        : 许世林
版本        : V3.6  Copyright(C) 1997-2003 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2005/06/07  3.6         许世林      创建
******************************************************************************/

#include "mtadpinst.h"
#include "evmcumt.h"
#include "boardagent.h"
#include "mcuver.h"


/*=============================================================================
  函 数 名： DaemonInstanceEntry
  功    能： Daemon实例入口
  算法实现： 
  全局变量： 
  参    数： CMessage *const pcMsg
             CApp * pcApp
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::DaemonInstanceEntry(CMessage *const pcMsg, CApp * pcApp)
{
    if(NULL == pcMsg)
    {
        MtAdpLog(LOG_CRITICAL, "[DaemonInstanceEntry] Msg pointer is Null!\n");
        return;
    }

    MtAdpLog(LOG_DEBUG, "[DaemonInstanceEntry] rcv message :%u(%s)\n", pcMsg->event, OspEventDesc(pcMsg->event));

    switch(pcMsg->event)
    { 
        
    case OSP_POWERON:
        // 上电处理
        OnDaemonPowerOn();
        break;

        //适配模块向mcu注册
    case MTADP_MCU_REGISTER_REQ:
        OnDaemonRegisterMcuReq(pcMsg, TRUE);
        break;
    case MTADP_MCU_REGISTER_REQ2:
        OnDaemonRegisterMcuReq(pcMsg, FALSE);
        break;
    case MCU_MTADP_REGISTER_ACK:
    case MCU_MTADP_REGISTER_NACK:
        OnDaemonRegisterMcuRsp(pcMsg);
        break;

        //osp断链处理
    case OSP_DISCONNECT:
        {
            MtAdpLog(LOG_INFO, "[DaemonInstanceEntry] rcv OSP_DISCONNECT\n");

            OnDaemonOspDisconnect(pcMsg, pcApp);
        }
        break;

    case TIMER_GETMSSTATUS_REQ:
    case MCU_MTADP_GETMSSTATUS_ACK:
        DaemonProcGetMsStatusRsp( pcMsg, pcApp );
        break;  

        //mcu邀请终端，分配实例
    case MCU_MT_INVITEMT_REQ:
        OnDaemonMcuInviteMtReq(pcMsg);
        break;

        //mcu挂断终端，回收实例
    case MCU_MT_DELMT_CMD:
        OnDaemonDelMtCmd(pcMsg, pcApp);
        break;        

        //会议信息通知
    case MCU_MT_CONF_NOTIF:    
    case MCU_MT_SIMPLECONF_NOTIF:
        //终端入会离会通知,通知所有与会实例
    case MCU_MT_MTJOINED_NOTIF:
    case MCU_MT_MTLEFT_NOTIF:
        OnDaemonBroadMsg2AllMt(pcMsg);            
        break;

        //将mcu消息分发到相应实例
    default:        
        OnDaemonMcuMtGeneral(pcMsg, pcApp);
        break;   
    }
}

/*=============================================================================
  函 数 名： OnDaemonPowerOn
  功    能： 上电处理
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnDaemonPowerOn()
{
	OspPost( MAKEIID( AID_MCU_MTADP, CInstance::DAEMON ), MTADP_MCU_REGISTER_REQ, NULL, 0 );

	if (g_cMtAdpApp.m_bDoubleLink)
	{
		OspPost( MAKEIID( AID_MCU_MTADP, CInstance::DAEMON ), MTADP_MCU_REGISTER_REQ2, NULL, 0 );
	}
}


/*=============================================================================
  函 数 名： OnRegisterMcuReq
  功    能： 向mcu发起注册请求
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnDaemonRegisterMcuReq(const CMessage *pcMsg, BOOL32 bMcuA)
{
    if (bMcuA)
    {
        //获取mcu A通讯节点
        g_cMtAdpApp.m_dwMcuANode = BrdGetDstMcuNode();

        if(!OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuANode))
        {
            SetTimer(MTADP_MCU_REGISTER_REQ, 3*1000);
            MtAdpLog(LOG_WARNING, "[OnRegisterMcuReq] Mcu A node is invalid!\n");
            return;
        }

        //注册断链处理消息
        OspNodeDiscCBRegQ(g_cMtAdpApp.m_dwMcuANode, GetAppID(), GetInsID());
    }
    else
    {
        //获取mcu B通讯节点
        g_cMtAdpApp.m_dwMcuBNode = BrdGetDstMcuNodeB();

        if(!OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuBNode))
        {
            SetTimer(MTADP_MCU_REGISTER_REQ2, 3*1000);
            MtAdpLog(LOG_WARNING, "[OnRegisterMcuReq] Mcu B node is invalid!\n");
            return;
        }

        //注册断链处理消息
        OspNodeDiscCBRegQ(g_cMtAdpApp.m_dwMcuBNode, GetAppID(), GetInsID());
    }

    //外设支持的E1个数
    u8 byE1Num = m_pcH320Stack->GetPeriE1Num();
    
    MtAdpLog(LOG_INFO, "[OnRegisterMcuReq] E1 num of Brd supporting is %d.\n", byE1Num);
    
    if(byE1Num > MAXNUM_CHAN)
    {
        byE1Num = MAXNUM_CHAN;
    }

    //向mcu注册
    TMtAdpReg tReg;
    tReg.SetDriId(g_cMtAdpApp.m_byDriId);
    tReg.SetIpAddr(ntohl(g_cMtAdpApp.m_dwLocalIP));
    tReg.SetAttachMode(0);
    tReg.SetAlias(g_cMtAdpApp.m_achMtAdpAlias);
    tReg.SetProtocolType(PROTOCOL_TYPE_H320);
    tReg.SetMaxMtNum(byE1Num);
    tReg.SetVersion(DEVVER_MTADP);

    CServMsg cServMsg;
    cServMsg.SetEventId(MTADP_MCU_REGISTER_REQ);
    cServMsg.SetMsgBody((u8 *)&tReg, sizeof(tReg));

    if (bMcuA)
    {
        g_cMtAdpApp.m_dwMcuAIId = MAKEIID(AID_MCU_MTADPSSN, g_cMtAdpApp.m_byDriId);
    
        OspPost(g_cMtAdpApp.m_dwMcuAIId, 
                cServMsg.GetEventId(),
                cServMsg.GetServMsg(),
                cServMsg.GetServMsgLen(),
                g_cMtAdpApp.m_dwMcuANode,
                MAKEIID(AID_MCU_MTADP, GetInsID()));

        MtAdpLog(LOG_CRITICAL, "[OnRegisterMcuReq] MtAdp registering to mcu A\n");

        // 启动注册超时定时器
        SetTimer(MTADP_MCU_REGISTER_REQ, 5*1000);
    }
    else
    {
        g_cMtAdpApp.m_dwMcuBIId = MAKEIID(AID_MCU_MTADPSSN, g_cMtAdpApp.m_byDriId);
    
        OspPost(g_cMtAdpApp.m_dwMcuBIId, 
                cServMsg.GetEventId(),
                cServMsg.GetServMsg(),
                cServMsg.GetServMsgLen(),
                g_cMtAdpApp.m_dwMcuBNode,
                MAKEIID(AID_MCU_MTADP, GetInsID()));

        MtAdpLog(LOG_CRITICAL, "[OnRegisterMcuReq] MtAdp registering to mcu B\n");
        
        // 启动注册超时定时器
        SetTimer(MTADP_MCU_REGISTER_REQ2, 5*1000);
    }
    
    MtAdpLog(LOG_INFO, "\t DriId is %d\n", tReg.GetDriId());
    MtAdpLog(LOG_INFO, "\t MaxE1Num is %d\n", tReg.GetMaxMtNum());
    MtAdpLog(LOG_INFO, "\t LocalIP is %s\n", StrIPv4(htonl(tReg.GetIpAddr())));

}

/*=============================================================================
  函 数 名： OnDaemonRegisterMcuRsp
  功    能： mcu注册响应处理
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnDaemonRegisterMcuRsp(const CMessage *pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    
    BOOL32 bMcuA = TRUE;
    if (g_cMtAdpApp.m_dwMcuANode == pcMsg->srcnode)
    {
       bMcuA = TRUE;
    }
    else if (g_cMtAdpApp.m_dwMcuBNode == pcMsg->srcnode)
    {
       bMcuA = FALSE;
    }

    // 注册成功
    if (pcMsg->event == MCU_MTADP_REGISTER_ACK)
    {
        if(cServMsg.GetMsgBodyLen() != sizeof(TMtAdpRegRsp))
        {
            MtAdpLog(LOG_CRITICAL, "[OnRegisterMcuAck] msg body is invalid!\n");
            return;
        }
        
        TMtAdpRegRsp *ptRsp = (TMtAdpRegRsp *)cServMsg.GetMsgBody();
        if (bMcuA)
        {
            KillTimer(MTADP_MCU_REGISTER_REQ);

            g_cMtAdpApp.m_byMcuAId = ptRsp->GetMcuNetId();
            MtAdpLog(LOG_CRITICAL, "[OnRegisterMcuAck] MtAdp Register to Mcu A success!\n");;
        }
        else
        {
            KillTimer(MTADP_MCU_REGISTER_REQ2);

            g_cMtAdpApp.m_byMcuBId = ptRsp->GetMcuNetId();
            MtAdpLog(LOG_CRITICAL, "[OnRegisterMcuAck] MtAdp Register to Mcu B success!\n");;
        }
    }
    else
    {
        //重新发起注册
        if (bMcuA)
        {
            SetTimer(MTADP_MCU_REGISTER_REQ, 5*1000);
        }
        else
        {
            SetTimer(MTADP_MCU_REGISTER_REQ2, 5*1000);
        }
        
    }
}

/*=============================================================================
  函 数 名： OnOspDisconnect
  功    能： osp 断链处理
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnDaemonOspDisconnect(const CMessage *pcMsg, CApp *pcApp)
{
    u32 dwNodeDisconnect = *(u32*)pcMsg->content;
    if ( INVALID_NODE != dwNodeDisconnect )
    {
        OspDisconnectTcpNode( dwNodeDisconnect );
    }
    
    if (!OspIsValidTcpNode( g_cMtAdpApp.m_dwMcuANode ) &&
        !OspIsValidTcpNode( g_cMtAdpApp.m_dwMcuBNode ) )
    {
        // 都断链则清除
        for(s32 i = 1; i <= MAXNUM_CHAN; i++)
        {
            CInstance *pcInst = pcApp->GetInstance(i);
            if(NULL != pcInst)
            {
                ((CMtAdpInst*)pcInst)->ClearInst();
            }
        }
    }
    else
    {
        // 否则请求主备状态
		if (OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuANode))
		{
			post( g_cMtAdpApp.m_dwMcuAIId, MTADP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cMtAdpApp.m_dwMcuANode );     
			MtAdpLog(LOG_INFO, "[MsgDisconnectProc] GetMsStatusReq. McuNode.A\n");
		}
		else if (OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuBNode))
		{
			post( g_cMtAdpApp.m_dwMcuBIId, MTADP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cMtAdpApp.m_dwMcuBNode );        
			MtAdpLog(LOG_INFO, "[MsgDisconnectProc] GetMsStatusReq. McuNode.B\n");
		}
		// 等待指定时间
		SetTimer(TIMER_GETMSSTATUS_REQ, WAITING_MSSTATUS_TIMEOUT);        
    }

    //重新发起注册
    if (dwNodeDisconnect == g_cMtAdpApp.m_dwMcuANode)
    {        
        g_cMtAdpApp.m_dwMcuANode = INVALID_NODE;

        SetTimer(MTADP_MCU_REGISTER_REQ, 1000);
    }
    else if (dwNodeDisconnect == g_cMtAdpApp.m_dwMcuBNode)
    {
        g_cMtAdpApp.m_dwMcuBNode = INVALID_NODE;

        SetTimer(MTADP_MCU_REGISTER_REQ2, 1000);
    }
}


/*=============================================================================
函 数 名： DaemonProcGetMsStatusRsp
功    能： 等待获取主备状态超时
算法实现： 
全局变量： 
参    数： CMessage * const pcMsg
返 回 值： void 
=============================================================================*/
void CMtAdpInst::DaemonProcGetMsStatusRsp( const CMessage *pcMsg, CApp *pcApp )
{
    KillTimer(TIMER_GETMSSTATUS_REQ);

    BOOL32 bMsOk = FALSE;
    if(MCU_MTADP_GETMSSTATUS_ACK == pcMsg->event)
    {                
        CServMsg cServMsg(pcMsg->content, pcMsg->length);
        TMcuMsStatus *ptMsStatus = (TMcuMsStatus *)cServMsg.GetMsgBody();               
        
        MtAdpLog(LOG_INFO, "[DaemonProcGetMsStatusRsp]. receive msg MCU_MTADP_GETMSSTATUS_ACK. IsMsSwitchOK :%d\n", 
                  ptMsStatus->IsMsSwitchOK());

        if(ptMsStatus->IsMsSwitchOK())
        {
            bMsOk = TRUE;
        }
    }

    if (!bMsOk)
    {
        // 超时或ack中或者主备同步失败，则清除呼叫实例信息
        MtAdpLog(LOG_CRITICAL, "[DaemonProcGetMsStatusRsp]. Clear All Inst.\n");
    
        for(s32 i = 1; i <= MAXNUM_CHAN; i++)
        {
            CInstance *pcInst = pcApp->GetInstance(i);
            if(NULL != pcInst)
            {
                ((CMtAdpInst*)pcInst)->ClearInst();
            }
        }
    }

    if (g_cMtAdpApp.m_dwMcuANode == INVALID_NODE)
    {
        SetTimer(MTADP_MCU_REGISTER_REQ, 1000);
    }
    if (g_cMtAdpApp.m_dwMcuBNode == INVALID_NODE)
    {
        SetTimer(MTADP_MCU_REGISTER_REQ2, 1000);
    }    

    return;
}


/*=============================================================================
  函 数 名： OnDaemonMcuInviteMtReq
  功    能： mcu邀请终端入会，参数校验，分配相应实例(实例id为通道号＋1)
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnDaemonMcuInviteMtReq(const CMessage *pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    cServMsg.SetEventId(pcMsg->event);
    u8 byCause = emDisconnectReasonLocal;

    u16 wBodyLen = sizeof(TMt) + sizeof(TMtAlias)*2 + sizeof(u8) + sizeof(u16) + 
                   sizeof(TCapSupport);
    if(cServMsg.GetMsgBodyLen() < wBodyLen)
    {
        MtAdpLog(LOG_CRITICAL, "[OnDaemonMcuInviteMtReq] msg body is invalid, ignore it!\n");    
        return;
    }
    
    TMt *ptMt = (TMt *)cServMsg.GetMsgBody();
    TMtAlias tMtAlias = *(TMtAlias *)(cServMsg.GetMsgBody() + sizeof(TMt));

    //mtId & confId
    if(ptMt->GetMtId() > MAXNUM_CONF_MT || cServMsg.GetConfIdx() > MAXNUM_MCU_CONF)
    {
        MtAdpLog(LOG_CRITICAL, "[OnDaemonMcuInviteMtReq] MtId<%d> or ConfId<%d> is invalid, nack!\n", 
                 ptMt->GetMtId(), cServMsg.GetConfIdx());
        
        cServMsg.SetEventId(cServMsg.GetEventId() + 2);
        cServMsg.SetMsgBody((u8 *)&tMtAlias, sizeof(tMtAlias));
        cServMsg.CatMsgBody(&byCause, sizeof(byCause));
        SendMsg2Mcu(cServMsg);
        return;
    }

    //get E1 number    
    u8 byE1Num = 0, byLayer, bySlot;
    BOOL32 bRet = tMtAlias.GetH320Alias(byLayer, bySlot, byE1Num);
    u8 byDriId = byLayer<<4 | (bySlot+1);
    if(!bRet || byE1Num >= MAXNUM_CHAN || byDriId != g_cMtAdpApp.m_byDriId)
    {
        MtAdpLog(LOG_CRITICAL, "[OnDaemonMcuInviteMtReq] E1 num is invalid(should < %d) or DriId is invalid<%d> nack!\n", 
                 MAXNUM_CHAN, byDriId);
        
        cServMsg.SetEventId(cServMsg.GetEventId() + 2);
        cServMsg.SetMsgBody((u8 *)&tMtAlias, sizeof(tMtAlias));
        cServMsg.CatMsgBody(&byCause, sizeof(byCause));
        SendMsg2Mcu(cServMsg);
        return;
    }

    //check encrypt
    u8 byEncrypt = *(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TMtAlias)*2);
    if(0 != byEncrypt)
    {
        MtAdpLog(LOG_CRITICAL, "[OnDaemonMcuInviteMtReq] not support encpyt mode. nack!\n");
        
        cServMsg.SetEventId(cServMsg.GetEventId() + 2);
        cServMsg.SetMsgBody((u8 *)&tMtAlias, sizeof(tMtAlias));
        cServMsg.CatMsgBody(&byCause, sizeof(byCause));
        SendMsg2Mcu(cServMsg);
        return;
    }    

    //分配相应实例(InsId = E1 number + 1)    
    post(MAKEIID(AID_MCU_MTADP, byE1Num+1), 
         cServMsg.GetEventId(),
         cServMsg.GetServMsg(), 
         cServMsg.GetServMsgLen());
}

/*=============================================================================
  函 数 名： OnDaemonDelMtCmd
  功    能： 删除终端
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnDaemonDelMtCmd(const CMessage *pcMsg, CApp *pcApp)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    cServMsg.SetEventId(pcMsg->event);
    u8 byConfIdx = cServMsg.GetConfIdx();
    u8 byMtId = cServMsg.GetDstMtId();
    u8 byInsId = 0;

    //删除确定终端
    if(0 != byMtId)
    {
        byInsId = g_cMtAdpApp.m_abyMt2InsMap[byConfIdx][byMtId];
        post(MAKEIID(AID_MCU_MTADP, byInsId), 
                    cServMsg.GetEventId(),
                    cServMsg.GetServMsg(), 
                    cServMsg.GetServMsgLen());    
    }
    //删除会议中所有终端
    else
    {        
        for(u8 byIndex = 1; byIndex <= MAXNUM_CHAN; byIndex++)
        {
            CMtAdpInst *pcIns = (CMtAdpInst *)pcApp->GetInstance(byIndex);
            if(pcIns->m_byConfIdx == byConfIdx)
            {
                post(MAKEIID(AID_MCU_MTADP, pcIns->GetInsID()), 
                            cServMsg.GetEventId(),
                            cServMsg.GetServMsg(), 
                            cServMsg.GetServMsgLen());    
            }
        }        
    }   

    MtAdpLog(LOG_INFO, "[OnDaemonDelMtCmd] del mt id :%d\n", byMtId);
}

/*=============================================================================
  函 数 名： OnDaemonMcuMtGeneral
  功    能： 负责mcu到mt部分消息的实例分发
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
             CApp *pcApp
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::OnDaemonMcuMtGeneral(const CMessage *pcMsg, CApp *pcApp)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    cServMsg.SetEventId(pcMsg->event);
    u8 byMtId = cServMsg.GetDstMtId();
    u8 byConfIdx = cServMsg.GetConfIdx();
    if(byMtId > MAXNUM_CONF_MT || byConfIdx > MAXNUM_MCU_CONF)
    {
        MtAdpLog(LOG_CRITICAL, "[DaemonInstanceEntry] invalid mtid<%d> or confidx<%d>\n",
            byMtId, byConfIdx);
        return;
    }
    
    u8 byInsId = g_cMtAdpApp.m_abyMt2InsMap[byConfIdx][byMtId];
    post(MAKEIID(AID_MCU_MTADP, byInsId),
        cServMsg.GetEventId(),
        cServMsg.GetServMsg(),
        cServMsg.GetServMsgLen());
}

/*=============================================================================
  函 数 名： OnDaemonBroadMsg2AllMt
  功    能： 
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void  
=============================================================================*/
void  CMtAdpInst::OnDaemonBroadMsg2AllMt(const CMessage *pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    cServMsg.SetEventId(pcMsg->event);
    u8 byMtId = cServMsg.GetDstMtId();
    u8 byConfIdx = cServMsg.GetConfIdx();
    if(byMtId > MAXNUM_CONF_MT || byConfIdx > MAXNUM_MCU_CONF)
    {
        MtAdpLog(LOG_CRITICAL, "[DaemonInstanceEntry] invalid mtid<%d> or confidx<%d>\n",
            byMtId, byConfIdx);
        return;
    }
    
    post(MAKEIID(AID_MCU_MTADP, CInstance::EACH),
        cServMsg.GetEventId(),
        cServMsg.GetServMsg(),
        cServMsg.GetServMsgLen());
}

/*=============================================================================
  函 数 名： SendMsg2Mcu
  功    能： 发送消息到mcu
  算法实现： 
  全局变量： 
  参    数： CServMsg &cServMsg
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::SendMsg2Mcu(CServMsg &cServMsg)
{    
    cServMsg.SetSrcDriId(g_cMtAdpApp.m_byDriId);
    cServMsg.SetSrcMtId(m_byMtId);
    cServMsg.SetConfIdx(m_byConfIdx);
    cServMsg.SetConfId(m_cConfId);    

    s32 nRet = OSP_OK;

    if (OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuANode))
    {
        cServMsg.SetMcuId(g_cMtAdpApp.m_byMcuAId);

        nRet = OspPost(g_cMtAdpApp.m_dwMcuAIId, cServMsg.GetEventId(),
                            cServMsg.GetServMsg(), cServMsg.GetServMsgLen(),
                            g_cMtAdpApp.m_dwMcuANode, MAKEIID(AID_MCU_MTADP, GetInsID()));

        if(nRet != OSP_OK)
        {
            MtAdpLog(LOG_CRITICAL, "send msg :%u<%s> to mcu A failed!\n", 
                cServMsg.GetEventId(), OspEventDesc(cServMsg.GetEventId()));
        }
        else
        {
            MtAdpLog(LOG_INFO, "send msg :%u<%s> to mcu A succeed!\n", cServMsg.GetEventId()
                , OspEventDesc(cServMsg.GetEventId()));
        }
    }
    
    if (OspIsValidTcpNode(g_cMtAdpApp.m_dwMcuBNode))
    {
        cServMsg.SetMcuId(g_cMtAdpApp.m_byMcuBId);

        nRet = OspPost(g_cMtAdpApp.m_dwMcuBIId, cServMsg.GetEventId(),
                            cServMsg.GetServMsg(), cServMsg.GetServMsgLen(),
                            g_cMtAdpApp.m_dwMcuBNode, MAKEIID(AID_MCU_MTADP, GetInsID()));

        if(nRet != OSP_OK)
        {
            MtAdpLog(LOG_CRITICAL, "send msg :%u<%s> to mcu B failed!\n", 
                cServMsg.GetEventId(), OspEventDesc(cServMsg.GetEventId()));
        }
        else
        {
            MtAdpLog(LOG_INFO, "send msg :%u<%s> to mcu B succeed!\n", cServMsg.GetEventId()
                , OspEventDesc(cServMsg.GetEventId()));
        }
    }

}

/*=============================================================================
  函 数 名： MtAdpLog
  功    能： 调试信息打印输出
  算法实现： 
  全局变量： 
  参    数： u8 byLevel
             s8 * pInfo
             ...
  返 回 值： void 
=============================================================================*/
void CMtAdpInst::MtAdpLog(u8 byLevel, s8 * pInfo, ...)
{
    if(byLevel > g_cMtAdpApp.m_byLogLevel)
    {
        return;
    }

    s32 nIndex = 0;
    s8 szBuf[512] = {0};
    if(GetInsID() != CInstance::DAEMON)		
		nIndex = sprintf(szBuf, "[MtAdpInst %u]", GetInsID());	
	else		
		nIndex = sprintf(szBuf, "[MtAdpDaemon]");

    va_list arg;
    va_start(arg, pInfo);
    vsprintf(szBuf+nIndex, pInfo, arg);
    OspPrintf(TRUE, FALSE, szBuf);
    va_end(arg);
}
