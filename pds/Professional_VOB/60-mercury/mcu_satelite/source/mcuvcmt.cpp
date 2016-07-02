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
#include "mcuerrcode.h"
#include "mpmanager.h"

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
u8 CMcuVcInst::AddMt( TMtAlias &tMtAlias, u16 wMtDialRate, u8 byCallMode, 
                      BOOL bPassiveAdd )
{
	u8 byMtId;
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
    
	if( mtAliasTypeTransportAddress == tMtAlias.m_AliasType || 
		mtAliasTypeE164 == tMtAlias.m_AliasType || 
		mtAliasTypeH323ID == tMtAlias.m_AliasType ||
		// PU特殊类型
		puAliasTypeIPPlusAlias == tMtAlias.m_AliasType)
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
                ConfLog(FALSE, "[AddMt] Mt.%d existed, replace.\n", byMtId);
                return byMtId;
		    }
        }

		// 原来不存在的终端,分配一个新的给他		
		byMtId = m_ptMtTable->AddMt( &tMtAlias, bRepeat );
		if(byMtId > 0)
		{
			m_ptMtTable->SetDialAlias( byMtId, &tMtAlias );
			m_ptMtTable->SetDialBitrate( byMtId, wMtDialRate );
			m_ptMtTable->SetMcuId( byMtId, LOCAL_MCUID);
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
                ConfLog(FALSE, "[AddMt] Mt.%d existed, replace.\n", byMtId);
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
			m_ptMtTable->SetMcuId( byMtId, LOCAL_MCUID);
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
            
            RemoveJoinedMt( tMt, TRUE );
        }
        if (!g_cMcuVcApp.IsMpConnected(byMpId))
        {
            ConfLog(FALSE, "[AddJoinedMt] byMpId.%d is OFFline for MT.%d\n", byMpId, tMt.GetMtId());
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
        ConfLog( FALSE, "[AddJoinedMt] AssignMpForMt.%d failed !\n", tMt.GetMtId() );
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

	bHasMsgMcsAlias = m_tConfAllMtInfo.MtInConf( tMt.GetMtId() ); 

	//加入与会终端列表
	m_tConfAllMtInfo.AddJoinedMt( tMt.GetMtId() );

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
            MtStatusChange( tMt.GetMtId(), TRUE );
		}

	}

	//通知会控
//	cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
//	SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
	
	//发给会控会议所有终端信息
	cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

	//发给会控终端表
	SendMtListToMcs(LOCAL_MCUID);
	
	if( !bHasMsgMcsAlias )
	{		
		SendMtAliasToMcs( tMt );
	}

	//发Trap消息
	TMtNotify tMtNotify;
	CConfId   cConfId;
	TMtAlias  tMtAlias;
	u8 byLen = 0;
	cConfId = m_tConf.GetConfId( );
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
    if ( tMt.GetMtType() == MT_TYPE_MT )
    {
        CServMsg cMsg;
        cMsg.SetEventId(MCU_MT_GETMTVERID_REQ);
        SendMsgToMt(tMt.GetMtId(), MCU_MT_GETMTVERID_REQ, cMsg);        
    }

#ifdef _SATELITE_
    RefreshConfState();
#endif


	//打印消息
	CallLog( "Mt%d: 0x%x(Dri:%d) joined conf success!\n", 
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
void CMcuVcInst::RemoveMt( TMt &tMt, BOOL32 bMsg )
{
	CServMsg	cServMsg;
	BOOL32      bNeedNotifyMt = FALSE;
		
	cServMsg.SetConfId( m_tConf.GetConfId() );
	
    tMt = m_ptMtTable->GetMt(tMt.GetMtId());

	m_ptMtTable->SetNotInvited( tMt.GetMtId(), FALSE );
	RemoveJoinedMt( tMt, bMsg );

	
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
	TConfMtInfo *pcInfo = m_tConfAllMtInfo.GetMtInfoPtr(tMt.GetMtId());
	if(pcInfo != NULL)
	{
		pcInfo->SetNull();
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
	CallLog( "Mt%d: 0x%x(Dri:%d) removed from conf!\n", 
			       tMt.GetMtId(), m_ptMtTable->GetIPAddr(tMt.GetMtId()), tMt.GetDriId() );

	//从会议终端表中删除
	m_ptMtTable->DelMt( tMt.GetMtId() );

	NotifyMcuDelMt(tMt);

	//发给会控会议所有终端信息
	cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

	//发给会控终端表
//	SendMtListToMcs(LOCAL_MCUID);

	//发给会控会议信息表
	cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
	SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );

    // zbq [03/23/2007] 如果是轮询列表里的终端，不通知会控刷轮询列表，会控自己维护；
    u8 byMtPos;
    if ( m_tConfPollParam.IsExistMt(tMt, byMtPos) )
    {
        m_tConfPollParam.RemoveMtFromList(tMt);
		m_tConfPollParam.SpecPollPos(byMtPos);
    }

	//会议中无终端,或者VCS会议中该会议为下级会议，上级会议结束，下级会议同样结束
	if( (m_tConfAllMtInfo.GetLocalMtNum() == 0 && m_tConf.GetConfAttrb().IsReleaseNoMt()) ||
		(MT_TYPE_MMCU == tMt.GetMtType() && CONF_CREATE_MT == m_byCreateBy && VCS_CONF == m_tConf.GetConfSource()))
	{
		ConfLog( FALSE, "There's no MT or vcs mmcuconf is over in conference %s now. Release it!\n", 
				 m_tConf.GetConfName() );
		ReleaseConf( TRUE );
		NEXTSTATE( STATE_IDLE );
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
	u16 wMinBitRate;
	TSimCapSet tSrcSimCapSet;
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
    if (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE 
		&& m_tConf.m_tStatus.IsBrdstVMP()
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
    else
    {
        u16 wAdaptBitRate = 0;      
           
        //双格式会议需类型适配终端降适配码率
        if (IsMtNeedAdapt(ADAPT_TYPE_VID, tMt.GetMtId())  &&
            (IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate) || IsNeedCifAdp()))
        {
            if (wMinBitRate != m_wVidBasBitrate)
            {
                if (m_tConf.m_tStatus.IsVidAdapting())
                {
                    Mt2Log("[ProcMtMcuFlowControlCmd1] change vid bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                           wMinBitRate, m_wVidBasBitrate);
                    ChangeAdapt(ADAPT_TYPE_VID, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }                
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
                    Mt2Log("[ProcMtMcuFlowControlCmd3]change br bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                           wMinBitRate, m_wBasBitrate);
                    ChangeAdapt(ADAPT_TYPE_BR, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }
                else
                {
                    Mt2Log("[ProcMtMcuFlowControlCmd3]start br bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                           wMinBitRate, m_wBasBitrate);
                    StartAdapt(ADAPT_TYPE_BR, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }                   
                
                StartSwitchToSubMt(m_tBrBasEqp, m_byBrBasChnnl, tMt.GetMtId(), MODE_VIDEO);
            }
            else if (m_tConf.m_tStatus.IsBrAdapting())//若不需要调节适配码率，直接将适配码流交换到此mt(会议中有其他终端的接收码率更低的情况)
            {
                Mt2Log("[ProcMtMcuFlowControlCmd1] switch br bas to mt<%d>\n", tMt.GetMtId());
                StartSwitchToSubMt(m_tBrBasEqp, m_byBrBasChnnl, tMt.GetMtId(), MODE_VIDEO);
            }
        }         
        //双格式会议无需适配终端降其视频源码率，可能会将码流压到最低
        else if (MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() || IsNeedCifAdp())
        {
            AdjustMtVideoSrcBR(tMt.GetMtId(), wMinBitRate, byMode);
        }
    }	

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
====================================================================*/
void CMcuVcInst::RemoveJoinedMt( TMt & tMt, BOOL32 bMsg, u8 byReason,BOOL32 bNeglectMtType )
{
	CServMsg	cServMsg;
    cServMsg.SetConfId( m_tConf.GetConfId() );
    cServMsg.SetConfIdx( m_byConfIdx );

	u8			byMode;

    // xsl [11/10/2006] 释放接入和转发资源
    ReleaseMtRes( tMt.GetMtId(), bNeglectMtType);

	//是否入会
	if (!m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId()))
	{
		//通知
		if (bMsg)
		{
			SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);
		}
		return;
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

	// xsl [10/26/2006] 移除丢包重传终端，停止时需考虑vmp等外设广播源的情况
	if (m_tConf.m_tStatus.IsPrsing())
	{
        if (tMt == m_tAudBrdSrc)
        {
            StopPrs(PRSCHANMODE_AUDIO);
        }
        else
        {
            AddRemovePrsMember(tMt, FALSE, PRSCHANMODE_AUDIO);
        }

        if (tMt == m_tVidBrdSrc)
        {
            StopPrs(PRSCHANMODE_FIRST);
        }
        else
        {
            AddRemovePrsMember(tMt, FALSE, PRSCHANMODE_FIRST);
        }

		if (tMt == m_tDoubleStreamSrc)
		{
			StopPrs(PRSCHANMODE_SECOND);
		}
        else
        {
            AddRemovePrsMember(tMt, FALSE, PRSCHANMODE_SECOND);
        }	
	}
	
	//若是最后一台适配终端,停止适配 //modify bas 2 -- 这里需要考虑是否停止适配
//	if( m_tConf.m_tStatus.IsMdtpAdapting() && 
//		IsMtNeedAdapt( tMt.GetMtId(), ADAPT_TYPE_MDTP ) && 
//		1 == GetNeedAdaptMtNum( ADAPT_TYPE_MDTP ) )
//	{
//		StopBasAdapt( ADAPT_TYPE_MDTP );	
//	}
//
//	if( m_tConf.m_tStatus.IsBrAdapting() && 
//		IsMtNeedAdapt( tMt.GetMtId(), ADAPT_TYPE_BR ) && 
//		1 == GetNeedAdaptMtNum( ADAPT_TYPE_BR ) )
//	{
//		StopBasAdapt( ADAPT_TYPE_BR );	
//	}		

	//停止混音
	if (m_tConf.m_tStatus.IsMixing() || m_tConf.m_tStatus.IsVACing())
	{
        // xsl [8/4/2006] 定制混音成员处理
        if (m_tConf.m_tStatus.IsSpecMixing())
        {
            if (m_ptMtTable->IsMtInMixing(tMt.GetMtId()))
            {
                AddRemoveSpecMixMember(&tMt, 1, FALSE);
            }            
        }
        else //vac或讨论移除通道
        {            
            //在混音组内此拆除交换
            if (m_ptMtTable->IsMtInMixGrp(tMt.GetMtId()))
            {               
                StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
			                (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt.GetMtId())), FALSE, MODE_AUDIO);
            }   
            RemoveMixMember(&tMt, FALSE); 
        }                    
	}

	//停止VMP
	if( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE )
	{
        BOOL32 bStopVmp = TRUE;
		BOOL32 bChangeNotify = FALSE;
		TVMPParam tVMPParam = m_tConf.m_tStatus.GetVmpParam();
		for( u8 byLoop = 0; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++  )
		{
			TMt tMembMt = *(TMt*)tVMPParam.GetVmpMember(byLoop);
			TMt tLocalMt = GetLocalMtFromOtherMcuMt( tMembMt );
			if( tLocalMt.GetMtId() == tMt.GetMtId() )
			{
                TVMPMember tVmpMember = *(TVMPMember*)tVMPParam.GetVmpMember(byLoop);

                //画面合成成员类型为“会控指定”时进行状态清理，删除其本身。其他画面
                //合成成员类型在此不作处理，防止误清除了相关的跟随通道。[03/21/2006-zbq]
                if ( VMP_MEMBERTYPE_MCSSPEC == tVmpMember.GetMemberType() )
                {
                    StopSwitchToPeriEqp( m_tVmpEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO );
                    if( !IsDynamicVmp() )
                    {
                        TVMPMember tVMPMember;
                        memset( &tVMPMember, 0, sizeof(TVMPMember) );
                        tVMPParam.SetVmpMember( byLoop, tVMPMember );
                        bChangeNotify = TRUE;
                    }
                }

                // zbq [04/25/2007] 如果该终端在会议模板的VMP信息里，直接删除之
                for( u8 byIndex = 0; byIndex < MAXNUM_MPUSVMP_MEMBER; byIndex ++ )
                {
                    // 不考虑级联画面合成的情况
                    if ( TYPE_MT == tMt.GetMtType() &&
                        m_tConfEqpModule.m_atVmpMember[byIndex].GetMtId() == tMt.GetMtId() ) 
                    {
                        m_tConfEqpModule.m_atVmpMember[byIndex].SetNull();
                    }
                }
			}

            TVMPMember tVmpMember = *tVMPParam.GetVmpMember(byLoop);
            if (tVmpMember.GetMemberType() != 0 || !tVmpMember.IsNull())
            {
                bStopVmp = FALSE;
            }
		}
		m_tConf.m_tStatus.SetVmpParam( tVMPParam );
        
		//动态分屏改变风格
		ChangeVmpStyle( tMt, FALSE );
		
		if( TRUE == bChangeNotify )
		{
			TPeriEqpStatus tVmpEqpStatus;
			g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpEqpStatus);

            if (bStopVmp)
            {
                //停止交换数据
			    ChangeVmpSwitch( 0, VMP_STATE_STOP );

                //停止
				TPeriEqpStatus tPeriEqpStatus;
				if (!m_tVmpEqp.IsNull() &&
					g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tPeriEqpStatus) &&
					tPeriEqpStatus.GetConfIdx() == m_byConfIdx &&
					tPeriEqpStatus.m_tStatus.tVmp.m_byUseState != TVmpStatus::WAIT_STOP)
				{
					SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER, 6*1000);
					tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_STOP;
					g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);	
				
					CServMsg cTempServ;
					SendMsgToEqp(m_tVmpEqp.GetEqpId(), MCU_VMP_STOPVIDMIX_REQ, cTempServ); 
				}
            }
            else
            {
                //ChangeVmpParam(&tVMPParam);
				// xliang [1/6/2009] 区分新老VMP设VMP param
				AdjustVmpParam(&tVMPParam);
            }

			tVmpEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
			g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tVmpEqpStatus);

//			//通知主席及所有会控
//			cServMsg.SetMsgBody( (u8*)&m_tConf.m_tStatus.m_tVMPParam, sizeof(TVMPParam) );
//			SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg );
//			if( HasJoinedChairman() )
//			{
//				SendMsgToMt( m_tConf.GetChairman().GetMtId(), MCU_MT_VMPPARAM_NOTIF, cServMsg );
//			}
//
		}

        ConfStatusChange();
	}
    
    //  xsl [2/10/2006] 停多画面电视墙
    if(m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE)
    {
        BOOL32 bStopVmpTw = TRUE;
        BOOL32 bChangeNotify = FALSE;
        TVMPParam tVMPParam = m_tConf.m_tStatus.GetVmpTwParam();
        for( u8 byLoop = 0; byLoop < MAXNUM_SDVMP_MEMBER; byLoop++  )
        {
            TMt tMembMt = *(TMt*)tVMPParam.GetVmpMember(byLoop);
            TMt tLocalMt = GetLocalMtFromOtherMcuMt( tMembMt );
            if( tLocalMt.GetMtId() == tMt.GetMtId() )
            {
                TVMPMember tVmpMember = *tVMPParam.GetVmpMember(byLoop);

                //画面合成成员类型为“会控指定”时进行状态清理，删除其本身。其他画面
                //合成成员类型在此不作处理，防止误清除了相关的跟随通道。[12/04/2006-zbq]
                if ( VMPTW_MEMBERTYPE_MCSSPEC == tVmpMember.GetMemberType() ) 
                {
                    StopSwitchToPeriEqp( m_tVmpTwEqp.GetEqpId(), byLoop, FALSE, MODE_VIDEO );
                    
                    TVMPMember tVMPMember;
                    memset( &tVMPMember, 0, sizeof(TVMPMember) );
                    tVMPParam.SetVmpMember( byLoop, tVMPMember );
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
            
            tVmpEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVmpTwParam;
            g_cMcuVcApp.SetPeriEqpStatus(m_tVmpTwEqp.GetEqpId(), &tVmpEqpStatus);
            
            if (bStopVmpTw)
            {
                //停止交换数据
                ChangeVmpTwSwitch( 0, VMPTW_STATE_STOP );
                
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

    //停止TvWall
    u8 byEqpId = TVWALLID_MIN;
	TPeriEqpStatus tTvwallStatus;
    while( byEqpId >= TVWALLID_MIN && byEqpId <= TVWALLID_MAX )
	{
		if (g_cMcuVcApp.IsPeriEqpValid(byEqpId))
		{
			g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTvwallStatus);
            if (tTvwallStatus.m_byOnline == 1)
            {
                u8 byTmpMtId = 0;
                u8 byMtConfIdx = 0;
                u8 byMemberNum = tTvwallStatus.m_tStatus.tTvWall.byChnnlNum;
				for(u8 byLoop = 0; byLoop < byMemberNum; byLoop++)
				{
                    byTmpMtId = tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetMtId();
                    byMtConfIdx = tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].GetConfIdx();
					if (tMt.GetMtId() == byTmpMtId 
						&& m_byConfIdx == byMtConfIdx
						&& TW_MEMBERTYPE_POLL != tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType
						)
					{
						StopSwitchToPeriEqp(byEqpId, byLoop);
                        tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType = 0;
                        tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetNull();
                        tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].SetConfIdx(0);
                        
                        //清终端状态
                        m_ptMtTable->SetMtInTvWall(byTmpMtId, FALSE);
					}
					else if ( TW_MEMBERTYPE_POLL == tTvwallStatus.m_tStatus.tTvWall.atVideoMt[byLoop].byMemberType )
					{
						TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
						
						TMtPollParam *ptCurPollMt = NULL;
						u8 byPollIdx = m_tConfPollParam.GetCurrentIdx();
						ptCurPollMt = GetNextMtPolled(byPollIdx, tPollInfo);
						ChangeTvWallSwitch( ptCurPollMt, byEqpId, byLoop, TW_MEMBERTYPE_POLL, TW_STATE_CHANGE );
						
        
                        //清终端状态
                        m_ptMtTable->SetMtInHdu(byTmpMtId, FALSE);
					}


                    // zbq [04/25/2007] 清除TvWall模版相应成员信息
                    for(u8 byLoop = 0; byLoop < MAXNUM_PERIEQP_CHNNL; byLoop++)
                    {
                        u8 byTvId = 0;
                        m_tConfEqpModule.m_tTvWallInfo[byLoop].RemoveMtByMtIdx(byTmpMtId, byTvId);
                        if( 0 != byTvId )
                        {
                            m_tConfEqpModule.m_tMultiTvWallModule.RemoveMtByTvId(byTvId, byTmpMtId);
                        }
                    }                    
				}
                g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tTvwallStatus);

                cServMsg.SetMsgBody((u8 *)&tTvwallStatus, sizeof(tTvwallStatus));
                SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
            }
		}
        byEqpId++;
	}

    //停止hdu
    u8 byHduId = HDUID_MIN;
	TPeriEqpStatus tHduStatus;
    while( byHduId >= HDUID_MIN && byHduId <= HDUID_MAX )
	{
		if (g_cMcuVcApp.IsPeriEqpValid(byHduId))
		{
			g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
            if (tHduStatus.m_byOnline == 1)
            {
                u8 byTmpMtId = 0;
                u8 byMtConfIdx = 0;
                u8 byMemberNum = tHduStatus.m_tStatus.tHdu.byChnnlNum;
				for(u8 byLoop = 0; byLoop < MAXNUM_HDU_CHANNEL; byLoop++)
				{
                    byTmpMtId = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetMtId();
                    byMtConfIdx = tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].GetConfIdx();
					if (tMt.GetMtId() == byTmpMtId 
						&& m_byConfIdx == byMtConfIdx 
						&&  TW_MEMBERTYPE_POLL != tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType
						&&  TW_MEMBERTYPE_SPEAKER != tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType
						&&  TW_MEMBERTYPE_CHAIRMAN != tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType
						&&  POLL_STATE_NONE == m_tHduBatchPollInfo.GetStatus()
						)
					{
						StopSwitchToPeriEqp(byHduId, byLoop);
                        tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType = 0;
                        tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetNull();
                        tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].SetConfIdx(0);
                        
                        //清终端状态
                        m_ptMtTable->SetMtInHdu(byTmpMtId, FALSE);
					}
					else if ( TW_MEMBERTYPE_POLL == tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType )
					{
						TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
						
						TMtPollParam *ptCurPollMt = NULL;
						u8 byPollIdx = m_tConfPollParam.GetCurrentIdx();
		                ptCurPollMt = GetNextMtPolled(byPollIdx, tPollInfo);
						ChangeHduSwitch( ptCurPollMt, byHduId, byLoop, TW_MEMBERTYPE_POLL, TW_STATE_CHANGE );
                        
                        //清终端状态
                        m_ptMtTable->SetMtInHdu(byTmpMtId, FALSE);
					}
					else if ( TW_MEMBERTYPE_SPEAKER == tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType )
					{ 
                        TMt tLocalSpeaker = m_tConf.GetSpeaker();
						tLocalSpeaker = GetLocalMtFromOtherMcuMt( tLocalSpeaker );
						ChangeHduSwitch( &tLocalSpeaker, byHduId, byLoop, TW_MEMBERTYPE_SPEAKER, TW_STATE_CHANGE );
					}
					else if ( TW_MEMBERTYPE_CHAIRMAN == tHduStatus.m_tStatus.tHdu.atVideoMt[byLoop].byMemberType )
					{
                        TMt tLocalSpeaker = m_tConf.GetChairman();
						tLocalSpeaker = GetLocalMtFromOtherMcuMt( tLocalSpeaker );
						ChangeHduSwitch( &tLocalSpeaker, byHduId, byLoop, TW_MEMBERTYPE_SPEAKER, TW_STATE_CHANGE );
					}
					else
					{
						// do nothing
					}
                   
				}
                g_cMcuVcApp.SetPeriEqpStatus(byHduId, &tHduStatus);
				
                cServMsg.SetMsgBody((u8 *)&tHduStatus, sizeof(tHduStatus));
                SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
            }
		}
        byHduId++;
	}	

	//电视墙轮询参数更改
	if ( POLL_STATE_NONE != m_tConf.m_tStatus.GethduPollState() )
	{
		ConfLog(FALSE, "[RemoveJoinedMt] adjust hdu poll param!\n");
		m_tHduPollParam.RemoveMtFromList( tMt);
	}
      
	//停止终端录像
	if( !m_ptMtTable->IsMtNoRecording( tMt.GetMtId() ) )
	{
		TEqp tEqp ;
		u8   byRecChnl;
		m_ptMtTable->GetMtRecordInfo( tMt.GetMtId() ,&tEqp ,&byRecChnl);
		StopSwitchToPeriEqp( tEqp.GetEqpId() ,byRecChnl);
		m_ptMtTable->SetMtNoRecording( tMt.GetMtId() );
		
		cServMsg.SetMsgBody( ( u8 * )&tEqp, sizeof( tEqp) );	//set TEqp
		cServMsg.SetChnIndex( byRecChnl );
		SendMsgToEqp( tEqp.GetEqpId(), MCU_REC_STOPREC_REQ, cServMsg );
	}
		
	//离开的终端是发言者
	if( GetLocalSpeaker() == tMt )
	{
		//混音模式下只对VIDEO部分进行处理
		byMode =  m_tConf.m_tStatus.IsMixing() ? MODE_VIDEO : MODE_BOTH;

		g_cMpManager.RemoveSwitchBridge( tMt, 0, byMode);
		
		if( !m_tConf.m_tStatus.IsNoRecording() )
		{
			StopSwitchToPeriEqp( m_tRecEqp.GetEqpId(), m_byRecChnnl, FALSE );
		}

        //modify bas 2 -- 验证这里停止交换是否多余，changvidbrdsrc 是否会做
        if (m_tConf.m_tStatus.IsAudAdapting())
        {
            StopSwitchToPeriEqp(m_tAudBasEqp.GetEqpId(), m_byAudBasChnnl, FALSE, byMode);
        }
        if (m_tConf.m_tStatus.IsVidAdapting())
        {
            StopSwitchToPeriEqp(m_tVidBasEqp.GetEqpId(), m_byVidBasChnnl, FALSE, byMode);
        }
        if (m_tConf.m_tStatus.IsBrAdapting())
        {
            StopSwitchToPeriEqp(m_tBrBasEqp.GetEqpId(), m_byBrBasChnnl, FALSE, byMode);
        }
	}

    ClearH239TokenOwnerInfo( &tMt );


    //停发第二路视频发送通道
    if(m_tDoubleStreamSrc == tMt)
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
    if (!m_tDoubleStreamSrc.IsNull() && !m_tConf.GetConfAttrb().IsSatDCastMode())
    {
        // 顾振华 [6/1/2006] 当不是卫星会议的时候，停第二路
        g_cMpManager.StopSwitchToSubMt(tMt, MODE_SECVIDEO, TRUE);
    }

	//判断是否是主席终端
	if( tMt == m_tConf.GetChairman() )
	{
		ChangeChairman( NULL );
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
            m_tConf.m_tStatus.GetMtPollParam().GetMcuId() == tMt.GetMtId() ) )
        {
            bIsMtInPolling = TRUE;
        }
    }    

	//判断是否为发言终端
	if( tMt == GetLocalSpeaker() )
	{
		ChangeSpeaker( NULL,  bIsMtInPolling);

		if( MT_TYPE_MMCU == tMt.GetMtType() )
		{
			TMt tNullMt;
			tNullMt.SetNull();
			m_tConf.SetSpeaker( tNullMt );
		}
		else
		{
			m_tConf.SetSpeaker( tMt );
		}
	}
    // zbq [03/12/2007] 如果是正在被轮询的终端，直接跳入下一个轮询终端
    if (bIsMtInPolling)
    {
        SetTimer( MCUVC_POLLING_CHANGE_TIMER, 10 );
    }

	//停止向这台终端交换
    if (!m_tConf.GetConfAttrb().IsSatDCastMode())
    {
        StopSwitchToSubMt( tMt.GetMtId(), MODE_BOTH );
    }	
    else
    {
        // 卫星会议则提示终端看自己
        NotifyMtReceive( tMt, tMt.GetMtId() );
    }
	
	//判断是否为回传通道,是则清除回传交换
	if( !m_tCascadeMMCU.IsNull() && !(m_tCascadeMMCU == tMt) )
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
		if( ptConfMcInfo != NULL && 
			( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
		{
			if( ptConfMcInfo->m_tSpyMt.GetMtId() == tMt.GetMtId() &&
				ptConfMcInfo->m_tSpyMt.GetMcuId() == tMt.GetMcuId() )
			{
				u8 byMode = MODE_BOTH;
				if (m_tConf.m_tStatus.IsMixing())
				{
					byMode = MODE_VIDEO;
				}					
				StopSwitchToSubMt( m_tCascadeMMCU.GetMtId(), byMode );
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
		g_cMpManager.StopMulticast( tMt, 0, MODE_BOTH );
		m_ptMtTable->SetMtMulticasting( tMt.GetMtId(), FALSE );
	}
	

	//发Trap消息
	TMtNotify tMtNotify;
	CConfId cConfId;
	TMtAlias tMtAlias;
	u8 byLen = 0;
	cConfId = m_tConf.GetConfId( );
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
	
	// 挂断或删除下级MCU则把下级终端从轮询列表中删除, MCS自己维护列表，不用通知, zgc, 2007-03-29
	if( MT_TYPE_SMCU == tMt.GetMtType() )
	{
		if( POLL_STATE_NONE != m_tConf.m_tStatus.GetPollState() )
		{
			TMtPollParam atMtPollNew[MAXNUM_CONF_MT];
            TMtPollParam tTmpMt;
			u8 byPos;
            u8 byNewNum= 0;
			BOOL32 bPollingMtInSMcu = ( m_tConf.m_tStatus.GetMtPollParam().GetMcuId() == tMt.GetMtId() ) ? TRUE : FALSE ;
			u8 byNextPos = bPollingMtInSMcu ? 0xFF : 0;     // 下一个要轮询的终端
			for( byPos = 0; byPos < m_tConfPollParam.GetPolledMtNum(); byPos++ )
			{
                tTmpMt = *(m_tConfPollParam.GetPollMtByIdx(byPos));
				if( tMt.GetMtId() != tTmpMt.GetMcuId() )
				{
                    atMtPollNew[byNewNum] = tTmpMt;
					if( byPos > m_tConfPollParam.GetCurrentIdx() && byNextPos == 0xFF )
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
				if( byNextPos == 0xFF )
				{
					byNextPos = m_tConfPollParam.GetPolledMtNum();
				}
				m_tConfPollParam.SpecPollPos( byNextPos );
				SetTimer( MCUVC_POLLING_CHANGE_TIMER, 10 );
			}
		}
	}
    
	//zbq [12/05/2007] 清选看
    StopSelectSrc(tMt, MODE_BOTH, FALSE);
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
    if ( ROLLCALL_MODE_NONE != m_tConf.m_tStatus.GetRollCallMode() )
    {
        if ( tMt == m_tRollCaller )
        {
            NotifyMcsAlarmInfo( 0, ERR_MCU_ROLLCALL_CALLERLEFT );
            RollCallStop(cServMsg);
        }
        else if ( tMt == GetLocalMtFromOtherMcuMt(m_tRollCallee) )
        {
            NotifyMcsAlarmInfo( 0, ERR_MCU_ROLLCALL_CALLEELEFT );
            RollCallStop(cServMsg);
        }
    }

	TConfMtInfo *pcInfo = m_tConfAllMtInfo.GetMtInfoPtr(tMt.GetMtId());
	if(pcInfo != NULL)
	{
		pcInfo->SetNull();
	}

	NotifyMcuDropMt(tMt);
	
	//将此级联mcu信息移除出列表
	m_ptConfOtherMcTable->RemoveMcInfo( tMt.GetMtId() );
	m_tConfAllMtInfo.RemoveMcuInfo( tMt.GetMtId(), tMt.GetConfIdx() );

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
        //释放对应高清适配资源
        StopHDCascaseAdp();

		m_tCascadeMMCU.SetNull();
		m_tConfAllMtInfo.m_tMMCU.SetNull();
		m_tConfInStatus.SetNtfMtStatus2MMcu(FALSE);
	}

    // 通知终端离线原因
	// fxh 在RestoreAllSubMtJoinedConfWatchingSrcMt前设置正确的VCS会议状态,是否有当前调度终端
    MtOnlineChange( tMt, FALSE, byReason );	
	//移除
	m_tConfAllMtInfo.RemoveJoinedMt( tMt.GetMtId() );
	
	RestoreAllSubMtJoinedConfWatchingSrcMt( tMt );

	StopSwitchToAllMcWatchingSrcMt( tMt );
	StopSwitchToAllPeriEqpWatchingSrcMt( tMt );


	//清除逻辑通道
	m_ptMtTable->ClearMtLogicChnnl( tMt.GetMtId() );
    m_ptMtTable->SetMtType( tMt.GetMtId(), MT_TYPE_NONE );
    //清状态
    TMtStatus tMtStatus;
    if ( m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tMtStatus) )
    {
        tMtStatus.Clear();
		tMtStatus.SetMtId(tMt.GetMtId());
		tMtStatus.SetMcuId(tMt.GetMcuId());
        m_ptMtTable->SetMtStatus(tMt.GetMtId(), &tMtStatus);
		MtStatusChange(tMt.GetMtId(), TRUE);
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
	SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
	
	//发给会控会议所有终端信息
	cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
	
	//通知所有终端
	cServMsg.SetMsgBody( ( u8 * )&tMt, sizeof( tMt ) );	
	BroadcastToAllSubMtJoinedConf( MCU_MT_MTLEFT_NOTIF, cServMsg );

    // xsl [10/11/2006] 释放端口
	g_cMcuVcApp.ReleaseMtPort( m_byConfIdx, tMt.GetMtId() );
    m_ptMtTable->ClearMtSwitchAddr( tMt.GetMtId() );

    // zbq [02/18/2008] 降速终端离会，恢复VMP广播的码率
    if ( m_tConf.m_tStatus.IsBrdstVMP() &&
         m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
         m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()) )
    {
        AdjustVmpBrdBitRate();
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
        m_cMtRcvGrp.RemoveMem(tMt.GetMtId(), tMainCap);
        m_cMtRcvGrp.RemoveMem(tMt.GetMtId(), tSecCap);
        m_cMtRcvGrp.RemoveMem(tMt.GetMtId(), tDCap);
    }

#ifdef _SATELITE_
    RefreshConfState();
#endif

	//打印消息
	CallLog( "Mt%d: 0x%x(Dri:%d) droped from conf for the reason.%d!\n", 
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
        ptMtStatus->SetVideoFreeze(tMtOldStatus.IsVideoFreeze());
        ptMtStatus->SetSelByMcsDrag(tMtOldStatus.GetSelByMcsDragMode());
        ptMtStatus->m_tVideoMt  = tMtOldStatus.m_tVideoMt;
        ptMtStatus->m_tAudioMt  = tMtOldStatus.m_tAudioMt;
        ptMtStatus->m_tRecState = tMtOldStatus.m_tRecState;
        ptMtStatus->SetSendVideo(tMtOldStatus.IsSendVideo());

        // 增加双流状态标识 [02/05/2007-zbq]
        ptMtStatus->SetSndVideo2( m_tDoubleStreamSrc.GetType() == ptMtStatus->GetType() && 
                                  m_tDoubleStreamSrc.GetMtId() == ptMtStatus->GetMtId() );

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
        MtStatusChange();

        break;

    default:
        ConfLog(FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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

#ifndef _SATELITE_
    TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
    TMtAlias tMtH323Alias = *(TMtAlias*)cServMsg.GetMsgBody();
    TMtAlias tMtAddr = *((TMtAlias*)cServMsg.GetMsgBody()+1);
    u8 byType = *(u8*)( cServMsg.GetMsgBody() + sizeof(TMtAlias)*2 );
    u8 byEncrypt = *(u8*)( cServMsg.GetMsgBody() + sizeof(TMtAlias)*2 +1);
#else
    TMt	tMt = *(TMt*)cServMsg.GetMsgBody();
    TTransportAddr tAddr = *(TTransportAddr*)(cServMsg.GetMsgBody() + sizeof(TMt));
    u8 byType = *(u8*)( cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(TTransportAddr) );

    TMtAlias tMtH323Alias;
    TMtAlias tMtAddr;
    u8 byEncrypt = CONF_ENCRYPTMODE_NONE;
#endif

	if(byEncrypt == 0 && m_tConf.GetConfAttrb().GetEncryptMode() != CONF_ENCRYPTMODE_NONE)
	{
		//drop call
		cServMsg.SetMsgBody();
		SendMsgToMt( tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg );
		return;
	}
	//设置终端类型
	if(byType == TYPE_MCU )
	{
		m_ptMtTable->SetMtType( tMt.GetMtId(), MT_TYPE_SMCU );
	}
	else
	{
		m_ptMtTable->SetMtType( tMt.GetMtId(), MT_TYPE_MT); 
	}

	//设置终端别名
	m_ptMtTable->SetMtAlias( tMt.GetMtId(), &tMtAddr );
	m_ptMtTable->SetMtAlias( tMt.GetMtId(), &tMtH323Alias);

	//设置终端IP地址
	if( tMtAddr.m_AliasType == mtAliasTypeTransportAddress )
	{
		m_ptMtTable->SetIPAddr( tMt.GetMtId(), tMtAddr.m_tTransportAddr.GetIpAddr() );
	}

	//标记终端为被邀加入
	//m_ptMtTable->SetMtCallingIn( tMt.GetMtId(), FALSE );

	SendMtAliasToMcs( tMt );
    
#ifdef _SATELITE_
    //卫星会议终端注册成功后，作如下处理
    
    //1、模拟终端终端入会
    u8 byMaster = FALSE;    //暂不考虑级联的情况
    CServMsg cMsg;
    cMsg.SetSrcMtId(tMt.GetMtId());
    cMsg.SetMsgBody((u8*)&byMaster, sizeof(u8));
    OspPost( MAKEIID(GetAppID(), GetInsID()),
             MT_MCU_MTJOINCONF_NOTIF,
             cMsg.GetServMsg(), cMsg.GetServMsgLen() );

    //2、发送会议能力集，触发打开逻辑通道
    TCapSupport tCap = m_tConf.GetCapSupport();
	TCapSupportEx tCapEx = m_tConf.GetCapSupportEx();

    cMsg.SetMsgBody();
    cMsg.SetConfIdx( m_byConfIdx );
    cMsg.SetMsgBody( ( u8 * )&tMt, sizeof( tMt ) );
    cMsg.CatMsgBody( ( u8 * )&tCap, sizeof( TCapSupport ) );
	cMsg.CatMsgBody( ( u8 * )&tCapEx, sizeof( TCapSupportEx ) );
    SendMsgToMt( tMt.GetMtId(),
                 MCU_MT_CAPBILITYSET_NOTIF, cMsg );

	BOOL32 bMtHd = ::topoGetMtInfo( LOCAL_MCUID, tMt.GetMtId(), g_atMtTopo, g_wMtTopoNum ).IsMtHd();

	//主流能力整理
	if (!bMtHd &&
		MEDIA_TYPE_H264 == tCap.GetMainVideoType())
	{
		if (tCap.GetSecVideoType() != MEDIA_TYPE_NULL &&
			tCap.GetSecVideoType() != 0)
		{
			TSimCapSet tSimCap = tCap.GetSecondSimCapSet();
			tCap.SetMainSimCapSet(tSimCap);
			
			//模拟单音频能力交叉交互
			u8 byMainAudType = tCap.GetMainAudioType();
			if (tCap.GetMainAudioType() == MEDIA_TYPE_NULL)
			{
				TSimCapSet tSimCapSet = tCap.GetMainSimCapSet();
				tSimCapSet.SetAudioMediaType(byMainAudType);
				tCap.SetMainSimCapSet(tSimCapSet);
			}
		}
	}


	//双流能力整理
	if (!bMtHd && tCapEx.IsDDStreamCap())
	{
		TDStreamCap tDStreamCap;
		tDStreamCap.SetMediaType(tCapEx.GetSecDSType());
		tDStreamCap.SetResolution(tCapEx.GetSecDSRes());
		tDStreamCap.SetFrameRate(tCapEx.GetSecDSFrmRate());
		tDStreamCap.SetMaxBitRate(tCapEx.GetSecDSBitRate());
		tDStreamCap.SetSupportH239(tCap.IsDStreamSupportH239());
		
		tCap.SetDStreamCapSet(tDStreamCap);
	}


    //3、模拟写入终端能力集
    m_ptMtTable->SetMtCapSupport(tMt.GetMtId(), &tCap);

    //4、通知终端入会, 三次通知
    RefreshConfState();
	g_cMcuVcApp.SetConfRefreshTimes(m_byConfIdx, 3);
#endif
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

    CallLog("Mt.%d Invite Nack because of Reason \"%s\"(%d), ErrCode.%u.\n", tMt.GetMtId(), 
             GetMtLeftReasonStr(byReason), byReason, wErrorCode );
    
    CallFailureNotify(cServMsg);
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

		if( tMt.GetMcuId() != LOCAL_MCUID )
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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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


	switch( CurState() )
	{
	case STATE_ONGOING:

	    SendReplyBack( cServMsg, pcMsg->event + 1 );

		//得到MCU
		tMcu = *(TMcu*)cServMsg.GetMsgBody();         

		//所有终端状态
		if( tMcu.GetMcuId() == 0 && m_ptConfOtherMcTable != NULL)
		{
            // 非本级只有一个别名
            byAliasNum = 1;
            u8 byAliasType = mtAliasTypeH323ID;

			//先发其它MC
			for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
			{                
				TConfMcInfo *ptConfMcInfo = &(m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]);
				if( ptConfMcInfo->m_byMcuId == 0 )
				{
					continue;
				}

                // McuID
                ptrBuf = abyBuf ;
                memcpy(ptrBuf, &(ptConfMcInfo->m_byMcuId), sizeof(u8));

                // 跳过MtNum
                ptrBuf += 2;
                wBufLen = 2;

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

			//本级的最后发
			tMcu.SetMcu( LOCAL_MCUID );
		}

		//不是本级的MCU
		if( ! tMcu.IsLocal() && m_ptConfOtherMcTable != NULL)
		{
			TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMcu.GetMcuId());
			if( ptConfMcInfo == NULL )
			{
				return;
			}

            byAliasNum = 1;
            u8 byAliasType = mtAliasTypeH323ID;

            // McuID
            ptrBuf = abyBuf ;
            memcpy(ptrBuf, &ptConfMcInfo->m_byMcuId, sizeof(u8));

            // 跳过MtNum
            ptrBuf += 2;
            wBufLen = 2;

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
            abyBuf[0] = LOCAL_MCUID;

            // 跳过MtNum
            ptrBuf += 2;
            wBufLen = 2;

            byValidMtNum = 0;
            
            u8* ptrBufTmp = NULL;
            TMtAlias tMtAlias;
            u8 byAliasNum = 0;
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
            abyBuf[1] = byValidMtNum;
            cServMsg.SetMsgBody( abyBuf, wBufLen );
            SendReplyBack( cServMsg, MCU_MCS_ALLMTALIAS_NOTIF );
		}

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
        ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
                ConfLog( FALSE, "[GetMtExtInfo] Mt.%d verInfo unexist, nack\n", tMt.GetMtId() );
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
                TMt tMt = m_ptMtTable->GetMt(byMtId);
            
                if ( 0 == m_ptMtTable->GetHWVerID(tMt.GetMtId()) ||
                     NULL == m_ptMtTable->GetSWVerID(tMt.GetMtId()))
                {
                    ConfLog( FALSE, "[GetMtExtInfo] Mt.%d verInfo unexist, nack\n", tMt.GetMtId() );
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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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

			TRY_MEM_COPY(pbyTmp, (void*)atMtAlias[i].m_achAlias, ntohs(aliasLen), pbyMemLimit,bResult);
			if( !bResult )
				return (pbyTmp - pbyBuf);
			pbyTmp += ntohs(aliasLen);
		}
		else if(type = mtAliasTypeTransportAddress)
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
	TMt			tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	TMt			tChairman;

	switch( CurState() )
	{
	case STATE_ONGOING:

		//未与会
		if( !m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
		{
			return;
		}
				
		//有主席
		if( HasJoinedChairman() )
		{
			tChairman = m_tConf.GetChairman();
			
			//主席申请发言,同意
			if( tChairman.GetMtId() == tMt.GetMtId() )
			{
                // xsl [8/22/2006]卫星分散会议时需要判断回传通道数
                if (m_tConf.GetConfAttrb().IsSatDCastMode() && IsOverSatCastChnnlNum(tMt.GetMtId()))
                {
                    ConfLog(FALSE, "[ProcMtMcuApplySpeakerNotif] over max upload mt num. ignore it!\n");
                }
                else
                {
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
                ConfLog(FALSE, "[ProcMtMcuApplySpeakerNotif] not permit send message to chair mt for mt apply speaker\n");
            }
		}        

		//通知会控
		cServMsg.SetMsgBody((u8*)&tMt, sizeof(tMt));
		SendMsgToAllMcs( MCU_MCS_MTAPPLYSPEAKER_NOTIF, cServMsg );

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

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
                ConfLog(FALSE, "[ProcMtMcuApplyChairmanReq] not permit send message to chair mt for mt apply chair\n");
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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
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

    CallLog("[DaemonProcMtMcuApplyJoinReq] ConfNum.%d\n", byNum);

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
				g_cMcuVcApp.SendMsgToConf( ptConfInfo->GetConfId(), pcMsg->event, pcMsg->content, pcMsg->length );                
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

	// 准备会议列表
	cServMsg.SetMsgBody( );
    
	for (u8 byConfIdx1 = MIN_CONFIDX; byConfIdx1 <= MAX_CONFIDX; byConfIdx1++)
	{
		if (NULL != g_cMcuVcApp.GetConfInstHandle(byConfIdx1))
		{
			ptConfInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx1 )->m_tConf;
            
			if ( CONF_OPENMODE_CLOSED != ptConfInfo->GetConfAttrb().GetOpenMode() && // 配置允许终端入会
                 ( byType == TYPE_MCU ||    //admin用户创的会议允许级联，不允许终端呼入
                   g_cMcuVcApp.IsMtIpInAllowSeg( ptConfInfo->GetUsrGrpId(), tMtAddr.m_tTransportAddr.GetNetSeqIpAddr() ) )  ) // 或者终端在该组的允许段
			{
                nConfNum ++;

				strncpy( tConfList.achConfName, ptConfInfo->GetConfName(), sizeof( tConfList.achConfName ) );
				tConfList.achConfName[sizeof( tConfList.achConfName) - 1 ] = 0;					
				tConfList.m_cConfId = ptConfInfo->GetConfId();
				cServMsg.CatMsgBody( (u8*)&tConfList, sizeof( tConfList) );	

                CallLog("[DaemonProcMtMcuApplyJoinReq] conf list. Index.%d, confname.%s\n", 
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
        g_cMcuVcApp.SendMsgToConf( tConfList.m_cConfId, pcMsg->event, pcMsg->content, pcMsg->length );
        return;
    }
    else
    {
        // 发送终端列表
	    cServMsg.SetErrorCode( ERR_MCU_NULLCID );
	    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
        CallLog("[DaemonProcMtMcuApplyJoinReq] send conf list to mt. nConfNum.%d\n", nConfNum);
        return;
    }

    // 开始缺省会议
    if (bStartDefConf)
    {
		CallLog("[DaemonProcMtMcuApplyJoinReq] start def conf!\n");
		TConfStore tConfStore;
		TConfStore tConfStoreBuf;
		TPackConfStore *ptPackConfStore = (TPackConfStore *)&tConfStoreBuf;
		//提取后的 tConfStore 为 TPackConfStore 结构，即已进行Pack处理的会议数据

		//根据缺省会议创会
		if (!GetConfFromFile(MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE, ptPackConfStore))
		{
			cServMsg.SetErrorCode(ERR_MCU_NULLCID);
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);

            ConfLog(FALSE, "Conference %s failure because Get Conf.%d From File failed!\n", 
                            tConfStore.m_tConfInfo.GetConfName(), MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE);
			return;
		}
		//将 已进行Pack处理的会议数据 进行UnPack处理
		if (!UnPackConfStore(ptPackConfStore, tConfStore))
		{
			cServMsg.SetErrorCode(ERR_MCU_NULLCID);
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);

            ConfLog(FALSE, "Conference %s failure because UnPackConf.%d From File failed!\n", 
                            tConfStore.m_tConfInfo.GetConfName(), MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE);
            return;
		}

		if (0 == byEncrypt && 
			CONF_ENCRYPTMODE_NONE != tConfStore.m_tConfInfo.GetConfAttrb().GetEncryptMode())
		{
			cServMsg.SetErrorCode(ERR_MCU_SETENCRYPT);
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);

			ConfLog(FALSE, "Conference %s failure because encrypt setting is confused - byEncrypt.%d EncryptMode.%d!\n", 
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
			else 
			{
				if (mtAliasTypeE164 == tConfStore.m_atMtAlias[byLoop].m_AliasType)
				{
					//如果与建会终端一致，则不重复添加
					if (0 == memcmp(tConfStore.m_atMtAlias[byLoop].m_achAlias, tMtE164Alias.m_achAlias, MAXLEN_ALIAS))
					{
						bRepeatThisLoop = TRUE;
						bInMtTable = TRUE;
						atMtAlias[0] = tMtE164Alias;
					}		
				} 
				else 
				{
					if (mtAliasTypeH323ID == tConfStore.m_atMtAlias[byLoop].m_AliasType)
					{
						//如果与建会终端一致，则不重复添加
						if (0 == memcmp(tConfStore.m_atMtAlias[byLoop].m_achAlias, tMtH323Alias.m_achAlias, MAXLEN_ALIAS))
						{
							bRepeatThisLoop = TRUE;
							bInMtTable = TRUE;
							atMtAlias[0] = tMtH323Alias;
						}
					}
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
                                  tNewTvwallModule, tNewVmpModule);            
		}

		//终端数超过 MAXNUM_CONF_MT，则舍弃最末尾一个终端
		if (byMtNumInUse > MAXNUM_CONF_MT)
		{
			byMtNumInUse = MAXNUM_CONF_MT;
		}

		//终端别名数组打包 
		s8  achAliasBuf[SERV_MSG_LEN];
		u16 wAliasBufLen = 0;
		PackTMtAliasArray(atMtAlias, awMtDialBitRate, byMtNumInUse, achAliasBuf, wAliasBufLen);
		wAliasBufLen = htons(wAliasBufLen);

		cServMsg.SetMsgBody((u8 *)&tConfStore.m_tConfInfo, sizeof(TConfInfo));
		cServMsg.CatMsgBody((u8 *)&wAliasBufLen, sizeof(wAliasBufLen));
		cServMsg.CatMsgBody((u8 *)achAliasBuf, ntohs(wAliasBufLen));
		if (tConfStore.m_tConfInfo.GetConfAttrb().IsHasTvWallModule())
		{
			cServMsg.CatMsgBody((u8*)&tNewTvwallModule, sizeof(TMultiTvWallModule));
		}
		if (tConfStore.m_tConfInfo.GetConfAttrb().IsHasVmpModule())
		{
			cServMsg.CatMsgBody((u8*)&tNewVmpModule, sizeof(TVmpModule));
		}
		cServMsg.CatMsgBody((u8 *)&tMtH323Alias, sizeof(tMtH323Alias));
		cServMsg.CatMsgBody((u8 *)&tMtE164Alias, sizeof(tMtE164Alias));
		cServMsg.CatMsgBody((u8 *)&tMtAddr, sizeof(tMtAddr));
		cServMsg.CatMsgBody((u8 *)&byType, sizeof(byType));
		// xliang [12/26/2008] (modify for MT call MCU initially) add bInMtTalbe Info
		cServMsg.CatMsgBody((u8*)&bInMtTable,sizeof(bInMtTable));
		//print test
		CallLog("bInMtTable is %d\n",bInMtTable);

        u8 byInsID = AssignIdleConfInsID();
        if(0 != byInsID)
        {
            ::OspPost(MAKEIID(AID_MCU_VC, byInsID), MT_MCU_CREATECONF_REQ,
                      cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
        }
        else
        {
            ConfLog(FALSE, "[DaemonProcMtMcuApplyJoinReq] assign instance id failed!\n");
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
			
			CallLog( "Mt 0x%x join conf %s request was refused because encrypt!\n", 
				     tMtAddr.m_tTransportAddr.GetIpAddr(), m_tConf.GetConfName() );
            return;
		}
        
        // byMtId = m_ptMtTable->GetMtIdByAlias( &tMtAddr );

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
                     tMtE164Alias == tDialAlias &&
                     //即使有呼叫E164别名匹配，也要校验一下Ip，防止未注册GK(或注册了不同的非邻居GK)的终端冒名顶替
                     m_ptMtTable->GetMtAlias(byMtIdx, mtAliasTypeTransportAddress, &tInConfMtAddr) &&
                     tInConfMtAddr.m_tTransportAddr.GetIpAddr() == tMtAddr.m_tTransportAddr.GetIpAddr())
                {
                    byMtId = byMtIdx;
                    tFstAlias = tMtE164Alias;
                    break;
                }
                else if ( mtAliasTypeH323ID == tDialAlias.m_AliasType &&
                          tMtH323Alias == tDialAlias &&
                          //即使有呼叫E164别名匹配，也要校验一下Ip，防止未注册GK(或注册了不同的非邻居GK)的终端冒名顶替
                          m_ptMtTable->GetMtAlias(byMtIdx, mtAliasTypeTransportAddress, &tInConfMtAddr) &&
                          tInConfMtAddr.m_tTransportAddr.GetIpAddr() == tMtAddr.m_tTransportAddr.GetIpAddr())
                {
                    byMtId = byMtIdx;
                    tFstAlias = tMtH323Alias;
                    break;
                }
                else if( mtAliasTypeTransportAddress == tDialAlias.m_AliasType &&
                         tMtAddr.m_tTransportAddr.GetIpAddr() == tDialAlias.m_tTransportAddr.GetIpAddr() )
                {
                    byMtId = byMtIdx;
                    tFstAlias = tMtAddr;
                    break;
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

			    CallLog( "Mt 0x%x join conf %s request was refused because conf is closed!\n", 
					      tMtAddr.m_tTransportAddr.GetIpAddr(), m_tConf.GetConfName() );
			    return;
		    }

            // xsl [8/2/2006] 允许mcu级联呼叫
            if ( byType != TYPE_MCU )
            {
                //本会议没有配置允许该地址的终端加入, 拒绝
				// xliang [12/25/2008] 取消用户组限制
//                 if ( !g_cMcuVcApp.IsMtIpInAllowSeg( m_tConf.GetUsrGrpId(), tMtAddr.m_tTransportAddr.GetNetSeqIpAddr()) )
//                 {
//                     cServMsg.SetConfIdx( m_byConfIdx );
//                     cServMsg.SetConfId( m_tConf.GetConfId() );
//                     
//                     g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
//                     
//                     CallLog( "Mt 0x%x join conf %s request was refused because usr group %d not allowed!\n", 
//                               tMtAddr.m_tTransportAddr.GetIpAddr(), m_tConf.GetConfName(), m_tConf.GetUsrGrpId() );
//                     return;
//                 }
//                 else
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
   
		wCallRate -= GetAudioBitrate( m_tConf.GetMainAudioMediaType() );               
        
		//分配终端ID号
		byMtId = AddMt( tFstAlias, wCallRate, CONF_CALLMODE_NONE, TRUE );

        // zbq [08/09/2007] 经过允许Ip段呼入,但呼叫别名又不是Ip的终端,此处需保存其Ip
        if ( bMtCallingInWithSegIp && !(tFstAlias == tMtAddr) )
        {
            m_ptMtTable->SetMtAlias( byMtId, &tMtAddr );
            // guzh [10/29/2007] 保存TMtExt 里面的IP地址
            m_ptMtTable->SetIPAddr( byMtId, tMtAddr.m_tTransportAddr.GetIpAddr() );
        }
        
        // xsl [11/8/2006] 接入板终端计数加1
		// xliang [2/14/2009] 区分是MT还是MCU,补计数
		u8 byDriId = cServMsg.GetSrcDriId();
        g_cMcuVcApp.IncMtAdpMtNum( cServMsg.GetSrcDriId(), m_byConfIdx, byMtId );
        m_ptMtTable->SetDriId(byMtId, byDriId);
		if(byType == TYPE_MCU)
		{
			//接入板的终端计数还要+1
			//在mtadplib那端已通过判断，所以肯定有剩余接入点让其占用
			g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_wMtNum++;
		}
		
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
				m_ptMtTable->SetMtType( byMtId, MT_TYPE_MMCU );
			}
			else if(byType = TYPE_MT)
			{
				m_ptMtTable->SetMtType( byMtId, MT_TYPE_MT );
			}
			
			m_ptMtTable->SetMtAlias(byMtId, &tMtH323Alias);
			m_ptMtTable->SetMtAlias(byMtId, &tMtE164Alias);
			
			cServMsg.SetConfIdx( m_byConfIdx );
			cServMsg.SetConfId( m_tConf.GetConfId() );
			cServMsg.SetDstMtId( byMtId );
			cServMsg.SetMsgBody(&byEncrypt, sizeof(u8));
			TCapSupport tCap = m_tConf.GetCapSupport();
			cServMsg.CatMsgBody( (u8*)&tCap, sizeof(tCap));
			TMtAlias tAlias;
			tAlias.SetH323Alias(m_tConf.GetConfName());
			cServMsg.CatMsgBody((u8 *)&tAlias, sizeof(tAlias));
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+1, cServMsg);			
		}
		else
		{
			cServMsg.SetConfIdx( m_byConfIdx );
			cServMsg.SetConfId( m_tConf.GetConfId() );
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);

			CallLog( "Mt 0x%x join conf %s request was refused because conf full!\n", 
					  tMtAddr.m_tTransportAddr.GetIpAddr(), m_tConf.GetConfName() );
		}

		break;
    }
	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
	TSwitchInfo	tSwitchInfo;
	TMt	tDstMt, tSrcMt;
	u8  byDstMtId;
	CServMsg  cMsg( pcMsg->content, pcMsg->length );
	TMtStatus tSrcMtStatus;
    TMtStatus tDstMtStatus;    

    tSwitchInfo = *( TSwitchInfo * )cServMsg.GetMsgBody();
    tSrcMt = tSwitchInfo.GetSrcMt();
    tDstMt = tSwitchInfo.GetDstMt();
	
	// xliang [4/2/2009] 取消主席选看VMP的状态
	if(tDstMt == m_tConf.GetChairman() 
		&& m_tConf.m_tStatus.IsVmpSeeByChairman() )
	{
		m_tConf.m_tStatus.SetVmpSeebyChairman(FALSE);
		// 状态同步刷新到TPeriStatus中
		TPeriEqpStatus tVmpStatus;
		g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tVmpStatus );
		tVmpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
		g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tVmpStatus );

		// xliang [4/14/2009] 主席选看VMP的交换停掉
		StopSelectSrc(m_tConf.GetChairman(), MODE_VIDEO);
	}

	//防止SendReplyBack将消息返回应答，将Src设为NULL
	if( pcMsg->event == MT_MCU_STARTSELMT_CMD )
	{
		cServMsg.SetNoSrc();
        cServMsg.SetSrcMtId(0);
	}

	//如果不是会控选看终端，强制广播下的非主席选看，NACK
	if( pcMsg->event != MCS_MCU_STARTSWITCHMC_REQ && 
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
        ConfLog(FALSE, "Mt(%d,%d) Select see Mt(%d,%d), returned with ack!\n", 
                tDstMt.GetMcuId(), 
                tDstMt.GetMtId(),
                tSrcMt.GetMcuId(), 
                tSrcMt.GetMtId());

        SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
        return;
    }

	if( tDstMt.GetType() == TYPE_MT)
	{
		ConfLog( FALSE, "Mt(%d,%d) select see Mt(%d,%d)\n", tDstMt.GetMcuId(), tDstMt.GetMtId(), tSrcMt.GetMcuId(), tSrcMt.GetMtId() );

		tDstMt = GetLocalMtFromOtherMcuMt( tDstMt );				
		tDstMt = m_ptMtTable->GetMt(tDstMt.GetMtId());
		tSwitchInfo.SetDstMt( tDstMt );		
	}

    TMt tSrcMtSMcu = tSrcMt;    // 记录下标准的选看源，可能是下级终端
	tSrcMt = GetLocalMtFromOtherMcuMt(tSrcMt);
	tSwitchInfo.SetSrcMt( tSrcMt );
	byDstMtId = tDstMt.GetMtId();

    //源终端未入会议，NACK
    if( !m_tConfAllMtInfo.MtJoinedConf( tSrcMt.GetMtId() ) &&
	    m_tConf.GetConfSource() != VCS_CONF)
    {
        ConfLog( FALSE, "Select source Mt(%u,%u) has not joined current conference!\n",
                 tSrcMt.GetMcuId(), tSrcMt.GetMtId() );
        cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        return;
    }    
		
    //终端状态和选看模式不匹配, NACK
    // zgc, 2008-07-10, 修改判断方式, 当最终过滤结果为MODE_NONE时，拒绝选看，否则给出提示
	u8 bySwitchMode = tSwitchInfo.GetMode();	// 记录初始MODE

	m_ptMtTable->GetMtStatus(tSrcMt.GetMtId(), &tSrcMtStatus);
    m_ptMtTable->GetMtStatus(byDstMtId, &tDstMtStatus);
	if( (!tSrcMtStatus.IsSendVideo() ||
         (tDstMt.GetType() == TYPE_MT && !tDstMtStatus.IsReceiveVideo()) ) &&
         (bySwitchMode == MODE_VIDEO || bySwitchMode == MODE_BOTH) )
	{
        bySwitchMode = ( MODE_BOTH == bySwitchMode ) ? MODE_AUDIO : MODE_NONE;
	}	
    
    if( (!tSrcMtStatus.IsSendAudio() ||
         (tDstMt.GetType() == TYPE_MT && !tDstMtStatus.IsReceiveAudio()) ) &&
        ( bySwitchMode == MODE_AUDIO || bySwitchMode == MODE_BOTH ) )
    {
        bySwitchMode = ( MODE_BOTH == bySwitchMode ) ? MODE_VIDEO : MODE_NONE;
    }

    if ( MODE_NONE == bySwitchMode )
    {
        // 请求的选看模式完全被拒绝，回NACK
        cServMsg.SetErrorCode( ERR_MCU_SRCISRECVONLY );
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        return;
	}


    //zbq[05/22/2009] 终端选看适配支持 FIXME: 暂未处理标清适配的动态抢占
    BOOL32 bSelAccordAdp  = FALSE;

    //终端选看
    if( TYPE_MT == tDstMt.GetType() )
    {
        //取源终端与目的终端的能力集
        TSimCapSet tSrcSCS = m_ptMtTable->GetSrcSCS( tSrcMt.GetMtId() );
        TSimCapSet tDstSCS = m_ptMtTable->GetDstSCS( byDstMtId );
        
        //选看模式和对应能力集不匹配, NACK
        if ( !IsSelModeAndCapMatched( bySwitchMode, tSrcSCS, tDstSCS, bSelAccordAdp) ) 
        {
            cServMsg.SetErrorCode( ERR_MCU_NOTSAMEMEDIATYPE );
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            return;
        }
	}
	
	// 给MCS发通知
	u8 byMcsId = 0;
	if ( pcMsg->event == MCS_MCU_STARTSWITCHMT_REQ || 
		 pcMsg->event == MCS_MCU_STARTSWITCHMC_REQ )
	{
		byMcsId = cServMsg.GetSrcSsnId();
	}

    if ( MODE_VIDEO == bySwitchMode && MODE_BOTH == tSwitchInfo.GetMode() )
	{
		NotifyMcsAlarmInfo( byMcsId, ERR_MCU_SELMODECHANGE_AUDIOFAIL );
	}

    if ( MODE_AUDIO == bySwitchMode && MODE_BOTH == tSwitchInfo.GetMode() )
	{
		NotifyMcsAlarmInfo( byMcsId, ERR_MCU_SELMODECHANGE_VIDEOFAIL );
	}


    tSwitchInfo.SetMode( bySwitchMode );

    switch( cServMsg.GetEventId() )
    {
	case MCS_MCU_STARTSWITCHMT_REQ:
	case MT_MCU_STARTSELMT_CMD:
	case MT_MCU_STARTSELMT_REQ:
        {
            //目的终端未入会议，NACK
            if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
            {
                ConfLog( FALSE, "Specified Mt(%u,%u) not joined conference!\n", 
                    tDstMt.GetMcuId(), tDstMt.GetMtId() );
                cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }            
            
            // xsl [7/20/2006] 卫星分散会议时需要判断回传通道数
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
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
                else if (IsOverSatCastChnnlNum(tSrcMt.GetMtId()))
                {
                    bOverNum = TRUE;
                }

                if (bOverNum)
                {
                    ConfLog(FALSE, "[ProcMtMcuStartSwitchMtReq] over max upload mt num. nack!\n");
                    cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    return;
                }
            }
        
			//混音时调整选看模式
			if( m_tConf.m_tStatus.IsMixing() )
			{
                //选看音频，NACK
				if( tSwitchInfo.GetMode() == MODE_AUDIO  )
				{
					ConfLog( FALSE, "CMcuVcInst: Conference %s is mixing now. Cannot switch only audio!\n", 
                             m_tConf.GetConfName() );
					cServMsg.SetErrorCode( ERR_MCU_SELAUDIO_INMIXING );
					SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    return;
				}
                //其他，转为选看视频
				else
				{
                    //选看音视频，提示音频选看失败
                    if ( MODE_BOTH == tSwitchInfo.GetMode() && cServMsg.GetEventId() == MCS_MCU_STARTSWITCHMT_REQ )
                    {
                        ConfLog( FALSE, "CMcuVcInst: Conference %s is mixing now. Cannot switch audio!\n", 
                                 m_tConf.GetConfName() );
                        cServMsg.SetErrorCode( ERR_MCU_SELBOTH_INMIXING );
                        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );                        
                    }

					tSwitchInfo.SetMode( MODE_VIDEO );
				}
			}
            
			//需要适配终端不能选看
            //zbq[05/22/2009] 需要适配的选看不调整码率
            if (g_cMcuVcApp.IsSelAccord2Adp() && bSelAccordAdp)
            {
                //稍后动态抢占适配器
            }
            else
            {
                u16 wSrcMtBitrate   = m_ptMtTable->GetMtSndBitrate(tSrcMt.GetMtId());
                u16 wDstMtBitrate   = m_ptMtTable->GetMtReqBitrate(tDstMt.GetMtId());
                u16 wSrcDialBitrate = m_ptMtTable->GetSndBandWidth(tSrcMt.GetMtId());
                u16 wDstDialBitrate = m_ptMtTable->GetRcvBandWidth(tDstMt.GetMtId());      
                
                if (wSrcDialBitrate <= wDstDialBitrate && wSrcMtBitrate == wSrcDialBitrate &&
                    !m_tDoubleStreamSrc.IsNull())
                {
                    CServMsg cTmpServMsg;
                    TLogicalChannel tLogicalChannel;
                    if (m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
                    {
                        tLogicalChannel.SetFlowControl(GetDoubleStreamVideoBitrate(wSrcDialBitrate));
                        cTmpServMsg.SetMsgBody((u8 *)&tLogicalChannel, sizeof(tLogicalChannel));
                        SendMsgToMt(tSrcMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cTmpServMsg);
                    }                
                }
            }

            //级联选看
            if ( !tSrcMtSMcu.IsLocal() )
            {
                //查看MC
                TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tSrcMtSMcu.GetMcuId());
                if(ptMcInfo == NULL)
                {
                    cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    return;
                }
                //构造选看源
                TMt tMt;
                tMt.SetMcuId(tSrcMtSMcu.GetMcuId());
                tMt.SetMtId(0);
                TMcMtStatus *ptStatus = ptMcInfo->GetMtStatus(tMt);
                if(ptStatus == NULL)
                {
                    cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    return;
                }
            
                OnMMcuSetIn( tSrcMtSMcu, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
            }
        
            // 顾振华@2006.4.17 如果有选看(音频或者视频)，都要记录下来
            if( MODE_AUDIO == tSwitchInfo.GetMode() || 
                MODE_VIDEO == tSwitchInfo.GetMode() ||
                MODE_BOTH  == tSwitchInfo.GetMode() )
            {
                m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tDstMtStatus ); // 选看
                tDstMtStatus.SetSelectMt( tSrcMt, tSwitchInfo.GetMode() );
                m_ptMtTable->SetMtStatus( tDstMt.GetMtId(), &tDstMtStatus );
            }
        
            if ( MT_MCU_STARTSELMT_CMD != cServMsg.GetEventId() )
            {
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
            }                

            //上级MCU
            if( cServMsg.GetEventId() == MCS_MCU_STARTSWITCHMT_REQ &&
                (tDstMt.GetMtType() == MT_TYPE_SMCU || tDstMt.GetMtType() == MT_TYPE_MMCU) )
            {
                TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tDstMt.GetMtId());
                if(ptConfMcInfo != NULL)
                {
                    ptConfMcInfo->m_tSpyMt = tSrcMt;
                    //send output 消息
                    TSetOutParam tOutParam;
                    tOutParam.m_nMtCount = 1;
                    tOutParam.m_atConfViewOutInfo[0].m_tMt = tDstMt;
                    tOutParam.m_atConfViewOutInfo[0].m_nOutViewID = ptConfMcInfo->m_dwSpyViewId;
                    tOutParam.m_atConfViewOutInfo[0].m_nOutVideoSchemeID = ptConfMcInfo->m_dwSpyVideoId;
                    CServMsg cServMsg2;
                    cServMsg2.SetMsgBody((u8 *)&tOutParam, sizeof(tOutParam));
                    cServMsg2.SetEventId(MCU_MCU_SETOUT_NOTIF);
                    SendMsgToMt(tDstMt.GetMtId(), MCU_MCU_SETOUT_NOTIF, cServMsg2);
                }
            }
            
            //for h320 mcu cascade select
            if( MT_MCU_STARTSELMT_REQ == cServMsg.GetEventId() && 
                MT_TYPE_SMCU == m_ptMtTable->GetMtType(tDstMt.GetMtId()) )
            {            
                m_tLastSpyMt = tSrcMt;
            } 
			
			//若被选看的MT在画面合成中，则去尝试占VMP高清前适配通道
			if ( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE 
				&& m_tConf.m_tStatus.GetVmpParam().IsMtInMember( tSrcMt ) )
			{
				TPeriEqpStatus tPeriEqpStatus; 
				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
				
				if(byVmpSubType != VMP) //用的VMP是MPU/EVPU
				{
					TVMPParam	tVMPParam   = m_tConf.m_tStatus.GetVmpParam(); 
					
					// xliang [4/21/2009] 调整分辨率，让其尝试进VMP前适配通道
					u8	byVmpStyle  = tVMPParam.GetVMPStyle();
					u8	byChlPos	= tVMPParam.GetChlOfMtInMember(tSrcMt);	

					ChangeMtVideoFormat(tSrcMt, &tVMPParam, TRUE, TRUE, FALSE); 
				
				}
			}

            // xsl [7/21/2006] 通知终端接收地址
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
                if (MODE_AUDIO == tSwitchInfo.GetMode() || MODE_BOTH == tSwitchInfo.GetMode())
                {
                    ChangeSatDConfMtRcvAddr(tDstMt.GetMtId(), LOGCHL_AUDIO, FALSE);
                }

                if (MODE_VIDEO == tSwitchInfo.GetMode() || MODE_BOTH == tSwitchInfo.GetMode())
                {
                    ChangeSatDConfMtRcvAddr(tDstMt.GetMtId(), LOGCHL_VIDEO, FALSE);
                }                
            }            
        }            
		break;
        
    case MCS_MCU_STARTSWITCHMC_REQ:
        {
            // xsl [7/20/2006] 卫星分散会议时需要判断回传通道数
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {
                if (IsOverSatCastChnnlNum(tSrcMt.GetMtId()))
                {
                    ConfLog(FALSE, "[ProcMtMcuStartSwitchMtReq] over max upload mt num. nack!\n");
                    cServMsg.SetErrorCode( ERR_MCU_DCAST_OVERCHNNLNUM );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    return;
                }
            }
            // guzh [8/25/2006]  如果是级联选看，要切换回传通道
            if ( !tSrcMtSMcu.IsLocal() )
            {
                //查看MC
                TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tSrcMtSMcu.GetMcuId());
                if(ptMcInfo == NULL)
                {
                    cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
                    SendReplyBack( cMsg, cMsg.GetEventId() + 2 );
                    return;
                }
                //构造选看源
                TMt tMt;
                tMt.SetMcuId(tSrcMtSMcu.GetMcuId());
                tMt.SetMtId(0);
                TMcMtStatus *ptStatus = ptMcInfo->GetMtStatus(tMt);
                if(ptStatus == NULL)
                {                    
                    cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
                    SendReplyBack( cMsg, cMsg.GetEventId() + 2 );
                    return;
                }
            
                OnMMcuSetIn( tSrcMtSMcu, cServMsg.GetSrcSsnId(), SWITCH_MODE_SELECT);
            }

            //加密以及动态载荷信息
            TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
            TSimCapSet    tSrcSCS  = m_ptMtTable->GetSrcSCS( tSrcMt.GetMtId() ); 
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

            // zw [06/26/2008] 适应AAC LC格式
			if ( MEDIA_TYPE_AACLC == tSrcSCS.GetAudioMediaType() )
			{
                TAudAACCap tAudAACCap;
                tAudAACCap.SetMediaType(MEDIA_TYPE_AACLC);
                tAudAACCap.SetSampleFreq(AAC_SAMPLE_FRQ_32);
                tAudAACCap.SetChnlType(AAC_CHNL_TYPE_SINGLE);
                tAudAACCap.SetBitrate(96);
                tAudAACCap.SetMaxFrameNum(AAC_MAX_FRM_NUM);
                
				cMsg.CatMsgBody( (u8 *)&tAudAACCap, sizeof(tAudAACCap) );
			}

            SendReplyBack( cMsg, cMsg.GetEventId() + 1 );
        }
        break;
        
    default:
        ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
                 pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
        break;    
    }

    // 这里需要考虑是否是广播交换, zgc, 2008-06-03
    // BUG2354, 需要防止会控监控进入, zgc, 2008-06-12
    if( tSrcMt == m_tVidBrdSrc && cServMsg.GetEventId() != MCS_MCU_STARTSWITCHMC_REQ &&
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
	}

    if (bSelAccordAdp && g_cMcuVcApp.IsSelAccord2Adp())
    {
        if (MODE_BOTH == tSwitchInfo.GetMode())
        {
            tSwitchInfo.SetMode(MODE_AUDIO);
            SwitchSrcToDst(tSwitchInfo, cServMsg);
            tSwitchInfo.SetMode(MODE_VIDEO);
        }
        SwitchSrcToDstAccord2Adp( tSwitchInfo, cServMsg );
    }
    else
    {
	    SwitchSrcToDst( tSwitchInfo, cServMsg );
    }

	
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
	
	ChairmanSeeVmp(tDstMt, cServMsg);
	
	return;

}

