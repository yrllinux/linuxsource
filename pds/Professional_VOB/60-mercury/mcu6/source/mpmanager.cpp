 /*****************************************************************************
   模块名      : MP
   文件名      : mpmanage.h
   相关文件    : 
   文件实现功能: MP与MCU接口实现
   作者        : 胡昌威
   版本        : V0.1  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/07/10  0.1         胡昌威      创建
   2005/02/19  3.6         万春雷      级联修改、与3.5版本合并
******************************************************************************/

#include "evmcumt.h"
#include "evmcu.h"
#include "evmp.h"
#include "mcuvc.h"
//#include "mpmanager.h"
#include "mtadpssn.h"
#include "mpssn.h"
#include "mcuver.h"	//[pengguofeng 5/10/2012]IPV6 

/*lint -save -esym(666, min, max, kmin, kmax)*/

// MPC2 支持
#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
        #include "brdwrapperdef.h"
        #include "nipwrapper.h"
        //#include "nipwrapperdef.h"
    #else
        #include "boardwrapper.h"
    #endif
#else
    #include "brddrvlib.h"
#endif

CMpManager *pcMpManager = NULL;

CMpManager::CMpManager()
{
	//初始化
	pcMpManager = this;
}

CMpManager::~CMpManager()
{

}

/*====================================================================
    函数名      ：showmp
    功能        ：屏幕打印所有Mp信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/17    1.0         胡昌威        创建
====================================================================*/
API void showmp(void)
{
	pcMpManager->ShowMp();

	return;
}

/*====================================================================
    函数名      ：showmtadp
    功能        ：屏幕打印所有MtAdp信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/06/01    3.0         胡昌威        创建
====================================================================*/
API void showmtadp(void)
{
	pcMpManager->ShowMtAdp();

	return;
}

/*====================================================================
    函数名      : showbridge
    功能        ：屏幕打印交换桥
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/17    1.0         胡昌威        创建
====================================================================*/
API void showbridge(void)
{
	pcMpManager->ShowBridge();

	return;
}

/*====================================================================
    函数名      ：ssw
    功能        ：屏幕打印交换信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/17    1.0         胡昌威        创建
====================================================================*/	
API void ssw( u8 byType )
{
	pcMpManager->ShowSwitch( byType );

	return;
}

API void sswi(const s8* pchDstIp, u16 wDstPort, u8 byConfIdx, BOOL32 bDetail)
{
	BOOL32 bPrintUsage = FALSE;
	do 
	{
		if (0 == byConfIdx && 0 == wDstPort && NULL == pchDstIp)
		{
			bPrintUsage = TRUE;
			break;
		}

		if (0 == byConfIdx || MAXNUM_MCU_CONF < byConfIdx || 0 == wDstPort || NULL == pchDstIp)
		{
			StaticLog("Invlid argument! See the usage below:\n");
			bPrintUsage = TRUE;
			break;
		}
		
		//check if the user don't enwrap the DstIp with quotation marks
		u32 dwPValue = (u32)pchDstIp;
		if ((dwPValue & (~0x000000FF)) == 0)
		{
			StaticLog("the DstIp should be enwrapped with \"\"\n");
			bPrintUsage = TRUE;
			break;
		}

		if (strlen(pchDstIp) > 15 || strlen(pchDstIp) < 7)
		{
			StaticLog( "Invlid Ip! See the usage below:\n");
			bPrintUsage = TRUE;
			break;
		}
		if (0 == INET_ADDR(pchDstIp))
		{
			StaticLog( "Invlid Ip! See the usage below:\n");
			bPrintUsage = TRUE;
			break;
		}

	} while (0);

	if (bPrintUsage)
	{
		s8 achFormat[]	= "usage: sswi \"DstIp\" DstPort confIdx";
		s8 achExample[] = "Example: sswi \"172.16.230.202\" 60040 3";
		s8 achNote[]	= "Note that the DstIp must be enwrapped with \"\" ";
		u8 byLen = max( (strlen(achFormat)), (strlen(achNote)) );
		byLen = max(byLen, strlen(achExample));
		
		u8 byLoop =0;
		StaticLog( "+");
		for(; byLoop< ( byLen + 4 ); byLoop ++)
		{
			StaticLog( "-");
		}
		StaticLog( "+\n");
		
		StaticLog( "|  %-*s  |\n", byLen,achFormat);
		StaticLog( "|  %-*s  |\n", byLen, achExample); 
		StaticLog( "|  %-*s  |\n", byLen, achNote); 
		
		StaticLog( "+");
		for(byLoop =0; byLoop< byLen + 4; byLoop ++)
		{
			StaticLog( "-");
		}
		StaticLog( "+\n");
		
	}
	else
	{
		if(pchDstIp != NULL)
		{
			u32 dwDstIp = ntohl(INET_ADDR(pchDstIp));
			if (!pcMpManager->ShowSwitchRouteToDst(byConfIdx, dwDstIp, wDstPort, bDetail))
			{
				StaticLog( "no match info found!\n");
			}
		}
	}

	return;
	
}


/*====================================================================
    函数名      ：StartMulticast
    功能        ：开始Mt组播
    算法实现    ：
    引用全局变量：
    输入参数说明：tSrc 源终端
	              bySrcChnnl 通道号
				  byMode 交换模式,缺省为MODE_BOTH
    返回值说明  ：TRUE，如果不能开始广播返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/08/20    2.0         胡昌威        创建
====================================================================*/
BOOL32 CMpManager::StartMulticast( const TMt & tSrc, u16 wSrcChnnl, u8 byMode, BOOL32 bConf )
{
	u32  dwSrcIp = 0;
	u32  dwRcvIp = 0;
	u16  wRcvPort = 0;
	BOOL32 bResult1 = TRUE;
	BOOL32 bResult2 = TRUE;
	u32  dwMultiCastAddr;
	u16  dwMultiCastPort;
	 
	//得到交换源地址
	if( !GetSwitchInfo(tSrc, dwRcvIp, wRcvPort, dwSrcIp))
	{
		 LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartMulticast() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//得到MCU的组播地址
    //dwMultiCastAddr = g_cMcuAgent.GetCastIpAddr();
    //dwMultiCastPort = g_cMcuAgent.GetCastPort();
    // 顾振华@2006.4.6 这里要从会议里面取
    dwMultiCastAddr = g_cMcuVcApp.AssignMulticastIp(tSrc.GetConfIdx());
	if (bConf)
	{
		dwMultiCastPort = g_cMcuVcApp.AssignMulticastPort(tSrc.GetConfIdx(), 0);
	}
	else
	{
		dwMultiCastPort = g_cMcuVcApp.AssignMulticastPort(tSrc.GetConfIdx(), tSrc.GetMtId());
	}
	
	//multispy port
	if( wSrcChnnl >= CASCADE_SPY_STARTPORT && wSrcChnnl < MT_MCU_STARTPORT ) //用多回传的通道
	{
		wRcvPort = wSrcChnnl;
	}
	else
	{
		//根据信道号计算偏移量
		wRcvPort = wRcvPort + PORTSPAN * wSrcChnnl;
	}

	//根据信道号计算偏移量	
	//wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;

    //multicasting
    u8 byIsMulticast;
    u8 byMulticastFlag = 0;
    u8 byFirstMpId = 0;
    for(u16 wMpId = 0; wMpId < MAXNUM_DRI + 1; wMpId++)
    {
        if (g_cMcuVcApp.IsMpConnected(wMpId+1) || (MAXNUM_DRI == wMpId))
        {

            if (MAXNUM_DRI > wMpId)
            {
                if (0 == byFirstMpId)
                {
                    byFirstMpId = wMpId+1;
                }

                byIsMulticast = g_cMcuVcApp.GetMpMulticast(wMpId+1);
                LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "\n[StartMulticast]byIsMulticast = %d  wMpId+1 = %d\n\n", byIsMulticast, wMpId+1);
                if (1 != byIsMulticast)
                {
                    continue;
                }

                byMulticastFlag++;

                dwRcvIp = g_cMcuVcApp.GetMpIpAddr(wMpId+1);
                LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "\n[StartMulticast]dwRcvIp = 0x%x\n\n", dwRcvIp);
            }
            else
            {
                if (0 < byMulticastFlag)
                {
                    continue;
                }
                dwRcvIp = g_cMcuVcApp.GetMpIpAddr(byFirstMpId);
            }

            //图像
            if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
            {
                //建立交换
                bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort,
                                       dwMultiCastAddr, dwMultiCastPort, dwRcvIp, dwRcvIp);
                if (!bResult1)
                {
                    LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Switch video failure!\n");
                }
            }

            //语音
            if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
            {
                //建立交换
                bResult2 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, (wRcvPort+2),
                                       dwMultiCastAddr, (dwMultiCastPort+2), dwRcvIp, dwRcvIp);
                if (!bResult2)
                {
                    LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Switch audio failure!\n");
                }
            }
            
            //第二路视频
            if (byMode == MODE_SECVIDEO)
            {
                bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, (wRcvPort+4),
                                       dwMultiCastAddr, (dwMultiCastPort+4), dwRcvIp, dwRcvIp);
                if (!bResult1)
                {
                    LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Switch snd video failure!\n");
                }
            }
        }
    }

	return  bResult1 && bResult2;
}

/*====================================================================
    函数名      ：StopMulticast
    功能        ：停止Mt组播
    算法实现    ：
    引用全局变量：
    输入参数说明：tSrc 源终端
	              bySrcChnnl 通道号
				  byMode 交换模式,缺省为MODE_BOTH
    返回值说明  ：TRUE，如果不能开始广播返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/08/20    2.0         胡昌威        创建
====================================================================*/
BOOL32 CMpManager::StopMulticast(const TMt & tSrc, u8 bySrcChnnl, u8 byMode, BOOL32 bConf)
{
	u32  dwSrcIp  = 0;
	u32  dwRcvIp  = 0;
	u16  wRcvPort = 0;

	u32  dwMultiCastAddr;
	u16  dwMultiCastPort;
		
	//得到交换源地址
	if (!GetSwitchInfo(tSrc, dwRcvIp, wRcvPort, dwSrcIp))
	{
		 LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StopMulticast() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//得到MCU的组播地址
    // dwMultiCastAddr = g_cMcuAgent.GetCastIpAddr();
    // 顾振华@2006.4.6 这里要从会议里面取
    dwMultiCastAddr = g_cMcuVcApp.AssignMulticastIp(tSrc.GetConfIdx());    
    // dwMultiCastPort = g_cMcuAgent.GetCastPort();
	if (bConf)
	{
		dwMultiCastPort = g_cMcuVcApp.AssignMulticastPort(tSrc.GetConfIdx(), 0);
	}
	else
	{
		dwMultiCastPort = g_cMcuVcApp.AssignMulticastPort(tSrc.GetConfIdx(), tSrc.GetMtId());
	}

	//根据信道号计算偏移量	
	wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;

	//图像
	if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
	{
		//移除交换
		StopSwitch(tSrc.GetConfIdx(), dwMultiCastAddr, dwMultiCastPort);
	}
		
	//语音
	if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
	{
		//移除交换
		StopSwitch(tSrc.GetConfIdx(), dwMultiCastAddr, (dwMultiCastPort+2));
	}
    
    //第二路视频
    if (byMode == MODE_SECVIDEO)
    {
        StopSwitch(tSrc.GetConfIdx(), dwMultiCastAddr, (dwMultiCastPort+4));
    }

	return TRUE;
}


/*====================================================================
    函数名      ：StartDistrConfCast
    功能        ：开始分散会议组播
    算法实现    ：
    引用全局变量：
    输入参数说明：tSrc 源终端
	              bySrcChnnl 通道号
				  byMode 交换模式,缺省为MODE_BOTH
    返回值说明  ：TRUE，如果不能开始广播返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/04/13    4.0         顾振华        创建
====================================================================*/
BOOL32 CMpManager::StartDistrConfCast( const TMt &tSrc, u8 byMode, u8 bySrcChnnl)
{
	u32  dwSrcIp = 0;
	u32  dwRcvIp = 0;
	u16  wRcvPort = 0;
	BOOL32 bResult1 = TRUE;
	BOOL32 bResult2 = TRUE;
	u32  dwMultiCastAddr;
	u16  wMultiCastPort;
	 
	//得到交换源地址
	if( !GetSwitchInfo(tSrc, dwRcvIp, wRcvPort, dwSrcIp))
	{
		 LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartDistrConfCast() failure because of can't get switch info!\n");
		 return FALSE ;
	}

    CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(tSrc.GetConfIdx());
    if ( NULL == pVcInst )
    {
        // Fatal error!
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Cannot get VC Instance(conf idx: %d) while StartDistrConfCast!\n", 
                            tSrc.GetConfIdx());
        return FALSE;
    }
	
	//  [3/2/2012 pengguofeng]取会议终端
	TConfMtTable *ptConfMtTab = g_cMcuVcApp.GetConfMtTable(tSrc.GetConfIdx());
	if ( ptConfMtTab == NULL)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "[StartDistrConfCast]Get conf:%d mt table failed\n", tSrc.GetConfIdx());
		return FALSE;
	}

	//  [3/2/2012 pengguofeng]取会议信息
	TConfAllMtInfo *ptConfAllMtInfo = g_cMcuVcApp.GetConfAllMtInfo(tSrc.GetConfIdx());
	if ( ptConfMtTab == NULL)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "[StartDistrConfCast]Get conf:%d mt info failed\n", tSrc.GetConfIdx());
		return FALSE;
	}

	TLogicalChannel tLogCnnl;

// 	TMtStatus tStatus;
// 	TMt tSelMt;
	//  [3/2/2012 pengguofeng]设置音视频源（目前只有视频源设置正确才会正确的请求关键帧）
	/*if ( tSrc.GetType() == TYPE_MT && (byMode == MODE_AUDIO || byMode == MODE_VIDEO) )
	{
		for ( u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
		{
			//  [3/2/2012 pengguofeng]入会+视频接收通道打开
			if ( ptConfAllMtInfo->MtJoinedConf(byMtId)
				&& !(ptConfMtTab->GetMt(byMtId) == tSrc)
				&& ptConfMtTab->GetMtLogicChnnl(byMtId, byMode, &tLogCnnl, TRUE))
			{
				ptConfMtTab->SetMtSrc(byMtId, &tSrc, byMode);
			}
		}
	}*/

    dwMultiCastAddr = pVcInst->m_tConf.GetConfAttrb().GetSatDCastIp();
    wMultiCastPort = pVcInst->m_tConf.GetConfAttrb().GetSatDCastPort();
	
	// xsl [8/19/2006] 卫星分散会议混音器N模式交换到组播地址
	//tianzhiyong  2010/03/21 支持EMIXER
    /*if (tSrc.GetType() == TYPE_MCUPERI && tSrc.GetEqpType() == EQP_TYPE_MIXER )
    {
        //wRcvPort = wRcvPort + MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId;    //这里不考虑多混音组的情况，有需求时再考虑
    }
    else //根据信道号计算偏移量*/
    {
        wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;
    }

    u8 byIsMulticast;
    u8 byMulticastFlag = 0;
    u8 byFirstMpId = 0;
	u32 dwSndBindIp = 0; // 组播发送地址 [pengguofeng 6/6/2013]
#if defined(_8KH_) || defined(_8KI_)
	if ( g_cMcuAgent.Is8000HmMcu() )
	{
		// 8000H-M需要设置dwSndBindIP，涉及到组播地址 [pengguofeng 6/5/2013]
		dwSndBindIp = GetSwitchSndBindIp(); //通用接口，对于8000H-M特殊返回，其他均返回0
	}
#endif
    for(u16 wMpId = 0; wMpId < MAXNUM_DRI + 1; wMpId++)
    {
        if (g_cMcuVcApp.IsMpConnected(wMpId+1) || (MAXNUM_DRI == wMpId))
        {

            if (MAXNUM_DRI > wMpId)
            {
                if (0 == byFirstMpId)
                {
                    byFirstMpId = wMpId+1;
                }

                byIsMulticast = g_cMcuVcApp.GetMpMulticast(wMpId+1);
                LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "\n[StartDistrConfCast()]byIsMulticast = %d  wMpId+1 = %d\n\n", byIsMulticast, wMpId+1);
                if (1 != byIsMulticast)
                {
                    continue;
                }

                byMulticastFlag++;

                dwRcvIp = g_cMcuVcApp.GetMpIpAddr(wMpId+1);
                LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "\n[StartDistrConfCast()]dwRcvIp = 0x%x\n\n", dwRcvIp);
            }
            else
            {
                if (0 < byMulticastFlag)
                {
                    continue;
                }
                dwRcvIp = g_cMcuVcApp.GetMpIpAddr(byFirstMpId);
            }

			// 8000H－M取出来里是实现地址 [pengguofeng 6/6/2013]
			if ( dwSndBindIp == 0 )
			{
				dwSndBindIp = dwRcvIp;
			}
			
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[StartMulticast]dwSndBindIp:%x\n", dwSndBindIp);
            //图像
            if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
            {
                //建立交换
                LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StartDistrConfCast() Switch video\n");
                bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort,
                                       dwMultiCastAddr, wMultiCastPort, dwRcvIp, dwSndBindIp,
                                       SWITCHCHANNEL_UNIFORMMODE_NONE,
                                       INVALID_PAYLOAD,
                                       // 建立RTCP交换
                                       1);
                if (!bResult1)
                {
                    LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartDistrConfCast() Switch video failure!\n");
                }
            }

            //语音
            if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
            {
                //建立交换
                LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StartDistrConfCast() Switch audio\n");
                bResult2 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, (wRcvPort+2),
                                       dwMultiCastAddr, (wMultiCastPort+2), dwRcvIp, dwSndBindIp,
                                       SWITCHCHANNEL_UNIFORMMODE_NONE,
                                       INVALID_PAYLOAD,
                                       // 建立RTCP交换
                                       1);
                if (!bResult2)
                {
                    LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartDistrConfCast() Switch audio failure!\n");
                }
            }
            
            //第二路视频
            if (byMode == MODE_SECVIDEO)
            {
                LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StartDistrConfCast() Switch snd video\n");
                bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, (wRcvPort+4),
                                       dwMultiCastAddr, (wMultiCastPort+4), dwRcvIp, dwSndBindIp,
                                       SWITCHCHANNEL_UNIFORMMODE_NONE,
                                       INVALID_PAYLOAD,
                                       // 建立RTCP交换
                                       1);
                if (!bResult1)
                {
                    LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartDistrConfCast() Switch snd video failure!\n");
                }
            }
        }
    }

	return  bResult1 && bResult2;
}

/*====================================================================
    函数名      ：StopDistrConfCast
    功能        ：停止分散会议组播
    算法实现    ：
    引用全局变量：
    输入参数说明：tSrc 源终端
	              bySrcChnnl 通道号
				  byMode 交换模式,缺省为MODE_BOTH
    返回值说明  ：TRUE，如果不能开始广播返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/04/13    4.0         顾振华        创建
====================================================================*/
BOOL32 CMpManager::StopDistrConfCast(const TMt &tSrc, u8 byMode, u8 bySrcChnnl)
{
    u32  dwSrcIp  = 0;
	u32  dwRcvIp  = 0;
	u16  wRcvPort = 0;

	u32  dwMultiCastAddr;
	u16  wMultiCastPort;
		
	//得到交换源地址
	if (!GetSwitchInfo(tSrc, dwRcvIp, wRcvPort, dwSrcIp))
	{
		 LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StopDistrConfCast() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//得到MCU的组播地址
    CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(tSrc.GetConfIdx());
    if ( NULL == pVcInst )
    {
        // Fatal error!
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Cannot get VC Instance(conf idx: %d) while StopDistrConfCast!\n", 
                            tSrc.GetConfIdx());
        return FALSE;
    }

    dwMultiCastAddr = pVcInst->m_tConf.GetConfAttrb().GetSatDCastIp();
    wMultiCastPort = pVcInst->m_tConf.GetConfAttrb().GetSatDCastPort();


	//根据信道号计算偏移量	
	wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;

	//图像
	if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
	{
		//移除交换
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StopDistrConfCast() Switch video\n");
		StopSwitch(tSrc.GetConfIdx(), dwMultiCastAddr, wMultiCastPort, 1);
	}
		
	//语音
	if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
	{
		//移除交换
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StopDistrConfCast() Switch audio\n");
		StopSwitch(tSrc.GetConfIdx(), dwMultiCastAddr, (wMultiCastPort+2), 1);
	}
    
    //第二路视频
    if (byMode == MODE_SECVIDEO)
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StopDistrConfCast() Switch snd video\n");
        StopSwitch(tSrc.GetConfIdx(), dwMultiCastAddr, (wMultiCastPort+4), 1);
    }

	return TRUE;
}


/*====================================================================
    函数名      ：GetSwitchInfo
    功能        ：得到交换信息
    算法实现    ：
    引用全局变量：
    输入参数说明：tSrc 源终端
	              dwSwitchIpAddr 交换板IP地址
				  wSwitchPort 交换端口
    返回值说明  ：TRUE，如果不能得到返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/16    1.0         胡昌威        创建
	10/02/25				xl			  modify method for eqp
====================================================================*/
BOOL32 CMpManager::GetSwitchInfo(const TMt & tSrc, u32 &dwSwitchIpAddr, u16 &wSwitchPort, u32 &dwSrcIpAddr)
{
	// [11/29/2011 liuxu] 如果源为空, 则可直接返回
	if (tSrc.IsNull())
	{
		dwSwitchIpAddr = 0;
		wSwitchPort = 0;
		dwSrcIpAddr = 0;
		return FALSE;
	}

	u32  dwSrcIp  = 0;
	u32  dwRcvIp  = 0;
	u16  wRcvPort = 0;

	//u16  byMcuId  = tSrc.GetMcuId();
	u8   byMtId   = tSrc.GetMtId();
	u8   byEqpId  = tSrc.GetEqpId();
	u8   byPeriType;

    BOOL32 bHDVmp = FALSE;

	//得到交换源的地址与端口
	switch(tSrc.GetType()) 
	{

	case TYPE_MT://终端
		{
			CMcuVcInst * pcVcInst = g_cMcuVcApp.GetConfInstHandle(tSrc.GetConfIdx());
			if(NULL == pcVcInst)
			{
				LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "[GetSwitchInfo]pcVcInst(ConfIdx.%d) is null!\n", tSrc.GetConfIdx());
				return FALSE;
			}
			TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable(tSrc.GetConfIdx());
			if (ptConfMtTable == NULL)
			{
				LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "GetSwitchInfo() failure, Type: %d SubType: %d Id: %d ConfIdx: %d!\n", 
					   tSrc.GetType(), tSrc.GetEqpType(), tSrc.GetEqpId(),tSrc.GetConfIdx());
				return FALSE;
			}
			TMt tLocalMt;
			if( !tSrc.IsLocal() )
			{
				tLocalMt = ptConfMtTable->GetMt(pcVcInst->GetFstMcuIdFromMcuIdx(tSrc.GetMcuId()));
				byMtId = tLocalMt.GetMtId();
			}

			//GetSubMtSwitchAddr(tSrc,dwRcvIp,wRcvPort);
			ptConfMtTable->GetMtSwitchAddr(byMtId, dwRcvIp, wRcvPort);
			dwSrcIp = ptConfMtTable->GetIPAddr(byMtId);
		}
		break;

	case TYPE_MCUPERI://外设
		{
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
			if( tSrc.GetEqpType() == EQP_TYPE_TVWALL)
			{
				dwRcvIp = g_cMcuVcApp.GetAnyValidMp();
				g_cMcuAgent.GetPeriInfo(byEqpId, &dwSrcIp, &byPeriType);
				dwSrcIp = ntohl(dwSrcIp);
			}
			else
			{	
				u16 wEqpStartPort = 0;
				GetSwitchInfo( byEqpId, dwRcvIp, wRcvPort, wEqpStartPort);

				//[2011/10/12/zhangli]hud和recorder都要从配置文件读eqpip
				if (tSrc.GetEqpType() == EQP_TYPE_RECORDER)
				{
					g_cMcuAgent.GetPeriInfo(byEqpId, &dwSrcIp, &byPeriType);
					dwSrcIp = ntohl(dwSrcIp);
				}
				else
				{
					dwSrcIp = g_cMcuVcApp.GetEqpIpAddr(byEqpId);
				}
			}

#else
			switch(tSrc.GetEqpType()) 
			{
			case EQP_TYPE_MIXER://混音器
				GetMixerSwitchAddr(byEqpId,dwRcvIp,wRcvPort);
				break;
			case EQP_TYPE_RECORDER://新录像机
				GetRecorderSwitchAddr(byEqpId,dwRcvIp,wRcvPort);
				break;
			case EQP_TYPE_BAS://码率适配器
				GetBasSwitchAddr(byEqpId,dwRcvIp,wRcvPort);
				break;
			case EQP_TYPE_VMP://画面合成器
				GetVmpSwitchAddr(byEqpId,dwRcvIp,wRcvPort);
				break;
			case EQP_TYPE_PRS://丢包重传器
				GetPrsSwitchAddr(byEqpId,dwRcvIp,wRcvPort);
				break;
			case EQP_TYPE_TVWALL://电视墙
				dwRcvIp = g_cMcuVcApp.GetAnyValidMp();
				break;
			default:
				break;
			}
			g_cMcuAgent.GetPeriInfo(byEqpId, &dwSrcIp, &byPeriType);
			dwSrcIp = ntohl(dwSrcIp);

#endif
			break;
		}
	default:
		break;
	}

	//判断交换源的地址与端口是否合法
	if (dwRcvIp == 0 || wRcvPort == 0)
	{
        if (!(tSrc.GetType() == TYPE_MCUPERI  && tSrc.GetEqpType() == EQP_TYPE_TVWALL))
        {
		    LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "GetSwitchInfo() failure, Type: %d SubType: %d Id: %d, dwRcvIp: %d wRcvPort: %d!\n", 
			       tSrc.GetType(), tSrc.GetEqpType(), tSrc.GetEqpId(), dwRcvIp, wRcvPort);
		    return FALSE;
        }
	}

	//判断交换源的地址与端口是否合法
	if (dwSrcIp == 0)
	{
        if ( tSrc.GetType() == TYPE_MCUPERI )
        {
            if (bHDVmp)
            {
                dwSrcIp = ntohl(g_cMcuVcApp.GetEqpIp(byEqpId));

                dwSrcIpAddr    = dwSrcIp;
                dwSwitchIpAddr = dwRcvIp;
	            wSwitchPort    = wRcvPort;

                return TRUE;
            }
		    LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "GetSwitchInfo() cann't get switch src IP, dwSrcIp: 0, byEqpId: %d!\n", byEqpId);
        }
        else
            LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "GetSwitchInfo() cann't get switch src IP, dwSrcIp: 0, byMtId: %d!\n", byMtId);
		return FALSE;
	}

	dwSrcIpAddr    = dwSrcIp;
	dwSwitchIpAddr = dwRcvIp;
	wSwitchPort    = wRcvPort;

	return TRUE;
}

