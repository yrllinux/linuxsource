/*****************************************************************************
   模块名      : mcu
   文件名      : mcuvcmt.cpp
   相关文件    : mcuvc.h
   文件实现功能: MCU业务交换函数
   作者        : 胡昌威
   版本        : V2.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/08/06  2.0         胡昌威      创建
   2005/02/19  3.6         万春雷      级联修改、与3.5版本合并
******************************************************************************/

#include "evmcumcs.h"
#include "evmcuvcs.h"
#include "evmcumt.h"
#include "evmcueqp.h"
#include "evmcu.h"
#include "evmcutest.h"
#include "mcuvc.h"
#include "mcsssn.h"
#include "mtadpssn.h"
//#include "mcuerrcode.h"
//#include "mpmanager.h"

/*====================================================================
    函数名      ：AddMt
    功能        ：添加终端
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMtAlias &tMtAlias 终端别名
	              u16   wMtDialRate        呼叫速率(主机序)
				  BOOL  bPassiveAdd        是否是终端主动呼入添加。如果是终端主动呼入，则被动添加标记为TRUE
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/01/10    3.0         胡昌威         创建
====================================================================*/
u8 CMcuVcInst::AddMt( TMtAlias &tMtAlias, u16 wMtDialRate, u8 byCallMode, BOOL bPassiveAdd )
{
	u8 byMtId = 0xff;
    if( tMtAlias.m_AliasType == 0 ||
		(tMtAlias.m_AliasType > mtAliasTypeOthers &&
		 tMtAlias.m_AliasType != puAliasTypeIPPlusAlias))
	{
		return 0;
	}

	if(wMtDialRate == 0 || wMtDialRate > m_tConf.GetBitRate())
	{
		wMtDialRate = m_tConf.GetBitRate();
	}       

	//zjj20130502 恢复原呼叫码率，在TConfMtTable::GetDialBitrate方法中再根据实际的音频通道去除实际值
	wMtDialRate += GetAudioBitrate( m_tConf.GetMainAudioMediaType() );

    
	if( mtAliasTypeTransportAddress == tMtAlias.m_AliasType || 
		mtAliasTypeE164 == tMtAlias.m_AliasType || 
		mtAliasTypeH323ID == tMtAlias.m_AliasType ||
		// PU特殊类型
		puAliasTypeIPPlusAlias == tMtAlias.m_AliasType)
	{
		TMtAlias tTempAlias;
		if(tMtAlias.m_AliasType == puAliasTypeIPPlusAlias)
		{
			// PU特殊别名IP+ALIAS
			tTempAlias.m_AliasType = puAliasTypeIPPlusAlias;
			
			s8* pbyAlias = (s8*)tMtAlias.m_achAlias;
			tTempAlias.m_tTransportAddr.SetIpAddr(ntohl(*(u32*)pbyAlias));
			
			pbyAlias += sizeof(u32);
			u32 wActualAlias = strlen(pbyAlias);
			//u32 wMaxAlias = sizeof(tTempAlias.m_achAlias);
			if( wActualAlias > sizeof(tTempAlias.m_achAlias) )
			{
				wActualAlias = sizeof(tTempAlias.m_achAlias);
			}

			memcpy(tTempAlias.m_achAlias,pbyAlias,wActualAlias);
		}
		else
		{
			tTempAlias = tMtAlias;
		}
		
		byMtId = m_ptMtTable->GetMtIdByAlias( &tTempAlias );
        
        BOOL bRepeat = FALSE;
        // guzh [2008/01/22]  似乎终端已存在
		if (byMtId > 0)
        {
            // 对于主动添加，则做别名过滤
            if ( !bPassiveAdd )
            {
                bRepeat = TRUE;
            }
            // 对于终端呼入添加，则若检测原终端不在线就可以替换，否则必须重新分配一个
            else if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
            {
                bRepeat = TRUE;
            }

            if (bRepeat)
		    {
                ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[AddMt] Mt.%d existed, replace.\n", byMtId);
                return byMtId;
		    }
        }

		// 原来不存在的终端,分配一个新的给他		
		byMtId = m_ptMtTable->AddMt( &tMtAlias, bRepeat );
		if(byMtId > 0)
		{
			m_ptMtTable->SetDialAlias( byMtId, &tMtAlias );
			m_ptMtTable->SetDialBitrate( byMtId, wMtDialRate );
			

			m_ptMtTable->SetMcuId( byMtId, LOCAL_MCUIDX);
			m_ptMtTable->SetConfIdx( byMtId, m_byConfIdx );
			m_ptMtTable->SetCallMode( byMtId, byCallMode );
			m_ptMtTable->SetMtNoRecording(byMtId);
						
			if( !bPassiveAdd )
			{
				m_tConfAllMtInfo.AddMt( byMtId );
			}

			//设置终端状态
			TMtStatus tMtStatus;
			tMtStatus.SetTMt( m_ptMtTable->GetMt( byMtId ) );		
			m_ptMtTable->SetMtStatus( tMtStatus.GetMtId(), &tMtStatus );
		} 
	}
	else if( mtAliasTypeH320ID == tMtAlias.m_AliasType )
	{
		byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAlias );

        BOOL bRepeat = FALSE;
        // guzh [2008/01/22]  似乎终端已存在
        if (byMtId > 0)
        {
            // 对于主动添加，则做别名过滤
            if ( !bPassiveAdd )
            {
                bRepeat = TRUE;
            }
            // 对于终端呼入添加，则若检测原终端不在线就可以替换，否则必须重新分配一个
            else if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
            {
                bRepeat = TRUE;
            }
            
            if (bRepeat)
            {
                ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[AddMt] Mt.%d existed, replace.\n", byMtId);
                return byMtId;
            }
        }

		//原来不存在的终端，分配一个新的给他		
		u8 byLayer   = 0; 
		u8 bySlot    = 0;
		u8 byChannel = 0;
		byMtId = m_ptMtTable->AddMt( &tMtAlias, bRepeat );
		if(byMtId > 0 && tMtAlias.GetH320Alias( byLayer, bySlot, byChannel ) )
		{
			m_ptMtTable->SetDialAlias( byMtId, &tMtAlias );
			m_ptMtTable->SetDialBitrate( byMtId, wMtDialRate );


			m_ptMtTable->SetMcuId( byMtId, LOCAL_MCUIDX);
			m_ptMtTable->SetConfIdx( byMtId, m_byConfIdx );
			m_ptMtTable->SetCallMode( byMtId, byCallMode );
			m_ptMtTable->SetMtNoRecording(byMtId);
			
			if( !bPassiveAdd )
			{
				m_tConfAllMtInfo.AddMt( byMtId );
			}

			//设置终端状态
			TMtStatus tMtStatus;
			tMtStatus.SetTMt( m_ptMtTable->GetMt( byMtId ) );		
			m_ptMtTable->SetMtStatus( tMtStatus.GetMtId(), &tMtStatus );
		}
	}

	return byMtId;
}

/*====================================================================
    函数名      ：AssignMpForMt
    功能        ：给新入会的终端分配MP
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt &tMt, 加入的与会终端
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	07/05/31    4.0         张宝卿        创建
====================================================================*/
BOOL32 CMcuVcInst::AssignMpForMt( TMt & tMt )
{
    CServMsg	cServMsg;
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetConfIdx(m_byConfIdx);

    u8 byMpId = m_ptMtTable->GetMpId(tMt.GetMtId());
    
    if ( !g_cMcuVcApp.IsMpConnected(byMpId) || !g_cMcuVcApp.IsMtAssignInMp(byMpId, m_byConfIdx, tMt.GetMtId()) )
    {
        // guzh [12/29/2006] 获取该终端的别名
        TMtAlias tMtAlias;        
        if ( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeE164, &tMtAlias ) )
        {
            tMtAlias.m_tTransportAddr.SetIpAddr( m_ptMtTable->GetIPAddr(tMt.GetMtId()) );                    
        }
        else
        {
            m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeTransportAddress, &tMtAlias);
        }
        
        // MP能力限制 PM数 约束支持 [02/09/2007-zbq]
        byMpId = g_cMcuVcApp.AssignMpIdByBurden(m_byConfIdx, tMtAlias, tMt.GetMtId(), m_tConf.GetBitRate());
        
        if ( byMpId == 0 ) 
        {
            // guzh [3/30/2007] 通知MCS
            u32 dwIp = htonl(0);
            cServMsg.SetMsgBody((u8*)&dwIp, sizeof(u32));
            cServMsg.SetEventId(MCU_MCS_MPFLUXOVERRUN_NOTIFY);
            SendMsgToAllMcs( MCU_MCS_MPFLUXOVERRUN_NOTIFY, cServMsg );
            
            ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "AssignMpForMt(%d,%d) failed\n", tMt.GetMcuId(), tMt.GetMtId());
            RemoveJoinedMt( tMt, TRUE );
        }
        if (!g_cMcuVcApp.IsMpConnected(byMpId))
        {
            ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[AddJoinedMt] byMpId.%d is OFFline for MT.%d\n", byMpId, tMt.GetMtId());
            return FALSE;
        }
        m_ptMtTable->SetMpId(tMt.GetMtId(), byMpId);
    }
    
    return TRUE;
}

/*====================================================================
    函数名      ：AddJoinedMt
    功能        ：会议信息中加入与会终端；
	              设置Mt的优先会议；
	              通知MC，MT和SMCU；
				  如果有发言向该MT建立交换
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt &tMt, 加入的与会终端
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/01/02    1.0         LI Yi         创建
	04/01/12    3.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::AddJoinedMt( TMt &tMt )
{
	CServMsg	cServMsg;
    cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetConfIdx(m_byConfIdx);

	BOOL32 bHasMsgMcsAlias = FALSE;

    // xsl [11/8/2006] 分配转发板id
    if ( !AssignMpForMt(tMt) )
    {
        ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[AddJoinedMt] AssignMpForMt.%d failed !\n", tMt.GetMtId() );
        return;
    }

	//判断是否已加入(是否终端创会)
	if( m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
	{
        //终端创会：别名上报
        SendMtAliasToMcs( tMt );

        //终端创会：发会议通知
        cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
        SendMsgToMt( tMt.GetMtId(), MCU_MT_CONF_NOTIF, cServMsg );

		return;
	}
	
	//通知其它终端
	cServMsg.SetMsgBody( ( u8 * )&tMt, sizeof( tMt ) );
	BroadcastToAllSubMtJoinedConf( MCU_MT_MTJOINED_NOTIF, cServMsg );

	//bHasMsgMcsAlias = m_tConfAllMtInfo.MtInConf( tMt.GetMtId() ); 

	//加入与会终端列表
	m_tConfAllMtInfo.AddJoinedMt( tMt.GetMtId() );

	m_ptMtTable->InitMtVidAlias( tMt.GetMtId() );

	if (m_ptMtTable->GetAddMtMode(tMt.GetMtId()) == ADDMTMODE_CHAIRMAN)
	{
		m_ptMtTable->SetAddMtMode(tMt.GetMtId(), ADDMTMODE_MCS);
	}

    //若是主席加入
    if( m_tConf.HasChairman() &&  m_tConf.GetChairman() == tMt )
    {
        ChangeChairman( &tMt );
    }

	//清空广播记录
	m_ptMtTable->ClearMonitorSrcAndDst( tMt.GetMtId() );
	m_ptMtTable->SetMtMulticasting( tMt.GetMtId(), FALSE );

	//发会议通知
	TMt tSpeaker = m_tConf.GetSpeaker();
	m_tConf.SetSpeaker( GetLocalSpeaker() );
	cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
	SendMsgToMt( tMt.GetMtId(), MCU_MT_CONF_NOTIF, cServMsg );
	m_tConf.SetSpeaker( tSpeaker );
	
	MtOnlineChange( tMt, TRUE, 0 );
	
	//是否全体静音，是则强制终端静音
	if( TRUE == m_tConf.GetConfAttrb().IsAllInitDumb() )
	{
		if(m_ptMtTable->GetManuId(tMt.GetMtId()) == MT_MANU_KDC)
		{
			u8 byMuteOpenFlag = 1;
			cServMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
			cServMsg.CatMsgBody( &byMuteOpenFlag, sizeof(byMuteOpenFlag) );
			cServMsg.SetDstMtId( tMt.GetMtId() );
			SendMsgToMt( tMt.GetMtId(), MCU_MT_MTDUMB_CMD, cServMsg );
		}
		else
		{
			TMtStatus tMtStatus;
			m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tMtStatus);
			tMtStatus.SetCaptureMute(TRUE);
			m_ptMtTable->SetMtStatus(tMt.GetMtId(), &tMtStatus);
			NotifyMtSend( tMt.GetMtId(), MODE_AUDIO, FALSE );
            MtStatusChange( &tMt, TRUE );
		}
	}

    //[2011/05/25 zhushz]VCS组呼会议被级联，自动切换成呼叫锁定状态
    if (MT_TYPE_MMCU == tMt.GetMtType() &&
        VCS_CONF == m_tConf.GetConfSource() &&
        ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
        !m_cVCSConfStatus.IsGroupModeLock())
    {
        m_cVCSConfStatus.SetGroupModeLock(TRUE);
        VCSConfStatusNotif();
    }
	//通知会控
//	cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
//	SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
	
	//发给会控会议所有终端信息
	//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
	//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

	//发给会控终端表

	SendMtListToMcs(LOCAL_MCUIDX);
	
// 	if( !bHasMsgMcsAlias )
// 	{		
		SendMtAliasToMcs( tMt );
/*	}*/
	
	// 发Trap消息 MCU呼叫终端与会, 与会终端信息以Trap方式发送给网管
	TMtNotify tMtNotify;
	CConfId   cConfId;
	TMtAlias  tMtAlias;
	u8        byLen = 0;
	cConfId = m_tConf.GetConfId();
	memset( &tMtNotify, 0, sizeof(tMtNotify) );
	memcpy( tMtNotify.m_abyConfId, &cConfId, 16 );

	if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeTransportAddress, &tMtAlias ) )
	{
		byLen = byLen + sprintf( (s8 *)tMtNotify.m_abyMtAlias, StrOfIP( tMtAlias.m_tTransportAddr.GetIpAddr() ) );		
	}
	if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH323ID, &tMtAlias ) )
	{
		byLen = byLen + sprintf( (s8 *)tMtNotify.m_abyMtAlias + byLen, "(%s)%c", tMtAlias.m_achAlias, 0 );
	}
	if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH320ID, &tMtAlias ) )
	{
		byLen = byLen + sprintf( (s8 *)tMtNotify.m_abyMtAlias + byLen, "(%s)%c", tMtAlias.m_achAlias, 0 );
	}
	if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH320Alias, &tMtAlias ) )
	{
		byLen = byLen + sprintf( (s8 *)tMtNotify.m_abyMtAlias + byLen, "(%s)%c", tMtAlias.m_achAlias, 0 );
	}
	tMtNotify.m_abyMtAlias[sizeof(tMtNotify.m_abyMtAlias)-1] = 0;
	SendTrapMsg( SVC_AGT_ADD_MT, (u8*)&tMtNotify, sizeof(tMtNotify) );

    //n+1备份更新终端信息
    if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
    {
        ProcNPlusConfMtInfoUpdate();
    }

    //zbq[12/18/2007] 获取终端版本信息
//     if ( tMt.GetMtType() == MT_TYPE_MT )
//     {
//         CServMsg cMsg;
//         cMsg.SetEventId(MCU_MT_GETMTVERID_REQ);
//         SendMsgToMt(tMt.GetMtId(), MCU_MT_GETMTVERID_REQ, cMsg);        
//     }

	//打印消息
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "Mt%d: 0x%x(Dri:%d) joined conf success!\n", 
			  tMt.GetMtId(), m_ptMtTable->GetIPAddr(tMt.GetMtId()), tMt.GetDriId() );
}

/*====================================================================
    函数名      ：RemoveMt
    功能        ：将受邀终端从会议中删除，亦自动将其从与会终端中删除；
	              通知MC，MT和SMCU；
	              停止向该MT交换；
				  如果会议中无终端自动释放该会议;
				  如果为直连终端，清除其优先会议；
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt &tMt, 要删除的终端
				  BOOL32 bMsg, 是否发命令给该终端要求退出
    返回值说明  ：TRUE/FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/01/03    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::RemoveMt( TMt &tMt, BOOL32 bMsg, BOOL32 bIsSendMsgToMcs /*= TRUE*/ )
{
	CServMsg	cServMsg;
	BOOL32      bNeedNotifyMt = FALSE;
		
	cServMsg.SetConfId( m_tConf.GetConfId() );
	cServMsg.SetConfIdx( m_byConfIdx );
	
    tMt = m_ptMtTable->GetMt(tMt.GetMtId());
	
	// 记录在线状态
	BOOL32 bIsJoinedConf = m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId());
	u8 byType = m_ptMtTable->GetMtType( tMt.GetMtId() );
	m_ptMtTable->SetNotInvited( tMt.GetMtId(), FALSE );
	RemoveJoinedMt( tMt, bMsg );

	ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "RemoveMt(%d,%d), bMsg.%d\n", tMt.GetMcuId(), tMt.GetMtId(), bMsg);

	// 删除不在线终端时做处理
	if (!bIsJoinedConf)
	{
		RemoveUnjoinedMtFormPeriInfo(tMt);
	}

	//判断是否是主席或者发言终端
	if( tMt == m_tConf.GetChairman() )
	{
		bNeedNotifyMt = TRUE;
		m_tConf.SetNoChairman();
	}

	//判断是否发言终端
	if( tMt == GetLocalSpeaker() )
	{
		bNeedNotifyMt = TRUE;
		m_tConf.SetNoSpeaker();
	}

	//从会议信息中删除
	m_tConfAllMtInfo.RemoveMt( tMt.GetMtId() );
	if( MT_TYPE_SMCU == byType )
	{
		TConfMtInfo *pcInfo = m_tConfAllMtInfo.GetMtInfoPtr(GetMcuIdxFromMcuId(tMt.GetMtId()));
		if(pcInfo != NULL)
		{
			pcInfo->SetNull();
		}
	}
			
	//通知所有下级终端
	cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
	if( bNeedNotifyMt )
	{		
		TMt tSpeaker = m_tConf.GetSpeaker();
		m_tConf.SetSpeaker( GetLocalSpeaker() );
		cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
		BroadcastToAllSubMtJoinedConf( MCU_MT_CONF_NOTIF, cServMsg );
		m_tConf.SetSpeaker( tSpeaker );
	}

	//打印消息
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "Mt%d: 0x%x(Dri:%d) removed from conf!\n", 
			       tMt.GetMtId(), m_ptMtTable->GetIPAddr(tMt.GetMtId()), tMt.GetDriId() );

	//从会议终端表中删除
	m_ptMtTable->DelMt( tMt.GetMtId() );

	NotifyMcuDelMt(tMt);

	//zjj20110118 非邀请终端在删除时应该清空,避免在主动呼入终端后当作非受邀终端处理
	if( m_byMtIdNotInvite == tMt.GetMtId() )
	{
		m_byMtIdNotInvite = 0;
	}

	if( m_byOtherConfMtId == tMt.GetMtId() )
	{
		SetInOtherConf( FALSE );
	}

	if( VCS_CONF == m_tConf.GetConfSource() )
	{
		BOOL32 bDeled = m_cVCSConfStatus.OprNewMt(tMt, FALSE);
	}

	if( bIsSendMsgToMcs )
	{
		if( VCS_CONF == m_tConf.GetConfSource() )
		{			
			VCSConfStatusNotif();
		}
		//发给会控会议所有终端信息
		//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
		//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
		SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

		// [11/21/2011 liuxu] 补上
		//发给会控终端表
		SendMtListToMcs(LOCAL_MCUIDX);
	}

	//发给会控会议信息表
	cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
	if(m_tConf.HasConfExInfo())
	{
		u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
		u16 wPackDataLen = 0;
		PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
		cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);
	}
	
	SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );

    // zbq [03/23/2007] 如果是轮询列表里的终端，不通知会控刷轮询列表，会控自己维护；
    u8 byMtPos;
    if ( m_tConfPollParam.IsExistMt(tMt, byMtPos) )
    {
        m_tConfPollParam.RemoveMtFromList(tMt);
		m_tConfPollParam.SpecPollPos(byMtPos);
    }
	//该终端在电视墙轮询列表中则清除
	for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
	{
		if (m_tTWMutiPollParam.m_tTWPollParam[byIdx].IsExistMt(tMt,byMtPos))
		{
			m_tTWMutiPollParam.m_tTWPollParam[byIdx].RemoveMtFromList(tMt);
		}
	}
	//会议中无终端自动结束会议
	if( (m_tConfAllMtInfo.GetLocalMtNum() == 0 && m_tConf.GetConfAttrb().IsReleaseNoMt()))
	{
		ConfPrint( LOG_LVL_WARNING, MID_PUB_ALWAYS, "There's no MT in conference %s now. Release it!\n", 
				 m_tConf.GetConfName() );
		ReleaseConf( TRUE );
		NEXTSTATE( STATE_IDLE );
	}

    //VCS会议中该会议为下级会议，上级会议结束,
	//修改会议的创建者属性,通告VCS会议级联调用已释放,将会议调度权限重新交给VCS	
	if ( VCS_CONF == m_tConf.GetConfSource() && 
		 MT_TYPE_MMCU == tMt.GetMtType() &&
		 CONF_CREATE_MT == m_byCreateBy)
	{
		RestoreVCConf(VCS_SINGLE_MODE);
 		m_byCreateBy = CONF_CREATE_MCS;
		cServMsg.SetEventId(MCUVC_VCSSN_MMCURLSCTRL_CMD);
		CMcsSsn::SendMsgToMcsSsn( CInstance::DAEMON, MCUVC_VCSSN_MMCURLSCTRL_CMD,
			                      cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	}

}

/*=============================================================================
  函 数 名： RestoreVidSrcBitrate
  功    能： 恢复视频源码率
  算法实现： 
  全局变量： 
  参    数： TMt tMt
  返 回 值： void 
=============================================================================*/
void CMcuVcInst::RestoreVidSrcBitrate(TMt tMt, u8 byMode)
{
	u16 wMinBitRate = 0;

	TSimCapSet tDstSimCapSet;
	
    //  若为第二通道，则直接发送flowcontrol进行调节
	if( MODE_SECVIDEO == byMode )
	{
		if(!m_tDoubleStreamSrc.IsNull())
		{
            // guzh [3/13/2007] 同时考虑发送端能力上限
            u16 wSndBitrate = GetDoubleStreamVideoBitrate(
                                m_ptMtTable->GetDialBitrate(m_tDoubleStreamSrc.GetMtId()),
                                FALSE);
            wMinBitRate = GetLeastMtReqBitrate(FALSE, MEDIA_TYPE_NULL, m_tDoubleStreamSrc.GetMtId()); 
            wMinBitRate = min(wSndBitrate, wMinBitRate);
            AdjustMtVideoSrcBR( tMt.GetMtId(), wMinBitRate, MODE_SECVIDEO );
			return;
		}
	}

    //modify bas 2 -- 是否需要区分不同格式的最小值
    tDstSimCapSet = m_ptMtTable->GetDstSCS(tMt.GetMtId());
    if (MODE_VIDEO == byMode)
    {
        wMinBitRate = GetLeastMtReqBitrate(TRUE, tDstSimCapSet.GetVideoMediaType(), tMt.GetMtId());
    }        

	//若会议正在广播画面合成图像,降画面合成码率
    if (g_cMcuVcApp.GetVMPMode(m_tVmpEqp) != CONF_VMPMODE_NONE 
		&& g_cMcuVcApp.IsBrdstVMP(m_tVmpEqp)
		&& MODE_VIDEO == byMode
		)
    {
        /*
		u16 wBasBitrate;
        u8  byVmpChanNo;
        u8  byMediaType = m_tConf.GetMainVideoMediaType();
        
        //双速会议要考虑入会时的呼叫码率
        if ( 0 != m_tConf.GetSecBitRate() && 
            MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType())
        {
            if (m_ptMtTable->GetDialBitrate(tMt.GetMtId()) == m_tConf.GetBitRate())
            {
                byVmpChanNo = 1;
                wBasBitrate = m_wVidBasBitrate;
            }
            else
            {
                byVmpChanNo = 2;
                wBasBitrate = m_wBasBitrate;
            }
        }
        //单速或双格式会议
        else
        {
            if (tDstSimCapSet.GetVideoMediaType() == byMediaType)
            {
                byVmpChanNo = 1;
                wBasBitrate = m_wVidBasBitrate;
            }
            else
            {
                byVmpChanNo = 2;
                wBasBitrate = m_wBasBitrate;
            }
        }
        
        if (wMinBitRate != wBasBitrate && MODE_VIDEO == byMode)
        {
            ChangeVmpBitRate(wMinBitRate, byVmpChanNo);                
        }
		*/
		AdjustVmpBrdBitRate(&tMt);
        return;
    }
   
    //不启用适配,直接降发言人码率
	if( !m_tConf.GetConfAttrb().IsUseAdapter() )
	{
		AdjustMtVideoSrcBR( tMt.GetMtId(), wMinBitRate, byMode );
	}
	//过适配的逻辑已无用，本接口只有在删除终端或者终端强制码率控制会调用
	//这两种场景下都会进行RefreshBasParam,无需再这里再次刷参数
//     else
//     {
//         u16 wAdaptBitRate = 0;      
//            
//         //双格式会议需类型适配终端降适配码率
//         if (IsMtNeedAdapt(ADAPT_TYPE_VID, tMt.GetMtId())  &&
//             (IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate) || IsNeedCifAdp()))
//         {
//             if (wMinBitRate != m_wVidBasBitrate)
//             {
//                 if (m_tConf.m_tStatus.IsVidAdapting())
//                 {
//                     ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd1] change vid bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
//                            wMinBitRate, m_wVidBasBitrate);
//                     ChangeAdapt(ADAPT_TYPE_VID, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
//                 }                
//             }            
//         }
//         //双速会议降速或变速（需要适配）
//         else if (IsMtNeedAdapt(ADAPT_TYPE_BR, tMt.GetMtId()) &&
//             IsNeedBrAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate))
//         {
//             if (wMinBitRate != m_wBasBitrate)
//             {
//                 if (m_tConf.m_tStatus.IsBrAdapting())
//                 {
//                     ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd3]change br bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
//                            wMinBitRate, m_wBasBitrate);
//                     ChangeAdapt(ADAPT_TYPE_BR, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
//                 }
//                 else
//                 {
//                     ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd3]start br bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
//                            wMinBitRate, m_wBasBitrate);
//                     StartAdapt(ADAPT_TYPE_BR, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
//                 }                   
//                 
//                 StartSwitchToSubMt(m_tBrBasEqp, m_byBrBasChnnl, tMt.GetMtId(), MODE_VIDEO);
//             }
//             else if (m_tConf.m_tStatus.IsBrAdapting())//若不需要调节适配码率，直接将适配码流交换到此mt(会议中有其他终端的接收码率更低的情况)
//             {
//                 ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd1] switch br bas to mt<%d>\n", tMt.GetMtId());
//                 StartSwitchToSubMt(m_tBrBasEqp, m_byBrBasChnnl, tMt.GetMtId(), MODE_VIDEO);
//             }
//         }         
//         //双格式会议无需适配终端降其视频源码率，可能会将码流压到最低
//         else if (MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() || IsNeedCifAdp())
//         {
//             AdjustMtVideoSrcBR(tMt.GetMtId(), wMinBitRate, byMode);
//         }
//     }	

	return;
}

/*====================================================================
    函数名      ：RemoveJoinedMt
    功能        ：将与会终端从会议中删除；
	              通知MC，MT和SMCU；
	              停止向该MT交换
				  如果该终端是混音成员，删除其对应的混音channel
				  如果是数据终端, 且在数据会议中, 通知DCS删除之
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt &tMt, 要删除的终端
				  BOOL32 bMsg, 是否发命令给该终端要求退出
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/01/03    1.0         LI Yi         创建
	05/12/20	4.0			张宝卿		  T120集成终端删除
	10/11/25    5.0         苗庆松        Trap消息发送(网管5.0拓扑显示)
====================================================================*/
void CMcuVcInst::RemoveJoinedMt( TMt & tMt, BOOL32 bMsg, u8 byReason,BOOL32 bNeglectMtType )
{
	CServMsg	cServMsg;
    cServMsg.SetConfId( m_tConf.GetConfId() );
    cServMsg.SetConfIdx( m_byConfIdx );
	
	u8			byMode;
	
	//BOOL32		bIsExceptionReason = FALSE;

	// [11/29/2011 liuxu] 提高优先级,便于测试
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "RemoveJoinedMt(%d,%d), Reason.%d, NeglectMtType.%d\n", tMt.GetMcuId(), tMt.GetMtId(), byReason, bNeglectMtType);
	
	//刷新第一路适配参数
	if (m_tConf.m_tStatus.IsVidAdapting())
	{
		RefreshBrdBasParamForSingleMt(tMt.GetMtId(), MODE_VIDEO, TRUE);
	}	
	
	//刷新第二路适配参数
	if (!m_tDoubleStreamSrc.IsNull() && 
		!(m_tDoubleStreamSrc == tMt))
	{
		RefreshBrdBasParamForSingleMt(tMt.GetMtId(), MODE_SECVIDEO, TRUE);
	}
	
    // xsl [11/10/2006] 释放接入和转发资源
    ReleaseMtRes( tMt.GetMtId());
	

	if( tMt.GetMtId() == m_byNewTokenReqMtid )
	{
		m_byNewTokenReqMtid = 0;
	}
	
	//是否入会
	if (!m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId()))
	{
		TConfAttrb tConfattrb = m_tConf.GetConfAttrb();
		// 2011-9-1 add by pgf: 如果有VMP模板，必须删除模板中的信息
		if ( !m_tConfInStatus.IsVmpModuleUsed()	// 特殊场景：开启会议，没有VMP，此时删除VMP模板的成员，将导致开启VMP缺少该成员
			&& tConfattrb.IsHasVmpModule()
			&& m_tConfEqpModule.IsMtInVmp(tMt) )
		{
			//删除模板成员
			m_tConfEqpModule.RemoveSpecMtFromModul(tMt);
		}
		
		RemoveMcu( tMt );

		//[5/8/2012 chendaiwei]释放端口
		g_cMcuVcApp.ReleaseMtPort( m_byConfIdx, tMt.GetMtId() );
		m_ptMtTable->ClearMtSwitchAddr( tMt.GetMtId() );

		
		m_tConfEqpModule.RemoveMtInHduVmpModule(tMt);

		//通知
		if (bMsg)
		{
			SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);
		}		
		return;
	}
	
	//将停止发送主流，放到后面。因为如果该终端也在发送双流的话，在StopDoubleStream中恢复发送主流，会导致计数错误
	/*if (m_tConf.GetConfAttrb().IsSatDCastMode())
	{
		NotifyMtSend(tMt.GetMtId(),MODE_VIDEO,FALSE);
		NotifyMtSend(tMt.GetMtId(),MODE_SECVIDEO,FALSE);
	}*/
	//删除其在会议模板中电视墙成员的信息
	RemoveMtInMutiTvWallMoudle(tMt);

	if( m_byCreateBy == CONF_CREATE_NPLUS )
	{
		g_cMcuVcApp.NplusRemoveVmpMember(tMt.GetMtId(),m_byConfIdx);
	}
	
	//zjj20091014 终端离线原因判断
	if( MTLEFT_REASON_EXCEPT == byReason ||
		MTLEFT_REASON_RTD == byReason ||
		MTLEFT_REASON_UNREACHABLE == byReason )	 
	{		
		TMt tLocalMt = GetLocalMtFromOtherMcuMt( tMt );
		m_ptMtTable->SetDisconnectReason( tMt.GetMtId(),byReason );
		//bIsExceptionReason = TRUE;		
	}
	
	// 	//移除
	// 	m_tConfAllMtInfo.RemoveJoinedMt( tMt.GetMtId() );  
	
	// VCS会议处理需处理主席掉线
	if (VCS_CONF == m_tConf.GetConfSource() && tMt == m_tConf.GetChairman())
	{
		VCSChairOffLineProc();
	}
	
    // guzh [4/29/2007] 
    if ( MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) )
    {
        KillTimer(MCU_SMCUOPENDVIDEOCHNNL_TIMER);
    }	
	
	//songkun,20110530,注掉，放后面进行，挂掉后无法升码率问题
	//恢复源码率（源可能为bas、vmp、mt）
	// 	if (m_tConf.GetSecBitRate() == 0)
	// 	{
	// 		//防止取最小码率时取到
	// 		m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), m_tConf.GetBitRate(), LOGCHL_VIDEO);
	// 	}	
	// 	RestoreVidSrcBitrate(tMt, MODE_VIDEO);
	//     if (!m_tDoubleStreamSrc.IsNull() && !(tMt == m_tDoubleStreamSrc))
	//     {
	//         //防止取最小码率时取到
	//         m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), m_tConf.GetBitRate(), LOGCHL_SECVIDEO);
	//         RestoreVidSrcBitrate(tMt, MODE_SECVIDEO);
	//     }    
	
    //拆除dump	
	if ((TYPE_MT == tMt.GetType() &&
		(MT_TYPE_MMCU == tMt.GetMtType() ||
		MT_TYPE_SMCU == tMt.GetMtType())) //没有发生过回传或广播的MCU也要拆dump
        ||
        m_ptMtTable->IsMtAudioSending(tMt.GetMtId()))
	{
		g_cMpManager.StopRecvMt( tMt, MODE_AUDIO );
	}
	
    if ((TYPE_MT == tMt.GetType() &&
		(MT_TYPE_MMCU == tMt.GetMtType() ||
		MT_TYPE_SMCU == tMt.GetMtType())) //没有发生过回传或广播的MCU也要拆dump
        ||
        m_ptMtTable->IsMtVideoSending(tMt.GetMtId()))
	{
		g_cMpManager.StopRecvMt(tMt, MODE_VIDEO);
	}
	
    //拆除交换桥 // libo [5/13/2005]
    g_cMpManager.RemoveSwitchBridge(tMt, 0);
	
    // [pengjie 2010/4/17]  清该终端在外设中的信息、改变外设参数和状态、调整相应交换
	RemoveMtFormPeriInfo( tMt, byReason );	
	
	//离开的终端是发言者
	if( GetLocalSpeaker() == tMt )
	{
		//混音模式下只对VIDEO部分进行处理
		byMode =  m_tConf.m_tStatus.IsMixing() ? MODE_VIDEO : MODE_BOTH;
		
		g_cMpManager.RemoveSwitchBridge( tMt, 0, byMode);
		
		if( !m_tConf.m_tStatus.IsNoRecording() && !g_cMcuVcApp.IsBrdstVMP(m_tVmpEqp) )
		{
			StopSwitchToPeriEqp( m_tRecEqp.GetEqpId(), m_byRecChnnl, FALSE, byMode );
		}
		
        //changevid 时会拆除交换，这里没必要啊
		//         if (m_tConf.m_tStatus.IsAudAdapting())
		//         {
		//             StopSwitchToPeriEqp(m_tAudBasEqp.GetEqpId(), m_byAudBasChnnl, FALSE, byMode);
		//         }
		//         if (m_tConf.m_tStatus.IsVidAdapting())
		//         {
		//             StopSwitchToPeriEqp(m_tVidBasEqp.GetEqpId(), m_byVidBasChnnl, FALSE, byMode);
		//         }
		//         if (m_tConf.m_tStatus.IsBrAdapting())
		//         {
		//             StopSwitchToPeriEqp(m_tBrBasEqp.GetEqpId(), m_byBrBasChnnl, FALSE, byMode);
		//         }
	}
	
    if (MT_MANU_KDCMCU != m_ptMtTable->GetManuId(tMt.GetMtId()))
    {
        ClearH239TokenOwnerInfo( &tMt );
    }
    
	
	
    //停发第二路视频发送通道
    if(m_tDoubleStreamSrc == m_ptMtTable->GetMt(tMt.GetMtId()))
    {
        StopDoubleStream( TRUE, TRUE );
    }
    else
    {
        //sony G70SP在H263-H239时采用活动双流,且不使用令牌而使用FlowControlIndication进行发送激活或关闭 或者
        //POLYCOM7000的静态双流时，使用令牌进行发送激活或关闭
        //均在呼叫建立时即打开,之后与会期间保留通道，直至退出会议
        TLogicalChannel tH239LogicChnnl;
        if( ( ( MT_MANU_SONY == m_ptMtTable->GetManuId(tMt.GetMtId()) && 
            MEDIA_TYPE_H263 == m_tConf.GetCapSupport().GetDStreamMediaType() &&
            m_tConf.GetCapSupport().IsDStreamSupportH239()) || 
            MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) ) && 
            m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tH239LogicChnnl, FALSE ) )			
        {
            u8 byOut = 0;
            TLogicalChannel tNullLogicChnnl;
            cServMsg.SetMsgBody( ( u8 * )&tH239LogicChnnl, sizeof( tH239LogicChnnl ) );	
            cServMsg.CatMsgBody( &byOut, sizeof(byOut) );
            SendMsgToMt( tMt.GetMtId(), MCU_MT_CLOSELOGICCHNNL_CMD, cServMsg );
            m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tNullLogicChnnl, FALSE );
        }
    }
    
    //清除第二路视频接收通道
    //fix:卫星分散会议，拆单播终端交换
    if ( !m_tDoubleStreamSrc.IsNull()
		// 所有终端统一处理 [pengguofeng 3/5/2013]
// 		&& 
//          ( !m_tConf.GetConfAttrb().IsSatDCastMode() ||
//            !IsMultiCastMt( tMt.GetMtId() ) 
//          ) 
       )
    {
		// 		if (IsNeedAdapt(m_tDoubleStreamSrc, tMt, MODE_SECVIDEO))
		// 		{
		// 			StopDSAdapt(m_tDoubleStreamSrc, tMt);
		// 		}
		// 		else
		{
			//zjl 20110510 Mp: StopSwitchToSubMt 接口重载替换 
			//g_cMpManager.StopSwitchToSubMt(tMt, MODE_SECVIDEO, TRUE);
			g_cMpManager.StopSwitchToSubMt(m_byConfIdx, 1, &tMt, MODE_SECVIDEO);
		}     
    }

	//如果正处于“主席轮询选看”模式下，挂断或者删除主席终端时要停主席轮询选看
	//主席轮询过程中，主席掉线了，就要停主席轮询
	if( /*(MTLEFT_REASON_NORMAL == byReason || MTLEFT_REASON_LOCAL == byReason) && */
		((CONF_POLLMODE_VIDEO_CHAIRMAN == m_tConf.m_tStatus.GetPollMode())||
		(CONF_POLLMODE_BOTH_CHAIRMAN == m_tConf.m_tStatus.GetPollMode())) && 
		(tMt == m_tConf.GetChairman()) )
	{
		ProcStopConfPoll();
	}
	//vcs会议，当前主席（有备份主席的话已经切过了）为掉线终端（没备份主席或者备份主席不在线）停掉当前的主席轮询
	if ( VCS_CONF == m_tConf.GetConfSource() && tMt == m_tConf.GetChairman()
			&& m_cVCSConfStatus.GetChairPollState() != VCS_POLL_STOP 
		)
	{
		//此时，主席还认为是在线的，VcsStopChairmanPoll里面对在线的主席可能会去选看当前调度终端。
		//临时线让其掉线，处理完后恢复，掉线处理在后面处理
		m_tConfAllMtInfo.RemoveJoinedMt( tMt.GetMtId() );
		VcsStopChairManPoll();
		m_tConfAllMtInfo.AddJoinedMt( tMt.GetMtId() );
	}
	
	//判断是否是主席终端
	if( tMt == m_tConf.GetChairman() )
	{
		// 此时，该终端还认为是在线的，若该终端既是主席又是发言人时，自动画面合成时，会导致画面合成第0通道变为发言人。
		// 追加接口，交换已经调整过，不做交换调整处理
		ChangeChairman( NULL , FALSE);
		if( MT_TYPE_MMCU == tMt.GetMtType() )
		{
			TMt tNullMt;
			tNullMt.SetNull();
			m_tConf.SetChairman( tNullMt );
		}
		else
		{
			m_tConf.SetChairman( tMt );
		}
	}
	
    // 顾振华@2006.4.17 要检查本终端是否在轮询
    BOOL32 bIsMtInPolling = FALSE;
    if ( CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollState() )
    {
        // guzh [6/14/2007]
        if (tMt == (TMt)m_tConf.m_tStatus.GetMtPollParam() ||
            (!m_tConf.m_tStatus.GetMtPollParam().IsLocal() && 
			MT_TYPE_SMCU == m_ptMtTable->GetMtType( tMt.GetMtId() ) &&
			IsMtInMcu( tMt,(TMt)m_tConf.m_tStatus.GetMtPollParam() )
            /*m_tConf.m_tStatus.GetMtPollParam().GetMcuId() == tMt.GetMtId()*/
			)
			)
        {
            bIsMtInPolling = TRUE;
        }
    }    
	
	//判断是否为发言终端
	//zjj20120904 本地发言人掉线后再上线恢复，下级终端或上级作为发言人就不恢复
	if( tMt == GetLocalSpeaker() )
	{
		TMt tSpeaker = m_tConf.GetSpeaker();
		ChangeSpeaker( NULL,  bIsMtInPolling, FALSE);//不调整vmp，已在RemoveMtFormPeriInfo中调整
		
		if( MT_TYPE_MMCU == tMt.GetMtType() )
		{			
			m_tConf.SetNoSpeaker();
		}
		else
		{
			if( tSpeaker.IsLocal() )
			{
				m_tConf.SetSpeaker( tMt );
			}			
		}
	}
	else if (tMt == GetSecVidBrdSrc())
	{
		TMt tNullMt;
		ChangeSecSpeaker(tNullMt, FALSE);
	}

    // zbq [03/12/2007] 如果是正在被轮询的终端，直接跳入下一个轮询终端
    if (bIsMtInPolling 
		&& m_tConf.m_tStatus.GetPollState() == POLL_STATE_NORMAL // [5/7/2010 xliang] 轮询暂停态不要设定时
		)
    {
        SetTimer( MCUVC_POLLING_CHANGE_TIMER, 500 );
    }

	//检查VCS是否在主席轮询，正在轮询，跳入下一个轮询终端 [6/7/2012 chendaiwei]
	if ( m_cVCSConfStatus.GetChairPollState() == VCS_POLL_START 
		&& tMt.IsLocal()
		&& tMt == m_cVCSConfStatus.GetCurChairPollMt() )
	{
		SetTimer(MCUVC_VCS_CHAIRPOLL_TIMER, 10);

		ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_CONF,"[VcsChairmanPoll] poll to next mt for cur mt %d removed!\n",tMt.GetMtId());
	}

	//停止向这台终端交换
    //fix:卫星分散会议，拆单播终端交换
//     if ( !m_tConf.GetConfAttrb().IsSatDCastMode() ||
//          !IsMultiCastMt( tMt.GetMtId() ))
//     {
	//zjj20100422 对于挂断上级mcu这里先不拆交换,在后面拆交换
	if( m_tCascadeMMCU.IsNull() || !(m_tCascadeMMCU == tMt) )
	{
		//zjl 20110510 StopSwitchToSubMt 接口重载替换
		//StopSwitchToSubMt( tMt.GetMtId(), MODE_BOTH );
		StopSwitchToSubMt(1, &tMt, MODE_BOTH);
	}
    /*}	
    else
    {
        // 卫星会议则提示终端看自己
        NotifyMtReceive( tMt, tMt.GetMtId() );
    }*/
	
	//判断是否为回传通道,是则清除回传交换
	if( !m_tCascadeMMCU.IsNull() && !(m_tCascadeMMCU == tMt) )
	{
		u16 wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMMcuIdx);
		if( ptConfMcInfo != NULL && 
			( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
		{
			if( ptConfMcInfo->m_tSpyMt.GetMtId() == tMt.GetMtId() &&
				ptConfMcInfo->m_tSpyMt.GetMcuId() == tMt.GetMcuId() )
			{
				u8 byStopMode = MODE_BOTH;
				if (m_tConf.m_tStatus.IsMixing())
				{
					byStopMode = MODE_VIDEO;
				}
				//zjl 20110510 StopSwitchToSubMt 接口重载替换
				//StopSwitchToSubMt( m_tCascadeMMCU.GetMtId(), byMode );
				StopSwitchToSubMt(1, &m_tCascadeMMCU, byStopMode);
				//未拆桥，可能留下隐患？？？
			}
		}
	}
	
	//更改组播记录
	u8 byMtList[MAXNUM_MT_CHANNL];
	u8 byMtNum = m_ptMtTable->GetMonitorSrcMtList( tMt.GetMtId(), byMtList );
	TMt tSrcMt;
	for( u8 byLoop = 0; byLoop < byMtNum; byLoop++ )
	{
		m_ptMtTable->RemoveMonitorDstMt( byMtList[byLoop], tMt.GetMtId() );
		if( !m_ptMtTable->HasMonitorDstMt( byMtList[byLoop] ) )
		{
			tSrcMt = m_ptMtTable->GetMt( byMtList[byLoop] );
			g_cMpManager.StopMulticast( tSrcMt, 0, MODE_BOTH, FALSE );	
			m_ptMtTable->SetMtMulticasting( byMtList[byLoop], FALSE );
		}
	}
	
	//停组播
	if( m_ptMtTable->IsMtMulticasting( tMt.GetMtId() ) )
	{
		//[1/24/2013 chendaiwei]会议组播时，挂断发言人导致画面合成至组播地址的交换被拆除
		if ( m_tConf.GetConfAttrb().IsMulticastMode() && tMt.GetMtId() == GetLocalVidBrdSrc().GetMtId())
		{
			g_cMpManager.StopMulticast( tMt, 0, MODE_VIDEO );
		}
		
		if ( m_tConf.GetConfAttrb().IsMulticastMode() && tMt.GetMtId() == GetLocalAudBrdSrc().GetMtId())
		{
			g_cMpManager.StopMulticast( tMt, 0, MODE_AUDIO );
		}
		
		m_ptMtTable->SetMtMulticasting( tMt.GetMtId(), FALSE );
	}
    
	// 发Trap消息 删除MT时将MT与会信息以Trap方式上报给网管
	TMtNotify tMtNotify;
	TMtAlias tMtAlias;
	CConfId cConfId;
	u8 byLen = 0;
	cConfId = m_tConf.GetConfId();
	memset( &tMtNotify, 0, sizeof(tMtNotify) );
	memcpy( tMtNotify.m_abyConfId, &cConfId, 16 );
	
	if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeTransportAddress, &tMtAlias ) )
	{
		byLen = byLen + sprintf( (s8 *)tMtNotify.m_abyMtAlias, StrOfIP( tMtAlias.m_tTransportAddr.GetIpAddr() ) );		
	}
	if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH323ID, &tMtAlias ) )
	{
		byLen = byLen + sprintf( (s8 *)tMtNotify.m_abyMtAlias + byLen, "(%s)%c", tMtAlias.m_achAlias, 0 );
	}
	if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH320ID, &tMtAlias ) )
	{
		byLen = byLen + sprintf( (s8 *)tMtNotify.m_abyMtAlias + byLen, "(%s)%c", tMtAlias.m_achAlias, 0 );
	}
	if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH320Alias, &tMtAlias ) )
	{
		byLen = byLen + sprintf( (s8 *)tMtNotify.m_abyMtAlias + byLen, "(%s)%c", tMtAlias.m_achAlias, 0 );
	}
	tMtNotify.m_abyMtAlias[sizeof(tMtNotify.m_abyMtAlias)-1] = 0;
	SendTrapMsg( SVC_AGT_DEL_MT, (u8*)&tMtNotify, sizeof(tMtNotify) );
	
	// 针对vmp单通道轮询,终端掉线刷新轮询列表,并更新新的轮询终端
	ProcVmpPollingByRemoveMt( tMt );

	u16 wMcuIdx = INVALID_MCUIDX;
	// 挂断或删除下级MCU则把下级终端从轮询列表中删除, MCS自己维护列表，不用通知, zgc, 2007-03-29
	if( MT_TYPE_SMCU == tMt.GetMtType() )
	{
		ProcConfPollingByRemoveMt( tMt );
		/*wMcuIdx = GetMcuIdxFromMcuId( tMt.GetMtId() );
		if( POLL_STATE_NONE != m_tConf.m_tStatus.GetPollState() )
		{
		TMtPollParam atMtPollNew[MAXNUM_CONF_MT];
		TMtPollParam tTmpMt;
		u8 byPos;
		u8 byNewNum= 0;
		BOOL32 bPollingMtInSMcu = IsMtInMcu( tMt,(TMt)m_tConf.m_tStatus.GetMtPollParam() );// ( m_tConf.m_tStatus.GetMtPollParam().GetMcuIdx() == wMcuIdxtMt.GetMtId() ) ? TRUE : FALSE ;
		u8 byNextPos = bPollingMtInSMcu ? POLLING_POS_START : 0;     // 下一个要轮询的终端
		for( byPos = 0; byPos < m_tConfPollParam.GetPolledMtNum(); byPos++ )
		{
		tTmpMt = *(m_tConfPollParam.GetPollMtByIdx(byPos));
		if( !IsMtInMcu( tMt,(TMt)tTmpMt ) )//wMcuIdxtMt.GetMtId() != tTmpMt.GetMcuIdx() )
		{
		atMtPollNew[byNewNum] = tTmpMt;
		if( byPos > m_tConfPollParam.GetCurrentIdx() && byNextPos == POLLING_POS_START )
		{
		//记录下一个被轮询的终端在新列表中的位置, zgc, 2007-04-07
		byNextPos = byNewNum; 
		}
		byNewNum ++;
		}
		}
		m_tConfPollParam.SetPollList( byNewNum, atMtPollNew );
		// 如果离线的下级MCU中有当前轮询的终端，则立即切换
		if ( bPollingMtInSMcu )
		{
		// 指定新的轮询位置, zgc, 2007-04-07
		if( byNextPos == POLLING_POS_START )
		{
		byNextPos = m_tConfPollParam.GetPolledMtNum();
		}
		m_tConfPollParam.SpecPollPos( byNextPos );
		SetTimer( MCUVC_POLLING_CHANGE_TIMER, 10 );
		}
	}*/
		//lukunpeng 释放本级保存的此mcu下所有的回传终端及通道信息
		//		if( IsLocalAndSMcuSupMultSpy( wMcuIdx ) )
		//		{
		//			m_cSMcuSpyMana.FreeSpyChannlInfoByMcuId( GetMcuIdxFromMcuId( tMt.GetMtId() ),g_cMcuVcApp.GetCMultiSpyMgr() );
		//		}
	}
    
	if( MT_TYPE_MMCU == tMt.GetMtType() )
	{
		// [7/31/2010 xliang] 上传轮询停止
		u8 byPollMode = m_tConf.m_tStatus.GetPollMode();
		if (CONF_POLLMODE_VIDEO_SPY == byPollMode
			|| CONF_POLLMODE_SPEAKER_SPY == byPollMode)
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT, " tmt.%u is MMCU, so stop SpyPoll!\n", tMt.GetMtId());		
            ProcStopConfPoll();
		}
	}
	
	
	//zjj20110218挂断被点名人所在的mcu,点名业务要停止
	if ( MT_TYPE_SMCU == m_ptMtTable->GetMtType( tMt.GetMtId() ) &&
		ROLLCALL_MODE_NONE != m_tConf.m_tStatus.GetRollCallMode() 
		)
    {
        if ( !m_tRollCallee.IsLocal() && IsMtInMcu(tMt,m_tRollCallee) )
        {
            NotifyMcsAlarmInfo( 0, ERR_MCU_ROLLCALL_CALLEELEFT );
            RollCallStop(cServMsg);
        }
    }
	
	//zbq [12/05/2007] 清选看
	//zjj20100422 对于挂断上级mcu这里先不拆交换,在后面拆交换
	if( m_tCascadeMMCU.IsNull() || !( m_tCascadeMMCU == tMt ) )
	{
		StopSelectSrc(tMt, MODE_BOTH, FALSE);
		StopSelectSrc(tMt, MODE_VIDEO2SECOND, FALSE);
	}
	//fxh 清除选看该终端的状态
	// 	TMt tNullMt;
	// 	tNullMt.SetNull();
	// 	for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
	// 	{
	// 		TMtStatus tStat;
	// 		m_ptMtTable->GetMtStatus(byMtId, &tStat);
	// 		if (tStat.GetSelectMt(MODE_VIDEO) == tMt)
	// 		{
	// 			tStat.SetSelectMt(tNullMt, MODE_VIDEO);
	// 		}
	// 		if (tStat.GetSelectMt(MODE_AUDIO) == tMt)
	// 		{
	// 			tStat.SetSelectMt(tNullMt, MODE_AUDIO);
	// 		}
	// 		m_ptMtTable->SetMtStatus(byMtId, &tStat);
	// 	}
    
    //zbq [12/05/2007] 整理点名
	//     if ( ROLLCALL_MODE_NONE != m_tConf.m_tStatus.GetRollCallMode() )
	//     {
	//         if ( tMt == m_tRollCaller )
	//         {
	//             NotifyMcsAlarmInfo( 0, ERR_MCU_ROLLCALL_CALLERLEFT );
	//             RollCallStop(cServMsg);
	//         }
	//         else if ( tMt == m_tRollCallee )
	//         {
	//             NotifyMcsAlarmInfo( 0, ERR_MCU_ROLLCALL_CALLEELEFT );
	//             RollCallStop(cServMsg);
	//         }
	//     }
	
	NotifyMcuDropMt(tMt);
	
	
	//清空上级MCU锁定
	if( m_tConf.m_tStatus.GetProtectMode() == CONF_LOCKMODE_LOCK &&
		tMt.GetMtId() == m_tConfProtectInfo.GetLockedMcuId() &&
		!m_tCascadeMMCU.IsNull() && m_tCascadeMMCU == tMt )
	{
		m_tConf.m_tStatus.SetProtectMode(CONF_LOCKMODE_NONE);
        m_tConfProtectInfo.SetLockByMcu(0);
        m_tConfProtectInfo.SetLockByMcs(0);
	}
	
	//清空上级MCU
	if(!m_tCascadeMMCU.IsNull() && m_tCascadeMMCU == tMt)
	{	
		wMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
		if( IsLocalAndSMcuSupMultSpy( wMcuIdx )/*IsSupportMultiSpy()*/ )
		{
			StopAllLocalSpySwitch();
		}
		// 		else
		// 		{
		// 			StopSpyMtCascaseSwitch();
		// 			//zjl 20110510 StopSwitchToSubMt 接口重载替换
		// 			//StopSwitchToSubMt( tMt.GetMtId(), MODE_BOTH );
		// 			StopSwitchToSubMt(1, &tMt, MODE_BOTH);
		// 		}
		
		StopSpyMtCascaseSwitch();
        //释放对应高清适配资源
        //StopHDCascaseAdp();
		
		
		m_tCascadeMMCU.SetNull();
		m_tConfAllMtInfo.m_tMMCU.SetNull();
		m_tConfInStatus.SetNtfMtStatus2MMcu(FALSE);
	}
	
	
    // 通知终端离线原因
	// fxh 在RestoreAllSubMtJoinedConfWatchingSrcMt前设置正确的VCS会议状态,是否有当前调度终端
    MtOnlineChange( tMt, FALSE, byReason );	
	//对于挂断终端，如果当前终端开的接收通道是mpeg4 auto，判断是否需要改变MP4编码的参数
	BOOL32 bMepg4AutoMt = FALSE;
	TLogicalChannel tForwadChnnl;
	u16 wMtBitRate = 0;
	if (tMt.IsLocal() && m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId() ,LOGCHL_VIDEO,&tForwadChnnl,TRUE))
	{
		if (tForwadChnnl.GetChannelType() == MEDIA_TYPE_MP4 &&tForwadChnnl.GetVideoFormat() == VIDEO_FORMAT_AUTO ) 
		{
			bMepg4AutoMt = TRUE;
			wMtBitRate = m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE);
		}
	}

	//如果是卫星终端，在将终端置为不在线之前，让终端停止发送主流，以便计数正确。
	//如果该终端是双流源，会在在StopDoubleStream中让终端停止发送双流，在此就不必做了
	if (m_tConf.GetConfAttrb().IsSatDCastMode())
	{
		NotifyMtSend(tMt.GetMtId(),MODE_VIDEO,FALSE);
	}

	//移除

	m_tConfAllMtInfo.RemoveJoinedMt( tMt.GetMtId() );
	if ( bMepg4AutoMt && m_tConf.GetSecBitRate() != 0) 
	{
		u8 byOutChnl = GetVmpOutChnnlByRes(m_tVmpEqp.GetEqpId(), VIDEO_FORMAT_AUTO,MEDIA_TYPE_MP4);
		u16 wMinMp4GrpBirate = GetMinMtRcvBitByVmpChn(m_tVmpEqp.GetEqpId(), TRUE,byOutChnl);
		
		if ( wMtBitRate < wMinMp4GrpBirate )
		{
			//改变mp4 的那路编码参数
			TPeriEqpStatus tVmpStatus;
			g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpStatus);
			TVMPParam_25Mem tVmpParam = tVmpStatus.m_tStatus.tVmp.GetVmpParam();
			if (tVmpStatus.m_tStatus.tVmp.m_byUseState == TVmpStatus::WAIT_START || 
				tVmpStatus.m_tStatus.tVmp.m_byUseState == TVmpStatus::START) 
			{
				ChangeVmpParam(m_tVmpEqp.GetEqpId(), &tVmpParam);
			}
		}
	}

	RestoreAllSubMtJoinedConfWatchingSrcMt( tMt );
	RestoreAllSubMtJoinedConfWatchingSrcMt(tMt, MODE_VIDEO2SECOND);
	AdjustSwitchToMonitorWatchingSrc( tMt );			//停监控联动
	AdjustSwitchToAllMcWatchingSrcMt( tMt );
	StopSwitchToAllPeriEqpWatchingSrcMt( tMt );
	
	//[2011/11/14/zhangli]从RestoreAllSubMtJoinedConfWatchingSrcMt中移到此处
	if( !m_tCascadeMMCU.IsNull() && !(m_tCascadeMMCU==tMt) )
	{
		wMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
		if( IsLocalAndSMcuSupMultSpy( wMcuIdx ) )
		{		
			TSimCapSet tSrcCap;
			CSendSpy *ptSndSpy = NULL;
			TLogicalChannel tDstAudLgc,tSrcAudLgc;	
			m_ptMtTable->GetMtLogicChnnl(m_tCascadeMMCU.GetMtId(), MODE_AUDIO, &tDstAudLgc, TRUE);
			
			
			for (u8 bySpyIdx = 0; bySpyIdx < MAXNUM_CONF_SPY; bySpyIdx++)
			{
				ptSndSpy = m_cLocalSpyMana.GetSendSpy(bySpyIdx);
				if (NULL == ptSndSpy)
				{
					continue;
				}
				if (ptSndSpy->GetSpyMt().IsNull() || ptSndSpy->GetSimCapset().IsNull() || !(ptSndSpy->GetSpyMt() == tMt))
				{
					continue;
				}
				tSrcCap = m_ptMtTable->GetSrcSCS(GetLocalMtFromOtherMcuMt(ptSndSpy->GetSpyMt()).GetMtId());
				if(ptSndSpy->GetSimCapset() < tSrcCap)
				{
					StopSpyAdapt(ptSndSpy->GetSpyMt(), ptSndSpy->GetSimCapset(), MODE_VIDEO);
				}
				/*if( m_ptMtTable->GetMtLogicChnnl(GetLocalMtFromOtherMcuMt(ptSndSpy->GetSpyMt()).GetMtId(), MODE_AUDIO, &tSrcAudLgc, FALSE) )
				{
					if ( ptSndSpy->GetSimCapset().GetAudioMediaType() != MEDIA_TYPE_NULL &&
						tSrcCap.GetAudioMediaType() != MEDIA_TYPE_NULL && 
						( ptSndSpy->GetSimCapset().GetAudioMediaType() != tSrcCap.GetAudioMediaType() || 
						tSrcAudLgc.GetAudioTrackNum() != tDstAudLgc.GetAudioTrackNum() )
						)
					{
						StopSpyAdapt(ptSndSpy->GetSpyMt(), ptSndSpy->GetSimCapset(), MODE_AUDIO);
					}
				}*/

				if( IsNeedSpyAdpt(ptSndSpy->GetSpyMt(), ptSndSpy->GetSimCapset(), MODE_AUDIO) )
				{
					StopSpyAdapt(ptSndSpy->GetSpyMt(), ptSndSpy->GetSimCapset(), MODE_AUDIO);
				}
				
				StopSwitchToSubMt(1, &m_tCascadeMMCU, MODE_BOTH,TRUE,TRUE,TRUE,ptSndSpy->m_tSpyAddr.GetPort());

				m_cLocalSpyMana.FreeSpyChannlInfo( tMt,MODE_BOTH );

				ProcMMcuBandwidthNotify( m_tCascadeMMCU,GetRealSndSpyBandWidth() );
				break;
			}
		}
		
	}

	//zjj20100210 多回传补充 清除多回传交换
	/*u16 wSpyPort = SPY_CHANNL_NULL;	
	CSendSpy cSendSpy;
	wSpyPort = SPY_CHANNL_NULL;
	u8 byMediaMode = MODE_NONE;
	byMediaMode = m_cSMcuSpy.GetMode(tMt);
	if( MODE_NONE != byMediaMode &&
	!m_tCascadeMMCU.IsNull() && 
	m_cSMcuSpy.GetSpyChannlInfo(tMt, cSendSpy) )
	{
	wSpyPort = cSendSpy.m_wSpyStartPort;		
	StopSwitchToSubMt( m_tCascadeMMCU.GetMtId(), byMediaMode, SWITCH_MODE_BROADCAST, TRUE,TRUE,FALSE,wSpyPort );
	}	*/
	
	//wMcuIdx = GetMcuIdxFromMcuId( tMt.GetMtId() );
	RemoveMcu( tMt );
	
	
	//清除逻辑通道
    StopH460PinHoleNotifyByMtId( tMt.GetMtId() );
	m_ptMtTable->ClearMtLogicChnnl( tMt.GetMtId() );
    m_ptMtTable->SetMtType( tMt.GetMtId(), MT_TYPE_NONE );
    //清状态
    TMtStatus tMtStatus;
    if ( m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tMtStatus) )
    {
		BOOL32 byIsMtInMixing = m_ptMtTable->IsMtInMixing( tMt.GetMtId() );
		BOOL32 byIsMtInHdu = m_ptMtTable->IsMtInHdu( tMt.GetMtId() );		
		
        tMtStatus.Clear();
		tMtStatus.SetMtId(tMt.GetMtId());
		tMtStatus.SetMcuIdx(tMt.GetMcuId());
		tMtStatus.SetConfIdx(m_byConfIdx);
		
		tMtStatus.SetMtDisconnectReason( byReason );
		tMtStatus.SetInMixing(byIsMtInMixing);
		if (byIsMtInHdu && m_tConf.m_tStatus.m_tConfMode.GetHduInBatchPoll() != POLL_STATE_NONE)
		{
			tMtStatus.SetInHdu(TRUE);
		}
        m_ptMtTable->SetMtStatus(tMt.GetMtId(), &tMtStatus);
		MtStatusChange(&tMt, TRUE);
    }
	
    //通知
	if( bMsg )
	{
        cServMsg.SetMsgBody();
		SendMsgToMt( tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg );
	}
	
	//是数据终端
	if ( m_ptMtTable->IsMtInDataConf( tMt.GetMtId() ) )
	{
		//通知DCS删除
		SendMcuDcsDelMtReq( tMt.GetMtId() );
	}    
	
	//通知所有会控
	cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
	if(m_tConf.HasConfExInfo())
	{
		u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
		u16 wPackDataLen = 0;
		PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
		cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);
	}
	SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
	
	//发给会控会议所有终端信息
	//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
	//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
	
	//通知所有终端
	cServMsg.SetMsgBody( ( u8 * )&tMt, sizeof( tMt ) );	
	BroadcastToAllSubMtJoinedConf( MCU_MT_MTLEFT_NOTIF, cServMsg );
	
    // xsl [10/11/2006] 释放端口
	g_cMcuVcApp.ReleaseMtPort( m_byConfIdx, tMt.GetMtId() );
    m_ptMtTable->ClearMtSwitchAddr( tMt.GetMtId() );
	
    // zbq [02/18/2008] 降速终端离会，恢复VMP广播的码率
	// zjj20101115 如果非VMP广播就恢复广播源码率,由于时序问题在发送流控消息前先清除终端E1状态
    if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
		m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()) )
    {
		if(g_cMcuVcApp.IsBrdstVMP(m_tVmpEqp) )
		{
			AdjustVmpBrdBitRate();
		}
		else 
		{
			TMt tSrc = GetLocalVidBrdSrc();
			if( !tSrc.IsNull() )
			{				
				m_ptMtTable->SetMtTransE1(tMt.GetMtId(), FALSE);
				m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), FALSE);
				NotifyMtSend( tSrc.GetMtId(), MODE_VIDEO );
			}			
		}
    }
	
    //20110527,songkun,VMP广播就恢复广播原码率,VMP降速，低速终端离开升速
    //恢复源码率（源可能为bas、vmp、mt）
    if (m_tConf.GetSecBitRate() == 0)
	{
		//防止取最小码率时取到
		m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), m_tConf.GetBitRate(), LOGCHL_VIDEO);
	}	
	RestoreVidSrcBitrate(tMt, MODE_VIDEO);
	if (!m_tDoubleStreamSrc.IsNull() && !(tMt == m_tDoubleStreamSrc))
	{
		//防止取最小码率时取到
		m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), m_tConf.GetBitRate(), LOGCHL_SECVIDEO);
		RestoreVidSrcBitrate(tMt, MODE_SECVIDEO);
	} 
	
	
    //恢复E1状态
    m_ptMtTable->SetMtTransE1(tMt.GetMtId(), FALSE);
    m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), FALSE);
	
    //n+1备份更新终端信息
    if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
    {
        ProcNPlusConfMtInfoUpdate();
    }
	
    //清出群组
    TCapSupport tCap;
    if (m_ptMtTable->GetMtCapSupport(tMt.GetMtId(), &tCap))
    {
        TSimCapSet tMainCap = tCap.GetMainSimCapSet();
        TSimCapSet tSecCap = tCap.GetSecondSimCapSet();
        TDStreamCap tDCap = tCap.GetDStreamCapSet();
		//        m_cMtRcvGrp.RemoveMem(tMt.GetMtId(), tMainCap);
		//        m_cMtRcvGrp.RemoveMem(tMt.GetMtId(), tSecCap);
		//        m_cMtRcvGrp.RemoveMem(tMt.GetMtId(), tDCap);
    }
	
	if (m_tH239TokenOwnerInfo.m_tH239TokenMt == tMt)
	{
		ClearH239TokenOwnerInfo(NULL);
	}
	
	//zbq [05/13/2010] 清出等待申请队列
	BOOL32 bIsSendToChairman = FALSE;
    if (m_tApplySpeakQue.IsMtInQueue(tMt) )
    {
        m_tApplySpeakQue.ProcQueueInfo(tMt, bIsSendToChairman, FALSE);
        NotifyMcsApplyList( bIsSendToChairman );
    }
	
	if( VCS_CONF == m_tConf.GetConfSource() && MTLEFT_REASON_NORMAL == byReason &&
		!(tMt == m_tConf.GetChairman()) &&
		!(tMt == m_cVCSConfStatus.GetVcsBackupChairMan()) 
		)
	{		
		m_ptMtTable->SetCallMode( tMt.GetMtId(),CONF_CALLMODE_NONE );
	}

	if( m_byOtherConfMtId == tMt.GetMtId() )
	{
		SetInOtherConf( FALSE );
	}
	//打印消息
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mt%d: 0x%x(Dri:%d) droped from conf for the reason.%d!\n", 
		tMt.GetMtId(), m_ptMtTable->GetIPAddr(tMt.GetMtId()), tMt.GetDriId(), byReason );
	
	return;
}

/*====================================================================
    函数名      ：ProcMtMcuMtStatusNotif
    功能        ：终端状态上报处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/05    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::ProcMtMcuMtStatusNotif(const CMessage * pcMsg)
{
    CServMsg   cServMsg( pcMsg->content, pcMsg->length);
    TMtStatus  tMtOldStatus;
    TMtStatus *ptMtStatus = (TMtStatus *)cServMsg.GetMsgBody();
    BOOL32     bStatusChange = FALSE;

    switch (CurState())
    {
    case STATE_ONGOING:
		{
        //得到终端状态
        m_ptMtTable->GetMtStatus( cServMsg.GetSrcMtId(), &tMtOldStatus );

        if (ptMtStatus->IsEnableFECC() != tMtOldStatus.IsEnableFECC() || 
            ptMtStatus->GetCurVideo() != tMtOldStatus.GetCurVideo()   || 
            ptMtStatus->GetCurAudio() != tMtOldStatus.GetCurAudio()   || 
            ptMtStatus->GetMtBoardType() != tMtOldStatus.GetMtBoardType() ||
            //zbq[08/02/2007] 本来不需要修正，因为VideoLose在本版本未启用；但原则上此处应该支持到。
            ptMtStatus->IsVideoLose() != tMtOldStatus.IsVideoLose() )
        {
            bStatusChange = TRUE;
        }

        //添加终端未上报的
        ptMtStatus->SetTMt(m_ptMtTable->GetMt(cServMsg.GetSrcMtId()));
        //ptMtStatus->SetVideoFreeze(tMtOldStatus.IsVideoFreeze());
        ptMtStatus->SetSelByMcsDrag(tMtOldStatus.GetSelByMcsDragMode());
        ptMtStatus->m_tVideoMt  = tMtOldStatus.m_tVideoMt;
        ptMtStatus->m_tAudioMt  = tMtOldStatus.m_tAudioMt;
        ptMtStatus->m_tRecState = tMtOldStatus.m_tRecState;
        ptMtStatus->SetSendVideo(tMtOldStatus.IsSendVideo());

        // 增加双流状态标识 [02/05/2007-zbq]
		BOOL32 bSndVid2 = (m_tDoubleStreamSrc.GetType() == ptMtStatus->GetType() && m_tDoubleStreamSrc.GetMtId() == ptMtStatus->GetMtId()) ? TRUE : FALSE;
        ptMtStatus->SetSndVideo2( bSndVid2 );

        ptMtStatus->SetSendAudio(tMtOldStatus.IsSendAudio());
        ptMtStatus->SetInMixing(tMtOldStatus.IsInMixing());
        ptMtStatus->SetIsInMixGrp(tMtOldStatus.IsInMixGrp());
        ptMtStatus->SetReceiveVideo(tMtOldStatus.IsReceiveVideo());
        ptMtStatus->SetReceiveAudio(tMtOldStatus.IsReceiveAudio());        
        ptMtStatus->SetInTvWall(tMtOldStatus.IsInTvWall());
		ptMtStatus->SetInHdu(tMtOldStatus.IsInHdu());

        // 这里要记录原先选看
        ptMtStatus->SetSelectMt( tMtOldStatus.GetSelectMt(MODE_AUDIO), MODE_AUDIO );
        ptMtStatus->SetSelectMt( tMtOldStatus.GetSelectMt(MODE_VIDEO), MODE_VIDEO );
        
        //设置终端状态
        m_ptMtTable->SetMtStatus(cServMsg.GetSrcMtId(), ptMtStatus);
        
        if (!m_tCascadeMMCU.IsNull() && bStatusChange)
        {
            OnNtfMtStatusToMMcu(m_tCascadeMMCU.GetMtId(), ptMtStatus->GetMtId() );
        }

        //通知会控
		// [pengjie 2011/1/11] 这里应该只上报该终端的就可以了
		//MtStatusChange();
		TMt tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
        MtStatusChange( &tMt );

        break;
	}
    default:
        ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
                pcMsg->event, ::OspEventDesc(pcMsg->event), CurState());
        break;
	}
}

/*====================================================================
    函数名      ：ProcMtMcuInviteMtAck
    功能        ：终端接受邀请处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/05    1.0         LI Yi         创建
	06/01/06	4.0			张宝卿		  标记终端呼叫方向
====================================================================*/
void CMcuVcInst::ProcMtMcuInviteMtAck( const CMessage * pcMsg )
{
	STATECHECK
		
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	TMtAlias tMtH323Alias = *(TMtAlias*)cServMsg.GetMsgBody();
	TMtAlias tMtAddr = *((TMtAlias*)cServMsg.GetMsgBody()+1);
	u8 byType = *(u8*)( cServMsg.GetMsgBody() + sizeof(TMtAlias)*2 );
    u8 byEncrypt = *(u8*)( cServMsg.GetMsgBody() + sizeof(TMtAlias)*2 +1);

	if(byEncrypt == 0 && m_tConf.GetConfAttrb().GetEncryptMode() != CONF_ENCRYPTMODE_NONE)
	{
		//drop call
		cServMsg.SetMsgBody();
		SendMsgToMt( tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg );
		
		cServMsg.SetMsgBody( (u8*)&tMt, sizeof(tMt) );
		cServMsg.SetErrorCode(ERR_MCU_CONF_REJECT_NOENCYPT_MT);
		cServMsg.SetConfId( m_tConf.GetConfId() );
		SendMsgToAllMcs( MCU_MCS_CALLMTFAILURE_NOTIF, cServMsg );
		
		return;
	}

	//设置终端类型
	if( byType == TYPE_MCU )
	{
		if( m_tConf.GetConfAttrb().IsSupportCascade() )
		{
			m_ptMtTable->SetMtType( tMt.GetMtId(), MT_TYPE_SMCU );
		}
		else
		{
			m_ptMtTable->SetMtType( tMt.GetMtId(), MT_TYPE_MT);
			SetInOtherConf( TRUE,tMt.GetMtId() );
		}		
	}
	else
	{
		// 追加新录播实体类型支持,判Mtid对应的录放像模式，为录像或放像则定为vrs新录播实体
		if (m_ptMtTable->GetRecChlType(tMt.GetMtId()) == TRecChnnlStatus::TYPE_RECORD
			|| m_ptMtTable->GetRecChlType(tMt.GetMtId()) == TRecChnnlStatus::TYPE_PLAY)
		{
			m_ptMtTable->SetMtType( tMt.GetMtId(), MT_TYPE_VRSREC);
		}
		else
		{
			m_ptMtTable->SetMtType( tMt.GetMtId(), MT_TYPE_MT); 
		}
	}

	//设置终端别名
    TMtAlias tDialAlias;
    m_ptMtTable->GetDialAlias( tMt.GetMtId(), &tDialAlias );
    
    if(!tDialAlias.IsNull() && tDialAlias.m_AliasType ==  puAliasTypeIPPlusAlias)
    {
		//避免同IP网关上报的323别名都一致，应保存呼叫时的别名[1/10/2013 chendaiwei]
        tDialAlias.m_AliasType = mtAliasTypeH323ID;
        m_ptMtTable->SetMtAlias( tMt.GetMtId(), &tDialAlias);
        m_ptMtTable->SetMtAlias( tMt.GetMtId(), &tMtAddr );
    }
    else
    {
		m_ptMtTable->SetMtAlias( tMt.GetMtId(), &tMtAddr );
		m_ptMtTable->SetMtAlias( tMt.GetMtId(), &tMtH323Alias);
	}

	//设置终端IP地址
	if( tMtAddr.m_AliasType == mtAliasTypeTransportAddress )
	{
		m_ptMtTable->SetIPAddr( tMt.GetMtId(), tMtAddr.m_tTransportAddr.GetIpAddr() );
	}

	//标记终端为被邀加入
	//m_ptMtTable->SetMtCallingIn( tMt.GetMtId(), FALSE );

	//终端上线时统一上报[9/13/2012 chendaiwei]
	//SendMtAliasToMcs( tMt );
}

/*=============================================================================
  函 数 名： SendMtAliasToMcs
  功    能： 发送终端别名给mcs
  算法实现： 
  全局变量： 
  参    数： TMt tMt
  返 回 值： void 
=============================================================================*/
void CMcuVcInst::SendMtAliasToMcs(TMt tMt)
{
    CServMsg cServMsg;
    TMtAlias tMtH323IDAlias;
    TMtAlias tMtH320IDAlias;
    TMtAlias tMtH320Alias;
    TMtAlias tMtE164Alias;

    if( m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeH320ID, &tMtH320IDAlias) )
    {
        cServMsg.SetMsgBody((u8*)&tMt, sizeof(TMt));
        cServMsg.CatMsgBody((u8*)&tMtH320IDAlias, sizeof(tMtH320IDAlias));
        
        m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeH320Alias, &tMtH320Alias);
        cServMsg.CatMsgBody((u8*)&tMtH320Alias, sizeof(tMtH320Alias));
    }
    else
    {
        m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeH323ID, &tMtH323IDAlias);
        m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeE164, &tMtE164Alias);
        cServMsg.SetMsgBody((u8*)&tMt, sizeof(TMt));
        cServMsg.CatMsgBody((u8*)&tMtH323IDAlias, sizeof(tMtH323IDAlias));
        cServMsg.CatMsgBody((u8*)&tMtE164Alias, sizeof(tMtE164Alias));
    }
    SendMsgToAllMcs(MCU_MCS_MTALIAS_NOTIF, cServMsg);

    return;
}

/*====================================================================
    函数名      ：ProcMtMcuInviteMtNack
    功能        ：终端拒绝邀请处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/05    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::ProcMtMcuInviteMtNack( const CMessage * pcMsg )
{
    STATECHECK;

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );    
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
    u8 byReason = 0;
    u16 wErrorCode = cServMsg.GetErrorCode();
    if ( cServMsg.GetMsgBodyLen() > 0 )
    {
        byReason = *(cServMsg.GetMsgBody()+sizeof(TMtAlias));
    }

    ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "Mt%d Invite Nack-%s.%d,Err%u\n", tMt.GetMtId(), 
             GetMtLeftReasonStr(byReason), byReason, wErrorCode );
    
	// 如果是新录播实体，清除新录播相关信息
	TRecChnnlStatus tRecStatus = m_ptMtTable->GetRecChnnlStatus(tMt.GetMtId());
	if (tRecStatus.m_byType == TRecChnnlStatus::TYPE_RECORD 
		|| tRecStatus.m_byType == TRecChnnlStatus::TYPE_PLAY)
	{
		ReleaseVrsMt(tMt.GetMtId(), wErrorCode);
		return;
	}

    CallFailureNotify(cServMsg);



// [pengjie 2010/6/3] 给界面上报拒绝入会原因
// [zhushz 2010/11/20] 拒绝入会原因已经上报
/*
	if( ERR_MCU_CALLMCUERROR_CONFISHOLDING == wErrorCode ||
		ERR_MCU_CASADEBYOTHERHIGHLEVELMCU == wErrorCode )
	{
		//NotifyMcsAlarmInfo( 0, wErrorCode );		
		cServMsg.SetEventId(MCU_MCS_ALARMINFO_NOTIF);
		cServMsg.SetErrorCode(wErrorCode);
		cServMsg.SetConfIdx( 0 );
		SendMsgToAllMcs(MCU_MCS_ALARMINFO_NOTIF, cServMsg);
	}
*/
// End
}


/*====================================================================
    函数名      ：ProcMtMcuGetMtAliasReq
    功能        ：终端查询其他终端别名
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/21    3.0         JQL           创建
	04/03/11    3.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::ProcMtMcuGetMtAliasReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt = *( TMt* )cServMsg.GetMsgBody();
	TMtAlias    tMtAlias;

	switch( CurState() )
	{
	case STATE_SCHEDULED:
	case STATE_ONGOING:

		if( MCS_MCU_GETMTALIAS_REQ == cServMsg.GetEventId() && !tMt.IsLocal() )
		{
			cServMsg.SetErrorCode( ERR_MCU_OPRONLYLOCALMT );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	
			return;
		}

		cServMsg.SetMsgBody( ( u8* )&tMt, sizeof( tMt ) );
		
		//根据终端表，查询终端别名
		if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH320ID, &tMtAlias ) )
		{			
			cServMsg.CatMsgBody( ( u8* )&tMtAlias, sizeof( tMtAlias ) );
            
            if(m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeH320Alias, &tMtAlias))
            {
                cServMsg.CatMsgBody( ( u8* )&tMtAlias, sizeof( tMtAlias ) );
            }
		}
		else
        {
			if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH323ID, &tMtAlias ) )
			{
				cServMsg.CatMsgBody( ( u8* )&tMtAlias, sizeof( tMtAlias ) );
			}
			if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeE164, &tMtAlias ) )
			{
				cServMsg.CatMsgBody( ( u8* )&tMtAlias, sizeof( tMtAlias ) );
			}
            if( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeTransportAddress, &tMtAlias ) )
		    {			
			    cServMsg.CatMsgBody( ( u8* )&tMtAlias, sizeof( tMtAlias ) );
		    }
		}

		if( cServMsg.GetMsgBodyLen() > sizeof(TMt) )
		{
			SendReplyBack( cServMsg, pcMsg->event + 1 );
		}
		else
		{
			SendReplyBack( cServMsg, pcMsg->event + 2 );
		}

		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}	
}

/*====================================================================
    函数名      ：ProcMcsMcuGetAllMtAliasReq
    功能        ：查询所有终端别名
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/21    3.0         JQL           创建
	04/04/27    3.0         胡昌威        修改
	05/01/29	3.6			Jason         修改
    06/12/14	4.0			顾振华        新的打包格式，发送所有别名
====================================================================*/
void CMcuVcInst::ProcMcsMcuGetAllMtAliasReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
    u8          abyBuf[SERV_MSG_LEN-SERV_MSGHEAD_LEN] = {0};
    u8*         ptrBuf = abyBuf;
    u16         wBufLen = 0;

	TMcu        tMcu;

    u8 byMtId = 0;
    u8 byAliasNum = 0;
    u8 byValidMtNum = 0;

	u16 wMcuIdx = INVALID_MCUIDX;

	switch( CurState() )
	{
	case STATE_ONGOING:

	    SendReplyBack( cServMsg, pcMsg->event + 1 );

		//得到MCU
		tMcu = *(TMcu*)cServMsg.GetMsgBody();         

		//所有终端状态
		if( ( INVALID_MCUIDX == tMcu.GetMcuIdx() ) &&
			 m_ptConfOtherMcTable != NULL)
		{
            // 非本级只有一个别名
            byAliasNum = 1;
            u8 byAliasType = mtAliasTypeH323ID;

			//先发其它MC
			SendMtListToMcs(INVALID_MCUIDX);
/*			TConfMcInfo *ptConfMcInfo = NULL;
			for( u16 wLoop = 0; wLoop < TConfOtherMcTable::GetMaxMcuNum(); wLoop++ )
			{                
				ptConfMcInfo = (m_ptConfOtherMcTable->GetMcInfo(wLoop));
				if(	NULL == ptConfMcInfo
					|| !IsValidSubMcuId(ptConfMcInfo->GetMcuIdx()))
				{
					continue;
				}

                // McuID
                ptrBuf = abyBuf ;
				wMcuIdx = ptConfMcInfo->GetMcuIdx();
				wMcuIdx = htons( wMcuIdx );
                memcpy(ptrBuf, &wMcuIdx, sizeof(u16));

                // 跳过MtNum
                ptrBuf += 3;
                wBufLen = 3;

                byValidMtNum = 0;
                
				for( u8 byLoop1 = 0; byLoop1 < MAXNUM_CONF_MT; byLoop1++ )
				{
					TMtExt *ptMtExt = &(ptConfMcInfo->m_atMtExt[byLoop1]);
					if ( ptMtExt->IsNull() ||
					  	 ptMtExt->GetMtId() == 0) //过滤自己
					{
						continue;
					}
                    byMtId = ptMtExt->GetMtId();
                    byValidMtNum ++;
										
                    // MtID
                    memcpy(ptrBuf, &byMtId, sizeof(u8));
                    ptrBuf++;                    
                    // Alias Num = 1
                    memcpy(ptrBuf, &byAliasNum, sizeof(u8));
                    ptrBuf++;       
                    // Alias Type = H323ID
                    memcpy(ptrBuf, &byAliasType, sizeof(u8));
                    ptrBuf++;
                    wBufLen += 3;

                    // Alias Len
					u8 byAliasLen = strlen(ptMtExt->GetAlias());
                    memcpy(ptrBuf, &byAliasLen, sizeof(u8));
                    ptrBuf++;
                    wBufLen++;
					if (byAliasLen>0)
                    {
                        // Alias
                        memcpy(ptrBuf, (u8 *)ptMtExt->GetAlias(), byAliasLen);
                        ptrBuf += byAliasLen;
                        wBufLen += byAliasLen;
                    }
				}
                abyBuf[2] = byValidMtNum;
                cServMsg.SetMsgBody( abyBuf, wBufLen );
				SendReplyBack( cServMsg, MCU_MCS_ALLMTALIAS_NOTIF );
			}
*/
			//本级的最后发
			tMcu.SetNull();
			tMcu.SetMcu( LOCAL_MCUID );

			tMcu.SetMcuIdx( LOCAL_MCUIDX );
		}

		//不是本级的MCU
		if( ! tMcu.IsLocal() && m_ptConfOtherMcTable != NULL)
		{
			TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMcu.GetMcuId());
			if( ptConfMcInfo == NULL )
			{
				return;
			}
			if( ptConfMcInfo->IsNull() )
			{
				return;
			}

            byAliasNum = 1;
            u8 byAliasType = mtAliasTypeH323ID;

            // McuID
            ptrBuf = abyBuf ;
			wMcuIdx = ptConfMcInfo->GetMcuIdx();
			wMcuIdx = htons( wMcuIdx );
            memcpy(ptrBuf, &wMcuIdx, sizeof(u16));

            // 跳过MtNum
            ptrBuf += 3;
            wBufLen = 3;

            byValidMtNum = 0;
            
			for( u8 byLoop1 = 0; byLoop1 < MAXNUM_CONF_MT; byLoop1++ )
			{
				TMtExt *ptMtExt = &(ptConfMcInfo->m_atMtExt[byLoop1]);
				if ( ptMtExt->IsNull() ||
					 ptMtExt->GetMtId() == 0) //过滤自己
				{
					continue;
				}
                byMtId = ptMtExt->GetMtId();
                byValidMtNum ++;
									
                // MtID
                memcpy(ptrBuf, &byMtId, sizeof(u8));
                ptrBuf++;                    
                // Alias Num = 1
                memcpy(ptrBuf, &byAliasNum, sizeof(u8));
                ptrBuf++;       
                // Alias Type = H323ID
                memcpy(ptrBuf, &byAliasType, sizeof(u8));
                ptrBuf++;
                wBufLen += 3;

                // Alias Len
				u8 byAliasLen = strlen(ptMtExt->GetAlias());
                memcpy(ptrBuf, &byAliasLen, sizeof(u8));
                ptrBuf++;
                wBufLen++;

				if (byAliasLen>0)
                {
                    // Alias
                    memcpy(ptrBuf, (u8 *)ptMtExt->GetAlias(), byAliasLen);
                    ptrBuf += byAliasLen;
                    wBufLen += byAliasLen;
                }
			}

            abyBuf[1] = byValidMtNum;
            cServMsg.SetMsgBody( abyBuf, wBufLen );
            SendReplyBack( cServMsg, MCU_MCS_ALLMTALIAS_NOTIF );
		}
		else
		{
            // McuID
            ptrBuf = abyBuf ;
            //abyBuf[0] = LOCAL_MCUID;


			wMcuIdx = LOCAL_MCUIDX;
			wMcuIdx = htons( wMcuIdx );
			memcpy(ptrBuf, &wMcuIdx, sizeof(u16));

            // 跳过MtNum
            ptrBuf += 3;
            wBufLen = 3;

            byValidMtNum = 0;
            
            u8* ptrBufTmp = NULL;
            TMtAlias tMtAlias;
            byAliasNum = 0;
			for( u8 byLoopMtId = 1; byLoopMtId <= MAXNUM_CONF_MT; byLoopMtId++ )
			{
				if( m_tConfAllMtInfo.MtInConf( byLoopMtId ) )
				{
					byValidMtNum ++;
                    
                    // MtID
                    memcpy(ptrBuf, &byLoopMtId, sizeof(u8));
                    ptrBuf++;  
                    wBufLen++;
                    
                    // 跳过Alias Num
                    ptrBufTmp = ptrBuf;
                    ptrBuf++;  
                    wBufLen++;
                    byAliasNum = 0;

                    for (u8 byLoopAliasType = mtAliasTypeE164; byLoopAliasType < mtAliasTypeOthers; byLoopAliasType ++)
                    {
                        tMtAlias.SetNull();
                        if (m_ptMtTable->GetMtAlias(byLoopMtId, (mtAliasType)byLoopAliasType, &tMtAlias))
                        {
                            byAliasNum ++;

                            // Alias Type
                            memcpy(ptrBuf, &byLoopAliasType, sizeof(u8));
                            ptrBuf++;
                            wBufLen++;     
                            
                            // Alias Len
                            u8 byAliasLen = strlen(tMtAlias.m_achAlias);
                            memcpy(ptrBuf, &byAliasLen, sizeof(u8));
                            ptrBuf++;
                            wBufLen++;

				            if (byAliasLen>0)
                            {
                                // Alias
                                memcpy(ptrBuf, (u8 *)tMtAlias.m_achAlias, byAliasLen);
                                ptrBuf += byAliasLen;
                                wBufLen += byAliasLen;
                            }
                        }
                    }

                    // 回填Alias Num
                    memcpy(ptrBufTmp, &byAliasNum, sizeof(u8));
                }
			}

            // 回填MtNum
            abyBuf[2] = byValidMtNum;
            cServMsg.SetMsgBody( abyBuf, wBufLen );
            SendReplyBack( cServMsg, MCU_MCS_ALLMTALIAS_NOTIF );
		}

		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}	
}

/*=============================================================================
  函 数 名： ProcMcsMcuGetMtBitrateReq
  功    能： 会控查询终端码率
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMcuVcInst::ProcMcsMcuGetMtBitrateReq( const CMessage *pcMsg )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );
    if(cServMsg.GetMsgBodyLen() != sizeof(TMt))
    {
        cServMsg.SetEventId(pcMsg->event+2);
        SendReplyBack(cServMsg, pcMsg->event+2);
        return;
    }

    TMtStatus tMtStatus;
    TMt tMt = *(TMt *)cServMsg.GetMsgBody();

    switch( CurState() )
    {
    case STATE_ONGOING:
    case STATE_SCHEDULED:
        if(tMt.IsNull())//所有终端
        {
            cServMsg.SetMsgBody(NULL, 0);
            for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
            {
                if( m_tConfAllMtInfo.MtInConf( byLoop ) ) 
                {
                    //请求终端码率信息
                    SendMsgToMt(byLoop, MCU_MT_GETBITRATEINFO_REQ, cServMsg);
                }
            }
        }
        else//特定终端
        {
            if( m_tConfAllMtInfo.MtInConf( tMt.GetMtId() )) 
            {
                cServMsg.SetMsgBody( (u8*)((TMtBitrate *)&tMtStatus), sizeof(TMtBitrate) );
                
                //请求终端码率信息
                SendMsgToMt(tMt.GetMtId(), MCU_MT_GETBITRATEINFO_REQ, cServMsg);
            }
        }

        SendReplyBack(cServMsg, pcMsg->event+1); 
        break;

    default:
        ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
            pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
        break;
    }

    return;   
}

/*====================================================================
    函数名      ：ProcMcsMcuGetMtExtInfoReq
    功能        ：终端查询终端的扩展信息
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/12/18    4.5         张宝卿           创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuGetMtExtInfoReq( const CMessage *pcMsg )
{
    STATECHECK;

    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TMt tMt = *(TMt*)cServMsg.GetMsgBody();
    if ( !tMt.IsNull() )
    {
        if ( m_tConfAllMtInfo.MtInConf(tMt.GetMtId()))
        {
            if ( 0 == m_ptMtTable->GetHWVerID(tMt.GetMtId()) ||
                 NULL == m_ptMtTable->GetSWVerID(tMt.GetMtId()))
            {
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[GetMtExtInfo] Mt.%d verInfo unexist, nack\n", tMt.GetMtId() );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            }
            else
            {
                TMtExt2 tMtExt2;
                tMtExt2.SetMt(tMt);
                tMtExt2.SetHWVerId(m_ptMtTable->GetHWVerID(tMt.GetMtId()));
                tMtExt2.SetSWVerId(m_ptMtTable->GetSWVerID(tMt.GetMtId()));
                cServMsg.SetMsgBody((u8*)&tMtExt2, sizeof(tMtExt2));
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
            }            
        }
    }
    else
    {
        for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
        {
            if ( m_tConfAllMtInfo.MtInConf(byMtId))
            {
                tMt = m_ptMtTable->GetMt(byMtId);
            
                if ( 0 == m_ptMtTable->GetHWVerID(tMt.GetMtId()) ||
                     NULL == m_ptMtTable->GetSWVerID(tMt.GetMtId()))
                {
                    ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "[GetMtExtInfo] Mt.%d verInfo unexist, nack\n", tMt.GetMtId() );
                    continue;
                }
                TMtExt2 tMtExt2;
                tMtExt2.SetMt(tMt);
                tMtExt2.SetHWVerId(m_ptMtTable->GetHWVerID(byMtId));
                tMtExt2.SetSWVerId(m_ptMtTable->GetSWVerID(byMtId));
            
                if ( 0 != cServMsg.GetMsgBodyLen() )
                {
                    cServMsg.CatMsgBody((u8*)&tMtExt2, sizeof(tMtExt2));
                }
                else
                {
                    cServMsg.SetMsgBody((u8*)&tMtExt2, sizeof(tMtExt2));
                }
            }
        }
        if ( cServMsg.GetMsgBodyLen() > 0 )
        {
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
        }
    }

    return;
}

/*====================================================================
    函数名      ：ProcMtMcuJoinedMtListReq
    功能        ：终端查询所有与会终端号
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/21    3.0         JQL           创建
====================================================================*/
void CMcuVcInst::ProcMtMcuJoinedMtListReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt;
	u8			byMtId;

	switch( CurState() )
	{
	case STATE_ONGOING:
		if( !m_tConfAllMtInfo.MtJoinedConf( cServMsg.GetSrcMtId() ) )	//not in conference
		{
			SendReplyBack( cServMsg, pcMsg->event + 2 );
			break;
		}

		cServMsg.SetMsgBody( NULL, 0 );
		//将会议成员信息发送给终端适配会话
		for( byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++ )
		{
			if( m_tConfAllMtInfo.MtJoinedConf( byMtId ) )
			{
				tMt.SetMt( LOCAL_MCUID, byMtId );
				cServMsg.CatMsgBody( (u8*)&tMt, sizeof( tMt ) );
			}

		}
		
		SendReplyBack( cServMsg, pcMsg->event + 1 );
		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}	

}

/*====================================================================
    函数名      ：ProcMtMcuJoinedMtListIdReq
    功能        ：终端查询所有与会终端号
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/21    3.0         JQL           创建
====================================================================*/
void CMcuVcInst::ProcMtMcuJoinedMtListIdReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			atMt[MAXNUM_CONF_MT];
	TMtAlias	atMtAlias[MAXNUM_CONF_MT];
	u8			abyBuff[MAXNUM_CONF_MT*( sizeof(TMt) + sizeof(TMtAlias) )];
	u8			byMtNum = 0;
	u8			byMtId;
	u32			dwBuffSize = 0;

	switch( CurState() )
	{
	case STATE_ONGOING:

		memset( atMt, 0, sizeof(atMt) );
		memset( atMtAlias, 0, sizeof(atMtAlias) );
		memset( abyBuff, 0, sizeof(abyBuff) );
		for( byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++ )
		{
			if( m_tConfAllMtInfo.MtJoinedConf( byMtId ) )
			{
				atMt[byMtNum] = m_ptMtTable->GetMt( byMtId );
				atMtAlias[byMtNum].SetNull();
                if(!m_ptMtTable->GetMtAlias(byMtId, mtAliasTypeH320Alias, &atMtAlias[byMtNum]))
                {
                    if(!m_ptMtTable->GetMtAlias( byMtId, mtAliasTypeH320ID, &atMtAlias[byMtNum] ))
                    {
                        if(!m_ptMtTable->GetMtAlias( byMtId, mtAliasTypeH323ID, &atMtAlias[byMtNum] ))
                        {
                            if(!m_ptMtTable->GetMtAlias( byMtId, mtAliasTypeE164, &atMtAlias[byMtNum] ))
                            {					
                                m_ptMtTable->GetMtAlias( byMtId, mtAliasTypeTransportAddress, &atMtAlias[byMtNum] );
                            }
                        }
                    }
                }				
				byMtNum++;
			}
		}
		
		cServMsg.SetMsgBody( NULL, 0 );

		dwBuffSize = PackMtInfoListIntoBuf( atMt, atMtAlias, byMtNum, abyBuff, sizeof( abyBuff ) );
		//Attation please, the buffer size may exceed ONE CServMsg that can hold//
		cServMsg.SetMsgBody( abyBuff, (u16)dwBuffSize );
		SendReplyBack( cServMsg, pcMsg->event + 1 );
		
		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}	

}

/*====================================================================
    函数名      ：PackMtInfoListIntoBuf
    功能        ：终端别名列表打包
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt atMt[] 终端数组
	              const TMtAlias atMtAlias[] 终端别名数组 
				  u8 byArraySize  数组大小
				  u8 *pbyBuf 包缓存
				  u32 dwBufLen 缓存长度
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/21    3.0         JQL           创建
====================================================================*/
u32 CMcuVcInst::PackMtInfoListIntoBuf( const TMt atMt[], const TMtAlias atMtAlias[], u8 byArraySize, u8 *pbyBuf, u32 dwBufLen )
{

#define  TRY_MEM_COPY( dst, src, len, limit, result ) \
	do\
	{\
		if((u32)(dst) + (len) > (u32)(limit)) \
		{\
			OspLog(LOGLVL_DEBUG1, "insufficient memory to store the data.\n"); \
			result = FALSE; \
		} \
		memcpy((dst), (src), (len) ); \
		result = TRUE;\
	}\
	while (0)	
	
	
	u8 *pbyTmp = pbyBuf;
	u8 *pbyMemLimit = pbyBuf + dwBufLen;
	BOOL32 bResult;

	for(int i = 0; i < byArraySize; i++)
	{
		TRY_MEM_COPY(pbyTmp, (void*)&atMt[i], sizeof(TMt), pbyMemLimit, bResult);
		if( !bResult )
			return (pbyTmp - pbyBuf);
		pbyTmp += sizeof(TMt);

		u8 type = atMtAlias[i].m_AliasType;		
		TRY_MEM_COPY(pbyTmp, &type, sizeof(type), pbyMemLimit, bResult );
		if( !bResult )
			return (pbyTmp - pbyBuf);
		pbyTmp += sizeof(type);			

		if(type == mtAliasTypeH323ID || type == mtAliasTypeE164 || type == mtAliasTypeH320ID || type == mtAliasTypeH320Alias)
		{
			u16 aliasLen = htons( strlen(atMtAlias[i].m_achAlias) );
			TRY_MEM_COPY(pbyTmp, &aliasLen, sizeof(aliasLen), pbyMemLimit,bResult);
			if( !bResult )
				return (pbyTmp - pbyBuf);
			pbyTmp += sizeof(aliasLen);

			u16 wHostValue = ntohs(aliasLen);
			TRY_MEM_COPY(pbyTmp, (void*)atMtAlias[i].m_achAlias, wHostValue, pbyMemLimit,bResult);
			if( !bResult )
				return (pbyTmp - pbyBuf);
			pbyTmp += ntohs(aliasLen);
		}
		else if(type == mtAliasTypeTransportAddress)
		{
			TRY_MEM_COPY(pbyTmp, (void*)&(atMtAlias[i].m_tTransportAddr), 
				sizeof(TTransportAddr), pbyMemLimit, bResult );
			if( !bResult )
				return (pbyTmp - pbyBuf);
			pbyTmp += sizeof(TTransportAddr);
		}
	}
	
	return (pbyTmp - pbyBuf);

#undef TRY_MEM_COPY
	
}

/*====================================================================
    函数名      ：ProcMtMcuApplySpeakerNotif
    功能        ：终端申请发言
    算法实现    ：将发言申请提交给主席
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/21    3.0          JQL          创建
====================================================================*/
void CMcuVcInst::ProcMtMcuApplySpeakerNotif( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

    TMt tMt;
    TMt tChairman;
	
    
	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );


	switch( CurState() )
	{
	case STATE_ONGOING:
        {
			//未与会
			if( !m_tConfAllMtInfo.MtJoinedConf( tMt ) )
			{								
				return;
			}			
					
			//有主席
			if( HasJoinedChairman() )
			{
				tChairman = m_tConf.GetChairman();
				
				//主席申请发言,同意
            
				if( tMt.IsLocal() && 
					tChairman.GetMtId() == tMt.GetMtId() &&
					//zbq[06/11/2010] VCS会议通过UI确认才可以
					VCS_CONF != m_tConf.GetConfSource() )
				{
					// xsl [8/22/2006]卫星分散会议时需要判断回传通道数
					if (m_tConf.GetConfAttrb().IsSatDCastMode() && IsMultiCastMt(tMt.GetMtId())
						&& //IsSatMtOverConfDCastNum(tMt))
						!IsSatMtCanContinue(tChairman.GetMtId(),emChairMan))
					{
						if (cServMsg.GetEventId() == MCU_MCU_APPLYSPEAKER_REQ)
						{
							cServMsg.SetEventId(pcMsg->event + 2);
							SendReplyBack(cServMsg, cServMsg.GetEventId());
						}

						ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMtMcuApplySpeakerNotif] over max upload mt num. ignore it!\n");
					}
					else
					{
						if( !tMt.IsLocal() )
						{
							OnMMcuSetIn( tMt,m_cVCSConfStatus.GetCurSrcSsnId(),SWITCH_MODE_BROADCAST );
						}			

						ChangeSpeaker( &tMt );						
					}				
					return;
				}

				// xsl [11/3/2006] 由配置文件决定是否将终端申请信息通知主席终端
				if (g_cMcuVcApp.IsChairDisplayMtApplyInfo())
				{
					//通知主席
					cServMsg.SetMsgBody((u8*)&tMt, sizeof(tMt));
					cServMsg.SetDstMtId( tChairman.GetMtId() );			
					SendMsgToMt( tChairman.GetMtId(), MCU_MT_APPLYSPEAKER_NOTIF, cServMsg );
				}            
				else
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMtMcuApplySpeakerNotif] not permit send message to chair mt for mt apply speaker\n");
				}
			}			

			//通知会控
			cServMsg.SetMsgBody((u8*)&tMt, sizeof(tMt));
			SendMsgToAllMcs( MCU_MCS_MTAPPLYSPEAKER_NOTIF, cServMsg );

        }
		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

}

/*====================================================================
函数名      ：NotifyMcsApplyList
功能        ：刷UI当前最新的LIST
算法实现    ：
引用全局变量：
输入参数说明：const CMessage * pcMsg, 传入的消息
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
05/13/2010  4.6         张宝卿         创建
====================================================================*/
void CMcuVcInst::NotifyMcsApplyList( BOOL32 bSendToChairman /*= FALSE*/ )
{
    u8 byLen = MAXNUM_CONF_MT;
    TMt atMt[MAXNUM_CONF_MT];
    m_tApplySpeakQue.GetQueueList(atMt, byLen);
    
	
    CServMsg cServMsg;
	cServMsg.SetConfIdx( m_byConfIdx );
	cServMsg.SetConfId(m_tConf.GetConfId());
    cServMsg.SetMsgBody((u8*)&byLen, sizeof(u8));
	if (0 != byLen)
	{
		cServMsg.CatMsgBody((u8*)atMt, sizeof(TMt) * byLen);
	}
    
    SendMsgToAllMcs( MCU_MCS_MTAPPLYSPEAKERLIST_NOTIF, cServMsg );
	if( bSendToChairman && VCS_CONF == m_tConf.GetConfSource() && 
		CONF_SPEAKMODE_ANSWERINSTANTLY == m_tConf.GetConfSpeakMode() 
		)
	{
		CRollMsg cRollMsg;
		cRollMsg.Reset();
		TMt tDstMt = m_tConf.GetChairman();
		u8 achTemp[255];
		memset( &achTemp[0],0,sizeof(achTemp) );
		cRollMsg.SetType( ROLLMSG_TYPE_SMS );
		cRollMsg.SetRollTimes( 0xff );//因为0不代表无限滚动所有设置一个大一点的值
		cRollMsg.SetRollRate( 1 );
		cRollMsg.SetMsgSrcMtId( LOCAL_MCUID,0 );
		cRollMsg.SetRollMsgContent();

		
		if( byLen > 0 )
		{
			strcat( (s8*)&achTemp[0],"当前抢答列表:" );
			for( u8 byLoop = 0;byLoop < MAXNUM_ANSWERINSTANTLY_MT && byLoop < byLen;++byLoop )
			{			
				if( byLoop > 0 )
				{
					strcat( (s8*)&achTemp[0],"、" );
				}
				GetMtAlias( atMt[byLoop],(s8*)(&achTemp[0] + strlen((s8*)&achTemp[0])) );			
			}
			cRollMsg.SetRollMsgContent( &achTemp[0],strlen((s8*)&achTemp[0]) + 1 );		
		}
		
		
		ProcSingleMtSmsOpr( cServMsg,&tDstMt,&cRollMsg );
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT,  "[NotifyMcsApplyList] Send Msg To Chairman,Msg(%s)\n",&achTemp[0] );
	}
    
    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[NotifyMcsApplyList] MCU_MCS_MTAPPLYSPEAKERLIST_NOTIF with Len.%d!\n", byLen);
	
    return;
}

/*====================================================================
    函数名      ：ProcMtMcuApplyChairmanReq
    功能        ：终端申请主席处理函数
    算法实现    ：如果无主席直接给予主席令牌，负拒绝并通知主席
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/01/02    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::ProcMtMcuApplyChairmanReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	TMt	tChairman;

	switch( CurState() )
	{
	case STATE_ONGOING:

		//无主席模式,拒绝
		if( !m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) || m_tConf.m_tStatus.IsNoChairMode() )
		{
			cServMsg.SetErrorCode( ERR_MCU_NOCHAIRMANMODE );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				
			return;
		}
		
		//有主席时
		if( HasJoinedChairman() && m_tConf.GetChairman().GetMtId() != tMt.GetMtId() )
		{
			
			cServMsg.SetErrorCode( ERR_MCU_CHAIRMANEXIST );
			SendReplyBack( cServMsg, pcMsg->event + 2 );

            // xsl [11/3/2006] 由配置文件决定是否将终端申请信息通知主席终端
            if (g_cMcuVcApp.IsChairDisplayMtApplyInfo())
            {
                tChairman = m_tConf.GetChairman();
			    cServMsg.SetSrcMtId( tMt.GetMtId() );
			    cServMsg.SetDstMtId( tChairman.GetMtId() );
			    cServMsg.SetMsgBody((u8*)&tMt, sizeof(tMt));
			    SendMsgToMt( tChairman.GetMtId(), MCU_MT_APPLYCHAIRMAN_NOTIF, cServMsg );
            }	
            else
            {
                ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMtMcuApplyChairmanReq] not permit send message to chair mt for mt apply chair\n");
            }

			//发往各会控
			cServMsg.SetMsgBody((u8*)&tMt, sizeof(tMt));
			SendMsgToAllMcs( MCU_MCS_MTAPPLYCHAIRMAN_NOTIF, cServMsg );
		}
		
		//无主席
		else
		{
			SendReplyBack( cServMsg, pcMsg->event + 1 );
			ChangeChairman( &tMt );
		}

		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：DaemonProcMtMcuApplyJoinReq
    功能        ：终端申请加入会议处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/06/20    4.0         顾振华   	  创建
====================================================================*/
void CMcuVcInst::DaemonProcMtMcuApplyJoinReq(BOOL32 bLowLevleMcuCalledIn, const CConfId &cConfId, const CMessage * pcMsg)
{
    if (!bLowLevleMcuCalledIn)
    {
        g_cMcuVcApp.SendMsgToConf( cConfId, pcMsg->event, pcMsg->content, pcMsg->length );
    }
    else
    {
        CServMsg cServMsg(pcMsg->content, pcMsg->length);
        TMtAlias tMtH323Alias = *(TMtAlias *)cServMsg.GetMsgBody();
        TMtAlias tMtE164Alias = *(TMtAlias *)(cServMsg.GetMsgBody() + sizeof(TMtAlias));
        TMtAlias tMtAddr      = *(TMtAlias *)(cServMsg.GetMsgBody() + sizeof(TMtAlias)*2);
	    u16      wCallRate = *(u16*)((cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+sizeof(u8)+sizeof(u8)));

        // guzh [6/19/2007] 下级MCU主动呼叫，作为本级下级MCU主动添加
        TAddMtInfo tAddMtInfo;
        u8 byOnGoingConfIdx = g_cMcuVcApp.GetConfIdx(cConfId);
        cServMsg.SetConfIdx(byOnGoingConfIdx);            
        cServMsg.SetConfId(cConfId);
        cServMsg.SetSrcMtId(0);
        cServMsg.SetSrcSsnId(0);
        if ( !tMtE164Alias.IsNull() )
        {
            memcpy(&tAddMtInfo, &tMtE164Alias, sizeof(TMtAlias));
        }
        else
        {
            memcpy(&tAddMtInfo, &tMtAddr, sizeof(TMtAlias));
        }
        tAddMtInfo.SetCallMode(CONF_CALLMODE_NONE);
        tAddMtInfo.SetCallBitRate(wCallRate);//mtadp实际以主机序发送该值[1/16/2013 chendaiwei]
        TMcu tLocalMcu;
        tLocalMcu.SetMcu(LOCAL_MCUID);
        cServMsg.SetMsgBody((u8*)&tLocalMcu, sizeof(TMcu));
        cServMsg.CatMsgBody((u8*)&tAddMtInfo, sizeof(TAddMtInfo));
		// xliang [9/20/2008] 增加标记
		cServMsg.CatMsgBody((u8*)&bLowLevleMcuCalledIn,sizeof(BOOL32));
        g_cMcuVcApp.SendMsgToConf( byOnGoingConfIdx, MCS_MCU_ADDMT_REQ, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

    }
}
/*====================================================================
    函数名      ：DaemonProcMtMcuApplyJoinReq
    功能        ：终端申请加入会议处理函数，给出会议列表
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/21    1.0         JQL			  创建
	04/02/19    3.0         胡昌威        修改
    06/06/22    4.0         顾振华        增加终端IP过滤
====================================================================*/
void CMcuVcInst::DaemonProcMtMcuApplyJoinReq(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	TMtAlias tMtH323Alias = *(TMtAlias *)cServMsg.GetMsgBody();
	TMtAlias tMtE164Alias = *(TMtAlias *)(cServMsg.GetMsgBody() + sizeof(TMtAlias));
	TMtAlias tMtAddr      = *(TMtAlias *)(cServMsg.GetMsgBody() + sizeof(TMtAlias)*2);
	TMtAlias tConfE164    = *(TMtAlias *)(cServMsg.GetMsgBody() + sizeof(TMtAlias)*3);
	u8       byType    = *(cServMsg.GetMsgBody()+4*sizeof(TMtAlias));
	u8       byEncrypt = *(cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+sizeof(u8));
	u16      wCallRate = *(u16*)((cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+sizeof(u8)+sizeof(u8)));
    // guzh [6/19/2007] 
    BOOL32   bLowLevelMcuCalledIn = *(BOOL32*)((cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+sizeof(u8)+sizeof(u8))+sizeof(u16));
	
	TConfInfo    *ptConfInfo = NULL;
	TConfMtTable *ptConfMtTable = NULL;
	u8 byNum = 0;
	u8 byMtId = 0;
	CConfId       cConfId = cServMsg.GetConfId();

    // 是否需要开始缺省会议
    BOOL32        bStartDefConf = FALSE;

	// 即时会议个数
	byNum = g_cMcuVcApp.GetConfNum( TRUE, FALSE, FALSE );

	//MCU上无会议
	if (0 == byNum)
	{
        bStartDefConf = TRUE;
    }

    ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq] ConfNum.%d\n", byNum);

	// 与某一会议的会议号匹配，即用户已经选择了某个会议
	if (!cConfId.IsNull() && 0 != g_cMcuVcApp.GetConfIdx(cConfId))
	{
        // 通知入会
		g_cMcuVcApp.SendMsgToConf( cConfId, pcMsg->event, pcMsg->content, pcMsg->length );
		return;
	}
    
	// 在某一会议的受邀终端列表中，找到一个即可
	for (u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++)
	{
		if (NULL != g_cMcuVcApp.GetConfInstHandle(byConfIdx))
		{
			ptConfInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConf;
			ptConfMtTable = g_cMcuVcApp.GetConfMtTable(byConfIdx);

            // zbq [08/08/2007] 基于新的终端鉴别方式，这里要作详细认定，以免投错会议，造成后期误判
            for ( u8 byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx ++ )
            {
                TMtAlias tDialAlias;
                if ( ptConfMtTable->GetDialAlias(byMtIdx, &tDialAlias) &&
                     !tDialAlias.IsNull() )
                {
                    TMtAlias tInConfMtAddr;
                    
                    if ( mtAliasTypeE164 == tDialAlias.m_AliasType &&
                         tMtE164Alias == tDialAlias &&
                         //即使有呼叫E164别名匹配，也要校验一下Ip，防止未注册GK(或注册了不同的非邻居GK)的终端冒名顶替
                         ptConfMtTable->GetMtAlias(byMtIdx, mtAliasTypeTransportAddress, &tInConfMtAddr) &&
                         tInConfMtAddr.m_tTransportAddr.GetIpAddr() == tMtAddr.m_tTransportAddr.GetIpAddr())
                    {
                        byMtId = byMtIdx;
                        break;
                    }
                    else if ( mtAliasTypeH323ID == tDialAlias.m_AliasType &&
                              tMtH323Alias == tDialAlias &&
                              //即使有呼叫E164别名匹配，也要校验一下Ip，防止未注册GK(或注册了不同的非邻居GK)的终端冒名顶替
                              ptConfMtTable->GetMtAlias(byMtIdx, mtAliasTypeTransportAddress, &tInConfMtAddr) &&
                              tInConfMtAddr.m_tTransportAddr.GetIpAddr() == tMtAddr.m_tTransportAddr.GetIpAddr())
                    {
                        byMtId = byMtIdx;
                        break;
                    }
                    else if( mtAliasTypeTransportAddress == tDialAlias.m_AliasType &&
                             tMtAddr.m_tTransportAddr.GetIpAddr() == tDialAlias.m_tTransportAddr.GetIpAddr() )
                    {
                        byMtId = byMtIdx;
                        break;
                    }
                }
            }
			//byMtId = ptConfMtTable->GetMtIdByAlias( &tMtAddr );
            
			TConfAllMtInfo *pConfAllMt = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConfAllMtInfo;
			if (byMtId > 0 && pConfAllMt->MtInConf( byMtId ))
			{
// 				g_cMcuVcApp.SendMsgToConf( ptConfInfo->GetConfId(), pcMsg->event, pcMsg->content, pcMsg->length ); 
				DaemonProcMtMcuApplyJoinReq(bLowLevelMcuCalledIn, ptConfInfo->GetConfId(), pcMsg );
				return;
			}
            /*
			byMtId = ptConfMtTable->GetMtIdByAlias( &tMtE164Alias );
			if (byMtId > 0 && pConfAllMt->MtInConf( byMtId ))
			{
				g_cMcuVcApp.SendMsgToConf( ptConfInfo->GetConfId(), pcMsg->event, pcMsg->content, pcMsg->length );                
				return;
			}
			byMtId = ptConfMtTable->GetMtIdByAlias( &tMtH323Alias );
			if (byMtId > 0 && pConfAllMt->MtInConf( byMtId ))
			{
				g_cMcuVcApp.SendMsgToConf( ptConfInfo->GetConfId(), pcMsg->event, pcMsg->content, pcMsg->length );                
				return;
			}
            */
		}
	}

	TConfNameInfo  tConfList;
    s32 nConfNum = 0;

	CMcuVcInst *pInstance = NULL;
	// 准备会议列表
	cServMsg.SetMsgBody( );
    
	for (u8 byConfIdx1 = MIN_CONFIDX; byConfIdx1 <= MAX_CONFIDX; byConfIdx1++)
	{
		pInstance = g_cMcuVcApp.GetConfInstHandle( byConfIdx1 );
		if (NULL != pInstance )
		{
		
			TConfInfo    * ptLoopConfInfo = &(pInstance->m_tConf);	
			if (ptLoopConfInfo == NULL )
			{
				 ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq] ptConfInfo is Null confidx.%d\n", byConfIdx1);
				 continue;
			}

			if ( !ptLoopConfInfo->m_tStatus.IsOngoing() )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq]confidx.%d is not a ongoing conf,not countin\n", byConfIdx1);
				continue;
			}
            ptConfInfo = ptLoopConfInfo;
			if ( CONF_OPENMODE_CLOSED != ptConfInfo->GetConfAttrb().GetOpenMode() && // 配置允许终端入会
                 ( byType == TYPE_MCU ||    //admin用户创的会议允许级联，不允许终端呼入
                   g_cMcuVcApp.IsMtIpInAllowSeg( ptConfInfo->GetUsrGrpId(), tMtAddr.m_tTransportAddr.GetNetSeqIpAddr() ) )  ) // 或者终端在该组的允许段
			{
                nConfNum ++;

				strncpy( tConfList.achConfName, ptConfInfo->GetConfName(), sizeof( tConfList.achConfName ) );
				tConfList.achConfName[sizeof( tConfList.achConfName) - 1 ] = 0;					
				tConfList.m_cConfId = ptConfInfo->GetConfId();
				cServMsg.CatMsgBody( (u8*)&tConfList, sizeof( tConfList) );	

                ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq] conf list. Index.%d, confname.%s\n", 
                        nConfNum, tConfList.GetConfName());
			}
		}
	}

    // 可用的会议数=0，直接召开缺省会议
    if (0 == nConfNum)
    {
        bStartDefConf = TRUE;
    }
    // 可用的会议数=1，直接加入
	else if (1 == nConfNum)
	{
		//////////////////////////////////////////////////////////////////////////				
		//zjj20100113已经被高行政级别级联的会议不能再被级联
		if( !bLowLevelMcuCalledIn && TYPE_MCU == byType 
			&& ptConfInfo
			&& MCS_CONF == ptConfInfo->GetConfSource() 
			&& pInstance
			&& !pInstance->m_tCascadeMMCU.IsNull() 
			)
		{
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq] conf has Cascade by a high adminLevel mcu, so can't Cascade by other mcu!!!\n");
			cServMsg.SetErrorCode( ERR_MCU_CASADEBYOTHERHIGHLEVELMCU );
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			return;
		}		
		//////////////////////////////////////////////////////////////////////////	
		if(ptConfInfo)
			DaemonProcMtMcuApplyJoinReq(bLowLevelMcuCalledIn, ptConfInfo->GetConfId(), pcMsg );
//         g_cMcuVcApp.SendMsgToConf( tConfList.m_cConfId, pcMsg->event, pcMsg->content, pcMsg->length );
        return;
    }
    else
    {
        // 发送终端列表
	    cServMsg.SetErrorCode( ERR_MCU_NULLCID );
	    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq] send conf list to mt. nConfNum.%d\n", nConfNum);
        return;
    }

    // 开始缺省会议
	// 其他会议都是通过内存中取的，缺省会议只从文件中读取 [pengguofeng 7/9/2013]
    if (bStartDefConf)
    {
		if( !g_cMcuVcApp.IsHoldDefaultConf() )
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq] Can't start def conf,because g_cMcuVcApp.IsHoldDefaultConf is false!\n");
			cServMsg.SetErrorCode(ERR_MCU_NULLCID);
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);           
			return;
		}
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq] start def conf!\n");
		TConfStore tConfStore;
		TConfStore tConfStoreBuf;
		TPackConfStore *ptPackConfStore = (TPackConfStore *)&tConfStoreBuf;
		//提取后的 tConfStore 为 TPackConfStore 结构，即已进行Pack处理的会议数据

		//根据缺省会议创会
		if (!GetConfFromFile(MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE, ptPackConfStore))
		{
			cServMsg.SetErrorCode(ERR_MCU_NULLCID);
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);

            ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "Conference %s failure because Get Conf.%d From File failed!\n", 
                            tConfStore.m_tConfInfo.GetConfName(), MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE);
			return;
		}
		//将 已进行Pack处理的会议数据 进行UnPack处理
		u16 wPackConfDataLen = 0;
		if (!UnPackConfStore(ptPackConfStore, tConfStore,wPackConfDataLen))
		{
			cServMsg.SetErrorCode(ERR_MCU_NULLCID);
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);

            ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "Conference %s failure because UnPackConf.%d From File failed!\n", 
                            tConfStore.m_tConfInfo.GetConfName(), MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE);
            return;
		}

		if (0 == byEncrypt && 
			CONF_ENCRYPTMODE_NONE != tConfStore.m_tConfInfo.GetConfAttrb().GetEncryptMode())
		{
			cServMsg.SetErrorCode(ERR_MCU_SETENCRYPT);
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);

			ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "Conference %s failure because encrypt setting is confused - byEncrypt.%d EncryptMode.%d!\n", 
					        tConfStore.m_tConfInfo.GetConfName(), byEncrypt,
                            tConfStore.m_tConfInfo.GetConfAttrb().GetEncryptMode());
			return;
		}

		TMtAlias atMtAlias[MAXNUM_CONF_MT+1];
		u16      awMtDialBitRate[MAXNUM_CONF_MT+1];
		u8       byMtNumInUse = 0;

		atMtAlias[0] = tMtAddr;
		awMtDialBitRate[0] = wCallRate;
		byMtNumInUse = 1;

        // 刷新终端列表，电视墙和画面合成模板
        TMultiTvWallModule tNewTvwallModule = tConfStore.m_tMultiTvWallModule;
        TVmpModule         tNewVmpModule    = tConfStore.m_atVmpModule;
        BOOL32 bRepeatThisLoop;
		// xliang [12/26/2008] (modify for MT calling MCU initially) 
		//cancel limit here, but need to input password later
        BOOL32 bInMtTable = FALSE; 

		TConfInfoEx tConfInfoEx;
		THduVmpModuleOrTHDTvWall tHduVmpModule;
		TVmpModuleInfo tVmpModuleEx25;
		u16 wPackConfExInfoLength = 0;
		BOOL32 bUnkownConfInfo = FALSE;
		u8 byEncoding = emenCoding_GBK;
		UnPackConfInfoEx(tConfInfoEx,tConfStore.m_byConInfoExBuf, wPackConfExInfoLength,
						bUnkownConfInfo, &tHduVmpModule, &tVmpModuleEx25, &byEncoding);
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq]byEncoding:%d\n", byEncoding);

		for (u8 byLoop = 0; byLoop < tConfStore.m_byMtNum; byLoop++)
		{
            bRepeatThisLoop = FALSE;
			if (mtAliasTypeH320ID == tConfStore.m_atMtAlias[byLoop].m_AliasType)
			{
				//如果与建会终端一致，则不重复添加
				if (0 == memcmp(tConfStore.m_atMtAlias[byLoop].m_achAlias, tMtH323Alias.m_achAlias, MAXLEN_ALIAS))
				{
					bRepeatThisLoop = TRUE;
					bInMtTable = TRUE;
					atMtAlias[0] = tMtH323Alias;
				}				
			}
			else if (mtAliasTypeTransportAddress == tConfStore.m_atMtAlias[byLoop].m_AliasType)
			{
				//如果与建会终端一致，则不重复添加
				if (tConfStore.m_atMtAlias[byLoop].m_tTransportAddr.m_dwIpAddr == tMtAddr.m_tTransportAddr.m_dwIpAddr
                    // zbq [09/19/2007] 新策略不再比较端口
					// tConfStore.m_atMtAlias[byLoop].m_tTransportAddr.m_wPort == tMtAddr.m_tTransportAddr.m_wPort
                    )
				{
					bRepeatThisLoop = TRUE;
					bInMtTable = TRUE;
					if ( tMtAddr.m_tTransportAddr.GetPort() != tConfStore.m_atMtAlias[byLoop].m_tTransportAddr.GetPort() )
					{
						tMtAddr.m_tTransportAddr.SetPort(tConfStore.m_atMtAlias[byLoop].m_tTransportAddr.GetPort());
					}
                    atMtAlias[0] = tMtAddr;
				}		
			}
			else if (mtAliasTypeE164 == tConfStore.m_atMtAlias[byLoop].m_AliasType)
			{
				//如果与建会终端一致，则不重复添加
				if (0 == memcmp(tConfStore.m_atMtAlias[byLoop].m_achAlias, tMtE164Alias.m_achAlias, MAXLEN_ALIAS))
				{
					bRepeatThisLoop = TRUE;
					bInMtTable = TRUE;
					atMtAlias[0] = tMtE164Alias;
				}		
			} 
			else if (mtAliasTypeH323ID == tConfStore.m_atMtAlias[byLoop].m_AliasType)
			{
				//如果与建会终端一致，则不重复添加
				if (0 == memcmp(tConfStore.m_atMtAlias[byLoop].m_achAlias, tMtH323Alias.m_achAlias, MAXLEN_ALIAS))
				{
					bRepeatThisLoop = TRUE;
					bInMtTable = TRUE;
					atMtAlias[0] = tMtH323Alias;
				}
			}
			else if (puAliasTypeIPPlusAlias == tConfStore.m_atMtAlias[byLoop].m_AliasType)
			{
				u32 dwNetIpAddr = *(u32*)tConfStore.m_atMtAlias[byLoop].m_achAlias; //网络序
				TMtAlias tTmpAlias;
				memcpy(tTmpAlias.m_achAlias,&tConfStore.m_atMtAlias[byLoop].m_achAlias[sizeof(dwNetIpAddr)],strlen(tConfStore.m_atMtAlias[byLoop].m_achAlias)-sizeof(dwNetIpAddr));
				
				if(tMtAddr.m_tTransportAddr.m_dwIpAddr == dwNetIpAddr 
					&& (strcmp(tMtH323Alias.m_achAlias,tTmpAlias.m_achAlias) == 0
					|| strcmp(tMtE164Alias.m_achAlias,tTmpAlias.m_achAlias) == 0)
					)
				{
					bRepeatThisLoop = TRUE;
					bInMtTable = TRUE;
					atMtAlias[0] = tConfStore.m_atMtAlias[byLoop];
				}
			}

            if (!bRepeatThisLoop)
            {
                atMtAlias[byMtNumInUse] = tConfStore.m_atMtAlias[byLoop];
                awMtDialBitRate[byMtNumInUse] = tConfStore.m_awMtDialBitRate[byLoop];
			    byMtNumInUse++;
            }

            //每次循环添加一个终端别名后，要把它原来在画面合成和电视墙里面的下标更新，
            //这有两种情况，可能是与创会者相同，则现在的下标是1，不同，则下标是当前的终端末尾
            UpdateConfStoreModule(tConfStore, 
                                  byLoop+1, bRepeatThisLoop ? 1 : byMtNumInUse,
                                  tNewTvwallModule, tNewVmpModule,tHduVmpModule,tVmpModuleEx25);         
		}

		//终端数超过 MAXNUM_CONF_MT，则舍弃最末尾一个终端
		if (byMtNumInUse > MAXNUM_CONF_MT)
		{
			byMtNumInUse = MAXNUM_CONF_MT;
		}

		const u8 byConfOpenMode = tConfStore.m_tConfInfo.GetConfAttrb().GetOpenMode();
		if ( CONF_OPENMODE_CLOSED == byConfOpenMode )
		{
			if ( !bInMtTable )
			{
				g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
				
				ConfPrint( LOG_LVL_WARNING, MID_MCU_CALL, 
					"Conference %s failure because def conf mode is CLOSED and the mt is not in conf mt list!\n", 
					tConfStore.m_tConfInfo.GetConfName());
				return;
			}
		}

		if ( byEncoding != emenCoding_Utf8 )
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq]default conference trans encoding from byEncoding.%d to UTF8\n", byEncoding);
			//confinfo 
			s8 achName[MAXLEN_ALIAS];
			memset(achName, 0, sizeof(achName));

			u32 dwLen = gb2312_to_utf8(tConfStore.m_tConfInfo.GetConfName(), achName, MAXLEN_CONFNAME+1);
			if ( dwLen > 0)
			{
				tConfStore.m_tConfInfo.SetConfName(achName);
			}
			else
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq]default conf:trans failed: conf name:%s\n", tConfStore.m_tConfInfo.GetConfName());
			}
			 
			//mt323alias ps:E164号不存在中文
			// 创建终端过来的时候已经是UTF8了，这步是由协议栈做的 [pengguofeng 7/23/2013]
/*			memset(achName, 0, sizeof(achName));
			dwLen = gb2312_to_utf8(tMtH323Alias.m_achAlias, achName, MAXLEN_ALIAS);
			if ( dwLen > 0)
			{
				memcpy(tMtH323Alias.m_achAlias, achName, dwLen);
				tMtH323Alias.m_achAlias[dwLen] = 0;
			}
			else
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq]default conf:trans failed: tMtH323Alias:%s\n", tMtH323Alias.m_achAlias);
			}

			//TMtAddr
			// 这是MT的地址，alias内容为空；transport是IP+端口，没必要转 [pengguofeng 7/23/2013]
			memset(achName, 0, sizeof(achName));
			dwLen = gb2312_to_utf8(tMtAddr.m_achAlias, achName, MAXLEN_ALIAS);
			if ( dwLen > 0)
			{
				memcpy(tMtAddr.m_achAlias, achName, dwLen);
				tMtAddr.m_achAlias[dwLen] = 0;
			}
			else
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq]default conf:trans failed: tMtAddr:%s\n", tMtAddr.m_achAlias);
			}
*/
			for (u8 byMtIdx = 0;byMtIdx < byMtNumInUse;byMtIdx++)
			{
				memset(achName, 0, sizeof(achName));
				dwLen = gb2312_to_utf8(atMtAlias[byMtIdx].m_achAlias, achName, MAXLEN_ALIAS);
				if ( dwLen > 0)
				{
					memcpy(atMtAlias[byMtIdx].m_achAlias, achName, dwLen);
					atMtAlias[byMtIdx].m_achAlias[dwLen] = 0;
				}
				else
				{
					LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[DaemonProcMtMcuApplyJoinReq]default conf:trans failed: atMtAlias[idx.%d]:%s\n", byMtIdx, atMtAlias[byMtIdx].m_achAlias);
				}
			}
		}

		//终端别名数组打包 
		s8  achAliasBuf[SERV_MSG_LEN];
		u16 wAliasBufLen = 0;
		PackTMtAliasArray(atMtAlias, awMtDialBitRate, byMtNumInUse, achAliasBuf, wAliasBufLen);
		wAliasBufLen = htons(wAliasBufLen);

		tConfStore.m_tConfInfo.m_tStatus.SetProtectMode( CONF_LOCKMODE_NONE );

		cServMsg.SetMsgBody((u8 *)&tConfStore.m_tConfInfo, sizeof(TConfInfo));
		cServMsg.CatMsgBody((u8 *)&wAliasBufLen, sizeof(wAliasBufLen));
		cServMsg.CatMsgBody((u8 *)achAliasBuf, ntohs(wAliasBufLen));
		TConfAttrb tConfAttrb = tConfStore.m_tConfInfo.GetConfAttrb();
		if (tConfAttrb.IsHasTvWallModule())
		{
			cServMsg.CatMsgBody((u8*)&tNewTvwallModule, sizeof(TMultiTvWallModule));
		}
		if (tConfAttrb.IsHasVmpModule())
		{
			cServMsg.CatMsgBody((u8*)&tNewVmpModule, sizeof(TVmpModule));
		}
		
		//将调整后的HDUvmp模板和后5风格vmp模板打包[5/30/2013 chendaiwei]
		u16 wConfInfoExLength = 0;
		PackConfInfoEx(tConfInfoEx, &tConfStore.m_byConInfoExBuf[0],
			wConfInfoExLength,&tHduVmpModule,&tVmpModuleEx25);
		if( wConfInfoExLength > CONFINFO_EX_BUFFER_LENGTH )
		{
			ConfPrint(LOG_LVL_WARNING,MID_MCU_CONF,"[DaemonProcMtMcuApplyJoinReq] conf.%s ConfInfEx.%s > CONFINFO_EX_BUFFER_LENGTH, error!\n",tConfStore.m_tConfInfo.GetConfName(),wConfInfoExLength);
			
			return;
		}
		else
		{
			cServMsg.CatMsgBody((u8*)&tConfStore.m_byConInfoExBuf[0],wConfInfoExLength);
		}
		
		cServMsg.CatMsgBody((u8 *)&tMtH323Alias, sizeof(tMtH323Alias));
		cServMsg.CatMsgBody((u8 *)&tMtE164Alias, sizeof(tMtE164Alias));
		cServMsg.CatMsgBody((u8 *)&tMtAddr, sizeof(tMtAddr));
		cServMsg.CatMsgBody((u8 *)&byType, sizeof(byType));
		// xliang [12/26/2008] (modify for MT call MCU initially) add bInMtTalbe Info
		cServMsg.CatMsgBody((u8*)&bInMtTable,sizeof(bInMtTable));

		//print test
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "bInMtTable is %d\n",bInMtTable);

        u8 byInsID = AssignIdleConfInsID();
        if(0 != byInsID)
        {
            ::OspPost(MAKEIID(AID_MCU_VC, byInsID), MT_MCU_CREATECONF_REQ,
                      cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
        else
        {
            ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[DaemonProcMtMcuApplyJoinReq] assign instance id failed!\n");
        }
		
	}

	return;
}

/*====================================================================
    函数名      ：ProcMtMcuApplyJoinReq
    功能        ：终端申请加入会议处理函数
    算法实现    ：如果开放式会议直接允许加入，否则拒绝并通知主席或主MCU控制台
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/11/06    1.0         Liaoweijiang  创建
	04/07/20    3.0         胡昌威        修改
	06/01/06	4.0			张宝卿		  T120集成标记终端呼叫方向
====================================================================*/
void CMcuVcInst::ProcMtMcuApplyJoinReq( const CMessage * pcMsg )
{
	CServMsg    cServMsg( pcMsg->content, pcMsg->length );
	TMtAlias    tMtH323Alias = *( TMtAlias * )cServMsg.GetMsgBody();
	TMtAlias    tMtE164Alias = *( TMtAlias * )( cServMsg.GetMsgBody() + sizeof(TMtAlias) );
	TMtAlias    tMtAddr      = *( TMtAlias * )( cServMsg.GetMsgBody() + sizeof(TMtAlias)*2 );
	TMtAlias    tConfE164    = *( TMtAlias * )( cServMsg.GetMsgBody() + sizeof(TMtAlias)*3 );
	u8          byType       = *(cServMsg.GetMsgBody()+sizeof(TMtAlias)*4);
	u8          byEncrypt    = *(cServMsg.GetMsgBody()+sizeof(TMtAlias)*4+sizeof(u8));
	u16         wCallRate    = *(u16*)((cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+sizeof(u8)+sizeof(u8)));
	u8          byMtId       = 0;
	u8          byRepeateMtId= 0;

	switch( CurState() )
	{
	case STATE_ONGOING:
    {
		if( byEncrypt == 0 && 
			m_tConf.GetConfAttrb().GetEncryptMode() != CONF_ENCRYPTMODE_NONE )
		{
			cServMsg.SetConfIdx( m_byConfIdx );
			cServMsg.SetConfId( m_tConf.GetConfId() );

			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			
			ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL,  "Mt 0x%x join conf %s request was refused because encrypt!\n", 
				     tMtAddr.m_tTransportAddr.GetIpAddr(), m_tConf.GetConfName() );
            return;
		}
        
        // byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAddr );

		//加密模式应该按照会议，而非来主动呼叫的终端[12/12/2012 chendaiwei]
		byEncrypt = (m_tConf.GetConfAttrb().GetEncryptMode() != CONF_ENCRYPTMODE_NONE) ? 1 : 0;

        //本终端最初的呼入方式
        TMtAlias tFstAlias;
        
        // zbq [08/07/2007] 终端加入会议，要比对呼叫别名确定新到来的终端是否在受邀列表里
        for ( u8 byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx ++ )
        {
            TMtAlias tDialAlias;
            if ( m_tConfAllMtInfo.MtInConf(byMtIdx) &&
                 m_ptMtTable->GetDialAlias(byMtIdx, &tDialAlias) )
            {
                TMtAlias tInConfMtAddr;

                if ( mtAliasTypeE164 == tDialAlias.m_AliasType &&
                     tMtE164Alias == tDialAlias )                     
                {
					//即使有呼叫E164别名匹配，也要校验一下Ip，防止未注册GK(或注册了不同的非邻居GK)的终端冒名顶替
                    if(TYPE_MT == byType && m_ptMtTable->GetMtAlias(byMtIdx, mtAliasTypeTransportAddress, &tInConfMtAddr) )
					{
						if( tInConfMtAddr.m_tTransportAddr.GetIpAddr() == tMtAddr.m_tTransportAddr.GetIpAddr() )
						{
							byMtId = byMtIdx;
							tFstAlias = tMtE164Alias;
						}
					}
					else
					{
						byMtId = byMtIdx;
						tFstAlias = tMtE164Alias;
					}                                         
                    break;
                }
                else if ( mtAliasTypeH323ID == tDialAlias.m_AliasType &&
                          tMtH323Alias == tDialAlias)
                {
					//即使有呼叫H323别名匹配，也要校验一下Ip，防止未注册GK(或注册了不同的非邻居GK)的终端冒名顶替
                    if(TYPE_MT == byType && m_ptMtTable->GetMtAlias(byMtIdx, mtAliasTypeTransportAddress, &tInConfMtAddr) )
					{
						if( tInConfMtAddr.m_tTransportAddr.GetIpAddr() == tMtAddr.m_tTransportAddr.GetIpAddr() )
						{
							byMtId = byMtIdx;
							tFstAlias = tMtH323Alias;
						}
					}
					else
					{
						byMtId = byMtIdx;
						tFstAlias = tMtH323Alias;
					}                                         
                    break;
                }
                else if( mtAliasTypeTransportAddress == tDialAlias.m_AliasType &&
                         tMtAddr.m_tTransportAddr.GetIpAddr() == tDialAlias.m_tTransportAddr.GetIpAddr() )
                {
                    byMtId = byMtIdx;
                    tFstAlias = tMtAddr;
                    break;
                }
				else if (puAliasTypeIPPlusAlias == tDialAlias.m_AliasType)
				{					
					if(tMtAddr.m_tTransportAddr.m_dwIpAddr == tDialAlias.m_tTransportAddr.m_dwIpAddr 
						&& (strcmp(tMtH323Alias.m_achAlias,tDialAlias.m_achAlias) == 0
						|| strcmp(tMtE164Alias.m_achAlias,tDialAlias.m_achAlias) == 0)
						)
					{
						byMtId = byMtIdx;
						
						//发送到VCS的Alias，需确认[12/14/2012 chendaiwei]
						tFstAlias.m_AliasType = puAliasTypeIPPlusAlias;
						memcpy(tFstAlias.m_achAlias,(s8*)&tMtAddr.m_tTransportAddr.m_dwIpAddr,sizeof(tMtAddr.m_tTransportAddr.m_dwIpAddr));
						u32 dwMaxAliasSize = sizeof(tFstAlias.m_achAlias)-sizeof(tMtAddr.m_tTransportAddr.m_dwIpAddr);
						u32 dwActualAliasSize = strlen(tDialAlias.m_achAlias);
						memcpy(&tFstAlias.m_achAlias[sizeof(tMtAddr.m_tTransportAddr.m_dwIpAddr)],tDialAlias.m_achAlias,min(dwMaxAliasSize,dwActualAliasSize));
						
						break;
					}
				}
            }
        }

        BOOL32 bMtInConf = ( byMtId > 0 /*&& m_tConfAllMtInfo.MtInConf( byMtId )*/ );

        BOOL32 bMtCallingInWithSegIp = FALSE;

        //不是受邀终端
        if ( !bMtInConf )
        {
            //封闭会议, 拒绝
		    if( m_tConf.GetConfAttrb().GetOpenMode() == CONF_OPENMODE_CLOSED )
		    {			
			    cServMsg.SetConfIdx( m_byConfIdx );
			    cServMsg.SetConfId( m_tConf.GetConfId() );

			    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);

			    ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL,  "Mt 0x%x join conf %s request was refused because conf is closed!\n", 
					      tMtAddr.m_tTransportAddr.GetIpAddr(), m_tConf.GetConfName() );
			    return;
		    }

            //[5/19/2011 zhushengze]组呼锁定状态拒绝终端申请入会
            if ( VCS_CONF == m_tConf.GetConfSource() && TYPE_MT == byType &&
                ( CONF_CREATE_MT == m_byCreateBy ||
                ( ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) && m_cVCSConfStatus.IsGroupModeLock() )
					)
                )
            {
                cServMsg.SetConfIdx( m_byConfIdx );
                cServMsg.SetConfId( m_tConf.GetConfId() );
                
                g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
                
                ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL,  "Mt 0x%x join conf %s request was refused because GroupMode conf lock!\n", 
                    tMtAddr.m_tTransportAddr.GetIpAddr(), m_tConf.GetConfName() );
			    return;
            }

            // xsl [8/2/2006] 允许mcu级联呼叫
            if ( byType != TYPE_MCU )
            {
                //本会议没有配置允许该地址的终端加入, 拒绝
				// xliang [12/25/2008] 取消用户组限制
				//zjj20130821 呼叫ip时需要校验，呼叫会议e164号时不需要校验
                /*if ( !g_cMcuVcApp.IsMtIpInAllowSeg( m_tConf.GetUsrGrpId(), tMtAddr.m_tTransportAddr.GetNetSeqIpAddr()) )
                {
                    cServMsg.SetConfIdx( m_byConfIdx );
                    cServMsg.SetConfId( m_tConf.GetConfId() );
                    
                    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
                    
                    ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "Mt 0x%x join conf %s request was refused because usr group %d not allowed!\n", 
                              tMtAddr.m_tTransportAddr.GetIpAddr(), m_tConf.GetConfName(), m_tConf.GetUsrGrpId() );
                    return;
                }
                else*/
                {
                    //zbq[08/09/2007] FIXME:介于呼入的终端挂断即被移除，这里不需
                    //要考虑MCU对其重新呼叫的情况,故可以不区分是否注册GK来来决定
                    //保存哪种类型的别名。但个别情况仍然未考虑到，比如未注册GK的
                    //终端的E164相同的情况造成误判。这里我们信任E164的高度唯一性。

                    //若为允许地址段的终端，记录其别名，优先E164或H323ID，其次Ip。
                    if ( !tMtE164Alias.IsNull() )
                    {
                        tFstAlias = tMtE164Alias;
                    }
                    else if ( !tMtH323Alias.IsNull() )
                    {
                        tFstAlias = tMtH323Alias;
                    }
                    else
                    {
                        tFstAlias = tMtAddr;
                    }

                    bMtCallingInWithSegIp = TRUE;
                }
            }
            else
            {
                //级联始终根据上级呼入的接入板地址分配ID
                tFstAlias = tMtAddr;
            }
        }

		if( VCS_CONF == m_tConf.GetConfSource() && 
					CONF_CREATE_MT != m_byCreateBy && byType == TYPE_MT //&&
					//VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode() &&
					//!ISGROUPMODE( m_cVCSConfStatus.GetCurVCMode() ) 
					 )
		{
            u8 byMtInConf = bMtInConf ? 1 : 0;
            if (!ISGROUPMODE( m_cVCSConfStatus.GetCurVCMode()))
            {
                if( m_cVCSConfStatus.GetReqVCMT().IsNull() )
                {
                    if( !m_cVCSConfStatus.GetCurVCMT().IsNull() )
                    {
                        u8 bySrcDriId = cServMsg.GetSrcDriId();	
                        u8 bySrcSsnId = cServMsg.GetSrcSsnId();
                        cServMsg.SetEventId( MCU_VCS_MTJOINCONF_REQ );
                        cServMsg.SetMsgBody(&byEncrypt, sizeof(u8));						
                        cServMsg.CatMsgBody( (u8*)&bySrcDriId,sizeof(u8) );	
                        cServMsg.CatMsgBody( (u8*)&tFstAlias,sizeof(TMtAlias) );
                        cServMsg.CatMsgBody( (u8*)&tMtH323Alias,sizeof(TMtAlias) );
                        cServMsg.CatMsgBody( (u8*)&tMtE164Alias,sizeof(TMtAlias) );
                        cServMsg.CatMsgBody( (u8*)&tMtAddr,sizeof(TMtAlias) );
                        wCallRate = ntohs(wCallRate);
                        cServMsg.CatMsgBody( (u8*)&wCallRate,sizeof(u16) );
                        cServMsg.CatMsgBody( (u8*)&bySrcSsnId,sizeof(u8) );
                        cServMsg.CatMsgBody( (u8*)&byMtInConf,sizeof(byMtInConf) );
                        SendMsgToAllMcs( MCU_VCS_MTJOINCONF_REQ,cServMsg );
                        return;
                    }
                }
                else
                {
                    cServMsg.SetConfIdx( m_byConfIdx );
                    cServMsg.SetConfId( m_tConf.GetConfId() );
                    cServMsg.SetErrorCode( ERR_MCU_CONFNUM_EXCEED );
                    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
                    
                    ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcMtMcuApplyJoinReq] Mt join conf request was refused because vcs conf has reqMt(%d)!\n", 
                        m_cVCSConfStatus.GetReqVCMT().GetMtId() );
                    return;
			    }	
            }
            else
            {
                if( !bMtInConf )
                {
                    u8 bySrcDriId = cServMsg.GetSrcDriId();	
                    u8 bySrcSsnId = cServMsg.GetSrcSsnId();
                    cServMsg.SetEventId( MCU_VCS_MTJOINCONF_REQ );
                    cServMsg.SetMsgBody(&byEncrypt, sizeof(u8));						
                    cServMsg.CatMsgBody( (u8*)&bySrcDriId,sizeof(u8) );	
                    cServMsg.CatMsgBody( (u8*)&tFstAlias,sizeof(TMtAlias) );
                    cServMsg.CatMsgBody( (u8*)&tMtH323Alias,sizeof(TMtAlias) );
                    cServMsg.CatMsgBody( (u8*)&tMtE164Alias,sizeof(TMtAlias) );
                    cServMsg.CatMsgBody( (u8*)&tMtAddr,sizeof(TMtAlias) );
                    wCallRate = ntohs(wCallRate);
                    cServMsg.CatMsgBody( (u8*)&wCallRate,sizeof(u16) );
                    cServMsg.CatMsgBody( (u8*)&bySrcSsnId,sizeof(u8) );
                    cServMsg.CatMsgBody( (u8*)&byMtInConf,sizeof(byMtInConf) );
                    SendMsgToAllMcs( MCU_VCS_MTJOINCONF_REQ,cServMsg );
                    return;
                }
                
			}
					
		}
   
		wCallRate -= GetAudioBitrate( m_tConf.GetMainAudioMediaType() );               
        
		//分配终端ID号
		byMtId = AddMt( tFstAlias, wCallRate, CONF_CALLMODE_NONE, TRUE );

		//终端呼叫入会，其必不在受邀列表，如果为单方调度则设置为请求调度终端，待通道打开后和主席互相选看
		if ( VCS_CONF == m_tConf.GetConfSource() && !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
			CONF_CREATE_MT != m_byCreateBy && byType == TYPE_MT
			)
		{
			TMt tMt = m_ptMtTable->GetMt(byMtId);
			m_cVCSConfStatus.SetReqVCMT( tMt );
			SetTimer( MCUVC_VCMTOVERTIMER_TIMER, (g_cMcuVcApp.GetVcsMtOverTimeInterval() + 5)*1000  );			
		}
		

        // zbq [08/09/2007] 经过允许Ip段呼入,但呼叫别名又不是Ip的终端,此处需保存其Ip
        if ( bMtCallingInWithSegIp && !(tFstAlias == tMtAddr) )
        {
            m_ptMtTable->SetMtAlias( byMtId, &tMtAddr );
		}

		//[1/10/2013 chendaiwei]IP为0，补填IP
		if(0 == m_ptMtTable->GetIPAddr( byMtId))
		{
			m_ptMtTable->SetIPAddr( byMtId, tMtAddr.m_tTransportAddr.GetIpAddr() );
		}

        // xsl [11/8/2006] 接入板终端计数加1
		// xliang [2/14/2009] 区分是MT还是MCU,补计数
		u8 byDriId = cServMsg.GetSrcDriId();
// 		u16 wExtraNum = (byType == TYPE_MCU)? 1: 0;
//         g_cMcuVcApp.IncMtAdpMtNum( cServMsg.GetSrcDriId(), m_byConfIdx, byMtId,wExtraNum);
        g_cMcuVcApp.IncMtAdpMtNum( cServMsg.GetSrcDriId(), m_byConfIdx, byMtId,byType);
        m_ptMtTable->SetDriId(byMtId, byDriId);
// 		if(byType == TYPE_MCU)
// 		{
// 			//接入板的终端计数还要+1
// 			//在mtadplib那端已通过判断，所以肯定有剩余接入点让其占用
// 			g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_wMtNum++;
// 		}
		
		if( byMtId > 0  )
		{
			//标记为呼入终端
			//m_ptMtTable->SetMtCallingIn( byMtId, TRUE );
            
            // zbq [08/31/2007] 受邀终端重新呼入保持其受邀身份
            if ( !bMtInConf )
            {
                m_ptMtTable->SetNotInvited( byMtId, TRUE );
            }
            //m_ptMtTable->SetAddMtMode(byMtId, ADDMTMODE_MTSELF);
			
			if(byType == TYPE_MCU)
			{
				if( m_tConf.GetConfAttrb().IsSupportCascade() )
				{
					m_ptMtTable->SetMtType(byMtId, MT_TYPE_MMCU);
				}
				else
				{
					SetInOtherConf( TRUE,byMtId );
					m_ptMtTable->SetMtType( byMtId, MT_TYPE_MT );
				}
				
// 				if (g_cMcuVcApp.IsMMcuSpeaker())
// 				{
// 					m_bMMcuSpeakerValid = TRUE;
// 				}
			}
			else if(byType == TYPE_MT)
			{
				m_ptMtTable->SetMtType( byMtId, MT_TYPE_MT );
			}
			
			m_ptMtTable->SetMtAlias(byMtId, &tMtH323Alias);
			m_ptMtTable->SetMtAlias(byMtId, &tMtE164Alias);
			if( /*!bMtInConf*/
				!m_tConfAllMtInfo.MtJoinedConf( byMtId )&& 
				VCS_CONF == m_tConf.GetConfSource() && 
				TYPE_MT == byType &&
				CONF_CREATE_MT != m_byCreateBy
				)
			{
				TMt tMt = m_ptMtTable->GetMt( byMtId );
				m_cVCSConfStatus.OprNewMt(tMt, TRUE);
				VCSConfStatusNotif();
			}
			
			cServMsg.SetConfIdx( m_byConfIdx );
			cServMsg.SetConfId( m_tConf.GetConfId() );
			cServMsg.SetDstMtId( byMtId );

			
			cServMsg.SetMsgBody(&byEncrypt, sizeof(u8));
			TCapSupport tCap = m_tConf.GetCapSupport();
			cServMsg.CatMsgBody( (u8*)&tCap, sizeof(tCap));
			TMtAlias tAlias;
			tAlias.SetH323Alias(m_tConf.GetConfName());
			cServMsg.CatMsgBody((u8 *)&tAlias, sizeof(tAlias));

			
			// Ex能力集需要带到本端Mtadp侧，避免出现本端Mtadp侧没有进行能力集更新的时候（即还没有Ex能力时），对端能力集到来，
			// 导致比出的共同双流能力有误
			TCapSupportEx tCapEx = m_tConf.GetCapSupportEx();
			cServMsg.CatMsgBody( (u8*)&tCapEx, sizeof(tCapEx));
			
			// 组织扩展能力集勾选给Mtadp [12/8/2011 chendaiwei]
			TVideoStreamCap atMSVideoCap[MAX_CONF_CAP_EX_NUM];
			u8 byCapNum = MAX_CONF_CAP_EX_NUM;
			m_tConfEx.GetMainStreamCapEx(atMSVideoCap,byCapNum);
			
			TVideoStreamCap atDSVideoCap[MAX_CONF_CAP_EX_NUM];
			u8 byDSCapNum = MAX_CONF_CAP_EX_NUM;
			m_tConfEx.GetDoubleStreamCapEx(atDSVideoCap,byDSCapNum);
			
			cServMsg.CatMsgBody((u8*)&atMSVideoCap[0], sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);
			cServMsg.CatMsgBody((u8*)&atDSVideoCap[0], sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);
			//TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
			//u8 byAudioTrackNum = tAudioType.GetAudioTrackNum();
			//cServMsg.CatMsgBody(&byAudioTrackNum,sizeof(byAudioTrackNum));
			TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];//音频能力
			//从会议属性中取出会议支持的音频类型
			m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);
			cServMsg.CatMsgBody((u8*)&atAudioTypeDesc[0], sizeof(TAudioTypeDesc)* MAXNUM_CONF_AUDIOTYPE);
			
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+1, cServMsg);			
			
			g_cMcuVcApp.UpdateAgentAuthMtNum();
		}
		else
		{
			cServMsg.SetConfIdx( m_byConfIdx );
			cServMsg.SetConfId( m_tConf.GetConfId() );
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);

			ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL,  "Mt 0x%x join conf %s request was refused because conf full!\n", 
					  tMtAddr.m_tTransportAddr.GetIpAddr(), m_tConf.GetConfName() );
		}

		break;
    }
	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

	return;
}

/*====================================================================
    函数名      ：ProcMtMcuSendMcMsgReq
    功能        ：终端发给主MCU控制台的短消息处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本 1.0         Liaoweijiang  创建
====================================================================*/
void CMcuVcInst::ProcMtMcuSendMcMsgReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	
	switch( CurState() )
	{
	case STATE_ONGOING:
		//发到本MCU所有的控制台
		SendMsgToAllMcs( MCU_MCS_SENDRUNMSG_NOTIF, cServMsg );
		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMtMcuStartSwitchMtReq
    功能        ：终端交换处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	02/11/26	1.0			LI Yi         创建
	03/02/26    1.0         Qzj           增强源终端与会与否的判断
	04/02/21    3.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::ProcMtMcuStartSwitchMtReq( const CMessage * pcMsg )
{    
    STATECHECK;
    
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	
	ProcStartSelSwitchMt(cServMsg);
	
    return;
}

/*====================================================================
函数名      ：ProcStartSelSwitchMt
功能        ：选看终端处理函数
算法实现    ：
引用全局变量：
输入参数说明： CServMsg &cServMsg 传入的消息
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2010/09/15	4.6			薛亮		  整理创建，VMP前适配修改			   	
====================================================================*/
void CMcuVcInst::ProcStartSelSwitchMt(CServMsg &cServMsg)
{
	TSwitchInfo	tSwitchInfo;
	TMt	tDstMt, tSrcMt;
	u8  byDstMtId;
	CServMsg  cMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	TMtStatus tSrcMtStatus;
    TMtStatus tDstMtStatus;    

    tSwitchInfo = *( TSwitchInfo * )cServMsg.GetMsgBody();	
    tSrcMt = tSwitchInfo.GetSrcMt();
    tDstMt = tSwitchInfo.GetDstMt();
	
	TMt tMcAudCurSrc;
	TMt tMcVidCurSrc;
	g_cMcuVcApp.GetMcSrc( cServMsg.GetSrcSsnId(), &tMcAudCurSrc, tSwitchInfo.GetDstChlIdx(), MODE_AUDIO );
	g_cMcuVcApp.GetMcSrc( cServMsg.GetSrcSsnId(), &tMcVidCurSrc, tSwitchInfo.GetDstChlIdx(), MODE_VIDEO );

	// 卫星会议限制，移到前面来判断，后面的都删除 [pengguofeng 1/16/2013]
    m_ptMtTable->GetMtStatus(tDstMt.GetMtId(), &tDstMtStatus);
    TMt tVidSrc/* = tDstMtStatus.GetSelectMt(MODE_VIDEO)*/;
	u8 bySkipVcChnnl = 0xff;
	emSatReplaceType emSatType = emDefault;
	if ( cServMsg.GetEventId() == MCS_MCU_STARTSWITCHMC_REQ )
	{
		tVidSrc.SetNull();
		tVidSrc.SetEqpId(cServMsg.GetSrcSsnId());
		bySkipVcChnnl = tSwitchInfo.GetDstChlIdx();
		emSatType = emStartOrChangeMonitor;
	}
	else
	{
		tVidSrc = tDstMt;
		emSatType = emSelect;
	}
	if ( /*!tVidSrc.IsNull()*/
		( tSwitchInfo.GetMode() == MODE_VIDEO || tSwitchInfo.GetMode() == MODE_BOTH ) //只有当选看带视频时才判断
		//&& !(tVidSrc == tSrcMt)
		&& IsMultiCastMt(tSrcMt.GetMtId())
		&& m_tConf.GetConfAttrb().IsSatDCastMode())
	{
		//if ( IsSatMtOverConfDCastNum(tSrcMt, emSatType, bySkipVcId, bySkipVcChnnl, 0xff, 0xff, 0xff, bySkipMtId))
		if(!IsSatMtCanContinue(GetLocalMtFromOtherMcuMt(tSrcMt).GetMtId(),emSatType,&tVidSrc,bySkipVcChnnl))
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[ProcStartSelSwitchMt]Select Src:%d over Max Sat BandSwitch, cancel\n", tSrcMt.GetMtId());
			cServMsg.SetErrorCode(ERR_MCU_DCAST_OVERCHNNLNUM);
			SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
			return;
		}
	}

	//双选看暂不支持回传
	if (MODE_VIDEO2SECOND == tSwitchInfo.GetMode() && !tSrcMt.IsLocal())
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, "[ProcStartSelSwitchMt] tSrcMt(%d,%d) is not local!\n", 
			tSrcMt.GetMcuId(), tSrcMt.GetMtId());
		SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
		return;
	}

	//zhouyiliang 20120831 主席轮询模式下 目的端为主席终端时选看则停轮询
	// miaoqingsong [2011/08/09] 主席轮询模式下
	if ( ( (CONF_POLLMODE_VIDEO_CHAIRMAN == m_tConf.m_tStatus.GetPollMode() && tSwitchInfo.GetMode() != MODE_AUDIO ) ||
		    (CONF_POLLMODE_BOTH_CHAIRMAN == m_tConf.m_tStatus.GetPollMode())
		  )  
		  && tDstMt == m_tConf.GetChairman() && !(tSrcMt == m_tConf.m_tStatus.GetMtPollParam().GetTMt()) 
		)
	{
// 		ConfPrint( LOG_LVL_ERROR, MID_MCU_MT, "[ProcStartSelSwitchMt] Can't start chairman's select because Chairman Polling!\n" );
// 		cServMsg.SetErrorCode( ERR_MCU_SPECCHAIREMAN_NACK );
// 		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	//nack
// 		return;
		ProcStopConfPoll();
	}
	
	//20120330 yhz 新需求,监控选看vmp
	if (cServMsg.GetEventId() == MCS_MCU_STARTSWITCHMC_REQ &&
		tSrcMt.GetType() == TYPE_MCUPERI && EQP_TYPE_VMP == tSrcMt.GetEqpType())
	{
		//会议不在vmp中
		if (!IsVmpIdInVmpList(tSrcMt.GetEqpId()) ||g_cMcuVcApp.GetVMPMode(tSrcMt) == CONF_VMPMODE_NONE)	
		{
			cServMsg.SetErrorCode(ERR_MCU_VMPNOTSTART);
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ProcStartSelSwitchMt] The Conf is not in the VMP mode!\n");
			SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
			return ;
		}
		
		//老VMP不支持选看
		if(GetVmpSubType(tSrcMt.GetEqpId()) == VMP)
		{
			cServMsg.SetErrorCode(ERR_FUNCTIONNOTSUPPORTBYOLDVMP);
			SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
			return ;
		}

		/*传入的eqp不是当前画面合成器外设
		if (!(tSrcMt == m_tVmpEqp))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ProcStartSelSwitchMt] The tSrcMt[%d] is not m_tVmpEqp[%d]!\n", tSrcMt.GetEqpId(), m_tVmpEqp.GetEqpId());
			SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
			return ;
		}*/
	}

	//终控台发来的消息tmt信息需要转换
	if (MT_MCU_STARTSELMT_CMD == cServMsg.GetEventId() ||
		MT_MCU_STARTSELMT_REQ == cServMsg.GetEventId())
	{
		//[nizhijun 2010/11/12] 终控台轮询，无视频源跳过
		BOOL32 bVideoLose = FALSE;
		if (tSrcMt.IsLocal())
        {
            TMtStatus tMtStatus;
            m_ptMtTable->GetMtStatus(tSrcMt.GetMtId(), &tMtStatus);
            bVideoLose = tMtStatus.IsVideoLose();
        }
        else
        {
			TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tSrcMt.GetMcuId());
            if (NULL != ptMcInfo)
            {
				TMcMtStatus *pMcMtStatus = ptMcInfo->GetMtStatus((TMt &)tSrcMt);
                bVideoLose = pMcMtStatus->IsVideoLose();
            }
        }
		if (bVideoLose)
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "Mt(%d,%d) Select see Mt(%d,%d), returned with Nack,because of VideoLose!\n", 
                tDstMt.GetMcuId(), 
                tDstMt.GetMtId(),
                tSrcMt.GetMcuId(), 
                tSrcMt.GetMtId());

			// 回NACK
			SendReplyBack( cServMsg, MCU_MT_STARTSELMT_NACK );
			return;
		}

	}

	if ( MT_MCU_STARTSELMT_CMD == cServMsg.GetEventId() ||
		 MT_MCU_STARTSELMT_REQ == cServMsg.GetEventId() || 
		 MCS_MCU_STARTSWITCHMT_REQ == cServMsg.GetEventId())
	{
		if(m_ptMtTable->GetMtStatus(tDstMt.GetMtId(), &tDstMtStatus))
		{
			//指定目的终端已选看视频
			if (!tDstMtStatus.GetSelectMt(MODE_VIDEO).IsNull() &&
				tDstMtStatus.GetSelectMt(MODE_VIDEO) == tSrcMt)
			{
				//新选看模式为video,老选看模式为video或both，直接return
				if (tDstMtStatus.HasModeInSelMode(MODE_VIDEO) && MODE_VIDEO == tSwitchInfo.GetMode())
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcStartSelSwitchMt] OldSel[Mode.%d] is equal or larger than NewSel[Mode.%d]!\n",
						tDstMtStatus.GetSelByMcsDragMode(), tSwitchInfo.GetMode());

					SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
					return;
				}
				//新选看模式为both,老选看模式为video或both，过滤视频，改成新选看音频
				else if (tDstMtStatus.HasModeInSelMode(MODE_VIDEO) && MODE_BOTH == tSwitchInfo.GetMode())
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcStartSelSwitchMt]  NewSel[Mode.%d] change to NewSel[Mode.%d]]!\n",
						tSwitchInfo.GetMode(), MODE_AUDIO);

					tSwitchInfo.SetMode(MODE_AUDIO);
				}
			}
			
			if (!tDstMtStatus.GetSelectMt(MODE_AUDIO).IsNull() &&
				tDstMtStatus.GetSelectMt(MODE_AUDIO) == tSrcMt)
			{
				//新选看模式为audio,老选看模式为audio或both，直接return
				if (tDstMtStatus.HasModeInSelMode(MODE_AUDIO) && MODE_AUDIO == tSwitchInfo.GetMode())
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcStartSelSwitchMt] OldSel[Mode.%d] is equal or larger than NewSel[Mode.%d]!\n",
						tDstMtStatus.GetSelByMcsDragMode(), tSwitchInfo.GetMode());

					SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
					return;
				}
				//新选看模式为both,老选看模式为audio或both，过滤音频，改成新选看视频
				else if (tDstMtStatus.HasModeInSelMode(MODE_AUDIO) && MODE_BOTH == tSwitchInfo.GetMode())
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcStartSelSwitchMt]  NewSel[Mode.%d] change to NewSel[Mode.%d]]!\n",
						tSwitchInfo.GetMode(), MODE_VIDEO);
					
					tSwitchInfo.SetMode(MODE_VIDEO);
				}
			}
		}

		if (MODE_VIDEO2SECOND == tSwitchInfo.GetMode() 
			&& !(m_ptMtTable->GetMtSelMtByMode(tDstMt.GetMtId(), MODE_VIDEO2SECOND).IsNull())
			&& m_ptMtTable->GetMtSelMtByMode(tDstMt.GetMtId(), MODE_VIDEO2SECOND) == tSrcMt)
		{
			tSwitchInfo.SetMode(MODE_NONE);
		}
	}


	//防止SendReplyBack将消息返回应答，将Src设为NULL
	if( cServMsg.GetEventId() == MT_MCU_STARTSELMT_CMD )
	{
		cServMsg.SetNoSrc();
        cServMsg.SetSrcMtId(0);
	}


	//如果不是会控选看终端，强制广播下的非主席选看，NACK
	if( cServMsg.GetEventId() != MCS_MCU_STARTSWITCHMC_REQ && 
		m_tConf.m_tStatus.IsMustSeeSpeaker() && 
        !(tDstMt == m_tConf.GetChairman()) )
	{
		cServMsg.SetErrorCode( ERR_MCU_MUSTSEESPEAKER );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

    // 顾振华 [5/30/2006]
    // 如果是下级请求选看上级，就直接返回, 不NACK
    if (cServMsg.GetEventId() == MCS_MCU_STARTSWITCHMT_REQ &&
        (tDstMt.IsNull() || !tDstMt.IsLocal()) )
    {
        ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "Mt(%d,%d) Select see Mt(%d,%d), returned with ack!\n", 
                tDstMt.GetMcuId(), 
                tDstMt.GetMtId(),
                tSrcMt.GetMcuId(), 
                tSrcMt.GetMtId());

        SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
        return;
    }

	if( tDstMt.GetType() == TYPE_MT)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "Mt(%d,%d) select see Mt(%d,%d)\n", tDstMt.GetMcuId(), tDstMt.GetMtId(), tSrcMt.GetMcuId(), tSrcMt.GetMtId() );

		tDstMt = GetLocalMtFromOtherMcuMt( tDstMt );				
		tDstMt = m_ptMtTable->GetMt(tDstMt.GetMtId());
		tSwitchInfo.SetDstMt( tDstMt );		
	}

    TMt tLocalMt;
	u8 bySwitchMode = tSwitchInfo.GetMode();	// 记录初始MODE
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "Select tSrcMt(%d, %d)!\n", tSrcMt.GetMcuId(), tSrcMt.GetMtId());
	// 源为外设时,如监控选看vmp
	if (tSrcMt.GetType() == TYPE_MCUPERI)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcStartSelSwitchMt] The Src is EQP, EqpType:%d, EqpID:%d!\n",
			tSrcMt.GetEqpType(), tSrcMt.GetEqpId());
		if (tSrcMt.GetEqpType() == EQP_TYPE_VMP)
		{
			// 画面合成只有视频源
			bySwitchMode = MODE_VIDEO;
		}
	}
	else
	{
		tLocalMt = GetLocalMtFromOtherMcuMt(tSrcMt);
		byDstMtId = tDstMt.GetMtId();
		//源终端未入会议，NACK
		if( !m_tConfAllMtInfo.MtJoinedConf( tSrcMt ) &&
			m_tConf.GetConfSource() != VCS_CONF)
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "Select source Mt(%u,%u) has not joined current conference!\n",
				tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
			cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}    
		
		//终端状态和选看模式不匹配, NACK
		// zgc, 2008-07-10, 修改判断方式, 当最终过滤结果为MODE_NONE时，拒绝选看，否则给出提示
		GetMtStatus(tLocalMt, tSrcMtStatus);
		m_ptMtTable->GetMtStatus(byDstMtId, &tDstMtStatus);
		if( (!tSrcMtStatus.IsSendVideo() ||
			(tDstMt.GetType() == TYPE_MT && !tDstMtStatus.IsReceiveVideo()) ) &&
			(bySwitchMode == MODE_VIDEO || bySwitchMode == MODE_BOTH || bySwitchMode == MODE_VIDEO2SECOND) )
		{
			bySwitchMode = ( MODE_BOTH == bySwitchMode ) ? MODE_AUDIO : MODE_NONE;
		}	
		
		if( (!tSrcMtStatus.IsSendAudio() ||
			(tDstMt.GetType() == TYPE_MT && !tDstMtStatus.IsReceiveAudio()) ) &&
			( bySwitchMode == MODE_AUDIO || bySwitchMode == MODE_BOTH ) )
		{
			bySwitchMode = ( MODE_BOTH == bySwitchMode ) ? MODE_VIDEO : MODE_NONE;
		}
	}

	// 给MCS发通知
	u8 byMcsId = 0;
	if ( cServMsg.GetEventId() == MCS_MCU_STARTSWITCHMT_REQ || 
		cServMsg.GetEventId() == MCS_MCU_STARTSWITCHMC_REQ )
	{
		byMcsId = cServMsg.GetSrcSsnId();
	}

	//20110818 zjl 音视频接收或发送通道都没打开，直接nack
    if ( MODE_NONE == bySwitchMode )
    {
        cServMsg.SetErrorCode( ERR_MCU_SRCISRECVONLY );
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        return;
	}
	//音频或视频(接收或发送)有一个没打开，则通告mcs
	else if(bySwitchMode !=  tSwitchInfo.GetMode())
	{
		NotifyMcsAlarmInfo(byMcsId, ERR_MCU_SRCISRECVONLY);
	}


    //zbq[05/22/2009] 终端选看适配支持 FIXME: 暂未处理标清适配的动态抢占
	BOOL32 bVidAdp = FALSE;
	BOOL32 bAudAdp = FALSE;
	BOOL32 bSecVidAdp = FALSE;
    //终端选看
    if( TYPE_MT == tDstMt.GetType() )
    {
		//如果是本级终端或单回传则判断上级是否需要适配，多回传情况下上级直接指定终端可接受分辨率让下级调整
		if(tSrcMt.IsLocal() || (!tSrcMt.IsLocal() && !IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuId())))
		{
			if (MODE_VIDEO == bySwitchMode || MODE_BOTH == bySwitchMode)
			{
				if (IsNeedSelAdpt(tSrcMt, tDstMt, MODE_VIDEO))
				{
					if (g_cMcuVcApp.IsSelAccord2Adp())
					{
						bVidAdp = TRUE;
					}
					else
					{
						cServMsg.SetErrorCode( ERR_MCU_NOTSAMEMEDIATYPE );
						SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
						if (MODE_VIDEO == bySwitchMode)
						{
							return;
						}
						else if (MODE_BOTH == bySwitchMode)
						{
							bySwitchMode = MODE_AUDIO;
						}
					}					
				}
			}
			if(MODE_AUDIO == bySwitchMode || MODE_BOTH == bySwitchMode)
			{
				if (IsNeedSelAdpt(tSrcMt, tDstMt, MODE_AUDIO))
				{
					if (g_cMcuVcApp.IsSelAccord2Adp())
					{
						bAudAdp = TRUE;
					}
					else
					{
						cServMsg.SetErrorCode( ERR_MCU_NOTSAMEMEDIATYPE );
						SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
						if (MODE_AUDIO == bySwitchMode)
						{
							return;
						}
						else if (MODE_BOTH == bySwitchMode)
						{
							bySwitchMode = MODE_VIDEO;
						}
					}
				}
			}

			TLogicalChannel tLogicalChannel;
			if (MODE_VIDEO2SECOND == bySwitchMode
				&& m_ptMtTable->GetMtLogicChnnl(tDstMt.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, TRUE))
			{
				if (IsNeedSelAdpt(tSrcMt, tDstMt, MODE_VIDEO2SECOND))
				{
					if (g_cMcuVcApp.IsSelAccord2Adp())
					{
						bSecVidAdp = TRUE;
					}
					else
					{
						cServMsg.SetErrorCode(ERR_MCU_NOTSAMEMEDIATYPE);
						SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
						return;
					}					
				}
			}
		}	
	}
	
	// 给MCS发通知
// 	u8 byMcsId = 0;
// 	if ( cServMsg.GetEventId() == MCS_MCU_STARTSWITCHMT_REQ || 
// 		 cServMsg.GetEventId() == MCS_MCU_STARTSWITCHMC_REQ )
// 	{
// 		byMcsId = cServMsg.GetSrcSsnId();
// 	}

//  if ( MODE_VIDEO == bySwitchMode && MODE_BOTH == tSwitchInfo.GetMode() )
// 	{
// 		NotifyMcsAlarmInfo( byMcsId, ERR_MCU_SELMODECHANGE_AUDIOFAIL );
// 	}
// 
//     if ( MODE_AUDIO == bySwitchMode && MODE_BOTH == tSwitchInfo.GetMode() )
// 	{
// 		NotifyMcsAlarmInfo( byMcsId, ERR_MCU_SELMODECHANGE_VIDEOFAIL );
// 	}

	// 是内部选看，不需要设置mcsdragmod
	BOOL32 bInnerSelect = FALSE;

	//[2011/10/14/zhangli]因为在停目的终端旧的选看避免图像闪动，StopSelectSrc最后一个参数为FALSE，只清空选看状态，不停交换
	//这里记录目的终端旧的选看源，如果选看失败，用于停掉旧的选看
	TMt tOldSelVid;
	TMt tOldSelAdu;
	TMt tOldSelSecVid;

    tSwitchInfo.SetMode( bySwitchMode );

    switch( cServMsg.GetEventId() )
    {
	case MCS_MCU_STARTSWITCHMT_REQ:
	case MT_MCU_STARTSELMT_CMD:
	case MT_MCU_STARTSELMT_REQ:
        {
			if ( cServMsg.GetSrcSsnId() == 0   )		
			{
				bInnerSelect = TRUE;
			}
			//[杨怀志 2012/02/22] 主席选看轮询时,若轮询终端是当前发言人,不过滤发言人
			if (tSrcMt == m_tConf.GetSpeaker() &&
				tDstMt == m_tConf.GetChairman() &&
				(CONF_POLLMODE_BOTH_CHAIRMAN == m_tConf.m_tStatus.GetPollMode() ||
				 CONF_POLLMODE_VIDEO_CHAIRMAN == m_tConf.m_tStatus.GetPollMode()))
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "It's Chairman select pollmode, Select source Mt(%u,%u) is speaker!\n",
					tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
			}
			//[田志勇 2010/12/01] 过滤speaker			
			else if(tSrcMt  == m_tConf.GetSpeaker() ||
				( tSrcMt.IsLocal() && m_ptMtTable->GetMtType(tSrcMt.GetMtId()) == MT_TYPE_SMCU &&
					!IsLocalAndSMcuSupMultSpy( GetMcuIdxFromMcuId(tSrcMt.GetMtId()) ) &&
					tSrcMt == GetLocalMtFromOtherMcuMt(m_tConf.GetSpeaker()) 
					|| tSrcMt == GetLocalMtFromOtherMcuMt(GetSecVidBrdSrc()))
				)
			{
				TMtStatus tStatus;
				m_ptMtTable->GetMtStatus( tDstMt.GetMtId(),&tStatus );
				TMt tSelVMt = tStatus.GetSelectMt( MODE_VIDEO );
				TMt tSelAMt = tStatus.GetSelectMt( MODE_AUDIO );
				TMt tVSrc,tASrc;
				m_ptMtTable->GetMtSrc( tDstMt.GetMtId(), &tVSrc, MODE_VIDEO );
				m_ptMtTable->GetMtSrc( tDstMt.GetMtId(), &tASrc, MODE_AUDIO );				

				if(MODE_BOTH == bySwitchMode)
				{
					if(GetVidBrdSrc().GetType() != TYPE_MCUPERI && (tSelVMt == GetVidBrdSrc() || tVSrc == GetVidBrdSrc())
						|| tSrcMt == GetSecVidBrdSrc())
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcStartSelSwitchMt] Now DstMt(%d.%d) Is See SrcMt(%d.%d),so change Mode to MODE_AUDIO.\n",
							tDstMt.GetMcuId(),tDstMt.GetMtId(),tSrcMt.GetMcuId(),tSrcMt.GetMtId());
						bySwitchMode = MODE_AUDIO;
					}
					
					if(GetAudBrdSrc().GetType() != TYPE_MCUPERI 
						&& (tSelAMt == GetAudBrdSrc() || tASrc == GetAudBrdSrc()))
					{
						bySwitchMode = (bySwitchMode == MODE_BOTH) ? MODE_VIDEO : MODE_NONE;
						
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcStartSelSwitchMt] Now DstMt(%d.%d) Is Listen SrcMt(%d.%d),so change Mode to %d.\n",
							tDstMt.GetMcuId(),tDstMt.GetMtId(),tSrcMt.GetMcuId(),tSrcMt.GetMtId(), bySwitchMode);
					}
				}
				else if(MODE_VIDEO == bySwitchMode)
				{
					if(GetVidBrdSrc().GetType() != TYPE_MCUPERI && (tSelVMt == GetVidBrdSrc() || tVSrc == GetVidBrdSrc())
						|| tSrcMt == GetSecVidBrdSrc())
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcStartSelSwitchMt] Now DstMt(%d.%d) Is See SrcMt(%d.%d),so change Mode to MODE_AUDIO.\n",
							tDstMt.GetMcuId(),tDstMt.GetMtId(),tSrcMt.GetMcuId(),tSrcMt.GetMtId());
						bySwitchMode = MODE_NONE;
					}
				}
				else if(MODE_AUDIO == bySwitchMode)
				{
					if(GetAudBrdSrc().GetType() != TYPE_MCUPERI 
						&& (tSelAMt == GetAudBrdSrc() || tASrc == GetAudBrdSrc()))
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcStartSelSwitchMt] Now DstMt(%d.%d) Is Listen SrcMt(%d.%d),so change Mode to MODE_NONE.\n",
							tDstMt.GetMcuId(),tDstMt.GetMtId(),tSrcMt.GetMcuId(),tSrcMt.GetMtId());
						bySwitchMode = MODE_NONE;
					}
				}
				else if (MODE_VIDEO2SECOND == bySwitchMode)
				{
					if(tSrcMt == GetSecVidBrdSrc())
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcStartSelSwitchMt] Now DstMt(%d.%d) Is Listen SrcMt(%d.%d),so change Mode to MODE_NONE.\n",
							tDstMt.GetMcuId(),tDstMt.GetMtId(),tSrcMt.GetMcuId(),tSrcMt.GetMtId());
						bySwitchMode = MODE_NONE;
					}
				}

				if( MODE_NONE == bySwitchMode )
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "Specified SrcMt(%u,%u) has been SPEAKER,so no need to Selct Src!\n", 
						tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
					SendReplyBack( cMsg, cMsg.GetEventId() + 1 );
					return;
				}
				tSwitchInfo.SetMode( bySwitchMode );
			}

            //目的终端未入会议，NACK
            if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
            {
                ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "Specified Mt(%u,%u) not joined conference!\n", 
                    tDstMt.GetMcuId(), tDstMt.GetMtId() );
                cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }            
            
            // xsl [7/20/2006] 卫星分散会议时需要判断回传通道数
            /*if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
                //获取此目的终端上一次选看的终端，以判断是否为替换
                BOOL32 bOverNum = FALSE;
                TMtStatus tDstStatus;
                m_ptMtTable->GetMtStatus(tDstMt.GetMtId(), &tDstStatus);
                TMt tLastMt = tDstStatus.GetSelectMt(MODE_VIDEO);
                if (!tLastMt.IsNull())
                {
                    if (IsMtSendingVidToOthers(tLastMt, FALSE, FALSE, tDstMt.GetMtId()) && IsOverSatCastChnnlNum(0))
                    {
                        bOverNum = TRUE;
                    }                    
                }
                else if (IsOverSatCastChnnlNum(tLocalMt.GetMtId()))
                {
                    bOverNum = TRUE;
                }

                if (bOverNum)
                {
                    ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMtMcuStartSwitchMtReq] over max upload mt num. nack!\n");
                    cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    return;
                }
            }*/
        
			//混音时调整选看模式
			if( m_tConf.m_tStatus.IsMixing()  )
			{
                //选看音频，NACK
				if( !(tSwitchInfo.GetDstMt() == m_tConf.GetChairman())  )
				{
					if( tSwitchInfo.GetMode() == MODE_AUDIO  )
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "CMcuVcInst: Conference %s is mixing now. Cannot switch only audio!\n", 
                             m_tConf.GetConfName() );
						cServMsg.SetErrorCode( ERR_MCU_SELAUDIO_INMIXING );
						SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
						return;
					}
					else if( tSwitchInfo.GetMode() == MODE_BOTH )
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "CMcuVcInst: Conference %s is mixing now. Cannot switch only audio!\n", 
                             m_tConf.GetConfName() );
						NotifyMcsAlarmInfo( cServMsg.GetSrcSsnId(),ERR_MCU_SELAUDIO_INMIXING );
						tSwitchInfo.SetMode( MODE_VIDEO );
					}					
				}
                
			}
            
			// [9/15/2010 xliang] vmp adapter channel seize prompt：
			if (MODE_VIDEO2SECOND != tSwitchInfo.GetMode())
			{
				TEqp tVmpEqp;
				if(!CheckSelSrcVmpAdaptChnl(tSrcMt, tDstMt, tSwitchInfo.GetMode(), tVmpEqp))
				{
					TVmpPriSeizeInfo tPriSeiInfo = g_cMcuVcApp.GetVmpPriSeizeInfo(tVmpEqp);
					//记录抢占回来的后续操作
					if(tPriSeiInfo.m_tPriSeizeMember.IsNull())
					{
						tPriSeiInfo.m_tPriSeizeMember.SetMt(tSrcMt);
						tPriSeiInfo.m_wEventId = MCS_MCU_STARTSWITCHMT_REQ;
						//tSwitchInfo.SetMode(MODE_VIDEO); 此处更新了tSwitchInfo，将导致下面直接return
						tPriSeiInfo.m_tSwitchInfo = tSwitchInfo;
						tPriSeiInfo.m_tSwitchInfo.SetMode(MODE_VIDEO);
						tPriSeiInfo.m_bySrcSsnId = cServMsg.GetSrcSsnId();//记录源会话号,用于区分是否mcs拖拽选看
						g_cMcuVcApp.SetVmpPriSeizeInfo(tVmpEqp, tPriSeiInfo);
					}
					
					if (MODE_VIDEO == tSwitchInfo.GetMode())
					{
						return;
					}
					else
					{
						tSwitchInfo.SetMode(MODE_AUDIO);
					}
				}
			}
			//需要适配终端不能选看
            //zbq[05/22/2009] 需要适配的选看不调整码率

            if((MODE_BOTH == tSwitchInfo.GetMode() || MODE_VIDEO == tSwitchInfo.GetMode()) &&
				!bVidAdp)
            {
                u16 wSrcMtBitrate   = m_ptMtTable->GetMtSndBitrate(tLocalMt.GetMtId());
                u16 wDstMtBitrate   = m_ptMtTable->GetMtReqBitrate(tDstMt.GetMtId());
                u16 wSrcDialBitrate = m_ptMtTable->GetSndBandWidth(tLocalMt.GetMtId());
                u16 wDstDialBitrate = m_ptMtTable->GetRcvBandWidth(tDstMt.GetMtId());      
                
                if (wSrcDialBitrate <= wDstDialBitrate && wSrcMtBitrate == wSrcDialBitrate &&
                    !m_tDoubleStreamSrc.IsNull())
                {
                    CServMsg cTmpServMsg;
                    TLogicalChannel tLogicalChannel;
                    if (m_ptMtTable->GetMtLogicChnnl(tLocalMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
                    {
                        tLogicalChannel.SetFlowControl(GetDoubleStreamVideoBitrate(wSrcDialBitrate));
                        cTmpServMsg.SetMsgBody((u8 *)&tLogicalChannel, sizeof(tLogicalChannel));
                        SendMsgToMt(tLocalMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cTmpServMsg);
                    }                
                }
            }

			//对当前选看的目的终端，其先前的选看源不是目前的选看源，则先对先前的选看进行处理
			//(包含调整MT分辨率等)
			// [12/28/2009 xliang] 该处理务必要挪至"级联选看"处理前
			TMt tPreviousMt;	
			m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tDstMtStatus );
			tPreviousMt = tDstMtStatus.GetSelectMt(MODE_VIDEO);
			if( tPreviousMt.GetType() == TYPE_MT && !(tPreviousMt == tSrcMt) 
				&& (tSwitchInfo.GetMode() == MODE_VIDEO || tSwitchInfo.GetMode() == MODE_BOTH)
				)
			{
				//调整分辨率
				if( ChgMtVidFormatRequired(tPreviousMt) )
				{
					//tPreviousMt = GetLocalMtFromOtherMcuMt( tPreviousMt);
					ChangeMtVideoFormat(tPreviousMt);
				}
				
				// 重新调整选看源的发送码率
				TMtStatus tSrcStatus;
				GetMtStatus(tPreviousMt, tSrcStatus);
				if ( m_tConfAllMtInfo.MtJoinedConf( tPreviousMt ) 
					&& tSrcStatus.IsSendVideo() 
					)
				{
					NotifyMtSend( tPreviousMt.GetMtId(), MODE_VIDEO );
				}
				//StopSelectSrc(tDstMt, MODE_VIDEO, FALSE);
			}

			//  [11/17/2009 pengjie] 级联多回传判断
			//zjj20100201 多回传代码合并
			if( !tSrcMt.IsLocal() )
			{
				if( IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuId()) )
				{
					TPreSetInReq tSpySrcInitInfo;
					tSpySrcInitInfo.m_tSpyMtInfo.SetSpyMt(tSrcMt);
					tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.m_tDstMt = tDstMt;
					tSpySrcInitInfo.m_bySpyMode = tSwitchInfo.GetMode();
					tSpySrcInitInfo.SetEvId(cServMsg.GetEventId());
					TMcsRegInfo tMcRegInfo;
					if (g_cMcuVcApp.GetMcsRegInfo(byMcsId, &tMcRegInfo))
					{
						tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.SetMcIp(tMcRegInfo.GetMcsIpAddr());
						tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.SetMcSSRC(tMcRegInfo.GetMcsSSRC());
					}				
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcStartSelSwitchMt] McsIP.%s, McSSrc.%x!\n", 
												 StrOfIP(tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.GetMcIp()),
												 tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.GetMcSSRC());

					// [pengjie 2010/8/25] 上级同一终端选看下级不同终端替换支持
					TMtStatus tMtStatus;
					TMt tMtVidSelSrc;
					TMt tMtAudSelSrc;
					m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tMtStatus );
					tMtVidSelSrc = tMtStatus.GetSelectMt( MODE_VIDEO );
					tMtAudSelSrc = tMtStatus.GetSelectMt( MODE_AUDIO );
// 					if( tMtVidSelSrc == tMtAudSelSrc && !( tMtVidSelSrc == tSrcMt ) )
// 					{
// 						tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMtVidSelSrc;
// 						tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_BOTH;
// 						tSpySrcInitInfo.m_tReleaseMtInfo.m_swCount = 0;
// 						tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
// 				        tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
// 					}
					// End

					// [pengjie 2010/9/13] 填目的端能力
					TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()));	
					//zjl20101116 如果当前终端已回传则能力集要与已回传目的能力集取小
					TSimCapSet tDstCapSet = m_ptMtTable->GetDstSCS(tDstMt.GetMtId());
					tSimCapSet.SetVideoMaxBitRate(tDstCapSet.GetVideoMaxBitRate());
					if(!GetMinSpyDstCapSet(tSrcMt, tSimCapSet))
					{
						ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcStartSelSwitchMt] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
							tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
						return;
					}
					
					tSpySrcInitInfo.m_tSpyMtInfo.SetSimCapset( tSimCapSet );
		            // End

					//[nizhijun 2011/01/15]级联多回传选看替换支持修改						
					//1.原来的选看源：aud和vid不同，
					if ( !tMtVidSelSrc.IsNull() && !tMtAudSelSrc.IsNull() && !(tMtVidSelSrc==tMtAudSelSrc))
					{
						//1.1选看新终端的音视频，需要发两条presetin消息
						if (MODE_BOTH == tSwitchInfo.GetMode())
						{
							tSpySrcInitInfo.m_bySpyMode = MODE_VIDEO;													
							tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMtVidSelSrc;
							tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_VIDEO;
							tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
							tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
							TPreSetInReq tTempPreSetInReq = tSpySrcInitInfo;
							OnMMcuPreSetIn( tTempPreSetInReq );

							tSpySrcInitInfo.m_bySpyMode = MODE_AUDIO;						
							tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMtAudSelSrc;
							tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_AUDIO;
							tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
							tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
							OnMMcuPreSetIn( tSpySrcInitInfo );
						}
						//1.2 单一选看vid或aud则按照选看模式来发一条presetin消息
						else
						{
							if (MODE_VIDEO == tSwitchInfo.GetMode())
							{
								tSpySrcInitInfo.m_bySpyMode = MODE_VIDEO;
								if (!tMtVidSelSrc.IsNull())
								{
									tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMtVidSelSrc;
									tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_VIDEO;
									tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
									tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
								}
						    	OnMMcuPreSetIn( tSpySrcInitInfo );
							}
							else if (MODE_AUDIO == tSwitchInfo.GetMode())
							{
								tSpySrcInitInfo.m_bySpyMode = MODE_AUDIO;
								if (!tMtAudSelSrc.IsNull())
								{
									tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMtAudSelSrc;
									tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_AUDIO;
									tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
									tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
								}
						    	OnMMcuPreSetIn( tSpySrcInitInfo );
							}					
						}
					}
					//2.原来的选看源：aud和vid相同或者两者只存在一者,只发一条presetin消息
					else
					{
						tSpySrcInitInfo.m_bySpyMode = tSwitchInfo.GetMode();
						//对于新选看音视频的情况，目前还是发一条消息，releaseMT还是填旧的选看VID源
						if ( MODE_BOTH == tSwitchInfo.GetMode() )
						{							
							if (!tMtVidSelSrc.IsNull() && !tMtAudSelSrc.IsNull() )
							{
								tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMtVidSelSrc;
								tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_BOTH;
								tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
								tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
								tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
							}
							else
							{
								if (!tMtVidSelSrc.IsNull())
								{
									tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMtVidSelSrc;
									tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_VIDEO;
									tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
									tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
								}								
								else if (!tMtAudSelSrc.IsNull())
								{
									tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMtAudSelSrc;
									tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_AUDIO;
									tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
									tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
								}
							}
							OnMMcuPreSetIn( tSpySrcInitInfo );
						}
						else if ( MODE_VIDEO == tSwitchInfo.GetMode())
						{							
							if (!tMtVidSelSrc.IsNull())
							{
								tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMtVidSelSrc;
								tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_VIDEO;
								tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
								tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 1;
							}
							OnMMcuPreSetIn( tSpySrcInitInfo );
						}
						else if (MODE_AUDIO == tSwitchInfo.GetMode())
						{
							if (!tMtAudSelSrc.IsNull())
							{
								tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMtAudSelSrc;
								tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode	= MODE_AUDIO;
								tSpySrcInitInfo.m_tReleaseMtInfo.SetCount(0);
								tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
							}
							OnMMcuPreSetIn( tSpySrcInitInfo );
						}				
					}
					// End
					//OnMMcuPreSetIn( tSpySrcInitInfo );
									
					if ( MT_MCU_STARTSELMT_CMD != cServMsg.GetEventId() )
					{
						SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
					}  
					return;
				}
			}
			//zjj20100201 多回传代码合并 之后得代码可能要移植
			
			//[2011/10/14]
			tOldSelVid = tDstMtStatus.GetSelectMt(MODE_VIDEO);
			tOldSelAdu = tDstMtStatus.GetSelectMt(MODE_AUDIO);
			tOldSelSecVid = m_ptMtTable->GetMtSelMtByMode(tDstMt.GetMtId(), MODE_VIDEO2SECOND);

			/* xliang [4/2/2009] 取消主席选看VMP的状态, 在stopSelectSrc中进行
			if(tDstMt == m_tConf.GetChairman() 
				&& m_tConf.m_tStatus.IsVmpSeeByChairman() )
			{
				StopSelectSrc(m_tConf.GetChairman(), MODE_VIDEO);
			}
			else*/
			{
				StopSelectSrc( tDstMt,tSwitchInfo.GetMode(),FALSE,FALSE );
			}

			//级联选看			
            if ( !tSrcMt.IsLocal() )
            {
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "[ProcStartSelSwitchMt]unlocal Mt(%d,%d) start setin!\n", tSrcMt.GetMcuId(), tSrcMt.GetMtId());
                //查看MC
                TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tSrcMt.GetMcuId());
                if(ptMcInfo == NULL)
                {
                    cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    return;
                }
                //构造选看源
                TMt tMt;
                tMt.SetMcuIdx(tSrcMt.GetMcuId());
                tMt.SetMtId(0);
                TMcMtStatus *ptStatus = ptMcInfo->GetMtStatus(tMt);
                if(ptStatus == NULL)
                {
                    cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    return;
                }

                OnMMcuSetIn( tSrcMt, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
            }
			
            // 顾振华@2006.4.17 如果有选看(音频或者视频)，都要记录下来
            if(MODE_AUDIO == tSwitchInfo.GetMode() 
				|| MODE_VIDEO == tSwitchInfo.GetMode() 
				|| MODE_BOTH  == tSwitchInfo.GetMode() 
				|| MODE_VIDEO2SECOND  == tSwitchInfo.GetMode()
			  )
            {
				m_ptMtTable->SetMtSelMtByMode(tDstMt.GetMtId(), tSwitchInfo.GetMode(), tSrcMt);
            }
        
            if ( MT_MCU_STARTSELMT_CMD != cServMsg.GetEventId() )
            {
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
            }  
			else
			{
				//通知终端
				ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "Send MCU_MT_STARTSELMT_ACK to mt.%d\n",tSwitchInfo.GetDstMt().GetMtId());
				SendMsgToMt(tSwitchInfo.GetDstMt().GetMtId(), MCU_MT_STARTSELMT_ACK, cServMsg);
			}

            //上级MCU
            if( cServMsg.GetEventId() == MCS_MCU_STARTSWITCHMT_REQ &&
                (tDstMt.GetMtType() == MT_TYPE_SMCU || tDstMt.GetMtType() == MT_TYPE_MMCU) )
            {
                TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(tDstMt.GetMtId()));
                if(ptConfMcInfo != NULL)
                {
                    ptConfMcInfo->m_tSpyMt = tLocalMt;
                    //send output 消息
                    TSetOutParam tOutParam;
					TMsgHeadMsg tHeadMsg;
					memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );					
                    tOutParam.m_nMtCount = 1;
                    tOutParam.m_atConfViewOutInfo[0].m_tMt = tDstMt;
                    tOutParam.m_atConfViewOutInfo[0].m_nOutViewID = ptConfMcInfo->m_dwSpyViewId;
                    tOutParam.m_atConfViewOutInfo[0].m_nOutVideoSchemeID = ptConfMcInfo->m_dwSpyVideoId;
                    CServMsg cServMsg2;
					//tHeadMsg.m_tMsgSrc.m_tMt = tDstMt;
					cServMsg2.SetMsgBody((u8 *)&tOutParam, sizeof(tOutParam));
					cServMsg2.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
                    
                    cServMsg2.SetEventId(MCU_MCU_SETOUT_NOTIF);
                    SendMsgToMt(tDstMt.GetMtId(), MCU_MCU_SETOUT_NOTIF, cServMsg2);
                }
            }
            
            //for h320 mcu cascade select
            if( MT_MCU_STARTSELMT_REQ == cServMsg.GetEventId() && 
                MT_TYPE_SMCU == m_ptMtTable->GetMtType(tDstMt.GetMtId()) )
            {            
                m_tLastSpyMt = tLocalMt;
            } 
			
			//若被选看的MT在画面合成中，则去尝试占VMP高清前适配通道
			if(MODE_VIDEO2SECOND != tSwitchInfo.GetMode() && ChgMtVidFormatRequired(tSrcMt))
			{ 
				ChangeMtVideoFormat(tSrcMt, FALSE); 
			}
            // xsl [7/21/2006] 通知终端接收地址
            /*if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
                if (MODE_AUDIO == tSwitchInfo.GetMode() || MODE_BOTH == tSwitchInfo.GetMode())
                {
                    ChangeSatDConfMtRcvAddr(tDstMt.GetMtId(), LOGCHL_AUDIO, FALSE);
                }

                if (MODE_VIDEO == tSwitchInfo.GetMode() || MODE_BOTH == tSwitchInfo.GetMode())
                {
                    ChangeSatDConfMtRcvAddr(tDstMt.GetMtId(), LOGCHL_VIDEO, FALSE);
                }                
            }*/

			//双选看处理：目的终端双流通道未打开先通道，否则调一个源的码率
            if (MODE_VIDEO2SECOND == tSwitchInfo.GetMode())
			{
				//通道未打开先开通道
				TLogicalChannel tLogicalChannel;
				if (!m_ptMtTable->GetMtLogicChnnl(tDstMt.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, TRUE))
				{
					//取选看源主视频能力开目的终端的双流通道
					if (m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
					{
						//设置通道是否带有H239标签
						TCapSupport tCapSupport;
						if (m_ptMtTable->GetMtCapSupport(tDstMt.GetMtId(), &tCapSupport))
						{
							tLogicalChannel.SetSupportH239(tCapSupport.IsDStreamSupportH239());
						}
						else
						{
							tLogicalChannel.SetSupportH239(m_tConf.GetCapSupport().IsDStreamSupportH239());
						}
						
						McuMtOpenDoubleStreamChnnl(tDstMt, tLogicalChannel);
					}
					return;
				}
				
				//调第一个源的码率，主流
				BOOL32 bIsBrdNeedAdp = FALSE;
				if (!GetVidBrdSrc().IsNull() && GetVidBrdSrc().GetType() == TYPE_MT)
				{
					bIsBrdNeedAdp = IsNeedAdapt(GetVidBrdSrc(), tDstMt, MODE_VIDEO);
				}
				AdjustFitstSrcMtBit(tDstMt.GetMtId(), bIsBrdNeedAdp);
            }
        }            
		break;
        
    case MCS_MCU_STARTSWITCHMC_REQ:
        {
            // xsl [7/20/2006] 卫星分散会议时需要判断回传通道数
           /* if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
                if (IsOverSatCastChnnlNum(tSrcMt))
                {
                    ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMtMcuStartSwitchMtReq] over max upload mt num. nack!\n");
                    cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    return;
                }
            }*/

            //加密以及动态载荷信息
            TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
            TSimCapSet    tSrcSCS  = m_ptMtTable->GetSrcSCS( tLocalMt.GetMtId() ); 
            TDoublePayload tDVPayload;
            TDoublePayload tDAPayload;
			// 选看画面合成时,用会议主格式能力
			if (tSrcMt.GetEqpType() == EQP_TYPE_VMP)
			{
				tSrcSCS  = m_tConf.GetMainSimCapSet(); 
				tSrcSCS.SetAudioMediaType(MEDIA_TYPE_NULL);
			}
            
            if( MEDIA_TYPE_H264 == tSrcSCS.GetVideoMediaType() || 
                MEDIA_TYPE_H263PLUS == tSrcSCS.GetVideoMediaType() || 
                CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] 适应FEC支持其他格式
                m_tConf.GetCapSupportEx().IsVideoSupportFEC())
            {
                tDVPayload.SetRealPayLoad( tSrcSCS.GetVideoMediaType() );
                tDVPayload.SetActivePayload( GetActivePayload(m_tConf, tSrcSCS.GetVideoMediaType() ) );
            }
            else
            {
                tDVPayload.SetRealPayLoad( tSrcSCS.GetVideoMediaType() );
                tDVPayload.SetActivePayload( tSrcSCS.GetVideoMediaType() );
            }
            if( CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] 适应FEC支持其他格式
                m_tConf.GetCapSupportEx().IsAudioSupportFEC() )
            {
                tDAPayload.SetRealPayLoad( tSrcSCS.GetAudioMediaType() );
                tDAPayload.SetActivePayload( GetActivePayload(m_tConf, tSrcSCS.GetAudioMediaType() ) );
            }
            else
            {
                tDAPayload.SetRealPayLoad( tSrcSCS.GetAudioMediaType() );
                tDAPayload.SetActivePayload( tSrcSCS.GetAudioMediaType() );
            }
            
            cMsg.SetMsgBody(cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
            cMsg.CatMsgBody( (u8 *)&tEncrypt, sizeof(tEncrypt));
            cMsg.CatMsgBody( (u8 *)&tDVPayload, sizeof(tDVPayload));
            cMsg.CatMsgBody( (u8 *)&tEncrypt, sizeof(tEncrypt));
            cMsg.CatMsgBody( (u8 *)&tDAPayload, sizeof(tDAPayload));

            // zw [06/26/2008] 适应AAC LC格式
			if ( MEDIA_TYPE_AACLC == tSrcSCS.GetAudioMediaType() )
			{
                TAudAACCap tAudAACCap;
                tAudAACCap.SetMediaType(MEDIA_TYPE_AACLC);
                tAudAACCap.SetSampleFreq(AAC_SAMPLE_FRQ_32);
				TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
                tAudAACCap.SetChnlType(GetAACChnlTypeByAudioTrackNum(tAudioType.GetAudioTrackNum()));
                tAudAACCap.SetBitrate(96);
                tAudAACCap.SetMaxFrameNum(AAC_MAX_FRM_NUM);
                
				cMsg.CatMsgBody( (u8 *)&tAudAACCap, sizeof(tAudAACCap) );
			}
			//tianzhiyong 2010/04/02 适应AAC LD格式
			if ( MEDIA_TYPE_AACLD == tSrcSCS.GetAudioMediaType() )
			{
                TAudAACCap tAudAACCap;
                tAudAACCap.SetMediaType(MEDIA_TYPE_AACLD);
                tAudAACCap.SetSampleFreq(AAC_SAMPLE_FRQ_32);
				TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
                tAudAACCap.SetChnlType(GetAACChnlTypeByAudioTrackNum(tAudioType.GetAudioTrackNum()));
                tAudAACCap.SetBitrate(96);
                tAudAACCap.SetMaxFrameNum(AAC_MAX_FRM_NUM);
                
				cMsg.CatMsgBody( (u8 *)&tAudAACCap, sizeof(tAudAACCap) );
			}

            SendReplyBack( cMsg, cMsg.GetEventId() + 1 );
			
			// [9/13/2010 xliang] if UI ask the same Src in the same MC channel, MCU should do nothing.
// 			TMt tMcAudCurSrc;
// 			TMt tMcVidCurSrc;
// 			g_cMcuVcApp.GetMcSrc( byMcsId, &tMcAudCurSrc, tSwitchInfo.GetDstChlIdx(), MODE_AUDIO );
// 			g_cMcuVcApp.GetMcSrc( byMcsId, &tMcVidCurSrc, tSwitchInfo.GetDstChlIdx(), MODE_VIDEO );
			//zjj20101119补充具体不同音视频模式
			if( MODE_VIDEO == bySwitchMode &&  tMcVidCurSrc == tSrcMt )
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "Mt(%d,%d) video already in Mc channel.%d, Do nothing!\n", tSrcMt.GetMcuId(), tSrcMt.GetMtId(), tSwitchInfo.GetDstChlIdx());
				return;
			}
			if( MODE_AUDIO == bySwitchMode && tMcAudCurSrc == tMcVidCurSrc )
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "Mt(%d,%d) audio already in Mc channel.%d, Do nothing!\n", tSrcMt.GetMcuId(), tSrcMt.GetMtId(), tSwitchInfo.GetDstChlIdx());
				return;
			}
			if( MODE_BOTH == bySwitchMode && tMcVidCurSrc == tSrcMt && tMcAudCurSrc == tMcVidCurSrc )
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "Mt(%d,%d) video and audio already in Mc channel.%d, Do nothing!\n", tSrcMt.GetMcuId(), tSrcMt.GetMtId(), tSwitchInfo.GetDstChlIdx());
				return;
			}

			// [1/27/2011 xliang] further filter mode
			if( MODE_BOTH == bySwitchMode && tMcVidCurSrc == tSrcMt && (!(tMcAudCurSrc == tSrcMt)) )
			{
				bySwitchMode = MODE_AUDIO;
				
			}
			else if( MODE_BOTH == bySwitchMode && (!(tMcVidCurSrc == tSrcMt)) && tMcAudCurSrc == tSrcMt )
			{
				bySwitchMode = MODE_VIDEO;
			}
			tSwitchInfo.SetMode(bySwitchMode);
			
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcStartSelSwitchMt] Switch Mt(%d.%d) To Mc(%d) Channel(%d)	Mode:(%d)\n",
				tSrcMt.GetMcuId(),tSrcMt.GetMtId(),
				cServMsg.GetSrcSsnId(),tSwitchInfo.GetDstChlIdx(),
				tSwitchInfo.GetMode()
				);
			
			// 源是画面合成时
			if (tSrcMt.GetEqpType() == EQP_TYPE_VMP)
			{
				// vmp只有视频没有音频,需单独停掉原有音频
				StopSwitchToMc( cServMsg.GetSrcSsnId(), tSwitchInfo.GetDstChlIdx(), FALSE, MODE_AUDIO );

				u8 bySrcChnnl = GetVmpOutChnnlByRes(tSrcMt.GetEqpId(),
					tSrcSCS.GetVideoResolution(),
					tSrcSCS.GetVideoMediaType(),
					tSrcSCS.GetUserDefFrameRate(),
					tSrcSCS.GetVideoProfileType());
				if ( 0xFF == bySrcChnnl ) //没找到对应的通道
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ProcStartSelSwitchMt] Get MCS vmp outchannl failed \n");
					return;
				}
				
				// 8kh有特殊场景
				if (VMP_8KH == GetVmpSubType(tSrcMt.GetEqpId()))
				{
					// 会议是1080p30fpsBP适配会议或720p30fpsHP适配会议时,为避免vmp广播非广播切换时通道改变,取1通道
					if (IsConfExcludeDDSUseAdapt())
					{
						if (VIDEO_FORMAT_HD1080 == tSrcSCS.GetVideoResolution() && 
							tSrcSCS.GetUserDefFrameRate() < 50 &&
							tSrcSCS.GetVideoProfileType() == emBpAttrb)
						{
							if (IsMSSupportCapEx(VIDEO_FORMAT_HD720))
							{
								bySrcChnnl = 1;
							}
							else if (IsMSSupportCapEx(VIDEO_FORMAT_4CIF))
							{
								bySrcChnnl = 2;
							}
							else if (IsMSSupportCapEx(VIDEO_FORMAT_CIF))
							{
								bySrcChnnl = 3;
							}
						}
						if (VIDEO_FORMAT_HD720 == tSrcSCS.GetVideoResolution() && 
							tSrcSCS.GetUserDefFrameRate() < 50 &&
							tSrcSCS.GetVideoProfileType() == emHpAttrb)
						{
							if (IsMSSupportCapEx(VIDEO_FORMAT_HD720, tSrcSCS.GetUserDefFrameRate(), emBpAttrb))
							{
								bySrcChnnl = 1;
							}
							else if (IsMSSupportCapEx(VIDEO_FORMAT_4CIF))
							{
								bySrcChnnl = 2;
							}
							else if (IsMSSupportCapEx(VIDEO_FORMAT_CIF))
							{
								bySrcChnnl = 3;
							}
						}
					}
				}
				// 8ki有特殊场景
				else if (VMP_8KI == GetVmpSubType(tSrcMt.GetEqpId()))
				{
					// 会议是1080p30fps或720p60fps适配会议时,为避免vmp广播非广播切换时通道改变,取1通道
					if (IsConfExcludeDDSUseAdapt()) //适配会议
					{
						if ((m_tConf.GetMainVideoFormat() == VIDEO_FORMAT_HD1080 && m_tConf.GetMainVidUsrDefFPS() <= 30) ||
							(m_tConf.GetMainVideoFormat() == VIDEO_FORMAT_HD720 && m_tConf.GetMainVidUsrDefFPS() > 30))
						{
							TKDVVMPOutParam tVMPOutParam = g_cMcuVcApp.GetVMPOutParam(tSrcMt);
							TVideoStreamCap tTmpStrCap = tVMPOutParam.GetVmpOutCapIdx(1);
							// 720/4cif/cif都未勾选时,是不允许非广播的,仍取第0路
							if (MEDIA_TYPE_H264 == tTmpStrCap.GetMediaType())
							{
								bySrcChnnl = 1;
							}
							else
							{
								bySrcChnnl = 0;
							}
						}
					}
				}
				//更新源chnnl
				tSwitchInfo.SetSrcChlIdx(bySrcChnnl);
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[MCSSeeVmp] VMP --> MCS.%u, SrcChnnlIdx is %u\n",byMcsId, bySrcChnnl);
			}

			//  [11/17/2009 pengjie] 级联多回传判断
			if( !tSrcMt.IsLocal() )
			{
				if( IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuId()) )
				{
					TPreSetInReq tSpySrcInitInfo;
					tSpySrcInitInfo.m_tSpyMtInfo.SetSpyMt(tSrcMt);
					tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.m_tDstMt = tDstMt;
					tSpySrcInitInfo.m_bySpyMode = bySwitchMode;
					tSpySrcInitInfo.SetEvId(cServMsg.GetEventId());
					tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.m_byDstChlIdx = tSwitchInfo.GetDstChlIdx();
					tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.m_bySrcChlIdx = tSwitchInfo.GetSrcChlIdx();
					TMcsRegInfo tMcRegInfo;
					if (g_cMcuVcApp.GetMcsRegInfo(cServMsg.GetSrcSsnId(), &tMcRegInfo))
					{
						tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.SetMcIp(tMcRegInfo.GetMcsIpAddr());
						tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.SetMcSSRC(tMcRegInfo.GetMcsSSRC());
					}
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcStartSelSwitchMt:2] McsIP.%s, McSSrc.%x!\n",
												 StrOfIP(tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.GetMcIp()),
												 tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.GetMcSSRC());
					

					// [pengjie 2010/9/13] 填目的端能力
					TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()));
					//zjl20101116 如果当前终端已回传则能力集要与已回传目的能力集取小
					if(!GetMinSpyDstCapSet(tSrcMt, tSimCapSet))
					{
						ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcStartSelSwitchMt] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
							tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
						return;
					}
					
					tSpySrcInitInfo.m_tSpyMtInfo.SetSimCapset( tSimCapSet );
		            // End

				    //zhouyiliang 20100814 将上次在这个通道的终端设成可替换终端
					TMt tMcSrc;
					tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode = MODE_NONE;
					TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);//此数据仅用于VCS
		
					g_cMcuVcApp.GetMcSrc( byMcsId, &tMcSrc, tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.m_byDstChlIdx, MODE_VIDEO );
					if( !tMcSrc.IsNull() )
					{
						if( tMcSrc == tSrcMt )
						{
							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "StartSwitchToMc GetMcSrc == NewMt, MODE_VIDEO\n" );
						}
						else
						{
							//[2011/11/24/zhangli]处理多个vcs连同一mcu的情况，带宽不足或者适配等资源不足时对可释放基数比较敏感，
							//适配不足时在ProcMcuMcuPreSetInAck里先把监控停掉了（此操作可以去掉），这里依然加上正确的基数
							//单通道监控联动业务也走这里的逻辑，但是监控联动业务每个VCS可独立操作，故不能累加基数
							u8 byMtInMcNum = 1;
							if( VCS_CONF == m_tConf.GetConfSource() )
							{
								byMtInMcNum = GetCanRlsNumInVcsMc(tMcSrc, MODE_VIDEO);
							}
							
							tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode += MODE_VIDEO;
							tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMcSrc;
							tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = byMtInMcNum;
							if( ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() ||
									ROLLCALL_MODE_CALLER == m_tConf.m_tStatus.GetRollCallMode() ) &&
									m_tRollCallee == tSpySrcInitInfo.m_tSpyMtInfo.GetSpyMt() )
							{								
								u8 byChlPos = tConfVmpParam.GetChlOfMtInMember(tMcSrc);
								if(	tConfVmpParam.GetVMPMode() == CONF_VMPMODE_CTRL  &&
									MAXNUM_VMP_MEMBER != byChlPos )
								{
									++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
								}
								TMtStatus tStatus;
								m_ptMtTable->GetMtStatus( m_tRollCaller.GetMtId(),&tStatus );
								if( !tStatus.GetSelectMt(MODE_VIDEO).IsNull() )
								{
									if( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
									{
										++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
									}
									//zjj20101203 由于时序问题,点名人模式下,非直属下级的被点名人回传协商时要加1路可释放视频,是给点名人看的视频
									if( ROLLCALL_MODE_CALLER == m_tConf.m_tStatus.GetRollCallMode() &&
										!m_tConfAllMcuInfo.IsSMcuByMcuIdx(m_tRollCallee.GetMcuId()) 
										)
									{
										++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
									}
								}
							}
							
							//zjj20101203 由于时序问题,被点名人模式下,非直属下级的被点名人回传协商时要加1路可释放视频,加1路可释放音频,因为前一个被点名人是发言人
							if( ROLLCALL_MODE_CALLEE == m_tConf.m_tStatus.GetRollCallMode() &&
								m_tRollCallee == tSpySrcInitInfo.m_tSpyMtInfo.GetSpyMt() &&
								!m_tConfAllMcuInfo.IsSMcuByMcuIdx(m_tRollCallee.GetMcuId()) )
							{
								tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode += MODE_AUDIO;
								++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
								++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
							}
						}
					}
		
					g_cMcuVcApp.GetMcSrc( byMcsId, &tMcSrc, tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.m_byDstChlIdx, MODE_AUDIO );
					if( !tMcSrc.IsNull() )
					{
						if( tMcSrc == tSrcMt )
						{
							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "StartSwitchToMc GetMcSrc == NewMt, MODE_AUDIO !\n" );
						}
						else
						{
							if( tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode == MODE_VIDEO ||
								tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode == MODE_NONE)
							{
								tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode += MODE_AUDIO;
							}		
							u8 byMtInMcNum = 1;
							if( VCS_CONF == m_tConf.GetConfSource() )
							{
								byMtInMcNum = GetCanRlsNumInVcsMc(tMcSrc, MODE_AUDIO);
							}
							
							tSpySrcInitInfo.m_tReleaseMtInfo.m_tMt = tMcSrc;
							tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum += byMtInMcNum;
						}
					}
					
					// [pengjie 2011/1/15] 在vcs组呼会议，点名双画面模式下释放视频回传计数加一(这一路是释放到监控窗口的)
					if ( m_tConf.GetConfSource() == VCS_CONF && tConfVmpParam.IsVMPAuto() &&
						m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPROLLCALL_MODE &&
						VCS_VCMT_MCCHLIDX == tSpySrcInitInfo.m_tSpyInfo.m_tSpySwitchInfo.m_byDstChlIdx )  
					{
						++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
					}
			
					//zhouyiliang 20101015 点名双画面的情况，被主席选看终端，还要再加1
					if ( m_tConf.GetConfSource() == VCS_CONF && m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPROLLCALL_MODE ) 
					{
						//判断主席选看的终端是否是可替换终端
						TMt tChairman = m_tConf.GetChairman();
						if ( !tChairman.IsNull() ) 
						{
							TMtStatus tStat;
							m_ptMtTable->GetMtStatus(tChairman.GetMtId(), &tStat );
							if (tStat.GetSelectMt( MODE_VIDEO ) == tMcSrc  ) 
							{
								++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
							}
							if (tStat.GetSelectMt( MODE_AUDIO ) == tMcSrc  ) 
							{
								++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
							}

						}

						
					}
					
					/*if (MODE_NONE != byReleaseMode  ) 
					{
						tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode = byReleaseMode;
						if( MODE_BOTH == tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode )
						{
							++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 
							++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
						}
						else if( MODE_VIDEO == tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode )
						{
							++tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
						}
						else if( MODE_AUDIO == tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseMode )
						{
							tSpySrcInitInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
						}
				
						
													
					}*/
					OnMMcuPreSetIn( tSpySrcInitInfo );
					return;
				}
				//2010/05/12 lukunpeng 在非多回传情况下，也需要发送setin，通知下级mcu将此终端进上传通道
				else
				{
				
					//zhouyiliang 20110225 当下级有当前调度终端，且同一个下级的其它终端被选看，回NACK
					if( m_tConf.GetConfSource() == VCS_CONF && !m_cVCSConfStatus.GetCurVCMT().IsNull()
						&&  IsMtInMcu(GetLocalMtFromOtherMcuMt(tSrcMt), m_cVCSConfStatus.GetCurVCMT()) 
						&& !( m_cVCSConfStatus.GetCurVCMT() == tSrcMt ))
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[ProcVcsMcuMtInTwMsg] CurVCMT(%d.%d) and Mt(%d.%d) is in same smcu,Mt can't be selected \n ",
											m_cVCSConfStatus.GetCurVCMT().GetMcuId(),
											m_cVCSConfStatus.GetCurVCMT().GetMtId(),
											tSrcMt.GetMcuId(),
											tSrcMt.GetMtId() );

						cServMsg.SetErrorCode( ERR_MCU_VCS_NOUSABLEBACKCHNNL );
						SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
						return;
					}
					//查看MC
					TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tSrcMt.GetMcuId());
					if(ptMcInfo == NULL)
					{
						cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
						SendReplyBack( cMsg, cMsg.GetEventId() + 2 );
						return;
					}
					//构造选看源
					TMt tMt;
					tMt.SetMcuIdx(tSrcMt.GetMcuId());
					tMt.SetMtId(0);
					TMcMtStatus *ptStatus = ptMcInfo->GetMtStatus(tMt);
					if(ptStatus == NULL)
					{                    
						cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
						SendReplyBack( cMsg, cMsg.GetEventId() + 2 );
						return;
					}
					
					// 如果该成员已是smcu的上传通道终端,无需再次发送SetIn消息
					TMt tMcu = GetLocalMtFromOtherMcuMt(tSrcMt);
					if (!(tSrcMt == GetSMcuViewMt(tMcu, TRUE)))
					{
						OnMMcuSetIn( tSrcMt, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
					}
				}
			}
        }
        break;
        
    default:
        ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
                 cServMsg.GetEventId(), ::OspEventDesc( cServMsg.GetEventId() ), CurState() );
        break;    
    }
	
	//[nizhijun 2010/11/30]更新
	//[nizhijun 2010/11/17]tSrcMt == GetVidBrdSrc()，支持多回传情况下，选看下级MCU
    // 这里需要考虑是否是广播交换, zgc, 2008-06-03
    // BUG2354, 需要防止会控监控进入, zgc, 2008-06-12
	//zjj20101201选看重整，策略改变，这里先删除
    /*if( tSrcMt == GetVidBrdSrc() && cServMsg.GetEventId() != MCS_MCU_STARTSWITCHMC_REQ &&
        ( !( (tSrcMt==m_tRollCaller && GetLocalMtFromOtherMcuMt(tDstMt)==GetLocalMtFromOtherMcuMt(m_tRollCallee)) 
		|| (tSrcMt==GetLocalMtFromOtherMcuMt(m_tRollCallee) && tDstMt==m_tRollCaller) ) ) &&
        ( MODE_VIDEO == tSwitchInfo.GetMode() || MODE_BOTH == tSwitchInfo.GetMode() ) )
    {
        ConfLog( FALSE, "Dst(Mt.%d) receive Src(Mt.%d) as video broadcast src!\n", tDstMt.GetMtId(), tSrcMt.GetMtId() );
        
        StartSwitchFromBrd( tSrcMt, 0, 1, &tDstMt );
        
        if ( MODE_BOTH == tSwitchInfo.GetMode() )
        {
            tSwitchInfo.SetMode( MODE_AUDIO );
        }
        else
        {
            // 这里可以直接return，因为在这种情况下实际并没有发生选看
            return;
        }
	}*/

	//[2011/10/14/zhangli]
	//上级MT1选看下级MT1，然后上级MT2选看下级MT2，单回传下要求上级MT1和MT2都看下级MT2
	//修改终端的选看源，如果过适配，则改掉过适配占用的bas 的源
	//R3_Full等版本，在setin下发后，没ProcMcuMcuSetOutNotify 回来，因此要在这里处理
	if (!tSrcMt.IsLocal() && !IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuIdx()))
	{
		TMtStatus tCurrMtStatus;
		TMt tCurrMt;
		TMt tSelVidMt;
		TMt tSelAudMt;
		
		TBasOutInfo tOutInfo;

		for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
		{
			if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
			{
				continue;
			}
			tCurrMt = m_ptMtTable->GetMt(byMtId);
			m_ptMtTable->GetMtStatus(byMtId, &tCurrMtStatus);
			tSelVidMt = tCurrMtStatus.GetSelectMt(MODE_VIDEO);
			tSelAudMt = tCurrMtStatus.GetSelectMt(MODE_AUDIO);
			
			if (!tSelVidMt.IsNull() && tSrcMt.GetMcuId() == tSelVidMt.GetMcuId() && !(tSelVidMt == tSrcMt))
			{	
				if (IsNeedSelAdpt(tSelVidMt, tCurrMt, MODE_VIDEO))
				{
					if (FindBasChn2SelForMt(tSelVidMt, tCurrMt, MODE_VIDEO, tOutInfo))
					{
						SetBasChnSrc(tOutInfo.m_tBasEqp, tOutInfo.m_byChnId, tSrcMt);
					}
				}
				tCurrMtStatus.SetVideoMt(tSrcMt);
				tCurrMtStatus.SetSelectMt(tSrcMt, MODE_VIDEO);
				m_ptMtTable->SetMtStatus(byMtId, &tCurrMtStatus);
			}
			
			if (!tSelAudMt.IsNull() && tSrcMt.GetMcuId() == tSelAudMt.GetMcuId() && !(tSelAudMt == tSrcMt))
			{	
				if (IsNeedSelAdpt(tSelAudMt, tCurrMt, MODE_AUDIO))
				{
					if (FindBasChn2SelForMt(tSelVidMt, tCurrMt, MODE_AUDIO, tOutInfo))
					{
						SetBasChnSrc(tOutInfo.m_tBasEqp, tOutInfo.m_byChnId, tSrcMt);
					}
				}
				tCurrMtStatus.SetAudioMt(tSrcMt);
				tCurrMtStatus.SetSelectMt(tSrcMt, MODE_AUDIO);
				m_ptMtTable->SetMtStatus(byMtId, &tCurrMtStatus);
			}
		}
	}

	TSwitchInfo tTempSwitchInfo;
	if(MODE_BOTH == tSwitchInfo.GetMode() || MODE_VIDEO == tSwitchInfo.GetMode())
	{
		if (bVidAdp)
		{
			if(!StartSelAdapt(tSwitchInfo.GetSrcMt(), tSwitchInfo.GetDstMt(), MODE_VIDEO, bInnerSelect))
			{
				TMtStatus tMtStatus;
				m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tMtStatus );
				tMtStatus.SetSelectMt( tOldSelVid, MODE_VIDEO);
                m_ptMtTable->SetMtStatus( tDstMt.GetMtId(), &tMtStatus );
				/*20110610 zjl Bug00055391 如果前适配通道没满，那么上面会先占前适配通道，但视频选看如果由于适配不足导致选看失败，
						   而取消选看释放前适配通道又是依靠终端选看状态，所以这里如果占用适配失败要释放下占用的前适配通道
			   */
				ClearAdpChlInfoInAllVmp(tSwitchInfo.GetSrcMt());
				//m_tVmpChnnlInfo.ClearChnlByMt( tSwitchInfo.GetSrcMt() );

				if (!tOldSelVid.IsNull())
				{
					StopSelectSrc(tDstMt, MODE_VIDEO);
				}
			}
		}
		else
		{
			memcpy(&tTempSwitchInfo, &tSwitchInfo, sizeof(tTempSwitchInfo));
			tTempSwitchInfo.SetMode(MODE_VIDEO);
			SwitchSrcToDst(tTempSwitchInfo, cServMsg);
		}
	}
	
	if (MODE_BOTH == tSwitchInfo.GetMode() || MODE_AUDIO == tSwitchInfo.GetMode())
	{
		if (bAudAdp)
		{
			if(!StartSelAdapt(tSwitchInfo.GetSrcMt(), tSwitchInfo.GetDstMt(), MODE_AUDIO, bInnerSelect))
			{
				TMtStatus tMtStatus;
				m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tMtStatus );
				tMtStatus.SetSelectMt( tOldSelAdu, MODE_AUDIO);
                m_ptMtTable->SetMtStatus( tDstMt.GetMtId(), &tMtStatus );

				if (!tOldSelAdu.IsNull())
				{
					StopSelectSrc(tDstMt, MODE_AUDIO);
				}
			}
		}
		else
		{
			memcpy(&tTempSwitchInfo, &tSwitchInfo, sizeof(tTempSwitchInfo));
			tTempSwitchInfo.SetMode(MODE_AUDIO);
			SwitchSrcToDst(tTempSwitchInfo, cServMsg);
		}
	}

	if (MODE_SECVIDEO == tSwitchInfo.GetMode())
	{
		SwitchSrcToDst(tSwitchInfo, cServMsg);
	}

	if(MODE_VIDEO2SECOND == tSwitchInfo.GetMode())
	{
		if (bSecVidAdp)
		{
			if (!StartSelAdapt(tSwitchInfo.GetSrcMt(), tSwitchInfo.GetDstMt(), MODE_VIDEO2SECOND, bInnerSelect))
			{
				m_ptMtTable->SetMtSelMtByMode(tDstMt.GetMtId(), MODE_VIDEO2SECOND, tOldSelSecVid);
				
				if (!tOldSelSecVid.IsNull())
				{
					StopSelectSrc(tDstMt, MODE_VIDEO2SECOND);
				}
			}
		}
		else
		{
			memcpy(&tTempSwitchInfo, &tSwitchInfo, sizeof(tTempSwitchInfo));
			SwitchSrcToDst(tTempSwitchInfo, cServMsg);
		}
	}
	
// #ifdef _8KE_
// 	if (bSelAccordAdp)
// #else
//     if (bSelAccordAdp && g_cMcuVcApp.IsSelAccord2Adp())
// #endif
//     {
//         if (MODE_BOTH == tSwitchInfo.GetMode())
//         {
//             tSwitchInfo.SetMode(MODE_AUDIO);
//             SwitchSrcToDst(tSwitchInfo, cServMsg);
//             tSwitchInfo.SetMode(MODE_VIDEO);
//         }
//         SwitchSrcToDstAccord2Adp( tSwitchInfo, cServMsg );
//     }
//     else
//     {
// 	    SwitchSrcToDst( tSwitchInfo, cServMsg );
//     }

	
	// 设置发言人源为非会议自动指定, zgc, 2008-04-14
	if ( HasJoinedSpeaker() && tDstMt == GetLocalSpeaker() )
	{
		SetSpeakerSrcSpecType( tSwitchInfo.GetMode(), SPEAKER_SRC_MCSDRAGSEL );
	}
    
    return;
}

/*==============================================================================
函数名    :  ProcMcsMcuStartSwitchVmpMtReq
功能      :  处理主席终端选看VMP请求
算法实现  :  
参数说明  :  
返回值说明:  void
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-1-7					薛亮							创建
==============================================================================*/
void CMcuVcInst::ProcMcsMcuStartSwitchVmpMtReq(const CMessage * pcMsg)
{
	STATECHECK;
    
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt tDstMt;

	if ( cServMsg.GetEventId() == MT_MCU_STARTSWITCHVMPMT_REQ )
	{
		tDstMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	}
	else
	{
		tDstMt = *(TMt *)cServMsg.GetMsgBody();
	}
	
	// 支持非主席终端选看vmp
	TMtSeeVmp(cServMsg);
	/*if( HasJoinedChairman() && m_tConf.GetChairman().GetMtId() == tDstMt.GetMtId() )
	{
		ChairmanSeeVmp(cServMsg);
	}
	else
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_MCS, "[ProcMcsMcuStartSwitchVmpMtReq] tDstMt.%d isn't ConfChairman.%d!\n", 
			tDstMt.GetMtId(), m_tConf.GetChairman().GetMtId() );
	}*/
	
	return;
}

/*====================================================================
    函数名      ：GetMtNumSeeVmp
    功能        ：获得选看vmp指定通道终端的个数
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byChnnl [in]		指定通道
				  u8 byNeglectMtId [in]	忽略终端
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	5/09/2012 	4.7			杨怀志        创建
====================================================================*/
u8 CMcuVcInst::GetMtNumSeeVmp(u8 byVmpId, u8 byChnnl, u8 byNeglectMtId)
{
	u8 byMtNum = 0;
	if (byChnnl >= MAXNUM_MPU2_OUTCHNNL)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[GetMtNumSeeVmp] The byChnnl(%d) is wrong!\n", byChnnl);
		return byMtNum;
	}
	
	TLogicalChannel tFwLogicChnnl;
	TMtStatus tMtStatus;
	TMt tSelectSrc;
	for ( u8 byMtLoop = 1; byMtLoop <= MAXNUM_CONF_MT; byMtLoop ++)
	{
		if (m_tConfAllMtInfo.MtJoinedConf(byMtLoop) && byNeglectMtId != byMtLoop)
        {
			tFwLogicChnnl.Clear();
			tMtStatus.Clear();
			if (!m_ptMtTable->GetMtLogicChnnl( byMtLoop,LOGCHL_VIDEO,&tFwLogicChnnl,TRUE ) )
			{
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP,"[GetMtNumSeeVmp]Get DstMt：%d forward logicalchnnl failed \n",byMtLoop );
				continue;
			}
			m_ptMtTable->GetMtStatus(byMtLoop, &tMtStatus);
			tSelectSrc = tMtStatus.GetSelectMt(MODE_VIDEO);
			// 选看源不是外设vmp时,跳过统计
			if (!(TYPE_MCUPERI == tSelectSrc.GetType() && EQP_TYPE_VMP == tSelectSrc.GetEqpType()))
			{
				continue;
			}
			// 选看源不是指定vmp时,跳过统计
			if (tSelectSrc.GetEqpId() != byVmpId)
			{
				continue;
			}

			u8 byMtRecVmpChnnl  = GetVmpOutChnnlByRes(byVmpId,
				tFwLogicChnnl.GetVideoFormat(),
				tFwLogicChnnl.GetChannelType(),
				tFwLogicChnnl.GetChanVidFPS(), 
				tFwLogicChnnl.GetProfileAttrb());
			
            if (byMtRecVmpChnnl != byChnnl )
			{
				continue;
			}

			// 计数+1
			byMtNum++;
		}
	}

	return byMtNum;
}

/*====================================================================
    函数名      ：TMtSeeVmp
    功能        ：终端选看vmp建交换，置状态的操作,包括主席与非主席
    算法实现    ：
    引用全局变量：
    输入参数说明：CServMsg &cServMsg
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	4/14/2009 	4.6			薛亮          创建
	5/09/2012 	4.7			杨怀志        普通终端选看vmp支持
====================================================================*/
void CMcuVcInst::TMtSeeVmp(CServMsg &cServMsg, BOOL32 bNeglectVmpMode /* = FALSE */)
{
	/*非主席终端不让选看VMP
	if (!HasJoinedChairman())
	{
		cServMsg.SetErrorCode(ERR_MCU_INVALID_OPER);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ChairmanSeeVmp] Only chairman can select see VMP!\n");
		SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
		return ;
	}*/
	
	//会议不在vmp中
	u8 byVmpCount = GetVmpCountInVmpList();
	if (byVmpCount == 0)	
	{
		cServMsg.SetErrorCode(ERR_MCU_VMPNOTSTART);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[TMtSeeVmp] The Conf is not in the VMP mode!\n");
		SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
		return ;
	}
	
	TMt tDstMt;
	u8 byVmpId;
	if ( cServMsg.GetEventId() == MT_MCU_STARTSWITCHVMPMT_REQ )
	{
		tDstMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
		if (!HasJoinedChairman() || m_tConf.GetChairman().GetMtId() != tDstMt.GetMtId())
		{
			cServMsg.SetErrorCode(ERR_MCU_INVALID_OPER);
			ConfPrint( LOG_LVL_WARNING, MID_MCU_MCS, "[TMtSeeVmp] tDstMt.%d isn't ConfChairman.%d!\n", 
				tDstMt.GetMtId(), m_tConf.GetChairman().GetMtId() );
			SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
			return ;
		} 
		byVmpId = GetTheOnlyVmpIdFromVmpList();//获得当前唯一合成器外设
	}
	else
	{
		byVmpId = cServMsg.GetEqpId();
		tDstMt = *(TMt *)cServMsg.GetMsgBody();
	}

	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	u16 wError = 0;
	BOOL32 bIsChairManSeeVmp = FALSE;
	if (!CheckTMtSeeVmp(tVmpEqp, tDstMt, bIsChairManSeeVmp, wError))
	{
		cServMsg.SetErrorCode(wError);
		SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
		return ;
	}

	SendReplyBack(cServMsg,cServMsg.GetEventId()+1);
	
	if (bIsChairManSeeVmp)
	{
		if(cServMsg.GetEventId() != MT_MCU_STARTSWITCHVMPMT_REQ)
		{
			// xliang [7/3/2009] 给主席终端也回应，从而使mcs，mtc状态一致
			SendMsgToMt( tDstMt.GetMtId(), MCU_MT_STARTSWITCHVMPMT_ACK, cServMsg);
		}
		
		//m_tConf.m_tStatus.SetVmpSeebyChairman(TRUE);
		/*tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
		g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );//刷新tPeriEqpStatus*/
	}

	// xliang [4/15/2009] 若主席终端此时正在选看某个MT，则要取消那个主席选看Mt。
	// 那个被选看的MT如果没有其他MT在选看他，即该MT被选看取消，并尝试退出前适配
	u8 byMode = MODE_VIDEO;
	TMtStatus tDstMtStatus;
	m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tDstMtStatus );
	if ( !(tDstMtStatus.GetSelectMt(byMode).IsNull()) && tDstMtStatus.GetSelectMt(byMode).GetType() == TYPE_MT )
	{
		StopSelectSrc(tDstMt, byMode);
	}

	// StopSelectSrc中会更新MtStatus信息,此处需重新取最新MtStatus
	m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tDstMtStatus );
	tDstMtStatus.SetSelectMt( tVmpEqp, byMode ); //记录新的选看源
	m_ptMtTable->SetMtStatus( tDstMt.GetMtId(), &tDstMtStatus );

	if(g_cMcuVcApp.IsBrdstVMP(tVmpEqp))
	{
		//画面合成本来就在广播码流，此处不必再建交换
		//选看状态在此处上报
		MtStatusChange( &tDstMt, TRUE );
	}
	else
	{
//		TLogicalChannel tFwLogicChnnel;
//		//zhouyiliang 20101222 调整vmp到终端建交换，不再通过m_cMtRcvGrp
//		if (!m_ptMtTable->GetMtLogicChnnl( tDstMt.GetMtId(), byChnnlType, &tFwLogicChnnel, TRUE) ) 
//		{
//			ConfLog(FALSE,"[ChairmanSeeVmp]Get DstMt(%d,%d) forward logicalchnnl failed \n",tDstMt.GetMcuId(),tDstMt.GetMtId() );
//			return;
//		}
//		
//		u8 bySrcChnnl  = GetVmpOutChnnlByRes( tFwLogicChnnel.GetVideoFormat(), 
//												m_tVmpEqp.GetEqpId(),
//												m_tConf, 
//												tFwLogicChnnel.GetChannelType() );
		u8 bySrcChnnl = GetVmpOutChnnlByDstMtId( tDstMt.GetMtId(), tVmpEqp.GetEqpId());
		if ( 0xFF == bySrcChnnl ) //没找到对应的通道
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[TMtSeeVmp] Get DstMt(%d,%d) vmp outchannl failed \n",tDstMt.GetMcuId(),tDstMt.GetMtId() );
			return;
		}

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[TMtSeeVmp] VMP --> MT.%u, SrcChnnlIdx is %u\n",tDstMt.GetMtId(), bySrcChnnl);

		// 调整该通道码率
		AdjustVmpSelBitRate(tVmpEqp);

		//建交换
		TSwitchInfo tSwitchInfo;
		tSwitchInfo.SetDstMt(tDstMt);
		tSwitchInfo.SetSrcMt(tVmpEqp);
		tSwitchInfo.SetSrcChlIdx(bySrcChnnl);
		tSwitchInfo.SetMode(MODE_VIDEO);

		SwitchSrcToDst( tSwitchInfo, cServMsg );
	}
}

/*==============================================================================
函数名    :  CheckTMtSeeVmp
功能      :  终端选看画面合成Check
算法实现  :  
参数说明  :  u8 byVmpId
			 const TMt &tDstMt
			 u16 &wError
返回值说明:  BOOL32
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
130530					yanghuaizhi							
==============================================================================*/
BOOL32 CMcuVcInst::CheckTMtSeeVmp(const TEqp &tVmpEqp, const TMt &tDstMt, BOOL32 &bIsChairManSeeVmp, u16 &wError)
{
	if (tVmpEqp.IsNull() || !IsValidVmpId(tVmpEqp.GetEqpId()) || !IsVmpIdInVmpList(tVmpEqp.GetEqpId()))
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[TMtSeeVmp] VmpId(%d) is wrong!\n", tVmpEqp.GetEqpId());
		return FALSE;
	}
	
	//老VMP不支持选看
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;			//取vmp子类型
	if(byVmpSubType == VMP)
	{
		wError = ERR_FUNCTIONNOTSUPPORTBYOLDVMP;
		return FALSE;
	}
	
	//vmp广播状态下,终端选看vmp没有意义,回Nack
	if (g_cMcuVcApp.IsBrdstVMP(tVmpEqp))
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[TMtSeeVmp] tMt can't select see VMP because the VMP is Brdst!\n");
		return FALSE;
	}
	
	// 非本级终端不支持选看画面合成
	if (tDstMt.IsNull() || !tDstMt.IsLocal())
	{
		wError = ERR_MCU_OPRONLYLOCALMT;
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[TMtSeeVmp] tDstMt is null or is not local. can't select see VMP because Chairman Polling!\n");
		return FALSE;
	}
	
	bIsChairManSeeVmp = FALSE;
	//会议正处于"主席轮询选看"模式，不允许选看VMP
	if (HasJoinedChairman() && m_tConf.GetChairman().GetMtId() == tDstMt.GetMtId())
	{
		bIsChairManSeeVmp = TRUE;
		//20110414_miaoqs 主席轮询选看功能添加
		//20120903 zhouyiliang 主席选看vmp则停轮询
		if ( (CONF_POLLMODE_VIDEO_CHAIRMAN == m_tConf.m_tStatus.GetPollMode()) 
			||(CONF_POLLMODE_BOTH_CHAIRMAN == m_tConf.m_tStatus.GetPollMode()) )
		{
			// 			cServMsg.SetErrorCode(ERR_MCU_SPECCHAIREMAN_NACK);
			// 			ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[TMtSeeVmp] Chairman can't select see VMP because Chairman Polling!\n");
			// 			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	//nack
			// 			return;
			ProcStopConfPoll();
		}
	}
	
    //已处于强制广播状态，不允许选看画面合成
    if ( m_tConf.m_tStatus.IsMustSeeSpeaker() && !bIsChairManSeeVmp) 
    {
		wError = ERR_MCU_MUSTSEESPEAKER;
		return FALSE;
    }
	
	u8 byChnnlType = LOGCHL_VIDEO;   
    TLogicalChannel tLogicChannel;
    if ( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) ||
		!m_ptMtTable->GetMtLogicChnnl( tDstMt.GetMtId(), byChnnlType, &tLogicChannel, TRUE ) )
    {
		wError = ERR_MCU_GETMTLOGCHN_FAIL;
        ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ChairmanSeeVmp] Mt.%u isn't online in conf or forward logical channel not open!\n",tDstMt.GetMtId());
		return FALSE;
    }

	// 匹配不到画面合成通道，同样Nack,此处查找合适通道时，忽略当前选看源
	u8 byVmpOutChl  = GetVmpOutChnnlByRes(tVmpEqp.GetEqpId(), 
		tLogicChannel.GetVideoFormat(), 
		tLogicChannel.GetChannelType(),
		tLogicChannel.GetChanVidFPS(), 
		tLogicChannel.GetProfileAttrb());
	if (0XFF == byVmpOutChl)
	{
		wError = ERR_MCU_VCS_NOVCMTVID;
        ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[TMtSeeVmp] Mt.%u can't get correct vmp out channel!\n",tDstMt.GetMtId());
		return FALSE;
	}

	return TRUE;
}

/*====================================================================
    函数名      MCSSeeVmp
    功能        ：会控选看建交换，置状态的操作
    算法实现    ：
    引用全局变量：
    输入参数说明：CServMsg &cServMsg
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	3/29/2012 	4.7			yanghuaizhi     创建
====================================================================*/
void CMcuVcInst::MCSSeeVmp(CServMsg &cServMsg, BOOL32 bNeglectVmpMode /* = FALSE */)
{
	TSwitchInfo	tSwitchInfo;
	TMt	tDstMt, tSrcMt;
	CServMsg  cMsg( cServMsg.GetServMsg(), cServMsg.GetServMsgLen() ); 
	
    tSwitchInfo = *( TSwitchInfo * )cServMsg.GetMsgBody();
    tSrcMt = tSwitchInfo.GetSrcMt();
    tDstMt = tSwitchInfo.GetDstMt();
	u8 bySwitchMode = tSwitchInfo.GetMode();	// 记录初始MODE
	
	if (tSrcMt.GetType() != TYPE_MCUPERI || tSrcMt.GetEqpType() != EQP_TYPE_VMP)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[MCSSeeVmp] tSrcMt(%d) is not VmpEqp(%d)!\n",tSrcMt.GetEqpId(),tSrcMt.GetEqpId());
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//会议不在vmp中
	u8 byVmpCount = GetVmpCountInVmpList();
	if (0 == byVmpCount || IsVmpIdInVmpList(tSrcMt.GetEqpId()))	
	{
		cServMsg.SetErrorCode(ERR_MCU_VMPNOTSTART);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ChairmanSeeVmp] The Conf is not in the VMP mode!\n");
		SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
		return ;
	}
	
	//老VMP不支持选看
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( tSrcMt.GetEqpId(), &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;			//取vmp子类型
	if(byVmpSubType == VMP)
	{
		cServMsg.SetErrorCode(ERR_FUNCTIONNOTSUPPORTBYOLDVMP);
		SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
		return ;
	}
	
	if (cServMsg.GetEventId() != MCS_MCU_STARTSWITCHMC_REQ)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[MCSSeeVmp] EventId[%d] is not MCS_MCU_STARTSWITCHMC_REQ!\n",cServMsg.GetEventId());
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	// 选看vmp,mode不可能为audio
	if (MODE_NONE == bySwitchMode || MODE_AUDIO == bySwitchMode)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[MCSSeeVmp] SwitchMode(%d) is wrong!\n", bySwitchMode);
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	/* 卫星分散会议时需要判断回传通道数 待确认是否需要
	if (m_tConf.GetConfAttrb().IsSatDCastMode())
	{
		if (IsOverSatCastChnnlNum(tSrcMt))
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMtMcuStartSwitchMtReq] over max upload mt num. nack!\n");
			cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
	}*/

    //加密以及动态载荷信息
	u8 byMcsId = cServMsg.GetSrcSsnId();
    TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
    TSimCapSet    tSrcSCS  = m_tConf.GetMainSimCapSet(); 
	tSrcSCS.SetAudioMediaType(MEDIA_TYPE_NULL);
    TDoublePayload tDVPayload;
    TDoublePayload tDAPayload;
    
    if( MEDIA_TYPE_H264 == tSrcSCS.GetVideoMediaType() || 
        MEDIA_TYPE_H263PLUS == tSrcSCS.GetVideoMediaType() || 
        CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
        // zbq [10/29/2007] 适应FEC支持其他格式
        m_tConf.GetCapSupportEx().IsVideoSupportFEC())
    {
        tDVPayload.SetRealPayLoad( tSrcSCS.GetVideoMediaType() );
        tDVPayload.SetActivePayload( GetActivePayload(m_tConf, tSrcSCS.GetVideoMediaType() ) );
    }
    else
    {
        tDVPayload.SetRealPayLoad( tSrcSCS.GetVideoMediaType() );
        tDVPayload.SetActivePayload( tSrcSCS.GetVideoMediaType() );
    }
    if( CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
        // zbq [10/29/2007] 适应FEC支持其他格式
        m_tConf.GetCapSupportEx().IsAudioSupportFEC() )
    {
        tDAPayload.SetRealPayLoad( tSrcSCS.GetAudioMediaType() );
        tDAPayload.SetActivePayload( GetActivePayload(m_tConf, tSrcSCS.GetAudioMediaType() ) );
    }
    else
    {
        tDAPayload.SetRealPayLoad( tSrcSCS.GetAudioMediaType() );
        tDAPayload.SetActivePayload( tSrcSCS.GetAudioMediaType() );
    }
    
    cMsg.SetMsgBody(cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
    cMsg.CatMsgBody( (u8 *)&tEncrypt, sizeof(tEncrypt));
    cMsg.CatMsgBody( (u8 *)&tDVPayload, sizeof(tDVPayload));
    cMsg.CatMsgBody( (u8 *)&tEncrypt, sizeof(tEncrypt));
    cMsg.CatMsgBody( (u8 *)&tDAPayload, sizeof(tDAPayload));

    SendReplyBack( cMsg, cMsg.GetEventId() + 1 );
	
	// [9/13/2010 xliang] if UI ask the same Src in the same MC channel, MCU should do nothing.
	TMt tMcAudCurSrc;
	TMt tMcVidCurSrc;
	g_cMcuVcApp.GetMcSrc( byMcsId, &tMcAudCurSrc, tSwitchInfo.GetDstChlIdx(), MODE_AUDIO );
	g_cMcuVcApp.GetMcSrc( byMcsId, &tMcVidCurSrc, tSwitchInfo.GetDstChlIdx(), MODE_VIDEO );
	//zjj20101119补充具体不同音视频模式
	if( MODE_VIDEO == bySwitchMode &&  tMcVidCurSrc == tSrcMt )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "Mt(%d,%d) video already in Mc channel.%d, Do nothing!\n", tSrcMt.GetMcuId(), tSrcMt.GetMtId(), tSwitchInfo.GetDstChlIdx());
		return;
	}
	if( MODE_BOTH == bySwitchMode && tMcVidCurSrc == tSrcMt && tMcAudCurSrc == tMcVidCurSrc )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "Mt(%d,%d) video and audio already in Mc channel.%d, Do nothing!\n", tSrcMt.GetMcuId(), tSrcMt.GetMtId(), tSwitchInfo.GetDstChlIdx());
		return;
	}

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcStartSelSwitchMt] Switch Mt(%d.%d) To Mc(%d) Channel(%d)	Mode:(%d)\n",
		tSrcMt.GetMcuId(),tSrcMt.GetMtId(),
		cServMsg.GetSrcSsnId(),tSwitchInfo.GetDstChlIdx(),
		tSwitchInfo.GetMode()
		);

	u8 bySrcChnnl = GetVmpOutChnnlByRes(tSrcMt.GetEqpId(),
		tSrcSCS.GetVideoResolution(),
		tSrcSCS.GetVideoMediaType(),
		tSrcSCS.GetUserDefFrameRate(),
		tSrcSCS.GetVideoProfileType());
	if ( 0xFF == bySrcChnnl ) //没找到对应的通道
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[MCSSeeVmp] Get MCS vmp outchannl failed \n");
		return;
	}

	// 8kh有特殊场景
	if (VMP_8KH == GetVmpSubType(tSrcMt.GetEqpId()))
	{
		// 会议是1080p30fpsBP适配会议或720p30fpsHP适配会议时,为避免vmp广播非广播切换时通道改变,取1通道
		if (IsConfExcludeDDSUseAdapt())
		{
			if (VIDEO_FORMAT_HD1080 == tSrcSCS.GetVideoResolution() && 
				tSrcSCS.GetUserDefFrameRate() < 50 &&
				tSrcSCS.GetVideoProfileType() == emBpAttrb)
			{
				if (IsMSSupportCapEx(VIDEO_FORMAT_HD720))
				{
					bySrcChnnl = 1;
				}
				else if (IsMSSupportCapEx(VIDEO_FORMAT_4CIF))
				{
					bySrcChnnl = 2;
				}
				else if (IsMSSupportCapEx(VIDEO_FORMAT_CIF))
				{
					bySrcChnnl = 3;
				}
			}
			if (VIDEO_FORMAT_HD720 == tSrcSCS.GetVideoResolution() && 
				tSrcSCS.GetUserDefFrameRate() < 50 &&
				tSrcSCS.GetVideoProfileType() == emHpAttrb)
			{
				if (IsMSSupportCapEx(VIDEO_FORMAT_HD720, tSrcSCS.GetUserDefFrameRate(), emBpAttrb))
				{
					bySrcChnnl = 1;
				}
				else if (IsMSSupportCapEx(VIDEO_FORMAT_4CIF))
				{
					bySrcChnnl = 2;
				}
				else if (IsMSSupportCapEx(VIDEO_FORMAT_CIF))
				{
					bySrcChnnl = 3;
				}
			}
		}
	}
	// 8ki有特殊场景
	else if (VMP_8KI == GetVmpSubType(tSrcMt.GetEqpId()))
	{
		// 会议是1080p30fps或720p60fps适配会议时,为避免vmp广播非广播切换时通道改变,取1通道
		if (IsConfExcludeDDSUseAdapt()) //适配会议
		{
			if ((m_tConf.GetMainVideoFormat() == VIDEO_FORMAT_HD1080 && m_tConf.GetMainVidUsrDefFPS() <= 30) ||
				(m_tConf.GetMainVideoFormat() == VIDEO_FORMAT_HD720 && m_tConf.GetMainVidUsrDefFPS() > 30))
			{
				TKDVVMPOutParam tVMPOutParam = g_cMcuVcApp.GetVMPOutParam(tSrcMt);
				TVideoStreamCap tTmpStrCap = tVMPOutParam.GetVmpOutCapIdx(1);
				// 720/4cif/cif都未勾选时,是不允许非广播的,仍取第0路
				if (MEDIA_TYPE_H264 == tTmpStrCap.GetMediaType())
				{
					bySrcChnnl = 1;
				}
				else
				{
					bySrcChnnl = 0;
				}
			}
		}
	}
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[MCSSeeVmp] VMP --> MCS.%u, SrcChnnlIdx is %u\n",byMcsId, bySrcChnnl);
	
	//建交换
	TSwitchInfo tTempSwitchInfo;
	memcpy(&tTempSwitchInfo, &tSwitchInfo, sizeof(tTempSwitchInfo));
	tTempSwitchInfo.SetSrcChlIdx(bySrcChnnl);
	tTempSwitchInfo.SetMode(MODE_VIDEO);
	
	SwitchSrcToDst( tTempSwitchInfo, cServMsg );

}

/*====================================================================
    函数名      ：ProcMtMcuStopSwitchMtReq
    功能        ：停止终端选看处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	02/12/19	1.0			LI Yi         创建
	04/02/26    3.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::ProcMtMcuStopSwitchMtReq(const CMessage * pcMsg)
{
    STATECHECK;

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TMt tDstMt = *(TMt*)cServMsg.GetMsgBody();

    /*@ 20110603 zjl Mcs停选看通信结构为TSwitchInfo(带选看模式)
                     终端停选看通信结构为TMt,
                     这里区分两条消息处理，
                     不直接将选看模式全停掉*/
    u8 byStopMode = MODE_BOTH;
    if( cServMsg.GetEventId() == MCS_MCU_STOPSWITCHMT_REQ )
    {
        TSwitchInfo tSwitchInfo = *(TSwitchInfo*)cServMsg.GetMsgBody();
        byStopMode = tSwitchInfo.GetMode();
    }     

    // miaoqingsong [2011/08/09] 主席轮询模式下不允许取消“目的端”为主席终端的选看
	if ( ((CONF_POLLMODE_VIDEO_CHAIRMAN == m_tConf.m_tStatus.GetPollMode()) ||
		(CONF_POLLMODE_BOTH_CHAIRMAN == m_tConf.m_tStatus.GetPollMode())) &&
		tDstMt == m_tConf.GetChairman() && cServMsg.GetSrcMtId() != m_tConf.GetChairman().GetMtId())
	{
        // [1/25/2013 liaokang] 新需求：主席轮询不带音频情况下，允许取消选听
        if( CONF_POLLMODE_BOTH_CHAIRMAN == m_tConf.m_tStatus.GetPollMode() || 
            ( CONF_POLLMODE_VIDEO_CHAIRMAN == m_tConf.m_tStatus.GetPollMode() && !( MODE_AUDIO == byStopMode ) ) )
        {
            ConfPrint( LOG_LVL_ERROR, MID_MCU_MT, "Can't stop chairman's select because Chairman Polling!\n" );
            cServMsg.SetErrorCode( ERR_MCU_SPECCHAIREMAN_NACK );
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );	//nack
            return;
        }		
	}

    if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
    {
        ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[ProcMtMcuStopSwitchMtReq] Dst Mt%u-%u not joined conference!\n", 
                         tDstMt.GetMcuId(), tDstMt.GetMtId() );
        cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        return;
    }
    
    if( cServMsg.GetEventId() == MCS_MCU_STOPSWITCHMT_REQ )
    {
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
    } 
	//针对主席有选看,切发言人时,某些旧终端(如8010A)收到h_ctrl_SimpleConfInfoInd后会发MT_MCU_STOPSELMT_CMD消息给MCU
	//导致主席选看被取消,属终端原因,此处不做对应.新终端都不会再回复MT_MCU_STOPSELMT_CMD消息主动取消选看.

	if (byStopMode & MODE_AUDIO)
	{
		StopSelectSrc(tDstMt, MODE_AUDIO);
	}
	
	if (byStopMode & MODE_VIDEO)
	{
		StopSelectSrc(tDstMt, MODE_VIDEO);
	}
	
	if (byStopMode & MODE_VIDEO2SECOND)
	{
		StopSelectSrc(tDstMt, MODE_VIDEO2SECOND);
	}

	// 设置发言人源为无源, zgc, 2008-04-14
	if ( HasJoinedSpeaker() && tDstMt == GetLocalSpeaker() )
	{
		SetSpeakerSrcSpecType( MODE_BOTH, SPEAKER_SRC_NOTSEL );
	}
    
    return;
}

/*====================================================================
    函数名      ：ProcMtMcuOpenLogicChnnlRsp
    功能        ：终端发来的打开逻辑通道应答
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/01/23    1.0         LI Yi         创建
	04/02/12    3.0         胡昌威        胡昌威
	05/12/19	4.0			张宝卿		  T120集成通道策略修改
====================================================================*/
void CMcuVcInst::ProcMtMcuOpenLogicChnnlRsp( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt;
	u8	bySrcChnnl, byChannel = 0;
	TLogicalChannel	tLogicChnnl;

	switch( CurState() )
	{
	case STATE_ONGOING:
    {
		TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();
		tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
		tLogicChnnl = *( TLogicalChannel * )( cServMsg.GetMsgBody() );
		TMt tSpeaker = m_tConf.GetSpeaker();

		if( pcMsg->event == MT_MCU_OPENLOGICCHNNL_NACK )
		{
			// vrs新录播支持，通道打开失败，挂断vrs新录播实体
			if (tMt.GetMtType() == MT_TYPE_VRSREC)
			{
				// 释放vrs新录播资源和相关信息
				ReleaseVrsMt(tMt.GetMtId());

				return;
			}

			if(tLogicChnnl.GetMediaType() == MODE_VIDEO)
			{
                //[9/27/2011 zhushengze]第一次打开不成功再尝试用辅格式开
                TMultiCapSupport tComCap;
                if (m_ptMtTable->GetMtMultiCapSupport( tMt.GetMtId(), &tComCap))
                {
                    TSimCapSet tSecCap = tComCap.GetSecondSimCapSet();
                    
                    if ( MEDIA_TYPE_NULL != tSecCap.GetVideoMediaType() &&
                         tLogicChnnl.GetChannelType() != tSecCap.GetVideoMediaType() )
                    {
                        tLogicChnnl.SetChannelType(tSecCap.GetVideoMediaType());
                        tLogicChnnl.SetVideoFormat(tSecCap.GetVideoResolution());
                        tLogicChnnl.SetChanVidFPS(tSecCap.GetUserDefFrameRate());
						//设置Profile(HP/BP)属性[12/9/2011 chendaiwei]
                        tLogicChnnl.SetProfieAttrb(tSecCap.GetVideoCap().GetH264ProfileAttrb());
                        CServMsg cOpenMainChnlMsg;
                        u8 byRes = tLogicChnnl.GetVideoFormat();
                        cOpenMainChnlMsg.SetMsgBody((u8*)&tLogicChnnl, sizeof(TLogicalChannel));
                        cOpenMainChnlMsg.CatMsgBody(&byRes, sizeof(u8));
                        SendMsgToMt(tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cOpenMainChnlMsg);
                        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlRsp]Open MT:%d Main Logic Channel again with SecMain!\n", tMt.GetMtId());
                        
                        return;
                    }
                }

				m_ptMtTable->SetMtVideoRecv( tMt.GetMtId(), FALSE );
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mcu open Mt%d primary video logicChannel(0x%x:%d%s) error!\n", 
				tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
				tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) );
			}
			if(tLogicChnnl.GetMediaType() == MODE_AUDIO)
			{
				m_ptMtTable->SetMtAudioRecv( tMt.GetMtId(), FALSE );
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mcu open Mt%d audio logicChannel(0x%x:%d%s:%d) error!\n", 
					tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
					tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ),tLogicChnnl.GetAudioTrackNum() );
			}
			if(tLogicChnnl.GetMediaType() == MODE_SECVIDEO)
			{
                //[6/16/2011 zhushengze]第一次打开不成功再尝试用第二共同双流开
                TMultiCapSupport tMultiComCap;
                if (m_ptMtTable->GetMtMultiCapSupport( tMt.GetMtId(), &tMultiComCap))
                {
                    TDStreamCap tSecDSCap = tMultiComCap.GetSecComDStreamCapSet();

                    if (/*tSecDSCap.GetMediaType() == MEDIA_TYPE_H264 &&*/
                        MEDIA_TYPE_NULL != tSecDSCap.GetMediaType() &&
                        tLogicChnnl.GetVideoFormat() != tSecDSCap.GetResolution())
                    {
                        tLogicChnnl.SetVideoFormat(tSecDSCap.GetResolution());
                        tLogicChnnl.SetChanVidFPS(tSecDSCap.GetUserDefFrameRate());
                        tLogicChnnl.SetProfieAttrb(tSecDSCap.GetH264ProfileAttrb());
                        CServMsg cOpenSecChnlMsg;
                        u8 byRes = tLogicChnnl.GetVideoFormat();
                        cOpenSecChnlMsg.SetMsgBody((u8*)&tLogicChnnl, sizeof(TLogicalChannel));
                        cOpenSecChnlMsg.CatMsgBody(&byRes, sizeof(u8));
                        SendMsgToMt(tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cOpenSecChnlMsg);
                        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlRsp]Open MT:%d DS Logic Channel again with SecCom DS!\n", tMt.GetMtId());
                        
                        return;
                    }
                }
                
				m_ptMtTable->SetMtVideo2Recv( tMt.GetMtId(), FALSE );
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mcu open Mt%d second video logicChannel(0x%x:%d%s) error!\n", 
					tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
					tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) );
                
                //  xsl [6/15/2006] 打开双流失败时，终端恢复接收带宽，若存在自动升降速则不恢复
                if (m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE) == GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())))
                {
                    u16 wMtDialBitrate = m_ptMtTable->GetRcvBandWidth(tMt.GetMtId());
                    m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), wMtDialBitrate, LOGCHL_VIDEO);
                    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuOpenLogicChnnlRsp]tDstMt.GetMtId() = %d, MtReqBitrate = %d\n",
                        tMt.GetMtId(), wMtDialBitrate);
                }            

				//尝试切换码流置第一路
				StartSwitchDStreamToFirstLChannel( tMt );

				if (!(m_ptMtTable->GetMtSelMtByMode(tMt.GetMtId(), MODE_VIDEO2SECOND).IsNull()))
				{
					m_ptMtTable->RemoveMtSelMtByMode(tMt.GetMtId(), MODE_VIDEO2SECOND);
					
					//报终端状态和双选看状态
					MtStatusChange(&tMt, TRUE);
					MtSecSelSrcChange(tMt);
				}
			}

			//如果打开T120数据通道失败
			if ( MODE_DATA == tLogicChnnl.GetMediaType() )
			{
				if ( MEDIA_TYPE_T120 == tLogicChnnl.GetChannelType() )
				{
					if ( m_ptMtTable->IsMtIsMaster( tMt.GetMtId() ) && m_ptMtTable->IsNotInvited( tMt.GetMtId() ))
					{
						SendMcuDcsDelMtReq( tMt.GetMtId() );
						ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "Mcu open mt%d t120 logicchannel (0x%x:%d%s) as master failed !\n",
							tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
							tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) ); 
					}		
					//如果对端做主, 且是KDV MCU, 则是正常操作
					else
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "Mcu open mt%d t120 logicchannel (0x%x:%d%s) as slave failed, its legal affair if KEDA MCU as master !\n",
							tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
							tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) ); 
					}
				}
			}

			//对于VCS会议, 需提示该终端无法接收图像
			u8 byMediaType = tLogicChnnl.GetVideoFormat();
			if (VCS_CONF == m_tConf.GetConfSource() && 
				(MODE_VIDEO == byMediaType || MODE_AUDIO == byMediaType))
			{
				VCSMTAbilityNotif(tMt, byMediaType);
			}
			return;
		}

		//vcs快速呼叫挂断下级导致上级到下级的交换未拆，因为消息消息在osp队列中堵塞，
		//上级挂断下级后，才收到之前呼下级时，下级回上级开逻辑通道的应答。
		if( !m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
		{
			LogPrint(LOG_LVL_WARNING, MID_MCU_CALL,"[ProcMtMcuOpenLogicChnnlRsp] Mt%d doesnot join conf, then doesnot deal open chl rsp\n", 
				tMt.GetMtId() );
			return;
		}

        // 以下是成功打开逻辑通道的处理
		switch( tLogicChnnl.GetMediaType() )
		{
		case MODE_VIDEO:
			{
				m_ptMtTable->SetMtVideoRecv( tMt.GetMtId(), TRUE, tLogicChnnl.GetFlowControl() );
				byChannel = LOGCHL_VIDEO;
				MtStatusChange( &tMt, TRUE );
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mcu open Mt%d primary video logicChannel([IP]:0x%x:%d,[Type]:%s,[Res]:%s,[Fps]:%d,[BR]:%d) success!\n", 
					tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
					tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ),
					GetResStr(tLogicChnnl.GetVideoFormat()), tLogicChnnl.GetChanVidFPS(),
					tLogicChnnl.GetFlowControl());

				//如果当前终端开的接收通道是mpeg4 auto，判断是否需要改变MP4编码的参数
				BOOL32 bMepg4AutoMt = FALSE;
				u16 wMtBitRate = 0;
				if (tMt.IsLocal() )
				{
					if (tLogicChnnl.GetChannelType() == MEDIA_TYPE_MP4 && tLogicChnnl.GetVideoFormat() == VIDEO_FORMAT_AUTO ) 
					{
						bMepg4AutoMt = TRUE;
						wMtBitRate = m_ptMtTable->GetDialBitrate(tMt.GetMtId());
					}
				}
				if ( bMepg4AutoMt && m_tConf.GetSecBitRate() != 0) 
				{
					u8 byOutChnl = GetVmpOutChnnlByRes(m_tVmpEqp.GetEqpId(), VIDEO_FORMAT_AUTO,MEDIA_TYPE_MP4);
					if (0XFF != byOutChnl)
					{
						u16 wMinMp4GrpBirate = GetMinMtRcvBitByVmpChn(m_tVmpEqp.GetEqpId(), TRUE,byOutChnl);
						if ( wMtBitRate < wMinMp4GrpBirate )
						{
							//改变mp4 的那路编码参数
							TPeriEqpStatus tVmpStatus;
							g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpStatus);
							TVMPParam_25Mem tVmpParam = tVmpStatus.m_tStatus.tVmp.GetVmpParam();
							if (tVmpStatus.m_tStatus.tVmp.m_byUseState == TVmpStatus::WAIT_START || 
								tVmpStatus.m_tStatus.tVmp.m_byUseState == TVmpStatus::START) 
							{
								ChangeVmpParam(m_tVmpEqp.GetEqpId(), &tVmpParam);
							}
						}
					}
				}

				// xliang [11/10/2008] 打开第一路通道成功后，直接打开Polycom的MCU第二路通道
				if ( ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) ||
					   MT_MANU_AETHRA == m_ptMtTable->GetManuId(tMt.GetMtId()) )
					&& 
					 ( MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
					   MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) )
					)
				{
					SetTimer( EV_MCU_OPENSECVID_TIMER, TIMESPACE_WAIT_FSTVIDCHL_SUC, tMt.GetMtId() );
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[OpenLogicChnnlRsp] Mt.%d Fst chnnl open succeed, try the second one immediately due to Polycom\n", tMt.GetMtId() );
				}
			}
			break;

		case MODE_SECVIDEO:
			//zjj20120801 2个mcu都配置任意终端发起,且都发起双流,上级呼叫下级,导致上级认为下级为上流源,下级认为上级为双流源,导致全局双流无法发起
			if( !m_tDoubleStreamSrc.IsNull() && tMt.GetMtId() == m_tDoubleStreamSrc.GetMtId() &&
				m_ptMtTable->GetManuId(tMt.GetMtId()) == MT_MANU_KDCMCU )
			{				
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlRsp] m_tDoubleStreamSrc.%d can't open forward sec chl\n",
								m_tDoubleStreamSrc.GetMtId() );
				u8 byOut = 1;
				cServMsg.SetEventId( MCU_MT_CLOSELOGICCHNNL_CMD );
				cServMsg.SetMsgBody( ( u8 * )&tLogicChnnl, sizeof( tLogicChnnl ) );	
				cServMsg.CatMsgBody( &byOut, sizeof(byOut) );				
				SendMsgToMt( tMt.GetMtId(), MCU_MT_CLOSELOGICCHNNL_CMD, cServMsg );
				return;				
			}
			m_ptMtTable->SetMtVideo2Recv( tMt.GetMtId(), TRUE );
			byChannel = LOGCHL_SECVIDEO;
			MtStatusChange( &tMt, TRUE );
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "Mcu open Mt%d second video logicChannel([IP]:0x%x:%d,[H239]:%d,[type]:%s,[Res]:%s,[Fps]:%d) success!\n", 
                tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
                tLogicChnnl.m_tRcvMediaChannel.GetPort(), tLogicChnnl.IsSupportH239(), 
                GetMediaStr(tLogicChnnl.GetChannelType()),
                GetResStr(tLogicChnnl.GetVideoFormat()),
                    tLogicChnnl.GetChanVidFPS());
        
			//zbq[09/02/2008] 兼容老的终端，Res为空，参照能力补齐（暂只处理双流）
			if (0 == tLogicChnnl.GetVideoFormat())
			{
				TCapSupport tMtCap;
				if (!m_ptMtTable->GetMtCapSupport(tMt.GetMtId(), &tMtCap))
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[OpenLogicChanRsp] get mt.%d's cap failed, check it\n", tMt.GetMtId());
				}
				tLogicChnnl.SetVideoFormat(tMtCap.GetDStreamCapSet().GetResolution());
			}
			// xliang [11/14/2008]  打开第二路通道成功后，判断当前是否有双流源，以期请求令牌并建双流交换
			if ( (MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) ||
				 MT_MANU_AETHRA == m_ptMtTable->GetManuId(tMt.GetMtId()) )
				&&
				(MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
				MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ) &&
				!m_tDoubleStreamSrc.IsNull() && !(tMt == m_tDoubleStreamSrc) )
			{
				//请求令牌
				TH239TokenInfo tH239TokenInfo;
				
				CServMsg cSendServMsg;
				cSendServMsg.SetEventId(MCU_POLY_GETH239TOKEN_REQ);
				cSendServMsg.SetMsgBody((u8*)&tMt, sizeof(TMt));
				cSendServMsg.CatMsgBody((u8*)&tH239TokenInfo, sizeof(tH239TokenInfo));
				SendMsgToMt(tMt.GetMtId(), MCU_POLY_GETH239TOKEN_REQ, cSendServMsg );
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[OpenChnnlRsp] MCU_POLY_GETH239TOKEN_REQ to PolyMCU.%d\n", tMt.GetMtId());
				
			}
			break;

		case MODE_AUDIO:
			m_ptMtTable->SetMtAudioRecv( tMt.GetMtId(), TRUE );
			byChannel = LOGCHL_AUDIO;
			MtStatusChange( &tMt, TRUE );
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mcu open Mt%d audio logicChannel(0x%x:%d:%s:%d) success!\n", 
					tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
					tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ),tLogicChnnl.GetAudioTrackNum() );
			break;
		case MODE_DATA:
			if( tLogicChnnl.GetChannelType() == MEDIA_TYPE_H224 )
			{
				byChannel = LOGCHL_H224DATA;
				TMtStatus tMtStatus;
				m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tMtStatus);
				tMtStatus.SetIsEnableFECC(TRUE);
				m_ptMtTable->SetMtStatus(tMt.GetMtId(), &tMtStatus);
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mcu open Mt%d data H224 logicChannel(0x%x:%d:%s) success!\n", 
						tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
						tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) );
			}

			//t120 数据通道打开成功, 逻辑通道的保存
			else if ( MEDIA_TYPE_T120 == tLogicChnnl.GetChannelType() ) 
			{					
				//如果本端主呼, 需要保存对端的地址和端口
				if ( !m_ptMtTable->IsNotInvited( tMt.GetMtId() ) )
				{	
					u32 dwDcsIp  = tLogicChnnl.m_tRcvMediaChannel.GetIpAddr();
					u16 wDcsPort = tLogicChnnl.m_tRcvMediaChannel.GetPort();

					byChannel = LOGCHL_T120DATA;
					
					//地址有效
					if ( 0 != dwDcsIp && 0 != wDcsPort )
					{
						//保存该地址, 断链时作重新邀请之用
						m_ptMtTable->SetMtDcsAddr( tMt.GetMtId(), dwDcsIp, wDcsPort );

						//告诉DCS增加该终端, 增加方式为邀请下级
						SendMcuDcsAddMtReq( tMt.GetMtId(), dwDcsIp, wDcsPort );

						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mcu open Mt%d data T.120 logicChannel(0x%x:%d:%s) as master success!\n", 
							tMt.GetMtId(), dwDcsIp, wDcsPort, GetMediaStr( tLogicChnnl.GetChannelType() ) );
					}
					else
					{
						ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mcu open Mt%d data T.120 logicChannel(0x%x:%d:%s) as master failed!\n", 
							tMt.GetMtId(), dwDcsIp, wDcsPort, GetMediaStr( tLogicChnnl.GetChannelType() ) );
					}
				}
				//如果本端被呼
				else
				{
					//这里只能收到MT的ACK消息, 可以认为通道打开成功
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mcu open Mt%d data T.120 logicChannel as master success!\n", tMt.GetMtId() );
				}
			}
			break;
		default:
			break;
		}

        // [2013/04/08 chenbing] H460打洞逻辑处理
        //前向通道进行rtcp打洞
        if( cServMsg.GetMsgBodyLen()  >= sizeof(BOOL32) + sizeof(TLogicalChannel))
        {
            BOOL32 bIsPinHole = *( BOOL32 * )( cServMsg.GetMsgBody() + sizeof(TLogicalChannel));
            if ( TRUE == bIsPinHole &&
                 ( MODE_VIDEO == tLogicChnnl.GetMediaType() || 
                   MODE_AUDIO == tLogicChnnl.GetMediaType() ||
                   MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
               )
            {
                Starth460PinHoleNotify(tLogicChnnl.m_tSndMediaCtrlChannel.GetIpAddr(),
                    tLogicChnnl.m_tSndMediaCtrlChannel.GetPort(),
                    tLogicChnnl.m_tRcvMediaCtrlChannel.GetIpAddr(),
                    tLogicChnnl.m_tRcvMediaCtrlChannel.GetPort(),
                    pinhole_rtcp,
                    tLogicChnnl.m_byActivePayload);
                m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_bIsPinHole = TRUE;
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  
                    "[ProcMtMcuOpenLogicChnnlRsp] H460 LocalRtcpAddr:%d LocalRtcpPort:%d RemoteRtcpAddr:%d RemoteRtcpPort:%d Payload:%d!\n", 
                tLogicChnnl.m_tSndMediaCtrlChannel.GetIpAddr(), 
                tLogicChnnl.m_tSndMediaCtrlChannel.GetPort(), 
                tLogicChnnl.m_tRcvMediaCtrlChannel.GetIpAddr(), 
                tLogicChnnl.m_tRcvMediaCtrlChannel.GetPort(), 
                tLogicChnnl.m_byActivePayload);
                
            }
        }

		BOOL32 bIsBrdVidNeedAdp = FALSE;	//双广播是否过适配

		//保存逻辑通道
		if( tLogicChnnl.GetMediaType() == MODE_VIDEO && byChannel == LOGCHL_VIDEO )
		{           
            //终端下行带宽初始化
			
			//zbq[09/10/2008] 下行带宽调整过，则遵循调整好的带宽
			if (!m_ptMtTable->GetRcvBandAdjusted(tMt.GetMtId()))
			{
				m_ptMtTable->SetRcvBandWidth(tMt.GetMtId(), m_ptMtTable->GetDialBitrate(tMt.GetMtId()));
			}

// 			TDri2E1Cfg tDri2E1Cfg[MAXNUM_SUB_MCU];
// 			g_cMcuVcApp.GetDri2E1CfgTable(tDri2E1Cfg);
// 			u16 wRealBandwidth = 0;
// 			for ( u8 byCfgIndx = 0; byCfgIndx < MAXNUM_SUB_MCU; byCfgIndx++)
// 			{
// 				u32 dwConIpAddr = tDri2E1Cfg[byCfgIndx].GetIpAddr();
// 				//判断MMCU(SMCU)是否在配置文件中配置[10/25/2012 chendaiwei]
// 				if( g_cMcuVcApp.GetRealBandwidthByIp(dwConIpAddr,wRealBandwidth)
// 					&& dwConIpAddr == m_ptMtTable->GetIPAddr(tMt.GetMtId())
// 					&& (m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_SMCU
// 					|| m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_MMCU))
// 				{
// 					m_ptMtTable->SetMtTransE1(tMt.GetMtId(),TRUE);
// 					
// 					u16 wActualBR = wRealBandwidth - GetAudioBitrate(m_tConf.GetMainAudioMediaType());
// 					m_ptMtTable->SetRcvBandWidth(tMt.GetMtId(),wActualBR);
// 					
// 					break;
// 				}
// 			}
			
            if (!m_tDoubleStreamSrc.IsNull() || !GetSecVidBrdSrc().IsNull())
            {
                //zbq[02/01/2008] 降速后的终端调整码率不依赖于呼入，直接取降速后的带宽
                if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
                     m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()) ) 
                {
                    tLogicChnnl.SetFlowControl( GetDoubleStreamVideoBitrate(m_ptMtTable->GetLowedRcvBandWidth(tMt.GetMtId())) );
                }
                else
                {
                    //tLogicChnnl.SetFlowControl( GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())) );
					LogPrint(LOG_LVL_DETAIL,MID_MCU_CALL,"[ProcMtMcuOpenLogicChnnlRsp] Main FlowControl adjust to %d\n",tLogicChnnl.GetFlowControl());
                }
                //启动FlowControl定时器, 防止双速会议呼入的终端码率低于第二路码流, 07-01-31
				//终端码率不同采用适配策略而不再通过取小来调源终端码率
				//SetTimer( MCUVC_SENDFLOWCONTROL_TIMER, 1200 );
            }
            /*else
            {
                tLogicChnnl.SetFlowControl( m_ptMtTable->GetDialBitrate( tMt.GetMtId() ) );
            }*/

			// [12/11/2009 xliang] send flow control indication to Aethra's MMCU
			if ( MT_MANU_AETHRA == m_ptMtTable->GetManuId(tMt.GetMtId())
				&& MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId())
				)
			{
				cServMsg.SetMsgBody( ( u8 * )&tLogicChnnl, sizeof( tLogicChnnl ) );
				
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq]Send flowcontrol indication to Mt.%u due to Eathra! br: %d\n", 
					tMt.GetMtId(), tLogicChnnl.GetFlowControl());
				
				SendMsgToMt(tMt.GetMtId(), MCU_MT_FLOWCONTROLINDICATION_NOTIF, cServMsg);
				
			}
		}
		
        //  xsl [4/28/2006] 根据双流码率比例分配视频带宽
        else if ( tLogicChnnl.GetMediaType() == MODE_SECVIDEO && byChannel == LOGCHL_SECVIDEO )
        {
			// 第一路广播是否过适配 双选看时需要调终端第一路码率，双流通道打开后会误判第一路发言人是否过适配，在保存接收码率前判断下
			if (m_tConf.GetConfAttrb().IsUseAdapter()
				&& !GetVidBrdSrc().IsNull() && GetVidBrdSrc().GetType() == TYPE_MT)
			{
				bIsBrdVidNeedAdp = IsNeedAdapt(GetVidBrdSrc(), tMt, MODE_VIDEO);
			}

            //zbq[01/31/2008] 降速终端分配带宽的基准应该是降速后的带宽, 非降速终端为呼叫带宽
            if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
                 m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()) )
            {
                tLogicChnnl.SetFlowControl( GetDoubleStreamVideoBitrate(m_ptMtTable->GetLowedRcvBandWidth(tMt.GetMtId()), FALSE) );
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlRsp] Lowed TMt.%d set DSVBR.%d\n", tMt.GetMtId(), tLogicChnnl.GetFlowControl());
                
                //降速终端此处同时计算调整第一路视频带宽.
                m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), GetDoubleStreamVideoBitrate(m_ptMtTable->GetLowedRcvBandWidth(tMt.GetMtId()), TRUE), LOGCHL_VIDEO );
            }
            else
            {
                tLogicChnnl.SetFlowControl( GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId()), FALSE) );
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlRsp] TMt.%d set DSVBR.%d\n", tMt.GetMtId(), tLogicChnnl.GetFlowControl());
            }
            
            //zbq[02/01/2008] 启动FlowControl定时器, 防止本终端为E1降速终端，后续调整当前来不及处理的带宽
			//终端码率不同采用适配策略而不再通过取小来调源终端码率
            //SetTimer( MCUVC_SENDFLOWCONTROL_TIMER, 1200 );            

            //zbq[02/18/2007] 打开双流通道，调整当前的VMP广播带宽
//             if ( m_tConf.m_tStatus.IsBrdstVMP() )
//             {
//                 AdjustVmpBrdBitRate();
//             }

            /*
            // 添加接收类型, zgc, 2008-08-07
            u8 emType = m_cConfVidTypeMgr.ConvertOut2In( tLogicChnnl.GetChannelType(), tLogicChnnl.GetVideoFormat(), TRUE );
            m_cConfVidTypeMgr.AddType( emType, TRUE );*/

			//zbq[09/12/2008]写入帧率
			//zjl[10/21/2010]目前终端上报里含有帧率
// 			TCapSupport tCap;
// 			m_ptMtTable->GetMtCapSupport(tMt.GetMtId(), &tCap);
// 			if (tCap.GetDStreamCapSet().IsFrameRateUserDefined())
// 			{
// 				tLogicChnnl.SetChanVidFPS(tCap.GetDStreamCapSet().GetUserDefFrameRate());
// 			}
// 			else
// 			{
// 				tLogicChnnl.SetChanVidFPS(tCap.GetDStreamCapSet().GetFrameRate());
// 			}
        }
		tLogicChnnl.SetMediaEncrypt(m_tConf.GetMediaKey());
		tLogicChnnl.SetActivePayload(GetActivePayload(m_tConf,tLogicChnnl.GetChannelType()));
		m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), byChannel, &tLogicChnnl, TRUE );
	
		if ( g_cMcuVcApp.IsBrdstVMP(m_tVmpEqp) )
		{
			AdjustVmpBrdBitRate();
        }

		//将此级联mcu信息进行添加置列表中
		if( MEDIA_TYPE_MMCU == tLogicChnnl.GetChannelType() )
		{
			u16 wMcuIdx = INVALID_MCUIDX;
			u8 abyMcuId[MAX_CASCADEDEPTH-1];
			memset( &abyMcuId[0],0,sizeof(abyMcuId) );
			abyMcuId[0] = tMt.GetMtId();
			m_tConfAllMcuInfo.AddMcu( &abyMcuId[0],2,&wMcuIdx );
			m_ptConfOtherMcTable->AddMcInfo( wMcuIdx );//tMt.GetMtId() );	
			m_tConfAllMtInfo.AddMcuInfo( wMcuIdx/*tMt.GetMtId()*/, tMt.GetConfIdx() );
	
			//发给会控会议所有终端信息
			//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
			//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
			SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
		
			// 2011-10-14 add by pgf:由于ProcMtMcuOpenLogicChnnlReq发请求时,下级MCU尚未加入导致未进VMP,此处加入补充处理
			TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
			if ( g_cMcuVcApp.GetVMPMode(m_tModuleVmpEqp) != CONF_VMPMODE_NONE
				&& tConfAttrb.IsHasVmpModule() && m_tConfEqpModule.IsMtInVmp(tMt))
			{
				TPeriEqpStatus tPeriEqpStatus;
				g_cMcuVcApp.GetPeriEqpStatus(m_tModuleVmpEqp.GetEqpId() , &tPeriEqpStatus);
				TVMPParam_25Mem tParamInModule = GetVmpParam25MemFromModule();
				TVMPParam_25Mem tParamInEqpStatus = tPeriEqpStatus.m_tStatus.tVmp.GetVmpParam();
				TVMPParam_25Mem tParamInConf = g_cMcuVcApp.GetConfVmpParam(m_tModuleVmpEqp);
				if ( !tParamInConf.IsMtInMember(tMt)
					&& tParamInModule.IsMtInMember(tMt))//conf信息里没有，模版里有
				{
					if (tParamInEqpStatus.IsMtInMember(tMt))//外设状态里有的话，表示下过参，模版信息未变
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "add smcu<%d %d> into VMP cause it is in module but not in conf now!\n",
							tMt.GetMcuIdx(), tMt.GetMtId());
						ChangeVmpParam(m_tModuleVmpEqp.GetEqpId(), &tParamInEqpStatus);//再下一次参，为Setvmpchnl
					}
					else//外设状态里没有的话，表示合成成员信息有变化，不做恢复
					{
						/* 此处不能直接用tParamInModule，可能会议的合成信息已改变，不能因为一个终端将所有信息恢复到模版初始状态
						// 比如模版为广播，开启后取消广播，终端上线直接拿模版信息覆盖是不对的。
						TVMPMember *ptConfMember = NULL;
						TVMPMember *ptModuleMember = NULL;
						BOOL32 bChgVmp = FALSE;
						for (u8 byIdx=0; byIdx<tParamInConf.GetMaxMemberNum() && byIdx<tParamInModule.GetMaxMemberNum(); byIdx++)
						{
							ptConfMember = tParamInConf.GetVmpMember(byIdx);
							ptModuleMember = tParamInModule.GetVmpMember(byIdx);
							if (NULL != ptModuleMember && !ptModuleMember->IsNull() && ptModuleMember->GetMtId() == tMt.GetMtId())
							{
								if (NULL != ptConfMember && ptConfMember->IsNull())
								{
									*ptConfMember = *ptModuleMember;
									bChgVmp = TRUE;
								}
							}
						}
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "add smcu<%d %d> into VMP cause it is in module but not in conf now!\n",
							tMt.GetMcuIdx(), tMt.GetMtId());
						if (bChgVmp)
						{
							ChangeVmpParam(m_tModuleVmpEqp.GetEqpId(), &tParamInConf);
						}*/
					}
				}
			}
			
			CServMsg cTmpMsg;
			//若会议在全体讨论下，尝试自动开启下级mcu的会议讨论			
            if( m_tConf.m_tStatus.IsMixing() )
			{                
			    TMcu tSMcu;			
			    tSMcu.SetMcuId( tMt.GetMtId() );
                cTmpMsg.SetMsgBody( (u8*)&tSMcu, sizeof(tSMcu) );
                if (m_tConf.m_tStatus.IsSpecMixing())
                {
                    // xsl [9/27/2006] 请求一次下级会议混音状态供mcs界面刷新
                    OnGetMixParamToSMcu(&cTmpMsg);
                }
                else //开启下级mcu的会议讨论
                {
                    u8 byDepth = MAXNUM_MIXER_DEPTH;
				    cTmpMsg.CatMsgBody( &byDepth, sizeof(byDepth) );
				    OnStartMixToSMcu(&cTmpMsg);
                }				
			}     
			
			if( VCS_CONF == m_tConf.GetConfSource() )
			{
				// [9/22/2011 liuxu] 通知下级是否静哑音
				VCSMTMute(tMt, m_cVCSConfStatus.IsRemMute(), VCS_AUDPROC_MUTE);
				VCSMTMute(tMt, m_cVCSConfStatus.IsRemSilence(), VCS_AUDPROC_SILENCE);

				if (m_cVCSConfStatus.GetConfSpeakMode() == CONF_SPEAKMODE_ANSWERINSTANTLY )
				{
					u8 bySpeakMode = CONF_SPEAKMODE_ANSWERINSTANTLY;
					cTmpMsg.SetMsgBody( &bySpeakMode,sizeof(bySpeakMode) );
					cTmpMsg.SetEventId( MCU_MCU_SPEAKERMODE_NOTIFY );
					SendMsgToMt( tMt.GetMtId(),MCU_MCU_SPEAKERMODE_NOTIFY,cTmpMsg );
				}
			}	
		}

		BOOL32 bIsMultiCastMt = IsMultiCastMt(tMt.GetMtId());
		
		// vrs新录播开通道成功后不直接建交换，需根据情况做后续处理
		if (tMt.GetMtType() == MT_TYPE_VRSREC)
		{
			TRecChnnlStatus tVrsChlStatus = m_ptMtTable->GetRecChnnlStatus(tMt.GetMtId());
			// 状态为呼叫时，才做以下处理，防止执行多次
			if (tVrsChlStatus.m_byType == TRecChnnlStatus::TYPE_RECORD)
			{
				BOOL32 bStartVrsRec = TRUE;
				if (tVrsChlStatus.m_byState != TRecChnnlStatus::STATE_CALLING)
				{
					// 非呼叫状态进入，不做开启处理，保证只开启一次
					bStartVrsRec = FALSE;
				}
				TLogicalChannel tTmpLogicChanl;
				if (!m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_AUDIO, &tTmpLogicChanl, TRUE ))
				{
					bStartVrsRec = FALSE;
				}
				if (!m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tTmpLogicChanl, TRUE ))
				{
					bStartVrsRec = FALSE;
				}
				if (!m_tDoubleStreamSrc.IsNull())
				{
					if (m_ptMtTable->GetRecSrc(tMt.GetMtId()).IsNull())
					{
						// 会议录像
						if (!m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tTmpLogicChanl, TRUE ))
						{
							bStartVrsRec = FALSE;
						}
					}
					else
					{
						if (GetLocalMtFromOtherMcuMt(m_tDoubleStreamSrc) == GetLocalMtFromOtherMcuMt(m_ptMtTable->GetRecSrc(tMt.GetMtId())))
						{
							// 终端录像
							if (!m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tTmpLogicChanl, TRUE ))
							{
								bStartVrsRec = FALSE;
							}
						}
					}
				}
				if (bStartVrsRec)
				{
					// 录像通道
					TStartRecMsgInfo tStartVrsMsgInfo;
					if (PrepareStartRecMsgInfo(tMt, tStartVrsMsgInfo))
					{
						StartVrsRec(tStartVrsMsgInfo, TRecChnnlStatus::STATE_RECREADY);
					}
					else
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMtMcuOpenLogicChnnlRsp] PrepareStartRecMsgInfo error, Cann't start vrs recorder!\n");
					}
				}
			}
			else if (tVrsChlStatus.m_byType == TRecChnnlStatus::TYPE_PLAY)
			{
				// 放像通道，mcu不会主动请求打开vrs新录播前向通道
				return;
			}
			else
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMtMcuOpenLogicChnnlRsp] tMt(%d,%d) RecChnnlStatus.%d is invalid, Cann't start vrs recorder!\n",
					tMt.GetMcuIdx(), tMt.GetMtId(), tVrsChlStatus.m_byType);
				ReleaseVrsMt(tMt.GetMtId());//释放该vrs实体
				return;
			}
		}

		//视频交换措施
		if( tLogicChnnl.GetMediaType() == MODE_VIDEO && byChannel == LOGCHL_VIDEO )
		{
            //zbq[04/22/2009] 主流 依赖通道动态刷新其群组位置
//            RefreshRcvGrp(tMt.GetMtId());

			//20110913 zjl 等待终端第一路通道打开后再尝试打开其第二路双流通道
			if ((!m_tDoubleStreamSrc.IsNull() || !GetSecVidBrdSrc().IsNull()) &&
				!m_ptMtTable->IsLogicChnnlOpen(tMt.GetMtId(), LOGCHL_SECVIDEO, TRUE) &&
				m_tConf.GetBitRate() >= 64)
			{
				TLogicalChannel tDVLogicalChnnl;
				BOOL32 bOpenSec = FALSE;
				if ( TYPE_MCUPERI == m_tDoubleStreamSrc.GetType() &&
					EQP_TYPE_RECORDER == m_tDoubleStreamSrc.GetEqpType() )
				{
					tDVLogicalChnnl.SetChannelType( m_tPlayEqpAttrib.GetDVideoType() );
					tDVLogicalChnnl.SetSupportH239( m_tConf.GetCapSupport().IsDStreamSupportH239() );
					u16 wFileDSW = 0; 
					u16 wFileDSH = 0;
					u8  byFileDSType = 0;
					m_tPlayFileMediaInfo.GetDVideo(byFileDSType, wFileDSW, wFileDSH);
					u8 byFileDSRes = GetResByWH(wFileDSW, wFileDSH);
					tDVLogicalChnnl.SetVideoFormat(byFileDSRes);
					bOpenSec = TRUE;
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlRsp:Rec] Try to Open Mt.%d DVchn after MVChn opened!\n", tMt.GetMtId());
				}
				else if(m_ptMtTable->GetMtLogicChnnl(m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tDVLogicalChnnl, FALSE))
				{
					bOpenSec = TRUE;
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlRsp:MT] Try to Open Mt.%d DVchn after MVChn opened!\n", tMt.GetMtId());
				}
				else if(m_ptMtTable->GetMtLogicChnnl(GetSecVidBrdSrc().GetMtId(), LOGCHL_VIDEO, &tDVLogicalChnnl, FALSE))
				{
					bOpenSec = TRUE;
					//设置通道是否带有H239标签
					TCapSupport tCapSupport;
					if (m_ptMtTable->GetMtCapSupport(tMt.GetMtId(), &tCapSupport))
					{
						tDVLogicalChnnl.SetSupportH239(tCapSupport.IsDStreamSupportH239());
					}
					else
					{
						tDVLogicalChnnl.SetSupportH239(m_tConf.GetCapSupport().IsDStreamSupportH239());
					}
					
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlRsp:MT] Try to Open Mt.%d DVchn after MVChn opened!\n", tMt.GetMtId());
				}
				else
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlRsp] GetMtLogicChnnl fail!\n");
				}	
				// vrs新录播支持,区分会议录像与终端录像
				if (bOpenSec)
				{
					if (tMt.GetMtType() == MT_TYPE_VRSREC && m_ptMtTable->GetRecChlType(tMt.GetMtId()) == TRecChnnlStatus::TYPE_RECORD)
					{
						if (m_ptMtTable->GetRecSrc(tMt.GetMtId()).IsNull())
						{
							TVideoStreamCap tDSCap;
							if (m_cRecAdaptMgr.GetRecVideoCapSet(MODE_SECVIDEO, tDSCap))
							{
								tDVLogicalChnnl.SetChannelType(tDSCap.GetMediaType());
								tDVLogicalChnnl.SetVideoFormat(tDSCap.GetResolution());
							}
						}
						else
						{
							//终端录像，判双流源是否是录像终端
							if (!(m_tDoubleStreamSrc == m_ptMtTable->GetRecSrc(tMt.GetMtId())))
							{
								bOpenSec = FALSE;
							}
						}
					}
				}
				if (bOpenSec)
				{
					McuMtOpenDoubleStreamChnnl(tMt, tDVLogicalChnnl);	
				}							 			
			}
			else
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlRsp] bSecSrcIsNull.%d, bDstMTDVOpened.%d, ConfBR.%d\n",
					m_tDoubleStreamSrc.IsNull(),
					m_ptMtTable->IsLogicChnnlOpen(tMt.GetMtId(), LOGCHL_SECVIDEO, TRUE),
					m_tConf.GetBitRate());
			}
			
			// vrs新录播不在此处建交换
			if (tMt.GetMtType() == MT_TYPE_VRSREC)
			{
				return;
			}

            // xsl [7/21/2006] 告诉其从组播地址接收
            /*if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {                
                ChangeSatDConfMtRcvAddr(tMt.GetMtId(), LOGCHL_VIDEO);
            }*/
			if (m_tConf.GetConfAttrb().IsUseAdapter()
				&& ( m_ptMtTable->GetMtType(tMt.GetMtId() ) == MT_TYPE_SMCU || 
						!(GetLocalVidBrdSrc() == tMt) )
				)
			{
				//是否需要适配
				if (IsNeedAdapt(GetLocalVidBrdSrc(), tMt, MODE_VIDEO))
				{
					// [2013/01/31 chenbing] 当前上线终端为卫星终端不过适配
					// 当前上线终端为非卫星终端过适配
					if (m_tConf.m_tStatus.IsVidAdapting())
					{
                        if(!bIsMultiCastMt )
                        {
                            //1.开启则刷参数
                            RefreshBrdBasParamForSingleMt(tMt.GetMtId(), MODE_VIDEO);
						    StartSwitchToSubMtFromAdp(tMt.GetMtId(), MODE_VIDEO);
                        }
					}		
					else
					{
						StartBrdAdapt(MODE_VIDEO);
					}
				}
			}

			if( !tLocalVidBrdSrc.IsNull() )
			{
				// xliang [3/19/2009] 广播源不是老VMP，则建普通交换
				BOOL32 bNewVmpBrd = FALSE;
				if(tLocalVidBrdSrc == m_tVmpEqp)
				{
					// 普通终端需要调码率，卫星终端则不需要 [pengguofeng 1/14/2013]
					//　凡是能收的卫星终端，也需要让VMP调码率
					if ( !bIsMultiCastMt
						|| IsCanSatMtReceiveBrdSrc(tMt))
					{
						TPeriEqpStatus tPeriEqpStatus; 
						g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
						u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
						if(byVmpSubType == VMP) 
						{
							AdjustVmpBrdBitRate(&tMt);
						}
						else
						{
							//8000E-vmp 也建普通交换
							bNewVmpBrd = TRUE;
							AdjustVmpBrdBitRate();//songkun,20110527,呼叫低速终端，VMP广播降码率
						}
					}
				}
				if( bNewVmpBrd &&
					(	VCS_CONF != m_tConf.GetConfSource() ||					
						( ( m_byCreateBy == CONF_CREATE_MT ||
							!(tMt == m_tConf.GetChairman() ) ||
							m_cVCSConfStatus.GetCurVCMT().IsNull() ||							
							( VCS_GROUPROLLCALL_MODE != m_cVCSConfStatus.GetCurVCMode() && 
							  VCS_GROUPCHAIRMAN_MODE != m_cVCSConfStatus.GetCurVCMode() )	
							  // 2011-9-28 comment by pgf : 开启本地轮询且广播源是VMP时，此时有新终端入会，应该打VMP的交换给它，而不是走下面的广播
							) /*&& VCS_POLL_STOP == m_cVCSConfStatus.GetChairPollState()*/ ) 
						)
					)
				{
					// 非卫星组播，收交换；卫星终端则去组播地址 [pengguofeng 1/14/2013]
					//能收的卫星终端也走建交换的路子
					if ( !bIsMultiCastMt 
						|| IsCanSatMtReceiveBrdSrc(tMt))
					{
						SwitchNewVmpToSingleMt(tMt);
					}
				}
				else
				{
					if (CanMtRecvVideo(tMt, tLocalVidBrdSrc))
					{
						//添加丢包重传终端
						TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
						if( tConfAttrb.IsResendLosePack())
						{
							u8 byPrsId = 0;
							u8 byPrsChnId = 0;
							if (!(tMt == m_tCascadeMMCU) &&
								FindPrsChnForBrd(MODE_VIDEO, byPrsId, byPrsChnId) &&
								IsPrsChnStart(byPrsId, byPrsChnId))
							{
								AddRemovePrsMember(tMt.GetMtId(), byPrsId, byPrsChnId, MODE_VIDEO, TRUE);
							}
						}
					}

					bySrcChnnl = (tLocalVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
                    //卫星会议中非组播终端建交换
					if ( !bIsMultiCastMt
						|| IsCanSatMtReceiveBrdSrc(tMt))
					{
						// zbq[12/18/2008] 直接走广播交换是有问题的.为什么这样修改？增加过滤之.
						// 有广播源应走广播交换, zgc, 2008-03-06
						//StartSwitchToSubMt( m_tVidBrdSrc, bySrcChnnl, tMt.GetMtId(), MODE_VIDEO );
						// [pengjie 2010/4/15] 多回传支持，发言人和画面合成广播区分
						if (!IsNeedAdapt(GetLocalVidBrdSrc(), tMt, MODE_VIDEO))
						{
							tSpeaker = m_tConf.GetSpeaker();
							u16 wSpyPort = SPY_CHANNL_NULL;
							CRecvSpy tSpyResource;	
							if( !(tLocalVidBrdSrc == m_tPlayEqp) && !tSpeaker.IsLocal() &&
								m_cSMcuSpyMana.GetRecvSpy( tSpeaker, tSpyResource )
								)
							{
								wSpyPort = tSpyResource.m_tSpyAddr.GetPort();
							}
							if( !tSpeaker.IsNull() && !(tSpeaker == tMt) &&
								!(tLocalVidBrdSrc.GetType() == TYPE_MCUPERI && tLocalVidBrdSrc.GetEqpType() == EQP_TYPE_BAS) )
							{
								StartSwitchFromBrd( tSpeaker, bySrcChnnl, 1, &tMt,wSpyPort );
							}
							else
							{
								StartSwitchFromBrd( tLocalVidBrdSrc, bySrcChnnl, 1, &tMt );
							}
						}
                        else
                        {
                            NotifyMtReceive(tLocalVidBrdSrc, tMt.GetMtId());
                        }
					}
					else
					{
						if( !bIsMultiCastMt )
						{
							NotifyMtReceive( tLocalVidBrdSrc, tMt.GetMtId() );
						}
						else
						{
							NotifyMtReceive( tMt, tMt.GetMtId() );
						}					
					}					
				}
				//如果主席终端掉线又上线，视频在这里处理
				if( VCS_CONF == m_tConf.GetConfSource() &&
					tMt == m_tConf.GetChairman() &&
					( VCS_GROUPROLLCALL_MODE == m_cVCSConfStatus.GetCurVCMode() || 
						VCS_GROUPCHAIRMAN_MODE == m_cVCSConfStatus.GetCurVCMode() ) &&
						m_byCreateBy != CONF_CREATE_MT &&
						VCS_POLL_STOP == m_cVCSConfStatus.GetChairPollState() &&
						!m_cVCSConfStatus.GetCurVCMT().IsNull()
					)
				{
					TSwitchInfo tSwitchInfo;							
					tSwitchInfo.SetSrcMt(m_cVCSConfStatus.GetCurVCMT());
					tSwitchInfo.SetDstMt(m_tConf.GetChairman());
					tSwitchInfo.SetMode(MODE_VIDEO);							
					//主席选看当前调度终端					
					VCSConfSelMT(tSwitchInfo);												
				}
			}
			else
			{
				//通知终端收看本地
				cServMsg.SetMsgBody( (u8*)&tMt, sizeof(tMt) );
				SendMsgToMt( tMt.GetMtId(), MCU_MT_YOUARESEEING_NOTIF, cServMsg );
				//zjl 20110510 StartSwitchToAll 替换 StartSwitchToSubMt
				//自己的视频交换给自己
				//StartSwitchToSubMt( tMt, 0, tMt.GetMtId(), MODE_VIDEO );
				if( m_ptMtTable->GetManuId( tMt.GetMtId() ) != MT_MANU_KDC )
				{
					TSwitchGrp tSwitchGrp;
					tSwitchGrp.SetSrcChnl(0);
					tSwitchGrp.SetDstMtNum(1);
					tSwitchGrp.SetDstMt(&tMt);
					StartSwitchToAll(tMt, 1, &tSwitchGrp, MODE_VIDEO);
				}

				//对于VCS会议当前调度终端看主席
				if( VCS_CONF == m_tConf.GetConfSource() &&
					!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) )
				{
					GoOnSelStep(tMt, MODE_VIDEO, TRUE); 
				}
				if( !m_tDoubleStreamSrc.IsNull() && 
					m_ptMtTable->GetManuId( tMt.GetMtId() ) != MT_MANU_KDC &&
					m_ptMtTable->GetSrcSCS(tMt.GetMtId()).GetVideoMediaType() != MEDIA_TYPE_NULL )
				{
					TLogicalChannel tMvLogicalChannel;
					if (m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tMvLogicalChannel, FALSE) &&
						tMvLogicalChannel.GetFlowControl() > 0)
					{
						u16 wBitrate = 0;
						const u16 wSendBand = m_ptMtTable->GetSndBandWidth(tMt.GetMtId());
						const u16 wRcvBand = m_ptMtTable->GetRcvBandWidth(tMt.GetMtId());
						wBitrate = min (wSendBand ,  wRcvBand);
						wBitrate = GetDoubleStreamVideoBitrate(wBitrate);
						tMvLogicalChannel.SetFlowControl(wBitrate);
						cServMsg.SetMsgBody((u8*)&tMvLogicalChannel, sizeof(tMvLogicalChannel));
						SendMsgToMt(tMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
					}
				}
			}
	
			// 使用新的发言人选看函数, zgc, 2008-03-06
			if( HasJoinedSpeaker() && 
				!g_cMcuVcApp.IsBrdstVMP(m_tVmpEqp) && 
				!( m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO ) &&
				( tMt == GetLocalSpeaker() || tMt == m_tConf.GetChairman() || tMt == m_tLastSpeaker ) )
			{
				// 新发言人码流源调整逻辑, zgc, 2008-04-12
				ChangeSpeakerSrc(MODE_VIDEO, emReasonChangeSpeaker);
			}
			
			//E1 MCU需要调整源和前向通道码率[10/25/2012 chendaiwei]
			// 			TDri2E1Cfg tDri2E1Cfg[MAXNUM_SUB_MCU];
			// 			g_cMcuVcApp.GetDri2E1CfgTable(tDri2E1Cfg);
			// 			u16 wRealBandwidth = 0;
			// 			for ( u8 byCfgIndx = 0; byCfgIndx < MAXNUM_SUB_MCU; byCfgIndx++)
			// 			{
			// 				u32 dwConIpAddr = tDri2E1Cfg[byCfgIndx].GetIpAddr();
			// 				//判断MMCU(SMCU)是否在配置文件中配置[10/25/2012 chendaiwei]
			// 				if( g_cMcuVcApp.GetRealBandwidthByIp(dwConIpAddr,wRealBandwidth)
			// 					&& dwConIpAddr == m_ptMtTable->GetIPAddr(tMt.GetMtId())
			// 					&& (m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_SMCU
			// 					|| m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_MMCU))
			// 				{
			// 					m_ptMtTable->SetMtTransE1(tMt.GetMtId(),TRUE);
			// 					
			// 					u16 wActualBR = wRealBandwidth - GetAudioBitrate(m_tConf.GetMainAudioMediaType());
			// 					m_ptMtTable->SetRcvBandWidth(tMt.GetMtId(),wActualBR);
			// 					
			// 					break;
			// 				}
			// 			}

			TDri2E1Cfg tDri2E1Cfg[MAXNUM_SUB_MCU];
			g_cMcuVcApp.GetDri2E1CfgTable(tDri2E1Cfg);
			u16 wRealBandwidth = 0;
			if(m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_SMCU
			  ||m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_MMCU)
			{
				for ( u8 byCfgIndx = 0; byCfgIndx < MAXNUM_SUB_MCU; byCfgIndx++)
				{
					u32 dwConIpAddr = tDri2E1Cfg[byCfgIndx].GetIpAddr();
					//判断MMCU(SMCU)是否在配置文件中配置[10/25/2012 chendaiwei]
					if( g_cMcuVcApp.GetRealBandwidthByIp(dwConIpAddr,wRealBandwidth)
						&& dwConIpAddr == m_ptMtTable->GetIPAddr(tMt.GetMtId()))
					{
						m_ptMtTable->SetMtTransE1(tMt.GetMtId(),TRUE);
						
						u16 wActualBR = wRealBandwidth - GetAudioBitrate(m_tConf.GetMainAudioMediaType());
						m_ptMtTable->SetRcvBandWidth(tMt.GetMtId(),wActualBR);
						
						if(m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_MMCU)
						{
							continue;
						}
						
						u32 dwLocalDriIp = tDri2E1Cfg[byCfgIndx].GetDriIp();
						
						//虚端口带宽
						u16 wBandWidthEachPort[MAXNUM_E1PORT];
						memset(wBandWidthEachPort, 0,  sizeof(wBandWidthEachPort));
						
						//虚端口配置E1对数
						u8 byPortE1Num[MAXNUM_E1PORT];
						memset(byPortE1Num, 0, sizeof(byPortE1Num));
						
						for(u8 byIdx = 0; byIdx < MAXNUM_SUB_MCU; byIdx++)
						{
							//找到和该MMCU(SMCU)相同的DRI2板的所有虚口信息[10/25/2012 chendaiwei]
							if(tDri2E1Cfg[byIdx].GetDriIp() == dwLocalDriIp)
							{
								u8 byPortIdx = tDri2E1Cfg[byIdx].GetPortNO();
								wBandWidthEachPort[byPortIdx] = tDri2E1Cfg[byIdx].GetRealBandWidth();
								byPortE1Num[byPortIdx] = tDri2E1Cfg[byIdx].GetCfgE1Num();
							}
						}
						
						dwLocalDriIp = htonl(dwLocalDriIp);
						u8 byBuf[sizeof(wBandWidthEachPort)+sizeof(byPortE1Num)+sizeof(u32)] = {0};
						memcpy(&byBuf[0],&wBandWidthEachPort[0],sizeof(wBandWidthEachPort));
						memcpy(&byBuf[sizeof(wBandWidthEachPort)],&byPortE1Num[0],sizeof(byPortE1Num));
						memcpy(&byBuf[sizeof(wBandWidthEachPort)+sizeof(byPortE1Num)],&dwLocalDriIp,sizeof(dwLocalDriIp));
						
						CMessage cMsg;
						cMsg.content = &byBuf[0];
						cMsg.length = sizeof(byBuf);
						ProcAgtSvgE1BandWidthNotif(&cMsg);
						
						ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMtMcuOpenLogicChnnlRsp]SMCU<%d> connected by E1, adjust bandwidth.\n",tMt.GetMtId());
						
						break;
					}
				}
			}

		}

		//双流交换
        if (tLogicChnnl.GetMediaType() == MODE_SECVIDEO && byChannel == LOGCHL_SECVIDEO)
        {
			//是否要转发双流数据
            TLogicalChannel tH239LogicalChannel;
            
			if (!m_tConf.GetConfAttrb().IsSatDCastMode() ||
                !bIsMultiCastMt )
            {
				TMt tSrc = m_ptMtTable->GetMtSelMtByMode(tMt.GetMtId(), MODE_VIDEO2SECOND);
				if (!tSrc.IsNull())					//双选看
				{
					if (IsNeedSelAdpt(tSrc, tMt, MODE_VIDEO2SECOND))
					{
						if (!StartSelAdapt(tSrc, tMt, MODE_VIDEO2SECOND))
						{
							RestoreRcvSecMediaBrdSrc(1, &tMt);
							m_ptMtTable->RemoveMtSelMtByMode(tMt.GetMtId(), MODE_VIDEO2SECOND);
						}
					}
					else
					{
						TSwitchGrp tSwitchGrp;
						tSwitchGrp.SetSrcChnl(0);
						tSwitchGrp.SetDstMtNum(1);
						tSwitchGrp.SetDstMt(&tMt);

						StartSwitchToAll(tSrc, 1, &tSwitchGrp, MODE_VIDEO2SECOND, SWITCH_MODE_SELECT);

						TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
						if (tConfAttrb.IsResendLosePack())
						{
							BuildRtcpSwitchForSrcToDst(tMt, tSrc, MODE_VIDEO2SECOND);
						}
						
						//不过适配调第一个源的码率，主流
						AdjustFitstSrcMtBit(tMt.GetMtId(), bIsBrdVidNeedAdp);
					}
				}
				else if (!m_tSecVidBrdSrc.IsNull()					//双广播
					&& m_ptMtTable->GetMtLogicChnnl(GetSecVidBrdSrc().GetMtId(), LOGCHL_VIDEO, &tH239LogicalChannel, FALSE))
				{
					if (IsNeedAdapt(m_tSecVidBrdSrc, tMt, MODE_VIDEO2SECOND))
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "[ProcMtMcuOpenLogicChnnlRsp]tDst(%d-%d),need adapt!\n", 
							tMt.GetMcuId(), tMt.GetMtId());
					}
					else
					{
						TSwitchGrp tSwitchGrp;
						tSwitchGrp.SetSrcChnl(0);
						tSwitchGrp.SetDstMtNum(1);
						tSwitchGrp.SetDstMt(&tMt);
						g_cMpManager.StartSwitchToAll(m_tSecVidBrdSrc, 1, &tSwitchGrp, MODE_VIDEO2SECOND);

						TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
						if (tConfAttrb.IsResendLosePack() && m_tSecVidBrdSrc.GetMtId() != tMt.GetMtId())
						{
							u8 byPrsId = 0;
							u8 byPrsChnId = 0;
							if (FindPrsChnForBrd(MODE_SECVIDEO, byPrsId, byPrsChnId) && IsPrsChnStart(byPrsId, byPrsChnId))
							{
								AddRemovePrsMember(tMt.GetMtId(), byPrsId, byPrsChnId, MODE_VIDEO2SECOND, TRUE);
							}
						}
					}
				}
				else if (TYPE_MCUPERI == m_tDoubleStreamSrc.GetType()		//双流交换
					|| m_ptMtTable->GetMtLogicChnnl(m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tH239LogicalChannel, FALSE))
				{
					if (IsNeedAdapt(m_tDoubleStreamSrc, tMt, MODE_SECVIDEO))
					{
						if (m_tConf.m_tStatus.IsDSAdapting())
						{
							//1.开启则刷参数
							RefreshBrdBasParamForSingleMt(tMt.GetMtId(), MODE_SECVIDEO);
							StartSwitchToSubMtFromAdp(tMt.GetMtId(), MODE_SECVIDEO);
						}		
						else
						{
							StartBrdAdapt(MODE_SECVIDEO);
						}
					}
					else
					{						
						// zbq [07/26/2007] 录像机的双流交换依赖于其放象通道
						u8 byDSSrcChnnl = m_tDoubleStreamSrc == m_tPlayEqp ? m_byPlayChnnl : 0;
						
						TSwitchGrp tSwitchGrp;
						tSwitchGrp.SetSrcChnl(byDSSrcChnnl);
						tSwitchGrp.SetDstMtNum(1);
						tSwitchGrp.SetDstMt(&tMt);
						
						g_cMpManager.StartSwitchToAll(m_tDoubleStreamSrc, 1, &tSwitchGrp, MODE_SECVIDEO);

						TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
						if (tConfAttrb.IsResendLosePack() && m_tSecVidBrdSrc.GetMtId() != tMt.GetMtId())
						{
							u8 byPrsId = 0;
							u8 byPrsChnId = 0;
							if (FindPrsChnForBrd(MODE_SECVIDEO, byPrsId, byPrsChnId) && IsPrsChnStart(byPrsId, byPrsChnId))
							{
								AddRemovePrsMember(tMt.GetMtId(), byPrsId, byPrsChnId, MODE_SECVIDEO, TRUE);
							}
						}
					}
				}
            }

            // xsl [7/21/2006] 告诉其从组播地址接收
            if (m_tConf.GetConfAttrb().IsSatDCastMode() &&
                bIsMultiCastMt)
            {
				if ( !(tMt == m_tDoubleStreamSrc) )
				{
					ChangeSatDConfMtRcvAddr(tMt.GetMtId(), LOGCHL_SECVIDEO);
				}
            }
        }

		//音频交换措施
		if( tLogicChnnl.GetMediaType() == MODE_AUDIO )
		{
            //添加终端的音频丢包重传
			TConfAttrb tAudioConfAttrb = m_tConf.GetConfAttrb();
			if (tAudioConfAttrb.IsResendLosePack())
			{
				u8 byPrsId = 0;
				u8 byPrsChnId = 0;
				if (!(tMt == m_tCascadeMMCU) &&
					FindPrsChnForBrd(MODE_AUDIO, byPrsId, byPrsChnId) &&
					IsPrsChnStart(byPrsId, byPrsChnId))
				{
					AddRemovePrsMember(tMt.GetMtId(), byPrsId, byPrsChnId, MODE_AUDIO, TRUE);
				}
			}

			//vrs新录播开音频逻辑通道后不做建交换处理
			if (tMt.GetMtType() == MT_TYPE_VRSREC)
			{
				return;
			}

            // xsl [7/21/2006] 告诉其从组播地址接收
            if (m_tConf.GetConfAttrb().IsSatDCastMode() && 
                bIsMultiCastMt &&
				!GetAudBrdSrc().IsNull() &&
                !(tMt == GetAudBrdSrc()/*GetLocalSpeaker()*/))  // [1/24/2013 liaokang] 过滤发言人，防止发言人听自己
            {               
                ChangeSatDConfMtRcvAddr(tMt.GetMtId(), LOGCHL_AUDIO);
            }

			if( HasJoinedSpeaker() )
			{
				bySrcChnnl = (tLocalVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;

				if (m_tConf.m_tStatus.IsNoMixing() || m_tConf.m_tStatus.IsMustSeeSpeaker())
				{
                    // 顾振华 [5/29/2006]
                    tSpeaker = m_tConf.GetSpeaker();
					if ( !bIsMultiCastMt
						 || IsCanSatMtReceiveBrdSrc(tMt, MODE_AUDIO))
                    {
						//tSpeaker = m_tConf.GetSpeaker();
						// 不需要过适配，或需要过适配适配已起时，直接从广播源接收
						if(!IsNeedAdapt(tSpeaker, tMt, MODE_AUDIO) || m_tConf.m_tStatus.IsAudAdapting())
						{
							//StartSwitchToSubMtFromAdp(tMt.GetMtId(), MODE_AUDIO);
							RestoreRcvMediaBrdSrc(tMt.GetMtId(),MODE_AUDIO);
						}		
						else
						{
							StartBrdAdapt(MODE_AUDIO);
						}
						/*if (IsNeedAdapt(tSpeaker, tMt, MODE_AUDIO))
						{
							if (m_tConf.m_tStatus.IsAudAdapting())
							{
								StartSwitchToSubMtFromAdp(tMt.GetMtId(), MODE_AUDIO);
							}		
							else
							{
								StartBrdAdapt(MODE_AUDIO);
							}
						}
						else
						{
							u16 wSpyPort = SPY_CHANNL_NULL;
							CRecvSpy tSpyResource;	
							if( !(tLocalVidBrdSrc == m_tPlayEqp) && !tSpeaker.IsLocal() &&
								m_cSMcuSpyMana.GetRecvSpy( tSpeaker, tSpyResource )
								)
							{
								wSpyPort = tSpyResource.m_tSpyAddr.GetPort();
							}
							//zjl 20110510 StartSwitchToAll 替换 StartSwitchToSubMt
							//                        StartSwitchToSubMt( tSpeaker, bySrcChnnl, 
							//							tMt.GetMtId(), MODE_AUDIO,SWITCH_MODE_BROADCAST,TRUE,TRUE,FALSE,wSpyPort );
							TSwitchGrp tSwitchGrp;
							tSwitchGrp.SetSrcChnl(bySrcChnnl);
							tSwitchGrp.SetDstMtNum(1);
							tSwitchGrp.SetDstMt(&tMt);
							StartSwitchToAll(tSpeaker, 1, &tSwitchGrp, MODE_AUDIO, SWITCH_MODE_BROADCAST, TRUE, TRUE, wSpyPort);
						}*/
                    }
                    else
                    {
                        // 如果是卫星会议，则用户收听发言人                        
                    }
					
				}
				// 使用新的发言人选看函数, zgc, 2008-03-06
				if( m_tConf.m_tStatus.IsNoMixing() && 
					m_tConf.GetConfAttrb().GetSpeakerSrcMode() == MODE_BOTH &&
					( tMt == GetLocalSpeaker() || tMt == m_tConf.GetChairman() || tMt == m_tLastSpeaker ) )
				{
					//新发言人码流源调整逻辑，zgc, 2008-04-12
					ChangeSpeakerSrc(MODE_AUDIO, emReasonChangeSpeaker);
				}
			}
            
			TMtStatus tMtStatus;
			m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tMtStatus);

			// xsl [8/4/2006] 定制混音将n模式码流交换给终端(发言人除外)
			if( m_tConf.m_tStatus.IsMixing() &&
				// zjj20090911对于VCS组呼会议进行远端静音，则不交换混音器码流给下级MCU
				!((MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId())) 
				&& (/*(VCS_CONF == m_tConf.GetConfSource() && m_cVCSConfStatus.IsRemSilence())
				//lukp [02/03/2010]非VCS会议下如果是下级mcu且此mcu是静音,不做混音器到下级mcu的交换
				||*/ ((VCS_CONF != m_tConf.GetConfSource()) && tMtStatus.IsDecoderMute()))))
		
			{
                if (m_tConf.m_tStatus.IsSpecMixing())
                {
                    if ( (tMt == GetLocalSpeaker() || m_ptMtTable->IsMtInMixing( tMt.GetMtId() ) )
						&& (GetMixMtNumInGrp() < GetMaxMixNum(m_tMixEqp.GetEqpId()) ||
						(GetMixMtNumInGrp()== GetMaxMixNum(m_tMixEqp.GetEqpId()) && m_ptMtTable->IsMtInMixGrp(tMt.GetMtId()))) 
						&& !(tMt == m_cVCSConfStatus.GetVcsBackupChairMan())   //zhouyiliang 20101213 备份主席不进混音
						)
                    {
                        // guzh [7/14/2007] 这里在ChangeSpeaker里保护后，这里需要重新AddRemove一次。否则InMixing图标会不长
                        // 原来为什么 AddMixMember？
                        //AddSpecMixMember(&tMt, 1, m_ptMtTable->IsMtAutoInSpec(tMt.GetMtId())); 
						AddMixMember(&tMt, DEFAULT_MIXER_VOLUME, FALSE);
						StartSwitchToPeriEqp(tMt, 0, m_tMixEqp.GetEqpId(), 
							(MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt)),
								MODE_AUDIO, SWITCH_MODE_SELECT);
						m_ptMtTable->SetMtInMixing(tMt.GetMtId(),TRUE,m_ptMtTable->IsMtAutoInSpec(tMt.GetMtId()));
                    }
                    else
                    {
                        SwitchMixMember(&tMt, TRUE);
                    }   
                }
                else 
                {   
					TLogicalChannel tLog;
					if ( m_tConf.m_tStatus.IsAutoMixing() && 
						 !m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_AUDIO, &tLog, FALSE) )
					{
						// 超然终端上线后只听N码流
						SwitchMixMember(&tMt, TRUE);
					}
					// miaoqingsong [05/11/2011] 会议讨论模式，当终端数目超出混音器能力时，增加切换为定制混音逻辑；
					// 同时增加"添加混音成员是否成功"的判断，防止建立一些"从终端到混音器"的无效交换
                    if (GetMixMtNumInGrp() < GetMaxMixNum(m_tMixEqp.GetEqpId()) ||
						(GetMixMtNumInGrp()== GetMaxMixNum(m_tMixEqp.GetEqpId()) && 
						m_ptMtTable->IsMtInMixGrp(tMt.GetMtId())))
                    {
						
						if(  m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_AUDIO, &tLog, FALSE) &&
							AddMixMember(&tMt, DEFAULT_MIXER_VOLUME, FALSE) )
						{
							StartSwitchToPeriEqp(tMt, 0, m_tMixEqp.GetEqpId(), 
								(MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt)),
								MODE_AUDIO, SWITCH_MODE_SELECT);
						}
                    }
                    else
					{
// 						if (m_tConf.m_tStatus.GetMixerMode() != mcuVacMix)
// 						{
// 							//由智能切换为定制统一在终端的后向发送逻辑通道时处理
// 							SwitchDiscuss2SpecMix();
// 							u16 wError = ERR_MCU_DISCUSSAUTOCHANGE2SPECMIX;
// 							NotifyMcsAlarmInfo(0, wError);   // 设置错误码通知MCS
// 						}
						if (m_tConf.m_tStatus.GetMixerMode() == mcuVacMix)
						{
							StopMixing();
						}
					}
                }
            }
			else
			{
				// 对于VCS会议非混音模式下，终端听主席
				if( VCS_CONF == m_tConf.GetConfSource() &&
					!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) )
				{
					GoOnSelStep(tMt, MODE_AUDIO, TRUE); 
				}

				//lukunpeng[03/05/2010]由于没有收到逻辑通道打开回复前，添加混音成员都是失败的
				//需要在此处补添加一次
				if (m_tConf.m_tStatus.IsVACing())
				{
                    if (GetMixMtNumInGrp() < GetMaxMixNum(m_tMixEqp.GetEqpId()))
                    {
                        AddMixMember(&tMt, DEFAULT_MIXER_VOLUME, FALSE);
                    }  
				}
			}

			//如果主席终端掉线又上线，音频在这里处理
			if( VCS_CONF == m_tConf.GetConfSource() )
			{			
				if(	m_byCreateBy != CONF_CREATE_MT &&				
					ISGROUPMODE( m_cVCSConfStatus.GetCurVCMode() ) &&
					tMt == m_tConf.GetChairman() &&
					!m_tConf.m_tStatus.IsMixing() && 
					m_tMixEqp.IsNull()
				 )	
				{
					m_ptMtTable->SetMtInMixing( tMt.GetMtId(),TRUE, TRUE );
					m_ptMtTable->SetMtInMixing( GetLocalSpeaker().GetMtId(),TRUE, TRUE );
					//tianzhiyong 2010/03/21  增加开启模式参数
					BOOL32 dwStartResult = StartMixing(mcuPartMix);							
					if (!dwStartResult)
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ProcMtMcuOpenLogicChnnlRsp] Find no mixer\n");	
						cServMsg.SetMsgBody();
						cServMsg.SetErrorCode(ERR_MCU_VCS_NOMIXSPEAKER);					
						SendMsgToAllMcs(MCU_MCS_ALARMINFO_NOTIF, cServMsg);
					}				
				}
				if( m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_MT &&
					m_tConf.GetChairman().GetMtId() != tMt.GetMtId() )
				{
					VCSMTMute(tMt, m_cVCSConfStatus.IsRemMute(), VCS_AUDPROC_MUTE);					
					VCSMTMute(tMt, m_cVCSConfStatus.IsRemSilence(), VCS_AUDPROC_SILENCE);
				}
			}
		}

		if (!m_tCascadeMMCU.IsNull() &&
			m_tConfAllMtInfo.MtJoinedConf(m_tCascadeMMCU.GetMtId()) &&
			(tLogicChnnl.GetMediaType() == MODE_VIDEO ||
			 tLogicChnnl.GetMediaType() == MODE_AUDIO))
		{
			OnNtfMtStatusToMMcu(m_tCascadeMMCU.GetMtId(), tMt.GetMtId());
		}
        
		break;
    }		
	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	
	return;
}


/*====================================================================
    函数名      ：ProcMtMcuMsdRsp
    功能        ：主从决定通知
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/10/09    2.5         胡昌威         创建
====================================================================*/
void CMcuVcInst::ProcMtMcuMsdRsp( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() ); 	     
}

/*====================================================================
    函数名      ：ProcMtMcuOpenLogicChnnlReq
    功能        ：终端发来的打开逻辑通道请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/01/23    1.0         LI Yi         创建
	04/02/12    3.0         胡昌威        胡昌威
	05/12/19	4.0			张宝卿		  T120集成逻辑通道处理
	06/03/06	4.0			张宝卿		  判断双流发起源
====================================================================*/
void CMcuVcInst::ProcMtMcuOpenLogicChnnlReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt;
	u32	dwRcvIp;
	u16	wRcvPort;
	u8  byChannel = 0;
    u8  byAudioType;

    //TCapSupportEx tCapSupportEx;
	TLogicalChannel	tLogicChnnl;

	switch( CurState() )
	{
	case STATE_ONGOING:
        {
		tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
		tLogicChnnl = *( TLogicalChannel * )( cServMsg.GetMsgBody() );
		
		//被动打开逻辑通道后，主动触发的时序调整(目前暂只处理双流)
        if (MT_MCU_LOGICCHNNLOPENED_NTF == pcMsg->event)
        {
            if( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
		    {
                //sony G70SP在H263-H239时采用活动双流,且不使用令牌而使用FlowControlIndication进行发送激活或关闭 或者
                //POLYCOM7000的静态双流时，使用令牌进行发送激活或关闭
                //均在呼叫建立时即打开,之后与会期间保留通道，直至退出会议
                if( ( MT_MANU_SONY == m_ptMtTable->GetManuId(tMt.GetMtId()) && 
                      MEDIA_TYPE_H263 == m_tConf.GetCapSupport().GetDStreamMediaType() &&
                      m_tConf.GetCapSupport().IsDStreamSupportH239()) || 
                      (MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
					  MT_TYPE_MT == m_ptMtTable->GetMtType(tMt.GetMtId()) ) ||
                     MT_MANU_CHAORAN == m_ptMtTable->GetManuId(tMt.GetMtId()))
                {
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq]Special Manu(%d) Mt(%d)!!!\n",m_ptMtTable->GetManuId(tMt.GetMtId()), tMt.GetMtId());
                }
                else
                {                
                    //判断双流的发起源// xliang [10/10/2009] 
					if (CheckDualMode(tMt))
                    {       
						//对IsMMcuSpeaker判断有效性标记置False，只有每次被呼进会议的第一次判断有效
						if( m_bMMcuSpeakerValid && tMt.GetMtType() == MT_TYPE_MMCU )
						{
							m_bMMcuSpeakerValid = FALSE;
						}

                        // xliang [11/14/2008]  polycomMCU 打开双流通道，不作开启双流处理，后续flowctrl里作对应调整
						if ( (MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) ||
							MT_MANU_AETHRA == m_ptMtTable->GetManuId(tMt.GetMtId()) )
							&&
							( MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
							MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId())) )
						{
							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] PolyMcu or AethraMcu.%d open DVideo req here\n", tMt.GetMtId());
						}
						else
						{
							//UpdateH239TokenOwnerInfo( tMt );
							StartDoubleStream( tMt, tLogicChnnl );
                            ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] Chn connected ntf rcved, start ds!\n");
						}
                    }
                    else
                    {
                        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] Chn connected ntf rcved, no start ds due to DualMode.%d, tMt.%d\n",
                            m_tConf.GetConfAttrb().GetDualMode(), tMt.GetMtId());
                    }
    			}
            }
			else
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq]MT_MCU_LOGICCHNNLOPENED_NTF BUT NOT MODE_SECVIDEO,DO NOTHING!\n");
			}
            return;
        }

		// 当呼叫终端开始请求打开MCU通道时，认为该调度终端调度成功，可进行下一个终端的调度
		if ( VCS_CONF == m_tConf.GetConfSource() &&
			tMt == m_cVCSConfStatus.GetReqVCMT() &&
			!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) 
			)
		{		 	
			// 此处只针对单方调度模式处理,多方多画面等通道完全打开再做处理
			if (VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode())
			{
				KillTimer(MCUVC_VCMTOVERTIMER_TIMER);
				//yrl20131112 单方调度下，终端进电视墙会引起VCS调度该终端，故添加参数(是否假进墙)，此时认为终端非假进墙
				if (!IsMtNotInOtherHduChnnl(tMt,0,0,FALSE,FALSE) || !IsMtNotInOtherTvWallChnnl(tMt,0,0,FALSE,FALSE))
				{
					TMt tMtNull;
					tMtNull.SetNull();
					m_cVCSConfStatus.SetReqVCMT( tMtNull );
				}
				else
				{
					ChgCurVCMT(tMt);
				}
			}
			/*if( VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode() &&
				(!IsMtNotInOtherHduChnnl(tMt,0,0) || !IsMtNotInOtherTvWallChnnl(tMt,0,0)) )
			{
				TMt tMtNull;
				tMtNull.SetNull();
				m_cVCSConfStatus.SetReqVCMT( tMtNull );
				KillTimer(MCUVC_VCMTOVERTIMER_TIMER);
			}
			else
			{
				// Timer在此处需直接kill掉,非科达终端进多方多画面在下面会有相应处理
				KillTimer(MCUVC_VCMTOVERTIMER_TIMER);
				if( VCS_MULVMP_MODE != m_cVCSConfStatus.GetCurVCMode()  ||
					IsKedaMt(tMt, TRUE) )
				{
					//KillTimer(MCUVC_VCMTOVERTIMER_TIMER);
					ChgCurVCMT(tMt);
				}	
			}	*/		
		}
		
		
        //以下为正常的逻辑通道ACK/NACK处理
		if(tLogicChnnl.GetMediaType() == MODE_VIDEO)
		{
            m_ptMtTable->SetMtVideoSend(tMt.GetMtId(), FALSE);

            //  xsl [3/17/2006] 263+格式后面根据动态载荷过滤
			if (!m_tConf.GetCapSupport().IsSupportMediaType(tLogicChnnl.GetChannelType())
				// [091027]甘肃检察院支持第一路H263+能力
				/*&& MEDIA_TYPE_H263PLUS != tLogicChnnl.GetChannelType()*/)
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "Mt%d 0x%x(Dri:%d) not support video(%s), nack!\n",
					tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() , GetMediaStr(tLogicChnnl.GetChannelType()));
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}
            // 顾振华@2006.4.13 如果<64K码率，则拒绝打开逻辑通道
            if (m_tConf.GetBitRate() < 64)
            {
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "Bitrate less than 64K, open video logic chnl nack!\n");
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
            }
		}
		if(tLogicChnnl.GetMediaType() == MODE_AUDIO)
		{
            m_ptMtTable->SetMtAudioSend(tMt.GetMtId(), FALSE);

			//if(!m_tConf.GetCapSupport().IsSupportMediaType(tLogicChnnl.GetChannelType()))
			TAudioTypeDesc tAudioTypeDesc(tLogicChnnl.GetChannelType(),tLogicChnnl.GetAudioTrackNum());
			if(!m_tConfEx.IsSupportAudioMediaType(tAudioTypeDesc))
			{                                                                                                                         
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "Mt%d 0x%x(Dri:%d) not support audio(%s,%d), nack!\n",
					tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() , 
					GetMediaStr(tLogicChnnl.GetChannelType()),tLogicChnnl.GetAudioTrackNum());
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}			
		}
		if( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
		{
			if( tLogicChnnl.GetChannelType() == MEDIA_TYPE_NULL
				|| (m_tConf.GetDStreamMediaType() != tLogicChnnl.GetChannelType()
				    && m_tConf.GetCapSupportEx().GetSecDSType() != tLogicChnnl.GetChannelType()))
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq]Src:MT%d MediaType: %u, unmatched with conf, refuse!\n",tMt.GetMtId(),tLogicChnnl.GetMediaType());
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );

                return;
			}

            m_ptMtTable->SetMtVideo2Send(tMt.GetMtId(), FALSE);
			
			// xliang [12/18/2008]  双流通道分辨率匹配校验
			u8 bySrcRes = tLogicChnnl.GetVideoFormat();
			u8 byDstRes = m_tConf.GetCapSupport().GetDStreamCapSet().GetResolution();
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq]Src:MT%d MediaType: %u, Src Res: %u, Fps:%u\n", 
                tMt.GetMtId(), 
                tLogicChnnl.GetChannelType(), 
                bySrcRes, 
                tLogicChnnl.GetChanVidFPS());
			if (!IsDSResMatched(bySrcRes, byDstRes))
			{
                if (MT_MANU_TAIDE == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
                    VIDEO_FORMAT_CIF == bySrcRes)
                {
                    ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] Mt.%d's DS Format.%d adjust to Format.%d due to Tand!\n",
                        tMt.GetMtId(), bySrcRes, byDstRes);
                    tLogicChnnl.SetVideoFormat(byDstRes);
                }
                else
                {
                    ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] Mt.%d's DS Res dismatched<src.%d, dst.%d>, but still go on",
                              tMt.GetMtId(), bySrcRes, byDstRes);
                }

// 				if (MT_MANU_KDC == m_ptMtTable->GetManuId(tMt.GetMtId()))
// 				{
// 					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] keda DsSrc'Res.%d dismatched with Conf DsRes.%d, but go on!\n",
// 								bySrcRes, byDstRes);
// 				}
// 				else if (MT_MANU_TAIDE == m_ptMtTable->GetManuId(tMt.GetMtId()))
//                 {
//                     if (VIDEO_FORMAT_CIF == bySrcRes)
//                     {
//                         ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] Mt.%d's DS Format.%d adjust to Format.%d due to Tand!\n",
//                             tMt.GetMtId(), bySrcRes, byDstRes);
//                         tLogicChnnl.SetVideoFormat(byDstRes);
//                     }
//                 }
// 				// cheat 1: let it go
// 				else if(MT_MANU_AETHRA == m_ptMtTable->GetManuId(tMt.GetMtId()) ||
// 					    MT_MANU_CODIAN == m_ptMtTable->GetManuId(tMt.GetMtId()))
// 				{
// 					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] Mt.%d's DS Res dismatched<src.%d, dst.%d>, but still go on due to Eathra or codian!\n",
//                             tMt.GetMtId(), bySrcRes, byDstRes);
// 					
// 					// cheat 2: let src = dst
// 					
// 					if (VIDEO_FORMAT_CIF == bySrcRes)
//                     {
//                         ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] Mt.%d's DS Format.%d adjust to Format.%d due to Tand!\n",
//                             tMt.GetMtId(), bySrcRes, byDstRes);
//                         tLogicChnnl.SetVideoFormat(byDstRes);
//                     }
					

// 				}
//[5/4/2011 zhushengze]VCS控制发言人发双流
// 				else
// 				{
// 					ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] Mt.%d's DS res dismatched<src.%d, dst.%d>, ignore it\n", 
// 						tMt.GetMtId(), bySrcRes, byDstRes);
// 					SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
// 					return;
// 				}
			}
            
            //[10/19/2011 zhushengze]同主视频时不判断239是否匹配，同主视频时同时支持239和非239
			//同主视频需要判断239是否匹配[4/13/2012 chendaiwei]
            if( ( /*m_tConf.GetCapSupport().GetDStreamType() != VIDEO_DSTREAM_MAIN && */
                 tLogicChnnl.IsSupportH239() != m_tConf.GetCapSupport().IsDStreamSupportH239()) ||
                (tLogicChnnl.GetChannelType() != m_tConf.GetCapSupport().GetDStreamMediaType() &&
               //zbq[01/04/2009] 双双流过滤放开
                tLogicChnnl.GetChannelType() != m_tConf.GetCapSupportEx().GetSecDSType()))
            {   
                ConfPrint( LOG_LVL_WARNING, MID_MCU_CALL, "MT%d:H239(%d),ChanType(%s) not match with conf, NACK\n",
                          tMt.GetMtId(), tLogicChnnl.IsSupportH239(), GetMediaStr(tLogicChnnl.GetChannelType()));
//                 ConfLog( FALSE, "Mt%d 0x%x(Dri:%d) not support second video (%s), nack!\n",
//                     tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() , GetMediaStr(tLogicChnnl.GetChannelType()));
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }
            // 顾振华@2006.4.13 如果<64K码率，则拒绝打开逻辑通道
            if (m_tConf.GetBitRate() < 64)
            {
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "Bitrate less than 64K, open sec video logic chnl, nack!\n");
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
            }            			
		}

		//如果本端做主, 直接据掉对端T120通道打开请求
		if ( MODE_DATA == tLogicChnnl.GetMediaType() )
		{
			if ( MEDIA_TYPE_T120 == tLogicChnnl.GetChannelType() )
			{
				if ( !m_ptMtTable->IsMtIsMaster( tMt.GetMtId() ) )
				{	
					ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "Mt%d open mcu T.120 logicchannel as slave, nack it directly!\n", tMt.GetMtId() );
					SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
					return;
                }
			}
		}
		
		//加密适配
		if(tLogicChnnl.GetMediaEncrypt().GetEncryptMode()!= m_tConf.GetConfAttrb().GetEncryptMode())
		{
			if( tLogicChnnl.GetMediaType() == MODE_VIDEO || 
				tLogicChnnl.GetMediaType() == MODE_SECVIDEO || 
				tLogicChnnl.GetMediaType() == MODE_AUDIO || 
				tLogicChnnl.GetChannelType() == MEDIA_TYPE_H224 )
			{			
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "Mt%d 0x%x(Dri:%d) not encrypt, nack!\n",
					tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;	
			}
		}

		// FECType适配, zgc, 2007-10-10
        // 取消FEC支持，guzh 2008-02-19
        /*
		if ( tLogicChnnl.GetMediaType() == MODE_VIDEO 
			|| tLogicChnnl.GetMediaType() == MODE_SECVIDEO
			|| tLogicChnnl.GetMediaType() == MODE_AUDIO )
		{
			TCapSupportEx tCapSupportEx = m_tConf.GetCapSupportEx();
			u8 byLocalFECType;
			u8 byRemoteFECType;
			if ( tLogicChnnl.GetMediaType() == MODE_VIDEO )
			{
				byLocalFECType = tCapSupportEx.GetVideoFECType();
			}
			else if ( tLogicChnnl.GetMediaType() == MODE_SECVIDEO )
			{
				byLocalFECType = tCapSupportEx.GetDVideoFECType();
			}
			else
			{
				byLocalFECType = tCapSupportEx.GetAudioFECType();
			}
			byRemoteFECType = tLogicChnnl.GetFECType();

			if ( byLocalFECType != byRemoteFECType )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "Mt%d 0x%x(Dri:%d) FECType(%d), diff from local.%d, nack!\n",
					      tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId(), byRemoteFECType, byLocalFECType );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;	
			}
		}
        */

		//设置逻辑通道
		if( !m_ptMtTable->GetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort ) )
		{
			dwRcvIp = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( tMt.GetMtId() ) );
			if( dwRcvIp == 0 )
			{
                ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "Mt%d 0x%x(Dri:%d) mp not connected, nack!\n",
						 tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;			    
			}

			g_cMcuVcApp.FindMatchedMpIpForMt(m_ptMtTable->GetIPAddr(tMt.GetMtId()),dwRcvIp);

			wRcvPort = g_cMcuVcApp.AssignMtPort( tMt.GetConfIdx(), tMt.GetMtId() );
			m_ptMtTable->SetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort );

			ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] GetMtSwitchAddr for Mt.%d failed, so AssignMtPort and get Port is:%d\n",
				tMt.GetMtId(), wRcvPort);
			
		}

		//正常的逻辑通道打开处理
		tLogicChnnl.m_tRcvMediaChannel.SetIpAddr( dwRcvIp );
		tLogicChnnl.m_tRcvMediaCtrlChannel.SetIpAddr( dwRcvIp );

		if( MODE_NONE == tLogicChnnl.GetMediaType() )
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] Mt.%d open mcu logical chl.Mediatype is none.\n",tMt.GetMtId() );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}

		// vrs新录播支持，若是录像设备，拒绝开启后向通道
		if (tMt.GetMtType() == MT_TYPE_VRSREC && m_ptMtTable->GetRecChlType(tMt.GetMtId()) == TRecChnnlStatus::TYPE_RECORD)
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] VrsMt.%d open mcu logical chl.RecChlType is TYPE_RECORD.\n",tMt.GetMtId() );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}

		//zbq[09/02/2008] HD双流适配时序考虑，延后双流发起点
		BOOL32 bStartDS = FALSE;

		switch( tLogicChnnl.GetMediaType() )
		{
		case MODE_VIDEO:
			//第一路不允许以H263+方式打开
			byChannel = LOGCHL_VIDEO;
			tLogicChnnl.m_tRcvMediaChannel.SetPort( wRcvPort );
			tLogicChnnl.m_tRcvMediaCtrlChannel.SetPort( wRcvPort + 1 );

			// 通道码率超过会议码率，拒绝
            // xsl [7/28/2006] 比较时加上音频，增加点裕量，防止上级会议音频码率小于本级音频码率的情况被拒绝
            byAudioType = m_tConf.GetMainAudioMediaType();
			if( tLogicChnnl.GetFlowControl() > m_tConf.GetBitRate() + GetAudioBitrate(byAudioType) )
			{
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] remote bitrate %d, local bitrate %d. nack\n", 
                                tLogicChnnl.GetFlowControl(), 
                                m_tConf.GetBitRate() + GetAudioBitrate(byAudioType));
				return;	
			}

			// vrs新录播支持，记录放像适配Flowcontrol值，停止恢复放像时需要
			if (tMt.GetMtType() == MT_TYPE_VRSREC && m_tPlayEqp.GetMtId() == tMt.GetMtId())
			{
				m_tPlayEqpAttrib.SetMStremBR(tLogicChnnl.GetFlowControl());
			}

			// [091027]甘肃检察院支持第一路H263+能力
// 			if( MEDIA_TYPE_H263PLUS == tLogicChnnl.GetChannelType() && 
// 				MEDIA_TYPE_H263PLUS == tLogicChnnl.GetActivePayload() )
// 			{
// 				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
//                 ConfLog(FALSE, "[ProcMtMcuOpenLogicChnnlReq] ChnType.%d, ActivePayload.%d, nack\n", 
//                                 tLogicChnnl.GetChannelType(),
//                                 tLogicChnnl.GetActivePayload() );
// 				return;
// 			}

			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "Mt%d open mcu primary video logicChannel(0x%x:%d:%s)\n", 
					tMt.GetMtId(), dwRcvIp, wRcvPort, GetMediaStr( tLogicChnnl.GetChannelType() ) );
					
			break;

		case MODE_SECVIDEO:
				
			byChannel = LOGCHL_SECVIDEO;
			tLogicChnnl.m_tRcvMediaChannel.SetPort( wRcvPort + 4 );
			tLogicChnnl.m_tRcvMediaCtrlChannel.SetPort( wRcvPort + 5 );		
			
			//sony G70SP在H263-H239时采用活动双流,且不使用令牌而使用FlowControlIndication进行发送激活或关闭 或者
			//POLYCOM7000的静态双流时，使用令牌进行发送激活或关闭
			//均在呼叫建立时即打开,之后与会期间保留通道，直至退出会议
			if( ( MT_MANU_SONY == m_ptMtTable->GetManuId(tMt.GetMtId()) && 
				MEDIA_TYPE_H263 == m_tConf.GetCapSupport().GetDStreamMediaType() &&
				m_tConf.GetCapSupport().IsDStreamSupportH239()) || 
				MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) )
			{
			}
			else
			{                
                //判断双流的发起源 
				if(CheckDualMode(tMt) ||
				//对于科达MCU的双流通道无条件允许打开
				MT_MANU_KDCMCU == m_ptMtTable->GetManuId(tMt.GetMtId()))
                {
					//对IsMMcuSpeaker判断有效性标记置False，只有每次被呼进会议的第一次判断有效
					//以下的置位动作在NTF中进行
// 					if( m_bMMcuSpeakerValid && tMt.GetMtType() == MT_TYPE_MMCU )
// 					{
// 						m_bMMcuSpeakerValid = FALSE;
// 					}

                    // xsl [7/20/2006]卫星分散会议时需要考虑回传通道数
                    if (m_tConf.GetConfAttrb().IsSatDCastMode() && IsMultiCastMt(tMt.GetMtId())
						&& //IsSatMtOverConfDCastNum(tMt)
						!IsSatMtCanContinue(tMt.GetMtId(),emstartDs)
						/*IsOverSatCastChnnlNum(tMt.GetMtId())*/)
                    {
                        ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] over max upload mt num.\n");
                        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                        return;
                    }
					//延迟到connect后发起
					/*UpdateH239TokenOwnerInfo( tMt );
					bStartDS = TRUE;
					StartDoubleStream( tMt, tLogicChnnl );*/
                }
                else
                {

                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mt%d open mcu second video logicChannel(0x%x:%d:H239.%d:%s.Fps:%d) with wrong Dualmode, NACK !\n",
                        tMt.GetMtId(), dwRcvIp, wRcvPort + 4, tLogicChnnl.IsSupportH239(), GetMediaStr(tLogicChnnl.GetChannelType()), tLogicChnnl.GetChanVidFPS());
                    return;	
                }
			}

			// zbq [09/12/2008] Mtadp适配取不到帧率，此处保护为本地
			// zjl [10/21/2010] 目前打开对端通道都有帧率
// 			if (tLogicChnnl.GetChanVidFPS() > 25)
// 			{
// 				if (MEDIA_TYPE_H264 == tLogicChnnl.GetChannelType())
// 				{
// 					tLogicChnnl.SetChanVidFPS(m_tConf.GetDStreamUsrDefFPS());
// 				}
// 				else
// 				{
// 					tLogicChnnl.SetChanVidFPS(m_tConf.GetDStreamFrameRate());
// 				}
// 			}
            // zbq [09/05/2007] FIXME: H264双流的分辨率暂不考虑，后续处理
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mt%d open mcu second video logicChannel([address]0x%x:%d, [H239]%d, [type]%s, [Res]%s, [Fps]%d), Dualmode: %d\n", 
                tMt.GetMtId(), 
                dwRcvIp, wRcvPort + 4, 
                tLogicChnnl.IsSupportH239(), 
                GetMediaStr(tLogicChnnl.GetChannelType()), 
                GetResStr(tLogicChnnl.GetVideoFormat()),
                tLogicChnnl.GetChanVidFPS(),                    
                m_tConf.GetConfAttrb().GetDualMode() );			
			break;

		case MODE_AUDIO:
			byChannel = LOGCHL_AUDIO;
			tLogicChnnl.m_tRcvMediaChannel.SetPort( wRcvPort + 2 );
			tLogicChnnl.m_tRcvMediaCtrlChannel.SetPort( wRcvPort + 3 );
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mt%d open mcu audio logicChannel(0x%x:%d:%s)\n", 
				tMt.GetMtId(), dwRcvIp, wRcvPort+2, GetMediaStr( tLogicChnnl.GetChannelType() ) );
			// [pengjie 2011/12/17] 呼叫电话终端支持，在开其音频通道时，就认为调度ok
			if( IsPhoneMt(tMt) )
			{
				KillTimer(MCUVC_VCMTOVERTIMER_TIMER);
				
				if( VCS_CONF == m_tConf.GetConfSource() && 
					( VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode() ||
					(m_byCreateBy != CONF_CREATE_MT && VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode() ) ))
				{
					ChgCurVCMT( tMt );
				}
				
				if( VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode() )
				{
					ChangeVmpStyle(tMt, TRUE);
				}

				// MCS 自动画面合成,调整合成成员
				if (MCS_CONF == m_tConf.GetConfSource())
				{
					ChangeVmpStyle(tMt, TRUE);//调整自动画面合成
					//终端开通道调整模版开启的会控指定画面合成
					AdjustCtrlModeVmpModuleByMt(tMt);
				}

			}
			break;
			
		case MODE_DATA:
			if( tLogicChnnl.GetChannelType() == MEDIA_TYPE_H224 )
			{
				byChannel = LOGCHL_H224DATA;
				dwRcvIp = g_cMcuVcApp.GetMtAdpIpAddr( tMt.GetDriId() );
				tLogicChnnl.m_tRcvMediaChannel.SetIpAddr( dwRcvIp );
				tLogicChnnl.m_tRcvMediaChannel.SetPort( wRcvPort + 6 );
				tLogicChnnl.m_tRcvMediaCtrlChannel.SetPort( wRcvPort + 7 );
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mt%d open mcu H.224 logicChannel(0x%x:%d:%s)\n", 
					tMt.GetMtId(), dwRcvIp, wRcvPort + 6 , GetMediaStr( tLogicChnnl.GetChannelType() ) );
				
			}
			//对端请求打开本端的T120数据通道
			else if( tLogicChnnl.GetChannelType() == MEDIA_TYPE_T120 )
			{
				//此时, 本端只能是被呼. 对端如果是KEDA MCU, 其地址应该为空
				//填写本端的DCS监听地址, 回复ACK, 由对端主MCU发起邀请
				if ( !m_ptMtTable->IsNotInvited( tMt.GetMtId() ) )
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mt%d should be calling in !\n", tMt.GetMtId() );
					return;
				}
				else
				{
					byChannel = LOGCHL_T120DATA;
					
					//取本端DCS地址和端口
					u32 dwDcsIp  = g_cMcuVcApp.m_atPeriDcsTable[m_byDcsIdx-1].m_dwDcsIp;
					u16 wDcsPort = g_cMcuVcApp.m_atPeriDcsTable[m_byDcsIdx-1].m_wDcsPort;

					if ( 0 != dwDcsIp && 0 != wDcsPort )
					{
						tLogicChnnl.m_tRcvMediaChannel.SetIpAddr( dwDcsIp );
						tLogicChnnl.m_tRcvMediaChannel.SetPort( wDcsPort );

						//此处, 可以认为T120逻辑通道打开成功
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mcu open Mt%d data T.120 logicChannel(0x%x:%d:%s) as slave success!\n", 
							tMt.GetMtId(), dwDcsIp, wDcsPort, GetMediaStr( tLogicChnnl.GetChannelType() ) );		
					}
					else
					{
						ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Get local DCS Ip failed in open logic channel ack !\n" );
						return;
					}
				}				
			} 
			else if( tLogicChnnl.GetChannelType() == MEDIA_TYPE_MMCU )
			{			
				if( !m_tConf.GetConfAttrb().IsSupportCascade() ||
					tMt.GetMtType() != MT_TYPE_MMCU || 
					m_tConfAllMcuInfo.GetSMcuNum() >= MAXNUM_SUB_MCU)
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "Mt%d 0x%x(Dri:%d) not support mmcu(%s), nack!\n",
						tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() , GetMediaStr(tLogicChnnl.GetChannelType()));
					SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
					return;	
				}
                else if ( !m_tCascadeMMCU.IsNull() )
                {
                    // guzh [4/12/2007] 如果已经有上级MCU，则检查是否就是该MCU
                    // 这里不能绝对采用IP比较，上级可能换接入板。只能首先尝试别名或者E164（有可能会发生误判)
                    BOOL32 bRejectAndRemoveMMcu = FALSE;
					BOOL32 bJoinedConf = FALSE;
                    if( m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
                    {
						bJoinedConf = TRUE;
                        TMtAlias tMtAlias;
                        TMtAlias tMMcuAlias;
                        if ( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH323ID, &tMtAlias) &&
                             m_ptMtTable->GetMtAlias( m_tCascadeMMCU.GetMtId(), mtAliasTypeH323ID, &tMMcuAlias) )
                        {
                            if ( tMtAlias == tMMcuAlias )
                            {
                                bRejectAndRemoveMMcu = TRUE;
                            }
                        }
                        else if ( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeE164, &tMtAlias) &&
                                  m_ptMtTable->GetMtAlias( m_tCascadeMMCU.GetMtId(), mtAliasTypeE164, &tMMcuAlias) )
                        {
                            if ( tMtAlias == tMMcuAlias )
                            {
                                bRejectAndRemoveMMcu = TRUE;
                            }
                        }
                        else if ( m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeTransportAddress, &tMtAlias) &&
                                  m_ptMtTable->GetMtAlias( m_tCascadeMMCU.GetMtId(), mtAliasTypeTransportAddress, &tMMcuAlias) )
                        {
                            if ( tMtAlias == tMMcuAlias )
                            {
                                bRejectAndRemoveMMcu = TRUE;
                            }
                        }
                    }
                    if (bRejectAndRemoveMMcu && g_cMcuVcApp.IsAutoDetectMMcuDupCall())
                    {
                        // 如果发生这种情形,主动把该上级MCU挂断(很可能是残留) ,然后等待对端重呼
                        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "Mt%d 0x%x(Dri:%d) calling in seems to be the MMCU, Drop both!",
                                tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId());

                        SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);
                        SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);   
                    }
                    else
                    {
						if (bJoinedConf)
						{
							ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] Has current MMcu, del it!\n");
							RemoveJoinedMt(tMt, TRUE);
						}
                        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mt%d 0x%x(Dri:%d) calling in, but current has MMCU, nack!\n",
                            tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() );
					    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );            
                    }
                    return;
                }

				dwRcvIp = g_cMcuVcApp.GetMtAdpIpAddr( tMt.GetDriId() );
				tLogicChnnl.m_tRcvMediaChannel.SetIpAddr( dwRcvIp );
				tLogicChnnl.m_tRcvMediaChannel.SetPort( 3337 );
				byChannel =  LOGCHL_MMCUDATA;
				//对于上级MCU，将作为非受邀终端，断开后将自动删除出列表
				m_ptMtTable->SetNotInvited( tMt.GetMtId(), TRUE );
				
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mt%d open mcu mmcu logicChannel(0x%x:%d:%s)\n", 
					tMt.GetMtId(), dwRcvIp, wRcvPort+9, GetMediaStr( tLogicChnnl.GetChannelType() ) );

				//本终端是否已经通过密码验证
				if( m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
				{
					m_tCascadeMMCU = tMt;
					m_tConfAllMtInfo.m_tMMCU = tMt;
                    m_tConfInStatus.SetNtfMtStatus2MMcu(TRUE);
					m_ptMtTable->SetMtType(tMt.GetMtId(), MT_TYPE_MMCU);
					
					//将此级联mcu信息进行添加置列表中
					u16 wMcuIdx = INVALID_MCUIDX;
					u8 abyMcuId[MAX_CASCADEDEPTH-1];
					memset( &abyMcuId[0],0,sizeof(abyMcuId) );
					abyMcuId[0] = tMt.GetMtId();
					m_tConfAllMcuInfo.AddMcu( &abyMcuId[0],2,&wMcuIdx );
					m_ptConfOtherMcTable->AddMcInfo( wMcuIdx );	
					m_tConfAllMtInfo.AddMcuInfo( wMcuIdx/*tMt.GetMtId()*/, tMt.GetConfIdx() );

					//通知会控
					cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
					if(m_tConf.HasConfExInfo())
					{
						u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
						u16 wPackDataLen = 0;
						PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
						cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);
					}
					SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
					
					//发给会控会议所有终端信息
					//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
					//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
					SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

					//下级不再自动将上级作为发言人
					if(m_tConf.GetSpeaker().IsNull() && g_cMcuVcApp.IsMMcuSpeaker())
					{
						ChangeSpeaker(&m_tCascadeMMCU);
					}
				}

			}
			break;
		default:
			ConfPrint( LOG_LVL_WARNING, MID_MCU_CALL,"Mt.%d open mcu logical chl.mediatype(%d)\n",tMt.GetMtId(),tLogicChnnl.GetMediaType() );
			break;
		}

		
        
        // zbq [10/11/2007] 前向纠错动态载荷类型增加 音频/视频/双流 支持
        BOOL32 bLogicChanFEC = FALSE;
        if ( tLogicChnnl.GetFECType() != FECTYPE_NONE )
        {
            bLogicChanFEC = TRUE;
            tLogicChnnl.SetActivePayload(MEDIA_TYPE_FEC);
            ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "Mt.%d set ActivePayload.%d due to FEC\n", tMt.GetMtId(), MEDIA_TYPE_FEC);
        }

		//不加密模式并且mcu为主下,h263plus、H239(H263＋/H264)和h264都需要动态载荷，应强制发送源按照mcu约定设置其动态载荷
		//但考虑到Polycom7000可能支持H263双流，强制后导致误判 以及 即使强制也可能不予理睬（如sony），故不强制
		//对于h263plus的非标准动态载荷值在 码流“归一整理”中加以规范控制
		if(!m_ptMtTable->IsMtIsMaster(tMt.GetMtId())&& 
            // guzh zbq [07/25/2007] 终端回馈动态载荷 逻辑修正
            (tLogicChnnl.GetMediaEncrypt().GetEncryptMode() != CONF_ENCRYPTMODE_NONE ||
             MEDIA_TYPE_H264     == tLogicChnnl.GetChannelType() || 
             MEDIA_TYPE_H263PLUS == tLogicChnnl.GetChannelType() ||
             // guzh [9/26/2007] G7221.C 需要支持 
             MEDIA_TYPE_G7221C   == tLogicChnnl.GetChannelType() ||
             // guzh [02/20/2008] AAC支持
             MEDIA_TYPE_AACLC   == tLogicChnnl.GetChannelType() ||
             MEDIA_TYPE_AACLD   == tLogicChnnl.GetChannelType()
            ) 
          )
		{
			tLogicChnnl.SetMediaEncrypt(m_tConf.GetMediaKey());
            if (!bLogicChanFEC)
            {
                tLogicChnnl.SetActivePayload(GetActivePayload(m_tConf, tLogicChnnl.GetChannelType()));
            }
		}
        
		if(tLogicChnnl.GetMediaType() == MODE_VIDEO)
		{
			TDri2E1Cfg tDri2E1Cfg[MAXNUM_SUB_MCU];
			g_cMcuVcApp.GetDri2E1CfgTable(tDri2E1Cfg);
			u16 wRealBandwidth = 0;

			if(m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_SMCU
			  || m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_MMCU)
			{
				for ( u8 byCfgIndx = 0; byCfgIndx < MAXNUM_SUB_MCU; byCfgIndx++)
				{
					u32 dwConIpAddr = tDri2E1Cfg[byCfgIndx].GetIpAddr();
					//判断MMCU(SMCU)是否在配置文件中配置[10/25/2012 chendaiwei]
					if( g_cMcuVcApp.GetRealBandwidthByIp(dwConIpAddr,wRealBandwidth)
						&& dwConIpAddr == m_ptMtTable->GetIPAddr(tMt.GetMtId()))
					{
						m_ptMtTable->SetMtTransE1(tMt.GetMtId(),TRUE);
						
						u16 wActualBR = wRealBandwidth - GetAudioBitrate(m_tConf.GetMainAudioMediaType());
						if( wActualBR < tLogicChnnl.GetFlowControl())
						{
							tLogicChnnl.SetFlowControl(wActualBR);
							
							ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMtMcuOpenLogicChnnlReq]Adjust SMCU<%d> RL from %d to %d\n",tMt.GetMtId(),tLogicChnnl.GetFlowControl(),wActualBR);
						}
						
						break;
					}
				}
			}
		}

		//保存
		m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), byChannel, &tLogicChnnl, FALSE );
		ConfPrint(LOG_LVL_DETAIL,MID_MCU_MT2,"[ProcMtMcuOpenLogicChnnlReq]tLogicChnnl.GetMediaType(): %d, tLogicChnnl.GetChannelType(): %d\n",
			tLogicChnnl.GetMediaType(), tLogicChnnl.GetChannelType());
	
		BOOL32 bIsCanInVmp = TRUE;
		if( VCS_CONF == m_tConf.GetConfSource() && VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode() &&
			tMt == m_cVCSConfStatus.GetReqVCMT() &&
			( tLogicChnnl.GetMediaType() == MODE_VIDEO && byChannel == LOGCHL_VIDEO )
			)
		{
			//多方多画面科达终端也在此处处理,保证逻辑通道正常打开后,再ChgCurVCMT
			//终端会按能力从大到小多次请求,直到成功.
			KillTimer(MCUVC_VCMTOVERTIMER_TIMER);
			if (IsKedaMt(tMt, TRUE))
			{
				ChgCurVCMT( tMt );
			}
			//zhouyiliang 20110309 对于8000G来说，非keda>h264 cif 终端不进画面合成
			//8000A的话，先判是否还有前适配通道
			else //if( !IsKedaMt(tMt, TRUE) )
			{
				if( !IsNoneKedaMtCanInMpu(tMt) )
				{
/*#ifdef _8KE_				
					if( IsMtNotInOtherTvWallChnnl(tMt,0,0) && IsMtNotInOtherHduChnnl(tMt,0,0) )
					{
						NotifyMcsAlarmInfo(0,ERR_VMP_8000E_NOTSPPORT_NONEKEDAMT);
						VCSDropMT( tMt );
						return;
					}
					else
					{
						if( !m_cVCSConfStatus.GetMtInTvWallCanMixing() )
						{
							RemoveSpecMixMember( &tMt, 1,TRUE,TRUE );
						}
						bIsCanInVmp = FALSE;
					}
#else*/
					u8 byVmpSubType = GetVmpSubType(m_tVmpEqp.GetEqpId());
					TVmpChnnlInfo tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(m_tVmpEqp);
					//8000A(MPU/MPU2) 8000H 有前适配的vmp,8KG前适配个数为0
					// vpu同样不支持非keda>h264 cif 终端进画面合成
					if( VMP == byVmpSubType
						|| tVmpChnnlInfo.GetHDChnnlNum() >= tVmpChnnlInfo.GetMaxNumHdChnnl())
					{
						if( IsMtNotInOtherTvWallChnnl(tMt,0,0) && IsMtNotInOtherHduChnnl(tMt,0,0) )
						{
							//没有前适配通道了
							NotifyMcsAlarmInfo(0,ERR_VMP_NO_VIP_VMPCHNNL);//ERR_VMP_NONEKEDAMT_CANNOTINMPU
							VCSDropMT( tMt );
							return;
						}
						else
						{
							if( !m_cVCSConfStatus.GetMtInTvWallCanMixing() )
							{
								RemoveSpecMixMember( &tMt, 1,TRUE,TRUE );
							}
							bIsCanInVmp = FALSE;
						}
					}
//#endif
				}

				ChgCurVCMT( tMt );
			}
		}

		//zbq[09/02/2008] 双流延迟到这里发起
// 		if (MODE_SECVIDEO == tLogicChnnl.GetMediaType() && bStartDS)
// 		{
// 			StartDoubleStream( tMt, tLogicChnnl );
// 		}

        //终端上行带宽初始化
        if(tLogicChnnl.GetMediaType() == MODE_VIDEO && byChannel == LOGCHL_VIDEO)
        {
            if( m_ptMtTable->IsLogicChnnlOpen(tMt.GetMtId(),LOGCHL_AUDIO,FALSE) )
			{
				u16 wDialRate = m_ptMtTable->GetDialBitrate( tMt.GetMtId());
				u16 wFlowCtrl = tLogicChnnl.GetFlowControl();
				m_ptMtTable->SetSndBandWidth( tMt.GetMtId(),min(wDialRate,wFlowCtrl));
			}
			else
			{
				m_ptMtTable->SetSndBandWidth(tMt.GetMtId(), tLogicChnnl.GetFlowControl());
			}
        }

		if( tLogicChnnl.GetMediaType() == MODE_AUDIO && byChannel == LOGCHL_AUDIO )
		{
			if( m_ptMtTable->IsLogicChnnlOpen(tMt.GetMtId(),LOGCHL_VIDEO,FALSE) )
			{
				u16 wDialRate = m_ptMtTable->GetDialBitrate( tMt.GetMtId());
				u16 wOriginalBW = m_ptMtTable->GetSndBandWidth(tMt.GetMtId());
				m_ptMtTable->SetSndBandWidth( tMt.GetMtId(),min(wDialRate,wOriginalBW));
			}			
		}


		//[nizhijun 2010/12/21] 下级终端在下级开启双流，这时候上级会收到下级的239令还牌通告，
		//但是这时候不应该用changeprssrc去建立prs到上级各终端的RTP交换，因为这时候对上级来说
		//m_tDoubleStreamSrc是空的，不能成功建立RTP交换
		//因此，现在建立双流PRS到接收端的RTP交换应该放在startdoublestream中，因为那时候m_tDoubleStreamSrc是存在的
		//第二路视频的丢包重传
// 		if( byChannel == LOGCHL_SECVIDEO)
// 		{
// 			if(m_tConf.GetConfAttrb().IsResendLosePack())
// 			{
// 				ChangePrsSrc( tMt, PRSCHANMODE_SECOND);
// 			}
// 		}
	
        //音视频、双流打洞
        if ( cServMsg.GetMsgBodyLen()  >= sizeof(BOOL32) + sizeof(TTransportAddr) + sizeof(TLogicalChannel) &&
             ( tLogicChnnl.GetMediaType() == MODE_VIDEO ||
               tLogicChnnl.GetMediaType() == MODE_AUDIO ||
			   tLogicChnnl.GetMediaType() == MODE_SECVIDEO ))
        {
            BOOL32 bIsPinHole = *( BOOL32 * )( cServMsg.GetMsgBody() + sizeof(TLogicalChannel));
            TTransportAddr tRemoteRtpSndAddr = *( TTransportAddr * )( cServMsg.GetMsgBody() + sizeof(TLogicalChannel) + sizeof(BOOL32));
            
            if (tLogicChnnl.GetMediaType() == MODE_AUDIO)
            {
                m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tAudRtpSndTransportAddr.SetIpAddr(tRemoteRtpSndAddr.GetIpAddr());
                m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tAudRtpSndTransportAddr.SetPort(tRemoteRtpSndAddr.GetPort());
            }
            else if(tLogicChnnl.GetMediaType() == MODE_VIDEO)
            {
                m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tVidRtpSndTransportAddr.SetIpAddr(tRemoteRtpSndAddr.GetIpAddr());
                m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tVidRtpSndTransportAddr.SetPort(tRemoteRtpSndAddr.GetPort());                
            }
            else if (tLogicChnnl.GetMediaType() == MODE_SECVIDEO)
            {
                m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tSecRtpSndTransportAddr.SetIpAddr(tRemoteRtpSndAddr.GetIpAddr());
                m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tSecRtpSndTransportAddr.SetPort(tRemoteRtpSndAddr.GetPort());                                
            }
            
            if ( TRUE == bIsPinHole )
            {
                //rtp打洞
                Starth460PinHoleNotify( tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(),
                                        tLogicChnnl.m_tRcvMediaChannel.GetPort(),
                                        tRemoteRtpSndAddr.GetIpAddr(),
                                        htons(tRemoteRtpSndAddr.GetPort()),
                                        pinhole_rtp,
                                        tLogicChnnl.m_byActivePayload);
                ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] rtp PinHole Mt:%d, Local(0x%x:%d),Remote(0x%x:%d),payLoad:%d !\n", 
                           tMt.GetMtId(), 
                           tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(),
                           tLogicChnnl.m_tRcvMediaChannel.GetPort(),
                           tRemoteRtpSndAddr.GetIpAddr(),
                           htons(tRemoteRtpSndAddr.GetPort()),
                           tLogicChnnl.m_byActivePayload);
                
                //rtcp打洞
                Starth460PinHoleNotify( tLogicChnnl.m_tRcvMediaCtrlChannel.GetIpAddr(),
                                        tLogicChnnl.m_tRcvMediaCtrlChannel.GetPort(),
                                        tLogicChnnl.m_tSndMediaCtrlChannel.GetIpAddr(),
                                        tLogicChnnl.m_tSndMediaCtrlChannel.GetPort(),
                                        pinhole_rtcp,
                                        tLogicChnnl.m_byActivePayload);
                ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] rtcp PinHole Mt:%d, Local(0x%x:%d),Remote(0x%x:%d),payLoad:%d !\n", 
                           tMt.GetMtId(), 
                           tLogicChnnl.m_tRcvMediaCtrlChannel.GetIpAddr(),
                           tLogicChnnl.m_tRcvMediaCtrlChannel.GetPort(),
                           tLogicChnnl.m_tSndMediaCtrlChannel.GetIpAddr(),
                           tLogicChnnl.m_tSndMediaCtrlChannel.GetPort(),
                           tLogicChnnl.m_byActivePayload);               
                
                m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_bIsPinHole = TRUE;
            }                
        }
        
		//应答
		if(tLogicChnnl.GetMediaType() == MODE_VIDEO)
		{
			m_ptMtTable->SetMtVideoSend(tMt.GetMtId(), TRUE);			
		}
		else if(tLogicChnnl.GetMediaType() == MODE_AUDIO)
		{
			m_ptMtTable->SetMtAudioSend(tMt.GetMtId(), TRUE);
		} 
		else if(tLogicChnnl.GetMediaType() == MODE_SECVIDEO)
		{
            KillTimer(MCUVC_WAITOPENDSCHAN_TIMER);
		}

		cServMsg.SetMsgBody( ( u8 * )&tLogicChnnl, sizeof( tLogicChnnl ) );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );


		ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "when mt.%d olc(mediatype:%d) mcu, tLogicChnnl's flowcontrol is: %d; conf's br is: %d!\n",
					tMt.GetMtId(),tLogicChnnl.GetMediaType(), tLogicChnnl.GetFlowControl(), m_tConf.GetBitRate());

		TMt tNullMt;
		if( tLogicChnnl.GetMediaType() == MODE_SECVIDEO )
		{
			if(MT_MANU_AETHRA == m_ptMtTable->GetManuId(tMt.GetMtId())
				//&& MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId())
				)
			{
				// [12/11/2009 xliang] send 2 flow control cmd when Aethra OLC(DV)
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq]Send flowcontrol cmd to Mt.%u(V) due to Aethra\n", tMt.GetMtId());

				TLogicalChannel tLogicVChnnl;
				m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tLogicVChnnl, FALSE);
					
				tLogicVChnnl.SetMediaType(MODE_VIDEO);
				tLogicVChnnl.SetFlowControl(m_tConf.GetBitRate()/2);	//码率按会议折半算
				cServMsg.SetMsgBody( ( u8 * )&tLogicVChnnl, sizeof( tLogicVChnnl ) );
				u8 byDirection = 1; //outgoing:0 ;	incoming: 1
				cServMsg.CatMsgBody( &byDirection, sizeof(u8) );
				SendMsgToMt(tMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
				
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq]Send flowcontrol cmd to Mt.%u(SecV) due to Eathra\n", tMt.GetMtId());
				tLogicChnnl.SetMediaType(MODE_SECVIDEO);
				tLogicChnnl.SetFlowControl( m_tConf.GetBitRate()/2 );
				cServMsg.SetMsgBody();
				cServMsg.SetMsgBody( ( u8 * )&tLogicChnnl, sizeof( tLogicChnnl ) );
				cServMsg.CatMsgBody( &byDirection, sizeof(u8) );
				SendMsgToMt(tMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
			}
			//卫星意义下，卫星终端在发起双流之前，要先让终端停止发送双流，否则会导致计数错误
			if(IsMultiCastMt(tMt.GetMtId()))
			{
				// 记录当前卫星点数，此处初始化flowcontrol 0，不调整卫星点数
				s16 byCurSatMtNum = GetCurSatMtNum();
				CServMsg cTempServMsg;
				tLogicChnnl.SetFlowControl(0);
				cTempServMsg.SetMsgBody((u8*)&tLogicChnnl, sizeof(tLogicChnnl)); 
				SendMsgToMt(tMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cTempServMsg); 
				// 保证不调整卫星点数
				SetCurSatMtNum(byCurSatMtNum);
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_CONF,"[ProcMtMcuOpenLogicChnnlReq]DV RemoveMt(%d) SatMtNum(%d)\n",
					tMt.GetMtId(),GetCurSatMtNum());
			}
		}

		if( tLogicChnnl.GetMediaType() == MODE_VIDEO ||
			 tLogicChnnl.GetMediaType() == MODE_AUDIO ||
			 tLogicChnnl.GetMediaType() == MODE_SECVIDEO) 
		{
			if ( !m_tCascadeMMCU.IsNull() &&
				m_tConfAllMtInfo.MtJoinedConf(m_tCascadeMMCU.GetMtId()) )
			{
				OnNtfMtStatusToMMcu(m_tCascadeMMCU.GetMtId(), tMt.GetMtId());
			}			

			MtStatusChange( &tMt,TRUE );
		}
	
		//本终端可能未通过密码验证
		if( !m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mt%d doesnot join conf, then doesnot deal audio/video switch ... \n", 
					  tMt.GetMtId() );
			return;
		}

        //zbq[09/24/2007] 基于VMP主席跟随考虑，此处尝试调整主席相关交换
        if( m_tConf.HasChairman() &&  m_tConf.GetChairman() == tMt )
        {
            //AdjustChairmanSwitch();
			//zgc [04/21/2008] 音频视频分开处理
			// 音频通道
			if ( tLogicChnnl.GetMediaType() == MODE_AUDIO )
			{
				AdjustChairmanAudSwitch();
			}
			
			// 视频通道
			//卫星会议考虑，将主席恢复放到之后处理
// 			if ( tLogicChnnl.GetMediaType() == MODE_VIDEO )
// 			{
// 				AdjustChairmanVidSwitch();
// 			}
        }

		//发送视频码流处理
		if( tLogicChnnl.GetMediaType() == MODE_VIDEO && byChannel == LOGCHL_VIDEO )
		{
			//吃下码流, 转入dump，防止icmp
			g_cMpManager.StartRecvMt( tMt, MODE_VIDEO );
            
            //zbq[07/18/2007] SSRC 的修改由归一重整统一处理
			//g_cMpManager.ResetRecvMtSSRC( tMt, MODE_VIDEO );

			BOOL32 bIsStopSpeakerMtSend = FALSE;
			//若是发言人加入
			if(GetLocalSpeaker() == tMt )
			{
				u16 wBitRate = 0;
				BOOL32 bIsChangeSpeaker = TRUE;
				wBitRate = tLogicChnnl.GetFlowControl();
				tLogicChnnl.SetFlowControl( 0 );
				m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), byChannel, &tLogicChnnl, FALSE );
                // xsl [8/22/2006]卫星分散会议时需要判断回传通道数
                if (m_tConf.GetConfAttrb().IsSatDCastMode()
					&& IsMultiCastMt(tMt.GetMtId()) )
                {
					//if( IsSatMtOverConfDCastNum(tMt) ) 
					if( !IsSatMtCanContinue(tMt.GetMtId(),emSpeaker) ) 
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlReq] over max upload mt num. cancel speaker!\n");
						//ChangeSpeaker(NULL);
						bIsChangeSpeaker = FALSE;
					}   
					//tLogicChnnl.SetFlowControl( wBitRate );
					//m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), byChannel, &tLogicChnnl, FALSE );
                }
                if( bIsChangeSpeaker )
                {
					//zhouyiliang 20121016如果是恢复发言人，当前会议轮询带音频，且暂停，此时bPolling 要为TRUE，不停轮询
					BOOL32 bPolling = FALSE;
					if ( m_tConf.m_tStatus.GetPollMedia() == MODE_BOTH )
					{
						bPolling = TRUE;
					}
					// 切换发言人之前,先将会议发言人置空,防止改变发言人时以为新旧发言人相等
					m_tConf.SetSpeaker(tNullMt);
                    ChangeSpeaker( &tMt,bPolling );
                }
				tLogicChnnl.SetFlowControl( wBitRate );
				m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), byChannel, &tLogicChnnl, FALSE );
				if (!IsSpeakerCanBrdVid( &tMt ) || !bIsChangeSpeaker )
				{
					bIsStopSpeakerMtSend = TRUE;
				}
			}

			if( !( GetLocalSpeaker() == tMt ) || bIsStopSpeakerMtSend )
			{
				if( tMt.GetMtType() == MT_TYPE_MMCU || tMt.GetMtType() == MT_TYPE_SMCU )
				{				
					NotifyMtSend( tMt.GetMtId(), MODE_VIDEO, TRUE );
				}
				else
				{
					// 用于临时记录卫星点数
					s16 byCurSatMtNum = 0;
                    //  xsl [1/18/2006] 如果在vmp或tvwall模板里不发flowcontrol 0                   
                    if( (!m_tConfEqpModule.IsMtInVmp(tMt) && 
                         !m_tConfEqpModule.IsMtInTvwall(tMt) &&
						 !m_tConfEqpModule.IsMtInHduVmpChnnl(tMt)) || 
						 bIsStopSpeakerMtSend )
                    {
						// 记录当前卫星点数，此处初始化flowcontrol 0，不调整卫星点数
						byCurSatMtNum = GetCurSatMtNum();
                        NotifyMtSend( tMt.GetMtId(), MODE_VIDEO, FALSE );
						// 保证不调整卫星点数
						if (GetCurSatMtNum() > 0 && IsMultiCastMt(tMt.GetMtId()))
						{
							SetCurSatMtNum(byCurSatMtNum);
							LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_CONF,"[ProcMtMcuOpenLogicChnnlReq]RemoveMt(%d) SatMtNum(%d)\n",
								tMt.GetMtId(),GetCurSatMtNum());
						}
                    }
					else
					{
						// 在vmp或tvwall模板中，需占用卫星点数
						if (IsMultiCastMt(tMt.GetMtId()))
						{
							// 若有点数，则占用
							if (GetCurSatMtNum() > 0)
							{
								SetCurSatMtNum(GetCurSatMtNum() - 1);
								LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_CONF,"[ProcMtMcuOpenLogicChnnlReq]Mt is in ConfEqpModule,RemoveMt(%d) SatMtNum(%d)\n",
															tMt.GetMtId(),GetCurSatMtNum());
							}
							else // 若无点数，则需对卫星终端发送flowcontrol 0
							{
								// 记录当前卫星点数，此处初始化flowcontrol 0，不调整卫星点数
								byCurSatMtNum = GetCurSatMtNum();
								// 模版保存时有保护，此场景不会出现
								NotifyMtSend( tMt.GetMtId(), MODE_VIDEO, FALSE );
								// 保证不调整卫星点数
								SetCurSatMtNum(byCurSatMtNum);
								LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_CONF,"[ProcMtMcuOpenLogicChnnlReq]SatMtNum is not enough,RemoveMt(%d) SatMtNum(%d)\n",
									tMt.GetMtId(),GetCurSatMtNum());
							}
						}
					}
				}			
			}
			AdjustChairmanVidSwitch();


			//zjj20091026福建公安 如果电视墙通道中有成员信息说明该成员已经被放入该通道,
			//终端后向通道打开后要建交换
			if( m_tConf.GetConfSource() == VCS_CONF )
			{
				// [6/2/2011 liuxu] 对电视墙和hdu业务进行整合

				// 一个会议支持的电视墙通道的总数
				const u8 byMaxTvwChnnlNum = MAXNUM_HDUBRD * MAXNUM_HDU_CHANNEL + MAXNUM_PERIEQP_CHNNL * MAXNUM_MAP;
				CConfTvwChnnl acTvwChnnlFind[byMaxTvwChnnlNum];	// 待寻找的电视墙通道
				u8 byEqpId, byChnnlIdx;							// 电视墙通道对应的设备号和在设备中的通道号
				
				TMt tTvwMt;										// 待退出电视墙通道的终端
				TTvwMember tTvwMember;							// 电视墙通道成员
				
				// 获取所有电视墙通道
				const u8 byTvwChnnlNum = GetAllCfgedTvwChnnl( acTvwChnnlFind, byMaxTvwChnnlNum );
				
				// 依次遍历获取得到的每个电视墙通道进行清除工作
				for ( u8 byLoop = 0; byLoop < byTvwChnnlNum; ++byLoop )
				{
					// 稳妥点判断是否为空
					if (acTvwChnnlFind[byLoop].IsNull())
					{
						continue;
					}
					
					byEqpId = acTvwChnnlFind[byLoop].GetEqpId();
					byChnnlIdx = acTvwChnnlFind[byLoop].GetChnnlIdx();
					
					// 不在线, 则不进行操作
					if (!g_cMcuVcApp.IsPeriEqpConnected(byEqpId))
					{
						continue;
					}

					tTvwMember = acTvwChnnlFind[byLoop].GetMember();
					tTvwMt	   = (TMt)tTvwMember;
					tTvwMt     = GetLocalMtFromOtherMcuMt( tTvwMt );

					// 本级终端或mcu上线, 如果下级mcu上线,则它下面终端如果在电视墙中, 则恢复电视墙交换
					if (tTvwMt.IsNull() 
						|| tTvwMt.GetMtId() != GetLocalMtFromOtherMcuMt(tMt).GetMtId())
					{
						continue;
					}
					
					if( ( TW_MEMBERTYPE_VCSSPEC == tTvwMember.byMemberType || TW_MEMBERTYPE_VCSAUTOSPEC == tTvwMember.byMemberType ) 
						&& tTvwMember.GetConfIdx() == m_byConfIdx )
					{ 
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, 
							"[ProcMtMcuOpenLogicChnnlReq] VcsConf mt(%d.%d) in tvwall member channeltype(%d) format(%d) \n" ,
							tTvwMember.GetMcuId(), tTvwMember.GetMtId(),
							tLogicChnnl.GetChannelType(),tLogicChnnl.GetVideoFormat());
						
						// [8/19/2011 liuxu] 防止终端进入电视墙失败,却进入了混音
						if (IsValidHduChn(byEqpId, byChnnlIdx))
						{
							if( !ISGROUPMODE( m_cVCSConfStatus.GetCurVCMode() ) 
								&& !CheckHduAbility( tMt, byEqpId, byChnnlIdx) 
								&& !(tMt == m_cVCSConfStatus.GetCurVCMT())
								)
							{
								ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,  
									"!GroupMode, tMt(%d.%d) Can't switch to hdu(%d), so Drop it\n",
									tMt.GetMcuId(), tMt.GetMtId(), byEqpId);
								ClearTvwChnnlStatus(acTvwChnnlFind[byLoop].GetEqpId(), acTvwChnnlFind[byLoop].GetChnnlIdx());
								VCSDropMT( tMt );
								return;
							}
							else 
							{
								// [2013/03/11 chenbing] VCS会议不支持HDU多画面,子通道置0
								ChangeHduSwitch( &tMt, byEqpId, byChnnlIdx, 0, tTvwMember.byMemberType,
									TW_STATE_START );
							}
						}
						else
						{
							ChangeTvWallSwitch( &tTvwMt, byEqpId, byChnnlIdx, tTvwMember.byMemberType, TW_STATE_START);
						}

						if( !tTvwMember.IsLocal() )
						{
							OnMMcuSetIn( tTvwMt, m_cVCSConfStatus.GetCurSrcSsnId(), SWITCH_MODE_SELECT ); 
						}
					}
				}
			}

            //有关画面合成的处理
            if( GetVmpCountInVmpList() > 0 )
            {
				//调整自动画面合成
				if (bIsCanInVmp)
				{
					ChangeVmpStyle(tMt, TRUE);
				}

				//终端开通道调整模版开启的会控指定画面合成
				AdjustCtrlModeVmpModuleByMt(tMt);

				if( m_byCreateBy == CONF_CREATE_NPLUS)
				{
					TEqp tVmpEqp;
					u8 byVmpSubType;
					TVMPParam_25Mem tVmpParam;
					BOOL32 bAdjustVmpMember = FALSE;
					u8 byVmpId;
					u8 byChIdx;
					TMt tSpeaker = m_tConf.GetSpeaker();
					u8 byNplusMemberType;
					TVMPMember tVMPMember;
					//s8 achAlias[VALIDLEN_ALIAS+1] = {0};
                    s8 achAlias[MAXLEN_ALIAS] = {0};// 扩容 [7/4/2013 liaokang]                    
					CServMsg cMsg;
					TPeriEqpStatus tVmpPeriEqpStatus;
					for (byVmpId = VMPID_MIN; byVmpId <= VMPID_MAX; byVmpId++)
					{
						tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
						byVmpSubType = GetVmpSubType(byVmpId);
						tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);

						//VMP尚未开启成功或者自动VMP
						if(tVmpParam.GetVMPMode() == CONF_VMPMODE_NONE || IsDynamicVmp(tVmpEqp.GetEqpId()))
						{
							continue;
						}

						bAdjustVmpMember = FALSE;
						for (byChIdx = 0; byChIdx < MAXNUM_VMP_MEMBER; byChIdx++)
						{
							tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);//confvmpparam会更新
							byNplusMemberType = 0;
							if(g_cMcuVcApp.NPlusIsMtInVmpModule(tMt.GetMtId(),byChIdx,m_byConfIdx,byVmpId,byNplusMemberType))
							{
								// 在终端上线前,该通道成员已变,不能再恢复此通道成员
								tVMPMember = *tVmpParam.GetVmpMember(byChIdx);
								if (VMP_MEMBERTYPE_NULL != tVMPMember.GetMemberType()
									&& byNplusMemberType != tVMPMember.GetMemberType())
								{
									ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq][N+1]vmp member.%d membertype: %d, is not nplus module membertype%d, can't into vmp!\n", 
										byChIdx, tVMPMember.GetMemberType(), byNplusMemberType);
									
									continue;
								}
								if (!tVMPMember.IsNull() && !(tVMPMember == tMt))
								{
									ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq][N+1]vmp member.%d info: mt(%d,%d), is not openlogic mt(%d,%d), can't into vmp!\n", 
										byChIdx, tVMPMember.GetMcuId(), tVMPMember.GetMtId(), tMt.GetMcuId(), tMt.GetMtId());
									
									continue;
								}
								// 针对发言人跟随,若当前发言人已不是模版发言人终端,不恢复发言人跟随通道成员
								// 场景:模版开启时,发言人终端未上线,改变发言人又取消发言人后,此终端又上线,此终端不会变发言人,但模版发言人跟随通道仍有此终端
								if (VMP_MEMBERTYPE_SPEAKER == tVMPMember.GetMemberType() && 
									!(tMt == tSpeaker))
								{
									ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq][N+1]vmp member.%d info: SpeakerMt(%d,%d), is not openlogic mt(%d,%d), can't into vmp!\n", 
										byChIdx, tSpeaker.GetMcuId(), tSpeaker.GetMtId(), tMt.GetMcuId(), tMt.GetMtId());
									
									continue;
								}

								UpdataVmpEncryptParam(tMt, byChIdx,byVmpId);
								//这里重新加上，会在SetVmpChnnl()时，自动加入并更新到会议中
								memset(&tVMPMember, 0, sizeof(tVMPMember));
								tVMPMember.SetMemberTMt(tMt);
								tVMPMember.SetMemberType(byNplusMemberType);
								tVMPMember.SetMemStatus(MT_STATUS_AUDIENCE);
								tVmpParam.SetVmpMember(byChIdx,tVMPMember);
								g_cMcuVcApp.SetConfVmpParam(tVmpEqp, tVmpParam);
															
								// 8KE,8KH,MPU2都需要发送别名信息,目前只有vpu不需发送别名信息,mpu要支持通道根据别名显示图片
								if (byVmpSubType != VMP)
								{
									// [9/7/2010 xliang] 8000G 还需发送一次别名信息，20110331 zhouyiliang 8000H一样
									memset(achAlias, 0, sizeof(achAlias));
									if( /*GetMtAliasInVmp(tMt, achAlias)*/
                                        GetMtAliasToVmp(tMt, achAlias) ) // 扩容 [7/4/2013 liaokang]
									{
										ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq][N+1]vmp member.%d info: mt(%d,%d), alias is %s!\n", 
											byChIdx, tMt.GetMcuId(), tMt.GetMtId(), achAlias);
										
										cMsg.Init();
										cMsg.SetChnIndex(byChIdx);
										cMsg.SetMsgBody( (u8*)achAlias, sizeof(achAlias) );
										cMsg.CatMsgBody(&byChIdx, sizeof(u8));
										SendMsgToEqp(m_tVmpEqp.GetEqpId(), MCU_VMP_CHANGEMEMALIAS_CMD, cMsg);
									}
								}
								
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS, "[ProcMtMcuOpenLogicChnnlReq][N+1] tMt<%d, %d>, byChnnl:%d, mode:%d\n",
									tMt.GetMcuId(),tMt.GetMtId(), byChIdx, byNplusMemberType);
								

								//[Bug00056410]songkun,20110610,设定VMP模板，之前由于终端没上线，开启VMP合成不成功，并清掉了成员信息
								// [2/22/2010 xliang] VMP，MPU保持统一流程：先调分辨率再建交换
								if( ChangeMtVideoFormat( tMt) )
								{
									SetVmpChnnl(byVmpId, tMt, byChIdx, byNplusMemberType, TRUE );
								}

								bAdjustVmpMember = TRUE;

								//n+1备份更新vmp信息
								if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
								{
									ProcNPlusVmpUpdate();
								}  
							}
						}

						g_cMcuVcApp.NplusRemoveVmpMember( tMt.GetMtId(),m_byConfIdx,byVmpId);

						if(bAdjustVmpMember)
						{
							//同步外设复合状态
							g_cMcuVcApp.GetPeriEqpStatus( byVmpId, &tVmpPeriEqpStatus );
							tVmpPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tVmpParam);
							g_cMcuVcApp.SetPeriEqpStatus( byVmpId, &tVmpPeriEqpStatus );

						}
					}
					
				}
			}

			//判断是否为回传通道,是则恢复回传交换
			if( !m_tCascadeMMCU.IsNull() && !(m_tCascadeMMCU == tMt) )
			{
				TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
				if( ptConfMcInfo != NULL && 
					( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
				{
					if( ptConfMcInfo->m_tSpyMt.GetMtId() == tMt.GetMtId() &&
						ptConfMcInfo->m_tSpyMt.GetMcuId() == tMt.GetMcuId() )
					{
						ptConfMcInfo->m_tSpyMt.SetNull();
						//StartSwitchToMcu( tMt, 0, m_tCascadeMMCU.GetMtId(), MODE_VIDEO, SWITCH_MODE_SELECT);
						//ptConfMcInfo->m_tSpyMt = tMt;
						// 直接调用OnSetOutView处理,包含给上级发消息
						OnSetOutView(tMt, MODE_VIDEO);
					}
				}
			}

			//有电视墙模板并且所请求的设备已连接的话
			TConfAttrb tTmpConfAttrb = m_tConf.GetConfAttrb();
            if (tTmpConfAttrb.IsHasTvWallModule() )
            {
                u8 byMemberType = 0;
                for(u8 byTvPos = 0; byTvPos < MAXNUM_PERIEQP_CHNNL; byTvPos++)
                {
                    u8 byTvWallId = m_tConfEqpModule.m_tTvWallInfo[byTvPos].GetTvWallEqp().GetEqpId();
					u8 byEqpType  = m_tConfEqpModule.m_tTvWallInfo[byTvPos].GetTvWallEqp().GetEqpType();
				
					//zjl[20091208]会议模板新增设置在hdu预案中设置终端成员
					if (EQP_TYPE_HDU_SCHEME == byEqpType)
					{
						u8 byHduSchemeNum = 0;
						THduStyleInfo atHduStyleInfoTable[MAX_HDUSTYLE_NUM];
						u16 wRet = g_cMcuAgent.ReadHduSchemeTable(&byHduSchemeNum, atHduStyleInfoTable);
						if (SUCCESS_AGENT == wRet)
						{
							THduChnlInfo  atHduChnlInfo[MAXNUM_TVWALL_CHNNL_INSMOUDLE];
							
							u16 wTempLoop = 0;
							for ( wTempLoop = 0; wTempLoop < byHduSchemeNum && wTempLoop < MAX_HDUSTYLE_NUM; wTempLoop++)
							{
								if (atHduStyleInfoTable[wTempLoop].GetStyleIdx() == byTvWallId)
								{
									atHduStyleInfoTable[wTempLoop].GetHduChnlTable(atHduChnlInfo);
									break;
								}
							}
							
							if (wTempLoop >= byHduSchemeNum || byHduSchemeNum > MAX_HDUSTYLE_NUM)
							{
								continue;
							}

							//atHduStyleInfoTable[byTvWallId-1].GetHduChnlTable(atHduChnlInfo);
							
							for (u8 byTvChlLp = 0; byTvChlLp < MAXNUM_TVWALL_CHNNL_INSMOUDLE; byTvChlLp++)
							{
								if( m_tConfEqpModule.IsMtInTvWallChannel( byTvWallId, byTvChlLp, tMt, byMemberType) &&
									g_cMcuVcApp.IsPeriEqpConnected(atHduChnlInfo[byTvChlLp].GetEqpId()))
								{
									ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq] HDU_SCHEME:%d, EQPID:%d, CHNNL:%d, MT:%d, MEMBERTYPE:%d\n",
										byTvWallId, atHduChnlInfo[byTvChlLp].GetEqpId(), atHduChnlInfo[byTvChlLp].GetChnlIdx(), tMt.GetMtId(), byMemberType);
									
									// [2013/03/11 chenbing] 老HDU模板不支持HDU多画面,子通道置0
									ChangeHduSwitch(&tMt, atHduChnlInfo[byTvChlLp].GetEqpId(),
										atHduChnlInfo[byTvChlLp].GetChnlIdx(), 0, byMemberType, TW_STATE_START);
								}
							}
						}
						else
						{
							ConfPrint(LOG_LVL_WARNING, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq] ReadHduSchemeTable failed!\n");
						}
					}
					else
					{
						if( 0 != byTvWallId && g_cMcuVcApp.IsPeriEqpConnected(byTvWallId) )
						{
							for(u8 byTvChlLp = 0; byTvChlLp < MAXNUM_PERIEQP_CHNNL; byTvChlLp++)
							{
								// 同上，这里暂时只处理了本终端出现在Tw成员里面。
								// 如果是指定了其他跟随，则应该在改变主席/发言人的时候自动会调整
								if( m_tConfEqpModule.IsMtInTvWallChannel( byTvWallId, byTvChlLp, tMt, byMemberType) )
								{
									if (!IsMultiCastMt(GetLocalMtFromOtherMcuMt(tMt).GetMtId()))
									{
										// 顾振华 [5/11/2006] 支持指定类型的开始方式
										//ChangeTvWallSwitch(&tMt, byTvWallId, byTvChlLp, TW_MEMBERTYPE_MCSSPEC, TW_STATE_START);
										ChangeTvWallSwitch(&tMt, byTvWallId, byTvChlLp, byMemberType, TW_STATE_START);
									} 
									else
									{
										ChangeTvWallSwitch(NULL, byTvWallId, byTvChlLp, byMemberType, TW_STATE_STOP);
									}
								}
							}
						}
					}
                }

				//删除其在会议模板中电视墙成员的信息
				RemoveMtInMutiTvWallMoudle(tMt);
            }

			//Hdu多画面通道成员处理逻辑[3/11/2013 chendaiwei]
			if(m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.IsUnionStoreHduVmpInfo())
			{
				for( u8 byHduEqpId = HDUID_MIN; byHduEqpId <= HDUID_MAX; byHduEqpId++ )
				{
					if(g_cMcuVcApp.IsPeriEqpConnected(byHduEqpId))
					{
						for(u8 byHduChIdx = 0; byHduChIdx < MAXNUM_HDU_CHANNEL; byHduChIdx++)
						{
							for(u8 bySubChIdx = 0; bySubChIdx <HDU_MODEFOUR_MAX_SUBCHNNUM; bySubChIdx++)
							{
								u8 byVmpChnlMemberType = 0;
								if(m_tConfEqpModule.IsMtInHduVmpChnnl(byHduEqpId,byHduChIdx,bySubChIdx,tMt,byVmpChnlMemberType))
								{
									StaticLog("[ProcMtMcuOpenLogicChnnlReq]byHduEqpId.%d byHduChIdx.%d bySubChIdx.%d MtId.%d Type.%d\n",
										byHduEqpId,byHduChIdx,bySubChIdx,tMt.GetMtId(),byVmpChnlMemberType);
									// [2013/03/11 chenbing] HDU多画面,只开启视频(最后一个参数传入四风格)
									ChangeHduSwitch(&tMt, byHduEqpId, byHduChIdx, bySubChIdx, byVmpChnlMemberType, TW_STATE_START,
										MODE_VIDEO, FALSE, TRUE, TRUE, HDUCHN_MODE_FOUR);
								}
							}
						}
					}
				}

				m_tConfEqpModule.RemoveMtInHduVmpModule(tMt);
			}

			u8 byReason = m_ptMtTable->GetDisconnectReason( GetLocalMtFromOtherMcuMt( tMt ).GetMtId());

			if( MTLEFT_REASON_EXCEPT == byReason ||
				MTLEFT_REASON_RTD == byReason ||
				MTLEFT_REASON_UNREACHABLE == byReason ||
				m_tConf.m_tStatus.m_tConfMode.GetHduInBatchPoll() != POLL_STATE_NONE  )	 
			{	
				BOOL32 bCanRestore = TRUE;
				if ( m_tConf.GetConfAttrb().IsSatDCastMode())
				{
					if ( IsMultiCastMt( tMt.GetMtId())
						&& GetCurSatMtNum() <= 0)
					{
						// 记录当前卫星点数，此处初始化flowcontrol 0，不调整卫星点数
						s16 byCurSatMtNum = GetCurSatMtNum();
						LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtMcuOpenLogicChnnlReq]CurSatMtNum is 0, cannot restore vmpmem or hdumem.\n");
						bCanRestore = FALSE;
						// followctrl置0
                        //zjj20131030 发言终端前面已经处理过
						if( !(GetLocalSpeaker() == tMt) )
						{
							NotifyMtSend( tMt.GetMtId(), MODE_VIDEO, FALSE );
						}
						// 保证不调整卫星点数
						SetCurSatMtNum(byCurSatMtNum);
						LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_CONF,"[ProcMtMcuOpenLogicChnnlReq]RemoveMt(%d) SatMtNum(%d)\n",
							tMt.GetMtId(),GetCurSatMtNum());
					}
				}
				//if (bCanRestore)
				{
					//zjj20091014异常挂断的成员虽然交换拆除，但在画面合成的成员信息中仍然存在
					//如果此时画面合成器正在广播，则恢复这个终端与画面合成交换
					//这里处理的只是固定画面下的恢复，自动合成时的恢复在上面
					//待解决的问题:自动合成时是否要在原来的通道中恢复，还没有试过
					// [pengjie 2010/5/28] 无论是不是画面合成广播，都应该恢复终端
					u8 byTmpVmpId;
					TEqp tTmpVmpEqp;
					TPeriEqpStatus tPeriEqpStatus;
					TVMPParam_25Mem tVmpParam;
					u8 byVmpSubType;
					u8 byChnnl;
					TVMPMember *ptVmpMember = NULL;
					s8 achAlias[MAXLEN_ALIAS] = {0};
					CServMsg cMsg;
					for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
					{
						if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
						{
							continue;
						}
						byTmpVmpId = m_abyVmpEqpId[byIdx];
						tTmpVmpEqp = g_cMcuVcApp.GetEqp( byTmpVmpId );
						
						if( !IsDynamicVmp(byTmpVmpId) &&
							(MTLEFT_REASON_EXCEPT == byReason ||
							MTLEFT_REASON_RTD == byReason ||
							MTLEFT_REASON_UNREACHABLE == byReason ) 
							)
						{
							g_cMcuVcApp.GetPeriEqpStatus( byTmpVmpId, &tPeriEqpStatus );
							byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
							
							tVmpParam = g_cMcuVcApp.GetConfVmpParam(tTmpVmpEqp);
							if( tPeriEqpStatus.m_byOnline != 0   // VMP在线
								&& tVmpParam.IsMtInMember(tMt) ) // 掉线成员在VMP中
							{
								// 2011-11-3 add by pgf: 掉线tMt在多个VMP通道时，必须恢复所有这些通道的交换
								for (byChnnl = 0; byChnnl < MAXNUM_VMP_MEMBER; byChnnl++)
								{
									ptVmpMember = tVmpParam.GetVmpMember(byChnnl);
									
									if ( ptVmpMember == NULL || ptVmpMember->IsNull()
										|| !((TMt)(*ptVmpMember) == tMt))
									{
										continue;
									}
									// 能恢复则建交换，不能恢复则清成员
									if (bCanRestore)
									{
										// 终端异常掉线，上线后对其更改别名，然后再呼入会议，别名有改变带合成器通道内显示非最新别名，再次发送别名给外设
										// 8KE,8KH,MPU2都需要发送别名信息,目前只有vpu不需发送别名信息,mpu要支持通道根据别名显示图片
										if (byVmpSubType != VMP)
										{
											memset(achAlias, 0, sizeof(achAlias));
											if(GetMtAliasToVmp(tMt, achAlias))
											{
												ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq]vmp member.%d info: mt(%d,%d), alias is %s!\n", 
													byChnnl, tMt.GetMcuId(), tMt.GetMtId(), achAlias);
												
												cMsg.Init();
												cMsg.SetChnIndex(byChnnl);
												cMsg.SetMsgBody( (u8*)achAlias, sizeof(achAlias) );
												cMsg.CatMsgBody(&byChnnl, sizeof(u8));
												SendMsgToEqp(tTmpVmpEqp.GetEqpId(), MCU_VMP_CHANGEMEMALIAS_CMD, cMsg);
											}
										}

										SetVmpChnnl(byTmpVmpId, tMt, byChnnl, ptVmpMember->GetMemberType(), TRUE);
									}
									else
									{
										ClearOneVmpMember(byTmpVmpId, byChnnl, tVmpParam);
									}
								}
								
								if ( byVmpSubType != VMP && bCanRestore)
								{
									ChangeMtVideoFormat(tMt);
								}
							}
						}
					}
					
					//zjj20091014针对异常挂断的成员恢复电视墙交换
					//异常挂断的成员虽然交换拆除，但在电视墙的成员信息中仍然存在
					if (bCanRestore)
					{
						RestoreMtInTvWallOrHdu( tMt );
					}
					else
					{
						FindConfHduHduChnnlAndStop(&tMt, TW_MEMBERTYPE_NULL, TRUE);
					}
				}

				//清空掉线原因
				m_ptMtTable->SetDisconnectReason( GetLocalMtFromOtherMcuMt( tMt ).GetMtId(), 0 );
			}
			if( !m_tDoubleStreamSrc.IsNull() && 
				m_ptMtTable->GetManuId( tMt.GetMtId() ) != MT_MANU_KDC &&
				m_ptMtTable->GetSrcSCS(tMt.GetMtId()).GetVideoMediaType() != MEDIA_TYPE_NULL )
			{
				TLogicalChannel tMvLogicalChannel;
				if (m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tMvLogicalChannel, TRUE) &&
					tMvLogicalChannel.GetFlowControl() > 0)
				{
					tMvLogicalChannel = tLogicChnnl;
					u16 wBitrate = 0;
					const u16 wSendBand = m_ptMtTable->GetSndBandWidth(tMt.GetMtId());
					const u16 wRcvBand = m_ptMtTable->GetRcvBandWidth(tMt.GetMtId());
					wBitrate = min (wSendBand ,  wRcvBand);
					wBitrate = GetDoubleStreamVideoBitrate(wBitrate);
					tMvLogicalChannel.SetFlowControl(wBitrate);
					cServMsg.SetMsgBody((u8*)&tMvLogicalChannel, sizeof(tMvLogicalChannel));
					SendMsgToMt(tMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
				}
			}
			/* 此段不需要补交换 ，在VMP自动调整里会建好的
			if( VCS_CONF == m_tConf.GetConfSource() && tMt == m_tConf.GetChairman() )
			{
				TVMPParam tVMPParam = m_tConf.m_tStatus.GetVmpParam();
				if( (VCS_GROUPVMP_MODE == m_cVCSConfStatus.GetCurVCMode() ||
					VCS_GROUPROLLCALL_MODE == m_cVCSConfStatus.GetCurVCMode()) &&
					tVMPParam.IsMtInMember( tMt )
					)
				{
					
					BOOL32 bRet = ChangeMtVideoFormat( tMt, &tVMPParam, 
											TRUE, FALSE );
					if(bRet)
					{
						SetVmpChnnl(tMt, tVMPParam.GetChlOfMtInMember(tMt), 
							tVMPParam.GetVmpMember( tVMPParam.GetChlOfMtInMember(tMt) )->GetMemberType()
							);
					}															
				}
			}*/

			// vrs新录播放像支持
			if (tMt.GetMtType() == MT_TYPE_VRSREC)
			{
				if (m_tPlayEqp.GetMtId() == tMt.GetMtId())
				{
					if (!(GetLocalSpeaker() == tMt))
					{
						//设置新录播实体为发言人
						ChangeSpeaker(&tMt, FALSE, FALSE, FALSE, TRUE);
					}
					//准备放像
					m_tConf.m_tStatus.SetPlayReady();
					m_ptMtTable->SetRecChlState(tMt.GetMtId(), TRecChnnlStatus::STATE_PLAYREADY);
					// 开启等待交换建立定时器
					SetTimer( MCUVC_RECPLAY_WAITMPACK_TIMER, TIMESPACE_WAIT_PLAY_SWITCHOK );
				}
				else
				{
					ReleaseVrsMt(tMt.GetMtId());
				}
			}

		}
	
		//发送音频码流处理	
		if( tLogicChnnl.GetMediaType() == MODE_AUDIO )
		{
			if( VCS_CONF == m_tConf.GetConfSource() && 
				m_cVCSConfStatus.GetMtInTvWallCanMixing() &&
				(  VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode() || 
				   VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode() 
				))
			{
				m_ptMtTable->SetMtInMixing(tMt.GetMtId(),TRUE,FALSE);
			}
			//吃下码流, 转入dump，防止icmp
			g_cMpManager.StartRecvMt( tMt, MODE_AUDIO );
            
            //zbq[07/18/2007] SSRC 的修改由归一重整统一处理
			//g_cMpManager.ResetRecvMtSSRC( tMt, MODE_AUDIO );

			//若是发言人加入, 考虑到纯音频时的发言人也需此处理
			if(GetLocalSpeaker() == tMt )
			{
				//纯音频不需考虑视频复合情况
				//zhouyiliang 20121016如果是恢复发言人，当前会议轮询带音频，且暂停，此时bPolling 要为TRUE，不停轮询
				BOOL32 bPolling = FALSE;
				if ( m_tConf.m_tStatus.GetPollMedia() == MODE_BOTH )
				{
					bPolling = TRUE;
				}
				ChangeSpeaker( &tMt, bPolling, FALSE );		
			}

            // xsl [8/4/2006] vac或讨论模式下将此终端加入混音通道，并建立到混音器的交换
			if(   m_tConf.m_tStatus.IsVACing() ||
                    m_tConf.m_tStatus.IsAutoMixing() || 
                    ( m_tConf.m_tStatus.IsSpecMixing() && (tMt == GetLocalSpeaker() || m_ptMtTable->IsMtInMixing( tMt.GetMtId())) &&
						// zhouyiliang 20101213 vcs会议备份主席不进混音
						(m_tConf.GetConfSource() != VCS_CONF || !(tMt == m_cVCSConfStatus.GetVcsBackupChairMan()) )
						)
						)
			{
                // guzh [1/31/2007] 8000B 能力限制
                u16 wError = 0;
                if ( CMcuPfmLmt::IsMtOprSupported( m_tConf, m_tConfAllMtInfo.GetLocalJoinedMtNum(), GetMixMtNumInGrp(), wError ) )
                {
                    if (GetMixMtNumInGrp() < GetMaxMixNum(m_tMixEqp.GetEqpId()) ||
						(GetMixMtNumInGrp()== GetMaxMixNum(m_tMixEqp.GetEqpId()) && 
						m_ptMtTable->IsMtInMixGrp(tMt.GetMtId())))
					{
						// miaoqingsong [05/11/2011] 增加"混音成员添加是否成功"判断，防止建立从终端到混音器的无效交换
						if( AddMixMember( &tMt, DEFAULT_MIXER_VOLUME, FALSE ) ||
							m_tConf.m_tStatus.IsVACing())
						{
							StartSwitchToPeriEqp(tMt, 0, m_tMixEqp.GetEqpId(), 
								(MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt)),
								MODE_AUDIO, SWITCH_MODE_SELECT);
						}
					} 
					else
					{
						// 给出提示，根据设定策略决定执行
						//20100708_tzy 之前当新加终端数超过混音器能力时，会保持原来不变。
						//现在如果为会议讨论时新加终端致使超过混音器能力，则将会议讨论变为定制混音，并给出提示
						//如果是语音激励，则将语音激励停止，给出相应提示
						//只有当处于会议讨论或者会议讨论与语音激励同时开启时，才会进行切换
						if (m_tConf.m_tStatus.GetMixerMode() != mcuVacMix)
						{                        
							SwitchDiscuss2SpecMix();
							wError = ERR_MCU_DISCUSSAUTOCHANGE2SPECMIX;
							NotifyMcsAlarmInfo(0, wError);  // 提示MCS
						}
						else
						{
							StopMixing();
						}
					}
                }
			}
			else
			{
				NotifyMtSend( tMt.GetMtId(), MODE_AUDIO, FALSE );
			}

			//判断是否为回传通道,是则恢复回传交换
			if( !m_tCascadeMMCU.IsNull() && !(m_tCascadeMMCU == tMt) && 
				!m_tConf.m_tStatus.IsMixing() )
			{
				TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
				if( ptConfMcInfo != NULL && 
					( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
				{
					if( ptConfMcInfo->m_tSpyMt.GetMtId() == tMt.GetMtId() &&
						ptConfMcInfo->m_tSpyMt.GetMcuId() == tMt.GetMcuId() )
					{
						ptConfMcInfo->m_tSpyMt.SetNull();
						StartSwitchToMcu( tMt, 0, m_tCascadeMMCU.GetMtId(), MODE_AUDIO, SWITCH_MODE_SELECT);
						ptConfMcInfo->m_tSpyMt = tMt;
					}
				}
			}
			if( VCS_CONF ==	m_tConf.GetConfSource() &&
				MT_TYPE_MT == m_ptMtTable->GetMtType(tMt.GetMtId()) &&
				(m_tConf.GetChairman().GetMtId() != tMt.GetMtId() || !m_tCascadeMMCU.IsNull()))
			{
				VCSMTMute(tMt, m_cVCSConfStatus.IsRemMute(), VCS_AUDPROC_MUTE);					
				VCSMTMute(tMt, m_cVCSConfStatus.IsRemSilence(), VCS_AUDPROC_SILENCE);
			}
			
			// [2013/04/27 chenbing] 解决模板开启立体声没有音频问题 
			if (MCS_CONF ==	m_tConf.GetConfSource())
			{
				//需要判断是否在HDU通道中
				TDoublePayload tDAPayload;
				TPeriEqpStatus tHduStatus;
				CServMsg cSMsg;
				for (u8 byHduId=HDUID_MIN; byHduId<=HDUID_MAX; byHduId++)
				{
					g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
					if (!tHduStatus.m_byOnline)
					{
						continue;
					}

					for (u8 byHduChnId=0; byHduChnId<MAXNUM_HDU_CHANNEL; byHduChnId++)
					{
						TMt tMtInHduVmp = (TMt)tHduStatus.m_tStatus.tHdu.GetChnMt(byHduChnId);
						if( (tMtInHduVmp.GetConfIdx() == m_byConfIdx) && (tMtInHduVmp == tMt) )
						{
							u8 byAudioNum = tLogicChnnl.GetAudioTrackNum();
							u8 bIsNeedPrs = m_tConf.GetConfAttrb().IsResendLosePack();

							tDAPayload.SetRealPayLoad(m_ptMtTable->GetSrcSCS(tMt.GetMtId()).GetAudioMediaType());
							tDAPayload.SetActivePayload(m_ptMtTable->GetSrcSCS(tMt.GetMtId()).GetAudioMediaType());
							cSMsg.SetChnIndex(byHduChnId);
							cSMsg.SetMsgBody((u8 *)&byAudioNum, sizeof(u8));
							cSMsg.CatMsgBody((u8 *)&tDAPayload, sizeof(tDAPayload));

							ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq] MCU_HDU_FRESHAUDIO_PAYLOAD_CMD\n");
							ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_EQP, "HduId(%d) ChnId(%d) AudioNum(%d) AudioRealPayLoad(%d) AudioActivePayLoad(%d)!!!\n",
										byHduId, byHduChnId, byAudioNum, tDAPayload.GetRealPayLoad(), tDAPayload.GetActivePayload());
							//发送音频载荷
							SendMsgToEqp(byHduId, MCU_HDU_FRESHAUDIO_PAYLOAD_CMD, cSMsg);
						}
					}
				}
			}
		}	
		
		//打开某逻辑通道后更新能力级,仅用于radvision 双流级联
		if( tLogicChnnl.GetMediaType() == MODE_VIDEO &&
			m_ptMtTable->GetManuId(tMt.GetMtId()) == MT_MANU_RADVISION)
		{
			TCapSupport tCapSupport = m_tConf.GetCapSupport();
			// MCU前向纠错, zgc, 2007-09-27
			TCapSupportEx tCapSupportEx = m_tConf.GetCapSupportEx();

            TSimCapSet tSim = tCapSupport.GetMainSimCapSet();
            tSim.SetVideoMaxBitRate(m_ptMtTable->GetDialBitrate( tMt.GetMtId()));
            tCapSupport.SetMainSimCapSet(tSim);
            tSim = tCapSupport.GetSecondSimCapSet();
            if(!tSim.IsNull())
            {
                tSim.SetVideoMaxBitRate(m_ptMtTable->GetDialBitrate( tMt.GetMtId()));
                tCapSupport.SetSecondSimCapSet(tSim);
            }			
            tCapSupport.SetDStreamMaxBitRate(m_ptMtTable->GetDialBitrate( tMt.GetMtId()));
            
			u8 bySupportActiveH263 = 1; // 是否设置活动载荷标识
			u8 bySendMSDetermine   = 0; // 是否发送主从决定
	
			cServMsg.SetMsgBody( ( u8 * )&tCapSupport, sizeof( tCapSupport ) );
			cServMsg.CatMsgBody( ( u8 * )&bySupportActiveH263, sizeof( bySupportActiveH263 ) );
			cServMsg.CatMsgBody( ( u8 * )&bySendMSDetermine, sizeof( bySendMSDetermine ) );
			// MCU前向纠错, zgc, 2007-09-27
			cServMsg.CatMsgBody( ( u8 * )&tCapSupportEx, sizeof( tCapSupportEx ) );

            //[6/16/2011 zhushz]iMax会议取配置文件中适配帧率
            /*u8 byFpsAdp = 0;
            if (m_tConf.GetConfAttrbEx().IsImaxModeConf())
            {
                byFpsAdp = g_cMcuVcApp.GetFpsAdp();
            }            
	        cServMsg.CatMsgBody(&byFpsAdp, sizeof(u8));
            */

			// 组织扩展能力集勾选给Mtadp [12/8/2011 chendaiwei]
			TVideoStreamCap atMSVideoCap[MAX_CONF_CAP_EX_NUM];
			u8 byCapNum = MAX_CONF_CAP_EX_NUM;
			m_tConfEx.GetMainStreamCapEx(atMSVideoCap,byCapNum);
			
			TVideoStreamCap atDSVideoCap[MAX_CONF_CAP_EX_NUM];
			u8 byDSCapNum = MAX_CONF_CAP_EX_NUM;
			m_tConfEx.GetDoubleStreamCapEx(atDSVideoCap,byDSCapNum);

			cServMsg.CatMsgBody((u8*)&atMSVideoCap[0], sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);
			cServMsg.CatMsgBody((u8*)&atDSVideoCap[0], sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);
			
			//TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
			//u8 byAudioTrackNum = tAudioType.GetAudioTrackNum();
			//cServMsg.CatMsgBody(&byAudioTrackNum,sizeof(byAudioTrackNum));
			TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];//音频能力
			//从会议属性中取出会议支持的音频类型
			m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);	
			cServMsg.CatMsgBody((u8*)&atAudioTypeDesc[0], sizeof(TAudioTypeDesc)* MAXNUM_CONF_AUDIOTYPE);

			SendMsgToMt( tMt.GetMtId(), MCU_MT_CAPBILITYSET_NOTIF, cServMsg );
		}

		// 对于VCS会议，第一路音频或视频后向通道打开后，继续进行下一步调度操作
		if (VCS_CONF == m_tConf.GetConfSource() &&
			!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
		{
			u8 tMediaType = tLogicChnnl.GetMediaType();
			if (MODE_VIDEO == tMediaType || MODE_AUDIO == tMediaType)
			{
				GoOnSelStep(tMt, tMediaType, FALSE);
			}			
			
			if( m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE && tMt == m_tConf.GetChairman() &&
				CONF_CREATE_MT != m_byCreateBy )
			{
				TSwitchInfo tSwitchInfo;
				TMt tMtDst;
				for( u8 byLoop = 1;byLoop <= MAXNUM_CONF_MT; ++byLoop )
				{
					if( !m_tConfAllMtInfo.MtJoinedConf(byLoop) ||
						m_cVCSConfStatus.GetCurVCMT().GetMtId() == byLoop ||
						m_cVCSConfStatus.GetVcsBackupChairMan().GetMtId() == byLoop )
					{
						continue;
					}
					
					tMtDst = m_ptMtTable->GetMt( byLoop );
					tSwitchInfo.SetMode( tMediaType );
					tSwitchInfo.SetDstMt( tMtDst );
					tSwitchInfo.SetSrcMt( tMt );
					if (!VCSConfSelMT(tSwitchInfo))
					{
						NotifyMtReceive(tMtDst, tMtDst.GetMtId());
					}
				}
			}
		}

		break;
        }
	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

	return;
}

/*==============================================================================
函数名    :  AdjustCtrlModeVmpModuleByMt
功能      :  终端开通道调整模版开启的会控指定画面合成
			 用于终端开逻辑通道时调整模版开启的画面合成，给对应通道建交换
算法实现  :  
参数说明  :  TMt &tMt
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013-04					yanghuaizhi							
==============================================================================*/
void CMcuVcInst::AdjustCtrlModeVmpModuleByMt(const TMt &tMt)
{
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();

	// 只针对本级终端开通道时调用,ProcMtMcuOpenLogicChnnlReq调用
	if (tMt.IsNull() || !tMt.IsLocal())
	{
		return;
	}

	// 只调整模版开启非自动画面合成
    if( tConfAttrb.IsHasVmpModule() && 
        !m_tConfInStatus.IsVmpModuleUsed() &&
		g_cMcuVcApp.GetVMPMode(m_tModuleVmpEqp) != CONF_VMPMODE_NONE &&
		!IsDynamicVmp(m_tModuleVmpEqp.GetEqpId())
		)
    {
		u8 byVmpSubType = GetVmpSubType(m_tModuleVmpEqp.GetEqpId());
		TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tModuleVmpEqp);
        u8 byChlNum = tVmpParam.GetMaxMemberNum();
        u8 byMemberType = 0;
		u8 byVmpStyle = tVmpParam.GetVMPStyle();
		TMt tSpeaker = m_tConf.GetSpeaker();
		TVMPMember tVMPMember;
		TVMPMember *ptVmpMem = NULL;
		//s8 achAlias[VALIDLEN_ALIAS+1] = {0};
        s8 achAlias[MAXLEN_ALIAS] = {0}; // 扩容 [7/4/2013 liaokang]
		CServMsg cMsg;
		TMt tNullMt;
        for( u8 byLoop = 0; byLoop < byChlNum; byLoop++ )
        {
			tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tModuleVmpEqp);//合成成员会更新
            if( m_tConfEqpModule.IsMtAtVmpChannel( byLoop, tMt, byMemberType ) )
            {
				// 在终端上线前,该通道成员已变,不能再恢复此通道成员
				tVMPMember = *tVmpParam.GetVmpMember(byLoop);
				if (VMP_MEMBERTYPE_NULL != tVMPMember.GetMemberType()
					&& byMemberType != tVMPMember.GetMemberType())
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq]vmp member.%d membertype: %d, is not Module membertype%d, can't into vmp!\n", 
						byLoop, tVMPMember.GetMemberType(), byMemberType);
					m_tConfEqpModule.SetVmpMember( byLoop, tNullMt );//不再恢复vmp模版中的此通道
					continue;
				}
				if (!tVMPMember.IsNull() && !(tVMPMember == tMt))
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq]vmp member.%d info: mt(%d,%d), is not openlogic mt(%d,%d), can't into vmp!\n", 
						byLoop, tVMPMember.GetMcuId(), tVMPMember.GetMtId(), tMt.GetMcuId(), tMt.GetMtId());
					m_tConfEqpModule.SetVmpMember( byLoop, tNullMt );//不再恢复vmp模版中的此通道
					continue;
				}
				// 针对发言人跟随,若当前发言人已不是模版发言人终端,不恢复发言人跟随通道成员
				// 场景:模版开启时,发言人终端未上线,改变发言人又取消发言人后,此终端又上线,此终端不会变发言人,但模版发言人跟随通道仍有此终端
				if (VMP_MEMBERTYPE_SPEAKER == tVMPMember.GetMemberType() && 
					!(tMt == tSpeaker))
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq]vmp member.%d info: SpeakerMt(%d,%d), is not openlogic mt(%d,%d), can't into vmp!\n", 
						byLoop, tSpeaker.GetMcuId(), tSpeaker.GetMtId(), tMt.GetMcuId(), tMt.GetMtId());
					m_tConfEqpModule.SetVmpMember( byLoop, tNullMt );//不再恢复vmp模版中的此通道
					continue;
				}
				// 更新confVmpParam合成成员
                memset(&tVMPMember, 0, sizeof(tVMPMember));
                tVMPMember.SetMemberTMt(tMt);
                tVMPMember.SetMemberType(byMemberType);
                tVMPMember.SetMemStatus(MT_STATUS_AUDIENCE);
                tVmpParam.SetVmpMember(byLoop,tVMPMember);
				// 卫星会议判断终端类型 [pengguofeng 1/16/2013]
				if ( m_tConf.GetConfAttrb().IsSatDCastMode())
				{
					if ( IsMultiCastMt( tMt.GetMtId())
						//&& IsSatMtOverConfDCastNum(tMt))
						&& !IsSatMtCanContinue(tMt.GetMtId(),emVmpChnChange,&m_tModuleVmpEqp,byLoop, 0xFF, &tVmpParam))
					{
						LogPrint(LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtMcuOpenLogicChnnlReq]vmp chnnl:%d member:%d is Sat Mt and Over Max, clear it\n",
							byLoop, tMt.GetMtId());
						continue;
					}
				}
				// 更新confVmpParam合成成员
				g_cMcuVcApp.SetConfVmpParam(m_tModuleVmpEqp, tVmpParam);
                UpdataVmpEncryptParam(tMt, byLoop);
				
				// 8KE,8KH,MPU2都需要发送别名信息,目前只有vpu不需发送别名信息,mpu要支持通道根据别名显示图片
				if (byVmpSubType != VMP)
				//if (GetVmpSubType(m_tVmpEqp.GetEqpId()) == VMP_8KE || GetVmpSubType(m_tVmpEqp.GetEqpId()) == VMP_8KH)
				{
					// [9/7/2010 xliang] 8000G 还需发送一次别名信息，20110331 zhouyiliang 8000H一样
					memset(achAlias, 0, sizeof(achAlias));
					if( /*GetMtAliasInVmp(tMt, achAlias) */
                        GetMtAliasToVmp(tMt, achAlias) )
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ProcMtMcuOpenLogicChnnlReq]vmp member.%d info: mt(%d,%d), alias is %s!\n", 
							byLoop, tMt.GetMcuId(), tMt.GetMtId(), achAlias);
						
						cMsg.Init();
						cMsg.SetChnIndex(byLoop);
						cMsg.SetMsgBody( (u8*)achAlias, sizeof(achAlias) );
						cMsg.CatMsgBody(&byLoop, sizeof(u8));
						SendMsgToEqp(m_tModuleVmpEqp.GetEqpId(), MCU_VMP_CHANGEMEMALIAS_CMD, cMsg);
					}
				}

                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuOpenLogicChnnlReq] tMt<%d, %d>, byChnnl:%d, mode:%d\n",
                    tMt.GetMcuId(),tMt.GetMtId(), byLoop, byMemberType);

				// 进画面合成通道使用新方式，调分辨率+建交换
				ChangeMtResFpsInVmp(m_tModuleVmpEqp.GetEqpId(), tMt, &tVmpParam, TRUE, tVMPMember.GetMemberType(), byLoop, TRUE);

				//n+1备份更新vmp信息
				if (MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState())
				{
					ProcNPlusVmpUpdate();
				}                     
            }
        }		
		
		//同步外设复合状态
		TPeriEqpStatus tVmpPeriEqpStatus;
        g_cMcuVcApp.GetPeriEqpStatus( m_tModuleVmpEqp.GetEqpId(), &tVmpPeriEqpStatus );
        tVmpPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tVmpParam);
        g_cMcuVcApp.SetPeriEqpStatus( m_tModuleVmpEqp.GetEqpId(), &tVmpPeriEqpStatus );
    }
	return;
}

/*=============================================================================
  函 数 名： ProcMtMcuMediaLoopOpr
  功    能： 终端远端环回消息处理
  算法实现： 
  全局变量： 
  参    数： const CMessage *pcMsg
  返 回 值： void 
=============================================================================*/
void CMcuVcInst::ProcMtMcuMediaLoopOpr( const CMessage *pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TLogicalChannel *ptChan = (TLogicalChannel *)cServMsg.GetMsgBody();
    TMt tLoopMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
    u8 byMode = ptChan->GetMediaType();
    TMtStatus tMtStatus;
    m_ptMtTable->GetMtStatus(tLoopMt.GetMtId(), &tMtStatus);

	switch( CurState() )
	{
	case STATE_ONGOING:
		//开启
		if (pcMsg->event == MT_MCU_MEDIALOOPON_REQ)
		{              
            //双流模式下不允许环回
            if (!m_tDoubleStreamSrc.IsNull())
            {
                cServMsg.SetEventId(pcMsg->event+2);
                SendMsgToMt(cServMsg.GetSrcMtId(), pcMsg->event+2, cServMsg);
                ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMtMcuMediaLoopOpr] mt can't loop in double stream mode.\n");
                return;
            }
			//zjl 20110510 Mp:StartSwitchToAll 替换 StartSwitchToSubMt
            //if (!g_cMpManager.StartSwitchToSubMt(tLoopMt, 0, tLoopMt, byMode, SWITCH_MODE_SELECT))
			TSwitchGrp tSwitchGrp;
			tSwitchGrp.SetSrcChnl(0);
			tSwitchGrp.SetDstMtNum(1);
			tSwitchGrp.SetDstMt(&tLoopMt);
			if (!g_cMpManager.StartSwitchToAll(tLoopMt, 1, &tSwitchGrp, byMode, SWITCH_MODE_SELECT))
            {
                cServMsg.SetEventId(pcMsg->event+2);
                SendMsgToMt(cServMsg.GetSrcMtId(), pcMsg->event+2, cServMsg);
                ConfPrint(LOG_LVL_WARNING, MID_MCU_MT,  "[ProcMtMcuMediaLoopOpr] start switch failed, byMode<%d>!\n", byMode );
                return;
            }

            //ack
            cServMsg.SetEventId(pcMsg->event+1);
            SendMsgToMt(cServMsg.GetSrcMtId(), pcMsg->event+1, cServMsg);
            
            NotifyMtReceive(tLoopMt, tLoopMt.GetMtId());

            //置环回标记
            tMtStatus.SetMediaLoop(byMode, TRUE);
            m_ptMtTable->SetMtStatus(tLoopMt.GetMtId(), &tMtStatus);

            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT,  "[ProcMtMcuMediaLoopOpr] mt<%d> mode<%d> start media loop.\n", tLoopMt.GetMtId(), byMode );
		}
		//关闭
		else
		{
            if (tMtStatus.IsMediaLoop(byMode))
            {
                tMtStatus.SetMediaLoop(byMode, FALSE);
                m_ptMtTable->SetMtStatus(tLoopMt.GetMtId(), &tMtStatus);

                //恢复接收媒体源
                RestoreRcvMediaBrdSrc(tLoopMt.GetMtId(), byMode, FALSE);

                //恢复接收混音器
                // xsl [8/4/2006] 定制混音交换n模式码流，其他情况加入混音通道
                if (m_tConf.m_tStatus.IsSpecMixing())
                {
                    SwitchMixMember(&tLoopMt, TRUE);
                }
                else
                {
                    if (GetMixMtNumInGrp() < GetMaxMixNum(m_tMixEqp.GetEqpId()))
                    {
                        AddMixMember( &tLoopMt, DEFAULT_MIXER_VOLUME, FALSE );
						StartSwitchToPeriEqp(tLoopMt, 0, m_tMixEqp.GetEqpId(), 
											 (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tLoopMt)), 
											 MODE_AUDIO, SWITCH_MODE_SELECT);	
                    }                    
                }           

                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT,  "[ProcMtMcuMediaLoopOpr] mt<%d> mode<%d> stop media loop.\n", tLoopMt.GetMtId(), byMode );
            }
	        else
            {
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT,  "[ProcMtMcuMediaLoopOpr] mt<%d> mode<%d> not in media loop. ignore it!\n", 
                        tLoopMt.GetMtId(), byMode );
            }
		}
		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[ProcMtMcuMediaLoopOpr] Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

	return;
}

/*=============================================================================
    函 数 名： ProcMtMcuGetH239TokenReq
    功    能： 终端给MCU的 获取 H239令牌 权限请求
    算法实现： 
			   20110525 zjl 

			   1. 如果请求令牌环者和本级当前令牌环拥有者相同，则拒绝。

			   2. 该请求者必须符合模板配置的双流身份(即发言人或任意终端)，否则请求失败

			   3. 释放老的令牌环拥有者，无论它是终端还是mcu

	              注意：
	              a.不发此消息给终端，终端仍然认为自己是令牌环拥有者，如果有新的双流源时，会导致mcu开其双流通道被拒绝。

	              b.上面放开科达mcu的限制，主要是为了级联抢发双流时，后发双流的mcu先让另一个mcu去释放令牌环，

	               消息releasetoken在mtadp会转成tokenreq, 该mcu收到此消息后直接先让本级先前发双流的终端释放临牌环。

	           4. 停止本级双流, 注意StopDoubleStream中会调用 ClearH239TokenOwnerInfo，还会再发一次releasetoken，
				  而在本接口里已经释放过一次了, 想把本接口下的releasetoken删掉，但不清楚原来是否还有别的作用，暂时保留。
				  发两次也没问题，本接口入口会保护。
	
			   5. 更新令牌环拥有者并通告给所有与会终端。
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/6/8    3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::ProcMtMcuGetH239TokenReq( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	TH239TokenInfo tSrcH239Info = *((PTH239TokenInfo)(cServMsg.GetMsgBody()));
	
	//[2012/22/8 zhangli]存在双选看拒绝再发起双流 
	if (!GetSecVidBrdSrc().IsNull())
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMtMcuGetH239TokenReq] m_tSecVidBrdSrc is not NULL,nack!\n");
		
		cServMsg.SetErrorCode(ERR_MCU_SECSPEAKER_SECSPEAKERNOTNULL);
		SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
		
		return;
	}

	// MCU自认为有Token的管理权限，不需要进行请求确认
// 	if (MT_MANU_KDCMCU == m_ptMtTable->GetManuId(tMt.GetMtId()))
// 	{
// 		return;
// 	}

	if( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) && 
		!g_cMcuVcApp.IsPolyComMtCanSendDoubleStream() )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MT2, "[ProcMtMcuGetH239TokenReq] Polycom Mt.%d can't send double stream\n",tMt.GetMtId() );
		cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
	    cServMsg.CatMsgBody((u8 *)&tSrcH239Info, sizeof(TH239TokenInfo));
		cServMsg.SetEventId(MCU_MT_GETH239TOKEN_NACK);	
	    SendMsgToMt( tMt.GetMtId(), MCU_MT_GETH239TOKEN_NACK, cServMsg);
		return;
	}
	
	if (m_tH239TokenOwnerInfo.m_tH239TokenMt == tMt)
	{
		if( MT_MANU_KDCMCU != m_ptMtTable->GetManuId(m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId()) 
				 && MT_MANU_KDC != m_ptMtTable->GetManuId(m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId())
				 && m_tH239TokenOwnerInfo.GetChannelId() != tSrcH239Info.GetChannelId())
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMtMcuGetH239TokenReq] No keda Mt(Mcu) req H239 Token after ds has started!\n");
		}
		else
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcMtMcuGetH239TokenReq] MT:%d has been tokenMT, so return!\n", tMt.GetMtId());
			
			return;
		}
	}

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcMtMcuGetH239TokenReq] Rcv MT:%d tokenReq!\n", tMt.GetMtId());

    //判断双流的发起源
	if (CheckDualMode(tMt)) 
	{
    }
    else
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MT2,  "[ProcMtMcuGetH239TokenReq] MtId.%d, ManuId.%d, DStreamMtId.%d open double stream with Wrong DualMode, NACK !\n", 
			tMt.GetMtId(), m_ptMtTable->GetManuId( tMt.GetMtId() ), m_tDoubleStreamSrc.GetMtId() );	

		cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
	    cServMsg.CatMsgBody((u8 *)&tSrcH239Info, sizeof(TH239TokenInfo));
        cServMsg.SetEventId(MCU_MT_GETH239TOKEN_NACK);	
	    SendMsgToMt( tMt.GetMtId(), MCU_MT_GETH239TOKEN_NACK, cServMsg);
        return;
	}

	//REQUEST RELEASING TOKEN OF LAST MT OWNER
	if( !m_tH239TokenOwnerInfo.m_tH239TokenMt.IsNull() )
	{
		if( !(m_tH239TokenOwnerInfo.m_tH239TokenMt == tMt) /*&& 
			  MT_MANU_KDCMCU != m_ptMtTable->GetManuId(tMt.GetMtId())*/)
		{
			TH239TokenInfo tH239Info;
			tH239Info.SetChannelId( m_tH239TokenOwnerInfo.GetChannelId() );
			tH239Info.SetSymmetryBreaking( 0 );
			cServMsg.SetEventId(MCU_MT_RELEASEH239TOKEN_CMD);
			cServMsg.SetMsgBody((u8 *)&m_tH239TokenOwnerInfo.m_tH239TokenMt, sizeof(TMt));
			cServMsg.CatMsgBody((u8 *)&tH239Info, sizeof(TH239TokenInfo));
			SendMsgToMt( m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId(), MCU_MT_RELEASEH239TOKEN_CMD, cServMsg);
		}
		/*对于在KEDAMCU上的某个会议发起双流后该会议会设置一个令牌环通知定时器（向其他MCU发送令牌环通知），其他级联KEDAMCU收到该通知时会根根据条件开启双流。
		但是这样可能会导致级联会议中的各个会议可能双流不一致。针对该情况增加了一种确认机制。保证整个会议的双流一致性。
		机制描述:如果当前令牌环拥有者是KEDAMCU，新的终端来请求令牌环时只是记录下该令牌请求者的ID，但并不给他回ACK。而是先向此时的双流源（即KEDAMCU）发送释放令牌命令消息，
		该消息在MTADP侧会转换成获取令牌请求消息，对方收到回一个令牌环请求确认消息，收到后在MTADP侧会转化成释放令牌通知，当我们主业务收到该令牌释放通知后会根据令牌请求记录
		终端回确认消息，以便让该终端能顺利发起双流。*/
		if (MT_MANU_KDCMCU == m_ptMtTable->GetManuId(m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId()))
		{
			//如果还没给之前请求令牌的终端回令牌确认时又收到了其他终端的令牌请求则以后来的为准，并拒绝之前终端的令牌请求
			if( 0 != m_byNewTokenReqMtid &&  m_byNewTokenReqMtid != tMt.GetMtId() &&
				MT_TYPE_MT == m_ptMtTable->GetManuId(m_byNewTokenReqMtid) )
			{
				cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
				cServMsg.CatMsgBody((u8 *)&m_tH239TokenOwnerInfo, sizeof(TH239TokenInfo));
				cServMsg.SetEventId(MCU_MT_GETH239TOKEN_NACK);	
				SendMsgToMt( m_byNewTokenReqMtid, MCU_MT_GETH239TOKEN_NACK, cServMsg);	
			}
			m_byNewTokenReqMtid = tMt.GetMtId();
			//如果新的请求令牌终端不是KEDAMCU则只是记录该终端ID，在收到原双流源的令牌释放通知后再给新终端回令牌确认消息
			if( MT_MANU_KDCMCU != m_ptMtTable->GetManuId(tMt.GetMtId()) )
			{
				m_tH239TokenOwnerInfo.SetChannelId( tSrcH239Info.GetChannelId() );
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2,"[ProcMtMcuGetH239TokenReq]Current TokenMt(%d) Is KEDAMCU New Token ReqMt(%d)chnIdx(%d),So Return!!\n",
					m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId(),m_byNewTokenReqMtid,tSrcH239Info.GetChannelId());	
				return;
			}
		}
	}
	SetTimer( MCUVC_WAITOPENDSCHAN_TIMER, 3000);
	if( !m_tDoubleStreamSrc.IsNull() )
	{
		//SONY:------FIRST OPENCHANNEL,NEXT REQUEST TOKEN
		//CLOSE LAST CHANNELS
		if( !(m_tDoubleStreamSrc == tMt) )
		{
			StopDoubleStream( TRUE, FALSE, FALSE);//双流源切换时，停双流不调整vmp，新双流源开启时会调整
		}
	}	

    cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
	cServMsg.CatMsgBody((u8 *)&tSrcH239Info, sizeof(TH239TokenInfo));	
		
	//POLYCOM7000的静态双流时,发送通道在一开始打开后一直不关闭，使用H239Token进行发送激活或关闭
	//IF CHANNEL HAS BEEN OPENED, START DOUBLESTREAM,
	//ELSE IF TOKEN HAS BEEN OWNED, START DOUBLESTREAM WHEN RECEIVED OPENLOGICAL_ERQ...
	TLogicalChannel tH239LogicChnnl;
	if( ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) ||
		 ( MT_MANU_AETHRA == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
		  MT_TYPE_MT != m_ptMtTable->GetMtType(tMt.GetMtId()) ) )
		 && m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tH239LogicChnnl, FALSE ) )
	{
        // xsl [7/20/2006]卫星分散会议时需要考虑回传通道数
        if (m_tConf.GetConfAttrb().IsSatDCastMode() && /*IsOverSatCastChnnlNum(tMt.GetMtId())*/
			IsMultiCastMt(tMt.GetMtId())
			&& //IsSatMtOverConfDCastNum(tMt))
			!IsSatMtCanContinue(tMt.GetMtId(),emstartDs))
        {
            ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtMcuGetH239TokenReq] over max upload mt num.\n");
            cServMsg.SetEventId(MCU_MT_GETH239TOKEN_NACK);	
			cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
			cServMsg.CatMsgBody((u8 *)&tSrcH239Info, sizeof(TH239TokenInfo));
	        SendMsgToMt( tMt.GetMtId(), MCU_MT_GETH239TOKEN_NACK, cServMsg);
            return;
        }

		StartDoubleStream( tMt, tH239LogicChnnl );
	}	

    //RESPONSE TOKEN TO NEW MT OWNER
	cServMsg.SetEventId(MCU_MT_GETH239TOKEN_ACK);	
	SendMsgToMt( tMt.GetMtId(), MCU_MT_GETH239TOKEN_ACK, cServMsg);

    //SEND OTHER MT TOKEN INDICATION
    m_tH239TokenOwnerInfo.SetChannelId( tSrcH239Info.GetChannelId() );
	m_tH239TokenOwnerInfo.SetSymmetryBreaking( tSrcH239Info.GetSymmetryBreaking() );
	m_tH239TokenOwnerInfo.SetTokenMt( tMt );
	NotifyH239TokenOwnerInfo( NULL );
    
	return;
}

/*=============================================================================
    函 数 名： ProcMtMcuOwnH239TokenNotify
    功    能： 终端给MCU的 拥有 H239令牌 权限通知
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/6/8    3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::ProcMtMcuOwnH239TokenNotify( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	TH239TokenInfo tSrcH239Info = *((PTH239TokenInfo)(cServMsg.GetMsgBody()));

	//SEND OTHER MT TOKEN INDICATION
	if( m_tH239TokenOwnerInfo.m_tH239TokenMt == tMt )
	{
		NotifyH239TokenOwnerInfo( NULL );
	}
	else
	{
		//如果m_byNewTokenReqMtid不为0并且与不是当前通知MCU的ID，则说明当前有其他终端请求了令牌，但MCU还未批准其发起双流，此时不理会该令牌通知，以免被冲掉
		if (m_byNewTokenReqMtid != 0 && m_byNewTokenReqMtid != tMt.GetMtId())
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2,  "[ProcMtMcuOwnH239TokenNotify]m_byNewTokenReqMtid(%d) != tMt(%d),So Return!\n",m_byNewTokenReqMtid,tMt.GetMtId());
			return;
		}
		// xliang [11/14/2008] polycomMCU不认为kedaMCU有Token管理权，故不请求Token.
		//其直接在此发TokenOwnerInd就表示当前PolycomMCU要发双流，这里起或抢...
	// 	if ( (( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) || MT_MANU_AETHRA ==m_ptMtTable->GetManuId(tMt.GetMtId()))
	// 		  && ( MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) || MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId())) )
	   // 若MCU作为双流源，1)该实体自认为有Token环的管理权限，不进行请求，直接以通告形式告知
	   //                  2)MCU间的双流通道打开后不关闭，通过流控进行启用
	   //                  3)判断是否满足发起双流的条件，满足则改双流源为MCU，不满足则不转发其双流
		TLogicalChannel tFstLogicChnnl;
		TLogicalChannel tSecLogicChnnl;
		if ((MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) || MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) || MT_MANU_KDCMCU == m_ptMtTable->GetManuId( tMt.GetMtId() )) 
			&& m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tFstLogicChnnl, FALSE)
			&& m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_SECVIDEO, &tSecLogicChnnl, FALSE)
			&& !(m_tH239TokenOwnerInfo.m_tH239TokenMt == tMt)
			&& CheckDualMode(tMt))
		{
 			u16 wDailBR = m_ptMtTable->GetDialBitrate(tMt.GetMtId());
			u16 wSecBR = tSecLogicChnnl.GetFlowControl();
			
			if ( wSecBR * 100 / wDailBR > 10 )
			{
				if ( m_tDoubleStreamSrc == tMt )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[ProcMtMcuOwnH239TokenNotify] DS has been started, ignore the same Ind\n" );
					if (m_tH239TokenOwnerInfo.GetTokenMt().IsNull())
					{
						//[20101224 zjl]三级场景：第一级发言人发双流，第二三级皆为任意终端发双流，
						//第三级终端发双流，第二级打开通道后就发起其本级的双流，但是TokenMt为空
						//拖第三级终端做第一级发言人，导致第二级mcu始终无法将TokenMt告知第一级，
						//最终第一级无法成功开启双流
						m_tH239TokenOwnerInfo.SetTokenMt(tMt);
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMtMcuOwnH239TokenNotify] DS has been started and Token Mt is null!\n");
					}
				}
				else
				{
					if(!m_tH239TokenOwnerInfo.m_tH239TokenMt.IsNull() &&
					   !(m_tH239TokenOwnerInfo.m_tH239TokenMt == tMt))
					{
						ClearH239TokenOwnerInfo(&m_tH239TokenOwnerInfo.m_tH239TokenMt);
					}
					//释放令牌环
					//ClearH239TokenOwnerInfo(NULL);

					//更新令牌环拥有者信息
					m_tH239TokenOwnerInfo.SetTokenMt(tMt);
					NotifyH239TokenOwnerInfo(NULL);

					StartDoubleStream(tMt, tSecLogicChnnl);	
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[ProcMtMcuOwnH239TokenNotify] tMt.%d, wSecBR.%d, wDailBR.%d, StartDS\n",
						tMt.GetMtId(), wSecBR, wDailBR );
				}

			}
			else
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "[ProcMtMcuOwnH239TokenNotify] tMt.%d, wSecBR.%d, wDailBR.%d, should but no StartDS due to bad band distribution.%d\n",
					tMt.GetMtId(), wSecBR, wDailBR);
			}		
		}
		else
		{
			tFstLogicChnnl.Clear();
			tSecLogicChnnl.Clear();
			//本级未开启双流成功，但级联MCU双流发送通道开启，故级联MCU主流发送通道码率也应该调整[1/6/2013 chendaiwei]
			if ( (MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) 
				|| MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId())) 
				&& m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_SECVIDEO, &tSecLogicChnnl, FALSE)
				&& m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tFstLogicChnnl, FALSE)
				&& tFstLogicChnnl.GetFlowControl() == m_ptMtTable->GetSndBandWidth(tMt.GetMtId()))
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcMtMcuOwnH239TokenNotify] Adjust Mt<%d> Main Stream Send BR from %d to %d due to DStream chnnl opened!\n",
					tMt.GetMtId(),tFstLogicChnnl.GetFlowControl(),GetDoubleStreamVideoBitrate(m_ptMtTable->GetSndBandWidth(tMt.GetMtId())));
				
				tFstLogicChnnl.SetFlowControl(GetDoubleStreamVideoBitrate(m_ptMtTable->GetSndBandWidth(tMt.GetMtId())));
				m_ptMtTable->SetMtLogicChnnl(tMt.GetMtId(),LOGCHL_VIDEO,&tFstLogicChnnl,FALSE);		
			}

			ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[ProcMtMcuOwnH239TokenNotify] tmt.%d(type:%d) should not start ds due to no condition(checkdualmode:%d, tokenowner(%d))\n",
				  tMt.GetMtId(), m_ptMtTable->GetMainType(tMt.GetMtId()), CheckDualMode(tMt), m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId());
		}
	}

	return;
}

/*=============================================================================
    函 数 名： ProcMtMcuReleaseH239TokenNotify
    功    能： 终端给MCU的 释放 H239令牌 权限通知
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/6/8    3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::ProcMtMcuReleaseH239TokenNotify( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	
	if( m_tH239TokenOwnerInfo.m_tH239TokenMt == tMt )
	{
		TH239TokenInfo tH239TokenInfo;
		tH239TokenInfo.SetChannelId( m_tH239TokenOwnerInfo.GetChannelId() );
		
		m_tH239TokenOwnerInfo.Clear();

		//POLYCOM7000的静态双流时,发送通道在一开始打开后一直不关闭，使用H239Token进行发送激活或关闭
		if( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) && 
			m_tDoubleStreamSrc == tMt )
		{
			StopDoubleStream( FALSE, FALSE );
		}

		TMt tMtPolycom;
		CServMsg cMsg;
		// xliang [11/14/2008] 同时释放 POLYMCU 的TOKEN
		for ( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
		{
// 			if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(byMtId) &&
// 				( MT_TYPE_SMCU == m_ptMtTable->GetMtType(byMtId) ||
			// 				MT_TYPE_MMCU == m_ptMtTable->GetMtType(byMtId) ) )
			// 释放令牌环都应该通知非科达MCU(或终端),不应该只限于宝利通MCU [6/24/2013 chendaiwei]
			//[H239]11.2.3  End-user system owns the token and wants to release the token 
			//The end-user system shall send presentationTokenRelease.
			if ( MT_MANU_KDC != m_ptMtTable->GetManuId(byMtId)
				&& MT_MANU_KDCMCU != m_ptMtTable->GetManuId(byMtId))
			{
				tMtPolycom = m_ptMtTable->GetMt( byMtId );
				cMsg.SetMsgBody((u8*)&tMtPolycom, sizeof(TMt));
				cMsg.CatMsgBody((u8*)&tH239TokenInfo, sizeof(TH239TokenInfo));
				cMsg.SetEventId(MCU_POLY_RELEASEH239TOKEN_CMD);
				SendMsgToMt(byMtId, MCU_POLY_RELEASEH239TOKEN_CMD, cServMsg);
			}
		}
	}
	else
	{
		//如果令牌释放通知的发送者是KEDAMCU并且令牌请求记录不为0，则说明有其他终端请求过令牌，但还没有给它会令牌确认，那么需要在此给它回相应确认消息，以便它能顺利发起双流
		if (MT_MANU_KDCMCU == m_ptMtTable->GetManuId(tMt.GetMtId()) && m_byNewTokenReqMtid != 0)
		{
			TMt tNewTokenMt = m_ptMtTable->GetMt(m_byNewTokenReqMtid);
			if (MT_MANU_KDCMCU != m_ptMtTable->GetMtType(m_byNewTokenReqMtid))
			{
				if( !m_tDoubleStreamSrc.IsNull() && m_tDoubleStreamSrc.GetMtId() != tNewTokenMt.GetMtId())
				{
					StopDoubleStream( TRUE, FALSE );
				}
				m_byNewTokenReqMtid = 0;
				TH239TokenInfo tSrcH239Info = m_tH239TokenOwnerInfo;
				tSrcH239Info.SetTokenMt(tNewTokenMt);
				TLogicalChannel tH239LogicChnnl;
				SetTimer( MCUVC_WAITOPENDSCHAN_TIMER, 3000);
				cServMsg.SetMsgBody((u8 *)&tNewTokenMt, sizeof(TMt));
				cServMsg.CatMsgBody((u8 *)&tSrcH239Info, sizeof(TH239TokenInfo));
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2,"[ProcMtMcuReleaseH239TokenNotify]Current TokenMt(%d) chnIdx(%d) break(%d),So Return!!\n",
					tNewTokenMt.GetMtId(),tSrcH239Info.GetChannelId(),tSrcH239Info.GetSymmetryBreaking());	
				if( ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tNewTokenMt.GetMtId()) ||
					( MT_MANU_AETHRA == m_ptMtTable->GetManuId(tNewTokenMt.GetMtId()) &&
					MT_TYPE_MT != m_ptMtTable->GetMtType(tNewTokenMt.GetMtId()) ) )
					&& m_ptMtTable->GetMtLogicChnnl( tNewTokenMt.GetMtId(), LOGCHL_SECVIDEO, &tH239LogicChnnl, FALSE ) )
				{
					if (m_tConf.GetConfAttrb().IsSatDCastMode() /*&& IsOverSatCastChnnlNum(tNewTokenMt.GetMtId())*/
						&& IsMultiCastMt(tNewTokenMt.GetMtId())
						&& //IsSatMtOverConfDCastNum(tNewTokenMt))
						!IsSatMtCanContinue(tNewTokenMt.GetMtId(),emstartDs))
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtMcuReleaseH239TokenNotify] over max upload mt num.\n");
						cServMsg.SetEventId(MCU_MT_GETH239TOKEN_NACK);	
						SendMsgToMt( tNewTokenMt.GetMtId(), MCU_MT_GETH239TOKEN_NACK, cServMsg);
						return;
					}
					StartDoubleStream( tNewTokenMt, tH239LogicChnnl );
				}	
				
				cServMsg.SetEventId(MCU_MT_GETH239TOKEN_ACK);	
				SendMsgToMt( tNewTokenMt.GetMtId(), MCU_MT_GETH239TOKEN_ACK, cServMsg);

				m_tH239TokenOwnerInfo.SetChannelId( tSrcH239Info.GetChannelId() );
				m_tH239TokenOwnerInfo.SetSymmetryBreaking( tSrcH239Info.GetSymmetryBreaking() );
				m_tH239TokenOwnerInfo.SetTokenMt( tNewTokenMt );
				NotifyH239TokenOwnerInfo( NULL );
			}
		}
	}
	
	return;
}

/*=============================================================================
    函 数 名： procWaitOpenDSChanTimeout
    功    能： 终端申请239token后未打开双流逻辑通道超时处理
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2011/9/22    3.6		朱胜泽                  创建
=============================================================================*/
void CMcuVcInst::procWaitOpenDSChanTimeout( void )
{
    //如果当前双流源双流通道打开则不释放239拥有者
    TLogicalChannel tLogicalChannel;
    if ( !m_tDoubleStreamSrc.IsNull() &&
         m_ptMtTable->GetMtLogicChnnl( m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE ))
    {
        LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[procWaitOpenDSChanTimeout]DsChan has be open!\n");
        return;
    }
    else if(!m_tH239TokenOwnerInfo.m_tH239TokenMt.IsNull())
    {
        LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[procWaitOpenDSChanTimeout]mt:%d open DsChan time out after 239 requst!\n",
            m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId() );
        ClearH239TokenOwnerInfo( &m_tH239TokenOwnerInfo.m_tH239TokenMt );  
        NotifyH239TokenOwnerInfo( NULL );
    }

	// 双流开启超时需清vmp双流跟随通道
	TEqp tVmpEqp;
	TVMPParam_25Mem tConfVmpParam;
	TPeriEqpStatus tPeriEqpStatus;
	TVMPParam_25Mem tVmpParam;
	u8 byDSChl;
	TVMPMember tVmpMember;
	TVmpChnnlInfo tVmpChnnlInfo;
	TVMPParam_25Mem tLastVmpParam;
	CServMsg cServMsg;
	u8 byTmp = 0XFF;//此字段保留,因旧mpu有做处理,为兼容,另开一个字节标识关闭通道
	u8 byAdd = 0;//标识关闭通道
	for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
	{
		if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
		{
			continue;
		}
		tVmpEqp = g_cMcuVcApp.GetEqp( m_abyVmpEqpId[byIdx] );
		tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
		if (tConfVmpParam.GetVMPMode() == CONF_VMPMODE_CTRL)
		{
			if (g_cMcuVcApp.GetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus))
			{
				tVmpParam = tPeriEqpStatus.m_tStatus.tVmp.GetVmpParam();
				//有双流跟随通道
				byDSChl = tVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_DSTREAM);
				if (byDSChl < MAXNUM_VMP_MEMBER)
				{
					tVmpMember = *tVmpParam.GetVmpMember(byDSChl);
					if (!tVmpMember.IsNull())
					{
						tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
						//停交换
						StopSwitchToPeriEqp(tVmpEqp.GetEqpId(), byDSChl, FALSE, MODE_SECVIDEO);
						tVmpChnnlInfo.ClearChnlByVmpPos(byDSChl); //清该通道前适配
						g_cMcuVcApp.SetVmpChnnlInfo(tVmpEqp, tVmpChnnlInfo);
						//双流跟随更新通道
						tVmpMember.SetNull();
						tVmpParam.SetVmpMember(byDSChl, tVmpMember);
						tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tVmpParam);	//外设状态更新
						g_cMcuVcApp.SetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
						//更新会议vmpparam
						if (byDSChl == tConfVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_DSTREAM))
						{
							tConfVmpParam.SetVmpMember(byDSChl, tVmpMember);
							//m_tConf.m_tStatus.SetVmpParam(tVmpParam);
							g_cMcuVcApp.SetConfVmpParam(tVmpEqp, tConfVmpParam);
							// 更新LastVmpParam前,刷新vmp双流跟随通道台标
							RefreshVmpChlMemalias(tVmpEqp.GetEqpId());
							tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tVmpEqp);
							tLastVmpParam.SetVmpMember(byDSChl, tVmpMember);//更新m_tLastVmpParam
							g_cMcuVcApp.SetLastVmpParam(tVmpEqp, tLastVmpParam);
						}
						//告诉外设清除该通道
						{
							LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "[StopDoubleStream]Send MCU_VMP_ADDREMOVECHNNL_CMD To Vmp, byChnnl:%d.\n", byDSChl);
							cServMsg.Init();
							cServMsg.SetChnIndex(byDSChl);
							cServMsg.SetMsgBody( &byTmp, sizeof(byTmp) );
							cServMsg.CatMsgBody( &byAdd, sizeof(byAdd) );
							SendMsgToEqp( tVmpEqp.GetMtId(), MCU_VMP_ADDREMOVECHNNL_CMD, cServMsg);
						}
						cServMsg.Init();
						cServMsg.SetEqpId(tVmpEqp.GetEqpId());
						cServMsg.SetMsgBody((u8*)&tVmpParam, sizeof(tVmpParam));
						SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );
						/*cServMsg.SetMsgBody( ( u8 * )&tPeriEqpStatus, sizeof( tPeriEqpStatus ) );
						SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );*/
					}
				}
			}
		}

	}
    return;    
}

/*=============================================================================
    函 数 名： ProcMcsMcuSetMtVolumeCmd
    功    能： 会控给MCU的 设置终端音量 命令
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/12/26  4.0			周广程                  创建
=============================================================================*/
void CMcuVcInst::ProcMcsMcuSetMtVolumeCmd(const CMessage * pcMsg)
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt tMt = *(TMt *)cServMsg.GetMsgBody();

	if( !m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
	{
		return;
	}

	u8 byVolumeType = *(u8 *)( cServMsg.GetMsgBody() + sizeof(TMt) );
	u8 byVolume = *(u8 *)( cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) );
	cServMsg.SetDstMtId( tMt.GetMtId() );
	cServMsg.SetEventId( MCU_MT_SETMTVOLUME_CMD );
	cServMsg.SetMsgBody( &byVolumeType, sizeof(u8) );
	cServMsg.CatMsgBody( &byVolume, sizeof(u8) );
	SendMsgToMt( tMt.GetMtId(), MCU_MT_SETMTVOLUME_CMD, cServMsg );
	return;
}

/*=============================================================================
    函 数 名： ProcPolyMCUH239Rsp
    功    能： 处理对polycom令牌获取响应
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2008/3/20   4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::ProcPolyMCUH239Rsp( const CMessage * pcMsg )
{
	STATECHECK;

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	TMt tPolyMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());

	switch(pcMsg->event)
	{
	case POLY_MCU_GETH239TOKEN_ACK:			//获取PolyMCU的H239TOKEN 同意应答
		{
			TLogicalChannel tSecChnnl;
			if (m_ptMtTable->GetMtLogicChnnl(tPolyMt.GetMtId(), LOGCHL_SECVIDEO, &tSecChnnl, TRUE))
			{
				//zjl 20110510 Mp:StartSwitchToAll 替换 StartSwitchToSubMt 这里原来参数bH239chn为什么没有填，这样交换应该建不起来！
				//g_cMpManager.StartSwitchToSubMt(m_tDoubleStreamSrc, 0, tPolyMt, MODE_SECVIDEO);
				TSwitchGrp tSwitchGrp;
				tSwitchGrp.SetSrcChnl(0);
				tSwitchGrp.SetDstMtNum(1);
				tSwitchGrp.SetDstMt(&tPolyMt);
				g_cMpManager.StartSwitchToAll(m_tDoubleStreamSrc, 1, &tSwitchGrp, MODE_SECVIDEO);
			}
			else
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcPolyMCUH239Rsp] PolyMcu.%d Fwd chnnl should not be closed, reinvite it, please\n", tPolyMt.GetMtId() );
			}
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[ProcPolyMCUH239Rsp] PolyMcu.%d accept TokenReq, POLY_MCU_GETH239TOKEN_ACK received\n", tPolyMt.GetMtId() );
			break;
		}
	case POLY_MCU_GETH239TOKEN_NACK:		//获取PolyMCU的H239TOKEN 拒绝应答
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcPolyMCUH239Rsp] GETH239TOKEN_NACK\n");
			break;
		}
	case POLY_MCU_OWNH239TOKEN_NOTIF:		//PolyMCU通知当前的TOKEN的拥有者
		{
			//FIXME：消息体是什么，打印一下，要不要作响应
			//谁拥有了H239
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcPolyMCUH239Rsp] POLY_MCU_OWNH239TOKEN_NOTIF\n");
			break;
		}
	case POLY_MCU_RELEASEH239TOKEN_CMD:		//PolyMCU释放H329TOKEN 命令
		{
			//双流和第一路Flowctrl回去
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcPolyMCUH239Rsp] POLY_MCU_RELEASEH239TOKEN_CMD\n");
			break;
		}
	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcPolyMCUH239Rsp] unexpected msg.%d<%s>\n", pcMsg->event, OspEventDesc(pcMsg->event) );
		break;
	}
	return;
}

/*====================================================================
    函数名      ：ProcMtMcuCloseLogicChnnlCmd
    功能        ：终端发来的关闭逻辑通道请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/04/04    3.0         胡昌威          创建
====================================================================*/
void CMcuVcInst::ProcMtMcuCloseLogicChnnlNotify( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt;
	u8  byChannelType = 0;
	TLogicalChannel	tLogicChnnl, tH239LogicChnnl, tNullLogicChnnl;

	STATECHECK

	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	tLogicChnnl = *( TLogicalChannel * )( cServMsg.GetMsgBody() );
	BOOL32 bOut = ISTRUE(*(cServMsg.GetMsgBody()+sizeof(TLogicalChannel)));
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "Mt%d close Out.%d logical channel received!\n", tMt.GetMtId(), bOut );

	if( !m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
	{
		return;
	}

	//改变通道状态
	if( bOut )
	{
		if( MODE_VIDEO == tLogicChnnl.GetMediaType() )
		{
			m_ptMtTable->SetMtVideoRecv( tMt.GetMtId(), FALSE );
		}
		if( MODE_AUDIO == tLogicChnnl.GetMediaType() )
		{
			m_ptMtTable->SetMtAudioRecv( tMt.GetMtId(), FALSE );
		}
		if( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
		{
			m_ptMtTable->SetMtVideo2Recv( tMt.GetMtId(), FALSE );
			//20100708_tzy 当第二路视频关闭时，应恢复原来第一路视频的码率，并通知终端第一路视频码率恢复过来。
			TLogicalChannel tLogicalChannel;
			if (m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, TRUE ))
			{
				u16 wMtDialBitrate = m_ptMtTable->GetRcvBandWidth(tMt.GetMtId());
				//[nizhijun 2010/11/2] Bug00040303
				//tLogicalChannel.SetFlowControl( wMtDialBitrate );
				//cServMsg.SetMsgBody( (u8*)&tLogicalChannel, sizeof( tLogicalChannel ) );
				//SendMsgToMt( tMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg );
				m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), wMtDialBitrate, LOGCHL_VIDEO);
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuCloseLogicChnnlNotify]tMt.GetMtId() = %d, MtBitrate = %d LOGCHL_VIDEO ReStore\n",
					tMt.GetMtId(), wMtDialBitrate);	
			}
		}

		// 卫星终端直接调此接口 [pengguofeng 3/5/2013]
		if ( IsMultiCastMt(tMt.GetMtId()))
		{
			StopSwitchToSubMt(1, &tMt, tLogicChnnl.GetMediaType());
		}
	}
	else
	{
		if( MODE_VIDEO == tLogicChnnl.GetMediaType() )
		{
			m_ptMtTable->SetMtVideoSend( tMt.GetMtId(), FALSE );
		}
		if( MODE_AUDIO == tLogicChnnl.GetMediaType() )
		{
			m_ptMtTable->SetMtAudioSend( tMt.GetMtId(), FALSE );
		}
		if( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
		{
			m_ptMtTable->SetMtVideo2Send( tMt.GetMtId(), FALSE );
		}
		MtStatusChange( &tMt,TRUE );
	}

	//清空级联的上级的标识
	if(MEDIA_TYPE_MMCU == tLogicChnnl.GetChannelType() )
	{
		byChannelType = LOGCHL_MMCUDATA;
		if(tMt == m_tCascadeMMCU)
		{
			//清空上级MCU锁定
			if( m_tConf.m_tStatus.GetProtectMode() == CONF_LOCKMODE_LOCK &&
				tMt.GetMtId() == m_tConfProtectInfo.GetLockedMcuId() )
			{
				m_tConf.m_tStatus.SetProtectMode(CONF_LOCKMODE_NONE);
                m_tConfProtectInfo.SetLockByMcu(0);
                m_tConfProtectInfo.SetLockByMcs(0);
			}

			//lukunpeng 2010/06/17 此处还不能释放，在RemoveJoinedMT时还需要此变量拆交换
			/*
			m_tCascadeMMCU.SetNull();
			m_tConfAllMtInfo.m_tMMCU.SetNull();
            m_tConfInStatus.SetNtfMtStatus2MMcu(FALSE);
			*/
		}
	}
	else if( MEDIA_TYPE_H224 == tLogicChnnl.GetChannelType() )
	{
		byChannelType = LOGCHL_H224DATA;
		TMtStatus tMtStatus;
		m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tMtStatus);
		tMtStatus.SetIsEnableFECC(FALSE);
		m_ptMtTable->SetMtStatus(tMt.GetMtId(), &tMtStatus);
	}
	else if( MEDIA_TYPE_T120 == tLogicChnnl.GetChannelType() )
	{
		byChannelType = LOGCHL_T120DATA;
	}
	else if( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
	{
		byChannelType = LOGCHL_SECVIDEO;
		
        // zbq [08/31/2007] 这里首先判断通道方向，否则导致误判
		if(!bOut && 
            m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tH239LogicChnnl, FALSE ))
		{
			TLogicalChannel tFirstChnnl;
			if( m_tDoubleStreamSrc == tMt )
			{
				StopDoubleStream( FALSE, FALSE );
				//卫星会议下，如果是卫星终端主动离会，双流源需要让该终端停止发送双流，否则计数会错误
				if( IsMultiCastMt(tMt.GetMtId()) )
				{
					CServMsg cTempServMsg;
					tH239LogicChnnl.SetFlowControl(0);
					cTempServMsg.SetMsgBody((u8*)&tH239LogicChnnl, sizeof(tH239LogicChnnl));
					SendMsgToMt(tMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cTempServMsg);
				}
			}
			else if(m_tDoubleStreamSrc.IsNull() 
				&& (tMt.GetMtType() == MT_TYPE_MMCU || tMt.GetMtType() == MT_TYPE_SMCU)
				&& m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(),LOGCHL_VIDEO,&tFirstChnnl,FALSE))
			{
				
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuCloseLogicChnnlNotify]adjust MT<%d> MSream send BR from.%d to.%d due to DStream chnnl closed!\n",
					tMt.GetMtId(),tFirstChnnl.GetFlowControl(),m_ptMtTable->GetSndBandWidth(tMt.GetMtId()));
				
				tFirstChnnl.SetFlowControl(m_ptMtTable->GetSndBandWidth(tMt.GetMtId()));
				m_ptMtTable->SetMtLogicChnnl(tMt.GetMtId(),LOGCHL_VIDEO,&tFirstChnnl,FALSE);
			}

			if (m_tSecVidBrdSrc == tMt)
			{
				TMt tNullMt;
				ChangeSecSpeaker(tNullMt);
			}
		}
        // zbq [08/31/2007] 这里首先判断通道方向，否则导致误判
		else if(bOut &&
                m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tH239LogicChnnl, TRUE ))
		{
            // 顾振华 [4/30/2006] 如果终端主动关闭我们发给它的通道，则停交换
            // 否则，就是该终端尝试打开双流通道
            // 但是它不是双流源被NCU拒绝后的关闭通知
            // MCU不需要做处理

            // 卫星会议不停交换 [6/2/2006]
            if ( !m_tConf.GetConfAttrb().IsSatDCastMode() ||
                 !IsMultiCastMt(tMt.GetMtId()))
            {
				//zjl 20110510 Mp: StopSwitchToSubMt 接口重载替换 
                //g_cMpManager.StopSwitchToSubMt( tMt, MODE_SECVIDEO, TRUE  );
				g_cMpManager.StopSwitchToSubMt(m_byConfIdx, 1, &tMt, MODE_SECVIDEO);
            }
		}
	}
	else if( MODE_VIDEO == tLogicChnnl.GetMediaType() )
	{
		byChannelType = LOGCHL_VIDEO;
        // 卫星会议不停交换 [6/2/2006]
        if ( !m_tConf.GetConfAttrb().IsSatDCastMode() ||
             !IsMultiCastMt(tMt.GetMtId()) )
        {
			TMtStatus tDstStatus;
			tDstStatus.SetNull();
		    if(bOut)
		    {
				//1.停目的终端到prs的rtcp交换(StopSwitchToSubMt里会请当前目的终端源信息，这里提前处理)
				StopPrsMemberRtcp(tMt, MODE_VIDEO);
				

				//3.停选看
				m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tDstStatus);
				TMt tSelSrc = tDstStatus.GetSelectMt(MODE_VIDEO);
				if( !tSelSrc.IsNull() && IsNeedSelAdpt(tSelSrc, tMt, MODE_VIDEO) )
				{
					//[2011/08/30/zhangli]Bug00063241如果这里选看的是下级终端，需要FreeRecvSpy
					StopSelAdapt(tSelSrc, tMt, MODE_VIDEO);
					FreeRecvSpy(tSelSrc, MODE_VIDEO);
					NotifyMtReceive( tMt,tMt.GetMtId() );
				}
				else
				{
					StopSwitchToSubMt(1, &tMt, MODE_VIDEO);
					TConfAttrb tConfattrb = m_tConf.GetConfAttrb();
					if (tConfattrb.IsResendLosePack())
					{
						u8 bySrcChn = 0;
						if (EQP_TYPE_VMP == tSelSrc.GetEqpType())
						{
							bySrcChn = GetVmpOutChnnlByDstMtId( tMt.GetMtId(), tSelSrc.GetEqpId());
						}
						if(0xff != bySrcChn)
						{
							BuildRtcpSwitchForSrcToDst(tMt, tSelSrc, MODE_VIDEO, 0, bySrcChn, FALSE);
						}
					}
				}
		    }
		    else 
		    {
				//2.停所有多对一规则
				StopRtcpSwitchAlltoOne(tMt, 0, MODE_VIDEO);
 				TMt tOrgSrc;
				TMt tLocalSrc;
				tOrgSrc.SetNull();
				for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
				{
					if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
					{
						continue;
					}
					if (byMtId == tMt.GetMtId())
					{
						continue;
					}
					tDstStatus.SetNull();
					m_ptMtTable->GetMtStatus(byMtId, &tDstStatus);
					tOrgSrc   = tDstStatus.GetSelectMt(MODE_VIDEO);
					tLocalSrc = tOrgSrc;
					tLocalSrc = GetLocalMtFromOtherMcuMt(tLocalSrc);
					if ( tLocalSrc == tMt )
					{
						TMt tDstMt = m_ptMtTable->GetMt(byMtId);
						StopSelectSrc(tDstMt,MODE_VIDEO);
					}
				}

				//如果当前终端同时回传到上级，回收适配器并停交换
				if(IsSupportMultiSpy())
				{
					TSimCapSet tSrcCap;
					CSendSpy *ptSndSpy = NULL;
					for (u8 bySpyIdx = 0; bySpyIdx < MAXNUM_CONF_SPY; bySpyIdx++)
					{
						ptSndSpy = m_cLocalSpyMana.GetSendSpy(bySpyIdx);
						if (NULL == ptSndSpy)
						{
							continue;
						}
						if (ptSndSpy->GetSpyMt().IsNull() || ptSndSpy->GetSimCapset().IsNull() || !(ptSndSpy->GetSpyMt() == tMt))
						{
							continue;
						}
						tSrcCap = m_ptMtTable->GetSrcSCS(GetLocalMtFromOtherMcuMt(ptSndSpy->GetSpyMt()).GetMtId());
						if(ptSndSpy->GetSimCapset() < tSrcCap)
						{
							StopSpyAdapt(ptSndSpy->GetSpyMt(), ptSndSpy->GetSimCapset(), MODE_VIDEO);
						}
						break;
					}
				}
		    }
        }
	}
	else if( MODE_AUDIO == tLogicChnnl.GetMediaType() )
	{
		byChannelType = LOGCHL_AUDIO;
        // 卫星会议不停交换 [6/2/2006]
        if ( !m_tConf.GetConfAttrb().IsSatDCastMode() ||
             !IsMultiCastMt(tMt.GetMtId()) )
        {
			TMtStatus tDstStatus;
			tDstStatus.SetNull();
			if(bOut)
			{				
				//1.停目的终端到prs的rtcp交换(StopSwitchToSubMt里会请当前目的终端源信息，这里提前处理)
				StopPrsMemberRtcp(tMt, MODE_AUDIO);
				

				//3.停选看
				m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tDstStatus);
				TMt tSelSrc = tDstStatus.GetSelectMt(MODE_AUDIO);
				if( !tSelSrc.IsNull() && IsNeedSelAdpt(tSelSrc, tMt, MODE_AUDIO) )
				{
					StopSelAdapt(tSelSrc, tMt, MODE_AUDIO);
					FreeRecvSpy(tSelSrc, MODE_AUDIO);
					NotifyMtReceive( tMt,tMt.GetMtId() );
				}
				else
				{
					StopSwitchToSubMt(1, &tMt, MODE_AUDIO);
					TConfAttrb tTmpConfattrb = m_tConf.GetConfAttrb();
					if (tTmpConfattrb.IsResendLosePack())
					{
						u8 bySrcChn = 0;
						if (EQP_TYPE_VMP == tSelSrc.GetEqpType())
						{
							bySrcChn = GetVmpOutChnnlByDstMtId( tMt.GetMtId(), tSelSrc.GetEqpId());
						}
						if(0xff != bySrcChn)
						{
							BuildRtcpSwitchForSrcToDst(tMt, tSelSrc, MODE_AUDIO, 0, bySrcChn, FALSE);
						}
					}
				}
			}
			else 
			{
				//2.停所有多对一规则
				StopRtcpSwitchAlltoOne(tMt, 0, MODE_AUDIO);
				TMt tOrgSrc;
				TMt tLocalSrc;
				tOrgSrc.SetNull();
				for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
				{
					if (!m_tConfAllMtInfo.MtJoinedConf(byMtId))
					{
						continue;
					}
					if (byMtId == tMt.GetMtId())
					{
						continue;
					}
					tDstStatus.SetNull();
					m_ptMtTable->GetMtStatus(byMtId, &tDstStatus);
					tOrgSrc   = tDstStatus.GetSelectMt(MODE_AUDIO);
					tLocalSrc = tOrgSrc;
					tLocalSrc = GetLocalMtFromOtherMcuMt(tLocalSrc);
					if ( tLocalSrc == tMt )
					{
						TMt tDstMt = m_ptMtTable->GetMt(byMtId);
						StopSelectSrc(tDstMt,MODE_AUDIO);
					}
				}

				//如果当前终端同时回传到上级，回收适配器并停交换
				if(IsSupportMultiSpy())
				{
					TSimCapSet tSrcCap;
					CSendSpy *ptSndSpy = NULL;
					for (u8 bySpyIdx = 0; bySpyIdx < MAXNUM_CONF_SPY; bySpyIdx++)
					{
						ptSndSpy = m_cLocalSpyMana.GetSendSpy(bySpyIdx);
						if (NULL == ptSndSpy)
						{
							continue;
						}
						if (ptSndSpy->GetSpyMt().IsNull() || ptSndSpy->GetSimCapset().IsNull() || !(ptSndSpy->GetSpyMt() == tMt))
						{
							continue;
						}
						tSrcCap = m_ptMtTable->GetSrcSCS(GetLocalMtFromOtherMcuMt(ptSndSpy->GetSpyMt()).GetMtId());
						/*if (ptSndSpy->GetSimCapset().GetAudioMediaType() != MEDIA_TYPE_NULL &&
							tSrcCap.GetAudioMediaType() != MEDIA_TYPE_NULL && 
							ptSndSpy->GetSimCapset().GetAudioMediaType() != tSrcCap.GetAudioMediaType())
						{
							StopSpyAdapt(ptSndSpy->GetSpyMt(), ptSndSpy->GetSimCapset(), MODE_AUDIO);
						}*/
						if( IsNeedSpyAdpt( ptSndSpy->GetSpyMt(),tSrcCap,MODE_AUDIO) )
						{
							StopSpyAdapt(ptSndSpy->GetSpyMt(), ptSndSpy->GetSimCapset(), MODE_AUDIO);
						}
						break;
					}
				}
		    }
        }
	}


	if ( bOut )
	{
		//刷新第一路适配参数
		if (m_tConf.m_tStatus.IsVidAdapting() && MODE_VIDEO == tLogicChnnl.GetMediaType() )
		{
			RefreshBrdBasParamForSingleMt(tMt.GetMtId(), MODE_VIDEO, TRUE);
		}	
		
		//刷新第二路适配参数
		if (!m_tDoubleStreamSrc.IsNull() && 
			!(m_tDoubleStreamSrc == tMt) &&
			MODE_SECVIDEO == tLogicChnnl.GetMediaType()
			)
		{
			RefreshBrdBasParamForSingleMt(tMt.GetMtId(), MODE_SECVIDEO, TRUE);
		}
	}

    //关闭通道要对h460穿越补洞
    if ( ( MODE_SECVIDEO == tLogicChnnl.GetMediaType() ||
           MODE_AUDIO == tLogicChnnl.GetMediaType() ||
           MODE_VIDEO == tLogicChnnl.GetMediaType() ) &&
           m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_bIsPinHole)
    {
        u8 byMpId = m_ptMtTable->GetMpId( tMt.GetMtId() );

        if( bOut )
        {//前向通道

            //rtcp
            StopH460PinHole(byMpId, 
                tLogicChnnl.m_tSndMediaCtrlChannel.GetPort(), 
                tLogicChnnl.m_tRcvMediaCtrlChannel.GetIpAddr(),
                tLogicChnnl.m_tRcvMediaCtrlChannel.GetPort()); 
        }
        else
        {//后向通道

            //rtp
            u32 dwRtpSndIp = 0;
            u16 wRtpSndPort = 0;
            if (MODE_AUDIO == tLogicChnnl.GetMediaType())
            {
                dwRtpSndIp = m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tAudRtpSndTransportAddr.GetIpAddr();
                wRtpSndPort = htons( m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tAudRtpSndTransportAddr.GetPort() );
            }
            else if(MODE_VIDEO == tLogicChnnl.GetMediaType())
            {
                dwRtpSndIp = m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tVidRtpSndTransportAddr.GetIpAddr();
                wRtpSndPort = htons(m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tVidRtpSndTransportAddr.GetPort());
            }
            else if(MODE_SECVIDEO == tLogicChnnl.GetMediaType())
            {
                dwRtpSndIp = m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tSecRtpSndTransportAddr.GetIpAddr();
                wRtpSndPort = htons( m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tSecRtpSndTransportAddr.GetPort() );
            }
            StopH460PinHole(byMpId, 
                tLogicChnnl.m_tRcvMediaChannel.GetPort(), 
                dwRtpSndIp,
                wRtpSndPort);   
            
            //rtcp
            StopH460PinHole(byMpId, 
                tLogicChnnl.m_tRcvMediaCtrlChannel.GetPort(), 
                tLogicChnnl.m_tSndMediaCtrlChannel.GetIpAddr(),
                tLogicChnnl.m_tSndMediaCtrlChannel.GetPort()); 
        }
//         StopH460PinHole(byMpId, 
//             tLogicChnnl.m_tRcvMediaChannel.GetPort(), 
//             m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tSecRtpSndTransportAddr.GetIpAddr(),
//             htons(m_ptMtTable->m_atMtData[tMt.GetMtId() - 1].m_tSecRtpSndTransportAddr.GetPort()));    
//         StopH460PinHole(byMpId, 
//             tLogicChnnl.m_tRcvMediaCtrlChannel.GetPort(), 
//             tLogicChnnl.m_tSndMediaCtrlChannel.GetIpAddr(),
//             tLogicChnnl.m_tSndMediaCtrlChannel.GetPort());  
    }

	//lukunpeng 2010/06/17 此处还不能释放，在RemoveJoinedMT时还需要Clear此通道
    //[9/21/2011 zhushengze]对于正常关闭双流,双流通道不会被在之后被Clear，加入双流判断
	if (tMt.GetMtType() != MT_TYPE_MMCU || MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuCloseLogicChnnlNotify]MediaType: %d ChannelType: %d\n",
			byChannelType, tNullLogicChnnl.GetChannelType());
		//卫星会议下，卫星终端主动退会，在后向主流逻辑通道清空前应该让该卫星终端停发主流，否则会导致计数错误
		if(MODE_VIDEO == tLogicChnnl.GetMediaType() && IsMultiCastMt(tMt.GetMtId()) && !bOut)
		{
			NotifyMtSend(tMt.GetMtId(),MODE_VIDEO,FALSE);
		}
		m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), byChannelType, &tNullLogicChnnl, bOut );
	}
	
	return;
}

/*====================================================================
    函数名      ：ProcMtCapSet
    功能        ：处理终端能力集
    算法实现    ：
    引用全局变量：
    输入参数说明：[in]  TMt tMt 终端
	              [in]  TCapSupport tCapSupport 终端能力集
                  [in]  TCapSupport tCapSupportEx 终端扩展能力集
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/04/04    3.0         胡昌威          创建
	2005/12/16	4.0			张宝卿			T120集成通道打开处理
    2006/04/11  4.0         顾振华          支持建立分散会议，交换到组播地址
    2007/08/28  4.0         顾振华          8000B/8000C 双媒体能力会议支持
    2007/10/13  4.0         张宝卿          终端扩展能力集/FEC 分析支持
====================================================================*/
void CMcuVcInst::ProcMtCapSet(TMt tMt, TMultiCapSupport &tCapSupport, u8 bMtHasVidCap /* = 1 */)
{
    TLogicalChannel tDataLogicChnnl;

    //zbq[04/09/2009] Tandberg 能力作弊
    if (MT_MANU_TAIDE == m_ptMtTable->GetManuId((tMt.GetMtId())) &&
        IsHDConf(m_tConf))
    {		
        if (g_cMcuVcApp.IsSendFakeCap2Taide() && 
			0 == strcmp("58", m_ptMtTable->GetVersionId(tMt.GetMtId())))
        {
            tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_CIF);
        }
        else if (g_cMcuVcApp.IsSendFakeCap2TaideHD())
        {
            tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_HD720);
        }
    }

    //zbq[09/12/2009] 华为能力作弊
    if (MT_MANU_HUAWEI == m_ptMtTable->GetManuId(tMt.GetMtId()))
    {
        /*if (g_cMcuVcApp.IsSendFakeCap2HuaweiSDEp() &&
            0 == strcmp("Huawei H.323 Stack Verion 1.1", m_ptMtTable->GetVersionId(tMt.GetMtId())))
        {
            tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_CIF);
        }*/
		u8 byVideoFormat = 0;
        if ( m_tConf.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&			
            g_cMcuVcApp.GetFakeCap2HuaweiSDEp(m_ptMtTable->GetIPAddr(tMt.GetMtId()),byVideoFormat) )
        {
			if(m_tConf.GetConfAttrbEx().IsResExCif() && VIDEO_FORMAT_CIF == byVideoFormat )
			{
				tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_CIF);
			}
			if( m_tConf.GetConfAttrbEx().IsResEx4Cif() && VIDEO_FORMAT_4CIF == byVideoFormat )
			{
				tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_4CIF);
			}
			if( m_tConf.GetConfAttrbEx().IsResEx720() && VIDEO_FORMAT_HD720 == byVideoFormat )
			{
				tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_HD720);
			}            
        }
    }

	TMt tNullMt;
	tNullMt.SetNull();
	u8 byDriId = m_ptMtTable->GetDriId(tMt.GetMtId());

	//[liu lijiu][20101028] USBKEY License 授权数据校验
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
#ifdef  _LINUX_
#ifndef _NOUSBKEY_
	u16 wUSBKEYError = 0;
	if( !g_cMcuVcApp.GetLicesenUSBKEYFlag(wUSBKEYError))
	{    
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "Invalid USBKEY license data, the error is :%d\n", wUSBKEYError);
		NotifyMcsAlarmInfo( 0, wUSBKEYError);
		RemoveJoinedMt( tMt, TRUE );
		return;
	}
#endif//_NOUSB_ ends
#endif//_LINUX_ ends
#endif//_8KE_ ends

	u16 wError = 0;
	//增加语音接入计数
    if(!g_cMcuVcApp.IncAudMtAdpNum(m_byConfIdx, tMt.GetMtId(), bMtHasVidCap, wError))
	{
		CServMsg cSendServMsg;
		cSendServMsg.SetEventId( MCU_MCS_CALLMTMODE_NOTIF );
		cSendServMsg.SetMsgBody( (u8*)&m_ptMtTable->m_atMtExt[tMt.GetMtId()-1], sizeof(TMtExt) );
        SendMsgToAllMcs( MCU_MCS_CALLMTMODE_NOTIF, cSendServMsg );
		NotifyMcsAlarmInfo( 0, wError);
		RemoveJoinedMt(tMt, TRUE);
		return;
	}

	if(g_cMcuVcApp.IsMtNumOverMcuLicense(m_byConfIdx, tMt, tCapSupport, wError))
	{    
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "Call Mt failed due to license limit, the error is :%d\n", wError);
		
		//修改终端呼叫模式为手动呼叫，避免一直弹出呼叫失败提示
		m_ptMtTable->SetCallMode( tMt.GetMtId(),CONF_CALLMODE_NONE );
		
		//刷新界面终端呼叫方式
		CServMsg cSendServMsg;
		cSendServMsg.SetEventId( MCU_MCS_CALLMTMODE_NOTIF );
		cSendServMsg.SetMsgBody( (u8*)&m_ptMtTable->m_atMtExt[tMt.GetMtId()-1], sizeof(TMtExt) );
        SendMsgToAllMcs( MCU_MCS_CALLMTMODE_NOTIF, cSendServMsg );
		
		NotifyMcsAlarmInfo( 0, wError);
		RemoveJoinedMt( tMt, TRUE );
		return;
	}

#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
	// 接入板能力检查，对8000E意义不大，可省
	if ( g_cMcuVcApp.GetPerfLimit().IsLimitMtNum() )
	{
		u8 by8KeDriId = tMt.GetDriId();
		
		// 判断终端数量是否超过接入板的限制
		u8 byMtNum = 0;
		u8 byMcuNum = 0;
		//BOOL32 bOverCap = g_cMcuVcApp.GetMtNumOnDri( by8KeDriId, TRUE, byMtNum, byMcuNum );
		//  [12/22/2010 chendaiwei] 下面接口对GetMtNumOnDri进行封装，避免GetMtNumOnDri不同地方调用的歧义
		BOOL32 bOverCap = g_cMcuVcApp.IsMtNumOverDriLimit( by8KeDriId, TRUE, byMtNum, byMcuNum );
		if ( bOverCap )
		{
			CServMsg cServMsg;
			cServMsg.SetMsgBody( (u8*)&tMt, sizeof(tMt) );
			cServMsg.SetErrorCode(ERR_MCU_OVERMAXCONNMT);
			cServMsg.SetConfId( m_tConf.GetConfId() );
			SendMsgToAllMcs( MCU_MCS_CALLMTFAILURE_NOTIF, cServMsg );
			
			cServMsg.SetMsgBody();
			SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);
			
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "[ProcMtMcuMtJoinNotif] mtadp.%d ability(Max=%d) is full! \n", 
				by8KeDriId,
				g_cMcuVcApp.GetMtAdpSupportMtNum(by8KeDriId));
			return;
		}
	}       


	// xliang [10/27/2008] 对于HD MT且内嵌接入情况下的处理
	if(tCapSupport.GetMainVideoType() == MEDIA_TYPE_H264 &&
		(tCapSupport.GetMainVideoResolution() == VIDEO_FORMAT_HD1080 ||
		 tCapSupport.GetMainVideoResolution() == VIDEO_FORMAT_HD720) )
	{

#ifdef _LINUX_
		//内嵌接入	
		if (byDriId == MCU_BOARD_MPC ||
			byDriId == MCU_BOARD_MPCD )
		{
			u8 byIdx;
			u8 byEmptyIdx = MAXHDLIMIT_MPC_MTADP;
			BOOL32 bAlreadyExist = FALSE;
			TMtAdpHDChnnlInfo tHdChnnlInfo;
			tHdChnnlInfo.SetConfIdx(m_byConfIdx);
			tHdChnnlInfo.SetHDMtId(tMt.GetMtId());
			for (byIdx = 0; byIdx < MAXHDLIMIT_MPC_MTADP; byIdx ++)
			{
				if(g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_atHdChnnlInfo[byIdx] == tHdChnnlInfo)
				{
					bAlreadyExist = TRUE;
					break;
				}
				if(g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_atHdChnnlInfo[byIdx].GetHDMtId() == 0 &&
					byEmptyIdx == MAXHDLIMIT_MPC_MTADP)
				{
					byEmptyIdx = byIdx;
				}
			}
			if( !bAlreadyExist)
			{
				if(byEmptyIdx < MAXHDLIMIT_MPC_MTADP )
				{
					g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_atHdChnnlInfo[byEmptyIdx].SetHDMtId(tMt.GetMtId());
					g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_atHdChnnlInfo[byEmptyIdx].SetConfIdx(m_byConfIdx);
				}
				else //接入能力满，上报错误给mcs
				{
                    //修改终端呼叫模式为手动呼叫，避免一直弹出呼叫失败提示
                    m_ptMtTable->SetCallMode( tMt.GetMtId(),CONF_CALLMODE_NONE );

                    //刷新界面终端呼叫方式
                    CServMsg cSendServMsg;
                    cSendServMsg.SetEventId( MCU_MCS_CALLMTMODE_NOTIF );
                    cSendServMsg.SetMsgBody( (u8*)&m_ptMtTable->m_atMtExt[tMt.GetMtId()-1], sizeof(TMtExt) );
                    SendMsgToAllMcs( MCU_MCS_CALLMTMODE_NOTIF, cSendServMsg );

					NotifyMcsAlarmInfo( 0, ERR_MCU_CALLHDMTFAIL);
					RemoveJoinedMt( tMt, TRUE );
					return;
				}
			}
			else
			{
				//已计算在HD接入能力信息中,不做处理
			}
		}
#endif
		
		//  [8/5/2011 chendaiwei]非8KE,非8000b走新的高清接入策略逻辑
#ifndef _MINIMCU_
		//  [7/28/2011 chendaiwei]入会能力集是高清的终端需要占用一个高清接入点,是高清MCU占用两个高清接入点
		BOOL32 bIsMcu = FALSE;
		
		if( tMt.GetMtType() == MT_TYPE_SMCU || tMt.GetMtType() == MT_TYPE_MMCU )
		{
			bIsMcu = TRUE;
		}
		
		if( !g_cMcuVcApp.IncMpcCurrentHDMtNum(m_byConfIdx, tMt.GetMtId(),bIsMcu) )
		{
            m_ptMtTable->SetCallMode( tMt.GetMtId(),CONF_CALLMODE_NONE );
            CServMsg cSendServMsg;
            cSendServMsg.SetEventId( MCU_MCS_CALLMTMODE_NOTIF );
            cSendServMsg.SetMsgBody( (u8*)&m_ptMtTable->m_atMtExt[tMt.GetMtId()-1], sizeof(TMtExt) );
            SendMsgToAllMcs( MCU_MCS_CALLMTMODE_NOTIF, cSendServMsg );
			NotifyMcsAlarmInfo( 0, ERR_MCU_CALLHDMTFAIL);
			RemoveJoinedMt( tMt, TRUE );
			
			return;
		}
#endif
	}
	
#endif


	// xliang [2/4/2009] 根据Mt类型，接入板补计数
	// ====================【说明】======================
	//1,由于最初分配接入板，并不知终端的类型(MT/MCU),只能在此处知道MT类型后，
	//根据终端类型(MT/MCU)补算计数值，(对于MCU，接入板计数值占2个)
	//这里只对终端类型是SMCU的情形(outgoing)进行补计数，对于MMCU(对端incoming)不补计数
	//( 在mcu和mtadplib中对于计数处理都采取了不对称处理，保证主叫/被叫的即时性 )
	//2,若没有多余空位可占，则
	//a)提示出错。
	//------
	//【FIXME】:对于接入板均衡分配原则是没有问题的，但内嵌接入板和HDI板共存时
	//			在HD会议的情况下，接入板并不是严格均衡分配的,而是Mt优先分配到HDI板上，
	//			这样就会造成在内嵌接入板尚有2个空位可分配，但却呼不上MCU的情况。
	//【solution】:改变原有接入板的均衡分配原则(选接入数目最小的一个)，
	//             改为选剩余接入能力最多的一个// xliang [2/16/2009] 
	//------
	//b)清MT资源，封装在函数里执行，这里要忽略Mt类型，计数值统一-1，而不能由于是MCU就-2.
	//3, 目前不支持2个不同接入板各提供1个接入能力凑满2个供MCU占2个位用
	if( tMt.GetMtType() == MT_TYPE_SMCU /*|| tMt.GetMtType() == MT_TYPE_MMCU*/ )
	{
		// [9/9/2010 xliang] 由于有VCS强拆机制，下面的判断可能误判。且接入板处有过滤，这里补计数肯定是可以的。
// 		if ( g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_wMtNum +1 > g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_byMaxMtNum )
// 		{
// 			// xliang [2/16/2009]  该判断应该进不来,在mtadp那已进行过滤。
// 			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMtCapSet]Take MCU as 2 into account of Mtadp access capabilty: fail! CRI.%d(cur: %d, max:%d)\n",
// 				byDriId, 
// 				g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_wMtNum, 
// 				g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_byMaxMtNum );
// 			NotifyMcsAlarmInfo( 0, ERR_MCU_CALLHDMTFAIL);
// 			RemoveJoinedMt( tMt, TRUE, MTLEFT_REASON_NORMAL, TRUE);
// 			return;
// 		}
// 		else
// 		{
            g_cMcuVcApp.IncMtAdpMtNum( byDriId, m_byConfIdx, tMt.GetMtId(), TYPE_MCU);
			g_cMcuVcApp.UpdateAgentAuthMtNum();
			//g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_wMtNum++;
			//AddMtExtraPoint(tMt.GetMtId());
// 		}
	}
//#endif

	//通知其他MCU new mt
    NotifyMcuNewMt(tMt); 

    ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcMtCapSet] Mt main video cap: type.%d, Res.%d, HP.%d \n", tCapSupport.GetMainVideoType(), tCapSupport.GetMainVideoResolution(),tCapSupport.GetMainStreamProfileAttrb() );
	
	// vrs新录播支持，新录播录像时需要开vrs接收通道，判共同能力是否满足
	if (tMt.GetMtType() == MT_TYPE_VRSREC)
	{
		if (m_ptMtTable->GetRecChlType(tMt.GetMtId()) == TRecChnnlStatus::TYPE_RECORD)
		{
			if (!CheckVrsMtCapSet(tMt, tCapSupport))
			{
				//共同能力不匹配，挂断vrs实体
				ReleaseVrsMt(tMt.GetMtId());
				return;
			}
			// 更新vrs共同能力，用于接下来的开通道
			m_ptMtTable->SetMtMultiCapSupport(tMt.GetMtId(), &tCapSupport);
		}
		else
		{
			return;
		}
	}

	u32	dwRcvIp;
	u16	wRcvPort;
	if( !m_ptMtTable->GetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort ) )
	{
		dwRcvIp = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( tMt.GetMtId() ) );
		if( dwRcvIp == 0 )
		{
			return;			    
		}
		
		g_cMcuVcApp.FindMatchedMpIpForMt(m_ptMtTable->GetIPAddr(tMt.GetMtId()),dwRcvIp);
		
		wRcvPort = g_cMcuVcApp.AssignMtPort( tMt.GetConfIdx(), tMt.GetMtId() );
		m_ptMtTable->SetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort );
	}

	
	/*TLogicalChannel	tAudioLogicChnnl;
    TLogicalChannel tVideoLogicChnnl;
	tAudioLogicChnnl.SetMediaType( MODE_AUDIO );
	tVideoLogicChnnl.SetMediaType( MODE_VIDEO );
	TCapSupport	tConfCapSupport = m_tConf.GetCapSupport();
	TSimCapSet tConfMainSCS = tConfCapSupport.GetMainSimCapSet();
	TSimCapSet tConfSecSCS  = tConfCapSupport.GetSecondSimCapSet();
    // guzh [8/28/2007] 第零步，对于8000B/8000C，处理双媒体会议
#ifdef _MINIMCU_
    if ( ISTRUE(m_byIsDoubleMediaConf) )
    {
        // 视频：主码率采用主格式，辅码率采用辅格式（避免纯粹的速率适配）
        u16 wMainBitrate = m_tConf.GetBitRate();
        u16 wSecBitrate = m_tConf.GetSecBitRate();
        u16 wDialBitrate = m_ptMtTable->GetDialBitrate(tMt.GetMtId());
        if ( wDialBitrate == wMainBitrate && 
             tCapSupport.IsSupportMediaType(tConfMainSCS.GetVideoMediaType()) ) 
        {
            tVideoLogicChnnl.SetChannelType(tConfMainSCS.GetVideoMediaType());	
        }
        else if ( wSecBitrate == wDialBitrate &&
                  tCapSupport.IsSupportMediaType(tConfSecSCS.GetVideoMediaType()) )
        {
            tVideoLogicChnnl.SetChannelType(tConfSecSCS.GetVideoMediaType());
        }
        else
        {
            tVideoLogicChnnl.SetChannelType(MEDIA_TYPE_NULL);
        }
        // 音频：和原来相同
        if (tCapSupport.IsSupportMediaType(tConfMainSCS.GetAudioMediaType()))
        {
            tAudioLogicChnnl.SetChannelType(tConfMainSCS.GetAudioMediaType());
        }			
        else if (tCapSupport.IsSupportMediaType(tConfSecSCS.GetAudioMediaType()))
        {
            tAudioLogicChnnl.SetChannelType(tConfSecSCS.GetAudioMediaType());
        }
        else
        {
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtCapSet] (MCU8000b)  tAudioLogicChnnl Set NULL!\n");
            tAudioLogicChnnl.SetChannelType(MEDIA_TYPE_NULL);
        }        
    }    
#endif
    //根据能力进群组
    BOOL32 bAddAccordMSC = TRUE;
    //第一步，判断对方的同时能力集与会议的是否有交集
    if( tConfMainSCS == tCapSupport.GetMainSimCapSet() || 
	    tConfMainSCS == tCapSupport.GetSecondSimCapSet() )
	{
		tAudioLogicChnnl.SetChannelType( tConfMainSCS.GetAudioMediaType() );
		tVideoLogicChnnl.SetChannelType( tConfMainSCS.GetVideoMediaType() );
	}
	else if( tConfSecSCS == tCapSupport.GetMainSimCapSet() || 
			 tConfSecSCS == tCapSupport.GetSecondSimCapSet() )
	{
		tAudioLogicChnnl.SetChannelType( tConfSecSCS.GetAudioMediaType() );
		tVideoLogicChnnl.SetChannelType( tConfSecSCS.GetVideoMediaType() );
        bAddAccordMSC = FALSE;
    }
	//第二步，没有交集后确定打开对方的音频与视频类型
	else
	{
		if( tCapSupport.IsSupportMediaType( tConfMainSCS.GetAudioMediaType() ) && 
			tCapSupport.IsSupportMediaType( tConfMainSCS.GetVideoMediaType() ) )
		{
			tAudioLogicChnnl.SetChannelType( tConfMainSCS.GetAudioMediaType() );
			tVideoLogicChnnl.SetChannelType( tConfMainSCS.GetVideoMediaType() );
		}
		else if( tCapSupport.IsSupportMediaType( tConfSecSCS.GetAudioMediaType() ) && 
			     tCapSupport.IsSupportMediaType( tConfSecSCS.GetVideoMediaType() ) )
		{
			tAudioLogicChnnl.SetChannelType( tConfSecSCS.GetAudioMediaType() );
			tVideoLogicChnnl.SetChannelType( tConfSecSCS.GetVideoMediaType() );	

            bAddAccordMSC = FALSE;
		}
		//第三步，确定打开终端的一个逻辑通道的类型
		else
		{
            if (tCapSupport.IsSupportMediaType(tConfMainSCS.GetAudioMediaType()))
            {
                tAudioLogicChnnl.SetChannelType(tConfMainSCS.GetAudioMediaType());
            }			
            else if (tCapSupport.IsSupportMediaType(tConfSecSCS.GetAudioMediaType()))
            {
                tAudioLogicChnnl.SetChannelType(tConfSecSCS.GetAudioMediaType());
            }
            else
            {
                tAudioLogicChnnl.SetChannelType(MEDIA_TYPE_NULL);
            }
            if (tCapSupport.IsSupportMediaType(tConfMainSCS.GetVideoMediaType())) 
            {
                tVideoLogicChnnl.SetChannelType(tConfMainSCS.GetVideoMediaType());
            }
            else if (tCapSupport.IsSupportMediaType(tConfSecSCS.GetVideoMediaType()))
            {
                tVideoLogicChnnl.SetChannelType(tConfSecSCS.GetVideoMediaType());

                bAddAccordMSC = FALSE;
            }
		}
	}
    TSimCapSet tSimCap;
    tSimCap.Clear();
    if (bAddAccordMSC)
    {
        tSimCap = tCapSupport.GetMainSimCapSet();
    }
    else
    {
        tSimCap = tCapSupport.GetSecondSimCapSet();
    }
	*/

	TLogicalChannel	tAudioLogicChnnl;
    TLogicalChannel tVideoLogicChnnl;
	tAudioLogicChnnl.SetMediaType( MODE_AUDIO );
	tAudioLogicChnnl.SetChannelType(tCapSupport.GetMainAudioType());
	tAudioLogicChnnl.SetAudioTrackNum( tCapSupport.GetMainAudioTrackNum() );
	tVideoLogicChnnl.SetMediaType( MODE_VIDEO );
	tVideoLogicChnnl.SetChannelType(tCapSupport.GetMainVideoType());



	TCapSupport	tConfCapSupport = m_tConf.GetCapSupport();
#ifdef _MINIMCU_
	TSimCapSet tConfMainSCS = tConfCapSupport.GetMainSimCapSet();
	TSimCapSet tConfSecSCS  = tConfCapSupport.GetSecondSimCapSet();
    if ( ISTRUE(m_byIsDoubleMediaConf) )
    {
        // 视频：主码率采用主格式，辅码率采用辅格式（避免纯粹的速率适配）
        u16 wMainBitrate = m_tConf.GetBitRate();
        u16 wSecBitrate = m_tConf.GetSecBitRate();
        u16 wDialBitrate = m_ptMtTable->GetDialBitrate(tMt.GetMtId());
        if ( wDialBitrate == wMainBitrate && 
			tCapSupport.IsSupportMediaType(tConfMainSCS.GetVideoMediaType()) ) 
        {
            tVideoLogicChnnl.SetChannelType(tConfMainSCS.GetVideoMediaType());	
        }
        else if ( wSecBitrate == wDialBitrate &&
			tCapSupport.IsSupportMediaType(tConfSecSCS.GetVideoMediaType()) )
        {
            tVideoLogicChnnl.SetChannelType(tConfSecSCS.GetVideoMediaType());
        }
        else
        {
            tVideoLogicChnnl.SetChannelType(MEDIA_TYPE_NULL);
        }       
    }    
	
#endif
    
    //zbq[10/13/2007] FEC能力不足，拒绝打开终端的通道
	BOOL32 bFECOpenAudioChnnl = TRUE;
	BOOL32 bFECOpenVideoChnnl = TRUE;
	CServMsg cServMsg;	
	u16 wAudioBand = 0;

	
	//发送打开终端语音逻辑通道信息
	//代码修改  取出MTADP侧匹配出的最优音频能力，按照该音频能力打开对方逻辑通道
	if( bFECOpenAudioChnnl &&
        tAudioLogicChnnl.GetChannelType() != MEDIA_TYPE_NULL )
	{		
		wAudioBand = GetAudioBitrate( tAudioLogicChnnl.GetChannelType() );
		tAudioLogicChnnl.SetFlowControl( wAudioBand );
		tAudioLogicChnnl.m_tSndMediaCtrlChannel.SetIpAddr( dwRcvIp );
		tAudioLogicChnnl.m_tSndMediaCtrlChannel.SetPort( wRcvPort + 3 );
		tAudioLogicChnnl.SetMediaEncrypt(m_tConf.GetMediaKey());
		tAudioLogicChnnl.SetActivePayload(GetActivePayload(m_tConf, tAudioLogicChnnl.GetChannelType()));
		
		cServMsg.SetMsgBody( ( u8 * )&tAudioLogicChnnl, sizeof( tAudioLogicChnnl ) );
		SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );

		//打印消息
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Open audio logicChannel request send to Mt%d, type: %s ChnNum(%d)\n", 
				tMt.GetMtId(), GetMediaStr( tAudioLogicChnnl.GetChannelType() ) ,tAudioLogicChnnl.GetAudioTrackNum() );
		
	}
	else
	{
		VCSMTAbilityNotif(tMt, MODE_AUDIO);
		//打印消息
		ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL,  "[ProcMtCapSet] Mt%d not support any conf audio media type, NO open audio logicChannel request send to it!\n", tMt.GetMtId() );
	}	   

    //  xsl [4/26/2006] 判断是否打开终端的视频逻辑通道
    BOOL32 bOpenVideoChnnl = TRUE;

    // guzh [2/26/2007] 通过终端最大的码率计算，    
    u16 wCallRate = m_ptMtTable->GetDialBitrate( tMt.GetMtId() );
    u16 wMtMaxRecvBitrate = 0;

    // 只对Keda终端采用这样的策略，如果是其他终端(目前只是Polycon)，由MtAdp保护    
    if ( MT_MANU_KDC == m_ptMtTable->GetManuId(tMt.GetMtId()) )
    {
		TSimCapSet tMtMainSCS = tCapSupport.GetMainSimCapSet();
		TSimCapSet tMtSecSCS  = tCapSupport.GetSecondSimCapSet();
        if ( !tMtMainSCS.IsNull() )
        {
            wMtMaxRecvBitrate = tMtMainSCS.GetVideoMaxBitRate();
        }
        if ( !tMtSecSCS.IsNull() )
        {
			const u16 wVideoMaxBitR = tMtSecSCS.GetVideoMaxBitRate();
            wMtMaxRecvBitrate = max( wMtMaxRecvBitrate, wVideoMaxBitR );
        }
        // guzh [7/16/2007] 老版本终端的视频逻辑通道是总码率-64
        wMtMaxRecvBitrate += 64;

        wMtMaxRecvBitrate = min ( wMtMaxRecvBitrate, wCallRate );
    }
    else
    {
        wMtMaxRecvBitrate = wCallRate;
    }        

	wAudioBand = GetMaxAudioBiterate(m_tConfEx);
	if( wCallRate < wAudioBand )
	{
		bOpenVideoChnnl = FALSE;
		ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "Open Mt.%d video logic chnnl failed due to wCallRate.%d < wAudioBand.%d\n",
            tMt.GetMtId(),wCallRate,wAudioBand );
	}

    
    //zbq[12/18/2007]逻辑通道打开的判断依赖呼叫带宽限制
    if ( wMtMaxRecvBitrate < (wCallRate-wAudioBand) )
    {
        bOpenVideoChnnl = FALSE;
        ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "Open Mt.%d video logic chnnl failed due to ChnnlRcvBR.%d, DailBR.%d\n",
            tMt.GetMtId(), wMtMaxRecvBitrate, (wCallRate-wAudioBand) );
    }
	else
	{
		wMtMaxRecvBitrate = (wCallRate-wAudioBand);
	}

    /*
    if ( 0 == m_tConf.GetSecBitRate() )// 单速会议
    {
        if ( wMtMaxRecvBitrate < m_tConf.GetBitRate() ) 
        {
            bOpenVideoChnnl = FALSE;
        }
    }
    else //双速会议
    {
        if( wMtMaxRecvBitrate < m_tConf.GetSecBitRate() )
        {
            bOpenVideoChnnl = FALSE;
        } 
    }
    */
    /*
    //zbq[12/30/2008] 查看本会议是否支持该接收群组
	u8 byChnRes = 0;
    BOOL32 bAccord2MainCap = FALSE;
    if (!IsMtMatchedSrc(tMt.GetMtId(), byChnRes, bAccord2MainCap))
    {
        bOpenVideoChnnl = FALSE;
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "Open Mt.%d video logic chnnl failed due to src<%d-%s> not matched!\n", 
            tMt.GetMtId(), tSimCap.GetVideoMediaType(), GetResStr(tSimCap.GetVideoResolution()));
    }*/

	//发送打开终端视频逻辑通道信息
	if( bFECOpenVideoChnnl &&
        bOpenVideoChnnl &&
        tVideoLogicChnnl.GetChannelType() != MEDIA_TYPE_NULL && 
		m_tConf.GetBitRate() >= 64 )
	{
		tVideoLogicChnnl.SetFlowControl( wMtMaxRecvBitrate );
		tVideoLogicChnnl.m_tSndMediaCtrlChannel.SetIpAddr( dwRcvIp );
		tVideoLogicChnnl.m_tSndMediaCtrlChannel.SetPort( wRcvPort + 1 );
		tVideoLogicChnnl.SetMediaEncrypt(m_tConf.GetMediaKey());
		tVideoLogicChnnl.SetActivePayload(GetActivePayload(m_tConf, tVideoLogicChnnl.GetChannelType()));
        
		//直接取分辨率填入逻辑通道[12/9/2011 chendaiwei]
		u8 byChnRes = 0;
		if (!GetMtMatchedRes(tMt.GetMtId(), tVideoLogicChnnl.GetChannelType(), byChnRes))
		{
			//zbq[04/22/2009] 主能力即使打开也不能适配的情况下，重新按辅能力构造打开
            if (MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType())
            {
                tVideoLogicChnnl.SetChannelType(m_tConf.GetSecVideoMediaType());
                tVideoLogicChnnl.SetActivePayload(GetActivePayload(m_tConf, tVideoLogicChnnl.GetChannelType()));
                byChnRes = m_tConf.GetSecVideoFormat();

                ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtCapSet] no proper res for mt(%d) with mediatype(%d), try SecVidCap!\n", 
                    tMt.GetMtId(), tVideoLogicChnnl.GetChannelType());
            }
            else
            {
                bOpenVideoChnnl = FALSE;
                ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtCapSet] no proper res for mt(%d) with mediatype(%d)!\n", 
                    tMt.GetMtId(), tVideoLogicChnnl.GetChannelType());
            }
		}
		

		BOOL32 bAdjustCapEx = FALSE;
		TVideoStreamCap tSrcCapEx;
		TVideoStreamCap tAdjCapEx;

		//如果支持根据终端呼叫码率调整分辨率(配置开关)
		if(bOpenVideoChnnl  
			&&  0 != m_tConf.GetSecBitRate() 
			&&  g_cMcuVcApp.IsSupportChgLGCResAcdBR()) 
		{
			//DialBR在存储时省去音频不准：尊重既有策略，临时补充如下
            u16 wDailBR = m_ptMtTable->GetDialBitrate(tMt.GetMtId()) + 
                          GetAudioBitrate(m_tConf.GetMainAudioMediaType());

			if(tVideoLogicChnnl.GetChannelType() == MEDIA_TYPE_H264
				&& m_tConf.GetMainVideoMediaType() == MEDIA_TYPE_H264)
			{
				tSrcCapEx = tCapSupport.GetMainSimCapSet().GetVideoCap();
				tSrcCapEx.SetResolution(byChnRes);

				if(GetMtLegalCapExAccord2CallBR(wDailBR,tSrcCapEx,tAdjCapEx))
				{
					bAdjustCapEx = TRUE;
					byChnRes = tAdjCapEx.GetResolution();
				}
			}
			
			if(bAdjustCapEx)
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMtCapSet] Mt.%d's Fwd chn Media(%d)Res adjust from <%s,%d> to <%s,%d> due to CallBR.%d!\n",
					tMt.GetMtId(),
					tVideoLogicChnnl.GetChannelType(),
					GetResStr(tSrcCapEx.GetResolution()),
					tSrcCapEx.GetUserDefFrameRate(),
					GetResStr(tAdjCapEx.GetResolution()),
					tAdjCapEx.GetUserDefFrameRate());
			}
			else
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtCapSet]  not adjust Mt.%d's Fwd chn <Media(%d),Res(%d)> due to CallBR.%d!\n",
								tMt.GetMtId(), tVideoLogicChnnl.GetChannelType(), byChnRes, wDailBR);
			}
		}

		
		if ( bOpenVideoChnnl )
		{
			if(g_cMcuVcApp.IsSendFakeCap2Polycom() &&
			   MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
			   VIDEO_FORMAT_4CIF == tCapSupport.GetMainVideoResolution() &&
			   VIDEO_FORMAT_4CIF != m_tConf.GetMainVideoFormat() &&
			   IsHDConf(m_tConf))
			{
				byChnRes = VIDEO_FORMAT_HD720;
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtCapSet] Mt.%d 's video open res has been adjusted from 4CIF to 720p!\n");
			}


            //[4/20/2011 zhushengze]IMAX帧率取共同能力集中帧率
			// [pengjie 2010/6/2] 打开对端逻辑通道时设置帧率
			//                    对于是1080/720 50/60帧的会议，如果要打开对端4cif/cif的通道，将帧率降成30帧，
			//                    也可在mtadp侧限制，但鉴于下层对mcu业务的隔离,保证mtadp侧不感知,所以还是在mcu侧改动
			// 1、取帧率
			u8 byFps = 0;   
			u8 byChannelType = tVideoLogicChnnl.GetChannelType();

			if( byChannelType == m_tConf.GetSecVideoMediaType() )
			{
				if( MEDIA_TYPE_H264 == byChannelType )
				{
					// byFps = m_tConf.GetSecVidUsrDefFPS();
                    byFps = tCapSupport.GetSecVidUsrDefFPS();
				}
				else
				{
					// byFps = m_tConf.GetSecVidFrameRate();
                    byFps = tCapSupport.GetSecVidFrmRate();
				}
			}
			else
			{
				if( MEDIA_TYPE_H264 == byChannelType )
				{
					// byFps = m_tConf.GetMainVidUsrDefFPS();
                    byFps = tCapSupport.GetMainVidUsrDefFPS();
				}
				else
				{
					// byFps = m_tConf.GetMainVidFrameRate();
                    byFps = tCapSupport.GetMainVidFrmRate();
				}
			}

			// 2、设置帧率, 并进行限制
			if( byFps > 30 )
			{
				if( VIDEO_FORMAT_4CIF == byChnRes || VIDEO_FORMAT_CIF == byChnRes || VIDEO_FORMAT_MPEG4 == byChnRes)
				{
					tVideoLogicChnnl.SetChanVidFPS( 25 ); 
				}
				else
				{
					tVideoLogicChnnl.SetChanVidFPS( byFps );
				}
			}
			else
			{
				tVideoLogicChnnl.SetChanVidFPS( byFps );
			}
			
			//3.针对呼叫码率可能调整过帧率分辨率
			if(bAdjustCapEx)
			{
				byFps = tAdjCapEx.GetUserDefFrameRate();
				byChnRes = tAdjCapEx.GetResolution();
				tVideoLogicChnnl.SetChanVidFPS( byFps );
			}

			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcMtCapSet] Open Mt%d Video LogicChannel,Set VidFPS: %d!\n",
				tMt.GetMtId(), tVideoLogicChnnl.GetChanVidFPS() );
			// End

			tVideoLogicChnnl.SetVideoFormat(byChnRes);

			
			//设置HP/BP属性
			if( VIDEO_FORMAT_4CIF == byChnRes || 
				VIDEO_FORMAT_CIF == byChnRes )
			{
				tVideoLogicChnnl.SetProfieAttrb( emBpAttrb );
			}
			else
			{
				tVideoLogicChnnl.SetProfieAttrb(tCapSupport.GetMainStreamProfileAttrb());
			}
			
			cServMsg.SetMsgBody( ( u8 * )&tVideoLogicChnnl, sizeof( tVideoLogicChnnl ) );
			cServMsg.CatMsgBody( &byChnRes, sizeof(u8) );
			SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );
			
			//打印消息
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcMtCapSet] Open Video LogicChannel Request send to Mt%d, type: %s, res.%s, Fps:%d, HP:%d, BR:%d\n", 
				tMt.GetMtId(), GetMediaStr( tVideoLogicChnnl.GetChannelType() ), GetResStr(tVideoLogicChnnl.GetVideoFormat()), tVideoLogicChnnl.GetChanVidFPS(),tVideoLogicChnnl.GetProfileAttrb(),tVideoLogicChnnl.GetFlowControl() );
		}
		else
		{
			VCSMTAbilityNotif(tMt, MODE_VIDEO);			
		}
	}
	else
	{
		VCSMTAbilityNotif(tMt, MODE_VIDEO);		

		//打印消息
		ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL,  "[ProcMtCapSet] Mt%d not support any conf video media type, NO open video logicChannel request send to it!\n", tMt.GetMtId() );
	}


	//打开终端的双流通道 
// 	if( bFECOpenVideoChnnl &&
//         bOpenVideoChnnl &&
//         //m_ptMtTable->GetMtLogicChnnl( m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE ) &&
// 		!m_tDoubleStreamSrc.IsNull() &&
// 		m_tConf.GetBitRate() >= 64 )
// 	{	
// 		// xliang [11/14/2008] polycomMCU的双流通道已经打开，此处不必重复
// 		if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
// 			( MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
// 			MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ) )
// 		{
// 			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "PolyMcu.%d's secvideo chan has been open somewhere else, ignore it\n");
// 		}
// 		if (MT_TYPE_MMCU == tMt.GetMtType())
// 		{
// 			if( m_tDoubleStreamSrc.GetMtId() != tMt.GetMtId() )
// 			{
// 				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "Mt%d is mmcu, settimer open double video chan. \n", tMt.GetMtId());
// 				SetTimer(MCU_SMCUOPENDVIDEOCHNNL_TIMER, 3000, tMt.GetMtId());
// 			}	
// 		}
// 		else
// 		{
//             BOOL32 bDStreamSrcLegal = FALSE;
// 
//             //zbq[01/11/2008] 双流放象需要伪装放象通道
//             if ( TYPE_MCUPERI == m_tDoubleStreamSrc.GetType() &&
//                  EQP_TYPE_RECORDER == m_tDoubleStreamSrc.GetEqpType() )
//             {
//                 tLogicalChannel.SetChannelType( m_tPlayEqpAttrib.GetDVideoType() );
//                 tLogicalChannel.SetSupportH239( m_tConf.GetCapSupport().IsDStreamSupportH239() );
// 				// fxh 需要双流源的分辨率信息,已用于后续开通道时判断是否目的端与之匹配
// 				u16 wFileDSW = 0; 
// 				u16 wFileDSH = 0;
// 				u8  byFileDSType = 0;
// 				m_tPlayFileMediaInfo.GetDVideo(byFileDSType, wFileDSW, wFileDSH);
// 		     	u8 byFileDSRes = GetResByWH(wFileDSW, wFileDSH);
// 				tLogicalChannel.SetVideoFormat(byFileDSRes);
//                 bDStreamSrcLegal = TRUE;
//             }
//             else
//             {
//                 if ( m_ptMtTable->GetMtLogicChnnl( m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE ))
//                 {
//                     bDStreamSrcLegal = TRUE;
//                 }
//                 else
//                 {
//                     ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcMtCapSet] Get DS Src.%d's logic chnnl failed\n", m_tDoubleStreamSrc.GetMtId());
//                 }
//             }
// 
//             if ( bDStreamSrcLegal )
//             {
//                 McuMtOpenDoubleStreamChnnl(tMt, tLogicalChannel/*, tCapSupport*/);
//             }
// 		}
// 	}
//     else
//     {
//         ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mt%d not support any conf DVideo media type, NO open DVideo logicChannel request send to it!\n", tMt.GetMtId() );
//     }

	//根据终端的能力集，打开终端的H.224数据通道
	if( tCapSupport.IsSupportH224())
	{
		tDataLogicChnnl.SetMediaType( MODE_DATA );
		tDataLogicChnnl.SetChannelType( MEDIA_TYPE_H224 );
		tDataLogicChnnl.m_tSndMediaCtrlChannel.SetIpAddr( dwRcvIp );
		tDataLogicChnnl.m_tSndMediaCtrlChannel.SetPort( wRcvPort + 7 );
		tDataLogicChnnl.SetMediaEncrypt(m_tConf.GetMediaKey());
		tDataLogicChnnl.SetActivePayload(GetActivePayload(m_tConf, tDataLogicChnnl.GetChannelType()));
		
		cServMsg.SetMsgBody( ( u8 * )&tDataLogicChnnl, sizeof( tDataLogicChnnl ) );			
		SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );
		
		//打印消息
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Open Data LogicChannel Request send to Mt%d, type: %s\n", 
			tMt.GetMtId(), GetMediaStr( tDataLogicChnnl.GetChannelType() ) );			
	}

	// 级联数据通道
    // guzh [1/19/2007] 增加判断是否级联会议数量超限,该会议下级MCU是否超限
	u8 byMtType = m_ptMtTable->GetMtType(tMt.GetMtId());
	if ( byMtType == MT_TYPE_SMCU &&
	 	 tCapSupport.IsSupportMMcu() &&
		 m_tConf.GetConfAttrb().IsSupportCascade() &&
         !g_cMcuVcApp.IsCasConfOverCap() &&         
		 m_tConfAllMcuInfo.GetSMcuNum() < MAXNUM_SUB_MCU )
	{
		//先addmcu将下级mcu加到，mcu信息表中，模板创会呼叫多个下级的时候就能知道呼叫上多少个下级mcu了
		u16 wMcuIdx = INVALID_MCUIDX;
		u8 abyMcuId[MAX_CASCADEDEPTH-1];
		memset( &abyMcuId[0],0,sizeof(abyMcuId) );
		abyMcuId[0] = tMt.GetMtId();
		m_tConfAllMcuInfo.AddMcu( &abyMcuId[0],2,&wMcuIdx );
		m_ptConfOtherMcTable->AddMcInfo( wMcuIdx );	
		m_tConfAllMtInfo.AddMcuInfo( wMcuIdx, tMt.GetConfIdx() );

		tDataLogicChnnl.SetMediaType( MODE_DATA );
		tDataLogicChnnl.SetChannelType( MEDIA_TYPE_MMCU );

		//打开级联通道时，相应tcp建链的ip及port设置为接入板级联地址
		TMt tDstMt = m_ptMtTable->GetMt(tMt.GetMtId());
		u32 dwCasIp = g_cMcuVcApp.GetMtAdpIpAddr(tDstMt.GetDriId()); //主机序
		tDataLogicChnnl.m_tRcvMediaChannel.SetIpAddr(dwCasIp);

        // 顾振华 [6/15/2006] 为了测试。上级呼叫下级MCU时允许配置级联端口        
		tDataLogicChnnl.m_tRcvMediaChannel.SetPort( g_cMcuVcApp.GetSMcuCasPort() );
		
		cServMsg.SetMsgBody( ( u8 * )&tDataLogicChnnl, sizeof( tDataLogicChnnl ) );			
		SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );
        
		// [6/14/2011 xliang] no need to set timer
// 		u16 wTimerSpan = g_cMcuVcApp.GetMmcuOlcTimerOut();	//对应配置项"MmcuOLCTimerOut"
// 		if( 0 != wTimerSpan )
// 		{
// 			m_abyCasChnCheckTimes[tMt.GetMtId()-1] = 1;
// 			SetTimer( MCUVC_WAIT_CASCADE_CHANNEL_TIMER, wTimerSpan );
// 			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "SetTimer MCUVC_WAIT_CASCADE_CHANNEL_TIMER: %d (ms)\n", wTimerSpan );
// 		}

		//打印消息
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Open Data LogicChannel Request send to Mt%d, type: %s\n", 
				tMt.GetMtId(), GetMediaStr( tDataLogicChnnl.GetChannelType() ) );			
	}
    else
    {
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "Not send MMcu OLC to Mt%d for MtType.%d, RemoteCap.%d, ConfAtrb.%d, CasConfNum.%d, CasMcuNum.%d \n", 
                 tMt.GetMtId(),
                 byMtType,
                 tCapSupport.IsSupportMMcu(),
                 m_tConf.GetConfAttrb().IsSupportCascade(),
                 g_cMcuVcApp.GetMcuCasConfNum(),
                 m_tConfAllMcuInfo.GetSMcuNum() );
    }

	//根据对端能力集，打开对端的T.120数据通道
	if( tCapSupport.IsSupportT120() && 
		tConfCapSupport.IsSupportT120() && 
		( m_tConf.GetConfAttrb().GetDataMode() == CONF_DATAMODE_VAANDDATA ||
		  m_tConf.GetConfAttrb().GetDataMode() == CONF_DATAMODE_DATAONLY ) )
	{
		//如果本端主呼, 不带地址打开对端通道
		if ( !m_ptMtTable->IsNotInvited( tMt.GetMtId() ) )
		{
			tDataLogicChnnl.SetMediaType( MODE_DATA );
			tDataLogicChnnl.SetChannelType( MEDIA_TYPE_T120 );

			tDataLogicChnnl.m_tRcvMediaChannel.SetIpAddr(0);
			tDataLogicChnnl.m_tRcvMediaChannel.SetPort(0);
			
			cServMsg.SetMsgBody( ( u8 * )&tDataLogicChnnl, sizeof( tDataLogicChnnl ) );				
			SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );

			//打印消息
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Open Data LogicChannel Request send to Mt%d, type: %s as Master \n", 
					  tMt.GetMtId(), GetMediaStr( tDataLogicChnnl.GetChannelType() ) );
		}

		// 如果本端被呼, 带地址打开. 对KEDA MCU, 将被拒.
		else
		{	
			tDataLogicChnnl.SetMediaType( MODE_DATA );
			tDataLogicChnnl.SetChannelType( MEDIA_TYPE_T120 );
		
			//取本会议的DCS地址和端口
			u32 dwDcsIp  = g_cMcuVcApp.m_atPeriDcsTable[m_byDcsIdx-1].m_dwDcsIp;
			u16 wDcsPort = g_cMcuVcApp.m_atPeriDcsTable[m_byDcsIdx-1].m_wDcsPort;
			
			//地址有效
			if ( 0 != dwDcsIp && 0 != wDcsPort ) 
			{
				tDataLogicChnnl.m_tRcvMediaChannel.SetIpAddr(dwDcsIp);
				tDataLogicChnnl.m_tRcvMediaChannel.SetPort(wDcsPort);
				
				cServMsg.SetMsgBody( ( u8 * )&tDataLogicChnnl, sizeof( tDataLogicChnnl ) );			
				SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );
				
				//如果本端做主, 等待下级终端打开本端通道
				if ( !m_ptMtTable->IsMtIsMaster( tMt.GetMtId() ) )
				{
					//打印消息
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Open Data LogicChannel Request send to Mt%d, type: %s as master with no address ! \n", 
						      tMt.GetMtId(), GetMediaStr( tDataLogicChnnl.GetChannelType() ) );
				}
				//本端做从, 等待上级MCU打开本端通道
				else
				{
					//打印消息
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Open Data LogicChannel Request send to Mt%d, type: %s as slave with no address ! \n", 
						      tMt.GetMtId(), GetMediaStr( tDataLogicChnnl.GetChannelType() ) );
				}				
			}
			else
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "Get DCS Address 0x%x:%d failed in Open T120 channel !\n", dwDcsIp, wDcsPort );
			}	
		}
	}

    return;
}

/*====================================================================
    函数名      ：ProcTimerReopenCascadeChannel
    功能        ：尝试再次打开级联通道
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	07/01/11	4.0			张宝卿		  创建
====================================================================*/
// void CMcuVcInst::ProcTimerReopenCascadeChannel()
// {
//     BOOL32 bNonOpenStill = FALSE;
//     BOOL32 bFailedThreeTimes = FALSE;
//     TLogicalChannel tDataLogicChnnl;
//     for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++ )
//     {
//         if ( m_abyCasChnCheckTimes[byMtId-1] == 0 )
//         {
//             continue;
//         }
//         //最多尝试六次 (3 * 10 + 3 * 60) s
//         if ( m_abyCasChnCheckTimes[byMtId-1] <= 2*MAXNUM_CHECK_CAS_TIMES )
//         {
//             m_abyCasChnCheckTimes[byMtId-1] ++;
// 
//             //尝试再次打开级联数据通道
//             u8 byMtType = m_ptMtTable->GetMtType(byMtId);
//             // guzh [1/19/2007] 增加判断是否级联会议数量超限,该会议下级MCU是否超限
// 	        if ( byMtType == MT_TYPE_SMCU &&
//                 !g_cMcuVcApp.IsCasConfOverCap() &&                
// 		        m_tConfAllMcuInfo.GetSMcuNum() < MAXNUM_SUB_MCU )
//             {
//                 tDataLogicChnnl.SetMediaType( MODE_DATA );
//                 tDataLogicChnnl.SetChannelType( MEDIA_TYPE_MMCU );
//     
//                 //打开级联通道时，相应tcp建链的ip及port设置为接入板级联地址
//                 TMt tDstMt = m_ptMtTable->GetMt(byMtId);
//                 u32 dwCasIp = g_cMcuVcApp.GetMtAdpIpAddr(tDstMt.GetDriId()); //主机序
//                 tDataLogicChnnl.m_tRcvMediaChannel.SetIpAddr(dwCasIp);
//     
//                 tDataLogicChnnl.m_tRcvMediaChannel.SetPort( g_cMcuVcApp.GetSMcuCasPort() );
//                 
//                 CServMsg cServMsg;
//                 cServMsg.SetMsgBody( ( u8 * )&tDataLogicChnnl, sizeof( tDataLogicChnnl ) );			
//                 SendMsgToMt( byMtId, MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );
//                 
//                 if ( m_abyCasChnCheckTimes[byMtId-1] > MAXNUM_CHECK_CAS_TIMES )
//                 {
//                     bFailedThreeTimes = TRUE;
//                 }
//                 bNonOpenStill = TRUE;
//                 ConfLog( FALSE, "[ProcTimerReopenCascadeChannel] Open Mt.%d Cascade Channel failed %d time(s), retry now !\n", 
//                                  byMtId, m_abyCasChnCheckTimes[byMtId-1] );
//             }
//         }
//         else
//         {
//             m_abyCasChnCheckTimes[byMtId-1]  = 0;
//             ConfLog( FALSE, "[ProcTimerReopenCascadeChannel] Open Mt.%d Cascade Channel failed entirely !\n", byMtId );
//         }
//     }
// 
//     if ( bNonOpenStill )
//     {
//         if ( bFailedThreeTimes )
//         {
//             SetTimer( MCUVC_WAIT_CASCADE_CHANNEL_TIMER, TIMESPACE_WAIT_CASCADE_CHECK * 6 );
//         }
//         else
//         {
//             SetTimer( MCUVC_WAIT_CASCADE_CHANNEL_TIMER, TIMESPACE_WAIT_CASCADE_CHECK );
//         }
//     }
//     else
//     {
//         KillTimer( MCUVC_WAIT_CASCADE_CHANNEL_TIMER );
//     }
// 	return;
// }

/*====================================================================
    函数名      ：ProcMtMcuCapSetNotif
    功能        ：终端发给MCU的能力集通知函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/04/25    1.0         杨皞昀         创建
	04/05/17    3.0         胡昌威         修改
	06/01/05	4.0			张宝卿		  T120集成
====================================================================*/
void CMcuVcInst::ProcMtMcuCapSetNotif( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
    TMultiCapSupport tMultiCapSupport = *(TMultiCapSupport *)( cServMsg.GetMsgBody() );

    TCapSupportEx tCapSupportEx = *(TCapSupportEx*)(cServMsg.GetMsgBody()+sizeof(TCapSupport));
	u8   bMtHasVidCap = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMultiCapSupport) + sizeof(tCapSupportEx));

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, 
				"[ProcMtMcuCapSetNotif] MT.%d has vidcap(%d)!", tMt.GetMtId(), bMtHasVidCap);

// 	if( VCS_CONF == m_tConf.GetConfSource() && m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE )
// 	{
// 		if( tMultiCapSupport.GetMainVideoType() == MEDIA_TYPE_NULL)
// 		{
// 			TCapExtraSet tCapExtraSet;
// 			tMultiCapSupport.GetCapExtraSet(tCapExtraSet,0);
// 			if( tCapExtraSet.GetVideoStreamCap().GetMediaType() != MEDIA_TYPE_NULL )
// 			{
// 				TSimCapSet tMainSimCapSet;
// 				memcpy(&tMainSimCapSet,&tCapExtraSet,sizeof(TSimCapSet));
// 				tMultiCapSupport.SetMainSimCapSet(tMainSimCapSet);
// 			}
// 		}
// 	}

	u32 dwBitRate = 0;

	switch( CurState() )
	{
	case STATE_ONGOING:
		{
			//设置该终端的能力集
			u8 byMtId = tMt.GetMtId();
			// m_ptMtTable->SetMtCapSupport(byMtId, &tCapSupport);
            m_ptMtTable->SetMtMultiCapSupport(byMtId, &tMultiCapSupport);
			m_ptMtTable->SetMtCapSupportEx(byMtId, &tCapSupportEx);
            if (g_bPrintMtMsg)
            {
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcMtMcuCapSetNotif]capability specified by mt:\n");
                tMultiCapSupport.Print();
            }
			if (m_ptMtTable->IsMtCapSpecByMCS(byMtId))
			{
				TMultiCapSupport tCap;
				TVideoStreamCap tMainVideoCap;
				TVideoStreamCap tDStreamVidoCap; 

				m_ptMtTable->GetMtCapSupport(byMtId, &tCap);	
				tMainVideoCap   = m_ptMtTable->m_tMTInfoEx[byMtId].GetMainVideoCap();
				tDStreamVidoCap = m_ptMtTable->m_tMTInfoEx[byMtId].GetDStreamVideoCap();

				// 保存MCS指定的终端主视频及双流能力级
				tCap.SetMainVideoType(tMainVideoCap.GetMediaType());
				tCap.SetMainVideoResolution(tMainVideoCap.GetResolution());
				if (tMainVideoCap.IsFrameRateUserDefined())
				{
					tCap.SetMainVidUsrDefFPS(tMainVideoCap.GetUserDefFrameRate());
				}
				else
				{
					tCap.SetMainVidFrmRate(tMainVideoCap.GetFrameRate());
				}

				tCap.SetDStreamMediaType(tDStreamVidoCap.GetMediaType());
				tCap.SetDStreamResolution(tDStreamVidoCap.GetResolution());
				if (tDStreamVidoCap.IsFrameRateUserDefined())
				{
					tCap.SetDStreamUsrDefFPS(tDStreamVidoCap.GetUserDefFrameRate());
				}
				else
				{
					tCap.SetDStreamFrameRate(tDStreamVidoCap.GetFrameRate());
				}

				//m_ptMtTable->SetMtCapSupport(byMtId, &tCap);
                m_ptMtTable->SetMtMultiCapSupport(byMtId, &tCap);
				
				m_ptMtTable->GetMtCapSupport(byMtId, &tMultiCapSupport);			
			}

            if (g_bPrintMtMsg)
            {
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcMtMcuCapSetNotif]final mt capability:\n");
                tMultiCapSupport.Print();
            }
			
			//确定该终端已经入会
			if( m_tConfAllMtInfo.MtJoinedConf( byMtId ) )
			{
				//处理终端能力集
				ProcMtCapSet( tMt, tMultiCapSupport, bMtHasVidCap);

				//本端主呼
				if ( !m_ptMtTable->IsNotInvited( byMtId ) )
				{
					//需要拿到地址和端口
				}
				//本端被呼
				else
				{
					//终端和会议均支持数据功能
					if ( tMultiCapSupport.IsSupportT120() && 
						 m_tConf.GetCapSupport().IsSupportT120() && 
						 m_tConf.GetConfAttrb().GetDataMode() != CONF_DATAMODE_VAONLY )
					{
						//告诉DCS增加该终端
						SendMcuDcsAddMtReq( byMtId );
					}	
				}
			}
			break;
		}


	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "CMcuVcInst: ProcMtMcuCapSetNotif() Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

/*====================================================================
    函数名      ：ProcMtMcuMtAliasNotif
    功能        ：终端发送给MCU的别名通知
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/31    3.0         
====================================================================*/
void CMcuVcInst::ProcMtMcuMtAliasNotif( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	
	u8 byMtId = cServMsg.GetSrcMtId();
	TMtAlias tMtAlias = *( TMtAlias* )cServMsg.GetMsgBody();
	TMt tMt;
	TMcu  tMcu;

	TMt			atMt[MAXNUM_CONF_MT];
	TMtAlias	atMtAlias[MAXNUM_CONF_MT];
	u8			abyBuff[MAXNUM_CONF_MT*( sizeof(TMt) + sizeof(TMtAlias) )];
	u8			byMtNum = 0;
	u8			byLoop;
	u32			dwBuffSize;

	switch( CurState()) 
	{
	case STATE_ONGOING:
		
		m_ptMtTable->SetMtAlias( byMtId, &tMtAlias );

		//给会控发送别名通知消息 
		tMt = m_ptMtTable->GetMt( byMtId );
		cServMsg.SetMsgBody( (u8*)&tMt, sizeof( TMt ) );
		cServMsg.CatMsgBody( (u8*)&tMtAlias, sizeof(tMtAlias ) );
		SendMsgToAllMcs( MCU_MCS_MTALIAS_NOTIF, cServMsg ); 		

		for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
		{
			if( m_tConfAllMtInfo.MtInConf( byLoop ) )
			{
				atMt[byMtNum] = m_ptMtTable->GetMt( byLoop );
				atMtAlias[byMtNum].SetNull();
                if(!m_ptMtTable->GetMtAlias(byLoop, mtAliasTypeH320Alias, &atMtAlias[byMtNum]))
                {
                    if( !m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeH320ID, &atMtAlias[byMtNum] ) )
                    {
                        if(!m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeH323ID, &atMtAlias[byMtNum] ))
                        {
                            if(!m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeE164, &atMtAlias[byMtNum] ))
                            {
                                m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeTransportAddress, &atMtAlias[byMtNum] );
                            }
                        }
                    }
                }				
				byMtNum++;
			}			
		}
		
		tMcu.SetMcu( LOCAL_MCUID );
		cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(TMcu) );
		dwBuffSize = PackMtInfoListIntoBuf( atMt, atMtAlias, byMtNum, abyBuff, sizeof( abyBuff ) );
		cServMsg.CatMsgBody( abyBuff, (u16)dwBuffSize );
		SendMsgToAllMcs( MCU_MCS_GETALLMTALIAS_ACK, cServMsg );

        byMtNum = 0;
		memset( atMtAlias, 0, sizeof(atMtAlias) );
		memset( abyBuff, 0, sizeof(abyBuff) );
		for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
		{
			if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
			{
				atMt[byMtNum] = m_ptMtTable->GetMt( byLoop );
				atMtAlias[byMtNum].SetNull();
                if(!m_ptMtTable->GetMtAlias(byLoop, mtAliasTypeH320Alias, &atMtAlias[byMtNum]))
                {
                    if(!m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeH320ID, &atMtAlias[byMtNum] ))
                    {
                        if(!m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeH323ID, &atMtAlias[byMtNum] ))
                        {
                            if(!m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeE164, &atMtAlias[byMtNum] ))
                            {
                                m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeTransportAddress, &atMtAlias[byMtNum] );
                            }
                        }
                    }
                }				
				byMtNum++;
			}			
		}

		dwBuffSize = PackMtInfoListIntoBuf( atMt, atMtAlias, byMtNum, abyBuff, sizeof( abyBuff ) );
		cServMsg.SetMsgBody( abyBuff, (u16)dwBuffSize );

		cServMsg.SetConfIdx( m_byConfIdx );
		cServMsg.SetConfId( m_tConf.GetConfId() );
		cServMsg.SetDstMtId( 0 );
		g_cMtAdpSsnApp.BroadcastToAllMtAdpSsn( MCU_MT_JOINEDMTLISTID_ACK, cServMsg );
					
		break;
		
	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: ProcMtMcuMtAliasNotif() Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );		
	}
}
/*====================================================================
    功能        ：是否可以向某个类型的终端欺骗终端会议能力
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtBoardType, 终端类型
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2010/02/05				周晶晶
====================================================================*/
BOOL32 CMcuVcInst::IsMtCanCapabilityCheat( s8 *pachProductID )
{
	if( NULL == pachProductID )
	{
		return FALSE;
	}
	//MT_BOARD_8010
	return g_cMcuVcApp.IsMtBoardTypeCanCapabilityCheat( pachProductID );
}

/*====================================================================
    功能        ：终端登记成功处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/05    1.0         LI Yi         创建
    04/02/14    3.0         胡昌威        修改
	11/08/04    4.6         彭国锋        Bug00059851修改
====================================================================*/
void CMcuVcInst::ProcMtMcuConnectedNotif( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	
    u8 byManuId = *(u8*)cServMsg.GetMsgBody();
    s8 achProductId[MAXLEN_PRODUCTID];
    s8 achVersionId[MAXLEN_VERSIONID];

    //增量获取对端的 VersionId 和 ProductId
    if (cServMsg.GetMsgBodyLen() > sizeof(u8))
    {
        u8 *pbyMsgBody = cServMsg.GetMsgBody() + sizeof(byManuId);

        //versionId
        u8 byVersionIdLen = *pbyMsgBody;
        pbyMsgBody += sizeof(byVersionIdLen);
        
        memset(achVersionId, 0, sizeof(achVersionId));
        memcpy(achVersionId, pbyMsgBody, min(byVersionIdLen, MAXLEN_VERSIONID));
        
        m_ptMtTable->SetVersionId(tMt.GetMtId(), achVersionId);

        pbyMsgBody += byVersionIdLen;

        //productId
        u8 byProductIdLen = *pbyMsgBody;
        pbyMsgBody += sizeof(byProductIdLen);

        memset(achProductId, 0, sizeof(achProductId));
        memcpy(achProductId, pbyMsgBody, min(byProductIdLen, MAXLEN_PRODUCTID));

        m_ptMtTable->SetProductId(tMt.GetMtId(), achProductId);
    }
	
	TCapSupport tCapSupport;
	TCapSupportEx tCapSupportEx;
	TSimCapSet tSimCapSet;

	//设置厂商信息
	m_ptMtTable->SetManuId( tMt.GetMtId(), byManuId );
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mt%d: 0x%x(Dri:%d) connected with manu.%s, verId.%s, productId.%s!\n", 
		     tMt.GetMtId(), 
             m_ptMtTable->GetIPAddr(tMt.GetMtId()),
             tMt.GetDriId(),
             GetManufactureStr( byManuId ),
             achVersionId,
             achProductId);


    // xsl [10/27/2006] 分散会议不支持级联
    if (m_tConf.GetConfAttrb().IsSatDCastMode() &&
        MT_TYPE_MT != m_ptMtTable->GetMtType(tMt.GetMtId()))
    {
        SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);
        ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtMcuConnectedNotif] sat conf don't support cascade.\n");  
        return;
    }

	tCapSupport = m_tConf.GetCapSupport();
	tCapSupportEx = m_tConf.GetCapSupportEx();

    TSimCapSet tSim = tCapSupport.GetMainSimCapSet();
    //  xsl [4/26/2006] 填会议码率，代表mcu收发能力
//  tSim.SetVideoMaxBitRate(m_ptMtTable->GetDialBitrate( tMt.GetMtId()));
    if (tSim.GetVideoMaxBitRate() == 0)
    {
        tSim.SetVideoMaxBitRate(m_tConf.GetBitRate());
        tCapSupport.SetMainSimCapSet(tSim);
    }
    tSim = tCapSupport.GetSecondSimCapSet();
    if(!tSim.IsNull() && tSim.GetVideoMaxBitRate() == 0)
    {
//        tSim.SetVideoMaxBitRate(m_ptMtTable->GetDialBitrate( tMt.GetMtId()));
        tSim.SetVideoMaxBitRate(m_tConf.GetBitRate());
        tCapSupport.SetSecondSimCapSet(tSim);
    }	
//    tCapSupport.SetDStreamMaxBitRate(m_ptMtTable->GetDialBitrate( tMt.GetMtId()));
    tCapSupport.SetDStreamMaxBitRate(m_tConf.GetBitRate());

#ifdef _MINIMCU_
    if ( ISTRUE(m_byIsDoubleMediaConf) )
    {
        // 高码率采用主格式，低码率采用辅格式（避免纯粹的速率适配）
        // 这里暂时不考虑音频的双格式，因为双音频格式的会议应该是无法同双媒体同时开启的（BAS通道不够）
        u16 wMainBitrate = m_tConf.GetBitRate();
        u16 wSecBitrate = m_tConf.GetSecBitRate();
        u16 wDialBitrate = m_ptMtTable->GetDialBitrate(tMt.GetMtId());
        if ( wDialBitrate == wMainBitrate ) 
        {
            tSim.Clear();
            tCapSupport.SetSecondSimCapSet(tSim);
        }
        else if ( /*wSecBitrate == wDialBitrate*/wDialBitrate < wMainBitrate && wDialBitrate >= wSecBitrate)
        {
            tSim = tCapSupport.GetSecondSimCapSet();
			// [12/31/2009 xliang] 若辅格式无音频，此处进行保护
			if (tCapSupport.GetSecAudioType() == MEDIA_TYPE_NULL)
			{
				tSim.SetAudioMediaType(tCapSupport.GetMainAudioType());
			}
            tCapSupport.SetMainSimCapSet(tSim);
            tSim.Clear();
            tCapSupport.SetSecondSimCapSet(tSim);
        }
        else
        {
			ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtMcuConnectedNotif]Call mt%d with uncorrect bitrate(%d confbitrate(%d, %d))\n",
				      tMt.GetMtId(), wDialBitrate, wSecBitrate, wMainBitrate);
            tSim.Clear();
            tCapSupport.SetSecondSimCapSet(tSim);
        }           
    }    
#else 
	//////////////////////////////////////////////////////////////////////////
	//zjj20100205武警会议能力欺骗，当双速双格式会议且主副格式未mp4或h264时就读取配置文件，
	//如果该终端是高速呼入而且该终端型号在配置文件中就发送mp4-16cif能力给终端，以打开终端后向通道为mp4-16cif
	//zjj20100208 如果是高清会议且是呼入的终端型号是再配置文件中配置的高清终端型号就不能力欺骗
	if( m_ptMtTable->GetMtType( tMt.GetMtId() ) == MT_TYPE_MT &&
			IsConfDbCapDbBitrateCapH264OrMp4() &&
			!( IsHDConf(m_tConf) && g_cMcuVcApp.IsHDMt( achProductId ) )
		)
	{		
		/*if( m_ptMtTable->GetDialBitrate( tMt.GetMtId() ) < m_tConf.GetBitRate() )
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[ProcMtMcuConnectedNotif] double speed and double mediaType,mt(%d) is low speed call,so send single mediaType\n",
				tMt.GetMtId()
				);

			TSimCapSet tSecCapset = tCapSupport.GetSecondSimCapSet();
			if( !tSecCapset.IsNull() )
			{
				if( MEDIA_TYPE_NULL == tSecCapset.GetAudioMediaType() )
				{
					tSecCapset.SetAudioMediaType( tCapSupport.GetMainAudioType() );
				}
				tCapSupport.SetMainSimCapSet( tSecCapset );
			}	
			TSimCapSet tNullSimCap;
			tNullSimCap.Clear();
			tCapSupport.SetSecondSimCapSet( tNullSimCap );
		}*/
		TSimCapSet tNullSimCap;
		TMtStatus tMtStatus;
		memset( &tMtStatus,0,sizeof( tMtStatus ) );
		if( m_ptMtTable->GetDialBitrate( tMt.GetMtId() ) == m_tConf.GetBitRate() &&
			IsMtCanCapabilityCheat( achProductId ) )
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[ProcMtMcuConnectedNotif] MT PrdouctID:%s is in cheat list.",achProductId );
			if( ( tCapSupport.GetMainSimCapSet().GetVideoMediaType() == MEDIA_TYPE_MP4 &&
				tCapSupport.GetMainSimCapSet().GetVideoResolution() == VIDEO_FORMAT_16CIF ) ||
				( tCapSupport.GetSecondSimCapSet().GetVideoMediaType() == MEDIA_TYPE_MP4 &&
					tCapSupport.GetSecondSimCapSet().GetVideoResolution() == VIDEO_FORMAT_16CIF  )
				)
			{	
				
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "so send mp4-16cif conf cap to mt. SecAudioType(%d)\n",
					tCapSupport.GetSecAudioType() 
					);

				if( tCapSupport.GetSecondSimCapSet().GetVideoMediaType() == MEDIA_TYPE_MP4  )
				{
					tNullSimCap = tCapSupport.GetSecondSimCapSet();
					if (tCapSupport.GetSecAudioType() == MEDIA_TYPE_NULL)
					{
						tNullSimCap.SetAudioMediaType(tCapSupport.GetMainAudioType());
					}
					tCapSupport.SetMainSimCapSet( tNullSimCap );
				}
				tCapSupport.SetMainVideoType( MEDIA_TYPE_MP4 );
				tCapSupport.SetMainVideoResolution( VIDEO_FORMAT_16CIF );

				
				tNullSimCap.Clear();
				tCapSupport.SetSecondSimCapSet( tNullSimCap );
			}			
		}	
		else
		{
			if( m_ptMtTable->GetDialBitrate( tMt.GetMtId() ) < m_tConf.GetBitRate() )
			{
				if( g_cMcuVcApp.IsLowCalledMtChlOpenMp4() )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[ProcMtMcuConnectedNotif] Low Called Mt Request Open Mp4 Forward Channel.\n" );
					if( tCapSupport.GetSecondSimCapSet().GetVideoMediaType() == MEDIA_TYPE_MP4  )
					{
						tNullSimCap = tCapSupport.GetSecondSimCapSet();
						if (tCapSupport.GetSecAudioType() == MEDIA_TYPE_NULL)
						{
							tNullSimCap.SetAudioMediaType(tCapSupport.GetMainAudioType());
						}
						tCapSupport.SetMainSimCapSet( tNullSimCap );
					}
					tCapSupport.SetMainVideoType( MEDIA_TYPE_MP4 );
					//tCapSupport.SetMainVideoResolution( VIDEO_FORMAT_16CIF );
					tNullSimCap.Clear();
					tCapSupport.SetSecondSimCapSet( tNullSimCap );
				}
				else if( g_cMcuVcApp.IsLowCalledMtChlOpenH264() )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[ProcMtMcuConnectedNotif] Low Called Mt Request Open H264 Forward Channel.\n" );
					if( tCapSupport.GetSecondSimCapSet().GetVideoMediaType() == MEDIA_TYPE_H264  )
					{
						tNullSimCap = tCapSupport.GetSecondSimCapSet();
						if (tCapSupport.GetSecAudioType() == MEDIA_TYPE_NULL)
						{
							tNullSimCap.SetAudioMediaType(tCapSupport.GetMainAudioType());
						}
						tCapSupport.SetMainSimCapSet( tNullSimCap );
					}
					tCapSupport.SetMainVideoType( MEDIA_TYPE_H264 );					
					tNullSimCap.Clear();
					tCapSupport.SetSecondSimCapSet( tNullSimCap );
				}
			}
		}
	}
#endif
	//////////////////////////////////////////////////////////////////////////    
    
    //zbq[02/04/2009] 高清会议面向Polycom进行480p的保护
    if (g_cMcuVcApp.IsSendFakeCap2Polycom() &&
        MT_MANU_POLYCOM == m_ptMtTable->GetManuId((tMt.GetMtId())) &&
        IsHDConf(m_tConf))
    {
        tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_4CIF);
    }

    //tandberg能力debug调整
    if (MT_MANU_TAIDE == m_ptMtTable->GetManuId((tMt.GetMtId())) &&
        IsHDConf(m_tConf))
    {
        if (g_cMcuVcApp.IsSendFakeCap2Taide() &&
			0 == strcmp("58", m_ptMtTable->GetVersionId(tMt.GetMtId())))
        {
            tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_CIF);
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuConnectedNotif] tanberg sd mt video resolution modify cif, versionid 58.\n");
        }
        else if (g_cMcuVcApp.IsSendFakeCap2TaideHD())
        {
            tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_HD720);
        }
    }

	BOOL32 bClearCapEx = FALSE;
	
	if (MT_MANU_TAIDE == m_ptMtTable->GetManuId((tMt.GetMtId())) /*&& IsHDConf(m_tConf)*/)
    {
		//主流
		if( tCapSupport.GetMainVideoType() == MEDIA_TYPE_H264 &&
			tCapSupport.GetSecVideoType() == MEDIA_TYPE_H263 )
		{
			TSimCapSet tMainSimCap;
			TSimCapSet tSecSimCap;
			ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_CALL,"[ProcMtMcuConnectNotif] Tanberg MT VersionId: %s\n",m_ptMtTable->GetVersionId(tMt.GetMtId()));

			//针对某些特殊型号的泰德终端，发送H263能力给对端，对端会开h263+通道，导致后向通道开启失败，选用H264能力发送给对端 [8/4/2012 chendaiwei]
			if( 0 == strcmp("512", m_ptMtTable->GetVersionId(tMt.GetMtId())) )
			{
				ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_CALL,"[ProcMtMcuConnectedNotif] Tanberg MT.send 264 ability to mt(%d)\n",tMt.GetMtId() );
			}
			else
			{
				tMainSimCap = tCapSupport.GetSecondSimCapSet();
				tMainSimCap.SetAudioMediaType(tCapSupport.GetMainAudioType());
				tCapSupport.SetMainSimCapSet( tMainSimCap );   // 把第二路填入第一路
			}

			tCapSupport.SetSecondSimCapSet(tSecSimCap);    // 第二路设为空
		}
		if ( tCapSupport.GetSecVideoType() == MEDIA_TYPE_H264 )
		{
			tCapSupport.SetSecVideoType( MEDIA_TYPE_NULL );
		}
		
		//双流
		if( tCapSupport.GetDStreamMediaType() == MEDIA_TYPE_H264 &&
			m_tConf.GetCapSupportEx().IsDDStreamCap() )
		{
			//能力描述迁移
			TDStreamCap tDSSim;
			tDSSim.SetSupportH239(tCapSupport.IsDStreamSupportH239());
			tDSSim.SetMediaType(m_tConf.GetCapSupportEx().GetSecDSType());
			tDSSim.SetMaxBitRate(m_tConf.GetCapSupportEx().GetSecDSBitRate());
			tDSSim.SetFrameRate(m_tConf.GetCapSupportEx().GetSecDSFrmRate());
			tDSSim.SetResolution(m_tConf.GetCapSupportEx().GetSecDSRes());
			
			tCapSupport.SetDStreamCapSet(tDSSim);
			
			//清空扩展
			bClearCapEx = TRUE;
		}
    }
	
	//huawei能力debug调整
	if (MT_MANU_HUAWEI == m_ptMtTable->GetManuId((tMt.GetMtId())))
    {
        /*if (g_cMcuVcApp.IsSendFakeCap2HuaweiSDEp() &&
            0 == strcmp("Huawei H.323 Stack Verion 1.1", m_ptMtTable->GetVersionId(tMt.GetMtId())))
        {
            tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_CIF);
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuConnectedNotif] huawei sd mt video resolution modify cif\n");
        }*/
		u8 byVideoFormat = 0;
        if ( m_tConf.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&			 		
            g_cMcuVcApp.GetFakeCap2HuaweiSDEp(m_ptMtTable->GetIPAddr(tMt.GetMtId()),byVideoFormat) )
        {
			if(m_tConf.GetConfAttrbEx().IsResExCif() && VIDEO_FORMAT_CIF == byVideoFormat )
			{
				tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_CIF);
			}
			if( m_tConf.GetConfAttrbEx().IsResEx4Cif() && VIDEO_FORMAT_4CIF == byVideoFormat )
			{
				tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_4CIF);
			}
			if( m_tConf.GetConfAttrbEx().IsResEx720() && VIDEO_FORMAT_HD720 == byVideoFormat )
			{
				tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_HD720);
			}
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,"[ProcMtMcuConnectedNotif] huawei sd mt video resolution modify to %d\n",byVideoFormat );
        }
    }

	// [12/11/2009 xliang] Aethra 能力调整
	if ( MT_MANU_AETHRA == m_ptMtTable->GetManuId(tMt.GetMtId()) )
	{
		//zbq 20091207 modify for suzhou renfang, 强制取消AethraMt的H239，AethraMcu仍然保留
		TDStreamCap tDSCap = tCapSupport.GetDStreamCapSet();
		if (MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
			MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()))
		{
		}
		else
		{
			tDSCap.SetSupportH239(FALSE);
		}
		
		//双双流 强制放弃 第一双流，只给Aethra第二双流
		if (MEDIA_TYPE_H264 == tCapSupport.GetDStreamMediaType() &&
			tCapSupport.IsDStreamSupportH239() &&
			m_tConf.GetCapSupportEx().IsDDStreamCap())
		{
			TCapSupportEx tCapEx = m_tConf.GetCapSupportEx();
			
			tDSCap.SetMaxBitRate(tCapEx.GetSecDSBitRate());
			tDSCap.SetFrameRate(tCapEx.GetSecDSFrmRate());
			tDSCap.SetMediaType(tCapEx.GetSecDSType());
			tDSCap.SetResolution(tCapEx.GetSecDSRes());
			
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuConnectedNotif] dual cap adj from ExCap due to Aethra!\n");
		}
		
		tCapSupport.SetDStreamCapSet(tDSCap);
	}
	
	//[10/11/25 zhushz] 能力集中包含H264能力polycom不ACK
	if ( MEDIA_TYPE_H264 == tCapSupport.GetMainVideoType() && 
		 MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
		 !strcmp(achVersionId, "Release 7.5.4 - 04 Mar 2005"))
	{
		TSimCapSet tTempSimCap = tCapSupport.GetSecondSimCapSet();
		tCapSupport.SetMainSimCapSet(tTempSimCap);
		tCapSupport.SetSecVideoType(MEDIA_TYPE_NULL);
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuConnectedNotif] polycom mt can't support h264!\n");
	}

	//zjj20110802 外厂商终端没有呼叫吗率和会议吗率的取小机制,所以发送会议能力时填入呼叫吗率
	if( MT_MANU_KDC != m_ptMtTable->GetManuId(tMt.GetMtId()) &&
		MT_MANU_KDCMCU != m_ptMtTable->GetManuId(tMt.GetMtId()) &&
		m_ptMtTable->GetDialBitrate( tMt.GetMtId() ) != m_tConf.GetBitRate() )
	{
		tSimCapSet = tCapSupport.GetMainSimCapSet();
		tSimCapSet.SetVideoMaxBitRate( m_ptMtTable->GetDialBitrate( tMt.GetMtId() ) );
		tCapSupport.SetMainSimCapSet( tSimCapSet );
		tSimCapSet = tCapSupport.GetSecondSimCapSet();
		if( MEDIA_TYPE_NULL != tSimCapSet.GetVideoMediaType() )
		{
			tSimCapSet.SetVideoMaxBitRate( m_ptMtTable->GetDialBitrate( tMt.GetMtId() ) );
			tCapSupport.SetSecondSimCapSet( tSimCapSet );
		}
	}

	// 主流和双流都通过配置文件控制 [pengguofeng 7/24/2012]
	TSimCapSet tMainCapSet = tCapSupport.GetMainSimCapSet();
	TDStreamCap tDSCapSet = tCapSupport.GetDStreamCapSet();
	u32 dwIp = m_ptMtTable->GetIPAddr(tMt.GetMtId());

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuConnectedNotif] Main Conf Capset Type:%d Res:%d Fps:%d BR:%d\n",
		tMainCapSet.GetVideoMediaType(), tMainCapSet.GetVideoResolution(),tMainCapSet.GetUserDefFrameRate(), tMainCapSet.GetVideoMaxBitRate());
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuConnectedNotif] Double Conf Capset Type:%d Res:%d Fps:%d BR:%d\n",
		tDSCapSet.GetMediaType(), tDSCapSet.GetResolution(),tDSCapSet.GetUserDefFrameRate(), tDSCapSet.GetMaxBitRate());

	//     u32 dwIp = INET_ADDR("172.16.227.63");
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuConnectedNotif]Mt:%d ManuId:%d IP:0x%x\n",
		tMt.GetMtId(), byManuId, dwIp);
	
	BOOL32 bSetFake = FALSE;
	BOOL32 bAuioFake = FALSE;
	TAudioTypeDesc tFakeAudioTypeDesc;
	u8 byFakeMask = 0;
	if( g_cMcuVcApp.GetFakeCapParamByMtInfo(dwIp,achProductId,byManuId,tMainCapSet, tDSCapSet,tFakeAudioTypeDesc,byFakeMask))
	{
		bSetFake = TRUE;

		u16 wMainStreamBR = tMainCapSet.GetVideoMaxBitRate();
		u16 wMaxAudioBR = GetMaxAudioBiterate(m_tConfEx);
		if ( wMainStreamBR > wMaxAudioBR)
		{
			wMainStreamBR -= wMaxAudioBR;
			u16 wMainAudioBitrate = GetAudioBitrate(m_tConf.GetMainAudioMediaType());
			//主流码率应小于等于会议码率减去最大音频码率[6/25/2013 chendaiwei]
			if( wMainStreamBR > m_tConf.GetBitRate()+wMainAudioBitrate-wMaxAudioBR)
			{
				wMainStreamBR = m_tConf.GetBitRate()+wMainAudioBitrate-wMaxAudioBR;
			}
		}
		else
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcMtMcuConnectedNotif]Mt:%d faked MStream BitRate is too small<%d> error!\n",
				tMt.GetMtId(),wMainStreamBR);
			
			return;
		}
		
		tMainCapSet.SetVideoMaxBitRate(wMainStreamBR);

		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuConnectedNotif]Faked MainCapset Type:%d Res:%d Fps:%d BR:%d HP:%d\n",
			tMainCapSet.GetVideoMediaType(), tMainCapSet.GetVideoResolution(),
			tMainCapSet.GetUserDefFrameRate(), tMainCapSet.GetVideoMaxBitRate(),
			tMainCapSet.GetVideoCap().GetH264ProfileAttrb());
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuConnectedNotif]Faked DoubleCapset Type:%d Res:%d Fps:%d BR:%d HP:%d\n",
			tDSCapSet.GetMediaType(), tDSCapSet.GetResolution(),
			tDSCapSet.GetUserDefFrameRate(), tDSCapSet.GetMaxBitRate(),
			tDSCapSet.GetH264ProfileAttrb());
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,"[ProcMtMcuConnectedNotif]Faked AudioTypeDesc type:%d,trackNum:%d\n",tFakeAudioTypeDesc.GetAudioMediaType()
			,tFakeAudioTypeDesc.GetAudioTrackNum());

		TVideoStreamCap tOrignalCap = tCapSupport.GetMainSimCapSet().GetVideoCap();
		TVideoStreamCap tOrignalSecCap = tCapSupport.GetSecondSimCapSet().GetVideoCap();
		TDStreamCap tOrignalDSCap = tCapSupport.GetDStreamCapSet();
		TVideoStreamCap tOrignalSecDSCap = m_tConf.GetCapSupportEx().GetSecDSVideoCap();
		//  [8/30/2012 chendaiwei]判断Faked能力集会议是否具有
		if( IS_MAINSTREAMCAP_FAKE(byFakeMask) &&
			( tMainCapSet.GetVideoMediaType() == MEDIA_TYPE_NULL || tOrignalCap == tMainCapSet.GetVideoCap())
			|| m_tConfEx.IsMSSupportCapEx(tMainCapSet.GetVideoResolution(),tMainCapSet.GetVideoMediaType() == MEDIA_TYPE_H264?tMainCapSet.GetUserDefFrameRate():tMainCapSet.GetVideoFrameRate(),tMainCapSet.GetVideoProfileType())
			|| tOrignalSecCap == tMainCapSet.GetVideoCap())
		{
			//主流
			tCapSupport.SetMainSimCapSet(tMainCapSet);
			TSimCapSet tNullsimCapset;
			tCapSupport.SetSecondSimCapSet(tNullsimCapset);
		}
		else
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtMcuConnectedNotif] Faked Main Capset unmatch with MCU Capbility!\n");
		}

		if ( IS_DOUBLESTREAMCAP_FAKE(byFakeMask) && 
			( tDSCapSet.GetMediaType() == MEDIA_TYPE_NULL ||
			(tOrignalDSCap.IsSupportH239() == tDSCapSet.IsSupportH239()
			&& tOrignalDSCap.GetVideoStremCap() == tDSCapSet.GetVideoStremCap())
				|| m_tConfEx.IsDSSupportCapEx(tDSCapSet.GetVideoStremCap().GetResolution(),tDSCapSet.GetMediaType() == MEDIA_TYPE_H264?tDSCapSet.GetUserDefFrameRate():tDSCapSet.GetFrameRate(),tDSCapSet.GetH264ProfileAttrb())
				|| (tOrignalSecDSCap.GetMediaType() == tDSCapSet.GetMediaType()
					&& tOrignalSecDSCap.GetResolution() == tDSCapSet.GetResolution()
						&& tOrignalSecDSCap.GetFrameRate() == tDSCapSet.GetFrameRate()) )
			)
		{
			//双流
			if ( m_tConf.GetCapSupport().GetDStreamMediaType() != MEDIA_TYPE_NULL )
			{
				tCapSupport.SetDStreamCapSet(tDSCapSet);
				tCapSupportEx.SetSecDSType( MEDIA_TYPE_NULL );
			}
		}
		else
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[ProcMtMcuConnectedNotif] Faked DS Capset unmatch with MCU Capbility!\n");
		}

		if ( IS_AUDIODESC_FAKE(byFakeMask) )
		{
			bAuioFake = TRUE;
		}

	}

	cServMsg.SetMsgBody( ( u8 * )&tCapSupport, sizeof( tCapSupport ) );
	u8 bySupportActiveH263 = 0; // 是否设置活动载荷标识
	u8 bySendMSDetermine   = 1; // 是否发送主从决定
	if(m_ptMtTable->GetManuId(tMt.GetMtId()) != MT_MANU_RADVISION)
	{
		bySupportActiveH263 = 1;
	}
	cServMsg.CatMsgBody( ( u8 * )&bySupportActiveH263, sizeof( bySupportActiveH263 ) );
	cServMsg.CatMsgBody( ( u8 * )&bySendMSDetermine, sizeof( bySendMSDetermine ) );

	if (bClearCapEx)
	{
		/*		memset(&tCapSupportEx, 0, sizeof(tCapSupportEx));*/
		//  [7/7/2011 chendaiwei]memset所有值为0不代表能力集为空
		tCapSupportEx.Clear();
	}

	// MCU前向纠错, zgc, 2007-09-27
	cServMsg.CatMsgBody( ( u8 * )&tCapSupportEx, sizeof( tCapSupportEx ) );

    //[6/16/2011 zhushz]iMax会议取配置文件中适配帧率
    /*u8 byFpsAdp = 0;
    if (m_tConf.GetConfAttrbEx().IsImaxModeConf())
    {
        byFpsAdp = g_cMcuVcApp.GetFpsAdp();
    }
    
	cServMsg.CatMsgBody(&byFpsAdp, sizeof(u8));
	*/

	// 组织扩展能力集勾选给Mtadp [12/8/2011 chendaiwei]
	TVideoStreamCap atMSVideoCap[MAX_CONF_CAP_EX_NUM];
	u8 byCapNum = MAX_CONF_CAP_EX_NUM;
	//Fake终端已经伪造能力集，不带勾选[8/2/2012 chendaiwei]
	if(!bSetFake)
	{
		m_tConfEx.GetMainStreamCapEx(atMSVideoCap,byCapNum);
	}

	TVideoStreamCap atDSVideoCap[MAX_CONF_CAP_EX_NUM];
	u8 byDSCapNum = MAX_CONF_CAP_EX_NUM;
	//Fake终端已经伪造能力集，不带勾选[8/2/2012 chendaiwei]
	if(!bSetFake)
	{
		m_tConfEx.GetDoubleStreamCapEx(atDSVideoCap,byDSCapNum);
	}

	cServMsg.CatMsgBody((u8*)&atMSVideoCap[0], sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);
	cServMsg.CatMsgBody((u8*)&atDSVideoCap[0], sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);

	//TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
	//u8 byAudioTrackNum = tAudioType.GetAudioTrackNum();
	//cServMsg.CatMsgBody(&byAudioTrackNum,sizeof(byAudioTrackNum));
	TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];//音频能力
	//从会议属性中取出会议支持的音频类型
	if ( bAuioFake )
	{
		if ( m_tConfEx.IsSupportAudioMediaType(tFakeAudioTypeDesc) )
		{
			atAudioTypeDesc[0].SetAudioMediaType(tFakeAudioTypeDesc.GetAudioMediaType());
			atAudioTypeDesc[0].SetAudioTrackNum(tFakeAudioTypeDesc.GetAudioTrackNum());
		}
		//会议没勾要作假的能力，直接报个空的音频给终端
	}
	else
	{
		m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);
	}
		
	cServMsg.CatMsgBody((u8*)&atAudioTypeDesc[0], sizeof(TAudioTypeDesc)* MAXNUM_CONF_AUDIOTYPE);

	SendMsgToMt( tMt.GetMtId(), MCU_MT_CAPBILITYSET_NOTIF, cServMsg );
	
	//MCU同步终端适配层该会议的会议密码，用于合并级联请求的认证校验
	CServMsg cServMsgNtf;
	cServMsgNtf.SetMsgBody((u8*)m_tConf.GetConfPwd(), MAXLEN_PWD);
	cServMsgNtf.SetEventId(MCU_MT_CONFPWD_NTF);
	BroadcastToAllSubMtJoinedConf( MCU_MT_CONFPWD_NTF, cServMsgNtf );

	return;
}

/*====================================================================
    函数名      ：ProcMcuMtDisonnectedNotif
    功能        ：终端断链处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/05    1.0         LI Yi         创建
    04/02/14    3.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::ProcMtMcuDisconnectedNotif( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8  byReason = *(u8*)cServMsg.GetMsgBody();     // MTLEFT_REASON_*
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );	
	
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Mt%d Disced for %s.%d\n", tMt.GetMtId(), 
             GetMtLeftReasonStr(byReason), byReason );
	TMtStatus tMtStatus;

	TMt tVcsVCMT = m_cVCSConfStatus.GetCurVCMT();
	
	switch( CurState() )
	{
		
	case STATE_ONGOING:	


		
		// 当前请求的终端断链，则可以kill定时器，允许接收下一个调度请求
		if(VCS_CONF == m_tConf.GetConfSource() &&
		   tMt == m_cVCSConfStatus.GetReqVCMT())
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "Mt.%d is vcs ReqMt,Kill req timer\n",tMt.GetMtId() );
			KillTimer(MCUVC_VCMTOVERTIMER_TIMER);

			TMt tNull;
			tNull.SetNull();
			m_cVCSConfStatus.SetReqVCMT(tNull);
		}

		//zjj20100330 vcs会议如果非主动挂断，就把超时挂断设置为终端不可达
		if( VCS_CONF == m_tConf.GetConfSource() &&
			MTLEFT_REASON_LOCAL == byReason )
		{
			
			if( m_ptMtTable->GetMtStatus( tMt.GetMtId(),&tMtStatus ) )
			{
				if( tMtStatus.GetMtDisconnectReason() != MTLEFT_REASON_NORMAL )
				{
					if( tMtStatus.GetMtDisconnectReason() != MTLEFT_REASON_UNREACHABLE )
					{
						tMtStatus.SetMtDisconnectReason( MTLEFT_REASON_UNREACHABLE );
						m_ptMtTable->SetMtStatus( tMt.GetMtId(),&tMtStatus );
						if( !m_tCascadeMMCU.IsNull() )
						{
							OnNtfMtStatusToMMcu( m_tCascadeMMCU.GetMtId(),tMt.GetMtId() );
						}
						else
						{
							MtStatusChange( &tMt,TRUE );
						}
					}
				}
			}
		}

		if( MTLEFT_REASON_NORMAL   == byReason ||
            MTLEFT_REASON_REJECTED == byReason ||
            // zbq [05/14/2007] 终端呼叫类型不匹配处理
            MTLEFT_REASON_UNMATCHTYPE == byReason )
		{
            // guzh [4/16/2007] 如果是下级支持检测上级MCU重复呼叫，则不修改呼叫策略，继续重复呼叫
            if ( (m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_SMCU &&
                  m_ptMtTable->GetManuId(tMt.GetMtId()) == MT_MANU_KDCMCU &&
                  g_cMcuVcApp.IsAutoDetectMMcuDupCall()) ||
				  // VCS会议呼叫模式有特定含义，不可随意修改（当对端拒绝时，更改为手动模式，避免不断弹出提示框）
				  (VCS_CONF == m_tConf.GetConfSource() && (tMt == m_tConf.GetChairman() || tMt == m_cVCSConfStatus.GetVcsBackupChairMan() )
				  ) )
            {
            }
            else
            {
				if( CONF_CALLMODE_NONE != m_ptMtTable->GetCallMode( tMt.GetMtId()) )
				{
					//清除终端呼叫模式
					m_ptMtTable->SetCallMode( tMt.GetMtId(),  CONF_CALLMODE_NONE );
            
					//清除呼叫模式后立即发扩展结构，主要是适应Reject原因下的终端信息不能单发的情况。
					//实际上在Normal原因下，终端离会处理也会发该信息。但为保证会控对终端状态处理的一致性，这里不予理睬。
					cServMsg.SetEventId( MCU_MCS_CALLMTMODE_NOTIF );
					cServMsg.SetMsgBody( (u8*)&m_ptMtTable->m_atMtExt[tMt.GetMtId()-1], sizeof(TMtExt) );
					SendMsgToAllMcs( cServMsg.GetEventId(), cServMsg );
				}
            }
        }

		

		if( m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
		{
			NotifyMcuDropMt(tMt);				
		}
		
		// vrs新录播实体，走新录播逻辑
		if (tMt.GetType() == TYPE_MT && tMt.GetMtType() == MT_TYPE_VRSREC)
		{
			ReleaseVrsMt(tMt.GetMtId());
			return;
		}
        else if( MTLEFT_REASON_NORMAL != byReason  )
		{
            if (MTLEFT_REASON_LOCAL == byReason && 
				m_tConf.HasChairman() && 
				m_ptMtTable->GetAddMtMode(tMt.GetMtId()) == ADDMTMODE_CHAIRMAN)
            {
                TMtAlias tMtAlias;
                BOOL32 bRet = m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeH320ID, &tMtAlias);
				if (!bRet)
                {
					bRet = m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeTransportAddress, &tMtAlias);
					if (!bRet)
					{
						bRet = m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeH323ID, &tMtAlias);
						if (!bRet)
						{
							bRet = m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeE164, &tMtAlias);
							if (!bRet)
							{
								ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "m_ptMtTable->GetMtAlias Fail, mtid = %d\n", tMt.GetMtId());
							}
						}
					}
                }
				if (bRet)
				{
					cServMsg.SetEventId(MCU_MT_ADDMT_NACK);
					cServMsg.SetMsgBody((u8 *)&tMtAlias, sizeof(TMtAlias));
					SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_ADDMT_NACK, cServMsg);
				}

				//通知删除终端
				RemoveMt(tMt, FALSE);
            }
            else if (MT_TYPE_MMCU == tMt.GetMtType() ||
                     m_ptMtTable->IsNotInvited(tMt.GetMtId()) ||
					 MTLEFT_REASON_REMOTERECONNECT == byReason
                     /*m_ptMtTable->GetAddMtMode(tMt.GetMtId()) == ADDMTMODE_MTSELF*/)
            {
				//对于上级MCU，将作为非受邀终端，断开后将自动删除出列表
				RemoveMt(tMt, FALSE);
            }
            else
            {
			    RemoveJoinedMt( tMt, FALSE, byReason );
            }
		}
		else
		{
            if ( MT_TYPE_MMCU == tMt.GetMtType() ||
                 m_ptMtTable->IsNotInvited(tMt.GetMtId()) )
			{
				//通知删除终端
				RemoveMt(tMt, FALSE);
			}
			else
			{
				//通知挂断终端
				RemoveJoinedMt( tMt, FALSE, byReason );
			}
			
			//发给会控终端表, 更新相应状态

			SendMtListToMcs( LOCAL_MCUIDX );
		}

        //[9/14/2011 zhushengze]终端mtc-box申请发言状态广播，下级感知不到上级发言列表，
        //如果上下级异常断开，上级通告发不下来，故在与上级断开统一发广播。
        if ( tMt.GetMtType() == MT_TYPE_MMCU )
        {
            for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
            {
                if ( m_tConfAllMtInfo.MtJoinedConf(byMtId) )
                {
                    TMt tMt2 = m_ptMtTable->GetMt( byMtId );
                    if (tMt2.GetMtType() == MT_TYPE_MT)
                    {
                        NotifyMtSpeakStatus(tMt2, emDenid);
                    }
                    else
                    {
                        TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(tMt2.GetMtId()));

						// 2011-11-2 add by pgf: 对指针作保护
						if ( !ptConfMcInfo)
						{
							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "Get Mc Info Of MtId:%d Error\n", byMtId);
							continue;
						}

                        for (u8 bySmcuMtId = 1; bySmcuMtId <= MAXNUM_CONF_MT; bySmcuMtId++)
                        {
                            if (m_tConfAllMtInfo.MtJoinedConf((TMt)ptConfMcInfo->m_atMtExt[bySmcuMtId - 1]))
                            {
                                NotifyMtSpeakStatus((TMt)ptConfMcInfo->m_atMtExt[bySmcuMtId - 1], emDenid);
                            }
                        }
                    }                    
                }                
            }
            ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuDisconnectedNotif]mmcu disconnect notify all mt speakstatus!\n");
        }

        // xsl [11/8/2006] 释放接入板和转发板资源
        ReleaseMtRes(tMt.GetMtId());
		
		// [11/29/2011 liuxu] 
		RemoveMcu(tMt);
		//若该终端本地发言人，清空本地发言人相关
		if ( VCS_CONF == m_tConf.GetConfSource() && tMt == tVcsVCMT )
		{
//			if(  && 
//				!m_tConf.GetChairman().IsNull() &&
//				( VCS_GROUPCHAIRMAN_MODE == m_cVCSConfStatus.GetCurVCMode() ||
//				VCS_GROUPSPEAK_MODE == m_cVCSConfStatus.GetCurVCMode() )
//				)
			{
				TMt tSpeakMt;
				
				if( CONF_SPEAKMODE_ANSWERINSTANTLY == m_tConf.GetConfSpeakMode() &&
					!m_tApplySpeakQue.IsQueueNull() )
				{
					if( m_tApplySpeakQue.GetQueueHead(tSpeakMt) ) 
					{
						//ChangeSpeaker( &tSpeakMt );
						
						//tSpeakMt.SetConfIdx( m_byConfIdx );
						//m_cVCSConfStatus.SetCurVCMT( tSpeakMt );
						
						//VCSConfStatusNotif();
						
						//NotifyMcsApplyList();
						ChgCurVCMT( tSpeakMt );
					}
					else
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMcuMcuDropMtNotify] Fail to get Queue Head!\n" );
					}
				}
				else
				{
					//tSpeakMt = m_tConf.GetChairman();
					//ChangeSpeaker( &tSpeakMt );
					tSpeakMt.SetNull();
					ChgCurVCMT( tSpeakMt );
					
					//tSpeakMt.SetNull();
					//m_cVCSConfStatus.SetCurVCMT( tSpeakMt );
					//VCSConfStatusNotif();
				}
				
			}		
		}
		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[ProcMcuMtDisconnectedNotif] Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMtMcuCallFailureNotify
    功能        ：终端呼叫失败通知
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/11/23    3.5        胡昌威        创建
====================================================================*/
void CMcuVcInst::ProcMtMcuCallFailureNotify(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);

	if( STATE_ONGOING == CurState() )
	{
		CallFailureNotify(cServMsg);
	}
	else
	{
//		ConfLog( FALSE, "Message %u(%s) received in state %u!\n", 
//			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
	}

	return;
}

/*=============================================================================
    函 数 名： CallFailureNotify
    功    能： 呼叫失败通知
    算法实现： 
    全局变量： 
    参    数： CServMsg	&cServMsg
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/09/08  4.0			李博                   创建
=============================================================================*/
void CMcuVcInst::CallFailureNotify(CServMsg	&cServMsg)
{	
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	
	u8* pbyMsgBody = cServMsg.GetMsgBody();
	pbyMsgBody += sizeof(TMtAlias);
	u8 byReason = *(u8*)pbyMsgBody;
	pbyMsgBody += sizeof(u8);

	// 当前请求的终端呼叫失败，则可以kill定时器，允许接收下一个调度请求
	if(VCS_CONF == m_tConf.GetConfSource() &&
	   tMt == m_cVCSConfStatus.GetReqVCMT())
	{
		KillTimer(MCUVC_VCMTOVERTIMER_TIMER);

		TMt tNull;
		tNull.SetNull();
		m_cVCSConfStatus.SetReqVCMT(tNull);		
	}	
	
	if( ( MTLEFT_REASON_REJECTED == byReason || 
		MTLEFT_REASON_UNREACHABLE == byReason ||
		ERR_MCU_RAS_GK_REJECT == cServMsg.GetErrorCode() ) &&
		VCS_CONF == m_tConf.GetConfSource() &&
		!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) )
	{
		VCSClearTvWallChannelByMt( tMt,TRUE );
	}
	
	BOOL32 bIsDisconnectReasonChg = FALSE;
	BOOL32 bIsCallModeChg = FALSE;
	
	//zjj20100327
	TMtStatus tMtStatus;
	if( m_ptMtTable->GetMtStatus(tMt.GetMtId(),&tMtStatus) )
	{
		if( tMtStatus.GetMtDisconnectReason() != byReason )
		{
			bIsDisconnectReasonChg = TRUE;
			tMtStatus.SetMtDisconnectReason( byReason );
			m_ptMtTable->SetMtStatus( tMt.GetMtId(),&tMtStatus );
		}		
	}	
	//zhouyiliang 20110117 备份主席也要保持定时呼叫
	if( m_tConf.GetConfSource() == VCS_CONF &&
		( MTLEFT_REASON_BUSY == byReason || MTLEFT_REASON_REJECTED == byReason ) &&
		!( tMt==m_tConf.GetChairman() || tMt == m_cVCSConfStatus.GetVcsBackupChairMan() )
		 )
	{
		if( m_ptMtTable->GetCallMode( tMt.GetMtId() ) != CONF_CALLMODE_NONE )
		{
			bIsCallModeChg = TRUE;
			m_ptMtTable->SetCallMode( tMt.GetMtId(),CONF_CALLMODE_NONE );
		}		
	}

	u8 byConfLevel = 0;
	s8 achConfName[MAXLEN_CONFNAME] = {0};
	if (MTLEFT_REASON_BUSYEXT == byReason)
	{
		byConfLevel = *(u8*)pbyMsgBody;
		pbyMsgBody += sizeof(byConfLevel);

		memcpy(achConfName, pbyMsgBody, MAXLEN_CONFNAME);

		// 当前终端在级别更高的会议中不允许抢占
		if (byConfLevel > m_tConf.GetConfLevel())
		{
			byReason = MTLEFT_REASON_BUSY;
			cServMsg.SetErrorCode(ERR_MCU_MTBUSY);

			//zjj20100927 如果终端在高等级会议中一律改成手动呼叫( except for chairman and backup-chairman )
			if( m_tConf.GetConfSource() == VCS_CONF 
				&& CONF_CREATE_MT != m_byCreateBy
				&& !( tMt == m_tConf.GetChairman() ) 
				&& !(tMt == m_cVCSConfStatus.GetVcsBackupChairMan())
				)
			{
				if( m_ptMtTable->GetCallMode( tMt.GetMtId() ) != CONF_CALLMODE_NONE )
				{
					bIsCallModeChg = TRUE;
					m_ptMtTable->SetCallMode( tMt.GetMtId(),CONF_CALLMODE_NONE );
				}
					
			}	

			if( VCS_CONF == m_tConf.GetConfSource() && !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
			{
				VCSClearTvWallChannelByMt( tMt,TRUE );
			}
		}
	}
	
	cServMsg.SetMsgBody( (u8*)&tMt, sizeof(tMt) );
	if (MTLEFT_REASON_BUSYEXT == byReason)
	{
		cServMsg.CatMsgBody((u8*)achConfName, sizeof(achConfName));
	}

	// 对于本地终端强拆不需界面干涉,直接进行强拆
	u8 byAutoForceCall = FALSE;
	if (VCS_CONF == m_tConf.GetConfSource() &&
		MTLEFT_REASON_BUSYEXT == byReason &&
		( tMt == m_tConf.GetChairman()  ||
			 CONF_CREATE_MT == m_byCreateBy ||
			 tMt == m_cVCSConfStatus.GetVcsBackupChairMan()
			 )		
		)
	{
		byAutoForceCall = TRUE;
	}	
	else
	{
	    SendMsgToAllMcs( MCU_MCS_CALLMTFAILURE_NOTIF, cServMsg ); 
	}

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[CallFailureNotify] tMt(%d.%d) Is byAutoForceCall(%d)\n",
		tMt.GetMcuId(),
		tMt.GetMtId(),
		byAutoForceCall 
		);

	//zjj20100327
	//vcs上报终端呼叫失败原因
	if( VCS_CONF == m_tConf.GetConfSource() &&		 
		0 != byReason &&
		( bIsDisconnectReasonChg || bIsCallModeChg )
		)
	{		
		if( CONF_CREATE_MT == m_byCreateBy &&
			!m_tCascadeMMCU.IsNull() )
		{
			OnNtfMtStatusToMMcu( m_tCascadeMMCU.GetMtId(),tMt.GetMtId() );
		}
		else
		{
			MtStatusChange( &tMt,TRUE );
		}
	}
	         
    if (m_tConf.HasChairman() &&
        m_ptMtTable->GetAddMtMode(tMt.GetMtId()) == ADDMTMODE_CHAIRMAN )
    {
        TMtAlias tMtAlias;
        BOOL32 bRet = m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeH320ID, &tMtAlias);
		if (!bRet)
        {
			bRet = m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeTransportAddress, &tMtAlias);
			if (!bRet)
			{
				bRet = m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeH323ID, &tMtAlias);
				if (!bRet)
				{
					bRet = m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeE164, &tMtAlias);
					if (!bRet)
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "m_ptMtTable->GetMtAlias Fail, mtid = %d\n", tMt.GetMtId());
					}
				}
			}
		}
		if (bRet)
		{
			cServMsg.SetEventId(MCU_MT_ADDMT_NACK);
			cServMsg.SetMsgBody((u8 *)&tMtAlias, sizeof(TMtAlias));
			SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_ADDMT_NACK, cServMsg);
		}

		//通知删除终端
		RemoveMt(tMt, FALSE);
    }

    // xsl [11/8/2006] 释放接入板和转发板资源
    ReleaseMtRes(tMt.GetMtId()); 

	if (byAutoForceCall &&
		// 对于平级不启用自动强拆,必须等待对端释放
			( tMt == m_tConf.GetChairman()  ||
			 CONF_CREATE_MT == m_byCreateBy || 
			 tMt == m_cVCSConfStatus.GetVcsBackupChairMan() ||
			 byConfLevel != m_tConf.GetConfLevel() )
		)
	{
		cServMsg.SetConfId( m_tConf.GetConfId() );
		cServMsg.SetNoSrc();
		cServMsg.SetMsgBody( NULL, 0 );
		InviteUnjoinedMt(cServMsg, &tMt, FALSE, FALSE, VCS_FORCECALL_CMD);
	}

	return;
}

/*=============================================================================
    函 数 名： ProcMtMcuMtTypeNotif
    功    能： 终端类型通知（320接入时的额外通知）
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/19  3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::ProcMtMcuMtTypeNotif( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	u8 byMtType = *(cServMsg.GetMsgBody());

	switch( CurState() )
	{
	case STATE_ONGOING:
		
		//设置终端类型
		if( TYPE_MCU == byMtType )
		{
			m_ptMtTable->SetMtType( tMt.GetMtId(), MT_TYPE_SMCU );

			TLogicalChannel	tDataLogicChnnl;
			u32	dwRcvIp  = 0;
			u16	wRcvPort = 0;
			TMtAlias tMtAlias;
			if( !m_ptMtTable->GetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort ) )
			{
				dwRcvIp = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( tMt.GetMtId() ) );
				if( dwRcvIp == 0 )
				{
					return;			    
				}
				g_cMcuVcApp.FindMatchedMpIpForMt(m_ptMtTable->GetIPAddr(tMt.GetMtId()),dwRcvIp);

				wRcvPort = g_cMcuVcApp.AssignMtPort( tMt.GetConfIdx(), tMt.GetMtId() );
				m_ptMtTable->SetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort );
			}

			//级联数据通道
			if( m_ptMtTable->GetDialAlias( tMt.GetMtId(), &tMtAlias ) && 
				mtAliasTypeH320ID == tMtAlias.m_AliasType && 
				m_tConf.GetConfAttrb().IsSupportCascade() &&
				m_tConfAllMcuInfo.GetSMcuNum() < MAXNUM_SUB_MCU )
			{
				tDataLogicChnnl.SetMediaType( MODE_DATA );
				tDataLogicChnnl.SetChannelType( MEDIA_TYPE_MMCU );
				
				tDataLogicChnnl.m_tRcvMediaChannel.SetIpAddr(dwRcvIp);
				tDataLogicChnnl.m_tRcvMediaChannel.SetPort(wRcvPort+9);
				
				cServMsg.SetMsgBody( ( u8 * )&tDataLogicChnnl, sizeof( tDataLogicChnnl ) );			
				SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );
				
				//打印消息
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "Open Data LogicChannel Request send to Mt%d, type: %s\n", 
					      tMt.GetMtId(), GetMediaStr( tDataLogicChnnl.GetChannelType() ) );
			}

			SendMtListToMcs( LOCAL_MCUIDX );

			//  xsl [10/24/2005] 入会后发言人、主席状态通知,用于同步下级mcu状态
			TSimConfInfo tSimConfInfo;
			tSimConfInfo.m_tSpeaker  = GetLocalSpeaker();
			tSimConfInfo.m_tChairMan = m_tConf.GetChairman();            
			cServMsg.SetMsgBody( ( u8 * )&tSimConfInfo, sizeof( tSimConfInfo ) );
			SendMsgToMt(tMt.GetMtId(), MCU_MT_SIMPLECONF_NOTIF, cServMsg );
		}
		else
		{
			m_ptMtTable->SetMtType( tMt.GetMtId(), MT_TYPE_MT); 
		}
		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMtMcuMtJoinNotif
    功能        ：终端成功入会通知
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/02/19    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::ProcMtMcuMtJoinNotif( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	u8 byMcuMaster = *(cServMsg.GetMsgBody());

	switch( CurState() )
	{
	case STATE_ONGOING:
		{
			
			// xsl [10/20/2006] 若达到Mcu License失效日期， 不允许终端入会
			if (g_cMcuVcApp.IsMcuExpiredDate())
			{
				cServMsg.SetMsgBody( (u8*)&tMt, sizeof(tMt) );
				cServMsg.SetErrorCode(ERR_MCU_OVERMAXCONNMT);
				
				SendMsgToAllMcs( MCU_MCS_CALLMTFAILURE_NOTIF, cServMsg );
				
				cServMsg.SetMsgBody();
				SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);
				
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcMtMcuMtJoinNotif] MCU License EXPIRED.\n");  
				
				return;
			}
			
			// [7/9/2010 xliang] mcu license 授权数检查以及接入能力检查不在此处进行了，等到获取终端能力后统一校验
			
			// xsl [11/16/2006] 若会议支持gk计费，且认证失败，则不允许终端入会
			if (m_tConf.IsSupportGkCharge() && !m_tConf.m_tStatus.IsGkCharge())
			{
				SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ProcMtMcuMtJoinNotif] conf %s support gk charge, but gk charge failed.\n", m_tConf.GetConfName());  
				return;
			}
			
			m_ptMtTable->SetMtIsMaster(cServMsg.GetSrcMtId(), byMcuMaster == 1?FALSE:TRUE);
			
			if( m_ptMtTable->GetCallMode(cServMsg.GetSrcMtId()) == CONF_CALLMODE_TIMER )
			{
				m_ptMtTable->SetCallLeftTimes(cServMsg.GetSrcMtId(), m_tConf.m_tStatus.GetCallTimes());
			}
			
			// [9/13/2011 liuxu] 为新加入的终端添加confidx
			TMtStatus tStatus;
			tStatus.Clear();
			if ( m_ptMtTable && m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tStatus) )
			{
				tStatus.SetConfIdx(tMt.GetConfIdx());
				m_ptMtTable->SetMtStatus(tMt.GetMtId(), &tStatus);
			}
			
			//需要密码的会议，对方是受邀终端则直接进入 
			if( m_tConf.GetConfAttrb().GetOpenMode() == CONF_OPENMODE_NEEDPWD && 
				m_ptMtTable &&
				m_ptMtTable->IsNotInvited( tMt.GetMtId() ) )
			{
				// zbq [05/31/2007] 先分配MP给该终端，保证其后向逻辑通道正常打开
				AssignMpForMt(tMt);
				SendMsgToMt( tMt.GetMtId(), MCU_MT_ENTERPASSWORD_REQ, cServMsg );							
			}
			else
			{
				//加入会议
				AddJoinedMt( tMt );
				
				if( tMt.GetMtType() == MT_TYPE_MMCU && g_cMcuVcApp.IsMMcuSpeaker() )
				{
					m_bMMcuSpeakerValid = TRUE;
				}
			}

			// 新录播放像支持
			if (tMt.GetMtType() == MT_TYPE_VRSREC)
			{
				TMtAlias tVrsDialAlias;
				m_ptMtTable->GetDialAlias(tMt.GetMtId(), &tVrsDialAlias);
				if (m_tPlayEqpAttrib.GetReqType() == TPlayEqpAttrib::TYPE_FILELIST)
				{
					//列表请求
					ProcGetVrsListRecord(tVrsDialAlias, tMt, m_tPlayEqpAttrib.GetReqType(), 
						m_tPlayEqpAttrib.GetGroupId(), m_tPlayEqpAttrib.GetPageNo(), m_tPlayEqpAttrib.GetListNum());
				}
				else if (m_tPlayEqpAttrib.GetReqType() == TPlayEqpAttrib::TYPE_STARTPLAY)
				{
					//开启放像请求
					StartVrsPlay(tVrsDialAlias, m_tPlayEqpAttrib);
				}
			}
			
			// [7/9/2010 xliang] 该通知在mcu license授权校验过了后再发
			//通知其他MCU new mt
			//NotifyMcuNewMt(tMt);     
		}
		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMtMcuEnterPwdRsp
    功能        ：终端应答询问密码
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/02/19    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::ProcMtMcuEnterPwdRsp( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	s8 *pszPwd  = (s8 *)cServMsg.GetMsgBody();
	s32 nPwdLen = cServMsg.GetMsgBodyLen();
	
	if( tMt.IsNull() )
	{
		return;
	}

	switch( CurState() )
	{
	case STATE_ONGOING:
		if (m_tConfAllMtInfo.MtJoinedConf( tMt.GetMcuId(), tMt.GetMtId() ))
		{
			break;
		}
		if( MT_MCU_ENTERPASSWORD_NACK == pcMsg->event)
		{
			//失败应答，实际上并不会产生这条信令
			if( MT_TYPE_SMCU == tMt.GetMtType() || MT_TYPE_MMCU == tMt.GetMtType() )
			{
				RemoveMt( tMt, TRUE );
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mcu%d password wrong, drop it!\n", tMt.GetMtId() );
			}
			else
			{
				SendMsgToMt( tMt.GetMtId(), MCU_MT_ENTERPASSWORD_REQ, cServMsg );
				ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mt%d password wrong, request once more!\n", tMt.GetMtId() );
			}
		}
		else
		{
			DealMtMcuEnterPwdRsp(tMt, pszPwd, nPwdLen);
		}
		break;
		
	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			     pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

	return;
}

/*=============================================================================
    函 数 名： DealMtMcuEnterPwdRsp
    功    能： 处理终端应答询问密码
    算法实现： 
    全局变量： 
    参    数： TMt tMt
               s8 *pszPwd
               s32 nPwdLen
    返 回 值： BOOL32
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/11/3   3.6			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcInst::DealMtMcuEnterPwdRsp(TMt tMt, s8 *pszPwd, s32 nPwdLen)
{
	if (NULL == pszPwd || 0 == nPwdLen)
	{
		return FALSE;
	}
	
	BOOL32 bRet = FALSE;	
	CServMsg cServMsg;
	TMultiCapSupport tCapSupport;
    TCapSupportEx tCapSupportEx;
	TLogicalChannel tLogicalChannel;
	s8 achConfPwd [MAXLEN_PWD+1] = {0};
	strncpy( achConfPwd, pszPwd, sizeof( achConfPwd ) );
	achConfPwd[sizeof( achConfPwd ) - 1] = '\0';
	
	if( nPwdLen != (s32)strlen(m_tConf.GetConfPwd()) ||
		0 != strncmp( achConfPwd, m_tConf.GetConfPwd(), MAXLEN_PWD) )
	{
		if ( MT_TYPE_SMCU == tMt.GetMtType() || MT_TYPE_MMCU ==  tMt.GetMtType() )
		{
			RemoveMt( tMt, TRUE );
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mcu%d password wrong, drop it!\n", tMt.GetMtId() );
		}
		else
		{
			SendMsgToMt( tMt.GetMtId(), MCU_MT_ENTERPASSWORD_REQ, cServMsg );
			ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "Mt%d password wrong, request once more!\n", tMt.GetMtId() );
		}
	}
	else
	{
		//加入会议
		AddJoinedMt( tMt );

		if( tMt.GetMtType() == MT_TYPE_MMCU && g_cMcuVcApp.IsMMcuSpeaker() )
		{
			m_bMMcuSpeakerValid = TRUE;
		}

		//处理终端能力集
		m_ptMtTable->GetMtMultiCapSupport( tMt.GetMtId(), &tCapSupport );
        m_ptMtTable->GetMtCapSupportEx( tMt.GetMtId(), &tCapSupportEx);

		ProcMtCapSet( tMt, tCapSupport);

		//发送视频码流处理
		if( m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE ) )
		{			
			//动态分屏改变风格
			if( g_cMcuVcApp.GetVMPMode(m_tVmpEqp) != CONF_VMPMODE_NONE )
			{
				ChangeVmpStyle( tMt, TRUE );
			}

			//若是发言人加入
			if( m_tConf.HasSpeaker() && GetLocalSpeaker() == tMt )
			{
                // xsl [8/22/2006]卫星分散会议时需要判断回传通道数
                if (m_tConf.GetConfAttrb().IsSatDCastMode() /*&& IsOverSatCastChnnlNum(tMt.GetMtId())*/
					&& IsMultiCastMt(tMt.GetMtId())
					&& //IsSatMtOverConfDCastNum(tMt))
					!IsSatMtCanContinue(tMt.GetMtId(),emSpeaker))
                {
                    ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[DealMtMcuEnterPwdRsp] over max upload mt num. cancel speaker!\n");
                    ChangeSpeaker(NULL);
                }
                else
                {
                    ChangeSpeaker( &tMt );		
                }				
			}

			if( !( GetLocalSpeaker() == tMt ) )
			{
				NotifyMtSend( tMt.GetMtId(), MODE_VIDEO, FALSE );
			}
		}
	
		//发送音频码流处理	
		if( m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE ) )
		{
			if( m_tConf.m_tStatus.IsMixing() )
			{				
                // xsl [8/4/2006] 定制混音交换n模式码流，其他情况加入混音通道
                if (m_tConf.m_tStatus.IsSpecMixing())
                {
                    SwitchMixMember(&tMt, TRUE);
                }
                else
                {
                    if (GetMixMtNumInGrp() < GetMaxMixNum(m_tMixEqp.GetEqpId()))
                    {
                        AddMixMember( &tMt, DEFAULT_MIXER_VOLUME, FALSE );
						StartSwitchToPeriEqp(tMt, 0, m_tMixEqp.GetEqpId(), 
											 (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt)), 
											 MODE_AUDIO, SWITCH_MODE_SELECT);
                    }                    
                }                				             			
			}
			else
			{
				NotifyMtSend( tMt.GetMtId(), MODE_AUDIO, FALSE );
			}
		}

		//本终端已经通过密码验证, 发送级联列表处理	
		if( m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_MMCUDATA, &tLogicalChannel, FALSE ) )
		{
			m_tCascadeMMCU = tMt;
			m_tConfAllMtInfo.m_tMMCU = tMt;
            m_tConfInStatus.SetNtfMtStatus2MMcu(TRUE);
			
			//将此级联mcu信息进行添加置列表中
			u16 wMcuIdx = INVALID_MCUIDX;
			u8 abyMcuId[MAX_CASCADEDEPTH-1];
			memset( &abyMcuId[0],0,sizeof(abyMcuId) );
			abyMcuId[0] = tMt.GetMtId();
			m_tConfAllMcuInfo.AddMcu( &abyMcuId[0],2,&wMcuIdx );
			m_ptConfOtherMcTable->AddMcInfo( wMcuIdx );	
			m_tConfAllMtInfo.AddMcuInfo( wMcuIdx, tMt.GetConfIdx() );
			
			//通知会控
			cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
			if(m_tConf.HasConfExInfo())
			{
				u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
				u16 wPackDataLen = 0;
				PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
				cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);
			}
			SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
			
			//发给会控会议所有终端信息
			//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
			//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
			SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
			
			//auto speak
			if(m_tConf.GetSpeaker().IsNull() && g_cMcuVcApp.IsMMcuSpeaker())
			{
				ChangeSpeaker(&m_tCascadeMMCU);
			}
		}

		bRet = TRUE;
	}

	return bRet;
}

/*====================================================================
    函数名      ：ProcMtMcuEnterPwdReq
    功能        ：终端询问密码
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/02/19    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::ProcMtMcuEnterPwdReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );


	switch( CurState() )
	{
	case STATE_ONGOING:
		if(strlen(m_tConf.GetConfPwd()) > 0)
		{
			cServMsg.SetMsgBody((u8 *)(m_tConf.GetConfPwd()), strlen(m_tConf.GetConfPwd()));
			SendMsgToMt( tMt.GetMtId(), MT_MCU_ENTERPASSWORD_ACK,  cServMsg);
		}
		else
		{
			SendMsgToMt( tMt.GetMtId(), MT_MCU_ENTERPASSWORD_NACK,  cServMsg);
		}
		break;
	default:
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*=============================================================================
    函 数 名： ProcMtMcuFlowControlCmd
    功    能： 终端要求改变接收码率 - 码流控制命令
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
               BOOL32 bNtfPhyBandWidth: 是否响应终端物理带宽通知
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/21   3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::ProcMtMcuFlowControlCmd( const CMessage * pcMsg, BOOL32 bNtfPhyBandWidth )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt;
	TLogicalChannel	tLogicChnnl;
	u16 wBitRate, wMinBitRate = 0;

	BOOL bStartDoubleStream = FALSE;
    BOOL32 bMtLowedAlready = FALSE;

	BOOL bIsPrimaryUp = TRUE;
	BOOL bIsDoubleUp = TRUE;

	BOOL32 bIsNeedPrimaryBrdAdapt = FALSE;
	BOOL32 bIsNeedDoubleBrdAdapt = FALSE;
	TMt tBrdSrc = GetVidBrdSrc();

	BOOL32 bIsAdjustMStreamWithDiscount = FALSE; //标识调整的主流码率是否已经打折.TRUE表示已经打折，FALSE表示未打折.
	u16 wRealMtReqBR = 0; //bIsAdjustMStreamWithDiscount为TRUE，wRealMtReqBR代表打折后的主流码率；为FALSE，代表未打折的主流码率[9/27/2012 chendaiwei]
	u16	wCurMtReqBR = 0;

	BOOL32 bIsE1Mcu = FALSE;

	switch( CurState() )
	{
	case STATE_ONGOING:
		{
		tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
		if((m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_MMCU|| 
			m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_SMCU)
			&& m_ptMtTable->GetMtTransE1(tMt.GetMtId()))
		{
			bIsE1Mcu = TRUE;
		}

		tLogicChnnl = *( TLogicalChannel * )( cServMsg.GetMsgBody() );
		wBitRate = tLogicChnnl.GetFlowControl();

		//////////////////////////////////////////////
		//FlowControlToZero
		if( !m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId()) || 0 == wBitRate )
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] 0 == wBitRate MT.%d\n",
                   tMt.GetMtId());
			return;
		}

		if( MODE_AUDIO == tLogicChnnl.GetMediaType() )
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] flowcontrol audio channel.MT.%d\n",
                   tMt.GetMtId());
			return;
		}

		

		//不理睬其它厂商终端的FlowControl
		if( ( m_ptMtTable->GetManuId( tMt.GetMtId() ) != MT_MANU_KDC && 
				m_ptMtTable->GetManuId( tMt.GetMtId() ) != MT_MANU_KDCMCU ) /*||
				wBitRate > m_tConf.GetBitRate() */)
		{
			// xliang [11/14/2008] 单独处理PolycomMCU的flowctrl，对应于双流起停处理
			if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
				( MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
				MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId())))
			{
				ProcPolycomMcuFlowctrlCmd(tMt.GetMtId(), tLogicChnnl);
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcMtMcuFlowControlCmd] PolycomMcu.%d flowctrl received\n", tMt.GetMtId() );
			}
			return;
		}

		//避免E1级联下级调上级码率失败[7/6/2012 chendaiwei]
// 		// xsl [8/7/2006] 下级mcu发送的第一路flowcontrol不处理
// 		if( MODE_SECVIDEO != tLogicChnnl.GetMediaType() &&
// 			m_ptMtTable->GetManuId( tMt.GetMtId() ) == MT_MANU_KDCMCU )
// 		{
// 			if (m_tCascadeMMCU.IsNull() || m_tCascadeMMCU.GetMtId() != tMt.GetMtId())
//             {
//                 ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] ignore smcu MtReqBitrate :%d from mcu<%d>\n",
//                    wBitRate, tMt.GetMtId());
//                 return;
//             }
// 		}

		if( MODE_VIDEO == tLogicChnnl.GetMediaType() )
		{
			if (wBitRate > m_tConf.GetBitRate())
			{
				wBitRate = m_tConf.GetBitRate();
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "wBitRate is larger than conf bitrate, modify it equal to conf's bitrate! \n");
			}

			//打折后的主流码率有可能小于双速低速。此处判断有误[10/22/2012 chendaiwei]
// 			if ( 0 != m_tConf.GetSecBitRate() && wBitRate < m_tConf.GetSecBitRate() )
// 			{
// 				wBitRate = m_tConf.GetSecBitRate();
// 				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "wBitRate is smaller than conf sec bitrate, modify it equal to conf's sec bitrate! \n");
// 			}

			//主流前向通道与MCU调主流码率相同，跳过[9/27/2012 chendaiwei]
			if(wBitRate == m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE))
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] wBitRate(%d) is not change.MT.%d\n",
                   wBitRate,tMt.GetMtId() );
				return;	
			}
			else if( !m_ptMtTable->GetMtTransE1(tMt.GetMtId())
				     && wBitRate < m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE))
			{
				//级联MCU调源。有可能上下级MCU双流码率百分比不同，导致wBitRate与m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE)
				//不一致[9/27/2012 chendaiwei]
				bIsAdjustMStreamWithDiscount = TRUE;
			}
			else if (!m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
					wBitRate > m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE)
				   && wBitRate < (m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE)+m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(),FALSE)))
			{
				bIsAdjustMStreamWithDiscount = TRUE;
			}

			if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) )
			{
				u16 wRcvBandWidth = m_ptMtTable->GetRcvBandWidth(tMt.GetMtId());            
				if (wRcvBandWidth < wBitRate)
				{
					m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), FALSE);
				}
				else
				{
					if ( abs(wBitRate-wRcvBandWidth)*100 / wRcvBandWidth > 20 )
					{
						m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), TRUE);
					}
					else
					{
						m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), FALSE);
					}
				}
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuFlowControlCmd] mt.%u bLowed: %d (wRcvBandWidth:%d,wBitRate:%d, conf bitrate:%d)\n",
					tMt.GetMtId(), m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()),
					wRcvBandWidth, wBitRate,m_tConf.GetBitRate());			
			}

			if( !m_tDoubleStreamSrc.IsNull() )
			{
				if( !bIsAdjustMStreamWithDiscount && GetDoubleStreamVideoBitrate( wBitRate,TRUE ) == m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(),TRUE ) 
					&& !bIsE1Mcu)
				{
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] Fst wBitRate(%d) is not change.MT.%d\n",
							GetDoubleStreamVideoBitrate( wBitRate,TRUE ),tMt.GetMtId() );
					return;
				}
				if( !bIsAdjustMStreamWithDiscount && GetDoubleStreamVideoBitrate( wBitRate,FALSE ) == m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(),FALSE )
					&& !bIsE1Mcu)
				{
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] Sec wBitRate(%d) is not change.MT.%d\n",
							GetDoubleStreamVideoBitrate( wBitRate,FALSE ),tMt.GetMtId() );
					return;
				}
				if( (!bIsAdjustMStreamWithDiscount && GetDoubleStreamVideoBitrate( wBitRate,TRUE ) < m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(),TRUE )) 
					|| (bIsAdjustMStreamWithDiscount && wBitRate < m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(),TRUE ))
					)
				{
					bIsPrimaryUp = FALSE;
				}
				if( (!bIsAdjustMStreamWithDiscount && GetDoubleStreamVideoBitrate( wBitRate,FALSE ) < m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(),FALSE))
					|| (bIsAdjustMStreamWithDiscount && wBitRate < m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(),FALSE ))
					)
				{
					bIsDoubleUp = FALSE;
				}
				
			}
			else
			{
				if( wBitRate == m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(),TRUE ) )
				{
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] wBitRate(%d) is not change.MT.%d\n",
							wBitRate,tMt.GetMtId() );
					return;
				}
				if(  wBitRate < m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(),TRUE ) )
				{
					bIsPrimaryUp = FALSE;
				}
			}

			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] bIsPrimaryUp.%d bIsDoubleUp.%d .MT.%d BitRate.%d\n",
							bIsPrimaryUp,bIsDoubleUp,tMt.GetMtId(),wBitRate );
			
			wCurMtReqBR = m_ptMtTable->GetMtReqBitrate(tMt.GetMtId());
			
			if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) && m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_MT)
			{
				if( !m_tDoubleStreamSrc.IsNull() )
				{			
					//开着双流 呼入终端，记录即可
					if ( bNtfPhyBandWidth )
					{
						wRealMtReqBR = wBitRate;
					}
					//双流中 E1终端降速，重新计算, 并整理第一路和第二路的带宽，并通知双流源更新发送带宽
					else
					{
						wRealMtReqBR = wBitRate * 2;
						if( wRealMtReqBR > m_tConf.GetBitRate() )
						{
							wRealMtReqBR = m_tConf.GetBitRate();
						}
					}
				}
				else
				{
					wRealMtReqBR = wBitRate;
				}	
				if( m_ptMtTable->GetMtBRBeLowed( tMt.GetMtId() ) )
				{
					m_ptMtTable->SetLowedRcvBandWidth(tMt.GetMtId(), wRealMtReqBR);
				}
			}
			else
			{
				wRealMtReqBR = wBitRate;			
			}

			if( !tBrdSrc.IsNull() && tBrdSrc.GetType() != TYPE_MCUPERI)
			{				
				bIsNeedPrimaryBrdAdapt = IsNeedAdapt( tBrdSrc,tMt,MODE_VIDEO );				
			}
			if( !m_tDoubleStreamSrc.IsNull() )
			{
				bIsNeedDoubleBrdAdapt = IsNeedAdapt( m_tDoubleStreamSrc,tMt,MODE_SECVIDEO );
			}

			//E1MCU带宽始终是未打折带宽[10/29/2012 chendaiwei]
			if( !m_tDoubleStreamSrc.IsNull() 
				&& ( bIsE1Mcu || !bIsAdjustMStreamWithDiscount))
			{
				m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), GetDoubleStreamVideoBitrate(wRealMtReqBR, TRUE) , LOGCHL_VIDEO );
				m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), GetDoubleStreamVideoBitrate(wRealMtReqBR, FALSE) , LOGCHL_SECVIDEO );		
			}
			else
			{			
				m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), wRealMtReqBR, LOGCHL_VIDEO );
			}
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] CurMtReqBR.%d while RealReqBR.%d, adjust to Fst.%d, Sec.%d bIsNeedPrimaryBrdAdapt.%d bIsNeedDoubleBrdAdapt.%d bIsFlowCtrlMainStream.%d\n",
				wCurMtReqBR, wRealMtReqBR, 
				m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), LOGCHL_VIDEO),
				m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), LOGCHL_SECVIDEO),
				bIsNeedPrimaryBrdAdapt,
				bIsNeedDoubleBrdAdapt,
					bIsAdjustMStreamWithDiscount);
			

		//}


		//if( MODE_VIDEO == tLogicChnnl.GetMediaType() )
		//{
			//标识本地终端是否是通过多回传上传的全局发言人[11/27/2012 chendaiwei]
			BOOL32 bHasGlobalSpeakerMultiSpy = FALSE;
			TMt tLocalGlobalSpeaker;
			u8 byGlobalSpeakerId = 0;
			CSendSpy *ptSndSpy = NULL;
			TSimCapSet tDstCap;
			if (!m_tCascadeMMCU.IsNull() && tMt == m_tConf.GetSpeaker() && m_tCascadeMMCU == tMt )
			{
				TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
				
				if( ptConfMcInfo != NULL						
					&& ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.IsMcuIdLocal()
					)
				{			
					byGlobalSpeakerId = ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId();
					tLocalGlobalSpeaker = m_ptMtTable->GetMt(byGlobalSpeakerId);
					tLocalGlobalSpeaker.SetMcuId(LOCAL_MCUID);
					tLocalGlobalSpeaker.SetMtId(byGlobalSpeakerId);
					
					u16 wMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
					if( !IsValidMcuId( wMcuIdx ) )
					{
						ConfPrint( LOG_LVL_ERROR, MID_MCU_MT2, "[ProMtMcuFlowCtrl] wMcuIdx.%d is invalid\n",wMcuIdx );
						return;
					}
					
					BOOL32 bIsMMcuSupMultSpy = IsLocalAndSMcuSupMultSpy( wMcuIdx );
					u8 bySpyNum = 0;
					u32 dwSpyBandWidth = 0;
					u8 bySpyIdx = 0;
					if( bIsMMcuSupMultSpy )
					{
						for (bySpyIdx = 0; bySpyIdx < MAXNUM_CONF_SPY; bySpyIdx++)
						{
							tDstCap.Clear();
							ptSndSpy = m_cLocalSpyMana.GetSendSpy(bySpyIdx);
							if (NULL == ptSndSpy)
							{
								continue;
							}
							if( ptSndSpy->GetSpyMt().IsNull() )
							{
								continue;
							}
							if( !m_tConfAllMtInfo.MtJoinedConf( GetLocalMtFromOtherMcuMt( ptSndSpy->GetSpyMt() ).GetMtId() ) )
							{
								continue;
							}
							
							TMt tLocalSpyMt = GetLocalMtFromOtherMcuMt( ptSndSpy->GetSpyMt() );
							if(tLocalSpyMt.GetMtId()!=byGlobalSpeakerId)
							{
								continue;
							}
							else
							{
								bHasGlobalSpeakerMultiSpy = TRUE;
								tDstCap = ptSndSpy->GetSimCapset();
								ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2,"[ProcMtMcuFlowCrlCmd] Mt(%d) is GlobalSpeaker\n",byGlobalSpeakerId );
								break;
							}
						}
					}	
				}
			}

			TMtStatus tStatus;
			if( !m_ptMtTable->GetMtStatus(tMt.GetMtId(),&tStatus) )
			{
				return;
			}
			
			TMt tSelMt = tStatus.GetSelectMt( MODE_VIDEO );
			if( tSelMt.IsNull() )
			{
				TMt tVidSrc = tStatus.GetVideoMt();
				if( tMt.GetMtId() == m_tCascadeMMCU.GetMtId() && !tVidSrc.IsNull())
				{
					if( !(tVidSrc == tMt))
					{
						if( !IsNeedSelAdpt( tVidSrc,tMt,MODE_VIDEO ) )
						{
							AdjustMtVideoSrcBR( tMt.GetMtId(),wRealMtReqBR,MODE_VIDEO,TRUE );
						}
						else
						{
							if ( TRUE == bIsPrimaryUp )
							{
								RefreshSelBasParam( tVidSrc,tMt,MODE_VIDEO,TRUE,TRUE );
							}
							else
							{
								RefreshSelBasParam( tVidSrc,tMt,MODE_VIDEO );
							}
						}
					}	
				}
				//下级收上级FlowCtrl 下级真实广播源是本级终端[11/27/2012 chendaiwei]
				else if( GetLocalMtFromOtherMcuMt(tBrdSrc)== tMt 
					&& tMt.GetMtId() == m_tCascadeMMCU.GetMtId()
					&& bHasGlobalSpeakerMultiSpy)
				{
					if( !IsNeedSpyAdpt( tLocalGlobalSpeaker,tDstCap,MODE_VIDEO ))
					{
						TLogicalChannel tLogic;
						CServMsg cMsg;
						if(m_ptMtTable->GetMtLogicChnnl(tLocalGlobalSpeaker.GetMtId(),LOGCHL_VIDEO,&tLogic,FALSE))
						{
							u16 wAcualBR = 0;
							if( !m_tDoubleStreamSrc.IsNull() && !bIsAdjustMStreamWithDiscount/* && m_tDoubleStreamSrc == tBrdSrc*/ )
							{
								wAcualBR = GetDoubleStreamVideoBitrate(wRealMtReqBR,TRUE);
							}
							else
							{
								wAcualBR = wRealMtReqBR;
							}

							tLogic.SetFlowControl(wAcualBR);
							cMsg.SetMsgBody( (u8*)&tLogic, sizeof( tLogic ) );			

							SendMsgToMt( tLocalGlobalSpeaker.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cMsg );
						}
					}
					else
					{
						tDstCap.SetVideoMaxBitRate( m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(), TRUE ));
						if( ptSndSpy != NULL)
						{
							m_cLocalSpyMana.SaveSpySimCap(ptSndSpy->GetSpyMt(), tDstCap);
						}

						if ( TRUE == bIsPrimaryUp )
						{
							RefreshSpyBasParam( tLocalGlobalSpeaker,tDstCap,MODE_VIDEO,TRUE,TRUE );
						}
						else
						{
							RefreshSpyBasParam( tLocalGlobalSpeaker,tDstCap,MODE_VIDEO);
						}

						ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowCtrl] SpyMt(%d.%d) refresh bas to wModifiedBandWidth.%d!\n", 
							tLocalGlobalSpeaker.GetMcuId(),tLocalGlobalSpeaker.GetMtId(),m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(), TRUE ) );
	
					}
				}
				else if( !tBrdSrc.IsNull() && 
					      ( m_tCascadeMMCU.IsNull() || tMt.GetMtId() != m_tCascadeMMCU.GetMtId()) &&
						  //CMD源和广播源同时是下级MCU[11/27/2012 chendaiwei]
					      ( !(GetLocalMtFromOtherMcuMt(tBrdSrc)==tMt)
						  || (m_ptMtTable->GetMtType(tMt.GetMtId())== MT_TYPE_SMCU && GetLocalMtFromOtherMcuMt(tBrdSrc)==tMt))
					   )
				{
					if( tBrdSrc.GetType() == TYPE_MCUPERI )
					{
						 if( tBrdSrc.GetEqpType() == EQP_TYPE_RECORDER )
						 {
							ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] brd is recorder.so not handle. mt.%d\n",tMt.GetMtId() );
						 }
						 else if( tBrdSrc.GetEqpType() == EQP_TYPE_VMP )
						 {
							ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] brd is vmp.adjust vmp birrate.mt.%d\n",tMt.GetMtId() );
							AdjustVmpBrdBitRate( &tMt );
						 }
					}
					else
					{
						if( 0 == m_tConf.GetSecBitRate() )
						{
							if( !bIsNeedPrimaryBrdAdapt )
							{
								if( tBrdSrc.IsLocal() 
									|| (m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_SMCU && GetLocalMtFromOtherMcuMt(tBrdSrc)==tMt))
								{
									if( !m_tDoubleStreamSrc.IsNull() && !bIsAdjustMStreamWithDiscount/* && m_tDoubleStreamSrc == tBrdSrc*/ )
									{
										AdjustMtVideoSrcBR( tMt.GetMtId(), GetDoubleStreamVideoBitrate(wRealMtReqBR,TRUE), MODE_VIDEO,TRUE );
									}
									else
									{
										AdjustMtVideoSrcBR( tMt.GetMtId(),wRealMtReqBR,MODE_VIDEO,TRUE );
									}
								}
								else
								{
									//下级E1自行检测
								}
							}
							else
							{
								if ( TRUE == bIsPrimaryUp  )
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_VIDEO,TRUE,TRUE );
								}
								else
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_VIDEO);
								}
							
							}
						}
						else
						{
							u16 wSpyPort = SPY_CHANNL_NULL;
							CRecvSpy tSpyResource;	
							if( !tBrdSrc.IsLocal() &&
								m_cSMcuSpyMana.GetRecvSpy( tBrdSrc, tSpyResource )
								)
							{
								wSpyPort = tSpyResource.m_tSpyAddr.GetPort();
							}				
							if( !bIsNeedPrimaryBrdAdapt )
							{												
								if( IsNeedAdapt( tBrdSrc,tMt,MODE_VIDEO ) )
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_VIDEO );
									StartSwitchFromBrd( tBrdSrc,0,1,&tMt,wSpyPort );									
								}
								else
								{
									ConfPrint( LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMtMcuFlowControlCmd] before after all not adapt,it's impossible.mt(%d)\n",tMt.GetMtId() );
								}
							}
							else
							{
								if( !IsNeedAdapt( tBrdSrc,tMt,MODE_VIDEO ) )
								{
									ConfPrint( LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMtMcuFlowControlCmd] before nedd adapt,after not adapt,rehandle switch.mt(%d)\n",tMt.GetMtId() );
									StartSwitchFromBrd( tBrdSrc,0,1,&tMt,wSpyPort );
									
									u16 wMtBitRate = m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(),TRUE );
									m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(),wMtBitRate - 10,LOGCHL_VIDEO);
									//恢复刚才接收
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_VIDEO,TRUE,TRUE );
									m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(),wMtBitRate,LOGCHL_VIDEO);
								}
								else
								{
									if ( TRUE == bIsPrimaryUp  )
									{
										RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_VIDEO,TRUE,TRUE );
									}
									else
									{
										RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_VIDEO);
									}
								}
							}
						}
					}
				}
				//E1 SMCU做主流广播源，其发送逻辑通道码率需调整[11/6/2012 chendaiwei]
				else if(GetLocalVidBrdSrc()== tMt && bIsE1Mcu)
				{
					TLogicalChannel tMainLogicChnnl;
					if (m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tMainLogicChnnl, FALSE))
					{
						u16 wMainBR = m_tDoubleStreamSrc.IsNull()?wRealMtReqBR:GetDoubleStreamVideoBitrate(wRealMtReqBR,TRUE);
						tMainLogicChnnl.SetFlowControl(wMainBR);
						m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tMainLogicChnnl, FALSE);
						
						LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMtMcuFlowControlCmd] SMCU<%d> is BRD SRC.Adjust RL BR to%d\n",tMt.GetMtId(),wMainBR);
					}
				}
			}//!tSelMt.IsNull()
			else
			{
				if( tSelMt.GetType() == TYPE_MCUPERI && tSelMt.GetEqpType() == EQP_TYPE_VMP )
				{
					ConfPrint( LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMtMcuFlowControlCmd] sel vmp.so ajdust vmp mt(%d)\n",tMt.GetMtId() );
					AdjustVmpSelBitRate( tSelMt );
				}
				else
				{
					//是否需要判码率是否被双流占用[11/27/2012 chendaiwei]
					if( !(tSelMt == tMt))
					{
						if( !IsNeedSelAdpt( tSelMt,tMt,MODE_VIDEO ) )
						{
							AdjustMtVideoSrcBR( tMt.GetMtId(),wRealMtReqBR,MODE_VIDEO,TRUE );
						}
						else
						{
							if ( TRUE == bIsPrimaryUp )
							{
								RefreshSelBasParam( tSelMt,tMt,MODE_VIDEO,TRUE,TRUE );
							}
							else
							{
								RefreshSelBasParam( tSelMt,tMt,MODE_VIDEO );
							}
						}
					}					
				}
			}
			if ( !m_tDoubleStreamSrc.IsNull()  && !(m_tDoubleStreamSrc == m_tPlayEqp) )
            {
				if(!(tMt == m_tDoubleStreamSrc) )
				{
					TMt tMtNull;
					tMtNull.SetNull();
					//ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] E1 Lowed Mt restore, adjust DS Src.\n");
					if( 0 == m_tConf.GetSecBitRate() )
					{
						if( !bIsNeedDoubleBrdAdapt )
						{
							if( !m_tDoubleStreamSrc.IsNull() && m_tDoubleStreamSrc == tBrdSrc )
							{
								//AdjustMtVideoSrcBR( tMt.GetMtId(), GetDoubleStreamVideoBitrate(wRealMtReqBR,FALSE), MODE_SECVIDEO );
								RestoreVidSrcBitrate(tMtNull, MODE_SECVIDEO);
							}
							else
							{
								//AdjustMtVideoSrcBR( tMt.GetMtId(),wRealMtReqBR,MODE_SECVIDEO );
								RestoreVidSrcBitrate(tMtNull, MODE_SECVIDEO);
							}								
						}
						else
						{
							if ( TRUE == bIsDoubleUp  )
							{
								RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
							}
							else
							{
								RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO);
							}
						}
					}
					else
					{
						if( !bIsNeedDoubleBrdAdapt )
						{
							if( IsNeedAdapt(m_tDoubleStreamSrc, tMt, MODE_SECVIDEO)/*m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId(), FALSE)*/)
							{							
								if ( TRUE == bIsDoubleUp  )
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
								}
								else
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO);
								}
								StartSwitchToSubMtFromAdp(tMt.GetMtId(), MODE_SECVIDEO);
							}
							else
							{
								ConfPrint( LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMtMcuFlowControlCmd] before after all not Sec adapt,it's impossible.mt(%d)\n",tMt.GetMtId() );
							}
						}					
						else
						{
							if( !IsNeedAdapt(m_tDoubleStreamSrc, tMt, MODE_SECVIDEO) )
							{			
								ConfPrint( LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMtMcuFlowControlCmd] before need Sec adapt,after not Sec adapt,rehandle switch.mt(%d)\n",tMt.GetMtId() );
								TSwitchGrp tSwitchGrp;
								tSwitchGrp.SetSrcChnl(0);
								tSwitchGrp.SetDstMtNum(1);
								tSwitchGrp.SetDstMt(&tMt);
								g_cMpManager.StartSwitchToAll(m_tDoubleStreamSrc, 1, &tSwitchGrp, MODE_SECVIDEO);

								u16 wMtBitRate = m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(),FALSE );
								m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(),wMtBitRate - 10,LOGCHL_SECVIDEO );
								//恢复刚才接收
								RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
								m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(),wMtBitRate,LOGCHL_SECVIDEO );
							}
							else
							{
								if ( TRUE == bIsDoubleUp  )
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
								}
								else
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO);
								}
							}
						}
					}
				}
				//E1 SMCU做双流源，其发送逻辑通道码率需调整[11/6/2012 chendaiwei]
				else if(tMt == m_tDoubleStreamSrc && bIsE1Mcu)
				{
					TLogicalChannel tSecLogicChnnl;
					if (m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tSecLogicChnnl, FALSE))
					{
						tSecLogicChnnl.SetFlowControl(GetDoubleStreamVideoBitrate(wRealMtReqBR,FALSE));
						m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tSecLogicChnnl, FALSE);

						LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMtMcuFlowControlCmd] SMCU<%d> is DS SRC.Adjust RL BR to%d\n",tMt.GetMtId(),GetDoubleStreamVideoBitrate(wRealMtReqBR,FALSE));
					}
				}
				/*else
				{
					RestoreVidSrcBitrate(tMtNull, MODE_SECVIDEO);
				}*/				
			}
		}

		if( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
		{
			if (tMt.GetMtId() == m_tCascadeMMCU.GetMtId())
			{
				OnNtfDsMtStatusToMMcu();
			}
			if( m_tDoubleStreamSrc.GetType() == TYPE_MCUPERI && m_tDoubleStreamSrc.GetEqpType() == EQP_TYPE_RECORDER )
			{
				ConfPrint( LOG_LVL_KEYSTATUS,MID_MCU_MT2," [ProcMtMcuFlowControlCmd]m_tDoubleStreamSrc == EQP_TYPE_RECORDER\n" );
				return;
			}
			if( m_tDoubleStreamSrc == tMt )
			{
				m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), wBitRate, LOGCHL_SECVIDEO );
				ConfPrint( LOG_LVL_KEYSTATUS,MID_MCU_MT2," [ProcMtMcuFlowControlCmd]m_tDoubleStreamSrc == tMt.%d,so not adjust!\n",tMt.GetMtId() );
				return;
			}

			//过滤码率相同的消息
            if (wBitRate == m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), FALSE))
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] rcv snd ds MtReqBitrate :%d from mt<%d>,no change,ignore it.\n",
                    wBitRate, tMt.GetMtId());

				return;
			}
			else if (!m_tDoubleStreamSrc.IsNull() && wBitRate > GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId()), FALSE))
            {
				bIsDoubleUp = TRUE;
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] rcv snd ds MtReqBitrate :%d from mt<%d>, bIsDoubleUp.%d\n",
                    wBitRate, tMt.GetMtId(),bIsDoubleUp);
            }
			else if(!m_tDoubleStreamSrc.IsNull() && wBitRate < GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId()), FALSE))
			{
				bIsDoubleUp = FALSE;
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] rcv snd ds MtReqBitrate :%d from mt<%d>, bIsDoubleUp.%d\n",
                    wBitRate, tMt.GetMtId(),bIsDoubleUp);
			}
			
			if( !m_tDoubleStreamSrc.IsNull() )
			{
				bIsNeedDoubleBrdAdapt = IsNeedAdapt( m_tDoubleStreamSrc,tMt,MODE_SECVIDEO );
			}
			
			m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), wBitRate, LOGCHL_SECVIDEO );
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] rcv snd doublestream .MtReqBitrate :%d from mt<%d>\n",
                    wBitRate, tMt.GetMtId());

            if ( !m_tDoubleStreamSrc.IsNull())
            {
				if(!(tMt == m_tDoubleStreamSrc) )
				{
					TMt tMtNull;
					tMtNull.SetNull();
					if( 0 == m_tConf.GetSecBitRate() )
					{
						if( !bIsNeedDoubleBrdAdapt )
						{
							AdjustMtVideoSrcBR(tMt.GetMtId(),wBitRate,MODE_SECVIDEO,TRUE);						
						}
						else
						{
							if ( TRUE == bIsDoubleUp  )
							{
								RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
							}
							else
							{
								RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO);
							}
						}
					}
					else
					{
						if( !bIsNeedDoubleBrdAdapt )
						{
							if( IsNeedAdapt(m_tDoubleStreamSrc, tMt, MODE_SECVIDEO)/*m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId(), FALSE)*/)
							{							
								if ( TRUE == bIsDoubleUp  )
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
								}
								else
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO);
								}
								StartSwitchToSubMtFromAdp(tMt.GetMtId(), MODE_SECVIDEO);
							}
							else
							{
								ConfPrint( LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMtMcuFlowControlCmd][2]before after all not Sec adapt,it's impossible.mt(%d)\n",tMt.GetMtId() );
							}
						}					
						else
						{
							if( !IsNeedAdapt(m_tDoubleStreamSrc, tMt, MODE_SECVIDEO) )
							{			
								ConfPrint( LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMtMcuFlowControlCmd] before need Sec adapt,after not Sec adapt,rehandle switch.mt(%d)\n",tMt.GetMtId() );
								TSwitchGrp tSwitchGrp;
								tSwitchGrp.SetSrcChnl(0);
								tSwitchGrp.SetDstMtNum(1);
								tSwitchGrp.SetDstMt(&tMt);
								g_cMpManager.StartSwitchToAll(m_tDoubleStreamSrc, 1, &tSwitchGrp, MODE_SECVIDEO);
								
								u16 wMtBitRate = m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(),FALSE );
								m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(),wMtBitRate - 10,LOGCHL_SECVIDEO );
								//恢复刚才接收
								RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
								m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(),wMtBitRate,LOGCHL_SECVIDEO );
							}
							else
							{
								if ( TRUE == bIsDoubleUp  )
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
								}
								else
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(),MODE_SECVIDEO);
								}
							}
						}
					}
				}
            }

		}


		//return;

		
		/*tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );

		tLogicChnnl = *( TLogicalChannel * )( cServMsg.GetMsgBody() );
		wBitRate = tLogicChnnl.GetFlowControl();

        //zbq[02/19/2008] 终端本来是否被降速，用于其作源情况下的发送带宽整理.
        if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
             m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()))
        {
            bMtLowedAlready = TRUE;
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "bMtLowedAlready is %d\n",bMtLowedAlready);
        }

        //E1终端升降速特殊标志 刷新
        if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) )
        {
            u16 wRcvBandWidth = m_ptMtTable->GetRcvBandWidth(tMt.GetMtId());
            // xliang [2/16/2009] 先前的wRcvBandwidth和目前的(wBitRate)比较
			if (wRcvBandWidth < wBitRate)
			{
				m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), FALSE);
			}
			else
			{
				if ( abs(wBitRate-wRcvBandWidth)*100 / wRcvBandWidth > 20 )
				{
					m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), TRUE);
				}
				else
				{
					m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), FALSE);
				}
			}
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuFlowControlCmd] mt.%u bLowed: %d (wRcvBandWidth:%d,wBitRate:%d, conf bitrate:%d)\n",
				tMt.GetMtId(), m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()),
				wRcvBandWidth, wBitRate,m_tConf.GetBitRate());
			
        }
		
		//不理睬其它厂商终端的FlowControl
		if( ( m_ptMtTable->GetManuId( tMt.GetMtId() ) != MT_MANU_KDC && 
		m_ptMtTable->GetManuId( tMt.GetMtId() ) != MT_MANU_KDCMCU ) )
		{
			// xliang [11/14/2008] 单独处理PolycomMCU的flowctrl，对应于双流起停处理
			if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
				( MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
				MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId())))
			{
				ProcPolycomMcuFlowctrlCmd(tMt.GetMtId(), tLogicChnnl);
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcMtMcuFlowControlCmd] PolycomMcu.%d flowctrl received\n", tMt.GetMtId() );
			}
			return;
		}
		
		if (wBitRate > m_tConf.GetBitRate())
		{
			wBitRate = m_tConf.GetBitRate();
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "wBitRate is larger than conf bitrate, modify it equal to conf's bitrate! \n");
		}

        // xsl [8/7/2006] 下级mcu发送的第一路flowcontrol不处理
		if( MODE_SECVIDEO != tLogicChnnl.GetMediaType() &&
			m_ptMtTable->GetManuId( tMt.GetMtId() ) == MT_MANU_KDCMCU )
		{
			if (m_tCascadeMMCU.IsNull() || m_tCascadeMMCU.GetMtId() != tMt.GetMtId())
            {
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] ignore smcu MtReqBitrate :%d from mcu<%d>\n",
                   wBitRate, tMt.GetMtId());
                return;
            }
		}

		//FlowControlToZero,停发
		if( 0 == wBitRate )
		{
			return;
		}

		if( MODE_AUDIO == tLogicChnnl.GetMediaType() )
		{
			return;
		}
				
		//开启双流时，这里获取到的FlowControl的码率已经折半了
		if( MODE_VIDEO == tLogicChnnl.GetMediaType() )
		{
            //zbq[02/01/2008] 降速终端需要整理其带宽
            BOOL32 bMtE1Lowed = FALSE;

            if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
                 m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()))
            {
                bMtE1Lowed = TRUE;
            }
            //过滤码率相同的消息
            if (wBitRate == m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE) ||
                 ( !m_tDoubleStreamSrc.IsNull() && 
                   GetDoubleStreamVideoBitrate(wBitRate) > GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())) &&
                   !bMtE1Lowed) )
            {
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] same bitrate.%d and ReqBitRate.%d for mt.%d!\n", 
                    wBitRate, m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE), tMt.GetMtId());
                return;
            }
            if ( !bMtE1Lowed )
            {
                //当前有双流源要将码率打折
                if ( !m_tDoubleStreamSrc.IsNull() )
                {
                    m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), GetDoubleStreamVideoBitrate(wBitRate, TRUE) , LOGCHL_VIDEO );
                    m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), GetDoubleStreamVideoBitrate(wBitRate, FALSE) , LOGCHL_SECVIDEO );
                    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] rcv fst MtReqBitrate :%d sec MtReqBitrate :%d from mt<%d>\n", 
                        GetDoubleStreamVideoBitrate(wBitRate, TRUE), GetDoubleStreamVideoBitrate(wBitRate, FALSE), tMt.GetMtId());
                }
                else
                {
                    m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), wBitRate, LOGCHL_VIDEO );
                    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] rcv fst MtReqBitrate :%d from mt<%d>\n", 
                        wBitRate, tMt.GetMtId());
                }
                //m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), wBitRate, LOGCHL_VIDEO );
                //ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] rcv fst MtReqBitrate :%d from mt<%d>\n", wBitRate, tMt.GetMtId());

                //zbq[02/19/2008] 本来作选看或发言源的终端带宽升速,需整理其发送带宽
                if ( bMtLowedAlready )
                {
                    LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuFlowControlCmd]MtE1Lowed is %d, MtLowedAlready is %d!\n");
                    //双流
                    if ( tMt == m_tDoubleStreamSrc )
                    {
                        TMt tMtNull;
                        tMtNull.SetNull();
                        RestoreVidSrcBitrate(tMtNull, MODE_SECVIDEO);
                        ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] E1 Lowed Mt restore, adjust DS Src.\n");
                    }
                    //视频选看或发言
                    for( u8 byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx ++)
                    {
                        if ( !m_tConfAllMtInfo.MtJoinedConf(byMtIdx) )
                        {
                            continue;
                        }

                        if ( m_ptMtTable->GetMtSrc(byMtIdx, &tMtSrc, MODE_VIDEO) &&
                             tMt == tMtSrc )
                        {
                            TMt tMtRcv = m_ptMtTable->GetMt(byMtIdx);
                            RestoreVidSrcBitrate(tMtRcv, MODE_VIDEO);
                            ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] E1 Lowed Mt restore, adjust FstVid Src.\n");
                            break;
                        }
                    }
                }
            }
            else
            {
                u16 wRealMtReqBR = 0;

                //zbq[02/01/2008] 不能相信终端带宽，因为其强行取50%. 这里要自己算一下.
                if ( !m_tDoubleStreamSrc.IsNull() )
                {
                    u16 wCurMtReqBR = 0;

                    //开着双流 呼入终端，记录即可
                    if ( bNtfPhyBandWidth )
                    {
                        wCurMtReqBR = m_ptMtTable->GetMtReqBitrate(tMt.GetMtId());
                        wRealMtReqBR = wBitRate;
                    }
                    //双流中 E1终端降速，重新计算, 并整理第一路和第二路的带宽，并通知双流源更新发送带宽
                    else
                    {
                        wCurMtReqBR = m_ptMtTable->GetMtReqBitrate(tMt.GetMtId());
                        wRealMtReqBR = wBitRate * 2;
                    }
                    
                    m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), GetDoubleStreamVideoBitrate(wRealMtReqBR, TRUE) , LOGCHL_VIDEO );
                    m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), GetDoubleStreamVideoBitrate(wRealMtReqBR, FALSE) , LOGCHL_SECVIDEO );
                    
                    //启动调整定时器，防止第二路发送端带宽偏大
                    //SetTimer( MCUVC_SENDFLOWCONTROL_TIMER, 1200 );
                    ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] CurMtReqBR.%d while RealReqBR.%d, adjust for E1MtLowed to Fst.%d, Sec.%d\n",
                            wCurMtReqBR, wRealMtReqBR, 
                            m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), LOGCHL_VIDEO),
                            m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), LOGCHL_SECVIDEO) );                    
                }
                else
                {
                    u16 wCurMtReqBR = m_ptMtTable->GetMtReqBitrate(tMt.GetMtId());
                    m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), wBitRate, LOGCHL_VIDEO );
                    wRealMtReqBR = wBitRate;
                    ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] CurMtReqBR.%d while RealReqBR.%d, adjust for E1MtLowed None DS\n", wCurMtReqBR, wBitRate);
                }
                m_ptMtTable->SetLowedRcvBandWidth(tMt.GetMtId(), wRealMtReqBR);
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] Lowed Mt.%d's LowedRcvBandWidth adjust to.%d\n", tMt.GetMtId(), wRealMtReqBR);
            }

            //zbq[02/18/2008] 不论是否为降速终端的flowctrl，尝试调整Vmp的广播码率
			// xliang [8/4/2009] 在下面同changeVmpBitRate合并
//             if ( m_tConf.m_tStatus.IsBrdstVMP() )
//             {
//                 AdjustVmpBrdBitRate();
//             }

            //zbq[12/11/2008] 从适配接收码流的终端，刷新适配编码
            if (IsNeedAdapt(GetLocalVidBrdSrc(), tMt, MODE_VIDEO))
            {
                RefreshBrdBasParamForSingleMt(tMt.GetMtId(), MODE_VIDEO);
            }
		}
		if( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
		{
            //过滤码率相同的消息
            if (wBitRate == m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), FALSE) ||
                (!m_tDoubleStreamSrc.IsNull() && wBitRate > GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId()), FALSE)))
            {
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] rcv snd MtReqBitrate :%d from mt<%d>, ignore it.\n",
                    wBitRate, tMt.GetMtId());
                return;
            }

			m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), wBitRate, LOGCHL_SECVIDEO );
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] rcv snd MtReqBitrate :%d from mt<%d>\n",
                    wBitRate, tMt.GetMtId());
            
            //zbq[12/11/2008] 从适配接收码流的终端，刷新适配编码
            if (IsNeedAdapt(m_tDoubleStreamSrc, tMt, MODE_SECVIDEO))
            {
                RefreshBrdBasParamForSingleMt(tMt.GetMtId(), MODE_SECVIDEO);
            }
		}
		
		if( MODE_VIDEO == tLogicChnnl.GetMediaType() )
		{
			m_ptMtTable->GetMtSrc( tMt.GetMtId(), &tMtSrc, MODE_VIDEO );
			if(tMtSrc.IsNull())
			{
                //zjj20101111 由于时序问题E1终端被呼入时可能视频源还未赋值所以保护一下				
				tMtSrc = GetLocalVidBrdSrc();				
				if( tMtSrc.IsNull() )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] Mt.%d 's Vid src is Null, ignore it!\n", tMt.GetMtId());
					return;
				}				
			}
			if( tMtSrc == tMt )
			{			
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuFlowControlCmd] Mt.%d 's Vid src is itself, ignore it!\n", tMt.GetMtId());
				return;			
			}
			if( !(tMtSrc == GetLocalVidBrdSrc()) )
			{
				//对于选看的终端，直接转发给相应源
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2,  "[ProcMtMcuFlowControlCmd] SelectSee SrcMtId.%d, DstMtId.%d, wBitRate.%d, ChnnlType.%d\n", 
						tMtSrc.GetMtId(), tMt.GetMtId(), wBitRate, tLogicChnnl.GetMediaType() );

				AdjustMtVideoSrcBR( tMt.GetMtId(), wBitRate, tLogicChnnl.GetMediaType() );
				return;
			}
		}

        //  若为第二通道，则直接发送flowcontrol进行调节
		if( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
		{
			if(!m_tDoubleStreamSrc.IsNull())
			{
                wMinBitRate = GetLeastMtReqBitrate(FALSE, MEDIA_TYPE_NULL, m_tDoubleStreamSrc.GetMtId()); 
                AdjustMtVideoSrcBR( tMt.GetMtId(), wMinBitRate, MODE_SECVIDEO );
				return;
			}
		}

        //modify bas 2 -- 是否需要区分不同格式的最小值
        tDstSimCapSet = m_ptMtTable->GetDstSCS(tMt.GetMtId());
        if (MODE_VIDEO == tLogicChnnl.GetMediaType())
        {
            wMinBitRate = GetLeastMtReqBitrate(TRUE, tDstSimCapSet.GetVideoMediaType());
			if( bMtLowedAlready &&
				wBitRate < wMinBitRate )
			{
				wMinBitRate = wBitRate;
			}
        }        

		//若会议正在广播画面合成图像,降画面合成码率
		// xliang [8/4/2009] 调新接口
        if (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE 
            && m_tConf.m_tStatus.IsBrdstVMP()
			&& MODE_VIDEO == tLogicChnnl.GetMediaType()
			)
        {
            
			AdjustVmpBrdBitRate(&tMt);
            return;
        }

        if (m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tFirstLogicChnnl, TRUE) &&
            m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tSecLogicChnnl, TRUE))
        {
            bStartDoubleStream = TRUE;
        }

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2,  "[ProcMtMcuFlowControlCmd] IsUseAdapter.%d, IsStartDStream.%d, SrcMtId.%d, DstMtId.%d, wBitRate.%d, wMinBitRate.%d, ChnnlType.%d\n", 
			    m_tConf.GetConfAttrb().IsUseAdapter(), bStartDoubleStream, tMtSrc.GetMtId(), tMt.GetMtId(), wBitRate, wMinBitRate, tLogicChnnl.GetMediaType() );

        //不启用适配,直接调整发言人码率
		if( !m_tConf.GetConfAttrb().IsUseAdapter() )
		{
			AdjustMtVideoSrcBR( tMt.GetMtId(), wMinBitRate, tLogicChnnl.GetMediaType() );
			return;
		}
        else
        {
            u16 wAdaptBitRate = 0;            

            //双速会议,启用适配时,
            //双流下，如果新的终端接收速率低于第二速率/2，则禁止该终端的视频转发
            //非双流下，如果新的终端接收速率低于第二速率，则禁止该终端的视频转发
            //否则，则开启转发
            if (0 != m_tConf.GetSecBitRate())
            {			
                u16 wFirstMinBitRate =  m_tConf.GetSecBitRate();
                if (TRUE == bStartDoubleStream && !IsVidBrdSrcNull())
                {
                    wFirstMinBitRate /= 2;
                }
                u16 wFabsBitrate;
                u16 wMainAudioBitrate = GetAudioBitrate(m_tConf.GetMainAudioMediaType());
                u16 wSecAudioBitrate = GetAudioBitrate(m_tConf.GetSecAudioMediaType());
                if (wMainAudioBitrate < wSecAudioBitrate)
                {
                    wFabsBitrate = wSecAudioBitrate - wMainAudioBitrate;
                }
                else
                {
                    wFabsBitrate = wMainAudioBitrate - wSecAudioBitrate;
                }
                if (wBitRate < wFirstMinBitRate - wFabsBitrate)
                {

                    ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuFlowControlCmd] req bitrate < second bitrate, ignore it\n"); 
                    return;
                }            
            }

//            RefreshRcvGrp(tMt.GetMtId());
			switch(tLogicChnnl.GetMediaType())
			{
			case MODE_VIDEO:
				{
					//广播源
					if (!tMtSrc.IsNull())
					{
						if (tMtSrc == GetLocalVidBrdSrc())
						{
							if (IsNeedAdapt(tMtSrc, tMt, MODE_VIDEO))
							{
								if (m_tConf.m_tStatus.IsVidAdapting())
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(), MODE_VIDEO);
									StartSwitchToSubMtFromAdp(tMt.GetMtId(), MODE_VIDEO);
								}
								else
								{
									StartBrdAdapt(MODE_VIDEO);
								}
							}
							else
							{
								AdjustMtVideoSrcBR( tMt.GetMtId(), wMinBitRate, MODE_VIDEO );
							}
						}
						//选看源(稍后)
						else
						{
							
						}
					}					
				}
				break;
			case MODE_AUDIO:
				{
					//广播源
					if (!tMtSrc.IsNull())
					{
						if (tMtSrc == GetLocalAudBrdSrc())
						{
							if (IsNeedAdapt(tMtSrc, tMt, MODE_AUDIO))
							{
								if (m_tConf.m_tStatus.IsAudAdapting())
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(), MODE_AUDIO);
									StartSwitchToSubMtFromAdp(tMt.GetMtId(), MODE_AUDIO);
								}
								else
								{
									StartBrdAdapt(MODE_AUDIO);
								}
							}
						}
						//选看源(稍后)
						else
						{
							
						}
					}				
				}
				break;
			case MODE_SECVIDEO:
				{
					//广播源
					if (!tMtSrc.IsNull())
					{
						if (tMtSrc == GetLocalVidBrdSrc())
						{
							if (IsNeedAdapt(tMtSrc, tMt, MODE_SECVIDEO))
							{
//								StartDSAdapt(tMtSrc, tMt);
								if (m_tConf.m_tStatus.IsVidAdapting())
								{
									RefreshBrdBasParamForSingleMt(tMt.GetMtId(), MODE_SECVIDEO);
									StartSwitchToSubMtFromAdp(tMt.GetMtId(), MODE_SECVIDEO);
								}
								else
								{
									StartBrdAdapt(MODE_SECVIDEO);
								}
							}
						}
					}				
				}
				break;
			default:
				ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[ProcMtMcuFlowControlCmd] unexpected lgc type:%d!\n", tLogicChnnl.GetMediaType());
				break;
			}

        }	*/	
		}	
		break;
		
	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: ProcMtMcuFlowControlCmd() Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

    return;
}
/*=============================================================================
    函 数 名： ProcPolycomMcuFlowctrlCmd
    功    能： 响应处理polycom终端的flowctrl请求
    算法实现： 
    全局变量： 
    参    数： u8 byPolyMCUId
			   TLogicalChannel &tLogicChnnl
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2008/3/31   4.0			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::ProcPolycomMcuFlowctrlCmd(u8 byPolyMCUId, TLogicalChannel &tLogicChnnl)
{	
	if ( MT_MANU_POLYCOM != m_ptMtTable->GetManuId(byPolyMCUId) ||
		 ( MT_TYPE_MMCU != m_ptMtTable->GetMtType(byPolyMCUId) &&
		   MT_TYPE_SMCU != m_ptMtTable->GetMtType(byPolyMCUId)))
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] No polycom MCU.%d deal with, ignore it\n", byPolyMCUId );
		return;
	}

	TMt tPolyMCU = m_ptMtTable->GetMt(byPolyMCUId);
	
	//Polymcu只在刚入会时调一次双流通道
	if ( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
	{
		if ( tPolyMCU == m_tDoubleStreamSrc )
		{
			StopDoubleStream( TRUE, FALSE );
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcPolycomMcuFlowctrlCmd] StopDS(MODE_SECVIDEO)\n" );
		}

		TLogicalChannel tFstLogicChnnl;
		TLogicalChannel tSecLogicChnnl;
		if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstLogicChnnl, TRUE) )
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] get PolyMCU.%d's Fst Fwd chnnl failed\n", byPolyMCUId );
			return;
		}
		if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecLogicChnnl, TRUE) )
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] get PolyMCU.%d's Fst Fwd chnnl failed\n", byPolyMCUId );
			return;
		}

		u16 wSecBR = tLogicChnnl.GetFlowControl();
		tSecLogicChnnl.SetFlowControl(wSecBR);
		u16 wFstBR = m_ptMtTable->GetDialBitrate(byPolyMCUId) - wSecBR;
		tFstLogicChnnl.SetFlowControl(wFstBR);

		m_ptMtTable->SetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstLogicChnnl, TRUE);
		m_ptMtTable->SetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecLogicChnnl, TRUE);

		ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL,  "[ProcPolycomMcuFlowctrlCmd] MODE_SECVIDEO has been ctrled, wBR.%d\n", tLogicChnnl.GetFlowControl() );
	}
	else if ( MODE_VIDEO == tLogicChnnl.GetMediaType() )
	{
		
		//粗略计算
		u16 wDailBR = m_ptMtTable->GetDialBitrate(byPolyMCUId);

		TLogicalChannel tFstLogicChnnl;
		TLogicalChannel tSecLogicChnnl;
		if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstLogicChnnl, TRUE) )
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] get PolyMCU.%d's Fst Fwd chnnl failed(1)\n", byPolyMCUId );
			return;
		}
		if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecLogicChnnl, TRUE) )
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] get PolyMCU.%d's Fst Fwd chnnl failed(1)\n", byPolyMCUId );
			return;
		}

		u16 wFstBR = tLogicChnnl.GetFlowControl();
		tFstLogicChnnl.SetFlowControl(wFstBR);
		
		u16 wSecBR = m_ptMtTable->GetDialBitrate(byPolyMCUId) - wFstBR;
		if ( wSecBR * 100 / wDailBR < 5 )
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] wSecBR.%d, wDailBR.%d, sec BR adjust to 0\n", wSecBR, wDailBR );
			wSecBR = 0;
		}
		tSecLogicChnnl.SetFlowControl(wSecBR);

		m_ptMtTable->SetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstLogicChnnl, TRUE);
		m_ptMtTable->SetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecLogicChnnl, TRUE);

		u16 wBitRate = tLogicChnnl.GetFlowControl();


		// 恢复带宽，停止双流
		if ( abs(wBitRate-(wDailBR-64))*100 / wDailBR < 10 )
		{
			if ( tPolyMCU == m_tDoubleStreamSrc )
			{
				StopDoubleStream( TRUE, FALSE );
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcPolycomMcuFlowctrlCmd] wBR.%d, wDailBR.%d, StopDS\n", wBitRate, wDailBR );
			}
		}
		//开启双流
		else 
		{
			if ( CONF_DUALMODE_EACHMTATWILL == m_tConf.GetConfAttrb().GetDualMode() ||
				 ( CONF_DUALMODE_SPEAKERONLY == m_tConf.GetConfAttrb().GetDualMode() &&
				   tPolyMCU == GetLocalSpeaker() ) )
			{
				//即使当前有双流源，给Polymcu一个高的优先级
				TLogicalChannel tSecVidChnnl;
				if ( m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecVidChnnl, FALSE))
				{
					StartDoubleStream(tPolyMCU, tSecVidChnnl);
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcPolycomMcuFlowctrlCmd] wBR.%d, wDailBR.%d, StartDS\n", wBitRate, wDailBR );
				}
				else
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] wBR.%d, wDailBR.%d, should but no StartDS due to get sec chnnl failed\n", wBitRate, wDailBR );
				}
			}
			else
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcPolycomMcuFlowctrlCmd] wBR.%d, wDailBR.%d, should but no StartDS due to dualmode.%d\n",
								 wBitRate, wDailBR, m_tConf.GetConfAttrb().GetDualMode());
			}
		}
	}
	else
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] unexpected chnnl mode.%d\n", tLogicChnnl.GetMediaType() );
	}

	//本flowctrl调整终端的前向通道，适应于对polymcu的特殊处理，同时调整其后向通道
	TLogicalChannel tFstFwdChnnl;
	TLogicalChannel tFstRcvChnnl;
	TLogicalChannel tSecFwdChnnl;
	TLogicalChannel tSecRcvChnnl;

	if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstFwdChnnl, TRUE) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] polyMcu.%d Fwd VIDEO chnnl failed\n", byPolyMCUId );
		return;
	}
	if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstRcvChnnl, FALSE) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] polyMcu.%d Rcv VIDEO chnnl failed\n", byPolyMCUId );
		return;
	}
	if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecFwdChnnl, TRUE) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] polyMcu.%d Fwd LOGCHL_SECVIDEO chnnl failed\n", byPolyMCUId );
		return;
	}
	if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecRcvChnnl, FALSE) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcPolycomMcuFlowctrlCmd] polyMcu.%d Rcv LOGCHL_SECVIDEO chnnl failed\n", byPolyMCUId );
		return;
	}

	u16 wFstRcvBR = tFstRcvChnnl.GetFlowControl();
	u16 wSecRcvBR = tSecRcvChnnl.GetFlowControl();

	tFstRcvChnnl.SetFlowControl(tFstFwdChnnl.GetFlowControl());
	tSecRcvChnnl.SetFlowControl(tSecFwdChnnl.GetFlowControl());

	m_ptMtTable->SetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstRcvChnnl, FALSE);
	m_ptMtTable->SetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecRcvChnnl, FALSE);

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcPolycomMcuFlowctrlCmd] polyMcu.%d FstRcvBR adjust from.%d to %d, SecRcvBR adjust from.%d to %d\n",
			  byPolyMCUId, 
			  wFstRcvBR, tFstRcvChnnl.GetFlowControl(),
			  wSecRcvBR, tSecRcvChnnl.GetFlowControl());
	return;
}
/*=============================================================================
    函 数 名： ProcMtMcuFlowControlIndication
    功    能： 终端要求改变发送码率 - 码流控制指令（针对sony G70SP活动双流）
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/21   3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::ProcMtMcuFlowControlIndication( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt;
	TLogicalChannel	tLogicChnnl;
	TLogicalChannel tH239LogicChnnl;
	u16 wBitRate = 0;

	switch( CurState() )
	{
	case STATE_ONGOING:

		tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );

		tLogicChnnl = *( TLogicalChannel * )( cServMsg.GetMsgBody() );
		wBitRate = tLogicChnnl.GetFlowControl();

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2,  "[Mt2McuFlowctrlInd] MtId.%d, ManuId.%d, DStreamMtId.%d, wBitRate.%d, ChnnlType.%d\n", 
				tMt.GetMtId(), m_ptMtTable->GetManuId( tMt.GetMtId() ), 
				m_tDoubleStreamSrc.GetMtId(), wBitRate, tLogicChnnl.GetMediaType() );
		
		// xliang [11/14/2008] 增加处理对polycom的指示响应
		if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId( tMt.GetMtId() ) )
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcMtMcuFlowControlIndication] PolycomMt.%d Ind ChnType.%d BR.%d\n",
				tMt.GetMtId(), tLogicChnnl.GetMediaType(), wBitRate );
			return;
		}
		//sony G70SP在H263-H239时采用活动双流,使用FlowControlIndication进行发送激活或关闭
		//不理睬其他情况下的FlowControlIndication
		if( MT_MANU_SONY != m_ptMtTable->GetManuId( tMt.GetMtId() ) || 
			MEDIA_TYPE_H263 != m_tConf.GetCapSupport().GetDStreamMediaType() || 
			!m_tConf.GetCapSupport().IsDStreamSupportH239() ||
			MODE_SECVIDEO != tLogicChnnl.GetMediaType() ||  
			!m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tH239LogicChnnl, FALSE ) )
		{
			return;
		}

		//FlowControlIndication To Zero, Stop Active DStream
		if( 0 == wBitRate )
		{
			if( m_tDoubleStreamSrc == tMt )
			{
				StopDoubleStream( FALSE, FALSE );
			}
		}
		else
		{
			//判断双流的发起源
			if ( CONF_DUALMODE_EACHMTATWILL == m_tConf.GetConfAttrb().GetDualMode() ||
				 (CONF_DUALMODE_SPEAKERONLY == m_tConf.GetConfAttrb().GetDualMode() && tMt == GetLocalSpeaker()) ) 
			{
                // xsl [7/20/2006]卫星分散会议时需要考虑回传通道数
                if (m_tConf.GetConfAttrb().IsSatDCastMode()/* && IsOverSatCastChnnlNum(tMt.GetMtId())*/
					&& IsMultiCastMt(tMt.GetMtId())
					&& //IsSatMtOverConfDCastNum(tMt))
					!IsSatMtCanContinue(tMt.GetMtId(),emstartDs))
                {
                    ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[Mt2McuFlowctrlInd] over max upload mt num.\n");
                    return;
                }
               
				//活动双流时，不检测令牌有效性
				//UpdateH239TokenOwnerInfo( tMt );
				StartDoubleStream( tMt, tH239LogicChnnl );
			}
			else
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2,  "[Mt2McuFlowctrlInd] MtId.%d, ManuId.%d, DStreamMtId.%d, wBitRate.%d, ChnnlType.%d open double stream with wrong DualMode, NACK !\n", 
					tMt.GetMtId(), m_ptMtTable->GetManuId( tMt.GetMtId() ), m_tDoubleStreamSrc.GetMtId(), wBitRate, tLogicChnnl.GetMediaType() );
			}
		}
		break;
		
	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[Mt2McuFlowctrlInd] Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：HasJoinedSubMtNeedAdapt
    功能        ：会议中是否有直连入会终端需要码率适配
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：TRUE/FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/18    3.0         胡昌威          创建
====================================================================*/
/*u8   CMcuVcInst::GetNeedAdaptMtNum( u8 byAdaptType )
{
	u8 byMtNum = 0;
	u8	byLoop;

	for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
		{
			if( IsMtNeedAdapt(byAdaptType, byLoop) )
			{
				byMtNum++;
			}
		}
	}
	
	return byMtNum;
}*/
/*====================================================================
    函数名      :GetMinMtRcvBitByVmpChn
    功能        ：得到会终端接收到VMP输出的最小要求码率
    算法实现    ：
    引用全局变量：
    输入参数说明：BOOL32 bPrimary [IN]主视频信道标志，用来得到终端接收的
                     主视频信道码率标志
                  u8 byVmpOutChnnl[IN]Vmp输出某通道号
                  u8 bySrcMtId    [IN]源MtId，这里暂时没有，仅用作遍历
                     会议终端列表时不从零开始
    返回值说明  ：码率
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    11/05/30    4.6         宋锟         创建
====================================================================*/
u16 CMcuVcInst::GetMinMtRcvBitByVmpChn(u8 byVmpId, BOOL32 bPrimary,u8 byVmpOutChnnl,u8 bySrcMtId /* = 0*/)
{
    u16 wMinBitRate = m_tConf.GetBitRate();
    u16 wMtBitRate;
    u8  byScale = 100;
    
	if (!IsValidVmpId(byVmpId))
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP,"[GetMinMtRcvBitByVmpChn]VmpId：%d is wrong \n",byVmpId );
		return wMinBitRate;
	}
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
    // xsl [8/7/2006] 若回传通道内有终端，则mcu接收码率不进行比较(因为mcu接收码率由回传终端发送码率决定)
    // 保证上级升降速时，不影响下级广播源码率
    BOOL32 bMMcuSpyMtOprSkip = FALSE;
    if( !m_tCascadeMMCU.IsNull() )
    {
        TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
        // guzh [3/26/2007] 这里的限制是为了处理如下的问题：
        // 当上级MCU发给 本级的回传终端 m_tSpyMt flowcontrol 命令时
        // 一般是需要处理的，这样才能保证有双流情况下下级回传的第二路码率合适（双流不适配）
        // 但是这样可以会导致上级升降速时，同时在上下级发言的该终端，会影响下级广播源码率也同时下降
        // 所以目前的策略是：如果有双流的情况，或者只要发言人不是回传终端，就不会跳过该MMCU。
        // 否则就跳过(bMMcuSpyMtOprSkip=TRUE)
        if ( ptConfMcInfo != NULL && 
            !ptConfMcInfo->m_tSpyMt.IsNull() && 
            bySrcMtId == ptConfMcInfo->m_tSpyMt.GetMtId() )
        {
            if ( !m_tDoubleStreamSrc.IsNull())
                bMMcuSpyMtOprSkip = FALSE;
            else if ( !HasJoinedSpeaker() || GetLocalSpeaker().GetMtId() != ptConfMcInfo->m_tSpyMt.GetMtId() ) 
                bMMcuSpyMtOprSkip = FALSE;
            else
                bMMcuSpyMtOprSkip = TRUE;
        }
    }

	//调整vmp到终端的交换
	TLogicalChannel tFwLogicChnnl;
	TMtStatus tMtStatus;
	TMt tSelectSrc;
	BOOL32 bIsVMPBrdst = FALSE;
	TPeriEqpStatus tPeriEqpStatus; 
	u8 byMtRecVmpChnnl;
	g_cMcuVcApp.GetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	if (tPeriEqpStatus.m_tStatus.tVmp.GetVmpParam().IsVMPBrdst() || 
		g_cMcuVcApp.IsBrdstVMP(tVmpEqp))
	{
		bIsVMPBrdst = TRUE;
	}
	for(u8 byMtLoop = 0; byMtLoop < MAXNUM_CONF_MT; byMtLoop ++)
	{
	    if (m_tConfAllMtInfo.MtJoinedConf(byMtLoop) && bySrcMtId != byMtLoop)
        {
			tFwLogicChnnl.Clear();
			tMtStatus.Clear();
			if (m_tCascadeMMCU.GetMtId() == byMtLoop && bMMcuSpyMtOprSkip )
            {
				continue;
            }
			if (!m_ptMtTable->GetMtLogicChnnl( byMtLoop,LOGCHL_VIDEO,&tFwLogicChnnl,TRUE ) )
			{
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP,"[GetMinMtRcvBitByVmpChn]Get DstMt：%d forward logicalchnnl failed \n",byMtLoop );
				continue;
			}        
			// vmp非广播状态下,跳过选看源不是vmp的终端
			if( !bIsVMPBrdst )
			{
				m_ptMtTable->GetMtStatus(byMtLoop, &tMtStatus);
				tSelectSrc = tMtStatus.GetSelectMt(MODE_VIDEO);
				// 选看源不是外设vmp时,跳过统计
				if (!(TYPE_MCUPERI == tSelectSrc.GetType() && EQP_TYPE_VMP == tSelectSrc.GetEqpType() && tSelectSrc.GetEqpId() == tVmpEqp.GetEqpId()))
				{
					continue;
				}
			}

      	    byMtRecVmpChnnl  = GetVmpOutChnnlByRes(tVmpEqp.GetEqpId(),
				tFwLogicChnnl.GetVideoFormat(),
				tFwLogicChnnl.GetChannelType(),
				tFwLogicChnnl.GetChanVidFPS(), 
				tFwLogicChnnl.GetProfileAttrb());
			
            if (byMtRecVmpChnnl != byVmpOutChnnl )
			{
				continue;
			}           
	
			wMtBitRate = m_ptMtTable->GetMtReqBitrate(byMtLoop, bPrimary);
					
			if (wMtBitRate != 0 && wMtBitRate < wMinBitRate)
			{
				wMinBitRate = wMtBitRate;
			}
		}
	}
       

    if (0 != m_tConf.GetSecBitRate() && bIsVMPBrdst)   //双速会议
    {
        // guzh [7/10/2007] 低速录像需要考虑是否从BrBas录        
        BOOL32 bSecRateDirectly = FALSE;

        if (m_tConf.GetConfAttrb().IsMulticastMode() && m_tConf.GetConfAttrb().IsMulcastLowStream())
        {
			if ( !m_tVmpEqp.IsNull() && g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus ))
			{
				if (tPeriEqpStatus.m_tStatus.tVmp.GetVmpParam().IsVMPBrdst())
				{
					TVideoStreamCap tStrCap = m_tConf.GetMainSimCapSet().GetVideoCap();
					u8 byChnnl4MultiCast = GetVmpOutChnnlByRes (m_tVmpEqp.GetEqpId(),
						tStrCap.GetResolution(), 
						tStrCap.GetMediaType(),
						tStrCap.GetUserDefFrameRate(),
						tStrCap.GetH264ProfileAttrb());
					if (  byVmpOutChnnl == byChnnl4MultiCast )
					{
						bSecRateDirectly = TRUE;
					}
				}
				
			}

        }

        if (!bSecRateDirectly && m_tRecPara.IsRecLowStream())
        {
			TMt tRecSrc;
            u8 byRecChnlIdx = 0;
			u8 byBasChnId   = 0;
            // guzh [7/27/2007] 需要区分获取的主格式还是非主格式的最低码率（因为认为永远录主格式）
            if (IsRecordSrcBas(MODE_VIDEO, tRecSrc, byBasChnId, byRecChnlIdx))
            {
                {
                    bSecRateDirectly = TRUE;
                }
            } //录像机低速录像，编码码流用会议的最低速
			else if (!m_tConf.m_tStatus.IsNoRecording())
			{
				TVideoStreamCap tStrCap = m_tConf.GetMainSimCapSet().GetVideoCap();
				u8 byChnnl = GetVmpOutChnnlByRes (m_tVmpEqp.GetEqpId(),
					tStrCap.GetResolution(), 
					tStrCap.GetMediaType(),
					tStrCap.GetUserDefFrameRate(),
					tStrCap.GetH264ProfileAttrb());
				if (byVmpOutChnnl == byChnnl)
				{
					bSecRateDirectly = TRUE;
				}
			}
        }

        if (bSecRateDirectly)
        {
            wMinBitRate = m_tConf.GetSecBitRate()*byScale/100;
        }
        else
        {
			const u16 wSecBitrate = m_tConf.GetSecBitRate()*byScale/100;
            wMinBitRate = kmax(wMinBitRate, wSecBitrate);
        }
    }

    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[GetLeastMtReqBitrate] Bitrate.%d, primary.%d, chnl.%d\n", 
            wMinBitRate, bPrimary, byVmpOutChnnl);

    return wMinBitRate;
}
/*====================================================================
    函数名      GetLstRcvMediaResMtBr
    功能        ：得到某种类型某种分辨率接收终端中最小要求码率
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：码率
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/08/04    4.6         薛亮          创建
====================================================================*/
u16 CMcuVcInst::GetLstRcvMediaResMtBr(BOOL32 bPrimary, u8 byMediaType, u8 byRes, u8 bySrcMtId /* = 0*/)
{
    u16 wMinBitRate = m_tConf.GetBitRate();
    u16 wMtBitRate;
	//    u8	byLoop;
    u8  byScale = 100;
    
    // xsl [8/7/2006] 若回传通道内有终端，则mcu接收码率不进行比较(因为mcu接收码率由回传终端发送码率决定)
    // 保证上级升降速时，不影响下级广播源码率
    BOOL32 bMMcuSpyMtOprSkip = FALSE;
    if( !m_tCascadeMMCU.IsNull() )
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
        // guzh [3/26/2007] 这里的限制是为了处理如下的问题：
        // 当上级MCU发给 本级的回传终端 m_tSpyMt flowcontrol 命令时
        // 一般是需要处理的，这样才能保证有双流情况下下级回传的第二路码率合适（双流不适配）
        // 但是这样可以会导致上级升降速时，同时在上下级发言的该终端，会影响下级广播源码率也同时下降
        // 所以目前的策略是：如果有双流的情况，或者只要发言人不是回传终端，就不会跳过该MMCU。
        // 否则就跳过(bMMcuSpyMtOprSkip=TRUE)
		if ( ptConfMcInfo != NULL && 
			!ptConfMcInfo->m_tSpyMt.IsNull() && 
			bySrcMtId == ptConfMcInfo->m_tSpyMt.GetMtId() )
        {
            if ( !m_tDoubleStreamSrc.IsNull())
                bMMcuSpyMtOprSkip = FALSE;
            else if ( !HasJoinedSpeaker() || GetLocalSpeaker().GetMtId() != ptConfMcInfo->m_tSpyMt.GetMtId() ) 
                bMMcuSpyMtOprSkip = FALSE;
            else
                bMMcuSpyMtOprSkip = TRUE;
        }
    }
	
    if (!m_tDoubleStreamSrc.IsNull())
    {
        if (bPrimary)
        {
            byScale = 100 - m_tConf.GetDStreamScale();       
        }
        else
        {
            byScale = m_tConf.GetDStreamScale();        
        }
    }
    wMinBitRate = wMinBitRate*byScale/100;
	
	
	//zhouyiliang 20101222 调整vmp到终端的交换
	//	u8  byNum = 0;
	//	u8	abyMtId[MAXNUM_CONF_MT] = { 0 };
	//m_cMtRcvGrp.GetMtMediaRes(tMt.GetMtId(), byMVType, byRes);
	
	//m_cMtRcvGrp.GetMVMtList(byMediaType, byRes, byNum, abyMtId, TRUE);//强制取接收MT列表，忽略免适配区
	
	//	if(byNum > 0)
	//	{
	for(u8 byMtLoop = 0; byMtLoop < MAXNUM_CONF_MT; byMtLoop ++)
	{
		//byMtLoop = abyMtId[byLoop];
		
		if (m_tConfAllMtInfo.MtJoinedConf(byMtLoop) && bySrcMtId != byMtLoop)
		{
			if (m_tCascadeMMCU.GetMtId() == byMtLoop && bMMcuSpyMtOprSkip )
				continue;
			TLogicalChannel tFwLogicChnnl;
			if (!m_ptMtTable->GetMtLogicChnnl( byMtLoop,LOGCHL_VIDEO,&tFwLogicChnnl,TRUE ) )
			{
				continue;
			}
			if (byMediaType != tFwLogicChnnl.GetChannelType() || byRes != tFwLogicChnnl.GetVideoFormat() )
			{
				continue;
			}
			//				tMtDstSCS = m_ptMtTable->GetDstSCS(byMtLoop);
			//				if (MEDIA_TYPE_NULL == byMediaType || tMtDstSCS.GetVideoMediaType() == byMediaType)
			//				{
			//zbq[07/28/2009] 双流flowctrl修正：不响应非免适配终端的flowctrl，AdpParam自然调整
			//					if (bPrimary &&
			//						m_cMtRcvGrp.IsMtNeedAdp(byMtLoop))
			//					{
			//						continue;
			//					}
			//					if (!bPrimary &&
			//						m_cMtRcvGrp.IsMtNeedAdp(byMtLoop, FALSE))
			//					{
			//						continue;
			//					}
			
			wMtBitRate = m_ptMtTable->GetMtReqBitrate(byMtLoop, bPrimary);
			
			if (wMtBitRate != 0 && wMtBitRate < wMinBitRate)
			{
				wMinBitRate = wMtBitRate;
			}
			//				}
		}
	}
	//    }       
	
    if (0 != m_tConf.GetSecBitRate())   //双速会议
    {
        // guzh [7/10/2007] 低速录像需要考虑是否从BrBas录        
        BOOL32 bSecRateDirectly = FALSE;
		
        if (m_tConf.GetConfAttrb().IsMulticastMode() && m_tConf.GetConfAttrb().IsMulcastLowStream())
        {
		/*
		#ifdef _MINIMCU_
		// guzh [8/29/2007] 双媒体会议，区分从BrBas或VidBas组播
		if ( !ISTRUE(m_byIsDoubleMediaConf) )
		{
		bSecRateDirectly = TRUE;
		}
		else if ( MEDIA_TYPE_NULL == byMediaType ||
		( MEDIA_TYPE_NULL != byMediaType &&
		m_tConf.GetSecVideoMediaType() == byMediaType ) 
		) 
		{
		bSecRateDirectly = TRUE;
		}
		#else
			*/
			// [5/9/2011 xliang] 组播地址收的那出码流跟主格式主分辨率终端接收的那出应该是相同的，
			// 并不能认为收的是主格式主分辨率的那出，因为有的VMP根本编不出那出来。eg:
			// conf: 1080, 720, 8kg VMP can only output 720, in that case, 720 also multicast.
			// 			if ( byMediaType == m_tConf.GetMainVideoMediaType() 
			// 				&& byRes == m_tConf.GetMainVideoFormat() )
			// 			{
			// 				bSecRateDirectly = TRUE;
			// 			}
			
			TPeriEqpStatus tPeriEqpStatus;
			if ( !m_tVmpEqp.IsNull() && g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus ))
			{
				if (tPeriEqpStatus.m_tStatus.tVmp.GetVmpParam().IsVMPBrdst())
				{
					u8 byChnnl = GetVmpOutChnnlByRes(m_tVmpEqp.GetEqpId(), byRes, byMediaType); //此函数未被用到
					TVideoStreamCap tStrCap = m_tConf.GetMainSimCapSet().GetVideoCap();
					u8 byChnnl4MultiCast = GetVmpOutChnnlByRes (m_tVmpEqp.GetEqpId(),
						tStrCap.GetResolution(), 
						tStrCap.GetMediaType(),
						tStrCap.GetUserDefFrameRate(),
						tStrCap.GetH264ProfileAttrb());
					if ( byChnnl == byChnnl4MultiCast )
					{
						bSecRateDirectly = TRUE;
					}
				}
				
			}
            // 双媒体会议不支持低速组播
            //bSecRateDirectly = TRUE;
			/*
			#endif
			*/
        }
		
        if (!bSecRateDirectly && m_tRecPara.IsRecLowStream())
        {
			TMt tRecSrc;
            u8 byRecChnlIdx = 0;
			u8 byBasChnId   = 0;
            // guzh [7/27/2007] 需要区分获取的主格式还是非主格式的最低码率（因为认为永远录主格式）
            if (IsRecordSrcBas(MODE_VIDEO, tRecSrc, byBasChnId, byRecChnlIdx))
            {
				//                 if ( ( tRecSrc == m_tBrBasEqp &&
				//                        ( MEDIA_TYPE_NULL == byMediaType || m_tConf.GetMainVideoMediaType() == byMediaType ) )
				//                    || ( tRecSrc == m_tVidBasEqp &&
				//                         MEDIA_TYPE_NULL != byMediaType &&
				//                         m_tConf.GetSecVideoMediaType() == byMediaType )
				//                    )
                {
                    bSecRateDirectly = TRUE;
                }
            }
        }
		
        if (bSecRateDirectly)
        {
            wMinBitRate = m_tConf.GetSecBitRate()*byScale/100;
        }
        else
        {
			const u16 wSecBitrate = m_tConf.GetSecBitRate()*byScale/100;
            wMinBitRate = kmax(wMinBitRate, wSecBitrate);
        }
    }
	
    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[GetLeastMtReqBitrate] Bitrate.%d, primary.%d, media.%d\n", 
		wMinBitRate, bPrimary, byMediaType);
	
    return wMinBitRate;
}
/*====================================================================
    函数名      ：GetLeastMtReqBitrate
    功能        ：得到最小的终端要求码率
    算法实现    ：
    引用全局变量：
    输入参数说明：BOOL32 bPrimary = TRUE
    返回值说明  ：码率
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/05/21    3.0         胡昌威          创建
====================================================================*/
//modify bas 2
u16 CMcuVcInst::GetLeastMtReqBitrate(BOOL32 bPrimary, u8 byMediaType, u8 bySrcMtId)
{
    u16 wMinBitRate = m_tConf.GetBitRate();
    u16 wMtBitRate;
    u8	byLoop;
    TSimCapSet tMtDstSCS;
    u8  byScale = 100;
	TMt tSrcMt = m_ptMtTable->GetMt( bySrcMtId );
    
    // xsl [8/7/2006] 若回传通道内有终端，则mcu接收码率不进行比较(因为mcu接收码率由回传终端发送码率决定)
    // 保证上级升降速时，不影响下级广播源码率
    BOOL32 bMMcuSpyMtOprSkip = FALSE;
    if( !m_tCascadeMMCU.IsNull() )
	{
		u16 wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() ); 		
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo( wMMcuIdx );//m_tCascadeMMCU.GetMtId());
        // guzh [3/26/2007] 这里的限制是为了处理如下的问题：
        // 当上级MCU发给 本级的回传终端 m_tSpyMt flowcontrol 命令时
        // 一般是需要处理的，这样才能保证有双流情况下下级回传的第二路码率合适（双流不适配）
        // 但是这样可以会导致上级升降速时，同时在上下级发言的该终端，会影响下级广播源码率也同时下降
        // 所以目前的策略是：如果有双流的情况，或者只要发言人不是回传终端，就不会跳过该MMCU。
        // 否则就跳过(bMMcuSpyMtOprSkip=TRUE)
		if ( ptConfMcInfo != NULL && 
             !ptConfMcInfo->m_tSpyMt.IsNull() && 
             bySrcMtId == ptConfMcInfo->m_tSpyMt.GetMtId() )
        {
            if ( !m_tDoubleStreamSrc.IsNull())
                bMMcuSpyMtOprSkip = FALSE;
            else if ( !HasJoinedSpeaker() || GetLocalSpeaker().GetMtId() != ptConfMcInfo->m_tSpyMt.GetMtId() ) 
                bMMcuSpyMtOprSkip = FALSE;
            else
                bMMcuSpyMtOprSkip = TRUE;
        }
    }

    if (!m_tDoubleStreamSrc.IsNull() || !m_tSecVidBrdSrc.IsNull())
    {
        if (bPrimary)
        {
            byScale = 100 - m_tConf.GetDStreamScale();       
        }
        else
        {
            byScale = m_tConf.GetDStreamScale();        
        }

		//先乘积计算双流码率，在用减法算出主流码流。与GetDoubleStreamVideoBitrate函数保持一致
		//避免由于小数点造成1bitrate的误差[9/17/2012 chendaiwei]
		//wMinBitRate = wMinBitRate - wMinBitRate*m_tConf.GetDStreamScale()/100;
    }

    wMinBitRate = wMinBitRate*byScale/100;

    for (byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        if (m_tConfAllMtInfo.MtJoinedConf(byLoop) && bySrcMtId != byLoop)
        {
            if (m_tCascadeMMCU.GetMtId() == byLoop && bMMcuSpyMtOprSkip )
                continue;
            
            tMtDstSCS = m_ptMtTable->GetDstSCS(byLoop);
            if (MEDIA_TYPE_NULL == byMediaType || tMtDstSCS.GetVideoMediaType() == byMediaType)
            {
				if (bPrimary && IsNeedAdapt(GetVidBrdSrc(), m_ptMtTable->GetMt(byLoop), MODE_VIDEO))
				{
					continue;
				}

				if (!bPrimary && !m_tDoubleStreamSrc.IsNull()
					&& IsNeedAdapt(m_tDoubleStreamSrc, m_ptMtTable->GetMt(byLoop), MODE_SECVIDEO))
				{
					continue;
				}
				
				if (!bPrimary && !GetSecVidBrdSrc().IsNull()
					&& IsNeedAdapt(GetSecVidBrdSrc(), m_ptMtTable->GetMt(byLoop), MODE_VIDEO2SECOND))
				{
					continue;
				}

                wMtBitRate = m_ptMtTable->GetMtReqBitrate(byLoop, bPrimary);

				//zjj20120511 E1升降速策略调整,注去
				/*if ( m_ptMtTable->GetMtTransE1(byLoop) &&
						m_ptMtTable->GetMtBRBeLowed(byLoop) && bPrimary &&
						tSrcMt == GetLocalVidBrdSrc()
						)
				{					
					wMinBitRate = m_ptMtTable->GetLowedRcvBandWidth( byLoop );

					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[GetLeastMtReqBitrate] mt.%d is Transport by E1 Lowed Bitrate.%d\n",
							byLoop,wMinBitRate );
				}*/
				
                if (wMtBitRate != 0 && wMtBitRate < wMinBitRate)
                {
                    wMinBitRate = wMtBitRate;
                }
            }
        }
    }       

    if (0 != m_tConf.GetSecBitRate())   //双速会议
    {
        // guzh [7/10/2007] 低速录像需要考虑是否从BrBas录        
        BOOL32 bSecRateDirectly = FALSE;

        if (m_tConf.GetConfAttrb().IsMulticastMode() && m_tConf.GetConfAttrb().IsMulcastLowStream())
        {
/*
#ifdef _MINIMCU_
            // guzh [8/29/2007] 双媒体会议，区分从BrBas或VidBas组播
            if ( !ISTRUE(m_byIsDoubleMediaConf) )
            {
                bSecRateDirectly = TRUE;
             }
            else if ( MEDIA_TYPE_NULL == byMediaType ||
                      ( MEDIA_TYPE_NULL != byMediaType &&
                        m_tConf.GetSecVideoMediaType() == byMediaType ) 
                    ) 
            {
                bSecRateDirectly = TRUE;
            }
#else
*/
            // 双媒体会议不支持低速组播
            bSecRateDirectly = TRUE;
/*
#endif
*/
        }

        if (!bSecRateDirectly && m_tRecPara.IsRecLowStream())
        {
			TMt tRecSrc;
            u8  byRecChnlIdx = 0;
			u8  byBasChnId	 = 0;
            // guzh [7/27/2007] 需要区分获取的主格式还是非主格式的最低码率（因为认为永远录主格式）
            if (IsRecordSrcBas(MODE_VIDEO, tRecSrc, byBasChnId, byRecChnlIdx))
            {
//                 if ( ( tRecSrc == m_tBrBasEqp &&
//                        ( MEDIA_TYPE_NULL == byMediaType || m_tConf.GetMainVideoMediaType() == byMediaType ) )
//                    || ( tRecSrc == m_tVidBasEqp &&
//                         MEDIA_TYPE_NULL != byMediaType &&
//                         m_tConf.GetSecVideoMediaType() == byMediaType )
//                    )
                {
                    bSecRateDirectly = TRUE;
                }
            }
        }

        if (bSecRateDirectly)
        {
            wMinBitRate = m_tConf.GetSecBitRate()*byScale/100;
        }
        else
        {
			const u16 wSecBitrate = m_tConf.GetSecBitRate()*byScale/100;
            wMinBitRate = kmax(wMinBitRate, wSecBitrate);
        }
    }

    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[GetLeastMtReqBitrate] Bitrate.%d, primary.%d, media.%d\n", 
            wMinBitRate, bPrimary, byMediaType);

    return wMinBitRate;
}

/*====================================================================
    函数名      ：GetLeastMtReqBR
    功能        ：得到最小的终端要求码率
    算法实现    ：
    引用全局变量：
    输入参数说明：BOOL32      bPrimary
                  TSimCapSet &tSrcSCS 
                  u8          bySrtMtId
    返回值说明  ：码率
                  目前暂不支持双流的本类比对获取
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    10/10/2008  4.5         张宝卿          创建
====================================================================*/
u16 CMcuVcInst::GetLeastMtReqBR(TSimCapSet &tSrcSCS, 
                                u8          bySrcMtId /* = 0 */,
                                BOOL32      bPrimary  /* = TRUE */)
{
    u16 wMinBitRate = m_tConf.GetBitRate();
    u16 wMtBitRate;
    u8	byLoop;
    TSimCapSet tMtDstSCS;
    u8  byScale = 100;

    u8 byMediaType = tSrcSCS.GetVideoMediaType();
    u8 byMediaRes = tSrcSCS.GetVideoResolution();
    
    // xsl [8/7/2006] 若回传通道内有终端，则mcu接收码率不进行比较(因为mcu接收码率由回传终端发送码率决定)
    // 保证上级升降速时，不影响下级广播源码率
    BOOL32 bMMcuSpyMtOprSkip = FALSE;
    if( !m_tCascadeMMCU.IsNull() )
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
        // guzh [3/26/2007] 这里的限制是为了处理如下的问题：
        // 当上级MCU发给 本级的回传终端 m_tSpyMt flowcontrol 命令时
        // 一般是需要处理的，这样才能保证有双流情况下下级回传的第二路码率合适（双流不适配）
        // 但是这样可以会导致上级升降速时，同时在上下级发言的该终端，会影响下级广播源码率也同时下降
        // 所以目前的策略是：如果有双流的情况，或者只要发言人不是回传终端，就不会跳过该MMCU。
        // 否则就跳过(bMMcuSpyMtOprSkip=TRUE)
		if ( ptConfMcInfo != NULL && 
             !ptConfMcInfo->m_tSpyMt.IsNull() && 
             bySrcMtId == ptConfMcInfo->m_tSpyMt.GetMtId() )
        {
            if ( !m_tDoubleStreamSrc.IsNull())
                bMMcuSpyMtOprSkip = FALSE;
            else if ( !HasJoinedSpeaker() || GetLocalSpeaker().GetMtId() != ptConfMcInfo->m_tSpyMt.GetMtId() ) 
                bMMcuSpyMtOprSkip = FALSE;
            else
                bMMcuSpyMtOprSkip = TRUE;
        }
    }

    if (!m_tDoubleStreamSrc.IsNull())
    {
        if (bPrimary)
        {
            byScale = 100 - m_tConf.GetDStreamScale();       
        }
        else
        {
            byScale = m_tConf.GetDStreamScale();        
        }
    }
    wMinBitRate = wMinBitRate*byScale/100;

    for (byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        if (m_tConfAllMtInfo.MtJoinedConf(byLoop) && bySrcMtId != byLoop)
        {
            if (m_tCascadeMMCU.GetMtId() == byLoop && bMMcuSpyMtOprSkip )
                continue;
            
            tMtDstSCS = m_ptMtTable->GetDstSCS(byLoop);
            if (tMtDstSCS.GetVideoMediaType() == byMediaType &&
                tMtDstSCS.GetVideoResolution() == byMediaRes)
            {
                wMtBitRate = m_ptMtTable->GetMtReqBitrate(byLoop, bPrimary);
                if (wMtBitRate != 0 && wMtBitRate < wMinBitRate)
                {
                    wMinBitRate = wMtBitRate;
                }
            }
        }
    }       

    if (0 != m_tConf.GetSecBitRate())   //双速会议
    {
        // guzh [7/10/2007] 低速录像需要考虑是否从BrBas录        
        BOOL32 bSecRateDirectly = FALSE;

        if (m_tConf.GetConfAttrb().IsMulticastMode() && m_tConf.GetConfAttrb().IsMulcastLowStream())
        {
/*
#ifdef _MINIMCU_
            // guzh [8/29/2007] 双媒体会议，区分从BrBas或VidBas组播
            if ( !ISTRUE(m_byIsDoubleMediaConf) )
            {
                bSecRateDirectly = TRUE;
             }
            else if ( MEDIA_TYPE_NULL == byMediaType ||
                      ( MEDIA_TYPE_NULL != byMediaType &&
                        m_tConf.GetSecVideoMediaType() == byMediaType ) 
                    ) 
            {
                bSecRateDirectly = TRUE;
            }
#else
*/
            // 双媒体会议不支持低速组播
            bSecRateDirectly = TRUE;
/*
#endif
*/
        }

        if (!bSecRateDirectly && m_tRecPara.IsRecLowStream())
        {
            TMt tRecSrc;
            u8 byRecChnlIdx = 0;
			u8 byBasChnId   = 0;
            // guzh [7/27/2007] 需要区分获取的主格式还是非主格式的最低码率（因为认为永远录主格式）
            if (IsRecordSrcBas(MODE_VIDEO, tRecSrc, byBasChnId, byRecChnlIdx))
            {
//                 if ( ( tRecSrc == m_tBrBasEqp &&
//                        ( MEDIA_TYPE_NULL == byMediaType || m_tConf.GetMainVideoMediaType() == byMediaType ) )
//                    || ( tRecSrc == m_tVidBasEqp &&
//                         MEDIA_TYPE_NULL != byMediaType &&
//                         m_tConf.GetSecVideoMediaType() == byMediaType )
//                    )
                {
                    bSecRateDirectly = TRUE;
                }
            }
        }

        if (bSecRateDirectly)
        {
            wMinBitRate = m_tConf.GetSecBitRate()*byScale/100;
        }
        else
        {
			const u16 wSecBitrate = m_tConf.GetSecBitRate()*byScale/100;
            wMinBitRate = kmax(wMinBitRate, wSecBitrate);
        }
    }

    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[GetLeastMtReqBitrate] Bitrate.%d, primary.%d, media.%d\n", 
            wMinBitRate, bPrimary, byMediaType);

    return wMinBitRate;
}

/*=============================================================================
函 数 名： GetLeastMtReqFramerate
功    能： 得到最小的终端要求帧率
算法实现： 
全局变量： 
参    数： u8 byMediaType
           u8 bySrcMtId
返 回 值： u8 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/8/12   4.0		周广程                  创建
=============================================================================*/
u8 CMcuVcInst::GetLeastMtReqFramerate(u8 byMediaType, u8 bySrcMtId)
{
    u8 byMinFrameRate = 0xff;
    u8 byMtFrameRate;
    u8	byLoop;
    TSimCapSet tMtDstSCS;

    for (byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        if (m_tConfAllMtInfo.MtJoinedConf(byLoop) && bySrcMtId != byLoop)
        {           
            tMtDstSCS = m_ptMtTable->GetDstSCS(byLoop);
            if (MEDIA_TYPE_NULL == byMediaType || tMtDstSCS.GetVideoMediaType() == byMediaType)
            {
                if ( MEDIA_TYPE_H264 == tMtDstSCS.GetVideoMediaType() )
                {
                    byMtFrameRate = tMtDstSCS.GetUserDefFrameRate();
                }
                else
                {
                    byMtFrameRate = tMtDstSCS.GetVideoFrameRate();
                }
                if ( byMtFrameRate != 0 && byMtFrameRate < byMinFrameRate)
                {
                    byMinFrameRate = byMtFrameRate;
                }
            }
        }
    }       

    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[GetLeastMtReqFramerate] FrameRate.%d, media.%d\n", byMinFrameRate, byMediaType);

    return byMinFrameRate;
}

/*==============================================================================
函数名    :  ShowVmpAdaptMember
功能      :  显示占VMP前适配通道的成员信息
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-3-27
==============================================================================*/
void CMcuVcInst::ShowVmpAdaptMember (void)
{
	TVmpChnnlInfo tVmpChnnlInfo;
	TEqp tVmpEqp;
	u8 byAdpChnnlNum;
	TChnnlMemberInfo tChnnlMemInfo;
	u16 wMcuIdx;
	u8 byMtId;
	BOOL32 bSpeaker = FALSE;
	BOOL32 bSelected= FALSE;
	BOOL32 bNoneKeda= FALSE;
	BOOL32 bDStream = FALSE;
	BOOL32 bIsMMcu	= FALSE;
	BOOL32 bIPC = FALSE;
	u8 byLoop;
	StaticLog("\n---会议 %s中 VMP前适配通道成员信息---\n",m_tConf.GetConfName() );
	for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
	{
		if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
		{
			continue;
		}
		tVmpEqp = g_cMcuVcApp.GetEqp( m_abyVmpEqpId[byIdx] );
		tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
		byAdpChnnlNum = tVmpChnnlInfo.GetHDChnnlNum();
		StaticLog("   VMP[%d]最大支持%u风格不降分辨率进vmp,当前共有%u个Mt占用前适配通道\n", tVmpEqp.GetEqpId(), tVmpChnnlInfo.GetMaxStyleNum(), byAdpChnnlNum);
		StaticLog("--------------------------------------\n");
		for(byLoop = 0; byLoop < tVmpChnnlInfo.GetMaxNumHdChnnl(); byLoop ++)
		{
			tVmpChnnlInfo.GetHdChnnlInfo(byLoop,&tChnnlMemInfo);
			wMcuIdx = tChnnlMemInfo.GetMt().GetMcuIdx();
			byMtId = tChnnlMemInfo.GetMtId();
			bSpeaker = tChnnlMemInfo.IsAttrSpeaker();
			bSelected = tChnnlMemInfo.IsAttrSelected();
			bNoneKeda = tChnnlMemInfo.IsAttrNoneKeda();
			bDStream = tChnnlMemInfo.IsAttrDstream();
			bIsMMcu = tChnnlMemInfo.IsAttrMMcu();
			bIPC = tChnnlMemInfo.IsAttrBeIPC();
			StaticLog("[ID.%u]:(%u,%u)\tbSpeaker:%d\tbSelected:%d\tbNoneKeda:%d\tbDStream:%d\tbIsSpecMcu:%d\tbIsIPC:%d\n",
				byLoop+1, wMcuIdx, byMtId, bSpeaker, bSelected, bNoneKeda, bDStream, bIsMMcu, bIPC);
		}
	}
}
/*====================================================================
    函数名      ：ShowConfMt
    功能        ：打印会议终端
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId    = 0
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/11    3.0         胡昌威          创建
====================================================================*/
void CMcuVcInst::ShowConfMt(u8 byMtId)
{
	BOOL32 bInConf,bInJoinedConf;
	TMtAlias tMtAliasH323id, tMtAliasE164, tMtAliasH320id, tMtAliasH320Alias;
	char achTemp[255];
	TLogicalChannel tLogicalChannel;
	u8 byLoop;

	StaticLog( "\n---会议 %s 终端信息---\n", m_tConf.GetConfName() );
	StaticLog("IsTemplate %d\n", m_tConf.m_tStatus.IsTemplate() );
	StaticLog("InstID %d\n", GetInsID() );
    StaticLog("ConfIdx %d\n", m_byConfIdx );
    m_tConf.GetConfId().GetConfIdString(achTemp, sizeof(achTemp));
    StaticLog("cConfId %s\n", achTemp );
	StaticLog("Current Audio Mt Num.%d\n",g_cMcuVcApp.GetCurrentAudMtNum());
	StaticLog("Current SatMt Num.%d\n",GetCurSatMtNum());

	StaticLog("本地终端: McId-%d\n", LOCAL_MCUID );

	for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		if(NULL == m_ptMtTable)
		{
			break;
		}
		bInConf = m_tConfAllMtInfo.MtInConf( byLoop );
		bInJoinedConf = m_tConfAllMtInfo.MtJoinedConf( byLoop );

        if ( byMtId != 0 )
        {
            if ( byLoop != byMtId )
            {
                continue;
            }
            if ( !bInConf )
            {
                StaticLog("MtId.%d unexist, mishanding ? ...\n", byMtId );
                break;
            }
        }

		if(bInConf)
		{
			if( bInJoinedConf )
			{
				sprintf( achTemp, "√ %c", 0 );
			}
			else
			{
				sprintf( achTemp, "× %c", 0 ); 
			}

			char achHDMtFlag[5];
			
			if( g_cMcuVcApp.IsOccupyHDAccessPoint(m_byConfIdx, byLoop))
			{
				sprintf( achHDMtFlag, "(HD)");
			}
			else
			{
				sprintf( achHDMtFlag, "");
			}

			char achAudioMt[6];
			if( g_cMcuVcApp.IsOccupyAudAccessPoint(m_byConfIdx, byLoop))
			{
				sprintf( achAudioMt, "(Aud)");
			}
			else
			{
				sprintf( achAudioMt, "");
			}
				
			tMtAliasH323id.SetNull();
			tMtAliasE164.SetNull();
            tMtAliasH320id.SetNull();
            tMtAliasH320Alias.SetNull();
			m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeH323ID, &tMtAliasH323id );
			m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeE164, &tMtAliasE164 );
			m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeH320ID, &tMtAliasH320id );
            m_ptMtTable->GetMtAlias( byLoop, mtAliasTypeH320Alias, &tMtAliasH320Alias);
			TMt tMt = m_ptMtTable->GetMt( byLoop );

            TMtAlias tDailAlias;
            BOOL32 bRet = m_ptMtTable->GetDialAlias(tMt.GetMtId(), &tDailAlias);
            if ( !bRet )
            {
                StaticLog("[ShowConfMt] GetMt.%d DailAlias failed\n", tMt.GetMtId() );
            }

            if ( !bRet )
            {
                StaticLog("%sMT%d:0x%x(Dri:%d Mp:%d),St:%d Type: %d Manu: %s, 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s,ProductId:%s\n",
                                        achTemp, byLoop, m_ptMtTable->GetIPAddr( byLoop ), tMt.GetDriId(), 
                                        m_ptMtTable->GetMpId( tMt.GetMtId() ),IsMultiCastMt(byLoop),
                                        m_ptMtTable->GetMtType( tMt.GetMtId() ),
                                        GetManufactureStr( m_ptMtTable->GetManuId( byLoop ) ), 
                                        tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
                                        tMtAliasH320Alias.m_achAlias,
										m_ptMtTable->GetProductId(tMt.GetMtId()));
            }
            else
            {
                if ( mtAliasTypeTransportAddress == tDailAlias.m_AliasType )
                {
                    StaticLog("%sMT%d%s%s:0x%x(Dri:%d Mp:%d),St:%d Type: %d Manu: %s, 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s, DialAlias.%s@%d,ProductId:%s\n",
                                            achTemp, byLoop, achHDMtFlag,achAudioMt,m_ptMtTable->GetIPAddr( byLoop ), tMt.GetDriId(), 
                                            m_ptMtTable->GetMpId( tMt.GetMtId() ),IsMultiCastMt(byLoop),
                                            m_ptMtTable->GetMtType( tMt.GetMtId() ),
                                            GetManufactureStr( m_ptMtTable->GetManuId( byLoop ) ), 
                                            tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
                                            tMtAliasH320Alias.m_achAlias,
                                            StrOfIP(tDailAlias.m_tTransportAddr.GetIpAddr()), tDailAlias.m_tTransportAddr.GetPort(),
											m_ptMtTable->GetProductId(tMt.GetMtId()));
                }
                else
                {
                    StaticLog("%sMT%d%s%s:0x%x(Dri:%d Mp:%d),St:%d Type: %d Manu: %s, 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s, DialAlias.%s,ProductId:%s\n",
                                            achTemp, byLoop, achHDMtFlag,achAudioMt, m_ptMtTable->GetIPAddr( byLoop ), tMt.GetDriId(), 
                                            m_ptMtTable->GetMpId( tMt.GetMtId() ),IsMultiCastMt(byLoop),
                                            m_ptMtTable->GetMtType( tMt.GetMtId() ),
                                            GetManufactureStr( m_ptMtTable->GetManuId( byLoop ) ), 
                                            tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
                                            tMtAliasH320Alias.m_achAlias,
                                            tDailAlias.m_achAlias,
											m_ptMtTable->GetProductId(tMt.GetMtId()));
                }
            }

			if( bInJoinedConf )
			{
				//正向逻辑通道信息
				memset( achTemp, ' ', 255 );
				int l = sprintf( achTemp, "   FL Aud:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_AUDIO, &tLogicalChannel, TRUE ) )
				{
					l += sprintf( achTemp+l, "%d(%s)(Num:%d)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
						                                       GetMediaStr( tLogicalChannel.GetChannelType() ),
															   tLogicalChannel.GetAudioTrackNum());
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 33;
				l += sprintf( achTemp+l, "Vid:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_VIDEO, &tLogicalChannel, TRUE ) )
				{
                    // 增加分辨率打印, zgc, 2008-08-09
					l += sprintf( achTemp+l, "%d(%s)(%s)(%dFps)(%dK)(%s)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
						                                       GetMediaStr( tLogicalChannel.GetChannelType() ),
                                                               GetResStr( tLogicalChannel.GetVideoFormat() ),
															   tLogicalChannel.GetChanVidFPS(),
															   tLogicalChannel.GetFlowControl(),
															   tLogicalChannel.GetProfileAttrb()==emHpAttrb?"HP":"BP");
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 75;//70 //50;
				l += sprintf( achTemp+l, "H239:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_SECVIDEO, &tLogicalChannel, TRUE ) )
				{
					l += sprintf( achTemp+l, "%d(%s)(%s)(%dFps)<%dK>(%s)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
                                                            GetMediaStr( tLogicalChannel.GetChannelType() ),
															GetResStr(tLogicalChannel.GetVideoFormat()),
                                                            tLogicalChannel.GetChanVidFPS(),
															tLogicalChannel.GetFlowControl(),
															tLogicalChannel.GetProfileAttrb()==emHpAttrb?"HP":"BP");
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}
				
				memset( achTemp+l, ' ', 255-l );
				l = 117;//86//68;
				l += sprintf( achTemp+l, "T120:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_T120DATA, &tLogicalChannel, TRUE ) )
				{
					l += sprintf( achTemp+l, "%d", tLogicalChannel.GetRcvMediaChannel().GetPort() );
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 126;//97//79;
				l += sprintf( achTemp+l, "H224:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_H224DATA, &tLogicalChannel, TRUE ) )
				{
					l += sprintf( achTemp+l, "%d", tLogicalChannel.GetRcvMediaChannel().GetPort() );
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

                memset( achTemp+l, ' ', 255-l );
                l = 136;//109//91;
                l += sprintf(achTemp+l, "VidBand:%dK", m_ptMtTable->GetRcvBandWidth(byLoop));                

				sprintf( achTemp+l, "%c", 0 );
				StaticLog("%s\n", achTemp );

				//反向逻辑通道信息
				memset( achTemp, ' ', 255 );
				l = sprintf( achTemp, "   RL Aud:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_AUDIO, &tLogicalChannel, FALSE ) )
				{
					l += sprintf( achTemp+l, "%d(%s)(%d)(Num:%d)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
						                                    GetMediaStr( tLogicalChannel.GetChannelType() ),
															tLogicalChannel.GetFlowControl(),
															tLogicalChannel.GetAudioTrackNum());
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 33;
				l += sprintf( achTemp+l, "Vid:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_VIDEO, &tLogicalChannel, FALSE ) )
				{
                    // 增加分辨率打印, zgc, 2008-08-09
					l += sprintf( achTemp+l, "%d(%s)(%s)(%dFps)(%dK)(%s)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
						                                       GetMediaStr( tLogicalChannel.GetChannelType() ),
                                                               GetResStr( tLogicalChannel.GetVideoFormat() ),
															   tLogicalChannel.GetChanVidFPS(),
                                                               tLogicalChannel.GetFlowControl(),
															   tLogicalChannel.GetProfileAttrb()==emHpAttrb?"HP":"BP");
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 75;//70;//50
				l += sprintf( achTemp+l, "H239:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_SECVIDEO, &tLogicalChannel, FALSE ) )
				{
					l += sprintf( achTemp+l, "%d(%s)(%s)(%dFps)<%dK>(%s)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
                                                            GetMediaStr( tLogicalChannel.GetChannelType() ),
															GetResStr(tLogicalChannel.GetVideoFormat()),
                                                            tLogicalChannel.GetChanVidFPS(),
															tLogicalChannel.GetFlowControl(),
															tLogicalChannel.GetProfileAttrb()==emHpAttrb?"HP":"BP");
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 117;//86//68;
				l += sprintf( achTemp+l, "T120:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_T120DATA, &tLogicalChannel, FALSE ) )
				{
					l += sprintf( achTemp+l, "%d", tLogicalChannel.GetRcvMediaChannel().GetPort() );
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 126;//97//79;
				l += sprintf( achTemp+l, "H224:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_H224DATA, &tLogicalChannel, FALSE ) )
				{
					l += sprintf( achTemp+l, "%d", tLogicalChannel.GetRcvMediaChannel().GetPort() );
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

                memset( achTemp+l, ' ', 255-l );
                l = 136;//109//91;
                l += sprintf(achTemp+l, "VidBand:%dK", m_ptMtTable->GetSndBandWidth(byLoop));     

				sprintf( achTemp+l, "%c", 0 );
				StaticLog("%s\n", achTemp );
			}
            
            if ( byMtId != 0 )
            {
                StaticLog("\nMT.%d's Cap is as follows:\n\n", byMtId );
                TCapSupport tCap;
                if ( m_ptMtTable->GetMtCapSupport(byMtId, &tCap) )
                {
                    tCap.Print();
                }
                TCapSupportEx tCapEx;
                if ( m_ptMtTable->GetMtCapSupportEx(byMtId, &tCapEx))
                {
                    tCapEx.Print();
                }
            }
		}
	}

	if (NULL == m_ptConfOtherMcTable)
	{
		return;
	}

	u16 wMcuIdx;
	//u8 byMcuId = 0,bySecMcuId = 0;
	u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
	memset( &abyMcuId[0],0,sizeof( abyMcuId ) );
	
	for( u16 wLoop1 = 0; wLoop1 < m_tConfAllMtInfo.GetMaxMcuNum(); wLoop1++ )
	{
		wMcuIdx = m_tConfAllMtInfo.GetMtInfo(wLoop1).GetMcuIdx();
		if( !IsValidMcuId( wMcuIdx ))
		{
			continue;
		}


        if ( m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0] ) &&
			 byMtId != 0 &&
             abyMcuId[0] != byMtId )
        {
            continue;
        }		

		if( 0 == abyMcuId[0] )
		{
			continue;
		}

		
		if( m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) )
		{
			StaticLog("其它终端: McuId-%d IsSupportMultiSpy:%d IsSupportMultiCascade:%d\n", abyMcuId[0],
				IsLocalAndSMcuSupMultSpy( wMcuIdx ), m_ptConfOtherMcTable->IsMcuSupMultCas(wMcuIdx)
				);
		}
		else
		{
			StaticLog("其它非直属终端: McuId-%d SMcuId-%d IsSupportMultiSpy:%d IsSupportMultiCascade:%d\n", abyMcuId[0],
				abyMcuId[1],
				IsLocalAndSMcuSupMultSpy( wMcuIdx ),
				 m_ptConfOtherMcTable->IsMcuSupMultCas(wMcuIdx)
				);
		}
		
		TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
		if(ptMcInfo == NULL)
		{
			continue;
		}
		TConfMtInfo *ptConfMtInfo = m_tConfAllMtInfo.GetMtInfoPtr(wMcuIdx); 
		if(ptConfMtInfo == NULL)
		{
			continue;
		}
		for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
		{	
			TMtExt tMtExt = ptMcInfo->m_atMtExt[byLoop-1];
			if(tMtExt.GetMtId() == 0)
			{
				continue;//自己
			}
			bInConf = ptConfMtInfo->MtInConf( tMtExt.GetMtId() );
			bInJoinedConf = ptConfMtInfo->MtJoinedConf( tMtExt.GetMtId() );

			if(!bInConf)
			{
				continue;
			}

			if( bInJoinedConf )
			{
				sprintf( achTemp, "√ %c", 0 );
			}
			else
			{
				sprintf( achTemp, "× %c", 0 ); 
			}
	
			if( 0 == abyMcuId[1] )
			{
				StaticLog("%sMcu%dMT%d:0x%x(Dri:%d), MainType: %d SubType: %d Manu: %s, Alias: %s, DialBitRate: %d. \n",
						achTemp, abyMcuId[0], tMtExt.GetMtId(), tMtExt.GetIPAddr(), tMtExt.GetDriId(), 
						tMtExt.GetType(),tMtExt.GetMtType(),
						GetManufactureStr( tMtExt.GetManuId() ), 
						tMtExt.GetAlias(), 
						tMtExt.GetDialBitRate() );

			}
			else
			{
				StaticLog("%sMcu%dMcu%dMT%d:0x%x(Dri:%d), MainType: %d SubType: %d Manu: %s, Alias: %s, DialBitRate: %d. \n",
					achTemp, abyMcuId[0], abyMcuId[1],tMtExt.GetMtId(), tMtExt.GetIPAddr(), tMtExt.GetDriId(), 
					tMtExt.GetType(),tMtExt.GetMtType(),
					GetManufactureStr( tMtExt.GetManuId() ), 
					tMtExt.GetAlias(), 
					tMtExt.GetDialBitRate() );
			}
		}				
	}

    return;
}
/*====================================================================
    函数名      ：ShowConfMcu
    功能        ：打印会议所有mcu信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    10/08/02    4.6         周晶晶          创建
====================================================================*/
void CMcuVcInst::ShowConfMcu( void )
{
	//m_tConfAllMcuInfo.AddMcu()

	u16 wLoop = 0;
	u8 abyMcuId[MAX_CASCADEDEPTH-1] = {0};
	TConfMcInfo *ptMcInfo = NULL;
	TConfMtInfo *ptConfMtInfo = NULL;
	TMtExt tMtExt;
	TMtAlias tDailAlias,tMtAliasH323id,tMtAliasE164,tMtAliasH320id,tMtAliasH320Alias;

	u8 byLoop = 0;
	while ( wLoop < MAXNUM_CONFSUB_MCU ) 
	{
		m_tConfAllMcuInfo.GetMcuIdByIdx( wLoop, &abyMcuId[0] );
		if( 0 == abyMcuId[0] )
		{
			++wLoop;
			continue;
		}

		ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(wLoop);
		if (NULL == ptMcInfo)
		{
			++wLoop;
			continue;
		}

		if( 0 == abyMcuId[1] )
		{
			tMtAliasH323id.SetNull();
			tMtAliasE164.SetNull();
            tMtAliasH320id.SetNull();
            tMtAliasH320Alias.SetNull();
			m_ptMtTable->GetMtAlias( abyMcuId[0], mtAliasTypeH323ID, &tMtAliasH323id );
			m_ptMtTable->GetMtAlias( abyMcuId[0], mtAliasTypeE164, &tMtAliasE164 );
			m_ptMtTable->GetMtAlias( abyMcuId[0], mtAliasTypeH320ID, &tMtAliasH320id );
            m_ptMtTable->GetMtAlias( abyMcuId[0], mtAliasTypeH320Alias, &tMtAliasH320Alias);			
            
            BOOL32 bRet = m_ptMtTable->GetDialAlias(abyMcuId[0], &tDailAlias);
            
            if ( !bRet )
            {
				StaticLog("McuIdx(%d) McuId(%d) ViewMt(%d,%d) LastViewMt(%d,%d) IP:0x%08x 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s\n,",
								wLoop, abyMcuId[0], 
								ptMcInfo->m_tMMcuViewMt.GetMcuId(), ptMcInfo->m_tMMcuViewMt.GetMtId(), 
								ptMcInfo->m_tLastMMcuViewMt.GetMcuId(), ptMcInfo->m_tLastMMcuViewMt.GetMtId(),
								m_ptMtTable->GetIPAddr( abyMcuId[0] ),
								tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
                                tMtAliasH320Alias.m_achAlias
								);
            }
            else
            {
                if ( mtAliasTypeTransportAddress == tDailAlias.m_AliasType )
                {
                    StaticLog("McuIdx(%d) McuId(%d) ViewMt(%d,%d) LastViewMt(%d,%d) IP:0x%08x 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s DialAlias.%s@%d\n",
								wLoop, abyMcuId[0], 
								ptMcInfo->m_tMMcuViewMt.GetMcuId(), ptMcInfo->m_tMMcuViewMt.GetMtId(), 
								ptMcInfo->m_tLastMMcuViewMt.GetMcuId(), ptMcInfo->m_tLastMMcuViewMt.GetMtId(), 
								m_ptMtTable->GetIPAddr( abyMcuId[0] ),
                                tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
                                tMtAliasH320Alias.m_achAlias,
                                StrOfIP(tDailAlias.m_tTransportAddr.GetIpAddr()), tDailAlias.m_tTransportAddr.GetPort());
                }
                else
                {
					StaticLog("McuIdx(%d) McuId(%d) ViewMt(%d,%d) LastViewMt(%d,%d) IP:0x%08x 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s DialAlias.%s\n,",
								wLoop, abyMcuId[0], 
								ptMcInfo->m_tMMcuViewMt.GetMcuId(), ptMcInfo->m_tMMcuViewMt.GetMtId(), 
								ptMcInfo->m_tLastMMcuViewMt.GetMcuId(), ptMcInfo->m_tLastMMcuViewMt.GetMtId(), 
								m_ptMtTable->GetIPAddr( abyMcuId[0] ),
                                tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
                                tMtAliasH320Alias.m_achAlias,
                                tDailAlias.m_achAlias
								);
                }
			}		
		}
		else
		{
			u16 wMcuIdx = INVALID_MCUIDX;
			m_tConfAllMcuInfo.GetIdxByMcuId( &abyMcuId[0],1,&wMcuIdx );
			if( INVALID_MCUIDX == wMcuIdx )
			{
				++wLoop;
				continue;
			}
			
			ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
			if (NULL == ptMcInfo)
			{
				++wLoop;
				continue;
			}

			ptConfMtInfo = m_tConfAllMtInfo.GetMtInfoPtr(wMcuIdx); 

			m_tConfAllMcuInfo.GetIdxByMcuId( &abyMcuId[0],2,&wMcuIdx );
			TConfMcInfo *ptSMcuMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);

			if( NULL != ptConfMtInfo)
			{
				for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
				{	
					tMtExt = ptMcInfo->m_atMtExt[byLoop-1];
					if( tMtExt.GetMtId() == abyMcuId[1] )
					{
						if( ptConfMtInfo->MtInConf( tMtExt.GetMtId() ) && NULL != ptSMcuMcInfo  )
						{
							StaticLog("McuIdx(%d) McuId(%d-%d) ViewMt(%d,%d) LastViewMt(%d,%d) IP:0x%08x Alias.%s\n",
								wLoop, abyMcuId[0], abyMcuId[1], 
								ptSMcuMcInfo->m_tMMcuViewMt.GetMcuId(), ptSMcuMcInfo->m_tMMcuViewMt.GetMtId(), 
								ptSMcuMcInfo->m_tLastMMcuViewMt.GetMcuId(), ptSMcuMcInfo->m_tLastMMcuViewMt.GetMtId(), 
								tMtExt.GetIPAddr(), tMtExt.GetAlias()
								);
						}
						
						break;
					}
				}
			}
		}
		
		++wLoop;
	}
}

/*====================================================================
    函数名      ：ShowConfMix
    功能        ：打印会议混音信息
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId    = 0
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/11    3.0         胡昌威          创建
====================================================================*/
void CMcuVcInst::ShowConfMix()
{
	char achTemp[255];
	u8 byLoop = 0;
	StaticLog("\n---会议 %s 混音信息---\n", m_tConf.GetConfName() );
	StaticLog("IsTemplate %d\n", m_tConf.m_tStatus.IsTemplate() );
	StaticLog("InstID %d\n", GetInsID() );
    StaticLog("ConfIdx %d\n", m_byConfIdx );
    m_tConf.GetConfId().GetConfIdString(achTemp, sizeof(achTemp));
    StaticLog("ConfId %s\n", achTemp );

	StaticLog("\n");
	for( u8 byPos = 0; byPos <= MAXNUM_MIXER_CHNNL; byPos++ )
	{
		if (!m_atMixMt[byPos].IsNull())
		{
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "Mt(%d, %d) has used MIXER.%d Chnl(%d)\n",
				m_atMixMt[byPos].GetMcuIdx(), m_atMixMt[byPos].GetMtId(), m_tMixEqp.GetMtId(), byPos - 1 );
		}
	}

	static u8 s_achMixMode[][16] = {"NoMix", "WholeMix", "PartMix", "VacMix", "VacWholeMix", "VacPartMix"}; 
	u8 byMixKind = sizeof(s_achMixMode) / 16;
	u8 byMixMode = m_tConf.m_tStatus.GetMixerMode();
	if (byMixMode >= byMixKind)
	{
		StaticLog("ERROR: The Current Mix Mode is %d\n", byMixMode );
		return;
	}

	StaticLog("\n");
	StaticLog("The Current Mix Mode: %s\n", s_achMixMode[byMixMode] );
	if (m_byLastMixMode >= byMixKind)
	{
		StaticLog("ERROR: The Last Mix Mode is %d\n", m_byLastMixMode );
		return;
	}
	StaticLog("The Last Mix Mode: %s\n", s_achMixMode[m_byLastMixMode] );
	StaticLog("The MixMemNum: %d\n", m_tConf.m_tStatus.GetMixerParam().GetMemberNum());
	StaticLog("The MixMaxMemNum: %d\n",m_tConf.m_tStatus.GetMixerParam().GetMtMaxNum());
	
	if (mcuNoMix == byMixMode)
	{
		return;
	}

	TLogicalChannel tLogicalChannel;
	u32	dwVSndIp = 0, dwASndIp = 0, dwVRcvIp = 0,dwARcvIp = 0;
	u16 wVSndPort = 0, wASndPort = 0, wVRcvPort = 0, wARcvPort = 0;
	u8 byFwdChannelNum = 0;
	u8 byRvsChannelNum = 0;
	
	StaticLog("\n");
	StaticLog("当前使用的混音器信息:\n");
	StaticLog("EQPID  SNDAIP          SPORT CHNNUM RCVAIP           RPORT CHNNUM\n");
	StaticLog("------ --------------- ----- ------ --------------- ----- ------\n");

	u8 byIndex = m_tMixEqp.GetEqpId();
	
	if( g_cMcuVcApp.IsPeriEqpConnected( byIndex ) )
	{
		if( g_cMcuVcApp.GetPeriEqpLogicChnnl( byIndex, MODE_AUDIO, &byFwdChannelNum, &tLogicalChannel, TRUE) )
		{
			dwASndIp = tLogicalChannel.m_tSndMediaCtrlChannel.GetIpAddr();
			wASndPort = tLogicalChannel.m_tSndMediaCtrlChannel.GetPort();
		}
		
		if( g_cMcuVcApp.GetPeriEqpLogicChnnl( byIndex, MODE_AUDIO, &byRvsChannelNum, &tLogicalChannel, FALSE) )
		{
			dwARcvIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
			wARcvPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();
		}
		StaticLog("%6u %15s %5u %6u %15s %5u %6u\n", byIndex,
			StrOfIP(dwASndIp), wASndPort, byFwdChannelNum, StrOfIP(dwARcvIp), wARcvPort, byRvsChannelNum);
	}

	if (!m_tConf.m_tStatus.IsSpecMixing())
	{
		return;
	}

	StaticLog("\n参与混音的终端列表:\n");
	TMtStatus tMixMtStatus;
	u32 dwCnt = 0;
	StaticLog(" MTID        MTIP           FL Aud            RL Aud       AutoInMix \n");
	StaticLog("------ --------------- --------------- ------------------- --------- \n");

	for(byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		m_ptMtTable->GetMtStatus(byLoop, &tMixMtStatus);

		if (tMixMtStatus.IsInMixing())
		{
			dwCnt = 0;
			memset( achTemp, 0, 255 );
			dwCnt += (u32)sprintf( achTemp, "%6u %15s ", byLoop, StrOfIP(m_ptMtTable->GetIPAddr(byLoop)));

			//前向通道
			if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_AUDIO, &tLogicalChannel, TRUE ) )
			{
				dwCnt += (u32)sprintf( achTemp + dwCnt, "%6u(%6s)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
					GetMediaStr( tLogicalChannel.GetChannelType() ) );
			}
			else
			{
				dwCnt += (u32)sprintf( achTemp + dwCnt, "NO" );
			}
			//后向通道
			dwCnt += (u32)sprintf( achTemp + dwCnt, " ");
			if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_AUDIO, &tLogicalChannel, FALSE ) )
			{
				dwCnt += (u32)sprintf( achTemp + dwCnt, "%6u(%6s)(%3d)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
					GetMediaStr( tLogicalChannel.GetChannelType() ), tLogicalChannel.GetFlowControl());
			}
			else
			{
				dwCnt += (u32)sprintf( achTemp + dwCnt, "NO" );
			}
			dwCnt += (u32)sprintf( achTemp + dwCnt, " %6u", m_ptMtTable->IsMtAutoInSpec(byLoop));
			StaticLog("%s\n", achTemp);
		}
	}

	//全混终端
	StaticLog("\n接收全混的终端列表:\n");
	TMtStatus tNRcvMtStatus;
	u32 dwNCnt = 0;
	StaticLog(" MTID        MTIP           FL Aud            RL Aud       AutoInMix \n");
	StaticLog("------ --------------- --------------- ------------------- --------- \n");
	for(byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		m_ptMtTable->GetMtStatus(byLoop, &tNRcvMtStatus);
		if (m_tConfAllMtInfo.MtJoinedConf(byLoop) && !tNRcvMtStatus.IsInMixing() && m_tConf.m_tStatus.IsMixing())
		{
			dwNCnt = 0;
			memset(achTemp, 0, sizeof(achTemp));
			dwNCnt += (u32)sprintf( achTemp, "%6u %15s ", byLoop, StrOfIP(m_ptMtTable->GetIPAddr(byLoop)));

			//前向通道
			if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_AUDIO, &tLogicalChannel, TRUE ) )
			{
				dwNCnt += (u32)sprintf( achTemp + dwNCnt, "%6u(%6s)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
					GetMediaStr( tLogicalChannel.GetChannelType() ) );
			}
			else
			{
				dwNCnt += (u32)sprintf( achTemp + dwNCnt, "NO" );
			}
			//后向通道
			dwNCnt += (u32)sprintf( achTemp + dwNCnt, " ");
			if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_AUDIO, &tLogicalChannel, FALSE ) )
			{
				dwNCnt += (u32)sprintf( achTemp + dwNCnt, "%6u(%6s)(%3d)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
					GetMediaStr( tLogicalChannel.GetChannelType() ), tLogicalChannel.GetFlowControl());
			}
			else
			{
				dwNCnt += (u32)sprintf( achTemp + dwNCnt, "NO" );
			}
			//是否自动混音
			dwNCnt += (u32)sprintf( achTemp + dwNCnt, " %6u", m_ptMtTable->IsMtAutoInSpec(byLoop));
			StaticLog("%s\n", achTemp);
		}
	}
}

/*====================================================================
    函数名      ：ShowBasInfo
    功能        ：打印会议bas信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/09/03    4.5         张宝卿          创建
====================================================================*/
void CMcuVcInst::ShowBasInfo( u8 byBasId )
{
	if ( byBasId != 0 )
	{
		if (byBasId > BASID_MAX || byBasId < BASID_MIN)
		{
			StaticLog("Illegal bas id.%d, mishanding ? ...\n", byBasId );
			return;
		}
		if (!g_cMcuVcApp.IsPeriEqpConnected(byBasId))
		{
			StaticLog("Bas.%d is not online yet, try another ...\n", byBasId );
			return;
		}
	}
	PrintBas();
	return;
}


/*====================================================================
    函数名      ：ShowConfMod
    功能        ：打印会议模板信息
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/11    3.0         胡昌威          创建
====================================================================*/
void CMcuVcInst::ShowConfMod()
{
	TVmpModuleInfo tVmpModule = m_tConfEqpModule.m_tVmpModuleInfo;

	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
    if( tConfAttrb.IsHasVmpModule() )
	{
		StaticLog("Vmp module info:\n" );
		StaticLog("tVmpModule.m_tVMPParam.m_byVMPAuto = %d\n", tVmpModule.m_byVMPAuto  );
		StaticLog("tVmpModule.m_tVMPParam.m_byVMPBrdst = %d\n", tVmpModule.m_byVMPBrdst  );
		StaticLog("tVmpModule.m_tVMPParam.m_byVMPStyle = %d\n", tVmpModule.m_byVMPStyle );
		for( u8 byLoop = 0; byLoop < MAXNUM_MPU2VMP_MEMBER; byLoop++ )
		{
			if( m_tConfEqpModule.m_tVmpModuleInfo.m_abyVmpMember[byLoop] != 0)
			{
				StaticLog("Vmp Chl%d(mt%d)\n", byLoop, m_tConfEqpModule.m_tVmpModuleInfo.m_abyVmpMember[byLoop] );
			}
		}
	}
    
    TMultiTvWallModule tMultiTvWallModule = m_tConfEqpModule.m_tMultiTvWallModule;
    TTvWallModule tOneModule;
	TConfAttrb tConfattrb = m_tConf.GetConfAttrb();
    if ( tConfattrb.IsHasTvWallModule() )
    {
        StaticLog("Tvwall module info:\n" );
        StaticLog("tMultiTvWallModule.m_byTvModuleNum %d\n", tMultiTvWallModule.GetTvModuleNum());
        for ( u8 byLoop = 0; byLoop < tMultiTvWallModule.GetTvModuleNum(); byLoop++ )
        {
            tMultiTvWallModule.GetTvModuleByIdx(byLoop, tOneModule);
            StaticLog("Tvwall module %d:\n", byLoop );            
            StaticLog("Tvwall EqpId.%d\n", tOneModule.GetTvEqp().GetEqpId() );
            
            for ( u8 byLoop2 = 0; byLoop2 < MAXNUM_PERIEQP_CHNNL; byLoop2 ++ )
            {
                if ( tOneModule.m_abyTvWallMember[byLoop2] == 0 || 
                     tOneModule.m_abyMemberType[byLoop2] == 0 )
                     break;

                StaticLog("\tMember %d: Mt.%d, type: %d\n", 
                           byLoop2,
                           tOneModule.m_abyTvWallMember[byLoop2],
                           tOneModule.m_abyMemberType[byLoop2] );
            }
            
        }
    }

	StaticLog("HDU vmp Chnnl module info:\n" );
	m_tConfEqpModule.m_tHduVmpModuleOrHDTvWall.Print();
}

/*====================================================================
    函数名      ：ShowMtMonitor
    功能        ：打印终端监控信息
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/28    3.0         胡昌威          创建
====================================================================*/
void CMcuVcInst::ShowMtMonitor()
{
	char achTemp[255];
	StaticLog("\n---会议 %s 终端监控信息---\n", m_tConf.GetConfName() );
    int l = 0;

	for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
		{
			if( m_ptMtTable->IsMtMulticasting( byLoop ) )
			{
				sprintf( achTemp, "√ %c", 0 );
			}
			else
			{
				sprintf( achTemp, "× %c", 0 ); 
			}

			StaticLog("MT%d-0x%x multicasting: %s \n",
				       byLoop, m_ptMtTable->GetIPAddr( byLoop ), achTemp );

			u8 byMtList[MAXNUM_MT_CHANNL];
			u8 byMtNum = m_ptMtTable->GetMonitorSrcMtList( byLoop, byMtList );
			if( byMtNum > 0 )
			{
				l = sprintf( achTemp, "SrcMtList: " );
				for( u8 byIndex = 0; byIndex < byMtNum; byIndex++ )
				{
					l = l + sprintf( achTemp+l, " %d", byMtList[byIndex] );
				}
				StaticLog("%s\n", achTemp );
			}
		}
	}
}

/*=============================================================================
    函 数 名： InviteUnjoinedMt
    功    能： 邀请未与会终端入会
    算法实现： 
    全局变量： 
    参    数： const CServMsg & cServMsgHdr,	业务消息头
			   const TMt * ptMt, 受邀终端指针，NULL表示邀请会议中所有直连未与会终端
               BOOL32 bSendAlert  是否向级联mcu发送Alert呼叫通知
               BOOL32 bBrdSend    是否向所有mcu广播此Alert呼叫通知或者只是回复重邀端mcu
			   u8     byCallType  呼叫类型(呼叫请求(VCS_FORCECALL_REQ) 呼叫强拆命令(VCS_FORCECALL_CMD))
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/5/25    3.5			万春雷                  创建
=============================================================================*/
void CMcuVcInst::InviteUnjoinedMt( const CServMsg& cServMsgHdr, const TMt* ptMt, 
								   BOOL32 bSendAlert, BOOL32 bBrdSend, 
								   u8 byCallType/* = VCS_FORCECALL_REQ*/,
								   BOOL bLowLevelMcuCalledIn/* = FALSE*/)
{
	CServMsg	cServMsg;
	TMtAlias	tMtAlias;
	TMtAlias	tConfAlias;

	tConfAlias.SetH323Alias( m_tConf.GetConfName() );

	//send invite message to connected MTs
	cServMsg.SetServMsg( cServMsgHdr.GetServMsg(), SERV_MSGHEAD_LEN );
    cServMsg.SetConfIdx( m_byConfIdx );
    cServMsg.SetConfId( m_tConf.GetConfId() );

	u16 wAudioBand = GetAudioBitrate( m_tConf.GetMainAudioMediaType() );
	u8  byEncrypt  =  (m_tConf.GetConfAttrb().GetEncryptMode() != CONF_ENCRYPTMODE_NONE) ? 1 : 0;

	if( ptMt != NULL )
	{
		//判断终端是否已经与会
		if( m_tConfAllMtInfo.MtJoinedConf( ptMt->GetMtId() ) )
		{
			cServMsg.SetErrorCode( ERR_MCU_ADDEDMT_INCONF );
			
			post( MAKEIID( GetAppID(), GetInsID() ), MT_MCU_INVITEMT_NACK, 
					    cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			return;
		}

        if (!m_ptMtTable->GetDialAlias( ptMt->GetMtId(), &tMtAlias ))
        {
            cServMsg.SetErrorCode( ERR_MCU_CALLEDMT_NOADDRINFO);
            post( MAKEIID( GetAppID(), GetInsID() ), MT_MCU_INVITEMT_NACK, 
                        cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
            
            ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[InviteUnjoinedMt] GetDialAlias failed, byMtId.%d\n", ptMt->GetMtId());
            return;
        }

		//呼叫H320终端
		if( mtAliasTypeH320ID == tMtAlias.m_AliasType )
		{
            // xsl [11/8/2006] 分配320接入板id
            u8 byDriId = m_ptMtTable->GetDriId(ptMt->GetMtId());

            if ( !g_cMcuVcApp.IsMtAdpConnected(byDriId) || !g_cMcuVcApp.IsMtAssignInDri(byDriId, m_byConfIdx, ptMt->GetMtId()) )
            {
                byDriId = g_cMcuVcApp.AssignH320MtDriId(m_byConfIdx, tMtAlias, ptMt->GetMtId());			
			    if( !g_cMcuVcApp.IsMtAdpConnected(byDriId) || 
				    PROTOCOL_TYPE_H320 != g_cMcuVcApp.GetMtAdpProtocalType(byDriId) )
			    {
				    cServMsg.SetDstMtId( ptMt->GetMtId() );
				    cServMsg.SetErrorCode( ERR_MCU_MTUNREACHABLE );
				    SendMsgToAllMcs( MCU_MCS_CALLMT_NACK, cServMsg );
                				    
				    ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[InviteUnjoinedMt] Cannot Get IsMtAdpConnected|GetMtAdpProtocalType For Mt.%d.\n", ptMt->GetMtId() );
				    return;
			    }
                m_ptMtTable->SetDriId( ptMt->GetMtId(), byDriId );
            }            
            m_ptMtTable->SetProtocolType(ptMt->GetMtId(), PROTOCOL_TYPE_H320);
		}
		else
		{            
			m_ptMtTable->SetProtocolType(ptMt->GetMtId(), PROTOCOL_TYPE_H323);

            /*
            //zbq [09/19/2007] 本部分逻辑由上述 DialAlias 涵盖

			//根据终端表，查询终端别名
			if( m_tConf.m_tStatus.IsRegToGK() )
			{
				if( ptMt->GetMtType() == MT_TYPE_SMCU|| ptMt->GetMtType() == MT_TYPE_MMCU)
				{
					if( !m_ptMtTable->GetMtAlias( ptMt->GetMtId(), mtAliasTypeE164, &tMtAlias ) && 
						!m_ptMtTable->GetDialAlias( ptMt->GetMtId(), &tMtAlias )  )
					{
						cServMsg.SetErrorCode( ERR_MCU_CALLEDMT_NOADDRINFO);
						post( MAKEIID( GetAppID(), GetInsID() ), MT_MCU_INVITEMT_NACK, 
							cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
						
						ConfLog( FALSE, "[InviteUnjoinedMt] Cannot Get E164MtAlias|DialAlias For Mt.%d.\n", ptMt->GetMtId() );
						return;
					}
				}
				else
				{
					if( !m_ptMtTable->GetDialAlias( ptMt->GetMtId(), &tMtAlias ) )
					{
						cServMsg.SetErrorCode( ERR_MCU_CALLEDMT_NOADDRINFO);
						post( MAKEIID( GetAppID(), GetInsID() ), MT_MCU_INVITEMT_NACK, 
							cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
						
						ConfLog( FALSE, "[InviteUnjoinedMt] Cannot Get DialAlias For Mt.%d.\n", ptMt->GetMtId() );
						return;
					}
				}
			}
			else
			{
				if( !m_ptMtTable->GetMtAlias( ptMt->GetMtId(), mtAliasTypeTransportAddress, &tMtAlias) )
				{
					cServMsg.SetErrorCode( ERR_MCU_CALLEDMT_NOADDRINFO);
					post( MAKEIID( GetAppID(), GetInsID() ), MT_MCU_INVITEMT_NACK, 
						cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
					
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[InviteUnjoinedMt] Cannot Get IP For Mt.%d.\n", ptMt->GetMtId() );
					return;
				}			
			}
            */

            // xsl [11/8/2006] 分配323接入板id            
            u8 byDriId = m_ptMtTable->GetDriId(ptMt->GetMtId());   
            u8 byMtNum = 0;
            u8 byMcuNum = 0;
            if ( !g_cMcuVcApp.IsMtAdpConnected(byDriId) || 
                 !g_cMcuVcApp.IsMtAssignInDri(byDriId, m_byConfIdx, ptMt->GetMtId()) ||
                 g_cMcuVcApp.GetMtNumOnDri(byDriId, FALSE, byMtNum, byMcuNum)  // guzh [1/18/2007] 如果本DRI已经满了则重新分配
               )
            {
                byDriId = g_cMcuVcApp.AssignH323MtDriId(m_byConfIdx, tMtAlias, ptMt->GetMtId());
                if ( !g_cMcuVcApp.IsMtAdpConnected(byDriId) )
                {
                    ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[InviteUnjoinedMt] DriId.%d for MT.%d is OFFLine.\n", byDriId, ptMt->GetMtId());
                    g_cMcuVcApp.DecMtAdpMtNum(byDriId, m_byConfIdx, ptMt->GetMtId());
                    
                    if ( byDriId > MAXNUM_DRI || 0 == byDriId )
                    {
                        // guzh [3/1/2007] 如果是找不到可以分配的空闲DRI，则提示MCS超过负载能力
                        cServMsg.SetMsgBody( (u8*)ptMt, sizeof(TMt) );                        
                        cServMsg.SetErrorCode( ERR_MCU_OVERMAXCONNMT );
                        SendMsgToAllMcs( MCU_MCS_CALLMTFAILURE_NOTIF, cServMsg ); 
                    }
                    return;
                }            
                m_ptMtTable->SetDriId(ptMt->GetMtId(), byDriId);
            }            
		}		

		//邀请终端或上级MCU
		// xliang [12/26/2008] modify：for MT call MCU initially. 
		if(ptMt->GetMtId()  == m_byMtIdNotInvite ||
		   bLowLevelMcuCalledIn) // xliang [8/29/2008] 反呼情况处理)
		{
			m_ptMtTable->SetNotInvited( ptMt->GetMtId(), TRUE );
		}
		else
		{
			m_ptMtTable->SetNotInvited( ptMt->GetMtId(), FALSE );
		}
	
		cServMsg.SetMsgBody( (u8*)ptMt, sizeof( TMt ) );
		LogPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[InviteUnjoinedMt]: mt.%d aliasType.%d\n", ptMt->GetMtId(), tMtAlias.m_AliasType);
		cServMsg.CatMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
		cServMsg.CatMsgBody( (u8*)&tConfAlias, sizeof( tConfAlias ) );	
		cServMsg.CatMsgBody( (u8*)&byEncrypt, sizeof(byEncrypt));
		//u16 wCallBand = htons( m_ptMtTable->GetDialBitrate( ptMt->GetMtId() ) + wAudioBand );
		u16 wCallBand = htons( m_ptMtTable->GetDialBitrate( ptMt->GetMtId() ) );
		cServMsg.CatMsgBody( (u8*)&wCallBand, sizeof(u16)  );
		
        TCapSupport tCapSupport = m_tConf.GetCapSupport();
		TSimCapSet tSim = tCapSupport.GetMainSimCapSet();
        // guzh [6/6/2007] 填会议码率
        if (tSim.GetVideoMaxBitRate() == 0)
        {
            tSim.SetVideoMaxBitRate(m_tConf.GetBitRate());
            tCapSupport.SetMainSimCapSet(tSim);
        }
        tSim = tCapSupport.GetSecondSimCapSet();
        if (!tSim.IsNull() && tSim.GetVideoMaxBitRate() == 0)
        {
            tSim.SetVideoMaxBitRate(m_tConf.GetBitRate());
            tCapSupport.SetSecondSimCapSet(tSim);
        }		
        tCapSupport.SetDStreamMaxBitRate(m_ptMtTable->GetDialBitrate( ptMt->GetMtId()));
       
		cServMsg.CatMsgBody( (u8*)&tCapSupport, sizeof(tCapSupport) );
		//强拆 
		u16 wForceCallInfo = MAKEWORD(m_tConf.GetConfLevel(), byCallType);
		wForceCallInfo = htons(wForceCallInfo);
		cServMsg.CatMsgBody( (u8*)&wForceCallInfo, sizeof(u16));


		u8 byAdminLevel = g_cMcuVcApp.GetCascadeAdminLevel();
		//vcs会议不感知行政级别,永远以最高等级去调度终端
		if( VCS_CONF == m_tConf.GetConfSource() || !m_tConf.GetConfAttrb().IsSupportCascade() )
		{
			byAdminLevel = MAX_ADMINLEVEL;
		}
		cServMsg.CatMsgBody( (u8*)&byAdminLevel, sizeof(u8));

		// Ex能力集需要带到本端Mtadp侧，避免出现本端Mtadp侧没有进行能力集更新的时候（即还没有Ex能力时），对端能力集到来，
		// 导致比出的共同双流能力有误
		TCapSupportEx tCapEx = m_tConf.GetCapSupportEx();
		cServMsg.CatMsgBody( (u8*)&tCapEx, sizeof(tCapEx));

		// 组织扩展能力集勾选给Mtadp [12/8/2011 chendaiwei]
		TVideoStreamCap atMSVideoCap[MAX_CONF_CAP_EX_NUM];
		u8 byCapNum = MAX_CONF_CAP_EX_NUM;
		m_tConfEx.GetMainStreamCapEx(atMSVideoCap,byCapNum);
		
		TVideoStreamCap atDSVideoCap[MAX_CONF_CAP_EX_NUM];
		u8 byDSCapNum = MAX_CONF_CAP_EX_NUM;
		m_tConfEx.GetDoubleStreamCapEx(atDSVideoCap,byDSCapNum);

		cServMsg.CatMsgBody((u8*)&atMSVideoCap[0], sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);
		cServMsg.CatMsgBody((u8*)&atDSVideoCap[0], sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);
		//TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
		//u8 byAudioTrackNum = 1;
		//cServMsg.CatMsgBody(&byAudioTrackNum,sizeof(byAudioTrackNum));

		TMtAlias tConfE164Alias;
		tConfE164Alias.SetE164Alias( m_tConf.GetConfE164() );
		cServMsg.CatMsgBody( (u8 *)&tConfE164Alias, sizeof(tConfE164Alias) );

		u8 byNPlusSmcuAliasFlag = 0; //Flag为1，标识N+1会议下级会议别名类型是E164号，但
		//实质是IP呼叫.[11/16/2012 chendaiwei]
		if(g_cMcuAgent.GetGkIpAddr()==0 
			&& m_byCreateBy == CONF_CREATE_NPLUS
			&& tMtAlias.m_AliasType == mtAliasTypeE164
			&& !tMtAlias.m_tTransportAddr.IsNull())
		{
			byNPlusSmcuAliasFlag = 1;
		}
		cServMsg.CatMsgBody( (u8 *)&byNPlusSmcuAliasFlag, sizeof(byNPlusSmcuAliasFlag) );

		TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];//音频能力
		//从会议属性中取出会议支持的音频类型
		m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);
		cServMsg.CatMsgBody((u8*)&atAudioTypeDesc[0], sizeof(TAudioTypeDesc)* MAXNUM_CONF_AUDIOTYPE);
		SendMsgToMt( ptMt->GetMtId(), MCU_MT_INVITEMT_REQ, cServMsg );

		if( TRUE == bSendAlert )
		{
			TMsgHeadMsg tHeadMsg;			
			
			// [12/8/2011 liuxu] 防止修改cServMsgHdr
			CServMsg cTempMsg;
			cTempMsg.SetServMsg( cServMsgHdr.GetServMsg(), SERV_MSGHEAD_LEN );
			cTempMsg.SetConfIdx( m_byConfIdx );
			cTempMsg.SetConfId( m_tConf.GetConfId() );

			TMt tInviteMt = *ptMt;
			tInviteMt.SetMcuId( LOCAL_MCUID );
			cTempMsg.SetMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
			cTempMsg.CatMsgBody( (u8*)&tInviteMt, sizeof( TMt ) );
			cTempMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
			if( bBrdSend )
			{
				BroadcastToAllMcu( MCU_MCU_CALLALERTING_NOTIF, cTempMsg );
			}
			else
			{
				SendReplyBack( cTempMsg, MCU_MCU_CALLALERTING_NOTIF );
			}
		}
	}
	//邀请所有终端，用于创会成功后邀请终端，不管终端呼叫模式怎样保证呼叫一次
	//fxh 但VCS只呼叫一次呼叫模式为定时呼叫的终端
	else
	{
		for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
		{
			TMt tInviteMt;
			if( !m_tConfAllMtInfo.MtJoinedConf( byLoop ) && m_tConfAllMtInfo.MtInConf( byLoop ) )
			{
				tInviteMt = m_ptMtTable->GetMt( byLoop );	
				if (!m_ptMtTable->GetDialAlias( tInviteMt.GetMtId(), &tMtAlias ))
                {
                    continue;
                }
  
                // guzh [5/23/2007] 如果终端创会则不再去呼叫他自己
                if( CONF_CREATE_MT == m_byCreateBy && byLoop == 1)
                {
					// xliang [1/16/2009]  创会终端此处会跳过，但是要补上NotInvited标志位，
					// 否则密码会议无效
					if( byLoop == m_byMtIdNotInvite)
					{
						m_ptMtTable->SetNotInvited( byLoop, TRUE );
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "Mt.%u is not invited!\n",m_byMtIdNotInvite);
					}
					else
					{
						m_ptMtTable->SetNotInvited( byLoop, FALSE );
					}
					continue;  
					
                }

				// fxh 对于VCS会议的整体呼叫，只呼叫呼叫模式为定时呼叫的终端
				// 对于手动呼叫实体，不进行自动呼叫
				if (VCS_CONF == m_tConf.GetConfSource() 
					&& m_ptMtTable->GetCallMode(byLoop) != CONF_CALLMODE_TIMER)
				{
					continue;
				}
				

				//呼叫H320终端
				if(mtAliasTypeH320ID == tMtAlias.m_AliasType )
				{
                    // xsl [11/8/2006] 分配320接入板id
                    u8 byDri = m_ptMtTable->GetDriId(tInviteMt.GetMtId());
                    if (!g_cMcuVcApp.IsMtAdpConnected(byDri) || !g_cMcuVcApp.IsMtAssignInDri(byDri, m_byConfIdx, tInviteMt.GetMtId()))
                    {
                        byDri = g_cMcuVcApp.AssignH320MtDriId(m_byConfIdx, tMtAlias, tInviteMt.GetMtId());
					    if( !g_cMcuVcApp.IsMtAdpConnected(byDri) || 
						    PROTOCOL_TYPE_H320 != g_cMcuVcApp.GetMtAdpProtocalType(byDri) )
					    {
						    cServMsg.SetDstMtId( tInviteMt.GetMtId() );
						    cServMsg.SetErrorCode( ERR_MCU_MTUNREACHABLE );
						    SendMsgToAllMcs( MCU_MCS_CALLMT_NACK, cServMsg );
						    continue;
					    }
                        m_ptMtTable->SetDriId(tInviteMt.GetMtId(), byDri);
                    }					
                    m_ptMtTable->SetProtocolType(tInviteMt.GetMtId(), PROTOCOL_TYPE_H320);
				}
				else
				{                  
					//获取终端对应别名
					if( m_tConf.m_tStatus.IsRegToGK() )
					{
						if( tInviteMt.GetMtType() == MT_TYPE_SMCU|| tInviteMt.GetMtType() == MT_TYPE_MMCU)
						{
							if( !m_ptMtTable->GetMtAlias( tInviteMt.GetMtId(), mtAliasTypeE164, &tMtAlias ) && 
								!m_ptMtTable->GetDialAlias( tInviteMt.GetMtId(), &tMtAlias )  )
							{
								continue;
							}
						}
						else
						{
							if( !m_ptMtTable->GetDialAlias( tInviteMt.GetMtId(), &tMtAlias ) )
							{
								continue;
							}
						}
					}
					else
					{
						if( m_byCreateBy == CONF_CREATE_NPLUS)
						{
							if(!m_ptMtTable->GetDialAlias( tInviteMt.GetMtId(), &tMtAlias ) )
							{
								continue;
							}
						}
						else if( !m_ptMtTable->GetMtAlias( tInviteMt.GetMtId(), mtAliasTypeTransportAddress, &tMtAlias ) )
						{
							continue;
						}
					}

                    // xsl [11/8/2006] 分配323接入板id
                    u8 byDri = m_ptMtTable->GetDriId(tInviteMt.GetMtId());
                    if ( !g_cMcuVcApp.IsMtAdpConnected(byDri) || !g_cMcuVcApp.IsMtAssignInDri(byDri, m_byConfIdx, tInviteMt.GetMtId()) )
                    {
                        byDri = g_cMcuVcApp.AssignH323MtDriId(m_byConfIdx, tMtAlias, tInviteMt.GetMtId());
                        if (!g_cMcuVcApp.IsMtAdpConnected(byDri))
                        {
                            g_cMcuVcApp.DecMtAdpMtNum(byDri, m_byConfIdx, tInviteMt.GetMtId());

                            if (byDri > MAXNUM_DRI || byDri == 0)
                            {
                                // guzh [3/1/2007] 一次性呼叫失败时暂时不给出提示
                            }
                            continue;
                        }
                        m_ptMtTable->SetDriId(tInviteMt.GetMtId(), byDri);
                    }                        
                    m_ptMtTable->SetProtocolType(tInviteMt.GetMtId(), PROTOCOL_TYPE_H323);
				}

				cServMsg.SetConfIdx( m_byConfIdx );

				//发送消息给终端适配会话
				// xliang [12/26/2008] modify：for MT call MCU initially. 
				if( tInviteMt.GetMtId() == m_byMtIdNotInvite)
				{
					m_ptMtTable->SetNotInvited( tInviteMt.GetMtId(), TRUE );
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "Mt.%u is not invited!\n",m_byMtIdNotInvite);
				}
				else
				{
					m_ptMtTable->SetNotInvited( tInviteMt.GetMtId(), FALSE );
				}
			
				cServMsg.SetMsgBody( (u8*)&tInviteMt, sizeof( tInviteMt ) );
				LogPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[InviteUnjoinedMt]: mt.%d aliasType.%d\n", tInviteMt.GetMtId(), tMtAlias.m_AliasType);
				cServMsg.CatMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
				cServMsg.CatMsgBody( (u8*)&tConfAlias, sizeof( tConfAlias ) );
				cServMsg.CatMsgBody( (u8*)&byEncrypt, sizeof(byEncrypt));
				//u16 wCallBand = htons( m_ptMtTable->GetDialBitrate( tInviteMt.GetMtId() ) + wAudioBand );
				u16 wCallBand = htons( m_ptMtTable->GetDialBitrate( tInviteMt.GetMtId() ) );
				cServMsg.CatMsgBody( (u8*)&wCallBand, sizeof(u16)  );
				
                TCapSupport tCapSupport = m_tConf.GetCapSupport();
                TSimCapSet tSim = tCapSupport.GetMainSimCapSet();
                tSim.SetVideoMaxBitRate(m_ptMtTable->GetDialBitrate( tInviteMt.GetMtId()));
                tCapSupport.SetMainSimCapSet(tSim);
                tSim = tCapSupport.GetSecondSimCapSet();
                if(!tSim.IsNull())
                {
                    tSim.SetVideoMaxBitRate(m_ptMtTable->GetDialBitrate( tInviteMt.GetMtId()));
                    tCapSupport.SetSecondSimCapSet(tSim);
                }		
                tCapSupport.SetDStreamMaxBitRate(m_ptMtTable->GetDialBitrate( tInviteMt.GetMtId()));

                cServMsg.CatMsgBody( (u8*)&tCapSupport, sizeof(tCapSupport));

				//强拆 
				u16 wForceCallInfo = MAKEWORD(m_tConf.GetConfLevel(), byCallType);
				wForceCallInfo = htons(wForceCallInfo);
				cServMsg.CatMsgBody( (u8*)&wForceCallInfo, sizeof(u16));

				u8 byAdminLevel = g_cMcuVcApp.GetCascadeAdminLevel();
				//vcs会议不感知行政级别,永远以最高等级去调度终端
				if( VCS_CONF == m_tConf.GetConfSource() || !m_tConf.GetConfAttrb().IsSupportCascade() )
				{
					byAdminLevel = MAX_ADMINLEVEL;
				}
				cServMsg.CatMsgBody( (u8*)&byAdminLevel, sizeof(u8));
				// Ex能力集需要带到本端Mtadp侧，避免出现本端Mtadp侧没有进行能力集更新的时候（即还没有Ex能力时），对端能力集到来，
				// 导致比出的共同双流能力有误
				TCapSupportEx tCapEx = m_tConf.GetCapSupportEx();
				cServMsg.CatMsgBody( (u8*)&tCapEx, sizeof(tCapEx));

				// 组织扩展能力集勾选给Mtadp [12/8/2011 chendaiwei]
				TVideoStreamCap atMSVideoCap[MAX_CONF_CAP_EX_NUM];
				u8 byCapNum = MAX_CONF_CAP_EX_NUM;
				m_tConfEx.GetMainStreamCapEx(atMSVideoCap,byCapNum);
				
				TVideoStreamCap atDSVideoCap[MAX_CONF_CAP_EX_NUM];
				u8 byDSCapNum = MAX_CONF_CAP_EX_NUM;
				m_tConfEx.GetDoubleStreamCapEx(atDSVideoCap,byDSCapNum);

				cServMsg.CatMsgBody((u8*)&atMSVideoCap[0], sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);
				cServMsg.CatMsgBody((u8*)&atDSVideoCap[0], sizeof(TVideoStreamCap)*MAX_CONF_CAP_EX_NUM);

				//TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
				//u8 byAudioTrackNum = 1;
				//cServMsg.CatMsgBody(&byAudioTrackNum,sizeof(byAudioTrackNum));

				TMtAlias tConfE164Alias;
				tConfE164Alias.SetE164Alias( m_tConf.GetConfE164() );
				cServMsg.CatMsgBody( (u8 *)&tConfE164Alias, sizeof(tConfE164Alias) );
				
				u8 byNPlusSmcuAliasFlag = 0; //Flag为1，标识N+1会议下级会议别名类型是E164号，但
				//实质是IP呼叫.[11/16/2012 chendaiwei]
				if(g_cMcuAgent.GetGkIpAddr()==0 
					&& m_byCreateBy == CONF_CREATE_NPLUS
					&& tMtAlias.m_AliasType == mtAliasTypeE164
					&& !tMtAlias.m_tTransportAddr.IsNull())
				{
					byNPlusSmcuAliasFlag = 1;
				}
				cServMsg.CatMsgBody((u8*)&byNPlusSmcuAliasFlag,sizeof(byNPlusSmcuAliasFlag));
				TAudioTypeDesc atAudioTypeDesc[MAXNUM_CONF_AUDIOTYPE];//音频能力
				m_tConfEx.GetAudioTypeDesc(atAudioTypeDesc);
				cServMsg.CatMsgBody((u8*)&atAudioTypeDesc[0], sizeof(TAudioTypeDesc)* MAXNUM_CONF_AUDIOTYPE);
				SendMsgToMt( tInviteMt.GetMtId(), MCU_MT_INVITEMT_REQ, cServMsg );					
			}
		}
	}
	
	g_cMcuVcApp.UpdateAgentAuthMtNum();
}

/*====================================================================
    函数名      ：ProcBuildCaseSpecialMessage
    功能        ：立案系统特殊消息处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    2.1         胡昌威         创建
====================================================================*/
void CMcuVcInst::ProcBuildCaseSpecialMessage(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TSwitchInfo tSwitchInfo;
    TMt tSrcMt,tDstMt;
    TMt tMediaSrcMt;
    u32  dwMultiCastAddr,dwRcvIp;
    u16  wMultiCastPort, wRcvPort;
    TTransportAddr tVidAddr, tAudAddr;

    TMediaEncrypt  tEncrypt = m_tConf.GetMediaKey();
    TSimCapSet     tSrcSCS; 
    TDoublePayload tDVPayload;
    TDoublePayload tDAPayload;

	u32 dwTimeIntervalTicks = 3*OspClkRateGet();

    //TMt(被组播的终端)+TTransportAddr(视频流组播地址)+TTransportAddr(音频流组播地址)
    switch(CurState())
    {
    case STATE_ONGOING:
        switch(pcMsg->event)
        {
        case MT_MCU_STARTBROADCASTMT_REQ://让MCU组播其它终端的命令

            tSrcMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
            tDstMt = *(TMt *)(cServMsg.GetMsgBody());
            m_ptMtTable->GetMtSwitchAddr(tSrcMt.GetMtId(), dwRcvIp, wRcvPort);

            // 得到MCU的组播地址
            // 顾振华@2006.4.6 这里要从会议里面取
            // dwMultiCastAddr = g_cMcuAgent.GetCastIpAddr();
            // wMultiCastPort = g_cMcuAgent.GetCastPort();
            dwMultiCastAddr = g_cMcuVcApp.AssignMulticastIp(m_byConfIdx);
            wMultiCastPort = g_cMcuVcApp.AssignMulticastPort(m_byConfIdx, tDstMt.GetMtId());

            tVidAddr.SetNetSeqIpAddr(dwMultiCastAddr);
            tVidAddr.SetPort(wMultiCastPort);
            tAudAddr.SetNetSeqIpAddr(dwMultiCastAddr);
            tAudAddr.SetPort(wMultiCastPort + 2);

            tSrcSCS = m_ptMtTable->GetSrcSCS(tSrcMt.GetMtId());
            if (MEDIA_TYPE_H264 == tSrcSCS.GetVideoMediaType() || 
                MEDIA_TYPE_H263PLUS == tSrcSCS.GetVideoMediaType() || 
                CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
                // zbq [10/29/2007] 适应FEC支持其他格式
                m_tConf.GetCapSupportEx().IsVideoSupportFEC())
            {
                tDVPayload.SetRealPayLoad(tSrcSCS.GetVideoMediaType());
                tDVPayload.SetActivePayload(GetActivePayload(m_tConf, tSrcSCS.GetVideoMediaType()));
            }
            else
            {
                tDVPayload.SetRealPayLoad(tSrcSCS.GetVideoMediaType());
                tDVPayload.SetActivePayload(tSrcSCS.GetVideoMediaType());
            }
            if(CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
               // zbq [10/29/2007] 适应FEC支持其他格式
               m_tConf.GetCapSupportEx().IsAudioSupportFEC() )
            {
                tDAPayload.SetRealPayLoad(tSrcSCS.GetAudioMediaType());
                tDAPayload.SetActivePayload(GetActivePayload(m_tConf, tSrcSCS.GetAudioMediaType()));
            }
            else
            {
                tDAPayload.SetRealPayLoad(tSrcSCS.GetAudioMediaType());
                tDAPayload.SetActivePayload(tSrcSCS.GetAudioMediaType());
            }

            cServMsg.CatMsgBody((u8 *)&tVidAddr, sizeof(tVidAddr));
            cServMsg.CatMsgBody((u8 *)&tAudAddr, sizeof(tAudAddr));
            cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));
            cServMsg.CatMsgBody((u8 *)&tDVPayload, sizeof(tDVPayload));
            cServMsg.CatMsgBody((u8 *)&tEncrypt, sizeof(tEncrypt));
            cServMsg.CatMsgBody((u8 *)&tDAPayload, sizeof(tDAPayload));
            SendReplyBack(cServMsg, pcMsg->event+1);

            //记录
            m_ptMtTable->AddMonitorSrcMt(tSrcMt.GetMtId(), tDstMt.GetMtId());
            m_ptMtTable->AddMonitorDstMt(tDstMt.GetMtId(), tSrcMt.GetMtId());

            //开始组播	
            if (!m_ptMtTable->IsMtMulticasting(tDstMt.GetMtId()))
            {
                //通知终端开始发送
                if (tDstMt.GetType() == TYPE_MT)
                {
                    NotifyMtSend(tDstMt.GetMtId());
                    NotifyFastUpdate(tDstMt, MODE_VIDEO);
                }
                //case 会议以终端区分多个组播端口,非case只有一个组播地址
                g_cMpManager.StartMulticast(tDstMt, 0, MODE_BOTH, FALSE);
                m_ptMtTable->SetMtMulticasting(tDstMt.GetMtId());
                //顾振华@2006.4.12 高法项目不支持卫星组播
            }
            break;

        case MT_MCU_STOPBROADCASTMT_CMD://让MCU停止组播其它终端的命令

            tSrcMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
            tDstMt = *(TMt*)( cServMsg.GetMsgBody() );

            //记录
            m_ptMtTable->RemoveMonitorSrcMt( tSrcMt.GetMtId(), tDstMt.GetMtId() );
            m_ptMtTable->RemoveMonitorDstMt( tDstMt.GetMtId(), tSrcMt.GetMtId() );

            if( !m_ptMtTable->HasMonitorDstMt( tDstMt.GetMtId() ) )
            {
                g_cMpManager.StopMulticast( tDstMt, 0, MODE_BOTH, FALSE );	
                //顾振华@2006.4.13 高法项目不支持卫星组播
                m_ptMtTable->SetMtMulticasting( tDstMt.GetMtId(), FALSE );
            }
            break;

        case MT_MCU_STARTMTSELME_REQ://让其它终端选看自己的请求

            tSwitchInfo = *(TSwitchInfo *)cServMsg.GetMsgBody();
            tSrcMt = tSwitchInfo.GetSrcMt();
            tDstMt = tSwitchInfo.GetDstMt();

            m_ptMtTable->GetMtSrc(tDstMt.GetMtId(), &tMediaSrcMt, MODE_VIDEO);

            if (0 == tMediaSrcMt.GetMtId())
            {
                //通知终端开始发送
                if (tSrcMt.IsLocal() && tSrcMt.GetType() == TYPE_MT)
                {
                    NotifyMtSend(tSrcMt.GetMtId());
                }
					
				//zjl 20110510 StartSwitchToAll 替换 StartSwitchToSubMt
                //StartSwitchToSubMt(tSrcMt, 0, tDstMt.GetMtId(), MODE_BOTH, SWITCH_MODE_SELECT);
				TSwitchGrp tSwitchGrp;
				tSwitchGrp.SetSrcChnl(0);
				tSwitchGrp.SetDstMtNum(1);
				tSwitchGrp.SetDstMt(&tDstMt);
				StartSwitchToAll(tSrcMt, 1, &tSwitchGrp, MODE_BOTH, SWITCH_MODE_SELECT);

                //ACK				 
                SendReplyBack(cServMsg, pcMsg->event+1);
            }
            else
            {
                //NACK
                SendReplyBack(cServMsg, pcMsg->event+2);
            }
            break;

        case MT_MCU_GETMTSELSTUTS_REQ://得到其它终端选看情况的请求

            tDstMt = *(TMt *)(cServMsg.GetMsgBody()+sizeof(TMt));

            m_ptMtTable->GetMtSrc(tDstMt.GetMtId(), &tSrcMt, MODE_VIDEO);
            cServMsg.CatMsgBody((u8*)&tSrcMt, sizeof(TMt));

            //ACK				 
            SendReplyBack(cServMsg, pcMsg->event+1);

            break;

		default:
			break;
        }
        break;
    default:
        break;
    }
}

/*====================================================================
    函数名      : ProcMtMcuGetChairmanReq
    功能        ：查询会议中主席
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/18    3.0         JQL           创建
====================================================================*/
void CMcuVcInst::ProcMtMcuGetChairmanReq( const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt      tMt;
	u16      wEvent;

	switch( CurState() )
	{
	case STATE_ONGOING:
		
		if( m_tConf.HasChairman() )
		{
			//设置主席终端相关信息
			TMtAlias tMtAlias;
			tMt.SetMt( m_tConf.GetChairman().GetMcuId(), m_tConf.GetChairman().GetMtId() );
			if( !m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH323ID, &tMtAlias ) )
			{
				if(!m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH320Alias, &tMtAlias ))
                {
                    m_ptMtTable->GetMtAlias( tMt.GetMtId(), mtAliasTypeH320ID, &tMtAlias );
                }
			}
			
			cServMsg.SetMsgBody( ( u8* )&tMt, sizeof( tMt ) );
			cServMsg.CatMsgBody( ( u8* )&tMtAlias, sizeof( tMtAlias ) );
			
			wEvent = pcMsg->event + 1;		
		}
		else
			wEvent = pcMsg->event + 2;

		//回复响应
		SendReplyBack( cServMsg, wEvent );
		break;
		
	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
	}
}

/*====================================================================
    函数名      : ProcMcuMtFastUpdatePic
    功能        ：快速更新图像
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/13    胡昌威        JQL           创建
====================================================================*/
// void CMcuVcInst::ProcMcuMtFastUpdatePic( const CMessage * pcMsg )
// {
//     //CServMsg cServMsg( pcMsg->content, pcMsg->length );
//     return;
// }

/*====================================================================
    函数名      ：ProcMtMcuVidAliasNtf
    功能        ：终端视频源别名通知处理
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	201104112   4.6         pengjie		   create
====================================================================*/
void CMcuVcInst::ProcMtMcuVidAliasNtf( const CMessage * pcMsg )
{
	STATECHECK;
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	u8 byMtId = cServMsg.GetSrcMtId();
	u8 byMtVidPortNum = *(u8 *)(cServMsg.GetMsgBody());
	u8 *pbyVidInfo = (u8 *)(cServMsg.GetMsgBody() + sizeof(u8) );

	if( !m_tConfAllMtInfo.MtJoinedConf( byMtId ) )
	{
		return;
	}

	CServMsg cSendtoMcsMsg;
	u8 byMtNum = 1;
	cSendtoMcsMsg.SetMsgBody( (u8 *)&byMtNum, sizeof(u8) );

	TMt tMt = m_ptMtTable->GetMt( byMtId );
	cSendtoMcsMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt) );

	TVidSrcAliasInfo atVidSrcAliasInfo[MT_MAXNUM_VIDSOURCE];
	byMtVidPortNum = min(byMtVidPortNum, MT_MAXNUM_VIDSOURCE);
	cSendtoMcsMsg.CatMsgBody( (u8 *)&byMtVidPortNum, sizeof(u8) );

	for( u8 byLoop = 0; byLoop < byMtVidPortNum; byLoop++ )
	{
		// 1、先取视频源端口索引
		atVidSrcAliasInfo[byLoop].byVidPortIdx = *pbyVidInfo;
		cSendtoMcsMsg.CatMsgBody( (u8 *)&(atVidSrcAliasInfo[byLoop].byVidPortIdx), sizeof(u8) );

		// 2、再取别名长度
		u8 byVidAliasLen = *(pbyVidInfo + sizeof(u8));
		cSendtoMcsMsg.CatMsgBody( (u8 *)&byVidAliasLen, sizeof(u8) );

		// 3、再取别名字段
		memcpy( atVidSrcAliasInfo[byLoop].achVidAlias, (s8 *)(pbyVidInfo + sizeof(u8) * 2), byVidAliasLen );
		cSendtoMcsMsg.CatMsgBody( (u8 *)atVidSrcAliasInfo[byLoop].achVidAlias, byVidAliasLen );

		// 4、偏移到下一个视频源别名信息地址
		pbyVidInfo = pbyVidInfo + (sizeof(u8) * 2) + byVidAliasLen;
	}

	if( byMtVidPortNum != 0 )
	{
		// 存储
		m_ptMtTable->SetMtVidAlias( byMtId, byMtVidPortNum, atVidSrcAliasInfo );
		// 通知界面
        SendMsgToAllMcs( MCU_MCS_MTVIDEOALIAS_NOTIF, cSendtoMcsMsg );
	}

	return;
}

/*====================================================================
    函数名      ：ProcMcsMcuGetMtVidAliaseReq
    功能        ：mcs请求终端视频源别名信息
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	201104112   4.6         pengjie		   create
====================================================================*/
void CMcuVcInst::ProcMcsMcuGetMtVidAliaseReq( const CMessage * pcMsg )
{
	STATECHECK;
	
	CServMsg cServMsg( pcMsg->content, pcMsg->length );

	u8 byMtNum = *(u8 *)(cServMsg.GetMsgBody());
	TMt *ptMt = (TMt *)(cServMsg.GetMsgBody() + sizeof(u8) );

	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

	CServMsg cSendtoMcsMsg;
	cSendtoMcsMsg.SetMsgBody( (u8 *)&byMtNum, sizeof(u8) );
	
	// 1、组织终端视频源别名信息
	u8 byOnlinMtNum = 0;
	if( byMtNum == 0 ) // 1.1 请求所以终端
	{
		for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++ )
		{
			if( m_tConfAllMtInfo.MtJoinedConf( byMtId ) ) // 在线终端
			{
				//SendMsgToMt( byMtId, MCU_MT_GETMTVIDEOALIAS_CMD, cSendMsg );
				TMt tMt = m_ptMtTable->GetMt( byMtId );
				cSendtoMcsMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt) );
				u8 byMtVidNum = m_ptMtTable->GetMtVidPortNum( byMtId );
				cSendtoMcsMsg.CatMsgBody( (u8 *)&byMtVidNum, sizeof(u8) );

				for( u8 byLoop = 0; byLoop < byMtVidNum; byLoop++ )
				{
					TVidSrcAliasInfo tVidAliasInfo = m_ptMtTable->GetMtVidAliasbyVidIdx( byMtId, byLoop );
					cSendtoMcsMsg.CatMsgBody( (u8 *)&tVidAliasInfo.byVidPortIdx, sizeof(u8) );

					const u8 byAcutualLen = strlen(tVidAliasInfo.achVidAlias);
					u8 byAliasLen = min( byAcutualLen, MT_MAX_PORTNAME_LEN );
					cSendtoMcsMsg.CatMsgBody( (u8 *)&byAliasLen, sizeof(byAliasLen) );
					cSendtoMcsMsg.CatMsgBody( (u8 *)tVidAliasInfo.achVidAlias, byAliasLen );
				}
				byOnlinMtNum++;
			}
		}

	}
	else       // 1.2 请求部分终端
	{
		for( u8 byLoop = 0; byLoop < byMtNum; byLoop++ )
		{
			if( ptMt != NULL && !ptMt->IsNull() && m_tConfAllMtInfo.MtJoinedConf( *ptMt ) )
			{
				if( ptMt->IsLocal() )
				{
					//SendMsgToMt( ptMt->GetMtId(), MCU_MT_GETMTVIDEOALIAS_CMD, cSendMsg );
					cSendtoMcsMsg.CatMsgBody( (u8 *)ptMt, sizeof(TMt) );
					u8 byMtVidNum = m_ptMtTable->GetMtVidPortNum( ptMt->GetMtId() );
					cSendtoMcsMsg.CatMsgBody( (u8 *)&byMtVidNum, sizeof(u8) );
					
					for( u8 byLoop2 = 0; byLoop2 < byMtVidNum; byLoop2++ )
					{
						TVidSrcAliasInfo tVidAliasInfo = m_ptMtTable->GetMtVidAliasbyVidIdx( ptMt->GetMtId(), byLoop2 );
						cSendtoMcsMsg.CatMsgBody( (u8 *)&tVidAliasInfo.byVidPortIdx, sizeof(u8) );

						const u8 byRealLen = strlen(tVidAliasInfo.achVidAlias);
						u8 byAliasLen = min( byRealLen, MT_MAX_PORTNAME_LEN );
						cSendtoMcsMsg.CatMsgBody( (u8 *)&byAliasLen, sizeof(byAliasLen) );
						cSendtoMcsMsg.CatMsgBody( (u8 *)tVidAliasInfo.achVidAlias, byAliasLen );
					}
				}
				else //请求级联的下级终端
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,"[ProcMcsMcuGetMtVidAliaseReq] Get mcuid.%dmtid%d VidSrcAlias !\n", \
						ptMt->GetMcuId(), ptMt->GetMtId() );
				}
				byOnlinMtNum++;
			}
			
			if(ptMt) ptMt++;
		}
	}

	// 2、上报mcs
	if( byOnlinMtNum != 0 ) //更新为实际终端数量
	{
		u8 *pbyMtNum = cSendtoMcsMsg.GetMsgBody();
		*pbyMtNum = byOnlinMtNum;
		SendMsgToAllMcs( MCU_MCS_MTVIDEOALIAS_NOTIF, cSendtoMcsMsg );
	}

	return;
}

/*====================================================================
    函数名      ：ProcMcsMcuGetMtStatusReq
    功能        ：查询所有终端状态处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/06    1.0         LI Yi         创建
	02/11/01	1.1			Liaoweijiang  修改
	04/05/10	3.0			胡昌威        修改
====================================================================*/
void CMcuVcInst::ProcMcsMcuGetMtStatusReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMtStatus	tMtStatus;
	TMt			tMt = *( TMt* )cServMsg.GetMsgBody();

	switch( CurState() )
	{
	case STATE_ONGOING:

		//应答
		if( m_tConfAllMtInfo.MtInConf( tMt.GetMtId() ) && 
			m_ptMtTable->GetMtStatus( tMt.GetMtId(), &tMtStatus ) )
		{            
			tMtStatus.SetTMt( tMt );
			cServMsg.SetMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		}
		else
		{
			cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            SendMsgToMt( tMt.GetMtId(), MCU_MT_GETMTSTATUS_REQ, cServMsg );
		}
		
		//查询终端状态
		//SendMsgToMt( tMt.GetMtId(), MCU_MT_GETMTSTATUS_REQ, cServMsg );

		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
	}
}

/*====================================================================
    函数名      ；ProcMcsMcuGetAllMtStatusReq
    功能        ：查询所有终端状态处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/04/27	3.0			胡昌威         修改
====================================================================*/
void CMcuVcInst::ProcMcsMcuGetAllMtStatusReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMcu tMcu;
	u8 byLoop;

	switch( CurState() )
	{
	case STATE_ONGOING: 
		
		//是否在本级上增加
		tMcu = *(TMcu*)cServMsg.GetMsgBody(); 

		//所有终端状态
		if( tMcu.GetMcuIdx() == INVALID_MCUIDX )
		{
			//先发其它MC
			for( u16 wLoop = 0; wLoop < m_tConfAllMtInfo.GetMaxMcuNum(); wLoop++ )
			{
				u16 wMcuIdx = m_tConfAllMtInfo.GetMtInfo(wLoop).GetMcuIdx();
				if( !IsValidMcuId( wMcuIdx ))
				{
					continue;
				}

				TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo( wMcuIdx );
				if(ptConfMcInfo == NULL)
				{
					continue;
				}
				tMcu.SetNull();
				tMcu.SetMcu( 0 );	
				tMcu.SetMcuIdx( wMcuIdx );	
				cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(TMcu) );
				
				for(s32 nLoop =0; nLoop<MAXNUM_CONF_MT; nLoop++)
				{
					if(ptConfMcInfo->m_atMtStatus[nLoop].IsNull()|| 
						ptConfMcInfo->m_atMtStatus[nLoop].GetMtId() == 0) //自己
					{
						continue;
					}
					TMtStatus tMtStatus = ptConfMcInfo->m_atMtStatus[nLoop].GetMtStatus();
					cServMsg.CatMsgBody((u8 *)&tMtStatus, sizeof(TMtStatus));
				}
				SendReplyBack( cServMsg, MCU_MCS_ALLMTSTATUS_NOTIF );
			}

			//本级的最后发
			SendAllLocalMtStatus( MCU_MCS_ALLMTSTATUS_NOTIF );
		}		

		//不是本级的MCU
		cServMsg.SetMsgBody( (u8*)&tMcu,sizeof(tMcu) );

		if( tMcu.IsLocal())
		{
			TMtStatus tMtStatus;
			//得到状态列表
			for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
			{
				if( m_tConfAllMtInfo.MtInConf( byLoop ) && 
					m_ptMtTable->GetMtStatus( byLoop, &tMtStatus ) ) 
				{
					tMtStatus.SetTMt( m_ptMtTable->GetMt( tMtStatus.GetMtId() ) );
					cServMsg.CatMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );
				}
			}
		}
		else
		{
			u16 wMcuIdx = tMcu.GetMcuIdx();
			if( !m_tConfAllMtInfo.m_tLocalMtInfo.MtJoinedConf( GetFstMcuIdFromMcuIdx( wMcuIdx ) ) )
			{
				cServMsg.SetErrorCode( ERR_MCU_THISMCUNOTJOIN );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}

			TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo( wMcuIdx );
			if (NULL == ptConfMcInfo)
			{
				cServMsg.SetErrorCode( ERR_MCU_THISMCUNOTJOIN );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}

			cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(TMcu) );
			for(s32 nLoop =0; nLoop<MAXNUM_CONF_MT; nLoop++)
			{
				if( ptConfMcInfo->m_atMtStatus[nLoop].IsNull()|| 
					ptConfMcInfo->m_atMtStatus[nLoop].GetMtId() == 0) //自己
				{
					continue;
				}
				TMtStatus tMtStatus = ptConfMcInfo->m_atMtStatus[nLoop].GetMtStatus();
                cServMsg.CatMsgBody((u8*)&tMtStatus, sizeof(TMtStatus));
			}
		}	

		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 ); 

		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
	}
}

/*=============================================================================
    函 数 名： MtVideoSourceSwitched
    功    能： 处理终端的视频源变化
    算法实现： 
    全局变量： 
    参    数： CServMsg & cServMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/04/06  3.6			李博                   创建
=============================================================================*/
void CMcuVcInst::MtVideoSourceSwitched(CServMsg & cServMsg)
{
    //CServMsg	cServMsg( pcMsg->content, pcMsg->length );
    u8 bySrcMtId = cServMsg.GetSrcMtId();
    TMt tMt = *(TMt*)cServMsg.GetMsgBody();
    TMt	tDstMt;
    u16	wEvent;

    if (STATE_ONGOING == CurState())
    {
        u8 byCurrVidNo = *(cServMsg.GetMsgBody() + sizeof(TMt));

        m_ptMtTable->SetCurrVidSrcNo(bySrcMtId, byCurrVidNo);

        if (m_tConf.HasChairman())
        {
            TMt tSpeakerMt = m_tConf.GetSpeaker();
            TMt tChairmanMt = m_tConf.GetChairman();
            if (tSpeakerMt.GetMtId() == bySrcMtId)
            {
                tDstMt = m_tConf.GetChairman();
                wEvent = MCU_MT_VIDEOSOURCESWITCHED_CMD;
            }
            else if (tChairmanMt.GetMtId() == bySrcMtId)
            {
                tDstMt = m_tConf.GetSpeaker();
                wEvent = MCU_MT_VIDEOSOURCESWITCHED_CMD;
            }
            else
            {
                return;
            }
        }
        else
        {
            return;
        }

        //send messge
        cServMsg.SetDstMtId( tDstMt.GetMtId() );
        SendMsgToMt( tDstMt.GetMtId(), wEvent, cServMsg );
    
    }
    else
    {
        ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
                        cServMsg.GetEventId(), ::OspEventDesc(cServMsg.GetEventId()), CurState());
    }
}

/*=============================================================================
    函 数 名： ProcMtMcuVideoSourceSwitched
    功    能： 处理终端的视频源变化
    算法实现： 
    全局变量： 
    参    数： CServMsg & cServMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/04/06  3.6			李博                   创建
=============================================================================*/
void CMcuVcInst::ProcMtMcuVideoSourceSwitched(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    MtVideoSourceSwitched(cServMsg);
}

/*====================================================================
    函数名      ：ProcMcsMcuCamCtrlCmd
    功能        ：终端操作统一处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/04/02    3.0         胡昌威        创建
    2006.4.18   4.0         顾振华        有关限制代码调整
====================================================================*/
void CMcuVcInst::ProcMcsMcuCamCtrlCmd( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u8 bySrcMtId = cServMsg.GetSrcMtId();
	TMt tMt = *(TMt*)cServMsg.GetMsgBody();
	TMt	tDstMt;
	u16	wEvent;

	// [pengjie 2010/8/12] 级联远摇
	u8 byFeccParam  = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt));
	// End

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcsMcuCamCtrlCmd] step 1 bySrcMtId.%d - MtMcuId.%d MtMtId.%d\n", 
			 bySrcMtId, tMt.GetMcuId(), tMt.GetMtId() );

	switch( cServMsg.GetEventId() )
	{
		case MT_MCU_MTCAMERA_CTRL_CMD:		    //主席命令终端摄像头移动
		case MT_MCU_MTCAMERA_CTRL_STOP:		    //主席命令终端摄像头停止移动
		case MT_MCU_MTCAMERA_RCENABLE_CMD:	    //主席命令终端摄像头遥控器使能
		case MT_MCU_MTCAMERA_MOVETOPOS_CMD:	    //主席命令终端摄像头调整到指定位置
		case MT_MCU_MTCAMERA_SAVETOPOS_CMD:	    //主席命令终端摄像头保存到指定位置
		case MT_MCU_SELECTVIDEOSOURCE_CMD:           //会控要求MCU设置终端视频源
			tMt = m_ptMtTable->GetMt( tMt.GetMtId() );
			break;
		default:
			break;
	}
	

	switch( CurState() )
	{
	case STATE_ONGOING:
        // 不是本级MCU下的终端
        if(!tMt.IsLocal()&& (!tMt.IsNull()) )
		{
            if ( bySrcMtId != 0 )
            {
                // 需求：各级MCU上的主席只能对本级MCU下的终端进行控制，不能跨级控制
                // 终端没有权限切换下级MCU上终端的视频源
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcsMcuCamCtrlCmd] MT.%d no permission to operate, return\n", 
		                 bySrcMtId );					
                return;                
            }
// 			// 修改为遥控对方MCU
// 			tDstMt.SetMtId(tMt.GetMcuId());
// 			tDstMt.SetMcuId(LOCAL_MCUID);
			// [pengjie 2010/8/12] 级联远摇下级终端支持
			else
			{
				if(IsLocalAndSMcuSupMultSpy( tMt.GetMcuId() ))
				{
					tDstMt = m_ptMtTable->GetMt( GetFstMcuIdFromMcuIdx(tMt.GetMcuId()) );
				}
				else
				{
					tDstMt.SetMtId(GetFstMcuIdFromMcuIdx(tMt.GetMcuId()));
					tDstMt.SetMcuId(LOCAL_MCUID);
				}
			}
		}
        // 如果是终端消息
		else if( bySrcMtId != 0 )
		{	          
			if( tMt.IsNull() )
			{
				m_ptMtTable->GetMtSrc( bySrcMtId, &tDstMt, MODE_VIDEO );
				if( !tDstMt.IsNull() )
				{
					if( tDstMt.IsLocal() )
					{
						if( tDstMt.GetMtId() == bySrcMtId && MT_MANU_KDCMCU == m_ptMtTable->GetManuId(bySrcMtId))
						{
							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcsMcuCamCtrlCmd] bySrcMtId(MCU).%d src is self.return\n", bySrcMtId );
							return;
						}
						else
						{
							tDstMt = m_ptMtTable->GetMt( tDstMt.GetMtId() );
						}
							
					}
					else
					{
						if(IsLocalAndSMcuSupMultSpy( tDstMt.GetMcuId() ))
						{
							tMt = tDstMt;
							tDstMt = m_ptMtTable->GetMt( GetFstMcuIdFromMcuIdx(tDstMt.GetMcuId()) );
						}
						else
						{
							tDstMt.SetMtId(GetFstMcuIdFromMcuIdx(tDstMt.GetMcuId()));
							tDstMt.SetMcuId(LOCAL_MCUID);
						}
					}					
				}
			}
			else
			{
                tDstMt = m_ptMtTable->GetMt( tMt.GetMtId() );

				if(tDstMt.IsNull())
				{
					m_ptMtTable->GetMtSrc( bySrcMtId, &tDstMt, MODE_VIDEO );
					if(tDstMt.IsNull())
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcsMcuCamCtrlCmd] bySrcMtId.%d tDstMt.IsNull() return\n", bySrcMtId );

						return;
					}
				}                
			}

			// 终端远遥，要判别身份, 主席/上级mcu/简单级联MCU，可远遥

			if( // 来自本级主席
                bySrcMtId != m_tConf.GetChairman().GetMtId() && 
                // 合并级联时来自上级MCU
				bySrcMtId != m_tCascadeMMCU.GetMtId()        &&
                // 简单级联时来自MCU
                MT_MANU_KDCMCU != m_ptMtTable->GetManuId(bySrcMtId) )
			{
                // 如果是普通终端，则看他是否在选看要远遥的终端
                TMtStatus tMtStatus;

                if ( m_ptMtTable->GetMtStatus( bySrcMtId, &tMtStatus) )
                {
                    if ( ( GetLocalSpeaker().GetMtId() == tDstMt.GetMtId() && GetLocalSpeaker().GetType() != TYPE_MCUPERI ) ||
                         tMtStatus.GetVideoMt().GetMtId() != tDstMt.GetMtId() )
                    {
                        // 不在选看本终端，不能遥控
				        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcsMcuCamCtrlCmd] bySrcMtId.%d is not chairman or inspecter to Dst%d, return\n", 
						         bySrcMtId, tDstMt.GetMtId() );

                        // 通知他不是主席
                        SendReplyBack( cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF );

                        return;
                    }                    
                }               
			}

            // 如果合并级联要操作上级，拒绝
            if ( !m_tCascadeMMCU.IsNull() &&
                 tDstMt.GetMtId() == m_tCascadeMMCU.GetMtId() )
            {
				ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU,  "[ProcMcsMcuCamCtrlCmd] cannot operate mmcu, return\n" );
				return;
            }
			
		}
		else
		{
			// 会控远遥 － bySrcMtId == 0
			if( !tMt.IsNull())
			{
				tDstMt = m_ptMtTable->GetMt( tMt.GetMtId() );
			}
			else
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcsMcuCamCtrlCmd] bySrcMtId.%d tDstMt.IsNull() return\n", bySrcMtId );

				return;
			}
            // guzh [6/7/2007] 如果合并级联要操作上级，拒绝
            if ( !m_tCascadeMMCU.IsNull() &&
                tDstMt.GetMtId() == m_tCascadeMMCU.GetMtId() )
            {
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcsMcuCamCtrlCmd] cannot operate mmcu, return\n" );
                return;
            }
		}

		//not joined conference
		if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcsMcuCamCtrlCmd] step 3 bySrcMtId.%d - event.%d DstMcuId.%d DstMtId.%d\n", 
				     bySrcMtId, pcMsg->event, tDstMt.GetMcuId(), tDstMt.GetMtId() );

			return;
		}

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcsMcuCamCtrlCmd] step 4 bySrcMtId.%d - event.%d DstMcuId.%d DstMtId.%d\n", 
				  bySrcMtId, pcMsg->event, tDstMt.GetMcuId(), tDstMt.GetMtId() );

		switch( pcMsg->event )
		{
		//终端摄像头控制
		case MCS_MCU_MTCAMERA_CTRL_CMD:		//终端摄像头移动
		case MT_MCU_MTCAMERA_CTRL_CMD:		//主席命令终端摄像头移动
			wEvent = MCU_MT_MTCAMERA_CTRL_CMD;
			break;
		case MCS_MCU_MTCAMERA_CTRL_STOP:		//终端摄像头停止移动
		case MT_MCU_MTCAMERA_CTRL_STOP:		//主席命令终端摄像头停止移动
			wEvent = MCU_MT_MTCAMERA_CTRL_STOP;
			break;
		case MCS_MCU_MTCAMERA_RCENABLE_CMD:	//终端摄像头遥控器使能
		case MT_MCU_MTCAMERA_RCENABLE_CMD:	//主席命令终端摄像头遥控器使能
			wEvent = MCU_MT_MTCAMERA_RCENABLE_CMD;
			break;
		case MCS_MCU_MTCAMERA_MOVETOPOS_CMD:	//终端摄像头调整到指定位置
		case MT_MCU_MTCAMERA_MOVETOPOS_CMD:	//主席命令终端摄像头调整到指定位置
			wEvent = MCU_MT_MTCAMERA_MOVETOPOS_CMD;
			break;
		case MCS_MCU_MTCAMERA_SAVETOPOS_CMD:	//终端摄像头保存到指定位置
		case MT_MCU_MTCAMERA_SAVETOPOS_CMD:	//主席命令终端摄像头保存到指定位置
			wEvent = MCU_MT_MTCAMERA_SAVETOPOS_CMD;
			break;
        case MCS_MCU_SETMTVIDSRC_CMD:           //选择终端视频源
        // libo [4/4/2005]
        case MT_MCU_SELECTVIDEOSOURCE_CMD:
        // libo [4/4/2005]end
            wEvent = MCU_MT_SETMTVIDSRC_CMD;
			break;
		default:
			return;
		}

		// send messge
		if( bySrcMtId != 0 && bySrcMtId == m_tCascadeMMCU.GetMtId() && 
			bySrcMtId == tDstMt.GetMtId() )
		{
			//对于上级mcu远遥下级mcu终端，但回传通道为上级mcu时，过滤此请求
            // remark by 顾振华：似乎现在在上面就能过滤掉。但是先保留
		}
		else
		{
			// [pengjie 2010/8/12] 级联远摇支持
			cServMsg.SetDstMtId( tDstMt.GetMtId() );
			if( !tMt.IsNull() && !tMt.IsLocal() && IsLocalAndSMcuSupMultSpy( tMt.GetMcuId() ) )
			{
				TMsgHeadMsg tHeadMsg;
				TMt tMcuMt;
				tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt, tMcuMt );
				u16 wNetEventId = htons(wEvent);
				cServMsg.SetEventId( MCU_MCU_FECC_CMD );
				cServMsg.SetMsgBody();
				cServMsg.SetMsgBody( (u8 *)&tMcuMt, sizeof(tMcuMt) );
				cServMsg.CatMsgBody( (u8 *)&byFeccParam, sizeof(byFeccParam) );
				cServMsg.CatMsgBody( (u8 *)&(wNetEventId), sizeof(wNetEventId) );
				cServMsg.CatMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );

				SendMsgToMt( (u8)tMcuMt.GetMcuId(), cServMsg.GetEventId(), cServMsg);
			}
			else
			{
				SendMsgToMt( tDstMt.GetMtId(), wEvent, cServMsg );
			}
			// End
		}
		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMMcuMcuCamCtrlCmd
    功能        ：上级MCU控制本级终端摄像头
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/04/02    3.0         胡昌威        创建
	20100812    4.6         pengjie	      级联远摇支持
====================================================================*/
void CMcuVcInst::ProcMMcuMcuCamCtrlCmd( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	TMt tMt = *(TMt *)(cServMsg.GetMsgBody());
	u8 byFeccParam = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt));
	u16 wFeccEventId = *(u16 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8));
	wFeccEventId = ntohs(wFeccEventId);

	if( !tMt.IsMcuIdLocal())
	{
		TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) + sizeof(u16));
		TMt tDstMt = GetMtFromMultiCascadeMtInfo(tHeadMsg.m_tMsgDst, tMt);

		if (IsLocalAndSMcuSupMultSpy(tDstMt.GetMcuId()))
		{
			cServMsg.SetEventId( MCU_MCU_FECC_CMD );
			SendMsgToMt( u8(tMt.GetMcuId()), MCU_MCU_FECC_CMD, cServMsg );
			
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMMcuMcuCamCtrlCmd] Unlocal tmt, send MCU_MCU_FECC_CMD to smcu!\n" );
		}
		else
		{		
			cServMsg.SetEventId( wFeccEventId );
			SendMsgToMt( GetFstMcuIdFromMcuIdx(tDstMt.GetMcuIdx()), wFeccEventId, cServMsg );

			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMMcuMcuCamCtrlCmd] Unlocal tmt, send .%s to smcu.%d!\n", OspEventDesc(wFeccEventId), GetFstMcuIdFromMcuIdx(tDstMt.GetMcuIdx()));
		}		
		return;
	}

// 	TMt tDstMt = *(TMt *)cServMsg.GetMsgBody();
// 	u8 byFeccParam = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt));
// 	u16 wFeccEventId = *(u16 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8));
// 	wFeccEventId = ntohs(wFeccEventId);

	SendMsgToMt( tMt.GetMtId(), wFeccEventId, cServMsg );

	return;
}

/*====================================================================
    函数名      ：ProcMtMcuMatrixMsg
    功能        ：终端矩阵操作应答消息
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/03/08    3.0         胡昌威        创建
====================================================================*/
void CMcuVcInst::ProcMtMcuMatrixMsg( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u8 bySrcMtId = cServMsg.GetSrcMtId();
	u16	wEvent = 0;
	TMt tSrcMt;
	CServMsg cMsg( pcMsg->content, pcMsg->length );

	switch( CurState() )
	{
	case STATE_ONGOING:

		switch( pcMsg->event )
		{	
	    //内置矩阵
        case MT_MCU_MATRIX_ALLSCHEMES_NOTIF:    
            wEvent = MCU_MCS_MATRIX_ALLSCHEMES_NOTIF;
            break;

        case MT_MCU_MATRIX_ONESCHEME_NOTIF:
            wEvent = MCU_MCS_MATRIX_ONESCHEME_NOTIF;
            break;

        case MT_MCU_MATRIX_SAVESCHEME_NOTIF:
            wEvent = MCU_MCS_MATRIX_SAVESCHEME_NOTIF;
            break;

        case MT_MCU_MATRIX_SETCURSCHEME_NOTIF:
            wEvent = MCU_MCS_MATRIX_SETCURSCHEME_NOTIF;
            break;

        case MT_MCU_MATRIX_CURSCHEME_NOTIF:
            wEvent = MCU_MCS_MATRIX_CURSCHEME_NOTIF;
            break;

            //外置矩阵
        case MT_MCU_EXMATRIXINFO_NOTIFY:  
            wEvent = MCU_MCS_EXMATRIXINFO_NOTIFY;
            break;
        
        case MT_MCU_EXMATRIX_GETPORT_NOTIF:
            wEvent = MCU_MCS_EXMATRIX_GETPORT_NOTIF;
            break;        
     
        case MT_MCU_EXMATRIX_PORTNAME_NOTIF:
            wEvent = MCU_MCS_EXMATRIX_PORTNAME_NOTIF;
            break;
        case MT_MCU_EXMATRIX_ALLPORTNAME_NOTIF:
            wEvent = MCU_MCS_EXMATRIX_ALLPORTNAME_NOTIF;
            break;

            //扩展视频源
        case MT_MCU_ALLVIDEOSOURCEINFO_NOTIF:
            wEvent = MCU_MCS_ALLVIDEOSOURCEINFO_NOTIF;
            break;
        case MT_MCU_VIDEOSOURCEINFO_NOTIF:
            wEvent = MCU_MCS_VIDEOSOURCEINFO_NOTIF;
            break;
		
        case MT_MCU_EXMATRIX_GETPORT_ACK:
        case MT_MCU_EXMATRIX_GETPORT_NACK:
			return;
			
		default:
			return;
		}

		//send messge
		tSrcMt = m_ptMtTable->GetMt( bySrcMtId );
		cMsg.SetMsgBody( (u8*)&tSrcMt, sizeof(tSrcMt) );
		cMsg.CatMsgBody( cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen() );
		SendMsgToAllMcs( wEvent, cMsg );
		break;

	default:
		ConfPrint( LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMMcuBandwidthNotify
    功能        ：级联E1上级带宽指示
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt& tMMcu			源终端
				  const u32 dwBandWidth		带宽				  
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	2012/05/12   4.7.1        周晶晶        创建
====================================================================*/
void CMcuVcInst::ProcMMcuBandwidthNotify( const TMt& tMMcu,const u32 dwBandWidth )
{	
	switch( CurState() )
	{
	case STATE_ONGOING:
		{		
			if( m_tCascadeMMCU.IsNull() || m_tCascadeMMCU.GetMtId() != tMMcu.GetMtId() )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] mt<%d> is not mmcu.so not handle.\n", 
						tMMcu.GetMtId() );
				return;
			}
			if( !m_ptMtTable->GetMtTransE1(m_tCascadeMMCU.GetMtId()) )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] mt<%d> is mmcu.but not e1 trans.so not handle.\n", 
						tMMcu.GetMtId() );
				return;
			}
			if( 0 == dwBandWidth )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] dwBandWidth is zero.so not handle.\n" );
				return;
			}
			CServMsg cServMsg(NULL,0);
			

			//u16 wBandWidth = ntohs(*(u16 *)cServMsg.GetMsgBody());
			/*u16 wDailBandWidth = m_ptMtTable->GetRcvBandWidth( m_tCascadeMMCU.GetMtId() );

			
			u16 wRealBandWidth = wBandWidth;
			if( wRealBandWidth > m_tConf.GetBitRate() )
			{
				wRealBandWidth = m_tConf.GetBitRate();
			}
			if( wRealBandWidth < m_tConf.GetSecBitRate() )
			{
				wRealBandWidth = m_tConf.GetSecBitRate();
			}
			if( wRealBandWidth > wDailBandWidth )
			{
				wRealBandWidth = wDailBandWidth;
			}*/
			u32 dwRealBandWidth = dwBandWidth;
			u16 wMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
			if( !IsValidMcuId( wMcuIdx ) )
			{
				ConfPrint( LOG_LVL_ERROR, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] wMcuIdx.%d is invalid\n",wMcuIdx );
				return;
			}
			BOOL32 bIsMMcuSupMultSpy = IsLocalAndSMcuSupMultSpy( wMcuIdx );
		
			if( bIsMMcuSupMultSpy )
			{
				if( dwRealBandWidth > m_tConf.GetSndSpyBandWidth() )
				{
					dwRealBandWidth = m_tConf.GetSndSpyBandWidth();
				}

				SetRealSndSpyBandWidth( dwRealBandWidth );
			}
			else
			{	
				SetRealSndSpyBandWidth( dwRealBandWidth );
			}
			
			
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] mt<%d> dwBandWidth(%d) is adjust to dwBandWidth(%d).\n", m_tCascadeMMCU.GetMtId(),
						 dwBandWidth,dwRealBandWidth );

			CSendSpy *ptSndSpy = NULL;
			TSimCapSet tDstCap;
			
			u8 bySpyNum = 0;
			u32 dwSpyBandWidth = 0;
			u8 bySpyIdx = 0;
			if( bIsMMcuSupMultSpy )
			{
				for (bySpyIdx = 0; bySpyIdx < MAXNUM_CONF_SPY; bySpyIdx++)
				{
					tDstCap.Clear();
					ptSndSpy = m_cLocalSpyMana.GetSendSpy(bySpyIdx);
					if (NULL == ptSndSpy)
					{
						continue;
					}
					if( ptSndSpy->GetSpyMt().IsNull() )
					{
						continue;
					}
					if( !m_tConfAllMtInfo.MtJoinedConf( GetLocalMtFromOtherMcuMt( ptSndSpy->GetSpyMt() ).GetMtId() ) )
					{
						continue;
					}
					
					tDstCap = ptSndSpy->GetSimCapset();
					if( ptSndSpy->GetSpyMode() == MODE_VIDEO  || MODE_BOTH == ptSndSpy->GetSpyMode() )
					{
						++bySpyNum;
						dwSpyBandWidth += tDstCap.GetVideoMaxBitRate();
					}
					if( ptSndSpy->GetSpyMode() == MODE_AUDIO  || MODE_BOTH == ptSndSpy->GetSpyMode() )
					{	
						dwSpyBandWidth += GetAudioBitrate( tDstCap.GetAudioMediaType() );
					}
				}
			}
			
			//双流主流算作一路带宽
			TLogicalChannel tLogChn;
			TLogicalChannel tSecLogChn;
			TMtStatus tStatus;
			if( m_ptMtTable->GetMtLogicChnnl(m_tCascadeMMCU.GetMtId(),LOGCHL_SECVIDEO,&tSecLogChn,TRUE)
				&& !m_tDoubleStreamSrc.IsNull() )
			{
				bySpyNum++;
			}
			else
			{
				m_ptMtTable->GetMtStatus( m_tCascadeMMCU.GetMtId(),&tStatus );
				TMt tVideoSrcMt = tStatus.GetVideoMt();
				if(  !tVideoSrcMt.IsNull() )
				{
					++bySpyNum;
					tVideoSrcMt = GetLocalMtFromOtherMcuMt(tVideoSrcMt);
					dwSpyBandWidth += m_ptMtTable->GetMtReqBitrate( tVideoSrcMt.GetMtId(),TRUE );
					tVideoSrcMt = tStatus.GetAudioMt();
					if( !tVideoSrcMt.IsNull() )
					{
						tVideoSrcMt = GetLocalMtFromOtherMcuMt(tVideoSrcMt);					
						if( m_ptMtTable->GetMtLogicChnnl( tVideoSrcMt.GetMtId(),LOGCHL_AUDIO,&tLogChn,TRUE) )
						{						
							dwSpyBandWidth += GetAudioBitrate( tLogChn.GetChannelType() );
						}					
					}
				}
				else
				{
					tVideoSrcMt = tStatus.GetAudioMt();
					if( !tVideoSrcMt.IsNull() )
					{
						++bySpyNum;
						tVideoSrcMt = GetLocalMtFromOtherMcuMt(tVideoSrcMt);					
						if( m_ptMtTable->GetMtLogicChnnl( tVideoSrcMt.GetMtId(),LOGCHL_AUDIO,&tLogChn,TRUE) )
						{						
							dwSpyBandWidth += GetAudioBitrate( tLogChn.GetChannelType() );
						}
					}				
				}
			}


			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] all spy BandWidth(%d)   E1 wBandWidth(%d)!\n", 
						 dwSpyBandWidth,dwRealBandWidth );
					
			u16 wSingleBandWidth = 0;
			if( 0 != bySpyNum )
			{
				wSingleBandWidth = (u16)(dwRealBandWidth / bySpyNum);
			}
			else
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] bySpyNum is zero,needn't handle!\n" );
				return;
			}
			
			u16 wModifiedBandWidth = wSingleBandWidth;
			BOOL32 bIsUp = FALSE;
			TMt tSelMt;
			for (bySpyIdx = 0; bySpyIdx < MAXNUM_CONF_SPY; bySpyIdx++)
			{
				wModifiedBandWidth = wSingleBandWidth;
				bIsUp = FALSE;
				tDstCap.Clear();
				ptSndSpy = m_cLocalSpyMana.GetSendSpy(bySpyIdx);
				if (NULL == ptSndSpy)
				{
					continue;
				}
				if( ptSndSpy->GetSpyMt().IsNull() )
				{
					continue;
				}
				if( !m_tConfAllMtInfo.MtJoinedConf( GetLocalMtFromOtherMcuMt( ptSndSpy->GetSpyMt() ).GetMtId() ) )
				{
					continue;
				}
				
				tDstCap = ptSndSpy->GetSimCapset();
				if( ptSndSpy->GetSpyMode() == MODE_AUDIO )
				{
					continue;
				}

				if( wModifiedBandWidth == tDstCap.GetVideoMaxBitRate() )
				{
					continue;
				}
				tSelMt = GetLocalMtFromOtherMcuMt(ptSndSpy->GetSpyMt());
				if( wModifiedBandWidth > m_ptMtTable->GetDialBitrate(tSelMt.GetMtId()) )
				{
					wModifiedBandWidth = m_ptMtTable->GetDialBitrate(tSelMt.GetMtId());
				}

				if( !m_tDoubleStreamSrc.IsNull() )
				{
					wModifiedBandWidth = GetDoubleStreamVideoBitrate( wModifiedBandWidth,FALSE );
				}

				if( ptSndSpy->GetSpyMode() == MODE_BOTH )
				{
					wModifiedBandWidth -= GetAudioBitrate( tDstCap.GetAudioMediaType() );
				}

				if( wModifiedBandWidth == tDstCap.GetVideoMaxBitRate() )
				{
					continue;
				}
				if( wModifiedBandWidth > tDstCap.GetVideoMaxBitRate() )
				{
					bIsUp = TRUE;
				}
				tDstCap.SetVideoMaxBitRate( wModifiedBandWidth );
				m_cLocalSpyMana.SaveSpySimCap(ptSndSpy->GetSpyMt(), tDstCap);
				if( IsNeedSpyAdpt( ptSndSpy->GetSpyMt(),tDstCap,MODE_VIDEO ) )
				{				
					if ( TRUE == bIsUp )
					{
						RefreshSpyBasParam( ptSndSpy->GetSpyMt(),tDstCap,MODE_VIDEO,TRUE,TRUE );
					}
					else
					{
						RefreshSpyBasParam( ptSndSpy->GetSpyMt(),tDstCap,MODE_VIDEO);
					}
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] SpyMt(%d.%d) refresh bas to wModifiedBandWidth.%d!\n", 
						ptSndSpy->GetSpyMt().GetMcuId(),ptSndSpy->GetSpyMt().GetMtId(),wModifiedBandWidth );
				}
				else
				{
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] SpyMt(%d.%d) refresh mt wModifiedBandWidth.%d!\n", 
											ptSndSpy->GetSpyMt().GetMcuId(),ptSndSpy->GetSpyMt().GetMtId(),wModifiedBandWidth );
					if( ptSndSpy->GetSpyMt().IsLocal() )
					{					
						if (m_ptMtTable->GetMtLogicChnnl(ptSndSpy->GetSpyMt().GetMtId(), LOGCHL_VIDEO, &tLogChn, FALSE))
						{							
							tLogChn.SetFlowControl(wModifiedBandWidth);							
							cServMsg.SetMsgBody((u8*)&tLogChn, sizeof(tLogChn)); 
							SendMsgToMt( ptSndSpy->GetSpyMt().GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg );  
						}
					}
					else
					{
						CascadeAdjMtBitrate( ptSndSpy->GetSpyMt(),wModifiedBandWidth );
					}
				}				
			}
			wModifiedBandWidth = wSingleBandWidth;
			if( wModifiedBandWidth - GetAudioBitrate(m_tConf.GetMainAudioMediaType()) != m_ptMtTable->GetMtReqBitrate( m_tCascadeMMCU.GetMtId(),TRUE ) )
			{				
				BOOL32 bIsNeedDoubleBrdAdapt = IsNeedAdapt( m_tDoubleStreamSrc,m_tCascadeMMCU,MODE_SECVIDEO );
				BOOL32 bIsDoubleUp = FALSE;
				bIsUp = FALSE;
				tSelMt = tStatus.GetVideoMt();
				if(  !tSelMt.IsNull() && !(tSelMt == m_tCascadeMMCU) )
				{
					tSelMt = GetLocalMtFromOtherMcuMt(tSelMt);
					if( wModifiedBandWidth > m_ptMtTable->GetDialBitrate(tSelMt.GetMtId()) )
					{
						wModifiedBandWidth = m_ptMtTable->GetDialBitrate(tSelMt.GetMtId());
					}
					tSelMt = tStatus.GetAudioMt();
					if( !tSelMt.IsNull() )
					{
						tSelMt = GetLocalMtFromOtherMcuMt(tSelMt);					
						if( m_ptMtTable->GetMtLogicChnnl( tSelMt.GetMtId(),LOGCHL_AUDIO,&tLogChn,TRUE) )
						{						
							wModifiedBandWidth -= GetAudioBitrate( tLogChn.GetChannelType() );
						}					
					}
					tSelMt = tStatus.GetVideoMt();				

					if( !m_tDoubleStreamSrc.IsNull() )
					{
						bIsUp = GetDoubleStreamVideoBitrate(wModifiedBandWidth,TRUE) > m_ptMtTable->GetMtReqBitrate(m_tCascadeMMCU.GetMtId(),TRUE);
						m_ptMtTable->SetMtReqBitrate( m_tCascadeMMCU.GetMtId(),GetDoubleStreamVideoBitrate(wModifiedBandWidth,TRUE),LOGCHL_VIDEO );
						m_ptMtTable->SetMtReqBitrate( m_tCascadeMMCU.GetMtId(),GetDoubleStreamVideoBitrate(wModifiedBandWidth,FALSE),LOGCHL_SECVIDEO );	
						
						if( GetDoubleStreamVideoBitrate(wModifiedBandWidth,FALSE) > m_ptMtTable->GetMtReqBitrate( m_tCascadeMMCU.GetMtId(),FALSE ) )
						{
							bIsDoubleUp = TRUE;
						}
					}
					else
					{
						bIsUp = wModifiedBandWidth > m_ptMtTable->GetMtReqBitrate(m_tCascadeMMCU.GetMtId(),TRUE);
						m_ptMtTable->SetMtReqBitrate( m_tCascadeMMCU.GetMtId(),wModifiedBandWidth,LOGCHL_VIDEO );
					}

					if( IsNeedSelAdpt(tSelMt,m_tCascadeMMCU,MODE_VIDEO) )
					{
						ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] tSelMt(%d.%d) refresh bas. wModifiedBandWidth.%d bIsUp.%d!\n", 
											tSelMt.GetMcuId(),tSelMt.GetMtId(),m_ptMtTable->GetMtReqBitrate(m_tCascadeMMCU.GetMtId(),TRUE),bIsUp );
						if ( TRUE == bIsUp )
						{
							RefreshSelBasParam( tSelMt,m_tCascadeMMCU,MODE_VIDEO,TRUE,TRUE );
						}
						else
						{
							RefreshSelBasParam( tSelMt,m_tCascadeMMCU,MODE_VIDEO );
						}
					}
					else
					{
						ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] tSelMt(%d.%d) refresh mt. wModifiedBandWidth.%d!\n", 
											tSelMt.GetMcuId(),tSelMt.GetMtId(),wModifiedBandWidth );
						if( tSelMt.IsLocal() )
						{
							AdjustMtVideoSrcBR( m_tCascadeMMCU.GetMtId(),wModifiedBandWidth,MODE_VIDEO );
						}
						else
						{
							CascadeAdjMtBitrate( tSelMt,wModifiedBandWidth );
						}
					}
				}
				else
				{
					wModifiedBandWidth = wModifiedBandWidth - GetAudioBitrate(m_tConf.GetMainAudioMediaType());
					if( !m_tDoubleStreamSrc.IsNull() &&
						GetDoubleStreamVideoBitrate(wModifiedBandWidth,FALSE) > m_ptMtTable->GetMtReqBitrate( m_tCascadeMMCU.GetMtId(),FALSE ) )
					{
						bIsDoubleUp = TRUE;
					}

					if( !m_tDoubleStreamSrc.IsNull() )
					{						
						m_ptMtTable->SetMtReqBitrate( m_tCascadeMMCU.GetMtId(),GetDoubleStreamVideoBitrate(wModifiedBandWidth,TRUE),LOGCHL_VIDEO );
						m_ptMtTable->SetMtReqBitrate( m_tCascadeMMCU.GetMtId(),GetDoubleStreamVideoBitrate(wModifiedBandWidth,FALSE),LOGCHL_SECVIDEO );						
					}
					else
					{					
						m_ptMtTable->SetMtReqBitrate( m_tCascadeMMCU.GetMtId(),wModifiedBandWidth,LOGCHL_VIDEO );
					}
					ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMMcuBandwidthNotify] tSelMt not exists.set reqbitrate. wModifiedBandWidth.%d !\n", 
											tSelMt.GetMcuId(),tSelMt.GetMtId(),m_ptMtTable->GetMtReqBitrate(m_tCascadeMMCU.GetMtId(),TRUE),bIsUp );
				}
				
				if( !m_tDoubleStreamSrc.IsNull() && !(m_tDoubleStreamSrc == m_tPlayEqp) )
				{					
					tSelMt.SetNull();
					
					if( 0 == m_tConf.GetSecBitRate() )
					{
						if( !bIsNeedDoubleBrdAdapt )
						{							
							RestoreVidSrcBitrate(tSelMt, MODE_SECVIDEO);														
						}
						else
						{
							if ( TRUE == bIsDoubleUp  )
							{
								RefreshBrdBasParamForSingleMt(m_tCascadeMMCU.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
							}
							else
							{
								RefreshBrdBasParamForSingleMt(m_tCascadeMMCU.GetMtId(),MODE_SECVIDEO);
							}
						}
					}
					else
					{
						if( !bIsNeedDoubleBrdAdapt )
						{
							if( IsNeedAdapt(m_tDoubleStreamSrc, m_tCascadeMMCU, MODE_SECVIDEO)/*m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId(), FALSE)*/)
							{
								if ( TRUE == bIsDoubleUp  )
								{
									RefreshBrdBasParamForSingleMt(m_tCascadeMMCU.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
								}
								else
								{
									RefreshBrdBasParamForSingleMt(m_tCascadeMMCU.GetMtId(),MODE_SECVIDEO);
								}
								StartSwitchToSubMtFromAdp(m_tCascadeMMCU.GetMtId(), MODE_SECVIDEO);
							}
							else
							{
								ConfPrint( LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMMcuBandwidthNotify] before after all not Sec adapt,it's impossible.mt(%d)\n",m_tCascadeMMCU.GetMtId() );
							}
						}					
						else
						{
							if( !IsNeedAdapt(m_tDoubleStreamSrc, m_tCascadeMMCU, MODE_SECVIDEO) )
							{			
								ConfPrint( LOG_LVL_KEYSTATUS,MID_MCU_MT2,"[ProcMMcuBandwidthNotify] before need Sec adapt,after not Sec adapt,rehandle switch.mt(%d)\n",m_tCascadeMMCU.GetMtId() );
								TSwitchGrp tSwitchGrp;
								tSwitchGrp.SetSrcChnl(0);
								tSwitchGrp.SetDstMtNum(1);
								tSwitchGrp.SetDstMt(&m_tCascadeMMCU);
								g_cMpManager.StartSwitchToAll(m_tDoubleStreamSrc, 1, &tSwitchGrp, MODE_SECVIDEO);

								u16 wMtBitRate = m_ptMtTable->GetMtReqBitrate( m_tCascadeMMCU.GetMtId(),FALSE );
								m_ptMtTable->SetMtReqBitrate( m_tCascadeMMCU.GetMtId(),wMtBitRate - 10,LOGCHL_SECVIDEO );
								
								RefreshBrdBasParamForSingleMt(m_tCascadeMMCU.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
								m_ptMtTable->SetMtReqBitrate( m_tCascadeMMCU.GetMtId(),wMtBitRate,LOGCHL_SECVIDEO );
							}
							else
							{
								if ( TRUE == bIsDoubleUp  )
								{
									RefreshBrdBasParamForSingleMt(m_tCascadeMMCU.GetMtId(),MODE_SECVIDEO,TRUE,TRUE );
								}
								else
								{
									RefreshBrdBasParamForSingleMt(m_tCascadeMMCU.GetMtId(),MODE_SECVIDEO);
								}
							}
						}
					}
				}
			}

			
		}
		break;
	default:
		break;
	}
	
}
/*=============================================================================
函 数 名： ProcMtMcuBandwidthNotif
功    能： 终端带宽指示
算法实现： 
全局变量： 
参    数： const CMessage *pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/3/21   4.0			许世林                创建
2008/1/29   4.0         张宝卿                启用本指示的下行处理，转投FlowctrlCmd处理
=============================================================================*/
void CMcuVcInst::ProcMtMcuBandwidthNotif(const CMessage *pcMsg)
{

    if (STATE_ONGOING != CurState())
    {
        ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: ProcMtMcuBandwidthNotif received in state %u, ignore it\n", CurState());
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

	
    u8  byDstMtId = cServMsg.GetSrcMtId();
    
    //u16 wMinBitRate;    
    //TMt tMtSrc;
    //TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );

    u16 wRcvBandWidth = ntohs(*(u16 *)cServMsg.GetMsgBody());
    u16 wSndBandWidth = ntohs(*(u16 *)(cServMsg.GetMsgBody() + sizeof(u16)));

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuBandwidthNotif] mt<%d> wRcvBandWidth :%d, wSndBandWidth :%d\n", 
            cServMsg.GetSrcMtId(), wRcvBandWidth, wSndBandWidth);

	

	//zbq[09/10/2008] 调整标识增加，避免时序问题
	m_ptMtTable->SetRcvBandAdjusted(cServMsg.GetSrcMtId(), TRUE);

    TLogicalChannel tLogicChan;
    /*tLogicChan.SetMediaType(MODE_VIDEO);
    tLogicChan.SetFlowControl(wRcvBandWidth);
    
    cServMsg.SetMsgBody((u8*)&tLogicChan, sizeof(TLogicalChannel));*/

    //标记或恢复标记该终端的升降速情况
    u16 wDailBandWidth = m_ptMtTable->GetRcvBandWidth(byDstMtId);
    
    //通道可能还没来得及打开成功
    if ( 0 == wDailBandWidth )
    {
        wDailBandWidth = m_ptMtTable->GetDialBitrate(byDstMtId);
		m_ptMtTable->SetRcvBandWidth(byDstMtId, wDailBandWidth);
    }

    BOOL32 bE1Switch2ETH = FALSE;

    //临时粗略估计，应该够用. 相差小于5％认为恢复，否则认为降速. 只针对E1终端.
    //以太终端始终不认为降速, 其物理线路带宽大于8M，一般为10M.
    if ( wRcvBandWidth > 1024 * 8 )
    {
        if (m_ptMtTable->GetMtTransE1( byDstMtId ) )
        {
            bE1Switch2ETH = TRUE;
        }
        m_ptMtTable->SetMtTransE1( byDstMtId, FALSE );
    }
    else
    {
        m_ptMtTable->SetMtTransE1( byDstMtId, TRUE );

		//zbq[9/27/2008] 呼叫码率小于E1当前额定带宽，保护其为正常带宽
        if (wDailBandWidth < wRcvBandWidth)
        {
            m_ptMtTable->SetMtBRBeLowed(byDstMtId, FALSE);
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuBandwidthNotif] DstMt.%d NOT be set lowed due to<Dail.%d, Rcv.%d>(less)\n",
                      byDstMtId, wDailBandWidth, wRcvBandWidth );            
        }
        else
        {
			if (abs(wDailBandWidth-wRcvBandWidth)*100 / wDailBandWidth > 20)
			{
				m_ptMtTable->SetMtBRBeLowed(byDstMtId, TRUE);
				m_ptMtTable->SetLowedRcvBandWidth(byDstMtId, wRcvBandWidth);
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuBandwidthNotif] DstMt.%d be set lowed due to<Dail.%d, Rcv.%d>\n",
					byDstMtId, wDailBandWidth, wRcvBandWidth );

			}
			else
			{
				m_ptMtTable->SetMtBRBeLowed(byDstMtId, FALSE);
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuBandwidthNotif] DstMt.%d NOT be set lowed due to<Dail.%d, Rcv.%d>(greater less than 20)\n",
					byDstMtId, wDailBandWidth, wRcvBandWidth );            
			}
        }
    }

    //zbq[05/25/2009] 非E1终端不允许通过本消息刷带宽；E1切换到ETH允许通过本消息调整带宽；
    //因为其BR=10240，会保护到ConfBR，从而影响正常低速ETH终端的带宽控制
    if (!bE1Switch2ETH &&
        !m_ptMtTable->GetMtTransE1(byDstMtId))
    {
        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[ProcMtMcuBandwidthNotif] DstMt.%d BandWidth ntf has been ignored due to ETH!\n", byDstMtId);
        return;
    }

	u16 wDialBitRate = m_ptMtTable->GetDialBitrate(byDstMtId);
	if( wRcvBandWidth > wDialBitRate )
	{
		wRcvBandWidth = wDialBitRate;
	}
	if( wRcvBandWidth < m_tConf.GetSecBitRate() )
	{
		wRcvBandWidth = m_tConf.GetSecBitRate();
	}
	if( wSndBandWidth > wDialBitRate )
	{
		wSndBandWidth = wDialBitRate;
	}
	if( wSndBandWidth < m_tConf.GetSecBitRate() )
	{
		wSndBandWidth = m_tConf.GetSecBitRate();
	}

	tLogicChan.SetMediaType(MODE_VIDEO);

	u16 wVideoBandWidth = 0;
	if( wRcvBandWidth > GetAudioBitrate(m_tConf.GetMainAudioMediaType()))
	{
		wVideoBandWidth = wRcvBandWidth - GetAudioBitrate(m_tConf.GetMainAudioMediaType());
	}
	tLogicChan.SetFlowControl(wVideoBandWidth);
    
    cServMsg.SetMsgBody((u8*)&tLogicChan, sizeof(TLogicalChannel));


	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuBandwidthNotif] Modify mt<%d> wRcvBandWidth :%d, wSndBandWidth :%d\n", 
            cServMsg.GetSrcMtId(), wRcvBandWidth, wSndBandWidth);    

    CMessage cMsg;
    cMsg.content = cServMsg.GetServMsg();
    cMsg.length = cServMsg.GetServMsgLen();
    ProcMtMcuFlowControlCmd(&cMsg, TRUE);

    /*
    u16 wAudBandWidth = GetAudioBitrate(m_tConf.GetCapSupport().GetMainSimCapSet().GetAudioMediaType());
    wSndBandWidth -= wAudBandWidth; 
    wRcvBandWidth -= wAudBandWidth;

    //1. 终端上行带宽处理
    if (wSndBandWidth != m_ptMtTable->GetSndBandWidth(tMt.GetMtId()) &&
        wSndBandWidth <= m_tConf.GetBitRate() && wSndBandWidth >= m_tConf.GetSecBitRate())
    {
        m_ptMtTable->SetSndBandWidth(tMt.GetMtId(), wSndBandWidth); 
                
        //是否应该给源终端发送接收带宽指示**
        if (IsMtSendingVideo(tMt.GetMtId()))
        {
            TLogicalChannel tLogicChnnl;
            if( m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tLogicChnnl, FALSE ) )
            {
                if (!m_tDoubleStreamSrc.IsNull())
                {
                    tLogicChnnl.SetFlowControl( wSndBandWidth/2 );
                }
                else
                {
                    tLogicChnnl.SetFlowControl( wSndBandWidth );
                }				
                cServMsg.SetMsgBody( (u8*)&tLogicChnnl, sizeof( tLogicChnnl ) );
                SendMsgToMt( tMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg );
            }
        }
    }   
	
    //2. 终端下行带宽处理
    if( wRcvBandWidth > m_tConf.GetBitRate() || wRcvBandWidth < m_tConf.GetSecBitRate() ||
        wRcvBandWidth == m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE))
    {
        return;
    }    
    
    m_ptMtTable->SetRcvBandWidth( tMt.GetMtId(), wRcvBandWidth );
    m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), wRcvBandWidth, LOGCHL_VIDEO);

    //处理选看源
    m_ptMtTable->GetMtSrc( tMt.GetMtId(), &tMtSrc, MODE_VIDEO );
    if(tMtSrc.IsNull())
    {
        return;
    }
    if( tMtSrc == tMt )
    {
        return;
    }
    if( !(tMtSrc == m_tVidBrdSrc) )
    {
        //对于选看的终端，直接转发给相应源
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2,  "[ProcMtMcuBandwidthNotif] SelectSee SrcMtId.%d, DstMtId.%d, wBitRate.%d\n", 
            tMtSrc.GetMtId(), tMt.GetMtId(), wRcvBandWidth);
        
        AdjustMtVideoSrcBR( tMt.GetMtId(), wRcvBandWidth );
        return;
    }
	
    TSimCapSet tSrcSimCapSet;
    TSimCapSet tDstSimCapSet;
    tDstSimCapSet = m_ptMtTable->GetDstSCS(tMt.GetMtId());
    wMinBitRate = GetLeastMtReqBitrate(TRUE, tDstSimCapSet.GetVideoMediaType());    

	//若会议正在广播画面合成图像,降画面合成码率
    if (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE &&
        m_tConf.m_tStatus.IsBrdstVMP())
    {
        u16 wBasBitrate;
        u8  byVmpChanNo;
        u8  byMediaType = m_tConf.GetCapSupport().GetMainSimCapSet().GetVideoMediaType();
        
        //双速会议要考虑入会时的呼叫码率
        if ( 0 != m_tConf.GetSecBitRate() && 
            MEDIA_TYPE_NULL == m_tConf.GetCapSupport().GetSecondSimCapSet().GetVideoMediaType())
        {
            if (m_ptMtTable->GetDialBitrate(tMt.GetMtId()) == m_tConf.GetBitRate())
            {
                byVmpChanNo = 1;
                wBasBitrate = m_wVidBasBitrate;
            }
            else
            {
                byVmpChanNo = 2;
                wBasBitrate = m_wBasBitrate;
            }
        }
        //单速或双格式会议
        else
        {
            if (tDstSimCapSet.GetVideoMediaType() == byMediaType)
            {
                byVmpChanNo = 1;
                wBasBitrate = m_wVidBasBitrate;
            }
            else
            {
                byVmpChanNo = 2;
                wBasBitrate = m_wBasBitrate;
            }
        }
        
        if (wMinBitRate != wBasBitrate)
        {
            ChangeVmpBitRate(wMinBitRate, byVmpChanNo);                
        }
        return;
    }   

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2,  "[ProcMtMcuBandwidthNotif] IsUseAdapter.%d, SrcMtId.%d, DstMtId.%d, wBitRate.%d, wMinBitRate.%d\n", 
			m_tConf.GetConfAttrb().IsUseAdapter(), tMtSrc.GetMtId(), tMt.GetMtId(), wRcvBandWidth, wMinBitRate );

    //不启用适配,直接降发言人码率
	if( !m_tConf.GetConfAttrb().IsUseAdapter() )
	{
        if (!m_tDoubleStreamSrc.IsNull())
        {
            TLogicalChannel tLogicalChannel;         
            if (m_ptMtTable->GetMtLogicChnnl(tMtSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
            {
                if (!m_tVidBrdSrc.IsNull() && m_tVidBrdSrc.GetMtId() != tMtSrc.GetMtId())
                {
                    m_ptMtTable->SetMtSndBitrate(tMtSrc.GetMtId(), wMinBitRate);                            
                }
            }
        }

		AdjustMtVideoSrcBR( tMt.GetMtId(), wMinBitRate );
	}
    else
    {
        u16 wAdaptBitRate = 0;            

        //双速会议,启用适配时,
        //双流下，如果新的终端接收速率低于第二速率/2，则禁止该终端的视频转发
        //非双流下，如果新的终端接收速率低于第二速率，则禁止该终端的视频转发
        //否则，则开启转发
        if (0 != m_tConf.GetSecBitRate())
        {			
            u16 wFirstMinBitRate =  m_tConf.GetSecBitRate();
            if (!m_tDoubleStreamSrc.IsNull() && !m_tVidBrdSrc.IsNull())
            {
                wFirstMinBitRate = GetFstVideoBitrate(wFirstMinBitRate);
            }
            u16 wFabsBitrate;
            u16 wMainAudioBitrate = GetAudioBitrate(m_tConf.GetMainAudioMediaType());
            u16 wSecAudioBitrate = GetAudioBitrate(m_tConf.GetSecAudioMediaType());
            if (wMainAudioBitrate < wSecAudioBitrate)
            {
                wFabsBitrate = wSecAudioBitrate - wMainAudioBitrate;
            }
            else
            {
                wFabsBitrate = wMainAudioBitrate - wSecAudioBitrate;
            }
            if (wRcvBandWidth < wFirstMinBitRate - wFabsBitrate)
            {
/ *                     if( MODE_VIDEO == tLogicChnnl.GetMediaType() )
                {
                   StopSwitchToSubMt( tMt.GetMtId(), MODE_VIDEO );
                    if( TRUE == bStartDoubleStream && !m_tVidBrdSrc.IsNull() )
                    {
                        g_cMpManager.StopSwitchToSubMt( tMt, MODE_VIDEO, TRUE  );
                    }
                }
                else
                {
                    g_cMpManager.StopSwitchToSubMt( tMt, MODE_VIDEO, TRUE  );
                    if( TRUE == bStartDoubleStream && !m_tVidBrdSrc.IsNull() )
                    {
                        StopSwitchToSubMt( tMt.GetMtId(), MODE_VIDEO );
                    }
                }* /
                
                return;
            }            
        }
        
        //双格式会议需类型适配终端降适配码率
        if (IsMtNeedAdapt(ADAPT_TYPE_VID, tMt.GetMtId())  &&
            (IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate) || IsNeedCifAdp()))
        {
            if (wMinBitRate != m_wVidBasBitrate)
            {
                if (m_tConf.m_tStatus.IsVidAdapting())
                {
                    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuBandwidthNotif] change vid bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                           wMinBitRate, m_wVidBasBitrate);
                    ChangeAdapt(ADAPT_TYPE_VID, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }
                else
                {
                    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuBandwidthNotif] start vid bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                           wMinBitRate, m_wVidBasBitrate);
                    StartAdapt(ADAPT_TYPE_VID, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }                   

                StartSwitchToSubMt(m_tVidBasEqp, m_byVidBasChnnl, tMt.GetMtId(), MODE_VIDEO);
            }
            else if (m_tConf.m_tStatus.IsVidAdapting())//若不需要调节适配码率，直接将适配码流交换到此mt(会议中有其他终端的接收码率更低的情况)
            {
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuBandwidthNotif] switch vid bas to mt<%d>\n", tMt.GetMtId());
                StartSwitchToSubMt(m_tVidBasEqp, m_byVidBasChnnl, tMt.GetMtId(), MODE_VIDEO);
            }
        }
        //双速会议降速或变速（需要适配）
        else if (IsMtNeedAdapt(ADAPT_TYPE_BR, tMt.GetMtId()) &&
            IsNeedBrAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate))
        {
            if (wMinBitRate != m_wBasBitrate)
            {
                if (m_tConf.m_tStatus.IsBrAdapting())
                {
                    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuBandwidthNotif]change br bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                           wMinBitRate, m_wBasBitrate);
                    ChangeAdapt(ADAPT_TYPE_BR, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }
                else
                {
                    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuBandwidthNotif]start br bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                           wMinBitRate, m_wBasBitrate);
                    StartAdapt(ADAPT_TYPE_BR, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }                   
                
                StartSwitchToSubMt(m_tBrBasEqp, m_byBrBasChnnl, tMt.GetMtId(), MODE_VIDEO);
            }
            else if (m_tConf.m_tStatus.IsBrAdapting())//若不需要调节适配码率，直接将适配码流交换到此mt(会议中有其他终端的接收码率更低的情况)
            {
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuBandwidthNotif] switch br bas to mt<%d>\n", tMt.GetMtId());
                StartSwitchToSubMt(m_tBrBasEqp, m_byBrBasChnnl, tMt.GetMtId(), MODE_VIDEO);
            }
        }            
        //升速到原始速率，由适配变为不适配
        else if (0 != m_tConf.GetSecBitRate())
        {
            StartSwitchToSubMt(tMtSrc, 0, tMt.GetMtId(), MODE_VIDEO);
        }
        //双格式会议无需适配终端降其视频源码率，可能会将码流压到最低
        else if (MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() || IsNeedCifAdp())
        {
            AdjustMtVideoSrcBR(tMt.GetMtId(), wMinBitRate);
        }
    }*/

    return;
}

/*====================================================================
    函数名      ：ProcMtMcuOtherMsg
    功能        ：终端其它消息处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
====================================================================*/
void CMcuVcInst::ProcMtMcuOtherMsg( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u8 bySrcMtId = cServMsg.GetSrcMtId();
    TMt tMt = m_ptMtTable->GetMt(bySrcMtId);

	switch( CurState() )
	{
	case STATE_ONGOING:

		//是否加入会议
		if( !m_tConfAllMtInfo.MtJoinedConf( bySrcMtId ))
		{
			return;
		}

		switch( pcMsg->event )
		{	 
            //主席终端查询终端状态应答
		case MT_MCU_GETMTSTATUS_ACK:	
            {
                TMt tChairMt = m_tConf.GetChairman();
                if(!tChairMt.IsNull())
                {
                    TMtStatus tMtStatus = *(TMtStatus *)cServMsg.GetMsgBody();
                    tMtStatus.SetMt(tMt);
                    cServMsg.SetMsgBody((u8*)&tMtStatus, sizeof(tMtStatus));
                    SendMsgToMt(tChairMt.GetMtId(), MCU_MT_GETMTSTATUS_ACK, cServMsg);
                }                
            }
			break;
			
		case MT_MCU_GETMTSTATUS_NACK:			
			break;       

        case MT_MCU_GETBITRATEINFO_ACK:
        case MT_MCU_GETBITRATEINFO_NACK:
            break;

        case MT_MCU_GETBITRATEINFO_NOTIF:
            {
                TMtBitrate tMtBitrate = *(TMtBitrate *)cServMsg.GetMsgBody();
                tMtBitrate.SetMt(tMt);
                cServMsg.SetMsgBody((u8*)&tMtBitrate, sizeof(tMtBitrate));
                SendMsgToAllMcs(MCU_MCS_MTBITRATE_NOTIF, cServMsg); 
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[ProcMtMcuOtherMsg] Mt<%d,%d> bitrate : %u, %u, %u, %u\n", 
                    tMt.GetMcuId(), tMt.GetMtId(), tMtBitrate.GetRecvBitRate(), tMtBitrate.GetSendBitRate(), 
                    tMtBitrate.GetH239RecvBitRate(), tMtBitrate.GetH239SendBitRate());
            }            
            break;

//         case MT_MCU_GETMTVERID_ACK:
//             {
//                 u8 byHWVerId = *(u8*)cServMsg.GetMsgBody();
//                 LPCSTR lpszSWVerId = (LPCSTR)(cServMsg.GetMsgBody() + sizeof(u8));
//                 m_ptMtTable->SetHWVerID(tMt.GetMtId(), byHWVerId);
//                 m_ptMtTable->SetSWVerID(tMt.GetMtId(), lpszSWVerId);
//             }
//             break;
            
        case MT_MCU_GETMTVERID_NACK:
            {
                //FIXME: 异步独立获取，考虑错误码上报
                ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "[ProcMtMcuOtherMsg] Get mt.%d VerId failed due to reason.%d\n", tMt.GetMtId(), cServMsg.GetErrorCode() );
            }
            break;
            
		default:
			return;
		}

		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

    return;
}


/*====================================================================
    函数名      ：ProcMcsMcuMtOperCmd
    功能        ：终端操作统一处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/14    1.0         LI Yi         创建
	03/02/27    1.0         Qzj           终端轮询使能设置支持
	04/03/08    3.0         胡昌威        修改
    04/04/15    4.0         张宝卿        主席终端静哑下级MCU支持
====================================================================*/
void CMcuVcInst::ProcMcsMcuMtOperCmd( const CMessage * pcMsg )
{
	CServMsg   cServMsg( pcMsg->content, pcMsg->length );	
	TMt tMt        = *(TMt*)cServMsg.GetMsgBody();
    u8  byOpenFlag = *( cServMsg.GetMsgBody() + sizeof(TMt) );
    u8  bySrcMtId  =  cServMsg.GetSrcMtId();
	TLogicalChannel tLogicalChannel;
	u16	wEvent;
	u16 wBitRate;
    BOOL32 bDecordMute = FALSE;
    
	switch( CurState() )
	{
	case STATE_ONGOING:
		//来自非主席终端，nack
		if( bySrcMtId != 0 && bySrcMtId != m_tConf.GetChairman().GetMtId() )
		{
			SendReplyBack( cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF );
            //guzh [2008/03/27]
            return; 
		}

		//是否加入会议
		if( !m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
		{
			return;
		}

		switch( pcMsg->event )
		{				
		case MCS_MCU_SETMTBITRATE_CMD:	  //会议中设置终端码率
			wEvent = MCU_MT_FLOWCONTROL_CMD;
			wBitRate = *(u16*)( cServMsg.GetMsgBody()+ sizeof(TMt) );
			m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE );
            tLogicalChannel.SetFlowControl( ntohs( wBitRate ) );
			cServMsg.SetMsgBody( (u8*)&tLogicalChannel, sizeof( tLogicalChannel ) );
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "MCS set mt%d bitrate to %d !\n", tMt.GetMtId(), ntohs( wBitRate ) );
			break;

		case MT_MCU_MTMUTE_CMD:                    //主席终端命令MCU进行终端静音设置
			wEvent = MCU_MT_MTMUTE_CMD;           
            bDecordMute = TRUE;
			break;
		case MT_MCU_MTDUMB_CMD:                    //主席终端命令MCU进行终端哑音设置
			wEvent = MCU_MT_MTDUMB_CMD;
            bDecordMute = FALSE;
			break;
		default:
			return;
		}

        if ( MT_MANU_KDC != m_ptMtTable->GetManuId(tMt.GetMtId()) &&
             ( MCU_MT_MTMUTE_CMD == wEvent || MCU_MT_MTDUMB_CMD == wEvent ) )
        {
            //调整非KEDA终端（KEDAMCU和Tandberg）静音和哑音
            AdjustKedaMcuAndTaideMuteInfo( &tMt, bDecordMute, byOpenFlag );          
        }
        else
        {
            cServMsg.SetDstMtId( tMt.GetMtId() );
            SendMsgToMt( tMt.GetMtId(), wEvent, cServMsg ); 
        }
		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			             pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
    函数名      ：ProcMcsMcuMtOperReq
    功能        ：终端操作统一处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/14    1.0         LI Yi         创建
	03/02/27    1.0         Qzj           终端轮询使能设置支持
	04/04/02    3.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::ProcMcsMcuMtOperReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u8 bySrcMtId = cServMsg.GetSrcMtId();
	//u8  byMuteType;
	//u8  byMuteOpenFlag;
	u8  byMtNum = 0;
	u8  byIndex = 0;
	TMt* ptDstMt = NULL;
	u16	wEvent;
	TPollInfo tPollInfo;
	memset( &tPollInfo, 0, sizeof(tPollInfo) );
	TMt tDstMt = *(TMt*)cServMsg.GetMsgBody();

	switch( CurState() )
	{
	case STATE_ONGOING:
		//来自非主席终端，nack
		if( bySrcMtId != 0 && bySrcMtId != m_tConf.GetChairman().GetMtId() )
		{
			SendReplyBack( cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF );
		}

		//是否加入会议
		if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMcuIdx(), tDstMt.GetMtId() ) )
		{
			return;
		}

		if( pcMsg->event == MCS_MCU_MTAUDMUTE_REQ || 		
            pcMsg->event == MCS_MCU_EXMATRIX_GETPORT_REQ)
		{
			SendReplyBack( cServMsg, pcMsg->event+1 );
		}

		switch( pcMsg->event )
		{
            //终端内置矩阵
		case MCS_MCU_MATRIX_SAVESCHEME_CMD:     //保存矩阵方案	
			wEvent = MCU_MT_MATRIX_SAVESCHEME_CMD;
			break;		

        case MCS_MCU_MATRIX_GETALLSCHEMES_CMD:  //所有矩阵方案请求
            wEvent = MCU_MT_MATRIX_GETALLSCHEMES_CMD;
            break;

        case MCS_MCU_MATRIX_GETONESCHEME_CMD: //单个矩阵方案请求
            wEvent = MCU_MT_MATRIX_GETONESCHEME_CMD;
            break;

        case MCS_MCU_MATRIX_SETCURSCHEME_CMD://指定当前方案
            wEvent = MCU_MT_MATRIX_SETCURSCHEME_CMD;
            break;

        case MCS_MCU_MATRIX_GETCURSCHEME_CMD://获取当前方案
            wEvent = MCU_MT_MATRIX_GETCURSCHEME_CMD;
            break;
            
            //终端外置矩阵
        case MCS_MCU_EXMATRIX_GETINFO_CMD:      //获取终端外置矩阵类型
            wEvent = MCU_MT_EXMATRIX_GETINFO_CMD;
            break;
        case MCS_MCU_EXMATRIX_SETPORT_CMD:      //设置外置矩阵连接端口号
            wEvent = MCU_MT_EXMATRIX_SETPORT_CMD;
            break;
        case MCS_MCU_EXMATRIX_GETPORT_REQ:      //请求获取外置矩阵连接端口
            wEvent = MCU_MT_EXMATRIX_GETPORT_REQ;
            break;
        case MCS_MCU_EXMATRIX_SETPORTNAME_CMD:  //设置外置矩阵连接端口名
            wEvent = MCU_MT_EXMATRIX_SETPORTNAME_CMD;
            break;
        case MCS_MCU_EXMATRIX_GETALLPORTNAME_CMD://请求获取外置矩阵的所有端口名
            wEvent = MCU_MT_EXMATRIX_GETALLPORTNAME_CMD;
            break;

            //终端扩展视频源
        case MCS_MCU_GETVIDEOSOURCEINFO_CMD:
            wEvent = MCU_MT_GETVIDEOSOURCEINFO_CMD;
            break;
        case MCS_MCU_SETVIDEOSOURCEINFO_CMD:
            wEvent = MCU_MT_SETVIDEOSOURCEINFO_CMD;
            break;
            
            //终端切换扩展视频源
        case MCS_MCU_SELECTEXVIDEOSRC_CMD:
            wEvent = MCU_MT_SELECTEXVIDEOSRC_CMD;
            break;

		case MCS_MCU_MTAUDMUTE_REQ:             //会控要求MCU设置终端静音
			ProcMtMuteDumbReq(cServMsg);
			return;

		default:
			return;
		}

		//send messge
		cServMsg.SetDstMtId( tDstMt.GetMtId() );
		if( pcMsg->event != MCS_MCU_MTAUDMUTE_REQ )
		{
			CServMsg cTempMsg = cServMsg;
			cServMsg.SetMsgBody( cTempMsg.GetMsgBody() + sizeof(TMt), cTempMsg.GetMsgBodyLen() - sizeof(TMt) );
		}

		SendMsgToMt( tDstMt.GetMtId(), wEvent, cServMsg );
		
		break;

	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF,"CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
函数名      ：ProcSingleMtSmsOpr
功能        ：对单个终端进行短消息操作
算法实现    ：
引用全局变量：
输入参数说明： 
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
 10/08/12    4.6			XL			create
====================================================================*/
void CMcuVcInst::ProcSingleMtSmsOpr(const CServMsg &cServMsg, TMt *ptDstMt, CRollMsg *ptROLLMSG)
{
	if( ptDstMt == NULL )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[ProcSingleMtSmsOpr] param-ptDstMt is NULL, return\n");
		return;
	}

	TMt tMt = *ptDstMt;
	BOOL32 bIsLocal = FALSE;

	TMsgHeadMsg tHeadMsg;
	memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );

	ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "tmt info: GetMcuIdx = %d, GetMcuId = %d, GetMtId = %d, GetMtType = %d\n",
		tMt.GetMcuIdx(), tMt.GetMcuId(), tMt.GetMtId(), tMt.GetMtType());

	if(cServMsg.GetEventId() == MCU_MCU_SENDMSG_NOTIF)
	{
		bIsLocal = tMt.IsMcuIdLocal();
	}
	else
	{
		bIsLocal = tMt.IsLocal();
	}
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT, "[ProcSingleMtSmsOpr] bLocal is %d!\n", bIsLocal);

	if(!bIsLocal)
	{
		
		if(cServMsg.GetEventId() != MCU_MCU_SENDMSG_NOTIF)
		{
			memset( &tHeadMsg, 0, sizeof(TMsgHeadMsg) );
			tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt, tMt );
			//tMt = tHeadMsg.m_tMsgDst.m_tMt;
										
		}
		else
		{
			tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody() + sizeof(TMt) + ptROLLMSG->GetTotalMsgLen() );
			//tHeadMsg.m_tMsgDst.m_tMt = tMt;

			// if ack is needed, build HeadMsgAck info here： 
			//tHeadMsgAck.m_tMsgSrc = tHeadMsg.m_tMsgDst;
			//tHeadMsgAck.m_tMsgDst = tHeadMsg.m_tMsgSrc;
		}
		
		CServMsg cMsg;
		cMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
		cMsg.CatMsgBody( (u8*)&tMt, sizeof(TMt) );
		cMsg.CatMsgBody( (u8*)ptROLLMSG, ptROLLMSG->GetTotalMsgLen() );

		
		SendMsgToMt( (u8)tMt.GetMcuId(), MCU_MCU_SENDMSG_NOTIF, cMsg );

	}
	else
	{
		//local的处理
		CServMsg cSendMsg;
		TMt tDstMt = m_ptMtTable->GetMt( tMt.GetMtId() );
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT,  "tmt in mtTable --mttype is: %u; tmt-- mttype is: %u\n", tDstMt.GetMtType(), tMt.GetMtType());
		if(MT_TYPE_MT == tDstMt.GetMtType() )
		{
			// 发给终端

			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT,  "MCU_MT_SENDMSG_NOTIF to mt.(%d,%d)\n", tDstMt.GetMcuId(), tDstMt.GetMtId());
			cSendMsg.SetMsgBody( (u8*)ptDstMt, sizeof(TMt) );
			cSendMsg.CatMsgBody( (u8*)ptROLLMSG, ptROLLMSG->GetTotalMsgLen() );
			SendMsgToMt( tMt.GetMtId(), MCU_MT_SENDMSG_NOTIF, cSendMsg );
		}
		else
		{
			// 发给mcu
			TMt tMtNull;
			tMtNull.SetNull();
			//tHeadMsg.m_tMsgDst.m_tMt = tMtNull;
			cSendMsg.SetMsgBody( (u8*)&tHeadMsg, sizeof(TMsgHeadMsg) );
			cSendMsg.CatMsgBody( (u8*)&tMtNull, sizeof(TMt) );
			cSendMsg.CatMsgBody( (u8*)ptROLLMSG, ptROLLMSG->GetTotalMsgLen() );

			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT,  "MCU_MT_SENDMSG_NOTIF to mcu.(%d,%d)\n",tDstMt.GetMcuId(), tDstMt.GetMtId());
			SendMsgToMt( tMt.GetMtId(), MCU_MCU_SENDMSG_NOTIF, cSendMsg );
		}
	}

	return;
}
/*====================================================================
函数名      ：ProcSingleMtMuteDumbOpr
功能        ：对单个终端进行静音哑音操作
算法实现    ：
引用全局变量：
输入参数说明： 
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
  10/08/06      4.6			XL			  创建
====================================================================*/
void CMcuVcInst::ProcSingleMtMuteDumbOpr(CServMsg &cServMsg, TMt *ptDstMt, u8 byMuteOpenFlag, u8 byMuteType)
{
	if (CurState() != STATE_ONGOING)
	{
		return;
	}
		
	if( ptDstMt == NULL )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[ProcSingleMtMuteDumbOpr] param-ptDstMt is NULL, return\n");
		return;
	}

	TMt tMt = *ptDstMt;
	BOOL32 bIsLocal = FALSE;
	//u16 wMcuIdx;
	//u8  byMtId;
	TMsgHeadMsg tHeadMsg;
	memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT, "tmt info: GetMcuIdx = %d, GetMcuId = %d, GetMtId = %d\n",
		tMt.GetMcuIdx(), tMt.GetMcuId(), tMt.GetMtId() );

	// [9/21/2011 liuxu] vcs会议上级请求下级静哑音
	if ( tMt.IsNull() 
		 && VCS_CONF == m_tConf.GetConfSource()
		 && cServMsg.GetSrcMtId() == m_tCascadeMMCU.GetMtId())
	{
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "mmcu req to mute all\n");
		CServMsg cMsg;
		memset(&cMsg, 0, sizeof(CServMsg));
		cMsg.SetEventId(VCS_MCU_MUTE_REQ);

		u8 byMode = m_cVCSConfStatus.GetCurVCMode();
		cMsg.SetMsgBody(&byMode, sizeof(u8));
		cMsg.CatMsgBody(&byMuteOpenFlag, sizeof(u8));
		
		u8  byOprObj = VCS_OPR_REMOTE;
		cMsg.CatMsgBody(&byOprObj, sizeof(u8));
		cMsg.CatMsgBody(&byMuteType, sizeof(u8));

		u8 byFromMcuFlag = 1;
		cMsg.CatMsgBody(&byFromMcuFlag, sizeof(u8));

		ProcVcsMcuMuteReq(cMsg);
		return;		
	}

	if(cServMsg.GetEventId() == MCU_MCU_MTAUDMUTE_REQ)
	{
		bIsLocal = tMt.IsMcuIdLocal();
		//wMcuIdx = GetMcuIdxFromMcuId( (u8)tMt.GetMcuId() );
		//byMtId = tMt.GetMtId();
	}
	else
	{
		bIsLocal = tMt.IsLocal();
		//wMcuIdx = tMt.GetMcuIdx();
		//byMtId = tMt.GetMtId();
	}
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT, "[ProcSingleMtMuteDumbOpr] bLocal is %d!\n", bIsLocal);

	if(!bIsLocal)
	{
		if(!IsSupportMultCas(tMt.GetMcuId()))
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "tMt(mcuid:%d, mtId:%d)not support multicas!\n", tMt.GetMcuId(), tMt.GetMtId());
			NotifyMcsAlarmInfo( cServMsg.GetSrcSsnId(), ERR_MCU_OPRINVALID_NOTSUPPORTMULTICASCADE);
			return;
		}
		if(cServMsg.GetEventId() != MCU_MCU_MTAUDMUTE_REQ)
		{
			memset( &tHeadMsg, 0, sizeof(TMsgHeadMsg) );
			tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt, tMt );
			//tMt = tHeadMsg.m_tMsgDst.m_tMt;
										
		}
		else
		{
			tHeadMsg = *(TMsgHeadMsg*)( cServMsg.GetMsgBody() );
			//tHeadMsg.m_tMsgDst.m_tMt = tMt;

			// if ack is needed, build HeadMsgAck info here： 
			//tHeadMsgAck.m_tMsgSrc = tHeadMsg.m_tMsgDst;
			//tHeadMsgAck.m_tMsgDst = tHeadMsg.m_tMsgSrc;
		}
		
		CServMsg cMsg;
		cMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
		cMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt));
		cMsg.CatMsgBody( (u8 *)&byMuteOpenFlag, sizeof(byMuteOpenFlag));
		cMsg.CatMsgBody( (u8 *)&byMuteType, sizeof(byMuteType));
		
		SendMsgToMt( (u8)tMt.GetMcuId(), MCU_MCU_MTAUDMUTE_REQ, cMsg );
		
// 		if(cServMsg.GetEventId() == MCU_MCU_XXXREQ)
// 		{
// 			tMt = m_ptMtTable->GetMt( (u8)tMt.GetMcuId() );
// 			tHeadMsgAck.m_tMsgSrc.m_tMt = tMt;
// 			cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof( TMsgHeadMsg ) );
// 		}
		//SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

		return;
	}
	
	//local的处理
	u8 byVendor = m_ptMtTable->GetManuId(ptDstMt->GetMtId());
	if(byVendor == MT_MANU_KDC)
	{
		u16 wEvent;
		CServMsg cMsg;
		if( byMuteType == 1 )
		{
			wEvent = MCU_MT_MTMUTE_CMD;
		}
		else
		{
			wEvent = MCU_MT_MTDUMB_CMD;
		}	
		cMsg.SetMsgBody( (u8*)ptDstMt, sizeof(TMt) );
		cMsg.CatMsgBody( &byMuteOpenFlag, sizeof(byMuteOpenFlag) );
		cMsg.SetDstMtId( ptDstMt->GetMtId() );
		SendMsgToMt( ptDstMt->GetMtId(), wEvent, cMsg );
	}
	else
	{                    
		// 会控 byMuteType: 2－哑音, 1－静音
		// 终控 byMuteType: 0－哑音, 1－静音 ---- 保持一致，统一处理
		if ( 2 == byMuteType )
		{
			byMuteType = 0;
		}
		
		//调整非KEDA终端（KEDAMCU和Tandberg）静音和哑音
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcSingleMtMuteDumbOpr]Call AdjustKedaMcuAndTaideMuteInfo here!\n");
		AdjustKedaMcuAndTaideMuteInfo( ptDstMt, byMuteType, byMuteOpenFlag );
	}

}

/*====================================================================
    函数名      ProcMtMuteDumbReq
    功能        ：对终端进行静音哑音操作
    算法实现    ：
    引用全局变量：
    输入参数说明：CServMsg &cServMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
  10/08/06      4.6			XL			  创建
====================================================================*/
void CMcuVcInst::ProcMtMuteDumbReq(CServMsg &cServMsg)
{

	u8  byMuteType;
	u8  byMuteOpenFlag;
	u8  byMtNum = 0;
	u8  byIndex = 0;
	TMt* ptDstMt = NULL;

	//u8 bySrcMtId = cServMsg.GetSrcMtId();
	//TMt tDstMt = *(TMt*)cServMsg.GetMsgBody();

	//可同时处理多个mt静/哑音请求
	byMtNum = cServMsg.GetMsgBodyLen() / (sizeof(TMt)+2);
	for( byIndex=0; byIndex<byMtNum; byIndex++ )
	{
		
		ptDstMt = (TMt*)( cServMsg.GetMsgBody() + byIndex*(sizeof(TMt)+2) );
		byMuteOpenFlag = *( (u8*)ptDstMt + sizeof(TMt) );
		byMuteType     = *( (u8*)ptDstMt + sizeof(TMt) + 1 );

		ProcSingleMtMuteDumbOpr(cServMsg, ptDstMt, byMuteOpenFlag, byMuteType);
		
	}
}
/*====================================================================
    函数名      ：ProcMcsMcuCallMtReq
    功能        ：会控呼叫终端请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
  03/12/09      3.0			JQL			  创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuCallMtReq( const CMessage * pcMsg )
{
	STATECHECK
		
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt;
	TMsgHeadMsg tHeadMsg,tHeadMsgAck;
	BOOL32 bIsLocal = TRUE;
	u16 wMcuIdx = INVALID_MCUIDX;
	u8 byMtId = 0;
	if(cServMsg.GetEventId() == MCU_MCU_REINVITEMT_REQ)
	{
		tMt = *( TMt * )(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq));
		if( cServMsg.GetMsgBodyLen() > ( sizeof(TMcuMcuReq) + sizeof( TMt ) ) )
		{
			tHeadMsg = *(TMsgHeadMsg*)( cServMsg.GetMsgBody()+sizeof(TMcuMcuReq) + sizeof(TMt) );
			tHeadMsgAck.m_tMsgSrc = tHeadMsg.m_tMsgDst;
			tHeadMsgAck.m_tMsgDst = tHeadMsg.m_tMsgSrc;		
		}
		
		bIsLocal = tMt.IsMcuIdLocal();
		wMcuIdx = GetMcuIdxFromMcuId( (u8)tMt.GetMcuId() );
		//m_tConfAllMcuInfo.GetIdxByMcuId( tMt.GetMcuId(),0,&wMcuIdx );
		byMtId = tMt.GetMtId();
	}
	else
	{
		tMt = *( TMt * )cServMsg.GetMsgBody();
		bIsLocal = tMt.IsLocal();
		wMcuIdx = tMt.GetMcuIdx();
		byMtId = tMt.GetMtId();
	}

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcsMcuCallMtReq] Call Mt(%d.%d.%d) level.%d\n",wMcuIdx,byMtId,
						tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],tHeadMsg.m_tMsgDst.m_byCasLevel 
						);

	

	//不在受邀终端列表中, NACK!
    TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(wMcuIdx);//tMt.GetMcuId());
	//会议表中没有
	if(tMtInfo.IsNull())
	{
		if(cServMsg.GetEventId() == MCU_MCU_REINVITEMT_REQ)
		{
			cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof( TMsgHeadMsg ) );
		}
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	//已经加入会议,NACK
	if( tMtInfo.MtJoinedConf( tMt.GetMtId() ) )
	{	
		if(cServMsg.GetEventId() == MCU_MCU_REINVITEMT_REQ)
		{
			cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof( TMsgHeadMsg ) );
		}
		cServMsg.SetErrorCode( ERR_MCU_CALLEDMT_JOINEDCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//不在本MCU上
	if( !bIsLocal )
	{
		TMcuMcuReq tReq;
		TMcsRegInfo	tMcsReg;
		g_cMcuVcApp.GetMcsRegInfo( cServMsg.GetSrcSsnId(), &tMcsReg );
		astrncpy(tReq.m_szUserName, tMcsReg.m_achUser, 
			sizeof(tReq.m_szUserName), sizeof(tMcsReg.m_achUser));
		astrncpy(tReq.m_szUserPwd, tMcsReg.m_achPwd, 
			sizeof(tReq.m_szUserPwd), sizeof(tMcsReg.m_achPwd));
		if(cServMsg.GetEventId() != MCU_MCU_REINVITEMT_REQ)
		{
			//u8 byFstMcuId = 0,bySecMcuId = 0;
			u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
			memset( &abyMcuId[0],0,sizeof( abyMcuId ) );
			memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );
			tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tMt );
			/*tMt = tHeadMsg.m_tMsgDst.m_tMt;
			if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) )
			{			
				m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0] );
				if( 0 != abyMcuId[1] )
				{
					tHeadMsg.m_tMsgDst.m_byCasLevel = 1;				
					tHeadMsg.m_tMsgDst.m_abyMtIdentify[0] = tMt.GetMtId();
					tHeadMsg.m_tMsgDst.m_tMt.SetMcuId( abyMcuId[0] );
					tHeadMsg.m_tMsgDst.m_tMt.SetMtId( abyMcuId[1] );
				}
				else
				{
					tHeadMsg.m_tMsgDst.m_tMt.SetMcuId( abyMcuId[0] );
					tHeadMsg.m_tMsgDst.m_tMt.SetMtId( tMt.GetMtId() );
					tHeadMsg.m_tMsgDst.m_byCasLevel = 0;
				}
			}*/									
		}
		/*else
		{
			tHeadMsg.m_tMsgDst.m_tMt = tMt;
		}*/
		
		
		cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));		
		cServMsg.CatMsgBody((u8 *)&tMt, sizeof(tMt));		
		cServMsg.CatMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
		//SendMsgToMt( tMt.GetMcuId(), MCU_MCU_REINVITEMT_REQ, cServMsg );
		SendMsgToMt( GetFstMcuIdFromMcuIdx(wMcuIdx), MCU_MCU_REINVITEMT_REQ, cServMsg );
		
		if(cServMsg.GetEventId() == MCU_MCU_REINVITEMT_REQ)
		{
			//tMt = m_ptMtTable->GetMt( (u8)tMt.GetMcuId() );
			//tHeadMsgAck.m_tMsgSrc.m_tMt = tMt;
			cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof( TMsgHeadMsg ) );
			cServMsg.CatMsgBody( (u8*)&tMt,sizeof( tMt ) );
		}		
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		return;
	}

	if(cServMsg.GetEventId() == MCU_MCU_REINVITEMT_REQ)
	{		
		//tHeadMsgAck.m_tMsgSrc.m_tMt = tMt;
		cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof( TMsgHeadMsg ) );
		cServMsg.CatMsgBody( (u8*)&tMt,sizeof( tMt ) );
	}
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
 
	//zjj20100928 以下代码意义及其不明确,先注掉
	//删除该终端
	/*if(cServMsg.GetEventId() == MCU_MCU_REINVITEMT_REQ &&
	   // vcs不进行资源删除操作，只进行呼叫、挂断操作
	   // 删除将导致下级PU的IP＋别名信息被删除，而在后续的callalert通告中无法带入特殊的PU别名信息
	   m_tConf.GetConfSource() != VCS_CONF)
	{
		cServMsg.SetMsgBody((u8 *)&tMt, sizeof(tMt));
		SendReplyBack( cServMsg, MCU_MCU_DELMT_NOTIF );
	}*/

	//邀请终端
	if(cServMsg.GetEventId() == MCU_MCU_REINVITEMT_REQ)
	{
		//向重呼的级联mcu发送呼叫通知
		if (VCS_CONF == m_tConf.GetConfSource())
		{
	 		InviteUnjoinedMt( cServMsg, &tMt, FALSE, FALSE );
		}
		else
		{
			InviteUnjoinedMt( cServMsg, &tMt, TRUE, FALSE );
		}
	}
	else
	{
		InviteUnjoinedMt( cServMsg, &tMt );
	}

	return;
}

/*====================================================================
    函数名      ：ProcMcsMcuDropMtReq
    功能        ：会控挂断终端请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
  03/12/09      3.0			JQL			  创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuDropMtReq( const CMessage * pcMsg )
{
	STATECHECK

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt;
	TMsgHeadMsg tHeadMsg,tHeadMsgAck;
	BOOL32 bIsLocal = TRUE;
	u16 wMcuIdx = INVALID_MCUIDX;
	u8 byMtId = 0;
	if(cServMsg.GetEventId() == MCU_MCU_DROPMT_REQ)
	{
		tMt = *( TMt * )(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq));
		if( cServMsg.GetMsgBodyLen() > ( sizeof(TMcuMcuReq) + sizeof( TMt ) ) )
		{
			tHeadMsg = *(TMsgHeadMsg*)( cServMsg.GetMsgBody()+sizeof(TMcuMcuReq) + sizeof(TMt) );
			tHeadMsgAck.m_tMsgSrc = tHeadMsg.m_tMsgDst;
			tHeadMsgAck.m_tMsgDst = tHeadMsg.m_tMsgSrc;		
		}
		
		bIsLocal = tMt.IsMcuIdLocal();
		wMcuIdx = GetMcuIdxFromMcuId( (u8)tMt.GetMcuId() );
		//m_tConfAllMcuInfo.GetIdxByMcuId( tMt.GetMcuId(),0,&wMcuIdx );
		byMtId = tMt.GetMtId();
	}
	else
	{
		tMt = *( TMt * )cServMsg.GetMsgBody();
		bIsLocal = tMt.IsLocal();
		wMcuIdx = tMt.GetMcuIdx();
		byMtId = tMt.GetMtId();
	}

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcsMcuDropMtReq] Drop Mt(%d.%d.%d) level.%d\n",wMcuIdx,byMtId,
					tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],tHeadMsg.m_tMsgDst.m_byCasLevel 
					);

	//不在受邀终端列表中, NACK!
	TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(wMcuIdx);//tMt.GetMcuId());
	//会议表中没有
	if(tMtInfo.IsNull())
	{
		if(cServMsg.GetEventId() == MCU_MCU_DROPMT_REQ)
		{
			cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof( TMsgHeadMsg ) );
		}
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	//已经不在会议中,NACK
	if( !tMtInfo.MtJoinedConf( byMtId ) )//tMt.GetMtId()
	{	
		if(cServMsg.GetEventId() == MCU_MCU_DROPMT_REQ)
		{
			cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof( TMsgHeadMsg ) );
		}
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	//该终端是非直连终端
	if( !bIsLocal )//!tMt.IsLocal())
	{
		TMcuMcuReq tReq;
		TMcsRegInfo	tMcsReg;
		g_cMcuVcApp.GetMcsRegInfo( cServMsg.GetSrcSsnId(), &tMcsReg );
		astrncpy( tReq.m_szUserName, tMcsReg.m_achUser, 
				  sizeof(tReq.m_szUserName), sizeof(tMcsReg.m_achUser) );
		astrncpy( tReq.m_szUserPwd, tMcsReg.m_achPwd, 
				  sizeof(tReq.m_szUserPwd), sizeof(tMcsReg.m_achPwd) );

		if(cServMsg.GetEventId() != MCU_MCU_DROPMT_REQ)
		{
			//u8 byFstMcuId = 0,bySecMcuId = 0;
			u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
			memset( &abyMcuId[0],0,sizeof(abyMcuId) );
			memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );
			tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tMt );
			/*tMt = tHeadMsg.m_tMsgDst.m_tMt;
			if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) )
			{			
				m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0] );
				if( 0 != abyMcuId[1] )
				{
					tHeadMsg.m_tMsgDst.m_byCasLevel = 1;
					tHeadMsg.m_tMsgDst.m_abyMtIdentify[0] = tMt.GetMtId();
					tHeadMsg.m_tMsgDst.m_tMt.SetMcuId( abyMcuId[0] );
					tHeadMsg.m_tMsgDst.m_tMt.SetMtId( abyMcuId[1] );
				}
				else
				{
					tHeadMsg.m_tMsgDst.m_tMt.SetMcuId( abyMcuId[0] );
					tHeadMsg.m_tMsgDst.m_tMt.SetMtId( tMt.GetMtId() );
					tHeadMsg.m_tMsgDst.m_byCasLevel = 0;
				}				
			}	*/								
		}
		/*else
		{
			tHeadMsg.m_tMsgDst.m_tMt = tMt;
		}*/
		
		
		cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
		cServMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt));
		cServMsg.CatMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
		
		SendMsgToMt( (u8)tMt.GetMcuId(), MCU_MCU_DROPMT_REQ, cServMsg );

		if(cServMsg.GetEventId() == MCU_MCU_DROPMT_REQ)
		{
			//tMt = m_ptMtTable->GetMt( (u8)tMt.GetMcuId() );
			//tHeadMsgAck.m_tMsgSrc.m_tMt = tMt;
			cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof( TMsgHeadMsg ) );
			cServMsg.CatMsgBody( (u8*)&tMt,sizeof( tMt ) );
		}
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

		return;
	}
	if(cServMsg.GetEventId() == MCU_MCU_DROPMT_REQ)
	{
		//tHeadMsgAck.m_tMsgSrc.m_tMt = tMt;
		cServMsg.SetMsgBody( (u8*)&tHeadMsgAck,sizeof( TMsgHeadMsg ) );
		cServMsg.CatMsgBody( (u8*)&tMt,sizeof( tMt ) );
	}
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	tMt = m_ptMtTable->GetMt(tMt.GetMtId());		
	//设置终端呼叫模式
	// 对于VCS会议呼叫模式有着特殊的含义(区分固定资源和可调度资源)不可更改
	if (m_tConf.GetConfSource() != VCS_CONF)
	{
		m_ptMtTable->SetCallMode( tMt.GetMtId(),  CONF_CALLMODE_NONE);
	}
	else
	{
		//zjj20100329 vcs会议，下级手动挂断后设置为手动呼叫
		if( !( tMt==m_tConf.GetChairman() ) &&
			!(tMt == m_cVCSConfStatus.GetVcsBackupChairMan())
			)//&& CONF_CREATE_MT == m_byCreateBy)
		{
			m_ptMtTable->SetCallMode( tMt.GetMtId(),  CONF_CALLMODE_NONE);
			//zjj20100327
			TMtStatus tMtStatus;
			if( m_ptMtTable->GetMtStatus(tMt.GetMtId(),&tMtStatus) )
			{
				tMtStatus.SetMtDisconnectReason( MTLEFT_REASON_NORMAL );
				m_ptMtTable->SetMtStatus( tMt.GetMtId(),&tMtStatus );
			}
		}
		//vcs会议不能删除主席
		if( tMt == m_tConf.GetChairman() ||
			tMt == m_cVCSConfStatus.GetVcsBackupChairMan() )
		{
			return;
		}
	}

    cServMsg.SetMsgBody((u8*)&m_ptMtTable->m_atMtExt[tMt.GetMtId()-1], sizeof(TMtExt));
    SendMsgToAllMcs(MCU_MCS_CALLMTMODE_NOTIF, cServMsg);
		
	RemoveJoinedMt( tMt, TRUE );		
}

/*====================================================================
    函数名      ：ProcMcuMcuSetCallMtModeReq
    功能        ：设置终端呼叫模式
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
  03/12/09      3.0			JQL			  创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuSetCallMtModeReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt = *( TMt * )cServMsg.GetMsgBody();
	u8			byCallMode = *( u8* )( cServMsg.GetMsgBody() + sizeof(TMt) );

	switch( CurState() )
	{
	case STATE_ONGOING:
		
		if( !m_tConfAllMtInfo.MtInConf( tMt.GetMtId() ) )	//not in conference
		{
			cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}

		//无效的会议呼叫策略,拒绝
		if( m_tConf.m_tStatus.GetCallMode() > CONF_CALLMODE_TIMER )
		{
			cServMsg.SetErrorCode( ERR_MCU_INVALID_CALLMODE );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
			return;
		}
		
		m_ptMtTable->SetCallMode( tMt.GetMtId(), byCallMode );
		if( CONF_CALLMODE_TIMER == byCallMode )
		{
			m_ptMtTable->SetCallLeftTimes( tMt.GetMtId(), m_tConf.m_tStatus.GetCallTimes() );
		}
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		
	    cServMsg.SetMsgBody((u8*)&m_ptMtTable->m_atMtExt[tMt.GetMtId()-1], sizeof(TMtExt));
        SendMsgToAllMcs(MCU_MCS_CALLMTMODE_NOTIF, cServMsg);	
		//sgx20100703[Bug00033516]向上级mcu发送本级直连某终端的状态报告
		OnNtfMtStatusToMMcu(m_tCascadeMMCU.GetMtId(),tMt.GetMtId());

		break;
		
	case STATE_SCHEDULED:
		break;
	default:
		ConfPrint( LOG_LVL_WARNING, MID_MCU_CONF, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
		
}

/*====================================================================
    函数名      ：ProcMtMcuFastUpDate
    功能        ：终端请求关键帧
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/06/03      3.0			胡昌威		  创建
====================================================================*/
void CMcuVcInst::ProcMtMcuFastUpdatePic( const CMessage * pcMsg )
{
	STATECHECK

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	u8 bySrcMtId = cServMsg.GetSrcMtId();
	u8 byMode    = *(cServMsg.GetMsgBody());

	u32 dwTimeIntervalTicks = 3*OspClkRateGet();

    // 请求关键帧的终端, zgc, 2008-08-15
    TMt tMt = m_ptMtTable->GetMt( bySrcMtId );
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "Mt%u req KF.%u\n", bySrcMtId, byMode );

	TMt  tSrcMt;
	tSrcMt.SetNull();
	//获取请求终端的源信息
	BOOL32 bRet = m_ptMtTable->GetMtSrc(bySrcMtId, &tSrcMt, byMode);

	// [pengjie 2010/9/7] 由于终端表里面没有记录该终端的双流视频源，所以这里是双流就不能return，
	// 在后面判双流源是否为空，为空再return
	if (!bRet && byMode != MODE_SECVIDEO)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMtMcuFastUpdatePic] GetMtSrc failed!\n");
		return;
	}

	switch (byMode)
	{
	case  MODE_AUDIO:
		{
			//是否要过适配
			// [pengjie 2010/11/3] 上级mcu来请求关键帧应该走选看（适配）
			if (tSrcMt == GetVidBrdSrc() && !(m_tCascadeMMCU == tMt))
			{
				if (IsNeedAdapt(tSrcMt, tMt, MODE_AUDIO))
				{
					TBasOutInfo tOutInfo;
					//bas编关键帧
					if(FindBasChn2BrdForMt(tMt.GetMtId(), MODE_AUDIO, tOutInfo))
					{
						NotifyFastUpdate(tOutInfo.m_tBasEqp, tOutInfo.m_byChnId);
					}
				}	
				else
				{
					//通知终端编关键帧
					NotifyFastUpdate(tSrcMt, MODE_AUDIO);
				}
			}			
			else
			{
				//选看稍后
				//选看稍后
			//[nizhijun 2010/11/10] 对于级联会议，上级选看下级MCU，作为下级，GetSelectMt不能获得selectsrc
			//	TMtStatus tMtStatus;
			//	m_ptMtTable->GetMtStatus(bySrcMtId, &tMtStatus);
			//	TMt tSelectSrc = tMtStatus.GetSelectMt(MODE_AUDIO);
				if (!tSrcMt.IsNull())
				{
					if (IsNeedSelAdpt(tSrcMt, tMt, MODE_AUDIO))
					{
						TBasOutInfo tOutInfo;
						if(FindBasChn2SelForMt(tSrcMt, tMt, MODE_AUDIO,tOutInfo))
						{
							NotifyFastUpdate(tOutInfo.m_tBasEqp, tOutInfo.m_byChnId);
						}
					}
					else
					{
						//通知终端编关键帧
						NotifyFastUpdate(tSrcMt, MODE_AUDIO);
					}
				}
				else
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFastUpdatePic:Aud] Mt(McuId:%d, MtId:%d)'s SelectSrc is null!\n",
						tSrcMt.GetMcuId(), 	tSrcMt.GetMtId());
				}
			}
		}
		break;
	case MODE_VIDEO:
		{
			BOOL32 bNeglect = FALSE;
			
			if( VCS_CONF == m_tConf.GetConfSource() &&
				m_byCreateBy != CONF_CREATE_MT &&
				m_tConf.GetChairman().GetMtId() == bySrcMtId )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFastUpdatePic] bySrcMtId.%d is chariman,so not neglect fastupdate\n",bySrcMtId );
				bNeglect = TRUE;
			}

			//zjj20120309 轮回时间内忽略关键帧请求fix2代码合并
			if( !bNeglect && 0 != g_cMcuVcApp.GetMtFastUpdateNeglectNum() &&
				( ( TYPE_MCUPERI == tSrcMt.GetType() &&
						EQP_TYPE_VMP == tSrcMt.GetEqpType() && g_cMcuVcApp.IsBrdstVMP(m_tVmpEqp) ) ||
					( tSrcMt == GetVidBrdSrc() && !(m_tCascadeMMCU == tMt) ) 
					)
				)
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFastUpdatePic] In FastUpdate Interval Logical OspClkRateGet().%d\n",OspClkRateGet() );
				if( 0 == m_dwBrdFastUpdateTick )
				{
					m_dwBrdFastUpdateTick = OspTickGet();
					m_byFastNeglectNum = 1;							
				}
				else
				{
					u32 dwTickCount = OspTickGet();
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFastUpdatePic] m_dwBrdFastUpdateTick.%d minus dwTickCount.%d is lager than OspClkRateGet(%d) * g_cMcuVcApp.GetMtFastUpdateInterval(%d) neglect.%d m_byFastNeglectNum.%d\n",
									m_dwBrdFastUpdateTick,dwTickCount,OspClkRateGet(),g_cMcuVcApp.GetMtFastUpdateInterval(),
									g_cMcuVcApp.GetMtFastUpdateNeglectNum(),m_byFastNeglectNum
									);
					if( ( dwTickCount - m_dwBrdFastUpdateTick ) > ( OspClkRateGet() * g_cMcuVcApp.GetMtFastUpdateInterval() )						
						)
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFastUpdatePic] Clear FastUpdateTick\n" );
						m_dwBrdFastUpdateTick = OspTickGet();
						m_byFastNeglectNum = 1;						
					}
					else
					{
						if( m_byFastNeglectNum++ > g_cMcuVcApp.GetMtFastUpdateNeglectNum() )
						{
							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFastUpdatePic] FastNeglectNum(%d) is large.Fail to fast update pic\n",m_byFastNeglectNum );
							return;
						}
						
					}
				}
			}			

		   // pengjie 级联画面合成下级请求关键帧问题，先判画面合成
			if (TYPE_MCUPERI == tSrcMt.GetType() &&
				EQP_TYPE_VMP == tSrcMt.GetEqpType())
			{
				// 单独请求1路

				u8 bySrcChnnl = GetVmpOutChnnlByDstMtId( tMt.GetMtId(), tSrcMt.GetEqpId());
				if (0xFF == bySrcChnnl) 
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_MT2, "[ProcMtMcuFastUpdatePic] get mt.%d vmp out chnnl failed!\n", 
					tMt.GetMtId() );
					return;
				}
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFastUpdatePic] Ask VMP.%d (chnnl.%d) to setfastupdate!\n", 
					tSrcMt.GetEqpId(), bySrcChnnl);
				NotifyFastUpdate(tSrcMt, bySrcChnnl); 
				
				return;
			}

			//是否要过适配
			// [pengjie 2010/11/3] 上级mcu来请求关键帧应该走选看（适配）
			if (tSrcMt == GetVidBrdSrc() && !(m_tCascadeMMCU == tMt))
			{
				if (IsNeedAdapt(tSrcMt, tMt, MODE_VIDEO))
				{
					TBasOutInfo tOutInfo;
					//bas编关键帧
					if(FindBasChn2BrdForMt(tMt.GetMtId(), MODE_VIDEO, tOutInfo))
					{
						NotifyFastUpdate(tOutInfo.m_tBasEqp, tOutInfo.m_byChnId);
					}
				}				
				else
				{
					//通知终端编关键帧
					NotifyFastUpdate(tSrcMt, MODE_VIDEO);
				}
			}			
			else
			{
				//选看稍后
			//[nizhijun 2010/11/10] 对于级联会议，上级选看下级MCU，作为下级，GetSelectMt不能获得selectsrc
			//	TMtStatus tMtStatus;
			//	m_ptMtTable->GetMtStatus(bySrcMtId, &tMtStatus);
			//	TMt tSelectSrc = tMtStatus.GetSelectMt(MODE_VIDEO);
				if (!tSrcMt.IsNull())
				{
					if (IsNeedSelAdpt(tSrcMt, tMt, MODE_VIDEO))
					{
						TBasOutInfo tOutInfo;
						if(FindBasChn2SelForMt(tSrcMt, tMt, MODE_VIDEO, tOutInfo))
						{
							NotifyFastUpdate(tOutInfo.m_tBasEqp,tOutInfo.m_byChnId);
						}
					}
					else
					{
						//通知终端编关键帧
						NotifyFastUpdate(tSrcMt, MODE_VIDEO);
					}
				}
				else
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ProcMtMcuFastUpdatePic:Vid] Mt(McuId:%d, MtId:%d)'s SelectSrc is null!\n",
									tSrcMt.GetMcuId(), 	tSrcMt.GetMtId());
				}
			}
		}
		break;
	case MODE_SECVIDEO:
		{
			tSrcMt = m_ptMtTable->GetMtSelMtByMode(tMt.GetMtId(), MODE_VIDEO2SECOND);
			
			//双选看
			if (!tSrcMt.IsNull())
			{
				if (IsNeedSelAdpt(tSrcMt, tMt, MODE_VIDEO2SECOND))
				{
					TBasOutInfo	tOutInfo;
					//bas编关键帧
					if(FindBasChn2SelForMt(tSrcMt, tMt, MODE_VIDEO2SECOND, tOutInfo))
					{
						NotifyFastUpdate(tOutInfo.m_tBasEqp, tOutInfo.m_byChnId);
					}
				}
				else
				{
					NotifyFastUpdate(tSrcMt, MODE_VIDEO);
				}
				return;
			}
			
			//是否要过适配
			if (!m_tDoubleStreamSrc.IsNull())
			{
				if (IsNeedAdapt(m_tDoubleStreamSrc, tMt, MODE_SECVIDEO))
				{
					TBasOutInfo	tOutInfo;
					//bas编关键帧
					if(FindBasChn2BrdForMt(tMt.GetMtId(), MODE_SECVIDEO, tOutInfo))
					{
						NotifyFastUpdate(tOutInfo.m_tBasEqp, tOutInfo.m_byChnId);
					}
				}	
				else
				{
					//通知终端编关键帧
					NotifyFastUpdate(m_tDoubleStreamSrc, MODE_SECVIDEO);
				}
			}
			else if (!m_tSecVidBrdSrc.IsNull())
			{
				TBasOutInfo tBasOutInfo;
				if (m_tConf.GetConfAttrb().IsUseAdapter() 
					&& IsNeedAdapt(m_tSecVidBrdSrc, tMt, MODE_VIDEO2SECOND)
					&& FindBasChn2BrdForMt(tMt.GetMtId(), MODE_VIDEO2SECOND, tBasOutInfo))
				{
				}	
				else
				{
					NotifyFastUpdate(m_tSecVidBrdSrc, MODE_VIDEO);
				}
			}
			else
			{
				//选看稍后
			}
		}

		break;
	default:
		break;

	}

}

/*====================================================================
    函数名      ：ProcMtMcuMixReq
    功能        ：处理mt->mcu混音请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
  05/03/08      3.6			Jason		  创建
====================================================================*/
void CMcuVcInst::ProcMtMcuMixReq( const CMessage * pcMsg )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );

    STATECHECK

    //来自非主席终端，nack
    if (cServMsg.GetSrcMtId() != 0 && 
        cServMsg.GetSrcMtId() != m_tConf.GetChairman().GetMtId())
    {
        cServMsg.SetErrorCode(ERR_MCU_INVALID_OPER);
        SendReplyBack(cServMsg, cServMsg.GetEventId() + 2);
        //取消主席
        SendReplyBack(cServMsg, MCU_MT_CANCELCHAIRMAN_NOTIF);
        return;
    }
	
	// [11/10/2010 xueliang] translate mt msg
	if( MT_MCU_STARTDISCUSS_REQ == cServMsg.GetEventId() )
	{
		u8 byMtNum = (cServMsg.GetMsgBodyLen()-  sizeof(u8)) / sizeof(TMt);
		if (byMtNum != 0)
		{
			CServMsg cTransMsg = cServMsg;
			u8 byDissMtNum = (cServMsg.GetMsgBodyLen()-  sizeof(u8)) / sizeof(TMt);
			cTransMsg.SetMsgBody((u8*)&byDissMtNum, sizeof(byDissMtNum));
			TMt tMt;
			for( u8 bylp = 0; bylp < byMtNum; bylp ++ )
			{
				TMt *ptMt = (TMt *)(cServMsg.GetMsgBody() + sizeof(u8) + sizeof(TMt) * bylp);
				if (ptMt == NULL)
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_MIXER, "[ProcMtMcuMixReq]ptMt == NULL,So Return!\n");
					return;
				}
				tMt = *ptMt;
				tMt.SetMcuId(m_ptMtTable->GetMt(tMt.GetMtId()).GetMcuId());
				cTransMsg.CatMsgBody((u8*)&tMt, sizeof(TMt));
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MIXER, "[ProcMtMcuMixReq] tmt after trans is (%d,%d)\n", tMt.GetMcuId(), tMt.GetMtId());
			}
			cServMsg = cTransMsg;
		}
	}

	switch(pcMsg->event)
	{
	case MT_MCU_STARTDISCUSS_REQ:      //终端开始会议讨论请求
		{
            ProcMixStart(cServMsg);
			break;
		}
    case MT_MCU_STOPDISCUSS_REQ:      //会议控制台请求MCU结束会议讨论
		{
            ProcMixStop(cServMsg);
			break;
		}
	 default:	break;
	}
}

/*=============================================================================
    函 数 名： StopDoubleStream
    功    能： 
    算法实现： 
    全局变量： 
    参    数： BOOL32 bMsgSrcMt 是否通知双流源关闭通道，mcu主动关闭通道时使用
	           BOOL32 bForceClose 是否强制双流源关闭通道，退出会议时使用
			   BOOL32 bClearLastDsInfo  是否清空上次双流源的信息(m_byLastDsSrcMtId 和 m_tLogicChnnl)
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/02/19  3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::StopDoubleStream( BOOL32 bMsgSrcMt, BOOL32 bForceClose, BOOL32 bIsNeedAdjustVmpParam  )
{
	KillTimer( MCUVC_NOTIFYMCUH239TOKEN_TIMER );

	TMt tOldDStreamSrc = m_tDoubleStreamSrc;

	if(m_tDoubleStreamSrc.IsNull())
	{
		return;
	}

    TLogicalChannel tNullLogicChnnl;
	TLogicalChannel tH239LogicChnnl;
	CServMsg cServMsg;
	
	// 对于KEDAMCU其双流通道的关闭仅由双流源自身控制，接收双流方不可关闭该通道,仅有权停止转发该码流
	if (MT_MANU_KDCMCU == m_ptMtTable->GetManuId(m_tDoubleStreamSrc.GetMtId()))
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[StopDoubleStream]not permit to operate the dual chanl of mcu\n");
	}
	//sony G70SP在H263-H239时采用活动双流,且不使用令牌而使用FlowControlIndication进行发送激活或关闭 或者
	//POLYCOM7000的静态双流时，使用令牌进行发送激活或关闭
	//均在呼叫建立时即打开,之后与会期间保留通道，直至退出会议
	else if( ( MT_MANU_SONY == m_ptMtTable->GetManuId(m_tDoubleStreamSrc.GetMtId()) && 
		  MEDIA_TYPE_H263 == m_tConf.GetCapSupport().GetDStreamMediaType() &&
          m_tConf.GetCapSupport().IsDStreamSupportH239()) || 
		MT_MANU_POLYCOM == m_ptMtTable->GetManuId(m_tDoubleStreamSrc.GetMtId()) )
	{
        if( TRUE == bMsgSrcMt && 
            m_tConfAllMtInfo.MtJoinedConf( m_tDoubleStreamSrc.GetMtId() ) && 
            m_ptMtTable->GetMtLogicChnnl( m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tH239LogicChnnl, FALSE ) )			
        {
            if( TRUE == bForceClose )
            {
                //即使强制关通道，也不关polycomMCU的双流通道
				if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(m_tDoubleStreamSrc.GetMtId()) &&
					( MT_TYPE_MMCU == m_ptMtTable->GetMtType(m_tDoubleStreamSrc.GetMtId()) ||
					MT_TYPE_SMCU == m_ptMtTable->GetMtType(m_tDoubleStreamSrc.GetMtId()) ) )
				{				
					// xliang [11/14/2008]  Poly
					u16 wBitRate =  0;		
					tH239LogicChnnl.SetFlowControl( wBitRate );
					cServMsg.SetMsgBody( (u8*)&tH239LogicChnnl, sizeof( tH239LogicChnnl ) );
					SendMsgToMt( m_tDoubleStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg );
				}
				else
				{
					u8 byOut = 0;
					cServMsg.SetMsgBody( ( u8 * )&tH239LogicChnnl, sizeof( tH239LogicChnnl ) );	
					cServMsg.CatMsgBody( &byOut, sizeof(byOut) );
					SendMsgToMt( m_tDoubleStreamSrc.GetMtId(), MCU_MT_CLOSELOGICCHNNL_CMD, cServMsg );
					m_ptMtTable->SetMtLogicChnnl( m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tNullLogicChnnl, FALSE );
				}
			}
            else
            {
                u16 wBitRate =  0;		
                tH239LogicChnnl.SetFlowControl( wBitRate );
                cServMsg.SetMsgBody( (u8*)&tH239LogicChnnl, sizeof( tH239LogicChnnl ) );
                SendMsgToMt( m_tDoubleStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg );
            }
		}
	}
	else
	{
		if( TRUE == bMsgSrcMt && 
			m_tConfAllMtInfo.MtJoinedConf( m_tDoubleStreamSrc.GetMtId() ) && 
			m_ptMtTable->GetMtLogicChnnl( m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tH239LogicChnnl, FALSE ) )			
		{
			u8 byOut = 0;
			cServMsg.SetMsgBody( ( u8 * )&tH239LogicChnnl, sizeof( tH239LogicChnnl ) );	
			cServMsg.CatMsgBody( &byOut, sizeof(byOut) );
			SendMsgToMt( m_tDoubleStreamSrc.GetMtId(), MCU_MT_CLOSELOGICCHNNL_CMD, cServMsg );
			//卫星会议下，如果是卫星终端主动离会，双流源需要让该终端停止发送双流，否则计数会错误
			if( IsMultiCastMt(m_tDoubleStreamSrc.GetMtId()) )
			{
				CServMsg cTempServMsg;
				tH239LogicChnnl.SetFlowControl(0);
				cTempServMsg.SetMsgBody((u8*)&tH239LogicChnnl, sizeof(tH239LogicChnnl));
				SendMsgToMt(m_tDoubleStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cTempServMsg);
			}
			m_ptMtTable->SetMtLogicChnnl( m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tNullLogicChnnl, FALSE );
		}
	}

    // zbq [09/05/2007] 录像机双流放象需拆对应的双流桥
    u8 bySrcChn = m_tDoubleStreamSrc == m_tPlayEqp ? m_byPlayChnnl : 0;
    g_cMpManager.RemoveSwitchBridge(m_tDoubleStreamSrc, bySrcChn, MODE_SECVIDEO);
    //g_cMpManager.RemoveSwitchBridge( m_tDoubleStreamSrc, 1, MODE_VIDEO );

    //组播会议组播数据
    if (m_tConf.GetConfAttrb().IsMulticastMode())
    {
        g_cMpManager.StopMulticast(m_tDoubleStreamSrc, 0, MODE_SECVIDEO);
    }
    //卫星组播会议
    if (m_tConf.GetConfAttrb().IsSatDCastMode())
    {
        g_cMpManager.StopDistrConfCast(m_tDoubleStreamSrc, MODE_SECVIDEO);
    }

    // 通知会控终端双流状态改变 [02/05/2007-zbq]
    if ( TYPE_MCUPERI != m_tDoubleStreamSrc.GetType() )
    {
        // guzh [3/1/2007]
		//对于KDCMCU是在等到关闭逻辑通道后再置状态		
		if (  MT_MANU_KDCMCU  != m_ptMtTable->GetManuId( m_tDoubleStreamSrc.GetMtId() ))
		{
			m_ptMtTable->SetMtVideo2Send( m_tDoubleStreamSrc.GetMtId(), FALSE );
		}
		else
		{
			TMt tRealDsMt = GetConfRealDsMt();
			if (!tRealDsMt.IsNull())
			{
				TMtStatus tMtStatus;
				GetMtStatus(tRealDsMt,tMtStatus);
				tMtStatus.SetSndVideo2(FALSE);
				SetMtStatus(tRealDsMt,tMtStatus);
				MtStatusChange(&tRealDsMt,TRUE);
				if (!m_tConfAllMcuInfo.IsSMcuByMcuIdx(tRealDsMt.GetMcuIdx()))
				{
					u8 abyMcuId[MAX_CASCADEDEPTH-1];
					memset( &abyMcuId[0],0,sizeof(abyMcuId) );
					m_tConfAllMcuInfo.GetMcuIdByIdx(tRealDsMt.GetMcuIdx(),abyMcuId);
					tRealDsMt.SetMcuIdx( GetMcuIdxFromMcuId( &abyMcuId[0] ));
					tRealDsMt.SetMtId( abyMcuId[1] );
					GetMtStatus(tRealDsMt,tMtStatus);
					tMtStatus.SetSndVideo2(FALSE);
					SetMtStatus(tRealDsMt,tMtStatus);
					MtStatusChange(&tRealDsMt,TRUE);
				}
			}
		}
        /*TMtStatus tMtStatus;
        m_ptMtTable->GetMtStatus(m_tDoubleStreamSrc.GetMtId(), &tMtStatus);
                
        CServMsg cSendServMsg;
        cSendServMsg.SetSrcMtId( m_tDoubleStreamSrc.GetMtId() );
        cSendServMsg.SetEventId( MT_MCU_MTSTATUS_NOTIF );
        cSendServMsg.SetMsgBody( (u8*)&tMtStatus, sizeof(TMtStatus) );
        g_cMcuVcApp.SendMsgToConf( m_tConf.GetConfId(), MT_MCU_MTSTATUS_NOTIF,
                            cSendServMsg.GetServMsg(), cSendServMsg.GetServMsgLen() );*/
    }

	TMt atDstMt[MAXNUM_CONF_MT];
	u8 byDstMtNum = 0;
	TMt tDstMt;
	//关闭其它终端的H.239通道
    u8 byLoop = 1;
	for( ; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		//E1 MCU做双流源，停双流时，需要恢复前向码率[10/30/2012 chendaiwei]
		if( m_ptMtTable->GetMt( byLoop ) == tOldDStreamSrc
			&&  m_ptMtTable->GetMtTransE1(byLoop)
			&& (m_ptMtTable->GetMtType(byLoop) == MT_TYPE_SMCU
			||m_ptMtTable->GetMtType(byLoop) == MT_TYPE_MMCU))
		{
			u16 wMtDialBitrate = m_ptMtTable->GetRcvBandWidth(byLoop);
			m_ptMtTable->SetMtReqBitrate(byLoop, wMtDialBitrate, LOGCHL_VIDEO);
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[StopDoubleStream] Double Src.%d is E1 MCU.restore FL BR to%d.\n",byLoop,wMtDialBitrate);
			
			continue;
		}

		if(m_tConfAllMtInfo.MtJoinedConf(byLoop)
			&& m_ptMtTable->GetMtLogicChnnl(byLoop, LOGCHL_SECVIDEO, &tH239LogicChnnl, TRUE)
			&& m_ptMtTable->GetMtSelMtByMode(byLoop, MODE_VIDEO2SECOND).IsNull())
		{	
			// xliang [11/14/2008] PolycomMCU 双流通道不关，但对非双流源的polycom终端，需释放令牌(目前支持polyMCU主呼，keda被动获取令牌方式)
// 			if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(byLoop) &&
// 				( MT_TYPE_MMCU == m_ptMtTable->GetMtType(byLoop) ||
			// 				MT_TYPE_SMCU == m_ptMtTable->GetMtType(byLoop) ) )
			// 释放令牌环都应该通知非科达MCU(或终端),不应该只限于宝利通MCU，参考如下 [6/24/2013 chendaiwei]
			//[H239]11.2.3  End-user system owns the token and wants to release the token 
			//The end-user system shall send presentationTokenRelease.
			if ( MT_MANU_KDC != m_ptMtTable->GetManuId(byLoop)
				&& MT_MANU_KDCMCU != m_ptMtTable->GetManuId(byLoop))
			{
				if ( !(tOldDStreamSrc == m_ptMtTable->GetMt(byLoop)) )
				{
					TH239TokenInfo tH239TokenInfo;
					CServMsg cMsg;
					cMsg.SetEventId(MCU_MT_RELEASEH239TOKEN_CMD);
					cMsg.SetMsgBody((u8*)&tOldDStreamSrc, sizeof(TMt));
					cMsg.CatMsgBody((u8*)&tH239TokenInfo, sizeof(TH239TokenInfo));
					SendMsgToMt(byLoop, MCU_POLY_RELEASEH239TOKEN_CMD, cMsg);
				}
				
                //如果是polycom的MCU不关通道,
                //其它外厂终端的通道关掉，如果不关，交换不会拆，并且新双流源发起双流，通道已经打开，不会重新建交换
                //而polycom的MCU则是在239token请求和释放的时候拆建交换的
                if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(byLoop) &&
                    ( MT_TYPE_MMCU == m_ptMtTable->GetMtType(byLoop) ||
                    MT_TYPE_SMCU == m_ptMtTable->GetMtType(byLoop) ))
                {
                    continue;
                }
			}
			u8 byOut = 1;
			cServMsg.SetMsgBody( ( u8 * )&tH239LogicChnnl, sizeof( tH239LogicChnnl ) );	
			cServMsg.CatMsgBody( &byOut, sizeof(byOut) );
			SendMsgToMt( byLoop, MCU_MT_CLOSELOGICCHNNL_CMD, cServMsg );
			
			tDstMt = m_ptMtTable->GetMt( byLoop );
			m_ptMtTable->SetMtVideo2Recv(byLoop, FALSE);
			MtStatusChange(&tDstMt, TRUE);
			//zjl 20110510 Mp: StopSwitchToSubMt 接口重载替换 
            //g_cMpManager.StopSwitchToSubMt(tDstMt, MODE_SECVIDEO, TRUE);
			atDstMt[byDstMtNum] = tDstMt;
			byDstMtNum ++;

			//m_ptMtTable->SetMtLogicChnnl( byLoop, LOGCHL_SECVIDEO, &tNullLogicChnnl, TRUE );

            // xsl [4/28/2006] 结束双流时，所有接收双流的终端恢复接收带宽，自动升降速终端依靠其flowcontrol重新调整
            // if (m_ptMtTable->GetMtReqBitrate(tDstMt.GetMtId(), TRUE) == GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tDstMt.GetMtId())))
            if ( m_ptMtTable->GetMtTransE1(tDstMt.GetMtId()) && 
                 m_ptMtTable->GetMtBRBeLowed(tDstMt.GetMtId()) )
            {
                //zbq[01/30/2008] 自动升降速终端此种情况下不予调整，恢复其当前降速带宽
                //u16 wCurFstBR = m_ptMtTable->GetMtReqBitrate(tDstMt.GetMtId(), TRUE);
                //u8  byDStreamScale = m_tConf.GetDStreamScale();
                //u16 wRealBR = wCurFstBR * 100 / (100-byDStreamScale);
                u16 wRealBR = m_ptMtTable->GetLowedRcvBandWidth(tDstMt.GetMtId());
                m_ptMtTable->SetMtReqBitrate(tDstMt.GetMtId(), wRealBR, LOGCHL_VIDEO);
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[StopDoubleStream]tDstMt.%d, MtReqBitrate = %d, adjust restore\n", tDstMt.GetMtId(), wRealBR);                
            }
            else
            {
                u16 wMtDialBitrate = m_ptMtTable->GetRcvBandWidth(tDstMt.GetMtId());
                m_ptMtTable->SetMtReqBitrate(tDstMt.GetMtId(), wMtDialBitrate, LOGCHL_VIDEO);
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT2, "[StopDoubleStream]tDstMt.%d, MtReqBitrate = %d, normally restore\n", tDstMt.GetMtId(), wMtDialBitrate);
            }
		}
		else if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) && 
			     !m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_SECVIDEO, &tH239LogicChnnl, TRUE ) )

		{
			tDstMt = m_ptMtTable->GetMt( byLoop );
			if( !(tOldDStreamSrc == tDstMt) )
			{
				StopSwitchDStreamToFirstLChannel( tDstMt );
			}
		}       
	}

	if (byDstMtNum > 0)
	{
		g_cMpManager.StopSwitchToSubMt(m_byConfIdx, byDstMtNum, atDstMt, MODE_SECVIDEO);
		for (u8 byIdx = 0; byIdx < byDstMtNum; byIdx++)
		{
			m_ptMtTable->SetMtLogicChnnl( atDstMt[byIdx].GetMtId(), LOGCHL_SECVIDEO, &tNullLogicChnnl, TRUE );
		}
	}

    //zbq[02/18/2008] 停止双流，需整理VMP的广播码率
    if ( g_cMcuVcApp.IsBrdstVMP(m_tVmpEqp) )
    {
        AdjustVmpBrdBitRate();
    }
    // guzh [6/28/2007] 停定时器
    KillTimer(MCU_SMCUOPENDVIDEOCHNNL_TIMER);   
    

    // 停发录像机的双流
    if ( m_tRecPara.IsRecDStream() &&  !m_tRecEqp.IsNull() )
    {
        StopSwitchToPeriEqp(m_tRecEqp.GetEqpId(), m_byRecChnnl, FALSE, MODE_SECVIDEO );
    }
    
 	// [5/27/2011 liuxu] 对正在终端录像的录像机通道进行双流交换的处理
 	OnMtRecDStreamSrcChanged(FALSE);

	//第二路视频的丢包重传
	StopBrdPrs(MODE_SECVIDEO);

	//停止hdu选看双流
	TPeriEqpStatus tHduStatus;
	for (u8 byHduId = HDUID_MIN; byHduId <= HDUID_MAX; byHduId++)
	{
		if (!g_cMcuVcApp.IsPeriEqpConnected(byHduId))
		{
			continue;
		}
		if (!g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus))
		{
			continue;
		}
		u8 byChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byHduId));
		for(u8 byChnIdx = 0; byChnIdx < min(byChnNum,MAXNUM_HDU_CHANNEL); byChnIdx++)
		{
			if (TW_MEMBERTYPE_DOUBLESTREAM == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].byMemberType)
			{
				TMt tNullMt;
				tNullMt.SetNull();
				// [2013/03/11 chenbing] HDU多画面目前不支持选看双流,子通道置0
				ChangeHduSwitch(&tNullMt, byHduId, byChnIdx, 0, TW_MEMBERTYPE_DOUBLESTREAM, TW_STATE_STOP, MODE_SECVIDEO);
			}
		}
	}

	//停双流交换
	StopAllBasSwitch(MODE_SECVIDEO);
	//[nizhijun 2010/9/9] 停广播适配
	StopBrdAdapt(MODE_SECVIDEO);
	//设置会议适配模式
	m_tConf.m_tStatus.SetAdaptMode(ADAPT_MODE_SECVID, FALSE);

	//停止双流时同时停止双流监控
	u8 byChannelNum = 0;
	BOOL bStopSecMc = FALSE;
	TMt tSecSrc;
	tSecSrc.SetNull();
	for (u8 byMCIdx = 1; byMCIdx <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byMCIdx++)
	{
		for( u8 byChanId = 0;byChanId < MAXNUM_MC_CHANNL;++byChanId )
		{
			g_cMcuVcApp.GetMcSrc( byMCIdx, &tSecSrc, byChanId , MODE_SECVIDEO);
			if( tSecSrc == m_tDoubleStreamSrc )
			{				
				bStopSecMc = TRUE;
				StopSwitchToMc( byMCIdx, byChanId, TRUE, MODE_SECVIDEO );
				break;
			}
		}
		if (bStopSecMc)
		{
			break;
		}
	}
	
    // xsl [4/29/2006] 恢复适配器码率
//     TSimCapSet tDstSCS, tSrcSCS;
//     if (m_tConf.m_tStatus.IsVidAdapting())
//     {        
//         //目前暂不考虑双速会议分辨率适配恢复...
//         if (IsNeedVidAdapt(tDstSCS, tSrcSCS, m_wVidBasBitrate))
//         {
//             ChangeAdapt(ADAPT_TYPE_VID, m_wVidBasBitrate, &tDstSCS, &tSrcSCS);
//         }        
//     }
//     if (m_tConf.m_tStatus.IsBrAdapting())
//     {
//         if (IsNeedBrAdapt(tDstSCS, tSrcSCS, m_wBasBitrate))
//         {
//             ChangeAdapt(ADAPT_TYPE_BR, m_wBasBitrate, &tDstSCS, &tSrcSCS);
//         }        
//     }

    //zbq[12/11/2008] 高清适配：停双流交换；整理主流适配带宽
//    StopAllHdBasSwitch(TRUE);
//    m_tConf.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_DOUBLEVID, FALSE);

//     if (m_tConf.m_tStatus.IsHdVidAdapting())
//     {
//         RefreshBasParam4AllMt(MODE_VIDEO);
//     }

	//归还令牌环	
    if( MT_MANU_KDCMCU == m_ptMtTable->GetManuId(m_tDoubleStreamSrc.GetMtId()) )
    {
        ClearH239TokenOwnerInfo(NULL);
    }
	else
	{
		ClearH239TokenOwnerInfo(&m_tDoubleStreamSrc);
	}

	if ( !m_tCascadeMMCU.IsNull() && !IsMcu(m_tDoubleStreamSrc) &&  TYPE_MCUPERI != m_tDoubleStreamSrc.GetType() &&
		m_tConfAllMtInfo.MtJoinedConf(m_tCascadeMMCU.GetMtId()) )
	{
		OnNtfMtStatusToMMcu(m_tCascadeMMCU.GetMtId(), m_tDoubleStreamSrc.GetMtId());
	}
	//zbq[09/12/2008] 时序调整
	m_tDoubleStreamSrc.SetNull();
	
	// 画面合成双流跟随调整
	// 停双流时,直接拆双流源到vmp双流跟随通道的交换,刷新vmpParam成员,通知界面,不全调vmp,
	// 防止一个处理调整两次vmp,导致后面那次vmp因vmpnotify未回复而不能调整.
	
	// 双流开启超时需清vmp双流跟随通道
	//1.默认清双流跟随通道中的成员。
	//2.双流源切换时，会先停再开，停双流时不调整vmp，开启时会调整
	if (bIsNeedAdjustVmpParam)
	{
		TEqp tVmpEqp;
		TVMPParam_25Mem tConfVmpParam;
		TPeriEqpStatus tPeriEqpStatus;
		TVMPParam_25Mem tVmpParam;
		u8 byDSChl;
		TVMPMember tVmpMember;
		TVmpChnnlInfo tVmpChnnlInfo;
		TVMPParam_25Mem tLastVmpParam;
		CServMsg cTmpMsg;
		u8 byTmp = 0XFF;//此字段保留,因旧mpu有做处理,为兼容,另开一个字节标识关闭通道
		u8 byAdd = 0;//标识关闭通道
		for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
		{
			if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
			{
				continue;
			}
			tVmpEqp = g_cMcuVcApp.GetEqp( m_abyVmpEqpId[byIdx] );
			tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
			if (tConfVmpParam.GetVMPMode() == CONF_VMPMODE_CTRL)
			{
				if (g_cMcuVcApp.GetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus))
				{
					tVmpParam = tPeriEqpStatus.m_tStatus.tVmp.GetVmpParam();
					//有双流跟随通道
					byDSChl = tVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_DSTREAM);
					if (byDSChl < MAXNUM_VMP_MEMBER)
					{
						tVmpMember = *tVmpParam.GetVmpMember(byDSChl);
						if (!tVmpMember.IsNull())
						{
							tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
							//停交换
							StopSwitchToPeriEqp(tVmpEqp.GetEqpId(), byDSChl, FALSE, MODE_SECVIDEO);
							tVmpChnnlInfo.ClearChnlByVmpPos(byDSChl); //清该通道前适配
							g_cMcuVcApp.SetVmpChnnlInfo(tVmpEqp, tVmpChnnlInfo);
							//双流跟随更新通道
							tVmpMember.SetNull();
							tVmpParam.SetVmpMember(byDSChl, tVmpMember);
							tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tVmpParam);	//外设状态更新
							g_cMcuVcApp.SetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
							//更新会议vmpparam
							if (byDSChl == tConfVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_DSTREAM))
							{
								tConfVmpParam.SetVmpMember(byDSChl, tVmpMember);
								//m_tConf.m_tStatus.SetVmpParam(tVmpParam);
								g_cMcuVcApp.SetConfVmpParam(tVmpEqp, tConfVmpParam);
								// 更新LastVmpParam前,刷新vmp双流跟随通道台标
								RefreshVmpChlMemalias(tVmpEqp.GetEqpId());
								tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tVmpEqp);
								tLastVmpParam.SetVmpMember(byDSChl, tVmpMember);//更新m_tLastVmpParam
								g_cMcuVcApp.SetLastVmpParam(tVmpEqp, tLastVmpParam);
							}
							//告诉外设清除该通道
							{
								LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "[StopDoubleStream]Send MCU_VMP_ADDREMOVECHNNL_CMD To Vmp, byChnnl:%d.\n", byDSChl);
								cTmpMsg.Init();
								cTmpMsg.SetChnIndex(byDSChl);
								cTmpMsg.SetMsgBody( &byTmp, sizeof(byTmp) );
								cTmpMsg.CatMsgBody( &byAdd, sizeof(byAdd) );
								SendMsgToEqp( tVmpEqp.GetMtId(), MCU_VMP_ADDREMOVECHNNL_CMD, cTmpMsg);
							}
							cTmpMsg.Init();
							cTmpMsg.SetEqpId(tVmpEqp.GetEqpId());
							cTmpMsg.SetMsgBody((u8*)&tVmpParam, sizeof(tVmpParam));
							SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cTmpMsg );
							/*cServMsg.SetMsgBody( ( u8 * )&tPeriEqpStatus, sizeof( tPeriEqpStatus ) );
							SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg );*/
						}
					}
				}
			}
		}
	}
	else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "[StopDoubleStream]bIsNeedAdjustVmpParam:%d, Not adjust VmpParam.\n", bIsNeedAdjustVmpParam);
	}

	TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();

    //如果关双流时有终端发言人，则应调整该终端的发送码率恢复
	TLogicalChannel tLogicalChannel;
	if (!tLocalVidBrdSrc.IsNull() && TYPE_MT == tLocalVidBrdSrc.GetType() 
		//双流源是MCU，本级MCU主动停双流情况下(不是收到MT_MCU_CLOSELOGICCHNNL_NOTIF
		//情况下)，不收该双流，但该MCU双流通道发送通道不关闭，故主流发送也不应该调整[1/5/2013 chendaiwei]
		&& (MT_MANU_KDCMCU != m_ptMtTable->GetManuId(tOldDStreamSrc.GetMtId())
		|| !bMsgSrcMt))
    {
        if (m_ptMtTable->GetMtLogicChnnl(tLocalVidBrdSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
        {            
            tLogicalChannel.SetFlowControl(m_ptMtTable->GetSndBandWidth(tLocalVidBrdSrc.GetMtId()));
            cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
            SendMsgToMt(tLocalVidBrdSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
        }
    }    

	//刷新第一路适配参数,码率恢复	
	if (m_tConf.m_tStatus.IsVidAdapting())
	{
		StartBrdAdapt(MODE_VIDEO);
		//补建下级MCU的交换
		//此处补建这条交换，是了规避上下级如果双流比例不同时，一开始下级收广播码流，
		//后来由于发双流，上下级双流比例又不同，导致下级主流接收广播码流需要码率适配，建立了适配交换，
		//此时由于停止双流，导致下级主流码率恢复，需要接收不过适配的码流，故需要补建交换
		//平滑考虑：如果先前该MCU已经接收广播码流，那么重复建交换应该是平滑的
		//该场景只需要双速会议考虑，因为单速会议的话，上下级双流比例不同，同时也会调整广播源码率，所以没问题
		if ( m_tConf.GetSecBitRate() != 0 )
		{
			u8 byMtId = 0 ;
			TMt tTempMcu;
			TMt tVidSrc = GetVidBrdSrc();
			for ( u8 byMcuIdx =0; byMcuIdx < MAXNUM_SUB_MCU; byMcuIdx++ )
			{
				byMtId = GetFstMcuIdFromMcuIdx( byMcuIdx );
				if ( byMtId >0 && byMtId<=MAXNUM_CONF_MT )
				{
					tTempMcu = m_ptMtTable->GetMt( byMtId );
					StartSwitchFromBrd(tVidSrc,0,1,&tTempMcu);
					ConfPrint(LOG_LVL_DETAIL,MID_MCU_CONF,"[StopDoubleStream]rebuild brd switch to submcu:%d\n",byMtId);
				}
			}
		}
	}   
	else
	{
		RefreshBasBrdAdpParam(MODE_VIDEO);
		RefreshBrdBasParam4AllMt(MODE_VIDEO);
	}
    
	//若发双流终端正在发送第一路码流，恢复码率
	if(IsDStreamMtSendingVideo(tOldDStreamSrc))
	{
		if (m_ptMtTable->GetMtLogicChnnl(tOldDStreamSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE)
			//双流主流源是同一MCU，停双流，不收该双流，但该MCU双流通道发送通道不关闭，故主流发送也不应该调整[1/5/2013 chendaiwei]
			&& (MT_MANU_KDCMCU != m_ptMtTable->GetManuId(tOldDStreamSrc.GetMtId())
		     || tOldDStreamSrc.GetMtId() != tLocalVidBrdSrc.GetMtId()))
        {            
            tLogicalChannel.SetFlowControl(m_ptMtTable->GetSndBandWidth(tOldDStreamSrc.GetMtId()));
            cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
            SendMsgToMt(tOldDStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
        }
	}

	// 若有双流放像，则恢复双流放像, zgc, 2007-05-30
	if ( TYPE_MCUPERI != tOldDStreamSrc.GetType() || EQP_TYPE_RECORDER != tOldDStreamSrc.GetEqpType())
	{
		if ( !m_tPlayEqp.IsNull() && !m_tConf.m_tStatus.IsNoPlaying() && m_tPlayEqpAttrib.IsDStreamPlay() && m_tPlayEqpAttrib.IsDStreamFile())
		{
			TLogicalChannel tLogicChnnl;
			tLogicChnnl.SetChannelType(m_tPlayEqpAttrib.GetDVideoType());
			tLogicChnnl.SetSupportH239(m_tConf.GetCapSupport().IsDStreamSupportH239());
			StartDoubleStream(m_tPlayEqp, tLogicChnnl);
		}
	}
	
    // guzh [3/19/2007] 处理其他终端选看的情况
    TMtStatus tStatus;
    TMt tOtherMt;
    for( byLoop = 1 ; byLoop <= MAXNUM_CONF_MT; byLoop++ )
    {
        if ( m_tConfAllMtInfo.MtJoinedConf( byLoop ) && 
             m_ptMtTable->GetMtStatus(byLoop, &tStatus) )
        {
			//如果存在双选看，不调第一选看源的发送
			if (!m_ptMtTable->GetMtSelMtByMode(byLoop, MODE_VIDEO2SECOND).IsNull())
			{
				continue;
			}

			//双流主流源是同一MCU，停双流，不收该双流，但该MCU双流通道发送通道不关闭，故主流发送也不应该调整[1/5/2013 chendaiwei]
			if(MT_MANU_KDCMCU == m_ptMtTable->GetManuId(tOldDStreamSrc.GetMtId())
				&& tOldDStreamSrc.GetMtId() == tLocalVidBrdSrc.GetMtId()
				&& byLoop == tOldDStreamSrc.GetMtId())
			{
				continue;
			}

			if(MT_MANU_KDC != m_ptMtTable->GetManuId(byLoop))
			{
				m_ptMtTable->GetMtLogicChnnl(byLoop, LOGCHL_VIDEO, &tLogicalChannel, FALSE);
				tLogicalChannel.SetCurrFlowControl(0xFFFF);
				tLogicalChannel.SetFlowControl(m_ptMtTable->GetDialBitrate(byLoop));
				cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
                SendMsgToMt(byLoop, MCU_MT_FLOWCONTROL_CMD, cServMsg);
			}
            tOtherMt = tStatus.GetSelectMt(MODE_VIDEO);
			tOtherMt = GetLocalMtFromOtherMcuMt(tOtherMt);
            if ( !tOtherMt.IsNull() )
            {
                // 恢复Flowcontrol的值，在具体发送时计算
                m_ptMtTable->GetMtLogicChnnl(tOtherMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);
                tLogicalChannel.SetCurrFlowControl(0xFFFF);
                tLogicalChannel.SetFlowControl(m_ptMtTable->GetDialBitrate(tOtherMt.GetMtId()));
                cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
                SendMsgToMt(tOtherMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
            }
        }
    }

	// 从vmp双流跟随过渡下来的，有可能这时候卫星终端作为发言人还没被切为广播源 [pengguofeng 3/2/2013]
	/*TMt tSpeaker = m_tConf.GetSpeaker();
	if ( IsSpeakerCanBrdVid( &tSpeaker ) &&
		IsMultiCastMt( tSpeaker.GetMtId())
		&& !(tSpeaker == GetLocalVidBrdSrc() )
		&& !IsSatMtSend(tSpeaker)
		&& !IsSatMtOverConfDCastNum(tSpeaker))
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[StopDoubleStream]Sat Conf recover vid brd src:%d when canceling dual src\n",
			tSpeaker.GetMtId());
		ChangeVidBrdSrc(&tSpeaker);
	}*/
//    RefreshRcvGrp();
	return;
}

/*=============================================================================
    函 数 名： StartDoubleStream
    功    能： 开启双流
    算法实现： 
    全局变量： 
    参    数： TMt &tDstMt
               TLogicalChannel &tLogicChnnl 
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/02/19  3.6			万春雷                创建
	2006/01/19	4.0			张宝卿				  录像机录放象虚能力集的设置
=============================================================================*/
void CMcuVcInst::StartDoubleStream(TMt &tMt, TLogicalChannel &tLogicChnnl)
{
	if (!m_tDoubleStreamSrc.IsNull())
	{
		// xliang [12/19/2008] 增加标记
		// zjl    [03/04/2010] 如果上级mcu是双流源,则不记录其mtid， 保证下级终端发双流抢占时能顺利打开上级双流通道
		//（原来采用延时策略过滤，在上级发送关闭前一次双流通道时才尝试打开上级双流通道，但上级此时不会主动关闭前次双流通道）
		if (MT_MANU_KDCMCU != m_ptMtTable->GetManuId(m_tDoubleStreamSrc.GetMtId()))
		{
			// [pengjie 2010/9/9] 放开对于双流抢发，延后打开上一次双流源的通道的限制
// 			
// 			m_byLastDsSrcMtId = m_tDoubleStreamSrc.GetMtId(); 
		}		
		else
		{	
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[StartDoubleStream] not record last double stream mtid:% because it's keda mcu!\n", tMt.GetMtId());
		}
		StopDoubleStream(TRUE, FALSE, FALSE);//切换双流源时，停双流不用调整vmp
	}

	/*20110805 zjl 终端放像走的是标准令牌环，请求令牌环时mcu侧已经保存了token
				   放像机双流放像时，不会请求令牌环，这里需要判断如果发起了双流，
				   但是token为空，则将token赋为新的双流预案
	*/
	if (m_tH239TokenOwnerInfo.m_tH239TokenMt.IsNull())
	{
		m_tH239TokenOwnerInfo.SetTokenMt(tMt);
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[StartDoubleStream] Set New Token<Type:%d, McuId:%d, MtId:%d>!\n",
									tMt.GetType(), tMt.GetMcuId(), tMt.GetMtId());
	}

    // zbq [09/04/2007] 录像机双流放象，此处还不能建交换
    if ( TYPE_MCUPERI == tMt.GetType() && EQP_TYPE_RECORDER == tMt.GetEqpType())
    {
    }
    else
    {
        g_cMpManager.SetSwitchBridge(tMt, 0, MODE_SECVIDEO);
    }

	// [pengjie 2010/3/10] MCU定时给级联MCU发239令牌环，不管是不是239的双流
    SetTimer( MCUVC_NOTIFYMCUH239TOKEN_TIMER, 10000);
    TCapSupport tCapSupport;
	//打开其它终端的H.239通道
	for(u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
	{
		//E1 MCU做双流源，其主流接收通道也应该调整为减去双流占带宽的带宽。这样后续调整主流广播源时，才能找到实际
		//接收终端前向通道的最小值，从而正确调整主流广播源码率[10/30/2012 chendaiwei]
		if( byLoop == tMt.GetMtId() 
			&& m_ptMtTable->GetMtTransE1(byLoop)
			&& (m_ptMtTable->GetMtType(byLoop) == MT_TYPE_SMCU ||
			    m_ptMtTable->GetMtType(byLoop) == MT_TYPE_MMCU))
		{
			m_ptMtTable->SetMtReqBitrate(byLoop,GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(byLoop)),LOGCHL_VIDEO);

			continue;
		}

        if (byLoop == tMt.GetMtId() && TYPE_MCUPERI != tMt.GetType())
        {
            continue;
        }

        if (!m_tConfAllMtInfo.MtJoinedConf(byLoop))
        {
            continue;
        }

		// 跳过vrs终端录像实体中录像源不是该终端的实体
		if (m_ptMtTable->GetMtType(byLoop) == MT_TYPE_VRSREC)
		{
			TMt tRecSrc = m_ptMtTable->GetRecSrc(byLoop);
			if (m_ptMtTable->GetRecChlType(byLoop) == TRecChnnlStatus::TYPE_RECORD
				&& !tRecSrc.IsNull())
			{
				if (!(GetLocalMtFromOtherMcuMt(tRecSrc) == GetLocalMtFromOtherMcuMt(tMt)))
				{
					// 与录像终端不一致，不录双流
					continue;
				}
			}
		}

		//[2012/8/31 zhangli]如果存在双选看，先停双选看 
		if (!m_ptMtTable->GetMtSelMtByMode(byLoop, MODE_VIDEO2SECOND).IsNull())
		{
			StopSelectSrc(m_ptMtTable->GetMt(byLoop), MODE_VIDEO2SECOND);
		}

        TMt tOtherMt = m_ptMtTable->GetMt(byLoop);

		//zbq[09/18/2008] 双流放象伪装. 帧率临时取会议属性. 低帧率文件会误判.
		if ( TYPE_MCUPERI == tMt.GetType() && EQP_TYPE_RECORDER == tMt.GetEqpType() ) 
		{
			u16 wFileDSW = 0;
			u16 wFileDSH = 0;
			u8 byFileDSType = 0;
			m_tPlayFileMediaInfo.GetDVideo(byFileDSType, wFileDSW, wFileDSH);
			u8 byFileDSRes = GetResByWH(wFileDSW, wFileDSH);

			tLogicChnnl.SetVideoFormat(byFileDSRes);

			if (MEDIA_TYPE_H264 == byFileDSType)
			{
				tLogicChnnl.SetChanVidFPS(m_tConf.GetDStreamUsrDefFPS());
			}
			else
			{
				tLogicChnnl.SetChanVidFPS(m_tConf.GetDStreamUsrDefFPS());
			}

		}

        //普通终端打开双流
        if(!m_ptMtTable->GetMtCapSupport( tOtherMt.GetMtId(), &tCapSupport ))
        {
            continue;
        }
		if (MEDIA_TYPE_NULL == tCapSupport.GetDStreamMediaType())
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[StartDoubleStream] Mt.%d's DsCap is null so continue!\n",
						tOtherMt.GetMtId());
			continue;
		}
        
		// xliang [11/14/2008]  针对polycom双流的特殊处理
		//polycom的双流通道在终端呼入时就确立并完成了打开操作;
		//此处对非令牌Polycom的MT，直接flowctrl其带宽即可. 对Polycom的MCU，需先请求令牌，请求令牌成功后发起flowctrl
		if ( (MT_MANU_POLYCOM == m_ptMtTable->GetManuId(byLoop) ||
              MT_MANU_AETHRA == m_ptMtTable->GetManuId(byLoop) )
			&&
			( MT_TYPE_SMCU == m_ptMtTable->GetMtType(byLoop) || 
			  MT_TYPE_MMCU == m_ptMtTable->GetMtType(byLoop)) )
		{
			CServMsg cServMsg;
			TMt tPolyMCU = m_ptMtTable->GetMt(byLoop);
			TH239TokenInfo tH239TokenInfo;
			
			//请求令牌要判断 其前向通道是否存在， 否则会导致取前向通道句柄异常
			TLogicalChannel tSecVidChnnl;
			if ( m_ptMtTable->GetMtLogicChnnl(byLoop, LOGCHL_SECVIDEO, &tSecVidChnnl, TRUE) )
			{
				cServMsg.SetEventId(MCU_POLY_GETH239TOKEN_REQ);
				cServMsg.SetMsgBody((u8*)&tPolyMCU, sizeof(TMt));
				cServMsg.CatMsgBody((u8*)&tH239TokenInfo, sizeof(tH239TokenInfo));
				SendMsgToMt(byLoop, MCU_POLY_GETH239TOKEN_REQ, cServMsg );
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[StartDoubleStream] MCU_POLY_GETH239TOKEN_REQ to PolyMCU.%d\n", byLoop);
			}
			else
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[StartDoubleStream] No send MCU_POLY_GETH239TOKEN_REQ to polyMCU due to Forward chnnl closed, try to reopen it, start DS once more, please\n" );
			}
		}
		else
		{
			McuMtOpenDoubleStreamChnnl(tOtherMt, tLogicChnnl/*, tCapSupport*/);  
		}
	}

	// fxh[090213]获取原双流源的能力级
	TDStreamCap tOldDSCap;
	if (!m_tDoubleStreamSrc.IsNull())
	{
		GetDSBrdSrcSim(tOldDSCap);
	}

    if (TYPE_MCUPERI == tMt.GetType())
    {
	    m_tDoubleStreamSrc = tMt;
    }
    else
    {
        m_tDoubleStreamSrc = m_ptMtTable->GetMt(tMt.GetMtId());
        // guzh [3/1/2007]
        m_ptMtTable->SetMtVideo2Send( m_tDoubleStreamSrc.GetMtId(), TRUE );
		MtStatusChange(&m_tDoubleStreamSrc,TRUE);

        // 通知会控终端双流状态改变 [02/05/2007-zbq]
        /*TMtStatus tMtStatus;
        m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tMtStatus);
        
        CServMsg cServMsg;
        cServMsg.SetSrcMtId( tMt.GetMtId() );
        cServMsg.SetEventId( MT_MCU_MTSTATUS_NOTIF );
        cServMsg.SetMsgBody( (u8*)&tMtStatus, sizeof(TMtStatus) );
        g_cMcuVcApp.SendMsgToConf( m_tConf.GetConfId(), MT_MCU_MTSTATUS_NOTIF,
                            cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );*/
    }

	//[nizhijun 2010/12/21] 改变PRS双流源
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	if (tConfAttrb.IsResendLosePack() &&!m_tDoubleStreamSrc.IsNull())
	{	
		StartBrdPrs(MODE_SECVIDEO);				
	}

   TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();
    
	//启用适配, 则强制调整第一路发送源的发送码率折半,没有适配器则根据FlowControl调整
    if (!tLocalVidBrdSrc.IsNull() && TYPE_MT == tLocalVidBrdSrc.GetType())
    {
        CServMsg cServMsg;
        TLogicalChannel tLogicalChannel;
        if (TRUE == m_ptMtTable->GetMtLogicChnnl(tLocalVidBrdSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
        {
            //如果开双流时有终端发言人，则应调整该终端的发送码率减半，并保存在通道信息中
            tLogicalChannel.SetFlowControl(GetDoubleStreamVideoBitrate(m_ptMtTable->GetSndBandWidth(tLocalVidBrdSrc.GetMtId())));
            cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
            SendMsgToMt(tLocalVidBrdSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
        }
    }

	//级联MCU开双流通道触发的本级发起双流，先调上级MCU（双流源）的码率，避免上下级
	//双流比率不一致，导致本级终端开双流接受通道建交换时误判要过适配[3/21/2013 chendaiwei]
	CServMsg cServMsg;
	TLogicalChannel tDSLogicChannel;
	if (TRUE == m_ptMtTable->GetMtLogicChnnl(m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tDSLogicChannel, FALSE)
		&& (m_tDoubleStreamSrc.GetMtType() == MT_TYPE_SMCU || m_tDoubleStreamSrc.GetMtType() == MT_TYPE_MMCU))
	{
		tDSLogicChannel.SetFlowControl(GetDoubleStreamVideoBitrate(m_ptMtTable->GetSndBandWidth(m_tDoubleStreamSrc.GetMtId()),FALSE));
		cServMsg.SetMsgBody((u8*)&tDSLogicChannel, sizeof(tDSLogicChannel));
		SendMsgToMt(m_tDoubleStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
	}

//	RefreshRcvGrp();

    //双流录像
    //[2011/07/16/zhanlgi]整理录像机拆建RTP/RTCP交换
	AdjustConfRecordSrcStream(MODE_SECVIDEO, TRUE);

	// [5/27/2011 liuxu] 对正在终端录像的录像机通道进行双流交换的处理
	OnMtRecDStreamSrcChanged(TRUE);

    //hdu双流选看调整
	TPeriEqpStatus tHduStatus;
	for (u8 byHduId = HDUID_MIN; byHduId <= HDUID_MAX; byHduId++)
	{
		if (!g_cMcuVcApp.IsPeriEqpConnected(byHduId))
		{
			continue;
		}
		if (!g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus))
		{
			continue;
		}
		u8 byChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byHduId));
		for(u8 byChnIdx = 0; byChnIdx < min(byChnNum,MAXNUM_HDU_CHANNEL); byChnIdx++)
		{
			if (TW_MEMBERTYPE_DOUBLESTREAM == tHduStatus.m_tStatus.tHdu.atVideoMt[byChnIdx].byMemberType)
			{
				// [2013/03/11 chenbing] HDU多画面目前不支持选看双流,子通道置0
				ChangeHduSwitch(&m_tDoubleStreamSrc, byHduId, byChnIdx, 0, TW_MEMBERTYPE_DOUBLESTREAM, TW_STATE_START, MODE_SECVIDEO);
			}
		}
	}

	u8 byVmpId;
	TEqp tVmpEqp;
	TPeriEqpStatus tPeriEqpStatus;
	TVMPParam_25Mem tVmpParam;
	u16 wError = 0;
	for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
	{
		if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
		{
			continue;
		}
		byVmpId = m_abyVmpEqpId[byIdx];
		tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
		
		// 画面合成双流跟随调整
		if (g_cMcuVcApp.GetVMPMode(tVmpEqp) == CONF_VMPMODE_CTRL)
		{
			if (g_cMcuVcApp.GetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus))
			{
				tVmpParam = tPeriEqpStatus.m_tStatus.tVmp.GetVmpParam();
				//有双流跟随通道
				if (tVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_DSTREAM) < MAXNUM_VMP_MEMBER)
				{
					wError = 0;
					if (CheckVmpParam(byVmpId, tVmpParam, wError))
					{
						// 双流不牵涉级联多回传
						AdjustVmpParam(byVmpId, &tVmpParam, FALSE, FALSE);
					}
					if (wError)
					{
						tVmpParam.Print(LOG_LVL_KEYSTATUS);
						ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[StartDoubleStream] CheckVmpParam failed, wError[%d].\n", wError);
					}
				}
			}
		}
	}
	//无论当前双流适配是否开启，每次都开下, 跟主流靠拢，如果有问题则再更改
	StartBrdAdapt(MODE_SECVIDEO);

//     if (m_cMtRcvGrp.IsNeedAdp(FALSE))
//     {
		//无论当前双流适配是否开启，每次都开下, 跟主流靠拢，如果有问题则再更改
//		StartBrdAdapt(MODE_SECVIDEO);
//         if (!m_tConf.m_tStatus.IsDSAdapting())
//         {
//             StartBrdAdapt(MODE_SECVIDEO);
//         }
//         else
//         {
// 			// fxh[20090213]bas已启动，双流源切换时需判断输入码流参数是否有修改
// 			TDStreamCap tNewDSCap;
// 			GetDSBrdSrcSim(tNewDSCap);
// 			if (tOldDSCap.GetMediaType() != tNewDSCap.GetMediaType())
// 			{
// 				u8 byDSBasNum = 0;
// 
//                 //调整比源能力大的输出参数
//                 TBasChn atBasChn[MAXNUM_CONF_MVCHN];
//                 m_pcBasMgr->GetChnGrp(byDSBasNum, atBasChn, CHN_ADPMODE_DSBRD);
// 
// 				for(u8 byIdx = 0; byIdx < byDSBasNum; byIdx ++)
// 				{
//                     if (!atBasChn[byIdx].IsNull())
//                     {
//                         ChangeHDAdapt(atBasChn[byIdx].GetEqp(), atBasChn[byIdx].GetChnId());
//                     }
//                 }
// 			}
// 			// 输出码流参数调整
//             RefreshBasParam4AllMt(MODE_SECVIDEO);
//             StartAllBasSwitch(MODE_SECVIDEO);
//         }
//    }

	// 刷新第一路适配编码
    if ( m_tConf.m_tStatus.IsVidAdapting() )
    {
		StartBrdAdapt(MODE_VIDEO);
	}
    
    //组播会议组播数据
    if (m_tConf.GetConfAttrb().IsMulticastMode())
    {
        g_cMpManager.StartMulticast(m_tDoubleStreamSrc, 0, MODE_SECVIDEO);
    }

    //卫星组播
    if (m_tConf.GetConfAttrb().IsSatDCastMode())
    {
        g_cMpManager.StartDistrConfCast(m_tDoubleStreamSrc, MODE_SECVIDEO);      
    }

    // guzh [10/30/2007] 
    if (m_tDoubleStreamSrc.GetType() == TYPE_MT)
    {
        //新的双流开启发送时，强制关键帧
        SetTimer(MCUVC_SECVIDEO_FASTUPDATE_TIMER, 1200, 100*m_tDoubleStreamSrc.GetMtId()+1);	       
    }

    // guzh [10/30/2007] 注意该消息的处理，不仅仅是向双流源发送调整码率请求，所以一定要设置定时器
    //等待1s向双流发起终端发送flowcontrol调节双流码率
    SetTimer(MCUVC_SENDFLOWCONTROL_TIMER, 1200);

	//更新当前的双流状态
    if (VCS_CONF == m_tConf.GetConfSource())
    {
//        m_cVCSConfStatus.SetConfDualing(TRUE);
        VCSConfStatusNotif();

        //[11/18/2011 zhushengze]通知VCS下级开始发双流，VCS收到消息后会请求建交换，
        //适用于VCS下级终端抢发双流
        if (m_tDoubleStreamSrc.GetMtType() == MT_TYPE_SMCU)
        {   
			cServMsg.SetEventId( MCU_VCS_STARTDOUBLESTREAM_NOTIFY );
            cServMsg.SetMsgBody( (u8*)&m_tDoubleStreamSrc, sizeof(TMt) );
            SendMsgToAllMcs(MCU_VCS_STARTDOUBLESTREAM_NOTIFY, cServMsg);
            LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_VCS, "send vcs start double stream notify, DSSrc is %d\n", m_tDoubleStreamSrc.GetMtId());
        }
    }

	if ( !m_tCascadeMMCU.IsNull() && /*!IsMcu(m_tDoubleStreamSrc) &&*/  TYPE_MCUPERI != m_tDoubleStreamSrc.GetType() &&
		m_tConfAllMtInfo.MtJoinedConf(m_tCascadeMMCU.GetMtId()) )
	{
		OnNtfMtStatusToMMcu(m_tCascadeMMCU.GetMtId(), m_tDoubleStreamSrc.GetMtId());
	}
	m_byNewTokenReqMtid = 0;
    return;
}


/*=============================================================================
函 数 名： ProcSendFlowctrlToDSMtTimeout
功    能： 发送flowcontrol给双流发起源，因为终端也需要调节双流码率，所有等待1s后在发送flowcontrol，防止终端将码率改回
算法实现： 
全局变量： 
参    数： const CMessage * pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/4/27  4.0			许世林                  创建
=============================================================================*/
void CMcuVcInst::ProcSendFlowctrlToDSMtTimeout( void )
{
    if (m_tDoubleStreamSrc.IsNull())
    {
        ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[ProcSendFlowctrlToDSMtTimeout] no double stream source mt!\n");
        return;
    }

    CServMsg cServMsg0, cServMsg1;
    TLogicalChannel tLogicalChannel;
    if ( ( !IsMultiCastMt(m_tDoubleStreamSrc.GetMtId()) || 
           IsDStreamMtSendingVideo(m_tDoubleStreamSrc) ) &&
         m_ptMtTable->GetMtLogicChnnl(m_tDoubleStreamSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE) )
    {
        tLogicalChannel.SetFlowControl(GetDoubleStreamVideoBitrate(m_ptMtTable->GetSndBandWidth(m_tDoubleStreamSrc.GetMtId())));
        tLogicalChannel.SetCurrFlowControl(0xffff);//将上次保存的flowcontrol改变，防止被过滤
        cServMsg0.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));   
		SendMsgToMt(m_tDoubleStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg0); 
    }
	if (m_ptMtTable->GetMtLogicChnnl(m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE))
    {
		u16 wDSBitrate = GetDoubleStreamVideoBitrate(m_ptMtTable->GetSndBandWidth( m_tDoubleStreamSrc.GetMtId() ), FALSE);
        tLogicalChannel.SetFlowControl(wDSBitrate);
        tLogicalChannel.SetCurrFlowControl(0xffff);//将上次保存的flowcontrol改变，防止被过滤
        cServMsg1.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel)); 
		SendMsgToMt(m_tDoubleStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg1);  
    }

	TMtStatus tStatus;
	u8 byLoop = 1;
	//终端码率不同采用适配策略而不再通过取小来调源终端码率
    
    // guzh [3/19/2007] 重新查找每个终端的选看源，分析是否要调整码率
    TMt tOtherMt;
    u16 wBitrate;
    for (byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        if ( !m_tConfAllMtInfo.MtJoinedConf(byLoop) )
        {
            continue;
        }      
        m_ptMtTable->GetMtStatus(byLoop, &tStatus);
        tOtherMt = tStatus.GetSelectMt(MODE_VIDEO);
		tOtherMt = GetLocalMtFromOtherMcuMt(tOtherMt);
        if ( !tOtherMt.IsNull() &&
             !(tOtherMt == m_tDoubleStreamSrc) )
        {
			// 根据终端当前的发送码率、双方的收发能力确定flowcontrol的值
            m_ptMtTable->GetMtLogicChnnl( tOtherMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);
            
			const u16 wSendBand = m_ptMtTable->GetSndBandWidth(tOtherMt.GetMtId());
			const u16 wRcvBand = m_ptMtTable->GetRcvBandWidth(byLoop);
			wBitrate = min (wSendBand ,  wRcvBand);
            wBitrate = GetDoubleStreamVideoBitrate(wBitrate);

			const u16 wFlowCtrl = tLogicalChannel.GetFlowControl() ;
            wBitrate = min( wBitrate, wFlowCtrl);
            tLogicalChannel.SetFlowControl(wBitrate);
			cServMsg0.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
            SendMsgToMt(tOtherMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg0);
        }
		//调整非KEDA终端主流码流，以便非KEDA终端主流能自环成功（双流发起后会调整非双流源的主流接收，因此要调整其主流发送码率）。
		if (MT_MANU_KDC != m_ptMtTable->GetManuId(byLoop) && MT_TYPE_MT == m_ptMtTable->GetMtType(byLoop))
		{
			m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_VIDEO, &tLogicalChannel, FALSE);
			const u16 wSendBand = m_ptMtTable->GetSndBandWidth(tOtherMt.GetMtId());
			const u16 wRcvBand = m_ptMtTable->GetRcvBandWidth(byLoop);
			wBitrate = min (wSendBand ,  wRcvBand);
            wBitrate = GetDoubleStreamVideoBitrate(wBitrate);
			const u16 wFlowCtrl = tLogicalChannel.GetFlowControl() ;
            wBitrate = min( wBitrate, wFlowCtrl);
            tLogicalChannel.SetFlowControl(wBitrate);
			cServMsg0.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
            SendMsgToMt(tOtherMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg0);
		}
    }

    return;
}

/*=============================================================================
  函 数 名： ProcSmcuOpenDVideoChnnlTimer
  功    能： 下级mcu向上级打开双流通道
  算法实现： 
  全局变量： 
  参    数： const CMessage * pcMsg
  返 回 值： void 
=============================================================================*/
void CMcuVcInst::ProcSmcuOpenDVideoChnnlTimer(const CMessage * pcMsg)
{
	u32 dwMtId = *(u32 *)pcMsg->content;
	TLogicalChannel tLogicalChannel;
	TCapSupport tCapSupport;
	TMt tMt = m_ptMtTable->GetMt((u8)dwMtId);
	if( m_ptMtTable->GetMtLogicChnnl( m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE ) 
		&& m_ptMtTable->GetMtCapSupport((u8)dwMtId, &tCapSupport) )
	{
		McuMtOpenDoubleStreamChnnl(tMt, tLogicalChannel/*, tCapSupport*/);
	}

	return;
}

/*=============================================================================
  函 数 名： OpenDoubleStreamChnnl
  功    能： 打开双流通道
  算法实现： 
  全局变量： 
  参    数： TMt &tMt
             TLogicalChannel &tLogicChnnl
             TCapSupport &tCapSupport
  返 回 值： void 
=============================================================================*/
void CMcuVcInst::McuMtOpenDoubleStreamChnnl(TMt &tMt, 
                                            const TLogicalChannel &tLogicChnnl/*, 
                                            const TCapSupport &tCapSupport*/)
{
	if (TYPE_MCUPERI != m_tDoubleStreamSrc.GetType() && tMt.GetMtId() == m_tDoubleStreamSrc.GetMtId()
		&& m_ptMtTable->GetMtSelMtByMode(tMt.GetMtId(), MODE_VIDEO2SECOND).IsNull())
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[McuMtOpenDoubleStreamChnnl]Mtid(%d)m_tDoubleStreamSrc(Mtid:%d,Type:%d)!So Return!\n",
						tMt.GetMtId(),m_tDoubleStreamSrc.GetMtId(),m_tDoubleStreamSrc.GetType());
		return;
	}

	//[2011/04/25 zhushz] 第一路逻辑通道未打开则不开第二路
	TLogicalChannel tMainVideoLogicalChannel;
	m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tMainVideoLogicalChannel, TRUE);
	if ( tMainVideoLogicalChannel.GetChannelType() == MEDIA_TYPE_NULL ||
		tMainVideoLogicalChannel.GetMediaType() == MODE_NONE )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[McuMtOpenDoubleStreamChnnl] Not Open Mt.%d Second Channel because its first Channel not opened!\n", tMt.GetMtId());
		
		return;
    }

	u32 dwRcvIp = 0;
    u16 wRcvPort = 0;
    TLogicalChannel tDStreamChnnl;
    CServMsg cMsg;

    TCapSupport tCapSupport;
    m_ptMtTable->GetMtCapSupport(tMt.GetMtId(), &tCapSupport);

	// 1.xliang [11/14/2008]  针对polycom双流的特殊处理
	if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId())
		|| ( MT_MANU_AETHRA == m_ptMtTable->GetManuId(tMt.GetMtId()) //对AethraMcu 也作特殊处理
			&& MT_TYPE_MT != m_ptMtTable->GetMtType(tMt.GetMtId()) )
		)
	{
		//Polycom打开双流是无条件的，通道信息不做校验，直接打开
		if ( tCapSupport.IsDStreamSupportH239() 
			&& m_tConf.GetCapSupport().GetDStreamCapSet().GetMediaType() == tCapSupport.GetDStreamMediaType())
		{
			if( !m_ptMtTable->GetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort ) )
			{
				dwRcvIp = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( tMt.GetMtId() ) );
				if( dwRcvIp == 0 )
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "Mt%d 0x%x(Dri:%d) mp not connected(Polycom)!\n",
						tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() );		    
				}
				g_cMcuVcApp.FindMatchedMpIpForMt(m_ptMtTable->GetIPAddr(tMt.GetMtId()),dwRcvIp);
				wRcvPort = g_cMcuVcApp.AssignMtPort( tMt.GetConfIdx(), tMt.GetMtId() );
				m_ptMtTable->SetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort );
			}
			
			tDStreamChnnl.SetSupportH239( tCapSupport.IsDStreamSupportH239() );
			NotifyH239TokenOwnerInfo( &tMt );
			
			tDStreamChnnl.SetMediaType( MODE_SECVIDEO );
			tDStreamChnnl.SetChannelType( tCapSupport.GetDStreamMediaType() );
			
			//zbq[03/20/2008]FIXME: 没有双流源，带宽该填多少？
			
			//zbq[01/31/2008] FIXME: 这个打开双流通道带宽是有问题的
			//tDStreamChnnl.SetFlowControl(  );
			
			// guzh [2008/03/07] 兼顾考虑E1 降速和Polycom等其他厂商对于通道码率的严格校验
			// 另一部分控制在MtAdp中
			u16 wChanBitrate = m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(), FALSE );
			if ( !( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) && m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId())) )
			{
				if ( wChanBitrate == 0 )
				{
					wChanBitrate = GetDoubleStreamVideoBitrate(m_ptMtTable->GetDialBitrate( tMt.GetMtId() ), FALSE);
				}
				else
				{
					const u16 wDSVBit = GetDoubleStreamVideoBitrate(m_ptMtTable->GetDialBitrate(tMt.GetMtId()), FALSE);
					wChanBitrate = min(wChanBitrate, wDSVBit);
				}
			}
			tDStreamChnnl.SetFlowControl(wChanBitrate);
			
			tDStreamChnnl.m_tSndMediaCtrlChannel.SetIpAddr( dwRcvIp );
			tDStreamChnnl.m_tSndMediaCtrlChannel.SetPort( wRcvPort + 5 );        
			tDStreamChnnl.SetMediaEncrypt(m_tConf.GetMediaKey());
			tDStreamChnnl.SetActivePayload( GetActivePayload( m_tConf,tDStreamChnnl.GetChannelType() ) );    
			
			// [pengjie 2010/6/2] 打开对端双流通道时加上帧率
			u8 byFps = 0;
			u8 byChannelType = tDStreamChnnl.GetChannelType();
			
			if (byChannelType == m_tConf.GetDStreamMediaType())
            {
                if (MEDIA_TYPE_H264 == byChannelType)
                {
                    byFps = m_tConf.GetDStreamUsrDefFPS();
                }
                else
                {
                    byFps = m_tConf.GetDStreamFrameRate();
                }
            }
            else if (byChannelType == m_tConf.GetCapSupportEx().GetSecDSType())
            {
                byFps = m_tConf.GetCapSupportEx().GetSecDSFrmRate();
            }
            else
            {
                ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[McuMtOpenDoubleStreamChnnl] unexpected ds MediaType.%d, Set Fps = 0 !\n", byChannelType);
            }
			
			tDStreamChnnl.SetChanVidFPS( byFps );
			//设置HP/BP属性[12/9/2011 chendaiwei]
			tDStreamChnnl.SetProfieAttrb(m_tConf.GetProfileAttrb());
			// End

			cMsg.SetMsgBody( ( u8 * )&tDStreamChnnl, sizeof( tDStreamChnnl ) );
			
			u8 byVideoFormat = m_tConf.GetVideoFormat(tDStreamChnnl.GetChannelType(), LOGCHL_SECVIDEO);
			cMsg.CatMsgBody( (u8*)&byVideoFormat, sizeof(u8) );
			
			SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cMsg );
			
			//FIXME: TEST
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[McuMtOpenDoubleStreamChnnl] Mt.%d 's CurReqBR.%d, GetDSVBR.%d(polycom)\n",
				tMt.GetMtId(),
				m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE),
				GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())));
			
			// zbq [02/01/2008] 降速终端不作此处理，后续双流通道打开完成后处理之，同说明zbq[01/30/2008]
			if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
				m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()))
			{
				//zbq[01/30/2008] 开双流自动升降速终端单独调整。在通道打开成功后独立计算处理。
				//m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), GetDoubleStreamVideoBitrate(m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE)));            
			}
			else
			{
				// xsl [4/28/2006] 打开双流通道时根据双流码率比例分配视频带宽
				if (m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE) >= GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())))
				{
					//zbq[01/30/2008] 双流码率比例的参考值应该是当前的接收带宽（适应自动升降速）
					//m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), GetDoubleStreamVideoBitrate(
					//    /*m_ptMtTable->GetRcvBandWidth(tMt.GetMtId()))*/m_ptMtTable->GetMtReqBitrate(tMt.GetMtId())), TRUE);
					m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())), LOGCHL_VIDEO);
				}            
			}
			
			//打印消息
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Open Second Video LogicChannel Request send to Mt%d, H239.%d, BR.%d, type: %s, HP:%d\n", 
				tMt.GetMtId(), tDStreamChnnl.IsSupportH239(),
				tDStreamChnnl.GetMediaType(), GetMediaStr( tDStreamChnnl.GetChannelType() ),tDStreamChnnl.GetProfileAttrb() );
		}
		else
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[McuMtOpenDoubleStreamChnnl] IsH239.%d, CapDSType.%d. ConfDSType.%d\n",
				tCapSupport.IsDStreamSupportH239(),
				tCapSupport.GetDStreamMediaType(),
				m_tConf.GetCapSupport().GetDStreamCapSet().GetMediaType());
		}
		return;
	}

    TLogicalChannel tLogicChnAdj = tLogicChnnl; //  pengjie[8/11/2009]
    //TMultiCapSupport tMultiCapSupport;
    //m_ptMtTable->GetMtMultiCapSupport(tMt.GetMtId(), &tMultiCapSupport);
    //TDStreamCap tSecComDSCap =  tMultiCapSupport.GetSecComDStreamCapSet();
    //u8 bySecDstRes = tSecComDSCap.GetResolution();

	//2.如果存在第二共同双流能力，通过源判断接收端动态或静态双流接收通道[12/9/2011 chendaiwei]
	/*u8 bySrcRes = tLogicChnnl.GetVideoFormat();
	u8 byDstRes = tCapSupport.GetDStreamCapSet().GetResolution();
	//zbq[09/02/2008] 双流通道分辨率匹配校验
	if (!IsDSResMatched(bySrcRes, byDstRes))
	{
        if( IsDSResMatched(bySrcRes, bySecDstRes))
        {
            TDStreamCap tDSCap = tCapSupport.GetDStreamCapSet();
            tCapSupport.SetDStreamCapSet(tSecComDSCap);

            //将能力集保存到终端表中
            tMultiCapSupport.SetDStreamCapSet(tSecComDSCap);
            tMultiCapSupport.SetSecComDStreamCapSet(tDSCap);
            m_ptMtTable->SetMtMultiCapSupport(tMt.GetMtId(), &tMultiCapSupport);
			m_ptMtTable->GetMtCapSupport(tMt.GetMtId(), &tCapSupport);
        }

        if (MT_MANU_TAIDE == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
            VIDEO_FORMAT_CIF == byDstRes)
        {
            ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[McuMtOpenDoubleStreamChnnl] Mt.%d's DS Format.%d adjust to Format.%d due to Tand!\n",
                tMt.GetMtId(), bySrcRes, byDstRes);
			tLogicChnAdj.SetVideoFormat(byDstRes);
        }
        else
        {
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[McuMtOpenDoubleStreamChnnl] DsSrc's Res.%d dismatched with DstMt%d's Res.%d, but go on!\n",
				bySrcRes, tMt.GetMtId(), byDstRes);
        }
	}*/

	//3.校验发送端和接收端是否同主视频，或者是否都有H239标签。设置终端接收双流通道[12/9/2011 chendaiwei]
	//双流是否是同主视频
    BOOL32 bIsEvMain = FALSE;
    TCapSupport tConfCapSupport = m_tConf.GetCapSupport();
    if ( tConfCapSupport.GetDStreamType() == VIDEO_DSTREAM_MAIN  || tConfCapSupport.GetDStreamType() == VIDEO_DSTREAM_MAIN_H239)
    {
        bIsEvMain = TRUE;
    }
    
	//tLogicChnAdj和会议是否支持H239是一致的。开启到终端双流通道共两种情况：
	//1.会议H239标签和终端一致
	//2.会议不带H239标签,终端有H239标签[6/28/2012 chendaiwei]
    if( (bIsEvMain || 
		tLogicChnAdj.IsSupportH239() == tCapSupport.IsDStreamSupportH239() ||
		(!m_tConf.GetCapSupport().IsDStreamSupportH239() && tCapSupport.IsDStreamSupportH239()) ))
    {
        if( !m_ptMtTable->GetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort ) )
        {
            dwRcvIp = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( tMt.GetMtId() ) );
            if( dwRcvIp == 0 )
            {
                ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "Mt%d 0x%x(Dri:%d) mp not connected!\n",
                    tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() );		    
            }
			g_cMcuVcApp.FindMatchedMpIpForMt(m_ptMtTable->GetIPAddr(tMt.GetMtId()),dwRcvIp);
            wRcvPort = g_cMcuVcApp.AssignMtPort( tMt.GetConfIdx(), tMt.GetMtId() );
            m_ptMtTable->SetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort );
        }
        
        if( tLogicChnAdj.IsSupportH239())
        {
            tDStreamChnnl.SetSupportH239( tCapSupport.IsDStreamSupportH239() );
            NotifyH239TokenOwnerInfo( &tMt );
        }

        tDStreamChnnl.SetMediaType( MODE_SECVIDEO );
        tDStreamChnnl.SetChannelType( tCapSupport.GetDStreamMediaType() );

		if (MEDIA_TYPE_H264 == tCapSupport.GetDStreamMediaType())
		{
			tDStreamChnnl.SetChanVidFPS(tCapSupport.GetDStreamUsrDefFPS());
		}
		else
		{
			tDStreamChnnl.SetChanVidFPS(tCapSupport.GetDStreamFrmRate());
		}

		tDStreamChnnl.SetVideoFormat(tCapSupport.GetDStreamResolution());

        //zbq[01/31/2008] FIXME: 这个打开双流通道带宽是有问题的
        //tDStreamChnnl.SetFlowControl(  );

        // guzh [2008/03/07] 兼顾考虑E1 降速和Polycom等其他厂商对于通道码率的严格校验
        // 另一部分控制在MtAdp中
        u16 wChanBitrate = m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(), FALSE );

		if( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) 
			&& (m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_MMCU || 
			m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_SMCU))
		{
			wChanBitrate = GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth( tMt.GetMtId() ),FALSE);
		}
        else if ( !( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) && m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId())) )
        {
			//zbq[09/10/2008] 非降速的E1终端的双流带宽获取要取实际接收值
			if (m_ptMtTable->GetMtTransE1(tMt.GetMtId()))
			{
				if ( wChanBitrate == 0 )
				{
					wChanBitrate = GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth( tMt.GetMtId() ), FALSE);
				}
				else
				{
					const u16 wSecDVidBit = GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId()), FALSE);
					wChanBitrate = min(wChanBitrate, wSecDVidBit);
				}
			}
			else
			{
				if ( wChanBitrate == 0 )
				{
					wChanBitrate = GetDoubleStreamVideoBitrate(m_ptMtTable->GetDialBitrate( tMt.GetMtId() ), FALSE);
				}
				else
				{
					const u16 wDSVidBit = GetDoubleStreamVideoBitrate(m_ptMtTable->GetDialBitrate(tMt.GetMtId()), FALSE);
					wChanBitrate = min(wChanBitrate, wDSVidBit);
				}
			}

        }
		else
		{
			//zbq[07/17/2008] E1降速终端通道打开带宽处理
			if ( wChanBitrate == 0 )
			{
				wChanBitrate = GetDoubleStreamVideoBitrate(m_ptMtTable->GetLowedRcvBandWidth(tMt.GetMtId()), FALSE);
			}
			else
			{
				const u16 wDSVidBit = GetDoubleStreamVideoBitrate(m_ptMtTable->GetLowedRcvBandWidth(tMt.GetMtId()), FALSE);
				wChanBitrate = min(wChanBitrate, wDSVidBit);
			}
		}
        tDStreamChnnl.SetFlowControl(wChanBitrate);

        tDStreamChnnl.m_tSndMediaCtrlChannel.SetIpAddr( dwRcvIp );
        tDStreamChnnl.m_tSndMediaCtrlChannel.SetPort( wRcvPort + 5 );        
        tDStreamChnnl.SetMediaEncrypt(m_tConf.GetMediaKey());
        tDStreamChnnl.SetActivePayload( GetActivePayload(m_tConf, tDStreamChnnl.GetChannelType() ) );        
        
		//zbq[09/02/2008] Format按终端能力打开，会议只作保护辅助
        u8 byVideoFormat = tDStreamChnnl.GetVideoFormat();
		if (byVideoFormat == 0)
		{
			byVideoFormat = m_tConf.GetVideoFormat(tDStreamChnnl.GetChannelType(), LOGCHL_SECVIDEO);
		}
		tDStreamChnnl.SetVideoFormat(byVideoFormat);
		//设置HP/BP属性[12/9/2011 chendaiwei]
		tDStreamChnnl.SetProfieAttrb(tCapSupport.GetDStreamCapSet().GetH264ProfileAttrb());
		cMsg.SetMsgBody( ( u8 * )&tDStreamChnnl, sizeof( tDStreamChnnl ) );
        cMsg.CatMsgBody( (u8*)&byVideoFormat, sizeof(u8) );
        
        SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cMsg );

        //FIXME: TEST
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[McuMtOpenDoubleStreamChnnl] Mt.%d 's CurReqBR.%d, GetDSVBR.%d, Format.%d, Fps:%d\n",
                 tMt.GetMtId(),
				 m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE),
				 GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())),
				 byVideoFormat, tDStreamChnnl.GetChanVidFPS());

        // zbq [02/01/2008] 降速终端不作此处理，后续双流通道打开完成后处理之，同说明zbq[01/30/2008]
        if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
             m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()))
        {
            //zbq[01/30/2008] 开双流自动升降速终端单独调整。在通道打开成功后独立计算处理。
            //m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), GetDoubleStreamVideoBitrate(m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE)));            
        }
        else
        {
            // xsl [4/28/2006] 打开双流通道时根据双流码率比例分配视频带宽
            if (m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE) >= GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())))
            {
                //zbq[01/30/2008] 双流码率比例的参考值应该是当前的接收带宽（适应自动升降速）
                //m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), GetDoubleStreamVideoBitrate(
                //    /*m_ptMtTable->GetRcvBandWidth(tMt.GetMtId()))*/m_ptMtTable->GetMtReqBitrate(tMt.GetMtId())), TRUE);
                m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())), LOGCHL_VIDEO);
            }            
        }
                
        //打印消息
        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "Open Second Video LogicChannel Request send to Mt%d, DStream.%d, type: %s, HP:%d\n", 
                tMt.GetMtId(), tDStreamChnnl.IsSupportH239(), GetMediaStr( tDStreamChnnl.GetChannelType() ),tDStreamChnnl.GetProfileAttrb() );
    }
    else
    {
        //打印消息
        ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL,  "Mt%d <%d-H239.%d> not support any conf second video media type<%d-H239.%d>, \
                 no open video logicChannel request send to it!\n", 
                 tMt.GetMtId(),
                 tCapSupport.GetDStreamMediaType(), tCapSupport.IsDStreamSupportH239(),
                 tLogicChnAdj.GetChannelType(), tLogicChnAdj.IsSupportH239());
    }

    return;
}

/*=============================================================================
    函 数 名： StartSwitchDStreamToFirstLChannel
    功    能： 开始 将双流数据交换置指定终端的第一路视频通道中
    算法实现： 
    全局变量： 
    参    数： TMt &tDstMt
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/6/4    3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::StartSwitchDStreamToFirstLChannel( TMt &tDstMt )
{
	//双流是以 与主视频格式一致 的方式加以支持的 则允许将双流数据交换置第一路视频通道中
	if( m_tConf.GetCapSupport().IsDStreamSupportH239() ||
		m_tDoubleStreamSrc.IsNull() )
	{
		return;
	}

	TLogicalChannel tLogicChnnl;
	if( MT_MANU_SONY == m_ptMtTable->GetManuId(tDstMt.GetMtId()) &&
        !m_ptMtTable->GetMtLogicChnnl( tDstMt.GetMtId(), LOGCHL_SECVIDEO, &tLogicChnnl, TRUE ) && 
		m_ptMtTable->GetMtLogicChnnl( tDstMt.GetMtId(), LOGCHL_VIDEO, &tLogicChnnl, TRUE ) && 
		tLogicChnnl.GetChannelType() == m_tConf.GetCapSupport().GetDStreamMediaType() )
	{	
		//zjl 20110510 StopSwitchToSubMt 接口重载替换
		//变更交换
		//StopSwitchToSubMt( tDstMt.GetMtId(), MODE_VIDEO );
		StopSwitchToSubMt(1, &tDstMt, MODE_VIDEO);

		m_ptMtTable->SetMtVideo2Recv( tDstMt.GetMtId(), TRUE );

        //zbq [08/23/2007] 录像机放双流建交换要依据放象通道建立
        u8 bySrcChn = m_tDoubleStreamSrc == m_tPlayEqp ? m_byPlayChnnl : 0;
		//zjl 20110510 StartSwitchToAll 替换 StartSwitchToSubMt
		//g_cMpManager.StartSwitchToSubMt( m_tDoubleStreamSrc, bySrcChn, tDstMt, MODE_SECVIDEO, SWITCH_MODE_BROADCAST, FALSE );
		TSwitchGrp tSwitchGrp;
		tSwitchGrp.SetSrcChnl(bySrcChn);
		tSwitchGrp.SetDstMtNum(1);
		tSwitchGrp.SetDstMt(&tDstMt);
		g_cMpManager.StartSwitchToAll(m_tDoubleStreamSrc, 1, &tSwitchGrp, MODE_SECVIDEO);


		//设置终端状态
		m_ptMtTable->SetMtSrc( tDstMt.GetMtId(), ( const TMt * )&m_tDoubleStreamSrc, MODE_VIDEO );

		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "StartSwitchDStreamToFirstLChannel Mt%d\n", tDstMt.GetMtId() );
	}

	return;
}

/*=============================================================================
    函 数 名： StopSwitchDStreamToFirstLChannel
    功    能： 停止 将双流数据交换置指定终端的第一路视频通道中,恢复为主视频源
    算法实现： 
    全局变量： 
    参    数： TMt &tDstMt
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/6/4    3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::StopSwitchDStreamToFirstLChannel( TMt &tDstMt )
{
	if( m_tConf.GetCapSupport().IsDStreamSupportH239() )
	{
		return;
	}
	
	TLogicalChannel tLogicChnnl;
	if( MT_MANU_SONY == m_ptMtTable->GetManuId(tDstMt.GetMtId()) &&
        !m_ptMtTable->GetMtLogicChnnl( tDstMt.GetMtId(), LOGCHL_SECVIDEO, &tLogicChnnl, TRUE ) && 
		m_ptMtTable->GetMtLogicChnnl( tDstMt.GetMtId(), LOGCHL_VIDEO, &tLogicChnnl, TRUE ) && 
		tLogicChnnl.GetChannelType() == m_tConf.GetCapSupport().GetDStreamMediaType() )
	{
		//恢复为主视频源
		RestoreRcvMediaBrdSrc( tDstMt.GetMtId(), MODE_VIDEO, TRUE );
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "StopSwitchDStreamToFirstLChannel Mt%d\n", tDstMt.GetMtId() );
	}

	return;
}

/*=============================================================================
    函 数 名： JudgeIfSwitchFirstLChannel
    功    能： 判别目标终端是否为 已借用第一路视频通道传输双流的终端
    算法实现： 
    全局变量： 
    参    数： TMt &tSrc
               u8 bySrcChnnl
               TMt &tDstMt
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/6/4    3.6			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcInst::JudgeIfSwitchFirstLChannel( TMt &tSrc, u8 bySrcChnnl, TMt &tDstMt )
{
	BOOL32 bRet = FALSE;

	if( m_tConf.GetCapSupport().IsDStreamSupportH239() ||  
		m_tDoubleStreamSrc.IsNull() )
	{
		return bRet;
	}
	
	//目前暂时区分SONY终端，之后最好是通过能力集支持情况判别
	TLogicalChannel tLogicChnnl;
	u8 byManuID = m_ptMtTable->GetManuId(tDstMt.GetMtId());
	if( MT_MANU_SONY == byManuID &&
		!m_ptMtTable->GetMtLogicChnnl( tDstMt.GetMtId(), LOGCHL_SECVIDEO, &tLogicChnnl, TRUE ) && 
		m_ptMtTable->GetMtLogicChnnl( tDstMt.GetMtId(), LOGCHL_VIDEO, &tLogicChnnl, TRUE ) && 
		tLogicChnnl.GetChannelType() == m_tConf.GetCapSupport().GetDStreamMediaType() )
	{
		bRet = TRUE;
	}

	ConfPrint(LOG_LVL_DETAIL,MID_MCU_MPMGR,"[JudgeIfSwitchFirstLChannel] tSrc<mcuId.%d MtId.%d>bySrcChnnl = %d\n",tSrc.GetMcuId(),tSrc.GetMtId(),bySrcChnnl);

	return bRet;
}

/*=============================================================================
    函 数 名： ClearH239TokenOwnerInfo
    功    能： 清除当前会议的 H239双流令牌拥有终端信息
    算法实现： 
    全局变量： 
    参    数： TMt *ptMt非空时进行通知
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/6/9    3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::ClearH239TokenOwnerInfo( TMt *ptMt )
{
	if( m_tH239TokenOwnerInfo.m_tH239TokenMt.IsNull() )
	{
		return;
	}

	if(  NULL == ptMt || 
		(NULL != ptMt && m_tH239TokenOwnerInfo.m_tH239TokenMt == *ptMt) )
	{
		//REQUEST RELEASING TOKEN OF LAST MT OWNER
		//zjl [02/25/2010] 如果当前令牌环拥有者是上级mcu，释放令牌环信息在mtadp会转成令牌环请求到上级mcu
		//r6mcu会无条件过滤该请求，r2则不过滤，直接认为有新的终端或mcu请求令牌环，导致真正的双流源token被剥夺
		// [pengjie 2010/2/26] 如果是双流放像，及令牌环拥有者是外设，则不用通知，只Clear，就好
		if( NULL != ptMt && /*MT_MANU_KDCMCU != m_ptMtTable->GetManuId(m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId()) &&*/
			m_tH239TokenOwnerInfo.m_tH239TokenMt.GetType() != TYPE_MCUPERI)
		{
			CServMsg cServMsg;
			TH239TokenInfo tH239Info;
			tH239Info.SetChannelId( m_tH239TokenOwnerInfo.GetChannelId() );
			tH239Info.SetSymmetryBreaking( 0 );
			cServMsg.SetEventId(MCU_MT_RELEASEH239TOKEN_CMD);
			cServMsg.SetMsgBody((u8 *)&m_tH239TokenOwnerInfo.m_tH239TokenMt, sizeof(TMt));
			cServMsg.CatMsgBody((u8 *)&tH239Info, sizeof(TH239TokenInfo));
			SendMsgToMt( m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId(), MCU_MT_RELEASEH239TOKEN_CMD, cServMsg);
		}
		u8 byTokenMtId = m_tH239TokenOwnerInfo.GetTokenMt().GetMtId();
		u8 byChnId = m_tH239TokenOwnerInfo.GetChannelId();
		m_tH239TokenOwnerInfo.Clear();
		if( m_byNewTokenReqMtid != 0 &&
			MT_MANU_KDCMCU == m_ptMtTable->GetManuId(byTokenMtId))
		{
			m_tH239TokenOwnerInfo.SetChannelId(byChnId);
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[ClearH239TokenOwnerInfo]Old DsSrc(%d) is KEDAMCU AND HAS NEWTOKENMT(%d) ,So Reccord The Chniid!!\n",m_tDoubleStreamSrc.GetMtId(),byChnId);
		}
	}
	
	return;
}

/*=============================================================================
    函 数 名： UpdateH239TokenOwnerInfo
    功    能： 更新当前会议的 H239双流令牌拥有终端信息
    算法实现： 
    全局变量： 
    参   -------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/6/9    3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::UpdateH239TokenOwnerInfo(TMt &tMt)
{
	if( !m_tH239TokenOwnerInfo.m_tH239TokenMt.IsNull() )
	{
		//TD:------FIRST REQUEST TOKEN,NEXT OPENCHANNEL
		if( m_tH239TokenOwnerInfo.m_tH239TokenMt == tMt )
		{
		}
		//REQUEST RELEASING TOKEN OF LAST MT OWNER
		else
		{
			if (MT_MANU_KDCMCU != m_ptMtTable->GetManuId(tMt.GetMtId()))
			{
				CServMsg cServMsg;
				TH239TokenInfo tH239Info;
				tH239Info.SetChannelId( m_tH239TokenOwnerInfo.GetChannelId() );
				tH239Info.SetSymmetryBreaking( 0 );
				cServMsg.SetEventId(MCU_MT_RELEASEH239TOKEN_CMD);
				cServMsg.SetMsgBody((u8 *)&m_tH239TokenOwnerInfo.m_tH239TokenMt, sizeof(TMt));
				cServMsg.CatMsgBody((u8 *)&tH239Info, sizeof(TH239TokenInfo));
				SendMsgToMt( m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId(), MCU_MT_RELEASEH239TOKEN_CMD, cServMsg);

			}

			m_tH239TokenOwnerInfo.Clear();
		}
	}
	
	return;
}

/*=============================================================================
    函 数 名： NotifyH239TokenOwnerInfo
    功    能： 通知其他直连终端 当前会议的 H239双流令牌拥有终端信息
    算法实现： 
    全局变量： 
    参    数： TMt *ptMt
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/6/9    3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::NotifyH239TokenOwnerInfo(TMt *ptMt)
{
	if( m_tH239TokenOwnerInfo.m_tH239TokenMt.IsNull() )
	{
		return;
	}

	TMt tLocalMt;
	CServMsg cServMsg;
	TH239TokenInfo tH239Info;
	tH239Info.Clear();

	if( NULL == ptMt )
	{
		for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
		{
			// [pengjie 2010/2/27] Modify 如果令牌环的拥有者是外设，则发给所有终端，而不过滤掉与它id相同的那个
			//  因为GetMtId()就会将外设的ID当成某个终端的ID，进而导致某个终端可能收不到令牌通知			
			if( m_tH239TokenOwnerInfo.m_tH239TokenMt.GetType() != TYPE_MCUPERI )
			{
				if( byLoop == m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId() )
				{
					continue;
				}
			}
			// End

			if( !m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
			{
				continue;
			}
			
			tLocalMt = m_ptMtTable->GetMt( byLoop );

			tH239Info.SetChannelId( m_tH239TokenOwnerInfo.GetChannelId() );
			cServMsg.SetEventId(MCU_MT_OWNH239TOKEN_NOTIF);
			cServMsg.SetMsgBody((u8 *)&tLocalMt, sizeof(TMt));
			cServMsg.CatMsgBody((u8 *)&tH239Info, sizeof(TH239TokenInfo));
			SendMsgToMt( tLocalMt.GetMtId(), MCU_MT_OWNH239TOKEN_NOTIF, cServMsg);
		}
	}
	else
	{
		// [pengjie 2010/2/26] Modify 如果令牌环拥有者是外设，则直接发给目的终端，不用过滤
		if( m_tH239TokenOwnerInfo.m_tH239TokenMt.GetType() == TYPE_MCUPERI )
		{
			tH239Info.SetChannelId( m_tH239TokenOwnerInfo.GetChannelId() );
			cServMsg.SetEventId(MCU_MT_OWNH239TOKEN_NOTIF);
			cServMsg.SetMsgBody((u8 *)ptMt, sizeof(TMt));
			cServMsg.CatMsgBody((u8 *)&tH239Info, sizeof(TH239TokenInfo));
			SendMsgToMt( ptMt->GetMtId(), MCU_MT_OWNH239TOKEN_NOTIF, cServMsg);
		}
		// 否则要判断令牌环拥有者是否是要通知的那个终端，并将其排除
		else
		{   
			if( ptMt->GetMtId() != m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId() )
			{
				tH239Info.SetChannelId( m_tH239TokenOwnerInfo.GetChannelId() );
				cServMsg.SetEventId(MCU_MT_OWNH239TOKEN_NOTIF);
				cServMsg.SetMsgBody((u8 *)ptMt, sizeof(TMt));
				cServMsg.CatMsgBody((u8 *)&tH239Info, sizeof(TH239TokenInfo));
				SendMsgToMt( ptMt->GetMtId(), MCU_MT_OWNH239TOKEN_NOTIF, cServMsg);
			}
		}
	}

	return;
}
/*=============================================================================
    函 数 名： ProcMtSelectdByOtherMt
    功    能： 处理指定终端被其他终端选看（调整非Keda终端音频时）
    算法实现： 
    全局变量： 
    参    数： u8 byMtId     被选看的终端ID
	           BOOL32 bStartSwitch 是否交换过去
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/02/23  4.0			刘辉云                  创建
=============================================================================*/
void CMcuVcInst::ProcMtSelectdByOtherMt( u8 byMtId, BOOL32 bStartSwitch )
{
	TMt tSrcMt = m_ptMtTable->GetMt( byMtId );
	if( tSrcMt.IsNull() )
	{
		return;
	}

	TMt atDstMt[MAXNUM_CONF_MT];
	u8 byDstMtNum = 0;

	TMtStatus tMtStatus;
	TMt  tDstMt;
	u16 wSpyPort = SPY_CHANNL_NULL;
	for ( u8 byMtIdxLp = 1; byMtIdxLp <= MAXNUM_CONF_MT; byMtIdxLp ++ )
	{
		if( byMtIdxLp != byMtId ) // 排除本身
		{
			tDstMt = m_ptMtTable->GetMt( byMtIdxLp );
			if( !tDstMt.IsNull() ) // 该终端是否有效
			{
				m_ptMtTable->GetMtStatus(byMtIdxLp, &tMtStatus);
				// 对于下级的MCU的终端与上级终端互相选看，实际为下级MCU与上级终端互相选看
				TMt tSelMt = tMtStatus.GetSelectMt(MODE_AUDIO);

				// [1/4/2011 xliang] filter null tSelMt
				if ( tSelMt.IsNull() )
				{
					continue;
				}

				if( (!tSelMt.IsLocal() && GetFstMcuIdFromMcuIdx( tSelMt.GetMcuId() ) == byMtId) ||
					tMtStatus.GetSelectMt(MODE_AUDIO) == tSrcMt ) // 交换音频给它
				{
					if( !tSelMt.IsLocal() && IsLocalAndSMcuSupMultSpy(tSelMt.GetMcuId()) )
					{
						CRecvSpy cRecvSpy;
						if( m_cSMcuSpyMana.GetRecvSpy( tSelMt, cRecvSpy ) )
						{
							wSpyPort = cRecvSpy.m_tSpyAddr.GetPort();
							ConfPrint(LOG_LVL_DETAIL, MID_MCU_MT, "[ProcMtSelectdByOtherMt] GetspyPort is：%d\n", wSpyPort);
						}
					}
					if( bStartSwitch )
					{
						if ( IsNeedSelAdpt(tSelMt, tDstMt, MODE_AUDIO)  )
						{
							StartSelAdapt(tSelMt, tDstMt, MODE_AUDIO);
						}
						else
						{
							//zjl 20110510 StartSwitchToAll 替换 StartSwitchToSubMt，这里有可能多个spyport
							//StartSwitchToSubMt( tSrcMt, 0,  byMtIdxLp, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE, TRUE, FALSE, wSpyPort );
							TSwitchGrp tSwitchGrp;
							tSwitchGrp.SetSrcChnl(0);
							tSwitchGrp.SetDstMtNum(1);
							tSwitchGrp.SetDstMt(&tDstMt);
							StartSwitchToAll(tSrcMt, 1, &tSwitchGrp, MODE_AUDIO, SWITCH_MODE_SELECT, TRUE,  TRUE, wSpyPort);
						}
					}
					else
					{
						//zjl 20110510 StopSwitchToSubMt 接口重载替换
						//StopSwitchToSubMt( tDstMt.GetMtId(), MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE, TRUE, TRUE, wSpyPort);
						atDstMt[byDstMtNum] = tDstMt;
						byDstMtNum++;
					}
				}
			}
		}
	}
 
	if (byDstMtNum > 0)
	{
		StopSwitchToSubMt(byDstMtNum, atDstMt, MODE_AUDIO, TRUE, FALSE, TRUE);
	}


	// 对于VCS MCS当前选看终端为该哑音对象时需要进行相应的建拆交换
	//zjj20120717 在外面处理,这里注掉
	/*TMt tMCAudSrc;
	tMCAudSrc.SetNull();
	for (u8 byMCIdx = 1; byMCIdx <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byMCIdx++)
	{
		for (u8 byChnl = 0; byChnl < MAXNUM_MC_CHANNL; byChnl++)
		{
			if (g_cMcuVcApp.GetMcSrc(byMCIdx, &tMCAudSrc, byChnl, MODE_AUDIO) &&
				((!tMCAudSrc.IsLocal() && GetFstMcuIdFromMcuIdx(tMCAudSrc.GetMcuId()) == tSrcMt.GetMtId()) ||
				  tMCAudSrc == tSrcMt))
			{
				if (bStartSwitch)
				{
				    g_cMpManager.StartSwitchToMc(tMCAudSrc, 0, byMCIdx, byChnl, MODE_AUDIO, wSpyPort);
				}
				else
				{
					g_cMpManager.StopSwitchToMc(m_byConfIdx, byMCIdx, byChnl, MODE_AUDIO);
				}
			}
		}		
	}*/


	if( !m_tCascadeMMCU.IsNull() )
	{		
		m_ptMtTable->GetMtStatus(m_tCascadeMMCU.GetMtId(), &tMtStatus);		
    
		tDstMt = GetLocalMtFromOtherMcuMt( tMtStatus.GetAudioMt() );
		if( !tDstMt.IsNull() && byMtId == tDstMt.GetMtId() )
		{
			if( bStartSwitch )
			{
				tDstMt.SetNull();
				m_ptMtTable->SetMtSrc( m_tCascadeMMCU.GetMtId(),&tDstMt,MODE_AUDIO );
				StartSwitchToMcu(tMtStatus.GetVideoMt(), 0, m_tCascadeMMCU.GetMtId(), MODE_AUDIO, SWITCH_MODE_SELECT);
			}
			else
			{
				StopSpyMtCascaseSwitch( MODE_AUDIO );
				tDstMt = tMtStatus.GetAudioMt();
				m_ptMtTable->SetMtSrc( m_tCascadeMMCU.GetMtId(),&tDstMt,MODE_AUDIO );
			}
		}		
	}
	

	return;
}

/*=============================================================================
    函 数 名： FindMtAndAdjustMtRecSrcStream
    功    能： 调整非KDC（泰德）音频码流交换码流（值处理广播音频源）
    算法实现： 
    全局变量： 
    参    数： u8 tMcu     要查找的终端所在的mcu
               u8 byMode  要调整的码流的音视频模式
               BOOL32 bIsStart   是否拆建交换
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2012/04/11  4.7			周晶晶                  创建
=============================================================================*/
void CMcuVcInst::FindMtAndAdjustMtRecSrcStream( const TMt &tMcu,const u8 byMode,const BOOL32 bIsStart )
{
	/*if( !IsMcu(tMcu) )
	{
		return;
	}*/

	u8 byRecId = RECORDERID_MIN;
	TPeriEqpStatus tRecStatus;
	TRecChnnlStatus tStatus;
	u8 byRecChnNum = 0;
	TEqp tEqp;
	TMt tLocalMt;
    while( byRecId >= RECORDERID_MIN && byRecId <= RECORDERID_MAX )
	{
		if (g_cMcuVcApp.IsPeriEqpValid(byRecId))
		{
			g_cMcuVcApp.GetPeriEqpStatus(byRecId, &tRecStatus);
            if (tRecStatus.m_byOnline == 1)
            {                
                byRecChnNum = tRecStatus.m_tStatus.tRecorder.GetRecChnnlNum();
				if (0 == byRecChnNum)
				{
					++byRecId;
					ConfPrint(LOG_LVL_ERROR, MID_MCU_EQP, "[RemoveMtFormPeriInfo] byRecId just is zero!\n");
					continue;
				}

				tEqp = (TEqp)tRecStatus;
				for(u8 byLoop = 0; byLoop < byRecChnNum; byLoop++)
				{
					if(FALSE == tRecStatus.m_tStatus.tRecorder.GetChnnlStatus(byLoop, TRecChnnlStatus::TYPE_RECORD, &tStatus))
					{
						continue;
					}

					if (tStatus.m_tSrc.IsNull())
					{
						continue;
					}

					if (tStatus.m_tSrc.GetConfIdx() != m_byConfIdx)
					{
						continue;
					}
					tLocalMt = GetLocalMtFromOtherMcuMt(tStatus.m_tSrc);
					if ( tMcu.GetMtId() == tLocalMt.GetMtId() )
					{						
						AdjustMtRecSrcStream(byMode, tEqp, byLoop, TMt(tStatus.m_tSrc), bIsStart);
					}					
				}				
			}			
		}
		++byRecId;
	}
}

/*=============================================================================
    函 数 名： AdjustTaideAudioSwitch
    功    能： 调整非KDC（泰德）音频码流交换码流（值处理广播音频源）
    算法实现： 
    全局变量： 
    参    数： u8 byDstMtId     待调整的终端ID
               BOOL32 bDecodeAud  TRUE-解码静音处理 FALSE-编码哑音处理
               BOOL32 bOPenFlag   TRUE-打开         FALSE-关闭
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/03/25  3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::AdjustKedaMcuAndTaideAudioSwitch( u8 byDstMtId, BOOL32 bDecodeAud, BOOL32 bOpenFlag )
{
    // xsl [9/20/2006] 需要考虑广播源为混音器的情况
	//zhouyiliang 20101116 不能再用local化的srcmt，否则重新建音频交换的时候不正确。
	//TMt tAudSrcMt = GetLocalAudBrdSrc();
	TMt tAudSrcMt = GetAudBrdSrc();
	u16 wSpyStartPort = SPY_CHANNL_NULL;
	CRecvSpy cRcvSpy;

	//终端静音，停止外部向该终端交换音频
	if( TRUE == bDecodeAud && TRUE == bOpenFlag )
	{
        //不取消选看
		//StopSwitchToSubMt( byDstMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE, FALSE );
		TMt tMt = m_ptMtTable->GetMt(byDstMtId);
		StopSwitchToSubMt(1, &tMt, MODE_AUDIO, FALSE, FALSE);
	}
		
	TMtStatus tMtStatus;
	m_ptMtTable->GetMtStatus(byDstMtId, &tMtStatus);

	//取消终端静音，恢复外部向该终端交换音频
	if( TRUE == bDecodeAud && FALSE == bOpenFlag )
	{
		TMt tSelectMt = tMtStatus.GetSelectMt(MODE_AUDIO);

        //没有音频选看
		if(tSelectMt.IsNull() && !tAudSrcMt.IsNull())
		{
            // xsl [9/26/2006] 有混音器先恢复混音器
			//tianzhiyong 2010/03/21 增加EAPU类型混音器
            if (tAudSrcMt.GetType() == TYPE_MCUPERI && tAudSrcMt.GetEqpType() == EQP_TYPE_MIXER )
            {
                TMt tDstMt = m_ptMtTable->GetMt(byDstMtId);
				
				// fxh[Bug00014234级联混音]定制混音非混音组终端取混音器N模式输出端口码流，
				// 而定制混音组中和讨论模式下终端取N-1模式输出端口码流
                if (m_tConf.m_tStatus.IsSpecMixing() && !m_ptMtTable->IsMtInMixing(byDstMtId))
                {
                    SwitchMixMember(&tDstMt, TRUE);
                }
                else
                {
                    SwitchMixMember(&tDstMt, FALSE);
                }                
            }
            else
            {
				//zhouyiliang 20101116 非local化的srcmt要将spyport带入，否则重新建音频交换的时候不正确。				
				if ( !tAudSrcMt.IsLocal() )
				{					
					if ( m_cSMcuSpyMana.GetRecvSpy(tAudSrcMt,cRcvSpy) ) 
					{
						wSpyStartPort = cRcvSpy.m_tSpyAddr.GetPort();
					}
				}
				//zjl 20110510 StartSwitchToAll 替换 StartSwitchToSubMt
                //StartSwitchToSubMt( tAudSrcMt, 0,  byDstMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE ,TRUE,FALSE,wSpyStartPort);
				TMt tMt = m_ptMtTable->GetMt(byDstMtId);
				if ( IsNeedAdapt(tAudSrcMt, tMt, MODE_AUDIO) &&
					m_tConf.m_tStatus.IsAudAdapting()
					)
				{
					StartSwitchToSubMtFromAdp(tMt.GetMtId(), MODE_AUDIO);
				}
				else
				{
					TSwitchGrp tSwitchGrp;
					tSwitchGrp.SetSrcChnl(0);
					tSwitchGrp.SetDstMtNum(1);
					tSwitchGrp.SetDstMt(&tMt);
					StartSwitchToAll(tAudSrcMt, 1, &tSwitchGrp, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE, TRUE, wSpyStartPort);
				}
			}			     
		}
		else
		{	
			//zjl 20110510 StartSwitchToAll 替换 StartSwitchToSubMt
            //恢复音频选看
			//StartSwitchToSubMt( tSelectMt, 0,  byDstMtId, MODE_AUDIO, SWITCH_MODE_SELECT, FALSE );
			TMt tMt = m_ptMtTable->GetMt(byDstMtId);
			TSwitchGrp tSwitchGrp;
			if( !tSelectMt.IsNull() )
			{
				if ( !tSelectMt.IsLocal() )
				{					
					if ( m_cSMcuSpyMana.GetRecvSpy(tSelectMt,cRcvSpy) ) 
					{
						wSpyStartPort = cRcvSpy.m_tSpyAddr.GetPort();
					}
				}				
				if ( IsNeedSelAdpt(tSelectMt, tMt, MODE_AUDIO) )
				{
					StartSelAdapt(tSelectMt, tMt, MODE_AUDIO);
				}
				else
				{
					tSwitchGrp.SetSrcChnl(0);
					tSwitchGrp.SetDstMtNum(1);				
					tSwitchGrp.SetDstMt(&tMt);
					StartSwitchToAll(tSelectMt, 1, &tSwitchGrp,  MODE_AUDIO, SWITCH_MODE_SELECT, FALSE,FALSE,wSpyStartPort );
				}
			}
			if( !m_tCascadeMMCU.IsNull() && byDstMtId == m_tCascadeMMCU.GetMtId() )
			{
				m_ptMtTable->GetMtStatus(m_tCascadeMMCU.GetMtId(), &tMtStatus);    
				tMt = tMtStatus.GetAudioMt();
				if( !tMt.IsNull() )
				{
					if ( !tMt.IsLocal() )
					{					
						if ( m_cSMcuSpyMana.GetRecvSpy(tMt,cRcvSpy) ) 
						{
							wSpyStartPort = cRcvSpy.m_tSpyAddr.GetPort();
						}
					}
					tSelectMt = tMt;
					tMt = m_tCascadeMMCU;
					if ( IsNeedSelAdpt(tSelectMt, tMt, MODE_AUDIO)  )
					{
						StartSelAdapt(tSelectMt, tMt, MODE_AUDIO);
					}
					else
					{
						tSwitchGrp.SetSrcChnl(0);
						tSwitchGrp.SetDstMtNum(1);				
						tSwitchGrp.SetDstMt( &tMt );
						StartSwitchToAll(tSelectMt, 1, &tSwitchGrp,  MODE_AUDIO, SWITCH_MODE_SELECT, FALSE,FALSE,wSpyStartPort );
					}
				}
			}
		}
	}

	//终端哑音，停止该终端向外部交换音频
	if( FALSE == bDecodeAud && TRUE == bOpenFlag )
	{
		//停止到混音器的交换
		if (!m_tMixEqp.IsNull() && (m_tConf.m_tStatus.IsMixing() || m_tConf.m_tStatus.IsVACing()) && m_ptMtTable->IsMtInMixGrp(byDstMtId))
		{
			TMt tmt = m_ptMtTable->GetMt(byDstMtId);
			StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
				(MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tmt)), 
				FALSE, MODE_AUDIO, SWITCH_MODE_BROADCAST);
		}
		if( tAudSrcMt.IsNull() )
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[AdjustKedaMcuAndTaideAudioSwitch] tAudSrcMt is NULL\n");

			//[2011/12/23/zhangli]如果广播音频源为NULL，处理该终端被选看
			ProcMtSelectdByOtherMt(byDstMtId, FALSE);
		}
		else
		{
			if (tAudSrcMt.GetType() == TYPE_MCUPERI)
			{
			}
			else
			{
				//被哑音终端是发言人
				if( ( tAudSrcMt.IsLocal() && tAudSrcMt.GetMtId() == byDstMtId ) 
					|| ( !tAudSrcMt.IsLocal() && GetFstMcuIdFromMcuIdx( tAudSrcMt.GetMcuId() ) == byDstMtId) 
					)
					//if( (tAudSrcMt.GetMtId() == byDstMtId) && (m_tConf.GetSpeaker().IsLocal() && IsSupportMultiSpy() || !IsSupportMultiSpy()))
				{
					TMt atDstMt[MAXNUM_CONF_MT];
					memset(atDstMt, 0, sizeof(atDstMt));
					u8  byDstMtNum = 0;
					TMt tMtSel;
					TMtStatus status;
					
					for( u8 byMtLoop = 1; byMtLoop <= MAXNUM_CONF_MT; byMtLoop++ )
					{
						if( m_tConfAllMtInfo.MtJoinedConf( byMtLoop ) )
						{	
							//如果发言人是下级MCU，对该MCU进行哑音时也要拆掉自己打给自己的音频交换否则下级还能收到声音
							if( tAudSrcMt.IsLocal() && tAudSrcMt.GetMtId() == byMtLoop && 
								m_ptMtTable->GetMtType(byMtLoop) == MT_TYPE_MT)
							{
								continue;
							}
							m_ptMtTable->GetMtStatus( byMtLoop,&status );
							tMtSel = status.GetSelectMt( MODE_AUDIO );
							if( !tMtSel.IsNull() && 
								( !( tMtSel == tAudSrcMt ) && !( GetLocalMtFromOtherMcuMt(tMtSel) == GetLocalMtFromOtherMcuMt(tAudSrcMt) ) )
								)
							{
								continue;
							}
							//zjl 20110510 StopSwitchToSubMt 接口重载替换
							//StopSwitchToSubMt( byMtLoop, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE );
							atDstMt[byDstMtNum] = m_ptMtTable->GetMt(byMtLoop);
							byDstMtNum ++;
						}
					}

					if (byDstMtNum > 0)
					{
						StopSwitchToSubMt(byDstMtNum, atDstMt, MODE_AUDIO, FALSE);
					}
					
				}
				else
				{
					//处理该终端被其它终端选看
					ProcMtSelectdByOtherMt(byDstMtId, FALSE);
				}
			}
		}
        
		TMt tMt = m_ptMtTable->GetMt( byDstMtId );
		
		if (m_tConf.GetConfSource() == VCS_CONF)
		{
			VCSFindMtInTvWallAndChangeSwitch( tMt,TRUE,MODE_AUDIO );
		} 
		else
		{
			MCSFindMtInTvWallAndChangeSwitch( tMt,TRUE,MODE_AUDIO );
		}	

		if( !m_tConf.m_tStatus.IsNoRecording() && tAudSrcMt.GetType() != TYPE_MCUPERI &&
			GetLocalAudBrdSrc().GetMtId() == byDstMtId )
		{
			AdjustConfRecordSrcStream(MODE_AUDIO, FALSE);
		}
		
		FindMtAndAdjustMtRecSrcStream( tMt,MODE_AUDIO,FALSE );		

		AdjustSwitchToAllMcWatchingSrcMt( tMt,FALSE,MODE_AUDIO,TRUE );

		AdjustSwitchToMonitorWatchingSrc( tMt,MODE_AUDIO,TRUE,TRUE );

		CSendSpy cSendSpy;
		if( !m_tCascadeMMCU.IsNull() && m_cLocalSpyMana.GetSpyChannlInfo( tMt,cSendSpy ) )
		{
			if( MODE_BOTH == cSendSpy.GetSpyMode() || MODE_AUDIO == cSendSpy.GetSpyMode() )
			{
				TSimCapSet tSrcCap = m_ptMtTable->GetSrcSCS(tMt.GetMtId());
				if (cSendSpy.GetSimCapset().GetAudioMediaType() != tSrcCap.GetAudioMediaType() &&
					m_ptMtTable->IsLogicChnnlOpen( tMt.GetMtId(), LOGCHL_AUDIO, FALSE ) &&
					!cSendSpy.GetSimCapset().IsNull() )
				{
					StopSpyAdapt(tMt, cSendSpy.GetSimCapset(), MODE_AUDIO);
				}				
				g_cMpManager.StopSwitchToSubMt(m_byConfIdx, 1, &m_tCascadeMMCU, MODE_AUDIO, cSendSpy.m_tSpyAddr.GetPort());	
			}			
		}
	}

	//取消终端哑音，恢复该终端向外部交换音频
	if( FALSE == bDecodeAud && FALSE == bOpenFlag )
	{
		//恢复到混音器的交换
		if (!m_tMixEqp.IsNull() && (m_tConf.m_tStatus.IsMixing() || m_tConf.m_tStatus.IsVACing()) && m_ptMtTable->IsMtInMixGrp(byDstMtId))
		{
			TMt tSrcMt = m_ptMtTable->GetMt(byDstMtId);
			StartSwitchToPeriEqp(tSrcMt, 0, m_tMixEqp.GetEqpId(), 
				(MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tSrcMt)),
				MODE_AUDIO, SWITCH_MODE_SELECT);
        }
		if( tAudSrcMt.IsNull() )
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[AdjustKedaMcuAndTaideAudioSwitch] tAudSrcMt is NULL\n");
		}

        if (tAudSrcMt.GetType() == TYPE_MCUPERI)
        {
        }
        else
        {
            //被哑音终端是发言人
			if( tAudSrcMt.GetType() == TYPE_MT && 
				( ( tAudSrcMt.IsLocal() && tAudSrcMt.GetMtId() == byDstMtId ) ||
				  ( !tAudSrcMt.IsLocal() && GetFstMcuIdFromMcuIdx( tAudSrcMt.GetMcuId() ) == byDstMtId) )
				)
		    {
				u16 wSpyPort = SPY_CHANNL_NULL;
				if( !tAudSrcMt.IsLocal() && IsLocalAndSMcuSupMultSpy(tAudSrcMt.GetMcuId()) )
				{
					CRecvSpy cRecvSpy;
					if( m_cSMcuSpyMana.GetRecvSpy( tAudSrcMt, cRecvSpy ) )
					{
						wSpyPort = cRecvSpy.m_tSpyAddr.GetPort();
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[AdjustKedaMcuAndTaideAudioSwitch] GetspyPort is：%d\n", wSpyPort);
					}
				}

				//zjj20120627 终端取消哑音,唇音交换处理
				if ( tAudSrcMt == GetVidBrdSrc() )
				{
					StartSwitchAud2Perieqp(tAudSrcMt);		
				}

			    StartSwitchToAllSubMtJoinedConf( tAudSrcMt, 0, wSpyPort);
				//目前StartSwitchToAllSubMtJoinedConf不会直接建立音频适配交换，故音频适配广播交换另行建立
				if ( m_tConf.m_tStatus.IsAudAdapting()  )
				{
					u8 byNum = 0;
					CBasChn *apcBasChn[MAXNUM_PERIEQP]={NULL};
					if (GetBasBrdChnGrp(byNum, apcBasChn, MODE_AUDIO))
					{
						TEqp tTempBas;
						for ( u8 byBasLoop =0; byBasLoop<byNum; byBasLoop++ )
						{
							tTempBas = apcBasChn[byBasLoop]->GetBas();
							StartSwitchToSubMtNeedAdp(tTempBas, apcBasChn[byBasLoop]->GetChnId());
							StartBasPrsSupport(tTempBas, apcBasChn[byBasLoop]->GetChnId());
						}
					}
				}
		    }
// 		    else
// 		    {
//                 //处理该终端被其它终端选看
// 			    ProcMtSelectdByOtherMt(byDstMtId, TRUE);
// 		    }
        }
		
		//[2011/12/23/zhangli]处理选看放在这里，不管有没有音频广播源，选看都要处理
		ProcMtSelectdByOtherMt(byDstMtId, TRUE);

		TMt tMt = m_ptMtTable->GetMt( byDstMtId );
		
			
		if (m_tConf.GetConfSource() == VCS_CONF)
		{
			VCSFindMtInTvWallAndChangeSwitch( tMt,FALSE,MODE_AUDIO );
		} 
		else
		{
			MCSFindMtInTvWallAndChangeSwitch( tMt,FALSE,MODE_AUDIO );
		}

		if( !m_tConf.m_tStatus.IsNoRecording() && tAudSrcMt.GetType() != TYPE_MCUPERI && 
						GetLocalAudBrdSrc().GetMtId() == byDstMtId )
		{
			AdjustConfRecordSrcStream(MODE_AUDIO, TRUE);
		}

		FindMtAndAdjustMtRecSrcStream( tMt,MODE_AUDIO,TRUE );
		
		AdjustSwitchToAllMcWatchingSrcMt( tMt,FALSE,MODE_AUDIO,FALSE );

		AdjustSwitchToMonitorWatchingSrc( tMt,MODE_AUDIO,FALSE,TRUE );
		
		CSendSpy cSendSpy;
		if( !m_tCascadeMMCU.IsNull() && m_cLocalSpyMana.GetSpyChannlInfo( tMt,cSendSpy ) )
		{
			if( MODE_BOTH == cSendSpy.GetSpyMode() || MODE_AUDIO == cSendSpy.GetSpyMode() )
			{				
				u16 wErrorCode = 0;
				StartSwitchToMcuByMultiSpy( tMt, 0, m_tCascadeMMCU.GetMtId(), cSendSpy.GetSimCapset(), wErrorCode,
											MODE_AUDIO, SWITCH_MODE_SELECT, FALSE );
			}
		}
		
	}

	return;
}

/*=============================================================================
    函 数 名： IsSelModeAndCapMatched
    功    能： 判断选看模式和对应能力集是否匹配
    算法实现： 
    全局变量： 
    参    数： 
               u8 &bySwitchMode       : IN/OUT，选看模式
               TSimCapSet tSimCapSrc  : 源端能力集
               TSimCapSet tSimCapDst  : 目的端能力集
               BOOL32     bAccord2Adp : 针对视频，是否需要经过适配选看 FIXME:暂未支持标清会议的跨适配选看
    返 回 值： BOOL32     选看是否可以继续(模式完全匹配或过适配后匹配)
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/04/14  4.0			张宝卿                创建
    2008/07/10  4.5         周广程                修改
    2009/05/22  4.5         张宝卿                视频选看格式适配支持、跨分辨率适配支持
=============================================================================*/
BOOL32 CMcuVcInst::IsSelModeAndCapMatched( u8               &bySwitchMode, 
                                           const TSimCapSet &tSimCapSrc, 
                                           const TSimCapSet &tSimCapDst,
                                           BOOL32           &bAccord2Adp)
{
    bAccord2Adp = FALSE;

    // xsl [11/10/2006] 若配置文件允许双格式会议选看，则不进行判断
    if (g_cMcuVcApp.IsSelInDoubleMediaConf() && !IsHDConf(m_tConf))
    {
        return TRUE;
    }

// #ifdef _8KE_		
// 	//8KE有专门的视频选看通道,对于高标清会议均自动允许视频选看适配
// 	if(tSimCapSrc.GetVideoMediaType() != tSimCapDst.GetVideoMediaType() ||
// 		IsResG(tSimCapSrc.GetVideoResolution(), tSimCapDst.GetVideoResolution()))
// 	{
// 		ConfLog(FALSE, "[IsSelModeAndCapMatched] 8KE permit Dst<VidType:%d, Res:%d> sel to see Src<VidType:%d, Res:%d> across Bas!\n",
// 			tSimCapDst.GetVideoMediaType(), tSimCapDst.GetVideoResolution(),
// 			tSimCapSrc.GetVideoMediaType(), tSimCapSrc.GetVideoResolution());			
// 		bAccord2Adp = TRUE;
// 	}		
// 	return TRUE;
// #else

    //选看音视频
    if ( MODE_BOTH == bySwitchMode )
    {
        //音频或视频能力项不匹配
        if ( tSimCapSrc.GetAudioMediaType() != tSimCapDst.GetAudioMediaType() )
        {
            bySwitchMode = MODE_VIDEO;
        }
        if ( tSimCapSrc.GetVideoMediaType() != tSimCapDst.GetVideoMediaType() ) 
        {
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
            if (!IsHDConf(m_tConf) || !g_cMcuVcApp.IsSelAccord2Adp())
            {
                bySwitchMode = ( MODE_BOTH == bySwitchMode ) ? MODE_AUDIO : MODE_NONE;

                ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[IsSelModeAndCapMatched] capbility match failed: \n\t\tSrcCap audio<%d> video<%d> vs DstCap audio<%d> video<%d>\n",
                    tSimCapSrc.GetAudioMediaType(), tSimCapSrc.GetVideoMediaType(),
                    tSimCapDst.GetAudioMediaType(), tSimCapDst.GetVideoMediaType() );
            }
            else
#endif // ifndef _8KE_
            {
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[IsSelModeAndCapMatched] capbility match loss, need adp: \n\t\tSrcCap audio<%d> video<%d> vs DstCap audio<%d> video<%d>\n",
                    tSimCapSrc.GetAudioMediaType(), tSimCapSrc.GetVideoMediaType(),
                    tSimCapDst.GetAudioMediaType(), tSimCapDst.GetVideoMediaType() );
                
                bAccord2Adp = TRUE;
            }
        }
        if ( MODE_NONE == bySwitchMode )
        {           
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MT, "[IsSelModeAndCapMatched] capbility match failed: \n\t\tSrcCap audio<%d> video<%d> vs DstCap audio<%d> video<%d>\n",
				     tSimCapSrc.GetAudioMediaType(), tSimCapSrc.GetVideoMediaType(),
                     tSimCapDst.GetAudioMediaType(), tSimCapDst.GetVideoMediaType() );
            return FALSE;
        }
    }
    //选看视频
    else if ( MODE_VIDEO == bySwitchMode )
    {
        //视频能力项不匹配
        if ( tSimCapSrc.GetVideoMediaType() != tSimCapDst.GetVideoMediaType() )
        {
#if !defined(_8KE_) && !defined(_8KH_)	&& !defined(_8KI_)
			if (!IsHDConf(m_tConf) || !g_cMcuVcApp.IsSelAccord2Adp())
            {
                bySwitchMode = MODE_NONE;
				ConfPrint( LOG_LVL_WARNING, MID_MCU_MT, "[IsSelModeAndCapMatched] capbility match failed: \n\t\tSrcCap video<%d> vs DstCap video<%d>\n",
					tSimCapSrc.GetVideoMediaType(), tSimCapDst.GetVideoMediaType() );
                return FALSE;
            }
            else
#endif //ifndef _8KE_
            {
				ConfPrint( LOG_LVL_WARNING, MID_MCU_MT, "[IsSelModeAndCapMatched] capbility match loss, need adp: \n\t\tSrcCap video<%d> vs DstCap video<%d>\n",
                    tSimCapSrc.GetVideoMediaType(), tSimCapDst.GetVideoMediaType() );
                bAccord2Adp = TRUE;
            }          
        }
    }
    //选看音频
    else if ( MODE_AUDIO == bySwitchMode )
    {
        //音频能力项不匹配
        if ( tSimCapSrc.GetAudioMediaType() != tSimCapDst.GetAudioMediaType() )
        {
            ConfPrint( LOG_LVL_WARNING, MID_MCU_MT, "[IsSelModeAndCapMatched] capbility match failed: \n\t\tSrcCap audio<%d> vs DstCap audio<%d>\n",
                     tSimCapSrc.GetAudioMediaType(), tSimCapDst.GetAudioMediaType() );
            bySwitchMode = MODE_NONE;
            return FALSE;
        }
    }

    // 若选看视频, 分辨率不一致，NACK
    // zgc, 2008-07-10, 移到 IsSelModeAndCapMatched 中
    if( ( MODE_VIDEO == bySwitchMode || MODE_BOTH  == bySwitchMode) &&
        tSimCapSrc.GetVideoResolution() != tSimCapDst.GetVideoResolution() ) 
    {
        // xsl [10/10/2006] 接收终端分辨率为自适应时允许选看源与目的终端分辨率不一致
        if( VIDEO_FORMAT_AUTO != tSimCapDst.GetVideoResolution() &&
            ( MEDIA_TYPE_H264 != tSimCapDst.GetVideoMediaType() || 
              IsResGE(tSimCapSrc.GetVideoResolution(), tSimCapDst.GetVideoResolution()))) 
        {
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)	
            if (!IsHDConf(m_tConf) || !g_cMcuVcApp.IsSelAccord2Adp())
            {
                bySwitchMode = ( MODE_BOTH == bySwitchMode ) ? MODE_AUDIO : MODE_NONE;
				ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[IsSelModeAndCapMatched] capbility match failed: \n\t\tSrcCap VidRes<%d> vs DstCap VidRes<%d>\n",
                    tSimCapSrc.GetVideoResolution(), tSimCapDst.GetVideoResolution() );
            }
            else
#endif //ifndef _8KE_
            {
				ConfPrint(LOG_LVL_WARNING, MID_MCU_MT,  "[IsSelModeAndCapMatched] capbility match loss, need adp: \n\t\tSrcCap VidRes<%d> vs DstCap VidRes<%d>\n",
                    tSimCapSrc.GetVideoResolution(), tSimCapDst.GetVideoResolution() );
                bAccord2Adp = TRUE;
            }
        }
        if ( MODE_NONE == bySwitchMode )
        {
            return FALSE;
        }
    }
    
    return TRUE;
// #endif //ifdef _8KE_
}

/*=============================================================================
    函 数 名： AdjustKedaMcuAndTaideMuteInfo
    功    能： 调整非KEDA终端（KEDAMCU和Tandberg）静音和哑音
    算法实现： 
    全局变量： 
    参    数： TMt * ptMt       : 须调整的终端结构
               BOOL32 bDecodeAud: 1－解码，0－采集
               BOOL32 bOpenFlag : 1－静音, 0－非静音
    返 回 值： void
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/04/15  4.0			张宝卿                创建
=============================================================================*/
void CMcuVcInst::AdjustKedaMcuAndTaideMuteInfo( TMt * ptMt, BOOL32 bDecodeAud, BOOL32 bOpenFlag )
{
    if ( NULL == ptMt )
    {
        ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[AdjustKedaMcuAndTaideMuteInfo] ptMt NULL ! \n");
        return;
    }
    
    //上报MCS
    TMtStatus tMtStatus;
    m_ptMtTable->GetMtStatus(ptMt->GetMtId(), &tMtStatus);
    if( TRUE == bDecodeAud ) 
    {
        tMtStatus.SetDecoderMute( bOpenFlag ); //解码是否静音	        
    }
    else
    {
        tMtStatus.SetCaptureMute( bOpenFlag );	//采集是否静音(是否哑音)
    }
    m_ptMtTable->SetMtStatus(ptMt->GetMtId(), &tMtStatus);

    MtStatusChange( ptMt, TRUE );
    
    //调整Taidberg和KDCMCU静哑音后的码流交换
    AdjustKedaMcuAndTaideAudioSwitch( ptMt->GetMtId(), bDecodeAud, bOpenFlag );
    
    return;
}

/*=============================================================================
函 数 名： ProcPolycomSecVidChnnl
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： void
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2008/03/20  4.0			张宝卿                创建
=============================================================================*/
void CMcuVcInst::ProcPolycomSecVidChnnl(CMessage * const pcMsg)
{
	u8 byPolycomMtId = (u8)(*(u32*)pcMsg->content);
	if ( byPolycomMtId < 1 || byPolycomMtId > MAXNUM_CONF_MT )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MT, "[PolycomSecChnnl] byPolycomMtId.%d ignore it\n", byPolycomMtId );
		return;
	}
	
	TCapSupport tPolyMcuCap;
	BOOL32 bRet = m_ptMtTable->GetMtCapSupport(byPolycomMtId, &tPolyMcuCap);
	if ( !bRet )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[PolycomSecChnnl] get PolyMt.%d's cap failed\n", byPolycomMtId );
		return;
	}
	TLogicalChannel tLogicChan;
	TMt tPolyMt = m_ptMtTable->GetMt(byPolycomMtId);
	McuMtOpenDoubleStreamChnnl(tPolyMt, tLogicChan/*, tPolyMcuCap*/);
	
	return;
}

/*==============================================================================
函数名    :  SwitchNewVmpToSingleMt
功能      :  将新VMP输出交换到单个MT
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-3-31	4.6				薛亮							create
==============================================================================*/
void CMcuVcInst::SwitchNewVmpToSingleMt(TMt tMt)
{
	if (tMt.IsNull())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[SwitchNewVmpToSingleMt] illegal tMt!\n");
		return;
	}

	if (CONF_VMPMODE_NONE == g_cMcuVcApp.GetVMPMode(m_tVmpEqp))
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP,"[SwitchNewVmpToSingleMt] No VMP is used!\n");
		return;
	}
	

	u8 bySrcChnnl = GetVmpOutChnnlByDstMtId( tMt.GetMtId(), m_tVmpEqp.GetEqpId());


	if( bySrcChnnl == (u8)~0 )
	{
		ConfPrint(LOG_LVL_WARNING,  MID_MCU_VMP, "NewVmpToMt Mt.%u cannot rcv vmp brd for unmatched fmt or res!\n");
	}
	else
	{
		g_cMpManager.SetSwitchBridge(m_tVmpEqp, bySrcChnnl, MODE_VIDEO);	//建桥
		//zjl 20110510 StartSwitchToAll 替换 StartSwitchToSubMt
		//StartSwitchToSubMt(m_tVmpEqp, bySrcChnnl, tMt.GetMtId(), MODE_VIDEO, SWITCH_MODE_SELECT);//switchmode 非默认值，是SWITCH_MODE_SELECT
		TSwitchGrp tSwitchGrp;
		tSwitchGrp.SetSrcChnl(bySrcChnnl);
		tSwitchGrp.SetDstMtNum(1);
		tSwitchGrp.SetDstMt(&tMt);
		StartSwitchToAll(m_tVmpEqp, 1, &tSwitchGrp, MODE_VIDEO, SWITCH_MODE_SELECT);
	}

	//vmp广播时，增加单条终端到PRS交换
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	if (tConfAttrb.IsResendLosePack())
	{
		if ( 0xFF != bySrcChnnl ) 
		{
			u8 byPrsId    = 0;
			u8 byPrsChnId = 0;
			if (FindPrsChnForSrc(m_tVmpEqp, bySrcChnnl, MODE_VIDEO, byPrsId, byPrsChnId))
			{
				AddRemovePrsMember(tMt.GetMtId(), byPrsId, byPrsChnId, MODE_VIDEO, TRUE);
			}
		}
	}
	
/*	u8 byOutIdx = 0;
	u8 abyVmpFormat[3] = {	VIDEO_FORMAT_HD720,  //tempariry value.  720的mt也能收1080
		0, 
		VIDEO_FORMAT_CIF 
	};
	if(m_tConf.GetMainVideoFormat() == VIDEO_FORMAT_4CIF)
	{
		abyVmpFormat[0] = VIDEO_FORMAT_4CIF;
		abyVmpFormat[1] = 0;
	}
	
	for(byOutIdx = 0; byOutIdx < 3; byOutIdx++)
	{
		byVmpRes = abyVmpFormat[byOutIdx];
		if(byVmpRes <= byMtRes) 
		{
			bySrcChnnl = byOutIdx;
			break;
		}
	}
	if( byOutIdx >= 3 
		|| byMtMediaType != MEDIA_TYPE_H264 //目前不支持非h264
		)
	{
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMtMcuOpenLogicChnnlRsp] Mt.%u cannot receive vmp broardcast \
		because unmatched format or resolution!\n");
	}
	else
	{
		g_cMpManager.SetSwitchBridge(m_tVidBrdSrc, bySrcChnnl, MODE_VIDEO);	//建桥
		StartSwitchToSubMt(m_tVidBrdSrc, bySrcChnnl, tMt.GetMtId(), MODE_VIDEO, SWITCH_MODE_SELECT);//switchmode 非默认值，是SWITCH_MODE_SELECT
	}
*/

}

/*====================================================================
    函数名      ：PackAndSendMtList2Mcs
    功能        ：打包消息，发MCU_MCS_GETMTLIST_NOTIF给界面
    算法实现    ：
    引用全局变量：
    输入参数说明：TMtExtU *atMtExtU,　终端列表
				　u8 &byMtNum, 终端数量
				　u16 &wMcuIdx　所在mcu
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2013/7/10   4.7.2       彭国锋          创建
====================================================================*/
void CMcuVcInst::PackAndSendMtList2Mcs( u8 &byMtNum, TMtExtU *ptMtExtU, const u16 &wMcuIdx)
{
	if ( !ptMtExtU )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[PackAndSendMtList2Mcs]invalid ptMtExtU\n");
		return;
	}
	CServMsg cMsg2Mcs;
	cMsg2Mcs.SetConfId(m_tConf.GetConfId());
	cMsg2Mcs.SetConfIdx(m_byConfIdx);
	cMsg2Mcs.SetEventId(MCU_MCS_GETMTLIST_NOTIF);

	cMsg2Mcs.SetMsgBody(&byMtNum, sizeof(u8));
	u16 wSndMcuIdx = htons(wMcuIdx);
	cMsg2Mcs.CatMsgBody((u8 *)&wSndMcuIdx, sizeof(u16));
	u8 byHasHeadInfo = 1; //是否有头部信息
	cMsg2Mcs.CatMsgBody(&byHasHeadInfo, sizeof(u8));

	u32 dwHeadLen = ptMtExtU[0].GetTotalMemLen()+sizeof(u8); //成员长度+成员个数
	LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp] dwHeadLen:%d\n", dwHeadLen);
	dwHeadLen = htonl(dwHeadLen);
	cMsg2Mcs.CatMsgBody((u8 *)&dwHeadLen, sizeof(u32));

	u8 byMemNum = ptMtExtU[0].GetMemNum();
	cMsg2Mcs.CatMsgBody(&byMemNum, sizeof(u8));
	u16 wMemLen = 0;
	LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp] struct member:\n", dwHeadLen);
	for ( u8 byMemId = 1; byMemId <= byMemNum;byMemId++)
	{
		wMemLen = ptMtExtU[0].GetMemLen(byMemId);
		LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "%d ", wMemLen);
		wMemLen = htons(wMemLen);
		cMsg2Mcs.CatMsgBody((u8*)&wMemLen, sizeof(u16));
	}

	for ( u8 byMtId = 0;byMtId < byMtNum;byMtId++)
	{
		cMsg2Mcs.CatMsgBody((u8*)&ptMtExtU[byMtId], sizeof(TMtExtU));
	}

	SendMsgToAllMcs(MCU_MCS_GETMTLIST_NOTIF, cMsg2Mcs);

	return;
}

/*====================================================================
    函数名      ：ProcMtAdpMcuMtListRsp
    功能        ：打包消息，发MCU_MCS_GETMTLIST_NOTIF给界面 AND TO MMCU
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg,　消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2013/7/1    4.7.2       彭国锋          创建
====================================================================*/
void CMcuVcInst::ProcMtAdpMcuMtListRsp(const CMessage * pcMsg )
{
	// 添加状态校验 [pengguofeng 7/23/2013]
	STATECHECK 

	if ( !pcMsg )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp]pcMsg == NULL\n");
		return;
	}
	
	CServMsg cServMsg(pcMsg->content,pcMsg->length);

	if ( cServMsg.GetEventId() != MTADP_MCU_GETMTLIST_NOTIF )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp]invalid Event(NOT MTADP_MCU_GETMTLIST_NOTIF)\n");
		return; //非ACK或NOTIF的消息不需要处理
	}
	
	u8 abyMcuIdx[MAX_CASCADEDEPTH-1];
	abyMcuIdx[0] = cServMsg.GetSrcMtId();
	
	// 遍历所有SMcu [pengguofeng 7/1/2013]
	u8 *pMsg = cServMsg.GetMsgBody();
	u32 dwMsgRLen = 0;

	u8 bySend2MMcu = 0;
	while ( cServMsg.GetMsgBodyLen() > dwMsgRLen )
	{
		bySend2MMcu = *(u8 *)(pMsg + dwMsgRLen);
		dwMsgRLen += sizeof(u8);

		abyMcuIdx[1] = *(u8 *)(pMsg + dwMsgRLen);
		dwMsgRLen += sizeof(u8);
		
		u16 wMcuIdx = INVALID_MCUIDX;
		if( !m_tConfAllMcuInfo.AddMcu( abyMcuIdx,2,&wMcuIdx ) )
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_CONF,  "[ProcMtAdpMcuMtListRsp] Fail to Add Mcu<%d %d> Info.McuId(%d)\n",
				abyMcuIdx[0], abyMcuIdx[1], wMcuIdx);
			return; //有一个出错就必须return
		}
		else
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF,  "[ProcMtAdpMcuMtListRsp] Add Mcu<%d %d> Info.McuId(%d) successd\n",
				abyMcuIdx[0], abyMcuIdx[1], wMcuIdx);
		}
	
		u8 byMtNum = *(u8 *)(pMsg + dwMsgRLen);

		dwMsgRLen += sizeof(u8);
		
		u8 byMtIdx = 0;
		u8 byMtId = 0;
		TMtExtU atMtExtU[MAXNUM_CONF_MT];
		memset(atMtExtU, 0, sizeof(atMtExtU));

		if ( !m_ptConfOtherMcTable ) // 防止结会时操作它 [pengguofeng 7/16/2013]
		{
			LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp]m_ptConfOtherMcTable is NULL state.%d\n", CurState());
			return;
		}
		
		TConfMcInfo* ptMcMtInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
		if ( !ptMcMtInfo )
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_CONF,  "[ProcMtAdpMcuMtListRsp]2 Fail to Add Mcu Info.McuId(%d)\n", wMcuIdx);
			return;
		}
		else
		{
			LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp]2 add wMcuIdx.%d succeed\n", wMcuIdx);
		}
		
		LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp]recv %d mt from mtadp\n", byMtNum);

		s8 achMtName[VALIDLEN_ALIAS_UTF8];
		u16 wMtNameLen = 0;
/*		CServMsg cMsg2Mcs;
		cMsg2Mcs.SetConfId(m_tConf.GetConfId());
		cMsg2Mcs.SetConfIdx(m_byConfIdx);
		cMsg2Mcs.SetEventId(MCU_MCS_GETMTLIST_NOTIF);
		
		cMsg2Mcs.SetMsgBody(&byMtNum, sizeof(u8));
		u16 wSndMcuIdx = htons(wMcuIdx);
		cMsg2Mcs.CatMsgBody((u8 *)&wSndMcuIdx, sizeof(u16));
		u8 byHasHeadInfo = 1;
		cMsg2Mcs.CatMsgBody(&byHasHeadInfo, sizeof(u8));
		u32 dwHeadLen = atMtExtU[0].GetTotalMemLen()+sizeof(u8);
 		LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp] dwHeadLen:%d\n", dwHeadLen);
		dwHeadLen = htonl(dwHeadLen);
		cMsg2Mcs.CatMsgBody((u8 *)&dwHeadLen, sizeof(u32));
		
		u8 byMemNum = atMtExtU[0].GetMemNum();
		cMsg2Mcs.CatMsgBody(&byMemNum, sizeof(u8));
		u16 wMemLen = 0;
 		LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp] struct member:\n", dwHeadLen);
		for ( u8 byMemId = 1; byMemId <= byMemNum;byMemId++)
		{
			wMemLen = atMtExtU[0].GetMemLen(byMemId);
 			LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "%d ", wMemLen);
			wMemLen = htons(wMemLen);
			cMsg2Mcs.CatMsgBody((u8*)&wMemLen, sizeof(u16));
		}
*/
		TMtExt *ptMtExt = NULL;
		TMt tMt;
		tMt.SetNull();
		tMt.SetMcuId(wMcuIdx);

		CServMsg cMsg2Mcu; // 发消息给MMcu [pengguofeng 7/10/2013]
		cMsg2Mcu.SetConfId(m_tConf.GetConfId());
		cMsg2Mcu.SetConfIdx(m_byConfIdx);
		cMsg2Mcu.SetEventId(MCU_MCU_MTLIST_ACK);
		u8 byLastPack = 1;
		cMsg2Mcu.SetMsgBody(&byLastPack, sizeof(u8));

		TMcuMcuMtInfo tMtInfo[MAXNUM_CONF_MT];
		memset(&tMtInfo, 0, sizeof(tMtInfo)); //待发给上级的

		while (byMtIdx < byMtNum)
		{
			byMtId = *(u8 *)(pMsg + dwMsgRLen);
			if (/* byMtId == 0 ||*/ byMtId > MAXNUM_CONF_MT )
			{
				LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp]invalid MtId.%d\n", byMtId);
				return;
			}

			dwMsgRLen += sizeof(u8);
			wMtNameLen = *(u16 *)(pMsg + dwMsgRLen);
			dwMsgRLen += sizeof(u16);
			wMtNameLen = ntohs(wMtNameLen);
			u16 wOrgNameLen = wMtNameLen;

			if ( wMtNameLen > VALIDLEN_ALIAS_UTF8-1)
			{
				wMtNameLen = VALIDLEN_ALIAS_UTF8-1;
			}
			memcpy(achMtName, pMsg+ dwMsgRLen, wMtNameLen);
			achMtName[wMtNameLen] = 0;
			dwMsgRLen += wOrgNameLen; //消息先移动，防止下面修改了长度

			tMt.SetMtId(byMtId);
			ptMtExt = ptMcMtInfo->GetMtExt(tMt);
			if ( !ptMtExt )
			{
				LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp]Mt<%d.%d> not found in other mc table\n", wMcuIdx, byMtId);
				return;
			}

 			LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp] copy Mt.%d Name:(%s )Len:(%d)\n", byMtId, achMtName, wMtNameLen);
			// 注意mt table的下标和byMtId不是一一对应的 [pengguofeng 7/3/2013]
			memcpy(&atMtExtU[byMtIdx/*byMtId-1*/], ptMtExt/*&(ptMcMtInfo->m_atMtExt[byMtId-1])*/, sizeof(TMt) + sizeof(u8)*2+sizeof(u16) + sizeof(u32)*2 );
			atMtExtU[byMtIdx].SetProtocolType(ptMtExt->GetProtocolType());
			atMtExtU[byMtIdx].SetAlias(achMtName);
// 			cMsg2Mcs.CatMsgBody((u8*)&atMtExtU[byMtIdx], sizeof(TMtExtU));

 			SetMtInfoByMtExtU(wMcuIdx, atMtExtU[byMtIdx], tMtInfo[byMtIdx]); //由mcu来完成
			byMtIdx++;
		}
		
//		SendMsgToAllMcs(MCU_MCS_GETMTLIST_NOTIF, cMsg2Mcs);
		PackAndSendMtList2Mcs(byMtNum, atMtExtU, wMcuIdx);

		// 上报MMCU [pengguofeng 7/4/2013]
		cMsg2Mcu.CatMsgBody((u8 *)tMtInfo, byMtNum*sizeof(TMcuMcuMtInfo));

		LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp]bySend2MMcu:%d from mcu.%d MMcu.%d\n",
			bySend2MMcu, abyMcuIdx[0], m_tCascadeMMCU.GetMtId());
		if ( bySend2MMcu != 0 && !m_tCascadeMMCU.IsNull() 
			&& abyMcuIdx[0] != m_tCascadeMMCU.GetMtId() ) // 上级自己的则不用回过去 [pengguofeng 7/10/2013]
		{
			LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp]Pack mcu.%d and send to MMCU.%d\n", wMcuIdx, m_tCascadeMMCU.GetMtId());
			SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MCU_MTLIST_ACK, cMsg2Mcu);

			SendAllSMcuMtStatusToMMcu(); // 补报MtStatus [pengguofeng 7/26/2013]
			SendSMcuUpLoadMtToMMcu();
		}

		// 上报SMCU [pengguofeng 7/4/2013]
/*		if ( g_cMcuVcApp.IsShowMMcuMtList() )
		{
			for ( u8 bySMcuMtId = 1; bySMcuMtId <= MAXNUM_CONF_MT; bySMcuMtId++)
			{
				if ( m_ptMtTable->GetMtType(bySMcuMtId) == MT_TYPE_SMCU )
				{
					LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMtAdpMcuMtListRsp]Pack mcu.%d and send to MMCU.%d\n", wMcuIdx, bySMcuMtId);
					SendMsgToMt(bySMcuMtId, MCU_MCU_MTLIST_ACK, cMsg2Mcu);
				}
			}
		}
*/	}
}

/*====================================================================
    函数名      ：ProcMtAdpMcuSmcuMtAliasRsp
    功能        ：获得下级mcu终端别名通知
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg,　消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2013/8/8    4.7.2      yanghuaizhi      创建
====================================================================*/
void CMcuVcInst::ProcMtAdpMcuSmcuMtAliasRsp(const CMessage * pcMsg )
{
	// 添加状态校验
	STATECHECK 

	if ( !pcMsg )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtAdpMcuSmcuMtAliasRsp]pcMsg == NULL\n");
		return;
	}
	
	CServMsg cServMsg(pcMsg->content,pcMsg->length);
	if ( cServMsg.GetEventId() != MTADP_MCU_GETSMCUMTALIAS_NOTIF )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtAdpMcuSmcuMtAliasRsp]invalid Event(NOT MTADP_MCU_GETSMCUMTALIAS_NOTIF)\n");
		return; //非ACK或NOTIF的消息不需要处理
	}
	
	u8 abyMcuIdx[MAX_CASCADEDEPTH-1];
	abyMcuIdx[0] = cServMsg.GetSrcMtId();
	
	// 解析消息：byEqpid + byMcuId + byMtNum + byMtNum*[ byMtId + byChlNum + byChlNum*[byChlIdx] + byNameLen + pName]
	u8 *pbyBuf = (u8 *)cServMsg.GetMsgBody();
	u8 byEqpid = *pbyBuf;// byEqpid
	pbyBuf++;
	abyMcuIdx[1] = *pbyBuf;
	pbyBuf++;
	u8 byMtNum = *pbyBuf;
	pbyBuf++;
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF,  "[ProcMtAdpMcuSmcuMtAliasRsp] Eqpid.%d MtNum.%d\n",
				byEqpid, byMtNum);

	u16 wMcuIdx = INVALID_MCUIDX;
	m_tConfAllMcuInfo.GetIdxByMcuId(abyMcuIdx,2,&wMcuIdx);
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF,  "[ProcMtAdpMcuSmcuMtAliasRsp] Mcu<%d %d> Info.McuId(%d)\n",
				abyMcuIdx[0], abyMcuIdx[1], wMcuIdx);

	TMt tMt;
	tMt.SetNull();
	tMt.SetMcuId(wMcuIdx);
	// 判eqpid是画面合成的话，走画面合成逻辑
	if (IsValidVmpId(byEqpid))
	{
		TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byEqpid );
		TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
		TVMPMember tVmpMember;
		// 兼容考虑，发MCU_VMP_CHANGEMEMALIAS_CMD消息时，第一个终端需更新ChnIndex，其余终端填入MsgBody中
		BOOL32 bFristMt = TRUE;
		
		u8 byMtId;
		u8 byChlNum;
		s8 achAlias[MAXLEN_ALIAS] = {0};
		u8 byNameLen = 0;
		u8 byMtIdx;
		u8 byChlIdx;
		u8 abyVmpChl[MAXNUM_VMP_MEMBER];
		CServMsg cMsg;
		cMsg.SetMsgBody();
		for (byMtIdx=0; byMtIdx<byMtNum;byMtIdx++)
		{
			memset(achAlias, 0, sizeof(achAlias));
			memset(abyVmpChl, 0, sizeof(abyVmpChl));
			//获得Mtid
			byMtId = *pbyBuf;
			pbyBuf++;
			if (byMtId > MAXNUM_CONF_MT)
			{
				LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtAdpMcuSmcuMtAliasRsp]invalid MtId.%d\n", byMtId);
				return;
			}
			else
			{
				tMt.SetMtId(byMtId);
			}
			byChlNum = *pbyBuf;
			pbyBuf++;
			if (byChlNum > MAXNUM_VMP_MEMBER)
			{
				LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtAdpMcuSmcuMtAliasRsp]invalid VmpChlNum.%d\n", byChlNum);
				return;
			}
			// 读取vmp通道数组
			memcpy(abyVmpChl, pbyBuf, byChlNum);
			pbyBuf += byChlNum;
			// 读取终端别名信息
			byNameLen = *pbyBuf;
			pbyBuf++;
			if (byNameLen > VALIDLEN_ALIAS_UTF8-1)
			{
				byNameLen = VALIDLEN_ALIAS_UTF8-1;
			}
			memcpy(achAlias, pbyBuf, byNameLen);
			pbyBuf += byNameLen;
			for (byChlIdx=0; byChlIdx<byChlNum;byChlIdx++)
			{
				if (abyVmpChl[byChlIdx] >= MAXNUM_VMP_MEMBER)
				{
					LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtAdpMcuSmcuMtAliasRsp]invalid VmpChl.%d\n", abyVmpChl[byChlIdx]);
					return;
				}
				tVmpMember = *tConfVmpParam.GetVmpMember(abyVmpChl[byChlIdx]);
				// 终端与对应vmp通道一致，才给外设发送刷新别名消息
				if (tMt.GetMcuId() == tVmpMember.GetMcuId() &&
					tMt.GetMtId() == tVmpMember.GetMtId())
				{
					if (bFristMt)
					{
						bFristMt = FALSE;
						cMsg.SetChnIndex(abyVmpChl[byChlIdx]);
					}
					cMsg.CatMsgBody( (u8*)achAlias, sizeof(achAlias) );
					cMsg.CatMsgBody( (u8*)&abyVmpChl[byChlIdx], sizeof(u8));
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ProcMtAdpMcuSmcuMtAliasRsp]vmp member.%d info: mt(%d,%d), alias is %s!\n", 
						abyVmpChl[byChlIdx], tMt.GetMcuId(), tMt.GetMtId(), achAlias);
				}
			}
		}

		// 对指定通道发送消息
		if (cMsg.GetMsgBodyLen() > 0)
		{
			// 刷新别名消息扩容，[别名128 + ChlIdx] * 通道个数
			SendMsgToEqp(tVmpEqp.GetEqpId(), MCU_VMP_CHANGEMEMALIAS_CMD, cMsg);
		}
	}
	else
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[ProcMtAdpMcuSmcuMtAliasRsp]Eqpid.%d is not vmp.\n", byEqpid);
		return;
	}


}

/********************************************************************
	created:	2013/07/04
	created:	4:7:2013   9:36
	name: 		SetMtInfoByMtExtU
	author:		peng guofeng
	
	purpose:	根据wMcuIdx和tMtExtU来设置tMtInfo，主要为了向对方报MtList_Ack
*********************************************************************/
/*====================================================================
    函数名      ：SetMtInfoByMtExtU
    功能        ：根据wMcuIdx和tMtExtU来设置tMtInfo，主要为了向对方报MtList_Ack
    算法实现    ：
    引用全局变量：
    输入参数说明：const u16 &wMcuIdx,　mcu
				　const TMtExtU &tMtExtU，输入的MtExt信息
				　TMcuMcuMtInfo &tMtInfo待上报的结构体
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2013/7/1    4.7.2       彭国锋          创建
====================================================================*/
BOOL32 CMcuVcInst::SetMtInfoByMtExtU(const u16 &wMcuIdx, 
								   const TMtExtU &tMtExtU, TMcuMcuMtInfo &tMtInfo)
{
	if ( wMcuIdx == INVALID_MCUIDX )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[SetMtInfoByMtExtU]wMcuIdx == INVALID\n");
		return FALSE;
	}

	if ( !m_ptConfOtherMcTable ) // 防止结会时操作它 [pengguofeng 7/16/2013]
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[SetMtInfoByMtExtU]m_ptConfOtherMcTable is NULL state.%d\n", CurState());
		return FALSE;
	}
	
	//0. get mcu info
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
	
	if ( !ptMcInfo )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[PackMtListAckMsg]Get mcu.%d mcinfo Failed\n", wMcuIdx);
		return false;
	}

	//1. get mcuId
	u8 abyMcuId[MAX_CASCADEDEPTH-1];
	memset( &abyMcuId[0],0,sizeof(abyMcuId) );
	m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0] );

	//2. copy
	strncpy((s8 *)(tMtInfo.m_szMtName), tMtExtU.GetAlias(), sizeof(tMtInfo.m_szMtName) - 1);

	TMt tMt;
	memcpy(&tMt, &tMtExtU, sizeof(TMt));
	tMt.SetMcuIdx(wMcuIdx);
// 	tMt.SetMcuIdx(abyMcuId[0]); //此处仍用Idx的
	tMtInfo.m_tMt = tMt;
	tMtInfo.m_tMt.SetMcuIdx(abyMcuId[0]); // 传上去让一级得到正确的MtId [pengguofeng 7/26/2013]

	tMtInfo.m_dwMtIp = ntohl(tMtExtU.GetIPAddr());
	tMtInfo.m_byMtType = tMtExtU.GetMtType();
	tMtInfo.m_byManuId = tMtExtU.GetManuId();
	tMtInfo.m_byProtocolType = tMtExtU.GetProtocolType();
	strncpy((s8 *)tMtInfo.m_szMtDesc, "notsupport", sizeof(tMtInfo.m_szMtDesc)-1);

	// online
	TConfMtInfo *ptConfMtInfo = m_tConfAllMtInfo.GetMtInfoPtr(wMcuIdx);
	if ( !ptConfMtInfo )
	{
		tMtInfo.m_byIsConnected = 0;
	}
	else
	{
		tMtInfo.m_byIsConnected = ptConfMtInfo->MtJoinedConf( tMtExtU.GetMtId() );
	}

	//3 logic channel
	// Bug:此处应该取下级MCU的状态 [pengguofeng 7/26/2013]
	TLogicalChannel tChannel;
	TMcMtStatus *ptSubMtSts = ptMcInfo->GetMtStatus(tMt);	
	if ( !ptSubMtSts )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[SetMtInfoByMtExtU]get Mt<%d %d> status failed\n", tMt.GetMcuId(), tMt.GetMtId());
		return FALSE;
	}
	tMtInfo.m_byIsAudioMuteIn = ptSubMtSts->IsSendAudio() ? 0 : 1;
	tMtInfo.m_byIsAudioMuteOut = ptSubMtSts->IsReceiveAudio() ? 0: 1;
	tMtInfo.m_byIsVideoMuteIn = ptSubMtSts->IsSendVideo() ? 0:1;
	tMtInfo.m_byIsVideoMuteOut = ptSubMtSts->IsReceiveVideo()?0:1;
	
	tMtInfo.m_byVideoIn = ptSubMtSts->GetVideoIn();
	tMtInfo.m_byVideoOut = ptSubMtSts->GetVideoOut();
	tMtInfo.m_byAudioIn = ptSubMtSts->GetAudioIn();
	tMtInfo.m_byAudioOut = ptSubMtSts->GetAudioOut();
	tMtInfo.m_byVideo2In = ptSubMtSts->IsSndVideo2() ? 1: 0;
	tMtInfo.m_byVideo2Out = ptSubMtSts->IsRcvVideo2() ? 1:0;
	tMtInfo.m_byIsFECCEnable = ptSubMtSts->IsEnableFECC()? 1:0;
	TMtVideoInfo tMtVidInfo = ptSubMtSts->GetMtVideoInfo();
	memcpy(&tMtInfo.m_tPartVideoInfo, &tMtVidInfo, sizeof(TMtVideoInfo));
	tMtInfo.m_byIsDataMeeting = 0; 
	
	//注：以下目前没有用到，仅保留
	if ( tMt.GetMtId() == 0 ) // 下级MCU [pengguofeng 7/26/2013]
	{
		tMtInfo.m_byIsDataMeeting = m_ptMtTable->GetMtLogicChnnl( abyMcuId[0], LOGCHL_T120DATA, &tChannel, TRUE ) ? 1:0;
		if ( tMtInfo.m_byIsDataMeeting == 0 )
		{
			tMtInfo.m_byIsDataMeeting =  m_ptMtTable->GetMtLogicChnnl( abyMcuId[0], LOGCHL_T120DATA, &tChannel, FALSE ) ? 1:0;
		}
	}
	
	// 打印 [pengguofeng 7/5/2013]
	LogPrint( LOG_LVL_DETAIL, MID_MCU_CONF, "[SetMtInfoByMtExtU]set mtinfo to mt<%d %d> online:%d\n",tMt.GetMcuId(), tMt.GetMtId(), tMtInfo.m_byIsConnected);
	
	return TRUE;
}

/*=============================================================================
函 数 名： ProcMtMcuReleaseMtReq
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： void
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2009/07/14  4.0			付秀华                创建
=============================================================================*/
void CMcuVcInst::ProcMtMcuReleaseMtReq( const CMessage * pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	TMt tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	s8  achConfName[MAXLEN_CONFNAME + 1] = {0};

	const u16 wMsgBodyLen = cServMsg.GetMsgBodyLen();
	memcpy(achConfName, cServMsg.GetMsgBody(), min(MAXLEN_CONFNAME, wMsgBodyLen));

	cServMsg.SetMsgBody((u8*)&tMt, sizeof(tMt));
	cServMsg.CatMsgBody((u8*)achConfName, strlen(achConfName));
	cServMsg.SetEventId(MCU_VCS_RELEASEMT_REQ);
	SendMsgToAllMcs(MCU_VCS_RELEASEMT_REQ, cServMsg);
	
}

/*=============================================================================
函 数 名： RemoveMtInMutiTvWallMoudle
功    能： 
算法实现： 
全局变量： 
参    数： 
返 回 值： void
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2009/12/08  4.0			周嘉麟                创建
=============================================================================*/
void CMcuVcInst:: RemoveMtInMutiTvWallMoudle(TMt tMt)
{
	for(u8 byLoop = 0; byLoop < MAXNUM_PERIEQP_CHNNL; byLoop++)
	{
		u8 byTvId = 0;
		m_tConfEqpModule.m_tTvWallInfo[byLoop].RemoveMtByMtIdx(tMt.GetMtId(), byTvId);
		if( 0 != byTvId )
		{
			m_tConfEqpModule.m_tMultiTvWallModule.RemoveMtByTvId(byTvId, tMt.GetMtId());
		}
	} 
}

/*=============================================================================
函 数 名： RemoveMtFormPeriInfo
功    能： 在删除、挂断、终端掉线等情况下清除其在外设中的信息并调整
           外设参数拆除相应交换等
算法实现： 
全局变量： 
参    数： TMt tMt  要删除的终端（支持非local的Tmt信息）
           bReason        被删除的原因
返 回 值： void
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20100417    4.6		    pengjie                create
=============================================================================*/
void CMcuVcInst::RemoveMtFormPeriInfo( const TMt &tMt, u8 byReason )
{
	// [pengjie 2010/4/17] 该接口支持传非local的Tmt信息，而部分业务现在
	//                     是不进行多回传的，所以这部分业务只取local化得MCU信息
	TMt tLocalMt = GetLocalMtFromOtherMcuMt( tMt );

	CServMsg	cServMsg;
	BOOL32		bIsExceptionReason = FALSE;

	//zjj20091014 终端离线原因判断
	if( MTLEFT_REASON_EXCEPT == byReason ||
		MTLEFT_REASON_RTD == byReason ||
		MTLEFT_REASON_UNREACHABLE == byReason )	 
	{		
		m_ptMtTable->SetDisconnectReason( tMt.GetMtId(),byReason );
		bIsExceptionReason = TRUE;		
	}

	// 0、 [11/3/2010 xliang] 先看点名，点名将影响下面的MIXER，VMP
	BOOL32 bRollIgnoreMixer = FALSE;
	BOOL32 bRollIgnoreVmp	= FALSE;
	if ( ROLLCALL_MODE_NONE != m_tConf.m_tStatus.GetRollCallMode() )
    {
		if( tMt == m_tRollCaller || tMt == m_tRollCallee)
		{
			bRollIgnoreMixer = TRUE;
			if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
			{
				bRollIgnoreVmp = TRUE;
			}
			u16 wErrorCode = (tMt == m_tRollCaller)? ERR_MCU_ROLLCALL_CALLERLEFT: ERR_MCU_ROLLCALL_CALLEELEFT;
			NotifyMcsAlarmInfo(0, wErrorCode);
			RollCallStop(cServMsg);

		}
    }

	// 1、移除终端丢包重传相关信息 
	// Prs只是针对本级终端，所以用local化得信息
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	if (tConfAttrb.IsResendLosePack())
	{
		if(tMt.IsLocal())
		{
			if (tMt == GetAudBrdSrc())
			{
				StopBrdPrs(MODE_AUDIO);
			}
			else
			{
				TMt tMtSrc;
				u8 bySrcChn = 0;
				if(GetMtRealSrc(tLocalMt.GetMtId(), MODE_AUDIO, tMtSrc, bySrcChn))
				{
					u8 byPrsId = 0;
					u8 byPrsChnId = 0;
					if (tMtSrc == GetLocalAudBrdSrc())
					{
						if(FindPrsChnForBrd(MODE_AUDIO, byPrsId, byPrsChnId))
						{
							AddRemovePrsMember(tLocalMt.GetMtId(), byPrsId, byPrsChnId, MODE_AUDIO, FALSE);
						}
					}
					else
					{
						if(FindPrsChnForSrc(tMtSrc, bySrcChn, MODE_AUDIO, byPrsId, byPrsChnId))
						{
							AddRemovePrsMember(tLocalMt.GetMtId(), byPrsId, byPrsChnId, MODE_AUDIO, FALSE);
						}
					}
				}
				else
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[RemoveMtFormPeriInfo] GetMtRealSrc Mt.%d failed!\n", tLocalMt.GetMtId());
				}
			}

			if (tMt == GetVidBrdSrc())
			{
				StopBrdPrs(MODE_VIDEO);
			}
			else
			{
				TMt tMtSrc;
				u8 bySrcChn = 0;
				if(GetMtRealSrc(tLocalMt.GetMtId(), MODE_VIDEO, tMtSrc, bySrcChn))
				{
					u8 byPrsId = 0;
					u8 byPrsChnId = 0;
					if (tMtSrc == GetLocalVidBrdSrc())
					{
						if(FindPrsChnForBrd(MODE_VIDEO, byPrsId, byPrsChnId))
						{
							AddRemovePrsMember(tLocalMt.GetMtId(), byPrsId, byPrsChnId, MODE_VIDEO, FALSE);
						}
					}
					else
					{
						if(FindPrsChnForSrc(tMtSrc, bySrcChn, MODE_VIDEO, byPrsId, byPrsChnId))
						{
							AddRemovePrsMember(tLocalMt.GetMtId(), byPrsId, byPrsChnId, MODE_VIDEO, FALSE);
						}
					}
				}
				else
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[RemoveMtFormPeriInfo] GetMtRealSrc Mt.%d failed!\n", tLocalMt.GetMtId());
				}
			}

			if (tMt == m_tDoubleStreamSrc)
			{
				StopBrdPrs(MODE_SECVIDEO);
			}
			else
			{
				TMt tMtSrc;
				u8 bySrcChn = 0;
				if(GetMtRealSrc(tLocalMt.GetMtId(), MODE_SECVIDEO, tMtSrc, bySrcChn))
				{
					u8 byPrsId = 0;
					u8 byPrsChnId = 0;
					if (tMtSrc == m_tDoubleStreamSrc)
					{
						if(FindPrsChnForBrd(MODE_SECVIDEO, byPrsId, byPrsChnId))
						{
							AddRemovePrsMember(tLocalMt.GetMtId(), byPrsId, byPrsChnId, MODE_SECVIDEO, FALSE);
						}
					}
					else
					{
						if(FindPrsChnForSrc(tMtSrc, bySrcChn, MODE_SECVIDEO, byPrsId, byPrsChnId))
						{
							AddRemovePrsMember(tLocalMt.GetMtId(), byPrsId, byPrsChnId, MODE_SECVIDEO, FALSE);
						}
					}
				}
				else
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[RemoveMtFormPeriInfo] GetMtRealSrc Mt.%d failed!\n", tLocalMt.GetMtId());
				}
			}	
			
			if (GetLocalVidBrdSrc() == m_tVmpEqp)
			{
				u8 bySrcChnnl = GetVmpOutChnnlByDstMtId( tMt.GetMtId() , m_tVmpEqp.GetEqpId());
				if ( 0xFF != bySrcChnnl ) 
				{
					u8 byPrsId    = 0;
					u8 byPrsChnId = 0;
					if (FindPrsChnForSrc(m_tVmpEqp, bySrcChnnl, MODE_VIDEO, byPrsId, byPrsChnId))
					{
						AddRemovePrsMember(tMt.GetMtId(), byPrsId, byPrsChnId, MODE_VIDEO, FALSE);
					}
				}
				else
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[RemoveMtFormPeriInfo]Get tMt(%d,%d) vmp out chnnl failed! \n",tMt.GetMcuId(),tMt.GetMtId() );
				}
				TVMPParam_25Mem tVMPParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
				for (u8 byMtIdx = 0; byMtIdx < tVMPParam.GetMaxMemberNum(); byMtIdx++)
				{
					TVMPMember *ptVmpMember = tVMPParam.GetVmpMember(byMtIdx);
					if (ptVmpMember != NULL && *ptVmpMember == tMt)
					{
						StopRtcpSwitchAlltoOne(tMt, 0, MODE_VIDEO);
						break;
					}
				}	    	
			}	
		}
	}
			
	// 2、停止混音 
	if ( (m_tConf.m_tStatus.IsMixing() || m_tConf.m_tStatus.IsVACing() )
		&& !bRollIgnoreMixer && !tMt.IsNull() && tMt.IsLocal())
	{
		// xsl [8/4/2006] 定制混音成员处理
		if (m_tConf.m_tStatus.IsSpecMixing())
		{
			if (m_ptMtTable->IsMtInMixing(tMt.GetMtId()))
			{
				//zjj20091014
				if( bIsExceptionReason )
				{
					//20101110_tzy 如果异常掉线终端在混音器中并且仅有该终端，那么应该停止混音
					if (GetCurConfMixMtNum() == 1)
					{
						for (u8 byMtid = 1; byMtid <= MAXNUM_CONF_MT; byMtid++)
						{
							if (m_ptMtTable->IsMtInMixing(byMtid) )
							{
								if (m_ptMtTable->GetMtType(byMtid) == MT_TYPE_MT
									&& tMt.GetMtId() ==  byMtid)
								{
									RemoveSpecMixMember(&tMt, 1, FALSE);
								}
							}
						}
					}
					else
					{
						RemoveSpecMixMember(&tMt, 1, FALSE);
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true reseve mt in mixing" );

						BOOL32 bKeepInMixing = TRUE; // 是否保留在混音的状态，以便重新上线的恢复
						if ( m_tConf.GetConfSource() == VCS_CONF)
						{
							bKeepInMixing = FALSE; // VCS不需要恢复处理，因为单方调度和多方多画面是掉线之后不主动呼该终端，组呼模式下，虽然会主动呼叫，但是已经不是被调度终端了
						}
						TMtStatus tRemeMtStatus;
						m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tRemeMtStatus);
						tRemeMtStatus.SetInMixing(bKeepInMixing);
						m_ptMtTable->SetMtStatus(tMt.GetMtId(), &tRemeMtStatus);
					}
				}
				else
				{
					RemoveSpecMixMember(&tMt, 1, FALSE);
				}
				
			}            
		}
		else //vac或讨论移除通道
		{            
			//在混音组内此拆除交换
			if (m_ptMtTable->IsMtInMixGrp(tMt.GetMtId()))
			{       
				StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
							(MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt)), FALSE, MODE_AUDIO);
			}   
			RemoveMixMember(&tMt, FALSE); 
			GetMixChnPos(tMt,TRUE);

			//zjj20091014
			if( bIsExceptionReason )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true reseve mt in mixing group" );
				//m_ptMtTable->SetMtInMixGrp( tMt.GetMtId(), TRUE );
			}
		} 
	}

	// 3、停止VMP
	if (GetVmpCountInVmpList() > 0)
	{
		u8 byVmpId;
		TEqp tVmpEqp;
		BOOL32 bStopVmp = TRUE;
		BOOL32 bChangeNotify = FALSE;
		BOOL32 bChangeStyle = TRUE;
		TVMPParam_25Mem tVMPParam;
		TVMPMember* ptVmpMember = NULL;
		TMt tMtNull;
		tMtNull.SetNull();
		BOOL32 bMemberInMcu = FALSE;
		u8 byMaxNum;
		u16 wVcsCurChIndx;
		BOOL32 bChangeVmpStyle = TRUE;//ChangeVmpStyle针对所有自动画面合成，仅需调整1次
		for (u8 byVmpIdx=0; byVmpIdx<MAXNUM_CONF_VMP; byVmpIdx++)
		{
			if (!IsValidVmpId(m_abyVmpEqpId[byVmpIdx]))
			{
				continue;
			}
			byVmpId = m_abyVmpEqpId[byVmpIdx];
			tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );

			if( g_cMcuVcApp.GetVMPMode(tVmpEqp) != CONF_VMPMODE_NONE 
				&& !bRollIgnoreVmp )
			{
				bStopVmp = TRUE;
				bChangeNotify = FALSE;
				bChangeStyle = TRUE;
				tVMPParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);// m_tConf.m_tStatus.GetVmpParam();

				ptVmpMember = NULL;

				//lukunpeng 2010/07/09 VMP成员调整整理
				bMemberInMcu = FALSE;
				byMaxNum = tVMPParam.GetMaxMemberNum();
				wVcsCurChIndx = byMaxNum;
				//zhouyiliang 20101025 如果画面风格不变的情况下挂两个或两个以上终端，当前可用通道号不能用maxnum
				//只有多方多画面情况才有可能出现挂断终端画面合成风格不变的情况（其余组呼模式画面合成最多4画面，挂断后风格改变）
				if ( VCS_CONF == m_tConf.GetConfSource() && m_cVCSConfStatus.GetCurVCMode() == VCS_MULVMP_MODE )
				{
					//zhouyiliang 20101111多方多画面如果当前curchIndx和maxnum不一致,根据实际在线终端数来确定curchIndx
					if ( m_cVCSConfStatus.GetCurUseVMPChanInd() !=  byMaxNum ) 
					{
						wVcsCurChIndx = 1;
						for( u8 byMtIndex = 1; byMtIndex <= MAXNUM_CONF_MT; byMtIndex++ )
						{
							if ( !m_tConfAllMtInfo.MtJoinedConf(byMtIndex) 
							|| m_cVCSConfStatus.GetVcsBackupChairMan().GetMtId() == byMtIndex 
							|| m_tConf.GetChairman().GetMtId() == byMtIndex
							|| m_ptMtTable->GetMtType(byMtIndex) == MT_TYPE_VRSREC) //过滤vrs新录播
							{
								continue;
							}
							wVcsCurChIndx++;
						}
					}
					
				}
				for (u8 byIndex = 0; byIndex < byMaxNum ; ++byIndex)
				{
					ptVmpMember = tVMPParam.GetVmpMember(byIndex);
					bMemberInMcu = FALSE;
					if (ptVmpMember == NULL)
					{
						continue;
					}

					if (ptVmpMember->IsNull())
					{
						continue;
					}

					//如果是此终端在画面合成中，或者挂断的是下级的mcu，此下级的mcu也需要出画面合成
					//zhouyiliang 20100901 挂断的是mcu,下级mcu下的画面合成成员要出,先将成员设置为不在线，自动调整成员的时候就不会把它算进去
					bMemberInMcu = !tMt.IsNull() && IsMcu(tMt) && IsMtInMcu(tMt, (TMt)*ptVmpMember);
					if ( bMemberInMcu ) 
					{
						m_tConfAllMtInfo.RemoveJoinedMt( (TMt)*ptVmpMember );
					}
					if (*ptVmpMember == tMt
						|| bMemberInMcu
						)
					{
						//画面合成成员类型为“会控指定”时进行状态清理，删除其本身。其他画面
						//合成成员类型在此不作处理，防止误清除了相关的跟随通道。[03/21/2006-zbq]
						if ( VMP_MEMBERTYPE_MCSSPEC == ptVmpMember->GetMemberType() )
						{
							StopSwitchToPeriEqp( tVmpEqp.GetEqpId(), byIndex, FALSE, MODE_VIDEO );
						}
						if( !IsDynamicVmp(byVmpId) )
						{
							//zjj20091014
							//zhouyiliang 20101022 下级mcu异常掉线或者重启的时候该mcu下的vmp成员也要清除
							if( bIsExceptionReason && !bMemberInMcu )
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveMtFromVmpInfo] bReason is true reseve mt in vmp" );
								bChangeStyle = FALSE;
							}
							else 
							{
								//zhouyiliang 20100901 只有会控指定的vmpmember 才用clear，其他如发言人跟随等保留type
								if ( VMP_MEMBERTYPE_MCSSPEC == ptVmpMember->GetMemberType() )
								{
									tVMPParam.ClearVmpMember(byIndex);
								}
								else
								{
									ptVmpMember->SetMemberTMt(tMtNull);
								}
								bChangeNotify = TRUE;
							}
						}
						else//zhouyiliang 20100901 是动态（自动画面合成）
						{
							//zjj20091014
							//TMt tRemoveMt = bMemberInMcu ? (TMt)*ptVmpMember : tMt;
							// 不做循环调整,将ChangeVmpStyle移到for循环以外,bMemberInMcu是true时,上面已经对下级成员做了RemoveJoinedMt
							// 此处调ChangeVmpStyle传参*ptVmpMember没必要,只需要在for循环外将mcu做参数传入即可
							if( bChangeStyle )
							{
								//动态分屏改变风格
								//zhouyiliang 20100907 vcs自动画面合成当前可用通道号应该变
								//主席终端掉线时,不需要更新CurUseVMPChanInd,等待主席终端上线
								if ( VCS_CONF == m_tConf.GetConfSource() && tVMPParam.IsVMPAuto() 
									&& !(tMt == m_tConf.GetChairman()) )
								{
									
									wVcsCurChIndx--;
									if ( 0 == wVcsCurChIndx  ) 
									{
										wVcsCurChIndx =  1;
									}
									m_cVCSConfStatus.SetCurUseVMPChanInd( wVcsCurChIndx );
								}
								bChangeNotify = TRUE;
							}

						}
						
						//zjj20091014
						if( !bIsExceptionReason )
						{
							// zbq [04/25/2007] 如果该终端在会议模板的VMP信息里，直接删除之
							for( u8 byLoop = 0; byLoop < MAXNUM_MPU2VMP_MEMBER; byLoop ++ )
							{
								// 不考虑级联画面合成的情况
								if ( TYPE_MT == tMt.GetMtType() &&
									m_tConfEqpModule.m_tVmpModuleInfo.m_abyVmpMember[byLoop] == tMt.GetMtId() ) 
								{
									m_tConfEqpModule.m_tVmpModuleInfo.m_abyVmpMember[byLoop] = 0;
								}
							}
						}
					}
				}
			
				if( !IsDynamicVmp(byVmpId) )//zhouyiliang 20100901动态分屏的时候已经做了ChangeVmpStyle
				{
					g_cMcuVcApp.SetConfVmpParam(tVmpEqp, tVMPParam);
				}
				
				if( TRUE == bChangeNotify )
				{
					if (IsDynamicVmp(byVmpId))
					{
						if (bChangeVmpStyle)
						{
							//动态分屏在此处统一调整
							ChangeVmpStyle( tMt , FALSE );
							bChangeVmpStyle = FALSE;//自动vmp只做1次ChangeVmpStyle
						}
					}
					else
					{
						/*if (tVMPParam.GetVMPMemberNum() == 0
							&& !tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_DSTREAM)	//双流跟随时,不停vmp
							&& !tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_VMPCHLPOLL)	//单通道轮询时,也不停vmp
							&& !tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_POLL)
							&& !tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER)) // 2011-10-12 add by pgf: 如果还有跟随通道，则不停VMP
						{
							//停止交换数据,与界面统一，不再停止vmp
							ChangeVmpSwitch(byVmpId, VMP_STATE_STOP );
							
							//停止
							TPeriEqpStatus tPeriEqpStatus;
							if (!tVmpEqp.IsNull() &&
								g_cMcuVcApp.GetPeriEqpStatus(tVmpEqp.GetEqpId(), &tPeriEqpStatus) &&
								tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
								tPeriEqpStatus.m_tStatus.tVmp.m_byUseState != TVmpStatus::WAIT_STOP)
							{
								u8 byTimerVmpIdx = tVmpEqp.GetEqpId() - VMPID_MIN;
								SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER+byTimerVmpIdx, TIMESPACE_WAIT_VMPRSP);
								tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_STOP;
								g_cMcuVcApp.SetPeriEqpStatus(tVmpEqp.GetEqpId() , &tPeriEqpStatus);	
								
								CServMsg cTempServ;
								SendMsgToEqp(tVmpEqp.GetEqpId(), MCU_VMP_STOPVIDMIX_REQ, cTempServ); 
							}
						}
						else*/
						{
							// 针对轮询，在挂断当前轮询终端时，会立即轮询下一终端，若有跟随通道，此次不必调整，
							// 轮询到下一终端时还会调整，防止连续两次快速调整vmp
							if (CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode()
								&& m_tConf.m_tStatus.GetPollState() == POLL_STATE_NORMAL)
							{
								bMemberInMcu = !tMt.IsNull() && IsMcu(tMt) && IsMtInMcu(tMt, (TMt)m_tConf.m_tStatus.GetMtPollParam());//当前轮询终端是否在掉线mcu下
								if (CONF_POLLMODE_SPEAKER == m_tConf.m_tStatus.GetPollMode() ||
									CONF_POLLMODE_SPEAKER_BOTH == m_tConf.m_tStatus.GetPollMode())
								{
									if ((tMt == m_tConf.GetSpeaker() || bMemberInMcu) && 
										(tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_POLL)
										|| tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER)))
									{
										bChangeNotify = FALSE;
									}
								}
								else if (CONF_POLLMODE_VIDEO == m_tConf.m_tStatus.GetPollMode() ||
									CONF_POLLMODE_VIDEO_BOTH == m_tConf.m_tStatus.GetPollMode())
								{
									if ((tMt == (TMt)m_tConf.m_tStatus.GetMtPollParam() || bMemberInMcu) &&
										tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_POLL))
									{
										bChangeNotify = FALSE;
									}
								}
							}
							// 需要调整vmp时，调整vmp
							if (bChangeNotify)
							{
								AdjustVmpParam(byVmpId, &tVMPParam);
							}
						}
					}
				}
			}
		}
		ConfStatusChange();
	}

    //  4、 停多画面电视墙
	// [pengjie 2010/4/17] Fixme：多画面电视墙也不多回传，取local的Tmt信息
    if(m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE)
    {
        BOOL32 bStopVmpTw = TRUE;
        BOOL32 bChangeNotify = FALSE;
        TVMPParam tVMPParam = m_tConf.m_tStatus.GetVmpTwParam();
        for( u8 byLoop = 0; byLoop < MAXNUM_SDVMP_MEMBER; byLoop++  )
        {
            TMt tMembMt = (TMt)(*tVMPParam.GetVmpMember(byLoop));
            TMt tTempLocalMt = GetLocalMtFromOtherMcuMt( tMembMt );
            if( tTempLocalMt.GetMtId() == tMt.GetMtId() )
            {
                TVMPMember tVmpMember = *tVMPParam.GetVmpMember(byLoop);

                //画面合成成员类型为“会控指定”时进行状态清理，删除其本身。其他画面
                //合成成员类型在此不作处理，防止误清除了相关的跟随通道。[12/04/2006-zbq]
                if ( VMPTW_MEMBERTYPE_MCSSPEC == tVmpMember.GetMemberType() ) 
                {
                    StopSwitchToPeriEqp( m_tVmpTwEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO );
                    
                    TVMPMember tVMPMember;
                    memset( &tVMPMember, 0, sizeof(TVMPMember) );
					//zjj20091014
					if( bIsExceptionReason )
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true reseve mt in vmpTW" );						
					}
					else
					{
						tVMPParam.SetVmpMember( byLoop, tVMPMember );
					}
                    bChangeNotify = TRUE;
                }
            }
            
            TVMPMember tVmpMember = *tVMPParam.GetVmpMember(byLoop);            
            if (tVmpMember.GetMemberType() != 0 || !tVmpMember.IsNull())
            {
                bStopVmpTw = FALSE;
            }
        }
        m_tConf.m_tStatus.SetVmpTwParam( tVMPParam );               
                
        if( TRUE == bChangeNotify )
        {
            TPeriEqpStatus tVmpEqpStatus;
            g_cMcuVcApp.GetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tVmpEqpStatus);
            
            if (bStopVmpTw)
            {
                tVmpEqpStatus.m_tStatus.tVmp.m_byUseState = FALSE;
                m_tConf.m_tStatus.SetVmpTwMode(CONF_VMPTWMODE_NONE);
            }
            
            tVmpEqpStatus.m_tStatus.tVmp.SetVmpParam(m_tConf.m_tStatus.m_tVmpTwParam);
            g_cMcuVcApp.SetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tVmpEqpStatus);
            
            if (bStopVmpTw)
            {
                //停止交换数据
                ChangeVmpTwSwitch( VMPTW_STATE_STOP );
                
                //停止
                CServMsg cTempServ;
                SendMsgToEqp(m_tVmpTwEqp.GetEqpId(), MCU_VMPTW_STOPVIDMIX_REQ, cTempServ);         
            }
            else
            {
                ChangeVmpTwParam(&tVMPParam);
            }
        }   
        
        ConfStatusChange();
    }

	TMtStatus tMtStatus;
  // 5、停止TvWall
    u8 byEqpId = TVWALLID_MIN;
	TPeriEqpStatus tTvwallStatus;
	TMt tTmpMt;
	tTmpMt.SetNull();
    while( byEqpId >= TVWALLID_MIN && byEqpId <= TVWALLID_MAX )
	{
		if (g_cMcuVcApp.IsPeriEqpValid(byEqpId))
		{
			g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTvwallStatus);
            if (tTvwallStatus.m_byOnline == 1)
            {
                u8 byMemberNum = tTvwallStatus.m_tStatus.tTvWall.byChnnlNum;
				for(u8 byLoop = 0; byLoop < min(byMemberNum,MAXNUM_PERIEQP_CHNNL); byLoop++)
				{
					tTmpMt = tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop];
					const u8 byOrigMmbType = tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType;

                    //20100708_tzy 将相关条件判断进行整合，按照原来逻辑，如果开启轮询跟随进电视墙，此时挂断并不处于轮询状态的终端，
                    //那么电视墙也会自动跳到下一个终端去
					if (tMt == tTmpMt || (IsMcu(tMt) && IsMtInMcu(tMt,tTmpMt))
						&& m_byConfIdx == tTmpMt.GetConfIdx())
					{
						if (TW_MEMBERTYPE_POLL == byOrigMmbType)
						{
							TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
							
							ChangeTvWallSwitch( NULL, byEqpId, byLoop, TW_MEMBERTYPE_POLL, TW_STATE_CHANGE );
							g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTvwallStatus);
							tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = byOrigMmbType;
							tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(m_byConfIdx);
        
							//zjj20091014
							if( bIsExceptionReason )
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true(TW_MEMBERTYPE_POLL) reseve mt in TVWall" );						
							}
						}
						else if ( TW_MEMBERTYPE_TWPOLL == byOrigMmbType )
						{
							// 立即开始新的轮询
							u32 dwTimerIdx = 0;
							if( m_tTWMutiPollParam.GetTimerIdx(byEqpId, byLoop, dwTimerIdx) )
							{
								SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx,100);
							}
						}else if ( TW_MEMBERTYPE_CHAIRMAN == byOrigMmbType 
							|| TW_MEMBERTYPE_SPEAKER == byOrigMmbType )
						{
							ChangeTvWallSwitch(NULL, byEqpId, byLoop, byOrigMmbType, TW_STATE_CHANGE);
							g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTvwallStatus);
							tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = byOrigMmbType;
							tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(m_byConfIdx);
						}
						// [9/26/2011 liuxu] Vcs单方调度下终端掉线, 需要清空电视墙
						else if ( VCS_CONF == m_tConf.GetConfSource() 
							&& m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE
							&& TW_MEMBERTYPE_VCSSPEC == tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType )
						{
							ChangeTvWallSwitch(NULL,  byEqpId, byLoop, TW_MEMBERTYPE_VCSSPEC, TW_STATE_STOP);
							g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTvwallStatus);
						}
						else
						{
							StopSwitchToPeriEqp(byEqpId, byLoop);

							//zjj20091014
							if( bIsExceptionReason )
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true reseve mt in TVWall" );						
							}
							else
							{
								//20101111_tzy VCS会议预览模式下电视墙不变化
								if ( (VCS_CONF == m_tConf.GetConfSource() 
									&& m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_REVIEW_MODE))
								{
									tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = byOrigMmbType;
									tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetMt(tTmpMt);
									tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(m_byConfIdx);
								}
								else
								{
									tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = TW_MEMBERTYPE_NULL;
									tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
									tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(0);
									GetMtStatus(tMt, tMtStatus);
									tMtStatus.SetInTvWall(FALSE);
									SetMtStatus(tMt, tMtStatus);
									MtStatusChange(&tMt, TRUE);
								}
							}
						}
					}

					//20101206_tzy 只要挂断的是mcu，则不再需要恢复
					if (IsMcu(tMt) && IsMtInMcu(tMt,tTmpMt))
					{
						tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = 0;
						tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
						tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(0);
					}
				}

                g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tTvwallStatus);
                cServMsg.SetMsgBody((u8 *)&tTvwallStatus, sizeof(tTvwallStatus));
                SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);

				ConfStatusChange();
            }
		}

        byEqpId++;
	}

    // 6、停止hdu
    u8 byHduId = HDUID_MIN;
	TPeriEqpStatus tHduStatus;

	//20101130_tzy Bug00042441R6B2:三级多回传，自动电视墙模式下，调度各级终端进电视墙，重启下级MCU后，所有与电视墙的交换都被拆掉
	//将该标志位移到里面否则可能某些非本级终端也被踢出电视墙
	//BOOL32 bIsRemoveMtInSMcu = FALSE;
    while( byHduId >= HDUID_MIN && byHduId <= HDUID_MAX )
	{
		if (g_cMcuVcApp.IsPeriEqpValid(byHduId))
		{
			g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
            if (tHduStatus.m_byOnline == 1)
            {
                u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byHduId));
				if (0 == byHduChnNum)
				{
					byHduId++;
					ConfPrint(LOG_LVL_WARNING, MID_MCU_HDU, "[RemoveMtFormPeriInfo] GetHduChnNumAcd2Eqp failed!\n");
					continue;
				}
				for(u8 byLoop = 0; byLoop < min(byHduChnNum,MAXNUM_HDU_CHANNEL); byLoop++)
				{
					// [2013/03/11 chenbing]  
					if ( HDUCHN_MODE_FOUR == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byLoop) )
					{
						for (u8 byIndex=0; byIndex<HDU_MODEFOUR_MAX_SUBCHNNUM; byIndex++)
						{
							tTmpMt = tHduStatus.m_tStatus.tHdu.GetChnMt(byLoop, byIndex);
							if( tTmpMt.IsNull() || tTmpMt.GetConfIdx() != m_byConfIdx )
							{
								continue;
							}

							BOOL32 bIsRemoveMtInSMcu = FALSE;
							// 挂断的是mcu，其下的终端如有在电视墙中的要清掉
							if( !tMt.IsNull() && IsMcu(tMt) && IsMtInMcu(tMt, tTmpMt) )
							{
								bIsRemoveMtInSMcu = TRUE;
							}

							if ( (tMt == tTmpMt || bIsRemoveMtInSMcu ) 
								&& m_byConfIdx == tTmpMt.GetConfIdx()
							   )
							{
								// 停止到子通道的交换
								//StopSwitchToPeriEqp(byHduId, byLoop, TRUE, MODE_VIDEO, SWITCH_MODE_BROADCAST, byIndex);
								//Bug00161975:级联会议中下级MCU进电视墙画面合成通道，挂断下级会议后在上传通道中的终端分辨率不恢复
								//yrl20131119:HDU四分格不单单要拆交换，还有分辨率调整
								const u8 byMemberType = tHduStatus.m_tStatus.tHdu.GetMemberType(byLoop, byIndex);
								ChangeHduSwitch( NULL, byHduId, byLoop, byIndex, byMemberType, TW_STATE_STOP );
	
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP,"[RemoveMtFormPeriInfo] remove mt(%d)(%d) from Hdu(%d)(%d)!\n",
										tTmpMt.GetMcuId(),tTmpMt.GetMtId() ,byHduId,byLoop);
									
								if( bIsExceptionReason )
								{
									ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true reseve mt in HDU" );	
								}
								else
								{
									tHduStatus.m_tStatus.tHdu.SetMemberType(TW_MEMBERTYPE_NULL, byLoop, byIndex);
									tHduStatus.m_tStatus.tHdu.SetChnNull(byLoop, byIndex);
									tHduStatus.m_tStatus.tHdu.SetSchemeIdx(0, byLoop, byIndex);
									GetMtStatus(tMt, tMtStatus);
									tMtStatus.SetInHdu(FALSE);
									SetMtStatus(tMt, tMtStatus);
									MtStatusChange(&tMt, TRUE);
								}
							}

							//只要挂断的是mcu，则不再需要恢复
							if (bIsRemoveMtInSMcu)
							{
								tHduStatus.m_tStatus.tHdu.SetMemberType(TW_MEMBERTYPE_NULL, byLoop, byIndex);
								tHduStatus.m_tStatus.tHdu.SetChnNull(byLoop, byIndex);
								tHduStatus.m_tStatus.tHdu.SetSchemeIdx(0, byLoop, byIndex);
							}
						}
					}
					else
					{
						tTmpMt = (TMt)tHduStatus.m_tStatus.tHdu.GetChnMt(byLoop);
						const u8 byOrigMmbType = tHduStatus.m_tStatus.tHdu.GetMemberType(byLoop);

						if( tTmpMt.IsNull() || tTmpMt.GetConfIdx() != m_byConfIdx )
						{
							continue;
						}

						BOOL32 bIsRemoveMtInSMcu = FALSE;
						// [pengjie 2010/9/3] 挂断的是mcu，其下的终端如有在电视墙中的要清掉
						if( !tMt.IsNull() && IsMcu(tMt) && IsMtInMcu(tMt, tTmpMt) )
						{
							bIsRemoveMtInSMcu = TRUE;
						}

						//20100708_tzy 将相关条件判断进行整合，按照原来逻辑，如果开启轮询跟随进电视墙，此时挂断并不处于轮询状态的终端，
						//那么电视墙也会自动跳到下一个终端去
						if ( (tMt == tTmpMt
								|| ( IsMcu(tTmpMt) && GetSMcuViewMt(tTmpMt, TRUE) == tMt )
								|| bIsRemoveMtInSMcu 
								|| (byOrigMmbType == TW_MEMBERTYPE_DOUBLESTREAM && GetLocalMtFromOtherMcuMt(tMt) == tTmpMt) ) 
							&& m_byConfIdx == tTmpMt.GetConfIdx() )
						{
							if (TW_MEMBERTYPE_POLL == byOrigMmbType)
							{
								// [2013/03/11 chenbing] HDU多画面目前不支持轮询跟随,子通道置0
								ChangeHduSwitch( NULL, byHduId, byLoop, 0, TW_MEMBERTYPE_POLL, TW_STATE_STOP );

								// 重取通道状态
								g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
								tHduStatus.m_tStatus.tHdu.SetMemberType(TW_MEMBERTYPE_POLL, byLoop);
								tHduStatus.m_tStatus.tHdu.SetConfIdx(m_byConfIdx, byLoop);
                        
								//zjj20091014
								if( bIsExceptionReason )
								{
									ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true(TW_MEMBERTYPE_POLL) reseve mt in HDU" );						
								}
							} 
							else if (TW_MEMBERTYPE_SPEAKER == byOrigMmbType)
							{
								// [pengjie 2010/4/17] 发言人一样不能用被local化的
								TMt tSpeaker = m_tConf.GetSpeaker();
								// [2013/03/11 chenbing] HDU多画面目前不支持发言人跟随,子通道置0
								ChangeHduSwitch( NULL, byHduId, byLoop, 0, TW_MEMBERTYPE_SPEAKER, TW_STATE_STOP );

								// 重取通道状态
								g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
								tHduStatus.m_tStatus.tHdu.SetMemberType(byOrigMmbType, byLoop);
								tHduStatus.m_tStatus.tHdu.SetConfIdx(m_byConfIdx, byLoop);

								//zjj20091014
								if( bIsExceptionReason )
								{
									ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true(TW_MEMBERTYPE_SPEAKER) reseve mt in HDU" );						
								}
							} 
							else if (TW_MEMBERTYPE_CHAIRMAN == byOrigMmbType)
							{
								// [pengjie 2010/4/17] 主席不能是下级终端所以取local的Tmt信息
								TMt tLocalSpeaker = m_tConf.GetChairman();
								tLocalSpeaker = GetLocalMtFromOtherMcuMt( tLocalSpeaker );
								// [2013/03/11 chenbing] HDU多画面目前不支持主席跟随,子通道置0
								ChangeHduSwitch( NULL, byHduId, byLoop, 0, TW_MEMBERTYPE_CHAIRMAN, TW_STATE_STOP);

								// 重取通道状态
								g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
								tHduStatus.m_tStatus.tHdu.SetMemberType(byOrigMmbType, byLoop);
								tHduStatus.m_tStatus.tHdu.SetConfIdx(m_byConfIdx, byLoop);

								if( bIsExceptionReason )
								{
									ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true(TW_MEMBERTYPE_CHAIRMAN) reseve mt in HDU" );						
								}
							}
							else if ( TW_MEMBERTYPE_DOUBLESTREAM == byOrigMmbType )
							{
								// [pengjie 2010/9/9] 在这里不停，等下级mcu发关闭双流逻辑通道时统一处理
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  
									"[RemoveMtFormPeriInfo] Wait Mt.%d come to CloseDulChl then remove it from Hdu!\n",
									tTmpMt.GetMtId() );
							}
							else if ( TW_MEMBERTYPE_TWPOLL == byOrigMmbType )
							{
								u32 dwTimerIdx = 0;
								if( m_tTWMutiPollParam.GetTimerIdx(byHduId, byLoop, dwTimerIdx) )
								{
									SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx,100);
								}
							}
							// [9/26/2011 liuxu] Vcs单方调度下终端掉线, 需要清空电视墙
							else if ( VCS_CONF == m_tConf.GetConfSource() 
								&& m_cVCSConfStatus.GetCurVCMode() == VCS_SINGLE_MODE
								&& TW_MEMBERTYPE_VCSSPEC == byOrigMmbType )
							{
								// [2013/03/11 chenbing] VCS会议不支持HDU多画面,子通道置0
								ChangeHduSwitch(NULL,  byHduId, byLoop, 0, TW_MEMBERTYPE_VCSSPEC, TW_STATE_STOP);
								g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
							}
							else
							{
								StopSwitchToPeriEqp(byHduId, byLoop);

								ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP,"[RemoveMtFormPeriInfo] remove mt(%d)(%d) from Hdu(%d)(%d)!\n",
									tTmpMt.GetMcuId(),tTmpMt.GetMtId() ,byHduId,byLoop);
								
								//zjj20091014
								if( bIsExceptionReason )
								{
									ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true reseve mt in HDU" );	
								}
								else
								{
									//20101111_tzy VCS会议预览模式下电视墙不变化
									//20110420 批量轮询状态下也不用清状态，需求如此
									if ( POLL_STATE_NONE != m_tHduBatchPollInfo.GetStatus() 
										|| (VCS_CONF == m_tConf.GetConfSource() && m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_REVIEW_MODE))
									{
										tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType = byOrigMmbType;
										tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetMt(tTmpMt);
										tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(m_byConfIdx);
										if (MTLEFT_REASON_DELETE == byReason)
										{
											//正常删除下需要清除状态
											GetMtStatus(tMt, tMtStatus);
											tMtStatus.SetInHdu(FALSE);
											SetMtStatus(tMt, tMtStatus);
											MtStatusChange(&tMt, TRUE);
										}
									}
									else
									{
										tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType = TW_MEMBERTYPE_NULL;
										tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
										tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
										tHduStatus.m_tStatus.tHdu.atHduChnStatus[byLoop].SetSchemeIdx(0);
										GetMtStatus(tMt, tMtStatus);
										tMtStatus.SetInHdu(FALSE);
										SetMtStatus(tMt, tMtStatus);
										MtStatusChange(&tMt, TRUE);
									}
								}
							}
						}
						//20101206_tzy 只要挂断的是mcu，则不再需要恢复
						if (bIsRemoveMtInSMcu)
						{

							tHduStatus.m_tStatus.tHdu.SetMemberType(0, byLoop);
							tHduStatus.m_tStatus.tHdu.SetChnNull(byLoop);
							tHduStatus.m_tStatus.tHdu.SetConfIdx(0, byLoop);
						}
					}
				}

                g_cMcuVcApp.SetPeriEqpStatus(byHduId, &tHduStatus);
                cServMsg.SetMsgBody((u8 *)&tHduStatus, sizeof(tHduStatus));
                SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
            }
			else/*HDU不在线也要清除mt*/
			{
				for(u8 byLoop = 0; byLoop < MAXNUM_HDU_CHANNEL; byLoop++)
				{
					// 四风格  
					if ( HDUCHN_MODE_FOUR == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byLoop) )
					{
						for (u8 byIndex=0; byIndex<HDU_MODEFOUR_MAX_SUBCHNNUM; byIndex++)
						{
							tTmpMt = tHduStatus.m_tStatus.tHdu.GetChnMt(byLoop, byIndex);
							if( tTmpMt.IsNull() || tTmpMt.GetConfIdx() != m_byConfIdx )
							{
								continue;
							}

							BOOL32 bIsRemoveMtInSMcu = FALSE;
							// 挂断的是mcu，其下的终端如有在电视墙中的要清掉
							if( !tMt.IsNull() && IsMcu(tMt) && IsMtInMcu(tMt, tTmpMt) )
							{
								bIsRemoveMtInSMcu = TRUE;
							}

							if ( (tMt == tTmpMt || bIsRemoveMtInSMcu ) 
								&& m_byConfIdx == tTmpMt.GetConfIdx()
							   )
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP,"[RemoveMtFormPeriInfo] remove mt(%d)(%d) from Hdu(%d)(%d)!\n",
										tTmpMt.GetMcuId(),tTmpMt.GetMtId() ,byHduId,byLoop);
									
								if( bIsExceptionReason )
								{
									ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true reseve mt in HDU" );	
								}
								else
								{
									tHduStatus.m_tStatus.tHdu.SetMemberType(TW_MEMBERTYPE_NULL, byLoop, byIndex);
									tHduStatus.m_tStatus.tHdu.SetChnNull(byLoop, byIndex);
									tHduStatus.m_tStatus.tHdu.SetSchemeIdx(0, byLoop, byIndex);
									if (byIndex)
									{
										tHduStatus.m_tStatus.tHdu.SetConfIdx(0, byLoop, byIndex);
									}
									GetMtStatus(tMt, tMtStatus);
									tMtStatus.SetInHdu(FALSE);
									SetMtStatus(tMt, tMtStatus);
									MtStatusChange(&tMt, TRUE);
								}
							}

							//只要挂断的是mcu，则不再需要恢复
							if (bIsRemoveMtInSMcu)
							{
								tHduStatus.m_tStatus.tHdu.SetMemberType(TW_MEMBERTYPE_NULL, byLoop, byIndex);
								tHduStatus.m_tStatus.tHdu.SetChnNull(byLoop, byIndex);
								tHduStatus.m_tStatus.tHdu.SetSchemeIdx(0, byLoop, byIndex);
							}
						}
					}
					else
					{
						tTmpMt = (TMt)tHduStatus.m_tStatus.tHdu.GetChnMt(byLoop);
						const u8 byOrigMmbType = tHduStatus.m_tStatus.tHdu.GetMemberType(byLoop);

						if( tTmpMt.IsNull() || tTmpMt.GetConfIdx() != m_byConfIdx )
						{
							continue;
						}

						BOOL32 bIsRemoveMtInSMcu = FALSE;
						// [pengjie 2010/9/3] 挂断的是mcu，其下的终端如有在电视墙中的要清掉
						if( !tMt.IsNull() && IsMcu(tMt) && IsMtInMcu(tMt, tTmpMt) )
						{
							bIsRemoveMtInSMcu = TRUE;
						}

						//20100708_tzy 将相关条件判断进行整合，按照原来逻辑，如果开启轮询跟随进电视墙，此时挂断并不处于轮询状态的终端，
						//那么电视墙也会自动跳到下一个终端去
						if ( (tMt == tTmpMt
							 || bIsRemoveMtInSMcu 
							 || ( IsMcu(tTmpMt) && GetSMcuViewMt(tTmpMt, TRUE) == tMt ))
							&& m_byConfIdx == tTmpMt.GetConfIdx() )
						{
							ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP,"[RemoveMtFormPeriInfo] remove mt(%d)(%d) from Hdu(%d)(%d)!\n",
									tTmpMt.GetMcuId(),tTmpMt.GetMtId() ,byHduId,byLoop);
								
								//zjj20091014
							if( bIsExceptionReason )
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL,  "[RemoveJoinedMt] byIsExceptionReason is true reseve mt in HDU" );	
							}
							else
							{
								u8 byMeMberType = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType;
								if ( TW_MEMBERTYPE_SPEAKER == byMeMberType
									|| TW_MEMBERTYPE_CHAIRMAN == byMeMberType
									|| TW_MEMBERTYPE_POLL == byMeMberType)
								{
									tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
									tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
								}
								else
								{
									tHduStatus.m_tStatus.tHdu.atHduChnStatus[byLoop].SetSchemeIdx(0);
									tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType = TW_MEMBERTYPE_NULL;
									tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
									tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
								}
								
								g_cMcuVcApp.SetChnnlMMode(byHduId, byLoop, MODE_NONE);
								GetMtStatus(tMt, tMtStatus);
								tMtStatus.SetInHdu(FALSE);
								SetMtStatus(tMt, tMtStatus);
								MtStatusChange(&tMt, TRUE);
							}
						}
						//20101206_tzy 只要挂断的是mcu，则不再需要恢复
						if (bIsRemoveMtInSMcu)
						{

							tHduStatus.m_tStatus.tHdu.SetMemberType(0, byLoop);
							tHduStatus.m_tStatus.tHdu.SetChnNull(byLoop);
							tHduStatus.m_tStatus.tHdu.SetConfIdx(0, byLoop);
						}
					}
					g_cMcuVcApp.SetPeriEqpStatus(byHduId, &tHduStatus);
				}
			}
		}
	
        byHduId++;
	}

	// [11/11/2011 liuxu] 应该由各业务自己控制, 这里不应该再画蛇添足
// 	TMtStatus tMtStatus;
// 	GetMtStatus(tMt, tMtStatus);
// 
// 	SetMtStatus(tMt, tMtStatus);

	//[03/04/2010]	zjl modify (电视墙多通道轮询代码合并)
	//电视墙轮询参数更改
	u32 dwTwTimerIdx = 0;
	for (u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL; byIdx++)
	{
		//要移除终端在轮询列表中的位置
		u8 byTmtPos = ~0;
		u8 byCurPollPos = m_tTWMutiPollParam.m_tTWPollParam[byIdx].GetCurrentIdx();
		//处于轮询非停止状态并且该终端在轮询列表中
		if (POLL_STATE_NONE != m_tTWMutiPollParam.m_tTWPollParam[byIdx].GetPollState() && 
			m_tTWMutiPollParam.m_tTWPollParam[byIdx].IsExistMt(tMt,byTmtPos)&&
			m_tTWMutiPollParam.GetTimerIdx(m_tTWMutiPollParam.m_tTWPollParam[byIdx].GetTvWall().GetEqpId(),
			m_tTWMutiPollParam.m_tTWPollParam[byIdx].GetTWChnnl(), dwTwTimerIdx))
		{			
			//20100701_tzy 轮询从轮询列表中移除,是否要明确哪些原因应该移出轮询列表，哪些原因不移出，
			//因为各个流上不太一致
			//m_tTWMutiPollParam.m_tTWPollParam[byIdx].RemoveMtFromList(tMt);
			//删除的终端为当前轮询终端，则暂停一下，以便下次轮训时从当前位置开始
			if (byCurPollPos == byTmtPos)
			{
				m_tTWMutiPollParam.m_tTWPollParam[byIdx].SetIsStartAsPause(TRUE);
				//如果处于轮询正常状态，则立刻执行对下个终端的轮询
				if (m_tTWMutiPollParam.m_tTWPollParam[byIdx].GetPollState() == POLL_STATE_NORMAL)
				{
					SetTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTwTimerIdx, 1000*1, dwTwTimerIdx);
				}
			}
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[RemoveJoinedMt]adjust TW and hdu poll param TW<EqpId:%d, ChnId:%d> Current PollChnState %d!\n",
					m_tTWMutiPollParam.m_tTWPollParam[byIdx].GetTvWall().GetEqpId(),
					m_tTWMutiPollParam.m_tTWPollParam[byIdx].GetTWChnnl(),m_tTWMutiPollParam.m_tTWPollParam[byIdx].GetPollState());
		}		
	}

	// 7、停止监控
	AdjustSwitchToAllMcWatchingSrcMt( tMt );
	AdjustSwitchToMonitorWatchingSrc( tMt );	//监控联动

	// 8、停止终端录像
	RemoveMtFromRec(&tMt);
	MtStatusChange(&tMt, TRUE);
	return;
}

/*=============================================================================
函 数 名： RemoveMtFormPeriInfo
功    能： 删除不在线终端时,清除外设中信息
		   终端异常掉线时,会保留外设信息等待上线恢复,
		   会控删除异常掉线终端时,需要清除外设中信息,保证信息正确
算法实现： 
全局变量： 
参    数： TMt tMt  要删除的终端
返 回 值： void
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20120712    4.7		  yanghuaizhi              create
=============================================================================*/
void CMcuVcInst::RemoveUnjoinedMtFormPeriInfo( const TMt &tMt)
{
	// 终端为空
	if (tMt.IsNull() || !tMt.IsLocal())
	{
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[RemoveUnjoinedMtFormPeriInfo]Mt(%d,%d) is none or not local!\n",
			tMt.GetMcuId(), tMt.GetMtId());
		return;
	}

	// 终端在线
	if (m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId()))
	{
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[RemoveUnjoinedMtFormPeriInfo]Mt(%d,%d) is Joined Conf!\n",
			tMt.GetMcuId(), tMt.GetMtId());
		return;
	}
	
	// 若终端在vmp中,清除vmp成员,只针对会控指定模式
	// 终端异常掉线时,会保留vmp通道成员,等终端上线时会恢复,此处为针对此场景清除vmp成员
	// 删除不在线的终端,此时的终端未建到vmp的交换,只需要将vmpparam的成员信息清除即可
	// 可能是最后一个终端,清除后需要停vmp
	if (GetVmpCountInVmpList() > 0)
	{
		TEqp tVmpEqp;
		BOOL32 bChangeNotify;
		TVMPParam_25Mem tVMPParam;
		TVMPMember* ptVmpMember;
		u8 byMaxNum;
		TMt tMtNull;
		tMtNull.SetNull();
		TVMPParam_25Mem tLastVmpParam;
		TPeriEqpStatus tPeriEqpStatus;
		//u8 byVmpIdx;
		CServMsg cTempServ;
		for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
		{
			if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
			{
				continue;
			}
			tVmpEqp = g_cMcuVcApp.GetEqp( m_abyVmpEqpId[byIdx] );
			
			if( g_cMcuVcApp.GetVMPMode(tVmpEqp) == CONF_VMPMODE_CTRL)
			{
				bChangeNotify = FALSE;
				tVMPParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);//m_tConf.m_tStatus.GetVmpParam();
				ptVmpMember = NULL;
				byMaxNum = tVMPParam.GetMaxMemberNum();
				
				tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tVmpEqp);
				for (u8 byIndex = 0; byIndex < byMaxNum ; ++byIndex)
				{
					ptVmpMember = tVMPParam.GetVmpMember(byIndex);
					if (ptVmpMember == NULL || ptVmpMember->IsNull())
					{
						continue;
					}
					
					if (*ptVmpMember == tMt)
					{
						bChangeNotify = TRUE;
						// 只有会控指定的vmpmember 才用clear，其他如发言人跟随等保留type
						if ( VMP_MEMBERTYPE_MCSSPEC == ptVmpMember->GetMemberType() )
						{
							tVMPParam.ClearVmpMember(byIndex);
						}
						else
						{
							ptVmpMember->SetMemberTMt(tMtNull);
						}
						tLastVmpParam.SetVmpMember(byIndex, *ptVmpMember);
					}
				}
				g_cMcuVcApp.SetLastVmpParam(tVmpEqp, tLastVmpParam);
				
				if (TRUE == bChangeNotify &&
					g_cMcuVcApp.GetPeriEqpStatus(tVmpEqp.GetEqpId(), &tPeriEqpStatus))
				{
					//m_tConf.m_tStatus.SetVmpParam( tVMPParam );
					g_cMcuVcApp.SetConfVmpParam(tVmpEqp, tVMPParam);
					/*if (tVMPParam.GetVMPMemberNum() == 0
						&& !tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_DSTREAM)
						&& !tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_VMPCHLPOLL)
						&& !tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_POLL)
						&& !tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER))
					{
						//停止vmp,与界面统一，不再停止vmp
						if (!tVmpEqp.IsNull() &&
							tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
							tPeriEqpStatus.m_tStatus.tVmp.m_byUseState != TVmpStatus::WAIT_STOP)
						{
							byVmpIdx = tVmpEqp.GetEqpId() - VMPID_MIN;
							SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER+byVmpIdx, TIMESPACE_WAIT_VMPRSP);
							tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_STOP;
							g_cMcuVcApp.SetPeriEqpStatus(tVmpEqp.GetEqpId() , &tPeriEqpStatus);	
							
							SendMsgToEqp(tVmpEqp.GetEqpId(), MCU_VMP_STOPVIDMIX_REQ, cTempServ); 
						}
					}
					else*/
					{
						tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tVMPParam);
						g_cMcuVcApp.SetPeriEqpStatus(tVmpEqp.GetEqpId() , &tPeriEqpStatus);
						// 刷新状态
						ConfStatusChange();
					}
				}
			}
		}
	}

	//删除其在会议模板中电视墙成员的信息
	RemoveMtInMutiTvWallMoudle(tMt);
	return;
}

/*=============================================================================
    函 数 名： ProcVcsApplyCancelSpeakerReq
    功    能： VCS会议MTC-BOX发言人申请/取消消息处理
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： void
 -----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2010/05/13  4.6			张宝卿                创建
=============================================================================*/
void CMcuVcInst::ProcVcsApplyCancelSpeakerReq(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	TMt tMt;
	TMsgHeadMsg tHeadMsg;
	BOOL32 bIsApplySpeaker = FALSE;
	TMt tNormalMt;
	
	if( MT_MCU_APPLYCANCELSPEAKER_REQ == cServMsg.GetEventId() )
	{
		//SendReplyBack(cServMsg, pcMsg->event + 1);
		
		memset( &tHeadMsg,0,sizeof(tHeadMsg) );
		
		tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
		bIsApplySpeaker = (*(u8*)cServMsg.GetMsgBody()) == 1;
	}
	else if( MCU_MCU_CANCELMESPEAKER_REQ == cServMsg.GetEventId() )
	{
		tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
		tMt = *(TMt*)( cServMsg.GetMsgBody()+sizeof(TMsgHeadMsg) );
		tMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc,tMt );
	}
	else if (pcMsg->event == MCU_MCU_APPLYSPEAKER_REQ)
    {
		tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();	
        tMt = *(TMt*)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );
		tMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc,tMt );
		bIsApplySpeaker = TRUE;
    }
	
	if( VCS_CONF != m_tConf.GetConfSource() ||
		( !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
		m_byCreateBy != CONF_CREATE_MT )
		)
	{
		//zjj20101126 单方调度模式下,除调度终端是发言状态,其它终端都不可申请发言,且要明确回拒绝(否则申请灯会闪)
		//多方多画面在电视墙的终端必在画面合成且让MTC-BOX发言灯亮,所以不必处理
		if( VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode() )
		{
			if( !(tMt == m_cVCSConfStatus.GetCurVCMT()) &&
				( m_ptMtTable->IsMtInHdu(tMt.GetMtId()) || m_ptMtTable->IsMtInTvWall(tMt.GetMtId()) )
				)
			{
				NotifyMtSpeakStatus( tMt, emDenid );
			}
		}
		return;
	}
	
	if ( !m_tCascadeMMCU.IsNull() )
    {        
		if( bIsApplySpeaker )
		{
			if( MT_MCU_APPLYCANCELSPEAKER_REQ == cServMsg.GetEventId() )
			{
				cServMsg.SetEventId( MCU_MCU_APPLYSPEAKER_REQ );
				cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
				tMt = GetMcuIdMtFromMcuIdxMt( tMt );
				cServMsg.CatMsgBody( (u8*)&tMt,sizeof(tMt) );
			}
			SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_APPLYSPEAKER_REQ, cServMsg);	
		}
		else
		{			
			if( MT_MCU_APPLYCANCELSPEAKER_REQ == cServMsg.GetEventId() )
			{
				cServMsg.SetEventId( MCU_MCU_CANCELMESPEAKER_REQ );
				cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
				tMt = GetMcuIdMtFromMcuIdxMt( tMt );
				cServMsg.CatMsgBody( (u8*)&tMt,sizeof(tMt) );
			}			
			SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_CANCELMESPEAKER_REQ, cServMsg);
		}
        return;
    }
	
    //1.取消本级
    if( bIsApplySpeaker )
	{
		if( !m_tConfAllMtInfo.MtJoinedConf( tMt ) )
		{
			if (cServMsg.GetEventId() == MCU_MCU_APPLYSPEAKER_REQ)
			{
				memset( &tHeadMsg,0,sizeof(tHeadMsg) );							
				tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tNormalMt );
				cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
				cServMsg.CatMsgBody( (u8*)&tNormalMt,sizeof(tNormalMt) );
				cServMsg.SetEventId(pcMsg->event + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
			}
			
			return;
		}
		
		BOOL32 bIsSendToChairman = FALSE;
		if (CONF_SPEAKMODE_ANSWERINSTANTLY == m_tConf.GetConfSpeakMode())
		{
			if( m_tConf.GetSpeaker() == tMt )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS,  "[ProcVcsApplyCancelSpeakerReq] tMt(%d.%d) has already speaker,so not handle!\n",
					tMt.GetMcuId(),tMt.GetMtId()
					);					
				return;
			}
			//如果当前发言人是主席,抢发言人
			if ( m_tConf.GetChairman() == m_tConf.GetSpeaker() )
			{
				if (cServMsg.GetEventId() == MCU_MCU_APPLYSPEAKER_REQ)
				{
					memset( &tHeadMsg,0,sizeof(tHeadMsg) );							
					tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tNormalMt );
					cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
					cServMsg.CatMsgBody( (u8*)&tNormalMt,sizeof(tNormalMt) );						
				}						
// 				cServMsg.SetEventId(pcMsg->event + 1);							
// 				SendReplyBack(cServMsg, cServMsg.GetEventId());
				
				ChgCurVCMT( tMt );

                if ( !(m_tConf.GetSpeaker() == tMt) )
                {//抢答模式申请发言失败
                    cServMsg.SetEventId(pcMsg->event + 2);							
				    SendReplyBack(cServMsg, cServMsg.GetEventId());
                    if (tMt.IsLocal())
                    {
                        NotifyMtSpeakStatus(tMt, emDenid);
                    }
                }
                else
                {//抢答模式申请发言成功
                    cServMsg.SetEventId(pcMsg->event + 1);							
				    SendReplyBack(cServMsg, cServMsg.GetEventId());
                    if (tMt.IsLocal())
                    {
                        NotifyMtSpeakStatus(tMt, emAgreed);
                    }
                }
				
				//通知会控
				//cServMsg.SetMsgBody((u8*)&tMt, sizeof(tMt));						
				//SendMsgToAllMcs( MCU_MCS_MTAPPLYSPEAKER_NOTIF, cServMsg );						
			}
			else
			{
				if (!m_tApplySpeakQue.IsQueueFull())
				{
					if (cServMsg.GetEventId() == MCU_MCU_APPLYSPEAKER_REQ)
					{
						memset( &tHeadMsg,0,sizeof(tHeadMsg) );							
						tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tNormalMt );
						cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
						cServMsg.CatMsgBody( (u8*)&tNormalMt,sizeof(tNormalMt) );							
					}
					cServMsg.SetEventId(pcMsg->event + 1);
					SendReplyBack(cServMsg, cServMsg.GetEventId());
					
					m_tApplySpeakQue.ProcQueueInfo(tMt,bIsSendToChairman);
					NotifyMcsApplyList( bIsSendToChairman );
				}
				else
				{
					if (cServMsg.GetEventId() == MCU_MCU_APPLYSPEAKER_REQ)
					{
						memset( &tHeadMsg,0,sizeof(tHeadMsg) );							
						tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tNormalMt );
						cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
						cServMsg.CatMsgBody( (u8*)&tNormalMt,sizeof(tNormalMt) );							
					}
					cServMsg.SetEventId(pcMsg->event + 2);
					SendReplyBack(cServMsg, cServMsg.GetEventId());
					
					ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcVcsApplyCancelSpeakerReq] over max speak queue num. ignore it!\n");
				}
			}
		}
		else
		{
			if (!m_tApplySpeakQue.IsQueueFull())
			{
				if (cServMsg.GetEventId() == MCU_MCU_APPLYSPEAKER_REQ)
				{
					memset( &tHeadMsg,0,sizeof(tHeadMsg) );							
					tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tNormalMt );
					cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
					cServMsg.CatMsgBody( (u8*)&tNormalMt,sizeof(tNormalMt) );						
				}
				cServMsg.SetEventId(pcMsg->event + 1);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
                
				m_tApplySpeakQue.ProcQueueInfo(tMt,bIsSendToChairman);
				NotifyMcsApplyList( bIsSendToChairman );
			}
			else
			{
				if (cServMsg.GetEventId() == MCU_MCU_APPLYSPEAKER_REQ)
				{
					memset( &tHeadMsg,0,sizeof(tHeadMsg) );							
					tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tNormalMt );
					cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
					cServMsg.CatMsgBody( (u8*)&tNormalMt,sizeof(tNormalMt) );						
				}
				cServMsg.SetEventId(pcMsg->event + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
                
				ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcVcsApplyCancelSpeakerReq] over max speak queue num. ignore it!\n");
			}
		}
	}
		//取消发言
	else
	{
		if( !(m_cVCSConfStatus.GetCurVCMT() == tMt) )
		{
			if( m_tCascadeMMCU.IsNull() )
			{
				if( MT_MCU_APPLYCANCELSPEAKER_REQ != cServMsg.GetEventId() )
				{						
					memset( &tHeadMsg,0,sizeof(tHeadMsg) );
					tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tMt );
					cServMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(tHeadMsg) );		//要申请的Mt
					cServMsg.CatMsgBody( (u8 *)&tMt, sizeof(tMt) );
				}					
				cServMsg.SetEventId( cServMsg.GetEventId() + 2 );
				SendReplyBack( cServMsg,cServMsg.GetEventId() );
			}
			return;
		}
		TMt tOrgMt = tMt;
		if( CONF_SPEAKMODE_ANSWERINSTANTLY != m_tConf.GetConfSpeakMode() ||
			m_tApplySpeakQue.IsQueueNull() )
		{
			tMt.SetNull();
			ChgCurVCMT( tMt );				
		}
		else
		{
			if(  m_tApplySpeakQue.GetQueueHead( tMt ) )
			{
				if( !tMt.IsNull() )
				{						
					ChgCurVCMT( tMt );													
				}
				else
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcVcsApplyCancelSpeakerReq] Queue Head is null!\n" );
				}
			}
			else
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcVcsApplyCancelSpeakerReq] Fail to Get Mt From Queue!\n" );
			}
		}	
		
		if( m_cVCSConfStatus.GetCurVCMode() == VCS_GROUPVMP_MODE )
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcVcsApplyCancelSpeakerReq] Now Is GroupVmp Mode,Can't Cancel mt,so not reply msg to mcu.\n" );
			return;
		}
		
		if( MCU_MCU_CANCELMESPEAKER_REQ == cServMsg.GetEventId() )
		{				
			memset( &tHeadMsg,0,sizeof(tHeadMsg) );
			tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tOrgMt,tOrgMt );
			cServMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(tHeadMsg) );		//要申请的Mt
			cServMsg.CatMsgBody( (u8 *)&tOrgMt, sizeof(tOrgMt) );				
		}
		cServMsg.SetEventId( cServMsg.GetEventId() + 1 );
		SendReplyBack( cServMsg,cServMsg.GetEventId() );
	}
	
	return;
}
/*=============================================================================
    函 数 名： ProcMcsApplyCancelSpeakerReq
    功    能： MCS会议MTC-BOX发言人申请/取消消息处理
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： void
 -----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2011/03/28  4.6			朱胜泽                创建
=============================================================================*/
void CMcuVcInst::ProcMcsApplyCancelSpeakerReq(const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	TMsgHeadMsg tHeadMsg;
	TMt tMt;
	
	BOOL32 bIsApplySpeaker = FALSE;//TRUE:申请发言, FALSE:取消申请/发言
	TMt tNormalMt;

	if( MT_MCU_APPLYCANCELSPEAKER_REQ == cServMsg.GetEventId() )
	{
		//SendReplyBack(cServMsg, pcMsg->event + 1);
		
		memset( &tHeadMsg,0,sizeof(tHeadMsg) );
		
		tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
		bIsApplySpeaker = (*(u8*)cServMsg.GetMsgBody()) == 1;
	}
	else if( MCU_MCU_CANCELMESPEAKER_REQ == cServMsg.GetEventId() )
	{
		tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
		tMt = *(TMt*)( cServMsg.GetMsgBody()+sizeof(TMsgHeadMsg) );
		tMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc,tMt );
		bIsApplySpeaker = FALSE;
	}
	else if (pcMsg->event == MCU_MCU_APPLYSPEAKER_REQ)
    {
		tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();	
        tMt = *(TMt*)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );
		tMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc,tMt );
		bIsApplySpeaker = TRUE;
    }

	//级联上级MCU不空,继续往上发
	if ( !m_tCascadeMMCU.IsNull() )
    {        
		if( bIsApplySpeaker )
		{
			if( MT_MCU_APPLYCANCELSPEAKER_REQ == cServMsg.GetEventId() )
			{
				cServMsg.SetEventId( MCU_MCU_APPLYSPEAKER_REQ );
				cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
				tMt = GetMcuIdMtFromMcuIdxMt( tMt );
				cServMsg.CatMsgBody( (u8*)&tMt,sizeof(tMt) );
			}
			SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_APPLYSPEAKER_REQ, cServMsg);	
		}
		else
		{			
			if( MT_MCU_APPLYCANCELSPEAKER_REQ == cServMsg.GetEventId() )
			{
				cServMsg.SetEventId( MCU_MCU_CANCELMESPEAKER_REQ );
				cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
				tMt = GetMcuIdMtFromMcuIdxMt( tMt );
				cServMsg.CatMsgBody( (u8*)&tMt,sizeof(tMt) );
			}			
			SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_CANCELMESPEAKER_REQ, cServMsg);
		}
        return;
    }
	
	BOOL32 bIsSendToChairman = FALSE;
	
	if (bIsApplySpeaker)
	{//申请发言

		if(!m_tConfAllMtInfo.MtJoinedConf( tMt ))
		{
			if (cServMsg.GetEventId() == MCU_MCU_APPLYSPEAKER_REQ)
			{
				memset( &tHeadMsg,0,sizeof(tHeadMsg) );							
				tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tNormalMt );
				cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
				cServMsg.CatMsgBody( (u8*)&tNormalMt,sizeof(tNormalMt) );
				cServMsg.SetEventId(pcMsg->event + 2);
				SendReplyBack(cServMsg, cServMsg.GetEventId());
			}
			return;
		}

		if (m_tConf.GetSpeaker() == tMt)
		{//当前正在发言

			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMcsApplyCancelSpeakerReq]MT%d is speaker\n", tMt.GetMtId());
			return;
		}
		
		if (m_tApplySpeakQue.IsMtInQueue(tMt))
		{//如果正在队列中则视为要取消申请

// 			m_tApplySpeakQue.ProcQueueInfo(tMt, bIsSendToChairman, FALSE, TRUE);
// 			NotifyMtSpeakStatus( tMt, emDenid );
//             NotifyMcsApplyList( bIsSendToChairman );

			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMcsApplyCancelSpeakerReq]MT%d has been in the ApplySpeak Queue!\n", tMt.GetMtId());
			return;
		}

		if (!m_tApplySpeakQue.IsQueueFull())
		{
			if (cServMsg.GetEventId() == MCU_MCU_APPLYSPEAKER_REQ)
			{
				memset( &tHeadMsg,0,sizeof(tHeadMsg) );							
				tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tNormalMt );
				cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
				cServMsg.CatMsgBody( (u8*)&tNormalMt,sizeof(tNormalMt) );						
			}
			cServMsg.SetEventId(pcMsg->event + 1);
			SendReplyBack(cServMsg, cServMsg.GetEventId());
			
			m_tApplySpeakQue.ProcQueueInfo(tMt,bIsSendToChairman);
			NotifyMcsApplyList( bIsSendToChairman );
		}
		else
		{
			if (cServMsg.GetEventId() == MCU_MCU_APPLYSPEAKER_REQ)
			{
				memset( &tHeadMsg,0,sizeof(tHeadMsg) );							
				tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tNormalMt );
				cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(tHeadMsg) );
				cServMsg.CatMsgBody( (u8*)&tNormalMt,sizeof(tNormalMt) );						
			}
			cServMsg.SetEventId(pcMsg->event + 2);
			SendReplyBack(cServMsg, cServMsg.GetEventId());
			
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT, "[ProcMcsApplyCancelSpeakerReq] over max speak queue num. ignore it!\n");
		}					
	}
	else
	{//取消发言

		if (m_tApplySpeakQue.IsMtInQueue(tMt))
		{
			m_tApplySpeakQue.ProcQueueInfo(tMt, bIsSendToChairman, FALSE, TRUE);
			NotifyMcsApplyList( bIsSendToChairman );
			NotifyMtSpeakStatus(tMt, emCanceled);
		}

		if ((m_tConf.GetSpeaker() == tMt))
		{
            //语音激励下不能取消发言人
            if (m_tConf.m_tStatus.IsVACing())
            {
                ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT, "[ProcMcsApplyCancelSpeakerReq]MT%d can't cancel speak in Vac\n", tMt.GetMtId());
            }
            else
            {                
                tMt.SetNull();
			    ChangeSpeaker(&tMt);
            }			
		}
	}

	return;
}
/*=============================================================================
    函 数 名： ProcMtMcuApplyCancelSpeakerReq
    功    能： MT通过MTC-BOX发言人申请/取消消息处理
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： void
 -----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2011/03/28  4.6			朱胜泽                创建
=============================================================================*/
void CMcuVcInst::ProcMtMcuApplyCancelSpeakerReq(const CMessage * pcMsg)
{
	STATECHECK
		
	if(VCS_CONF == m_tConf.GetConfSource())
	{//VCS会议处理
		ProcVcsApplyCancelSpeakerReq(pcMsg);
	}
	else
	{//MCS会议处理
		ProcMcsApplyCancelSpeakerReq(pcMsg);
	}
	return;
}

/*==============================================================================
函数名    :  ShowMcOtherTable
功能      :  打印其他终端信息
算法实现  :  
参数说明  :  
			 
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2011-7-27     4.6          周嘉麟						   创建
==============================================================================*/
void CMcuVcInst::ShowMcOtherTable()
{
	if (NULL == m_ptConfOtherMcTable)
	{
		return;
	}
	TConfMcInfo *pConfMcInfo = NULL;
	for(u16 wIdx = 0; wIdx < MAXNUM_CONFSUB_MCU; wIdx++)
	{
		pConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(wIdx);
		if (NULL == pConfMcInfo)
		{
			continue;
		}
		StaticLog("McuIdx.%d Info as follows\n", wIdx);
		StaticLog("\t\t AudioInfo [MixerCount.%d, Speaker(McuId.%d, MtId.%d)]\n",
						pConfMcInfo->m_tConfAudioInfo.m_byMixerCount,
						pConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMcuId(),
						pConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId());

		StaticLog("\t\t SpyMt(McuId.%d, MtId.%d), MMcuViewMt(McuId.%d, MtId.%d), LastMMcuViewMt(McuId.%d, MtId.%d)\n\n",
						pConfMcInfo->m_tSpyMt.GetMcuId(),
						pConfMcInfo->m_tSpyMt.GetMtId(),
						pConfMcInfo->m_tMMcuViewMt.GetMcuId(), 
						pConfMcInfo->m_tMMcuViewMt.GetMtId(),
						pConfMcInfo->m_tLastMMcuViewMt.GetMcuId(),
						pConfMcInfo->m_tLastMMcuViewMt.GetMtId());

	}
	return;
}

/*==============================================================================
函数名    :  IsCanGetSpecCapByMtIp
功能      :  判断能不能取得指定IP的能力集，如果成功则存放于pwCapSet中
算法实现  :  
注意点    :  (1) 根据mcudebug.ini取得,需要手动修改
			 (2) 存放数组由调用者保证空间
参数说明  :  s8* pIpAddr : [in]  匹配的IP地址
			 u32 adwCapSet[] : [out] 存放取得的能力集数组
			 BOOL32 bIsGetMainCapSet: [in] 取的是不是主能力集,默认TRUE

返回值说明:  成功：返回TRUE
			 失败：返回FALSE
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
11-08-10     1.0           彭国锋						   创建
==============================================================================*/
BOOL32 CMcuVcInst::IsCanGetSpecCapByMtIp(const s8* pIpAddr, u32 adwCapSet[] , BOOL32 bIsGetMainCapSet /* = TRUE */)
{
	// 0. 参数检查
	if ( !pIpAddr)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[IsCanGetSpecCapByMtIp]Error: Invalid Parameter<pIpAddr>!\n");
		return FALSE;
	}

	// 1. 从[mcuCheatIp]取对应的IP
	s8 achMcuCfgFile[64];
	memset(achMcuCfgFile, 0, sizeof(achMcuCfgFile));
	sprintf(achMcuCfgFile, "%s/%s", DIR_CONFIG, MCUDEBUGFILENAME);

	s32 nIPCnt = 0;						// 记录IP个数
	GetRegKeyInt(achMcuCfgFile, "mcuCheatIp", "EntryNum", 0, &nIPCnt);

	if ( 0 == nIPCnt)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[IsCanGetSpecCapByMtIp]Error: No Match <IP> in section<[mcuCheatIp]>!\n");
		return FALSE;
	}

	s8 achKeyName[6];					// 暂时最多是Entry255, 或Cap255
	memset(achKeyName, 0, sizeof(achKeyName));
	strncpy(achKeyName, "Entry", 5);	// 关键字的内容都是 Entry + XXX, XXX表示序号

	u16 wLen = strlen(pIpAddr);			// 输入IP地址的长度,用于比较
	u8 byCapSetIdx = 255;				// 所在能力集的序列号,如EntryXXX,初值为最大,防止取不到也是对的

	for (u8 byLoop = 0; byLoop < nIPCnt; byLoop++)
	{
		achKeyName[5] = '0'+ byLoop;
		s8 achContent[24];				// 内容最大长度: 15位IP + tab + 3 + tab + 3 + \0
		memset(achContent, 0, sizeof(achContent));

		GetRegKeyString(achMcuCfgFile, "mcuCheatIp", achKeyName, 0, achContent, sizeof(achContent));
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[IsCanGetSpecCapByMtIp]get content(%s) of key(%s)\n", achContent, achKeyName);

		if (0 == strncmp(achContent, pIpAddr, wLen))
		{
			if ( bIsGetMainCapSet)
			{
				byCapSetIdx = achContent[wLen+1];
			}
			else
			{
				byCapSetIdx = achContent[wLen+3];
			}
			break;
		}
	}
	
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[IsCanGetSpecCapByMtIp]byCapSetIdx is (%d)\n", byCapSetIdx);

	// 2. 取能力集[主或辅由第三个参数决定]
	if ( 'N' == byCapSetIdx || 255 == byCapSetIdx)
	{
		// 这是针对辅格式的,如果是主格式,建议不要用'N'
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[IsCanGetSpecCapByMtIp]Warning: No Need To Cheat<MainCapSet>!\n");
		return FALSE;
	}

	// 3. 从[NonKedaCap]取对应能力集，不需要循环
	// "CapXXX ＝ 106	3	25	768"的形式
	strncpy(achKeyName, "Cap", 3);
	achKeyName[3] = byCapSetIdx;
	achKeyName[4] = '\0';
	s8 achStore[20];		// 每一行内容暂时定为20个字符		
	memset(achStore, 0, sizeof(achStore));
	GetRegKeyString(achMcuCfgFile, "NonKedaCap", achKeyName, 0, achStore, sizeof(achStore)-1);

	s8 *pCur = &achStore[0];
	s8 *pStart = &achStore[0];
	s32 nStoreLen = strlen(achStore);	// 不能用U类型的
	u8 byCapIdx = 0;
	s8 tmp[16]={0};			// 临时数组,用来存放tab和tab之间的字符,此处根据配置文件的取值,最大为IP的长度,所以定为15+1
	while ( nStoreLen-- > 0)
	{
		if ( '\t' != *pCur)
		{
			if ( nStoreLen == 0)
			{
				// 已经是最后一个
				memset(tmp, 0, sizeof(tmp));
				memcpy(tmp, pStart, strlen(pStart));
				adwCapSet[byCapIdx++] = strtoul(tmp, 0, 10);
			}
		}
		else
		{
			if ( pCur == pStart || (size_t(pCur-pStart) >= sizeof(tmp)) )
			{
				// 1.防止内容只有一个tab的情况<实际验证如果只有一个tab的情况,转成10进制就是0>
				// 2.防止2个指针之间的差值超过临时数组可以存放的空间(包括\0)
				break;
			}
			memset(tmp, 0, sizeof(tmp));
			memcpy(tmp, pStart, pCur-pStart);
			adwCapSet[byCapIdx++] = atoi(pStart);
			pStart = pCur+1; // 不能++,有副作用
		}
		pCur++;
	}

	// 针对内容都没取到的情况
	if ( byCapIdx == 0)
	{
		return FALSE;
	}
	else 
	{
		return TRUE;
	}
}

/*=============================================================================
函 数 名： ProcMtMcuTransParentMsgNotify
功    能： 
算法实现： 界面、终端消息透传
全局变量： 
参    数： const CMessage * pcMsg 
返 回 值： void
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2/23/2012   4.6			朱胜泽                创建
=============================================================================*/
void CMcuVcInst::ProcMtMcuTransParentMsgNotify( const CMessage * pcMsg )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );
    u8 byMtId = cServMsg.GetSrcMtId();
    TMt tMt = m_ptMtTable->GetMt( byMtId );
    
    CServMsg cMsgToMcs;
    cMsgToMcs.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
    cMsgToMcs.CatMsgBody( (u8*)cServMsg.GetMsgBody(), cServMsg.GetMsgBodyLen());
    cMsgToMcs.SetEventId(MCU_MCS_TRANSPARENTMSG_NOTIFY);
    SendMsgToAllMcs(MCU_MCS_TRANSPARENTMSG_NOTIFY, cMsgToMcs);
	
    return;    
}

/*=============================================================================
函 数 名： ConstructDSCapExFor8KH8KE
功    能： 非同主视频带H264双流的8000H，8000G，构造双流勾选能力集
算法实现： 
全局变量： 
参    数： const TCapSupport & tCapsupport 会议能力集
		   TVideoStreamCap *pTDSCapEx 双流能力集指针
返 回 值： void
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
4/14/2012   4.7		    陈代伟               创建
=============================================================================*/
void CMcuVcInst::ConstructDSCapExFor8KH8KE(const TCapSupport & tCapSupport, TVideoStreamCap *pTDSCapEx)
{
	if(pTDSCapEx == NULL)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ConstructDSCapExFor8KH8KE]pTDSCapEx == NULL, error!");

		return;
	}
#if defined(_8KH_)	
	// 只支持转发 [4/14/2012 chendaiwei]
	if( (tCapSupport.GetMainVideoType() == MEDIA_TYPE_H264 && tCapSupport.GetDStreamResolution() == VIDEO_FORMAT_UXGA &&
		( tCapSupport.GetDStreamUsrDefFPS() == 15 ||
		  tCapSupport.GetDStreamUsrDefFPS() == 20 ||
		  tCapSupport.GetDStreamUsrDefFPS() == 30 ||
		  tCapSupport.GetDStreamUsrDefFPS() == 60 ) ) 
		||( tCapSupport.GetMainVideoType() == MEDIA_TYPE_H264 && tCapSupport.GetDStreamResolution() == VIDEO_FORMAT_SXGA &&
		   ( tCapSupport.GetDStreamUsrDefFPS() == 30 ||
		     tCapSupport.GetDStreamUsrDefFPS() == 60) )
		)
	{
		return;
	}
#endif


	
	BOOL32 bIgnoreDSBpCapEx = TRUE; //8000H主流HP无BP勾选情况下，双流无BP勾选[8/24/2012 chendaiwei]

#if defined(_8KH_) || defined(_8KI_)
	if( IsConfHasBpCapOrBpExCap(m_tConf,m_tConfEx) )
	{
		bIgnoreDSBpCapEx = FALSE;
	}
//	if( tCapSupport.GetMainVideoType() == MEDIA_TYPE_H264 )
//	{
//		if( tCapSupport.GetMainStreamProfileAttrb() == emHpAttrb )
//		{
//			TVideoStreamCap atMStreamCapEx[MAX_CONF_CAP_EX_NUM];
//			u8 byCapExNum = MAX_CONF_CAP_EX_NUM;
//			m_tConfEx.GetMainStreamCapEx(atMStreamCapEx,byCapExNum);
//
//			for( u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++ )
//			{
//				if( atMStreamCapEx[byIdx].GetMediaType() == MEDIA_TYPE_H264 
//					&& atMStreamCapEx[byIdx].GetH264ProfileAttrb() == emBpAttrb)
//				{
//					bIgnoreDSBpCapEx = FALSE;
//					break;
//				}
//			}
//		}
//		else
//		{
//			bIgnoreDSBpCapEx = FALSE;
//		}
//	}
#endif

		BOOL32 bIsCanReserveXga = TRUE;

#ifdef _8KI_
		if( Is8KINeedDsAdaptH264SXGA(m_tConf) && IsConfDoubleDual(m_tConf) )
		{
			bIsCanReserveXga = FALSE;
		}
#endif

	// 8000H/8000G非同主视频H264双流构造勾选 [4/13/2012 chendaiwei]
	if( tCapSupport.GetDStreamMediaType() == MEDIA_TYPE_H264 
		&& tCapSupport.GetDStreamType() != VIDEO_DSTREAM_MAIN 
		&& tCapSupport.GetDStreamType()!=VIDEO_DSTREAM_MAIN_H239 )
	{
		
		u8 byCount = 0;
		// 8KH HP主分辨率勾选填入BP [4/14/2012 chendaiwei]
#if defined(_8KH_) || defined(_8KI_)
		if(tCapSupport.GetMainStreamProfileAttrb() == emHpAttrb && !bIgnoreDSBpCapEx)
		{
			TVideoStreamCap tTempCapBp(MEDIA_TYPE_H264,tCapSupport.GetDStreamResolution(),tCapSupport.GetDStreamUsrDefFPS(),emBpAttrb,0);
			pTDSCapEx[byCount] = tTempCapBp;
			byCount ++;
		}
#endif

		u8 abyRes[] = { VIDEO_FORMAT_UXGA, VIDEO_FORMAT_SXGA, VIDEO_FORMAT_XGA};
		u8 byResNum = sizeof(abyRes)/sizeof(abyRes[0]);

		if( !bIsCanReserveXga )
		{
			abyRes[2] = VIDEO_FORMAT_INVALID;
		}
		
		//本地视频能力分辨率
		u8 byLocalResPos;
		for (byLocalResPos = 0; byLocalResPos < byResNum; byLocalResPos++)
		{			

			if (tCapSupport.GetDStreamResolution() == abyRes[byLocalResPos])
			{
				break;
			}
			else
			{
				if ( byLocalResPos == byResNum - 1 )
				{					
					LogPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[ConstructDSCapExFor8KH8KE]not find Res of h264 video cap of ds cap !");

					return;
				}
				else
				{
					continue;
				}				
			}
		}
		
		byLocalResPos ++; //主双流能力不填入勾选

		for(; byLocalResPos < byResNum; byLocalResPos++ )
		{

			if( VIDEO_FORMAT_INVALID == abyRes[byLocalResPos] )
			{
				continue;
			}
			
#if defined(_8KH_) || defined(_8KI_)
			if(tCapSupport.GetMainStreamProfileAttrb() == emHpAttrb)
			{
				TVideoStreamCap tTempCapHp(MEDIA_TYPE_H264,abyRes[byLocalResPos],tCapSupport.GetDStreamUsrDefFPS(),emHpAttrb,0);
				pTDSCapEx[byCount] = tTempCapHp;
				byCount ++;
			}

			if( !bIgnoreDSBpCapEx )
			{
				TVideoStreamCap tTempCapBp(MEDIA_TYPE_H264,abyRes[byLocalResPos],tCapSupport.GetDStreamUsrDefFPS(),emBpAttrb,0);
				pTDSCapEx[byCount] = tTempCapBp;
				byCount ++;	
			}
#else
			TVideoStreamCap tTempCapBp(MEDIA_TYPE_H264,abyRes[byLocalResPos],tCapSupport.GetDStreamUsrDefFPS(),emBpAttrb,0);
			pTDSCapEx[byCount] = tTempCapBp;
			byCount ++;	
#endif //8000G
		}
#if defined(_8KI_)
		if( Is8KINeedDsAdaptH264SXGA(m_tConf) &&  tCapSupport.GetDStreamUsrDefFPS() > 20 )
		{
			if(tCapSupport.GetMainStreamProfileAttrb() == emHpAttrb) 
			{
				TVideoStreamCap tTempCapHp(MEDIA_TYPE_H264,VIDEO_FORMAT_SXGA,20,emHpAttrb,0);
				pTDSCapEx[byCount] = tTempCapHp;
				byCount++;
			}

			if( !bIgnoreDSBpCapEx )
			{
				TVideoStreamCap tTempCapBp(MEDIA_TYPE_H264,VIDEO_FORMAT_SXGA,20,emBpAttrb,0);
				pTDSCapEx[byCount] = tTempCapBp;
				byCount ++;	
			}
		}
		if( bIsCanReserveXga && tCapSupport.GetDStreamUsrDefFPS() > 5 )
		{
			if(tCapSupport.GetMainStreamProfileAttrb() == emHpAttrb) 
			{
				TVideoStreamCap tTempCapHp(MEDIA_TYPE_H264,VIDEO_FORMAT_XGA,5,emHpAttrb,0);
				pTDSCapEx[byCount] = tTempCapHp;
				byCount++;
			}

			if( !bIgnoreDSBpCapEx )
			{
				TVideoStreamCap tTempCapBp(MEDIA_TYPE_H264,VIDEO_FORMAT_XGA,5,emBpAttrb,0);
				pTDSCapEx[byCount] = tTempCapBp;
				byCount ++;	
			}
		}
		
#endif
	}

	return;
}
/*
 *	通过MTID判断MT的IP类型
 */
BOOL32 CMcuVcInst::IsMtIpV6(u8 byMtId )
{
	if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT)
	{
		return FALSE;
	}

// 	u32 dwMtIp = 0;
// 	u16 wPort = 0;
// 	m_ptMtTable->GetMtSwitchAddr(byMtId, dwMtIp, wPort);

	return g_cMcuVcApp.IsMtIpV6(/*dwMtIp*/m_ptMtTable->GetIPAddr(byMtId));
}

//END OF FILE
