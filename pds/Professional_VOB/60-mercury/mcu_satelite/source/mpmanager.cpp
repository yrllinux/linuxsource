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
#include "mpmanager.h"
#include "mtadpssn.h"
#include "mpssn.h"

#if defined(_LINUX_)
#include "boardwrapper.h"
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
void ssw( u8 byType )
{
	pcMpManager->ShowSwitch( byType );

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
BOOL32 CMpManager::StartMulticast( const TMt & tSrc, u8 bySrcChnnl, u8 byMode, BOOL32 bConf )
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
		 MpManagerLog("StartMulticast() failure because of can't get switch info!\n");
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
	
	//根据信道号计算偏移量	
	wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;

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
                MpManagerLog("\n[StartMulticast]byIsMulticast = %d  wMpId+1 = %d\n\n", byIsMulticast, wMpId+1);
                if (1 != byIsMulticast)
                {
                    continue;
                }

                byMulticastFlag++;

                dwRcvIp = g_cMcuVcApp.GetMpIpAddr(wMpId+1);
                MpManagerLog("\n[StartMulticast]dwRcvIp = 0x%x\n\n", dwRcvIp);
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
                    MpManagerLog("Switch video failure!\n");
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
                    MpManagerLog("Switch audio failure!\n");
                }
            }
            
            //第二路视频
            if (byMode == MODE_SECVIDEO)
            {
                bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, (wRcvPort+4),
                                       dwMultiCastAddr, (dwMultiCastPort+4), dwRcvIp, dwRcvIp);
                if (!bResult1)
                {
                    MpManagerLog("Switch snd video failure!\n");
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
		 MpManagerLog("StopMulticast() failure because of can't get switch info!\n");
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
		 MpManagerLog("StartDistrConfCast() failure because of can't get switch info!\n");
		 return FALSE ;
	}

    CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(tSrc.GetConfIdx());
    if ( NULL == pVcInst )
    {
        // Fatal error!
        MpManagerLog("Cannot get VC Instance(conf idx: %d) while StartDistrConfCast!\n", 
                            tSrc.GetConfIdx());
        return FALSE;
    }

    dwMultiCastAddr = pVcInst->m_tConf.GetConfAttrb().GetSatDCastIp();
    wMultiCastPort = pVcInst->m_tConf.GetConfAttrb().GetSatDCastPort();
	
	// xsl [8/19/2006] 卫星分散会议混音器N模式交换到组播地址
    if (tSrc.GetType() == TYPE_MCUPERI && tSrc.GetEqpType() == EQP_TYPE_MIXER)
    {
        //wRcvPort = wRcvPort + MAXPORTSPAN_MIXER_GROUP*m_byMixGrpId;    //这里不考虑多混音组的情况，有需求时再考虑
    }
    else //根据信道号计算偏移量
    {
        wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;
    }	

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
                MpManagerLog("\n[StartDistrConfCast()]byIsMulticast = %d  wMpId+1 = %d\n\n", byIsMulticast, wMpId+1);
                if (1 != byIsMulticast)
                {
                    continue;
                }

                byMulticastFlag++;

                dwRcvIp = g_cMcuVcApp.GetMpIpAddr(wMpId+1);
                MpManagerLog("\n[StartDistrConfCast()]dwRcvIp = 0x%x\n\n", dwRcvIp);
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
                MpManagerLog("StartDistrConfCast() Switch video\n");
                bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort,
                                       dwMultiCastAddr, wMultiCastPort, dwRcvIp, dwRcvIp,
                                       SWITCHCHANNEL_UNIFORMMODE_NONE,
                                       INVALID_PAYLOAD,
                                       // 建立RTCP交换
                                       1);
                if (!bResult1)
                {
                    MpManagerLog("StartDistrConfCast() Switch video failure!\n");
                }
            }

            //语音
            if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
            {
                //建立交换
                MpManagerLog("StartDistrConfCast() Switch audio\n");
                bResult2 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, (wRcvPort+2),
                                       dwMultiCastAddr, (wMultiCastPort+2), dwRcvIp, dwRcvIp,
                                       SWITCHCHANNEL_UNIFORMMODE_NONE,
                                       INVALID_PAYLOAD,
                                       // 建立RTCP交换
                                       1);
                if (!bResult2)
                {
                    MpManagerLog("StartDistrConfCast() Switch audio failure!\n");
                }
            }
            
            //第二路视频
            if (byMode == MODE_SECVIDEO)
            {
                MpManagerLog("StartDistrConfCast() Switch snd video\n");
                bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, 0, dwRcvIp, (wRcvPort+4),
                                       dwMultiCastAddr, (wMultiCastPort+4), dwRcvIp, dwRcvIp,
                                       SWITCHCHANNEL_UNIFORMMODE_NONE,
                                       INVALID_PAYLOAD,
                                       // 建立RTCP交换
                                       1);
                if (!bResult1)
                {
                    MpManagerLog("StartDistrConfCast() Switch snd video failure!\n");
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
		 MpManagerLog("StopDistrConfCast() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//得到MCU的组播地址
    CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(tSrc.GetConfIdx());
    if ( NULL == pVcInst )
    {
        // Fatal error!
        MpManagerLog("Cannot get VC Instance(conf idx: %d) while StopDistrConfCast!\n", 
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
        MpManagerLog("StopDistrConfCast() Switch video\n");
		StopSwitch(tSrc.GetConfIdx(), dwMultiCastAddr, wMultiCastPort, 1);
	}
		
	//语音
	if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
	{
		//移除交换
        MpManagerLog("StopDistrConfCast() Switch audio\n");
		StopSwitch(tSrc.GetConfIdx(), dwMultiCastAddr, (wMultiCastPort+2), 1);
	}
    
    //第二路视频
    if (byMode == MODE_SECVIDEO)
    {
        MpManagerLog("StopDistrConfCast() Switch snd video\n");
        StopSwitch(tSrc.GetConfIdx(), dwMultiCastAddr, (wMultiCastPort+4), 1);
    }

	return TRUE;
}

/*====================================================================
    函数名      ：StartSatConfCast
    功能        ：开始卫星会议组播（含单播）
    算法实现    ：
    引用全局变量：
    输入参数说明：tSrc 源终端
	              bySrcChnnl 通道号
				  byMode 交换模式,缺省为MODE_BOTH
    返回值说明  ：TRUE，如果不能开始广播返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	11/11/09    4.6         张宝卿        创建
====================================================================*/
BOOL32 CMpManager::StartSatConfCast(const TMt &tSrc, u8 byCastType, u8 byMode, u8 bySrcChnnl)
{
	if (MODE_BOTH == byMode)
	{
		return StartSatConfCast(tSrc, byCastType, MODE_AUDIO, bySrcChnnl) &
			   StartSatConfCast(tSrc, byCastType, MODE_VIDEO, bySrcChnnl);
	}

    u32  dwSrcIp  = 0;
	u32  dwRcvIp  = 0;
	u16  wRcvPort = 0;
	
	
	//得到交换源地址
	if (!GetSwitchInfo(tSrc, dwRcvIp, wRcvPort, dwSrcIp))
	{
		MpManagerLog("StartSatConfCast() failure because of can't get switch info!\n");
		return FALSE ;
	}

	u16 wPortAdj = 0;
	
	switch (byMode)
	{
	case MODE_AUDIO:
		wPortAdj += 2;
		break;
	case MODE_VIDEO:
		break;
	case MODE_SECVIDEO:
		wPortAdj += 4;
		break;
	}

	u16 wMultiCastPort = g_cMcuVcApp.GetMcuMulticastDataPort() + wPortAdj;

	u32 dwMultiCastAddr = 0;
	if (CAST_FST == byCastType)
	{
		//第一组播点
		dwMultiCastAddr = g_cMcuVcApp.GetMcuMulticastDataIpAddr();
	}
	else if (CAST_SEC == byCastType)
	{
		//第二组播点
		dwMultiCastAddr = g_cMcuVcApp.GetMcuSecMulticastIpAddr();

		//标清的polycom不支持双流，此处抢其第一路视频组播地址，停双流时恢复;
		if (MODE_SECVIDEO == byMode)
		{
			wMultiCastPort -= wPortAdj;
		}
		if (MODE_SECVIDEO == byMode &&
			tSrc.GetType() == TYPE_MCUPERI &&
			tSrc.GetEqpType() == EQP_TYPE_BAS)
		{
			wPortAdj -= 4;
		}
	}
	
	wRcvPort = wRcvPort + bySrcChnnl * PORTSPAN + wPortAdj;

	BOOL32 bRet = StartSwitch(tSrc,
								tSrc.GetConfIdx(),
								dwSrcIp,
								0,
								dwRcvIp,
								wRcvPort,
								dwMultiCastAddr, wMultiCastPort,
								dwRcvIp, dwRcvIp,
								SWITCHCHANNEL_UNIFORMMODE_NONE,
								INVALID_PAYLOAD,
								// 建立RTCP交换
								1);
	
	if (!bRet)
	{
		OspPrintf( TRUE, FALSE, "[StartSatConfCast] startSwitch <0x%x@%d>-><0x%x@%d> failed!\n",
								 dwRcvIp,
								 wRcvPort,
								 dwMultiCastAddr,
								 wMultiCastPort);
	}
	return bRet;
}

/*====================================================================
    函数名      ：StopSatConfCast
    功能        ：停止卫星会议组播（含单播）
    算法实现    ：
    引用全局变量：
    输入参数说明：tSrc 源终端
	              bySrcChnnl 通道号
				  byMode 交换模式,缺省为MODE_BOTH
    返回值说明  ：TRUE，如果不能开始广播返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	11/11/09    4.6         张宝卿        创建
====================================================================*/
BOOL32 CMpManager::StopSatConfCast(const TMt &tSrc, u8 byCastType, u8 byMode, u8 bySrcChnnl)
{

	if (MODE_BOTH == byMode)
	{
		return StopSatConfCast(tSrc, byCastType, MODE_AUDIO, bySrcChnnl) &
			   StopSatConfCast(tSrc, byCastType, MODE_VIDEO, bySrcChnnl);
	}

	u32  dwSrcIp  = 0;
	u32  dwRcvIp  = 0;
	u16  wRcvPort = 0;

	//得到交换源地址
	if (!GetSwitchInfo(tSrc, dwRcvIp, wRcvPort, dwSrcIp))
	{
		MpManagerLog("StopSatConfCast() failure because of can't get switch info!\n");
		return FALSE ;
	}
	
	BOOL32 bRet = FALSE;

	u16 wMultiCastPort = g_cMcuVcApp.GetMcuMulticastDataPort();

	u16 wUniPort = g_cMcuVcApp.GetMcuRcvMtMediaStartPort() + (tSrc.GetMtId()-1) * PORTSPAN;
	
	u16 wPortAdj = 0;
	
	switch (byMode)
	{
	case MODE_AUDIO:
		wPortAdj += 2;
		break;
	case MODE_VIDEO:
		break;
	case MODE_SECVIDEO:
		wPortAdj += 4;
		break;
	}
	
	wUniPort += wPortAdj;

	if (CAST_UNI == byCastType)
	{
		//[1] 单播处理：只取IP，端口取配置文件

		//FIXME: 组播重传和归一重整的加入
		bRet = StopSwitch(tSrc.GetConfIdx(),
							dwRcvIp,
							wUniPort
							);
		
		if (!bRet)
		{
			OspPrintf( TRUE, FALSE, "[StartSatConfCast] stopSwitch <0x%x@%d> failed!\n",
									dwRcvIp,
									wUniPort);
			return bRet;
		}
	}
	
	wMultiCastPort += wPortAdj;

	if (CAST_FST == byCastType)
	{
		//[2] 取消到组播点的
		u32 dwMultiCastAddr = g_cMcuVcApp.GetMcuMulticastDataIpAddr();
		
		bRet = StopSwitch( tSrc.GetConfIdx(), dwMultiCastAddr, wMultiCastPort);
		
		if (!bRet)
		{
			OspPrintf( TRUE, FALSE, "[StartSatConfCast] stopSwitch <0x%x@%d> failed!\n",
									dwMultiCastAddr,
									wMultiCastPort);
		}
	}
	
	if (CAST_SEC == byCastType)
	{
		//[3] 停到第二组播点的
		u32 dwSecMultiCastIpAddr = g_cMcuVcApp.GetMcuSecMulticastIpAddr();
	
		
		//标清的polycom不支持双流，此处抢其第一路视频组播地址，停双流时恢复;
		if (MODE_SECVIDEO == byMode)
		{
			wMultiCastPort -= wPortAdj;
		}

		bRet = StopSwitch( tSrc.GetConfIdx(),
						   dwSecMultiCastIpAddr, wMultiCastPort);
		
		if (!bRet)
		{
			OspPrintf( TRUE, FALSE, "[StopSatConfCast] startSwitch secMulSwitch <0x%x@%d> failed!\n",
									dwSecMultiCastIpAddr,
									wMultiCastPort);
		}
	}

	return bRet;
}


BOOL32 CMpManager::StartSwitchSrc2Dst(const TMt &tSrc,
									  const TMt &tDst,
									  u8		byMode,
									  u8		bySrcChnnl)
{
	if (MODE_BOTH == byMode)
	{
		return StartSwitchSrc2Dst(tSrc, tDst, MODE_AUDIO, bySrcChnnl) &
			   StartSwitchSrc2Dst(tSrc, tDst, MODE_VIDEO, bySrcChnnl);
	}

	u32  dwSrcIp  = 0;
	u32  dwRcvIp  = 0;
	u16  wRcvPort = 0;
	
	//得到交换源地址
	if (!GetSwitchInfo(tSrc, dwRcvIp, wRcvPort, dwSrcIp))
	{
		MpManagerLog("StartSwitchSrc2Dst() failure because of can't get switch info!\n");
		return FALSE ;
	}

	//[1] 单播处理：只取IP，端口取配置文件
	u16 wUniPort = g_cMcuVcApp.GetMcuRcvMtMediaStartPort() + (tSrc.GetMtId()-1) * PORTSPAN;
	
	u16 wPortAdj = 0;
	
	switch (byMode)
	{
	case MODE_AUDIO:
		wPortAdj += 2;
		break;
	case MODE_VIDEO:
		break;
	case MODE_SECVIDEO:
		wPortAdj += 4;
		break;
	}
	
	wUniPort += wPortAdj;
	wRcvPort += wPortAdj;
	
	/*
	//FIXME: 组播重传和归一重整的加入
	BOOL32 bRet = StartSwitch(tSrc,
								tSrc.GetConfIdx(),
								dwSrcIp,
								0,
								dwRcvIp,
								wRcvPort,
								dwRcvIp,
								wUniPort,
								0,
								0,
								SWITCHCHANNEL_UNIFORMMODE_NONE
								);
	
	if (!bRet)
	{
		OspPrintf( TRUE, FALSE, "[StartSwitchSrc2Dst] startSwitch <0x%x@%d>-><0x%x@%d> failed!\n",
								dwRcvIp,
								wRcvPort,
								dwRcvIp,
								wUniPort);
		return FALSE;
	}
*/
	//[2] 单播下行，直接交换给目的

	//得到交换目的地址
	u32 dwDstRcvIp = 0;
	u16 wDstRcvPort = 0;
	u32 dwDstSrcIp = 0;
	if (!GetSwitchInfo(tDst, dwDstRcvIp, wDstRcvPort, dwDstSrcIp))
	{
		MpManagerLog("StartSwitchSrc2Dst() failure because of can't get switch info!\n");
		return FALSE ;
	}

	u32 dwDstIp = dwDstSrcIp;
	u16 wDstPort = g_cMcuVcApp.GetMtRcvMcuMediaPort();
	
	wDstPort += wPortAdj;

	BOOL32 bRet = StartSwitch(tSrc,
								tSrc.GetConfIdx(),
								dwSrcIp,
								0,
								dwRcvIp,
								wUniPort,
								dwDstIp,
								wDstPort,
								0,
								0,
								SWITCHCHANNEL_UNIFORMMODE_NONE
								);

	if (!bRet)
	{
		OspPrintf( TRUE, FALSE, "[StartSwitchSrc2Dst] startSwitch <0x%x@%d>-><0x%x@%d> failed!\n",
								 dwRcvIp,
								 wUniPort,
								 dwDstRcvIp,
								 wDstRcvPort);
		return FALSE;
	}
	return TRUE;
}

BOOL32 CMpManager::StopSwitchSrc2Dst(const TMt &tSrc,
									 const TMt &tDst,
									 u8			byMode, 
									 u8			bySrcChnnl)
{
	if (MODE_BOTH == byMode)
	{
		return StopSwitchSrc2Dst(tSrc, tDst, MODE_AUDIO, bySrcChnnl) &
			   StopSwitchSrc2Dst(tSrc, tDst, MODE_VIDEO, bySrcChnnl);
	}
	u32  dwSrcIp  = 0;
	u32  dwRcvIp  = 0;
	u16  wRcvPort = 0;
	
	//得到交换源地址
	if (!GetSwitchInfo(tSrc, dwRcvIp, wRcvPort, dwSrcIp))
	{
		MpManagerLog("StopSwitchSrc2Dst() failure because of can't get switch info!\n");
		return FALSE ;
	}


	//[1] 单播处理：只取IP，端口取配置文件
	u16 wUniPort = g_cMcuVcApp.GetMcuRcvMtMediaStartPort() + (tSrc.GetMtId()-1) * PORTSPAN;
	
	u16 wPortAdj = 0;
	
	switch (byMode)
	{
	case MODE_AUDIO:
		wPortAdj += 2;
		break;
	case MODE_VIDEO:
		break;
	case MODE_SECVIDEO:
		wPortAdj += 4;
		break;
	}
	
	wUniPort += wPortAdj;
	
	/*
	//FIXME: 组播重传和归一重整的加入
	BOOL32 bRet = StopSwitch(tSrc.GetConfIdx(),
							 dwRcvIp,
							 wUniPort
							 );
	
	if (!bRet)
	{
		OspPrintf( TRUE, FALSE, "[StopSwitchSrc2Dst] stopSwitch <0x%x@%d> failed!\n",
								 dwRcvIp,
								 wUniPort);
		return bRet;
	}
	*/

	//[2] 取消到单播目的点的
	
	//得到交换目的地址
	u32 dwDstRcvIp = 0;
	u16 wDstRcvPort = 0;
	u32 dwDstSrcIp = 0;
	if (!GetSwitchInfo(tDst, dwDstRcvIp, wDstRcvPort, dwDstSrcIp))
	{
		MpManagerLog("StopSwitchSrc2Dst() failure because of can't get switch info!\n");
		return FALSE ;
	}

	u32 dwDstIp = dwDstSrcIp;
	u16 wDstPort = g_cMcuVcApp.GetMtRcvMcuMediaPort();
	
	wDstPort += wPortAdj;
	
	BOOL32 bRet = StopSwitch(tSrc.GetConfIdx(), dwDstIp, wDstPort);
	
	if (!bRet)
	{
		OspPrintf( TRUE, FALSE, "[StartSwitchSrc2Dst] stopSwitch <0x%x@%d> failed!\n",
			dwDstIp,
			wDstPort);
		return FALSE;
	}

	return bRet;

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
====================================================================*/
BOOL32 CMpManager::GetSwitchInfo(const TMt & tSrc, u32 &dwSwitchIpAddr, u16 &wSwitchPort, u32 &dwSrcIpAddr)
{
	u32  dwSrcIp  = 0;
	u32  dwRcvIp  = 0;
	u16  wRcvPort = 0;

	u16  byMcuId  = tSrc.GetMcuId();
	u8   byMtId   = tSrc.GetMtId();
	u8   byEqpId  = tSrc.GetEqpId();
	u8   byPeriType;

    BOOL32 bHDVmp = FALSE;

	//得到交换源的地址与端口
	switch(tSrc.GetType()) 
	{

	case TYPE_MT://终端
		{
			TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable(tSrc.GetConfIdx());
			if (ptConfMtTable == NULL)
			{
				MpManagerLog("GetSwitchInfo() failure, Type: %d SubType: %d Id: %d ConfIdx: %d!\n", 
					   tSrc.GetType(), tSrc.GetEqpType(), tSrc.GetEqpId(),tSrc.GetConfIdx());
				return FALSE;
			}

			//GetSubMtSwitchAddr(tSrc,dwRcvIp,wRcvPort);
			ptConfMtTable->GetMtSwitchAddr(tSrc.GetMtId(), dwRcvIp, wRcvPort);
			dwSrcIp = ptConfMtTable->GetIPAddr(byMtId);
		}
		break;

	case TYPE_MCUPERI://外设

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
		}
		g_cMcuAgent.GetPeriInfo(byEqpId, &dwSrcIp, &byPeriType);
		dwSrcIp = ntohl(dwSrcIp);
		break;
	}

	//判断交换源的地址与端口是否合法
	if (dwRcvIp == 0 || wRcvPort == 0)
	{
        if (!(tSrc.GetType() == TYPE_MCUPERI  && tSrc.GetEqpType() == EQP_TYPE_TVWALL))
        {
		    MpManagerLog("GetSwitchInfo() failure, Type: %d SubType: %d Id: %d, dwRcvIp: %d wRcvPort: %d!\n", 
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
		    MpManagerLog("GetSwitchInfo() cann't get switch src IP, dwSrcIp: 0, byEqpId: %d!\n", byEqpId);
        }
        else
            MpManagerLog("GetSwitchInfo() cann't get switch src IP, dwSrcIp: 0, byMtId: %d!\n", byMtId);
		return FALSE;
	}

	dwSrcIpAddr    = dwSrcIp;
	dwSwitchIpAddr = dwRcvIp;
	wSwitchPort    = wRcvPort;

	return TRUE;
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
BOOL32 CMpManager::SetSwitchBridge(const TMt & tSrc, u8 bySrcChnnl, u8 byMode, BOOL32 bSrcHDBas)
{
	u32  dwSrcIp = 0;
	u32  dwRcvIp = 0;
	u16  wRcvPort = 0;

	TMt tSrcMt = tSrc;

	//得到交换源地址
	if (!GetSwitchInfo(tSrcMt, dwRcvIp, wRcvPort, dwSrcIp))
	{
        MpManagerLog("SetSwitchBridge() failure because of can't get switch info!\n");
		return FALSE;
	}

	//根据IP地址查找交换MP编号
	u8 byMpId = g_cMcuVcApp.FindMp(dwRcvIp);
	 
    wRcvPort = wRcvPort +PORTSPAN*bySrcChnnl;

	//建立交换桥
	u32 dwDstIp = 0;
	BOOL32 bResult;
	BOOL32 bResultAnd = TRUE;
    dwRcvIp = 0;
    u8  byFirstMpId = 0;
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
               continue;
            }            

            //下一个mp                   
            dwDstIp = g_cMcuVcApp.GetMpIpAddr(nMpId+1);

			bResult = StartBridgeSwitch(byMode, tSrcMt, dwSrcIp, dwRcvIp, wRcvPort, dwDstIp, bSrcHDBas);

			bResultAnd = bResultAnd && bResult;

            //本次mp的目的地址作为下一个mp的接收地址
            dwRcvIp = dwDstIp;
	   } 
	}

    //若源不是第一个mp，需要建立尾部到头部的桥交换，最终形成链形桥交换
    if ( byMpId > 1 && byMpId != byFirstMpId && g_cMcuVcApp.IsMpConnected(byFirstMpId) )
    {
        dwDstIp = g_cMcuVcApp.GetMpIpAddr(byFirstMpId);

        bResult = StartBridgeSwitch(byMode, tSrcMt, dwSrcIp, dwRcvIp, wRcvPort, dwDstIp, bSrcHDBas);

		bResultAnd = bResultAnd && bResult;
    }

    return bResultAnd; 
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
BOOL32 CMpManager::RemoveSwitchBridge(const TMt & tSrc,u8 bySrcChnnl,u8 byMode, BOOL32 bEqpHDBas )
{
	u32  dwSrcIp = 0;
	u32  dwRcvIp = 0;
	u16  wRcvPort = 0;
 
	//得到交换源地址
	if( !GetSwitchInfo( tSrc, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
        MpManagerLog("RemoveSwitchBridge() failure because of can't get switch info!");
		return FALSE;
	}

	//根据IP地址查找交换MP编号
	u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	
	wRcvPort = wRcvPort + PORTSPAN*bySrcChnnl;
  
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
        bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort, dwDstIp, wRcvPort, 0, 0, SWITCHCHANNEL_UNIFORMMODE_VALID);
        break;
    case MODE_AUDIO:
        bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort+2, dwDstIp, wRcvPort+2, 0, 0, SWITCHCHANNEL_UNIFORMMODE_VALID);
        break;
    case MODE_BOTH:
        bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort, dwDstIp, wRcvPort, 0, 0, SWITCHCHANNEL_UNIFORMMODE_VALID);
        bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort+2, dwDstIp, wRcvPort+2, 0, 0, SWITCHCHANNEL_UNIFORMMODE_VALID);
        break;
	case MODE_SECVIDEO:
		//zbq[09/05/2008] 高清适配的双流桥交换不作＋4处理
		if (bSrcHDBas)
		{
			bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort, dwDstIp, wRcvPort, 0, 0, SWITCHCHANNEL_UNIFORMMODE_VALID);
		}
		else
		{
			bResult = StartSwitch(tSrcMt, tSrcMt.GetConfIdx(), dwSrcIp, 0, dwRcvIp, wRcvPort+4, dwDstIp, wRcvPort+4, 0, 0, SWITCHCHANNEL_UNIFORMMODE_VALID);
		}
		break;
    default:
        break;
    }

    return bResult;
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
BOOL32 CMpManager::StartSwitchToMc( const TMt & tSrc, u8 bySrcChnnl, u16 wMcInstId, u8 byDstChnnl, u8 byMode )
{
	u32  dwSrcIp = 0;
	u32	dwRcvIp;
	u16	wRcvPort;
	u8  byChannelNum;
	TLogicalChannel tLogicalChannel;
	BOOL32 bResult1 = TRUE;
	BOOL32 bResult2 = TRUE;

	//得到交换源地址
	if( !GetSwitchInfo( tSrc, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
		MpManagerLog("StartSwitchToMc() failure because of can't get switch info!");
		return FALSE ;
	}

	//根据信道号计算偏移量	
	wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;

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
			MpManagerLog( "Cannot get video send address to Meeting Console%u!\n", wMcInstId );
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
			MpManagerLog("Cannot get audio send address to Meeting Console%u!\n", wMcInstId );
		}
	}
	
    return bResult1&&bResult2; 
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
}