/*=============================================================================
函 数 名： GetSwitchInfo
功    能： 此函数专供8000e 使用
算法实现： 
全局变量： 
参    数：  u8 byEqpId
			u32 &dwSwitchIpAddr      MP交换板地址(主机序)
			u16 &wMcuStartPort       MCU接收端口号(主机序)
			u16 &wEqpStartPort       外设接收端口号(主机序)
									 返回参数均为主机序
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/2/16   4.0		周广程                  创建
=============================================================================*/
BOOL32 CMpManager::GetSwitchInfo( u8 byEqpId, u32 &dwSwitchIpAddr, u16 &wMcuStartPort, u16 &wEqpStartPort )
{
    CApp * pcApp = (CApp *)&g_cMpSsnApp;
    CMpSsnInst * pcMpSsnInst = NULL;
    u16 wInstId = 1;
    for ( wInstId = 1; wInstId <= MAXNUM_DRI; wInstId++ )
    {
        pcMpSsnInst = (CMpSsnInst *)pcApp->GetInstance(wInstId);
        if ( pcMpSsnInst->GetMpIp() != 0 )
        {
            dwSwitchIpAddr = pcMpSsnInst->GetMpIp();    //找到第一个为止
			break;
        }
    }
    if ( wInstId > MAXNUM_DRI )
    {
        dwSwitchIpAddr = 0;
        wMcuStartPort = 0;
        wEqpStartPort = 0;
		
        LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[GetSwitchInfo] byEqpId.%d failed for no mp found\n", byEqpId );
        return FALSE;
    }
	
    if( byEqpId >= MIXERID_MIN && byEqpId <= MIXERID_MAX  )
    {
		TEqpMixerInfo tTempMixerInfo;
		if (SUCCESS_AGENT != g_cMcuAgent.GetEqpMixerCfg( byEqpId , &tTempMixerInfo ))
		{
			LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR,"[GetSwitchInfo] GetEqpMixerCfg(%d) failed!\n", byEqpId);
			return FALSE;	
		}
		wEqpStartPort = tTempMixerInfo.GetEqpRecvPort();
		wMcuStartPort = tTempMixerInfo.GetMcuRecvPort();
		LogPrint( LOG_LVL_DETAIL, MID_MCU_MPMGR,"[GetSwitchInfo]m_byEqpId:%d MixMem:%dGetSwitchInfo--wMcuStartPort: %d, wEqpStartPort: %d\n", 
			byEqpId,tTempMixerInfo.GetMaxChnInGrp(),wMcuStartPort, wEqpStartPort);
        return TRUE;
    }
    
    if( byEqpId >= BASID_MIN && byEqpId <= BASID_MAX )
    {
        wMcuStartPort = BAS_MCU_STARTPORT + (byEqpId-BASID_MIN)*BAS_MCU_PORTSPAN;
		wEqpStartPort = BAS_EQP_STARTPORT + (byEqpId-BASID_MIN) * PORTSPAN;
/*#if	defined(_8KE_)
		wEqpStartPort = BAS_EQP_STARTPORT + (byEqpId-BASID_MIN)*BAS_8KE_PORTSPAN;
#elif defined(_8KH_)
		wEqpStartPort = BAS_EQP_STARTPORT + (byEqpId-BASID_MIN)*BAS_8KH_PORTSPAN;
#else
        wEqpStartPort = BAS_EQP_STARTPORT +  (byEqpId-BASID_MIN)*BAS_8KE_PORTSPAN;//(byEqpId-BASID_MIN)*BAS_MCU_PORTSPAN; 
#endif*/
        return TRUE;
    }
    
    if( byEqpId >= VMPID_MIN && byEqpId <= VMPID_MAX )
    {
        wMcuStartPort = VMP_MCU_STARTPORT + (byEqpId-VMPID_MIN)*VMP_MCU_PORTSPAN;
        wEqpStartPort = VMP_EQP_STARTPORT; 
        return TRUE;
    }
    
    if( byEqpId >= PRSID_MIN && byEqpId <= PRSID_MAX )
    {
        wMcuStartPort = PRS_MCU_STARTPORT + (byEqpId-PRSID_MIN)*PRS_MCU_PORTSPAN;
        wEqpStartPort = PRS_EQP_STARTPORT; 
        return TRUE;
    }
    
    if ( byEqpId >= RECORDERID_MIN && byEqpId <= RECORDERID_MAX )
    {
        wMcuStartPort = REC_MCU_STARTPORT + (byEqpId-RECORDERID_MIN)*REC_MCU_PORTSPAN;
        wEqpStartPort = REC_EQP_STARTPORT; 
        return TRUE;
    }
	
    wMcuStartPort = 0;
    wEqpStartPort = 0;
    LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[GetSwitchInfo] byEqpId.%d failed\n", byEqpId );
    return FALSE;
}

/*====================================================================
    函数名      ：SetSwitchBridge
    功能        ：设置交换桥
    算法实现    ：
    引用全局变量：
    输入参数说明：tSrc 源终端
	              bySrcChnnl 通道号
				  byMode 交换模式,缺省为MODE_BOTH
    返回值说明  ：TRUE，如果不能开始广播返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/16    1.0         胡昌威        创建
====================================================================*/
BOOL32 CMpManager::SetSwitchBridge(const TMt & tSrc, u8 bySrcChnnl, u8 byMode, BOOL32 bSrcHDBas,u16 wSpyStartPort)
{
	u32  dwSrcIp = 0;
	u32  dwRcvIp = 0;
	u16  wRcvPort = 0;

	TMt tSrcMt = tSrc;

	//得到交换源地址
	if (!GetSwitchInfo(tSrcMt, dwRcvIp, wRcvPort, dwSrcIp))
	{
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "SetSwitchBridge() failure because of can't get switch info!\n");
		return FALSE;
	}

	//根据IP地址查找交换MP编号
	u8 byMpId = g_cMcuVcApp.FindMp(dwRcvIp);
	 
    wRcvPort = wRcvPort +PORTSPAN*bySrcChnnl;
	LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "Get Mt<%d> Ip:%x AssignMpId:%d Mp IP:%x Port:%d Mode:%d\n",
		tSrcMt.GetMtId(), dwSrcIp, byMpId, dwRcvIp, wRcvPort, byMode);

	//zjj20100201
	//  [11/26/2009 pengjie] Modify 级联多回传支持
	if(wSpyStartPort != SPY_CHANNL_NULL)
	{
		wRcvPort = wSpyStartPort;
	}
	// End

	//建立交换桥
	u32 dwDstIp = 0;
	BOOL32 bResult = TRUE;
//	BOOL32 bResultAnd = TRUE;
    dwRcvIp = 0;
    u8  byFirstMpId = 0;
	u8 byTmpLastMpId = 0; // 记录上一个在线的MP ID [pengguofeng 5/23/2012]
	u8 byLastMpId = 0;
    for(s32  nMpId = 0; nMpId < MAXNUM_DRI; nMpId++)
	{	   
	   if (g_cMcuVcApp.IsMpConnected(nMpId+1))
	   {
            //第一个mp 或下一个为接入终端mp
            if ( 0 == dwRcvIp || byMpId == nMpId + 1 )
            {
                if ( 0 == dwRcvIp )
                {
                    byFirstMpId = nMpId + 1;
                }
               dwRcvIp = g_cMcuVcApp.GetMpIpAddr(nMpId+1);
				byTmpLastMpId = nMpId + 1;
               continue;
            }            

            //下一个mp                   
            dwDstIp = g_cMcuVcApp.GetMpIpAddr(nMpId+1);

			if ( byMpId != nMpId + 1
				&& byTmpLastMpId != byFirstMpId )
			{
				dwSrcIp = g_cMcuVcApp.GetMpIpAddr( byTmpLastMpId);
			}

			LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "set bridge from %x[which to send] to %x, last src:%x\n",
				dwRcvIp, dwDstIp, dwSrcIp);
			bResult = bResult && StartBridgeSwitch(byMode, tSrcMt, dwSrcIp, dwRcvIp, wRcvPort, dwDstIp, bSrcHDBas);

			//bResultAnd = bResultAnd && bResult;
			dwSrcIp = dwRcvIp;

           //本次mp的目的地址作为下一个mp的接收地址
            dwRcvIp = dwDstIp;
	   } 
	}

    //若源不是第一个mp，需要建立尾部到头部的桥交换，最终形成链形桥交换
    if ( byMpId > 1 && byMpId != byFirstMpId && g_cMcuVcApp.IsMpConnected(byFirstMpId) )
    {
        dwDstIp = g_cMcuVcApp.GetMpIpAddr(byFirstMpId);

		LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "set bridge from %x[which to send] to %x, last src:%x\n",
			dwRcvIp, dwDstIp, dwSrcIp);

       bResult = StartBridgeSwitch(byMode, tSrcMt, dwSrcIp, dwRcvIp, wRcvPort, dwDstIp, bSrcHDBas);

//		bResultAnd = bResultAnd && bResult;
    }

//    return bResultAnd; 
	return bResult;
}

/*====================================================================
    函数名      ：RemoveSwitchBridge
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：tSrc 交换源
	              bySrcChnnl 源通道号
				  byMode 交换模式,缺省为MODE_BOTH
    返回值说明  ：TRUE，如果不能停止广播返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/16    1.0         胡昌威        创建
====================================================================*/
BOOL32 CMpManager::RemoveSwitchBridge(const TMt & tSrc,u8 bySrcChnnl,u8 byMode, BOOL32 bEqpHDBas,u16 wSpyStartPort )
{
	u32  dwSrcIp = 0;
	u32  dwRcvIp = 0;
	u16  wRcvPort = 0;
 
	//得到交换源地址
	if( !GetSwitchInfo( tSrc, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "RemoveSwitchBridge() failure because of can't get switch info!");
		return FALSE;
	}

	//根据IP地址查找交换MP编号
	u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	
	wRcvPort = wRcvPort + PORTSPAN*bySrcChnnl;
	//zjj20100201
	//  [11/26/2009 pengjie] Modify 级联多回传支持
	if(wSpyStartPort != SPY_CHANNL_NULL)
	{
		wRcvPort = wSpyStartPort;
	}
	// End
  
	//拆除交换桥
	u32 dwDstIp;
    for(s32  nMpId = 0; nMpId < MAXNUM_DRI; nMpId++)
	{	   
	   if( g_cMcuVcApp.IsMpConnected( nMpId+1 ) && (byMpId != nMpId + 1))
	   {
		    dwDstIp = g_cMcuVcApp.GetMpIpAddr(nMpId+1);
			switch(byMode)
			{
			case MODE_VIDEO:
			    StopSwitch(tSrc.GetConfIdx(), dwDstIp,wRcvPort);
				break;
			case MODE_AUDIO:
				StopSwitch(tSrc.GetConfIdx(), dwDstIp,wRcvPort+2);
				break;
			case MODE_BOTH:
				StopSwitch(tSrc.GetConfIdx(), dwDstIp,wRcvPort);
                StopSwitch(tSrc.GetConfIdx(), dwDstIp,wRcvPort+2);
				break;
			case MODE_SECVIDEO:
                if (bEqpHDBas)
                {
                    StopSwitch(tSrc.GetConfIdx(), dwDstIp,wRcvPort);
                }
                else
                {
			        StopSwitch(tSrc.GetConfIdx(), dwDstIp,wRcvPort + 4);
                }
				break;
			default:
				break;
			}
	   } 
	}

    return TRUE; 
}

/*=============================================================================
函 数 名： StartBridgeSwitch
功    能： 
算法实现： 
全局变量： 
参    数： u8 byMode
           TMt tSrcMt
           u32 dwSrcIp
           u32 dwRcvIp
           u16 wRcvPort
           u32 dwDstIp
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/9/14  4.0			许世林                  创建
=============================================================================*/
BOOL32 CMpManager::StartBridgeSwitch(u8		byMode, 
									 TMt	tSrcMt,
									 u32	dwSrcIp,
									 u32	dwRcvIp,
									 u16	wRcvPort,
									 u32	dwDstIp, BOOL32 bSrcHDBas)
{
    BOOL32 bResult;
    switch(byMode)
    {
    case MODE_VIDEO:
        bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort, dwDstIp, wRcvPort, 0, 0, SWITCHCHANNEL_UNIFORMMODE_NONE);
        break;
    case MODE_AUDIO:
        bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort+2, dwDstIp, wRcvPort+2, 0, 0, SWITCHCHANNEL_UNIFORMMODE_NONE);
        break;
    case MODE_BOTH:
        bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort, dwDstIp, wRcvPort, 0, 0, SWITCHCHANNEL_UNIFORMMODE_NONE);
        bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort+2, dwDstIp, wRcvPort+2, 0, 0, SWITCHCHANNEL_UNIFORMMODE_NONE);
        break;
	case MODE_SECVIDEO:
		//zbq[09/05/2008] 高清适配的双流桥交换不作＋4处理
		if (bSrcHDBas)
		{
			bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort, dwDstIp, wRcvPort, 0, 0, SWITCHCHANNEL_UNIFORMMODE_NONE);
		}
		else
		{
			bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort+4, dwDstIp, wRcvPort+4, 0, 0, SWITCHCHANNEL_UNIFORMMODE_NONE);
		}
		break;
    default:
		bResult = FALSE;
        break;
    }

    return bResult;
}


/*====================================================================
    函数名      ：StartSwitchToDst
    功能        ：将指定源数据交换到目的
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：TRUE，如果不交换直接返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    10/08/18    4.6         薛亮	        创建
====================================================================*/
BOOL32 CMpManager::StartSwitchToDst(TSwitchDstInfo &tSwitchDstInfo, u16 wSpyStartPort /* = SPY_CHANNL_NULL */)
{
	
	LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StartSwitchToDst()--param is: mode: %u, srcChannel: %u, VidIp: 0x%x, VidPort: %d\n\tAudIp:0x%x, AudPort:%d, srcmt(%d,%d)\n", 
		tSwitchDstInfo.m_byMode, 
		tSwitchDstInfo.m_bySrcChnnl, 
		tSwitchDstInfo.m_tDstVidAddr.GetIpAddr(),
		tSwitchDstInfo.m_tDstVidAddr.GetPort(),
		tSwitchDstInfo.m_tDstAudAddr.GetIpAddr(),
		tSwitchDstInfo.m_tDstAudAddr.GetPort(),
		tSwitchDstInfo.m_tSrcMt.GetMcuId(),
		tSwitchDstInfo.m_tSrcMt.GetMtId()
		);

	TMt tSrc = tSwitchDstInfo.m_tSrcMt;
	u8	byMode = tSwitchDstInfo.m_byMode;
	u8  bySrcChnnl = tSwitchDstInfo.m_bySrcChnnl;

	u32 dwSrcIp = 0;
	u32	dwRcvIp = 0;
	u16	wRcvPort = 0;

	BOOL32 bResult1 = TRUE;
	BOOL32 bResult2 = TRUE;
	
	// [9/30/2010 xliang] local conversion if tSrc is TYPE_MT
	if( tSrc.GetType() == TYPE_MT )
	{
		TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable(tSrc.GetConfIdx());
		if (ptConfMtTable == NULL)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchToDst() failure, Type: %d SubType: %d Id: %d ConfIdx: %d!\n", 
				tSrc.GetType(), tSrc.GetEqpType(), tSrc.GetEqpId(),tSrc.GetConfIdx());
			return FALSE;
		}
		
		CMcuVcInst *pcIns = g_cMcuVcApp.GetConfInstHandle(tSrc.GetConfIdx()) ;
		if( pcIns == NULL  || !pcIns->m_tConf.m_tStatus.IsOngoing())
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchToDst() failure for finding invalid VcInst, Type: %d SubType: %d Id: %d ConfIdx: %d!\n", 
				tSrc.GetType(), tSrc.GetEqpType(), tSrc.GetEqpId(),tSrc.GetConfIdx());
			return FALSE;
		}
		
		
		if( !tSrc.IsLocal() )
		{
			u8 byMcuId = pcIns->GetFstMcuIdFromMcuIdx(tSrc.GetMcuId());
			tSrc = ptConfMtTable->GetMt(byMcuId);
			LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[StartSwitchToDst] tSrc change to local is (%d,%d)\n", tSrc.GetMcuId(), tSrc.GetMtId());
		}
		
	}
	
	//得到交换源地址
	if( !GetSwitchInfo( tSrc, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchToDst() failure because of can't get switch info!");
		return FALSE ;
	}
	
	//根据信道号计算偏移量	
	wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;
	
	//级联多回传支持
	if( wSpyStartPort != SPY_CHANNL_NULL )
	{
		wRcvPort = wSpyStartPort;
	}

	//目的地址
	u32 dwDstIp		= 0;//tSwitchDstInfo.m_tDstVidAddr.GetIpAddr();
	u16 wVidDstPort	= tSwitchDstInfo.m_tDstVidAddr.GetPort();
	u16 wAudDstPort = tSwitchDstInfo.m_tDstAudAddr.GetPort();

	//图像
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		dwDstIp		= tSwitchDstInfo.m_tDstVidAddr.GetIpAddr();
		bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort, dwDstIp, wVidDstPort );		
	}
	
	//语音
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		dwDstIp		= tSwitchDstInfo.m_tDstAudAddr.GetIpAddr();
		bResult2 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, (wRcvPort+2), dwDstIp, wAudDstPort );
	}
	
    return bResult1&&bResult2; 
	
}

/*====================================================================
    函数名      ：StopSwitchToDst
    功能        ：停止交换到目的
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    10/08/18    4.6         薛亮	        创建
====================================================================*/
void CMpManager::StopSwitchToDst(TSwitchDstInfo &tSwitchDstInfo)
{
	u8 byMode = tSwitchDstInfo.m_byMode;
	TMt tSrcMt = tSwitchDstInfo.m_tSrcMt;
	u8 byConfIdx = tSrcMt.GetConfIdx();

	TTransportAddr tAddr = ( MODE_VIDEO == byMode)? tSwitchDstInfo.m_tDstVidAddr: tSwitchDstInfo.m_tDstAudAddr;
	u32 dwDstIp = tAddr.GetIpAddr();
	u16 wVidDstPort = tSwitchDstInfo.m_tDstVidAddr.GetPort();
	u16 wAudDstPort = tSwitchDstInfo.m_tDstAudAddr.GetPort();
	
	//图像	
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		StopSwitch( byConfIdx, dwDstIp, wVidDstPort );
	}
	
	//语音
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		StopSwitch( byConfIdx, dwDstIp, wAudDstPort );
	}
}


/*====================================================================
    函数名      ：StartSwitchToMc
    功能        ：将指定终端数据交换到会控
    算法实现    ：
    引用全局变量：
    输入参数说明：tSrc, 源
				  bySrcChnnl, 源的信道号
				  wMcInstId, 会议控制台实例号
				  byDstChnnl, 目的信道索引号，缺省为0
   				  byMode, 交换模式，缺省为MODE_BOTH
   返回值说明  ：TRUE，如果不交换直接返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/18    1.0         LI Yi         创建
	03/07/15    1.0         胡昌威        从业务移入
====================================================================*/
BOOL32 CMpManager::StartSwitchToMc( const TMt & tSrc, u8 bySrcChnnl, u16 wMcInstId, u8 byDstChnnl, u8 byMode, u16 wSpyPort )
{
	u32  dwSrcIp = 0;
	u32	dwRcvIp;
	u16	wRcvPort;
	u8  byChannelNum;
	TLogicalChannel tLogicalChannel;
	BOOL32 bResult1 = TRUE;
	BOOL32 bResult2 = TRUE;
	BOOL32 bResult3 = TRUE;

	//得到交换源地址
	if( !GetSwitchInfo( tSrc, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchToMc(%d,%d) get switch info fail!\n", tSrc.GetMcuId(), tSrc.GetMtId());
		return FALSE ;
	}

	//根据信道号计算偏移量	
	wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;

	//  [11/27/2009 pengjie] Modify 级联多回传支持
	if( wSpyPort != SPY_CHANNL_NULL )
	{
		wRcvPort = wSpyPort;
	}
	// End

    //图像
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		if( g_cMcuVcApp.GetMcLogicChnnl( wMcInstId, MODE_VIDEO, &byChannelNum, &tLogicalChannel ) && 
			( byChannelNum > byDstChnnl ) )
		{
			bResult1 = StartSwitch( tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort, tLogicalChannel, byDstChnnl );			
		}
		else
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR,  "Cannot get video send address to Meeting Console%u!\n", wMcInstId );
		}

	}
	
	//语音
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		if( g_cMcuVcApp.GetMcLogicChnnl( wMcInstId, MODE_AUDIO, &byChannelNum, &tLogicalChannel ) && 
			( byChannelNum > byDstChnnl ) )
		{
			bResult2 = StartSwitch( tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, (wRcvPort+2), tLogicalChannel, byDstChnnl );			
		}
		else
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "Cannot get audio send address to Meeting Console%u!\n", wMcInstId );
		}
	}
	
	//双流
	if ( byMode == MODE_SECVIDEO )
	{
		if( g_cMcuVcApp.GetMcLogicChnnl( wMcInstId, MODE_SECVIDEO, &byChannelNum, &tLogicalChannel ) && 
			( byChannelNum > byDstChnnl ) )
		{
			bResult3 = StartSwitch( tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, (wRcvPort+4), tLogicalChannel, byDstChnnl );			
		}
		else
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "Cannot get SecVideo send address to Meeting Console%u!\n", wMcInstId );
		}
	}
    return bResult1&&bResult2&&bResult3; 
}

/*====================================================================
    函数名      ：StopSwitchToMc
    功能        ：停止将数据交换到会控
    算法实现    ：
    引用全局变量：
    输入参数说明：wMcInstId,  实例号
				  byDstChnnl, 信道索引号，缺省为0
				  byMode, 交换模式，缺省为MODE_BOTH
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/18    1.0         LI Yi         创建
	03/07/15    1.0         胡昌威        从业务移入
====================================================================*/
void CMpManager::StopSwitchToMc( u8 byConfIdx, u16 wMcInstId,  u8 byDstChnnl, u8 byMode )
{
    u8  byChannelNum;
	TLogicalChannel tLogicalChannel;
	
	//图像	
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		if( g_cMcuVcApp.GetMcLogicChnnl( wMcInstId, MODE_VIDEO, &byChannelNum, &tLogicalChannel ) 
			&& ( byChannelNum > byDstChnnl ) )
		{
			StopSwitch( byConfIdx, tLogicalChannel, byDstChnnl );
		}
	}
	
	//语音
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		if( g_cMcuVcApp.GetMcLogicChnnl( wMcInstId, MODE_AUDIO, &byChannelNum, &tLogicalChannel ) 
			&& ( byChannelNum > byDstChnnl ) )
		{
			StopSwitch( byConfIdx, tLogicalChannel, byDstChnnl );
		}
	}

	//双流
	if( byMode == MODE_SECVIDEO )
	{
		if( g_cMcuVcApp.GetMcLogicChnnl( wMcInstId, MODE_SECVIDEO, &byChannelNum, &tLogicalChannel ) 
			&& ( byChannelNum > byDstChnnl ) )
		{
			StopSwitch( byConfIdx, tLogicalChannel, byDstChnnl );
		}
	}
}


/*====================================================================
    函数名      ：StartSwitchToBrd
    功能        ：将指定终端的视频作为广播源
    算法实现    ：
    引用全局变量：
    输入参数说明：tSrc, 源
				  bySrcChnnl,  源的信道号
				  u8 bySpecMpId[IN] 发给具体的转发板ID。值为0表示发给所有转发板，值非0表示发给具体的转发板
    返回值说明  ：TRUE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/03/21    4.0         顾振华        创建
	12/04/12	4.7         chendaiei	  modify
====================================================================*/
BOOL32 CMpManager::StartSwitchToBrd( const TMt &tSrc, u8 bySrcChnnl, BOOL32 bForce, u16 wSpyStartPort,u8 bySpecMpId)
{
	u32  dwSrcIp = 0;
	u32  dwRcvIp = 0;
	u16  wRcvPort = 0;
	BOOL32 bResult = TRUE;

	//得到交换源地址
	if( !GetSwitchInfo( tSrc, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
		 LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchToBrd() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//根据信道号计算偏移量
	wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl; 

	//  [11/26/2009 pengjie] Modify 级联多回传支持
	if( wSpyStartPort != SPY_CHANNL_NULL)
	{
		wRcvPort = wSpyStartPort;
	}
	// End

    //建立交换

    //根据IP地址查找交换MP编号
    u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
    if( 0 == byMpId )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "No MP's Addrs is 0x%x in StartSwitchToBrd(),please check switch recv IP!\n",dwRcvIp);
        return FALSE;
    }
    
    //构造交换信道 
    TSwitchChannel tSwitchChannel;
    tSwitchChannel.SetSrcMt( tSrc );
    tSwitchChannel.SetSrcIp( dwSrcIp );
    tSwitchChannel.SetRcvIP( dwRcvIp );
    tSwitchChannel.SetRcvPort( wRcvPort );
    
    // 发增加广播源消息给所有MP
    CServMsg cServMsg;
    cServMsg.SetConfIdx(tSrc.GetConfIdx());
//     cServMsg.SetMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));
//     u8 byForce = bForce ? 1 : 0;
//     cServMsg.CatMsgBody(&byForce, sizeof(byForce));
    
	if( bySpecMpId != 0 )
	{
	    cServMsg.SetMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));
	    u8 byForce = bForce ? 1 : 0;
	    cServMsg.CatMsgBody(&byForce, sizeof(byForce));
		g_cMpSsnApp.SendMsgToMpSsn( bySpecMpId, 
			MCU_MP_ADDBRDSRCSWITCH_REQ, 
			cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "Send MCU_MP_ADDBRDSRCSWITCH_REQ from RecvMp %s:%d to Mp%d.\n",
                 StrOfIP(tSwitchChannel.GetRcvIP()), tSwitchChannel.GetRcvPort(),bySpecMpId);
	}
    else
	{
	// for loop [pengguofeng 5/16/2012]
// 		g_cMpSsnApp.BroadcastToAllMpSsn(MCU_MP_ADDBRDSRCSWITCH_REQ, 
//                                     cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		TSwitchChannel tTmpChannel;
		for ( u8 byMpIdLoop = 1; byMpIdLoop <= MAXNUM_DRI; byMpIdLoop++)
		{
			if ( g_cMcuVcApp.IsMpConnected(byMpIdLoop) )
			{
				tTmpChannel = tSwitchChannel;
				if ( byMpIdLoop != byMpId)
				{
					// 设置源的IP，防止MP侧判错 [pengguofeng 5/16/2012]
					tTmpChannel.SetSrcIp(0);
				}
			    cServMsg.SetMsgBody((u8 *)&tTmpChannel, sizeof(tTmpChannel));				
			    u8 byForce = bForce ? 1 : 0;
			    cServMsg.CatMsgBody(&byForce, sizeof(byForce));
				g_cMpSsnApp.SendMsgToMpSsn( byMpIdLoop, 
					MCU_MP_ADDBRDSRCSWITCH_REQ, 
					cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "Send MCU_MP_ADDBRDSRCSWITCH_REQ from RecvMp %s:%d srcIp:%x to Mp%d.\n",
			        StrOfIP(tSwitchChannel.GetRcvIP()), tSwitchChannel.GetRcvPort(), tTmpChannel.GetSrcIp(), byMpIdLoop);
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
    07/03/21    4.0         顾振华        创建
====================================================================*/
BOOL32 CMpManager::StartSwitchFromBrd(const TMt &tSrc, u8 bySrcChnnl, u8 byDstMtNum, const TMt* const ptDstBase, u16 wSpyStartPort)
{
    CMcuVcInst * pcVcInst = g_cMcuVcApp.GetConfInstHandle(tSrc.GetConfIdx());
	if (!pcVcInst)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchFromBrd Get pcVcint failed!\n");
        return FALSE;
	}

    TConfInfo  * ptConfInfo = &pcVcInst->m_tConf;
    TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable( tSrc.GetConfIdx() );
    u8 bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_NONE;
    u8 byUniformPayload = INVALID_PAYLOAD;
    u8 byIsSwitchRtcp = 0;
    u32 dwSrcMpIp = 0;
    u32 dwSrcIp = 0;
    u32 dwMpIP = 0;
    u32 dwDisIp = 0;
    u32 dwRcvIp = 0;
    u8 byManuID = 0;    
    u16 wRcvPort = 0;
    TLogicalChannel tLogicChannel;

    if( ptConfMtTable == NULL || pcVcInst == NULL || ptConfInfo == NULL)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchFromBrd() param err. ptMtTable.0x%x, ptVcInst.0x%x!\n", ptConfMtTable, pcVcInst);
        return FALSE;
    }

    //得到交换源地址
    if( !GetSwitchInfo( tSrc, dwSrcMpIp, wRcvPort, dwSrcIp ) )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchFromBrd() failure because of can't get switch info!\n");
        return FALSE ;
	}
    //根据信道号计算偏移量
    wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;

	//  [11/26/2009 pengjie] Modify 级联多互传支持
	if( wSpyStartPort != SPY_CHANNL_NULL )
	{
		wRcvPort = wSpyStartPort;
	}
	// End
	u16 wMMcuIdx = INVALID_MCUIDX;
	if( !pcVcInst->m_tCascadeMMCU.IsNull() )
	{
		u8 byId = pcVcInst->m_tCascadeMMCU.GetMtId();
		pcVcInst->m_tConfAllMcuInfo.GetIdxByMcuId( &byId,1,&wMMcuIdx );
	}

    CServMsg cServMsg;
    cServMsg.SetConfIdx(tSrc.GetConfIdx());    
    TSwitchChannelExt tSwitchChannel;

	// liuxu, 获取它的mc table
	TConfOtherMcTable * pConfOtherMc = pcVcInst->GetConfOtherMc();
	BOOL32 bSrcIsMMcu = FALSE;
	
	if (NULL != pConfOtherMc)
	{
		if (NULL != pConfOtherMc->GetMcInfo(wMMcuIdx)
			&& pConfOtherMc->GetMcInfo(wMMcuIdx)->m_tSpyMt == tSrc)
		{
			bSrcIsMMcu = TRUE;
		}			
	}

    for (u8 byMpLoop = 1; byMpLoop <= MAXNUM_DRI; byMpLoop ++)
    {
        if ( !g_cMcuVcApp.IsMpConnected(byMpLoop) )
        {
            continue;
        }

        cServMsg.SetDstDriId( byMpLoop );
        cServMsg.SetMsgBody();
        
        dwMpIP = g_cMcuVcApp.GetMpIpAddr( byMpLoop );

        u8 byMpId = 0;
		
        for ( u8 byMtLoop = 0; byMtLoop < byDstMtNum; byMtLoop ++ )
        {
			if( ptDstBase[byMtLoop].IsNull() )
			{
				continue;
			}

			// 通知卫星接收地址 [pengguofeng 2/21/2013]
			TMt tDstMt = ptDstBase[byMtLoop];
			if ( pcVcInst->IsMultiCastMt(tDstMt.GetMtId()) )
			{
				pcVcInst->ChangeSatDConfMtRcvAddr(tDstMt.GetMtId(), LOGCHL_VIDEO, TRUE);
				continue;
			}			

            // 每次处理当前MP接入的终端
            byMpId = ptConfMtTable->GetMpId( ptDstBase[byMtLoop].GetMtId() );
            if ( byMpId != byMpLoop )
            {
                continue;
            }

            // zbq [06/29/2007] 回传源的广播交换不能替换其普通回传交换
            if (!pcVcInst->m_tCascadeMMCU.IsNull()  &&
				bSrcIsMMcu &&
                pcVcInst->m_tCascadeMMCU == ptDstBase[byMtLoop] )
            {
                LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MPMGR,  "[StartSwitchFromBrd] tSrc.%d create no brd switch to MMcu.%d\n", 
                               tSrc.GetMtId(), pcVcInst->m_tCascadeMMCU.GetMtId() );
                continue;
            }

            byManuID = ptConfMtTable->GetManuId(ptDstBase[byMtLoop].GetMtId());     
            ptConfMtTable->GetMtLogicChnnl(ptDstBase[byMtLoop].GetMtId(), LOGCHL_VIDEO, &tLogicChannel, TRUE);
            
			TConfAttrb tConfAtrrb = ptConfInfo->GetConfAttrb();
            if (( TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_VMP == tSrc.GetEqpType() && MT_TYPE_MT != ptDstBase[byMtLoop].GetMtType() &&
                tConfAtrrb.IsResendLosePack()) ||
                tSrc == ptDstBase[byMtLoop]) //  xsl [1/17/2006] (外厂商mt或mcu)将视频码流交换给自己时不进行rtcp交换
            {
                byIsSwitchRtcp = 0;
            }
            else
            {
                byIsSwitchRtcp = 1;
            }
            
            if (byManuID == MT_MANU_KDC || byManuID == MT_MANU_KDCMCU)
            {
                bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_NONE;
            }
            else
            {
                bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_VALID;
                if (TYPE_MT == tSrc.GetType() && TYPE_MT == ptDstBase[byMtLoop].GetType())
                {
                    if (tLogicChannel.GetChannelType() >= AP_MIN && tLogicChannel.GetChannelType() <= AP_MAX)
                    {
                        byUniformPayload = tLogicChannel.GetChannelType();
                    }
                }
            }
            
            //得到目的Mt的交换信息                    
            if( dwMpIP != dwSrcMpIp )//需要转发
            {
                dwDisIp = dwSrcMpIp;
            }
            else
            {
                dwDisIp = 0;                
            }                        
            dwRcvIp = dwMpIP;
            //建立交换
            
            //构造交换信道             
            tSwitchChannel.Clear();
            tSwitchChannel.SetSrcMt( tSrc );
            tSwitchChannel.SetSrcIp( dwSrcIp );
            tSwitchChannel.SetRcvIP( dwRcvIp );
            tSwitchChannel.SetRcvPort( wRcvPort );
            tSwitchChannel.SetDisIp( dwDisIp );
            tSwitchChannel.SetRcvBindIP( dwRcvIp );
            tSwitchChannel.SetDstIP( tLogicChannel.m_tRcvMediaChannel.GetIpAddr() );			
        	tSwitchChannel.SetDstPort( tLogicChannel.m_tRcvMediaChannel.GetPort() );
			//根据配置文件读取是否要作假的ip/port，看dstip和port是否是要作假的
			if ( g_cMcuVcApp.IsNeedMapDstIpAddr( htonl(tSwitchChannel.GetDstIP()) ) )
			{
				//做假的ip为目的终端所挂转发板ip，端口为后向通道的端口
				u32 dwMappedIp = 0;
				u16 wMappedPort = 0 ;
				u32 dwDstSrcIp = 0;
				GetSwitchInfo( ptDstBase[byMtLoop],dwMappedIp,wMappedPort,dwDstSrcIp);
				if ( dwMappedIp == 0 || wMappedPort == 0 )//获取失败，则用固定的ip和端口
				{
					StaticLog("[StartSwitchFromBrd]Get dstmt(mtid:%d) switchip and port failed\n",ptDstBase[byMtLoop].GetMtId());
					//设置为recvip和port
					dwMappedIp = dwRcvIp;
					wMappedPort = DEFAULT_MAPPORT;

				}
				//成功则用取出来的ip和端口
				tSwitchChannel.SetMapIp( dwMappedIp );
				tSwitchChannel.SetMapPort(wMappedPort);//视频的端口不需要+2之类的
				
				
			}
            
            // 准备给当前MP的消息体
            cServMsg.CatMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));                  
            cServMsg.CatMsgBody((u8 *)&bySwitchChannelMode, sizeof(u8));
            cServMsg.CatMsgBody((u8 *)&byUniformPayload, sizeof(u8));            
            cServMsg.CatMsgBody((u8 *)&byIsSwitchRtcp, sizeof(u8));

            LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "Send MCU_MP_ADDBRDDSTSWITCH_REQ for Mt.%d(%s:%d) to Mp.%d\n",
                ptDstBase[byMtLoop].GetMtId(),
                StrOfIP(tSwitchChannel.GetDstIP()), tSwitchChannel.GetDstPort(), byMpLoop);
            LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "dwSrcIp:%s dwRcvIP:%s,dwDisIP:%s\n",StrOfIP(dwSrcIp),StrOfIP(dwRcvIp),StrOfIP(dwDisIp));
        }
        if ( cServMsg.GetMsgBodyLen() > 0 )
        {
            g_cMpSsnApp.SendMsgToMpSsn( byMpLoop, 
                                        MCU_MP_ADDBRDDSTSWITCH_REQ, 
                                        cServMsg.GetServMsg(), 
                                        cServMsg.GetServMsgLen());
			LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, " Send MCU_MP_ADDBRDDSTSWITCH_REQ to Mp.%d !!!!!!!!!!!!!!!!! \n", byMpLoop );
        }
    }

    return TRUE;
}

