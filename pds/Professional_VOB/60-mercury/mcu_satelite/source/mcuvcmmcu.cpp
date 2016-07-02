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
#include "mcuerrcode.h"

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
        OspPrintf(TRUE, FALSE, "[DaemonProcMtMcuCreateConfReq] mt can't create conf in n+1 mode!\n");
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
    u16      wConfFirstRate = 0;
    u16      wConfSecondRate = 0;

	TConfInfo *ptConfInfo = NULL;
	if (cServMsg.GetMsgBodyLen() > 4*sizeof(TMtAlias)+2*sizeof(u8)+sizeof(u16))
	{
		ptConfInfo = (TConfInfo *)(cServMsg.GetMsgBody()+4*sizeof(TMtAlias)+2*sizeof(u8)+sizeof(u16));
	}

	cServMsg.SetSrcMtId(CONF_CREATE_MT);

    TLocalInfo tLocalInfo;
    g_cMcuAgent.GetLocalInfo(&tLocalInfo);
    u8 byCmpLen = max(strlen(tLocalInfo.GetE164Num()), strlen(tConfAlias.m_achAlias));
    
    // zbq [06/23/2007] 终端创会: 1、呼会议E164入会 或 呼模板E164通过模板创会；2、呼MCU别名/E164 创会或入会
    if ( mtAliasTypeE164 == tConfAlias.m_AliasType &&
         0 != memcmp( tLocalInfo.GetE164Num(), tConfAlias.m_achAlias, byCmpLen ) )
    {
        u8 byTemplateConfIdx = g_cMcuVcApp.GetTemConfIdxByE164(tConfAlias.m_achAlias);
	    u8 byOnGoingConfIdx  = g_cMcuVcApp.GetOngoingConfIdxByE164(tConfAlias.m_achAlias);

        CallLog("[DaemonProcMtMcuCreateConfReq] byTemplateConfIdx.%d byOnGoingConfIdx.%d\n", 
            byTemplateConfIdx, byOnGoingConfIdx);
  
	    if (byOnGoingConfIdx > 0)  //终端呼叫会议
	    {
			// 对于VCS级联会议，若下级调度席已被调度，不允许进行级联
			CConfId cConfId = g_cMcuVcApp.GetConfIdByE164(tConfAlias.m_achAlias, FALSE);
			if (!cConfId.IsNull() && VCS_CONF == cConfId.GetConfSource() && TYPE_MCU == byType)
			{
                CallLog("[DaemonProcMtMcuCreateConfReq] subconf has been created by other vcs\n");
				cServMsg.SetErrorCode(ERR_MCU_VCS_SMCUINVC);
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
	    
        }
	    else if (byTemplateConfIdx > 0)  //呼叫会议模板,创建会议
	    {
            //让会议开始
		    cServMsg.SetConfIdx(byTemplateConfIdx);
		    TTemplateInfo tTemConf;
		    if (!g_cMcuVcApp.GetTemplate(byTemplateConfIdx, tTemConf))
		    {
                CallLog("[DaemonProcMtMcuCreateConfReq] Get Template %d failed!\n", byTemplateConfIdx);
                g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			    return;
		    }

		    //码流加密
		    if (0 == byEncrypt && 
			    CONF_ENCRYPTMODE_NONE != tTemConf.m_tConfInfo.GetConfAttrb().GetEncryptMode())
		    {
                CallLog("[DaemonProcMtMcuCreateConfReq] Template encrypt!\n");
			    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			    return;
		    }

            //  xsl [2/13/2006] 在CreateConf处理中添加终端成功后回ack
            //SendReplyBack(cServMsg, cServMsg.GetEventId()+1);

		    tTemConf.m_tConfInfo.m_tStatus.SetOngoing();
		    tTemConf.m_tConfInfo.SetStartTime( time(NULL) );    
		    tTemConf.m_tConfInfo.m_tStatus.SetProtectMode( CONF_LOCKMODE_NONE );

		    wCallRate -= GetAudioBitrate( tTemConf.m_tConfInfo.GetMainAudioMediaType() );

		    TMtAlias atMtAlias[MAXNUM_CONF_MT+1];
            u16      awMtDialBitRate[MAXNUM_CONF_MT+1] = {0};
		    u8       byMtNumInUse = 0;
		    
            // zbq [09/19/2007] 呼模板创会，保持该终端在原模板中的呼叫模式不变; 若原为IP,注意保护端口.
		    atMtAlias[0] = tMtAddr;
		    awMtDialBitRate[0] = wCallRate;
		    byMtNumInUse = 1;

		    // 刷新终端列表，电视墙和画面合成模板
            TMultiTvWallModule tNewTvwallModule = tTemConf.m_tMultiTvWallModule;
            TVmpModule         tNewVmpModule    = tTemConf.m_atVmpModule;
            
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
			    else 
			    {
				    if (mtAliasTypeE164 == tTemConf.m_atMtAlias[byLoop].m_AliasType)
				    {
					    if (0 == memcmp(tTemConf.m_atMtAlias[byLoop].m_achAlias, tMtE164Alias.m_achAlias, MAXLEN_ALIAS))
					    {
						    bInMtTable = TRUE;
                            bRepeatThisLoop = TRUE;
                            
                            // zbq [09/19/2007] 取本来的呼叫模式
                            atMtAlias[0] = tMtE164Alias;
					    }
				    } 
				    else 
				    {
					    if (mtAliasTypeH323ID == tTemConf.m_atMtAlias[byLoop].m_AliasType)
					    {
						    if (0 == memcmp(tTemConf.m_atMtAlias[byLoop].m_achAlias, tMtH323Alias.m_achAlias, MAXLEN_ALIAS))
						    {
                                bInMtTable = TRUE;
                                bRepeatThisLoop = TRUE;

                                // zbq [09/19/2007] 取本来的呼叫模式
                                atMtAlias[0] = tMtH323Alias;
						    }
					    }
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
                                      tNewTvwallModule, tNewVmpModule);
		    }
            
		    //会议开放模式
		    //错误的开放模式
		    if ( CONF_OPENMODE_CLOSED  != tTemConf.m_tConfInfo.GetConfAttrb().GetOpenMode() &&
			     CONF_OPENMODE_NEEDPWD != tTemConf.m_tConfInfo.GetConfAttrb().GetOpenMode() &&
			     CONF_OPENMODE_OPEN    != tTemConf.m_tConfInfo.GetConfAttrb().GetOpenMode() ) 
		    {
                CallLog("[DaemonProcMtMcuCreateConfReq] Wrong Open Mode!\n");
			    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			    return;
		    }
		    //不公开
		    if ( CONF_OPENMODE_CLOSED == tTemConf.m_tConfInfo.GetConfAttrb().GetOpenMode() )
		    {
                CallLog("[DaemonProcMtMcuCreateConfReq] Open Mode is CLOSE!\n");
			    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
			    return;
		    }
			// xliang [12/26/2008] (modify for MT calling MCU initially) 
			//cancel limit here, but need to input password later
		    //会议根据密码加入
// 		    else if ( CONF_OPENMODE_NEEDPWD == tTemConf.m_tConfInfo.GetConfAttrb().GetOpenMode() &&
// 				      !bInMtTable )
// 		    {
//                 CallLog("[DaemonProcMtMcuCreateConfReq] Conf Need Password!\n");
// 			    g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
// 			    return;
// 		    }
            // 不在受邀列表且不在允许地址段
//             if (!bInMtTable && 
//                 (byType != TYPE_MCU) &&   // MCU 则不考虑IP是否允许段
//                 !g_cMcuVcApp.IsMtIpInAllowSeg( tTemConf.m_tConfInfo.GetUsrGrpId(), tMtAddr.m_tTransportAddr.GetNetSeqIpAddr()))
//             {
//                 CallLog("[DaemonProcMtMcuCreateConfReq] Type is not MCU(type=%d), neither in Allow IP range(0x%x)!\n", 
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
		    
		    cServMsg.SetMsgBody((u8 *)&tTemConf.m_tConfInfo, sizeof(TConfInfo));
		    cServMsg.CatMsgBody((u8 *)&wAliasBufLen, sizeof(wAliasBufLen));
		    cServMsg.CatMsgBody((u8 *)achAliasBuf, ntohs(wAliasBufLen));
		    if (tTemConf.m_tConfInfo.GetConfAttrb().IsHasTvWallModule())
		    {
			    cServMsg.CatMsgBody((u8*)&tNewTvwallModule, sizeof(TMultiTvWallModule));
		    }
		    if (tTemConf.m_tConfInfo.GetConfAttrb().IsHasVmpModule())
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
					ConfLog(FALSE, "[DaemonProcMtMcuCreateConfReq] VCSConf start command to inst%d\n",
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
                ConfLog(FALSE, "[DaemonProcMtMcuCreateConfReq] assign instance id failed!\n");
            }
        }
        else
        {
            cServMsg.SetErrorCode( ERR_MCU_NULLCID );
            g_cMtAdpSsnApp.SendMsgToMtAdpSsn(cServMsg.GetSrcDriId(), cServMsg.GetEventId()+2, cServMsg);
            ConfLog(FALSE, "[DaemonProcMtMcuCreateConfReq] UNEXIST E164.%s to be joined or created !\n", tConfAlias.m_achAlias);
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
			ConfLog( FALSE, "Conference %s failure because encrypt setting is confused - byEncrypt.%d EncryptMode.%d!\n", 
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
                ConfLog( FALSE, "Conference %s failure because create mt has no Alias(GK exist), ignore it!\n", ptConfInfo->GetConfName());
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
                ConfLog( FALSE, "Conference %s failure because create mt has no Alias, ignore it!\n", ptConfInfo->GetConfName());
                return;                
            }
        }
        
		//终端别名数组打包 
		// xliang [1/16/2009] 打包是否要带上会议模板信息中的其他终端----不需
		PackTMtAliasArray(&tMtCreateAlias, &wCallRate, 1, achAliasBuf, wAliasBufLen);
		wAliasBufLen = htons(wAliasBufLen);

		cServMsg.SetMsgBody((u8 *)ptConfInfo, sizeof(TConfInfo));
		cServMsg.CatMsgBody((u8*)&wAliasBufLen, sizeof(wAliasBufLen));
		cServMsg.CatMsgBody((u8*)achAliasBuf, ntohs(wAliasBufLen));
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
            CallLog("[DaemonProcMtMcuCreateConfReq] mt create conf, insId is %d\n", byInsID);
        }
        else
        {
            ConfLog(FALSE, "[DaemonProcMtMcuCreateConfReq] assign instance id failed!\n");
        }
		
	}
	else  //加入会议
	{
		cServMsg.SetMsgBody((u8*)&tMtH323Alias, sizeof(tMtH323Alias));
		cServMsg.CatMsgBody((u8*)&tMtE164Alias, sizeof(tMtE164Alias));
		cServMsg.CatMsgBody((u8*)&tMtAddr, sizeof(tMtAddr));
		cServMsg.CatMsgBody((u8*)&tConfAlias, sizeof(tConfAlias));
		cServMsg.CatMsgBody(&byType, sizeof(byType));
		cServMsg.CatMsgBody(&byEncrypt, sizeof(byEncrypt));
		cServMsg.CatMsgBody((u8*)&wCallRate, sizeof(wCallRate));
		g_cMcuVcApp.SendMsgToDaemonConf( MT_MCU_MTJOINCONF_REQ, cServMsg.GetServMsg(),cServMsg.GetServMsgLen() );
        CallLog("[DaemonProcMtMcuCreateConfReq] send MT_MCU_MTJOINCONF_REQ to daemon conf\n");
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
	s32 nPwdLen = cServMsg.GetMsgBodyLen();
	
	if( tMt.IsNull() )
	{
		return;
	}

	switch( CurState() )
	{
	case STATE_ONGOING:
		if ( m_tConfAllMtInfo.MtJoinedConf(tMt.GetMcuId(), tMt.GetMtId()) || 
			 TRUE == DealMtMcuEnterPwdRsp(tMt, pszPwd, nPwdLen) )
		{
			cServMsg.SetMsgBody((u8 *)&(m_tConfAllMtInfo.m_tLocalMtInfo), sizeof(TConfMtInfo));
			SendMsgToMt( tMt.GetMtId(), MCU_MCU_ROSTER_NOTIF,  cServMsg);
            MMcuLog("Recv MCU_MCU_REGISTER_NOTIF, Sending MCU_MCU_ROSTER_NOTIF!\n");
		}
		else
		{
			MMcuLog("Recv MCU_MCU_REGISTER_NOTIF but ConfPwd-Validate Err and drop mt.%d\n", tMt.GetMtId());
		}

        //将对应下级MCU的级联通道打开次数清零，认为打开成功 [01/11/2006-zbq]
        m_abyCasChnCheckTimes[tMt.GetMtId()-1] = 0;	
		MMcuLog("Recv MCU_MCU_REGISTER_NOTIF, m_abyCasChnCheckTimes[%d]=0 !\n", tMt.GetMtId()-1);
		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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

	switch( CurState() )
	{
	case STATE_ONGOING:
		
		//刷新TConfMtInfo
		m_tConfAllMtInfo.SetMtInfo( *ptConfMtInfo );

		//请求终端列表
		TMcuMcuReq tReq;
		memset(&tReq, 0, sizeof(tReq));
		cServMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));
		SendMsgToMt( tMt.GetMtId(), MCU_MCU_MTLIST_REQ,  cServMsg);
        
        MMcuLog("Recv MCU_MCU_ROSTER_NOTIF and send MCU_MCU_MTLIST_REQ to mt.%d\n", tMt.GetMtId());

		//发给会控会议所有终端信息
		cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
		SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
	
	STATECHECK;		

	//请求终端列表
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMtId());
    // 顾振华 [5/31/2006] MtId循环应该是1-192
	for(u8 byLoop = 1; byLoop <= MAXNUM_CONF_MT; byLoop++)
	{
		tTmpMt = m_ptMtTable->GetMt(byLoop);
		if(!tTmpMt.IsLocal())
		{
			continue;
		}
		atInfo[byInfoNum].m_tMt = tTmpMt;
		strncpy((s8 *)(atInfo[byInfoNum].m_szMtName), m_ptMtTable->GetMtAliasFromExt(byLoop), sizeof(atInfo[byInfoNum].m_szMtName) - 1);
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
		atInfo[byInfoNum].m_byIsConnected     = m_tConfAllMtInfo.MtJoinedConf(tTmpMt.GetMcuId(), tTmpMt.GetMtId()) ? 1:0;
		atInfo[byInfoNum].m_byIsFECCEnable    = m_ptMtTable->GetMtLogicChnnl( byLoop, LOGCHL_H224DATA, &tChannel, TRUE ); 
	
		atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount = 0;
		if(tTmpMt == m_tConf.GetSpeaker())
		{
			atInfo[byInfoNum].m_tPartVideoInfo.m_atViewPos[atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount].m_nViewID = m_dwSpeakerViewId;
			atInfo[byInfoNum].m_tPartVideoInfo.m_atViewPos[atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount].m_bySubframeIndex = 0;
			atInfo[byInfoNum].m_tPartVideoInfo.m_nViewCount++;
		}
		if(m_tConf.GetStatus().GetVmpParam().IsMtInMember(tTmpMt))
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

	MMcuLog( "[ProcMcuMcuMtListReq] Notified McuID.%d LocalTotalMtNum.%d\n", tMt.GetMtId(), byInfoNum );

	u8 byLastPack = 1;
	cServMsg.SetMsgBody(&byLastPack, sizeof(u8));
	cServMsg.CatMsgBody((u8 *)atInfo, byInfoNum * sizeof(TMcuMcuMtInfo));
	SendMsgToMt( tMt.GetMtId(), MCU_MCU_MTLIST_ACK,  cServMsg);

	//级联上级MCU后的第一次下级MCU已有终端状态强制通知，之后的个别终端状态进行实时通知
	if( m_tConfInStatus.IsNtfMtStatus2MMcu() ||
		// vcs刷列表，同时需要刷新其状态信息，由于其中包含重要的终端呼叫模式信息
		VCS_CONF == m_tConf.GetConfSource())
	{
		OnNtfMtStatusToMMcu( m_tCascadeMMCU.GetMtId() );
		m_tConfInStatus.SetNtfMtStatus2MMcu(FALSE);
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
void CMcuVcInst::GetConfViewInfo(u8 byMcuId, TCConfViewInfo &tInfo)
{
    u8 byViewIndex = 0;
    tInfo.m_byDefViewIndex = 0;
    
    TCapSupport tCapSupport = m_tConf.GetCapSupport();

    //spy view 
    TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(byMcuId);
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
        tInfo.m_atViewInfo[byViewIndex].m_atMts[0] = m_tConf.GetSpeaker();
        
        MMcuLog( "[ProcMcuMcuVideoInfoReq] Speaker byViewIndex.%d McuId.%d MtId.%d\n", 
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
    if(m_tConf.m_tStatus.GetVMPMode() != CONF_VMPMODE_NONE)
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
                tInfo.m_atViewInfo[byViewIndex].m_atMts[tInfo.m_atViewInfo[byViewIndex].m_byMtCount].SetMt(*((TMt *)(tVMPParam.GetVmpMember(nIndex))));
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
    
    MMcuLog( "[ProcMcuMcuVideoInfoReq] byViewCount.%d\n", byViewIndex );
    
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
    GetConfViewInfo( tMt.GetMtId(), tInfo );

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

	TConfMcInfo*	ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMtId()); 
	if(ptConfMcInfo == NULL)
	{
		return;
	}
	memcpy(&(ptConfMcInfo->m_tConfViewInfo), ptConfViewInfo, sizeof(TCConfViewInfo));

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

	TConfMcInfo*	ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMtId()); 
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
	
	TCConfAudioInfo*	ptConfAudioInfo = (TCConfAudioInfo *)(cServMsg.GetMsgBody());

	TConfMcInfo*	ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMtId()); 
	if(ptConfMcInfo == NULL)
	{
		return;
	}
	memcpy(&(ptConfMcInfo->m_tConfAudioInfo), ptConfAudioInfo, sizeof(TCConfAudioInfo));
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
void CMcuVcInst::GetLocalAudioInfo(TCConfAudioInfo &tAudioInfo)
{
    tAudioInfo.m_byMixerCount = 0;
    tAudioInfo.m_byDefMixerIndex = 0;
    tAudioInfo.m_tMixerList[0].m_tSpeaker.SetNull();
    if(!m_tConf.GetSpeaker().IsNull())
    {
        tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_nMixerID = m_dwSpeakerAudioId;
        tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_tSpeaker = m_tConf.GetSpeaker();
        if ( !m_tConf.GetSpeaker().IsLocal() )
        {
            u8 byMcuId = m_tConf.GetSpeaker().GetMcuId();
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
            TMt tRealSpeaker = GetMcuMediaSrc(byMcuId);
            if (!tRealSpeaker.IsNull())
            {
                tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_tSpeaker = tRealSpeaker;
            }  
        }
        tAudioInfo.m_byMixerCount++;
    }
    MMcuLog("[GetLocalAudioInfo] Local Speaker: (%d, %d)\n", 
        tAudioInfo.m_tMixerList[0].m_tSpeaker.GetMcuId(), 
        tAudioInfo.m_tMixerList[0].m_tSpeaker.GetMtId() );
    
    if(m_tConf.GetStatus().IsMixing())
    {
        tAudioInfo.m_byDefMixerIndex = tAudioInfo.m_byMixerCount;
        
        //zbq[11/01/2007] 混音优化
        //TMixParam tParam = m_tConf.GetStatus().GetMixParam();
        //tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_tSpeaker = tParam.m_atMtMember[0];
        tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_nMixerID = m_dwMixerAudioId;
        tAudioInfo.m_tMixerList[tAudioInfo.m_byMixerCount].m_tSpeaker = GetLocalSpeaker();
        tAudioInfo.m_byMixerCount++;
        
        MMcuLog("[GetLocalAudioInfo] Local Mixer: (%d, %d)\n", 
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

	TCConfAudioInfo tAudioInfo; 
    GetLocalAudioInfo(tAudioInfo);

	cServMsg.SetMsgBody((u8 *)&tAudioInfo, sizeof(tAudioInfo));
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
    if ( MODE_AUDIO == byMode || MODE_BOTH == byMode )
    {
        TCConfAudioInfo tAudioInfo; 
        GetLocalAudioInfo(tAudioInfo);  
        cServMsg.SetEventId(MCU_MCU_AUDIOINFO_ACK);
        cServMsg.SetMsgBody((u8 *)&tAudioInfo, sizeof(tAudioInfo));
        
        u8 byMcuId;
        for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
        {
            byMcuId = m_tConfAllMtInfo.m_atOtherMcMtInfo[byLoop].GetMcuId();
            if( byMcuId != 0 && m_tCascadeMMCU.GetMtId() != byMcuId &&
                (byTargetMcuId == 0 || byTargetMcuId == byMcuId ) )
            {
                SendMsgToMt( byMcuId, MCU_MCU_AUDIOINFO_ACK, cServMsg );
            }
        }
    }
    if ( MODE_VIDEO == byMode || MODE_BOTH == byMode )
    {
        cServMsg.SetEventId(MCU_MCU_VIDEOINFO_ACK);
        TCConfViewInfo tInfo;
        u8 byMcuId;
        for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
        {
            byMcuId = m_tConfAllMtInfo.m_atOtherMcMtInfo[byLoop].GetMcuId();
            if( byMcuId != 0 && m_tCascadeMMCU.GetMtId() != byMcuId &&
                (byTargetMcuId == 0 || byTargetMcuId == byMcuId ) )
            {
                GetConfViewInfo(byMcuId, tInfo);
                cServMsg.SetMsgBody((u8 *)&tInfo, sizeof(tInfo));
                SendMsgToMt( byMcuId, MCU_MCU_VIDEOINFO_ACK, cServMsg );
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
    GetLocalAudioInfo(tAudioInfo);
    cMsg.SetEventId(MCU_MCU_AUDIOINFO_ACK);
    cMsg.SetMsgBody((u8 *)&tAudioInfo, sizeof(tAudioInfo));
    SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_AUDIOINFO_ACK, cMsg );
    
    TCConfViewInfo tVideoInfo;                
    GetConfViewInfo(m_tCascadeMMCU.GetMtId(), tVideoInfo);
    cMsg.SetEventId(MCU_MCU_VIDEOINFO_ACK);
    cMsg.SetMsgBody((u8 *)&tVideoInfo, sizeof(tVideoInfo));
    SendMsgToMt( m_tCascadeMMCU.GetMtId(), MCU_MCU_VIDEOINFO_ACK, cMsg );
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

	TConfMcInfo *ptMcMtInfo = m_ptConfOtherMcTable->AddMcInfo(tMt.GetMtId());
	if(ptMcMtInfo == NULL)
	{
		return;
	}
	
	MMcuLog( "[ProcMcuMcuMtListAck] RemoteMcuID.%d LastMtNum.%d,This Pack include MtNum.%d\n", 
		     tMt.GetMtId(), ptMcMtInfo->m_byLastPos, nMtNum );

	
	//全新的一包
	if(ptMcMtInfo->m_byLastPos == 0)
	{
		//部分初始化
		memset(&(ptMcMtInfo->m_atMtExt), 0, sizeof(ptMcMtInfo->m_atMtExt));
		//memset(&(ptMcMtInfo->m_atMtStatus), 0, sizeof(ptMcMtInfo->m_atMtStatus));
		ptMcMtInfo->m_byMcuId = 0;
		memset(&(ptMcMtInfo->m_tConfAudioInfo), 0, sizeof(ptMcMtInfo->m_tConfAudioInfo));
		memset(&(ptMcMtInfo->m_tConfViewInfo), 0, sizeof(ptMcMtInfo->m_tConfViewInfo));
	}
	ptMcMtInfo->m_byMcuId = tMt.GetMtId();

	s32 nIndex = 0;
	s32 nLoop  = 0;
	for(nIndex = ptMcMtInfo->m_byLastPos, nLoop =0; nIndex<ptMcMtInfo->m_byLastPos+nMtNum&&nIndex<MAXNUM_CONF_MT; nIndex++, nLoop++)
	{
		ptMcMtInfo->m_atMtStatus[nIndex].SetAudioIn(ptMcuMcuMtInfo[nLoop].m_byAudioIn);
		ptMcMtInfo->m_atMtStatus[nIndex].SetAudioOut(ptMcuMcuMtInfo[nLoop].m_byAudioOut);
		ptMcMtInfo->m_atMtStatus[nIndex].SetSendAudio(!ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsAudioMuteIn));
		if(ptMcMtInfo->m_atMtStatus[nIndex].GetAudioIn() == MEDIA_TYPE_NULL)
		{
			ptMcMtInfo->m_atMtStatus[nIndex].SetSendAudio(FALSE);
		}
		ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveAudio(!ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsAudioMuteOut));
		if(ptMcMtInfo->m_atMtStatus[nIndex].GetAudioOut() == MEDIA_TYPE_NULL)
		{
			ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveAudio(FALSE);
		}
		ptMcMtInfo->m_atMtExt[nIndex].SetMt(ptMcuMcuMtInfo[nLoop].m_tMt);
		ptMcMtInfo->m_atMtStatus[nIndex].SetMt(ptMcuMcuMtInfo[nLoop].m_tMt);
		ptMcMtInfo->m_atMtExt[nIndex].SetAlias(ptMcuMcuMtInfo[nLoop].m_szMtName);
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
		ptMcMtInfo->m_atMtStatus[nIndex].SetSendVideo(!ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsVideoMuteIn));    
		ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveVideo(!ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsVideoMuteOut));
		if(ptMcMtInfo->m_atMtStatus[nIndex].GetVideoIn() == MEDIA_TYPE_NULL)
		{
			ptMcMtInfo->m_atMtStatus[nIndex].SetSendVideo(FALSE);    
		}
		if(ptMcMtInfo->m_atMtStatus[nIndex].GetVideoOut() == MEDIA_TYPE_NULL)
		{
			ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveVideo(FALSE);
		}

		TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(tMt.GetMtId());
		tMtInfo.RemoveMt(ptMcuMcuMtInfo[nLoop].m_tMt.GetMtId());
		if(ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsConnected))
		{
			tMtInfo.AddJoinedMt(ptMcuMcuMtInfo[nLoop].m_tMt.GetMtId());
		}
		else
		{
			tMtInfo.AddMt(ptMcuMcuMtInfo[nLoop].m_tMt.GetMtId());
		}
		m_tConfAllMtInfo.SetMtInfo(tMtInfo);
        ptMcMtInfo->m_atMtStatus[nIndex].SetIsEnableFECC(ISTRUE(ptMcuMcuMtInfo[nLoop].m_byIsFECCEnable));
		ptMcMtInfo->m_atMtStatus[nIndex].SetMtVideoInfo(ptMcuMcuMtInfo[nLoop].m_tPartVideoInfo);  
	}
	if(ISTRUE(byIsLastPack))
	{
        // 顾振华 [6/1/2006] 清除后面的内容
        if (nIndex < MAXNUM_CONF_MT)
        {
            memset( ptMcMtInfo->m_atMtStatus+nIndex, 0, sizeof(TMcMtStatus) * (MAXNUM_CONF_MT - nIndex) );
        }
        
		ptMcMtInfo->m_byLastPos = 0;

		cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
		SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

		//发给会控终端表
		SendMtListToMcs(tMt.GetMtId());

        // 顾振华 [6/1/2006] 发送下级Mcu终端状态给会控
	    CServMsg cServBackMsg;
	    cServBackMsg.SetNoSrc();
        cServBackMsg.SetMsgBody();
		for(nIndex = 0; nIndex < MAXNUM_CONF_MT; nIndex++)
		{
			if( ptMcMtInfo->m_atMtStatus[nIndex].IsNull() || 
				ptMcMtInfo->m_atMtStatus[nIndex].GetMtId() == 0 ) //自己
			{
				continue;
			}
            TMtStatus tMtStatus = ptMcMtInfo->m_atMtStatus[nIndex].GetMtStatus();
            cServBackMsg.CatMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );	
        }
        if (cServBackMsg.GetMsgBodyLen() != 0)
        {
            SendMsgToAllMcs( MCU_MCS_MTSTATUS_NOTIF, cServBackMsg );       
        }        
	}
	else
	{
		ptMcMtInfo->m_byLastPos += nMtNum;
	}
	
	return;
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

	//增加到终端表里
	TConfMtInfo tConfMtInfo = m_tConfAllMtInfo.GetMtInfo(tMt.GetMtId());
    if(tConfMtInfo.IsNull())
	{
		return;
	}
	tConfMtInfo.AddJoinedMt(tMcuMcuMtInfo.m_tMt.GetMtId());
	m_tConfAllMtInfo.SetMtInfo(tConfMtInfo);

	//发给会控会议所有终端信息
	cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );
	
	//查找mc table
	TConfMcInfo *ptMcMtInfo = m_ptConfOtherMcTable->AddMcInfo(tMt.GetMtId());
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
		if(ptMcMtInfo->m_atMtStatus[nIndex].GetMtId() == tMcuMcuMtInfo.m_tMt.GetMtId())
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
	ptMcMtInfo->m_atMtStatus[nIndex].SetSendAudio(!ISTRUE(tMcuMcuMtInfo.m_byIsAudioMuteIn));
	ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveAudio(!ISTRUE(tMcuMcuMtInfo.m_byIsAudioMuteOut));
	ptMcMtInfo->m_atMtExt[nIndex].SetMt(tMcuMcuMtInfo.m_tMt);
	ptMcMtInfo->m_atMtStatus[nIndex].SetMt(tMcuMcuMtInfo.m_tMt);
	ptMcMtInfo->m_atMtExt[nIndex].SetAlias(tMcuMcuMtInfo.m_szMtName);
	ptMcMtInfo->m_atMtExt[nIndex].SetIPAddr(ntohl(tMcuMcuMtInfo.m_dwMtIp));
	ptMcMtInfo->m_atMtExt[nIndex].SetMtType(tMcuMcuMtInfo.m_byMtType);
	ptMcMtInfo->m_atMtExt[nIndex].SetManuId(tMcuMcuMtInfo.m_byManuId);
	ptMcMtInfo->m_atMtStatus[nIndex].SetMtType(tMcuMcuMtInfo.m_byMtType);
	ptMcMtInfo->m_atMtStatus[nIndex].SetVideoIn(tMcuMcuMtInfo.m_byVideoIn);
	ptMcMtInfo->m_atMtStatus[nIndex].SetVideoOut(tMcuMcuMtInfo.m_byVideoOut);