/*====================================================================
    函数名      ：StartSwitchToBrd
    功能        ：将指定终端的视频作为广播源
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
BOOL32 CMpManager::StartSwitchToBrd( const TMt &tSrc, u8 bySrcChnnl, BOOL32 bForce)
{
	u32  dwSrcIp = 0;
	u32  dwRcvIp = 0;
	u16  wRcvPort = 0;
	BOOL32 bResult = TRUE;

	//得到交换源地址
	if( !GetSwitchInfo( tSrc, dwRcvIp, wRcvPort, dwSrcIp ) )
	{
		 MpManagerLog("StartSwitchToBrd() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//根据信道号计算偏移量
	wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl; 

    //建立交换

    //根据IP地址查找交换MP编号
    u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
    if( 0 == byMpId )
    {
        MpManagerLog("No MP's Addrs is 0x%x in StartSwitchToBrd(),please check switch recv IP!\n",dwRcvIp);
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
    cServMsg.SetMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));
    u8 byForce = bForce ? 1 : 0;
    cServMsg.CatMsgBody(&byForce, sizeof(byForce));
    
    g_cMpSsnApp.BroadcastToAllMpSsn(MCU_MP_ADDBRDSRCSWITCH_REQ, 
                                    cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
    
    MpManagerLog("Send MCU_MP_ADDBRDSRCSWITCH_REQ from RecvMp %s:%d to all Mp.\n",
                 StrOfIP(tSwitchChannel.GetRcvIP()), tSwitchChannel.GetRcvPort());
    
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
BOOL32 CMpManager::StartSwitchFromBrd(const TMt &tSrc, u8 bySrcChnnl, u8 byDstMtNum, const TMt* const ptDstBase)
{
    CMcuVcInst * pcVcInst = g_cMcuVcApp.GetConfInstHandle(tSrc.GetConfIdx());
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

    if( ptConfMtTable == NULL || pcVcInst == NULL )
    {
        MpManagerLog("StartSwitchToSubMt() param err. ptMtTable.0x%x, ptVcInst.0x%x!\n", ptConfMtTable, pcVcInst);
        return FALSE;
    }

    //得到交换源地址
    if( !GetSwitchInfo( tSrc, dwSrcMpIp, wRcvPort, dwSrcIp ) )
    {
        MpManagerLog("StartSwitchToSubMt() failure because of can't get switch info!\n");
        return FALSE ;
	}
    //根据信道号计算偏移量
    wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;

    CServMsg cServMsg;
    cServMsg.SetConfIdx(tSrc.GetConfIdx());    
    TSwitchChannel tSwitchChannel;
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
            // 每次处理当前MP接入的终端
            byMpId = ptConfMtTable->GetMpId( ptDstBase[byMtLoop].GetMtId() );
            if ( byMpId != byMpLoop )
            {
                continue;
            }

            // zbq [06/29/2007] 回传源的广播交换不能替换其普通回传交换
            if (!pcVcInst->m_tCascadeMMCU.IsNull()  &&
                 pcVcInst->m_ptConfOtherMcTable->GetMcInfo(pcVcInst->m_tCascadeMMCU.GetMtId())->m_tSpyMt == tSrc &&
                 pcVcInst->m_tCascadeMMCU == ptDstBase[byMtLoop] )
            {
                MpManagerLog( "[StartSwitchFromBrd] tSrc.%d create no brd switch to MMcu.%d\n", 
                               tSrc.GetMtId(), pcVcInst->m_tCascadeMMCU.GetMtId() );
                continue;
            }

            byManuID = ptConfMtTable->GetManuId(ptDstBase[byMtLoop].GetMtId());     
            ptConfMtTable->GetMtLogicChnnl(ptDstBase[byMtLoop].GetMtId(), LOGCHL_VIDEO, &tLogicChannel, TRUE);
            
            if (( TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_VMP == tSrc.GetEqpType() && MT_TYPE_MT != ptDstBase[byMtLoop].GetMtType() &&
                ptConfInfo->GetConfAttrb().IsResendLosePack()) ||
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
            tSwitchChannel.SetSrcMt( tSrc );
            tSwitchChannel.SetSrcIp( dwSrcIp );
            tSwitchChannel.SetRcvIP( dwRcvIp );
            tSwitchChannel.SetRcvPort( wRcvPort );
            tSwitchChannel.SetDisIp( dwDisIp );
            tSwitchChannel.SetRcvBindIP( dwRcvIp );
            tSwitchChannel.SetDstIP( tLogicChannel.m_tRcvMediaChannel.GetIpAddr() );
        	tSwitchChannel.SetDstPort( tLogicChannel.m_tRcvMediaChannel.GetPort() );
            
            // 准备给当前MP的消息体
            cServMsg.CatMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));                  
            cServMsg.CatMsgBody((u8 *)&bySwitchChannelMode, sizeof(u8));
            cServMsg.CatMsgBody((u8 *)&byUniformPayload, sizeof(u8));            
            cServMsg.CatMsgBody((u8 *)&byIsSwitchRtcp, sizeof(u8));

            MpManagerLog("Send MCU_MP_ADDBRDDSTSWITCH_REQ for Mt.%d(%s:%d) to Mp.%d \n",
                ptDstBase[byMtLoop].GetMtId(),
                StrOfIP(tSwitchChannel.GetDstIP()), tSwitchChannel.GetDstPort(), byMpLoop);
        }
        if ( cServMsg.GetMsgBodyLen() > 0 )
        {
            g_cMpSsnApp.SendMsgToMpSsn( byMpLoop, 
                                        MCU_MP_ADDBRDDSTSWITCH_REQ, 
                                        cServMsg.GetServMsg(), 
                                        cServMsg.GetServMsgLen());        
        }
    }

    return TRUE;
}

/*====================================================================
    函数名      ：StartSwitchEqpFromBrd
    功能        ：将广播源的码流交换给外设
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byEqpId, 交换目标
    返回值说明  ：TRUE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/08/07    4.0         张宝卿        创建
====================================================================*/
BOOL32 CMpManager::StartSwitchEqpFromBrd( const TMt &tSrc, u8 bySrcChnnl, u8 byEqpId, u16 wDstChnnl, u8 bySwitchMode )
{
    u8  byChannelNum = 0;
    u32 dwDisIp = 0;
    u32 dwSrcIp = 0;
    u32 dwRcvIp = 0;
    u16 wRcvPort = 0;
    u32 dwDstIp = 0;
    u16 wDstPort = 0;
    TLogicalChannel	tLogicalChannel;

        //得到交换源地址
    if (!GetSwitchInfo(tSrc, dwRcvIp, wRcvPort, dwSrcIp))
    {
        MpManagerLog("StartSwitchEqpFromBrd() failure because of can't get switch info!");
        return FALSE;
    }

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
    }

    // 需要转发
    if (SWITCH_MODE_BROADCAST == bySwitchMode || EQP_TYPE_RECORDER == byEqpType
        /* || 
        // zbq [06/14/2007] VMP交换策略调整
        EQP_TYPE_VMP == byEqpType*/ )
    {
        if (dwMpIP != dwRcvIp)
        {
            dwDisIp = dwRcvIp;
            dwRcvIp = dwMpIP;
        }
    }
    
    wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;

    if (g_cMcuVcApp.GetPeriEqpLogicChnnl(byEqpId, MODE_VIDEO, &byChannelNum, &tLogicalChannel, TRUE))
    {
        CServMsg cServMsg;
        cServMsg.SetConfIdx( tSrc.GetConfIdx() );
        
        dwDstIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
        wDstPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();
        
        TSwitchChannel tSwitchChannel;
        u8 bySwitchChannelMode = SWITCHCHANNEL_UNIFORMMODE_NONE;
        u8 byUniformPayload = INVALID_PAYLOAD;
        u8 byIsSwitchRtcp = 0;
        
        
        //构造交换信道
        tSwitchChannel.SetSrcMt( tSrc );
        tSwitchChannel.SetSrcIp( dwSrcIp );
        tSwitchChannel.SetRcvIP( dwRcvIp );
        tSwitchChannel.SetRcvPort( wRcvPort );
        tSwitchChannel.SetDisIp( dwDisIp );
        tSwitchChannel.SetRcvBindIP( dwRcvIp );
        tSwitchChannel.SetDstIP( dwDstIp );
        tSwitchChannel.SetDstPort( wDstPort + PORTSPAN * wDstChnnl );
        
        // 准备给当前MP的消息体
        cServMsg.CatMsgBody((u8 *)&tSwitchChannel, sizeof(tSwitchChannel));                  
        cServMsg.CatMsgBody((u8 *)&bySwitchChannelMode, sizeof(u8));
        cServMsg.CatMsgBody((u8 *)&byUniformPayload, sizeof(u8));            
        cServMsg.CatMsgBody((u8 *)&byIsSwitchRtcp, sizeof(u8));
        
        
        MpManagerLog("Send MCU_MP_ADDBRDDSTSWITCH_REQ for Eqp.%d(%s:%d)\n",
                      byEqpId,
                      StrOfIP(tSwitchChannel.GetDstIP()), tSwitchChannel.GetDstPort());
        
        BOOL32 bDstMpExist = FALSE;
        u8 byDstMpId = 0;
        for( u8 byMpId = 1; byMpId <= MAXNUM_DRI; byMpId ++ )
        {
            if ( g_cMcuVcApp.GetMpIpAddr(byMpId) == dwMpIP )
            {
                bDstMpExist = TRUE;
                byDstMpId = byMpId;
                break;
            }
        }
        if ( !bDstMpExist )
        {
            OspPrintf( TRUE, FALSE, "[StartSwitchEqpFromBrd] Dst Mp.0x%x Unexist\n", dwMpIP );
        }
        
        g_cMpSsnApp.SendMsgToMpSsn( byDstMpId, 
                                    MCU_MP_ADDBRDDSTSWITCH_REQ, 
                                    cServMsg.GetServMsg(), 
                                    cServMsg.GetServMsgLen());
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
BOOL32 CMpManager::StopSwitchToBrd( const TMt &tSrc, u8 bySrcChnnl )
{
    CServMsg cServMsg;
    TSwitchChannel tSwitchChannel;
    
    //得到交换源地址
    u32 dwSrcMpIp;
    u16 wRcvPort;
    u32 dwSrcIp;
    if( !GetSwitchInfo( tSrc, dwSrcMpIp, wRcvPort, dwSrcIp ) )
    {
        MpManagerLog("StopSwitchToBrd() failure because of can't get switch info!\n");
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

    MpManagerLog("Send MCU_MP_REMOVEBRDSRCSWITCH_REQ from RecvMp %s:%d to all Mp.\n",
                 StrOfIP(tSwitchChannel.GetRcvIP()), tSwitchChannel.GetRcvPort());

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
BOOL32 CMpManager::StartSwitchToSubMt( const TMt & tSrc, 
                                     u8        bySrcChnnl, 
                                     const TMt & tDst, 
                                     u8        byMode,
                                     u8        bySwitchMode, 
                                     BOOL32 bH239Chnnl,
									 BOOL32 bStopBeforeStart,
									 BOOL32 bSrcHDBas)
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
		 MpManagerLog("StartSwitchToSubMt() failure because of can't get switch info!\n");
		 return FALSE ;
	}
    
    TConfInfo * ptConfInfo = &g_cMcuVcApp.GetConfInstHandle(tSrc.GetConfIdx())->m_tConf;
    u8 byIsSwitchRtcp;
    if (( TYPE_MCUPERI == tSrc.GetType() && EQP_TYPE_VMP == tSrc.GetEqpType() && MT_TYPE_MT != tDst.GetMtType() &&
        ptConfInfo->GetConfAttrb().IsResendLosePack()) ||
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
        MpManagerLog("StartSwitchToSubMt() tDstMt is NULL! It's send-only MT?\n");        
		return TRUE;
	}

	//得到会议终端表指针
	TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable( tSrc.GetConfIdx() );
	if( ptConfMtTable == NULL )
	{
		MpManagerLog("StartSwitchToSubMt() failure because invalid ConfMtTable Pointer!\n");
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
            u8 byChannelType;
            TLogicalChannel  tChannel;
            memset(&tChannel, 0, sizeof(TLogicalChannel));
            //Get channal info
            if (0 == bySrcChnnl)
            {
                byChannelType = LOGCHL_VIDEO;
            }
            else if (1 == bySrcChnnl)
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
	//if(bySwitchMode==SWITCH_MODE_BROADCAST)
	{
		//得到目的Mt的交换信息
		u32  dwMpIP = 0;
		dwMpIP = g_cMcuVcApp.GetMpIpAddr( ptConfMtTable->GetMpId( tDst.GetMtId() ) );
		if( dwMpIP != dwRcvIp )//需要转发
		{
			dwDisIp = dwRcvIp;
			dwRcvIp = dwMpIP;
		}
	}

	//根据信道号计算偏移量
	wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;    //双流要调整

	//图像
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		//建立交换
		if( ptConfMtTable->GetMtLogicChnnl( tDst.GetMtId(), LOGCHL_VIDEO, &tLogicalChannel, TRUE ) )	//可以交换
		{
            bResult1 = StartSwitch( tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort, tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp, bStopBeforeStart);
			if(!bResult1)MpManagerLog("Switch video failure!\n");

			/*
#ifdef  _SATELITE_
			//卫星会议同步打到卫星广播点
			//目前主流独立走，适配流走此处
			if (SWITCH_MODE_BROADCAST == bySwitchMode)
			{
				if ( bySrcChnnl != 0 )
				{
					StartSatConfCast(tSrc, MODE_VIDEO, bySrcChnnl);
				}
			}
			else
			{
				if ( bySrcChnnl != 0 )
				{
					StartSwitchSrc2Dst(tSrc, tDst, MODE_VIDEO, bySrcChnnl);
				}
			}
#endif
			*/
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
										   tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp);
					/*
			#ifdef  _SATELITE_
					//卫星会议同步打到卫星广播点
					//目前主流独立走，适配流走此处
					if (SWITCH_MODE_BROADCAST == bySwitchMode)
					{
						StartSatConfCast(tSrc, MODE_SECVIDEO, bySrcChnnl);
					}
			#endif
			*/
				}
				else
				{
					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort+4,
										   tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp);					
				}
                if (!bResult1)
                {
                    MpManagerLog("Switch video h.239 failure!\n");
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
			bResult2 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, (wRcvPort+2),
                                   tLogicalChannel, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp);
			if(!bResult2)MpManagerLog("Switch audio failure!\n");
		}
	}	

	return  bResult1 && bResult2 ;
}

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
void CMpManager::StopSwitchToSubMt( const TMt & tDst, u8 byMode, BOOL32 bH239Chnnl )
{
	u8   byDstMtId = tDst.GetMtId();
	TLogicalChannel	tLogicalChannel;

	TConfMtTable *ptConfMtTable = g_cMcuVcApp.GetConfMtTable( tDst.GetConfIdx() );
	if( ptConfMtTable == NULL )
	{
		MpManagerLog("StopSwitchToSubMt() failure because invalid ConfMtTable Pointer!\n");
		return ;
	}

    TConfInfo * ptConfInfo = &g_cMcuVcApp.GetConfInstHandle(tDst.GetConfIdx())->m_tConf;
    u8 byIsSwitchRtcp;
    if (MT_TYPE_MT != tDst.GetMtType() && ptConfInfo->GetConfAttrb().IsResendLosePack())
    {
        byIsSwitchRtcp = 0;
    }
    else
    {
        byIsSwitchRtcp = 1;
    }

    //图像
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		if( ptConfMtTable->GetMtLogicChnnl( byDstMtId, LOGCHL_VIDEO, &tLogicalChannel, TRUE ) )	//已登记
		{
			StopSwitch(tDst.GetConfIdx(), tLogicalChannel, 0, byIsSwitchRtcp);

			/*
#ifdef  _SATELITE_
			//卫星会议同步停止卫星广播
			//目前主流独立停止，适配流走此处
			if ( bySrcChnnl != 0 )
			{
				StopSatConfCast(tSrc, MODE_VIDEO, bySrcChnnl);
			}
			if ( bySrcChnnl != 0 )
			{
				StopSwitchSrc2Dst(tSrc, tDst, MODE_VIDEO, bySrcChnnl);
			}
#endif
			*/
		}
		
	}

    if (MODE_SECVIDEO == byMode)
	{
		//H.239视频逻辑通道
		if (bH239Chnnl)
		{
			if (ptConfMtTable->GetMtLogicChnnl(tDst.GetMtId(), LOGCHL_SECVIDEO, &tLogicalChannel, TRUE))
			{
				StopSwitch(tDst.GetConfIdx(), tLogicalChannel, 0, byIsSwitchRtcp);
			/*
#ifdef  _SATELITE_
				//卫星会议同步停止卫星广播点
				if (SWITCH_MODE_BROADCAST == bySwitchMode)
				{
					StopSatConfCast(tSrc, MODE_SECVIDEO, bySrcChnnl);
				}
#endif
			*/
			}
			return;
		}
	}

    //语音
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		if( ptConfMtTable->GetMtLogicChnnl( byDstMtId, LOGCHL_AUDIO, &tLogicalChannel, TRUE ) )	//已登记
		{
			StopSwitch(tDst.GetConfIdx(), tLogicalChannel, 0, byIsSwitchRtcp);
		}
	}
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
        dwSwitchIpAddr = ntohl( g_cMcuAgent.GetBoardIpAddrFromIdx(tRecInfo.GetSwitchBrdId()) );
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
    算法实现    ：
    引用全局变量：
    输入参数说明：tSrc, 源
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
====================================================================*/
BOOL32 CMpManager::StartSwitchToPeriEqp(const TMt & tSrc,
										u8	bySrcChnnl,
										u8	byEqpId,
										u16 wDstChnnl,
										u8	byMode,
										u8	bySwitchMode,
										BOOL32 bDstHDBas,
										BOOL32 bStopBeforeStart,
										BOOL32 bVCSConf)
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
        MpManagerLog("StartSwitchToPeriEqp() failure because of can't get switch info!\n");
        return FALSE;
    }
	
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
    }

    // 需要转发
    if (SWITCH_MODE_BROADCAST == bySwitchMode || EQP_TYPE_RECORDER == byEqpType)
    {
        if (dwMpIP != dwRcvIp)
        {
            dwDisIp = dwRcvIp;
            dwRcvIp = dwMpIP;
        }
    }

    wRcvPort = wRcvPort + PORTSPAN * bySrcChnnl;

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

    MpManagerLog("[StartSwitchToPeriEqp] bModify2Inner.%d, mcueqp recv ip: 0x%x\n", bModified2InnerIp, dwEqpIp);