/*====================================================================
    函数名      ：StartSwitchEqpFromBrd
    功能        ：将广播源的码流交换给外设
    算法实现    ：目前广播源到外设的交换所采用的转发板为广播源所在的转发板，
	              而非为外设所分配的mcu侧码流接收转发板，因为后者在转发板掉
				  线时mcu将自动重新分配
    引用全局变量：
    输入参数说明：const TMt &tSrc:交换源
				  u8 bySrcChnnl:  源通道
				  u8 byEqpId:     目的外设ID
				  u16 wDstChnnl:  目的外设通道号
    返回值说明  ：TRUE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/08/07    4.0         张宝卿        创建
	10/02/25	4.6			薛亮		  整合8000E
====================================================================*/
BOOL32 CMpManager::StartSwitchEqpFromBrd( const TMt &tSrc, u16 bySrcChnnl, u8 byEqpId, u16 wDstChnnl)
{
    u8  byChannelNum = 0;
    u32 dwSrcIp = 0;
    u32 dwRcvIp = 0;
    u16 wRcvPort = 0;
    u32 dwDstIp = 0;
    u16 wDstPort = 0;
    TLogicalChannel	tLogicalChannel;

    //得到交换源地址
    if (!GetSwitchInfo(tSrc, dwRcvIp, wRcvPort, dwSrcIp))
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchEqpFromBrd() failure because of can't get switch info!");
        return FALSE;
    }
	u8 byDstMpId = 0;
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
    u8 byEqpType = g_cMcuAgent.GetPeriEqpType(byEqpId);

    //广播交换方式
    u32  dwMpIP = 0;
    u16  wMpPort = 0;

    //得到目的Mt的交换信息
    switch (byEqpType)
    {
    case EQP_TYPE_BAS://码率适配器
        GetBasSwitchAddr(byEqpId, dwMpIP, wMpPort);
        break;
    case EQP_TYPE_MIXER://混音器
        GetMixerSwitchAddr(byEqpId, dwMpIP, wMpPort);
        break;
    case EQP_TYPE_RECORDER://新录像机
        GetRecorderSwitchAddr(byEqpId, dwMpIP, wMpPort);
        break;
    case EQP_TYPE_VMP://画面合成器
        GetVmpSwitchAddr(byEqpId, dwMpIP, wMpPort);
        break;
    case EQP_TYPE_PRS://丢包重传器
        GetPrsSwitchAddr(byEqpId, dwMpIP, wMpPort);
        break;
	default:
		break;
    }
#else //8ke/8kh/8ki可以配代理，广播给外设打的时候直接用非代理的59000出来
	byDstMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	dwRcvIp = g_cMcuVcApp.GetMpIpAddr(byDstMpId);
#endif

    wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;

    if (g_cMcuVcApp.GetPeriEqpLogicChnnl(byEqpId, MODE_VIDEO, &byChannelNum, &tLogicalChannel, TRUE))
    {
        CServMsg cServMsg;
        cServMsg.SetConfIdx( tSrc.GetConfIdx() );
        
        dwDstIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
        wDstPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();
        
        TSwitchChannelExt tSwitchChannel;
        u8 bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_NONE;
        u8 byUniformPayload = INVALID_PAYLOAD;
        u8 byIsSwitchRtcp = 0;
        
        
        //构造交换信道
        tSwitchChannel.SetSrcMt( tSrc );
        tSwitchChannel.SetSrcIp( dwSrcIp );
        tSwitchChannel.SetRcvIP( dwRcvIp );
        tSwitchChannel.SetRcvPort( wRcvPort );
        tSwitchChannel.SetRcvBindIP( dwRcvIp );
        tSwitchChannel.SetDstIP( dwDstIp );
        tSwitchChannel.SetDstPort( wDstPort + PORTSPAN * wDstChnnl );
        
        // 准备给当前MP的消息体
        cServMsg.CatMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));                  
        cServMsg.CatMsgBody((u8 *)&bySwitchChannelMode, sizeof(u8));
        cServMsg.CatMsgBody((u8 *)&byUniformPayload, sizeof(u8));            
        cServMsg.CatMsgBody((u8 *)&byIsSwitchRtcp, sizeof(u8));
        
        
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "Send MCU_MP_ADDBRDDSTSWITCH_REQ for Eqp.%d(%s:%d)\n",
                      byEqpId,
                      StrOfIP(tSwitchChannel.GetDstIP()), tSwitchChannel.GetDstPort());
        
        BOOL32 bDstMpExist = FALSE;
       

//         for( u8 byMpId = 1; byMpId <= MAXNUM_DRI; byMpId ++ )
//         {
//             if ( g_cMcuVcApp.GetMpIpAddr(byMpId) == dwMpIP )
//             {
//                 bDstMpExist = TRUE;
//                 byDstMpId = byMpId;
//                 break;
//             }
//         }
// 
//         if ( !bDstMpExist )
//         {
//             LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[StartSwitchEqpFromBrd] Dst Mp.0x%x Unexist\n", dwMpIP );
//         }
    
		byDstMpId = g_cMcuVcApp.FindMp( dwRcvIp );
		
        bDstMpExist = g_cMcuVcApp.IsMpConnected(byDstMpId);

		if ( !bDstMpExist )
        {
            LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[StartSwitchEqpFromBrd] Dst Mp.0x%x Unexist\n", dwRcvIp );
        }
	    else
		{
			g_cMpSsnApp.SendMsgToMpSsn( byDstMpId, 
										MCU_MP_ADDBRDDSTSWITCH_REQ, 
										cServMsg.GetServMsg(), 
										cServMsg.GetServMsgLen());
		}
    }

    return TRUE;
}

/*====================================================================
    函数名      ：StopSwitchToBrd
    功能        ：停止将指定终端的视频作为广播源
    算法实现    ：
    引用全局变量：
    输入参数说明：tSrc, 源
				  bySrcChnnl,  源的信道号
    返回值说明  ：TRUE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/03/21    4.0         顾振华        创建
====================================================================*/
BOOL32 CMpManager::StopSwitchToBrd( const TMt &tSrc, u8 bySrcChnnl, u16 wSpyStartPort )
{
    /*CServMsg cServMsg;
    TSwitchChannel tSwitchChannel;
    
    //得到交换源地址
    u32 dwSrcMpIp;
    u16 wRcvPort;
    u32 dwSrcIp;
    if( !GetSwitchInfo( tSrc, dwSrcMpIp, wRcvPort, dwSrcIp ) )
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StopSwitchToBrd() failure because of can't get switch info!\n");
        return FALSE ;
	}

    tSwitchChannel.SetSrcMt(tSrc);
    tSwitchChannel.SetSrcIp(dwSrcIp);
    tSwitchChannel.SetRcvIP(dwSrcMpIp);
    tSwitchChannel.SetRcvPort(wRcvPort);

    cServMsg.SetConfIdx(tSrc.GetConfIdx());
    cServMsg.SetMsgBody((u8*)&tSwitchChannel, sizeof(TSwitchChannel));

    g_cMpSsnApp.BroadcastToAllMpSsn(MCU_MP_REMOVEBRDSRCSWITCH_REQ, 
                                    cServMsg.GetServMsg(), cServMsg.GetServMsgLen());  

    LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "Send MCU_MP_REMOVEBRDSRCSWITCH_REQ from RecvMp %s:%d to all Mp.\n",
                 StrOfIP(tSwitchChannel.GetRcvIP()), tSwitchChannel.GetRcvPort());

    return TRUE;
	*/
	CServMsg cServMsg;
    TSwitchChannel tSwitchChannel;
    
    //得到交换源地址
    u32 dwSrcMpIp;
    u16 wRcvPort;
    u32 dwSrcIp;
    if( !GetSwitchInfo( tSrc, dwSrcMpIp, wRcvPort, dwSrcIp ) )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StopSwitchToBrd() failure because of can't get switch info!\n");
        return FALSE ;
	}

    //  [11/26/2009 pengjie] Modify 级联多回传支持
	if( wSpyStartPort != SPY_CHANNL_NULL)
	{
		wRcvPort = wSpyStartPort;
	}
	//  End

    tSwitchChannel.SetSrcMt(tSrc);
    tSwitchChannel.SetSrcIp(dwSrcIp);
    tSwitchChannel.SetRcvIP(dwSrcMpIp);
    tSwitchChannel.SetRcvPort(wRcvPort);

    cServMsg.SetConfIdx(tSrc.GetConfIdx());
    cServMsg.SetMsgBody((u8*)&tSwitchChannel, sizeof(TSwitchChannel));

    g_cMpSsnApp.BroadcastToAllMpSsn(MCU_MP_REMOVEBRDSRCSWITCH_REQ, 
                                    cServMsg.GetServMsg(), cServMsg.GetServMsgLen());  

    LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "Send MCU_MP_REMOVEBRDSRCSWITCH_REQ from RecvMp %s:%d to all Mp. bySrcChnnl<%d>\n",
                 StrOfIP(tSwitchChannel.GetRcvIP()), tSwitchChannel.GetRcvPort(),bySrcChnnl);

    return TRUE;
}

/*====================================================================
    函数名      ：StartSwitchToSubMt
    功能        ：将指定终端数据交换到下级直连非次级终端
    算法实现    ：
    引用全局变量：
    输入参数说明：tSrc, 源
				  bySrcChnnl,  源的信道号
				  byDstMtId,   目的终端
				  byMode,      交换模式，缺省为MODE_BOTH
				  bySwitchMode 交换方式
                  bH239Channel
                  bStopBeforeStart 开新交换前是否停交换
    返回值说明  ：TRUE，如果不交换直接返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/12    1.0         LI Yi         创建
	03/07/15    1.0         胡昌威        从业务移入
====================================================================*/
// BOOL32 CMpManager::StartSwitchToSubMt( const TMt & tSrc, 
//                                      u8        bySrcChnnl, 
//                                      const TMt & tDst, 
//                                      u8        byMode,
//                                      u8        bySwitchMode, 
//                                      BOOL32 bH239Chnnl,
// 									 BOOL32 bStopBeforeStart,
// 									 BOOL32 bSrcHDBas,
// 									 u16 wSpyPort)
// {
// 	u32  dwSrcIp = 0;
// 	u32  dwDisIp = 0;
// 	u32  dwRcvIp = 0;
// 	u16  wRcvPort = 0;
// 	TLogicalChannel	tLogicalChannel;
// 	BOOL32 bResult1 = TRUE;
// 	BOOL32 bResult2 = TRUE;
// 
// 	//得到交换源地址
// 	if( !GetSwitchInfo( tSrc, dwRcvIp, wRcvPort, dwSrcIp ) )
// 	{
// 		 LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchToSubMt() failure because of can't get switch info!\n");
// 		 return FALSE ;
// 	}
//     
//     TConfInfo * ptConfInfo = &g_cMcuVcApp.GetConfInstHandle(tSrc.GetConfIdx())->m_tConf;
//     u8 byIsSwitchRtcp;
//     if (( TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_VMP == tSrc.GetEqpType() && MT_TYPE_MT != tDst.GetMtType() &&
//         ptConfInfo->GetConfAttrb().IsResendLosePack()) ||
//         tSrc == tDst) //  xsl [1/17/2006] (外厂商mt或mcu)将视频码流交换给自己时不进行rtcp交换
//     {
//         byIsSwitchRtcp = 0;
//     }
//     else
//     {
//         byIsSwitchRtcp = 1;
//     }
// 
// 	//只收数据不发
// 	TMt tDstMt = tDst;
// 	if( tDstMt.IsNull() )
// 	{
//         // 顾振华 [4/30/2006] 只收不发的处理不在这里,在逻辑通道打开后就转Dump 
//         LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "StartSwitchToSubMt() tDstMt is NULL! It's send-only MT?\n");        
// 		return TRUE;
// 	}
// 
// 	//得到会议终端表指针
// 	TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable( tSrc.GetConfIdx() );
// 	if( ptConfMtTable == NULL )
// 	{
// 		LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchToSubMt() failure because invalid ConfMtTable Pointer!\n");
// 		return FALSE;
// 	}
// 
//     u8 bySwitchChannelMode;
//     u8 byUniformPayload = INVALID_PAYLOAD;
//     u8 byManuID = ptConfMtTable->GetManuId(tDst.GetMtId());
// 
//     if (byManuID == MT_MANU_KDC || byManuID == MT_MANU_KDCMCU)
// 	{
// 		bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_NONE;
// 	}
//     else
//     {
//         bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_VALID;
//         //Active Payload Uniform
//         if (TYPE_MT == tSrc.GetType() && TYPE_MT == tDst.GetType())
//         {
//             u8 byChannelType;
//             TLogicalChannel  tChannel;
//             memset(&tChannel, 0, sizeof(TLogicalChannel));
//             //Get channal info
//             if (0 == bySrcChnnl)
//             {
//                 byChannelType = LOGCHL_VIDEO;
//             }
//             else if (1 == bySrcChnnl)
//             {
//                 byChannelType = LOGCHL_SECVIDEO;
//             }
// 
//             if (ptConfMtTable->GetMtLogicChnnl(tDst.GetMtId(), byChannelType, &tChannel, TRUE))
//             {
//                 if (tChannel.GetChannelType() >= AP_MIN && tChannel.GetChannelType() <= AP_MAX)
//                 {
//                     byUniformPayload = tChannel.GetChannelType();
//                 }
//             }
//         }
//     }
// 
// 	//广播交换方式
// 	//if(bySwitchMode==SWITCH_MODE_BROADCAST)
// 	// [pengjie 2010/9/26] 这里选看适配不过桥
// 	if( ( TYPE_MT == tSrc.GetType() || (TYPE_MCUPERI == tSrc.GetType() && tSrc.GetEqpType() == EQP_TYPE_BAS)) &&
// 		SWITCH_MODE_SELECT == bySwitchMode )
// 	{
// 		//zjj20091023选看终端不走桥，所以直接从源交换地址发送码流到目的地址
// 		//选看外设，比如vmp虽然选看，但是作为广播源的要建桥，所以还是要走桥
// 		//缺陷:如果其它外设是否有不作为广播源，且要走选看的话这里就不完善了，还要在if中加外设类型的判断
// 		LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[StartSwitchToSubMt] tSrc's type is mt and bySwitchMode is SWITCH_MODE_SELECT so not use brige  \n");
// 	}
// 	else
// 	{
// 		//得到目的Mt的交换信息
// 		u32  dwMpIP = 0;
// 		dwMpIP = g_cMcuVcApp.GetMpIpAddr( ptConfMtTable->GetMpId( tDst.GetMtId() ) );
// 		if( dwMpIP != dwRcvIp )//需要转发
// 		{
// 			dwDisIp = dwRcvIp;
// 			dwRcvIp = dwMpIP;
// 		}
// 	}
// 
// 	//根据信道号计算偏移量
// 	wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;    //双流要调整
// 
// 	//图像
// 	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
// 	{
// 		//建立交换
// 		if( ptConfMtTable->GetMtLogicChnnl( tDst.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, TRUE ) )	//可以交换
// 		{
// 			//  [11/9/2009 pengjie] 级联多回传支持
// 			if( wSpyPort != SPY_CHANNL_NULL )
// 			{
// 				wRcvPort = wSpyPort;
// 			}
//             bResult1 = StartSwitch( tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort, tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp, bStopBeforeStart);
// 			if(!bResult1)LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Switch video failure!\n");
// 		}
// 	}
// 
//     if (MODE_SECVIDEO == byMode)
//     {
//         //H.239视频逻辑通道
//         if (bH239Chnnl)
//         {
//             if (ptConfMtTable->GetMtLogicChnnl(tDst.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, TRUE))
//             {
// 				//zbq[09/03/2008] HD Bas作源的双流 从端口基址+PORTSPAN 建交换
// 				if (bSrcHDBas)
// 				{
// 					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort/* + PORTSPAN*/,
// 										   tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp);					
// 				}
// 				else
// 				{
// 					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort+4,
// 										   tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp);					
// 				}
//                 if (!bResult1)
//                 {
//                     LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Switch video h.239 failure!\n");
//                 }
//             }
//             return bResult1;
//         }
//     }
// 
// 	//语音
// 	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
// 	{
// 		//建立交换
// 		if( ptConfMtTable->GetMtLogicChnnl(tDst.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, TRUE ) )	//可以交换
// 		{
// 			//  [11/9/2009 pengjie] 级联多回传支持
// 			if( wSpyPort != SPY_CHANNL_NULL )
// 			{
// 				wRcvPort = wSpyPort;
// 			}
// 			bResult2 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, (wRcvPort+2),
//                                    tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp);
// 			if(!bResult2)LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Switch audio failure!\n");
// 		}
// 	}	
// 
// 	return  bResult1 && bResult2 ;
// }

/*====================================================================
    函数名      ：StopSwitchToSubMt
    功能        ：停止将数据交换到直连下级非次级终端
    算法实现    ：
    引用全局变量：
    输入参数说明：byDstMtId, 目的终端号
				  byMode, 交换模式，缺省为MODE_BOTH
				  bySwitchMode 交换方式
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/11    1.0         LI Yi         创建
	03/07/15    1.0         胡昌威        从业务移入
====================================================================*/
// void CMpManager::StopSwitchToSubMt( const TMt & tDst, u8 byMode, BOOL32 bH239Chnnl, u16 wSpyStartPort )
// {
// 	u8   byDstMtId = tDst.GetMtId();
// 	TLogicalChannel	tLogicalChannel;
// 
// 	TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable( tDst.GetConfIdx() );
// 	if( ptConfMtTable == NULL )
// 	{
// 		LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StopSwitchToSubMt() failure because invalid ConfMtTable Pointer!\n");
// 		return ;
// 	}
// 
//     TConfInfo * ptConfInfo = &g_cMcuVcApp.GetConfInstHandle(tDst.GetConfIdx())->m_tConf;
//     u8 byIsSwitchRtcp;
//     if (MT_TYPE_MT != tDst.GetMtType() && ptConfInfo->GetConfAttrb().IsResendLosePack())
//     {
//         byIsSwitchRtcp = 0;
//     }
//     else
//     {
//         byIsSwitchRtcp = 1;
//     }
// 
//     //图像
// 	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
// 	{
// 		if( ptConfMtTable->GetMtLogicChnnl( byDstMtId, LOGCHL_VIDEO, &tLogicalChannel, TRUE ) )	//已登记
// 		{
// 			//zjj20100201
// 			//  [12/7/2009 pengjie] 级联多回传支持
// 			if( wSpyStartPort != SPY_CHANNL_NULL )
// 			{
// 				tLogicalChannel.m_tRcvMediaChannel.SetPort( wSpyStartPort );
// 			}
// 			// End
// 			
// 			StopSwitch(tDst.GetConfIdx(), tLogicalChannel, 0, byIsSwitchRtcp);
// 		}
// 		
// 	}
// 
//     if (MODE_SECVIDEO == byMode)
// 	{
// 		//H.239视频逻辑通道
// 		if (bH239Chnnl)
// 		{
// 			if (ptConfMtTable->GetMtLogicChnnl(tDst.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, TRUE))
// 			{
// 				//zjj20100201
// 				//  [12/7/2009 pengjie] 级联多回传支持
// 				if( wSpyStartPort != SPY_CHANNL_NULL )
// 				{
// 					tLogicalChannel.m_tRcvMediaChannel.SetPort( wSpyStartPort + 4 );
// 				}
// 				// End
// 				
// 				StopSwitch(tDst.GetConfIdx(), tLogicalChannel, 0, byIsSwitchRtcp);
// 			}
// 			return;
// 		}
// 	}
// 
//     //语音
// 	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
// 	{
// 		if( ptConfMtTable->GetMtLogicChnnl( byDstMtId, LOGCHL_AUDIO, &tLogicalChannel, TRUE ) )	//已登记
// 		{
// 			//zjj20100201
// 			//  [12/7/2009 pengjie] 级联多回传支持
// 			if( wSpyStartPort != SPY_CHANNL_NULL )
// 			{
// 				tLogicalChannel.m_tRcvMediaChannel.SetPort( wSpyStartPort + 2 );
// 			}
// 			// End
// 			StopSwitch(tDst.GetConfIdx(), tLogicalChannel, 0, byIsSwitchRtcp);
// 		}
// 	}
// }

