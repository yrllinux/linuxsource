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
#include "evmcu.h"
#include "evmcutest.h"
#include "mcuvc.h"
#include "mcsssn.h"
#include "mcuerrcode.h"
#include "mpmanager.h"
#include "mcuutility.h"

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
BOOL32 CMcuVcInst::StartSwitchToSubMt(TMt   tSrc, 
                                      u8    bySrcChnnl,
                                      u8    byDstMtId,
                                      u8    byMode,
                                      u8    bySwitchMode,
                                      BOOL32 bMsgStatus,
                                      BOOL32 bSndFlowCtrl, BOOL32 bIsSrcBrd)
{
    TMtStatus tMtStatus;
    CServMsg  cServMsg;
    u8 byMediaMode = byMode;
    BOOL32 bSwitchedAudio = FALSE;

    tSrc = GetLocalMtFromOtherMcuMt(tSrc);

	//检测非KDC终端是否处于哑音状态，是则不交换音频码流	
	if( TYPE_MT == tSrc.GetType() && 
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
	if( MT_MANU_KDC != m_ptMtTable->GetManuId( byDstMtId ) && 
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

    BOOL32 bMMcuNeedAdp = FALSE;
	
	//只向上级Mcu发spyMt
	TMt tDstMt = m_ptMtTable->GetMt( byDstMtId );
	if(m_tCascadeMMCU.GetMtId() != 0 && byDstMtId == m_tCascadeMMCU.GetMtId())
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tDstMt.GetMtId());
		if( ptConfMcInfo != NULL && 
			( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
		{
			if( TYPE_MT == tSrc.GetType() &&
                !(ptConfMcInfo->m_tSpyMt.GetMtId() == tSrc.GetMtId()&&
				  ptConfMcInfo->m_tSpyMt.GetMcuId() == tSrc.GetMcuId()))
			{
                return FALSE;
			}

            //判断spymt到上级是否需要适配
            TSimCapSet tSrcSimCap = m_ptMtTable->GetSrcSCS(ptConfMcInfo->m_tSpyMt.GetMtId());
            TSimCapSet tDstSimCap = m_ptMtTable->GetDstSCS(byDstMtId);
            
            if (tSrcSimCap.IsNull())
            {
                ConfLog(FALSE, "[StartSwitchToSubMt] tSrc.%d primary logic chan unexist!\n", ptConfMcInfo->m_tSpyMt.GetMtId());
                return FALSE;
            }
            tSrcSimCap.SetVideoMaxBitRate(m_ptMtTable->GetMtSndBitrate(ptConfMcInfo->m_tSpyMt.GetMtId()));
            
            if (tDstSimCap.IsNull())
            {
                ConfLog(FALSE, "[StartSwitchToSubMt] tDst.%d back logic chan unexist!\n", byDstMtId);
                return FALSE;
            }
            tDstSimCap.SetVideoMaxBitRate(m_ptMtTable->GetMtReqBitrate(byDstMtId));
            
            if (tDstSimCap < tSrcSimCap)
            {
                bMMcuNeedAdp = TRUE;
            }
		}
		g_cMpManager.SetSwitchBridge(tSrc, 0, byMediaMode);
	}
    
	//只收不发 
	TMt TNullMt;
	TNullMt.SetNull();
	if( byDstMtId == 0 )
	{
        g_cMpManager.StartSwitchToSubMt( tSrc, bySrcChnnl, TNullMt, byMediaMode, bySwitchMode );
		return TRUE;
	}

	//只发送终端, 则不用进行交换
	m_ptMtTable->GetMtStatus(byDstMtId, &tMtStatus);
	if( ( (!tMtStatus.IsReceiveVideo()) && byMediaMode == MODE_VIDEO ) ||
		( (!tMtStatus.IsReceiveAudio()) && byMediaMode == MODE_AUDIO ) )
	{
		return FALSE;
	}

    //目的终端是否在环回
    if (tMtStatus.IsMediaLoop(MODE_VIDEO))
    {
        if (byMediaMode == MODE_VIDEO)
        {
            return TRUE;
        }
        else if (byMediaMode == MODE_BOTH)
        {
            byMediaMode = MODE_AUDIO;
        }
    }
    
    if (tMtStatus.IsMediaLoop(MODE_AUDIO))
    {
        if (byMediaMode == MODE_AUDIO)
        {
            return TRUE;
        }
        else if (byMediaMode == MODE_BOTH)
        {
            byMediaMode = MODE_VIDEO;
        }
    }

	//对于已借用第一路视频通道传输双流的终端，切换第一路视频源将被拒绝
	if( TRUE == JudgeIfSwitchFirstLChannel(tSrc, bySrcChnnl, tDstMt) )
	{	
		if( byMediaMode == MODE_VIDEO )
		{
			return TRUE;
		}
		else if ( byMediaMode == MODE_BOTH )
		{
			byMediaMode = MODE_AUDIO;
		}
	}
	
	//音频不交换给自己, 如果是源与目标均为mcu，仍应进行交换
	if( (tSrc.GetMtId() == byDstMtId) && 
		(tSrc.GetType() == TYPE_MT) && 
		(tSrc.GetMtType() == MT_TYPE_MT) )
	{
		if( byMediaMode == MODE_AUDIO )
		{
			StopSwitchToSubMt( byDstMtId, byMediaMode, SWITCH_MODE_BROADCAST, bMsgStatus );
			return TRUE;
		}
		else if ( byMediaMode == MODE_BOTH )
		{
			StopSwitchToSubMt( byDstMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, bMsgStatus );
			byMediaMode = MODE_VIDEO;
		}
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
		}
	}
	
	//不要把自己的码流交换回去, 如果是源与目标均为mcu，仍应进行交换
	if( (tSrc.GetType() == TYPE_MT) &&
		(tSrc.GetMtType() == TYPE_MT) && 
		(tSrc.GetMtId() == byDstMtId) && 
		(m_ptMtTable->GetManuId( byDstMtId ) == MT_MANU_KDC) )
	{
		if( byMediaMode == MODE_BOTH || byMediaMode == MODE_VIDEO )
		{
			NotifyMtReceive( tSrc, tDstMt.GetMtId() );
		}
		
		StopSwitchToSubMt( byDstMtId, byMediaMode, SWITCH_MODE_BROADCAST, bMsgStatus );

		// guzh [8/31/2006] 无需再设置终端状态，Stop的时候会设置
		// m_ptMtTable->SetMtSrc( byDstMtId, ( const TMt * )&tSrc, byMediaMode );			
        
		return TRUE;
	}
    
    //for h320 mcu cascade select
    if ( m_ptMtTable->GetMtType(byDstMtId) == MT_TYPE_SMCU && tSrc.GetMtId() == byDstMtId &&
         !m_tLastSpyMt.IsNull() && 
         m_tConfAllMtInfo.MtJoinedConf(m_tLastSpyMt.GetMcuId(), m_tLastSpyMt.GetMtId()) )
    {
        tSrc = m_tLastSpyMt;
    }

    // zbq [08/31/2007] 本函数只处理音频和视频，和双流毫无关系
    /*
	//是否要转发H.239数据，对于双流，并不进行发言人限制，以后发后生效为准
	TLogicalChannel tLogicalChannel;
	if( tSrc.GetType() == TYPE_MT && 
		m_ptMtTable->GetMtLogicChnnl( tSrc.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, FALSE ) )
	{
		//g_cMpManager.StartSwitchToSubMt( tSrc, 1, tDstMt, MODE_VIDEO, SWITCH_MODE_BROADCAST, TRUE );
        g_cMpManager.StartSwitchToSubMt( tSrc, 0, tDstMt, MODE_SECVIDEO, SWITCH_MODE_BROADCAST, TRUE );
		//新的双流接收端加入，不再强制补发关键帧，是否需要关键帧由终端自行申请
	}*/

    //modify bas 2
	//终端是否需适配
    TMt tSwitchSrc;
    u8  bySwitchSrcChnnl;
    tSwitchSrc.SetNull();

	if (!m_tConf.GetConfAttrb().IsUseAdapter() || SWITCH_MODE_SELECT == bySwitchMode)
	{
        tSwitchSrc = tSrc;
        bySwitchSrcChnnl = bySrcChnnl;
	}
	else
	{
        if (MODE_AUDIO != byMediaMode && m_tConf.m_tStatus.IsBrdstVMP())
        {
            TSimCapSet tSimCapSet; 
            u8 byVmpSrcChl = 0;
            u8 byMediaType = m_tConf.GetMainVideoMediaType();
            u16 wDstMtReqBitrate = m_ptMtTable->GetMtReqBitrate(byDstMtId);

            TCapSupport tCap;
			/*
            //先处理vmp双速单格式的情况
            if (0 != m_tConf.GetSecBitRate() && 
                MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType())
            {
                //发第2路
                if(wDstMtReqBitrate < m_tConf.GetBitRate())
                {
                    byVmpSrcChl = 1;
                    if (wDstMtReqBitrate < m_wBasBitrate)
                    {
                        ChangeVmpBitRate(wDstMtReqBitrate, 2);
                    }
                }
                //发第1路
                else
                {
                    byVmpSrcChl = 0;
                    if (wDstMtReqBitrate < m_wVidBasBitrate)
                    {
                        ChangeVmpBitRate(wDstMtReqBitrate, 1);
                    }
                }

                //FIXME: 未处理高清VMP双速
            }
            //单速双格式或双速双格式
            else
            {
                tSimCapSet = m_ptMtTable->GetDstSCS(byDstMtId);
                if (tSimCapSet.GetVideoMediaType() == byMediaType)
                {
                    byVmpSrcChl = 0;
                    if (wDstMtReqBitrate < m_wVidBasBitrate)
                    {
                        ChangeVmpBitRate(wDstMtReqBitrate, 1);
                    }
                }
                else
                {
                    byVmpSrcChl = 1;
                    if (wDstMtReqBitrate < m_wBasBitrate)
                    {
                        ChangeVmpBitRate(wDstMtReqBitrate, 2);
                    }
                }
            }            
			bySwitchSrcChnnl = byVmpSrcChl;*/
			// xliang [8/4/2009] 调VMP广播码率,此处调用只有老VMP广播会进去
			bySwitchSrcChnnl = AdjustVmpBrdBitRate(&tDstMt);
            tSwitchSrc = m_tVmpEqp;
            
        }
        else
        {
            TSimCapSet tTmpSCS;
            TSimCapSet tSrcSCS;
            TSimCapSet tDstSCS;
            u16 wAdaptBitRate;

            if (MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode)
            {
                if (tSrc.GetType() == TYPE_MT &&
                    IsMtNeedAdapt(ADAPT_TYPE_AUD, byDstMtId, &tSrc))
                {
                    if (!m_tConf.m_tStatus.IsAudAdapting())
                    {
                        tTmpSCS = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
                        tSrcSCS.SetAudioMediaType(tTmpSCS.GetAudioMediaType());
                        tTmpSCS = m_ptMtTable->GetDstSCS(byDstMtId);
                        tDstSCS.SetAudioMediaType(tTmpSCS.GetAudioMediaType());
                        if (!StartAdapt(ADAPT_TYPE_AUD, 0, &tDstSCS, &tSrcSCS))
                        {
                            MpManagerLog("StartSwitchToSubMt %d failed because audio Adapter cann't available!\n", byDstMtId);
                            return FALSE;
                        }
                    }                    

                    MpManagerLog("IsMtNeedAdapt(ADAPT_TYPE_AUD, %d)\n", byDstMtId);
                    tSwitchSrc = m_tAudBasEqp;
                    bySwitchSrcChnnl = m_byAudBasChnnl;
                }
                else
                {
                    tSwitchSrc = tSrc;
                    bySwitchSrcChnnl = bySrcChnnl;
                }

                if (MODE_BOTH == byMediaMode)
                {
                    MpManagerLog("IsMtNeedAdapt(ADAPT_TYPE_AUD, %d, MODE_BOTH)\n", byDstMtId);
                    g_cMpManager.StartSwitchToSubMt(tSwitchSrc, bySwitchSrcChnnl, tDstMt, MODE_AUDIO, bySwitchMode);
                    bSwitchedAudio = TRUE;
                }
            }

            if (MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode)
            {
                // 判断高清适配, zgc, 2008-08-08
                TMt tSrcTemp;
                tSrcTemp.SetNull();
                TSimCapSet tSrcSCSTemp;
                if ( IsHDConf( m_tConf ) )
                {
                    if (m_cMtRcvGrp.IsMtNeedAdp(byDstMtId) ||
                        bMMcuNeedAdp)
                    {
						tSwitchSrc = tSrc;
						bySwitchSrcChnnl = bySrcChnnl;
                    }
                    else
                    {
                        tSwitchSrc = m_tVidBrdSrc;
                        bySwitchSrcChnnl = m_tVidBrdSrc == m_tPlayEqp ? m_byPlayChnnl : 0;
                    }
                }
                else
                if (IsMtNeedAdapt(ADAPT_TYPE_VID, byDstMtId, &tSrc))
                {
                    if (!m_tConf.m_tStatus.IsVidAdapting())
                    {
                        if (IsNeedVidAdapt(tDstSCS, tSrcSCS, wAdaptBitRate, &tSrc))
                        {
                            //需要考虑是否可能StartAdapt()多次
                            if (!StartAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSCS, &tSrcSCS))
                            {
                                MpManagerLog("StartSwitchToSubMt %d failed because video Adapter cann't available!\n", byDstMtId);
                                return FALSE;
                            }
                        }
                    }
                    MpManagerLog("IsMtNeedAdapt(ADAPT_TYPE_VID, %d)\n", byDstMtId);
                    tSwitchSrc = m_tVidBasEqp;
                    bySwitchSrcChnnl = m_byVidBasChnnl;
                }
                else if (IsMtNeedAdapt(ADAPT_TYPE_BR, byDstMtId, &tSrc))
                {
                    if (!m_tConf.m_tStatus.IsBrAdapting())
                    {
                        if (IsNeedBrAdapt(tDstSCS, tSrcSCS, wAdaptBitRate, &tSrc))
                        {
                            if (!StartAdapt(ADAPT_TYPE_BR, wAdaptBitRate, &tDstSCS, &tSrcSCS))
                            {
                                MpManagerLog("StartSwitchToSubMt %d failed because bitrate Adapter cann't available!\n", byDstMtId);
                                return FALSE;
                            }
                        }
                    }
                    MpManagerLog("IsMtNeedAdapt(ADAPT_TYPE_BR, %d)\n", byDstMtId);
                    tSwitchSrc = m_tBrBasEqp;
                    bySwitchSrcChnnl = m_byBrBasChnnl;
                }
                else
                {
                    tSwitchSrc = tSrc;
                    bySwitchSrcChnnl = bySrcChnnl;
                }
            }
        }        
    }

    if (!tSwitchSrc.IsNull())
    {
        u8 byTmpMediaMode = byMediaMode;
        if (MODE_BOTH == byMediaMode && bSwitchedAudio) //避免音频重复交换
        {
            byTmpMediaMode = MODE_VIDEO;
        }
        
        //zbq [08/07/2007] BAS建向终端的交换,基于平滑考虑,不再作StartStop操作.
        BOOL32 bStopBeforeStart = TRUE;
        if ( (tSwitchSrc.GetType()    == TYPE_MCUPERI &&
              tSwitchSrc.GetEqpType() == EQP_TYPE_BAS ) ||
             (tSwitchSrc.GetType()    == TYPE_MT &&
              tSwitchSrc.GetEqpType() == MT_TYPE_SMCU) )
        {
            bStopBeforeStart = FALSE;
        }

		if (!g_cMpManager.StartSwitchToSubMt(tSwitchSrc, bySwitchSrcChnnl, tDstMt, byTmpMediaMode, bySwitchMode, FALSE, bStopBeforeStart))
		{
			MpManagerLog( "StartSwitchToSubMt() failed! Cannot switch to specified sub mt!\n" );
			return FALSE;
		}

        if (m_tConf.GetConfAttrb().IsResendLosePack())
        {
            //适配时的丢包重传(在建立bas－>dstmt的rtp时，建立dstmt－>prs的rtcp)
            if (EQP_TYPE_BAS == tSwitchSrc.GetEqpType())
            {                     
                if (tSwitchSrc == m_tVidBasEqp && (MODE_BOTH == byMediaMode || MODE_VIDEO == byMediaMode))
                {
                    AddRemovePrsMember(tDstMt, TRUE, PRSCHANMODE_VIDBAS);
                }            
                else if (tSwitchSrc == m_tBrBasEqp && (MODE_BOTH == byMediaMode || MODE_VIDEO == byMediaMode))
                {               
                    AddRemovePrsMember(tDstMt, TRUE, PRSCHANMODE_BRBAS);
                } 
                
                if (tSwitchSrc == m_tAudBasEqp && (MODE_BOTH == byMediaMode || MODE_AUDIO == byMediaMode))
                {     
                    AddRemovePrsMember(tDstMt, TRUE, PRSCHANMODE_AUDBAS);
                }

                // FIXME：高清适配丢包重传未添加,zgc
            }
            
            //vmp.2
//             if (0 != m_tConf.GetSecBitRate() && 
//                 MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType() &&
//                 m_tVidBrdSrc == m_tVmpEqp)
//             {
//                 if (tSwitchSrc == m_tVmpEqp && 1 == bySwitchSrcChnnl &&
//                     (MODE_BOTH == byMediaMode || MODE_VIDEO == byMediaMode))
//                 {
//                     AddRemovePrsMember(tDstMt, TRUE, PRSCHANMODE_VMP2);
//                 }
//             }
			
			if( m_tVidBrdSrc == m_tVmpEqp )
			{
				TPeriEqpStatus tPeriEqpStatus; 
				g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;
				if( byVmpSubType != VMP )
				{
					// xliang [4/28/2009] FIXME 根据通道bySwitchSrcChnnl建prs交换
					 u8 abyPrsModVmp[4] = {PRSCHANMODE_VMPOUT1, PRSCHANMODE_VMPOUT2, PRSCHANMODE_VMPOUT3, PRSCHANMODE_VMPOUT4};
					 AddRemovePrsMember(tDstMt, TRUE, abyPrsModVmp[bySwitchSrcChnnl]);

					// xliang [5/7/2009] temporarily test version. The final version is above
					/*
					 if(bySwitchSrcChnnl == 0) //0出
					{
						if( EQP_CHANNO_INVALID != m_byPrsChnnlVmpOut2 )
						{
							AddRemovePrsMember(tDstMt, TRUE, PRSCHANMODE_VMPOUT2);
						}
						else
						{
							AddRemovePrsMember(tDstMt, TRUE, PRSCHANMODE_VMPOUT1);
						}
					}
					else if(bySwitchSrcChnnl == 2)
					{
						AddRemovePrsMember(tDstMt, TRUE, PRSCHANMODE_VMPOUT4);
					}
					// end temporary version
					*/

				}
				
			}
        }        
    }

	//通知终端开始发送 
	if( tSrc.GetType() == TYPE_MT )
	{
        if (bSndFlowCtrl)
        {
            NotifyMtSend( tSrc.GetMtId(), byMediaMode, TRUE );
        }		

        // zbq [06/25/2007] 建音频交换不请求关键帧
        if ( MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode )
        {
            NotifyFastUpdate(tSrc, MODE_VIDEO, TRUE);
        }
	}
    else
    {
        //zbq[05/07/2009] 级联回传 尝试主动 发三个关键帧
        if ((MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode) &&
            tSrc.GetEqpId() == m_tCasdVidBasEqp.GetEqpId() &&
            byDstMtId == m_tCascadeMMCU.GetMtId())
        {
            NotifyFastUpdate(tSrc, 0);
            NotifyFastUpdate(tSrc, 1);
        }
    }
	
	// xliang [6/16/2009] FIXME: 向MPU请求关键帧？
	
	//设置终端的码流提示标识
	TPeriEqpStatus tEqpStatus;
	if( tSrc == m_tPlayEqp )
	{
		g_cMcuVcApp.GetPeriEqpStatus( m_tPlayEqp.GetEqpId(), &tEqpStatus );
		cServMsg.SetMsgBody( (u8*)tEqpStatus.GetAlias(), MAXLEN_EQP_ALIAS );
		SendMsgToMt( byDstMtId, MCU_MT_MTSOURCENAME_NOTIF, cServMsg );		
	}
	//如果是VMP,则清空码流提示标识
	if( tSrc == m_tVmpEqp )
	{
		u8 abyVmpAlias[8];
		abyVmpAlias[0] = ' ';
		abyVmpAlias[1] = '\0';
		cServMsg.SetMsgBody( abyVmpAlias, sizeof(abyVmpAlias) );
		SendMsgToMt( byDstMtId, MCU_MT_MTSOURCENAME_NOTIF, cServMsg );		
	}

    
    //zbq[01/01/2009] 高清bas交换策略调整：作源单独建交换，单独通知接收
    if (tSrc == m_tAudBasEqp || tSrc == m_tVidBasEqp || tSrc == m_tBrBasEqp /*|| 
             ( tSrc.GetType() == TYPE_MCUPERI && 
             tSrc.GetEqpType() == EQP_TYPE_BAS &&
             g_cMcuAgent.IsEqpBasHD( tSrc.GetEqpId() ) ) */)
	{
		return TRUE;
	}

	//通知终端开始接收
	if( byMediaMode == MODE_BOTH || byMediaMode == MODE_VIDEO )
	{
		NotifyMtReceive( tSrc, tDstMt.GetMtId() );
	}

    if (tSrc.GetType() == TYPE_MCUPERI && 
        tSrc.GetEqpType() == EQP_TYPE_BAS &&
        g_cMcuAgent.IsEqpBasHD(tSrc.GetEqpId()))
    {
		m_ptMtTable->SetMtSrc(byDstMtId, &m_tVidBrdSrc, byMediaMode);
		MtStatusChange(byDstMtId, TRUE);
        return TRUE;
    }
		
	// xsl [5/22/2006]设置终端状态, 这里保存广播源信息，不保存bas信息，因为其他地方会根据源信息做相应处理
	m_ptMtTable->SetMtSrc( byDstMtId, ( const TMt * )&tSrc, byMediaMode );

	//过滤目标终端为上级mcu的情况
	m_ptMtTable->GetMtStatus( byDstMtId, &tMtStatus );
	if( tSrc.GetType() == TYPE_MT && tSrc.GetMtId() != byDstMtId && 
		(m_tCascadeMMCU.GetMtId() == 0 || byDstMtId != m_tCascadeMMCU.GetMtId()) )
	{
		TMtStatus tSrcMtStatus;
		u8 byAddSelByMcsMode = MODE_NONE;
		m_ptMtTable->GetMtStatus( tSrc.GetMtId(), &tSrcMtStatus );

        //zbq[12/06/2007] VMP点名时序问题
        if( ((!(tSrc == m_tVidBrdSrc)) || (tSrc == m_tVidBrdSrc && 
                                           ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() &&
                                           m_tRollCaller == m_tVidBrdSrc)) &&
                                           tSrcMtStatus.IsSendVideo() && 
            ( MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode ) )
		{
			byAddSelByMcsMode = MODE_VIDEO;
		}
		if( !(tSrc == m_tAudBrdSrc) && tSrcMtStatus.IsSendAudio() && 
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
		}
	}
	else
	{
		tMtStatus.RemoveSelByMcsMode( byMediaMode );
	}
	m_ptMtTable->SetMtStatus( byDstMtId, &tMtStatus );
	
	if( bMsgStatus )
	{
        MtStatusChange( byDstMtId, TRUE );
	}

	//若是选看，按目的终端的需要调节发送终端的码虑
	if( tSrc.GetType() == TYPE_MT && bySwitchMode == SWITCH_MODE_SELECT && 
		( byMediaMode == MODE_VIDEO || byMediaMode == MODE_BOTH ) && 
	    tSrc.GetMtId() != m_tVidBrdSrc.GetMtId() )
	{
		u16 wMtBitRate = m_ptMtTable->GetMtReqBitrate( byDstMtId );
		AdjustMtVideoSrcBR( byDstMtId, wMtBitRate );
	}

	return TRUE;
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
void CMcuVcInst::StopSwitchToSubMt( u8 byDstMtId, u8 byMode,u8 bySwitchMode,BOOL32 bMsgStatus, BOOL32 bStopSelByMcs /* = TRUE*/, BOOL32 bSelSelf  )
{
	CServMsg	cServMsg;
	TMt    TMtNull;
	TMtStatus   tMtStatus;

	TMtNull.SetNull();
	TMt tDstMt = m_ptMtTable->GetMt( byDstMtId );
    m_ptMtTable->GetMtStatus( byDstMtId, &tMtStatus );

    //目的终端是否在环回
    if (tMtStatus.IsMediaLoop(MODE_VIDEO))
    {
        if (byMode == MODE_VIDEO)
        {
            return;
        }
        else if (byMode == MODE_BOTH)
        {
            byMode = MODE_AUDIO;
        }
    }
    
    if (tMtStatus.IsMediaLoop(MODE_AUDIO))
    {
        if (byMode == MODE_AUDIO)
        {
            return;
        }
        else if (byMode == MODE_BOTH)
        {
            byMode = MODE_VIDEO;
        }
    }

    if ( bSelSelf &&
          // 为图像平滑，vcs在单方模式下主席终端不进行看自己的图像切换，直到下一个调度终端上线
		 !(VCS_CONF == m_tConf.GetConfSource() && 
 		   byDstMtId == m_tConf.GetChairman().GetMtId() && 
 		   (VCS_SINGLE_MODE == m_cVCSConfStatus.GetCurVCMode() &&
		    !m_cVCSConfStatus.GetCurVCMT().IsNull()) ||
		   (ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
		    m_cVCSConfStatus.GetChairPollState() != VCS_POLL_STOP)))
    {
		// xliang [4/15/2009] FIXME：主席从选看某个MT切换到选看VMP，就会先看自己再看VMP 。
	    //通知终端停止接收，看自己 
	    if( byMode == MODE_BOTH || byMode == MODE_VIDEO )
	    {
			if( m_tConfAllMtInfo.MtJoinedConf( byDstMtId ) )
		    {                    
                NotifyMtReceive( tDstMt, byDstMtId );
		    }		
	    }
    }
	
    // zbq [06/20/2007] 为保证点名人平滑切看被点名人，此处不停上一个被点名
    // 人的音视频交换，依赖于新的点名人建交换的时候的StartStopSwitch()停之。
    // zbq [09/11/2007] FIXME: 此处未特殊处理点名人逻辑，导致的点名人看多次切换的还面问题 后续考虑解决
    // if ( !(!m_tRollCaller.IsNull() && tDstMt == m_tRollCaller) )
    {
        // stop switch
        g_cMpManager.StopSwitchToSubMt( tDstMt, byMode );
    }
		
	if( bStopSelByMcs )
	{
		tMtStatus.RemoveSelByMcsMode( byMode );
	}

	m_ptMtTable->SetMtStatus( byDstMtId, &tMtStatus );	
	
    // guzh [8/31/2006] 要从上面移动到下面，否则SetMtStatus后MtSrc会被冲掉
	// 设置终端视频状态为看自己，而音频是 TMTNull
    if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
    {
        m_ptMtTable->SetMtSrc( byDstMtId, &TMtNull, MODE_AUDIO );    
    }
    else if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
    {
        m_ptMtTable->SetMtSrc( byDstMtId, &tDstMt, MODE_VIDEO );    
    }	

	if( bMsgStatus )
	{
        MtStatusChange( byDstMtId, TRUE );
	}
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
    BOOL32 bSwitchedAudio = FALSE;

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
	
	//只向上级Mcu发spyMt
	TMt tDstMt = m_ptMtTable->GetMt(byDstMtId);
	if(m_tCascadeMMCU.GetMtId() != 0 && byDstMtId == m_tCascadeMMCU.GetMtId())
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tDstMt.GetMtId());
		if( ptConfMcInfo != NULL && 
			( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
		{
			if(!(ptConfMcInfo->m_tSpyMt.GetMtId() == tSrc.GetMtId()&&
				ptConfMcInfo->m_tSpyMt.GetMcuId() == tSrc.GetMcuId()))
			{
				return FALSE;				
			}
		}
		g_cMpManager.SetSwitchBridge(tSrc, 0, byMediaMode);
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
		}
	}

    // zbq [06/29/2007] 回传通道的视频交换建前不停，依赖于DS替换建立
    BOOL32 bMediaDstMMcu = FALSE;
        
    // libo [11/24/2005]
    TSimCapSet tTmpSCS;
    TSimCapSet tSrcSCS;
    TSimCapSet tDstSCS;
    u16 wAdaptBitRate = 0;
    TMt tSwitchSrc;
    u8  bySwitchSrcChnnl;
    BOOL32 bAudBasCap = TRUE;
    BOOL32 bVidBasCap = TRUE;
    if (0 == m_tCascadeMMCU.GetMtId() || byDstMtId != m_tCascadeMMCU.GetMtId())
    {

        if (MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode)
        {
            if (g_cMcuVcApp.IsPeriEqpConnected(m_tAudBasEqp.GetEqpId()) &&
                IsMtNeedAdapt(ADAPT_TYPE_AUD, byDstMtId, &tSrc))
            {
                if (!m_tConf.m_tStatus.IsAudAdapting())
                {
                    tTmpSCS = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
                    tSrcSCS.SetAudioMediaType(tTmpSCS.GetAudioMediaType());
                    tTmpSCS = m_ptMtTable->GetDstSCS(byDstMtId);
                    tDstSCS.SetAudioMediaType(tTmpSCS.GetAudioMediaType());
                    if (!StartAdapt(ADAPT_TYPE_AUD, 0, &tDstSCS, &tSrcSCS))
                    {
                        ConfLog(FALSE, "StartSwitchToSubMt %d failed because audio Adapter cann't available!\n", byDstMtId);
                        return FALSE;
                    }
                }
                Mt2Log("IsMtNeedAdapt(ADAPT_TYPE_AUD, %d)\n", byDstMtId);
                tSwitchSrc = m_tAudBasEqp;
                bySwitchSrcChnnl = m_byAudBasChnnl;
            }
            else
            {
                tSwitchSrc = tSrc;
                bySwitchSrcChnnl = bySrcChnnl;
            }

            if (MODE_BOTH == byMediaMode)
            {
                Mt2Log("IsMtNeedAdapt(ADAPT_TYPE_AUD, %d, MODE_BOTH)\n", byDstMtId);
                g_cMpManager.StartSwitchToSubMt(tSwitchSrc, bySwitchSrcChnnl, tDstMt, MODE_AUDIO, bySwitchMode);
                bSwitchedAudio = TRUE;
            }
        }

        if (MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode)
        {
            if (g_cMcuVcApp.IsPeriEqpConnected(m_tVidBasEqp.GetEqpId()) &&
                IsMtNeedAdapt(ADAPT_TYPE_VID, byDstMtId, &tSrc))
            {
                if (!m_tConf.m_tStatus.IsVidAdapting())
                {
                    if (IsNeedVidAdapt(tDstSCS, tSrcSCS, wAdaptBitRate, &tSrc))
                    {
                        //需要考虑是否可能StartAdapt()多次
                        if (!StartAdapt(ADAPT_TYPE_VID, wAdaptBitRate, &tDstSCS, &tSrcSCS))
                        {
                            ConfLog(FALSE, "StartSwitchToSubMt %d failed because video Adapter cann't available!\n", byDstMtId);
                            return FALSE;
                        }
                    }
                }
                Mt2Log("IsMtNeedAdapt(ADAPT_TYPE_VID, %d)\n", byDstMtId);
                tSwitchSrc = m_tVidBasEqp;
                bySwitchSrcChnnl = m_byVidBasChnnl;
            }
            else if (g_cMcuVcApp.IsPeriEqpConnected(m_tBrBasEqp.GetEqpId()) &&
                     IsMtNeedAdapt(ADAPT_TYPE_BR, byDstMtId, &tSrc))
            {
                if (!m_tConf.m_tStatus.IsBrAdapting())
                {
                    if (IsNeedBrAdapt(tDstSCS, tSrcSCS, wAdaptBitRate, &tSrc))
                    {
                        if (!StartAdapt(ADAPT_TYPE_BR, wAdaptBitRate, &tDstSCS, &tSrcSCS))
                        {
                            ConfLog(FALSE, "StartSwitchToSubMt %d failed because bitrate Adapter cann't available!\n", byDstMtId);
                            return FALSE;
                        }
                    }
                }
                Mt2Log("IsMtNeedAdapt(ADAPT_TYPE_BR, %d)\n", byDstMtId);
                tSwitchSrc = m_tBrBasEqp;
                bySwitchSrcChnnl = m_byBrBasChnnl;
            }
            else
            {
                tSwitchSrc = tSrc;
                bySwitchSrcChnnl = bySrcChnnl;
            }
        }
    }
    else //!if (0 == m_tCascadeMMCU.GetMtId() || byDstMtId != m_tCascadeMMCU.GetMtId())
    {
        u8     byEqpId;
        u8     byChnIdx;

        if (MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode)
        {
            if (IsMtNeedAdapt(ADAPT_TYPE_CASDAUD, byDstMtId, &tSrc))
            {
                if (EQP_CHANNO_INVALID == m_byCasdAudBasChnnl || m_tCasdAudBasEqp.IsNull())
                {
                    if (g_cMcuVcApp.GetIdleBasChl(ADAPT_TYPE_AUD, byEqpId, byChnIdx))
                    {
                        m_tCasdAudBasEqp.SetMcuEqp(LOCAL_MCUID, byEqpId, EQP_TYPE_BAS);
                        m_tCasdAudBasEqp.SetConfIdx(m_byConfIdx);
                        m_byCasdAudBasChnnl = byChnIdx;
                        EqpLog("m_byCasdAudBasChnnl = %d\n", m_byCasdAudBasChnnl);
                    }
                    else
                    {
                        bAudBasCap = FALSE;
                        ConfLog(FALSE, "no idle cascade audio adapte channel!\n");
                    }
                }

                if (FALSE == bAudBasCap)
                {
                    tSwitchSrc = tSrc;
                    bySwitchSrcChnnl = bySrcChnnl;
                }
                else
                {
                    if (!m_tConf.m_tStatus.IsCasdAudAdapting())
                    {
                        tSrcSCS.Clear();
                        tDstSCS.Clear();
                        tTmpSCS = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
                        tSrcSCS.SetAudioMediaType(tTmpSCS.GetAudioMediaType());
                        tTmpSCS = m_ptMtTable->GetDstSCS(byDstMtId);
                        tDstSCS.SetAudioMediaType(tTmpSCS.GetAudioMediaType());
                        if (!StartAdapt(ADAPT_TYPE_CASDAUD, 0, &tDstSCS, &tSrcSCS))
                        {
                            ConfLog(FALSE, "StartSwitchToSubMt %d failed because video Adapter cann't available!\n", byDstMtId);
                            bAudBasCap = FALSE;
                        }
                    }
                    if (FALSE == bAudBasCap)
                    {
                        tSwitchSrc = tSrc;
                        bySwitchSrcChnnl = bySrcChnnl;
                    }
                    else
                    {
                        tSwitchSrc = m_tCasdAudBasEqp;
                        bySwitchSrcChnnl = m_byCasdAudBasChnnl;
                    }
                }

            }
            else
            {
                tSwitchSrc = tSrc;
                bySwitchSrcChnnl = bySrcChnnl;
            }

            if (MODE_BOTH == byMediaMode)
            {
                Mt2Log("IsMtNeedAdapt(ADAPT_TYPE_AUD, %d, MODE_BOTH)\n", byDstMtId);
                g_cMpManager.StartSwitchToSubMt(tSwitchSrc, bySwitchSrcChnnl, tDstMt, MODE_AUDIO, bySwitchMode);
                bSwitchedAudio = TRUE;
            }
        }

        if ((tSwitchSrc == m_tCasdAudBasEqp) && (bySwitchSrcChnnl == m_byCasdAudBasChnnl))
        {
            if (!g_cMpManager.StartSwitchToPeriEqp(tSrc, bySrcChnnl, m_tCasdAudBasEqp.GetEqpId(), m_byCasdAudBasChnnl, MODE_AUDIO))
            {
                ConfLog( FALSE, "[StartSwitchToMcu]StartSwitchToPeriEqp() failed! - m_tCasdAudBasEqp\n" );
                return FALSE;
            }
        }

        BOOL32 bHDCasNeedAdp = FALSE;
        if (MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode)
        {
            //标清会议动态占用
            if (!IsHDConf(m_tConf))
            {
                BOOL32 bCasdVid = IsMtNeedAdapt(ADAPT_TYPE_CASDVID, byDstMtId, &tSrc);
                // xsl [9/29/2006] 级联不进行码率适配，通过flowcontrol调节回传终端发送码率
                //            BOOL32 bCasdBr = IsMtNeedAdapt(ADAPT_TYPE_BR, byDstMtId, tSrc);
                if (bCasdVid/* || bCasdBr*/)
                {
                    if (EQP_CHANNO_INVALID == m_byCasdVidBasChnnl || m_tCasdVidBasEqp.IsNull())
                    {
                        if (g_cMcuVcApp.GetIdleBasChl(ADAPT_TYPE_VID, byEqpId, byChnIdx))
                        {
                            m_tCasdVidBasEqp.SetMcuEqp(LOCAL_MCUID, byEqpId, EQP_TYPE_BAS);
                            m_tCasdVidBasEqp.SetConfIdx(m_byConfIdx);
                            m_byCasdVidBasChnnl = byChnIdx;
                            EqpLog("m_byCasdVidBasChnnl = %d\n", m_byCasdVidBasChnnl);
                        }
                        else
                        {
                            bVidBasCap = FALSE;
                            ConfLog(FALSE, "no idle cascade video adapte channel!\n");
                        }
                    }
                    
                    if (FALSE == bVidBasCap)
                    {
                        tSwitchSrc = tSrc;
                        bySwitchSrcChnnl = bySrcChnnl;
                    }
                    else
                    {
                        if (!m_tConf.m_tStatus.IsCasdVidAdapting() && (bCasdVid/* || bCasdBr*/))
                        {
                            tSrcSCS.Clear();
                            tDstSCS.Clear();
                            tTmpSCS = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
                            tSrcSCS.SetVideoMediaType(tTmpSCS.GetVideoMediaType());
                            tTmpSCS = m_ptMtTable->GetDstSCS(byDstMtId);
                            tDstSCS.SetVideoMediaType(tTmpSCS.GetVideoMediaType());
                            wAdaptBitRate = m_ptMtTable->GetMtReqBitrate(byDstMtId);
                            if (wAdaptBitRate > m_ptMtTable->GetMtSndBitrate(tSrc.GetMtId()))
                            {
                                wAdaptBitRate = m_ptMtTable->GetMtSndBitrate(tSrc.GetMtId());
                            }
                            if (!StartAdapt(ADAPT_TYPE_CASDVID, wAdaptBitRate, &tDstSCS, &tSrcSCS))
                            {
                                ConfLog(FALSE, "StartSwitchToMcu %d failed because cascade video Adapter not available!\n", byDstMtId);
                                bVidBasCap = FALSE;
                            }
                        }
                        if (FALSE == bVidBasCap)
                        {
                            tSwitchSrc = tSrc;
                            bySwitchSrcChnnl = bySrcChnnl;
                        }
                        else
                        {
                            tSwitchSrc = m_tCasdVidBasEqp;
                            bySwitchSrcChnnl = m_byCasdVidBasChnnl;
                        }
                    }
                }        
                else
                {
                    tSwitchSrc = tSrc;
                    bySwitchSrcChnnl = bySrcChnnl;
                }
            }
            //zbq[03/26/2009]高清会议沿用标清会议的方式和标识 处理 
            else
            {
                if (MT_TYPE_MT != tSrc.GetMtType() &&
                    MT_TYPE_SMCU != tSrc.GetMtType())
                {
                    ConfLog(FALSE, "[StartSwitchToMcu] Mt can be spy only, ignore<%d, %d>!\n", tSrc.GetMcuId(), tSrc.GetMtId());
                    return FALSE;
                }

                TSimCapSet tSrcSimCap = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
                TSimCapSet tDstSimCap = m_ptMtTable->GetDstSCS(byDstMtId);

                if (tSrcSimCap.IsNull())
                {
                    ConfLog(FALSE, "[StartSwitchToMcu] tSrc.%d primary logic chan unexist!\n", tSrc.GetMtId());
                    return FALSE;
                }
                tSrcSimCap.SetVideoMaxBitRate(m_ptMtTable->GetMtSndBitrate(tSrc.GetMtId()));

                if (tDstSimCap.IsNull())
                {
                    ConfLog(FALSE, "[StartSwitchToMcu] tDst.%d back logic chan unexist!\n", byDstMtId);
                    return FALSE;
                }
                tDstSimCap.SetVideoMaxBitRate(m_ptMtTable->GetMtReqBitrate(byDstMtId));
                
                //FIXME: 上述能力未考虑帧率，可能会漏触发帧率适配（但主流一般不帧率适配）

                bHDCasNeedAdp = tDstSimCap < tSrcSimCap;

                if (bHDCasNeedAdp)
                {
                    //未级联适配，先查找；已经开启过帧率适配，调整其解码和编码参数；
                    if (m_tCasdVidBasEqp.IsNull())
                    {
                        //动态抢占空闲的MAU, 没有可用则不回传；一旦占用，直到级联结束或会议结束释放；
                        u8 byEqpId = 0;
                        u8 byChnId = 0;
                        if (!g_cMcuVcApp.GetIdleHDBasVidChl(byEqpId, byChnId))
                        {
                            ConfLog(FALSE, "[StartSwitchToMcu] switch failed due to no Idle Chn!\n");
                            return FALSE;
                        }
                        m_tCasdVidBasEqp.SetMcuEqp(LOCAL_MCUID, byEqpId, EQP_TYPE_BAS);
                        m_tCasdVidBasEqp.SetConfIdx(m_byConfIdx);
                        m_byCasdVidBasChnnl = byChnId;
                    }
                    tSwitchSrc = tSrc;
                    bySwitchSrcChnnl = bySrcChnnl;
                }
                else
                {
                    tSwitchSrc = tSrc;
                    bySwitchSrcChnnl = bySrcChnnl;
                }
            }
        }

        if (IsHDConf(m_tConf) && bHDCasNeedAdp)
        {
            if (!StartHDCascadeAdp(&tSwitchSrc))
            {
                ConfLog(FALSE, "[StartSwitchToMcu] StartHDCascadeAdp() failed!\n" );
                return FALSE;
            }
        }
        else
        {
            if ((tSwitchSrc == m_tCasdVidBasEqp) && (bySwitchSrcChnnl == m_byCasdVidBasChnnl))
            {
                if (!g_cMpManager.StartSwitchToPeriEqp(tSrc, bySrcChnnl, m_tCasdVidBasEqp.GetEqpId(), m_byCasdVidBasChnnl, MODE_VIDEO))
                {
                    ConfLog( FALSE, "[StartSwitchToMcu]StartSwitchToPeriEqp() failed! - m_tCasdVidBasEqp\n" );
                    return FALSE;
                }
            }
        }
    }

    if (!tSwitchSrc.IsNull())
    {
        u8 byTmpMediaMode = byMediaMode;
        if (MODE_BOTH == byMediaMode && bSwitchedAudio) //避免音频重复交换
        {
            byTmpMediaMode = MODE_VIDEO;
        }
        
        BOOL32 bStopBeforeStart = bMediaDstMMcu ? FALSE : TRUE;

        if (!g_cMpManager.StartSwitchToSubMt(tSwitchSrc, bySwitchSrcChnnl, tDstMt, byTmpMediaMode, bySwitchMode, FALSE, bStopBeforeStart))
        {
            ConfLog( FALSE, "StartSwitchToSubMt() failed! Cannot switch to specified sub mt!\n" );
            return FALSE;
        }
    }
    // libo [11/24/2005]end

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

    //modify bas 2
	//if( tSrc == m_tBrBasEqp || tSrc == m_tRtpBasEqp )
    if (tSrc == m_tAudBasEqp || tSrc == m_tVidBasEqp || tSrc == m_tBrBasEqp)
	{
		return TRUE;
	}

	//通知终端开始接收
	if( byMediaMode == MODE_BOTH || byMediaMode == MODE_VIDEO )
	{
		NotifyMtReceive( tSrc, tDstMt.GetMtId() );
	}

	//设置终端状态
	m_ptMtTable->SetMtSrc( byDstMtId, ( const TMt * )&tSrc, byMediaMode );

	//过滤目标终端为上级mcu的情况
	m_ptMtTable->GetMtStatus( byDstMtId, &tMtStatus );
	if( tSrc.GetType() == TYPE_MT && tSrc.GetMtId() != byDstMtId && 
		(m_tCascadeMMCU.GetMtId() == 0 || byDstMtId != m_tCascadeMMCU.GetMtId()) )
	{
		TMtStatus tSrcMtStatus;
		u8 byAddSelByMcsMode = MODE_NONE;
		m_ptMtTable->GetMtStatus( tSrc.GetMtId(), &tSrcMtStatus );

		if( !(tSrc == m_tVidBrdSrc) && tSrcMtStatus.IsSendVideo() && 
			( MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode ) )
		{
			byAddSelByMcsMode = MODE_VIDEO;
		}
		if( !(tSrc == m_tAudBrdSrc) && tSrcMtStatus.IsSendAudio() && 
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
		}
	}
	else
	{
		tMtStatus.RemoveSelByMcsMode( byMediaMode );
	}
	m_ptMtTable->SetMtStatus( byDstMtId, &tMtStatus );
	
	if( bMsgStatus )
	{
        MtStatusChange(byDstMtId, TRUE);
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
        TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tDst.GetMtId());
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
    TSimCapSet tTmpSrcSCS;
    if ( IsHDConf( m_tConf ) )
    {
        if (m_cMtRcvGrp.IsMtNeedAdp(byMtId))
        {
            return FALSE;
        }
    }
    else
    //是否需要适配才能交换
    if ( IsMtNeedAdapt( ADAPT_TYPE_BR, byMtId, &tTmpSrc  )  ||
         IsMtNeedAdapt( ADAPT_TYPE_VID, byMtId, &tTmpSrc ) )
    {
        return FALSE;
    }

	// 增加对双格式VMP的判断, zgc, 20070604
	if( tStatus.IsReceiveVideo() && m_tConf.m_tStatus.IsBrdstVMP() )
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
BOOL32 CMcuVcInst::StartSwitchFromBrd(const TMt &tSrc,
                                      u8        bySrcChnnl,
                                      u8        byDstMtNum,
                                      const TMt* const ptDstBase)
{
    u8 byLoop = 0;

    // zgc, 2008-05-28, 清选看
    TMtStatus tMtStatus;
    u8 bySelMode = MODE_NONE;
    for ( byLoop = 0; byLoop < byDstMtNum; byLoop ++ )
    {
        // zgc, 2008-06-02, 点名人和被点名人不从广播交换，保留选看
        if ( m_tVidBrdSrc == m_tVmpEqp && 
            ( ptDstBase[byLoop] == m_tRollCaller ||
            ptDstBase[byLoop] == GetLocalMtFromOtherMcuMt(m_tRollCallee)) )
        {
            continue;
        }

        m_ptMtTable->GetMtStatus(ptDstBase[byLoop].GetMtId(), &tMtStatus);
        bySelMode = tMtStatus.GetSelByMcsDragMode();
        if ( MODE_VIDEO == bySelMode || MODE_BOTH == bySelMode )
        {	
			u8 byIsRestore = FALSE;
			if(!m_tVidBrdSrc.IsNull() && tSrc == m_tVidBrdSrc)
			{
				byIsRestore = TRUE;
			}
            StopSelectSrc(ptDstBase[byLoop], MODE_VIDEO, byIsRestore);
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
            if ( !( m_tVidBrdSrc == m_tVmpEqp && 
                    ( ptDstBase[0] == m_tRollCaller ||
                      ptDstBase[0] == GetLocalMtFromOtherMcuMt(m_tRollCallee)) ) )
            {
				
				if (!m_tCascadeMMCU.IsNull()  &&
					m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId())->m_tSpyMt == tSrc &&
					m_tCascadeMMCU == ptDstBase[0] )
				{
					// xliang [3/2/2009] 若该src正好又在回传通道里，则不拆交换
					MpManagerLog("[StartSwitchFromBrd] Since the source is also under back-to-MMCU switch, we shouldn't stop switch in this case.\n" );
				}
				else
				{
					// 解决BUG10501, 从上面移到这里,保证停--建交换对应, zgc, 2008-04-22
					StopSwitchToSubMt( ptDstBase[0].GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE, FALSE, FALSE );
				}
				
                bRet = g_cMpManager.StartSwitchFromBrd(tSrc, bySrcChnnl, byDstMtNum, ptDstBase);
            }
            else
            {
                MpManagerLog("[StartSwitchFromBrd] Mt.%d is in Rollcall, no accept from brd(DstNum=1)\n", ptDstBase[0].GetMtId() );
                bRet = TRUE;                
            }
		}
		else
		{
			// 不完善
            if (!(m_cMtRcvGrp.IsMtNeedAdp(ptDstBase[0].GetMtId()) &&
				 StartSwitchToSubMtFromAdp(ptDstBase[0].GetMtId())))
			{
				bRet = StartSwitchToSubMt( tSrc, bySrcChnnl, ptDstBase[0].GetMtId(), MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE, FALSE );
			}
			return bRet;
		}
	}
    else if( byDstMtNum > 1 )
    {
        //zbq[03/29/2008] 整理接收的广播目标，过滤点名人和被点名人收VMP的情况
        if ( m_tVidBrdSrc == m_tVmpEqp &&
             ROLLCALL_MODE_NONE != m_tConf.m_tStatus.GetRollCallMode() )
        {
            for( byLoop = 0; byLoop < byDstMtNum; byLoop ++ )
            {
                if ( ptDstBase[byLoop] == m_tRollCaller ||
                     ptDstBase[byLoop] == GetLocalMtFromOtherMcuMt(m_tRollCallee))
                {
                    continue;
                }
                else
                {
                    bRet &= g_cMpManager.StartSwitchFromBrd(tSrc, bySrcChnnl, 1, &ptDstBase[byLoop]);
                }
            }
        }
        else
        {
            bRet = g_cMpManager.StartSwitchFromBrd(tSrc, bySrcChnnl, byDstMtNum, ptDstBase);
        }
    }
	else
	{
		return FALSE;
	}

    //zbq[04/15/2009] Tand的关键帧编的比较快，且间隔为3s. 故交换建立后再发youareseeing，尽量拿到第一个关键帧
    if (IsDelayVidBrdVCU() && tSrc == m_tVidBrdSrc)
    {
    }
    else
    {
        //guzh [2008/03/31] 请求关键帧
        if (TYPE_MT == tSrc.GetType())
        {
            NotifyFastUpdate(tSrc, MODE_VIDEO, TRUE);
        }
    }

    CServMsg cServMsg;
    TPeriEqpStatus tEqpStatus;
    //TMtStatus tMtStatus;
    for (byLoop = 0; byLoop < byDstMtNum; byLoop ++)
    {
        // zbq [06/19/2007] 点名人和被点名人不从广播交换
        if ( !( m_tVidBrdSrc == m_tVmpEqp && 
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

            m_ptMtTable->SetMtSrc( ptDstBase[byLoop].GetMtId(), &tSrc, MODE_VIDEO );
        }
        else
        {
            // do nothing
            MpManagerLog("[StartSwitchFromBrd] Mt.%d is in Rollcall, no accept from brd\n", ptDstBase[byLoop].GetMtId() );
        }
    }
    
    MtStatusChange();
    
    return bRet;

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
BOOL32 CMcuVcInst::StartSwitchToAllSubMtJoinedConf(const TMt &tSrc, u8 bySrcChnnl, const TMt &tOldSrc, BOOL32 bForce)
{
    // 添加广播源
    if ( TYPE_MT == tSrc.GetType() )
    {
        if ( !m_ptMtTable->IsLogicChnnlOpen(tSrc.GetMtId(), LOGCHL_VIDEO, FALSE) )
        {
            MpManagerLog("[StartSwitchToAllSubMtJoinedConf]Source MT.%d is not sending video!\n",tSrc.GetMtId());
            return FALSE;
        }
    }
    g_cMpManager.StartSwitchToBrd(tSrc, bySrcChnnl, bForce);
    
    TMtStatus tStatus;

    TMt atMtList[MAXNUM_CONF_MT];
    u8 byDstMtNum = 0;

    u8 byLoop = 1;
    TMt tLoopMt;
    for ( ; byLoop <= MAXNUM_CONF_MT; byLoop++ )
    {
        if (!m_tConfAllMtInfo.MtJoinedConf(byLoop))
        {
            continue;
        }

		// 对于VCS会议,主席轮询时,主席不看广播源
		if (VCS_CONF == m_tConf.GetConfSource() &&
			m_tConf.GetChairman().GetMtId() == byLoop &&
			VCS_POLL_START == m_cVCSConfStatus.GetChairPollState())
		{
			continue;
		}

        tLoopMt = m_ptMtTable->GetMt(byLoop);
        if ( !CanMtRecvVideo( tLoopMt, tSrc ) )
        {
            // guzh [5/11/2007] 不能通过广播交换的，走普通交换（可能自动起适配）
			// 发言人不走普通交换逻辑, zgc, 2008-04-12
			if ( !(tLoopMt == GetLocalMtFromOtherMcuMt(m_tConf.GetSpeaker())) )
			{
                //zbq[01/01/2009] 高清适配策略调整：从适配接收码流单独建立，不再自动起适配
				if (!(IsHDConf(m_tConf) && m_cMtRcvGrp.IsMtNeedAdp(tLoopMt.GetMtId())))
                {
                    StartSwitchToSubMt( tSrc, bySrcChnnl, byLoop, MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE, FALSE );
                }
			}
            continue;
        }
        
        atMtList[byDstMtNum] = tLoopMt;
        byDstMtNum ++;
    }

    //建立交换
    
    //高清部分
    if ( byDstMtNum > 0)
    {
        StartSwitchFromBrd(tSrc, bySrcChnnl, byDstMtNum, atMtList);
    }
    
    //发言人自身的码流交换处理：根据会议属性进行相应回传设置
	ChangeSpeakerSrc( MODE_VIDEO, emReasonChangeBrdSrc );
    
    MtStatusChange();
    
	return TRUE;
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
BOOL32 CMcuVcInst::StartSwitchToAllSubMtJoinedConf(const TMt &tSrc, u8 bySrcChnnl)
{
	u8	byLoop;
   
    BOOL32 bResult[MAXNUM_CONF_MT];
    BOOL32 bResultAnd = TRUE;
    for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
    {
        if( m_tConfAllMtInfo.MtJoinedConf( byLoop ))
        {
			// 调整发言人源
			if ( byLoop == GetLocalSpeaker().GetMtId() && GetLocalSpeaker().GetType() == TYPE_MT)
			{
				//新发言人码流源调整逻辑, zgc, 2008-04-12
				ChangeSpeakerSrc( MODE_AUDIO, emReasonChangeBrdSrc );
			}
            else
            {
            	//对于需要视频适配的终端，音频同样需要进行缓冲处理，以保证唇音同步
                TLogicalChannel tVidChn;
                BOOL32 bNeedAdp = IsHDConf(m_tConf) &&
                                  m_cMtRcvGrp.IsMtNeedAdp(byLoop) &&
                                  m_ptMtTable->GetMtLogicChnnl(byLoop, LOGCHL_VIDEO, &tVidChn, TRUE) &&
                                  !IsMtNeedAdapt(ADAPT_TYPE_AUD, byLoop);
                if (bNeedAdp)
                {
                    continue;
                }
                bResult[byLoop-1] = StartSwitchToSubMt(tSrc,
                                                       bySrcChnnl,
                                                       byLoop,
                                                       MODE_AUDIO,
                                                       SWITCH_MODE_BROADCAST,
                                                       FALSE, FALSE, FALSE);
            }
            
            bResultAnd = bResultAnd && bResult[byLoop-1];
        }
    }
    
    MtStatusChange();
    
	return bResultAnd;
}


/*====================================================================
    函数名      ：StartSwitchToAllSubMtJoinedConfNeedAdapt
    功能        ：将指定终端数据交换到会议中所有与会直连需适配终端
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt & tSrc, 源
				  u8 bySrcChnnl, 源的信道号
				  u8 byMode, 交换模式，缺省为MODE_BOTH
    返回值说明  ：TRUE/FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/27    1.0         LI Yi         创建
	03/07/23    2.0         胡昌威        修改
	04/03/22    3.0         胡昌威        修改
====================================================================*/
//modify bas 2
BOOL32 CMcuVcInst::StartSwitchToAllSubMtJoinedConfNeedAdapt(TMt tSrc, u8 bySrcChnnl, u8 byMode, u8 byAdaptType)
{
    u8 byLoop;
    TConfMtInfo tConfMtInfo;

    tSrc = GetLocalMtFromOtherMcuMt(tSrc);

    if (!m_tConf.m_tStatus.IsVidAdapting() &&
        !m_tConf.m_tStatus.IsBrAdapting() &&
        !m_tConf.m_tStatus.IsAudAdapting())
    {
        return FALSE;
    }

    BOOL32 bResult[MAXNUM_CONF_MT];
    BOOL32 bResultAnd = TRUE;
    //TMt tDstMt;
    for (byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        if (m_tConfAllMtInfo.MtJoinedConf(byLoop) && tSrc.GetMtId() != byLoop)
        {
            //tDstMt = m_ptMtTable->GetMt(byLoop);
            Mt2Log("\n\nJoined mtid.%d byAdaptType = %d\n", byLoop, byAdaptType);
            if (IsMtNeedAdapt(byAdaptType, byLoop, &tSrc))
            {
                Mt2Log("need adapt mtid.%d byAdaptType = %d\n", byLoop, byAdaptType);
                bResult[byLoop] = StartSwitchToSubMt(tSrc, bySrcChnnl, byLoop, byMode, SWITCH_MODE_BROADCAST, FALSE);
                bResultAnd = bResultAnd && bResult[byLoop-1];
            }
            Mt2Log("\n\n");
        }
    }

    MtStatusChange();

    return bResultAnd;
}


/*=============================================================================
    函 数 名： StartSwitchToRecNeedHDVidAdapt
    功    能： 
    算法实现： 
    全局变量： 
    参    数： void
    返 回 值： BOOL32 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2008/8/27   4.0		    周广程                  创建
    2008/12/11  4.5         张宝卿                  适应新策略调整
=============================================================================*/
BOOL32 CMcuVcInst::StartSwitchToRecNeedHDVidAdapt( void )
{
    if ( !IsRecordSrcHDBas() || m_tConf.m_tStatus.IsNoRecording() )
    {
        return TRUE;
    }

    if (0 == m_tConf.GetBitRate())
    {
        return FALSE;
    }

    TEqp tLBEqp;
    u8 byOutIdx = 0;
    BOOL32 bRet = m_cBasMgr.GetLowBREqp(m_tConf, tLBEqp, byOutIdx);

    if (!bRet)
    {
        ConfLog( FALSE, "[StartSwitchToRecNeedHDVidAdapt] get LB Bas failed!\n" );
        return FALSE;
    }

    StartSwitchToPeriEqp(tLBEqp, byOutIdx, m_tRecEqp.GetEqpId(), m_byRecChnnl, MODE_VIDEO);

    return TRUE;
}

/*====================================================================
    函数名      ：StartSwitchToSubMtNeedAdp
    功能        ：将数据交换到会议中 接本适配通道的所有终端
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/20    4.5         张宝卿          创建
    09/04/17    4.6         张宝卿          重写，兼容mpu
    09/07/26    4.6 				张宝卿 				  增加对音频交换的相关处理
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchToSubMtNeedAdp(const TEqp &tEqp, u8 byChnId)
{
    u8 byMediaMode = MODE_NONE;
    u8 bySwitchMode = SWITCH_MODE_NONE;

    u8 byChnMode = m_cBasMgr.GetChnMode(tEqp, byChnId);
    
    if (!CheckHdBasChnMode(byChnMode, byMediaMode, bySwitchMode))
    {
        ConfLog(FALSE, "[StartSwitchToSubMtNeedAdp] BasChn<%d, %d> unexist!\n", 
                        tEqp.GetEqpId(), byChnId);
        return FALSE;
    }

    THDBasVidChnStatus tStatus;
    memset(&tStatus, 0, sizeof(tStatus));

    if (!m_cBasMgr.GetChnStatus(tEqp, byChnId, tStatus))
    {
        ConfLog(FALSE, "[StartSwitchToSubMtNeedAdp] eqp<%d,%d> status failed\n", 
                        tEqp.GetEqpId(), byChnId);
        return FALSE;
    }

    u8 byOutIdx = 0;
    switch (byChnMode)
    {
    //1.主流广播
    case CHN_ADPMODE_MVBRD:
        {
        for(byOutIdx = 0; byOutIdx < MAXNUM_VOUTPUT; byOutIdx++)
        {
            THDAdaptParam tAdpParam = *tStatus.GetOutputVidParam(byOutIdx);
            
            if (tAdpParam.IsNull())
            {
                continue;
            }            
            
            u8 byMVType = tAdpParam.GetVidType();
            u16 wWidth  = tAdpParam.GetWidth();
            u16 wHeight = tAdpParam.GetHeight();
            u8 byRes = GetResByWH(wWidth, wHeight);
            
            u8 byNum = 0;
            u8 abyMt[MAXNUM_CONF_MT];
            m_cMtRcvGrp.GetMVMtList(byMVType, byRes, byNum, abyMt);
            
            u8 byMtIdx = 0;
            for(; byMtIdx < byNum; byMtIdx ++)
            {
                //不交换给自己
                if (m_tVidBrdSrc.GetMtType() == MT_TYPE_MT &&
                    m_tVidBrdSrc.GetMtId() == abyMt[byMtIdx])
                {
                    continue;
                }
                //不交换给上级MCU
                if (m_tCascadeMMCU.GetMtId() == abyMt[byMtIdx])
                {
                    continue;
                }

#ifndef _SATELITE_
                StartSwitchToSubMt(tEqp, 
                                   byOutIdx+byChnId*MAXNUM_VOUTPUT,
                                   abyMt[byMtIdx], MODE_VIDEO);
                                   

				//音频伴随处理
                StartSwitchAud2MtNeedAdp(abyMt[byMtIdx], bySwitchMode);

#else
				BOOL32 bVidSrcHd = ::topoGetMtInfo(LOCAL_MCUID, m_tVidBrdSrc.GetMtId(), g_atMtTopo, g_wMtTopoNum).IsMtHd();
				if (bVidSrcHd)
				{
					g_cMpManager.StartSatConfCast(tEqp, CAST_SEC, MODE_VIDEO, byChnId*MAXNUM_VOUTPUT+byOutIdx);
				}
				else
				{
					g_cMpManager.StartSatConfCast(tEqp, CAST_FST, MODE_VIDEO, byChnId*MAXNUM_VOUTPUT+byOutIdx);
				}
				m_ptMtTable->SetMtSrc(abyMt[byMtIdx], &tEqp, MODE_VIDEO);
#endif

            }

            //zbq[01/05/2009] 临时处理：反向适配资源不足时的紧凑对齐
            TSimCapSet tSrcSim;
            memset(&tSrcSim, 0, sizeof(tSrcSim));
            GetMVBrdSrcSim(tSrcSim);


            if (MEDIA_TYPE_H264 != tSrcSim.GetVideoMediaType() &&
                MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType()
                //取消单速限制
                //FIXME: 对高分辨率低码率的终端的带宽调控未作处理
                /*0 == m_tConf.GetSecBitRate() &&*/)
            {
                u8 byAdjRes = m_tConf.GetMainVideoFormat();
                u8 byAdjType = m_tConf.GetMainVideoMediaType();
                
                u8 byProximalType = 0;
                u8 byProximalRes = 0;
                BOOL32 bRet = GetProximalGrp(byAdjType, 
                                             byAdjRes,
                                             byProximalType, byProximalRes);
                if (!bRet)
                {
                    ConfLog(FALSE, "[StartSwitchToSubMtNeedAdp] no proximal grp for<%d,%d\n", byAdjType, byAdjRes);
                    continue;
                }
                if (byMVType == byProximalType && byRes == byProximalRes)
                {
                    byNum = 0;
                    memset(&abyMt, 0, sizeof(abyMt));
                    m_cMtRcvGrp.GetMVMtList(byAdjType, byAdjRes, byNum, abyMt);
                    
                    for(byMtIdx = 0; byMtIdx < byNum; byMtIdx ++)
                    {
                        if (m_tVidBrdSrc.GetMtType() == MT_TYPE_MT &&
                            m_tVidBrdSrc.GetMtId() == abyMt[byMtIdx])
                        {
                            continue;
                        }

#ifndef _SATELITE_
                        StartSwitchToSubMt(tEqp, 
                                           byChnId * MAXNUM_VOUTPUT + byOutIdx, 
                                           abyMt[byMtIdx], 
                                           byMediaMode, 
                                           bySwitchMode );

						//音频伴随处理
                        StartSwitchAud2MtNeedAdp(abyMt[byMtIdx], bySwitchMode);

#else
						BOOL32 bVidSrcHd = ::topoGetMtInfo(LOCAL_MCUID, m_tVidBrdSrc.GetMtId(), g_atMtTopo, g_wMtTopoNum).IsMtHd();
						if (bVidSrcHd)
						{
							g_cMpManager.StartSatConfCast(tEqp, CAST_SEC, MODE_VIDEO, byChnId*MAXNUM_VOUTPUT+byOutIdx);
						}
						else
						{
							g_cMpManager.StartSatConfCast(tEqp, CAST_FST, MODE_VIDEO, byChnId*MAXNUM_VOUTPUT+byOutIdx);
						}
						m_ptMtTable->SetMtSrc(abyMt[byMtIdx], &tEqp, MODE_VIDEO);
#endif

                    }
                }
            }
            //桥处理
            g_cMpManager.SetSwitchBridge(tEqp, byChnId * MAXNUM_VOUTPUT + byOutIdx, MODE_VIDEO);
            g_cMpManager.SetSwitchBridge(tEqp, byChnId * MAXNUM_VOUTPUT + byOutIdx, MODE_AUDIO);
        }
        }
    	break;
    	
    //FIXME: 暂时不处理选看适配的唇音同步问题
    case CHN_ADPMODE_DSBRD:
        
        for(byOutIdx = 0; byOutIdx < MAXNUM_VOUTPUT; byOutIdx++)
        {
            THDAdaptParam tAdpParam = *tStatus.GetOutputVidParam(byOutIdx);
            if (tAdpParam.IsNull())
            {
                continue;
            }
            
            u8 byDSType = tAdpParam.GetVidType();
            u16 wWidth  = tAdpParam.GetWidth();
            u16 wHeight = tAdpParam.GetHeight();
            u8 byRes = GetResByWH(wWidth, wHeight);
            
            u8 byNum = 0;
            u8 abyMt[MAXNUM_CONF_MT];
            m_cMtRcvGrp.GetDSMtList(byDSType, byRes, byNum, abyMt);
            
            for(u8 byMtIdx = 0; byMtIdx < byNum; byMtIdx ++)
            {
                if (m_tDoubleStreamSrc.GetMtType() == MT_TYPE_MT &&
                    m_tDoubleStreamSrc.GetMtId() == abyMt[byMtIdx])
                {
                    continue;
                }
                TMt tDstMt = m_ptMtTable->GetMt(abyMt[byMtIdx]);
#ifdef _SATELITE_
				BOOL32 bVidSrcHd = ::topoGetMtInfo(LOCAL_MCUID, m_tDoubleStreamSrc.GetMtId(), g_atMtTopo, g_wMtTopoNum).IsMtHd();
				if (bVidSrcHd)
				{
					g_cMpManager.StartSatConfCast(tEqp, CAST_SEC, MODE_SECVIDEO, byChnId*MAXNUM_VOUTPUT+byOutIdx);
				}
				else
				{
					g_cMpManager.StartSatConfCast(tEqp, CAST_FST, MODE_SECVIDEO, byChnId*MAXNUM_VOUTPUT+byOutIdx);
				}
				m_ptMtTable->SetMtSrc(abyMt[byMtIdx], &tEqp, MODE_SECVIDEO);
#else
				g_cMpManager.StartSwitchToSubMt(tEqp, 
												byChnId * MAXNUM_VOUTPUT + byOutIdx,
												tDstMt, 
												byMediaMode, 
												bySwitchMode, TRUE, TRUE, TRUE);

#endif
            }
            //桥处理
            g_cMpManager.SetSwitchBridge(tEqp, byChnId * MAXNUM_VOUTPUT + byOutIdx, MODE_SECVIDEO, TRUE);
        }
        break;

    case CHN_ADPMODE_DSSEL:
        break;
    case CHN_ADPMODE_MVSEL:
        break;
    }
    
    //zbq[07/11/2009] 交换建立完成后发关键帧请求
    
    NotifyFastUpdate(tEqp, byChnId, TRUE);
    
    return TRUE;
}