/*====================================================================
    函数名      ChairmanSeeVmp
    功能        ：主席选看建交换，置状态的操作
    算法实现    ：
    引用全局变量：
    输入参数说明：TMt tDstMt
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	4/14/2009 	4.6			薛亮          创建
====================================================================*/
void CMcuVcInst::ChairmanSeeVmp(TMt tDstMt, CServMsg &cServMsg)
{
	u8 byDstMtId = tDstMt.GetMtId();
	//非主席终端不让选看VMP
	if (!HasJoinedChairman() ||
		m_tConf.GetChairman().GetMtId() != byDstMtId)
	{
		cServMsg.SetErrorCode(ERR_MCU_INVALID_OPER);
		ConfLog(FALSE, "[ProcMcsMcuStartSwitchVmpMtReq] Only chairman can select see VMP!\n");
		SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
		return ;
	}

	//会议不在vmp中
	if (m_tConf.m_tStatus.GetVMPMode() == CONF_VMPMODE_NONE )	
	{
		cServMsg.SetErrorCode(ERR_MCU_VMPNOTSTART);
		ConfLog(FALSE, "[ProcMcsMcuStartSwitchVmpMtReq] The Conf is not in the VMP mode!\n");
		SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
		return ;
	}

	//老VMP不支持选看
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;			//取vmp子类型
	if(byVmpSubType == VMP)
	{
		cServMsg.SetErrorCode(ERR_FUNCTIONNOTSUPPORTBYOLDVMP);
		ConfLog(FALSE, "Old VMP doesn't support selected!\n");
		SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
		return ;
	}

	u8 byChnnlType = LOGCHL_VIDEO;   
	
    TLogicalChannel tLogicChannel;
    if ( !m_tConfAllMtInfo.MtJoinedConf( byDstMtId ) ||
		!m_ptMtTable->GetMtLogicChnnl( byDstMtId, byChnnlType, &tLogicChannel, FALSE ) )
    {
        ConfLog(FALSE,"[ProcMcsMcuStartSwitchVmpMtReq]Mt.%u isn't online in conf or backward logical channel not open!\n",byDstMtId);
		SendReplyBack(cServMsg,cServMsg.GetEventId()+2);
		return ;
    }

	SendReplyBack(cServMsg,cServMsg.GetEventId()+1);
	
	if(cServMsg.GetEventId() != MT_MCU_STARTSWITCHVMPMT_REQ)
	{
		// xliang [7/3/2009] 给主席终端也回应，从而使mcs，mtc状态一致
		SendMsgToMt( byDstMtId, MCU_MT_STARTSWITCHVMPMT_ACK, cServMsg);
	}

	m_tConf.m_tStatus.SetVmpSeebyChairman(TRUE);
	tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
	g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );//刷新tPeriEqpStatus

	// xliang [4/15/2009] 若主席终端此时正在选看某个MT，则要取消那个主席选看Mt。
	// 那个被选看的MT如果没有其他MT在选看他，即该MT被选看取消，并尝试退出前适配
	u8 byMode = MODE_VIDEO;
	TMtStatus tDstMtStatus;
	m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tDstMtStatus );
	if ( !(tDstMtStatus.GetSelectMt(byMode).IsNull()) )
	{
		StopSelectSrc(tDstMt, byMode);
	}

	tDstMtStatus.SetSelectMt( m_tVmpEqp, byMode ); //记录新的选看源
	m_ptMtTable->SetMtStatus( tDstMt.GetMtId(), &tDstMtStatus );

	if(m_tConf.m_tStatus.IsBrdstVMP())
	{
		//画面合成本来就在广播码流，此处不必再建交换
		//选看状态在此处上报
		MtStatusChange( tDstMt.GetMtId(), TRUE );
	}
	else
	{
		u8 bySrcChnnl = 0;
		u8 byMtMediaType = 0;
		u8 byMtRes = 0;
		m_cMtRcvGrp.GetMtMediaRes(tDstMt.GetMtId(), byMtMediaType, byMtRes);
		bySrcChnnl = GetVmpOutChnnlByRes(byMtRes, m_tVmpEqp.GetEqpId(), byMtMediaType);

//		u8 byMtFormat = tLogicChannel.GetVideoFormat(); // 获取分辨率
		
// 		//根据分辨率决定SrcChnnl,temperary modify to adapt mpu current ability
// 		u8 bySrcChnnl = 0;
// 		switch ( byMtFormat)
// 		{
// 		case VIDEO_FORMAT_HD1080:
// 		case VIDEO_FORMAT_HD720:
// 			bySrcChnnl = 0;
// 			break;
// 		case VIDEO_FORMAT_CIF:
// 			bySrcChnnl = 2;
// 			break;
// 		}

		CallLog("VMP --> MT.%u, SrcChnnlIdx is %u\n",byDstMtId, bySrcChnnl);

		//建交换
		TSwitchInfo tSwitchInfo;
		tSwitchInfo.SetDstMt(tDstMt);
		tSwitchInfo.SetSrcMt(m_tVmpEqp);
		tSwitchInfo.SetSrcChlIdx(bySrcChnnl);
		tSwitchInfo.SetMode(MODE_VIDEO);

		SwitchSrcToDst( tSwitchInfo, cServMsg );
	}
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
    if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
    {
        ConfLog( FALSE, "[ProcMtMcuStopSwitchMtReq] Dst Mt%u-%u not joined conference!\n", 
                         tDstMt.GetMcuId(), tDstMt.GetMtId() );
        cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        return;
    }

    //Ack
    if( cServMsg.GetEventId() == MCS_MCU_STOPSWITCHMT_REQ )
    {
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		
		//同时通知主席终端，停止选看VMP
		if(HasJoinedChairman() && m_tConf.GetChairman() == tDstMt)
		{
			TMtStatus tMtStatus;
			m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tMtStatus );
			TMt tSrc = tMtStatus.GetSelectMt( MODE_VIDEO );
			if(tSrc.GetType() == TYPE_MCUPERI && tSrc.GetMtId() == m_tVmpEqp.GetEqpId())
			{
				SendMsgToMt(tDstMt.GetMtId(), MCU_MT_STARTSWITCHVMPMT_NACK,cServMsg);
			}
			m_tConf.m_tStatus.SetVmpSeebyChairman(FALSE);
			// 状态同步刷新到TPeriStatus中
			TPeriEqpStatus tVmpStatus;
			g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tVmpStatus );
			tVmpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
			g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tVmpStatus );
		}
    } 

    StopSelectSrc(tDstMt, MODE_BOTH);

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
	TLogicalChannel	tLogicChnnl, tOtherLogicalChannel;

	switch( CurState() )
	{
	case STATE_ONGOING:
    {
		tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
		tLogicChnnl = *( TLogicalChannel * )( cServMsg.GetMsgBody() );

		if( pcMsg->event == MT_MCU_OPENLOGICCHNNL_NACK )
		{
			if(tLogicChnnl.GetMediaType() == MODE_VIDEO)
			{
				m_ptMtTable->SetMtVideoRecv( tMt.GetMtId(), FALSE );
				CallLog( "Mcu open Mt%d primary video logicChannel(0x%x:%d%s) error!\n", 
				tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
				tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) );
			}
			if(tLogicChnnl.GetMediaType() == MODE_AUDIO)
			{
				m_ptMtTable->SetMtAudioRecv( tMt.GetMtId(), FALSE );
				CallLog( "Mcu open Mt%d audio logicChannel(0x%x:%d%s) error!\n", 
					tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
					tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) );
			}
			if(tLogicChnnl.GetMediaType() == MODE_SECVIDEO)
			{
				m_ptMtTable->SetMtVideo2Recv( tMt.GetMtId(), FALSE );
				CallLog( "Mcu open Mt%d second video logicChannel(0x%x:%d%s) error!\n", 
					tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
					tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) );
                
                //  xsl [6/15/2006] 打开双流失败时，终端恢复接收带宽，若存在自动升降速则不恢复
                if (m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE) == GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())))
                {
                    u16 wMtDialBitrate = m_ptMtTable->GetRcvBandWidth(tMt.GetMtId());
                    m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(), wMtDialBitrate, LOGCHL_VIDEO);
                    Mt2Log("[ProcMtMcuOpenLogicChnnlRsp]tDstMt.GetMtId() = %d, MtReqBitrate = %d\n",
                        tMt.GetMtId(), wMtDialBitrate);
                }            

				//尝试切换码流置第一路
				StartSwitchDStreamToFirstLChannel( tMt );
			}

			//如果打开T120数据通道失败
			if ( MODE_DATA == tLogicChnnl.GetMediaType() )
			{
				if ( MEDIA_TYPE_T120 == tLogicChnnl.GetChannelType() )
				{
					if ( m_ptMtTable->IsMtIsMaster( tMt.GetMtId() ) && m_ptMtTable->IsNotInvited( tMt.GetMtId() ))
					{
						SendMcuDcsDelMtReq( tMt.GetMtId() );
						ConfLog( FALSE, "Mcu open mt%d t120 logicchannel (0x%x:%d%s) as master failed !\n",
							tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
							tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) ); 
					}		
					//如果对端做主, 且是KDV MCU, 则是正常操作
					else
					{
						ConfLog( FALSE, "Mcu open mt%d t120 logicchannel (0x%x:%d%s) as slave failed, its legal affair if KEDA MCU as master !\n",
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

        // 以下是成功打开逻辑通道的处理
		switch( tLogicChnnl.GetMediaType() )
		{
		case MODE_VIDEO:
			m_ptMtTable->SetMtVideoRecv( tMt.GetMtId(), TRUE );
			byChannel = LOGCHL_VIDEO;
			MtStatusChange();
			CallLog( "Mcu open Mt%d primary video logicChannel(0x%x:%d:%s) success!\n", 
				tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
				tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) );
			
			// xliang [11/10/2008] 打开第一路通道成功后，直接打开Polycom的MCU第二路通道
			if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
				(MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
				MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ))
			{
				SetTimer( EV_MCU_OPENSECVID_TIMER, TIMESPACE_WAIT_FSTVIDCHL_SUC, tMt.GetMtId() );
				CallLog( "[OpenLogicChnnlRsp] Mt.%d Fst chnnl open succeed, try the second one immediately due to Polycom\n", tMt.GetMtId() );
			}
			break;

		case MODE_SECVIDEO:
			m_ptMtTable->SetMtVideo2Recv( tMt.GetMtId(), TRUE );
			byChannel = LOGCHL_SECVIDEO;
			MtStatusChange();
			CallLog( "Mcu open Mt%d second video logicChannel(0x%x:%d:H239.%d:%s,Res.%s) success!\n", 
					tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
					tLogicChnnl.m_tRcvMediaChannel.GetPort(), tLogicChnnl.IsSupportH239(), 
					GetMediaStr(tLogicChnnl.GetChannelType()),
					GetResStr(tLogicChnnl.GetVideoFormat()));

			//zbq[09/02/2008] 兼容老的终端，Res为空，参照能力补齐（暂只处理双流）
			if (0 == tLogicChnnl.GetVideoFormat())
			{
				TCapSupport tMtCap;
				if (!m_ptMtTable->GetMtCapSupport(tMt.GetMtId(), &tMtCap))
				{
					ConfLog(FALSE, "[OpenLogicChanRsp] get mt.%d's cap failed, check it\n", tMt.GetMtId());
				}
				tLogicChnnl.SetVideoFormat(tMtCap.GetDStreamCapSet().GetResolution());
			}
			// xliang [11/14/2008]  打开第二路通道成功后，判断当前是否有双流源，以期请求令牌并建双流交换
			if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
				(MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
				MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ) &&
				!m_tDoubleStreamSrc.IsNull() && !(tMt == m_tDoubleStreamSrc) )
			{
				//请求令牌
				TH239TokenInfo tH239TokenInfo;
				TLogicalChannel tSecVidChnnl;
				
				CServMsg cServMsg;
				cServMsg.SetEventId(MCU_POLY_GETH239TOKEN_REQ);
				cServMsg.SetMsgBody((u8*)&tMt, sizeof(TMt));
				cServMsg.CatMsgBody((u8*)&tH239TokenInfo, sizeof(tH239TokenInfo));
				SendMsgToMt(tMt.GetMtId(), MCU_POLY_GETH239TOKEN_REQ, cServMsg );
				CallLog( "[OpenChnnlRsp] MCU_POLY_GETH239TOKEN_REQ to PolyMCU.%d\n", tMt.GetMtId());
				
			}
			break;

		case MODE_AUDIO:
			m_ptMtTable->SetMtAudioRecv( tMt.GetMtId(), TRUE );
			byChannel = LOGCHL_AUDIO;
			MtStatusChange();
			CallLog( "Mcu open Mt%d audio logicChannel(0x%x:%d:%s) success!\n", 
					tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
					tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) );
			break;
		case MODE_DATA:
			if( tLogicChnnl.GetChannelType() == MEDIA_TYPE_H224 )
			{
				byChannel = LOGCHL_H224DATA;
				TMtStatus tMtStatus;
				m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tMtStatus);
				tMtStatus.SetIsEnableFECC(TRUE);
				m_ptMtTable->SetMtStatus(tMt.GetMtId(), &tMtStatus);
				CallLog( "Mcu open Mt%d data H224 logicChannel(0x%x:%d:%s) success!\n", 
						tMt.GetMtId(), tLogicChnnl.m_tRcvMediaChannel.GetIpAddr(), 
						tLogicChnnl.m_tRcvMediaChannel.GetPort(), GetMediaStr( tLogicChnnl.GetChannelType() ) );
			}
			else if( tLogicChnnl.GetChannelType() == MEDIA_TYPE_MMCU )
			{
				byChannel = LOGCHL_MMCUDATA;
				CallLog( "Mcu open Mt%d data mmcu logicChannel(0x%x:%d:%s) success!\n", 
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

						CallLog( "Mcu open Mt%d data T.120 logicChannel(0x%x:%d:%s) as master success!\n", 
							tMt.GetMtId(), dwDcsIp, wDcsPort, GetMediaStr( tLogicChnnl.GetChannelType() ) );
					}
					else
					{
						CallLog( "Mcu open Mt%d data T.120 logicChannel(0x%x:%d:%s) as master failed!\n", 
							tMt.GetMtId(), dwDcsIp, wDcsPort, GetMediaStr( tLogicChnnl.GetChannelType() ) );
					}
				}
				//如果本端被呼
				else
				{
					//这里只能收到MT的ACK消息, 可以认为通道打开成功
					CallLog( "Mcu open Mt%d data T.120 logicChannel as master success!\n", tMt.GetMtId() );
				}
			}
			break;
		}

		//保存逻辑通道
		if( tLogicChnnl.GetMediaType() == MODE_VIDEO && byChannel == LOGCHL_VIDEO )
		{           
            //终端下行带宽初始化
			
			//zbq[09/10/2008] 下行带宽调整过，则遵循调整好的带宽
			if (!m_ptMtTable->GetRcvBandAdjusted(tMt.GetMtId()))
			{
				m_ptMtTable->SetRcvBandWidth(tMt.GetMtId(), m_ptMtTable->GetDialBitrate(tMt.GetMtId()));
			}
			
            if ( !m_tDoubleStreamSrc.IsNull() )
            {
                //zbq[02/01/2008] 降速后的终端调整码率不依赖于呼入，直接取降速后的带宽
                if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
                     m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()) ) 
                {
                    tLogicChnnl.SetFlowControl( GetDoubleStreamVideoBitrate(m_ptMtTable->GetLowedRcvBandWidth(tMt.GetMtId())) );
                }
                else
                {
                    tLogicChnnl.SetFlowControl( GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())) );
                }
                //启动FlowControl定时器, 防止双速会议呼入的终端码率低于第二路码流, 07-01-31
				SetTimer( MCUVC_SENDFLOWCONTROL_TIMER, 1200 );
            }
            else
            {
                tLogicChnnl.SetFlowControl( m_ptMtTable->GetDialBitrate( tMt.GetMtId() ) );
            }
		}
        //  xsl [4/28/2006] 根据双流码率比例分配视频带宽
        else if ( tLogicChnnl.GetMediaType() == MODE_SECVIDEO && byChannel == LOGCHL_SECVIDEO )
        {
            //zbq[01/31/2008] 降速终端分配带宽的基准应该是降速后的带宽, 非降速终端为呼叫带宽
            if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
                 m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()) )
            {
                tLogicChnnl.SetFlowControl( GetDoubleStreamVideoBitrate(m_ptMtTable->GetLowedRcvBandWidth(tMt.GetMtId()), FALSE) );
                CallLog("[ProcMtMcuOpenLogicChnnlRsp] Lowed TMt.%d set DSVBR.%d\n", tMt.GetMtId(), tLogicChnnl.GetFlowControl());
                
                //降速终端此处同时计算调整第一路视频带宽.
                m_ptMtTable->SetMtReqBitrate(tMt.GetMtId(),
											 GetDoubleStreamVideoBitrate(m_ptMtTable->GetLowedRcvBandWidth(tMt.GetMtId()), TRUE),
											 LOGCHL_VIDEO );
            }
            else
            {
                tLogicChnnl.SetFlowControl( GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId()), FALSE) );
                CallLog("[ProcMtMcuOpenLogicChnnlRsp] TMt.%d set DSVBR.%d\n", tMt.GetMtId(), tLogicChnnl.GetFlowControl());
            }
            
            //zbq[02/01/2008] 启动FlowControl定时器, 防止本终端为E1降速终端，后续调整当前来不及处理的带宽
            SetTimer( MCUVC_SENDFLOWCONTROL_TIMER, 1200 );            

            //zbq[02/18/2007] 打开双流通道，调整当前的VMP广播带宽
            if ( m_tConf.m_tStatus.IsBrdstVMP() )
            {
                AdjustVmpBrdBitRate();
            }

            /*
            // 添加接收类型, zgc, 2008-08-07
            u8 emType = m_cConfVidTypeMgr.ConvertOut2In( tLogicChnnl.GetChannelType(), tLogicChnnl.GetVideoFormat(), TRUE );
            m_cConfVidTypeMgr.AddType( emType, TRUE );*/

			//zbq[09/12/2008]写入帧率
			TCapSupport tCap;
			m_ptMtTable->GetMtCapSupport(tMt.GetMtId(), &tCap);
			if (tCap.GetDStreamCapSet().IsFrameRateUserDefined())
			{
				tLogicChnnl.SetChanVidFPS(tCap.GetDStreamCapSet().GetUserDefFrameRate());
			}
			else
			{
				tLogicChnnl.SetChanVidFPS(tCap.GetDStreamCapSet().GetFrameRate());
			}
        }
		tLogicChnnl.SetMediaEncrypt(m_tConf.GetMediaKey());
		tLogicChnnl.SetActivePayload(GetActivePayload(m_tConf,tLogicChnnl.GetChannelType()));
		m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), byChannel, &tLogicChnnl, TRUE );
		
		//将此级联mcu信息进行添加置列表中
		if( MEDIA_TYPE_MMCU == tLogicChnnl.GetChannelType() )
		{
			m_ptConfOtherMcTable->AddMcInfo( tMt.GetMtId() );	
			m_tConfAllMtInfo.AddMcuInfo( tMt.GetMtId(), tMt.GetConfIdx() );
	
			//发给会控会议所有终端信息
			cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
			SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
		
			//若会议在全体讨论下，尝试自动开启下级mcu的会议讨论			
            if( m_tConf.m_tStatus.IsMixing() )
			{
                CServMsg cTmpMsg;
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
		}
	
		//视频交换措施
		if( tLogicChnnl.GetMediaType() == MODE_VIDEO && byChannel == LOGCHL_VIDEO )
		{
            //zbq[04/22/2009] 主流 依赖通道动态刷新其群组位置
            RefreshRcvGrp(tMt.GetMtId());

			//添加丢包重传终端
			if( m_tConf.m_tStatus.IsPrsing() )
			{
				AddRemovePrsMember( tMt, TRUE, PRSCHANMODE_FIRST );
			}

            // xsl [7/21/2006] 告诉其从组播地址接收
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {                
                ChangeSatDConfMtRcvAddr(tMt.GetMtId(), LOGCHL_VIDEO);
            }

			//开始媒体类型适配 - modify bas 2 -- 可以考虑是否需要StartAdapt()
            TSimCapSet tSrcSimCapSet;
            TSimCapSet tDstSimCapSet;
            u16 wAdaptBitRate;
            if (m_tConf.GetConfAttrb().IsUseAdapter())
            {
                // 考虑高清适配会议
                if ( IsHDConf( m_tConf ) )
                {
                    if (m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId()))
                    {
                        //zbq[04/11/2009] 新增终端动态触发新适配
                        if (m_tConf.m_tStatus.IsHdVidAdapting())
                        {
                            RefreshBasParam(tMt.GetMtId(), TRUE);
                            StartSwitchToSubMtFromAdp(tMt.GetMtId());
                        }
                        else
                        {
                            StartHDMVBrdAdapt();
                        }
                    }
                }
                else
                // guzh [7/24/2007] 如果需要格式适配，分情况讨论
                // zbq [08/13/2007] 基于降低开销考虑，只判断当前终端是否需要适配
                if (IsMtNeedAdapt(ADAPT_TYPE_VID, tMt.GetMtId()) &&
                    IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate))
                {
                    // 1. 原来不需要，新MT上线导致开启格式适配
                    if (!m_tConf.m_tStatus.IsVidAdapting())
                    {
                        StartAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                    }
                    else                    
                    {
                        // 2. 原来就在格式适配，考察是否需要降码率
                        // zbq [08/11/2007] 或直接进行适配调整
                        bySrcChnnl = (m_tVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
                        StartSwitchToPeriEqp(m_tVidBrdSrc, bySrcChnnl, m_tVidBasEqp.GetEqpId(),
                                             m_byVidBasChnnl, MODE_VIDEO);

                        ChangeAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                    }
                }
                // zbq [08/13/2007] 基于降低开销考虑，只判断当前终端是否需要适配
                else if(IsMtNeedAdapt(ADAPT_TYPE_BR, tMt.GetMtId()) &&
                        IsNeedBrAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate) )
                {
                    if (!m_tConf.m_tStatus.IsBrAdapting() )
                    {
                        StartAdapt(ADAPT_TYPE_BR, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                    }
                    else
                    {                        
                        bySrcChnnl = (m_tVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
                        StartSwitchToPeriEqp(m_tVidBrdSrc, bySrcChnnl, m_tBrBasEqp.GetEqpId(),
                                             m_byBrBasChnnl, MODE_VIDEO);
                        
                        ChangeAdapt(ADAPT_TYPE_BR, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                    }

                }
            }
			if( !m_tVidBrdSrc.IsNull() )
			{
				// xliang [3/19/2009] 广播源是mpu，则建普通交换
				BOOL32 bNewVmpBrd = FALSE;
				if(m_tVidBrdSrc == m_tVmpEqp)
				{
					TPeriEqpStatus tPeriEqpStatus; 
					g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
					u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
					if(byVmpSubType != VMP)
					{
						bNewVmpBrd = TRUE;
					}
				}
				if(bNewVmpBrd)
				{
					SwitchNewVmpToSingleMt(tMt);
				}
				else
				{
					bySrcChnnl = (m_tVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
					if (!m_tConf.GetConfAttrb().IsSatDCastMode())
					{
						// zbq[12/18/2008] 直接走广播交换是有问题的.为什么这样修改？增加过滤之.
						// 有广播源应走广播交换, zgc, 2008-03-06
						//StartSwitchToSubMt( m_tVidBrdSrc, bySrcChnnl, tMt.GetMtId(), MODE_VIDEO );
						if (!(IsHDConf(m_tConf) && m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId())))
						{
							StartSwitchFromBrd( m_tVidBrdSrc, bySrcChnnl, 1, &tMt );
						}
                        else
                        {
                            NotifyMtReceive(m_tVidBrdSrc, tMt.GetMtId());
                        }
					}
					else
					{
						// 顾振华 [5/29/2006]
						//卫星会议则简单通知它看发言人，而不再去建立交换
						NotifyMtReceive( m_tVidBrdSrc, tMt.GetMtId() );
					}
				}
			}
			else
			{
				//通知终端收看本地
				cServMsg.SetMsgBody( (u8*)&tMt, sizeof(tMt) );
				SendMsgToMt( tMt.GetMtId(), MCU_MT_YOUARESEEING_NOTIF, cServMsg );
				//自己的视频交换给自己
				StartSwitchToSubMt( tMt, 0, tMt.GetMtId(), MODE_VIDEO );

				//对于VCS会议当前调度终端看主席
				if (VCS_CONF == m_tConf.GetConfSource() &&
					!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
				{
					GoOnSelStep(tMt, MODE_VIDEO, TRUE); 
				}
			}
	
			// 使用新的发言人选看函数, zgc, 2008-03-06
			if( HasJoinedSpeaker() && 
				!m_tConf.m_tStatus.IsBrdstVMP() && 
				!( m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO ) &&
				( tMt == GetLocalSpeaker() || tMt == m_tConf.GetChairman() || tMt == m_tLastSpeaker ) )
			{
				// 新发言人码流源调整逻辑, zgc, 2008-04-12
				ChangeSpeakerSrc(MODE_VIDEO, emReasonChangeSpeaker);
			}
		}

		//双流交换
        if (tLogicChnnl.GetMediaType() == MODE_SECVIDEO && byChannel == LOGCHL_SECVIDEO)
        {
            //是否要转发双流数据
            TLogicalChannel tH239LogicalChannel;
            if (TYPE_MCUPERI == m_tDoubleStreamSrc.GetType() ||
                m_ptMtTable->GetMtLogicChnnl(m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tH239LogicalChannel, FALSE))
            {
                // 顾振华[5/25/2006] 如果不是卫星分散会议,需要交换双流
                if (!m_tConf.GetConfAttrb().IsSatDCastMode())                
                {
                    // 不需要适配的直接建交换, zgc, 2008-08-20
                    if ( !m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId(), FALSE))
                    {
                        // zbq [07/26/2007] 录像机的双流交换依赖于其放象通道
                        u8 bySrcChnnl = m_tDoubleStreamSrc == m_tPlayEqp ? m_byPlayChnnl : 0;
#ifndef _SATELITE_
                        g_cMpManager.StartSwitchToSubMt(m_tDoubleStreamSrc, bySrcChnnl, tMt, MODE_SECVIDEO, SWITCH_MODE_BROADCAST, TRUE);
#else
						g_cMpManager.StartSatConfCast(m_tDoubleStreamSrc, CAST_FST, MODE_SECVIDEO,  bySrcChnnl);
#endif
                    }
                    else
                    {
                        StartSwitchToSubMtFromAdp(tMt.GetMtId(), TRUE);
                    }
                }               
            }

            //第二路视频的丢包重传
            if (m_tConf.m_tStatus.IsPrsing() && m_tDoubleStreamSrc.GetMtId() != tMt.GetMtId())
            {
                // 不需要适配的直接建丢包重传交换, zgc, 2008-08-20
                if ( !m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId(), FALSE) )
                {
                    AddRemovePrsMember(tMt, TRUE, PRSCHANMODE_SECOND);
                }
            }

            // xsl [7/21/2006] 告诉其从组播地址接收
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {                
                ChangeSatDConfMtRcvAddr(tMt.GetMtId(), LOGCHL_SECVIDEO);
            }
        }

		//音频交换措施
		if( tLogicChnnl.GetMediaType() == MODE_AUDIO )
		{
            //添加终端的音频丢包重传
			if (m_tConf.m_tStatus.IsPrsing())
			{
				AddRemovePrsMember(tMt, TRUE, PRSCHANMODE_AUDIO);
			}

            // xsl [7/21/2006] 告诉其从组播地址接收
            if (m_tConf.GetConfAttrb().IsSatDCastMode())
            {               
                ChangeSatDConfMtRcvAddr(tMt.GetMtId(), LOGCHL_AUDIO);
            }

			if( HasJoinedSpeaker() )
			{
				bySrcChnnl = (m_tVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;

				if (m_tConf.m_tStatus.IsNoMixing() || m_tConf.m_tStatus.IsMustSeeSpeaker())
				{
                    // 顾振华 [5/29/2006]
                    if (!m_tConf.GetConfAttrb().IsSatDCastMode())
                    {
                        StartSwitchToSubMt( GetLocalSpeaker(), bySrcChnnl, tMt.GetMtId(), MODE_AUDIO );
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
            
			// xsl [8/4/2006] 定制混音将n模式码流交换给终端(发言人除外)
			if( m_tConf.m_tStatus.IsMixing() )
			{
                if (m_tConf.m_tStatus.IsSpecMixing())
                {
                    if (tMt == GetLocalSpeaker() && GetMixMtNumInGrp() < GetMaxMixNum())
                    {
                        // guzh [7/14/2007] 这里在ChangeSpeaker里保护后，这里需要重新AddRemove一次。否则InMixing图标会不长
                        // 原来为什么 AddMixMember？
                        AddRemoveSpecMixMember(&tMt, 1, TRUE); 
                        //AddMixMember(&tMt, DEFAULT_MIXER_VOLUME, FALSE);
                    }
                    else
                    {
                        SwitchMixMember(&tMt, TRUE);
                    }   
                }
                else 
                {
                    if (GetMixMtNumInGrp() < GetMaxMixNum())
                    {
                        AddMixMember(&tMt, DEFAULT_MIXER_VOLUME, FALSE);
                    }                    
                }
            }
			else
			{
				// 对于VCS会议非混音模式下，终端听主席
				if (VCS_CONF == m_tConf.GetConfSource() &&
					!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
				{
					GoOnSelStep(tMt, MODE_AUDIO, TRUE); 
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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
	CServMsg	cSendMsg( pcMsg->content, pcMsg->length );
	TMt	tMt;
	u32	dwRcvIp;
	u16	wRcvPort;
	u8  byChannel;
    u8  byAudioType;

    TCapSupportEx tCapSupportEx;
	TLogicalChannel	tLogicChnnl, tVideoLogicChnnl, tH239LogicChnnl;

	switch( CurState() )
	{
	case STATE_ONGOING:
        {
		tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
		tLogicChnnl = *( TLogicalChannel * )( cServMsg.GetMsgBody() );

#ifdef _SATELITE_
		if (MODE_SECVIDEO == tLogicChnnl.GetMediaType())
		{
			//卫星终端发起双流，根据会议主双流能力构造TLogicChannel；8005不支持双流
			u8 byConfDSMainType = m_tConf.GetDStreamMediaType();
			tLogicChnnl.SetChannelType(byConfDSMainType);
			tLogicChnnl.SetActivePayload(GetActivePayload(m_tConf, byConfDSMainType));
			
			if (MEDIA_TYPE_H264 == byConfDSMainType)
			{
				tLogicChnnl.SetChanVidFPS(m_tConf.GetDStreamUsrDefFPS());
			}
			else
			{
				tLogicChnnl.SetChanVidFPS(m_tConf.GetDStreamFrameRate());
			}
			
			tLogicChnnl.SetVideoFormat(m_tConf.GetDoubleVideoFormat());
			tLogicChnnl.SetSupportH239(TRUE);
		}
#endif
		
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
					   MT_TYPE_MT == m_ptMtTable->GetMtType(tMt.GetMtId()) ) )
                {
                }
                else
                {                
                    //判断双流的发起源 
                    if (CONF_DUALMODE_EACHMTATWILL == m_tConf.GetConfAttrb().GetDualMode() ||
                        (CONF_DUALMODE_SPEAKERONLY == m_tConf.GetConfAttrb().GetDualMode() && tMt == GetLocalSpeaker() ) ||
						// xliang [11/14/2008] PolycomMCU请求，则允许
						// xliang [11/14/2008] FIXME:这里有潜在时序问题
						MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId())) 
                    {                       
                        // xliang [11/14/2008]  polycomMCU 打开双流通道，不作开启双流处理，后续flowctrl里作对应调整
						if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
							( MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
							MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId())) )
						{
							CallLog("[ProcMtMcuOpenLogicChnnlReq] PolyMcu.%d open DVideo req here\n", tMt.GetMtId());
						}
						else
						{
							UpdateH239TokenOwnerInfo( tMt );
							StartDoubleStream( tMt, tLogicChnnl );
                            CallLog("[ProcMtMcuOpenLogicChnnlReq] Chn connected ntf rcved, start ds!\n");
						}
                    }
                    else
                    {
                        CallLog("[ProcMtMcuOpenLogicChnnlReq] Chn connected ntf rcved, no start ds due to DualMode.%d, tMt.%d\n",
                            m_tConf.GetConfAttrb().GetDualMode(), tMt.GetMtId());
                    }
    			}
            }
            return;
        }

		// 当呼叫终端开始请求打开MCU通道时，认为该调度终端调度成功，可进行下一个终端的调度
		if (VCS_CONF == m_tConf.GetConfSource() &&
			tMt == m_cVCSConfStatus.GetReqVCMT())
		{
		 	KillTimer(MCUVC_VCMTOVERTIMER_TIMER);
			ChgCurVCMT(tMt);
		}
		
        //以下为正常的逻辑通道ACK/NACK处理
		if(tLogicChnnl.GetMediaType() == MODE_VIDEO)
		{
            m_ptMtTable->SetMtVideoSend(tMt.GetMtId(), FALSE);

            //  xsl [3/17/2006] 263+格式后面根据动态载荷过滤
			if (!m_tConf.GetCapSupport().IsSupportMediaType(tLogicChnnl.GetChannelType()) && 
                MEDIA_TYPE_H263PLUS != tLogicChnnl.GetChannelType())
			{
				ConfLog( FALSE, "Mt%d 0x%x(Dri:%d) not support video(%s), nack!\n",
					tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() , GetMediaStr(tLogicChnnl.GetChannelType()));
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}
            // 顾振华@2006.4.13 如果<64K码率，则拒绝打开逻辑通道
            if (m_tConf.GetBitRate() < 64)
            {
				ConfLog( FALSE, "Bitrate less than 64K, open video logic chnl nack!\n");
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
            }
		}
		if(tLogicChnnl.GetMediaType() == MODE_AUDIO)
		{
            m_ptMtTable->SetMtAudioSend(tMt.GetMtId(), FALSE);

			if(!m_tConf.GetCapSupport().IsSupportMediaType(tLogicChnnl.GetChannelType()))
			{
				ConfLog( FALSE, "Mt%d 0x%x(Dri:%d) not support audio(%s), nack!\n",
					tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() , GetMediaStr(tLogicChnnl.GetChannelType()));
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}			
		}
		if( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
		{
            m_ptMtTable->SetMtVideo2Send(tMt.GetMtId(), FALSE);
			
			// xliang [12/18/2008]  双流通道分辨率匹配校验
			u8 bySrcRes = tLogicChnnl.GetVideoFormat();
			u8 byDstRes = m_tConf.GetCapSupport().GetDStreamCapSet().GetResolution();
			if (!IsDSResMatched(bySrcRes, byDstRes))
			{
				CallLog("[ProcMtMcuOpenLogicChnnlReq] Mt.%d's DS res dismatched<src.%d, dst.%d>, ignore it\n", 
					tMt.GetMtId(), bySrcRes, byDstRes);
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}

            if(tLogicChnnl.IsSupportH239() != m_tConf.GetCapSupport().IsDStreamSupportH239() &&
               tLogicChnnl.GetChannelType() != m_tConf.GetCapSupport().GetDStreamMediaType() &&
               //zbq[01/04/2009] 双双流过滤放开
               tLogicChnnl.GetChannelType() != m_tConf.GetCapSupportEx().GetSecDSType())
            {
                ConfLog( FALSE, "Mt%d 0x%x(Dri:%d) not support second video (%s), nack!\n",
                    tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() , GetMediaStr(tLogicChnnl.GetChannelType()));
                SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                return;
            }
            // 顾振华@2006.4.13 如果<64K码率，则拒绝打开逻辑通道
            if (m_tConf.GetBitRate() < 64)
            {
				ConfLog( FALSE, "Bitrate less than 64K, open sec video logic chnl, nack!\n");
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
					ConfLog( FALSE, "Mt%d open mcu T.120 logicchannel as slave, nack it directly!\n", tMt.GetMtId() );
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
				ConfLog( FALSE, "Mt%d 0x%x(Dri:%d) not encrypt, nack!\n",
					tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;	
			}
		}


		//设置逻辑通道
		if( !m_ptMtTable->GetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort ) )
		{
			dwRcvIp = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( tMt.GetMtId() ) );
			if( dwRcvIp == 0 )
			{
				ConfLog( FALSE, "Mt%d 0x%x(Dri:%d) mp not connected, nack!\n",
						 tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;			    
			}
			wRcvPort = g_cMcuVcApp.AssignMtPort( tMt.GetConfIdx(), tMt.GetMtId() );
			m_ptMtTable->SetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort );
		}

		//正常的逻辑通道打开处理
		tLogicChnnl.m_tRcvMediaChannel.SetIpAddr( dwRcvIp );
		tLogicChnnl.m_tRcvMediaCtrlChannel.SetIpAddr( dwRcvIp );

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
                ConfLog(FALSE, "[ProcMtMcuOpenLogicChnnlReq] remote bitrate %d, local bitrate %d. nack\n", 
                                tLogicChnnl.GetFlowControl(), 
                                m_tConf.GetBitRate() + GetAudioBitrate(byAudioType));
				return;	
			}

			if( MEDIA_TYPE_H263PLUS == tLogicChnnl.GetChannelType() && 
				MEDIA_TYPE_H263PLUS == tLogicChnnl.GetActivePayload() )
			{
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                ConfLog(FALSE, "[ProcMtMcuOpenLogicChnnlReq] ChnType.%d, ActivePayload.%d, nack\n", 
                                tLogicChnnl.GetChannelType(),
                                tLogicChnnl.GetActivePayload() );
				return;
			}

			CallLog( "Mt%d open mcu primary video logicChannel(0x%x:%d:%s)\n", 
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
                if (CONF_DUALMODE_EACHMTATWILL == m_tConf.GetConfAttrb().GetDualMode() ||
                    ( CONF_DUALMODE_SPEAKERONLY == m_tConf.GetConfAttrb().GetDualMode() && tMt == GetLocalSpeaker() ) ) 
                {
                    // xsl [7/20/2006]卫星分散会议时需要考虑回传通道数
                    if (m_tConf.GetConfAttrb().IsSatDCastMode() && IsOverSatCastChnnlNum(tMt.GetMtId()))
                    {
                        ConfLog(FALSE, "[ProcMtMcuOpenLogicChnnlReq] over max upload mt num.\n");
                        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );

                        return;
                    }
					//延迟到connect后发起
					/*UpdateH239TokenOwnerInfo( tMt );
					bStartDS = TRUE;
					StartDoubleStream( tMt, tLogicChnnl );*/
/*
#ifdef _SATELITE_
					//卫星环境下，缺乏进一步的消息驱动，不作延后发起
					StartDoubleStream( tMt, tLogicChnnl );
#endif
*/
                }
                else
                {
#ifdef _SATELITE_
					//卫星会议目前只可能在此处拒绝终端的双流发起
					CServMsg cMsg;
					cMsg.SetConfIdx(m_byConfIdx);
					cMsg.SetDstMtId(tMt.GetMtId());
					cMsg.SetEventId(30255 /*MCU_MT_STARTDS_REQ + 1*/);
					
					u8 byDSMsgMode = 1;	//1: open req/rsp; 0: close req/rsp
					u16 wBitRate = 0;	//reject
					cServMsg.SetMsgBody(&byDSMsgMode, sizeof(u8));
					cServMsg.CatMsgBody((u8*)&wBitRate, sizeof(u16));
					SendMsgToMt(tMt.GetMtId(), 30255 /*MCU_MT_STARTDS_REQ + 1*/, cMsg);
#endif
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
                    CallLog( "Mt%d open mcu second video logicChannel(0x%x:%d:H239.%d:%s) with wrong Dualmode, NACK !\n",
                        tMt.GetMtId(), dwRcvIp, wRcvPort + 4, tLogicChnnl.IsSupportH239(), GetMediaStr(tLogicChnnl.GetChannelType()));
                    return;	
                }
			}

			// zbq [09/12/2008] Mtadp适配取不到帧率，此处保护为本地
			if (tLogicChnnl.GetChanVidFPS() > 25)
			{
				if (MEDIA_TYPE_H264 == tLogicChnnl.GetChannelType())
				{
					tLogicChnnl.SetChanVidFPS(m_tConf.GetDStreamUsrDefFPS());
				}
				else
				{
					tLogicChnnl.SetChanVidFPS(m_tConf.GetDStreamFrameRate());
				}
			}
            // zbq [09/05/2007] FIXME: H264双流的分辨率暂不考虑，后续处理
			CallLog( "Mt%d open mcu second video logicChannel(0x%x:%d:H239.%d:%s), Dualmode: %d\n", 
					  tMt.GetMtId(), dwRcvIp, wRcvPort + 4, tLogicChnnl.IsSupportH239(), 
					  GetMediaStr(tLogicChnnl.GetChannelType()), m_tConf.GetConfAttrb().GetDualMode() );			
			break;

		case MODE_AUDIO:
			byChannel = LOGCHL_AUDIO;
			tLogicChnnl.m_tRcvMediaChannel.SetPort( wRcvPort + 2 );
			tLogicChnnl.m_tRcvMediaCtrlChannel.SetPort( wRcvPort + 3 );
			CallLog( "Mt%d open mcu audio logicChannel(0x%x:%d:%s)\n", 
					tMt.GetMtId(), dwRcvIp, wRcvPort+2, GetMediaStr( tLogicChnnl.GetChannelType() ) );
			break;
			
		case MODE_DATA:
			if( tLogicChnnl.GetChannelType() == MEDIA_TYPE_H224 )
			{
				byChannel = LOGCHL_H224DATA;
				dwRcvIp = g_cMcuVcApp.GetMtAdpIpAddr( tMt.GetDriId() );
				tLogicChnnl.m_tRcvMediaChannel.SetIpAddr( dwRcvIp );
				tLogicChnnl.m_tRcvMediaChannel.SetPort( wRcvPort + 6 );
				tLogicChnnl.m_tRcvMediaCtrlChannel.SetPort( wRcvPort + 7 );
				CallLog( "Mt%d open mcu H.224 logicChannel(0x%x:%d:%s)\n", 
					tMt.GetMtId(), dwRcvIp, wRcvPort + 6 , GetMediaStr( tLogicChnnl.GetChannelType() ) );
				
			}
			//对端请求打开本端的T120数据通道
			else if( tLogicChnnl.GetChannelType() == MEDIA_TYPE_T120 )
			{
				//此时, 本端只能是被呼. 对端如果是KEDA MCU, 其地址应该为空
				//填写本端的DCS监听地址, 回复ACK, 由对端主MCU发起邀请
				if ( !m_ptMtTable->IsNotInvited( tMt.GetMtId() ) )
				{
					CallLog( "Mt%d should be calling in !\n", tMt.GetMtId() );
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
						CallLog( "Mcu open Mt%d data T.120 logicChannel(0x%x:%d:%s) as slave success!\n", 
							tMt.GetMtId(), dwDcsIp, wDcsPort, GetMediaStr( tLogicChnnl.GetChannelType() ) );		
					}
					else
					{
						CallLog( "Get local DCS Ip failed in open logic channel ack !\n" );
						return;
					}
				}				
			} 
			else if( tLogicChnnl.GetChannelType() == MEDIA_TYPE_MMCU )
			{			
				if( !m_tConf.GetConfAttrb().IsSupportCascade() ||
					tMt.GetMtType() != MT_TYPE_MMCU || 
					m_tConfAllMtInfo.GetCascadeMcuNum() >= MAXNUM_SUB_MCU)
				{
					ConfLog( FALSE, "Mt%d 0x%x(Dri:%d) not support mmcu(%s), nack!\n",
						tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() , GetMediaStr(tLogicChnnl.GetChannelType()));
					SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
					return;	
				}
                else if ( !m_tCascadeMMCU.IsNull() )
                {
                    // guzh [4/12/2007] 如果已经有上级MCU，则检查是否就是该MCU
                    // 这里不能绝对采用IP比较，上级可能换接入板。只能首先尝试别名或者E164（有可能会发生误判)
                    BOOL32 bRejectAndRemoveMMcu = FALSE;
                    if( m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
                    {
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
                        CallLog("Mt%d 0x%x(Dri:%d) calling in seems to be the MMCU, Drop both!",
                                tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId());

                        SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);
                        SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);   
                    }
                    else
                    {
                        CallLog( "Mt%d 0x%x(Dri:%d) calling in, but current has MMCU, nack!\n",
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
				
				CallLog( "Mt%d open mcu mmcu logicChannel(0x%x:%d:%s)\n", 
					tMt.GetMtId(), dwRcvIp, wRcvPort+9, GetMediaStr( tLogicChnnl.GetChannelType() ) );

				//本终端是否已经通过密码验证
				if( m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
				{
					m_tCascadeMMCU = tMt;
					m_tConfAllMtInfo.m_tMMCU = tMt;
                    m_tConfInStatus.SetNtfMtStatus2MMcu(TRUE);
					
					//将此级联mcu信息进行添加置列表中
					m_ptConfOtherMcTable->AddMcInfo( tMt.GetMtId() );	
					m_tConfAllMtInfo.AddMcuInfo( tMt.GetMtId(), tMt.GetConfIdx() );

					//通知会控
					cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
					SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
					
					//发给会控会议所有终端信息
					cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
					SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

					//下级不再自动将上级作为发言人
					if(m_tConf.GetSpeaker().IsNull() && g_cMcuVcApp.IsMMcuSpeaker())
					{
						ChangeSpeaker(&m_tCascadeMMCU);
					}
				}

			}
			break;
		}
        
        // zbq [10/11/2007] 前向纠错动态载荷类型增加 音频/视频/双流 支持
        BOOL32 bLogicChanFEC = FALSE;
        if ( tLogicChnnl.GetFECType() != FECTYPE_NONE )
        {
            bLogicChanFEC = TRUE;
            tLogicChnnl.SetActivePayload(MEDIA_TYPE_FEC);
            CallLog("Mt.%d set ActivePayload.%d due to FEC\n", tMt.GetMtId(), MEDIA_TYPE_FEC);
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
        
		//保存
		m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), byChannel, &tLogicChnnl, FALSE );

		//zbq[09/02/2008] 双流延迟到这里发起
// 		if (MODE_SECVIDEO == tLogicChnnl.GetMediaType() && bStartDS)
// 		{
// 			StartDoubleStream( tMt, tLogicChnnl );
// 		}

#ifdef _SATELITE_
		//卫星环境下，缺乏进一步的消息驱动，不作延后发起
		if (MODE_SECVIDEO == tLogicChnnl.GetMediaType())
		{
			StartDoubleStream( tMt, tLogicChnnl );
		}
#endif

        //终端上行带宽初始化
        if(tLogicChnnl.GetMediaType() == MODE_VIDEO && byChannel == LOGCHL_VIDEO)
        {
            m_ptMtTable->SetSndBandWidth(tMt.GetMtId(), tLogicChnnl.GetFlowControl());
        }

		//第二路视频的丢包重传
		if( byChannel == LOGCHL_SECVIDEO)
		{
			if(m_tConf.GetConfAttrb().IsResendLosePack())
			{
				ChangePrsSrc( tMt, PRSCHANMODE_SECOND);
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
			m_ptMtTable->SetMtVideo2Send(tMt.GetMtId(), TRUE);
		}

		cServMsg.SetMsgBody( ( u8 * )&tLogicChnnl, sizeof( tLogicChnnl ) );

#ifndef _SATELITE_
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
#else
		if( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
		{
			//卫星会议ACK应答
			CServMsg cMsg;
			cMsg.SetConfIdx(m_byConfIdx);
			cMsg.SetDstMtId(tMt.GetMtId());
			cMsg.SetEventId(30255 /*MCU_MT_STARTDS_REQ + 1*/);
			
			u16 wBitRate = GetDoubleStreamVideoBitrate(m_ptMtTable->GetDialBitrate(tMt.GetMtId()), FALSE);
			wBitRate = htons(wBitRate);
			u8 byDSMsgMode = 1;
			cMsg.SetMsgBody(&byDSMsgMode, sizeof(u8));
			cMsg.CatMsgBody((u8*)&wBitRate, sizeof(u16));
			SendMsgToMt(tMt.GetMtId(), 30255/*MCU_MT_STARTDS_REQ + 1*/, cMsg);

		}
#endif
		if (!m_tCascadeMMCU.IsNull() &&
			m_tConfAllMtInfo.MtJoinedConf(m_tCascadeMMCU.GetMtId()) &&
			(tLogicChnnl.GetMediaType() == MODE_VIDEO ||
			 tLogicChnnl.GetMediaType() == MODE_AUDIO))
		{
			OnNtfMtStatusToMMcu(m_tCascadeMMCU.GetMtId(), tMt.GetMtId());
		}
	
		//本终端可能未通过密码验证
		if( !m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
		{
			CallLog( "Mt%d doesnot join conf, then doesnot deal audio/video switch ... \n", 
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
			if ( tLogicChnnl.GetMediaType() == MODE_VIDEO )
			{
				AdjustChairmanVidSwitch();
			}
        }

		//发送视频码流处理
		if( tLogicChnnl.GetMediaType() == MODE_VIDEO && byChannel == LOGCHL_VIDEO )
		{
			//吃下码流, 转入dump，防止icmp
			g_cMpManager.StartRecvMt( tMt, MODE_VIDEO );
            
            //zbq[07/18/2007] SSRC 的修改由归一重整统一处理
			//g_cMpManager.ResetRecvMtSSRC( tMt, MODE_VIDEO );

			//若是发言人加入
			if(GetLocalSpeaker() == tMt )
			{
                // xsl [8/22/2006]卫星分散会议时需要判断回传通道数
                if (m_tConf.GetConfAttrb().IsSatDCastMode() && IsOverSatCastChnnlNum(tMt.GetMtId()))
                {
                    ConfLog(FALSE, "[ProcMtMcuOpenLogicChnnlReq] over max upload mt num. cancel speaker!\n");
                    ChangeSpeaker(NULL);
                }
                else
                {
                    ChangeSpeaker( &tMt );
                }				
			}

			if( !( GetLocalSpeaker() == tMt ) )
			{
				if( tMt.GetMtType() == MT_TYPE_MMCU || tMt.GetMtType() == MT_TYPE_SMCU )
				{				
					NotifyMtSend( tMt.GetMtId(), MODE_VIDEO, TRUE );
				}
				else
				{
                    //  xsl [1/18/2006] 如果在vmp或tvwall模板里不发flowcontrol 0                   
                    if (!m_tConfEqpModule.IsMtInVmp(tMt) && 
                        FALSE == m_tConfEqpModule.IsMtInTvwall(tMt))
                    {
                        NotifyMtSend( tMt.GetMtId(), MODE_VIDEO, FALSE );
                    }					
				}			
			}

            //有关画面合成的处理
            if( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE )
            {
				TPeriEqpStatus tPeriEqpStatus;
                g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;

                if( IsDynamicVmp() )
                {
                    if (m_tConfInStatus.IsVmpNotify())
                    {
                        ChangeVmpStyle(tMt, TRUE);
                    }
                    else
                    {
                        ChangeVmpStyle(tMt, TRUE, FALSE);
                    }
                }
                else if( m_tConf.GetConfAttrb().IsHasVmpModule() && 
                         !m_tConfInStatus.IsVmpModuleUsed() )
                {
					TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
                    u8 byChlNum = tVmpParam.GetMaxMemberNum();
                    u8 byMemberType = 0;
					u8 byVmpStyle = m_tConf.m_tStatus.GetVmpStyle();
                    for( u8 byLoop = 0; byLoop < byChlNum; byLoop++ )
                    {
                        if( m_tConfEqpModule.IsMtAtVmpChannel( byLoop, tMt, byMemberType ) )
                        {
                            UpdataVmpEncryptParam(tMt, byLoop);
                            Mt2Log("[ProcMtMcuOpenLogicChnnlReq] tMt<%d, %d>, byChnnl:%d, mode:%d\n",
                                tMt.GetMcuId(),tMt.GetMtId(), byLoop, byMemberType);

                            //SetVmpChnnl( tMt, byLoop, VMP_MEMBERTYPE_MCSSPEC, TRUE );
                            // 顾振华 [5/11/2006] 支持指定跟随方式
                            // 这里只处理本终端出现在Vmp成员里面。
                            // 如果是跟随的话，在改变主席/发言人时会处理
							// xliang [3/19/2009] FIXME： 对于MPU不能无条件将该MT码流就交换给VMP
							if( byVmpSubType == VMP )
							{
								SetVmpChnnl( tMt, byLoop, byMemberType, TRUE );
							}
							else
							{
								ChangeMtVideoFormat( tMt, &tVmpParam );
								SetVmpChnnl( tMt, byLoop, byMemberType, TRUE );
							}
                                                        
                        }
                    }
                    //同步外设复合状态
                    TPeriEqpStatus tPeriEqpStatus;
                    g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
                    tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
                    g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );

                }
            }

			//判断是否为回传通道,是则恢复回传交换
			if( !m_tCascadeMMCU.IsNull() && !(m_tCascadeMMCU == tMt) )
			{
				TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
				if( ptConfMcInfo != NULL && 
					( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
				{
					if( ptConfMcInfo->m_tSpyMt.GetMtId() == tMt.GetMtId() &&
						ptConfMcInfo->m_tSpyMt.GetMcuId() == tMt.GetMcuId() )
					{
						ptConfMcInfo->m_tSpyMt.SetNull();
						StartSwitchToMcu( tMt, 0, m_tCascadeMMCU.GetMtId(), MODE_VIDEO, SWITCH_MODE_SELECT);
						ptConfMcInfo->m_tSpyMt = tMt;
					}
				}
			}

			//有电视墙模板并且所请求的设备已连接的话
            if (m_tConf.GetConfAttrb().IsHasTvWallModule() )
            {
                u8 byMemberType = 0;
                for(u8 byTvPos = 0; byTvPos < MAXNUM_PERIEQP_CHNNL; byTvPos++)
                {
                    u8 byTvWallId = m_tConfEqpModule.m_tTvWallInfo[byTvPos].GetTvWallEqp().GetEqpId();
                    if( 0 != byTvWallId && g_cMcuVcApp.IsPeriEqpConnected(byTvWallId) )
                    {
                        for(u8 byTvChlLp = 0; byTvChlLp < MAXNUM_PERIEQP_CHNNL; byTvChlLp++)
                        {
                            // 同上，这里暂时只处理了本终端出现在Tw成员里面。
                            // 如果是指定了其他跟随，则应该在改变主席/发言人的时候自动会调整
                            if( m_tConfEqpModule.IsMtInTvWallChannel( byTvWallId, byTvChlLp, tMt, byMemberType) )
                            {
                                // 顾振华 [5/11/2006] 支持指定类型的开始方式
                                //ChangeTvWallSwitch(&tMt, byTvWallId, byTvChlLp, TW_MEMBERTYPE_MCSSPEC, TW_STATE_START);
                                ChangeTvWallSwitch(&tMt, byTvWallId, byTvChlLp, byMemberType, TW_STATE_START);
                            }
                        }
                    }
                }
            }
		}
	
		//发送音频码流处理	
		if( tLogicChnnl.GetMediaType() == MODE_AUDIO )
		{
			//吃下码流, 转入dump，防止icmp
			g_cMpManager.StartRecvMt( tMt, MODE_AUDIO );
            
            //zbq[07/18/2007] SSRC 的修改由归一重整统一处理
			//g_cMpManager.ResetRecvMtSSRC( tMt, MODE_AUDIO );

			//若是发言人加入, 考虑到纯音频时的发言人也需此处理
			if(GetLocalSpeaker() == tMt )
			{
				//纯音频不需考虑视频复合情况
				ChangeSpeaker( &tMt, FALSE, FALSE );		
			}

            // xsl [8/4/2006] vac或讨论模式下将此终端加入混音通道，并建立到混音器的交换
			if ( (  m_tConf.m_tStatus.IsVACing() ||
                    m_tConf.m_tStatus.IsAutoMixing() || 
                    m_tConf.m_tStatus.IsSpecMixing() && tMt == GetLocalSpeaker() )
                 && GetMixMtNumInGrp() < GetMaxMixNum() )
			{
                // guzh [1/31/2007] 8000B 能力限制
                u16 wError = 0;
                if ( CMcuPfmLmt::IsMtOprSupported( m_tConf, m_tConfAllMtInfo.GetLocalJoinedMtNum(), GetMixMtNumInGrp(), wError ) )
                {
                    AddMixMember( &tMt, DEFAULT_MIXER_VOLUME, FALSE );
                    StartSwitchToPeriEqp(tMt, 0, m_tMixEqp.GetEqpId(), 
                                         (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt.GetMtId())),
                                         MODE_AUDIO, SWITCH_MODE_SELECT);
                }
                else
                {
                    // 给出提示，根据设定策略决定执行
                    SwitchDiscuss2SpecMix();
                    wError = ERR_MCU_DISCUSSAUTOCHANGE2SPECMIX;
                    NotifyMcsAlarmInfo(0, wError);  // 提示MCS
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
				TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
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

            //有电视墙模板并且所请求的设备已连接的话(不支持只有音频的终端电视墙，后续考虑)
/*            u8 byTvWallId = m_tConfEqpModule.GetTvWallEqp().GetEqpId();
            if (m_tConf.GetConfAttrb().IsHasTvWallModule() && 
                g_cMcuVcApp.IsPeriEqpConnected(byTvWallId))
            {
                for(u8 byLoop = 0; byLoop < MAXNUM_PERIEQP_CHNNL; byLoop++)
                {
                    if (m_tConfEqpModule.IsMtAtTvwallChannel(byLoop, tMt))
                    {
                        ChangeTvWallSwitch(&tMt, byTvWallId, byLoop, TW_MEMBERTYPE_MCSSPEC, TW_STATE_START);
                    }
                }
            }*/
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
		}

		break;
        }
	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
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
                MtLog("[ProcMtMcuMediaLoopOpr] mt can't loop in double stream mode.\n");
                return;
            }

            if (!g_cMpManager.StartSwitchToSubMt(tLoopMt, 0, tLoopMt, byMode, SWITCH_MODE_SELECT))
            {
                cServMsg.SetEventId(pcMsg->event+2);
                SendMsgToMt(cServMsg.GetSrcMtId(), pcMsg->event+2, cServMsg);
                MtLog( "[ProcMtMcuMediaLoopOpr] start switch failed, byMode<%d>!\n", byMode );
                return;
            }

            //ack
            cServMsg.SetEventId(pcMsg->event+1);
            SendMsgToMt(cServMsg.GetSrcMtId(), pcMsg->event+1, cServMsg);
            
            NotifyMtReceive(tLoopMt, tLoopMt.GetMtId());

            //置环回标记
            tMtStatus.SetMediaLoop(byMode, TRUE);
            m_ptMtTable->SetMtStatus(tLoopMt.GetMtId(), &tMtStatus);

            MtLog( "[ProcMtMcuMediaLoopOpr] mt<%d> mode<%d> start media loop.\n", tLoopMt.GetMtId(), byMode );
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
                    if (GetMixMtNumInGrp() < GetMaxMixNum())
                    {
                        AddMixMember( &tLoopMt, DEFAULT_MIXER_VOLUME, FALSE );
				        StartSwitchToPeriEqp(tLoopMt, 0, m_tMixEqp.GetEqpId(), 
					                         (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tLoopMt.GetMtId())), 
									         MODE_AUDIO, SWITCH_MODE_SELECT);	
                    }                    
                }           

                MtLog( "[ProcMtMcuMediaLoopOpr] mt<%d> mode<%d> stop media loop.\n", tLoopMt.GetMtId(), byMode );
            }
	        else
            {
                MtLog( "[ProcMtMcuMediaLoopOpr] mt<%d> mode<%d> not in media loop. ignore it!\n", 
                        tLoopMt.GetMtId(), byMode );
            }
		}
		break;

	default:
		ConfLog( FALSE, "[ProcMtMcuMediaLoopOpr] Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

	return;
}

/*=============================================================================
    函 数 名： ProcMtMcuGetH239TokenReq
    功    能： 终端给MCU的 获取 H239令牌 权限请求
    算法实现： 
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
	
    	//判断双流的发起源
	if ( CONF_DUALMODE_EACHMTATWILL == m_tConf.GetConfAttrb().GetDualMode() ||
		 ( CONF_DUALMODE_SPEAKERONLY == m_tConf.GetConfAttrb().GetDualMode() &&
		   tMt == GetLocalSpeaker() ) ) 
	{
    }
    else
	{
		Mt2Log( "[ProcMtMcuGetH239TokenReq] MtId.%d, ManuId.%d, DStreamMtId.%d open double stream with Wrong DualMode, NACK !\n", 
			tMt.GetMtId(), m_ptMtTable->GetManuId( tMt.GetMtId() ), m_tDoubleStreamSrc.GetMtId() );	

        cServMsg.SetEventId(MCU_MT_GETH239TOKEN_NACK);	
	    SendMsgToMt( tMt.GetMtId(), MCU_MT_GETH239TOKEN_NACK, cServMsg);
        return;
	}

	//REQUEST RELEASING TOKEN OF LAST MT OWNER
	if( !m_tH239TokenOwnerInfo.m_tH239TokenMt.IsNull() )
	{
		if( !(m_tH239TokenOwnerInfo.m_tH239TokenMt == tMt) )
		{
			TH239TokenInfo tH239Info;
			tH239Info.SetChannelId( m_tH239TokenOwnerInfo.GetChannelId() );
			tH239Info.SetSymmetryBreaking( 0 );
			cServMsg.SetEventId(MCU_MT_RELEASEH239TOKEN_CMD);
			cServMsg.SetMsgBody((u8 *)&m_tH239TokenOwnerInfo.m_tH239TokenMt, sizeof(TMt));
			cServMsg.CatMsgBody((u8 *)&tH239Info, sizeof(TH239TokenInfo));
			SendMsgToMt( m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId(), MCU_MT_RELEASEH239TOKEN_CMD, cServMsg);
		}
	}

	if( !m_tDoubleStreamSrc.IsNull() )
	{
		//SONY:------FIRST OPENCHANNEL,NEXT REQUEST TOKEN
		//CLOSE LAST CHANNELS
		if( !(m_tDoubleStreamSrc == tMt) )
		{
			StopDoubleStream( TRUE, FALSE );
		}
	}	

    cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
	cServMsg.CatMsgBody((u8 *)&tSrcH239Info, sizeof(TH239TokenInfo));	
		
	//POLYCOM7000的静态双流时,发送通道在一开始打开后一直不关闭，使用H239Token进行发送激活或关闭
	//IF CHANNEL HAS BEEN OPENED, START DOUBLESTREAM,
	//ELSE IF TOKEN HAS BEEN OWNED, START DOUBLESTREAM WHEN RECEIVED OPENLOGICAL_ERQ...
	TLogicalChannel tH239LogicChnnl;
	if( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) && 
		m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tH239LogicChnnl, FALSE ) )
	{
        // xsl [7/20/2006]卫星分散会议时需要考虑回传通道数
        if (m_tConf.GetConfAttrb().IsSatDCastMode() && IsOverSatCastChnnlNum(tMt.GetMtId()))
        {
            ConfLog(FALSE, "[ProcMtMcuGetH239TokenReq] over max upload mt num.\n");
            cServMsg.SetEventId(MCU_MT_GETH239TOKEN_NACK);	
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
	
	// xliang [11/14/2008] polycomMCU不认为kedaMCU有Token管理权，故不请求Token.
	//其直接在此发TokenOwnerInd就表示当前PolycomMCU要发双流，这里起或抢...
	if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
		( MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
		MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId())) )
	{
		//带宽合法，开启双流
		TLogicalChannel tFstLogicChnnl;
		TLogicalChannel tSecLogicChnnl;
		
		if ( m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_VIDEO, &tFstLogicChnnl, FALSE) )
		{
			u16 wFstBR = tFstLogicChnnl.GetFlowControl();
			u16 wDailBR = m_ptMtTable->GetDialBitrate(tMt.GetMtId());
			u16 wSecBR = wDailBR - wFstBR;
			
			m_ptMtTable->GetMtLogicChnnl(tMt.GetMtId(), LOGCHL_SECVIDEO, &tSecLogicChnnl, FALSE);
			tSecLogicChnnl.SetFlowControl(wSecBR);
			
			if ( wSecBR * 100 / wDailBR > 10 )
			{
				//非polycomMCU发起的双流，冲掉再来
				if ( m_tDoubleStreamSrc == tMt )
				{
					CallLog( "[ProcMtMcuOwnH239TokenNotify] DS has been started, ignore the same Ind\n" );
				}
				else
				{
					//是否满足当前的双流约束
					if ( CONF_DUALMODE_EACHMTATWILL == m_tConf.GetConfAttrb().GetDualMode() ||
						( CONF_DUALMODE_SPEAKERONLY == m_tConf.GetConfAttrb().GetDualMode() &&
						tMt == GetLocalSpeaker() ))
					{
						StartDoubleStream(tMt, tSecLogicChnnl);	
						CallLog( "[ProcMtMcuOwnH239TokenNotify] tMt.%d, wSecBR.%d, wDailBR.%d, StartDS\n",
							tMt.GetMtId(), wSecBR, wDailBR );
					}
					else
					{
						CallLog( "[ProcMtMcuOwnH239TokenNotify] tMt.%d, wSecBR.%d, wDailBR.%d, should but no StartDS due to dual mode.%d\n",
							tMt.GetMtId(), wSecBR, wDailBR, m_tConf.GetConfAttrb().GetDualMode() );
					}
				}
			}
			else
			{
				CallLog(  "[ProcMtMcuOwnH239TokenNotify] tMt.%d, wSecBR.%d, wDailBR.%d, No StartDS\n",
					tMt.GetMtId(), wSecBR, wDailBR );
			}
		}
		
		//释放当前的双流源(非polycomMCU)
		if (!m_tH239TokenOwnerInfo.m_tH239TokenMt.IsNull() &&
			!(m_tH239TokenOwnerInfo.m_tH239TokenMt == tMt))
		{
			TH239TokenInfo tH239Info;
			tH239Info.SetChannelId( tSrcH239Info.GetChannelId() );
			tH239Info.SetSymmetryBreaking( 0 );
			cServMsg.SetEventId(MCU_MT_RELEASEH239TOKEN_CMD);
			cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
			cServMsg.CatMsgBody((u8 *)&tH239Info, sizeof(TH239TokenInfo));
			SendMsgToMt( tMt.GetMtId(), MCU_MT_RELEASEH239TOKEN_CMD, cServMsg);
		}
		return;
	}

	if( m_tH239TokenOwnerInfo.m_tH239TokenMt.IsNull() )
	{
		return;
	}
	//SEND OTHER MT TOKEN INDICATION
	else if( m_tH239TokenOwnerInfo.m_tH239TokenMt == tMt )
	{
		NotifyH239TokenOwnerInfo( NULL );
	}
	//REQUEST RELEASING TOKEN OF LAST MT OWNER
	else
	{
		TH239TokenInfo tH239Info;
		tH239Info.SetChannelId( tSrcH239Info.GetChannelId() );
		tH239Info.SetSymmetryBreaking( 0 );
		cServMsg.SetEventId(MCU_MT_RELEASEH239TOKEN_CMD);
		cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
		cServMsg.CatMsgBody((u8 *)&tH239Info, sizeof(TH239TokenInfo));
		SendMsgToMt( tMt.GetMtId(), MCU_MT_RELEASEH239TOKEN_CMD, cServMsg);
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
		m_tH239TokenOwnerInfo.Clear();

		//POLYCOM7000的静态双流时,发送通道在一开始打开后一直不关闭，使用H239Token进行发送激活或关闭
		if( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) && 
			m_tDoubleStreamSrc == tMt )
		{
			StopDoubleStream( FALSE, FALSE );
		}

		// xliang [11/14/2008] 同时释放 POLYMCU 的TOKEN
		for ( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++)
		{
			if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(byMtId) &&
				( MT_TYPE_SMCU == m_ptMtTable->GetMtType(byMtId) ||
				MT_TYPE_MMCU == m_ptMtTable->GetMtType(byMtId) ) )
			{
				CServMsg cMsg;
				cMsg.SetEventId(MCU_POLY_RELEASEH239TOKEN_CMD);
				SendMsgToMt(byMtId, MCU_POLY_RELEASEH239TOKEN_CMD, cServMsg);
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
				g_cMpManager.StartSwitchToSubMt(m_tDoubleStreamSrc, 0, tPolyMt, MODE_SECVIDEO);
			}
			else
			{
				ConfLog( FALSE, "[ProcPolyMCUH239Rsp] PolyMcu.%d Fwd chnnl should not be closed, reinvite it, please\n", tPolyMt.GetMtId() );
			}
			CallLog( "[ProcPolyMCUH239Rsp] PolyMcu.%d accept TokenReq, POLY_MCU_GETH239TOKEN_ACK received\n", tPolyMt.GetMtId() );
			break;
		}
	case POLY_MCU_GETH239TOKEN_NACK:		//获取PolyMCU的H239TOKEN 拒绝应答
		{
			ConfLog(FALSE, "[ProcPolyMCUH239Rsp] GETH239TOKEN_NACK\n");
			break;
		}
	case POLY_MCU_OWNH239TOKEN_NOTIF:		//PolyMCU通知当前的TOKEN的拥有者
		{
			//FIXME：消息体是什么，打印一下，要不要作响应
			//谁拥有了H239
			ConfLog(FALSE, "[ProcPolyMCUH239Rsp] POLY_MCU_OWNH239TOKEN_NOTIF\n");
			break;
		}
	case POLY_MCU_RELEASEH239TOKEN_CMD:		//PolyMCU释放H329TOKEN 命令
		{
			//双流和第一路Flowctrl回去
			ConfLog(FALSE, "[ProcPolyMCUH239Rsp] POLY_MCU_RELEASEH239TOKEN_CMD\n");
			break;
		}
	default:
		ConfLog( FALSE, "[ProcPolyMCUH239Rsp] unexpected msg.%d<%s>\n", pcMsg->event, OspEventDesc(pcMsg->event) );
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
	
	CallLog( "Mt%d close Out.%d logical channel received!\n", tMt.GetMtId(), bOut );

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
			// xliang [12/19/2008] 对于上次的双流源MT，在此处OLC(ds) 
			if(tMt.GetMtId() == m_byLastDsSrcMtId)
			{
				TCapSupport tCapSupport;
				
				if(!m_ptMtTable->GetMtCapSupport( tMt.GetMtId(), &tCapSupport ))
				{
					ConfLog(FALSE,"[ProcMtMcuCloseLogicChnnlNotify]can't get last Ds Src Mt.%u's Cap, Fail to Open ds olc!\n",tMt.GetMtId());
				}
				else
				{
					McuMtOpenDoubleStreamChnnl(tMt, m_tLogicChnnl, tCapSupport);
					CallLog("[ProcMtMcuCloseLogicChnnlNotify]after delay,start ds! \t tLogicChnnl's VideoFormat is%u.\n",m_tLogicChnnl.GetVideoFormat());
				}
			}
		}
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
			m_tCascadeMMCU.SetNull();
			m_tConfAllMtInfo.m_tMMCU.SetNull();
            m_tConfInStatus.SetNtfMtStatus2MMcu(FALSE);
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
			if( m_tDoubleStreamSrc == tMt )
			{
				StopDoubleStream( FALSE, FALSE );
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
            if ( !m_tConf.GetConfAttrb().IsSatDCastMode() )
            {
                g_cMpManager.StopSwitchToSubMt( tMt, MODE_SECVIDEO, TRUE  );
            }
		}
	}
	else if( MODE_VIDEO == tLogicChnnl.GetMediaType() )
	{
		byChannelType = LOGCHL_VIDEO;
        // 卫星会议不停交换 [6/2/2006]
        if ( !m_tConf.GetConfAttrb().IsSatDCastMode() )
        {
		    if(bOut)
		    {
			    StopSwitchToSubMt( tMt.GetMtId(), MODE_VIDEO );
		    }
		    else
		    {
		    }
        }
	}
	else if( MODE_AUDIO == tLogicChnnl.GetMediaType() )
	{
		byChannelType = LOGCHL_AUDIO;
        // 卫星会议不停交换 [6/2/2006]
        if ( !m_tConf.GetConfAttrb().IsSatDCastMode() )
        {
		    if(bOut)
		    {
			    StopSwitchToSubMt( tMt.GetMtId(), MODE_AUDIO );
		    }
		    else
		    {
		    }
        }
	}

	m_ptMtTable->SetMtLogicChnnl( tMt.GetMtId(), byChannelType, &tNullLogicChnnl, bOut );

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
void CMcuVcInst::ProcMtCapSet( TMt tMt, TCapSupport &tCapSupport, const TCapSupportEx &tCapSupportEx )
{
    TLogicalChannel tDataLogicChnnl;
    TLogicalChannel tLogicalChannel;
    TLogicalChannel tH239LogicChnnl;
    

    //zbq[04/09/2009] Tandberg 能力作弊
    if (MT_MANU_TAIDE == m_ptMtTable->GetManuId((tMt.GetMtId())) &&
        IsHDConf(m_tConf))
    {
        if (g_cMcuVcApp.IsSendFakeCap2Taide())
        {
            tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_CIF);
        }
        else if (g_cMcuVcApp.IsSendFakeCap2TaideHD())
        {
            tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_HD720);
        }
    }

	TMt tNullMt;
	tNullMt.SetNull();
	u8 byDriId = m_ptMtTable->GetDriId(tMt.GetMtId());
#ifdef _LINUX_
	// xliang [10/27/2008] 对于HD MT且内嵌接入情况下的处理
	if(tCapSupport.GetMainVideoType() == MEDIA_TYPE_H264 &&
		(tCapSupport.GetMainVideoResolution() == VIDEO_FORMAT_HD1080 ||
		 tCapSupport.GetMainVideoResolution() == VIDEO_FORMAT_HD720) )
	{
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
		if ( g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_wMtNum +1 > g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_byMaxMtNum )
		{
			// xliang [2/16/2009]  该判断应该进不来,在mtadp那已进行过滤。
			CallLog("[ProcMtCapSet]Take MCU as 2 into account of Mtadp access capabilty: fail!\n");
			NotifyMcsAlarmInfo( 0, ERR_MCU_CALLHDMTFAIL);
			RemoveJoinedMt( tMt, TRUE, MTLEFT_REASON_NORMAL, TRUE);
			return;
		}
		else
		{
			g_cMcuVcApp.m_atMtAdpData[byDriId-1].m_wMtNum++;
		}
	}
    CallLog( "[ProcMtCapSet] Mt main video cap: type.%d, Res.%d \n", tCapSupport.GetMainVideoType(), tCapSupport.GetMainVideoResolution() );

	u32	dwRcvIp;
	u16	wRcvPort;
	if( !m_ptMtTable->GetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort ) )
	{
		dwRcvIp = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( tMt.GetMtId() ) );
		if( dwRcvIp == 0 )
		{
			return;			    
		}
		wRcvPort = g_cMcuVcApp.AssignMtPort( tMt.GetConfIdx(), tMt.GetMtId() );
		m_ptMtTable->SetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort );
	}

	TLogicalChannel	tAudioLogicChnnl;
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
    
    //zbq[10/13/2007] FEC能力不足，拒绝打开终端的通道
    BOOL32 bFECOpenAudioChnnl = TRUE;
    BOOL32 bFECOpenVideoChnnl = TRUE;
    BOOL32 bFECOpenDVideoChnnl = TRUE;

	CServMsg cServMsg;	
	//发送打开终端语音逻辑通道信息
	if( bFECOpenAudioChnnl &&
        tAudioLogicChnnl.GetChannelType() != MEDIA_TYPE_NULL )
	{
		u16 wAudioBand = GetAudioBitrate( m_tConf.GetMainAudioMediaType() );
		tAudioLogicChnnl.SetFlowControl( wAudioBand );
		tAudioLogicChnnl.m_tSndMediaCtrlChannel.SetIpAddr( dwRcvIp );
		tAudioLogicChnnl.m_tSndMediaCtrlChannel.SetPort( wRcvPort + 3 );
		tAudioLogicChnnl.SetMediaEncrypt(m_tConf.GetMediaKey());
		tAudioLogicChnnl.SetActivePayload(GetActivePayload(m_tConf, tAudioLogicChnnl.GetChannelType()));
		
		cServMsg.SetMsgBody( ( u8 * )&tAudioLogicChnnl, sizeof( tAudioLogicChnnl ) );
		SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );

		//打印消息
		CallLog( "Open audio logicChannel request send to Mt%d, type: %s\n", 
				tMt.GetMtId(), GetMediaStr( tAudioLogicChnnl.GetChannelType() ) );
		
	}
	else
	{
		VCSMTAbilityNotif(tMt, MODE_AUDIO);
		//打印消息
		CallLog( "[ProcMtCapSet] Mt%d not support any conf audio media type, NO open audio logicChannel request send to it!\n", tMt.GetMtId() );
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
            wMtMaxRecvBitrate = max( wMtMaxRecvBitrate, tMtSecSCS.GetVideoMaxBitRate() );
        }
        // guzh [7/16/2007] 老版本终端的视频逻辑通道是总码率-64
        wMtMaxRecvBitrate += 64;

        wMtMaxRecvBitrate = min ( wMtMaxRecvBitrate, wCallRate );
    }
    else
    {
        wMtMaxRecvBitrate = wCallRate;
    }        
    
    //zbq[12/18/2007]逻辑通道打开的判断依赖呼叫带宽限制
    if ( wMtMaxRecvBitrate < m_ptMtTable->GetDialBitrate(tMt.GetMtId()))
    {
        bOpenVideoChnnl = FALSE;
        CallLog("Open Mt.%d video logic chnnl failed due to ChnnlRcvBR.%d, DailBR.%d\n",
            tMt.GetMtId(), wMtMaxRecvBitrate, m_ptMtTable->GetDialBitrate(tMt.GetMtId()));
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
        CallLog("Open Mt.%d video logic chnnl failed due to src<%d-%s> not matched!\n", 
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
        
		u8 byChnRes = 0;
		if (!GetMtMatchedRes(tMt.GetMtId(), tVideoLogicChnnl.GetChannelType(), byChnRes))
		{
			//zbq[04/22/2009] 主能力即使打开也不能适配的情况下，重新按辅能力构造打开
            if (MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType())
            {
                tVideoLogicChnnl.SetChannelType(m_tConf.GetSecVideoMediaType());
                tVideoLogicChnnl.SetActivePayload(GetActivePayload(m_tConf, tVideoLogicChnnl.GetChannelType()));
                byChnRes = m_tConf.GetSecVideoFormat();

                CallLog("[ProcMtCapSet] no proper res for mt(%d) with mediatype(%d), try SecVidCap!\n", 
                    tMt.GetMtId(), tVideoLogicChnnl.GetChannelType());
            }
            else
            {
                bOpenVideoChnnl = FALSE;
                CallLog("[ProcMtCapSet] no proper res for mt(%d) with mediatype(%d)!\n", 
                    tMt.GetMtId(), tVideoLogicChnnl.GetChannelType());
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
				CallLog("[ProcMtCapSet] Mt.%d 's video open res has been adjusted from 4CIF to 720p!\n");
			}

			tVideoLogicChnnl.SetVideoFormat(byChnRes);
			cServMsg.SetMsgBody( ( u8 * )&tVideoLogicChnnl, sizeof( tVideoLogicChnnl ) );
			cServMsg.CatMsgBody( &byChnRes, sizeof(u8) );
			SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );
			
			//打印消息
			CallLog( "[ProcMtCapSet] Open Video LogicChannel Request send to Mt%d, type: %s, res.%d\n", 
				tMt.GetMtId(), GetMediaStr( tVideoLogicChnnl.GetChannelType() ), tVideoLogicChnnl.GetVideoFormat() );
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
		CallLog( "[ProcMtCapSet] Mt%d not support any conf video media type, NO open video logicChannel request send to it!\n", tMt.GetMtId() );
	}


	//打开终端的双流通道 
	if( bFECOpenVideoChnnl &&
        bOpenVideoChnnl &&
        //m_ptMtTable->GetMtLogicChnnl( m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE ) &&
		!m_tDoubleStreamSrc.IsNull() &&
		m_tConf.GetBitRate() >= 64 )
	{	
		// xliang [11/14/2008] polycomMCU的双流通道已经打开，此处不必重复
		if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) &&
			( MT_TYPE_MMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ||
			MT_TYPE_SMCU == m_ptMtTable->GetMtType(tMt.GetMtId()) ) )
		{
			CallLog("PolyMcu.%d's secvideo chan has been open somewhere else, ignore it\n");
		}
		if (MT_TYPE_MMCU == tMt.GetMtType())
		{
			CallLog("Mt%d is mmcu, settimer open double video chan. \n", tMt.GetMtId());
			SetTimer(MCU_SMCUOPENDVIDEOCHNNL_TIMER, 3000, tMt.GetMtId());
		}
		else
		{
            BOOL32 bDStreamSrcLegal = FALSE;

            //zbq[01/11/2008] 双流放象需要伪装放象通道
            if ( TYPE_MCUPERI == m_tDoubleStreamSrc.GetType() &&
                 EQP_TYPE_RECORDER == m_tDoubleStreamSrc.GetEqpType() )
            {
                tLogicalChannel.SetChannelType( m_tPlayEqpAttrib.GetDVideoType() );
                tLogicalChannel.SetSupportH239( m_tConf.GetCapSupport().IsDStreamSupportH239() );
				// fxh 需要双流源的分辨率信息,已用于后续开通道时判断是否目的端与之匹配
				u16 wFileDSW = 0; 
				u16 wFileDSH = 0;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
				u8  byFileDSType = 0;
				m_tPlayFileMediaInfo.GetDVideo(byFileDSType, wFileDSW, wFileDSH);
		     	u8 byFileDSRes = GetResByWH(wFileDSW, wFileDSH);
				tLogicalChannel.SetVideoFormat(byFileDSRes);
                bDStreamSrcLegal = TRUE;
            }
            else
            {
                if ( m_ptMtTable->GetMtLogicChnnl( m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE ))
                {
                    bDStreamSrcLegal = TRUE;
                }
                else
                {
                    CallLog("[ProcMtCapSet] Get DS Src.%d's logic chnnl failed\n", m_tDoubleStreamSrc.GetMtId());
                }
            }

            if ( bDStreamSrcLegal )
            {
                McuMtOpenDoubleStreamChnnl(tMt, tLogicalChannel, tCapSupport);
            }
		}
	}
    else
    {
        CallLog( "Mt%d not support any conf DVideo media type, NO open DVideo logicChannel request send to it!\n", tMt.GetMtId() );
    }

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
		CallLog( "Open Data LogicChannel Request send to Mt%d, type: %s\n", 
			tMt.GetMtId(), GetMediaStr( tDataLogicChnnl.GetChannelType() ) );			
	}

	// 级联数据通道
    // guzh [1/19/2007] 增加判断是否级联会议数量超限,该会议下级MCU是否超限
	u8 byMtType = m_ptMtTable->GetMtType(tMt.GetMtId());
	if ( byMtType == MT_TYPE_SMCU &&
	 	 tCapSupport.IsSupportMMcu() &&
		 m_tConf.GetConfAttrb().IsSupportCascade() &&
         !g_cMcuVcApp.IsCasConfOverCap() &&
         m_tConfAllMtInfo.GetCascadeMcuNum() < MAXLIMIT_CONF_SMCU &&
		 m_tConfAllMtInfo.GetCascadeMcuNum() < MAXNUM_SUB_MCU )
	{
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
        
        // 上级MCU打开下级级联通道，对可能发生失败的情况，尝试重新打开[01/11/2007_zbq]
        m_abyCasChnCheckTimes[tMt.GetMtId()-1] = 1;
        SetTimer( MCUVC_WAIT_CASCADE_CHANNEL_TIMER, TIMESPACE_WAIT_CASCADE_CHECK );
		
		//打印消息
		CallLog( "Open Data LogicChannel Request send to Mt%d, type: %s\n", 
				tMt.GetMtId(), GetMediaStr( tDataLogicChnnl.GetChannelType() ) );			
	}
    else
    {
        CallLog( "Not send MMcu OLC to Mt%d for MtType.%d, RemoteCap.%d, ConfAtrb.%d, CasConfNum.%d, CasMcuNum.%d \n", 
                 tMt.GetMtId(),
                 byMtType,
                 tCapSupport.IsSupportMMcu(),
                 m_tConf.GetConfAttrb().IsSupportCascade(),
                 g_cMcuVcApp.GetMcuCasConfNum(),
                 m_tConfAllMtInfo.GetCascadeMcuNum() );
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
			CallLog( "Open Data LogicChannel Request send to Mt%d, type: %s as Master \n", 
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
					CallLog( "Open Data LogicChannel Request send to Mt%d, type: %s as master with no address ! \n", 
						      tMt.GetMtId(), GetMediaStr( tDataLogicChnnl.GetChannelType() ) );
				}
				//本端做从, 等待上级MCU打开本端通道
				else
				{
					//打印消息
					CallLog( "Open Data LogicChannel Request send to Mt%d, type: %s as slave with no address ! \n", 
						      tMt.GetMtId(), GetMediaStr( tDataLogicChnnl.GetChannelType() ) );
				}				
			}
			else
			{
				CallLog("Get DCS Address 0x%x:%d failed in Open T120 channel !\n", dwDcsIp, wDcsPort );
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
void CMcuVcInst::ProcTimerReopenCascadeChannel()
{
    BOOL32 bNonOpenStill = FALSE;
    BOOL32 bFailedThreeTimes = FALSE;
    TLogicalChannel tDataLogicChnnl;
    for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++ )
    {
        if ( m_abyCasChnCheckTimes[byMtId-1] == 0 )
        {
            continue;
        }
        //最多尝试六次 (3 * 10 + 3 * 60) s
        if ( m_abyCasChnCheckTimes[byMtId-1] <= 2*MAXNUM_CHECK_CAS_TIMES )
        {
            m_abyCasChnCheckTimes[byMtId-1] ++;

            //尝试再次打开级联数据通道
            u8 byMtType = m_ptMtTable->GetMtType(byMtId);
            // guzh [1/19/2007] 增加判断是否级联会议数量超限,该会议下级MCU是否超限
	        if ( byMtType == MT_TYPE_SMCU &&
                !g_cMcuVcApp.IsCasConfOverCap() &&
                m_tConfAllMtInfo.GetCascadeMcuNum() < MAXLIMIT_CONF_SMCU &&
		        m_tConfAllMtInfo.GetCascadeMcuNum() < MAXNUM_SUB_MCU )
            {
                tDataLogicChnnl.SetMediaType( MODE_DATA );
                tDataLogicChnnl.SetChannelType( MEDIA_TYPE_MMCU );
    
                //打开级联通道时，相应tcp建链的ip及port设置为接入板级联地址
                TMt tDstMt = m_ptMtTable->GetMt(byMtId);
                u32 dwCasIp = g_cMcuVcApp.GetMtAdpIpAddr(tDstMt.GetDriId()); //主机序
                tDataLogicChnnl.m_tRcvMediaChannel.SetIpAddr(dwCasIp);
    
                tDataLogicChnnl.m_tRcvMediaChannel.SetPort( g_cMcuVcApp.GetSMcuCasPort() );
                
                CServMsg cServMsg;
                cServMsg.SetMsgBody( ( u8 * )&tDataLogicChnnl, sizeof( tDataLogicChnnl ) );			
                SendMsgToMt( byMtId, MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );
                
                if ( m_abyCasChnCheckTimes[byMtId-1] > MAXNUM_CHECK_CAS_TIMES )
                {
                    bFailedThreeTimes = TRUE;
                }
                bNonOpenStill = TRUE;
                ConfLog( FALSE, "[ProcTimerReopenCascadeChannel] Open Mt.%d Cascade Channel failed %d time(s), retry now !\n", 
                                 byMtId, m_abyCasChnCheckTimes[byMtId-1] );
            }
        }
        else
        {
            m_abyCasChnCheckTimes[byMtId-1]  = 0;
            ConfLog( FALSE, "[ProcTimerReopenCascadeChannel] Open Mt.%d Cascade Channel failed entirely !\n", byMtId );
        }
    }

    if ( bNonOpenStill )
    {
        if ( bFailedThreeTimes )
        {
            SetTimer( MCUVC_WAIT_CASCADE_CHANNEL_TIMER, TIMESPACE_WAIT_CASCADE_CHECK * 6 );
        }
        else
        {
            SetTimer( MCUVC_WAIT_CASCADE_CHANNEL_TIMER, TIMESPACE_WAIT_CASCADE_CHECK );
        }
    }
    else
    {
        KillTimer( MCUVC_WAIT_CASCADE_CHANNEL_TIMER );
    }
	return;
}

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
    TCapSupport tCapSupport = *(TCapSupport *)( cServMsg.GetMsgBody() );
    TCapSupportEx tCapSupportEx = *(TCapSupportEx*)(cServMsg.GetMsgBody()+sizeof(TCapSupport));
    
	u32 dwBitRate = 0;

	switch( CurState() )
	{
	case STATE_ONGOING:
		{
			//设置该终端的能力集
			u8 byMtId = tMt.GetMtId();
			m_ptMtTable->SetMtCapSupport(byMtId, &tCapSupport);
			m_ptMtTable->SetMtCapSupportEx(byMtId, &tCapSupportEx);
            if (g_bPrintMtMsg)
            {
                MtLog("[ProcMtMcuCapSetNotif]capability specified by mt:\n");
                tCapSupport.Print();
            }
			if (m_ptMtTable->IsMtCapSpecByMCS(byMtId))
			{
				TCapSupport tCap;
				TDStreamCap tDStreamCap;
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

				m_ptMtTable->SetMtCapSupport(byMtId, &tCap);
				
				m_ptMtTable->GetMtCapSupport(byMtId, &tCapSupport);			
			}

            if (g_bPrintMtMsg)
            {
                MtLog("[ProcMtMcuCapSetNotif]final mt capability:\n");
                tCapSupport.Print();
            }
			
			//确定该终端已经入会
			if( m_tConfAllMtInfo.MtJoinedConf( byMtId ) )
			{
				//处理终端能力集
				ProcMtCapSet( tMt, tCapSupport, tCapSupportEx );

				//本端主呼
				if ( !m_ptMtTable->IsNotInvited( byMtId ) )
				{
					//需要拿到地址和端口
				}
				//本端被呼
				else
				{
					//终端和会议均支持数据功能
					if ( tCapSupport.IsSupportT120() && 
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
		ConfLog( FALSE, "CMcuVcInst: ProcMtMcuCapSetNotif() Wrong message %u(%s) received in state %u!\n", 
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
		ConfLog( FALSE, "CMcuVcInst: ProcMtMcuMtAliasNotif() Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );		
	}
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
====================================================================*/
void CMcuVcInst::ProcMtMcuConnectedNotif( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	u8 byManuId = *(u8*)cServMsg.GetMsgBody();
	TCapSupport tCapSupport;
	TCapSupportEx tCapSupportEx;
	TSimCapSet tSimCapSet;

	//设置厂商信息
	m_ptMtTable->SetManuId( tMt.GetMtId(), byManuId );
	CallLog( "Mt%d: 0x%x(Dri:%d) connected and it's manufacture is %s!\n", 
		     tMt.GetMtId(), m_ptMtTable->GetIPAddr(tMt.GetMtId()), tMt.GetDriId(), GetManufactureStr( byManuId ) );


    // xsl [10/27/2006] 分散会议不支持级联
    if (m_tConf.GetConfAttrb().IsSatDCastMode() &&
        MT_TYPE_MT != m_ptMtTable->GetMtType(tMt.GetMtId()))
    {
        SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);
        CallLog("[ProcMtMcuConnectedNotif] sat conf don't support cascade.\n");  
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
        // 主码率采用主格式，辅码率采用辅格式（避免纯粹的速率适配）
        // 这里暂时不考虑音频的双格式，因为双音频格式的会议应该是无法同双媒体同时开启的（BAS通道不够）
        u16 wMainBitrate = m_tConf.GetBitRate();
        u16 wSecBitrate = m_tConf.GetSecBitRate();
        u16 wDialBitrate = m_ptMtTable->GetDialBitrate(tMt.GetMtId());
        if ( wDialBitrate == wMainBitrate ) 
        {
            tSim.Clear();
            tCapSupport.SetSecondSimCapSet(tSim);
        }
        else if ( wSecBitrate == wDialBitrate )
        {
            tSim = tCapSupport.GetSecondSimCapSet();
            tCapSupport.SetMainSimCapSet(tSim);
            tSim.Clear();
            tCapSupport.SetSecondSimCapSet(tSim);
        }
        else
        {
            tSim.Clear();
            tCapSupport.SetSecondSimCapSet(tSim);
        }           
    }    
#endif    
    
    //zbq[02/04/2009] 高清会议面向Polycom进行480p的保护
    if (g_cMcuVcApp.IsSendFakeCap2Polycom() &&
        MT_MANU_POLYCOM == m_ptMtTable->GetManuId((tMt.GetMtId())) &&
        IsHDConf(m_tConf))
    {
        tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_4CIF);
    }

    if (MT_MANU_TAIDE == m_ptMtTable->GetManuId((tMt.GetMtId())) &&
        IsHDConf(m_tConf))
    {
        if (g_cMcuVcApp.IsSendFakeCap2Taide())
        {
            tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_CIF);
        }
        else if (g_cMcuVcApp.IsSendFakeCap2TaideHD())
        {
            tCapSupport.SetMainVideoResolution(VIDEO_FORMAT_HD720);
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

	// MCU前向纠错, zgc, 2007-09-27
	cServMsg.CatMsgBody( ( u8 * )&tCapSupportEx, sizeof( tCapSupportEx ) );
	
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
	
	CallLog( "Mt.%d Disconnected because of Reason \"%s\"(%d).\n", tMt.GetMtId(), 
             GetMtLeftReasonStr(byReason), byReason );
	
	switch( CurState() )
	{
		
	case STATE_ONGOING:		
        
		// 当前请求的终端断链，则可以kill定时器，允许接收下一个调度请求
		if(VCS_CONF == m_tConf.GetConfSource() &&
		   tMt == m_cVCSConfStatus.GetReqVCMT())
		{
			KillTimer(MCUVC_VCMTOVERTIMER_TIMER);

			TMt tNull;
			tNull.SetNull();
			m_cVCSConfStatus.SetReqVCMT(tNull);
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
				  (VCS_CONF == m_tConf.GetConfSource() && byReason != MTLEFT_REASON_REJECTED))
            {
            }
            else
            {
                //清除终端呼叫模式
                m_ptMtTable->SetCallMode( tMt.GetMtId(),  CONF_CALLMODE_NONE );
            
                //清除呼叫模式后立即发扩展结构，主要是适应Reject原因下的终端信息不能单发的情况。
                //实际上在Normal原因下，终端离会处理也会发该信息。但为保证会控对终端状态处理的一致性，这里不予理睬。
                CServMsg cServMsg;
                cServMsg.SetEventId( MCU_MCS_CALLMTMODE_NOTIF );
                cServMsg.SetMsgBody( (u8*)&m_ptMtTable->m_atMtExt[tMt.GetMtId()-1], sizeof(TMtExt) );
                SendMsgToAllMcs( cServMsg.GetEventId(), cServMsg );
            }
        }

		if( m_tConfAllMtInfo.MtJoinedConf( tMt.GetMtId() ) )
		{
			NotifyMcuDropMt(tMt);				
		}
		
		if( MTLEFT_REASON_NORMAL != byReason  )
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
								OspPrintf(TRUE, FALSE, "m_ptMtTable->GetMtAlias Fail, mtid = %d\n", tMt.GetMtId());
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
                     m_ptMtTable->IsNotInvited(tMt.GetMtId())
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
			SendMtListToMcs( LOCAL_MCUID );
		}

        // xsl [11/8/2006] 释放接入板和转发板资源
        ReleaseMtRes(tMt.GetMtId());
		break;

	default:
		ConfLog( FALSE, "[ProcMcuMtDisconnectedNotif] Wrong message %u(%s) received in state %u!\n", 
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

	// 当前请求的终端呼叫失败，则可以kill定时器，允许接收下一个调度请求
	if(VCS_CONF == m_tConf.GetConfSource() &&
	   tMt == m_cVCSConfStatus.GetReqVCMT())
	{
		KillTimer(MCUVC_VCMTOVERTIMER_TIMER);

		TMt tNull;
		tNull.SetNull();
		m_cVCSConfStatus.SetReqVCMT(tNull);
	}

	u8* pbyMsgBody = cServMsg.GetMsgBody();
	pbyMsgBody += sizeof(TMtAlias);
	u8 byReason = *(u8*)pbyMsgBody;
	pbyMsgBody += sizeof(u8);

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
		tMt == m_tConf.GetChairman() && 
		MTLEFT_REASON_BUSYEXT == byReason)
	{
		byAutoForceCall = TRUE;
	}
	else
	{
	    SendMsgToAllMcs( MCU_MCS_CALLMTFAILURE_NOTIF, cServMsg ); 
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
						OspPrintf(TRUE, FALSE, "m_ptMtTable->GetMtAlias Fail, mtid = %d\n", tMt.GetMtId());
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
		byConfLevel != m_tConf.GetConfLevel())
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
				wRcvPort = g_cMcuVcApp.AssignMtPort( tMt.GetConfIdx(), tMt.GetMtId() );
				m_ptMtTable->SetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort );
			}

			//级联数据通道
			if( m_ptMtTable->GetDialAlias( tMt.GetMtId(), &tMtAlias ) && 
				mtAliasTypeH320ID == tMtAlias.m_AliasType && 
				m_tConf.GetConfAttrb().IsSupportCascade() &&
				m_tConfAllMtInfo.GetCascadeMcuNum() < MAXNUM_SUB_MCU )
			{
				tDataLogicChnnl.SetMediaType( MODE_DATA );
				tDataLogicChnnl.SetChannelType( MEDIA_TYPE_MMCU );
				
				tDataLogicChnnl.m_tRcvMediaChannel.SetIpAddr(dwRcvIp);
				tDataLogicChnnl.m_tRcvMediaChannel.SetPort(wRcvPort+9);
				
				cServMsg.SetMsgBody( ( u8 * )&tDataLogicChnnl, sizeof( tDataLogicChnnl ) );			
				SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cServMsg );
				
				//打印消息
				CallLog( "Open Data LogicChannel Request send to Mt%d, type: %s\n", 
					      tMt.GetMtId(), GetMediaStr( tDataLogicChnnl.GetChannelType() ) );
			}

			SendMtListToMcs( LOCAL_MCUID );

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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
        /*
        // License 接入能力检查
        if(g_cMcuVcApp.IsMtNumOverLicense())
        {       
			//增加通知会控超过接入能力告警 zgc 06-12-21
			cServMsg.SetMsgBody( (u8*)&tMt, sizeof(tMt) );
			cServMsg.SetErrorCode(ERR_MCU_OVERMAXCONNMT);
			
			SendMsgToAllMcs( MCU_MCS_CALLMTFAILURE_NOTIF, cServMsg );
			
            cServMsg.SetMsgBody();
            SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);

            CallLog("[ProcMtMcuMtJoinNotif] Licensed Max connected mt num(%d) is overflow.\n", 
                     g_cMcuVcApp.GetPerfLimit().m_wMaxConnMtNum );
            return;
        }
        // xsl [10/20/2006] 若达到Mcu License失效日期， 不允许终端入会
        if (g_cMcuVcApp.IsMcuExpiredDate())
        {
            cServMsg.SetMsgBody( (u8*)&tMt, sizeof(tMt) );
            cServMsg.SetErrorCode(ERR_MCU_OVERMAXCONNMT);
            
            SendMsgToAllMcs( MCU_MCS_CALLMTFAILURE_NOTIF, cServMsg );
            
            cServMsg.SetMsgBody();
            SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);
            
            CallLog("[ProcMtMcuMtJoinNotif] MCU License EXPIRED.\n");  
            
            return;
        }
        
		if ( g_cMcuVcApp.GetPerfLimit().IsLimitMtNum() )
		{
			u8 byDriId = tMt.GetDriId();
            
            // guzh [1/16/2007] 判断终端数量是否超限
            u8 byMtNum = 0;
            u8 byMcuNum = 0;
            BOOL32 bOverCap = g_cMcuVcApp.GetMtNumOnDri( byDriId, TRUE, byMtNum, byMcuNum );
			if ( bOverCap )
			{
				cServMsg.SetMsgBody( (u8*)&tMt, sizeof(tMt) );
				cServMsg.SetErrorCode(ERR_MCU_OVERMAXCONNMT);
				cServMsg.SetConfId( m_tConf.GetConfId() );
				SendMsgToAllMcs( MCU_MCS_CALLMTFAILURE_NOTIF, cServMsg );

				cServMsg.SetMsgBody();
				SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);

				CallLog( "[ProcMtMcuMtJoinNotif] mtadp.%d ability(Max=%d) is full! \n", 
                        byDriId,
                        g_cMcuVcApp.GetMtAdpSupportMtNum(byDriId));
				return;
			}
		}        
        */

        // xsl [11/16/2006] 若会议支持gk计费，且认证失败，则不允许终端入会
        if (m_tConf.IsSupportGkCharge() && !m_tConf.m_tStatus.IsGkCharge())
        {
            SendMsgToMt(tMt.GetMtId(), MCU_MT_DELMT_CMD, cServMsg);
            CallLog("[ProcMtMcuMtJoinNotif] conf %s support gk charge, but gk charge failed.\n", m_tConf.GetConfName());  
            return;
        }
		
		m_ptMtTable->SetMtIsMaster(cServMsg.GetSrcMtId(), byMcuMaster == 1?FALSE:TRUE);
	
		if( m_ptMtTable->GetCallMode(cServMsg.GetSrcMtId()) == CONF_CALLMODE_TIMER )
		{
			m_ptMtTable->SetCallLeftTimes(cServMsg.GetSrcMtId(), m_tConf.m_tStatus.GetCallTimes());
		}

		//需要密码的会议，对方是受邀终端则直接进入 
		if( m_tConf.GetConfAttrb().GetOpenMode() == CONF_OPENMODE_NEEDPWD && 
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
		}

        //卫星会议暂不考虑级联

    #ifndef _SATELITE_
		//通知其他MCU new mt
        NotifyMcuNewMt(tMt);        
    #endif

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
				CallLog( "Mcu%d password wrong, drop it!\n", tMt.GetMtId() );
			}
			else
			{
				SendMsgToMt( tMt.GetMtId(), MCU_MT_ENTERPASSWORD_REQ, cServMsg );
				CallLog( "Mt%d password wrong, request once more!\n", tMt.GetMtId() );
			}
		}
		else
		{
			DealMtMcuEnterPwdRsp(tMt, pszPwd, nPwdLen);
		}
		break;
		
	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
	TCapSupport tCapSupport;
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
			CallLog( "Mcu%d password wrong, drop it!\n", tMt.GetMtId() );
		}
		else
		{
			SendMsgToMt( tMt.GetMtId(), MCU_MT_ENTERPASSWORD_REQ, cServMsg );
			CallLog( "Mt%d password wrong, request once more!\n", tMt.GetMtId() );
		}
	}
	else
	{
		//加入会议
		AddJoinedMt( tMt );

		//处理终端能力集
		m_ptMtTable->GetMtCapSupport( tMt.GetMtId(), &tCapSupport );
        m_ptMtTable->GetMtCapSupportEx( tMt.GetMtId(), &tCapSupportEx);

		ProcMtCapSet( tMt, tCapSupport, tCapSupportEx );

		//发送视频码流处理
		if( m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE ) )
		{			
			//动态分屏改变风格
			if( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE )
			{
				ChangeVmpStyle( tMt, TRUE );
			}

			//若是发言人加入
			if( m_tConf.HasSpeaker() && GetLocalSpeaker() == tMt )
			{
                // xsl [8/22/2006]卫星分散会议时需要判断回传通道数
                if (m_tConf.GetConfAttrb().IsSatDCastMode() && IsOverSatCastChnnlNum(tMt.GetMtId()))
                {
                    ConfLog(FALSE, "[DealMtMcuEnterPwdRsp] over max upload mt num. cancel speaker!\n");
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
                    if (GetMixMtNumInGrp() < GetMaxMixNum())
                    {
                        AddMixMember( &tMt, DEFAULT_MIXER_VOLUME, FALSE );
				        StartSwitchToPeriEqp(tMt, 0, m_tMixEqp.GetEqpId(), 
					                         (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(tMt.GetMtId())), 
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
			m_ptConfOtherMcTable->AddMcInfo( tMt.GetMtId() );	
			m_tConfAllMtInfo.AddMcuInfo( tMt.GetMtId(), tMt.GetConfIdx() );
			
			//通知会控
			cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
			SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
			
			//发给会控会议所有终端信息
			cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
			SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
			
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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
	TMt	tMt, tMtSrc;;
	TLogicalChannel	tLogicChnnl;
	u16 wBitRate, wMinBitRate;
	TSimCapSet tSrcSimCapSet;
	TSimCapSet tDstSimCapSet;

	TLogicalChannel tFirstLogicChnnl;
	TLogicalChannel tSecLogicChnnl;
	BOOL bStartDoubleStream = FALSE;
    BOOL32 bMtLowedAlready = FALSE;

	switch( CurState() )
	{
	case STATE_ONGOING:

		tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );

		tLogicChnnl = *( TLogicalChannel * )( cServMsg.GetMsgBody() );
		wBitRate = tLogicChnnl.GetFlowControl();

        //zbq[02/19/2008] 终端本来是否被降速，用于其作源情况下的发送带宽整理.
        if ( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) &&
             m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()))
        {
            bMtLowedAlready = TRUE;
			CallLog("bMtLowedAlready is %d\n",bMtLowedAlready);
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
			Mt2Log("[ProcMtMcuFlowControlCmd] mt.%u bLowed: %d (wRcvBandWidth:%d,wBitRate:%d, conf bitrate:%d)\n",
				tMt.GetMtId(), m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()),
				wRcvBandWidth, wBitRate,m_tConf.GetBitRate());
			/*
            if ( m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId()) )
            {
                if (wBitRate > wRcvBandWidth)
				{
					if (abs(wBitRate-wRcvBandWidth)*100 / wRcvBandWidth < 20)
					{
						m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), FALSE);
					}
				}
				else
				{
					m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), FALSE);
				}
            }
            else
            {
				if (wBitRate > wRcvBandWidth)
				{
					if ( abs(wBitRate-wRcvBandWidth)*100 / wRcvBandWidth > 20 )
					{
						m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), TRUE);
					}
				}
				else
				{
					m_ptMtTable->SetMtBRBeLowed(tMt.GetMtId(), FALSE);
				}
            }
*/
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
				CallLog( "[ProcMtMcuFlowControlCmd] PolycomMcu.%d flowctrl received\n", tMt.GetMtId() );
			}
			return;
		}
		
		if (wBitRate > m_tConf.GetBitRate())
		{
			wBitRate = m_tConf.GetBitRate();
			CallLog("wBitRate is larger than conf bitrate, modify it equal to conf's bitrate! \n");
		}

        // xsl [8/7/2006] 下级mcu发送的第一路flowcontrol不处理
		if( MODE_SECVIDEO != tLogicChnnl.GetMediaType() &&
			m_ptMtTable->GetManuId( tMt.GetMtId() ) == MT_MANU_KDCMCU )
		{
			if (m_tCascadeMMCU.IsNull() || m_tCascadeMMCU.GetMtId() != tMt.GetMtId())
            {
                Mt2Log("[ProcMtMcuFlowControlCmd] ignore smcu MtReqBitrate :%d from mcu<%d>\n",
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
                   wBitRate > GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())) &&
                   !bMtE1Lowed) )
            {
                Mt2Log("[ProcMtMcuFlowControlCmd] same bitrate.%d for mt.%d!\n", wBitRate, tMt.GetMtId());
                return;
            }
            if ( !bMtE1Lowed )
            {
                m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), wBitRate, LOGCHL_VIDEO );
                Mt2Log("[ProcMtMcuFlowControlCmd] rcv fst MtReqBitrate :%d from mt<%d>\n", wBitRate, tMt.GetMtId());

                //zbq[02/19/2008] 本来作选看或发言源的终端带宽升速,需整理其发送带宽
                if ( bMtLowedAlready )
                {
                    //双流
                    if ( tMt == m_tDoubleStreamSrc )
                    {
                        TMt tMtNull;
                        tMtNull.SetNull();
                        RestoreVidSrcBitrate(tMtNull, MODE_SECVIDEO);
                        Mt2Log("[ProcMtMcuFlowControlCmd] E1 Lowed Mt restore, adjust DS Src.\n");
                    }
                    //视频选看或发言
                    for( u8 byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx ++)
                    {
                        if ( !m_tConfAllMtInfo.MtJoinedConf(byMtIdx) )
                        {
                            continue;
                        }
                        TMt tMtSrc;
                        if ( m_ptMtTable->GetMtSrc(byMtIdx, &tMtSrc, MODE_VIDEO) &&
                             tMt == tMtSrc )
                        {
                            TMt tMtRcv = m_ptMtTable->GetMt(byMtIdx);
                            RestoreVidSrcBitrate(tMtRcv, MODE_VIDEO);
                            Mt2Log("[ProcMtMcuFlowControlCmd] E1 Lowed Mt restore, adjust FstVid Src.\n");
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
                    SetTimer( MCUVC_SENDFLOWCONTROL_TIMER, 1200 );
                    Mt2Log("[ProcMtMcuFlowControlCmd] CurMtReqBR.%d while RealReqBR.%d, adjust for E1MtLowed to Fst.%d, Sec.%d\n",
                            wCurMtReqBR, wRealMtReqBR, 
                            m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), LOGCHL_VIDEO),
                            m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), LOGCHL_SECVIDEO) );                    
                }
                else
                {
                    u16 wCurMtReqBR = m_ptMtTable->GetMtReqBitrate(tMt.GetMtId());
                    m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), wBitRate, LOGCHL_VIDEO );
                    wRealMtReqBR = wBitRate;
                    Mt2Log("[ProcMtMcuFlowControlCmd] CurMtReqBR.%d while RealReqBR.%d, adjust for E1MtLowed None DS\n", wCurMtReqBR, wBitRate);
                }
                m_ptMtTable->SetLowedRcvBandWidth(tMt.GetMtId(), wRealMtReqBR);
                Mt2Log("[ProcMtMcuFlowControlCmd] Lowed Mt.%d's LowedRcvBandWidth adjust to.%d\n", tMt.GetMtId(), wRealMtReqBR);
            }

            //zbq[02/18/2008] 不论是否为降速终端的flowctrl，尝试调整Vmp的广播码率
			// xliang [8/4/2009] 在下面同changeVmpBitRate合并