/*====================================================================
    函数名      ：StopSwitchToSubMt
    功能        ：停止将数据交换到直连下级非次级终端
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx 会议Id
	              u8 byMtNum 终端数
				  const TMt *ptDst 终端信息
				  byMode, 交换模式，缺省为MODE_BOTH
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/11    1.0         LI Yi         创建
	03/07/15    1.0         胡昌威        从业务移入
	20110505    4.6         pengjie       批量建交换支持
====================================================================*/
void CMpManager::StopSwitchToSubMt( u8 byConfIdx, u8 byMtNum, const TMt *ptDst, u8 byMode, u16 wSpyPort )
{
	if( byMtNum == 0 || ptDst == NULL )
	{
		return;
	}

	TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable( byConfIdx );
	if( ptConfMtTable == NULL )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "StopSwitchToSubMt failure because invalid ConfMtTable Pointer!\n");
		return ;
	}

	CMcuVcInst* pcInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
	TConfInfo *ptConfInfo = pcInst != NULL ? &pcInst->m_tConf : NULL;

	if (NULL == ptConfInfo)
	{
		return;
	}

	if( ptConfMtTable == NULL )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "StopSwitchToSubMt() failure because invalid TConfInfo Pointer!\n");
		return ;
	}

	if( NULL == pcInst )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "StopSwitchToSubMt() failure because invalid pcInst Pointer!\n");
		return;
	}

	u8 byIsSwitchRtcp = 0;
	CServMsg cServMsg;
	cServMsg.SetConfIdx(byConfIdx);
	//u32 dwMpIP = 0;
	TSwitchChannel tSwitchChannel;
	for (u8 byMpLoop = 1; byMpLoop <= MAXNUM_DRI; byMpLoop ++)
    {
		if ( !g_cMcuVcApp.IsMpConnected(byMpLoop) )
        {
            continue;
        }
		
        cServMsg.SetDstDriId( byMpLoop );
        cServMsg.SetMsgBody();
        
        //dwMpIP = g_cMcuVcApp.GetMpIpAddr( byMpLoop );

		u8 byMpId = 0;
		u8   byDstMtId = 0;
		for( u8 byLoop = 0; byLoop < byMtNum; byLoop++ )
		{
			// 通知接收地址 [pengguofeng 2/21/2013]
			byDstMtId = ptDst[byLoop].GetMtId();
			if( pcInst->IsMultiCastMt(byDstMtId) )
			{
				if (  byMode == MODE_VIDEO)
				{
					//pcInst->NotifyMtReceive(ptDst[byLoop], byDstMtId); //通知看自己					
					pcInst->ChangeSatDConfMtRcvAddr(byDstMtId, MODE_VIDEO, FALSE);		
				}
				
				//正在发送的双流源
				if ( byMode == MODE_SECVIDEO )
				{					
					pcInst->ChangeSatDConfMtRcvAddr(byDstMtId, MODE_SECVIDEO, FALSE);					
				}
				
				//音频广播源
				if ( byMode == MODE_AUDIO )
				{					
					pcInst->ChangeSatDConfMtRcvAddr(byDstMtId, MODE_AUDIO, FALSE);				
				}
// 				continue;//卫星终端不用真正拆交换
			}
			
			// 每次处理当前MP接入的终端
			byMpId = ptConfMtTable->GetMpId( ptDst[byLoop].GetMtId() );
			if ( byMpId != byMpLoop )
			{
				continue;
			}
			
			TLogicalChannel	tLogicalChannel;
			TConfAttrb tConfAttrb = ptConfInfo->GetConfAttrb();
			if (MT_TYPE_MT != ptDst[byLoop].GetMtType() && tConfAttrb.IsResendLosePack())
			{
				byIsSwitchRtcp = 0;
			}
			else
			{
				byIsSwitchRtcp = 1;
			}
			
			if( byMode == MODE_VIDEO )
			{
				if( !ptConfMtTable->GetMtLogicChnnl( byDstMtId, LOGCHL_VIDEO, &tLogicalChannel, TRUE ) )
				{				
					//StopSwitch(byConfIdx, tLogicalChannel, 0, byIsSwitchRtcp);
					continue;
				}
				
			}
			else if(byMode == MODE_SECVIDEO || byMode == MODE_VIDEO2SECOND)
			{
				if( !ptConfMtTable->GetMtLogicChnnl( byDstMtId, LOGCHL_SECVIDEO, &tLogicalChannel, TRUE ) )
				{				
					continue;
				}
			}
			else if( byMode == MODE_AUDIO )
			{
				if( !ptConfMtTable->GetMtLogicChnnl( byDstMtId, LOGCHL_AUDIO, &tLogicalChannel, TRUE ) )
				{				
					continue;
				}
			}
			else
			{
				LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[StopSwitchToSubMt] Failed Mt.%d, mode.%d !\n", byDstMtId, byMode );
				continue;
			}
			
			tSwitchChannel.SetDstIP(tLogicalChannel.m_tRcvMediaChannel.GetIpAddr());
			if (wSpyPort != SPY_CHANNL_NULL)
			{
				if (MODE_VIDEO == byMode)
				{
					//wSpyPort不变
				}
				else if(MODE_AUDIO == byMode)
				{
					wSpyPort += 2;
				}	
				else if (MODE_VIDEO2SECOND == byMode || MODE_SECVIDEO == byMode)
				{
					wSpyPort += 4;
				}
				else
					
				{
					continue;
				}
				tSwitchChannel.SetDstPort(wSpyPort);
			}
			else
			{
				tSwitchChannel.SetDstPort(tLogicalChannel.m_tRcvMediaChannel.GetPort());
			}	
		
			cServMsg.CatMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));
			cServMsg.CatMsgBody((u8 *)&byIsSwitchRtcp, sizeof(u8));
			
			LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "remove Mt.%d(%s:%d)  \n",
													 	ptDst[byLoop].GetMtId(),
														StrOfIP(tSwitchChannel.GetDstIP()), tSwitchChannel.GetDstPort());

			LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[StopSwitchToSubMt] Add Mt.%d,MpId.%d in remove Msg !\n", ptDst[byLoop].GetMtId(), byMpLoop );
		}

		if ( cServMsg.GetMsgBodyLen() > 0 )
		{
			g_cMpSsnApp.BroadcastToAllMpSsn(MCU_MP_REMOVESWITCH_REQ, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());  
		}
	}

	return;
}

/*====================================================================
    函数名      ：StopRecvSubMt
    功能        ：停止接收终端数据
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt & tMt 停止接收
				  u8 byMode
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/11    1.0         LI Yi         创建
	03/07/15    1.0         胡昌威        从业务移入
====================================================================
void CMpManager::StopRecvSubMt( const TMt & tMt, u8 byMode )
{
	u32 dwDstIp = 0x7f000001;
	u16 wDstPort;
	if( byMode == MODE_VIDEO || byMode == MODE_AUDIO )
	{			
		wDstPort = RECV_MT_START_PORT - 512 + 4*tMt.GetMtId() + 2*(byMode -1);//???Temp
		StopSwitch( tMt.GetConfIdx(), dwDstIp, wDstPort );
	}
	else
	{
		wDstPort = RECV_MT_START_PORT - 512 + 4*tMt.GetMtId() + 2*(MODE_VIDEO -1);
		StopSwitch( tMt.GetConfIdx(), dwDstIp, wDstPort );
		wDstPort = RECV_MT_START_PORT - 512 + 4*tMt.GetMtId() + 2*(MODE_AUDIO -1);
		StopSwitch( tMt.GetConfIdx(), dwDstIp, wDstPort );
	}
}
*/

/*====================================================================
    函数名      : GetRecorderSwitchAddr
    功能        ：得到录像机的数据交换IP地址与端口
    算法实现    ：
    引用全局变量：
    输入参数说明：byRecorderId 外设号
	              dwSwitchIpAddr 交换数据的IP地址
                  wSwitchPort  交换数据的端口
    返回值说明  ：TRUE，如果不存在返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/15    1.0         胡昌威        创建
====================================================================*/
BOOL32 CMpManager::GetRecorderSwitchAddr(u8 byRecorderId, u32 &dwSwitchIpAddr,u16 &wSwitchPort)
{
	TEqpRecInfo tRecInfo;
	if(SUCCESS_AGENT == g_cMcuAgent.GetEqpRecorderCfg( byRecorderId, &tRecInfo))
	{
        wSwitchPort = tRecInfo.GetMcuRecvPort();
//        dwSwitchIpAddr = ntohl( g_cMcuAgent.GetBoardIpAddrFromIdx(tRecInfo.GetSwitchBrdId()) );
// [pengjie 2010/3/30] 这里如果是8000e，就不能用GetSwitchBrdId()以及GetBoardIpAddrFromIdx(),来得到SwitchIp，
//                     因为8000e就一个mcu的IP.
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
		u16 wMcuStartPort = 0;
		u16 wEqpStartPort = 0;
		GetSwitchInfo( byRecorderId, dwSwitchIpAddr, wSwitchPort, wEqpStartPort );
		//wSwitchPort = wMcuStartPort;
#else
        dwSwitchIpAddr = ntohl( g_cMcuAgent.GetBoardIpAddrFromIdx(tRecInfo.GetSwitchBrdId()) );
#endif

		if( dwSwitchIpAddr > 0 )
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*====================================================================
    函数名      : GetMixerSwitchAddr
    功能        ：得到混音器的数据交换IP地址与端口
    算法实现    ：
    引用全局变量：
    输入参数说明：byMixerId 外设号
	              dwSwitchIpAddr 交换数据的IP地址
                  wSwitchPort  交换数据的端口
    返回值说明  ：TRUE，如果不存在返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/15    1.0         胡昌威        创建
====================================================================*/
BOOL32 CMpManager::GetMixerSwitchAddr(u8 byMixerId, u32 &dwSwitchIpAddr,u16 &wSwitchPort)
{
	TEqpMixerInfo tMixerInfo;
	if(SUCCESS_AGENT == g_cMcuAgent.GetEqpMixerCfg( byMixerId, &tMixerInfo))
	{
        wSwitchPort = tMixerInfo.GetMcuRecvPort();
        dwSwitchIpAddr = ntohl( g_cMcuAgent.GetBoardIpAddrFromIdx(tMixerInfo.GetSwitchBrdId()) );
		if( dwSwitchIpAddr > 0 )
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*====================================================================
    函数名      : GetBasSwitchAddr
    功能        ：得到码率适配器的数据交换IP地址与端口
    算法实现    ：
    引用全局变量：
    输入参数说明：byBasId 外设号
	              dwSwitchIpAddr 交换数据的IP地址
                  wSwitchPort  交换数据的端口
    返回值说明  ：TRUE，如果不存在返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/15    1.0         胡昌威        创建
====================================================================*/
BOOL32 CMpManager::GetBasSwitchAddr(u8 byBasId, u32 &dwSwitchIpAddr,u16 &wSwitchPort)
{
    TEqpBasInfo tBasInfo;
	if(SUCCESS_AGENT == g_cMcuAgent.GetEqpBasCfg( byBasId, &tBasInfo))
	{
        wSwitchPort = tBasInfo.GetMcuRecvPort();
		dwSwitchIpAddr = ntohl( g_cMcuAgent.GetBoardIpAddrFromIdx(tBasInfo.GetSwitchBrdId()) );
        
		if( dwSwitchIpAddr > 0 )
		{
			return TRUE;
		}
	}

    TEqpBasHDInfo tBasHdInfo;
	if(SUCCESS_AGENT == g_cMcuAgent.GetEqpBasHDCfg( byBasId, &tBasHdInfo))
	{
        wSwitchPort = tBasHdInfo.GetMcuRecvPort();
		//dwSwitchIpAddr = tBasHdInfo.GetIpAddr();
        dwSwitchIpAddr = ntohl( g_cMcuAgent.GetBoardIpAddrFromIdx(tBasHdInfo.GetSwitchBrdId()) );
        
		if( dwSwitchIpAddr > 0 )
		{
			return TRUE;
		}
	}    

	return FALSE;
}

/*====================================================================
    函数名      : GetVmpSwitchAddr
    功能        ：得到画面合成器的数据交换IP地址与端口
    算法实现    ：
    引用全局变量：
    输入参数说明：byVmpId 外设号
	              dwSwitchIpAddr 交换数据的IP地址
                  wSwitchPort  交换数据的端口
    返回值说明  ：TRUE，如果不存在返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/15    1.0         胡昌威        创建
====================================================================*/
BOOL32 CMpManager::GetVmpSwitchAddr(u8 byVmpId, u32 &dwSwitchIpAddr,u16 &wSwitchPort)
{
    TEqpVMPInfo tVmpInfo;

    if(SUCCESS_AGENT == g_cMcuAgent.GetEqpVMPCfg( byVmpId, &tVmpInfo))
    {
        wSwitchPort = tVmpInfo.GetMcuRecvPort();
        dwSwitchIpAddr = ntohl( g_cMcuAgent.GetBoardIpAddrFromIdx(tVmpInfo.GetSwitchBrdId()) );
        if( dwSwitchIpAddr > 0 )
        {
            return TRUE;
        }
    }
	return FALSE;
}

/*====================================================================
    函数名      : GetVmpTwSwitchAddr
    功能        ：得到复合电视墙的数据交换IP地址与端口
    算法实现    ：
    引用全局变量：
    输入参数说明：byVmpTwId 外设号
	              dwSwitchIpAddr 交换数据的IP地址
                  wSwitchPort  交换数据的端口
    返回值说明  ：TRUE，如果不存在返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/15    1.0         胡昌威        创建
====================================================================*/
//BOOL32 CMpManager::GetVmpTwSwitchAddr(u8 byVmpTwId, u32 &dwSwitchIpAddr, u16 &wSwitchPort)
//{
//    TEqpMpwInfo tVmpTwInfo;
//
//    if(g_cMcuAgent.GetEqpMpwCfg(byVmpTwId, &tVmpTwInfo))
//    {
//        wSwitchPort = tVmpTwInfo.wMcueqpMpwEntPort;
//        dwSwitchIpAddr = ntohl(g_cMcuAgent.GetBoardIpAddrFromIdx(tVmpTwInfo.byMcueqpVMPEntSwitchBrdId));
//        if (dwSwitchIpAddr > 0)
//        {
//            return TRUE;
//        }
//    }
//
//    return FALSE;
//}

/*====================================================================
    函数名      : GetPrsSwitchAddr
    功能        ：得到丢包重传器的数据交换IP地址与端口
    算法实现    ：
    引用全局变量：
    输入参数说明：byPrsId 外设号
	              dwSwitchIpAddr 交换数据的IP地址
                  wSwitchPort  交换数据的端口
    返回值说明  ：TRUE，如果不存在返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/15    1.0         胡昌威        创建
====================================================================*/
BOOL32 CMpManager::GetPrsSwitchAddr( u8 byPrsId, u32 &dwSwitchIpAddr,u16 &wSwitchPort )
{
    TEqpPrsInfo tPrsInfo;
	if(SUCCESS_AGENT == g_cMcuAgent.GetEqpPrsCfg( byPrsId,  tPrsInfo))
	{
        wSwitchPort = tPrsInfo.GetMcuRecvPort();
        dwSwitchIpAddr = ntohl( g_cMcuAgent.GetBoardIpAddrFromIdx(tPrsInfo.GetSwitchBrdId()) );
		if( dwSwitchIpAddr > 0 )
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*====================================================================
    函数名      ：StartSwitchToPeriEqp
    功能        ：将指定终端数据交换到直连外设
    算法实现    ：目前广播源到外设的交换所采用的转发板为广播源所在的转发板，
				  而非为外设所分配的mcu侧码流接收转发板，因为后者在转发板掉
				  线时mcu将自动重新分配
    引用全局变量：
    输入参数说明：tSrc, 源, 为本地化后的终端
				  bySrcChnnl, 源的信道号
				  byEqpId, 外设号
				  byDstChnnl, 目的信道索引号，缺省为0
   				  byMode, 交换模式，缺省为MODE_BOTH
				  bySwitchMode 交换方式
   返回值说明  ：TRUE，如果不交换直接返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/20    1.0         LI Yi         创建
	03/07/15    1.0         胡昌威        从业务移入
	10/02/25	4.6			薛亮		  整合8000E
====================================================================*/
BOOL32 CMpManager::StartSwitchToPeriEqp(const TMt & tSrc,
										u16	wSrcChnnl,
										u8	byEqpId,
										u16 wDstChnnl,
										u8	byMode,
										u8	bySwitchMode,
										BOOL32 bDstHDBas,
										BOOL32 bStopBeforeStart,
										u8 byHduSubChnId)
{
    u8  byChannelNum = 0;
    u32 dwDisIp = 0;
    u32 dwSrcIp = 0;
    u32 dwRcvIp = 0;
    u16 wRcvPort = 0;
    u32 dwDstIp = 0;
    u16 wDstPort = 0;
    TLogicalChannel	tLogicalChannel;
    BOOL32 bResult1 = TRUE;
    BOOL32 bResult2 = TRUE;

    //得到交换源地址
    if (!GetSwitchInfo(tSrc, dwRcvIp, wRcvPort, dwSrcIp))
    {
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[StartSwitchToPeriEqp]failure because of can't get switch info for mt()!\n");
        return FALSE;
    }

	//zjj20100306 多回传修改
    //wRcvPort = wRcvPort + PORTSPAN * wSrcChnnl;

	if( wSrcChnnl >= CASCADE_SPY_STARTPORT && wSrcChnnl < MT_MCU_STARTPORT ) //用多回传的通道
	{
		wRcvPort = wSrcChnnl;
	}
	else
	{
		wRcvPort = wRcvPort + PORTSPAN * wSrcChnnl;
	}

	// [2/25/2010 xliang] 取外设类型.
	//注：8000A和8000E实现不同，在mcuagtlib和mcuagt8000elib中区分，此处接口统一
    u8 byEqpType = g_cMcuAgent.GetPeriEqpType(byEqpId);
    if (0 == byEqpType)
    {
        if (byEqpId >= VMPID_MIN && byEqpId <= VMPID_MAX)
        {
            byEqpType = EQP_TYPE_VMP;
        }
    }


    //广播交换方式
    u32  dwMpIP = 0;
    u16  wMpPort = 0;

    //得到目的Mt的交换信息
	// [2/25/2010 xliang] 下面接口的调用主要是为了得到外设对应的转发板ip，以确定disIp的赋值；
	// 对于8000E，此步没有必要，且下面接口对8000E也不适用。
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
    switch (byEqpType)
    {
    case EQP_TYPE_MIXER://混音器
        GetMixerSwitchAddr(byEqpId, dwMpIP, wMpPort);
        break;
    case EQP_TYPE_RECORDER://新录像机
        GetRecorderSwitchAddr(byEqpId, dwMpIP, wMpPort);
        break;
    case EQP_TYPE_BAS://码率适配器
        GetBasSwitchAddr(byEqpId, dwMpIP, wMpPort);
        break;
    case EQP_TYPE_VMP://画面合成器
        GetVmpSwitchAddr(byEqpId, dwMpIP, wMpPort);
        break;
    case EQP_TYPE_PRS://丢包重传器
        GetPrsSwitchAddr(byEqpId, dwMpIP, wMpPort);
        break;
	default:
		break;
    }

    // 需要转发
	// [pengjie 2010/9/27] 终端录像不走桥，直接从源到录像机
//     if (SWITCH_MODE_BROADCAST == bySwitchMode /*|| EQP_TYPE_RECORDER == byEqpType*/)
//     {
//         if (dwMpIP != dwRcvIp)
//         {
//             dwDisIp = dwRcvIp;
//             dwRcvIp = dwMpIP;
//         }
//     }
#endif


    // guzh [5/12/2007] 如果是8000B 外设是在MPC上，且配置了内部IP， 则采用内部通信IP
#ifdef _MINIMCU_
    u32 dwEqpIp = 0;
    BOOL32 bModified2InnerIp = FALSE;
    g_cMcuAgent.GetPeriInfo(byEqpId, &dwEqpIp, &byEqpType);

	TMcuStatus tMcuStatus;
	g_cMcuVcApp.GetMcuCurStatus( tMcuStatus );
    // FIXME: 这里没有考虑MPC和DSC都启MP的情况, zgc, 2007-09-03
	

    if ( dwEqpIp == g_cMcuAgent.GetMpcIp() && tMcuStatus.IsExistDSC() )
    {
        TDSCModuleInfo tDscInfo;
        g_cMcuAgent.GetDscInfo(&tDscInfo);

		if ( htonl(tDscInfo.GetMcuInnerIp()) != g_cMcuAgent.GetMpcIp())
		{
			dwEqpIp = htonl(tDscInfo.GetMcuInnerIp());
			bModified2InnerIp = TRUE;
		}
    }

    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[StartSwitchToPeriEqp] bModify2Inner.%d, mcueqp recv ip: 0x%x\n", bModified2InnerIp, dwEqpIp);
#endif


    //图像
    if (MODE_VIDEO == byMode || MODE_BOTH == byMode)
    {
//         //与baphd保持同步
//         if (EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId))
//         {
// 			u8 byOutNum = g_cMcuVcApp.GetBasOutPutChnNum(g_cMcuVcApp.GetEqp(byEqpId));
//             wDstChnnl = wDstChnnl * byOutNum;
//         }
        // zbq [08/04/2007] BAS交换同广播目标交还调整为同一逻辑
        if ( EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId) && bySwitchMode == SWITCH_MODE_BROADCAST)
        {
            StartSwitchEqpFromBrd( tSrc, wSrcChnnl, byEqpId, wDstChnnl);
        }
        else
        {
            if (g_cMcuVcApp.GetPeriEqpLogicChnnl(byEqpId, MODE_VIDEO, &byChannelNum, &tLogicalChannel, TRUE))
            {
                dwDstIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
                wDstPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();
            #ifdef _MINIMCU_
                if (bModified2InnerIp)
                {
                    dwDstIp = dwEqpIp;
                }
            #endif
                bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort,
                                       dwDstIp, (wDstPort + PORTSPAN * wDstChnnl), 0, 0, 
									   SWITCHCHANNEL_UNIFORMMODE_NONE, INVALID_PAYLOAD, 1, bStopBeforeStart
									   );// xliang [4/13/2009] modify for 4 last param
            }
        }
    }

    //语音
    if (MODE_AUDIO == byMode || MODE_BOTH == byMode)
    {
//         //与baphd保持同步
//         if (EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId))
//         {
//             u8 byOutNum = g_cMcuVcApp.GetBasOutPutChnNum(g_cMcuVcApp.GetEqp(byEqpId));
//             wDstChnnl = wDstChnnl * byOutNum;
//         }

        if (g_cMcuVcApp.GetPeriEqpLogicChnnl(byEqpId, MODE_AUDIO, &byChannelNum, &tLogicalChannel, TRUE))
        {
            dwDstIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
            wDstPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();

#ifdef _MINIMCU_
            if (bModified2InnerIp)
            {
                dwDstIp = dwEqpIp;
            }
#endif

            bResult2 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, (wRcvPort + 2),
                                   dwDstIp, (wDstPort + PORTSPAN * wDstChnnl), 0, 0);
        }	
    }

    //双流图像
    if (MODE_SECVIDEO == byMode)
    {
        if (g_cMcuVcApp.GetPeriEqpLogicChnnl(byEqpId, MODE_VIDEO, &byChannelNum, &tLogicalChannel, TRUE))
        {
            dwDstIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
            wDstPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();

#ifdef _MINIMCU_
            if (bModified2InnerIp)
            {
                dwDstIp = dwEqpIp;
            }
#endif
			TEqp tTempEqp = g_cMcuVcApp.GetEqp( byEqpId );
            if (EQP_TYPE_PRS ==  g_cMcuVcApp.GetEqpType(byEqpId) )
            {
				//[nizhijun 2010/12/15] 需要判断maintpye是否是外设
				//[liu lijiu][2010/11/03]strc是bas时，端口不加4
				if (TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_BAS == tSrc.GetEqpType())
				{
					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort,
                                       dwDstIp, (wDstPort + PORTSPAN * wDstChnnl), 0, 0);
				}
				else
				{
					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, (wRcvPort + 4),
                                       dwDstIp, (wDstPort + PORTSPAN * wDstChnnl), 0, 0);
				}
            }
			else if (EQP_TYPE_VMP ==  g_cMcuVcApp.GetEqpType(byEqpId) )
            {
				bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, (wRcvPort + 4),
                                       dwDstIp, (wDstPort + PORTSPAN * wDstChnnl), 0, 0);
			}
			else if ( IsValidHduEqp(tTempEqp) )
			{
				if ( IsValidHduChn(byEqpId, (u8)wDstChnnl, byHduSubChnId) )
				{
					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, (wRcvPort + 4),
                                       dwDstIp, (wDstPort + PORTSPAN * wDstChnnl), 0, 0);
				}
			}
            else
            {
				//对于源是BAS，目的是录像机的话，双流模式下，源BAS的端口不偏移+4
				if ( TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_BAS == tSrc.GetEqpType() &&
				 	 EQP_TYPE_RECORDER ==  g_cMcuVcApp.GetEqpType(byEqpId)
					)
				{
					
				}
				else
				{
					wRcvPort+=4;
				}

				//zbq[09/03/2008] 发给HD Bas的双流交换从 端口基址+PORTSPAN 开始建交换
				if (!bDstHDBas)
				{
					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort,
										   dwDstIp, (wDstPort + PORTSPAN * wDstChnnl + 4), 0, 0);					
				}
				else
				{
// 					// [2/25/2010 xliang] FIXME: 8000E-VMPBAS 交换
// 					u8 byOutNum = g_cMcuVcApp.GetBasOutPutChnNum(g_cMcuVcApp.GetEqp(byEqpId));
// 					wDstChnnl = wDstChnnl * byOutNum;
					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort,
										   dwDstIp, (wDstPort + PORTSPAN * wDstChnnl), 0, 0);
				}
            }            
        }
    }

    return bResult1 && bResult2;
}

/*====================================================================
    函数名      ：StopSwitchToPeriEqp
    功能        ：停止将数据交换到外设
    算法实现    ：
    引用全局变量：
    输入参数说明：byEqpId,  外设号
				  byDstChnnl, 信道索引号，缺省为0	
				  byMode, 交换模式，缺省为MODE_BOTH
				  bySwitchMode 交换方式
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/11    1.0         LI Yi         创建
	03/07/15    1.0         胡昌威        从业务移入
====================================================================*/
void CMpManager::StopSwitchToPeriEqp( u8 byConfIdx, u8 byEqpId, u16 wDstChnnl,  u8 byMode, u8 byHduSubChnId )
{
	u8 byChannelNum = 0;
    TLogicalChannel tLogicalChannel;

    // guzh [5/12/2007] 如果是8000B 外设是在MPC上，且配置了内部IP， 则采用内部通信IP
#ifdef _MINIMCU_
    u32 dwEqpIp = 0;
    u8 byEqpType = 0;
    BOOL32 bModified2InnerIp = FALSE;
    g_cMcuAgent.GetPeriInfo(byEqpId, &dwEqpIp, &byEqpType); // 这里IP 为网络序

	TMcuStatus tMcuStatus;
	g_cMcuVcApp.GetMcuCurStatus( tMcuStatus );
    // FIXME: 这里没有考虑MPC和DSC都启MP的情况, zgc, 2007-09-03
    if (dwEqpIp == g_cMcuAgent.GetMpcIp() && tMcuStatus.IsExistDSC() )
    {
        TDSCModuleInfo tDscInfo;
        g_cMcuAgent.GetDscInfo(&tDscInfo);
		if ( htonl(tDscInfo.GetMcuInnerIp()) != g_cMcuAgent.GetMpcIp())
		{
			dwEqpIp = htonl(tDscInfo.GetMcuInnerIp());
			bModified2InnerIp = TRUE;
		}
    }   
    
    LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[StopSwitchToPeriEqp] bModify2Inner.%d, mcueqp recv ip: 0x%x\n", bModified2InnerIp, dwEqpIp);
#endif
    
    //图像
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		if( g_cMcuVcApp.GetPeriEqpLogicChnnl( byEqpId, MODE_VIDEO, &byChannelNum, &tLogicalChannel, TRUE ) )
		{
#ifdef _MINIMCU_
            if (bModified2InnerIp)
            {
                tLogicalChannel.m_tRcvMediaChannel.SetIpAddr( dwEqpIp );
            }            
#endif
			u8 byMulPortSpace = 1;
// 			if (EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId))
// 			{
// 				u8 byOutNum = g_cMcuVcApp.GetBasOutPutChnNum(g_cMcuVcApp.GetEqp(byEqpId));
// 				byMulPortSpace = byOutNum;
// 			}
			StopSwitch( byConfIdx, tLogicalChannel, wDstChnnl, 1, byMulPortSpace);
		}
	}

	//语音
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		if( g_cMcuVcApp.GetPeriEqpLogicChnnl( byEqpId, MODE_AUDIO, &byChannelNum, &tLogicalChannel, TRUE ) )
		{
#ifdef _MINIMCU_
            if (bModified2InnerIp)
            {
                tLogicalChannel.m_tRcvMediaChannel.SetIpAddr( dwEqpIp );
            }            
#endif
//             // 目前仅高清bas通道间隔翻倍10*2
//             if (EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId) &&
//                 g_cMcuAgent.IsEqpBasHD(byEqpId))
//             {
//                 wDstChnnl = 2 * wDstChnnl;
// 			}
			StopSwitch( byConfIdx, tLogicalChannel, wDstChnnl );
		}
	}
    
    // 双流
    if ( byMode == MODE_SECVIDEO )
    {
        u8 byEqpType = g_cMcuAgent.GetPeriEqpType(byEqpId);
		TEqp tEqp = g_cMcuVcApp.GetEqp( byEqpId );
		if( g_cMcuVcApp.GetPeriEqpLogicChnnl( byEqpId, MODE_VIDEO, &byChannelNum, &tLogicalChannel, TRUE ) )
		{
#ifdef _MINIMCU_
            if (bModified2InnerIp)
            {
                tLogicalChannel.m_tRcvMediaChannel.SetIpAddr( dwEqpIp );
            }            
#endif
            if (EQP_TYPE_PRS == byEqpType)
            {
                StopSwitch( byConfIdx, 
                            tLogicalChannel.m_tRcvMediaChannel.GetIpAddr(),
		                    tLogicalChannel.m_tRcvMediaChannel.GetPort() + PORTSPAN * wDstChnnl, 
                            TRUE);
            }
			else if ( IsValidHduEqp( tEqp ) && IsValidHduChn(byEqpId, (u8)wDstChnnl, byHduSubChnId) )
			{
				//拆双流交换
				StopSwitch( byConfIdx, 
					tLogicalChannel.m_tRcvMediaChannel.GetIpAddr(),
					tLogicalChannel.m_tRcvMediaChannel.GetPort() + PORTSPAN * wDstChnnl, 
                            TRUE);
				//补拆到hdu的音频交换
				TLogicalChannel tAudLGC;
				if( g_cMcuVcApp.GetPeriEqpLogicChnnl( byEqpId, MODE_AUDIO, &byChannelNum, &tAudLGC, TRUE ) )
				{
#ifdef _MINIMCU_
					if (bModified2InnerIp)
					{
						tAudLGC.m_tRcvMediaChannel.SetIpAddr( dwEqpIp );
					}            
#endif
					StopSwitch( byConfIdx, tAudLGC, wDstChnnl );
				}
			}
			else if ( EQP_TYPE_RECORDER == byEqpType )
			{
				StopSwitch( byConfIdx, 
						tLogicalChannel.m_tRcvMediaChannel.GetIpAddr(),
						tLogicalChannel.m_tRcvMediaChannel.GetPort() + PORTSPAN * wDstChnnl + 4, 
                        TRUE);
			
			}
			else if ( EQP_TYPE_VMP == byEqpType )
			{
				//拆双流交换
				StopSwitch( byConfIdx, 
					tLogicalChannel.m_tRcvMediaChannel.GetIpAddr(),
					tLogicalChannel.m_tRcvMediaChannel.GetPort() + PORTSPAN * wDstChnnl, 
					TRUE);
				
			}
            else 
            {
                //zbq[01/07/2009] 双流适配交换
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
				if (g_cMcuAgent.IsEqpBasHD(byEqpId))
#endif          
                {
	//				u8 byOutNum = g_cMcuVcApp.GetBasOutPutChnNum(g_cMcuVcApp.GetEqp(byEqpId));
                    StopSwitch( byConfIdx, 
                        tLogicalChannel.m_tRcvMediaChannel.GetIpAddr(),
                        tLogicalChannel.m_tRcvMediaChannel.GetPort() + PORTSPAN * wDstChnnl /** byOutNum*/, 
                        TRUE);
                }
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
                else
                {
					StopSwitch( byConfIdx, 
                        tLogicalChannel.m_tRcvMediaChannel.GetIpAddr(),
                        tLogicalChannel.m_tRcvMediaChannel.GetPort() + PORTSPAN * wDstChnnl + 4, 
                        TRUE);
                }
#endif
            }  
		

		}        
    }
}