/*====================================================================
    函数名      ：StopSwitchToSubMtNeedAdp
    功能        ：将数据停止交换到会议中 接本适配通道的某终端
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/27    4.5         张宝卿          创建
    09/04/17    4.6         张宝卿          重写，兼容mpu
    09/07/26    4.6         张宝卿          增加处理音频交换相关
====================================================================*/
BOOL32 CMcuVcInst::StopSwitchToSubMtNeedAdp(const TEqp &tEqp, u8 byChnId)
{
    u8 byMediaMode = MODE_NONE;
    u8 bySwitchMode = SWITCH_MODE_NONE;
    
    u8 byChnMode = m_cBasMgr.GetChnMode(tEqp, byChnId);
    
    if (!CheckHdBasChnMode(byChnMode, byMediaMode, bySwitchMode))
    {
        ConfLog(FALSE, "[StopSwitchToSubMtNeedAdp] BasChn<%d, %d> unexist!\n", 
            tEqp.GetEqpId(), byChnId);
        return FALSE;
    }

    THDBasVidChnStatus tStatus;
    memset(&tStatus, 0, sizeof(tStatus));
    
    if (!m_cBasMgr.GetChnStatus(tEqp, byChnId, tStatus))
    {
        ConfLog(FALSE, "[StopSwitchToSubMtNeedAdp] eqp<%d,%d> status failed\n", 
            tEqp.GetEqpId(), byChnId);
        return FALSE;
    }
    
    //音频适配交换的伴随
    BOOL32 bRemoveAudSW = MODE_VIDEO == byMediaMode;

    u8 byOutIdx = 0;
    switch (byChnMode)
    {
    case CHN_ADPMODE_MVBRD:

        for(byOutIdx = 0; byOutIdx < MAXNUM_VOUTPUT; byOutIdx++)
        {
            THDAdaptParam tAdpParam = *tStatus.GetOutputVidParam(byOutIdx);
            
            u8 byMVType = tAdpParam.GetVidType();
            
            u16 wWidth = tAdpParam.GetWidth();
            u16 wHeight = tAdpParam.GetHeight();
            u8 byRes = GetResByWH(wWidth, wHeight);
            
            u8 byNum = 0;
            u8 abyMt[MAXNUM_CONF_MT];
            m_cMtRcvGrp.GetMVMtList(byMVType, byRes, byNum, abyMt);
            
            u8 byMtIdx = 0;
            for(; byMtIdx < byNum; byMtIdx ++)
            {
                //保护过滤上级mcu
                if (!m_tCascadeMMCU.IsNull() && m_tCascadeMMCU.GetMtId() == abyMt[byMtIdx])
                {
                    continue;
                }
                StopSwitchToSubMt(abyMt[byMtIdx], MODE_VIDEO);
                
                if(bRemoveAudSW &&
                   !IsMtNeedAdapt(ADAPT_TYPE_AUD, abyMt[byMtIdx]))
                {
                	StopSwitchToSubMt(abyMt[byMtIdx], MODE_AUDIO);
                }
            }
            
            //zbq[01/05/2009] 临时处理：反向适配资源不足时的紧凑对齐 对称取消
            TSimCapSet tSrcSim;
            memset(&tSrcSim, 0, sizeof(tSrcSim));
            GetMVBrdSrcSim(tSrcSim);
            if (MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType() &&
                MEDIA_TYPE_H264 != tSrcSim.GetVideoMediaType() 
                //取消单速限制
                /*&&
                0 == m_tConf.GetSecBitRate()*/)
            {
                u8 byAdjType = m_tConf.GetMainVideoMediaType();
                u8 byAdjRes = m_tConf.GetMainVideoFormat();
                
                u8 byProximalType = 0;
                u8 byProximalRes = 0;
                BOOL32 bRet = GetProximalGrp(byAdjType, byAdjRes, byProximalType, byProximalRes);
                if (!bRet)
                {
                    ConfLog(FALSE, "[StopSwitchToSubMtNeedAdp] no proximal grp for <%d, %d\n", byAdjType, byAdjRes);
                    continue;
                }
                if (byMVType == byProximalType && byRes == byProximalRes)
                {
                    byNum = 0;
                    memset(&abyMt, 0, sizeof(abyMt));
                    m_cMtRcvGrp.GetMVMtList(byAdjType, byAdjRes, byNum, abyMt);
                    
                    for(byMtIdx = 0; byMtIdx < byNum; byMtIdx ++)
                    {
                        //保护过滤上级mcu
                        if (!m_tCascadeMMCU.IsNull() && m_tCascadeMMCU.GetMtId() == abyMt[byMtIdx])
                        {
                            continue;
                        }
                        StopSwitchToSubMt(abyMt[byMtIdx], byMediaMode);

                        if(bRemoveAudSW &&
                           !IsMtNeedAdapt(ADAPT_TYPE_AUD, abyMt[byMtIdx]))
                        {
                        	StopSwitchToSubMt(abyMt[byMtIdx], byMediaMode);
                        }
                    }
                }
            }
        }
        break;

    case CHN_ADPMODE_DSBRD:

        for(byOutIdx = 0; byOutIdx < MAXNUM_VOUTPUT; byOutIdx++)
        {
            THDAdaptParam tAdpParam = *tStatus.GetOutputVidParam(byOutIdx);
            if (tAdpParam.IsNull())
            {
                continue;
            }
            
            u8 byDSType = tAdpParam.GetVidType();
            u16 wWidth = tAdpParam.GetWidth();
            u16 wHeight = tAdpParam.GetHeight();
            u8 byRes = GetResByWH(wWidth, wHeight);
            
            u8 byNum = 0;
            u8 abyMt[MAXNUM_CONF_MT];
            m_cMtRcvGrp.GetDSMtList(byDSType, byRes, byNum, abyMt);
            
            for(u8 byMtIdx = 0; byMtIdx < byNum; byMtIdx ++)
            {
                TMt tDstMt = m_ptMtTable->GetMt(abyMt[byMtIdx]);
                g_cMpManager.StopSwitchToSubMt(tDstMt, byMediaMode);
            }
        }
        break;
    case CHN_ADPMODE_DSSEL:
        break;
    case CHN_ADPMODE_MVSEL:
        break;
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
    09/08/07    4.6         张宝卿          创建
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchAud2MtNeedAdp(u8 byDstMtId, u8 bySwitchMode)
{
    if (!m_cMtRcvGrp.IsMtNeedAdp(byDstMtId))
    {
        ConfLog(FALSE, "[StartSwitchAud2MtNeedAdp] Mt.%d needn't adp, aud switch lossed!\n", byDstMtId);
        return FALSE;
    }

    if (CONF_POLLMODE_VIDEO == m_tConf.m_tStatus.GetPollMode())
    {
        EqpLog("[StartSwitchAud2MtNeedAdp] Mt.%d needn't aud switch be ignored due to VID poll!\n", byDstMtId);
        return TRUE;
    }

    //本会议的音频缓冲输出点确立

    //1、h264分辨率紧凑上靠
    u8 byAudBindRes = 0xff;
    u8 byAudBindPT = MEDIA_TYPE_NULL;

    if (0 != m_tConf.GetSecBitRate())
    {
        byAudBindRes = m_tConf.GetMainVideoFormat();
    }
    else
    {
        if (m_tConf.GetConfAttrbEx().IsResEx720())
        {
            byAudBindRes = VIDEO_FORMAT_HD720;
        }
        else if (m_tConf.GetConfAttrbEx().IsResEx4Cif())
        {
            byAudBindRes = VIDEO_FORMAT_4CIF;
        }
        else if (m_tConf.GetConfAttrbEx().IsResExCif())
        {
            byAudBindRes = VIDEO_FORMAT_CIF;
        }        
    }
    byAudBindPT = m_tConf.GetMainVideoMediaType();

    //2、双格式
    if (0xff == byAudBindRes ||
        MEDIA_TYPE_NULL == byAudBindPT)
    {
        byAudBindPT = m_tConf.GetSecVideoMediaType();
        byAudBindRes = m_tConf.GetSecVideoFormat();
    }

    if (0xff == byAudBindRes &&
        MEDIA_TYPE_NULL == m_tConf.GetSecVideoMediaType())
    {
        ConfLog(FALSE, "[StartSwitchAud2MtNeedAdp] no adp chn bind for aud, impossible!\n");
        return FALSE;
    }

    //3、找到对应的Eqp、chnId、outIdx
    TEqp tBindBas;
    u8 byBindChnId = 0;
    u8 byBindOutIdx = 0;

    BOOL32 bRet = m_cBasMgr.GetBasResource(byAudBindPT,
                                           byAudBindRes,
                                           tBindBas,
                                           byBindChnId, byBindOutIdx);
    if (!bRet)
    {
        ConfLog(FALSE, "[StartSwitchAud2MtNeedAdp] adp chn<PT.%d, Res.%d> has no Bas resource!\n",
                        byAudBindPT, byAudBindRes);
        return FALSE;
    }

    //4、建立对应的音频桥和交换
    tBindBas.SetConfIdx(m_byConfIdx);

    bRet = TRUE;
    bRet &= StartSwitchToSubMt(tBindBas,
                               byBindChnId * MAXNUM_VOUTPUT,
                               byDstMtId,
                               MODE_AUDIO,
                               bySwitchMode);

    bRet &= g_cMpManager.SetSwitchBridge(tBindBas,
                                         byBindChnId * MAXNUM_VOUTPUT,
                                         MODE_AUDIO);

//暂不考虑唇音同步
/*
#ifdef _SATELITE_
	g_cMpManager.StartSatConfCast(tBindBas, MODE_AUDIO, byBindChnId * MAXNUM_VOUTPUT);
#endif
*/

    return bRet;
}

/*====================================================================
    函数名      ：StartAllHdBasSwitch
    功能        ：开启hdbas的所有交换
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/12/11    4.5         张宝卿          创建
    09/04/17    4.6         张宝卿          重写
====================================================================*/
BOOL32 CMcuVcInst::StartAllHdBasSwitch(BOOL32 bDual)
{
    u8 byNum = 0;

    if (!bDual)
    {
        TBasChn atBasChn[MAXNUM_CONF_MVCHN];
        m_cBasMgr.GetChnGrp(byNum, atBasChn, CHN_ADPMODE_MVBRD);
        
        for (u8 byIdx = 0; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
        {
            if (atBasChn[byIdx].IsNull())
            {
                continue;
            }
            atBasChn[byIdx].SetConfIdx(m_byConfIdx);
            StartHdBasSwitch(atBasChn[byIdx].GetEqp(), atBasChn->GetChnId());
        }
    }
    else
    {
        TBasChn atBasChn[MAXNUM_CONF_DSCHN];
        m_cBasMgr.GetChnGrp(byNum, atBasChn, CHN_ADPMODE_DSBRD);

        for (u8 byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
        {
            if (atBasChn[byIdx].IsNull())
            {
                continue;
            }
            atBasChn[byIdx].SetConfIdx(m_byConfIdx);
            StartHdBasSwitch(atBasChn[byIdx].GetEqp(), atBasChn->GetChnId());
        }
    }

    return TRUE;
}

/*====================================================================
函数名      ：StopAllHdBasSwitch
功能        ：停止hdbas的所有交换
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：无
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
08/12/11    4.5         张宝卿          创建
09/04/17    4.6         张宝卿          重写
====================================================================*/
BOOL32 CMcuVcInst::StopAllHdBasSwitch(BOOL32 bDual)
{
    u8 byNum = 0;

    if (!bDual)
    {
        TBasChn atBasChn[MAXNUM_CONF_MVCHN];
        m_cBasMgr.GetChnGrp(byNum, atBasChn, CHN_ADPMODE_MVBRD);
		        
        for (u8 byIdx = 0; byIdx < MAXNUM_CONF_MVCHN; byIdx++)
        {
            if (atBasChn[byIdx].IsNull())
            {
                continue;
            }
            StopHdBasSwitch(atBasChn[byIdx].GetEqp(), atBasChn[byIdx].GetChnId());
        }
    }
    else
    {
        TBasChn atBasChn[MAXNUM_CONF_DSCHN];
        m_cBasMgr.GetChnGrp(byNum, atBasChn, CHN_ADPMODE_DSBRD);

        for (u8 byIdx = 0; byIdx < MAXNUM_CONF_DSCHN; byIdx++)
        {
            if (atBasChn[byIdx].IsNull())
            {
                continue;
            }
            StopHdBasSwitch(atBasChn[byIdx].GetEqp(), atBasChn[byIdx].GetChnId());
        }
    }

    return FALSE;
}

/*====================================================================
    函数名      ：StartHdBasSwitch
    功能        ：开启某hdbas的所有交换
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/12/11    4.5         张宝卿         创建
====================================================================*/
BOOL32 CMcuVcInst::StartHdBasSwitch(const TEqp &tEqp, u8 byChnId)
{
    u8 byMediaMode = MODE_NONE;
    u8 bySwitchMode = SWITCH_MODE_NONE;
    u8 byChnMode = m_cBasMgr.GetChnMode(tEqp, byChnId);
    
    if (!CheckHdBasChnMode(byChnMode, byMediaMode, bySwitchMode))
    {
        ConfLog(FALSE, "[StartHdBasSwitch] BasChn<%d, %d> unexist!\n", 
                        tEqp.GetEqpId(), byChnId);
        return FALSE;
    }

    TMt tSrc;
    tSrc.SetNull();
    
    //主流视频适配交换建立时，同时建立上行的音频“适配”交换
    BOOL32 bAudSWAdd = byMediaMode == MODE_VIDEO;
    bAudSWAdd &= CONF_POLLMODE_VIDEO != m_tConf.m_tStatus.GetPollMode();

    //bas
    u8 bySrcChn = 0;
    if (CHN_ADPMODE_MVBRD == byChnMode)
    {
        bySrcChn = m_tPlayEqp == m_tVidBrdSrc ? m_byPlayChnnl : 0;
        tSrc = m_tVidBrdSrc;
    }
    else if (CHN_ADPMODE_DSBRD == byChnMode)
    {
        bySrcChn = m_tPlayEqp == m_tDoubleStreamSrc ? m_byPlayChnnl : 0;
        tSrc = m_tDoubleStreamSrc;
    }

    if (tSrc.IsNull())
    {
        ConfLog(FALSE, "[StartHdBasSwitch] tSrc.IsNull(), failed!\n");
        return FALSE;
    }

    StartSwitchToPeriEqp(tSrc, 
                         bySrcChn,
                         tEqp.GetEqpId(),
                         byChnId,
                         byMediaMode, bySwitchMode, TRUE);
                         
    if(bAudSWAdd)
    {
        StartSwitchToPeriEqp(tSrc, 
		                     bySrcChn,
		                     tEqp.GetEqpId(),
		                     byChnId,
		                     MODE_AUDIO, bySwitchMode, TRUE);	
    }
    
    //桥
    g_cMpManager.SetSwitchBridge(tEqp, byChnId*MAXNUM_VOUTPUT, byMediaMode, TRUE );
    g_cMpManager.SetSwitchBridge(tEqp, byChnId*MAXNUM_VOUTPUT+1, byMediaMode, TRUE );
    
    if(bAudSWAdd)
   	{
   		g_cMpManager.SetSwitchBridge(tEqp, byChnId*MAXNUM_VOUTPUT, MODE_AUDIO, TRUE);
   		g_cMpManager.SetSwitchBridge(tEqp, byChnId*MAXNUM_VOUTPUT+1, MODE_AUDIO, TRUE);
   	}
    
    //低速录像
    if (IsRecordSrcHDBas() &&
        !m_tConf.m_tStatus.IsNoRecording())
    {
        TEqp tLowBRBas;
        tLowBRBas.SetNull();
        u8 byLowBRBasChnId = 0;
        m_cBasMgr.GetLowBREqp(m_tConf, tLowBRBas, byLowBRBasChnId);
        if (tLowBRBas == tEqp && byLowBRBasChnId == byChnId)
        {
            StartSwitchToPeriEqp(tEqp, 
                                 byChnId*MAXNUM_VOUTPUT+1, 
                                 m_tRecEqp.GetEqpId(), 
                                 m_byRecChnnl, 
                                 byMediaMode, bySwitchMode, TRUE);
        }
    }
    
    //mt
    StartSwitchToSubMtNeedAdp(tEqp, byChnId);

    //prs
    if (m_tConf.GetConfAttrb().IsResendLosePack())
    {
		// BAS输入RTCP申述交换
		TLogicalChannel tLogicalChannel;
		m_ptMtTable->GetMtLogicChnnl(tSrc.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);
		u32 dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
		u16 wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();    
		SwitchVideoRtcpToDst(dwDstIp, wDstPort, tEqp, byChnId, MODE_VIDEO, SWITCH_MODE_SELECT, TRUE);
		
		// BAS输出交换
		for (u8 byOutChnl = 0; byOutChnl < MAXNUM_VOUTPUT; byOutChnl++)
		{
			u8 byPrsId, byPrsChnlId = 0;
			if (m_tConfPrsInfo.FindPrsForSrc(tEqp, byChnId * MAXNUM_VOUTPUT + byOutChnl,
				                             byPrsId, byPrsChnlId))
			{
				BuildRtcpAndRtpSwithForPrs(byPrsId, byPrsChnlId);
			}
			else
			{
				TPrsChannel tIdlePrsChnl;
				if (g_cMcuVcApp.GetIdlePrsChls(1, tIdlePrsChnl))
				{
					if (ChangePrsSrc(tIdlePrsChnl.GetPrsId(), tIdlePrsChnl.m_abyPrsChannels[0],
									 tEqp, byChnId * MAXNUM_VOUTPUT + byOutChnl) &&
						m_tConfPrsInfo.AddPrsChnl(tIdlePrsChnl.GetPrsId(), tIdlePrsChnl.m_abyPrsChannels[0],
												  byMediaMode, tEqp, byChnId * MAXNUM_VOUTPUT + byOutChnl))
					{
						OccupyPrsChnl(tIdlePrsChnl.GetPrsId(), tIdlePrsChnl.m_abyPrsChannels[0]);
						// 若PRS无响应需释放对应的PRS通道
					}
				}
				else
				{
					EqpLog("[StartHdBasSwitch]warning Not enough idle prs channel for the channel%d of eqp%d\n",
							byChnId * MAXNUM_VOUTPUT + byOutChnl, tEqp.GetEqpId());
				}
			}
		}

    }

    //低速组播
    
    return TRUE;
}

/*====================================================================
    函数名      ：StopHdBasSwitch
    功能        ：停止某hdbas的所有交换
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/27    4.5         张宝卿         创建
====================================================================*/
BOOL32 CMcuVcInst::StopHdBasSwitch(const TEqp &tEqp, u8 byChnId)
{
    u8 byMediaMode = MODE_NONE;
    u8 bySwitchMode = SWITCH_MODE_NONE;
    u8 byChnMode = m_cBasMgr.GetChnMode(tEqp, byChnId);

    if (!CheckHdBasChnMode(byChnMode, byMediaMode, bySwitchMode))
    {
        ConfLog(FALSE, "[StopHdBasSwitch] BasChn<%d, %d> unexist!\n", tEqp.GetEqpId(), byChnId);
        return FALSE;
    }

    //FIXME: 这里有漏洞，暂时认为RemoveVID一定伴随RemoveAud，这个在选看适配时不成立，需要独立调整
    BOOL32 bStopAud = MODE_VIDEO == bySwitchMode;
    
    //zbq[08/29/2009]这里在带发言人的纯视频轮询下果然有问题：轮询到非适配源时误拆了发言人的交换
    //这里过滤不拆的交换在轮询结束时强制补拆一次
    bStopAud &= CONF_POLLMODE_VIDEO != m_tConf.m_tStatus.GetPollMode();

    //Eqp
    StopSwitchToPeriEqp(tEqp.GetEqpId(), byChnId, TRUE, byMediaMode);
    if(bStopAud)
    {
    	StopSwitchToPeriEqp(tEqp.GetEqpId(), byChnId, TRUE, MODE_AUDIO);
    }
    
    //桥
    g_cMpManager.RemoveSwitchBridge(tEqp, byChnId*MAXNUM_VOUTPUT, byMediaMode, TRUE );
    g_cMpManager.RemoveSwitchBridge(tEqp, byChnId*MAXNUM_VOUTPUT+1, byMediaMode, TRUE );
    if(bStopAud)
    {
    	g_cMpManager.RemoveSwitchBridge(tEqp, byChnId*MAXNUM_VOUTPUT, MODE_AUDIO, TRUE );
    	g_cMpManager.RemoveSwitchBridge(tEqp, byChnId*MAXNUM_VOUTPUT+1, MODE_AUDIO, TRUE );    	
    }

    //mt
    StopSwitchToSubMtNeedAdp(tEqp, byChnId);

    //广播适配模式下停低速录像
    if (CHN_ADPMODE_MVBRD == byChnMode)
    {
        //低速录像
        if (IsRecordSrcHDBas() &&
            !m_tConf.m_tStatus.IsNoRecording())
        {
            StopSwitchToPeriEqp(m_tRecEqp.GetEqpId(), m_byRecChnnl, FALSE, byMediaMode);
        }
    }

    //prs
    if (m_tConf.GetConfAttrb().IsResendLosePack())
    {
		u8 byPrsId, byPrsChnlId = 0;
		for(u8 byOutChnl = 0; byOutChnl < MAXNUM_VOUTPUT; byOutChnl++)
		{
			if (m_tConfPrsInfo.FindPrsForSrc(tEqp, byChnId*MAXNUM_VOUTPUT + byOutChnl,
				                             byPrsId, byPrsChnlId))
			{
				StopPrs(byPrsId, byPrsChnlId);
				RlsPrsChnl(byPrsId, byPrsChnlId);				
			}
		}
	}


    return TRUE;
}

/*====================================================================
    函数名      ：StartSwitchToSubMtFromAdp
    功能        ：将数据交换到会议中 接本适配通道的某终端
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/27    4.5         张宝卿         创建
    09/07/26	  4.6		张宝卿         增加支持音频过适配支持
	09/11/23	4.6			张宝卿		   增加支持卫星会议的适配广播
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchToSubMtFromAdp(u8 byMtId, BOOL32 bDual)
{
    if ((!m_cMtRcvGrp.IsMtNeedAdp(byMtId) && !bDual) ||
        (!m_cMtRcvGrp.IsMtNeedAdp(byMtId, FALSE) && bDual))
    {
        ConfLog(FALSE, "[StartSwitchToSubMtFromAdp] Mt.%d needn't adp, bDual.%d\n", byMtId, bDual);
        return FALSE;
    }

    u8 byMediaType = MEDIA_TYPE_NULL;
    u8 byRes = 0;

    TEqp tHDBas;
    u8 byChnId = 0;
    u8 byOutIdx = 0;
    
    BOOL32 bRet = FALSE;
    BOOL32 bRet1 = FALSE;

    if (!bDual)
    {
        //不交换给自己，即使需要适配
        if (m_tVidBrdSrc.GetMtType() == MT_TYPE_MT &&
            m_tVidBrdSrc.GetMtId() == byMtId)
        {
            return TRUE;
        }
        bRet = m_cMtRcvGrp.GetMtMediaRes(byMtId, byMediaType, byRes);
        bRet1 = m_cBasMgr.GetBasResource(byMediaType, byRes, tHDBas, byChnId, byOutIdx);
        tHDBas.SetConfIdx(m_byConfIdx);

        if ( bRet && bRet1 )
        {
        	//视频
			g_cMpManager.SetSwitchBridge(tHDBas, byChnId * MAXNUM_VOUTPUT + byOutIdx, MODE_VIDEO);
            NotifyEqpFastUpdate(tHDBas, byChnId, TRUE);


#ifndef _SATELITE_
            StartSwitchToSubMt(tHDBas, byChnId * MAXNUM_VOUTPUT + byOutIdx, byMtId, MODE_VIDEO);

			//音频伴随处理
            StartSwitchAud2MtNeedAdp(byMtId);
#else
			//标清作源，广播适配打到第一组播点；选看另外处理

			BOOL32 bVidSrcHd = ::topoGetMtInfo(LOCAL_MCUID, m_tVidBrdSrc.GetMtId(), g_atMtTopo, g_wMtTopoNum).IsMtHd();

			if (bVidSrcHd)
			{
				g_cMpManager.StartSatConfCast(tHDBas, CAST_SEC, MODE_VIDEO, byChnId * MAXNUM_VOUTPUT + byOutIdx);
			}
			else
			{
				g_cMpManager.StartSatConfCast(tHDBas, CAST_FST, MODE_VIDEO, byChnId * MAXNUM_VOUTPUT + byOutIdx);
			}
			m_ptMtTable->SetMtSrc(byMtId, &tHDBas, MODE_VIDEO);
#endif
        }
        else
        {
            //zbq[01/05/2009] 临时处理：反向适配资源不足时的紧凑对齐
            TSimCapSet tSrcSim;
            memset(&tSrcSim, 0, sizeof(tSrcSim));
            GetMVBrdSrcSim(tSrcSim);
            if (MEDIA_TYPE_H264 == m_tConf.GetMainVideoMediaType() &&
                MEDIA_TYPE_H264 != tSrcSim.GetVideoMediaType() 
                //取消单速限制
                //FIXME: 对高分辨率低码率的终端的带宽调控未作处理
                /*&&
                0 == m_tConf.GetSecBitRate()*/)
            {
                u8 byAdjType = m_tConf.GetMainVideoMediaType();
                u8 byAdjRes = m_tConf.GetMainVideoFormat();
                
                //本终端是否需要走紧凑适配
                u8 byProximalType = 0;
                u8 byProximalRes = 0;
                BOOL32 bRet = GetProximalGrp(byAdjType,
                                             byAdjRes, 
                                             byProximalType, byProximalRes);
                if (!bRet)
                {
                    ConfLog(FALSE, "[StartSwitchToSubMtFromAdp] no proximal grp for <%d, %d\n", byAdjType, byAdjRes);
                    return FALSE;
                }
                //取紧凑适配的bas资源
                bRet1 = m_cBasMgr.GetBasResource(byProximalType, 
                                                 byProximalRes, 
                                                 tHDBas, byChnId, byOutIdx);
                tHDBas.SetConfIdx(m_byConfIdx);
                if (!bRet1)
                {
                    ConfLog(FALSE, "[StartSwitchToSubMtFromAdp] get proximal bas src failed!\n");
                    return FALSE;
                }
                
                //视频
				g_cMpManager.SetSwitchBridge(tHDBas, byChnId * MAXNUM_VOUTPUT + byOutIdx, MODE_VIDEO); 
                NotifyEqpFastUpdate(tHDBas, byChnId, TRUE);

#ifndef _SATELITE_
                StartSwitchToSubMt(tHDBas, byChnId * MAXNUM_VOUTPUT + byOutIdx, byMtId, MODE_VIDEO);

				//音频伴随处理
                StartSwitchAud2MtNeedAdp(byMtId);

#else
				//标清作源，广播适配打到第一组播点；选看另外处理
				
				BOOL32 bVidSrcHd = ::topoGetMtInfo(LOCAL_MCUID, m_tVidBrdSrc.GetMtId(), g_atMtTopo, g_wMtTopoNum).IsMtHd();
				if (bVidSrcHd)
				{
					g_cMpManager.StartSatConfCast(tHDBas, CAST_SEC, MODE_VIDEO, byChnId * MAXNUM_VOUTPUT + byOutIdx);
				}
				else
				{
					g_cMpManager.StartSatConfCast(tHDBas, CAST_FST, MODE_VIDEO, byChnId * MAXNUM_VOUTPUT + byOutIdx);
				}
				m_ptMtTable->SetMtSrc(byMtId, &tHDBas, MODE_VIDEO);
#endif

            }
            else
            {
                ConfLog(FALSE, "[StartSwitchToSubMtFromAdp] failed, ret<%d, %d>\n", bRet, bRet1);
            }
        }
    }
    else
    {
        //不交换给自己，即使需要适配
        if (m_tDoubleStreamSrc.GetMtType() == MT_TYPE_MT &&
            m_tDoubleStreamSrc.GetMtId() == byMtId)
        {
            return TRUE;
        }
        bRet = m_cMtRcvGrp.GetMtMediaRes(byMtId, byMediaType, byRes, TRUE);
        bRet1 = m_cBasMgr.GetBasResource(byMediaType,
                                         byRes,
                                         tHDBas,
                                         byChnId,
                                         byOutIdx, 
                                         TRUE, 
                                         IsDSSrcH263p() /*|| IsConfDualEqMV(m_tConf)*/);

        if ( bRet && bRet1 )
        {
            tHDBas.SetConfIdx(m_byConfIdx);


			g_cMpManager.SetSwitchBridge(tHDBas, byChnId * MAXNUM_VOUTPUT + byOutIdx, MODE_SECVIDEO, TRUE); 

#ifndef _SATELITE_
            g_cMpManager.StartSwitchToSubMt(tHDBas,
                                            byChnId * MAXNUM_VOUTPUT + byOutIdx,
                                            m_ptMtTable->GetMt(byMtId),
                                            MODE_SECVIDEO,
                                            SWITCH_MODE_BROADCAST, TRUE, TRUE, TRUE);
            

#else
			BOOL32 bVidSrcHd = ::topoGetMtInfo(LOCAL_MCUID, m_tDoubleStreamSrc.GetMtId(), g_atMtTopo, g_wMtTopoNum).IsMtHd();
			
			if (bVidSrcHd)
			{
				g_cMpManager.StartSatConfCast(tHDBas, CAST_SEC, MODE_SECVIDEO, byChnId * MAXNUM_VOUTPUT + byOutIdx);
			}
			else
			{
				g_cMpManager.StartSatConfCast(tHDBas, CAST_FST, MODE_SECVIDEO, byChnId * MAXNUM_VOUTPUT + byOutIdx);
			}
			m_ptMtTable->SetMtSrc(byMtId, &tHDBas, MODE_SECVIDEO);
#endif

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
                bRet1 = m_cBasMgr.GetBasResource(byProximalType,
                                                 byProximalRes,
                                                 tHDBas,
                                                 byChnId,
                                                 byOutIdx, TRUE, TRUE);
                tHDBas.SetConfIdx(m_byConfIdx);
                if (!bRet1)
                {
                    ConfLog(FALSE, "[StartSwitchToSubMtFromAdp] get proximal bas src failed(dual)!\n");
                    return FALSE;
                }
				
				g_cMpManager.SetSwitchBridge(tHDBas, byChnId * MAXNUM_VOUTPUT + byOutIdx, MODE_SECVIDEO, TRUE); 

#ifndef _SATELITE_
                g_cMpManager.StartSwitchToSubMt(tHDBas,
                                                byChnId * MAXNUM_VOUTPUT + byOutIdx,
                                                m_ptMtTable->GetMt(byMtId),
                                                MODE_SECVIDEO,
                                                SWITCH_MODE_BROADCAST, TRUE, TRUE, TRUE);
                

#else
				BOOL32 bVidSrcHd = ::topoGetMtInfo(LOCAL_MCUID, m_tDoubleStreamSrc.GetMtId(), g_atMtTopo, g_wMtTopoNum).IsMtHd();
				if (bVidSrcHd)
				{
					g_cMpManager.StartSatConfCast(tHDBas, CAST_SEC, MODE_SECVIDEO, byChnId * MAXNUM_VOUTPUT + byOutIdx);
				}
				else
				{
					g_cMpManager.StartSatConfCast(tHDBas, CAST_FST, MODE_SECVIDEO, byChnId * MAXNUM_VOUTPUT + byOutIdx);
				}
				m_ptMtTable->SetMtSrc(byMtId, &tHDBas, MODE_SECVIDEO);
#endif

            }
            else
            {
                ConfLog(FALSE, "[StartSwitchToSubMtFromAdp] failed, ret<%d, %d(dual)>\n", bRet, bRet1);
            }
        }
    }

    return bRet & bRet1;
}