#endif


    //图像
    if (MODE_VIDEO == byMode || MODE_BOTH == byMode)
    {
        //与baphd保持同步
        if (EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId) &&
            g_cMcuAgent.IsEqpBasHD(byEqpId))
        {
            wDstChnnl = wDstChnnl * 2;
        }

#ifndef _SATELITE_
        // zbq [08/04/2007] BAS交换同广播目标交还调整为同一逻辑
        if ( EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId) && bySwitchMode == SWITCH_MODE_BROADCAST &&
			 !bVCSConf )
        {
            StartSwitchEqpFromBrd( tSrc, bySrcChnnl, byEqpId, wDstChnnl, bySwitchMode );
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
#else
		StartSatConfCast(tSrc, CAST_UNI, MODE_VIDEO, bySrcChnnl);
		//StartSatConfCast(tSrc, CAST_DST, MODE_VIDEO, bySrcChnnl);

		if (g_cMcuVcApp.GetPeriEqpLogicChnnl(byEqpId, MODE_VIDEO, &byChannelNum, &tLogicalChannel, TRUE))
		{
			dwDstIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
			wDstPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();

			wRcvPort =  g_cMcuVcApp.GetMcuRcvMtMediaStartPort() + (tSrc.GetMtId()-1) * PORTSPAN;

			bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, wRcvPort,
									dwDstIp, (wDstPort + PORTSPAN * wDstChnnl), 0, 0, 
									SWITCHCHANNEL_UNIFORMMODE_NONE, INVALID_PAYLOAD, 1, bStopBeforeStart
									);// xliang [4/13/2009] modify for 4 last param
            }