/*====================================================================
    函数名      : StartSwitchToAll
    功能        ：开始交换(批量)
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMt &tSrc 源终端/外设
	              const u8 bySwitchGrpNum 该源有几个接收组（bas可能有多个）
				  TSwitchGrp *pSwitchGrp 各个接收组
				  u8    byMode // 音/视频模式
				  BOOL32 bStopBeforeStart // 建之前是否要先拆之前的交换
				  u16 wSpyStartPort // 多回传端口，回传源
    返回值说明  ：如果交换成功，返回值为真
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/07    1.0         JQL           创建
	03/07/15    1.0         胡昌威        从业务移入
	20110505    4.6         pengjie	      批量建交换支持
====================================================================*/
BOOL32 CMpManager::StartSwitchToAll( const TMt &tSrc,
									 const u8 bySwitchGrpNum,
									 TSwitchGrp *pSwitchGrp,
									 u8    byMode,
									 u8 bySwitchMode,
									 BOOL32 bStopBeforeStart,
									 u16 wSpyStartPort)
{
    CMcuVcInst * pcVcInst = g_cMcuVcApp.GetConfInstHandle(tSrc.GetConfIdx());
    TConfInfo  * ptConfInfo = pcVcInst != NULL ? &pcVcInst->m_tConf : NULL;
    TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable( tSrc.GetConfIdx() );

    u8 bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_NONE;
    u8 byUniformPayload = INVALID_PAYLOAD;
    u8 byIsSwitchRtcp = 0;
    u32 dwSrcMpIp = 0;
    u32 dwSrcIp = 0;
    u32 dwMpIP = 0;
    u32 dwDisIp = 0;
    u32 dwRcvIp = 0;
    u8 byManuID = 0;    
    u16 wRcvPort = 0;
    TLogicalChannel tLogicChannel;
	BOOL32 bRst = FALSE;

	if( bySwitchGrpNum == 0 || pSwitchGrp == NULL)
	{
		return FALSE;
	}

    if( ptConfMtTable == NULL || pcVcInst == NULL )
    {
        LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitch param err ptMtTable.0x%x ptVcInst.0x%x\n", ptConfMtTable, pcVcInst);
        return FALSE;
    }

	//按源所挂转发板Id发消息
	BOOL32 bSndMsgBySrcMp = FALSE;
	if( (TYPE_MT == tSrc.GetType() ||
		   (TYPE_MCUPERI == tSrc.GetType() 
			&& tSrc.GetEqpType() == EQP_TYPE_BAS) 
		  )&&
		SWITCH_MODE_SELECT == bySwitchMode)
	{
		bSndMsgBySrcMp = TRUE;
	}

	BOOL32 bMixNCutSwitch = FALSE;
	if ( TYPE_MCUPERI == tSrc.GetType() && 
		(EQP_TYPE_MIXER == tSrc.GetEqpType()) &&
		SWITCH_MODE_SELECT == bySwitchMode )
	{
		OspPrintf(TRUE, FALSE, "[StartSwitchToAll] The switch mode is Mixer's N-1 Switch!\n");
		bMixNCutSwitch = TRUE;
	}

    //得到交换源地址
    if( !GetSwitchInfo( tSrc, dwSrcMpIp, wRcvPort, dwSrcIp ) )
    {
        LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitch get switch info failed!\n");
        return FALSE ;
	}
    
	// src mp is Ipv4 or Ipv6? [pengguofeng 5/29/2012]
	BOOL32 bySrcMpIpType = g_cMcuVcApp.GetMpIpType(g_cMcuVcApp.FindMp(dwSrcMpIp));
	for ( u8 byMtLop1 = 0; byMtLop1 < bySwitchGrpNum; byMtLop1++ )
	{
		BOOL32 bFind = FALSE;
		u8 byDstMtNum = pSwitchGrp[byMtLop1].GetDstMtNum();
		if ( byDstMtNum == 0)
		{
			continue;
		}
		for ( u8 byMtLop2 = 0; byMtLop2 < byDstMtNum; byMtLop2++ )
		{
			TMt *pDstMt = pSwitchGrp[byMtLop1].GetDstMt();
			if ( pDstMt == NULL || pDstMt->IsNull())
			{
				continue;
			}

			if ( bySrcMpIpType == IP_V4
				&& pcVcInst->IsMtIpV6(pDstMt->GetMtId()) )
			{
				bSndMsgBySrcMp = FALSE;
				LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[StartSwitchToAll]src mp(%d) is Ipv4, and dst Mt(%d) is Ipv6, bSndMsgBySrcMp:%d!\n",
					g_cMcuVcApp.FindMp(dwSrcMpIp), pDstMt->GetMtId(), bSndMsgBySrcMp);
				bFind = TRUE;
				break;
			}
		}
		if ( bFind)
		{
			break;
		}
	}

	if( wSpyStartPort != SPY_CHANNL_NULL)
	{
		wRcvPort = wSpyStartPort;
	}
    u16 wTempRcvPort = wRcvPort;
    CServMsg cServMsg;
    cServMsg.SetConfIdx(tSrc.GetConfIdx());    
    TSwitchChannelExt tSwitchChannel;

    //[5/9/2011 zhushengze] GetMcInfo()参数修正
    u16 wMMcuIdx = INVALID_MCUIDX;
    if ( !pcVcInst->m_tCascadeMMCU.IsNull() )
    {
        wMMcuIdx = pcVcInst->GetMcuIdxFromMcuId( pcVcInst->m_tCascadeMMCU.GetMtId() );
	}

	// 1、 先停
	if( bStopBeforeStart )
	{
		CServMsg cServMsgToStop;
		cServMsgToStop.SetMsgBody();
		cServMsgToStop.SetConfIdx(tSrc.GetConfIdx());
		TSwitchChannel tSwitchChannelToStop;
		u8 byRemoveRtcp = 0;
		
		for( u8 bySrcChnl = 0; bySrcChnl < bySwitchGrpNum; bySrcChnl++ )
		{
			u8 byDstMtNum = pSwitchGrp[bySrcChnl].GetDstMtNum();
			TMt *pDstMt = pSwitchGrp[bySrcChnl].GetDstMt();
			if( pDstMt == NULL )
			{
				LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[StartSwitchToAll] pDstMt == NULL !\n" );
				continue;
			}
			
			for ( u8 byMtLoop = 0; byMtLoop < byDstMtNum; byMtLoop++ )
			{
				if (SWITCH_MODE_BROADCAST == bySwitchMode &&
					MODE_SECVIDEO != byMode &&
					!pcVcInst->m_tCascadeMMCU.IsNull()  &&
					pcVcInst->GetConfOtherMc()&&
					
					pcVcInst->GetConfOtherMc()->GetMcInfo(wMMcuIdx)&&
					pcVcInst->GetConfOtherMc()->GetMcInfo(wMMcuIdx)->m_tSpyMt == tSrc &&
					pcVcInst->m_tCascadeMMCU == pDstMt[byMtLoop] )
				{
					LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[StartSwitchToAll] tSrc.%d create no brd switch to MMcu.%d\n", 
						tSrc.GetMtId(), pcVcInst->m_tCascadeMMCU.GetMtId() );
					continue;
				}
				
				/*建立广播视频交换时，根据平滑考虑，指定以下规则，建交换前不拆交换
				 *1.当前目的终端上一个源和当前的源都是终端
				 *2.当前目的终端上一个源和当前的源相同，包括TMT和源通道号
				 */
				if (SWITCH_MODE_BROADCAST == bySwitchMode && MODE_VIDEO == byMode)
				{
					TMt tOldSrc;
					u8  byOldSrcChn = 0;
					if (pcVcInst->GetMtCurVidSrc(pDstMt[byMtLoop].GetMtId(), tOldSrc, byOldSrcChn))
					{
						
						if ((TYPE_MT == tOldSrc.GetType() && TYPE_MT == tSrc.GetType()) ||
							(tOldSrc == tSrc && byOldSrcChn == pSwitchGrp[bySrcChnl].GetSrcChnl()))
						{	
							LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[StartSwitchToAll] No need to stopswitch Mt.%d before start because its lastsrc<type.%d, EqpType.%d, McuId.%d, MtId.%d, SrcChn.%d>\n",
										pDstMt[byMtLoop].GetMtId(), tOldSrc.GetType(), tOldSrc.GetEqpType(), tOldSrc.GetMcuId(), tOldSrc.GetMtId(), byOldSrcChn);
							continue;
						}					
					}
				}
				byManuID = ptConfMtTable->GetManuId(pDstMt[byMtLoop].GetMtId()); 
				if (pDstMt[byMtLoop].IsNull())
				{
					continue;
				}
				
				if( byMode == MODE_VIDEO )
				{
					if( !ptConfMtTable->GetMtLogicChnnl(pDstMt[byMtLoop].GetMtId(), LOGCHL_VIDEO, &tLogicChannel, TRUE) )
					{
						continue;
					}
				}
				else if(byMode == MODE_SECVIDEO || byMode == MODE_VIDEO2SECOND)
				{
					if( !ptConfMtTable->GetMtLogicChnnl(pDstMt[byMtLoop].GetMtId(), LOGCHL_SECVIDEO, &tLogicChannel, TRUE) )
					{
						continue;
					}
				}
				else if( byMode == MODE_AUDIO)
				{
					if( !ptConfMtTable->GetMtLogicChnnl(pDstMt[byMtLoop].GetMtId(), LOGCHL_AUDIO, &tLogicChannel, TRUE) )
					{
						continue;
					}
				}
				else
				{
					LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[StartSwitchToAll] byMode is Invalid !!!\n" );
					return FALSE;
				}
				
				tSwitchChannelToStop.SetDstIP(tLogicChannel.m_tRcvMediaChannel.GetIpAddr());
				tSwitchChannelToStop.SetDstPort(tLogicChannel.m_tRcvMediaChannel.GetPort());
				
				cServMsgToStop.CatMsgBody( (u8 *)&tSwitchChannelToStop, sizeof(tSwitchChannelToStop) );
				cServMsgToStop.CatMsgBody( (u8 *)&byRemoveRtcp, sizeof(byRemoveRtcp) );
				
				LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "remove before start Mt.%d(%s:%d)  \n",
											 pDstMt[byMtLoop].GetMtId(),
											 StrOfIP(tSwitchChannelToStop.GetDstIP()), 
											 tSwitchChannelToStop.GetDstPort());
			}
		}
		
		if( cServMsgToStop.GetMsgBodyLen() > 0 )
		{
			g_cMpSsnApp.BroadcastToAllMpSsn(MCU_MP_REMOVESWITCH_REQ, cServMsgToStop.GetServMsg(), cServMsgToStop.GetServMsgLen());
		}
	}
	
	
	// 2、再重新建
    for (u8 byMpLoop = 1; byMpLoop <= MAXNUM_DRI; byMpLoop ++)
    {
        if ( !g_cMcuVcApp.IsMpConnected(byMpLoop) )
        {
            continue;
        }
		
        cServMsg.SetDstDriId( byMpLoop );
        cServMsg.SetMsgBody();
        
        dwMpIP = g_cMcuVcApp.GetMpIpAddr( byMpLoop );
		
		// 8KH/G/I多运营商可能导致src Mp Ip与dwMpIP不一致，但实际是同一块转发板 [5/30/2012 chendaiwei]
#if !defined(_8KI_) && !defined(_8KH_) && !defined(_8KE_)
		//zjl 20100510 选看不走桥，不能按照目的终端所挂转发板为依据发消息，而要按照源所挂转发板发消息
		if (bSndMsgBySrcMp &&
			dwMpIP != dwSrcMpIp)
		{
			continue;
		}
#endif
		BOOL32 bMulti = FALSE;
		for( u8 bySrcChnl = 0; bySrcChnl < bySwitchGrpNum; bySrcChnl++ )
		{
			u8 byDstMtNum = pSwitchGrp[bySrcChnl].GetDstMtNum();
			TMt *pDstMt = pSwitchGrp[bySrcChnl].GetDstMt();
			if( pDstMt == NULL )
			{
				LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "StartSwitch pDstMt is NULL\n" );
				continue;
			}
			wRcvPort = wTempRcvPort;
			wRcvPort = wRcvPort + PORTSPAN * pSwitchGrp[bySrcChnl].GetSrcChnl();
			
			if( byMode == MODE_AUDIO )
			{
				wRcvPort += 2;
			}
			else if( byMode == MODE_SECVIDEO )
			{

				if( !( (tSrc.GetType() == TYPE_MCUPERI && tSrc.GetEqpType() == EQP_TYPE_BAS) 
#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
					&& g_cMcuAgent.IsEqpBasHD(tSrc.GetEqpId())
#endif						
					) )
				{
					wRcvPort += 4;
				}

			}
			else
			{
				// vide Port don't need add
			}
			u8 byMpId = 0;
			for ( u8 byMtLoop = 0; byMtLoop < byDstMtNum; byMtLoop ++ )
			{
				// 通知卫星接收地址 [pengguofeng 2/21/2013]
				TMt tDstMt = pDstMt[byMtLoop];
				if ( bySwitchMode == SWITCH_MODE_BROADCAST )
				{
					bMulti = TRUE;
				}
				else if ( bySwitchMode == SWITCH_MODE_SELECT 
					&& tSrc == pcVcInst->GetVmpEqp() )
				{
					bMulti = TRUE; //vmp建交换的时候模式填的就是Select，所以此地保证一下
				}
				
				if ( pcVcInst->IsMultiCastMt(tDstMt.GetMtId()) )
				{
					LogPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "MP:[StartSwitchToAll]SatMt:%d Mode:%d SwitchMode:%d\n",
						tDstMt.GetMtId(), byMode, bySwitchMode);
					if ( byMode & MODE_VIDEO )
					{
						pcVcInst->ChangeSatDConfMtRcvAddr(tDstMt.GetMtId(), LOGCHL_VIDEO, bMulti);
					}
					if ( byMode & MODE_AUDIO )
					{
						pcVcInst->ChangeSatDConfMtRcvAddr(tDstMt.GetMtId(), LOGCHL_AUDIO, bMulti);
					}
					if ( byMode == MODE_SECVIDEO )
					{
						pcVcInst->ChangeSatDConfMtRcvAddr(tDstMt.GetMtId(), LOGCHL_SECVIDEO, bMulti);
					}
					
					if ( bMulti )
					{
						bRst = TRUE; //为了让McuVcInst的该接口往下走
						continue;//当广播或是VMP广播时，卫星终端不用真正建交换
					}
				}

				// 每次处理当前MP接入的终端
				byMpId = ptConfMtTable->GetMpId( pDstMt[byMtLoop].GetMtId() );
				if (!bSndMsgBySrcMp && 
					byMpId != byMpLoop )
				{
					continue;
				}
				
				// zbq [06/29/2007] 回传源的广播交换不能替换其普通回传交换
				if (SWITCH_MODE_BROADCAST == bySwitchMode &&
					MODE_SECVIDEO != byMode &&
					!pcVcInst->m_tCascadeMMCU.IsNull()  &&
					pcVcInst->GetConfOtherMc()->GetMcInfo(wMMcuIdx)&&
					pcVcInst->GetConfOtherMc()->GetMcInfo(wMMcuIdx)->m_tSpyMt == tSrc &&
					pcVcInst->m_tCascadeMMCU == pDstMt[byMtLoop] )
				{
					LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "StartSwitch tSrc.%d create no brd switch to MMcu.%d\n", 
						tSrc.GetMtId(), pcVcInst->m_tCascadeMMCU.GetMtId() );
					continue;
				}
				
				byManuID = ptConfMtTable->GetManuId(pDstMt[byMtLoop].GetMtId()); 
				
				if( byMode == MODE_VIDEO )
				{
					if( !ptConfMtTable->GetMtLogicChnnl(pDstMt[byMtLoop].GetMtId(), LOGCHL_VIDEO, &tLogicChannel, TRUE) )
					{
						continue;
					}
				}
				else if(byMode == MODE_SECVIDEO || byMode == MODE_VIDEO2SECOND)
				{
					if( !ptConfMtTable->GetMtLogicChnnl(pDstMt[byMtLoop].GetMtId(), LOGCHL_SECVIDEO, &tLogicChannel, TRUE) )
					{
						continue;
					}
				}
				else if( byMode == MODE_AUDIO)
				{
					if( !ptConfMtTable->GetMtLogicChnnl(pDstMt[byMtLoop].GetMtId(), LOGCHL_AUDIO, &tLogicChannel, TRUE) )
					{
						continue;
					}
				}
				else
				{
					LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitch byMode is Invalid\n" );
					return FALSE;
				}
				
				TConfAttrb tConfAttrb;
				if( ptConfInfo != NULL )
				{
					tConfAttrb = ptConfInfo->GetConfAttrb();
				}

				if (( TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_VMP == tSrc.GetEqpType() && MT_TYPE_MT != pDstMt[byMtLoop].GetMtType()
					  && ptConfInfo
					  && tConfAttrb.IsResendLosePack()) 
					|| tSrc == pDstMt[byMtLoop]) //  xsl [1/17/2006] (外厂商mt或mcu)将视频码流交换给自己时不进行rtcp交换
				{
					byIsSwitchRtcp = 0;
				}
				else
				{
					byIsSwitchRtcp = 1;
				}
				
				if (byManuID == MT_MANU_KDC || byManuID == MT_MANU_KDCMCU)
				{
					bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_NONE;
				}
				else
				{
					bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_VALID;
					if (TYPE_MT == tSrc.GetType() && TYPE_MT == pDstMt[byMtLoop].GetType())
					{
						if (tLogicChannel.GetChannelType() >= AP_MIN && tLogicChannel.GetChannelType() <= AP_MAX)
						{
							byUniformPayload = tLogicChannel.GetChannelType();
						}
					}
				}
				

				//8000H/8000G/8000I直接取源MP的IP [5/25/2012 chendaiwei]
#if defined(_8KH_) || defined(_8KE_) || defined(_8KI_)
				dwRcvIp = dwSrcMpIp;

				LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_MPMGR,"[startswitchToAll] dwRcvIP = dwSrcMpIp == %s\n",StrOfIP(dwSrcMpIp));	
#else
				//如果选看，接收IP直接指定为源所挂转发板ip,分发ip为0
				if (bSndMsgBySrcMp)
				{
					dwRcvIp = dwSrcMpIp;
				}
				else
				{	
					//得到目的Mt的交换信息                    
					if( dwMpIP != dwSrcMpIp )//需要转发
					{
						dwDisIp = dwSrcMpIp;
					}
					else
					{
						dwDisIp = 0;                
					}   
					dwRcvIp = dwMpIP;
				}
#endif				
				
				//建立交换
				
				//构造交换信道             
				tSwitchChannel.Clear();
				tSwitchChannel.SetSrcMt( tSrc );
				tSwitchChannel.SetSrcIp( dwSrcIp );
				tSwitchChannel.SetRcvIP( dwRcvIp );
				tSwitchChannel.SetRcvPort( wRcvPort );
				
				// 混音器N-1模式交换：混音器直接打给各混音成员所在转发板
				if(bMixNCutSwitch)
				{
					//[pengguofeng 5/11/2012]虽然此处目前不需要改，由MP侧自行判断，但是最终版本还是要改的 
					tSwitchChannel.SetDisIp( 0 );
				}
				else
				{
					tSwitchChannel.SetDisIp( dwDisIp );
				}

				tSwitchChannel.SetRcvBindIP( dwRcvIp );
				tSwitchChannel.SetSndBindIP( dwRcvIp );
				tSwitchChannel.SetDstIP( tLogicChannel.m_tRcvMediaChannel.GetIpAddr() );
				tSwitchChannel.SetDstPort( tLogicChannel.m_tRcvMediaChannel.GetPort() );
				//根据配置文件读取是否要作假的ip/port，看dstip和port是否是要作假的
				if ( g_cMcuVcApp.IsNeedMapDstIpAddr( htonl(tSwitchChannel.GetDstIP()) ) )
				{
					//做假的ip为目的终端所挂转发板ip，端口为后向通道的端口
					u32 dwMappedIp = 0;
					u16 wMappedPort = 0 ;
					u32 dwDstSrcIp = 0;
					GetSwitchInfo(pDstMt[byMtLoop],dwMappedIp,wMappedPort,dwDstSrcIp);
					if ( dwMappedIp == 0 || wMappedPort == 0 )//获取失败，则用固定的ip和端口
					{
						StaticLog("[StartSwitchToAll]Get dstmt(mtid:%d) switchip and port failed\n",pDstMt[byMtLoop].GetMtId());
						//设置为recvip和port
						dwMappedIp = dwRcvIp;
						wMappedPort = DEFAULT_MAPPORT;

					}
					//成功则用取出来的ip和端口
					tSwitchChannel.SetMapIp( dwMappedIp );
					if ( MODE_AUDIO == byMode )
					{
						wMappedPort += 2;
					}
					else if (MODE_SECVIDEO == byMode)
					{
						wMappedPort += 4;
					}
					tSwitchChannel.SetMapPort(wMappedPort);
				}
			
				// 准备给当前MP的消息体
				cServMsg.CatMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));                  
				cServMsg.CatMsgBody((u8 *)&bySwitchChannelMode, sizeof(u8));
				cServMsg.CatMsgBody((u8 *)&byUniformPayload, sizeof(u8));            
				cServMsg.CatMsgBody((u8 *)&byIsSwitchRtcp, sizeof(u8));
				
				LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[StartSwitchToAll]Add Mt.%d(%s:%d) From %s(dis:%s)in Msg to Mp.%d \n",
					pDstMt[byMtLoop].GetMtId(),
					StrOfIP(tSwitchChannel.GetDstIP()), tSwitchChannel.GetDstPort(), StrOfIP(dwSrcIp), StrOfIP(dwDisIp),byMpLoop);
			}
		}

        if ( cServMsg.GetMsgBodyLen() > 0 )
        {
			LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "Send MCU_MP_ADDSWITCH_REQ to Mp.%d \n", byMpLoop );
            g_cMpSsnApp.SendMsgToMpSsn( byMpLoop, 
                                        MCU_MP_ADDSWITCH_REQ, 
                                        cServMsg.GetServMsg(), 
                                        cServMsg.GetServMsgLen());
			bRst = TRUE;
        }
    }

    return bRst;
}

/*====================================================================
    函数名      : StartSwitch
    功能        ：开始交换
    算法实现    ：
    引用全局变量：
    输入参数说明：dwRcvIp     源地址
				  wRcvPort    源端口号
				  dwDstIp     目的地址
				  wDstPort    目的端口号
    返回值说明  ：如果交换成功，返回值为真
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/07    1.0         JQL           创建
	03/07/15    1.0         胡昌威        从业务移入
====================================================================*/
BOOL32 CMpManager::StartSwitch(const TMt & tSrcMt, u8 byConfIdx, 
                             u32 dwSrcIp, u32 dwDisIp, 
                             u32 dwRcvIp, u16 wRcvPort, 
                             u32 dwDstIp, u16 wDstPort, 
                             u32 dwRcvBindIP, u32 dwSndBindIP,
                             u8 bySwitchChannelMode,
                             u8 byUniformPayload,
                             u8 byIsSwitchRtcp, BOOL32 bStopBeforeStart,u8 byDstMtId)
{
	//根据IP地址查找交换MP编号
	u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	//没找到
	if( 0 == byMpId )
	{
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "No MP's Addrs is 0x%x in StartSwitch(),please check switch recv IP!\n",dwRcvIp);
		return FALSE;
	}

	//构造交换信道 
	TSwitchChannelExt tSwitchChannel;
	tSwitchChannel.SetSrcMt( tSrcMt );
	tSwitchChannel.SetDisIp( dwDisIp );
	tSwitchChannel.SetSrcIp( dwSrcIp );
    tSwitchChannel.SetRcvIP( dwRcvIp );
	tSwitchChannel.SetRcvPort( wRcvPort );
	if( 0 == dwRcvBindIP )
	{
		tSwitchChannel.SetRcvBindIP( dwRcvIp );
	}
	else
	{
		tSwitchChannel.SetRcvBindIP( dwRcvBindIP );
	}
	tSwitchChannel.SetDstIP( dwDstIp );
	tSwitchChannel.SetDstPort( wDstPort );
	if( 0 == dwSndBindIP )
	{
		if( 0 == dwRcvBindIP )
		{
			tSwitchChannel.SetSndBindIP(dwRcvIp);
		}
		else
		{
			tSwitchChannel.SetSndBindIP(dwRcvBindIP);
		}
	}
	else
	{
		tSwitchChannel.SetSndBindIP(dwSndBindIP);
	}
	//根据配置文件读取是否要作假的ip/port，看dstip和port是否是要作假的
	if ( byDstMtId != 0 && g_cMcuVcApp.IsNeedMapDstIpAddr( htonl(dwDstIp) ) )
	{
		//做假的ip为目的终端所挂转发板ip，端口为后向通道的端口
		u32 dwMappedIp = 0;
		u16 wMappedPort = 0 ;
		u32 dwDstSrcIp = 0;
		TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable( byConfIdx );
		if( ptConfMtTable == NULL )
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitch() failure because invalid ConfMtTable Pointer!\n");
			return FALSE;
		}
		GetSwitchInfo( ptConfMtTable->GetMt(byDstMtId),dwMappedIp,wMappedPort,dwDstSrcIp);
		if ( dwMappedIp == 0 || wMappedPort == 0 )//失败则用rcvip和固定port
		{
			StaticLog("[StartSwitchToAll]Get dstmt(mtid:%d) switchip and port failed\n",byDstMtId);
			dwMappedIp = dwRcvIp;
			wMappedPort = DEFAULT_MAPPORT;

		}
		
		tSwitchChannel.SetMapIp( dwMappedIp );
		if ( wDstPort%PORTSPAN == 2 )
		{
			wMappedPort += 2;
		}
		else if (wDstPort%PORTSPAN == 4)
		{
			wMappedPort += 4;
		}
		tSwitchChannel.SetMapPort(wMappedPort);
			
	}

	//停止其它设备向这一地址交换数据
    // guzh [6/6/2007] 交给MP保护
    // zbq [06/15/2007] 暂时不能让MP保护
    // zbq [06/29/2007] 特殊处理的确不需要关掉重开的交换
    if ( bStopBeforeStart )
    {
		StartStopSwitch(byConfIdx, dwDstIp, wDstPort, byMpId);
    }

	//发增加交换消息给MP
	CServMsg cServMsg;
	cServMsg.SetConfIdx(byConfIdx);
	cServMsg.SetDstDriId(byMpId);
	cServMsg.SetMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));

    // libo [5/13/2005]
    cServMsg.CatMsgBody((u8 *)&bySwitchChannelMode, sizeof(u8));
    // libo [5/13/2005]end

    cServMsg.CatMsgBody((u8 *)&byUniformPayload, sizeof(u8));

    cServMsg.CatMsgBody((u8 *)&byIsSwitchRtcp, sizeof(u8));

	g_cMpSsnApp.SendMsgToMpSsn(byMpId, MCU_MP_ADDSWITCH_REQ, 
                               cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

	s8 achRcvIp[17] = {0};
	memcpy(achRcvIp, StrOfIP(tSwitchChannel.GetRcvIP()), sizeof(achRcvIp));
    LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "Send Message: MCU_MP_ADDSWITCH_REQ for %s:%d --> %s:%d to Mp.\n",
                 achRcvIp, tSwitchChannel.GetRcvPort(),
                 StrOfIP(tSwitchChannel.GetDstIP()), tSwitchChannel.GetDstPort() );

    return TRUE; 
}