//	ptMcMtInfo->m_atMtStatus[nIndex].SetVideo2In(tMcuMcuMtInfo.m_byVideo2In);
//	ptMcMtInfo->m_atMtStatus[nIndex].SetVideo2Out(tMcuMcuMtInfo.m_byVideo2Out);
	ptMcMtInfo->m_atMtStatus[nIndex].SetSendVideo(!ISTRUE(tMcuMcuMtInfo.m_byIsVideoMuteIn));    
	ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveVideo(!ISTRUE(tMcuMcuMtInfo.m_byIsVideoMuteOut));
    TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(tMt.GetMtId());
	tMtInfo.RemoveMt(tMcuMcuMtInfo.m_tMt.GetMtId());
	if(ISTRUE(tMcuMcuMtInfo.m_byIsConnected))
	{
		tMtInfo.AddJoinedMt(tMcuMcuMtInfo.m_tMt.GetMtId());
	}
	else
	{
		tMtInfo.AddMt(tMcuMcuMtInfo.m_tMt.GetMtId());
	}
	m_tConfAllMtInfo.SetMtInfo(tMtInfo);
    ptMcMtInfo->m_atMtStatus[nIndex].SetIsEnableFECC(ISTRUE(tMcuMcuMtInfo.m_byIsFECCEnable));
	ptMcMtInfo->m_atMtStatus[nIndex].SetMtVideoInfo(tMcuMcuMtInfo.m_tPartVideoInfo);  

	
	MMcuLog( "[ProcMcuMcuNewMtNotify] nIndex.%d MtId.%d szMtName.%s bOnLine.%d\n", 
		     nIndex, tMcuMcuMtInfo.m_tMt.GetMtId(), tMcuMcuMtInfo.m_szMtName, tMcuMcuMtInfo.m_byIsConnected );

	SendMtListToMcs( tMt.GetMtId() );

	// 对于VCS会议，还需要进行选看，回传操作
	if (VCS_CONF == m_tConf.GetConfSource())
	{
		TMt tNewMt = tMcuMcuMtInfo.m_tMt;
		MtOnlineChange(tNewMt, TRUE, 0);

		if (m_cVCSConfStatus.GetCurVCMT() == tNewMt)
		{
			// 将该下级mcu原回传通道中的成员挂断
			TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tNewMt.GetMcuId());
			if (ptMcInfo != NULL && !(ptMcInfo->m_tMMcuViewMt.IsNull()) &&
				m_tConfAllMtInfo.MtJoinedConf(ptMcInfo->m_tMMcuViewMt.GetMcuId(), ptMcInfo->m_tMMcuViewMt.GetMtId()) &&
				!(ptMcInfo->m_tMMcuViewMt == tNewMt))
			{
				MMcuLog("[ProcMcuMcuNewMtNotify]drop mt(mcuid:%d, mtid:%d) because of new submt(mcuid:%d, mtid:%d) online\n",
					    ptMcInfo->m_tMMcuViewMt.GetMcuId(), ptMcInfo->m_tMMcuViewMt.GetMtId(), tNewMt.GetMcuId(), tNewMt.GetMtId());
				VCSDropMT(ptMcInfo->m_tMMcuViewMt);	
			}

			OnMMcuSetIn(tNewMt, m_cVCSConfStatus.GetCurSrcSsnId(), SWITCH_MODE_SELECT);
		}

		if (!ISGROUPMODE(m_cVCSConfStatus.GetCurVCMode()))
		{
			GoOnSelStep(tMt, MODE_VIDEO, TRUE);
			GoOnSelStep(tMt, MODE_AUDIO, TRUE);
			GoOnSelStep(tNewMt, MODE_VIDEO, FALSE);
			GoOnSelStep(tNewMt, MODE_AUDIO, FALSE);
		}

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

	STATECHECK;

	//增加到终端表里
	TConfMtInfo tConfMtInfo = m_tConfAllMtInfo.GetMtInfo(tMt.GetMtId());
    if(tConfMtInfo.IsNull())
	{
		return;
	}

	tConfMtInfo.AddMt( tAlertMt.GetMtId() );
	m_tConfAllMtInfo.SetMtInfo( tConfMtInfo );
	
	//查找mc table
	TConfMcInfo *ptMcMtInfo = m_ptConfOtherMcTable->AddMcInfo(tMt.GetMtId());
	if( NULL == ptMcMtInfo )
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
		if(ptMcMtInfo->m_atMtStatus[nIndex].GetMtId() == tAlertMt.GetMtId())
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
	cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

	ptMcMtInfo->m_atMtStatus[nIndex].SetAudioIn(MEDIA_TYPE_NULL);
	ptMcMtInfo->m_atMtStatus[nIndex].SetAudioOut(MEDIA_TYPE_NULL);
	ptMcMtInfo->m_atMtStatus[nIndex].SetSendAudio( FALSE );
	ptMcMtInfo->m_atMtStatus[nIndex].SetReceiveAudio( FALSE );

	ptMcMtInfo->m_atMtExt[nIndex].SetMt(tAlertMt);
	ptMcMtInfo->m_atMtStatus[nIndex].SetMt(tAlertMt);

	if( mtAliasTypeTransportAddress == tAlertMtAlias.m_AliasType )
	{
		ptMcMtInfo->m_atMtExt[nIndex].SetIPAddr(tAlertMtAlias.m_tTransportAddr.GetIpAddr());
		u32  dwDialIP  = tAlertMtAlias.m_tTransportAddr.GetIpAddr();
		ptMcMtInfo->m_atMtExt[nIndex].SetAlias(StrOfIP(dwDialIP));
	}
	else
	{
		ptMcMtInfo->m_atMtExt[nIndex].SetAlias(tAlertMtAlias.m_achAlias);
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
    TConfMtInfo tMtInfo = m_tConfAllMtInfo.GetMtInfo(tMt.GetMtId());
	tMtInfo.RemoveMt( tAlertMt.GetMtId() );
	tMtInfo.AddMt( tAlertMt.GetMtId() );
	m_tConfAllMtInfo.SetMtInfo(tMtInfo);
    ptMcMtInfo->m_atMtStatus[nIndex].SetIsEnableFECC( FALSE );

	//ptMcMtInfo->m_atMtStatus[nIndex].m_tPartVideoInfo =  tMcuMcuMtInfo.m_tPartVideoInfo;  
	
	MMcuLog( "[ProcMcuMcuCallAlertMtNotify] nIndex.%d MtId.%d\n", nIndex, tAlertMt.GetMtId() );

	SendMtListToMcs( tMt.GetMtId() );

	return;
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
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt			tMt = m_ptMtTable->GetMt(cServMsg.GetSrcMtId());
	TMt*		ptDropMt = (TMt* )(cServMsg.GetMsgBody());

	STATECHECK;

	//若该终端作为非直连终端是本地发言人，清空本地发言人相关
	if ( *ptDropMt == m_tConf.GetSpeaker() )
	{
		ChangeSpeaker( NULL );
	}
	
	//增加到终端表里
	TConfMtInfo tConfMtInfo = m_tConfAllMtInfo.GetMtInfo(ptDropMt->GetMcuId());
    if(tConfMtInfo.IsNull())
	{
		return;
	}

	tConfMtInfo.RemoveJoinedMt(ptDropMt->GetMtId());
	m_tConfAllMtInfo.SetMtInfo(tConfMtInfo);
    
    // guzh [3/9/2007] 通知MCS
    MtOnlineChange(*ptDropMt, FALSE, MTLEFT_REASON_NORMAL);

	cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

    if ( *ptDropMt == (TMt)(m_tConf.m_tStatus.GetMtPollParam()) )
    {
        SetTimer( MCUVC_POLLING_CHANGE_TIMER, 10 );
    }

	//有关画面合成的处理
	if (VCS_CONF == m_tConf.GetConfSource() && 
		VCS_MULVMP_MODE == m_cVCSConfStatus.GetCurVCMode())
	{
		ChangeVmpStyle(tMt, FALSE);

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
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	u8			byMtId = cServMsg.GetSrcMtId();
	TMt			tDelMt = *(TMt* )(cServMsg.GetMsgBody());

	STATECHECK;

	//增加到终端表里
	TConfMtInfo tConfMtInfo = m_tConfAllMtInfo.GetMtInfo(tDelMt.GetMcuId());
    if(tConfMtInfo.IsNull())
	{
		return;
	}
    
	if(tConfMtInfo.MtInConf(tDelMt.GetMtId()))
	{
		tConfMtInfo.RemoveMt(tDelMt.GetMtId());
		//SendMsgToMt(byMtId, MCU_MCU_DELMT_NOTIF,cServMsg); 
	}
	m_tConfAllMtInfo.SetMtInfo(tConfMtInfo);
    
	//发给会控会议所有终端信息
	cServMsg.SetMsgBody( ( u8 * )&m_tConfAllMtInfo, sizeof( TConfAllMtInfo ) );
	SendMsgToAllMcs( MCU_MCS_CONFALLMTINFO_NOTIF, cServMsg );

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
		if(bFind)
		{
			SendMtListToMcs(tDelMt.GetMcuId());
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
====================================================================*/
void CMcuVcInst::SendMtListToMcs(u8 byMcuId)
{
	//发给会控终端表
	TMcu tMcu;
	tMcu.SetMcu( byMcuId );
	CServMsg cServMsg;
	if(byMcuId == LOCAL_MCUID)
	{
		cServMsg.SetMsgBody( (u8*)&tMcu, sizeof(tMcu) );
		cServMsg.CatMsgBody( ( u8 * )m_ptMtTable->m_atMtExt, 
		                  	 m_ptMtTable->m_byMaxNumInUse * sizeof( TMtExt ) );
	}
	else
	{
		TConfMcInfo* ptMcMtInfo = m_ptConfOtherMcTable->GetMcInfo(byMcuId);	
		if(ptMcMtInfo == NULL)
		{
			return;
		}
        // guzh [4/30/2007] 上级MCU列表过滤
        if ( !g_cMcuVcApp.IsShowMMcuMtList() && 
            !m_tCascadeMMCU.IsNull() && m_tCascadeMMCU.GetMtId() == tMcu.GetMcuId() )
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
	}
	SendMsgToAllMcs( MCU_MCS_MTLIST_NOTIF, cServMsg );
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

	TConfMcInfo *ptInfo = m_ptConfOtherMcTable->GetMcInfo(ptMtChanStatus->m_tMt.GetMcuId());
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
                ptStatus->SetSendVideo(!ptMtChanStatus->m_bMute);
				//ptStatus->SetVideoIn(GETBBYTE(!(ptMtChanStatus->m_bMute)));
			}
			else
			{
                ptStatus->SetReceiveVideo(!ptMtChanStatus->m_bMute);
				//ptStatus->SetVideoOut(GETBBYTE(!(ptMtChanStatus->m_bMute)));
			}
		}
		else if(ptMtChanStatus->m_byMediaMode == MODE_AUDIO)
		{
			if(ISTRUE(ptMtChanStatus->m_byIsDirectionIn))
			{
                ptStatus->SetSendAudio(!(ptMtChanStatus->m_bMute));
				//ptStatus->SetAudioIn(GETBBYTE(!(ptMtChanStatus->m_bMute)));
			}
			else
			{
                ptStatus->SetReceiveAudio(!(ptMtChanStatus->m_bMute));
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
void CMcuVcInst::OnSetOutView( TMt &tSetInMt )
{
    if ( m_tCascadeMMCU.IsNull() )
    {
        return;
    }

    if ( !tSetInMt.IsLocal() )
    {
        // 首先通知下级MCU
        OnMMcuSetIn( tSetInMt, 0, SWITCH_MODE_BROADCAST );
    }
    
    TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
    if (ptConfMcInfo->m_tSpyMt == tSetInMt)
    {
        // 相同则直接返回
        return ;
    }
    
    tSetInMt = GetLocalMtFromOtherMcuMt(tSetInMt);
    
    TMt tMMcu;
    tMMcu.SetMcuId(m_tCascadeMMCU.GetMtId());
    tMMcu.SetMtId(0);
    OnMMcuSetIn(tMMcu, 0, SWITCH_MODE_BROADCAST, TRUE); //不切换发言人，此时给上级发setin消息的作用？？？    
    
    ptConfMcInfo->m_tSpyMt = tSetInMt;
    
    //send output notify
    CServMsg cMsg;
    TSetOutParam tOutParam;
    tOutParam.m_nMtCount = 1;
    tOutParam.m_atConfViewOutInfo[0].m_tMt = tSetInMt;
    tOutParam.m_atConfViewOutInfo[0].m_nOutViewID = ptConfMcInfo->m_dwSpyViewId;
    tOutParam.m_atConfViewOutInfo[0].m_nOutVideoSchemeID = ptConfMcInfo->m_dwSpyVideoId;
    cMsg.SetMsgBody((u8 *)&tOutParam, sizeof(tOutParam));
    cMsg.SetEventId(MCU_MCU_SETOUT_NOTIF);
    SendMsgToMt(m_tCascadeMMCU.GetMtId(), MCU_MCU_SETOUT_NOTIF, cMsg);
    
    NotifyMtSend(tSetInMt.GetMtId(), MODE_BOTH);
    
    //混音优先
    u8 byMode = MODE_BOTH;
    if(m_tConf.m_tStatus.IsMixing())
    {
        byMode = MODE_VIDEO;
    }
    
    StartSwitchToMcu(tSetInMt, 0, m_tCascadeMMCU.GetMtId(), byMode, SWITCH_MODE_SELECT);
    if (m_tConf.GetConfAttrb().IsResendLosePack())
    {
        //把目的视频Rtcp交换给源
        TLogicalChannel tLogicalChannel;
        m_ptMtTable->GetMtLogicChnnl(tSetInMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);
        
        u32 dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
        u16 wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
        // guzh [7/25/2007] 回传通道也要Map
        //将 MT.RTCP -> PRS 时，考虑到防火墙支撑，需将交换包的源ip、port映射为 MT.RTP，以便PRS重传        
        SwitchVideoRtcpToDst(dwDstIp, wDstPort, m_tCascadeMMCU, 0, MODE_VIDEO, SWITCH_MODE_SELECT, TRUE);
        
        //把目的音频Rtcp交换给源
        m_ptMtTable->GetMtLogicChnnl(tSetInMt.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE);
        
        dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
        wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
        SwitchVideoRtcpToDst(dwDstIp, wDstPort, m_tCascadeMMCU, 0, MODE_AUDIO, SWITCH_MODE_SELECT, TRUE);
    }   
    
    // guzh [5/9/2007] 通知上级新的视频/音频源
    // NofityMMcuMediaInfo();
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

    TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(m_tCascadeMMCU.GetMtId());
    if(ptConfMcInfo == NULL)
    {
        //NACK
        SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
        return;
    }    
    
    //ACK
    SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );

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

	STATECHECK;

	if( !ptSetInParam->m_tMt.IsLocal() ||
		ptSetInParam->m_tMt.GetMtId() == byMtId )
	{
		//ACK
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
		return;
	}
	
	ptSetInParam->m_tMt = m_ptMtTable->GetMt(ptSetInParam->m_tMt.GetMtId());
	TConfMcInfo *ptConfMcInfo = m_ptConfOtherMcTable->GetMcInfo(byMtId);
	if(ptConfMcInfo == NULL)
	{
		//NACK
		SendReplyBack( cServMsg, cServMsg.GetEventId() + 2 );
		return;
	}

	TMt tMt;
	tMt.SetMcuId(byMtId);
	tMt.SetMtId(0);
	OnMMcuSetIn(tMt, 0, SWITCH_MODE_BROADCAST, TRUE); //不切换发言人，此时给上级发setin消息的作用？？？
	
	
	//send output notify
	CServMsg cMsg;
	TSetOutParam tOutParam;
	tOutParam.m_nMtCount = 1;
	tOutParam.m_atConfViewOutInfo[0].m_tMt = m_ptMtTable->GetMt(byMtId);
	tOutParam.m_atConfViewOutInfo[0].m_nOutViewID = ptConfMcInfo->m_dwSpyViewId;
	tOutParam.m_atConfViewOutInfo[0].m_nOutVideoSchemeID = ptConfMcInfo->m_dwSpyVideoId;
	cMsg.SetMsgBody((u8 *)&tOutParam, sizeof(tOutParam));
	cMsg.SetEventId(MCU_MCU_SETOUT_NOTIF);
	SendMsgToMt(byMtId, MCU_MCU_SETOUT_NOTIF, cMsg);


	ptConfMcInfo->m_tSpyMt = ptSetInParam->m_tMt;
	NotifyMtSend( ptSetInParam->m_tMt.GetMtId(), MODE_BOTH);
	TMt tDstMt = m_ptMtTable->GetMt(byMtId);
	TMt tSrcMt = ptSetInParam->m_tMt;
   
	//混音优先
	u8 byMode = MODE_BOTH;
	if(m_tConf.m_tStatus.IsMixing())
	{
		byMode = MODE_VIDEO;
	}

    StartSwitchToMcu(tSrcMt, 0, tDstMt.GetMtId(), byMode, SWITCH_MODE_SELECT);
    if (m_tConf.GetConfAttrb().IsResendLosePack())
    {
        //把目的视频Rtcp交换给源
        TLogicalChannel tLogicalChannel;
        m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, FALSE);

        u32 dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
        u16 wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
        // guzh [7/25/2007] 回传通道也要Map
        //将 MT.RTCP -> PRS 时，考虑到防火墙支撑，需将交换包的源ip、port映射为 MT.RTP，以便PRS重传        
        SwitchVideoRtcpToDst(dwDstIp, wDstPort, tDstMt, 0, MODE_VIDEO, SWITCH_MODE_SELECT, TRUE);

        //把目的音频Rtcp交换给源
        m_ptMtTable->GetMtLogicChnnl(tSrcMt.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, FALSE);

        dwDstIp = tLogicalChannel.GetSndMediaCtrlChannel().GetIpAddr();
        wDstPort = tLogicalChannel.GetSndMediaCtrlChannel().GetPort();
        SwitchVideoRtcpToDst(dwDstIp, wDstPort, tDstMt, 0, MODE_AUDIO, SWITCH_MODE_SELECT, TRUE);
    }

	//ACK
	SendReplyBack( cServMsg, cServMsg.GetEventId() + 1 );
	
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

	STATECHECK;

	TConfMcInfo *ptInfo = m_ptConfOtherMcTable->GetMcInfo(byMtId);
	if(ptInfo == NULL)
	{
		return;
	}
	
	if(ptSetOutParam->m_tMt.IsNull())
	{
		return;
	}
	TMcMtStatus *ptMcMtStatus = ptInfo->GetMtStatus(ptSetOutParam->m_tMt);
	if(ptMcMtStatus ==  NULL)
	{
		return;
	}
    TMtVideoInfo tInfo = ptMcMtStatus->GetMtVideoInfo();
	tInfo.m_nOutputLID = ptSetOutParam->m_nOutViewID;
	tInfo.m_nOutVideoSchemeID = ptSetOutParam->m_nOutVideoSchemeID;
    ptMcMtStatus->SetMtVideoInfo(tInfo);

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
    u8 byMtNum = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMcu));
    if ( byMtNum == 0 || m_tConf.m_tStatus.IsNoMixing() )
    {
        ProcMixStart(cServMsg);
    }
    else
    {
        TMt *ptMt = (TMt*)(cServMsg.GetMsgBody() + sizeof(TMcu) + sizeof(u8));
        AddRemoveSpecMixMember(ptMt, byMtNum);
    }
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

	//混音开始成功，请求一次混音参数
	if (!tMcu.IsLocal())
	{
		OnGetMixParamToSMcu(&cServMsg);
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
	
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(ptMcu->GetMcuId());
	if(ptMcInfo == NULL)
	{
		return;
	}
	
    // guzh [11/7/2007] 
	//cServMsg.SetSrcMtId(0);
	//cServMsg.SetEventId(MCU_MCS_STOPDISCUSS_NOTIF);
	//SendMsgToAllMcs(MCU_MCS_STOPDISCUSS_NOTIF,cServMsg); 

    // xsl [8/22/2006] 剔除会导致下级定制混音恢复时不能恢复级联混音
	//剔除下级mcu在本级的混音成员位置
//	if( m_tConfAllMtInfo.MtJoinedConf( ptMcu->GetMcuId() ) && 
//		m_ptMtTable->IsMtInMixing( ptMcu->GetMcuId() )  )            
//	{
//		TMt tMcuMt = m_ptMtTable->GetMt(ptMcu->GetMcuId());
//		AddRemoveSpecMixMember( &tMcuMt, 1, FALSE );
//	}

    // xsl [7/28/2006] 更新终端混音状态
    if (!m_ptConfOtherMcTable->ClearMtInMixing(*ptMcu))
    {
        ConfLog(FALSE, "[ProcMcuMcuStopMixerNotif] ClearMtInMixing failed!\n");
    }       

	return;
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
void CMcuVcInst::CascadeAdjMtRes( TMt tMt, u8 byNewFormat, BOOL32 bStart, u8 byVmpStyle, u8 byPos)
{
	CServMsg cServMsg;
	cServMsg.SetEventId( MCU_MCU_ADJMTRES_REQ );
	u8 byStart = bStart ? 1: 0;
	cServMsg.SetMsgBody( (u8 *)&tMt, sizeof(TMt) );		//要调整的Mt
	cServMsg.CatMsgBody( &byStart, sizeof(u8) );		//是否是恢复Res
	cServMsg.CatMsgBody( &byNewFormat, sizeof(u8) );	//要调至的新分辨率

	// 填以下2项，下级则可以自己查表获取要调到的RES
	if( byVmpStyle <= VMP_STYLE_TWENTY && byVmpStyle > 0
		&& byPos < MAXNUM_MPUSVMP_MEMBER )
	{
		cServMsg.CatMsgBody( &byVmpStyle, sizeof(u8) );		//VMP 合成风格
		cServMsg.CatMsgBody( &byPos, sizeof(u8) );			//所处VMP通道位置
	}
	
	SendMsgToMt( tMt.GetMcuId(), cServMsg.GetEventId(), cServMsg);
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

	BOOL32 bRspNack	= FALSE;				//是否回nack
	BOOL32 bNeedAdjustRes = TRUE;			//默认是要调分辨率
	BOOL32 bGetResBySelf = FALSE;			//是否自己查表获取要调整的分辨率
	u8 byChnnlType = LOGCHL_VIDEO;			//目前只改变第一路视频通道的分辨率

	TMt tMt = *(TMt *)cServMsg.GetMsgBody();
	u8 byMtId = tMt.GetMtId();
	u8 byStart = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt));		//开始调Res
	u8 byReqRes = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8)); 
	u8 byPos = ~0;
	BOOL32 bNoneKeda = ( MT_MANU_KDC != m_ptMtTable->GetManuId(byMtId)  
							&& MT_MANU_KDCMCU != m_ptMtTable->GetManuId(byMtId) );
	
	CallLog("[ProcMcuMcuAdjustMtResReq] begin adjust Mt.%u(bNoneKeda: %d) resolution!\n",byMtId, bNoneKeda);

	TLogicalChannel tLogicChannel;
	if ( !m_tConfAllMtInfo.MtJoinedConf( byMtId ) ||
		!m_ptMtTable->GetMtLogicChnnl( byMtId, byChnnlType, &tLogicChannel, FALSE ) )
	{
		CallLog("[ProcMcuMcuAdjustMtResReq]Backward LC has problems. Nack!!\n");
		bRspNack = TRUE;
	}
	
	if( bNoneKeda )	//非科达的回nack
	{
		CallLog("[ProcMcuMcuAdjustMtResReq]None Keda Mt. Nack!!\n");
		bRspNack = TRUE;
	}

	if(bRspNack)
	{
		SendReplyBack( cServMsg, MCU_MCU_ADJMTRES_NACK );
		return;
	}

	if( cServMsg.GetMsgBodyLen() > sizeof(TMt) + sizeof(u8) + sizeof(u8) )
	{
		bGetResBySelf = TRUE;
	}
	
	u8 byMtStandardFormat = tLogicChannel.GetVideoFormat(); // 原始分辨率
	u8 byNewFormat;
	if( bGetResBySelf)
	{	
		byPos = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) + sizeof(u8) + sizeof(u8) );
		if( MEDIA_TYPE_H264 != tLogicChannel.GetMediaType() )
		{
			byReqRes = VIDEO_FORMAT_CIF;
		}
		else
		{
			u8 byVmpStyle =  *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8) + sizeof(u8));
			
			bNeedAdjustRes = VidResAdjust( byVmpStyle, byPos, byMtStandardFormat, byReqRes);
		}
		
	}

	if( byStart == 0 )
	{
		byNewFormat = byMtStandardFormat;
	}
	else
	{
		byNewFormat = byReqRes;
	}