/*====================================================================
    函数名      ：CheckHdBasChnMode
    功能        ：HD-BAS适配模式校验 提取交换模式等
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/04/17    4.6         张宝卿         创建
====================================================================*/
BOOL32 CMcuVcInst::CheckHdBasChnMode(u8 byMode, u8 &byMediaMode, u8 &bySwitchMode)
{
    byMediaMode = MODE_NONE;
    bySwitchMode = SWITCH_MODE_NONE;
    
    switch (byMode)
    {
    case CHN_ADPMODE_MVBRD:
        byMediaMode = MODE_VIDEO;
        bySwitchMode = SWITCH_MODE_BROADCAST;
        break;
        
    case CHN_ADPMODE_DSBRD:
        byMediaMode = MODE_SECVIDEO;
        bySwitchMode = SWITCH_MODE_BROADCAST;
        break;
        
    case CHN_ADPMODE_MVSEL:
        byMediaMode = MODE_VIDEO;
        bySwitchMode = SWITCH_MODE_SELECT;
        break;
        
    case CHN_ADPMODE_DSSEL:
        byMediaMode = MODE_SECVIDEO;
        bySwitchMode = SWITCH_MODE_SELECT;
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

/*====================================================================
    函数名      ：StopSwitchToSubMtFromAdp
    功能        ：将数据停止交换到会议中 接本适配通道的某终端
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/11/27    4.5         张宝卿         创建
    09/07/26    4.6         张宝卿         增加音频交换的绑定处理
====================================================================*/
BOOL32 CMcuVcInst::StopSwitchToSubMtFromAdp(u8 byMtId, BOOL32 bDual)
{
    if ((!m_cMtRcvGrp.IsMtNeedAdp(byMtId) && !bDual) ||
        (!m_cMtRcvGrp.IsMtNeedAdp(byMtId, FALSE) && bDual))
    {
        ConfLog(FALSE, "[StopSwitchToSubMtFromAdp] Mt.%d needn't adp, bDual.%d\n", byMtId, bDual);
        return FALSE;
    }

    if (!bDual)
    {
        StopSwitchToSubMt(byMtId, MODE_VIDEO);
        StopSwitchToSubMt(byMtId, MODE_AUDIO);
    }
    else
    {
        StopSwitchToSubMt(byMtId, MODE_SECVIDEO);
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

    if ( MODE_AUDIO == byMode || MODE_BOTH == byMode )
    {
        for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
        {
            if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )    
            {
                StopSwitchToSubMt( byLoop, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE );
            }
        }
    }
    
    if ( MODE_VIDEO == byMode || MODE_BOTH == byMode )
    {
        // 删除广播源
        u8 bySrcChnl = (m_tVidBrdSrc==m_tPlayEqp) ? m_byPlayChnnl : 0;
        g_cMpManager.StopSwitchToBrd( m_tVidBrdSrc, bySrcChnl );
        
        // 删除广播观众
        for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
        {
            if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )           
            {
                StopSwitchToSubMt( byLoop, MODE_VIDEO, SWITCH_MODE_BROADCAST, FALSE );
            }
        }
    }

	MtStatusChange();
    
    return;
}

/*====================================================================
    函数名      ：StopSwitchToAllSubMtJoinedConfNeedAdapt
    功能        ：停止将数据交换到会议中所有需适配终端
    算法实现    ：
    引用全局变量：
    输入参数说明：BOOL32 bMsg, 是否发通知接收信息，缺省为TRUE
				  u8 byMode, 交换模式，缺省为MODE_BOTH
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/08    1.0         LI Yi         创建
====================================================================*/
//modify bas 2
void CMcuVcInst::StopSwitchToAllSubMtJoinedConfNeedAdapt(BOOL32 bMsg, u8 byMode, u8 byAdaptType)
{
    u8	byLoop;

    TMt tSrcMt;
    tSrcMt.SetNull();
    TMt tDstMt;
    for (byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        //MT joined conference and need adapt, stop switch to it
        if (m_tConfAllMtInfo.MtJoinedConf(byLoop))
        {
            tDstMt = m_ptMtTable->GetMt(byLoop);
            if (IsMtNeedAdapt(byAdaptType, byLoop, &tSrcMt))
            {
                TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
                tConfAttrb.SetUseAdapter(FALSE);
                m_tConf.SetConfAttrb(tConfAttrb);
                //RestoreRcvMediaBrdSrc(byLoop, byMode, FALSE);
                StopSwitchToSubMt( byLoop, byMode, SWITCH_MODE_BROADCAST, FALSE );
                tConfAttrb.SetUseAdapter(TRUE);
                m_tConf.SetConfAttrb(tConfAttrb);
            }			
        }
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
   返回值说明  ：TRUE，如果不交换直接返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/20    1.0         LI Yi         创建
	03/07/24    2.0         胡昌威        修改
====================================================================*/
BOOL32 CMcuVcInst::StartSwitchToPeriEqp(TMt tSrc,
                                        u8	bySrcChnnl,
                                        u8	byEqpId,
                                        u16 wDstChnnl,
                                        u8	byMode,
                                        u8	bySwitchMode,
                                        BOOL32 bDstHDBas,
                                        BOOL32 bStopBeforeStart,
                                        BOOL32 bBatchPollChg) 
{
	CServMsg	cServMsg;
	TPeriEqpStatus	tStatus;

	tSrc = GetLocalMtFromOtherMcuMt( tSrc );

	//未连接
	if( !g_cMcuVcApp.IsPeriEqpConnected( byEqpId ) )
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
			return TRUE;
		}
		else if ( byMode == MODE_BOTH )
		{
			byMode = MODE_VIDEO;
		}
	}

    if(!g_cMpManager.StartSwitchToPeriEqp(tSrc, bySrcChnnl, byEqpId, wDstChnnl, 
										  byMode, bySwitchMode, bDstHDBas, bStopBeforeStart))
	{
		ConfLog( FALSE, "StartSwitchToPeriEqp() failed! Cannot switch to specified eqp!\n" );
		return FALSE;
	}

	u32 dwTimeIntervalTicks = 3*OspClkRateGet();

	//通知终端开始发送
    if (tSrc.GetType() == TYPE_MT)
    {
        //zbq[04/15/2009] Tand的关键帧编的比较快，且间隔为3s. 故交换建立后再发youareseeing，尽量拿到第一个关键帧
        if (IsDelayVidBrdVCU() && tSrc == m_tVidBrdSrc)
        {
        }
        else
        {
            NotifyMtSend( tSrc.GetMtId(), byMode, TRUE );
			
			TEqp tEqp = g_cMcuVcApp.GetEqp(byEqpId);
			if ( byEqpId >= HDUID_MIN && byEqpId <= HDUID_MAX)
			{
				// zbq [06/25/2007] 建音频交换不请求关键帧
				if( MODE_BOTH == byMode || MODE_VIDEO == byMode )
				{            
					NotifyFastUpdate(tSrc, MODE_VIDEO, FALSE);
				}
			}
			else
			{
				// zbq [06/25/2007] 建音频交换不请求关键帧
				if( MODE_BOTH == byMode || MODE_VIDEO == byMode )
				{            
					NotifyFastUpdate(tSrc, MODE_VIDEO, TRUE);
				}
			}
        }
    }

	g_cMcuVcApp.SetPeriEqpSrc( byEqpId, (const TMt*)&tSrc, (u8)wDstChnnl, byMode );

	//send message according to its type
	if( !g_cMcuVcApp.GetPeriEqpStatus( byEqpId, &tStatus ) )
	{
		return FALSE;
	}

    TTvWallStartPlay tTwPlayPara;
    THduStartPlay  tHduPlayPara;
	if (MODE_AUDIO  == byMode)
    {
        byMode = HDU_OUTPUTMODE_AUDIO;
    }
    else if (MODE_VIDEO == byMode)
    {
        byMode = HDU_OUTPUTMODE_VIDEO;
    }
    else if (MODE_BOTH  == byMode)
    {
        byMode = HDU_OUTPUTMODE_BOTH;
    }
    else
    {
        byMode = HDU_OUTPUTMODE_BOTH;
    }

	tHduPlayPara.SetMode( byMode );

    TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
    if (EQP_TYPE_HDU == tStatus.GetEqpType())
    {
        tHduPlayPara.SetMt(tSrc);
		tHduPlayPara.SetVideoEncrypt(tEncrypt);
    }
	else
	{
		tTwPlayPara.SetMt(tSrc);
	    tTwPlayPara.SetVideoEncrypt(tEncrypt);
	}

    if (m_tConf.GetConfAttrb().IsResendLosePack())
    {
        u32 dwMtSwitchIp;
        u16 wMtSwitchPort;
        u32 dwMtSrcIp;

        g_cMpManager.GetSwitchInfo(tSrc, dwMtSwitchIp, wMtSwitchPort, dwMtSrcIp);
		if (EQP_TYPE_HDU == tStatus.GetEqpType())
		{
            tHduPlayPara.SetRtcpBackAddr(dwMtSwitchIp, 0);
            tHduPlayPara.SetIsNeedByPrs(TRUE);
		}
		else
		{
            tTwPlayPara.SetRtcpBackAddr(dwMtSwitchIp, 0);
            tTwPlayPara.SetIsNeedByPrs(TRUE);
		}

    }
    else
    {
		if (EQP_TYPE_HDU == tStatus.GetEqpType())
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
		EqpLog("[StartSwitchToPeriEqp] tDVPayload.GetRealPayLoad() = %u\n", tDVPayload.GetRealPayLoad());
		EqpLog("[StartSwitchToPeriEqp] tDVPayload.GetActivePayload() = %u\n", tDVPayload.GetActivePayload());
    }
    else
    {
        tDVPayload.SetRealPayLoad(tSrcSCS.GetVideoMediaType());
        tDVPayload.SetActivePayload(tSrcSCS.GetVideoMediaType());
		EqpLog("[StartSwitchToPeriEqp] tDVPayload.GetRealPayLoad() = %u\n", tDVPayload.GetRealPayLoad());
		EqpLog("[StartSwitchToPeriEqp] tDVPayload.GetActivePayload() = %u\n", tDVPayload.GetActivePayload());
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



	switch(tStatus.GetEqpType())
	{
	case EQP_TYPE_TVWALL:
		{
		    cServMsg.SetMsgBody((u8 *)&tTwPlayPara, sizeof(tTwPlayPara));
		    cServMsg.CatMsgBody((u8 *)&tDVPayload, sizeof(tDVPayload));
		    cServMsg.CatMsgBody((u8 *)&tDAPayload, sizeof(tDAPayload));
  	        cServMsg.SetChnIndex((u8)wDstChnnl);
			// MCU前向纠错，zgc，2007-09-27
			TCapSupportEx tCapSupportEx = m_tConf.GetCapSupportEx();
			cServMsg.CatMsgBody((u8*)&tCapSupportEx, sizeof(tCapSupportEx));

			SendMsgToEqp(byEqpId, MCU_TVWALL_START_PLAY_REQ, cServMsg);
			break;
		}
	case EQP_TYPE_HDU:
		cServMsg.SetMsgBody((u8 *)&tHduPlayPara, sizeof(tHduPlayPara));
		cServMsg.CatMsgBody((u8 *)&tDVPayload, sizeof(tDVPayload));
		cServMsg.CatMsgBody((u8 *)&tDAPayload, sizeof(tDAPayload));
		cServMsg.SetChnIndex((u8)wDstChnnl);
        
        if (!bBatchPollChg)
        {
            SendMsgToEqp(byEqpId, MCU_HDU_START_PLAY_REQ, cServMsg);
        }

		// xliang [7/15/2009] 向VMP请求关键帧
		if(EQP_TYPE_VMP == tSrc.GetEqpType())
		{
			NotifyFastUpdate(m_tVmpEqp, bySrcChnnl);
		}
        break;
	}

	return TRUE;
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
void CMcuVcInst::StopSwitchToPeriEqp( u8 byEqpId,  u16 wDstChnnl, BOOL32 bMsg, u8 byMode, u8 bySwitchMode )
{
	CServMsg	cServMsg;
	TEqp	tEqp;
	TPeriEqpStatus	tStatus;
	TMt    TMtNull;
	
    // guzh [5/12/2007] 检查是否是停混音交换
    if (g_cMcuVcApp.GetEqpType( byEqpId ) == EQP_TYPE_MIXER &&
        m_tMixEqp.GetEqpId() == byEqpId)
    {
        // zbq [03/19/2007] 这里清一次混音通道位置
        for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++ )
        {
            if ( m_tConfAllMtInfo.MtJoinedConf(byMtId) )
            {
                if ( 0 != GetMixChnPos(byMtId) &&
                     GetMixChnPos(byMtId) == wDstChnnl )
                {
                    GetMixChnPos( byMtId, TRUE );
                    MpManagerLog( "[StopSwitchToPeriEqp] Mixer Chn pos.%d for MT.%d has been cleared !\n", wDstChnnl, byMtId );
                    break;
                }
            }
        }
    }

	//stop switch
	g_cMpManager.StopSwitchToPeriEqp( m_byConfIdx, byEqpId, wDstChnnl, byMode );
	
	TMtNull.SetNull();
	if( g_cMcuVcApp.GetEqpType( byEqpId ) != EQP_TYPE_MIXER )
	{
		g_cMcuVcApp.SetPeriEqpSrc( byEqpId, &TMtNull, (u8)wDstChnnl, byMode );
	}

	//send stop message
	if( bMsg )
	{
		g_cMcuVcApp.GetPeriEqpStatus( byEqpId, &tStatus );
		cServMsg.SetChnIndex( (u8)wDstChnnl );
		cServMsg.SetConfId( m_tConf.GetConfId() );
		tEqp.SetMcuEqp( (u8)LOCAL_MCUID, byEqpId, tStatus.GetEqpType() );
		cServMsg.SetMsgBody( ( u8 * )&tEqp, sizeof( tEqp ) );
		switch( tStatus.GetEqpType() )
		{
		case EQP_TYPE_TVWALL:
			SendMsgToEqp( byEqpId, MCU_TVWALL_STOP_PLAY_REQ, cServMsg );
			break;

		case EQP_TYPE_HDU:    //4.6 新加  jlb
			SendMsgToEqp( byEqpId, MCU_HDU_STOP_PLAY_REQ, cServMsg );
			break;

		case EQP_TYPE_RECORDER:
			SendMsgToEqp( byEqpId, MCU_REC_STOPREC_REQ, cServMsg );
			break;
        case EQP_TYPE_BAS:
            SendMsgToEqp( byEqpId, MCU_BAS_STOPADAPT_REQ, cServMsg );
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

	tSrc = GetLocalMtFromOtherMcuMt( tSrc );

	for( byEqpId = 1; byEqpId < MAXNUM_MCU_PERIEQP; byEqpId++)
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
void CMcuVcInst::StopSwitchToAllPeriEqpWatchingSrcMcu( const u16 wMcuId, BOOL32 bMsg , u8 byMode )
{
	TMt tSrcMt;
	u8      byEqpId;
	u8      byChnnlNo;

	for( byEqpId = 1; byEqpId < MAXNUM_MCU_PERIEQP; byEqpId++)
	{
		if( !g_cMcuVcApp.IsPeriEqpConnected( byEqpId ) )
			continue;

		//目前只对电视墙操作
		if( g_cMcuAgent.GetPeriEqpType( byEqpId ) != EQP_TYPE_TVWALL )
			continue;

		for( byChnnlNo = 0; byChnnlNo < MAXNUM_PERIEQP_CHNNL; byChnnlNo++ )
		{
			if( g_cMcuVcApp.GetPeriEqpSrc( byEqpId, &tSrcMt, byChnnlNo, MODE_VIDEO )
				&& tSrcMt.GetMcuId() == wMcuId )
				StopSwitchToPeriEqp( byEqpId, byChnnlNo, TRUE, MODE_VIDEO );

			if( g_cMcuVcApp.GetPeriEqpSrc( byEqpId, &tSrcMt, byChnnlNo, MODE_AUDIO )
				&& tSrcMt.GetMcuId() == wMcuId )
				StopSwitchToPeriEqp( byEqpId, byChnnlNo, TRUE, MODE_AUDIO );
		}
	}
}

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
								u8 byDstChnnl, u8 byMode ) 
{
	CServMsg	cServMsg;

	tSrc = GetLocalMtFromOtherMcuMt( tSrc );
	

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
			return TRUE;
		}
		else if ( byMode == MODE_BOTH )
		{
			byMode = MODE_VIDEO;
		}
	}

    if(!g_cMpManager.StartSwitchToMc(tSrc,bySrcChnnl,wMcInstId,byDstChnnl,byMode))
	{
		ConfLog( FALSE, "StartSwitchToMc() failed! Cannot switch to specified eqp!\n" );
		return FALSE;
	}

	u32 dwTimeIntervalTicks = 3*OspClkRateGet();

	//通知终端开始发送
	if( tSrc.GetType() == TYPE_MT )
	{
		NotifyMtSend( tSrc.GetMtId(), byMode, TRUE );

        // zbq [06/25/2007] 建音频交换不请求关键帧
        if ( MODE_VIDEO == byMode || MODE_BOTH == byMode )
        {
            NotifyFastUpdate(tSrc, MODE_VIDEO, TRUE);
        }
	}

	g_cMcuVcApp.SetMcSrc( wMcInstId, ( const TMt* )&tSrc, byDstChnnl, byMode );


	//send message
	TSwitchInfo tSwitchInfo;
	tSwitchInfo.SetSrcMt( tSrc );
	tSwitchInfo.SetSrcChlIdx( bySrcChnnl );
	tSwitchInfo.SetDstChlIdx( byDstChnnl );
	tSwitchInfo.SetMode( byMode );
	
	//加密以及动态载荷信息
	TMediaEncrypt tEncrypt = m_tConf.GetMediaKey();
	TSimCapSet tSrcSCS = m_ptMtTable->GetSrcSCS( tSrc.GetMtId() ); 
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
		
    cServMsg.SetChnIndex( byDstChnnl );
	cServMsg.SetMsgBody( ( u8 * )&tSwitchInfo, sizeof( tSwitchInfo ) );
	cServMsg.CatMsgBody( (u8 *)&tEncrypt, sizeof(tEncrypt));
	cServMsg.CatMsgBody( (u8 *)&tDVPayload, sizeof(tDVPayload));
	cServMsg.CatMsgBody( (u8 *)&tEncrypt, sizeof(tEncrypt));
	cServMsg.CatMsgBody( (u8 *)&tDAPayload, sizeof(tDAPayload));
    
    // zw [06/26/2008] 适应AAC LC格式
    if ( MEDIA_TYPE_AACLC == tSrcSCS.GetAudioMediaType() )
    {
        TAudAACCap tAudAACCap;
        tAudAACCap.SetMediaType(MEDIA_TYPE_AACLC);
        tAudAACCap.SetSampleFreq(AAC_SAMPLE_FRQ_32);
        tAudAACCap.SetChnlType(AAC_CHNL_TYPE_SINGLE);
        tAudAACCap.SetBitrate(96);
        tAudAACCap.SetMaxFrameNum(AAC_MAX_FRM_NUM);
    
	    cServMsg.CatMsgBody( (u8 *)&tAudAACCap, sizeof(tAudAACCap) );
    }
	
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

	if( bFilterConn && !g_cMcuVcApp.IsMcConnected( wMcInstId ) ) return;
	
	//stop switch
    g_cMpManager.StopSwitchToMc( m_byConfIdx, wMcInstId, byDstChnnl, byMode );

	tMtNull.SetNull();
	g_cMcuVcApp.SetMcSrc( wMcInstId, &tMtNull, byDstChnnl, byMode );

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
	u8  byLoop;
	TMt tMt;

	for( byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		if( m_tConfAllMtInfo.MtJoinedConf( byLoop ) )
		{
			tMt = m_ptMtTable->GetMt( byLoop );
			StopSwitchToAllMcWatchingSrcMt( tMt, bMsg, byMode );	
		}
	}
}

/*====================================================================
    函数名      ：StopSwitchToAllMcWatchingSrcMt
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
    03/06/28    1.0         JQL           创建
====================================================================*/
void CMcuVcInst::StopSwitchToAllMcWatchingSrcMt(TMt tSrc, BOOL32 bMsg, u8 byMode )
{
	TMt tCurSrc;
	u8      byIndex;
	u8      byChannel;
	u8      byChannelNum;
	TLogicalChannel tLogicalChannel;

	tSrc = GetLocalMtFromOtherMcuMt( tSrc );
	
	for( byIndex = 1; byIndex <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byIndex++ )
	{
		if( !g_cMcuVcApp.IsMcConnected( byIndex ) )
			continue;
        
		g_cMcuVcApp.GetMcLogicChnnl( byIndex, MODE_VIDEO, &byChannelNum, &tLogicalChannel );
		
		for( byChannel = 0; byChannel < byChannelNum; byChannel++ )
		{
			if( ( byMode == MODE_VIDEO || byMode == MODE_BOTH ) 
				&& g_cMcuVcApp.GetMcSrc( byIndex, &tCurSrc, byChannel, MODE_VIDEO )
				&& tCurSrc == tSrc )
				StopSwitchToMc( byIndex, byChannel, bMsg, MODE_VIDEO );

			if( ( byMode == MODE_AUDIO || byMode == MODE_BOTH ) 
				&& g_cMcuVcApp.GetMcSrc( byIndex, &tCurSrc, byChannel, MODE_AUDIO )
				&& tCurSrc == tSrc )
				StopSwitchToMc( byIndex, byChannel, bMsg, MODE_AUDIO );
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
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/01/04    1.0         LI Yi         创建
====================================================================*/
void CMcuVcInst::SwitchSrcToDst( const TSwitchInfo &tSwitchInfo,
                                 const CServMsg & cServMsgHdr,
                                 BOOL32 bBatchPollChg )
{
	CServMsg	cServMsg;
	TMt	tDstMt = GetLocalMtFromOtherMcuMt(tSwitchInfo.GetDstMt());
	TMt tSrcMt = GetLocalMtFromOtherMcuMt(tSwitchInfo.GetSrcMt());
	u8	byMode = tSwitchInfo.GetMode();
	u8	bySrcChnnl = tSwitchInfo.GetSrcChlIdx();// xliang [1/7/2009] 目前仅在选看VMP时有用

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
#ifndef _SATELITE_
        g_cMpManager.SetSwitchBridge(tSrcMt, 0, byMode);
		StartSwitchToSubMt(tSrcMt, 0, tDstMt.GetMtId(), byMode, SWITCH_MODE_SELECT, TRUE, FALSE);
#else
		g_cMpManager.StartSwitchSrc2Dst(tSrcMt, tDstMt, byMode);


		//FIXME: 稍后整理本部分 刷新代码

		// xsl [5/22/2006]设置终端状态, 这里保存广播源信息，不保存bas信息，因为其他地方会根据源信息做相应处理
		m_ptMtTable->SetMtSrc( tDstMt.GetMtId(), ( const TMt * )&tSrcMt, byMode );
		
		//过滤目标终端为上级mcu的情况
		TMtStatus tSrcMtStatus;
		TMtStatus tMtStatus;
		u8 byAddSelByMcsMode = MODE_NONE;

		m_ptMtTable->GetMtStatus( tDstMt.GetMtId(), &tMtStatus );
		if( tSrcMt.GetType() == TYPE_MT && tSrcMt.GetMtId() != tDstMt.GetMtId() && 
			(m_tCascadeMMCU.GetMtId() == 0 || tDstMt.GetMtId() != m_tCascadeMMCU.GetMtId()) )
		{
			TMtStatus tSrcMtStatus;
			u8 byAddSelByMcsMode = MODE_NONE;
			m_ptMtTable->GetMtStatus( tSrcMt.GetMtId(), &tSrcMtStatus );
			
			//zbq[12/06/2007] VMP点名时序问题
			if( ((!(tSrcMt == m_tVidBrdSrc)) || (tSrcMt == m_tVidBrdSrc && 
												 ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() &&
												 m_tRollCaller == m_tVidBrdSrc)) &&
												 tSrcMtStatus.IsSendVideo() && 
				( MODE_VIDEO == byMode || MODE_BOTH == byMode ) )
			{
				byAddSelByMcsMode = MODE_VIDEO;
			}
			if( !(tSrcMt == m_tAudBrdSrc) && tSrcMtStatus.IsSendAudio() && 
				( MODE_AUDIO == byMode || MODE_BOTH == byMode ) )
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
				tMtStatus.AddSelByMcsMode( byMode );
			}
			else
			{
				tMtStatus.RemoveSelByMcsMode( byMode );
			}
		}
		else
		{
			tMtStatus.RemoveSelByMcsMode( byMode );
		}
		m_ptMtTable->SetMtStatus( tDstMt.GetMtId(), &tMtStatus );
		
		MtStatusChange( tDstMt.GetMtId(), TRUE );
		
		//若是选看，按目的终端的需要调节发送终端的码虑
		if( tSrcMt.GetType() == TYPE_MT && 
			( byMode == MODE_VIDEO || byMode == MODE_BOTH ) && 
			  tSrcMt.GetMtId() != m_tVidBrdSrc.GetMtId() )
		{
			u16 wMtBitRate = m_ptMtTable->GetMtReqBitrate( tDstMt.GetMtId() );
			AdjustMtVideoSrcBR( tDstMt.GetMtId(), wMtBitRate );
		}

		RefreshConfState();
#endif
		}
		break;
	case MCS_MCU_STARTSWITCHVMPMT_REQ:// xliang [1/7/2009] 主席选看VMP 
	case MT_MCU_STARTSWITCHVMPMT_REQ:	// xliang [4/9/2009] MT 触发的选看
		g_cMpManager.SetSwitchBridge(tSrcMt, bySrcChnnl, byMode); // xliang [4/2/2009] 不走广播逻辑，没必要建桥？// xliang [4/28/2009] 虽不走广播逻辑但是做为广播源要建桥
		StartSwitchToSubMt(tSrcMt, bySrcChnnl, tDstMt.GetMtId(), byMode, SWITCH_MODE_SELECT, TRUE, FALSE);
		break;
	case MCS_MCU_STARTSWITCHMC_REQ:
		StartSwitchToMc(tSrcMt, 0, cServMsgHdr.GetSrcSsnId(),tSwitchInfo.GetDstChlIdx(), byMode);
		break;
	case MCS_MCU_START_SWITCH_TW_REQ:
		StartSwitchToPeriEqp(tSrcMt, 0, tDstMt.GetEqpId(), tSwitchInfo.GetDstChlIdx(), byMode, SWITCH_MODE_SELECT);
		break;
	case MCS_MCU_START_SWITCH_HDU_REQ: // xliang [6/23/2009] 第2参数不填死0，因为可能看VMP，这时SrcChnnl由具体情况决定
		StartSwitchToPeriEqp(tSrcMt,
                             bySrcChnnl,
                             tDstMt.GetEqpId(),
                             tSwitchInfo.GetDstChlIdx(),
                             byMode,
                             SWITCH_MODE_SELECT, FALSE, TRUE, bBatchPollChg);
		break;
	default:
		ConfLog( FALSE, "Exception - wrong switch eventid %u!\n", cServMsgHdr.GetEventId() );
		return;
	}


	if ( TYPE_MT == tSrcMt.GetType())
	{
		NotifyMtFastUpdate( tSrcMt.GetMtId(), MODE_VIDEO );
	}
    else if ( TYPE_MCUPERI == tSrcMt.GetType() )
    {
		NotifyFastUpdate( tSrcMt, 0 );
		NotifyFastUpdate( tSrcMt, 1 );
		NotifyFastUpdate( tSrcMt, 2 );
    }
	else
	{
		ConfLog( FALSE, "Wrong type :tSrcMt.GetType = %d\n" ,tSrcMt.GetType());
	}
	
	if (m_tConf.GetConfAttrb().IsResendLosePack()/* && MODE_AUDIO != byMode*/)
	{
		//把目的的Rtcp交换给源
		TLogicalChannel tLogicalChannel;
        u8 bySrcChnnl = 0;
		u32 dwDstIp;
		u16 wDstPort;

        if (TYPE_MT == tDstMt.GetType())
        {
            bySrcChnnl = 0;
        }
        else if (TYPE_MCUPERI == tDstMt.GetType())
        {
            bySrcChnnl = tSwitchInfo.GetDstChlIdx();
        }
        
        if (MODE_VIDEO == byMode || MODE_BOTH == byMode)
        {
            m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);

		    dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
		    wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();

            // guzh [7/25/2007] 选看交换也要Map
            //将 MT.RTCP -> PRS 时，考虑到防火墙支撑，需将交换包的源ip、port映射为 MT.RTP，以便PRS重传                        
            SwitchVideoRtcpToDst(dwDstIp, wDstPort, tDstMt, bySrcChnnl, MODE_VIDEO, SWITCH_MODE_SELECT, TRUE);
        }
    
        if (MODE_AUDIO == byMode || MODE_BOTH == byMode)
        {
            m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE);

		    dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
		    wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();

            SwitchVideoRtcpToDst(dwDstIp, wDstPort, tDstMt, bySrcChnnl, MODE_AUDIO, SWITCH_MODE_SELECT, TRUE);
        }
	}

	return;
}