/*====================================================================
    函数名      : StopSwitch
    功能        ：停止交换
    算法实现    ：
    引用全局变量：
    输入参数说明：dwDstIp 目的地址
				  wDstPort 目的端口号
    返回值说明  ：如果停止交换成功，返回值为真
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/07    1.0         JQL           创建
	03/07/15    1.0         胡昌威        从业务移入
====================================================================*/
BOOL32 CMpManager::StopSwitch(u8 byConfIdx, u32 dwDstIp, u16 wDstPort, u8 byIsSwitchRtcp)
{
    //构造交换信道 
	TSwitchChannel tSwitchChannel;
	tSwitchChannel.SetDstIP(dwDstIp);
	tSwitchChannel.SetDstPort(wDstPort);

	//发移除交换消息给MP
	CServMsg cServMsg;
	cServMsg.SetConfIdx(byConfIdx);

	cServMsg.SetMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));

    cServMsg.CatMsgBody((u8 *)&byIsSwitchRtcp, sizeof(u8));

    g_cMpSsnApp.BroadcastToAllMpSsn(MCU_MP_REMOVESWITCH_REQ, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());

    LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "Send Message: MCU_MP_REMOVESWITCH_REQ for %s:%d to Mp in StopSwitch().\n",
                 StrOfIP(tSwitchChannel.GetDstIP()), tSwitchChannel.GetDstPort() );

    return TRUE;
}

/*=============================================================================
  函 数 名： StartStopSwitch
  功    能： 开始新的交换前清除原来的交换（此处没有移除RTCP交换）
  算法实现： 
  全局变量： 
  参    数： u8 byConfIdx
             u32 dwDstIp
             u16 wDstPort
             u8 byMpId
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMpManager::StartStopSwitch(u8 byConfIdx, u32 dwDstIp, u16 wDstPort, u8 byMpId)
{
    //构造交换信道 
	TSwitchChannel tSwitchChannel;
	tSwitchChannel.SetDstIP(dwDstIp);
	tSwitchChannel.SetDstPort(wDstPort);
	
	//发移除交换消息给MP
    u8 byRemoveRtcp = 0;
	CServMsg cServMsg;
	cServMsg.SetConfIdx( byConfIdx );
	cServMsg.SetMsgBody((u8 *)&tSwitchChannel,sizeof(tSwitchChannel));
    cServMsg.CatMsgBody( &byRemoveRtcp, sizeof(byRemoveRtcp) );

    //u32 dwIpAddr = g_cMcuAgent.GetCastIpAddr();   
    u32 dwIpAddr = g_cMcuVcApp.AssignMulticastIp(byConfIdx);   
    if (dwIpAddr == htonl(dwDstIp))
    {
        g_cMpSsnApp.SendMsgToMpSsn(byMpId, MCU_MP_REMOVESWITCH_REQ,
                                   cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
    }
    else
    {
        g_cMpSsnApp.BroadcastToAllMpSsn(MCU_MP_REMOVESWITCH_REQ, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
    }

    LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "Send Message: MCU_MP_REMOVESWITCH_REQ for %s:%d to Mp in StartStopSwitch() .\n",
                 StrOfIP(tSwitchChannel.GetDstIP()), tSwitchChannel.GetDstPort());

    return TRUE;
}

/*====================================================================
    函数名      ：StartSwitch
    功能        ：将指定端口数据交换到前向逻辑信道指定的端口
    算法实现    ：
    引用全局变量：
    输入参数说明：dwRcvIp,    源地址
				  wRcvPort,   源端口号
				  TFwdChnnl,  前向逻辑信道
				  byDstChnnl, 信道索引，缺省为0
                  bStopBeforeStart 开新交换前是否停交换
    返回值说明  ：如果交换成功，返回值为真
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/07    1.0         JQL           创建
	03/07/15    1.0         胡昌威        从业务移入
====================================================================*/
BOOL32 CMpManager::StartSwitch( const TMt & tSrcMt, u8 byConfIdx, 
							  u32 dwSrcIp, u32 dwDisIp, 
							  u32 dwRcvIp, u16 wRcvPort, 
							  const TLogicalChannel& tFwdChnnl, u16 wDstChnnl,
                              u8 bySwitchChannelMode,
                              u8 byUniformPayload,
                              u8 byIsSwitchRtcp, BOOL32 bStopBeforeStart,u8 byDstMtId)
{
	return StartSwitch(tSrcMt, byConfIdx, dwSrcIp, dwDisIp, dwRcvIp, wRcvPort, 
		               tFwdChnnl.m_tRcvMediaChannel.GetIpAddr(),
		               (tFwdChnnl.m_tRcvMediaChannel.GetPort() + PORTSPAN*wDstChnnl),
                       0, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp, bStopBeforeStart,byDstMtId);
}

/*====================================================================
    函数名      ：StopSwitch
    功能        ：停止向前向逻辑信道指定的端口交换
    算法实现    ：
    引用全局变量：
    输入参数说明：tFwdChnnl, 目标逻辑信道
				  byDstChnnl, 信道索引，缺省为0
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/07    1.0         JQL           创建
	03/07/15    1.0         胡昌威        从业务移入
====================================================================*/
BOOL32 CMpManager::StopSwitch(u8 byConfIdx, const TLogicalChannel& tFwdChnnl, u16 wDstChnnl, u8 byIsSwitchRtcp, u8 byMulPortSpace)
{
	return StopSwitch( byConfIdx, tFwdChnnl.m_tRcvMediaChannel.GetIpAddr(),
		              tFwdChnnl.m_tRcvMediaChannel.GetPort() + PORTSPAN * wDstChnnl * byMulPortSpace, byIsSwitchRtcp);
}

/*====================================================================
    函数名      ：AddMultiToOneSwitch
    功能        ：增加多点到一点的交换
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx, 会议索引号索引号
				  u32 dwSrcIp, 源IP
				  u32 dwDisIp, 分发IP
                  u32 dwRcvIp, 接收IP
				  u16 wRcvPort, 接收端口
				  u32 dwDstIp, 目的IP
				  u16 wDstPort, 目的端口
				  u32 dwRcvBindIP, 接收绑定IP
				  u32 dwSndBindIP, 发送帮定IP
				  u32 dwMapIpAddr
				  u16 wMapPort
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/05/12    3.0         胡昌威        创建
====================================================================*/
BOOL32 CMpManager::AddMultiToOneSwitch( u8 byConfIdx, u32 dwSrcIp, u32 dwDisIp, u32 dwRcvIp, u16 wRcvPort, 
						              u32 dwDstIp, u16 wDstPort, u32 dwRcvBindIP, u32 dwSndBindIP, 
									  u32 dwMapIpAddr, u16 wMapPort )
{
	//根据IP地址查找交换MP编号
	u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	
	LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "[AddMultiToOneSwitch] MP %s, port %d!",StrOfIP(dwRcvIp), wRcvPort);

	//没找到
	if(byMpId==0)
	{
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "No MP's Addrs is 0x%x in AddMultiToOneSwitch(),please check switch recv IP!",dwRcvIp);
		return FALSE;
	}

	//构造交换信道 
	TSwitchChannel tSwitchChannel;
	tSwitchChannel.SetDisIp( dwDisIp );
	tSwitchChannel.SetSrcIp( dwSrcIp );
	tSwitchChannel.SetRcvIP( dwRcvIp );
	tSwitchChannel.SetRcvPort( wRcvPort );
	if( dwRcvBindIP == 0 )
	{
		tSwitchChannel.SetRcvBindIP( dwRcvIp );
	}
	else
	{
		tSwitchChannel.SetRcvBindIP( dwRcvBindIP );	
	}
	tSwitchChannel.SetDstIP( dwDstIp );
	tSwitchChannel.SetDstPort( wDstPort );
	if(dwSndBindIP==0)
	{
		if(dwRcvBindIP==0)tSwitchChannel.SetSndBindIP(dwRcvIp);
		else tSwitchChannel.SetSndBindIP(dwRcvBindIP);
	}
	else
	{
		tSwitchChannel.SetSndBindIP(dwSndBindIP);
	}
	wMapPort    = htons(wMapPort);
	dwMapIpAddr = htonl(dwMapIpAddr);

	//发增加交换消息给MP
	CServMsg cServMsg;
	cServMsg.SetConfIdx( byConfIdx );
	cServMsg.SetDstDriId( byMpId );
	cServMsg.SetMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));
	cServMsg.CatMsgBody((u8 *)&dwMapIpAddr, sizeof(dwMapIpAddr));
	cServMsg.CatMsgBody((u8 *)&wMapPort, sizeof(wMapPort));
	g_cMpSsnApp.SendMsgToMpSsn( byMpId, MCU_MP_ADDMULTITOONESWITCH_REQ, 
		                        cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

    return TRUE; 

}
/*====================================================================
    函数名      ：AddMultiToOneSwitch
    功能        ：增加多点到一点的交换
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx,							会议索引号索引号
				  u8 byDstNum							目的数
				  TSwitchChannelExt *ptSwitchChannelExt 交换组
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	11/05/06    4.0         周嘉麟          创建
====================================================================*/
BOOL32 CMpManager::AddMultiToOneSwitch(u8 byConfIdx, 
									   u8 byDstNum, 
									   TSwitchChannelExt *ptSwitchChannelExt)
{
	if (0 == byDstNum || NULL == ptSwitchChannelExt)
	{
		return FALSE;
	}

	CServMsg cServMsg;
	cServMsg.SetConfIdx( byConfIdx );
	for (u8 byMpLoop = 1; byMpLoop <= MAXNUM_DRI; byMpLoop ++)
    {
        if ( !g_cMcuVcApp.IsMpConnected(byMpLoop) )
        {
            continue;
        }
		
		cServMsg.SetDstDriId(byMpLoop);
		cServMsg.SetMsgBody();
		for (u8 byIdx = 0; byIdx < byDstNum; byIdx++)
		{
			//根据IP地址查找交换MP编号
			u32 dwRcvIp = ptSwitchChannelExt[byIdx].GetRcvIP();
			u8   byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
			
			//没找到
			if(byMpId==0)
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_MSMGR, "No MP's Addrs is 0x%x in AddMultiToOneSwitch(),please check switch recv IP!",dwRcvIp);
				continue;
			}
			
			if (byMpId != byMpLoop)
			{
				continue;
			}

			//构造交换信道 
			TSwitchChannel tSwitchChannel;
			tSwitchChannel.SetDisIp(ptSwitchChannelExt[byIdx].GetDisIp());
			tSwitchChannel.SetSrcIp(ptSwitchChannelExt[byIdx].GetSrcIp());

			tSwitchChannel.SetRcvIP(ptSwitchChannelExt[byIdx].GetRcvIP());
			tSwitchChannel.SetRcvPort(ptSwitchChannelExt[byIdx].GetRcvPort());

			tSwitchChannel.SetRcvBindIP(ptSwitchChannelExt[byIdx].GetRcvBindIP());
			tSwitchChannel.SetSndBindIP(ptSwitchChannelExt[byIdx].GetSndBindIP());

			tSwitchChannel.SetDstIP(ptSwitchChannelExt[byIdx].GetDstIP());
			tSwitchChannel.SetDstPort(ptSwitchChannelExt[byIdx].GetDstPort());

			u16 wMapPort    = htons(ptSwitchChannelExt[byIdx].GetMapPort());
			u32 dwMapIpAddr = htonl(ptSwitchChannelExt[byIdx].GetMapIp());
			
			//发增加交换消息给MP
			cServMsg.CatMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));
			cServMsg.CatMsgBody((u8 *)&dwMapIpAddr, sizeof(dwMapIpAddr));
			cServMsg.CatMsgBody((u8 *)&wMapPort, sizeof(wMapPort));
		}
		if (cServMsg.GetMsgBodyLen() > 0)
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_MSMGR, "Send MCU_MP_ADDMULTITOONESWITCH_REQ to Mp.%d !!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", byMpLoop );
			g_cMpSsnApp.SendMsgToMpSsn( byMpLoop, MCU_MP_ADDMULTITOONESWITCH_REQ, 
				cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		}
	}
    return TRUE; 	
}

/*====================================================================
    函数名      RemoveMultiToOneSwitch
    功能        ：移除多点到一点的交换
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx,							会议索引号索引号
				  u8 byDstNum							目的数
				  TSwitchChannelExt *ptSwitchChannelExt 交换组
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	11/05/06    4.0         周嘉麟          创建
====================================================================*/
BOOL32 CMpManager::RemoveMultiToOneSwitch(u8 byConfIdx, u8 byDstNum, TSwitchChannelExt *ptSwitchChannelExt)
{
	if (0 == byDstNum || NULL == ptSwitchChannelExt)
	{
		return FALSE;
	}

	CServMsg cServMsg;
	cServMsg.SetConfIdx( byConfIdx );

	for (u8 byMpLoop = 1; byMpLoop <= MAXNUM_DRI; byMpLoop ++)
    {
        if ( !g_cMcuVcApp.IsMpConnected(byMpLoop) )
        {
            continue;
        }

		cServMsg.SetDstDriId(byMpLoop);
		cServMsg.SetMsgBody();

		for (u8 byIdx = 0; byIdx < byDstNum; byIdx++)
		{
			u32 dwRcvIp = ptSwitchChannelExt[byIdx].GetRcvIP();
	   	   //根据IP地址查找交换MP编号
			u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	
	       //没找到
	    	if(byMpId==0)
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_MSMGR,"No MP's Addrs is 0x%x in RemoveMultiToOneSwitch(),please check switch recv IP!\n",dwRcvIp);
				continue;
			}
			
			if (byMpId != byMpLoop)
			{
				continue;
			}
			//构造交换信道 
			TSwitchChannel tSwitchChannel;
			tSwitchChannel.SetRcvIP(ptSwitchChannelExt[byIdx].GetRcvIP());
			tSwitchChannel.SetRcvPort(ptSwitchChannelExt[byIdx].GetRcvPort());
			tSwitchChannel.SetDstIP(ptSwitchChannelExt[byIdx].GetDstIP());
			tSwitchChannel.SetDstPort(ptSwitchChannelExt[byIdx].GetDstPort());
			//发增加交换消息给MP
			cServMsg.CatMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));
		}
		if (cServMsg.GetMsgBodyLen() > 0)
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_MSMGR, "Send MCU_MP_REMOVEMULTITOONESWITCH_REQ to Mp.%d !!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", byMpLoop );
			g_cMpSsnApp.SendMsgToMpSsn( byMpLoop, MCU_MP_REMOVEMULTITOONESWITCH_REQ, 
								cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		}
	}	
    return TRUE;
}
/*====================================================================
    函数名      ：RemoveMultiToOneSwitch
    功能        ：移除多点到一点的交换
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx, 会议索引号索引号
                  u32 dwRcvIp, 接收IP
				  u16 wRcvPort, 接收端口
				  u32 dwDstIp, 目的IP
				  u16 wDstPort, 目的端口
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/05/12    3.0         胡昌威        创建
====================================================================*/
BOOL32 CMpManager::RemoveMultiToOneSwitch( u8 byConfIdx, u32 dwRcvIp, u16 wRcvPort, u32 dwDstIp, u16 wDstPort )
{
	//根据IP地址查找交换MP编号
	u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	
	//没找到
	if(byMpId==0)
	{
        LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "No MP's Addrs is 0x%x in RemoveMultiToOneSwitch(),please check switch recv IP!\n",dwRcvIp);
		return FALSE;
	}

	//构造交换信道 
	TSwitchChannel tSwitchChannel;
    tSwitchChannel.SetRcvIP( dwRcvIp );
	tSwitchChannel.SetRcvPort( wRcvPort );
	tSwitchChannel.SetDstIP( dwDstIp );
	tSwitchChannel.SetDstPort( wDstPort );

	//发增加交换消息给MP
	CServMsg cServMsg;
	cServMsg.SetConfIdx( byConfIdx );
	cServMsg.SetDstDriId( byMpId );
	cServMsg.SetMsgBody((u8 *)&tSwitchChannel,sizeof(tSwitchChannel));
	g_cMpSsnApp.SendMsgToMpSsn( byMpId, MCU_MP_REMOVEMULTITOONESWITCH_REQ, 
		                        cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

    return TRUE;
}

/*====================================================================
    函数名      ：StopMultiToOneSwitch
    功能        ：停止多点到一点的交换
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx, 会议索引号索引号
				  u32 dwDstIp, 目的IP
				  u16 wDstPort, 目的端口
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/05/12    3.0         胡昌威        创建
====================================================================*/
BOOL32 CMpManager::StopMultiToOneSwitch( u8 byConfIdx, u32 dwDstIp, u16 wDstPort )
{
    //构造交换信道 
	TSwitchChannel tSwitchChannel;
	tSwitchChannel.SetDstIP(dwDstIp);
	tSwitchChannel.SetDstPort(wDstPort);
	
	//发移除交换消息给MP
	CServMsg cServMsg;
	cServMsg.SetConfIdx( byConfIdx );
	cServMsg.SetMsgBody((u8 *)&tSwitchChannel,sizeof(tSwitchChannel));
	//g_cMpSsnApp.BroadcastToAllMpSsn( MCU_MP_REMOVEMULTITOONESWITCH_REQ,
	//	                             cServMsg.GetServMsg(),cServMsg.GetServMsgLen());

    //MCU_MP_STOPMULTITOONESWITCH_REQ
    g_cMpSsnApp.BroadcastToAllMpSsn(MCU_MP_STOPMULTITOONESWITCH_REQ,
		                            cServMsg.GetServMsg(),cServMsg.GetServMsgLen());

    return TRUE;
}

/*====================================================================
    函数名      ：AddOnlyRecvSwitch
    功能        ：向mp发出ssrc增加只接收交换请求
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx, 会议索引号索引号
				  u32 dwRcvIp, 接收IP
				  u16 wRcvPort, 接收端口
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/05/12    3.0         胡昌威        创建
	20120/5/14	4.7			pgf				modify
====================================================================*/
BOOL32 CMpManager::AddRecvOnlySwitch( u8 byConfIdx, u32 dwSrcIp,u32 dwRcvIp, u16 wRcvPort,u32 dwRtcpBackIp,u16 wRtcpBackPort )
{
	//根据IP地址查找交换MP编号
	u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	
	//没找到
	if(byMpId==0)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "No MP's Addrs is 0x%x in AddRecvOnlySwitch(),please check switch recv IP!\n",dwRcvIp);
		return FALSE;
	}

	//构造交换信道 
	TSwitchChannel tSwitchChannel; 
	tSwitchChannel.SetSrcIp(dwSrcIp);
    tSwitchChannel.SetRcvIP( dwRcvIp );
	tSwitchChannel.SetRcvPort( wRcvPort );
	tSwitchChannel.SetRcvBindIP( dwRcvIp );
	tSwitchChannel.SetDstIP(dwRtcpBackIp);
	tSwitchChannel.SetDstPort(wRtcpBackPort);

	//发增加交换消息给MP
	CServMsg cServMsg;
	cServMsg.SetConfIdx( byConfIdx );
	cServMsg.SetDstDriId( byMpId );
	cServMsg.SetMsgBody((u8   *)&tSwitchChannel,sizeof(tSwitchChannel));
	StaticLog("Send MCU_MP_ADDRECVONLYSWITCH_REQ to mp!!\n");
	g_cMpSsnApp.SendMsgToMpSsn( byMpId, MCU_MP_ADDRECVONLYSWITCH_REQ, 
		                        cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

    return TRUE; 
}
	

/*====================================================================
    函数名      ：RemoveOnlyRecvSwitch
    功能        ：向mp发出ssrc交换移除只接收交换请求
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byConfIdx, 会议索引号索引号
				  u32 dwRcvIp, 接收IP
				  u16 wRcvPort, 接收端口
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/05/12    3.0         胡昌威        创建
====================================================================*/
BOOL32 CMpManager::RemoveRecvOnlySwitch( u8 byConfIdx, u32 dwRcvIp, u16 wRcvPort )
{
	//根据IP地址查找交换MP编号
	u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	
	//没找到
	if(byMpId==0)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "No MP's Addrs is 0x%x in RemoveRecvOnlySwitch(),please check switch recv IP!\n",dwRcvIp);
		return FALSE;
	}

	//构造交换信道 
	TSwitchChannel tSwitchChannel;
    tSwitchChannel.SetRcvIP( dwRcvIp );
	tSwitchChannel.SetRcvPort( wRcvPort );
	tSwitchChannel.SetRcvBindIP( dwRcvIp );

	//发增加交换消息给MP
	CServMsg cServMsg;
	cServMsg.SetConfIdx( byConfIdx );
	cServMsg.SetDstDriId( byMpId );
	cServMsg.SetMsgBody((u8   *)&tSwitchChannel,sizeof(tSwitchChannel));
	g_cMpSsnApp.SendMsgToMpSsn( byMpId, MCU_MP_REMOVERECVONLYSWITCH_REQ, 
		                        cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

    return TRUE; 
}

/*=============================================================================
    函 数 名： StartRecvMt
    功    能： 开始接收终端数据（建立只接收交换，扔至dump）
    算法实现： 
    全局变量： 
    参    数： const TMt & tMt, 要接收的终端
	           u8 byMode, 交换模式，缺省为MODE_BOTH
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/03/02  3.6			万春雷                  创建
=============================================================================*/
BOOL32 CMpManager::StartRecvMt( const TMt &tMt, u8 byMode )
{
	u32  dwSrcIp = 0;
	u32  dwRcvIp = 0;
	u16  wRcvPort = 0;
	BOOL32 bResult1 = TRUE;
	BOOL32 bResult2 = TRUE;
		
	//得到交换源地址
	if( !GetSwitchInfo( tMt, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
		 LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartRecvMt() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//图像
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		//建立dump交换
		bResult1 = AddRecvOnlySwitch( tMt.GetConfIdx(),dwSrcIp, dwRcvIp, wRcvPort );
		if(!bResult1)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Switch video dump failure!\n");
		}
	}
		
	//语音
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		//建立dump交换,对于外厂商终端，需要将rtcp的ip和port告知mp
		CMcuVcInst * pcVcInst = g_cMcuVcApp.GetConfInstHandle(tMt.GetConfIdx());
		if(NULL == pcVcInst)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "[StartRecvMt]pcVcInst(ConfIdx.%d) is null!\n", tMt.GetConfIdx());
			return FALSE;
		}
		TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable(tMt.GetConfIdx());
		if (ptConfMtTable == NULL)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartRecvMt() GetConfMtTable failure, Type: %d SubType: %d Id: %d ConfIdx: %d!\n", 
				tMt.GetType(), tMt.GetEqpType(), tMt.GetEqpId(),tMt.GetConfIdx());
			return FALSE;
		}
		//tMt是localmt，不用考虑非local
		u32 dwRtcpBackIp = 0;
		u16 wRtcpBackPort = 0;
		if ( MT_MANU_KDC !=  ptConfMtTable->GetManuId(tMt.GetMtId()) && 
			MT_MANU_KDCMCU != ptConfMtTable->GetManuId(tMt.GetMtId()) )
		{
			//外厂商终端
			TLogicalChannel tLogicChannel;
			if (ptConfMtTable->GetMtLogicChnnl(tMt.GetMtId(),LOGCHL_AUDIO,&tLogicChannel,FALSE))
			{
				dwRtcpBackIp = tLogicChannel.GetSndMediaCtrlChannel().GetIpAddr();
				wRtcpBackPort = tLogicChannel.GetSndMediaCtrlChannel().GetPort();
			}
			
			
		}
		StaticLog("dwRtcpBackIp:%d,wRtcpBackPort:%d\n",dwRtcpBackIp,wRtcpBackPort);
		bResult2 = AddRecvOnlySwitch( tMt.GetConfIdx(), dwSrcIp,dwRcvIp, wRcvPort+2 ,dwRtcpBackIp,wRtcpBackPort);
		if(!bResult2)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Switch audio dump failure!\n");
		}
	}	
	
	return bResult1&&bResult2;
}

/*=============================================================================
    函 数 名： StopRecvMt
    功    能： 停止接收终端数据（拆除只接收交换）
    算法实现： 
    全局变量： 
    参    数： const TMt & tMt, 要接收的终端
	           u8 byMode, 交换模式，缺省为MODE_BOTH
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/03/02  3.6			万春雷                  创建
=============================================================================*/
BOOL32 CMpManager::StopRecvMt( const TMt & tMt, u8 byMode )
{
	u32  dwSrcIp = 0;
	u32  dwRcvIp = 0;
	u16  wRcvPort = 0;
	BOOL32 bResult1 = TRUE;
	BOOL32 bResult2 = TRUE;
		
	//得到交换源地址
	if( !GetSwitchInfo( tMt, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
		 LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StopRecvMt() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//图像
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		//拆除dump交换
		bResult1 = RemoveRecvOnlySwitch( tMt.GetConfIdx(), dwRcvIp, wRcvPort );
		if(!bResult1)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Stop switch video dump failure!\n");
		}
	}
		
	//语音
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		//拆除dump交换
		bResult2 = RemoveRecvOnlySwitch( tMt.GetConfIdx(), dwRcvIp, wRcvPort+2 );
		if(!bResult2)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Stop switch audio dump failure!\n");
		}
	}

	return bResult1&&bResult2;
}



/*=============================================================================
    函 数 名： SetRecvMtSSRCValue
    功    能： 向mp发出ssrc变更请求
    算法实现： 
    全局变量： 
    参    数： u8   byConfIdx 会议索引号索引号
               u32  dwRcvIp   接收IP
               u16  wRcvPort  接收端口
			   BOOL32 bReset    是否进行SSRC变更，FALSE-恢复原有值
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/03/06  3.6			万春雷                  创建
=============================================================================*/
/*
//zbq[07/18/2007] SSRC 的修改由归一重整统一处理
BOOL32 CMpManager::SetRecvMtSSRCValue( u8 byConfIdx, u32 dwRcvIp, u16 wRcvPort, BOOL32 bReset )
{
	//根据IP地址查找交换MP编号
	u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	
	//没找到
	if(byMpId==0)
	{
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "No MP's Addrs is 0x%x in SetRecvMtSSRCValue(),please check switch recv IP!\n",dwRcvIp);
		return FALSE;
	}

	//设置SSRC变更值 1－变更，否则恢复原有SSRC Value
	u8 byChangeSSRC = 0;
	if( TRUE == bReset )
	{
		byChangeSSRC = 1;
	}

	//构造交换信道 
	TSwitchChannel tSwitchChannel;
    tSwitchChannel.SetRcvIP( dwRcvIp );
	tSwitchChannel.SetRcvPort( wRcvPort );
	tSwitchChannel.SetRcvBindIP( dwRcvIp );

	//发增加交换消息给MP
	CServMsg cServMsg;
	cServMsg.SetConfIdx( byConfIdx );
	cServMsg.SetDstDriId( byMpId );
	cServMsg.SetMsgBody( (u8 *)&tSwitchChannel, sizeof(tSwitchChannel) );
	cServMsg.CatMsgBody( &byChangeSSRC, sizeof(u8) );
	g_cMpSsnApp.SendMsgToMpSsn( byMpId, MCU_MP_SETRECVSWITCHSSRC_REQ, 
		                        cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );

    return TRUE; 
}
*/