//	if( bNeedAdjustRes) //无条件调Res，理由同changeMtVidFormat()中
	
	CServMsg cMsg;
	cMsg.SetEventId( MCU_MT_VIDEOPARAMCHANGE_CMD );
	cMsg.SetMsgBody( &byChnnlType, sizeof(u8) );
	cMsg.CatMsgBody( &byNewFormat, sizeof(u8) );
	SendMsgToMt( byMtId, cMsg.GetEventId(), cMsg );
	
	cServMsg.SetMsgBody();
	cServMsg.SetMsgBody( (u8 *)&tMt, sizeof(TMt) );
	cServMsg.CatMsgBody( &byStart, sizeof(u8) );
	if( byPos < MAXNUM_MPUSVMP_MEMBER)
	{
		cServMsg.CatMsgBody( &byPos, sizeof(u8) );
	}
	SendReplyBack( cServMsg, MCU_MCU_ADJMTRES_ACK );

	CallLog("[ProcMcuMcuAdjustMtResReq] byNewFormat: %u, byStart: %u, byPos: %u \n", byNewFormat, byStart, byPos);

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
=============================================================================*/
void CMcuVcInst::ProcMcuMcuAdjustMtResAck( const CMessage * pcMsg)
{	
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
	STATECHECK;	

	u8  byPos = ~0;
	
	TMt tMt = *(TMt *)cServMsg.GetMsgBody();	//被调好的MT
	u8  byStart = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt));
	if( cServMsg.GetMsgBodyLen() > sizeof(TMt) + sizeof(u8) )
	{
		byPos = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TMt) + sizeof(u8));
	}

	CallLog("[ProcMcuMcuAdjustMtResAck] Adjust Mt(%u,%u) res success!---bStart:%u \n",tMt.GetMcuId(), tMt.GetMtId(), byStart);

	if(byStart)
	{
		BOOL32 bSetChnnl = FALSE;
		u8 byVmpMemberType = VMP_MEMBERTYPE_VAC;
		if(byPos < MAXNUM_MPUSVMP_MEMBER)			//建立到VMP某通道的交换
		{
			
			TVMPParam tVmpParam = m_tConf.m_tStatus.GetVmpParam();
			if( tVmpParam.IsVMPAuto() )				//自动画面合成不校验成员
			{
				byVmpMemberType = VMP_MEMBERTYPE_SPEAKER;	//下级的Mt只能作为发言人
				bSetChnnl = TRUE;
			}
			else
			{
				TVMPMember tVmpMember = *tVmpParam.GetVmpMember(byPos);
				if ( tVmpMember.GetMtId() == tMt.GetMcuId()	)//对于定制合成，保护一下，以防ack收到之前合成参数有变
					
				{
					byVmpMemberType = tVmpMember.GetMemberType();
					bSetChnnl = TRUE;
				}
			}
			if( bSetChnnl)
			{
				SetVmpChnnl(tMt, byPos, byVmpMemberType);

				if(tVmpParam.IsVMPAuto())
				{
					//仅对于自动画面合成，更新tPeriEqpStatus，mcs要依据此刷MT图标
					TPeriEqpStatus tPeriEqpStatus; 
					g_cMcuVcApp.GetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
					tPeriEqpStatus.m_tStatus.tVmp.m_tVMPParam = m_tConf.m_tStatus.m_tVMPParam;
					g_cMcuVcApp.SetPeriEqpStatus( m_tVmpEqp.GetEqpId(), &tPeriEqpStatus );
					CServMsg cMsg;
					cMsg.SetMsgBody( ( u8 * )&tPeriEqpStatus, sizeof( tPeriEqpStatus ) );
					SendMsgToAllMcs( MCU_MCS_MCUPERIEQPSTATUS_NOTIF, cMsg );
				}
			}
			else
			{
				ConfLog(FALSE,"[ProcMcuMcuAdjustMtResAck] not set in vmp chnnl for certain reason!!\n");
			}
			
		}
		else
		{
			//其他预留操作
		}
	}
	else	//恢复分辨率
	{
		//Do nothing
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
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
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
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
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
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
    TMcu *ptMcu = (TMcu *)cServMsg.GetMsgBody();
	TMt *ptMt  = (TMt*)(cServMsg.GetMsgBody()+sizeof(TMcu));
	u8 byMtNum = (cServMsg.GetMsgBodyLen()-sizeof(TMcu))/sizeof(TMt);
	
    //zbq[11/29/2007] 替换逻辑支持
    BOOL32 bReplace = FALSE;
    if ( cServMsg.GetMsgBodyLen() > sizeof(TMcu)+sizeof(TMt)*byMtNum)
    {
        u8 byReplace = *(u8*)(cServMsg.GetMsgBody() + sizeof(TMcu)+sizeof(TMt)*byMtNum);
        bReplace = byReplace == 1 ? TRUE : FALSE;
    }
	STATECHECK
		
    //  [7/19/2006] 用于上级指定发言人时，下级没有开启混音，需要先开启混音
    // guzh [11/7/2007] 
    if (m_tConf.m_tStatus.IsNoMixing())
    {     
        //将上级mcu加入混音列表
        byMtNum++;
        
        CServMsg cMsg;
        cMsg.SetEventId(MCU_MCU_STARTMIXER_CMD);
        cMsg.SetMsgBody((u8*)ptMcu, sizeof(TMcu));        
        cMsg.CatMsgBody(&byMtNum, sizeof(byMtNum));
        cMsg.CatMsgBody((u8*)&m_tCascadeMMCU, sizeof(TMt));        
        cMsg.CatMsgBody((u8*)ptMt, sizeof(TMt)*(byMtNum-1));
        g_cMcuVcApp.SendMsgToConf(m_byConfIdx, MCU_MCU_STARTMIXER_CMD, cMsg.GetServMsg(), cMsg.GetServMsgLen());
    }
    else
    {
        //zbq [11/26/2007] 级联定制、智能混音无条件刷新下级支持
        //zbq [09/30/2007] 当前在VAC，拒绝上级的级联定制混音请求
        if (m_tConf.m_tStatus.IsVACing())
        {
            //ConfLog(FALSE, "[AddMixerMemberCmd] IsVACMode is true. nack\n");
            //NotifyMcsAlarmInfo( 0, ERR_MCU_CASSPECMIXMUTEXVAC );
            //return;
            
            NotifyMcsAlarmInfo(0, ERR_MCU_MMCUSPECMIX_VAC);
            ConfLog( FALSE, "[AddMixerMemberCmd] Local VAC mode has been canceled due to cas specmixing\n" );
            
            CServMsg cMsg;
            cMsg.SetEventId(MCS_MCU_STOPVAC_REQ);
            MixerVACReq(cMsg);
        }
        


        // zbq [09/30/2007] 以下从R2_GDZF合并过来，并增强了从讨论到定制
        // 混音的伪装功能：可以解决在MCS不把MCU作为发言人自动加入混音的
        // 前提下的级联混音导致的时序问题。若MCS开始支持MCU作为发言人自
        // 动加入混音，本部分修正需要MtAdpLib作同步修正才能支持到.

        // zbq [06/29/2007] 级联混音策略调整: 本级已经开启的混音若不包括
        // 上级MCU，则认为本级的混音不是上级和本级之间的级联混音。需停掉
        // 本级(包括本级以下)的(级联)混音，重新开启本请求下的级联混音。
        
        BOOL32 bMMcuInLocalMix = FALSE;
        BOOL32 bLocalWholeMix = FALSE;

        if ( m_tCascadeMMCU.IsNull() )
        {
            ConfLog( FALSE, "[AddMixerMemberCmd] m_tCascadeMMCU.IsNull, impossible \n" );
            return;
        }

        if ( m_ptMtTable->IsMtInMixing(m_tCascadeMMCU.GetMtId()) )
        {
            bMMcuInLocalMix = TRUE;
        }

        if ( !m_tConf.m_tStatus.IsSpecMixing() )
        {
            bLocalWholeMix = TRUE;
        }
        
        if ( !bMMcuInLocalMix || bLocalWholeMix || bReplace )
        {
            if ( !bMMcuInLocalMix || bReplace )
            {
                // 移除本级原有的所有成员
                for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
                {
                    if (m_tConfAllMtInfo.MtJoinedConf(byMtId) &&
                        m_ptMtTable->IsMtInMixing(byMtId) )            
                    {
                        TMt tMt = m_ptMtTable->GetMt(byMtId);
                        AddRemoveSpecMixMember(&tMt, 1, FALSE, TRUE);
                    }
                }
            }

            // zbq [09/30/2007] 讨论模式，须先整理恢复当前的交换
            if ( bLocalWholeMix )
            {
                for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
                {
                    if (m_tConfAllMtInfo.MtJoinedConf(byMtId))
                    {
                        StopSwitchToPeriEqp(m_tMixEqp.GetEqpId(), 
                                            (MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId/PORTSPAN+byMtId), FALSE, MODE_AUDIO);
                        
                        StopSwitchToSubMt(byMtId, MODE_AUDIO, SWITCH_MODE_BROADCAST, FALSE);                       
                        
                        // 恢复从组播地址接收
                        if (m_tConf.GetConfAttrb().IsSatDCastMode() && m_ptMtTable->IsMtInMixGrp(byMtId))
                        {
                            ChangeSatDConfMtRcvAddr(byMtId, LOGCHL_AUDIO);
                        }
                        m_ptMtTable->SetMtInMixGrp(byMtId, FALSE);
                    }
                }
            }
            
            // 停级联混音
            CServMsg cServMsg;
            cServMsg.SetEventId(MCS_MCU_STOPDISCUSS_REQ);            
            OnStopDiscussToAllSMcu( &cServMsg );
            
            // zbq [07/12/2007] 上级MCU也需要增加
            ptMt[byMtNum] = m_tCascadeMMCU;
            byMtNum ++;
        }

        // zbq [09/30/2007] 如果当前是讨论模式，修正为定制混音
        if ( bLocalWholeMix )
        {
            m_tConf.m_tStatus.SetSpecMixing(TRUE);
            ConfModeChange();
        }

        AddRemoveSpecMixMember( ptMt, byMtNum, TRUE );

        // zbq [09/30/2007] 如果当前是讨论模式, 需整理N模式的交换
        if ( bLocalWholeMix )
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

        //定制混音状态通知
        ConfStatusChange();
    }	
	
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
	TMt *ptMt  = (TMt*)(cServMsg.GetMsgBody()+sizeof(TMcu));
	u8 byMtNum = (cServMsg.GetMsgBodyLen()-sizeof(TMcu))/sizeof(TMt);
	
	STATECHECK
		
	AddRemoveSpecMixMember( ptMt, byMtNum, FALSE );

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

	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(cServMsg.GetSrcMtId());
	if (NULL == ptMcInfo)
	{
		cServMsg.SetSrcMtId(0);
		cServMsg.SetEventId(MCU_MCS_LOCKSMCU_NACK);
		SendReplyBack( cServMsg, cServMsg.GetEventId());
	}
	u8 byLock = *(cServMsg.GetMsgBody()+sizeof(TMcu));

	ptMcInfo->SetIsLocked(ISTRUE(byLock));

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
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	
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
	if (0 != ptMcu->GetMcuId())
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
		for (u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++)
		{
			ptMcInfo = &(m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]);
			if (0 == ptMcInfo->m_byMcuId || 
				(!m_tCascadeMMCU.IsNull() && ptMcInfo->m_byMcuId == m_tCascadeMMCU.GetMtId()))
			{
				continue;
			}
			tMcu.SetMcu(ptMcInfo->m_byMcuId);
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
	
	STATECHECK;	

	if( NULL == m_ptConfOtherMcTable )
	{
		return;
	}

	TMcu *ptMcu = (TMcu *)(cServMsg.GetMsgBody());
	TConfMcInfo *ptMcInfo = NULL;
	if( 0 == ptMcu->GetMcuId() )
	{
		return;
	}
	ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(ptMcu->GetMcuId());
	if( NULL == ptMcInfo )
	{
		return;
	}

	s32 nMtNum = (cServMsg.GetMsgBodyLen()-sizeof(TMcu))/sizeof(TSMcuMtStatus);
	TSMcuMtStatus *ptSMcuMtStatus = (TSMcuMtStatus *)(cServMsg.GetMsgBody()+sizeof(TMcu));
	CServMsg cServBackMsg;
	cServBackMsg.SetNoSrc();
	for( s32 nPos=0; nPos<nMtNum; nPos++ )
	{	
		u8 byMtId  = ptSMcuMtStatus->GetMtId();
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
			
		if( nIndex < MAXNUM_CONF_MT )
		{
			//ptMcInfo->m_atMtStatus[nIndex].SetMt( *((TMt *)ptSMcuMtStatus) );
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

			TMtStatus tMtStatus = ptMcInfo->m_atMtStatus[nIndex].GetMtStatus();
            cServBackMsg.SetMsgBody( (u8*)&tMtStatus, sizeof( TMtStatus ) );
			SendMsgToAllMcs( MCU_MCS_MTSTATUS_NOTIF, cServBackMsg );
		}
		ptSMcuMtStatus++;
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

    MMcuLog("[ProcMcuMcuAutoSwitchReq] tSwitchReq with On.%d, level.%d, time.%d, lid.%d\n",
            tSwitchReq.m_bSwitchOn, tSwitchReq.m_nAutoSwitchLevel, 
            tSwitchReq.m_nSwitchSpaceTime, tSwitchReq.m_nSwitchLayerId );
    
    if ( m_tCascadeMMCU.IsNull() ||
         bySrcMtId != m_tCascadeMMCU.GetMtId() ||
         ( MT_MANU_RADVISION != m_ptMtTable->GetManuId(m_tCascadeMMCU.GetMtId()) &&
           MT_MANU_KDCMCU != m_ptMtTable->GetManuId(m_tCascadeMMCU.GetMtId()) ) )
    {
        ConfLog( FALSE, "[AutoSwitchReq] Roused by Mt.%d, Manu.%d, ignore it\n",
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

        ConfLog( FALSE, "Start auto poll switch.\n" );
        
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
void CMcuVcInst::ProcMcuMcuAutoSwitchTimer( const CMessage *pcMsg )
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
        ConfLog( FALSE, "All Mts has be switched over.\n");
        
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
            MMcuLog("Notify restart auto poll swtich to mmcu, bOn.%d, lelev.%d, time.%d, lid.%d\n" ,
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
        ConfLog( FALSE, "Auto Switched to Setout Mt.%d to poll.\n", byMtId );
        
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
    if ( byMtId > MAXNUM_CONF_MT )
    {
        ConfLog( FALSE, "[OnNtfMtStatusToMMcu] Mt.%d is illeagal, ignore it\n", byMtId );
        return;
    }

	CServMsg cServMsg;
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
			if( (m_tConfAllMtInfo.MtJoinedConf( byMtLoop ) && 
				 byDstMcuId != byMtLoop) ||
		        (VCS_CONF == m_tConf.GetConfSource() && CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode(byMtLoop)))
			{		
				m_ptMtTable->GetMtStatus( byMtLoop, &tLocalMtStatus );
				memcpy( (void*)&tSMcuMtStatus, (void*)&tLocalMtStatus, sizeof(TMt) );
				tSMcuMtStatus.SetIsEnableFECC( tLocalMtStatus.IsEnableFECC() );
				tSMcuMtStatus.SetCurVideo( tLocalMtStatus.GetCurVideo() );
				tSMcuMtStatus.SetCurAudio( tLocalMtStatus.GetCurAudio() );
				tSMcuMtStatus.SetMtBoardType( tLocalMtStatus.GetMtBoardType() );
                tSMcuMtStatus.SetIsMixing( tLocalMtStatus.IsInMixing() );
                tSMcuMtStatus.SetVideoLose( tLocalMtStatus.IsVideoLose() );
				tSMcuMtStatus.SetIsAutoCallMode(CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode(byMtLoop));
				tSMcuMtStatus.SetRecvVideo( tLocalMtStatus.IsReceiveVideo() );
				tSMcuMtStatus.SetRecvAudio( tLocalMtStatus.IsReceiveAudio() );
				tSMcuMtStatus.SetSendVideo( tLocalMtStatus.IsSendVideo() );
				tSMcuMtStatus.SetSendAudio( tLocalMtStatus.IsSendAudio() );

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
			tSMcuMtStatus.SetIsAutoCallMode(CONF_CALLMODE_TIMER == m_ptMtTable->GetCallMode(byMtId));
			tSMcuMtStatus.SetRecvVideo( tLocalMtStatus.IsReceiveVideo() );
			tSMcuMtStatus.SetRecvAudio( tLocalMtStatus.IsReceiveAudio() );
			tSMcuMtStatus.SetSendVideo( tLocalMtStatus.IsSendVideo() );
			tSMcuMtStatus.SetSendAudio( tLocalMtStatus.IsSendAudio() );

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
		SendMsgToMt( byDstMcuId, MCU_MCU_MTSTATUS_NOTIF, cServMsg );
	}

	return;
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
====================================================================*/
void CMcuVcInst::BroadcastToAllMcu( u16 wEvent, CServMsg & cServMsg, u8 byExceptMc )
{
	u8 byMcuId = 0;
	for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
	{
		byMcuId = m_tConfAllMtInfo.m_atOtherMcMtInfo[byLoop].GetMcuId();
		if( byMcuId != 0 )
		{
			SendMsgToMt( byMcuId, wEvent, cServMsg );
		}
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
	tInfo.m_tMt = tMt;
	strncpy((s8 *)(tInfo.m_szMtName), m_ptMtTable->GetMtAliasFromExt(tMt.GetMtId()), sizeof(tInfo.m_szMtName));
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
	tInfo.m_byIsConnected		= GETBBYTE(m_tConfAllMtInfo.MtJoinedConf(tMt.GetMcuId(), tMt.GetMtId()));
	tInfo.m_byIsFECCEnable		= GETBBYTE(m_ptMtTable->GetMtLogicChnnl( byMtId, LOGCHL_H224DATA, &tChannel, TRUE )); 
	
	tInfo.m_tPartVideoInfo.m_nViewCount = 0;
	if(tMt == m_tConf.GetSpeaker())
	{
		tInfo.m_tPartVideoInfo.m_atViewPos[tInfo.m_tPartVideoInfo.m_nViewCount].m_nViewID = m_dwSpeakerViewId;
		tInfo.m_tPartVideoInfo.m_atViewPos[tInfo.m_tPartVideoInfo.m_nViewCount].m_bySubframeIndex = 0;
		tInfo.m_tPartVideoInfo.m_nViewCount++;
	}
	if(m_tConf.GetStatus().GetVmpParam().IsMtInMember(tMt))
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

    MMcuLog("[NotifyMcuNewMt] MtId.%d szMtName.%s \n", tInfo.m_tMt.GetMtId(), tInfo.m_szMtName);

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
	
	cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
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
	
	cServMsg.SetMsgBody((u8 *)&tMt, sizeof(TMt));
	BroadcastToAllMcu( MCU_MCU_DELMT_NOTIF, cServMsg );
}

/*====================================================================
    函数名      : GetConfOtherMcMtInfo
    功能        : 得到其它Mc的终端信息
    算法实现    : 
    引用全局变量: 无
    输入参数说明: 无
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/31    3.0         胡昌威       创建
====================================================================*/
TConfMcInfo *CMcuVcInst::GetConfMcInfo( u8 byMcId )
{
	//查找Mc号
	TConfMcInfo *ptConfOtherMcInfo = NULL;
	
	u8 byLoop = 0;
	while( byLoop < MAXNUM_SUB_MCU )
	{
		if( m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop].m_byMcuId != byMcId )
		{
			byLoop ++;
		}
		else
		{
			//已存在
			ptConfOtherMcInfo = &m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop];
			break;
		}
	}

	return ptConfOtherMcInfo;
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
    函数名      ：GetLocalMtFromSmcuMt
    功能        ：得到下级MCU对应的本级终端
    算法实现    ：
    引用全局变量：
    输入参数说明：TMt tMt 下级终端
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/09/01    3.5         胡昌威        创建
====================================================================*/
TMt  CMcuVcInst::GetLocalMtFromOtherMcuMt( TMt tMt )
{
    u8 byMtId;
    if( tMt.IsLocal() )
    {
        if (TYPE_MCUPERI == tMt.GetType())
        {
            return tMt;
        }
        byMtId = tMt.GetMtId();
    }
    else
    {
        byMtId = tMt.GetMcuId();
    }
    return m_ptMtTable->GetMt(byMtId);
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
	if( m_tConf.GetSpeaker().IsNull() )
	{
		return m_tConf.GetSpeaker();
	}
	else if( m_tConf.GetSpeaker().IsLocal() )
	{
		return m_tConf.GetSpeaker();
	}
	else
	{		
		return m_ptMtTable->GetMt(m_tConf.GetSpeaker().GetMcuId());
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
TMt  CMcuVcInst::GetMcuMediaSrc(u8 byMcuId)
{
	TMt tMt;
	tMt.SetMcuId(LOCAL_MCUID);
	tMt.SetMtId(byMcuId);
	TConfMcInfo *ptInfo = m_ptConfOtherMcTable->GetMcInfo(byMcuId);
	if(ptInfo == NULL)
	{
		return tMt;
	}
	TMt tTmpMt;
	tTmpMt.SetMcuId(byMcuId);
	tTmpMt.SetMtId(0);
	TMcMtStatus *ptStatus = ptInfo->GetMtStatus(tTmpMt);
	if(ptStatus == NULL)
	{
		return tMt;
	}

	s32 nOutputViewId = ptStatus->GetMtVideoInfo().m_nOutputLID;
	s32 nVideoId = ptStatus->GetMtVideoInfo().m_nOutVideoSchemeID;
	if(nOutputViewId == 0)
	{
		nOutputViewId = ptInfo->m_tConfViewInfo.m_atViewInfo[ptInfo->m_tConfViewInfo.m_byDefViewIndex].m_nViewId;
	}

	TCViewInfo *ptViewInfo = NULL;
	for(s32 nIndex=0; nIndex<ptInfo->m_tConfViewInfo.m_byViewCount; nIndex++)
	{
		ptViewInfo = &(ptInfo->m_tConfViewInfo.m_atViewInfo[nIndex]);
		if(ptViewInfo->m_nViewId == nOutputViewId)
		{
			if(ptViewInfo->m_byMtCount > 0)
			{
				tMt = ptViewInfo->m_atMts[0];
			}
			break;
		}
	}
	return tMt;
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
====================================================================*/
void CMcuVcInst::ProcMcuMcuSendMsgReq( const CMessage * pcMsg )
{
	CServMsg	cServMsg( pcMsg->content, pcMsg->length );
	TMt	tMt = *(TMt *)( cServMsg.GetMsgBody() );

	// 如果是下级MCU向上级传递, 则忽略, zgc, 2007-04-07
	TMt tSrcMt = m_ptMtTable->GetMt( cServMsg.GetSrcMtId() );
	if( tSrcMt.GetMtType() == MT_TYPE_SMCU )
	{
		ConfLog( FALSE, "[ProcMcuMcuSendMsgReq]This is unallowed that transmit the short msgs to mmcu from smcu!\n");
		return;
	}

	switch( CurState() )
	{
	case STATE_ONGOING:

		if( tMt.IsNull() )	//发给所有终端 MCU_MCU_SENDMSG_NOTIF
		{
			for(s32 nLoop = 1; nLoop<=MAXNUM_CONF_MT; nLoop++)
			{
				if( m_tConfAllMtInfo.MtJoinedConf(nLoop) && nLoop != cServMsg.GetSrcMtId() )
                {
                    if (m_ptMtTable->GetMtType(nLoop) == MT_TYPE_MT)
                    {
                        SendMsgToMt( nLoop, MCU_MT_SENDMSG_NOTIF, cServMsg );					
                    }
                    else if(m_ptMtTable->GetMtType(nLoop) == MT_TYPE_SMCU)// xsl [9/26/2006] 支持多级短消息广播, zgc [04/03/2007] 不往上级MCU发送
                    {
                        SendMsgToMt( nLoop, MCU_MCU_SENDMSG_NOTIF, cServMsg );
                    }
                }			
			}
		}
		else //发给某一终端 
		{
			if(tMt.GetMtId() != cServMsg.GetSrcMtId())
            {
                if (m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_MT)
                {
                    SendMsgToMt( tMt.GetMtId(), MCU_MT_SENDMSG_NOTIF, cServMsg );
                }
                else if (m_ptMtTable->GetMtType(tMt.GetMtId()) == MT_TYPE_SMCU)// xsl [9/26/2006] 支持多级短消息广播, zgc [04/03/2007] 不往上级MCU发送
                {
                    CServMsg cTmpMsg(pcMsg->content, pcMsg->length);
                    TMt tNullMt;
                    tNullMt.SetNull();
                    cTmpMsg.SetMsgBody((u8*)&tNullMt, sizeof(tNullMt));
                    cTmpMsg.CatMsgBody((cServMsg.GetMsgBody() + sizeof(TMt)), cServMsg.GetMsgBodyLen()-sizeof(TMt));
                    SendMsgToMt( tMt.GetMtId(), MCU_MCU_SENDMSG_NOTIF, cTmpMsg );
                }
            }            
		}
		break;

	default:
		ConfLog( FALSE, "Wrong message %u(%s) received in state %u!\n", 
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
void CMcuVcInst::OnMMcuSetIn(TMt &tMt, u8 byMcsSsnId, u8 bySwitchMode, BOOL32 bPolling)
{
	TSetInParam tSetInParam;
	tSetInParam.m_nViewId = -1145368303;
	tSetInParam.m_bySubFrameIndex = 0;
	tSetInParam.m_tMt = tMt;
	
	CServMsg  cMsg;
	TMcuMcuReq tReq;
	TMcsRegInfo	tMcsReg;
	TMtAlias tMtAlias;
	g_cMcuVcApp.GetMcsRegInfo( byMcsSsnId, &tMcsReg );
	astrncpy(tReq.m_szUserName, tMcsReg.m_achUser, 
		sizeof(tReq.m_szUserName), sizeof(tMcsReg.m_achUser));
	astrncpy(tReq.m_szUserPwd, tMcsReg.m_achPwd, 
		sizeof(tReq.m_szUserPwd), sizeof(tMcsReg.m_achPwd));
	cMsg.SetMsgBody((u8 *)&tReq, sizeof(tReq));					
	cMsg.CatMsgBody((u8 *)&tSetInParam, sizeof(tSetInParam));

	if( m_ptMtTable->GetDialAlias( tMt.GetMcuId(), &tMtAlias ) && 
		mtAliasTypeH320ID == tMtAlias.m_AliasType )
	{
		//可能出现误判，如之前是广播源，广播源不变更的同时进行选看操作，状态被覆盖了
		//通知H320接入交换方式
		cMsg.CatMsgBody((u8 *)&bySwitchMode, sizeof(bySwitchMode));
	}

	SendMsgToMt(tMt.GetMcuId(), MCU_MCU_SETIN_REQ, cMsg);
    
	TConfMcInfo* ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(tMt.GetMcuId());
	if (ptMcInfo != NULL)
	{
		ptMcInfo->m_tLastMMcuViewMt = ptMcInfo->m_tMMcuViewMt;
		ptMcInfo->m_tMMcuViewMt = tMt;
	}

    // guzh [8/31/2006] 判断下级终端在本级发言的情况
    // guzh [9/1/2006]  Polling时不切换发言人，切换发言人会导致轮询停止，后面自然会切换
    TMt tSpeaker = m_tConf.GetSpeaker();
    if ( tSpeaker.GetType() == TYPE_MT &&
         tMt.GetMcuId() == tSpeaker.GetMcuId() &&
         tMt.GetMtId() != tSpeaker.GetMtId() &&
         !bPolling)
    {
        ChangeSpeaker(&tMt);
    }    
	
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
	if(ptMcu->IsLocal())
	{
		return;
	}
	TMt tMcuMt = m_ptMtTable->GetMt( ptMcu->GetMcuId() );
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(ptMcu->GetMcuId());
	if(ptMcInfo == NULL)
	{
		return;
	}
	if( ( 0 == ptMcInfo->m_byMcuId ) || 
		( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->m_byMcuId == m_tCascadeMMCU.GetMtId() ) )
	{
		return;
	}
	
	CServMsg cMsg;
	cMsg.SetServMsg(pcSerMsg->GetServMsg(), pcSerMsg->GetServMsgLen());
	cMsg.SetEventId(MCU_MCU_STARTMIXER_CMD);
	cMsg.SetDstMtId( tMcuMt.GetMtId() );
	
	SendMsgToMt(tMcuMt.GetMtId(), MCU_MCU_STARTMIXER_CMD, cMsg);

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
	if(ptMcu->IsLocal())
	{
		return;
	}
	TMt tMcuMt = m_ptMtTable->GetMt( ptMcu->GetMcuId() );
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(ptMcu->GetMcuId());
	if(ptMcInfo == NULL)
	{
		return;
	}
	if( ( 0 == ptMcInfo->m_byMcuId ) || 
		( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->m_byMcuId == m_tCascadeMMCU.GetMtId() ) )
	{
		return;
	}

	CServMsg cMsg;
	cMsg.SetServMsg(pcSerMsg->GetServMsg(), pcSerMsg->GetServMsgLen());
	cMsg.SetEventId(MCU_MCU_STOPMIXER_CMD);
	cMsg.SetDstMtId( tMcuMt.GetMtId() );
	
	SendMsgToMt(tMcuMt.GetMtId(), MCU_MCU_STOPMIXER_CMD, cMsg);

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
	
	TConfMcInfo* ptMcInfo = NULL;
	for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
	{
		ptMcInfo = &(m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]);
		if( ( 0 == ptMcInfo->m_byMcuId ) || 
			( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->m_byMcuId == m_tCascadeMMCU.GetMtId() ) )
		{
			continue;
		}
		cMsg.SetDstMtId( ptMcInfo->m_byMcuId );
		SendMsgToMt(ptMcInfo->m_byMcuId, MCU_MCU_STARTMIXER_CMD, cMsg);
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
	
	TConfMcInfo* ptMcInfo = NULL;
	for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
	{
		ptMcInfo = &(m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]);
		if( ( 0 == ptMcInfo->m_byMcuId ) || 
			( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->m_byMcuId == m_tCascadeMMCU.GetMtId() ) )
		{
			continue;
		}
		cMsg.SetDstMtId( ptMcInfo->m_byMcuId );
		SendMsgToMt(ptMcInfo->m_byMcuId, MCU_MCU_STOPMIXER_CMD, cMsg);
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
	if(ptMcu->IsLocal())
	{
		return;
	}

	CServMsg cMsg;
	cMsg.SetServMsg(pcSerMsg->GetServMsg(), pcSerMsg->GetServMsgLen());
	cMsg.SetEventId(MCU_MCU_GETMIXERPARAM_REQ);
	
	if( 0 == ptMcu->GetMcuId() )
	{
		TConfMcInfo* ptMcInfo = NULL;
		for( u8 byLoop = 0; byLoop < MAXNUM_SUB_MCU; byLoop++ )
		{
			ptMcInfo = &(m_ptConfOtherMcTable->m_atConfOtherMcInfo[byLoop]);
			if( ( 0 == ptMcInfo->m_byMcuId ) || 
				( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->m_byMcuId == m_tCascadeMMCU.GetMtId() ) )
			{
				continue;
			}
			cMsg.SetDstMtId( ptMcInfo->m_byMcuId );
			SendMsgToMt(ptMcInfo->m_byMcuId, MCU_MCU_GETMIXERPARAM_REQ, cMsg);
		}
	}
	else
	{
		cMsg.SetDstMtId( ptMcu->GetMcuId() );
		SendMsgToMt(ptMcu->GetMcuId(), MCU_MCU_GETMIXERPARAM_REQ, cMsg);
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
void CMcuVcInst::OnAddRemoveMixToSMcu(CServMsg *pcSerMsg, BOOL32 bAdd)
{
	TMcu * ptMcu = (TMcu *)(pcSerMsg->GetMsgBody());
	if(ptMcu->IsLocal())
	{
		return;
	}
	TMt tMcuMt = m_ptMtTable->GetMt( ptMcu->GetMcuId() );
	TConfMcInfo *ptMcInfo = m_ptConfOtherMcTable->GetMcInfo(ptMcu->GetMcuId());
	if(ptMcInfo == NULL)
	{
		return;
	}
	if( ( 0 == ptMcInfo->m_byMcuId ) || 
		( !m_tCascadeMMCU.IsNull() &&  ptMcInfo->m_byMcuId == m_tCascadeMMCU.GetMtId() ) )
	{
		return;
	}

    // xsl [7/27/2006] 若下级mcu没有参加混音，将其加入混音组
    if (!m_ptMtTable->IsMtInMixing(tMcuMt.GetMtId()))
    {
        AddRemoveSpecMixMember(&tMcuMt, 1, TRUE);
    }
	
	CServMsg cMsg;
	cMsg.SetServMsg( pcSerMsg->GetServMsg(), pcSerMsg->GetServMsgLen() );
	cMsg.SetDstMtId( tMcuMt.GetMtId() );
	if( TRUE == bAdd )
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

// END OF FILE