//             if ( m_tConf.m_tStatus.IsBrdstVMP() )
//             {
//                 AdjustVmpBrdBitRate();
//             }

            //zbq[12/11/2008] 从适配接收码流的终端，刷新适配编码
            if (m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId()))
            {
                RefreshBasParam(tMt.GetMtId(), TRUE);
            }
		}
		if( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
		{
            //过滤码率相同的消息
            if (wBitRate == m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), FALSE) ||
                (!m_tDoubleStreamSrc.IsNull() && wBitRate > GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId()), FALSE)))
            {
                Mt2Log("[ProcMtMcuFlowControlCmd] rcv snd MtReqBitrate :%d from mt<%d>, ignore it.\n",
                    wBitRate, tMt.GetMtId());
                return;
            }

			m_ptMtTable->SetMtReqBitrate( tMt.GetMtId(), wBitRate, LOGCHL_SECVIDEO );
            Mt2Log("[ProcMtMcuFlowControlCmd] rcv snd MtReqBitrate :%d from mt<%d>\n",
                    wBitRate, tMt.GetMtId());
            
            //zbq[12/11/2008] 从适配接收码流的终端，刷新适配编码
            if (m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId(), FALSE))
            {
                RefreshBasParam(tMt.GetMtId(), TRUE, TRUE);
            }
		}
		
		if( MODE_VIDEO == tLogicChnnl.GetMediaType() )
		{
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
				Mt2Log( "[ProcMtMcuFlowControlCmd] SelectSee SrcMtId.%d, DstMtId.%d, wBitRate.%d, ChnnlType.%d\n", 
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
        }        

		//若会议正在广播画面合成图像,降画面合成码率
		// xliang [8/4/2009] 调新接口
        if (m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE 
            && m_tConf.m_tStatus.IsBrdstVMP()
			&& MODE_VIDEO == tLogicChnnl.GetMediaType()
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
            
            if (wMinBitRate != wBasBitrate && 
                MODE_VIDEO == tLogicChnnl.GetMediaType())
            {
                ChangeVmpBitRate(wMinBitRate, byVmpChanNo);                
            }*/
			AdjustVmpBrdBitRate(&tMt);
            return;
        }

        if (m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_VIDEO, &tFirstLogicChnnl, TRUE) &&
            m_ptMtTable->GetMtLogicChnnl( tMt.GetMtId(), LOGCHL_SECVIDEO, &tSecLogicChnnl, TRUE))
        {
            bStartDoubleStream = TRUE;
        }

		Mt2Log( "[ProcMtMcuFlowControlCmd] IsUseAdapter.%d, IsStartDStream.%d, SrcMtId.%d, DstMtId.%d, wBitRate.%d, wMinBitRate.%d, ChnnlType.%d\n", 
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
                if (TRUE == bStartDoubleStream && !m_tVidBrdSrc.IsNull())
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
/*                     if( MODE_VIDEO == tLogicChnnl.GetMediaType() )
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
                    }*/
                    OspPrintf(TRUE, FALSE, "[ProcMtMcuFlowControlCmd] req bitrate < second bitrate, ignore it\n"); 
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
                        Mt2Log("[ProcMtMcuFlowControlCmd1] change vid bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                               wMinBitRate, m_wVidBasBitrate);
                        ChangeAdapt(ADAPT_TYPE_VID, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                    }
                    else
                    {
                        Mt2Log("[ProcMtMcuFlowControlCmd1] start vid bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                               wMinBitRate, m_wVidBasBitrate);
                        StartAdapt(ADAPT_TYPE_VID, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                    }                   

                    StartSwitchToSubMt(m_tVidBasEqp, m_byVidBasChnnl, tMt.GetMtId(), MODE_VIDEO);
                }
                else if (m_tConf.m_tStatus.IsVidAdapting())//若不需要调节适配码率，直接将适配码流交换到此mt(会议中有其他终端的接收码率更低的情况)
                {
                    Mt2Log("[ProcMtMcuFlowControlCmd1] switch vid bas to mt<%d>\n", tMt.GetMtId());
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
                        Mt2Log("[ProcMtMcuFlowControlCmd3]change br bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                               wMinBitRate, m_wBasBitrate);
                        ChangeAdapt(ADAPT_TYPE_BR, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                    }
                    else
                    {
                        Mt2Log("[ProcMtMcuFlowControlCmd3]start br bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                               wMinBitRate, m_wBasBitrate);
                        StartAdapt(ADAPT_TYPE_BR, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                    }                   
                    
                    StartSwitchToSubMt(m_tBrBasEqp, m_byBrBasChnnl, tMt.GetMtId(), MODE_VIDEO);
                }
                else if (m_tConf.m_tStatus.IsBrAdapting())//若不需要调节适配码率，直接将适配码流交换到此mt(会议中有其他终端的接收码率更低的情况)
                {
                    Mt2Log("[ProcMtMcuFlowControlCmd1] switch br bas to mt<%d>\n", tMt.GetMtId());
                    StartSwitchToSubMt(m_tBrBasEqp, m_byBrBasChnnl, tMt.GetMtId(), MODE_VIDEO);
                }
            }            
            //升速到原始速率，由适配变为不适配
            else if (0 != m_tConf.GetSecBitRate())
            {
                StartSwitchToSubMt(tMtSrc, 0, tMt.GetMtId(), MODE_VIDEO);
                NotifyMtSend(tMtSrc.GetMtId(), MODE_VIDEO, TRUE);

				//恢复适配码率
				if (IsNeedBrAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate) && m_tConf.m_tStatus.IsBrAdapting())
				{
					ChangeAdapt(ADAPT_TYPE_BR, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
				}
            }
            //双格式会议无需适配终端降其视频源码率，可能会将码流压到最低
            else if (MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() || IsNeedCifAdp())
            {
                AdjustMtVideoSrcBR(tMt.GetMtId(), wMinBitRate, tLogicChnnl.GetMediaType());
            }

            return;
        }				
		break;
		
	default:
		ConfLog( FALSE, "CMcuVcInst: ProcMtMcuFlowControlCmd() Wrong message %u(%s) received in state %u!\n", 
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
		ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] No polycom MCU.%d deal with, ignore it\n", byPolyMCUId );
		return;
	}

	TMt tPolyMCU = m_ptMtTable->GetMt(byPolyMCUId);
	
	//Polymcu只在刚入会时调一次双流通道
	if ( MODE_SECVIDEO == tLogicChnnl.GetMediaType() )
	{
		if ( tPolyMCU == m_tDoubleStreamSrc )
		{
			StopDoubleStream( TRUE, FALSE );
			CallLog( "[ProcPolycomMcuFlowctrlCmd] StopDS(MODE_SECVIDEO)\n" );
		}

		TLogicalChannel tFstLogicChnnl;
		TLogicalChannel tSecLogicChnnl;
		if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstLogicChnnl, TRUE) )
		{
			ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] get PolyMCU.%d's Fst Fwd chnnl failed\n", byPolyMCUId );
			return;
		}
		if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecLogicChnnl, TRUE) )
		{
			ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] get PolyMCU.%d's Fst Fwd chnnl failed\n", byPolyMCUId );
			return;
		}

		u16 wSecBR = tLogicChnnl.GetFlowControl();
		tSecLogicChnnl.SetFlowControl(wSecBR);
		u16 wFstBR = m_ptMtTable->GetDialBitrate(byPolyMCUId) - wSecBR;
		tFstLogicChnnl.SetFlowControl(wFstBR);

		m_ptMtTable->SetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstLogicChnnl, TRUE);
		m_ptMtTable->SetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecLogicChnnl, TRUE);

		CallLog( "[ProcPolycomMcuFlowctrlCmd] MODE_SECVIDEO has been ctrled, wBR.%d\n", tLogicChnnl.GetFlowControl() );
	}
	else if ( MODE_VIDEO == tLogicChnnl.GetMediaType() )
	{
		
		//粗略计算
		u16 wDailBR = m_ptMtTable->GetDialBitrate(byPolyMCUId);

		TLogicalChannel tFstLogicChnnl;
		TLogicalChannel tSecLogicChnnl;
		if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstLogicChnnl, TRUE) )
		{
			ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] get PolyMCU.%d's Fst Fwd chnnl failed(1)\n", byPolyMCUId );
			return;
		}
		if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecLogicChnnl, TRUE) )
		{
			ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] get PolyMCU.%d's Fst Fwd chnnl failed(1)\n", byPolyMCUId );
			return;
		}

		u16 wFstBR = tLogicChnnl.GetFlowControl();
		tFstLogicChnnl.SetFlowControl(wFstBR);
		
		u16 wSecBR = m_ptMtTable->GetDialBitrate(byPolyMCUId) - wFstBR;
		if ( wSecBR * 100 / wDailBR < 5 )
		{
			ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] wSecBR.%d, wDailBR.%d, sec BR adjust to 0\n", wSecBR, wDailBR );
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
				CallLog( "[ProcPolycomMcuFlowctrlCmd] wBR.%d, wDailBR.%d, StopDS\n", wBitRate, wDailBR );
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
					CallLog( "[ProcPolycomMcuFlowctrlCmd] wBR.%d, wDailBR.%d, StartDS\n", wBitRate, wDailBR );
				}
				else
				{
					ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] wBR.%d, wDailBR.%d, should but no StartDS due to get sec chnnl failed\n", wBitRate, wDailBR );
				}
			}
			else
			{
				CallLog( "[ProcPolycomMcuFlowctrlCmd] wBR.%d, wDailBR.%d, should but no StartDS due to dualmode.%d\n",
								 wBitRate, wDailBR, m_tConf.GetConfAttrb().GetDualMode());
			}
		}
	}
	else
	{
		ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] unexpected chnnl mode.%d\n", tLogicChnnl.GetMediaType() );
	}

	//本flowctrl调整终端的前向通道，适应于对polymcu的特殊处理，同时调整其后向通道
	TLogicalChannel tFstFwdChnnl;
	TLogicalChannel tFstRcvChnnl;
	TLogicalChannel tSecFwdChnnl;
	TLogicalChannel tSecRcvChnnl;

	if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstFwdChnnl, TRUE) )
	{
		ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] polyMcu.%d Fwd VIDEO chnnl failed\n", byPolyMCUId );
		return;
	}
	if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstRcvChnnl, FALSE) )
	{
		ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] polyMcu.%d Rcv VIDEO chnnl failed\n", byPolyMCUId );
		return;
	}
	if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecFwdChnnl, TRUE) )
	{
		ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] polyMcu.%d Fwd LOGCHL_SECVIDEO chnnl failed\n", byPolyMCUId );
		return;
	}
	if ( !m_ptMtTable->GetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecRcvChnnl, FALSE) )
	{
		ConfLog( FALSE, "[ProcPolycomMcuFlowctrlCmd] polyMcu.%d Rcv LOGCHL_SECVIDEO chnnl failed\n", byPolyMCUId );
		return;
	}

	u16 wFstRcvBR = tFstRcvChnnl.GetFlowControl();
	u16 wSecRcvBR = tSecRcvChnnl.GetFlowControl();

	tFstRcvChnnl.SetFlowControl(tFstFwdChnnl.GetFlowControl());
	tSecRcvChnnl.SetFlowControl(tSecFwdChnnl.GetFlowControl());

	m_ptMtTable->SetMtLogicChnnl(byPolyMCUId, LOGCHL_VIDEO, &tFstRcvChnnl, FALSE);
	m_ptMtTable->SetMtLogicChnnl(byPolyMCUId, LOGCHL_SECVIDEO, &tSecRcvChnnl, FALSE);

	CallLog( "[ProcPolycomMcuFlowctrlCmd] polyMcu.%d FstRcvBR adjust from.%d to %d, SecRcvBR adjust from.%d to %d\n",
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

		Mt2Log( "[Mt2McuFlowctrlInd] MtId.%d, ManuId.%d, DStreamMtId.%d, wBitRate.%d, ChnnlType.%d\n", 
				tMt.GetMtId(), m_ptMtTable->GetManuId( tMt.GetMtId() ), 
				m_tDoubleStreamSrc.GetMtId(), wBitRate, tLogicChnnl.GetMediaType() );
		
		// xliang [11/14/2008] 增加处理对polycom的指示响应
		if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId( tMt.GetMtId() ) )
		{
			CallLog( "[ProcMtMcuFlowControlIndication] PolycomMt.%d Ind ChnType.%d BR.%d\n",
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
                if (m_tConf.GetConfAttrb().IsSatDCastMode() && IsOverSatCastChnnlNum(tMt.GetMtId()))
                {
                    ConfLog(FALSE, "[Mt2McuFlowctrlInd] over max upload mt num.\n");
                    return;
                }
               
				//活动双流时，不检测令牌有效性
				UpdateH239TokenOwnerInfo( tMt );
				StartDoubleStream( tMt, tH239LogicChnnl );
			}
			else
			{
				Mt2Log( "[Mt2McuFlowctrlInd] MtId.%d, ManuId.%d, DStreamMtId.%d, wBitRate.%d, ChnnlType.%d open double stream with wrong DualMode, NACK !\n", 
					tMt.GetMtId(), m_ptMtTable->GetManuId( tMt.GetMtId() ), m_tDoubleStreamSrc.GetMtId(), wBitRate, tLogicChnnl.GetMediaType() );
			}
		}
		break;
		
	default:
		ConfLog( FALSE, "[Mt2McuFlowctrlInd] Wrong message %u(%s) received in state %u!\n", 
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
    函数名      GetLstRcvGrpMtBr
    功能        ：得到某接受群组中最小要求码率
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：码率
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/08/04    4.6         薛亮          创建
====================================================================*/
u16 CMcuVcInst::GetLstRcvGrpMtBr(BOOL32 bPrimary, u8 byMediaType, u8 byRes, u8 bySrcMtId /* = 0*/)
{
    u16 wMinBitRate = m_tConf.GetBitRate();
    u16 wMtBitRate;
    u8	byLoop;
    TSimCapSet tMtDstSCS;
    u8  byScale = 100;
    
    // xsl [8/7/2006] 若回传通道内有终端，则mcu接收码率不进行比较(因为mcu接收码率由回传终端发送码率决定)
    // 保证上级升降速时，不影响下级广播源码率
    BOOL32 bMMcuSpyMtOprSkip = FALSE;
    if( !m_tCascadeMMCU.IsNull() )
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
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



	u8  byNum = 0;
	u8	abyMtId[MAXNUM_CONF_MT] = { 0 };
	//m_cMtRcvGrp.GetMtMediaRes(tMt.GetMtId(), byMVType, byRes);
	m_cMtRcvGrp.GetMVMtList(byMediaType, byRes, byNum, abyMtId, TRUE);//强制取接收MT列表，忽略免适配区
	if(byNum > 0)
	{
		u8 byMtLoop = 0;
		for(byLoop = 0; byLoop < byNum; byLoop ++)
		{
			byMtLoop = abyMtId[byLoop];

			if (m_tConfAllMtInfo.MtJoinedConf(byMtLoop) && bySrcMtId != byMtLoop)
			{
				if (m_tCascadeMMCU.GetMtId() == byMtLoop && bMMcuSpyMtOprSkip )
					continue;
				
				tMtDstSCS = m_ptMtTable->GetDstSCS(byMtLoop);
				if (MEDIA_TYPE_NULL == byMediaType || tMtDstSCS.GetVideoMediaType() == byMediaType)
				{
					//zbq[07/28/2009] 双流flowctrl修正：不响应非免适配终端的flowctrl，AdpParam自然调整
					if (bPrimary &&
						m_cMtRcvGrp.IsMtNeedAdp(byMtLoop))
					{
						continue;
					}
					if (!bPrimary &&
						m_cMtRcvGrp.IsMtNeedAdp(byMtLoop, FALSE))
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
            // guzh [7/27/2007] 需要区分获取的主格式还是非主格式的最低码率（因为认为永远录主格式）
            if (IsRecordSrcBas(MODE_VIDEO, tRecSrc, byRecChnlIdx))
            {
                if ( ( tRecSrc == m_tBrBasEqp &&
                       ( MEDIA_TYPE_NULL == byMediaType || m_tConf.GetMainVideoMediaType() == byMediaType ) )
                   || ( tRecSrc == m_tVidBasEqp &&
                        MEDIA_TYPE_NULL != byMediaType &&
                        m_tConf.GetSecVideoMediaType() == byMediaType )
                   )
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
            wMinBitRate = kmax(wMinBitRate, m_tConf.GetSecBitRate()*byScale/100);
        }
    }

    Mt2Log("[GetLeastMtReqBitrate] Bitrate.%d, primary.%d, media.%d\n", 
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
    
    // xsl [8/7/2006] 若回传通道内有终端，则mcu接收码率不进行比较(因为mcu接收码率由回传终端发送码率决定)
    // 保证上级升降速时，不影响下级广播源码率
    BOOL32 bMMcuSpyMtOprSkip = FALSE;
    if( !m_tCascadeMMCU.IsNull() )
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
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
            if (MEDIA_TYPE_NULL == byMediaType || tMtDstSCS.GetVideoMediaType() == byMediaType)
            {
                //zbq[07/28/2009] 双流flowctrl修正：不响应非免适配终端的flowctrl，HD-AdpParam自然调整
                if (IsHDConf(m_tConf))
                {
                    if (bPrimary &&
                        m_cMtRcvGrp.IsMtNeedAdp(byLoop))
                    {
                        continue;
                    }
                    if (!bPrimary &&
                        m_cMtRcvGrp.IsMtNeedAdp(byLoop, FALSE))
                    {
                        continue;
                    }
                }

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
            // guzh [7/27/2007] 需要区分获取的主格式还是非主格式的最低码率（因为认为永远录主格式）
            if (IsRecordSrcBas(MODE_VIDEO, tRecSrc, byRecChnlIdx))
            {
                if ( ( tRecSrc == m_tBrBasEqp &&
                       ( MEDIA_TYPE_NULL == byMediaType || m_tConf.GetMainVideoMediaType() == byMediaType ) )
                   || ( tRecSrc == m_tVidBasEqp &&
                        MEDIA_TYPE_NULL != byMediaType &&
                        m_tConf.GetSecVideoMediaType() == byMediaType )
                   )
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
            wMinBitRate = kmax(wMinBitRate, m_tConf.GetSecBitRate()*byScale/100);
        }
    }

    Mt2Log("[GetLeastMtReqBitrate] Bitrate.%d, primary.%d, media.%d\n", 
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
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
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
            u8  byRecChnlIdx = 0;
            // guzh [7/27/2007] 需要区分获取的主格式还是非主格式的最低码率（因为认为永远录主格式）
            if (IsRecordSrcBas(MODE_VIDEO, tRecSrc, byRecChnlIdx))
            {
                if ( ( tRecSrc == m_tBrBasEqp &&
                       ( MEDIA_TYPE_NULL == byMediaType || m_tConf.GetMainVideoMediaType() == byMediaType ) )
                   || ( tRecSrc == m_tVidBasEqp &&
                        MEDIA_TYPE_NULL != byMediaType &&
                        m_tConf.GetSecVideoMediaType() == byMediaType )
                   )
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
            wMinBitRate = kmax(wMinBitRate, m_tConf.GetSecBitRate()*byScale/100);
        }
    }

    Mt2Log("[GetLeastMtReqBitrate] Bitrate.%d, primary.%d, media.%d\n", 
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

    Mt2Log("[GetLeastMtReqFramerate] FrameRate.%d, media.%d\n", byMinFrameRate, byMediaType);

    return byMinFrameRate;
}

/*====================================================================
    函数名      ：IsNeedAudAdapt
    功能        ：是否需要音频类型适配
    算法实现    ：还没考虑音频广播源为混音器和录像机的情况
    引用全局变量：
    输入参数说明：无
    返回值说明  ：TRUE/FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/09/02    4.0         libo          创建
====================================================================*/
BOOL32 CMcuVcInst::IsNeedAudAdapt(TSimCapSet &tDstSCS, TSimCapSet &tSrcSCS, TMt *ptSrcMt/* = NULL*/)
{
    TSimCapSet tBrdSrcSCS;
    u8 byMainType;
    u8 bySecondType;
    TMt tSrcMt;
    tSrcMt.SetNull();

    bySecondType = m_tConf.GetSecAudioMediaType();
    if (MEDIA_TYPE_NULL == bySecondType)
    {
        return FALSE;
    }

    if (!HasJoinedSpeaker())
    {
        return FALSE;
    }

    //得到广播源的SCS
    if (NULL != ptSrcMt)
    {
        tSrcMt = *ptSrcMt;
    }
    else if (HasJoinedSpeaker())
    {
        tSrcMt = GetLocalSpeaker();
    }

    if (tSrcMt.IsNull())
    {
        ConfLog(FALSE, "[IsNeedAudAdapt] src mt not in conf\n");
        return FALSE;
    }

    //考虑录像机
    if (TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_RECORDER == tSrcMt.GetMtType())
    {
        tBrdSrcSCS.SetAudioMediaType(m_tPlayEqpAttrib.GetAudioType());
    }
    else
    {
        tBrdSrcSCS = m_ptMtTable->GetSrcSCS(tSrcMt.GetMtId());
        if (tBrdSrcSCS.IsNull())
        {
            ConfLog(FALSE, "广播源未向MCU打开音频逻辑通道!\n");
            return FALSE;
        }
    }
    
    tSrcSCS.Clear();
    tSrcSCS.SetAudioMediaType(tBrdSrcSCS.GetAudioMediaType());

    tDstSCS.Clear();
    byMainType = m_tConf.GetMainAudioMediaType();
    if (tBrdSrcSCS.GetAudioMediaType() == byMainType)
    {
        tDstSCS.SetAudioMediaType(bySecondType);
    }
    else if (tBrdSrcSCS.GetAudioMediaType() == bySecondType)
    {
        tDstSCS.SetAudioMediaType(byMainType);
    }
    else
    {
        ConfLog(FALSE, "[IsNeedAudAdapt]广播源的同时能力集与会议不符!\n");
        return FALSE;
    }

    //是否有目的SCS与源不同
    TSimCapSet tMtDstSCS;
    for (u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        if (m_tConfAllMtInfo.MtJoinedConf(byLoop) && tSrcMt.GetMtId() != byLoop)
        {
            tMtDstSCS = m_ptMtTable->GetDstSCS(byLoop);
            if (tMtDstSCS.GetAudioMediaType() != tSrcSCS.GetAudioMediaType())
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}


/*====================================================================
    函数名      ：IsNeedVidAdapt
    功能        ：是否需要视频类型适配
    算法实现    ：还没有考虑视频源为录像机的情况
    引用全局变量：
    输入参数说明：无
    返回值说明  ：TRUE/FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/09/02    4.0         libo          创建
====================================================================*/
BOOL32 CMcuVcInst::IsNeedVidAdapt(TSimCapSet &tDstSCS, TSimCapSet &tSrcSCS, u16 &wAdaptBitRate, TMt *ptSrcMt/* = NULL*/)
{
	TSimCapSet tBrdSrcSCS;
    u8 byMainType;
    u8 bySecondType;
    TMt tSrcMt;
    tSrcMt.SetNull();

    bySecondType = m_tConf.GetSecVideoMediaType();
	if (MEDIA_TYPE_NULL == bySecondType)
	{
		return FALSE;
	}

    if (m_tConf.m_tStatus.IsBrdstVMP())
    {
        return FALSE;
    }

    if (!HasJoinedSpeaker() && CONF_POLLMODE_VIDEO != m_tConf.m_tStatus.GetPollMode())
    {
        return FALSE;
    }

	//得到广播源的SCS
    if (NULL != ptSrcMt)
    {
        tSrcMt = *ptSrcMt;
    }
	else if (HasJoinedSpeaker())
	{
        tSrcMt = GetLocalSpeaker();
	}
    else if (CONF_POLLMODE_VIDEO == m_tConf.m_tStatus.GetPollMode())
    {
        tSrcMt = m_tVidBrdSrc;
    }

    if (tSrcMt.IsNull())
    {
        ConfLog(FALSE, "[IsNeedVidAdapt] no src mt in conf\n");
        return FALSE;
    }

    //考虑录像机
    if (TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_RECORDER == tSrcMt.GetMtType())
    {
        tBrdSrcSCS.SetVideoMediaType(m_tPlayEqpAttrib.GetVideoType());        
    }
    else
    {
        tBrdSrcSCS = m_ptMtTable->GetSrcSCS(tSrcMt.GetMtId());	
        if (tBrdSrcSCS.IsNull())
        {
            ConfLog(FALSE, "广播源未向MCU打开音频逻辑通道!\n");
            return FALSE;
        }
    }
    
    tSrcSCS.Clear();
	tSrcSCS.SetVideoMediaType(tBrdSrcSCS.GetVideoMediaType());
    tSrcSCS.SetVideoResolution( tBrdSrcSCS.GetVideoResolution() );

    tDstSCS.Clear();
    byMainType = m_tConf.GetMainVideoMediaType();
    if (tBrdSrcSCS.GetVideoMediaType() == byMainType)
    {
        tDstSCS.SetVideoMediaType(bySecondType);
    }
    else if (tBrdSrcSCS.GetVideoMediaType() == bySecondType)
    {
        tDstSCS.SetVideoMediaType(byMainType);
    }
    else
    {
        ConfLog(FALSE, "[IsNeedVidBrAdapt]广播源的同时能力集与会议不符!\n");
        return FALSE;
    }

    u8 byConfMainResolution = m_tConf.GetMainVideoFormat();
    u8 byConfSecResolution = m_tConf.GetSecVideoFormat();
    if (TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_RECORDER == tSrcMt.GetMtType())
    {
        //分辨率先置为会议主分辨率防止启动适配，等录像机接口可以获取分辨率信息时再根据录像机设置分辨率
        tDstSCS.SetVideoResolution(byConfMainResolution);
		tSrcSCS.SetVideoResolution(byConfMainResolution);
    }
	else
	{
		TLogicalChannel tMtLogicChannel;
		memset(&tMtLogicChannel, 0, sizeof(tMtLogicChannel));
		m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_VIDEO, &tMtLogicChannel, FALSE);
		u8 bySrcMtResolution = tMtLogicChannel.GetVideoFormat();
		if( bySrcMtResolution == byConfMainResolution)
		{
			tDstSCS.SetVideoResolution( byConfSecResolution );
		}
		else if (bySrcMtResolution == byConfSecResolution)
		{
			tDstSCS.SetVideoResolution( byConfMainResolution );
		}
		else
		{
			tDstSCS.SetVideoResolution( byConfMainResolution );
		}
	}

    //是否有目的SCS与源不同
    BOOL32 bRet = FALSE;
    TSimCapSet tMtDstSCS;
    for (u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        if (m_tConfAllMtInfo.MtJoinedConf(byLoop) && tSrcMt.GetMtId() != byLoop)
        {
            tMtDstSCS = m_ptMtTable->GetDstSCS(byLoop);
            // guzh [6/1/2007] 如果没有该终端打开逻辑通道，无须考虑
            if (tMtDstSCS.GetVideoMediaType() != MEDIA_TYPE_NULL &&
                tMtDstSCS.GetVideoMediaType() != tSrcSCS.GetVideoMediaType())
            {
                bRet = TRUE;
                break;
            }
        }
    }

    // guzh [8/29/2007] FIXME：如果需要支持动态辅格式的低速码率组播（录像），
    // 如果会议还没有终端需要适配，上面会没有统计到，直接Return FALSE
    // 会导致不启适配，无法录像/低速组播等
    if ( !bRet)
    {
        return FALSE;
    }

    wAdaptBitRate = GetLeastMtReqBitrate(TRUE, tDstSCS.GetVideoMediaType(), tSrcMt.GetMtId());
    return bRet;
}

/*=============================================================================
  函 数 名： IsNeedCifAdp
  功    能： 是否需要分辨率适配
  算法实现： 
  全局变量： 
  参    数： TSimCapSet &tDstSCS
             TSimCapSet &tSrcSCS
  返 回 值： BOOL32 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  07/08/20    4.0         张宝卿        H264的D1(4CIF)分辨率不起适配 支持
  08/02/21    4.5         顾振华        取消上面的修改，H264适配不支持在其他地方限制
=============================================================================*/
BOOL32 CMcuVcInst::IsNeedCifAdp( void )
{
    BOOL32 bRet = FALSE;
    u8 byConfFstVidType = m_tConf.GetMainVideoMediaType();
    u8 byConfSecVidType = m_tConf.GetSecVideoMediaType();
    u8 byConfFstFormat  = m_tConf.GetMainVideoFormat();
    u8 byConfSecFormat  = m_tConf.GetSecVideoFormat();

    if(byConfFstVidType == byConfSecVidType &&
       byConfFstFormat  != byConfSecFormat )
    {
        bRet = TRUE;
    }
    return bRet;
}

//modify bas 2
/*====================================================================
    函数名      ：IsNeedBrAdapt
    功能        ：是否需要码率类型适配
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：TRUE/FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/09/02    4.0         libo          创建
====================================================================*/
BOOL32 CMcuVcInst::IsNeedBrAdapt(TSimCapSet &tDstSCS, TSimCapSet &tSrcSCS, u16 &wAdaptBitRate, TMt *ptSrcMt/* = NULL*/)
{
	TSimCapSet tBrdSrcSCS;
    TMt tSrcMt;
	u8 bySrcMtId;
    tSrcMt.SetNull();

	if (0 == m_tConf.GetSecBitRate())
	{
		return FALSE;
	}

    if (m_tConf.m_tStatus.IsBrdstVMP())
    {
        return FALSE;
    }

    if (!HasJoinedSpeaker() && CONF_POLLMODE_VIDEO != m_tConf.m_tStatus.GetPollMode())
    {
        return FALSE;
    }

	//得到广播源的SCS
    if (NULL != ptSrcMt)
    {
        tSrcMt = *ptSrcMt;
    }
	else if (HasJoinedSpeaker())
	{
        tSrcMt = GetLocalSpeaker();
	}
    else if (CONF_POLLMODE_VIDEO == m_tConf.m_tStatus.GetPollMode())
    {
        tSrcMt = m_tVidBrdSrc;
    }

    if (tSrcMt.IsNull())
    {
        ConfLog(FALSE, "[IsNeedBrAdapt] no src mt in conf\n");
        return FALSE;
    }

	bySrcMtId = tSrcMt.GetMtId();

    if (TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_RECORDER == tSrcMt.GetMtType())
    {
        tBrdSrcSCS.SetVideoMediaType(m_tPlayEqpAttrib.GetVideoType());
		bySrcMtId = 0;
    }
	else
	{
		tBrdSrcSCS = m_ptMtTable->GetSrcSCS(tSrcMt.GetMtId());
		if (tBrdSrcSCS.IsNull())
		{
			ConfLog(FALSE, "广播源未向MCU打开音视频逻辑通道 -- 码率适配!\n");
			return FALSE;
		}
	}
	
    tSrcSCS.Clear();
	tSrcSCS.SetVideoMediaType(tBrdSrcSCS.GetVideoMediaType());
    tSrcSCS.SetVideoResolution(tBrdSrcSCS.GetVideoResolution());

    //是否需要码率适配	
    u16 wMinBitrate = GetLeastMtReqBitrate(TRUE, tSrcSCS.GetVideoMediaType(), bySrcMtId);

    //zbq [08/26/2007] 是否需要码率适配要与当前的视频源的发送码率比较
	u16 wSrcSndBitrate = 0;
	if ( TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_RECORDER == tSrcMt.GetMtType() )
	{
		wSrcSndBitrate = m_tConf.GetBitRate();
	}
	else
	{
		wSrcSndBitrate = m_ptMtTable->GetMtSndBitrate(tSrcMt.GetMtId());
	}
    if (wMinBitrate >= wSrcSndBitrate /*m_tConf.GetBitRate()*/)
    {
        return FALSE;
    }

    wAdaptBitRate = wMinBitrate;
    tDstSCS.Clear();

    // zbq [06/07/2007] 取正确的目的SCS
    TSimCapSet tBrdDstSCS;
    tBrdDstSCS = m_ptMtTable->GetDstSCS(tSrcMt.GetMtId());
    tDstSCS.SetVideoMediaType(tBrdDstSCS.GetVideoMediaType());

	if (TYPE_MCUPERI == tSrcMt.GetType() && EQP_TYPE_RECORDER == tSrcMt.GetMtType())
	{
		tDstSCS.SetVideoResolution(m_tConf.GetMainVideoFormat());
	}
	else
	{
		TLogicalChannel tMtLogicChannel;
		memset(&tMtLogicChannel, 0, sizeof(tMtLogicChannel));
		m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_VIDEO, &tMtLogicChannel, FALSE);
		tDstSCS.SetVideoResolution( tMtLogicChannel.GetVideoFormat() );
	}

    return TRUE;
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
	u8 byAdpChnnlNum = m_tVmpChnnlInfo.m_byHDChnnlNum;
	TChnnlMemberInfo tChnnlMemInfo;
	u8 byMcuId;
	u8 byMtId;
	BOOL32 bSpeaker = FALSE;
	BOOL32 bSelected= FALSE;
	BOOL32 bNoneKeda= FALSE;
	u8 byLoop;
	OspPrintf(TRUE,FALSE,"\n---会议 %s中 VMP前适配通道成员信息---\n",m_tConf.GetConfName() );
	OspPrintf(TRUE,FALSE,"   当前共有%u个Mt占用前适配通道\n",byAdpChnnlNum);
	OspPrintf(TRUE,FALSE,"--------------------------------------\n");
	for(byLoop = 0; byLoop < MAXNUM_SVMPB_HDCHNNL; byLoop ++)
	{
		m_tVmpChnnlInfo.GetHdChnnlInfo(byLoop,&tChnnlMemInfo);
		byMcuId = tChnnlMemInfo.GetMt().GetMcuId();
		byMtId = tChnnlMemInfo.GetMtId();
		bSpeaker = tChnnlMemInfo.IsAttrSpeaker();
		bSelected = tChnnlMemInfo.IsAttrSelected();
		bNoneKeda = tChnnlMemInfo.IsAttrNoneKeda();
		OspPrintf(TRUE, FALSE, "[ID.%u]:(%u,%u)\tbSpeaker:%d\tbSelected:%d\tbNoneKeda:%d\n",
			byLoop+1, byMcuId, byMtId, bSpeaker, bSelected, bNoneKeda);
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
	TConfMtInfo tConfMtInfo;

	OspPrintf( TRUE, FALSE, "\n---会议(%d) %s 终端信息---\n", CurState(), m_tConf.GetConfName() );
	OspPrintf( TRUE, FALSE, "IsTemplate %d\n", m_tConf.m_tStatus.IsTemplate() );
	OspPrintf( TRUE, FALSE, "InstID %d\n", GetInsID() );
    OspPrintf( TRUE, FALSE, "ConfIdx %d\n", m_byConfIdx );
    m_tConf.GetConfId().GetConfIdString(achTemp, sizeof(achTemp));
    OspPrintf( TRUE, FALSE, "cConfId %s\n", achTemp );

	OspPrintf( TRUE, FALSE, "本地终端: McId-%d\n", LOCAL_MCUID );

	for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
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
                OspPrintf( TRUE, FALSE, "MtId.%d unexist, mishanding ? ...\n", byMtId );
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
                OspPrintf( TRUE, FALSE, "[ShowConfMt] GetMt.%d DailAlias failed\n", tMt.GetMtId() );
            }

            if ( !bRet )
            {
                OspPrintf( TRUE, FALSE, "%sMT%d:0x%x(Dri:%d Mp:%d), Type: %d Manu: %s, 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s\n",
                                        achTemp, byLoop, m_ptMtTable->GetIPAddr( byLoop ), tMt.GetDriId(), 
                                        m_ptMtTable->GetMpId( tMt.GetMtId() ),
                                        m_ptMtTable->GetMtType( tMt.GetMtId() ),
                                        GetManufactureStr( m_ptMtTable->GetManuId( byLoop ) ), 
                                        tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
                                        tMtAliasH320Alias.m_achAlias);
            }
            else
            {
                if ( mtAliasTypeTransportAddress == tDailAlias.m_AliasType )
                {
                    OspPrintf( TRUE, FALSE, "%sMT%d:0x%x(Dri:%d Mp:%d), Type: %d Manu: %s, 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s, DialAlias.%s@%d\n",
                                            achTemp, byLoop, m_ptMtTable->GetIPAddr( byLoop ), tMt.GetDriId(), 
                                            m_ptMtTable->GetMpId( tMt.GetMtId() ),
                                            m_ptMtTable->GetMtType( tMt.GetMtId() ),
                                            GetManufactureStr( m_ptMtTable->GetManuId( byLoop ) ), 
                                            tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
                                            tMtAliasH320Alias.m_achAlias,
                                            StrOfIP(tDailAlias.m_tTransportAddr.GetIpAddr()), tDailAlias.m_tTransportAddr.GetPort());
                }
                else
                {
                    OspPrintf( TRUE, FALSE, "%sMT%d:0x%x(Dri:%d Mp:%d), Type: %d Manu: %s, 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s, DialAlias.%s\n",
                                            achTemp, byLoop, m_ptMtTable->GetIPAddr( byLoop ), tMt.GetDriId(), 
                                            m_ptMtTable->GetMpId( tMt.GetMtId() ),
                                            m_ptMtTable->GetMtType( tMt.GetMtId() ),
                                            GetManufactureStr( m_ptMtTable->GetManuId( byLoop ) ), 
                                            tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
                                            tMtAliasH320Alias.m_achAlias,
                                            tDailAlias.m_achAlias);
                }
            }

			if( bInJoinedConf )
			{
				//正向逻辑通道信息
				memset( achTemp, ' ', 255 );
				int l = sprintf( achTemp, "   FL Aud:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_AUDIO, &tLogicalChannel, TRUE ) )
				{
					l += sprintf( achTemp+l, "%d(%s)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
						                                       GetMediaStr( tLogicalChannel.GetChannelType() ) );
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 26;
				l += sprintf( achTemp+l, "Vid:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_VIDEO, &tLogicalChannel, TRUE ) )
				{
                    // 增加分辨率打印, zgc, 2008-08-09
					l += sprintf( achTemp+l, "%d(%s)(%s)(%dK)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
						                                       GetMediaStr( tLogicalChannel.GetChannelType() ),
                                                               GetResStr( tLogicalChannel.GetVideoFormat() ),
															   tLogicalChannel.GetFlowControl() );
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 60;//50;
				l += sprintf( achTemp+l, "H239:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_SECVIDEO, &tLogicalChannel, TRUE ) )
				{
					l += sprintf( achTemp+l, "%d(%s)<%dK>", tLogicalChannel.GetRcvMediaChannel().GetPort(),
															GetResStr(tLogicalChannel.GetVideoFormat()),
															tLogicalChannel.GetFlowControl() );
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}
				
				memset( achTemp+l, ' ', 255-l );
				l = 86;//68;
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
				l = 97;//79;
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
                l = 109;//91;
                l += sprintf(achTemp+l, "VidBand:%dK", m_ptMtTable->GetRcvBandWidth(byLoop));                

				sprintf( achTemp+l, "%c", 0 );
				OspPrintf( TRUE, FALSE, "%s\n", achTemp );

				//反向逻辑通道信息
				memset( achTemp, ' ', 255 );
				l = sprintf( achTemp, "   RL Aud:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_AUDIO, &tLogicalChannel, FALSE ) )
				{
					l += sprintf( achTemp+l, "%d(%s)(%d)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
						                                    GetMediaStr( tLogicalChannel.GetChannelType() ),
															tLogicalChannel.GetFlowControl());
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 26;
				l += sprintf( achTemp+l, "Vid:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_VIDEO, &tLogicalChannel, FALSE ) )
				{
                    // 增加分辨率打印, zgc, 2008-08-09
					l += sprintf( achTemp+l, "%d(%s)(%s)(%dK)", tLogicalChannel.GetRcvMediaChannel().GetPort(),
						                                       GetMediaStr( tLogicalChannel.GetChannelType() ),
                                                               GetResStr( tLogicalChannel.GetVideoFormat() ),
                                                               tLogicalChannel.GetFlowControl());
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 60;//50
				l += sprintf( achTemp+l, "H239:");
				if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_SECVIDEO, &tLogicalChannel, FALSE ) )
				{
					l += sprintf( achTemp+l, "%d(%s)<%dK>", tLogicalChannel.GetRcvMediaChannel().GetPort(), 
															GetResStr(tLogicalChannel.GetVideoFormat()),
															tLogicalChannel.GetFlowControl() );
				}
				else
				{
					l += sprintf( achTemp+l, "NO" );
				}

				memset( achTemp+l, ' ', 255-l );
				l = 86;//68;
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
				l = 97;//79;
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
                l = 109;//91;
                l += sprintf(achTemp+l, "VidBand:%dK", m_ptMtTable->GetSndBandWidth(byLoop));     

				sprintf( achTemp+l, "%c", 0 );
				OspPrintf( TRUE, FALSE, "%s\n", achTemp );
			}
            
            if ( byMtId != 0 )
            {
                OspPrintf( TRUE, FALSE, "\nMT.%d's Cap is as follows:\n\n", byMtId );
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

	u8 byMcuId;
	for( u8 byLoop1 = 0; byLoop1 < MAXNUM_SUB_MCU; byLoop1++ )
	{
		byMcuId = m_tConfAllMtInfo.m_atOtherMcMtInfo[byLoop1].GetMcuId();
		if( byMcuId == 0  )
		{
			continue;
		}

        if ( byMtId != 0 &&
             byMcuId != byMtId )
        {
            continue;
        }
		
		OspPrintf( TRUE, FALSE, "其它终端: McuId-%d\n", byMcuId );
		TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(byMcuId);
		if(ptMcInfo == NULL)
		{
			continue;
		}
		TConfMtInfo *ptConfMtInfo = m_tConfAllMtInfo.GetMtInfoPtr(byMcuId); 
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
	
			
			OspPrintf( TRUE, FALSE, "%sMcu%dMT%d:0x%x(Dri:%d), Type: %d Manu: %s, Alias: %s, DialBitRate: %d. \n",
					achTemp, byMcuId, tMtExt.GetMtId(), tMtExt.GetIPAddr(), tMtExt.GetDriId(), 
					tMtExt.GetMtType(),
					GetManufactureStr( tMtExt.GetManuId() ), 
					tMtExt.GetAlias(), 
					tMtExt.GetDialBitRate() );
		}				
	}

    return;
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
			OspPrintf( TRUE, FALSE, "Illegal bas id.%d, mishanding ? ...\n", byBasId );
			return;
		}
		if (!g_cMcuVcApp.IsPeriEqpConnected(byBasId))
		{
			OspPrintf( TRUE, FALSE, "Bas.%d is not online yet, try another ...\n", byBasId );
			return;
		}
	}
    m_cBasMgr.Print();
    m_cMtRcvGrp.Print();
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
	TVmpModule tVmpModule = m_tConfEqpModule.m_tVmpModule;

    if( m_tConf.GetConfAttrb().IsHasVmpModule() )
	{
		OspPrintf( TRUE, FALSE, "Vmp module info:\n" );
		OspPrintf( TRUE, FALSE, "tVmpModule.m_tVMPParam.m_byVMPAuto = %d\n", tVmpModule.m_tVMPParam.IsVMPAuto()  );
		OspPrintf( TRUE, FALSE, "tVmpModule.m_tVMPParam.m_byVMPBrdst = %d\n", tVmpModule.m_tVMPParam.IsVMPBrdst()  );
		OspPrintf( TRUE, FALSE, "tVmpModule.m_tVMPParam.m_byVMPStyle = %d\n", tVmpModule.m_tVMPParam.GetVMPStyle() );
		for( u8 byLoop = 0; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++ )
		{
			if( !m_tConfEqpModule.m_atVmpMember[byLoop].IsNull() )
			{
				OspPrintf( TRUE, FALSE, "Vmp Chl%d(mt%d):", byLoop, m_tConfEqpModule.m_atVmpMember[byLoop].GetMtId() );
			}
		}
	}
    
    TMultiTvWallModule tMultiTvWallModule = m_tConfEqpModule.m_tMultiTvWallModule;
    TTvWallModule tOneModule;
    if ( m_tConf.GetConfAttrb().IsHasTvWallModule() )
    {
        OspPrintf( TRUE, FALSE, "Tvwall module info:\n" );
        OspPrintf( TRUE, FALSE, "tMultiTvWallModule.m_byTvModuleNum %d\n", tMultiTvWallModule.GetTvModuleNum());
        for ( u8 byLoop = 0; byLoop < tMultiTvWallModule.GetTvModuleNum(); byLoop++ )
        {
            tMultiTvWallModule.GetTvModuleByIdx(byLoop, tOneModule);
            OspPrintf( TRUE, FALSE, "Tvwall module %d:\n", byLoop );            
            OspPrintf( TRUE, FALSE, "Tvwall EqpId.%d\n", tOneModule.GetTvEqp().GetEqpId() );
            
            for ( u8 byLoop2 = 0; byLoop2 < MAXNUM_PERIEQP_CHNNL; byLoop2 ++ )
            {
                if ( tOneModule.m_abyTvWallMember[byLoop2] == 0 || 
                     tOneModule.m_abyMemberType[byLoop2] == 0 )
                     break;

                OspPrintf( TRUE, FALSE, "\tMember %d: Mt.%d, type: %d\n", 
                           byLoop2,
                           tOneModule.m_abyTvWallMember[byLoop2],
                           tOneModule.m_abyMemberType[byLoop2] );
            }
            
        }

    }
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
	ConfLog( FALSE, "\n---会议 %s 终端监控信息---\n", m_tConf.GetConfName() );
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

			ConfLog( FALSE, "MT%d-0x%x multicasting: %s \n",
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
				ConfLog( FALSE, "%s\n", achTemp );
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
void CMcuVcInst::InviteUnjoinedMt( CServMsg& cServMsgHdr, const TMt* ptMt, 
								   BOOL32 bSendAlert, BOOL32 bBrdSend, 
								   u8 byCallType/* = VCS_FORCECALL_REQ*/)
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
	u8  byEncrypt  =  (m_tConf.GetConfAttrb().GetEncryptMode() != CONF_ENCRYPTMODE_NONE)?1:0;

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
            
            ConfLog(FALSE, "[InviteUnjoinedMt] GetDialAlias failed, byMtId.%d\n", ptMt->GetMtId());
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
                				    
				    ConfLog( FALSE, "[InviteUnjoinedMt] Cannot Get IsMtAdpConnected|GetMtAdpProtocalType For Mt.%d.\n", ptMt->GetMtId() );
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
					
					McsLog( "[InviteUnjoinedMt] Cannot Get IP For Mt.%d.\n", ptMt->GetMtId() );
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
                    ConfLog(FALSE, "[InviteUnjoinedMt] DriId.%d for MT.%d is OFFLine.\n", byDriId, ptMt->GetMtId());
                    g_cMcuVcApp.DecMtAdpMtNum(byDriId, m_byConfIdx, ptMt->GetMtId());
                    
                    if ( 0 == byDriId )
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
		if(ptMt->GetMtId()  == m_byMtIdNotInvite)
		{
			m_ptMtTable->SetNotInvited( ptMt->GetMtId(), TRUE );
		}
		else
		{
			m_ptMtTable->SetNotInvited( ptMt->GetMtId(), FALSE );
		}
	
		cServMsg.SetMsgBody( (u8*)ptMt, sizeof( TMt ) );
		cServMsg.CatMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
		cServMsg.CatMsgBody( (u8*)&tConfAlias, sizeof( tConfAlias ) );	
		cServMsg.CatMsgBody( (u8*)&byEncrypt, sizeof(byEncrypt));
		u16 wCallBand = htons( m_ptMtTable->GetDialBitrate( ptMt->GetMtId() ) + wAudioBand );
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
		SendMsgToMt( ptMt->GetMtId(), MCU_MT_INVITEMT_REQ, cServMsg );

		if( TRUE == bSendAlert )
		{
			cServMsgHdr.SetMsgBody( (u8*)ptMt, sizeof( TMt ) );
			cServMsgHdr.CatMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
			if( bBrdSend )
			{
				BroadcastToAllMcu( MCU_MCU_CALLALERTING_NOTIF, cServMsgHdr );
			}
			else
			{
				SendReplyBack( cServMsgHdr, MCU_MCU_CALLALERTING_NOTIF );
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
						CallLog("Mt.%u is not invited!\n",m_byMtIdNotInvite);
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
						if( !m_ptMtTable->GetMtAlias( tInviteMt.GetMtId(), mtAliasTypeTransportAddress, &tMtAlias ) )
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

                            if (byDri == 0)
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
					CallLog("Mt.%u is not invited!\n",m_byMtIdNotInvite);
				}
				else
				{
					m_ptMtTable->SetNotInvited( tInviteMt.GetMtId(), FALSE );
				}
			
				cServMsg.SetMsgBody( (u8*)&tInviteMt, sizeof( tInviteMt ) );
				cServMsg.CatMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
				cServMsg.CatMsgBody( (u8*)&tConfAlias, sizeof( tConfAlias ) );
				cServMsg.CatMsgBody( (u8*)&byEncrypt, sizeof(byEncrypt));
				u16 wCallBand = htons( m_ptMtTable->GetDialBitrate( tInviteMt.GetMtId() ) + wAudioBand );
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

				SendMsgToMt( tInviteMt.GetMtId(), MCU_MT_INVITEMT_REQ, cServMsg );					
			}
		}
	}		
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
    TLogicalChannel	tLogicChnnl;
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
                if (tSrcMt.GetMcuId() == LOCAL_MCUID && tSrcMt.GetType() == TYPE_MT)
                {
                    NotifyMtSend(tSrcMt.GetMtId());
                }

                StartSwitchToSubMt(tSrcMt, 0, tDstMt.GetMtId(), MODE_BOTH, SWITCH_MODE_SELECT);

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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
void CMcuVcInst::ProcMcuMtFastUpdatePic( const CMessage * pcMsg )
{
    //CServMsg cServMsg( pcMsg->content, pcMsg->length );
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
            TMt tMt = m_ptMtTable->GetMt( tMtStatus.GetMtId() );
			tMtStatus.SetTMt( tMt );
			cServMsg.SetMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		}
		else
		{
			cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
			SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		}
		
		//查询终端状态
		SendMsgToMt( tMt.GetMtId(), MCU_MT_GETMTSTATUS_REQ, cServMsg );

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
	TMtStatus	tMtStatus;
	TMcu tMcu;
	u8 byLoop;

	switch( CurState() )
	{
	case STATE_ONGOING: 
		
		//是否在本级上增加
		tMcu = *(TMcu*)cServMsg.GetMsgBody(); 

		//所有终端状态
		if( tMcu.GetMcuId() == 0 )
		{
			//先发其它MC
			for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
			{
				u8 byMcuId = m_tConfAllMtInfo.m_atOtherMcMtInfo[byLoop].GetMcuId();
				if( byMcuId == 0 )
				{
					continue;
				}
				TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo( byMcuId );
				if(ptConfMcInfo == NULL)
				{
					continue;
				}
				tMcu.SetMcu( byMcuId );	
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
			tMcu.SetMcu( LOCAL_MCUID );
					
		}		

		//不是本级的MCU
		cServMsg.SetMsgBody( (u8*)&tMcu,sizeof(tMcu) );

		if( tMcu.IsLocal())
		{
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
			u8 byMcuId = tMcu.GetMcuId();
			if( !m_tConfAllMtInfo.m_tLocalMtInfo.MtJoinedConf( byMcuId ) )
			{
				cServMsg.SetErrorCode( ERR_MCU_THISMCUNOTJOIN );
				SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
				return;
			}

			TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo( byMcuId );
			cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(TMcu) );
			for(s32 nLoop =0; nLoop<MAXNUM_CONF_MT; nLoop++)
			{
				if(ptConfMcInfo->m_atMtStatus[nLoop].IsNull()|| 
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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
        ConfLog(FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
	TLogicalChannel tLogicalChannel;
	u16	wEvent;

	MMcuLog( "[ProcMcsMcuCamCtrlCmd] step 1 bySrcMtId.%d - MtMcuId.%d MtMtId.%d\n", 
			 bySrcMtId, tMt.GetMcuId(), tMt.GetMtId() );

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
                MMcuLog( "[ProcMcsMcuCamCtrlCmd] MT.%d no permission to operate, return\n", 
		                 bySrcMtId );					
                return;                
            }
			// 修改为遥控对方MCU
			tDstMt.SetMtId(tMt.GetMcuId());
			tDstMt.SetMcuId(LOCAL_MCUID);
		}
        // 如果是终端消息
		else if( bySrcMtId != 0 )
		{	          
			if( tMt.IsNull() )
			{
				m_ptMtTable->GetMtSrc( bySrcMtId, &tDstMt, MODE_VIDEO );
				if( !tDstMt.IsNull() )
				{
					tDstMt = m_ptMtTable->GetMt( tDstMt.GetMtId() );
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
						MMcuLog( "[ProcMcsMcuCamCtrlCmd] bySrcMtId.%d tDstMt.IsNull() return\n", bySrcMtId );

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
				        MMcuLog( "[ProcMcsMcuCamCtrlCmd] bySrcMtId.%d is not chairman or inspecter to Dst%d, return\n", 
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
				MMcuLog( "[ProcMcsMcuCamCtrlCmd] cannot operate mmcu, return\n" );
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
				MMcuLog( "[ProcMcsMcuCamCtrlCmd] bySrcMtId.%d tDstMt.IsNull() return\n", bySrcMtId );

				return;
			}
            // guzh [6/7/2007] 如果合并级联要操作上级，拒绝
            if ( !m_tCascadeMMCU.IsNull() &&
                tDstMt.GetMtId() == m_tCascadeMMCU.GetMtId() )
            {
                MMcuLog( "[ProcMcsMcuCamCtrlCmd] cannot operate mmcu, return\n" );
                return;
            }
		}

		//not joined conference
		if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
		{
			MMcuLog( "[ProcMcsMcuCamCtrlCmd] step 3 bySrcMtId.%d - event.%d DstMcuId.%d DstMtId.%d\n", 
				     bySrcMtId, pcMsg->event, tDstMt.GetMcuId(), tDstMt.GetMtId() );

			return;
		}

		MMcuLog( "[ProcMcsMcuCamCtrlCmd] step 4 bySrcMtId.%d - event.%d DstMcuId.%d DstMtId.%d\n", 
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
			cServMsg.SetDstMtId( tDstMt.GetMtId() );
			SendMsgToMt( tDstMt.GetMtId(), wEvent, cServMsg );
		}
		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
====================================================================*/
void CMcuVcInst::ProcMMcuMcuCamCtrlCmd( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt tDstMt = *(TMt*)cServMsg.GetMsgBody();
	u8 byOperType = *(u8*)( cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) );
	u16	wEvent;

	switch( CurState() )
	{
	case STATE_ONGOING:
		
		switch( byOperType )
		{
		//终端摄像头控制
		case 1:		//终端摄像头移动
			wEvent = MCU_MT_MTCAMERA_CTRL_CMD;
			break;
		case 2:		//终端摄像头停止移动
			wEvent = MCU_MT_MTCAMERA_CTRL_STOP;
			break;
		case 3:	    //终端摄像头遥控器使能
			wEvent = MCU_MT_MTCAMERA_RCENABLE_CMD;
			break;
		case 4:	    //终端摄像头调整到指定位置
			wEvent = MCU_MT_MTCAMERA_MOVETOPOS_CMD;
			break;
		case 5:	    //终端摄像头保存到指定位置
			wEvent = MCU_MT_MTCAMERA_SAVETOPOS_CMD;
			break;
		case 6:	    //选择终端视频源 
			wEvent = MCU_MT_SETMTVIDSRC_CMD;
			break;
		default:
			return;
		}

		//send messge
		cServMsg.SetDstMtId( tDstMt.GetMtId() );
		SendMsgToMt( tDstMt.GetMtId(), wEvent, cServMsg );
		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
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
	u16	wEvent;
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
			break;
			
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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
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
        ConfLog(FALSE, "CMcuVcInst: ProcMtMcuBandwidthNotif received in state %u, ignore it\n", CurState());
        return;
    }

    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    
    //u16 wMinBitRate;    
    //TMt tMtSrc;
    //TMt	tMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );

    u16 wRcvBandWidth = ntohs(*(u16 *)cServMsg.GetMsgBody());
    u16 wSndBandWidth = ntohs(*(u16 *)(cServMsg.GetMsgBody() + sizeof(u16)));
    Mt2Log("[ProcMtMcuBandwidthNotif] mt<%d> wRcvBandWidth :%d, wSndBandWidth :%d\n", 
            cServMsg.GetSrcMtId(), wRcvBandWidth, wSndBandWidth);

	//zbq[09/10/2008] 调整标识增加，避免时序问题
	m_ptMtTable->SetRcvBandAdjusted(cServMsg.GetSrcMtId(), TRUE);

    TLogicalChannel tLogicChan;
    tLogicChan.SetMediaType(MODE_VIDEO);
    tLogicChan.SetFlowControl(wRcvBandWidth);
    
    cServMsg.SetMsgBody((u8*)&tLogicChan, sizeof(TLogicalChannel));

    //标记或恢复标记该终端的升降速情况
    u8  byDstMtId = cServMsg.GetSrcMtId();
    u16 wDailBandWidth = m_ptMtTable->GetRcvBandWidth(byDstMtId);
    
    //通道可能还没来得及打开成功
    if ( 0 == wDailBandWidth )
    {
        wDailBandWidth = m_ptMtTable->GetDialBitrate(byDstMtId);
		m_ptMtTable->SetRcvBandWidth(byDstMtId, wDailBandWidth);
    }

    //临时粗略估计，应该够用. 相差小于5％认为恢复，否则认为降速. 只针对E1终端.
    //以太终端始终不认为降速, 其物理线路带宽大于8M，一般为10M.
    if ( wRcvBandWidth > 1024 * 8 )
    {
        m_ptMtTable->SetMtTransE1( byDstMtId, FALSE );
    }
    else
    {
        m_ptMtTable->SetMtTransE1( byDstMtId, TRUE );

		//zbq[9/27/2008] 呼叫码率小于E1当前额定带宽，保护其为正常带宽
        if (wDailBandWidth < wRcvBandWidth)
        {
            m_ptMtTable->SetMtBRBeLowed(byDstMtId, FALSE);
            CallLog("[ProcMtMcuBandwidthNotif] DstMt.%d NOT be set lowed due to<Dail.%d, Rcv.%d>(less)\n",
                      byDstMtId, wDailBandWidth, wRcvBandWidth );            
        }
        else
        {
			if (abs(wDailBandWidth-wRcvBandWidth)*100 / wDailBandWidth > 20)
			{
				m_ptMtTable->SetMtBRBeLowed(byDstMtId, TRUE);
				m_ptMtTable->SetLowedRcvBandWidth(byDstMtId, wRcvBandWidth);
				CallLog("[ProcMtMcuBandwidthNotif] DstMt.%d be set lowed due to<Dail.%d, Rcv.%d>\n",
					byDstMtId, wDailBandWidth, wRcvBandWidth );

			}
			else
			{
				m_ptMtTable->SetMtBRBeLowed(byDstMtId, FALSE);
				CallLog("[ProcMtMcuBandwidthNotif] DstMt.%d NOT be set lowed due to<Dail.%d, Rcv.%d>(greater less than 20)\n",
					byDstMtId, wDailBandWidth, wRcvBandWidth );            
			}
        }
    }

    //zbq[05/25/2009] 非E1终端不允许通过本消息刷带宽
    //因为其BR=10240，会保护到ConfBR，从而影响正常低速ETH终端的带宽控制
    if (!m_ptMtTable->GetMtTransE1(byDstMtId))
    {
        CallLog("[ProcMtMcuBandwidthNotif] DstMt.%d BandWidth ntf has been ignored due to ETH!\n", byDstMtId);
        return;
    }

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
        Mt2Log( "[ProcMtMcuBandwidthNotif] SelectSee SrcMtId.%d, DstMtId.%d, wBitRate.%d\n", 
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

	Mt2Log( "[ProcMtMcuBandwidthNotif] IsUseAdapter.%d, SrcMtId.%d, DstMtId.%d, wBitRate.%d, wMinBitRate.%d\n", 
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
                OspPrintf(TRUE, FALSE, "[ProcMtMcuBandwidthNotif] req bitrate < second bitrate, ignore it\n"); 
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
                    Mt2Log("[ProcMtMcuBandwidthNotif] change vid bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                           wMinBitRate, m_wVidBasBitrate);
                    ChangeAdapt(ADAPT_TYPE_VID, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }
                else
                {
                    Mt2Log("[ProcMtMcuBandwidthNotif] start vid bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                           wMinBitRate, m_wVidBasBitrate);
                    StartAdapt(ADAPT_TYPE_VID, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }                   

                StartSwitchToSubMt(m_tVidBasEqp, m_byVidBasChnnl, tMt.GetMtId(), MODE_VIDEO);
            }
            else if (m_tConf.m_tStatus.IsVidAdapting())//若不需要调节适配码率，直接将适配码流交换到此mt(会议中有其他终端的接收码率更低的情况)
            {
                Mt2Log("[ProcMtMcuBandwidthNotif] switch vid bas to mt<%d>\n", tMt.GetMtId());
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
                    Mt2Log("[ProcMtMcuBandwidthNotif]change br bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                           wMinBitRate, m_wBasBitrate);
                    ChangeAdapt(ADAPT_TYPE_BR, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }
                else
                {
                    Mt2Log("[ProcMtMcuBandwidthNotif]start br bas wMinBitRate = %d , m_wVidBasBitrate = %d\n",
                           wMinBitRate, m_wBasBitrate);
                    StartAdapt(ADAPT_TYPE_BR, wMinBitRate, &tDstSimCapSet, &tSrcSimCapSet);
                }                   
                
                StartSwitchToSubMt(m_tBrBasEqp, m_byBrBasChnnl, tMt.GetMtId(), MODE_VIDEO);
            }
            else if (m_tConf.m_tStatus.IsBrAdapting())//若不需要调节适配码率，直接将适配码流交换到此mt(会议中有其他终端的接收码率更低的情况)
            {
                Mt2Log("[ProcMtMcuBandwidthNotif] switch br bas to mt<%d>\n", tMt.GetMtId());
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
                Mt2Log("[ProcMtMcuOtherMsg] Mt<%d,%d> bitrate : %u, %u, %u, %u\n", 
                    tMt.GetMcuId(), tMt.GetMtId(), tMtBitrate.GetRecvBitRate(), tMtBitrate.GetSendBitRate(), 
                    tMtBitrate.GetH239RecvBitRate(), tMtBitrate.GetH239SendBitRate());
            }            
            break;

        case MT_MCU_GETMTVERID_ACK:
            {
                u8 byHWVerId = *(u8*)cServMsg.GetMsgBody();
                LPCSTR lpszSWVerId = (LPCSTR)(cServMsg.GetMsgBody() + sizeof(u8));
                m_ptMtTable->SetHWVerID(tMt.GetMtId(), byHWVerId);
                m_ptMtTable->SetSWVerID(tMt.GetMtId(), lpszSWVerId);
            }
            break;
            
        case MT_MCU_GETMTVERID_NACK:
            {
                //FIXME: 异步独立获取，考虑错误码上报
                CallLog( "[ProcMtMcuOtherMsg] Get mt.%d VerId failed due to reason.%d\n", tMt.GetMtId(), cServMsg.GetErrorCode() );
            }
            break;
            
		default:
			return;
		}

		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
			ConfLog( FALSE, "MCS set mt%d bitrate to %d !\n", tMt.GetMtId(), ntohs( wBitRate ) );
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
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
    CServMsg    cMsg( pcMsg->content, pcMsg->length );
	u8 bySrcMtId = cServMsg.GetSrcMtId();
	TLogicalChannel tLogicalChannel;
	u8  byMuteType;
	u8  byMuteOpenFlag;
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
		if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
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

			//可同时处理多个mt静/哑音请求
			byMtNum = cServMsg.GetMsgBodyLen() / (sizeof(TMt)+2);
			for( byIndex=0; byIndex<byMtNum; byIndex++ )
			{
				ptDstMt = (TMt*)( cServMsg.GetMsgBody() + byIndex*(sizeof(TMt)+2) );
				byMuteOpenFlag = *( (u8*)ptDstMt + sizeof(TMt) );
				byMuteType     = *( (u8*)ptDstMt + sizeof(TMt) + 1 );
				
				u8 byVendor = m_ptMtTable->GetManuId(ptDstMt->GetMtId());
				if(byVendor == MT_MANU_KDC)
				{
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
                    AdjustKedaMcuAndTaideMuteInfo( ptDstMt, byMuteType, byMuteOpenFlag );
				}
                
			}
			return;
				
			break;	

		default:
			return;
		}

		//send messge
		cServMsg.SetDstMtId( tDstMt.GetMtId() );
		if( pcMsg->event != MCS_MCU_MTAUDMUTE_REQ )
		{
			CServMsg cMsg = cServMsg;
			cServMsg.SetMsgBody( cMsg.GetMsgBody() + sizeof(TMt), cMsg.GetMsgBodyLen() - sizeof(TMt) );
		}

		SendMsgToMt( tDstMt.GetMtId(), wEvent, cServMsg );
		
		break;

	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
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
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt;
	if(cServMsg.GetEventId() == MCU_MCU_REINVITEMT_REQ)
	{
		tMt = *( TMt * )(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq));
	}
	else
	{
		tMt = *( TMt * )cServMsg.GetMsgBody();
	}

	STATECHECK

	//不在受邀终端列表中, NACK!
    TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(tMt.GetMcuId());
	//会议表中没有
	if(tMtInfo.IsNull())
	{
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	//已经加入会议,NACK
	if( tMtInfo.MtJoinedConf( tMt.GetMtId() ) )
	{	
		cServMsg.SetErrorCode( ERR_MCU_CALLEDMT_JOINEDCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	//不在本MCU上
	if( !tMt.IsLocal())
	{
		TMcuMcuReq tReq;
		TMcsRegInfo	tMcsReg;
		g_cMcuVcApp.GetMcsRegInfo( cServMsg.GetSrcSsnId(), &tMcsReg );
		astrncpy(tReq.m_szUserName, tMcsReg.m_achUser, 
			sizeof(tReq.m_szUserName), sizeof(tMcsReg.m_achUser));
		astrncpy(tReq.m_szUserPwd, tMcsReg.m_achPwd, 
			sizeof(tReq.m_szUserPwd), sizeof(tMcsReg.m_achPwd));
		
		cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
		cServMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt));
		SendMsgToMt( tMt.GetMcuId(), MCU_MCU_REINVITEMT_REQ, cServMsg );
		
		cServMsg.SetMsgBody( (u8 *)&tMt, sizeof(TMt));
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		return;
	}	

	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
 
	//删除该终端
	if(cServMsg.GetEventId() == MCU_MCU_REINVITEMT_REQ &&
	   // vcs不进行资源删除操作，只进行呼叫、挂断操作
	   // 删除将导致下级PU的IP＋别名信息被删除，而在后续的callalert通告中无法带入特殊的PU别名信息
	   m_tConf.GetConfSource() != VCS_CONF)
	{
		cServMsg.SetMsgBody((u8 *)&tMt, sizeof(tMt));
		SendReplyBack( cServMsg, MCU_MCU_DELMT_NOTIF );
	}

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
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt;
	if(cServMsg.GetEventId() == MCU_MCU_DROPMT_REQ)
	{
		tMt = *( TMt * )(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq));
	}
	else
	{
		tMt = *( TMt * )cServMsg.GetMsgBody();
	}

	STATECHECK

	//不在受邀终端列表中, NACK!
	TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(tMt.GetMcuId());
	//会议表中没有
	if(tMtInfo.IsNull())
	{
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	//已经不在会议中,NACK
	if( !tMtInfo.MtJoinedConf( tMt.GetMtId() ) )
	{	
		cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
	
	//该终端是非直连终端
	if( !tMt.IsLocal())
	{
		TMcuMcuReq tReq;
		TMcsRegInfo	tMcsReg;
		g_cMcuVcApp.GetMcsRegInfo( cServMsg.GetSrcSsnId(), &tMcsReg );
		astrncpy( tReq.m_szUserName, tMcsReg.m_achUser, 
				  sizeof(tReq.m_szUserName), sizeof(tMcsReg.m_achUser) );
		astrncpy( tReq.m_szUserPwd, tMcsReg.m_achPwd, 
				  sizeof(tReq.m_szUserPwd), sizeof(tMcsReg.m_achPwd) );
		
		cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
		cServMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt));
		
		SendMsgToMt( tMt.GetMcuId(), MCU_MCU_DROPMT_REQ, cServMsg );
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

		return;
	}
	
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
			
	//设置终端呼叫模式
	// 对于VCS会议呼叫模式有着特殊的含义(区分固定资源和可调度资源)不可更改
	if (m_tConf.GetConfSource() != VCS_CONF)
	{
		m_ptMtTable->SetCallMode( tMt.GetMtId(),  CONF_CALLMODE_NONE);
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
	
		break;
		
	case STATE_SCHEDULED:
		break;
	default:
		ConfLog( FALSE, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
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
	STATECHECK;

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );

	u8 bySrcMtId = cServMsg.GetSrcMtId();
	u8 byMode    = *(cServMsg.GetMsgBody());

	u32 dwTimeIntervalTicks = 3*OspClkRateGet();

    // 请求关键帧的终端, zgc, 2008-08-15
    TMt tMt = m_ptMtTable->GetMt( bySrcMtId );
	Mt2Log("[ProcMtMcuFastUpdatePic] Accept keyframe(mode:%d) require from mt(%d)\n", byMode, bySrcMtId);
	if(byMode == MODE_SECVIDEO)
	{
		if( m_tDoubleStreamSrc.IsNull() )
		{
			return;
		}

        if ( IsHDConf( m_tConf ) )
        {
            TSimCapSet tSrcSCS;
            if (m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId(), FALSE))
            {
                u8   byMediaType = MEDIA_TYPE_NULL;
                TEqp tHdBas;
                u8   byChnId;
                u8   byRecvIdx;
                u8   byRes = 0;
                if (m_cMtRcvGrp.GetMtMediaRes(tMt.GetMtId(), byMediaType, byRes, TRUE) &&
                    m_cBasMgr.GetBasResource(byMediaType,
                                             byRes,
                                             tHdBas,
                                             byChnId,
                                             byRecvIdx,
                                             TRUE, 
                                             IsDSSrcH263p() /*|| IsConfDualEqMV(m_tConf)*/))
                {
                    NotifyFastUpdate(tHdBas, byChnId);
                }
                else
                {
                    //zbq[01/17/2009] h263p反向适配资源不足时 紧凑适配
                    TDStreamCap tDSCap;
                    memset(&tDSCap, 0, sizeof(tDSCap));
                    GetDSBrdSrcSim(tDSCap);
                    if (MEDIA_TYPE_H263PLUS == tDSCap.GetMediaType())
                    {
                        //紧凑适配固定输出
                        u8 byProximalType = MEDIA_TYPE_H264;
                        u8 byProximalRes = VIDEO_FORMAT_XGA;
                        
                        //取紧凑适配的bas资源
                        BOOL32 bRet = m_cBasMgr.GetBasResource(byProximalType,
                                                               byProximalRes,
                                                               tHdBas,
                                                               byChnId,
                                                               byRecvIdx, TRUE, TRUE);
                        tHdBas.SetConfIdx(m_byConfIdx);
                        if (!bRet)
                        {
                            ConfLog(FALSE, "[ProcMtMcuFastUpdatePic] get proximal bas src failed(dual)!\n");
                            return;
                        }
                        //向紧凑BAS请求 关键帧
                        NotifyFastUpdate(tHdBas, byChnId);
                    }
                    else
                    {
                        ConfLog(FALSE, "[ProcMtMcuFastUpdatePic] get bas src for mt.%d<%d, %s> failed, h263p.%d!\n",
                            tMt.GetMtId(), byMediaType, GetResStr(byRes), IsDSSrcH263p());
                    }
                }
            }
            else
            {
                NotifyFastUpdate(m_tDoubleStreamSrc, MODE_SECVIDEO);
            }
        }
        else
        {
            //zbq[05/20/2009] 标清H263p适配关键帧请求 支持
            if (m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId(), FALSE))
            {
                u8   byMediaType = MEDIA_TYPE_NULL;
                TEqp tHdBas;
                u8   byChnId;
                u8   byRecvIdx;
                u8   byRes = 0;

                if (m_cMtRcvGrp.GetMtMediaRes(tMt.GetMtId(), byMediaType, byRes, TRUE) &&
                    m_cBasMgr.GetBasResource(byMediaType,
                                             byRes,
                                             tHdBas,
                                             byChnId,
                                             byRecvIdx,
                                             TRUE, 
                                             IsDSSrcH263p() /*|| IsConfDualEqMV(m_tConf))*/))
                {
                    NotifyFastUpdate(tHdBas, byChnId);
                }
                else
                {
                    ConfLog(FALSE, "[ProcMtMcuFastUpdatePic] get bas dual for mt.%d<%d, %s> failed, h263p.%d!\n",
                        tMt.GetMtId(), byMediaType, GetResStr(byRes), IsDSSrcH263p());
                }
            }
            else
            {
                NotifyFastUpdate(m_tDoubleStreamSrc, MODE_SECVIDEO);
            }
        }
	}
	else
	{
        //  xsl [5/22/2006] 是否源为bas
		TMt  tSrcMt;
		tSrcMt.SetNull();
        BOOL32 bRet = m_ptMtTable->GetMtSrc(bySrcMtId, &tSrcMt, MODE_VIDEO);
        u8 byAdpType;
		// 看广播源
		if (tSrcMt == m_tVidBrdSrc)
		{
			// 请求终端是否在高清适配区中
			if (m_cMtRcvGrp.IsMtNeedAdp(tMt.GetMtId()))
			{
	            TEqp tHdBas;
                u8 byChnId = 0;
                u8 byOutIdx = 0;
                u8 byRes = 0;
                u8 byMediaType = MEDIA_TYPE_NULL;

                if (m_cMtRcvGrp.GetMtMediaRes(tMt.GetMtId(), byMediaType, byRes) &&
                    m_cBasMgr.GetBasResource(byMediaType, byRes, tHdBas, byChnId, byOutIdx))
                {
					Mt2Log("mt(mtid:%d) its srcmt(mcuid:%d, mtid:%d) require framekey to hdbas(id:%d, inputchnl:%d, outputchnl:%d)\n",
						   tMt.GetMtId(), tSrcMt.GetMcuId(), tSrcMt.GetMtId(), tHdBas.GetEqpId(), byChnId, byOutIdx);
                    NotifyFastUpdate(tHdBas, byChnId);
                    return;
                }
                else
                {
                    //zbq[01/05/2009] 临时处理：反向适配请求 对齐保护
                    TSimCapSet tSrcSim;
                    memset(&tSrcSim, 0, sizeof(tSrcSim));
                    GetMVBrdSrcSim(tSrcSim);
                    if (MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType() &&
                        MEDIA_TYPE_H264 != tSrcSim.GetVideoMediaType() 
                        //取消单速限制
                        /*&&
                        0 == m_tConf.GetSecBitRate()*/)
                    {
                        //FIXME: 临时取main，整体刷到RcvChn
                        TCapSupport tCap;
                        m_ptMtTable->GetMtCapSupport(bySrcMtId, &tCap);

                        u8 byAdjType = m_tConf.GetMainVideoMediaType();
                        u8 byAdjRes = m_tConf.GetMainVideoFormat();

                        if (byAdjType == tCap.GetMainVideoType() &&
                            byAdjRes == tCap.GetMainVideoResolution())
                        {
                            //本终端是否需要走紧凑适配
                            u8 byProximalType = 0;
                            u8 byProximalRes = 0;
                            BOOL32 bRet = GetProximalGrp(byAdjType, byAdjRes, 
                                                         byProximalType, byProximalRes);
                            if (!bRet)
                            {
                                ConfLog(FALSE, "[ProcMtMcuFastUpdatePic] no proximal grp for <%d, %d\n", byAdjType, byAdjRes);
                                return;
                            }
                            //取紧凑适配的bas资源
                            BOOL32 bRet1 = m_cBasMgr.GetBasResource(byProximalType, 
                                                                    byProximalRes,
                                                                    tHdBas, byChnId, byOutIdx);
                            tHdBas.SetConfIdx(m_byConfIdx);
                            if (!bRet1)
                            {
                                ConfLog(FALSE, "[ProcMtMcuFastUpdatePic] get proximal bas src failed!\n");
                                return;
                            }
                            //向紧凑的bas资源请求关键帧
                            NotifyFastUpdate(tHdBas, byChnId);
                            return;
                        }
                    }
                }
			}
			// 请求终端是否在标清适配中
			else if (IsMtSrcBas(bySrcMtId, MODE_VIDEO, byAdpType))
			{
			    // 顾振华 [6/2/2006] 要获取相应的外设的TEqp结构
				// tSrcMt 是最终的源，而不是适配的外设
				TEqp tBasEqp;
				u8 byChnl;
				if (ADAPT_TYPE_VID == byAdpType)
				{
					tBasEqp = m_tVidBasEqp;
					byChnl = m_byVidBasChnnl;
				}
				else if (ADAPT_TYPE_BR == byAdpType)
				{
					tBasEqp = m_tBrBasEqp;
					byChnl = m_byBrBasChnnl;
				}            
				else if (ADAPT_TYPE_CASDVID == byAdpType)
				{
					tBasEqp = m_tCasdVidBasEqp;
					byChnl = m_byCasdVidBasChnnl;
				}
				Mt2Log("mt(mtid:%d its srcmt(mcuid:%d, mtid:%d) require framekey to sdbas(id:%d, chnl:%d)\n",
					   tMt.GetMtId(), tSrcMt.GetMcuId(), tSrcMt.GetMtId(), tBasEqp.GetEqpId(), byChnl);
				NotifyFastUpdate(tBasEqp, byChnl);
				return;
			}
            
		}
		// 看选看源
		else
		{
			u8 bySelSrcId = (tSrcMt.IsLocal() ? tSrcMt.GetMtId() : tSrcMt.GetMcuId());
			u8 bySelDstId = tMt.GetMtId();
			TEqp tBasEqp;
			u8 byChnl;
			if (m_cSelChnGrp.IsMtInSelAdpGroup(bySelSrcId, bySelDstId, &tBasEqp, &byChnl))
			{
				Mt2Log("mt(mtid:%d its srcmt(mcuid:%d, mtid:%d) require framekey to bas(id:%d, chnl:%d) for seladp\n",
					   tMt.GetMtId(), tSrcMt.GetMcuId(), tSrcMt.GetMtId(), tBasEqp.GetEqpId(), byChnl);
				NotifyFastUpdate(tBasEqp, byChnl);
				return;
			}
		}

        
        if (!bRet ||
            TYPE_MT != tSrcMt.GetType() ||
            !m_tConfAllMtInfo.MtJoinedConf(tSrcMt.GetMtId()))
        {
            if (TYPE_MCUPERI == tSrcMt.GetType() &&
                EQP_TYPE_VMP == tSrcMt.GetEqpType())
            {
				// 单独请求1路
				u8 bySrcChnnl = 0;
				u8 byMtMediaType = 0;
				u8 byMtRes = 0;
				m_cMtRcvGrp.GetMtMediaRes(tMt.GetMtId(), byMtMediaType, byMtRes);
				bySrcChnnl = GetVmpOutChnnlByRes(byMtRes, m_tVmpEqp.GetEqpId(), byMtMediaType);
				NotifyFastUpdate(tSrcMt, bySrcChnnl); 
            }
            return;
        } 

		if (bRet && !tSrcMt.IsNull() && m_tConfAllMtInfo.MtJoinedConf(tSrcMt.GetMcuId(), tSrcMt.GetMtId()))
		{
			Mt2Log("mt(mtid:%d require framekey to mt(mcuid:%d, mtid:%d)\n",
				   tMt.GetMtId(), tSrcMt.GetMcuId(), tSrcMt.GetMtId());
			NotifyFastUpdate(tSrcMt, MODE_VIDEO);
		}
        
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
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/02/19  3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::StopDoubleStream( BOOL32 bMsgSrcMt, BOOL32 bForceClose )
{
	TMt tOldDStreamSrc = m_tDoubleStreamSrc;

	if(m_tDoubleStreamSrc.IsNull())
	{
		return;
	}

    TLogicalChannel tNullLogicChnnl;
	TLogicalChannel tH239LogicChnnl;
	CServMsg cServMsg;
	
	//sony G70SP在H263-H239时采用活动双流,且不使用令牌而使用FlowControlIndication进行发送激活或关闭 或者
	//POLYCOM7000的静态双流时，使用令牌进行发送激活或关闭
	//均在呼叫建立时即打开,之后与会期间保留通道，直至退出会议
	if( ( MT_MANU_SONY == m_ptMtTable->GetManuId(m_tDoubleStreamSrc.GetMtId()) && 
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
        m_ptMtTable->SetMtVideo2Send( m_tDoubleStreamSrc.GetMtId(), FALSE );

        TMtStatus tMtStatus;
        m_ptMtTable->GetMtStatus(m_tDoubleStreamSrc.GetMtId(), &tMtStatus);
                
        CServMsg cServMsg;
        cServMsg.SetSrcMtId( m_tDoubleStreamSrc.GetMtId() );
        cServMsg.SetEventId( MT_MCU_MTSTATUS_NOTIF );
        cServMsg.SetMsgBody( (u8*)&tMtStatus, sizeof(TMtStatus) );
        g_cMcuVcApp.SendMsgToConf( m_tConf.GetConfId(), MT_MCU_MTSTATUS_NOTIF,
                            cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
    }

	TMt tDstMt;
	//关闭其它终端的H.239通道
    u8 byLoop = 1;
	for( ; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) && 
			m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_SECVIDEO, &tH239LogicChnnl, TRUE ) )
		{	
			// xliang [11/14/2008] PolycomMCU 双流通道不关，但对非双流源的polycom终端，需释放令牌(目前支持polyMCU主呼，keda被动获取令牌方式)
			if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(byLoop) &&
				( MT_TYPE_MMCU == m_ptMtTable->GetMtType(byLoop) ||
				MT_TYPE_SMCU == m_ptMtTable->GetMtType(byLoop) ) )
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
				continue;
			}
			u8 byOut = 1;
			cServMsg.SetMsgBody( ( u8 * )&tH239LogicChnnl, sizeof( tH239LogicChnnl ) );	
			cServMsg.CatMsgBody( &byOut, sizeof(byOut) );
			SendMsgToMt( byLoop, MCU_MT_CLOSELOGICCHNNL_CMD, cServMsg );
			
			tDstMt = m_ptMtTable->GetMt( byLoop );
            
            g_cMpManager.StopSwitchToSubMt(tDstMt, MODE_SECVIDEO, TRUE);
			m_ptMtTable->SetMtLogicChnnl( byLoop, LOGCHL_SECVIDEO, &tNullLogicChnnl, TRUE );

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
                Mt2Log("[StopDoubleStream]tDstMt.%d, MtReqBitrate = %d, adjust restore\n", tDstMt.GetMtId(), wRealBR);                
            }
            else
            {
                u16 wMtDialBitrate = m_ptMtTable->GetRcvBandWidth(tDstMt.GetMtId());
                m_ptMtTable->SetMtReqBitrate(tDstMt.GetMtId(), wMtDialBitrate, LOGCHL_VIDEO);
                Mt2Log("[StopDoubleStream]tDstMt.%d, MtReqBitrate = %d, normally restore\n", tDstMt.GetMtId(), wMtDialBitrate);
            }

			//卫星会议恢复面向标清的主流交换
#ifdef _SATELITE_
			
			BOOL32 bHd = ::topoGetMtInfo(LOCAL_MCUID, tDstMt.GetMtId(), g_atMtTopo, g_wMtTopoNum).IsMtHd();
			if (!bHd && !m_tVidBrdSrc.IsNull())
			{
				StartSwitchToSubMtFromAdp(tDstMt.GetMtId(), FALSE);
			}
#endif


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

    //zbq[02/18/2008] 停止双流，需整理VMP的广播码率
    if ( m_tConf.m_tStatus.IsBrdstVMP() )
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
    
	//第二路视频的丢包重传
	if( m_tConf.m_tStatus.IsPrsing() )
	{
		StopPrs( PRSCHANMODE_SECOND );
	}

    // xsl [4/29/2006] 恢复适配器码率
    TSimCapSet tDstSCS, tSrcSCS;
    if (m_tConf.m_tStatus.IsVidAdapting())
    {        
        //目前暂不考虑双速会议分辨率适配恢复...
        if (IsNeedVidAdapt(tDstSCS, tSrcSCS, m_wVidBasBitrate))
        {
            ChangeAdapt(ADAPT_TYPE_VID, m_wVidBasBitrate, &tDstSCS, &tSrcSCS);
        }        
    }
    if (m_tConf.m_tStatus.IsBrAdapting())
    {
        if (IsNeedBrAdapt(tDstSCS, tSrcSCS, m_wBasBitrate))
        {
            ChangeAdapt(ADAPT_TYPE_BR, m_wBasBitrate, &tDstSCS, &tSrcSCS);
        }        
    }

    //zbq[12/11/2008] 高清适配：停双流交换；整理主流适配带宽
    StopAllHdBasSwitch(TRUE);
    m_tConf.m_tStatus.SetHDAdaptMode(CONF_HDBASMODE_DOUBLEVID, FALSE);

    if (m_tConf.m_tStatus.IsHdVidAdapting())
    {
        RefreshBasParam4AllMt(TRUE);
    }

	//zbq[09/12/2008] 时序调整
	m_tDoubleStreamSrc.SetNull();

    //如果关双流时有终端发言人，则应调整该终端的发送码率恢复
	TLogicalChannel tLogicalChannel;
    if (!m_tVidBrdSrc.IsNull() && TYPE_MT == m_tVidBrdSrc.GetType())
    {
        if (m_ptMtTable->GetMtLogicChnnl(m_tVidBrdSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
        {            
            tLogicalChannel.SetFlowControl(m_ptMtTable->GetSndBandWidth(m_tVidBrdSrc.GetMtId()));
            cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
            SendMsgToMt(m_tVidBrdSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
        }
    }    
    
	//若发双流终端正在发送第一路码流，恢复码率
	if(IsDStreamMtSendingVideo(tOldDStreamSrc))
	{
		if (m_ptMtTable->GetMtLogicChnnl(tOldDStreamSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
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
            tOtherMt = tStatus.GetSelectMt(MODE_VIDEO);
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

    RefreshRcvGrp();

#ifdef _SATELITE_
	bySrcChn = m_tPlayEqp == m_tDoubleStreamSrc ? m_byPlayChnnl : 0;
	g_cMpManager.StopSatConfCast(tOldDStreamSrc, CAST_FST, MODE_SECVIDEO, bySrcChn);
#endif

#ifdef _SATELITE_
	RefreshConfState();
#endif

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
		m_byLastDsSrcMtId = m_tDoubleStreamSrc.GetMtId(); 
		StopDoubleStream(TRUE, FALSE);
	}

    // zbq [09/04/2007] 录像机双流放象，此处还不能建交换
    if ( TYPE_MCUPERI == tMt.GetType() && EQP_TYPE_RECORDER == tMt.GetEqpType())
    {
    }
    else
    {
        g_cMpManager.SetSwitchBridge(tMt, 0, MODE_SECVIDEO);
    }

	TLogicalChannel tH239LogicChnnl;
    TCapSupport tCapSupport;
	CServMsg cSendMsg;
	//打开其它终端的H.239通道
	for(u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
	{
        if (byLoop == tMt.GetMtId() && TYPE_MCUPERI != tMt.GetType())
        {
            continue;
        }

        if (!m_tConfAllMtInfo.MtJoinedConf(byLoop))
        {
            continue;
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

		// xliang [12/19/2008]  对于上次发起双流的MT,延时打开起双流通道。
		//(在收到close LC notif之后才去打开，保证之前双流通道确实已关闭)
		if( byLoop == m_byLastDsSrcMtId )
		{
			m_tLogicChnnl = tLogicChnnl; // xliang [12/19/2008] 记录logical channel，为了双流抢发，延时策略
			continue;
		}
        //普通终端打开双流
        if(!m_ptMtTable->GetMtCapSupport( tOtherMt.GetMtId(), &tCapSupport ))
        {
            continue;
        }
		// xliang [11/14/2008]  针对polycom双流的特殊处理
		//polycom的双流通道在终端呼入时就确立并完成了打开操作;
		//此处对非令牌Polycom的MT，直接flowctrl其带宽即可. 对Polycom的MCU，需先请求令牌，请求令牌成功后发起flowctrl
		if (MT_MANU_POLYCOM == m_ptMtTable->GetManuId(byLoop) &&
            (MT_TYPE_SMCU == m_ptMtTable->GetMtType(byLoop) || 
			MT_TYPE_MMCU == m_ptMtTable->GetMtType(byLoop)))
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
				CallLog("[StartDoubleStream] MCU_POLY_GETH239TOKEN_REQ to PolyMCU.%d\n", byLoop);
			}
			else
			{
				ConfLog( FALSE, "[StartDoubleStream] No send MCU_POLY_GETH239TOKEN_REQ to polyMCU due to Forward chnnl closed, try to reopen it, start DS once more, please\n" );
			}
		}
		else
		{
			McuMtOpenDoubleStreamChnnl(tOtherMt, tLogicChnnl, tCapSupport);  
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

        // 通知会控终端双流状态改变 [02/05/2007-zbq]
        TMtStatus tMtStatus;
        m_ptMtTable->GetMtStatus(tMt.GetMtId(), &tMtStatus);
        
        CServMsg cServMsg;
        cServMsg.SetSrcMtId( tMt.GetMtId() );
        cServMsg.SetEventId( MT_MCU_MTSTATUS_NOTIF );
        cServMsg.SetMsgBody( (u8*)&tMtStatus, sizeof(TMtStatus) );
        g_cMcuVcApp.SendMsgToConf( m_tConf.GetConfId(), MT_MCU_MTSTATUS_NOTIF,
                            cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
    }

   
    
	//启用适配, 则强制调整第一路发送源的发送码率折半,没有适配器则根据FlowControl调整
    if (!m_tVidBrdSrc.IsNull() && TYPE_MT == m_tVidBrdSrc.GetType())
    {
        CServMsg cServMsg;
        TLogicalChannel tLogicalChannel;
        if (TRUE == m_ptMtTable->GetMtLogicChnnl(m_tVidBrdSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
        {
            //如果开双流时有终端发言人，则应调整该终端的发送码率减半，并保存在通道信息中
            tLogicalChannel.SetFlowControl(GetDoubleStreamVideoBitrate(m_ptMtTable->GetSndBandWidth(m_tVidBrdSrc.GetMtId())));
            cServMsg.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));
            SendMsgToMt(m_tVidBrdSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg);
        }
    }

	RefreshRcvGrp();

    //双流录像
    AdjustRecordSrcStream( MODE_SECVIDEO );

    //强制调节适配器码率 -- modify bas 2
    TSimCapSet tDstSimCapSet, tSrcSimCapSet;
    u16 wAdaptBitRate;
    if (m_tConf.m_tStatus.IsVidAdapting())
    {        
        if (IsNeedVidAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate))
        {
            ChangeAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
        }        
    }
    if (m_tConf.m_tStatus.IsBrAdapting())
    {
        if (IsNeedBrAdapt(tDstSimCapSet, tSrcSimCapSet, wAdaptBitRate, &m_tVidBrdSrc))
        {
            ChangeAdapt(ADAPT_TYPE_BR, wAdaptBitRate, &tDstSimCapSet, &tSrcSimCapSet);
        }        
    }
    
    if (m_cMtRcvGrp.IsNeedAdp(FALSE))
    {
        if (!m_tConf.m_tStatus.IsHDDoubleVidAdapting())
        {
            StartHDDSBrdAdapt();
        }
        else
        {
			// fxh[20090213]bas已启动，双流源切换时需判断输入码流参数是否有修改
			TDStreamCap tNewDSCap;
			GetDSBrdSrcSim(tNewDSCap);
			if (tOldDSCap.GetMediaType() != tNewDSCap.GetMediaType())
			{
				u8 byDSBasNum = 0;

                //调整比源能力大的输出参数
                TBasChn atBasChn[MAXNUM_CONF_MVCHN];
                m_cBasMgr.GetChnGrp(byDSBasNum, atBasChn, CHN_ADPMODE_DSBRD);

				for(u8 byIdx = 0; byIdx < byDSBasNum; byIdx ++)
				{
                    if (!atBasChn[byIdx].IsNull())
                    {
                        ChangeHDAdapt(atBasChn[byIdx].GetEqp(), atBasChn[byIdx].GetChnId());
                    }
                }
			}
			// 输出码流参数调整
            RefreshBasParam4AllMt(TRUE, TRUE);
            StartAllHdBasSwitch(TRUE);
        }
    }

	// 刷新适配编码
    if ( m_tConf.m_tStatus.IsHdVidAdapting() )
    {
        RefreshBasParam4AllMt(TRUE);
    }
    
    //组播会议组播数据
    if (m_tConf.GetConfAttrb().IsMulticastMode())
    {
        g_cMpManager.StartMulticast(m_tDoubleStreamSrc, 0, MODE_SECVIDEO);
    }
#ifndef _SATELITE_
    //卫星组播
    if (m_tConf.GetConfAttrb().IsSatDCastMode())
    {
        g_cMpManager.StartDistrConfCast(m_tDoubleStreamSrc, MODE_SECVIDEO);      
    }
#endif

#ifdef _SATELITE_
	u8 bySrcChn = m_tPlayEqp == m_tDoubleStreamSrc ? m_byPlayChnnl : 0;
	g_cMpManager.StartSatConfCast(m_tDoubleStreamSrc, CAST_FST, MODE_SECVIDEO, bySrcChn);
#endif

    // guzh [10/30/2007] 
    if (m_tDoubleStreamSrc.GetType() == TYPE_MT)
    {
        //新的双流开启发送时，强制关键帧
        SetTimer(MCUVC_SECVIDEO_FASTUPDATE_TIMER, 1200, 100*m_tDoubleStreamSrc.GetMtId()+1);	       
    }

    // guzh [10/30/2007] 注意该消息的处理，不仅仅是向双流源发送调整码率请求，所以一定要设置定时器
    //等待1s向双流发起终端发送flowcontrol调节双流码率
    SetTimer(MCUVC_SENDFLOWCONTROL_TIMER, 1200);

#ifdef _SATELITE_
	RefreshConfState();
#endif

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
void CMcuVcInst::ProcSendFlowctrlToDSMtTimeout(const CMessage * pcMsg )
{
    if (m_tDoubleStreamSrc.IsNull())
    {
        ConfLog(FALSE, "[ProcSendFlowctrlToDSMtTimeout] no double stream source mt!\n");
        return;
    }

    CServMsg cServMsg0, cServMsg1;
    TLogicalChannel tLogicalChannel;
    if (m_ptMtTable->GetMtLogicChnnl(m_tDoubleStreamSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
    {
        tLogicalChannel.SetFlowControl(GetDoubleStreamVideoBitrate(m_ptMtTable->GetSndBandWidth(m_tDoubleStreamSrc.GetMtId())));
        tLogicalChannel.SetCurrFlowControl(0xffff);//将上次保存的flowcontrol改变，防止被过滤
        cServMsg0.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));        
    }

	TMtStatus tStatus;
	BOOL32 bIsOpenLogChnn = TRUE;
    u16 wMtDialBitrate;     
    u16 wMinDialBitrate = m_ptMtTable->GetRcvBandWidth(m_tDoubleStreamSrc.GetMtId());
    u8 byLoop = 1;
    for (; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {         
        if (byLoop == m_tDoubleStreamSrc.GetMtId() || !m_tConfAllMtInfo.MtJoinedConf(byLoop))
        {
            continue;
        }

        //zbq[07/28/2009] 双流flowctrl修正：非免适配终端不响应flowctrl，HD-AdpParam自然调整
        if (IsHDConf(m_tConf) &&
            m_cMtRcvGrp.IsMtNeedAdp(byLoop, FALSE))
        {
            continue;
        }

        wMtDialBitrate = m_ptMtTable->GetRcvBandWidth(byLoop);
		
        //判断终端是否打开了视频逻辑通道, 防止最低码率被清0
		bIsOpenLogChnn = m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_VIDEO, &tLogicalChannel, TRUE);
		
        //zbq[09/25/2008] 保护被错误清0的第一路通道影响到双流带宽
        if (0 != wMtDialBitrate &&
			wMinDialBitrate > wMtDialBitrate && bIsOpenLogChnn )
        {
            wMinDialBitrate = wMtDialBitrate;
        }
    }
	
    if (m_ptMtTable->GetMtLogicChnnl(m_tDoubleStreamSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE))
    {
        tLogicalChannel.SetFlowControl(GetDoubleStreamVideoBitrate(wMinDialBitrate, FALSE));
        tLogicalChannel.SetCurrFlowControl(0xffff);//将上次保存的flowcontrol改变，防止被过滤
        cServMsg1.SetMsgBody((u8*)&tLogicalChannel, sizeof(tLogicalChannel));        
    }
		
	BOOL32 bOtherMtSrc = IsDStreamMtSendingVideo(m_tDoubleStreamSrc);
    //先发小的flowcontrol
    if (GetDoubleStreamVideoBitrate(m_tConf.GetBitRate()) < m_tConf.GetBitRate()/2)
    {
		if (bOtherMtSrc)
		{
			SendMsgToMt(m_tDoubleStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg0);     
		}
        SendMsgToMt(m_tDoubleStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg1);        
    }
    else
    {
        SendMsgToMt(m_tDoubleStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg1);    
        if (bOtherMtSrc)
		{
			SendMsgToMt(m_tDoubleStreamSrc.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg0);             
		}		
    }
    
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
        if ( !tOtherMt.IsNull() &&
             !(tOtherMt == m_tDoubleStreamSrc) )
        {
            // 根据终端当前的发送码率、双方的收发能力确定flowcontrol的值
            m_ptMtTable->GetMtLogicChnnl( tOtherMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, TRUE);
            wBitrate = min ( m_ptMtTable->GetSndBandWidth(tOtherMt.GetMtId()), m_ptMtTable->GetRcvBandWidth(byLoop) );
            wBitrate = GetDoubleStreamVideoBitrate(wBitrate);
            wBitrate = min( wBitrate, tLogicalChannel.GetFlowControl() );
            tLogicalChannel.SetFlowControl(wBitrate);
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
		McuMtOpenDoubleStreamChnnl(tMt, tLogicalChannel, tCapSupport);
	}

	return;
}

/*=============================================================================
函 数 名： GetDoubleStreamVideoBitrate
功    能： 获取双流视频码率，默认第一路
算法实现： 
全局变量： 
参    数： 
返 回 值： u16  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/3/23  4.0			许世林                  创建
=============================================================================*/
u16  CMcuVcInst::GetDoubleStreamVideoBitrate(u16 wDialBitrate, BOOL32 bFst /*= TRUE*/)
{ 
    u8 byDStreamScale = m_tConf.GetDStreamScale();    
    
    u16 wSndBitrate = byDStreamScale*wDialBitrate/100; 
    return bFst ? (wDialBitrate - wSndBitrate) : wSndBitrate;
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
                                            const TLogicalChannel &tLogicChnnl, 
                                            const TCapSupport &tCapSupport)
{
	u32 dwRcvIp = 0;
    u16 wRcvPort = 0;
    TLogicalChannel tDStreamChnnl;
    CServMsg cMsg;

	// xliang [11/14/2008]  针对polycom双流的特殊处理
	if ( MT_MANU_POLYCOM == m_ptMtTable->GetManuId(tMt.GetMtId()) )
	{
		//Polycom打开双流是无条件的，通道信息不做校验，直接打开
		if (tCapSupport.IsDStreamSupportH239() &&
			m_tConf.GetCapSupport().GetDStreamCapSet().GetMediaType() == tCapSupport.GetDStreamMediaType())
		{
			if( !m_ptMtTable->GetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort ) )
			{
				dwRcvIp = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( tMt.GetMtId() ) );
				if( dwRcvIp == 0 )
				{
					ConfLog( FALSE, "Mt%d 0x%x(Dri:%d) mp not connected(Polycom)!\n",
						tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() );		    
				}
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
					wChanBitrate = min(wChanBitrate, 
						GetDoubleStreamVideoBitrate(m_ptMtTable->GetDialBitrate(tMt.GetMtId()), FALSE) );
				}
			}
			tDStreamChnnl.SetFlowControl(wChanBitrate);
			
			tDStreamChnnl.m_tSndMediaCtrlChannel.SetIpAddr( dwRcvIp );
			tDStreamChnnl.m_tSndMediaCtrlChannel.SetPort( wRcvPort + 5 );        
			tDStreamChnnl.SetMediaEncrypt(m_tConf.GetMediaKey());
			tDStreamChnnl.SetActivePayload( GetActivePayload( m_tConf,tDStreamChnnl.GetChannelType() ) );        
			cMsg.SetMsgBody( ( u8 * )&tDStreamChnnl, sizeof( tDStreamChnnl ) );
			
			u8 byVideoFormat = m_tConf.GetVideoFormat(tDStreamChnnl.GetChannelType(), LOGCHL_SECVIDEO);
			cMsg.CatMsgBody( (u8*)&byVideoFormat, sizeof(u8) );
			
			SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cMsg );
			
			//FIXME: TEST
			CallLog("[McuMtOpenDoubleStreamChnnl] Mt.%d 's CurReqBR.%d, GetDSVBR.%d(polycom)\n",
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
			CallLog( "Open Second Video LogicChannel Request send to Mt%d, H239.%d, BR.%d, type: %s\n", 
				tMt.GetMtId(), tDStreamChnnl.IsSupportH239(),
				tDStreamChnnl.GetMediaType(), GetMediaStr( tDStreamChnnl.GetChannelType() ) );
		}
		else
		{
			CallLog("[McuMtOpenDoubleStreamChnnl] IsH239.%d, CapDSType.%d. ConfDSType.%d\n",
				tCapSupport.IsDStreamSupportH239(),
				tCapSupport.GetDStreamMediaType(),
				m_tConf.GetCapSupport().GetDStreamCapSet().GetMediaType());
		}
		return;
	}

    TLogicalChannel tLogicChnAdj = tLogicChnnl;  //  pengjie[8/11/2009]

	//zbq[09/02/2008] 双流通道分辨率匹配校验
	u8 bySrcRes = tLogicChnnl.GetVideoFormat();
	u8 byDstRes = tCapSupport.GetDStreamCapSet().GetResolution();
	if (!IsDSResMatched(bySrcRes, byDstRes))
	{
		CallLog("[McuMtOpenDoubleStreamChnnl] Mt.%d's DS res dismatched<src.%d, dst.%d>, ignore it\n", 
			tMt.GetMtId(), bySrcRes, byDstRes);
		return;
	}
    
    BOOL32 bNonMVAdp = TRUE;

    //zbq[04/15/2009] 高清会议，主流不支持适配，双流不打开小于会议帧率或分辨率的H264通道
    if (MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType()
        &&
        (m_tConf.GetConfAttrbEx().IsResEx1080() ||
         m_tConf.GetConfAttrbEx().IsResEx720()  ||
         m_tConf.GetConfAttrbEx().IsResEx4Cif() ||
         m_tConf.GetConfAttrbEx().IsResExCif()  ||
         (0 != m_tConf.GetSecVideoMediaType() &&
          MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType())) )
    {
        bNonMVAdp = FALSE;
    }

    BOOL32 bDstResMatch = IsResGE(tCapSupport.GetDStreamResolution(), tLogicChnnl.GetVideoFormat());
    BOOL32 bDstFpsMatch = tCapSupport.GetDStreamUsrDefFPS() >= tLogicChnnl.GetChanVidFPS();

    if (tLogicChnnl.GetChannelType() == MEDIA_TYPE_H264 &&
        tCapSupport.GetDStreamMediaType() == MEDIA_TYPE_H264)
    {
        if (bNonMVAdp)
        {
            if (!bDstFpsMatch || !bDstResMatch)
            {
                s8 achCapRes[32] = {0};
                strcpy(achCapRes, GetResStr(tCapSupport.GetDStreamResolution()));

                CallLog( "Mt%d second video media cap<Res:%s, Fps.%d> isn't match to conf<Res:%s, Fps.%d>, \
no open video logicChannel request send to it!\n", 
                    tMt.GetMtId(),
                    achCapRes, tCapSupport.GetDStreamUsrDefFPS(),
                    GetResStr(tLogicChnnl.GetVideoFormat()), tLogicChnnl.GetChanVidFPS());
                return;
            }
        }
        else
        {
            //zbq[05/08/2009] 按对端低能力打开双流通道 伪装
            if (!bDstFpsMatch)
            {
                tLogicChnAdj.SetChanVidFPS(tCapSupport.GetDStreamUsrDefFPS());
            }
            if (!bDstResMatch)
            {
                tLogicChnAdj.SetVideoFormat(tCapSupport.GetDStreamResolution());
            }
        }
    }

    //zbq[12/27/2008] 按第二双流能力打开双流通道 伪装
    if (tLogicChnAdj.GetChannelType() != tCapSupport.GetDStreamMediaType() &&
        (tLogicChnAdj.GetChannelType() == m_tConf.GetCapSupport().GetDStreamMediaType() ||
         tLogicChnAdj.GetChannelType() == m_tConf.GetCapSupportEx().GetSecDSType()))
    {
        CallLog("Mt.%d's dschan been adjusted from.%d to %d, conf<%d, %d>\n",
            tMt.GetMtId(), tLogicChnAdj.GetChannelType(), tCapSupport.GetDStreamMediaType(),
            m_tConf.GetCapSupport().GetDStreamMediaType(), m_tConf.GetCapSupportEx().GetSecDSType());

        tLogicChnAdj.SetActivePayload(GetActivePayload(m_tConf, tCapSupport.GetDStreamMediaType()));
        tLogicChnAdj.SetChannelType(tCapSupport.GetDStreamMediaType());
        if (MEDIA_TYPE_H264 == tCapSupport.GetDStreamMediaType())
        {
            tLogicChnAdj.SetChanVidFPS(tCapSupport.GetDStreamUsrDefFPS());
        }
        else
        {
            tLogicChnAdj.SetChanVidFPS(tCapSupport.GetDStreamFrmRate());
        }
        tLogicChnAdj.SetVideoFormat(tCapSupport.GetDStreamResolution());
        tLogicChnAdj.SetSupportH239(tCapSupport.IsDStreamSupportH239());
    }
         

    if( tLogicChnAdj.IsSupportH239() == tCapSupport.IsDStreamSupportH239() &&
        tLogicChnAdj.GetChannelType() == tCapSupport.GetDStreamMediaType() )
    {
        if( !m_ptMtTable->GetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort ) )
        {
            dwRcvIp = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( tMt.GetMtId() ) );
            if( dwRcvIp == 0 )
            {
                ConfLog( FALSE, "Mt%d 0x%x(Dri:%d) mp not connected!\n",
                    tMt.GetMtId(), m_ptMtTable->GetIPAddr( tMt.GetMtId() ), tMt.GetDriId() );		    
            }
            wRcvPort = g_cMcuVcApp.AssignMtPort( tMt.GetConfIdx(), tMt.GetMtId() );
            m_ptMtTable->SetMtSwitchAddr( tMt.GetMtId(), dwRcvIp, wRcvPort );
        }
        
        if( tLogicChnAdj.IsSupportH239())
        {
            tDStreamChnnl.SetSupportH239( tCapSupport.IsDStreamSupportH239() );
            NotifyH239TokenOwnerInfo( &tMt );
        }
        tDStreamChnnl.SetMediaType( MODE_SECVIDEO );
        tDStreamChnnl.SetChannelType( tLogicChnAdj.GetChannelType() );
        tDStreamChnnl.SetChanVidFPS( tLogicChnAdj.GetChanVidFPS() );
        tDStreamChnnl.SetVideoFormat( tLogicChnAdj.GetVideoFormat() );

        //zbq[01/31/2008] FIXME: 这个打开双流通道带宽是有问题的
        //tDStreamChnnl.SetFlowControl(  );

        // guzh [2008/03/07] 兼顾考虑E1 降速和Polycom等其他厂商对于通道码率的严格校验
        // 另一部分控制在MtAdp中
        u16 wChanBitrate = m_ptMtTable->GetMtReqBitrate( tMt.GetMtId(), FALSE );

        if ( !( m_ptMtTable->GetMtTransE1(tMt.GetMtId()) && m_ptMtTable->GetMtBRBeLowed(tMt.GetMtId())) )
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
					wChanBitrate = min(wChanBitrate, 
						GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId()), FALSE) );
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
					wChanBitrate = min(wChanBitrate, 
						GetDoubleStreamVideoBitrate(m_ptMtTable->GetDialBitrate(tMt.GetMtId()), FALSE) );
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
				wChanBitrate = min(wChanBitrate, 
					GetDoubleStreamVideoBitrate(m_ptMtTable->GetLowedRcvBandWidth(tMt.GetMtId()), FALSE) );
			}
		}
        tDStreamChnnl.SetFlowControl(wChanBitrate);

        tDStreamChnnl.m_tSndMediaCtrlChannel.SetIpAddr( dwRcvIp );
        tDStreamChnnl.m_tSndMediaCtrlChannel.SetPort( wRcvPort + 5 );        
        tDStreamChnnl.SetMediaEncrypt(m_tConf.GetMediaKey());
        tDStreamChnnl.SetActivePayload( GetActivePayload(m_tConf, tDStreamChnnl.GetChannelType() ) );        
        cMsg.SetMsgBody( ( u8 * )&tDStreamChnnl, sizeof( tDStreamChnnl ) );
        
		//zbq[09/02/2008] Format按终端能力打开，会议只作保护辅助
        u8 byVideoFormat = tDStreamChnnl.GetVideoFormat();
		if (byVideoFormat == 0)
		{
			byVideoFormat = m_tConf.GetVideoFormat(tDStreamChnnl.GetChannelType(), LOGCHL_SECVIDEO);
		}
		tDStreamChnnl.SetVideoFormat(byVideoFormat);
		
		cMsg.SetMsgBody( ( u8 * )&tDStreamChnnl, sizeof( tDStreamChnnl ) );
        cMsg.CatMsgBody( (u8*)&byVideoFormat, sizeof(u8) );

		cMsg.SetDstMtId( tMt.GetMtId() );
        
        SendMsgToMt( tMt.GetMtId(), MCU_MT_OPENLOGICCHNNL_REQ, cMsg );

        //FIXME: TEST
        CallLog("[McuMtOpenDoubleStreamChnnl] Mt.%d 's CurReqBR.%d, GetDSVBR.%d, Format.%d\n",
                 tMt.GetMtId(),
				 m_ptMtTable->GetMtReqBitrate(tMt.GetMtId(), TRUE),
				 GetDoubleStreamVideoBitrate(m_ptMtTable->GetRcvBandWidth(tMt.GetMtId())),
				 byVideoFormat);

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
        CallLog( "Open Second Video LogicChannel Request send to Mt%d, DStream.%d, type: %s\n", 
                tMt.GetMtId(), tDStreamChnnl.IsSupportH239(), GetMediaStr( tDStreamChnnl.GetChannelType() ) );
    }
    else
    {
        //打印消息
        CallLog( "Mt%d <%d-H239.%d> not support any conf second video media type<%d:%d-H239.%d>, \
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
		//变更交换
		StopSwitchToSubMt( tDstMt.GetMtId(), MODE_VIDEO );

		m_ptMtTable->SetMtVideo2Recv( tDstMt.GetMtId(), TRUE );

        //zbq [08/23/2007] 录像机放双流建交换要依据放象通道建立
        u8 bySrcChn = m_tDoubleStreamSrc == m_tPlayEqp ? m_byPlayChnnl : 0;
        g_cMpManager.StartSwitchToSubMt( m_tDoubleStreamSrc, bySrcChn, tDstMt, MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE );

		//设置终端状态
		m_ptMtTable->SetMtSrc( tDstMt.GetMtId(), ( const TMt * )&m_tDoubleStreamSrc, MODE_VIDEO );

		CallLog( "StartSwitchDStreamToFirstLChannel Mt%d\n", tDstMt.GetMtId() );
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
#ifndef _SATELITE_
	if( m_tConf.GetCapSupport().IsDStreamSupportH239() )
	{
		return;
	}
#endif
	
	TLogicalChannel tLogicChnnl;
	if( MT_MANU_SONY == m_ptMtTable->GetManuId(tDstMt.GetMtId()) &&
        !m_ptMtTable->GetMtLogicChnnl( tDstMt.GetMtId(), LOGCHL_SECVIDEO, &tLogicChnnl, TRUE ) && 
		m_ptMtTable->GetMtLogicChnnl( tDstMt.GetMtId(), LOGCHL_VIDEO, &tLogicChnnl, TRUE ) && 
		tLogicChnnl.GetChannelType() == m_tConf.GetCapSupport().GetDStreamMediaType() )
	{
		//恢复为主视频源
		RestoreRcvMediaBrdSrc( tDstMt.GetMtId(), MODE_VIDEO, TRUE );
		CallLog( "StopSwitchDStreamToFirstLChannel Mt%d\n", tDstMt.GetMtId() );
	}

	
#ifdef _SATELITE_
	//卫星标清终端 将主流接收切换收正常主流源
	if (!m_tVidBrdSrc.IsNull())
	{
		u8 bySrcChn = m_tVidBrdSrc == m_tPlayEqp ? m_byPlayChnnl : 0;
		g_cMpManager.StartSatConfCast(m_tVidBrdSrc, CAST_SEC, MODE_VIDEO, bySrcChn);
	}

	BOOL32 bHd = ::topoGetMtInfo(LOCAL_MCUID, tDstMt.GetMtId(), g_atMtTopo, g_wMtTopoNum).IsMtHd();
	if (!bHd && !m_tVidBrdSrc.IsNull())
	{
		StartSwitchToSubMtFromAdp(tDstMt.GetMtId(), FALSE);
	}
#endif

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
		if( NULL != ptMt )
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
			CServMsg cServMsg;
			TH239TokenInfo tH239Info;
			tH239Info.SetChannelId( m_tH239TokenOwnerInfo.GetChannelId() );
			tH239Info.SetSymmetryBreaking( 0 );
			cServMsg.SetEventId(MCU_MT_RELEASEH239TOKEN_CMD);
			cServMsg.SetMsgBody((u8 *)&m_tH239TokenOwnerInfo.m_tH239TokenMt, sizeof(TMt));
			cServMsg.CatMsgBody((u8 *)&tH239Info, sizeof(TH239TokenInfo));
			SendMsgToMt( m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId(), MCU_MT_RELEASEH239TOKEN_CMD, cServMsg);

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
			if( byLoop == m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId() )
            {
                continue;
			}
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
		if( ptMt->GetMtId() != m_tH239TokenOwnerInfo.m_tH239TokenMt.GetMtId() )
		{
			tH239Info.SetChannelId( m_tH239TokenOwnerInfo.GetChannelId() );
			cServMsg.SetEventId(MCU_MT_OWNH239TOKEN_NOTIF);
			cServMsg.SetMsgBody((u8 *)ptMt, sizeof(TMt));
			cServMsg.CatMsgBody((u8 *)&tH239Info, sizeof(TH239TokenInfo));
			SendMsgToMt( ptMt->GetMtId(), MCU_MT_OWNH239TOKEN_NOTIF, cServMsg);
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

	TMtStatus tMtStatus;
	TMt  tDstMt;
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
				if( (!tSelMt.IsLocal() && tSelMt.GetMcuId() == tSrcMt.GetMtId()) ||
					tMtStatus.GetSelectMt(MODE_AUDIO) == tSrcMt ) // 交换音频给它
				{
					if( bStartSwitch )
					{
						StartSwitchToSubMt( tSrcMt, 0,  byMtIdxLp, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE );
					}
					else
					{
						StopSwitchToSubMt( tDstMt.GetMtId(), MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE);
					}
				}
			}
		}
	}

	// 对于VCS MCS当前选看终端为该哑音对象时需要进行相应的建拆交换
	TMt tMCAudSrc;
	tMCAudSrc.SetNull();
	for (u8 byMCIdx = 1; byMCIdx <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byMCIdx++)
	{
		for (u8 byChnl = 0; byChnl < MAXNUM_MC_CHANNL; byChnl++)
		{
			if (g_cMcuVcApp.GetMcSrc(byMCIdx, &tMCAudSrc, byChnl, MODE_AUDIO) &&
				((!tMCAudSrc.IsLocal() && tMCAudSrc.GetMcuId() == tSrcMt.GetMtId()) ||
				  tMCAudSrc == tSrcMt))
			{
				if (bStartSwitch)
				{
				    g_cMpManager.StartSwitchToMc(tMCAudSrc, 0, byMCIdx, byChnl, MODE_AUDIO);
				}
				else
				{
					g_cMpManager.StopSwitchToMc(m_byConfIdx, byMCIdx, byChnl, MODE_AUDIO);
				}
			}
		}
		
	}

	return;
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
    /*
	if( FALSE == HasJoinedSpeaker() )
	{
        return;
	}
    */

    // xsl [9/20/2006] 需要考虑广播源为混音器的情况
	TMt tAudSrcMt = m_tAudBrdSrc;

	//终端静音，停止外部向该终端交换音频
	if( TRUE == bDecodeAud && TRUE == bOpenFlag )
	{
        //不取消选看
		StopSwitchToSubMt( byDstMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE, FALSE );
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
            if (tAudSrcMt.GetType() == TYPE_MCUPERI && tAudSrcMt.GetEqpType() == EQP_TYPE_MIXER)
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
                StartSwitchToSubMt( tAudSrcMt, 0,  byDstMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE );
            }            
		}
		else
		{	
            //恢复音频选看
			StartSwitchToSubMt( tSelectMt, 0,  byDstMtId, MODE_AUDIO, SWITCH_MODE_SELECT, FALSE );
		}
	}

    /*
    //  xsl [2/24/2006] 暂时保持原来的处理逻辑，有需求时再放开
	if( tAudSrcMt.GetMtId() != byDstMtId )
	{
		return;
	}
    */

	//终端哑音，停止该终端向外部交换音频
	if( FALSE == bDecodeAud && TRUE == bOpenFlag )
	{
        if (!tAudSrcMt.IsNull() && tAudSrcMt.GetType() == TYPE_MCUPERI)
        {
            // xsl [9/20/2006] 停止到混音器的交换
            if (tAudSrcMt.GetEqpType() == EQP_TYPE_MIXER && m_ptMtTable->IsMtInMixGrp(byDstMtId))
            {
                StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
							        (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(byDstMtId)), 
                                     FALSE, MODE_AUDIO );
            }
        }
        else
        {
            //被哑音终端是发言人
		    if( tAudSrcMt.GetMtId() == byDstMtId )
		    {
			    for( u8 byMtLoop = 1; byMtLoop <= MAXNUM_CONF_MT; byMtLoop++ )
			    {
				    if( m_tConfAllMtInfo.MtJoinedConf( byMtLoop ) )
				    {	
					    StopSwitchToSubMt( byMtLoop, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE );
				    }
			    }
		    }
		    else
		    {
                //处理该终端被其它终端选看
			    ProcMtSelectdByOtherMt(byDstMtId, FALSE);
		    }
        }
		
	}

	//取消终端哑音，恢复该终端向外部交换音频
	if( FALSE == bDecodeAud && FALSE == bOpenFlag )
	{
        if (!tAudSrcMt.IsNull() && tAudSrcMt.GetType() == TYPE_MCUPERI)
        {
            // xsl [9/20/2006] 恢复到混音器的交换
            if (tAudSrcMt.GetEqpType() == EQP_TYPE_MIXER && m_ptMtTable->IsMtInMixGrp(byDstMtId))
            {
                TMt tSrcMt = m_ptMtTable->GetMt(byDstMtId);
                StartSwitchToPeriEqp(tSrcMt, 0, m_tMixEqp.GetEqpId(), 
								     (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+GetMixChnPos(byDstMtId)),
								     MODE_AUDIO, SWITCH_MODE_SELECT);
            }
        }
        else
        {
            //被哑音终端是发言人
		    if( tAudSrcMt.GetType() == TYPE_MT && tAudSrcMt.GetMtId() == byDstMtId )
		    {
			    StartSwitchToAllSubMtJoinedConf( tAudSrcMt, 0 );
		    }
		    else
		    {
                //处理该终端被其它终端选看
			    ProcMtSelectdByOtherMt(byDstMtId, TRUE);
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
    返 回 值： BOOL32
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
    if (g_cMcuVcApp.IsSelInDoubleMediaConf())
    {
        return TRUE;
    }

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
            if (!IsHDConf(m_tConf) || !g_cMcuVcApp.IsSelAccord2Adp())
            {
                bySwitchMode = ( MODE_BOTH == bySwitchMode ) ? MODE_AUDIO : MODE_NONE;

                ConfLog( FALSE, "[IsSelModeAndCapMatched] capbility match failed: \n\t\tSrcCap audio<%d> video<%d> vs DstCap audio<%d> video<%d>\n",
                    tSimCapSrc.GetAudioMediaType(), tSimCapSrc.GetVideoMediaType(),
                    tSimCapDst.GetAudioMediaType(), tSimCapDst.GetVideoMediaType() );
            }
            else
            {
                ConfLog( FALSE, "[IsSelModeAndCapMatched] capbility match loss, need adp: \n\t\tSrcCap audio<%d> video<%d> vs DstCap audio<%d> video<%d>\n",
                    tSimCapSrc.GetAudioMediaType(), tSimCapSrc.GetVideoMediaType(),
                    tSimCapDst.GetAudioMediaType(), tSimCapDst.GetVideoMediaType() );
                
                bAccord2Adp = TRUE;
            }
        }
        if ( MODE_NONE == bySwitchMode )
        {           
            ConfLog( FALSE, "[IsSelModeAndCapMatched] capbility match failed: \n\t\tSrcCap audio<%d> video<%d> vs DstCap audio<%d> video<%d>\n",
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
            if (!IsHDConf(m_tConf) || !g_cMcuVcApp.IsSelAccord2Adp())
            {
                bySwitchMode = MODE_NONE;
                ConfLog( FALSE, "[IsSelModeAndCapMatched] capbility match failed: \n\t\tSrcCap video<%d> vs DstCap video<%d>\n",
                     tSimCapSrc.GetVideoMediaType(), tSimCapDst.GetVideoMediaType() );
                return FALSE;
            }
            else
            {
                ConfLog( FALSE, "[IsSelModeAndCapMatched] capbility match loss, need adp: \n\t\tSrcCap video<%d> vs DstCap video<%d>\n",
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
            ConfLog( FALSE, "[IsSelModeAndCapMatched] capbility match failed: \n\t\tSrcCap audio<%d> vs DstCap audio<%d>\n",
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
            if (!IsHDConf(m_tConf) || !g_cMcuVcApp.IsSelAccord2Adp())
            {
                bySwitchMode = ( MODE_BOTH == bySwitchMode ) ? MODE_AUDIO : MODE_NONE;
                ConfLog( FALSE, "[IsSelModeAndCapMatched] capbility match failed: \n\t\tSrcCap VidRes<%d> vs DstCap VidRes<%d>\n",
                    tSimCapSrc.GetVideoResolution(), tSimCapDst.GetVideoResolution() );
            }
            else
            {
                ConfLog( FALSE, "[IsSelModeAndCapMatched] capbility match loss, need adp: \n\t\tSrcCap VidRes<%d> vs DstCap VidRes<%d>\n",
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
        ConfLog( FALSE, "[AdjustKedaMcuAndTaideMuteInfo] ptMt NULL ! \n");
        return;
    }
    
    //上报MCS
    TMtStatus tMtStatus;
    m_ptMtTable->GetMtStatus(ptMt->GetMtId(), &tMtStatus);
    if( TRUE == bDecodeAud ) 
    {
        tMtStatus.SetDecoderMute( bOpenFlag != 0 ); //解码是否静音	        
    }
    else
    {
        tMtStatus.SetCaptureMute( bOpenFlag != 0 );	//采集是否静音(是否哑音)
    }
    m_ptMtTable->SetMtStatus(ptMt->GetMtId(), &tMtStatus);

    MtStatusChange( ptMt->GetMtId(), TRUE );
    
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
		ConfLog( FALSE, "[PolycomSecChnnl] byPolycomMtId.%d ignore it\n", byPolycomMtId );
		return;
	}
	
	TCapSupport tPolyMcuCap;
	BOOL32 bRet = m_ptMtTable->GetMtCapSupport(byPolycomMtId, &tPolyMcuCap);
	if ( !bRet )
	{
		ConfLog( FALSE, "[PolycomSecChnnl] get PolyMt.%d's cap failed\n", byPolycomMtId );
		return;
	}
	TLogicalChannel tLogicChan;
	TMt tPolyMt = m_ptMtTable->GetMt(byPolycomMtId);
	McuMtOpenDoubleStreamChnnl(tPolyMt, tLogicChan, tPolyMcuCap);
	
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
		ConfLog(FALSE, "[SwitchNewVmpToSingleMt] illegal tMt!\n");
		return;
	}

	if (CONF_VMPMODE_NONE == m_tConf.m_tStatus.GetVMPMode())
	{
		ConfLog(FALSE, "[SwitchNewVmpToSingleMt] No VMP is used!\n");
		return;
	}
	
	u8 bySrcChnnl = 0;
	u8 byMtMediaType = 0;
	u8 byMtRes = 0;
	u8 byVmpRes = 0;
	m_cMtRcvGrp.GetMtMediaRes(tMt.GetMtId(), byMtMediaType, byMtRes);
	bySrcChnnl = GetVmpOutChnnlByRes(byMtRes, m_tVmpEqp.GetEqpId(), byMtMediaType);

	if( bySrcChnnl == (u8)~0 )
	{
		ConfLog(FALSE, "[SwitchNewVmpToSingleMt] Mt.%u cannot receive vmp broardcast \
		because unmatched format or resolution!\n");
	}
	else
	{
		g_cMpManager.SetSwitchBridge(m_tVmpEqp, bySrcChnnl, MODE_VIDEO);	//建桥
		StartSwitchToSubMt(m_tVmpEqp, bySrcChnnl, tMt.GetMtId(), MODE_VIDEO, SWITCH_MODE_SELECT);//switchmode 非默认值，是SWITCH_MODE_SELECT
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
	CallLog("[ProcMtMcuOpenLogicChnnlRsp] Mt.%u cannot receive vmp broardcast \
		because unmatched format or resolution!\n");
	}
	else
	{
		g_cMpManager.SetSwitchBridge(m_tVidBrdSrc, bySrcChnnl, MODE_VIDEO);	//建桥
		StartSwitchToSubMt(m_tVidBrdSrc, bySrcChnnl, tMt.GetMtId(), MODE_VIDEO, SWITCH_MODE_SELECT);//switchmode 非默认值，是SWITCH_MODE_SELECT
	}
*/

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
	memcpy(achConfName, cServMsg.GetMsgBody(), min(MAXLEN_CONFNAME, cServMsg.GetMsgBodyLen()));

	cServMsg.SetMsgBody((u8*)&tMt, sizeof(tMt));
	cServMsg.CatMsgBody((u8*)achConfName, strlen(achConfName));
	cServMsg.SetEventId(MCU_VCS_RELEASEMT_REQ);
	SendMsgToAllMcs(MCU_VCS_RELEASEMT_REQ, cServMsg);
	
}


//END OF FILE