/*=============================================================================
    函 数 名： ResetRecvMtSSRC
    功    能： 设置对某路接收码流中SSRC作改动
    算法实现： 
    全局变量： 
    参    数： const TMt & tMt, 要接收的终端
	           u8 byMode, 交换模式，缺省为MODE_BOTH
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/03/06  3.6			万春雷                  创建
=============================================================================*/
/*
//zbq[07/18/2007] SSRC 的修改由归一重整统一处理
BOOL32 CMpManager::ResetRecvMtSSRC( const TMt &tMt, u8 byMode )
{
	u32  dwSrcIp  = 0;
	u32  dwRcvIp  = 0;
	u16  wRcvPort = 0;
	BOOL32 bResult1 = TRUE;
	BOOL32 bResult2 = TRUE;
		
	//得到交换源地址
	if( !GetSwitchInfo( tMt, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
		 LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "ResetRecvMtSSRC() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//图像
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		bResult1 = SetRecvMtSSRCValue( tMt.GetConfIdx(), dwRcvIp, wRcvPort, TRUE );
		if(!bResult1)
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "ResetRecvMtSSRC video switch failure!\n");
		}
	}
	//语音
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		bResult2 = SetRecvMtSSRCValue( tMt.GetConfIdx(), dwRcvIp, wRcvPort+2, TRUE );
		if(!bResult2)
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "ResetRecvMtSSRC audio switch failure!\n");
		}
	}	
	return bResult1&&bResult2;
}*/

/*=============================================================================
    函 数 名： RestoreRecvMtSSRC
    功    能： 取消对码流中SSRC作改动
    算法实现： 
    全局变量： 
    参    数： const TMt & tMt, 要接收的终端
	           u8 byMode, 交换模式，缺省为MODE_BOTH
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/03/06  3.6			万春雷                  创建
=============================================================================*/
/*
//zbq[07/18/2007]
BOOL32 CMpManager::RestoreRecvMtSSRC( const TMt & tMt, u8 byMode )
{
	u32  dwSrcIp  = 0;
	u32  dwRcvIp  = 0;
	u16  wRcvPort = 0;
	BOOL32 bResult1 = TRUE;
	BOOL32 bResult2 = TRUE;
		
	//得到交换源地址
	if( !GetSwitchInfo( tMt, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
		 LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "RestoreRecvMtSSRC() failure because of can't get switch info!\n");
		 return FALSE;
	}

	//图像
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		bResult1 = SetRecvMtSSRCValue( tMt.GetConfIdx(), dwRcvIp, wRcvPort, FALSE );
		if(!bResult1)
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "RestoreRecvMtSSRC video switch failure!\n");
		}
	}
		
	//语音
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		bResult2 = SetRecvMtSSRCValue( tMt.GetConfIdx(), dwRcvIp, wRcvPort+2, FALSE );
		if(!bResult2)
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "RestoreRecvMtSSRC audio switch failure!\n");
		}
	}

	return bResult1&&bResult2;
}*/

/*====================================================================
    函数名      ：ProcMpToMcuMessage
    功能        ：处理来自Mp的消息
    算法实现    ：
    引用全局变量：
    输入参数说明：pcMsg 消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/16    1.0         胡昌威        创建
====================================================================*/
void CMpManager::ProcMpToMcuMessage(const CMessage * pcMsg)
{
    switch( pcMsg->event ) 
	{
	  case MP_MCU_REG_REQ:
		  ProcMpRegReq(pcMsg);
		  break;
	  case MCU_MP_DISCONNECTED_NOTIFY:
		  ProcMpDisconnect(pcMsg);
          break;
	  default:
		  break;
	}  
}

/*====================================================================
    函数名      ：ProcMpRegReq
    功能        ：处理Mp注册消息
    算法实现    ：
    引用全局变量：
    输入参数说明：pcMsg 消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/17    1.0         胡昌威        创建
====================================================================*/
void CMpManager::ProcMpRegReq(const CMessage * pcMsg)
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);	
	TMp tMp = *(TMp*)cServMsg.GetMsgBody();

	//[pengguofeng 5/10/2012]IPV6 
	u8 byIpType = IP_NONE;
	if ( tMp.GetVersion() == DEVVER_MPV6 )
	{
		u8 abyIpV6[IPV6_STR_LEN] = {0};
		u8 abyIpV6Null[IPV6_STR_LEN];
		memset(abyIpV6Null, 0, sizeof(abyIpV6Null));
		//能取这么多内容，是因为在应答时，校验过内容长度了
		memcpy(abyIpV6, cServMsg.GetMsgBody() + sizeof(TMp), sizeof(abyIpV6));
		
		if ( memcmp(abyIpV6, abyIpV6Null, sizeof(abyIpV6)) != 0 )
		{
			if ( tMp.GetIpAddr() != 0)
			{
				byIpType = IP_V4V6;
			}
			else
				byIpType = IP_V6;
		}
		else if ( tMp.GetIpAddr() != 0)
		{
			byIpType = IP_V4;
		}
	}

	//[pengguofeng 5/12/2012]老版本直接设成V4
	if ( tMp.GetVersion() == DEVVER_MP )
	{
		byIpType = IP_V4;
	}

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[MpMcuReg]version:%d iptype:%d\n", tMp.GetVersion(), byIpType);
    //multicast
//    u8 byNetNo = g_cMcuAgent.GetCriDriNetChoice(tMp.GetMpId());
    u8 byIsMulticast = g_cMcuAgent.GetCriDriIsCast(tMp.GetMpId()) ? 1:0;
//    tMp.SetNetNo(byNetNo);
    tMp.SetMulticast(byIsMulticast);

    g_cMcuVcApp.AddMp(tMp);

	g_cMcuVcApp.SetMpIpType(tMp.GetMpId(), byIpType);

	g_cMcuVcApp.BroadcastToAllConf(MP_MCU_REG_REQ, pcMsg->content, pcMsg->length);
}

/*====================================================================
    函数名      ：ProcMpDisconnect
    功能        ：处理Mp断链消息
    算法实现    ：
    引用全局变量：
    输入参数说明：pcMsg 消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/17    1.0         胡昌威        创建
====================================================================*/
void CMpManager::ProcMpDisconnect(const CMessage * pcMsg)
{
    CServMsg cServMsg( pcMsg->content ,pcMsg->length );	
	TMp tMp = *(TMp*)cServMsg.GetMsgBody();
	g_cMcuVcApp.RemoveMp( tMp.GetMpId() );
	
	//zhouyiliang 20130403 主备转发板备份
//	if( !g_cMSSsnApp.IsDoubleLink() )
//	{
	g_cMcuVcApp.AssignNewMpToReplace(tMp.GetMpId());
//	}
	g_cMcuVcApp.BroadcastToAllConf( MCU_MP_DISCONNECTED_NOTIFY, pcMsg->content, pcMsg->length );


}

/*====================================================================
    函数名      ：ProcMtAdpToMcuMessage
    功能        ：处理来自Mtadp的消息
    算法实现    ：
    引用全局变量：
    输入参数说明：pcMsg 消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/16    3.0         胡昌威        创建
====================================================================*/
void CMpManager::ProcMtAdpToMcuMessage(const CMessage * pcMsg)
{
    switch( pcMsg->event ) 
	{
	  case MTADP_MCU_REGISTER_REQ:
		  ProcMtAdpRegReq(pcMsg);
		  break;
	  case MCU_MTADP_DISCONNECTED_NOTIFY:
		  ProcMtAdpDisconnect(pcMsg);
		  break;
	  default:
		  break;
	}  
}


/*====================================================================
    函数名      ：ProcMtAdpRegReq
    功能        ：处理MtAdp注册消息
    算法实现    ：
    引用全局变量：
    输入参数说明：pcMsg 消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/17    1.0         胡昌威        创建
====================================================================*/
void CMpManager::ProcMtAdpRegReq(const CMessage * pcMsg)
{
	CServMsg cServMsg( pcMsg->content ,pcMsg->length );	
	TMtAdpReg tMtAdpReg =  *( TMtAdpReg* )cServMsg.GetMsgBody();

	for( u8 byLoop = MIN_CONFIDX; byLoop <= MAX_CONFIDX; byLoop++ )
	{			
		TConfMapData tTempConfMapData = g_cMcuVcApp.GetConfMapData(byLoop);
		if( !tTempConfMapData.IsValidConf() )
		{
			continue;
		}

		CMcuVcInst* pcVcInst = NULL;
		pcVcInst = g_cMcuVcApp.GetConfInstHandle( byLoop );
		if( pcVcInst == NULL )
		{
			continue;
		}

		for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
		{
			//含不在线终端
			if ( !pcVcInst->m_tConfAllMtInfo.MtInConf(byMtId) )
			{
				continue;
			}
			if (NULL == pcVcInst->m_ptMtTable)
			{
				continue;
			}
			if (pcVcInst->m_ptMtTable->GetDisconnectDRI(byMtId) == tMtAdpReg.GetDriId())
			{
				pcVcInst->m_ptMtTable->SetDisconnectDRI(byMtId, 0);
			}
		}
	}

	//同步此时的注册MCU及会议实体 注册信息 到此适配板
	if( g_cMcuAgent.GetGkIpAddr()  && 0 != g_cMcuVcApp.GetRegGKDriId() && 
		tMtAdpReg.GetDriId() != g_cMcuVcApp.GetRegGKDriId() && 
		PROTOCOL_TYPE_H323 == tMtAdpReg.GetProtocolType() )
	{
		u8 byConfIdx  = 0; //0－mcu，1－MAXNUM_MCU_CONF 会议
		u8 byRegState = 0; //0－未注册，1－成功注册
		TMtAlias tMtAlias;
		char achMcuAlias[MAXLEN_ALIAS];
		g_cMcuAgent.GetE164Number( achMcuAlias, MAXLEN_ALIAS );

		byRegState = g_cMcuVcApp.GetConfRegState( byConfIdx );	
		tMtAlias.SetE164Alias( achMcuAlias );
		cServMsg.SetMcuId( LOCAL_MCUID );
		//cServMsg.SetConfIdx( 0 );
		cServMsg.SetMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
		cServMsg.CatMsgBody( (u8*)&byConfIdx, sizeof( byConfIdx ) );
		cServMsg.CatMsgBody( (u8*)&byRegState, sizeof( byRegState ) );

		for( byConfIdx=MIN_CONFIDX; byConfIdx<=MAX_CONFIDX; byConfIdx++ )
		{			
            TConfInfo* ptConfInfo = NULL;
            TTemplateInfo tTemInfo;
            
            TConfMapData tMapData = g_cMcuVcApp.GetConfMapData(byConfIdx);
            if(tMapData.IsValidConf())
            {
				CMcuVcInst* pcVcInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
	
                if( NULL == pcVcInst )
                {
                    continue;
                }
                else
                {
                    ptConfInfo = &pcVcInst->m_tConf;
                }
            }
            else if(tMapData.IsTemUsed())
            {
                if(!g_cMcuVcApp.GetTemplate(byConfIdx, tTemInfo))
                {
                    continue;
                }
                else
                {
                    ptConfInfo = &tTemInfo.m_tConfInfo;
                }
            }
            else
            {
                continue;
            }
				
			byRegState = g_cMcuVcApp.GetConfRegState( byConfIdx );
			tMtAlias.SetE164Alias( ptConfInfo->GetConfE164() );
			cServMsg.SetConfId( ptConfInfo->GetConfId() );
			//cServMsg.SetConfIdx( byConfIdx );
			cServMsg.CatMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
			cServMsg.CatMsgBody( (u8*)&byConfIdx, sizeof( byConfIdx ) );
			cServMsg.CatMsgBody( (u8*)&byRegState, sizeof( byRegState ) );
		}
	
		u8 byNewDri = tMtAdpReg.GetDriId();
		cServMsg.SetDstDriId( byNewDri );
		g_cMtAdpSsnApp.SendMsgToMtAdpSsn( byNewDri, MCU_MT_UPDATE_REGGKSTATUS_NTF, cServMsg );

		cServMsg.SetMsgBody( (u8*)g_cMcuVcApp.GetH323GKIDAlias(), sizeof( g_cMcuVcApp.m_tGKID ) );
		cServMsg.CatMsgBody( (u8*)g_cMcuVcApp.GetH323EPIDAlias(), sizeof( g_cMcuVcApp.m_tEPID ) );
		g_cMtAdpSsnApp.SendMsgToMtAdpSsn( byNewDri, MCU_MT_UPDATE_GKANDEPID_NTF, cServMsg );
	}

	return;
}
/*lint -restore*/
/*====================================================================
    函数名      ：ProcMpDisconnect
    功能        ：mtadpssn osp断链消息处理中调用的处理MtAdp断链函数
    算法实现    ：
    引用全局变量：
    输入参数说明：pcMsg 消息
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/17    1.0         胡昌威        创建
====================================================================*/
/*lint -save -e850*/
void CMpManager::ProcMtAdpDisconnect(const CMessage * pcMsg)
{
	CServMsg cServMsg( pcMsg->content ,pcMsg->length );	
	TMtAdpReg tMtAdpReg =  *( TMtAdpReg* )cServMsg.GetMsgBody();
	u8 byRRQReset = *( cServMsg.GetMsgBody() + sizeof(TMtAdpReg) );
	g_cMcuVcApp.BroadcastToAllConf( MCU_MTADP_DISCONNECTED_NOTIFY, pcMsg->content, pcMsg->length );

	if( 1 == byRRQReset )
	{
		u8 byMinDri   = 0;
		u8 byConfIdx  = 0; //0－mcu，1－MAXNUM_MCU_CONF 会议
		u8 byRegState = 0; //0－未注册，1－成功注册
		TMtAlias tMtAlias;
		char achMcuAlias[MAXLEN_ALIAS];
		g_cMcuAgent.GetE164Number( achMcuAlias, MAXLEN_ALIAS );

		//清空会议注册状态
		g_cMcuVcApp.SetConfRegState( byConfIdx, byRegState );
		
		tMtAlias.SetE164Alias( achMcuAlias );
		cServMsg.SetMcuId( LOCAL_MCUID );
		//cServMsg.SetConfIdx( 0 );
		cServMsg.SetMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
		cServMsg.CatMsgBody( (u8*)&byConfIdx, sizeof( byConfIdx ) );
		cServMsg.CatMsgBody( (u8*)&byRegState, sizeof( byRegState ) );
        
        for( byConfIdx=MIN_CONFIDX; byConfIdx<=MAX_CONFIDX; byConfIdx++ )
        {			
            TConfInfo* ptConfInfo = NULL;
            TTemplateInfo tTemInfo;

            g_cMcuVcApp.SetConfRegState( byConfIdx, byRegState );
            
            TConfMapData tMapData = g_cMcuVcApp.GetConfMapData(byConfIdx);
            if(tMapData.IsValidConf())
            {
				CMcuVcInst* pcVcInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
                if( NULL == pcVcInst )
                {
                    continue;
                }
                else
                {
                    ptConfInfo = &pcVcInst->m_tConf;

                    // 对于GK计费会议特别处理, zgc, 2008-09-26
                    // 等待主适配板注册上来后，重新开始注册计费会议
					if(ptConfInfo)
						ptConfInfo->m_tStatus.SetGkCharge( FALSE );
                }
            }
            else if(tMapData.IsTemUsed())
            {
                if(!g_cMcuVcApp.GetTemplate(byConfIdx, tTemInfo))
                {
                    continue;
                }
                else
                {
                    ptConfInfo = &tTemInfo.m_tConfInfo;
                }
            }
            else
            {
                continue;
            }

            tMtAlias.SetE164Alias( ptConfInfo->GetConfE164() );
            cServMsg.SetConfId( ptConfInfo->GetConfId() );
            //cServMsg.SetConfIdx( byConfIdx );
            cServMsg.CatMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
            cServMsg.CatMsgBody( (u8*)&byConfIdx, sizeof( byConfIdx ) );
            cServMsg.CatMsgBody( (u8*)&byRegState, sizeof( byRegState ) );
        }	

		//清空其他适配模块板的 注册MCU及会议实体 注册信息
		for( u8 byLoop = 1; byLoop <= MAXNUM_DRI; byLoop++)
		{
			if( g_cMcuVcApp.IsMtAdpConnected( byLoop ) && 
				PROTOCOL_TYPE_H323 == g_cMcuVcApp.GetMtAdpProtocalType(byLoop) )
			{
				cServMsg.SetDstDriId( byLoop );
				g_cMtAdpSsnApp.SendMsgToMtAdpSsn( byLoop, MCU_MT_UPDATE_REGGKSTATUS_NTF, cServMsg );

				if( 0 == byMinDri )
				{
					byMinDri = byLoop;
				}
			}
		}
		//  [12/18/2009 pengjie] Modify
		if( 0 == byMinDri)
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[ProcMtAdpDisconnect] Don't Find another Idle Mtadp!\n");
			return;
		}
		// End Modify
		
		//  [12/26/2009 pengjie] 伪装注销暂时注掉
		/*
		//用于注册的适配模块板断链，则进行注册迁移，重新选择适配模块板 注册MCU及会议实体
		cServMsg.SetDstDriId( byMinDri );
		if( 0 != byMinDri && byMinDri <= MAXNUM_DRI )
		{
            //1、刷新主接入地址（注：mtadp的状态刷新放在下一步“伪装注销”完成）
            u32 dwOrgRRQIp = g_cMcuAgent.GetRRQMtadpIp();
            u32 dwReplaceRRQIp = g_cMcuVcApp.GetMtAdpIpAddr(byMinDri);
            g_cMcuAgent.SetRRQMtadpIp(dwReplaceRRQIp);
            g_cMcuVcApp.SetRegGKDriId(byMinDri);
			
            //2、伪装注销
            s8 achMcuE164[MAXLEN_ALIAS];
            memset(achMcuE164, 0, MAXLEN_ALIAS);
            g_cMcuAgent.GetE164Number(achMcuE164, MAXLEN_ALIAS);
			
            TRASInfo tRASInfo;
            tRASInfo.SetEPID(&g_cMcuVcApp.m_tEPID);
            tRASInfo.SetGKID(&g_cMcuVcApp.m_tGKID);
            tRASInfo.SetGKIp(ntohl(g_cMcuAgent.GetGkIpAddr()));
            tRASInfo.SetMcuE164(achMcuE164);
            tRASInfo.SetRRQIp(dwOrgRRQIp);            
			
            CServMsg cServMsg;
            cServMsg.SetConfIdx( 0 );
            cServMsg.SetEventId( MCU_MT_UNREGGK_NPLUS_CMD );
            cServMsg.SetMsgBody( (u8*)&tRASInfo, sizeof(TRASInfo) );
            
            g_cMtAdpSsnApp.SendMsgToMtAdpSsn( byMinDri, 
				MCU_MT_UNREGGK_NPLUS_CMD, cServMsg );
			
            //3、重新注册（注：重新注册在“伪装注销”内将连带完成）
            
            //3.1 在GK上注册MCU
            g_cMcuVcApp.SetRegGKDriId( byMinDri );
            g_cMcuVcApp.RegisterConfToGK( 0, byMinDri );
            
            //3.2 在GK上注册会议实体(包括会议和模板)
            for( u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++ )
            {
                //注册模板
                TConfMapData tConfMapData = g_cMcuVcApp.GetConfMapData( byConfIdx );
                if ( tConfMapData.IsTemUsed() )
                {
                    g_cMcuVcApp.RegisterConfToGK( byConfIdx, byMinDri, TRUE, FALSE );
                }
                //注册会议
                else if ( tConfMapData.IsValidConf() )
                {
                    g_cMcuVcApp.RegisterConfToGK( byConfIdx, byMinDri, FALSE, FALSE );
                }
            }
			
            //4. 置标志位 用以重呼终端
            g_cMcuVcApp.SetRRQDriTransed( TRUE );
		}
		*/
		//清空GKID/EPID信息
        memset((void*)&g_cMcuVcApp.m_tGKID, 0, sizeof(TH323EPGKIDAlias));
        memset((void*)&g_cMcuVcApp.m_tEPID, 0, sizeof(TH323EPGKIDAlias));
	}
	else
	{
		if (g_cMcuVcApp.GetRegGKDriId() != 0)
		{
			g_cMcuVcApp.RegisterConfToGK( 0, g_cMcuVcApp.GetRegGKDriId() );
        }
	}
	
	return;
}
/*lint -restore*/

/*=============================================================================
    函 数 名： UpdateRRQInfoAfterURQ
    功    能： 注销的会议注册记录后 同步 其他适配模块板的 注册MCU及会议实体 注册信息
    算法实现： 
    全局变量： 
    参    数： u8 byURQConfIdx
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/16   3.6			万春雷                  创建
=============================================================================*/
/*lint -save -e850*/
void CMpManager::UpdateRRQInfoAfterURQ( u8 byURQConfIdx )
{
	if( g_cMcuAgent.GetGkIpAddr() && 0 != g_cMcuVcApp.GetRegGKDriId() )
	{
		CServMsg cServMsg;
		TMtAlias tMtAlias;
		u8 byConfIdx  = 0; //0－mcu，1－MAXNUM_MCU_CONF 会议
		u8 byRegState = 0; //0－未注册，1－成功注册
		char achMcuAlias[MAXLEN_ALIAS];
		g_cMcuAgent.GetE164Number( achMcuAlias, MAXLEN_ALIAS );

		byRegState = g_cMcuVcApp.GetConfRegState( byConfIdx );
		tMtAlias.SetNull();
		tMtAlias.SetE164Alias( achMcuAlias );
		cServMsg.SetMcuId( LOCAL_MCUID );
		//cServMsg.SetConfIdx( 0 );
		cServMsg.SetMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
		cServMsg.CatMsgBody( (u8*)&byConfIdx, sizeof( byConfIdx ) );
		cServMsg.CatMsgBody( (u8*)&byRegState, sizeof( byRegState ) );

        for( byConfIdx=MIN_CONFIDX; byConfIdx<=MAX_CONFIDX; byConfIdx++ )
        {			
            TConfInfo* ptConfInfo = NULL;
            TTemplateInfo tTemInfo;
            
            TConfMapData tMapData = g_cMcuVcApp.GetConfMapData(byConfIdx);
            if(tMapData.IsValidConf())
            {
				CMcuVcInst* pcVcInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
                if( NULL == pcVcInst )
                {
                    continue;
                }
                else
                {
                    ptConfInfo = &pcVcInst->m_tConf;
                }
            }
            else if(tMapData.IsTemUsed())
            {
                if(!g_cMcuVcApp.GetTemplate(byConfIdx, tTemInfo))
                {
                    continue;
                }
                else
                {
                    ptConfInfo = &tTemInfo.m_tConfInfo;
                }
            }
            else
            {
                continue;
            }

            //清空注销的会议注册记录
            if( byURQConfIdx == byConfIdx )
            {
                g_cMcuVcApp.SetConfRegState( byConfIdx, 0 );
                byRegState = 0;
                tMtAlias.SetNull();
            }
            else
            {
                byRegState = g_cMcuVcApp.GetConfRegState( byConfIdx );
                tMtAlias.SetE164Alias( ptConfInfo->GetConfE164() );
            }
            cServMsg.SetConfId( ptConfInfo->GetConfId() );
            //cServMsg.SetConfIdx( byConfIdx );
            cServMsg.CatMsgBody( (u8*)&tMtAlias, sizeof( tMtAlias ) );
            cServMsg.CatMsgBody( (u8*)&byConfIdx, sizeof( byConfIdx ) );
            cServMsg.CatMsgBody( (u8*)&byRegState, sizeof( byRegState ) );
        }		
		
		//注销的会议注册记录后 同步 其他适配模块板的 注册MCU及会议实体 注册信息
		for( u8 byLoop = 1; byLoop <= MAXNUM_DRI; byLoop++)
		{
			if( byLoop != g_cMcuVcApp.GetRegGKDriId() &&
				g_cMcuVcApp.IsMtAdpConnected( byLoop ) && 
				PROTOCOL_TYPE_H323 == g_cMcuVcApp.GetMtAdpProtocalType(byLoop) )
			{
				cServMsg.SetDstDriId( byLoop );
				g_cMtAdpSsnApp.SendMsgToMtAdpSsn( byLoop, MCU_MT_UPDATE_REGGKSTATUS_NTF, cServMsg );
			}
		}
	}

	return;
}
/*lint -restore*/

/*====================================================================
    函数名      ：ShowMp
    功能        ：屏幕打印所有Mp信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/17    1.0         胡昌威        创建
====================================================================*/	
void CMpManager::ShowMp()
{
	s32  nMpSum = 0;
	u8 byLoop;
	s8 achMcuAlias[32];
	g_cMcuAgent.GetMcuAlias( achMcuAlias, 32 );
    StaticLog("\n------------------MCU %s register MP list----------------", achMcuAlias );
	for( byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		if(g_cMcuVcApp.m_atMpData[byLoop].m_bConnected)
		{
            StaticLog( "\nMp%d:[IpType:%d] %s (Band: %d/%d[Total/Real] P.K, NetTraffic: %d/%d/%d[Total/Real/Reserved] Mbps)", 
                                  byLoop+1, g_cMcuVcApp.m_atMpData[byLoop].GetIpType(),
				                  StrOfIP( g_cMcuVcApp.m_atMpData[byLoop].m_tMp.GetIpAddr( ) ) ,
                                  g_cMcuVcApp.m_atMpData[byLoop].m_dwPkNumAllowed,
                                  g_cMcuVcApp.m_atMpData[byLoop].m_dwPkNumReal,
                                  g_cMcuVcApp.m_atMpData[byLoop].m_wNetBandAllowed,
                                  g_cMcuVcApp.m_atMpData[byLoop].m_wNetBandReal,
                                  g_cMcuVcApp.m_atMpData[byLoop].m_wNetBandReserved
                      );
			nMpSum++;
		}		
	}
	StaticLog("\nTotal Register MP num: %d \n",nMpSum);
}

/*====================================================================
    函数名      ：ShowMtAdp
    功能        ：屏幕打印所有MtAdp信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	04/06/01    3.0         胡昌威        创建
====================================================================*/	
void CMpManager::ShowMtAdp()
{
	s32  nMtAdpSum = 0;
	u8 byLoop;
	s8 achMcuAlias[32];
	g_cMcuAgent.GetMcuAlias( achMcuAlias, 32 );
    u8 byMtNum = 0;
    u8 bySMcuNum = 0;
    StaticLog("\n------------------MCU %s register MtAdp list----------------", achMcuAlias );
	for( byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		if( g_cMcuVcApp.m_atMtAdpData[byLoop].m_bConnected )
		{
            byMtNum = 0;
            bySMcuNum = 0;
            g_cMcuVcApp.GetMtNumOnDri(byLoop+1, TRUE, byMtNum, bySMcuNum);
            StaticLog( "\nMtAdp%d:[IpType:%d] %s (MaxMT/SMcu/HDMt/AUDMt ||OnlineMt/SMcu/All: %d/%d/%d/%d||%d/%d/%d)", byLoop+1,
						g_cMcuVcApp.m_atMtAdpData[byLoop].m_byIpType,
				       StrOfIP( g_cMcuVcApp.GetMtAdpIpAddr( byLoop+1 ) ),
                       g_cMcuVcApp.m_atMtAdpData[byLoop].m_byMaxMtNum,
                       g_cMcuVcApp.m_atMtAdpData[byLoop].m_byMaxSMcuNum,
					   g_cMcuVcApp.m_atMtAdpData[byLoop].m_byMaxHDMtNum,
					   g_cMcuVcApp.m_atMtAdpData[byLoop].m_wMaxAudMtNum,
                       byMtNum,
                       bySMcuNum,
                       g_cMcuVcApp.m_atMtAdpData[byLoop].m_wMtNum);
            
			nMtAdpSum++;
		}		
	}
	StaticLog( "\n Current HD Access Point Num : %d \n Total HD Access Point Num :%d \n Total Register MtAdp num: %d",
		g_cMcuVcApp.m_wCurrentHDMtNum, g_cMcuVcApp.GetMpcHDAccessNum(), nMtAdpSum);

	StaticLog( "\n Current Aud Access Point Num : %d \n Total Aud Access Point Num :%d!\n", 
				g_cMcuVcApp.m_wCurrentAudMtNum, g_cMcuVcApp.GetMpcCurrentAudMtAdpNum());
}