#endif
    }

    //语音
    if (MODE_AUDIO == byMode || MODE_BOTH == byMode)
    {
        //与baphd保持同步
        if (EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId) &&
            g_cMcuAgent.IsEqpBasHD(byEqpId))
        {
            wDstChnnl = wDstChnnl * 2;
        }

#ifndef _SATELITE_
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
#else
		StartSatConfCast(tSrc, CAST_UNI, MODE_AUDIO, bySrcChnnl);
		//StartSatConfCast(tSrc, CAST_DST, MODE_AUDIO, bySrcChnnl);

		if (g_cMcuVcApp.GetPeriEqpLogicChnnl(byEqpId, MODE_AUDIO, &byChannelNum, &tLogicalChannel, TRUE))
        {
            dwDstIp = tLogicalChannel.m_tRcvMediaChannel.GetIpAddr();
            wDstPort = tLogicalChannel.m_tRcvMediaChannel.GetPort();

			wRcvPort =  g_cMcuVcApp.GetMcuRcvMtMediaStartPort() + (tSrc.GetMtId()-1) * PORTSPAN;
			
            bResult2 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, (wRcvPort + 2),
				dwDstIp, (wDstPort + PORTSPAN * wDstChnnl), 0, 0);
        }
#endif
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

            if (EQP_TYPE_PRS == byEqpType)
            {
                bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, (wRcvPort + 4),
                                       dwDstIp, (wDstPort + PORTSPAN * wDstChnnl), 0, 0);
            }
            else
            {
				//zbq[09/03/2008] 发给HD Bas的双流交换从 端口基址+PORTSPAN 开始建交换
				if (!bDstHDBas)
				{
					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, (wRcvPort + 4),
										   dwDstIp, (wDstPort + PORTSPAN * wDstChnnl + 4), 0, 0);					
				}
				else
				{
					bResult1 = StartSwitch(tSrc, tSrc.GetConfIdx(), dwSrcIp, dwDisIp, dwRcvIp, (wRcvPort + 4),
										   dwDstIp, (wDstPort + PORTSPAN * (wDstChnnl * 2)), 0, 0);
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
void CMpManager::StopSwitchToPeriEqp( u8 byConfIdx, u8 byEqpId, u16 wDstChnnl,  u8 byMode )
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
    
    MpManagerLog("[StopSwitchToPeriEqp] bModify2Inner.%d, mcueqp recv ip: 0x%x\n", bModified2InnerIp, dwEqpIp);
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
			// 目前仅高清bas通道间隔翻倍10*2
			u8 byMulPortSpace = 1;
			if (EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId) &&
				g_cMcuAgent.IsEqpBasHD(byEqpId))
			{
				byMulPortSpace = 2;
			}
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
            // 目前仅高清bas通道间隔翻倍10*2
            if (EQP_TYPE_BAS == g_cMcuVcApp.GetEqpType(byEqpId) &&
                g_cMcuAgent.IsEqpBasHD(byEqpId))
            {
                wDstChnnl = 2 * wDstChnnl;
			}
			StopSwitch( byConfIdx, tLogicalChannel, wDstChnnl );
		}
	}
    
    // 双流
    if ( byMode == MODE_SECVIDEO )
    {
        u8 byEqpType = g_cMcuAgent.GetPeriEqpType(byEqpId);
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
            else
            {
                //zbq[01/07/2009] 双流适配交换
                if (!g_cMcuAgent.IsEqpBasHD(byEqpId))
                {
                    StopSwitch( byConfIdx, 
                        tLogicalChannel.m_tRcvMediaChannel.GetIpAddr(),
                        tLogicalChannel.m_tRcvMediaChannel.GetPort() + PORTSPAN * wDstChnnl + 4, 
                        TRUE);
                }
                else
                {
                    StopSwitch( byConfIdx, 
                        tLogicalChannel.m_tRcvMediaChannel.GetIpAddr(),
                        tLogicalChannel.m_tRcvMediaChannel.GetPort() + PORTSPAN * wDstChnnl * 2, 
                        TRUE);
                }
            }   

		}        
    }
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
                             u8 byIsSwitchRtcp, BOOL32 bStopBeforeStart)
{
	//根据IP地址查找交换MP编号
	u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	//没找到
	if( 0 == byMpId )
	{
        MpManagerLog("No MP's Addrs is 0x%x in StartSwitch(),please check switch recv IP!\n",dwRcvIp);
		return FALSE;
	}

	//构造交换信道 
	TSwitchChannel tSwitchChannel;
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
    MpManagerLog("Send Message: MCU_MP_ADDSWITCH_REQ for %s:%d --> %s:%d to Mp.\n",
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

    MpManagerLog("Send Message: MCU_MP_REMOVESWITCH_REQ for %s:%d to Mp in StopSwitch().\n",
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

    MpManagerLog("Send Message: MCU_MP_REMOVESWITCH_REQ for %s:%d to Mp in StartStopSwitch() .\n",
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
                              u8 byIsSwitchRtcp, BOOL32 bStopBeforeStart)
{
	return StartSwitch(tSrcMt, byConfIdx, dwSrcIp, dwDisIp, dwRcvIp, wRcvPort, 
		               tFwdChnnl.m_tRcvMediaChannel.GetIpAddr(),
		               (tFwdChnnl.m_tRcvMediaChannel.GetPort() + PORTSPAN*wDstChnnl),
                       0, 0, bySwitchChannelMode, byUniformPayload, byIsSwitchRtcp, bStopBeforeStart);
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
	
	//没找到
	if(byMpId==0)
	{
        MpManagerLog("No MP's Addrs is 0x%x in AddMultiToOneSwitch(),please check switch recv IP!",dwRcvIp);
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
        MpManagerLog("No MP's Addrs is 0x%x in RemoveMultiToOneSwitch(),please check switch recv IP!\n",dwRcvIp);
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
====================================================================*/
BOOL32 CMpManager::AddRecvOnlySwitch( u8 byConfIdx, u32 dwRcvIp, u16 wRcvPort )
{
	//根据IP地址查找交换MP编号
	u8 byMpId = g_cMcuVcApp.FindMp( dwRcvIp );
	
	//没找到
	if(byMpId==0)
	{
		MpManagerLog("No MP's Addrs is 0x%x in AddRecvOnlySwitch(),please check switch recv IP!\n",dwRcvIp);
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
		MpManagerLog("No MP's Addrs is 0x%x in RemoveRecvOnlySwitch(),please check switch recv IP!\n",dwRcvIp);
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
		 MpManagerLog("StartRecvMt() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//图像
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		//建立dump交换
		bResult1 = AddRecvOnlySwitch( tMt.GetConfIdx(), dwRcvIp, wRcvPort );
		if(!bResult1)
		{
			MpManagerLog("Switch video dump failure!\n");
		}
	}
		
	//语音
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		//建立dump交换
		bResult2 = AddRecvOnlySwitch( tMt.GetConfIdx(), dwRcvIp, wRcvPort+2 );
		if(!bResult2)
		{
			MpManagerLog("Switch audio dump failure!\n");
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
		 MpManagerLog("StopRecvMt() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//图像
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		//拆除dump交换
		bResult1 = RemoveRecvOnlySwitch( tMt.GetConfIdx(), dwRcvIp, wRcvPort );
		if(!bResult1)
		{
			MpManagerLog("Stop switch video dump failure!\n");
		}
	}
		
	//语音
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		//拆除dump交换
		bResult2 = RemoveRecvOnlySwitch( tMt.GetConfIdx(), dwRcvIp, wRcvPort+2 );
		if(!bResult2)
		{
			MpManagerLog("Stop switch audio dump failure!\n");
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
        MpManagerLog("No MP's Addrs is 0x%x in SetRecvMtSSRCValue(),please check switch recv IP!\n",dwRcvIp);
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
		 MpManagerLog("ResetRecvMtSSRC() failure because of can't get switch info!\n");
		 return FALSE ;
	}

	//图像
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		bResult1 = SetRecvMtSSRCValue( tMt.GetConfIdx(), dwRcvIp, wRcvPort, TRUE );
		if(!bResult1)
		{
			MpManagerLog("ResetRecvMtSSRC video switch failure!\n");
		}
	}
	//语音
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		bResult2 = SetRecvMtSSRCValue( tMt.GetConfIdx(), dwRcvIp, wRcvPort+2, TRUE );
		if(!bResult2)
		{
			MpManagerLog("ResetRecvMtSSRC audio switch failure!\n");
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
		 MpManagerLog("RestoreRecvMtSSRC() failure because of can't get switch info!\n");
		 return FALSE;
	}

	//图像
	if( byMode == MODE_VIDEO || byMode == MODE_BOTH )
	{
		bResult1 = SetRecvMtSSRCValue( tMt.GetConfIdx(), dwRcvIp, wRcvPort, FALSE );
		if(!bResult1)
		{
			MpManagerLog("RestoreRecvMtSSRC video switch failure!\n");
		}
	}
		
	//语音
	if( byMode == MODE_AUDIO || byMode == MODE_BOTH )
	{
		bResult2 = SetRecvMtSSRCValue( tMt.GetConfIdx(), dwRcvIp, wRcvPort+2, FALSE );
		if(!bResult2)
		{
			MpManagerLog("RestoreRecvMtSSRC audio switch failure!\n");
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

    //multicast
//    u8 byNetNo = g_cMcuAgent.GetCriDriNetChoice(tMp.GetMpId());
    u8 byIsMulticast = g_cMcuAgent.GetCriDriIsCast(tMp.GetMpId()) ? 1:0;
//    tMp.SetNetNo(byNetNo);
    tMp.SetMulticast(byIsMulticast);

    g_cMcuVcApp.AddMp(tMp);

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
                if( NULL == g_cMcuVcApp.GetConfInstHandle( byConfIdx ) )
                {
                    continue;
                }
                else
                {
                    ptConfInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConf;
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
void CMpManager::ProcMtAdpDisconnect(const CMessage * pcMsg)
{
	CServMsg cServMsg( pcMsg->content ,pcMsg->length );	
	TMtAdpReg tMtAdpReg =  *( TMtAdpReg* )cServMsg.GetMsgBody();
	u8 byRRQReset = *( cServMsg.GetMsgBody() + sizeof(TMtAdpReg) );
	g_cMcuVcApp.RemoveMtAdp( tMtAdpReg.GetDriId() );
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
                if( NULL == g_cMcuVcApp.GetConfInstHandle( byConfIdx ) )
                {
                    continue;
                }
                else
                {
                    ptConfInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConf;

                    // 对于GK计费会议特别处理, zgc, 2008-09-26
                    // 等待主适配板注册上来后，重新开始注册计费会议
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

		//清空GKID/EPID信息
		memset((void*)&g_cMcuVcApp.m_tGKID, 0, sizeof(TH323EPGKIDAlias));
		memset((void*)&g_cMcuVcApp.m_tEPID, 0, sizeof(TH323EPGKIDAlias));
			
		//用于注册的适配模块板断链，则进行注册迁移，重新选择适配模块板 注册MCU及会议实体
//		cServMsg.SetDstDriId( byMinDri );
//		if( 0 != byMinDri && byMinDri <= MAXNUM_DRI )
//		{
//			cServMsg.SetMsgBody( (u8*)&byMinDri, sizeof( byMinDri ) );
//			g_cMtAdpSsnApp.SendMsgToMtAdpSsn( byMinDri, MCU_MT_REREGISTERGK_REQ, cServMsg );
//		}
	}
	
	return;
}

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
                if( NULL == g_cMcuVcApp.GetConfInstHandle( byConfIdx ) )
                {
                    continue;
                }
                else
                {
                    ptConfInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConf;
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
    OspPrintf(TRUE,FALSE,"\n------------------MCU %s register MP list----------------", achMcuAlias );
	for( byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		if(g_cMcuVcApp.m_atMpData[byLoop].m_bConnected)
		{
            OspPrintf(TRUE,FALSE, "\nMp%d: %s (Band: %d/%d[Total/Real] P.K, NetTraffic: %d/%d/%d[Total/Real/Reserved] Mbps)", 
                                  byLoop+1, 
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
	OspPrintf(TRUE,FALSE,"\nTotal Register MP num: %d \n",nMpSum);
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
    OspPrintf(TRUE,FALSE,"\n------------------MCU %s register MtAdp list----------------", achMcuAlias );
	for( byLoop = 0; byLoop < MAXNUM_DRI; byLoop++)
	{
		if( g_cMcuVcApp.m_atMtAdpData[byLoop].m_bConnected )
		{
            byMtNum = 0;
            bySMcuNum = 0;
            g_cMcuVcApp.GetMtNumOnDri(byLoop+1, TRUE, byMtNum, bySMcuNum);
            OspPrintf( TRUE,FALSE, "\nMtAdp%d: %s (MaxMT/SMcu||OnlineMt/SMcu/All: %d/%d||%d/%d/%d)", byLoop+1, 
				       StrOfIP( g_cMcuVcApp.GetMtAdpIpAddr( byLoop+1 ) ),
                       g_cMcuVcApp.m_atMtAdpData[byLoop].m_byMaxMtNum,
                       g_cMcuVcApp.m_atMtAdpData[byLoop].m_byMaxSMcuNum,
                       byMtNum,
                       bySMcuNum,
                       g_cMcuVcApp.m_atMtAdpData[byLoop].m_wMtNum);
            
            if ( ISTRUE(g_cMcuVcApp.m_atMtAdpData[byLoop].m_byIsSupportHD) )
            {
                OspPrintf( TRUE,FALSE," (HD)" );
            }

			nMtAdpSum++;
		}		
	}
	OspPrintf(TRUE,FALSE,"\nTotal Register MtAdp num: %d", nMtAdpSum);
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
        if(NULL == g_cMcuVcApp.GetConfInstHandle( byConfIdx ))
        {
            continue;
        }

        ptConfFullInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConf;
		if( ptConfFullInfo->m_tStatus.IsOngoing() )
		{   
			OspPrintf( TRUE, FALSE, "\n--------Conf: %s bridge info--------\n", ptConfFullInfo->GetConfName() );
		    ptSwitchTable = g_cMcuVcApp.GetConfSwitchTable( byConfIdx );
			for( byLoop1 = 1; byLoop1 <= MAXNUM_DRI; byLoop1++ )
			{
				if( g_cMcuVcApp.IsMpConnected( byLoop1 ) )
				{
					OspPrintf( TRUE, FALSE, "\nMp%d switch info: ", byLoop1 );
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

								OspPrintf( TRUE, FALSE, achTemp );
							}
						}
					}
				}
			}
		}
	}
}

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
	TConfSwitchTable *ptSwitchTable;
	u8  byMpId, byLoop1;
    u16 wLoop2;
	u32 dwSrcIp, dwDisIp, dwRcvIp, dwDstIp;
	u16 wRcvPort, wDstPort;
	TSwitchChannel *ptSwitchChannel;
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

		ptConfFullInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConf;
		if( ptConfFullInfo->m_tStatus.IsOngoing() )
		{   
            // 获得视频广播源, zgc, 2008-07-23
            tVidBrdSrc = g_cMcuVcApp.GetConfInstHandle( byConfIdx )->GetVidBrdSrc();
            if ( !tVidBrdSrc.IsNull() )
            {
                GetSwitchInfo( tVidBrdSrc, dwSwitchIp, wBrdSrcRcvPort, dwSwitchSrcIp );
            }

			OspPrintf( TRUE, FALSE, "\n--------Conf: %s data switch info--------\n", ptConfFullInfo->GetConfName() );
		    ptSwitchTable = g_cMcuVcApp.GetConfSwitchTable( byConfIdx );
			for( byLoop1 = 1; byLoop1 <= MAXNUM_DRI; byLoop1++ )
			{				
                // guzh [3/29/2007] 支持打印特殊交换。参数为1或者2，就打印特殊信息(广播源/Rtcp)
				if( g_cMcuVcApp.IsMpConnected( byLoop1 ) ||
                    ( ( byType == 1 || byType == 2) &&
					  ( (bIsMiniMcu && byLoop1 == 1 && !g_cMcuVcApp.IsMpConnected( byLoop1 )) ||
                      ( ( byLoop1 == MCU_BOARD_MPC || byLoop1 == MCU_BOARD_MPCD ) && 
						( byLoop1 != byMpcBrdId && !bIsMiniMcu ) ) ) 
					)
                   )
				{
					OspPrintf( TRUE, FALSE, "\nMp%d(%s) switch info: ", byLoop1 , StrOfIP(g_cMcuVcApp.GetMpIpAddr(byLoop1)));
					for( wLoop2 = 0; wLoop2 < MAX_SWITCH_CHANNEL; wLoop2++ )
					{
						ptSwitchChannel = ptSwitchTable->GetSwitchChannel( byLoop1, wLoop2 );
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
                                wRcvPort = wBrdSrcRcvPort;
                            }

							byMpId = g_cMcuVcApp.FindMp( dwSrcIp );
							if( byMpId > 0 )
							{
								nLen = sprintf( achTemp, "\n[%d]  %s:%d(mp%d)", wLoop2, StrOfIP( dwSrcIp ), wRcvPort, byMpId );
							}
							else
							{
								nLen = sprintf( achTemp, "\n[%d]  %s:%d(src)", wLoop2, StrOfIP( dwSrcIp ), wRcvPort );
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
								OspPrintf( TRUE, FALSE, "%s\n", achTemp );
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
                                wDstPort = wBrdSrcRcvPort;
                            }

							if( byMpId > 0 )
							{
								nLen += sprintf( achTemp+nLen, "-->%s:%d(mp%d)", StrOfIP( dwDstIp ), wDstPort, byMpId );
							}
							else
							{
								nLen += sprintf( achTemp+nLen, "-->%s:%d(dst)", StrOfIP( dwDstIp ), wDstPort );
							}

							OspPrintf( TRUE, FALSE, achTemp );
						
						}
					}
					OspPrintf( TRUE, FALSE, "\n" );
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
		OspPrintf( TRUE, FALSE, "[GetMPSwitchInfo] Err: byMpId(%d) is invalid \n", byMpId );
		return;
	}
	if( FALSE == g_cMcuVcApp.IsMpConnected( byMpId ) )
	{
		OspPrintf( TRUE, FALSE, "[GetMPSwitchInfo] Err: byMpId(%d) is not connect \n", byMpId );
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
        
        ptConfFullInfo = &g_cMcuVcApp.GetConfInstHandle( byConfIdx )->m_tConf;        
		if( ptConfFullInfo->m_tStatus.IsOngoing() )
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

// END OF FILE
