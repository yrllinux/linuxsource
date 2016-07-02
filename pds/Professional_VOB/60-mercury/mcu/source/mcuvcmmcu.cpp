/*****************************************************************************
   模块名      : mcu
   文件名      : mcuvcmmcu.cpp
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
#include "rpctrl.h"
//#include "mcuerrcode.h"

//消息体:TMtAlias(mt别名)+TMtAlias(mt e164)+TMtAlias(mt ip)+TMtAlias(本MCU的会议名)+
//byEncrypt(1-加密,0-不加密)+TCreateConfParams
/*====================================================================
    函数名      ：DaemonProcMtMcuCreateConfReq
    功能        ：处理终端的创建会议请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/08/17    3.0         胡昌威         创建
====================================================================*/
void CMcuVcInst::DaemonProcMtMcuCreateConfReq( const CMessage * pcMsg )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );	

    //xsl [11/28/2006] N+1备份模式下不允许创建及呼叫会议
    if (g_cNPlusApp.GetLocalNPlusState() == MCU_NPLUS_SLAVE_IDLE)
    {
        ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] mt can't create conf in n+1 mode!\n");
		g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
        return;
    }

	TMtAlias tMtH323Alias = *(TMtAlias*)cServMsg.GetMsgBody();
	TMtAlias tMtE164Alias = *(((TMtAlias *)cServMsg.GetMsgBody())+1);
	TMtAlias tMtAddr    = *(((TMtAlias *)cServMsg.GetMsgBody())+2);
	TMtAlias tConfAlias = *(((TMtAlias *)cServMsg.GetMsgBody())+3);
	u8       byType     = *(cServMsg.GetMsgBody()+4*sizeof(TMtAlias));
	u8       byEncrypt  = *(cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+sizeof(u8));
	u16      wCallRate  = *(u16*)((cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+sizeof(u8)+sizeof(u8)));
	u8       byVendorId = *(u8*)((cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+sizeof(u8)+sizeof(u8)+sizeof(u16)));
    u16      wConfFirstRate = 0;
    u16      wConfSecondRate = 0;

    BOOL32   bLowLevelMcuCalledIn = ( pcMsg->event == MT_MCU_LOWERCALLEDIN_NTF );
	
	TConfInfo *ptConfInfo = NULL;
	if (cServMsg.GetMsgBodyLen() > 4*sizeof(TMtAlias)+3*sizeof(u8)+sizeof(u16))
	{
		ptConfInfo = (TConfInfo *)(cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+3*sizeof(u8)+sizeof(u16));
	}
	
	m_tConfEx.Clear();

	//TConfInfo之后数据块格式：
    //u8（TConfInfo后追加数据块个数） + u16（第一块长度）+ u8（AduioType）+ u8（声道数）+ ... ...(依次顺序追加其他数据块)
	if(ptConfInfo != NULL 
		&& ptConfInfo->HasConfExInfo()
		&& cServMsg.GetMsgBodyLen() > 4*sizeof(TMtAlias)+3*sizeof(u8)+sizeof(u16)+sizeof(TConfInfo))
	{
		u8 byAudioType = *(cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+3*sizeof(u8)+sizeof(u16)+sizeof(TConfInfo)+sizeof(u8)+sizeof(u16));
		u8 byAudioChnnNum = *(cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+3*sizeof(u8)+sizeof(u16)+sizeof(TConfInfo)+sizeof(u8)+sizeof(u16)+sizeof(u8));

		TCapSupport tCap = ptConfInfo->GetCapSupport();
		TSimCapSet tSim = tCap.GetMainSimCapSet();
		tSim.SetAudioMediaType(byAudioType);
		tCap.SetMainSimCapSet(tSim);
		ptConfInfo->SetCapSupport(tCap);

		m_tConfEx.Clear();
		TAudioTypeDesc tAudioType(byAudioType,byAudioChnnNum);
		//m_tConfEx.SetMainAudioTypeDesc(tAudioType);
		m_tConfEx.SetAudioTypeDesc(&tAudioType,1);
	}

	cServMsg.SetSrcMtId(CONF_CREATE_MT);
    if (bLowLevelMcuCalledIn)
    {
        // guzh [6/19/2007] 标记为下级MCU
        cServMsg.SetSrcMtId(CONF_CREATE_SMCU);
    }

    TLocalInfo tLocalInfo;
    g_cMcuAgent.GetLocalInfo(&tLocalInfo);

	const u8 byE164NumLen = strlen(tLocalInfo.GetE164Num());
	const u8 byConfAliasLen = strlen(tConfAlias.m_achAlias);
    u8 byCmpLen = max(byE164NumLen, byConfAliasLen);
    
    // zbq [06/23/2007] 终端创会: 1、呼会议E164入会 或 呼模板E164通过模板创会；2、呼MCU别名/E164 创会或入会
    if ( mtAliasTypeE164 == tConfAlias.m_AliasType &&
         0 != memcmp( tLocalInfo.GetE164Num(), tConfAlias.m_achAlias, byCmpLen ) )
    {
        u8 byTemplateConfIdx = g_cMcuVcApp.GetTemConfIdxByE164(tConfAlias.m_achAlias);
	    u8 byOnGoingConfIdx  = g_cMcuVcApp.GetOngoingConfIdxByE164(tConfAlias.m_achAlias);

        ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] byTemplateConfIdx.%d byOnGoingConfIdx.%d\n", 
            byTemplateConfIdx, byOnGoingConfIdx);
  
	    if (byOnGoingConfIdx > 0)  //终端呼叫会议
	    {
			// vcs会议/mcs会议不能互连
			CConfId cConfId = g_cMcuVcApp.GetConfIdByE164(tConfAlias.m_achAlias, FALSE , TRUE);
			if( byType != TYPE_MT && cConfId.GetConfSource() != cServMsg.GetConfId().GetConfSource() 
				&& byVendorId == MT_MANU_KDCMCU)
			{
				cServMsg.SetErrorCode( ERR_MCU_CALLMCUERROR_CONFISHOLDING );
				g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "VcsConf And McsConf Don't Connect!\n" );
				return;
			}

			
			CMcuVcInst *pInstance = g_cMcuVcApp.GetConfInstHandle(byOnGoingConfIdx);

			if( TYPE_MCU == byType && pInstance != NULL && pInstance->IsInOtherConf() //&&
				/*MCS_CONF == pInstance->m_tConf.GetConfSource()*/ )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] conf not support Cascade and in other conf!!!\n");
				cServMsg.SetErrorCode( ERR_MCU_CASADEBYOTHERHIGHLEVELMCU );
				g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
				return;
			}
		
			if (!bLowLevelMcuCalledIn)
			{
				//CConfId cConfId = g_cMcuVcApp.GetConfIdByE164(tConfAlias.m_achAlias, FALSE);
						
				//zjj20100113已经被高行政级别级联的会议不能再被级联
				//zjj20110316vcs会议同样处理
				if( TYPE_MCU == byType && pInstance != NULL && !pInstance->m_tCascadeMMCU.IsNull() //&&
					/*MCS_CONF == pInstance->m_tConf.GetConfSource()*/ )
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] conf has Cascade by a high adminLevel mcu, so can't Cascade by other mcu!!!\n");
					cServMsg.SetErrorCode( ERR_MCU_CASADEBYOTHERHIGHLEVELMCU );
					g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
					return;
				}
				
				
				

				//把终端加入会议。里面会再做准入验证
				cServMsg.SetConfIdx(byOnGoingConfIdx);
				cServMsg.SetMsgBody((u8*)&tMtH323Alias, sizeof(tMtH323Alias));
				cServMsg.CatMsgBody((u8*)&tMtE164Alias, sizeof(tMtE164Alias));
				cServMsg.CatMsgBody((u8*)&tMtAddr, sizeof(tMtAddr ));
				cServMsg.CatMsgBody((u8*)&tConfAlias, sizeof(tConfAlias));
				cServMsg.CatMsgBody(&byType, sizeof(byType));
				cServMsg.CatMsgBody(&byEncrypt, sizeof(byEncrypt));
				cServMsg.CatMsgBody((u8*)&wCallRate, sizeof(wCallRate));
				g_cMcuVcApp.SendMsgToConf( byOnGoingConfIdx, MT_MCU_MTJOINCONF_REQ, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );



				// 对于VCS级联会议，若下级调度席已被调度，不允许进行级联
				if (!cConfId.IsNull() && VCS_CONF == cConfId.GetConfSource() && TYPE_MCU == byType)
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] subconf has been created by other vcs, restore conf\n");
	// 				cServMsg.SetErrorCode(ERR_MCU_VCS_SMCUINVC);
	//              g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
	// 			    return;
					g_cMcuVcApp.SendMsgToConf( byOnGoingConfIdx, MCUVC_MMCUGETCTRL_CMD, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
				}
		  
			}
			else
			{
				TConfInfo    *ptTempConfInfo = NULL;
				TConfMtTable *ptConfMtTable = NULL;
				TConfAllMtInfo *ptConfAllMtInfo = NULL;
				
				//zbq[08/23/2008] 级联呼叫受邀权限界定
				if (NULL == pInstance/*g_cMcuVcApp.GetConfInstHandle(byOnGoingConfIdx)*/)
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] get handle of conf.%d failed\n", byOnGoingConfIdx);
					return;
				}
				
				ptTempConfInfo = &pInstance->m_tConf;//&g_cMcuVcApp.GetConfInstHandle(byOnGoingConfIdx)->m_tConf;
				ptConfMtTable = g_cMcuVcApp.GetConfMtTable(byOnGoingConfIdx);
				ptConfAllMtInfo = &pInstance->m_tConfAllMtInfo;//&g_cMcuVcApp.GetConfInstHandle(byOnGoingConfIdx)->m_tConfAllMtInfo;
				
				if( byEncrypt == 0 && 
					ptTempConfInfo->GetConfAttrb().GetEncryptMode() != CONF_ENCRYPTMODE_NONE )
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] Mt 0x%x join conf %s request was refused because encrypt!\n", 
						tMtAddr.m_tTransportAddr.GetIpAddr(), ptTempConfInfo->GetConfName() );
					return;
					
				}
				
				u8 byMtId = 0;
				//封闭会议, 且其不是受邀终端, 拒绝
				if(ptTempConfInfo->GetConfAttrb().GetOpenMode() == CONF_OPENMODE_CLOSED )
				{
					byMtId = ptConfMtTable->GetMtIdByAlias( &tMtAddr );
					if( !( byMtId > 0 && ptConfAllMtInfo->MtInConf( byMtId ) ) )
					{
						ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] Mt 0x%x join conf %s request was refused because conf is closed!\n", 
							tMtAddr.m_tTransportAddr.GetIpAddr(), ptTempConfInfo->GetConfName() );
						return;
					}
				}
				
				// guzh [6/19/2007] 下级MCU主动呼叫，作为本级下级MCU主动添加
				TAddMtInfo tAddMtInfo;
				cServMsg.SetConfIdx(byOnGoingConfIdx);            
				cServMsg.SetConfId(g_cMcuVcApp.GetConfId(byOnGoingConfIdx));
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
				tAddMtInfo.SetCallBitRate(wCallRate);//mtadp发送消息实际以主机序发送该值[1/16/2013 chendaiwei]
				TMcu tLocalMcu;
				tLocalMcu.SetMcu(LOCAL_MCUID);
				cServMsg.SetMsgBody((u8*)&tLocalMcu, sizeof(TMcu));
				cServMsg.CatMsgBody((u8*)&tAddMtInfo, sizeof(TAddMtInfo));
				// xliang [8/29/2008] 
				cServMsg.CatMsgBody((u8*)&bLowLevelMcuCalledIn,sizeof(BOOL32));
				g_cMcuVcApp.SendMsgToConf( byOnGoingConfIdx, MCS_MCU_ADDMT_REQ, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			}
  
        }
	    else if (byTemplateConfIdx > 0)  //呼叫会议模板,创建会议
	    {
			// vcs会议/mcs会议不能互连
			CConfId cConfId = g_cMcuVcApp.GetConfIdByE164(tConfAlias.m_achAlias, TRUE,FALSE);
			if( byType != TYPE_MT && cConfId.GetConfSource() != cServMsg.GetConfId().GetConfSource()
				&& byVendorId == MT_MANU_KDCMCU)
			{
				cServMsg.SetErrorCode( ERR_MCU_CALLMCUERROR_CONFISHOLDING );
				g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "VcsConf And McsConf Don't Connect!\n" );
				return;
			}

            //让会议开始
		    cServMsg.SetConfIdx(byTemplateConfIdx);
		    TTemplateInfo tTemConf;
		    if (!g_cMcuVcApp.GetTemplate(byTemplateConfIdx, tTemConf))
		    {
                ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] Get Template %d failed!\n", byTemplateConfIdx);
                g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			    return;
		    }
			
			//vcs会议拒绝终端创会
			if (TYPE_MT == byType && VCS_CONF == tTemConf.m_tConfInfo.GetConfSource())
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] VCSConf do not support Mt Creat Conf!\n");
                g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
				return;
			}

		    //码流加密
		    if (0 == byEncrypt && 
			    CONF_ENCRYPTMODE_NONE != tTemConf.m_tConfInfo.GetConfAttrb().GetEncryptMode())
		    {
				cServMsg.SetErrorCode( ERR_MCU_SETENCRYPT );
                ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] Template encrypt!\n");
			    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			    return;
		    }

            //  xsl [2/13/2006] 在CreateConf处理中添加终端成功后回ack
            //SendReplyBack(cServMsg, cServMsg.GetEventId()+1);

		    tTemConf.m_tConfInfo.m_tStatus.SetOngoing();
		    tTemConf.m_tConfInfo.SetStartTime( time(NULL) );    
		    tTemConf.m_tConfInfo.m_tStatus.SetProtectMode( CONF_LOCKMODE_NONE );

			//////////////////////////////////////////////////////////////////////////			
			//呼叫模板失败首先要上报上级mcu错误，同时上报本级mcu错误到mcs，两个错误码不同，在界面上显示不同信息
			if( TYPE_MCU == byType && MCS_CONF == tTemConf.m_tConfInfo.GetConfSource() &&
				g_cMcuVcApp.IsConfNumOverCap( TRUE, tTemConf.m_tConfInfo.GetConfSource() )
				)
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL,  "[DaemonProcMtMcuCreateConfReq] mcs on-going conf num has passed max(%d)\n",
							g_cMcuVcApp.GetMaxMcsOngoingConfNum() );
				cServMsg.SetErrorCode( ERR_MCU_CALLMCUERROR_CONFISHOLDING );
			    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);

				//cServMsg.SetErrorCode( ERR_MCU_MMCUCALLREFUSE );
				//SendMsgToAllMcs(  MCU_MCS_CREATECONF_BYTEMPLATE_NACK ,cServMsg );
				NotifyMcsAlarmInfo( 0,ERR_MCU_MMCUCALLREFUSE );
				return;
			}
			//////////////////////////////////////////////////////////////////////////

		    wCallRate -= GetAudioBitrate( tTemConf.m_tConfInfo.GetMainAudioMediaType() );

		    TMtAlias atMtAlias[MAXNUM_CONF_MT+1];
            u16      awMtDialBitRate[MAXNUM_CONF_MT+1] = {0};
		    u8       byMtNumInUse = 0;
		    
            // zbq [09/19/2007] 呼模板创会，保持该终端在原模板中的呼叫模式不变; 若原为IP,注意保护端口.
		    //atMtAlias[0] = tMtAddr;
			//zjj20100114 如果需要注册gk，就呼下级的164号			
			if( g_cMcuAgent.GetGkIpAddr() != 0 )
			{				
				if( mtAliasTypeE164 == tMtE164Alias.m_AliasType &&
						0 != strcmp( tMtE164Alias.m_achAlias,"" )
						)
				{
					atMtAlias[0] = tMtE164Alias;
				}
				else if( mtAliasTypeH323ID == tMtH323Alias.m_AliasType &&
					0 != strcmp( tMtH323Alias.m_achAlias,"" )
					)
				{
					atMtAlias[0] = tMtH323Alias;
				}
				else
				{
					atMtAlias[0] = tMtAddr;
				}			
			}
			else
			{
				atMtAlias[0] = tMtAddr;				
			}
		    awMtDialBitRate[0] = wCallRate;
		    byMtNumInUse = 1;

		    // 刷新终端列表，电视墙和画面合成模板
            TMultiTvWallModule tNewTvwallModule = tTemConf.m_tMultiTvWallModule;
            TVmpModule         tNewVmpModule    = tTemConf.m_atVmpModule;

			TConfInfoEx tConfInfoEx;
			THduVmpModuleOrTHDTvWall tHduVmpModule;
			TVmpModuleInfo tVmpModuleEx25;
			u16 wPackConfExInfoLength = 0;
			BOOL32 bUnkownConfInfo = FALSE;
			UnPackConfInfoEx(tConfInfoEx,tTemConf.m_byConInfoExBuf, wPackConfExInfoLength,
						bUnkownConfInfo, &tHduVmpModule, &tVmpModuleEx25);
            
		    BOOL32 bInMtTable = FALSE;  
            BOOL32 bRepeatThisLoop;
		    for (u8 byLoop = 0; byLoop < tTemConf.m_byMtNum; byLoop++)
		    {
                bRepeatThisLoop = FALSE;
                //如果与建会终端一致，则不重复添加 (bRepeatThisLoop = TRUE)
			    if (mtAliasTypeH320ID == tTemConf.m_atMtAlias[byLoop].m_AliasType)
			    {
				    if (0 == memcmp(tTemConf.m_atMtAlias[byLoop].m_achAlias, tMtH323Alias.m_achAlias, MAXLEN_ALIAS))
				    {
					    bInMtTable = TRUE;
                        bRepeatThisLoop = TRUE;
                        
                        // zbq [09/19/2007] 取本来的呼叫模式
                        atMtAlias[0] = tMtH323Alias;
				    }			    
			    }
			    else if (mtAliasTypeTransportAddress == tTemConf.m_atMtAlias[byLoop].m_AliasType)
			    {
				    if (tTemConf.m_atMtAlias[byLoop].m_tTransportAddr.m_dwIpAddr == tMtAddr.m_tTransportAddr.m_dwIpAddr //&& 
                        // zbq [09/19/2007] 新策略不再比较端口
					    // tTemConf.m_atMtAlias[byLoop].m_tTransportAddr.m_wPort == tMtAddr.m_tTransportAddr.m_wPort
                        )
				    {
					    bInMtTable = TRUE;
                        bRepeatThisLoop = TRUE;

                        // zbq [09/19/2007] 取本来的呼叫模式, 并保护端口
                        if ( tMtAddr.m_tTransportAddr.GetPort() != tTemConf.m_atMtAlias[byLoop].m_tTransportAddr.GetPort() )
                        {
                            tMtAddr.m_tTransportAddr.SetPort(tTemConf.m_atMtAlias[byLoop].m_tTransportAddr.GetPort());
                        }
                        atMtAlias[0] = tMtAddr;
				    }
			    }
				else if (mtAliasTypeE164 == tTemConf.m_atMtAlias[byLoop].m_AliasType)
				{
					if (0 == memcmp(tTemConf.m_atMtAlias[byLoop].m_achAlias, tMtE164Alias.m_achAlias, MAXLEN_ALIAS))
					{
						bInMtTable = TRUE;
						bRepeatThisLoop = TRUE;

						// zbq [09/19/2007] 取本来的呼叫模式
						atMtAlias[0] = tMtE164Alias;
					}
				}
				else if (mtAliasTypeH323ID == tTemConf.m_atMtAlias[byLoop].m_AliasType)
				{
					if (0 == memcmp(tTemConf.m_atMtAlias[byLoop].m_achAlias, tMtH323Alias.m_achAlias, MAXLEN_ALIAS))
					{
						bInMtTable = TRUE;
						bRepeatThisLoop = TRUE;

						// zbq [09/19/2007] 取本来的呼叫模式
						atMtAlias[0] = tMtH323Alias;
					}
				}
				else if (puAliasTypeIPPlusAlias == tTemConf.m_atMtAlias[byLoop].m_AliasType)
				{
					u32 dwNetIpAddr = *(u32*)tTemConf.m_atMtAlias[byLoop].m_achAlias; //网络序
					TMtAlias tTmpAlias;
					memcpy(tTmpAlias.m_achAlias,&tTemConf.m_atMtAlias[byLoop].m_achAlias[sizeof(dwNetIpAddr)],strlen(tTemConf.m_atMtAlias[byLoop].m_achAlias)-sizeof(dwNetIpAddr));
					
					if(tMtAddr.m_tTransportAddr.m_dwIpAddr == dwNetIpAddr 
						&& (strcmp(tMtH323Alias.m_achAlias,tTmpAlias.m_achAlias) == 0
						|| strcmp(tMtE164Alias.m_achAlias,tTmpAlias.m_achAlias) == 0)
						)
					{
						bInMtTable = TRUE;
						bRepeatThisLoop = TRUE;
						atMtAlias[0] = tTemConf.m_atMtAlias[byLoop];
					}
				}

                if (!bRepeatThisLoop)
                {
                    atMtAlias[byMtNumInUse] = tTemConf.m_atMtAlias[byLoop];
                    awMtDialBitRate[byMtNumInUse] = tTemConf.m_awMtDialBitRate[byLoop];
                    byMtNumInUse++;
                }	

                //每次循环添加一个终端别名后，要把它原来在画面合成和电视墙里面的下标更新，
                //这有两种情况，可能是与创会者相同，则现在的下标是1，不同，则下标是当前的终端末尾
                UpdateConfStoreModule(tTemConf, 
                                      byLoop+1, bRepeatThisLoop ? 1 : byMtNumInUse,
                                      tNewTvwallModule, tNewVmpModule,tHduVmpModule,tVmpModuleEx25);
		    }
            
		    //会议开放模式
		    //错误的开放模式
		    if ( CONF_OPENMODE_CLOSED  != tTemConf.m_tConfInfo.GetConfAttrb().GetOpenMode() &&
			     CONF_OPENMODE_NEEDPWD != tTemConf.m_tConfInfo.GetConfAttrb().GetOpenMode() &&
			     CONF_OPENMODE_OPEN    != tTemConf.m_tConfInfo.GetConfAttrb().GetOpenMode() ) 
		    {
                ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] Wrong Open Mode!\n");
			    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			    return;
		    }
		    //不公开
		    if ( CONF_OPENMODE_CLOSED == tTemConf.m_tConfInfo.GetConfAttrb().GetOpenMode() )
		    {
                ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] Open Mode is CLOSE!\n");
			    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			    return;
		    }
			// xliang [12/26/2008] (modify for MT calling MCU initially) 
			//cancel limit here, but need to input password later
		    //会议根据密码加入
// 		    else if ( CONF_OPENMODE_NEEDPWD == tTemConf.m_tConfInfo.GetConfAttrb().GetOpenMode() &&
// 				      !bInMtTable )
// 		    {
//                 ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] Conf Need Password!\n");
// 			    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
// 			    return;
// 		    }
            // 不在受邀列表且不在允许地址段
//             if (!bInMtTable && 
//                 (byType != TYPE_MCU) &&   // MCU 则不考虑IP是否允许段
//                 !g_cMcuVcApp.IsMtIpInAllowSeg( tTemConf.m_tConfInfo.GetUsrGrpId(), tMtAddr.m_tTransportAddr.GetNetSeqIpAddr()))
//             {
//                 ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] Type is not MCU(type=%d), neither in Allow IP range(0x%x)!\n", 
//                         byType, 
//                         tMtAddr.m_tTransportAddr.GetIpAddr());
// 			    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
// 			    return;
//             }
        		    
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


			//对于模板中的auto作假分辨率，转换为真实启用的分辨率cif
			if (VIDEO_FORMAT_AUTO == tTemConf.m_tConfInfo.GetMainVideoFormat() &&
				MEDIA_TYPE_H264   == tTemConf.m_tConfInfo.GetMainVideoMediaType())
			{
				tTemConf.m_tConfInfo.SetMainVideoFormat(VIDEO_FORMAT_CIF);
			}

			if (VIDEO_FORMAT_AUTO == tTemConf.m_tConfInfo.GetSecVideoFormat() &&
				MEDIA_TYPE_H264   == tTemConf.m_tConfInfo.GetSecVideoMediaType())
			{
				tTemConf.m_tConfInfo.SetSecVideoFormat(VIDEO_FORMAT_CIF);
			}
		    
		    cServMsg.SetMsgBody((u8 *)&tTemConf.m_tConfInfo, sizeof(TConfInfo));
		    cServMsg.CatMsgBody((u8 *)&wAliasBufLen, sizeof(wAliasBufLen));
		    cServMsg.CatMsgBody((u8 *)achAliasBuf, ntohs(wAliasBufLen));
			TConfAttrb tConfAttrb = tTemConf.m_tConfInfo.GetConfAttrb();
		    if (tConfAttrb.IsHasTvWallModule())
		    {
			    cServMsg.CatMsgBody((u8*)&tNewTvwallModule, sizeof(TMultiTvWallModule));
		    }
		    if (tConfAttrb.IsHasVmpModule())
		    {
			    cServMsg.CatMsgBody((u8*)&tNewVmpModule, sizeof(TVmpModule));
		    }

			// 对于VCS会议需要高清电视墙及下级mcu
			if (VCS_CONF == tTemConf.m_tConfInfo.GetConfSource())
			{
				u8 byIsHDTWCfg = (u8)tTemConf.IsHDTWCfg();
				cServMsg.CatMsgBody(&byIsHDTWCfg, sizeof(u8));
				// 配置了高清电视墙
				if (byIsHDTWCfg)
				{
					cServMsg.CatMsgBody((u8*)&tTemConf.m_tHDTWInfo, sizeof(THDTvWall));
				}

				//下级hdunum默认设置为0
				u8 byHduChnlNum = 0;
				cServMsg.CatMsgBody(&byHduChnlNum, sizeof(u8));
		
				// 配置了下级mcu
				u8 byIsVCSSMCUCfg = (u8)tTemConf.IsVCSMCUCfg();
				cServMsg.CatMsgBody(&byIsVCSSMCUCfg, sizeof(u8));
				if (byIsVCSSMCUCfg)
				{
					PackTMtAliasArray(tTemConf.m_tVCSSMCUCfg.m_atSMCUAlias, tTemConf.m_tVCSSMCUCfg.m_awSMCUDialBitRate,
									  tTemConf.m_tVCSSMCUCfg.m_wSMCUNum, achAliasBuf, wAliasBufLen);
					wAliasBufLen = htons(wAliasBufLen);
					cServMsg.CatMsgBody((u8*)&wAliasBufLen, sizeof(u16));
					cServMsg.CatMsgBody((u8*)achAliasBuf, ntohs(wAliasBufLen));
				}
				// 配置了分组		
				// 分组信息不由模板信息中给出
				tTemConf.m_byMTPackExist = FALSE;
				cServMsg.CatMsgBody((u8*)&tTemConf.m_byMTPackExist, sizeof(u8));
				cServMsg.CatMsgBody((u8*)&tTemConf.m_tVCSBackupChairMan, sizeof(TMtAlias));
				// [4/20/2011 xliang] VCAutoMode
				cServMsg.CatMsgBody((u8*)&tTemConf.m_byVCAutoMode, sizeof(u8));
			}
			
			//将调整后的HDUvmp模板和后5风格vmp模板打包[5/30/2013 chendaiwei]
			u16 wConfInfoExLength = 0;
			PackConfInfoEx(tConfInfoEx, &tTemConf.m_byConInfoExBuf[0],
				wConfInfoExLength,&tHduVmpModule,&tVmpModuleEx25);
			if( wConfInfoExLength > CONFINFO_EX_BUFFER_LENGTH )
			{
				ConfPrint(LOG_LVL_WARNING,MID_MCU_CONF,"[DaemonProcMtMcuCreateConfReq] conf.%s ConfInfEx.%s > CONFINFO_EX_BUFFER_LENGTH, error!\n",tTemConf.m_tConfInfo.GetConfName(),wConfInfoExLength);
				
				return;
			}
			else
			{
				cServMsg.CatMsgBody((u8*)&tTemConf.m_byConInfoExBuf[0],wConfInfoExLength);
			}

		    cServMsg.SetConfIdx(byTemplateConfIdx);
		    cServMsg.CatMsgBody((u8*)&tMtH323Alias, sizeof(tMtH323Alias));
		    cServMsg.CatMsgBody((u8*)&tMtE164Alias, sizeof(tMtE164Alias));
		    cServMsg.CatMsgBody((u8*)&tMtAddr, sizeof(tMtAddr));
		    cServMsg.CatMsgBody(&byType, sizeof(byType));
			// xliang [12/26/2008] (modify for MT call MCU initially) add bInMtTalbe Info
			cServMsg.CatMsgBody((u8*)&bInMtTable,sizeof(bInMtTable));

			

            u8 byInsID = AssignIdleConfInsID();
            if(0 != byInsID)
            {
				if (VCS_CONF == tTemConf.m_tConfInfo.GetConfSource())
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] VCSConf start command to inst%d\n",
								   byInsID);
					post(MAKEIID( AID_MCU_VC, byInsID ), MCU_SCHEDULE_VCSCONF_START, 
						 cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
				}
				else
				{
	                post(MAKEIID( AID_MCU_VC, byInsID ), MCU_SCHEDULE_CONF_START, 
						 cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
           
				}
            }
            else
            {
                ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] assign instance id failed!\n");
            }
        }
        else
        {
            cServMsg.SetErrorCode( ERR_MCU_NULLCID );
            g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
            ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] UNEXIST E164.%s to be joined or created !\n", tConfAlias.m_achAlias);
        }
        
        return;
    }

    if (NULL != ptConfInfo)  //创建会议
	{		
		s8  achAliasBuf[SERV_MSG_LEN];
		u16 wAliasBufLen = 0;			

		if ((0 == byEncrypt && 
			 CONF_ENCRYPTMODE_NONE != ptConfInfo->GetConfAttrb().GetEncryptMode()) || 
			(0 != byEncrypt && 
			 CONF_ENCRYPTMODE_NONE == ptConfInfo->GetConfAttrb().GetEncryptMode()))
		{
			cServMsg.SetErrorCode( ERR_MCU_SETENCRYPT );
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "Conference %s failure because encrypt setting is confused - byEncrypt.%d EncryptMode.%d!\n", 
					        ptConfInfo->GetConfName(), byEncrypt, ptConfInfo->GetConfAttrb().GetEncryptMode() );
			return;
		}

		ptConfInfo->m_tStatus.SetOngoing();
		ptConfInfo->SetStartTime( time( NULL ) );
		ptConfInfo->SetStartTime( 0 );
		ptConfInfo->m_tStatus.SetProtectMode( CONF_LOCKMODE_NONE );

		wCallRate -= GetAudioBitrate( ptConfInfo->GetMainAudioMediaType() );
		
        //zbq [09/19/2007] 终端创会 别名记录调整: MCU当前注册了GK,依次按优先级取E164/H323ID/IP；否则,只取IP.若取IP,须修正端口.
        TMtAlias tMtCreateAlias;
        if ( g_cMcuVcApp.GetConfRegState(0) )
        {
            if (!tMtE164Alias.IsNull())
            {
                tMtCreateAlias = tMtE164Alias;
            }
            else if (!tMtH323Alias.IsNull())
            {
                tMtCreateAlias = tMtH323Alias;
            }
            else if (!tMtAddr.IsNull())
            {
                tMtCreateAlias = tMtAddr;
                if ( MCU_Q931_PORT != tMtCreateAlias.m_tTransportAddr.GetPort())
                {
                    tMtCreateAlias.m_tTransportAddr.SetPort(MCU_Q931_PORT);
                }
            }
            else
            {
                g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
                ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "Conference %s failure because create mt has no Alias(GK exist), ignore it!\n", ptConfInfo->GetConfName());
                return;                
            }
        }
        else
        {
            if (!tMtAddr.IsNull())
            {
                tMtCreateAlias = tMtAddr;
                if ( MCU_Q931_PORT != tMtCreateAlias.m_tTransportAddr.GetPort())
                {
                    tMtCreateAlias.m_tTransportAddr.SetPort(MCU_Q931_PORT);
                }
            }
            else
            {
                g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
                ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "Conference %s failure because create mt has no Alias, ignore it!\n", ptConfInfo->GetConfName());
                return;                
            }
        }
        
		// vcs会议/mcs会议不能互连
		if( byType != TYPE_MT && ptConfInfo->GetConfId().GetConfSource() != cServMsg.GetConfId().GetConfSource()
			&& byVendorId == MT_MANU_KDCMCU)
		{
			cServMsg.SetErrorCode( ERR_MCU_CALLMCUERROR_CONFISHOLDING );
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "VcsConf And McsConf Don't Connect!\n" );
			return;
		}

		//终端别名数组打包 
		// xliang [1/16/2009] 打包是否要带上会议模板信息中的其他终端----不需
		PackTMtAliasArray(&tMtCreateAlias, &wCallRate, 1, achAliasBuf, wAliasBufLen);
		wAliasBufLen = htons(wAliasBufLen);

		TConfInfo tTmpConfInfo;
		memcpy(&tTmpConfInfo,ptConfInfo,sizeof(TConfInfo));

		//老终端bug,非264格式帧率为0,现在非264都是25帧的,保护一下
		if( tTmpConfInfo.GetMainSimCapSet().GetVideoMediaType() != MEDIA_TYPE_H264 &&
			tTmpConfInfo.GetMainSimCapSet().GetVideoFrameRate() == 0 )
		{			
			//非264的25帧为2，具体见mtadputils.cpp中FrameRateIn2Out函数,宏VIDEO_FPS_25的定义
			tTmpConfInfo.SetMainVidFrameRate(2);
		}
		TVmpModule tTmpVmpModule;
		//兼容老终端带画面合成创会，但vmp信息填写在vmpparam中，而非vmpmodule中[1/24/2013 chendaiwei]
		if(!ptConfInfo->GetConfAttrb().IsHasVmpModule() 
			&& ptConfInfo->m_tStatus.GetVmpParam().GetVMPMode() == CONF_VMPMODE_AUTO)
		{
			TVMPParam tTmpVmpParam;
			memset( &tTmpVmpParam, 0 ,sizeof(tTmpVmpParam) );
			tTmpConfInfo.m_tStatus.SetVmpParam(tTmpVmpParam);//clear
			
			tTmpVmpParam.SetVMPAuto(TRUE);
			tTmpVmpParam.SetVMPBrdst(TRUE);
			tTmpVmpModule.SetVmpParam(tTmpVmpParam);
			tTmpConfInfo.SetHasVmpModule(TRUE);
		}
		
		cServMsg.SetMsgBody((u8 *)&tTmpConfInfo, sizeof(TConfInfo));
		cServMsg.CatMsgBody((u8*)&wAliasBufLen, sizeof(wAliasBufLen));
		cServMsg.CatMsgBody((u8*)achAliasBuf, ntohs(wAliasBufLen));
		//处理画面合成模板 
		if( tTmpConfInfo.GetConfAttrb().IsHasVmpModule())
		{
			cServMsg.CatMsgBody((u8*)&tTmpVmpModule, sizeof(tTmpVmpModule));
		}

		u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
		u16 wPackDataLen = 0;
		PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
		cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);

		//TW Module ??
		//VMP Module ??
		cServMsg.CatMsgBody((u8*)&tMtH323Alias, sizeof(tMtH323Alias));
		cServMsg.CatMsgBody((u8*)&tMtE164Alias, sizeof(tMtE164Alias));
		cServMsg.CatMsgBody((u8*)&tMtAddr, sizeof(tMtAddr));
		cServMsg.CatMsgBody((u8*)&byType, sizeof(byType));

        u8 byInsID = AssignIdleConfInsID();
        if(0 != byInsID)
        {
            ::OspPost(MAKEIID( AID_MCU_VC, byInsID ), cServMsg.GetEventId(), 
                        cServMsg.GetServMsg(),cServMsg.GetServMsgLen());
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] mt create conf, insId is %d\n", byInsID);
        }
        else
        {
            /*ConfLog(FALSE, "[DaemonProcMtMcuCreateConfReq] assign instance id failed!\n");*/
            ConfPrint(LOG_LVL_WARNING, MID_MCU_CALL, "[DaemonProcMtMcuCreateConfReq] assign instance id failed!\n");
        }
		
	}
	else  //加入会议
	{
		// vcs会议/mcs会议不能互连
		CConfId cConfId = g_cMcuVcApp.GetConfIdByName(tConfAlias.m_achAlias, FALSE,TRUE);
		if( byType != TYPE_MT && cConfId.GetConfSource() != cServMsg.GetConfId().GetConfSource()
			&& byVendorId == MT_MANU_KDCMCU)
		{
			cServMsg.SetErrorCode( ERR_MCU_CALLMCUERROR_CONFISHOLDING );
			g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "VcsConf And McsConf Don't Connect!\n" );
			return;
		}

		cServMsg.SetMsgBody((u8*)&tMtH323Alias, sizeof(tMtH323Alias));
		cServMsg.CatMsgBody((u8*)&tMtE164Alias, sizeof(tMtE164Alias));
		cServMsg.CatMsgBody((u8*)&tMtAddr, sizeof(tMtAddr));
		cServMsg.CatMsgBody((u8*)&tConfAlias, sizeof(tConfAlias));
		cServMsg.CatMsgBody(&byType, sizeof(byType));
		cServMsg.CatMsgBody(&byEncrypt, sizeof(byEncrypt));
		cServMsg.CatMsgBody((u8*)&wCallRate, sizeof(wCallRate));
        // guzh [6/19/2007] 
        cServMsg.CatMsgBody((u8*)&bLowLevelMcuCalledIn, sizeof(bLowLevelMcuCalledIn));
		g_cMcuVcApp.SendMsgToDaemonConf( MT_MCU_MTJOINCONF_REQ, cServMsg.GetServMsg(),cServMsg.GetServMsgLen() );
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_CALL, "send MT_MCU_MTJOINCONF_REQ to daemon conf\n");
	}

	return;
}

/*====================================================================
    函数名      ：ProcMcuMcuRegisterNotify
    功能        ：mcu级联成功后的注册消息处理
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/21    3.6         Jason         创建
    2005/11/3   4.0			万春雷        增加合并级联时的会议密码校验
====================================================================*/
void CMcuVcInst::ProcMcuMcuRegisterNotify(  const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	s8 *pszPwd  = (s8 *)cServMsg.GetMsgBody();
	s32 nPwdLen = cServMsg.GetMsgBodyLen() - sizeof(BOOL32);
	BOOL32 bSupportMultCascade = *(BOOL32 *)(cServMsg.GetMsgBody()+ nPwdLen);

	if( tMt.IsNull() )
	{
		return;
	}

	switch( CurState() )
	{
	case STATE_ONGOING:
		if ( m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId()) || 
			 TRUE == DealMtMcuEnterPwdRsp(tMt, pszPwd, nPwdLen) )
		{
		
			// [7/9/2010 xliang] 记录是否支持多级联
			if( bSupportMultCascade )
			{

				u8 bySmcuId = cServMsg.GetSrcMtId();
				u16 wMcuIdx = GetMcuIdxFromMcuId( bySmcuId );
				m_ptConfOtherMcTable->SetMcuSupMultCas( wMcuIdx );
			
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "Mcu.(%d,%d) support multiple cascade!\n", tMt.GetMcuId(), tMt.GetMtId());
			}

			TConfMtInfo tConfMtInfo = m_tConfAllMtInfo.m_tLocalMtInfo;
			tConfMtInfo.SetMcuIdx( LOCAL_MCUID );
			cServMsg.SetMsgBody((u8 *)&tConfMtInfo, sizeof(TConfMtInfo));
			SendMsgToMt( tMt.GetMtId(), MCU_MCU_ROSTER_NOTIF,  cServMsg);
            ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "Recv MCU_MCU_REGISTER_NOTIF, Sending MCU_MCU_ROSTER_NOTIF!\n");

			//lukunpeng 2010/07/08 增加是否支持多回传判断
			if( IsSupportMultiSpy() )
			{
				//lukunpeng 2010/06/10 注意，修改掉消息体，通知本级所支持的最大带宽数
				u32 dwMaxSpyBW = m_tConf.GetSndSpyBandWidth();
				dwMaxSpyBW = htonl( dwMaxSpyBW );
				cServMsg.SetMsgBody((u8 *)&dwMaxSpyBW, sizeof(dwMaxSpyBW));
				SendMsgToMt( tMt.GetMtId(), MCU_MCU_MULTSPYCAP_NOTIF,  cServMsg);
				
				SendConfExtInfoToMcs( 0,tMt.GetMtId() );				
			}	
			// End
		}
		else
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "Recv MCU_MCU_REGISTER_NOTIF but ConfPwd-Validate Err and drop mt.%d\n", tMt.GetMtId());
		}

        //将对应下级MCU的级联通道打开次数清零，认为打开成功 [01/11/2006-zbq]
//        m_abyCasChnCheckTimes[tMt.GetMtId()-1] = 0;	
//		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "Recv MCU_MCU_REGISTER_NOTIF, m_abyCasChnCheckTimes[%d]=0 !\n", tMt.GetMtId()-1);
		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MMCU, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

	return;
}

/*====================================================================
    函数名      ：ProcMcuMcuRosterNotify
    功能        ：mcu级联成功后花名册通知
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/21    3.6         Jason         创建
====================================================================*/
void CMcuVcInst::ProcMcuMcuRosterNotify(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt  tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	TConfMtInfo *ptConfMtInfo = (TConfMtInfo *)(cServMsg.GetMsgBody());
	u8 byLastRosterNotify = *(u8*)(cServMsg.GetMsgBody() + sizeof(TConfMtInfo));

	u8 byMcuId = (u8)ptConfMtInfo->GetMcuIdx();	
	u16 wMcuIdx = 0;
	switch( CurState() )
	{
	case STATE_ONGOING:
		wMcuIdx = GetMcuIdxFromMcuId( byMcuId );
		if( INVALID_MCUIDX == wMcuIdx )
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "[ProcMcuMcuRosterNotify] Fail To Get McuIdx Info.McuId.%d\n",
				byMcuId
				);
			return;
		}
		ptConfMtInfo->SetMcuIdx( wMcuIdx );
		
		//刷新TConfMtInfo
		m_tConfAllMtInfo.SetMtInfo( *ptConfMtInfo );

		//请求终端列表
		if (byLastRosterNotify &&
			(MT_TYPE_SMCU == m_ptMtTable->GetMtType(byMcuId) ||
			 (MT_TYPE_MMCU == m_ptMtTable->GetMtType(byMcuId) &&
			  g_cMcuVcApp.IsShowMMcuMtList())))
		{
			TMcuMcuReq tReq;
			memset(&tReq, 0, sizeof(tReq));

			CServMsg	cTempServMsg;
			cTempServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
			SendMsgToMt( tMt.GetMtId(), MCU_MCU_MTLIST_REQ,  cTempServMsg);
			
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "Recv Last MCU_MCU_ROSTER_NOTIF and send MCU_MCU_MTLIST_REQ to mt.%d\n", tMt.GetMtId());
		}
		else
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "Recv MCU_MCU_ROSTER_NOTIF without send MCU_MCU_MTLIST_REQ \n");
		}


		//发给会控会议所有终端信息
		//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
		//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );

		// [12/9/2010 liuxu][走读三级手动电视墙]cServMsg有可能被MCU_MCU_MTLIST_REQ改掉了...
		SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
		break;

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MMCU, "Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}


/*====================================================================
    函数名      ：ProcMcuMcuMtListReq
    功能        ：对方mcu请求终端列表
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/21    3.6         Jason         创建
====================================================================*/
void CMcuVcInst::ProcMcuMcuMtListReq(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt  tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	TMt tTmpMt;
    TMcuMcuMtInfo atInfo[MAXNUM_CONF_MT];

	u8 byInfoNum = 0;
	TLogicalChannel tChannel;
	TMtStatus tMtStatus;
	BOOL32 bIsMMcuReq = FALSE;
	
	STATECHECK;		

	if( !m_tCascadeMMCU.IsNull() &&
		tMt.GetMtId() == m_tCascadeMMCU.GetMtId() )
	{
		bIsMMcuReq = TRUE;
	}

	//请求终端列表
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(tMt.GetMtId()));
	LogPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuMtListReq]Recv MtList_REQ from Mcu.%d McuIdx.%d\n", tMt.GetMtId(), GetMcuIdxFromMcuId(tMt.GetMtId()));
    // 顾振华 [5/31/2006] MtId循环应该是1-192
	for(u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
	{
		tTmpMt = m_ptMtTable->GetMt(byLoop);
		if(!tTmpMt.IsLocal())
		{
			continue;
		}
		tTmpMt.SetMcuIdx( LOCAL_MCUID );
		atInfo[byInfoNum].m_tMt = tTmpMt;
		
		TMtAlias tTmpMtAlias;
		m_ptMtTable->GetMtAlias(tTmpMt.GetMtId(), mtAliasTypeE164, &tTmpMtAlias);

		TMt tLocalMt = m_ptMtTable->GetMt(byLoop);
		// 如果是电话终端，优先取E164号
		if (IsPhoneMt(tLocalMt) &&!tTmpMtAlias.IsAliasNull())
		{
			strncpy((s8 *)(atInfo[byInfoNum].m_szMtName), tTmpMtAlias.m_achAlias, sizeof(atInfo[byInfoNum].m_szMtName) - 1);
		}
		else if ( m_ptMtTable->GetMtAlias(tTmpMt.GetMtId(), mtAliasTypeH323ID, &tTmpMtAlias))
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcuMcuMtListReq]copy 323ID to Mt.%d\n", byLoop);
			strncpy((s8 *)(atInfo[byInfoNum].m_szMtName), tTmpMtAlias.m_achAlias, sizeof(atInfo[byInfoNum].m_szMtName) - 1);
		}
		else
		{
			strncpy((s8 *)(atInfo[byInfoNum].m_szMtName), m_ptMtTable->GetMtAliasFromExt(byLoop), sizeof(atInfo[byInfoNum].m_szMtName) - 1);
		}

		atInfo[byInfoNum].m_dwMtIp   = htonl(m_ptMtTable->GetIPAddr(byLoop));
		atInfo[byInfoNum].m_byMtType = m_ptMtTable->GetMtType(byLoop);
		atInfo[byInfoNum].m_byManuId = m_ptMtTable->GetManuId(byLoop);
        atInfo[byInfoNum].m_byProtocolType = m_ptMtTable->GetProtocolType(byLoop);
		strncpy((s8 *)atInfo[byInfoNum].m_szMtDesc, "notsupport", sizeof(atInfo[byInfoNum].m_szMtDesc));
		atInfo[byInfoNum].m_szMtDesc[sizeof(atInfo[byInfoNum].m_szMtDesc)-1] = 0;
		if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_VIDEO, &tChannel, FALSE ) )
		{
			atInfo[byInfoNum].m_byVideoIn = tChannel.GetChannelType();
		}
		else
		{
			atInfo[byInfoNum].m_byVideoIn = MEDIA_TYPE_NULL;
		}
		if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_VIDEO, &tChannel, TRUE ) )
		{
			atInfo[byInfoNum].m_byVideoOut = tChannel.GetChannelType();
		}
		else
		{
			atInfo[byInfoNum].m_byVideoOut = MEDIA_TYPE_NULL;
		}
		if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_SECVIDEO, &tChannel, FALSE ) )
		{
			atInfo[byInfoNum].m_byVideo2In = tChannel.GetChannelType();
		}
		else
		{
			atInfo[byInfoNum].m_byVideo2In = MEDIA_TYPE_NULL;
		}
		if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_SECVIDEO, &tChannel, TRUE ) )
		{
			atInfo[byInfoNum].m_byVideo2Out = tChannel.GetChannelType();
		}
		else
		{
			atInfo[byInfoNum].m_byVideo2Out = MEDIA_TYPE_NULL;
		}
		if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_AUDIO, &tChannel, FALSE ) )
		{
			atInfo[byInfoNum].m_byAudioIn = tChannel.GetChannelType();
		}
		else
		{
			atInfo[byInfoNum].m_byAudioIn = MEDIA_TYPE_NULL;
		}
		if( m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_AUDIO, &tChannel, TRUE ) )
		{
			atInfo[byInfoNum].m_byAudioOut = tChannel.GetChannelType();
		}
		else
		{
			atInfo[byInfoNum].m_byAudioOut = MEDIA_TYPE_NULL;
		}
		atInfo[byInfoNum].m_byIsDataMeeting =  m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_T120DATA, &tChannel, TRUE ) ? 1:0;
		if(atInfo[byInfoNum].m_byIsDataMeeting == 0)
		{
			atInfo[byInfoNum].m_byIsDataMeeting =  m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_T120DATA, &tChannel, FALSE ) ? 1:0;
		}	
		m_ptMtTable->GetMtStatus(byLoop, &tMtStatus);
		atInfo[byInfoNum].m_byIsVideoMuteIn	= tMtStatus.IsSendVideo() ? 0:1;
		atInfo[byInfoNum].m_byIsVideoMuteOut	= tMtStatus.IsReceiveVideo() ? 0:1;   
		atInfo[byInfoNum].m_byIsAudioMuteIn	= tMtStatus.IsSendAudio() ? 0:1;
		atInfo[byInfoNum].m_byIsAudioMuteOut	= tMtStatus.IsReceiveAudio() ? 0:1;
		atInfo[byInfoNum].m_byIsConnected     = m_tConfAllMtInfo.MtJoinedConf(tTmpMt.GetMtId()) ? 1:0;
		atInfo[byInfoNum].m_byIsFECCEnable    = m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_H224DATA, &tChannel, TRUE ); 
	
		atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount = 0;
		if(tTmpMt == m_tConf.GetSpeaker())
		{
			atInfo[byInfoNum].m_tPartVideoInfo.m_atViewPos[atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount].m_nViewID = m_dwSpeakerViewId;
			atInfo[byInfoNum].m_tPartVideoInfo.m_atViewPos[atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount].m_bySubframeIndex = 0;
			atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount++;
		}
		TVMPParam tVmpParam = m_tConf.GetStatus().GetVmpParam();
		if(tVmpParam.IsMtInMember(tTmpMt))
		{
			u8 byMemberId  = m_tConf.GetStatus().GetVmpParam().GetChlOfMtInMember( tTmpMt );
			atInfo[byInfoNum].m_tPartVideoInfo.m_atViewPos[atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount].m_nViewID = m_dwVmpViewId;
			atInfo[byInfoNum].m_tPartVideoInfo.m_atViewPos[atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount].m_bySubframeIndex = byMemberId;
			atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount++;
		}
		
		if(ptMcInfo != NULL&&ptMcInfo->m_tSpyMt == tTmpMt)
		{
			atInfo[byInfoNum].m_tPartVideoInfo.m_atViewPos[atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount].m_nViewID = ptMcInfo->m_dwSpyViewId;
			atInfo[byInfoNum].m_tPartVideoInfo.m_atViewPos[atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount].m_bySubframeIndex = 0;
			atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount++;					
		}
    
		atInfo[byInfoNum].m_tPartVideoInfo.m_nOutputLID = 0;
		atInfo[byInfoNum].m_tPartVideoInfo.m_nOutVideoSchemeID = 0;
		TMt tMtVSrc;
		m_ptMtTable->GetMtSrc(byLoop, &tMtVSrc, MODE_VIDEO);
		if(tMtVSrc.IsNull())
		{
			atInfo[byInfoNum].m_tPartVideoInfo.m_nOutputLID = 0;
			atInfo[byInfoNum].m_tPartVideoInfo.m_nOutVideoSchemeID = 0;
		}

		if(tMt.GetMtId() == tTmpMt.GetMtId())
		{
			if(ptMcInfo != NULL&& 
				ptMcInfo->m_tSpyMt.GetMtId() == tMtVSrc.GetMtId())
			{
				atInfo[byInfoNum].m_tPartVideoInfo.m_nOutputLID = ptMcInfo->m_dwSpyViewId;				
				atInfo[byInfoNum].m_tPartVideoInfo.m_nOutVideoSchemeID = ptMcInfo->m_dwSpyVideoId;
			}
		}

		if(tMtVSrc == m_tConf.GetSpeaker())
		{
			atInfo[byInfoNum].m_tPartVideoInfo.m_nOutputLID = m_dwSpeakerViewId;
			atInfo[byInfoNum].m_tPartVideoInfo.m_nOutVideoSchemeID = m_dwSpeakerVideoId;
		}
		else if(tMtVSrc == m_tVmpEqp)
		{
			atInfo[byInfoNum].m_tPartVideoInfo.m_nOutputLID = m_dwVmpViewId;
			atInfo[byInfoNum].m_tPartVideoInfo.m_nOutVideoSchemeID = m_dwVmpVideoId;
		}

		if( tMt.GetMtId() == tTmpMt.GetMtId() && tMt == m_tCascadeMMCU )
		{
			if(ptMcInfo != NULL)
			{
				atInfo[byInfoNum].m_tPartVideoInfo.m_nOutputLID = ptMcInfo->m_dwSpyViewId;				
				atInfo[byInfoNum].m_tPartVideoInfo.m_nOutVideoSchemeID = ptMcInfo->m_dwSpyVideoId;
			}
		}
   
		byInfoNum++;	
	}

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuMtListReq] Notified McuID.%d LocalTotalMtNum.%d\n", tMt.GetMtId(), byInfoNum );

	u8 byLastPack = 1;
	cServMsg.SetMsgBody(&byLastPack, sizeof(u8));
	cServMsg.CatMsgBody((u8 *)atInfo, byInfoNum * sizeof(TMcuMcuMtInfo));
	SendMsgToMt( tMt.GetMtId(), MCU_MCU_MTLIST_ACK,  cServMsg); //本级的

	//级联上级MCU后的第一次下级MCU已有终端状态强制通知，之后的个别终端状态进行实时通知
	if( m_tConfInStatus.IsNtfMtStatus2MMcu() ||
		// vcs刷列表，同时需要刷新其状态信息，由于其中包含重要的终端呼叫模式信息
		VCS_CONF == m_tConf.GetConfSource())
	{
		OnNtfMtStatusToMMcu( m_tCascadeMMCU.GetMtId() );
		m_tConfInStatus.SetNtfMtStatus2MMcu(FALSE);
	}

	if( bIsMMcuReq )
	{
		SendMtListToMcs(INVALID_MCUIDX, FALSE, TRUE);// 发下级的 [pengguofeng 7/10/2013]
// 		SendAllSMcuMtInfoToMMcu();
// 		SendAllSMcuMtStatusToMMcu(); // 时序问题，必须在收到mtadp的消息之后再上报，否则上报加不进去mcuidx [pengguofeng 7/26/2013]
// 		SendSMcuUpLoadMtToMMcu();
	}
	
	return;
}

/*====================================================================
    函数名      ：GetConfViewInfo
    功能        ：获取视图信息
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMcuId            [In]
                  TCConfViewInfo &tInfo [Out]
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/05/14    4.0         顾振华         创建
====================================================================*/
void CMcuVcInst::GetConfViewInfo(u16 wMcuIdx, TCConfViewInfo &tInfo)
{
    u8 byViewIndex = 0;
    tInfo.m_byDefViewIndex = 0;
    
    TCapSupport tCapSupport = m_tConf.GetCapSupport();

    //spy view 
    TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
    if(ptConfMcInfo == NULL)
    {
        return;
    }
    tInfo.m_atViewInfo[byViewIndex].m_nViewId		= ptConfMcInfo->m_dwSpyViewId;
    tInfo.m_atViewInfo[byViewIndex].m_byRes			=  VIDEO_FORMAT_CIF;//目前版本只支持h263-cif
    tInfo.m_atViewInfo[byViewIndex].m_byCurGeoIndex	= 0;
    tInfo.m_atViewInfo[byViewIndex].m_byGeoCount	= 1;
    tInfo.m_atViewInfo[byViewIndex].m_abyGeoInfo[0]	= 1;
    if(ptConfMcInfo->m_tSpyMt.IsNull())
    {
        tInfo.m_atViewInfo[byViewIndex].m_byMtCount =  0;
    }
    else
    {
        tInfo.m_atViewInfo[byViewIndex].m_byMtCount =  1;
        tInfo.m_atViewInfo[byViewIndex].m_atMts[0] = ptConfMcInfo->m_tSpyMt;
		tInfo.m_atViewInfo[byViewIndex].m_atMts[0].SetMcuId( LOCAL_MCUID );
    }
    
    tInfo.m_atViewInfo[byViewIndex].m_byVSchemeCount = 1;
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_byMediaType = tCapSupport.GetMainVideoType();
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_dwMaxRate = m_tConf.GetBitRate();
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_dwMinRate = tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_dwMaxRate;
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_bCanUpdateRate = TRUE;
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_nFrameRate = 30;
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_nVideoSchemeID = ptConfMcInfo->m_dwSpyVideoId;

    // guzh [7/19/2007] 回传视图是否轮询切换状态
    tInfo.m_atViewInfo[byViewIndex].m_byAutoSwitchStatus = GETBBYTE( m_tConfInStatus.IsPollSwitch() );
    if (m_tConfInStatus.IsPollSwitch())
    {
        tInfo.m_atViewInfo[byViewIndex].m_nAutoSwitchTime = m_tPollSwitchParam.GetTimeSpan();
    }
    else
    {
        tInfo.m_atViewInfo[byViewIndex].m_nAutoSwitchTime = 0;
    }
    
    byViewIndex++;
    
    //发言人
    tInfo.m_atViewInfo[byViewIndex].m_nViewId			= m_dwSpeakerViewId;
    tInfo.m_atViewInfo[byViewIndex].m_byRes				=  VIDEO_FORMAT_CIF;//目前版本只支持h263-cif
    tInfo.m_atViewInfo[byViewIndex].m_byCurGeoIndex		= 0;
    tInfo.m_atViewInfo[byViewIndex].m_byGeoCount		= 1;
    tInfo.m_atViewInfo[byViewIndex].m_abyGeoInfo[0]		= 1;
    if(m_tConf.GetSpeaker().IsNull())
    {
        tInfo.m_atViewInfo[byViewIndex].m_byMtCount =  0;
    }
    else
    {
        tInfo.m_atViewInfo[byViewIndex].m_byMtCount =  1;
        //tInfo.m_atViewInfo[byViewIndex].m_atMts[0] = m_tConf.GetSpeaker();

		TMt tSpeaker = m_tConf.GetSpeaker();
		BuildMultiCascadeMtInfo( tSpeaker,
						tInfo.m_atViewInfo[byViewIndex].m_atMts[0]
						);
        
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuVideoInfoReq] Speaker byViewIndex.%d McuId.%d MtId.%d\n", 
            byViewIndex, tInfo.m_atViewInfo[byViewIndex].m_atMts[0].GetMcuId(), 
            tInfo.m_atViewInfo[byViewIndex].m_atMts[0].GetMtId() );
    }
    tInfo.m_atViewInfo[byViewIndex].m_byVSchemeCount = 1;
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_byMediaType = tCapSupport.GetMainVideoType();
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_dwMaxRate = m_tConf.GetBitRate();
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_dwMinRate = tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_dwMaxRate;
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_bCanUpdateRate = TRUE;
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_nFrameRate = 30;
    tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_nVideoSchemeID = m_dwSpeakerVideoId;
    byViewIndex++;
    //视频复合源
    if(g_cMcuVcApp.GetVMPMode(m_tVmpEqp) != CONF_VMPMODE_NONE)
    {	        
        TVMPParam tVMPParam = m_tConf.GetStatus().GetVmpParam();
        tInfo.m_atViewInfo[byViewIndex].m_nViewId			= m_dwVmpViewId;
        tInfo.m_atViewInfo[byViewIndex].m_byRes			=  VIDEO_FORMAT_CIF;//目前版本只支持h263-cif
        tInfo.m_atViewInfo[byViewIndex].m_byCurGeoIndex	= 0;
        tInfo.m_atViewInfo[byViewIndex].m_byGeoCount		= 1;
        tInfo.m_atViewInfo[byViewIndex].m_abyGeoInfo[0]	= tVMPParam.GetMaxMemberNum();
        tInfo.m_atViewInfo[byViewIndex].m_byMtCount =  0;
		// xliang [2/26/2009] FIXME: 这里暂不动MAXNUM_SUBFRAMEINGEO，保持列表总数为16，改成20会把底层撑死
        for(s32 nIndex=0; nIndex<MAXNUM_MPUSVMP_MEMBER&&nIndex<MAXNUM_SUBFRAMEINGEO; nIndex++)
        {
            if(!tVMPParam.GetVmpMember(nIndex)->IsNull())
            {
                tInfo.m_atViewInfo[byViewIndex].m_atMts[tInfo.m_atViewInfo[byViewIndex].m_byMtCount].SetMt((TMt)(*tVMPParam.GetVmpMember(nIndex)));
                tInfo.m_atViewInfo[byViewIndex].m_byMtCount++;
            }
        }
        tInfo.m_atViewInfo[byViewIndex].m_byVSchemeCount = 1;
        tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_byMediaType = tCapSupport.GetMainVideoType();
        tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_dwMaxRate = m_tConf.GetBitRate();
        tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_dwMinRate = tInfo.m_atViewInfo[0].m_atVSchemes[0].m_dwMaxRate;
        tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_bCanUpdateRate = TRUE;
        tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_nFrameRate = 30;
        tInfo.m_atViewInfo[byViewIndex].m_atVSchemes[0].m_nVideoSchemeID = m_dwVmpVideoId;
        byViewIndex++;
    }
    
    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuVideoInfoReq] byViewCount.%d\n", byViewIndex );
    
	tInfo.m_byViewCount = byViewIndex; 
}


/*====================================================================
    函数名      ：ProcMcuMcuVideoInfoReq
    功能        ：对方mcu请求视频信息
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/24    3.6         Jason         创建
====================================================================*/
void CMcuVcInst::ProcMcuMcuVideoInfoReq(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt  tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	
	STATECHECK;	

	TCConfViewInfo tInfo;
    GetConfViewInfo( GetMcuIdxFromMcuId(tMt.GetMtId()), tInfo );

	cServMsg.SetMsgBody((u8 *)&tInfo, sizeof(tInfo));
	SendMsgToMt( tMt.GetMtId(), MCU_MCU_VIDEOINFO_ACK,  cServMsg);
}

/*====================================================================
    函数名      ：ProcMcuMcuVideoInfoack
    功能        ：请求对方mcu视频信息成功应答
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/25   3.6         Jason         创建
====================================================================*/
void CMcuVcInst::ProcMcuMcuVideoInfoAck(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt  tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	
	STATECHECK;	
	
	TCConfViewInfo*	ptConfViewInfo = (TCConfViewInfo *)(cServMsg.GetMsgBody());

	TConfMcInfo*	ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(tMt.GetMtId())); 
	if(ptConfMcInfo == NULL)
	{

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuVideoInfoAck]   ptConfMcInfo is NULL \n" );
		return;
	}
	memcpy(&(ptConfMcInfo->m_tConfViewInfo), ptConfViewInfo, sizeof(TCConfViewInfo));

	/*ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuVideoInfoAck] Mcu(%d) lastSpeaker(%d,%d) now speaker(%d,%d)  videoinfo m_byViewCount:%d SpyMt(%d,%d)\n", tMt.GetMtId(),
							 ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMcuId(),
							ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId(),
							ptConfViewInfo->m_atViewInfo[1].m_atMts[0].GetMcuId(),
							ptConfViewInfo->m_atViewInfo[1].m_atMts[0].GetMtId(),
							ptConfViewInfo->m_byViewCount,
							ptConfMcInfo->m_tSpyMt.GetMcuId(),ptConfMcInfo->m_tSpyMt.GetMtId()
							);			
		

	
	if( tMt == m_tCascadeMMCU && ptConfViewInfo->m_byViewCount > 1 &&
		ptConfViewInfo->m_atViewInfo[1].m_byMtCount > 0 &&
		ptConfViewInfo->m_atViewInfo[1].m_atMts[0] == m_tCascadeMMCU )
	{
		
		//zjj[08/13/2009]如果上次上级mcu拖发言人且是下级mcu的终端和这次的下级mcu的发言人不同时
		//(即上级mcu取消发言人，且这个发言人是下级mcu的终端)
		if( ptConfMcInfo->m_tConfAudioInfo.m_byMixerCount > 0 &&
			ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.IsLocal() &&
			ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId() != ptConfViewInfo->m_atViewInfo[1].m_atMts[0].GetMtId())
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuVideoInfoAck] Mcu(%d) MMCU Speaker is cancel. \
				Now speaker is m_tCascadeMMCU \n",
				tMt.GetMtId() );

			u8 byMtId = ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId();

			//补建上级mcu到本级回传源的音频交换
			//因为要保持唇音同步如果回传终端的视频走bas，那么音频也走bas，否则建直接的交换
			TLogicalChannel tVidChn;
			if( IsHDConf(m_tConf) && m_cMtRcvGrp.IsMtNeedAdp(byMtId) &&							  
					m_ptMtTable->GetMtLogicChnnl(byMtId, LOGCHL_VIDEO, &tVidChn, TRUE) &&
					!IsNeedAdapt(GetLocalAudBrdSrc(), m_ptMtTable->GetMt(byMtId), MODE_AUDIO))
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "Mcu(%d) Start bas To Mt(%d,%d) Audio Switch\n",tMt.GetMtId(),
							m_ptMtTable->GetMt(byMtId).GetMcuId(),
							m_ptMtTable->GetMt(byMtId).GetMtId()
									);
				//continue;
				StartSwitchAud2MtNeedAdp( GetLocalAudBrdSrc(), m_ptMtTable->GetMt(byMtId));
			}
			else
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "Mcu(%d) Start m_tAudBrdSrc To Mt(%d,%d) Audio Switch\n",tMt.GetMtId(),
							m_ptMtTable->GetMt(byMtId).GetMcuId(),
							m_ptMtTable->GetMt(byMtId).GetMtId()
									);
				StartSwitchToSubMt( GetLocalAudBrdSrc(), 0, byMtId, MODE_AUDIO );
			}

			

			ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker = ptConfViewInfo->m_atViewInfo[1].m_atMts[0];
			
		}
		else
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuVideoInfoAck] Mcu(%d) MMCU Speaker not cancel lastSpeaker(%d,%d) now speaker(%d,%d)  audioinfo mixercount:%d SpyMt(%d,%d)\n", tMt.GetMtId(),
							 ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMcuId(),
							ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId(),
							ptConfViewInfo->m_atViewInfo[1].m_atMts[0].GetMcuId(),
							ptConfViewInfo->m_atViewInfo[1].m_atMts[0].GetMtId(),
							ptConfMcInfo->m_tConfAudioInfo.m_byMixerCount,
							ptConfMcInfo->m_tSpyMt.GetMcuId(),ptConfMcInfo->m_tSpyMt.GetMtId()
							);			
		}
		
	}
	*/

	//////////////////////////////////////////////////////////////////////////
	
	return;
}

/*====================================================================
    函数名      ：ProcMcuMcuConfViewChgNtf
    功能        ：会议视图改变通知
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/25   3.6          Jason         创建
====================================================================*/
void CMcuVcInst::ProcMcuMcuConfViewChgNtf(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt  tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	
	STATECHECK;	
	
	TCConfViewChangeNtf*	ptNtf = (TCConfViewChangeNtf *)(cServMsg.GetMsgBody());

	TConfMcInfo*	ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(tMt.GetMtId())); 
	if(ptConfMcInfo == NULL)
	{
		return;
	}
	for(s32 nLoop=0; nLoop<ptConfMcInfo->m_tConfViewInfo.m_byViewCount; nLoop++)
	{
		if(ptConfMcInfo->m_tConfViewInfo.m_atViewInfo[nLoop].m_nViewId != ptNtf->m_nViewID)
		{
			continue;
		}

		TCViewInfo *pInfo = &(ptConfMcInfo->m_tConfViewInfo.m_atViewInfo[nLoop]);
		for(s32 nIndex=0; nIndex<pInfo->m_byGeoCount; nIndex++)
		{
			if(pInfo->m_abyGeoInfo[nIndex] != ptNtf->m_bySubframeCount)
			{
				continue;
			}

			pInfo->m_byCurGeoIndex = nIndex;
			break;
		}
		break;
	}

}

/*====================================================================
    函数名      ：ProcMcuMcuAudioInfoack
    功能        ：请求对方mcu音频信息成功应答
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/25   3.6         Jason         创建
====================================================================*/
void CMcuVcInst::ProcMcuMcuAudioInfoAck(  const CMessage * pcMsg )
{

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt  tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	
	STATECHECK;	
	
	TCConfAudioInfo* ptConfAudioInfo = (TCConfAudioInfo *)(cServMsg.GetMsgBody());
	if (NULL == ptConfAudioInfo)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "[ProcMcuMcuAudioInfoAck]   Rcv ptConfAudioInfo is NULL \n" );
		return;
	}

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAudioInfoAck] Rcv AudioInfo Mcu<McuId.%d, MtId.%d>, MixerCount.%d, Speaker<McuId.%d, MtId.%d>\n",
								tMt.GetMcuId(), tMt.GetMtId(),
								ptConfAudioInfo->m_byMixerCount,
								ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMcuId(),
								ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMtId());

	TConfMcInfo* ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(tMt.GetMtId())); 
	if(ptConfMcInfo == NULL)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "[ProcMcuMcuAudioInfoAck]   ptConfMcInfo is NULL \n" );
		return;
	}

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAudioInfoAck] Last AudioInfo MixerCount.%d, Speaker<McuId.%d, MtId.%d>, SpyMt<McuId.%d, MtId.%d>, MMcuViewMt<McuId.%d, MtId.%d>!\n",
								ptConfMcInfo->m_tConfAudioInfo.m_byMixerCount,
								ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMcuId(),
								ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId(),
								ptConfMcInfo->m_tSpyMt.GetMcuId(),
								ptConfMcInfo->m_tSpyMt.GetMtId(),
								ptConfMcInfo->m_tMMcuViewMt.GetMcuId(),
								ptConfMcInfo->m_tMMcuViewMt.GetMtId());

	TMt tLastSpeaker;
	tLastSpeaker.SetNull();
	if( ptConfMcInfo->m_tConfAudioInfo.m_byMixerCount >0 )
	{
		tLastSpeaker = ptConfMcInfo->m_tConfAudioInfo.m_tMixerList[0].m_tSpeaker;

		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "Mcu(%d) Last Speaker: (%d,%d)\n", tMt.GetMtId(),
							      tLastSpeaker.GetMcuId(),
							      tLastSpeaker.GetMtId());
	}
	
	memcpy(&(ptConfMcInfo->m_tConfAudioInfo), ptConfAudioInfo, sizeof(TCConfAudioInfo));

	TMt tSpeaker;
	tSpeaker.SetNull();

	u16 wMMcuIdx = INVALID_MCUIDX;
	if( !m_tCascadeMMCU.IsNull() && 
		(wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() )) != INVALID_MCUIDX &&
		IsLocalAndSMcuSupMultSpy( wMMcuIdx ) 
		)
	{
		//如果上级拖下级mcu做发言人，把下级的上传终端做发言人。
		if (ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMcuId() ==  m_tCascadeMMCU.GetMtId() 
			&& ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMtId() == 0)
		{
			tSpeaker = tLastSpeaker;//ptConfMcInfo->m_tSpyMt;
		}
		//如果是非本地终端做上级发言人，还原为老上级发言人。
		else if (!ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.IsMcuIdLocal())
		{
			tSpeaker = tLastSpeaker;
		}
		else
		{
			//是本地终端做发言人
			tSpeaker = ptConfAudioInfo->m_tMixerList[0].m_tSpeaker;
		}
	}
	else
	{
		tSpeaker = ptConfMcInfo->m_tSpyMt;
		tSpeaker.SetMcuId( LOCAL_MCUID );
	}

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAudioInfoAck] Mcu(%d) Remote Speaker(AudioBrdSrc): (%d,%d) m_byMixerCount(%d) ptConfMcInfo->m_tMMcuViewMt(%d,%d)\n", tMt.GetMtId(),
				ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMcuId(),
				ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMtId(),
				ptConfAudioInfo->m_byMixerCount,
				tSpeaker.GetMcuId(),
				tSpeaker.GetMtId()
			);

	TMt tMtLocalSpeaker = tSpeaker;
	tMtLocalSpeaker.SetMcuId( LOCAL_MCUIDX );
	TMt tMtLocalLastSpeaker = tLastSpeaker;
	tMtLocalLastSpeaker.SetMcuId( LOCAL_MCUIDX );

    if ( tMt == m_tCascadeMMCU && GetLocalAudBrdSrc() == m_tCascadeMMCU )
    {
		TLogicalChannel tVidChn;
		TSwitchGrp tSwitchGrp;
		//20111009 zjl 新的媒体源是本级终端+上一个媒体源是下级mcu或者非本级终端，要通告上一个媒体源当前新的媒体源
		if( tSpeaker.IsMcuIdLocal())
		{
			if (!tLastSpeaker.IsMcuIdLocal())
			{
				SendMsgToMt( (u8)tLastSpeaker.GetMcuId(), MCU_MCU_AUDIOINFO_ACK, cServMsg );
			} 
			/*20111021 zjl 如果上一个发言人和当前的发言人都是本级所属的同一个mcu, 则不需要在这里通告，
					       这里通告的目的在于，当前的发言人是本级的“终端”，上一个发言人是下级mcu,
						   告诉它一个类似于<192, n>的发言人，下级收到后转述<n, 0>则认为是上级mcu做发言人
						   下级会补建到上一个发言人的交换*/
			else if(tLastSpeaker.IsMcuIdLocal() && 
				    !(tLastSpeaker == tSpeaker) &&
					MT_TYPE_SMCU == m_ptMtTable->GetMtType(tLastSpeaker.GetMtId()))
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAudioInfoAck] Notify Smcu start switch to its cur spymt with aud!\n");
				SendMsgToMt( (u8)tLastSpeaker.GetMtId(), MCU_MCU_AUDIOINFO_ACK, cServMsg );
			}			
		}

		//20110727 zjl 当前上级媒体源为第三级终端则消息透传给第三级，如果是本级mcu则尝试取上传终端信息透传到下级
		if( ptConfAudioInfo->m_byMixerCount > 0)
		{
			if (!ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.IsMcuIdLocal() && 
				MT_TYPE_SMCU == m_ptMtTable->GetMtType((u8)ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMcuId()))
			{
				SendMsgToMt( (u8)ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMcuId(), 
								 MCU_MCU_AUDIOINFO_ACK, cServMsg );
			}
			else if (ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.IsMcuIdLocal() && 
				MT_TYPE_SMCU == m_ptMtTable->GetMtType((u8)ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMtId()))
			{
				TCConfAudioInfo tConfAudioInfo = *(TCConfAudioInfo *)(cServMsg.GetMsgBody());
				CServMsg cMsg;
				TMsgHeadMsg tHeadMsg;
				u16 wSmcuIdx = GetMcuIdxFromMcuId( tConfAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId());
				TConfMcInfo * ptMcuMcInfo = m_ptConfOtherMcTable->GetMcInfo( wSmcuIdx );
				if( NULL != ptMcuMcInfo && !ptMcuMcInfo->m_tMMcuViewMt.IsNull() ) 
				{
					tHeadMsg.m_tMsgSrc = BuildMultiCascadeMtInfo( ptMcuMcInfo->m_tMMcuViewMt, tConfAudioInfo.m_tMixerList[0].m_tSpeaker);
				}
				cMsg.SetEventId(MCU_MCU_AUDIOINFO_ACK);
				cMsg.SetMsgBody((u8 *)&tConfAudioInfo, sizeof(tConfAudioInfo));
				cMsg.CatMsgBody((u8 *)&tHeadMsg, sizeof(tHeadMsg));
				SendMsgToMt( (u8)ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMtId(), 
								 MCU_MCU_AUDIOINFO_ACK, cMsg );
			}	
		}
		
		// guzh [5/14/2007] 判断上级的广播源是否是本级的回传终端（上级mcu拖下级mcu的MT为发言人会进此判断）
        if ( ptConfAudioInfo->m_byMixerCount > 0 &&
             ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.IsMcuIdLocal())
        {
			if (MT_TYPE_MMCU == m_ptMtTable->GetMtType( ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMtId()))
			{
				if ( ptConfAudioInfo->m_byMixerCount >= 1)
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAudioInfoAck] New Speaker is mmcu!\n");
					
					if( g_cMcuVcApp.IsVASimultaneous() && IsNeedAdapt(GetLocalVidBrdSrc(), tMtLocalSpeaker, MODE_VIDEO) &&							  
							m_ptMtTable->GetMtLogicChnnl(tSpeaker.GetMtId(), LOGCHL_VIDEO, &tVidChn, TRUE) &&
							!IsNeedAdapt(GetLocalAudBrdSrc(), tMtLocalSpeaker, MODE_AUDIO) 
							)
					{
						StartSwitchAud2MtNeedAdp( GetAudBrdSrc(), tMtLocalSpeaker);
					}
					else
					{
						RestoreRcvMediaBrdSrc(1,&tMtLocalSpeaker,MODE_AUDIO);
					}
					
				 }
			}
			else if(MT_TYPE_SMCU != m_ptMtTable->GetMtType( ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMtId()))
			{
				 if ( ptConfAudioInfo->m_byMixerCount > 1 )
				 {
					if( g_cMcuVcApp.IsVASimultaneous() && IsNeedAdapt(GetLocalVidBrdSrc(), tMtLocalSpeaker, MODE_VIDEO) &&							  
						m_ptMtTable->GetMtLogicChnnl(tSpeaker.GetMtId(), LOGCHL_VIDEO, &tVidChn, TRUE) &&
						!IsNeedAdapt(GetLocalAudBrdSrc(), tMtLocalSpeaker, MODE_AUDIO) 
						)
					{
						StartSwitchAud2MtNeedAdp( GetAudBrdSrc(), tMtLocalSpeaker);
					}
					else
					{
						RestoreRcvMediaBrdSrc(1,&tMtLocalSpeaker,MODE_AUDIO);
					}
				 }
				 else
				 {
					 TMtStatus tStatus;
					 m_ptMtTable->GetMtStatus( tSpeaker.GetMtId(),&tStatus );
					 
					 if( !tStatus.GetSelectMt(MODE_AUDIO).IsNull() )
					 {
						 ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcuMcuAudioInfoAck] Mt(%d) is global speaker,but now sel other mt audio so cann't stop switch to it.\n",
							 tSpeaker.GetMtId()
							 );
					 }
					 else
					 {
						 if (tSpeaker.IsMcuIdLocal() && MT_TYPE_SMCU != m_ptMtTable->GetMtType(tSpeaker.GetMtId()))
						 {
							StopSwitchToSubMt(1, &tMtLocalSpeaker, MODE_AUDIO);
						 }				 
					 }
				  }
			 }	
	
			//zjj [29/08/2009] 如果上次的上级的广播源是本级终端的情况下，比较和本级回传终端是否不同，
			//如果不同就恢复本级广播源（即上级MCU）到上次发言人的交换(音频)
			if( !tLastSpeaker.IsNull() && 
				tSpeaker.GetMcuId() == tLastSpeaker.GetMcuId() &&
				tSpeaker.GetMtId() != tLastSpeaker.GetMtId() 
				)
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "Mcu(%d) Last Speaker: Restore last speaker audio switch\n", tMt.GetMtId());
				TMtStatus tStatus;
				m_ptMtTable->GetMtStatus( tLastSpeaker.GetMtId(),&tStatus );

				if( !tStatus.GetSelectMt(MODE_AUDIO).IsNull() )
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcuMcuAudioInfoAck] Mt(%d) is canceled global speaker,but now sel other mt audio so cann't start switch to it.\n",
						tLastSpeaker.GetMtId());
				}
				else
				{
					//补建上级mcu到上次发言人的音频交换
					//因为要保持唇音同步如果回传终端的视频走bas，那么音频也走bas，否则建直接的交换					
					if( g_cMcuVcApp.IsVASimultaneous() && IsNeedAdapt(GetLocalVidBrdSrc(), tMtLocalLastSpeaker, MODE_VIDEO) &&							  
							m_ptMtTable->GetMtLogicChnnl(tLastSpeaker.GetMtId(), LOGCHL_VIDEO, &tVidChn, TRUE) &&
							!IsNeedAdapt(GetLocalAudBrdSrc(), tMtLocalLastSpeaker, MODE_AUDIO) 
							)
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "Mcu(%d) Start bas To Mt(%d,%d) Audio Switch\n",tMt.GetMtId(),
									m_ptMtTable->GetMt(tLastSpeaker.GetMtId()).GetMcuId(),
									m_ptMtTable->GetMt(tLastSpeaker.GetMtId()).GetMtId()
											);
						//continue;
						//StartSwitchAud2MtNeedAdp( tLastSpeaker.GetMtId() );
						StartSwitchAud2MtNeedAdp( GetAudBrdSrc(), tMtLocalLastSpeaker);
					}
					else
					{
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "Mcu(%d) Start m_tAudBrdSrc To Mt(%d,%d) Audio Switch\n",tMt.GetMtId(),
									m_ptMtTable->GetMt(tLastSpeaker.GetMtId()).GetMcuId(),
									m_ptMtTable->GetMt(tLastSpeaker.GetMtId()).GetMtId()
											);
						RestoreRcvMediaBrdSrc(1,&tMtLocalLastSpeaker,MODE_AUDIO);
					}
				}				
				ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.SetMtId( tSpeaker.GetMtId() );
			}	           
        }
        else
        {
			//如果上级的全局发言人不是本级的终端就取消回传源的音视频交换，
			//并补建上次本级的全局发言人的音频交换，再将回传源设置空
			if( ptConfAudioInfo->m_byMixerCount > 0 &&
				LOCAL_MCUID != ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMcuId() )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAudioInfoAck]Mcu(%d) Now Speaker(%d,%d) is mmcu's Speaker,Cancel SpyMt(%d,%d) \n", tMt.GetMtId(),
							 ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMcuId(),
							 ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMtId(),
							 tSpeaker.GetMcuId(),
							 tSpeaker.GetMtId()
							 );				

				//回传源不为空，将回传源的回传的音视频交换拆除，并补建到回传源音频
				if( !tSpeaker.IsNull() && tSpeaker.IsMcuIdLocal() )
				{				
					if( MT_TYPE_SMCU == m_ptMtTable->GetMtType(tSpeaker.GetMtId()) )
					{
						if( ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMcuId() != tSpeaker.GetMtId() ||
							0 == ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMtId() 
							)
						{
							ptConfAudioInfo->m_tMixerList[0].m_tSpeaker = tSpeaker;
						}							
						SendMsgToMt( tSpeaker.GetMtId(), MCU_MCU_AUDIOINFO_ACK, cServMsg );		
					}
					else
					{
						TMtStatus tStatus;
						m_ptMtTable->GetMtStatus( tSpeaker.GetMtId(),&tStatus );

						if( !tStatus.GetSelectMt(MODE_AUDIO).IsNull() )
						{
							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcuMcuAudioInfoAck] Mt(%d) is canceled global speaker,but now sel other mt audio so cann't start switch to it.\n",
								tSpeaker.GetMtId());
						}
						else
						{
							//补建上级mcu到上次发言人的音频交换
							//因为要保持唇音同步如果回传终端的视频走bas，那么音频也走bas，否则建直接的交换							
							if( g_cMcuVcApp.IsVASimultaneous() && IsNeedAdapt(GetLocalVidBrdSrc(), tMtLocalSpeaker, MODE_VIDEO) &&							  
									m_ptMtTable->GetMtLogicChnnl(tSpeaker.GetMtId(), LOGCHL_VIDEO, &tVidChn, TRUE) &&
									!IsNeedAdapt(GetLocalAudBrdSrc(), tMtLocalSpeaker, MODE_AUDIO) 
									)
							{
								ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "Mcu(%d) Start bas To Mt(%d,%d) Audio Switch\n",tMt.GetMtId(),
											m_ptMtTable->GetMt(tSpeaker.GetMtId()).GetMcuId(),
											m_ptMtTable->GetMt(tSpeaker.GetMtId()).GetMtId()
													);
								
								//continue;
								//StartSwitchAud2MtNeedAdp( tSpeaker.GetMtId() );
								StartSwitchAud2MtNeedAdp( GetAudBrdSrc(), tMtLocalSpeaker);
							}	
							else
							{
								RestoreRcvMediaBrdSrc(1,&tMtLocalSpeaker,MODE_AUDIO);
							}
						}
					}					
				}
				if( !tSpeaker.IsNull() && !tSpeaker.IsMcuIdLocal() )
				{
					if ( ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMcuId() ==  m_tCascadeMMCU.GetMtId() 
							&& ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMtId() == 0 )
					{
						ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.SetMcuId( LOCAL_MCUID );
						ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.SetMtId( (u8)tSpeaker.GetMcuId() );
					}
					SendMsgToMt( (u8)tSpeaker.GetMcuId(), MCU_MCU_AUDIOINFO_ACK, cServMsg );			
				}
			}
		}
	}
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuAudioInfoAck]Mcu(%d) tMt(%d,%d) m_tAudBrdSrc(%d,%d) m_tCascadeMMCU(%d,%d)\n", tMt.GetMtId(),
			tMt.GetMcuId(),tMt.GetMtId(),
			GetAudBrdSrc().GetMcuId(),GetAudBrdSrc().GetMtId(),
			m_tCascadeMMCU.GetMcuId(),m_tCascadeMMCU.GetMtId()
			);

		if( tMt == m_tCascadeMMCU && tSpeaker == tLastSpeaker && !tSpeaker.IsNull() && !tSpeaker.IsMcuIdLocal() )
		{		
			if ( ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMcuId() ==  m_tCascadeMMCU.GetMtId() 
					&& ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.GetMtId() == 0 )
			{
				ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.SetMcuId( LOCAL_MCUID );
				ptConfAudioInfo->m_tMixerList[0].m_tSpeaker.SetMtId( (u8)tSpeaker.GetMcuId() );
			}
			SendMsgToMt( (u8)tSpeaker.GetMcuId(), MCU_MCU_AUDIOINFO_ACK, cServMsg );				
		}
	}

	
	
}

/*====================================================================
    函数名      ：GetLocalAudioInfo
    功能        ：获取音频信息
    算法实现    ：
    引用全局变量：
    输入参数说明：TCConfAudioInfo &tInfo [Out]
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/05/14    4.0         顾振华         创建
====================================================================*/
void CMcuVcInst::GetLocalAudioInfo(TCConfAudioInfo &tAudioInfo,TMsgHeadMsg &tHeadMsg)
{
    tAudioInfo.m_byMixerCount = 0;
    tAudioInfo.m_byDefMixerIndex = 0;
    tAudioInfo.m_tMixerList[0].m_tSpeaker.SetNull();
	TMt tSpeaker = m_tConf.GetSpeaker();
    if(!m_tConf.GetSpeaker().IsNull())
    {
		// [pengjie 2010/12/15] 如果是下级mcu发言，则尝试取其上传源通知下级
		if( tSpeaker.IsLocal() && (tSpeaker.GetMtType() == MT_TYPE_SMCU) )
		{
			u16 wSmcuIdx = GetMcuIdxFromMcuId( tSpeaker.GetMtId() );
			TConfMcInfo * ptMcuMcInfo = m_ptConfOtherMcTable->GetMcInfo( wSmcuIdx );
			if( NULL != ptMcuMcInfo && !ptMcuMcInfo->m_tMMcuViewMt.IsNull() ) 
			{
				tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_nMixerID = m_dwSpeakerAudioId;
				tHeadMsg.m_tMsgSrc = BuildMultiCascadeMtInfo( ptMcuMcInfo->m_tMMcuViewMt, \
					tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_tSpeaker);
			}
		}
		// End
		else
		{
			tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_nMixerID = m_dwSpeakerAudioId;
			tHeadMsg.m_tMsgSrc = BuildMultiCascadeMtInfo( tSpeaker,
				tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_tSpeaker);
		}

        //tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_tSpeaker = m_tConf.GetSpeaker();
		//zjj 20090903传给下级mcu audioinfo ack时给下级的发言人信息非local的就行了
		//下级mcu的mtadp会转成下级mcu的本地终端信息，所以这里这段代码注掉
        //if ( !m_tConf.GetSpeaker().IsLocal() )
        //{
            
            /*            
            TConfMcInfo* ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(byMcuId);
            if (ptConfMcInfo != NULL)
            {
                TMt tRealSpeaker = ptConfMcInfo->m_tSpyMt;
                if (!tRealSpeaker.IsNull())
                {
                tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_tSpeaker = tRealSpeaker;
                }  
                }
            */
		
			//u8 byMcuId = m_tConf.GetSpeaker().GetMcuId();
            //TMt tRealSpeaker = GetMcuMediaSrc(byMcuId);
            //if (!tRealSpeaker.IsNull())
            //{
            //    tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_tSpeaker = tRealSpeaker;
            //}  
        //}
		
        tAudioInfo.m_byMixerCount++;
    }
    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[GetLocalAudioInfo] Local Speaker: (%d, %d)\n", 
        tAudioInfo.m_tMixerList[0].m_tSpeaker.GetMcuId(), 
        tAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId() );
    
    if(m_tConf.GetStatus().IsMixing())
    {
        tAudioInfo.m_byDefMixerIndex = tAudioInfo.m_byMixerCount;
        
        //zbq[11/01/2007] 混音优化
        //TMixParam tParam = m_tConf.GetStatus().GetMixParam();
        //tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_tSpeaker = tParam.m_atMtMember[0];
        tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_nMixerID = m_dwMixerAudioId;
		tSpeaker = GetLocalSpeaker();
		tSpeaker = GetMcuIdMtFromMcuIdxMt( tSpeaker );
        tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_tSpeaker = tSpeaker;//GetLocalSpeaker();
        tAudioInfo.m_byMixerCount++;
        
        ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[GetLocalAudioInfo] Local Mixer: (%d, %d)\n", 
            tAudioInfo.m_tMixerList[1].m_tSpeaker.GetMcuId(), 
            tAudioInfo.m_tMixerList[1].m_tSpeaker.GetMtId() );
    }
}

/*====================================================================
    函数名      ：ProcMcuMcuAudioInfoReq
    功能        ：对方mcu请求视频信息
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/24    3.6         Jason         创建
====================================================================*/
void CMcuVcInst::ProcMcuMcuAudioInfoReq(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt  tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	
	STATECHECK;	

	if( !m_tCascadeMMCU.IsNull() &&
		!m_tConf.GetStatus().IsMixing() &&
		m_tConf.GetSpeaker() == m_tCascadeMMCU )
	{
		return;
	}
	TCConfAudioInfo tAudioInfo; 
	TMsgHeadMsg tHeadMsg;
    GetLocalAudioInfo( tAudioInfo,tHeadMsg );

	cServMsg.SetMsgBody((u8 *)&tAudioInfo, sizeof(tAudioInfo));
	cServMsg.CatMsgBody((u8 *)&tHeadMsg, sizeof(tHeadMsg));
	SendMsgToMt( tMt.GetMtId(), MCU_MCU_AUDIOINFO_ACK,  cServMsg);
}

/*====================================================================
函数名      ：NotifyAllSMcuMediaInfo
功能        ：主动通知各个级联下级MCU本级媒体源
算法实现    ：
引用全局变量：
输入参数说明：byTargetMcuId     要通知的下级MCU，为0则是全部
byMode            MODE_AUDIO, MODE_VIDEO, MODE_BOTH
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
07/05/14    4.0         顾振华         创建
====================================================================*/
void CMcuVcInst::NotifyAllSMcuMediaInfo(u8 byTargetMcuId, u8 byMode)
{
    CServMsg cServMsg;
	//u8 byMcuId = 0,bySMcuId = 0;
	u8 abyMcuId[MAX_CASCADEDEPTH-1];
	memset( &abyMcuId[0],0,sizeof(abyMcuId) );
    if ( MODE_AUDIO == byMode || MODE_BOTH == byMode )
    {
        TCConfAudioInfo tAudioInfo; 
		TMsgHeadMsg tHeadMsg;
        GetLocalAudioInfo( tAudioInfo,tHeadMsg );  
        cServMsg.SetEventId(MCU_MCU_AUDIOINFO_ACK);
        cServMsg.SetMsgBody((u8 *)&tAudioInfo, sizeof(tAudioInfo));
		cServMsg.CatMsgBody((u8 *)&tHeadMsg, sizeof(tHeadMsg));
        
        u16 wMcuIdx = 0;
        for( u16 wLoop = 0; wLoop < m_tConfAllMtInfo.GetMaxMcuNum(); wLoop++ )
        {
            wMcuIdx = m_tConfAllMtInfo.GetMtInfo(wLoop).GetMcuIdx();
			if( IsValidMcuId(wMcuIdx) 
				&& m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) 
				&& m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0])
				)
			{
				if( abyMcuId[0] != 0 
					&& m_tCascadeMMCU.GetMtId() != abyMcuId[0] 
					&& (byTargetMcuId == 0 || byTargetMcuId == abyMcuId[0] ) )
				{
					SendMsgToMt( abyMcuId[0], MCU_MCU_AUDIOINFO_ACK, cServMsg );
				}
			}
        }
    }
    if ( MODE_VIDEO == byMode || MODE_BOTH == byMode )
    {
        cServMsg.SetEventId(MCU_MCU_VIDEOINFO_ACK);
        TCConfViewInfo tInfo;
        u16 wMcuIdx = 0;

		// liuxu, m_tConfAllMtInfo的相关内容修改
        for( u16 wLoop = 0; wLoop < m_tConfAllMtInfo.GetMaxMcuNum(); wLoop++ )
        {
            wMcuIdx = m_tConfAllMtInfo.GetMtInfo(wLoop).GetMcuIdx();
			
			if( IsValidSubMcuId(wMcuIdx) &&
				m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) &&
				m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0] )
				)
			{
				if( abyMcuId[0] != 0 
					&& m_tCascadeMMCU.GetMtId() != abyMcuId[0] 
					&& (byTargetMcuId == 0 || byTargetMcuId == abyMcuId[0] ) )
				{
					GetConfViewInfo(wMcuIdx, tInfo);
					cServMsg.SetMsgBody((u8 *)&tInfo, sizeof(tInfo));
					SendMsgToMt( abyMcuId[0], MCU_MCU_VIDEOINFO_ACK, cServMsg );
				}
			}
        }        
    }
}

/*====================================================================
函数名      ：NofityMMcuMediaInfo
功能        ：主动通知上级MCU本级媒体源
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
07/05/15    4.0         顾振华         创建
====================================================================*/
void CMcuVcInst::NofityMMcuMediaInfo()
{
    if (m_tCascadeMMCU.IsNull())
    {
        return;
    }
    
    CServMsg cMsg;
    
    TCConfAudioInfo tAudioInfo;
	TMsgHeadMsg tHeadMsg;
    GetLocalAudioInfo(tAudioInfo,tHeadMsg);
    cMsg.SetEventId(MCU_MCU_AUDIOINFO_ACK);
    cMsg.SetMsgBody((u8 *)&tAudioInfo, sizeof(tAudioInfo));
    SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_AUDIOINFO_ACK, cMsg );
    
    TCConfViewInfo tVideoInfo;                
    GetConfViewInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()), tVideoInfo);
    cMsg.SetEventId(MCU_MCU_VIDEOINFO_ACK);
    cMsg.SetMsgBody((u8 *)&tVideoInfo, sizeof(tVideoInfo));
    SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_VIDEOINFO_ACK, cMsg );
}

/*====================================================================
    函数名      ：ProcMcuMcuMtOperateRsp
    功能        ：级联终端呼叫，挂断，删除，邀请，请求回传终端响应处理
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/21    3.6         Jason         创建
====================================================================*/
void CMcuVcInst::ProcMcuMcuMtOperateRsp( const CMessage *pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMsgHeadMsg tHeadMsg = *( TMsgHeadMsg* )(cServMsg.GetMsgBody());

	STATECHECK;

	if( !m_tCascadeMMCU.IsNull() &&
		tHeadMsg.m_tMsgSrc.m_byCasLevel < MAX_CASCADELEVEL &&
		tHeadMsg.m_tMsgDst.m_byCasLevel > 0
		)
	{
		SendMsgToMt( m_tCascadeMMCU.GetMtId(),cServMsg.GetEventId(),cServMsg );
	}

}
/*====================================================================
    函数名      SendAllMtInfoToAllMcs
    功能        ：请求对方mcu终端列表回应
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/21    3.6         Jason         创建
	10/12/30    4.6         liuxu         修改
====================================================================*/
/*lint -save -e850*/
void CMcuVcInst::SendAllMtInfoToAllMcs( u16 wEvent, CServMsg & cServMsg, const BOOL32 bForce/* = FALSE*/ )
{
	if( MCU_MCS_CONFALLMTINFO_NOTIF != wEvent )
	{
		return;
	}
	
	if (CurState() != STATE_ONGOING)
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "SendAllMtInfoToAllMcs called at wrong Vc state.%d, return\n", CurState());
		return;
	}

	// 对SendAllMtInfoToAllMcs采取缓冲发送
	if ( g_cMcuVcApp.GetMcuMcsMtInfoBufFlag() != 0					// 启用了缓冲发送
		&& !bForce)
	{
		// [11/14/2011 liuxu] 由于大容量时SendAllMtInfoToAllMcs消息量太大, 这里采取缓冲发送
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MCS, "Buf Send AllMtInfoToAllMcs\n");

		if (m_byAllMtToAllMcsTimerFlag)
		{
			//SetTimer(MCUVC_SENDALLMT_ALLMCS_TIMER, 500);
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "SendAllMtInfoToAllMcs wait timer, return\n");
			return;
		}else
		{
			m_byAllMtToAllMcsTimerFlag = 1;
			u16 wTemp = SetTimer(MCUVC_SENDALLMT_ALLMCS_TIMER, g_cMcuVcApp.GetRefreshMtInfoInterval());
			return;
		}
	}else
	{
		KillTimer(MCUVC_SENDALLMT_ALLMCS_TIMER);
		m_byAllMtToAllMcsTimerFlag = 0;
	}
	
	u8 byPos = 0;
	cServMsg.SetMsgBody( ( u8 * )&byPos,sizeof(byPos) );
	cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ));
	cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo.m_tLocalMtInfo, sizeof( TConfMtInfo ) );
	cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo.m_tMMCU, sizeof( TMt ) );
	//cServMsg.CatMsgBody( (u8*)&m_tConfAllMtInfo.m_atOtherMcMtInfo[wLoop],sizeof(TConfMtInfo) * byPackSize );
	SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF,cServMsg );
// 	m_tConfAllMtInfo.Print(); // test [pengguofeng 7/8/2013]
	++byPos;
	
	const u16 wMaxPackSize = 100;						// 分包发送, 每次最大发送100个TConfMtInfo
	u16 wSendPackSize = 0;								// 实际发送的TConfMtInfo的个数

	u16 wLoop = 0;
	const u16 wMaxMcuNum = m_tConfAllMtInfo.GetMaxMcuNum();
	if( wMaxMcuNum > wMaxPackSize )
	{
		for( wLoop = 0; wLoop < wMaxMcuNum; wLoop += wMaxPackSize )
		{
			// 包的编号
			cServMsg.SetMsgBody( ( u8 * )&byPos, sizeof(byPos) );
			
			// TConfMtInfo的起始位置
			wLoop = htons(wLoop);
			cServMsg.CatMsgBody( ( u8 * )&wLoop, sizeof(u16) );
			
			// 计算实际发送的包数
			wLoop = ntohs(wLoop);
			wSendPackSize = wMaxMcuNum - wLoop;
			if( wSendPackSize >= wMaxPackSize )
			{
				wSendPackSize = wMaxPackSize;
			}		

			// 空指针保护
			if (NULL == m_tConfAllMtInfo.GetMtInfoPtr(wLoop))
			{
				break;
			}

			cServMsg.CatMsgBody( (u8*)m_tConfAllMtInfo.GetMtInfoPtr(wLoop), sizeof(TConfMtInfo) * wSendPackSize );
			SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF,cServMsg );
			
			++byPos;
		}
	}
	else
	{
		cServMsg.SetMsgBody( ( u8 * )&byPos,sizeof(byPos) );
		wLoop = htons(wLoop);
		cServMsg.CatMsgBody( ( u8 * )&wLoop,sizeof(u16) );
		cServMsg.CatMsgBody( (u8*)m_tConfAllMtInfo.GetMtInfoPtr(0),sizeof(TConfMtInfo) * wMaxMcuNum );
		SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF,cServMsg );
	}

	// 发送缓冲 // 不用多发 [pengguofeng 7/5/2013]
/*	u8 abyMcuIdx[2] = {0};
	for ( u16 wMcuIdx = 0; wMcuIdx < MAXNUM_CONFSUB_MCU; wMcuIdx++)
	{
		if ( m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuIdx[0] )) 
			SendMtListToMcs(wMcuIdx, TRUE);
	}

 	SendMtListToMcs(LOCAL_MCUIDX, TRUE);
*/
/*
	if (m_cMcuMtListSendBuf.GetUsedNum())
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "Start Buf Send m_cMcuMtListSendBuf(num:%d)\n", m_cMcuMtListSendBuf.GetUsedNum());

		u16 wMcuIdx = INVALID_MCUIDX;		
		const u32 dwMaxBufNum = m_cMcuMtListSendBuf.GetCapacity();
		
		for ( u32 dwLoop = 0; dwLoop < dwMaxBufNum; dwLoop++ )
		{
			if(!m_cMcuMtListSendBuf.Get(dwLoop, wMcuIdx))
				continue;
			
			// 有时候mcu会定时刷新所有本地终端的状态, 是通过在缓冲区插入一个空Tmt标记的
			// 同一个本地终端有空能被刷新多次, 不影响性能和功能
			if (IsValidMcuId(wMcuIdx)) 
			{
				SendMtListToMcs(wMcuIdx, TRUE);
				continue;
			}
		}
		
		m_cMcuMtListSendBuf.ClearAll();
	}
	else
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MCS, "[SendAllMtInfoToAllMcs]m_cMcuMtListSendBuf.GetUsedNum() == 0\n");
	}
*/
}
/*lint -restore*/

/*====================================================================
    函数名      ：SendAllSMcuMtStatusToMMcu
    功能        ：发送所有下级终端状态给上级
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    10/08/17    4.6         周晶晶         创建
====================================================================*/
void CMcuVcInst::SendAllSMcuMtStatusToMMcu( void )
{
	if( m_tCascadeMMCU.IsNull() || !IsMMcuSupportMultiCascade() )
	{
		return;
	}
	u8 byLoop;
	u16 wMcuIdx = INVALID_MCUIDX;
	u8 byMcuId = 0;
	for( byLoop = 1;byLoop <= MAXNUM_CONF_MT;++byLoop )
	{
		if( MT_TYPE_SMCU == m_ptMtTable->GetMtType(byLoop) )
		{
			byMcuId = byLoop;
			if( m_tConfAllMcuInfo.GetIdxByMcuId(&byMcuId,1,&wMcuIdx) )
			{
				SendSMcuMtStatusToMMcu( wMcuIdx );
			}
		}
	}
}

/*====================================================================
    函数名      ：SendSMcuMtStatusToMMcu
    功能        ：发送某个下级终端状态给上级
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    10/08/17    4.6         周晶晶         创建
====================================================================*/
void CMcuVcInst::SendSMcuMtStatusToMMcu( u16 wMcuIdx )
{
	if( m_tCascadeMMCU.IsNull() || !IsMMcuSupportMultiCascade() )
	{
		return;
	}
	u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
	memset( &abyMcuId[0],0,sizeof(abyMcuId) );

	if( !m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0]) )
	{
		return;
	}

	if( 0 == abyMcuId[0] || 0 != abyMcuId[1] )
	{
		return;
	}


	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
	if( NULL == ptMcInfo )
	{
		return;
	}

	s32 nIndex = 0;
	TSMcuMtStatus tSMcuMtStatus;
	u8 byMcuId = GetFstMcuIdFromMcuIdx( wMcuIdx );
	CServMsg cServMsg;
	CServMsg cMsg;
	u8 byMtNum = 0;
	cMsg.SetDstMtId( m_tCascadeMMCU.GetMtId() );
	cMsg.SetEventId( MCU_MCU_MTSTATUS_NOTIF );

	for(; nIndex<MAXNUM_CONF_MT; nIndex++)
	{
		if( ptMcInfo->m_atMtStatus[nIndex].IsNull() || 
			ptMcInfo->m_atMtStatus[nIndex].GetMtId() == 0 ) //自己
		{
			continue;
		}	
		memset( &tSMcuMtStatus,0,sizeof(tSMcuMtStatus) );
		memcpy( (void*)&tSMcuMtStatus, (void*)&ptMcInfo->m_atMtStatus[nIndex], sizeof(TMt) );
		tSMcuMtStatus.SetMcuId( byMcuId );
		tSMcuMtStatus.SetIsEnableFECC( ptMcInfo->m_atMtStatus[nIndex].IsEnableFECC() );
        tSMcuMtStatus.SetCurVideo( ptMcInfo->m_atMtStatus[nIndex].GetCurVideo() );
        tSMcuMtStatus.SetCurAudio( ptMcInfo->m_atMtStatus[nIndex].GetCurAudio() );
        tSMcuMtStatus.SetMtBoardType( ptMcInfo->m_atMtStatus[nIndex].GetMtBoardType() );
        tSMcuMtStatus.SetIsMixing( ptMcInfo->m_atMtStatus[nIndex].IsInMixing() );
        tSMcuMtStatus.SetVideoLose( ptMcInfo->m_atMtStatus[nIndex].IsVideoLose() );
		BOOL32 bIsAuto = ( ptMcInfo->m_atMtExt[nIndex].GetCallMode() == CONF_CALLMODE_TIMER) ? TRUE : FALSE;
		tSMcuMtStatus.SetIsAutoCallMode( bIsAuto );
		tSMcuMtStatus.SetRecvVideo( ptMcInfo->m_atMtStatus[nIndex].IsReceiveVideo() );
		tSMcuMtStatus.SetRecvAudio( ptMcInfo->m_atMtStatus[nIndex].IsReceiveAudio() );
		tSMcuMtStatus.SetSendVideo( ptMcInfo->m_atMtStatus[nIndex].IsSendVideo() );
		tSMcuMtStatus.SetSendAudio( ptMcInfo->m_atMtStatus[nIndex].IsSendAudio() );
		tSMcuMtStatus.SetDisconnectReason( ptMcInfo->m_atMtStatus[nIndex].GetMtDisconnectReason() );
		tSMcuMtStatus.SetSendVideo2(ptMcInfo->m_atMtStatus[nIndex].IsSndVideo2());


		if( 0 == byMtNum )
		{
			cServMsg.SetMsgBody( (u8*)&tSMcuMtStatus,sizeof(tSMcuMtStatus) );
		}
		else
		{
			cServMsg.CatMsgBody( (u8*)&tSMcuMtStatus,sizeof(tSMcuMtStatus) );
		}
		++byMtNum;
	}


	if( byMtNum > 0 )
	{		
		cMsg.SetMsgBody( (u8*)&byMtNum,sizeof(u8) );
		cMsg.CatMsgBody( cServMsg.GetMsgBody(),cServMsg.GetMsgBodyLen() );
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "send %u mtstatus of smcu.%d to mmcu\n", byMtNum, wMcuIdx);
		SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_MTSTATUS_NOTIF, cMsg );
	}

	
        
        /*if( m_tConfAllMtInfo.MtInConf(byMtId) && byDstMcuId != byMtId )
        {		
            m_ptMtTable->GetMtStatus( byMtId, &tLocalMtStatus );
            memcpy( (void*)&tSMcuMtStatus, (void*)&tLocalMtStatus, sizeof(TMt) );
            tSMcuMtStatus.SetIsEnableFECC( tLocalMtStatus.IsEnableFECC() );
            tSMcuMtStatus.SetCurVideo( tLocalMtStatus.GetCurVideo() );
            tSMcuMtStatus.SetCurAudio( tLocalMtStatus.GetCurAudio() );
            tSMcuMtStatus.SetMtBoardType( tLocalMtStatus.GetMtBoardType() );
            tSMcuMtStatus.SetIsMixing( tLocalMtStatus.IsInMixing() );
            tSMcuMtStatus.SetVideoLose( tLocalMtStatus.IsVideoLose() );
			tSMcuMtStatus.SetIsAutoCallMode(CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode(byMtId));
			tSMcuMtStatus.SetRecvVideo( tLocalMtStatus.IsReceiveVideo() );
			tSMcuMtStatus.SetRecvAudio( tLocalMtStatus.IsReceiveAudio() );
			tSMcuMtStatus.SetSendVideo( tLocalMtStatus.IsSendVideo() );
			tSMcuMtStatus.SetSendAudio( tLocalMtStatus.IsSendAudio() );

			//zjj20100327
			tSMcuMtStatus.SetMtDisconnectReason( tLocalMtStatus.GetMtDisconnectReason() );

			tSMcuMtStatus.SetMcuIdx( LOCAL_MCUID );
            if( 0 == byMtNum )
            {
                cServMsg.SetMsgBody( (u8*)&tSMcuMtStatus, sizeof(TSMcuMtStatus) );
            }
            else
            {
                cServMsg.CatMsgBody( (u8*)&tSMcuMtStatus, sizeof(TSMcuMtStatus) );
            }
            byMtNum++;
        }*/




}
/*====================================================================
    函数名      ：SendSMcuMtInfoToMMcu
    功能        ：发送所有下级终端列表给上级
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    10/07/28    4.6         周晶晶         创建
====================================================================*/
void CMcuVcInst::SendAllSMcuMtInfoToMMcu()
{
	if( m_tCascadeMMCU.IsNull() || !IsMMcuSupportMultiCascade() )
	{
		return;
	}
	u8 byLoop;
	u16 wMcuIdx = INVALID_MCUIDX;
	u8 byMcuId = 0;
	for( byLoop = 1;byLoop <= MAXNUM_CONF_MT;++byLoop )
	{
		if( MT_TYPE_SMCU == m_ptMtTable->GetMtType(byLoop) )
		{
			byMcuId = byLoop;
			if( m_tConfAllMcuInfo.GetIdxByMcuId(&byMcuId,1,&wMcuIdx) )
			{
				SendSMcuMtInfoToMMcu( wMcuIdx );
			}
		}
	}
}

/*====================================================================
    函数名      ：SendSMcuMtInfoToMMcu
    功能        ：发送某下级终端列表给上级
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    10/07/28    4.6         周晶晶         创建
====================================================================*/
void CMcuVcInst::SendSMcuMtInfoToMMcu( u16 wMcuIdx )
{
	if( m_tCascadeMMCU.IsNull() || !IsMMcuSupportMultiCascade() )
	{
		return;
	}

	u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
	memset( &abyMcuId[0],0,sizeof(abyMcuId) );

	if( !m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0]) )
	{
		return;
	}

	if( 0 == abyMcuId[0] || 0 != abyMcuId[1] )
	{
		return;
	}

	TMcuMcuMtInfo atInfo[MAXNUM_CONF_MT];

	u8 byInfoNum = 0;
	

	//请求终端列表
	TConfMcInfo *ptMcMtInfo = m_ptConfOtherMcTable->GetMcInfo( wMcuIdx );
    
	for(u8 byLoop = 0; byLoop < MAXNUM_CONF_MT; byLoop++)
	{	
		if (NULL == ptMcMtInfo)
		{
			continue;
		}
		if(	ptMcMtInfo->m_atMtExt[byLoop].IsNull() &&
			!( ptMcMtInfo->m_atMtStatus[byLoop].GetMtType() == MT_TYPE_MT && 
			 0 == ptMcMtInfo->m_atMtStatus[byLoop].GetMtId() )
			)
		{
			continue;
		}
		atInfo[byInfoNum].m_tMt = (TMt)ptMcMtInfo->m_atMtExt[byLoop];
		atInfo[byInfoNum].m_tMt.SetMcuId( abyMcuId[0] );
		strncpy((s8 *)(atInfo[byInfoNum].m_szMtName), ptMcMtInfo->m_atMtExt[byLoop].GetAlias(), sizeof(atInfo[byInfoNum].m_szMtName) - 1);
		atInfo[byInfoNum].m_dwMtIp   = htonl( ptMcMtInfo->m_atMtExt[byLoop].GetIPAddr() );
		atInfo[byInfoNum].m_byMtType = ptMcMtInfo->m_atMtExt[byLoop].GetMtType();
		atInfo[byInfoNum].m_byManuId = ptMcMtInfo->m_atMtExt[byLoop].GetManuId();
        atInfo[byInfoNum].m_byProtocolType = ptMcMtInfo->m_atMtExt[byLoop].GetProtocolType();
		
		atInfo[byInfoNum].m_byVideoIn = ptMcMtInfo->m_atMtStatus[byLoop].GetVideoIn();
		atInfo[byInfoNum].m_byVideoOut = ptMcMtInfo->m_atMtStatus[byLoop].GetVideoOut();

		atInfo[byInfoNum].m_byAudioIn = ptMcMtInfo->m_atMtStatus[byLoop].GetAudioIn();
		atInfo[byInfoNum].m_byAudioOut = ptMcMtInfo->m_atMtStatus[byLoop].GetAudioOut();

		atInfo[byInfoNum].m_byIsAudioMuteIn = ptMcMtInfo->m_atMtStatus[byLoop].IsSendAudio() ? 0 : 1;
		atInfo[byInfoNum].m_byIsAudioMuteOut = ptMcMtInfo->m_atMtStatus[byLoop].IsReceiveAudio() ? 0 : 1;

		atInfo[byInfoNum].m_byIsVideoMuteIn = ptMcMtInfo->m_atMtStatus[byLoop].IsSendVideo() ? 0 : 1;
		atInfo[byInfoNum].m_byIsVideoMuteOut = ptMcMtInfo->m_atMtStatus[byLoop].IsReceiveVideo() ? 0 : 1;
		
		TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo( wMcuIdx );
		atInfo[byInfoNum].m_byIsConnected = tMtInfo.MtJoinedConf( atInfo[byInfoNum].m_tMt.GetMtId() );

		atInfo[byInfoNum].m_byIsFECCEnable = ptMcMtInfo->m_atMtStatus[byLoop].IsEnableFECC() ? 1 : 0;

		atInfo[byInfoNum].m_tPartVideoInfo = ptMcMtInfo->m_atMtStatus[byLoop].GetMtVideoInfo();
    
	
		byInfoNum++;	
	}

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[SendSMcuMtInfoToMMcu] Notified McuID.%d LocalTotalMtNum.%d\n", m_tCascadeMMCU.GetMtId(), byInfoNum );

	if( 0 == byInfoNum )
	{
		return;
	}

	u8 byLastPack = 1;
	CServMsg cServMsg;
	cServMsg.SetEventId( MCU_MCU_MTLIST_ACK );
	cServMsg.SetMsgBody(&byLastPack, sizeof(u8));
	cServMsg.CatMsgBody((u8 *)atInfo, byInfoNum * sizeof(TMcuMcuMtInfo));
	SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_MTLIST_ACK,  cServMsg);

	//级联上级MCU后的第一次下级MCU已有终端状态强制通知，之后的个别终端状态进行实时通知
	/*if( m_tConfInStatus.IsNtfMtStatus2MMcu() ||
		// vcs刷列表，同时需要刷新其状态信息，由于其中包含重要的终端呼叫模式信息
		VCS_CONF == m_tConf.GetConfSource())
	{
		OnNtfMtStatusToMMcu( m_tCascadeMMCU.GetMtId() );
		m_tConfInStatus.SetNtfMtStatus2MMcu(FALSE);
	}
	*/


}
/*====================================================================
    函数名      ：SendSMcuUpLoadMtToMMcu
    功能        ：发送指定MCU的上传通道终端给MMCU
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    12/01/04    4.7         田志勇         创建
====================================================================*/
void CMcuVcInst::SendSMcuUpLoadMtToMMcu()
{
	TSetOutParam tOutParam;
	TMsgHeadMsg tHeadMsg;
	CServMsg cServMsg;
	u8 byMcuId = 0;
	u16 wMcuIdx = INVALID_MCUIDX;
	TMt tSetoutMt;
	for( u8 byLoop = 1;byLoop <= MAXNUM_CONF_MT;++byLoop )
	{
		if( MT_TYPE_SMCU == m_ptMtTable->GetMtType(byLoop) )
		{
			byMcuId = byLoop;
			if( m_tConfAllMcuInfo.GetIdxByMcuId(&byMcuId,1,&wMcuIdx) )
			{
				TConfMcInfo * ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
				if(NULL != ptMcInfo && !(ptMcInfo->m_tMMcuViewMt.IsNull()))
				{
					// 初始化
					memset(&tOutParam, 0, sizeof(tOutParam));
					memset(&tHeadMsg, 0, sizeof(tHeadMsg));
					memset(&cServMsg, 0, sizeof(cServMsg));
					
					BuildMultiCascadeMtInfo(ptMcInfo->m_tMMcuViewMt, tSetoutMt);
					tOutParam.m_nMtCount = 1;
					++tHeadMsg.m_tMsgDst.m_byCasLevel; //二级代第三级发NOTIF，MsgDstLevel先加1，标识为下级上传终端
					tOutParam.m_atConfViewOutInfo[0].m_tMt = tSetoutMt;
					TConfMcInfo *ptCascadeMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() ));
					if (ptCascadeMcInfo == NULL)
					{
						return;
					}
					tOutParam.m_atConfViewOutInfo[0].m_nOutViewID = ptCascadeMcInfo->m_dwSpyViewId;
					tOutParam.m_atConfViewOutInfo[0].m_nOutVideoSchemeID = ptCascadeMcInfo->m_dwSpyVideoId;
					
					cServMsg.SetMsgBody((u8 *)&tOutParam, sizeof(tOutParam));
					cServMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
					cServMsg.SetEventId(MCU_MCU_SETOUT_NOTIF);
					SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_SETOUT_NOTIF, cServMsg );
				}
			}
		}
	}
}
/*====================================================================
    函数名      ：ProcMcuMcuMtListAck
    功能        ：请求对方mcu终端列表回应
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/21    3.6         Jason         创建
====================================================================*/
void CMcuVcInst::ProcMcuMcuMtListAck(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	u8			byIsLastPack = *(cServMsg.GetMsgBody());
	TMcuMcuMtInfo*	ptMcuMcuMtInfo = (TMcuMcuMtInfo*)(cServMsg.GetMsgBody()+sizeof(u8));
	s32			nMtNum = (cServMsg.GetMsgBodyLen()-sizeof(u8))/sizeof(TMcuMcuMtInfo);

	STATECHECK;
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "Recv %d mtlist from smcu(%d,%d)\n", nMtNum, tMt.GetMcuId(), tMt.GetMtId());

	u16 wMcuIdx = INVALID_MCUIDX;

	u8 bySecMcuId = 0;

	//  [2/23/2011 chendaiwei]标志是否是N+1创会（恢复会议或回滚创会）时，拥有下级终端做发言人
	BOOL32 bIsNPlusSmcuSpeaker = FALSE;
	//  [2/23/2011 chendaiwei]N+1创会时的下级终端发言人
	TMt tNPlusSmcuSpeaker;
	tNPlusSmcuSpeaker.SetNull();

	if( nMtNum > 0 )
	{
		if( 0 != ptMcuMcuMtInfo->m_byCasLevel &&
			ptMcuMcuMtInfo->m_byCasLevel <= MAX_CASCADELEVEL
			)
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuMtListAck] Index 0 tMt(%d.%d.%d) byCasLevel(%d)\n",
				ptMcuMcuMtInfo->m_tMt.GetMcuId(),
				ptMcuMcuMtInfo->m_tMt.GetMtId(),
				ptMcuMcuMtInfo->m_abyMtIdentify[0],
				ptMcuMcuMtInfo->m_byCasLevel
				);
			bySecMcuId = ptMcuMcuMtInfo->m_tMt.GetMtId();
		}
	}
	u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
	memset( &abyMcuId[0],0,sizeof(abyMcuId) );
	abyMcuId[0] = tMt.GetMtId();
	abyMcuId[1] = bySecMcuId;
	
	// [11/29/2011 liuxu] 防止mcu被挂断时, 它的终端列表仍然在osp消息队里里
	if ( !m_tConfAllMtInfo.MtJoinedConf(tMt) )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "FatherMcu(%d,%d) has been off-line, ignore mcumt(1st.%d, 2nd.%d)' mtlist\n",
			tMt.GetMcuId(), tMt.GetMtId(), abyMcuId[0], abyMcuId[1] = bySecMcuId);
		return;
	}

	if( !m_tConfAllMcuInfo.AddMcu( &abyMcuId[0],2,&wMcuIdx ) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU,  "[ProcMcuMcuMtListAck] Fail to Add Mcu Info.McuId(%d)\n",tMt.GetMtId() );
		return;
	}
	
	TConfMcInfo *ptMcMtInfo = m_ptConfOtherMcTable->AddMcInfo(wMcuIdx);
	if(ptMcMtInfo == NULL)
	{
		return;
	}
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "It's from %d sMcu(%d,%d)\n", wMcuIdx, abyMcuId[0], abyMcuId[1]);

	if( 0 == nMtNum && ISTRUE(byIsLastPack) && 0 == ptMcMtInfo->m_byLastPos )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU,  "[ProcMcuMcuMtListAck] nMtNum is 0 and IsLastPack and m_byLastPos is 0,it's impossible.return\n" );
		return;
	}
	
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcuMcuMtListAck] RemoteMcuID.%d LastMtNum.%d,This Pack include MtNum.%d. byIsLastPack.%d\n", 
		     tMt.GetMtId(), ptMcMtInfo->m_byLastPos, nMtNum, byIsLastPack );

	
	//全新的一包
	if(ptMcMtInfo->m_byLastPos == 0)
	{
		//部分初始化
		memset((ptMcMtInfo->m_atMtExt), 0, sizeof(ptMcMtInfo->m_atMtExt));
		//memset(&(ptMcMtInfo->m_atMtStatus), 0, sizeof(ptMcMtInfo->m_atMtStatus));
		ptMcMtInfo->SetMcuIdx( INVALID_MCUIDX );
		//zjj [08/29/2009]以下结构不要清空,在过程ProcMcuMcuAudioInfoAck中要用
		//memset(&(ptMcMtInfo->m_tConfAudioInfo), 0, sizeof(ptMcMtInfo->m_tConfAudioInfo));
		memset(&(ptMcMtInfo->m_tConfViewInfo), 0, sizeof(ptMcMtInfo->m_tConfViewInfo));
	}
	ptMcMtInfo->SetMcuIdx( wMcuIdx );

	s32 nIndex = 0;
	s32 nLoop  = 0;
	u8 byRealMtId = 0;
/*	BOOL32 bIsCanNotifyToMMcu = FALSE;
	TMtExtU atMtExtU[MAXNUM_CONF_MT]; //  [pengguofeng 7/9/2013]上报界面的结构体
	memset(atMtExtU, 0, sizeof(atMtExtU));
*/
	for(nIndex = ptMcMtInfo->m_byLastPos, nLoop =0; nIndex<ptMcMtInfo->m_byLastPos+nMtNum&&nIndex<MAXNUM_CONF_MT; nIndex++, nLoop++)
	{
		ptMcMtInfo->m_atMtStatus[nIndex].SetAudioIn(ptMcuMcuMtInfo[nLoop].m_byAudioIn);
		ptMcMtInfo->m_atMtStatus[nIndex].SetAudioOut(ptMcuMcuMtInfo[nLoop].m_byAudioOut);
		BOOL32 bSendAudio = !ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsAudioMuteIn);
		ptMcMtInfo->m_atMtStatus[nIndex].SetSendAudio(bSendAudio);
		if(ptMcMtInfo->m_atMtStatus[nIndex].GetAudioIn() == MEDIA_TYPE_NULL)
		{
			ptMcMtInfo->m_atMtStatus[nIndex].SetSendAudio(FALSE);
		}
		BOOL32 bRecvAudio = !ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsAudioMuteOut);
		ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveAudio(bRecvAudio);
		if(ptMcMtInfo->m_atMtStatus[nIndex].GetAudioOut() == MEDIA_TYPE_NULL)
		{
			ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveAudio(FALSE);
		}
		ptMcMtInfo->m_atMtExt[nIndex].SetMt(ptMcuMcuMtInfo[nLoop].m_tMt);
		ptMcMtInfo->m_atMtStatus[nIndex].SetMt(ptMcuMcuMtInfo[nLoop].m_tMt);
#ifdef _UTF8
		// 去除被截断的汉字 [pengguofeng 5/24/2013]
		CorrectUtf8Str(ptMcuMcuMtInfo[nLoop].m_szMtName, strlen(ptMcuMcuMtInfo[nLoop].m_szMtName));
#endif
		ptMcMtInfo->m_atMtExt[nIndex].SetAlias(ptMcuMcuMtInfo[nLoop].m_szMtName);
		
		//  [2/23/2011 chendaiwei]NPlus创会，通过发言人别名检查是否有下级终端作发言人
		if( !m_tConf.HasSpeaker() && !m_tConf.GetSpeakerAlias().IsNull() &&
			memcmp(m_tConf.GetSpeakerAlias().m_achAlias,ptMcuMcuMtInfo[nLoop].m_szMtName,VALIDLEN_ALIAS) == 0 )
		{
			bIsNPlusSmcuSpeaker = TRUE;
			tNPlusSmcuSpeaker.SetMtId(ptMcuMcuMtInfo[nLoop].m_tMt.GetMtId());
			tNPlusSmcuSpeaker.SetMcuIdx(wMcuIdx);
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"Found SMCU speaker: alias == %s,McuIndex == %u, Mt Id == %u\n",m_tConf.GetSpeakerAlias().m_achAlias,tNPlusSmcuSpeaker.GetMcuId(),tNPlusSmcuSpeaker.GetMtId());
		}

		ptMcMtInfo->m_atMtExt[nIndex].SetIPAddr(ntohl(ptMcuMcuMtInfo[nLoop].m_dwMtIp));
		ptMcMtInfo->m_atMtExt[nIndex].SetMtType(ptMcuMcuMtInfo[nLoop].m_byMtType);
		ptMcMtInfo->m_atMtExt[nIndex].SetManuId(ptMcuMcuMtInfo[nLoop].m_byManuId);
        
        if(MT_MANU_KDC == ptMcuMcuMtInfo[nLoop].m_byManuId || MT_MANU_KDCMCU == ptMcuMcuMtInfo[nLoop].m_byManuId )
        {
            ptMcMtInfo->m_atMtExt[nIndex].SetProtocolType(ptMcuMcuMtInfo[nLoop].m_byProtocolType); // 取协议类型
        }

		ptMcMtInfo->m_atMtStatus[nIndex].SetMtType(ptMcuMcuMtInfo[nLoop].m_byMtType);
		ptMcMtInfo->m_atMtStatus[nIndex].SetVideoIn(ptMcuMcuMtInfo[nLoop].m_byVideoIn);
		ptMcMtInfo->m_atMtStatus[nIndex].SetVideoOut(ptMcuMcuMtInfo[nLoop].m_byVideoOut);
//		ptMcMtInfo->m_atMtStatus[nIndex].SetVideo2In(ptMcuMcuMtInfo[nLoop].m_byVideo2In);
//		ptMcMtInfo->m_atMtStatus[nIndex].SetVideo2Out(ptMcuMcuMtInfo[nLoop].m_byVideo2Out);
		BOOL32 bSendVideo = !ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsVideoMuteIn);
		ptMcMtInfo->m_atMtStatus[nIndex].SetSendVideo(bSendVideo);   
		BOOL32 bRecvVideo = !ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsVideoMuteOut);
		ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveVideo( bRecvVideo );
		if(ptMcMtInfo->m_atMtStatus[nIndex].GetVideoIn() == MEDIA_TYPE_NULL)
		{
			ptMcMtInfo->m_atMtStatus[nIndex].SetSendVideo(FALSE);    
		}
		if(ptMcMtInfo->m_atMtStatus[nIndex].GetVideoOut() == MEDIA_TYPE_NULL)
		{
			ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveVideo(FALSE);
		}
		if( 0 == ptMcuMcuMtInfo[nLoop].m_byCasLevel )
		{
			byRealMtId = ptMcuMcuMtInfo[nLoop].m_tMt.GetMtId();
		}
		else
		{
			if( ptMcuMcuMtInfo[nLoop].m_byCasLevel <= MAX_CASCADELEVEL )
			{
				byRealMtId = ptMcuMcuMtInfo[nLoop].m_abyMtIdentify[ptMcuMcuMtInfo[nLoop].m_byCasLevel-1];
			}
			else
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU,  "[ProcMcuMcuMtListAck] Error CasLevel.%d MtId(%d)\n",
					ptMcuMcuMtInfo[nLoop].m_byCasLevel,tMt.GetMtId()
					);				
			}
		}

		TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(wMcuIdx);
		tMtInfo.RemoveMt(byRealMtId);//ptMcuMcuMtInfo[nLoop].m_tMt.GetMtId());

		if(ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsConnected))
		{
			tMtInfo.AddJoinedMt(byRealMtId);//ptMcuMcuMtInfo[nLoop].m_tMt.GetMtId());

			RestoreSubMtInVmp(ptMcuMcuMtInfo[nLoop].m_tMt);
		}
		else
		{
			tMtInfo.AddMt(byRealMtId);//ptMcuMcuMtInfo[nLoop].m_tMt.GetMtId());
		}
		m_tConfAllMtInfo.SetMtInfo(tMtInfo);
		BOOL32 bEnableFECC = ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsFECCEnable);
        ptMcMtInfo->m_atMtStatus[nIndex].SetIsEnableFECC(bEnableFECC);
		ptMcMtInfo->m_atMtStatus[nIndex].SetMtVideoInfo(ptMcuMcuMtInfo[nLoop].m_tPartVideoInfo);
		ptMcMtInfo->m_atMtStatus[nIndex].SetMcuIdx( wMcuIdx );
		ptMcMtInfo->m_atMtExt[nIndex].SetMcuIdx( wMcuIdx );
		ptMcMtInfo->m_atMtStatus[nIndex].SetMtId( byRealMtId );
		ptMcMtInfo->m_atMtExt[nIndex].SetMtId( byRealMtId );
 		LogPrint(LOG_LVL_WARNING, MID_MCU_CONF, "[ProcMcuMcuMtListAck]McuIdx.%d conn:%d MtExt loop.%d MtId is %d, name:(%s )\n",
 			wMcuIdx, ptMcuMcuMtInfo[nLoop].m_byIsConnected, nIndex, byRealMtId, ptMcuMcuMtInfo[nLoop].m_szMtName);

		//以下为发送下级列表到上级作准备
		/*if( !m_tCascadeMMCU.IsNull() && 
			MT_TYPE_SMCU == m_ptMtTable->GetMtType( tMt.GetMtId() ) &&
			ptMcuMcuMtInfo[nLoop].m_byCasLevel < MAX_CASCADELEVEL
			)
		{
			ptMcuMcuMtInfo[nLoop].m_abyMtIdentify[ptMcuMcuMtInfo[nLoop].m_byCasLevel] = ptMcuMcuMtInfo[nLoop].m_tMt.GetMtId(); 

			++ptMcuMcuMtInfo[nLoop].m_byCasLevel;			
			ptMcuMcuMtInfo[nLoop].m_tMt.SetMcuId( LOCAL_MCUID );
			ptMcuMcuMtInfo[nLoop].m_tMt.SetMtId( tMt.GetMcuId() );

			bIsCanNotifyToMMcu = TRUE;
		}
		*/
/*
		memcpy(&atMtExtU[nIndex], &ptMcMtInfo->m_atMtExt[nIndex], sizeof(TMt) + sizeof(u8)*2+sizeof(u16) + sizeof(u32)*2);
		atMtExtU[nIndex].SetProtocolType(ptMcuMcuMtInfo[nLoop].m_byProtocolType);
		atMtExtU[nIndex].SetAlias(ptMcuMcuMtInfo[nLoop].m_szMtName);
*/	}

	if(ISTRUE(byIsLastPack))
	{
        // 顾振华 [6/1/2006] 清除后面的内容
        if (nIndex < MAXNUM_CONF_MT)
        {
            memset( ptMcMtInfo->m_atMtStatus+(u32)nIndex, 0, (sizeof(TMcMtStatus) * (MAXNUM_CONF_MT - (u32)nIndex)) );
        }
        
		ptMcMtInfo->m_byLastPos = 0;

		//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ));
		//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
		CServMsg cMsg2Mcs = cServMsg; //此接口会改变参数
		SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cMsg2Mcs/*cServMsg*/ );

		//发给会控终端表
		if ( cServMsg.GetSrcMtId() == m_tCascadeMMCU.GetMtId() )
		{
			SendMtListToMcs( wMcuIdx );// mmcu回过来的mtlist_ack就不用再回给MMcu自己了 [pengguofeng 7/10/2013]
		}
		else
		{
			SendMtListToMcs( wMcuIdx , FALSE, TRUE );
		}
		// 应该将此时下级的TMcuMcuMtInfo报给上级，本级的不用，因为早就报上去了 [pengguofeng 7/9/2013]
		//对于分开报的就出错了
 /*		u8 byMtNum = (u8)nMtNum;
 		PackAndSendMtList2Mcs(byMtNum, atMtExtU, wMcuIdx);
*/
		//上报下级列表给上级
/*		if( ptMcuMcuMtInfo->m_byCasLevel < MAX_CASCADELEVEL  &&
			!m_tCascadeMMCU.IsNull() &&
			MT_TYPE_SMCU == m_ptMtTable->GetMtType( tMt.GetMtId() ) 
			)
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF,"[ProcMcuMcuMtListAck] Send %d mt to MMcu.%d\n", nMtNum, m_tCascadeMMCU.GetMtId());
			cServMsg.SetMsgBody( (u8*)&byIsLastPack,sizeof(u8) );
			cServMsg.CatMsgBody( (u8*)ptMcuMcuMtInfo,sizeof(TMcuMcuMtInfo)*nMtNum );
			SendMsgToMt( m_tCascadeMMCU.GetMtId(),MCU_MCU_MTLIST_ACK,cServMsg );
//			SendSMcuMtInfoToMMcu( wMcuIdx );
			ConfLog( FALSE,"[ProcMcuMcuMtListAck] Send " );
			cServMsg.SetMsgBody( (u8*)&byIsLastPack,sizeof(u8) );
			cServMsg.CatMsgBody( (u8*)ptMcuMcuMtInfo,sizeof(TMcuMcuMtInfo)*nMtNum );
			SendMsgToMt( m_tCascadeMMCU.GetMtId(),MCU_MCU_MTLIST_ACK,cServMsg );
			
		}
*/
		//[2/21/2011 chendaiwei]NPlus创会找到并改变发言人为下级终端
// 		TMtAlias tSpeaker = m_tConf.GetSpeakerAlias();
// 		TConfMcInfo *ptMcMtInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
// 
// 		if(ptMcMtInfo != NULL)
// 		{
// 			TMt tTemp;
// 			tTemp.SetNull();
// 			tTemp.SetMcuId(wMcuIdx);
// 			for( u8 byMtId = 1;byMtId <= MAXNUM_CONF_MT;byMtId++)
// 			{
// 				tTemp.SetMtId(byMtId);
// 				TMtExt * ptEx = ptMcMtInfo->GetMtExt(tTemp);
// 				if( ptEx != NULL)
// 				{
// 					if(memcmp(ptEx->GetAlias(),tSpeaker.m_achAlias,VALIDLEN_ALIAS) == 0)
// 					{
// 						ChangeSpeaker(&tTemp);
// 
// 						break;
// 					}
// 				}
// 			}
// 		}

		
// [pengjie 2010/12/14] 下级会在上级请求mtlist时直接上报mt状态上来，这里不用重复发。
//         // 顾振华 [6/1/2006] 发送下级Mcu终端状态给会控
// 	    CServMsg cServBackMsg;
// 	    cServBackMsg.SetNoSrc();
//         cServBackMsg.SetMsgBody();
// 		for(nIndex = 0; nIndex < MAXNUM_CONF_MT; nIndex++)
// 		{
// 			if( ptMcMtInfo->m_atMtStatus[nIndex].IsNull() || 
// 				ptMcMtInfo->m_atMtStatus[nIndex].GetMtId() == 0 ) //自己
// 			{
// 				continue;
// 			}
//             TMtStatus tMtStatus = ptMcMtInfo->m_atMtStatus[nIndex].GetMtStatus();
//             cServBackMsg.CatMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );	
//         }
//         if (cServBackMsg.GetMsgBodyLen() != 0)
//         {
		//             SendMsgToAllMcs( MCU_MCS_MTSTATUS_NOTIF, cServBackMsg );       
//         }         
	}
	else
	{
		ptMcMtInfo->m_byLastPos += nMtNum;
	}

	//  [2/23/2011 chendaiwei]N+1创会有下级发言人，改变发言人为下级发言人。同时把发言人别名清空，避免在没有发言人，发言人别名依旧的情
	//  况下形成误判，进而错误地将发言人改成别名代表的发言人。
	if(bIsNPlusSmcuSpeaker && 
		( MCU_NPLUS_SLAVE_SWITCH == g_cNPlusApp.GetLocalNPlusState() || MCU_NPLUS_MASTER_CONNECTED == g_cNPlusApp.GetLocalNPlusState()) )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_NPLUS,"Change speaker to smcu speaker McuId == %u, MtId == %u\n",tNPlusSmcuSpeaker.GetMcuId(),tNPlusSmcuSpeaker.GetMtId());
		
		ChangeSpeaker(&tNPlusSmcuSpeaker);

		TMtAlias tNullAlias;
		tNullAlias.SetNull();
		m_tConf.SetSpeakerAlias(tNullAlias);
	}

	if (m_tConf.m_tStatus.IsAutoMixing())
	{
		TMcu tMcu;
		tMcu.SetMcuId(LOCAL_MCUID);	
		CServMsg cMsg;
		u8 byMixDepth = 10;
		cMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) ); 
		cMsg.CatMsgBody( (u8*)&byMixDepth, 1 );
		OnStartDiscussToAllSMcu( &cMsg );
	}


	return;
}

/*====================================================================
    函数名      ：RestoreSubMtInVmp
    功能        ：恢复下级终端进画面合成
    算法实现    ：
    引用全局变量：
    输入参数说明：TMt& tMt 
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    10/07/16    4.6         陆昆朋			创建
====================================================================*/
void CMcuVcInst::RestoreSubMtInVmp(TMt& tMt)
{
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[RestoreSubMtInVmp] tMt(%d.%d)\n",tMt.GetMcuId(),tMt.GetMtId() );
	return;

	/*if (tMt.IsNull() || tMt.IsLocal())
	{
	return;
	}
	
	  TVMPParam tVMPParam = m_tConf.m_tStatus.GetVmpParam();
	  
		u8 byIndex = 0;
		if (!tVMPParam.FindVmpMember(tMt, byIndex))
		{
		return;
		}
		
		  TVMPMember* ptVMPMember = tVMPParam.GetVmpMember(byIndex);
		  
			if (ptVMPMember == NULL)
			{
			return;
			}
			
			  BOOL32 bStart = FALSE;
			  u8 byNewFormat = 0;
			  if (tVMPParam.GetVMPMode() == CONF_VMPMODE_NONE)
			  {
			  bStart = TRUE;
			  }
			  
				if ( IsLocalAndSMcuSupMultSpy(ptVMPMember->GetMcuId()) )
				{
				//注意，在发PreSetIn时，要把此终端信息从Param中删除，等在PreSetInAck中处理时会加进去
				m_tConf.m_tStatus.m_tVMPParam.ClearVmpMember(byIndex);
				m_tLastVmpParam = m_tConf.m_tStatus.GetVmpParam();
				
				  CVmpChgFormatResult cVmpChgFormatResult;
				  if (!GetMtFormat(*(TMt *)ptVMPMember, &tVMPParam, byNewFormat, cVmpChgFormatResult, bStart))
				  {
				  return;
				  }
				  
					//发Pre Setin (包含了级联调分辨率)
					TPreSetInReq tPreSetInReq;
					tPreSetInReq.m_tSpyMtInfo.SetSpyMt(*(TMt *)ptVMPMember);
					tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_tVmp = m_tVmpEqp;
					tPreSetInReq.m_bySpyMode = MODE_VIDEO;
					tPreSetInReq.m_dwEvId = MCS_MCU_STARTVMP_REQ;
					
					  //		tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_byRes = byNewFormat;
					  tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_byPos = byIndex;		
					  //		tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_byKeepOrgRes = 0; //no adapt channel in old vmp, so set this value 0
					  tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_byMemberType = ptVMPMember->GetMemberType();
					  tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_byMemStatus = ptVMPMember->GetMemStatus();
					  
						// [pengjie 2010/9/13] 填目的端能力
						TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(ptVMPMember->GetMcuId()) );	
						if(tSimCapSet.IsNull())
						{
						MultiSpyMgrLog( SPY_CRIT, "[ChangeSpeaker] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
						ptVMPMember->GetMcuId(), ptVMPMember->GetMtId() );
						return;
						}
						// 调整到画面合成要求的分辨率
						tSimCapSet.SetVideoResolution( byNewFormat );
						//zjl20101116 如果当前终端已回传则能力集要与已回传目的能力集取小
						tSimCapSet = GetMinSpyDstCapSet(*(TMt*)ptVMPMember, tSimCapSet);
						tPreSetInReq.m_tSpyMtInfo.SetSimCapset( tSimCapSet );
						// End
						
						  OnMMcuPreSetIn( tPreSetInReq );
	}*/
}

/*====================================================================
    函数名      ：ProcMcuMcuNewMtNotify
    功能        ：请求对方mcu新终端通知
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/21    3.6         Jason         创建
====================================================================*/
void CMcuVcInst::ProcMcuMcuNewMtNotify(  const CMessage * pcMsg )
{
	CServMsg	 cServMsg( pcMsg->content, pcMsg->length );
	TMt			 tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	TMcuMcuMtInfo tMcuMcuMtInfo;
	memcpy(&tMcuMcuMtInfo, cServMsg.GetMsgBody(), sizeof(tMcuMcuMtInfo));

	STATECHECK;

	u16 wMcuIdx = INVALID_MCUIDX;

	u8 bySecMcuId = 0;

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuNewMtNotify]  tMt(%d.%d.%d) byCasLevel(%d)\n",
			tMcuMcuMtInfo.m_tMt.GetMcuId(),
			tMcuMcuMtInfo.m_tMt.GetMtId(),
			tMcuMcuMtInfo.m_abyMtIdentify[0],
			tMcuMcuMtInfo.m_byCasLevel
			);
	
	if( 0 != tMcuMcuMtInfo.m_byCasLevel &&
		tMcuMcuMtInfo.m_byCasLevel <= MAX_CASCADELEVEL
		)
	{		
		bySecMcuId = tMcuMcuMtInfo.m_tMt.GetMtId();
	}
	
	u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
	abyMcuId[0] = tMt.GetMtId();
	abyMcuId[1] = bySecMcuId;

	if( !m_tConfAllMcuInfo.GetIdxByMcuId(&abyMcuId[0],2,&wMcuIdx) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU,  "[ProcMcuMcuNewMtNotify] Fail to get Mcu Index.FstMcuId.%d SecMcuId.%d\n",
			tMt.GetMtId(),bySecMcuId
			);
		return;
	}
	

	u16 wSMcuIdx = INVALID_MCUIDX;
	m_tConfAllMcuInfo.GetIdxByMcuId(&abyMcuId[0],1,&wSMcuIdx);
	if( 0 == bySecMcuId && !IsRosterRecved(wSMcuIdx) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU,"[ProcMcuMcuNewMtNotify]smcu.%d mcuidx.%d is not recv roster ntf\n",
			abyMcuId[0],wSMcuIdx );	
		return;
	}

	u8 byRealMtId = 0;
	if( 0 == tMcuMcuMtInfo.m_byCasLevel )
	{
		byRealMtId = tMcuMcuMtInfo.m_tMt.GetMtId();
	}
	else
	{
		if( tMcuMcuMtInfo.m_byCasLevel <= MAX_CASCADELEVEL )
		{
			byRealMtId = tMcuMcuMtInfo.m_abyMtIdentify[tMcuMcuMtInfo.m_byCasLevel-1];
		}
		else
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU,  "[ProcMcuMcuNewMtNotify] Error CasLevel.%d MtId(%d)\n",
				tMcuMcuMtInfo.m_byCasLevel,tMt.GetMtId()
				);
			return;
		}
		
	}

	TMt tNewMt = tMcuMcuMtInfo.m_tMt;
	tNewMt.SetMcuIdx( wMcuIdx );
	tNewMt.SetMtId( byRealMtId );
	tNewMt.SetConfIdx( m_byConfIdx );

	/*20111024 zjl 1. 上级呼叫终端入会 + 本级支持显示上级终端列表 = 保存上级终端列表
				   2. 下级呼叫终端入会 = 本级保存下级终端列表
	               3. 没有上级mcu  即 等同于2的情况*/
	if (!(tMt == m_tCascadeMMCU) ||
		(tMt == m_tCascadeMMCU &&
		 g_cMcuVcApp.IsShowMMcuMtList()))
	{
		//增加到终端表里
		TConfMtInfo tConfMtInfo = m_tConfAllMtInfo.GetMtInfo(wMcuIdx);//tMt.GetMtId());
		if(tConfMtInfo.IsNull())
		{
			return;
		}
		tConfMtInfo.AddJoinedMt(byRealMtId);//tMcuMcuMtInfo.m_tMt.GetMtId());
		m_tConfAllMtInfo.SetMtInfo(tConfMtInfo);

		RestoreSubMtInVmp( tNewMt );

		//发给会控会议所有终端信息
		//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
		//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
		SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
		
		//查找mc table
		TConfMcInfo *ptMcMtInfo = m_ptConfOtherMcTable->AddMcInfo(wMcuIdx);//tMt.GetMtId());
		if(ptMcMtInfo == NULL)
		{
			return;
		}
		
		s32 nIndex = 0; 
		s32 nIdle = MAXNUM_CONF_MT;
		for(; nIndex<MAXNUM_CONF_MT; nIndex++)
		{
			if(ptMcMtInfo->m_atMtStatus[nIndex].IsNull() && nIdle == MAXNUM_CONF_MT)
			{
				// 找到第一个插入点
				nIdle = nIndex;
			}
			if(ptMcMtInfo->m_atMtStatus[nIndex].GetMtId() == byRealMtId )//tMcuMcuMtInfo.m_tMt.GetMtId())
			{
				// 找到原来的匹配
				break;
			}
		}

		if(nIndex >= MAXNUM_CONF_MT)
		{
			if(nIdle >= MAXNUM_CONF_MT)
			{
				//no idle pos;
				return;
			}
			else
			{
				nIndex = nIdle;
			}
		}

		ptMcMtInfo->m_atMtStatus[nIndex].SetAudioIn(tMcuMcuMtInfo.m_byAudioIn);
		ptMcMtInfo->m_atMtStatus[nIndex].SetAudioOut(tMcuMcuMtInfo.m_byAudioOut);
		BOOL32 bSendAudio = !ISTRUE(tMcuMcuMtInfo.m_byIsAudioMuteIn);
		ptMcMtInfo->m_atMtStatus[nIndex].SetSendAudio(bSendAudio);
		BOOL32 bRecvAudio = !ISTRUE(tMcuMcuMtInfo.m_byIsAudioMuteOut);
		ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveAudio(bRecvAudio);
		ptMcMtInfo->m_atMtExt[nIndex].SetMt(tMcuMcuMtInfo.m_tMt);
		ptMcMtInfo->m_atMtStatus[nIndex].SetMt(tMcuMcuMtInfo.m_tMt);
#ifdef _UTF8
		// 修正utf8字符串 [pengguofeng 5/24/2013]
		CorrectUtf8Str(tMcuMcuMtInfo.m_szMtName, strlen(tMcuMcuMtInfo.m_szMtName));
#endif
		ptMcMtInfo->m_atMtExt[nIndex].SetAlias(tMcuMcuMtInfo.m_szMtName);
		ptMcMtInfo->m_atMtExt[nIndex].SetIPAddr(ntohl(tMcuMcuMtInfo.m_dwMtIp));
		ptMcMtInfo->m_atMtExt[nIndex].SetMtType(tMcuMcuMtInfo.m_byMtType);
		ptMcMtInfo->m_atMtExt[nIndex].SetManuId(tMcuMcuMtInfo.m_byManuId);
		ptMcMtInfo->m_atMtStatus[nIndex].SetMtType(tMcuMcuMtInfo.m_byMtType);
		ptMcMtInfo->m_atMtStatus[nIndex].SetVideoIn(tMcuMcuMtInfo.m_byVideoIn);
		ptMcMtInfo->m_atMtStatus[nIndex].SetVideoOut(tMcuMcuMtInfo.m_byVideoOut);
	//	ptMcMtInfo->m_atMtStatus[nIndex].SetVideo2In(tMcuMcuMtInfo.m_byVideo2In);
	//	ptMcMtInfo->m_atMtStatus[nIndex].SetVideo2Out(tMcuMcuMtInfo.m_byVideo2Out);
		BOOL32 bSendVideo = !ISTRUE(tMcuMcuMtInfo.m_byIsVideoMuteIn);
		ptMcMtInfo->m_atMtStatus[nIndex].SetSendVideo(bSendVideo); 
		BOOL32 bRecvVideo = !ISTRUE(tMcuMcuMtInfo.m_byIsVideoMuteOut);
		ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveVideo(bRecvVideo);
		TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(wMcuIdx);//tMt.GetMtId());
		tMtInfo.RemoveMt(byRealMtId);//tMcuMcuMtInfo.m_tMt.GetMtId());
		if(ISTRUE(tMcuMcuMtInfo.m_byIsConnected))
		{
			tMtInfo.AddJoinedMt(byRealMtId);//tMcuMcuMtInfo.m_tMt.GetMtId());
		}
		else
		{
			tMtInfo.AddMt(byRealMtId);//tMcuMcuMtInfo.m_tMt.GetMtId());
		}
		m_tConfAllMtInfo.SetMtInfo(tMtInfo);
		BOOL32 bEnableFEC = ISTRUE(tMcuMcuMtInfo.m_byIsFECCEnable);
		ptMcMtInfo->m_atMtStatus[nIndex].SetIsEnableFECC(bEnableFEC);
		ptMcMtInfo->m_atMtStatus[nIndex].SetMtVideoInfo(tMcuMcuMtInfo.m_tPartVideoInfo); 
		ptMcMtInfo->m_atMtStatus[nIndex].SetMcuIdx( wMcuIdx );
		ptMcMtInfo->m_atMtStatus[nIndex].SetMtId( byRealMtId );
		ptMcMtInfo->m_atMtExt[nIndex].SetMcuIdx( wMcuIdx );
		ptMcMtInfo->m_atMtExt[nIndex].SetMtId( byRealMtId );

		
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcuMcuNewMtNotify] nIndex.%d MtId.%d szMtName.%s bOnLine.%d\n", 
				 nIndex, tMcuMcuMtInfo.m_tMt.GetMtId(), tMcuMcuMtInfo.m_szMtName, tMcuMcuMtInfo.m_byIsConnected );

 		SendMtListToMcs( wMcuIdx, FALSE, FALSE ); // 需要再报 [pengguofeng 7/10/2013]
	}
	else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuNewMtNotify] tMt<McuId.%d, MtId.%d> is equal to mmcu <McuId.%d, MtId.%d> , IsShowMMcuMtList.%d!\n",
									tMt.GetMcuId(), tMt.GetMtId(), 
									m_tCascadeMMCU.GetMcuId(), m_tCascadeMMCU.GetMtId(),
									g_cMcuVcApp.IsShowMMcuMtList());
	}

	//上报至上级
	if( !m_tCascadeMMCU.IsNull() && 
		MT_TYPE_SMCU == m_ptMtTable->GetMtType( tMt.GetMtId() ) &&
		tMcuMcuMtInfo.m_byCasLevel < MAX_CASCADELEVEL &&
		IsMMcuSupportMultiCascade()
		)
	{
		tMcuMcuMtInfo.m_tMt = tNewMt;
		tMcuMcuMtInfo.m_tMt.SetMcuId( abyMcuId[0] );
		cServMsg.SetMsgBody( (u8*)&tMcuMcuMtInfo,sizeof( TMcuMcuMtInfo ) );
		SendMsgToMt( m_tCascadeMMCU.GetMtId(),MCU_MCU_NEWMT_NOTIF,cServMsg );		
	}

	// 对于VCS会议，还需要进行选看，回传操作
	if (VCS_CONF == m_tConf.GetConfSource())
	{
		
		MtOnlineChange(tNewMt, TRUE, 0);

		if (m_cVCSConfStatus.GetCurVCMT() == tNewMt)
		{
			// 将该下级mcu原回传通道中的成员挂断
			TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tNewMt.GetMcuId());
			if (ptMcInfo != NULL && !(ptMcInfo->m_tMMcuViewMt.IsNull()) &&
				m_tConfAllMtInfo.MtJoinedConf(ptMcInfo->m_tMMcuViewMt.GetMcuId(), ptMcInfo->m_tMMcuViewMt.GetMtId()) &&
				!(ptMcInfo->m_tMMcuViewMt == tNewMt))
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuNewMtNotify]drop mt(mcuid:%d, mtid:%d) because of new submt(mcuid:%d, mtid:%d) online\n",
					    ptMcInfo->m_tMMcuViewMt.GetMcuId(), ptMcInfo->m_tMMcuViewMt.GetMtId(), tNewMt.GetMcuId(), tNewMt.GetMtId());
				VCSDropMT(ptMcInfo->m_tMMcuViewMt);	
			}

			OnMMcuSetIn(tNewMt, m_cVCSConfStatus.GetCurSrcSsnId(), SWITCH_MODE_SELECT);
		}

		/*if (!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
		{
			GoOnSelStep(tMt, MODE_VIDEO, TRUE);
			GoOnSelStep(tMt, MODE_AUDIO, TRUE);
			GoOnSelStep(tNewMt, MODE_VIDEO, FALSE);
			GoOnSelStep(tNewMt, MODE_AUDIO, FALSE);
		}*/

	    //有关画面合成的处理
        if(VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode())
        {
            ChangeVmpStyle(tMt, TRUE);
        }
	}
}

/*=============================================================================
    函 数 名： ProcMcuMcuCallAlertMtNotify
    功    能： mcu<->mcu发起终端呼叫通知
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/5/24   3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::ProcMcuMcuCallAlertMtNotify(  const CMessage * pcMsg )
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	TMt      tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	TMt tAlertMt = *( (TMt*)(cServMsg.GetMsgBody()) );
	TMtAlias tAlertMtAlias = *( (TMtAlias*)(cServMsg.GetMsgBody()+sizeof(TMt)) );
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)( cServMsg.GetMsgBody()+sizeof(TMt)+sizeof(TMtAlias) );

	STATECHECK;

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuCallAlertMtNotify]  tMt(%d.%d.%d) byCasLevel(%d)\n",
		tAlertMt.GetMcuId(),
		tAlertMt.GetMtId(),
		tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
		tHeadMsg.m_tMsgSrc.m_byCasLevel
		);

	//u8 byFstMcuId = tMt.GetMtId();
	u8 bySecMcuId = 0;
	
	u8 byReaMtId = 0;
	if( tHeadMsg.m_tMsgSrc.m_byCasLevel > 0 )
	{
		bySecMcuId = tAlertMt.GetMtId();
		if( tHeadMsg.m_tMsgSrc.m_byCasLevel <= MAX_CASCADELEVEL )
		{
			byReaMtId = tHeadMsg.m_tMsgSrc.m_abyMtIdentify[ tHeadMsg.m_tMsgSrc.m_byCasLevel - 1 ];
		}
	}
	else
	{
		byReaMtId = tAlertMt.GetMtId();
	}	
	
	u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
	abyMcuId[0] = tMt.GetMtId();
	abyMcuId[1] = bySecMcuId;

	u16 wMcuIdx = INVALID_MCUIDX;
	if( !m_tConfAllMcuInfo.GetIdxByMcuId( &abyMcuId[0],2,&wMcuIdx ) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU,  "[ProcMcuMcuCallAlertMtNotify] Fail to get mcuIdx by mcuid(%d.%d)\n",
			tMt.GetMtId(),bySecMcuId
			);
		return;
	}
	
	if (!(tMt == m_tCascadeMMCU) ||
		(tMt == m_tCascadeMMCU &&
		 g_cMcuVcApp.IsShowMMcuMtList()))
	{
		//增加到终端表里
		TConfMtInfo tConfMtInfo = m_tConfAllMtInfo.GetMtInfo(wMcuIdx);//tMt.GetMtId());
		if(tConfMtInfo.IsNull())
		{
			return;
		}
		tConfMtInfo.AddMt( byReaMtId );
		m_tConfAllMtInfo.SetMtInfo( tConfMtInfo );

		//查找mc table
		TConfMcInfo *ptMcMtInfo = m_ptConfOtherMcTable->AddMcInfo(wMcuIdx);//tMt.GetMtId());
		if( NULL == ptMcMtInfo )
		{
			return;
		}
		
		s32 nIndex = 0;
		s32 nIdle = MAXNUM_CONF_MT;
		for(; nIndex<MAXNUM_CONF_MT; nIndex++)
		{
			if(ptMcMtInfo->m_atMtStatus[nIndex].IsNull() && nIdle == MAXNUM_CONF_MT &&
				!( TYPE_MT == ptMcMtInfo->m_atMtStatus[nIndex].GetType() &&
				0 == ptMcMtInfo->m_atMtStatus[nIndex].GetMtId() )//mtid为0的终端比较特殊，代表mcu自己
				)
			{
				// 找到第一个插入点
				nIdle = nIndex;
			}
			if(ptMcMtInfo->m_atMtStatus[nIndex].GetMtId() == byReaMtId )//tAlertMt.GetMtId())
			{
				//已有，则不再重复设置
				return;
				//break;
			}
		}
		
		if(nIndex >= MAXNUM_CONF_MT)
		{
			if(nIdle >= MAXNUM_CONF_MT)
			{
				//no idle pos;
				return;
			}
			else
			{
				nIndex = nIdle;
			}
		}
		
		//发给会控会议所有终端信息
		//cServMsg.SetMsgBody( (u8*)&m_tConfAllMcuInfo,sizeof(TConfAllMcuInfo) );
		//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
		SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
		
		ptMcMtInfo->m_atMtStatus[nIndex].SetAudioIn(MEDIA_TYPE_NULL);
		ptMcMtInfo->m_atMtStatus[nIndex].SetAudioOut(MEDIA_TYPE_NULL);
		ptMcMtInfo->m_atMtStatus[nIndex].SetSendAudio( FALSE );
		ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveAudio( FALSE );
		
		ptMcMtInfo->m_atMtExt[nIndex].SetMt(tAlertMt);
		ptMcMtInfo->m_atMtStatus[nIndex].SetMt(tAlertMt);
		
		if( mtAliasTypeTransportAddress == tAlertMtAlias.m_AliasType )
		{
			ptMcMtInfo->m_atMtExt[nIndex].SetIPAddr(tAlertMtAlias.m_tTransportAddr.GetIpAddr());
			//zjj20100909 一定要把别名字符串设置为空,如果这个终端呼不上来,界面会显示存在里面的前一个终端的别名
			ptMcMtInfo->m_atMtExt[nIndex].SetAlias( "" );
			//zjj 20090907 不用把ip设置为别名
			//u32  dwDialIP  = tAlertMtAlias.m_tTransportAddr.GetIpAddr();
			//ptMcMtInfo->m_atMtExt[nIndex].SetAlias(StrOfIP(dwDialIP));
		}
		else
		{
#ifdef _UTF8
			s8 achMtName[VALIDLEN_ALIAS];
			memset(achMtName, 0, sizeof(achMtName));
			memcpy(achMtName, tAlertMtAlias.m_achAlias, VALIDLEN_ALIAS-1);
			CorrectUtf8Str(achMtName, strlen(achMtName));
			ptMcMtInfo->m_atMtExt[nIndex].SetAlias(achMtName);
#else
			ptMcMtInfo->m_atMtExt[nIndex].SetAlias(tAlertMtAlias.m_achAlias);
#endif
		}
		
		ptMcMtInfo->m_atMtExt[nIndex].SetMtType(MT_TYPE_MT);
		//ptMcMtInfo->m_atMtExt[nIndex].SetManuId(m_byManuId);
		ptMcMtInfo->m_atMtStatus[nIndex].SetMtType(MT_TYPE_MT);
		
		ptMcMtInfo->m_atMtStatus[nIndex].SetVideoIn(MEDIA_TYPE_NULL);
		ptMcMtInfo->m_atMtStatus[nIndex].SetVideoOut(MEDIA_TYPE_NULL);
		//	ptMcMtInfo->m_atMtStatus[nIndex].SetVideo2In(MEDIA_TYPE_NULL);
		//	ptMcMtInfo->m_atMtStatus[nIndex].SetVideo2Out(MEDIA_TYPE_NULL);
		ptMcMtInfo->m_atMtStatus[nIndex].SetSendVideo( FALSE );    
		ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveVideo( FALSE );
		TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(wMcuIdx);//tMt.GetMtId());
		tMtInfo.RemoveMt( byReaMtId );//tAlertMt.GetMtId() );
		tMtInfo.AddMt( byReaMtId );//tAlertMt.GetMtId() );
		m_tConfAllMtInfo.SetMtInfo(tMtInfo);
		ptMcMtInfo->m_atMtStatus[nIndex].SetIsEnableFECC( FALSE );
		ptMcMtInfo->m_atMtExt[nIndex].SetMcuIdx( wMcuIdx );
		ptMcMtInfo->m_atMtStatus[nIndex].SetMcuIdx( wMcuIdx );
		ptMcMtInfo->m_atMtExt[nIndex].SetMtId( byReaMtId );
		ptMcMtInfo->m_atMtStatus[nIndex].SetMtId( byReaMtId );
		//ptMcMtInfo->m_atMtStatus[nIndex].m_tPartVideoInfo =  tMcuMcuMtInfo.m_tPartVideoInfo;  
		
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuCallAlertMtNotify] nIndex.%d MtId.%d\n", nIndex, tAlertMt.GetMtId() );
		// 需要再报 [pengguofeng 7/10/2013]
  		SendMtListToMcs( wMcuIdx, FALSE, FALSE );//tMt.GetMtId() );
	}
	else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuCallAlertMtNotify] tMt<McuId.%d, MtId.%d> is equal to mmcu <McuId.%d, MtId.%d> , IsShowMMcuMtList.%d!\n",
									tMt.GetMcuId(), tMt.GetMtId(), 
									m_tCascadeMMCU.GetMcuId(), m_tCascadeMMCU.GetMtId(),
									g_cMcuVcApp.IsShowMMcuMtList());
	}
		
	if( !m_tCascadeMMCU.IsNull() && 
		MT_TYPE_SMCU == m_ptMtTable->GetMtType( tMt.GetMtId() ) &&
		tHeadMsg.m_tMsgSrc.m_byCasLevel < MAX_CASCADELEVEL &&
		IsMMcuSupportMultiCascade()
		)
	{
		//tHeadMsg.m_tMsgSrc.m_tMt = tAlertMt;		
		
		cServMsg.SetMsgBody( (u8*)&tAlertMtAlias, sizeof( TMtAlias ) );
		cServMsg.CatMsgBody( (u8*)&tAlertMt,sizeof(TMt) );
		cServMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
		SendMsgToMt( m_tCascadeMMCU.GetMtId(),MCU_MCU_CALLALERTING_NOTIF,cServMsg );
	}
	
}

/*====================================================================
    函数名      ：ProcMcuMcuDropMtNotify
    功能        ：mcu<->mcu挂断终端通知
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/21    3.6         Jason         创建
====================================================================*/
void CMcuVcInst::ProcMcuMcuDropMtNotify(  const CMessage * pcMsg )
{
	STATECHECK;

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	TMt		    tDropMt = *(TMt* )(cServMsg.GetMsgBody());
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody() + sizeof(TMt));
	TMt			tVcsVCMT = m_cVCSConfStatus.GetCurVCMT();

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuDropMtNotify]  tMt(%d.%d.%d) tDropMt(%d.%d) byCasLevel(%d)\n",
		tMt.GetMcuId(),
		tMt.GetMtId(),
		tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
		tDropMt.GetMcuId(),
		tDropMt.GetMtId(),
		tHeadMsg.m_tMsgSrc.m_byCasLevel
		);

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL,  "[ProcMcuMcuDropMt(%d.%d)\n", tDropMt.GetMcuId(), tDropMt.GetMtId());

	u8 bySecMcuId = 0;
	u8 byRealMtId = 0;
	if( tHeadMsg.m_tMsgSrc.m_byCasLevel > 0 )
	{
		bySecMcuId = tDropMt.GetMtId();
		if( tHeadMsg.m_tMsgSrc.m_byCasLevel <= MAX_CASCADELEVEL )
		{
			byRealMtId = tHeadMsg.m_tMsgSrc.m_abyMtIdentify[ tHeadMsg.m_tMsgSrc.m_byCasLevel - 1 ];
		}
	}
	else
	{
		byRealMtId = tDropMt.GetMtId();
	}
	
	u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
	abyMcuId[0] = tMt.GetMtId();
	abyMcuId[1] = bySecMcuId;

	u16 wMcuIdx = INVALID_MCUIDX;
	if( !m_tConfAllMcuInfo.GetIdxByMcuId( &abyMcuId[0],2,&wMcuIdx ) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU,  "[ProcMcuMcuDropMtNotify] Fail to get mcuIdx by mcuid(%d.%d)\n",
			tMt.GetMtId(),bySecMcuId
			);
		return;
	}	
	
	if( !m_tCascadeMMCU.IsNull() && 
		MT_TYPE_SMCU == m_ptMtTable->GetMtType( tMt.GetMtId() ) &&
		tHeadMsg.m_tMsgSrc.m_byCasLevel < MAX_CASCADELEVEL &&
		IsMMcuSupportMultiCascade()
		)
	{		
		cServMsg.SetMsgBody( (u8*)&tDropMt,sizeof(tDropMt) );
		cServMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
		SendMsgToMt( m_tCascadeMMCU.GetMtId(),MCU_MCU_DROPMT_NOTIF,cServMsg );
	}

	tDropMt.SetNull();
	tDropMt.SetMcuIdx( wMcuIdx );
	tDropMt.SetMtId( byRealMtId );
	tDropMt.SetConfIdx( m_byConfIdx );

	//若该终端作为非直连终端是本地发言人，清空本地发言人相关
	if ( tDropMt == m_tConf.GetSpeaker() || IsMtInMcu(tDropMt ,  m_tConf.GetSpeaker())  )
	{  
		// [pengjie 2010/9/2] 挂断下级终端如果在轮询，则不停止
		BOOL32 bIsMtInPolling = FALSE;
		if ( CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollState() )
		{
			if (tDropMt == (TMt)m_tConf.m_tStatus.GetMtPollParam() 
				|| (!m_tConf.m_tStatus.GetMtPollParam().IsLocal() 
				     && IsMtInMcu(tDropMt,(TMt)m_tConf.m_tStatus.GetMtPollParam()))
			   )
			{
				bIsMtInPolling = TRUE;
			}
		}
		ChangeSpeaker( NULL, bIsMtInPolling );
	}

	//增加到终端表里
	TConfMtInfo tConfMtInfo = m_tConfAllMtInfo.GetMtInfo(wMcuIdx);//tDropMt.GetMcuId());
    if(tConfMtInfo.IsNull())
	{
		return;
	}

	tConfMtInfo.RemoveJoinedMt(byRealMtId);//tDropMt.GetMtId());
	m_tConfAllMtInfo.SetMtInfo(tConfMtInfo);
    
    // guzh [3/9/2007] 通知MCS
    MtOnlineChange(tDropMt, FALSE, MTLEFT_REASON_NORMAL);
	ProcConfPollingByRemoveMt( tDropMt );
	ProcVmpPollingByRemoveMt( tDropMt );
	ProcMMcuSpyMtByRemoveMt( tDropMt );

	//有关画面合成的处理
	if (VCS_CONF == m_tConf.GetConfSource() && 
		VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode())
	{
		ChangeVmpStyle(tDropMt, FALSE);
	}

	if ( ROLLCALL_MODE_NONE != m_tConf.m_tStatus.GetRollCallMode() )
    {
        if ( tDropMt == m_tRollCallee || IsMtInMcu(tDropMt , m_tRollCallee))
        {
            NotifyMcsAlarmInfo( 0, ERR_MCU_ROLLCALL_CALLEELEFT );
            RollCallStop(cServMsg);
        }
    }

	// [pengjie 2010/10/14] 挂断终端时清该终端及该终端（SMCU）下的终端的状态及资源
	// 1、清外设相关信息
	RemoveMtFormPeriInfo( tDropMt, MTLEFT_REASON_NORMAL );

	// 2、清多回传选看信息
	TMt tMtSrc, tMtDst;
	TMtStatus tMtStatus;
	for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		if( m_tConfAllMtInfo.MtJoinedConf(byLoop) )
		{
			m_ptMtTable->GetMtStatus( byLoop, &tMtStatus );
			tMtSrc = tMtStatus.GetSelectMt( MODE_VIDEO );
			
			// 挂断的是下级终端，或下级终端所在的mcu，都要停选看
			if( !tMtSrc.IsNull() && 
				(tMtSrc == tDropMt || (IsMtInMcu(tDropMt, tMtSrc)) )
				)
			{
				tMtDst = m_ptMtTable->GetMt( byLoop );
				StopSelectSrc( tMtDst, MODE_VIDEO );
				FreeRecvSpy(tMtSrc, MODE_VIDEO, TRUE);
			}

			tMtSrc = tMtStatus.GetSelectMt( MODE_AUDIO );			
			if( !tMtSrc.IsNull() && 
				(tMtSrc == tDropMt || (IsMtInMcu(tDropMt, tMtSrc)) )
				)
			{
				tMtDst = m_ptMtTable->GetMt( byLoop );
				StopSelectSrc( tMtDst, MODE_AUDIO );
				FreeRecvSpy(tMtSrc, MODE_AUDIO, TRUE);
			}
		}
	}

	if( m_cSMcuSpyMana.IsMtInSpyMember(tDropMt) )
	{
		FreeRecvSpy(tDropMt, MODE_BOTH, TRUE);
	}	

	BOOL32 bIsSendToChairman = FALSE;
    if (m_tApplySpeakQue.IsMtInQueue(tDropMt) )
    {
        m_tApplySpeakQue.ProcQueueInfo(tDropMt, bIsSendToChairman, FALSE);
        NotifyMcsApplyList( bIsSendToChairman );
    }
	
	// 3、如果该终端是mcu，要删除所有该mcu下的多回传信息
	RemoveMcu( tDropMt );

	// End
	//cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMcuInfo, sizeof( TConfAllMcuInfo ) );
	//cServMsg.CatMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );


	//若该终端本地发言人，清空本地发言人相关
	if ( VCS_CONF == m_tConf.GetConfSource() )
	{
		if( tDropMt == tVcsVCMT )
		{
			TMt tSpeakMt;		
			if( CONF_SPEAKMODE_ANSWERINSTANTLY == m_tConf.GetConfSpeakMode() &&
				!m_tApplySpeakQue.IsQueueNull() )
			{
				if( m_tApplySpeakQue.GetQueueHead(tSpeakMt) ) 
				{	
					ChgCurVCMT( tSpeakMt );
				}
				else
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_VCS, "[ProcMcuMcuDropMtNotify] Fail to get Queue Head!\n" );
				}
			}
			else
			{
				tSpeakMt.SetNull();
				ChgCurVCMT( tSpeakMt );
			}
		}
		if( ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) )
		{
			VCSClearTvWallChannelByMt( tDropMt,TRUE );
		}
	}

	
}

/*====================================================================
    函数名      ：ProcMcuMcuDelMtNotify
    功能        ：mcu<->mcu删除终端通知
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/21    3.6         Jason         创建
====================================================================*/
void CMcuVcInst::ProcMcuMcuDelMtNotify(  const CMessage * pcMsg )
{
	STATECHECK;

	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u8			byMtId = cServMsg.GetSrcMtId();
	TMt			tDelMt = *(TMt* )(cServMsg.GetMsgBody());
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody()+sizeof( TMt ) );
	

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuDelMtNotify]  tMt(%d.%d.%d) byCasLevel(%d)\n",
		tDelMt.GetMcuId(),
		tDelMt.GetMtId(),
		tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0],
		tHeadMsg.m_tMsgSrc.m_byCasLevel
		);
	ConfPrint(LOG_LVL_ERROR, MID_MCU_CALL,  "[ProcMcuMcuDelMt(%d.%d)\n", tDelMt.GetMcuId(), tDelMt.GetMtId());

	//u8 byFstMcuId = byMtId;
	u8 bySecMcuId = 0;
	
	u8 byRealMtId = 0;
	if( tHeadMsg.m_tMsgSrc.m_byCasLevel > 0 )
	{
		bySecMcuId = tDelMt.GetMtId();
		if( tHeadMsg.m_tMsgSrc.m_byCasLevel <= MAX_CASCADELEVEL )
		{
			byRealMtId = tHeadMsg.m_tMsgSrc.m_abyMtIdentify[ tHeadMsg.m_tMsgSrc.m_byCasLevel - 1 ];
		}
	}
	else
	{
		byRealMtId = tDelMt.GetMtId();
	}
	
	u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
	abyMcuId[0] = byMtId;
	abyMcuId[1] = bySecMcuId;

	u16 wMcuIdx = INVALID_MCUIDX;
	if( !m_tConfAllMcuInfo.GetIdxByMcuId( &abyMcuId[0],2,&wMcuIdx ) )
	//if( !m_tConfAllMcuInfo.GetIdxByMcuId( byFstMcuId,bySecMcuId,&wMcuIdx ) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU,  "[ProcMcuMcuDelMtNotify] Fail to get mcuIdx by mcuid(%d.%d)\n",
			byMtId,bySecMcuId
			);
		return;
	}	

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcuMcuDelMtNotify] Recv Del Mt(%d.%d.%d) Notify\n",
		abyMcuId[0],abyMcuId[1],byRealMtId
		);
	

	if( !m_tCascadeMMCU.IsNull() && 
		MT_TYPE_SMCU == m_ptMtTable->GetMtType( byMtId ) &&
		tHeadMsg.m_tMsgSrc.m_byCasLevel < MAX_CASCADELEVEL &&
		IsMMcuSupportMultiCascade()
		)
	{		
		//tHeadMsg.m_tMsgSrc.m_tMt = tDelMt;
		cServMsg.SetMsgBody( (u8*)&tDelMt,sizeof(tDelMt) );
		cServMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
		SendMsgToMt( m_tCascadeMMCU.GetMtId(),MCU_MCU_DELMT_NOTIF,cServMsg );
	}

	//TMt tDelMt;
	tDelMt.SetNull();
	tDelMt.SetMcuIdx( wMcuIdx );
	tDelMt.SetMtId( byRealMtId );
	tDelMt.SetConfIdx( m_byConfIdx );

	if ( tDelMt == m_tConf.GetSpeaker() || IsMtInMcu(tDelMt ,  m_tConf.GetSpeaker())  )
	{  
		// [pengjie 2010/9/2] 挂断下级终端如果在轮询，则不停止
		BOOL32 bIsMtInPolling = FALSE;
		if ( CONF_POLLMODE_NONE != m_tConf.m_tStatus.GetPollState() )
		{
			if (tDelMt == (TMt)m_tConf.m_tStatus.GetMtPollParam() ||
				(!m_tConf.m_tStatus.GetMtPollParam().IsLocal() && 
				 IsMtInMcu(tDelMt,(TMt)m_tConf.m_tStatus.GetMtPollParam())
				) )
			{
				bIsMtInPolling = TRUE;
			}
		}
		ChangeSpeaker( NULL, bIsMtInPolling );
		// End
	}
	//增加到终端表里
	TConfMtInfo tConfMtInfo = m_tConfAllMtInfo.GetMtInfo(wMcuIdx);//tDelMt.GetMcuId());
    if(tConfMtInfo.IsNull())
	{
		return;
	}
    
	if(tConfMtInfo.MtInConf(byRealMtId))//tDelMt.GetMtId()))
	{
		tConfMtInfo.RemoveMt(byRealMtId);//tDelMt.GetMtId());
		//SendMsgToMt(byMtId, MCU_MCU_DELMT_NOTIF,cServMsg); 
	}
	m_tConfAllMtInfo.SetMtInfo(tConfMtInfo);
	ProcConfPollingByRemoveMt( tDelMt );
	ProcVmpPollingByRemoveMt( tDelMt );
	ProcMMcuSpyMtByRemoveMt( tDelMt );

	// [pengjie 2010/10/14] 删除终端时清该终端及该终端（SMCU）下的终端的状态及资源
	// 1、清外设相关信息
	RemoveMtFormPeriInfo( tDelMt, MTLEFT_REASON_DELETE );
	
	// 2、清终端多回传选看相关信息
	TMt tMtSrc, tMtDst;
	TMtStatus tMtStatus;
	for( u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++ )
	{
		if( m_tConfAllMtInfo.MtJoinedConf(byLoop) )
		{
			m_ptMtTable->GetMtStatus( byLoop, &tMtStatus );
			tMtSrc = tMtStatus.GetSelectMt( MODE_VIDEO );			
			
			// 删除的是下级终端，或下级终端所在的mcu，都要停选看
			if( !tMtSrc.IsNull() && 
				(tMtSrc == tDelMt || (IsMtInMcu(tDelMt, tMtSrc)) )
				)
			{
				tMtDst = m_ptMtTable->GetMt( byLoop );
				StopSelectSrc( tMtDst, MODE_VIDEO );
				FreeRecvSpy(tMtSrc, MODE_VIDEO, TRUE);
			}

			tMtSrc = tMtStatus.GetSelectMt( MODE_AUDIO );
			if( !tMtSrc.IsNull() && 
				(tMtSrc == tDelMt || (IsMtInMcu(tDelMt, tMtSrc)) )
				)
			{
				tMtDst = m_ptMtTable->GetMt( byLoop );
				StopSelectSrc( tMtDst, MODE_AUDIO );
				FreeRecvSpy(tMtSrc, MODE_AUDIO, TRUE);
			}
		}
	}

	//update mtlist
	if(!tDelMt.IsLocal())
	{
		BOOL32 bFind = FALSE;
		TConfMcInfo *ptInfo = m_ptConfOtherMcTable->GetMcInfo(tDelMt.GetMcuId());
		if(ptInfo != NULL)
		{
			for(s32 nLoop=0; nLoop<MAXNUM_CONF_MT; nLoop++)
			{
				if(ptInfo->m_atMtExt[nLoop].GetMtId() != tDelMt.GetMtId())
				{
					continue;
				}
				ptInfo->m_atMtExt[nLoop].SetNull();
				ptInfo->m_atMtStatus[nLoop].SetNull();
				bFind = TRUE;
			}
		}
		// 需要上报 [pengguofeng 7/10/2013]
		if(bFind)
		{
			SendMtListToMcs(tDelMt.GetMcuId(), FALSE, FALSE);
		}

        // guzh [4/4/2007] 对轮询的影响处理
        u8 byIdx;
        if ( m_tConfPollParam.IsExistMt(tDelMt, byIdx) )
        {
            m_tConfPollParam.RemoveMtFromList(tDelMt);
			if ( tDelMt == (TMt)(m_tConf.m_tStatus.GetMtPollParam()) )
			{
				m_tConfPollParam.SpecPollPos(  byIdx );
				SetTimer( MCUVC_POLLING_CHANGE_TIMER, 10 );
			}
        }
	}

	if( m_cSMcuSpyMana.IsMtInSpyMember(tDelMt) )
	{
		FreeRecvSpy(tDelMt, MODE_BOTH, TRUE);
	}

	// 3、如果该终端是mcu，要删除所有该mcu下的多回传信息
	RemoveMcu( tDelMt );
	// End

	//发给会控会议所有终端信息
	SendAllMtInfoToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
}

/*====================================================================
    函数名      ：SendMtListToMcs
    功能        ：发送终端列表给会控
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/02/25    3.6         Jason         创建
	2007-7-5    4.7.2       pengguofeng    修改：参数如果为INVALID_MCUIDX,则表示请求所有下级MCU的列表
====================================================================*/
void CMcuVcInst::SendMtListToMcs(u16 wMcuIdx, const BOOL32 bForce/* = true*/,
								 const BOOL32 bMMcuReq /*= FALSE*/ )//(u8 byMcuId)
{
	if ( g_cMcuVcApp.GetMcuMcsMtInfoBufFlag() != 0					// 启用了缓冲发送
		&& !bForce)
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF,"[SendMtListToMcs]use buffer to send\n");

		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MCS, "Buf SendMtList of mcu.%d To Mcs\n", wMcuIdx);
		m_cMcuMtListSendBuf.Add(wMcuIdx);
		
		// [11/14/2011 liuxu] 由于大容量时SendAllMtInfoToAllMcs消息量太大, 这里采取缓冲发送
		if (m_byAllMtToAllMcsTimerFlag)
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "SendAllMtInfoToAllMcs wait timer, return\n");
			return;
		}else
		{
			m_byAllMtToAllMcsTimerFlag = 1;
			u16 wTemp = SetTimer(MCUVC_SENDALLMT_ALLMCS_TIMER, g_cMcuVcApp.GetRefreshMtInfoInterval());
			return;
		}
	}

	if (CurState() != STATE_ONGOING)
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF,"[SendMtListToMcs]wrong state.%d\n", CurState());
		return;
	}

	//发给会控终端表
	TMcu tMcu;
	tMcu.SetNull();
	tMcu.SetMcu( LOCAL_MCUID );
	tMcu.SetMcuIdx( wMcuIdx );
	
	CServMsg cServMsg;

	if(IsLocalMcuId(wMcuIdx))
	{
		tMcu.SetEqpId( m_ptMtTable->m_byMaxNumInUse );

		cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(tMcu) );
		cServMsg.CatMsgBody( ( u8 * )m_ptMtTable->m_atMtExt, 
		                  	 m_ptMtTable->m_byMaxNumInUse * sizeof( TMtExt ) );

		for ( u8 byMtId = 1; byMtId <= m_ptMtTable->m_byMaxNumInUse; byMtId ++ )
		{
			TMtAlias tMtAlias;
			if(!m_ptMtTable->GetMtAlias( byMtId, mtAliasTypeH320Alias, &tMtAlias ))
			{
				if(!m_ptMtTable->GetMtAlias( byMtId, mtAliasTypeH320ID, &tMtAlias ))
				{
					if(!m_ptMtTable->GetMtAlias( byMtId, mtAliasTypeH323ID, &tMtAlias ))
					{
						if(!m_ptMtTable->GetMtAlias( byMtId, mtAliasTypeE164, &tMtAlias ))
						{
							if( !m_ptMtTable->GetMtAlias( byMtId, mtAliasTypeTransportAddress, &tMtAlias ) )
							{
								m_ptMtTable->GetDialAlias( byMtId, &tMtAlias );
							}
						}
					}
				}		
			}
			
			cServMsg.CatMsgBody( (u8*)tMtAlias.m_achAlias, VALIDLEN_ALIAS + MAXLEN_CONFNAME );
		}
	}
	else
	{
		u16 wMMcuIdx = GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId());
		if ( !g_cMcuVcApp.IsShowMMcuMtList() && wMMcuIdx != INVALID_MCUIDX
			  && wMMcuIdx == wMcuIdx )
		{
			LogPrint(LOG_LVL_WARNING, MID_MCU_CONF, "[SendMtListToMcs ]NO need to send mtlist to MMcu\n");
			return;
		}

		u8 bySMcuNum = 0xFF;  //需要所有SMCU的，则发255，需要指定的，则自己组织若干个
		// send REQ to mtadp [pengguofeng 6/7/2013]
		u8 bySend2Mmcu = bMMcuReq?1:0;
		u8 abyMcuId[MAX_CASCADEDEPTH-1];
		memset( &abyMcuId[0],0,sizeof(abyMcuId) );
		CServMsg cSerMsg;
		cSerMsg.SetConfId(m_tConf.GetConfId());
		cSerMsg.SetConfIdx(m_byConfIdx);
 		if ( m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0] ) )
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[SendMtListToMcs]SMcu MtId:%d, %d\n", abyMcuId[0], abyMcuId[1]);
			
			bySMcuNum = 1;
			cSerMsg.SetMsgBody(&bySend2Mmcu, sizeof(u8));
			cSerMsg.CatMsgBody(&bySMcuNum, sizeof(u8));
			cSerMsg.CatMsgBody(&abyMcuId[1], sizeof(u8));
			SendMsgToMt(abyMcuId[0], MCU_MTADP_GETMTLIST_CMD, cSerMsg);
			return;
		}
		else
		{
			//foreach every smcu by mtid from 1 to 192
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[SendMtListToMcs]to All sub mcu\n");
			for ( u8 byMtID = 1;byMtID <= MAXNUM_CONF_MT; byMtID++)
			{
			//pengguofeng 2013-7-11:如果显示上级，也要发给上级
				if ( m_ptMtTable->GetMtType(byMtID) == MT_TYPE_SMCU 
					|| m_ptMtTable->GetMtType(byMtID) == MT_TYPE_MMCU 
						&& g_cMcuVcApp.IsShowMMcuMtList()
					)
				{
					cSerMsg.SetMsgBody(&bySend2Mmcu, sizeof(u8));
					cSerMsg.CatMsgBody(&bySMcuNum, sizeof(u8)); //此处是0xFF
					SendMsgToMt(byMtID, MCU_MTADP_GETMTLIST_CMD, cSerMsg);
				}
			}

			return;
		}
/*		TConfMcInfo* ptMcMtInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);	
		if(ptMcMtInfo == NULL)
		{
			return;
		}
		u16 wMMcuIdx = INVALID_MCUIDX;
		if( !m_tCascadeMMCU.IsNull() )
		{
			wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
			//m_tConfAllMcuInfo.GetIdxByMcuId( m_tCascadeMMCU.GetMtId(),0,&wMMcuIdx );
		}

        // guzh [4/30/2007] 上级MCU列表过滤
        if ( !g_cMcuVcApp.IsShowMMcuMtList() 
			&& !m_tCascadeMMCU.IsNull() 
			&& wMMcuIdx == wMcuIdx )
        {
            return;
        }

		cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(tMcu) );
		for(s32 nLoop =0; nLoop<MAXNUM_CONF_MT; nLoop++)
		{	
			if( ptMcMtInfo->m_atMtStatus[nLoop].IsNull() || 
				0 == ptMcMtInfo->m_atMtStatus[nLoop].GetMtId() ) //自己
			{
				continue;
			}

			cServMsg.CatMsgBody((u8 *)&(ptMcMtInfo->m_atMtExt[nLoop]), sizeof(TMtExt));	
		}
*/	}
	
	SendMsgToAllMcs( MCU_MCS_MTLIST_NOTIF, cServMsg ); // 只发的本级! [pengguofeng 7/2/2013]
}

/*====================================================================
    函数名      ：ProcMcuMcuSetMtChanReq
    功能        ：mcu<->mcu设置媒体通道状态
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/26    3.6         Jason         创建
====================================================================*/
void CMcuVcInst::ProcMcuMcuSetMtChanReq(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u8 byMtId	= cServMsg.GetSrcMtId() ;
	TMtMediaChanStatus*		ptMtChanStatus = (TMtMediaChanStatus* )(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq));

	STATECHECK;
    
	//目前不支持
	SendMsgToMt(byMtId, cServMsg.GetEventId()+2, cServMsg);
}

/*====================================================================
    函数名      ：ProcMcuMcuSetMtChanNotify
    功能        ：mcu<->mcu设置媒体通道状态通知
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/27    3.6         Jason         创建
====================================================================*/
void CMcuVcInst::ProcMcuMcuSetMtChanNotify(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	TMtMediaChanStatus*	ptMtChanStatus = (TMtMediaChanStatus* )(cServMsg.GetMsgBody());

	STATECHECK;

	u16 wMcuIdx = GetMcuIdxFromMcuId(  (u8)ptMtChanStatus->m_tMt.GetMcuId() );
	//m_tConfAllMcuInfo.GetIdxByMcuId( ptMtChanStatus->m_tMt.GetMcuId(),0,&wMcuIdx );

	TConfMcInfo *ptInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);//ptMtChanStatus->m_tMt.GetMcuId());
	if(ptInfo == NULL)
	{
		return;
	}
	for(s32 nLoop=0; nLoop<MAXNUM_CONF_MT; nLoop++)
	{
		if(ptInfo->m_atMtStatus[nLoop].GetMtId() != ptMtChanStatus->m_tMt.GetMtId())
		{
			continue;
		}
		TMcMtStatus *ptStatus = &(ptInfo->m_atMtStatus[nLoop]);
        // guzh [6/27/2007] 这里没有完全对应的字段，暂时通过是否在收发状态设置
        // 同样也可以设置为媒体通道类型为 MEDIA_TYPE_NULL
        // 当然目前KedaMcu不支持该通知
		if(ptMtChanStatus->m_byMediaMode == MODE_VIDEO)
		{
			if(ISTRUE(ptMtChanStatus->m_byIsDirectionIn))
			{   
				BOOL32 bSendVideo = !ptMtChanStatus->m_bMute;
                ptStatus->SetSendVideo(bSendVideo);
			}
			else
			{
				BOOL32 bRecvVideo = !ptMtChanStatus->m_bMute;
                ptStatus->SetReceiveVideo(bRecvVideo);
			}
		}
		else if(ptMtChanStatus->m_byMediaMode == MODE_AUDIO)
		{
			if(ISTRUE(ptMtChanStatus->m_byIsDirectionIn))
			{
				BOOL32 bSendAudio = !(ptMtChanStatus->m_bMute);
                ptStatus->SetSendAudio(bSendAudio);
				//ptStatus->SetAudioIn(GETBBYTE(!(ptMtChanStatus->m_bMute)));
			}
			else
			{
				BOOL32 bSendVideo = !(ptMtChanStatus->m_bMute);
                ptStatus->SetReceiveAudio(bSendVideo);
				//ptStatus->SetAudioOut(GETBBYTE(!(ptMtChanStatus->m_bMute)));
			}
		}
		else
		{
			//不支持数据会议
		}
			
	}
}

/*====================================================================
    函数名      ：ProcMcsMcuSpecOutViewReq
    功能        ：mcs<->mcu 指定终端回传到上级MCU请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/05/09    4.0         顾振华         创建
====================================================================*/
void CMcuVcInst::OnSetOutView( const TMt &tMcuSetInMt, u8 byMode )
{
    if ( m_tCascadeMMCU.IsNull() )
    {
        return;
    }

	TMt tUnlocalSetInMt = tMcuSetInMt;
	TMt tSetInMt = tMcuSetInMt;

	if( !IsLocalAndSMcuSupMultSpy(tSetInMt.GetMcuId()) )
	{
		if ( !tSetInMt.IsLocal() )
		{
			// 首先通知下级MCU
			OnMMcuSetIn( tSetInMt, 0, SWITCH_MODE_BROADCAST );
		}
	}

    TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
    if (NULL != ptConfMcInfo && ptConfMcInfo->m_tSpyMt == tSetInMt)
    {
		TMt tSrcMtVid;
		TMt tSrcMtAud;
		tSrcMtVid.SetNull();
		tSrcMtAud.SetNull();

        m_ptMtTable->GetMtSrc( m_tCascadeMMCU.GetMtId(),&tSrcMtVid,MODE_VIDEO );
		m_ptMtTable->GetMtSrc( m_tCascadeMMCU.GetMtId(),&tSrcMtAud,MODE_AUDIO );
		if( MODE_BOTH == byMode )
		{
			if( tSrcMtVid == ptConfMcInfo->m_tSpyMt && tSrcMtAud == ptConfMcInfo->m_tSpyMt )
			{
				// 相同则直接返回
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[OnSetOutView] tSpyMt(%d.%d) is equal tCascadeMMCU's VideoSrc(%d.%d) And AudioSrc(%d.%d).MODE_BOTH\n",
					ptConfMcInfo->m_tSpyMt.GetMcuId(),ptConfMcInfo->m_tSpyMt.GetMtId(),
					tSrcMtVid.GetMcuId(),tSrcMtVid.GetMtId(),
					tSrcMtAud.GetMcuId(),tSrcMtAud.GetMtId()
					);
				return ;
			}
		}
		else if( MODE_VIDEO == byMode )
		{
			if( tSrcMtVid == ptConfMcInfo->m_tSpyMt )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[OnSetOutView] tSpyMt(%d.%d) is equal tCascadeMMCU's VideoSrc(%d.%d).MODE_VIDEO\n",
					ptConfMcInfo->m_tSpyMt.GetMcuId(),ptConfMcInfo->m_tSpyMt.GetMtId(),
					tSrcMtVid.GetMcuId(),tSrcMtVid.GetMtId()					
					);
				return;
			}
		}
		else if( MODE_AUDIO == byMode )
		{
			if( tSrcMtAud == ptConfMcInfo->m_tSpyMt )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[OnSetOutView] tSpyMt(%d.%d) is equal tCascadeMMCU's AudioSrc(%d.%d).MODE_AUDIOn",
					ptConfMcInfo->m_tSpyMt.GetMcuId(),ptConfMcInfo->m_tSpyMt.GetMtId(),
					tSrcMtAud.GetMcuId(),tSrcMtAud.GetMtId()
					);
				return;
			}
		}
    }

	if(!m_tConf.m_tStatus.IsMixing())
	{
		//因为上传轮询可能第二级轮询第三级终端,第三级可能哑音,哑音不建交换,所以先删除上传的音频交换
		if( !tUnlocalSetInMt.IsLocal() &&
			( m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_SPEAKER_SPY ||			
				m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_SPEAKER_BOTH ||
				m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO_SPY||
				m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO_BOTH ) 
			)
		{
			StopSpyMtCascaseSwitch( MODE_AUDIO );
		}
	}

   	BOOL32 bMultSpyMt = FALSE;
	if (!tSetInMt.IsLocal() && IsLocalAndSMcuSupMultSpy(tSetInMt.GetMcuId()))
	{
		bMultSpyMt = TRUE;
	}
	else
	{
		tSetInMt = GetLocalMtFromOtherMcuMt(tSetInMt);
	}
    
    //TMt tMMcu;
    //tMMcu.SetMcuId(m_tCascadeMMCU.GetMtId());
    //tMMcu.SetMtId(0);
    //OnMMcuSetIn(tMMcu, 0, SWITCH_MODE_BROADCAST, TRUE); //不切换发言人，此时给上级发setin消息的作用？？？    
   
	if(NULL != ptConfMcInfo)
		ptConfMcInfo->m_tSpyMt = tSetInMt;
    
    //send output notify
    CServMsg cMsg;
    TSetOutParam tOutParam;
	TMsgHeadMsg tHeadMsg;
	memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[OnSetOutView] abyMcuId[0].%d, abyMcuId[1].%d>!\n", 
		tSetInMt.GetMcuId(), tSetInMt.GetMtId());

	tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tSetInMt,tSetInMt );
    tOutParam.m_nMtCount = 1;
    tOutParam.m_atConfViewOutInfo[0].m_tMt = tSetInMt;
	if (!bMultSpyMt)
	{
		tOutParam.m_atConfViewOutInfo[0].m_tMt.SetMcuId( LOCAL_MCUID );
	}
	if (NULL != ptConfMcInfo)
	{
		tOutParam.m_atConfViewOutInfo[0].m_nOutViewID = ptConfMcInfo->m_dwSpyViewId;
	    tOutParam.m_atConfViewOutInfo[0].m_nOutVideoSchemeID = ptConfMcInfo->m_dwSpyVideoId;
	}
	
	//tHeadMsg.m_tMsgSrc.m_tMt = tSetInMt;
	cMsg.SetMsgBody((u8 *)&tOutParam, sizeof(tOutParam));
	cMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
    cMsg.SetEventId(MCU_MCU_SETOUT_NOTIF);
    SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MCU_SETOUT_NOTIF, cMsg);
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[OnSetOutView] abyMcuId[0].%d, abyMcuId[1].%d>!\n", 
		tSetInMt.GetMcuId(), tSetInMt.GetMtId());
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[OnSetOutView] tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0].%d, tHeadMsg.m_tMsgSrc.m_byCasLevel.%d>!\n", 
		tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0], tHeadMsg.m_tMsgSrc.m_byCasLevel);
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[OnSetOutView] tHeadMsg.m_tMsgDst.m_abyMtIdentify[0].%d, tHeadMsg.m_tMsgDst.m_byCasLevel.%d>!\n", 
		tHeadMsg.m_tMsgDst.m_abyMtIdentify[0], tHeadMsg.m_tMsgDst.m_byCasLevel);

	if(ptConfMcInfo != NULL)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[OnSetOutView] tSpyMt(%d.%d)\n", ptConfMcInfo->m_tSpyMt.GetMcuId(),ptConfMcInfo->m_tSpyMt.GetMtId());
	}
	else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[OnSetOutView] ptConfMcInfo == NULL!\n");
	}

    NotifyMtSend(tSetInMt.GetMtId(), byMode);
    
    //混音优先
    if(m_tConf.m_tStatus.IsMixing())
    {
        byMode = MODE_VIDEO;
    }
	//先停掉往上级mcu打的交换，不停的话StartSwitchToMcu如果过适配建不成功交换会遗留上次上传终端的视频交换
	StopSwitchToSubMt(1,&m_tCascadeMMCU,byMode);
	// 多回传下级终端时，需要用非Local的终端建多回传交换，其它情况用Local后的成员建交换
	if (bMultSpyMt)
	{
		StartSwitchToMcu(tUnlocalSetInMt, 0, m_tCascadeMMCU.GetMtId(), byMode, SWITCH_MODE_SELECT);
	}
	else
	{
		StartSwitchToMcu(tSetInMt, 0, m_tCascadeMMCU.GetMtId(), byMode, SWITCH_MODE_SELECT);
	}

	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
    if (tConfAttrb.IsResendLosePack())
    {
        //把目的视频Rtcp交换给源
        u32 dwDstIp = 0;
        u16 wDstPort = 0;
        
		//[2011/08/18/zhangli]这里取端口是用tSetInMt，如果是下级终端且多回传，就取错了，改用BuildRtcpSwitchForSrcToDst
		//如果需要过适配就不用建了，StartSwitchToMcu里面有适配的处理，这里可能会覆盖掉
        if ((byMode == MODE_VIDEO || byMode == MODE_BOTH) && !IsNeedSelAdpt(tUnlocalSetInMt, m_tCascadeMMCU, MODE_VIDEO))
        {
			BuildRtcpSwitchForSrcToDst(m_tCascadeMMCU, tUnlocalSetInMt, MODE_VIDEO);

//             m_ptMtTable->GetMtLogicChnnl(tSetInMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);
// 
//             dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
//             wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
//             // guzh [7/25/2007] 回传通道也要Map
//             //将 MT.RTCP -> PRS 时，考虑到防火墙支撑，需将交换包的源ip、port映射为 MT.RTP，以便PRS重传        
//             AddRemoveRtcpToDst(dwDstIp, wDstPort, m_tCascadeMMCU, 0, MODE_VIDEO, TRUE);
        }
        
        if ((byMode == MODE_AUDIO || byMode == MODE_BOTH) && !IsNeedSelAdpt(tUnlocalSetInMt, m_tCascadeMMCU, MODE_AUDIO))
        {
			BuildRtcpSwitchForSrcToDst(m_tCascadeMMCU, tUnlocalSetInMt, MODE_AUDIO);

            //把目的音频Rtcp交换给源
//             m_ptMtTable->GetMtLogicChnnl(tSetInMt.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE);
//             
//             dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
//             wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
//             AddRemoveRtcpToDst(dwDstIp, wDstPort, m_tCascadeMMCU, 0, MODE_AUDIO, TRUE);
        }
    }   
    
    // guzh [5/9/2007] 通知上级新的视频/音频源
    NofityMMcuMediaInfo();
}

/*====================================================================
    函数名      ：ProcMcsMcuSpecOutViewReq
    功能        ：mcs<->mcu 指定终端回传到上级MCU请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/05/09    4.0         顾振华         创建
====================================================================*/
void CMcuVcInst::ProcMcsMcuSpecOutViewReq(  const CMessage * pcMsg )
{
    STATECHECK;

    CServMsg cServMsg( pcMsg->content, pcMsg->length );
    TMt tSetInMt = *(TMt* )(cServMsg.GetMsgBody());
    
    if ( m_tCascadeMMCU.IsNull() )
    {
        //NACK
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        return;
    }

	u16 wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
	TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMMcuIdx);//m_tCascadeMMCU.GetMtId());
    if(ptConfMcInfo == NULL)
    {
        //NACK
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        return;
    }    
	//ACK
    SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

	if( !IsCanSetOutView(tSetInMt,MODE_BOTH) )
	{	
		//cServMsg.SetErrorCode( wErrorCode );
		//SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	// 拖终端进回传通道之前通道中有终端存在则先释放回传通道
	TMtStatus tMtStatus;
	m_ptMtTable->GetMtStatus(m_tCascadeMMCU.GetMtId(), &tMtStatus);
	TMt tLastSpyMt = tMtStatus.GetVideoMt();
	if ( !tLastSpyMt.IsNull() )
	{
		FreeRecvSpy( tLastSpyMt, MODE_BOTH );
	}

    // 上级向下级发PreSetIn请求：由此处统一建立拖下级终端进上传通道回传
    if( !tSetInMt.IsLocal() &&
		IsLocalAndSMcuSupMultSpy(tSetInMt.GetMcuId()) 
		)
	{
		TPreSetInReq tSpySrcInitInfo;
		tSpySrcInitInfo.m_tSpyMtInfo.SetSpyMt( tSetInMt );
		tSpySrcInitInfo.m_bySpyMode = MODE_BOTH;
		tSpySrcInitInfo.SetEvId(MCS_MCU_SPECOUTVIEW_REQ);
		
		// 填目的端能力
		TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSetInMt.GetMcuId()) );	
		// 如果当前终端已回传则能力集要与已回传目的能力集取小
		if(!GetMinSpyDstCapSet(tSetInMt, tSimCapSet))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcPollingChangeTimerMsg] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
				tSetInMt.GetMcuId(), tSetInMt.GetMtId() );
			return;
		}
		tSpySrcInitInfo.m_tSpyMtInfo.SetSimCapset( tSimCapSet );

		OnMMcuPreSetIn( tSpySrcInitInfo );			
		return;
	}

    // 指定回传
    OnSetOutView(tSetInMt);
}

/*====================================================================
    函数名      ：ProcMcuMcuSetInReq
    功能        ：mcu<->mcu设置终端输入到某个视图请求
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/27    3.6         Jason         创建
====================================================================*/
void CMcuVcInst::ProcMcuMcuSetInReq(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u8 byMtId = cServMsg.GetSrcMtId();
	TSetInParam* ptSetInParam = (TSetInParam* )(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq));

	TMsgHeadMsg tHeadMsg,tHeadMsgRsp;
	if( cServMsg.GetMsgBodyLen() > (sizeof(TMcuMcuReq)+sizeof(TSetInParam)) )
	{
		tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq)+sizeof(TSetInParam));		
		tHeadMsgRsp.m_tMsgSrc = tHeadMsg.m_tMsgDst;
		tHeadMsgRsp.m_tMsgDst = tHeadMsg.m_tMsgSrc;
	}
	

	STATECHECK;

	if( ptSetInParam->m_tMt.IsMcuIdLocal() &&
		ptSetInParam->m_tMt.GetMtId() == byMtId &&
		m_ptMtTable->GetMtType(byMtId) == MT_TYPE_SMCU
		)
	{
		//ACK		
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		return;
	}
	
	TMt tRealSetInMt = ptSetInParam->m_tMt;
	if( !ptSetInParam->m_tMt.IsMcuIdLocal() )
	{
		ptSetInParam->m_tMt = m_ptMtTable->GetMt( (u8)ptSetInParam->m_tMt.GetMcuId() );
	}
	else
	{
		ptSetInParam->m_tMt = m_ptMtTable->GetMt(ptSetInParam->m_tMt.GetMtId());
	}

	//tHeadMsgRsp.m_tMsgSrc.m_tMt = ptSetInParam->m_tMt;
	cServMsg.SetMsgBody( (u8*)&tHeadMsgRsp,sizeof(TMsgHeadMsg) );
	cServMsg.CatMsgBody( (u8*)&tRealSetInMt,sizeof(TMt) );
	
	

	u16 wMMcuIdx = GetMcuIdxFromMcuId( byMtId );
	//m_tConfAllMcuInfo.GetIdxByMcuId( byMtId,0,&wMMcuIdx );
	TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMMcuIdx);
	if(ptConfMcInfo == NULL)
	{
		//NACK		
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}
    
    //zbq [12/25/2009] 正常的setout处理开启，本地回传轮询停止
    u8 byPollMode = m_tConf.m_tStatus.GetPollMode();
    if (CONF_POLLMODE_NONE != byPollMode)
    {
        switch (byPollMode)
        {
        case CONF_POLLMODE_VIDEO_SPY:
        case CONF_POLLMODE_SPEAKER_SPY:
            ProcStopConfPoll();
            NotifyMcsAlarmInfo(0, ERR_MCU_CASPOLL_STOP);
            break;

        case CONF_POLLMODE_VIDEO_BOTH:
            m_tConf.m_tStatus.SetPollMedia(MODE_VIDEO);
            m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_VIDEO);
            NotifyMcsAlarmInfo(0, ERR_MCU_CASPOLL_STOP);
            break;
        
        case CONF_POLLMODE_SPEAKER_BOTH:
            m_tConf.m_tStatus.SetPollMedia(MODE_BOTH);
            m_tConf.m_tStatus.SetPollMode(CONF_POLLMODE_SPEAKER);
            NotifyMcsAlarmInfo(0, ERR_MCU_CASPOLL_STOP);
        	break;

		default:
			break;
        }
    }

	TMt tMt;
	//tMt.SetMcuId(byMtId);
	//tMt.SetMtId(0);
	//OnMMcuSetIn(tMt, 0, SWITCH_MODE_BROADCAST, TRUE); //不切换发言人，此时给上级发setin消息的作用？？？
	
	
	
	if( tRealSetInMt.IsMcuIdLocal() )
	{
		//send output notify
		CServMsg cMsg;
		TSetOutParam tOutParam;
		tOutParam.m_nMtCount = 1;
		tOutParam.m_atConfViewOutInfo[0].m_tMt = tRealSetInMt;//ptSetInParam->m_tMt;//m_ptMtTable->GetMt(byMtId);
		tOutParam.m_atConfViewOutInfo[0].m_nOutViewID = ptConfMcInfo->m_dwSpyViewId;
		tOutParam.m_atConfViewOutInfo[0].m_nOutVideoSchemeID = ptConfMcInfo->m_dwSpyVideoId;
		memset( &tHeadMsg,0,sizeof(TMsgHeadMsg) );
		tHeadMsg = tHeadMsgRsp;
		//tHeadMsg.m_tMsgSrc.m_tMt = tRealSetInMt;//ptSetInParam->m_tMt;
		cMsg.SetMsgBody((u8 *)&tOutParam, sizeof(tOutParam));
		cMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
		
		cMsg.SetEventId(MCU_MCU_SETOUT_NOTIF);
		SendMsgToMt(byMtId, MCU_MCU_SETOUT_NOTIF, cMsg);
	}

	ptConfMcInfo->m_tSpyMt = ptSetInParam->m_tMt;
	NotifyMtSend( ptSetInParam->m_tMt.GetMtId(), MODE_BOTH);
	TMt tDstMt = m_ptMtTable->GetMt(byMtId);
	TMt tSrcMt = ptSetInParam->m_tMt;

	
	


	NofityMMcuMediaInfo();
	
   
	//混音优先
	u8 byMode = MODE_BOTH;

	
	TPeriEqpStatus tPeriEqpStatus;
	g_cMcuVcApp.GetPeriEqpStatus( m_tMixEqp.GetEqpId(), &tPeriEqpStatus );
	
	
	

	if( m_tConf.m_tStatus.IsMixing() || 
		( !m_tMixEqp.IsNull() && 
		tPeriEqpStatus.m_tStatus.tMixer.m_atGrpStatus[m_byMixGrpId].m_byGrpState != TMixerGrpStatus::READY
		)
		)
	{
		byMode = MODE_VIDEO;
	}

	tSrcMt.SetMcuIdx( LOCAL_MCUIDX );
    StartSwitchToMcu(tSrcMt, 0, tDstMt.GetMtId(), byMode, SWITCH_MODE_SELECT);
	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
    if (tConfAttrb.IsResendLosePack())
    {
		u32 dwDstIp  = 0;
		u16 wDstPort = 0;
		TLogicalChannel tLogicalChannel;
		if ((MODE_VIDEO == byMode || MODE_BOTH == byMode))
		{
			if (IsNeedSelAdpt(tSrcMt, tDstMt, MODE_VIDEO))
			{
				TBasOutInfo tOutInfo;
				if (FindBasChn2SelForMt(tSrcMt, tDstMt, MODE_VIDEO, tOutInfo))
				{
					TTransportAddr tBasWaitRtcpAddr;
					if (GetRemoteRtcpAddr(tOutInfo.m_tBasEqp, tOutInfo.m_byFrontOutNum + tOutInfo.m_byOutIdx, MODE_VIDEO, tBasWaitRtcpAddr))
					{
						AddRemoveRtcpToDst(tBasWaitRtcpAddr.GetIpAddr(), tBasWaitRtcpAddr.GetPort(), tDstMt, 0, MODE_VIDEO, TRUE);
					}
					else
					{
						ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcMcuMcuSetInReq] GetPrsRemoteAddr:Video failed!\n");
					}				
				}
				else
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcMcuMcuSetInReq] FindBasChn2SelForMt:Video failed!\n");
				}
			}
			else
			{			
				m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);				
				dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
				wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();       
				AddRemoveRtcpToDst(dwDstIp, wDstPort, tDstMt, 0, MODE_VIDEO, TRUE);
			}
		}
        
		if ((MODE_AUDIO == byMode || MODE_BOTH == byMode))
		{
			if (IsNeedSelAdpt(tSrcMt, tDstMt, MODE_AUDIO))
			{
				TBasOutInfo tOutInfo;
				if (FindBasChn2SelForMt(tSrcMt, tDstMt, MODE_AUDIO, tOutInfo))
				{
					TTransportAddr tBasWaitRtcpAddr;
					if (GetRemoteRtcpAddr(tOutInfo.m_tBasEqp, tOutInfo.m_byFrontOutNum + tOutInfo.m_byOutIdx, MODE_AUDIO, tBasWaitRtcpAddr))
					{
						AddRemoveRtcpToDst(tBasWaitRtcpAddr.GetIpAddr(), tBasWaitRtcpAddr.GetPort(), tDstMt, 0, MODE_AUDIO, TRUE);
					}
					else
					{
						ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcMcuMcuSetInReq] GetPrsRemoteAddr:Audio failed!\n");
					}				
				}
				else
				{
					ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcMcuMcuSetInReq] FindBasChn2SelForMt:Audio failed!\n");
				}
			}
			else
			{
				//把目的音频Rtcp交换给源
				m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE);				
				dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
				wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
				AddRemoveRtcpToDst(dwDstIp, wDstPort, tDstMt, 0, MODE_AUDIO, TRUE);
			}
		}     
    }

	//ACK
	
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

	if( !tRealSetInMt.IsMcuIdLocal() )
	{
		tMt = GetMcuIdxMtFromMcuIdMt( tRealSetInMt );
		// [12/8/2010 xliang] further send SetInReq to it's smcu no matter multiSpy support or not
// 		if( IsLocalAndSMcuSupMultSpy( tMt.GetMcuId() ) )
// 		{
// 			TPreSetInReq tPreSetInReq;
// 			tPreSetInReq.m_tSpyMtInfo.SetSpyMt(tMt);
// 			tPreSetInReq.m_bySpyMode = byMode;
// 			tPreSetInReq.m_tSpyInfo.m_tSpySwitchInfo.m_tDstMt = m_tCascadeMMCU;
// 			//暂定事件ID为选看
// 			tPreSetInReq.m_dwEvId = MCS_MCU_STARTSWITCHMT_REQ;
// 			if( m_cSMcuSpyMana.GetFstSpyMtByMcuIdx(tMt.GetMcuId(),tMt) )
// 			{
// 				tPreSetInReq.m_tReleaseMtInfo.m_tMt = tMt;
// 				tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode = byMode;
// 				tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum = 
// 				tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum = 1;
// 			}			
// 			tPreSetInReq.m_tReleaseMtInfo.m_swCount = 0;
// 			
// 			// [pengjie 2010/9/13] 填目的端能力
// 			TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tMt.GetMcuId()) );	
// 			//zjl20101116 如果当前终端已回传则能力集要与已回传目的能力集取小
// 			tSimCapSet = GetMinSpyDstCapSet(tMt, tSimCapSet);
// 			if(tSimCapSet.IsNull())
// 			{
// 				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ChangeSpeaker] Get Mt(mcuid.%d, mtid.%d) SimCapSet Failed !\n",
// 					tMt.GetMcuId(), tMt.GetMtId() );
// 				return;
// 			}
// 			
// 			tPreSetInReq.m_tSpyMtInfo.SetSimCapset( tSimCapSet );
// 		    // End
// 
// 			OnMMcuPreSetIn( tPreSetInReq );
// 		}
// 		else
		{
			OnMMcuSetIn(tMt, 0, SWITCH_MODE_SELECT);
		}
		
	}
	
	if( !m_tCascadeMMCU.IsNull() && m_ptMtTable->GetMtTransE1(m_tCascadeMMCU.GetMtId()) )
	{
		ProcMMcuBandwidthNotify( m_tCascadeMMCU,GetRealSndSpyBandWidth() );
	}

    return;
}

/*====================================================================
    函数名      ：ProcMcuMcuSetOutReq
    功能        ：mcu<->mcu请求某个视图输入到终端
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/27    3.6         Jason         创建
====================================================================*/
void CMcuVcInst::ProcMcuMcuSetOutReq(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u8 byMtId = cServMsg.GetSrcMtId();
	TSetOutParam* ptSetOutParam = (TSetOutParam* )(cServMsg.GetMsgBody()+sizeof(TMcuMcuReq));

	STATECHECK;

	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

    return;
}

/*====================================================================
    函数名      ：ProcMcuMcuSetOutNotify
    功能        ：mcu<->mcu某个视图输入到终端通知
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/27    3.6         Jason         创建
====================================================================*/
void CMcuVcInst::ProcMcuMcuSetOutNotify(  const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u8 byMtId = cServMsg.GetSrcMtId();
	
	TCConfViewOutInfo* ptSetOutParam = (TCConfViewOutInfo* )(cServMsg.GetMsgBody());
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody()+sizeof(TCConfViewOutInfo));

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcuMcuSetOutNotify] ptSetOutParam->m_tMt<McuId.%d, MtId.%d>!\n", 
											  ptSetOutParam->m_tMt.GetMcuId(), ptSetOutParam->m_tMt.GetMtId());

	STATECHECK;
	
	if (!ptSetOutParam)
	{
		return;
	}

	u8 byFstMcuId = byMtId;
	u8 bySecMcuId = 0;
	
	if( tHeadMsg.m_tMsgSrc.m_byCasLevel > 0 )
	{
		bySecMcuId = ptSetOutParam->m_tMt.GetMtId();
	}
	
	u16 wMcuIdx = INVALID_MCUIDX;
	u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
	abyMcuId[0] = byFstMcuId;
	abyMcuId[1] = bySecMcuId;
	if( !m_tConfAllMcuInfo.GetIdxByMcuId( &abyMcuId[0],2,&wMcuIdx ) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU,  "[ProcMcuMcuCallAlertMtNotify] Fail to get mcuIdx by mcuid(%d.%d)\n",
			byFstMcuId,bySecMcuId
			);
		return;
	}
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMcuMcuSetOutNotify] abyMcuId[0].%d, abyMcuId[1].%d>!\n", 
		byFstMcuId, bySecMcuId);
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMcuMcuSetOutNotify] tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0].%d, tHeadMsg.m_tMsgSrc.m_byCasLevel.%d>!\n", 
		tHeadMsg.m_tMsgSrc.m_abyMtIdentify[0], tHeadMsg.m_tMsgSrc.m_byCasLevel);
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[ProcMcuMcuSetOutNotify] tHeadMsg.m_tMsgDst.m_abyMtIdentify[0].%d, tHeadMsg.m_tMsgDst.m_byCasLevel.%d>!\n", 
		tHeadMsg.m_tMsgDst.m_abyMtIdentify[0], tHeadMsg.m_tMsgDst.m_byCasLevel);

	//if(ptSetOutParam->m_tMt.IsNull())
	//{
	//	return;
	//}
	TMt tSetoutMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc,ptSetOutParam->m_tMt );//ptSetOutParam->m_tMt;
	//tSetoutMt.SetMcuIdx( wMcuIdx );
	
	wMcuIdx = tSetoutMt.GetMcuId();
	
	//是第2级设第3级终端进上传通道，此时应该取第2级的mcinfo
	//3级可以用下面方法，扩展后要修改
	if ( tHeadMsg.m_tMsgDst.m_byCasLevel == 1 && abyMcuId[1] != 0 ) 
	{
		wMcuIdx = GetMcuIdxFromMcuId(GetFstMcuIdFromMcuIdx(tSetoutMt.GetMcuId())) ;
	}
	else // >1的情况现在只有=2也就是第3级终端，以后扩展else里面要修改
	{
		
	}
	TConfMcInfo *ptInfo = m_ptConfOtherMcTable->GetMcInfo( wMcuIdx );
	if(ptInfo == NULL)
	{
		return;
	}
	TConfMcInfo * ptSetoutMtMcinfo = m_ptConfOtherMcTable->GetMcInfo(tSetoutMt.GetMcuId());
	if(ptInfo == NULL || ptSetoutMtMcinfo== NULL )
	{
		return;
	}
	TMcMtStatus *ptMcMtStatus = ptSetoutMtMcinfo->GetMtStatus(tSetoutMt);//ptSetOutParam->m_tMt);
	if(ptMcMtStatus ==  NULL)
	{
		return;
	}
    TMtVideoInfo tInfo = ptMcMtStatus->GetMtVideoInfo();
	tInfo.m_nOutputLID = ptSetOutParam->m_nOutViewID;
	tInfo.m_nOutVideoSchemeID = ptSetOutParam->m_nOutVideoSchemeID;
    ptMcMtStatus->SetMtVideoInfo(tInfo);

	//TMt tOldSrc;
    //zjl[20091226]新的回传源到上级后，统一更新其他外设通道的成员状态
	TMt tNewSrc = ptInfo->m_tMMcuViewMt;
	if( tNewSrc.IsNull() || !( tNewSrc==tSetoutMt ) )
	{
		// liuxu, 20110120, 防止ptSetOutParam->m_tMt为非法值
		// 针对的问题: sp4级联r2时, r2的终端进入sp4的电视墙,不能显示正确的名字
		//             原因在于r2在这里上报了一个错误的tmt
		if ( ptSetOutParam->m_tMt.IsNull() 
			|| (! ptSetOutParam->m_tMt.IsNull() && 0 != ptSetOutParam->m_tMt.GetMtId()))
		{
			// [2013/10/11 chenbing] 
			// 新的上传源为空，先清除上一上传源的图标
			// 防止下级进行上传轮询，上级拖未轮询终端进行其他操作时，不能清除上一上传源图标
			if (tSetoutMt.IsNull())
			{
				RefreshMtStatusInTw(ptInfo->m_tLastMMcuViewMt, FALSE, TRUE);
			}
			ptInfo->m_tLastMMcuViewMt = ptInfo->m_tMMcuViewMt;
			tNewSrc = tSetoutMt;//ptSetOutParam->m_tMt;
			ptInfo->m_tMMcuViewMt = tNewSrc;
		}
	}

	// [3/28/2011 xliang] tOldSrc按上面的方式确定有问题
	// 它只试用于下级指定上传终端的情况，对于上级指定回传源，该值失去意义
	TMt tOldSrc = ptInfo->m_tLastMMcuViewMt;
	
	BOOL32 bIsStartSwitch = TRUE;
	// [2013/04/26 chenbing] 单回传调整分辨率逻辑
	if ( !IsLocalAndSMcuSupMultSpy(tSetoutMt.GetMcuId()))
	{
		if (!tNewSrc.IsNull())
		{
			//单回传下，新的上传源分辨率必须下调
			if ( IsViewMtInHduVmp(tNewSrc, &tOldSrc) )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuSetOutNotify] !IsSupMultSpy AdjustResAndFps\n");
				ChangeMtResFpsInHduVmp(tNewSrc, 0, 0, 0, bIsStartSwitch, TRUE, TRUE);
 				if (!bIsStartSwitch)
 				{
 					TMt tTMtTemp = tOldSrc.IsNull() ? GetLocalMtFromOtherMcuMt(tNewSrc) : tOldSrc;
 					// 非科达终端不能降分辨率拆除到HDU2多画面的交换
 					StopViewToAllHduVmpSwitch(tTMtTemp);
 				}
			}
		}

		if ( !IsOldViewMtAndNewViewMtSame(tNewSrc, tOldSrc) )
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuSetOutNotify] !IsSupMultSpy Recover AdjustResAndFps\n");
			// 恢复旧上传源其分辨率(单回传下需要强制恢复)
			ChangeMtResFpsInHduVmp(tOldSrc, 0, 0, 0, bIsStartSwitch, FALSE, TRUE);
		}
	}
	else// [2013/04/26 chenbing] 多回传调整分辨率逻辑
	{	
		if (!tNewSrc.IsNull())
		{
			//MCU或Mt在通道中, 需要调
			if (IsViewMtInHduVmp(tNewSrc) )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuSetOutNotify] IsSupMultSpy AdjustResAndFps\n");
				ChangeMtResFpsInHduVmp(tNewSrc, 0, 0, 0, bIsStartSwitch, TRUE, TRUE);
 				if (!bIsStartSwitch)
 				{
 					TMt tTMtTemp = GetLocalMtFromOtherMcuMt(tNewSrc);
 					// 非科达终端不能降分辨率拆除下级Mcu到HDU2多画面的交换
 					StopViewToAllHduVmpSwitch(tTMtTemp);
 				}
			}
		}

		// 恢复旧上传源其分辨率
		if ( !IsViewMtInHduVmp(tOldSrc, NULL, FALSE) )
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuSetOutNotify] IsSupMultSpy Recover AdjustResAndFps\n");
			ChangeMtResFpsInHduVmp(tOldSrc, 0, 0, 0, bIsStartSwitch, FALSE);
		}
	}

	// [10/18/2011 liuxu] hdu电视墙通道更新
	if( m_tConf.GetConfSource() != VCS_CONF 
		|| ( m_tConf.GetConfSource() == VCS_CONF ) 
			&& ( VCS_TVWALLMANAGE_REVIEW_MODE != m_cVCSConfStatus.GetTVWallManageMode() ) )
	{
		// 更新hdu设备状态
		UpdateTvwOnSMcuViewMtChg( tNewSrc, TRUE, &tOldSrc );

		// 更新tvwall设备状态( 会自动调整分辨率 ) 
		UpdateTvwOnSMcuViewMtChg( tNewSrc, FALSE, &tOldSrc );
	}

// 	//hdu
// 	//zjj20091128 vcs会议不要去更新hdu的终端，因为vcs预览模式下电视墙管理比较特殊有可能会存放不是回传源的终端信息
//     if ( /*!IsLocalAndSMcuSupMultSpy( tNewSrc.GetMcuIdx() ) &&*/
// 		m_tConf.GetConfSource() != VCS_CONF &&
// 		tNewSrc.GetType() != TYPE_MCUPERI && !tNewSrc.IsLocal())
// 	{
// 		for (u8 byIdx = HDUID_MIN; byIdx <= HDUID_MAX; byIdx++)
// 		{
// 			if (!g_cMcuVcApp.IsPeriEqpConnected(byIdx))
// 			{
// 				continue;
// 			}
// 
// 			u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byIdx));
// 			if (0 == byHduChnNum)
// 			{
// 				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuSetOutNotify] GetHduChnNumAcd2Eqp failed!\n");
// 				continue;
// 			}
// 
// 			TPeriEqpStatus tStatus;
// 			g_cMcuVcApp.GetPeriEqpStatus(byIdx, &tStatus);
// 			//20101015_tzy 上传通道中终端改变时HDU电视墙图标刷新
// 			//分以下几种情况进行考虑（考虑撤销上传终端时的操作）注意与标清电视墙的区别，标清还要调整分辨率
// 			//遍历所有HDU通道
// 			//当电视墙通道中为下级MCU时，又分两种情况：
// 				//1.该上传终端属于该下级
// 					//单回传时，通道中终端不变，但原来上传终端电视墙图标刷掉，新的上传终端电视墙图标刷上
// 					//多回传时，//UNDO
// 				//1.该上传终端不属于该下级MCU时
// 					//UNDO
// 			//当电视墙通道中为下级终端时，又分两种情况：
// 				//1.该上传终端属于该下级MCU时
// 					//单回传时，通道中终端改为新的上传终端，且原来上传终端电视墙图标刷掉，新的上传终端电视墙图标刷上
// 					//多回传时，//UNDO
// 				//1.该上传终端不属于该下级MCU时
// 					//UNDO
// 
// 			for (u8 byChnId = 0; byChnId < byHduChnNum; byChnId++)//遍历所有HDU通道
// 			{
// 				TMtStatus tMtStatus;
// 				TMt tCurSpyMt = (TMt)tStatus.m_tStatus.tHdu.atVideoMt[byChnId];
// 
// 				//通道终端是否合法检查
// 				if (tCurSpyMt.IsNull() || tCurSpyMt == m_tVmpEqp || tCurSpyMt.GetConfIdx() != m_byConfIdx )
// 				{
// 					continue;
// 				}
// 				
// 				// 2011-10-13 add by pgf: 加入过滤条件，判断会议号
// 				if ( tCurSpyMt.GetConfIdx() != m_byConfIdx
// 					|| tNewSrc.GetConfIdx() != m_byConfIdx
// 					|| tOldSrc.GetConfIdx() != m_byConfIdx)
// 				{
// 					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "CONF IDX: m_byConfIdx:%d curspymt<%d> or mmcuviewmt<old:%d new:%d>\n",
// 						m_byConfIdx, tCurSpyMt.GetConfIdx(), tNewSrc.GetConfIdx(), tOldSrc.GetConfIdx());
// 					continue;
// 				}
// 
// 				// [pengjie 2011/9/27] 刷下级mcu的上传源状态
// 				if( /*tNewSrc.IsNull() &&*/ IsMtInMcu(tCurSpyMt, tOldSrc) && IsMtNotInOtherHduChnnl(tOldSrc, 0, 255))
// 				{
// 					RefreshMtStatusInTw( tOldSrc, FALSE, TRUE );
// 				}
// 				
// 				if( !tNewSrc.IsNull() && IsSMcuViewMtAndInTw( tNewSrc, TRUE )/*IsMtInMcu(tCurSpyMt, tNewSrc, TRUE)*/)
// 				{
// 					RefreshMtStatusInTw( tNewSrc, TRUE, TRUE );
// 				}
// 				
// // 				// [pengjie 2011/9/27] 刷下级mcu的上传源状态
// // 				if( IsMtInMcu(tCurSpyMt, tNewSrc) || (tNewSrc.IsNull() && IsMtInMcu(tCurSpyMt, tOldSrc)) )
// // 				{
// // 					RefreshMtStatusInTw( tOldSrc, FALSE, TRUE );
// // 					
// // 					if( !tNewSrc.IsNull() )
// // 					{
// // 						RefreshMtStatusInTw( tNewSrc, TRUE, TRUE );
// // 					}
// // 				}
// 
// 				//通道终端为下级终端并且单回传时
// 				if (!tCurSpyMt.IsLocal() 
// 					&& !IsLocalAndSMcuSupMultSpy(tCurSpyMt.GetMcuIdx()) 
// 					&& !(tCurSpyMt == tNewSrc)
// 					&& (!IsMtInMcu(tCurSpyMt, tNewSrc) && !IsMtInMcu(tNewSrc, tCurSpyMt)))
// 				{
// 					GetMtStatus(tCurSpyMt, tMtStatus);
// 					tMtStatus.SetInHdu(FALSE);
// 					SetMtStatus(tCurSpyMt, tMtStatus);
// 					MtStatusChange(&tCurSpyMt, TRUE);
// 					
// 					GetMtStatus(tNewSrc, tMtStatus);
// 					tMtStatus.SetInHdu(TRUE);
// 					SetMtStatus(tNewSrc, tMtStatus);
// 					MtStatusChange(&tNewSrc, TRUE);
// 
// 					tStatus.m_tStatus.tHdu.atVideoMt[byChnId].SetMt(tNewSrc);
// 					g_cMcuVcApp.SetPeriEqpStatus(byIdx, &tStatus);
// 					cServMsg.SetMsgBody((u8 *)&tStatus, sizeof(tStatus));
// 					SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
// 				}
// 			}	
// 		}				
// 	}
		
// 	//dec5
// 	//zjj20091128 vcs会议不要去更新hdu的终端，因为vcs预览模式下电视墙管理比较特殊有可能会存放不是回传源的终端信息
// 	if ( /*!IsLocalAndSMcuSupMultSpy( tNewSrc.GetMcuIdx() ) &&*/ // [pengjie 2010/10/13] 这里不区分多回传
// 		tNewSrc.GetType() != TYPE_MCUPERI 
// 		&& !tNewSrc.IsLocal()
// 		// [10/17/2011 liuxu] VCS模式下需要更改状态
// 		&& ( m_tConf.GetConfSource() != VCS_CONF 
// 			|| ( m_tConf.GetConfSource() == VCS_CONF ) && ( VCS_TVWALLMANAGE_REVIEW_MODE != m_cVCSConfStatus.GetTVWallManageMode() )  )
// 		)
// 	{
// 		for (u8 byIdx = TVWALLID_MIN; byIdx <= TVWALLID_MAX; byIdx++)
// 		{
// 			if (!g_cMcuVcApp.IsPeriEqpConnected(byIdx))
// 			{
// 				continue;
// 			}
// 			TPeriEqpStatus tStatus;
// 			g_cMcuVcApp.GetPeriEqpStatus(byIdx, &tStatus);
// 			for (u8 byChnId = 0; byChnId < MAXNUM_PERIEQP_CHNNL; byChnId++)
// 			{	
// 				//20101021_tzy 上传通道中终端改变时电视墙图标刷新
// 				TMtStatus tMtStatus;
// 				TMt tCurSpyMt = (TMt)tStatus.m_tStatus.tTvWall.atVideoMt[byChnId];
// 				if (tCurSpyMt.IsNull() || tCurSpyMt == m_tVmpEqp)//通道终端是否合法检查
// 				{
// 					continue;
// 				}
// 
// 				if (tCurSpyMt.IsLocal() 
// 					&& m_ptMtTable->GetMtType(tCurSpyMt.GetMtId()) == MT_TYPE_SMCU
// 					&& !(tCurSpyMt == tNewSrc)
// 					// [8/31/2011 liuxu] 新终端是该mcu下级直属终端才进行下面操作
// 					&& (IsMtInMcu(tCurSpyMt, tNewSrc, TRUE)
// 					    || (tNewSrc.IsNull() && IsMtInMcu(tCurSpyMt, tOldSrc, TRUE))))
// 				{
// 					u8 byTempMtId = tCurSpyMt.GetMtId();
// 					TLogicalChannel tLogicChannel;
// 					if ( m_tConfAllMtInfo.MtJoinedConf(byTempMtId )
// 						&& m_ptMtTable->GetMtLogicChnnl( byTempMtId, LOGCHL_VIDEO, &tLogicChannel, FALSE )
// 						&& MEDIA_TYPE_H264 == tLogicChannel.GetChannelType()	//目前的调整分辨率均是针对h264
// 						&& tLogicChannel.GetVideoFormat() != VIDEO_FORMAT_CIF)
// 					{
// 						//恢复原来终端分辨率
// 						if (!tOldSrc.IsNull())
// 						{
// 							CascadeAdjMtRes( tOldSrc, tLogicChannel.GetVideoFormat(), FALSE);
// 						}
// 						//调新加终端分辨率
// 						if (!tNewSrc.IsNull())
// 						{
// 							CascadeAdjMtRes( tNewSrc, VIDEO_FORMAT_CIF, TRUE);
// 						}
// 					}
// 				}
// 
// 				// [pengjie 2011/9/27] 刷下级mcu的上传源状态
// 				if( /*tNewSrc.IsNull() && */IsMtInMcu(tCurSpyMt, tOldSrc) && IsMtNotInOtherHduChnnl(tOldSrc, 0, 255) )
// 				{
// 					RefreshMtStatusInTw( tOldSrc, FALSE, FALSE );
// 				}
// 
// 				if( !tNewSrc.IsNull() && IsSMcuViewMtAndInTw( tNewSrc, FALSE )/*IsMtInMcu(tCurSpyMt, tNewSrc, TRUE)*/)
// 				{
// 					RefreshMtStatusInTw( tNewSrc, TRUE, FALSE );
// 				}
// 				
// 				if (!tCurSpyMt.IsLocal() 
// 					&& !IsLocalAndSMcuSupMultSpy(tCurSpyMt.GetMcuIdx()) 
// 					&& !(tCurSpyMt == tNewSrc))//通道终端为下级终端并且单回传时
// 				{
// 					GetMtStatus(tCurSpyMt, tMtStatus);
// 					tMtStatus.SetInTvWall(FALSE);
// 					SetMtStatus(tCurSpyMt, tMtStatus);
// 					MtStatusChange(&tCurSpyMt, TRUE);
// 					GetMtStatus(tNewSrc, tMtStatus);
// 					tMtStatus.SetInTvWall(TRUE);
// 					SetMtStatus(tNewSrc, tMtStatus);
// 					MtStatusChange(&tNewSrc, TRUE);
// 
// 					tStatus.m_tStatus.tTvWall.atVideoMt[byChnId].SetMt(tNewSrc);
// 					//tStatus.m_tStatus.tHdu.atVideoMt[byChnId].SetMt(tNewSrc);
// 					g_cMcuVcApp.SetPeriEqpStatus(byIdx, &tStatus);
// 					cServMsg.SetMsgBody((u8 *)&tStatus, sizeof(tStatus));
// 					SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
// 					u8 byLocalMtId = GetLocalMtFromOtherMcuMt(tCurSpyMt).GetMtId();
// 					TLogicalChannel tLogicChannel;
// 					if ( m_tConfAllMtInfo.MtJoinedConf(byLocalMtId )
// 						&& m_ptMtTable->GetMtLogicChnnl( byLocalMtId, LOGCHL_VIDEO, &tLogicChannel, FALSE )
// 						&& MEDIA_TYPE_H264 == tLogicChannel.GetChannelType()	//目前的调整分辨率均是针对h264
// 						&& tLogicChannel.GetVideoFormat() != VIDEO_FORMAT_CIF)
// 					{
// 						//恢复原来终端分辨率
// 						if (!tCurSpyMt.IsNull())
// 						{
// 							CascadeAdjMtRes( tCurSpyMt, tLogicChannel.GetVideoFormat(), FALSE);
// 						}
// 						//调新加终端分辨率
// 						if (!tNewSrc.IsNull())
// 						{
// 							CascadeAdjMtRes( tNewSrc, VIDEO_FORMAT_CIF, TRUE);
// 						}
// 					}
// 				}
// 
// 				// [8/31/2011 liuxu] 多回传上传通道上传终端变更处理
// 				if (!tCurSpyMt.IsLocal() 
// 					&& IsLocalAndSMcuSupMultSpy(tCurSpyMt.GetMcuIdx()) 
// 					&& IsMcu(tCurSpyMt)						// 是下级的一个mcu
// 					// 进入电视墙的mcu中上传通道成员改变
// 					&& (IsMtInMcu(tCurSpyMt, tNewSrc, TRUE)			
// 						|| ( tNewSrc.IsNull() && IsMtInMcu(tCurSpyMt, tOldSrc, TRUE))))
// 				{
// 					TLogicalChannel tLogicChannel;
// 					u8 byLocalMtId = GetLocalMtFromOtherMcuMt(tCurSpyMt).GetMtId();
// 
// 					if ( m_tConfAllMtInfo.MtJoinedConf(byLocalMtId )
// 						&& m_ptMtTable->GetMtLogicChnnl( byLocalMtId, LOGCHL_VIDEO, &tLogicChannel, FALSE )
// 						&& MEDIA_TYPE_H264 == tLogicChannel.GetChannelType()	//目前的调整分辨率均是针对h264
// 						&& tLogicChannel.GetVideoFormat() != VIDEO_FORMAT_CIF)
// 					{
// 						//恢复原来终端分辨率
// 						if (!tOldSrc.IsNull())
// 						{
// 							CascadeAdjMtRes( tOldSrc, tLogicChannel.GetVideoFormat(), FALSE);
// 						}
// 						//调新加终端分辨率
// 						if (!tNewSrc.IsNull())
// 						{
// 							CascadeAdjMtRes( tNewSrc, VIDEO_FORMAT_CIF, TRUE);
// 						}
// 					}
// 				}
// 
// 			}	
// 		}				
// 	}


// 	TMt tMt;
// 	if( m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE &&
// 		m_tConfAllMcuInfo.IsSMcuByMcuIdx(tNewSrc.GetMcuId())
// 		)
// 	{
// 		for (byChnId = 0; byChnId < MAXNUM_MPUSVMP_MEMBER; byChnId++)
// 		{
// 			ptVmpMember = m_tConf.m_tStatus.m_tVMPParam.GetVmpMember(byChnId);
// 			if( NULL == ptVmpMember || ptVmpMember->IsNull() )
// 			{
// 				continue;
// 			}
// 			tMt = (TMt)(*ptVmpMember);
// 			if( ptVmpMember->IsLocal() && 
// 				MT_TYPE_SMCU == m_ptMtTable->GetMtType( ptVmpMember->GetMtId() ) &&
// 				IsMtInMcu( tMt,tNewSrc)
// 				)
// 			{
// 				ChangeMtVideoFormat( tNewSrc, &m_tConf.m_tStatus.m_tVMPParam );	
// 				break;
// 			}
// 		}
		
// 		if( !(tNewSrc == tOldSrc) )
// 		{
// 			byChnId = m_tConf.m_tStatus.m_tVMPParam.GetChlOfMtInMember( tOldSrc );
// 			if( MAXNUM_MPUSVMP_MEMBER == byChnId )
// 			{				
// 				ChangeMtVideoFormat( tOldSrc, &m_tConf.m_tStatus.m_tVMPParam, FALSE );
// 			}
// 			else
// 			{
// 				ChangeMtVideoFormat( tOldSrc, &m_tConf.m_tStatus.m_tVMPParam );	
// 			}			
// 		}
// 	}


	TMt tMt;
	TMt tNullMt ;
	tNullMt.SetNull();
	u8 byCount = 0;
	u8 byChnId = 0;
	if( !m_tCascadeMMCU.IsNull() &&	
		IsMMcuSupportMultiCascade()
		)
	{
		// 三级级联单回传场景下，收到3级改变上传通道信息时，若二级会议上传通道成员是该3级mcu或属于该3级mcu
		// 则给上级上报此上传通道成员
		u16 wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
		//m_tConfAllMcuInfo.GetIdxByMcuId( m_tCascadeMMCU.GetMtId(),0,&wMMcuIdx );
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMMcuIdx);
		if(ptConfMcInfo != NULL && !IsLocalAndSMcuSupMultSpy(wMMcuIdx))
		{		
			//zyl 20121102 下级mcu报上来的setounotify，如果上级mcu的spymt和报上来的不是同一个mcu下的终端，不再往上报
			TMt tMMcuLocalSpyMt = GetLocalMtFromOtherMcuMt(ptConfMcInfo->m_tSpyMt);
			TMt tNewSrcLocalMt = GetLocalMtFromOtherMcuMt(tSetoutMt);
			//Bug00162355:三级级联单回传，进电视墙图标刷新异常
			//yrl20131113三级MCU取消上传通道终端时也上报给一级MCU(该3级mcu必须跟2级mcu的上传通道中的mcu一致)
			if ( !IsLocalAndSMcuSupMultSpy( tSetoutMt.GetMcuId() )
				&& ( tMMcuLocalSpyMt == tNewSrcLocalMt 
				     || ( tNewSrcLocalMt.IsNull() && ptConfMcInfo->m_tSpyMt.GetMcuId() == tSetoutMt.GetMcuId() ) ) 
				)
			{
				TSetOutParam tOutParam;
				tOutParam.m_nMtCount = 1;
				if( 0 == ptSetOutParam->m_tMt.GetMtId() )
				{
					tHeadMsg.m_tMsgSrc.m_abyMtIdentify[tHeadMsg.m_tMsgSrc.m_byCasLevel] = ptSetOutParam->m_tMt.GetMtId();
					++tHeadMsg.m_tMsgSrc.m_byCasLevel;
					tOutParam.m_atConfViewOutInfo[0].m_tMt.SetMtId( (u8)ptSetOutParam->m_tMt.GetMcuId() );
					tOutParam.m_atConfViewOutInfo[0].m_tMt.SetMcuId( LOCAL_MCUID );
				}
				else
				{
					tOutParam.m_atConfViewOutInfo[0].m_tMt = ptSetOutParam->m_tMt;			
				}				

				tOutParam.m_atConfViewOutInfo[0].m_nOutViewID = ptConfMcInfo->m_dwSpyViewId;
				tOutParam.m_atConfViewOutInfo[0].m_nOutVideoSchemeID = ptConfMcInfo->m_dwSpyVideoId;			
				//tHeadMsg.m_tMsgSrc.m_tMt = ptSetOutParam->m_tMt;;
				cServMsg.SetMsgBody((u8 *)&tOutParam, sizeof(tOutParam));
				cServMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
				
				cServMsg.SetEventId(MCU_MCU_SETOUT_NOTIF);
				SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_SETOUT_NOTIF, cServMsg );
				
				ptConfMcInfo->m_tSpyMt = tSetoutMt;
			}
		}
	}
	
 	tMt = m_tConf.GetSpeaker();
	if ( m_tConf.m_tStatus.IsSpecMixing() )
	{
		if (IsMcu(tMt))
		{
			TConfMcInfo *ptMcInfo = NULL;
			if (IsMtInMcu(tMt,tOldSrc) && !( tOldSrc == tNewSrc ))
			{
				ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tOldSrc.GetMcuId());
				if( NULL != ptMcInfo )
				{
					//20101224_tzy 如果该MCU下原来上传终端自动进的混音，则将该老终端移除混音器，如果是取消上传终端还要设置无终端自动停混音器标志
					if( ptMcInfo->GetMtAutoInMix(tOldSrc) )
					{
						BOOL32 bStopMixNoMem = ( 0 == tNewSrc.GetMtId() ) ? TRUE:FALSE;
						RemoveSpecMixMember( &tOldSrc,1,FALSE,bStopMixNoMem );
					}
				}
				else
				{
					BOOL32 bStopMixNoMem = ( 0 == tNewSrc.GetMtId() ) ? TRUE:FALSE;
					RemoveSpecMixMember( &tOldSrc,1,FALSE,bStopMixNoMem );
				}
			}
			if( 0 != tNewSrc.GetMtId() && IsMtInMcu(tMt,tNewSrc))
			{
				ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tNewSrc.GetMcuId());
				if( NULL != ptMcInfo)
				{
					if (!m_ptConfOtherMcTable->IsMtInMixing(tNewSrc))
					{
						AddSpecMixMember( &tNewSrc,1,TRUE);
					}
				}
				else
				{
					AddSpecMixMember( &tNewSrc,1,TRUE );
				}			
			}
		} 
	}

	TMt tSpeakerMcu = GetLocalMtFromOtherMcuMt(tMt);
	//zjj201009226 如果发言人所在mcu为单回传，且该mcu上传传源已经取消则取消发言人
	//如果是切换上传源那么发言人也要相应改变通知界面
	/*if( !IsLocalAndSMcuSupMultSpy( tMt.GetMcuId() ) &&
		!tMt.IsNull() && 
		tNewSrc.GetMcuId() == tMt.GetMcuId()
		)*/
	// 2011-12-13 mod by pgf：SMCU作发言人，其改变上传源，不应该改变当前的发言人
	if( !tMt.IsNull() && !tMt.IsLocal()		//对于拖下级mcu作发言人的情况，不应该进到下面的逻辑
		&& !IsLocalAndSMcuSupMultSpy( tMt.GetMcuId() ) 
		&&		 
		( tNewSrc.GetMcuId() == tMt.GetMcuId() || IsMtInMcu( tSpeakerMcu,tNewSrc ) )
		)
	{		
		if(0 == tNewSrc.GetMtId())
		{
			ChangeSpeaker( NULL );
		}
		else
		{
			m_tConf.SetSpeaker( tNewSrc );
			cServMsg.SetEventId( MCU_MCS_CONFINFO_NOTIF );
			cServMsg.SetMsgBody( ( u8 * )&m_tConf, sizeof( m_tConf ) );
			if(m_tConf.HasConfExInfo())
			{
				u8 abyConfInfExBuf[CONFINFO_EX_BUFFER_LENGTH] = {0};
				u16 wPackDataLen = 0;
				PackConfInfoEx(m_tConfEx,abyConfInfExBuf,wPackDataLen);
				cServMsg.CatMsgBody(abyConfInfExBuf, wPackDataLen);
			}
			SendMsgToAllMcs( MCU_MCS_CONFINFO_NOTIF, cServMsg );
			NotifyAllSMcuMediaInfo( 0,MODE_BOTH );
		}				
	}

	//zjj20100414 界面刷新真实媒体源
	if( //MT_TYPE_SMCU == m_ptMtTable->GetMtType( byMtId ) &&
		tNewSrc.IsNull() || IsMtInMcu( tMt, tNewSrc )
		)
	{
		//zhouyiliang 20101018 取消媒体源的时候，相应mcinfo要清空
		if ( tNewSrc.IsNull() ) 
		{
			TConfMcInfo * ptMcuMcInfo = m_ptConfOtherMcTable->GetMcInfo( tNewSrc.GetMcuId() );
			if (NULL != ptMcuMcInfo ) 
			{
				ptMcuMcInfo->m_tSrcMt = tNewSrc;
			}
		}

		NotifyAllSMcuMediaInfo( 0,MODE_BOTH );
	}	

	TMcu tMcu;
	tMcu.SetMcuId(wMcuIdx);
	cServMsg.SetEventId(MCU_MCS_MCUMEDIASRC_NOTIF);
	cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu));
	cServMsg.CatMsgBody((u8 *)&tNewSrc, sizeof(tNewSrc));
	SendMsgToAllMcs(MCU_MCS_MCUMEDIASRC_NOTIF, cServMsg);
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcuMcuSetOutNotify] Notify MMcu SMcu(%d)'s MediaSrc (%d.%d) IsNull(%d)\n",
								tMcu.GetMcuId(),tNewSrc.GetMcuId(),tNewSrc.GetMtId(), tNewSrc.IsNull());
	
	// [pengjie 2010/10/11] 对于单回传，多个上级终端选看某个mcu下的多个下级终端，由于只有一路回传
	//                      需要保证终端状态和实际看到的图像一致，所以将选看同一mcu下终端的选看源刷成上传源
	if(!IsLocalAndSMcuSupMultSpy(tSetoutMt.GetMcuId()))
	{
		tNullMt.SetNull();
		TMt tSelVidMt;
		TMt tSelAudMt;
		TMt tCurrMt;
		TMtStatus tMtStatus;
		
		//[2011/10/14/zhangli]源处于同一mcu且不等，则改终端选看源，如果过适配，改占用bas的源，使用下级切换上传源的情况
		TBasOutInfo tOutInfo;
		
		for(u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
		{
			if(m_tConfAllMtInfo.MtJoinedConf(byLoop))
			{
				tCurrMt = m_ptMtTable->GetMt(byLoop);
				m_ptMtTable->GetMtStatus(byLoop, &tMtStatus);
				tSelVidMt = tMtStatus.GetSelectMt(MODE_VIDEO);
				tSelAudMt = tMtStatus.GetSelectMt(MODE_AUDIO);
				
				if(!tSelVidMt.IsNull() && (tSelVidMt.GetMcuId() == tSetoutMt.GetMcuId())
					&& !(tSelVidMt == tSetoutMt))
				{
					if (IsNeedSelAdpt(tSelVidMt, tCurrMt, MODE_VIDEO))
					{
						if (FindBasChn2SelForMt(tSelVidMt, tCurrMt, MODE_VIDEO, tOutInfo))
						{
							SetBasChnSrc(tOutInfo.m_tBasEqp, tOutInfo.m_byChnId, tSetoutMt);
						}
					}
					tMtStatus.SetVideoMt(tSetoutMt);
					tMtStatus.SetSelectMt(tSetoutMt, MODE_VIDEO);
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "[ProcMcuMcuSetOutNotify]change select src of Mt(%d,%d)!\n",
						tCurrMt.GetMcuId(), byLoop);
				}
				
				if(!tSelAudMt.IsNull() && (tSelAudMt.GetMcuId() == tSetoutMt.GetMcuId())
					&& !(tSelAudMt == tSetoutMt))
				{
					if (IsNeedSelAdpt(tSetoutMt, tCurrMt, MODE_AUDIO))
					{
						if (FindBasChn2SelForMt(tSelVidMt, tCurrMt, MODE_AUDIO, tOutInfo))
						{
							SetBasChnSrc(tOutInfo.m_tBasEqp, tOutInfo.m_byChnId, tSetoutMt);
						}
					}
					tMtStatus.SetAudioMt(tSetoutMt);
					tMtStatus.SetSelectMt(tSetoutMt, MODE_AUDIO);
				}
				
				m_ptMtTable->SetMtStatus(byLoop, &tMtStatus);
				
				TMt tMtStatusChange = m_ptMtTable->GetMt(byLoop);
				MtStatusChange(&tMtStatusChange, TRUE);
			}
		}
	}
	// End

	//vmp 由于牵涉前适配处理,需等发言人与选看处理完后执行
	//zjj20091230 将同一个下级的mcu作清理工作，即某个下级mcu的终端只能有一个在画面合成成员中
	//且占据它所已经占据的最前一个通道
	// [3/28/2011 xliang] 上级指定回传源，下级指定上传源对终端分辨率的影响
	u8 byVmpCount = GetVmpCountInVmpList();
	if (byVmpCount > 0)
	{
		TEqp tVmpEqp;
		TVMPParam_25Mem tVmpParam;
		TVmpChnnlInfo tVmpChnnlInfo;
		TVMPMember *ptVmpMember = NULL;
		BOOL32 bChanged = FALSE;
		BOOL32 bVmpChged = FALSE; //该vmp是否有改变
		BOOL32 bNoneKeda = FALSE;
		BOOL32 bHDStyleOK;
		TVMPParam_25Mem tLastVmpParam;
		TVMPMember *ptLastVmpMember = NULL;
		u8 byTmpVmpId = 0;
		for (u8 byIdx=0; byIdx<MAXNUM_CONF_VMP; byIdx++)
		{
			if (!IsValidVmpId(m_abyVmpEqpId[byIdx]))
			{
				continue;
			}
			tVmpEqp = g_cMcuVcApp.GetEqp( m_abyVmpEqpId[byIdx] );
					
			tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
			tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
			ptVmpMember = NULL;
			//vmp处理,仅保留下级设置上传通道终端处理,上级导致的上传通道终端改变无需再调整vmp
			if (  byVmpCount > 0
				&& !tVmpParam.IsVMPAuto() 
				&& tNewSrc.GetType() != TYPE_MCUPERI && !tNewSrc.IsLocal() 
				)
			{
				bVmpChged = FALSE;
				bNoneKeda = FALSE;
				// 是否该风格支持所有终端不降分辨率全进
				bHDStyleOK = tVmpChnnlInfo.GetMaxStyleNum() >= tVmpParam.GetMaxMemberNum();
				if (!tNewSrc.IsNull())
				{
					bNoneKeda = !IsKedaMt(tNewSrc, FALSE);
				}
				for (byChnId = 0; byChnId < MAXNUM_VMP_MEMBER; byChnId++)
				{
					tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
					ptVmpMember = tVmpParam.GetVmpMember(byChnId);

					// [miaoqingsong 20111205] 增加判断条件：主席轮询/上传轮询到的终端不参与VMP视频轮询跟随
					// 这里为什么要增加这个判断条件?这样会导致主席轮询时,下级切上传通道终端,vmp成员都无法更新
					if( NULL == ptVmpMember || ptVmpMember->IsNull()/* || 
						(m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO_SPY || m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_SPEAKER_SPY) ||
						(m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_VIDEO_CHAIRMAN || m_tConf.m_tStatus.GetPollMode() == CONF_POLLMODE_BOTH_CHAIRMAN)*/
						)
					{
						continue;
					}

					// 跳过双流跟随通道
					if (VMP_MEMBERTYPE_DSTREAM == ptVmpMember->GetMemberType())
					{
						continue;
					}

					tMt = (TMt)(*ptVmpMember);

					// [3/28/2011 xliang] A.单回传替换回传源的逻辑（上级指定具体回传源）
					// 单回传,下级切换回传源,同样也有可能进A逻辑
					if( !IsLocalAndSMcuSupMultSpy( tNewSrc.GetMcuIdx() )
						/*&& ptVmpMember != NULL && !ptVmpMember->IsNull() */&& !ptVmpMember->IsLocal() &&
						/* GetLocalMtFromOtherMcuMt( *ptVmpMember ) ==  GetLocalMtFromOtherMcuMt( tNewSrc )*/
						!(tNewSrc == tOldSrc)
					  )
					{			
						// 上面if条件去除，此处添加判断新上传源为空的情况
						if ( !tOldSrc.IsNull() && tNewSrc.IsNull()
							&& !tOldSrc.IsLocal()
							&& *ptVmpMember == tOldSrc)
						{
							// 单回传，通道为下级终端，取消上传通道时，将该通道刷新为其所在mcu
							ptVmpMember->SetMt(GetLocalMtFromOtherMcuMt(tOldSrc));
							bChanged = TRUE;
							bVmpChged = TRUE;
							g_cMcuVcApp.SetConfVmpParam(tVmpEqp, tVmpParam);
						}
						else if (GetLocalMtFromOtherMcuMt( *ptVmpMember ) ==  GetLocalMtFromOtherMcuMt( tNewSrc ))
						{
							if( 0 == byCount )
							{
								ptVmpMember->SetMt( tNewSrc );	
								if (!IsAllowVmpMemRepeated(tVmpEqp.GetEqpId()) || (!bHDStyleOK && bNoneKeda)) 
								{
									byCount++;
								}				
								g_cMcuVcApp.SetConfVmpParam(tVmpEqp, tVmpParam);
							}
							else
							{	
								// 不能简单的置空，还需要将交换拆掉，通过ClearOneVmpMember接口实现
								//ptVmpMember->SetMt( tNullMt );
								ClearOneVmpMember(tVmpEqp.GetEqpId(), byChnId, tVmpParam);
							}
							bChanged = TRUE;
							bVmpChged = TRUE;
						}
					}
					// [3/28/2011 xliang] B.合成成员为下级MCU的逻辑（下级指定具体上传源）(单回传/多回传均适用)
					else if( /*ptVmpMember->IsLocal() && */
							IsMcu(tMt) &&
							((tNewSrc.IsNull() && IsMtInMcu(tMt,tOldSrc)) || IsMtInMcu( tMt,tNewSrc))
							)
					{
						// 若为非科达终端,且MCU在多个通道内,仅保留一路MCU
						if (bNoneKeda && byChnId != tVmpParam.GetChlOfMtInMember(tMt, TRUE))
						{
							ClearOneVmpMember(tVmpEqp.GetEqpId(), byChnId, tVmpParam);
						}
						else
						{
							bChanged = TRUE;
							bVmpChged = TRUE;	
							//ChangeMtVideoFormat( tMt, FALSE);//note: parameter shouldn't be tNewSrc here
						}
					}
					// 针对多回传mcu被其它终端选看,切换上传源时,需要调分辨率,上传通道终端进前适配
					else if (IsLocalAndSMcuSupMultSpy(tNewSrc.GetMcuIdx())
						&& (*ptVmpMember == tNewSrc || *ptVmpMember == tOldSrc)
						&& !(tNewSrc == tOldSrc)
						)
					{
						TMcu tSMcu = m_ptMtTable->GetMt(byMtId);
						//mcu被选看时,需映射选看终端,mcu做发言人时,同样需要映射
						if (IsSelectedbyOtherMt(tSMcu)
							|| (!tVmpParam.IsVMPBrdst() && tSMcu == m_tConf.GetSpeaker())
							)
						{
							bChanged = TRUE;
							bVmpChged = TRUE;
						}
					}
				}

				if (bVmpChged)
				{
					if (byTmpVmpId == 0)
					{
						byTmpVmpId = m_abyVmpEqpId[byIdx];
					}
					// 不调vmp外设,在更新m_tLastVmpParam前,需先让外设刷台标,保证台标显示正常
					RefreshVmpChlMemalias(tVmpEqp.GetEqpId());
					// 2010-10-21 add by pgf: 发言人更改且VMP无发言人跟随+下级单回传会议，VMP参数未设置
					// LastVmpParam不能简单的全覆盖，有可能在做别的处理，有可能正在调分辨率等等，LastVmpParam成员标识是否已建交换
					tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tVmpEqp);
					for (byChnId = 0; byChnId < MAXNUM_VMP_MEMBER; byChnId++)
					{
						ptVmpMember = tVmpParam.GetVmpMember(byChnId);
						ptLastVmpMember = tLastVmpParam.GetVmpMember(byChnId);
						if (ptVmpMember == NULL || ptLastVmpMember == NULL || ptLastVmpMember->IsNull())
						{
							continue;
						}
						if (*ptVmpMember == *ptLastVmpMember)
						{
							continue;
						}
						// 新上传源不为空
						if (!tNewSrc.IsNull())
						{
							// 单回传才需要更新
							if (!IsLocalAndSMcuSupMultSpy( tNewSrc.GetMcuIdx() ))
							{
								if (tNewSrc == *ptVmpMember &&
									GetLocalMtFromOtherMcuMt( *ptLastVmpMember ) ==  GetLocalMtFromOtherMcuMt( tNewSrc ))
								{
									// 更新成员
									*ptLastVmpMember = *ptVmpMember;
								}
							}
						}
						else
						{
							// 取消上传源，单回传才需要更新
							if (!IsLocalAndSMcuSupMultSpy( tOldSrc.GetMcuIdx() ))
							{
								if (tOldSrc == *ptLastVmpMember)
								{
									// 更新成员
									*ptLastVmpMember = *ptVmpMember;
								}
							}
						}
					}
					g_cMcuVcApp.SetLastVmpParam(tVmpEqp, tLastVmpParam);
					//memcpy(&m_tLastVmpParam, &m_tConf.m_tStatus.m_tVMPParam, sizeof(m_tLastVmpParam));
					LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "get smcu set out notify and vmp param changed, So keep is to m_tLastVmpParam\n");
					
					CServMsg cSendServMsg;
					cSendServMsg.SetEqpId(tVmpEqp.GetEqpId());
					cSendServMsg.SetMsgBody( (u8*)&tVmpParam, sizeof(tVmpParam) );
					SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cSendServMsg ); //告诉会控刷界面
				}
			}
		}

		if ( bChanged ) 
		{
			// change or recover res for OldSrc
			if( !tOldSrc.IsNull() && !(tNewSrc == tOldSrc) )
			{
				byChnId = tVmpParam.GetChlOfMtInMember( tOldSrc );
				if( MAXNUM_VMP_MEMBER == byChnId )
				{
					//A逻辑走,旧上传源不在合成器中,恢复其分辨率
					//ChangeMtVideoFormat( tOldSrc);
					ChangeMtResFpsInVmp(byTmpVmpId, tOldSrc, &tVmpParam, FALSE);
				}
				else
				{
					//B逻辑(多回传)才可能走
					ChangeMtVideoFormat( tOldSrc, FALSE);	
				}			
			}
			// change res for the NewSrc (A逻辑走此步) 
			//if ( !bSMcu )
			{
				ChangeMtVideoFormat( tNewSrc, FALSE );
			}
			
			/* 2011-10-18 add by pgf: 把外设状态+会议当前VMP参数发给MCS，刷新图标
			TPeriEqpStatus tStatus;
			g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tStatus);
			tStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
			//[2011/10/27/zhangli]保存eqp状态
// 			g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId(), &tStatus);
			CServMsg cSetOutMsg;
			cSetOutMsg.SetMsgBody((u8 *)&tStatus, sizeof(tStatus));
			SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cSetOutMsg);*/
		}

	}
	//更新监控联动窗口中终端信息
	//新上传源为空，若为单回传环境，则停旧上传源到监控的交换
	if (tNewSrc.IsNull())
	{
		// 解决问题：单回传环境，拖下级终端进监控，然后在下级会议中取消回传终端，上级未停止监控交换，导致无法关闭监控通道
		if (!IsLocalAndSMcuSupMultSpy(tOldSrc.GetMcuId()))
		{
			AdjustSwitchToAllMcWatchingSrcMt( tOldSrc, TRUE );
			AdjustSwitchToMonitorWatchingSrc( tOldSrc );	//监控联动
		}
	}
	else
	{
		UpdateMontiorOnSMcuViewMtChg( tNewSrc );
	}

	//Bug00161920:三级级联支持上传会议中，第三级MCU入电视墙画面合成通道，第三级终端进上传通道后不降分辨率
	//yrl20131114:场景：多回传拖3级mcu不带上传终端进1级mcu电视墙4画面(或TVWall或VMP)，然后拖终端进上传通道要求降分辨率
	if ( IsLocalAndSMcuSupMultSpy(tSetoutMt.GetMcuId()) )
	{
		//获取保存的上级下传的分辨率
		CRecvSpy tSpyInfo;
		TMt tMcuMt = GetLocalMtFromOtherMcuMt(tNewSrc);			
		if( m_cLocalSpyMana.GetSpyChannlInfo( tMcuMt, tSpyInfo ) )
		{
			u8 byMinRes = tSpyInfo.GetSimCapset().GetVideoResolution(); 
			
			// 获得终端在本级外设中的分辨率
			u8 byTmpMtRes = 0;
			GetMtFormat(tNewSrc, byTmpMtRes, FALSE);
			if ( VIDEO_FORMAT_AUTO == byTmpMtRes )
			{
				byTmpMtRes = GetAutoResByBitrate(VIDEO_FORMAT_AUTO, m_tConf.GetBitRate());
			}
			
			if(	byMinRes != 0 && IsSrcResThanDst(byTmpMtRes, byMinRes) )
			{
				// 发送调分辨率消息
				ChangeVFormat(tNewSrc, TRUE, byMinRes);
			}
		}
	}

	MtStatusChange(&tOldSrc,TRUE);
	MtStatusChange(&tNewSrc,TRUE);
    return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuStartMixerCmd
    功    能： 处理 上级MCU开启下级MCU会议讨论命令
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/5/27   3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::ProcMcuMcuStartMixerCmd( const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	u8 byMtNum = (cServMsg.GetMsgBodyLen() - sizeof(TMcu) - sizeof(u8)) / sizeof(TMt);
	if (byMtNum != 0)
	{
		CServMsg cTransMsg = cServMsg;
		cTransMsg.SetMsgBody((u8*)cServMsg.GetMsgBody(),sizeof(TMcu));
		u8 byDissMtNum = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMcu));
		cTransMsg.CatMsgBody((u8*)&byDissMtNum, sizeof(byDissMtNum));
		TMt tMt;
		for( u8 bylp = 0; bylp < byMtNum; bylp ++ )
		{
			TMt *ptMt = (TMt *)(cServMsg.GetMsgBody() + sizeof(TMcu) + sizeof(u8) + sizeof(TMt) * bylp);
			if (ptMt == NULL)
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_EQP, "[ProcMcuMcuStartMixerCmd]ptMt == NULL,So Return!\n");
				return;
			}
			tMt = *ptMt;
			tMt.SetMcuId(m_ptMtTable->GetMt(tMt.GetMtId()).GetMcuId());
			cTransMsg.CatMsgBody((u8*)&tMt, sizeof(TMt));
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ProcMcuMcuStartMixerCmd]tmt after trans is (%d,%d)\n", tMt.GetMcuId(), tMt.GetMtId());
		}
		u8 byReplace = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMcu) + sizeof(u8) + byMtNum * sizeof(TMt));
		cTransMsg.CatMsgBody((u8*)&byReplace,sizeof(u8));
		cServMsg = cTransMsg;
	}
	
	ProcMixStart(cServMsg);
	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuStartMixerNotif
    功    能： 处理 下级MCU开启会议讨论上报 通知
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMcuVcInst::ProcMcuMcuStartMixerNotif( const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	TMcu tMcu = *(TMcu*)cServMsg.GetMsgBody();

	STATECHECK;	
    
    // guzh [11/7/2007] 
	//cServMsg.SetSrcMtId(0);
	//cServMsg.SetEventId(MCU_MCS_STARTDISCUSS_NOTIF);
	//SendMsgToAllMcs(MCU_MCS_STARTDISCUSS_NOTIF,cServMsg);

	LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_MIXER,"[ProcMcuMcuStartMixerNotif]Smcu(%d) startmixing notify!\n",tMcu.GetMcuId());
	//混音开始成功，请求一次混音参数
	if (!tMcu.IsLocal())
	{
		OnGetMixParamToSMcu(&cServMsg);
	}
	u8 bySmcuId = (u8)(tMcu.GetMcuId());
	if (VCS_CONF == m_tConf.GetConfSource() && !m_ptMtTable->IsMtInMixing(bySmcuId))
	{
		TMt tMt = m_ptMtTable->GetMt(bySmcuId);
		AddSpecMixMember(&tMt,1);
	}
    
    return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuStopMixerCmd
    功    能： 处理 上级MCU停止下级MCU会议讨论命令
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/5/27   3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::ProcMcuMcuStopMixerCmd( const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	STATECHECK;

	//如果上级要下级停止混音，但是下级又没有起混音，还是需要向上级发送停混音notify的
	//以便上级剔除此mcu混音
// 	if (m_tConf.m_tStatus.IsNoMixing() && !m_tCascadeMMCU.IsNull())
// 	{
// 		TMt tMcu;
//         tMcu.SetMcu(LOCAL_MCUID);
//         cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(tMcu));
//         cServMsg.SetDstMtId( m_tCascadeMMCU.GetMtId() );
//         SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MCU_STOPMIXER_NOTIF, cServMsg);
// 	}
    ProcMixStop(cServMsg);
	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuStopMixerNotif
    功    能： 处理 下级MCU停止会议讨论上报 通知
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMcuVcInst::ProcMcuMcuStopMixerNotif( const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	
    
	TMcu *ptMcu = (TMcu *)(cServMsg.GetMsgBody());
	u8 byMtId = u8(ptMcu->GetMcuId());	
	u16 wMcuIdx = GetMcuIdxFromMcuId( (u8)ptMcu->GetMcuId() );
	ptMcu->SetMcuId( wMcuIdx );
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(ptMcu->GetMcuId());

	if(ptMcInfo == NULL)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_EQP, "[ProcMcuMcuStopMixerNotif]ptMcInfo == NULL,So Return!\n");
		return;
	}
	LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_MIXER,"[ProcMcuMcuStopMixerNotif]Smcu(%d) stopmixing notify!\n",byMtId);
	TMt tMt = m_ptMtTable->GetMt(byMtId);
	if ( (m_tConf.GetSpeaker() == tMt)  || (!IsLocalAndSMcuSupMultSpy(GetMcuIdxFromMcuId(byMtId)) && (GetLocalSpeaker() == tMt))
		)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_EQP, "[ProcMcuMcuStopMixerNotif]Speaker(%d,%d),So Don't Remove!!\n",
			m_tConf.GetSpeaker().GetMcuIdx(),m_tConf.GetSpeaker().GetMtId());
	}
	else
	{
		if( m_ptMtTable->IsMtInMixing(byMtId) )
		{
			RemoveSpecMixMember(&tMt,1);
		}
	}
	
    //20101209_tzy 与R3_FULL兼容性，R3_RULL做下级停掉混音器后并不向上级发送级联终端状态消息，导致界面刷新错误
	u8 byNeedClearMtMixing =  0;
	byNeedClearMtMixing = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMcu));
	if ( byNeedClearMtMixing == 0)
	{
		return;
	}

	for (u8 byMtid = 0; byMtid < MAXNUM_CONF_MT; byMtid++)
	{
		if (ptMcInfo->m_atMtStatus[byMtid].IsInMixing())
		{
			ptMcInfo->m_atMtStatus[byMtid].SetInMixing( FALSE );
			MtStatusChange((TMt*)&(ptMcInfo->m_atMtStatus[byMtid]));
		}
	}     

	return;
}

/*==============================================================================
函数名    : CascadeAdjMtRes
功能      : 发信令开始级联调分辨率请求
算法实现  :  
参数说明  : TMt		tMt					[in]要调的MT (形如[4,1])
			u16 wBitRate				[in]码率
			bIsRecover					[in]是否恢复
返回值说明: void
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-4-23	4.6				薛亮							创建
==============================================================================*/
void CMcuVcInst::CascadeAdjMtBitrate( TMt tMt, u16 wBitRate,BOOL32 bIsRecover /* = FALSE*/ )
{
	if( tMt.IsLocal() )
	{
		return;
	}
	TMsgHeadMsg tHeadMsg;
	TMt tMcuMt;
	CServMsg cServMsg;

	tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt, tMcuMt );
	cServMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
	cServMsg.CatMsgBody( (u8 *)&tMcuMt, sizeof(TMt) );		//要调整的Mt
	cServMsg.CatMsgBody( (u8 *)&bIsRecover, sizeof(bIsRecover) );		//要调整的Mt
	wBitRate =  htons(wBitRate);
	cServMsg.CatMsgBody( (u8 *)&wBitRate, sizeof(wBitRate) );		//要调整的Mt

	SendMsgToMt( (u8)tMcuMt.GetMcuId(), MCU_MCU_ADJMTBITRATE_CMD, cServMsg);

}

/*==============================================================================
函数名    : CascadeAdjMtRes
功能      : 发信令开始级联调分辨率请求
算法实现  :  
参数说明  : TMt		tMt					[in]要调的MT (形如[4,1])
			u8		byNewFormat			[in]要调至的分辨率
			BOOL	bStart				[in]为1表起调，为0表恢复到原始分辨率。默认为TRUE
			u8		byVmpStyle			[in]画面合成风格。默认为255，表无意义
			u8		byPos				[in]Mt在画面合成的通道位置。默认为255，表无意义

返回值说明: void
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-4-23	4.6				薛亮							创建
==============================================================================*/
void CMcuVcInst::CascadeAdjMtRes( TMt tMt, u8 byNewFormat, BOOL32 bStart, u8 byVmpStyle, u8 byPos, u32 dwResW, u32 dwResH,u8 byEqpId, u8 byHduChlId)
{
	// [pengjie 2010/10/29] 对于终端id为0直接返回，不会去调mcu
	if( tMt.GetMtId() == 0 )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[CascadeAdjMtRes] Dest mt.0 so return !\n" );
		return;
	}


	// End
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[CascadeAdjMtRes] Param--tMt:(%d,%d)\tbyFormat: %d\tbyPos: %d\tdwResWH[%d,%d]\tVmpId:%d\n", 
		tMt.GetMcuId(), tMt.GetMtId(), byNewFormat, byPos, dwResW, dwResH, byEqpId);
	// [10/19/2011 liuxu] 不能直接调本级mcu终端的分辨率,而转为调它下级上传终端的分辨率
	// 原因: 1. 当mcu终端和它的上传终端同时在上级电视墙中时, 该mcu退出电视墙时会
	// 企图恢复它上传通道中终端的分辨率
	// 转为调整其内真实上传终端的分辨率
	// [10/29/2011 liuxu] 添加时本地mcu判断, 为了满足R6-R2-R2级联时的情况
	// 原因: 第三级R2不会上报回传成员到第一级的R6,所以R6取不到第三季回传成员
	// R6-R6-R6级联时, 第一级调第三级终端转为第二级去调第三级的上传成员
	if (( IsMcu(tMt) && tMt.IsLocal() ))
	{
		TMt tTempMt = GetSMcuViewMt(tMt, FALSE);
		if (!tTempMt.IsNull())
		{
			tMt = tTempMt;
		}
	}

	// 电话终端因视频逻辑通道未开，不能调整分辨率，不发送调分辨率消息
	if (IsPhoneMt(tMt))
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[CascadeAdjMtRes] Dest mt.(%d,%d) is phone, so return !\n", tMt.GetMcuId(), tMt.GetMtId());
		return;
	}

	// 获得byNewFormat对应的分辨率宽和高
	u16 wResW = (u16)dwResW;
	u16 wResH = (u16)dwResH;
	// 若有传入宽高,则无需通过byNewFormat获得,上级可能会传下来,需要继续往第三级mcu发
	if (0 == dwResW || 0 == dwResH)
	{
		// mpeg4 auto需要根据会议码率获得分辨率
		if ( VIDEO_FORMAT_AUTO == byNewFormat )
		{
			byNewFormat = GetAutoResByBitrate(byNewFormat,m_tConf.GetBitRate());
		}
		GetWHByRes(byNewFormat, wResW, wResH);
		dwResW = wResW;
		dwResH = wResH;
	}
	// 转为网络序
	dwResW = htonl(dwResW);
	dwResH = htonl(dwResH);

	// 根据分辨率,获得对应通用分辨率,新旧MCU都能识别的分辨率
	if (bStart)
	{
		byNewFormat = GetNormalRes(wResW, wResH);
	}

	CServMsg cServMsg;
	cServMsg.SetEventId( MCU_MCU_ADJMTRES_REQ );
	u8 byStart = bStart ? 1: 0;
	TMsgHeadMsg tHeadMsg;
	TMt tMcuMt;

	tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt, tMcuMt );
	
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[CascadeAdjMtRes] SendMsg--tMt:(%d,%d)\tbyFormat: %d\tbyStart: %d\tbyPos: %d\tdwResWH[%d,%d]\n", 
		tMcuMt.GetMcuId(), tMcuMt.GetMtId(), byNewFormat, byStart, byPos, wResW, wResH);

	cServMsg.SetEqpId(byEqpId);
	cServMsg.SetChnIndex(byHduChlId);//更新hdu大通道
	cServMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
	cServMsg.CatMsgBody( (u8 *)&tMcuMt, sizeof(TMt) );		//要调整的Mt
	cServMsg.CatMsgBody( &byStart, sizeof(u8) );		//是否是恢复Res
	cServMsg.CatMsgBody( &byNewFormat, sizeof(u8) );	//要调至的新分辨率

	// 填以下2项，下级则可以自己查表获取要调到的RES
	//if( byVmpStyle <= VMP_STYLE_TWENTY && byVmpStyle > 0
	//	&& byPos < MAXNUM_MPUSVMP_MEMBER )
	{
		cServMsg.CatMsgBody( &byVmpStyle, sizeof(u8) );		//VMP 合成风格
		cServMsg.CatMsgBody( &byPos, sizeof(u8) );			//所处VMP通道位置
	}

	// v4r7及以后版本mcu专用
	cServMsg.CatMsgBody( (u8 *)&dwResW, sizeof(u32) );	//真正要调整的分辨率宽
	cServMsg.CatMsgBody( (u8 *)&dwResH, sizeof(u32) );	//真正要调整的分辨率高
	
	SendMsgToMt( (u8)tMcuMt.GetMcuId(), cServMsg.GetEventId(), cServMsg);
}

/*==============================================================================
函数名    : RecoverMtVFormat
功能      : 恢复终端的分辨率
算法实现  :  
参数说明  : TMt		tDstMt				[in]要恢复分辨率的的MT (形如[4,1])
			u8      bRecoverSMcuViewMt  [in]tDstMt是mcu时, 是否恢复它上传通道中终端的分辨率
返回值说明: 
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2011-10-19	4.6				liuxu							创建
==============================================================================*/
void CMcuVcInst::RecoverMtVFormat( const TMt& tDstMt, const BOOL32 bRecoverSMcuViewMt /*= FALSE */)
{
	if ( NeedChangeVFormat( tDstMt ) )
	{
		// tDstMt本身就不会被降分辨率, 所以不需要做恢复分辨率, 直接返回true
		return;
	}

	TMt tRealMt = tDstMt;
	if ( IsMcu(tDstMt) )
	{
		if ( !bRecoverSMcuViewMt )							// 不恢复其上传终端的分辨率
		{
			return;
		}

		// 获取上传终端失败
		tRealMt = GetSMcuViewMt(tDstMt, TRUE);
	}

	if (tRealMt.IsNull())
	{
		return;
	}

	// 获取tDstMt在本地的MtId
	u8 byLocalMtId = 0;
	if (!tDstMt.IsLocal())
	{
		byLocalMtId = GetLocalMtFromOtherMcuMt(tDstMt).GetMtId();
	}else
	{
		byLocalMtId = tDstMt.GetMtId();
	}
	
	// 获取后向视频通道
	TLogicalChannel tLogicChannel;
	if ( !m_ptMtTable 
		|| !m_ptMtTable->GetMtLogicChnnl( byLocalMtId, LOGCHL_VIDEO, &tLogicChannel, FALSE ))
	{
		return;
	}

	// 恢复成原始分辨率
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "TMt[%d,%d] res recovered\n", tRealMt.GetMcuId(), tRealMt.GetMtId() );
	ChangeVFormat( tRealMt, FALSE, tLogicChannel.GetVideoFormat());
}


/*=============================================================================
    函 数 名： DecreaseMtVFormatInTw
    功    能： 降低电视墙中终端的分辨率
    算法实现： 
    全局变量： 
    参    数： [in] tDstMt,    要调整的终端
    返 回 值： 
-----------------------------------------------------------------------------
    修改记录：
	日  期       版本          修改人          走读人          修改记录
	2011-10-19	4.6				liuxu							创建
=============================================================================*/
void CMcuVcInst::DecreaseMtVFormatInTw( const TMt& tDstMt )
{
	// 不在tvwall中或不是其通道成员的上传终端时, 不能恢复分辨率
	if ( !IsSMcuViewMtAndInTw(tDstMt, FALSE)					// 不是tvwall通道成员的直接上传终端
		&& IsMtNotInOtherTvWallChnnl(tDstMt, 0, 255))			// 不在tvwall设备中
	{
		return;
	}

	if ( !NeedChangeVFormat( tDstMt ) )
	{
		// tDstMt本身就不会被降分辨率, 所以不需要做恢复分辨率, 直接返回true
		return;
	}

	// 实际要调整的终端. tDstMt是Mcu类型时, 实际调整的是它上传通道中终端的分辨率
	TMt tRealMt = (IsMcu(tDstMt) && tDstMt.IsLocal() ) ? GetSMcuViewMt(tDstMt, FALSE) : tDstMt;
	if (tRealMt.IsNull())
	{
		return;
	}

	// 获取tDstMt在本地的MtId
	u8 byLocalMtId = 0;
	if (!tDstMt.IsLocal())
	{
		byLocalMtId = GetLocalMtFromOtherMcuMt(tDstMt).GetMtId();
	}else
	{
		byLocalMtId = tDstMt.GetMtId();
	}
	
	// 获取后向视频通道
	TLogicalChannel tLogicChannel;
	if ( !m_tConfAllMtInfo.MtJoinedConf(byLocalMtId )
		|| !m_ptMtTable 
		|| !m_ptMtTable->GetMtLogicChnnl( byLocalMtId, LOGCHL_VIDEO, &tLogicChannel, FALSE ))
	{
		return;
	}
	
	// 把分辨率降为Cif
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "TMt[%d,%d] res decreased\n", tRealMt.GetMcuId(), tRealMt.GetMtId() );
	ChangeVFormat( tRealMt, TRUE, VIDEO_FORMAT_CIF );
}


/*=============================================================================
    函 数 名： SendChgMtVidFormat
    功    能： 普通终端调分辨率
    算法实现： 
    全局变量： 
    参    数： u8 byMtId		要调整的终端
			   u8 byChnnlType	信道类型，主视频/双流(LOGCHL_VIDEO...)
			   u8 byFormat		调整的分辨率
			   BOOL32 bMmcuCmd	是上级要求调还是本级自己要求调，默认False：本级要求调
			   BOOL32 bStart	是否调整分辨率
			   u32 wResW		调整的分辨率宽
			   u32 wResH		调整的分辨率高(有宽高按宽高,无宽高按byFormat)
    返 回 值： BOOL32 调整是否成功（对于本级来说）
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
	20100913    4.6			pengjie				   create
=============================================================================*/
BOOL32 CMcuVcInst::SendChgMtVidFormat( u8 byMtId, u8 byChlType, u8 byFormat, BOOL32 bMmcuCmd, BOOL32 bStart, u32 dwResW, u32 dwResH)
{
	CServMsg cServMsg;
	CSendSpy cSendSpy;
	TMt tSrc = m_ptMtTable->GetMt( byMtId );

	// [8/31/2011 liuxu] 对本级mcu或下级终端分辨率的调整
	if ( !tSrc.IsLocal() || IsMcu(tSrc) )
	{
		CascadeAdjMtRes( tSrc, byFormat, bStart, 0XFF, 0XFF, (u16)dwResW, (u16)dwResH);
		return TRUE;
	}

	cServMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
	cServMsg.SetMsgBody( &byChlType, sizeof(u8) );
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[SendChgMtVidFormat] Param--byMtId:%d\t byFormat is: %d\t dwResWH[%d,%d]\n", 
		byMtId, byFormat, dwResW, dwResH);
	
	//对于分辨率为VIDEO_FORMAT_AUTO根据会议码率计算分辨率
	if ( VIDEO_FORMAT_AUTO == byFormat )
	{
		byFormat = GetAutoResByBitrate(byFormat, m_tConf.GetBitRate());
	}

	// 记录原始请求分辨率
	u8 byReqFormat = byFormat;
	// 临时变量
	u16 wTmpResW = 0;
	u16 wTmpResH = 0;
	BOOL32 bResChg = FALSE;
	BOOL32 bIsMMcuNotSupMultSpy = FALSE;//MMCU是否是单回传
	// 上级要求调
	if( bMmcuCmd == TRUE )
	{
		u16 wMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
		bIsMMcuNotSupMultSpy = !IsLocalAndSMcuSupMultSpy( wMcuIdx );
		// 获得终端在本级外设中的分辨率
		u8 byTmpMtRes = 0;
		GetMtFormat(tSrc, byTmpMtRes, FALSE);
		if ( VIDEO_FORMAT_AUTO == byTmpMtRes )
		{
			byTmpMtRes = GetAutoResByBitrate(VIDEO_FORMAT_AUTO, m_tConf.GetBitRate());
		}
		// 有传入宽高,上级发下来消息调分辨率时,用传入的宽高做比较
		if (0 != dwResW && 0 != dwResH)
		{
			GetWHByRes(byTmpMtRes, wTmpResW, wTmpResH);
			byFormat = GetSuitableResByWH((u16)dwResW, (u16)dwResH, wTmpResW, wTmpResH);
		}
		else //未传入宽高,用byFormat做比较
		{
			// 2011-9-7 add by pgf:比较不正确，应该采取比宽和高
			byFormat = GetSuitableRes(byTmpMtRes, byFormat);
		}
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[SendChgMtVidFormat] Line:%d byMtInVmpRes<%d> CMP result:<%d>\n",
			__LINE__, byTmpMtRes, byFormat);
		if ( byFormat != byTmpMtRes)
		{
			bResChg = TRUE;
		}
		else
		{
			u8 byTempRes = VIDEO_FORMAT_INVALID;
			TLogicalChannel tLogicChannel;
			if ( !m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_VIDEO, &tLogicChannel, FALSE ) )
			{
				return FALSE;
			}

			//对于分辨率为VIDEO_FORMAT_AUTO根据会议码率计算分辨率
			if ( VIDEO_FORMAT_AUTO == tLogicChannel.GetVideoFormat() )
			{
				byTempRes = GetAutoResByBitrate(VIDEO_FORMAT_AUTO, m_tConf.GetBitRate());
			}
			else
			{
				byTempRes = tLogicChannel.GetVideoFormat();
			}

			// 恢复到逻辑通道能力
			if (byFormat == byTempRes)
			{
				bResChg = TRUE;
			}
			
			// 引入当前回传分辨率判断，针对回传小分辨率，上级恢复原始分辨率时终端仍在本级vmp通道中，未按本级通道调整场景。
			u8 byCurSpyRes = VIDEO_FORMAT_INVALID;//记录当前回传分辨率
			if (m_cLocalSpyMana.GetSpyChannlInfo( tSrc, cSendSpy ))
			{
				byCurSpyRes = cSendSpy.GetSimCapset().GetVideoResolution();
				if ( VIDEO_FORMAT_AUTO == byCurSpyRes )
				{
					byCurSpyRes = GetAutoResByBitrate(VIDEO_FORMAT_AUTO, m_tConf.GetBitRate());
				}
				byTempRes = GetSuitableRes(byCurSpyRes, byFormat);
				// 如果回传通道的分辨率小，要调整的分辨率比本级需要的大，还是需要调整一次分辨率的
				if (byTempRes == byCurSpyRes)
				{
					bResChg = TRUE;
				}
			}
			else
			{
				// 单回传时，若上级要求调整的终端不是上传源终端，不在回传列表中，替换上传源时恢复旧上传源分辨率会出现此情况
				// 需调整一次分辨率
				if (bIsMMcuNotSupMultSpy)
				{
					bResChg = TRUE;
				}
			}
		}

		/*TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
		if( (tVmpParam.IsVMPBrdst() && tVmpParam.IsMtInMember(tSrc)) )
		{
			CVmpChgFormatResult cVmpChgResult;
			u8 byMtInVmpRes = 0;
			u8 byVmpSubType = GetVmpSubType(m_tVmpEqp.GetEqpId());
			
			if(byVmpSubType == VMP)
			{
				GetMtFormatInSdVmp(tSrc, &tVmpParam, byMtInVmpRes, TRUE);
			}
			else
			{
				GetMtFormatInMpu(tSrc, &tVmpParam, byMtInVmpRes, cVmpChgResult, TRUE, TRUE, FALSE);
			}
			
			// 有传入宽高,上级发下来消息调分辨率时,用传入的宽高做比较
			if (0 != dwResW && 0 != dwResH)
			{
				GetWHByRes(byMtInVmpRes, wTmpResW, wTmpResH);
				byFormat = GetSuitableResByWH((u16)dwResW, (u16)dwResH, wTmpResW, wTmpResH);
			}
			else //未传入宽高,用byFormat做比较
			{
				// 2011-9-7 add by pgf:比较不正确，应该采取比宽和高
				byFormat = GetSuitableRes(byMtInVmpRes, byFormat);
			}
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[SendChgMtVidFormat] Line:%d byMtInVmpRes<%d> CMP result:<%d>\n",
			__LINE__, byMtInVmpRes, byFormat);
			// if( byFormat < byMtInVmpRes )
			if ( byFormat != byMtInVmpRes )
			{
				bResChg = TRUE;
			}
		}
		else if( m_ptMtTable->IsMtInTvWall(byMtId) )
		{
			// 有传入宽高,上级发下来消息调分辨率时,用传入的宽高做比较
			if (0 != dwResW && 0 != dwResH)
			{
				GetWHByRes(VIDEO_FORMAT_CIF, wTmpResW, wTmpResH);
				byFormat = GetSuitableResByWH((u16)dwResW, (u16)dwResH, wTmpResW, wTmpResH);
			}
			else //未传入宽高,用byFormat做比较
			{
				// 2011-9-7 add by pgf:比较不正确，应该采取比宽和高
				byFormat = GetSuitableRes(VIDEO_FORMAT_CIF, byFormat);
			}
			if( byFormat != VIDEO_FORMAT_CIF )
			{
				bResChg = TRUE;
			}
		}// [2013/03/11 chenbing] 终端是否在HDU多画面中 
		else if( IsMtInHduVmp(tSrc) )
		{
			// 有传入宽高,上级发下来消息调分辨率时,用传入的宽高做比较
			if (0 != dwResW && 0 != dwResH)
			{
				GetWHByRes(VIDEO_FORMAT_HD720, wTmpResW, wTmpResH);
				byFormat = GetSuitableResByWH((u16)dwResW, (u16)dwResH, wTmpResW, wTmpResH);
			}
			else //未传入宽高,用byFormat做比较
			{
				byFormat = GetSuitableRes(VIDEO_FORMAT_HD720, byFormat);
			}
			if( byFormat != VIDEO_FORMAT_HD720 )
			{
				bResChg = TRUE;
			}
		}
		else
		{
			bResChg = TRUE;
		}*/
	}
	// 本级要求调
	else
	{
		// 不是回传成员直接调分辨率
		if( !m_cLocalSpyMana.GetSpyChannlInfo( tSrc, cSendSpy ) )
		{
			bResChg = TRUE;
		}
		else
		{
			u8 byCurRes = cSendSpy.GetSimCapset().GetVideoResolution();
			if( byCurRes != VIDEO_FORMAT_INVALID && byCurRes != 0 )
			{
				// 新的分辨率比老的要小才调整
				// 2011-9-7 add by pgf:比较不正确，应该采取比宽和高
				byFormat = GetSuitableRes(byCurRes, byFormat);
				// 2011-9-26 fix by pgf: 本级MT作为上级VMP成员，上下级分辨率均为1080p，当此
				// MT从16画面VMP中踢出时，恢复的分辨率和回传信息的分辨率一致，这种情况必须发消息(会带来一点性能的损失)
// 				if( byFormat != byCurRes )
				{
					bResChg = TRUE;
				}
			}
			// 之前分辨率为空，则直接调分辨率
			else 
			{
				bResChg = TRUE;
			}
		}
	}
	
	// 单回传会议，上传通道终端分辨率改变，将回传成员分辨率更新到回传列表中
	if (bIsMMcuNotSupMultSpy)
	{
		// 获得当前上传源，若一致，更新列表
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
		if (ptConfMcInfo != NULL)
		{
			if (tSrc.GetMcuId() == ptConfMcInfo->m_tSpyMt.GetMcuId() 
				&& tSrc.GetMtId() == ptConfMcInfo->m_tSpyMt.GetMtId())
			{
				if (bMmcuCmd)
				{
					TSimCapSet tSimCapSet = GetMtSimCapSetByMode(byMtId);
					cSendSpy.Clear();
					cSendSpy.SetSpyMt(tSrc);
					cSendSpy.SetSimCapset(tSimCapSet);
					cSendSpy.SetSpyMode(MODE_VIDEO);//只用于调分辨率
					// 添加单回传回传信息，将最新分辨率更新
					if( m_cLocalSpyMana.AddSpyChannlInfo( cSendSpy ) )
					{
						tSimCapSet.SetVideoResolution(byReqFormat);
						m_cLocalSpyMana.SaveSpySimCap(tSrc, tSimCapSet);
					}
				}
			}
			else
			{
				// 不一致，表示此终端不是当前上传终端，若在m_cLocalSpyMana中，需清除
				if (m_cLocalSpyMana.GetSpyChannlInfo(tSrc, cSendSpy))
				{
					m_cLocalSpyMana.FreeSpyChannlInfo(tSrc, MODE_VIDEO);
				}
			}
		}
	}

	// 需要调整
	if (bResChg)
	{
		wTmpResW = (u16)dwResW;
		wTmpResH = (u16)dwResH;
		if (0 == dwResW || 0 == dwResH)
		{
// 			// mpeg4 auto需要根据会议码率获得分辨率
// 			if ( VIDEO_FORMAT_AUTO == byFormat )
// 			{
// 				byFormat = GetAutoResByBitrate(byFormat,m_tConf.GetBitRate());
// 			}
			GetWHByRes(byFormat, wTmpResW, wTmpResH);
			dwResW = wTmpResW;
			dwResH = wTmpResH;
		}
		// 根据分辨率,获得对应通用分辨率,新旧MCU都能识别的分辨率
		if (bStart)
		{
			byFormat = GetNormalRes(wTmpResW, wTmpResH);
		}
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MT2, "[SendChgMtVidFormat] SendMsg--byMtId:%d\t byFormat is: %d\t dwResWH[%d,%d]\n", 
			byMtId, byFormat, dwResW, dwResH);
		//将宽高转为网络序
		dwResW = htonl(dwResW);
		dwResH = htonl(dwResH);
		cServMsg.CatMsgBody( &byFormat, sizeof(u8) );
		cServMsg.CatMsgBody( (u8 *)&dwResW, sizeof(u32) );	//真正要调整的分辨率宽
		cServMsg.CatMsgBody( (u8 *)&dwResH, sizeof(u32) );	//真正要调整的分辨率高
		SendMsgToMt( byMtId, cServMsg.GetEventId(), cServMsg );
		return TRUE;
	}

	return FALSE;
}

/*====================================================================
    函数名      ：SendChgMtFps
    功能        ：向mt发送调帧率的消息（涵盖级联调帧率）
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt &tMt					[i] 
				  u8 byChnnlType					[i]
				  u8 byFps							[i] 
				  BOOL32 bStart						[i] 是调整还是恢复
				  
    返回值说明  ：VOID 
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	11/01/17	4.6         薛亮		   创建
====================================================================*/
void CMcuVcInst::SendChgMtFps( const TMt &tMt, u8 byChnnlType, u8 byFps, BOOL32 bStart)
{
	if( !tMt.IsLocal() ||
		m_ptMtTable->GetMtType( tMt.GetMtId() ) == MT_TYPE_SMCU
		)
	{
		if( tMt.IsLocal() && m_ptMtTable->GetMtType( tMt.GetMtId() ) == MT_TYPE_SMCU )
		{
			TConfMcInfo *ptInfo = m_ptConfOtherMcTable->GetMcInfo( GetMcuIdxFromMcuId(tMt.GetMtId()) );
			if(ptInfo != NULL)
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[SendChgMtFps] tMt.IsLocal() CascadeAdjMtFps tMt(%d, %d) bIsStart <%d> Adjusted NewFps: <%d>!!!\n",
					tMt.GetMcuId(), tMt.GetMtId(), bStart, byFps);
				CascadeAdjMtFps(ptInfo->m_tMMcuViewMt, bStart, byChnnlType, byFps);
			}			
		}
		else
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[SendChgMtFps] CascadeAdjMtFps tMt(%d, %d) bIsStart <%d> Adjusted NewFps: <%d>!!!\n",
					tMt.GetMcuId(), tMt.GetMtId(), bStart, byFps);
			CascadeAdjMtFps(tMt, bStart, byChnnlType, byFps);
		}
	}
	else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[SendChgMtFps] tMt(%d, %d) bIsStart <%d> Adjusted NewFps: <%d>!!!\n",
					tMt.GetMcuId(), tMt.GetMtId(), bStart, byFps);
		SendChangeMtFps(tMt.GetMtId(), byChnnlType, byFps);
	}
}
/*=============================================================================
    函 数 名： SendChangeMtFps
    功    能： 普通终端调帧率(本级)
    算法实现： 
    全局变量： 
    参    数： u8 byMtId   要调整的终端
			   u8 byChnnlType  信道类型，主视频/双流(LOGCHL_VIDEO...)
			   u8 byFps  调整的帧率值
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
	20100423    4.6			pengjie				   create
=============================================================================*/
void CMcuVcInst::SendChangeMtFps( u8 byMtId, u8 byChnnlType, u8 byFps )
{
	CServMsg cServMsg;

	cServMsg.SetEventId( MCU_MT_VIDEOFPSCHANGE_CMD );
	cServMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
	cServMsg.CatMsgBody( &byFps, sizeof(u8) );

	SendMsgToMt( byMtId, cServMsg.GetEventId(), cServMsg );

	return;
}

/*=============================================================================
    函 数 名： CascadeAdjMtFps
    功    能： 级联调帧率
    算法实现： 
    全局变量： 
    参    数： TMt tMt   要调整的下级终端
	           u8 bStart 为TRUE表示要调，FLASE恢复终端本来的帧率
			   u8 byChnnlType  信道类型，主视频/双流(LOGCHL_VIDEO...)
			   u8 byFps  调整的帧率值
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
	20100423    4.6			pengjie				   create
=============================================================================*/
void CMcuVcInst::CascadeAdjMtFps( TMt tMt, BOOL32 bStart, u8 byChnnlType, u8 byFps )
{
	CServMsg cServMsg;
	cServMsg.SetEventId( MCU_MCU_ADJMTFPS_REQ );
	u8 byStart = bStart ? 1: 0;

	// [pengjie 2010/8/23] 三级级联调帧率支持
	TMsgHeadMsg tHeadMsg;
	TMt tMcuMt;
	tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt, tMcuMt );

	cServMsg.SetMsgBody( (u8 *)&tMcuMt, sizeof(TMt) );		// 要调整的Mt
	cServMsg.CatMsgBody( (u8 *)&byStart, sizeof(u8) );		// 是否是恢复Fps
	cServMsg.CatMsgBody( (u8 *)&byChnnlType, sizeof(u8) );	// 信道类型
	cServMsg.CatMsgBody( (u8 *)&byFps, sizeof(u8) );	// 信道类型
	cServMsg.CatMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[CascadeAdjMtFps] tMt(%d, %d) bIsStart <%d> Adjusted NewFps: <%d>!!!\n",
					tMt.GetMcuId(), tMt.GetMtId(), bStart, byFps);
	SendMsgToMt( (u8)tMcuMt.GetMcuId(), cServMsg.GetEventId(), cServMsg);
}

/*=============================================================================
    函 数 名： ProcMcuMcuAdjustMtFpsReq
    功    能： 处理 级联调Mt帧率 请求
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
	20100423    4.6			pengjie				   create
=============================================================================*/
void CMcuVcInst::ProcMcuMcuAdjustMtFpsReq ( const CMessage *pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	STATECHECK;	
	
	TMt tMt        = *(TMt *)( cServMsg.GetMsgBody() );
	u8 byStart     = *(u8 *)( cServMsg.GetMsgBody() + sizeof(TMt));		//开始调
	u8 byChnnlType = *(u8 *)( cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) );
	u8 byReqFps    = *(u8 *)( cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) + sizeof(u8) ); 
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody() + sizeof(TMt) + (sizeof(u8) * 3) );

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,  "[ProcMcuMcuAdjustMtFpsReq] Recv tMt(%d, %d) chnnltype(%d) byStart(%d) Fps(%d)!!!\n",
					tMt.GetMcuId(), tMt.GetMtId(), byChnnlType, byStart, byReqFps );

	// [pengjie 2010/8/23] 三级级联支持，非本级继续往下传
	if( !tMt.IsMcuIdLocal() )
	{	
		SendMsgToMt( tMt.GetMcuId(), MCU_MCU_ADJMTFPS_REQ, cServMsg );
		return;
	}
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuAdjustMtFpsReq] tMt is MCU!!!\n");

	//[20130516 chenbing]下级收到的tMt为本级Mcu时，需要继续往下传
	TMt tSrc = m_ptMtTable->GetMt( tMt.GetMtId() );
	if ( IsMcu(tSrc) && tSrc.IsLocal() )
	{
 		SendChgMtFps(tSrc, byChnnlType, byReqFps, byStart);
	 	return;
	}

	u16 wNackReason = CASCADE_ADJFPSNACK_REASON_UNKNOWN;
	u8 byMtId = tMt.GetMtId();
	BOOL32 bRspNack = FALSE;
	TLogicalChannel tLogicChannel;
	if ( !m_tConfAllMtInfo.MtJoinedConf( byMtId ) ||
		!m_ptMtTable->GetMtLogicChnnl( byMtId, byChnnlType, &tLogicChannel, FALSE ) )
	{
		wNackReason = CASCADE_ADJFPSNACK_REASON_DISCON;
		bRspNack = TRUE;
	}

	if( IsNeedChangeFpsMt(byMtId) )
	{
		if( byStart ) // 调整
		{
			SendChangeMtFps( byMtId, byChnnlType, byReqFps );
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuAdjustMtFpsReq] Change Fps for mt: %d, chnnl type: %d, Fps: %d !\n",
					byMtId, byChnnlType, byReqFps );
		}
		else  // 恢复
		{
			byReqFps = tLogicChannel.GetChanVidFPS();
			SendChangeMtFps( byMtId, byChnnlType, byReqFps );
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuAdjustMtFpsReq] Recover Fps for mt: %d, chnnl type: %d, Fps: %d !\n",
					byMtId, byChnnlType, byReqFps );
		}

		cServMsg.SetMsgBody();
		cServMsg.SetMsgBody( (u8 *)&tMt, sizeof(TMt) );
		SendReplyBack( cServMsg, MCU_MCU_ADJMTFPS_ACK );
	}
	else
	{
		wNackReason = CASCADE_ADJFPSNACK_REASON_CANT;
	}

	if( bRspNack == TRUE )
	{
		if(bRspNack)
		{
			cServMsg.SetMsgBody();
			cServMsg.SetErrorCode(wNackReason);
			cServMsg.SetMsgBody( (u8 *)&tMt, sizeof(TMt) );
			SendReplyBack( cServMsg, MCU_MCU_ADJMTFPS_NACK );
			return;
		}
	}

	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuAdjustMtFpsAck
    功    能： 处理 级联调Mt帧率 请求 接受
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
	20100423    4.6			pengjie				   create
=============================================================================*/
void CMcuVcInst::ProcMcuMcuAdjustMtFpsAck ( const CMessage *pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	

	TMt tMt = *(TMt *)cServMsg.GetMsgBody();

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "McuMcuAdjustMtFpsAck come McuId: %d, MtId: %d !\n",
		tMt.GetMcuId(), tMt.GetMtId() );

	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuAdjustMtFpsNack
    功    能： 处理 级联调Mt帧率 请求 拒绝
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
	20100423    4.6			pengjie				   create
=============================================================================*/
void CMcuVcInst::ProcMcuMcuAdjustMtFpsNack ( const CMessage *pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	

	TMt tMt = *(TMt *)cServMsg.GetMsgBody();

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "McuMcuAdjustMtFpsNack come McuId: %d, MtId: %d !\n",
		tMt.GetMcuId(), tMt.GetMtId());

	return;
}

/*=============================================================================
    函 数 名： CheckMtResAdjust
    功    能： 调整终端分辨率前检查，返回是否能调，并告知能调到的分辨率值
				(注意该函数并不发消息给终端)
    算法实现： 
    全局变量： 
    参    数：	TMt &tMt		[i]
				u8  byReqRes	[i]		
				u16 &wErrorCode	[o]
				u8	&byRealRes	[o]
				u8  byStart		[i]		default: TRUE
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
	4/11/2009   4.6			薛亮					create
=============================================================================*/
BOOL32 CMcuVcInst::CheckMtResAdjust( const TMt &tMt, u8 byReqRes, u16 &wErrorCode, u8 &byRealRes, u8 byStart /* = TRUE */, u32 dwResW, u32 dwResH)
{
	u8 byMtId = tMt.GetMtId();
	wErrorCode = CASCADE_ADJRESNACK_REASON_UNKNOWN;	
	BOOL32 bRspNack	= FALSE;				//是否回nack
	//	BOOL32 bNeedAdjustRes = TRUE;			//默认是要调分辨率
	u8 byChnnlType = LOGCHL_VIDEO;			//目前只改变第一路视频通道的分辨率
	
	BOOL32 bNoneKeda = !IsKedaMt(tMt, TRUE);
	BOOL32 bG400IPCMt = IsG400IPCMt(tMt);

	TLogicalChannel tLogicChannel;
	if ( !m_tConfAllMtInfo.MtJoinedConf( byMtId ) ||
		!m_ptMtTable->GetMtLogicChnnl( byMtId, byChnnlType, &tLogicChannel, FALSE ) )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[CheckMtResAdjust]Backward LC has problems. Nack!!\n");
		bRspNack = TRUE;
	}
	
	u8 byMtStandardFormat = tLogicChannel.GetVideoFormat(); // 原始分辨率

	// [11/26/2009 xliang] FIXME: 若要调的Res >= 原始分辨率，对于非科达的也回ack
	if( bNoneKeda || bG400IPCMt)
	{
		//yrl20131120判断：若要调的Res >= 原始分辨率，对于非科达的也回ack
		if (IsSrcResThanDst(byMtStandardFormat, byReqRes))
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CALL, "[CheckMtResAdjust]None Keda Mt. Nack!!\n");
			bRspNack = TRUE;
			wErrorCode = CASCADE_ADJRESNACK_REASON_NONEKEDA;
		}
	}
	
	if(bRspNack)
	{
		return FALSE;
	}
	
	u8 byNewFormat;
	u8 bySuitableRes = 0;
	// 未传入宽高时,按byReqRes处理,传入宽高时,按宽高处理
	if (0 == dwResW || 0 == dwResH)
	{
		bySuitableRes = GetSuitableRes(byReqRes, byMtStandardFormat);
	}
	else
	{
		u16 wTmpResW = 0;
		u16 wTmpResH = 0;
		GetWHByRes(byMtStandardFormat, wTmpResW, wTmpResH);
		bySuitableRes = GetSuitableResByWH((u16)dwResW, (u16)dwResH, wTmpResW, wTmpResH);
	}
	if( byStart == 0 )
	{
		byNewFormat = byMtStandardFormat;
	}
	else
	{
		byNewFormat = bySuitableRes;
	}
	
	byRealRes = byNewFormat;
	// 	CServMsg cMsg;
	// 	cMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
	// 	cMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
	// 	cMsg.CatMsgBody( &byNewFormat, sizeof(u8) );
	// 	SendMsgToMt( byMtId, cMsg.GetEventId(), cMsg );

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[CheckMtResAdjust] RealRes for Mt.%u is %s. ReqRes is: %s\n", 
		byMtId, GetResStr(byRealRes), GetResStr(byReqRes));
	
	return TRUE;
	
}

/*=============================================================================
    函 数 名： ProcMcuMcuMuteDumbReq
    功    能： 处理 级联调Mt静音哑音
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
	10/08/13   4.6			薛亮					create
=============================================================================*/
void CMcuVcInst::ProcMcuMcuMuteDumbReq(const CMessage *pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	
	
	TMt *ptMt = (TMt *)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );
	u8 byMuteOpenFlag = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) + sizeof(TMt) );
	u8 byMuteType = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) + sizeof(TMt) + sizeof(u8) );

	ProcSingleMtMuteDumbOpr(cServMsg, ptMt, byMuteOpenFlag, byMuteType );

	return;

}
/*=============================================================================
    函 数 名： ProcMcuMcuAdjustMtResReq
    功    能： 处理 级联调Mt分辨率 请求
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
	4/11/2009   4.6			薛亮					create
=============================================================================*/
void CMcuVcInst::ProcMcuMcuAdjustMtBitRateCmd( const CMessage *pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
	TMt tMt = *(TMt *)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );
	u8 bIsRecover = *(u8 *)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) + sizeof( TMt ) );
	u16 wBitRate = *(u16 *)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) + sizeof( TMt ) + sizeof(u8) );
	wBitRate = ntohs(wBitRate);
	TMt tOrgTmt = tMt; 

	TMt tUnLocalMt = GetMtFromMultiCascadeMtInfo(tHeadMsg.m_tMsgDst, tOrgTmt);

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtBitRateCmd]mt(%d.%d) bIsRecover.%d wBitRate.%d\n",
			tUnLocalMt.GetMcuId(),tUnLocalMt.GetMtId(),bIsRecover,wBitRate
			);
	
	if( !tUnLocalMt.IsLocal() )
	{
		SendMsgToMt((u8)tMt.GetMcuId(),MCU_MCU_ADJMTRES_REQ,cServMsg );
		return;
	}

	CSendSpy cSendSpy;
	if( !m_cLocalSpyMana.GetSpyChannlInfo( tUnLocalMt,cSendSpy ) )
	{
		return;
	}
	
	TSimCapSet tSim = cSendSpy.GetSimCapset();

	if( !bIsRecover && tSim.GetVideoMaxBitRate() == wBitRate )
	{
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtBitRateCmd] tSim bitrate.%d is equal bitrate.%d\n",tSim.GetVideoMaxBitRate(),wBitRate );
		return;
	}
	if( bIsRecover && tSim.GetVideoMaxBitRate() ==  m_ptMtTable->GetRcvBandWidth(tUnLocalMt.GetMtId()) )
	{
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtBitRateCmd] bIsRecover tSim bitrate.%d is equal bitrate.%d\n",tSim.GetVideoMaxBitRate(),m_ptMtTable->GetRcvBandWidth(tUnLocalMt.GetMtId()) );
		return;
	}


	ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtBitRateCmd] now bitrate.%d\n",tSim.GetVideoMaxBitRate() );

	
	

	if( IsNeedSpyAdpt(tUnLocalMt,tSim,MODE_VIDEO) )
	{		
		if( bIsRecover )
		{			
			tSim.SetVideoMaxBitRate( m_ptMtTable->GetRcvBandWidth(tUnLocalMt.GetMtId()) );
		}
		else
		{			
			tSim.SetVideoMaxBitRate( wBitRate );
		}
		BOOL32 bIsUp = FALSE;
		if( tSim.GetVideoMaxBitRate() > cSendSpy.GetSimCapset().GetVideoMaxBitRate()  )
		{
			bIsUp = TRUE;
		}
		
		if ( TRUE == bIsUp )
		{
			RefreshSpyBasParam( tUnLocalMt,tSim,MODE_VIDEO, TRUE,TRUE );
		}
		else
		{
			RefreshSpyBasParam( tUnLocalMt,tSim,MODE_VIDEO);
		}
		m_cLocalSpyMana.SaveSpySimCap(tUnLocalMt, tSim);
	}
	else
	{	
		
		TLogicalChannel tLogChn;
		if (m_ptMtTable->GetMtLogicChnnl(tUnLocalMt.GetMtId(), LOGCHL_VIDEO, &tLogChn, FALSE))
		{							
			if( bIsRecover )
			{
				tLogChn.SetFlowControl( m_ptMtTable->GetRcvBandWidth(tUnLocalMt.GetMtId()) );
				tSim.SetVideoMaxBitRate( m_ptMtTable->GetRcvBandWidth(tUnLocalMt.GetMtId()) );
			}
			else
			{
				tLogChn.SetFlowControl(wBitRate);	
				tSim.SetVideoMaxBitRate( wBitRate );
			}									
			cServMsg.SetMsgBody((u8*)&tLogChn, sizeof(tLogChn)); 

			m_cLocalSpyMana.SaveSpySimCap(tUnLocalMt, tSim);
			SendMsgToMt( tUnLocalMt.GetMtId(), MCU_MT_FLOWCONTROL_CMD, cServMsg );  
		}
	}
}
/*=============================================================================
    函 数 名： ProcMcuMcuAdjustMtResReq
    功    能： 处理 级联调Mt分辨率 请求
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
	4/11/2009   4.6			薛亮					create
=============================================================================*/
void CMcuVcInst::ProcMcuMcuAdjustMtResReq( const CMessage * pcMsg)
{	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	
	u8* pbyBuf = cServMsg.GetMsgBody();
	u16 wMsgLen = cServMsg.GetMsgBodyLen();
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)pbyBuf;//cServMsg.GetMsgBody();
	pbyBuf += sizeof(TMsgHeadMsg);
	wMsgLen -= sizeof(TMsgHeadMsg);
	TMt tMt = *(TMt *)pbyBuf;
	pbyBuf += sizeof(TMt);
	wMsgLen -= sizeof(TMt);
	TMt tOrgTmt = tMt; //[nizhijun 2010/12/24] 这边做下保存，为下面调整过BAS的接口使用

	TMsgHeadMsg tHeadMsgRsp;
	tHeadMsgRsp.m_tMsgDst = tHeadMsg.m_tMsgSrc;
	tHeadMsgRsp.m_tMsgSrc = tHeadMsg.m_tMsgDst;
	//20100708_tzy 当目的终端为NULL时，则表示调当前在上传通道中的终端
	if (tMt.IsNull())
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResReq]adjust m_tSpyMt res\n");
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
	
		if ( !ptConfMcInfo || ptConfMcInfo->m_tSpyMt.IsNull())
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResReq]CascadeMMCU.m_tSpyMt is NULL\n");
			return;
		}
		tMt = ptConfMcInfo->m_tSpyMt;
	}
	u8 byMtId = tMt.GetMtId();

	u8 byStart = *(u8 *)pbyBuf;		//开始调Res
	pbyBuf += sizeof(u8);
	wMsgLen -= sizeof(u8);
	u8 byReqRes = *(u8 *)pbyBuf;
	pbyBuf += sizeof(u8);
	wMsgLen -= sizeof(u8);
	
	u8 byPos = ~0;
	if( wMsgLen >= sizeof(u8) + sizeof(u8))
	{
		//跳过byVmpStyle
		pbyBuf += sizeof(u8);
		wMsgLen -= sizeof(u8);
		byPos = *(u8 *)pbyBuf;
		pbyBuf += sizeof(u8);
		wMsgLen -= sizeof(u8);
	}
	u32 dwResW = 0;
	u32 dwResH = 0;
	// 剩余长度包含分辨率宽和高
	if (wMsgLen >= sizeof(u32) + sizeof(u32))
	{
		dwResW = *(u32 *)pbyBuf;
		pbyBuf += sizeof(u32);
		wMsgLen -= sizeof(u32);
		dwResH = *(u32 *)pbyBuf;
		pbyBuf += sizeof(u32);
		wMsgLen -= sizeof(u32);
		//将宽高由网络序转为主机序
		dwResW = ntohl(dwResW);
		dwResH = ntohl(dwResH);
		byReqRes = GetMcuSupportedRes((u16)dwResW, (u16)dwResH);
	}
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResReq]Eqp(%d) Param--tMt:(%d,%d)\tbyStart: %d\tbyFormat: %d\tbyPos: %d\tdwResWH[%d,%d]\n", 
		cServMsg.GetEqpId(), tMt.GetMcuId(), tMt.GetMtId(), byStart, byReqRes, byPos, dwResW, dwResH);

	u16 wNackReason = CASCADE_ADJRESNACK_REASON_UNKNOWN;
	u8 byRealRes	= VIDEO_FORMAT_INVALID;

	// 若要调整的终端是电话终端，则直接回复Ack，下级支持电话终端进vmp,上级拖MCU进vmp时，会调上传通道终端分辨率
	if (IsPhoneMt(tMt))
	{
		cServMsg.SetMsgBody((u8*)&tHeadMsgRsp,sizeof(TMsgHeadMsg));
		cServMsg.CatMsgBody((u8 *)&tMt, sizeof(TMt));
		cServMsg.CatMsgBody(&byStart, sizeof(u8));
		cServMsg.CatMsgBody(&byPos, sizeof(u8));
		SendReplyBack(cServMsg, MCU_MCU_ADJMTRES_ACK);
		
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResReq] tMt(%d,%d) is phone,so return ack. byStart.%d, byPos.%d.\n", 
			tMt.GetMcuId(), byMtId, byStart, byPos);
		return;
	}

	/*[2011/12/14/zhangli]级联调分辨率逻辑修改，遗留问题：上级mcu通道H264-720P，MT通道H264-1080P，把MT拖入上传通道(过适配，bas720输出)，
	然后MT拖入vmp(回传不过适配)，把终端分辨率调为CIF，这里使上传过适配的码流降为CIF*/

	BOOL32 bIsNeedSpyAdp = FALSE;	//回传是否过适配
	BOOL32 bIsNeedSelAdp = FALSE;	//上传是否过适配
	BOOL32 bIsInSetOut	 = FALSE;	//是否在上传
	BOOL32 bIsInSpy		 = FALSE;	//是否在回传
	TMt tUnLocalMt = GetMtFromMultiCascadeMtInfo(tHeadMsg.m_tMsgDst, tOrgTmt);
	
	//有终端在上传
	TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
	if ( !IsSupportMultiSpy()
		|| ( NULL != ptConfMcInfo && ptConfMcInfo->m_tSpyMt == tUnLocalMt ) 
	    || ( NULL != ptConfMcInfo && IsMcu(tUnLocalMt) && IsSmcuViewMtInMcu(ptConfMcInfo->m_tSpyMt, tUnLocalMt) )
		|| ( NULL != ptConfMcInfo && IsMcu(ptConfMcInfo->m_tSpyMt) && IsSmcuViewMtInMcu(tUnLocalMt, ptConfMcInfo->m_tSpyMt) )
	   )
	{
		bIsInSetOut = TRUE;
		if (IsNeedSelAdpt(tUnLocalMt, m_tCascadeMMCU, MODE_VIDEO))
		{
			bIsNeedSelAdp = TRUE;
		}
	}

	if (IsSupportMultiSpy())
	{		
		CSendSpy *ptSndSpy = NULL;
		TSimCapSet tDstCap;
		for (u8 bySpyIdx = 0; bySpyIdx < MAXNUM_CONF_SPY; bySpyIdx++)
		{
			ptSndSpy = m_cLocalSpyMana.GetSendSpy(bySpyIdx);
			if (NULL == ptSndSpy)
			{
				continue;
			}
			if (!(ptSndSpy->GetSpyMt() == tUnLocalMt))
			{
				continue;
			}
			
			bIsInSpy = TRUE;
			
			tDstCap = ptSndSpy->GetSimCapset();
			
			if (IsNeedSpyAdpt(tUnLocalMt, tDstCap, MODE_VIDEO))
			{
				bIsNeedSpyAdp = TRUE;
			}
			break;
		}
		
		//byStart==1，并且没有在上传也没有在回传，直接ack
		//应用场景：多回传下拖下级一个终端进vmp(下级适配)，此时这个终端不在m_cLocalSpyMana里，误把终端的分辨率调了
		if (byStart == 1 && !bIsInSpy && !bIsInSetOut)
		{
			tDstCap = m_ptMtTable->GetDstSCS(m_tCascadeMMCU.GetMtId());
			//yrl20131120能过BAS的终端才回ACK
			if(IsNeedSpyAdpt(tUnLocalMt, tDstCap, MODE_VIDEO))
			{
				//tMt.SetMcuIdx(LOCAL_MCUID);//此处如果是3级终端,到2级被设成LOCAL再返回1级,会错误
				cServMsg.SetMsgBody((u8*)&tHeadMsgRsp,sizeof(TMsgHeadMsg));
				cServMsg.CatMsgBody((u8 *)&tMt, sizeof(TMt));
				cServMsg.CatMsgBody(&byStart, sizeof(u8));
				cServMsg.CatMsgBody(&byPos, sizeof(u8));
				SendReplyBack(cServMsg, MCU_MCU_ADJMTRES_ACK);
				
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResReq] not find in localmana,so return ack,byMtId: %d byNewFormat: %s, byStart: %u \n", 
					byMtId, GetResStr(byReqRes), byStart);	
				return;
			}
		}
	}
	
	//如果在上传且过适配，调整bas
	BOOL32 bIsNAck = FALSE;
	if (bIsInSetOut && bIsNeedSelAdp)
	{
		bIsNAck = !AdjustSpyStreamFromAdpForMMcu(tOrgTmt, byReqRes, tHeadMsg);
	}
	
	//如果在回传且过适配，调整bas
	if (bIsInSpy && bIsNeedSpyAdp)
	{
		bIsNAck = !AdjustSpyStreamFromAdp(tOrgTmt, byReqRes, tHeadMsg) || bIsNAck;
	}
	
	CServMsg cMsg = cServMsg;
	// 取得回传成员回传类型,若已是回传成员,不回nack.
	u8 bySpyMode = m_cLocalSpyMana.GetSpyMode(tUnLocalMt);
	//如果有一个没有调成功，回NACK
	if ( bIsNAck && !( MODE_BOTH == bySpyMode || MODE_VIDEO == bySpyMode) )
	{
		cMsg.SetEventId( MCU_MCU_ADJMTRES_NACK );
		cMsg.SetSrcMtId( cServMsg.GetSrcMtId() );
		cMsg.SetEqpId( cServMsg.GetEqpId() );
		cMsg.SetErrorCode(wNackReason);		
		cMsg.SetMsgBody( (u8*)&tHeadMsgRsp,sizeof(TMsgHeadMsg) );
		cMsg.CatMsgBody( cServMsg.GetMsgBody()+sizeof(TMsgHeadMsg),cServMsg.GetMsgBodyLen() - sizeof(TMsgHeadMsg) );
		SendReplyBack( cMsg, MCU_MCU_ADJMTRES_NACK );
		return;
	}

	//如果有一个调成功，回ACK
	if ((bIsInSetOut && bIsNeedSelAdp) || (bIsInSpy && bIsNeedSpyAdp))
	{
		cServMsg.SetMsgBody( (u8*)&tHeadMsgRsp,sizeof(TMsgHeadMsg) );
		cServMsg.CatMsgBody( (u8 *)&tOrgTmt, sizeof(TMt) );
		cServMsg.CatMsgBody( &byStart, sizeof(u8) );
		cServMsg.CatMsgBody( &byPos, sizeof(u8) );
		SendReplyBack( cServMsg, MCU_MCU_ADJMTRES_ACK );
		if (tMt.IsMcuIdLocal())
		{
			tMt.SetMcuIdx( LOCAL_MCUIDX );
		}
		byRealRes = byReqRes;
	}
	//如果byStart == 0（停止、恢复分辨率），或者是在上传且不过适配，调终端分辨率
	//如果byStart=1,在回传且不过适配，这里不响应，这种场景下会ProcMcuMcuSpyNotify里响应
	//if ( (byStart == 0 ) || (bIsInSetOut && !bIsNeedSelAdp) || (bIsInSpy && !bIsNeedSpyAdp))
	else
	{
		if (!tOrgTmt.IsMcuIdLocal())
		{
			SendMsgToMt((u8)tMt.GetMcuId(),MCU_MCU_ADJMTRES_REQ,cServMsg );
			return;
		}
		else
		{
			tMt.SetMcuIdx( LOCAL_MCUIDX );
		}
		
		if(!CheckMtResAdjust( tMt, byReqRes, wNackReason, byRealRes, byStart, dwResW, dwResH))
		{
			//回nack
			cMsg.SetEventId( MCU_MCU_ADJMTRES_NACK );
			cMsg.SetSrcMtId( cServMsg.GetSrcMtId() );
			cMsg.SetEqpId( cServMsg.GetEqpId() );
			cMsg.SetErrorCode(wNackReason);		
			cMsg.SetMsgBody( (u8*)&tHeadMsgRsp,sizeof(TMsgHeadMsg) );
			cMsg.CatMsgBody( cServMsg.GetMsgBody()+sizeof(TMsgHeadMsg),cServMsg.GetMsgBodyLen() - sizeof(TMsgHeadMsg) );
			SendReplyBack( cMsg, MCU_MCU_ADJMTRES_NACK );
			return;
		}
		
		// [pengjie 2010/9/13] 多回传调分辨率逻辑调整
		u8 byChnnlType = LOGCHL_VIDEO;			//目前只改变第一路视频通道的分辨率
		SendChgMtVidFormat( byMtId, byChnnlType, byRealRes, TRUE);
		
		tMt.SetMcuIdx( LOCAL_MCUID );
		cServMsg.SetMsgBody( (u8*)&tHeadMsgRsp,sizeof(TMsgHeadMsg) );
		cServMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt) );
		cServMsg.CatMsgBody( &byStart, sizeof(u8) );
		cServMsg.CatMsgBody( &byPos, sizeof(u8) );
		SendReplyBack( cServMsg, MCU_MCU_ADJMTRES_ACK );
		
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResReq] byMtId: %d byNewFormat: %s, byStart: %u \n", byMtId, GetResStr(byRealRes), byStart);	
	}

	// [3/16/2011 xliang] save tmt res to multispy related structure
	CSendSpy cSendSpy;
	if ( m_cLocalSpyMana.GetSpyChannlInfo( tUnLocalMt, cSendSpy ) )
	{
		TSimCapSet tDstCap = cSendSpy.GetSimCapset();
		tDstCap.SetVideoResolution(byRealRes);
		m_cLocalSpyMana.SaveSpySimCap(tUnLocalMt, tDstCap);
	}
	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuAdjustMtResAck
    功    能： 处理 级联调Mt分辨率 应答
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
	4/11/2009   4.6			薛亮					create
	2013/03/11   4.7.2      陈兵                  修改(HDU多画面支持)
=============================================================================*/
void CMcuVcInst::ProcMcuMcuAdjustMtResAck( const CMessage * pcMsg)
{	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	

	u8  byPos = ~0;
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
	if( tHeadMsg.m_tMsgDst.m_byCasLevel > 0 &&
		!m_tCascadeMMCU.IsNull() 
		)
	{
		SendMsgToMt( m_tCascadeMMCU.GetMtId(),MCU_MCU_ADJMTRES_ACK,cServMsg );
		return;
	}
	TMt tMt = *(TMt *)(cServMsg.GetMsgBody()+sizeof(TMsgHeadMsg));	//被调好的MT
	u8  byStart = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt)+sizeof(TMsgHeadMsg));
	if( cServMsg.GetMsgBodyLen() > (sizeof(TMt) + sizeof(u8) +sizeof(TMsgHeadMsg)) )
	{		
		//消息体 = TMsgHeadMsg(消息头)+TMt+byStart(降/恢复)+byPos(所处的风格位置)
		byPos = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) + sizeof(TMt) + sizeof(u8));		
	}
	if (byPos == 0xFF) //zhouyiliang 20101124如果下级没有回pos，或者回的pos是个0xff，不需要继续做相关的处理了
	{
		return;
	}

	tMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc,tMt );

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResAck] Adjust Mt(%u,%u) res success!---bStart:%u byPos(%d)\n",
		tMt.GetMcuId(), tMt.GetMtId(), byStart ,byPos
			);

	// [2013/03/11 chenbing] Hdu多画面调整分辨率 
	if(byStart)
	{
		// [2013/03/11 chenbing] 得到EqpId 
		u8  byEqpId    = cServMsg.GetEqpId();

		// [2013/03/11 chenbing] 是否为HDU 
		if ( IsValidHduEqp(g_cMcuVcApp.GetEqp(byEqpId)) )
		{
			u8  byChnId    = cServMsg.GetChnIndex();
			u8  bySubChnId = byPos;

			TPeriEqpStatus tHduStatus;
			g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tHduStatus);
			TMt tMtTemp = tHduStatus.m_tStatus.tHdu.GetChnMt(byChnId, bySubChnId);
			if ( !tMtTemp.IsNull() )
			{
				// 通知下级建回传交换SpyNotify,此处不能调用ChangeHduSwitch,所以需要在此下发SpyNotify
				// 调用ChangeHduSwitch会造成单回传下上级拖下级Mcu或Mt图标刷新错误
				if ( IsLocalAndSMcuSupMultSpy(tMt.GetMcuIdx()) )
				{
					SendMMcuSpyNotify( tMt, MCS_MCU_START_SWITCH_HDU_REQ, GetHduChnSupportCap(tMt, byEqpId));
				}

				StartSwitchToPeriEqp(tMt, 0, byEqpId, byChnId, MODE_VIDEO,
					SWITCH_MODE_BROADCAST, FALSE, TRUE, FALSE, TRUE, TRUE, bySubChnId, HDUCHN_MODE_FOUR);

				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,
					"[ProcMcuMcuAdjustMtResAck] Mt(%u,%u), Adjusted Res Success SetHduChnnl[%d] and SetHduSubChnnl[%d]!\n", 
					tMt.GetMcuId(), tMt.GetMtId(), byChnId, bySubChnId);
				return;
			}
		}
		else if (IsValidVmpId(byEqpId))
		{
			TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byEqpId );

			// 若在收到调分辨率Ack准备Setvmpchnl时,发现当前有新vmp处理消息,忽略此次Setvmpchnl,等待新消息处理
			// 防止Setvmpchnl时将m_tConf的VmpParam覆盖到Eqpstatus中.
			if(!m_tConfInStatus.IsVmpNotify())
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResAck] VmpNotify is false, can't set tMt(%d,%d)  in vmp channel:%d.\n",tMt.GetMcuId(), tMt.GetMtId(), byPos);
				return;
			}

			TPeriEqpStatus tPeriEqpStatus; 
			BOOL32 bSetChnnl = FALSE;
			u8 byVmpMemberType = VMP_MEMBERTYPE_VAC;
			TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
			tVmpParam.Print(LOG_LVL_KEYSTATUS);

			//1.调分辨率ACK传上来的的tmt和pos与上级vmpparam存的一致，建交换
			//2.调分辨率ACK传上来的tmt与上级vmpparam中相应pos的tmt同属单回传同一MCU下的终端。TMT以VMPParam中原来存的为准[9/19/2012 chendaiwei]
			if(byPos < MAXNUM_VMP_MEMBER && (tMt == (TMt)*tVmpParam.GetVmpMember(byPos) ||
				(GetLocalMtFromOtherMcuMt(tMt) == GetLocalMtFromOtherMcuMt((TMt)*tVmpParam.GetVmpMember(byPos))
				 && !IsLocalAndSMcuSupMultSpy(tMt.GetMcuIdx())
	 			 && IsAllowVmpMemRepeated(byEqpId)))
				)			//建立到VMP某通道的交换
			{
				//上述第二种情况[9/19/2012 chendaiwei]
				tMt = (TMt)*tVmpParam.GetVmpMember(byPos);

				TVMPParam_25Mem tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tVmpEqp);
				TVMPMember* ptVmpMember = tVmpParam.GetVmpMember(byPos);
				TVMPMember* ptLastVmpMember = tLastVmpParam.GetVmpMember(byPos);
				byVmpMemberType = ptVmpMember->GetMemberType();
				bSetChnnl = TRUE;

				if( bSetChnnl)
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "Mt(%u,%u), Pos.%u in McuMcuadjResAck, begin to SetVmpChnnl!\n", tMt.GetMcuId(), tMt.GetMtId(), byPos);
					
					TMt tMcu = (TMt)(*ptVmpMember);
					// yanghuaizhi20110802 如果画面合成中为SMCU，用SMCU做画面合成处理，防止真实终端建多回传交换。
					if (NULL != ptLastVmpMember &&  MT_TYPE_SMCU == ptLastVmpMember->GetMtType() && (TMt)(*ptVmpMember) == (TMt)(*ptLastVmpMember)
						&&  IsMtInMcu(tMcu,tMt)
						)
					{
						//如果conf的vmpparam里面该成员已经是mcu，且last也是mcu，该通道成员不需再设置，changemvpswitch里面已经建好了mcu
						//到vmp的交换
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResAck] mt.(%d,%d) is chnl:%d vmpmember(mcu)'s viewmt,no need setvmpchnl\n",
							tMt.GetMcuId(),tMt.GetMtId(),byPos);
					}
					else
					{
						SetVmpChnnl(byEqpId, tMt, byPos, byVmpMemberType);
					}

					// zjj20090911 对于VCS会议在组呼画面合成的模式下,混音成员与画面合成成员绑定
					if (VCS_CONF == m_tConf.GetConfSource() && 
						VCS_GROUPVMP_MODE == m_cVCSConfStatus.GetCurVCMode() &&
						m_tConf.m_tStatus.IsMixing())
					{
						u8 byMemNum = tVmpParam.GetMaxMemberNum();
						TMt tTempMt;
						TMt atMixMember[MAXNUM_CONF_MT];
						u8  byMixMemNum = 0;
						for (u8 byVMPMemIdx = 0; byVMPMemIdx < byMemNum; byVMPMemIdx++)
						{
							tTempMt = (TMt)(*tVmpParam.GetVmpMember(byVMPMemIdx));
							if (!tTempMt.IsNull())
							{
								atMixMember[byMixMemNum++] = tTempMt;				
							}

						}
						ChangeSpecMixMember(atMixMember, byMixMemNum);

					}
				}
				else
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResAck] not set in vmp chnnl for certain reason!!\n");
				}
			}
			else
			{
				//其他预留操作
			}
		}
		else
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,	"[ProcMcuMcuAdjustMtResAck] byEqpId(%d) is Not Valid HDUId or VMPid.\n", byEqpId);
		}
	}

}

/*=============================================================================
    函 数 名： ProcMcuMcuAdjustMtResNack
    功    能： 处理 级联调Mt分辨率 应答nack
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
	20/11/2009   4.6			薛亮					create
=============================================================================*/
void CMcuVcInst::ProcMcuMcuAdjustMtResNack( const CMessage * pcMsg)
{	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	

	u8  byPos = ~0;
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
	if( tHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		SendMsgToMt( m_tCascadeMMCU.GetMtId(),MCU_MCU_ADJMTRES_NACK,cServMsg );
		return;
	}
	u16 wErrorCode = cServMsg.GetErrorCode();
	TMt tMt = *(TMt *)(cServMsg.GetMsgBody() +sizeof(TMsgHeadMsg));	//被调好的MT
	u8  byStart = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt)+sizeof(TMsgHeadMsg));
	if( cServMsg.GetMsgBodyLen() > (sizeof(TMt) + sizeof(u8) + sizeof(u8) + sizeof(u8)+sizeof(TMsgHeadMsg)) )
	{		
		byPos = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) + sizeof(u8) + sizeof(u8)+sizeof(TMsgHeadMsg));
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResNack] has byPos(%d)\n",byPos );
	}
	else
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResNack] VideoFormat(%d) vmpStyle(%d) ChannelPos(%d) len(%d)\n",
			*(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8)),
			*(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) + sizeof(u8)),
			*(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) + sizeof(u8) + sizeof(u8)),
			cServMsg.GetMsgBodyLen()
			);
	}

	// 下级调分辨率Nack消息是新建的cMsg，老版mcu不支持返回EqpId，兼容考虑，Nack时不会做以下处理，尽量避免Nack后续处理
	tMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc,tMt );
	u8 byEqpId = cServMsg.GetEqpId();
	
	if (!byStart)
	{
		// do nothing
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResNAck]EqpId(%d) Adjust Mt(%u,%u) res failed for reason:%d!---bStart:%u --byPos:%d \n",
			byEqpId, tMt.GetMcuId(), tMt.GetMtId(), wErrorCode, byStart,byPos );
		return;
	}

	// [2013/03/11 chenbing] Hdu多画面调整分辨率 
	// [2013/03/11 chenbing] 是否为HDU 
	if ( IsValidHduEqp(g_cMcuVcApp.GetEqp(byEqpId)) )
	{
		u8  byChnId    = cServMsg.GetChnIndex();
		u8  bySubChnId = byPos;
		
		TPeriEqpStatus tHduStatus;
		g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tHduStatus);

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,
			"[ProcMcuMcuAdjustMtResNAck] Adjust Res Faild CurHduMode(%d) Mt(%d.%d) HduId(%d) ChnId(%d) SubChnId(%d)\n",
				tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(byChnId),
				tMt.GetMcuId(),tMt.GetMtId(), byEqpId, byChnId, bySubChnId );	

		//调整分辨率率失败，清除电视墙
		ChangeHduSwitch(NULL, byEqpId, byChnId, bySubChnId, tHduStatus.m_tStatus.tHdu.GetMemberType(byChnId, bySubChnId),
				TW_STATE_STOP, g_cMcuVcApp.GetChnnlMMode(byEqpId, byChnId), FALSE, FALSE);

		//不能降分辨率的终端不能建交换
		NotifyMcsAlarmInfo(0, ERR_MCU_RESOURCES_NOT_ENOUGH);

		return;
	}
	else if (IsValidVmpId(byEqpId))
	{
		// 画面合成相关处理
		TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byEqpId );
		
		TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
		
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResNAck]EqpId(%d) Adjust Mt(%u,%u) res failed for reason:%d!---bStart:%u --byPos:%d vmpMode:%d IsAuto:%d\n",
			byEqpId, tMt.GetMcuId(), tMt.GetMtId(), wErrorCode, byStart,byPos,
			tVmpParam.GetVMPMode(),
			tVmpParam.IsVMPAuto()
			);
		//建立到VMP某通道的交换处理
		if( byPos < MAXNUM_VMP_MEMBER
			&& tVmpParam.GetVMPMode() != CONF_VMPMODE_NONE
			&& IsChangeVmpParam()// 若当前有新vmp处理消息,忽略此次Setvmpchnl,等待新消息处理,防止Setvmpchnl时将m_tConf的VmpParam覆盖到Eqpstatus中.
			)			
		{
			BOOL32 bSetChnnl = TRUE;		//是否可以建交换
			u8 byVmpMemberType = VMP_MEMBERTYPE_VAC;

			//VMP校验
			
			if( !tVmpParam.IsVMPAuto() )						//自动画面合成不校验成员
			{
				TVMPMember tVmpMember = *tVmpParam.GetVmpMember(byPos);
				if ( !(tVmpMember == tMt) )//对于定制合成，保护一下，以防ack收到之前合成参数有变
				{
					bSetChnnl = FALSE;
				}
				else
				{
					byVmpMemberType = tVmpMember.GetMemberType();
				}
			}
			
			if( bSetChnnl )
			{
				//判nack reason
				switch ( wErrorCode)
				{
				case CASCADE_ADJRESNACK_REASON_NONEKEDA:
					{
						//依然能建交换的条件:
						//a, 4风格及其以下
						//b, 有前适配通道可占
						//FIXME: 前适配通道抢占
						// 现在下级的外厂商终端直接在上级判断，若存在此种场景，认为不可建交换，因无法占用前适配
						bSetChnnl = FALSE;//ChangeMtVideoFormat(tMt, TRUE, TRUE);
						break;
					}
				default:
					bSetChnnl = FALSE;//其它场景认为不能建交换，需拆旧交换
					break;
					
				}
			}

			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResNAck]  Mt(%d.%d) bSetChnnl(%d) byPos(%d)\n",
						tMt.GetMcuId(),tMt.GetMtId(),bSetChnnl,byPos );

			if( bSetChnnl)
			{
				SetVmpChnnl(tVmpEqp.GetEqpId(), tMt, byPos, byVmpMemberType);

				// zjj20090911 对于VCS会议在组呼画面合成的模式下,混音成员与画面合成成员绑定
				if (VCS_CONF == m_tConf.GetConfSource() && 
					VCS_GROUPVMP_MODE == m_cVCSConfStatus.GetCurVCMode() &&
					m_tConf.m_tStatus.IsMixing())
				{
					u8 byMemNum = tVmpParam.GetMaxMemberNum();
					TMt tTempMt;
					TMt atMixMember[MAXNUM_CONF_MT];
					u8  byMixMemNum = 0;
					for (u8 byVMPMemIdx = 0; byVMPMemIdx < byMemNum; byVMPMemIdx++)
					{
						tTempMt = (TMt)(*tVmpParam.GetVmpMember(byVMPMemIdx));
						if (!tTempMt.IsNull())
						{
							atMixMember[byMixMemNum++] = tTempMt;				
						}

					}
					ChangeSpecMixMember(atMixMember, byMixMemNum);

				}
			}
			else
			{
				// 将Last中成员更新到m_tConf的VmpParam中,使其保持一致<级联调分辨率失败，不能将LastVmpParam成员覆盖到ConfVmpParam中，需拆旧交换>
				//m_tConf.m_tStatus.m_tVMPParam.SetVmpMember(byPos, *m_tLastVmpParam.GetVmpMember(byPos));
				u8 byMode = MODE_VIDEO;
				TVMPParam_25Mem tLastVmpParam = g_cMcuVcApp.GetLastVmpParam(tVmpEqp);
				if (VMP_MEMBERTYPE_DSTREAM == tLastVmpParam.GetVmpMember(byPos)->GetMemberType())
				{
					byMode = MODE_SECVIDEO;
				}
				StopSwitchToPeriEqp(tVmpEqp.GetEqpId(), byPos, FALSE, byMode);
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuAdjustMtResNack] not set in vmp chnnl for certain reason!!\n");
			}
			
		}
		else
		{
			//其他预留操作
		}

	}
	else
	{
		//yrl20131120此处场景是先拖MCU进电视墙4画面，然后拖非科达终端进上传通道，取不到EqpId
		TMt tTempMcu = GetLocalMtFromOtherMcuMt(tMt);
		if (GetSMcuViewMt(tTempMcu, TRUE) == tMt || GetSMcuViewMt(tTempMcu, FALSE) == tMt)
		{
			//非科达终端不能降分辨率拆除下级Mcu到HDU2多画面的交换
 			StopViewToAllHduVmpSwitch(tTempMcu);
		}

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,	"[ProcMcuMcuAdjustMtResNAck] byEqpId(%d) is Not Valid HDUId or VMPId.\n",	byEqpId);
	}

}

/*=============================================================================
    函 数 名： ProcMcuMcuGetMixerParamReq
    功    能： 处理 上级MCU获取下级MCU混音参数 请求
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMcuVcInst::ProcMcuMcuGetMixerParamReq( const CMessage * pcMsg)
{	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	

	TMixParam tMixParam = m_tConf.m_tStatus.GetMixerParam();
    // guzh [11/7/2007] 
    /*
    if (m_tConf.m_tStatus.IsVACMode())
    {
        if (m_tConf.m_tStatus.IsDiscussMode())
        {
            tMixParam.SetMode(mcuVacWholeMix);
        }
        else
        {
            tMixParam.SetMode(mcuVacMix);
        }
    }
    else
    {
        if (FALSE == m_tConf.m_tStatus.IsDiscussMode())
        {
            tMixParam.SetMode(mcuNoMix);
        }
        else if (m_tConf.m_tStatus.IsMixSpecMt())
        {
            tMixParam.SetMode(mcuPartMix);
        }
        else
        {
            tMixParam.SetMode(mcuWholeMix);
        }
    }
    */

	tMixParam.ClearAllMembers();
	
	for (u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
	{
		if (m_tConfAllMtInfo.MtJoinedConf(byLoop) && m_ptMtTable->IsMtInMixing(byLoop))            
		{
			if (m_ptMtTable->GetMt(byLoop).GetMtId() != m_tCascadeMMCU.GetMtId())
			{
				tMixParam.AddMember(m_ptMtTable->GetMt(byLoop).GetMtId());		
			}
		}
	}
	
	m_tConf.m_tStatus.SetMixerParam(tMixParam);

	TMcu tMcu;
	tMcu.SetMcuId(LOCAL_MCUID);
	cServMsg.SetMsgBody( (u8 *)&tMcu, sizeof(tMcu) );
	cServMsg.CatMsgBody( (u8*)&tMixParam, sizeof(TMixParam) );
	SendReplyBack( cServMsg, MCU_MCU_MIXERPARAM_NOTIF );

	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuGetMixerParamAck
    功    能： 处理 下级MCU获取MCU混音参数 响应
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMcuVcInst::ProcMcuMcuGetMixerParamAck( const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	
    
	cServMsg.SetSrcMtId(0);
	//cServMsg.SetEventId(MCU_MCS_GETDISCUSSPARAM_ACK);

    /*
    //zbq[12/06/2007] 会控不再感知下级MCU的混音状态
	//请求下级混音参数的回馈以 MCU_MCS_MIXPARAM_NOTIF 加以通知
	cServMsg.SetEventId(MCU_MCS_MIXPARAM_NOTIF);
	SendReplyBack( cServMsg, cServMsg.GetEventId());*/

	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuGetMixerParamNack
    功    能： 处理 下级MCU获取MCU混音参数 响应
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMcuVcInst::ProcMcuMcuGetMixerParamNack( const CMessage * pcMsg)
{
	//CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	
/*    
	cServMsg.SetSrcMtId(0);
	cServMsg.SetEventId(MCU_MCS_GETDISCUSSPARAM_NACK);
	SendReplyBack( cServMsg, cServMsg.GetEventId());
*/
    return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuMixerParamNotif
    功    能： 处理 下级MCU混音参数 上报通知
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMcuVcInst::ProcMcuMcuMixerParamNotif( const CMessage * pcMsg)
{
	//CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	
    /*
    //zbq[12/06/2007] 会控不再感知下级MCU的混音状态
	cServMsg.SetSrcMtId(0);
	cServMsg.SetEventId(MCU_MCS_MIXPARAM_NOTIF);
	SendMsgToAllMcs(MCU_MCS_MIXPARAM_NOTIF, cServMsg);
	*/
	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuAddMixerMemberCmd
    功    能： 处理 上级MCU增加混音成员请求
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMcuVcInst::ProcMcuMcuAddMixerMemberCmd( const CMessage * pcMsg)
{
	if ( m_tCascadeMMCU.IsNull() )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMcuMcuAddMixerMemberCmdAddMixerMemberCmd] m_tCascadeMMCU.IsNull, impossible \n" );
		return;
    }
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	STATECHECK

	CServMsg cMsg(cServMsg);
	TMcu tmcu;
	cMsg.SetMsgBody((u8 *)&tmcu, sizeof(TMcu));
	TMt *ptTempMt = NULL;
	u8 byMixNum = (cServMsg.GetMsgBodyLen() - sizeof(u8))/sizeof(TMt);
	BOOL32 bIsReturn = FALSE;

	//组织所有混音成员
	for (u8 byIdx = 0; byIdx < byMixNum; byIdx++)
	{
		//本级混音终端
		ptTempMt = (TMt *)(cServMsg.GetMsgBody() + sizeof(TMt) * byIdx);
		if ( ptTempMt == NULL)
		{
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[ProcMcuMcuAddMixerMemberCmd] warning ptTempMt == NULL,so return.\n" );
			return;
		}

		//把第3级终端的MCU设置成本级混音终端
		if (!ptTempMt->IsLocal())
		{
			u16 wMcuIdx = GetMcuIdxFromMcuId(u8(ptTempMt->GetMcuId()));
			ptTempMt->SetMcuIdx(wMcuIdx);
		}

		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[ProcMcuMcuAddMixerMemberCmd] MT(%d,%d) Local(%d)\n", 
			       ptTempMt->GetMcuId(), ptTempMt->GetMtId(), ptTempMt->IsLocal() );

		cMsg.CatMsgBody((u8 *)ptTempMt, sizeof(TMt));
	}

	u8 byReplace = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMt) * byMixNum);
	cMsg.CatMsgBody( (u8 *)&byReplace, sizeof(u8) );
	cServMsg.SetMsgBody((u8*)cMsg.GetMsgBody(), cMsg.GetMsgBodyLen()); 
		
	ProcMixStart(cServMsg);
 
	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuRemoveMixerMemberCmd
    功    能： 处理 上级MCU移除混音成员请求
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
=============================================================================*/
void CMcuVcInst::ProcMcuMcuRemoveMixerMemberCmd( const CMessage * pcMsg)
{
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	
	u8 *pMsgBody = (u8 *)cServMsg.GetMsgBody();

	pMsgBody += sizeof(TMcu);
	TMt *ptMt  = (TMt*)pMsgBody;

	u8 byMtNum = 0;
	byMtNum = (cServMsg.GetMsgBodyLen() - sizeof(TMcu))/sizeof(TMt);
	pMsgBody += byMtNum * sizeof(TMt);

	//lukunpeng 2010/05/27 当没有混音成员时停混音，如果对于上级是老版本，下级默认为不停混音
//	BOOL32 bNotStopNoMix = TRUE;
	//lukunpeng 2010/05/27 是否强制删除混音成员，如果对于上级是老版本，下级默认为强制删除
	BOOL32 bCancelForceMix = FALSE;

	//lukunpeng 2010/05/27 判断兼容性问题
// 	if(cServMsg.GetMsgBodyLen() >= sizeof(TMcu) + byMtNum * sizeof(TMt) + 2 * sizeof(u8))
// 	{
// 		bNotStopNoMix	= (*(u8 *)pMsgBody == 1) ? TRUE : FALSE;
// 		pMsgBody += sizeof(u8);
// 		bForceRemove		= (*(u8 *)pMsgBody == 0) ? TRUE : FALSE;	
// 	}

	if(cServMsg.GetMsgBodyLen() >= sizeof(TMcu) + byMtNum * sizeof(TMt) + sizeof(u8))
	{
		bCancelForceMix		= (*(u8 *)pMsgBody == 0) ? FALSE : TRUE;	
	}
	
	STATECHECK
	if (!ptMt->IsLocal())
	{
		TMt *pTmpMt = ptMt;

		TMcu tMcu = *((TMcu*)cServMsg.GetMsgBody());
		tMcu.SetMcuId(GetMcuIdxFromMcuId(u8(pTmpMt->GetMcuId())));
		memcpy((u8*)(cServMsg.GetMsgBody()),(u8*)&tMcu,sizeof(TMcu));

		for( u8 byIdx = 0;byIdx < byMtNum; ++byIdx )
		{			
			if (!pTmpMt->IsLocal())
			{
				pTmpMt->SetMcuIdx(GetMcuIdxFromMcuId(u8(pTmpMt->GetMcuId())));				
			}
			
			// [miaoqingsong 20111209] 添加保护：三级级联混音上级删除下级混音成员时，要删除的第三级终端不在混音时跳过，以防止误向第三级发删除成员消息
			//            导致第二级级记录的第三级成员数目不正确。(Bug00071831)
			if (!m_ptConfOtherMcTable->IsMtInMixing(*pTmpMt))
			{
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[ProcMcuMcuRemoveMixerMemberCmd] The Mt(%d,%d) is not in Mixer, Can't remove, so return!\n", 
					pTmpMt->GetMcuId(), pTmpMt->GetMtId() );
				
				return;
			}

			++pTmpMt;
		}

		OnAddRemoveMixToSMcu(&cServMsg,FALSE,TRUE);
	}
	else
	{
		if (m_ptMtTable->GetMtType(ptMt->GetMtId()) == MT_TYPE_SMCU)
		{
			TConfMcInfo *ptMcInfo = NULL;
			ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(ptMt->GetMtId()));
			if( NULL != ptMcInfo )
			{
				//20101224_tzy 如果该MCU下原来上传终端自动进的混音，则将该老终端移除混音器，如果是取消上传终端还要设置无终端自动停混音器标志
				if(ptMcInfo != NULL && m_ptConfOtherMcTable->IsMtInMixing(ptMcInfo->m_tMMcuViewMt) && ptMcInfo->GetMtAutoInMix(ptMcInfo->m_tMMcuViewMt))
				{
					RemoveSpecMixMember( &ptMcInfo->m_tMMcuViewMt,1,FALSE);
				}

				// miaoqingsong [20110921] 版本兼容：R2支持将MCU拖入混音通道开启定制混音
				// Bug00065017：R2-R6-R2三级级联会议，关闭第一级混音通道中的第三级MCU应支持删除下级MCU
				RemoveSpecMixMember( ptMt,1,FALSE);
			}
		} 
		else
		{
			RemoveSpecMixMember( ptMt, byMtNum, bCancelForceMix);
		}
	}

	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuLockReq
    功    能： 处理上级mcu对本级MCU的锁定请求
    算法实现： 
    全局变量： 
    参    数： const CMessage *pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/11   3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::ProcMcuMcuLockReq( const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	STATECHECK;	

	
	u8 byLock = *(cServMsg.GetMsgBody()+sizeof(TMcu));
    u8 byMcuId = cServMsg.GetSrcMtId();
    if ( byMcuId != m_tCascadeMMCU.GetMtId() )
    {
	    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        return;
    }

	if (ISTRUE(byLock))
	{
		m_tConf.m_tStatus.SetProtectMode(CONF_LOCKMODE_LOCK);
        // guzh [7/5/2007] 同时会取消mcs的锁定
        m_tConfProtectInfo.SetLockByMcu(byMcuId);
	}
	else
	{
		if (m_tConfProtectInfo.GetLockedMcuId() == byMcuId)
		{
            // guzh [7/5/2007] 上级解锁会同时解锁mcs的锁定
			m_tConf.m_tStatus.SetProtectMode(CONF_LOCKMODE_NONE);
            m_tConfProtectInfo.SetLockByMcu(0);
            m_tConfProtectInfo.SetLockByMcs(0);
		}
	}

	if(  !m_tCascadeMMCU.IsNull() && byMcuId == m_tCascadeMMCU.GetMtId() )
	{
		TConfMcInfo *ptMcInfo = NULL;
		u16 wMMcuIdx = INVALID_MCUIDX;
		
		if( !m_tCascadeMMCU.IsNull() )
		{
			wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );			
		}
		for( u16 wLoop = 0; wLoop < TConfOtherMcTable::GetMaxMcuNum(); wLoop++ )
		{
			ptMcInfo = (m_ptConfOtherMcTable->GetMcInfo(wLoop));
			if( ( NULL == ptMcInfo || !IsValidSubMcuId(ptMcInfo->GetMcuIdx())) 
				|| ( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->GetMcuIdx() == wMMcuIdx ) )
			{
				continue;
			}

			if( m_tConfAllMcuInfo.IsSMcuByMcuIdx(ptMcInfo->GetMcuIdx()) )
			{
				SendMsgToMt( GetFstMcuIdFromMcuIdx(ptMcInfo->GetMcuIdx()), MCU_MCU_LOCK_REQ, cServMsg);		
			}
		}
	}
	
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

	//通知会控
    cServMsg.SetMsgBody((u8*)&byLock, sizeof(byLock));
	SendMsgToAllMcs( MCU_MCS_CONFLOCKMODE_NOTIF, cServMsg );

	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuLockAck
    功    能： 处理下级mcu对锁定请求的应答
    算法实现： 
    全局变量： 
    参    数： const CMessage *pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/11   3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::ProcMcuMcuLockAck( const CMessage * pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	STATECHECK;	

	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(cServMsg.GetSrcMtId()));
	if (NULL == ptMcInfo)
	{
		cServMsg.SetSrcMtId(0);
		cServMsg.SetEventId(MCU_MCS_LOCKSMCU_NACK);
		SendReplyBack( cServMsg, cServMsg.GetEventId());
	}
	BOOL32 byLock = (*(cServMsg.GetMsgBody()+sizeof(TMcu)) != 0) ? TRUE : FALSE;

	if(ptMcInfo)
		ptMcInfo->SetIsLocked(byLock);

	cServMsg.SetSrcMtId(0);
	/*
	cServMsg.SetEventId(MCU_MCS_LOCKSMCU_ACK);
	SendReplyBack( cServMsg, cServMsg.GetEventId());
	*/	
	cServMsg.SetEventId(MCU_MCS_MCULOCKSTATUS_NOTIF);
	SendMsgToAllMcs(MCU_MCS_MCULOCKSTATUS_NOTIF, cServMsg);

	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuLockNack
    功    能： 处理下级mcu对锁定请求的应答
    算法实现： 
    全局变量： 
    参    数： const CMessage *pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/11   3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::ProcMcuMcuLockNack( const CMessage * pcMsg)
{
	//CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	
/*  
	cServMsg.SetSrcMtId(0);
	cServMsg.SetEventId(MCU_MCS_LOCKSMCU_NACK);
	SendReplyBack( cServMsg, cServMsg.GetEventId());
*/

	return;
}

/*=============================================================================
    函 数 名： ProcMcsMcuGetMcuLockStatusReq
    功    能： 处理会控对本MCU上的下级MCU锁定状态的查询请求
    算法实现： 
    全局变量： 
    参    数： const CMessage *pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/11   3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::ProcMcsMcuGetMcuLockStatusReq(const CMessage *pcMsg)
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	STATECHECK;

	if (NULL == m_ptConfOtherMcTable)
	{
		cServMsg.SetErrorCode(ERR_LOCKSMCU_NOEXIT);
		SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
		return;
	}

	TMcu tMcu;
	u8   byMcuNum = 0;
	TMcu *ptMcu = (TMcu *)(cServMsg.GetMsgBody());
	TConfMcInfo *ptMcInfo = NULL;
	if (INVALID_MCUIDX != ptMcu->GetMcuIdx())
	{
		ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(ptMcu->GetMcuId());
		if (NULL == ptMcInfo)
		{
			cServMsg.SetErrorCode(ERR_LOCKSMCU_NOEXIT );
			SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
			return;
		}
		cServMsg.CatMsgBody((u8 *)&(ptMcInfo->m_byIsLocked), sizeof(u8));
	}
	else
	{
		u16 wMMcuIdx = INVALID_MCUIDX;
		if( !m_tCascadeMMCU.IsNull() )
		{
			wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
			//m_tConfAllMcuInfo.GetIdxByMcuId( m_tCascadeMMCU.GetMtId(),0,&wMMcuIdx );
		}
		for (u16 wLoop = 0; wLoop < TConfOtherMcTable::GetMaxMcuNum(); wLoop++)
		{
			ptMcInfo = (m_ptConfOtherMcTable->GetMcInfo(wLoop));
			if ( NULL == ptMcInfo 
				|| !IsValidSubMcuId(ptMcInfo->GetMcuIdx()) 
				|| (!m_tCascadeMMCU.IsNull() && ptMcInfo->GetMcuIdx() == wMMcuIdx))
			{
				continue;
			}

			tMcu.SetNull();
			tMcu.SetMcu(0);
			tMcu.SetMcuIdx( ptMcInfo->GetMcuIdx() );
			if (0 == byMcuNum)
			{
				cServMsg.SetMsgBody((u8 *)&tMcu, sizeof(TMcu));
			}
			else
			{
				cServMsg.CatMsgBody((u8 *)&tMcu, sizeof(TMcu));
			}
			cServMsg.CatMsgBody((u8 *)&(ptMcInfo->m_byIsLocked), sizeof(u8));

			byMcuNum++;
		}
		if (0 == byMcuNum)
		{
			cServMsg.SetErrorCode(ERR_LOCKSMCU_NOEXIT);
			SendReplyBack(cServMsg, cServMsg.GetEventId()+2);
			return;
		}
	}

	SendReplyBack(cServMsg, cServMsg.GetEventId()+1);

	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuMtStatusCmd
    功    能： 处理上级mcu对本级直连终端的状态查询命令
    算法实现： 
    全局变量： 
    参    数： const CMessage *pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/11   3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::ProcMcuMcuMtStatusCmd( const CMessage *pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	STATECHECK;	

	if (m_tCascadeMMCU.IsNull() || 
		m_tCascadeMMCU.GetMtId() != cServMsg.GetSrcMtId() )
	{
		return;
	}

	OnNtfMtStatusToMMcu( m_tCascadeMMCU.GetMtId() );

	return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuMtStatusNotif
    功    能： 处理下级mcu直连终端的状态通知
    算法实现： 
    全局变量： 
    参    数： const CMessage *pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/11   3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::ProcMcuMcuMtStatusNotif( const CMessage *pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TMt		tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());  //sgx
	
	STATECHECK;	

	if( NULL == m_ptConfOtherMcTable )
	{
		return;
	}

	TMcu *ptMcu = (TMcu *)(cServMsg.GetMsgBody());
	TConfMcInfo *ptMcInfo = NULL;
	
	if( NULL == ptMcu || 0 == ptMcu->GetMcuId() )
	{
		return;
	}

	u16 wMcuIdx = INVALID_MCUIDX;
	u8 abyMcuId[MAX_CASCADEDEPTH-1];
	abyMcuId[0] = (u8)ptMcu->GetMcuId();
	if( ptMcu->GetMtId() != 0 )
	{
		abyMcuId[1] = ptMcu->GetMtId();
		m_tConfAllMcuInfo.GetIdxByMcuId( &abyMcuId[0],2,&wMcuIdx );
	}
	else
	{
		m_tConfAllMcuInfo.GetIdxByMcuId( &abyMcuId[0],1,&wMcuIdx );
	}
	
	ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
	if( NULL == ptMcInfo )
	{
		return;
	}

	u8 byMtNum = *(u8*)( cServMsg.GetMsgBody() + sizeof(TMcu) );//(cServMsg.GetMsgBodyLen()-sizeof(TMcu))/sizeof(TSMcuMtStatus);
	
	TSMcuMtStatus *ptSMcuMtStatus = (TSMcuMtStatus *)(cServMsg.GetMsgBody()+sizeof(TMcu)+sizeof(u8));
	CServMsg cServBackMsg;
	cServBackMsg.SetNoSrc();
	u8 byMtId = 0;
	BOOL32 bIsSend = FALSE;

	
	for( u8 byPos=0; byPos<byMtNum; byPos++ )
	{	
		if( ptSMcuMtStatus->m_byCasLevel > 0 )
		{
			byMtId  = ptSMcuMtStatus->m_abyMtIdentify[ptSMcuMtStatus->m_byCasLevel-1];
		}
		else
		{
			byMtId  = ptSMcuMtStatus->GetMtId();
		}
		
		s32 nIndex = 0;
		for(; nIndex<MAXNUM_CONF_MT; nIndex++)
		{
			if( ptMcInfo->m_atMtStatus[nIndex].IsNull() || 
				ptMcInfo->m_atMtStatus[nIndex].GetMtId() == 0 ) //自己
			{
				continue;
			}
			if(ptMcInfo->m_atMtStatus[nIndex].GetMtId() == byMtId)
			{
				break;
			}
		}
			
		bIsSend = FALSE;
		if( nIndex < MAXNUM_CONF_MT )
		{
			/*//ptMcInfo->m_atMtStatus[nIndex].SetMt( *((TMt *)ptSMcuMtStatus) );
			ptMcInfo->m_atMtStatus[nIndex].SetIsEnableFECC( ptSMcuMtStatus->IsEnableFECC() );
			ptMcInfo->m_atMtStatus[nIndex].SetCurVideo( ptSMcuMtStatus->GetCurVideo() );
			ptMcInfo->m_atMtStatus[nIndex].SetCurAudio( ptSMcuMtStatus->GetCurAudio() );
			ptMcInfo->m_atMtStatus[nIndex].SetMtBoardType( ptSMcuMtStatus->GetMtBoardType() );   
            ptMcInfo->m_atMtStatus[nIndex].SetInMixing( ptSMcuMtStatus->IsMixing() );
            ptMcInfo->m_atMtStatus[nIndex].SetVideoLose( ptSMcuMtStatus->IsVideoLose() );
			ptMcInfo->m_atMtStatus[nIndex].SetReceiveVideo( ptSMcuMtStatus->IsRecvVideo() );
			ptMcInfo->m_atMtStatus[nIndex].SetReceiveAudio( ptSMcuMtStatus->IsRecvAudio() );
			ptMcInfo->m_atMtStatus[nIndex].SetSendVideo( ptSMcuMtStatus->IsSendVideo() );
			ptMcInfo->m_atMtStatus[nIndex].SetSendAudio( ptSMcuMtStatus->IsSendAudio() );
			//zjj20100327
			ptMcInfo->m_atMtStatus[nIndex].SetMtDisconnectReason( ptSMcuMtStatus->GetMtDisconnectReason() );
			//sgx20100703[Bug00033516] 设置下级终端的呼叫方式
            if (ptSMcuMtStatus->IsAutoCallMode())
			{
                ptMcInfo->m_atMtExt[nIndex].SetCallMode(CONF_CALLMODE_TIMER);
			}
			else
			{
                ptMcInfo->m_atMtExt[nIndex].SetCallMode(CONF_CALLMODE_NONE);
			}*/			

			if( ptMcInfo->m_atMtStatus[nIndex].IsEnableFECC() != ptSMcuMtStatus->IsEnableFECC() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetIsEnableFECC( ptSMcuMtStatus->IsEnableFECC() );
			}
			if( ptMcInfo->m_atMtStatus[nIndex].GetCurVideo() != ptSMcuMtStatus->GetCurVideo() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetCurVideo( ptSMcuMtStatus->GetCurVideo() );
			}
			if( ptMcInfo->m_atMtStatus[nIndex].GetCurAudio() != ptSMcuMtStatus->GetCurAudio() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetCurAudio( ptSMcuMtStatus->GetCurAudio() );
			}
			if( ptMcInfo->m_atMtStatus[nIndex].GetCurAudio() != ptSMcuMtStatus->GetCurAudio() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetCurAudio( ptSMcuMtStatus->GetCurAudio() );
			}
			if( ptMcInfo->m_atMtStatus[nIndex].GetMtBoardType() != ptSMcuMtStatus->GetMtBoardType() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetMtBoardType( ptSMcuMtStatus->GetMtBoardType() );   
			}
			if( ptMcInfo->m_atMtStatus[nIndex].IsInMixing() != ptSMcuMtStatus->IsMixing() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetInMixing( ptSMcuMtStatus->IsMixing() );
				if (ptSMcuMtStatus->IsMixing())
				{
					MtStatusChange(&tMt,TRUE);
				}
			}
			if( ptMcInfo->m_atMtStatus[nIndex].IsVideoLose() != ptSMcuMtStatus->IsVideoLose() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetVideoLose( ptSMcuMtStatus->IsVideoLose() );
			}
			if( ptMcInfo->m_atMtStatus[nIndex].IsReceiveVideo() != ptSMcuMtStatus->IsRecvVideo() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetReceiveVideo( ptSMcuMtStatus->IsRecvVideo() );
			}
			if( ptMcInfo->m_atMtStatus[nIndex].IsReceiveAudio() != ptSMcuMtStatus->IsRecvAudio() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetReceiveAudio( ptSMcuMtStatus->IsRecvAudio() );
			}

			// [2013/07/26 chenbing] 终端是否在发送视频 
			if( ptMcInfo->m_atMtStatus[nIndex].IsSendVideo() != ptSMcuMtStatus->IsSendVideo() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetSendVideo( ptSMcuMtStatus->IsSendVideo() );
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuMtStatusNotif] New Video: %d\n", ptSMcuMtStatus->IsSendVideo());
				if ( ptSMcuMtStatus->IsSendVideo()
					&& !m_tConf.GetSpeaker().IsNull()
					&& ptMcInfo->m_atMtStatus[nIndex] == m_tConf.GetSpeaker()
					)
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuMtStatusNotif] ChangeSpeaker Video Switch\n");
					ChangeSpeaker(&m_tConf.GetSpeaker());
				}
			} 
			
			// [2013/07/26 chenbing] 终端是否在发送音频
			if( ptMcInfo->m_atMtStatus[nIndex].IsSendAudio() != ptSMcuMtStatus->IsSendAudio() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetSendAudio( ptSMcuMtStatus->IsSendAudio() );
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuMtStatusNotif] New Audio: %d\n", ptSMcuMtStatus->IsSendAudio());
				if ( ptSMcuMtStatus->IsSendAudio()
					&& !m_tConf.GetSpeaker().IsNull()
					&& ptMcInfo->m_atMtStatus[nIndex] == m_tConf.GetSpeaker()
					)
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuMtStatusNotif] ChangeSpeaker Audio Switch\n");
					ChangeSpeaker(&m_tConf.GetSpeaker());
				}
			}

			//zjj20100327
			if( ptMcInfo->m_atMtStatus[nIndex].GetMtDisconnectReason() != ptSMcuMtStatus->GetDisconnectReason() )
			{
				bIsSend = TRUE;
				ptMcInfo->m_atMtStatus[nIndex].SetMtDisconnectReason( ptSMcuMtStatus->GetDisconnectReason() );
			}
			
			if( ptMcInfo->m_atMtStatus[nIndex].IsSndVideo2() != ptSMcuMtStatus->IsSendVideo2() )
			{
				if (ptSMcuMtStatus->IsSendVideo2())
				{
					if( !m_tDoubleStreamSrc.IsNull() && 
						m_tDoubleStreamSrc.GetMtId() == GetLocalMtFromOtherMcuMt((TMt)(ptMcInfo->m_atMtStatus[nIndex])).GetMtId())
					{		
						bIsSend = TRUE;
						ptMcInfo->m_atMtStatus[nIndex].SetSndVideo2( ptSMcuMtStatus->IsSendVideo2() );
						UpdateVmpDStream((TMt)(ptMcInfo->m_atMtStatus[nIndex]));
					}
				}
				else
				{
					bIsSend = TRUE;
					ptMcInfo->m_atMtStatus[nIndex].SetSndVideo2(ptSMcuMtStatus->IsSendVideo2());
				}
			}			
			//sgx20100703[Bug00033516] 设置下级终端的呼叫方式
            if (ptSMcuMtStatus->IsAutoCallMode())
			{
				if( ptMcInfo->m_atMtExt[nIndex].GetCallMode() != CONF_CALLMODE_TIMER )
				{
					bIsSend = TRUE;
					ptMcInfo->m_atMtExt[nIndex].SetCallMode(CONF_CALLMODE_TIMER);
				}
			}
			else
			{
				if( ptMcInfo->m_atMtExt[nIndex].GetCallMode() != CONF_CALLMODE_NONE )
				{
					bIsSend = TRUE;
					ptMcInfo->m_atMtExt[nIndex].SetCallMode(CONF_CALLMODE_NONE);
				}
			}	

			if( bIsSend )
			{
				MtStatusChange( (TMt*)&(ptMcInfo->m_atMtStatus[nIndex]));
			}

		}
		ptSMcuMtStatus++;
	}
	//sgx20100703[Bug00033516]添加“下级MCU直连终端列表信息发送到会控”的功能
	//zjj 不能直接发送列表到界面，会导致界面卡死
	//SendMtListToMcs( wMcuIdx );

	ptSMcuMtStatus = (TSMcuMtStatus *)(cServMsg.GetMsgBody()+sizeof(TMcu)+sizeof(s32));

	if( !m_tCascadeMMCU.IsNull() && 
		MT_TYPE_SMCU == m_ptMtTable->GetMtType( tMt.GetMtId() ) &&
		ptSMcuMtStatus->m_byCasLevel < MAX_CASCADELEVEL
		)
	{
		// 如果启用了缓冲发送, 则用缓冲发送, 否则直接发送
		if( g_cMcuVcApp.GetMcuMcuMtStatusBufFlag() != 0 )
		{			
			// [11/15/2011 liuxu] 采取缓冲发送
			m_cSMcuMtStatusToMMcuBuf.Add(wMcuIdx);
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "Buf SMcu.%d MtStatus To MMcu\n", wMcuIdx);
			if (m_byMtStatusToMMcuTimerFlag)
			{
				return;
			}else
			{
				SetTimer(MCUVC_SEND_SMCUMTSTATUS_TIMER, g_cMcuVcApp.GetRefreshSMcuMtInterval());
				m_byMtStatusToMMcuTimerFlag = 1;
				return;
			}
		}else
		{
			//u8 byMtNum = (u8)nMtNum;
			cServBackMsg.SetMsgBody( (u8*)&byMtNum, sizeof( byMtNum ) );
			cServBackMsg.CatMsgBody( cServMsg.GetMsgBody()+sizeof(u8)+sizeof(TMcu),
				cServMsg.GetMsgBodyLen()-sizeof(u8)-sizeof(TMcu)
				);
			cServBackMsg.SetEventId( MCU_MCU_MTSTATUS_NOTIF );
			cServBackMsg.SetDstMtId( m_tCascadeMMCU.GetMtId() );
			SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_MTSTATUS_NOTIF, cServBackMsg );
		}
	}
	
	return;
}


/*=============================================================================
    函 数 名： ProcMcuMcuAutoSwitchReq
    功    能： 处理RadMMcu的自动轮询 请求
    算法实现： 
    全局变量： 
    参    数： const CMessage *pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2007/07/09  4.0  		张宝卿                  创建
=============================================================================*/
void CMcuVcInst::ProcMcuMcuAutoSwitchReq( const CMessage *pcMsg )
{
    CServMsg cServMsg( pcMsg->content, pcMsg->length );    
    u8 bySrcMtId = cServMsg.GetSrcMtId();
    TCAutoSwitchReq tSwitchReq = *(TCAutoSwitchReq*)cServMsg.GetMsgBody();

    ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuAutoSwitchReq] tSwitchReq with On.%d, level.%d, time.%d, lid.%d\n",
            tSwitchReq.m_bSwitchOn, tSwitchReq.m_nAutoSwitchLevel, 
            tSwitchReq.m_nSwitchSpaceTime, tSwitchReq.m_nSwitchLayerId );
    
    if ( m_tCascadeMMCU.IsNull() ||
         bySrcMtId != m_tCascadeMMCU.GetMtId() ||
         ( MT_MANU_RADVISION != m_ptMtTable->GetManuId(m_tCascadeMMCU.GetMtId()) &&
           MT_MANU_KDCMCU != m_ptMtTable->GetManuId(m_tCascadeMMCU.GetMtId()) ) )
    {
        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[AutoSwitchReq] Roused by Mt.%d, Manu.%d, ignore it\n",
                         bySrcMtId, m_ptMtTable->GetManuId(m_tCascadeMMCU.GetMtId()) );

        SendReplyBack( cServMsg, cServMsg.GetEventId()+2 );
        return;
    }    
    
    // 开始轮询(上级命令开始轮询，或者是下级通知其完成本级轮询）
    if ( tSwitchReq.m_bSwitchOn ) /*||
         ( m_tConfInStatus.IsPollSwitch() && 
           !tSwitchReq.m_bSwitchOn && tSwitchReq.m_nAutoSwitchLevel > m_tPollSwitchParam.GetLevel() ) )*/
    {
        u8 byMtId = 1;
        for(; byMtId <= MAXNUM_CONF_MT; byMtId++ )
        {
            TMtStatus tMtStatus;
            if ( m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
                 m_ptMtTable->GetMtStatus(byMtId, &tMtStatus) 
                 /*&& !tMtStatus.IsVideoLose()*/ )
            {
                break;
            }
        }
        if ( byMtId > MAXNUM_CONF_MT )
        {
            SendReplyBack( cServMsg, cServMsg.GetEventId()+2 );
            return;
        }

        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "Start auto poll switch.\n" );
        
        // 如果本级已经在轮询，则继续
        if ( m_tConfInStatus.IsPollSwitch() )
        {
        }
        else
        {
            m_tConfInStatus.SetPollSwitch(TRUE);
            m_tPollSwitchParam.Reset();
            m_tPollSwitchParam.SetTimeSpan(tSwitchReq.m_nSwitchSpaceTime);
            m_tPollSwitchParam.SetLevel(tSwitchReq.m_nAutoSwitchLevel);
            m_tPollSwitchParam.SetLid(tSwitchReq.m_nSwitchLayerId);
            m_tPollSwitchParam.SpecPollPos(POLLING_POS_START);        
        }

        // 开始轮询
        SetTimer( MCUVC_AUTOSWITCH_TIMER, 10);        
    }
    else
    {
        KillTimer( MCUVC_AUTOSWITCH_TIMER );
        m_tConfInStatus.SetPollSwitch(FALSE);
        m_tPollSwitchParam.Reset();        
        
        SendReplyBack( cServMsg, cServMsg.GetEventId()+1 );
    }
    return;
}

/*=============================================================================
    函 数 名： ProcMcuMcuAutoSwitchRsp
    功    能： 处理MMcu的自动轮询回传 的响应
    算法实现： 
    全局变量： 
    参    数： const CMessage *pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2007/07/19  4.0  		顾振华                  创建
=============================================================================*/
void CMcuVcInst::ProcMcuMcuAutoSwitchRsp( const CMessage *pcMsg )
{
    if (pcMsg->event == MCU_MCU_AUTOSWITCH_NACK)
    {
        // 继续本级轮询
        if ( m_tConfInStatus.IsPollSwitch() )
        {
            SetTimer( MCUVC_AUTOSWITCH_TIMER, 10);
        }
    }
}


/*=============================================================================
    函 数 名： ProcMcuMcuAutoSwitchTimer
    功    能： 定时轮询请求处理
    算法实现： 根据定时器带的终端ID,依次轮询所有在线的非视频丢失终端。轮询中加
               入的终端的ID若在当前轮询的终端ID前，则不被轮询到。
    全局变量： 
    参    数： const CMessage *pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2007/07/13  4.0  		张宝卿                  创建
=============================================================================*/
void CMcuVcInst::ProcMcuMcuAutoSwitchTimer( void )
{
    if ( !m_tConfInStatus.IsPollSwitch() )
    {
        return;
    }

    u8 byStartMtId;
    if ( m_tPollSwitchParam.IsSpecPos() )
    {
        byStartMtId = m_tPollSwitchParam.GetSpecPos();
        if (byStartMtId == POLLING_POS_START)
        {
            byStartMtId = 0;
        }
        m_tPollSwitchParam.ClearSpecPos();
    }
    else
    {
        byStartMtId = m_tPollSwitchParam.GetCurrPos() + 1;
    }

    u8 byMtId = byStartMtId;
    for( ; byMtId <= MAXNUM_CONF_MT; byMtId++ )
    {
        TMtStatus tMtStatus;
        if ( m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
             m_tCascadeMMCU.GetMtId() != byMtId &&
             m_ptMtTable->GetMtStatus(byMtId, &tMtStatus) &&
             tMtStatus.IsSendVideo() 
             /*&& !tMtStatus.IsVideoLose()*/ )
        {
            break;
        }
    }    
    if ( byMtId > MAXNUM_CONF_MT || m_tCascadeMMCU.IsNull() )
    {
        // 本级结束，通知上级继续开始
        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "All Mts has be switched over.\n");
        
        if (!m_tCascadeMMCU.IsNull())
        {
            TCAutoSwitchReq tSwitchRestartReq;
            tSwitchRestartReq.m_bSwitchOn = TRUE;
            tSwitchRestartReq.m_nAutoSwitchLevel = m_tPollSwitchParam.GetLevel() - 1;
            tSwitchRestartReq.m_nSwitchSpaceTime = m_tPollSwitchParam.GetTimeSpan();
            tSwitchRestartReq.m_nSwitchLayerId = m_tPollSwitchParam.GetLid();
            
            CServMsg cServMsg;
            cServMsg.SetEventId(MCU_MCU_AUTOSWITCH_REQ);
            cServMsg.SetMsgBody((u8*)&tSwitchRestartReq, sizeof(tSwitchRestartReq));
            
            SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_AUTOSWITCH_REQ, cServMsg );
            ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "Notify restart auto poll swtich to mmcu, bOn.%d, lelev.%d, time.%d, lid.%d\n" ,
                    tSwitchRestartReq.m_bSwitchOn, tSwitchRestartReq.m_nAutoSwitchLevel,
                    tSwitchRestartReq.m_nSwitchSpaceTime, tSwitchRestartReq.m_nAutoSwitchLevel );
        }
        
        KillTimer( MCUVC_AUTOSWITCH_TIMER );
        m_tConfInStatus.SetPollSwitch(FALSE);
        m_tPollSwitchParam.Reset();
    }
    else
    {
        TMt tSetInMt = m_ptMtTable->GetMt(byMtId);
        m_tPollSwitchParam.SetCurrPos(byMtId);
        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "Auto Switched to Setout Mt.%d to poll.\n", byMtId );
        
        if ( tSetInMt.GetMtType() == MT_TYPE_MT)
        {            
            OnSetOutView(tSetInMt);

            SetTimer( MCUVC_AUTOSWITCH_TIMER, m_tPollSwitchParam.GetTimeSpan()*1000);
        }
        else
        {
            // 开始下级的回传轮询，本级定时器停止（但是不清除状态，等下级回来结束后继续）
            TCAutoSwitchReq tSwitchStartSMcuReq;
            tSwitchStartSMcuReq.m_bSwitchOn = TRUE;
            tSwitchStartSMcuReq.m_nAutoSwitchLevel = m_tPollSwitchParam.GetLevel() + 1;
            tSwitchStartSMcuReq.m_nSwitchSpaceTime = m_tPollSwitchParam.GetTimeSpan();
            tSwitchStartSMcuReq.m_nSwitchLayerId = m_tPollSwitchParam.GetLid();

            CServMsg cServMsg;
            cServMsg.SetEventId(MCU_MCU_AUTOSWITCH_REQ);
            cServMsg.SetMsgBody((u8*)&tSwitchStartSMcuReq, sizeof(tSwitchStartSMcuReq));
            
            SendMsgToMt( tSetInMt.GetMtId(), MCU_MCU_AUTOSWITCH_REQ, cServMsg );
            KillTimer( MCUVC_AUTOSWITCH_TIMER );
        }                
    }
    
    return;
}
/*=============================================================================
    函 数 名： OnGetMtStatusCmdToSMcu
    功    能： 向下级mcu发送请求其直连终端状态报告的命令
    算法实现： 
    全局变量： 
    参    数： u8 byDstMcuId
               TMtStatus* ptSrcMtStatus
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/11   3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::OnGetMtStatusCmdToSMcu( u8 byDstMcuId )
{
	CServMsg cServMsg;
	cServMsg.SetDstMtId( byDstMcuId );
	cServMsg.SetEventId( MCU_MCU_MTSTATUS_CMD );
	SendMsgToMt( byDstMcuId, MCU_MCU_MTSTATUS_CMD, cServMsg );

	return;
}
		
/*=============================================================================
    函 数 名： OnNtfMtStatusToMMcu
    功    能： 向上级mcu发送本级直连终端的状态报告
    算法实现： 
    全局变量： 
    参    数： u8 byDstMcuId
               u8 byMtId
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/11   3.6			万春雷                创建
    2007/11/29  4.0         张宝卿                接口优化
=============================================================================*/
void CMcuVcInst::OnNtfMtStatusToMMcu(u8 byDstMcuId, u8 byMtId)
{
    if ( byMtId > MAXNUM_CONF_MT || NULL == m_ptMtTable)
    {
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MMCU, "[OnNtfMtStatusToMMcu] Mt.%d is illeagal, ignore it!\n", byMtId);
        return;
    }
	// mcu终端实时上报
	if ( byMtId != 0 
		&&  (/*m_ptMtTable && */!IsMcu(m_ptMtTable->GetMt(byMtId))))
	{
		if(byDstMcuId == m_tCascadeMMCU.GetMtId())
		{
			// 启用了缓冲发送, 则用缓冲发送机制, 否则直接发送
			if(g_cMcuVcApp.GetMcuMcuMtStatusBufFlag() != 0)
			{
				// [11/14/2011 liuxu] 缓冲发送
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "Buf snd localMt.%d Status To MMcu\n", byMtId);
				m_cLocalMtStatusToMMcuBuf.Add(byMtId);
				if (m_byMtStatusToMMcuTimerFlag)
				{
					return;
				}else
				{
					SetTimer(MCUVC_SEND_SMCUMTSTATUS_TIMER, g_cMcuVcApp.GetRefreshSMcuMtInterval());
					m_byMtStatusToMMcuTimerFlag = 1;
					return;
				}
			}
		}
	}

	CServMsg cServMsg;
	CServMsg cMsg;
	u8 byMtNum = 0;
	cServMsg.SetDstMtId( byDstMcuId );
	cServMsg.SetEventId( MCU_MCU_MTSTATUS_NOTIF );

	if( 0 == byMtId )
	{
		TMtStatus tLocalMtStatus;
		TSMcuMtStatus tSMcuMtStatus;
		for( u8 byMtLoop = 1; byMtLoop <= MAXNUM_CONF_MT; byMtLoop++ )
		{
			// 对于下级MCU只上报上线的终端状态
			// 对于VCS会议还需要上报定时呼叫终端的信息
			//zhouyiliang 20101224 vcs会议也不将上级mcu再往上报
			if( byDstMcuId != byMtLoop &&
					( m_tConfAllMtInfo.MtJoinedConf( byMtLoop ) ||
						(VCS_CONF == m_tConf.GetConfSource() && m_tConfAllMtInfo.MtInConf(byMtLoop)) )//CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode(byMtLoop)))
				)
			{		
				m_ptMtTable->GetMtStatus( byMtLoop, &tLocalMtStatus );
				memcpy( (void*)&tSMcuMtStatus, (void*)&tLocalMtStatus, sizeof(TMt) );
				tSMcuMtStatus.SetIsEnableFECC( tLocalMtStatus.IsEnableFECC() );
				tSMcuMtStatus.SetCurVideo( tLocalMtStatus.GetCurVideo() );
				tSMcuMtStatus.SetCurAudio( tLocalMtStatus.GetCurAudio() );
				tSMcuMtStatus.SetMtBoardType( tLocalMtStatus.GetMtBoardType() );
                tSMcuMtStatus.SetIsMixing( tLocalMtStatus.IsInMixing() );
                tSMcuMtStatus.SetVideoLose( tLocalMtStatus.IsVideoLose() );
				BOOL32 bAutoMode = (CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode(byMtLoop)) ? TRUE : FALSE;
				tSMcuMtStatus.SetIsAutoCallMode(bAutoMode);
				tSMcuMtStatus.SetRecvVideo( tLocalMtStatus.IsReceiveVideo() );
				tSMcuMtStatus.SetRecvAudio( tLocalMtStatus.IsReceiveAudio() );
				tSMcuMtStatus.SetSendVideo( tLocalMtStatus.IsSendVideo() );
				tSMcuMtStatus.SetSendAudio( tLocalMtStatus.IsSendAudio() );

				//zjj20100327
				tSMcuMtStatus.SetDisconnectReason( tLocalMtStatus.GetMtDisconnectReason() );
				tSMcuMtStatus.SetSendVideo2(tLocalMtStatus.IsSndVideo2());
				tSMcuMtStatus.SetMcuIdx( LOCAL_MCUID );

				if( 0 == byMtNum )
				{
					cServMsg.SetMsgBody( (u8*)&tSMcuMtStatus, sizeof(TSMcuMtStatus) );
				}
				else
				{
					cServMsg.CatMsgBody( (u8*)&tSMcuMtStatus, sizeof(TSMcuMtStatus) );
				}
				byMtNum++;
			}
		}
	}
	else
	{
        TMtStatus tLocalMtStatus;
        TSMcuMtStatus tSMcuMtStatus;
        
        if( m_tConfAllMtInfo.MtInConf(byMtId) && byDstMcuId != byMtId )
        {		
            m_ptMtTable->GetMtStatus( byMtId, &tLocalMtStatus );
            memcpy( (void*)&tSMcuMtStatus, (void*)&tLocalMtStatus, sizeof(TMt) );
            tSMcuMtStatus.SetIsEnableFECC( tLocalMtStatus.IsEnableFECC() );
            tSMcuMtStatus.SetCurVideo( tLocalMtStatus.GetCurVideo() );
            tSMcuMtStatus.SetCurAudio( tLocalMtStatus.GetCurAudio() );
            tSMcuMtStatus.SetMtBoardType( tLocalMtStatus.GetMtBoardType() );
            tSMcuMtStatus.SetIsMixing( tLocalMtStatus.IsInMixing() );
            tSMcuMtStatus.SetVideoLose( tLocalMtStatus.IsVideoLose() );
			BOOL32 bAutoMode = (CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode(byMtId)) ? TRUE : FALSE;
			tSMcuMtStatus.SetIsAutoCallMode(bAutoMode);
			tSMcuMtStatus.SetRecvVideo( tLocalMtStatus.IsReceiveVideo() );
			tSMcuMtStatus.SetRecvAudio( tLocalMtStatus.IsReceiveAudio() );
			tSMcuMtStatus.SetSendVideo( tLocalMtStatus.IsSendVideo() );
			tSMcuMtStatus.SetSendAudio( tLocalMtStatus.IsSendAudio() );

			//zjj20100327
			tSMcuMtStatus.SetDisconnectReason( tLocalMtStatus.GetMtDisconnectReason() );
			tSMcuMtStatus.SetSendVideo2(tLocalMtStatus.IsSndVideo2());
			tSMcuMtStatus.SetMcuIdx( LOCAL_MCUID );
            if( 0 == byMtNum )
            {
                cServMsg.SetMsgBody( (u8*)&tSMcuMtStatus, sizeof(TSMcuMtStatus) );
            }
            else
            {
                cServMsg.CatMsgBody( (u8*)&tSMcuMtStatus, sizeof(TSMcuMtStatus) );
            }
            byMtNum++;
        }
	}

	if( byMtNum > 0 )
	{
		cMsg.SetDstMtId( byDstMcuId );
		cMsg.SetEventId( MCU_MCU_MTSTATUS_NOTIF );
		cMsg.SetMsgBody( (u8*)&byMtNum,sizeof(u8) );
		cMsg.CatMsgBody( cServMsg.GetMsgBody(),cServMsg.GetMsgBodyLen() );		
		SendMsgToMt( byDstMcuId, MCU_MCU_MTSTATUS_NOTIF, cMsg );
	}

	return;
}


/*=============================================================================
    函 数 名： OnNtfMtStatusToMMcu
    功    能： 向上级mcu发送本级直连终端的状态报告
    算法实现： 
    全局变量： 
    参    数： u8 byDstMcuId
               u8 byMtId
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2011/11/18  4.6         lixuu				  创建
=============================================================================*/
void CMcuVcInst::OnBufSendMtStatusToMMcu( )
{
	if (CurState() != STATE_ONGOING)
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "OnBufSendMtStatusToMMcu called at wrong Vc state.%d, return\n", CurState());
		return;
	}
	
	m_byMtStatusToMMcuTimerFlag = 0;				// 重置状态
	
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "start buf snd MtStatus To MMcu\n");

	// 没有上级, 清空并返回
	if (m_tCascadeMMCU.IsNull())
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "MMcu is null\n");
		m_cLocalMtStatusToMMcuBuf.ClearAll();
		m_cSMcuMtStatusToMMcuBuf.ClearAll();
		return;
	}

	// 获取本地终端数目
	const u8 bySMcuMtNum = (u8)m_cLocalMtStatusToMMcuBuf.GetUsedNum();
	
	// 先发送本地终端状态
	if ( bySMcuMtNum )
	{
		CServMsg cMsg;
		cMsg.SetDstMtId( m_tCascadeMMCU.GetMtId() );
		cMsg.SetEventId( MCU_MCU_MTSTATUS_NOTIF );
		cMsg.SetMsgBody( (u8*)&bySMcuMtNum,sizeof(u8) );
		
		const u32 dwMaxBufNum = m_cLocalMtStatusToMMcuBuf.GetCapacity();
		u8 byMtId;
		TMtStatus tLocalMtStatus;
		TSMcuMtStatus tSMcuMtStatus;

		u32 dwActSendNum = 0;
		
		for ( u32 dwLoop = 0; dwLoop < dwMaxBufNum; dwLoop++ )
		{
			byMtId = 0;
			if(!m_cLocalMtStatusToMMcuBuf.Get(dwLoop, byMtId))
				continue;
			
			if ( !IsValidMtId(byMtId)) 
			{
				continue;
			}
			
			if( m_tConfAllMtInfo.MtInConf(byMtId) && m_tCascadeMMCU.GetMtId() != byMtId )
			{		
				m_ptMtTable->GetMtStatus( byMtId, &tLocalMtStatus );
				memcpy( (void*)&tSMcuMtStatus, (void*)&tLocalMtStatus, sizeof(TMt) );
				tSMcuMtStatus.SetIsEnableFECC( tLocalMtStatus.IsEnableFECC() );
				tSMcuMtStatus.SetCurVideo( tLocalMtStatus.GetCurVideo() );
				tSMcuMtStatus.SetCurAudio( tLocalMtStatus.GetCurAudio() );
				tSMcuMtStatus.SetMtBoardType( tLocalMtStatus.GetMtBoardType() );
				tSMcuMtStatus.SetIsMixing( tLocalMtStatus.IsInMixing() );
				tSMcuMtStatus.SetVideoLose( tLocalMtStatus.IsVideoLose() );
				BOOL32 bAutoMode = ( CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode(byMtId) ) ? TRUE : FALSE;
				tSMcuMtStatus.SetIsAutoCallMode(bAutoMode);
				tSMcuMtStatus.SetRecvVideo( tLocalMtStatus.IsReceiveVideo() );
				tSMcuMtStatus.SetRecvAudio( tLocalMtStatus.IsReceiveAudio() );
				tSMcuMtStatus.SetSendVideo( tLocalMtStatus.IsSendVideo() );
				tSMcuMtStatus.SetSendAudio( tLocalMtStatus.IsSendAudio() );
				
				//zjj20100327
				tSMcuMtStatus.SetDisconnectReason( tLocalMtStatus.GetMtDisconnectReason() );
				tSMcuMtStatus.SetSendVideo2(tLocalMtStatus.IsSndVideo2());
				tSMcuMtStatus.SetMcuIdx( LOCAL_MCUID );
				
				dwActSendNum++;
				cMsg.CatMsgBody( (u8*)&tSMcuMtStatus, sizeof(TSMcuMtStatus) );
			}
		}
		
		m_cLocalMtStatusToMMcuBuf.ClearAll();
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "send %u localmt status to mmcu\n", dwActSendNum);
		SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_MTSTATUS_NOTIF, cMsg );
	}

	// 再发送下级终端状态
	const u16 wSMcuNum = (u16)m_cSMcuMtStatusToMMcuBuf.GetUsedNum();
	if (wSMcuNum)
	{
		const u32 dwMaxBufNum = m_cLocalMtStatusToMMcuBuf.GetCapacity();
		u16 wMcuIdx = INVALID_MCUIDX;
		u32 dwActSendNum = 0;
		for ( u32 dwSMcuLoop = 0; dwSMcuLoop < dwMaxBufNum; dwSMcuLoop++ )
		{
			wMcuIdx = INVALID_MCUIDX;
			if(!m_cSMcuMtStatusToMMcuBuf.Get(dwSMcuLoop, wMcuIdx))
				continue;
			
			if ( !IsValidMcuId(wMcuIdx)) 
			{
				continue;
			}

			SendSMcuMtStatusToMMcu(wMcuIdx);
			dwActSendNum++;
		}

		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "send %u smcu to mmcu\n", dwActSendNum);
	}
}

/*====================================================================
    函数名      ：BroadcastToAllMcu
    功能        ：发消息给所有下级MCU
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wEvent 消息号
	              const CMessage * pcMsg, 传入的消息
				  u8 byExceptMc 除此Mc外
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/08/30    3.5         胡昌威         创建
	10/12/30    4.6         liuxu          修改
====================================================================*/
void CMcuVcInst::BroadcastToAllMcu( u16 wEvent, CServMsg & cServMsg )
{
	u16 wMcuIdx = 0;
	u16 wNum = 0;
	u16 wCount = m_tConfAllMcuInfo.GetMcuCount();
	u8 abyMcuId[MAX_CASCADEDEPTH-1];	
	
	for( u16 wLoop = 0; wLoop < m_tConfAllMtInfo.GetMaxMcuNum(); ++wLoop )
	{
		if( wNum >= wCount )
		{
			break;
		}

		wMcuIdx = m_tConfAllMtInfo.GetMtInfo(wLoop).GetMcuIdx();
		if( m_tConfAllMtInfo.GetMtInfo(wLoop).IsNull() 
			|| !IsValidMcuId(wMcuIdx))
		{
			continue;
		}

		memset( &abyMcuId[0],0,sizeof(abyMcuId) );
		if( !m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuIdx,&abyMcuId[0] ) )
		{			
			continue;
		}	
		
		if( 0 == abyMcuId[0] )
		{
			continue;
		}

		if( abyMcuId[1] != 0 )
		{
			++wNum;
			continue;
		}
		
		SendMsgToMt( abyMcuId[0], wEvent, cServMsg );		
		++wNum;
	}
}


/*====================================================================
    函数名      ：NotifyMcuNewMt
    功能        ：通知其他MC新终端加入
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	05/01/25    3.6         Jason         修改
====================================================================*/
void CMcuVcInst::NotifyMcuNewMt(TMt tMt)
{
	CServMsg	cServMsg;
	TMtStatus   tMtStatus;
    TMcuMcuMtInfo tInfo;

	//请求终端列表
	u8 byMtId = tMt.GetMtId();
	tMt.SetMcuId( LOCAL_MCUID );
	tInfo.m_tMt = tMt;


	TMtAlias tTmpMtAlias;
	m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeE164, &tTmpMtAlias);
	
	TMt tLocalMt = m_ptMtTable->GetMt(byMtId);

	// 如果是电话终端，优先取E164号
	if (IsPhoneMt(tLocalMt)&&!tTmpMtAlias.IsAliasNull())
	{
		strncpy((s8 *)(tInfo.m_szMtName), tTmpMtAlias.m_achAlias, sizeof(tInfo.m_szMtName));
	}
	else if ( m_ptMtTable->GetMtAlias(tMt.GetMtId(), mtAliasTypeH323ID, &tTmpMtAlias))
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcMcuMcuMtListReq]copy 323ID to Mt.%d\n", tMt.GetMtId());
		strncpy((s8 *)(tInfo.m_szMtName), tTmpMtAlias.m_achAlias, sizeof(tInfo.m_szMtName) - 1);
	}
	else
	{
		strncpy((s8 *)(tInfo.m_szMtName), m_ptMtTable->GetMtAliasFromExt(tMt.GetMtId()), sizeof(tInfo.m_szMtName));
	}

	tInfo.m_szMtName[sizeof(tInfo.m_szMtName)-1] = 0;
	tInfo.m_dwMtIp   = htonl(m_ptMtTable->GetIPAddr(byMtId));
	tInfo.m_byMtType = m_ptMtTable->GetMtType(byMtId);
	tInfo.m_byManuId = m_ptMtTable->GetManuId(byMtId);
	strncpy((s8 *)tInfo.m_szMtDesc, "notsupport", sizeof(tInfo.m_szMtDesc));
	tInfo.m_szMtDesc[sizeof(tInfo.m_szMtDesc)-1] = 0;
	TLogicalChannel  tChannel;
	if( m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_VIDEO, &tChannel, FALSE ) )
	{
		tInfo.m_byVideoIn = tChannel.GetChannelType();
	}
	else
	{
		tInfo.m_byVideoIn = MEDIA_TYPE_NULL;
	}
	if( m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_VIDEO, &tChannel, TRUE ) )
	{
		tInfo.m_byVideoOut = tChannel.GetChannelType();
	}
	else
	{
		tInfo.m_byVideoOut = MEDIA_TYPE_NULL;
	}
	if( m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_SECVIDEO, &tChannel, FALSE ) )
	{
		tInfo.m_byVideo2In = tChannel.GetChannelType();
	}
	else
	{
		tInfo.m_byVideo2In = MEDIA_TYPE_NULL;
	}
	if( m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_SECVIDEO, &tChannel, TRUE ) )
	{
		tInfo.m_byVideo2Out = tChannel.GetChannelType();
	}
	else
	{
		tInfo.m_byVideo2Out = MEDIA_TYPE_NULL;
	}
	if( m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_AUDIO, &tChannel, FALSE ) )
	{
		tInfo.m_byAudioIn = tChannel.GetChannelType();
	}
	else
	{
		tInfo.m_byAudioIn = MEDIA_TYPE_NULL;
	}
	if( m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_AUDIO, &tChannel, TRUE ) )
	{
		tInfo.m_byAudioOut = tChannel.GetChannelType();
	}
	else
	{
		tInfo.m_byAudioOut = MEDIA_TYPE_NULL;
	}
	tInfo.m_byIsDataMeeting =  m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_T120DATA, &tChannel, TRUE ) ? 1:0;
	if(!ISTRUE(tInfo.m_byIsDataMeeting))
	{
		tInfo.m_byIsDataMeeting =  m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_T120DATA, &tChannel, FALSE ) ? 1:0;
	}	
	m_ptMtTable->GetMtStatus(byMtId, &tMtStatus);
	tInfo.m_byIsVideoMuteIn		= GETBBYTE(!tMtStatus.IsSendVideo());
	tInfo.m_byIsVideoMuteOut	= GETBBYTE(!tMtStatus.IsReceiveVideo());   
	tInfo.m_byIsAudioMuteIn		= GETBBYTE(!tMtStatus.IsSendAudio());
	tInfo.m_byIsAudioMuteOut	= GETBBYTE(!tMtStatus.IsReceiveAudio());
	tInfo.m_byIsConnected		= GETBBYTE(m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId()));
	tInfo.m_byIsFECCEnable		= GETBBYTE(m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_H224DATA, &tChannel, TRUE )); 
	
	tInfo.m_tPartVideoInfo.m_nViewCount = 0;
	if(tMt == m_tConf.GetSpeaker())
	{
		tInfo.m_tPartVideoInfo.m_atViewPos[tInfo.m_tPartVideoInfo.m_nViewCount].m_nViewID = m_dwSpeakerViewId;
		tInfo.m_tPartVideoInfo.m_atViewPos[tInfo.m_tPartVideoInfo.m_nViewCount].m_bySubframeIndex = 0;
		tInfo.m_tPartVideoInfo.m_nViewCount++;
	}
	TVMPParam tVmpParam = m_tConf.GetStatus().GetVmpParam();
	if(tVmpParam.IsMtInMember(tMt))
	{
		u8 byMemberId  = m_tConf.GetStatus().GetVmpParam().GetChlOfMtInMember( tMt );
		tInfo.m_tPartVideoInfo.m_atViewPos[tInfo.m_tPartVideoInfo.m_nViewCount].m_nViewID = m_dwVmpViewId;
		tInfo.m_tPartVideoInfo.m_atViewPos[tInfo.m_tPartVideoInfo.m_nViewCount].m_bySubframeIndex = byMemberId;
		tInfo.m_tPartVideoInfo.m_nViewCount++;
	}
    
	TMt tMtVSrc;
	m_ptMtTable->GetMtSrc(byMtId, &tMtVSrc, MODE_VIDEO);
	if(tMtVSrc.IsNull())
	{
		tInfo.m_tPartVideoInfo.m_nOutputLID = 0;
	}
	else if(tMtVSrc == m_tConf.GetSpeaker())
	{
		tInfo.m_tPartVideoInfo.m_nOutputLID = m_dwSpeakerViewId;
	}
	else if(tMtVSrc == m_tVmpEqp)
	{
			tInfo.m_tPartVideoInfo.m_nOutputLID = m_dwVmpViewId;
	}
    tInfo.m_tPartVideoInfo.m_nOutVideoSchemeID = 0;

    ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[NotifyMcuNewMt] MtId.%d szMtName.%s \n", tInfo.m_tMt.GetMtId(), tInfo.m_szMtName);

	cServMsg.SetMsgBody((u8 *)&tInfo, sizeof(TMcuMcuMtInfo));
	BroadcastToAllMcu( MCU_MCU_NEWMT_NOTIF, cServMsg );

	// vcs刷列表，同时需要刷新其状态信息，由于其中包含重要的终端呼叫模式信息
	if (VCS_CONF == m_tConf.GetConfSource() &&
		!m_tCascadeMMCU.IsNull())
	{
		OnNtfMtStatusToMMcu(m_tCascadeMMCU.GetMtId(),tMt.GetMtId());
	}

}

/*====================================================================
    函数名      ：NotifyMcuDropMt
    功能        ：通知其他MC挂断终端
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	05/01/25    3.6         Jason         修改
====================================================================*/
void CMcuVcInst::NotifyMcuDropMt(TMt tMt)
{
	CServMsg	cServMsg;
	TMsgHeadMsg tMsgHeadMsg;


	tMt.SetMcuId( LOCAL_MCUID );
	//tMsgHeadMsg.m_tMsgSrc.m_tMt = tMt;	
	cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
	cServMsg.CatMsgBody( (u8*)&tMsgHeadMsg,sizeof(TMsgHeadMsg) );	
	BroadcastToAllMcu( MCU_MCU_DROPMT_NOTIF, cServMsg );
}

/*====================================================================
    函数名      ：NotifyMcuDropMt
    功能        ：通知其他MC删除终端
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	05/01/25    3.6         Jason         修改
====================================================================*/
void CMcuVcInst::NotifyMcuDelMt(TMt tMt)
{
	CServMsg	cServMsg;
	TMsgHeadMsg tMsgHeadMsg;


	tMt.SetMcuId( LOCAL_MCUID );
	//tMsgHeadMsg.m_tMsgSrc.m_tMt = tMt;	
	
	cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
	cServMsg.CatMsgBody( (u8*)&tMsgHeadMsg,sizeof(TMsgHeadMsg) );	
	BroadcastToAllMcu( MCU_MCU_DELMT_NOTIF, cServMsg );
}


/*====================================================================
    函数名      : GetOtherMcData
    功能        : 得到其它Mc的数据
    算法实现    : 
    引用全局变量: 无
    输入参数说明: 无
    返回值说明  : 成功-TRUE 未级联-FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/11/13    3.5         胡昌威       创建
====================================================================*/
BOOL32 CMcuVcInst::GetMcData( u8 byMcuId, TConfMtInfo &tConfMtInfo, TConfMcInfo &tConfOtherMcInfo )
{
	TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(byMcuId);
	if(ptConfMcInfo == NULL)
	{
		return FALSE;
	}
	tConfMtInfo = m_tConfAllMtInfo.GetMtInfo(byMcuId);

	memcpy(&tConfOtherMcInfo, ptConfMcInfo,sizeof(TConfMcInfo));

	return TRUE;
}

 
/*====================================================================
    函数名      : GetMcuMcInfo
    功能        : 获取mcu的Mc信息
    算法实现    : 
    引用全局变量: 无
    输入参数说明: [in] tMcu, 要获取的mcu终端(必须是mcu终端, 否则返回NULL)
    返回值说明  : 成功返回mc信息, 失败返回NULL
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    11/10/18    4.6         刘旭          创建
====================================================================*/
TConfMcInfo* CMcuVcInst::GetMcuMcInfo( const TMt& tMcu)
{
	if (!m_ptConfOtherMcTable)
	{
		return NULL;
	}	

	// 本身不是mcu, 返回
	if (!IsMcu(tMcu))
	{
		return NULL;
	}
	
	// 获取的mcu信息
	TConfMcInfo *ptConfOtherMcInfo = NULL;
	
	if ( !tMcu.IsLocal() )
	{
		u8 abyMcuId[MAX_CASCADEDEPTH-1] = { 0 };
		abyMcuId[0] = GetFstMcuIdFromMcuIdx(tMcu.GetMcuIdx());
		abyMcuId[1] = tMcu.GetMtId();
		u16 wMcuIdx = INVALID_MCUIDX;
		
		if( !m_tConfAllMcuInfo.GetIdxByMcuId( &abyMcuId[0], 2, &wMcuIdx ))
		{
			return NULL;
		}

		ptConfOtherMcInfo = m_ptConfOtherMcTable->GetMcInfo( wMcuIdx );
	}else
	{
		ptConfOtherMcInfo = m_ptConfOtherMcTable->GetMcInfo( GetMcuIdxFromMcuId(tMcu.GetMtId()) );
	}
	
	return ptConfOtherMcInfo;
}

/*====================================================================
    函数名      ：GetLocalSpeaker
    功能        ：得到本地的发言人
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/09/13    3.5         胡昌威        创建
====================================================================*/
TMt  CMcuVcInst::GetLocalSpeaker( void )
{
	TMt tSpeaker = m_tConf.GetSpeaker();

	if( tSpeaker.IsNull() )								// NULL,返回NULL
	{
		return tSpeaker;
	}
	else if( tSpeaker.IsLocal() )						// 发言人是本地MT或本级子mcu
	{
		return tSpeaker;
	}
	else												// 发言人是下级MT或下级mcu
	{		
		return m_ptMtTable->GetMt( GetFstMcuIdFromMcuIdx( tSpeaker.GetMcuIdx() ) );
	}
}

/*====================================================================
    函数名      ：GetMcuMediaSrc
    功能        ：得到Mcu真正的数据源
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/09/13    3.5         胡昌威        创建
====================================================================*/
TMt  CMcuVcInst::GetMcuMediaSrc(u16 wMcuIdx)
{
	TMt tMt;
	tMt.SetMcuIdx(wMcuIdx);
	tMt.SetMtId(0);

	//u16  wMcuIdx = GetMcuIdxFromMcuId( byMcuId );
	

	//m_tConfAllMcuInfo.GetIdxByMcuId( byMcuId,0,&wMcuIdx );	
	TConfMcInfo *ptInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
	if(ptInfo == NULL)
	{
		return tMt;
	}

	return ptInfo->m_tMMcuViewMt;
}

/*====================================================================
    函数名      ：ProcMcuMcuSendMsgReq
    功能        ：短消息发送处理函数
    算法实现    ：
    引用全局变量：
    输入参数说明：const CMessage * pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/09/29    3.0         胡昌威          创建
	10/08/12	4.6			xl				multi-cascade support and codes improve
====================================================================*/
void CMcuVcInst::ProcMcuMcuSendMsgReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );


	// 如果是下级MCU向上级传递, 则忽略, zgc, 2007-04-07
	TMt tSrcMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	if( tSrcMt.GetMtType() == MT_TYPE_SMCU )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuSendMsgReq]This is unallowed that transmit the short msgs to mmcu from smcu!\n");
		return;
	}

	TMt tMt = *(TMt *)( cServMsg.GetMsgBody() );
	CRollMsg* ptROLLMSG = (CRollMsg*)( cServMsg.GetMsgBody() + sizeof(TMt));

	
// 	TMsgHeadMsg tHeadMsg;
// 	if( cServMsg.GetMsgBodyLen > (sizeof(TMt) + ptROLLMSG->GetTotalMsgLen()) )
// 	{
// 		tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody() + sizeof(TMt) + ptROLLMSG->GetTotalMsgLen() );
// 	}

	switch( CurState() )
	{
	case STATE_ONGOING:

		if( tMt.IsNull() )	//发给所有终端 MCU_MCU_SENDMSG_NOTIF
		{
			for(u8 byLoop = 1; byLoop<=MAXNUM_CONF_MT; byLoop++)
			{
				if( m_tConfAllMtInfo.MtJoinedConf(byLoop) && byLoop != cServMsg.GetSrcMtId() )
                {
                    if (m_ptMtTable->GetMtType(byLoop) == MT_TYPE_MT)
                    {
						cServMsg.SetMsgBody( cServMsg.GetMsgBody() , (sizeof(TMt) + ptROLLMSG->GetTotalMsgLen()));
                        SendMsgToMt( byLoop, MCU_MT_SENDMSG_NOTIF, cServMsg );	
                    }
                    else if(m_ptMtTable->GetMtType(byLoop) == MT_TYPE_SMCU)// xsl [9/26/2006] 支持多级短消息广播, zgc [04/03/2007] 不往上级MCU发送
                    {
						ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "[ProcMcuMcuSendMsgReq]-send to Mt.%u(type:SMCU) \n", byLoop);
						TMsgHeadMsg tHeadMsg;
						CServMsg cSendMsg;
						cSendMsg.SetMsgBody( (u8*)&tHeadMsg, sizeof(TMsgHeadMsg) );
						cSendMsg.CatMsgBody( (u8*)&tMt, sizeof(TMt) );
						cSendMsg.CatMsgBody( (u8*)ptROLLMSG, ptROLLMSG->GetTotalMsgLen() );
                        SendMsgToMt( byLoop, MCU_MCU_SENDMSG_NOTIF, cSendMsg );
                    }
                }			
			}
		}
		else //发给某一终端 
		{
			ProcSingleMtSmsOpr(cServMsg, &tMt, ptROLLMSG);
// 			if( !tMt.IsMcuIdLocal() )
// 			{
// 				SendMsgToMt( (u8)tMt.GetMcuId(), MCU_MCU_SENDMSG_NOTIF, cServMsg );
// 			}
// 			else
// 			{
// 				if(tMt.GetMtId() != cServMsg.GetSrcMtId())
// 				{
// 					if ( m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_MT )
// 					{
// 						cServMsg.SetMsgBody( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg),cServMsg.GetMsgBodyLen() - sizeof(TMsgHeadMsg));
// 						SendMsgToMt( tMt.GetMtId(), MCU_MT_SENDMSG_NOTIF, cServMsg );
// 					}
// 					else if (m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_SMCU)// xsl [9/26/2006] 支持多级短消息广播, zgc [04/03/2007] 不往上级MCU发送
// 					{
// 						CServMsg cTmpMsg(pcMsg->content, pcMsg->length);
// 						TMt tNullMt;
// 						tNullMt.SetNull();
// 						cTmpMsg.SetMsgBody((u8*)&tNullMt, sizeof(tNullMt));
// 						cTmpMsg.CatMsgBody((cServMsg.GetMsgBody() + sizeof(TMt)), cServMsg.GetMsgBodyLen()-sizeof(TMt));
// 						SendMsgToMt( tMt.GetMtId(), MCU_MCU_SENDMSG_NOTIF, cTmpMsg );
// 					}
// 				}
// 			}
		}
		break;

	default:
		ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "Wrong message %u(%s) received in state %u!\n", 
			   pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*=============================================================================
    函 数 名： OnMMcuSetIn
    功    能： 请求下级Mcu码流回传
    算法实现： 
    全局变量： 
    参    数： TMt &tMt
               u8 byMcsSsnId
               u8 bySwitchMode 交换方式 SWITCH_MODE_BROADCAST SWITCH_MODE_SELECT
               BOOL32 bPolling  是否是轮询中选入SetIn
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/10/19  3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::OnMMcuSetIn(TMt tMt, u8 byMcsSsnId, u8 bySwitchMode)
{
	TSetInParam tSetInParam;
	tSetInParam.m_nViewId = -1145368303;
	tSetInParam.m_bySubFrameIndex = 0;
	tSetInParam.m_tMt = tMt;
	
	CServMsg  cMsg;
	TMsgHeadMsg tHeadMsg;
	TMcuMcuReq tReq;
	TMcsRegInfo	tMcsReg;
	TMtAlias tMtAlias;
	g_cMcuVcApp.GetMcsRegInfo( byMcsSsnId, &tMcsReg );
	astrncpy(tReq.m_szUserName, tMcsReg.m_achUser, 
		sizeof(tReq.m_szUserName), sizeof(tMcsReg.m_achUser));
	astrncpy(tReq.m_szUserPwd, tMcsReg.m_achPwd, 
		sizeof(tReq.m_szUserPwd), sizeof(tMcsReg.m_achPwd));


	tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tSetInParam.m_tMt );
	/*tHeadMsg.m_tMsgDst.m_tMt = tMt;
	u8 byFstMcuId = 0,bySecMcuId = 0;
	u8 abyMcuId[MAX_CASCADEDEPTH-1];
	memset( &abyMcuId[0],0,sizeof(abyMcuId) );
	if( !m_tConfAllMcuInfo.GetMcuIdByIdx( tMt.GetMcuIdx(),&abyMcuId[0] ) )
	{
		ConfLog( FALSE,"[OnMMcuSetIn] Fail to Get Mcuid.McuIdx.%d\n",tMt.GetMcuIdx() );
		return;
	}
	if( 0 != bySecMcuId )	
	{
		tHeadMsg.m_tMsgDst.m_tMt.SetMcuId( byFstMcuId );
		tHeadMsg.m_tMsgDst.m_tMt.SetMtId( bySecMcuId );
		tHeadMsg.m_tMsgDst.m_abyMtIdentify[0] = tMt.GetMtId();
		tHeadMsg.m_tMsgDst.m_byCasLevel = 1;
	}
	else
	{
		tHeadMsg.m_tMsgDst.m_tMt.SetMcuId( byFstMcuId );
		tHeadMsg.m_tMsgDst.m_byCasLevel = 0;
	}*/
	
	
	cMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));					
	cMsg.CatMsgBody((u8 *)&tSetInParam, sizeof(tSetInParam));
	cMsg.CatMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );

	u8 byFstMcuId = GetFstMcuIdFromMcuIdx( tMt.GetMcuIdx() );
	if( m_ptMtTable->GetDialAlias( byFstMcuId/*tMt.GetMcuId()*/, &tMtAlias ) && 
		mtAliasTypeH320ID == tMtAlias.m_AliasType )
	{
		//可能出现误判，如之前是广播源，广播源不变更的同时进行选看操作，状态被覆盖了
		//通知H320接入交换方式
		cMsg.CatMsgBody((u8 *)&bySwitchMode, sizeof(bySwitchMode));
	}	

	SendMsgToMt( byFstMcuId, MCU_MCU_SETIN_REQ, cMsg);
    
	TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMcuIdx());//tMt.GetMcuId());
	if (ptMcInfo != NULL)
	{
		// miaoqingsong [20111017] 无论VCS会议还是MCS会议一定要严格保证m_tLastMMcuViewMt和m_tMMcuViewMt不同
		if( !(ptMcInfo->m_tMMcuViewMt == tMt) )
		{
			ptMcInfo->m_tLastMMcuViewMt = ptMcInfo->m_tMMcuViewMt;
			ptMcInfo->m_tMMcuViewMt = tMt;
		}

		//zjj20091218 vcs会议严格保证m_tLastMMcuViewMt和m_tMMcuViewMt不同
// 		if( VCS_CONF == m_tConf.GetConfSource() )
// 		{
// 			if( !(ptMcInfo->m_tMMcuViewMt == tMt) )
// 			{
// 				ptMcInfo->m_tLastMMcuViewMt = ptMcInfo->m_tMMcuViewMt;
// 				ptMcInfo->m_tMMcuViewMt = tMt;
// 			}
// 		}
// 		else
// 		{
// 			ptMcInfo->m_tLastMMcuViewMt = ptMcInfo->m_tMMcuViewMt;
// 			ptMcInfo->m_tMMcuViewMt = tMt;
// 		}	
	}

	//zhouyiliang 20101026 多级单回传情况，中间的mcu的viewmt也要变
	//目前只处理了3级的情况，3级以上要用for或者while循环
	if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx( tMt.GetMcuIdx() ) )
	{
		u8 byMcuId[MAX_CASCADEDEPTH-1];
		u16 wMcuIdx = GetMcuIdxFromMcuId( byFstMcuId );
		ptMcInfo = m_ptConfOtherMcTable->GetMcInfo( wMcuIdx );
		if( m_tConfAllMcuInfo.GetMcuIdByIdx( tMt.GetMcuIdx(),&byMcuId[0] ) &&
			0 != byMcuId[1]
			)
		{
			if (ptMcInfo != NULL)
			{
				// [10/19/2011 liuxu] 上一个上传终端也需要补啊, 竟然没有会议索引
				ptMcInfo->m_tMMcuViewMt.SetConfIdx(m_byConfIdx);
				ptMcInfo->m_tLastMMcuViewMt = ptMcInfo->m_tMMcuViewMt;
		
				ptMcInfo->m_tMMcuViewMt.SetMcuId( wMcuIdx );
				ptMcInfo->m_tMMcuViewMt.SetMtId( byMcuId[1] );

				// [10/19/2011 liuxu] 很痛恨的事情, 在这里竟然把mcu的上传终端改变了
				// 导致在ChangeHduSwitch/ChangeTvwallSwitch/PrecxxxSetoutNotify中此mcu的
				// 上一个上传源的电视墙标志不能正确被刷新. 所以这里打一个补丁
				//RefreshMtStatusInTw(ptMcInfo->m_tLastMMcuViewMt, FALSE, TRUE);
				//RefreshMtStatusInTw(ptMcInfo->m_tLastMMcuViewMt, FALSE, FALSE);
			}
		}
	}

    // guzh [8/31/2006] 判断下级终端在本级发言的情况
    // guzh [9/1/2006]  Polling时不切换发言人，切换发言人会导致轮询停止，后面自然会切换
	//zhouyiliang 20120720 在setoutnotify中会做，此处不必提前做
//     TMt tSpeaker = m_tConf.GetSpeaker();
// 	TMt tMcu = GetLocalMtFromOtherMcuMt(tMt);
//     if ( tSpeaker.GetType() == TYPE_MT &&
//          (tMt.GetMcuIdx() != tSpeaker.GetMcuIdx() ||
//          tMt.GetMtId() != tSpeaker.GetMtId()) &&
// 		 IsMtInMcu(tMcu, tSpeaker) &&
//          !bPolling )
//     {
//         ChangeSpeaker(&tMt);
//     }    
	
	return;
}

/*====================================================================
    函数名      ：OnStartMixToSMcu
    功能        ：请求级联MCU开始混音
    算法实现    ：
    引用全局变量：
    输入参数说明：CServMsg *pcSerMsg 消息字段
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	05/03/7		3.6			Jason          创建
====================================================================*/
void CMcuVcInst::OnStartMixToSMcu(CServMsg *pcSerMsg)
{
	TMcu * ptMcu = (TMcu *)(pcSerMsg->GetMsgBody());
	if(ptMcu == NULL || ptMcu->IsLocal())
	{
		return;
	}
	//TMt tMcuMt = m_ptMtTable->GetMt( GetFstMcuIdFromMcuIdx(ptMcu->GetMcuId()) );
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(ptMcu->GetMcuId()));
	//TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(ptMcu->GetMcuId());
	if(ptMcInfo == NULL)
	{
		return;
	}
	u16 wMMcuIdx = INVALID_MCUIDX;
	if( !m_tCascadeMMCU.IsNull() )
	{
		wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
		//m_tConfAllMcuInfo.GetIdxByMcuId( m_tCascadeMMCU.GetMtId(),0,&wMMcuIdx );
	}
	
	if( ( INVALID_MCUIDX == ptMcInfo->GetMcuIdx() ) || 
		( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->GetMcuIdx() == wMMcuIdx ) )
	{
		return;
	}
	
	CServMsg cMsg;
	cMsg.SetServMsg(pcSerMsg->GetServMsg(), pcSerMsg->GetServMsgLen());
	cMsg.SetEventId(MCU_MCU_STARTMIXER_CMD);
	cMsg.SetDstMtId(ptMcu->GetMcuId() );
	
	SendMsgToMt(ptMcu->GetMcuId(), MCU_MCU_STARTMIXER_CMD, cMsg);

	return;
}

/*====================================================================
    函数名      ：OnStopMixToSMcu
    功能        ：请求级联MCU停止混音
    算法实现    ：
    引用全局变量：
    输入参数说明：CServMsg *pcSerMsg 消息字段
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	05/03/7		3.6			Jason          创建
====================================================================*/
void CMcuVcInst::OnStopMixToSMcu(CServMsg *pcSerMsg)
{
	TMcu * ptMcu = (TMcu *)(pcSerMsg->GetMsgBody());
	if(ptMcu == NULL || ptMcu->IsMcuIdLocal())
	{
		return;
	}

	u16 wMcuIdx = GetMcuIdxFromMcuId( (u8)ptMcu->GetMcuId() );
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
	if(ptMcInfo == NULL)
	{
		return;
	}
	u16 wMMcuIdx = INVALID_MCUIDX;
	if( !m_tCascadeMMCU.IsNull() )
	{
		wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
		//m_tConfAllMcuInfo.GetIdxByMcuId( m_tCascadeMMCU.GetMtId(),0,&wMMcuIdx );
	}

	if( ( INVALID_MCUIDX == ptMcInfo->GetMcuIdx() ) || 
		( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->GetMcuIdx() == wMMcuIdx ) )
	{
		return;
	}
	TMt tMt = m_ptMtTable->GetMt(u8(ptMcu->GetMcuId()));
	RemoveSpecMixMember(&tMt, 1, FALSE, FALSE);
	CServMsg cMsg;
	cMsg.SetServMsg(pcSerMsg->GetServMsg(), pcSerMsg->GetServMsgLen());
	cMsg.SetEventId(MCU_MCU_STOPMIXER_CMD);
	cMsg.SetDstMtId( (u8)ptMcu->GetMcuId() );
	
	SendMsgToMt( (u8)ptMcu->GetMcuId(), MCU_MCU_STOPMIXER_CMD, cMsg);

	return;
}

/*=============================================================================
    函 数 名： OnStartDiscussToAllSMcu
    功    能： 请求开启所有直联下级MCU全体混音
    算法实现： 
    全局变量： 
    参    数： CServMsg *pcSerMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/28   3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::OnStartDiscussToAllSMcu(CServMsg *pcSerMsg)
{
	CServMsg cMsg;
	cMsg.SetServMsg(pcSerMsg->GetServMsg(), pcSerMsg->GetServMsgLen());
	cMsg.SetEventId(MCU_MCU_STARTMIXER_CMD);
	
	//u16 wMMcuIdx = INVALID_MCUIDX;
	if( !m_tCascadeMMCU.IsNull() )
	{
		//wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
		//m_tConfAllMcuInfo.GetIdxByMcuId( m_tCascadeMMCU.GetMtId(),0,&wMMcuIdx );
	}

	//TConfMcInfo* ptMcInfo = NULL;
	for( u8 byLoop = 0; byLoop < MAXNUM_CONF_MT; byLoop++ )
	{
		if( MT_TYPE_SMCU != m_ptMtTable->GetMtType(byLoop) )
		{
			continue;
		}
		//ptMcInfo = &(m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]);
		if( !m_tCascadeMMCU.IsNull() &&  byLoop == m_tCascadeMMCU.GetMtId() ) 
		{
			continue;
		}
		cMsg.SetDstMtId( byLoop );
		SendMsgToMt(byLoop, MCU_MCU_STARTMIXER_CMD, cMsg);
	}
	
	return;
}

/*=============================================================================
    函 数 名： OnStopDiscussToAllSMcu
    功    能： 请求停止所有直联下级MCU全体混音
    算法实现： 
    全局变量： 
    参    数： CServMsg *pcSerMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/28   3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::OnStopDiscussToAllSMcu(CServMsg *pcSerMsg)
{
	CServMsg cMsg;
	cMsg.SetServMsg(pcSerMsg->GetServMsg(), pcSerMsg->GetServMsgLen());
	cMsg.SetEventId(MCU_MCU_STOPMIXER_CMD);
	//u16 wMMcuIdx = INVALID_MCUIDX;
	if( !m_tCascadeMMCU.IsNull() )
	{
		//wMMcuIdx = GetMcuIdxFromMcuId( m_tCascadeMMCU.GetMtId() );
		//m_tConfAllMcuInfo.GetIdxByMcuId( m_tCascadeMMCU.GetMtId(),0,&wMMcuIdx );
	}
	
	/*TConfMcInfo* ptMcInfo = NULL;
	for( u8 byLoop = 1; byLoop < MAXNUM_SUB_MCU; byLoop++ )
	{
		ptMcInfo = &(m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]);
		if( ( INVALID_MCUIDX == ptMcInfo->GetMcuIdx() ) || 
			( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->GetMcuIdx() == wMMcuIdx ) )
		{
			continue;
		}
		cMsg.SetDstMtId( GetFstMcuIdFromMcuIdx(ptMcInfo->GetMcuIdx()) );
		SendMsgToMt( cMsg.GetDstMtId(), MCU_MCU_STOPMIXER_CMD, cMsg);
	}*/
	//TConfMcInfo* ptMcInfo = NULL;
	for( u8 byLoop = 0; byLoop < MAXNUM_CONF_MT; byLoop++ )
	{
		if( MT_TYPE_SMCU != m_ptMtTable->GetMtType(byLoop) )
		{
			continue;
		}
		//ptMcInfo = &(m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]);
		if( !m_tCascadeMMCU.IsNull() &&  byLoop == m_tCascadeMMCU.GetMtId() ) 
		{
			continue;
		}
		cMsg.SetDstMtId( byLoop );
		SendMsgToMt(byLoop, MCU_MCU_STOPMIXER_CMD, cMsg);
	}
	
	return;
}

/*====================================================================
    函数名      ：OnGetMixParamToSMcu
    功能        ：请求级联MCU混音参数
    算法实现    ：
    引用全局变量：
    输入参数说明：CServMsg *pcSerMsg 消息字段
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	05/03/7		3.6			Jason          创建
====================================================================*/
void CMcuVcInst::OnGetMixParamToSMcu(CServMsg *pcSerMsg)
{
	TMcu * ptMcu = (TMcu *)(pcSerMsg->GetMsgBody());
	if(ptMcu == NULL ||ptMcu->IsLocal())
	{
		return;
	}

	CServMsg cMsg;
	cMsg.SetServMsg(pcSerMsg->GetServMsg(), pcSerMsg->GetServMsgLen());
	cMsg.SetEventId(MCU_MCU_GETMIXERPARAM_REQ);
	
	if( INVALID_MCUIDX == ptMcu->GetMcuIdx() )
	{
		//TConfMcInfo* ptMcInfo = NULL;
		for( u8 byLoop = 1; byLoop < MAXNUM_CONF_MT; byLoop++ )
		{
			if( MT_TYPE_SMCU != m_ptMtTable->GetMtType(byLoop) )
			{
				continue;
			}
			//ptMcInfo = &(m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]);
			if( !m_tCascadeMMCU.IsNull() &&  byLoop == m_tCascadeMMCU.GetMtId()  )
			{
				continue;
			}
			cMsg.SetDstMtId( byLoop );
			SendMsgToMt( byLoop, MCU_MCU_GETMIXERPARAM_REQ, cMsg );
		}
	}
	else
	{
		cMsg.SetDstMtId( GetFstMcuIdFromMcuIdx(ptMcu->GetMcuIdx()) );
		SendMsgToMt( cMsg.GetDstMtId(), MCU_MCU_GETMIXERPARAM_REQ, cMsg);
	}

	return;
}

/*=============================================================================
    函 数 名： OnAddRemoveMixToSMcu
    功    能： 添加移除级联mcu混音成员
    算法实现： 
    全局变量： 
    参    数： CServMsg *pcSerMsg
               BOOL32 bAdd
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/6/2    3.6			万春雷                  创建
=============================================================================*/
void CMcuVcInst::OnAddRemoveMixToSMcu(CServMsg *pcSerMsg, BOOL32 bAdd, BOOL32 bStopMixerNoMember/* = TRUE */)
{
	TMcu *ptMcu = (TMcu *)(pcSerMsg->GetMsgBody());
	if(ptMcu == NULL)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[OnAddRemoveMixToSMcu] ptMcu == NULL! So Return!\n");
		return;
	}
	if(ptMcu->IsLocal())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[OnAddRemoveMixToSMcu] The MCU is local! So Return! bStopMixerNoMember = %d\n",bStopMixerNoMember);
		return;
	}
    
	u8 abyMcuId[ MAX_CASCADEDEPTH - 1 ];
	memset(&abyMcuId[0], 0, sizeof(abyMcuId));

	if( !m_tConfAllMcuInfo.GetMcuIdByIdx(ptMcu->GetMcuIdx(), &abyMcuId[0]) &&
		!m_tConfAllMcuInfo.IsSMcuByMcuIdx(ptMcu->GetMcuIdx())
		)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER,  "[OnAddRemoveMixToSMcu] Fail to Get McuId By McuIdx.%d!\n",
			ptMcu->GetMcuIdx());
		return;
	}

	TMt tMcuMt = m_ptMtTable->GetMt(abyMcuId[0]);
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(ptMcu->GetMcuIdx());
	if(ptMcInfo == NULL)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[OnAddRemoveMixToSMcu] ptMcInfo == NULL so return!\n");
		return;
	}

	if( (INVALID_MCUIDX == ptMcInfo->GetMcuIdx()) || 
		(!m_tCascadeMMCU.IsNull() && abyMcuId[0] == m_tCascadeMMCU.GetMtId() ) )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[OnAddRemoveMixToSMcu] INVALID_MCUIDX == ptMcInfo->GetMcuIdx() so return!\n");
		return;
	}

	TMt *ptMt = (TMt *)(pcSerMsg->GetMsgBody() + sizeof(TMcu));
	TMt tTempMt;	
	CServMsg cMsg;
	
	if (ptMt == NULL)
	{
		ConfPrint( LOG_LVL_ERROR, MID_MCU_MIXER, "[OnAddRemoveMixToSMcu] The add/remove ptMt is NULL, so return!\n" );
		return;
	}

	cMsg.SetServMsg(pcSerMsg->GetServMsg(), pcSerMsg->GetServMsgLen());
	u8  byMtNum = (pcSerMsg->GetMsgBodyLen() - sizeof(TMcu) - sizeof(u8))/sizeof(TMt);

	//u8  byReplace = 0;
	u8  byNotForceRemove = 0;
	if (bAdd)
	{
		cMsg.SetMsgBody((u8*)&tTempMt, sizeof(TMcu));
		//byReplace = *(u8*)(pcSerMsg->GetMsgBody() + sizeof(TMt) * byMtNum + sizeof(TMcu));

		u8  bySecMcuLocalMtNum = 0;     //第二级终端数
		TMt atSecMcuLocalMt[MAXNUM_MIXING_MEMBER];
		u8  byThdMcuNum = 0;
		TMt atThdMcuMt[MAXNUM_SUB_MCU];
		u8  abyThdMcuMtNum[MAXNUM_SUB_MCU] = { 0 };    //第三级MCU个数
		TMultiCacMtInfo tMultiCacMtInfo[MAXNUM_SUB_MCU][MAXNUM_MIXING_MEMBER];
		TMultiCacMtInfo tTempMultiCacMtInfo;

		for ( u8 byIndex = 0; byIndex < byMtNum; byIndex ++ )
		{
			if (ptMt == NULL)
			{
				return;
			}

			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[OnAddRemoveMixToSMcu]Add tMt[%d][%d]\n", 
					ptMt->GetMcuId(), ptMt->GetMtId() );
			tTempMultiCacMtInfo = BuildMultiCascadeMtInfo(*ptMt, tTempMt);
			if ( tTempMultiCacMtInfo.m_byCasLevel == 0 )
			{
				for ( u8 byIdx1 = 0; byIdx1 < MAXNUM_MIXING_MEMBER; byIdx1++ )
				{
					if (atSecMcuLocalMt[byIdx1].IsNull())
					{
						atSecMcuLocalMt[byIdx1] = tTempMt;
						bySecMcuLocalMtNum++;
						break;
					}
				}
			}
			else
			{
				for ( u8 byIdx = 0; byIdx < MAXNUM_SUB_MCU; byIdx++ )
				{
					if (atThdMcuMt[byIdx] == tTempMt)
					{
						for ( u8 byIdx1 = 0; byIdx1 < MAXNUM_MIXING_MEMBER; byIdx1++ )
						{
							if (tMultiCacMtInfo[byIdx][byIdx1].IsNull())
							{
								abyThdMcuMtNum[byIdx]++;
								tMultiCacMtInfo[byIdx][byIdx1] = tTempMultiCacMtInfo;
								break;
							}
						}
						break;
					}

					if (atThdMcuMt[byIdx].IsNull())
					{
						byThdMcuNum++;
						abyThdMcuMtNum[byIdx]++;
						atThdMcuMt[byIdx] = tTempMt;
						tMultiCacMtInfo[byIdx][0] = tTempMultiCacMtInfo;
						break;
					}
				}
			}

			ptMt++;
		}

		cMsg.SetMsgBody( (u8*)&bySecMcuLocalMtNum, sizeof(u8) );
		for ( u8 byIdx = 0; byIdx < bySecMcuLocalMtNum; byIdx++ )
		{
			cMsg.CatMsgBody( (u8*)&atSecMcuLocalMt[byIdx], sizeof(TMt) );
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[OnAddRemoveMixToSMcu]Add Idx(%d) tMt[%d][%d]\n",byIdx, 
					atSecMcuLocalMt[byIdx].GetMcuId(), atSecMcuLocalMt[byIdx].GetMtId() );
		}

		cMsg.CatMsgBody( &byThdMcuNum, sizeof(u8) );
		for ( u8 byThdIndex = 0; byThdIndex < byThdMcuNum; byThdIndex++ )
		{
			cMsg.CatMsgBody( (u8*)&atThdMcuMt[byThdIndex], sizeof(TMt) );
			cMsg.CatMsgBody( (u8*)&abyThdMcuMtNum[byThdIndex], sizeof(u8) );
			
			for ( u8 byMtIndex = 0; byMtIndex < abyThdMcuMtNum[byThdIndex]; byMtIndex++ )
			{
				cMsg.CatMsgBody( (u8*)&tMultiCacMtInfo[byThdIndex][byMtIndex], sizeof(TMultiCacMtInfo) );
			}
		}

		u8 byReplace2 = *(u8*)(pcSerMsg->GetMsgBody() + sizeof(TMt) * byMtNum + sizeof(TMcu));
		cMsg.CatMsgBody((u8*)&byReplace2, sizeof(u8));
		
		// zjj20100428 只有在添加下级终端进混音时，若下级mcu不在混音组时才将它加入混音
		if ( !m_ptMtTable->IsMtInMixing(tMcuMt.GetMtId()) )
		{
			AddSpecMixMember(&tMcuMt, 1, TRUE);
		}
	}
	else
	{
		TMt tCurMt = m_cVCSConfStatus.GetCurVCMT();
		if( VCS_CONF == m_tConf.GetConfSource() && ptMt != NULL && (ptMt->GetMcuId() == tCurMt.GetMcuId() && ptMt->GetMtId() == tCurMt.GetMtId()) )
		{
			ConfPrint( LOG_LVL_ERROR, MID_MCU_MIXER, "[OnAddRemoveMixToSMcu] TMT(%d, %d) IS CURRENT VCS MT, SO CAN'T REMOVE!!\n", 
				ptMt->GetMcuId(),ptMt->GetMtId() );

			return;
		}

		TMsgHeadMsg tMsgHeadMsg;
		tMsgHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo(*ptMt, tTempMt);
		byNotForceRemove = *(u8*)(pcSerMsg->GetMsgBody() + sizeof(TMcu) + sizeof(TMt) * byMtNum );
		cMsg.SetMsgBody((u8*)&tTempMt,sizeof(TMcu));
		if( tMsgHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
		{
			ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[OnAddRemoveMixToSMcu]Remove level = [%d] tMt[%d][%d]\n",
				tMsgHeadMsg.m_tMsgDst.m_byCasLevel, ptMt->GetMcuId(), ptMt->GetMtId());

			cMsg.CatMsgBody((u8*)ptMt, sizeof(TMt) * byMtNum);
		}
		else
		{
			for (u8 byIndex = 0 ;byIndex < byMtNum ;byIndex ++)
			{
				if (ptMt == NULL)
				{
					return;
				}

				BuildMultiCascadeMtInfo( *ptMt, tTempMt );
				cMsg.CatMsgBody( (u8*)&tTempMt, sizeof(TMt) );
				ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[OnAddRemoveMixToSMcu]Remove level = [%d] tMt[%d][%d]\n", 
					tMsgHeadMsg.m_tMsgDst.m_byCasLevel, tTempMt.GetMcuId(), tTempMt.GetMtId() );
				ptMt++;
			}
		}
		cMsg.CatMsgBody((u8*)&byNotForceRemove, sizeof(u8));
		cMsg.CatMsgBody((u8*)&tMsgHeadMsg, sizeof(TMsgHeadMsg));
	}
	
	cMsg.SetDstMtId( tMcuMt.GetMtId() );
	if(bAdd)
	{
		cMsg.SetEventId( MCU_MCU_ADDMIXMEMBER_CMD );	
		SendMsgToMt(tMcuMt.GetMtId(), MCU_MCU_ADDMIXMEMBER_CMD, cMsg);
	}
	else
	{
		cMsg.SetEventId( MCU_MCU_REMOVEMIXMEMBER_CMD );	
		SendMsgToMt(tMcuMt.GetMtId(), MCU_MCU_REMOVEMIXMEMBER_CMD, cMsg);
	}
	return;
}

/*==============================================================================
函数名    :  GetMcuMultiSpyBW
功能      :  获得下级mcu的回传最大带宽和剩余带宽
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010-08-06                 周晶晶							create
==============================================================================*/
BOOL32 CMcuVcInst::GetMcuMultiSpyBW( u16 wMcuIdx,u32 &dwMaxSpyBW, s32 &nRemainSpyBW )
{
	if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) )
	{
		u8 byMcuId = GetFstMcuIdFromMcuIdx( wMcuIdx );
		wMcuIdx = GetMcuIdxFromMcuId( &byMcuId );
	}

	return m_ptConfOtherMcTable->GetMultiSpyBW(wMcuIdx, dwMaxSpyBW, nRemainSpyBW);
}

/*==============================================================================
函数名    :  SetMcuSupMultSpyRemainBW
功能      :  设置某个下级mcu的剩余回传带宽
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010-08-06                 周晶晶							create
==============================================================================*/
BOOL32 CMcuVcInst::SetMcuSupMultSpyRemainBW( u16 wMcuIdx, s32 nRemainSpyBW )
{
	if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) )
	{
		u8 byMcuId = GetFstMcuIdFromMcuIdx( wMcuIdx );
		wMcuIdx = GetMcuIdxFromMcuId( &byMcuId );
	}

	return m_ptConfOtherMcTable->SetMcuSupMultSpyRemainBW(wMcuIdx, nRemainSpyBW );
}


/*==============================================================================
函数名    :  IsLocalAndSMcuSupMultSpy
功能      :  判断本地及下级mcu是否都支持多回传
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010-06-07                 lukunpeng						create
2010-08-06                 周晶晶							修改(增加判断是否为直属下级mcu)
==============================================================================*/
BOOL32 CMcuVcInst::IsLocalAndSMcuSupMultSpy( u16 wMcuIdx )
{
	if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) )
	{
		u8 byMcuId = GetFstMcuIdFromMcuIdx( wMcuIdx );
		wMcuIdx = GetMcuIdxFromMcuId( &byMcuId );
	}
	// [2010/12/30 miaoqingsong] 判断会议和mcu是否支持多回传
	return  IsSupportMultiSpy() && m_ptConfOtherMcTable->GetIsMcuSupMultSpy( wMcuIdx ); 
}

/*==============================================================================
函数名    :  IsSupportMultCas
功能      :  判断mcu是否都支持多级联
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010-07-09                 xueliang						   create
==============================================================================*/
BOOL32 CMcuVcInst::IsSupportMultCas( u16 wMcuIdx )
{
	if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) )
	{
		u8 byMcuId = GetFstMcuIdFromMcuIdx( wMcuIdx );
		wMcuIdx = GetMcuIdxFromMcuId( &byMcuId );
	}	
	return m_ptConfOtherMcTable->IsMcuSupMultCas(wMcuIdx);
}

/*==============================================================================
函数名    :  IsSupportMultCas
功能      :  判断上级mcu是否支持多级联
算法实现  :  
参数说明  :  
返回值说明:  是否支持多级联
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010-08-17                 周晶晶						   创建
==============================================================================*/
BOOL32 CMcuVcInst::IsMMcuSupportMultiCascade( void )
{
	if( m_tCascadeMMCU.IsNull() )
	{
		return FALSE;
	}
	u8 byMcuId = m_tCascadeMMCU.GetMtId();
	u16 wMcuIdx = GetMcuIdxFromMcuId( &byMcuId );
	if( INVALID_MCUIDX == wMcuIdx )
	{
		return FALSE;
	}
	return m_ptConfOtherMcTable->IsMcuSupMultCas(wMcuIdx);
}

/*==============================================================================
函数名    :  IsPreSetInRequired
功能      :  判断是否需要preSetIn
算法实现  :  
参数说明  :  [IN] tMt 下级某终端
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010-02-22                 薛亮							  create
==============================================================================*/
BOOL32 CMcuVcInst::IsPreSetInRequired (const TMt &tMt)
{
	BOOL32 bPreSetInRqrd = TRUE;
	
	//1，判该MT是否已经在某回传通道中了
	if( m_cSMcuSpyMana.IsMtInSpyMember(tMt, MODE_VIDEO) )
	{
		bPreSetInRqrd = FALSE;
	}
	
	return bPreSetInRqrd;
	
}

/*==============================================================================
函数名    :  GetMtSimCapSetByMode
功能      :  得到本级终端的音视频能力
算法实现  :  
参数说明  :  [IN] u8 byMtId 某终端id
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
20100911                   pengjie						   create
==============================================================================*/
TSimCapSet CMcuVcInst::GetMtSimCapSetByMode( u8 byMtId )
{
	TLogicalChannel tSrcLogicChl;
	TSimCapSet tSimCapSet;
	tSimCapSet.Clear();
	
	// 视频
	if( m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_VIDEO, &tSrcLogicChl, FALSE ) )
	{
		tSimCapSet.SetVideoMediaType( tSrcLogicChl.GetChannelType() );
		tSimCapSet.SetVideoMaxBitRate( m_ptMtTable->GetSndBandWidth( byMtId) );
		tSimCapSet.SetVideoResolution( tSrcLogicChl.GetVideoFormat() );
		tSimCapSet.SetVideoProfileType(tSrcLogicChl.GetProfileAttrb() );
		tSimCapSet.SetVideoProfileType(tSrcLogicChl.GetProfileAttrb());
		if( MEDIA_TYPE_H264 == tSrcLogicChl.GetChannelType() )
		{
			tSimCapSet.SetUserDefFrameRate( tSrcLogicChl.GetChanVidFPS() );
		}
		else
		{
			tSimCapSet.SetVideoFrameRate( tSrcLogicChl.GetChanVidFPS() );
		}
	}
	
	// 音频	
	if( m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_AUDIO, &tSrcLogicChl, FALSE ) )
	{
		tSimCapSet.SetAudioMediaType( tSrcLogicChl.GetChannelType() );
	}
	
	return tSimCapSet;
}

/*==============================================================================
函数名    :  GetMinSpyDstCapSet
功能      :  当前业务所需回传能力集与该回传终端目的能力集取小
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
20101117                  周嘉麟						   create
==============================================================================*/
BOOL32 CMcuVcInst::GetMinSpyDstCapSet(const TMt &tMt , TSimCapSet &tReqDstCap )
{
	if (tMt.IsNull() || tReqDstCap.IsNull())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_MMCU, "[GetMinSpyDstCapSet] param is invalid!\n");
		return FALSE;
	}
	CRecvSpy tSrcSpy;
	TSimCapSet tSpyDstCap;
	if (m_cSMcuSpyMana.GetRecvSpy(tMt, tSrcSpy))
	{
		tSpyDstCap = tSrcSpy.GetSimCapset();
		if (!tSpyDstCap.IsNull() && tSpyDstCap.GetVideoMediaType() == tReqDstCap.GetVideoMediaType())
		{
			u8 byMinRes = GetMinResAcdWHProduct(tReqDstCap.GetVideoResolution(), 
												tSpyDstCap.GetVideoResolution());
			if (VIDEO_FORMAT_INVALID != byMinRes)
			{
				tReqDstCap.SetVideoResolution(byMinRes);
			}		
			
			if (tSpyDstCap.GetVideoMaxBitRate() < tReqDstCap.GetVideoMaxBitRate())
			{
				tReqDstCap.SetVideoMaxBitRate(tSpyDstCap.GetVideoMaxBitRate());
			}
		}
	}	
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[GetMinSpyDstCapSet] tReqDstCap<Media:%d, Res:%d, BR:%d>!\n",
		tReqDstCap.GetVideoMediaType(), tReqDstCap.GetVideoResolution(), tReqDstCap.GetVideoMaxBitRate());
	
	return !tReqDstCap.IsNull();
}

/*==============================================================================
函数名    :  UpdateCurSpyDstCapSet
功能      :  根据上级业务变化更新当前回传终端目的能力集(上级保存)
			 
算法实现  :  取级联通道能力和本级业务所要求回传能力取小保存，
			 目前只判断了该终端是否在画面合成和是否在dec5里
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
20101117                  周嘉麟						   create
==============================================================================*/
void CMcuVcInst::UpdateCurSpyDstCapSet(const TMt &tMt )
{
	if (tMt.IsNull() || tMt.IsLocal())
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[UpdateCurSpyDstCapSet] param is invalid!\n");
		return;
	}

	//取级联通道能力集 vmp能力集 Dec5能力集 分辨率取小
	TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tMt.GetMcuId()) );
	if (tSimCapSet.IsNull())
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[UpdateCurSpyDstCapSet]get simcaptset faied!\n");
		return;
	}
	//vmp
	TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
	if( (tVmpParam.IsVMPBrdst() && tVmpParam.IsMtInMember(tMt)) )
	{
		CVmpChgFormatResult cVmpChgResult;
		u8 byMtInVmpRes = 0;
		u8 byVmpSubType = GetVmpSubType(m_tVmpEqp.GetEqpId());
		
		if(byVmpSubType == VMP)
		{
			GetMtFormatInSdVmp(tMt, &tVmpParam, byMtInVmpRes, TRUE);
		}
		else
		{
			GetMtFormatInMpu(tMt, &tVmpParam, byMtInVmpRes, cVmpChgResult, TRUE, TRUE, FALSE);
		}	

		const u8 byRealRes = tSimCapSet.GetVideoResolution();
		tSimCapSet.SetVideoResolution(min(byRealRes, byMtInVmpRes));
	}
	
	//dec5
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMcuId());		
	if (ptMcInfo != NULL)
	{
		TMcMtStatus *ptMcMtStatus = ptMcInfo->GetMtStatus(tMt);
		if (ptMcMtStatus != NULL && ptMcMtStatus->GetMtStatus().IsInTvWall())
		{
			const u8 byRealRes = tSimCapSet.GetVideoResolution();
			tSimCapSet.SetVideoResolution(min(byRealRes, VIDEO_FORMAT_CIF));
		}			
	}
	m_cSMcuSpyMana.SaveSpySimCap(tMt, tSimCapSet);
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[UpdateCurSpyDstCapSet] CurrentSpyCap<Media:%d, Res:%d, BR:%d>!\n",
					tSimCapSet.GetVideoMediaType(), tSimCapSet.GetVideoResolution(), tSimCapSet.GetVideoMaxBitRate());
	return;
}
/*==============================================================================
函数名    :  IsCanAddSpyMtDstNum
功能      :  是否可以增加回传终端的目的数
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-9-28                   薛亮							create
2009-11-01                   pengjie                         modify
==============================================================================*/
/*
BOOL32 CMcuVcInst::IsCanAddSpyMtDstNum( TMt tSrcSpyMt,TPreSetInRsp &tPreSetInRsp )
{
	
	switch( tPreSetInRsp.m_dwEvId )
	{
	case MT_MCU_STARTSELMT_CMD:			 //标准选看命令
	case MT_MCU_STARTSELMT_REQ:          
	case MCS_MCU_STARTSWITCHMT_REQ:	     //控制台要求交换命令
	case VCS_MCU_CONFSELMT_CMD:
		{
			TMtStatus tDstMtStatus;
			TMt tSrcMt;
			memset( &tDstMtStatus,0,sizeof( tDstMtStatus ) );
			if( m_ptMtTable->GetMtStatus( tPreSetInRsp.m_tDest.GetMtId(),&tDstMtStatus ) )
			{
				if( MODE_BOTH == tPreSetInRsp.m_byRspSpyMode || MODE_VIDEO == tPreSetInRsp.m_byRspSpyMode )
				{
					tSrcMt = tDstMtStatus.GetSelectMt( MODE_VIDEO );
					if( tSrcMt == tPreSetInRsp.m_tSrc )
					{
						return FALSE;
					}
				}
				if( MODE_BOTH == tPreSetInRsp.m_byRspSpyMode || MODE_AUDIO == tPreSetInRsp.m_byRspSpyMode )
				{
					tSrcMt = tDstMtStatus.GetSelectMt( MODE_AUDIO );
					if( tSrcMt == tPreSetInRsp.m_tSrc )
					{
						return FALSE;
					}
				}
			}
		}
		break;
	case MCS_MCU_STARTSWITCHMC_REQ:      //会控选看终端
		
		break;
	
	case MCS_MCU_STARTVMP_REQ:
		//zjj这里返回false，统一在setvmpchannel中增加多回传终端的音视频目的数
		return FALSE;
		break;

	case MCS_MCU_SPECSPEAKER_REQ:        //会议控制台指定一台终端发言
	case MT_MCU_SPECSPEAKER_REQ:		 //普通终端指定发言人请求
	case MT_MCU_SPECSPEAKER_CMD:
		{
			TMt tSpeaker = m_tConf.GetSpeaker();
			if( tSpeaker == tSrcSpyMt )
			{
				return FALSE;
			}
		}	
		//ProcSpeakerStartPreSetInAck( tPreSetInRsp );
		break;

	case MT_MCU_ADDMIXMEMBER_CMD:           //主席增加混音终端
	case MCS_MCU_ADDMIXMEMBER_CMD:          //增加混音成员
			
		//ProcMixerStartPreSetInAck( tPreSetInRsp );
		break;

	case MCS_MCU_START_SWITCH_HDU_REQ:     // 会控选择进高清电视墙
	case MCUVC_STARTSWITCHHDU_NOTIFY:
//	case MCS_MCU_CHANGEHDUVOLUME_REQ:
		{
			TSpyHduInfo tSpyHduInfo = tPreSetInRsp.m_tSpyInfo.m_tSpyHduInfo;
			TPeriEqpStatus tHduStatus;   
			memset( &tHduStatus,0,sizeof( tHduStatus ) );
			
			g_cMcuVcApp.GetPeriEqpStatus(tPreSetInRsp.m_tDest.GetEqpId(), &tHduStatus);

			if( tHduStatus.m_tStatus.tHdu.atVideoMt[tSpyHduInfo.m_byDstChlIdx].GetMcuId() == tSrcSpyMt.GetMcuId() &&
				tHduStatus.m_tStatus.tHdu.atVideoMt[tSpyHduInfo.m_byDstChlIdx].GetMtId() == tSrcSpyMt.GetMtId() &&
				tHduStatus.m_tStatus.tHdu.atVideoMt[tSpyHduInfo.m_byDstChlIdx].GetConfIdx() == m_byConfIdx )
			{
				return FALSE;
			}			
		}
		//ProcHduStartPreSetInAck( tPreSetInRsp );
		break;

	case MCUVC_POLLING_CHANGE_TIMER:       // 处理轮询
		//ProcPollStartPreSetInAck( tPreSetInRsp );
		break;

	case MCS_MCU_START_SWITCH_TW_REQ:      // 会控选择进电视墙
		{		
			TSpyTVWallInfo tSpyTVWallInfo = tPreSetInRsp.m_tSpyInfo.m_tSpyTVWallInfo;	
			TPeriEqpStatus tTvWallStatus;   
			memset( &tTvWallStatus,0,sizeof( tTvWallStatus ) );
			
			g_cMcuVcApp.GetPeriEqpStatus(tPreSetInRsp.m_tDest.GetEqpId(), &tTvWallStatus);

			if( tTvWallStatus.m_tStatus.tTvWall.atVideoMt[tSpyTVWallInfo.m_byDstChlIdx].GetMcuId() == tSrcSpyMt.GetMcuId() &&
				tTvWallStatus.m_tStatus.tTvWall.atVideoMt[tSpyTVWallInfo.m_byDstChlIdx].GetMtId() == tSrcSpyMt.GetMtId() &&
				tTvWallStatus.m_tStatus.tTvWall.atVideoMt[tSpyTVWallInfo.m_byDstChlIdx].GetConfIdx() == m_byConfIdx )
			{
				return FALSE;
			}	
		}
		//ProcTWStartPreSetInAck( tPreSetInRsp );  
		break;

    case MCS_MCU_CHANGEROLLCALL_REQ:       // 会议点名逻辑
		{
			TMtStatus tMtStatus;
			memset( &tMtStatus,0,sizeof( tMtStatus ) );
			if(  ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() ||
				ROLLCALL_MODE_CALLER == m_tConf.m_tStatus.GetRollCallMode()
				)
			{
				m_ptMtTable->GetMtStatus( m_tRollCaller.GetMtId(),&tMtStatus );
				TMt tSrcSelMt = tMtStatus.GetSelectMt( MODE_VIDEO );
				if( tSrcSelMt == tSrcSpyMt )
				{
					return FALSE;
				}
			}
			else if( ROLLCALL_MODE_CALLEE == m_tConf.m_tStatus.GetRollCallMode() )
			{

			}
		}
		
		//m_tRollCaller
		//ProcRollCallPreSetInAck( tPreSetInRsp );
		break;

// 	case :
// 		break;
	default:
		break;
	}	
	return TRUE;
}

*/

/*==============================================================================
函数名    :  EvaluateSpyFromEvent
功能      :  根据各event和SetInRsp判断增加源的目的数和此消息可以释放多少带宽
算法实现  :  
参数说明  :  const TPreSetInRsp &tPreSetInRsp PreSetIn响应消息体
			 u8 &byAddDestSpyNum	回传源可增加的目的数
			 u32 &dwCanReleaseBW	此消息中逻辑可释放带宽数
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010-6-10                  lukunpeng						create
==============================================================================*/
void CMcuVcInst::EvaluateSpyFromEvent(const TPreSetInRsp &tPreSetInRsp, u8 &byAddDestSpyNum, u32 &dwCanReleaseBW, s16 &swCanRelIndex)
{
	byAddDestSpyNum = 0;
	dwCanReleaseBW = 0;
	swCanRelIndex = -1;

	TMt tCanReleaseMt;

	if( tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.GetCount() >= 0 )
	{
		TMultiCacMtInfo tMtInfo;
		tMtInfo.m_byCasLevel = tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCasLevel;
		memcpy( &tMtInfo.m_abyMtIdentify[0],
			&tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_abyMtIdentify[0],
			sizeof(tMtInfo.m_abyMtIdentify)
			);
		tCanReleaseMt = GetMtFromMultiCascadeMtInfo( tMtInfo,
						tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_tMt
						);
		m_cSMcuSpyMana.IsCanFree( tCanReleaseMt, 
					tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCanReleaseMode,
					tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum,
					tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum,
					dwCanReleaseBW, swCanRelIndex 
					);
		if( tCanReleaseMt ==  tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt() )
		{
			byAddDestSpyNum = 0;
		}
		else
		{
			byAddDestSpyNum = 1;
		}
		if( GetFstMcuIdFromMcuIdx( tCanReleaseMt.GetMcuId() ) != 
			GetFstMcuIdFromMcuIdx (tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMcuId() )
			)
		{
			dwCanReleaseBW = 0;
		}
		
	}

	TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
	if( 0 == dwCanReleaseBW && m_tCascadeMMCU.IsNull() )
	{
		switch( tPreSetInRsp.m_tSetInReqInfo.GetEvId() )
		{
			//zjj20101220 会议轮询,轮询跟随终端其实也是可释放终端,应该加入可释放带宽
			case MCUVC_POLLING_CHANGE_TIMER:
				if( tConfVmpParam.GetVMPMode() != CONF_VMPMODE_NONE )
				{
					u8 byChl = tConfVmpParam.GetChlOfMemberType( VMP_MEMBERTYPE_POLL );
					if( tConfVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL) &&
						MAXNUM_VMP_MEMBER != byChl )
					{
						TVMPMember *ptMember = tConfVmpParam.GetVmpMember( byChl );
						memcpy( &tCanReleaseMt,ptMember,sizeof(tCanReleaseMt) );
						if( GetFstMcuIdFromMcuIdx( tCanReleaseMt.GetMcuId() ) == 
								GetFstMcuIdFromMcuIdx (tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMcuId() )
							)
						{
							m_cSMcuSpyMana.IsCanFree( tCanReleaseMt, 
										MODE_VIDEO,
										1,
										0,
										dwCanReleaseBW, swCanRelIndex 
										);

							ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[EvaluateSpyFromEvent] McuId:%d MtId:%d is in vmp.Type Is VMP_MEMBERTYPE_POLL.So alse can release\n", 
									tCanReleaseMt.GetMcuId(),
									tCanReleaseMt.GetMtId()									
									);
						}				
					}
				}
				break;
			default:
				break;
		}
	}

	/*switch( tPreSetInRsp.m_tSetInReqInfo.m_dwEvId )
	{
	case MT_MCU_STARTSELMT_CMD:			 //标准选看命令
	case MT_MCU_STARTSELMT_REQ:          
	case MCS_MCU_STARTSWITCHMT_REQ:	     //控制台要求交换命令
	case VCS_MCU_CONFSELMT_CMD:
		{
			TMtStatus tDstMtStatus;
			TMt tSrcMt;
			memset(&tDstMtStatus, 0, sizeof( tDstMtStatus ));
			if( m_ptMtTable->GetMtStatus( tSpyInfo.m_tSpySwitchInfo.m_tDstMt.GetMtId(),&tDstMtStatus ) )
			{
				//如果是Both，那么音视频选看源是不是必须相等!如果不相等，就有逻辑错误
				if( MODE_BOTH == tPreSetInRsp.m_byRspSpyMode || MODE_VIDEO == tPreSetInRsp.m_byRspSpyMode )
				{
					tSrcMt = tDstMtStatus.GetSelectMt( MODE_VIDEO );
					if( tSrcMt == tPreSetInRsp.m_tSetInReqInfo.m_tSrc )
					{
						byAddDestSpyNum = 0;
					}
					else
					{
						byAddDestSpyNum = 1;
						m_cSMcuSpyMana.LeftOnceToFree(tSrcMt, MODE_VIDEO, dwCanReleaseBW, swCanRelIndex);
						//只有同一下级的mcu才判定预释放带宽
						if (tSrcMt.GetMcuId() != tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId())
						{
							dwCanReleaseBW = 0;
						}
					}
				}

				if( MODE_BOTH == tPreSetInRsp.m_byRspSpyMode || MODE_AUDIO == tPreSetInRsp.m_byRspSpyMode )
				{
					tSrcMt = tDstMtStatus.GetSelectMt( MODE_AUDIO );
					if( tSrcMt == tPreSetInRsp.m_tSetInReqInfo.m_tSrc )
					{
						byAddDestSpyNum = 0;
					}
					else
					{
						byAddDestSpyNum = 1;
						m_cSMcuSpyMana.LeftOnceToFree(tSrcMt, MODE_AUDIO, dwCanReleaseBW, swCanRelIndex);
						//只有同一下级的mcu才判定预释放带宽
						if (tSrcMt.GetMcuId() != tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId())
						{
							dwCanReleaseBW = 0;
						}
					}
				}
			}
		}
		break;

	case MCS_MCU_STARTSWITCHMC_REQ:      //会控选看终端
		{
			u8 byDstChnnl = tSpyInfo.m_tSpySwitchInfo.m_byDstChlIdx;
			CMcChnnlInfo cMcChnnlInfo;
			if( m_cMcChnnlMgr.GetMcChnnlInfo( byDstChnnl, cMcChnnlInfo ) )
			{
				TMt tMcSrc = cMcChnnlInfo.GetMcSrc();
				u8  byMode = cMcChnnlInfo.GetMcChnnlMode();
				m_cSMcuSpyMana.LeftOnceToFree(tMcSrc, byMode, dwCanReleaseBW, swCanRelIndex);
				//只有同一下级的mcu才判定预释放带宽
				if (tMcSrc.GetMcuId() != tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId())
				{
					dwCanReleaseBW = 0;
				}
			}
			byAddDestSpyNum = 1;
		}
		break;
	case MCS_MCU_STARTVMP_REQ:
		{
			if (MODE_BOTH == tPreSetInRsp.m_byRspSpyMode || MODE_VIDEO == tPreSetInRsp.m_byRspSpyMode)
			{
				if (m_tLastVmpParam.IsMtInMember(tPreSetInRsp.m_tSetInReqInfo.m_tSrc))
				{
					byAddDestSpyNum = 0;
				}
				else
				{
					u32 dwRelBW = dwCanReleaseBW;
					s16 swRelIndex = -1;

					u8 byIndex = tSpyInfo.m_tSpyVmpInfo.m_byPos;

					TVMPMember *ptVMPMember = m_tLastVmpParam.GetVmpMember(byIndex);

					if (ptVMPMember != NULL)
					{	
						m_cSMcuSpyMana.LeftOnceToFree(*(TMt *)ptVMPMember, MODE_VIDEO, dwCanReleaseBW, swCanRelIndex);
					}

					
					byAddDestSpyNum = 1;
				}
			}
		}
		break;

	case MCS_MCU_SPECSPEAKER_REQ:        //会议控制台指定一台终端发言
	case MT_MCU_SPECSPEAKER_REQ:		 //普通终端指定发言人请求
	case MT_MCU_SPECSPEAKER_CMD:
		{
			TMt tSpeaker = m_tConf.GetSpeaker();
			if( tSpeaker == tPreSetInRsp.m_tSetInReqInfo.m_tSrc )
			{
				byAddDestSpyNum = 0;
			}
			else
			{
				byAddDestSpyNum = 1;
				m_cSMcuSpyMana.LeftOnceToFree(tSpeaker, MODE_BOTH, dwCanReleaseBW, swCanRelIndex);
				//只有同一下级的mcu才判定预释放带宽
				if (tSpeaker.GetMcuId() != tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId())
				{
					dwCanReleaseBW = 0;
				}
			}
		}
		break;

	case MT_MCU_ADDMIXMEMBER_CMD:           //主席增加混音终端
	case MCS_MCU_ADDMIXMEMBER_CMD:          //增加混音成员
		break;

	case MCS_MCU_START_SWITCH_HDU_REQ:     // 会控选择进高清电视墙
	case MCUVC_STARTSWITCHHDU_NOTIFY:
		{
			TSpyHduInfo tSpyHduInfo = tSpyInfo.m_tSpyHduInfo;
			TPeriEqpStatus tHduStatus;   
			memset( &tHduStatus,0,sizeof( tHduStatus ) );
			
			g_cMcuVcApp.GetPeriEqpStatus(tSpyHduInfo.m_tHdu.GetEqpId(), &tHduStatus);

			TMt tOldMt = (TMt)tHduStatus.m_tStatus.tHdu.atVideoMt[tSpyHduInfo.m_byDstChlIdx];

			if( tOldMt.GetMcuId() == tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId()
				&& tOldMt.GetMtId() == tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMtId() )
			{
				byAddDestSpyNum = 0;
			}
			else
			{
				byAddDestSpyNum = 1;
				m_cSMcuSpyMana.LeftOnceToFree(tOldMt, MODE_BOTH, dwCanReleaseBW, swCanRelIndex);
				//只有同一下级的mcu才判定预释放带宽
				if (tOldMt.GetMcuId() != tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId())
				{
					dwCanReleaseBW = 0;
				}
			}
		}
		break;

	case MCUVC_POLLING_CHANGE_TIMER:       // 处理轮询
		{
			switch(tPreSetInRsp.m_byRspSpyMode)
			{
			case MODE_BOTH:
				{
					//走改变发言人逻辑
					TMt tSpeaker = m_tConf.GetSpeaker();
					if( tSpeaker == tPreSetInRsp.m_tSetInReqInfo.m_tSrc )
					{
						byAddDestSpyNum = 0;
					}
					else
					{
						byAddDestSpyNum = 1;
						m_cSMcuSpyMana.LeftOnceToFree(tSpeaker, MODE_BOTH, dwCanReleaseBW, swCanRelIndex);
						//只有同一下级的mcu才判定预释放带宽
						if (tSpeaker.GetMcuId() != tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId())
						{
							dwCanReleaseBW = 0;
						}
					}
				}
				break;
			case MODE_VIDEO:
				{
					//视频会议轮询
					TMt tLastMt = m_tConfPollParam.GetLastPolledMt();		
					
					u8 bySrcChnnl = ( GetLocalMtFromOtherMcuMt(tLastMt) == m_tPlayEqp ? m_byPlayChnnl : 0);
					
					//本地视频广播
					if (!m_tConf.m_tStatus.IsBrdstVMP())
					{
						if( tLastMt == tPreSetInRsp.m_tSetInReqInfo.m_tSrc )
						{
							byAddDestSpyNum = 0;
						}
						else
						{
							byAddDestSpyNum = 1;
							m_cSMcuSpyMana.LeftOnceToFree(tLastMt, MODE_VIDEO, dwCanReleaseBW, swCanRelIndex);
							//只有同一下级的mcu才判定预释放带宽
							if (tLastMt.GetMcuId() != tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId())
							{
								dwCanReleaseBW = 0;
							}
						}
					}
				}
				break;
			default:
				break;
			}
		}
		break;

	case MCS_MCU_START_SWITCH_TW_REQ:      // 会控选择进电视墙
		{
			TSpyTVWallInfo tSpyTVWallInfo = tSpyInfo.m_tSpyTVWallInfo;	
			TPeriEqpStatus tTvWallStatus;   
			memset( &tTvWallStatus,0,sizeof( tTvWallStatus ) );
			
			g_cMcuVcApp.GetPeriEqpStatus(tSpyTVWallInfo.m_tTvWall.GetEqpId(), &tTvWallStatus);

			TMt tOldMt = (TMt)tTvWallStatus.m_tStatus.tTvWall.atVideoMt[tSpyTVWallInfo.m_byDstChlIdx];

			if( tOldMt.GetMcuId() == tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId()
				&&tOldMt.GetMtId() == tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMtId()
				)
			{
				byAddDestSpyNum = 0;
			}
			else
			{
				byAddDestSpyNum = 1;

				m_cSMcuSpyMana.LeftOnceToFree(tOldMt, MODE_BOTH, dwCanReleaseBW, swCanRelIndex);
				//只有同一下级的mcu才判定预释放带宽
				if (tOldMt.GetMcuId() != tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId())
				{
					dwCanReleaseBW = 0;
				}
			}
		}
		break;

    case MCS_MCU_CHANGEROLLCALL_REQ:       // 会议点名逻辑
		{
			TMtStatus tMtStatus;
			memset( &tMtStatus,0,sizeof( tMtStatus ) );
			if(  ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() ||
				ROLLCALL_MODE_CALLER == m_tConf.m_tStatus.GetRollCallMode()
				)
			{
				m_ptMtTable->GetMtStatus( m_tRollCaller.GetMtId(),&tMtStatus );
				TMt tSrcSelMt = tMtStatus.GetSelectMt( MODE_VIDEO );
				if( tSrcSelMt == tPreSetInRsp.m_tSetInReqInfo.m_tSrc )
				{
					byAddDestSpyNum = 0;
				}
				else
				{
					byAddDestSpyNum = 1;
					m_cSMcuSpyMana.LeftOnceToFree(tSrcSelMt, MODE_VIDEO, dwCanReleaseBW, swCanRelIndex);
					//只有同一下级的mcu才判定预释放带宽
					if (tSrcSelMt.GetMcuId() != tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId())
					{
						dwCanReleaseBW = 0;
					}
				}
			}
			else if( ROLLCALL_MODE_CALLEE == m_tConf.m_tStatus.GetRollCallMode() )
			{
				if (tPreSetInRsp.m_tSetInReqInfo.m_tSrc == tSpyInfo.m_tSpyRollCallInfo.m_tOldCallee)
				{
					byAddDestSpyNum = 0;
				}
				else
				{
					byAddDestSpyNum = 1;

					TMt tOldCallee;
					tOldCallee = tSpyInfo.m_tSpyRollCallInfo.m_tOldCallee;
					m_cSMcuSpyMana.LeftOnceToFree(tOldCallee, MODE_VIDEO, dwCanReleaseBW, swCanRelIndex);
					//只有同一下级的mcu才判定预释放带宽
					if (tOldCallee.GetMcuId() != tPreSetInRsp.m_tSetInReqInfo.m_tSrc.GetMcuId())
					{
						dwCanReleaseBW = 0;
					}
				}
			}
		}
		break;

	case MCS_MCU_STARTREC_REQ:		//终端录像
		{
			byAddDestSpyNum = 1;
		}
		break;
	default:
		break;
	}	*/
	return;
}

/*==============================================================================
函数名    :  OnMMcuPreSetIn
功能      :  级联多回传，在发起Setin之前，先进行握手以确定是否支持多回传
             及分配带宽和回传通道
算法实现  :  
参数说明  :  [IN] TPreSetInReq 请求下级回传的必要信息
返回值说明:  BOOL32
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-9-28                   薛亮							create
2009-11-01                   pengjie                         modify
20100730                    pengjie                        多回传代码整理
2011/01/19					xl								返回值修改
==============================================================================*/
BOOL32 CMcuVcInst::OnMMcuPreSetIn( TPreSetInReq &tPreSetInReq )
{
	if( tPreSetInReq.m_tSpyMtInfo.GetSpyMt().IsNull() || tPreSetInReq.m_tSpyMtInfo.GetSpyMt().IsLocal() )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[OnMMcuPreSetIn] SpySrcMt error !" );
		return FALSE;
	}

	s16 swIndex = m_cSMcuSpyMana.FindSpyMt(tPreSetInReq.m_tSpyMtInfo.GetSpyMt());
	//如果此回传源已经在回传列表中
	if (-1 != swIndex)
	{
		CRecvSpy tSrcSpyInfo;
		m_cSMcuSpyMana.GetRecvSpy(swIndex, tSrcSpyInfo);
		if( tSrcSpyInfo.m_byUseState == TSpyStatus::WAIT_FREE &&
			(  VCS_CONF != m_tConf.GetConfSource() ||
				m_cVCSConfStatus.GetTVWallManageMode() != VCS_TVWALLMANAGE_AUTO_MODE ||
				(
				 MCS_MCU_START_SWITCH_HDU_REQ != tPreSetInReq.GetEvId() &&
				 MCS_MCU_START_SWITCH_TW_REQ != tPreSetInReq.GetEvId()
				)
				||
				tPreSetInReq.m_tReleaseMtInfo.m_tMt.IsNull() ||
				GetFstMcuIdFromMcuIdx(tPreSetInReq.m_tSpyMtInfo.GetSpyMt().GetMcuId()) !=
				GetFstMcuIdFromMcuIdx( tPreSetInReq.m_tReleaseMtInfo.m_tMt.GetMcuId() )
				)
			)
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[OnMMcuPreSetIn] Fail to Presetin,SpySrcMt(%d.%d) Now Status  is WAIT_FREE!\n",
						tPreSetInReq.m_tSpyMtInfo.GetSpyMt().GetMcuId(),
						tPreSetInReq.m_tSpyMtInfo.GetSpyMt().GetMtId()
						);
			CServMsg cServMsg;
			cServMsg.SetEventId( MCU_MCS_ALARMINFO_NOTIF );
			cServMsg.SetErrorCode( ERR_MCU_CONFSNDBANDWIDTHISFULL );	
			SendMsgToAllMcs( MCU_MCS_ALARMINFO_NOTIF, cServMsg );
			return FALSE;
		}
	}

	//zhouyiliang 20101015 如果spymt的相应逻辑通道没打开，return
	TMtStatus tMtStatus;	
	if ( GetMtStatus( tPreSetInReq.m_tSpyMtInfo.GetSpyMt(),tMtStatus ) )
	{
		BOOL32 bLogicalChnlOpen = TRUE;
		if ( !tMtStatus.IsSendVideo() 
			&& ( tPreSetInReq.m_bySpyMode == MODE_VIDEO || tPreSetInReq.m_bySpyMode == MODE_BOTH ||  
			     tPreSetInReq.m_bySpyMode == MODE_VIDEO_CHAIRMAN || tPreSetInReq.m_bySpyMode == MODE_BOTH_CHAIRMAN )
			) 
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[OnMMcuPreSetIn] SpySrcMt video logical chanel not open !" );
			bLogicalChnlOpen = FALSE;
		} 
		if ( !tMtStatus.IsSendAudio() 
			&& ( tPreSetInReq.m_bySpyMode == MODE_AUDIO || tPreSetInReq.m_bySpyMode == MODE_BOTH || 
			     tPreSetInReq.m_bySpyMode == MODE_BOTH_CHAIRMAN )
			) 
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[OnMMcuPreSetIn] SpySrcMt audio logical chanel not open  !" );
			bLogicalChnlOpen = FALSE;
		} 	
		if ( !bLogicalChnlOpen ) 
		{
			//zhouyiliang 20101015 如果是vmp请求且为vcs自动画面合成，presetin不成功，应该恢复CurUseVMPChanInd
			TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
			if ( m_tConf.GetConfSource() == VCS_CONF 
				&& tConfVmpParam.IsVMPAuto()
				&& MCS_MCU_STARTVMP_REQ ==  tPreSetInReq.GetEvId())
			{
				u16 byCurUseChnId = tPreSetInReq.m_tSpyInfo.m_tSpyVmpInfo.m_byPos;
				m_cVCSConfStatus.SetCurUseVMPChanInd( byCurUseChnId );
				TMt tMtNull;
				tMtNull.SetNull();
				m_cVCSConfStatus.SetReqVCMT( tMtNull );
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[OnMMcuPreSetIn] Update vcs CurUseVMPChanInd.%d:\n",byCurUseChnId);
			}

			return FALSE;
		}
	}

	CServMsg cServMsg;
	TMsgHeadMsg tHeadMsg;

	TMt tNormalMt;
	tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tPreSetInReq.m_tSpyMtInfo.GetSpyMt(), tNormalMt );	
    tPreSetInReq.m_tSpyMtInfo.SetSpyMt( tNormalMt );

	//处理可释放终端信息
	TMultiCacMtInfo tMtInfo = BuildMultiCascadeMtInfo( tPreSetInReq.m_tReleaseMtInfo.m_tMt,tPreSetInReq.m_tReleaseMtInfo.m_tMt );
	tPreSetInReq.m_tReleaseMtInfo.m_byCasLevel	= tMtInfo.m_byCasLevel;
	memcpy( &tPreSetInReq.m_tReleaseMtInfo.m_abyMtIdentify[0],
		&tMtInfo.m_abyMtIdentify[0],
		sizeof(tPreSetInReq.m_tReleaseMtInfo.m_abyMtIdentify)
		);
	tPreSetInReq.m_tReleaseMtInfo.m_byIsNeedRelease = 0;
	tPreSetInReq.m_tReleaseMtInfo.SetCount(0);

	u8 bySrcMcuId = (u8)tPreSetInReq.m_tSpyMtInfo.GetSpyMt().GetMcuId();
	if( bySrcMcuId != (u8)tPreSetInReq.m_tReleaseMtInfo.m_tMt.GetMcuId() )
	{
		tPreSetInReq.m_tReleaseMtInfo.SetCount(tPreSetInReq.m_tReleaseMtInfo.GetCount()-1);
	}

	//zhouyiliang 20121023 注释掉，因为有可能虽然有可替换终端，但是发现可替换终端的计数为0
	//如会议轮询带音频，轮询过程中取消当前发言人，轮下一个终端。此时计数就可能为0，加1的话如果还在另外业务中，就被替换掉了
// 	if( 0 == tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum &&
// 		( MODE_BOTH == tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode ||
// 			MODE_VIDEO == tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode )
// 		)
// 	{
// 		++tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
// 	}
// 	if( 0 == tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum &&
// 		( MODE_BOTH == tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode ||
// 			MODE_AUDIO == tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseMode )
// 		)
// 	{
// 		++tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
// 	}

	//[2011/09/16/zhangli]如果是MP4-16CIF修正下发分辨率为4CIF
	TSimCapSet tSimCapSet = tPreSetInReq.m_tSpyMtInfo.GetSimCapset();
	if (MEDIA_TYPE_MP4 == tSimCapSet.GetVideoMediaType()
		&& VIDEO_FORMAT_16CIF == tSimCapSet.GetVideoResolution())
	{	
		tSimCapSet.SetVideoResolution(VIDEO_FORMAT_4CIF);
		tPreSetInReq.m_tSpyMtInfo.SetSimCapset(tSimCapSet);
	}
	
	// 对最终决定的能力中的分辨率做调整
	u32 dwResW = 0;
	u32 dwResH = 0;
	u16 wTmpResW = 0;
	u16 wTmpResH = 0;
	u8 byRes = tSimCapSet.GetVideoResolution();
	// mpeg4 auto需要根据会议码率获得分辨率
	if ( VIDEO_FORMAT_AUTO == byRes )
	{
		byRes = GetAutoResByBitrate(byRes,m_tConf.GetBitRate());
	}
	// 对分辨率进行调整,可能下级是旧mcu,需发共通分辨率,真实分辨率宽高在最后追加
	GetWHByRes(byRes, wTmpResW, wTmpResH);
	// 根据分辨率,获得对应通用分辨率,新旧MCU都能识别的分辨率
	dwResW = htonl((u32)wTmpResW);
	dwResH = htonl((u32)wTmpResH);
	byRes = GetNormalRes(wTmpResW, wTmpResH);
	tSimCapSet.SetVideoResolution(byRes);
	tPreSetInReq.m_tSpyMtInfo.SetSimCapset(tSimCapSet);

	//发送多回传请求命令
	cServMsg.SetEventId(MCU_MCU_PRESETIN_REQ);
	cServMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
	cServMsg.CatMsgBody( (u8 *)&tPreSetInReq, sizeof(TPreSetInReq) );
	cServMsg.CatMsgBody( (u8 *)&dwResW, sizeof(u32) );
	cServMsg.CatMsgBody( (u8 *)&dwResH, sizeof(u32) );

	SendMsgToMt( bySrcMcuId, MCU_MCU_PRESETIN_REQ, cServMsg);

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "Send PRESETIN_REQ to mt(%d.%d.%d) lvl.%d.(%s) ReleaseMt(%d,%d,%d)[VNum.%d,ANum.%d,mode.%d].Res.%d,Width.%dHeight.%d\n",
		tPreSetInReq.m_tSpyMtInfo.GetSpyMt().GetMcuId(),
		tPreSetInReq.m_tSpyMtInfo.GetSpyMt().GetMtId(),
		tHeadMsg.m_tMsgDst.m_abyMtIdentify[0],
		tHeadMsg.m_tMsgDst.m_byCasLevel,
		OspEventDesc( u16(tPreSetInReq.GetEvId()) ),
		tPreSetInReq.m_tReleaseMtInfo.m_tMt.GetMcuId(),
		tPreSetInReq.m_tReleaseMtInfo.m_tMt.GetMtId(),
		tPreSetInReq.m_tReleaseMtInfo.m_abyMtIdentify[0],
		tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum,
		tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum,
		tPreSetInReq.m_bySpyMode,
		byRes,
		wTmpResW,
		wTmpResH
		);

	return TRUE;
}


/*==============================================================================
函数名    :  ProcMcuMcuSpyFastUpdateCmd
功能      :  处理级联多回传请求关键帧
算法实现  :  
参数说明  :  [IN] const CMessage
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
20100402                 pengjie                         create
==============================================================================*/
void CMcuVcInst::ProcMcuMcuSpyFastUpdateCmd( const CMessage *pcMsg )
{
	STATECHECK;	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);	

	TMt tOrgMt = *(TMt *)( cServMsg.GetMsgBody() );
	u8  byMode = *(u8  *)( cServMsg.GetMsgBody() + sizeof(TMt) );
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)( cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) );

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[ProcMcuMcuSpyFastUpdateCmd] tOrgMt<McuId:%d, MtId:%d>!\n",
			  	   tOrgMt.GetMcuId(), tOrgMt.GetMtId());
	TMt tSrcMt;
	u8  byLocalMtId = 0;
	if (!tOrgMt.IsMcuIdLocal())
	{
		//非本级的转成本级所认识的mcuidx形式tmt
		tSrcMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgDst, tOrgMt); 
		byLocalMtId = u8(tOrgMt.GetMcuId());
	}
	else
	{
		tSrcMt = tOrgMt;

		tSrcMt.SetMcuIdx(LOCAL_MCUIDX);
		byLocalMtId = tSrcMt.GetMtId();
	}

// 	if( !tSrcMt.IsMcuIdLocal() )
// 	{
// 		cServMsg.SetEventId( MCU_MCU_SPYFASTUPDATEPIC_CMD );		
// 		cServMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
// 		cServMsg.CatMsgBody( (u8 *)&tSrcMt, sizeof(TMt) );
// 		cServMsg.CatMsgBody( (u8 *)&byMode, sizeof(u8) );
// 		
// 		SendMsgToMt( (u8)tSrcMt.GetMcuId(), MCU_MCU_SPYFASTUPDATEPIC_CMD, cServMsg );
// 		return;
// 	}

// 	tSrcMt.SetMcuIdx( LOCAL_MCUIDX );
	
	// [11/10/2010 xliang] check if the mt is spyMt before check multispyMana
	BOOL32 bSrcIsSpyMt = FALSE; //是否是下级上传终端
	TSimCapSet tDstCap;
	if( !m_tCascadeMMCU.IsNull() && tOrgMt.IsMcuIdLocal())
	{
		TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
		if( ptConfMcInfo != NULL && ptConfMcInfo->m_tSpyMt == tSrcMt )
		{
			bSrcIsSpyMt = TRUE;
			tDstCap = m_ptMtTable->GetDstSCS(m_tCascadeMMCU.GetMtId());
		}
	}

	if( !bSrcIsSpyMt )
	{
		//先获取本级保存的上级可接收能力集
		CSendSpy tSendSpy;
		if(!m_cLocalSpyMana.GetSpyChannlInfo(tSrcMt, tSendSpy))
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuSpyFastUpdateCmd] GetSpyChannlInfo Mt:%d failed!\n", tSrcMt.GetMtId());
			return;
		}
		if (tSendSpy.GetSimCapset().IsNull())
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuSpyFastUpdateCmd] Mt%d's m_cLocalSpyMana->Cap is null!\n", tSrcMt.GetMtId());
			return;
		}
		tDstCap = tSendSpy.GetSimCapset();
	}
 	
	TSimCapSet tSrcCap = m_ptMtTable->GetSrcSCS(byLocalMtId);

	TBasOutInfo tReqBasOutInfo;
	if (MODE_AUDIO == byMode)
	{
		if(tDstCap.GetAudioMediaType() != tSrcCap.GetAudioMediaType())
		{
			if(!FindBasChn2SpyForMt(tSrcMt, tDstCap, byMode,tReqBasOutInfo))
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuSpyFastUpdateCmd:Aud] FindBasChn2SpyForMt%d failed!\n", tSrcMt.GetMtId());
				return;
			}
			NotifyFastUpdate(tReqBasOutInfo.m_tBasEqp, tReqBasOutInfo.m_byChnId);
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuSpyFastUpdateCmd:Aud] tSrcId: %d, NotifyFastUpdate Bas<EqpId:%d, ChnId:%d>!\n",
 							tSrcMt.GetMtId(),  tReqBasOutInfo.m_tBasEqp.GetEqpId(), tReqBasOutInfo.m_byChnId);
		}
		else
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[ProcMcuMcuSpyFastUpdateCmd:Aud] tSrcId: %d, Not Used SelBas!\n",
				tSrcMt.GetMtId() );
			NotifyFastUpdate(tSrcMt, byMode);
		}
	}
	else if (MODE_VIDEO == byMode || MODE_SECVIDEO == byMode)
	{
		if(IsNeedSpyAdpt(tSrcMt, tDstCap, MODE_VIDEO))
		{
			if(!FindBasChn2SpyForMt(tSrcMt, tDstCap, byMode, tReqBasOutInfo))
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuSpyFastUpdateCmd:Vid] FindBasChn2SpyForMt%d failed!\n", tSrcMt.GetMtId());
				return;
			}
			NotifyFastUpdate(tReqBasOutInfo.m_tBasEqp, tReqBasOutInfo.m_byChnId);
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuSpyFastUpdateCmd:Vid] tSrcId: %d, NotifyFastUpdate Bas<EqpId:%d, ChnId:%d>!\n",
 						tSrcMt.GetMtId(),  tReqBasOutInfo.m_tBasEqp.GetEqpId(), tReqBasOutInfo.m_byChnId );
		}
		else
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[ProcMcuMcuSpyFastUpdateCmd:Vid] tSrcId: %d, Not Used SelBas!\n",
				tSrcMt.GetMtId() );
			NotifyFastUpdate(tSrcMt, byMode);
		}
	}
	else
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, "[ProcMcuMcuSpyFastUpdateCmd] unexpected Mode:%d!\n", byMode);
	}

	return;
}

/*==============================================================================
函数名    :  SendMcuMcuSpyFastUpdateCmd
功能      :  发送级联多回传请求关键帧
算法实现  :  
参数说明  :  TMt &tMt 下级某个源终端
             u8 byMode 视频类型（主流、双流）默认主流
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
20100402                 pengjie                         create
==============================================================================*/
void CMcuVcInst::SendMcuMcuSpyFastUpdateCmd( const TMt &tMt, u8 byMode )
{
	CServMsg cServMsg;
	TMsgHeadMsg tHeadMsg;


	cServMsg.SetEventId( MCU_MCU_SPYFASTUPDATEPIC_CMD );
	TMt tNormalMt;
	tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tNormalMt );
	//tMt = ;
	

	cServMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
	cServMsg.CatMsgBody( (u8 *)&tNormalMt, sizeof(TMt) );
	cServMsg.CatMsgBody( (u8 *)&byMode, sizeof(u8) );

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY,  "[SendMcuMcuSpyFastUpdateCmd] tSrcId: %d, Mode: %d ! \n", tMt.GetMtId(), byMode );
	
	SendMsgToMt( GetFstMcuIdFromMcuIdx(tMt.GetMcuId()), MCU_MCU_SPYFASTUPDATEPIC_CMD, cServMsg );

	return;
}


/*==============================================================================
类名		:  JudgeMcuMcuPreSetIn
功能		:  判断下级是否支持此终端的回传
参数说明	:	const TPreSetInReq& tPreSetInReq req参数，
				TPreSetInRsp& tPreSetInRsp 准备返回给上级的rsp参数，会被修改
				u16 &wErrorCode 如果不符合下级多回传条件的话，设置相应的ErrorCode
返回值说明	: TRUE支持，FALSE不支持
备注		:  
-------------------------------------------------------------------------------
修改记录:  
日  期     版本          修改人          走读人          修改记录
2010-6-10                lukunpeng                        modify
==============================================================================*/
BOOL32 CMcuVcInst::JudgeMcuMcuPreSetIn(const TPreSetInReq& tPreSetInReq, const TMsgHeadMsg& tHeadMsg, TPreSetInRsp& tPreSetInRsp, u16 &wErrorCode)
{
	//[2011/10/27/zhangli]为保留错误号原值，错误号不赋初值
	//wErrorCode = 0;
	TMt tOrgSrc = tPreSetInReq.m_tSpyMtInfo.GetSpyMt();
	TMt tUnLocalSrc = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgDst,tOrgSrc );
	u8  bySpyMode = tPreSetInReq.m_bySpyMode;
	u8 byRealRes = 0;

	TMt tSrc;
	if (!tOrgSrc.IsMcuIdLocal())
	{
		tSrc = m_ptMtTable->GetMt(u8(tOrgSrc.GetMcuId()));
	}
	else
	{
		tSrc = m_ptMtTable->GetMt(tOrgSrc.GetMtId());
	}

	//1、本机不支持多回传处理
	if( !IsSupportMultiSpy() )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[JudgeMcuMcuPreSetIn] Conf Is Not Support MultiSpy.\n" );
		wErrorCode = ERR_MCU_CONFNOTSUPPORTMULTISPY;
		return FALSE;
	}
	
	//2、如果回传源错误处理
	if( tSrc.IsNull())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "JudgeMcuMcuPreSetIn False, tSrc is NULL! \n" );
		return FALSE;
	}

	//3、如果回传mode是none处理
	if( bySpyMode == MODE_NONE )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "JudgeMcuMcuPreSetIn False, SpyMode == MODE_NONE! \n" );
		return FALSE;
	}

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "JudgeMcuMcuPreSetIn SpyMode = %d \n", bySpyMode );

	//4、回传源终端必须已经加入会议
	if (FALSE == m_tConfAllMtInfo.MtJoinedConf(tSrc.GetMtId()))
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "JudgeMcuMcuPreSetIn Failed Mt:%d Don't Online \n",tSrc.GetMtId() );
		return FALSE;
	}

	//5、如果回传源的后向通道没有打开，也要回NACK
	TLogicalChannel tVidChnnl;
	TLogicalChannel tAudChnnl;
	if (MODE_VIDEO == bySpyMode || MODE_BOTH == bySpyMode)
	{
		if (FALSE == m_ptMtTable->GetMtLogicChnnl( tSrc.GetMtId(), LOGCHL_VIDEO, &tVidChnnl, FALSE ))
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY,  "JudgeMcuMcuPreSetIn Failed Mt:%d 's Revert Vid LogicChnnl not open \n",
							tSrc.GetMtId() );
			return FALSE;
		}
	}

	if (MODE_AUDIO == bySpyMode || MODE_BOTH == bySpyMode)
	{
		if (FALSE == m_ptMtTable->GetMtLogicChnnl( tSrc.GetMtId(), LOGCHL_AUDIO, &tAudChnnl, FALSE ))
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY,  "JudgeMcuMcuPreSetIn Failed Mt:%d 's Revert Aud LogicChnnl not open \n",
							tSrc.GetMtId() );
			return FALSE;
		}
	}

	TSimCapSet tSimCap = tPreSetInReq.m_tSpyMtInfo.GetSimCapset();

	// 6.适配资源校验
	TBasOutInfo	tBasOutInfo;
	BOOL32 bAdpOK = FALSE;
	if(MODE_BOTH == bySpyMode || MODE_VIDEO == bySpyMode)
	{
		TSimCapSet tSrcCap = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
		
		if (IsNeedSpyAdpt(tSrc, tSimCap, MODE_VIDEO) &&
			!FindBasChn2SpyForMt(tUnLocalSrc, tSimCap, MODE_VIDEO, tBasOutInfo))
		{
			////[2011/08/25/zhangli]如果找不到，就向上找，看向上的是否仅为本回传占用，是的话，可以刷此通道参数
			////如果找不到再尝试占用空闲的
			TSimCapSet tTempSrcCap = tSimCap;
			u8 byNextUpStandRes = GetUpStandRes(tSimCap.GetVideoResolution());
			while (VIDEO_FORMAT_INVALID != byNextUpStandRes)
			{
				tTempSrcCap.SetVideoResolution(byNextUpStandRes);
				if (FindBasChn2SpyForMt(tUnLocalSrc, tTempSrcCap, MODE_VIDEO, tBasOutInfo))
				{
					if (IsCanRlsBasChn(tUnLocalSrc, tUnLocalSrc, tTempSrcCap, MODE_VIDEO))
					{	
						bAdpOK = TRUE;
						break;
					}
				}
				byNextUpStandRes = GetUpStandRes(byNextUpStandRes);
			}
			
			if (!bAdpOK)
			{
				//循序尝试降分辨率适配
				CBasChn *pcBasChn=NULL;
				TBasChnCapData tBasChnCapData;
				bAdpOK = GetBasChnForMultiSpy(tUnLocalSrc,tSimCap,MODE_VIDEO,&pcBasChn,tBasChnCapData);	
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[JudgeMcuMcuPreSetIn] tPreSetInReq.m_tReleaseMtInfo.GetCount() = %d!\n", tPreSetInReq.m_tReleaseMtInfo.GetCount());
				//没有空闲通道并且可释放终端需要过适配则尝试复用该可释放适配通道
				if( !bAdpOK && tPreSetInReq.m_tReleaseMtInfo.GetCount() >= 0 )
				{
					TMultiCacMtInfo tMtInfo;
					tMtInfo.m_byCasLevel = tPreSetInReq.m_tReleaseMtInfo.m_byCasLevel;
					memcpy( &tMtInfo.m_abyMtIdentify[0],
						&tPreSetInReq.m_tReleaseMtInfo.m_abyMtIdentify[0],
						sizeof(tMtInfo.m_abyMtIdentify));

					TMt tCanReleaseMt = GetMtFromMultiCascadeMtInfo( tMtInfo, tPreSetInReq.m_tReleaseMtInfo.m_tMt);
					CSendSpy tSendSpy;
					if (!tCanReleaseMt.IsNull() && 
						m_cLocalSpyMana.GetSpyChannlInfo(tCanReleaseMt, tSendSpy))
					{
						//第一次尝试可释放适配通道是否可复用
						bAdpOK = IsCanRlsBasChn(tCanReleaseMt, tUnLocalSrc, tSendSpy.GetSimCapset(), MODE_VIDEO);
						if(!bAdpOK)
						{
							//分辨率取小再尝试可释放适配通道是否可复用
							TSimCapSet tDstCap = tSendSpy.GetSimCapset();
							
							const u8 byDstRes = tDstCap.GetVideoResolution();
							const u8 bySrcRes = tSrcCap.GetVideoResolution();
							tDstCap.SetVideoResolution(min(byDstRes, bySrcRes));
							bAdpOK = IsCanRlsBasChn(tCanReleaseMt, tUnLocalSrc,tDstCap, MODE_VIDEO);
						}
						if (bAdpOK)
						{
							tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byIsReuseBasChl = MODE_VIDEO;
						}					
					}	
				}
			}
		}	
		else
		{
			//已有回传适配通道可复用
			bAdpOK = TRUE;
		}

		if (!bAdpOK)
		{
			bySpyMode  = (MODE_BOTH == bySpyMode) ? MODE_AUDIO:MODE_NONE;
			wErrorCode = ERR_MCU_CASDBASISNOTENOUGH;
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[JudgeMcuMcuPreSetIn] No idle Bas channel(Video) support Mt(%d) to Mt(%d).Now Mode(%d)\n",
				tSrc.GetMtId(),m_tCascadeMMCU.GetMtId(),bySpyMode );
		}		
	}

	bAdpOK = FALSE;
	if (MODE_BOTH == bySpyMode || MODE_AUDIO == bySpyMode)
	{
		CBasChn *pcBasChn=NULL;
		TBasChnCapData tBasChnCapData;

		//获得源能力
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

		//获得目的能力
		TAudioTypeDesc tDstAudCap;
		TLogicalChannel tDstAudLgc;
		if (!m_ptMtTable->GetMtLogicChnnl(m_tCascadeMMCU.GetMtId(), MODE_AUDIO, &tDstAudLgc, TRUE))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS,  "[IsNeedSpyAdpt] GetMtLogicChnnl Dst Aud Mt%d failed!\n", m_tCascadeMMCU.GetMtId());
			return FALSE;
		}
		tDstAudCap.SetAudioMediaType( tDstAudLgc.GetChannelType() );
		tDstAudCap.SetAudioTrackNum( tDstAudLgc.GetAudioTrackNum() );

		//先尝试获取空闲适配通道
		if ( IsNeedSpyAdpt(tSrc,tSimCap,MODE_AUDIO)&&
			!FindBasChn2SpyForMt(tUnLocalSrc, tSimCap, MODE_AUDIO, tBasOutInfo) &&
			!g_cMcuVcApp.GetIdleAudBasChn(tSrcAudCap, tDstAudCap,&pcBasChn))
		{
			//没有空闲通道并且可释放终端需要过适配则尝试复用适配通道
			if( tPreSetInReq.m_tReleaseMtInfo.GetCount() >= 0 )
			{
				TMultiCacMtInfo tMtInfo;
				tMtInfo.m_byCasLevel = tPreSetInReq.m_tReleaseMtInfo.m_byCasLevel;
				memcpy( &tMtInfo.m_abyMtIdentify[0],
					    &tPreSetInReq.m_tReleaseMtInfo.m_abyMtIdentify[0],
					    sizeof(tMtInfo.m_abyMtIdentify)
					     );
				TMt tCanReleaseMt = GetMtFromMultiCascadeMtInfo( tMtInfo, tPreSetInReq.m_tReleaseMtInfo.m_tMt);
				CSendSpy tSendSpy;
				if (!tCanReleaseMt.IsNull() && 
					m_cLocalSpyMana.GetSpyChannlInfo(tCanReleaseMt, tSendSpy) &&
					IsCanRlsBasChn(tCanReleaseMt,tUnLocalSrc, tSendSpy.GetSimCapset(), MODE_AUDIO))
				{
					if (MODE_VIDEO == tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byIsReuseBasChl)
					{
						tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byIsReuseBasChl = MODE_BOTH;
					}
					else
					{
						tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byIsReuseBasChl = MODE_AUDIO;
					}
					
					bAdpOK = TRUE;
				}			
			}		
		}
		else
		{	
			bAdpOK = TRUE;		
		}
		if (!bAdpOK)
		{
			bySpyMode  = (MODE_BOTH == bySpyMode) ? MODE_VIDEO:MODE_NONE;
			wErrorCode = ERR_MCU_CASDBASISNOTENOUGH;
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU,"[JudgeMcuMcuPreSetIn] No idle Bas channel(Audio) support Mt(%d) to Mt(%d).Now Mode(%d)\n",
				tSrc.GetMtId(),m_tCascadeMMCU.GetMtId(),bySpyMode );
		}		
	}

	//zjj20110119 对于会议轮询下级没有bas导致无法上传视频和终端无视频源都采取跳过策略
	if( MODE_AUDIO == bySpyMode && MCUVC_POLLING_CHANGE_TIMER == tPreSetInReq.GetEvId() )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY,  "JudgeMcuMcuPreSetIn Failed Mt:%d 's,no Video Mode No Polling.\n",
						tSrc.GetMtId() );
		return FALSE;
	}

	if( MODE_BOTH != bySpyMode && MCS_MCU_STARTREC_REQ == tPreSetInReq.GetEvId() )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY,  "JudgeMcuMcuPreSetIn Failed Mt:%d 's,no Both Mode No Recording.\n",
						tSrc.GetMtId() );
		return FALSE;
	}

	if( MODE_NONE == bySpyMode )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY,  "JudgeMcuMcuPreSetIn Failed Mt:%d 's Revert Aud LogicChnnl not open \n",
						tSrc.GetMtId() );
		return FALSE;
	}

	// 转换后的模式反馈给上级
	tPreSetInRsp.m_byRspSpyMode = bySpyMode;

	return TRUE;
}

/*==============================================================================
类名    :  ProcMcuMcuPreSetInReq
功能    :  处理上级发来的PreSetIn请求，在这里下级会分析是否满足回传条件及回传带宽分析等，然后通知上级
主要接口:  
参数    :  [IN] const CMessage
备注    :  
-------------------------------------------------------------------------------
修改记录:  
日  期     版本          修改人          走读人          修改记录
2010-6-10                lukunpeng                        modify
20100730                 pengjie                         多回传代码整理
==============================================================================*/
void CMcuVcInst::ProcMcuMcuPreSetInReq(const CMessage *pcMsg )
{
	STATECHECK;	

	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	CServMsg cMsg;
	BOOL32 bNack = FALSE;
	//[2011/10/27/zhangli]保留上级传下来的错误号，如果第二级出错，第三级要取出错误号，传给第一级
	u16 wErrorCode = cServMsg.GetErrorCode();
	
	TPreSetInReq tPreSetInReq = *(TPreSetInReq *)( cServMsg.GetMsgBody() );
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody()+sizeof(TPreSetInReq));

	//针对上级带真实分辨率的宽高，需要解析[11/12/2012 chendaiwei]
	u32 dwResW = 0;
	u32 dwResH = 0;
	if( cServMsg.GetMsgBodyLen() > sizeof(TPreSetInReq) + sizeof(TMsgHeadMsg))
	{
		dwResW = *(u32*)(cServMsg.GetMsgBody()+sizeof(TPreSetInReq)+sizeof(TMsgHeadMsg));
		dwResH = *(u32*)(cServMsg.GetMsgBody()+sizeof(TPreSetInReq)+sizeof(TMsgHeadMsg)+sizeof(u32));
		//将宽高由网络序转为主机序
		dwResW = ntohl(dwResW);
		dwResH = ntohl(dwResH);
		u8 byRes = GetMcuSupportedRes((u16)dwResW, (u16)dwResH);
		TSimCapSet tCap = tPreSetInReq.m_tSpyMtInfo.GetSimCapset();
		tCap.SetVideoResolution(byRes);
		tPreSetInReq.m_tSpyMtInfo.SetSimCapset(tCap);
	}

	TPreSetInRsp tPreSetInRsp;
	tPreSetInRsp.m_tSetInReqInfo = tPreSetInReq;
	TMt tSrc = tPreSetInReq.m_tSpyMtInfo.GetSpyMt();
	TMsgHeadMsg tHeadMsgRsp;

	tHeadMsgRsp.m_tMsgSrc = tHeadMsg.m_tMsgDst;
	tHeadMsgRsp.m_tMsgDst = tHeadMsg.m_tMsgSrc;
	cMsg.SetMsgBody( (u8 *)&tHeadMsgRsp, sizeof(TMsgHeadMsg) );

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuPreSetInReq] Rcv MMcu PreSetInReq SpyMode:%d , VidCap<Media.%d Res.%d BR.%d Fps.%d Profile:%d>, AudCap<Media.%d>, ReleaseCount.%d, ReleaseMt<McuId.%d, MtId.%d>[V.%d, A.%d], Ev.%d[%s]!\n",
												tPreSetInReq.m_bySpyMode,
												tPreSetInReq.m_tSpyMtInfo.GetSimCapset().GetVideoMediaType(),
												tPreSetInReq.m_tSpyMtInfo.GetSimCapset().GetVideoResolution(),
												tPreSetInReq.m_tSpyMtInfo.GetSimCapset().GetVideoMaxBitRate(),
												MEDIA_TYPE_H264 == tPreSetInReq.m_tSpyMtInfo.GetSimCapset().GetVideoMediaType() ?
												tPreSetInReq.m_tSpyMtInfo.GetSimCapset().GetUserDefFrameRate(): tPreSetInReq.m_tSpyMtInfo.GetSimCapset().GetVideoFrameRate(),
												tPreSetInReq.m_tSpyMtInfo.GetSimCapset().GetVideoProfileType(),
												tPreSetInReq.m_tSpyMtInfo.GetSimCapset().GetAudioMediaType(),
												tPreSetInReq.m_tReleaseMtInfo.GetCount(),
												tPreSetInReq.m_tReleaseMtInfo.m_tMt.GetMcuId(),
												tPreSetInReq.m_tReleaseMtInfo.m_tMt.GetMtId(),
												tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum,
												tPreSetInReq.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum,
												tPreSetInReq.GetEvId(),
												OspEventDesc((u16)tPreSetInReq.GetEvId())
												);

	//注意：此处judge如果条件合适会对tPreSetInRsp作修改
	if (!JudgeMcuMcuPreSetIn(tPreSetInReq, tHeadMsg, tPreSetInRsp, wErrorCode))
	{
		cMsg.SetErrorCode(wErrorCode);
		cMsg.SetEventId(MCU_MCU_PRESETIN_NACK);
		cMsg.CatMsgBody( (u8 *)&tPreSetInRsp, sizeof(tPreSetInRsp) );
		
		SendMsgToMt( cServMsg.GetSrcMtId(), MCU_MCU_PRESETIN_NACK, cMsg );
		return;
	}

	//转成本级McuIdx标识
	TMt tSpyMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgDst, tPreSetInReq.m_tSpyMtInfo.GetSpyMt() );

/*  20110531 zjl 这段代码没用，发消息给第三级时会再设置了一次
	if( !tPreSetInReq.m_tSpyMtInfo.GetSpyMt().IsMcuIdLocal() )
	{
		tPreSetInReq.m_tSpyMtInfo.SetSpyMt( m_ptMtTable->GetMt((u8)tPreSetInReq.m_tSpyMtInfo.GetSpyMt().GetMcuId()) );
	}
	else
	{
		tPreSetInReq.m_tSpyMtInfo.SetSpyMt( m_ptMtTable->GetMt( tPreSetInReq.m_tSpyMtInfo.GetSpyMt().GetMtId() ) );
	}*/

	s16 swIndex = m_cSMcuSpyMana.FindSpyMt( tSpyMt );
	CRecvSpy tSrcSpyInfo;	
	//如果此回传源已经在回传列表中,而且状态为等待释放就直接回Nack给上级
	if (-1 != swIndex)
	{
		m_cSMcuSpyMana.GetRecvSpy(swIndex, tSrcSpyInfo);
		if( tSrcSpyInfo.m_byUseState == TSpyStatus::WAIT_FREE &&
			(  VCS_CONF != m_tConf.GetConfSource() ||
				 ( CONF_CREATE_MT != m_byCreateBy &&
						m_cVCSConfStatus.GetTVWallManageMode() != VCS_TVWALLMANAGE_AUTO_MODE
					 )
					||
				(
				 MCS_MCU_START_SWITCH_HDU_REQ != tPreSetInReq.GetEvId() &&
				 MCS_MCU_START_SWITCH_TW_REQ != tPreSetInReq.GetEvId() 
				)
				||
				tPreSetInReq.m_tReleaseMtInfo.m_tMt.IsNull() || 
				tPreSetInReq.m_tReleaseMtInfo.GetCount() < 0 ||
				GetFstMcuIdFromMcuIdx(tSpyMt.GetMcuId()) != 
					tPreSetInReq.m_tReleaseMtInfo.m_tMt.GetMcuId()
					)
			)
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuPreSetInReq] Fail to Presetin,SpySrcMt(%d.%d) Now Status  is WAIT_FREE!\n",
						tSpyMt.GetMcuId(),
						tSpyMt.GetMtId()
						);
		
			cMsg.SetEventId(MCU_MCU_PRESETIN_NACK);
			cMsg.SetErrorCode( ERR_MCU_SPYMTSTATE_WAITFREE );
			cMsg.CatMsgBody( (u8 *)&tPreSetInRsp, sizeof(tPreSetInRsp) );
			SendMsgToMt( cServMsg.GetSrcMtId(), MCU_MCU_PRESETIN_NACK, cMsg );
			return;
		}
	}

	//如果回传终端不是本级的，再发往对应的mcu
	if( !tSrc.IsMcuIdLocal() )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuPreSetInReq] tSrc(%d.%d) is not local mt,continue send Req to Mcu(%d)\n",
													tSrc.GetMcuId(),tSrc.GetMtId(),tSrc.GetMcuId()
			);
		if( IsLocalAndSMcuSupMultSpy( tSpyMt.GetMcuId() ) )
		{
			//zjl20101117[add]获取本级和下级级联通道的能力填入SimCapSet
			TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSpyMt.GetMcuId()));

			//zjl20110117 如果二三级级联通道格式和第一级要求第二级回传的格式相同，则分辨率取小
			if(!tSimCapSet.IsNull() &&
					(tPreSetInReq.m_tSpyMtInfo.GetSimCapset().GetVideoMediaType() == tSimCapSet.GetVideoMediaType()
						)
				)
			{
				u8 byMinRes = GetMinResAcdWHProduct(tSimCapSet.GetVideoResolution(), 
													tPreSetInReq.m_tSpyMtInfo.GetSimCapset().GetVideoResolution());
				
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuPreSetInReq] GetMinRes %d between DstRes:%d and SrcRes:%d!\n",
											byMinRes, 
											tPreSetInReq.m_tSpyMtInfo.GetSimCapset().GetVideoResolution(),
											tSimCapSet.GetVideoResolution());
				
				if (VIDEO_FORMAT_INVALID != byMinRes)
				{
					tSimCapSet.SetVideoResolution(byMinRes);
				}				
			}

			//zjl20101116 如果当前终端已回传则能力集要与已回传目的能力集取小
			if (!GetMinSpyDstCapSet(tSpyMt, tSimCapSet))
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuPreSetInReq] tSimCapSet is null!\n" );
				return;
			}
			
			u16 wTmpResW = 0;
			u16 wTmpResH = 0;
			// 对分辨率进行调整,可能下级是旧mcu,需发共通分辨率,真实分辨率宽高在最后追加
			u8 byCommonRes = tSimCapSet.GetVideoResolution();
			GetWHByRes(byCommonRes, wTmpResW, wTmpResH);
			// 根据分辨率,获得对应通用分辨率,新旧MCU都能识别的分辨率
			dwResW = wTmpResW;
			dwResH = wTmpResH;
			byCommonRes = GetNormalRes(wTmpResW, wTmpResH);
			tSimCapSet.SetVideoResolution(byCommonRes);

			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcMcuMcuPreSetInReq:SendToNextMcu] VidCap<Media:%d, Res:%d, BR:%d, Fps:%d>!\n",
														tSimCapSet.GetVideoMediaType(), 
														tSimCapSet.GetVideoResolution(), 
														tSimCapSet.GetVideoMaxBitRate(),
														MEDIA_TYPE_H264 == tSimCapSet.GetVideoMediaType() ?
														tSimCapSet.GetUserDefFrameRate(): tSimCapSet.GetVideoFrameRate());

			tPreSetInReq.m_tSpyMtInfo.SetSimCapset(tSimCapSet);	
			tPreSetInReq.m_tSpyMtInfo.SetSpyMt( tSrc );
			tPreSetInReq.m_bySpyMode = tPreSetInRsp.m_byRspSpyMode;

			//tHeadMsg.m_tMsgDst.m_tMt = tSrc;//BuildMultiCascadeMtInfo( tPreSetInReq.m_tSrc );
			if(  tPreSetInReq.m_tReleaseMtInfo.GetCount() < 0 ||
				(u8)tPreSetInReq.m_tReleaseMtInfo.m_tMt.GetMcuId() != (u8)tSrc.GetMcuId() 
			  )
			{
				tPreSetInReq.m_tReleaseMtInfo.SetCount(tPreSetInReq.m_tReleaseMtInfo.GetCount()-1);				
			}
			
			//[2011/10/27/zhangli]把错误号传给下级，以便在得到ack时做相应提示
			tPreSetInReq.m_tReleaseMtInfo.m_byIsReuseBasChl = tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byIsReuseBasChl;
			cServMsg.SetErrorCode(wErrorCode);
			cServMsg.SetEventId(MCU_MCU_PRESETIN_REQ);
			cServMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
			cServMsg.CatMsgBody( (u8 *)&tPreSetInReq, sizeof(TPreSetInReq) );
			
			if( dwResW !=0 && dwResH != 0)
			{
				dwResW = htonl(dwResW);
				dwResH = htonl(dwResH);
				cServMsg.CatMsgBody( (u8 *)&dwResW, sizeof(u32) );
				cServMsg.CatMsgBody( (u8 *)&dwResH, sizeof(u32) );
			}

			SendMsgToMt( (u8)tSrc.GetMcuId(), MCU_MCU_PRESETIN_REQ, cServMsg );
			return;
		}		
		else
		{
			tSrc = m_ptMtTable->GetMt( (u8)tSrc.GetMcuId() );
		}
	}

	//下面进行回传终端带宽分析
	TLogicalChannel     tAudChnnl;
	m_ptMtTable->GetMtLogicChnnl( tSrc.GetMtId(), LOGCHL_AUDIO, &tAudChnnl, FALSE );

	u32 dwAudBitrate  = GetAudioBitrate( tAudChnnl.GetChannelType() );
	u32 dwVidBitrate  = m_ptMtTable->GetDialBitrate(tSrc.GetMtId());
	

	//直接把音视频带宽返回给上级，由上级根据SpyMode决定使用
	tPreSetInRsp.SetAudSpyBW(dwAudBitrate);
	tPreSetInRsp.SetVidSpyBW(dwVidBitrate);

	//zjl20101116PreSetInAck不带目的能力集
	TSimCapSet tNullCap;
	tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.SetSimCapset(tNullCap);

	//[2011/09/06/zhangli]如果砍掉了某一回传模式，这里传回错误号，以提示界面
	cMsg.SetErrorCode(wErrorCode);
	cMsg.SetEventId(MCU_MCU_PRESETIN_ACK);
	cMsg.CatMsgBody( (u8 *)&tPreSetInRsp, sizeof(tPreSetInRsp) );
	SendMsgToMt( cServMsg.GetSrcMtId(), MCU_MCU_PRESETIN_ACK, cMsg );

	return;
}



/*==============================================================================
类名        :  ProcMcuMcuSpyBWFull
功能        :  回传带宽满了，做相应的处理
参数说明	:  [IN] const CMessage *pcMsg
	           【out】swCanRelIndex可替换的多回传通道号，只对vcs自动画面合成该参数有效
			   【out】byInsertpos 请求presetin的终端应该放在vmpParam中的位置，只对vcs自动画面合成该参数会改变
返回值说明	:  void
备注		:  
-------------------------------------------------------------------------------
修改记录:  
日  期     版本          修改人          走读人          修改记录
20100730   4.6              pengjie                      多回传代码整理
==============================================================================*/
void CMcuVcInst::ProcMcuMcuSpyBWFull(const CMessage *pcMsg,TPreSetInRsp *ptPreSetInRsp,u16 wErrorCode /*= ERR_MCU_CONFSNDBANDWIDTHISFULL*/ )
{

	STATECHECK;
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	TPreSetInRsp tPreSetInRsp = *ptPreSetInRsp;
	TMt tSrc = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();

	u8 byLoop1 = 0,byLoop = 0;
	TConfMcInfo *ptMcInfo = NULL;
	TConfMtInfo *ptConfMtInfo = NULL;
	
	BOOL32 bInConf = FALSE,bInJoinedConf = FALSE;
	BOOL32 bAlarm = TRUE;
	u8 byMcuId = 0;

	//以下针对带宽满的消息特殊处理,主要是轮询的时候不要提示带宽已满
	switch(tPreSetInRsp.m_tSetInReqInfo.GetEvId())
	{
	case MCUVC_POLLING_CHANGE_TIMER:
		{
			//如果是会议轮询，并且回传带宽已满，为了速度考虑，直接跳过此终端。
			KillTimer(MCUVC_POLLING_CHANGE_TIMER);
			//zhouyiliang 20121225 如果只轮一个下级终端(或者只有一个下级终端能轮，别的终端没视频)，且带宽或bas不足，此时应该给个提示
			BOOL32 bOnlyPollOneMt = FALSE;
			TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
			u8 byPollIdx = m_tConfPollParam.GetCurrentIdx();
			// 20110413_miaoqingsong 得到下一个符合被轮询条件的终端
			TMtPollParam *ptNextPollMt = GetNextMtPolled(byPollIdx, tPollInfo); 
			if ( NULL != ptNextPollMt && !ptNextPollMt->IsNull() && !ptNextPollMt->IsLocal() && (TMt)*ptNextPollMt == tSrc )
			{
				bOnlyPollOneMt = TRUE;
			}
			if (!bOnlyPollOneMt)
			{
				ProcPollingChangeTimerMsg(pcMsg);
				return;
			}
			else
			{
				ProcStopConfPoll();
				LogPrint(LOG_LVL_ERROR,MID_MCU_SPY,"[ProcMcuMcuSpyBWFull]Only one submt in poll,and not enough spy bandwith or bas!\n");
				bAlarm = TRUE;
				break;
				
			}

		}
		//break;
	case MCS_MCU_START_SWITCH_HDU_REQ:
		{
			TSpyHduInfo* ptSpyHduInfo = &tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyHduInfo;

			//如果是hdu电视墙批量轮询，直接返回
			if (ptSpyHduInfo->m_bySrcMtType == TW_MEMBERTYPE_BATCHPOLL)
			{
				return;
			}
			else if (ptSpyHduInfo->m_bySrcMtType == TW_MEMBERTYPE_TWPOLL)
			{
				//如果是电视墙单通道轮询,直接跳过此终端
				//20110610_tzy Bug00055651级联多回传会议，下级回传带宽已满的情况下，对下级的终端进行电视墙轮询，轮询顺利不对

				u32 dwTimerIdx = 0;
				if( m_tTWMutiPollParam.GetTimerIdx(ptSpyHduInfo->m_tHdu.GetEqpId(), ptSpyHduInfo->m_byDstChlIdx, dwTimerIdx) )
				{
					TTvWallPollParam *ptTWPollParam = m_tTWMutiPollParam.GetTWPollParamByTimerIdx((u8)dwTimerIdx);

					u8 byCurPollPos     = ptTWPollParam->GetCurrentIdx();					
					u8 byTWId           = ptTWPollParam->GetTvWall().GetEqpId();
					u8 byChnId          = ptTWPollParam->GetTWChnnl();
					u8 byIsStartAsPause = ptTWPollParam->GetIsStartAsPause();

					if ( 0 == byIsStartAsPause)   
					{
						byCurPollPos++;
					}
					else // 由暂停恢复时，接着当前终端轮询   
					{
						m_tTWMutiPollParam.SetIsStartAsPause(byTWId, byChnId, 0);
					}

					TMtPollParam *ptCurPollMt = GetMtTWPollParam(byCurPollPos, byTWId, byChnId);

					CMessage cMsg;
					if( NULL != ptCurPollMt && (TMt)(*ptCurPollMt) == tSrc )
					{						
						cServMsg.SetEventId( MCS_MCU_STOPHDUPOLL_CMD );
						cServMsg.SetMsgBody( &byTWId,sizeof(byTWId) );
						cServMsg.CatMsgBody( &byChnId,sizeof(byChnId) );
						cMsg.event = MCS_MCU_STOPHDUPOLL_CMD;
						cMsg.content = cServMsg.GetServMsg();
						cMsg.length  = cServMsg.GetServMsgLen();
						ProcMcsMcuHduPollMsg( &cMsg );
					}
					else
					{		

						KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
						CMessage cMsg;
						memset(&cMsg, 0, sizeof(cMsg));
						cMsg.event = u16(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
						cMsg.content = (u8*)&dwTimerIdx;
						cMsg.length  = sizeof(u32);
						ProcTWPollingChangeTimerMsg(&cMsg);
						return;
					}
				}
			}
			//20101111_tzy VCS预览模式下都要将该终端设置到相应电视墙中				
			SetMtInTvWallAndHduInFailPresetinAndInReviewMode( tPreSetInRsp );
			
		}
		break;
	case MCS_MCU_START_SWITCH_TW_REQ:
		{

			TSpyTVWallInfo* ptSpyTvWallInfo = &tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyTVWallInfo;
			
			if (ptSpyTvWallInfo->m_bySrcMtType == TW_MEMBERTYPE_TWPOLL)
			{
				//如果是电视墙单通道轮询,直接跳过此终端
				//20110610_tzy Bug00055651级联多回传会议，下级回传带宽已满的情况下，对下级的终端进行电视墙轮询，轮询顺利不对
				u32 dwTimerIdx = 0;
				if( m_tTWMutiPollParam.GetTimerIdx(ptSpyTvWallInfo->m_tTvWall.GetEqpId(), ptSpyTvWallInfo->m_byDstChlIdx, dwTimerIdx) )
				{
					TTvWallPollParam *ptTWPollParam = m_tTWMutiPollParam.GetTWPollParamByTimerIdx((u8)dwTimerIdx);

					u8 byCurPollPos     = ptTWPollParam->GetCurrentIdx();					
					u8 byTWId           = ptTWPollParam->GetTvWall().GetEqpId();
					u8 byChnId          = ptTWPollParam->GetTWChnnl();
					u8 byIsStartAsPause = ptTWPollParam->GetIsStartAsPause();
					
					

					if ( 0 == byIsStartAsPause)   
					{
						byCurPollPos++;
					}
					else // 由暂停恢复时，接着当前终端轮询   
					{
						m_tTWMutiPollParam.SetIsStartAsPause(byTWId, byChnId, 0);
					}

					TMtPollParam *ptCurPollMt = GetMtTWPollParam(byCurPollPos, byTWId, byChnId);

					CMessage cMsg;
					if( NULL != ptCurPollMt && (TMt)(*ptCurPollMt) == tSrc )
					{						
						cServMsg.SetEventId( MCS_MCU_STOPTWPOLL_CMD );
						cServMsg.SetMsgBody( &byTWId,sizeof(byTWId) );
						cServMsg.CatMsgBody( &byChnId,sizeof(byChnId) );
						cMsg.event = MCS_MCU_STOPTWPOLL_CMD;
						cMsg.content = cServMsg.GetServMsg();
						cMsg.length  = cServMsg.GetServMsgLen();
						ProcMcsMcuTWPollMsg( &cMsg );
					}
					else
					{					
						KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
						CMessage cMsg;
						memset(&cMsg, 0, sizeof(cMsg));
						cMsg.event = u16(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
						cMsg.content = (u8*)&dwTimerIdx;
						cMsg.length  = sizeof(u32);
						ProcTWPollingChangeTimerMsg(&cMsg);
						return;
					}
				}

				
			}
			
			//20101111_tzy VCS预览模式下都要将该终端设置到相应电视墙中				
			SetMtInTvWallAndHduInFailPresetinAndInReviewMode( tPreSetInRsp );	
		}
		break;

	case MCS_MCU_STARTVMP_REQ:
		{
			TEqp tVmpEqp = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_tVmp;
			// ALLVMPPRESETIN_ACK_TIMER失效
			u8 byVmpIdx = tVmpEqp.GetEqpId() - VMPID_MIN;
			KillTimer(MCUVC_WAIT_ALLVMPPRESETIN_ACK_TIMER+byVmpIdx);


			TPeriEqpStatus tPeriEqpStatus;
			g_cMcuVcApp.GetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
			if( TVmpStatus::RESERVE == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState )
			{
				tPeriEqpStatus.SetConfIdx( 0 );
				tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::IDLE;
				g_cMcuVcApp.SetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
			}		
			

			TVmpChnnlInfo tVmpChnnlInfo = g_cMcuVcApp.GetVmpChnnlInfo(tVmpEqp);
			// 清此终端占用的前适配,AdjustVmpParam处理会Getmtformat,若为下级非科达MT,可能会先占前适配,带宽满要清理
			tVmpChnnlInfo.ClearChnlByMt(tSrc);
			
			g_cMcuVcApp.GetPeriEqpStatus(tVmpEqp.GetEqpId() , &tPeriEqpStatus);
			TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
			TVMPParam_25Mem tEqpStatusVmpParam = tPeriEqpStatus.m_tStatus.tVmp.GetVmpParam();// = g_cMcuVcApp.GetLastVmpParam(m_tVmpEqp);
			//zhouyiliang20100910，加个保护，确保param中没有事先设置进去带宽不满足的presetin成员
			if ( tConfVmpParam.IsMtInMember( tSrc ) )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMcuMcuSpyBWFull] tSrc is a member of tConfVmpParam. \n");
			}
			if (tEqpStatusVmpParam.IsMtInMember( tSrc ))
			{
				u8 byChl = tEqpStatusVmpParam.GetChlOfMtInMember( tSrc );	
				//zhouyiliang 20100902 如果带宽满了，恢复curUsechn和老style
				TVMPMember *pVmpMember = tConfVmpParam.GetVmpMember(byChl);
				
				if (pVmpMember != NULL)
				{
					tEqpStatusVmpParam.SetVmpMember(byChl, *pVmpMember);
				}
				else
				{
					tEqpStatusVmpParam.ClearVmpMember( byChl );
				}
			}
			
			// vmp单通道轮询时带宽已满,立即轮询下一终端
			if (POLL_STATE_NONE != m_tVmpPollParam.GetPollState() && 
				VMP_MEMBERTYPE_VMPCHLPOLL == tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byMemberType)
			{
				TVmpPollInfo tPollInfo = m_tVmpPollParam.GetVmpPollInfo();
				u8 byPollIdx = m_tVmpPollParam.GetCurrentIdx();
				TMtPollParam * ptCurPollMt = GetNextMtPolledForVmp(byPollIdx, tPollInfo);
				// 若要轮询的下一个终端就是当前终端,表示仅有此终端满足轮询条件,但此终端又不适合进vmp,停轮询
				if (!ptCurPollMt || tSrc == *ptCurPollMt )
				{
					ProcStopVmpPoll();
				} else
				{
					// 若此通道为vmp单通道轮询时,需要立即轮询下一终端
					SetTimer(MCUVC_VMPPOLLING_CHANGE_TIMER, 10);
					bAlarm = FALSE;
				}
			}

			if ( tConfVmpParam.IsVMPAuto() )
			{
				u8 byStyl = tConfVmpParam.GetVMPStyle();
				tEqpStatusVmpParam.SetVMPStyle( byStyl );
			}
			//恢复presetin之前的curUseVmpChanInd，如本来要变4画面，通道满，只能3画面，curUseVmpChan就不再是1，而是原来的3了
			if ( VCS_CONF == m_tConf.GetConfSource() ) 
			{
				tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tEqpStatusVmpParam);
				g_cMcuVcApp.SetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
				u16 byCurUseChnId = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos;
				m_cVCSConfStatus.SetCurUseVMPChanInd( byCurUseChnId );
				TMt tMtNull;
				tMtNull.SetNull();
				m_cVCSConfStatus.SetReqVCMT( tMtNull );
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMcuMcuSpyBWFull] Update vcs CurUseVMPChanInd.%d:\n",byCurUseChnId);
			}
		
			cServMsg.SetEqpId(tVmpEqp.GetEqpId());
			cServMsg.SetMsgBody( (u8*)&tConfVmpParam, sizeof(tConfVmpParam) );
			SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg ); //告诉会控刷界面		
			
			if (m_tConf.m_tStatus.GetRollCallMode() == ROLLCALL_MODE_VMP)
			{
				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_SPY,"[ProcMcuMcuSpyBWFull]Recover Rollcall finish Mark\n");
				SetLastMultiSpyRollCallFinish(TRUE);
			}

// 			TPeriEqpStatus tOldStatus;
// 			g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tOldStatus );
// 			tOldStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
// 			//如果画面合参数里面没有一个成员，将vmp停掉
// 			if (m_tConf.m_tStatus.m_tVMPParam.GetVMPMemberNum() == 0) 
// 			{
// 				CServMsg cTempServ;
// 				SendMsgToEqp(m_tVmpEqp.GetEqpId(), MCU_VMP_STOPVIDMIX_REQ, cTempServ); 
// 				//放弃画面合成器
// 				tOldStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::IDLE;
// 				tOldStatus.SetConfIdx( 0 );
// 				if (m_tConf.m_tStatus.GetVmpParam().IsVMPBrdst())
// 				{
// 					ChangeVidBrdSrc(NULL);
// 				}
// 			}
// 			g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tOldStatus );
// 			cServMsg.SetMsgBody((u8 *)&tOldStatus, sizeof(tOldStatus));
// 			SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
		}
		break;

	case MT_MCU_STARTSELMT_CMD:			 //标准选看命令
	case MT_MCU_STARTSELMT_REQ:          
	case MCS_MCU_STARTSWITCHMT_REQ:	     //控制台要求交换命令
	case MCS_MCU_STARTSWITCHMC_REQ:      //会控选看终端
		{
			break;
		}
	case VCS_MCU_CONFSELMT_CMD:
		{
			break;
		}
	case MCS_MCU_SPECSPEAKER_REQ:        //会议控制台指定一台终端发言
	case MT_MCU_SPECSPEAKER_REQ:		 //普通终端指定发言人请求
	case MT_MCU_SPECSPEAKER_CMD:
		{
			if( NULL != ptPreSetInRsp &&
				VCS_CONF == m_tConf.GetConfSource() &&
				MCS_MCU_SPECSPEAKER_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() &&
				CONF_SPEAKMODE_ANSWERINSTANTLY == m_tConf.GetConfSpeakMode() &&
				!m_tApplySpeakQue.IsQueueNull()
				)
			{				
				TMt tVCMT;
				TMt tCurMt = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
				if(  m_tApplySpeakQue.GetQueueNextMt( tCurMt,tVCMT ) )
				{
					ChgCurVCMT( tVCMT );	
				}
				else
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuSpyBWFull] Fail to get Queue NextMt.CurMt(%d.%d)\n",
						tCurMt.GetMcuId(),tCurMt.GetMtId()
						);
				}
			}
			break;
		}

	case MCS_MCU_STARTREC_REQ:
		{
			// 针对vrs新录播，需挂断vrs实体
			u8 byVrsMtId = GetVrsRecMtId(tSrc);
			if (byVrsMtId > 0)
			{
				if (m_ptMtTable->GetRecChlType(byVrsMtId) == TRecChnnlStatus::TYPE_RECORD
					&& m_ptMtTable->GetRecChlState(byVrsMtId) != TRecChnnlStatus::STATE_IDLE)
				{
					ReleaseVrsMt(byVrsMtId);
				}
			}
			break;
		}

		
	case MCS_MCU_CHANGEROLLCALL_REQ:
	//点名失败,恢复被点名人
		if( MCS_MCU_CHANGEROLLCALL_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() )
		{
			LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_SPY,"[ProcMcuMcuSpyBWFull]Recover Rollcall finish Mark\n");
			SetLastMultiSpyRollCallFinish(TRUE);
			m_tRollCallee = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyRollCallInfo.m_tOldCallee;
			//zhouyiliang 20110118 点名下级终端,下级回ack,回传带宽满，LastRollCallFinished也要置回true，期间ROLLCALL_MODE_VMP模式不允许
			//再切换被点名人，防止时序问题
			if (m_tConf.m_tStatus.GetRollCallMode() == ROLLCALL_MODE_VMP) 
			{
// 				m_tConfInStatus.SetLastVmpRollCallFinished(TRUE);
				//zhouyiliang 20110330如果第一次点名就不成功，此时应该将点名人也清空，conf的vmpparam默认memset为0
				TVMPParam_25Mem tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
				if ( tConfVmpParam.GetVMPStyle() == VMP_STYLE_NONE || 
					 tConfVmpParam.GetVMPStyle() == 0
					) 
				{
					m_tRollCaller.SetNull();
				}
			}
		}
		break;
		
	default:
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuSpyBWFull] Default Msg.%d %s!\n", tPreSetInRsp.m_tSetInReqInfo.GetEvId() ,OspEventDesc(u16(tPreSetInRsp.m_tSetInReqInfo.GetEvId())));
		}
		break;
		//return;
	}

	if( bAlarm )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuSpyBWFull] SendErrorAlarm to UI,error:%d\n", wErrorCode);

		cServMsg.SetEventId( MCU_MCS_ALARMINFO_NOTIF );
		//[2011/10/26/zhangli]根据错误号做相关提示
		cServMsg.SetErrorCode( wErrorCode );	
		SendMsgToAllMcs( MCU_MCS_ALARMINFO_NOTIF, cServMsg );
	}

	OnPresetinFailed( tSrc );	
}

/*==============================================================================
函数名    :  ProcMcuMcuPreSetInAck
功能      :  上级处理下级发来的PreSetInAck，在这里主要是分配回传带宽，回传通道等
算法实现  :  
参数说明  :  [IN] const CMessage
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010/07/19	 4.6		   陆昆朋						   多回传整理
==============================================================================*/
void CMcuVcInst::ProcMcuMcuPreSetInAck( const CMessage *pcMsg )
{
	STATECHECK;
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	CServMsg cMsg;
	
	TPreSetInRsp tPreSetInRsp = *(TPreSetInRsp *)(cServMsg.GetMsgBody());
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody()+sizeof(TPreSetInRsp));
	TMt tOrgMt = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	TMt tSrc = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	u8  bySpyMode = tPreSetInRsp.m_byRspSpyMode;
	u8  byIsNeedAck = TRUE;
	BOOL32 bIsPreAdd = FALSE;

	TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
	if(tConfAttrb.IsResendLosePack())
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[ProcMcuMcuPreSetInAck][AUDIO	RTCP-ACK][entry of function]%s@%d\n", StrOfIP(tPreSetInRsp.GetAudSpyRtcpAddr().GetIpAddr()),
			tPreSetInRsp.GetAudSpyRtcpAddr().GetPort());
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[ProcMcuMcuPreSetInAck][VIDEO RTCP-ACK]%s@%d\n", StrOfIP(tPreSetInRsp.GetVidSpyRtcpAddr().GetIpAddr()),
						tPreSetInRsp.GetVidSpyRtcpAddr().GetPort());
	}

	if( !m_tCascadeMMCU.IsNull() &&
		tHeadMsg.m_tMsgSrc.m_byCasLevel < MAX_CASCADELEVEL &&
		tHeadMsg.m_tMsgDst.m_byCasLevel > 0 )
	{
		bIsPreAdd = TRUE;
	}

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[ProcMcuMcuPreSetInAck] bySpyMode.%d EventDesc(%s)!\n",bySpyMode,
		OspEventDesc(u16(tPreSetInRsp.m_tSetInReqInfo.GetEvId()))
		);

	//容错判断回传源非NULL
	if( tSrc.IsNull() )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "ProcMcuMcuPreSetInAck False, tSrc is NULL!\n" );
		return;
	}

	//容错判断回传模式非MODE_NONE
	if( bySpyMode == MODE_NONE )
	{
        ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "ProcMcuMcuPreSetInAck False, SpyMode == MODE_NONE!\n" );
		return;
	}

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[ProcMcuMcuPreSetInAck]The MMcu Have SpyVidBW: %d, SpyAudBW: %d, SpyMode: %d\n", tPreSetInRsp.GetVidSpyBW(), tPreSetInRsp.GetAudSpyBW(), bySpyMode );
	//获取此mcu支持的总带宽及剩余带宽
	u32 dwMaxSpyBW = 0;
	s32 nRemainSpyBW = 0;

	//zjj20100804 要把mtadp传过来的TMt转换成McuIdx表示的TMt
	u16 wSrcMcuIdx = GetMcuIdxFromMcuId( (u8)tSrc.GetMcuId() );	
	tSrc.SetMcuIdx( wSrcMcuIdx );	

	m_ptConfOtherMcTable->GetMultiSpyBW(tSrc.GetMcuId(), dwMaxSpyBW, nRemainSpyBW);
	tSrc = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc, tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt() );
	tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.SetSpyMt( tSrc );

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[ProcMcuMcuPreSetInAck]Before Mcu.%u dwMaxSpyBW:%d, nRemainSpyBW:%d\n", 
		tSrc.GetMcuId(), dwMaxSpyBW,nRemainSpyBW ); 

	//zhouyiliang 20121221 下级bas资源不足，可能将both模式减为audio模式，这时发言人认为调度不成功
	if ( tPreSetInRsp.m_tSetInReqInfo.m_bySpyMode != bySpyMode &&  
		( MCS_MCU_SPECSPEAKER_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() ||
		MT_MCU_SPECSPEAKER_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() || 
		MT_MCU_SPECSPEAKER_CMD == tPreSetInRsp.m_tSetInReqInfo.GetEvId()
		)
		)
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "ProcMcuMcuPreSetInAck SpecSpeakerReq , ackSpyMode == %d,reqspymod=%d!\n", bySpyMode,tPreSetInRsp.m_tSetInReqInfo.m_bySpyMode);
		OnMMcuBanishSpyCmd( tSrc, bySpyMode, bySpyMode );
		//预占用表示还需要把nack消息发往上级，消息还没发送到源头
		if( bIsPreAdd )
		{
			tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.SetSpyMt(tOrgMt);
			cMsg.SetErrorCode( ERR_MCU_CASDBASISNOTENOUGH );
			cMsg.SetEventId( MCU_MCU_PRESETIN_NACK );
			cMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
			cMsg.CatMsgBody( (u8*)&tPreSetInRsp,sizeof(TPreSetInRsp) );
			SendMsgToMt( m_tCascadeMMCU.GetMtId(),cMsg.GetEventId(),cMsg );
			
			return;
		}
		//[2011/10/26/zhangli]传入错误号，根据错误号做相关提示
		ProcMcuMcuSpyBWFull(pcMsg ,&tPreSetInRsp, ERR_MCU_CASDBASISNOTENOUGH);
		return;
	}

	s16 swIndex = m_cSMcuSpyMana.FindSpyMt(tSrc);
	CRecvSpy tSrcSpyInfo;
	//如果此回传源已经在回传列表中,且使用状态是待释放，就直接返回
	if (-1 != swIndex)
	{
		m_cSMcuSpyMana.GetRecvSpy(swIndex, tSrcSpyInfo);
		if( tSrcSpyInfo.m_byUseState == TSpyStatus::WAIT_FREE &&
			(  VCS_CONF != m_tConf.GetConfSource() ||
				 ( CONF_CREATE_MT != m_byCreateBy &&
						m_cVCSConfStatus.GetTVWallManageMode() != VCS_TVWALLMANAGE_AUTO_MODE
					 ) 
					||
				(
				 MCS_MCU_START_SWITCH_HDU_REQ != tPreSetInRsp.m_tSetInReqInfo.GetEvId() &&
				 MCS_MCU_START_SWITCH_TW_REQ != tPreSetInRsp.m_tSetInReqInfo.GetEvId() 
				)
				||
				tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_tMt.IsNull() || 
				tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.GetCount() < 0 ||
				GetFstMcuIdFromMcuIdx(tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMcuId()) != 
					tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_tMt.GetMcuId()
				)
			)
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuPreSetInAck] Fail to Presetin,SpySrcMt(%d.%d) Now Status  is WAIT_FREE!\n",
						tSrc.GetMcuId(),
						tSrc.GetMtId()
						);
			if( !m_tCascadeMMCU.IsNull() &&
					tHeadMsg.m_tMsgSrc.m_byCasLevel < MAX_CASCADELEVEL &&
					tHeadMsg.m_tMsgDst.m_byCasLevel > 0
					)
			{
				tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.SetSpyMt(tOrgMt);
				cMsg.SetEventId( MCU_MCU_PRESETIN_NACK );
				cMsg.SetErrorCode( ERR_MCU_CONFSNDBANDWIDTHISFULL );
				cMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
				cMsg.CatMsgBody( (u8*)&tPreSetInRsp,sizeof(TPreSetInRsp) );
				SendMsgToMt( m_tCascadeMMCU.GetMtId(),cMsg.GetEventId(),cMsg );
			}
			return;
		}
	}

	// [9/20/2011 liuxu] 电视墙业务特殊处理, 如果回传模式不包含视频,则拒绝
	if ( !bIsPreAdd 
		&& (MCS_MCU_START_SWITCH_TW_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId()
			|| MCS_MCU_START_SWITCH_HDU_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId()
			|| MCUVC_STARTSWITCHHDU_NOTIFY == tPreSetInRsp.m_tSetInReqInfo.GetEvId()))
	{
		if (bySpyMode != MODE_BOTH && bySpyMode != MODE_VIDEO)
		{
			OnMMcuBanishSpyCmd( tSrc, bySpyMode, bySpyMode );
			//[2011/10/26/zhangli]传入错误号，根据错误号做相关提示
			ProcMcuMcuSpyBWFull(pcMsg ,&tPreSetInRsp, cServMsg.GetErrorCode());
			return;
		}
	}

	//计算此次回传所需要的带宽
	s32 nTotalBW = 0;
	if (bySpyMode == MODE_BOTH || bySpyMode == MODE_VIDEO)
	{
		nTotalBW += (s32)tPreSetInRsp.GetVidSpyBW();
		//如果已经在回传，总共需要的回传要剪掉已经存在的回传带宽
		nTotalBW -= (s32)tSrcSpyInfo.m_dwVidBW;
	}

	if (bySpyMode == MODE_BOTH || bySpyMode == MODE_AUDIO)
	{
		nTotalBW += (s32)tPreSetInRsp.GetAudSpyBW();
		//如果已经在回传，总共需要的回传要剪掉已经存在的回传带宽
		nTotalBW -= (s32)tSrcSpyInfo.m_dwAudBW;
	}

	//评估一下此回传是否需要增加目的数及能释放多少带宽
	u32 dwCanReleaseBW = 0;
	u8 byAddDestSpyNum = 1;
	s16 swCanRelIndex = -1;
	
	//此次回传的带宽大于剩余带宽加可释放带宽，通知mcs下级回传带宽已满
	if( nTotalBW > nRemainSpyBW )
	{
		EvaluateSpyFromEvent(tPreSetInRsp, byAddDestSpyNum, dwCanReleaseBW, swCanRelIndex);

		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "ProcMcuMcuPreSetInAck McuId:%d MtId:%d AddDest:%d CanRBW:%d\n", 
		tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMcuId(),
		tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMtId(),
		byAddDestSpyNum, dwCanReleaseBW);

	
		if (nTotalBW > nRemainSpyBW + (s32)dwCanReleaseBW)
		{
			u8 bySpyNoUse = 0;
			bySpyNoUse |= bySpyMode;
			
			// [8/17/2011 liuxu] 已经在回传业务的,不需要释放
			if( -1 != swIndex )
			{
				if (tSrcSpyInfo.m_byVSpyDstNum)
				{
					bySpyNoUse &= ~MODE_VIDEO;
				}
				
				if (tSrcSpyInfo.m_byASpyDstNum)
				{
					bySpyNoUse &= ~MODE_AUDIO;
				}
			}				
						
			OnMMcuBanishSpyCmd( tSrc,tPreSetInRsp.m_tSetInReqInfo.m_bySpyMode, bySpyNoUse);
			
			//预占用表示还需要把nack消息发往上级，消息还没发送到源头
			if( bIsPreAdd )
			{
				tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.SetSpyMt(tOrgMt);
				cMsg.SetErrorCode( ERR_MCU_CONFSNDBANDWIDTHISFULL );
				cMsg.SetEventId( MCU_MCU_PRESETIN_NACK );
				cMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
				cMsg.CatMsgBody( (u8*)&tPreSetInRsp,sizeof(TPreSetInRsp) );
				SendMsgToMt( m_tCascadeMMCU.GetMtId(),cMsg.GetEventId(),cMsg );
				
				return;
			}
			ProcMcuMcuSpyBWFull(pcMsg ,&tPreSetInRsp);
			return;
			
		}
		else
		{
			tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byIsNeedRelease = 1;
		}

	}

	

	//如果下级已经将可释放终端所占的bas认为可重用,就停掉所有的监控,只有点名业务和vcs会涉及
	//确保和监控联动的主业务可以先释放可释放终端从而释放下级bas通道
	if( !bIsPreAdd &&
		MODE_NONE != tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byIsReuseBasChl &&
		!tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_tMt.IsNull() )
	{
		TMultiCacMtInfo tMtInfo;
		tMtInfo.m_byCasLevel = tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCasLevel;
		memcpy( &tMtInfo.m_abyMtIdentify[0],
			&tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_abyMtIdentify[0],
			sizeof(tMtInfo.m_abyMtIdentify)
			);

		TMt tCanReleaseMt = GetMtFromMultiCascadeMtInfo( tMtInfo,
						tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_tMt
						);		
		
		if( MCS_MCU_CHANGEROLLCALL_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() 
			|| VCS_CONF == m_tConf.GetConfSource() )
		{
			s16 swIndexRelMt = m_cSMcuSpyMana.FindSpyMt(tCanReleaseMt/*tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt()*/);
				
			if (-1 != swIndexRelMt)
			{
				CRecvSpy tRelMtSrcSpyInfo;
				m_cSMcuSpyMana.GetRecvSpy(swIndexRelMt, tRelMtSrcSpyInfo);
				if( tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum < tRelMtSrcSpyInfo.m_byVSpyDstNum ||
					tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum < tRelMtSrcSpyInfo.m_byASpyDstNum)
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuPreSetInAck]Fail Spy Mt.Smcu(%d) is reuse bas chl,but mt(%d.%d) DstNum(V:%d,A:%d) is bigger than relMt(%d.%d.%d,level.%d) DstNum(V:%d,A:%d)!\n",
						cServMsg.GetSrcMtId(),tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMcuId(),
						tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMtId(), 
						tRelMtSrcSpyInfo.m_byVSpyDstNum,tRelMtSrcSpyInfo.m_byASpyDstNum,
						tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_tMt.GetMcuId(),
						tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_tMt.GetMtId(),						
						tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_abyMtIdentify[0],
						tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCasLevel,
						tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum,
						tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum);
					//[2011/11/21/zhangli]释放带宽，这里是因为适配资源不足导致业务失败，错误号填适配资源不足
					OnMMcuBanishSpyCmd(tSrc, tPreSetInRsp.m_tSetInReqInfo.m_bySpyMode, tPreSetInRsp.m_tSetInReqInfo.m_bySpyMode);
					ProcMcuMcuSpyBWFull(pcMsg ,&tPreSetInRsp, ERR_MCU_CASDBASISNOTENOUGH);
					return;
				}
			}
			
// 			u16 wMcIndex = 0;
// 			if (VCS_CONF == m_tConf.GetConfSource())
// 			{
// 				wMcIndex = m_cVCSConfStatus.GetCurSrcSsnId();
// 			}
// 			else
// 			{
// 				wMcIndex = g_cMcuVcApp.GetMcIns(tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyRollCallInfo.GetMcIp(), 
// 												tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyRollCallInfo.GetMcSSRC(),
// 												m_tConf.GetConfSource());
// 			}
			//u8 byMcIndex = VCS_CONF == m_tConf.GetConfSource() ? m_cVCSConfStatus.GetCurSrcSsnId() : tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyRollCallInfo.m_byMcInstId;
		
			TMt tMcSrc;	
			BOOL32 bIsNeedReGetBW = FALSE;
// 			for( u8 byLoop = 0; byLoop < MAXNUM_MC_CHANNL; byLoop++ ) 
// 			{
// 				if ( g_cMcuVcApp.GetMcSrc( wMcIndex, &tMcSrc, byLoop, MODE_VIDEO ) ) 
// 				{
// 					if( !tMcSrc.IsNull() &&  ( tMcSrc == tCanReleaseMt/*tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt()*/ ) )
// 					{
// 						StopSwitchToMc( wMcIndex,byLoop,TRUE,MODE_VIDEO );		
// 						bIsNeedReGetBW = TRUE;
// 					}
// 				}
// 			}		
			//[2011/11/24/zhangli]多个vcs连同一mcu，需要停所有vcs的监控
			for (u16 byMcIndex = MAXNUM_MCU_VC; byMcIndex <= MAXNUM_MCU_MC + MAXNUM_MCU_VC; ++byMcIndex)
			{
				for( u8 byLoop = 0; byLoop < MAXNUM_MC_CHANNL; byLoop++ ) 
				{
					if ( g_cMcuVcApp.GetMcSrc(byMcIndex, &tMcSrc, byLoop, MODE_VIDEO)) 
					{
						if(!tMcSrc.IsNull() &&  (tMcSrc == tCanReleaseMt))
						{
							StopSwitchToMc(byMcIndex, byLoop, TRUE, MODE_VIDEO);
							bIsNeedReGetBW = TRUE;
						}
					}
					
					if ( g_cMcuVcApp.GetMcSrc(byMcIndex, &tMcSrc, byLoop, MODE_AUDIO)) 
					{
						if(!tMcSrc.IsNull() &&  (tMcSrc == tCanReleaseMt))
						{
							StopSwitchToMc(byMcIndex, byLoop, TRUE, MODE_AUDIO);
							bIsNeedReGetBW = TRUE;
						}
					}
				}
			}

			if( bIsNeedReGetBW )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuPreSetInAck] Stop All Mc And Need ReGetBW.tSrc(%d.%d) tCanReleaseMt(%d.%d)\n",
										tSrc.GetMcuId(),tSrc.GetMtId(),tCanReleaseMt.GetMcuId(),tCanReleaseMt.GetMtId() );
					
				GetMcuMultiSpyBW(tCanReleaseMt.GetMcuId(), dwMaxSpyBW, nRemainSpyBW);
			}
		}
		else
		{
			//zjj20110124 带宽足够,但同一下级的终端要复用已经上传的同一下级的可释放终端的占用的bas通道,如果无法释放可释放终端就不能上传成功
			if( GetFstMcuIdFromMcuIdx( tCanReleaseMt.GetMcuId() ) == GetFstMcuIdFromMcuIdx( tSrc.GetMcuId() ))
			{
				s16 swRelIndex = -1;
				u32 dwReleaseBW = 0;
				CRecvSpy cRecvSpy;
				if (m_cSMcuSpyMana.GetRecvSpy(tCanReleaseMt, cRecvSpy))
				{
					//[2011/11/18/zhangli]byReleaseBasMode可能有MODE_VIDEO、MODE_AUDIO、MODE_BOTH三个值，代表该模式下的bas可复用
					//只需判断某中模式下的bas可释放即可。场景：下级只有一路空闲适配器，
					//上-MT1选看下-MT1音频，上-MT2选看下-MT1音视频，视频过适配，然后上-MT2选看下-MT2无法替换选看
					u8 byReleaseBasMode = tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byIsReuseBasChl;
					u8 byReleaseVideoNum = tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCanReleaseVideoDstNum;
					u8 byReleaseAduioNum = tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCanReleaseAudioDstNum;
					if (MODE_VIDEO == byReleaseBasMode)
					{
						byReleaseAduioNum = cRecvSpy.m_byASpyDstNum;
					}
					else if (MODE_AUDIO == byReleaseBasMode)
					{
						byReleaseVideoNum = cRecvSpy.m_byVSpyDstNum;
					}
					
					m_cSMcuSpyMana.IsCanFree(tCanReleaseMt, tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCanReleaseMode, 
						byReleaseVideoNum, byReleaseAduioNum, dwReleaseBW, swRelIndex);
				}
				
				if( -1 == swRelIndex )
				{		
					ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, "[ProcMcuMcuPreSetInAck] Fai to Spy tSrc(%d.%d),because it cann't use tMt's(%d.%d) Bas\n",
										tSrc.GetMcuId(),tSrc.GetMtId(),tCanReleaseMt.GetMcuId(),tCanReleaseMt.GetMtId() );
					
					OnMMcuBanishSpyCmd( tSrc,tPreSetInRsp.m_tSetInReqInfo.m_bySpyMode, MODE_NONE );
					ProcMcuMcuSpyBWFull(pcMsg, &tPreSetInRsp, ERR_MCU_CASDBASISNOTENOUGH);					
					return;
				}
			}
		}
	}

	//如果此回传源已经在回传列表中
	if (-1 != swIndex)
	{
		//如果SpyMode跟之前的不一样
		if(tSrcSpyInfo.m_bySpyMode != bySpyMode)
		{
			//增加新的SpyMode
			m_cSMcuSpyMana.AddSpyMode(swIndex, bySpyMode, bIsPreAdd);
			//修改新的SpyMode对应的回传带宽
			m_cSMcuSpyMana.ModifySpyModeBW(swIndex, bySpyMode, tPreSetInRsp.GetVidSpyBW(), tPreSetInRsp.GetAudSpyBW());
		}
	}
	else
	{
		//如果有可以重用的通道就先置该通道状态为待释放(如有再对该端口的终端的多回传操作就拒绝)
		if( swCanRelIndex != -1)
		{
			m_cSMcuSpyMana.GetRecvSpy(swCanRelIndex, tSrcSpyInfo);
			
			//zjj20100810 不再重用端口
			m_cSMcuSpyMana.ModifyUseState( swCanRelIndex,TSpyStatus::WAIT_FREE );			
		}

		//如果此回传源不在此回传列表中,分配此回传源通道，并返回在m_cSMcuSpyMana中存放的index
		if (!AddSpyChnnlInfo(tSrc, swIndex, bIsPreAdd))
		{
			//此处定义ErrorCode，表示回传通道已经沾满，不能分配
			return;
		}

		//增加新的SpyMode
		m_cSMcuSpyMana.AddSpyMode(swIndex, bySpyMode, bIsPreAdd);
		//修改新的SpyMode对应的回传带宽
		m_cSMcuSpyMana.ModifySpyModeBW(swIndex, bySpyMode, tPreSetInRsp.GetVidSpyBW(), tPreSetInRsp.GetAudSpyBW());
	}

	//只有需要的用的带宽大于0，才需要更新剩余带宽
	//否则说明此终端已经在回传中，且大于或等于此次所需要的回传带宽
	if (nTotalBW > 0)
	{
		//更新一下剩余带宽，由于没有计算可能会释放的带宽，所以此处剩余带宽可能会是负数
		m_ptConfOtherMcTable->SetMcuSupMultSpyRemainBW(wSrcMcuIdx/*tSrc.GetMcuId()*/, nRemainSpyBW - (s32)nTotalBW);

		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "PreSetInAck:After Mcu.%u MaxSpyBW:%d, RemainSpyBW:%d\n", 
		tSrc.GetMcuId(), dwMaxSpyBW, nRemainSpyBW - (s32)nTotalBW ); 
	}

	//给回传终端对应的SpyMode终端目的数加一
	if (byAddDestSpyNum != 0)
	{
		m_cSMcuSpyMana.IncSpyModeDst(swIndex, bySpyMode, byAddDestSpyNum, bIsPreAdd);
	}

	if( !m_tCascadeMMCU.IsNull() &&
		tHeadMsg.m_tMsgSrc.m_byCasLevel < MAX_CASCADELEVEL &&
		tHeadMsg.m_tMsgDst.m_byCasLevel > 0
		)
	{
		tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.SetSpyMt(tOrgMt);
		//[2011/10/27/zhangli]把错误号返回给上级
		cMsg.SetErrorCode(cServMsg.GetErrorCode());
		cMsg.SetEventId( MCU_MCU_PRESETIN_ACK );
		cMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
		cMsg.CatMsgBody( (u8*)&tPreSetInRsp,sizeof(TPreSetInRsp) );
		SendMsgToMt( m_tCascadeMMCU.GetMtId(),cMsg.GetEventId(),cMsg );
		return;
	}

	//处理上级回传剩余逻辑
	ProcUnfinishedCascadEvent( tPreSetInRsp );

	//[2011/09/06]如果适配资源不足，给以提示
	if(ERR_MCU_CASDBASISNOTENOUGH == cServMsg.GetErrorCode())
	{
		NotifyMcsAlarmInfo(0, ERR_MCU_CASDBASISNOTENOUGH);
	}

	if ( VCS_CONF == m_tConf.GetConfSource() 
		&& ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode())
		&& (tPreSetInRsp.m_tSetInReqInfo.GetEvId() == VCS_MCU_CONFSELMT_CMD
			|| ( tPreSetInRsp.m_tSetInReqInfo.GetEvId() == MCS_MCU_STARTVMP_REQ 
				&& ( m_cVCSConfStatus.GetCurVCMode() != VCS_GROUPROLLCALL_MODE || VCS_POLL_STOP != m_cVCSConfStatus.GetChairPollState() ))
		|| tPreSetInRsp.m_tSetInReqInfo.GetEvId() == MCS_MCU_SPECSPEAKER_REQ)
		&& !( tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt() == m_cVCSConfStatus.GetCurVCMT() )	
		)
	{
		//切换当前调度终端
		TMt tNewCurVCMT = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
		TMt tCurVCMT = m_cVCSConfStatus.GetCurVCMT();
		u8 byVCMode = m_cVCSConfStatus.GetCurVCMode();
		m_cVCSConfStatus.SetCurVCMT(tNewCurVCMT);

		//20110610_tzy Bug00055645 PDS：VCS级联多回传，配置5个电视墙，回传带宽为6路，电视墙预览模式只调度下级终端，依次调度，会提示回传带宽满。
		//预览模式下，电视墙通道数加上调度终端等于最大回传路数时，新调度终端调度上来后，
		//应将老调度终端在监控中的视频清理一遍，否则老调度终端未释放干净，导致有的终端进墙失败
		if ( m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_REVIEW_MODE )
		{
			for( u8 byMcId = MAXNUM_MCU_MC+1;byMcId<= MAXNUM_MCU_MC + MAXNUM_MCU_VC;++byMcId )
			{
				TMt tMcSrc;
				g_cMcuVcApp.GetMcSrc( byMcId, &tMcSrc, VCS_VCMT_MCCHLIDX , MODE_VIDEO );
				if( tMcSrc == tCurVCMT )
				{				
					StopSwitchToMc( byMcId, VCS_VCMT_MCCHLIDX, TRUE, MODE_VIDEO );
				}
			}
		}

		//zhouyiliang 20101213 现remove再Add，防止两个一样的情况出现时add不成功
		// [9/29/2011 liuxu] 移到电视墙操作之前, 这样避免了电视墙操作在做presetin时导致进墙又出墙的情况
		if( m_cVCSConfStatus.GetCurVCMode() != VCS_GROUPVMP_MODE 
			&& !tCurVCMT.IsNull() 
			&& ( !m_cVCSConfStatus.GetMtInTvWallCanMixing() || 
			( IsMtNotInOtherHduChnnl(tCurVCMT,0,0) && IsMtNotInOtherTvWallChnnl(tCurVCMT,0,0) ) )
			)
		{					
			RemoveSpecMixMember( &tCurVCMT, 1,FALSE,FALSE );
		}

		//zjj20101222 检查当前调度终端的回传状态，如果是WAIT_FREE说明本次要调度的终端tNewCurVCMT和当前调度终端是共回传路径的,
		//而且本次调度的终端在下级已经预占了当前调度终端的带宽，就等当前调度终端释放，所有在这种情况下当前调度终端在自动电视墙
		//模式下进墙是不可以的，因为要保证本次调度的终端先上来
		tSrcSpyInfo.Clear();
		swIndex = m_cSMcuSpyMana.FindSpyMt(tCurVCMT);
		if( -1 != swIndex )
		{
			m_cSMcuSpyMana.GetRecvSpy(swIndex, tSrcSpyInfo);

			if( m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_AUTO_MODE &&
				TSpyStatus::WAIT_FREE == tSrcSpyInfo.m_byUseState )
			{
				TEqp tEqp;
				TMt  tDropOutMt;
				tDropOutMt.SetNull();

				// [5/30/2011 liuxu] 重写了FindUsableTWChan, 用FindNextTvwChnnl代替
 				u8 byChanIdx = 0;
				CConfTvwChnnl cNextTvwChnnl;
				u16 wNextChnnlIdx = FindNextTvwChnnl( m_cVCSConfStatus.GetCurUseTWChanInd(), &tCurVCMT, &cNextTvwChnnl, FALSE );
				
				// Hdu设备能力检查
				BOOL32 bCheckHdu = IsValidHduChn( cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx() );
				bCheckHdu &= CheckHduAbility( tCurVCMT, cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx() );

				// 普通电视墙检查
				BOOL32 bCheckTvw = IsValidTvw( cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx() );
				
				// 获取旧通道成员和通道对应的设备
				tDropOutMt = (TMt)cNextTvwChnnl.GetMember();
				tEqp = g_cMcuVcApp.GetEqp( cNextTvwChnnl.GetEqpId() );
				byChanIdx = cNextTvwChnnl.GetChnnlIdx();

				if( wNextChnnlIdx && (bCheckHdu || bCheckTvw) )
				{
					TVMPParam_25Mem tVmpparm = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
					if( !tDropOutMt.IsNull() 
						&& !( tDropOutMt == m_cVCSConfStatus.GetCurVCMT() ) 
						&& ( m_cVCSConfStatus.GetCurVCMode() != VCS_GROUPVMP_MODE || !tVmpparm.IsMtInMember( tDropOutMt ) ) &&
							GetFstMcuIdFromMcuIdx( tDropOutMt.GetMcuId() ) == GetFstMcuIdFromMcuIdx( tCurVCMT.GetMcuId() )
					  ) 
					{
						TTWSwitchInfo tSwitchInfo;
						tSwitchInfo.SetMemberType(TW_MEMBERTYPE_VCSAUTOSPEC);
						tSwitchInfo.SetSrcMt(tCurVCMT);
						tSwitchInfo.SetDstMt(tEqp);
						tSwitchInfo.SetDstChlIdx(byChanIdx);
						tSwitchInfo.SetMode(MODE_BOTH);
						if (!tCurVCMT.IsNull())
						{
							VCSConfMTToTW(tSwitchInfo);						
						}
					}
					else
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,  "[ProcMcuMcuPreSetInAck] tCurVCMT(%d.%d) now spy useState is WAIT_FREE and now in autoTvWall Mode,so not switch to TW.\n",
							tCurVCMT.GetMcuId(),tCurVCMT.GetMtId()
							);
						//20110107_tzy Bug00045312自动电视墙回传带宽满时无提示
						cMsg.SetEventId( MCU_MCS_ALARMINFO_NOTIF );
						cMsg.SetErrorCode( ERR_MCU_CONFSNDBANDWIDTHISFULL );	
						SendMsgToAllMcs( MCU_MCS_ALARMINFO_NOTIF, cMsg );
					}
				}
			}			
		}

		if ( !tCurVCMT.IsNull() 
			&& m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_AUTO_MODE 
			&& ( //20101223_tzy VCS会议自动电视墙模式下原调度终端是本地则无条件进墙
				tCurVCMT.IsLocal() 
				// [11/3/2011 liuxu] 单回传也要进墙
				|| !IsLocalAndSMcuSupMultSpy( tCurVCMT.GetMcuId()) 
				//20101223_tzy 本次调度的终端如果非本级终端并且状态不是WAIT_FREE才进墙，否则说明新旧调度终端是共路径的原调度终端就不进墙
				|| (-1 != swIndex && tSrcSpyInfo.m_byUseState != TSpyStatus::WAIT_FREE )
				// [11/3/2011 liuxu] 多回传下如果旧调度终端已经不在回传通道里了,需要重新进墙
				|| (-1 == swIndex)
				)
			)
		{
			TEqp tEqp;
			TMt  tDropOutMt;
			tDropOutMt.SetNull();
			u8 byChanIdx = 0;

			CConfTvwChnnl cNextTvwChnnl;
			u16 wNextChnnlIdx = FindNextTvwChnnl( m_cVCSConfStatus.GetCurUseTWChanInd(), &tCurVCMT, &cNextTvwChnnl, FALSE );
			
			// Hdu设备能力检查
			BOOL32 bCheckHdu = IsValidHduChn( cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx() );
			bCheckHdu &= CheckHduAbility( tCurVCMT, cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx() );
			
			// 普通电视墙检查
			BOOL32 bCheckTvw = IsValidTvw( cNextTvwChnnl.GetEqpId(), cNextTvwChnnl.GetChnnlIdx() );
			
			// 获取旧通道成员和通道对应的设备
			tDropOutMt = (TMt)cNextTvwChnnl.GetMember();
			tEqp = g_cMcuVcApp.GetEqp( cNextTvwChnnl.GetEqpId() );
			byChanIdx = cNextTvwChnnl.GetChnnlIdx();

			if( wNextChnnlIdx && (bCheckHdu || bCheckTvw) )
			{
				if (!tDropOutMt.IsNull())
				{
					if (tEqp.GetEqpType() != EQP_TYPE_TVWALL)
					{
						// [2013/03/11 chenbing] VCS会议不支持HDU多画面,子通道置0
						ChangeHduSwitch( NULL,tEqp.GetEqpId(),byChanIdx,0, TW_MEMBERTYPE_VCSAUTOSPEC, TW_STATE_STOP,MODE_BOTH,FALSE,FALSE );
					}
					else
					{
						ChangeTvWallSwitch( &tDropOutMt,tEqp.GetEqpId(),byChanIdx,TW_MEMBERTYPE_VCSAUTOSPEC, TW_STATE_STOP);
					}
				}

				if (!tCurVCMT.IsNull())
				{
					TTWSwitchInfo tSwitchInfo;
					tSwitchInfo.SetMemberType(TW_MEMBERTYPE_VCSAUTOSPEC);
					tSwitchInfo.SetSrcMt(tCurVCMT);
					tSwitchInfo.SetDstMt(tEqp);
					tSwitchInfo.SetDstChlIdx(byChanIdx);
					tSwitchInfo.SetMode(MODE_BOTH);

					BOOL32 bInToTvwRet = VCSConfMTToTW(tSwitchInfo);

					if (bInToTvwRet 
						|| tCurVCMT.IsLocal() 
						// [8/17/2011 liuxu] 因能力检验不够没有进入墙时, 也需要
						|| !CheckHduAbility(tCurVCMT, tEqp.GetEqpId(), byChanIdx))
 					{
						m_cVCSConfStatus.SetCurUseTWChanInd(wNextChnnlIdx);
					}
				}
			}
		}
		
		AddSpecMixMember(&tNewCurVCMT, 1, TRUE);
	

		//如果调度终端在发言队列内，就要从发言队列内清除
		if( m_cVCSConfStatus.GetCurVCMT() == tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt() &&
			m_tApplySpeakQue.IsMtInQueue( tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt() )
			)
		{
			BOOL32 bIsSendToChairman = FALSE;
			if( !m_tApplySpeakQue.ProcQueueInfo( tSrc,bIsSendToChairman,FALSE ) )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,  "[ProcVmpStartPreSetInAck] Fail to pop tMt(%d.%d). \n",
							tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMcuId(),
							tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMtId()
							);
				bIsSendToChairman = FALSE;
			}
			NotifyMcsApplyList( bIsSendToChairman );
		}
		NotifyMtSpeakStatus( tNewCurVCMT, emAgreed );
		if( m_cVCSConfStatus.GetCurVCMode() != VCS_GROUPVMP_MODE )
		{
			NotifyMtSpeakStatus( tCurVCMT, emCanceled );
		}		
		TMt tNullMt;
		tNullMt.SetNull();

		KillTimer(MCUVC_VCMTOVERTIMER_TIMER);	
		m_cVCSConfStatus.SetReqVCMT(tNullMt);
		VCSConfStatusNotif();
	}
	
	return;
}


/*==============================================================================
函数名    :  AddSpyChnnlInfo
功能      :  添加回传通道
算法实现  :  
参数说明  :  const TMt &tMt [IN] 终端信息
			 s16 &swIndex	[OUT]返回添加后的回传信息索引
返回值说明:  是否添加成功
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010/07/19	 4.6		   陆昆朋						   多回传整理
==============================================================================*/
BOOL32 CMcuVcInst::AddSpyChnnlInfo(const TMt &tMt, s16 &swIndex,BOOL32 bIsPreAdd/* = FALSE*/ )
{
	swIndex = m_cSMcuSpyMana.FindSpyMt(tMt);

	//此处只是避免重复添加通道,如果真的已经在回传通道中，说明逻辑判断有错误
	if(-1 != swIndex)
	{
		return TRUE;
	}
	
	CMultiSpyMgr *pcMultiSpyMgr = g_cMcuVcApp.GetCMultiSpyMgr();
	u16 wSpyChannl;
	
	if( NULL == pcMultiSpyMgr )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "GetCMultiSpyMgr False! \n" );
		return FALSE;
	}
	if( !pcMultiSpyMgr->AssignSpyChnnl( wSpyChannl ) )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "AssignSpyChnnl False! \n" );
		return FALSE;
	}

	return m_cSMcuSpyMana.AddSpyChnnlInfo(tMt, wSpyChannl * PORTSPAN + CASCADE_SPY_STARTPORT, swIndex,bIsPreAdd);
}

/*==============================================================================
函数名    :  SendMMcuSpyNotify
功能      :  上级给下级分配回传信息的通知
算法实现  :  
参数说明  :  [IN] TPreSetInRsp 多回传必要信息
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010/07/19   4.6			陆昆朋							多回传整理
==============================================================================*/
void CMcuVcInst::SendMMcuSpyNotify( const TMt &tSpySrc, u32 dwEvId, TSimCapSet tDstCap/*u8 byRes = VIDEO_FORMAT_INVALID*/ )
{
	CServMsg cServMsg;
	cServMsg.SetEventId( MCU_MCU_SPYCHNNL_NOTIF );

	if (tDstCap.IsNull())
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "param is invalid, tDstCap is null!\n");
		return;
	}

	CRecvSpy tSrcSpyInfo;
	if( !m_cSMcuSpyMana.GetRecvSpy( tSpySrc, tSrcSpyInfo ) )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "GetSpyChannlInfo False! \n" );
		return;
	}
	
	//[2011/09/16/zhangli]如果是MP4-16CIF修正下发分辨率为4CIF
	if (MEDIA_TYPE_MP4 == tDstCap.GetVideoMediaType()
		&& VIDEO_FORMAT_16CIF == tDstCap.GetVideoResolution())
	{
		tDstCap.SetVideoResolution(VIDEO_FORMAT_4CIF);
	}
	//zjl20101116当前要求回传能力集与该下级终端已回传能力集取小
    if (!GetMinSpyDstCapSet(tSpySrc, tDstCap))
    {
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[SendMMcuSpyNotify] GetMinSpyDstCapSet failed!\n");
		return;
    }
	// 对最终决定的能力中的分辨率做调整
	u32 dwResW = 0;
	u32 dwResH = 0;
	u16 wTmpResW = 0;
	u16 wTmpResH = 0;
	u8 byRes = tDstCap.GetVideoResolution();
	// mpeg4 auto需要根据会议码率获得分辨率
	if ( VIDEO_FORMAT_AUTO == byRes )
	{
		byRes = GetAutoResByBitrate(byRes,m_tConf.GetBitRate());
	}
	// 对分辨率进行调整,可能下级是旧mcu,需发共通分辨率,真实分辨率宽高在最后追加
	GetWHByRes(byRes, wTmpResW, wTmpResH);

	dwResW = wTmpResW;
	dwResH = wTmpResH;
	// 根据分辨率,获得对应通用分辨率,新旧MCU都能识别的分辨率
	byRes = GetNormalRes(wTmpResW, wTmpResH);
	tDstCap.SetVideoResolution(byRes);

	//保存
	m_cSMcuSpyMana.SaveSpySimCap( tSpySrc, tDstCap);

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[SendMMcuSpyNotify] Cap<Media:%d, Res:%d, BR:%d, FPS:%d>-dwResWH[%d,%d]!\n",
						tDstCap.GetVideoMediaType(), tDstCap.GetVideoResolution(), tDstCap.GetVideoMaxBitRate(),
						tDstCap.GetUserDefFrameRate(), dwResW, dwResH);
	
	tSrcSpyInfo.SetSimCapset(tDstCap);

	TMsgHeadMsg tHeadMsg;
	tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tSrcSpyInfo.m_tSpy,tSrcSpyInfo.m_tSpy );

	TSpyResource tSpyResource;
	tSrcSpyInfo.GetSpyResource( tSpyResource );

	cServMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
	cServMsg.CatMsgBody( (u8 *)&tSpyResource, sizeof(TSpyResource) );
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "SendMMcuSpyNotify tSrcSpyInfo.%d Spymode.%d! \n",
		tSrcSpyInfo.m_tSpyAddr.GetPort(),tSrcSpyInfo.m_bySpyMode );
	
	// 转为网络序
	dwEvId = htonl(dwEvId);
	dwResW = htonl(dwResW);
	dwResH = htonl(dwResH);	
	cServMsg.CatMsgBody( (u8 *)&dwEvId, sizeof(u32) );
	cServMsg.CatMsgBody( (u8 *)&dwResW, sizeof(u32) );
	cServMsg.CatMsgBody( (u8 *)&dwResH, sizeof(u32) );

	SendMsgToMt( GetFstMcuIdFromMcuIdx(tSpySrc.GetMcuId()), MCU_MCU_SPYCHNNL_NOTIF, cServMsg );
}

/*==============================================================================
函数名    :  ProcUnfinishEvDeamon
功能      :  处理在多回传多次握手后，之前剩余的逻辑消息
算法实现  :  
参数说明  :  [IN] TPreSetInRsp 多回传必要信息
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-11-05                 pengjie							create
==============================================================================*/
void CMcuVcInst::ProcUnfinishedCascadEvent( const TPreSetInRsp &tPreSetInRsp )
{
	TMt tSrc = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	switch( tPreSetInRsp.m_tSetInReqInfo.GetEvId() )
	{
	case MT_MCU_STARTSELMT_CMD:			 //标准选看命令
	case MT_MCU_STARTSELMT_REQ:          
	case MCS_MCU_STARTSWITCHMT_REQ:	     //控制台要求交换命令
	case MCS_MCU_STARTSWITCHMC_REQ:      //会控选看终端
		ProcSelMtStartPreSetInAck( tPreSetInRsp );
		break;
	case VCS_MCU_START_MONITOR_UNION_REQ:
		ProcStartMonitorPreSetinAck(tPreSetInRsp);
		break;
	case MCS_MCU_STARTVMP_REQ:
		ProcVmpStartPreSetInAck( tPreSetInRsp );
		break;

	case MCS_MCU_SPECSPEAKER_REQ:        //会议控制台指定一台终端发言
	case MT_MCU_SPECSPEAKER_REQ:		 //普通终端指定发言人请求
	case MT_MCU_SPECSPEAKER_CMD:
		ProcSpeakerStartPreSetInAck( tPreSetInRsp );
		break;

	case MCS_MCU_START_SWITCH_HDU_REQ:     // 会控选择进高清电视墙
	case MCUVC_STARTSWITCHHDU_NOTIFY:
		ProcHduStartPreSetInAck( tPreSetInRsp );
		break;

	case MCUVC_POLLING_CHANGE_TIMER:       // 处理轮询
		ProcPollStartPreSetInAck( tPreSetInRsp );
		break;

	case MCS_MCU_START_SWITCH_TW_REQ:      // 会控选择进电视墙
		ProcTWStartPreSetInAck( tPreSetInRsp );  
		break;

    case MCS_MCU_CHANGEROLLCALL_REQ:       // 会议点名逻辑
		ProcRollCallPreSetInAck( tPreSetInRsp );
		break;

	case VCS_MCU_CONFSELMT_CMD:
		{
			if( VCS_POLL_STOP == m_cVCSConfStatus.GetChairPollState() )
			{
				TSwitchInfo tSwitchInfo;
				tSwitchInfo.SetSrcMt( tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt() );
				tSwitchInfo.SetDstMt( tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpySwitchInfo.m_tDstMt );
				tSwitchInfo.SetMode( tPreSetInRsp.m_byRspSpyMode );
				
				if(!VCSConfSelMT( tSwitchInfo,FALSE ))
				{
					// 主席模式选看下级终端失败 [7/19/2012 chendaiwei]
					FreeRecvSpy( tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt(),tPreSetInRsp.m_byRspSpyMode );
				}
			}
			else
			{
				//zjj20110222 如果因为正在本地轮询无法建交换到主席,就把刚才presetin所增加的目的数和带宽减去
				FreeRecvSpy( tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt(),tPreSetInRsp.m_byRspSpyMode );			
			}
			//如果调度终端在发言队列内，就要从发言队列内清除
			if( m_cVCSConfStatus.GetCurVCMT() == tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt() &&
				m_tApplySpeakQue.IsMtInQueue( tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt() )
				)
			{
				BOOL32 bIsSendToChairman = FALSE;
				if( !m_tApplySpeakQue.ProcQueueInfo( tSrc,bIsSendToChairman,FALSE ) )
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,  "[ProcVmpStartPreSetInAck] Fail to pop tMt(%d.%d). \n",
								tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMcuId(),
								tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt().GetMtId()
								);
					bIsSendToChairman = FALSE;
				}
				NotifyMcsApplyList( bIsSendToChairman );
			}
		}
		break;

	case MCS_MCU_STARTREC_REQ:
		{
			ProcRecStartPreSetInAck(tPreSetInRsp);
		}
		break;

	case MCS_MCU_SPECOUTVIEW_REQ:                   // 三级级联多回传会议中第二级会议拖三级与会终端进回传通道
		{
			ProcDragStartPreSetInAck( tPreSetInRsp );
			break;
		}

	default:
		ConfPrint(LOG_LVL_ERROR, MID_MCU_CONF, "Error: ProcUnfinishedCascadEvent event id is 0, may cause unusual error\n");
		break;
	}

	return;
}

/*==============================================================================
函数名    :  SendMMcuSpyBWFullNotify
功能      :  上级给下级的回传带宽不足的通知
算法实现  :  
参数说明  :  [IN] tSrcMt回传源，[IN] dwSpyBW回传带宽
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-10-15                   pengjie							create
==============================================================================*/
void CMcuVcInst::SendMMcuRejectSpyNotify( const TMt &tSrcMt, u32 dwSpyBW )
{
	CServMsg cServMsg;
	cServMsg.SetEventId( MCU_MCU_REJECTSPY_NOTIF );
	cServMsg.SetMsgBody( (u8 *)&tSrcMt, sizeof(TMt) );
	cServMsg.CatMsgBody( (u8 *)&dwSpyBW, sizeof(u32) );

	SendMsgToMt( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()), MCU_MCU_REJECTSPY_NOTIF, cServMsg );
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "MMcu SendMMcuRejectSpyNotify! \n" );

	return;
}

/*==============================================================================
函数名    :  ProcStartMonitorPreSetinAck
功能      :  处理未处理完的监控联动逻辑
算法实现  :  
参数说明  :  [IN] TPreSetInRsp 多回传必要信息
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010-08-28                 薛亮								create
==============================================================================*/
void CMcuVcInst::ProcStartMonitorPreSetinAck(const TPreSetInRsp &tPreSetInRsp )
{
	TMt tSrcMt    = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	
	TSpySwitchDstInfo tSpySwitchDstInfo = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpySwitchDstInfo;

	u8 byOldSpyMode = tPreSetInRsp.m_tSetInReqInfo.m_bySpyMode;
	u8 bySpyMode = tPreSetInRsp.m_byRspSpyMode;

	TSwitchDstInfo tSwitchDstInfo;
	tSwitchDstInfo.m_byMode = tPreSetInRsp.m_byRspSpyMode;
	tSwitchDstInfo.m_bySrcChnnl = tSpySwitchDstInfo.m_bySrcChnnl;
	tSwitchDstInfo.m_tSrcMt = tSrcMt;
	tSwitchDstInfo.m_tDstVidAddr = tSpySwitchDstInfo.m_tDstVidAddr;
	tSwitchDstInfo.m_tDstAudAddr = tSpySwitchDstInfo.m_tDstAudAddr;
	
	u16 wMcInsId = g_cMcuVcApp.GetMcIns(tSpySwitchDstInfo.GetMcIp(), 
										tSpySwitchDstInfo.GetMcSSRC(),
										m_tConf.GetConfSource());
	if (0 == wMcInsId)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_CONF, "[ProcStartMonitorPreSetinAck] GetMcIns from VcsIp.%s McSSrc.%x failed!\n",
								   StrOfIP(tSpySwitchDstInfo.GetMcIp()),
								   tSpySwitchDstInfo.GetMcSSRC());
	}
	else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcStartMonitorPreSetinAck] GetMcIns.%d success from VcsIp.%s McSSrc.%x!\n",
													wMcInsId, StrOfIP(tSpySwitchDstInfo.GetMcIp()),
													tSpySwitchDstInfo.GetMcSSRC());
	}

	//[2011/10/26/zhangli]要求回传与实际回传不一致，先停掉旧的监控
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY, "[ProcStartMonitorPreSetinAck] Presetin-Mode:%d, PresetinAck-Mode:%d\n", byOldSpyMode, bySpyMode);

	TMt tOldSrc;
	TSwitchDstInfo tTempSwitchInfo;
	memcpy(&tTempSwitchInfo, &tSwitchDstInfo, sizeof(TSwitchDstInfo));
	
	if (MODE_BOTH == byOldSpyMode && MODE_VIDEO == bySpyMode)
	{
		if(g_cMcuVcApp.GetMonitorSrc(wMcInsId, MODE_AUDIO, tSwitchDstInfo.m_tDstAudAddr, &tOldSrc))
		{
			if(!tOldSrc.IsNull())
			{
				tTempSwitchInfo.m_tSrcMt = tOldSrc;
				tTempSwitchInfo.m_byMode = MODE_AUDIO;
				StopSwitchToMonitor(tTempSwitchInfo, wMcInsId);
			}
		}
	}
	//监控一定监控视屏，如果MODE_AUDIO == bySpyMode，不管byOldSpyMode则认为下级因资源不足把MODE_VIDEO去掉，这里释放老的MODE_VIDEO
	if (/*MODE_BOTH == byOldSpyMode && */MODE_AUDIO == bySpyMode)
	{
		tOldSrc.SetNull();
		if(g_cMcuVcApp.GetMonitorSrc(wMcInsId, MODE_VIDEO, tSwitchDstInfo.m_tDstVidAddr, &tOldSrc))
		{
			if(!tOldSrc.IsNull())
			{
				tTempSwitchInfo.m_tSrcMt = tOldSrc;
				tTempSwitchInfo.m_byMode = MODE_VIDEO;
				StopSwitchToMonitor(tTempSwitchInfo, wMcInsId);
			}
		}
	}

	StartSwitchToMonitor(tSwitchDstInfo, wMcInsId);
}


/*==============================================================================
函数名    :  ProcSelMtStartPreSetInAck
功能      :  处理未处理完的选看逻辑
算法实现  :  
参数说明  :  [IN] TPreSetInRsp 多回传必要信息
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-11-05                 pengjie							create
20100730                   pengjie	                        多回传代码整理
==============================================================================*/
void CMcuVcInst::ProcSelMtStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp )
{
	TMt tSrcMt    = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	u8  bySpyMode = tPreSetInRsp.m_byRspSpyMode;
	u8 byOldSpyMode = tPreSetInRsp.m_tSetInReqInfo.m_bySpyMode;
	u32 EvId      = tPreSetInRsp.m_tSetInReqInfo.GetEvId();
	TSpySwitchInfo tSpySwitchInfo = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpySwitchInfo;
	TMt tDstMt    = tSpySwitchInfo.m_tDstMt;

	CServMsg cServMsg;
	u16 wMcIns = g_cMcuVcApp.GetMcIns(tSpySwitchInfo.GetMcIp(), 
									  tSpySwitchInfo.GetMcSSRC(),
									  m_tConf.GetConfSource());
	if (0 == wMcIns)
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_CONF, "[ProcSelMtStartPreSetInAck] GetMcIns from McIp.%s McSSrc.%x failed!\n",
								  StrOfIP(tSpySwitchInfo.GetMcIp()), tSpySwitchInfo.GetMcSSRC());
	}
	else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_CONF, "[ProcSelMtStartPreSetInAck] GetMcIns McsSSn.%d success from McIp.%s McSSrc.%x!\n",
									 wMcIns, StrOfIP(tSpySwitchInfo.GetMcIp()), tSpySwitchInfo.GetMcSSRC());

		cServMsg.SetSrcSsnId((u8)wMcIns );
	}
	cServMsg.SetEventId( (u16)EvId );

	TSwitchInfo	tSwitchInfo;
	TMtStatus tDstMtStatus;

 	tSwitchInfo.SetSrcMt( tSrcMt );
 	tSwitchInfo.SetDstMt( tDstMt );
 	tSwitchInfo.SetMode( bySpyMode );
	tSwitchInfo.SetDstChlIdx( tSpySwitchInfo.m_byDstChlIdx );
	tSwitchInfo.SetSrcChlIdx( tSpySwitchInfo.m_bySrcChlIdx );


	//终端状态和选看模式不匹配, NACK
    // zgc, 2008-07-10, 修改判断方式, 当最终过滤结果为MODE_NONE时，拒绝选看，否则给出提示
	TMtStatus tSrcMtStatus;    
	u8 bySwitchMode = tSwitchInfo.GetMode();	// 记录初始MODE

	m_ptMtTable->GetMtStatus( GetLocalMtFromOtherMcuMt(tSrcMt).GetMtId(), &tSrcMtStatus);
    m_ptMtTable->GetMtStatus(tDstMt.GetMtId(), &tDstMtStatus);
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
	BOOL32 bVidAdp = FALSE;
	BOOL32 bAudAdp = FALSE;
    //终端选看
    if( TYPE_MT == tDstMt.GetType() )
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
					//cServMsg.SetErrorCode( ERR_MCU_NOTSAMEMEDIATYPE );
					//[nizhijun 2010/11/15] 这里回消息应该回给会控
					FreeRecvSpy(tSrcMt, bySwitchMode);
					NotifyMcsAlarmInfo( 0, ERR_MCU_NOTSAMEMEDIATYPE);
					return;
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
					//	cServMsg.SetErrorCode( ERR_MCU_NOTSAMEMEDIATYPE );
					//[nizhijun 2010/11/15] 这里回消息应该回给会控
					FreeRecvSpy(tSrcMt, bySwitchMode);
					NotifyMcsAlarmInfo( 0, ERR_MCU_NOTSAMEMEDIATYPE );
					return;
				}
			}
		}
	}

	//防止SendReplyBack将消息返回应答，将Src设为NULL
	if( MT_MCU_STARTSELMT_CMD == EvId  )
	{
		cServMsg.SetNoSrc();
        cServMsg.SetSrcMtId(0);
	}
	
	// 给MCS发通知
	u8 byMcsId = 0;
	if ( MCS_MCU_STARTSWITCHMT_REQ == EvId   || 
		 MCS_MCU_STARTSWITCHMC_REQ == EvId )
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
	TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tSrcMt.GetMcuId());
	
	// 其它
	if( MCS_MCU_STARTSWITCHMC_REQ != EvId )
	{
		if ( tSrcMt.IsLocal() )
		{	
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "ProcSelMtStartPreSetInAck False, tSrc is local, Mtid: %d\n", tSrcMt.GetMtId() );
			return;
		}

		//目的终端未入会议，NACK
        if( !m_tConfAllMtInfo.MtJoinedConf( tDstMt.GetMtId() ) )
        {
            ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcSelMtStartPreSetInAck]Specified Mt(%u,%u) not joined conference!\n", 
                tDstMt.GetMcuId(), tDstMt.GetMtId() );
            cServMsg.SetErrorCode( ERR_MCU_MT_NOTINCONF );
            SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
            return;
        }

		//混音时调整选看模式
		if( m_tConf.m_tStatus.IsMixing() && !( tDstMt == m_tConf.GetChairman() )  )
		{
            //选看音频，NACK
			if( tSwitchInfo.GetMode() == MODE_AUDIO  )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "Conference %s is mixing now. Cannot switch only audio!\n", 
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
                    ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "CMcuVcInst: Conference %s is mixing now. Cannot switch audio!\n", 
                             m_tConf.GetConfName() );
                    cServMsg.SetErrorCode( ERR_MCU_SELBOTH_INMIXING );
                    SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );                        
                }

				tSwitchInfo.SetMode( MODE_VIDEO );
			}
		}

		StopSelectSrc( tDstMt,tSwitchInfo.GetMode(),FALSE,FALSE );
		
		// 如果有选看(音频或者视频)，都要记录下来
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

		//for h320 mcu cascade select
		if( MT_MCU_STARTSELMT_REQ == cServMsg.GetEventId() && 
			MT_TYPE_SMCU == m_ptMtTable->GetMtType(tDstMt.GetMtId()) )
		{            
			m_tLastSpyMt = tSrcMt;
		} 

		// [pengjie 2010/4/14] 选看下级的合成成员，调分辨率
		if( ChgMtVidFormatRequired(tSrcMt) )
		{
			ChangeMtVideoFormat(tSrcMt, FALSE);
		}

		// 通知终端接收地址
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
    } 

	// 建立交换
    // 这里需要考虑是否是广播交换
    // 需要防止会控监控进入
    if( tSrcMt == GetLocalVidBrdSrc() && EvId != MCS_MCU_STARTSWITCHMC_REQ &&
        ( !( (tSrcMt==m_tRollCaller && GetLocalMtFromOtherMcuMt(tDstMt)==GetLocalMtFromOtherMcuMt(m_tRollCallee)) 
		|| (tSrcMt==GetLocalMtFromOtherMcuMt(m_tRollCallee) && tDstMt==m_tRollCaller) ) ) &&
        ( MODE_VIDEO == tSwitchInfo.GetMode() || MODE_BOTH == tSwitchInfo.GetMode() ) )
    {
        ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "Dst(Mt.%d) receive Src(Mt.%d) as video broadcast src!\n", tDstMt.GetMtId(), tSrcMt.GetMtId() );

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

	//[2011/10/26/zhangli]要求回传与实际回传不一致
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY, "[ProcSelMtStartPreSetInAck] Presetin-Mode:%d, PresetinAck-Mode:%d\n", byOldSpyMode, bySpyMode);
	
	TMt tOldSrc;
	BOOL32 bInnerSelect = FALSE;
	if (MT_MCU_STARTSELMT_CMD == cServMsg.GetEventId()
		|| MT_MCU_STARTSELMT_REQ == cServMsg.GetEventId()
		|| MCS_MCU_STARTSWITCHMT_REQ == cServMsg.GetEventId())
	{
		if (cServMsg.GetSrcSsnId() == 0)		
		{
			bInnerSelect = TRUE;
		}
		//如果某种选看失败，停掉旧的选看
		if (MODE_BOTH == byOldSpyMode && MODE_VIDEO == bySpyMode)
		{
			TMtStatus tMtStatus;
			m_ptMtTable->GetMtStatus(tDstMt.GetMtId(), &tMtStatus);
			tOldSrc = tMtStatus.GetSelectMt(MODE_AUDIO);
			
			if (!tOldSrc.IsNull())
			{
				StopSelectSrc(tDstMt, MODE_AUDIO);
			}
		}
		
		if (MODE_BOTH == byOldSpyMode && MODE_AUDIO == bySpyMode)
		{
			TMtStatus tMtStatus;
			m_ptMtTable->GetMtStatus(tDstMt.GetMtId(), &tMtStatus);
			tOldSrc = tMtStatus.GetSelectMt(MODE_VIDEO);
			
			if (!tOldSrc.IsNull())
			{
				StopSelectSrc(tDstMt, MODE_VIDEO);
			}
		}
	}
	else if (MCS_MCU_STARTSWITCHMC_REQ == cServMsg.GetEventId())
	{
		//如果某种监控失败，停掉旧的监控，第三个参数FALSE，否则做替换操作时界面会清空监控
		if (MODE_BOTH == byOldSpyMode && MODE_VIDEO == bySpyMode)
		{
			g_cMcuVcApp.GetMcSrc(cServMsg.GetSrcSsnId(), &tOldSrc, tSwitchInfo.GetDstChlIdx(), MODE_AUDIO);
			
			if (!tOldSrc.IsNull())
			{
				StopSwitchToMc(cServMsg.GetSrcSsnId(), tSwitchInfo.GetDstChlIdx(), FALSE, MODE_AUDIO);
			}
		}
		
		//监控一定监控视屏，如果MODE_AUDIO == bySpyMode，不管byOldSpyMode则认为下级因资源不足把MODE_VIDEO去掉，这里释放老的MODE_VIDEO
		if (/*MODE_BOTH == byOldSpyMode && */MODE_AUDIO == bySpyMode)
		{
			g_cMcuVcApp.GetMcSrc(cServMsg.GetSrcSsnId(), &tOldSrc, tSwitchInfo.GetDstChlIdx(), MODE_VIDEO);
			
			if (!tOldSrc.IsNull())
			{
				StopSwitchToMc(cServMsg.GetSrcSsnId(), tSwitchInfo.GetDstChlIdx(), FALSE, MODE_VIDEO);
			}
		}
	}

	TSwitchInfo tTempSwitchInfo;
	BOOL32 bSwitchVideoSuc = FALSE;
	BOOL32 bSwitchAudioSuc = FALSE;
	if(MODE_BOTH == tSwitchInfo.GetMode() || MODE_VIDEO == tSwitchInfo.GetMode())
	{
		if (bVidAdp)
		{
			bSwitchVideoSuc = StartSelAdapt(tSwitchInfo.GetSrcMt(), tSwitchInfo.GetDstMt(), MODE_VIDEO, bInnerSelect);
			if ( !bSwitchVideoSuc ) 
			{
				FreeRecvSpy(tSwitchInfo.GetSrcMt(),MODE_VIDEO);
				TMt tNullMt;
				tDstMtStatus.SetSelectMt( tNullMt, MODE_VIDEO);
				m_ptMtTable->SetMtStatus( tSwitchInfo.GetDstMt().GetMtId(), &tDstMtStatus );
			}
		}
		else
		{			
			bSwitchVideoSuc = TRUE;
			memcpy(&tTempSwitchInfo, &tSwitchInfo, sizeof(tTempSwitchInfo));
			tTempSwitchInfo.SetMode(MODE_VIDEO);
			SwitchSrcToDst(tTempSwitchInfo, cServMsg);				
		}
	}
	
	if (MODE_BOTH == tSwitchInfo.GetMode() || MODE_AUDIO == tSwitchInfo.GetMode())
	{
		if (bAudAdp)
		{
			bSwitchAudioSuc = StartSelAdapt(tSwitchInfo.GetSrcMt(), tSwitchInfo.GetDstMt(), MODE_AUDIO, bInnerSelect);
			if ( !bSwitchAudioSuc ) 
			{
				FreeRecvSpy(tSwitchInfo.GetSrcMt(),MODE_AUDIO);
				TMt tNullMt;
				tDstMtStatus.SetSelectMt( tNullMt, MODE_AUDIO);
				m_ptMtTable->SetMtStatus( tSwitchInfo.GetDstMt().GetMtId(), &tDstMtStatus );
			}
		}
		else
		{
			bSwitchAudioSuc = TRUE;
			memcpy(&tTempSwitchInfo, &tSwitchInfo, sizeof(tTempSwitchInfo));
			tTempSwitchInfo.SetMode(MODE_AUDIO);
			SwitchSrcToDst(tTempSwitchInfo, cServMsg);
		}
	}

	// 设置发言人源为非会议自动指定
	if ( HasJoinedSpeaker() && tDstMt == GetLocalSpeaker() )
	{
		SetSpeakerSrcSpecType( tSwitchInfo.GetMode(), SPEAKER_SRC_MCSDRAGSEL );
	}	
    
	//zjl20101116选看业务发SpyNotify
	if( !tSwitchInfo.GetSrcMt().IsLocal() )
	{
		if( IsLocalAndSMcuSupMultSpy(tSwitchInfo.GetSrcMt().GetMcuId()) )
		{
			//保证上级交换成功
			if (bSwitchVideoSuc || bSwitchAudioSuc)
			{
				//选看业务获取级联通道能力保存为回传目的能力集
				TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSwitchInfo.GetSrcMt().GetMcuId()));				
				if(TYPE_MT == tDstMt.GetMtType())
				{
					TSimCapSet tDstCapSet = m_ptMtTable->GetDstSCS(tDstMt.GetMtId());
					tSimCapSet.SetVideoMaxBitRate(tDstCapSet.GetVideoMaxBitRate());
				}
				
				if (tSimCapSet.IsNull())
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, "[ProcSelMtStartPreSetInAck:SpyNotify] tSimCapSet is null!\n");
					return;
				}	
				SendMMcuSpyNotify( tSwitchInfo.GetSrcMt(), EvId, tSimCapSet);		
			}
// 			else
// 			{
// 				//如果上级选看失败，释放资源
// 				FreeRecvSpy(tSwitchInfo.GetSrcMt(), tSwitchInfo.GetMode());
// 			}
		}
	}	
}


/*==============================================================================
函数名    :  ProcVmpStartPreSetInAck
功能      :  处理未处理完的画面合成逻辑
算法实现  :  
参数说明  :  [IN] TPreSetInRsp 多回传必要信息
返回值说明:  void
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-11-11					薛亮							create
==============================================================================*/
void CMcuVcInst::ProcVmpStartPreSetInAck(const TPreSetInRsp &tPreSetInRsp1)
{
	//zhouyiliang 20100909 调整vmp代码，将设置会议vmpparam放在这做，AdjustVmpParam里面调整完只存在tPeriEqpStatus里面
	//TVMPParam tVMPParam	= m_tConf.m_tStatus.GetVmpParam();
	TEqp tVmpEqp = tPreSetInRsp1.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_tVmp;
	if (!IsValidVmpId(tVmpEqp.GetEqpId()))
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ProcVmpStartPreSetInAck] vmp id.%d is wrong.\n",
			tVmpEqp.GetEqpId());
		return;
	}
	TPeriEqpStatus tPeriEqpStatus; 
	BOOL32 bGetStatus = g_cMcuVcApp.GetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );


	// 判断当前状态是否正常,不正常时需要释放此路回传带宽
	if( bGetStatus 
		&& ( TVmpStatus::WAIT_STOP == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState ||
		TVmpStatus::IDLE == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
		)
	{
		TMt tSrc = tPreSetInRsp1.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
		FreeRecvSpy(tSrc, MODE_VIDEO);
		ConfPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ProcVmpStartPreSetInAck] vmp state.%d is not correct, so return here\n",
			tPeriEqpStatus.m_tStatus.tVmp.m_byUseState);
		return;
	}

	TVMPParam_25Mem tVMPParam = tPeriEqpStatus.m_tStatus.tVmp.GetVmpParam() ;

	//zhouyiliang 20100820 如果要走替换的逻辑的话，要改tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos
	TPreSetInRsp tPreSetInRsp = tPreSetInRsp1;
	/*if ( tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byIsNeedRelease ) 
	{
		//vcs 自动画面合成,只有vcs自动画面合成才有替换策略
		//zhouyiliang20100917 vcs 自动画面合成策略修改，注释掉老的策略
		//HandleVCSAutoVmpSpyRepalce( tPreSetInRsp );

		//zhouyiliang20100917 mcs定制画面合成,如果替换的通道内的终端在别的通道内还有，如发言人跟随通道，找到替换终端所在别的通道，并清空
		if (VCS_CONF != m_tConf.GetConfSource() && !tVMPParam.IsVMPAuto() && !IsAllowVmpMemRepeated())
		{
			TMt tSrc = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();

			TMultiCacMtInfo tMtInfo;
			tMtInfo.m_byCasLevel = tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_byCasLevel;
			memcpy( &tMtInfo.m_abyMtIdentify[0],
				&tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_abyMtIdentify[0],
				sizeof(tMtInfo.m_abyMtIdentify)
				);
			TMt tReleaseMt = GetMtFromMultiCascadeMtInfo( tMtInfo,
							tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_tMt
							);
			if ( !tReleaseMt.IsNull() ) 
			{				
				for ( u8 byIdx = 0; byIdx < tVMPParam.GetMaxMemberNum() ; byIdx++  ) 
				{
					TVMPMember tTempVmpMember;
					tTempVmpMember = *tVMPParam.GetVmpMember(byIdx);
					if ( tReleaseMt == (TMt)tTempVmpMember 
						&& byIdx != tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos ) 
					{
						tVMPParam.ClearVmpMember( byIdx );
					}
				}
			}
		}
	}*/

	BOOL32 bStart = FALSE;
	//根据外设状态判断是否是第一次开启vmp
	if (TVmpStatus::RESERVE == tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
	{
		bStart = TRUE;
	}

	//根据回传响应信息，添加到VMPParam中
	TVMPMember tVmpMember;
	tVmpMember.SetNull();
	tVmpMember.SetMemberTMt(tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt());
	tVmpMember.SetMemberType(tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byMemberType);
	tVmpMember.SetMemStatus(tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byMemStatus);
	tVMPParam.SetVmpMember(tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos, tVmpMember);
	//zhouyiliang 20100721 将vmpparam里面除了刚才替换的那个成员外，所有的成员，替换优先级都设为1（有待考虑是否++）
// 	for (u8 byLoop = 1; byLoop < tVMPParam.GetMaxMemberNum(); byLoop++)
// 	{
// 		if (tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos == byLoop)//跳过刚替换的成员
// 		{
//              m_cVCSConfStatus.SetVmpMemberReplacePrio(byLoop, 0 );//zhouyiliang20100721 给进vmp的成员最低的替换优先级
// 			continue;
// 		}
// 		//zhouyiliang 20100831 注释掉原来的vcs自动画面合成替换同级mcu的策略，沿用按顺序替换的老策略
// 	//	TMt tTemMemeber = *(TMt*)tVMPParam.GetVmpMember( byLoop );	
// 	// 	if ( tTemMemeber.GetMcuId() == tVmpMember.GetMcuId() ) 
// 	// {
// 		m_cVCSConfStatus.SetVmpMemberReplacePrio(byLoop, 1);
// 	//	}
// 		
// 	}
	
	//lukunpeng 2010/07/09 必须先SetVmpParam，以防止时序有问题
	//m_tConf.m_tStatus.SetVmpParam(tVMPParam);

	//调整VMP
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VMP, "[ProcVmpStartPreSetInAck]needsetin pos:%d\n ",tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos );
	AdjustVmpParam(tVmpEqp.GetEqpId(), &tVMPParam, bStart, FALSE);
	TMt tSrc = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	//如果调度终端在发言队列内，就要从发言队列内清除
	if( m_cVCSConfStatus.GetCurVCMT() == tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt() &&
		m_tApplySpeakQue.IsMtInQueue( tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt() ) 
		)
	{
		BOOL32 bIsSendToChairman = FALSE;
		if( !m_tApplySpeakQue.ProcQueueInfo( tSrc,bIsSendToChairman,FALSE ) )
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,  "[ProcVmpStartPreSetInAck] Fail to pop tMt(%d.%d). \n",
						tSrc.GetMcuId(),tSrc.GetMtId()
						);
			bIsSendToChairman = FALSE;
		}

		NotifyMcsApplyList( bIsSendToChairman );
	}
}

/*==============================================================================
函数名    :  ProcSpeakerStartPreSetInAck
功能      :  处理未处理完的拖下级终端做上级发言人的操作
算法实现  :  
参数说明  :  [IN] TPreSetInRsp 多回传必要信息
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-11-05                 pengjie							create
2011-7-8     4.6           彭国锋                          修改
==============================================================================*/
void CMcuVcInst::ProcSpeakerStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp )
{
	TMt tSrcMt      = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();

// 2011-7-8 del by peng guofeng : 没有用到
// 	u8  bySpyMode = tPreSetInRsp.m_byRspSpyMode;
// 	u32 EvId      = tPreSetInRsp.m_tSetInReqInfo.m_dwEvId;
// 	CServMsg cServMsg;
// 2011-7-8 del end
	
	//改变发言人 此时presetin已经完成，再次调用ChangeSpeaker建交换，传入参数presetin为false。
	ChangeSpeaker( &tSrcMt,FALSE,TRUE,FALSE );
	return;
}

/*==============================================================================
函数名    :  ProcHduStartPreSetInAck
功能      :  处理级联多回传下级终端进上级高清电视墙相关未处理完的业务逻辑
算法实现  :  
参数说明  :  [IN] TPreSetInRsp 多回传必要信息
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010-07-16                 陆昆朋							modify
==============================================================================*/
void CMcuVcInst::ProcHduStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp )
{
	TMt tSrcMt    = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	u8  byHduId   = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyHduInfo.m_tHdu.GetEqpId();;
	u8  bySpyMode = tPreSetInRsp.m_byRspSpyMode;
	u32 EvId      = tPreSetInRsp.m_tSetInReqInfo.GetEvId();
	TSpyHduInfo tSpyHduInfo = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyHduInfo;
	if (tSpyHduInfo.m_byDstChlIdx >= MAXNUM_HDU_CHANNEL) 
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ProcHduStartPreSetInAck]tSpyHduInfo.m_byDstChlIdx(%d) >= MAXNUM_HDU_CHANNEL(%d)\n");
		FreeRecvSpy(tSrcMt, bySpyMode);
		return;
	}

	//电视墙轮询优先级最高，不能选看VMP或选看终端
	TPeriEqpStatus tHduStatus;
    g_cMcuVcApp.GetPeriEqpStatus(byHduId, &tHduStatus);
	
	// [10/24/2011 liuxu] 
	TTvwMember tTvwMember = tHduStatus.m_tStatus.tHdu.GetHduMember(tSpyHduInfo.m_byDstChlIdx, tSpyHduInfo.GetSubChnIdx());
	if (tTvwMember.GetConfIdx() != 0 && tTvwMember.GetConfIdx() != m_byConfIdx)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ProcHduStartPreSetInAck] hdu chnnl has been occupyed by other conf\n");
		FreeRecvSpy(tSrcMt, bySpyMode);
		return;
	}

	// [10/24/2011 liuxu] 状态保护, 避免停止电视墙后又响应电视墙轮询的timer和presetin ack
	if( tSpyHduInfo.m_bySrcMtType == TW_MEMBERTYPE_TWPOLL )
	{
		if (tTvwMember.byMemberType != TW_MEMBERTYPE_TWPOLL)
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ProcHduStartPreSetInAck] poll has been stoped\n");
			FreeRecvSpy(tSrcMt, bySpyMode);
			return;
		}

		u8 byPollState = POLL_STATE_NONE;
		m_tTWMutiPollParam.GetPollState(byHduId, tSpyHduInfo.m_byDstChlIdx, byPollState);
		if (POLL_STATE_NORMAL != byPollState)
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[ProcHduStartPreSetInAck] poll mode has been changed\n");
			FreeRecvSpy(tSrcMt, bySpyMode);
			return;
		}
	}

	// [2013/03/11 chenbing] 需要判断是否为四风格 
	u8 byHduMode = HDUCHN_MODE_ONE;
	if ( HDUCHN_MODE_FOUR == tHduStatus.m_tStatus.tHdu.GetChnCurVmpMode(tSpyHduInfo.m_byDstChlIdx)
		&& TW_MEMBERTYPE_BATCHPOLL != tSpyHduInfo.m_bySrcMtType
		&& TW_MEMBERTYPE_TWPOLL != tSpyHduInfo.m_bySrcMtType
	   )
	{ 
		byHduMode = HDUCHN_MODE_FOUR;
	}
	if (!tTvwMember.IsNull() && 
		tHduStatus.m_tStatus.tHdu.GetChnStatus(tSpyHduInfo.m_byDstChlIdx, tSpyHduInfo.GetSubChnIdx()) == THduChnStatus::eRUNNING)
	{
		// [2013/03/11 chenbing]  添加 tSpyHduInfo.GetSubChnIdx()
		u8  byOldMemberType = tHduStatus.m_tStatus.tHdu.GetMemberType(tSpyHduInfo.m_byDstChlIdx, tSpyHduInfo.GetSubChnIdx());
		ChangeHduSwitch( NULL,byHduId, tSpyHduInfo.m_byDstChlIdx, tSpyHduInfo.GetSubChnIdx()/*子通道*/,
			byOldMemberType, TW_STATE_STOP, MODE_BOTH, FALSE, TRUE, FALSE );
		g_cMcuVcApp.GetPeriEqpStatus( byHduId, &tHduStatus );
	}
	tHduStatus.m_tStatus.tHdu.SetSchemeIdx( tSpyHduInfo.m_bySchemeIdx, tSpyHduInfo.m_byDstChlIdx, tSpyHduInfo.GetSubChnIdx());
    g_cMcuVcApp.SetPeriEqpStatus( byHduId, &tHduStatus );
	
	// [2013/03/11 chenbing]  添加 tSpyHduInfo.GetSubChnIdx()
	ChangeHduSwitch( &tSrcMt, byHduId, tSpyHduInfo.m_byDstChlIdx, tSpyHduInfo.GetSubChnIdx(),
		tSpyHduInfo.m_bySrcMtType, TW_STATE_START, bySpyMode, FALSE, TRUE, FALSE, byHduMode);

	if (VCS_CONF == m_tConf.GetConfSource() 
		&& ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode())
		&& m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_AUTO_MODE)
	{
		// [5/31/2011 liuxu] 新提供接口GetConfCfgedTvwIdx那些啰嗦的代码
		const u8 wCurrTwPosId = GetConfCfgedTvwIdx( tSpyHduInfo.m_tHdu.GetEqpId(), tSpyHduInfo.m_byDstChlIdx);
		m_cVCSConfStatus.SetCurUseTWChanInd( wCurrTwPosId );
		
		CRecvSpy tSrcSpyInfo;
		if( m_cSMcuSpyMana.GetRecvSpy(tSrcMt, tSrcSpyInfo) &&
			TSpyStatus::WAIT_FREE == tSrcSpyInfo.m_byUseState )
		{
			m_cSMcuSpyMana.ModifyUseState( tSrcMt,TSpyStatus::NORMAL );
		}
	}
	return;
}

/*==============================================================================
函数名    :  ProcPollingStartPreSetInAck
功能      :  处理级联多回传 轮询下级时未处理完的逻辑
算法实现  :  
参数说明  :  [IN] TPreSetInRsp 多回传必要信息
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-11-05                 pengjie							create
==============================================================================*/	
void CMcuVcInst::ProcPollStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp )
{
	TMt tSrcMt    = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	u8  bySpyMode = tPreSetInRsp.m_byRspSpyMode;
	u32 dwEvId      = tPreSetInRsp.m_tSetInReqInfo.GetEvId();
	u16 wKeepTime = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyPollInfo.m_wKeepTime;
	u8  byPollingPos = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyPollInfo.m_byPollingPos;
	u8  byPollMode = tPreSetInRsp.m_tSetInReqInfo.m_bySpyMode;

	//lukunpeng 2010/06/30 获取当前轮询的终端，如果跟回传终端不一致，就说明轮询已经走到下一个终端，
	//此时的回传终端已经没有意义，释放当前回传终端信息。
	TPollInfo tPollInfo = *(m_tConf.m_tStatus.GetPollInfo());
	TMt tCurPolledMt = tPollInfo.GetMtPollParam().GetTMt();
	if (!(tCurPolledMt == tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt()))
	{
		FreeRecvSpy(tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt(), bySpyMode);
		return;
	}

	//　TvWall和Hdu移到VMP前面来处理，和ChangeSpeaker对应
	//TvWall
	TPeriEqpStatus tTWStatus;
	u8 byChnlIdx;
	u8 byEqpId;
				
	for (byEqpId = TVWALLID_MIN; byEqpId <= TVWALLID_MAX; byEqpId++)
	{
		if (EQP_TYPE_TVWALL == g_cMcuVcApp.GetEqpType(byEqpId))
		{
			// [8/30/2011 liuxu] 如果该电视墙不在线, 就不要继续进行了
			if(g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTWStatus) && tTWStatus.m_byOnline )
			{	
				for (byChnlIdx = 0; byChnlIdx < MAXNUM_PERIEQP_CHNNL; byChnlIdx++)
				{
					// [miaoqingsong 20111205] 增加判断条件: 主席轮询/上传轮询模式轮到的MT均不进电视墙视频轮询跟随通道
					if ( (TW_MEMBERTYPE_POLL == tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType) && 
						 (m_byConfIdx == tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetConfIdx()) && 
						 !(tPollInfo.GetMediaMode() == MODE_VIDEO_CHAIRMAN || tPollInfo.GetMediaMode() == MODE_BOTH_CHAIRMAN) && 
						 !(tPollInfo.GetMediaMode() == MODE_VIDEO_SPY || tPollInfo.GetMediaMode() == MODE_BOTH_SPY) 
						 )
					{
						ChangeTvWallSwitch( &tSrcMt, byEqpId, byChnlIdx, TW_MEMBERTYPE_POLL, TW_STATE_CHANGE);
					}
				}
			}
			
		}
	}
				
	//hdu
	TPeriEqpStatus tHduStatus;
	u8 byHduChnlIdx;
	u8 byHduEqpId;
				
	for (byHduEqpId = HDUID_MIN; byHduEqpId <= HDUID_MAX; byHduEqpId++)
	{
		if(IsValidHduEqp(g_cMcuVcApp.GetEqp(byHduEqpId)))
		{
			if( !g_cMcuVcApp.GetPeriEqpStatus(byHduEqpId, &tHduStatus) || !tHduStatus.m_byOnline )
			{
				continue;
			}
			
			u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byHduEqpId));
			if (0 == byHduChnNum)
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU,  "[ProcPollStartPreSetInAck] GetHduChnNumAcd2Eqp failed!\n");
				continue;
			}
			
			for (byHduChnlIdx = 0; byHduChnlIdx < byHduChnNum; byHduChnlIdx++)
			{
				// [miaoqingsong 20111205] 增加判断条件: 主席轮询/上传轮询模式轮到的MT均不进电视墙视频轮询跟随通道
				if ( (TW_MEMBERTYPE_POLL == tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].byMemberType) &&  
					 (m_byConfIdx == tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].GetConfIdx()) && 
					 !(tPollInfo.GetMediaMode() == MODE_VIDEO_CHAIRMAN || tPollInfo.GetMediaMode() == MODE_BOTH_CHAIRMAN) && 
					 !(tPollInfo.GetMediaMode() == MODE_VIDEO_SPY || tPollInfo.GetMediaMode() == MODE_BOTH_SPY) 
					 )
				{
					// [2013/03/11 chenbing] HDU多画面不支持轮询,子通道置0
					ChangeHduSwitch( &tSrcMt, byHduEqpId, byHduChnlIdx, 0, TW_MEMBERTYPE_POLL, TW_STATE_START, bySpyMode );
				}
				else
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcPollStartPreSetInAck] HduEqp%d is not exist or not polling \n", byHduEqpId);
				}
			}
		}
	}
				
	// 2011-10-11: 添加此标记，表示要不要发言人再作一次VMP的参数调整
	BOOL32 bVmpNeedChgBySpeaker = TRUE;

	//变更画面复合的相应轮询更随的交换
	// xliang [3/31/2009] 重整逻辑
	//TVMPParam_25Mem tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
	u8 byVmpCount = GetVmpCountInVmpList();
	if (byVmpCount > 0)
	{
		u8 byVmpPollCount = GetVmpChnnlNumBySpecMemberType( VMP_MEMBERTYPE_POLL);//轮询跟随通道数
		BOOL32 bNoneKeda = FALSE;
		if (!tCurPolledMt.IsNull())
		{
			bNoneKeda = (!IsKedaMt(tCurPolledMt, TRUE)) || (!IsKedaMt(tCurPolledMt, FALSE));
		}
		/* xliang [4/21/2009] 主席在选看VMP失效
		if( tVmpParam.IsVMPSeeByChairman() )
		{
			m_tConf.m_tStatus.SetVmpSeebyChairman(FALSE);
			
            // 状态同步刷新到TPeriStatus中
			TPeriEqpStatus tVmpStatus;
			g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tVmpStatus );
			tVmpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
			g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId() , &tVmpStatus );
			
			// xliang [4/14/2009] 主席选看VMP的交换停掉
			StopSelectSrc(m_tConf.GetChairman(), MODE_VIDEO);
		}*/
		
		// xliang [4/2/2009] 带音频的会议轮询,VMP轮询跟随，VMP发言人跟随3者不能共存
		// MPU2支持轮询跟随和发言人跟随同时存在.
		// 发言人跟随通道取消，上报MCS错误提示
		// 带音频的上传+本地轮询,同样与VMP轮询跟随，VMP发言人跟随3者不能共存
		/*u8 byVmpSubType = GetVmpSubType(m_tVmpEqp.GetEqpId());
		if( (tPollInfo.GetMediaMode() == MODE_BOTH || tPollInfo.GetMediaMode() == MODE_BOTH_BOTH)
			&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER) //带音频的会议轮询=轮流变发言人
			&& tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_POLL)  
			//&& (byVmpSubType == MPU_SVMP || byVmpSubType == VMP_8KH)//不区分vmp类型	// [2/28/2010 xliang] 仅对于mpu-vmp作限制
			&& !IsAllowVmpMemRepeated(m_tVmpEqp.GetEqpId()) //  [2/17/2012 pengguofeng]
			)	
		{
			NotifyMcsAlarmInfo( 0, ERR_AUDIOPOLL_CONFLICTVMPFOLLOW );
			
			//调整VMP param
			u8 byLoop;
			TVMPMember tVmpMember;
			for(byLoop = 0; byLoop < tVmpParam.GetMaxMemberNum(); byLoop++)
			{
				tVmpMember = *tVmpParam.GetVmpMember(byLoop);
				if(tVmpMember.GetMemberType() == VMP_MEMBERTYPE_SPEAKER)
				{
					ClearOneVmpMember(m_tVmpEqp.GetEqpId(), byLoop, tVmpParam);
					break;
				}
			}
		}*/

		// [miaoqingsong 20111205] 增加判断条件：主席轮询模式轮到的MT均不进VMP视频轮询跟随通道
		if( byVmpPollCount > 0 && 
			!(tPollInfo.GetMediaMode() == MODE_VIDEO_CHAIRMAN || tPollInfo.GetMediaMode() == MODE_BOTH_CHAIRMAN) && 
			!(tPollInfo.GetMediaMode() == MODE_VIDEO_SPY || tPollInfo.GetMediaMode() == MODE_BOTH_SPY)
			//|| (tVmpParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER) //带音频的会议轮询进发言人跟随通道
			//&& tPollInfo.GetMediaMode() == MODE_BOTH ) 
			)
		{
			bVmpNeedChgBySpeaker = FALSE;
			// xliang [3/19/2009] 对于MPU，若轮询到的MT已经在其他通道中且该通道是发言人跟随或者该Mt是发言人
			// 则不进VMP中的轮询跟随通道  ----废除 xliang [4/2/2009] 
			// xliang [4/2/2009] 对于新VMP，若轮询到的MT已经在其他通道中均不进轮询跟随通道

			// [2/5/2010 xliang] 重整
			/*u8 byMtVmpChnl = tVmpParam.GetChlOfMtInMember(tCurPolledMt /*tLocalMt/);
			if( (!IsAllowVmpMemRepeated(m_tVmpEqp.GetEqpId())) 
				&&  tVmpParam.IsMtInMember(tCurPolledMt)
				)
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MCS, "[ProcPollingChangeTimerMsg] Mt.(%u,%u) has already in certain VMP channel!\n",
					tCurPolledMt.GetMcuId(), tCurPolledMt.GetMtId() );
			}
			else
			{*/
				/*提前释放轮询跟随通道的旧轮询终端多回传资源,防止时序问题(vmp未释放之前,轮询先发了SendMMcuSpyNotify)
				//轮询跟随可能在多个vmp通道内
				for (u8 byChlNum=0; byChlNum<tVmpParam.GetMaxMemberNum(); byChlNum++)
				{
					TVMPMember tOldMember = *tVmpParam.GetVmpMember(byChlNum);
					if (!tOldMember.IsNull() && !tOldMember.IsLocal() && !(tOldMember == tCurPolledMt))
					{
						// 释放轮询跟随多回传资源
						if (VMP_MEMBERTYPE_POLL == tOldMember.GetMemberType())
						{
							/* 非科达终端,仅保留1路轮询跟随通道(规格更新,不能设多路跟随)
							if (bNoneKeda && byChlNum != tVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_POLL))
							{
								ClearOneVmpMember(byChlNum, tVmpParam);
								continue;
							}*
							FreeRecvSpy( tOldMember, MODE_VIDEO );
						}
						// 如果是带音频的轮询,同样在此处释放发言人跟随多回传资源
						else if (VMP_MEMBERTYPE_SPEAKER == tOldMember.GetMemberType())
						{
							/* 非科达终端,仅保留1路轮询跟随通道
							if (bNoneKeda && byChlNum != tVmpParam.GetChlOfMemberType(VMP_MEMBERTYPE_SPEAKER))
							{
								ClearOneVmpMember(byChlNum, tVmpParam);
								continue;
							}*
							// 带音频的本地轮询,带音频的回传轮询+本地轮询
							if (tPollInfo.GetMediaMode() == MODE_BOTH ||
								tPollInfo.GetMediaMode() == MODE_BOTH_BOTH)
							{
								FreeRecvSpy( tOldMember, MODE_VIDEO );
							}
						}
					}
				}*/

				//u8 byVmpMemType = VMP_MEMBERTYPE_POLL;
				//TMt tLastMt = m_tConfPollParam.GetLastPolledMt();
				//ChangeVmpChannelParam(&tCurPolledMt/*&tLocalMt*/, byVmpMemType, &tLastMt);
				// 2011-10-11 轮询会顺便调整发言人的相关通道，所以不用发言人再调整了
				/*bVmpNeedChgBySpeaker = FALSE;
			}*/
		}
	}

	if ( (m_tConf.m_tStatus.GetVmpTwMode() != CONF_VMPTWMODE_NONE) && 
		 !(tPollInfo.GetMediaMode() == MODE_VIDEO_CHAIRMAN || tPollInfo.GetMediaMode() == MODE_BOTH_CHAIRMAN) && 
		 !(tPollInfo.GetMediaMode() == MODE_VIDEO_SPY || tPollInfo.GetMediaMode() == MODE_BOTH_SPY) 
		 )
	{
		ChangeVmpTwChannelParam(&tCurPolledMt, VMPTW_MEMBERTYPE_POLL);
	}

	TMt tLastMt = m_tConfPollParam.GetLastPolledMt();
	//tVmpParam = g_cMcuVcApp.GetConfVmpParam(m_tVmpEqp);
	BOOL32 bIsVmpBrd = (GetVidBrdSrc().GetEqpType() == EQP_TYPE_VMP);

 	switch( tPollInfo.GetMediaMode() )
 	{
 	case MODE_VIDEO:
 		{		
			u8 bySrcChnnl = (tLastMt == m_tPlayEqp ? m_byPlayChnnl : 0);
			
			FreeRecvSpy( tLastMt, MODE_VIDEO );	
			if(!bIsVmpBrd)
			{
				ChangeVidBrdSrc( &tSrcMt );
			}

			m_tConfPollParam.SetLastPolledMt( tSrcMt );
		}
		break;

	case MODE_BOTH:
        {
			ChangeSpeaker(&tSrcMt, TRUE, bVmpNeedChgBySpeaker/*TRUE*/, FALSE ); // 2011-10-8 mod by pgf:VMP发言人通道已经在轮询中处理，此处不再需要
			m_tConfPollParam.SetLastPolledMt( tSrcMt );	
		}	
        break;
    
	// miaoqingsong [20110608] 级联多回传主席轮询选看功能添加
	case MODE_VIDEO_CHAIRMAN:
	case MODE_BOTH_CHAIRMAN:
		{
			if( !tLastMt.IsNull() )
			{
				if( MODE_VIDEO_CHAIRMAN == tPollInfo.GetMediaMode() )
				{
					StopSelectSrc(m_tConf.GetChairman(), MODE_VIDEO, FALSE, FALSE);
				}
				else //both
				{
					StopSelectSrc(m_tConf.GetChairman(), MODE_BOTH, FALSE, FALSE);
				}
			}

			if(HasJoinedChairman())
			{
				ChangeSelectSrc(tSrcMt, m_tConf.GetChairman(), bySpyMode);
			}
			else
			{
				FreeRecvSpy( tSrcMt,tPollInfo.GetMediaMode() == MODE_VIDEO_CHAIRMAN?MODE_VIDEO:MODE_BOTH );
			}

			// 被选看终端在vmp中的调分辨率处理
			if (!(tLastMt == tSrcMt) )
			{
				//调整旧选看源分辨率
				if(!tLastMt.IsNull() && ChgMtVidFormatRequired(tLastMt) )
				{
					ChangeMtVideoFormat(tLastMt);
				}
				//调整新选看源分辨率
				if(!tSrcMt.IsNull() && ChgMtVidFormatRequired(tSrcMt) )
				{
					ChangeMtVideoFormat(tSrcMt, FALSE);
				}
			}

			m_tConfPollParam.SetLastPolledMt( tSrcMt );
		}
		break;

	case MODE_VIDEO_SPY:
    case MODE_BOTH_SPY:
		{
			if( !tLastMt.IsNull() )
			{
				if( MODE_VIDEO_SPY == tPollInfo.GetMediaMode() )
				{
					FreeRecvSpy( tLastMt, MODE_VIDEO );
				}
				else
				{
					FreeRecvSpy( tLastMt, MODE_BOTH );
				}
			}

			TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()));				
			TSimCapSet tDstCapSet = m_ptMtTable->GetDstSCS(m_tCascadeMMCU.GetMtId());
			tSimCapSet.SetVideoMaxBitRate(tDstCapSet.GetVideoMaxBitRate());
			if (tSimCapSet.IsNull())
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, " GetMtSimCapSetByMode is null!\n");
				return;
			}
			
			SendMMcuSpyNotify( tSrcMt, dwEvId, tSimCapSet);		
			OnSetOutView(tSrcMt, bySpyMode);
			m_tConfPollParam.SetLastPolledMt( tSrcMt );
		}
		break;

    case MODE_VIDEO_BOTH:
		{				
			u8 bySrcChnnl = ( GetLocalMtFromOtherMcuMt(tLastMt) == m_tPlayEqp ? m_byPlayChnnl : 0);
			
			//本地视频广播
			FreeRecvSpy( tLastMt, MODE_VIDEO );
			if (!bIsVmpBrd)
			{
				ChangeVidBrdSrc( &tSrcMt );
			}
			//释放上传通道回传资源
			if( !tLastMt.IsNull() )
			{
				FreeRecvSpy( tLastMt, MODE_VIDEO );
			}
			
			TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()));				
			TSimCapSet tDstCapSet = m_ptMtTable->GetDstSCS(m_tCascadeMMCU.GetMtId());
			tSimCapSet.SetVideoMaxBitRate(tDstCapSet.GetVideoMaxBitRate());
			if (tSimCapSet.IsNull())
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, " GetMtSimCapSetByMode is null!\n");
				return;
			}
			
			SendMMcuSpyNotify( tSrcMt, dwEvId, tSimCapSet);		
			OnSetOutView(tSrcMt, bySpyMode);
			m_tConfPollParam.SetLastPolledMt( tSrcMt );
		}
        break;

    case MODE_BOTH_BOTH:
        {
			//本地发言        
			ChangeSpeaker(&tSrcMt, TRUE, bVmpNeedChgBySpeaker, FALSE ); // 2011-10-8 mod by pgf:此条件下VMP成员由轮询修正，在改变发言人中不需要再作调整
			
			if( !tLastMt.IsNull() )
			{
				FreeRecvSpy( tLastMt, MODE_BOTH );
			}
			
			TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()));				
			TSimCapSet tDstCapSet = m_ptMtTable->GetDstSCS(m_tCascadeMMCU.GetMtId());
			tSimCapSet.SetVideoMaxBitRate(tDstCapSet.GetVideoMaxBitRate());
			if (tSimCapSet.IsNull())
			{
				ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, " GetMtSimCapSetByMode is null!\n");
				return;
			}
			
			SendMMcuSpyNotify( tSrcMt, dwEvId, tSimCapSet);		
			OnSetOutView(tSrcMt, bySpyMode);
			m_tConfPollParam.SetLastPolledMt( tSrcMt );
		}
        break;

    default:
        ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY,  "[ProcMcsMcuPollMsg] unexpected poll mode.%d rcved, ignore it!\n",bySpyMode );
        break;
	}

	// 等多回传资源都释放干净后,再做vmp处理
	if (!bVmpNeedChgBySpeaker)
	{
		u8 byVmpMemType = VMP_MEMBERTYPE_POLL;
		ChangeVmpChannelParam(&tCurPolledMt/*&tLocalMt*/, byVmpMemType, &tLastMt);
	}
				
	SetTimer( MCUVC_POLLING_CHANGE_TIMER, 1000 * wKeepTime );
	// 通知下一个即将被轮询到的终端
	NotifyMtToBePolledNext();
	
	CServMsg cServMsg;
	cServMsg.SetMsgBody( (u8*)&tPollInfo, sizeof(TPollInfo) );
	SendMsgToAllMcs( MCU_MCS_POLLSTATE_NOTIF, cServMsg );

	//通知下级建交换
	if (!tSrcMt.IsLocal() && IsLocalAndSMcuSupMultSpy(tSrcMt.GetMcuId()))
	{
		TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()));
		SendMMcuSpyNotify( tSrcMt, dwEvId , tSimCapSet);
	}	
	return;
}

/*==============================================================================
函数名    :  ProcTVWStartPreSetInAck
功能      :  处理级联多回传下级终端进上级电视墙相关未处理完的业务逻辑
算法实现  :  
参数说明  :  [IN] TPreSetInRsp 多回传必要信息
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010/07/19   4.6           陆昆朋							多回传电视墙整理
==============================================================================*/
void CMcuVcInst::ProcTWStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp )
{
	TMt tSrcMt    = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	u8 byTvWallId = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyTVWallInfo.m_tTvWall.GetEqpId();
	u8  bySpyMode = tPreSetInRsp.m_byRspSpyMode;
	u32 EvId      = tPreSetInRsp.m_tSetInReqInfo.GetEvId();
    TSpyTVWallInfo tSpyTVWallInfo = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyTVWallInfo;	

	TPeriEqpStatus tTvwStatus;
    g_cMcuVcApp.GetPeriEqpStatus(byTvWallId, &tTvwStatus);

	// [10/24/2011 liuxu] 
	TTvwMember tTvwMember = tTvwStatus.m_tStatus.tTvWall.atVideoMt[tSpyTVWallInfo.m_byDstChlIdx];
	if (tTvwMember.GetConfIdx() != 0 && tTvwMember.GetConfIdx() != m_byConfIdx)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcTWStartPreSetInAck] hdu chnnl has been occupyed by other conf\n");
		FreeRecvSpy(tSrcMt, bySpyMode);
		return;
	}
	
	// [10/24/2011 liuxu] 状态保护, 避免停止电视墙后又响应电视墙轮询的timer和presetin ack
	if( tSpyTVWallInfo.m_bySrcMtType == TW_MEMBERTYPE_TWPOLL )
	{
		if (tTvwMember.byMemberType != TW_MEMBERTYPE_TWPOLL)
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcTWStartPreSetInAck] poll has been stoped\n");
			FreeRecvSpy(tSrcMt, bySpyMode);
			return;
		}
		
		u8 byPollState = POLL_STATE_NONE;
		m_tTWMutiPollParam.GetPollState(byTvWallId, tSpyTVWallInfo.m_byDstChlIdx, byPollState);
		if (POLL_STATE_NORMAL != byPollState)
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[ProcTWStartPreSetInAck] poll mode has been changed\n");
			FreeRecvSpy(tSrcMt, bySpyMode);
			return;
		}
	}

 	BOOL32 bNeedSwitchMt = FALSE;

	ChangeTvWallSwitch(&tSrcMt, byTvWallId, tSpyTVWallInfo.m_byDstChlIdx, 
		tSpyTVWallInfo.m_bySrcMtType, TW_STATE_START,FALSE, bySpyMode );
	
	if (   VCS_CONF == m_tConf.GetConfSource() 
		&& ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode())
		&& m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_AUTO_MODE)
	{
		// [5/31/2011 liuxu] 新提供接口GetConfCfgedTvwIdx那些啰嗦的代码
		const u8 wCurrTwPosId = GetConfCfgedTvwIdx( tSpyTVWallInfo.m_tTvWall.GetEqpId(), tSpyTVWallInfo.m_byDstChlIdx);
		m_cVCSConfStatus.SetCurUseTWChanInd( wCurrTwPosId );

		CRecvSpy tSrcSpyInfo;
		if( m_cSMcuSpyMana.GetRecvSpy(tSrcMt, tSrcSpyInfo) &&
			TSpyStatus::WAIT_FREE == tSrcSpyInfo.m_byUseState )
		{
			m_cSMcuSpyMana.ModifyUseState( tSrcMt,TSpyStatus::NORMAL );
		}

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_VCS, "[ProcHduStartPreSetInAck] CURRENT TW AND HDU CHN NUM (%d) CURRENTID(%d)\n",GetVcsHduAndTwModuleChnNum(),wCurrTwPosId);
	}
	return;
}

/*==============================================================================
函数名    :  ProcRollCallPreSetInAck
功能      :  处理级联多回传下级终端做被点名人时相关未处理完的逻辑
算法实现  :  
参数说明  :  [IN] TPreSetInRsp 多回传必要信息
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-11-05                 pengjie							create
==============================================================================*/
void CMcuVcInst::ProcRollCallPreSetInAck( const TPreSetInRsp &tPreSetInRsp )
{
	TMt tNewRollCallee  = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	u8  bySpyMode       = tPreSetInRsp.m_byRspSpyMode;
	u32 EvId            = tPreSetInRsp.m_tSetInReqInfo.GetEvId();
	TMt tNewRollCaller  = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyRollCallInfo.m_tCaller;
	TMt tOldRollCaller  = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyRollCallInfo.m_tOldCaller;
	TMt tOldRollCallee  = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyRollCallInfo.m_tOldCallee;
    BOOL32 bCallerChged = tNewRollCaller == tOldRollCaller ? FALSE : TRUE;
    BOOL32 bCalleeChged = tNewRollCallee == tOldRollCallee ? FALSE : TRUE;

	/*20110809 zjl 被点名人待PreSetInAck记录，和发言人选看等业务保持一致，
				   防止下级终端掉链，本级收到通告后比对tmt取消业务失败*/
	m_tRollCallee = tNewRollCallee;


	if ( bCalleeChged &&
         ROLLCALL_MODE_CALLEE == m_tConf.m_tStatus.GetRollCallMode() )
    {
		// KDV-BUG2004: 点名人或被点名人被强制为发言人，而在MTC上终端状态未改变
		// zgc, 2008-05-21, 用ChangeSpeaker函数代替
        //m_tConf.SetSpeaker( tNewRollCallee );
		ChangeSpeaker( &tNewRollCallee,FALSE,TRUE,FALSE );
    }
	
	//zhouyiliang 20120921 点名时，点名人模式和被点名人模式，下级回了ack就认为这次点名成功了
	//点名与被点名人模式则需等vmp回了notify才认为本次点名成功
	if (m_tConf.m_tStatus.GetRollCallMode() == ROLLCALL_MODE_CALLEE || ROLLCALL_MODE_CALLER == m_tConf.m_tStatus.GetRollCallMode() )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcRollCallPreSetInAck]Recover Rollcall finish Mark \n" );
		SetLastMultiSpyRollCallFinish(TRUE);
	}

	//调整混音
    BOOL32 bLocalMixStarted = FALSE;

	if ( m_tConf.m_tStatus.GetMixerMode() != mcuNoMix)
	{	
		BOOL32 bRemoveMixMember = FALSE;
		BOOL32 bLocalAutoMix = FALSE;
		
		// 停语音激励
		if ( m_tConf.m_tStatus.IsVACing() )
		{
			bRemoveMixMember = TRUE;
			m_tConf.m_tStatus.SetVACing(FALSE);
			//通知主席及所有会控

			CServMsg cServMsg;
			cServMsg.SetEventId(MCU_MCS_STOPVAC_NOTIF);
			SendMsgToAllMcs(MCU_MCS_STOPVAC_NOTIF, cServMsg);
			if ( HasJoinedChairman() )
			{
				cServMsg.SetEventId(MCU_MT_STOPVAC_NOTIF);
				SendMsgToMt(m_tConf.GetChairman().GetMtId(), MCU_MT_STOPVAC_NOTIF, cServMsg);
			}
		}

		//停讨论
		if ( m_tConf.m_tStatus.IsAutoMixing() )
		{
			bRemoveMixMember = TRUE;
			bLocalAutoMix = TRUE;		
			m_tConf.m_tStatus.SetAutoMixing(FALSE);
		}
		
		if (bRemoveMixMember)
		{
			TMt atDstMt[MAXNUM_CONF_MT];
			memset(atDstMt, 0, sizeof(atDstMt));
			u8  byDstMtNum = 0;
			for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
			{
				if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
				{
					TMt tMt = m_ptMtTable->GetMt(byMtId);
					RemoveMixMember(&tMt, FALSE);
					StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
						(MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+byMtId), FALSE, MODE_AUDIO);
					
					if (bLocalAutoMix)
					{
						//zjl 20110510 StopSwitchToSubMt 接口重载替换
						//StopSwitchToSubMt(byMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE);
						atDstMt[byDstMtNum] = tMt;
						byDstMtNum ++;
					}
					
					// 恢复从组播地址接收
					/*if (m_tConf.GetConfAttrb().IsSatDCastMode() && m_ptMtTable->IsMtInMixGrp(byMtId))
					{
						ChangeSatDConfMtRcvAddr(byMtId, LOGCHL_AUDIO);
					}*/
				}
			}

			if (byDstMtNum > 0)
			{
				StopSwitchToSubMt(byDstMtNum, atDstMt, MODE_AUDIO, FALSE);
			}
		}

		if ( bCallerChged  || bCalleeChged)
		{
			//本来就是定制混音
			if ( m_tConf.m_tStatus.IsSpecMixing() )
			{
				//第一次点名，移除当前所有终端
				if ( tOldRollCaller.IsNull() || tOldRollCallee.IsNull())
				{
					for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
					{							
						if (m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
							m_ptMtTable->IsMtInMixing(byMtId))
						{
							TMt tMt = m_ptMtTable->GetMt(byMtId);
							RemoveSpecMixMember(&tMt, 1, FALSE, FALSE);
						}
					}
				}
			}
		}
		
		m_tConf.m_tStatus.SetSpecMixing(TRUE);

		if (bCallerChged)
		{
			if ( !tOldRollCaller.IsNull() )
            {
				RemoveSpecMixMember(&tOldRollCaller, 1, FALSE, FALSE);
			}

			AddSpecMixMember(&tNewRollCaller, 1, FALSE);
		}

		if (bCalleeChged)
		{
			BOOL32 bInSameSMcu = FALSE;
			
			if (!tOldRollCallee.IsLocal() && 
				!tNewRollCallee.IsLocal() && 
				tOldRollCallee.GetMcuId() == tNewRollCallee.GetMcuId())
			{
				bInSameSMcu = TRUE;
			}
			
			BOOL32 bStopNoMix = FALSE;
			
			if (!bInSameSMcu)
			{
				bStopNoMix = TRUE;
			}
			
			AddSpecMixMember(&tNewRollCallee, 1, FALSE);
			if ( !tOldRollCallee.IsNull() )
			{
				RemoveSpecMixMember(&tOldRollCallee, 1, FALSE, bStopNoMix);
			}
			
		}

		// 如果当前本来是讨论模式, 需整理N模式的交换
		if ( bRemoveMixMember )
		{
			for (u8 byMtId = 1; byMtId < MAXNUM_CONF_MT; byMtId++)
			{
				if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
				{
					TMt tMt = m_ptMtTable->GetMt(byMtId);
					if (m_tConf.m_tStatus.IsSpecMixing() && !m_ptMtTable->IsMtInMixing(byMtId))
					{
						SwitchMixMember(&tMt, TRUE);
					}
				}           
			}
        }
	}
	else
	{
		//出于时序问题考虑，只Start一次
		if ( !bLocalMixStarted )
		{
			bLocalMixStarted = TRUE;
			
			CServMsg cMsg;
			cMsg.SetMsgBody((u8*)&tNewRollCaller, sizeof(TMt));
			cMsg.CatMsgBody((u8*)&tNewRollCallee, sizeof(TMt));
			ProcMixStart(cMsg);
		}                    
    }

	//调整广播码流
	u8 byVmpCount = GetVmpCountInVmpList();
	u8 byVmpId = GetTheMainVmpIdFromVmpList();
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( byVmpId );
	TVMPParam_25Mem tVmpParam;
	tVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
	
	if ( m_tConf.m_tStatus.GetRollCallMode() == ROLLCALL_MODE_VMP )
	{
		tVmpParam.SetVMPBrdst(TRUE);
		tVmpParam.SetVMPMode(CONF_VMPMODE_CTRL);
		tVmpParam.SetVMPAuto(FALSE);
		tVmpParam.SetVMPSchemeId(0);
		tVmpParam.SetIsRimEnabled(FALSE);
		
#ifdef _MINIMCU_
		tVmpParam.SetVMPStyle(VMP_STYLE_VTWO);
#else
		tVmpParam.SetVMPStyle(VMP_STYLE_HTWO);
		u16 wError = 0;
		BOOL32 bRet;
		if (byVmpCount == 0)//当前没合成器时才需要找合成器
		{
			bRet = IsMCUSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, EQP_TYPE_VMP, wError);
		}  
		else
		{
			bRet = IsVMPSupportVmpStyle(tVmpParam.GetVMPStyle(), byVmpId, wError);
		}
		if ( !bRet )
		{
			//NotifyMcsAlarmInfo(cServMsg.GetSrcSsnId(), ERR_MCU_ROLLCALL_STYLECHANGE);           
			tVmpParam.SetVMPStyle(VMP_STYLE_VTWO);
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS, "[ProcMcsMcuRollCallMsg] rollcall change to style.%d due to vmp ability is not enough!\n",
				tVmpParam.GetVMPStyle() );
		} 
#endif                
	}
	
	
	if ( bCallerChged )
	{
		if ( ROLLCALL_MODE_CALLER == m_tConf.m_tStatus.GetRollCallMode() )
		{
			if ( !tNewRollCaller.IsLocal() )
			{
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "New roll caller(Mt.%d) is not local mt, it's impossible\n", tNewRollCaller.GetMtId() );
			}
		}
		else if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
		{
			u8 byCallerPos = 0;
			if ( !tOldRollCaller.IsNull() &&
				g_cMcuVcApp.GetVMPMode(tVmpEqp) != CONF_VMPMODE_NONE )
			{
				byCallerPos = tVmpParam.GetChlOfMtInMember(tOldRollCaller);
			}
			//旧点名人不存在，占0通道
			if ( byCallerPos >= MAXNUM_VMP_MEMBER ) 
			{
				byCallerPos = 0;
			}
			TVMPMember tVmpMember;
			memset(&tVmpMember, 0, sizeof(tVmpMember));
			tVmpMember.SetMemberType(VMP_MEMBERTYPE_MCSSPEC);
			tVmpMember.SetMemberTMt(tNewRollCaller);
			tVmpMember.SetMemStatus(MT_STATUS_AUDIENCE);
			tVmpParam.SetVmpMember(byCallerPos, tVmpMember);
		}
	}
	if ( bCalleeChged )
	{
		
		if ( ROLLCALL_MODE_CALLEE == m_tConf.m_tStatus.GetRollCallMode() )
		{
			//FreeRecvSpy(tOldRollCallee, MODE_VIDEO);
		}
		else if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
		{			
			u8 byCalleePos = 1;
			if ( !tOldRollCallee.IsNull() &&
				g_cMcuVcApp.GetVMPMode(tVmpEqp) != CONF_VMPMODE_NONE )
			{
//				byCalleePos = tVmpParam.GetChlOfMtInMember(GetLocalMtFromOtherMcuMt(tOldRollCallee));
				// [pengjie 2010/4/14] 这里不能填local化得信息
				byCalleePos = tVmpParam.GetChlOfMtInMember(tOldRollCallee);
			}
			
			//旧点名人不存在，占1通道
			if ( byCalleePos >= MAXNUM_VMP_MEMBER )
			{
				byCalleePos = 1;
			}
			TVMPMember tVmpMember;
			memset(&tVmpMember, 0, sizeof(tVmpMember));
			tVmpMember.SetMemberType(VMP_MEMBERTYPE_MCSSPEC);
//			tVmpMember.SetMemberTMt(GetLocalMtFromOtherMcuMt(tNewRollCallee));
			// [pengjie 2010/4/14] 这里不能填local化得信息
			tVmpMember.SetMemberTMt(tNewRollCallee);
			tVmpMember.SetMemStatus(MT_STATUS_AUDIENCE);
			tVmpParam.SetVmpMember(byCalleePos, tVmpMember);
			
		}
	}
	
	//调整选看
	// zbq [11/24/2007] VMP时，开启点名，VMP模式，需手动清当前视频选看；其他情况下的选看都会自动被冲掉.
	// 将除点名人和被点名人之外的其他所有终端的选看全停掉
	if ( ( tOldRollCaller.IsNull() || tOldRollCallee.IsNull() ) &&
		ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
	{
		for( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId ++ )
		{
			// miaoqingsong [20110916] 过滤上级MCU选看上传通道终端的选看
			if (!m_tCascadeMMCU.IsNull() && 
				m_tCascadeMMCU.GetMtId() == byMtId)
			{
				continue;
			}

			if ( m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
				byMtId != tNewRollCaller.GetMtId() &&
				( (tNewRollCallee.IsLocal() && byMtId != tNewRollCallee.GetMtId()) || (!tNewRollCallee.IsLocal() && byMtId != tNewRollCallee.GetMcuId()) ) 
				) 
			{
				TMtStatus tMtStatus;
				if ( m_ptMtTable->GetMtStatus( byMtId, &tMtStatus ) &&
					!tMtStatus.GetSelectMt(MODE_VIDEO).IsNull() )
				{
					TMt tMt = m_ptMtTable->GetMt(byMtId);
					StopSelectSrc(tMt, MODE_VIDEO);
				}
			}
		}
	}
	
	BOOL32 bSelAdjusted = FALSE;
	
	if ( bCallerChged )
	{
		if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
		{
			if (!tOldRollCaller.IsNull())
			{
				StopSelectSrc(tOldRollCaller, MODE_VIDEO);         
			}
			
			//zyl　20121106第一次点名，bCallerChged和bCalleeChged都为true,要停掉点名前的选看
			if (bCalleeChged)
			{
				StopSelectSrc(tNewRollCaller, MODE_VIDEO,FALSE);
			}
			       
			

			//点名人改变了，先停到被点名人的选看，释放资源
			if( tNewRollCallee.IsLocal() )
			{
				StopSelectSrc(tNewRollCallee, MODE_VIDEO,FALSE); 
			}
			
			//选看失败，恢复看广播，可能看vmp或看自己
			//点名人选看被点名人
			if (!ChangeSelectSrc(tNewRollCallee, tNewRollCaller, MODE_VIDEO))
			{
				RestoreRcvMediaBrdSrc(tNewRollCaller.GetMtId(), MODE_VIDEO);
				//[2011/09/15/zhangli]释放回传在ChangeSelectSrc做操作
				//FreeRecvSpy( tNewRollCallee,MODE_VIDEO );
			}
			
			//被点名人选看点名人
			if ( tNewRollCallee.IsLocal() )
			{
				if (!ChangeSelectSrc(tNewRollCaller, tNewRollCallee, MODE_VIDEO))
				{
					RestoreRcvMediaBrdSrc(tNewRollCallee.GetMtId(), MODE_VIDEO);
				}
			}
			bSelAdjusted = TRUE;
		}
		else if ( ROLLCALL_MODE_CALLER == m_tConf.m_tStatus.GetRollCallMode() )
		{
			if ( !tOldRollCaller.IsNull() )
			{
				StopSelectSrc(tOldRollCaller, MODE_VIDEO);                    
			}
			//zyl　20121106第一次点名，bCallerChged和bCalleeChged都为true,要停掉点名前的选看
			if ( bCalleeChged )
			{
				StopSelectSrc(tNewRollCaller,MODE_VIDEO);
			}
			if (!ChangeSelectSrc(tNewRollCallee, tNewRollCaller, MODE_VIDEO))
			{
				RestoreRcvMediaBrdSrc(tNewRollCaller.GetMtId(), MODE_VIDEO);
				//[2011/09/15/zhangli]释放回传在ChangeSelectSrc做操作
				//FreeRecvSpy( tNewRollCallee,MODE_VIDEO );
			}
		}
		else // ROLLCALL_MODE_CALLEE
		{
			//zjj20091031
			if( tNewRollCallee.IsLocal() )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcMcsMcuRollCallMsg] bCallerChged(%d) bCalleeChged(%d) StopSelectSrc(tNewRollCallee)\n",
					bCallerChged,bCalleeChged );
				StopSelectSrc(tNewRollCallee, MODE_VIDEO,FALSE );
			}

			if ( tNewRollCallee.IsLocal() )
			{
				if (!ChangeSelectSrc(tNewRollCaller, tNewRollCallee, MODE_VIDEO))
				{
					RestoreRcvMediaBrdSrc(tNewRollCallee.GetMtId(), MODE_VIDEO);
				}
			}
		}
	}
	if ( bCalleeChged )
	{
		if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
		{
//  [12/21/2009 pengjie]
			/*
 			if ( tOldRollCallee.IsLocal() &&
 				!tOldRollCallee.IsNull() )
 			{
 				StopSelectSrc(tOldRollCallee, MODE_VIDEO);
 			}
            */

			
 			if( bCallerChged &&
 				!tOldRollCaller.IsNull() &&
				!tNewRollCaller.IsNull() 
				)
			{						
				StopSelectSrc(tNewRollCaller, MODE_VIDEO,FALSE);
			}
			
 			if( !bCallerChged )
 			{
 				StopSelectSrc(tNewRollCaller, MODE_VIDEO,FALSE);
 			}

			if ( tOldRollCallee.IsLocal() &&
                !tOldRollCallee.IsNull() )
            {
                StopSelectSrc(tOldRollCallee, MODE_VIDEO);
				//zjl切换一次被点名人，拆掉前一个被点名人的桥交换
				//g_cMpManager.RemoveSwitchBridge(tOldRollCallee, 0, MODE_VIDEO);
            }		
// End
			if ( !bSelAdjusted )
			{
				if (!ChangeSelectSrc(tNewRollCallee, tNewRollCaller, MODE_VIDEO))
				{
					RestoreRcvMediaBrdSrc(tNewRollCaller.GetMtId(), MODE_VIDEO);
					//[2011/09/15/zhangli]释放回传在ChangeSelectSrc做操作
					//FreeRecvSpy( tNewRollCallee,MODE_VIDEO );
				}
				if ( tNewRollCallee.IsLocal() )
				{
					if (!ChangeSelectSrc(tNewRollCaller, tNewRollCallee, MODE_VIDEO))
					{
						RestoreRcvMediaBrdSrc(tNewRollCallee.GetMtId(), MODE_VIDEO);
					}
				}
			}
		}
		else if ( ROLLCALL_MODE_CALLER == m_tConf.m_tStatus.GetRollCallMode() )
		{
			//zjj20091031 
			if( !bCallerChged )
			{
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MCS,  "[ProcMcsMcuRollCallMsg] bCallerChged(%d) bCalleeChged(%d) StopSelectSrc(tOldRollCaller)\n",
					bCallerChged,bCalleeChged );
				StopSelectSrc(tOldRollCaller, MODE_VIDEO,FALSE );
			}
			
			if (!ChangeSelectSrc(tNewRollCallee, tNewRollCaller, MODE_VIDEO))
			{
				RestoreRcvMediaBrdSrc(tNewRollCaller.GetMtId(), MODE_VIDEO);
				//[2011/09/15/zhangli]释放回传在ChangeSelectSrc做操作
				//FreeRecvSpy( tNewRollCallee,MODE_VIDEO );
			}
		}
		else // ROLLCALL_MODE_CALLEE
		{
			if ( tOldRollCallee.IsLocal() &&
				!tOldRollCallee.IsNull() )
			{
				StopSelectSrc(tOldRollCallee, MODE_VIDEO);
			}
			if ( tNewRollCallee.IsLocal() )
			{
				if (!ChangeSelectSrc(tNewRollCaller, tNewRollCallee, MODE_VIDEO))
				{
					RestoreRcvMediaBrdSrc(tNewRollCallee.GetMtId(), MODE_VIDEO);
				}
			}
		}
	}

	if ( ROLLCALL_MODE_VMP == m_tConf.m_tStatus.GetRollCallMode() )
	{
		// 判会议的vmp开启模式
		if ( CONF_VMPMODE_CTRL == g_cMcuVcApp.GetVMPMode(m_tVmpEqp) )
		{
			/*TPeriEqpStatus tPeriEqpStatus;
			g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);
			tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
			g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus); */ 
			
			//ChangeVmpParam(&tVmpParam);
			// xliang [1/6/2009] 区分新老VMP设VMP param
			AdjustVmpParam(tVmpEqp.GetEqpId(), &tVmpParam);
			
		}
		else if ( CONF_VMPMODE_AUTO == g_cMcuVcApp.GetVMPMode(m_tVmpEqp) )
		{		
			/*TPeriEqpStatus tPeriEqpStatus;
			g_cMcuVcApp.GetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus);  
			tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.GetVmpParam();
			g_cMcuVcApp.SetPeriEqpStatus(m_tVmpEqp.GetEqpId() , &tPeriEqpStatus); */ 
			
			//ChangeVmpParam(&tVmpParam);
			// xliang [1/6/2009] 区分新老VMP设VMP param
			AdjustVmpParam(tVmpEqp.GetEqpId(), &tVmpParam);
			
			ConfModeChange();
		}
		else
		{
			CServMsg cMsg;
			cMsg.SetEventId(MCS_MCU_STARTVMP_REQ);
			cMsg.SetMsgBody((u8*)&tVmpParam, sizeof(tVmpParam));
			
			VmpCommonReq(cMsg);
		}
	}
	
	if( !tNewRollCallee.IsLocal() &&
		IsLocalAndSMcuSupMultSpy(tNewRollCallee.GetMcuId())
		)
	{		
		TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tNewRollCallee.GetMcuId()));	
		SendMMcuSpyNotify( tNewRollCallee, EvId, tSimCapSet);		
	}
	
	CServMsg cServMsg;
	cServMsg.SetMsgBody( (u8*)&m_tRollCaller, sizeof(TMt) );
	cServMsg.CatMsgBody( (u8*)&tNewRollCallee, sizeof(TMt) );
	cServMsg.SetEventId( MCU_MCS_CHANGEROLLCALL_NOTIF );
	SendMsgToAllMcs( cServMsg.GetEventId(), cServMsg );
	
	ConfStatusChange();
	
	return;
}

/*==============================================================================
函数名    :  ProcRecStartPreSetInAck
功能      :  处理级联多回传下级终端进上级录像机
算法实现  :  
参数说明  :  [IN] TPreSetInRsp 多回传必要信息
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010/07/19   4.6           陆昆朋							多回传电视墙整理
==============================================================================*/
void CMcuVcInst::ProcRecStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp )
{
	TMt tRecordMt = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();

	// [11/23/2010 liuxu] 通知下级mcu记录回传信息
	if (!tRecordMt.IsLocal() && IsLocalAndSMcuSupMultSpy(tRecordMt.GetMcuId()))
	{
		TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tRecordMt.GetMcuId()));
		SendMMcuSpyNotify( tRecordMt,MCS_MCU_STARTREC_REQ, tSimCapSet);
	}

	const TSpyRecInfo *ptSpyRecInfo = &tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyRecInfo;

	TStartRecMsgInfo tRecMsg;
	tRecMsg.m_tRecordMt = tRecordMt;
	tRecMsg.m_tRecEqp = ptSpyRecInfo->m_tRec;
	tRecMsg.m_tRecPara = ptSpyRecInfo->m_tRecPara;
	tRecMsg.bIsRecAdaptConf = FALSE;

	//  [5/16/2013 guodawei] 多回传录像文件名GBK转换
	s8  achRecFileName[MAX_FILE_NAME_LEN] = {0};
	memcpy(achRecFileName, ptSpyRecInfo->m_szRecFullName, sizeof(achRecFileName) - 1);
	u16 wRecNameLen = htons(strlen(achRecFileName) + 1);
// #ifdef _UTF8
// 	if (g_cMcuVcApp.GetEqpCodeFormat(tRecMsg.m_tRecEqp.GetEqpId()) == emenCoding_GBK)
// 	{
// 		s8 achGBKFileName[MAX_FILE_NAME_LEN] = {0};
// 		u16 wRet = gb2312_to_utf8(achRecFileName, achGBKFileName, sizeof(achGBKFileName) - 1);
// 		wRecNameLen = htons(strlen(achGBKFileName) + 1);
// 		memcpy(achRecFileName, achGBKFileName, sizeof(achGBKFileName));
// 	}
// #endif
	memcpy(tRecMsg.m_aszRecName, achRecFileName, sizeof(achRecFileName));

	// vrs新录播支持
	if (tRecMsg.m_tRecEqp.GetType() == TYPE_MT && tRecMsg.m_tRecEqp.GetMtType() == MT_TYPE_VRSREC)
	{
		StartVrsRec(tRecMsg, TRecChnnlStatus::STATE_RECREADY, FALSE);
	}
	else
	{
		StartMtRec(tRecMsg, FALSE,ptSpyRecInfo);
	}

	return;
}

/*==============================================================================
函数名    :  ProcRecStartPreSetInAck
功能      :  处理级联多回传下级终端进上级回传通道
算法实现  :  
参数说明  :  [IN] TPreSetInRsp 多回传必要信息
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2011/07/06   4.6           苗庆松			                 创建
==============================================================================*/
void CMcuVcInst::ProcDragStartPreSetInAck( const TPreSetInRsp &tPreSetInRsp )
{
	TMt tSrcMt    = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	u8  bySpyMode = tPreSetInRsp.m_byRspSpyMode;
	u32 dwEvId    = tPreSetInRsp.m_tSetInReqInfo.GetEvId();

	TSimCapSet tSimCapSet = GetMtSimCapSetByMode( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuId()));				
	TSimCapSet tDstCapSet = m_ptMtTable->GetDstSCS(m_tCascadeMMCU.GetMtId());
	tSimCapSet.SetVideoMaxBitRate(tDstCapSet.GetVideoMaxBitRate());
	if (tSimCapSet.IsNull())
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, " GetMtSimCapSetByMode is null!\n");
		return;
	}

	SendMMcuSpyNotify( tSrcMt, dwEvId, tSimCapSet);		
	OnSetOutView(tSrcMt, bySpyMode);
}

/*==============================================================================
函数名    :  StopSpeakerFollowSwitch
功能      :  停止发言人跟随的一些交换
算法实现  :  
参数说明  :  byMode 要停止的交换的模式(MODE_VIDEO,MODE_AUDIO,MODE_BOTH)

返回值说明:  

-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
10/03/26					周晶晶							create
==============================================================================*/
void CMcuVcInst::StopSpeakerFollowSwitch( u8 byMode )
{
	TMt tSpeaker = m_tConf.GetSpeaker();
	if( MODE_BOTH == byMode || MODE_VIDEO == byMode  )
	{	
		TPeriEqpStatus tTWStatus;
		u8 byChnlIdx;
		u8 byEqpId;
		for (byEqpId = TVWALLID_MIN; byEqpId <= TVWALLID_MAX; byEqpId++)
		{
			if (EQP_TYPE_TVWALL == g_cMcuVcApp.GetEqpType(byEqpId))
			{
				if (g_cMcuVcApp.GetPeriEqpStatus(byEqpId, &tTWStatus))
				{
					u8 byMemberType;
					u8 byMtConfIdx;
					for (byChnlIdx = 0; byChnlIdx < MAXNUM_PERIEQP_CHNNL; byChnlIdx++)
					{
						byMemberType = tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].byMemberType;
						byMtConfIdx = tTWStatus.m_tStatus.tTvWall.atVideoMt[byChnlIdx].GetConfIdx();
						if (TW_MEMBERTYPE_SPEAKER == byMemberType && m_byConfIdx == byMtConfIdx)
						{
							//zjl[03/01/2010] 发言人跟随，指定下级终端为发言人时，需要级联调分辨率，所以所传源端必须是非local
							ChangeTvWallSwitch(&tSpeaker/*tLocalNewSpeaker*/, byEqpId, byChnlIdx, TW_MEMBERTYPE_SPEAKER, TW_STATE_STOP);
						}
					}
				}
			}
		}

		
		TPeriEqpStatus tHduStatus;
		u8 byHduChnlIdx;
		u8 byHduEqpId;
		for (byHduEqpId = HDUID_MIN; byHduEqpId <= HDUID_MAX; byHduEqpId++)
		{
			if(IsValidHduEqp(g_cMcuVcApp.GetEqp(byHduEqpId)))
			{
				u8 byHduChnNum = g_cMcuVcApp.GetHduChnNumAcd2Eqp(g_cMcuVcApp.GetEqp(byHduEqpId));
				if (0 == byHduChnNum)
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_HDU, "[StopSpeakerFollowSwitch] GetHduChnNumAcd2Eqp failed!\n");
					continue;
				}

				if (g_cMcuVcApp.GetPeriEqpStatus(byHduEqpId, &tHduStatus))
				{
					u8 byMemberType;
					u8 byMtConfIdx;
					for (byHduChnlIdx = 0; byHduChnlIdx < byHduChnNum; byHduChnlIdx++)
					{
						byMemberType = tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].byMemberType;
						byMtConfIdx = tHduStatus.m_tStatus.tHdu.atVideoMt[byHduChnlIdx].GetConfIdx();
						if (TW_MEMBERTYPE_SPEAKER == byMemberType && m_byConfIdx == byMtConfIdx )
						{
							// [2013/03/11 chenbing] HDU多画面不支持发言人跟随,子通道置0
							ChangeHduSwitch(NULL, byHduEqpId, byHduChnlIdx, 0, TW_MEMBERTYPE_SPEAKER, TW_STATE_STOP);
						}
					}
				}
			}
		}
	}
}

/*==============================================================================
函数名    :  FreeSpyChannlInfoByMcuId
功能      :  释放此mcu下的所有回传终端及占用通道
算法实现  :  
参数说明  :  u8 byMcuId McuID
返回值说明: 
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010-06-10                 lukunpeng						modify
==============================================================================*/
// void CMcuVcInst::FreeSpyChannlInfoByMcuId(u8 byMcuId)
// {
// 	CMultiSpyMgr *pcMultiSpyMgr = g_cMcuVcApp.GetCMultiSpyMgr();
// 
// 	if (pcMultiSpyMgr == NULL)
// 	{
// 		return;
// 	}
// 
// 	m_cSMcuSpyMana.FreeSpyChannlInfoByMcuId(byMcuId, pcMultiSpyMgr);
// }

/*==============================================================================
函数名    :  FreeAllRecvSpyByMcuIdx
功能      :  释放某个mcu下的所有回传终端
算法实现  :  
参数说明  :  u16 wMcuIdx 要释放终端所在的mcu的mcuidx            

返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010-08-27                 周晶晶							创建
==============================================================================*/
void CMcuVcInst::FreeAllRecvSpyByMcuIdx( u16 wMcuIdx )
{
	if( INVALID_MCUIDX == wMcuIdx )
	{
		return;
	}
	
	const CRecvSpy *pcRecvSpy;
	u16 wSpyNum = MAXNUM_CONF_SPY/*m_cSMcuSpyMana.GetSpyNum()*/;
	for( u16 wIdx = 0;wIdx < wSpyNum;++wIdx )
	{
		pcRecvSpy = m_cSMcuSpyMana.GetSpyMemberInfo( wIdx );

		if (NULL == pcRecvSpy)
		{
			continue;
		}

		if (pcRecvSpy->m_tSpy.IsNull())
		{
			continue;
		}

		if( pcRecvSpy->m_tSpy.GetMcuIdx() == wMcuIdx )
		{
			//zhouyiliang 20110113 挂下级mcu，同时停掉该mcu下往上级mcu的交换
			if ( !m_tCascadeMMCU.IsNull() ) 
			{
				StopSpyMtSpySwitch( pcRecvSpy->m_tSpy );
			}
			FreeRecvSpy( pcRecvSpy->m_tSpy, MODE_BOTH, TRUE );
		}
	}	
}
/*==============================================================================
函数名    :  FreeRecvSpy
功能      :  释放本级接纳的回传终端所占有的回传资源
算法实现  :  
参数说明  :  tMt        回传源
             bySpyMode  要释放的回传模式（MODE_VIDEO/MODE_BOTH....）
			 bForce     是否要强制释放该回传模式（在回挂断传源、删除或掉线等情况
			             强制释放，默认参数为FALSE 不进行强制释放）

返回值说明:  TRUE       表示释放成功
             FALSE      表示释放失败（说明该回传源可能还有其它目的回传终端，所以
			             还无法释放其回传资源）

-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010-06-10                 lukunpeng						modify
==============================================================================*/
BOOL32 CMcuVcInst::FreeRecvSpy(const TMt tMt, u8 bySpyMode, BOOL32 bForce)
{
	if( tMt.IsNull() || tMt.IsLocal()  )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, "[FreeRecvSpy] tMt(%d.%d) is local or null .can't FreeSpy.\n",
			tMt.GetMcuId(),tMt.GetMtId()
			);
		return FALSE;
	}
	
	//判断此级联会议是否支持多回传
	if( !IsLocalAndSMcuSupMultSpy(tMt.GetMcuId()) )
	{
		ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, "[FreeRecvSpy] tMt's mcu (%d.%d) can't Support MultiSpy.\n",
			tMt.GetMcuId(),tMt.GetMtId()
			);
		return FALSE;
	}
	
	//回传终端目的数减一，如果可能释放SpyMode，就返回释放带宽，如果可以释放Chnnl,返回ChnnlID
	u32 dwReleaseBW = 0;
	s16 swSpyChnnlID = -1;

	//[2011/11/09/zhangli]加两个标志，是否清除rtcp信息，因为在ProcMcuMcuMtExtInfoNotif里建rtcp交换是会根据这里保存的判断是否新建
	BOOL32 bIsRemoveVidRtcp = FALSE;
	BOOL32 bIsRemoveAudRtcp = FALSE;

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "FreeRecvSpy McuId:%d, MtId:%d, SpyMode:%d, bForce:%d\n", 
		tMt.GetMcuId(), tMt.GetMtId(), bySpyMode, bForce);

	CRecvSpy cRecvSpyInfo;
	if( !m_cSMcuSpyMana.GetRecvSpy(tMt, cRecvSpyInfo) )
	{
		ConfPrint( LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "FreeRecvSpy GetRecvSpy Failed mcu.%dmt.%d\n", \
			tMt.GetMcuId(), tMt.GetMtId() );
		return FALSE;
	}

	//20110818 zjl 要求释放的模式和实际回传的模式取交集
	bySpyMode = (cRecvSpyInfo.GetSpyMode() & bySpyMode);
	if (bySpyMode == 0)
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[FreeRecvSpy] No simultaneous between SpyMode.%d and FreeMode.%d!\n",
													cRecvSpyInfo.GetSpyMode(), bySpyMode);
		return FALSE;
	}

	u8 byRelSpyMode = MODE_NONE;

	if (bForce)
	{
		if (!m_cSMcuSpyMana.RemoveSpyMode(tMt, bySpyMode, dwReleaseBW, swSpyChnnlID, byRelSpyMode))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[FreeRecvSpy] tMt's mcu (%d.%d) RemoveSpyMode Error\n",
				tMt.GetMcuId(),tMt.GetMtId()
				);
			return FALSE;
		}
	}
	else
	{	
		if (!m_cSMcuSpyMana.DecAndAdjustSpyMode(tMt, bySpyMode, dwReleaseBW, swSpyChnnlID, byRelSpyMode))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[FreeRecvSpy] tMt's mcu (%d.%d) DecAndAdjustSpyMode Error\n",
				tMt.GetMcuId(),tMt.GetMtId()
				);
			return FALSE;
		}
	}

	//获得此mcu的总带宽及剩余带宽
	u32 dwMaxSpyBW = 0;
	s32 nRemainSpyBW = 0;
	if (!GetMcuMultiSpyBW(tMt.GetMcuId(), dwMaxSpyBW, nRemainSpyBW))
	{
		return FALSE;
	}
	
	//释放此终端的带宽
	SetMcuSupMultSpyRemainBW(tMt.GetMcuId(), nRemainSpyBW + (s32)dwReleaseBW);
	
	if ( !m_tConfAllMcuInfo.IsSMcuByMcuIdx( tMt.GetMcuId() ) ||
		byRelSpyMode != MODE_NONE
		)
	{
		//[liu lijiu][20100828]拆除建立的RTCP交换
		TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
		if (tConfAttrb.IsResendLosePack() )
		{
			u32 dwRcvMpIp = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( GetLocalMtFromOtherMcuMt(tMt).GetMtId()));
			//拆除视频RTCP交换
			if(MODE_VIDEO == byRelSpyMode || MODE_BOTH == byRelSpyMode)
			{
				g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRcvMpIp, cRecvSpyInfo.m_tSpyAddr.GetPort() + 1, cRecvSpyInfo.m_tVideoRtcpAddr.GetIpAddr(),
					                                 cRecvSpyInfo.m_tVideoRtcpAddr.GetPort());
				bIsRemoveVidRtcp = TRUE;
			}
			
			//拆除音频RTCP交换
			if(MODE_AUDIO == byRelSpyMode || MODE_BOTH == byRelSpyMode)
			{
				g_cMpManager.RemoveMultiToOneSwitch(m_byConfIdx, dwRcvMpIp, cRecvSpyInfo.m_tSpyAddr.GetPort() + 3, cRecvSpyInfo.m_tAudioRtcpAddr.GetIpAddr(),
					                                  cRecvSpyInfo.m_tAudioRtcpAddr.GetPort());
				bIsRemoveAudRtcp = TRUE;
			}			
		}

		// [11/29/2010 xliang] 是否要拆交换使能
		u8 bySpyNoUse = 0;
		bySpyNoUse |= byRelSpyMode;

		if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx(tMt.GetMcuId()) )
		{
			byRelSpyMode = bySpyMode;
		}				
		
		//通知下级，释放此SpyMode
		OnMMcuBanishSpyCmd( tMt, byRelSpyMode, bySpyNoUse );
	}
	
	if (bIsRemoveAudRtcp || bIsRemoveVidRtcp)
	{
		TTransportAddr tVideoRtcpAddr;
		tVideoRtcpAddr.SetNull();
		s16 swSpyIndex = m_cSMcuSpyMana.FindSpyMt(tMt);
		
		if (bIsRemoveAudRtcp)
		{
			m_cSMcuSpyMana.SetSpyBackAudRtcpAddr(swSpyIndex, MODE_AUDIO, tVideoRtcpAddr);	
		}
		
		if (bIsRemoveVidRtcp)
		{
			m_cSMcuSpyMana.SetSpyBackVidRtcpAddr(swSpyIndex, MODE_VIDEO, tVideoRtcpAddr);	
		}
	}

	//如果返回的chnnID为 -1的话，说明此通道还在使用，不能释放
	if (swSpyChnnlID == -1)
	{
		//zjl20101117如果当前回传终端还在上级业务里(尚未释放)，则需要更新其回传目的能力集
		UpdateCurSpyDstCapSet(tMt);
		return TRUE;
	}
	
	//开始释放此通道
	CMultiSpyMgr *pcMultiSpyMgr = g_cMcuVcApp.GetCMultiSpyMgr();
	if( NULL == pcMultiSpyMgr )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "GetCMultiSpyMgr False! \n" );
		return FALSE;
	}
	pcMultiSpyMgr->ReleaseSpyChnnl(swSpyChnnlID);
	
	return TRUE;
}

/*==============================================================================
函数名    :  OnMMcuBanishSpyCmd
功能      :  上级发给下级的释放回传资源
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-10-15                 pengjie							create
==============================================================================*/
void CMcuVcInst::OnMMcuBanishSpyCmd(const TMt &tSrcMt, u8 bySpyMode, u8 bySpyNoUse )
{
	CServMsg cServMsg;
	TMsgHeadMsg tHeadMsg;


	cServMsg.SetEventId( MCU_MCU_BANISHSPY_CMD );
	TMt tMt;
	tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tSrcMt,tMt );

	cServMsg.SetMsgBody( (u8*)&tHeadMsg, sizeof(TMsgHeadMsg) );
	cServMsg.CatMsgBody( (u8 *)&tMt, sizeof(TMt) );
	cServMsg.CatMsgBody( (u8 *)&(bySpyMode), sizeof(u8) );
	cServMsg.CatMsgBody( (u8 *)&(bySpyNoUse), sizeof(bySpyNoUse) );

	
	SendMsgToMt( GetFstMcuIdFromMcuIdx(tSrcMt.GetMcuIdx()), MCU_MCU_BANISHSPY_CMD, cServMsg );
	return;
}

/*==============================================================================
函数名    :  ProcMcuMcuPreSetInNack
功能      :  下级回传带宽不足，回NACK
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-10-15                 pengjie							create
==============================================================================*/
void CMcuVcInst::ProcMcuMcuPreSetInNack( const CMessage *pcMsg )
{
	STATECHECK;	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	TPreSetInRsp tPreSetInRsp = *(TPreSetInRsp *)(cServMsg.GetMsgBody());
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody()+sizeof(TPreSetInRsp));
	TMt tSrc = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
	TEqp tDst = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_tVmp;
	
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  
		"[PreSetInNack]Req Mt(%d,%d)nacked, Error.%d EVENT %u(%s), RlsMt(%d,%d,%d)\n",
		tSrc.GetMcuIdx(), 
		tSrc.GetMtId(),
		cServMsg.GetErrorCode(),
		tPreSetInRsp.m_tSetInReqInfo.GetEvId(),
		::OspEventDesc((u16)tPreSetInRsp.m_tSetInReqInfo.GetEvId()),
		tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_tMt.GetMcuIdx(),
		tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_tMt.GetMtId(),
		tPreSetInRsp.m_tSetInReqInfo.m_tReleaseMtInfo.m_abyMtIdentify[0]
		);

	if( !m_tCascadeMMCU.IsNull() &&
		tHeadMsg.m_tMsgSrc.m_byCasLevel < MAX_CASCADELEVEL &&
		tHeadMsg.m_tMsgDst.m_byCasLevel > 0
		)
	{
		CServMsg cMsg;
		cMsg.SetErrorCode(cServMsg.GetErrorCode());
		cMsg.SetEventId( MCU_MCU_PRESETIN_NACK );
		cMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
		cMsg.CatMsgBody( (u8*)&tPreSetInRsp,sizeof(TPreSetInRsp) );
		SendMsgToMt( m_tCascadeMMCU.GetMtId(),cMsg.GetEventId(),cMsg );
		return;
	}

	tSrc = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc,tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt() );
	// [12/15/2009 xliang] FIXME: 判errorcode. 对于VMP，可能再给次preSetIn的机会// [3/16/2011 xliang] no way
	switch (cServMsg.GetErrorCode())
	{
// 	case  CASCADE_ADJRESNACK_REASON_NONEKEDA:
// 		{
// 			if( tDst.GetType() == TYPE_MCUPERI && tDst.GetEqpType() == EQP_TYPE_VMP && !tDst.IsNull() )
// 			{
// 				// tVmpParam对于定制风格来说是可以正确获取的
// 				TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
// 				ChangeMtVideoFormat(tSrc, &tVmpParam, TRUE, TRUE, TRUE);
// 			}
// 			break;
// 		}
		// [1/4/2011 xliang] 重整
	case ERR_MCU_CONFSNDBANDWIDTHISFULL:
	case ERR_MCU_CASDBASISNOTENOUGH:
	default:
		{	
			//点名标志置回，允许下次点名
			if ( tPreSetInRsp.m_tSetInReqInfo.GetEvId() == MCS_MCU_CHANGEROLLCALL_REQ || 
				 (tPreSetInRsp.m_tSetInReqInfo.GetEvId() == MCS_MCU_STARTVMP_REQ && m_tConf.m_tStatus.GetRollCallMode() == ROLLCALL_MODE_VMP )
			   )
			{
				StaticLog("[ProcMcuMcuPreSetInNack]Recover Rollcall finish Mark \n") ;
				SetLastMultiSpyRollCallFinish(TRUE);
			}
			BOOL32 bAlarm = TRUE;
			u8 byLoop1 = 0,byLoop = 0;
			TConfMcInfo *ptMcInfo = NULL;
			TConfMtInfo *ptConfMtInfo = NULL;

			BOOL32 bInConf = FALSE,bInJoinedConf = FALSE;
			u8 byMcuId = 0;
			TVMPParam_25Mem tConfVmpParam;
			TEqp tVmpEqp = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_tVmp;
			if( tPreSetInRsp.m_tSetInReqInfo.GetEvId() == MCS_MCU_STARTVMP_REQ )
			{
				tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
				TPeriEqpStatus tPeriEqpStatus; 
				if (  g_cMcuVcApp.GetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus ) && !tVmpEqp.IsNull() )
				{
					LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_EQP,"[ProcMcuMcuPreSetInNack] vmpstatus:%d!\n", tPeriEqpStatus.m_tStatus.tVmp.m_byUseState);
					
					switch(tPeriEqpStatus.m_tStatus.tVmp.m_byUseState)
					{
					case TVmpStatus::RESERVE://如果回nack且RESERVE状态，清vmp状态
						{
							tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::IDLE;
							tPeriEqpStatus.SetConfIdx( 0 );
							// Bug00082245,对下级终端,开启时,收到PresetinNack,将vmp状态置idle,同时需要kill掉此Timer
							u8 byVmpIdx = tVmpEqp.GetEqpId() - VMPID_MIN;
							KillTimer(MCUVC_WAIT_ALLVMPPRESETIN_ACK_TIMER+byVmpIdx);
							
							//状态有变，告知界面
							g_cMcuVcApp.SetPeriEqpStatus( tVmpEqp.GetEqpId(), &tPeriEqpStatus );
							cServMsg.SetMsgBody((u8 *)&tPeriEqpStatus, sizeof(tPeriEqpStatus));
							SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);
						}
						break;
					case TVmpStatus::WAIT_START://等待外设开启中，不做处理
						break;
					case TVmpStatus::START:
						{
							//恢复presetin之前的curUseVmpChanInd，如本来要变4画面，通道满，只能3画面，curUseVmpChan就不再是1，而是原来的3了
							if (tConfVmpParam.IsVMPAuto() && VCS_CONF == m_tConf.GetConfSource())
							{
								u16 byCurUseChnId = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos;
								m_cVCSConfStatus.SetCurUseVMPChanInd( byCurUseChnId );
								TMt tMtNull;
								tMtNull.SetNull();
								m_cVCSConfStatus.SetReqVCMT( tMtNull );	
								ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMcuMcuPreSetInNack] Update vcs CurUseVMPChanInd.%d:\n",byCurUseChnId);
							}
							//若此通道为vmp单通道轮询时,需要立即轮询下一终端,8ki功能，暂不支持
							
							cServMsg.SetEqpId(tVmpEqp.GetEqpId());
							cServMsg.SetMsgBody( (u8*)&tConfVmpParam, sizeof(tConfVmpParam) );
							SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg ); //告诉会控刷界面
							//更新EqpstatusVmpParam信息，与m_tConf一致
							tPeriEqpStatus.m_tStatus.tVmp.SetVmpParam(tConfVmpParam);
						}
						break;
					default:
						break;
					}
					//else /*if(tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam.IsMtInMember(tSrc))*/
					/*{
						if (m_tConf.m_tStatus.m_tVMPParam.IsVMPAuto())
						{
							//恢复curUsechn和老style
							u8 byStyl = m_tLastVmpParam.GetVMPStyle();
							m_tConf.m_tStatus.m_tVMPParam.SetVMPStyle( byStyl );
							
							//恢复presetin之前的curUseVmpChanInd，如本来要变4画面，通道满，只能3画面，curUseVmpChan就不再是1，而是原来的3了
							if ( VCS_CONF == m_tConf.GetConfSource() ) 
							{
								u16 byCurUseChnId = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos;
								m_cVCSConfStatus.SetCurUseVMPChanInd( byCurUseChnId );
								TMt tMtNull;
								tMtNull.SetNull();
								m_cVCSConfStatus.SetReqVCMT( tMtNull );				
								ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[ProcMcuMcuPreSetInNack] Update vcs CurUseVMPChanInd.%d:\n",byCurUseChnId);
							}
							else if(MCS_CONF == m_tConf.GetConfSource() )
							{
								//do nothing
							}
							
						}
						else //custom vmp
						{
							//u8 byChl = m_tConf.m_tStatus.m_tVMPParam.GetChlOfMtInMember( tSrc );
							u8 byChl = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyVmpInfo.m_byPos;
							// 加保护,防止取得的Chl不正确
							if (byChl >= MAXNUM_VMP_MEMBER)
							{
								ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuPreSetInNack]mt(%d,%d) can't join in vmp channel(%d)!\n",
									tSrc.GetMcuId(),tSrc.GetMtId(), byChl );
								break;
							}
							m_tConf.m_tStatus.m_tVMPParam.ClearVmpMember( byChl );
							
							//原通道成员恢复
							TVMPMember *pVmpMember = m_tLastVmpParam.GetVmpMember(byChl);
							
							if (pVmpMember != NULL)
							{
								m_tConf.m_tStatus.m_tVMPParam.SetVmpMember(byChl, *pVmpMember);
								// 若此通道为vmp单通道轮询时,需要立即轮询下一终端
								if (VMP_MEMBERTYPE_VMPCHLPOLL == pVmpMember->GetMemberType() && POLL_STATE_NONE != m_tVmpPollParam.GetPollState())
								{
									TVmpPollInfo tPollInfo = m_tVmpPollParam.GetVmpPollInfo();
									u8 byPollIdx = m_tVmpPollParam.GetCurrentIdx();
									TMtPollParam * ptCurPollMt = GetNextMtPolledForVmp(byPollIdx, tPollInfo);
									// 若要轮询的下一个终端就是当前终端,表示仅有此终端满足轮询条件,但此终端又不适合进vmp,停轮询
									if (!ptCurPollMt || tSrc == *ptCurPollMt )
									{
										ProcStopVmpPoll();
									} else
									{
										SetTimer(MCUVC_VMPPOLLING_CHANGE_TIMER, 10);
										bAlarm = FALSE;
									}
								}
							}
						}
						
						
					}
					
					// vmp单通道轮询时,收到nack后需要立即轮询下一终端,会控先不刷界面
					if (bAlarm)
					{
						cServMsg.SetMsgBody( (u8*)&m_tConf.m_tStatus.m_tVMPParam, sizeof(TVMPParam) );
						SendMsgToAllMcs( MCU_MCS_VMPPARAM_NOTIF, cServMsg ); //告诉会控刷界面
					}
					
					tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
					//如果画面合参数里面没有一个成员，将vmp停掉
					if (m_tConf.m_tStatus.m_tVMPParam.GetVMPMemberNum() == 0
						&& !m_tConf.m_tStatus.m_tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_DSTREAM)
						&& !m_tConf.m_tStatus.m_tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_VMPCHLPOLL)
						&& !m_tConf.m_tStatus.m_tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_POLL)
						&& !m_tConf.m_tStatus.m_tVMPParam.IsTypeInMember(VMP_MEMBERTYPE_SPEAKER)) //如果还有跟随通道，则不停VMP
					{
						//放弃画面合成器,WAIT_START在上面处理,若为START状态,则更新为WAIT_STOP,
						if (tPeriEqpStatus.m_tStatus.tVmp.m_byUseState == TVmpStatus::START)
						{
							SetTimer(MCUVC_VMP_WAITVMPRSP_TIMER, TIMESPACE_WAIT_VMPRSP);
							tPeriEqpStatus.m_tStatus.tVmp.m_byUseState = TVmpStatus::WAIT_STOP;
						}
						CServMsg cTempServ;
						SendMsgToEqp(m_tVmpEqp.GetEqpId(), MCU_VMP_STOPVIDMIX_REQ, cTempServ); 
						if (m_tConf.m_tStatus.GetVmpParam().IsVMPBrdst())
						{
							ChangeVidBrdSrc(NULL);
						}
					}
					else // 防止仅有1个成员时,替换失败恢复后,过1.5秒又将vmp停掉
					{
						KillTimer(MCUVC_WAIT_ALLVMPPRESETIN_ACK_TIMER);
					}
					g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
					cServMsg.SetMsgBody((u8 *)&tPeriEqpStatus, sizeof(tPeriEqpStatus));
					SendMsgToAllMcs(MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cServMsg);*/

				}
			}
			else if( tPreSetInRsp.m_tSetInReqInfo.GetEvId() == MCUVC_POLLING_CHANGE_TIMER )
			{			
				KillTimer(MCUVC_POLLING_CHANGE_TIMER);
				//ProcPollingChangeTimerMsg(pcMsg);
				tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.SetSpyMt(tSrc);
				ProcMcuMcuSpyBWFull( pcMsg,&tPreSetInRsp );
				bAlarm = FALSE;
			}
			//点名失败,恢复被点名人
			else if( MCS_MCU_CHANGEROLLCALL_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() )
			{
				
				m_tRollCallee = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyRollCallInfo.m_tOldCallee;
				if (m_tConf.m_tStatus.GetRollCallMode() == ROLLCALL_MODE_VMP) 
				{
// 					m_tConfInStatus.SetLastVmpRollCallFinished(TRUE);
					//zhouyiliang 20110330如果第一次点名就不成功，此时应该将点名人也清空。
					tConfVmpParam = g_cMcuVcApp.GetConfVmpParam(tVmpEqp);
					if ( tConfVmpParam.GetVMPStyle() == VMP_STYLE_NONE ||
						 tConfVmpParam.GetVMPStyle() == 0
						) 
					{
						m_tRollCaller.SetNull();
					}
				}
			}
			else if ( MCS_MCU_START_SWITCH_TW_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() 
				&& TW_MEMBERTYPE_TWPOLL == tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyTVWallInfo.m_bySrcMtType )
			{
				//20110519_tzy 立刻启动该通道轮训定时器，否则要等到该终端轮询时间用完才会开启下一个轮询
				
				/*u8 byTwId = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyTVWallInfo.m_tTvWall.GetEqpId();
				u8 byChnlIdx =  tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyTVWallInfo.m_byDstChlIdx;

				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuPreSetInNack]mt(%d,%d) can't join in (tvw(%d) channel(%d)!\n",
					tSrc.GetMcuId(),tSrc.GetMtId(),byTwId, byChnlIdx );

				u32 dwTimerIdx = 0;
				if( m_tTWMutiPollParam.GetTimerIdx(byTwId, byChnlIdx, dwTimerIdx) )
				{
					ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY, "dwTimerIdx is %d\n", dwTimerIdx);

					KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
					CMessage cMsg;
					memset(&cMsg, 0, sizeof(cMsg));
					cMsg.event = u16(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
					cMsg.content = (u8 *)&dwTimerIdx;
					cMsg.length  = sizeof(u32);
				    ProcTWPollingChangeTimerMsg(&cMsg);
				}*/
				tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.SetSpyMt(tSrc);
				ProcMcuMcuSpyBWFull( pcMsg,&tPreSetInRsp );
			}
			// HDU poll: member should be changed otherwise the SpyNum counting will be wrong lator
			else if ( MCS_MCU_START_SWITCH_HDU_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() 
				&& TW_MEMBERTYPE_TWPOLL == tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyHduInfo.m_bySrcMtType )
			{	
				tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.SetSpyMt(tSrc);
				ProcMcuMcuSpyBWFull( pcMsg,&tPreSetInRsp );
				//20110421_tzy Bug00052315电视墙轮询，第三级回传带宽被占满的时候，第二级回传带宽不释放
				//故不能将该通道中终端清掉，否则在CHANGEHDUSWITCH中就无法获得老终端，无法对老终端做FREE操作，导致带宽释放出问题

				/*u8 byHduId = tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyHduInfo.m_tHdu.GetEqpId();
				u8 byChnlIdx =  tPreSetInRsp.m_tSetInReqInfo.m_tSpyInfo.m_tSpyHduInfo.m_byDstChlIdx;
				
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuPreSetInNack]mt(%d,%d) can't join in (hdu(%d) channel(%d)!\n",
					tSrc.GetMcuId(),tSrc.GetMtId(),byHduId, byChnlIdx );
				
				//20110519_tzy 立刻启动该通道轮训定时器，否则要等到该终端轮询时间用完才会开启下一个轮询
				u32 dwTimerIdx = 0;
				if( m_tTWMutiPollParam.GetTimerIdx(byHduId, byChnlIdx, dwTimerIdx) )
				{
					KillTimer(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
					CMessage cMsg;
					memset(&cMsg, 0, sizeof(cMsg));
					cMsg.event = u16(MCUVC_TWPOLLING_CHANGE_TIMER + dwTimerIdx);
					cMsg.content = (u8*)&dwTimerIdx;
					cMsg.length  = sizeof(u32);
				    ProcTWPollingChangeTimerMsg(&cMsg);
				}*/
			}
			else if( VCS_CONF == m_tConf.GetConfSource() &&
				MCS_MCU_SPECSPEAKER_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() &&
				CONF_SPEAKMODE_ANSWERINSTANTLY == m_tConf.GetConfSpeakMode() &&
				!m_tApplySpeakQue.IsQueueNull()
				)
			{				
				TMt tVCMT;
				//TMt tCurMt = tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.GetSpyMt();
				if(  m_tApplySpeakQue.GetQueueNextMt( tSrc,tVCMT ) )
				{
					ChgCurVCMT( tVCMT );	
				}
				else
				{
					ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_VCS,"[ProcMcuMcuPreSetInNack] Fail to get Queue NextMt.tSrc(%d.%d)\n",
						tSrc.GetMcuId(),tSrc.GetMtId()
						);
				}
			}

			// vrs新录播下级终端录像收到Nack后需要挂断新录播实体
			if (MCS_MCU_STARTREC_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId())
			{
				u8 byVrsRecId = GetVrsRecMtId(tSrc);
				if (m_ptMtTable->GetRecChlType(byVrsRecId) == TRecChnnlStatus::TYPE_RECORD)
				{
					ReleaseVrsMt(byVrsRecId);
				}
			}
			
			//20101111_tzy VCS预览模式下都要将该终端设置到相应电视墙中		
			if(  MCS_MCU_START_SWITCH_HDU_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() ||
				MCS_MCU_START_SWITCH_TW_REQ == tPreSetInRsp.m_tSetInReqInfo.GetEvId() 
				)
			{
				tPreSetInRsp.m_tSetInReqInfo.m_tSpyMtInfo.SetSpyMt( tSrc );
				SetMtInTvWallAndHduInFailPresetinAndInReviewMode( tPreSetInRsp );
			}

			if( bAlarm )
			{
				if( ERR_MCU_CASDBASISNOTENOUGH == cServMsg.GetErrorCode() )
				{
					NotifyMcsAlarmInfo(0, ERR_MCU_CASDBASISNOTENOUGH);
				}
				else if(ERR_MCU_CONFSNDBANDWIDTHISFULL == cServMsg.GetErrorCode())
				{
					NotifyMcsAlarmInfo(0, ERR_MCU_CONFSNDBANDWIDTHISFULL);
				}
				else if(ERR_MCU_SPYMTSTATE_WAITFREE == cServMsg.GetErrorCode())
				{
					NotifyMcsAlarmInfo(0, ERR_MCU_SPYMTSTATE_WAITFREE);
				}
			}
		}
		break;
	}
	
	OnPresetinFailed( tSrc );	
}

/*=============================================================================
    函 数 名： ProcMcuMcuMultSpyCapNotif
    功    能： 处理 级联多回传能力互探（在呼入某个下级mcu时进行）
    算法实现： 
    全局变量： 
    参    数： const CMessage * pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
	2010/07/19  4.6			陆昆朋				  添加下级支持多回传带宽
=============================================================================*/
void CMcuVcInst::ProcMcuMcuMultSpyCapNotif( const CMessage * pcMsg )
{
	STATECHECK;	
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );	

	if( cServMsg.GetEventId() == MCU_MCU_MULTSPYCAP_NOTIF )
	{
        u8 bySmcuid = cServMsg.GetSrcMtId();
		u16 wMcuIdx = GetMcuIdxFromMcuId( bySmcuid );
		m_ptConfOtherMcTable->SetMcuSupMultSpy( wMcuIdx );
		u32 dwMaxSpyBW = *(u32 *)(cServMsg.GetMsgBody() + sizeof(TMcu));
		dwMaxSpyBW = ntohl( dwMaxSpyBW );
		m_ptConfOtherMcTable->SetMcuSupMultSpyMaxBW(wMcuIdx, dwMaxSpyBW);
		m_ptConfOtherMcTable->SetMcuSupMultSpyRemainBW(wMcuIdx, (s32)dwMaxSpyBW);
		
		// [chendaiwei 2010/09/11]
		// VCS 一级MCU与二级MCU级联时，根据本级调度席模板配置，判断下级调度席所需要的最大带宽值，如果
		// 该值大于下级调度席回传带宽值，可能会出现调度失败，需要通知用户
		if( MT_TYPE_SMCU == m_ptMtTable->GetMtType(bySmcuid) 
			&& VCS_CONF  == m_tConf.GetConfSource() 
			&& m_tConf.IsSupportMultiSpy() )
		{
			u32 dwSMCUMaxBw = GetVscSMCUMaxBW();
			
			//计算下级调度席需要的最大带宽值失败
			if(dwSMCUMaxBw == 0)
			{
				return;
			}
			
			//回传带宽小于下级调度席最大带宽
			if(dwMaxSpyBW < dwSMCUMaxBw)
			{
				NotifyMcsAlarmInfo( cServMsg.GetSrcSsnId(), ERR_MCU_VCS_SMCUSPYBWISNOTENOUGH );
			}	
		}
		SendConfExtInfoToMcs(0, bySmcuid);
	}

	return;
}

/*==============================================================================
函数名    :  ProcMcuMcuSpyNotify
功能      :  下级保存上级为其分配的回传信息
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-10-15                 pengjie							create
==============================================================================*/
void CMcuVcInst::ProcMcuMcuSpyNotify( const CMessage *pcMsg )
{
	STATECHECK;	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	if( !IsSupportMultiSpy() )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuSpyNotify] Conf Is Not Support MultiSpy.\n" );		
		return;
	}
	u16 wMsgLen = cServMsg.GetMsgBodyLen();
    TSpyResource tSrcSpyInfo = *(TSpyResource *)( cServMsg.GetMsgBody() );
	TMt tOrgSrc =  tSrcSpyInfo.GetSpyMt(); //[nizhijun 2010/12/15] 这里保存下，为下面返回给上级RTCP信息时使用
	u32 dwEvId = *(u32 *)( cServMsg.GetMsgBody() + sizeof(TSpyResource) );
	dwEvId = ntohl(dwEvId);
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)( cServMsg.GetMsgBody() + sizeof(TSpyResource) + sizeof(u32) /*+ sizeof(u8)*/ );
	
	// 消息体若有包含宽高,则记录宽高
	u8* pbyBuf = cServMsg.GetMsgBody() + sizeof(TSpyResource) + sizeof(u32) + sizeof(TMsgHeadMsg);
	u32 dwResW = 0;
	u32 dwResH = 0;
	if (wMsgLen > sizeof(TSpyResource) + sizeof(u32) + sizeof(TMsgHeadMsg) + sizeof(u32))
	{
		dwResW = *(u32 *)pbyBuf;
		pbyBuf += sizeof(u32);
		dwResH = *(u32 *)pbyBuf;
		//将宽高由网络序转为主机序
		dwResW = ntohl(dwResW);
		dwResH = ntohl(dwResH);

		u8 byRes = GetMcuSupportedRes((u16)dwResW, (u16)dwResH);
		// 标记:v4r7mcu收到有宽高信息时,先保证bas处理正常,将宽高对应分辨率更新到tSrcSpyInfo能力中,bas之后对应
		TSimCapSet tCap = tSrcSpyInfo.GetSimCapset();
		tCap.SetVideoResolution(byRes);
		tSrcSpyInfo.SetSimCapset(tCap);
	}

	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "ProcMcuMcuSpyNotify: SpySrc(Mtid): %d, MMcu Give SpyChnnl: %d, SpyMode: %d, SpyRes:%d, dwResWH[%d,%d]\n",
		tSrcSpyInfo.m_tSpy.GetMtId(), tSrcSpyInfo.m_tSpyAddr.GetPort(), tSrcSpyInfo.m_bySpyMode, tSrcSpyInfo.GetSimCapset().GetVideoResolution(), dwResW, dwResH);


	if( !tSrcSpyInfo.m_tSpy.IsMcuIdLocal() )
	{		

		u8 byMcuId = (u8)tSrcSpyInfo.m_tSpy.GetMcuId();
		u16 wSrcMcuIdx = GetMcuIdxFromMcuId( &byMcuId );
		TMt tSrcSpyMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgDst,tSrcSpyInfo.m_tSpy );//GetMcuIdxMtFromMcuIdMt( tSrcSpyInfo.m_tSpy );
			
		if( IsLocalAndSMcuSupMultSpy(wSrcMcuIdx) )
		{
			CRecvSpy tSpyInfo;
			if( !m_cSMcuSpyMana.GetRecvSpy( tSrcSpyMt, tSpyInfo ) )
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuSpyNotify] Fail to GetSpyChannlInfo.tSrc(%d.%d) ! \n",
					tSrcSpyInfo.m_tSpy.GetMcuId(),tSrcSpyInfo.m_tSpy.GetMtId()
					);
				return;
			}
			m_cSMcuSpyMana.ModifyUseState( tSrcSpyMt,TSpyStatus::NORMAL );
			
			tSpyInfo.m_tSpy = tSrcSpyInfo.m_tSpy;//BuildMultiCascadeMtInfo( tPreSetInReq.m_tSrc );	

			TSpyResource tSpyResource;
			tSpyInfo.GetSpyResource( tSpyResource );

			//zjl20101116本级保存回传终端目的能力集
			TSimCapSet tSimCapSet = GetMtSimCapSetByMode(u8(tSrcSpyInfo.GetSpyMt().GetMcuId()));
			
			//zjl20110117 如果二三级级联通道格式和第一级要求第二级回传的格式相同，则分辨率取小
			if(!tSimCapSet.IsNull() &&
				(tSrcSpyInfo.GetSimCapset().GetVideoMediaType() == tSimCapSet.GetVideoMediaType()))
			{
				u8 byMinRes = GetMinResAcdWHProduct(tSimCapSet.GetVideoResolution(),
													tSrcSpyInfo.GetSimCapset().GetVideoResolution());
				
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuSpyNotify] GetMinRes %d between DstRes:%d and SrcRes:%d!\n",
											byMinRes, 
											tSrcSpyInfo.GetSimCapset().GetVideoResolution(),
											tSimCapSet.GetVideoResolution());
				
				if (VIDEO_FORMAT_INVALID != byMinRes)
				{
					tSimCapSet.SetVideoResolution(byMinRes);
				}
				
				// [2013/05/14 chenbing] 帧率取小
				u8 byMinFps = 0;
				if ( MEDIA_TYPE_H264 == tSimCapSet.GetVideoMediaType() )
				{
					byMinFps = tSrcSpyInfo.GetSimCapset().GetUserDefFrameRate() >= tSimCapSet.GetUserDefFrameRate() 
						? tSimCapSet.GetUserDefFrameRate()
						: tSrcSpyInfo.GetSimCapset().GetUserDefFrameRate();
				
					tSimCapSet.SetUserDefFrameRate(byMinFps);
				}
				else
				{
					byMinFps = tSrcSpyInfo.GetSimCapset().GetVideoFrameRate() >= tSimCapSet.GetVideoFrameRate() 
						? tSimCapSet.GetVideoFrameRate()
						: tSrcSpyInfo.GetSimCapset().GetVideoFrameRate();
					
					tSimCapSet.SetUserDefFrameRate(byMinFps);
				}
			}

			//zjl20101116 如果当前终端已回传则能力集要与已回传目的能力集取小	
			if (!GetMinSpyDstCapSet(tSrcSpyMt, tSimCapSet))
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuSpyNotify] tSimCapSet is null!\n" );
				return;
			}

			//zjl20101116保存(更新)下级回传目的能力集
			m_cSMcuSpyMana.SaveSpySimCap(tSrcSpyMt, tSimCapSet);

			u16 wTmpResW = 0;
			u16 wTmpResH = 0;
			// 对分辨率进行调整,可能下级是旧mcu,需发共通分辨率,真实分辨率宽高在最后追加
			u8 byCommonRes = tSimCapSet.GetVideoResolution();
			GetWHByRes(byCommonRes, wTmpResW, wTmpResH);
			// 根据分辨率,获得对应通用分辨率,新旧MCU都能识别的分辨率
			dwResW = wTmpResW;
			dwResH = wTmpResH;
			byCommonRes = GetNormalRes(wTmpResW, wTmpResH);
			tSimCapSet.SetVideoResolution(byCommonRes);
			tSpyResource.SetSimCapset(tSimCapSet);

			u32 dwNetEvId = htonl(dwEvId);

			cServMsg.SetEventId(MCU_MCU_SPYCHNNL_NOTIF);
			cServMsg.SetMsgBody( (u8 *)&tHeadMsg, sizeof(TMsgHeadMsg) );
			cServMsg.CatMsgBody( (u8 *)&tSpyResource, sizeof(TSpyResource) );		
			cServMsg.CatMsgBody( (u8 *)&dwNetEvId, sizeof(u32) );

			if( dwResW !=0 && dwResH != 0)
			{
				dwResW = htonl(dwResW);
				dwResH = htonl(dwResH);
				cServMsg.CatMsgBody( (u8 *)&dwResW, sizeof(u32) );
				cServMsg.CatMsgBody( (u8 *)&dwResH, sizeof(u32) );
			}

			SendMsgToMt( (u8)tSrcSpyInfo.m_tSpy.GetMcuId(), MCU_MCU_SPYCHNNL_NOTIF, cServMsg );	
			
		}
		else
		{
			//tSrc = GetMcuIdxMtFromMcuIdMt( tSrc );
			OnMMcuSetIn( tSrcSpyMt,0,SWITCH_MODE_BROADCAST );
		}
		tSrcSpyInfo.m_tSpy = tSrcSpyMt;
	}
	else
	{
		tSrcSpyInfo.m_tSpy = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgDst,tSrcSpyInfo.m_tSpy );
	}

	BOOL32 bIsAlreadySpyVideo = FALSE;
	if( MODE_BOTH == m_cLocalSpyMana.GetSpyMode(tSrcSpyInfo.m_tSpy) ||
		MODE_VIDEO == m_cLocalSpyMana.GetSpyMode(tSrcSpyInfo.m_tSpy) )
	{
		bIsAlreadySpyVideo = TRUE;
	}
	BOOL32 bIsAlreadySpyAudio = FALSE;
	if( MODE_BOTH == m_cLocalSpyMana.GetSpyMode(tSrcSpyInfo.m_tSpy) ||
		MODE_AUDIO == m_cLocalSpyMana.GetSpyMode(tSrcSpyInfo.m_tSpy) )
	{
		bIsAlreadySpyAudio = TRUE;
	}

	// 下级mcu记录下相应的信息
	CSendSpy cSendSpy;
	cSendSpy.m_tSpy = tSrcSpyInfo.m_tSpy;
	cSendSpy.m_tSpyAddr = tSrcSpyInfo.m_tSpyAddr;
	cSendSpy.m_bySpyMode = tSrcSpyInfo.m_bySpyMode;
	//cSendSpy.m_dwTotalBW = tSrcSpyInfo.m_dwTotalBW;

//	BOOL32 bIsRepeatedSpy = m_cLocalSpyMana.IsRepeatedSpy(cSendSpy);
	//zhouyiliang 20110222 如果这次的spynotify上次已经存了，且请求的能力和上次一样，直接return
// 	if ( bIsRepeatedSpy ) 
// 	{
// 		CSendSpy cRepeatSendSpy;
// 		if ( m_cLocalSpyMana.GetSpyChannlInfo(tSrcSpyInfo.m_tSpy,cRepeatSendSpy) &&  
// 			tSrcSpyInfo.GetSimCapset() == cRepeatSendSpy.GetSimCapset() )
// 		{
// 			// 2011-10-10 add by pgf:增加LOG，提示两次Spy Notify重复，无须再建立交换
// 			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "Local Mana cSendSpy[mt<%d %d>] repeat, so dont set switch for it!\n",
// 				cSendSpy.m_tSpy.GetMcuIdx(), cSendSpy.m_tSpy.GetMtId());
// 			return;
// 		}
// 	}

	if( !m_cLocalSpyMana.AddSpyChannlInfo( cSendSpy ) )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, " SetSpyChannlInfo False When ProcMcuMcuSpyNotify!\n" );
		return;
	}

	//保存本级回传到上级的能力集
	m_cLocalSpyMana.SaveSpySimCap(tSrcSpyInfo.m_tSpy, tSrcSpyInfo.GetSimCapset());

	u8 byMode = tSrcSpyInfo.m_bySpyMode;
	TMt tDstMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	TMt tSrcMt = tSrcSpyInfo.m_tSpy;

	 u16 wSrcChnnl = 0;
	 if( !tSrcMt.IsLocal() )
	 {
		CRecvSpy cRecvSpy;
		if( m_cSMcuSpyMana.GetRecvSpy( tSrcMt,cRecvSpy ) )
		{
			wSrcChnnl = cRecvSpy.m_tSpyAddr.GetPort();
		}		
	 }
	 if ((byMode == MODE_BOTH && bIsAlreadySpyVideo && bIsAlreadySpyAudio) ||
		 (byMode == MODE_AUDIO && bIsAlreadySpyAudio) || 
		 (byMode == MODE_VIDEO && bIsAlreadySpyVideo))
	 {
		 ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,"[ProcMcuMcuSpyNotify]byMode(%d) bIsAlreadySpyVideo(%d) bIsAlreadySpyAudio(%d),So Return!\n",
					byMode,bIsAlreadySpyVideo,bIsAlreadySpyAudio);
		 return;
	 }
	 if (byMode == MODE_BOTH)
	 {
		 if (bIsAlreadySpyVideo)
		 {
			 ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,"[ProcMcuMcuSpyNotify]bIsAlreadySpyVideo(TRUE) So Remove MODE_VIDEO!\n");
			 byMode = MODE_AUDIO;
		 }
		 if (bIsAlreadySpyAudio)
		 {
			 ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,"[ProcMcuMcuSpyNotify]bIsAlreadySpyAudio(TRUE) So Remove MODE_AUDIO!\n");
			 byMode = MODE_VIDEO;
		 }
	 }

	 u16 wErrorCode = 0;
	 if ( StartSwitchToMcuByMultiSpy( tSrcMt, wSrcChnnl, tDstMt.GetMtId(), tSrcSpyInfo.GetSimCapset(), wErrorCode,
		 byMode, SWITCH_MODE_SELECT, FALSE ) )
	 {
		 //[nizhijun 2010/12/10] 级联多回传RTCP信息上告
		 TConfAttrb tConfAttrb = m_tConf.GetConfAttrb();
		 if (tConfAttrb.IsResendLosePack())
		 {
			 ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[ProcMcuMcuSpyNotify] start multiRtcpInfo orgnize\n!");
			 OnMMcuRtcpInfoNotif(tOrgSrc, tSrcSpyInfo.GetSimCapset(), tHeadMsg);
		 }
	 }
	 else
	 {
		 //[2011/09/09/zhangli]如果StartSwitchToMcuByMultiSpy失败，再试图调一次分辨率，如果失败，给调分辨率失败的消息，上级做移除vmp成员、释放回传等操作
		 if (byMode == MODE_BOTH || byMode == MODE_VIDEO)
		 {
			 if (IsNeedSpyAdpt(tSrcMt, tSrcSpyInfo.GetSimCapset(), MODE_VIDEO)
				 && !AdjustSpyStreamFromAdp(tOrgSrc, tSrcSpyInfo.GetSimCapset().GetVideoResolution(), tHeadMsg))
			 {
				 CServMsg cMsg;
				 
				 TMsgHeadMsg tHeadMsgRsp;
				 tHeadMsgRsp.m_tMsgDst = tHeadMsg.m_tMsgSrc;
				 tHeadMsgRsp.m_tMsgSrc = tHeadMsg.m_tMsgDst;
				 
				 if (tSrcMt.IsLocal())
				 {
					 tSrcMt.SetMcuId(LOCAL_MCUID);
					 //tSrcMt.SetMcuIdx(LOCAL_MCUID);
				 }
				 else
				 {
					tSrcMt = tOrgSrc;
				 }
				 cMsg.SetEventId(MCU_MCU_SWITCHTOMCUFAIL_NOTIF);
				 cMsg.SetSrcMtId(cServMsg.GetSrcMtId());
				 cMsg.SetErrorCode(wErrorCode);
				 cMsg.SetMsgBody((u8*)&tHeadMsgRsp, sizeof(TMsgHeadMsg));
				 cMsg.CatMsgBody((u8 *)&tSrcMt, sizeof(TMt));
				 SendReplyBack(cMsg, MCU_MCU_SWITCHTOMCUFAIL_NOTIF);

				 m_cLocalSpyMana.FreeSpyChannlInfo(cSendSpy.m_tSpy,byMode);
				
				 return;
				 
				 // 2011-10-10 add by pgf:如果建立交换失败，那么把该终端的回传信息清空
// 				 ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuSpyNotify]set switch failed:mt<%d %d> mode:%d, so remove it from LocalMana\n",
// 					 tSrcMt.GetMcuIdx(), tSrcMt.GetMtId(), tSrcSpyInfo.m_bySpyMode);
// 				 if(!m_cLocalSpyMana.FreeSpyChannlInfo(tSrcMt, tSrcSpyInfo.m_bySpyMode))
// 				 {
// 					 ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuSpyNotify] FreeSpyChannlInfo failed tSrc<%d %d>\n",
// 						 tSrcMt.GetMcuIdx(), tSrcMt.GetMtId() );
// 				 }
			 }
		 }
	 }

	 if( !bIsAlreadySpyVideo && !m_tCascadeMMCU.IsNull() && m_ptMtTable->GetMtTransE1(m_tCascadeMMCU.GetMtId()) )
	 {
		ProcMMcuBandwidthNotify( m_tCascadeMMCU,GetRealSndSpyBandWidth() );
	 }
	 
	return;
}

/*==============================================================================
函数名    :  ProcMcuMcuSwitchToMcuFailNotif
功能      :  下级建交换到上级mcu失败的通告
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2011-09-08   4.6           zhangli							create
==============================================================================*/
void CMcuVcInst::ProcMcuMcuSwitchToMcuFailNotif( const CMessage *pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	
	
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
	if(tHeadMsg.m_tMsgDst.m_byCasLevel > 0 && !m_tCascadeMMCU.IsNull())
	{
		SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MCU_SWITCHTOMCUFAIL_NOTIF, cServMsg);
		return;
	}
	u16 wErrorCode = cServMsg.GetErrorCode();
	TMt tMt = *(TMt *)(cServMsg.GetMsgBody() +sizeof(TMsgHeadMsg));	//被调好的MT
	
	tMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc,tMt );
	
	ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "[ProcMcuMcuSwitchToMcuFailNotif] Mt(%u,%u) switch to mmcu failed!\n",
		tMt.GetMcuId(), tMt.GetMtId());

	if( VCS_CONF == m_tConf.GetConfSource() && m_cVCSConfStatus.GetTVWallManageMode() == VCS_TVWALLMANAGE_MANUAL_MODE )
	{
		FindConfHduHduChnnlAndStop( &tMt,TW_MEMBERTYPE_NULL,TRUE );	
		FindConfTvWallChnnlAndStop( &tMt,TW_MEMBERTYPE_NULL,TRUE );	
	}
	
	//如果适配资源不足，给以提示
	if(wErrorCode > 0)
	{
		NotifyMcsAlarmInfo(0, wErrorCode);
	}
	
	//以下代码是把建交换失败的这个终端从画面合成器中移除并释放回传带宽
	//目前只需要给出提示，下面代码暂时注掉
	// 	if (tMt.IsNull())
	// 	{
	// 		return;
	// 	}
	// 	
	// 	//移除调失败的VMP成员
	// 	TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
	// 	TVMPMember tVmpMember;
	// 	
	// 	for (u8 byLoop = 0; byLoop < tVmpParam.GetVMPMemberNum(); ++byLoop)
	// 	{
	// 		tVmpMember = *(tVmpParam.GetVmpMember(byLoop));
	// 		if (tVmpMember.IsNull())
	// 		{
	// 			continue;
	// 		}
	// 		if (tVmpMember == tMt)
	// 		{
	// 			ClearOneVmpMember(byLoop, tVmpParam);
	// 			FreeRecvSpy(tMt, MODE_VIDEO);
	// 			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "McuMcuadjResAck, begin to SetVmpChnnl!\n");
	// 		}
	// 	}
}

/*==============================================================================
函数名    :  ProcMcuMcuRejectSpyNotify
功能      :  上级带宽满，给下级通知
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-10-15                 pengjie							create
==============================================================================*/
void CMcuVcInst::ProcMcuMcuRejectSpyNotify( const CMessage *pcMsg )
{
	STATECHECK;	
	//CServMsg cServMsg(pcMsg->content, pcMsg->length);	

	//lukunpeng 2010/06/10 不需要管理本级的上传还有多少带宽可用，完全交给上级控制
	/*
	TMt tMt      = *(TMt *)( cServMsg.GetMsgBody() );
	u32 dwSpyBW  = *(u32 *)( cServMsg.GetMsgBody() + sizeof(TMt) );
	u32 RemainBW = m_cLocalSpyMana.GetConfRemainSpyBW() + dwSpyBW;
	m_cLocalSpyMana.SetConfRemainSpyBW( RemainBW );
	u8 byOldMode = m_cLocalSpyMana.GetOldMode( tMt );
	m_cLocalSpyMana.SetMode( tMt, byOldMode );
	*/

	ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY,  "The Mmcu come to message RejectSpyNotify! \n" );
	
	//return;
}

BOOL32 CMcuVcInst::StopAllLocalSpySwitch()
{
	u16 wSpyNum = MAXNUM_CONF_SPY/*m_cLocalSpyMana.GetSendSpyNum()*/;

	CSendSpy* pcSendSpy = NULL;

	for (u16 wIndex = 0; wIndex < wSpyNum; ++wIndex)
	{
		pcSendSpy = m_cLocalSpyMana.GetSendSpy(wIndex);

		if (NULL == pcSendSpy)
		{
			continue;
		}

		if (pcSendSpy->m_tSpy.IsNull())
		{
			continue;
		}

		/* [2011/11/14/zhangli]如果是下级终端，强制FreeRecvSpy，缺陷：这将导致如果第三级终端同时在参与第二级业务也被free；
		让第三级终端进第一级两个以上的业务，重启第一级mcu，第二级会在这里处理m_cLocalSpyMana的回传信息。如果是第三级终端，
		StopSpyMtSpySwitch里会FreeRecvSpy一次，但是如果此终端参与第一级N个业务，将有N-1个残留*/
		if (!pcSendSpy->m_tSpy.IsLocal())
		{
			FreeRecvSpy(pcSendSpy->m_tSpy, pcSendSpy->m_bySpyMode, TRUE);
		}

		if (!StopSpyMtSpySwitch(pcSendSpy->m_tSpy, pcSendSpy->m_bySpyMode))
		{
			return FALSE;
		}
	}

	m_cLocalSpyMana.Clear();

	return TRUE;
}
/*==============================================================================
函数名    :  StopSpyMtSpySwitch
功能      :  释放一路音、视频回传通道,如果是要求释放的是mcu类型，那么释放到此mcu的所有多回传通道(下级mcu使用)
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010-06-17                 lukunpeng						modify
==============================================================================*/
BOOL32 CMcuVcInst::StopSpyMtSpySwitch( TMt tSrc,u8 bySpyMode, u8 bySpyNoUse/*,BOOL32 bIsNotifyMMcu */)
{
	CSendSpy cSendSpy;
	CRecvSpy cRecvSpy;
	if( !m_cLocalSpyMana.GetSpyChannlInfo( tSrc, cSendSpy ) )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[StopSpyMtSpySwitch] Stop tSrc(%d.%d)  Multispy Failed.Mt is not in spy member.\n",
			tSrc.GetMcuId(),tSrc.GetMtId()
			);
		//释放预占用资源
		if( !tSrc.IsLocal() )
		{
			u32 dwCanReleaseBW = 0;
			s16 swCanRelIndex = -1;
			u32 dwMaxSpyBW = 0;
			s32 nRemainSpyBW = 0;
			u8 byRelMode = 0;
			
			if( !m_cSMcuSpyMana.GetRecvSpy(tSrc,cRecvSpy) )
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[StopSpyMtSpySwitch] Fail to find spy mt tSrc(%d.%d) bySpyMode.%d!\n",
					tSrc.GetMcuId(),tSrc.GetMtId(),bySpyMode 
					);
				return FALSE;
			}
			if( TSpyStatus::WAIT_USE != cRecvSpy.m_byUseState || bySpyMode != cRecvSpy.m_byPreAddMode )
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[StopSpyMtSpySwitch] spy mt tSrc(%d.%d) bySpyMode.%d is not pre add or preadd mode(%d) is error!\n",
					tSrc.GetMcuId(),tSrc.GetMtId(),bySpyMode,cRecvSpy.m_byPreAddMode
					);
				return FALSE;
			}
			m_cSMcuSpyMana.ReleasePreAddRes( tSrc,dwCanReleaseBW, swCanRelIndex );
			u16 wSrcMcuIdx = tSrc.GetMcuId();
			if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx(tSrc.GetMcuId()) )
			{
				u8 byMcuId = GetFstMcuIdFromMcuIdx( tSrc.GetMcuId() );
				wSrcMcuIdx = GetMcuIdxFromMcuId( &byMcuId );
			}		

			m_ptConfOtherMcTable->GetMultiSpyBW(wSrcMcuIdx, dwMaxSpyBW, nRemainSpyBW);
			nRemainSpyBW += (s32)dwCanReleaseBW;				
			m_ptConfOtherMcTable->SetMcuSupMultSpyRemainBW(wSrcMcuIdx, nRemainSpyBW );		

			CMultiSpyMgr *pcMultiSpyMgr = g_cMcuVcApp.GetCMultiSpyMgr();				
			if( NULL == pcMultiSpyMgr )
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[StopSpyMtSpySwitch]GetCMultiSpyMgr False! \n" );
				return FALSE;
			}
			if( -1 != swCanRelIndex )
			{
				pcMultiSpyMgr->ReleaseSpyChnnl( swCanRelIndex );
			}
		}
		
		return FALSE;
	}
	
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  
		"[StopSpyMtSpySwitch] begin to call FreeSpyChannlInfo (Mt(%d.%d), SpyMode.%d) SpyPort:%d, SpyNoUse:%d\n",
		tSrc.GetMcuId(),tSrc.GetMtId(), bySpyMode,cSendSpy.m_tSpyAddr.GetPort(), bySpyNoUse
		);

	if( !tSrc.IsLocal() )
	{
		FreeRecvSpy( tSrc,bySpyMode );
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, 
			"[StopSpyMtSpySwitch] Mt(%d.%d) adjust free mode(%d)\n",
			tSrc.GetMcuId(),
			tSrc.GetMtId(),
			bySpyMode);
	}

	if( ( MODE_VIDEO == bySpyMode || MODE_BOTH == bySpyMode ) 
		&& ( (bySpyNoUse & MODE_VIDEO) != 0 )
		)
	{
		TSimCapSet tSrcCap;
		if (!tSrc.IsLocal())
		{
			tSrcCap = m_ptMtTable->GetSrcSCS(GetFstMcuIdFromMcuIdx(tSrc.GetMcuId()));
		}
		else
		{
			tSrcCap = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
		}

		ConfPrint(LOG_LVL_DETAIL, MID_MCU_SPY,  "[StopSpyMtSpySwitch] SrcCap<Media:%d, Res:%d, BR:%d, Fps:%d>, DstCap<Media:%d, Res:%d, BR:%d, Fps:%d>!\n",
												tSrcCap.GetVideoMediaType(),
												tSrcCap.GetVideoResolution(),
												tSrcCap.GetVideoMaxBitRate(),
												MEDIA_TYPE_H264 == tSrcCap.GetVideoMediaType()?
												tSrcCap.GetUserDefFrameRate():tSrcCap.GetVideoFrameRate(),
												cSendSpy.GetSimCapset().GetVideoMediaType(),
												cSendSpy.GetSimCapset().GetVideoResolution(),
												cSendSpy.GetSimCapset().GetVideoMaxBitRate(),
												MEDIA_TYPE_H264 == cSendSpy.GetSimCapset().GetVideoMediaType()?
												cSendSpy.GetSimCapset().GetUserDefFrameRate(): cSendSpy.GetSimCapset().GetVideoFrameRate());
		
		BOOL32 bAdp = FALSE;
		if( ( (tSrc.IsLocal() && 
				m_ptMtTable->IsLogicChnnlOpen( tSrc.GetMtId(), LOGCHL_VIDEO, FALSE )
				) || 
			  (!tSrc.IsLocal() && 
				m_ptMtTable->IsLogicChnnlOpen( GetFstMcuIdFromMcuIdx(tSrc.GetMcuId()), LOGCHL_VIDEO, FALSE )
				  )	
			 )  &&
			 !cSendSpy.GetSimCapset().IsNull() &&
			 IsNeedSpyAdpt(tSrc, cSendSpy.GetSimCapset(), MODE_VIDEO)
				)
		{			
			BOOL32 bIsStopAdpOk = StopSpyAdapt(tSrc, cSendSpy.GetSimCapset(), MODE_VIDEO);
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[StopSpyMtSpySwitch] StopSpyAdapt bIsStopAdpOk:%d!\n",bIsStopAdpOk);
			bAdp = TRUE;
		}
		
		//zjl 20110510 Mp: StopSwitchToSubMt 接口重载替换 
		//g_cMpManager.StopSwitchToSubMt( m_tCascadeMMCU, MODE_VIDEO, FALSE, cSendSpy.m_tSpyAddr.GetPort() );	
		g_cMpManager.StopSwitchToSubMt(m_byConfIdx, 1, &m_tCascadeMMCU, MODE_VIDEO, cSendSpy.m_tSpyAddr.GetPort());
	
		if( !bAdp )
		{
			if (tSrc.IsLocal())
			{
				SendChgMtVidFormat( tSrc.GetMtId(), MODE_VIDEO, tSrcCap.GetVideoResolution(), TRUE );

				if (IsNeedAdjustSpyFps(tSrc, cSendSpy.GetSimCapset()))
				{
					if (cSendSpy.GetSimCapset().GetVideoMediaType() == MEDIA_TYPE_H264)
					{
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[StopSpyMtSpySwitch] SendChangeMtFps %d.fps OK!\n", tSrcCap.GetUserDefFrameRate());
						SendChangeMtFps(tSrc.GetMtId(), LOGCHL_VIDEO, tSrcCap.GetUserDefFrameRate());
					}		
				}
			}
		}
	}
	
	if( ( MODE_AUDIO == bySpyMode || MODE_BOTH == bySpyMode ) 
		&& ( (bySpyNoUse & MODE_AUDIO) != 0 )
		)
	{
		TSimCapSet tSrcCap;

		/*if (!tSrc.IsLocal())
		{
			tSrcCap = m_ptMtTable->GetSrcSCS(GetFstMcuIdFromMcuIdx(tSrc.GetMcuId()));
		}
		else
		{
			tSrcCap = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
		}
		TMt tLocalSrc = GetLocalMtFromOtherMcuMt(tSrc);



		TLogicalChannel tDstAudLgc,tSrcAudLgc;	


		if( m_ptMtTable->GetMtLogicChnnl(tLocalSrc.GetMtId(), MODE_AUDIO, &tSrcAudLgc, FALSE) &&
				m_ptMtTable->GetMtLogicChnnl(m_tCascadeMMCU.GetMtId(), MODE_AUDIO, &tDstAudLgc, TRUE) )
		{
			if( ( cSendSpy.GetSimCapset().GetAudioMediaType() != tSrcCap.GetAudioMediaType() ||
				tSrcAudLgc.GetAudioTrackNum() != tDstAudLgc.GetAudioTrackNum() ) &&
				m_ptMtTable->IsLogicChnnlOpen( tLocalSrc.GetMtId(), LOGCHL_AUDIO, FALSE ) &&
				!cSendSpy.GetSimCapset().IsNull() )
			{
				StopSpyAdapt(tSrc, cSendSpy.GetSimCapset(), MODE_AUDIO);
			}
		}*/
		
		
		//TSimCapSet tSrcCap = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());

		if( IsNeedSpyAdpt( tSrc,tSrcCap,MODE_AUDIO ) )
		{
			StopSpyAdapt(tSrc, cSendSpy.GetSimCapset(), MODE_AUDIO);
		}
		
		
		//zjl 20110510 Mp: StopSwitchToSubMt 接口重载替换 
		//g_cMpManager.StopSwitchToSubMt( m_tCascadeMMCU, MODE_AUDIO, FALSE, cSendSpy.m_tSpyAddr.GetPort() );
		g_cMpManager.StopSwitchToSubMt(m_byConfIdx, 1, &m_tCascadeMMCU, MODE_AUDIO, cSendSpy.m_tSpyAddr.GetPort());
	}
	
	if( (bySpyNoUse & bySpyMode) == bySpyMode)
	{
		if(!m_cLocalSpyMana.FreeSpyChannlInfo(tSrc, bySpyMode))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[StopSpyMtSpySwitch] FreeSpyChannlInfo False When ProcMcuMcuBanishSpyCmd! tSrc(Mtid): %d\n",
				tSrc.GetMtId() );
		}
	}
	else
	{
		if( MODE_NONE != bySpyNoUse && !m_cLocalSpyMana.FreeSpyChannlInfo(tSrc, bySpyNoUse))
		{
			LogPrint(LOG_LVL_WARNING,MID_MCU_SPY,"[StopSpyMtSpySwitch] FreeSpyChannlInfo False When ProcMcuMcuBanishSpyCmd! tSrc(Mtid): %d bySpyNoUse.%d\n",tSrc.GetMtId(),bySpyNoUse );
		}
	}
	
	// [1/28/2011 xliang] shouldn't change res by force
	/*
	//无条件调整源分辨率，是否调成功接口内判断
    TSimCapSet tSrcCap = m_ptMtTable->GetSrcSCS(tSrc.GetMtId());
	SendChgMtVidFormat( tSrc.GetMtId(), MODE_VIDEO, tSrcCap.GetVideoResolution(), TRUE );

	//恢复帧率
	if (IsNeedAdjustSpyFps(tSrc, cSendSpy.GetSimCapset()))
	{
		if (cSendSpy.GetSimCapset().GetVideoMediaType() == MEDIA_TYPE_H264)
		{
			ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY,  "[StopSpyMtSpySwitch] SendChangeMtFps %d.fps OK!\n", tSrcCap.GetUserDefFrameRate());
			SendChangeMtFps(tSrc.GetMtId(), LOGCHL_VIDEO, tSrcCap.GetUserDefFrameRate());
		}		
	}
	*/

	if( !m_tCascadeMMCU.IsNull() && m_ptMtTable->GetMtTransE1(m_tCascadeMMCU.GetMtId()) )
	{
		ProcMMcuBandwidthNotify( m_tCascadeMMCU,GetRealSndSpyBandWidth() );
	}

	return TRUE;
}

/*==============================================================================
函数名    :  ProcMcuMcuBanishSpyCmd
功能      :  释放一路音、视频回传通道(下级mcu使用)
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-10-15                 pengjie							create
==============================================================================*/
void CMcuVcInst::ProcMcuMcuBanishSpyCmd( const CMessage *pcMsg )
{
	STATECHECK;	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	TMt tSrc =  *(TMt *)( cServMsg.GetMsgBody() );
    u8 bySpyMode = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt));
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)( cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) );
	u8 bySpyNoUse = *(u8*)( cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) + sizeof(TMsgHeadMsg));
	
	tSrc = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgDst,tSrc );
	
	StopSpyMtSpySwitch( tSrc,bySpyMode, bySpyNoUse );

	return;
}

/*==============================================================================
函数名    :  ProMcuMcuMtExtInfoNotif
功能      :  处理级联多回传RTCP信息告知
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010-12-15                 倪志俊							create
==============================================================================*/
void CMcuVcInst::ProcMcuMcuMtExtInfoNotif(const CMessage *pcMsg)
{
	STATECHECK;
	
	TConfAttrb tConfattrb = m_tConf.GetConfAttrb();
	if ( !tConfattrb.IsResendLosePack() )
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuMtExtInfoNotif] Conf doesn't support PRS:%d!\n");
		return;
	}

	TMt tSrc;
	CServMsg cServMsg(pcMsg->content,pcMsg->length);
	
	TMultiRtcpInfo tMultiRtcpInfo = *(TMultiRtcpInfo *)(cServMsg.GetMsgBody());
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)(cServMsg.GetMsgBody()+sizeof(TMultiRtcpInfo));
	tSrc = tMultiRtcpInfo.m_tSrcMt ;
	u16 wSrcMcuIdx = GetMcuIdxFromMcuId( (u8)tSrc.GetMcuId() );	
	tSrc.SetMcuIdx( wSrcMcuIdx );	
	tSrc = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc, tMultiRtcpInfo.m_tSrcMt );

	//如果回传终端不在回传列表中，则说明非法终端，直接return
	s16 swIndex = m_cSMcuSpyMana.FindSpyMt(tSrc);	
	if (-1 != swIndex)
	{
		CRecvSpy tSrcSpyInfo;
		BOOL32 bHasBuildedVidRtcp = FALSE;
		BOOL32 bHasBuildedAudRtcp = FALSE;
		m_cSMcuSpyMana.GetRecvSpy(swIndex, tSrcSpyInfo);
		u32 dwRcvMpIp = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( GetLocalMtFromOtherMcuMt(tSrc).GetMtId()));
		
		switch (tMultiRtcpInfo.m_bySpyMode)
		{
		case MODE_BOTH:
			{
				TTransportAddr tSpyRtcpAddr = tSrcSpyInfo.GetVidSpyBackRtcpAddr();
				if (!(tSpyRtcpAddr == tMultiRtcpInfo.m_tVidRtcpAddr))
				{	
					if (tMultiRtcpInfo.m_tVidRtcpAddr.GetIpAddr()!=0 && tMultiRtcpInfo.m_tVidRtcpAddr.GetPort()!=0)
					{
						bHasBuildedVidRtcp = TRUE;
						g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwRcvMpIp, 0, dwRcvMpIp, tSrcSpyInfo.m_tSpyAddr.GetPort() + 1, 
							tMultiRtcpInfo.m_tVidRtcpAddr.GetIpAddr(),tMultiRtcpInfo.m_tVidRtcpAddr.GetPort(), 
								0, 0, dwRcvMpIp, tSrcSpyInfo.m_tSpyAddr.GetPort());
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "Build MultiSpy Vid RTCP:%s@%d",StrOfIP(dwRcvMpIp),tSrcSpyInfo.m_tSpyAddr.GetPort() + 1);
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "------>%s@%d\n",StrOfIP(tMultiRtcpInfo.m_tVidRtcpAddr.GetIpAddr()),tMultiRtcpInfo.m_tVidRtcpAddr.GetPort());
					}
				}
				
				TTransportAddr tSpyRtcpAudAddr = tSrcSpyInfo.GetAudSpyBackRtcpAddr();
				if (!(tSpyRtcpAudAddr == tMultiRtcpInfo.m_tAudRtcpAddr))
				{	
					if (tMultiRtcpInfo.m_tAudRtcpAddr.GetIpAddr()!=0 && tMultiRtcpInfo.m_tAudRtcpAddr.GetPort()!=0)
					{
						bHasBuildedAudRtcp = TRUE;
						g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwRcvMpIp, 0, dwRcvMpIp, tSrcSpyInfo.m_tSpyAddr.GetPort() + 3, 
							tMultiRtcpInfo.m_tAudRtcpAddr.GetIpAddr(),tMultiRtcpInfo.m_tAudRtcpAddr.GetPort(), 
								0, 0, dwRcvMpIp, tSrcSpyInfo.m_tSpyAddr.GetPort());
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "Build MultiSpy Vid RTCP:%s@%d",StrOfIP(dwRcvMpIp),tSrcSpyInfo.m_tSpyAddr.GetPort() + 3);
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "------>%s@%d\n",StrOfIP(tMultiRtcpInfo.m_tAudRtcpAddr.GetIpAddr()),tMultiRtcpInfo.m_tAudRtcpAddr.GetPort());
					}
				}
			}
			break;
		case MODE_VIDEO:
			{
				TTransportAddr tSpyRtcpVidAddr = tSrcSpyInfo.GetVidSpyBackRtcpAddr();
				if ( tSpyRtcpVidAddr == tMultiRtcpInfo.m_tVidRtcpAddr )
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, "[ProcMcuMcuMtExtInfoNotif] MODE_VIDEO's RTCP hasbeen build to %s@%d!\n",
						StrOfIP(tSrcSpyInfo.GetVidSpyBackRtcpAddr().GetIpAddr()),tSrcSpyInfo.GetVidSpyBackRtcpAddr().GetPort());
					return;
				}
				else
				{
					if (tMultiRtcpInfo.m_tVidRtcpAddr.GetIpAddr()!=0 && tMultiRtcpInfo.m_tVidRtcpAddr.GetPort()!=0)
					{
						bHasBuildedVidRtcp = TRUE;
						g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwRcvMpIp, 0, dwRcvMpIp, tSrcSpyInfo.m_tSpyAddr.GetPort() + 1, 
							tMultiRtcpInfo.m_tVidRtcpAddr.GetIpAddr(),tMultiRtcpInfo.m_tVidRtcpAddr.GetPort(), 
								0, 0, dwRcvMpIp, tSrcSpyInfo.m_tSpyAddr.GetPort());
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "Build MultiSpy Vid RTCP:%s@%d",StrOfIP(dwRcvMpIp),tSrcSpyInfo.m_tSpyAddr.GetPort() + 1);
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "------>%s@%d\n",StrOfIP(tMultiRtcpInfo.m_tVidRtcpAddr.GetIpAddr()),tMultiRtcpInfo.m_tVidRtcpAddr.GetPort());
					}
				}
			}
			break;
		case MODE_AUDIO:
			{
				TTransportAddr tSpyRtcpAudAddr = tSrcSpyInfo.GetAudSpyBackRtcpAddr();
				if (tSpyRtcpAudAddr == tMultiRtcpInfo.m_tAudRtcpAddr )
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_SPY, "[ProcMcuMcuMtExtInfoNotif] MODE_AUDIO's RTCP hasbeen build to %s@%d!\n",
						StrOfIP(tSrcSpyInfo.GetAudSpyBackRtcpAddr().GetIpAddr()),tSrcSpyInfo.GetAudSpyBackRtcpAddr().GetPort());
					return;
				}
				else
				{
					if (tMultiRtcpInfo.m_tAudRtcpAddr.GetIpAddr()!=0 && tMultiRtcpInfo.m_tAudRtcpAddr.GetPort()!=0)
					{
						bHasBuildedAudRtcp = TRUE;
						g_cMpManager.AddMultiToOneSwitch(m_byConfIdx, dwRcvMpIp, 0, dwRcvMpIp, tSrcSpyInfo.m_tSpyAddr.GetPort() + 3, 
							tMultiRtcpInfo.m_tAudRtcpAddr.GetIpAddr(),tMultiRtcpInfo.m_tAudRtcpAddr.GetPort(), 
							0, 0, dwRcvMpIp, tSrcSpyInfo.m_tSpyAddr.GetPort());
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "Build MultiSpy Vid RTCP:%s@%d",StrOfIP(dwRcvMpIp),tSrcSpyInfo.m_tSpyAddr.GetPort() + 3);
						ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "------>%s@%d\n",StrOfIP(tMultiRtcpInfo.m_tAudRtcpAddr.GetIpAddr()),tMultiRtcpInfo.m_tAudRtcpAddr.GetPort());
					}
				}
			}
			break;
		default:
			ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuMtExtInfoNotif] illegal spymode:%d!\n",tMultiRtcpInfo.m_bySpyMode);
			break;
		}

		if ( TRUE == bHasBuildedVidRtcp )
		{
			m_cSMcuSpyMana.SetSpyBackVidRtcpAddr(swIndex, tMultiRtcpInfo.m_bySpyMode, tMultiRtcpInfo.m_tVidRtcpAddr);	
		}

		if ( TRUE == bHasBuildedAudRtcp )
		{
			m_cSMcuSpyMana.SetSpyBackAudRtcpAddr(swIndex, tMultiRtcpInfo.m_bySpyMode, tMultiRtcpInfo.m_tAudRtcpAddr);
		}
	}
	else
	{
		ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[ProcMcuMcuMtExtInfoNotif] TMtSrc(%d-%d) is not in RecvSpySource!\n",
						tSrc.GetMcuId(), tSrc.GetMtId());
		return;
	}

	return ;
}

/*==============================================================================
函数名    :  OnMMcuRtcpInfoNotif
功能      :  下级处理向上级发送notify的逻辑
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010-12-15                 倪志俊							create
==============================================================================*/
void CMcuVcInst::OnMMcuRtcpInfoNotif(TMt tSrc, const TSimCapSet &tDstCap,TMsgHeadMsg tHeadMsg)
{	 
	u8			byMediaMode	 = MODE_NONE;
	TBasOutInfo	tBasOutInfo;
	TTransportAddr tVidRtcpAddr;
	TTransportAddr tAudRtcpAddr;
	TMt tOrgSrc ;
	TMt tUnLocalSrc ;
	TSimCapSet tSrcCap;

	tOrgSrc	    = tSrc;
	tUnLocalSrc = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgDst,tSrc );
	tSrc        = GetLocalMtFromOtherMcuMt(tUnLocalSrc);
    tSrcCap		= m_ptMtTable->GetSrcSCS(tSrc.GetMtId());

	CSendSpy cSendSpy;
	if( m_cLocalSpyMana.GetSpyChannlInfo(tUnLocalSrc, cSendSpy) )
	{
		byMediaMode = m_cLocalSpyMana.GetSpyMode(tUnLocalSrc);
	}

	if (MODE_AUDIO == byMediaMode || MODE_BOTH == byMediaMode)
	{
		TAudioTypeDesc tSrcAudCap;		
		TMt tLocalSrcMt = GetLocalMtFromOtherMcuMt( tSrc );
		TLogicalChannel tSrcAudLgc;
		if (!m_ptMtTable->GetMtLogicChnnl(tLocalSrcMt.GetMtId(), MODE_AUDIO, &tSrcAudLgc, FALSE))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS,  "[IsNeedAdapt] GetMtLogicChnnl Src AUD Mt%d failed!\n", tLocalSrcMt.GetMtId());
		}
		
		tSrcAudCap.SetAudioMediaType( tSrcAudLgc.GetChannelType() );
		tSrcAudCap.SetAudioTrackNum( tSrcAudLgc.GetAudioTrackNum() );
		
		TAudioTypeDesc tDstAudCap;
		TLogicalChannel tDstAudLgc;
		if (!m_ptMtTable->GetMtLogicChnnl(m_tCascadeMMCU.GetMtId(), MODE_AUDIO, &tDstAudLgc, TRUE))
		{
			ConfPrint(LOG_LVL_ERROR, MID_MCU_BAS,  "[IsNeedSpyAdpt] GetMtLogicChnnl Dst Aud Mt%d failed!\n", m_tCascadeMMCU.GetMtId());
		}
		
		tDstAudCap.SetAudioMediaType( tDstAudLgc.GetChannelType() );
		tDstAudCap.SetAudioTrackNum( tDstAudLgc.GetAudioTrackNum() );
		
		// 1.1 不管是不是keda的，音频不匹配都要过适配
		if (  tSrcAudCap.GetAudioMediaType() != MEDIA_TYPE_NULL &&
			tDstAudCap.GetAudioMediaType() != MEDIA_TYPE_NULL &&
			( tSrcAudCap.GetAudioMediaType() != tDstAudCap.GetAudioMediaType() 
			||tSrcAudCap.GetAudioTrackNum() != tDstAudCap.GetAudioTrackNum()
			)
			)
		{
			if (FindBasChn2SpyForMt(tUnLocalSrc, tDstCap, MODE_AUDIO, tBasOutInfo))
			{
				//找到对应的BAS通道RTCP信息
				tAudRtcpAddr.SetNull();
				//GetSpyBasRtcpInfo(tBasOutInfo.m_tBasEqp,tBasOutInfo.m_byChnId,tBasOutInfo.m_byFrontOutNum,tBasOutInfo.m_byOutIdx,tAudRtcpAddr);
				GetRemoteRtcpAddr( tBasOutInfo.m_tBasEqp, tBasOutInfo.m_byFrontOutNum+tBasOutInfo.m_byOutIdx, MODE_AUDIO, tAudRtcpAddr);
			}
			else
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[OnMMcuRtcpInfoNotif] Can't find BAS's chnnl for MODE_AUDIO!\n" );
			}
		}
		// 1.2 音频不过适配
		else
		{
			//不过适配，需要将本级级联通道的音频RTCP信息告知上级
			if (tUnLocalSrc.IsLocal())
			{
				tAudRtcpAddr = tSrcAudLgc.GetSndMediaCtrlChannel();
				//8kh如果源终端是代理终端，需要填入的ip是MCU的IP
#if defined(_8KH_) || defined(_8KE_) || defined(_8KI_)
				TMultiManuNetAccess tMultiManuNetAccess;
				g_cMcuAgent.GetMultiManuNetAccess(tMultiManuNetAccess);
				TGKProxyCfgInfo tGKProxyCfgInfo;
				g_cMcuAgent.GetGkProxyCfgInfo(tGKProxyCfgInfo);
				if( tGKProxyCfgInfo.IsProxyUsed())
				{
					for( u8 byIdx = 0; byIdx < tMultiManuNetAccess.GetIpSecNum(); byIdx++ )
					{
						u32 dwProyIp = tMultiManuNetAccess.GetIpAddr(byIdx);
						if( dwProyIp == tAudRtcpAddr.GetIpAddr() )
						{
							LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_SPY,"8KE/H/I in dwIp:%x is Multi Proxy Ip\n",dwProyIp);
							tAudRtcpAddr.SetIpAddr( g_cMcuAgent.GetMpcIp() );
							break;
						}
					}
				}
#endif
			}
			else
			{
				GetSpyCascadeRtcpInfo(tUnLocalSrc,tVidRtcpAddr,tAudRtcpAddr);
			}
		}
	}
	
	//视频交换
	if (MODE_VIDEO == byMediaMode || MODE_BOTH == byMediaMode)
	{
		// 起适配
		if(IsNeedSpyAdpt(tSrc, tDstCap, MODE_VIDEO))
		{
			if (FindBasChn2SpyForMt(tUnLocalSrc, tDstCap, MODE_VIDEO, tBasOutInfo))
			{
				//找到对应的BAS通道RTCP信息
				tVidRtcpAddr.SetNull();
				//GetSpyBasRtcpInfo(tBasOutInfo.m_tBasEqp,tBasOutInfo.m_byChnId,tBasOutInfo.m_byFrontOutNum,tBasOutInfo.m_byOutIdx,tVidRtcpAddr);
				GetRemoteRtcpAddr(tBasOutInfo.m_tBasEqp, tBasOutInfo.m_byFrontOutNum+tBasOutInfo.m_byOutIdx, MODE_VIDEO, tVidRtcpAddr);
			}
			else
			{
				ConfPrint(LOG_LVL_ERROR, MID_MCU_SPY, "[OnMMcuRtcpInfoNotif] Can't find BAS's chnnl for MODE_VIDEO!\n" );
			}
		}
		else
		{
			//不过适配，需要将本级级联通道的音频RTCP信息告知上级
			if (tUnLocalSrc.IsLocal())
			{
				TLogicalChannel tLogiclChannel;
				m_ptMtTable->GetMtLogicChnnl(tUnLocalSrc.GetMtId(), LOGCHL_VIDEO, &tLogiclChannel, FALSE);
				tVidRtcpAddr = tLogiclChannel.GetSndMediaCtrlChannel();
				//8kh如果源终端是代理终端，需要填入的ip是MCU的IP
#if defined(_8KH_) || defined(_8KE_) || defined(_8KI_)
				TMultiManuNetAccess tMultiManuNetAccess;
				g_cMcuAgent.GetMultiManuNetAccess(tMultiManuNetAccess);
				TGKProxyCfgInfo tGKProxyCfgInfo;
				g_cMcuAgent.GetGkProxyCfgInfo(tGKProxyCfgInfo);
				if( tGKProxyCfgInfo.IsProxyUsed())
				{
					for( u8 byIdx = 0; byIdx < tMultiManuNetAccess.GetIpSecNum(); byIdx++ )
					{
						u32 dwProyIp = tMultiManuNetAccess.GetIpAddr(byIdx);
						if( dwProyIp == tVidRtcpAddr.GetIpAddr() )
						{
							LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_SPY,"8KE/H/I in dwIp:%x is Multi Proxy Ip\n",dwProyIp);
							tVidRtcpAddr.SetIpAddr( g_cMcuAgent.GetMpcIp() );
							break;
						}
					}
				}
#endif
			}
			else
			{
				GetSpyCascadeRtcpInfo(tUnLocalSrc,tVidRtcpAddr,tAudRtcpAddr);
			}
		}
	}
	
	//向上级发送RTCP信息NOTIFY
	SendMMcuMtExtInfoNotif(tOrgSrc,tHeadMsg,byMediaMode,tVidRtcpAddr,tAudRtcpAddr);

	return;
}

/*==============================================================================
函数名    :  GetSpyBasRtcpInfo
功能      :  获得多回传关于BAS的RTCP信息
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010-12-15                 倪志俊							create
==============================================================================*/
void CMcuVcInst::GetSpyBasRtcpInfo(TEqp &tBas, u8 &byChnId, u8 &byFrontOutNum, u8 &byOutIdx, TTransportAddr &tRtcpAddr)
{
	u32		dwRtcpSwitchIp  = 0;
	u16		wRtcpSwitchPort = 0;
	u32		dwSrcIp         = 0;

	ConfPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "[GetSpyBasRtcpInfo] byChnId = %d\n",byChnId);

#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	#ifdef _8KE_
	wRtcpSwitchPort = BAS_8KE_LOCALSND_PORT + (tBas.GetEqpId()-BASID_MIN)*BAS_8KE_PORTSPAN 
		+ PORTSPAN * (BAS_8KE_PORTSPAN * byChnId + byOutIdx) + 1; 
	#endif
	
	#ifdef _8KH_
	wRtcpSwitchPort = BAS_8KE_LOCALSND_PORT + (tBas.GetEqpId()-BASID_MIN)*BAS_8KH_PORTSPAN 
								+ PORTSPAN * (BAS_8KH_PORTSPAN * byChnId + byOutIdx) + 1; 
	#endif

	#ifdef _8KI_
	wRtcpSwitchPort = BAS_8KE_LOCALSND_PORT +PORTSPAN * (byFrontOutNum+byOutIdx) + 1; 
	#endif

	dwRtcpSwitchIp = g_cMcuVcApp.GetEqpIpAddr(tBas.GetEqpId());
	tRtcpAddr.SetIpAddr(dwRtcpSwitchIp);
	tRtcpAddr.SetPort(wRtcpSwitchPort);
#else
	g_cMpManager.GetSwitchInfo(tBas, dwRtcpSwitchIp, wRtcpSwitchPort, dwSrcIp);
	tRtcpAddr.SetIpAddr(dwSrcIp);
	tRtcpAddr.SetPort(wRtcpSwitchPort + PORTSPAN * (byFrontOutNum+byOutIdx) + 1);
#endif				
}

/*==============================================================================
函数名    :  GetSpyCascadeRtcpInfo
功能      :  获得多回传关于本级级联通道的RTCP信息
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010-12-10                 倪志俊							create
==============================================================================*/
void CMcuVcInst::GetSpyCascadeRtcpInfo(TMt tSrc, TTransportAddr &tVidRtcpAddr, TTransportAddr &tAudRtcpAddr)
{
	if ( IsLocalAndSMcuSupMultSpy(tSrc.GetMcuId()) )
	{
		CRecvSpy tRrvSpyInfo;
		m_cSMcuSpyMana.GetRecvSpy( tSrc, tRrvSpyInfo );
		u32 dwMpIpAddr = g_cMcuVcApp.GetMpIpAddr( m_ptMtTable->GetMpId( GetLocalMtFromOtherMcuMt(tSrc).GetMtId()) );
		
		//视频回传地址
		tVidRtcpAddr.SetIpAddr( dwMpIpAddr );
		tVidRtcpAddr.SetPort( tRrvSpyInfo.m_tSpyAddr.GetPort() + 1 );
		
		//音频回传地址
		tAudRtcpAddr.SetIpAddr( dwMpIpAddr );
		tAudRtcpAddr.SetPort( tRrvSpyInfo.m_tSpyAddr.GetPort() + 3 );

		ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_SPY,"[GetSpyCascadeRtcpInfo]tSrc:%d-%d is support multispy!\n",tSrc.GetMcuId(), tSrc.GetMtId());
	}
	else
	{
		TMt tLocalSrc = GetLocalMtFromOtherMcuMt(tSrc); 
		TLogicalChannel tLogiclChannel;
		m_ptMtTable->GetMtLogicChnnl(tLocalSrc.GetMtId(), LOGCHL_AUDIO, &tLogiclChannel, FALSE);
		tAudRtcpAddr = tLogiclChannel.GetSndMediaCtrlChannel();

		m_ptMtTable->GetMtLogicChnnl(tLocalSrc.GetMtId(), LOGCHL_VIDEO, &tLogiclChannel, FALSE);
		tVidRtcpAddr = tLogiclChannel.GetSndMediaCtrlChannel();

		ConfPrint(LOG_LVL_KEYSTATUS,MID_MCU_SPY,"[GetSpyCascadeRtcpInfo]tSrc:%d-%d is not support multispy!\n",tSrc.GetMcuId(), tSrc.GetMtId());
	}
}

/*==============================================================================
函数名    :  SendMMcuMtExtInfoNotif
功能      :  向上级发送RTCP信息NOTIFY
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010-12-15                 倪志俊							create
==============================================================================*/
void CMcuVcInst::SendMMcuMtExtInfoNotif(TMt tSpySrc, TMsgHeadMsg tHeadMsg, u8 bySpyMode, TTransportAddr &tVidRtcpAddr, TTransportAddr &tAudRtcpAddr)
{
	TMsgHeadMsg tHeadMsgMtextNotif;	 
	tHeadMsgMtextNotif.m_tMsgSrc = tHeadMsg.m_tMsgDst;
	tHeadMsgMtextNotif.m_tMsgDst = tHeadMsg.m_tMsgSrc;
	
	CServMsg cMsg;
	TMultiRtcpInfo tMultiRtcpInfo;
	tMultiRtcpInfo.m_tSrcMt = tSpySrc;
	tMultiRtcpInfo.m_bySpyMode = bySpyMode;
	tMultiRtcpInfo.m_tVidRtcpAddr = tVidRtcpAddr;
	tMultiRtcpInfo.m_tAudRtcpAddr = tAudRtcpAddr;
	cMsg.SetEventId( MCU_MCU_MTEXTINFO_NOTIF );
	cMsg.SetMsgBody( (u8*)&tHeadMsgMtextNotif,sizeof(TMsgHeadMsg) );
	cMsg.CatMsgBody( (u8*)&tMultiRtcpInfo,sizeof(TMultiRtcpInfo) );
	
	SendMsgToMt( m_tCascadeMMCU.GetMtId(),cMsg.GetEventId(),cMsg );
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[SendMMcuMtExtInfoNotif]SpyMode:%d-SpySrc:McuID%d-MtID%d\n 's RTCP info send to MMCU\n",
				  bySpyMode,tSpySrc.GetMcuId(),tSpySrc.GetMtId());
	ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[SendMMcuMtExtInfoNotif]vid :0x%x-%d,aud :0x%x-%d\n",
				  tVidRtcpAddr.GetIpAddr(), tVidRtcpAddr.GetPort(), tAudRtcpAddr.GetIpAddr(), tAudRtcpAddr.GetPort() 
			 );
}


// 根据终端信息释放一个回传通道
// BOOL32 CMcuVcInst::FreeSpyChannlInfo( const TMt &tMt, u8 bySpyMode/*= MODE_BOTH*/, BOOL32 bForce/*= FALSE*/ )
// {
// 	CRecvSpy tSrcSpyInfo;
// 	u32 dwMaxSpyBW = 0;
// 	s32 nRemainSpyBW = 0;
// 	
// 	//获得此mcu的总带宽及剩余带宽
// 	if (!m_ptConfOtherMcTable->GetMultiSpyBW(tMt.GetMcuId(), dwMaxSpyBW, nRemainSpyBW))
// 	{
// 		return FALSE;
// 	}
// 
// 	//获得此终端的回传信息
// 	if (!m_cSMcuSpyMana.GetRecvSpy(tMt, tSrcSpyInfo))
// 	{
// 		return FALSE;
// 	}
// 
// 	//释放此终端的带宽
// 	//dwRemainSpyBW += tSrcSpyInfo.m_dwTotalBW;
// 	m_ptConfOtherMcTable->SetMcuSupMultSpyRemainBW(tMt.GetMcuId(), nRemainSpyBW);
// 
// 	return TRUE;
// }


/*==============================================================================
函数名    :  ShowMultiSpyMMcuInfo
功能      :  显示本级存储的所有下级的多回传的信息和所有本级多回传的信息
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010-07-12                 陆昆朋							create
==============================================================================*/
void CMcuVcInst::ShowConfMultiSpy( void )
{
	u8 byMtLoop = 1,byMcuLoop = 1,byMtLoop1 = 0;
	u8 byMcuId = 0;
	TConfMcInfo *ptMcInfo = NULL;
	TConfMtInfo *ptConfMtInfo = NULL;
	
	BOOL32 bInConf = FALSE,bInJoinedConf = FALSE;
	char achTemp[255];

	TMtAlias tMtAliasH323id, tMtAliasE164, tMtAliasH320id, tMtAliasH320Alias;
	TMtAlias tDailAlias;

	CSendSpy *pcSendSpy = NULL;
	u16 wSpyNum = MAXNUM_CONF_SPY/*m_cLocalSpyMana.GetSendSpyNum()*/;
	//u8 byMtId = 0;

    
    m_tConf.GetConfId().GetConfIdString(achTemp, sizeof(achTemp));
    StaticLog("cConfId %s\n", achTemp );
	StaticLog( "\n本级多回传信息:\n");
	
	TMtExt tMtExt;

	//StaticLog( "在线\tMCUID\tMTID\t\tIP\t\t  Alias\tSpyPort SpyMode VidBas chl AudBas chl BasMode PrsId PrsChl\n");
	for (byMtLoop = 0; byMtLoop < wSpyNum; byMtLoop++)
	{
		pcSendSpy = m_cLocalSpyMana.GetSendSpy(byMtLoop);

		if (pcSendSpy == NULL)
		{
			break;
		}

		if (pcSendSpy->m_tSpy.IsNull())
		{
			continue;
		}

		//byMtId = pcSendSpy->m_tSpy.GetMtId();

		if( m_tConfAllMtInfo.MtJoinedConf(pcSendSpy->m_tSpy) )
		{
			sprintf( achTemp, "√ %c", 0 );
		}
		else
		{
			sprintf( achTemp, "× %c", 0 ); 
		}

		if( pcSendSpy->m_tSpy.IsLocal() )
		{
			tMtAliasH323id.SetNull();
			tMtAliasE164.SetNull();
			tMtAliasH320id.SetNull();
			tMtAliasH320Alias.SetNull();
			m_ptMtTable->GetMtAlias( pcSendSpy->m_tSpy.GetMtId(), mtAliasTypeH323ID, &tMtAliasH323id );
			m_ptMtTable->GetMtAlias( pcSendSpy->m_tSpy.GetMtId(), mtAliasTypeE164, &tMtAliasE164 );
			m_ptMtTable->GetMtAlias( pcSendSpy->m_tSpy.GetMtId(), mtAliasTypeH320ID, &tMtAliasH320id );
			m_ptMtTable->GetMtAlias( pcSendSpy->m_tSpy.GetMtId(), mtAliasTypeH320Alias, &tMtAliasH320Alias);			
        
			BOOL32 bRet = m_ptMtTable->GetDialAlias(pcSendSpy->m_tSpy.GetMtId(), &tDailAlias);
        
			if ( !bRet )
			{
				StaticLog("McuIdx(%d) McuId(%d)   IP:0x%08x 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s SpyPort:%d SpyMode:%d\n,",
								pcSendSpy->m_tSpy.GetMcuId(),pcSendSpy->m_tSpy.GetMtId(),m_ptMtTable->GetIPAddr( pcSendSpy->m_tSpy.GetMtId() ),
								tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
								tMtAliasH320Alias.m_achAlias,pcSendSpy->m_tSpyAddr.GetPort(),pcSendSpy->GetSpyMode()
								);
			}
			else
			{
				if ( mtAliasTypeTransportAddress == tDailAlias.m_AliasType )
				{
					StaticLog("McuIdx(%d) McuId(%d)   IP:0x%08x 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s DialAlias.%s@%d SpyPort:%d SpyMode:%d\n",
								pcSendSpy->m_tSpy.GetMcuId(),pcSendSpy->m_tSpy.GetMtId(),m_ptMtTable->GetIPAddr( pcSendSpy->m_tSpy.GetMtId() ),
								tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
								tMtAliasH320Alias.m_achAlias,
								StrOfIP(tDailAlias.m_tTransportAddr.GetIpAddr()), tDailAlias.m_tTransportAddr.GetPort(),
								pcSendSpy->m_tSpyAddr.GetPort(), pcSendSpy->GetSpyMode()
								);
				}
				else
				{
					StaticLog("McuIdx(%d) McuId(%d)   IP:0x%08x 323Alias: %s, E164: %s, 320ID: %s, 320Alias: %s DialAlias.%s SpyPort:%d SpyMode:%d\n,",
								pcSendSpy->m_tSpy.GetMcuId(),pcSendSpy->m_tSpy.GetMtId(),m_ptMtTable->GetIPAddr( pcSendSpy->m_tSpy.GetMtId() ),
								tMtAliasH323id.m_achAlias, tMtAliasE164.m_achAlias, tMtAliasH320id.m_achAlias,
								tMtAliasH320Alias.m_achAlias,
								tDailAlias.m_achAlias,pcSendSpy->m_tSpyAddr.GetPort(), pcSendSpy->GetSpyMode()
								);
				}
			}
		}
		else
		{
			ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(pcSendSpy->m_tSpy.GetMcuId());
			if(ptMcInfo == NULL)
			{
				continue;
			}
			ptConfMtInfo = m_tConfAllMtInfo.GetMtInfoPtr(pcSendSpy->m_tSpy.GetMcuId()); 
			if(ptConfMtInfo == NULL)
			{
				continue;
			}
			for( byMtLoop1 = 1; byMtLoop1 <= MAXNUM_CONF_MT; byMtLoop1++ )
			{
				tMtExt = ptMcInfo->m_atMtExt[byMtLoop1-1];
				if( tMtExt.GetMtId() == pcSendSpy->m_tSpy.GetMtId() )
				{
					StaticLog("McuIdx(%d) McuId(%d) IP:0x%08x(%s) Alias.%s SpyPort:%d SpyMode:%d\n",
								pcSendSpy->m_tSpy.GetMcuId(),pcSendSpy->m_tSpy.GetMtId(),tMtExt.GetIPAddr(),
								StrOfIP(tMtExt.GetIPAddr()),tMtExt.GetAlias(),pcSendSpy->m_tSpyAddr.GetPort(), pcSendSpy->GetSpyMode()
								);
					break;
				}
			}
		}	
		StaticLog( "\t\t\t SpyCap-->VideoCap<Media.%d, Res.%d, BR.%d, Fps.%d>-->AudCap<Media.%d>",
								pcSendSpy->GetSimCapset().GetVideoMediaType(),
								pcSendSpy->GetSimCapset().GetVideoResolution(),
								pcSendSpy->GetSimCapset().GetVideoMaxBitRate(),
								MEDIA_TYPE_H264 == pcSendSpy->GetSimCapset().GetVideoMediaType()?
								pcSendSpy->GetSimCapset().GetUserDefFrameRate():pcSendSpy->GetSimCapset().GetVideoFrameRate(),
								pcSendSpy->GetSimCapset().GetAudioMediaType());
	}

	StaticLog( "\n所有下级多回传信息:\n");

	//获取此mcu支持的总带宽及剩余带宽
	u32 dwMaxSpyBW = 0;
	s32 nRemainSpyBW = 0;
	
	u16 wMcuIdx = INVALID_MCUIDX;

	// liuxu, 改byMcuLoop为wLoop. 这里过去问题比较大
	for( u16 wLoop = 0; wLoop < m_tConfAllMtInfo.GetMaxMcuNum(); wLoop++ )
	{
		wMcuIdx = m_tConfAllMtInfo.GetMtInfo(wLoop).GetMcuIdx();
		if (!IsValidSubMcuId(wMcuIdx))
		{
			continue;
		}
		
		if( !m_tConfAllMcuInfo.IsSMcuByMcuIdx( wMcuIdx) )
		{
			continue;
		}
		
		ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(wMcuIdx);
		if(ptMcInfo == NULL)
		{
			continue;
		}
		ptConfMtInfo = m_tConfAllMtInfo.GetMtInfoPtr(wMcuIdx); 
		if(ptConfMtInfo == NULL)
		{
			continue;
		}

		m_ptConfOtherMcTable->GetMultiSpyBW(wMcuIdx, dwMaxSpyBW, nRemainSpyBW);
		
		StaticLog("McuIdx(%d) McuId(%d): MaxBW:%ld ReMainBW:%ld\n", wMcuIdx,GetFstMcuIdFromMcuIdx( wMcuIdx ),  dwMaxSpyBW, nRemainSpyBW);
	}


	CRecvSpy cRecvSpy;
	u8 abyMcuId[MAX_CASCADEDEPTH-1];
	
	for( byMcuLoop = 0;byMcuLoop < MAXNUM_CONF_SPY;++byMcuLoop)
	{
		if( m_cSMcuSpyMana.GetRecvSpy( byMcuLoop,cRecvSpy ) )
		{
			if( !cRecvSpy.m_tSpy.IsNull() )
			{
				m_tConfAllMcuInfo.GetMcuIdByIdx( cRecvSpy.m_tSpy.GetMcuId(),&abyMcuId[0] );
				if( m_tConfAllMcuInfo.IsSMcuByMcuIdx(cRecvSpy.m_tSpy.GetMcuId()) )
				{
					StaticLog( "McuIdx(%d) McuId(%d) MtId(%d)   VSpyDstNum:%d ASpyDstNum:%d SpyPort:%d VBW:%d ABW:%d SpyMode:%d State:%d\n",
							cRecvSpy.m_tSpy.GetMcuId(),abyMcuId[0],cRecvSpy.m_tSpy.GetMtId(),
							cRecvSpy.m_byVSpyDstNum,cRecvSpy.m_byASpyDstNum,cRecvSpy.m_tSpyAddr.GetPort(),
							cRecvSpy.m_dwVidBW, cRecvSpy.m_dwAudBW,cRecvSpy.m_bySpyMode,cRecvSpy.m_byUseState
							);
				}
				else
				{
					
					StaticLog( "McuIdx(%d) McuId(%d-%d) MtId(%d) VSpyDstNum:%d ASpyDstNum:%d SpyPort:%d VBW:%d ABW:%d SpyMode:%d State:%d\n",
							cRecvSpy.m_tSpy.GetMcuId(),abyMcuId[0],abyMcuId[1],cRecvSpy.m_tSpy.GetMtId(),
							cRecvSpy.m_byVSpyDstNum,cRecvSpy.m_byASpyDstNum,cRecvSpy.m_tSpyAddr.GetPort(),
							cRecvSpy.m_dwVidBW, cRecvSpy.m_dwAudBW,cRecvSpy.m_bySpyMode,cRecvSpy.m_byUseState
							);
				}
				StaticLog( "\t\t\t Require to tMt<McuId.%d>--> VideoCap<Media.%d, Res.%d, BR.%d, Fps.%d>--> AudCap<Media.%d>!\n",
										abyMcuId[0],
										cRecvSpy.GetSimCapset().GetVideoMediaType(),
										cRecvSpy.GetSimCapset().GetVideoResolution(),
										cRecvSpy.GetSimCapset().GetVideoMaxBitRate(),
										MEDIA_TYPE_H264 == cRecvSpy.GetSimCapset().GetVideoMediaType()?
										cRecvSpy.GetSimCapset().GetUserDefFrameRate():cRecvSpy.GetSimCapset().GetVideoFrameRate(),
										cRecvSpy.GetSimCapset().GetAudioMediaType());
			}
		}
	}
	
}


/*==============================================================================
函数名    :  IsSupportMultiSpy
功能      :  该会议是否支持多回传
算法实现  :  
参数说明  :  
返回值说明:  BOOL32
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-04-12                 周晶晶							create
==============================================================================*/
BOOL32 CMcuVcInst::IsSupportMultiSpy( )
{
	return m_tConf.IsSupportMultiSpy();
}
/*==============================================================================
函数名    :  IsCanSetOutView
功能      :  是否可以把终端放入回传通道
算法实现  :  
参数说明  :  
返回值说明:  BOOL32
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-04-12                 周晶晶							create
==============================================================================*/
BOOL32 CMcuVcInst::IsCanSetOutView( TMt &tSetInMt, u8 byMode/* = MODE_BOTH */)
{
	if( m_tCascadeMMCU.IsNull() )
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[IsCanSetOutView]: m_tCascadeMMCU is Null!\n");
		return FALSE;
	}

	if( !IsSupportMultiSpy() )
	{
		return TRUE;
	}
	
	//u32 dwSpyBW = 0;
	TLogicalChannel     tAudChnnl;
	

	TMt tSrcMtVid,tSrcMtAud;
	tSrcMtVid.SetNull();
	tSrcMtAud.SetNull();

	if( m_ptMtTable->GetMtLogicChnnl( m_tCascadeMMCU.GetMtId(), LOGCHL_VIDEO, &tAudChnnl, TRUE ) )
	{
		m_ptMtTable->GetMtSrc( m_tCascadeMMCU.GetMtId(),&tSrcMtVid,MODE_VIDEO );
	}

	if( m_ptMtTable->GetMtLogicChnnl( m_tCascadeMMCU.GetMtId(), LOGCHL_AUDIO, &tAudChnnl, TRUE ) )
	{
		m_ptMtTable->GetMtSrc( m_tCascadeMMCU.GetMtId(),&tSrcMtAud,MODE_AUDIO );
	}
	
	m_ptMtTable->GetMtLogicChnnl( tSetInMt.GetMtId(), LOGCHL_AUDIO, &tAudChnnl, FALSE );	
	u8 byMediaMode = byMode;

	if( tSrcMtVid == tSetInMt 
		&& ( MODE_BOTH == byMediaMode || MODE_VIDEO == byMediaMode ))
	{
		if( MODE_BOTH == byMediaMode )
		{
			byMediaMode = MODE_AUDIO;
		}
		else if( MODE_VIDEO == byMediaMode )
		{
			byMediaMode = MODE_NONE;
		}
	}
	if( tSrcMtAud == tSetInMt )
	{
		if( MODE_BOTH == byMediaMode )
		{
			byMediaMode = MODE_VIDEO;
		}
		else if( MODE_AUDIO == byMediaMode )
		{
			byMediaMode = MODE_NONE;
		}
	}

	switch( byMediaMode )
	{
	case MODE_VIDEO:
		//dwSpyBW = m_ptMtTable->GetDialBitrate( tSetInMt.GetMtId() );
		break;
	case MODE_AUDIO:
		//dwSpyBW = GetAudioBitrate( tAudChnnl.GetChannelType() );
		break;
	case MODE_BOTH:
		//dwSpyBW = m_ptMtTable->GetDialBitrate( tSetInMt.GetMtId() ) + GetAudioBitrate( tAudChnnl.GetChannelType() );
		break;
	case MODE_NONE:
		{
			ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[IsCanSetOutView] MediaMode is MODE_NONE, so can't setout mt(%d.%d)\n",
				tSetInMt.GetMcuId(),tSetInMt.GetMtId()
				);		
			//*pwErrorCode = ERR_MCU_BACKCHNNL_HAS_SAMEMT;
		}
		//lint -fallthrough
	default:
		return FALSE;
		
	}

	return TRUE;
}

/*=============================================================================
    函 数 名： ProcMcuMcuApplySpeakerRsp
    功    能： 上级MCU对申请发言的响应
    算法实现： 
    全局变量： 
    参    数： const CMessage *pcMsg
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    05/13/2010  4.6			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::ProcMcuMcuApplySpeakerRsp(const CMessage *pcMsg)
{
	STATECHECK

    CServMsg cServMsg(pcMsg->content, pcMsg->length);

    TMt tMt = *(TMt*)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );

    if (tMt.IsMcuIdLocal())
    {
		tMt = m_ptMtTable->GetMt( tMt.GetMtId() );
        if (MCU_MCU_APPLYSPEAKER_ACK == pcMsg->event)
        {
            //申请中 状态 终端自己保持
        }
        else
        {
            //nack给予实时通知
            NotifyMtSpeakStatus(tMt, emDenid);
        }
    }
    else
    {
        SendMsgToMt( (u8)tMt.GetMcuId(),cServMsg.GetEventId(),cServMsg );
    }
}

/*=============================================================================
函 数 名： ProcMcuMcuCancelMeSpeakerRsp
功    能： 
算法实现： 
全局变量： 
参    数： const CMessage *pcMsg
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
05/13/2010  4.6			张宝卿                  创建
=============================================================================*/
void CMcuVcInst::ProcMcuMcuCancelMeSpeakerRsp(const CMessage *pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TMt tMt = *(TMt*)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );
    
    if ( tMt.IsMcuIdLocal() ) 
    {
		tMt = m_ptMtTable->GetMt( tMt.GetMtId() );
		if( MCU_MCU_CANCELMESPEAKER_ACK == cServMsg.GetEventId() )
		{
			NotifyMtSpeakStatus(tMt, emCanceled);
		}
		else
		{

		}
    }
	else
	{
		SendMsgToMt( (u8)tMt.GetMcuId(),cServMsg.GetEventId(),cServMsg );
	}
    
    return;
}

/*=============================================================================
函 数 名： ProcMcuMcuMcuMcuSpeakStatusNtf
功    能： MCU对申请和抢答发言的状态通知
算法实现： 
全局变量： 
参    数： const CMessage *pcMsg
返 回 值： void 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
05/28/2010  4.6			周晶晶                  创建
=============================================================================*/
void CMcuVcInst::ProcMcuMcuSpeakStatusNtf( const CMessage *pcMsg )
{
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
	TMt tMt = *(TMt*)( cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) );
	u32 dwStatus = *(u32*)( cServMsg.GetMsgBody() + sizeof( TMt ) + sizeof(TMsgHeadMsg) );
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,  "[ProcMcuMcuSpeakStatusNtf] Event(%s) tMt(%d.%d.%d) level.%d dwStatus.%d\n",
		OspEventDesc( pcMsg->event ),tMt.GetMcuId(),
		tMt.GetMcuId(),
		tMt.GetMtId(),
		tHeadMsg.m_tMsgDst.m_abyMtIdentify[tHeadMsg.m_tMsgDst.m_byCasLevel],
		tHeadMsg.m_tMsgDst.m_byCasLevel,
		dwStatus
		);

	tMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgDst,tMt );

	NotifyMtSpeakStatus( tMt,(emMtSpeakerStatus)dwStatus );
}

/*=============================================================================
函 数 名： RemoveMcu
功    能： 删除某个mcu(直属或非直属)
算法实现： 
全局变量： 
参    数： const TMt& tMt mcu所代表的终端
返 回 值： 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
08/26/2010  4.6			周晶晶                  创建
=============================================================================*/
void CMcuVcInst::RemoveMcu( const TMt &tMt )
{
	u16 wMcuIdx = INVALID_MCUIDX;
	if( !IsMcu( tMt, &wMcuIdx ) )
	{
		ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "Removemcu(%d,%d) failed, it's not a mcu\n", tMt.GetMcuId(), tMt.GetMtId());
		return;
	}
	
	ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU, "Begin to Remove mcu(%d,%d), mcuidx.%d\n", tMt.GetMcuId(), tMt.GetMtId(), wMcuIdx);

	BOOL32 bIsNotify = FALSE;
	if( INVALID_MCUIDX != wMcuIdx )
	{
		// 上级MCU上传通道成员可能是要挂断mcu的下级终端
		if (!m_tCascadeMMCU.IsNull() && m_tCascadeMMCU.GetMtId() != tMt.GetMtId())
		{
			TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tCascadeMMCU.GetMtId()));
			if( ptConfMcInfo != NULL && 
				( !ptConfMcInfo->m_tSpyMt.IsNull() ) )
			{
				TMt tMMcuSpyMt = ptConfMcInfo->m_tSpyMt;
				tMMcuSpyMt = GetLocalMtFromOtherMcuMt(tMMcuSpyMt);
				if( tMMcuSpyMt.GetMtId() == tMt.GetMtId()&&
					tMMcuSpyMt.GetMcuId() == tMt.GetMcuId())
				{
					StopSpyMtCascaseSwitch();				
				}
			}
		}

		BOOL32 bSendChairman = FALSE;
		u16 wbyMcuIdx[ MAXNUM_SUB_MCU ];
		memset( &wbyMcuIdx[0],INVALID_MCUIDX,sizeof(wbyMcuIdx) );
		u8 abyMcuId[MAX_CASCADEDEPTH-1];
		memset( &abyMcuId[0],0,sizeof(abyMcuId) );		
		if( m_tConfAllMcuInfo.IsSMcuByMcuIdx(wMcuIdx) )
		{
			abyMcuId[0] = tMt.GetMtId();
			m_tConfAllMcuInfo.GetMcuIdxByMcuId( &abyMcuId[0],1,&wbyMcuIdx[0] );
			TConfMtInfo *pcInfo = NULL;
			for( u8 byIdx = 0;byIdx < MAXNUM_SUB_MCU;++byIdx)
			{
				if( INVALID_MCUIDX == wbyMcuIdx[byIdx] || 
					wbyMcuIdx[byIdx] == wMcuIdx )
				{
					continue;
				}
				pcInfo = m_tConfAllMtInfo.GetMtInfoPtr(wbyMcuIdx[byIdx]/*tMt.GetMtId()*/);
				if(pcInfo != NULL)
				{
					pcInfo->SetNull();
				}
				FreeAllRecvSpyByMcuIdx( wbyMcuIdx[byIdx] );
				m_tConfAllMcuInfo.RemoveMcu( wbyMcuIdx[byIdx] );
				m_ptConfOtherMcTable->RemoveMcInfo( wbyMcuIdx[byIdx] );
				m_tConfAllMtInfo.RemoveMcuInfo( wbyMcuIdx[byIdx], tMt.GetConfIdx() );				
				if( m_tApplySpeakQue.RemoveMtByMcuIdx( wbyMcuIdx[byIdx],bSendChairman ) )
				{
					bIsNotify = TRUE;
				}
				
			}
		}
		else
		{
			
		}
		FreeAllRecvSpyByMcuIdx( wMcuIdx );
		m_tConfAllMcuInfo.RemoveMcu( wMcuIdx );
		m_ptConfOtherMcTable->RemoveMcInfo( wMcuIdx );//tMt.GetMtId() );
		m_tConfAllMtInfo.RemoveMcuInfo( wMcuIdx/*tMt.GetMtId()*/, tMt.GetConfIdx() );
		if( m_tApplySpeakQue.RemoveMtByMcuIdx( wMcuIdx,bSendChairman ) )
		{
			bIsNotify = TRUE;
		}
		if( bIsNotify )
		{
			NotifyMcsApplyList( bSendChairman );
		}
	}
}
/*=============================================================================
函 数 名： IsMcu
功    能： 判断一个mt是否是mcu
算法实现： 
全局变量： 
参    数： const TMt& tMt
返 回 值： BOOL32 TRUE 表示tMt是Mcu
				  FALSE 表示tMt不是Mcu
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
08/26/2010  4.6			周晶晶                  创建
=============================================================================*/
BOOL32 CMcuVcInst::IsMcu( const TMt& tMt,u16 *pwMcuIdx/* = NULL */)
{
	if( 0 == tMt.GetMtId() )
	{
		return FALSE;
	}

	if( tMt.IsLocal() )
	{
		if( m_ptMtTable->GetMtType( tMt.GetMtId() ) == MT_TYPE_SMCU ||
					m_ptMtTable->GetMtType( tMt.GetMtId() ) == MT_TYPE_MMCU )	
		{
			if( NULL != pwMcuIdx )
			{
				*pwMcuIdx = GetMcuIdxFromMcuId( tMt.GetMtId() );
			}
			return TRUE;
		}
		
		return FALSE;
	}
	
	TConfMcInfo *ptMcMtInfo = m_ptConfOtherMcTable->GetMcInfo( tMt.GetMcuId() );
	if( NULL == ptMcMtInfo )
	{
		return FALSE;
	}

	for( u8 byLoop = 0;byLoop < MAXNUM_CONF_MT ; ++byLoop )
	{
		if( ptMcMtInfo->m_atMtExt[byLoop].GetMtId() == tMt.GetMtId() )
		{
			if( MT_TYPE_SMCU == ptMcMtInfo->m_atMtExt[byLoop].GetMtType() )
			{
				u8 abyMcuMcuId[MAX_CASCADEDEPTH - 1];				
				memset( abyMcuMcuId, 0 , sizeof(abyMcuMcuId) );
				m_tConfAllMcuInfo.GetMcuIdByIdx( tMt.GetMcuId(), abyMcuMcuId );					
				if( 0 == abyMcuMcuId[MAX_CASCADEDEPTH - 2] )
				{
					abyMcuMcuId[MAX_CASCADEDEPTH - 2] = tMt.GetMtId();
					if( NULL != pwMcuIdx )
					{
						m_tConfAllMcuInfo.GetIdxByMcuId( &abyMcuMcuId[0],2,pwMcuIdx );
					}					
				}
				else
				{
					if( NULL != pwMcuIdx )
					{
						*pwMcuIdx = INVALID_MCUIDX;
					}					
					return FALSE;
				}	

				return TRUE;
			}
			return FALSE;
		}
	}


	return FALSE;
}
/*=============================================================================
函 数 名： IsMtInMcu
功    能： 判断一个mt是否是mcu的终端
算法实现： 
全局变量： 
参    数： const TMt& tMcu, 
		   const TMt& tMt
		   bDirectlyUnder : 是否要求是直属关系(直接下属的终端)
返 回 值： BOOL32 TRUE 表示tMt是从属于 tMcu的
				  FALSE 表示tMt不从属于 tMcu
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
08/26/2010  4.6			周翼亮                  创建
08/26/2010  4.6			周晶晶                  修改
08/31/2011  4.6			刘旭                    添加直属关系判定
=============================================================================*/
BOOL32 CMcuVcInst::IsMtInMcu(const TMt& tMcu, const TMt& tMt, const BOOL32 bDirectlyUnder/* = FALSE*/)
{
	BOOL32 bRet = FALSE;

	if( !IsMcu(tMcu) || tMt.IsNull() )
	{
		return FALSE;
	}

	// [11/1/2011 liuxu] 需要判断confidx
	if ( tMcu.GetConfIdx() != m_byConfIdx
		|| tMt.GetConfIdx() != m_byConfIdx )
	{
		return FALSE;
	}

	u8 abyMcuMcuId[MAX_CASCADEDEPTH - 1];
	memset( abyMcuMcuId, 0 , sizeof(abyMcuMcuId) );
	u16 wMcuMcuIndx = INVALID_MCUIDX;
	if( tMcu.IsLocal() )
	{
		if( tMcu.GetMtId() == 0 )
		{
			return FALSE;
		}
		abyMcuMcuId[0] = tMcu.GetMtId();
		//wMcuMcuIndx = GetMcuIdxFromMcuId( tMcu.GetMtId() );
	}
	else
	{
		wMcuMcuIndx = tMcu.GetMcuIdx();
		if( !m_tConfAllMcuInfo.GetMcuIdByIdx( wMcuMcuIndx, abyMcuMcuId ) )
		{
			return FALSE;
		}
		if( 0 == abyMcuMcuId[MAX_CASCADEDEPTH - 2] )
		{
			abyMcuMcuId[MAX_CASCADEDEPTH - 2] = tMcu.GetMtId();
		}
	}
	
	u16 wMtMcuIndx = tMt.GetMcuIdx();
	if( wMcuMcuIndx == wMtMcuIndx )
	{
		//相同mcu下的mt，没有从属关系
		return FALSE;
	}
	
	u8 abyMtMcuId[MAX_CASCADEDEPTH - 1];	
	memset( abyMtMcuId, 0 , sizeof(abyMtMcuId) );	
	if( !m_tConfAllMcuInfo.GetMcuIdByIdx( wMtMcuIndx, abyMtMcuId ) )
	{
		return FALSE;
	}

	for( u8 byIdx = 0;byIdx < MAX_CASCADEDEPTH - 1;++byIdx )
	{
		// [8/31/2011 liuxu] 要求判定直属关系时, 必须一一进行比较,直到所有级别表示
		// 都相同, 才认为是直属关系; 非直属关系时, 只要mcu的表示对应终端相同位置的表示
		// 相同就可以了
		if( !bDirectlyUnder && 0 == abyMcuMcuId[byIdx] )
		{
			break;
		}
		if( abyMcuMcuId[byIdx] != abyMtMcuId[byIdx] )
		{
			return FALSE;
		}
	}

	return TRUE;

}

/*=============================================================================
函 数 名： IsSmcuViewMtInMcu
功    能： 判断终端tmt是否是tmcu的上传通道成员
算法实现： 
全局变量： 
参    数： [in] tMt, 终端
		   [in] tMcu, mcu
返 回 值： true, 该终端是该mcu的上传通道成员; false, 不是
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
10/10/2011  4.6			liuxu                  创建
=============================================================================*/	
BOOL32 CMcuVcInst::IsSmcuViewMtInMcu( const TMt& tMt, const TMcu& tMcu)
{
	// 校验tMt是否合法
	if ( tMt.IsNull() || tMt.IsLocal() )
	{
		return FALSE;
	}

	// 校验tMcu是否合法
	if ( tMcu.IsNull() || !IsMcu(tMcu) )
	{
		return FALSE;
	}

	// 校验是否存在mcu直属关系
	if (!IsMtInMcu(tMcu, tMt))
	{
		return FALSE;
	}

	TMt tSMcuViewMt = GetSMcuViewMt(tMcu);
	if(tSMcuViewMt.IsNull())
	{
		return FALSE;
	}

	if ( tMt == tSMcuViewMt )
	{
		return TRUE;
	}

	// 如果仍然是mcu, 则递归继续找
	if ( IsMcu( tSMcuViewMt ) )
	{
		return IsSmcuViewMtInMcu( tMt, tSMcuViewMt);
	}

	return FALSE;
}

/*=============================================================================
	函 数 名： GetDirectMcuFromMt
	功    能： 获得tmt直连mcu
	算法实现： 
	全局变量： 
	参    数： [in] TMt &tMt
	返 回 值： 直连mcu
	-----------------------------------------------------------------------------
	修改记录：
	日  期		版本		修改人		走读人    修改内容
	12/07/2011  4.6		yanghuaizhi                  创建
=============================================================================*/	
TMt CMcuVcInst::GetDirectMcuFromMt( const TMt &tMt )
{
	TMt tMcu;
	if (tMt.IsLocal())
	{
		return tMt;
	}
	u8 byFstMcuId = GetFstMcuIdFromMcuIdx(tMt.GetMcuIdx());
	tMcu = m_ptMtTable->GetMt(byFstMcuId);
	// 非直属关系
	if (!IsMtInMcu(tMcu, tMt, TRUE))
	{
		u16 wMcuIdx = GetMcuIdxFromMcuId(byFstMcuId);
		u8 abyMcuId[MAX_CASCADEDEPTH-1];
		memset( &abyMcuId[0],0,sizeof(abyMcuId) );
		if( m_tConfAllMcuInfo.GetMcuIdByIdx( tMt.GetMcuIdx(),&abyMcuId[0] ) )
		{
			tMcu.SetMcuIdx(wMcuIdx);
			tMcu.SetMtId(abyMcuId[1]);
		}
	}

	return tMcu;
}

/*=============================================================================
	函 数 名： GetSMcuViewMt
	功    能： 获取下级mcu的上传通道成员
	算法实现： 
	全局变量： 
	参    数： [in] tMcu, 要找的mcu
			   [in] bDeepest, true, 找最底层的上传通道成员; false, 找直接上传通道成员
	返 回 值： 找到的上传终端
	-----------------------------------------------------------------------------
	修改记录：
	日  期		版本		修改人		走读人    修改内容
	10/10/2011  4.6			liuxu                  创建
=============================================================================*/	
TMt CMcuVcInst::GetSMcuViewMt( const TMcu& tMcu, const BOOL32 bDeepest /*= FALSE*/ )
{
	TMt tSMcuViewMt;				// 待返回的上传终端
	tSMcuViewMt.SetNull();			// 先重置

	// 本身不是mcu, 返回
	if (!IsMcu(tMcu))
	{
		return tSMcuViewMt;
	}

	// 获取mcu信息
	TConfMcInfo *ptConfOtherMcInfo = GetMcuMcInfo( tMcu );
	if (!ptConfOtherMcInfo)
	{
		return tSMcuViewMt;
	}

	// 获取该mcu上传通道成员
	tSMcuViewMt = ptConfOtherMcInfo->m_tMMcuViewMt;
	if (tSMcuViewMt.IsNull())
	{
		return tSMcuViewMt;
	}
	
	// 如果是mcu, 且要找最底层的上传通道成员, 则递归继续找
	if ( IsMcu( tSMcuViewMt ) && bDeepest )
	{
		return GetSMcuViewMt( tSMcuViewMt, bDeepest);
	}

	// 已经找到了, 返回true
	return tSMcuViewMt;
}


/*=============================================================================
函 数 名： GetVscSMCUMaxBW
功    能： 计算VCS会议下级调度席的最大带宽数
算法实现： 
全局变量： 
参    数： 无
返 回 值： u32 0表示计算最大带宽值失败，其他值表示最大带宽值
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
09/11/2010  4.6			陈代伟                  创建
=============================================================================*/		
u32 CMcuVcInst::GetVscSMCUMaxBW(void)
{
	// 下级调度席最大带宽值为：（N+3+1+4）×会议码率
	u16 wUsingTvwallChlNum = 0;	// 下级调度席的电视墙通道使用数：N路
	u8 bySMCUVmpChlNum = 3;		// 下级调度席的画面合成路数：3路
	u8 bySMCUSelMTChlNum = 1;	// 选看下级终端预留路数：1路
	u8 bySMCUMonitorChlNum = 4;	// 外置调度席预览监控预留路数：4路
	u32 dwSMCUMaxBw = 0;        // 置0，表示计算下级调度席最大带宽值失败
	
	u32 dwConfBW = m_tConf.GetBitRate();

	u8 byTemplateConfIdx = g_cMcuVcApp.GetTemConfIdxByE164(m_tConf.GetConfE164());
		
	TTemplateInfo tTemConf;
	if(byTemplateConfIdx != 0 && g_cMcuVcApp.GetTemplate(byTemplateConfIdx, tTemConf))
	{
		for( u8 byChlIdx = 0; byChlIdx < MAXNUM_CONF_HDUBRD * MAXNUM_HDU_CHANNEL; byChlIdx++)
		{
			THduModChnlInfo tInfo = tTemConf.m_tHduModule.GetOneHduChnlInfo(byChlIdx);
			if(tInfo.GetMemberType() == TW_MEMBERTYPE_VCSAUTOSPEC)
			{
				wUsingTvwallChlNum ++;
			}
		}

		for( u8 byTvModulIdx = 0 ; byTvModulIdx < MAXNUM_PERIEQP_CHNNL; byTvModulIdx++)
		{
			TTvWallModule tTvwall;
			tTemConf.m_tMultiTvWallModule.GetTvModuleByIdx(byTvModulIdx,tTvwall);
			for(u8 byIdx = 0; byIdx < MAXNUM_TVWALL_CHNNL_INSMOUDLE; byIdx++)
			{
				if(tTvwall.m_abyMemberType[byIdx] == TW_MEMBERTYPE_VCSAUTOSPEC)
				{
					wUsingTvwallChlNum ++;
				}
			}
		}
		

		dwSMCUMaxBw = (wUsingTvwallChlNum+bySMCUMonitorChlNum+bySMCUSelMTChlNum+bySMCUVmpChlNum)*m_tConf.GetBitRate();
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[GetVscSMCUMaxBW] TVWall Channel Num == %u, confBitRate == %u, SMCU MaxBw == %u\n",wUsingTvwallChlNum,m_tConf.GetBitRate(),dwSMCUMaxBw);
	}
	else
	{
		ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_SPY, "[GetVscSMCUMaxBW] Failed to caculate Max Bandwidth of SMCU!\n");
	}

	return dwSMCUMaxBw;
}

/*=============================================================================
函 数 名： ProcMcuMcuChangeMtSecVidSendCmd
功    能： 处理控制发言人发双流级联消息
算法实现： 
全局变量： 
参    数： 
返 回 值： 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
05/06/2011  4.6			朱胜泽                  创建
=============================================================================*/	
//[5/4/2011 zhushengze]VCS控制发言人发双流
void CMcuVcInst::ProcMcuMcuChangeMtSecVidSendCmd(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
	STATECHECK;	    

    TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
    TMt tMt = *(TMt*)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg));
    u8 byIsSendDStream = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) + sizeof(TMt));

    //转换为本地标识
//    tMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgSrc, tMt );
    tMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgDst, tMt );

    //VCS会议双流状态翻转
    if (VCS_CONF == m_tConf.GetConfSource())
    {
        if (1 == byIsSendDStream)
        {
            m_cVCSConfStatus.SetConfDualEnable(CONF_DUALSTATUS_ENABLE);
            ConfPrint(MID_MCU_CALL, LOG_LVL_KEYSTATUS, "mmcu control send DStream, ConfDual Enable!\n");
        }
        else
        {
            m_cVCSConfStatus.SetConfDualEnable(CONF_DUALSTATUS_DISABLE);
            ConfPrint(MID_MCU_CALL, LOG_LVL_KEYSTATUS, "mmcu control close DStream, ConfDual Disable!\n");
        }
    }
    
    if (!tMt.IsLocal())
    {//如果非本级终端继续投递给下级MCU

        tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tMt );
        cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
        cServMsg.CatMsgBody( (u8*)&tMt,sizeof(TMt) );
        cServMsg.CatMsgBody( (u8*)&byIsSendDStream,sizeof(u32) );

        cServMsg.SetEventId(u16(MCU_MCU_CHANGEMTSECVIDSEND_CMD));
        SendMsgToMt( u8(tMt.GetMcuId()), MCU_MCU_CHANGEMTSECVIDSEND_CMD, cServMsg );
        return;
    }
    else
    {

        if (!m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId()))
        {
            ConfPrint(MID_MCU_CALL, LOG_LVL_KEYSTATUS, "[ProcMcuMcuChangeMtSecVidSendCmd]MT:%d is not in conf!\n", tMt.GetMtId());
            return;
        }
        cServMsg.SetEventId(MCU_MT_CHANGEMTSECVIDSEND_CMD);
        cServMsg.SetMsgBody((u8*)&byIsSendDStream, sizeof(u8));
        SendMsgToMt(tMt.GetMtId(), cServMsg.GetEventId(), cServMsg);
    }

    return;
}

/*==============================================================================
函数名    :  OnPresetinFailed
功能      :  处理带宽满或收到presetinnack等情况下的presetin失败处理

  算法实现  :  
  参数说明  :  
  返回值说明:  
  -------------------------------------------------------------------------------
  修改记录  :  
  日  期       版本          修改人          走读人          修改记录
  20110421     4.6.2         周晶晶						   create
==============================================================================*/
void CMcuVcInst::OnPresetinFailed( const TMt &tMt )
{
	if( tMt.IsNull() )
	{
		return;
	}
	
	if( VCS_CONF == m_tConf.GetConfSource() && 
		ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()) &&
		tMt ==  m_cVCSConfStatus.GetReqVCMT() )
	{
		ProcVCMTOverTime();
	}
}

/*=============================================================================
函 数 名： ProcMcuMcuTransparentMsgNotify
功    能： 处理界面、终端消息透传
算法实现： 
全局变量： 
参    数： 
返 回 值： 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2/23/2012   4.6			朱胜泽                  创建
=============================================================================*/	
void CMcuVcInst::ProcMcuMcuTransparentMsgNotify(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    STATECHECK;	    
    
    TMsgHeadMsg tHeadMsg = *(TMsgHeadMsg*)cServMsg.GetMsgBody();
    TMt tMt = *(TMt*)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg));
    u8* abyMsgContent = (u8*)(cServMsg.GetMsgBody() + sizeof(TMsgHeadMsg) + sizeof(TMt) );
    u16 dwMsgLen = cServMsg.GetServMsgLen() - sizeof(TMsgHeadMsg) - sizeof(TMt);
    
    tMt = GetMtFromMultiCascadeMtInfo( tHeadMsg.m_tMsgDst, tMt );
    
    if (!tMt.IsLocal())
    {//如果非本级终端继续投递给下级MCU
        
        tHeadMsg.m_tMsgDst = BuildMultiCascadeMtInfo( tMt,tMt );
        cServMsg.SetMsgBody( (u8*)&tHeadMsg,sizeof(TMsgHeadMsg) );
        cServMsg.CatMsgBody( (u8*)&tMt,sizeof(TMt) );
        cServMsg.CatMsgBody( (u8*)abyMsgContent,dwMsgLen );
        
        cServMsg.SetEventId(u16(MCU_MCU_TRANSPARENTMSG_NOTIFY));
        SendMsgToMt( u8(tMt.GetMcuId()), MCU_MCU_TRANSPARENTMSG_NOTIFY, cServMsg );
        return;
    }
    else
    {
        
        if (!m_tConfAllMtInfo.MtJoinedConf(tMt.GetMtId()))
        {
            ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "[ProcMcuMcuTransparentMsgNotify]MT:%d is not in conf!\n", tMt.GetMtId());
            return;
        }
        cServMsg.SetEventId(MCU_MT_TRANSPARENTMSG_NOTIFY);
        cServMsg.SetMsgBody((u8*)abyMsgContent, dwMsgLen);
        SendMsgToMt(tMt.GetMtId(), cServMsg.GetEventId(), cServMsg);
        return;
    }
}

/*=============================================================================
函 数 名： SetRealSndSpyBandWidth
功    能： E1线路级联,下级mcu记录真实上传带宽
算法实现： 
全局变量： 
参    数： 
返 回 值： 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20120518   4.7.1		周晶晶                  创建
=============================================================================*/	
void CMcuVcInst::SetRealSndSpyBandWidth( u32 dwBandWidth )
{
	m_dwRealSndSpyBandWidth = dwBandWidth;
}

/*=============================================================================
函 数 名： SetRealSndSpyBandWidth
功    能： E1线路级联,下级mcu获取真实上传带宽
算法实现： 
全局变量： 
参    数： 
返 回 值： 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
20120518   4.7.1		周晶晶                  创建
=============================================================================*/	
u32 CMcuVcInst::GetRealSndSpyBandWidth( void )
{
	return m_dwRealSndSpyBandWidth;
}
/*====================================================================
函数名      ：UpdateTvwOnSMcuViewMtChg
功能        ：单回传模式时, 上传终端改变时更新监控通道状态
算法实现    ：
引用全局变量：
输入参数说明：[in]tNewSMcuViewMt -- 新上来的上传终端
返回值说明  ：无
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/06/28  4.7         田志勇          创建
====================================================================*/
void CMcuVcInst::UpdateMontiorOnSMcuViewMtChg( const TMt& tNewSMcuViewMt)
{
	TMonitorData   tMonitorData;
	TSwitchDstInfo tSwitchDstInfo;
	TMt tMcVideoSrc;
	TMt tMcAudioSrc;
	TLogicalChannel tLogicalChannel;
	u8 byChannelNum;
	for(u8 byIndex = 1; byIndex <= (MAXNUM_MCU_MC + MAXNUM_MCU_VC); byIndex++ )
	{
		if( !g_cMcuVcApp.IsMcConnected( byIndex ) )
		{
			continue;
		}
		//all mc update
		//update video
		g_cMcuVcApp.GetMcLogicChnnl( byIndex, MODE_VIDEO, &byChannelNum, &tLogicalChannel ); 
		for (u8 byVidDstChnIdx = 0 ; byVidDstChnIdx < byChannelNum;byVidDstChnIdx++)
		{
			tMcVideoSrc.SetNull();
			g_cMcuVcApp.GetMcSrc( byIndex, &tMcVideoSrc, byVidDstChnIdx, MODE_VIDEO );
			
			if ( !tMcVideoSrc.IsNull() &&  !IsLocalAndSMcuSupMultSpy( tNewSMcuViewMt.GetMcuId() ) 
				&&	( tNewSMcuViewMt.GetMcuId() == tMcVideoSrc.GetMcuId() || IsMtInMcu( tMcVideoSrc,tNewSMcuViewMt ) )
				)
			{
				g_cMcuVcApp.SetMcSrc( byIndex, &tNewSMcuViewMt, byVidDstChnIdx, MODE_VIDEO );
			}
			
		
		}
		//update audio
		g_cMcuVcApp.GetMcLogicChnnl( byIndex, MODE_AUDIO, &byChannelNum, &tLogicalChannel );
		for (u8 byAudDstChnIdx = 0 ; byAudDstChnIdx < byChannelNum; byAudDstChnIdx++)
		{
			tMcAudioSrc.SetNull();
			g_cMcuVcApp.GetMcSrc( byIndex, &tMcAudioSrc, byAudDstChnIdx, MODE_AUDIO );
			//update audio
			if ( !tMcAudioSrc.IsNull() &&  !IsLocalAndSMcuSupMultSpy( tNewSMcuViewMt.GetMcuId() ) 
				&&	( tNewSMcuViewMt.GetMcuId() == tMcAudioSrc.GetMcuId() || IsMtInMcu( tMcAudioSrc,tNewSMcuViewMt ) )
				)
			{
				g_cMcuVcApp.SetMcSrc( byIndex, &tNewSMcuViewMt, byAudDstChnIdx, MODE_AUDIO );
			}
		}
		
		//all monitor update
		for( u16 wLoop = 0; wLoop < MAXNUM_MONITOR_NUM; wLoop ++ )
		{
			if( !g_cMcuVcApp.GetMonitorData(byIndex, wLoop, tMonitorData) )
			{
				continue;
			}
			if (tNewSMcuViewMt.GetConfIdx() == tMonitorData.GetMonitorSrc().GetConfIdx() &&
				!(tMonitorData.GetMonitorSrc() == tNewSMcuViewMt) &&
				!tMonitorData.GetMonitorSrc().IsMcuIdLocal() && 
				!IsLocalAndSMcuSupMultSpy(GetFstMcuIdFromMcuIdx(tMonitorData.GetMonitorSrc().GetMcuId())) &&
				GetFstMcuIdFromMcuIdx(tMonitorData.GetMonitorSrc().GetMcuId()) == GetFstMcuIdFromMcuIdx(tNewSMcuViewMt.GetMcuId()))
			{
				g_cMcuVcApp.SetMonitorSrc( byIndex, tMonitorData.GetMode(), tMonitorData.GetDstAddr(), tNewSMcuViewMt );
				tSwitchDstInfo.m_tSrcMt = tNewSMcuViewMt;
				tSwitchDstInfo.m_byMode = tMonitorData.GetMode();
				tSwitchDstInfo.m_bySrcChnnl = 0;
				if (tMonitorData.GetMode() == MODE_AUDIO)
				{
					tSwitchDstInfo.m_tDstAudAddr = tMonitorData.m_DstAddr;
				}
				else if (tMonitorData.GetMode() == MODE_VIDEO)
				{
					tSwitchDstInfo.m_tDstVidAddr = tMonitorData.m_DstAddr;
				}
				else
				{
					ConfPrint(LOG_LVL_WARNING, MID_MCU_MMCU, "[UpdateMontiorOnSMcuViewMtChg]Unknown Mode!\n");
					continue;
				}
				ConfPrint(LOG_LVL_DETAIL, MID_MCU_MMCU,"[NotifyUIStartMonitor]McIdx(%d) MonChnIdx(%d) Mt(%d,%d) mode(%d) DstIpAndPort(%s, %d)\n",
					byIndex,wLoop,tSwitchDstInfo.m_tSrcMt.GetMcuId(),tSwitchDstInfo.m_tSrcMt.GetMtId(),
					tSwitchDstInfo.m_byMode,StrOfIP(tMonitorData.m_DstAddr.GetIpAddr()),tMonitorData.m_DstAddr.GetPort());
				NotifyUIStartMonitor(tSwitchDstInfo, byIndex);
			}
		}
	}
}
/*====================================================================
函数名      OnNtfDsMtStatusToMMcu
功能        ：将本级真实双流终端状态发给上级
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：无
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/07/27  4.7         田志勇          创建
====================================================================*/
void CMcuVcInst::OnNtfDsMtStatusToMMcu()
{
	if( m_tCascadeMMCU.IsNull() || m_tDoubleStreamSrc.IsNull() )
	{
		return;
	}

	if( m_tDoubleStreamSrc == m_tCascadeMMCU)
	{
		return;
	}
	OnNtfMtStatusToMMcu(m_tCascadeMMCU.GetMtId(), m_tDoubleStreamSrc.GetMtId());
	TMt tRealDsMt = GetConfRealDsMt();
	if(tRealDsMt.IsNull())
	{
		return;
	}
	if (!tRealDsMt.IsLocal())
	{
		TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(GetMcuIdxFromMcuId(m_tDoubleStreamSrc.GetMtId()) );
		if(ptMcInfo == NULL)
		{
			ConfPrint(LOG_LVL_WARNING, MID_MCU_MT2, "[OnNtfDsMtStatusToMMcu]ptMcInfo == NULL,So Return!\n");
			return;
		}
		CServMsg cServMsg;
		TSMcuMtStatus tSMcuMtStatus;
		u8 byMtNum = 0;
		for( u8 byIndex = 0; byIndex < MAXNUM_CONF_MT; byIndex++ )
		{	
			if ( tRealDsMt == (TMt)(ptMcInfo->m_atMtStatus[byIndex]))
			{
				memset( &tSMcuMtStatus,0,sizeof(tSMcuMtStatus) );
				memcpy( (void*)&tSMcuMtStatus, (void*)&tRealDsMt, sizeof(TMt) );
				tSMcuMtStatus.SetMcuId( m_tDoubleStreamSrc.GetMtId() );
				tSMcuMtStatus.SetIsEnableFECC( ptMcInfo->m_atMtStatus[byIndex].IsEnableFECC() );
				tSMcuMtStatus.SetCurVideo( ptMcInfo->m_atMtStatus[byIndex].GetCurVideo() );
				tSMcuMtStatus.SetCurAudio( ptMcInfo->m_atMtStatus[byIndex].GetCurAudio() );
				tSMcuMtStatus.SetMtBoardType( ptMcInfo->m_atMtStatus[byIndex].GetMtBoardType() );
				tSMcuMtStatus.SetIsMixing( ptMcInfo->m_atMtStatus[byIndex].IsInMixing() );
				tSMcuMtStatus.SetVideoLose( ptMcInfo->m_atMtStatus[byIndex].IsVideoLose() );
				BOOL32 bAutoCallMode = (ptMcInfo->m_atMtExt[byIndex].GetCallMode() == CONF_CALLMODE_TIMER ) ? TRUE : FALSE;
				tSMcuMtStatus.SetIsAutoCallMode( bAutoCallMode );
				tSMcuMtStatus.SetRecvVideo( ptMcInfo->m_atMtStatus[byIndex].IsReceiveVideo() );
				tSMcuMtStatus.SetRecvAudio( ptMcInfo->m_atMtStatus[byIndex].IsReceiveAudio() );
				tSMcuMtStatus.SetSendVideo( ptMcInfo->m_atMtStatus[byIndex].IsSendVideo() );
				tSMcuMtStatus.SetSendAudio( ptMcInfo->m_atMtStatus[byIndex].IsSendAudio() );
				tSMcuMtStatus.SetDisconnectReason( ptMcInfo->m_atMtStatus[byIndex].GetMtDisconnectReason() );
				tSMcuMtStatus.SetSendVideo2(ptMcInfo->m_atMtStatus[byIndex].IsSndVideo2());
				byMtNum = 1;
				cServMsg.SetMsgBody( (u8*)&byMtNum,sizeof(u8) );
				cServMsg.CatMsgBody( (u8*)&tSMcuMtStatus,sizeof(tSMcuMtStatus) );
				cServMsg.SetEventId( MCU_MCU_MTSTATUS_NOTIF );
				ConfPrint(LOG_LVL_KEYSTATUS, MID_MCU_MMCU, "[OnNtfDsMtStatusToMMcu]DSmt(%d,%d) mtstatus isSndVideo2(%d) to mmcu\n",
					tSMcuMtStatus.GetMcuIdx(),tSMcuMtStatus.GetMtId(),tSMcuMtStatus.IsSendVideo2());
				SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_MTSTATUS_NOTIF, cServMsg );
				break;
			}		
		}
	}
}

/*====================================================================
函数名      IsRosterRecved
功能        ：是否收到某上级或下级mcu的花名册上报
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：无
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/11/21  4.7         周晶晶          创建
====================================================================*/
BOOL32 CMcuVcInst::IsRosterRecved( const u16 wMcuIdx )
{
	if( INVALID_MCUIDX == wMcuIdx )
	{
		return FALSE;
	}

	TConfMtInfo tConfMtInfo = m_tConfAllMtInfo.GetMtInfo( wMcuIdx );	
	return tConfMtInfo.IsMtExists();
}

// END OF FILE