/*====================================================================
    函数名      ：ShowBridge
    功能        ：屏幕打印交换桥
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/17    1.0         胡昌威        创建
====================================================================*/	
void CMpManager::ShowBridge()
{
	TConfInfo  *ptConfFullInfo = NULL;
	TConfSwitchTable *ptSwitchTable;
	u8  byMpId, byDstMpId, byLoop1;
    u16 wLoop2;
	u32 dwSrcIp, dwDisIp, dwRcvIp, dwDstIp;
	u16 wRcvPort, wDstPort;
	TSwitchChannel *ptSwitchChannel;
	s8  achTemp[255];
	s32 nLen;

	for( u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++ )
	{ 
		//依次发送每个会议的状态通知
		CMcuVcInst* pcVcInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
        if(NULL == pcVcInst)
        {
            continue;
        }

        ptConfFullInfo = &pcVcInst->m_tConf;
		if( ptConfFullInfo && ptConfFullInfo->m_tStatus.IsOngoing() )
		{   
			StaticLog("\n--------Conf: %s bridge info--------\n", ptConfFullInfo->GetConfName() );
		    ptSwitchTable = g_cMcuVcApp.GetConfSwitchTable( byConfIdx );
			for( byLoop1 = 1; byLoop1 <= MAXNUM_DRI; byLoop1++ )
			{
				if( g_cMcuVcApp.IsMpConnected( byLoop1 ) )
				{
					StaticLog("\nMp%d switch info: ", byLoop1 );
					for( wLoop2 = 0; wLoop2 < MAX_SWITCH_CHANNEL; wLoop2++ )
					{
						ptSwitchChannel = ptSwitchTable->GetSwitchChannel( byLoop1, wLoop2 );
						if( !ptSwitchChannel->IsNull() && ptSwitchChannel->GetRcvPort() == ptSwitchChannel->GetDstPort() )
						{
							dwDstIp = ptSwitchChannel->GetDstIP();
							byDstMpId = g_cMcuVcApp.FindMp( dwDstIp );
                            if( byDstMpId > 0 )
							{							
								dwSrcIp = ptSwitchChannel->GetSrcIp();
								dwDisIp = ptSwitchChannel->GetDisIp();
								dwRcvIp = ptSwitchChannel->GetRcvIP();
								wRcvPort = ptSwitchChannel->GetRcvPort();
								wDstPort = ptSwitchChannel->GetDstPort();

								memset( achTemp, 0, 255 );
							    nLen = 0;

								//SrcIp
								byMpId = g_cMcuVcApp.FindMp( dwSrcIp );
								if( byMpId > 0 )
								{
									nLen = sprintf( achTemp, "\n%d  %s:%d(mp%d)", wLoop2, StrOfIP( dwSrcIp ), wRcvPort, byMpId );
								}
								else
								{
									nLen = sprintf( achTemp, "\n%d  %s:%d(src)", wLoop2, StrOfIP( dwSrcIp ), wRcvPort );
								}					
								
								//DisIp
								if( dwDisIp > 0 )
								{
									byMpId = g_cMcuVcApp.FindMp( dwDisIp );
									if( byMpId > 0 )
									{
										nLen += sprintf( achTemp+nLen, "-->%s:%d(mp%d)", StrOfIP( dwDisIp ), wRcvPort, byMpId );
									}
									else
									{
										nLen += sprintf( achTemp+nLen, "-->%s:%d(src)", StrOfIP( dwDisIp ), wRcvPort );
									}
								}

								//RcvIp
								byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
								if( byMpId > 0 )
								{
									nLen += sprintf( achTemp+nLen, "-->%s:%d(mp%d)", StrOfIP( dwRcvIp ), wRcvPort, byMpId );
								}
								else
								{
									nLen += sprintf( achTemp+nLen, "-->%s:%d(dst)", StrOfIP( dwRcvIp ), wRcvPort );
								}
								
								//DstIp
								byMpId = g_cMcuVcApp.FindMp( dwDstIp );
								if( byMpId > 0 )
								{
									nLen += sprintf( achTemp+nLen, "-->%s:%d(mp%d)", StrOfIP( dwDstIp ), wDstPort, byMpId );
								}
								else
								{
									nLen += sprintf( achTemp+nLen, "-->%s:%d(dst)", StrOfIP( dwDstIp ), wDstPort );
								}	

								StaticLog(achTemp );
							}
						}
					}
				}
			}
		}
	}
}

/*====================================================================
    函数名      ：ShowSwitchRouteToDst
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：BOOL32
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	2011/07/7	4.6			薛亮		  create
====================================================================*/
/*lint -save -e850*/
BOOL32 CMpManager::ShowSwitchRouteToDst(u8 byConfIdx, u32 dwDstIp, u16 wDstPort, BOOL32 bDetail)
{
	CMcuVcInst* pcVcInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
	if(NULL == pcVcInst)
	{
		//print
		return FALSE;
	}
	
	TConfInfo  *ptConfFullInfo = &pcVcInst->m_tConf;
	if(NULL == ptConfFullInfo)
	{
		//print
		return FALSE;
	}
	if( !ptConfFullInfo->m_tStatus.IsOngoing() )
	{
		//print;
		return FALSE;
	}
		
	TConfSwitchTable *ptSwitchTable = g_cMcuVcApp.GetConfSwitchTable( byConfIdx );
	if (NULL == ptSwitchTable)
	{
		//print
		return FALSE;
	}
	
	TAllMpSwInfo tAllMpSwinfo;
	u8 byLp = 0;
	BOOL32 bFound = FALSE;
	u32 dwDestIp = dwDstIp;
	u16 wDestPort = wDstPort;
	u32 dwSrcIp = 0;
	for (u8 byLoop1 = 1; byLoop1 <= MAXNUM_DRI; byLoop1++ )
	{
		if (bFound)
		{
			break;
		}

		if (!g_cMcuVcApp.IsMpConnected( byLoop1 ))
		{
			continue;
		}
		
		TSwitchChannel *ptSwitchChannel = NULL;
		for (u16 wLoop2 = 0; wLoop2 < MAX_SWITCH_CHANNEL; wLoop2++ )
		{
			ptSwitchChannel = ptSwitchTable->GetSwitchChannel( byLoop1, wLoop2 );
			if( NULL == ptSwitchChannel )
			{
				continue;
			}
			
			if (dwDestIp != ptSwitchChannel->GetDstIP() || wDestPort != ptSwitchChannel->GetDstPort())
			{
				continue;
			}
			
			bFound = TRUE;
			dwSrcIp = ptSwitchChannel->GetSrcIp();
			u32 dwDisIp = ptSwitchChannel->GetDisIp();
			u32 dwRcvIp = ptSwitchChannel->GetRcvIP();
			u16 wRcvPort = ptSwitchChannel->GetRcvPort();
			if ( dwSrcIp == 0xffffffff )
			{
				wRcvPort = ptSwitchTable->GetBrdSrcRcvPort();
            }
			
			tAllMpSwinfo.m_tSwInfo[byLp].m_byMpId = byLoop1;
			tAllMpSwinfo.m_tSwInfo[byLp].m_dwRcvIp = dwRcvIp;
			tAllMpSwinfo.m_tSwInfo[byLp].m_wRcvPort = wRcvPort;
			tAllMpSwinfo.m_tSwInfo[byLp].m_dwDstIp = dwDestIp;
			tAllMpSwinfo.m_tSwInfo[byLp].m_wDstPort = wDestPort;
			tAllMpSwinfo.m_UsedMPNum ++;
			byLp ++;

			//DisIp is 0 means that the switch not include bridge.
			if (0 == dwDisIp)
			{
				break;
			}
			
			u8 byMidMpId = 0;
			byMidMpId = g_cMcuVcApp.FindMp( dwDisIp );
			if (0 < byMidMpId)
			{
				byLoop1 = byMidMpId - 1;
				dwDestIp = dwRcvIp;
				wDestPort = wRcvPort;
				bFound = FALSE;
				break;
			}
			else
			{
				StaticLog("unexpected result looking for midMP(%s)!\n", StrOfIP(dwDisIp));
				break;
			}
		}	
	}

	if (byLp == 0)
	{
		return FALSE;
	}

	//print the whole path from src to dst
	StaticLog("\n");
	StaticLog("\t+     +  ++++\n");
	StaticLog("\t++   ++  +  +\n");
	StaticLog("\t+ + + +  ++++\n");
	StaticLog("\t+  +  +  +\n");
	StaticLog("\t+  +  +  +   route to %s:%d\n", StrOfIP(dwDstIp), wDstPort);
	StaticLog("\n------------------------------\n");
	s8 achPrntBuf[1024] = {0};
	u16 wBufLen = 0;
	wBufLen = sprintf(achPrntBuf, "   [src] %-15s", StrOfIP(dwSrcIp));
	s16 wIdx = (s16)(byLp-1);
	for (; wIdx >= 0; wIdx --)
	{
		wBufLen += sprintf(achPrntBuf + wBufLen, "\n-->[mp%2u]%-15s:%d", 
			tAllMpSwinfo.m_tSwInfo[wIdx].m_byMpId,
			StrOfIP(tAllMpSwinfo.m_tSwInfo[wIdx].m_dwRcvIp),
			tAllMpSwinfo.m_tSwInfo[wIdx].m_wRcvPort);
	}
	wBufLen += sprintf(achPrntBuf + wBufLen, "\n==>[dst] %-15s:%d\n", 
		StrOfIP(dwDstIp), wDstPort);
	StaticLog( achPrntBuf );

	StaticLog("------------------------------\nThe packet statistic info of mps:\n");

	// send msg to each mps
	if (!bDetail)
	{
		return TRUE;
	}
	CServMsg cServMsg;
	for (wIdx = (s16)(byLp-1); wIdx >= 0; wIdx --)
	{
		u8 byMpId = tAllMpSwinfo.m_tSwInfo[wIdx].m_byMpId;
		if( g_cMcuVcApp.IsMpConnected( byMpId ) )
		{
			TTransportAddr tRcvAddr;
			tRcvAddr.SetIpAddr(tAllMpSwinfo.m_tSwInfo[wIdx].m_dwRcvIp);
			tRcvAddr.SetPort(tAllMpSwinfo.m_tSwInfo[wIdx].m_wRcvPort);

			TTransportAddr tDstAddr;
			tDstAddr.SetIpAddr(dwDstIp);
			tDstAddr.SetPort(wDstPort);

			cServMsg.SetMsgBody();
			cServMsg.SetMsgBody((u8*)&tRcvAddr, sizeof(tRcvAddr));
			cServMsg.CatMsgBody((u8*)&tDstAddr, sizeof(tDstAddr));
			cServMsg.SetDstDriId(byMpId);
			
			g_cMpSsnApp.SendMsgToMpSsn(byMpId, MCU_MP_GETDSINFO_REQ, 
				cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
		}
	}

	return TRUE;

}
/*lint -restore*/

/*====================================================================
    函数名      ：ShowSwitch
    功能        ：屏幕打印交换信息
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byType:           支持的打印类型
                  0. 打印桥，不打印RTCP等（默认）
                  1. 不打印桥，打印RTCP等
                  2. 打印桥，打印RTCP
                  3  不打印桥，不打印RTCP等      
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/17    1.0         胡昌威        创建
====================================================================*/	
void CMpManager::ShowSwitch( u8 byType )
{
	TConfInfo  *ptConfFullInfo = NULL;
	TConfSwitchTable *ptSwitchTable = NULL;
	u8  byMpId, byLoop1;
    u16 wLoop2;
	u32 dwSrcIp, dwDisIp, dwRcvIp, dwDstIp;
	u16 wRcvPort, wDstPort;
	TSwitchChannel *ptSwitchChannel = NULL;
	s8  achTemp[255];
	s32 nLen, nLen0;

    u8 byMpcBrdId = g_cMcuAgent.GetMpcBoardId();

	BOOL32 bIsMiniMcu = FALSE;
#ifdef _MINIMCU_
	bIsMiniMcu = TRUE;
#endif
	
    // 视频广播源接收端口, zgc, 2008-07-23
    u16 wBrdSrcRcvPort = 0;
    u32 dwSwitchIp = 0;
    u32 dwSwitchSrcIp = 0;
    TMt tVidBrdSrc;
	for( u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++ )
	{ 
        if(NULL == g_cMcuVcApp.GetConfInstHandle( byConfIdx ))
        {
            continue;
        }

		CMcuVcInst* pcVcInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
		ptConfFullInfo = pcVcInst != NULL ? &pcVcInst->m_tConf : NULL;
		if(NULL == ptConfFullInfo || pcVcInst == NULL)
		{
			continue;
		}
		if( ptConfFullInfo->m_tStatus.IsOngoing() )
		{   
            // 获得视频广播源, zgc, 2008-07-23
            tVidBrdSrc = pcVcInst->GetVidBrdSrc();
            if ( !tVidBrdSrc.IsNull() )
            {
                GetSwitchInfo( tVidBrdSrc, dwSwitchIp, wBrdSrcRcvPort, dwSwitchSrcIp );
            }

			StaticLog("\n--------Conf: %s data switch info--------\n", ptConfFullInfo->GetConfName() );
		    ptSwitchTable = g_cMcuVcApp.GetConfSwitchTable( byConfIdx );
			if(NULL == ptSwitchTable)
			{
				continue;
			}
			for( byLoop1 = 1; byLoop1 <= MAXNUM_DRI; byLoop1++ )
			{				
                // guzh [3/29/2007] 支持打印特殊交换。参数为1或者2，就打印特殊信息(广播源/Rtcp)
				if( g_cMcuVcApp.IsMpConnected( byLoop1 ) ||
                    ( ( byType == 1 || byType == 2)
						&& ( (bIsMiniMcu && byLoop1 == 1 && !g_cMcuVcApp.IsMpConnected( byLoop1 ))
						      ||
							 ( ( byLoop1 == MCU_BOARD_MPC || byLoop1 == MCU_BOARD_MPCD || byLoop1 == 8) && 
						/*( byLoop1 != byMpcBrdId && */!bIsMiniMcu /*)*/ ) ) 
					)
                   )
				{
					StaticLog("\nMp%d(%s) switch info: ", byLoop1 , StrOfIP(g_cMcuVcApp.GetMpIpAddr(byLoop1)));
					for( wLoop2 = 0; wLoop2 < MAX_SWITCH_CHANNEL; wLoop2++ )
					{
						ptSwitchChannel = ptSwitchTable->GetSwitchChannel( byLoop1, wLoop2 );
						if( NULL == ptSwitchChannel )
						{
							continue;
						}
						if( !ptSwitchChannel->IsNull() )
						{
                            // xsl [9/15/2006] 参数为1，不打印桥信息
                            if ( (byType == 1 || byType == 3) 
                                && ptSwitchChannel->GetRcvPort() == ptSwitchChannel->GetDstPort()
                                && g_cMcuVcApp.FindMp(ptSwitchChannel->GetDstIP()) > 0)
                            {
                                continue;
                            }

							dwSrcIp = ptSwitchChannel->GetSrcIp();
							dwDisIp = ptSwitchChannel->GetDisIp();
							dwRcvIp = ptSwitchChannel->GetRcvIP();
							dwDstIp = ptSwitchChannel->GetDstIP();
                            wRcvPort = ptSwitchChannel->GetRcvPort();
							wDstPort = ptSwitchChannel->GetDstPort();

							memset( achTemp, ' ', 255 );
							nLen = 0;

							//SrcIp

                            // BUG4123: 视频交换中未显示交换板的当前实际使用的端口
                            // zgc, 2008-07-23
                            if ( dwSrcIp == 0xffffffff )
                            {
                                wRcvPort = ptSwitchTable->GetBrdSrcRcvPort();//wBrdSrcRcvPort;
                            }

							byMpId = g_cMcuVcApp.FindMp( dwSrcIp );
							if( byMpId > 0 )
							{
								nLen = sprintf( achTemp, "\n%d  %s:%d(mp%d)", wLoop2, StrOfIP( dwSrcIp ), wRcvPort, byMpId );
							}
							else
							{
								nLen = sprintf( achTemp, "\n%d  %s:%d(src)", wLoop2, StrOfIP( dwSrcIp ), wRcvPort );
							}
							
							nLen0 = nLen;
							
							//DisIp
							memset( achTemp+nLen, ' ', 255-nLen );
							if( dwDisIp > 0 )
							{
								byMpId = g_cMcuVcApp.FindMp( dwDisIp );
								if( byMpId > 0 )
								{
									nLen += sprintf( achTemp+nLen, "-->%s:%d(mp%d)", StrOfIP( dwDisIp ), wRcvPort, byMpId );
								}
								else
								{
									nLen += sprintf( achTemp+nLen, "-->%s:%d(src)", StrOfIP( dwDisIp ), wRcvPort );
								}
								StaticLog("%s\n", achTemp );
								nLen = nLen0-1;
								memset( achTemp, ' ', 255 );
							}

							//RcvIp
							memset( achTemp+nLen, ' ', 255-nLen );
							byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
							if( byMpId > 0 )
							{
								nLen += sprintf( achTemp+nLen, "-->%s:%d(mp%d)", StrOfIP( dwRcvIp ), wRcvPort, byMpId );
							}
							else
							{
								nLen += sprintf( achTemp+nLen, "-->%s:%d(dst)", StrOfIP( dwRcvIp ), wRcvPort );
							}
							
							//DstIp
							memset( achTemp+nLen, ' ', 255-nLen );
							byMpId = g_cMcuVcApp.FindMp( dwDstIp );

                            // BUG4123: 视频交换中未显示交换板的当前实际使用的端口
                            // zgc, 2008-07-17
                            if ( dwDstIp == 0xffffffff )
                            {
                                wDstPort = ptSwitchTable->GetBrdSrcRcvPort();//wBrdSrcRcvPort;
                            }

							if( byMpId > 0 )
							{
								nLen += sprintf( achTemp+nLen, "-->%s:%d(mp%d)", StrOfIP( dwDstIp ), wDstPort, byMpId );
							}
							else
							{
								nLen += sprintf( achTemp+nLen, "-->%s:%d(dst)", StrOfIP( dwDstIp ), wDstPort );
							}

							StaticLog( achTemp );
						
						}
					}
					StaticLog( "\n" );
				}
			}
			
		}
	}
	
}

/*=============================================================================
    函 数 名： GetMPSwitchInfo
    功    能： 获取指定mp板上的交换信息
    算法实现： 
    全局变量： 
    参    数： u8 byMpId
               TSwitchTable &tSwitchTable
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/01/25  3.5			万春雷                  创建
=============================================================================*/
void CMpManager::GetMPSwitchInfo( u8 byMpId, TSwitchTable &tSwitchTable )
{
	if( 0 == byMpId || byMpId > MAXNUM_DRI )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[GetMPSwitchInfo] Err: byMpId(%d) is invalid \n", byMpId );
		return;
	}
	if( FALSE == g_cMcuVcApp.IsMpConnected( byMpId ) )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[GetMPSwitchInfo] Err: byMpId(%d) is not connect \n", byMpId );
		return;
	}

	u16	wSwChIdx = 0;
	TConfInfo  *ptConfFullInfo = NULL;
	TSwitchChannel *ptSwitchChannel = NULL;
	TConfSwitchTable *ptSwitchTable = NULL;

	memset(&tSwitchTable, 0, sizeof(tSwitchTable));
	for( u8 byConfIdx = MIN_CONFIDX; byConfIdx <= MAX_CONFIDX; byConfIdx++ )
	{ 
        if(NULL == g_cMcuVcApp.GetConfInstHandle( byConfIdx ))
        {
            continue;
        }
        
		CMcuVcInst* pcVcInst = g_cMcuVcApp.GetConfInstHandle( byConfIdx );
		ptConfFullInfo = pcVcInst != NULL ? &pcVcInst->m_tConf : NULL;
        
		if( ptConfFullInfo && ptConfFullInfo->m_tStatus.IsOngoing() )
		{   
		    ptSwitchTable = g_cMcuVcApp.GetConfSwitchTable( byConfIdx );

			for( u16 wChannelIdx = 0; wChannelIdx < MAX_SWITCH_CHANNEL; wChannelIdx++ )
			{
				ptSwitchChannel = ptSwitchTable->GetSwitchChannel( byMpId, wChannelIdx );
				if( FALSE == ptSwitchChannel->IsNull() )
				{
					tSwitchTable.m_atSwitchChannel[wSwChIdx] = *ptSwitchChannel;
					wSwChIdx++;
					if( wSwChIdx >= MAX_SWITCH_CHANNEL )
					{
						break;
					}
				}
			}
		}
	}

	return;
}

///////////////////////////////////////////////////////
//// 将指定终端数据交换到上级mcu（级联多回传支持） ////
///////////////////////////////////////////////////////
BOOL32 CMpManager::StartSwitchToMmcu( const TMt & tSrc, 
                                     u16        wSrcChnnl, 
                                     const TMt & tDst, 
                                     u8        byMode,
                                     u8        bySwitchMode, 
                                     BOOL32 bH239Chnnl,
									 BOOL32 bStopBeforeStart,
									 BOOL32 bSrcHDBas,
									 u16 wSpyPort)
{
	u32  dwSrcIp = 0;
	u32  dwDisIp = 0;
	u32  dwRcvIp = 0;
	u16  wRcvPort = 0;

	TLogicalChannel	tLogicalChannel;
	BOOL32 bResult1 = TRUE;
	BOOL32 bResult2 = TRUE;

	//得到交换源地址
	if( !GetSwitchInfo( tSrc, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
		 LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchToSubMt() failure because of can't get switch info!\n");
		 return FALSE ;
	}
    
	CMcuVcInst* pcVcInst = g_cMcuVcApp.GetConfInstHandle( tSrc.GetConfIdx() );
	TConfInfo * ptConfInfo = pcVcInst != NULL ? &pcVcInst->m_tConf : NULL;
    
	u8 byIsSwitchRtcp;
	TConfAttrb tTempConfAttrb;
	if( ptConfInfo != NULL )
	{
		tTempConfAttrb = ptConfInfo->GetConfAttrb();
	}
		 
    if (  ( TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_VMP == tSrc.GetEqpType() && MT_TYPE_MT != tDst.GetMtType() &&
        ptConfInfo && tTempConfAttrb.IsResendLosePack()) ||
        tSrc == tDst) //  xsl [1/17/2006] (外厂商mt或mcu)将视频码流交换给自己时不进行rtcp交换
    {
        byIsSwitchRtcp = 0;
    }
    else
    {
        byIsSwitchRtcp = 1;
    }

	//只收数据不发
	TMt tDstMt = tDst;
	if( tDstMt.IsNull() )
	{
        // 顾振华 [4/30/2006] 只收不发的处理不在这里,在逻辑通道打开后就转Dump 
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "StartSwitchToSubMt() tDstMt is NULL! It's send-only MT?\n");        
		return TRUE;
	}

	//得到会议终端表指针
	TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable( tSrc.GetConfIdx() );
	if( ptConfMtTable == NULL )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "StartSwitchToSubMt() failure because invalid ConfMtTable Pointer!\n");
		return FALSE;
	}

    u8 bySwitchChannelMode;
    u8 byUniformPayload = INVALID_PAYLOAD;
    u8 byManuID = ptConfMtTable->GetManuId(tDst.GetMtId());

    if (byManuID == MT_MANU_KDC || byManuID == MT_MANU_KDCMCU)
	{
		bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_NONE;
	}
    else
    {
        bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_VALID;
        //Active Payload Uniform
        if (TYPE_MT == tSrc.GetType() && TYPE_MT == tDst.GetType())
        {
            u8 byChannelType = 0;
            TLogicalChannel  tChannel;
            memset(&tChannel, 0, sizeof(TLogicalChannel));
            //Get channal info
            if (0 == wSrcChnnl)
            {
                byChannelType = LOGCHL_VIDEO;
            }
            else if (1 == wSrcChnnl)
            {
                byChannelType = LOGCHL_SECVIDEO;
            }

            if (ptConfMtTable->GetMtLogicChnnl(tDst.GetMtId(), byChannelType, &tChannel, TRUE))
            {
                if (tChannel.GetChannelType() >= AP_MIN && tChannel.GetChannelType() <= AP_MAX)
                {
                    byUniformPayload = tChannel.GetChannelType();
                }
            }
        }
    }

	//广播交换方式
	if(bySwitchMode==SWITCH_MODE_BROADCAST)
	{
		//得到目的Mt的交换信息
		u32  dwMpIP = 0;
		dwMpIP = g_cMcuVcApp.GetMpIpAddr( ptConfMtTable->GetMpId( tDst.GetMtId() ) );
#if !defined(_8KH_) && !defined(_8KE_) && !defined(_8KI_)
		if( dwMpIP != dwRcvIp )//需要转发
		{
			dwDisIp = dwRcvIp;
			dwRcvIp = dwMpIP;
		}
#endif
	}

	//根据信道号计算偏移量
	//wRcvPort = wRcvPort + PORTSPAN * wSrcChnnl;    //双流要调整
	if( wSrcChnnl >= CASCADE_SPY_STARTPORT && wSrcChnnl < MT_MCU_STARTPORT ) //用多回传的通道
	{
		wRcvPort = wSrcChnnl;
	}
	else
	{
		wRcvPort = wRcvPort + PORTSPAN * wSrcChnnl;
	}

	//图像
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		//建立交换
		if( ptConfMtTable->GetMtLogicChnnl( tDst.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, TRUE ) )	//可以交换
		{
			//  [11/9/2009 pengjie] 级联多回传支持
			if( wSpyPort != SPY_CHANNL_NULL )
			{

				tLogicalChannel.m_tRcvMediaChannel.SetPort( wSpyPort );
			}
			//  End

            bResult1 = StartSwitch( tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort, tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp, bStopBeforeStart,tDst.GetMtId());
			if(!bResult1)LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Switch video failure!\n");
		}
	}

    if (MODE_SECVIDEO == byMode)
    {
        //H.239视频逻辑通道
        if (bH239Chnnl)
        {
            if (ptConfMtTable->GetMtLogicChnnl(tDst.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, TRUE))
            {
				//zbq[09/03/2008] HD Bas作源的双流 从端口基址+PORTSPAN 建交换
				if (bSrcHDBas)
				{
					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort/* + PORTSPAN*/,
										   tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp,bStopBeforeStart,tDst.GetMtId());					
				}
				else
				{
					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort+4,
										   tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp,bStopBeforeStart,tDst.GetMtId());					
				}
                if (!bResult1)
                {
                    LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Switch video h.239 failure!\n");
                }
            }
            return bResult1;
        }
    }

	//语音
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		//建立交换
		if( ptConfMtTable->GetMtLogicChnnl(tDst.GetMtId(), LOGCHL_AUDIO, &tLogicalChannel, TRUE ) )	//可以交换
		{
			//  [11/9/2009 pengjie] 级联多回传支持
			if( wSpyPort != SPY_CHANNL_NULL )
			{
				tLogicalChannel.m_tRcvMediaChannel.SetPort( wSpyPort + 2 );
			}
			// End

			bResult2 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, (wRcvPort+2),
                                   tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp,bStopBeforeStart,tDst.GetMtId());
			if(!bResult2)LogPrint(LOG_LVL_ERROR, MID_MCU_MPMGR, "Switch audio failure!\n");
		}
	}	

	return  bResult1 && bResult2 ;
}

void CSwitchGrpAdpt::Reset()
{
	if ( 0 == m_nGrpNum || NULL == m_ppMtList)
	{
		return;
	}

	for (int nLoop = 0; nLoop < m_nGrpNum; nLoop++)
	{
		if (m_ppMtList[nLoop])
		{
			delete [] m_ppMtList[nLoop];
			m_ppMtList[nLoop] = NULL;
		}
	}

	delete []m_ppMtList;
	m_ppMtList = NULL;
}

BOOL32 CSwitchGrpAdpt::Convert( const s32 nGrpNum, TSwitchGrp* ptSwitchGrp )
{
	if ( 0 >= nGrpNum || ptSwitchGrp == NULL )
	{
		return FALSE;
	}

	Reset();

	m_nGrpNum = nGrpNum;
	if ( NULL == (m_ppMtList = new TMt*[(u32)m_nGrpNum] )) 
	{
		return FALSE;
	}

	s32 nLoop = 0;
	for ( nLoop = 0; nLoop < m_nGrpNum; nLoop++ )
	{
		if ( NULL == (m_ppMtList[nLoop] = new TMt[MAXNUM_CONF_MT] ) )
		{
			Reset();
			return FALSE;
		}
	}

	TMt* ptMtList = NULL;
	u8 byDstMtNum = 0;
	for ( nLoop = 0; nLoop < m_nGrpNum; nLoop++)
	{
		ptMtList = ptSwitchGrp[nLoop].GetDstMt();
		byDstMtNum = ptSwitchGrp[nLoop].GetDstMtNum();
		byDstMtNum = min(MAXNUM_CONF_MT, byDstMtNum);
		if (!ptMtList || 0 == byDstMtNum )
		{
			StaticLog("[CSwitchGrpAdpt] Err, ptMtList is null or byDstMtNum.%d\n", byDstMtNum);
			continue;
		}

		memcpy(m_ppMtList[nLoop], ptMtList, sizeof(TMt) * byDstMtNum);
		ptSwitchGrp[nLoop].SetDstMt(m_ppMtList[nLoop]);
	}

	return TRUE;
}

// END OF FILE