/*====================================================================
    函数名      ：SwitchSrcToDstAccord2Adp
    功能        ：将码流从源交换到目的（通过适配）
    算法实现    ：
    引用全局变量：
    输入参数说明：const TSwitchInfo &tSwitchInfo, 交换信息
	              const CServMsg & cServMsgHdr,	业务消息头
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/05/22    4.5         张宝卿         创建
====================================================================*/
BOOL32 CMcuVcInst::SwitchSrcToDstAccord2Adp(const TSwitchInfo & tSwitchInfo,
                                            const CServMsg & cServMsgHdr)
{
    if (!g_cMcuVcApp.IsSelAccord2Adp())
    {
        ConfLog(FALSE, "[SwitchSrcToDstAccord2Adp] failed due to debug not support!\n");
        return FALSE;
    }

    u8 bySrcMtId = tSwitchInfo.GetSrcMt().GetMtId();
    u8 byDstMtId = tSwitchInfo.GetDstMt().GetMtId();

    //取源终端与目的终端的能力集
    TLogicalChannel tSrcChnl;
    TLogicalChannel tDstChnl;
    m_ptMtTable->GetMtLogicChnnl(bySrcMtId, LOGCHL_VIDEO, &tSrcChnl, FALSE);
    m_ptMtTable->GetMtLogicChnnl(byDstMtId, LOGCHL_VIDEO, &tDstChnl, TRUE);

    //容错保护
    if (tSrcChnl.GetChannelType() == tDstChnl.GetChannelType() &&
        tSrcChnl.GetVideoFormat() == tDstChnl.GetVideoFormat() &&
        tSrcChnl.GetChanVidFPS() == tDstChnl.GetChanVidFPS() )
    {
        ConfLog(FALSE, "[SwitchSrcToDstAccord2Adp] Src Eq Dst<%d,%s,%d>, switch directly!\n",
                        tSrcChnl.GetChannelType(),
                        GetResStr(tSrcChnl.GetVideoFormat()),
                        tSrcChnl.GetChanVidFPS());
        SwitchSrcToDst(tSwitchInfo, cServMsgHdr);
        return TRUE;
    }

    u8 bySwitchMode = tSwitchInfo.GetMode();
    TMt tSrcMt = m_ptMtTable->GetMt(bySrcMtId);
    TMt tDstMt = m_ptMtTable->GetMt(byDstMtId);

    return StartHdVidSelAdp(tSrcMt, tDstMt);
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
                         g_cMcuVcApp.FindMp( ptSwitchChnnl->GetDstIP() ) != 0 &&
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
    
    //video广播源
    if (!m_tVidBrdSrc.IsNull())
    {
        bySrcChnnl = (m_tVidBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;
        g_cMpManager.SetSwitchBridge(m_tVidBrdSrc, bySrcChnnl, MODE_VIDEO);
    
        if ((0 != m_tConf.GetSecBitRate() || MEDIA_TYPE_NULL != m_tConf.GetCapSupport().GetSecondSimCapSet().GetVideoMediaType())
            && m_tVidBrdSrc == m_tVmpEqp)
        {
            g_cMpManager.SetSwitchBridge(m_tVidBrdSrc, bySrcChnnl+1, MODE_VIDEO);           
        }
    }        

    //audio广播源
    if (!m_tAudBrdSrc.IsNull())
    {
        bySrcChnnl = (m_tAudBrdSrc == m_tPlayEqp) ? m_byPlayChnnl : 0;            
        g_cMpManager.SetSwitchBridge(m_tAudBrdSrc, bySrcChnnl, MODE_AUDIO);
    }        

    //bas
    if (m_tConf.m_tStatus.IsAudAdapting() && EQP_CHANNO_INVALID != m_byAudBasChnnl)
    {
        g_cMpManager.SetSwitchBridge(m_tAudBasEqp, m_byAudBasChnnl, MODE_AUDIO);
    }

    if (m_tConf.m_tStatus.IsVidAdapting() && EQP_CHANNO_INVALID != m_byVidBasChnnl)
    {
        g_cMpManager.SetSwitchBridge(m_tVidBasEqp, m_byVidBasChnnl, MODE_VIDEO);
    }

    if (m_tConf.m_tStatus.IsBrAdapting() && EQP_CHANNO_INVALID != m_byBrBasChnnl)
    {
        g_cMpManager.SetSwitchBridge(m_tBrBasEqp, m_byBrBasChnnl, MODE_VIDEO);
    }

    if (m_tConf.m_tStatus.IsCasdAudAdapting() && EQP_CHANNO_INVALID != m_byCasdAudBasChnnl)
    {
        g_cMpManager.SetSwitchBridge(m_tCasdAudBasEqp, m_byCasdAudBasChnnl, MODE_AUDIO);
    }

    if (m_tConf.m_tStatus.IsCasdVidAdapting() && EQP_CHANNO_INVALID != m_byCasdVidBasChnnl)
    {
        g_cMpManager.SetSwitchBridge(m_tCasdVidBasEqp, m_byCasdVidBasChnnl, MODE_VIDEO);
    }

    //双流源
    if (!m_tDoubleStreamSrc.IsNull())
    {
        // zbq [09/04/2007] 若录像机双流放象，按对应端口建桥
        u8 bySrcChn = m_tPlayEqp == m_tDoubleStreamSrc ? m_byPlayChnnl : 0;        
        g_cMpManager.SetSwitchBridge(m_tDoubleStreamSrc, bySrcChn, MODE_SECVIDEO);
    }

    //选看源
    TMtStatus tStatus;
    TMt tTmpMt;
	for (u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
    {
        if (!m_tConfAllMtInfo.MtJoinedConf(byLoop))
        {
            continue;
        }      
		
		if (m_ptMtTable->GetMtStatus(byLoop, &tStatus))
        {
            tTmpMt = tStatus.GetSelectMt(MODE_VIDEO);
            if (!tTmpMt.IsNull() && m_tConfAllMtInfo.MtJoinedConf(tTmpMt.GetMtId()))
            {               
                g_cMpManager.SetSwitchBridge(tTmpMt, 0, MODE_VIDEO);
            }

            tTmpMt = tStatus.GetSelectMt(MODE_AUDIO);
            if (!tTmpMt.IsNull() && m_tConfAllMtInfo.MtJoinedConf(tTmpMt.GetMtId()))
            {               
                if (!m_tConf.m_tStatus.IsMixing())
                {
                    g_cMpManager.SetSwitchBridge(tTmpMt, 0, MODE_AUDIO);
                }                    
            }
        }
    }

    //回传终端
	if(m_tCascadeMMCU.GetMtId() != 0)
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
        tTmpMt = ptConfMcInfo->m_tSpyMt;
		if( ptConfMcInfo != NULL && !tTmpMt.IsNull() &&
            m_ptMtTable->GetMtSndBitrate(tTmpMt.GetMtId()) > 0 )
		{
            u8 byMode = MODE_BOTH;
            if( TYPE_MT == m_ptMtTable->GetMainType( tTmpMt.GetMtId() ) && 
		        MT_MANU_KDC != m_ptMtTable->GetManuId( tTmpMt.GetMtId() ) ) 
            {
                if (m_ptMtTable->IsMtAudioDumb(tTmpMt.GetMtId()) || 
                    m_ptMtTable->IsMtAudioMute(tTmpMt.GetMtId()))
                {
                    byMode = MODE_VIDEO;
                }
            }
            
            if (m_tConf.m_tStatus.IsMixing())
            {
                byMode = MODE_VIDEO;
            }
		        
			g_cMpManager.SetSwitchBridge(tTmpMt, 0, byMode);
		}		    
	}

    return TRUE;    
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
			OspPrintf( TRUE, FALSE, "\nMp%d(%s) switch info: ", byLoop1 , StrOfIP(g_cMcuVcApp.GetMpIpAddr(byLoop1)) );
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
						l = sprintf( achTemp, "\n%d  %s:%d(src)", wLoop2, StrOfIP( dwSrcIp ), wRcvPort, byMpId );
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

					OspPrintf( TRUE, FALSE, achTemp );
					
				}
			}
		}
	}
}

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
====================================================================*/
void CMcuVcInst::SwitchVideoRtcpToDst(u32 dwDstIp, u16 wDstPort, TMt tSrc, u8 bySrcChnnl, 
									  u8 byMode, u8 bySwitchMode, BOOL32 bMapSrcAddr)
{
    u32 dwSrcIpAddr, dwRecvIpAddr, dwOldDstIp, dwMapIpAddr;
    u16 wRecvPort, wOldDstPort, wMapPort;
    u8  byChlNum;
    u8  byChanType;

    u32 dwSwitchIpAddr;
    u16 wSwitchPort;

    TLogicalChannel tLogicalChannel;

    if (MODE_VIDEO != byMode && MODE_AUDIO != byMode)
    {
        return;
    }

    EqpLog("[SwitchVideoRtcpToDst] dwDstIp:0x%x, wDstPort:%u, tSrc<%d,%d>, bySrcChnnl:%d, byMode:%d\n",
            dwDstIp, wDstPort, tSrc.GetMcuId(), tSrc.GetMtId(), bySrcChnnl, byMode);

	wMapPort    = 0;
	dwMapIpAddr = 0;

	//得到原来的目的端口
	switch(tSrc.GetType()) 
	{
	case TYPE_MT://终端	
		m_ptMtTable->GetMtRtcpDstAddr(tSrc.GetMtId(), dwOldDstIp, wOldDstPort, byMode);
        if (MODE_VIDEO == byMode)
        {
            byChanType = LOGCHL_VIDEO;
        }
        else if (MODE_AUDIO == byMode)
        {
            byChanType = LOGCHL_AUDIO;
        }
		m_ptMtTable->GetMtLogicChnnl(tSrc.GetMtId(), byChanType, &tLogicalChannel, TRUE);
		wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
		//将 MT.RTCP -> PRS 时，考虑到防火墙支撑，需将交换包的源ip、port映射为 MT.RTP，以便PRS重传
		if (bMapSrcAddr)
		{
			wMapPort    = tLogicalChannel.GetRcvMediaChannel().GetPort();
			dwMapIpAddr = tLogicalChannel.GetRcvMediaChannel().GetIpAddr();
		}
		break;

	case TYPE_MCUPERI://外设
        g_cMcuVcApp.GetEqpRtcpDstAddr(tSrc.GetEqpId(), bySrcChnnl, dwOldDstIp, wOldDstPort, byMode);
        g_cMcuVcApp.GetPeriEqpLogicChnnl(tSrc.GetEqpId(), byMode, &byChlNum, &tLogicalChannel, TRUE);
        switch(tSrc.GetEqpType())
        {
        case EQP_TYPE_VMP:
            {       
                if (g_cMpManager.GetVmpSwitchAddr(tSrc.GetEqpId(), dwSwitchIpAddr, wSwitchPort))
                {
                    if (SWITCH_MODE_BROADCAST == bySwitchMode)
                    {
                        wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort() + PORTSPAN*bySrcChnnl;
                    }
                    else
                    {
                        //  xsl [6/3/2006] 画面合成接收码流通道，mp侧RTCP端口分配
						// xliang [5/6/2009] 分配原则 0~7	===> 2~9 
						//							  8~15	===> 12~19
						//							  16~20	===> 22~25
                        if (bySrcChnnl >= 16)
						{
							wRecvPort = wSwitchPort + 6 + bySrcChnnl;
						}
						if (bySrcChnnl >= 8)
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
                    ConfLog(FALSE, "SwitchVideoRtcpToDst - GetVmpSwitchAddr error!\n");
                }
            }
        	break;  
            
        case EQP_TYPE_BAS:
            {
                wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort() + PORTSPAN*bySrcChnnl;
            }
            break;
        default:
            wRecvPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort() + PORTSPAN*bySrcChnnl;
        	break;
        }

		break;
	}

    if (TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_TVWALL == tSrc.GetEqpType())
    {
        TPeriEqpStatus tTWStatus;
        if (g_cMcuVcApp.GetPeriEqpStatus(tSrc.GetEqpId(), &tTWStatus))
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

	if (TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_HDU == tSrc.GetEqpType())
    {
        TPeriEqpStatus tHduStatus;
        if (g_cMcuVcApp.GetPeriEqpStatus(tSrc.GetEqpId(), &tHduStatus))
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
    if (0 != dwOldDstIp)
    {
        g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRecvIpAddr, wRecvPort, dwOldDstIp, wOldDstPort);
    }

    //添加交换
    g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwSrcIpAddr, 0, dwRecvIpAddr,wRecvPort, 
		                             dwDstIp, wDstPort, 0, 0, dwMapIpAddr, wMapPort);

    //记录交换目的
    switch(tSrc.GetType()) 
    {
    case TYPE_MT://终端	
        m_ptMtTable->SetMtRtcpDstAddr(tSrc.GetMtId(), dwDstIp, wDstPort, byMode);
        break;

    case TYPE_MCUPERI://外设
        g_cMcuVcApp.SetEqpRtcpDstAddr(tSrc.GetEqpId(), bySrcChnnl, dwDstIp, wDstPort, byMode);
        break;
    }
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

	u8 byMode = MODE_VIDEO;
	cServMsg.SetMsgBody(&byMode, sizeof(u8));
	SendMsgToMt( byMtId, MCU_MT_FASTUPDATEPIC_CMD, cServMsg );
	m_ptMtTable->SetLastTick( byMtId, dwCurTick );
	
    if (MT_MANU_TAIDE == m_ptMtTable->GetManuId(byMtId))
    {
        if( byCount < 1 )
        {
            SetTimer( pcMsg->event, 3600, 100*byMtId + byCount + 1  );
            CallLog("[ProcMcuMtFastUpdateTimer] mt.%d fastupdate has been adjust to once more due to Tandberg!\n", byMtId);
        }
    }
    else
    {
	    if( byCount < 3 )
	    {
		    SetTimer( pcMsg->event, 1200, 100*byMtId + byCount + 1  );
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

    CallLog("[ProcMcuEqpFastUpdateTimer] the Eqp.%u, chn.%u, byCount.%d!\n", byEqpId, byChnl, byCount);

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
            
            break;
        }
        
    case EQP_TYPE_VMPTW:
        wEvent = MCU_VMPTW_FASTUPDATEPIC_CMD;
        dwLaskTick = m_dwVmpTwLastVCUTick;
        break;
        
    case EQP_TYPE_BAS:
        
        if ( !g_cMcuAgent.IsEqpBasHD( byEqpId ) )
        {
            cServMsg.SetChnIndex(byChnl);
            wEvent = MCU_BAS_FASTUPDATEPIC_CMD;
            SetBasLastVCUTick( byChnl, dwCurTick );
        }
        else
        {
            cServMsg.SetChnIndex(byChnl);
            wEvent = MCU_BAS_FASTUPDATEPIC_CMD;
            m_cBasMgr.SetChnVcuTick( tDstEqp, byChnl, dwCurTick );
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


// END OF FILE
