/*****************************************************************************
   模块名      : mcu
   文件名      : mcuvcswitch.cpp
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
#include "evmcumt.h"
#include "evmcueqp.h"
#include "evmp.h"
#include "evmcu.h"
#include "evmcutest.h"
#include "mcuvc.h"
#include "mcsssn.h"
#ifdef _8KH_
	#include "mpssn.h"
#endif
//#include "mcuerrcode.h"
//#include "mpmanager.h"
//#include "mcuutility.h"

/*====================================================================
    函数名      ：StartSwitchToSubMt
    功能        ：将指定终端数据交换到下级直连非次级终端，如果是非直连终端则
				  不进行交换直接返回
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt & tSrc, 源
				  u8 bySrcChnnl,    源的信道号
				  u8 byDstMtId,     目的终端
				  u8 byMode,        码流格式（音频还是视频），缺省为MODE_BOTH 
                  bySwitchMode，    交换方式（缺省是SWITCH_MODE_BROADCAST）
                  bIsSrcBrd，       该源终端是否是广播源
    返回值说明  ：TRUE，如果不交换直接返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/12    1.0         LI Yi         创建
	03/07/23    2.0         胡昌威        修改
	04/03/06    3.0         胡昌威        修改
    07/02/13    4.0         顾振华        修改
====================================================================*/
// BOOL32 CMcuVcInst::StartSwitchToSubMt(TMt   tSrc, 
//                                       u8    bySrcChnnl,
//                                       u8    byDstMtId,
//                                       u8    byMode,
//                                       u8    bySwitchMode,
//                                       BOOL32 bMsgStatus,
//                                       BOOL32 bSndFlowCtrl, BOOL32 bIsSrcBrd,
// 										u16 wSpyPort )
// {
//     TMtStatus tMtStatus;
//     CServMsg  cServMsg;
//     u8 byMediaMode = byMode;
//     BOOL32 bSwitchedAudio = FALSE;
// 
// 	//zjj20100201
// 	//  [11/26/2009 pengjie] Modify 级联多回传支持
// 	TMt tUnlocalSrc = tSrc;
// 	// End
// 
//     tSrc = GetLocalMtFromOtherMcuMt(tSrc);
// 
// 	//检测非KDC终端是否处于哑音状态，是则不交换音频码流	
// 	if( TYPE_MT == tSrc.GetType() && 
// 		MT_MANU_KDC != m_ptMtTable->GetManuId( tSrc.GetMtId() ) && 
// 		m_ptMtTable->IsMtAudioDumb( tSrc.GetMtId() ) )
// 	{
// 		if( byMediaMode == MODE_AUDIO )
// 		{
// 			return TRUE;
// 		}
// 		else if ( byMediaMode == MODE_BOTH )
// 		{
// 			byMediaMode = MODE_VIDEO;
// 		}
// 	}
// 
// 	//检测非KDC终端是否处于静音状态，是则不向他交换音频码流	
// 	if( MT_MANU_KDC != m_ptMtTable->GetManuId( byDstMtId ) && 
// 		m_ptMtTable->IsMtAudioMute( byDstMtId ) )
// 	{
// 		if( byMediaMode == MODE_AUDIO )
// 		{
// 			return TRUE;
// 		}
// 		else if ( byMediaMode == MODE_BOTH )
// 		{
// 			byMediaMode = MODE_VIDEO;
// 		}
// 	}
// 
// 	u32 dwTimeIntervalTicks = 3*OspClkRateGet();
// 
//     BOOL32 bMMcuNeedAdp = FALSE;
// 
// 	TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();
// 	
// 	//只向上级Mcu发spyMt
// 	TMt tDstMt = m_ptMtTable->GetMt( byDstMtId );
// 	if(m_tCascadeMMCU.GetMtId() != 0 && byDstMtId == m_tCascadeMMCU.GetMtId())
// 	{
// 		//////////////////////////////////////////////////////////////////////////
// 		//zjj20100201 多回传补充
// 		u16 wSpyPort1 = SPY_CHANNL_NULL;	
// 		CSendSpy cSendSpy;
// 		wSpyPort1 = SPY_CHANNL_NULL;
// 		if( TYPE_MT == tSrc.GetType() &&
// 			!m_cLocalSpyMana.GetSpyChannlInfo(tSrc, cSendSpy) )
// 		{
// 			u16 wMMcuIdx = GetMcuIdxFromMcuId( tDstMt.GetMtId() );
// 			//m_tConfAllMcuInfo.GetIdxByMcuId( tDstMt.GetMtId(),0,&wMMcuIdx );
// 			//zjj20100329 对上级mcu建交换不是多回传终端，就是回传通道中的终端，如果都不是就直接返回
// 			TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMMcuIdx);//tDstMt.GetMtId());
// 			if ( NULL == ptConfMcInfo ) 
// 			{
// 				return FALSE;
// 			} 
// 			if( !(ptConfMcInfo->m_tSpyMt.GetMtId() == tSrc.GetMtId()&&
// 				  ptConfMcInfo->m_tSpyMt.GetMcuId() == tSrc.GetMcuId())
// 				  )
// 			{
//                 return FALSE;
// 			}			
// 		}	
// 		 //判断spymt到上级是否需要适配
//         TSimCapSet tSrcSimCap = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
//         TSimCapSet tDstSimCap = m_ptMtTable->GetDstSCS(byDstMtId);
//         
//         if (tSrcSimCap.IsNull())
//         {
//             ConfLog(FALSE, "[StartSwitchToSubMt] tSrc.%d primary logic chan unexist!\n", tSrc.GetMtId());
//             return FALSE;
//         }
//         tSrcSimCap.SetVideoMaxBitRate(m_ptMtTable->GetMtSndBitrate(tSrc.GetMtId()));
//         
//         if (tDstSimCap.IsNull())
//         {
//             ConfLog(FALSE, "[StartSwitchToSubMt] tDst.%d back logic chan unexist!\n", byDstMtId);
//             return FALSE;
//         }
//         tDstSimCap.SetVideoMaxBitRate(m_ptMtTable->GetMtReqBitrate(byDstMtId));
//         
//         if (tDstSimCap < tSrcSimCap)
//         {
//             bMMcuNeedAdp = TRUE;
//         }
// 		//////////////////////////////////////////////////////////////////////////
// 		
// 		/*TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tDstMt.GetMtId());
// 		if( ptConfMcInfo != NULL && 
// 			( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
// 		{
// 			if( TYPE_MT == tSrc.GetType() &&
//                 !(ptConfMcInfo->m_tSpyMt.GetMtId() == tSrc.GetMtId()&&
// 				  ptConfMcInfo->m_tSpyMt.GetMcuId() == tSrc.GetMcuId()))
// 			{
//                 return FALSE;
// 			}
// 
//             //判断spymt到上级是否需要适配
//             TSimCapSet tSrcSimCap = m_ptMtTable->GetSrcSCS(ptConfMcInfo->m_tSpyMt.GetMtId());
//             TSimCapSet tDstSimCap = m_ptMtTable->GetDstSCS(byDstMtId);
//             
//             if (tSrcSimCap.IsNull())
//             {
//                 ConfLog(FALSE, "[StartSwitchToSubMt] tSrc.%d primary logic chan unexist!\n", ptConfMcInfo->m_tSpyMt.GetMtId());
//                 return FALSE;
//             }
//             tSrcSimCap.SetVideoMaxBitRate(m_ptMtTable->GetMtSndBitrate(ptConfMcInfo->m_tSpyMt.GetMtId()));
//             
//             if (tDstSimCap.IsNull())
//             {
//                 ConfLog(FALSE, "[StartSwitchToSubMt] tDst.%d back logic chan unexist!\n", byDstMtId);
//                 return FALSE;
//             }
//             tDstSimCap.SetVideoMaxBitRate(m_ptMtTable->GetMtReqBitrate(byDstMtId));
//             
//             if (tDstSimCap < tSrcSimCap)
//             {
//                 bMMcuNeedAdp = TRUE;
//             }
// 		}
// 		//zjj20100201
// 		g_cMpManager.SetSwitchBridge(tSrc, 0, byMediaMode,FALSE,wSpyPort );
// 		*/
// 	}
//     
// 	//只收不发 
// 	TMt TNullMt;
// 	TNullMt.SetNull();
// 	if( byDstMtId == 0 )
// 	{
//         g_cMpManager.StartSwitchToSubMt( tSrc, bySrcChnnl, TNullMt, byMediaMode, bySwitchMode,
// 									FALSE,TRUE
// 									,FALSE,wSpyPort 
// 									);
// 		return TRUE;
// 	}
// 
// 	//只发送终端, 则不用进行交换
// 	m_ptMtTable->GetMtStatus(byDstMtId, &tMtStatus);
// 	if( ( (!tMtStatus.IsReceiveVideo()) && byMediaMode == MODE_VIDEO ) ||
// 		( (!tMtStatus.IsReceiveAudio()) && byMediaMode == MODE_AUDIO ) )
// 	{
// 		return FALSE;
// 	}
// 
//     //目的终端是否在环回
//     if (tMtStatus.IsMediaLoop(MODE_VIDEO))
//     {
//         if (byMediaMode == MODE_VIDEO)
//         {
//             return TRUE;
//         }
//         else if (byMediaMode == MODE_BOTH)
//         {
//             byMediaMode = MODE_AUDIO;
//         }
//     }
//     
//     if (tMtStatus.IsMediaLoop(MODE_AUDIO))
//     {
//         if (byMediaMode == MODE_AUDIO)
//         {
//             return TRUE;
//         }
//         else if (byMediaMode == MODE_BOTH)
//         {
//             byMediaMode = MODE_VIDEO;
//         }
//     }
// 
// 	//对于已借用第一路视频通道传输双流的终端，切换第一路视频源将被拒绝
// 	if( TRUE == JudgeIfSwitchFirstLChannel(tSrc, bySrcChnnl, tDstMt) )
// 	{	
// 		if( byMediaMode == MODE_VIDEO )
// 		{
// 			return TRUE;
// 		}
// 		else if ( byMediaMode == MODE_BOTH )
// 		{
// 			byMediaMode = MODE_AUDIO;
// 		}
// 	}
// 	
// 	//音频不交换给自己, 如果是源与目标均为mcu，仍应进行交换
// 	if( (tSrc.GetMtId() == byDstMtId) && 
// 		(tSrc.GetType() == TYPE_MT) && 
// 		(tSrc.GetMtType() == MT_TYPE_MT) )
// 	{
// 		if( byMediaMode == MODE_AUDIO )
// 		{
// 			StopSwitchToSubMt( byDstMtId, byMediaMode, SWITCH_MODE_BROADCAST, bMsgStatus );
// 			return TRUE;
// 		}
// 		else if ( byMediaMode == MODE_BOTH )
// 		{
// 			StopSwitchToSubMt( byDstMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, bMsgStatus );
// 			byMediaMode = MODE_VIDEO;
// 		}
// 	}
// 
// 	if( m_tConf.m_tStatus.IsMixing() ) 
// 	{
// 		if( byMediaMode == MODE_AUDIO )
// 		{
// 			return FALSE;
// 		}
// 		else if( byMediaMode == MODE_BOTH )
// 		{
// 			byMediaMode = MODE_VIDEO;
// 		}
// 	}
// 
// 	// [pengjie 2010/8/20] 分配回传通道，通知多回传下级建交换
// //	CRecvSpy cRecvSpy;
// //  if(!tUnlocalSrc.IsLocal() && m_cSMcuSpyMana.GetRecvSpy( tUnlocalSrc, cRecvSpy ) )
// //	{
// //		SendMMcuSpyNotify( tUnlocalSrc, MCS_MCU_STARTSWITCHMT_REQ );
// //	}
// 	// End
// 	
// 	//不要把自己的码流交换回去, 如果是源与目标均为mcu，仍应进行交换
// 	if( (tSrc.GetType() == TYPE_MT) &&
// 		(tSrc.GetMtType() == MT_TYPE_MT) && 
// 		(tSrc.GetMtId() == byDstMtId) && 
// 		(m_ptMtTable->GetManuId( byDstMtId ) == MT_MANU_KDC) )
// 	{
// 		if( byMediaMode == MODE_BOTH || byMediaMode == MODE_VIDEO )
// 		{
// 			NotifyMtReceive( tSrc, tDstMt.GetMtId() );
// 		}
// 		
// 		StopSwitchToSubMt( byDstMtId, byMediaMode, SWITCH_MODE_BROADCAST, bMsgStatus );
// 
// 		// guzh [8/31/2006] 无需再设置终端状态，Stop的时候会设置
// 		// m_ptMtTable->SetMtSrc( byDstMtId, ( const TMt * )&tSrc, byMediaMode );			
//         
// 		return TRUE;
// 	}
// 
// 	//lukunpeng 2010/06/02 如果是下级的mcu，并且没有视频源，不要把自己的码流交换给自己
// 	if(tSrc == tDstMt
// 		&& tDstMt.GetMtType() == MT_TYPE_SMCU
// 		&& MODE_AUDIO != byMediaMode
// 		&& tLocalVidBrdSrc.IsNull())
// 	{
// 		//lukunpeng 2010/06/17 由于很多时候对于非科达终端，没有拆交换就重新建交换，
// 		//造成从此处返回后，发现原来的交换没有拆，故在此补拆一下交换
// 		StopSwitchToSubMt( byDstMtId, MODE_VIDEO, SWITCH_MODE_BROADCAST, bMsgStatus );
// 		return TRUE;
// 	}
// 
// 	//lukunpeng 2010/06/29 如果是上级的mcu做下级的发言人，不需要把自己的音频再交换回去
// 	if (tSrc == tDstMt
// 		&& tDstMt.GetMtType() == MT_TYPE_MMCU
// 		&& MODE_VIDEO != byMediaMode)
// 	{
// 		StopSwitchToSubMt( byDstMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, bMsgStatus );
// 		return TRUE;
// 	}
// 
// 	//lukunpeng 2010/07/13 对应本机终端做回传源，并且上级的广播源是本机此终端，不要把音频码流打给自己
// 	//但是在混音的情况下，不过滤，下级所有终端听上级混音器的N模式声音，包括做全局发言人的下级终端
// 	TConfMcInfo *ptConfMcInfo = NULL;
// 	if (!m_tCascadeMMCU.IsNull() && MODE_VIDEO != byMediaMode)
// 	{
// 		ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
// 		
// 		if (ptConfMcInfo != NULL
// 			&& tSrc == m_tCascadeMMCU
// 			&& ptConfMcInfo->m_tSpyMt.GetMtId() == byDstMtId
// 			&& ptConfMcInfo->m_tConfAudioInfo.m_byMixerCount == 1
// 			&& ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId() == byDstMtId
// 			)
// 		{
// 			StopSwitchToSubMt( byDstMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, bMsgStatus );
// 			return TRUE;
// 		}
// 	}
// 	
//     //for h320 mcu cascade select
//     if ( m_ptMtTable->GetMtType(byDstMtId) == MT_TYPE_SMCU && tSrc.GetMtId() == byDstMtId &&
//          !m_tLastSpyMt.IsNull() && 
//          m_tConfAllMtInfo.MtJoinedConf(m_tLastSpyMt.GetMcuId(), m_tLastSpyMt.GetMtId()) )
//     {
//         tSrc = m_tLastSpyMt;
//     }
// 
//     // zbq [08/31/2007] 本函数只处理音频和视频，和双流毫无关系
//     /*
// 	//是否要转发H.239数据，对于双流，并不进行发言人限制，以后发后生效为准
// 	TLogicalChannel tLogicalChannel;
// 	if( tSrc.GetType() == TYPE_MT && 
// 		m_ptMtTable->GetMtLogicChnnl( tSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE ) )
// 	{
// 		//g_cMpManager.StartSwitchToSubMt( tSrc, 1, tDstMt, MODE_VIDEO, SWITCH_MODE_BROADCAST, TRUE );
//         g_cMpManager.StartSwitchToSubMt( tSrc, 0, tDstMt, MODE_SECVIDEO, SWITCH_MODE_BROADCAST, TRUE );
// 		//新的双流接收端加入，不再强制补发关键帧，是否需要关键帧由终端自行申请
// 	}*/
// 
//     //modify bas 2
// 	//终端是否需适配
//     TMt tSwitchSrc;
//     u8  bySwitchSrcChnnl;
//     tSwitchSrc.SetNull();
// 
// 	if (!m_tConf.GetConfAttrb().IsUseAdapter() || SWITCH_MODE_SELECT == bySwitchMode)
// 	{
//         tSwitchSrc = tSrc;
//         bySwitchSrcChnnl = bySrcChnnl;
// 	}
// 	else
// 	{
//         if (MODE_AUDIO != byMediaMode && m_tConf.m_tStatus.IsBrdstVMP())
//         {
//             TSimCapSet tSimCapSet; 
//             u8 byVmpSrcChl = 0;
//             u8 byMediaType = m_tConf.GetMainVideoMediaType();
//             u16 wDstMtReqBitrate = m_ptMtTable->GetMtReqBitrate(byDstMtId);
// 
//             TCapSupport tCap;
// 			/*
//             //先处理vmp双速单格式的情况
//             if (0 != m_tConf.GetSecBitRate() && 
//                 MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType())
//             {
//                 //发第2路
//                 if(wDstMtReqBitrate < m_tConf.GetBitRate())
//                 {
//                     byVmpSrcChl = 1;
//                     if (wDstMtReqBitrate < m_wBasBitrate)
//                     {
//                         ChangeVmpBitRate(wDstMtReqBitrate, 2);
//                     }
//                 }
//                 //发第1路
//                 else
//                 {
//                     byVmpSrcChl = 0;
//                     if (wDstMtReqBitrate < m_wVidBasBitrate)
//                     {
//                         ChangeVmpBitRate(wDstMtReqBitrate, 1);
//                     }
//                 }
// 
//                 //FIXME: 未处理高清VMP双速
//             }
//             //单速双格式或双速双格式
//             else
//             {
//                 tSimCapSet = m_ptMtTable->GetDstSCS(byDstMtId);
//                 if (tSimCapSet.GetVideoMediaType() == byMediaType)
//                 {
//                     byVmpSrcChl = 0;
//                     if (wDstMtReqBitrate < m_wVidBasBitrate)
//                     {
//                         ChangeVmpBitRate(wDstMtReqBitrate, 1);
//                     }
//                 }
//                 else
//                 {
//                     byVmpSrcChl = 1;
//                     if (wDstMtReqBitrate < m_wBasBitrate)
//                     {
//                         ChangeVmpBitRate(wDstMtReqBitrate, 2);
//                     }
//                 }
//             }            
// 			bySwitchSrcChnnl = byVmpSrcChl;*/
// 			// xliang [8/4/2009] 调VMP广播码率,此处调用只有老VMP广播会进去
// 			bySwitchSrcChnnl = AdjustVmpBrdBitRate(&tDstMt);
//             tSwitchSrc = m_tVmpEqp;
//             
//         }
//         else
//         {
//             TSimCapSet tTmpSCS;
//             TSimCapSet tSrcSCS;
//             TSimCapSet tDstSCS;
// 
//             if (MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode)
//             {
// 			    if (tSrc.GetType() == TYPE_MT &&
// 					IsNeedAdapt(tSrc, m_ptMtTable->GetMt(byDstMtId), MODE_AUDIO))
// 			    {
// 					TEqp tBas;
// 					tBas.SetNull();
// 					u8 byChnId  = 0;
// 					u8 byOutNum = 0;
// 					u8 byOutIdx = 0;
// 					//[nizhijun 2010119] 需要比较tmt，而不能比较mtid，不交换给自己
// 					if (tUnlocalSrc == m_ptMtTable->GetMt(byDstMtId))
// 					{
// 						return FALSE;
// 					}
// 					if(!FindBasChn2BrdForMt(byDstMtId, MODE_AUDIO, tBas, byChnId, byOutNum, byOutIdx))
// 					{
// 						ConfLog(FALSE, "[FindBasChn2BrdForMt] FindBasChn2BrdForMt:%d with Mode:%d failed!\n", byDstMtId, MODE_AUDIO);
// 						return FALSE;
// 					}
// 					tBas.SetConfIdx(m_byConfIdx);
// 					tSwitchSrc = tBas;
// 					bySwitchSrcChnnl = byChnId * byOutNum + byOutIdx;
// 			    }
// 				else
// 				{
// 					tSwitchSrc = tSrc;
//                     bySwitchSrcChnnl = bySrcChnnl;
// 				}
// //                 if (tSrc.GetType() == TYPE_MT &&
// //                     IsMtNeedAdapt(ADAPT_TYPE_AUD, byDstMtId, &tSrc))
// //                 {
// //                     if (!m_tConf.m_tStatus.IsAudAdapting())
// //                     {
// //                         tTmpSCS = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
// //                         tSrcSCS.SetAudioMediaType(tTmpSCS.GetAudioMediaType());
// //                         tTmpSCS = m_ptMtTable->GetDstSCS(byDstMtId);
// //                         tDstSCS.SetAudioMediaType(tTmpSCS.GetAudioMediaType());
// //                         if (!StartAdapt(ADAPT_TYPE_AUD, 0, &tDstSCS, &tSrcSCS))
// //                         {
// //                             ConfPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StartSwitchToSubMt %d failed because audio Adapter cann't available!\n", byDstMtId);
// //                             return FALSE;
// //                         }
// //                     }                    
// // 
// //                     ConfPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "IsMtNeedAdapt(ADAPT_TYPE_AUD, %d)\n", byDstMtId);
// //                     tSwitchSrc = m_tAudBasEqp;
// //                     bySwitchSrcChnnl = m_byAudBasChnnl;
// //                 }
// //                 else
// //                 {
// //                     tSwitchSrc = tSrc;
// //                     bySwitchSrcChnnl = bySrcChnnl;
// //                 }
// 
//                 if (MODE_BOTH == byMediaMode)
//                 {
//                     ConfPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "IsMtNeedAdapt(ADAPT_TYPE_AUD, %d, MODE_BOTH)\n", byDstMtId);
// 					//zjj20100201
//                     g_cMpManager.StartSwitchToSubMt(tSwitchSrc, bySwitchSrcChnnl, tDstMt, MODE_AUDIO, bySwitchMode,
// 										FALSE,TRUE,FALSE,wSpyPort
// 										);
//                     bSwitchedAudio = TRUE;
//                 }
//             }
// 
//             if (MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode)
//             {
// 				// [9/20/2010 liuxu] 录像机外设放像时也需要建立适配通道
// 				if ((tSrc.GetType() == TYPE_MT || (TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_RECORDER == tSrc.GetEqpType()))
// 					&&
// 					IsNeedAdapt(tSrc, m_ptMtTable->GetMt(byDstMtId), MODE_VIDEO))
// 				{
// 					TEqp tBas;
// 					tBas.SetNull();
// 					u8 byChnId  = 0;
// 					u8 byOutNum = 0;
// 					u8 byOutIdx = 0;
// 					//[nizhijun 2010119] 需要比较tmt，而不能比较mtid，不交换给自己
// 					if (tUnlocalSrc == m_ptMtTable->GetMt(byDstMtId))
// 					{
// 						return FALSE;
// 					}
// 					if(!FindBasChn2BrdForMt(byDstMtId, MODE_VIDEO, tBas, byChnId, byOutNum, byOutIdx))
// 					{
// 						ConfLog(FALSE, "[FindBasChn2BrdForMt] FindBasChn2BrdForMt:%d with Mode:%d failed!\n",byDstMtId, MODE_VIDEO);
// 						return FALSE;
// 					}
// 					tBas.SetConfIdx(m_byConfIdx);
// 					tSwitchSrc = tBas;
// 					bySwitchSrcChnnl = byChnId * byOutNum + byOutIdx;
// 				}
// 				else
// 				{
// 					tSwitchSrc = tSrc;
//                     bySwitchSrcChnnl = bySrcChnnl;
// 				}
// //                 // 判断高清适配, zgc, 2008-08-08
// //                 TMt tSrcTemp;
// //                 tSrcTemp.SetNull();
// //                 TSimCapSet tSrcSCSTemp;
// //                 if ( IsHDConf( m_tConf ) )
// //                 {
// //                     if (m_cMtRcvGrp.IsMtNeedAdp(byDstMtId) ||
// //                         bMMcuNeedAdp)
// //                     {
// // 						tSwitchSrc = tSrc;
// // 						bySwitchSrcChnnl = bySrcChnnl;
// //                     }
// //                     else
// //                     {
// //                         if (!tLocalVidBrdSrc.IsNull())
// //                         {
// //                             tSwitchSrc = tLocalVidBrdSrc;
// //                             bySwitchSrcChnnl = tLocalVidBrdSrc == m_tPlayEqp ? m_byPlayChnnl : 0;
// //                         }
// //                         else
// //                         {
// //                             //m_tVidBrdSrc为空，又不适配，看默认源（比如：外厂商看自己）
// //                             tSwitchSrc = tSrc;
// //                             bySwitchSrcChnnl = bySrcChnnl;
// //                         }
// //                     }
// //                 }
// //                 else
// //                 if (IsMtNeedAdapt(ADAPT_TYPE_VID, byDstMtId, &tSrc))
// //                 {
// //                     if (!m_tConf.m_tStatus.IsVidAdapting())
// //                     {
// //                         if (IsNeedVidAdapt(tDstSCS, tSrcSCS, wAdaptBitRate, &tSrc))
// //                         {
// //                             //需要考虑是否可能StartAdapt()多次
// //                             if (!StartAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSCS, &tSrcSCS))
// //                             {
// //                                 ConfPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StartSwitchToSubMt %d failed because video Adapter cann't available!\n", byDstMtId);
// //                                 return FALSE;
// //                             }
// //                         }
// //                     }
// //                     ConfPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "IsMtNeedAdapt(ADAPT_TYPE_VID, %d)\n", byDstMtId);
// //                     tSwitchSrc = m_tVidBasEqp;
// //                     bySwitchSrcChnnl = m_byVidBasChnnl;
// //                 }
// //                 else if (IsMtNeedAdapt(ADAPT_TYPE_BR, byDstMtId, &tSrc))
// //                 {
// //                     if (!m_tConf.m_tStatus.IsBrAdapting())
// //                     {
// //                         if (IsNeedBrAdapt(tDstSCS, tSrcSCS, wAdaptBitRate, &tSrc))
// //                         {
// //                             if (!StartAdapt(ADAPT_TYPE_BR, wAdaptBitRate, &tDstSCS, &tSrcSCS))
// //                             {
// //                                 ConfPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StartSwitchToSubMt %d failed because bitrate Adapter cann't available!\n", byDstMtId);
// //                                 return FALSE;
// //                             }
// //                         }
// //                     }
// //                     ConfPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "IsMtNeedAdapt(ADAPT_TYPE_BR, %d)\n", byDstMtId);
// //                     tSwitchSrc = m_tBrBasEqp;
// //                     bySwitchSrcChnnl = m_byBrBasChnnl;
// //                 }
// //                 else
// //                 {
// //                     tSwitchSrc = tSrc;
// //                     bySwitchSrcChnnl = bySrcChnnl;
// //                 }
//             }
//         }        
//     }
// 
//     if (!tSwitchSrc.IsNull())
//     {
//         u8 byTmpMediaMode = byMediaMode;
//         if (MODE_BOTH == byMediaMode && bSwitchedAudio) //避免音频重复交换
//         {
//             byTmpMediaMode = MODE_VIDEO;
//         }
//         
//         //zbq [08/07/2007] BAS建向终端的交换,基于平滑考虑,不再作StartStop操作.
//         BOOL32 bStopBeforeStart = TRUE;
// 		TMt tMtOrgSrc;
// 
// 		//zjl [09/11/30] 获取当前选看终端先前的源，如果与当前源相同，则不拆交换
// 		m_ptMtTable->GetMtSrc(tDstMt.GetMtId(), &tMtOrgSrc, byTmpMediaMode);
// 		//zjl [03/11/2010] zjl 如果当前目的终端的源是bas，并且其先前的源也是bas, 同时出的通道也相同，则不拆交换
// 		TEqp tBas;
// 		u8 byOutChnl = 0;
// 		GetMtRealSrc(tDstMt.GetMtId(), byTmpMediaMode, tBas, byOutChnl);
//         if ( (tSwitchSrc.GetType()    == TYPE_MCUPERI &&
//               tSwitchSrc.GetEqpType() == EQP_TYPE_BAS  &&
// 			  tBas == tSwitchSrc && byOutChnl == bySwitchSrcChnnl) ||
//              (tSwitchSrc.GetType()    == TYPE_MT &&
//               tSwitchSrc.GetEqpType() == MT_TYPE_SMCU && 
// 			  tSwitchSrc == tMtOrgSrc))
//         {
//             bStopBeforeStart = FALSE;
// 			ConfLog( FALSE, "[StartSwitchToSubMt] DstMt<McuId:%d, MtId:%d>'s OrgSrc<McuId:%d,MtId:%d, BasId:%d, OutChn:%d> and Current Src<McuId:%d, MtId:%d>!\n",
// 						tDstMt.GetMcuId(), tDstMt.GetMtId(), tMtOrgSrc.GetMcuId(),
// 						tMtOrgSrc.GetMtId(), tBas.GetEqpId(), byOutChnl, 
// 						tSwitchSrc.GetMcuId(), tSwitchSrc.GetMtId()
// 						);
//         }
//         /*if ( (tSwitchSrc.GetType()    == TYPE_MCUPERI &&
//               tSwitchSrc.GetEqpType() == EQP_TYPE_BAS ) ||
//              (tSwitchSrc.GetType()    == TYPE_MT &&
//               tSwitchSrc.GetEqpType() == MT_TYPE_SMCU && 
// 			  tSwitchSrc == tMtOrgSrc))
//         {
//             bStopBeforeStart = FALSE;
//         }*/
// 
// 		if (!g_cMpManager.StartSwitchToSubMt(tSwitchSrc, bySwitchSrcChnnl, tDstMt, byTmpMediaMode, bySwitchMode,
// 					FALSE, bStopBeforeStart, FALSE,wSpyPort)
// 					)
// 		{
// 			ConfPrint(LOG_LVL_ERROR, MID_MCU_MPMGR,  "StartSwitchToSubMt() failed! Cannot switch to specified sub mt!\n" );
// 			return FALSE;
// 		}
// 
//         if (m_tConf.GetConfAttrb().IsResendLosePack())
//         {  		
// 			if( (MODE_VIDEO == byMode || MODE_BOTH == byMode) && tLocalVidBrdSrc == m_tVmpEqp )
// 			{
// 				TPeriEqpStatus tPeriEqpStatus; 
// 				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
// 				u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
// 				if( byVmpSubType != VMP )
// 				{				
// 					u8 byPrsId = 0;
// 					u8 byPrsChnId = 0;
// 					if (FindPrsChnForSrc( m_tVmpEqp, bySwitchSrcChnnl, byPrsId, byPrsChnId))
// 					{
// 						AddRemovePrsMember(tDstMt.GetMtId(), byPrsId, byPrsChnId, MODE_VIDEO, TRUE);
// 					}	
// 				}				
// 			}
//         }        
//     }
// 
// 	if( !tUnlocalSrc.IsLocal() && m_cSMcuSpyMana.IsMtInSpyMember( tUnlocalSrc, MODE_VIDEO ) )
// 	{
// 		if ( MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode )
//         {
//     		NotifyFastUpdate(tUnlocalSrc, MODE_VIDEO, TRUE);
// 		}
// 	}
// 	//通知终端开始发送 
// 	else if( tSrc.GetType() == TYPE_MT )
// 	{
//         if (bSndFlowCtrl)
//         {
//             NotifyMtSend( tSrc.GetMtId(), byMediaMode, TRUE );
//         }		
// 
//         // zbq [06/25/2007] 建音频交换不请求关键帧
//         if ( MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode )
//         {
// 			NotifyFastUpdate(tSrc, MODE_VIDEO, TRUE);
//         }
// 	}
//     else
//     {
//         //zbq[05/07/2009] 级联回传 尝试主动 发三个关键帧
// //         if ((MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode) &&
// //             tSrc.GetEqpId() == m_tCasdVidBasEqp.GetEqpId() &&
// //             byDstMtId == m_tCascadeMMCU.GetMtId())
// //         {
// //             NotifyFastUpdate(tSrc, 0);
// //             NotifyFastUpdate(tSrc, 1);
// //         }
//     }
// 	
// 	// xliang [6/16/2009] FIXME: 向MPU请求关键帧？
// 	
// 	//设置终端的码流提示标识
// 	TPeriEqpStatus tEqpStatus;
// 	if( tSrc == m_tPlayEqp )
// 	{
// 		g_cMcuVcApp.GetPeriEqpStatus( m_tPlayEqp.GetEqpId(), &tEqpStatus );
// 		cServMsg.SetMsgBody( (u8*)tEqpStatus.GetAlias(), MAXLEN_EQP_ALIAS );
// 		SendMsgToMt( byDstMtId, MCU_MT_MTSOURCENAME_NOTIF, cServMsg );		
// 	}
// 	//如果是VMP,则清空码流提示标识
// 	if( tSrc == m_tVmpEqp )
// 	{
// 		u8 abyVmpAlias[8];
// 		abyVmpAlias[0] = ' ';
// 		abyVmpAlias[1] = '\0';
// 		cServMsg.SetMsgBody( abyVmpAlias, sizeof(abyVmpAlias) );
// 		SendMsgToMt( byDstMtId, MCU_MT_MTSOURCENAME_NOTIF, cServMsg );		
// 	}
// 
// 	//通知终端开始接收
// 	if( byMediaMode == MODE_BOTH || byMediaMode == MODE_VIDEO )
// 	{
// 		NotifyMtReceive( tSrc, tDstMt.GetMtId() );
// 	}
// 
//     if (tSrc.GetType() == TYPE_MCUPERI && 
//         tSrc.GetEqpType() == EQP_TYPE_BAS)
// 	{	
// #ifndef _8KE_
// 		if (g_cMcuAgent.IsEqpBasHD(tSrc.GetEqpId()))
// #endif
// 		{		
// 			//zjj20100422记录真实视频源,不是外设就要记录真实发言人为视频源
// 			if( tLocalVidBrdSrc.GetType() != TYPE_MCUPERI )
// 			{
// 				tSrc = m_tConf.GetSpeaker();
// 				m_ptMtTable->SetMtSrc(byDstMtId, &tSrc, byMediaMode);
// 			}
// 			else
// 			{
// 				m_ptMtTable->SetMtSrc(byDstMtId, &tLocalVidBrdSrc, byMediaMode);
// 			}
// 			MtStatusChange(&tDstMt, TRUE);
// 			return TRUE;	
// 		}
//     }
// 	
// 		
// 	// xsl [5/22/2006]设置终端状态, 这里保存广播源信息，不保存bas信息，因为其他地方会根据源信息做相应处理
// 	BOOL32 bCanSetMtSrc = TRUE;
// 
// 	//zjj20100329 上级mcu的源只能是回传通道中的终端
// 	if( !m_tCascadeMMCU.IsNull() && byDstMtId == m_tCascadeMMCU.GetMtId() )
// 	{
// 		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(tDstMt.GetMtId()));
// 		if( NULL != ptConfMcInfo &&
// 			tSrc.GetMtId() != ptConfMcInfo->m_tSpyMt.GetMtId() &&
// 			tSrc.GetMcuId() != ptConfMcInfo->m_tSpyMt.GetMcuId()
// 			)
// 		{
// 			bCanSetMtSrc = FALSE;
// 		}
// 	}
// 	
// 	if( bCanSetMtSrc )
// 	{
// 		TMt tRealMt;
// 		tRealMt.SetNull();
// 		//lukunpeng 2010/07/01 多回传要设置真正的源
// 		m_ptMtTable->SetMtSrc( byDstMtId, &tUnlocalSrc, byMediaMode );	
// 	}
// 
// 	//过滤目标终端为上级mcu的情况
// 	m_ptMtTable->GetMtStatus( byDstMtId, &tMtStatus );
// 	if( tSrc.GetType() == TYPE_MT && tSrc.GetMtId() != byDstMtId && 
// 		(m_tCascadeMMCU.GetMtId() == 0 || byDstMtId != m_tCascadeMMCU.GetMtId()) )
// 	{
// 		TMtStatus tSrcMtStatus;
// 		u8 byAddSelByMcsMode = MODE_NONE;
// 		m_ptMtTable->GetMtStatus( tSrc.GetMtId(), &tSrcMtStatus );
// 
// 		//lukunpeng 2010/07/01 对于单回传，必须匹配local化终端，对于多回传，必须匹配真实源
// 		TMt tVidBrdSrc = tLocalVidBrdSrc;
// 		TMt tAudBrdSrc = GetLocalAudBrdSrc();
// 
// 		// [pengjie 2010/7/29] 这里源不能被覆盖tSrc = tUnlocalSrc
// 		TMt tMtSrc = tSrc;
// 		if( wSpyPort != SPY_CHANNL_NULL)
// 		{
// 			//tSrc = tUnlocalSrc;
// 			tMtSrc = tUnlocalSrc;
// 
// 			tVidBrdSrc = GetVidBrdSrc();
// 			tAudBrdSrc = GetAudBrdSrc();
// 		}
// 
// 	
// 		//[nizhijun 2010/11/30]IsLocalAndSMcuSupMultSpy(tUnlocalSrc.GetMcuId()),多回传才支持单回传不支持
// 		//[nizhijun 2010/11/17] 加入选看源是下级MCU的情况
//         //zbq[12/06/2007] VMP点名时序问题
//         if( (  (!(tMtSrc == tVidBrdSrc)) || (tMtSrc == tVidBrdSrc && 
//                                            ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() &&
//                                            m_tRollCaller == tVidBrdSrc)
// 										   || (tMtSrc == tVidBrdSrc &&                                            
// 											!tMtStatus.GetSelectMt(byMediaMode).IsNull()
// 										   )  ) &&
//                                            tSrcMtStatus.IsSendVideo() && 
//             ( MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode ) )
// 		{
// 
// 			byAddSelByMcsMode = MODE_VIDEO;
// 		}
// 		
// 		//[nizhijun 2010/11/30]IsLocalAndSMcuSupMultSpy(tUnlocalSrc.GetMcuId()),多回传才支持,单回传不支持
// 		if( ( !(tMtSrc == tAudBrdSrc) 
// 			  || ( tMtSrc == tVidBrdSrc &&
// 				   !tMtStatus.GetSelectMt(byMediaMode).IsNull()
// 				  )  )	
// 			&& tSrcMtStatus.IsSendAudio() && 
// 			( MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode ) )
// 		{
// 			if( MODE_VIDEO == byAddSelByMcsMode )
// 			{
// 				byAddSelByMcsMode = MODE_BOTH;
// 			}
// 			else
// 			{
// 				byAddSelByMcsMode = MODE_AUDIO;
// 			}
// 		}
// 
// 		if( MODE_NONE != byAddSelByMcsMode )
// 		{
// 			tMtStatus.AddSelByMcsMode( byMediaMode );
// 		}
// 		else
// 		{
// 			tMtStatus.RemoveSelByMcsMode( byMediaMode );
// 			// 清空上次选看终端	
// 			TMt tNullMt;
// 			tNullMt.SetNull();
// 			tMtStatus.SetSelectMt(tNullMt, byMediaMode);
// 		}
// 	}
// 	else
// 	{
// 		tMtStatus.RemoveSelByMcsMode( byMediaMode );
// 		// 清空上次选看终端	
// 		TMt tNullMt;
// 		tNullMt.SetNull();
// 		tMtStatus.SetSelectMt(tNullMt, byMediaMode);
// 	}
// 	m_ptMtTable->SetMtStatus( byDstMtId, &tMtStatus );
// 	
// 	if( bMsgStatus )
// 	{
//         MtStatusChange( &tDstMt, TRUE );
// 	}
// 
// 	//若是选看，按目的终端的需要调节发送终端的码虑
// 	if( tSrc.GetType() == TYPE_MT && bySwitchMode == SWITCH_MODE_SELECT && 
// 		( byMediaMode == MODE_VIDEO || byMediaMode == MODE_BOTH ) && 
// 	    tSrc.GetMtId() != tLocalVidBrdSrc.GetMtId() )
// 	{
// 		u16 wMtBitRate = m_ptMtTable->GetMtReqBitrate( byDstMtId );
// 		AdjustMtVideoSrcBR( byDstMtId, wMtBitRate );
// 	}
// 
// 	return TRUE;
// }

/*====================================================================
    函数名      StartSwitchToAll
    功能        ：将指定终端数据交换到下级直连非次级终端，如果是非直连终端则
				  不进行交换直接返回
    算法实现    ：
    引用全局变量：
    输入参数说明：TMt &tSrc 源终端/外设
	              const u8 bySwitchGrpNum 该源有几个接收组（bas可能有多个）
				  TSwitchGrp *pSwitchGrp 各个接收组
				  u8    byMode // 音/视频模式
				  u8 bySwitchMode // 交换模式（选看/广播）
				  BOOL32 bMsgStatus // 是否发送终端状态改变消息
				  BOOL32 bSndFlowCtrl 是否通知源开始发码流
				  u16 wSpyStartPort // 多回传端口，回传源
				  BOOL32 bInnerSelect 是否为内部选看，true不需设置mcsdragmode
    返回值说明  ：TRUE，如果不交换直接返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/12    1.0         LI Yi         创建
	03/07/23    2.0         胡昌威        修改
	04/03/06    3.0         胡昌威        修改
    07/02/13    4.0         顾振华        修改
	20110505    4.6         pengjie       批量建交换支持
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchToAll( TMt tSrc, 
										   const u8 bySwitchGrpNum,
										   TSwitchGrp *pSwitchGrp,
										   u8 byMode,
										   u8 bySwitchMode,
										   BOOL32 bMsgStatus,
										   BOOL32 bSndFlowCtrl,
										   u16 wSpyStartPort,
										   BOOL32 bInnerSelect)
{
	
	if( bySwitchGrpNum == 0 || pSwitchGrp == NULL)
	{
		return FALSE;
	}

	// [8/5/2011 liuxu] 用CSwitchGrpAdpt做一次转换, 替换pSwitchGrp的mtlist, 防止以下操作更改mtlist值,
	// 从而影响调用StartSwitchToAll的接口	
	CSwitchGrpAdpt cSwitchGrpAdpt;
	cSwitchGrpAdpt.Convert(bySwitchGrpNum, pSwitchGrp);

    TMtStatus tMtStatus;
    CServMsg  cServMsg;
    u8 byMediaMode = byMode;
    BOOL32 bSwitchedAudio = FALSE;
	
	//zjj20100201
	//  [11/26/2009 pengjie] Modify 级联多回传支持
	TMt tUnlocalSrc = tSrc;
	// End
	
    tSrc = GetLocalMtFromOtherMcuMt(tSrc);
	u8 byLoop = 0;
	u8 byMtNum = 0;
	//检测非KDC终端是否处于哑音状态，是则不交换音频码流	
	if( TYPE_MT == tSrc.GetType() && 
		MT_MANU_KDC != m_ptMtTable->GetManuId( tSrc.GetMtId() ) && 
		m_ptMtTable->IsMtAudioDumb( tSrc.GetMtId() ) )
	{
		if( byMediaMode == MODE_AUDIO )
		{
			//zhouyiliang 20121116 非kdc终端处于哑音，不建音频交换，但是mcsdragmod要刷上去
			if(bySwitchMode == SWITCH_MODE_SELECT)
			{
				for( byLoop = 0; byLoop < bySwitchGrpNum; byLoop++ )
				{
					TMt *pDstMt = pSwitchGrp[byLoop].GetDstMt();
					for( byMtNum = 0; byMtNum < pSwitchGrp[byLoop].GetDstMtNum(); byMtNum++ )
					{
						u8 byDstMtId = pDstMt[byMtNum].GetMtId();
						AddRemoveMcsDragMod(tUnlocalSrc,byDstMtId,byMediaMode,bySwitchMode,wSpyStartPort,bMsgStatus,bInnerSelect);
					
						
					}
				}
			}
		
		
			return TRUE;
		}
		else if ( byMediaMode == MODE_BOTH )
		{
			byMediaMode = MODE_VIDEO;
		}
	}
	
	
	u32 dwTimeIntervalTicks = 3*OspClkRateGet();
	
	TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();
	

	for( byLoop = 0; byLoop < bySwitchGrpNum; byLoop++ )
	{
		for( byMtNum = 0; byMtNum < pSwitchGrp[byLoop].GetDstMtNum(); byMtNum++ )
		{
			TMt *pDstMt = pSwitchGrp[byLoop].GetDstMt();
			u8 byDstMtId = pDstMt[byMtNum].GetMtId();
			
			//只向上级Mcu发spyMt
			TMt tDstMt = m_ptMtTable->GetMt( byDstMtId );

			if(m_tCascadeMMCU.GetMtId() != 0 && byDstMtId == m_tCascadeMMCU.GetMtId())
			{
				// 指定第三级与会终端为发言人或会议轮询第三级终端时忽略建到上级MCU的交换
				if (SWITCH_MODE_BROADCAST == bySwitchMode && byMode != MODE_SECVIDEO)
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[StartSwitchToAll] ignore MMcu's %d brd switch!\n", byMode);
					pDstMt[byMtNum].SetNull();
					continue;
				}

				if(TYPE_MT == tSrc.GetType() /*&& !m_cLocalSpyMana.GetSpyChannlInfo(tSrc, cSendSpy)*/)
				{
					TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(tDstMt.GetMtId()));
					if ( NULL == ptConfMcInfo ) 
					{
						pDstMt[byMtNum].SetNull();
						continue;
					} 
					if( !(ptConfMcInfo->m_tSpyMt == tSrc) )
					{
						pDstMt[byMtNum].SetNull();
						continue;
					}             
				}
                //songkun,20110630,级联时，下级开画面合成不再建立到上级MCU的交换
                if(TYPE_MCUPERI == tSrc.GetType())
                {
                    if(tSrc.GetEqpType() == EQP_TYPE_VMP)
                    {
                        pDstMt[byMtNum].SetNull();
                        continue;
                    }
                }
			}

			//双选看不交换到下级
			if (MODE_VIDEO2SECOND == byMode && tDstMt.GetMtType() == MT_TYPE_SMCU)
			{
				pDstMt[byMtNum].SetNull();
				continue;
			}
			
			//检测非KDC终端是否处于静音状态，是则不向他交换音频码流	
			if( MT_MANU_KDC != m_ptMtTable->GetManuId( byDstMtId ) && 
				m_ptMtTable->IsMtAudioMute( byDstMtId ) )
			{
				if( byMediaMode == MODE_AUDIO )
				{
					//zhouyiliang 20121116 非KDC终端是否处于静音状态，不向他交换音频码流，但是mcsdragmod要刷上去
					if(bySwitchMode == SWITCH_MODE_SELECT)
					{
						AddRemoveMcsDragMod(tUnlocalSrc,byDstMtId,byMediaMode,bySwitchMode,wSpyStartPort,bMsgStatus,bInnerSelect);
					
					}
					pDstMt[byMtNum].SetNull();
					continue;
				}
			}
			
			//只发送终端, 则不用进行交换
			m_ptMtTable->GetMtStatus(byDstMtId, &tMtStatus);
			if( ( (!tMtStatus.IsReceiveVideo()) && byMediaMode == MODE_VIDEO ) ||
				( (!tMtStatus.IsReceiveAudio()) && byMediaMode == MODE_AUDIO ) )
			{
				pDstMt[byMtNum].SetNull();
				continue;
			}
			
			//目的终端是否在环回
			if (tMtStatus.IsMediaLoop(MODE_VIDEO))
			{
				if (byMediaMode == MODE_VIDEO)
				{
					pDstMt[byMtNum].SetNull();
					continue;
				}
			}
			
			if (tMtStatus.IsMediaLoop(MODE_AUDIO))
			{
				if (byMediaMode == MODE_AUDIO)
				{
					pDstMt[byMtNum].SetNull();
					continue;
				}
			}
			
			//对于已借用第一路视频通道传输双流的终端，切换第一路视频源将被拒绝
			if(MODE_VIDEO2SECOND != byMode && JudgeIfSwitchFirstLChannel(tSrc, pSwitchGrp[byLoop].GetSrcChnl(), tDstMt) )
			{	
				if( byMediaMode == MODE_VIDEO )
				{
					pDstMt[byMtNum].SetNull();
					continue;
				}
			}
			
			//音频不交换给自己, 如果是源与目标均为mcu，仍应进行交换
			if( (tSrc.GetMtId() == byDstMtId) && 
				(tSrc.GetType() == TYPE_MT) && 
				(tSrc.GetMtType() == MT_TYPE_MT) )
			{
				if( byMediaMode == MODE_AUDIO )
				{
					//zjl 20110510 StopSwitchToSubMt 接口重载替换
					//StopSwitchToSubMt( byDstMtId, byMediaMode, SWITCH_MODE_BROADCAST, bMsgStatus );
					StopSwitchToSubMt(1, &pDstMt[byMtNum], byMediaMode, bMsgStatus);

					pDstMt[byMtNum].SetNull();
					continue;
				}
			}
			
			if( m_tConf.m_tStatus.IsMixing() && 
				!(tDstMt == m_tConf.GetChairman())
				&& !(tSrc  == m_tMixEqp )
               )
			{
				if( byMediaMode == MODE_AUDIO )
				{
					pDstMt[byMtNum].SetNull();
					continue;
				}
			}
			
			//不要把自己的码流交换回去, 如果是源与目标均为mcu，仍应进行交换
			if( (tSrc.GetType() == TYPE_MT) &&
				(tSrc.GetMtType() == MT_TYPE_MT) && 
				(tSrc.GetMtId() == byDstMtId) && 
				( m_ptMtTable->GetManuId( byDstMtId ) == MT_MANU_KDC || 
					m_ptMtTable->GetManuId( byDstMtId ) == MT_MANU_KDCMCU ) 
				)
			{
				//zjj20110330 在StopSwitchToSubMt函数参数中，byselself默认为true，所以该函数会让终端看自己，并且有一系列保护，所以这里注去
				/*if( byMediaMode == MODE_BOTH || byMediaMode == MODE_VIDEO )
				{
				NotifyMtReceive( tSrc, tDstMt.GetMtId() );
			}*/
				//zjl 20110510 StopSwitchToSubMt 接口重载替换
				//StopSwitchToSubMt( byDstMtId, byMediaMode, SWITCH_MODE_BROADCAST, bMsgStatus );
				StopSwitchToSubMt(1, &pDstMt[byMtNum], byMediaMode, bMsgStatus);
				
				// guzh [8/31/2006] 无需再设置终端状态，Stop的时候会设置
				// m_ptMtTable->SetMtSrc( byDstMtId, ( const TMt * )&tSrc, byMediaMode );			
				
				pDstMt[byMtNum].SetNull();
				continue;
			}
			
			//lukunpeng 2010/06/02 如果是下级的mcu，并且没有视频源，不要把自己的码流交换给自己
			if(tSrc == tDstMt
				&& (tDstMt.GetMtType() == MT_TYPE_SMCU || tDstMt.GetMtType() == MT_TYPE_MMCU)
				&& MODE_AUDIO != byMediaMode
				&& tLocalVidBrdSrc.IsNull())
			{
				//zjl 20110510 StopSwitchToSubMt 接口重载替换
				//lukunpeng 2010/06/17 由于很多时候对于非科达终端，没有拆交换就重新建交换，
				//造成从此处返回后，发现原来的交换没有拆，故在此补拆一下交换
				//StopSwitchToSubMt( byDstMtId, MODE_VIDEO, SWITCH_MODE_BROADCAST, bMsgStatus );
				StopSwitchToSubMt(1, &pDstMt[byMtNum], MODE_VIDEO, bMsgStatus);
				pDstMt[byMtNum].SetNull();
				continue;
			}
			
			//lukunpeng 2010/06/29 如果是上级的mcu做下级的发言人，不需要把自己的音频再交换回去
			if (tSrc == tDstMt
				&& tDstMt.GetMtType() == MT_TYPE_MMCU
				&& MODE_VIDEO != byMediaMode)
			{
				//zjl 20110510 StopSwitchToSubMt 接口重载替换
				//StopSwitchToSubMt( byDstMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, bMsgStatus );
				StopSwitchToSubMt(1,  &pDstMt[byMtNum], MODE_AUDIO, bMsgStatus);

				pDstMt[byMtNum].SetNull();
				continue;
			}
			
			//lukunpeng 2010/07/13 对应本机终端做回传源，并且上级的广播源是本机此终端，不要把音频码流打给自己
			//但是在混音的情况下，不过滤，下级所有终端听上级混音器的N模式声音，包括做全局发言人的下级终端
			TConfMcInfo *ptConfMcInfo = NULL;
			if (!m_tCascadeMMCU.IsNull() && MODE_VIDEO != byMediaMode)
			{
				ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
				
				if (ptConfMcInfo != NULL
					&& tSrc == m_tCascadeMMCU
					&& ptConfMcInfo->m_tSpyMt.GetMtId() == byDstMtId
					&& ptConfMcInfo->m_tConfAudioInfo.m_byMixerCount == 1
					&& ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.IsMcuIdLocal()
					&& ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId() == byDstMtId
					)
				{
					//zjl 20110510 StopSwitchToSubMt 接口重载替换
					//StopSwitchToSubMt( byDstMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, bMsgStatus );
					StopSwitchToSubMt(1, &pDstMt[byMtNum], MODE_AUDIO, bMsgStatus);
					pDstMt[byMtNum].SetNull();
					continue;
				}
			}
			
			//for h320 mcu cascade select
			if ( m_ptMtTable->GetMtType(byDstMtId) == MT_TYPE_SMCU && tSrc.GetMtId() == byDstMtId &&
				!m_tLastSpyMt.IsNull() && 
				m_tConfAllMtInfo.MtJoinedConf(m_tLastSpyMt.GetMcuId(), m_tLastSpyMt.GetMtId()) )
			{
				tSrc = m_tLastSpyMt;
			}
			
		}
	}
	
	if( !g_cMpManager.StartSwitchToAll( tSrc, bySwitchGrpNum, pSwitchGrp, byMode, bySwitchMode, TRUE, wSpyStartPort ) )
	{
		ConfPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitch failed! Cannot switch to specified sub mt!\n" );
		return FALSE;
	}
	
	//////////////////////////////
	if( !tUnlocalSrc.IsLocal() && m_cSMcuSpyMana.IsMtInSpyMember( tUnlocalSrc, MODE_VIDEO ) )
	{
		if ( MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode || MODE_VIDEO2SECOND == byMediaMode)
        {
			NotifyFastUpdate(tUnlocalSrc, MODE_VIDEO);
		}
	}
	//通知终端开始发送 
	else if( tSrc.GetType() == TYPE_MT )
	{
        if (bSndFlowCtrl)
        {
            NotifyMtSend( tSrc.GetMtId(), byMediaMode, TRUE );
        }		
		
        // zbq [06/25/2007] 建音频交换不请求关键帧
        if ( MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode || MODE_VIDEO2SECOND == byMediaMode)
        {
			// 2011-8-5 add by pgf : 取消发言人后，泰德终端掉线
			if ( MT_MANU_TAIDE == m_ptMtTable->GetManuId(tSrc.GetMtId()))
			{
				// 不请求关键帧
			}
			else
			// 2011-8-5 add end
			{

				NotifyFastUpdate(tSrc, MODE_VIDEO);
			}			
        }
	}
    else
    {
		//
    }
	
	for( byLoop = 0; byLoop < bySwitchGrpNum; byLoop++ )
	{
		for( byMtNum = 0; byMtNum < pSwitchGrp[byLoop].GetDstMtNum(); byMtNum++ )
		{
			TMt *pDstMtId = pSwitchGrp[byLoop].GetDstMt();
			u8 byDstMtId = pDstMtId[byMtNum].GetMtId();
			TMt tDstMt = m_ptMtTable->GetMt( byDstMtId );
			////////////////////////
			
			//设置终端的码流提示标识
			TPeriEqpStatus tEqpStatus;
			if( tSrc == m_tPlayEqp )
			{
				g_cMcuVcApp.GetPeriEqpStatus( m_tPlayEqp.GetEqpId(), &tEqpStatus );
				cServMsg.SetMsgBody( (u8*)tEqpStatus.GetAlias(), MAXLEN_EQP_ALIAS );
				SendMsgToMt( byDstMtId, MCU_MT_MTSOURCENAME_NOTIF, cServMsg );		
			}
			
			//如果是VMP,则清空码流提示标识
			if( tSrc.GetEqpType() == EQP_TYPE_VMP )
			{
				u8 abyVmpAlias[8];
				abyVmpAlias[0] = ' ';
				abyVmpAlias[1] = '\0';
				cServMsg.SetMsgBody( abyVmpAlias, sizeof(abyVmpAlias) );
				SendMsgToMt( byDstMtId, MCU_MT_MTSOURCENAME_NOTIF, cServMsg );		
			}   
			
			//通知终端开始接收
			if( byMediaMode == MODE_BOTH || byMediaMode == MODE_VIDEO )
			{
				NotifyMtReceive( tSrc, tDstMt.GetMtId() );
			}
			
			//级联会议会议轮询第三级终端，此时不能将“第三级被轮询终端(第二级会议发言人)”指定为上级MCU的音/视频源
			if ( SWITCH_MODE_BROADCAST == bySwitchMode &&
				 !m_tCascadeMMCU.IsNull() &&
				 m_tCascadeMMCU == tDstMt)
			{
				continue;
			}

			if (tSrc.GetType() == TYPE_MCUPERI && 
				tSrc.GetEqpType() == EQP_TYPE_BAS)
			{	
				
				TBasOutInfo tTempOutInfo;
				BOOL32  bFindBasChn = FALSE;
				if (  SWITCH_MODE_BROADCAST == bySwitchMode )
				{
					 bFindBasChn = FindBasChn2BrdForMt(byDstMtId,byMediaMode,tTempOutInfo);
					 if ( bFindBasChn )
					 {
						 TMt tTempRealSrc = GetBasChnSrc(tTempOutInfo.m_tBasEqp,tTempOutInfo.m_byChnId);
						 m_ptMtTable->SetMtSrc(byDstMtId, &tTempRealSrc, byMediaMode);	
						 ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_MPMGR,"[StartSwitchToAll]set mt:%d src:%d-%d\n",
									byDstMtId,tTempRealSrc.GetMcuId(), tTempRealSrc.GetMtId()
								  );
					 }
					 else
					 {
						 ConfPrint(LOG_LVL_ERROR,MID_MCU_MPMGR,"[StartSwitchToAll]FindBasChn2BrdForMt for mt:%d failed!\n",byDstMtId);
					 }			 
				}
				else
				{
					ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_MPMGR,"[StartSwitchToAll]bySwitchMode:%d,if it's sel-status,no need to SetMtSrc\n",bySwitchMode);
				}
// 				//zjj20100422记录真实视频源,不是外设就要记录真实发言人为视频源
// 				if( tLocalVidBrdSrc.GetType() != TYPE_MCUPERI )
// 				{
// 					TMt tTempSrc = m_tConf.GetSpeaker();
// 					
// 					if ( tTempSrc.IsNull() )
// 					{
// 						tTempSrc = tLocalVidBrdSrc;
// 					}
// 					m_ptMtTable->SetMtSrc(byDstMtId, &tTempSrc, byMediaMode);
// 				}
// 				else
// 				{
// 					m_ptMtTable->SetMtSrc(byDstMtId, &tLocalVidBrdSrc, byMediaMode);
// 				}
				MtStatusChange(&pDstMtId[byMtNum], TRUE);
				continue;	
			}
			
			// xsl [5/22/2006]设置终端状态, 这里保存广播源信息，不保存bas信息，因为其他地方会根据源信息做相应处理
			BOOL32 bCanSetMtSrc = TRUE;
			
			//zjj20100329 上级mcu的源只能是回传通道中的终端
			if( !m_tCascadeMMCU.IsNull() && byDstMtId == m_tCascadeMMCU.GetMtId() )
			{
				TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(tDstMt.GetMtId()));
				if( NULL != ptConfMcInfo &&
					tSrc.GetMtId() != ptConfMcInfo->m_tSpyMt.GetMtId() &&
					tSrc.GetMcuId() != ptConfMcInfo->m_tSpyMt.GetMcuId()
					)
				{
					bCanSetMtSrc = FALSE;
				}
			}

			if (MODE_VIDEO2SECOND == byMediaMode)
			{
				bCanSetMtSrc = FALSE;
			}
			
			if( bCanSetMtSrc )
			{
				TMt tRealMt;
				tRealMt.SetNull();
				//lukunpeng 2010/07/01 多回传要设置真正的源
				m_ptMtTable->SetMtSrc( byDstMtId, &tUnlocalSrc, byMediaMode );	
			}
			
			AddRemoveMcsDragMod(tUnlocalSrc,byDstMtId,byMediaMode,bySwitchMode,wSpyStartPort,bMsgStatus,bInnerSelect);
	
			
			//若是选看，按目的终端的需要调节发送终端的码虑
			if( tSrc.GetType() == TYPE_MT && bySwitchMode == SWITCH_MODE_SELECT && 
				( byMediaMode == MODE_VIDEO || byMediaMode == MODE_BOTH ) && 
				tSrc.GetMtId() != tLocalVidBrdSrc.GetMtId() )
			{
				u16 wMtBitRate = m_ptMtTable->GetMtReqBitrate( byDstMtId );
				AdjustMtVideoSrcBR( byDstMtId, wMtBitRate );
			}
			
		}
	}
	
	return TRUE;
}


/*====================================================================
    函数名      AddRemoveMcsDragMod
    功能        ：停止将数据交换到直连下级非次级终端
    算法实现    ：
    引用全局变量：
    输入参数说明：tUnlocalSrc:非local化的srcmt
				  u8 byDstMtId, 目的终端号
				  u8 byMediaMode，选看的模式
				  u8 bySwitchMode，是选看还是广播模式
				  u16 wSpyStartPort，多回传端口
				  BOOL32 bMsgStatus,是否状态通知mcs
				  BOOL32 bInnerSelect, 是否是内部选看
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    16/11/2012    4.7       zhouyiliang         创建
====================================================================*/
void CMcuVcInst::AddRemoveMcsDragMod(const TMt& tUnlocalSrc, const u8 &byDstMtId, const u8& byMediaMode, u8 bySwitchMode /* = SWITCH_MODE_BROADCAST */, u16 wSpyStartPort /* = SPY_CHANNL_NULL */, BOOL32 bMsgStatus,BOOL32 bInnerSelect /* = TRUE */)
{
	TMtStatus tMtStatus;
	m_ptMtTable->GetMtStatus( byDstMtId, &tMtStatus );
	TMt tSrc = GetLocalMtFromOtherMcuMt(tUnlocalSrc);
	TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();
	if (bInnerSelect)
	{
		// 主席轮询内部选看不设dragmode,但也不走else清选看源
	}
	else if( /*!bInnerSelect &&*/ tSrc.GetType() == TYPE_MT && tSrc.GetMtId() != byDstMtId && 
		(m_tCascadeMMCU.GetMtId() == 0 || byDstMtId != m_tCascadeMMCU.GetMtId()) )
	{
		TMtStatus tSrcMtStatus;
		u8 byAddSelByMcsMode = MODE_NONE;
		m_ptMtTable->GetMtStatus( tSrc.GetMtId(), &tSrcMtStatus );
		
		//lukunpeng 2010/07/01 对于单回传，必须匹配local化终端，对于多回传，必须匹配真实源
		TMt tVidBrdSrc = tLocalVidBrdSrc;
		TMt tAudBrdSrc = GetLocalAudBrdSrc();
		
		// [pengjie 2010/7/29] 这里源不能被覆盖tSrc = tUnlocalSrc
		TMt tMtSrc = tSrc;
		if( wSpyStartPort != SPY_CHANNL_NULL)
		{
			tMtSrc = tUnlocalSrc;		
			tVidBrdSrc = GetVidBrdSrc();
			tAudBrdSrc = GetAudBrdSrc();
		}
		
		//[nizhijun 2010/11/30]IsLocalAndSMcuSupMultSpy(tUnlocalSrc.GetMcuId()),多回传才支持单回传不支持
		//[nizhijun 2010/11/17] 加入选看源是下级MCU的情况
		//zbq[12/06/2007] VMP点名时序问题
		if( (  (!(tMtSrc == tVidBrdSrc)) || (tMtSrc == tVidBrdSrc && 
			ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() &&
			m_tRollCaller == tVidBrdSrc)
			|| (tMtSrc == tVidBrdSrc &&                                            
			!tMtStatus.GetSelectMt(byMediaMode).IsNull()
			)  ) &&
			tSrcMtStatus.IsSendVideo() && 
			( MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode ))
		{
			
			byAddSelByMcsMode = MODE_VIDEO;
		}
		
		//[nizhijun 2010/11/30]IsLocalAndSMcuSupMultSpy(tUnlocalSrc.GetMcuId()),多回传才支持,单回传不支持
		if( ( !(tMtSrc == tAudBrdSrc)
			|| ( tMtSrc == tVidBrdSrc &&
			!tMtStatus.GetSelectMt(byMediaMode).IsNull()
			)  )	
			&& tSrcMtStatus.IsSendAudio() && 
			( MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode ) &&
			bySwitchMode == SWITCH_MODE_SELECT)
		{
			if( MODE_VIDEO == byAddSelByMcsMode )
			{
				byAddSelByMcsMode = MODE_BOTH;
			}
			else
			{
				byAddSelByMcsMode = MODE_AUDIO;
			}
		}
		
		if (MODE_VIDEO2SECOND == byMediaMode)
		{
			if (!(tMtSrc == GetSecVidBrdSrc()) || !(m_ptMtTable->GetMtSelMtByMode(byDstMtId, MODE_VIDEO2SECOND).IsNull()))
			{
				byAddSelByMcsMode = MODE_VIDEO2SECOND;
			}
		}
		
		if( MODE_NONE != byAddSelByMcsMode )
		{
			tMtStatus.AddSelByMcsMode( byMediaMode );
			m_ptMtTable->SetMtStatus( byDstMtId, &tMtStatus );
		}
		else
		{
			m_ptMtTable->RemoveMtSelMtByMode(byDstMtId, byMediaMode);
		}
	}
	else if(EQP_TYPE_VMP == tSrc.GetEqpType())
	{
		//20110623 zjl vmp非广播时，主席终端选看vmp会设置主席终端选看源为vmp，走else选看源又被清空，
		//导致停选看vmp时无法正确标识会议主席是否看vmp，无法告知终端当前状态，rtcp交换需要知道源信息，因此也无法拆除
		//所以这里暂时不做处理
	}
	else
	{
		m_ptMtTable->RemoveMtSelMtByMode(byDstMtId, byMediaMode);
	}


	if( bMsgStatus )
	{
		TMt tDstMt = m_ptMtTable->GetMt(byDstMtId);
		if (MODE_VIDEO2SECOND == byMediaMode)
		{
			MtSecSelSrcChange(tDstMt);
		}	
		MtStatusChange(&tDstMt, TRUE);
	}
}

/*====================================================================
    函数名      ：StopSwitchToSubMt
    功能        ：停止将数据交换到直连下级非次级终端
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byDstMtId, 目的终端号
				  BOOL32 bMsg, 是否发通知接收信息，缺省为TRUE
				  u8 byMode, 交换模式，缺省为MODE_BOTH
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/11    1.0         LI Yi         创建
====================================================================*/
// void CMcuVcInst::StopSwitchToSubMt( u8 byDstMtId, u8 byMode,u8 bySwitchMode,BOOL32 bMsgStatus, BOOL32 bStopSelByMcs /* = TRUE*/, BOOL32 bSelSelf,u16 wSpyPort )
// {
// 	CServMsg	cServMsg;
// 	TMt    TMtNull;
// 	TMtStatus   tMtStatus;
// 
// 	TMtNull.SetNull();
// 	TMt tDstMt = m_ptMtTable->GetMt( byDstMtId );
//     m_ptMtTable->GetMtStatus( byDstMtId, &tMtStatus );
// 
//     //目的终端是否在环回
//     if (tMtStatus.IsMediaLoop(MODE_VIDEO))
//     {
//         if (byMode == MODE_VIDEO)
//         {
//             return;
//         }
//         else if (byMode == MODE_BOTH)
//         {
//             byMode = MODE_AUDIO;
//         }
//     }
//     
//     if (tMtStatus.IsMediaLoop(MODE_AUDIO))
//     {
//         if (byMode == MODE_AUDIO)
//         {
//             return;
//         }
//         else if (byMode == MODE_BOTH)
//         {
//             byMode = MODE_VIDEO;
//         }
//     }
// 
//     if ( bSelSelf &&
//           // 为图像平滑，vcs在单方模式下主席终端不进行看自己的图像切换，直到下一个调度终端上线
// 		 !(VCS_CONF == m_tConf.GetConfSource() && 
//  		   byDstMtId == m_tConf.GetChairman().GetMtId() && 
//  		   (VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode() &&
// 		    !m_cVCSConfStatus.GetCurVCMT().IsNull()) ||
// 		   (ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
// 		    m_cVCSConfStatus.GetChairPollState() != VCS_POLL_STOP)))
//     {
// 		// xliang [4/15/2009] FIXME：主席从选看某个MT切换到选看VMP，就会先看自己再看VMP 。
// 	    //通知终端停止接收，看自己 
// 	    if( byMode == MODE_BOTH || byMode == MODE_VIDEO )
// 	    {
// 		    if( m_tConfAllMtInfo.MtJoinedConf( byDstMtId ) )
// 		    {                    
//                 NotifyMtReceive( tDstMt, byDstMtId );
// 		    }		
// 	    }
//     }
// 	
//     // zbq [06/20/2007] 为保证点名人平滑切看被点名人，此处不停上一个被点名
//     // 人的音视频交换，依赖于新的点名人建交换的时候的StartStopSwitch()停之。
//     // zbq [09/11/2007] FIXME: 此处未特殊处理点名人逻辑，导致的点名人看多次切换的还面问题 后续考虑解决
//     // if ( !(!m_tRollCaller.IsNull() && tDstMt == m_tRollCaller) )
//     {
//         // stop switch
//         g_cMpManager.StopSwitchToSubMt( tDstMt, byMode,FALSE,wSpyPort );
//     }
// 		
// 	if( bStopSelByMcs )
// 	{
// 		tMtStatus.RemoveSelByMcsMode( byMode );
// 		// 清空上次选看终端	
// 		TMt tNullMt;
// 		tNullMt.SetNull();
// 		tMtStatus.SetSelectMt(tNullMt, byMode);
// 	}
// 
// 	m_ptMtTable->SetMtStatus( byDstMtId, &tMtStatus );	
// 	
//     // guzh [8/31/2006] 要从上面移动到下面，否则SetMtStatus后MtSrc会被冲掉
// 	// 设置终端视频状态为看自己，而音频是 TMTNull
//     if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
//     {
//         m_ptMtTable->SetMtSrc( byDstMtId, &TMtNull, MODE_AUDIO );    
//     }
// 
//     if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
//     {
// 		if( MT_TYPE_MMCU != m_ptMtTable->GetMtType( byDstMtId )  )
// 		{
// 			m_ptMtTable->SetMtSrc( byDstMtId, &tDstMt, MODE_VIDEO );
// 		}
// 		else
// 		{
// 			m_ptMtTable->SetMtSrc( byDstMtId, &TMtNull, MODE_VIDEO );
// 		}
//     }	
// 
// 	if( bMsgStatus )
// 	{
//         MtStatusChange( &tDstMt, TRUE );
// 	}
// }

/*====================================================================
    函数名      ：StopSwitchToSubMt
    功能        ：停止将数据交换到直连下级非次级终端
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtNum 终端数目
	              TMt *ptDstMt 终端信息
				  u8 byMode 音视频模式
				  BOOL32 bMsg, 是否发通知接收信息，缺省为TRUE
				  u8 byMode, 交换模式，缺省为MODE_BOTH
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/11    1.0         LI Yi         创建
	20110505    4.6         pengjie       批量拆交换支持
====================================================================*/
void CMcuVcInst::StopSwitchToSubMt(u8 byMtNum,
								   TMt *ptDstMt,
								   u8 byMode, // MODE_VIDEO Or MODE_AUDIO
								   BOOL32 bMsgStatus, 
								   BOOL32 bStopSelByMcs, 
								   BOOL32 bSelSelf,
								   u16 wSpyPort)
{
	if( byMtNum == 0 || ptDstMt == NULL )
	{
		return;
	}

	TMt atDstMt[MAXNUM_CONF_MT];
	memcpy( atDstMt, ptDstMt, sizeof(TMt)*byMtNum );

	if( MODE_BOTH == byMode )
	{
		StopSwitchToSubMt( byMtNum, atDstMt, MODE_VIDEO, bMsgStatus, bStopSelByMcs, bSelSelf, wSpyPort );
		StopSwitchToSubMt( byMtNum, atDstMt, MODE_AUDIO, bMsgStatus, bStopSelByMcs, bSelSelf, wSpyPort );
		return;
	}

	TMt    TMtNull;
	TMtStatus   tMtStatus;
	TMtNull.SetNull();

	for( u8 byLoop = 0; byLoop < byMtNum; byLoop++ )
	{
		m_ptMtTable->GetMtStatus( atDstMt[byLoop].GetMtId(), &tMtStatus );

		//目的终端是否在环回
		if (tMtStatus.IsMediaLoop(MODE_VIDEO) && byMode == MODE_VIDEO)
		{
			atDstMt[byLoop].SetNull();
			continue;
		}

		if (tMtStatus.IsMediaLoop(MODE_AUDIO) && byMode == MODE_AUDIO )
		{
			atDstMt[byLoop].SetNull();
			continue;
		}

		if ( bSelSelf &&
			// 为图像平滑，vcs在单方模式下主席终端不进行看自己的图像切换，直到下一个调度终端上线
			/*!(VCS_CONF == m_tConf.GetConfSource() && 
			atDstMt[byLoop].GetMtId() == m_tConf.GetChairman().GetMtId() && 
			(VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode() &&
			!m_cVCSConfStatus.GetCurVCMT().IsNull()) ||
			(ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
			m_cVCSConfStatus.GetChairPollState() != VCS_POLL_STOP &&
			m_cVCSConfStatus.GetChairPollState() != VCS_POLL_PAUSE))*/			
			( VCS_CONF != m_tConf.GetConfSource() ||
				( atDstMt[byLoop].GetMtId() != m_tConf.GetChairman().GetMtId() && 
					( !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) || GetVidBrdSrc().IsNull() && !m_tConfAllMtInfo.MtJoinedConf(m_tConf.GetChairman()) ) 
				) ||
				( atDstMt[byLoop].GetMtId() == m_tConf.GetChairman().GetMtId() &&
					( ( VCS_SINGLE_MODE != m_cVCSConfStatus.GetCurVCMode() || m_cVCSConfStatus.GetCurVCMT().IsNull() ) ||
						( !ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) ||
							m_cVCSConfStatus.GetChairPollState() == VCS_POLL_STOP ||
							m_cVCSConfStatus.GetChairPollState() == VCS_POLL_PAUSE 
						)
					)
				) 			
			)			
		)
		{
			// xliang [4/15/2009] FIXME：主席从选看某个MT切换到选看VMP，就会先看自己再看VMP 。
			//通知终端停止接收，看自己 
			if( byMode == MODE_BOTH || byMode == MODE_VIDEO )
			{
				if( m_tConfAllMtInfo.MtJoinedConf( atDstMt[byLoop].GetMtId() ) )
				{                    
					NotifyMtReceive( atDstMt[byLoop], atDstMt[byLoop].GetMtId() );
				}		
			}
		}
	}

	g_cMpManager.StopSwitchToSubMt( m_byConfIdx, byMtNum, atDstMt, byMode, wSpyPort);

	for( u8 byMtIdx = 0; byMtIdx < byMtNum; byMtIdx++ )
	{
		m_ptMtTable->GetMtStatus( atDstMt[byMtIdx].GetMtId(), &tMtStatus );
		if( bStopSelByMcs )
		{
			tMtStatus.RemoveSelByMcsMode( byMode );
		}
		
		m_ptMtTable->SetMtStatus( atDstMt[byMtIdx].GetMtId(), &tMtStatus );	
		
		// guzh [8/31/2006] 要从上面移动到下面，否则SetMtStatus后MtSrc会被冲掉
		// 设置终端视频状态为看自己，而音频是 TMTNull
		//zhouyiliang 20121015 如果是目的终端是上级mcu，且停的是多回传的交换，则不清其srcmt
		if ( m_tCascadeMMCU.IsNull() || atDstMt[byMtIdx].GetMtId() != m_tCascadeMMCU.GetMtId() 
			|| wSpyPort == SPY_CHANNL_NULL )
		{
			if (byMode == MODE_AUDIO )
			{
				m_ptMtTable->SetMtSrc( atDstMt[byMtIdx].GetMtId(), &TMtNull, MODE_AUDIO );    
			}
			
			if (byMode == MODE_VIDEO )
			{
				if( MT_TYPE_MMCU != m_ptMtTable->GetMtType( atDstMt[byMtIdx].GetMtId() )  )
				{
					m_ptMtTable->SetMtSrc( atDstMt[byMtIdx].GetMtId(), &atDstMt[byMtIdx], MODE_VIDEO );
				}
				else
				{
					m_ptMtTable->SetMtSrc( atDstMt[byMtIdx].GetMtId(), &TMtNull, MODE_VIDEO );
				}
			}	
		}
	
		
		if( bMsgStatus )
		{
			MtStatusChange( &atDstMt[byMtIdx], TRUE );
		}
	}
	

	return;
}

/*=============================================================================
    函 数 名： StartSwitchToMMcu
    功    能： 将指定终端数据交换给mcu
    算法实现： 
    全局变量： 
    参    数： const TMt & tSrc, 源
			   u8 bySrcChnnl, 源的信道号
			   u8 byDstMtId, 目的终端
			   u8 byMode, 交换模式，缺省为MODE_BOTH 
    返回值说明  ：TRUE，如果不交换直接返回FALSE
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/5/23   3.6			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcInst::StartSwitchToMcu(TMt tSrc,
                                    u8  bySrcChnnl,
                                    u8  byDstMtId,
                                    u8  byMode,
                                    u8  bySwitchMode,
                                    BOOL32 bMsgStatus)
{
	TMtStatus   tMtStatus;
	CServMsg    cServMsg;
	u8 byMediaMode = byMode;
    BOOL32 bSetRealMediaMode = FALSE;

	TMt tUnLocalSrc = tSrc;

	tSrc = GetLocalMtFromOtherMcuMt(tSrc);

	//检测非KDC终端是否处于哑音状态，是则不交换音频码流	
	if( TYPE_MT == m_ptMtTable->GetMainType( tSrc.GetMtId() ) && 
		MT_MANU_KDC != m_ptMtTable->GetManuId( tSrc.GetMtId() ) && 
		m_ptMtTable->IsMtAudioDumb( tSrc.GetMtId() ) )
	{
		if( byMediaMode == MODE_AUDIO )
		{
			return TRUE;
		}
		else if ( byMediaMode == MODE_BOTH )
		{
			byMediaMode = MODE_VIDEO;
			bSetRealMediaMode = TRUE;
		}
	}

	//检测非KDC终端是否处于静音状态，是则不向他交换音频码流	
	if( TYPE_MT == m_ptMtTable->GetMainType( byDstMtId ) && 
		MT_MANU_KDC != m_ptMtTable->GetManuId( byDstMtId ) && 
		m_ptMtTable->IsMtAudioMute( byDstMtId ) )
	{
		if( byMediaMode == MODE_AUDIO )
		{
			return TRUE;
		}
		else if ( byMediaMode == MODE_BOTH )
		{
			byMediaMode = MODE_VIDEO;
			bSetRealMediaMode = TRUE;
		}
	}

	u32 dwTimeIntervalTicks = 3*OspClkRateGet();

// 	TLogicalChannel     tAudChnnl;
// 	m_ptMtTable->GetMtLogicChnnl( tSrc, LOGCHL_AUDIO, &tAudChnnl, FALSE );	
	
	//只向上级Mcu发spyMt
	TMt tDstMt = m_ptMtTable->GetMt(byDstMtId);
	if(m_tCascadeMMCU.GetMtId() != 0 && byDstMtId == m_tCascadeMMCU.GetMtId())
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(tDstMt.GetMtId()));
		if( ptConfMcInfo != NULL && 
			( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
		{
			if(ptConfMcInfo->m_tSpyMt.GetMtId() == tSrc.GetMtId()&&
				ptConfMcInfo->m_tSpyMt.GetMcuId() == tSrc.GetMcuId())
			{
				// 本级终端或单回传下级终端进上传通道
			}
			else if (!tUnLocalSrc.IsLocal() && IsLocalAndSMcuSupMultSpy(tUnLocalSrc.GetMcuId())
				&&  ptConfMcInfo->m_tSpyMt.GetMtId() == tUnLocalSrc.GetMtId()&&
					ptConfMcInfo->m_tSpyMt.GetMcuId() == tUnLocalSrc.GetMcuId())
			{
				// 多回传下级终端进上传通道
			}
			else
			{
				return FALSE;
			}
		}		
		//zjj20100211 终端回传选看不建桥
		if( !(TYPE_MT == tSrc.GetType() && SWITCH_MODE_SELECT == bySwitchMode) )
		{
			g_cMpManager.SetSwitchBridge(tSrc, 0, byMediaMode);
		}
		if( !IsCanSetOutView( tUnLocalSrc, byMediaMode ) )
		{
			return FALSE;
		}
	}
 
	//只发送终端, 则不用进行交换
	m_ptMtTable->GetMtStatus(byDstMtId, &tMtStatus);
	if( ( (!tMtStatus.IsReceiveVideo()) && byMediaMode == MODE_VIDEO ) ||
		( (!tMtStatus.IsReceiveAudio()) && byMediaMode == MODE_AUDIO ) )
	{
		return FALSE;
	}

	if( m_tConf.m_tStatus.IsMixing() )
	{
		if( byMediaMode == MODE_AUDIO )
		{
			return FALSE;
		}
		else if( byMediaMode == MODE_BOTH )
		{
			byMediaMode = MODE_VIDEO;
			bSetRealMediaMode = TRUE;
		}
	}

    // zbq [06/29/2007] 回传通道的视频交换建前不停，依赖于DS替换建立
    BOOL32 bMediaDstMMcu = FALSE;
        
    // libo [11/24/2005]
    u16 wAdaptBitRate = 0;
    BOOL32 bAudBasCap = TRUE;
    BOOL32 bVidBasCap = TRUE;
	BOOL32 bCaseAdp   = FALSE;
	if (0 == m_tCascadeMMCU.GetMtId() || byDstMtId != m_tCascadeMMCU.GetMtId())
	{
		if (MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode)
        {
			if (IsNeedAdapt(tSrc, m_ptMtTable->GetMt(byDstMtId), MODE_AUDIO))
			{
				if (!m_tConf.m_tStatus.IsAudAdapting())
				{
					StartBrdAdapt(MODE_AUDIO);
				}
				else
				{
					RefreshBrdBasParamForSingleMt(byDstMtId, MODE_AUDIO);
					StartSwitchToSubMtFromAdp(byDstMtId, MODE_AUDIO);
				}
			}
			else
			{
				//zjl 20110510 StartSwitchToAll 替换 StartSwitchToSubMt
				//目的非上级mcu bStopBeforeStart = true
				//g_cMpManager.StartSwitchToSubMt(tSrc, bySrcChnnl, tDstMt, MODE_AUDIO, bySwitchMode, FALSE, TRUE);
				TSwitchGrp tSwitchGrp;
				tSwitchGrp.SetSrcChnl(bySrcChnnl);
				tSwitchGrp.SetDstMtNum(1);
				tSwitchGrp.SetDstMt(&tDstMt);
				g_cMpManager.StartSwitchToAll(tSrc, 1, &tSwitchGrp, MODE_AUDIO, bySwitchMode);
			}
		}

		if (MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode)
        {
			if (IsNeedAdapt(tSrc, m_ptMtTable->GetMt(byDstMtId), MODE_VIDEO))
			{
				if (!m_tConf.m_tStatus.IsVidAdapting())
				{
					StartBrdAdapt(MODE_VIDEO);
				}
				else
				{
					RefreshBrdBasParamForSingleMt(byDstMtId, MODE_VIDEO);
					StartSwitchToSubMtFromAdp(byDstMtId, MODE_VIDEO);
				}
			}
			else
			{
				//zjl 20110510 StartSwitchToAll 替换 StartSwitchToSubMt
				//目的非上级mcu bStopBeforeStart = true
				//g_cMpManager.StartSwitchToSubMt(tSrc, bySrcChnnl, tDstMt, MODE_VIDEO, bySwitchMode, FALSE, TRUE);
				TSwitchGrp tSwitchGrp;
				tSwitchGrp.SetSrcChnl(bySrcChnnl);
				tSwitchGrp.SetDstMtNum(1);
				tSwitchGrp.SetDstMt(&tDstMt);
				g_cMpManager.StartSwitchToAll(tSrc, 1, &tSwitchGrp, MODE_VIDEO, bySwitchMode);
			}
		}
	}
	else
	{
		CRecvSpy tSpyResource;
		u16 wSpyPort = SPY_CHANNL_NULL;	
		if( m_cSMcuSpyMana.GetRecvSpy( tUnLocalSrc, tSpyResource ) )
		{
			wSpyPort = tSpyResource.m_tSpyAddr.GetPort();
		}

		if (MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode)
        {
			//停上一个选看适配
			TMtStatus tMMcuStatus;
			m_ptMtTable->GetMtStatus(m_tCascadeMMCU.GetMtId(), &tMMcuStatus);
			TMt tOldAudSel = tMMcuStatus.GetSelectMt(MODE_AUDIO);
			if (!tOldAudSel.IsNull() &&
				IsNeedSelAdpt(tOldAudSel, m_tCascadeMMCU, MODE_AUDIO))
			{
				StopSelAdapt(tOldAudSel, m_tCascadeMMCU, MODE_AUDIO,TRUE,FALSE);
			}

			//音频交换
			if (IsNeedSelAdpt(tSrc, m_tCascadeMMCU, MODE_AUDIO))
			{
				StartSelAdapt(tUnLocalSrc, m_tCascadeMMCU, MODE_AUDIO);
			}
			else
			{	
				//zjl 20110510 StartSwitchToAll 替换 StartSwitchToSubMt
				//目的上级mcu bStopBeforeStart = TRUE
				//zhouyiliang 20100826修改回传通道终端替换交换建立stopbeforestart 为true
				//g_cMpManager.StartSwitchToSubMt(tSrc, bySrcChnnl, m_tCascadeMMCU, MODE_AUDIO, bySwitchMode, FALSE, TRUE);
				TSwitchGrp tSwitchGrp;
				tSwitchGrp.SetSrcChnl(bySrcChnnl);
				tSwitchGrp.SetDstMtNum(1);
				tSwitchGrp.SetDstMt(&m_tCascadeMMCU);
				g_cMpManager.StartSwitchToAll(tSrc, 1, &tSwitchGrp, MODE_AUDIO, bySwitchMode, TRUE, wSpyPort);
			}	
		}

		if (MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode)
		{
			//停上一个选看适配
			TMtStatus tMMcuStatus;
			m_ptMtTable->GetMtStatus(m_tCascadeMMCU.GetMtId(), &tMMcuStatus);
			TMt tOldVidSel = tMMcuStatus.GetSelectMt(MODE_VIDEO);
			if (!tOldVidSel.IsNull() &&
				IsNeedSelAdpt(tOldVidSel, m_tCascadeMMCU, MODE_VIDEO))
			{
				StopSelAdapt(tOldVidSel, m_tCascadeMMCU, MODE_VIDEO,TRUE,FALSE);
			}

			//视频交换
			if (IsNeedSelAdpt(tSrc, m_tCascadeMMCU, MODE_VIDEO))
			{
				StartSelAdapt(tUnLocalSrc, m_tCascadeMMCU, MODE_VIDEO);
			}
			else
			{	
				//zjl 20110510 StartSwitchToAll 替换 StartSwitchToSubMt
				//目的上级mcu bStopBeforeStart = TRUE
				//zhouyiliang 20100826修改回传通道终端替换交换建立stopbeforestart 为true
				//g_cMpManager.StartSwitchToSubMt(tSrc, bySrcChnnl, m_tCascadeMMCU, MODE_VIDEO, bySwitchMode, FALSE, TRUE);
				TSwitchGrp tSwitchGrp;
				tSwitchGrp.SetSrcChnl(bySrcChnnl);
				tSwitchGrp.SetDstMtNum(1);
				tSwitchGrp.SetDstMt(&m_tCascadeMMCU);
				g_cMpManager.StartSwitchToAll(tSrc, 1, &tSwitchGrp, MODE_VIDEO, bySwitchMode, TRUE, wSpyPort);
			}
		}
	}

	//通知终端开始发送 
	if( tSrc.GetType() == TYPE_MT )
	{
		NotifyMtSend( tSrc.GetMtId(), byMediaMode, TRUE );

        // zbq [06/25/2007] 建音频交换不请求关键帧
        if ( MODE_VIDEO == byMode || MODE_BOTH == byMode )
        {
            NotifyFastUpdate(tSrc, MODE_VIDEO, TRUE);
        }
	}

	//设置终端的码流提示标识
	TPeriEqpStatus tEqpStatus;
	if( tSrc == m_tPlayEqp )
	{
		g_cMcuVcApp.GetPeriEqpStatus( m_tPlayEqp.GetEqpId(), &tEqpStatus );
		cServMsg.SetMsgBody( (u8*)tEqpStatus.GetAlias(), MAXLEN_EQP_ALIAS );
		SendMsgToMt( byDstMtId, MCU_MT_MTSOURCENAME_NOTIF, cServMsg );		
	}

	//通知终端开始接收
	if( byMediaMode == MODE_BOTH || byMediaMode == MODE_VIDEO )
	{
		NotifyMtReceive( tSrc, tDstMt.GetMtId() );
	}

	//设置终端状态
	m_ptMtTable->SetMtSrc( byDstMtId, &tUnLocalSrc, bSetRealMediaMode ? byMode:byMediaMode );

	TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();

	//20110519 zjl 指定下级终端回传，相当于上级mcu选看下级终端，回传过适配时会设置保存selectmt,
	//因为释放bas的条件需要知道终端选看状态，但这里不设置mcs选看状态
	///*(m_tCascadeMMCU.GetMtId() == 0 || byDstMtId != m_tCascadeMMCU.GetMtId())*/
	//上述判断件在最外面过滤，否则startseladpt里设置的selectmt会在这里被清空
	
	if(0 == m_tCascadeMMCU.GetMtId() || m_tCascadeMMCU.GetMtId() != tDstMt.GetMtId())
	{
		m_ptMtTable->GetMtStatus( byDstMtId, &tMtStatus );
		if( tSrc.GetType() == TYPE_MT && tSrc.GetMtId() != byDstMtId //&& 
			/*(m_tCascadeMMCU.GetMtId() == 0 || byDstMtId != m_tCascadeMMCU.GetMtId())*/ )
		{
			TMtStatus tSrcMtStatus;
			u8 byAddSelByMcsMode = MODE_NONE;
			m_ptMtTable->GetMtStatus( tSrc.GetMtId(), &tSrcMtStatus );
			
			if( !(tSrc == tLocalVidBrdSrc) && tSrcMtStatus.IsSendVideo() && 
				( MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode ) )
			{
				byAddSelByMcsMode = MODE_VIDEO;
			}
			if( !(tSrc == GetLocalAudBrdSrc()) && tSrcMtStatus.IsSendAudio() && 
				( MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode ) )
			{
				if( MODE_VIDEO == byAddSelByMcsMode )
				{
					byAddSelByMcsMode = MODE_BOTH;
				}
				else
				{
					byAddSelByMcsMode = MODE_AUDIO;
				}
			}
			
			if( MODE_NONE != byAddSelByMcsMode )
			{
				tMtStatus.AddSelByMcsMode( byMediaMode );
			}
			else
			{
				tMtStatus.RemoveSelByMcsMode( byMediaMode );
				// 清空上次选看终端	
				TMt tNullMt;
				tNullMt.SetNull();
				tMtStatus.SetSelectMt(tNullMt, byMediaMode);
			}
		}
		else
		{
			tMtStatus.RemoveSelByMcsMode( byMediaMode );
			// 清空上次选看终端	
			TMt tNullMt;
			tNullMt.SetNull();
			tMtStatus.SetSelectMt(tNullMt, byMediaMode);
		}
		m_ptMtTable->SetMtStatus( byDstMtId, &tMtStatus );
	}


	
	if( bMsgStatus )
	{
        MtStatusChange(&tDstMt, TRUE);
	}
    
	return TRUE;
}

/*=============================================================================
    函 数 名： StartSwitchToMMcu
    功    能： 将指定终端数据交换给mcu
    算法实现： 
    全局变量： 
    参    数： const TMt & tSrc, 源
			   u8 bySrcChnnl, 源的信道号
			   u8 byDstMtId, 目的终端
			   const TSimCapSet &tDstCap, 目的端能力
			   u8 byMode, 交换模式，缺省为MODE_BOTH 
    返回值说明  ：TRUE，如果不交换直接返回FALSE
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/5/23   3.6			万春雷                  创建
=============================================================================*/
BOOL32 CMcuVcInst::StartSwitchToMcuByMultiSpy(TMt tSrc,
                                    u16  wSrcChnnl,
                                    u8  byDstMtId,
									const TSimCapSet &tDstCap,
									u16 &wErrorCode,
                                    u8  byMode,
                                    u8  bySwitchMode,
                                    BOOL32 bMsgStatus)
{
	TMtStatus   tMtStatus;
	CServMsg    cServMsg;
	u8 byMediaMode = byMode;
    BOOL32 bSwitchedAudio = FALSE;

	wErrorCode = 0;

	TMt tUnLocalSrc = tSrc;

	tSrc = GetLocalMtFromOtherMcuMt(tSrc);

	//检测非KDC终端是否处于哑音状态，是则不交换音频码流	
	if( TYPE_MT == m_ptMtTable->GetMainType( tSrc.GetMtId() ) && 
		MT_MANU_KDC != m_ptMtTable->GetManuId( tSrc.GetMtId() ) && 
		m_ptMtTable->IsMtAudioDumb( tSrc.GetMtId() ) )
	{
		if( byMediaMode == MODE_AUDIO )
		{
			return TRUE;
		}
		else if ( byMediaMode == MODE_BOTH )
		{
			byMediaMode = MODE_VIDEO;
		}
	}

	//检测非KDC终端是否处于静音状态，是则不向他交换音频码流	
	if( TYPE_MT == m_ptMtTable->GetMainType( byDstMtId ) && 
		MT_MANU_KDC != m_ptMtTable->GetManuId( byDstMtId ) && 
		m_ptMtTable->IsMtAudioMute( byDstMtId ) )
	{
		if( byMediaMode == MODE_AUDIO )
		{
			return TRUE;
		}
		else if ( byMediaMode == MODE_BOTH )
		{
			byMediaMode = MODE_VIDEO;
		}
	}

	u32 dwTimeIntervalTicks = 3*OspClkRateGet();

	//zjj20100201
	//  [11/9/2009 pengjie] 级联多回传判断
	// [11/14/2009 xliang] mode 的确定，不能依据上面的byMediaMode  
	CSendSpy cSendSpy;
	u16 wSpyPort = SPY_CHANNL_NULL;
	if( m_cLocalSpyMana.GetSpyChannlInfo(tUnLocalSrc, cSendSpy) )
	{
		wSpyPort = cSendSpy.m_tSpyAddr.GetPort() ;
		
		//进一步确定mode
		//byMediaMode = m_cLocalSpyMana.GetSpyMode(tUnLocalSrc);
	}
	
	//只向上级Mcu发spyMt
	TMt tDstMt = m_ptMtTable->GetMt(byDstMtId);
	if(m_tCascadeMMCU.GetMtId() != 0 && byDstMtId == m_tCascadeMMCU.GetMtId())
	{
		if( !m_cLocalSpyMana.GetSpyChannlInfo(tUnLocalSrc, cSendSpy) )
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY, "[StartSwitchToMcuByMultiSpy] tSrc(%d.%d) is not in SpyChannelInfo.\n",
				tSrc.GetMcuId(),tSrc.GetMtId()
				);
			
			return FALSE;
		}
		if( SPY_CHANNL_NULL == cSendSpy.m_tSpyAddr.GetPort() || 0 == cSendSpy.m_tSpyAddr.GetPort() )
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY, "[StartSwitchToMcuByMultiSpy] tSrc(%d.%d) m_wSpyStartPort(%d) Get Failed.\n",
				tSrc.GetMcuId(),tSrc.GetMtId(),cSendSpy.m_tSpyAddr.GetPort() );
			return FALSE;
		}		
	}
 
	//只发送终端, 则不用进行交换
	m_ptMtTable->GetMtStatus(byDstMtId, &tMtStatus);
	if( ( (!tMtStatus.IsReceiveVideo()) && byMediaMode == MODE_VIDEO ) ||
		( (!tMtStatus.IsReceiveAudio()) && byMediaMode == MODE_AUDIO ) )
	{
		return FALSE;
	}

	//2010/06/24 lukunpeng 完全根据上级的意思，即使在混音，照样把此终端音频打到上级
	/*
	if( m_tConf.m_tStatus.IsMixing() )
	{
		if( byMediaMode == MODE_AUDIO )
		{
			return FALSE;
		}
		else if( byMediaMode == MODE_BOTH )
		{
			byMediaMode = MODE_VIDEO;
		}
	}
	*/

    // zbq [06/29/2007] 回传通道的视频交换建前不停，依赖于DS替换建立
    BOOL32 bMediaDstMMcu = FALSE;

    // libo [11/24/2005]
    u16 wAdaptBitRate = 0;
    BOOL32 bAudBasCap = TRUE;
    BOOL32 bVidBasCap = TRUE;
	BOOL32 bCaseAdp   = FALSE;
	TEqp tCasdVidBasEqp,tCasdAudBasEqp;
	tCasdVidBasEqp.SetNull();
	tCasdAudBasEqp.SetNull();
	u8 byCasdAudBasChnnl=EQP_CHANNO_INVALID,byCasdVidBasChnnl=EQP_CHANNO_INVALID;

	TSimCapSet tSrcCap = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
	// [2013/04/19 chenbing] MPEG-4 AUTO根据会议码率得到分辨率 
	if ( VIDEO_FORMAT_AUTO == tSrcCap.GetVideoResolution() )
	{
		//根据会议码率得到分辨率
		tSrcCap.SetVideoResolution(GetAutoResByBitrate(VIDEO_FORMAT_AUTO, m_tConf.GetBitRate()));
	}
	if ( MEDIA_TYPE_MP4 == tSrcCap.GetVideoMediaType() && VIDEO_FORMAT_16CIF == tSrcCap.GetVideoResolution() )
	{
		tSrcCap.SetVideoResolution( VIDEO_FORMAT_4CIF );
	}

	if (0 == m_tCascadeMMCU.GetMtId() || byDstMtId != m_tCascadeMMCU.GetMtId())
	{
		ConfPrint( LOG_LVL_DETAIL, MID_MCU_SPY, "[StartSwitchToMcuByMultiSpy] The m_tCascadeMMCU is Null or m_tCascadeMMCU.%d != DstMtId.%\n",
			m_tCascadeMMCU.GetMtId(), byDstMtId );
		return FALSE;
	}
	else
	{
		if (MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode)
        {
			// [pengjie 2010/9/13] 不管是不是keda的，音频不匹配都要过适配
			// 1.1 音频要过适配			
			TAudioTypeDesc tSrcAudCap;		
			TMt tLocalSrcMt = GetLocalMtFromOtherMcuMt( tSrc );
			TLogicalChannel tSrcAudLgc;
			if (!m_ptMtTable->GetMtLogicChnnl(tLocalSrcMt.GetMtId(), MODE_AUDIO, &tSrcAudLgc, FALSE))
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS,  "[IsNeedAdapt] GetMtLogicChnnl Src AUD Mt%d failed!\n", tLocalSrcMt.GetMtId());
				return FALSE;
			}
			
			tSrcAudCap.SetAudioMediaType( tSrcAudLgc.GetChannelType() );
			tSrcAudCap.SetAudioTrackNum( tSrcAudLgc.GetAudioTrackNum() );
			
			TAudioTypeDesc tDstAudCap;
			TLogicalChannel tDstAudLgc;
			if (!m_ptMtTable->GetMtLogicChnnl(m_tCascadeMMCU.GetMtId(), MODE_AUDIO, &tDstAudLgc, TRUE))
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS,  "[IsNeedSpyAdpt] GetMtLogicChnnl Dst Aud Mt%d failed!\n", m_tCascadeMMCU.GetMtId());
				return FALSE;
			}
			
			tDstAudCap.SetAudioMediaType( tDstAudLgc.GetChannelType() );
			tDstAudCap.SetAudioTrackNum( tDstAudLgc.GetAudioTrackNum() );	

			if (  tSrcAudCap.GetAudioMediaType() != MEDIA_TYPE_NULL &&
				tDstAudCap.GetAudioMediaType() != MEDIA_TYPE_NULL &&
				 ( tSrcAudCap.GetAudioMediaType() != tDstAudCap.GetAudioMediaType() 
			    	||tSrcAudCap.GetAudioTrackNum() != tDstAudCap.GetAudioTrackNum()
				  )
				)
			{
				if ( !StartSpyAudAdapt(tUnLocalSrc) )
				{
					wErrorCode = ERR_MCU_CASDBASISNOTENOUGH;
					return FALSE;
				}
			}
			// 1.2 音频不过适配
			else
			{
				BOOL32 bStopBeforeStart = bMediaDstMMcu ? FALSE : TRUE;
				if (!g_cMpManager.StartSwitchToMmcu(tSrc, wSrcChnnl, tDstMt, MODE_AUDIO, bySwitchMode,
					FALSE, bStopBeforeStart, FALSE, wSpyPort))        
				{
					ConfPrint( LOG_LVL_DETAIL, MID_MCU_SPY, "[StartSwitchToMcuByMultiSpy] StartSwitchToMmcu() failed! Cannot switch to specified sub mt!\n" );
					return FALSE;
				}
			}
		}
		
		//视频交换
		if (MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode)
		{
			// 起适配
			//20101213 zjl 格式相同并且源的分辨率中宽高有一者大于目的，则认为可以调分辨率
			s32 nResCmpRst = ResWHCmp(tSrcCap.GetVideoResolution(), tDstCap.GetVideoResolution());
			if(IsNeedSpyAdpt(tSrc, tDstCap, MODE_VIDEO))
			{
				if (!StartSpyAdapt(tUnLocalSrc, tDstCap, MODE_VIDEO))
				{
					wErrorCode = ERR_MCU_CASDBASISNOTENOUGH;
					return FALSE;
				}
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY, "[StartSwitchToMcuByMultiSpy] StartSpyAdapt OK!\n");
			}
			//调分辨率
			else if (tSrcCap.GetVideoMediaType() == tDstCap.GetVideoMediaType() &&
				( 1 == nResCmpRst || 
				-1 == nResCmpRst || 
				2 == nResCmpRst))
			{
				// [pengjie 2010/9/13] 多回传调分辨率逻辑调整
				u8 byChnnlType = LOGCHL_VIDEO;
				u8 byRes = tDstCap.GetVideoResolution();

				ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY,  "[StartSwitchToMcuByMultiSpy] Adjust SendChangeMtRes %d.fps!\n",
						tDstCap.GetVideoResolution());

				// [2013/05/16 chenbing] 以下分辨率调整只能操作本地终端
				// Mcu或下级终端不进行操作
				//Bug00161920:三级级联支持上传会议中，第三级MCU入电视墙画面合成通道，第三级终端进上传通道后不降分辨率
				//yrl20131114:场景：多回传拖3级mcu带上传终端进1级mcu电视墙4画面(或TVWall或VMP)要求降分辨率
				//if (!IsMcu(tSrc) && tSrc.IsLocal())
				if (tSrc.IsLocal())
				{
					SendChgMtVidFormat( tSrc.GetMtId(), byChnnlType, byRes, TRUE );
				}
				
				if ( IsNeedAdjustSpyFps(tSrc, tDstCap) )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY,  "[StartSwitchToMcuByMultiSpy] 2Adjust tSrc(%d, %d) SendChangeMtFps %d.fps!\n",tSrc.GetMcuId(), tSrc.GetMtId(),
						tDstCap.GetUserDefFrameRate());
					SendChangeMtFps(tSrc.GetMtId(), LOGCHL_VIDEO, tDstCap.GetUserDefFrameRate());
				}
			
				//直接打交换
				BOOL32 bStopBeforeStart = bMediaDstMMcu ? FALSE : TRUE;
				if (!g_cMpManager.StartSwitchToMmcu(tSrc, wSrcChnnl, tDstMt, MODE_VIDEO, bySwitchMode, FALSE, bStopBeforeStart, FALSE, wSpyPort))        
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY, "[StartSwitchToMcuByMultiSpy] StartSwitchToMmcu() failed! Cannot switch to specified sub mt!\n");
					return FALSE;
				}
			}
			//调帧率
			else if (IsNeedAdjustSpyFps(tSrc, tDstCap))
			{
				if (tDstCap.GetVideoMediaType() == MEDIA_TYPE_H264)
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY,  "[StartSwitchToMcuByMultiSpy] Adjust SendChangeMtFps %d.fps!\n",
						tDstCap.GetUserDefFrameRate());
					SendChangeMtFps(tSrc.GetMtId(), LOGCHL_VIDEO, tDstCap.GetUserDefFrameRate());
					
					BOOL32 bStopBeforeStart = bMediaDstMMcu ? FALSE : TRUE;
					if (!g_cMpManager.StartSwitchToMmcu(tSrc, wSrcChnnl, tDstMt, MODE_VIDEO, bySwitchMode, FALSE, bStopBeforeStart, FALSE, wSpyPort))        
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY, "[StartSwitchToMcuByMultiSpy-after adjust fps] StartSwitchToMmcu() failed! Cannot switch to specified sub mt!\n" );
						return FALSE;
					}
				}				
			}
			else
			{
				BOOL32 bStopBeforeStart = bMediaDstMMcu ? FALSE : TRUE;
				if (!g_cMpManager.StartSwitchToMmcu(tSrc, wSrcChnnl, tDstMt, MODE_VIDEO, bySwitchMode, FALSE, bStopBeforeStart, FALSE, wSpyPort))        
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY, "[StartSwitchToMcuByMultiSpy] StartSwitchToMmcu() failed! Cannot switch to specified sub mt!\n");
					return FALSE;
				}
			}
		}
	}
	
	//通知终端开始发送 
	if( tSrc.GetType() == TYPE_MT )
	{
		NotifyMtSend( tSrc.GetMtId(), byMediaMode, TRUE );
		
        // zbq [06/25/2007] 建音频交换不请求关键帧
        if ( MODE_VIDEO == byMode || MODE_BOTH == byMode )
        {
            NotifyFastUpdate(tSrc, MODE_VIDEO, TRUE);
        }
	}
	
	//设置终端的码流提示标识
	TPeriEqpStatus tEqpStatus;
	if( tSrc == m_tPlayEqp )
	{
		g_cMcuVcApp.GetPeriEqpStatus( m_tPlayEqp.GetEqpId(), &tEqpStatus );
		cServMsg.SetMsgBody( (u8*)tEqpStatus.GetAlias(), MAXLEN_EQP_ALIAS );
		SendMsgToMt( byDstMtId, MCU_MT_MTSOURCENAME_NOTIF, cServMsg );		
	}
	
	
	//通知终端开始接收
	if( byMediaMode == MODE_BOTH || byMediaMode == MODE_VIDEO )
	{
		NotifyMtReceive( tSrc, tDstMt.GetMtId() );
	}
	
	//设置终端状态
	if( m_tCascadeMMCU.GetMtId() == 0 || byDstMtId != m_tCascadeMMCU.GetMtId() )
	{
		m_ptMtTable->SetMtSrc( byDstMtId, ( const TMt * )&tUnLocalSrc, byMediaMode );
	}	

	//[2011/07/22/zhangli]这段代码删除
	//上面有判断：if (0 == m_tCascadeMMCU.GetMtId() || byDstMtId != m_tCascadeMMCU.GetMtId())则return
	//因此if{}恒为假，同时删除else里清空上次选看终端没有必要(为了解决Bug00059187)

// 	TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();
// 	//过滤目标终端为上级mcu的情况
// 	m_ptMtTable->GetMtStatus( byDstMtId, &tMtStatus );
// 	if( tSrc.GetType() == TYPE_MT && tSrc.GetMtId() != byDstMtId && 
// 		(m_tCascadeMMCU.GetMtId() == 0 || byDstMtId != m_tCascadeMMCU.GetMtId()) )
// 	{
// 		TMtStatus tSrcMtStatus;
// 		u8 byAddSelByMcsMode = MODE_NONE;
// 		m_ptMtTable->GetMtStatus( tSrc.GetMtId(), &tSrcMtStatus );
// 
// 		if( !(tSrc == tLocalVidBrdSrc) && tSrcMtStatus.IsSendVideo() && 
// 			( MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode ) )
// 		{
// 			byAddSelByMcsMode = MODE_VIDEO;
// 		}
// 		if( !(tSrc == GetLocalAudBrdSrc()) && tSrcMtStatus.IsSendAudio() && 
// 			( MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode ) )
// 		{
// 			if( MODE_VIDEO == byAddSelByMcsMode )
// 			{
// 				byAddSelByMcsMode = MODE_BOTH;
// 			}
// 			else
// 			{
// 				byAddSelByMcsMode = MODE_AUDIO;
// 			}
// 		}
// 
// 		if( MODE_NONE != byAddSelByMcsMode )
// 		{
// 			tMtStatus.AddSelByMcsMode( byMediaMode );
// 		}
// 		else
// 		{
// 			tMtStatus.RemoveSelByMcsMode( byMediaMode );
// 			// 清空上次选看终端	
// 			TMt tNullMt;
// 			tNullMt.SetNull();
// 			tMtStatus.SetSelectMt(tNullMt, byMediaMode);
// 		}
// 	}
// 	else
// 	{
// 		tMtStatus.RemoveSelByMcsMode( byMediaMode );
// 		// 清空上次选看终端	
// 		TMt tNullMt;
// 		tNullMt.SetNull();
// 		tMtStatus.SetSelectMt(tNullMt, byMediaMode);
// 	}
// 	m_ptMtTable->SetMtStatus( byDstMtId, &tMtStatus );
	
	if( bMsgStatus )
	{
        MtStatusChange(&tDstMt, TRUE);
	}
    
	return TRUE;
}

/*====================================================================
    函数名      ：CanMtRecvVideo
    功能        ：判断目标终端是否能接受视频交换(同时判断能力集)
    算法实现    ：
    引用全局变量：
    输入参数说明：tDst, 交换目标
    返回值说明  ：TRUE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/03/31    4.0         顾振华        创建
====================================================================*/
BOOL32 CMcuVcInst::CanMtRecvVideo(const TMt &tDst, const TMt &tSrc)
{
    TMtStatus tStatus;

    u8 byMtId = tDst.GetMtId();
    m_ptMtTable->GetMtStatus(byMtId, &tStatus);
    
    if ( !m_tConfAllMtInfo.MtJoinedConf( byMtId ) || 
         !m_ptMtTable->IsLogicChnnlOpen(byMtId, LOGCHL_VIDEO, TRUE) //||
		 //zbq[08/27/2008] tSrc和tDst都是mcu，交换仍然建立(目前该类交换未走广播)
         /*tSrc == tDst*/ )
    {
        return FALSE;
    }            
    if ( !tStatus.IsReceiveVideo() )
    {
        return FALSE;
    }

    // 如果目标是MMCU，只向上级Mcu发spyMt，不建立交换
    if ( !m_tCascadeMMCU.IsNull() && tDst == m_tCascadeMMCU )
    {
		u16 wMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
        TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
        if  ( ptConfMcInfo != NULL && 
            ( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
        {
            if (!( ptConfMcInfo->m_tSpyMt.GetMtId() == tSrc.GetMtId() &&
                   ptConfMcInfo->m_tSpyMt.GetMcuId() == tSrc.GetMcuId() ) )
            {
                return FALSE;				
            }
        }
	}

    //对于已借用第一路视频通道传输双流的终端，切换第一路视频源将被拒绝
    u8 bySrcChn = tSrc == m_tPlayEqp ? m_byPlayChnnl : 0;
    TMt tTmpSrc = tSrc;
    TMt tTmpDst = tDst;
    if( JudgeIfSwitchFirstLChannel(tTmpSrc, bySrcChn, tTmpDst) )
    {
        return FALSE;
	}

    //不要把自己的码流交换回去, 如果是源与目标均为mcu，仍应进行交换
    if( tSrc == tDst && 
        m_ptMtTable->GetManuId( byMtId ) == MT_MANU_KDC )
    {               
        return FALSE;
	}

    //是否需要高清适配才能交换
//     if ( IsHDConf( m_tConf ) )
//     {
//         if (m_cMtRcvGrp.IsMtNeedAdp(byMtId))
//         {
//             return FALSE;
//         }
//     }
//     else
    //是否需要适配才能交换
    if (IsNeedAdapt(tSrc, tDst, MODE_VIDEO))
    {
        return FALSE;
    }
	
	// 增加对双格式VMP的判断, zgc, 20070604
	if( tStatus.IsReceiveVideo() && g_cMcuVcApp.IsBrdstVMP(m_tVmpEqp) )
	{
		TSimCapSet tSimCapSet; 
        u8 byMediaType = m_tConf.GetCapSupport().GetMainSimCapSet().GetVideoMediaType();
        u16 wDstMtReqBitrate = m_ptMtTable->GetMtReqBitrate( byMtId );

        //先处理单格式的情况
        if ( 0 != m_tConf.GetSecBitRate() &&
			MEDIA_TYPE_NULL == m_tConf.GetCapSupport().GetSecondSimCapSet().GetVideoMediaType() )
        {
			if ( wDstMtReqBitrate < m_wVidBasBitrate )
			{
				return FALSE;
			}
		}
		//双格式
		else
        {                
            tSimCapSet = m_ptMtTable->GetDstSCS( byMtId );
            if (tSimCapSet.GetVideoMediaType() == byMediaType)
            {
				if ( wDstMtReqBitrate < m_wVidBasBitrate )
				{
					return FALSE;
				}
            }
            else
            {
                // 辅格式终端不走广播交换收VMP第二路, zgc, 2008-09-20
                /*
				if ( wDstMtReqBitrate < m_wBasBitrate )
                {
                     return FALSE;
                }
                */
                return FALSE;
            }
        }          
	}

    return TRUE;
}


/*====================================================================
    函数名      ：StartSwitchFromBrd
    功能        ：将广播源的码流交换给指定终端
    算法实现    ：
    引用全局变量：
    输入参数说明：tDst, 交换目标
    返回值说明  ：TRUE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/03/31    4.0         顾振华        创建
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchFromBrd(const TMt &tSrcMt,
                                      u8        bySrcChnnl,
                                      u8        byDstMtNum,
                                      const TMt* const ptDstBase,
									  u16 wSpyStartPort)
{
    u8 byLoop = 0;

	// [pengjie 2010/4/15] 多回传支持
	TMt tUnLocalMt = tSrcMt;
	TMt tSrc = GetLocalMtFromOtherMcuMt( tSrcMt );

    // zgc, 2008-05-28, 清选看
    TMtStatus tMtStatus;
    u8 bySelMode = MODE_NONE;
	TMt tSelSrc;
	tSelSrc.SetNull();

	TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();

	TMt tVidBrdSrc = GetVidBrdSrc();

    for ( byLoop = 0; byLoop < byDstMtNum; byLoop ++ )
    {
        // zgc, 2008-06-02, 点名人和被点名人不从广播交换，保留选看
        if ( tLocalVidBrdSrc == m_tVmpEqp && 
            ( ptDstBase[byLoop] == m_tRollCaller ||
            ptDstBase[byLoop] == m_tRollCallee) )
        {
            continue;
        }

        m_ptMtTable->GetMtStatus(ptDstBase[byLoop].GetMtId(), &tMtStatus);
        bySelMode = tMtStatus.GetSelByMcsDragMode();

		//lukunpeng 2010/07/22 不要停发言人的选看
		if (ptDstBase[byLoop] == m_tConf.GetSpeaker() || bySelMode == MODE_NONE)
		{
			continue;
		}

		//  [11/26/2009 pengjie] Modify 级联多回传 这里应该根据实际选看的模式，来进行停止，而不是只停视屏
		u8 byIsRestore = FALSE;
		if(!tVidBrdSrc.IsNull() && tSrc == tVidBrdSrc)
		{
			byIsRestore = TRUE;
		}
		
		//只清视频选看[6/14/2012 chendaiwei]
		if( MODE_BOTH == bySelMode || MODE_VIDEO == bySelMode )
		{
			StopSelectSrc(ptDstBase[byLoop], MODE_VIDEO, byIsRestore);
		}
    }

	//zjj20100402 除回传通道终端，其它都不能往上级mcu级联端口打交换
	BOOL32 bIsCanSwitchBrdToMMCU = TRUE;
	TConfMcInfo *ptConfMcInfo = NULL;
	u16 wMMcuIdx = INVALID_MCUIDX;
	if ( !m_tCascadeMMCU.IsNull() )
    {
		wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
		//m_tConfAllMcuInfo.GetIdxByMcuId( m_tCascadeMMCU.GetMtId(),0,&wMMcuIdx );
        ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMMcuIdx);//m_tCascadeMMCU.GetMtId());
		if( NULL != ptConfMcInfo &&
			!( tSrc == ptConfMcInfo->m_tSpyMt ) )
		{
			bIsCanSwitchBrdToMMCU = FALSE;
		}
	}
    BOOL32 bRet = TRUE;
    // 对于单个的交换，是零星的请求（如新加入的终端等）要重新判断是否能接收
    // 对于批量交换，会在构建交换目标列表时统一判断
    if  ( byDstMtNum == 1 )
	{		
		if( CanMtRecvVideo(ptDstBase[0], tSrc) )
		{
            //StopSwitchToSubMt( ptDstBase[0].GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE, FALSE, FALSE );

            // zbq [06/19/2007] 点名人和被点名人不从广播交换
            if ( !( tLocalVidBrdSrc == m_tVmpEqp && 
                    ( ptDstBase[0] == m_tRollCaller ||
                      ptDstBase[0] == GetLocalMtFromOtherMcuMt(m_tRollCallee)) ) )
            {
				
				if (!m_tCascadeMMCU.IsNull()  &&
					m_ptConfOtherMcTable->GetMcInfo(wMMcuIdx/*m_tCascadeMMCU.GetMtId()*/) != NULL &&
					m_ptConfOtherMcTable->GetMcInfo(wMMcuIdx/*m_tCascadeMMCU.GetMtId()*/)->m_tSpyMt == tSrc &&
					m_tCascadeMMCU == ptDstBase[0] )
				{
					// xliang [3/2/2009] 若该src正好又在回传通道里，则不拆交换
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[StartSwitchFromBrd] Since the source is also under back-to-MMCU switch, we shouldn't stop switch in this case.\n" );
				}
				else
				{
					//zjl 20110510 StopSwitchToSubMt 接口重载替换
					// 解决BUG10501, 从上面移到这里,保证停--建交换对应, zgc, 2008-04-22
					//StopSwitchToSubMt( ptDstBase[0].GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE, FALSE, FALSE );
					TMt tMt = ptDstBase[0];
					StopSwitchToSubMt(1, &tMt, MODE_VIDEO, FALSE, FALSE, FALSE);
				}
				
				if(  MT_TYPE_MMCU != m_ptMtTable->GetMtType( ptDstBase[0].GetMtId() )  ||
						 ( ptDstBase[0].GetMtId() != tSrc.GetMtId() && bIsCanSwitchBrdToMMCU )						 	
					)
				{
					bRet = g_cMpManager.StartSwitchFromBrd(tSrc, bySrcChnnl, byDstMtNum, ptDstBase,wSpyStartPort);

					//[2011/08/22/zhangli]恢复到广播PRS的RTCP交换
					TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
					if (tConfAttrb.IsResendLosePack())
					{
						u8 byPrsId = 0;
						u8 byPrsChlId = 0;
						if (FindPrsChnForSrc(tUnLocalMt, bySrcChnnl, MODE_VIDEO, byPrsId, byPrsChlId))
						{
							AddRemovePrsMember(ptDstBase[0].GetMtId(), byPrsId, byPrsChlId, MODE_VIDEO);
						}
					}
				}
            }
            else
            {
                ConfPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[StartSwitchFromBrd] Mt.%d is in Rollcall, no accept from brd(DstNum=1)\n", ptDstBase[0].GetMtId() );
                bRet = TRUE;              
            }
		}
		else
		{
            if (!( ( m_ptMtTable->GetMtType(ptDstBase[0].GetMtId()) == MT_TYPE_SMCU || !(tSrc == ptDstBase[0] ) ) && IsNeedAdapt(tSrc, ptDstBase[0], MODE_VIDEO) && 
				StartSwitchToSubMtFromAdp(ptDstBase[0].GetMtId(), MODE_VIDEO)))
			{
				//zjl 20110510 StartSwitchToAll 替换 StartSwitchToSubMt
				//bRet = StartSwitchToSubMt( tSrc, bySrcChnnl, ptDstBase[0].GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE, FALSE,FALSE,wSpyStartPort );
				TSwitchGrp tSwitchGrp;
				tSwitchGrp.SetSrcChnl(bySrcChnnl);
				tSwitchGrp.SetDstMtNum(1);
				TMt tMt = ptDstBase[0];
				tSwitchGrp.SetDstMt(&tMt);
				StartSwitchToAll(tSrc, 1, &tSwitchGrp, MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE, FALSE, wSpyStartPort);

				//[2013/02/20]Bug00127469 如果是上级MCU，则不需要补建RTCP交换
				if ( !m_tCascadeMMCU.IsNull() && ptDstBase[0] == m_tCascadeMMCU && tSrc == ptDstBase[0] )
				{
					u16 wMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
					ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
					if  ( ptConfMcInfo != NULL && 
						( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
					{
						if (!( ptConfMcInfo->m_tSpyMt.GetMtId() == tSrc.GetMtId() &&
							ptConfMcInfo->m_tSpyMt.GetMcuId() == tSrc.GetMcuId() ) )
						{
							return bRet;				
						}
					}
				}

				//[2011/08/22/zhangli]恢复到广播PRS的RTCP交换
				TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
				if (tConfAttrb.IsResendLosePack())
				{
					u8 byPrsId = 0;
					u8 byPrsChlId = 0;
					if (FindPrsChnForSrc(tUnLocalMt, bySrcChnnl, MODE_VIDEO, byPrsId, byPrsChlId))
					{
						AddRemovePrsMember(ptDstBase[0].GetMtId(), byPrsId, byPrsChlId, MODE_VIDEO);
					}
				}
			}
			return bRet;
		}
	}
    else if( byDstMtNum > 1 )
    {
		TMt atDstMt[MAXNUM_CONF_MT];
		memcpy( atDstMt, ptDstBase, (sizeof(TMt) * byDstMtNum) );

        //zbq[03/29/2008] 整理接收的广播目标，过滤点名人和被点名人收VMP的情况
        if ( tLocalVidBrdSrc == m_tVmpEqp &&
             ROLLCALL_MODE_NONE != m_tConf.m_tStatus.GetRollCallMode() )
        {
            for( byLoop = 0; byLoop < byDstMtNum; byLoop ++ )
            {
                if ( ptDstBase[byLoop] == m_tRollCaller ||
                     ptDstBase[byLoop] == GetLocalMtFromOtherMcuMt(m_tRollCallee))
                {
					atDstMt[byLoop].SetNull();
                    continue;
                }
                else
                {
					if( !( MT_TYPE_MMCU != m_ptMtTable->GetMtType( ptDstBase[byLoop].GetMtId() )  ||
						( ptDstBase[byLoop].GetMtId() != tSrc.GetMtId() && bIsCanSwitchBrdToMMCU ))
						)
					{
						atDstMt[byLoop].SetNull();
                        continue;
						//bRet &= g_cMpManager.StartSwitchFromBrd(tSrc, bySrcChnnl, 1, &ptDstBase[byLoop],wSpyStartPort);
					}
                }
            }
			bRet &= g_cMpManager.StartSwitchFromBrd(tSrc, bySrcChnnl, byDstMtNum, atDstMt,wSpyStartPort);
        }
        else
        {
			for( byLoop = 0; byLoop < byDstMtNum; byLoop ++ )
            {
				if(  !(MT_TYPE_MMCU != m_ptMtTable->GetMtType( ptDstBase[byLoop].GetMtId() )  ||
						( ptDstBase[byLoop].GetMtId() != tSrc.GetMtId() && bIsCanSwitchBrdToMMCU ) )
						)
				{
					atDstMt[byLoop].SetNull();
                    continue;
					//bRet = g_cMpManager.StartSwitchFromBrd(tSrc, bySrcChnnl, 1, &ptDstBase[byLoop],wSpyStartPort);
				}				
			}
			bRet = g_cMpManager.StartSwitchFromBrd(tSrc, bySrcChnnl, byDstMtNum, atDstMt,wSpyStartPort);
        }
    }
	else
	{
		return FALSE;
	}

    //zbq[04/15/2009] Tand的关键帧编的比较快，且间隔为3s. 故交换建立后再发youareseeing，尽量拿到第一个关键帧
    if (IsDelayVidBrdVCU() && tSrc == tLocalVidBrdSrc)
    {
    }
    else
    {
        //guzh [2008/03/31] 请求关键帧
        if (TYPE_MT == tUnLocalMt.GetType())
        {		
			//如果是发言人请求关键帧，调整Timer,决定请求次数[7/17/2012 chendaiwei]
			if(m_tConf.GetSpeaker().IsLocal() && m_tConf.GetSpeaker().GetMtId() == tUnLocalMt.GetMtId())
			{
				NotifyFastUpdate(tUnLocalMt, MODE_VIDEO, TRUE);
			}
			else
			{
				NotifyFastUpdate(tUnLocalMt, MODE_VIDEO);
			}
		}
    }

    CServMsg cServMsg;
    TPeriEqpStatus tEqpStatus;
    //TMtStatus tMtStatus;
    for (byLoop = 0; byLoop < byDstMtNum; byLoop ++)
    {
        // zbq [06/19/2007] 点名人和被点名人不从广播交换
        if ( !( tLocalVidBrdSrc == m_tVmpEqp && 
                ( ptDstBase[byLoop] == m_tRollCaller ||
                  ptDstBase[byLoop] == GetLocalMtFromOtherMcuMt(m_tRollCallee)) ) )
        {
            NotifyMtReceive( tSrc, ptDstBase[byLoop].GetMtId() );

            //设置终端的码流提示标识        
            if( tSrc == m_tPlayEqp )
            {
                g_cMcuVcApp.GetPeriEqpStatus( m_tPlayEqp.GetEqpId(), &tEqpStatus );
                cServMsg.SetMsgBody( (u8*)tEqpStatus.GetAlias(), MAXLEN_EQP_ALIAS );
                SendMsgToMt( ptDstBase[byLoop].GetMtId(), MCU_MT_MTSOURCENAME_NOTIF, cServMsg );		
            }
            //如果是VMP,则清空码流提示标识
            else if( tSrc == m_tVmpEqp )
            {
                u8 abyVmpAlias[8];
                abyVmpAlias[0] = ' ';
                abyVmpAlias[1] = '\0';
                cServMsg.SetMsgBody( abyVmpAlias, sizeof(abyVmpAlias) );
                SendMsgToMt( ptDstBase[byLoop].GetMtId(), MCU_MT_MTSOURCENAME_NOTIF, cServMsg );		
    	    }

            /* zgc, 2008-05-28, 以下处理在函数开始处进行
            // 清除选看状态
            m_ptMtTable->GetMtStatus(ptDstBase[byLoop].GetMtId(), &tMtStatus);
            tMtStatus.RemoveSelByMcsMode( MODE_VIDEO );
            m_ptMtTable->SetMtStatus(ptDstBase[byLoop].GetMtId(), &tMtStatus);
            */

			if( MT_TYPE_MMCU != m_ptMtTable->GetMtType( ptDstBase[byLoop].GetMtId() )  ||
					( ptDstBase[byLoop].GetMtId() != tSrc.GetMtId() && bIsCanSwitchBrdToMMCU )
						)
			{
				TMt tRealMt;
				tRealMt.SetNull();
				//lukunpeng 2010/07/01 多回传要设置真正的源
				m_ptMtTable->SetMtSrc( ptDstBase[byLoop].GetMtId(), &tUnLocalMt, MODE_VIDEO );
			
			}
        }
        else
        {
            // do nothing
            ConfPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[StartSwitchFromBrd] Mt.%d is in Rollcall, no accept from brd\n", ptDstBase[byLoop].GetMtId() );
        }
    }
    
    MtStatusChange(NULL, TRUE);
    
    return bRet;
}
/*====================================================================
    函数名      StartSwitchToAllNeedAdpWhenChangVid
    功能        ：切换视频广播源时建立适配到需要适配的终端视频交换
    算法实现    ：
    引用全局变量：
    输入参数说明：         
    返回值说明  ：TRUE/FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/03/20    4.0         周嘉麟        创建
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchToAllNeedAdpWhenSrcChanged(TMt tSrc, u8 byDstNum, TMt *ptDstMt, u8 byMode)
{
	if (tSrc.IsNull() || 0 == byDstNum || NULL == ptDstMt)
	{
		return FALSE;
	}

	if (MODE_VIDEO != byMode && MODE_AUDIO != byMode)
	{
		return FALSE;
	}
	TMt tLocalSrc = GetLocalMtFromOtherMcuMt(tSrc);
	TMt tLoopMt;
	u8 byEqpNum = 0;
	TEqp atBas[MAXNUM_PERIEQP];
	memset(atBas, 0, sizeof(atBas));

	//记录几组交换
	u8 byGrpNum = 0;
	//对应某个bas适配某一输出的终端tmt信息
	TMt atDstMt[MAXNUM_BASOUTCHN][MAXNUM_CONF_MT];
	//记录源bas
	TEqp tTempBas;

	if (GetDiffBrdEqp(byMode, byEqpNum, atBas))
	{
		for (u8 byIdx = 0; byIdx < byEqpNum; byIdx++)
		{
			u8 byChnNum = g_cMcuVcApp.GetBasInPutChnNum(atBas[byIdx]);

			for (u8 byChnId = 0; byChnId < byChnNum; byChnId++)
			{
				//[2011/07/05/zhangli]bas开启之后再操作
				if (g_cMcuVcApp.GetBasChnStatus(atBas[byIdx], byChnId) != BASCHN_STATE_RUNNING)
				{
					continue;
				}

				u8 byOutNum = 0;
				u8 byFrontOutNum = 0;
				if ( !GetBasChnOutputNum(atBas[byIdx],byChnId,byOutNum,byFrontOutNum) )
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[StartSwitchToAllNeedAdpWhenSrcChanged] GetBasChnOutputNum:%d-%d failed!\n",
							 atBas[byIdx].GetEqpId(),byChnId
							 );
					return FALSE;
				}

				byGrpNum = 0;	
				tTempBas.SetNull();	
				TSwitchGrp atSwitchGrp[MAXNUM_PERIEQP];
			    memset(atDstMt, 0, sizeof(atDstMt));

				for (u8 byOutId = 0; byOutId < byOutNum; byOutId++)
				{
					//某一bas输出有效的终端数
				    u8 byVailedNum = 0;

					for (u8 byLoop = 0; byLoop < byDstNum; byLoop++)
					{ 
						tLoopMt = GetLocalMtFromOtherMcuMt(ptDstMt[byLoop]);

						if (!m_tConfAllMtInfo.MtJoinedConf(tLoopMt.GetMtId()))
						{
							continue;
						}

						if (ptDstMt[byLoop] == GetVidBrdSrc())
						{
							continue;
						}

						TBasOutInfo tOutInfo;
 						if (IsNeedAdapt(tLocalSrc, tLoopMt, byMode) &&
							FindBasChn2BrdForMt(tLoopMt.GetMtId(), byMode, tOutInfo) &&
							tOutInfo.m_tBasEqp.GetEqpId() == atBas[byIdx].GetEqpId() &&
							tOutInfo.m_byChnId == byChnId && 
							 tOutInfo.m_byOutIdx== byOutId)
 						{
							if (tTempBas.IsNull())
							{
								tTempBas = tOutInfo.m_tBasEqp;
								tTempBas.SetConfIdx(m_byConfIdx);
							}

							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "[StartSwitchToAllNeedAdpWhenSrcChanged] Src<BasId.%d, ChnId.%d, OutId.%d>->DstMt.%d!\n",
													    atBas[byIdx].GetEqpId(),
													    byChnId, byOutId, 
													    tLoopMt.GetMtId());

							atDstMt[byOutId][byVailedNum] = tLoopMt;
							byVailedNum++;
						}									
					}
					//某一适配通道的某一输出的目的终端数不为0
					if (byVailedNum != 0)
					{
						for (u8 byGrpIdx = 0; byGrpIdx < MAXNUM_PERIEQP; byGrpIdx++)
						{
							if (atSwitchGrp[byGrpIdx].IsNull())
							{
								byGrpNum ++;
								atSwitchGrp[byGrpIdx].SetSrcChnl(byFrontOutNum + byOutId);
								atSwitchGrp[byGrpIdx].SetDstMtNum(byVailedNum);
								atSwitchGrp[byGrpIdx].SetDstMt(atDstMt[byOutId]);
								break;
							}
						}
					}
				}
				if (!tTempBas.IsNull() && byGrpNum > 0)
				{
					for (u8 byGrpIdx = 0; byGrpIdx < byGrpNum; byGrpIdx++)
					{
						for (u8 byDstIdx = 0; byDstIdx < atSwitchGrp[byGrpIdx].GetDstMtNum(); byDstIdx++)
						{
							TMt *ptMt = atSwitchGrp[byGrpIdx].GetDstMt();
							if (ptMt != NULL)
							{
								ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[StartSwitchToAllNeedAdpWhenChangVid] DstMtId:%d\n", 
											ptMt[byDstIdx].GetMtId());
							}			
						}
					}					
					StartSwitchToAll(tTempBas, byGrpNum, atSwitchGrp, byMode);
				}
			}
		}
	}
	return TRUE;
}
/*====================================================================
    函数名      ：StartSwitchToAllSubMtJoinedConf
    功能        ：将指定终端主视频广播到会议中所有与会直连终端
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt & tSrc, 源
				  u8 bySrcChnnl, 源的信道号
                  const TMt &tOldSrc, 原来的发言人(暂未使用)
                  BOOL32 bForce, 是否强制立即切换                  
    返回值说明  ：TRUE/FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/03/20    4.0         顾振华        创建
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchToAllSubMtJoinedConf(const TMt &tSrc, u8 bySrcChnnl, const TMt &tOldSrc, BOOL32 bForce, u16 wSpyStartPort )
{

	TMt tLocalSrc = GetLocalMtFromOtherMcuMt(tSrc);

    // 添加广播源
    if ( TYPE_MT == tLocalSrc.GetType() )
    {
        if ( !m_ptMtTable->IsLogicChnnlOpen(tLocalSrc.GetMtId(), LOGCHL_VIDEO, FALSE) )
        {
            ConfPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "[StartSwitchToAllSubMtJoinedConf]Source MT.%d is not sending video! tOldSrc<McuId.%u MtId.%u>\n",tLocalSrc.GetMtId(),tOldSrc.GetMcuId(),tOldSrc.GetMtId());
            return FALSE;
        }
    }

    
    TMtStatus tStatus;


	//过适配交换打包
	TMt atMtListNeedAdp[MAXNUM_CONF_MT];
	u8 byNeedAdpDstMtNum = 0;
	
	//不过适配交换打包
    TMt atMtList[MAXNUM_CONF_MT];
    u8 byDstMtNum = 0;

    u8 byLoop = 1;
    TMt tLoopMt;

    for ( ; byLoop <= MAXNUM_CONF_MT; byLoop++ )
    {

		// 主席优先级最高，只要有主席选看源的终端都会continue
		if (m_tConf.GetChairman().GetMtId() == byLoop)
		{
			if( m_ptMtTable->GetMtStatus(byLoop,&tStatus) &&
				!tStatus.GetSelectMt(MODE_VIDEO).IsNull() )
			{
				continue;
			}
			
		}
        //跳过源终端自己
		tLoopMt = m_ptMtTable->GetMt(byLoop);
        if ( !m_tConfAllMtInfo.MtJoinedConf(byLoop) || 
             ( tSrc == tLoopMt && 
               tSrc.GetType() == TYPE_MT ) )
        {
			//跳过自己的时候，要通知终端看自己；否则会从组播地址收自己的码流
			if ( tSrc == tLoopMt
				&& tSrc.GetType() == TYPE_MT )
			{
				if ( IsMultiCastMt(byLoop) )
				{
					ChangeSatDConfMtRcvAddr(byLoop, MODE_VIDEO, FALSE);
				}
				NotifyMtReceive(tSrc, byLoop);
			}
            continue;
        }

		// 对于VCS会议,主席轮询时,主席不看广播源
		if (VCS_CONF == m_tConf.GetConfSource() &&
			m_tConf.GetChairman().GetMtId() == byLoop &&
			VCS_POLL_START == m_cVCSConfStatus.GetChairPollState())
		{
			continue;
		}

	

        //[2013/01/22 chenbing] 卫星终端，不建交换
		//pgf新方案：不能接收的卫星终端，要跳过，能接收的，和普通终端一样建交换
		if ( IsMultiCastMt(byLoop) && !IsCanSatMtReceiveBrdSrc(tLoopMt))
		{
			ChangeSatDConfMtRcvAddr(byLoop, MODE_VIDEO, FALSE);
			NotifyMtReceive(tLoopMt, byLoop);
			continue;
		}

		// [1/20/2010 xliang] 上传轮询过滤
		//zhouyiliang 20120328 上级mcu跳过
		if( MT_TYPE_MMCU == tLoopMt.GetMtType() && m_tCascadeMMCU == tLoopMt )
		{
		
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[StartSwitchToAllSubMtJoinedConf] brd to MMCU should be skipped !\n");
			continue; 
			
		}

		// vrs新录播过滤
		if (m_ptMtTable->GetMtType(byLoop) == MT_TYPE_VRSREC)
		{
			continue;
		}

        if ( !CanMtRecvVideo( tLoopMt, tLocalSrc ) )
        {			
			//[nizhijun 2010/11/19] 这里需要和非local的广播源比较
			if(!(tLoopMt == GetVidBrdSrc()))
			{
				//zbq[01/01/2009] 高清适配策略调整：从适配接收码流单独建立，不再自动起适配
				if (IsNeedAdapt(tLocalSrc, tLoopMt, MODE_VIDEO)/*!(IsHDConf(m_tConf) && m_cMtRcvGrp.IsMtNeedAdp(tLoopMt.GetMtId()))*/)
				{
					atMtListNeedAdp[byNeedAdpDstMtNum] = tLoopMt;
					byNeedAdpDstMtNum ++;
				//	StartSwitchToSubMt( tSrc, bySrcChnnl, byLoop, MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE, FALSE, FALSE);
				}
			}
			// [10/21/2010 xliang] 拆广播源到该终端的交换，并通知该终端看自己
			/* eg:
			Mt1,Mt2 in conf
			poll Mt1:
			Mt1--->Brd
			Brd--->Mt2
			poll Mt2:
			Mt1-X->Brd
			Mt2--->Brd
			Brd--->Mt1
			Brd-X->Mt2    this switch is removed by the following codes
			*/
			else if(CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode()
				&& MODE_VIDEO == m_tConf.m_tStatus.GetPollInfo()->GetMediaMode()
				)
			{		
				NotifyMtReceive(tLoopMt, tLoopMt.GetMtId());
				//zjl 20110510 StopSwitchToSubMt 接口重载替换
				//StopSwitchToSubMt( tLoopMt.GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE );
				StopSwitchToSubMt(1, &tLoopMt, MODE_VIDEO, FALSE);	
			}            
			
			continue;
		}
        
        atMtList[byDstMtNum] = tLoopMt;
        byDstMtNum ++;
    }
	
	if (byNeedAdpDstMtNum > 0)
	{
		StartSwitchToAllNeedAdpWhenSrcChanged(tSrc, byNeedAdpDstMtNum, atMtListNeedAdp, MODE_VIDEO);
	}
	
	//[nizhijun 2010/10/22] 建rtcp交换
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	if (tConfAttrb.IsResendLosePack())
	{
		StartBasPrsSupportEx(MODE_VIDEO);
	}
    //建立交换
    
    //高清部分
    if ( byDstMtNum > 0)
    {		
        StartSwitchFromBrd(tSrc, bySrcChnnl, byDstMtNum, atMtList, wSpyStartPort);
    }
    
    //发言人自身的码流交换处理：根据会议属性进行相应回传设置
	ChangeSpeakerSrc( MODE_VIDEO, emReasonChangeBrdSrc,wSpyStartPort );	 
    MtStatusChange();
    
	return TRUE;
//	TMt tLocalSrc = GetLocalMtFromOtherMcuMt(tSrc);
//
//    // 添加广播源
//    if ( TYPE_MT == tLocalSrc.GetType() )
//    {
//        if ( !m_ptMtTable->IsLogicChnnlOpen(tLocalSrc.GetMtId(), LOGCHL_VIDEO, FALSE) )
//        {
//            ConfPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "[StartSwitchToAllSubMtJoinedConf]Source MT.%d is not sending video! tOldSrc<McuId.%u MtId.%u>\n",tLocalSrc.GetMtId(),tOldSrc.GetMcuId(),tOldSrc.GetMtId());
//            return FALSE;
//        }
//    }
//
//	// [2013/01/22 chenbing] 卫星分散会议
//	if ( m_tConf.GetConfAttrb().IsSatDCastMode())
//	{
//		//组播会议新功能:同时也有广播源到卫星地址的交换
//		g_cMpManager.StartDistrConfCast(tSrc, MODE_VIDEO, bySrcChnnl);
//    }
//
//    g_cMpManager.StartSwitchToBrd(tLocalSrc, bySrcChnnl, bForce, wSpyStartPort);
//    
//    TMtStatus tStatus;
//
//	//老vmp广播打包交换
//	TMt atOldVmpDstMt[MAXNUM_CONF_MT];
//	u8  byOldVmpDstMtNum = 0;
//
//	//过适配交换打包
//	TMt atMtListNeedAdp[MAXNUM_CONF_MT];
//	u8 byNeedAdpDstMtNum = 0;
//	
//	//不过适配交换打包
//    TMt atMtList[MAXNUM_CONF_MT];
//    u8 byDstMtNum = 0;
//
//    u8 byLoop = 1;
//    TMt tLoopMt;
//
//    for ( ; byLoop <= MAXNUM_CONF_MT; byLoop++ )
//    {
//
//		// 主席优先级最高，只要有主席选看源的终端都会continue
//		if (m_tConf.GetChairman().GetMtId() == byLoop)
//		{
//			if( m_ptMtTable->GetMtStatus(byLoop,&tStatus) &&
//				!tStatus.GetSelectMt(MODE_VIDEO).IsNull() )
//			{
//				continue;
//			}
//			
//			// 主席选看画面合成时,不看广播源
//			if (IsVmpSeeByChairman())
//			{
//				continue;
//			}
//		}
//        //跳过源终端自己
//		tLoopMt = m_ptMtTable->GetMt(byLoop);
//        if ( !m_tConfAllMtInfo.MtJoinedConf(byLoop) || 
//             ( tSrc == tLoopMt && 
//               tSrc.GetType() == TYPE_MT ) )
//        {
//			//跳过自己的时候，要通知终端看自己；否则会从组播地址收自己的码流
//			if ( tSrc == tLoopMt
//				&& tSrc.GetType() == TYPE_MT )
//			{
//				if ( IsMultiCastMt(byLoop) )
//				{
//					ChangeSatDConfMtRcvAddr(byLoop, MODE_VIDEO, FALSE);
//				}
//				NotifyMtReceive(tSrc, byLoop);
//			}
//            continue;
//        }
//
//		// 对于VCS会议,主席轮询时,主席不看广播源
//		if (VCS_CONF == m_tConf.GetConfSource() &&
//			m_tConf.GetChairman().GetMtId() == byLoop &&
//			VCS_POLL_START == m_cVCSConfStatus.GetChairPollState())
//		{
//			continue;
//		}
//
//	
//
//        //[2013/01/22 chenbing] 卫星终端，不建交换
//		//pgf新方案：不能接收的卫星终端，要跳过，能接收的，和普通终端一样建交换
//		if ( IsMultiCastMt(byLoop) && !IsCanSatMtReceiveBrdSrc(tLoopMt))
//		{
//			ChangeSatDConfMtRcvAddr(byLoop, MODE_VIDEO, FALSE);
//			NotifyMtReceive(tLoopMt, byLoop);
//			continue;
//		}
//
//		// [1/20/2010 xliang] 上传轮询过滤
//		//zhouyiliang 20120328 上级mcu跳过
//		if( MT_TYPE_MMCU == tLoopMt.GetMtType() && m_tCascadeMMCU == tLoopMt )
//		{
//		
//			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[StartSwitchToAllSubMtJoinedConf] brd to MMCU should be skipped !\n");
//			continue; 
//			
//		}
//
//		
//
//        if ( !CanMtRecvVideo( tLoopMt, tLocalSrc ) )
//        {
//			// [10/21/2010 xliang] old vmp第2路的交换
//			TSimCapSet tSimCapSet = m_ptMtTable->GetDstSCS( byLoop );
//			if( MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType() 
//				&& ( tLocalSrc == m_tVmpEqp && VMP == GetVmpSubType(m_tVmpEqp.GetEqpId()) )
//				&& (tSimCapSet.GetVideoMediaType() == m_tConf.GetSecVideoMediaType() )
//				)
//			{
//				//zjl 20110510 StartSwitchToAll 替换 StartSwitchToSubMt
//				//StartSwitchToSubMt(tSrc, 1, byLoop, MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE, FALSE);
//				//g_cMpManager.StartSwitchToSubMt(tSrc, 1, tLoopMt, MODE_VIDEO);
//				atOldVmpDstMt[byOldVmpDstMtNum] = tLoopMt;
//				byOldVmpDstMtNum++;
//			}
//			else 
//			{
//				//[nizhijun 2010/11/19] 这里需要和非local的广播源比较
//				if(!(tLoopMt == GetVidBrdSrc()))
//				{
//					//zbq[01/01/2009] 高清适配策略调整：从适配接收码流单独建立，不再自动起适配
//					if (IsNeedAdapt(tLocalSrc, tLoopMt, MODE_VIDEO)/*!(IsHDConf(m_tConf) && m_cMtRcvGrp.IsMtNeedAdp(tLoopMt.GetMtId()))*/)
//					{
//						atMtListNeedAdp[byNeedAdpDstMtNum] = tLoopMt;
//						byNeedAdpDstMtNum ++;
//					//	StartSwitchToSubMt( tSrc, bySrcChnnl, byLoop, MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE, FALSE, FALSE);
//					}
//				}
//				// [10/21/2010 xliang] 拆广播源到该终端的交换，并通知该终端看自己
//				/* eg:
//				Mt1,Mt2 in conf
//				poll Mt1:
//				Mt1--->Brd
//				Brd--->Mt2
//				poll Mt2:
//				Mt1-X->Brd
//				Mt2--->Brd
//				Brd--->Mt1
//				Brd-X->Mt2    this switch is removed by the following codes
//				*/
//				else if(CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollMode()
//					&& MODE_VIDEO == m_tConf.m_tStatus.GetPollInfo()->GetMediaMode()
//					)
//				{		
//					NotifyMtReceive(tLoopMt, tLoopMt.GetMtId());
//					//zjl 20110510 StopSwitchToSubMt 接口重载替换
//					//StopSwitchToSubMt( tLoopMt.GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE );
//					StopSwitchToSubMt(1, &tLoopMt, MODE_VIDEO, FALSE);
//
//					
//					
//				}
//			}
//            
//            continue;
//        }
//        
//        atMtList[byDstMtNum] = tLoopMt;
//        byDstMtNum ++;
//    }
//	
//	if (byOldVmpDstMtNum > 0)
//	{
//		TSwitchGrp tSwitchGrp;
//		tSwitchGrp.SetSrcChnl(1);
//		tSwitchGrp.SetDstMtNum(byOldVmpDstMtNum);
//		tSwitchGrp.SetDstMt(atOldVmpDstMt);
//		StartSwitchToAll(tSrc, 1, &tSwitchGrp, MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE, FALSE);
//	}
//	
//	if (byNeedAdpDstMtNum > 0)
//	{
//		StartSwitchToAllNeedAdpWhenSrcChanged(tSrc, byNeedAdpDstMtNum, atMtListNeedAdp, MODE_VIDEO);
//	}
//	
//	//[nizhijun 2010/10/22] 建rtcp交换
//	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
//	if (tConfAttrb.IsResendLosePack())
//	{
//		StartBasPrsSupportEx(MODE_VIDEO);
//	}
//    //建立交换
//    
//    //高清部分
//    if ( byDstMtNum > 0)
//    {		
//        StartSwitchFromBrd(tSrc, bySrcChnnl, byDstMtNum, atMtList, wSpyStartPort);
//    }
//    
//    //发言人自身的码流交换处理：根据会议属性进行相应回传设置
//	ChangeSpeakerSrc( MODE_VIDEO, emReasonChangeBrdSrc,wSpyStartPort );	 
//    MtStatusChange();
//    
//	return TRUE;
}

/*====================================================================
    函数名      ：StartSwitchToAllSubMtJoinedConf
    功能        ：将指定终端音频交换到会议中所有与会直连终端
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt & tSrc, 源
				  u8 bySrcChnnl, 源的信道号
    返回值说明  ：TRUE/FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/12    1.0         LI Yi         创建
	03/07/23    2.0         胡昌威        修改
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchToAllSubMtJoinedConf(const TMt &tSrc, u8 bySrcChnnl, u16 wSpyStartPort)
{
	u8	byLoop;
    BOOL32 bResultAnd = TRUE;
	TMt tTempSrc;
	//u8  tTempSrcChn;
	//[nizhijun 2010/12/04] 建交换，使用临时的SypStartPort
	u16 tTempSypStartPort;

	u8 byGlobalSpeakerId = 0;
	if (!m_tCascadeMMCU.IsNull() && tSrc == m_tConf.GetSpeaker() &&
					m_tCascadeMMCU == tSrc )
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
		
		if( ptConfMcInfo != NULL						
			&& ptConfMcInfo->m_tConfAudioInfo.m_byMixerCount == 1
			&& ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.IsMcuIdLocal()
			)
		{			
			byGlobalSpeakerId = ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId();
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_CONF,"[StartSwitchToAllSubMtJoinedConf] Mt(%d) is GlobalSpeaker,so not switch audio to it.\n",byGlobalSpeakerId );
		}
	}

	//唇音同步交换组打包
	TMt atVADstMt[MAXNUM_CONF_MT];
	TSwitchGrp tSwitchGrpVA;
	u8 byVADstNum = 0;
	TEqp tVABas;
	u8 byVAChnId  = 0;
	u8 byVAOutNum = 0;
	u8 byVAOutIdx = 0;
	BOOL32 bVABasOut = TRUE;
	if(g_cMcuVcApp.IsVASimultaneous() &&
		GetAudBufOutPutInfo(tVABas, byVAChnId, byVAOutNum, byVAOutIdx))
	{
		tVABas.SetConfIdx(m_byConfIdx);
		tSwitchGrpVA.SetSrcChnl(byVAChnId * byVAOutNum);
	}
	else
	{
		bVABasOut = FALSE;
	}

	//音频适配交换组打包
	TMt atDstMtNeedAudAdp[MAXNUM_CONF_MT];
	u8 byNeedAudAdpDstNum = 0;
	
	//普通交换组打包
	TSwitchGrp tSwitchGrp;
	tSwitchGrp.SetSrcChnl( bySrcChnnl );
	
	u8 byDstMtNum = 0;
	TMt atDstMt[MAXNUM_CONF_MT];
	memset( atDstMt, 0, sizeof(atDstMt) );

	TMtStatus tMtStatus;

    for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
    {
		tTempSypStartPort = wSpyStartPort;
		tTempSrc    = tSrc;
	
		if( byGlobalSpeakerId == byLoop )
		{
			continue;
		}
        if( m_tConfAllMtInfo.MtJoinedConf( byLoop ))
        {
			if (m_ptMtTable->GetMtType(byLoop) == MT_TYPE_VRSREC)
			{
				//跳过vrs新录播实体
				continue;
			}

			if( m_ptMtTable->GetMtStatus(byLoop,&tMtStatus) &&
				!tMtStatus.GetSelectMt(MODE_AUDIO).IsNull() )
			{
				continue;
			}

//             //卫星分散会议组播终端由组播地址接收码流，不建交换
//pgf:不能收的跳过，能收的建交换
			if ( IsMultiCastMt(byLoop) && !IsCanSatMtReceiveBrdSrc(m_ptMtTable->GetMt(byLoop), MODE_AUDIO))
			{
				continue;
			}


			// 调整发言人源
			if ( byLoop == GetLocalSpeaker().GetMtId() && 
				m_ptMtTable->GetMtType( GetLocalSpeaker().GetMtId() ) == MT_TYPE_MT 
				)
			{
				//新发言人码流源调整逻辑, zgc, 2008-04-12
				ChangeSpeakerSrc( MODE_AUDIO, emReasonChangeBrdSrc,tTempSypStartPort );
			}
            else
            {            	
				if(IsNeedAdapt(GetLocalAudBrdSrc(), m_ptMtTable->GetMt(byLoop), MODE_AUDIO))
				{
					atDstMtNeedAudAdp[byNeedAudAdpDstNum] = m_ptMtTable->GetMt(byLoop);
					byNeedAudAdpDstNum++;
				}	
				else
				{
				//[03/01/2010] zjl modify 8000e 不支持唇音同步，音频码流不过bas，直接交换到目的终端               
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
					//zjl20101028唇音同步走配置文件,并且和发言人绑定
					if(g_cMcuVcApp.IsVASimultaneous() && !m_tConf.GetSpeaker().IsNull()
						&& tSrc == GetVidBrdSrc() )	// 2011-8-25 add by pgf:添加过滤条件，当音视频同源，则建立唇音同步，否则建立普通交换					
					{
						//目前只有mpu和mpu_h支持唇音同步，如果视频要适配并且非mpu或mpu_h，同时音频也不需要适配，
						//则直接建立音频交换到目的终端
						TLogicalChannel tVidChn;
						TPeriEqpStatus tBasStatus;
						TBasOutInfo	tOutInfo;
						BOOL32 bNeedVidAdp =  (m_ptMtTable->GetMtLogicChnnl(byLoop, LOGCHL_VIDEO, &tVidChn, TRUE) &&
							IsNeedAdapt(m_tConf.GetSpeaker(), m_ptMtTable->GetMt(byLoop), MODE_VIDEO) &&
							FindBasChn2BrdForMt(byLoop, MODE_VIDEO, tOutInfo) &&
							g_cMcuVcApp.GetPeriEqpStatus(tOutInfo.m_tBasEqp.GetEqpId(), &tBasStatus) &&
							( g_cMcuAgent.IsEqpBasHD( tOutInfo.m_tBasEqp.GetEqpId() ) ) &&
							(tBasStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU ||
							tBasStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU_H ||
							tBasStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU2_BASIC ||
							tBasStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU2_ENHANCED)
							);
						
						//统计需要唇音同步的目的终端
						//1.视频过适配
						//2.找到音频缓存输出(没找到则音频码流直接交换)
						if (bNeedVidAdp && bVABasOut)
						{
							atVADstMt[byVADstNum] = m_ptMtTable->GetMt(byLoop);
							byVADstNum ++;
							continue;
						}
					}
#endif		 
					//统计不过适配，音频码流直接交换的目的终端
					atDstMt[byDstMtNum] = m_ptMtTable->GetMt(byLoop);
					byDstMtNum++;
				}
			}
		}
	}

	//唇音同步交换打包
	if (byVADstNum > 0)
	{
		tSwitchGrpVA.SetDstMtNum(byVADstNum);
		tSwitchGrpVA.SetDstMt(atVADstMt);
		StartSwitchToAll( tVABas, 1, &tSwitchGrpVA, MODE_AUDIO);
	}	

// 	//适配交换打包
// 	if (byNeedAudAdpDstNum > 0)
// 	{
// 		StartSwitchToAllNeedAdpWhenSrcChanged(tSrc, byNeedAudAdpDstNum, atDstMtNeedAudAdp, MODE_AUDIO);
// 		TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
// 		if (tConfAttrb.IsResendLosePack())
// 		{
// 			StartBasPrsSupportEx(MODE_AUDIO);
// 		}		
// 	}

	//普通交换打包
	if( byDstMtNum > 0 )
	{
		tSwitchGrp.SetDstMtNum( byDstMtNum );
		tSwitchGrp.SetDstMt( atDstMt );
		StartSwitchToAll( tTempSrc, 1, &tSwitchGrp, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE, FALSE, tTempSypStartPort);
	}
    
    MtStatusChange();
    
	return bResultAnd;
}

/*====================================================================
    函数名      StartSwitchToSubMtNeedAdp
    功能        ：将数据交换到会议中 接本适配通道的所有终端
    算法实现    ：
    引用全局变量：
    输入参数说明：BOOL32 bIsConsiderBiteRate 是否只考虑码率适配
						默认为FALSE，说明只要需要适配就建立交换
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
  2010-06-30    4.6         周嘉麟		   创建
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchToSubMtNeedAdp(const TEqp &tEqp, u8 byChnId, BOOL32 bIsConsiderBiteRate /*= FALSE*/)
{

	u8 byMode = GetBasChnMode(tEqp, byChnId);
	if (MODE_NONE == byMode)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[StartSwitchToSubMtNeedAdpt] byMode is MODE_NONE!\n");
		return FALSE;
	}
	
	TMt tSrc = GetBasChnSrc(tEqp, byChnId);
	if (tSrc.IsNull())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[StartSwitchToSubMtNeedAdpt] EqpId:%d + ChnId:%d 's Src is null!\n", tEqp.GetEqpId(), byChnId);
		return FALSE;
	}
	
	TBasOutInfo tOutInfo;
	TMt tDst;
	tDst.SetNull();

	//交换有效组数
	u8 byGrpNum = 0;
	//交换组
	TSwitchGrp atSwitchGrp[MAXNUM_PERIEQP];
	memset(atSwitchGrp, 0, sizeof(atSwitchGrp));
	//每一路适配输出的有效终端数
	u8 byVailedMtNum = 0;

	TMt atDstMt[MAXNUM_BASOUTCHN][MAXNUM_CONF_MT];
	memset(atDstMt, 0, sizeof(atDstMt));

	//适配输出路数
	u8 byChnOutNum = 0;
	u8 byFrontOutNum = 0;
	if ( !GetBasChnOutputNum(tEqp,byChnId,byChnOutNum,byFrontOutNum) )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[StartSwitchToSubMtNeedAdpt] GetBasChnOutputNum:%d-%d failed!\n",
				  tEqp.GetEqpId(),byChnId
				  );
		return FALSE;
	}

	TMtStatus tMtStatus;
	TMt tSelSrc;
	for (u8 bySrcOutIdx = 0; bySrcOutIdx < byChnOutNum; bySrcOutIdx++)
	{
		//对应适配某一输出的终端id
		byVailedMtNum = 0;

		for (u8 byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx++)
		{
			if (!m_tConfAllMtInfo.MtJoinedConf(byMtIdx))
			{
				continue;
			}
			//不交换给自己,但要交换给下级mcu
			if ( tSrc == m_ptMtTable->GetMt(byMtIdx) &&
				MT_TYPE_SMCU != m_ptMtTable->GetMtType(byMtIdx) )
			{
				continue;
			}
			
			//不交换给上级mcu
			if (m_tCascadeMMCU.GetMtId() == byMtIdx)
			{
				continue;
			}

			// 不交换给终端录像的vrs新录播实体
			if (m_ptMtTable->GetMtType(byMtIdx) == MT_TYPE_VRSREC)
			{
				if (m_ptMtTable->GetRecChlType(byMtIdx) == TRecChnnlStatus::TYPE_RECORD
					&& !m_ptMtTable->GetRecSrc(byMtIdx).IsNull())
				{
					continue;
				}
			}

			if (m_tConf.GetChairman().GetMtId() == byMtIdx)
			{
				m_ptMtTable->GetMtStatus(byMtIdx, &tMtStatus);
				tSelSrc = tMtStatus.GetSelectMt( byMode );
				// 新策略,主席有选看源时,不看广播源
				if (!tSelSrc.IsNull())
				{
					continue;
				}				
			}

			// [2013/01/22 chenbing] 新策略，卫星分散会议下，卫星终端只接收组播地址码流，
			//不建适配到卫星终端的交换
			if(	IsMultiCastMt(byMtIdx) )
			{
				continue;
			}

			tOutInfo.clear();
			tDst = m_ptMtTable->GetMt(byMtIdx);
			if (!IsNeedAdapt(tSrc, tDst, byMode))
			{
				continue;
			}

			//如果只考虑码率适配，那么其他交换就不建立了
			//该场景使用于，停双流时，补建主流的码率适配交换
			if ( bIsConsiderBiteRate )
			{
				TSimCapSet  tMVSrcCap;
				TSimCapSet  tMVDstCap;
				TMt tTempSrc;
				TMt tTempDst;
				tTempSrc = GetLocalMtFromOtherMcuMt(tSrc);
				tMVSrcCap = m_ptMtTable->GetSrcSCS(tTempSrc.GetMtId());
				tTempDst = GetLocalMtFromOtherMcuMt(tDst);
				tMVDstCap = m_ptMtTable->GetDstSCS(tTempDst.GetMtId());
				
				//如果存在其他类型适配：格式、分辨率、帧率、profiletype 那么说明就没必要建立交换了
				//否则说明是只是码率适配，那么需要补建交换
				if ( tMVSrcCap.GetVideoMaxBitRate() <= tMVDstCap.GetVideoMaxBitRate() ||
					tMVSrcCap.GetVideoMediaType() != tMVDstCap.GetVideoMediaType() ||
					tMVSrcCap.GetVideoProfileType() != tMVDstCap.GetVideoProfileType() ||
					tMVDstCap.GetVideoCap().IsResLower( tMVDstCap.GetVideoResolution(), tMVSrcCap.GetVideoResolution() ) ||
					( tMVSrcCap.GetVideoMediaType() == MEDIA_TYPE_H264 && tMVDstCap.GetVideoMediaType() == MEDIA_TYPE_H264 &&
					tMVDstCap.GetVideoCap().IsH264CapLower( tMVSrcCap.GetVideoResolution(),tMVSrcCap.GetUserDefFrameRate() )
					)||
					( tMVSrcCap.GetVideoMediaType() != MEDIA_TYPE_H264 && tMVDstCap.GetVideoMediaType() != MEDIA_TYPE_H264 &&
					tMVSrcCap.GetVideoFrameRate() > 	tMVDstCap.GetVideoFrameRate()	
					)
					)		
				{
					continue;
				}	
				else
				{
					//对于下级MCU，如果源是下级，同时目的也是下级的的话，因为下级主流接收码率的调整，不再stopdoublestream里，
					//而是通过发flowcontrol来控制，所以这里会造成下级发给下级过码率适配情况，那么也需要过滤
					if ( tTempSrc == tTempDst &&
						tTempDst.GetMtType() == MT_TYPE_SMCU 
						)
					{
						continue;
					}
					ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_BAS,"[StartSwitchToSubMtNeedAdp]tMVSrcCap:%d-%d-%d-%d-%d,tMVSrcCap:%d-%d-%d-%d-%d\n",
						tMVSrcCap.GetVideoMediaType(),tMVSrcCap.GetVideoResolution(),tMVSrcCap.GetUserDefFrameRate(),tMVSrcCap.GetVideoProfileType(),tMVSrcCap.GetVideoMaxBitRate(),
						tMVDstCap.GetVideoMediaType(),tMVDstCap.GetVideoResolution(),tMVDstCap.GetUserDefFrameRate(),tMVDstCap.GetVideoProfileType(),tMVDstCap.GetVideoMaxBitRate()
						);
					ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_BAS,"[StartSwitchToSubMtNeedAdp] tSrc:%d-%d bIsConsiderBiteRate,tEqp:%d,chnid:%d build switch to tmt:%d-%d\n",
						tTempSrc.GetMcuId(),tTempSrc.GetMtId(),
						tEqp.GetEqpId(),byChnId,tTempDst.GetMcuId(),tTempDst.GetMtId()
						);
				}
			}

			if(!FindBasChn2BrdForMt(byMtIdx, byMode, tOutInfo))
			{
				continue;
			}
	
			//保證不重複建立交換
			if (tOutInfo.m_tBasEqp.GetEqpId() != tEqp.GetEqpId() 
				|| tOutInfo.m_byChnId != byChnId 
				|| tOutInfo.m_byOutIdx != bySrcOutIdx
				)
			{
				continue;
			}
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "[StartSwitchToSubMtNeedAdp] Src<BasId.%d, ChnId.%d, OutId.%d>->DstMt.%d!\n",
										tEqp.GetEqpId(), byChnId, bySrcOutIdx, byMtIdx);

			atDstMt[bySrcOutIdx][byVailedMtNum] = tDst;
			byVailedMtNum ++;
		}
		if (byVailedMtNum > 0)
		{
			atSwitchGrp[byGrpNum].SetSrcChnl(byFrontOutNum + bySrcOutIdx);
			atSwitchGrp[byGrpNum].SetDstMtNum(byVailedMtNum);
			atSwitchGrp[byGrpNum].SetDstMt(atDstMt[bySrcOutIdx]);
			byGrpNum ++;	
		}	
	}
// 	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "[StartSwitchToSubMtNeedAdpt] --------------GrpNum:%d--------------!\n", byGrpNum);
// 	for (u8 byGrpIdx = 0; byGrpIdx < byGrpNum; byGrpIdx++)
// 	{
// 		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[StartSwitchToSubMtNeedAdpt] Grp:%d, SrcChn:%d, DstNum:%d!\n",
// 					byGrpIdx, atSwitchGrp[byGrpIdx].GetSrcChnl(), atSwitchGrp[byGrpIdx].GetDstMtNum());
// 		for (u8 byDstIdx = 0; byDstIdx < atSwitchGrp[byGrpIdx].GetDstMtNum(); byDstIdx++)
// 		{
// 			TMt *ptMt = atSwitchGrp[byGrpIdx].GetDstMt();
// 			if (ptMt != NULL)
// 			{
// 				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[StartSwitchToSubMtNeedAdpt] DstMtId:%d\n", ptMt[byDstIdx].GetMtId());
// 			}			
// 		}
// 	}
	StartSwitchToAll(tEqp, byGrpNum, atSwitchGrp, byMode);
	//请求关键帧

	if ( byMode != MODE_AUDIO )
	{
		NotifyFastUpdate(tEqp, byChnId, TRUE);
	}
	return TRUE;
}

/*====================================================================
    函数名          StopSwitchToSubMtNeedAdp
    功能        ：将数据停止交换到会议中 接本适配通道的某终端
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2010-6-30   4.6         周嘉麟		   创建
====================================================================*/
BOOL32 CMcuVcInst::StopSwitchToSubMtNeedAdp(const TEqp &tEqp, u8 byChnId,BOOL32 bSelSelf /*= TRUE*/)
{

	u8 byMode = GetBasChnMode(tEqp, byChnId);
	//[nizhijun 2010/9/25] 判断是否是音频伴随
	TPeriEqpStatus tBasStatus;
	BOOL32 bStopAud =  ( g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tBasStatus) ) &&
					   ( g_cMcuAgent.IsEqpBasHD( tEqp.GetEqpId() ) ) &&
					   ( tBasStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU ||
					     tBasStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU_H ||
						 tBasStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU2_BASIC ||
					     tBasStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU2_ENHANCED 
						 )     &&
					   (byMode == MODE_VIDEO);
	if (MODE_NONE == byMode)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[StopSwitchToSubMtNeedAdpt] byMode is MODE_NONE!\n");
		return FALSE;
	}
	
	TMt tSrc = GetBasChnSrc(tEqp, byChnId);

	if (tSrc.IsNull())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[StopSwitchToSubMtNeedAdpt] EqpId:%d + ChnId:%d 's Src is null!\n", tEqp.GetEqpId(), byChnId);
		return FALSE;
	}
	
	TBasOutInfo tOutInfo;
	TMt tDst;
	tDst.SetNull();
	TMt tDstSrc;
	tDstSrc.SetNull();

	TMt atDst[MAXNUM_CONF_MT];
	u8 byMtNum = 0;
	TMt atDstAud[MAXNUM_CONF_MT];
	u8 byAudMtNum = 0;

	for (u8 byMtIdx = 1; byMtIdx <= MAXNUM_CONF_MT; byMtIdx++)
	{
		if (!m_tConfAllMtInfo.MtJoinedConf(byMtIdx))
		{
			continue;
		}

		//[2011/09/21/zhangli]如果是上级MCU，则过滤;Bug00065099,上传和发言人相同时且上传过适配，取消发言人，上传通道被清空
		if (byMtIdx == m_tCascadeMMCU.GetMtId())
		{
			continue;
 		}

		tOutInfo.clear();
		tDst = m_ptMtTable->GetMt(byMtIdx);

		//[nizhijun 2010/9/10]
// 		if(!(m_ptMtTable->GetMtStatus(byMtIdx, &tDstStatus) &&
// 		     tDstStatus.GetSelectMt(byMode) == tSrc))
// 		{
// 			continue;
// 		}

		// [pengjie 2011/3/3] 应该考虑终端在进行选看，没有接受适配器码流的情况
		if (byMode != MODE_SECVIDEO )
		{
			//Bug00104047 选看源非空，不停交换,此处不再使用GetMtSrc来判断
			//因为对于BAS掉线，需要对一些视频交换拆除
// 			TMt tMtSrc;
// 			m_ptMtTable->GetMtSrc( tDst.GetMtId(), &tMtSrc, byMode );
// 			if( !(tMtSrc == tSrc) )
// 			{
// 				continue;
// 			}
	
			TMtStatus tTempMtStatus;
			if ( m_ptMtTable->GetMtStatus(tDst.GetMtId(),&tTempMtStatus) && 
				 !tTempMtStatus.GetSelectMt(byMode).IsNull() 
				)
			{
				ConfPrint(LOG_LVL_DETAIL,MID_MCU_BAS,"[StopSwitchToSubMtNeedAdp]mt:%d-%d has select src:%d-%d\n",
						 tDst.GetMcuId(),tDst.GetMtId(), 
						 tTempMtStatus.GetSelectMt(byMode).GetMcuId(),tTempMtStatus.GetSelectMt(byMode).GetMtId()
						 );
				continue;
			}
			//Bug00104047 [end]

			TMt tMtSrc;
			m_ptMtTable->GetMtSrc( tDst.GetMtId(), &tMtSrc, byMode );
			// 主席轮询选看时,若轮询终端为当前发言人,取消发言人,不停bas到主席的交换
			if (byMode == MODE_VIDEO && 
				!tMtSrc.IsNull() && 
				m_tConf.GetChairman() == tDst &&
				(CONF_POLLMODE_BOTH_CHAIRMAN == m_tConf.m_tStatus.GetPollMode() ||
				 CONF_POLLMODE_VIDEO_CHAIRMAN == m_tConf.m_tStatus.GetPollMode()) &&
				 m_ptMtTable->GetMtStatus(tDst.GetMtId(),&tTempMtStatus) && tTempMtStatus.GetSelectMt(byMode) == tMtSrc 
				)
			{
				continue;
			}
		}		
		//End

		if(!IsNeedAdapt(tSrc, tDst, byMode))
		{
			continue;
		}
		if (!FindBasChn2BrdForMt(byMtIdx, byMode, tOutInfo))
		{
			continue;
		}

		if (tOutInfo.m_tBasEqp.GetEqpId() != tEqp.GetEqpId() || tOutInfo.m_byChnId != byChnId)
		{
			continue;
		}

		//代码合并
		if (MT_MANU_KDC != m_ptMtTable->GetManuId(byMtIdx) &&
			MT_MANU_KDCMCU != m_ptMtTable->GetManuId(byMtIdx) )
		{
			//zjl 20110510 StartSwitchToAll 替换 StartSwitchToSubMt
			//非KEDA在取消从适配获取码流后看自己
			//StartSwitchToSubMt(m_ptMtTable->GetMt(byMtIdx), 0, byMtIdx, MODE_VIDEO);
			TSwitchGrp tSwitchGrp;
			tSwitchGrp.SetSrcChnl(0);
			tSwitchGrp.SetDstMtNum(1); 
			tSwitchGrp.SetDstMt(&tDst);
			StartSwitchToAll(tDst, 1, &tSwitchGrp, MODE_VIDEO);
			continue;
		}
	
		//StopSwitchToSubMt(byMtIdx, byMode);	
		atDst[byMtNum] = tDst;
		byMtNum++;
		
	
		if (bStopAud)
		{
			TMtStatus   tMtStatus;	
			memset( &tMtStatus,0,sizeof( tMtStatus ) );
			m_ptMtTable->GetMtStatus( byMtIdx, &tMtStatus ) ;
			//zjj20091230如果终端音频源是混音器，在这里不能拆交换
			if( tMtStatus.GetAudioMt().GetType() == TYPE_MCUPERI &&
				EQP_TYPE_MIXER == tMtStatus.GetAudioMt().GetEqpType() )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "[StopSwitchToSubMtNeedAdpt] mt(%d)'s AudioSrc is mixer. can't stop audio switch\n", byMtIdx);                    
			}
			else
			{
				if(g_cMcuVcApp.IsVASimultaneous()&&
					!IsNeedAdapt(tSrc, tDst, MODE_AUDIO)
				  )
				{
					//StopSwitchToSubMt(byMtIdx, MODE_AUDIO);
					atDstAud[byAudMtNum] = tDst;
					byAudMtNum++;
				}				
			}
		}
	}

	StopSwitchToSubMt( byMtNum, atDst, byMode, TRUE ,TRUE,bSelSelf);
	
	if( byAudMtNum > 0 )
	{
		StopSwitchToSubMt( byAudMtNum, atDstAud, MODE_AUDIO, TRUE );
	}

	return TRUE;
}

/*====================================================================
    函数名      ：StartSwitchAud2MtNeedAdp
    功能        ：zjj20090925 补建唇音同步状态下bas到各个终端的音频交换
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/09/25    4.6         周晶晶          创建
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchAud3MtNeedAdp( u8 bySwitchMode )
{
	u8 byLoop = 0;
	BOOL32 bNeedAdp = FALSE;
	TLogicalChannel tVidChn;

	BOOL32 bRet = TRUE;

	TMt tLocalAudBrdSrc = GetLocalAudBrdSrc();

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_BAS,  "[StartSwitchAud3MtNeedAdp] Start Vmp To Mt Audio Switch. bySwitchMode.%u\n",bySwitchMode );

#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)	
	//8ke 不支持唇音同步，直接从广播源接收音频
 	return TRUE;
#endif
	
	if (!g_cMcuVcApp.IsVASimultaneous())
	{
		return FALSE;
	}
//////////////zjj20090925
	for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		if( m_tConfAllMtInfo.MtJoinedConf( byLoop ))
		{       

			//不交换给自己
			if (tLocalAudBrdSrc.GetMtType() == MT_TYPE_MT &&
				tLocalAudBrdSrc.GetMtId() == byLoop)
			{
				continue;
			}
			//不交换给上级MCU
			if (m_tCascadeMMCU.GetMtId() == byLoop )
			{
				continue;
			}			
			
            //对于需要视频适配的终端，音频同样需要进行缓冲处理，以保证唇音同步					
			bNeedAdp = IsHDConf(m_tConf) &&
							  IsNeedAdapt(GetLocalVidBrdSrc(), m_ptMtTable->GetMt(byLoop), MODE_VIDEO) &&
							  m_ptMtTable->GetMtLogicChnnl(byLoop, LOGCHL_VIDEO, &tVidChn, TRUE) &&
							  !IsNeedAdapt(GetLocalAudBrdSrc(), m_ptMtTable->GetMt(byLoop), MODE_AUDIO);
			if (bNeedAdp)
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP,  "[StartSwitchAud3MtNeedAdp] Start Vmp To Mt(%d,%d) Audio Switch\n",
					m_ptMtTable->GetMt(byLoop).GetMcuId(),
					m_ptMtTable->GetMt(byLoop).GetMtId()
					);
				//continue;
//				bRet = ( bRet && StartSwitchAud2MtNeedAdp( byLoop, bySwitchMode) );
				bRet = ( bRet && StartSwitchAud2MtNeedAdp( GetVidBrdSrc(), m_ptMtTable->GetMt(byLoop)));
			}				
		
		}
	}

	return bRet;

		
}

/*====================================================================
    函数名      StartSwitchAud2Perieqp
    功能        ：唇音同步适配 建立从源到所有广播视频适配的通道(setparam cmd 时用)
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/08/07    4.6         周嘉麟          创建
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchAud2Perieqp(TMt tSrc)
{
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	return TRUE;
#endif
	if(!g_cMcuVcApp.IsVASimultaneous())
	{
		return FALSE;
	}

	if (tSrc.IsNull())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[StartSwitchAud2Perieqp] tSrc is null!\n");
		return FALSE;
	}
	
	// [10/18/2010 xliang] vmp won't broadcast through bas
	if (TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_VMP == tSrc.GetEqpType())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[StartSwitchAud2Perieqp] tSrc is VMP, so no through bas!\n");
		return FALSE;
	}

	u8 byNum = 0;
	CBasChn *apcBasChn[MAXNUM_PERIEQP]={NULL};
	if(!GetBasBrdChnGrp(byNum, apcBasChn, MODE_VIDEO))
	{
		return FALSE;
	}
	u8 bySrcChn = m_tPlayEqp == GetLocalVidBrdSrc() ? m_byPlayChnnl : 0;

	TPeriEqpStatus tStatus;
	TEqp tBas;
	tBas.SetNull();
	for (u8 byIdx = 0; byIdx < byNum; byIdx++)
	{
		if ( NULL == apcBasChn[byIdx] )
		{
			continue;
		}
		if(g_cMcuVcApp.GetPeriEqpStatus(apcBasChn[byIdx]->GetBas().GetEqpId(), &tStatus) &&
			( g_cMcuAgent.IsEqpBasHD( apcBasChn[byIdx]->GetBas().GetEqpId() ) ) &&
		    (tStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU ||
		    tStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU_H ||
			tStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU2_BASIC ||
		    tStatus.m_tStatus.tHdBas.GetEqpType() == TYPE_MPU2_ENHANCED
			)
		   
		   )
		{
			//[2011/08/18/zhangli]唇音同步不支持PRS，StartSwitchToPeriEqp最后一个参数为FALSE
			StartSwitchToPeriEqp(tSrc, bySrcChn, apcBasChn[byIdx]->GetBas().GetEqpId(), apcBasChn[byIdx]->GetChnId(), 
				MODE_AUDIO, SWITCH_MODE_BROADCAST, TRUE, TRUE, FALSE, FALSE);
			tBas = apcBasChn[byIdx]->GetBas();
			tBas.SetConfIdx(m_byConfIdx);
		
			u8 byOutNum = 0;
			u8 byFrontOutNum = 0;
			if ( !GetBasChnOutputNum(apcBasChn[byIdx]->GetBas(),apcBasChn[byIdx]->GetChnId(),byOutNum,byFrontOutNum) )
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[StartSwitchToSubMtNeedAdpt] GetBasChnOutputNum:%d-%d failed!\n",
						 apcBasChn[byIdx]->GetBas().GetEqpId(), apcBasChn[byIdx]->GetChnId()
						);
				return FALSE;
			}

			for (u8 byOutIdx = 0; byOutIdx < byOutNum; byOutIdx++)
			{
				g_cMpManager.SetSwitchBridge(tBas, byFrontOutNum + byOutIdx, MODE_AUDIO, TRUE );
			}
		}
	}
	return TRUE;
}
/*====================================================================
    函数名      ：StartSwitchAud2MtNeedAdp
    功能        ：唇音同步适配 交换处理
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/08/07    4.6         周嘉麟          创建
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchAud2MtNeedAdp(TMt tSrc, TMt tDst, u8 bySwitchMode /*= SWITCH_MODE_BROADCAST*/)
{
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	//zjl 20110510 StartSwitchToAll 替换 StartSwitchToSubMt
	//8ke 不支持唇音同步，直接从广播源接收音频
	//StartSwitchToSubMt(tSrc, 0, tDst.GetMtId(), MODE_AUDIO, bySwitchMode);
	u16 wSpyPort = SPY_CHANNL_NULL;
	if( !tSrc.IsLocal() )
	{
		CRecvSpy cRecvSpy;
		if( m_cSMcuSpyMana.GetRecvSpy( tSrc,cRecvSpy ) )
		{
			wSpyPort = cRecvSpy.m_tSpyAddr.GetPort();
		}
	}
	TMt tLocalMt = GetLocalMtFromOtherMcuMt(tDst);
	u8 bySrcChn = 0;
	if ( tSrc == m_tMixEqp )
	{
		//支持APU2音频适配后,需要知道该混音器是作为适配器用还是混音器用，后续需要对下面代码进行修正
		if (m_tConf.m_tStatus.IsMixing())//如果是作为混音器用
		{
			/*if (m_ptMtTable->IsMtInMixGrp(tLocalMt.GetMtId()))
			{
				bySrcChn = GetMixChnPos(tLocalMt);
			} 
			else
			{
				bySrcChn = GetMixerNModeChn();
			}*/
			RestoreRcvMediaBrdSrc( tLocalMt.GetMtId(),MODE_AUDIO );
		}
		else//作为适配器用，后续需要获取对应适配通道，添加获取相应适配通道代码
		{
		}
	}
	/*TSwitchGrp t8kgSwitchGrp;
	t8kgSwitchGrp.SetSrcChnl(bySrcChn);
	t8kgSwitchGrp.SetDstMtNum(1);
	t8kgSwitchGrp.SetDstMt(&tDst);
	StartSwitchToAll(tSrc, 1, &t8kgSwitchGrp, MODE_AUDIO, bySwitchMode,FALSE,FALSE,wSpyPort);*/
 	return TRUE;
#endif
	
	if (!g_cMcuVcApp.IsVASimultaneous())
	{
		return FALSE;
	}

	if (tSrc.IsNull() || tDst.IsNull())
	{
		return FALSE;
	}
	
	if (CONF_POLLMODE_VIDEO == m_tConf.m_tStatus.GetPollMode())
    {
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "[StartSwitchAud2MtNeedAdp] Mt.%d needn't aud switch be ignored due to VID poll!\n", tDst.GetMtId());
        return TRUE;
    }
	
	//音频适配不过唇音同步
	if (IsNeedAdapt(tSrc, tDst, MODE_AUDIO))
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "[StartSwitchAud2MtNeedAdp] Mt:%d need aud adp, aud switch lossed!\n", tDst.GetMtId());
		return FALSE;
	}
	
	//视频不适配不过唇音同步
	if (!IsNeedAdapt(tSrc, tDst, MODE_VIDEO))
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "[StartSwitchAud2MtNeedAdp] Mt:%d needn't vid adp, aud switch lossed!\n", tDst.GetMtId());
		return FALSE;
	}
	
	TEqp tBas;
	tBas.SetNull();
	u8 byChnId  = 0;
	u8 byOutNum = 0;
	u8 byOutIdx = 0;
	//获取音频缓冲输出点
	if (!GetAudBufOutPutInfo(tBas, byChnId, byOutNum, byOutIdx))
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[StartSwitchAud2MtNeedAdp] GetAudBufOutPutInfo failed!\n");
		return FALSE;
	}
	
	//[nizhijun 2010/9/25] 非MPU 不建立音频伴随
	TPeriEqpStatus tBasStatus;
	if ( (!g_cMcuVcApp.GetPeriEqpStatus(tBas.GetEqpId(),&tBasStatus)) ||
		 ( g_cMcuAgent.IsEqpBasAud( tBas.GetEqpId() ) ) ||
		 ( tBasStatus.m_tStatus.tHdBas.GetEqpType() != TYPE_MPU && 
		   tBasStatus.m_tStatus.tHdBas.GetEqpType() != TYPE_MPU_H&&
		   tBasStatus.m_tStatus.tHdBas.GetEqpType() != TYPE_MPU2_BASIC && 
		   tBasStatus.m_tStatus.tHdBas.GetEqpType() != TYPE_MPU2_ENHANCED
		  )  	 	
	   )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "[StartSwitchAud2MtNeedAdp] Mt:%d not need Aud!\n",tDst.GetMtId());
		return FALSE;
	}

	tBas.SetConfIdx(m_byConfIdx);
	
	//zjl 20110510 StartSwitchToAll 替换 StartSwitchToSubMt
	//建音频交换
	//StartSwitchToSubMt(tBas, byChnId * byOutNum, tDst.GetMtId(), MODE_AUDIO, bySwitchMode);	
	TSwitchGrp tSwitchGrp;
	tSwitchGrp.SetSrcChnl(byChnId * byOutNum);
	tSwitchGrp.SetDstMtNum(1);
	tSwitchGrp.SetDstMt(&tDst);
	StartSwitchToAll(tBas, 1, &tSwitchGrp, MODE_AUDIO, bySwitchMode);
	return TRUE;
}

/*====================================================================
函数名       GetAudBufOutPutInfo
功能        ：获取音频缓冲输出点，找广播通道第一个通道来做输出
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：无
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
10/07/16    4.6         周嘉麟          创建
2011/12/31  4.7         倪志俊			重写
====================================================================*/
BOOL32 CMcuVcInst::GetAudBufOutPutInfo(TEqp &tBas, u8 &byChnId, u8 &byOutNum, u8 &byOutIdx)
{
	CBasChn *pBasChn[MAXNUM_PERIEQP] = {NULL};
	u8		byBasChnNum = 0;

	if ( !GetBasBrdChnGrp(byBasChnNum, pBasChn,MODE_VIDEO) )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[GetAudBufOutPutInfo]GetBasBrdChnGrp failed!\n");
		return FALSE;
	}

	for( u8 byIdx = 0 ;byIdx <byBasChnNum; byIdx++ )
	{
		if ( pBasChn[byIdx] != NULL  &&
			 !pBasChn[byIdx]->GetBas().IsNull()
			)
		{
			TPeriEqpStatus tBasStatus;
			if (!g_cMcuVcApp.GetPeriEqpStatus( pBasChn[byIdx]->GetBas().GetEqpId(), &tBasStatus))
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[GetAudBufOutPutInfo] GetPeriEqpStatus failed!\n");
				continue;
			}
			
			if (  g_cMcuAgent.IsEqpBasAud( tBas.GetEqpId() )  )
			{
				continue;
			}

			if (tBasStatus.m_tStatus.tHdBas.GetEqpType() != TYPE_MPU &&
				tBasStatus.m_tStatus.tHdBas.GetEqpType() != TYPE_MPU_H &&
				tBasStatus.m_tStatus.tHdBas.GetEqpType() != TYPE_MPU2_BASIC &&
				tBasStatus.m_tStatus.tHdBas.GetEqpType() != TYPE_MPU2_ENHANCED
				)
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_BAS, "[GetAudBufOutPutInfo] bas:%d-%d is not mpu or mpu2 which is not support With Aud!\n",
						  pBasChn[byIdx]->GetBas().GetEqpId(), pBasChn[byIdx]->GetChnId()
						  );
				continue;
			}

			tBas = pBasChn[byIdx]->GetBas();
			byChnId = pBasChn[byIdx]->GetChnId();
			THDAdaptParam atParam[MAXNUM_BASOUTCHN];
			if ( g_cMcuVcApp.GetBasOutPutParam(tBas,byChnId,byOutNum,atParam) )
			{
				for ( u8 byLoop = 0; byLoop<byOutNum; byLoop++ )
				{
					if ( !atParam[byLoop].IsNull() )
					{
						byOutIdx = byLoop;
						return TRUE;
					}
						
				}
			}
		}

	}

	return TRUE;
}
/*====================================================================
    函数名      StartSwitchToSubMtFromAdp
    功能        ：将数据交换到会议中 接本适配通道的某终端
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
  2010-6-30		4.6			周嘉麟			创建
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchToSubMtFromAdp(u8 byMtId, u8 byMediaMode)
{
	if (byMediaMode != MODE_AUDIO && byMediaMode != MODE_VIDEO && byMediaMode != MODE_SECVIDEO)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[StartSwitchToSubMtFromAdp2] unexpected MediaMode:%d!\n", byMediaMode);
		return FALSE;
	}

	TBasOutInfo tOutInfo;
	if (!FindBasChn2BrdForMt(byMtId, byMediaMode, tOutInfo))
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[StartSwitchToSubMtFromAdp2] FindAcceptBasChnForMt%d failed!\n", byMtId);
		return FALSE;
	}

	if ( !g_cMcuVcApp.IsPeriEqpConnected(tOutInfo.m_tBasEqp.GetEqpId()) )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[StartSwitchToSubMtFromAdp] tBas:%d-%d is not online,don't switch to byMtId:%d!\n",
				tOutInfo.m_tBasEqp.GetEqpId(),tOutInfo.m_byChnId,byMtId);
		return FALSE;
	}

	tOutInfo.m_tBasEqp.SetConfIdx(m_byConfIdx);
	//建桥
	g_cMpManager.SetSwitchBridge(tOutInfo.m_tBasEqp, tOutInfo.m_byFrontOutNum + tOutInfo.m_byOutIdx, byMediaMode, TRUE);
	//交换到目的终端
	if (MODE_SECVIDEO == byMediaMode)
	{
		//zjl 20110510 StartSwitchToAll 替换 StartSwitchToSubMt
// 		g_cMpManager.StartSwitchToSubMt(tBas, 
// 										byChnId * byOutNum + byOutIdx,
// 										m_ptMtTable->GetMt(byMtId), 
// 										byMediaMode, 
// 										SWITCH_MODE_BROADCAST, TRUE, TRUE, TRUE);
		TSwitchGrp tSwitchGrp;
		tSwitchGrp.SetSrcChnl(tOutInfo.m_byFrontOutNum + tOutInfo.m_byOutIdx);
		tSwitchGrp.SetDstMtNum(1);
		TMt tMt = m_ptMtTable->GetMt(byMtId);
		tSwitchGrp.SetDstMt(&tMt);
		g_cMpManager.StartSwitchToAll(tOutInfo.m_tBasEqp, 1, &tSwitchGrp, byMediaMode);
	}
	else
	{
		//zjl 20110510 StartSwitchToAll 替换 StartSwitchToSubMt
		//StartSwitchToSubMt(tBas, byChnId * byOutNum + byOutIdx, byMtId, byMediaMode);
		TSwitchGrp tSwitchGrp;
		tSwitchGrp.SetSrcChnl(tOutInfo.m_byFrontOutNum + tOutInfo.m_byOutIdx);
		tSwitchGrp.SetDstMtNum(1);
		TMt tMt = m_ptMtTable->GetMt(byMtId);
		tSwitchGrp.SetDstMt(&tMt);
		StartSwitchToAll(tOutInfo.m_tBasEqp, 1, &tSwitchGrp, byMediaMode);

		StartSwitchAud2MtNeedAdp(GetAudBrdSrc(), m_ptMtTable->GetMt(byMtId));	
	}
	
	//[nizhijun 2011/01/05]接收码流需要过适配的终端到prs的RTCP交换建立
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	if ( tConfAttrb.IsResendLosePack() )
	{
		u8 byPrsId, byPrsChnnlId = 0;
		if (FindPrsChnForSrc(tOutInfo.m_tBasEqp,tOutInfo.m_byFrontOutNum+tOutInfo.m_byOutIdx, byMediaMode, byPrsId,byPrsChnnlId))
		{
			AddRemovePrsMember( byMtId, byPrsId, byPrsChnnlId, byMediaMode, TRUE);
		} 
		else
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "[StartSwitchToSubMtFromAdp2] FindPrsChnnl for recvmt:%d failed!\n", byMtId);
		}
	}

	return TRUE;
}

/*====================================================================
函数名      StartAllBasSwitch
功能        ：建立对应模式的广播适配交换
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：无
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2010/07/14  3.0         周嘉麟         创建
====================================================================*/
BOOL32 CMcuVcInst::StartAllBasSwitch(u8 byMediaMode)
{
	if (byMediaMode != MODE_AUDIO && byMediaMode != MODE_VIDEO && byMediaMode != MODE_SECVIDEO)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_BAS, "[StartAllBasSwitch] unexpected ModiaMode:%d!\n", byMediaMode);
		return FALSE;
	}

	CBasChn *apcBasChn[MAXNUM_PERIEQP]={NULL};
	u8 byNum = 0;
	if(!GetBasBrdChnGrp(byNum, apcBasChn, byMediaMode))
	{
		return FALSE;
	}
	for (u8 byIdx = 0; byIdx < byNum; byIdx++)
	{
		if ( NULL == apcBasChn[byIdx] )
		{
			continue;
		}
		StartBasSwitch(apcBasChn[byIdx]->GetBas(), apcBasChn[byIdx]->GetChnId());
	}
	return TRUE;
}
/*====================================================================
函数名      StopAllBasSwitch
功能        ：拆除对应模式的广播适配交换
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：无
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2010/07/14  3.0         周嘉麟         创建
====================================================================*/
BOOL32 CMcuVcInst::StopAllBasSwitch(u8 byMediaMode)
{
	if (byMediaMode != MODE_AUDIO && byMediaMode != MODE_VIDEO && byMediaMode != MODE_SECVIDEO)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_BAS, "[StopAllBasSwitch] unexpected ModiaMode:%d!\n", byMediaMode);
		return FALSE;
	}
	
	CBasChn *apcBasChn[MAXNUM_PERIEQP]={NULL};
	u8 byNum = 0;
	if(!GetBasBrdChnGrp(byNum, apcBasChn, byMediaMode))
	{
		return FALSE;
	}
	for (u8 byIdx = 0; byIdx < byNum; byIdx++)
	{
		if ( NULL == apcBasChn[byIdx] )
		{
			continue;
		}
		TEqp tBas = apcBasChn[byIdx]->GetBas();
		tBas.SetConfIdx(m_byConfIdx);
		StopBasSwitch(tBas, apcBasChn[byIdx]->GetChnId());
	}
	return TRUE;
}

/*====================================================================
    函数名      ：StopSwitchToAllSubMtJoinedConf
    功能        ：停止将数据交换到会议中所有终端
    算法实现    ：
    引用全局变量：
    输入参数说明：BOOL32 bMsg, 是否发通知接收信息，缺省为TRUE
				  u8 byMode, 交换模式，缺省为MODE_BOTH
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/08    1.0         LI Yi         创建
    07/03/23    4.0         顾振华        广播视频交换修改
====================================================================*/
void CMcuVcInst::StopSwitchToAllSubMtJoinedConf( u8 byMode )
{
	u8	byLoop;

	TMt atDstMt[MAXNUM_CONF_MT];
	u8 byMtNum = 0;

    if ( MODE_AUDIO == byMode || MODE_BOTH == byMode )
    {
        for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
        {
            if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )    
            {
				atDstMt[byMtNum] = m_ptMtTable->GetMt( byLoop);
				byMtNum++;
                //StopSwitchToSubMt( byLoop, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE );
            }
        }

		StopSwitchToSubMt( byMtNum, atDstMt, MODE_AUDIO, FALSE );
    }

	TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();

	memset( atDstMt, 0, sizeof(atDstMt) );
    byMtNum = 0;
    if ( MODE_VIDEO == byMode || MODE_BOTH == byMode )
    {
        // 删除广播源
        u8 bySrcChnl = (tLocalVidBrdSrc==m_tPlayEqp) ? m_byPlayChnnl : 0;
        g_cMpManager.StopSwitchToBrd( tLocalVidBrdSrc, bySrcChnl );
        
        // 删除广播观众
        for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
        {
            if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )           
            {
				atDstMt[byMtNum] = m_ptMtTable->GetMt( byLoop);
				byMtNum++;
                //StopSwitchToSubMt( byLoop, MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE );
            }
        }

		StopSwitchToSubMt( byMtNum, atDstMt, MODE_VIDEO, FALSE );
    }

	MtStatusChange();
    
    return;
}

/*====================================================================
    函数名      ：StartSwitchToPeriEqp
    功能        ：将指定终端数据交换到外设
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt & tSrc, 源
				  u8 bySrcChnnl, 源的信道号 (用来分配端口)
				  u8 byEqpId, 外设号
				  u8 byDstChnnl, 目的信道索引号，缺省为0 (用来分配端口)
   				  u8 byMode, 交换模式，缺省为MODE_BOTH
				  BOOL32 bAddRtcp，是否建外设到源的RTCP交换，注意三点：
				  （1）源不占用PRS的，为TRUE
				  （2）占用PRS的情况，为FALE，例如非适配会议中，会议录像
				  （3）不需要建RTCP交换的为FALE，目前有PRS，HDU，TVWALL

   返回值说明  ：TRUE，如果不交换直接返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/20    1.0         LI Yi         创建
	03/07/24    2.0         胡昌威        修改
	2011/08/18  4.6         zhangli       修改（加参数bAddRtcp）
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchToPeriEqp(TMt tSrc,
                                        u16	wSrcChnnl,
                                        u8	byEqpId,
                                        u8  byDstChnnl,
                                        u8	byMode,
                                        u8	bySwitchMode,
                                        BOOL32 bDstHDBas,
                                        BOOL32 bStopBeforeStart,
                                        BOOL32 bBatchPollChg,
										BOOL32 bAddRtcp,
										BOOL32 bSendMsgToEqp,
										const u8 byHduSubChnId,
										const u8 byMulitModeHdu) 
{
	CServMsg	cServMsg;
	TPeriEqpStatus	tStatus;

	//需要建MODE_VIDEO2SECOND模式的交换到外设的有bas和prs,prs无需建rtcp交换，bas则能够把模式MODE_VIDEO2SECOND转为MODE_VIDEO
	if (MODE_VIDEO2SECOND == byMode)
	{
		byMode = MODE_VIDEO;
	}

	u8 bySrcChnnl = (u8)wSrcChnnl;
	//  [11/26/2009 pengjie] Modify 级联多回传支持，建到外设的交换
	CRecvSpy tSpyResource;
	TMt tUnlocalMt = tSrc;
    if(!tSrc.IsLocal() &&
	   m_cSMcuSpyMana.GetRecvSpy( tSrc, tSpyResource ) )
	{

		wSrcChnnl  = tSpyResource.m_tSpyAddr.GetPort();
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[StartSwitchToPeriEqp]recv mt(mcuid:%d, mtid:%d) from port %d.bBatchPollChg.%d\n",
			   tSrc.GetMcuId(), tSrc.GetMtId(), wSrcChnnl,bBatchPollChg);
	}
	// End

	tSrc = GetLocalMtFromOtherMcuMt( tSrc );

	//未连接
	if( !g_cMcuVcApp.IsPeriEqpConnected( byEqpId ) )
	{
		return FALSE;
	}
	if( !m_tConf.m_tStatus.IsSpecMixing() &&
		m_ptMtTable->GetManuId(tSrc.GetMtId()) == MT_MANU_CHAORAN && 
		byEqpId >= MIXERID_MIN && byEqpId <= MIXERID_MAX)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_EQP, "[StartSwitchToPeriEqp]ChaoRan Mt(%d,%d) Can't Joined Mixer(%d),So Return!\n",
			tSrc.GetMcuId(), tSrc.GetMtId(), byEqpId);
		return FALSE;
	}
	//检测非KDC终端是否处于哑音状态，是则不交换音频码流	
	if( TYPE_MT == m_ptMtTable->GetMainType( tSrc.GetMtId() ) && 
		MT_MANU_KDC != m_ptMtTable->GetManuId( tSrc.GetMtId() ) && 
		m_ptMtTable->IsMtAudioDumb( tSrc.GetMtId() ) )
	{
		if( byMode == MODE_AUDIO )
		{
			return TRUE;
		}
		else if ( byMode == MODE_BOTH )
		{
			byMode = MODE_VIDEO;
		}
	}

	if( !g_cMcuVcApp.GetPeriEqpStatus( byEqpId, &tStatus ) )
	{
		return FALSE;
	}


	//[nizhijun 2011/12/29] BAS接收端口特殊处理，直接在此处进行计算，不用放到mp层进行计算
	if ( EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId) )
	{
		u8 byBasOutNum = 0;
		u8 byBasFrontOutNum = 0;
		if ( !GetBasChnOutputNum(g_cMcuVcApp.GetEqp(byEqpId),(u8)byDstChnnl,byBasOutNum,byBasFrontOutNum) )
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[StopSwitchToPeriEqp] GetBasChnOutputNum:%d-%d failed!\n",
					byEqpId,byDstChnnl
					);
			return  FALSE;
		}
		if(!g_cMpManager.StartSwitchToPeriEqp(tSrc, wSrcChnnl, byEqpId, byBasFrontOutNum, 
										  byMode, bySwitchMode, bDstHDBas, bStopBeforeStart))
		{
			ConfPrint( LOG_LVL_ERROR, MID_MCU_EQP, "[StartSwitchToPeriEqp] ERROR: Cannot switch to specified eqp!\n" );
			return FALSE;
		}
	}
	else
	{
		// [2013/03/11 chenbing] 当前HDU是否支持多画面
		if ( IsValidHduEqp(g_cMcuVcApp.GetEqp(byEqpId))
			&& g_cMcuVcApp.IsMultiModeHdu(byEqpId)
			)
		{
			// [2013/03/11 chenbing] 多画面通道(0,1)间隔为20 
			if(!g_cMpManager.StartSwitchToPeriEqp(tSrc, wSrcChnnl, byEqpId, COMPOSECHAN(byDstChnnl, byHduSubChnId), 
				byMode, bySwitchMode, bDstHDBas, bStopBeforeStart, byHduSubChnId))
			{
				ConfPrint( LOG_LVL_ERROR, MID_MCU_EQP, "[StartSwitchToPeriEqp] ERROR: Cannot switch to specified eqp!\n" );
				return FALSE;
			}
		}
		else
		{
			if(!g_cMpManager.StartSwitchToPeriEqp(tSrc, wSrcChnnl, byEqpId, byDstChnnl, 
				byMode, bySwitchMode, bDstHDBas, bStopBeforeStart))
			{
				ConfPrint( LOG_LVL_ERROR, MID_MCU_EQP, "[StartSwitchToPeriEqp] ERROR: Cannot switch to specified eqp!\n" );
				return FALSE;
			}
		}

	}

	g_cMcuVcApp.SetPeriEqpSrc( byEqpId, (const TMt*)&tUnlocalMt, (u8)byDstChnnl, byMode);

	//[2011/08/18/zhangli]如果是丢包会议，建立eqp--->源的RTCP交换
	TEqp tEqp = g_cMcuVcApp.GetEqp(byEqpId);

	TConfAttrb tConfattrb = m_tConf.GetConfAttrb();
	if (bAddRtcp && tConfattrb.IsResendLosePack())
	{
		// [2013/03/11 chenbing] 修改目的通道 
		BuildRtcpSwitchForSrcToDst(tEqp, tUnlocalMt, byMode, u8(byDstChnnl), u8(wSrcChnnl), TRUE);
	}

	u32 dwTimeIntervalTicks = 3*OspClkRateGet();

	TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();

	//通知终端开始发送
    if (tSrc.GetType() == TYPE_MT)
    {
        //zbq[04/15/2009] Tand的关键帧编的比较快，且间隔为3s. 故交换建立后再发youareseeing，尽量拿到第一个关键帧
        if (IsDelayVidBrdVCU() && tSrc == tLocalVidBrdSrc)
        {
        }
        else
        {
            NotifyMtSend( tSrc.GetMtId(), byMode, TRUE );
			
			if ( byEqpId >= PRSID_MIN && byEqpId <= PRSID_MAX )
			{
				ConfPrint( LOG_LVL_DETAIL, MID_MCU_EQP, "[StartSwitchToPeriEqp] prs:%d don't need NotifyMtSend!\n",byEqpId);
			}
			else
			{
				
				if ( byEqpId >= HDUID_MIN && byEqpId <= HDUID_MAX)
				{
					// zbq [06/25/2007] 建音频交换不请求关键帧
					// [pengjie 2010/4/6] Modify 级联多回传请求关键帧支持
					if( tUnlocalMt.IsLocal() )
					{						
						if( MODE_BOTH == byMode || MODE_VIDEO == byMode )
						{            
							NotifyFastUpdate(tSrc, MODE_VIDEO, TRUE);
						}	
						
						// [pengjie 2010/9/6] 双流支持
						if( MODE_SECVIDEO == byMode )
						{
							ConfPrint( LOG_LVL_DETAIL, MID_MCU_EQP, "[StartSwitchToPeriEqp] Hdu NotifyFastUpdate MODE_SECVIDEO!\n" );
							NotifyFastUpdate(tSrc, MODE_SECVIDEO, TRUE);
						}
					}
					else
					{
						if(m_cSMcuSpyMana.IsMtInSpyMember( tUnlocalMt, MODE_VIDEO ))
						{
							// [pengjie 2010/9/6] 级联双流支持
							//NotifyFastUpdate(tUnlocalMt, MODE_VIDEO, FALSE);
							if( MODE_BOTH == byMode || MODE_VIDEO == byMode )
							{
								NotifyFastUpdate(tUnlocalMt, MODE_VIDEO, FALSE);
							}
							
							if( MODE_SECVIDEO == byMode )
							{
								ConfPrint( LOG_LVL_DETAIL, MID_MCU_EQP, "[StartSwitchToPeriEqp] Hdu Cascade NotifyFastUpdate MODE_SECVIDEO!\n" );
								NotifyFastUpdate( tUnlocalMt, MODE_SECVIDEO, FALSE );
							}
						}
					}
					// End
				}
				else
				{
					// zbq [06/25/2007] 建音频交换不请求关键帧
					// 				if( MODE_BOTH == byMode || MODE_VIDEO == byMode )
					// 				{            
					// 					NotifyFastUpdate(tSrc, MODE_VIDEO, TRUE);
					// 				}
					// [pengjie 2010/4/6] Modify 级联多回传请求关键帧支持
					if( tUnlocalMt.IsLocal() )
					{
						BOOL32 bNeedIFrame = TRUE;
#if defined(_8KH_) || defined(_8KE_) || defined(_8KI_)
						// 8khvmp有其特殊性，在切风格时或码流变化时会停收码流或创建新解码器，对本级终端mcu侧建交换时不主动请求关键帧
						if (IsValidVmpId(byEqpId))
						{
							// 不主动请求关键帧
							bNeedIFrame = FALSE;
						}
#endif
						if (bNeedIFrame)
						{
							if(MODE_BOTH == byMode || MODE_VIDEO == byMode || MODE_VIDEO2SECOND == byMode)
							{          
								NotifyFastUpdate(tSrc, MODE_VIDEO);
							}					
							// 双流支持
							if( MODE_SECVIDEO == byMode )
							{
								ConfPrint( LOG_LVL_DETAIL, MID_MCU_EQP, "[StartSwitchToPeriEqp] eqp(id:%d) NotifyFastUpdate MODE_SECVIDEO!\n", byEqpId);
								NotifyFastUpdate(tSrc, MODE_SECVIDEO, FALSE);
							}
						}
					}
					else
					{
						if(m_cSMcuSpyMana.IsMtInSpyMember( tUnlocalMt, MODE_VIDEO ))
						{
							NotifyFastUpdate(tUnlocalMt, MODE_VIDEO);
						}
					}
					// End
				}
			}
        }
    }

	//send message according to its type
	if( !g_cMcuVcApp.GetPeriEqpStatus( byEqpId, &tStatus ) )
	{
		return FALSE;
	}

    TTvWallStartPlay tTwPlayPara;
    THduStartPlay  tHduPlayPara;
	u8 byRtcpMode = byMode;
	if (MODE_AUDIO  == byMode)
    {
        byMode = HDU_OUTPUTMODE_AUDIO;
    }
    else if (MODE_VIDEO == byMode || MODE_SECVIDEO == byMode)
    {
        byMode = HDU_OUTPUTMODE_VIDEO;
    }
    else if (MODE_BOTH  == byMode || MODE_SECVIDEO == byMode)
    {
        byMode = HDU_OUTPUTMODE_BOTH;
    }
    else
    {
        byMode = HDU_OUTPUTMODE_BOTH;
    }

	tHduPlayPara.SetMode( byMode );

    TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();

	if(IsValidHduEqp(g_cMcuVcApp.GetEqp(tStatus.GetEqpId())))
    {
        tHduPlayPara.SetMt(tUnlocalMt);
		tHduPlayPara.SetVideoEncrypt(tEncrypt);
    }
	else
	{
		tTwPlayPara.SetMt(tUnlocalMt);
	    tTwPlayPara.SetVideoEncrypt(tEncrypt);
	}

	u32 dwVidSwitchIpAddr = 0;
	u16 wVidSwitchPort = 0;
	u32 dwAudSwitchIpAddr = 0;
	u16 wAudSwitchPort = 0;
	TTransportAddr tAudRemoteAddr;

	TConfAttrb tTmpConfAttrb = m_tConf.GetConfAttrb();
    if (tTmpConfAttrb.IsResendLosePack())
    {
		if(IsValidHduEqp(g_cMcuVcApp.GetEqp(tStatus.GetEqpId())))
		{
			if(MODE_VIDEO == byRtcpMode  || MODE_BOTH == byRtcpMode || MODE_SECVIDEO == byRtcpMode)
			{
				GetHduMemberRtcpAddrInfo(tUnlocalMt,bySrcChnnl, byRtcpMode, dwVidSwitchIpAddr, wVidSwitchPort, dwAudSwitchIpAddr, wAudSwitchPort);
				tHduPlayPara.SetVidRtcpBackAddr(dwVidSwitchIpAddr, wVidSwitchPort);
			}

			if(MODE_AUDIO == byRtcpMode || MODE_BOTH == byRtcpMode)
			{
				GetHduMemberRtcpAddrInfo(tUnlocalMt,bySrcChnnl, byRtcpMode, dwVidSwitchIpAddr, wVidSwitchPort, dwAudSwitchIpAddr, wAudSwitchPort);
				//tHduPlayPara.SetAudRtcpBackAddr(dwAudSwitchIpAddr, wAudSwitchPort);
				tAudRemoteAddr.SetIpAddr(dwAudSwitchIpAddr);
				tAudRemoteAddr.SetPort(wAudSwitchPort);
			}

            tHduPlayPara.SetIsNeedByPrs(TRUE);
		}
		else
		{
			if(MODE_VIDEO == byRtcpMode || MODE_SECVIDEO == byRtcpMode || MODE_BOTH == byRtcpMode)
			{
				GetHduMemberRtcpAddrInfo(tUnlocalMt,bySrcChnnl, byRtcpMode, dwVidSwitchIpAddr, wVidSwitchPort, dwAudSwitchIpAddr, wAudSwitchPort);
				tTwPlayPara.SetVidRtcpBackAddr(dwVidSwitchIpAddr, wVidSwitchPort);
			}
			
			if(MODE_AUDIO == byRtcpMode || MODE_BOTH == byRtcpMode)
			{
				GetHduMemberRtcpAddrInfo(tUnlocalMt,bySrcChnnl, byRtcpMode, dwVidSwitchIpAddr, wVidSwitchPort, dwAudSwitchIpAddr, wAudSwitchPort);
				//tTwPlayPara.SetAudRtcpBackAddr(dwAudSwitchIpAddr, wAudSwitchPort);
				tAudRemoteAddr.SetIpAddr(dwAudSwitchIpAddr);
				tAudRemoteAddr.SetPort(wAudSwitchPort);
			}

            tTwPlayPara.SetIsNeedByPrs(TRUE);
		}

    }
    else
    {
		if(IsValidHduEqp(g_cMcuVcApp.GetEqp(tStatus.GetEqpId())))
		{
			tHduPlayPara.SetIsNeedByPrs(FALSE);
		}
		else
		{
            tTwPlayPara.SetIsNeedByPrs(FALSE);
		}
	}
	
    TSimCapSet tSrcSCS;
	if ( EQP_TYPE_VMP == tSrc.GetEqpType() )    //hdu选看vmp时，vmp输出为终端源
	{
		// xliang [6/23/2009] 此处设mediatype维持会议主格式，建vmp到HDU交换处，会有选具体通道
        tSrcSCS.SetVideoMediaType( m_tConf.GetMainVideoMediaType() );
		// xliang [5/12/2009] 根据VMP 第0出实际输出的载荷赋值
// 		u8 byMediaType = 0;
// 		GetResByVmpOutChnnl(byMediaType, 0, m_tConf, m_tVmpEqp.GetEqpId());
// 		tSrcSCS.SetVideoMediaType(byMediaType);
	}
	else
	{
		tSrcSCS = m_ptMtTable->GetSrcSCS(tSrc.GetMtId()); 
	}
    TDoublePayload tDVPayload;
    TDoublePayload tDAPayload;

    if (MEDIA_TYPE_H264 == tSrcSCS.GetVideoMediaType() || 
        MEDIA_TYPE_H263PLUS == tSrcSCS.GetVideoMediaType() || 
        CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
        // zbq [10/29/2007] 适应FEC支持其他格式
        m_tConf.GetCapSupportEx().IsVideoSupportFEC())
    {
        tDVPayload.SetRealPayLoad(tSrcSCS.GetVideoMediaType());
        tDVPayload.SetActivePayload(GetActivePayload(m_tConf, tSrcSCS.GetVideoMediaType()));
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[StartSwitchToPeriEqp] tDVPayload.GetRealPayLoad() = %u\n", tDVPayload.GetRealPayLoad());
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[StartSwitchToPeriEqp] tDVPayload.GetActivePayload() = %u\n", tDVPayload.GetActivePayload());
    }
    else
    {
        tDVPayload.SetRealPayLoad(tSrcSCS.GetVideoMediaType());
        tDVPayload.SetActivePayload(tSrcSCS.GetVideoMediaType());
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[StartSwitchToPeriEqp] tDVPayload.GetRealPayLoad() = %u\n", tDVPayload.GetRealPayLoad());
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[StartSwitchToPeriEqp] tDVPayload.GetActivePayload() = %u\n", tDVPayload.GetActivePayload());
    }
    
    if (CONF_ENCRYPTMODE_NONE != tEncrypt.GetEncryptMode() ||
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

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[StartSwitchToPeriEqp] tSrcSCS.GetAudioMediaType(): %d\n", 
	tSrcSCS.GetAudioMediaType());


	if (bSendMsgToEqp)
	{
		switch(tStatus.GetEqpType())
		{
		case EQP_TYPE_TVWALL:
			{
				cServMsg.SetMsgBody((u8 *)&tTwPlayPara, sizeof(tTwPlayPara));
				cServMsg.CatMsgBody((u8 *)&tDVPayload, sizeof(tDVPayload));
				cServMsg.CatMsgBody((u8 *)&tDAPayload, sizeof(tDAPayload));
  				cServMsg.SetChnIndex(byDstChnnl);
				// MCU前向纠错，zgc，2007-09-27
				TCapSupportEx tCapSupportEx = m_tConf.GetCapSupportEx();
				cServMsg.CatMsgBody((u8*)&tCapSupportEx, sizeof(tCapSupportEx));
				cServMsg.CatMsgBody((u8*)&tAudRemoteAddr, sizeof(tAudRemoteAddr));

				SendMsgToEqp(byEqpId, MCU_TVWALL_START_PLAY_REQ, cServMsg);
				break;
			}
		case EQP_TYPE_HDU:
// 		case EQP_TYPE_HDU_H:
// 		case EQP_TYPE_HDU_L:
// 		case EQP_TYPE_HDU2:
// 		case EQP_TYPE_HDU2_L:
			{
				cServMsg.SetMsgBody((u8 *)&tHduPlayPara, sizeof(tHduPlayPara));
				cServMsg.CatMsgBody((u8 *)&tDVPayload, sizeof(tDVPayload));
				cServMsg.CatMsgBody((u8 *)&tDAPayload, sizeof(tDAPayload));
				cServMsg.SetChnIndex(byDstChnnl);
				// [2013/03/11 chenbing] 添加HDUId 
				cServMsg.SetEqpId(byEqpId);
        
				//  [1/13/2010 pengjie] Modify 设置HDU空闲通道的背景
				TEqpExCfgInfo tEqpExCfgInfo;
				if( SUCCESS_AGENT != g_cMcuAgent.GetEqpExCfgInfo( tEqpExCfgInfo ) )
				{
					tEqpExCfgInfo.Init();
				}
				// [2013/03/11 chenbing]  
				TPeriEqpStatus tHduStatus;
				g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tHduStatus);

				THDUExCfgInfo tHDUExCfgInfo;
				tHDUExCfgInfo = tEqpExCfgInfo.m_tHDUExCfgInfo;
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP,  "[StartSwitchToPeriEqp] Set HduChn IdleChlShowMode: %d \n", tHDUExCfgInfo.m_byIdleChlShowMode );	
				cServMsg.CatMsgBody( (u8 *)&tHDUExCfgInfo, sizeof(THDUExCfgInfo) );
				cServMsg.CatMsgBody( (u8 *)&tAudRemoteAddr, sizeof(tAudRemoteAddr) );
				//TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
				TLogicalChannel tLogicalChannel;
				m_ptMtTable->GetMtLogicChnnl(tSrc.GetMtId(),LOGCHL_AUDIO,&tLogicalChannel,FALSE);
				u8 byAudioTrackNum = tLogicalChannel.GetAudioTrackNum();
				cServMsg.CatMsgBody( (u8 *)&byAudioTrackNum, sizeof(u8) );
				// [2013/03/11 chenbing]  追加HDU子通道
				cServMsg.CatMsgBody( (u8 *)&byHduSubChnId, sizeof(u8) );
				// [2013/03/11 chenbing]  追加HDU风格
				cServMsg.CatMsgBody( (u8 *)&byMulitModeHdu, sizeof(u8) );
				// End Modify

				// [2013/03/11 chenbing]  
				u8 byCurChnState = tHduStatus.m_tStatus.tHdu.GetChnStatus(byDstChnnl, byHduSubChnId);

				if ( byDstChnnl >=MAXNUM_HDU_CHANNEL )
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_HDU,  "[StartSwitchToPeriEqp]HDUID:%d,CHNID:%d Status:%d,So Return!!\n",byEqpId,byDstChnnl,byCurChnState);
					return FALSE;
				}
				
				// [2013/03/11 chenbing]  
				tHduStatus.m_tStatus.tHdu.SetChnStatus(byDstChnnl, byHduSubChnId, THduChnStatus::eWAITSTART);
				ConfPrint( LOG_LVL_DETAIL, MID_MCU_EQP, "[StartSwitchToPeriEqp] ");
				ConfPrint( LOG_LVL_DETAIL, MID_MCU_EQP, "Mcs HduId(%d) ChnId(%d) HduSubChnId(%d) MeMber:(%d) Status(%d)!!!\n",
					byEqpId, byDstChnnl, byHduSubChnId, tHduStatus.m_tStatus.tHdu.GetMemberType(byDstChnnl),
				tHduStatus.m_tStatus.tHdu.GetChnStatus(byDstChnnl, byHduSubChnId) );

				g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tHduStatus);

				SendMsgToEqp(byEqpId, MCU_HDU_START_PLAY_REQ, cServMsg);

				// xliang [7/15/2009] 向VMP请求关键帧
				if(EQP_TYPE_VMP == tSrc.GetEqpType())
				{
					NotifyFastUpdate(tSrc, bySrcChnnl);
				}
				break;
			}
		default:
			break;
		}
	}

	return TRUE;
}

/*====================================================================
    函数名      : GetHduMemberRtcpAddrInfo
    功能        : 将指定终端数据交换到外设
    算法实现    :
    引用全局变量:
    输入参数说明: TMt      tSrc[in]:      电视墙的成员
   				  u8     byMode[in]:      媒体模式，audio， video, secdvideo;
				  u32 dwVidIpAddr[out]:      视频终端IP地址
				  u16    wVidPort[out]:      视频终端端口
				  u32 dwAudIpAddr[out]:      音频终端IP地址
				  u16    wAudPort[out]:      音频终端端口
   返回值说明  ： void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2010/09/13  4.6         刘利九        创建
====================================================================*/
void CMcuVcInst::GetHduMemberRtcpAddrInfo(const TMt tSrc, 
										  const u8 bySrcChl,
										  const u8 byMode, 
										  u32 &dwVidIpAddr, 
										  u16 &wVidPort, 
										  u32 &dwAudIpAddr, 
										  u16 &wAudPort)
{
	if(tSrc.IsNull())
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "Tmt is NULL!\n");
		return;
	}

	//终端
	if( TYPE_MT == tSrc.GetType())
	{
		TMt tTempPrsSrc = GetLocalMtFromOtherMcuMt(tSrc);
		if ( IsMultiCastMt(tTempPrsSrc.GetMtId()) )
		{
			ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_PRS,"[GetHduMemberRtcpAddrInfo]tTempPrsSrc:%d-%d is multicastmt!\n",tTempPrsSrc.GetMcuId(), tTempPrsSrc.GetMtId());
			return;
		}	

		TLogicalChannel tLogicalChannel;
		//本级终端
		if(tSrc.IsLocal())
		{
			if(MODE_VIDEO == byMode || MODE_BOTH == byMode)
			{
				if(!m_ptMtTable->GetMtLogicChnnl(tSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "[GetHduMemberRtcpAddrInfo:local+vid] Fail to Get Mt logical\n");
					
				}

				dwVidIpAddr = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
		        wVidPort =    tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
			}
			
			if(MODE_AUDIO == byMode || MODE_BOTH == byMode)
			{
				if(!m_ptMtTable->GetMtLogicChnnl(tSrc.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE))
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "[GetHduMemberRtcpAddrInfo:local+aud] Fail to Get Mt logical\n");

				}

				dwAudIpAddr = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
		        wAudPort =    tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
			}
			
			if(MODE_SECVIDEO == byMode)
			{
				if(!m_ptMtTable->GetMtLogicChnnl(tSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE))
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "[GetHduMemberRtcpAddrInfo:local+sec] Fail to Get Mt logical\n");
				}

				dwVidIpAddr = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
		        wVidPort =    tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
			}
				
		}
		//非本级终端
		else
		{
			if (!IsLocalAndSMcuSupMultSpy(tSrc.GetMcuId()))
			{
				TMt tLocalHduSrcMt = GetLocalMtFromOtherMcuMt(tSrc);				
				if (!tLocalHduSrcMt.IsNull())
				{
					if(MODE_VIDEO == byMode || MODE_BOTH == byMode)
					{
						if(m_ptMtTable->GetMtLogicChnnl(tLocalHduSrcMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE))
						{
							dwVidIpAddr = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
							wVidPort =    tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
						}
						else
						{
							ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "[GetHduMemberRtcpAddrInfo:Unlocal+UnSpy+Vid] Fail to Get Mt logical\n");
						}
					}
					
					if(MODE_AUDIO == byMode || MODE_BOTH == byMode)
					{
						if(m_ptMtTable->GetMtLogicChnnl(tLocalHduSrcMt.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE))
						{
							dwAudIpAddr = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
							wAudPort =    tLogicalChannel.GetSndMediaCtrlChannel().GetPort();						
						}
						else
						{
							ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "[GetHduMemberRtcpAddrInfo:Unlocal+UnSpy+Aud] Fail to Get Mt logical\n");
						}
						
					}
					
					if(MODE_SECVIDEO == byMode)
					{
						if(m_ptMtTable->GetMtLogicChnnl(tLocalHduSrcMt.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE))
						{
							dwVidIpAddr = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
							wVidPort =    tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
						}
						else
						{
							ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "[GetHduMemberRtcpAddrInfo:Unlocal+UnSpy+Sec] Fail to Get Mt logical\n");
						}
					}
				}
				else
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "[GetHduMemberRtcpAddrInfo] GetLocalMtFromOtherMcuMt(tmt:%d-%d) failed!\n",tSrc.GetMcuId(),tSrc.GetMtId());
					return;
				}
			}
			else
			{
				TMt tSecLocalHduSrcMt = GetLocalMtFromOtherMcuMt(tSrc);
				if(MODE_SECVIDEO == byMode)
				{
					if(m_ptMtTable->GetMtLogicChnnl(tSecLocalHduSrcMt.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE))
					{
						dwVidIpAddr = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
						wVidPort =    tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
					}
					else
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "[GetHduMemberRtcpAddrInfo:Unlocal+Spy+Sec] Fail to Get Mt logical\n");
					}
				}
				else
				{
					CRecvSpy tRecvSpyInfo;	
					u32 dwIpAddr;
					dwIpAddr = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( GetLocalMtFromOtherMcuMt(tSrc).GetMtId()));
					if(!m_cSMcuSpyMana.GetRecvSpy( tSrc, tRecvSpyInfo ))
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "[GetHduMemberRtcpAddrInfo] Fail to get multiple spy info\n");
						return;
					}
					if(MODE_VIDEO == byMode || MODE_BOTH == byMode)
					{
						dwVidIpAddr = dwIpAddr;
						wVidPort = tRecvSpyInfo.m_tSpyAddr.GetPort() + 1;
					}
					
					if(MODE_AUDIO == byMode || MODE_BOTH == byMode)
					{
						dwAudIpAddr = dwIpAddr;
						wAudPort = tRecvSpyInfo.m_tSpyAddr.GetPort() + 3;
					}
				}
			}
			return;
		}
	}
	else if(EQP_TYPE_VMP == tSrc.GetEqpType())
	{
		/*u8 byMediaType = m_tConf.GetMainVideoMediaType();
		u8 byRes = m_tConf.GetMainVideoFormat();
		
		
		
		TVideoStreamCap tStrCap = m_tConf.GetMainSimCapSet().GetVideoCap();
		bySrcChnnl = GetVmpOutChnnlByRes(tSrc.GetEqpId(),
			tStrCap.GetResolution(), 
			tStrCap.GetMediaType(),
			tStrCap.GetUserDefFrameRate(),
			tStrCap.GetH264ProfileAttrb());

		u32 dwSwitchIp = 0;
		u16 wSwitchPort = 0;*/
		TTransportAddr tRemoteAddr;
		if( GetRemoteRtcpAddr(tSrc, bySrcChl, MODE_VIDEO, tRemoteAddr) )
		{
			dwVidIpAddr = tRemoteAddr.GetIpAddr();
			wVidPort   = tRemoteAddr.GetPort();
		}
		//g_cMpManager.GetSwitchInfo(tSrc, dwSwitchIp, wSwitchPort, dwVidIpAddr);
		//wVidPort = wSwitchPort + bySrcChnnl * PORTSPAN + 1;
		return;
	}

}
/*====================================================================
    函数名      ：StopSwitchToPeriEqp
    功能        ：停止将数据交换到外设
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byEqpId,  外设号
				  u8 byDstChnnl, 信道索引号，缺省为0
				  BOOL32 bMsg, 是否发停止接收信息，缺省为TRUE
				  u8 byMode, 交换模式，缺省为MODE_BOTH
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/11    1.0         LI Yi         创建
	03/07/24    2.0         胡昌威        修改
====================================================================*/
void CMcuVcInst::StopSwitchToPeriEqp( u8 byEqpId,  u8 byDstChnnl, BOOL32 bMsg, u8 byMode, u8 bySwitchMode, u8 byHduSubChnId)
{
	CServMsg	cServMsg;
	TEqp	tEqp;
	TPeriEqpStatus	tStatus;
	TMt    TSrcMt;

	u8 byEqpChnnl = 0;
	//停到外设的交换时，目前只有bas的mode可能是MODE_VIDEO2SECOND
	if (MODE_VIDEO2SECOND == byMode)
	{
		byMode = MODE_VIDEO;
	}
    // guzh [5/12/2007] 检查是否是停混音交换
	// lukunpeng [03/26/2010] 如果当前是哑音状态，不释放通道号
	//[0901]fxh 此接口功能不应跟业务关联，对于混音器，停止到混音器得交换有两个场景1.从混音器中移除该终端2.只是为达到体现某终端哑音
	// 对于1业务需要将终端从混音列表中清除，而2却不可
// 	if (bReleaseChnnl)
// 	{
// 		//tianzhiyong 2010/03/21 增加EAPU类型混音器
// 		if (( g_cMcuVcApp.GetEqpType( byEqpId ) == EQP_TYPE_MIXER || g_cMcuVcApp.GetEqpType( byEqpId ) == EQP_TYPE_EMIXER) &&
// 			m_tMixEqp.GetEqpId() == byEqpId)
// 		{
// 			TMt tmt;
// 			// zbq [03/19/2007] 这里清一次混音通道位置
// 			for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++ )
// 			{
// 				if ( m_tConfAllMtInfo.MtJoinedConf(byMtId) )
// 				{
// 					tmt = m_ptMtTable->GetMt(byMtId);
// 					if ( 0 != GetMixChnPos(tmt) &&
// 						 GetMixChnPos(tmt) == byDstChnnl )
// 					{
// 						GetMixChnPos( tmt, TRUE );
// 						ConfPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR,  "[StopSwitchToPeriEqp] Mixer Chn pos.%d for MT.%d has been cleared !\n", byDstChnnl, byMtId );
// 						break;
// 					}
// 				}
// 			}
// 		}
// 	}

	//[nizhijun 2011/12/29] BAS接收端口特殊处理，直接在此处进行计算，不用放到mp层进行计算
	if ( EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId) )
	{
		u8 byBasOutNum = 0;
		u8 byBasFrontOutNum = 0;
		if ( !GetBasChnOutputNum(g_cMcuVcApp.GetEqp(byEqpId),byDstChnnl,byBasOutNum,byBasFrontOutNum) )
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[StopSwitchToPeriEqp] GetBasChnOutputNum:%d-%d failed!bySwitchMode.%d\n",
					  byEqpId,byDstChnnl,bySwitchMode
					);
			return ;
		}
		//stop switch
		g_cMpManager.StopSwitchToPeriEqp( m_byConfIdx, byEqpId, byBasFrontOutNum, byMode );
		byEqpChnnl = byDstChnnl;
	}
	else
	{
		if ( IsValidHduEqp(g_cMcuVcApp.GetEqp(byEqpId))
			&& g_cMcuVcApp.IsMultiModeHdu(byEqpId)
			)
		{
			// [2013/03/11 chenbing] 多画面通道(0,1)间隔为20
			byEqpChnnl = COMPOSECHAN(byDstChnnl, byHduSubChnId);
			g_cMpManager.StopSwitchToPeriEqp( m_byConfIdx, byEqpId, byEqpChnnl, byMode, byHduSubChnId );
		}
		else
		{
			g_cMpManager.StopSwitchToPeriEqp( m_byConfIdx, byEqpId, byDstChnnl, byMode );
			byEqpChnnl = byDstChnnl;
		}
	}

	//[2011/08/11/zhangli]停rtcp交换
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	if (tConfAttrb.IsResendLosePack())
	{
		TEqp tDstEqp = g_cMcuVcApp.GetEqp(byEqpId);

		u32 dwDstIp = 0;
		u16 wDstPort = 0;
		if (byMode == MODE_SECVIDEO)
		{
			g_cMcuVcApp.GetEqpRtcpDstAddr(byEqpId, byEqpChnnl, dwDstIp, wDstPort, MODE_VIDEO);
		}
		else
		{
			g_cMcuVcApp.GetEqpRtcpDstAddr(byEqpId, byEqpChnnl, dwDstIp, wDstPort, byMode);
		}
		
		if (dwDstIp != 0)
		{
			AddRemoveRtcpToDst(dwDstIp,wDstPort,tDstEqp, byEqpChnnl, byMode, TRUE, SPY_CHANNL_NULL, FALSE);
		}
		else
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[StopSwitchToPeriEqp] GetEqpRtcpDstAddr for eqp:%d chnl:%d failed!\n",byEqpId,byDstChnnl);
		}
	}


	TSrcMt.SetNull();
	g_cMcuVcApp.SetPeriEqpSrc( byEqpId, &TSrcMt, byDstChnnl, byMode );

	//send stop message
	if( bMsg )
	{
		g_cMcuVcApp.GetPeriEqpStatus( byEqpId, &tStatus );
		// [2013/03/11 chenbing] 添加外设Id
		cServMsg.SetEqpId(byEqpId);
		cServMsg.SetChnIndex( byDstChnnl );
		cServMsg.SetConfId( m_tConf.GetConfId() );
		tEqp.SetMcuEqp( (u8)LOCAL_MCUID, byEqpId, tStatus.GetEqpType() );
		cServMsg.SetMsgBody( ( u8 * )&tEqp, sizeof( tEqp ) );
		switch( tStatus.GetEqpType() )
		{
		case EQP_TYPE_TVWALL:
			SendMsgToEqp( byEqpId, MCU_TVWALL_STOP_PLAY_REQ, cServMsg );
			break;

		case EQP_TYPE_HDU:    //4.6 新加  jlb
// 		case EQP_TYPE_HDU_H:
// 		case EQP_TYPE_HDU_L:
// 		case EQP_TYPE_HDU2:
// 		case EQP_TYPE_HDU2_L:
			{
				if (byDstChnnl >=MAXNUM_HDU_CHANNEL
					|| THduChnStatus::eRUNNING != tStatus.m_tStatus.tHdu.GetChnStatus(byDstChnnl, byHduSubChnId))
				{
					LogPrint(LOG_LVL_WARNING, MID_MCU_PRS, "[StopSwitchToPeriEqp] EQP(id:%d,chn:%d) Status != THduChnStatus::eRUNNING,So Break!\n",
						byEqpId,byDstChnnl);
					break;
				}
		
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[StopSwitchToPeriEqp] EQP(id:%d, chn:%d, SubChnId:%d) byMode(%d)\n",
						byEqpId, byDstChnnl, byHduSubChnId, byMode);

				// [2013/03/11 chenbing] 切换通道状态
				tStatus.m_tStatus.tHdu.SetChnStatus(byDstChnnl, byHduSubChnId, THduChnStatus::eWAITSTOP);
				g_cMcuVcApp.SetPeriEqpStatus(byEqpId, &tStatus);

				// [2013/03/11 chenbing] 添加子通道
				cServMsg.CatMsgBody((u8 *)&byHduSubChnId, sizeof(u8));
				cServMsg.CatMsgBody((u8 *)&byMode, sizeof(u8));
				SendMsgToEqp( byEqpId, MCU_HDU_STOP_PLAY_REQ, cServMsg );
				CServMsg cToMcsPeriStatusMessage;
				cToMcsPeriStatusMessage.SetMsgBody((u8 *)&tStatus, sizeof(tStatus));
				SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
				break;
			}

		case EQP_TYPE_RECORDER:
			SendMsgToEqp( byEqpId, MCU_REC_STOPREC_REQ, cServMsg );
			break;
        case EQP_TYPE_BAS:
#if defined(_8KE_) || defined(_8KH_)																//[03/01/2010] zjl add
			SendMsgToEqp( byEqpId, MCU_BAS_STOPADAPT_REQ, cServMsg );
#else
			if (g_cMcuAgent.IsEqpBasHD(byEqpId))
			{
				SendMsgToEqp( byEqpId, MCU_HDBAS_STOPADAPT_REQ, cServMsg );
			}
			else
			{
				SendMsgToEqp( byEqpId, MCU_BAS_STOPADAPT_REQ, cServMsg );
			}          
#endif
		 
            break;
        default:
            break;
		}
	}
}

/*====================================================================
    函数名      ：StopSwitchToAllPeriEqpWatchingSrcMt
    功能        ：停止将数据交换到会控中选看媒体源为指定终端的通道
    算法实现    ：
    引用全局变量：
    输入参数说明：TMt & tSrc, 指定终端 
				  BOOL32 bMsg, 是否发停止接收信息，缺省为TRUE
				  u8 byMode, 交换模式，缺省为MODE_BOTH
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/30    1.0         JQL           创建
====================================================================*/
void CMcuVcInst::StopSwitchToAllPeriEqpWatchingSrcMt(TMt  tSrc, BOOL32 bMsg , u8 byMode )
{
	TMt tSrcMt;
	u8      byEqpId;
	u8      byChnnlNo;

	ConfPrint(LOG_LVL_DETAIL,MID_MCU_MPMGR,"[StopSwitchToAllPeriEqpWatchingSrcMt] bMsg.%d byMode.%d\n",bMsg,byMode);

	tSrc = GetLocalMtFromOtherMcuMt( tSrc );

	for( byEqpId = 1; byEqpId <= MAXNUM_MCU_PERIEQP; byEqpId++)
	{
		if( !g_cMcuVcApp.IsPeriEqpConnected( byEqpId ) )
			continue;

		//目前只对电视墙与画面合成器操作
		if( g_cMcuAgent.GetPeriEqpType( byEqpId ) != EQP_TYPE_TVWALL 
			&& g_cMcuAgent.GetPeriEqpType( byEqpId ) != EQP_TYPE_VMP )
		{
			continue;
		}

		for( byChnnlNo = 0; byChnnlNo < MAXNUM_PERIEQP_CHNNL; byChnnlNo++ )
		{
			if( g_cMcuVcApp.GetPeriEqpSrc( byEqpId, &tSrcMt, byChnnlNo, MODE_VIDEO )
				&& tSrcMt == tSrc )
				StopSwitchToPeriEqp( byEqpId, byChnnlNo, TRUE, MODE_VIDEO );

			if( g_cMcuVcApp.GetPeriEqpSrc( byEqpId, &tSrcMt, byChnnlNo, MODE_AUDIO )
				&& tSrcMt == tSrc )
				StopSwitchToPeriEqp( byEqpId, byChnnlNo, TRUE, MODE_AUDIO );
		}
	}
}

/*====================================================================
    函数名      ：StopSwitchToAllPeriEqpWatchingSrcMcu
    功能        ：停止将数据交换到会控中选看媒体源属于指定MCU的通道
    算法实现    ：
    引用全局变量：
    输入参数说明：const u16 wMcuId, 指定MCU
				  BOOL32 bMsg, 是否发停止接收信息，缺省为TRUE
				  u8 byMode, 交换模式，缺省为MODE_BOTH
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/30    1.0         JQL           创建
====================================================================*/
//void CMcuVcInst::StopSwitchToAllPeriEqpWatchingSrcMcu( const u16 wMcuId, BOOL32 bMsg , u8 byMode )
// {
// 	TMt tSrcMt;
// 	u8      byEqpId;
// 	u8      byChnnlNo;
// 
// 	for( byEqpId = 1; byEqpId <= MAXNUM_MCU_PERIEQP; byEqpId++)
// 	{
// 		if( !g_cMcuVcApp.IsPeriEqpConnected( byEqpId ) )
// 			continue;
// 
// 		//目前只对电视墙操作
// 		if( g_cMcuAgent.GetPeriEqpType( byEqpId ) != EQP_TYPE_TVWALL )
// 			continue;
// 
// 		for( byChnnlNo = 0; byChnnlNo < MAXNUM_PERIEQP_CHNNL; byChnnlNo++ )
// 		{
// 			if( g_cMcuVcApp.GetPeriEqpSrc( byEqpId, &tSrcMt, byChnnlNo, MODE_VIDEO )
// 				&& tSrcMt.GetMcuId() == wMcuId )
// 				StopSwitchToPeriEqp( byEqpId, byChnnlNo, TRUE, MODE_VIDEO );
// 
// 			if( g_cMcuVcApp.GetPeriEqpSrc( byEqpId, &tSrcMt, byChnnlNo, MODE_AUDIO )
// 				&& tSrcMt.GetMcuId() == wMcuId )
// 				StopSwitchToPeriEqp( byEqpId, byChnnlNo, TRUE, MODE_AUDIO );
// 		}
// 	}
// }

/*====================================================================
    函数名      ：StopSwitchToAllPeriEqpInConf
    功能        ：停止将数据交换到会控中选看媒体源为指定终端的通道
    算法实现    ：
    引用全局变量：
    输入参数说明：BOOL32 bMsg, 是否发停止接收信息，缺省为TRUE
				  u8 byMode, 交换模式，缺省为MODE_BOTH
    返回值说明  ：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/30    1.0         JQL           创建
====================================================================*/
void CMcuVcInst::StopSwitchToAllPeriEqpInConf(BOOL32 bMsg , u8 byMode )
{
	TMt tMt;
	u8  byLoop;

	for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
		{
			tMt = m_ptMtTable->GetMt( byLoop );
			StopSwitchToAllPeriEqpWatchingSrcMt( tMt, bMsg, byMode );
		}
	}
}

/*====================================================================
    函数名      ：StartSwitchToMc
    功能        ：将指定终端数据交换到会控
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt & tSrc, 源
				  u8 bySrcChnnl, 源的信道号
				  u16 wMcInstId, 会议控制台实例号
				  u8 byDstChnnl, 目的信道索引号，缺省为0
   				  u8 byMode, 交换模式，缺省为MODE_BOTH
   返回值说明  ：TRUE，如果不交换直接返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/18    1.0         LI Yi         创建
	03/07/24    2.0         胡昌威        修改
	03/03/25    3.0         胡昌威        修改
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchToMc(TMt tSrc, u8 bySrcChnnl, u16 wMcInstId, 
								u8 byDstChnnl, u8 byMode, u16 wSpyPort ) 
{
	CServMsg	cServMsg;	

	u8 bySwitchMode = byMode;
	//  [11/27/2009 pengjie] Modify 级联多回传 这里记录下未被Local化得源终端
	TMt tUnlocalSrc = tSrc;
	tSrc = GetLocalMtFromOtherMcuMt( tSrc );
	// End
	

	//未连接
	if( !g_cMcuVcApp.IsMcConnected( wMcInstId ) )
	{
		return FALSE;
	}


	//检测非KDC终端是否处于哑音状态，是则不交换音频码流	
	if( TYPE_MT == m_ptMtTable->GetMainType( tSrc.GetMtId() ) && 
		MT_MANU_KDC != m_ptMtTable->GetManuId( tSrc.GetMtId() ) && 
		m_ptMtTable->IsMtAudioDumb( tSrc.GetMtId() ) )
	{
		if( byMode == MODE_AUDIO )
		{
			//return TRUE;
			bySwitchMode = MODE_NONE;
		}
		else if ( byMode == MODE_BOTH )
		{
			//byMode = MODE_VIDEO;
			bySwitchMode = MODE_VIDEO;
		}
	}


	//  [11/27/2009 pengjie] Modify 级联多回传支持  这里记录下监控的终端和相应的监控通道
// 	CMcChnnlInfo cMcChnnlInfo;
// 	if( m_cMcChnnlMgr.GetMcChnnlInfo( byDstChnnl, cMcChnnlInfo ) )
// 	{
// 		TMt tMcSrc = cMcChnnlInfo.GetMcSrc();
// 		u8  byMode = cMcChnnlInfo.GetMcChnnlMode();
// 		FreeRecvSpy( tMcSrc, byMode);
// 		m_cMcChnnlMgr.FreeMcChnnl( byDstChnnl );
// 	}
//     if(!g_cMpManager.StartSwitchToMc(tSrc,bySrcChnnl,wMcInstId,byDstChnnl,byMode,wSpyPort))
// 	{
// 		ConfLog( FALSE, "StartSwitchToMc() failed! Cannot switch to specified eqp!\n" );
// 		return FALSE;
// 	}
// 	cMcChnnlInfo.Clear();
// 	cMcChnnlInfo.SetMcChnnlMode( byMode );
// 	cMcChnnlInfo.SetMcSrc( tUnlocalSrc );
// 	m_cMcChnnlMgr.SetMcChnnlInfo( byDstChnnl, cMcChnnlInfo );
	// End

	// [pengjie 2010/8/4] 级联多回传支持  会控监控策略调整
	TMt tMcSrc;
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH ) // 音频
	{
		g_cMcuVcApp.GetMcSrc( wMcInstId, &tMcSrc, byDstChnnl, MODE_AUDIO );
		if( !tMcSrc.IsNull() )
		{
			if( tMcSrc == tUnlocalSrc )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "StartSwitchToMc GetMcSrc == NewMt, MODE_AUDIO\n" );
				FreeRecvSpy( tMcSrc, MODE_AUDIO);
			}
			else
			{
				//zhouyiliang 20121017 如果上个源是另外一个会议的则让另外一个会议停止
				if ( tMcSrc.GetConfIdx() != m_byConfIdx )
				{
					CMcuVcInst* pcVcInst = g_cMcuVcApp.GetConfInstHandle(tMcSrc.GetConfIdx());
					if (NULL != pcVcInst  )
					{
						pcVcInst->FreeRecvSpy( tMcSrc, MODE_AUDIO);
					}
					g_cMpManager.StopSwitchToMc(tMcSrc.GetConfIdx(), wMcInstId, byDstChnnl, MODE_AUDIO);
				}			
				else
				{
					FreeRecvSpy( tMcSrc, MODE_AUDIO);
				}

				g_cMcuVcApp.SetMcSrc( wMcInstId, ( const TMt* )&tUnlocalSrc, byDstChnnl, MODE_AUDIO );
			}
		}
		else
		{
			g_cMcuVcApp.SetMcSrc( wMcInstId, ( const TMt* )&tUnlocalSrc, byDstChnnl, MODE_AUDIO );
		}
	}
	
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH ) // 音频
	{
		g_cMcuVcApp.GetMcSrc( wMcInstId, &tMcSrc, byDstChnnl, MODE_VIDEO );
		if( !tMcSrc.IsNull() )
		{
			if( tMcSrc == tUnlocalSrc )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "StartSwitchToMc GetMcSrc == NewMt, MODE_VIDEO !\n" );
				FreeRecvSpy( tMcSrc, MODE_VIDEO);
			}
			else
			{
				//zhouyiliang 20121017 如果上个源是另外一个会议的则让另外一个会议停止
				if ( tMcSrc.GetConfIdx() != m_byConfIdx )
				{			
					CMcuVcInst* pcVcInst = g_cMcuVcApp.GetConfInstHandle(tMcSrc.GetConfIdx());
					if (NULL != pcVcInst  )
					{
						pcVcInst->FreeRecvSpy( tMcSrc, MODE_VIDEO);
					}
					g_cMpManager.StopSwitchToMc(tMcSrc.GetConfIdx(), wMcInstId, byDstChnnl, MODE_VIDEO);
				}			
				else
				{
					FreeRecvSpy( tMcSrc, MODE_VIDEO);
				}

				g_cMcuVcApp.SetMcSrc( wMcInstId, ( const TMt* )&tUnlocalSrc, byDstChnnl, MODE_VIDEO );
			}
		}
		else
		{
			g_cMcuVcApp.SetMcSrc( wMcInstId, ( const TMt* )&tUnlocalSrc, byDstChnnl, MODE_VIDEO );
		}
	}
	
	if (byMode == MODE_SECVIDEO)
	{
		g_cMcuVcApp.GetMcSrc( wMcInstId, &tMcSrc, byDstChnnl, MODE_SECVIDEO );
		if (!tMcSrc.IsNull())
		{
			if (tMcSrc == tUnlocalSrc)
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "StartSwitchToMc GetMcSrc == NewMt, MODE_SECVIDEO !\n" );
			}
			else
			{
				g_cMcuVcApp.SetMcSrc( wMcInstId, ( const TMt* )&tUnlocalSrc, byDstChnnl, MODE_SECVIDEO );
			}
		}
		else
		{
			g_cMcuVcApp.SetMcSrc( wMcInstId, ( const TMt* )&tUnlocalSrc, byDstChnnl, MODE_SECVIDEO );
		}
	}

	if( bySwitchMode != MODE_NONE && !g_cMpManager.StartSwitchToMc(tSrc, bySrcChnnl, wMcInstId, byDstChnnl, bySwitchMode, wSpyPort) )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "StartSwitchToMc() failed! Cannot switch to specified eqp! bySwitchMode.%d\n",bySwitchMode );
		return FALSE;
	}
	//zhouyiliang 20100813 spychnnllnotify
	if (!tUnlocalSrc.IsLocal() && IsLocalAndSMcuSupMultSpy(tUnlocalSrc.GetMcuId()))
	{
		TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tUnlocalSrc.GetMcuId()));
 		SendMMcuSpyNotify( tUnlocalSrc,MCS_MCU_STARTSWITCHMT_REQ, tSimCapSet);
	}
	
	// End

	//通知终端开始发送
	if( tSrc.GetType() == TYPE_MT )
	{
		NotifyMtSend( tSrc.GetMtId(), byMode, TRUE );


        // zbq [06/25/2007] 建音频交换不请求关键帧
        if ( MODE_VIDEO == byMode || MODE_BOTH == byMode )
        {
			// [pengjie 2010/8/4] 这里应该请求下级终端的关键帧
            NotifyFastUpdate(tUnlocalSrc, MODE_VIDEO, TRUE);
        }
		if ( MODE_SECVIDEO == byMode )
		{
			NotifyFastUpdate(tUnlocalSrc, MODE_SECVIDEO);
		}
	}
	else if (tSrc.GetType() == TYPE_MCUPERI)
	{
		// 如果源是vmp,向vmp对应通道请求关键帧
		if (tSrc.GetEqpType() == EQP_TYPE_VMP)
		{
			//在SwitchSrcToDst中调用此方法后会对vmp指定通道请求关键帧,向vmp多次请求关键帧会影响vmp画面质量?
			//NotifyFastUpdate(m_tVmpEqp, bySrcChnnl);
		}
	}

	//send message
	TSwitchInfo tSwitchInfo;
	// [pengjie 2010/8/30] 这里需要报非local的终端信息给界面
	tSwitchInfo.SetSrcMt( tUnlocalSrc );
	tSwitchInfo.SetSrcChlIdx( bySrcChnnl );
	tSwitchInfo.SetDstChlIdx( byDstChnnl );
	tSwitchInfo.SetMode( byMode );
	
	//加密以及动态载荷信息
	TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
	TSimCapSet tSrcSCS = m_ptMtTable->GetSrcSCS( tSrc.GetMtId() ); 
	TDoublePayload tDVPayload;
	TDoublePayload tDAPayload;

	// 如果源是vmp
	if (tSrc.GetType() == TYPE_MCUPERI && tSrc.GetEqpType() == EQP_TYPE_VMP)
	{
		tSrcSCS = m_tConf.GetMainSimCapSet();
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
		
    cServMsg.SetChnIndex( byDstChnnl );
	cServMsg.SetMsgBody( ( u8 * )&tSwitchInfo, sizeof( tSwitchInfo ) );
	cServMsg.CatMsgBody( (u8 *)&tEncrypt, sizeof(tEncrypt));
	cServMsg.CatMsgBody( (u8 *)&tDVPayload, sizeof(tDVPayload));
	cServMsg.CatMsgBody( (u8 *)&tEncrypt, sizeof(tEncrypt));
	cServMsg.CatMsgBody( (u8 *)&tDAPayload, sizeof(tDAPayload));
    
	TLogicalChannel tAudLog;
	m_ptMtTable->GetMtLogicChnnl( tSrc.GetMtId(),LOGCHL_AUDIO,&tAudLog,FALSE );
    // zw [06/26/2008] 适应AAC LC格式
	TAudAACCap tAudAACCap;
    if ( MEDIA_TYPE_AACLC == tSrcSCS.GetAudioMediaType() )
    {
        tAudAACCap.SetMediaType(MEDIA_TYPE_AACLC);
        tAudAACCap.SetSampleFreq(AAC_SAMPLE_FRQ_32);
		//TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
        tAudAACCap.SetChnlType(GetAACChnlTypeByAudioTrackNum(tAudLog.GetAudioTrackNum())); //[5/22/2012 chendaiwei]目前支持AAC_CHNL_TYPE_SINGLE和AAC_CHNL_TYPE_DOUBLE
        tAudAACCap.SetBitrate(96);
        tAudAACCap.SetMaxFrameNum(AAC_MAX_FRM_NUM);
    
	    cServMsg.CatMsgBody( (u8 *)&tAudAACCap, sizeof(tAudAACCap) );
    }
	//tianzhiyong 2010/04/02 适应AAC LD格式
    else if ( MEDIA_TYPE_AACLD == tSrcSCS.GetAudioMediaType() )
    {
        tAudAACCap.SetMediaType(MEDIA_TYPE_AACLD);
        tAudAACCap.SetSampleFreq(AAC_SAMPLE_FRQ_32);
		//TAudioTypeDesc tAudioType = m_tConfEx.GetMainAudioTypeDesc();
		tAudAACCap.SetChnlType(GetAACChnlTypeByAudioTrackNum(tAudLog.GetAudioTrackNum())); //[5/22/2012 chendaiwei]目前支持AAC_CHNL_TYPE_SINGLE和AAC_CHNL_TYPE_DOUBLE
        tAudAACCap.SetBitrate(96);
        tAudAACCap.SetMaxFrameNum(AAC_MAX_FRM_NUM);
    
	    cServMsg.CatMsgBody( (u8 *)&tAudAACCap, sizeof(tAudAACCap) );
    }
	else /*( MEDIA_TYPE_AACLD == tSrcSCS.GetAudioMediaType() && MEDIA_TYPE_AACLC == tSrcSCS.GetAudioMediaType() )*/
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[StartSwitchToMc] m_tConf.GetMainAudioMediaType() does'nt equals 103 or 104 !\n");
		cServMsg.CatMsgBody( (u8 *)&tAudAACCap, sizeof(tAudAACCap) );
	}

	TTransportAddr  tVidAddr;
	TTransportAddr  tAudAddr;
	tVidAddr.SetNull();
	tAudAddr.SetNull();

	//[2011/11/09/zhangli]用统一接口获取，原写法不支持单回传
	//双流RTCP通道, 双流的PRS支持是通过PRS外设来是实现的
	if (MODE_SECVIDEO == byMode)
	{
		TLogicalChannel tEqpLogicalChannel;
		u8 byChlNum = 0;
		u8 byPrsId = 0;
		u8 byPrsChnId = 0;
		if (FindPrsChnForBrd(MODE_SECVIDEO, byPrsId, byPrsChnId) &&
			g_cMcuVcApp.GetPeriEqpLogicChnnl(byPrsId, MODE_VIDEO, &byChlNum, &tEqpLogicalChannel, TRUE))
		{
			tVidAddr.SetIpAddr(tEqpLogicalChannel.GetRcvMediaCtrlChannel().GetIpAddr());
			tVidAddr.SetPort(tEqpLogicalChannel.GetRcvMediaCtrlChannel().GetPort() + PORTSPAN * byPrsChnId + 2);
		}
	}
	else
	{
		if (!GetRemoteRtcpAddr(tUnlocalSrc, bySrcChnnl, MODE_AUDIO, tAudAddr))
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[StartSwitchToMc]GetRemoteRtcpAddr failed\n");
		}
		
		if (!GetRemoteRtcpAddr(tUnlocalSrc, bySrcChnnl, MODE_VIDEO, tVidAddr))
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[StartSwitchToMc]GetRemoteRtcpAddr failed\n");
		}
	}


	//视频RTCP通道
	cServMsg.CatMsgBody( (u8 *)&tVidAddr, sizeof(TTransportAddr) );	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[StartSwitchToMc]tell ui monitor video rtcp port(%s, %d)\n", 
			StrOfIP(tVidAddr.GetIpAddr()), tVidAddr.GetPort());

	//音频RTCP通道
	cServMsg.CatMsgBody( (u8 *)&tAudAddr, sizeof(TTransportAddr) );	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[StartSwitchToMc]tell ui monitor video rtcp port(%s, %d)\n", 
			StrOfIP(tAudAddr.GetIpAddr()), tAudAddr.GetPort());

	SendMsgToMcs(  (u8)wMcInstId, MCU_MCS_STARTPLAY_NOTIF, cServMsg );
	
	return( TRUE );
}

/*====================================================================
    函数名      ：StopSwitchToMc
    功能        ：停止将数据交换到会控
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wMcInstId,  实例号
				  u8 byDstChnnl, 信道索引号，缺省为0
				  BOOL32 bMsg, 是否发停止接收信息，缺省为TRUE
				  u8 byMode, 交换模式，缺省为MODE_BOTH
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/18    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::StopSwitchToMc( u16 wMcInstId, u8 byDstChnnl, BOOL32 bMsg, u8 byMode, BOOL32 bFilterConn )
{
	CServMsg	cServMsg;
	TMt     tMtNull;
	BOOL32  bSrcAudioValid = TRUE;
	BOOL32  bSrcVideoValid = TRUE;
	u8 byStopMode = byMode;

	if( bFilterConn && !g_cMcuVcApp.IsMcConnected( wMcInstId ) )
    {
        LogPrint( LOG_LVL_WARNING, MID_MCU_VCS, "[StopSwitchToMc]Mc.%d not connected!\n", wMcInstId);
        return;
    }
	
	//[2011/11/29/zhangli]不能无条件停监控，有源的时候才停，bug:开一个vcs会议，然后再开一个vcs，登录失败，但是前一个vcs监控被停
	//stop switch
    //g_cMpManager.StopSwitchToMc( m_byConfIdx, wMcInstId, byDstChnnl, byMode );

	// [pengjie 2010/8/4] 级联多回传，取消监控，回收回传资源 策略调整
	TMt tMcSrc;
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH ) // 停音频
	{
		g_cMcuVcApp.GetMcSrc( wMcInstId, &tMcSrc, byDstChnnl, MODE_AUDIO );
		// [pengjie 2010/9/1] 判断该终端是否是本会议的终端或该终端是NULL
		if( tMcSrc.IsNull() || tMcSrc.GetConfIdx() != m_byConfIdx )
		{
			bSrcAudioValid = FALSE;
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MCS, "[StopSwitchToMc] GetMcSrc MODE_AUDIO Is Invalid, McId.%d, m_byConfIdx = %d, MtConfIdx.%d\n",
				wMcInstId, m_byConfIdx, tMcSrc.GetConfIdx() );

			byStopMode = MODE_VIDEO & byStopMode;
		}
		else
		{
			FreeRecvSpy( tMcSrc, MODE_AUDIO);
			tMtNull.SetNull();
			g_cMcuVcApp.SetMcSrc( wMcInstId, &tMtNull, byDstChnnl, MODE_AUDIO );
		}

	}

	if( byMode == MODE_VIDEO || byMode == MODE_BOTH ) // 停视频
	{
		g_cMcuVcApp.GetMcSrc( wMcInstId, &tMcSrc, byDstChnnl, MODE_VIDEO );

		// [pengjie 2010/9/1] 判断该终端是否是本会议的终端或该终端是NULL
		if( tMcSrc.IsNull() || tMcSrc.GetConfIdx() != m_byConfIdx )
		{
			bSrcVideoValid = FALSE;
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MCS, "[StopSwitchToMc] GetMcSrc MODE_VIDEO Is Invalid, McId.%d, m_byConfIdx = %d, MtConfIdx.%d\n",
				wMcInstId, m_byConfIdx, tMcSrc.GetConfIdx() );

			byStopMode = MODE_AUDIO & byStopMode;
		}
		else
		{
			FreeRecvSpy( tMcSrc, MODE_VIDEO);
			tMtNull.SetNull();
			g_cMcuVcApp.SetMcSrc( wMcInstId, &tMtNull, byDstChnnl, MODE_VIDEO );
		}
	}
	// End

	g_cMpManager.StopSwitchToMc(m_byConfIdx, wMcInstId, byDstChnnl, byStopMode);
	
	if( byMode == MODE_BOTH && !bSrcAudioValid && !bSrcVideoValid )
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_MCS, "[StopSwitchToMc] Src is invalid, so return here! Mode:BOTH\n");
		return;
	}
	if( byMode == MODE_AUDIO && !bSrcAudioValid )
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_MCS, "[StopSwitchToMc] Src is invalid, so return here! Mode:AUDIO\n");
		return;
	}
	if( byMode == MODE_VIDEO && !bSrcVideoValid )
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_MCS, "[StopSwitchToMc] Src is invalid, so return here! Mode:VIDEO\n");
		return;
	}	


	if( (byMode == MODE_AUDIO || byMode == MODE_BOTH) &&  bSrcAudioValid )
	{
		tMtNull.SetNull();
		g_cMcuVcApp.SetMcSrc( wMcInstId, &tMtNull, byDstChnnl, MODE_AUDIO );
	}

	if( (byMode == MODE_VIDEO || byMode == MODE_BOTH) &&  bSrcVideoValid )
	{
		tMtNull.SetNull();
		g_cMcuVcApp.SetMcSrc( wMcInstId, &tMtNull, byDstChnnl, MODE_VIDEO );
	}
	

	//send stop message
	if( bMsg )
	{
		TSwitchInfo tSwitchInfo;
		tSwitchInfo.SetDstChlIdx( byDstChnnl );
		tSwitchInfo.SetMode( byMode );
		cServMsg.SetChnIndex( byDstChnnl );
		cServMsg.SetMsgBody( ( u8 * )&tSwitchInfo, sizeof( tSwitchInfo ) );
		SendMsgToMcs(  (u8)wMcInstId, MCU_MCS_STOPPLAY_NOTIF, cServMsg );
	}
}

/*====================================================================
    函数名      ：StopSwitchToAllMcInConf
    功能        ：停止将数据交换到所有收看会议中终端的会控
    算法实现    ：
    引用全局变量：
    输入参数说明：BOOL32 bMsg, 是否发停止接收信息，缺省为TRUE
				  u8 byMode, 交换模式，缺省为MODE_BOTH
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/28    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::StopSwitchToAllMcInConf( BOOL32 bMsg, u8 byMode )
{	
// 	u8  byLoop;
// 	TMt tMt;
// 
// 	for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
// 	{
// 		if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
// 		{
// 			tMt = m_ptMtTable->GetMt( byLoop );
// 			StopSwitchToAllMcWatchingSrcMt( tMt, bMsg, byMode );	
// 		}
// 	}

	TMt		tCurSrc;
	u8      byIndex;
	u8      byChannel;
	u8      byChannelNum;
	TLogicalChannel tLogicalChannel;
	
	for( byIndex = 1; byIndex <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byIndex++ )
	{
		if( !g_cMcuVcApp.IsMcConnected( byIndex ) )
			continue;
        
		g_cMcuVcApp.GetMcLogicChnnl( byIndex, MODE_VIDEO, &byChannelNum, &tLogicalChannel );
		
		for( byChannel = 0; byChannel < byChannelNum; byChannel++ )
		{
			if( ( byMode == MODE_VIDEO || byMode == MODE_BOTH ) 
				&& g_cMcuVcApp.GetMcSrc( byIndex, &tCurSrc, byChannel, MODE_VIDEO )
				&& tCurSrc.GetConfIdx() == m_byConfIdx
				)
			{
				StopSwitchToMc( byIndex, byChannel, bMsg, MODE_VIDEO );
			}
			
			if( ( byMode == MODE_AUDIO || byMode == MODE_BOTH ) 
				&& g_cMcuVcApp.GetMcSrc( byIndex, &tCurSrc, byChannel, MODE_AUDIO )
				&& tCurSrc.GetConfIdx() == m_byConfIdx
				)
			{
				StopSwitchToMc( byIndex, byChannel, bMsg, MODE_AUDIO );
			}
		}
	}
}

/*====================================================================
    函数名      ：AdjustSwitchToAllMcWatchingSrcMt
    功能        ：调整数据交换到会控中选看媒体源为指定终端的通道
    算法实现    ：
    引用全局变量：
    输入参数说明：TMt & tSrc, 指定终端 
				  BOOL32 bMsg, 是否发停止接收信息，缺省为TRUE
				  u8 byMode, 交换模式，缺省为MODE_BOTH
				  BOOL32 bIsStop，是建交换还是拆除交换
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/28    1.0         JQL           创建
====================================================================*/
void CMcuVcInst::AdjustSwitchToAllMcWatchingSrcMt(TMt tSrc, BOOL32 bMsg, u8 byMode, BOOL32 bIsStop/*=TRUE*/,u8 byChl /*= 0xFF*/ )
{
	TMt tCurSrc;
	u8      byIndex;
	u8      byChannel;
	u8      byChannelNum;
	TLogicalChannel tLogicalChannel;
	

	CRecvSpy tSpyResource;
	u16 wSpyPort = SPY_CHANNL_NULL;	
	
	for( byIndex = 1; byIndex <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byIndex++ )
	{
		if( !g_cMcuVcApp.IsMcConnected( byIndex ) )
			continue;
        
		g_cMcuVcApp.GetMcLogicChnnl( byIndex, MODE_VIDEO, &byChannelNum, &tLogicalChannel );
		
		for( byChannel = 0; byChannel < byChannelNum; byChannel++ )
		{
			if( ( byMode == MODE_VIDEO || byMode == MODE_BOTH ) && ( 0xff == byChl || byChannel == byChl )
				&& g_cMcuVcApp.GetMcSrc( byIndex, &tCurSrc, byChannel, MODE_VIDEO )
				&& ( tCurSrc == tSrc || ( IsMcu(tSrc) && IsMtInMcu(tSrc, tCurSrc) ) ) // [9/10/2010 xliang] ?à??μ?ê?????mcu??μ??????éD?
				)
			{
				if( bIsStop )
				{
					if( bMsg )
					{
						StopSwitchToMc( byIndex, byChannel, bMsg, MODE_VIDEO );
					}
					else
					{
						g_cMpManager.StopSwitchToMc(m_byConfIdx, byIndex, byChannel, MODE_VIDEO);
					}
				}
				else
				{
					if( m_cSMcuSpyMana.GetRecvSpy( tCurSrc, tSpyResource ) )
					{
						wSpyPort = tSpyResource.m_tSpyAddr.GetPort();
					}
					if( bMsg )
					{						
						
						StartSwitchToMc( tCurSrc,0,byIndex,byChannel,MODE_VIDEO,wSpyPort );
					}
					else
					{
						g_cMpManager.StartSwitchToMc(tCurSrc, 0, byIndex, byChannel, MODE_VIDEO, wSpyPort);
					}
				}				
			}
			
			if( ( byMode == MODE_AUDIO || byMode == MODE_BOTH ) && ( 0xff == byChl || byChannel == byChl )
				&& g_cMcuVcApp.GetMcSrc( byIndex, &tCurSrc, byChannel, MODE_AUDIO )
				&& ( tCurSrc == tSrc || ( IsMcu(tSrc) && IsMtInMcu(tSrc, tCurSrc) ) )
				)
			{
				

				if( bIsStop )
				{
					if( bMsg )
					{						
						StopSwitchToMc( byIndex, byChannel, bMsg, MODE_AUDIO );
					}
					else
					{
						g_cMpManager.StopSwitchToMc(m_byConfIdx, byIndex, byChannel, MODE_AUDIO);
					}
				}
				else
				{
					if( m_cSMcuSpyMana.GetRecvSpy( tCurSrc, tSpyResource ) )
					{
						wSpyPort = tSpyResource.m_tSpyAddr.GetPort();
					}
					if( bMsg )
					{	
						StartSwitchToMc( tCurSrc,0,byIndex,byChannel,MODE_AUDIO,wSpyPort );
					}
					else
					{
						g_cMpManager.StartSwitchToMc(tCurSrc, 0, byIndex, byChannel, MODE_AUDIO, wSpyPort);
					}
				}		
			}
		}
	}
}

/*====================================================================
    函数名      ：StopSwitchToAllMcWatchingSrcMcu
    功能        ：停止将数据交换到会控中选看媒体源属于指定MCU的通道
    算法实现    ：
    引用全局变量：
    输入参数说明：const u16 wMcuId, 指定MCU
				  BOOL32 bMsg, 是否发停止接收信息，缺省为TRUE
				  u8 byMode, 交换模式，缺省为MODE_BOTH
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/28    1.0         JQL           创建
====================================================================*/
void CMcuVcInst::StopSwitchToAllMcWatchingSrcMcu( const u16 wMcuId, BOOL32 bMsg, u8 byMode )
{
	TMt tCurSrc;
	u8      byIndex;
	u8      byChannel;
	u8      byChannelNum;
	TLogicalChannel tLogicalChannel;

	for( byIndex = 1; byIndex <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byIndex++ )
	{
		if( !g_cMcuVcApp.IsMcConnected( byIndex ) )
			continue;
        
		g_cMcuVcApp.GetMcLogicChnnl( byIndex, MODE_VIDEO, &byChannelNum, &tLogicalChannel );
		
		for( byChannel = 0; byChannel < byChannelNum; byChannel++ )
		{
			if( ( byMode == MODE_VIDEO || byMode == MODE_BOTH ) 
				&& g_cMcuVcApp.GetMcSrc( byIndex, &tCurSrc, byChannel, MODE_VIDEO )
				&& tCurSrc.GetMcuId() == wMcuId )
				StopSwitchToMc( byIndex, byChannel, bMsg, MODE_VIDEO );

			if( ( byMode == MODE_AUDIO || byMode == MODE_BOTH ) 
				&& g_cMcuVcApp.GetMcSrc( byIndex, &tCurSrc, byChannel, MODE_AUDIO )
				&& tCurSrc.GetMcuId() == wMcuId )
				StopSwitchToMc( byIndex, byChannel, bMsg, MODE_AUDIO );
		}
	}
}

/*====================================================================
    函数名      ：SwitchSrcToDst
    功能        ：将码流从源交换到目的
    算法实现    ：
    引用全局变量：
    输入参数说明：const TSwitchInfo &tSwitchInfo, 交换信息
	              const CServMsg & cServMsgHdr,	业务消息头
				  bBatchPollChg
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/01/04    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::SwitchSrcToDst( const TSwitchInfo &tSwitchInfo,
                                 const CServMsg & cServMsgHdr,
                                 BOOL32 bBatchPollChg,
								 const THduSwitchInfo *ptHduSwitchInfo,
								 const u8 byMulitModeHdu)
{
	CServMsg	cServMsg;
	TMt	tDstMt = GetLocalMtFromOtherMcuMt(tSwitchInfo.GetDstMt());
	TMt tSrcMt = GetLocalMtFromOtherMcuMt(tSwitchInfo.GetSrcMt());
	u8	byMode = tSwitchInfo.GetMode();
	u8	bySrcChnnl = tSwitchInfo.GetSrcChlIdx();// xliang [1/7/2009] 目前仅在选看VMP时有用

	//zjj20100201
	//  [11/17/2009 pengjie] Modify 级联多回传处理
	TMt tUnlocalSrcMt = tSwitchInfo.GetSrcMt();
	TMt tUnlocalDstMt = tSwitchInfo.GetDstMt();
	CRecvSpy tSpyResource;
	u16 wSpyPort = SPY_CHANNL_NULL;	
	if( m_cSMcuSpyMana.GetRecvSpy( tUnlocalSrcMt, tSpyResource ) )
	{
		wSpyPort = tSpyResource.m_tSpyAddr.GetPort();
	}
	// End Modify

	//set initial message header
	cServMsg.SetServMsg( ( u8 * )cServMsgHdr.GetServMsg(), cServMsgHdr.GetServMsgLen() );
            
    NotifyMtSend( tSrcMt.GetMtId(), byMode );

    //非kdc厂商有广播源且节省带宽时，通知mt停止发送码流
    if(g_cMcuVcApp.IsSavingBandwidth() && 
        MT_MANU_KDC != m_ptMtTable->GetManuId( tDstMt.GetMtId()) &&
        MT_MANU_KDCMCU != m_ptMtTable->GetManuId( tDstMt.GetMtId()))
    {
        NotifyOtherMtSend(tDstMt.GetMtId(), FALSE);
    }

	switch( cServMsgHdr.GetEventId() )
	{
	case MT_MCU_STARTSELMT_REQ:
	case MT_MCU_STARTSELMT_CMD:
	case MCS_MCU_STARTSWITCHMT_REQ:
		{
			//zjl 20110510 StartSwitchToAll 替换 StartSwitchToSubMt
			//zjj20091023 选看不建桥
			//g_cMpManager.SetSwitchBridge(tSrcMt, 0, byMode);
			//StartSwitchToSubMt(tUnlocalSrcMt, 0, tDstMt.GetMtId(), byMode, SWITCH_MODE_SELECT, TRUE, FALSE, FALSE, wSpyPort);
			TSwitchGrp tSwitchGrp;
			tSwitchGrp.SetSrcChnl(0);
			tSwitchGrp.SetDstMtNum(1);
			tSwitchGrp.SetDstMt(&tDstMt);
			//zhouyiliang 20120924 主席轮询模式下，是内部选看，不需要设置mcsdragmod
			BOOL32 bInnerSelect = FALSE;
			if ( cServMsgHdr.GetSrcSsnId() == 0   )		
			{
				bInnerSelect = TRUE;
			}
			StartSwitchToAll(tUnlocalSrcMt, 1, &tSwitchGrp, byMode, SWITCH_MODE_SELECT, TRUE, FALSE, wSpyPort,bInnerSelect);
		}		
		break;
	case MCS_MCU_STARTSWITCHVMPMT_REQ:// xliang [1/7/2009] 主席选看VMP 
	case MT_MCU_STARTSWITCHVMPMT_REQ:	// xliang [4/9/2009] MT 触发的选看
		{
			g_cMpManager.SetSwitchBridge(tSrcMt, bySrcChnnl, byMode); // xliang [4/2/2009] 不走广播逻辑，没必要建桥？// xliang [4/28/2009] 虽不走广播逻辑但是做为广播源要建桥
			//zjl 20110510 StartSwitchToAll 替换 StartSwitchToSubMt
			//StartSwitchToSubMt(tSrcMt, bySrcChnnl, tDstMt.GetMtId(), byMode, SWITCH_MODE_SELECT, TRUE, FALSE);
			TSwitchGrp tSwitchGrp;
			tSwitchGrp.SetSrcChnl(bySrcChnnl);
			tSwitchGrp.SetDstMtNum(1);
			tSwitchGrp.SetDstMt(&tDstMt);
			StartSwitchToAll(tSrcMt, 1, &tSwitchGrp, byMode, SWITCH_MODE_SELECT, TRUE, FALSE);
		}		
		break;
	case MCS_MCU_STARTSWITCHMC_REQ:
		StartSwitchToMc(tUnlocalSrcMt, bySrcChnnl, cServMsgHdr.GetSrcSsnId(),tSwitchInfo.GetDstChlIdx(), byMode,wSpyPort);
		break;
	case MCS_MCU_START_SWITCH_TW_REQ:
		//[2011/08/18/zhangli]最后一个参数填FALSE
		StartSwitchToPeriEqp(tUnlocalSrcMt, 0, tDstMt.GetEqpId(), tSwitchInfo.GetDstChlIdx(), byMode, SWITCH_MODE_SELECT, wSpyPort, TRUE, FALSE, FALSE);
		break;
	case MCS_MCU_START_SWITCH_HDU_REQ: // xliang [6/23/2009] 第2参数不填死0，因为可能看VMP，这时SrcChnnl由具体情况决定
		// [2013/03/11 chenbing] 添加HDU子通道与通道风格 
		if (HDUCHN_MODE_FOUR == byMulitModeHdu && NULL != ptHduSwitchInfo)
		{
			StartSwitchToPeriEqp(tUnlocalSrcMt, bySrcChnnl, tDstMt.GetEqpId(), tSwitchInfo.GetDstChlIdx(),
				byMode, SWITCH_MODE_SELECT, FALSE, TRUE, bBatchPollChg, FALSE, TRUE, ptHduSwitchInfo->GetDstSubChn(), byMulitModeHdu );
		}
		else
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MCS, "[SwitchSrcToDst] ptHduSwitchInfo is NULL!!!\n" );
			StartSwitchToPeriEqp(tUnlocalSrcMt, bySrcChnnl, tDstMt.GetEqpId(), tSwitchInfo.GetDstChlIdx(),
				byMode, SWITCH_MODE_SELECT, FALSE, TRUE, bBatchPollChg, FALSE);
		}
		break;
	default:

		ConfPrint(LOG_LVL_WARNING, MID_MCU_MCS, "Exception - wrong switch eventid %u!\n", cServMsgHdr.GetEventId() );
		return;
	}


	if ( TYPE_MT == tSrcMt.GetType())
	{
		NotifyMtFastUpdate( tSrcMt.GetMtId(), MODE_VIDEO );
	}
	// 对于vmp,只请求对应通道关键帧
	else if (tSrcMt.GetEqpType() == EQP_TYPE_VMP)
	{
		NotifyFastUpdate(tSrcMt, bySrcChnnl);
	}
    else if ( TYPE_MCUPERI == tSrcMt.GetType() )
    {
		NotifyFastUpdate( tSrcMt, 0 );
		NotifyFastUpdate( tSrcMt, 1 );
		NotifyFastUpdate( tSrcMt, 2 );
    }
	else
	{
		ConfPrint( LOG_LVL_WARNING, MID_MCU_MCS, "Wrong type :tSrcMt.GetType = %d\n" ,tSrcMt.GetType() );
	}
	
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	if (tConfAttrb.IsResendLosePack()/* && MODE_AUDIO != byMode*/)
	{
		if( MCS_MCU_START_SWITCH_HDU_REQ == cServMsgHdr.GetEventId() 
			|| MCS_MCU_START_SWITCH_TW_REQ == cServMsgHdr.GetEventId()
			|| MCS_MCU_STARTSWITCHMC_REQ == cServMsgHdr.GetEventId())
		{
			ConfPrint( LOG_LVL_WARNING, MID_MCU_MCS, "[SwitchSrcToDst] HDU or TW'S or mc RTCP don't build here!\n" );
			return;
		}
		
		u8 byRtpDstChnnl = 0;
		if (TYPE_MT == tDstMt.GetType())
        {
            byRtpDstChnnl = 0;
        }
        else if (TYPE_MCUPERI == tDstMt.GetType())
        {
            byRtpDstChnnl = tSwitchInfo.GetDstChlIdx();
        }
		BuildRtcpSwitchForSrcToDst(tUnlocalDstMt, tUnlocalSrcMt, byMode, byRtpDstChnnl, bySrcChnnl, TRUE);
	}

	return;
}

/*=============================================================================
函 数 名： AdjustSwitchBridge
功    能： mp板断链或注册时调整当前桥交换
算法实现： 
全局变量： 
参    数： u8 byMpId 断链或新注册的mp板id
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/9/15  4.0			许世林                  创建
=============================================================================*/
BOOL32 CMcuVcInst::AdjustSwitchBridge(u8 byMpId, BOOL32 bInsert)
{
    //断链时先删除前一个mp的桥交换
    if (!bInsert)
    {
        u8 byTmpMpId = byMpId-1;
        while(byTmpMpId > 0)
        {
            if (g_cMcuVcApp.IsMpConnected(byTmpMpId))
            {
                break;
            }
            byTmpMpId--;
        }

        //前面没有mp板
        if (byTmpMpId == 0)    
        {
            byTmpMpId = MAXNUM_DRI;
            while(byTmpMpId > byMpId)
            {
                if (g_cMcuVcApp.IsMpConnected(byTmpMpId))
                {
                    break;
                }
                byTmpMpId--;
            }

            //只有这一个mp版
            if (byTmpMpId == byMpId)
            {
                return TRUE;
            }        
        }

        //找到了变化mpid前面的mp板id
        if (byTmpMpId > 0 && byTmpMpId <= MAXNUM_DRI)
        {        
            u32 dwInsertMpIp = g_cMcuVcApp.GetMpIpAddr(byMpId);
            u16 wTableId = 0;
	        while( wTableId < MAX_SWITCH_CHANNEL )
	        {
		        TSwitchChannel *ptSwitchChnnl = m_ptSwitchTable->GetSwitchChannel(byTmpMpId, wTableId);
                if (!ptSwitchChnnl->IsNull())
                {
                    TMt tSrc = ptSwitchChnnl->GetSrcMt();                
                    // guzh [7/3/2007] 判断接收是否是MP，明确桥交换
                    if ( ptSwitchChnnl->GetRcvPort() == ptSwitchChnnl->GetDstPort() && 
                         //g_cMcuVcApp.FindMp( ptSwitchChnnl->GetDstIP() ) != 0 &&
                         !tSrc.IsNull() )
                    {                        
                        //找到一条桥路，拆除到byMPId的桥
                        g_cMpManager.StopSwitch(tSrc.GetConfIdx(), ptSwitchChnnl->GetDstIP(), ptSwitchChnnl->GetRcvPort());                        
                    }
                }
                wTableId++;
            }
        }
    }

    // xsl 重新建桥
    u8 bySrcChnnl;
    
	TMt tLocalVidBrdSrc = GetLocalVidBrdSrc();
    //video广播源
    if (!tLocalVidBrdSrc.IsNull())
    {
        bySrcChnnl = (tLocalVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;

		//[2011/09/02/zhangli]如果视频发言人是下级终端，需要建30000回传端口的交换
		u16 dSpyPort = SPY_CHANNL_NULL;
		CRecvSpy tSpyResource;

		if (!tLocalVidBrdSrc.IsLocal() && m_cSMcuSpyMana.GetRecvSpy(tLocalVidBrdSrc, tSpyResource))
		{
			dSpyPort  = tSpyResource.m_tSpyAddr.GetPort();
		}

        g_cMpManager.SetSwitchBridge(tLocalVidBrdSrc, bySrcChnnl, MODE_VIDEO, FALSE, dSpyPort);
    
        if (tLocalVidBrdSrc == m_tVmpEqp)
        {
			TPeriEqpStatus tPeriEqpStatus;	
			g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);
			u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;

			if (((0 != m_tConf.GetSecBitRate() || MEDIA_TYPE_NULL != m_tConf.GetCapSupport().GetSecondSimCapSet().GetVideoMediaType()))
				&& VMP == byVmpSubType)
			{
				g_cMpManager.SetSwitchBridge(tLocalVidBrdSrc, bySrcChnnl+1, MODE_VIDEO);
			}
			/*[2011/09/02/zhangli]vmp有两出，svmp有四出
			if (MPU_SVMP == byVmpSubType || VMP_8KE == byVmpSubType)
			{
				g_cMpManager.SetSwitchBridge(tLocalVidBrdSrc, bySrcChnnl+1, MODE_VIDEO);
				g_cMpManager.SetSwitchBridge(tLocalVidBrdSrc, bySrcChnnl+2, MODE_VIDEO);
				g_cMpManager.SetSwitchBridge(tLocalVidBrdSrc, bySrcChnnl+3, MODE_VIDEO);
			}*/
			if (VMP != byVmpSubType)
			{
				u8 byMaxOutChnl = MAXNUM_MPU_OUTCHNNL;// MPU类型vmp有4路后适配,8KG有4路后适配,8KH有5路后适配
				// MPU2类型vmp有9路后适配
				if (MPU2_VMP_ENHACED == byVmpSubType ||
					MPU2_VMP_BASIC == byVmpSubType)
				{
					TKDVVMPOutParam tVMPOutParam = g_cMcuVcApp.GetVMPOutParam(m_tVmpEqp);
					byMaxOutChnl = tVMPOutParam.GetVmpOutCount();//需根据后适配情况确定个数
				}
				//建立交换桥
				for (bySrcChnnl = 1; bySrcChnnl < byMaxOutChnl; bySrcChnnl++)
				{
					g_cMpManager.SetSwitchBridge(tLocalVidBrdSrc, bySrcChnnl, MODE_VIDEO); 
				}

				//pgf:其实下面两个可以合并
                TVideoStreamCap tStrCap = m_tConf.GetMainSimCapSet().GetVideoCap();
                // [1/19/2011 xliang] 组播会议组播数据
                if (m_tConf.GetConfAttrb().IsMulticastMode())
                {
                    bySrcChnnl = GetVmpOutChnnlByRes(m_tVmpEqp.GetEqpId(),
						tStrCap.GetResolution(), 
                        tStrCap.GetMediaType(),
                        tStrCap.GetUserDefFrameRate(),
                        tStrCap.GetH264ProfileAttrb());
                    g_cMpManager.StartMulticast(tLocalVidBrdSrc, bySrcChnnl, MODE_VIDEO);
                    m_ptMtTable->SetMtMulticasting(tLocalVidBrdSrc.GetMtId());
                }
                
                //  [3/2/2012 pengguofeng]添加卫星会议交换
                if ( m_tConf.GetConfAttrb().IsSatDCastMode() )
                {
                    bySrcChnnl = GetVmpOutChnnlByRes(m_tVmpEqp.GetEqpId(),
						tStrCap.GetResolution(), 
                        tStrCap.GetMediaType(),
                        tStrCap.GetUserDefFrameRate(),
                        tStrCap.GetH264ProfileAttrb());
                    g_cMpManager.StartDistrConfCast(tLocalVidBrdSrc, MODE_VIDEO, bySrcChnnl);
                }
			}
        }
        else
        {
            //建视频组播交换
            if ( m_tConf.GetConfAttrb().IsSatDCastMode() )
            {
                g_cMpManager.StartDistrConfCast(tLocalVidBrdSrc, MODE_VIDEO, bySrcChnnl);
            }
        }
    }        

	TMt tLocalAudBrdSrc = GetLocalAudBrdSrc();
    //audio广播源
    if (!tLocalAudBrdSrc.IsNull())
    {
        bySrcChnnl = (tLocalAudBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;            
		u16 dSpyPort = SPY_CHANNL_NULL;
		CRecvSpy tSpyResource;
		
		if (!tLocalAudBrdSrc.IsLocal() && m_cSMcuSpyMana.GetRecvSpy(tLocalAudBrdSrc, tSpyResource))
		{
			dSpyPort  = tSpyResource.m_tSpyAddr.GetPort();
		}

		TAudioTypeDesc atAudTypeInfo[MAXNUM_CONF_AUDIOTYPE];
		u8 byAudCapNum = 1;
		if ( tLocalAudBrdSrc.GetEqpType() == EQP_TYPE_MIXER )
		{
			bySrcChnnl = GetMixerNModeChn();
			byAudCapNum = m_tConfEx.GetAudioTypeDesc(atAudTypeInfo);
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[AdjustSwitchBridge eMix AudBrdSrc]:EQPID=[%d]NmodeChnIndx=[%d]\n",m_tMixEqp.GetEqpId(),GetMixerNModeChn());
		}
		for (u8 byLoop = 0; byLoop < byAudCapNum; byLoop++)
		{
			g_cMpManager.SetSwitchBridge(tLocalAudBrdSrc, bySrcChnnl + byLoop, MODE_AUDIO, FALSE, dSpyPort);
		}
		//卫星分散会议，补建组播交换，转发板掉线，这里补建，转发板上线时也会重建一次
        //建组播交换首先找支持组播的转发板，如果没有则由第一块转发板建
        if ( m_tConf.GetConfAttrb().IsSatDCastMode() )
        {
            g_cMpManager.StartDistrConfCast(tLocalAudBrdSrc, MODE_AUDIO, bySrcChnnl);
        }
    }        

	if (m_tConf.m_tStatus.IsAudAdapting())
	{
		u8 byNum = 0;
		CBasChn *apcBasChn[MAXNUM_PERIEQP]={NULL};
		u8 byOutNum = 0;

		if(GetBasBrdChnGrp(byNum, apcBasChn, MODE_AUDIO))
		{
			for (u8 byIdx = 0; byIdx < byNum; byIdx++)
			{
				if ( NULL == apcBasChn[byIdx] )
				{
					continue;
				}
				TEqp tBas = apcBasChn[byIdx]->GetBas();
				u8 byBasChnl = apcBasChn[byIdx]->GetChnId();
				
				//不在线没必要补建桥
				if ( !g_cMcuVcApp.IsPeriEqpConnected(tBas.GetEqpId()) )
				{
					continue;
				}

				byOutNum = 0;
				u8 byFrontOutNum = 0;
				if ( !GetBasChnOutputNum(tBas,byBasChnl,byOutNum,byFrontOutNum) )
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[AdjustSwitchBridge] GetBasChnOutputNum:%d-%d failed!\n",
							tBas.GetEqpId(), byBasChnl
							);
					return FALSE;
				}

				for(u8 byOutIdx = 0; byOutIdx < byOutNum; ++byOutIdx)
				{
					g_cMpManager.SetSwitchBridge(tBas, byFrontOutNum + byOutIdx, MODE_AUDIO);
				}
			}
		}
	}
	
	if (m_tConf.m_tStatus.IsVidAdapting())
	{
		u8 byNum = 0;
		CBasChn *apcBasChn[MAXNUM_PERIEQP]={NULL};
		
		u8 byOutNum = 0;
		
		if(GetBasBrdChnGrp(byNum, apcBasChn, MODE_VIDEO))
		{
			for (u8 byIdx = 0; byIdx < byNum; byIdx++)
			{
				if ( NULL == apcBasChn[byIdx] )
				{
					continue;
				}
				TEqp tBas = apcBasChn[byIdx]->GetBas();
				tBas.SetConfIdx(m_byConfIdx);

				u8 byBasChnl = apcBasChn[byIdx]->GetChnId();
				//不在线没必要补建桥
				if ( !g_cMcuVcApp.IsPeriEqpConnected(tBas.GetEqpId()) )
				{
					continue;
				}
				byOutNum = 0;
				u8 byFrontOutNum = 0;
				if ( !GetBasChnOutputNum(tBas,byBasChnl,byOutNum,byFrontOutNum) )
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[AdjustSwitchBridge] GetBasChnOutputNum:%d-%d failed!\n",
						tBas.GetEqpId(), byBasChnl
						);
					return FALSE;
				}

				for(u8 byOutIdx = 0; byOutIdx < byOutNum; ++byOutIdx)
				{
					//g_cMpManager.StopSwitch(m_byConfIdx, ptSwitchChnnl->GetDstIP(), ptSwitchChnnl->GetRcvPort());

					g_cMpManager.SetSwitchBridge(tBas, byFrontOutNum + byOutIdx, MODE_VIDEO);
					if (g_cMcuVcApp.IsVASimultaneous())
					{
						g_cMpManager.SetSwitchBridge(tBas, byFrontOutNum + byOutIdx, MODE_AUDIO);
					}
				}
			}
		}
	} 

    //双流源
    if (!m_tDoubleStreamSrc.IsNull())
    {
        // zbq [09/04/2007] 若录像机双流放象，按对应端口建桥
        u8 bySrcChn = m_tPlayEqp == m_tDoubleStreamSrc ? m_byPlayChnnl : 0;        
        g_cMpManager.SetSwitchBridge(m_tDoubleStreamSrc, bySrcChn, MODE_SECVIDEO);

        //卫星分散会议，补建组播交换，转发板掉线，这里补建，转发板上线时也会重建一次
        //建组播交换首先找支持组播的转发板，如果没有则由第一块转发板建
        if ( m_tConf.GetConfAttrb().IsSatDCastMode() )
        {
            g_cMpManager.StartDistrConfCast(m_tDoubleStreamSrc, MODE_SECVIDEO, bySrcChn);
        }
    }


	//[2011/09/02/zhangli]选看和回传都不走桥交换，因此这里无需处理
//     //选看源
//     TMtStatus tStatus;
//     TMt tTmpMt;
// 	for (u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
//     {
//         if (!m_tConfAllMtInfo.MtJoinedConf(byLoop))
//         {
//             continue;
//         }      
// 		
// 		if (m_ptMtTable->GetMtStatus(byLoop, &tStatus))
//         {
//             tTmpMt = tStatus.GetSelectMt(MODE_VIDEO);
//             if (!tTmpMt.IsNull() && m_tConfAllMtInfo.MtJoinedConf(tTmpMt.GetMtId()))
//             {               
//                 g_cMpManager.SetSwitchBridge(tTmpMt, 0, MODE_VIDEO);
//             }
// 
//             tTmpMt = tStatus.GetSelectMt(MODE_AUDIO);
//             if (!tTmpMt.IsNull() && m_tConfAllMtInfo.MtJoinedConf(tTmpMt.GetMtId()))
//             {               
//                 if (!m_tConf.m_tStatus.IsMixing())
//                 {
// 					//tianzhiyong 2010/03/21 判断广播源是否为EAPU混音器
// 					if ( tTmpMt.GetEqpType() == EQP_TYPE_EMIXER )
// 					{
// 						g_cMpManager.SetSwitchBridge(tTmpMt, GetMixerNModeChn(), MODE_AUDIO);
// 						ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[AdjustSwitchBridge eMix SeeSrc]:EQPID=[%d]NmodeChnIndx=[%d]\n",tTmpMt.GetEqpId(),GetMixerNModeChn());
// 					}
// 					else
// 					{
// 						g_cMpManager.SetSwitchBridge(tTmpMt, 0, MODE_AUDIO);
// 					}
//                 }                    
//             }
//         }
//     }
// 
//     //回传终端
// 	if(m_tCascadeMMCU.GetMtId() != 0)
// 	{
// 		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
// 		if( NULL != ptConfMcInfo )
// 		{
// 			tTmpMt = ptConfMcInfo->m_tSpyMt;
// 		}
// 		if( ptConfMcInfo != NULL && !tTmpMt.IsNull() &&
//             m_ptMtTable->GetMtSndBitrate(tTmpMt.GetMtId()) > 0 )
// 		{
//             u8 byMode = MODE_BOTH;
//             if( TYPE_MT == m_ptMtTable->GetMainType( tTmpMt.GetMtId() ) && 
// 		        MT_MANU_KDC != m_ptMtTable->GetManuId( tTmpMt.GetMtId() ) ) 
//             {
//                 if (m_ptMtTable->IsMtAudioDumb(tTmpMt.GetMtId()) || 
//                     m_ptMtTable->IsMtAudioMute(tTmpMt.GetMtId()))
//                 {
//                     byMode = MODE_VIDEO;
// 					g_cMpManager.SetSwitchBridge(tTmpMt, 0, byMode);
//                 }
//             }
//             
//             if (m_tConf.m_tStatus.IsMixing())
//             {
//                 byMode = MODE_AUDIO;
// 				//tianzhiyong 2010/03/21 判断广播源是否为EAPU混音器
// 				if ( tTmpMt.GetEqpType() == EQP_TYPE_EMIXER )//为混音器
// 				{
// 					g_cMpManager.SetSwitchBridge(tTmpMt, GetMixerNModeChn(), MODE_AUDIO);
// 					OspPrintf(TRUE,FALSE,"[AdjustSwitchBridge eMix MMcu]:EQPID=[%d]NmodeChnIndx=[%d]\n",tTmpMt.GetEqpId(),GetMixerNModeChn());
// 				}
// 				else
// 				{
// 					g_cMpManager.SetSwitchBridge(tTmpMt, 0, MODE_AUDIO);
// 				}
//             }
// 		        
// 		}		    
// 	}

    return TRUE;    
}

/*====================================================================
函数名      ：Proc8khmMultiSwitch
功能        ：处理8000H-M的组播地址切换
算法实现    ：根据交换表来进行
引用全局变量：
输入参数说明：无
返回值说明  ：无
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013-6-6    4.7         彭国锋          创建
====================================================================*/
void CMcuVcInst::Proc8khmMultiSwitch()
{
#ifdef _8KH_
	if ( !g_cMcuAgent.Is8000HmMcu() )
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[Proc8khmMultiSwitch]non 8khm not need to handle\n");
		return;
	}
	
	if ( !m_tConf.GetConfAttrb().IsMulticastMode() )
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[Proc8khmMultiSwitch]non multicast not need to handle\n");
		return;
	}
	
	TConfSwitchTable *ptSwitchTable = g_cMcuVcApp.GetConfSwitchTable( m_byConfIdx );
	if ( !ptSwitchTable)
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[Proc8khmMultiSwitch]non switch table in conf:%d not need to handle\n", m_byConfIdx);
		return;
	}
	
	u8 byMpId = 16;
	u32 dwMultiCastAddr = g_cMcuVcApp.AssignMulticastIp(m_byConfIdx);
	
	for( u16 wLoop = 0; wLoop < MAX_SWITCH_CHANNEL; wLoop++ )
	{
		TSwitchChannel *ptSwitchChannel = ptSwitchTable->GetSwitchChannel( byMpId, wLoop );
		if( NULL == ptSwitchChannel || ptSwitchChannel->IsNull() )
		{
			continue;
		}
		
		if ( ptSwitchChannel->GetDstIP() != dwMultiCastAddr)
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[switch idx:%d]dstip:%x not the multicast IP\n", wLoop, ptSwitchChannel->GetDstIP() );
			continue;
		}
		
		u32 dwSndBindIp = ptSwitchChannel->GetSndBindIP();
		u32 dwModSndBindIp = GetSwitchSndBindIp();
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[Proc8khmMultiSwitch]SndBindIp is %x will change to :%x\n", dwSndBindIp, dwModSndBindIp);
		
		if ( dwModSndBindIp == 0 )
		{
			continue; //取出来0，则没必要修改原来的交换
		}
		
		//1.stop multicast switch:原因在于直接建，MP侧已经存在此交换，会直接回ACK，不作处理
		g_cMpManager.StartStopSwitch(m_byConfIdx, ptSwitchChannel->GetDstIP(), ptSwitchChannel->GetDstPort(), byMpId);
		
		//2.start multicast switch
		CServMsg cServMsg;
		cServMsg.SetConfIdx(m_byConfIdx);
		cServMsg.SetDstDriId(byMpId);
		TSwitchChannelExt tSwitchChannelExt;
		tSwitchChannelExt.SetNull();
		memcpy(&tSwitchChannelExt, ptSwitchChannel, sizeof(TSwitchChannel));
		tSwitchChannelExt.SetSndBindIP(dwModSndBindIp);
		cServMsg.SetMsgBody((u8 *)&tSwitchChannelExt, sizeof(TSwitchChannelExt));
		
		
		u8 bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_NONE;
		u8 byUniformPayload = INVALID_PAYLOAD;
		u8 byIsSwitchRtcp = 1;
		cServMsg.CatMsgBody((u8 *)&bySwitchChannelMode, sizeof(u8));
		
		cServMsg.CatMsgBody((u8 *)&byUniformPayload, sizeof(u8));
		
		cServMsg.CatMsgBody((u8 *)&byIsSwitchRtcp, sizeof(u8));
		
		g_cMpSsnApp.SendMsgToMpSsn(byMpId, MCU_MP_ADDSWITCH_REQ, 
			cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[Proc8khmMultiSwitch]send MCU_MP_ADDSWITCH_REQ to MP:16\n");
		break;
	}
	
#endif
	return;
}

/*====================================================================
    函数名      ：ShowConfSwitch
    功能        ：打印会议交换信息
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/10    3.0         胡昌威          创建
====================================================================*/
void CMcuVcInst::ShowConfSwitch( void )
{
	u8 byMpId, byLoop1;
    u16 wLoop2;
	u32 dwSrcIp, dwDisIp, dwRcvIp, dwDstIp;
	u16 wRcvPort, wDstPort;
	TSwitchChannel *ptSwitchChannel;
	char achTemp[255];
	int l;
	
	for( byLoop1 = 1; byLoop1 <= MAXNUM_DRI; byLoop1++ )
	{
		if( g_cMcuVcApp.IsMpConnected( byLoop1 ) )
		{            
			StaticLog("\nMp%d(%s) switch info: ", byLoop1 , StrOfIP(g_cMcuVcApp.GetMpIpAddr(byLoop1)) );
			for( wLoop2 = 0; wLoop2 < MAX_SWITCH_CHANNEL; wLoop2++ )
			{
				ptSwitchChannel = m_ptSwitchTable->GetSwitchChannel( byLoop1, wLoop2 );
				if( !ptSwitchChannel->IsNull() )
				{
					dwSrcIp = ptSwitchChannel->GetSrcIp();
					dwDisIp = ptSwitchChannel->GetDisIp();
					dwRcvIp = ptSwitchChannel->GetRcvIP();
					dwDstIp = ptSwitchChannel->GetDstIP();
                    wRcvPort = ptSwitchChannel->GetRcvPort();
					wDstPort = ptSwitchChannel->GetDstPort();

					memset( achTemp, ' ', 255 );
					l = 0;

					//SrcIp
					byMpId = g_cMcuVcApp.FindMp( dwSrcIp );
					if( byMpId > 0 )
					{
						l = sprintf( achTemp, "\n%d  %s:%d(mp%d)", wLoop2, StrOfIP( dwSrcIp ), wRcvPort, byMpId );
					}
					else
					{
						l = sprintf( achTemp, "\n%d  %s:%d(src)", wLoop2, StrOfIP( dwSrcIp ), wRcvPort );
					}
					
					//DisIp
					memset( achTemp+l, ' ', 255-l );
					if( dwDisIp > 0 )
					{
						byMpId = g_cMcuVcApp.FindMp( dwDisIp );
						if( byMpId > 0 )
						{
							l += sprintf( achTemp+l, "-->%s:%d(mp%d)", StrOfIP( dwDisIp ), wRcvPort, byMpId );
						}
						else
						{
							l += sprintf( achTemp+l, "-->%s:%d(src)", StrOfIP( dwDisIp ), wRcvPort );
						}	
					}

					//RcvIp
					memset( achTemp+l, ' ', 255-l );
					byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
					if( byMpId > 0 )
					{
						l += sprintf( achTemp+l, "-->%s:%d(mp%d)", StrOfIP( dwRcvIp ), wRcvPort, byMpId );
					}
					else
					{
						l += sprintf( achTemp+l, "-->%s:%d(dst)", StrOfIP( dwRcvIp ), wRcvPort );
					}
					
					//DstIp
					memset( achTemp+l, ' ', 255-l );
					byMpId = g_cMcuVcApp.FindMp( dwDstIp );
					if( byMpId > 0 )
					{
						l += sprintf( achTemp+l, "-->%s:%d(mp%d)", StrOfIP( dwDstIp ), wDstPort, byMpId );
					}
					else
					{
						l += sprintf( achTemp+l, "-->%s:%d(dst)", StrOfIP( dwDstIp ), wDstPort );
					}

					StaticLog(achTemp );
					
				}
			}
		}
	}
}

//[liu lijiu][20100902]建立到本级会议回传通道的RTCP交换
/*====================================================================
    函数名      ：SwitchLocalMultipleSpyRtcp
    功能        ：建立到本级会议回传通道的RTCP交换
    算法实现    ：
    引用全局变量：
    输入参数说明：TMt    tSrc:              源终端
	              u8     bySrcChnnl         源终端的通道索引
				  TMt    tDst               回传源
	              u8     byMode:            交换模式
	              BOOL32 bIsBuildSwitch     拆交换(false)或建交换(true)
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期        版本        修改人        修改内容
    2010/08/28    4.6         刘利九         创建
====================================================================*/
// void CMcuVcInst::SwitchLocalMultipleSpyRtcp( TMt tSrc, u8 bySrcChnnl, TMt tDst, u8 byMode, BOOL32 bIsBuildSwitch)
// {
// 	if (MODE_NONE == byMode || tSrc.IsNull() || tDst.IsNull())
// 	{
// 		LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[SwitchLocalMultipleSpyRtcp] Wrong params, conn't build rtcp!\n");
// 		return;
// 	}
// 	
// 	u32 dwMpIpAddr = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( GetLocalMtFromOtherMcuMt(tDst).GetMtId()));
// 	CRecvSpy tRcvSpyInfo;
// 	if( !m_cSMcuSpyMana.GetRecvSpy(tDst, tRcvSpyInfo))
// 	{
// 		LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[SwitchLocalMultipleSpyRtcp] invalid spy channel!\n");
// 		return;
// 	}
// 
// 	if ( bIsBuildSwitch )//建交换
// 	{
// 		if (TYPE_MT == tSrc.GetMtType())//终端类型
// 		{
// 			TLogicalChannel tLogicalChannel;
// 			//建视频的RTCP交换
// 			if( MODE_VIDEO == byMode || MODE_BOTH == byMode )
// 			{
// 				if(m_ptMtTable->GetMtLogicChnnl( tSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, TRUE ))
// 				{
// 					g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, 
// 													tLogicalChannel.GetRcvMediaCtrlChannel().GetIpAddr(), 0, 
// 													tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr(), 
// 													tLogicalChannel.GetSndMediaCtrlChannel().GetPort(),
// 													dwMpIpAddr, 
// 					                                tRcvSpyInfo.m_tSpyAddr.GetPort() + 1, 
// 													0, 0, 
// 													tLogicalChannel.GetRcvMediaChannel().GetIpAddr(), 
// 													tLogicalChannel.GetRcvMediaChannel().GetPort());
// 				}
// 				
// 			}
// 
// 			//建音频的RTCP交换
// 			if( MODE_AUDIO == byMode || MODE_BOTH == byMode )
// 			{
// 				if(m_ptMtTable->GetMtLogicChnnl( tSrc.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, TRUE ))
// 				{
// 					g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, 
// 													 tLogicalChannel.GetRcvMediaCtrlChannel().GetIpAddr(), 0, 
// 													 tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr(),
// 													 tLogicalChannel.GetSndMediaCtrlChannel().GetPort(), 
// 													 dwMpIpAddr, 
// 					                                 tRcvSpyInfo.m_tSpyAddr.GetPort() + 3,
// 													 0, 0, 
// 													 tLogicalChannel.GetRcvMediaChannel().GetIpAddr(), 
// 													 tLogicalChannel.GetRcvMediaChannel().GetPort());
// 				}
// 			}
// 		}
// 
// 		else if(TYPE_MCUPERI == tSrc.GetMtType())//外设类型
// 		{
// 			u32 dwSwitchIpAddr = 0;
// 			u16 wSwitchPort    = 0;
// 			u16 wRtcpBackPort  = 0;
// 			
// 			switch(tSrc.GetEqpType())
// 			{
// 			case EQP_TYPE_VMP://画面合成
// 				{
// #ifdef _8KE_
// 					u16 wEqpStartPort = 0;					
// 					g_cMpManager.GetSwitchInfo( tSrc.GetEqpId(), dwSwitchIpAddr, wSwitchPort, wEqpStartPort);
// #else
// 					g_cMpManager.GetVmpSwitchAddr(tSrc.GetEqpId(), dwSwitchIpAddr, wSwitchPort);
// #endif
// 					
// 					if (bySrcChnnl >= 16)
// 					{
// 						wRtcpBackPort = wSwitchPort + 6 + bySrcChnnl;
// 					}
// 					if (bySrcChnnl >= 8)
// 					{
// 						wRtcpBackPort = wSwitchPort + 4 + bySrcChnnl;
// 					}
// 					else
// 					{
// 						wRtcpBackPort = wSwitchPort + 2 + bySrcChnnl;
// 					}
// 					
// 					g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwSwitchIpAddr, 0, dwSwitchIpAddr, wRtcpBackPort, dwMpIpAddr, 
// 						tRcvSpyInfo.m_tSpyAddr.GetPort() + 1);
// 					break;
// 				}
// 			default:
// 				{
// 					;
// 				}
// 			}
// 		}
// 	}
// 	else//拆交换
// 	{
// 		if (TYPE_MT == tSrc.GetMtType())
// 		{
// 			TLogicalChannel tLogicalChannel;
// 			//拆视频的RTCP交换
// 			if( MODE_VIDEO == byMode || MODE_BOTH == byMode )
// 			{
// 				if(m_ptMtTable->GetMtLogicChnnl( tSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, TRUE ))
// 				{
// 					u32 dwVideoRtcpIpAddr = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
// 					u16 wVideoRtcpPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
// 					g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwVideoRtcpIpAddr, wVideoRtcpPort, dwMpIpAddr, tRcvSpyInfo.m_tSpyAddr.GetPort() + 1);
// 					
// 				}
// 			}	
// 			
// 			//拆音频的rtcp交换
// 			if( MODE_AUDIO == byMode || MODE_BOTH == byMode )
// 			{
// 				if(m_ptMtTable->GetMtLogicChnnl( tSrc.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, TRUE ))
// 				{
// 					u32 dwAudioRtcpIpAddr = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
// 					u16 wAudioRtcpPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
// 					g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwAudioRtcpIpAddr, wAudioRtcpPort, dwMpIpAddr, tRcvSpyInfo.m_tSpyAddr.GetPort() + 3);
// 				}
// 			}
// 			
// 		}
// 		//为外设时
// 		else if(TYPE_MCUPERI == tSrc.GetType())
// 		{
// 			u32 dwSwitchIpAddr = 0;
// 			u16 wSwitchPort    = 0;
// 			u16 wRtcpBackPort  = 0;
// 			
// 			switch(tSrc.GetEqpType())
// 			{
// 			case EQP_TYPE_VMP://画面合成
// 				{
// #ifdef _8KE_
// 					u16 wEqpStartPort = 0;					
// 					g_cMpManager.GetSwitchInfo( tSrc.GetEqpId(), dwSwitchIpAddr, wSwitchPort, wEqpStartPort);
// #else
// 					g_cMpManager.GetVmpSwitchAddr(tSrc.GetEqpId(), dwSwitchIpAddr, wSwitchPort);
// #endif
// 					
// 					if (bySrcChnnl >= 16)
// 					{
// 						wRtcpBackPort = wSwitchPort + 6 + bySrcChnnl;
// 					}
// 					if (bySrcChnnl >= 8)
// 					{
// 						wRtcpBackPort = wSwitchPort + 4 + bySrcChnnl;
// 					}
// 					else
// 					{
// 						wRtcpBackPort = wSwitchPort + 2 + bySrcChnnl;
// 					}
// 					g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwSwitchIpAddr, wRtcpBackPort, dwMpIpAddr, tRcvSpyInfo.m_tSpyAddr.GetPort() + 1);
// 					break;
// 				}
// 			default:
// 				{
// 					;
// 				}
// 			}
// 		}
// 	}
// }
/*====================================================================
    函数名      ：SwitchVideoRtcpToDst
    功能        ：将视频流的Rtcp交换到目的
    算法实现    ：
    引用全局变量：
    输入参数说明：u32 dwDstIp, 目的Ip 
	              u16 wDstPort, 目的端口
	              TEqp tSrcEqp, 源终端或设备
	              u8 bySrcChnnl 源通道号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/10/20    3.0         胡昌威         创建
	2011/08/11  4.6         zhangli        修改
====================================================================*/
//[2011/08/11/zhanlgi]去掉参数bySwitchMode，此参数没有用
void CMcuVcInst::AddRemoveRtcpToDst(u32 dwDstIp, u16 wDstPort, TMt tSrc, u8 bySrcChnnl, 
									u8 byMode, /*u8 bySwitchMode,*/ BOOL32 bMapSrcAddr,
									u16 wSpyStartPort, BOOL32 bAddRtcp /* = TRUE*/)
{
	if (MODE_VIDEO2SECOND == byMode)
	{
		byMode = MODE_VIDEO;
	}

    u32 dwSrcIpAddr = 0, dwRecvIpAddr = 0, dwOldDstIp = 0, dwMapIpAddr = 0;
    u16 wRecvPort = 0, wOldDstPort = 0, wMapPort = 0;
    u8  byChlNum = 0;
    u8  byChanType = 0;

    u32 dwSwitchIpAddr;
    u16 wSwitchPort;

    TLogicalChannel tLogicalChannel;

    ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[AddRemoveRtcpToDst] dwDstIp:%s, wDstPort:%u, tSrc<%d,%d>, bySrcChnnl:%d, byMode:%d\n",
            StrOfIP(dwDstIp), wDstPort, tSrc.GetMcuId(), tSrc.GetMtId(), bySrcChnnl, byMode);

	wMapPort    = 0;
	dwMapIpAddr = 0;

	switch(tSrc.GetType()) 
	{
	case TYPE_MT:
		{
			TMt tTempPrsSrc = GetLocalMtFromOtherMcuMt(tSrc);
			if ( IsMultiCastMt(tTempPrsSrc.GetMtId()) )
			{
				ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_PRS,"[AddRemoveRtcpToDst]tTempPrsSrc:%d-%d is multicastmt!\n",tTempPrsSrc.GetMcuId(), tTempPrsSrc.GetMtId());
				return ;
			}

			if (MODE_SECVIDEO == byMode)
			{
				m_ptMtTable->GetMtRtcpDstAddr(tSrc.GetMtId(), dwOldDstIp, wOldDstPort, MODE_VIDEO);
				wOldDstPort = wOldDstPort + 4;
			}
			else
			{
				m_ptMtTable->GetMtRtcpDstAddr(tSrc.GetMtId(), dwOldDstIp, wOldDstPort, byMode);
			}
			if (MODE_VIDEO == byMode)
			{
				byChanType = LOGCHL_VIDEO;
			}
			else if (MODE_AUDIO == byMode)
			{
				byChanType = LOGCHL_AUDIO;
			}
			else if (MODE_SECVIDEO == byMode)
			{
				byChanType = LOGCHL_SECVIDEO;
			}
			m_ptMtTable->GetMtLogicChnnl(tSrc.GetMtId(), byChanType, &tLogicalChannel, TRUE);

			if( SPY_CHANNL_NULL == wSpyStartPort )
			{
				wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
			}
			else
			{
				if( MODE_VIDEO == byMode )
				{
					wRecvPort = wSpyStartPort + 1;
				}
				else if( MODE_AUDIO == byMode )
				{
					wRecvPort = wSpyStartPort + 3;
				}
				else
				{
					wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
				}
			}
			//将 MT.RTCP -> PRS 时，考虑到防火墙支撑，需将交换包的源ip、port映射为 MT.RTP，以便PRS重传
			if (bMapSrcAddr)
			{
				wMapPort    = tLogicalChannel.GetRcvMediaChannel().GetPort();
				dwMapIpAddr = tLogicalChannel.GetRcvMediaChannel().GetIpAddr();
			}
		}
		break;

	case TYPE_MCUPERI://外设
		if ( MODE_SECVIDEO != byMode)
		{
			g_cMcuVcApp.GetEqpRtcpDstAddr(tSrc.GetEqpId(), bySrcChnnl, dwOldDstIp, wOldDstPort, byMode);
			g_cMcuVcApp.GetPeriEqpLogicChnnl(tSrc.GetEqpId(), byMode, &byChlNum, &tLogicalChannel, TRUE);
			switch(tSrc.GetEqpType())
			{
			case EQP_TYPE_VMP:
				{       
					BOOL32 bGetSwitchInfo = FALSE;
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
					u16 wEqpStartPort = 0;					
					bGetSwitchInfo = g_cMpManager.GetSwitchInfo( tSrc.GetEqpId(), dwSwitchIpAddr, wSwitchPort, wEqpStartPort);
#else
					bGetSwitchInfo = g_cMpManager.GetVmpSwitchAddr(tSrc.GetEqpId(), dwSwitchIpAddr, wSwitchPort);
#endif
					if (bGetSwitchInfo)
					{
						//[2011/08/11/zhangli]去掉参数bySwitchMode，这里取出的端口是40701，这个端口vmp接收码流端口
// 						if (SWITCH_MODE_BROADCAST == bySwitchMode)
// 						{
// 							wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort() + PORTSPAN*bySrcChnnl;
// 						}
// 						else
						{
							//  xsl [6/3/2006] 画面合成接收码流通道，mp侧RTCP端口分配
							// xliang [5/6/2009] 分配原则 0~7	===> 2~9 
							//							  8~15	===> 12~19
							//16~23	===> 22~29	如vmp侧rtp发送从39000开始则端口为：39022-39029
							//24~25 ====> 32~33	如vmp侧rtp发送从40700开始则端口为：39032-39033
							if (bySrcChnnl >= 24)
							{
								wRecvPort = wSwitchPort+8+bySrcChnnl;
							}
							else if (bySrcChnnl >= 16)
							{
								wRecvPort = wSwitchPort + 6 + bySrcChnnl;
							}
							else if (bySrcChnnl >= 8)
							{
								wRecvPort = wSwitchPort + 4 + bySrcChnnl;
							}
							else
							{
								wRecvPort = wSwitchPort + 2 + bySrcChnnl;
							}                        
						}
					}
					else
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "AddRemoveRtcpToDst - GetVmpSwitchAddr error!\n");
					}
				}
				break;  
				
			case EQP_TYPE_BAS:
				{
					//[nizhijun 2010/9/8] BAS的申述，rtcp端口按照byOutChnNum计算
					u8 byOutChnNum = 0;
					u8 byFrontOutNum = 0;
					if ( !GetBasChnOutputNum(tSrc,bySrcChnnl,byOutChnNum,byFrontOutNum) )
					{
						ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[AddRemoveRtcpToDst] GetBasChnOutputNum:%d-%d failed!\n",
								tSrc.GetEqpId(), bySrcChnnl
								);
						return ;
					}
					wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort() + PORTSPAN*byFrontOutNum;
				}
				break;
			case EQP_TYPE_RECORDER:
				{
					wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort() + PORTSPAN * bySrcChnnl;
				}
				break;
			default:
				wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort() + PORTSPAN*bySrcChnnl;
				break;
			}
			//将 MT.RTCP -> PRS 时，考虑到防火墙支撑，需将交换包的源ip、port映射为 MT.RTP，以便PRS重传
			if (bMapSrcAddr)
			{
				//[nizhijun 2012/06/08]录像机的端口需要计算偏移，因为存在多个通道
				if ( EQP_TYPE_RECORDER == tSrc.GetEqpType() )
				{
					wMapPort    = tLogicalChannel.GetRcvMediaChannel().GetPort()+PORTSPAN*bySrcChnnl;
				}
				else
				{
					wMapPort    = tLogicalChannel.GetRcvMediaChannel().GetPort();
				}
				dwMapIpAddr = tLogicalChannel.GetRcvMediaChannel().GetIpAddr();
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[AddRemoveRtcpToDst]dwMapIpAddr: %s, wMapPort: %d\n", StrOfIP(dwMapIpAddr), wMapPort);
			}
		}
		else//[liu lijiu][20100812][add]双流模式的处理start
		{
			//[2011/08/18/zhangli]对于外设，某个通道保存的wOldDstPort没有-4，因此这里不用+4
			g_cMcuVcApp.GetEqpRtcpDstAddr(tSrc.GetEqpId(), bySrcChnnl, dwOldDstIp, wOldDstPort, MODE_VIDEO);
			//wOldDstPort = wOldDstPort + 4;//双流的端口比视频的端口多4
			g_cMcuVcApp.GetPeriEqpLogicChnnl(tSrc.GetEqpId(), MODE_VIDEO, &byChlNum, &tLogicalChannel, TRUE);
			switch(tSrc.GetEqpType())
			{
			case EQP_TYPE_VMP:
				{       
					if (g_cMpManager.GetVmpSwitchAddr(tSrc.GetEqpId(), dwSwitchIpAddr, wSwitchPort))
					{
// 						if (SWITCH_MODE_BROADCAST == bySwitchMode)
// 						{
// 							wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort() + PORTSPAN*bySrcChnnl + 4;
// 						}
//						else
						{
							//  xsl [6/3/2006] 画面合成接收码流通道，mp侧RTCP端口分配
							// xliang [5/6/2009] 分配原则 0~7	===> 2~9 
							//							  8~15	===> 12~19
							//16~23	===> 22~29	如vmp侧rtp发送从39000开始则端口为：39022-39029
							//24~25 ====> 32~33	如vmp侧rtp发送从40700开始则端口为：39032-39033
							if (bySrcChnnl >= 24)
							{
								wRecvPort = wSwitchPort+8+bySrcChnnl;
							}
							else if (bySrcChnnl >= 16)
							{
								wRecvPort = wSwitchPort + 6 + bySrcChnnl;
							}
							else if (bySrcChnnl >= 8)
							{
								wRecvPort = wSwitchPort + 4  + bySrcChnnl;
							}
							else
							{
								wRecvPort = wSwitchPort + 2 + bySrcChnnl;
							}                        
						}
					}
					else
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_PRS, "AddRemoveRtcpToDst - GetVmpSwitchAddr error!\n");
					}
				}
				break;  
				
			case EQP_TYPE_BAS:
				{
					//[nizhijun 2010/9/8] BAS的申述，rtcp端口按照byOutChnNum计算
					u8 byOutChnNum = 0;
					//[nizhijun 2010/9/9] 目前BAS的双流申请端口还是原来端口，不需要+4,这里就把前面加的减掉
					//wOldDstPort = wOldDstPort - 4;
					u8 byFrontOutNum = 0;
					if ( !GetBasChnOutputNum(tSrc,bySrcChnnl,byOutChnNum,byFrontOutNum) )
					{
						ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[AddRemoveRtcpToDst] GetBasChnOutputNum:%d-%d failed!\n",
							tSrc.GetEqpId(), bySrcChnnl
							);
						return ;
					}

					wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort() + PORTSPAN*byFrontOutNum;
				}
				break;
			case EQP_TYPE_RECORDER:
				{
					wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort() + 4 + PORTSPAN * bySrcChnnl;
				}
				break;
			default:
				wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort() + 4 + PORTSPAN*bySrcChnnl;
				break;
			}
			//将 MT.RTCP -> PRS 时，考虑到防火墙支撑，需将交换包的源ip、port映射为 MT.RTP，以便PRS重传
			if (bMapSrcAddr)
			{
				//[nizhijun 2012/06/08]录像机的端口需要计算偏移，因为存在多个通道
				if ( EQP_TYPE_RECORDER == tSrc.GetEqpType() )
				{
					wMapPort    = tLogicalChannel.GetRcvMediaChannel().GetPort()+PORTSPAN*bySrcChnnl+4;
				}
				else
				{
					wMapPort    = tLogicalChannel.GetRcvMediaChannel().GetPort() + 4;
				}
				dwMapIpAddr = tLogicalChannel.GetRcvMediaChannel().GetIpAddr();
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[AddRemoveRtcpToDst]dwMapIpAddr: %s, wMapPort: %d\n", StrOfIP(dwMapIpAddr), wMapPort);
			}			
		}
		break;

	default:
		break;
	}

    if (TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_TVWALL == tSrc.GetEqpType())
    {
        TPeriEqpStatus tTWStatus;
        if ( bySrcChnnl < MAXNUM_PERIEQP_CHNNL && g_cMcuVcApp.GetPeriEqpStatus(tSrc.GetEqpId(), &tTWStatus))
        {
            TMt tMt = (TMt)tTWStatus.m_tStatus.tTvWall.atVideoMt[bySrcChnnl];
            u32 dwMtSwitchIp;
            u16 wMtSwitchPort;
            u32 dwMtSrcIp;

            g_cMpManager.GetSwitchInfo(tMt, dwMtSwitchIp, wMtSwitchPort, dwMtSrcIp);

            dwSrcIpAddr = dwMtSwitchIp;
            dwRecvIpAddr = dwMtSwitchIp;
        }
    }
    else
    {
        dwSrcIpAddr = tLogicalChannel.GetRcvMediaChannel().GetIpAddr();
        dwRecvIpAddr = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
    }

	if(IsValidHduEqp(tSrc))
    {
        TPeriEqpStatus tHduStatus;
        if ( bySrcChnnl < MAXNUM_HDU_CHANNEL && g_cMcuVcApp.GetPeriEqpStatus(tSrc.GetEqpId(), &tHduStatus))
        {
            TMt tMt = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[bySrcChnnl];
            u32 dwMtSwitchIp;
            u16 wMtSwitchPort;
            u32 dwMtSrcIp;
			
            g_cMpManager.GetSwitchInfo(tMt, dwMtSwitchIp, wMtSwitchPort, dwMtSrcIp);
			
            dwSrcIpAddr = dwMtSwitchIp;
            dwRecvIpAddr = dwMtSwitchIp;
        }
    }
    else
    {
        dwSrcIpAddr = tLogicalChannel.GetRcvMediaChannel().GetIpAddr();
        dwRecvIpAddr = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
    }

    //交换到新的目的前移除老的交换
	u32 dwSaveDstIp = 0;
	u16 wSaveDstPort = 0;
    if (0 != dwOldDstIp)
    {
        g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwOldDstIp, wOldDstPort);
    }

	g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwDstIp, wDstPort);

	if (bAddRtcp)
	{
		dwSaveDstIp = dwDstIp;
		wSaveDstPort= wDstPort;

		//添加交换
		g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwSrcIpAddr, 0, dwRecvIpAddr,wRecvPort, 
			dwDstIp, wDstPort, 0, 0, dwMapIpAddr, wMapPort);

		LogPrint(LOG_LVL_DETAIL, MID_MCU_PRS, "[AddRemoveRtcpToDst]dwRecvIpAddr::%s,wRecvPort%d\n", StrOfIP(dwRecvIpAddr), wRecvPort);
	} 
	
	//记录交换目的
	if(MODE_SECVIDEO == byMode)//[liu lijiu][20100814][add]双流模式
	{
		switch(tSrc.GetType()) 
		{
		case TYPE_MT://终端	
			m_ptMtTable->SetMtRtcpDstAddr(tSrc.GetMtId(), dwSaveDstIp, ((wSaveDstPort == 0) ? 0: (wSaveDstPort - 4)), MODE_VIDEO);
			break;
			
		case TYPE_MCUPERI://外设
			g_cMcuVcApp.SetEqpRtcpDstAddr(tSrc.GetEqpId(), bySrcChnnl, dwSaveDstIp, wSaveDstPort, MODE_VIDEO);
			break;

		default:
			break;
		}
	}//[liu lijiu][20100814][add]
	else//非双流模式
	{
		switch(tSrc.GetType()) 
		{
		case TYPE_MT://终端	
			m_ptMtTable->SetMtRtcpDstAddr(tSrc.GetMtId(), dwSaveDstIp, wSaveDstPort, byMode);
			break;
			
		case TYPE_MCUPERI://外设
			g_cMcuVcApp.SetEqpRtcpDstAddr(tSrc.GetEqpId(), bySrcChnnl, dwSaveDstIp, wSaveDstPort, byMode);
			break;

		default:
			break;
		}
	}

	return;
}

/*====================================================================
函数名      ：BuidRtcpSwitchSrcToDst
功能        ：建立Src到Dst的RTCP交换
算法实现    ：
引用全局变量：
输入参数说明：TMt tSrc, RTCP包发送者，即RTP包接收方 终端/外设
			  TMt tDst，RTCP包接受者，即RTP包发送者 终端/回传通道/外设
			  u8 byMediaMode，模式，音/视频
			  u8 bySrcChnnl   tSrc通道号
			  u8 bySwitchMode 交换模式：选看/广播
			  u8 byDstChnnl  tDst的通道，目前仅在选看VMP时使用
返回值说明  ：无
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2011/01/06  4.6         倪志俊        创建
2011/08/11  4.6         zhangli        修改
====================================================================*/
//[2011/08/11/zhanlgi]去掉参数bySwitchMode，此参数没有用
void CMcuVcInst::BuildRtcpSwitchForSrcToDst(TMt tSrc, TMt tUnlocalDst, u8 byMediaMode, 
										   u8 bySrcChnnl /* = 0 */, 
										   /*u8 bySwitchMode = SWITCH_MODE_BROADCAST,*/
										   u8 byDstChnnl /*= 0*/,
										   BOOL32 bAddRtcp/* = TRUE*/)
{
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	if (!tConfAttrb.IsResendLosePack())
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchForSrcToDst]Conf:%d is not support PRS\n",m_byConfIdx);
		return;
	}
	
	if (tSrc.IsNull() || tUnlocalDst.IsNull())
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst] tSrc's or tUnlocalDst's Tmt Info is Null\n");
		return;
	}

	//申述源是非keda的不支持
	if(TYPE_MT == tSrc.GetType() && !IsKedaMt(tSrc, TRUE))
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[BuidRtcpSwitchSrcToDst] tSrc:%d-%d isn't mt of kedacom!\n",tSrc.GetMcuId(), tSrc.GetMtId());
		return;
	} 

	TMt tTempPrsSrc = GetLocalMtFromOtherMcuMt(tSrc);
	if ( TYPE_MT == tTempPrsSrc.GetType() && IsMultiCastMt(tTempPrsSrc.GetMtId()) )
	{
		ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_PRS,"[BuildRtcpSwitchForSrcToDst]tTempPrsSrc:%d-%d is multicastmt!\n",tTempPrsSrc.GetMcuId(), tTempPrsSrc.GetMtId());
		return;
	}

	//被申述目的是非keda的不支持
	if(TYPE_MT == tUnlocalDst.GetType() && !IsKedaMt(tUnlocalDst, TRUE))
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[BuidRtcpSwitchSrcToDst] tUnlocalDst:%d-%d isn't mt of kedacom!\n",tUnlocalDst.GetMcuId(), tUnlocalDst.GetMtId());
		return;
	} 
	
	if (MODE_NONE == byMediaMode)
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst] MeidaMode is Null\n");
		return;
	}
	
	if (MODE_BOTH == byMediaMode)
	{
		BuildRtcpSwitchForSrcToDst(tSrc, tUnlocalDst, MODE_VIDEO, bySrcChnnl, byDstChnnl, bAddRtcp);
		BuildRtcpSwitchForSrcToDst(tSrc, tUnlocalDst, MODE_AUDIO, bySrcChnnl, byDstChnnl, bAddRtcp);
		return;
	}

// 	if (TYPE_MCUPERI == tSrc.GetType())
// 	{
// 		switch (tSrc.GetEqpType())
// 		{
// 		case EQP_TYPE_RECORDER:
// 			//会议录像 && 非适配录像则是占用prs的，返回
// 			if (tSrc == m_tRecEqp && bySrcChnnl == m_byRecChnnl && !IsRecNeedAdapt())
// 			{
// 				return;
// 			}
// 		case EQP_TYPE_PRS:
// 			return;
// 		default:
// 			break;
// 		}
// 	}

	u8 bySrcMode = byMediaMode;
	u8 byDstMode = byMediaMode;
	if (MODE_VIDEO2SECOND == byMediaMode)
	{
		bySrcMode = MODE_SECVIDEO;
		byDstMode = MODE_VIDEO;
	}
	TTransportAddr tRemoteAddr;
	if (!GetRemoteRtcpAddr(tUnlocalDst, byDstChnnl, byDstMode, tRemoteAddr))
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_PRS, "[BuidRtcpSwitchForSrcToDst] GetRemoteRtcpAddr failed,dstMt:%d-%d!\n",
			tUnlocalDst.GetMcuId(), tUnlocalDst.GetMtId());
		return;
	}
	
	AddRemoveRtcpToDst(tRemoteAddr.GetIpAddr(),tRemoteAddr.GetPort(), 
		tSrc, bySrcChnnl, bySrcMode, TRUE, SPY_CHANNL_NULL, bAddRtcp);
// 	u32 dwRtcpDstIp   = 0;
// 	u16 wRctpDstPort  = 0;
// 	
// 	if (tUnlocalDst.GetType() == TYPE_MCUPERI && EQP_TYPE_VMP == tUnlocalDst.GetEqpType())
// 	{
// 		u32 dwEqpIP,dwRcvIp;
//         u16 wRcvPort;
// 		if(g_cMpManager.GetSwitchInfo(tUnlocalDst, dwRcvIp, wRcvPort, dwEqpIP))
// 		{
// 			dwRtcpDstIp =  dwEqpIP;
// 			wRctpDstPort = wRcvPort + PORTSPAN * byDstChnnl + 1;
// 			if (dwRtcpDstIp>0 && wRctpDstPort>0)
// 			{
// 				LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]Dst is VMP:tUnlocalDst(EQPID:%d) SwitchVideoRtcpToDst begin,Mode:%d",
// 					tUnlocalDst.GetEqpId(),MODE_VIDEO);
// 				AddRemoveRtcpToDst(dwRtcpDstIp,wRctpDstPort,tSrc,bySrcChnnl,byMediaMode,bySwitchMode,TRUE);
// 			}
// 			else
// 			{
// 				LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]SingleSpy:tUnlocalDst(%d-%d) GetSndMediaCtrlChannel failed!Mode:%d\n",
// 					tUnlocalDst.GetMcuId(),tUnlocalDst.GetMtId(),byMediaMode);
// 				return;
// 			}
// 		}
// 		else
// 		{
// 			LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchForSrcToDst]tUnlocalDst(%d) is VMP,Fail to get his switch info!\n",tUnlocalDst.GetEqpId());
// 			return;
// 		}
// 	}
// 	else
// 	{
// 		//此接口不区分RTCP 申述源是终端或者外设，具体区分在下述SwitchVideoRtcpToDst和SwitchLocalMultipleSpyRtcp中区分。
// 		TLogicalChannel tLogicalChannel;
// 		BOOL32 bIsSingleSpy = (!tUnlocalDst.IsLocal()) && ( !IsLocalAndSMcuSupMultSpy(tUnlocalDst.GetMcuId()) );  //是否支持单回传
// 		
// 		if (tUnlocalDst.IsLocal() || bIsSingleSpy)
// 		{	
// 			//如果目的终端是本级或者单回传情况下的下级，将目的终端本地化
// 			TMt tLocalDst = GetLocalMtFromOtherMcuMt(tUnlocalDst);
// 			if (MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode)
// 			{
// 				if ( m_ptMtTable->GetMtLogicChnnl(tLocalDst.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE) )
// 				{
// 					dwRtcpDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
// 					wRctpDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
// 					if (dwRtcpDstIp >0 && wRctpDstPort>0)
// 					{
// 						LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]SingleSpy/Local:tUnlocalDst(%d-%d) SwitchVideoRtcpToDst begin,Mode:%d",
// 							tUnlocalDst.GetMcuId(),tUnlocalDst.GetMtId(),MODE_VIDEO);
// 						AddRemoveRtcpToDst(dwRtcpDstIp,wRctpDstPort,tSrc,bySrcChnnl,MODE_VIDEO,bySwitchMode,TRUE);
// 					}
// 					else
// 					{
// 						LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]SingleSpy:tUnlocalDst(%d-%d) GetSndMediaCtrlChannel failed!Mode:%d\n",
// 							tUnlocalDst.GetMcuId(),tUnlocalDst.GetMtId(),byMediaMode);
// 						return;
// 					}
// 				}
// 				else
// 				{
// 					LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]SingleSpy:tUnlocalDst(%d-%d) GetMtLogicChnnl failed!\n",
// 						tUnlocalDst.GetMcuId(),tUnlocalDst.GetMtId());
// 					return;
// 				}
// 			}
// 			
// 			if (MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode)
// 			{
// 				if ( m_ptMtTable->GetMtLogicChnnl(tLocalDst.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE) )
// 				{
// 					dwRtcpDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
// 					wRctpDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
// 					if (dwRtcpDstIp >0 && wRctpDstPort>0)
// 					{
// 						LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]SingleSpy/Local:tUnlocalDst(%d-%d) SwitchVideoRtcpToDst begin,Mode:%d",
// 							tUnlocalDst.GetMcuId(),tUnlocalDst.GetMtId(),MODE_AUDIO);
// 						AddRemoveRtcpToDst(dwRtcpDstIp,wRctpDstPort,tSrc,bySrcChnnl,MODE_AUDIO,bySwitchMode,TRUE);
// 					}
// 					else
// 					{	LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]SingleSpy:tUnlocalDst(%d-%d) GetSndMediaCtrlChannel failed!,Mode:%d\n",
// 							tUnlocalDst.GetMcuId(),tUnlocalDst.GetMtId(),byMediaMode);
// 						return;
// 					}
// 				}
// 				else
// 				{
// 					LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]SingleSpy:tDst(%d-%d) GetMtLogicChnnl failed!\n",
// 						tUnlocalDst.GetMcuId(),tUnlocalDst.GetMtId());
// 					return;
// 				}				
// 			}
// 			
// 			if (MODE_SECVIDEO == byMediaMode)
// 			{
// 				if ( m_ptMtTable->GetMtLogicChnnl(tLocalDst.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE) )
// 				{
// 					dwRtcpDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
// 					wRctpDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
// 					if (dwRtcpDstIp >0 && wRctpDstPort>0)
// 					{
// 						LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]SingleSpy/Local:tUnlocalDst(%d-%d) SwitchVideoRtcpToDst begin,Mode:%d",
// 							tUnlocalDst.GetMcuId(),tUnlocalDst.GetMtId(),MODE_SECVIDEO);
// 						AddRemoveRtcpToDst(dwRtcpDstIp,wRctpDstPort,tSrc,bySrcChnnl,MODE_SECVIDEO,bySwitchMode,TRUE);
// 					}
// 					else
// 					{
// 						LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]SingleSpy:tUnlocalDst(%d-%d) GetSndMediaCtrlChannel failed!Mode:%d\n",
// 							tUnlocalDst.GetMcuId(),tUnlocalDst.GetMtId(),MODE_SECVIDEO);
// 						return;
// 					}
// 				}
// 				else
// 				{
// 					LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]SingleSpy:tUnlocalDst(%d-%d) GetMtLogicChnnl failed!\n",
// 						tUnlocalDst.GetMcuId(),tUnlocalDst.GetMtId());
// 					return;
// 				}
// 			}
// 		}
// 		//多回传情况，tUnlocalDst来自下级,建立本级终端到回传通道的RTCP交换
// 		else
// 		{	
// 			LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[BuidRtcpSwitchSrcToDst]MultiSpy:tUnlocalDst is come from smcu",tUnlocalDst.GetMcuId(),tUnlocalDst.GetMtId());
// 			SwitchLocalMultipleSpyRtcp(tSrc,bySrcChnnl,tUnlocalDst,byMediaMode,TRUE);		
// 		}		
// 		return;
// 	}
}



/*====================================================================
    函数名      ：ProcMcuMtFastUpdateTimer
    功能        ：处理向终端请求关键帧定时器
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/10/20    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::ProcMcuMtFastUpdateTimer( const CMessage * pcMsg )
{
	u8 byMtId  = (u8)(*((u32*)pcMsg->content)/100); 
	u8 byCount = (u8)(*((u32*)pcMsg->content)%100); 
	u32 dwCurTick = OspTickGet();
	CServMsg cServMsg;
	
	STATECHECK;	

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMcuMtFastUpdateTimer] Mcu Request Mt(%d %d) send FastUpdate Count(%d)\n", 
			m_ptMtTable->GetMcuId(byMtId), byMtId, byCount);

	u8 byMode = MODE_VIDEO;
	cServMsg.SetMsgBody(&byMode, sizeof(u8));
	SendMsgToMt( byMtId, MCU_MT_FASTUPDATEPIC_CMD, cServMsg );
	m_ptMtTable->SetLastTick( byMtId, dwCurTick );
	
	byCount--;
	
    if (MT_MANU_TAIDE == m_ptMtTable->GetManuId(byMtId))
    {
//  [4/10/2012 chendaiwei] 按原有逻辑，泰德终端此处始终不会进到，故注释掉
//         if( byCount < 1)
//         {
//             SetTimer( pcMsg->event, 3600, 100*byMtId + byCount);
//             ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMcuMtFastUpdateTimer] mt.%d fastupdate has been adjust to once more due to Tandberg!\n", byMtId);
//         }
    }
    else
    {
	    if( byCount > 0 )
	    {
		    SetTimer( pcMsg->event, 1200, 100*byMtId + byCount);
	    }
    }
}

/*====================================================================
    函数名      ：ProcMcuEqpFastUpdateTimer
    功能        ：处理向外设请求关键帧定时器
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/07/11    4.6         胡昌威         创建
====================================================================*/
void CMcuVcInst::ProcMcuEqpFastUpdateTimer(const CMessage * pcMsg)
{
    u32 dwParam = *((u32*)pcMsg->content);
    u8 byEqpId = (u8)(dwParam >> 16);
    u8 byChnl  = (u16)dwParam >> 8;
    u8 byCount = (u8)dwParam;

    byEqpId = byEqpId;

    STATECHECK;	

    ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[ProcMcuEqpFastUpdateTimer] the Eqp.%u, chn.%u, byCount.%d!\n", byEqpId, byChnl, byCount);

    CServMsg cServMsg;
    u16 wEvent;
    u32 dwCurTick = OspTickGet();
    u32 dwLaskTick = 0;

    TEqp tDstEqp = g_cMcuVcApp.GetEqp(byEqpId);
    
    switch (tDstEqp.GetEqpType())
    {
    case EQP_TYPE_VMP:
        {
            cServMsg.SetChnIndex(byChnl);
            wEvent = MCU_VMP_FASTUPDATEPIC_CMD;
            m_dwVmpLastVCUTick = dwCurTick;
            SendMsgToEqp(byEqpId, wEvent, cServMsg); 
            return;
        }
        
    case EQP_TYPE_VMPTW:
		wEvent = MCU_VMPTW_FASTUPDATEPIC_CMD;
        //dwLaskTick = m_dwVmpTwLastVCUTick;
        break;
        
    case EQP_TYPE_BAS:
		{
			cServMsg.SetChnIndex(byChnl);
			wEvent = MCU_BAS_FASTUPDATEPIC_CMD;
			SetBasVcuTick(tDstEqp, byChnl, dwCurTick);
		}

		break;
    default:
        return;
    }

    //过滤1s间隙
    if ( dwCurTick - dwLaskTick > 1 * OspClkRateGet() )
    {
        SendMsgToEqp(tDstEqp.GetEqpId(), wEvent, cServMsg);  
    }
    
    if( byCount < 3 )
    {
        byCount++;
        dwParam = (dwParam & 0xffffff00) | byCount;
        SetTimer( pcMsg->event, 1200, dwParam );
    }

    return;
}

/*====================================================================
    函数名      ：ProcMcuMtSecVideoFastUpdateTimer
    功能        ：处理向终端请求关键帧定时器
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/10/20    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::ProcMcuMtSecVideoFastUpdateTimer( const CMessage * pcMsg )
{
	u8 byMtId  = (u8)(*((u32*)pcMsg->content)/100); 
	u8 byCount = (u8)(*((u32*)pcMsg->content)%100);
	u32 dwCurTick = OspTickGet();
	CServMsg cServMsg;

	STATECHECK;	

	u8 byMode = MODE_SECVIDEO;
	cServMsg.SetMsgBody(&byMode, sizeof(u8));
	SendMsgToMt( byMtId, MCU_MT_FASTUPDATEPIC_CMD, cServMsg );
	m_ptMtTable->SetLastTick( byMtId, dwCurTick, TRUE );
	
	if( byCount < 3 )
	{
		SetTimer( pcMsg->event, 1200, 100*byMtId + byCount + 1  );
	}
}

/*=============================================================================
    函 数 名： StopSpyMtCascaseSwitch
    功    能： 停止上传交换（由本级建到上级mcu的交换）
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： BOOL32
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2010/03/02  4.5		    周晶晶                创建
	20110702    4.6         彭杰                  逻辑调整、注释修改
=============================================================================*/
BOOL32 CMcuVcInst::StopSpyMtCascaseSwitch( u8 byMode /*= MODE_BOTH*/ )
{
	if( m_tCascadeMMCU.IsNull() )
	{
		return FALSE;
	}

	u16 wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
	TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo( wMMcuIdx/*m_tCascadeMMCU.GetMtId()*/ );
	if(ptConfMcInfo == NULL)
	{		
		return FALSE;
	}

	TMt tSpyMt = ptConfMcInfo->m_tSpyMt;
	if( MODE_BOTH == byMode )
	{
  		ptConfMcInfo->m_tSpyMt.SetNull();
	}

	CServMsg cServMsg;
    cServMsg.SetConfId(m_tConf.GetConfId());

	TMt tSrc;
	tSrc.SetNull();

	TMt tLocalMt;

	TMtStatus tMtStatus;

	if( MODE_BOTH == byMode || MODE_VIDEO ==  byMode )
	{
		m_ptMtTable->GetMtSrc( m_tCascadeMMCU.GetMtId(),&tSrc,MODE_VIDEO );
		tLocalMt = GetLocalMtFromOtherMcuMt( tSrc );
		//if( tSrc == tSpyMt )
		{
			if (IsNeedSelAdpt(tSrc, m_tCascadeMMCU, MODE_VIDEO))
			{
				StopSelAdapt(tSrc, m_tCascadeMMCU, MODE_VIDEO);
			}
			else
			{
				//zjl 20110510 StopSwitchToSubMt 接口重载替换
				//StopSwitchToSubMt( m_tCascadeMMCU.GetMtId(), MODE_VIDEO, SWITCH_MODE_SELECT );
				StopSwitchToSubMt(1, &m_tCascadeMMCU, MODE_VIDEO );

				//[2011/08/18/zhangli]拆RTCP交换
				//不能用BuildRtcpSwitchForSrcToDst，如果是下级终端，这里回传源已经被free了，GetRemoteRtcpAddr是失败
				TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
				if (tConfAttrb.IsResendLosePack())
				{
					//BuildRtcpSwitchForSrcToDst(m_tCascadeMMCU, tSrc, MODE_VIDEO, 0, 1, FALSE);

					u32 dwOldDstIp = 0;
					u16 wOldDstPort = 0;
					m_ptMtTable->GetMtRtcpDstAddr(m_tCascadeMMCU.GetMtId(), dwOldDstIp, wOldDstPort, MODE_VIDEO);
					if (dwOldDstIp != 0)
					{
						AddRemoveRtcpToDst(dwOldDstIp, wOldDstPort, m_tCascadeMMCU, 0, MODE_VIDEO, TRUE, SPY_CHANNL_NULL, FALSE);
					}
				}
			}		
			tSrc.SetNull();
			m_ptMtTable->SetMtSrc( m_tCascadeMMCU.GetMtId(),&tSrc,MODE_VIDEO );	
			m_ptMtTable->GetMtStatus( m_tCascadeMMCU.GetMtId(),&tMtStatus );
			tMtStatus.SetSelectMt( tSrc,MODE_VIDEO );
			m_ptMtTable->SetMtStatus( m_tCascadeMMCU.GetMtId(),&tMtStatus );

		}
		/*else
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[StopSpyMtCascaseAdp] m_tCascadeMMCU's Video SrcMt(%d.%d) is not equal SpyMt(%d.%d).so can't stop spymt video spy switch\n",
				tSrc.GetMcuId(),
				tSrc.GetMtId(),
				tSpyMt.GetMcuId(),
				tSpyMt.GetMtId()
				);
		}*/

	}

	if( MODE_BOTH == byMode || MODE_AUDIO ==  byMode )
	{
		m_ptMtTable->GetMtSrc( m_tCascadeMMCU.GetMtId(),&tSrc,MODE_AUDIO );
		tLocalMt = GetLocalMtFromOtherMcuMt( tSrc );
		CSendSpy cSendSpy;
		//if( tSrc == tSpyMt )	
		{			
			if (IsNeedSelAdpt(tSrc, m_tCascadeMMCU, MODE_AUDIO) &&
				( !m_cLocalSpyMana.GetSpyChannlInfo(tSrc,cSendSpy) || cSendSpy.GetSpyMode() == MODE_VIDEO )
				)
			{
				StopSelAdapt(tSrc, m_tCascadeMMCU, MODE_AUDIO);
			}
			else
			{
				//zjl 20110510 StopSwitchToSubMt 接口重载替换
				//StopSwitchToSubMt( m_tCascadeMMCU.GetMtId(), MODE_AUDIO, SWITCH_MODE_SELECT );
				StopSwitchToSubMt( 1, &m_tCascadeMMCU, MODE_AUDIO );
				TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
				if (tConfAttrb.IsResendLosePack())
				{	
					u32 dwOldDstIp = 0;
					u16 wOldDstPort = 0;
					m_ptMtTable->GetMtRtcpDstAddr(m_tCascadeMMCU.GetMtId(), dwOldDstIp, wOldDstPort, MODE_AUDIO);
					if (dwOldDstIp != 0)
					{
						AddRemoveRtcpToDst(dwOldDstIp, wOldDstPort, m_tCascadeMMCU, 0, MODE_AUDIO, TRUE, SPY_CHANNL_NULL, FALSE);
					}
				}
			}
			tSrc.SetNull();

			m_ptMtTable->SetMtSrc( m_tCascadeMMCU.GetMtId(), &tSrc,MODE_AUDIO );
			m_ptMtTable->GetMtStatus( m_tCascadeMMCU.GetMtId(),&tMtStatus );
			tMtStatus.SetSelectMt( tSrc,MODE_AUDIO );
			m_ptMtTable->SetMtStatus( m_tCascadeMMCU.GetMtId(),&tMtStatus );


		}
		/*else
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[StopSpyMtCascaseAdp] m_tCascadeMMCU's Audio SrcMt(%d.%d) is not equal SpyMt(%d.%d).so can't stop spymt audio spy switch\n",
				tSrc.GetMcuId(),
				tSrc.GetMtId(),
				tSpyMt.GetMcuId(),
				tSpyMt.GetMtId()
				);
		}*/

	}

	if( !m_tCascadeMMCU.IsNull() && MODE_BOTH == byMode )
	{
		CServMsg cMsg;
		TMsgHeadMsg tHeadMsg;
		TSetOutParam tOutParam;
		memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );
		tOutParam.m_nMtCount = 1;
		//tOutParam.m_atConfViewOutInfo[0].m_tMt = m_tCascadeMMCU;
		//tOutParam.m_atConfViewOutInfo[0].m_tMt.SetMcuId( LOCAL_MCUID );
		tOutParam.m_atConfViewOutInfo[0].m_tMt.SetMcuId( m_tCascadeMMCU.GetMtId() );
		tOutParam.m_atConfViewOutInfo[0].m_tMt.SetMtId( 0 );
		tOutParam.m_atConfViewOutInfo[0].m_nOutViewID = ptConfMcInfo->m_dwSpyViewId;
		tOutParam.m_atConfViewOutInfo[0].m_nOutVideoSchemeID = ptConfMcInfo->m_dwSpyVideoId;		
		cMsg.SetMsgBody((u8 *)&tOutParam, sizeof(tOutParam));
		cMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
		cMsg.SetEventId(MCU_MCU_SETOUT_NOTIF);
		SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MCU_SETOUT_NOTIF, cMsg);

		//[2011/10/08/zhangli] Bug00064460混音状态下，停过适配上传终端时，未向mcs发送更新状态对应
		MtStatusChange(&m_tCascadeMMCU, TRUE);
	}

	return TRUE;
}

// END OF FILE
