/*****************************************************************************
   模块名      : 高清解码卡
   文件名      : hdu2inst.cpp
   相关文件    : hdu2inst.h
   文件实现功能: HDU2类实现
   作者        : 田志勇
   版本        : V1.0  Copyright(C) 2011-2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2011/11/28  4.7         田志勇       创建
******************************************************************************/
#include "hdu2eventid.h"
#include "hdu2inst.h"

/*lint -save -e843*/
CHdu2App g_cHdu2App;
BOOL32 g_bPrintFrameInfo = FALSE;
extern u8     g_byHduPrintLevel;
/*=============================================================================
	函数  : hdulog
	功能  : 打印
	输入  : 
	输出  : 无
	返回  : 无
	注    : 
  -----------------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人      修改内容
	2011/11/28  4.7         田志勇        创建
=============================================================================*/
/*lint -save -e530 */
/*lint -save -esym(438, argptr)*/
/*lint -save -esym(526, __builtin_va_start)*/
/*lint -save -esym(628, __builtin_va_start)*/
void hdu2log( const u8 byLevel, const u16 wModule, const s8* pszFmt, ...)
{
	s8 achBuf[1024] = {0};
    va_list argptr;		      
    va_start( argptr, pszFmt );
	vsnprintf(achBuf , 1024, pszFmt, argptr);
	LogPrint( byLevel, wModule, achBuf );
	va_end(argptr); 
    return;
}


SEMHANDLE	g_hIpToStr = NULL;						// StrOfIP专用
/*=============================================================================
	函数  : IpToStr
	功能  : 转换Ip为字符串
	输入  : 
	输出  : 无
	返回  : 无
	注    : 
  -----------------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人      修改内容
	2013/09/03				陈兵        创建
=============================================================================*/
s8 *IpToStr( u32 dwIP )
{
	if (NULL == g_hIpToStr)
	{
		if( !OspSemBCreate(&g_hIpToStr))
		{
			OspSemDelete( g_hIpToStr );
			g_hIpToStr = NULL;
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[StrOfIP] create g_hStrOfIP failed!\n" );
		}
	}
	
	if (NULL != g_hIpToStr)
	{
		OspSemTake(g_hIpToStr);
	}	
	
    dwIP = htonl(dwIP);
	static char strIP[17];  
	u8 *p = (u8 *)&dwIP;
	sprintf(strIP,"%d.%d.%d.%d%c",p[0],p[1],p[2],p[3],0);
	
	if (NULL != g_hIpToStr)
	{
		OspSemGive(g_hIpToStr);
	}
	
	return strIP;
}


/*=============================================================================
	函数  : hdulog
	功能  : 打印
	输入  : 
	输出  : 无
	返回  : 无
	注    : 
  -----------------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人      修改内容
	2013/03/11				陈兵        创建
=============================================================================*/
void Hdu2Print( const u8 byLevel, const s8* pszFmt, ...)
{
	s8 achBuf[1024] = { 0 };
    va_list argptr;		      
    va_start( argptr, pszFmt );
	vsnprintf(achBuf, 1024, pszFmt, argptr);
    va_end(argptr); 
	
	if (g_byHduPrintLevel >= byLevel)
	{
		if ( byLevel <= LOG_LVL_DETAIL)
		{
			LogPrint(byLevel, MID_PUB_ALWAYS, achBuf);
		}
		else
		{
			OspPrintf(TRUE, FALSE, achBuf);
		}
	}

    return;
}
/*lint -restore*/

/*=============================================================================
  函 数 名： CBVidRecvFrame    
  功    能： 处理收到的视频帧数据
  算法实现： 
  全局变量： 
  参    数： PFRAMEHDR pFrmHdr, u32 dwContext
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2011/11/28  4.7         田志勇       创建
=============================================================================*/
static void CBVidRecvFrame(PFRAMEHDR pFrmHdr, u32 dwContext)
{
	//CHdu2ChnMgrGrp *pHdu2ChnMgrGrp = (CHdu2ChnMgrGrp*)dwContext;
	CRcvData *pcRcvData = (CRcvData *)dwContext;

    if ( NULL == pFrmHdr || NULL == pcRcvData )
    {
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[CBVidRecvFrame] PTFrameHeader 0x%x, dwContext 0x%x\n", pFrmHdr, dwContext);
        return ;
    }

	u8 byHduSubChnId = pcRcvData->m_byHduSubChnId;

	TFrameHeader tFrameHdr;
	memset(&tFrameHdr, 0x00, sizeof(tFrameHdr));
	tFrameHdr.m_dwMediaType = pFrmHdr->m_byMediaType;
	tFrameHdr.m_dwFrameID   = pFrmHdr->m_dwFrameID;
    tFrameHdr.m_dwSSRC      = pFrmHdr->m_dwSSRC;
	tFrameHdr.m_pData       = pFrmHdr->m_pData + pFrmHdr->m_dwPreBufSize; 
	tFrameHdr.m_dwDataSize  = pFrmHdr->m_dwDataSize;
	tFrameHdr.m_tVideoParam.m_bKeyFrame    = pFrmHdr->m_tVideoParam.m_bKeyFrame;
	tFrameHdr.m_tVideoParam.m_wVideoHeight = pFrmHdr->m_tVideoParam.m_wVideoHeight;
	tFrameHdr.m_tVideoParam.m_wVideoWidth  = pFrmHdr->m_tVideoParam.m_wVideoWidth;
	//pHdu2ChnMgrGrp->SetData(MODE_VIDEO, tFrameHdr);
	pcRcvData->m_pcHdu2ChnMgrGrp->SetData(MODE_VIDEO, tFrameHdr, byHduSubChnId);

	if (g_bPrintFrameInfo)
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CBVidRecvFrame] MediaType:%u, FrameID:%u, KF:%d, W*H:%d*%d, SSRC:%u, DataSize:%u\n",
			tFrameHdr.m_dwMediaType,tFrameHdr.m_dwFrameID,tFrameHdr.m_tVideoParam.m_bKeyFrame,tFrameHdr.m_tVideoParam.m_wVideoWidth,
			tFrameHdr.m_tVideoParam.m_wVideoHeight,tFrameHdr.m_dwSSRC,tFrameHdr.m_dwDataSize);
	}
	return;
}
/*=============================================================================
  函 数 名： CBAudRecvFrame    
  功    能： 处理收到的音频帧数据
  算法实现： 
  全局变量： 
  参    数： PFRAMEHDR pFrmHdr, u32 dwContext
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2011/11/28  4.7         田志勇       创建
=============================================================================*/
static void CBAudRecvFrame(PFRAMEHDR pFrmHdr, u32 dwContext)
{
	CHdu2ChnMgrGrp *pHdu2ChnMgrGrp = (CHdu2ChnMgrGrp*)dwContext;
    if ( NULL == pFrmHdr || NULL == pHdu2ChnMgrGrp )
    {
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[CBAudRecvFrame] PTFrameHeader 0x%x, dwContext 0x%x\n", pFrmHdr, dwContext);
        return ;
    }
	TFrameHeader tFrameHdr;
	memset(&tFrameHdr, 0x00, sizeof(tFrameHdr));
	tFrameHdr.m_dwMediaType = pFrmHdr->m_byMediaType;
	tFrameHdr.m_dwFrameID   = pFrmHdr->m_dwFrameID;
    tFrameHdr.m_dwSSRC      = pFrmHdr->m_dwSSRC;
	tFrameHdr.m_pData       = pFrmHdr->m_pData + pFrmHdr->m_dwPreBufSize; 
	tFrameHdr.m_dwDataSize  = pFrmHdr->m_dwDataSize;
    tFrameHdr.m_dwAudioMode = pFrmHdr->m_byAudioMode;
	pHdu2ChnMgrGrp->SetData(MODE_AUDIO,tFrameHdr);
	if (g_bPrintFrameInfo)
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CBAudRecvFrame] MediaType:%u, FrameID:%u, AudioMode:%u, SSRC:%u, DataSize:%u\n",
			tFrameHdr.m_dwMediaType,tFrameHdr.m_dwFrameID,tFrameHdr.m_dwAudioMode,tFrameHdr.m_dwSSRC,tFrameHdr.m_dwDataSize);
	}
	return;
}
/*=============================================================================
  函 数 名： GetVidPlyPortTypeByChnAndType    
  功    能： 视频输出口类型转换接口
  算法实现： 
  全局变量： 
  参    数： u16 wChnIdx
  返 回 值： u32
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2011/11/28  4.7         田志勇       创建
=============================================================================*/
u32 GetVidPlyPortTypeByChnAndType(u16 wChnIdx,u32 dwType)
{
	u32 dwVidPlyPortType = 0;
	switch(dwType) {
	case HDU_OUTPUT_YPbPr:
		dwVidPlyPortType = (wChnIdx == 0 ? VIDIO_YPbPr0 : VIDIO_YPbPr1);
		break;
	case HDU_OUTPUT_VGA:
		dwVidPlyPortType = (wChnIdx == 0 ? VIDIO_VGA0 : VIDIO_VGA1);
		break;
	case HDU_OUTPUT_DVI:
		dwVidPlyPortType = (wChnIdx == 0 ? VIDIO_DVI0 : VIDIO_DVI1);
		break;
	case HDU_OUTPUT_HDMI:
		dwVidPlyPortType = (wChnIdx == 0 ? VIDIO_HDMI0 : VIDIO_HDMI1);
		break;
	case HDU_OUTPUT_SDI:
		dwVidPlyPortType = (wChnIdx == 0 ? VIDIO_SDI0 : VIDIO_SDI1);
		break;
	case HDU_OUTPUT_C:
		dwVidPlyPortType = (wChnIdx == 0 ? VIDIO_C0 : VIDIO_C1);
		break;
	default:
		Hdu2Print(HDU_LVL_WARNING,  "[ProcChangeChnCfg]Unknown PortType!\n");
		return 0;
	}
	return dwVidPlyPortType;
}
/*=============================================================================
  函 数 名： CheckChnCfgParam    
  功    能： 检查参数是否合法
  算法实现： 
  全局变量： 
  参    数： THduModePort &tHdu2Cfg BOOL32 bIsHdu2_l
  返 回 值： BOOL32
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2013/02/01  4.7         田志勇       创建
=============================================================================*/
BOOL32 CheckChnCfgParam(THduModePort &tChnCfg,BOOL32 bIsHdu2_l)
{
	u8 byOutPortType = tChnCfg.GetOutPortType();
	u8 byOutModeType = tChnCfg.GetOutModeType();
	if (bIsHdu2_l)//HDU2_L
	{
		if (byOutPortType != HDU_OUTPUT_C)
		{
			tChnCfg.SetOutPortType(HDU_OUTPUT_C);
			tChnCfg.SetOutModeType(HDU_C_576i_50HZ);
			tChnCfg.SetZoomRate(HDU_ZOOMRATE_4_3);
			tChnCfg.SetScalingMode(HDU2_SHOWMODE_NONGEOMETRIC);
			Hdu2Print(HDU_LVL_WARNING,  "[CheckChnCfgParam]HDU2_L Only Support HDU_OUTPUT_C(%d) HDU_C_576i_50HZ(%d) OR HDU_C_480i_60HZ(%d)!!",
				HDU_OUTPUT_C,HDU_C_576i_50HZ,HDU_C_480i_60HZ);
			return FALSE;
		}
		if (tChnCfg.GetOutModeType() != HDU_C_576i_50HZ && tChnCfg.GetOutModeType() != HDU_C_480i_60HZ)
		{
			tChnCfg.SetOutModeType(HDU_C_576i_50HZ);
			tChnCfg.SetZoomRate(HDU_ZOOMRATE_4_3);
			tChnCfg.SetScalingMode(HDU2_SHOWMODE_NONGEOMETRIC);
			Hdu2Print(HDU_LVL_WARNING,  "[CheckChnCfgParam]HDU2_L Only Support HDU_OUTPUT_C(%d) HDU_C_576i_50HZ(%d) OR HDU_C_480i_60HZ(%d)!!",
				HDU_OUTPUT_C,HDU_C_576i_50HZ,HDU_C_480i_60HZ);
			return FALSE;
		}
	}
	else//HDU2
	{
		if (byOutPortType != HDU_OUTPUT_YPbPr && byOutPortType != HDU_OUTPUT_DVI &&
			byOutPortType != HDU_OUTPUT_HDMI && byOutPortType != HDU_OUTPUT_VGA &&
			byOutPortType != HDU_OUTPUT_SDI && byOutPortType != HDU_OUTPUT_C)
		{
			tChnCfg.SetOutPortType(HDU_OUTPUT_YPbPr);
			tChnCfg.SetOutModeType(HDU_YPbPr_1080i_50HZ);
			tChnCfg.SetZoomRate(HDU_ZOOMRATE_16_9);
			tChnCfg.SetScalingMode(HDU2_SHOWMODE_NONGEOMETRIC);
			Hdu2Print(HDU_LVL_WARNING,  "[CheckChnCfgParam]HDU2 Not support(%d)(%d)!!",tChnCfg.GetOutPortType(),byOutModeType);
			return FALSE;
		}
		if ((byOutPortType == HDU_OUTPUT_YPbPr || byOutPortType == HDU_OUTPUT_DVI ||
			byOutPortType == HDU_OUTPUT_HDMI || byOutPortType == HDU_OUTPUT_SDI) && 
			byOutModeType != HDU_YPbPr_1080P_24fps && byOutModeType != HDU_YPbPr_1080P_25fps && 
			byOutModeType != HDU_YPbPr_1080P_29970HZ && byOutModeType != HDU_YPbPr_1080P_30fps && 
			byOutModeType != HDU_YPbPr_1080P_50fps && byOutModeType != HDU_YPbPr_1080P_59940HZ && 
			byOutModeType != HDU_YPbPr_1080P_60fps&& byOutModeType != HDU_YPbPr_1080i_50HZ && 
			byOutModeType != HDU_YPbPr_1080i_60HZ&& byOutModeType != HDU_YPbPr_720P_50fps && 
			byOutModeType != HDU_YPbPr_720P_60fps && byOutModeType != HDU_YPbPr_576i_50HZ && 
			byOutModeType != HDU_YPbPr_480i_60HZ)
		{
			tChnCfg.SetOutModeType(HDU_YPbPr_1080i_50HZ);
			tChnCfg.SetZoomRate(HDU_ZOOMRATE_16_9);
			tChnCfg.SetScalingMode(HDU2_SHOWMODE_NONGEOMETRIC);
			Hdu2Print(HDU_LVL_WARNING,  "[CheckChnCfgParam]HDU2 Not support(%d)(%d)!!",tChnCfg.GetOutPortType(),byOutModeType);
			return FALSE;
		}
		if (byOutPortType == HDU_OUTPUT_VGA &&
			byOutModeType != HDU_VGA_SXGA_60HZ && byOutModeType != HDU_VGA_XGA_60HZ  &&
			byOutModeType != HDU_VGA_XGA_75HZ  && byOutModeType != HDU_VGA_SVGA_60HZ &&
			byOutModeType != HDU_VGA_SVGA_75HZ && byOutModeType != HDU_VGA_VGA_60HZ  &&
			byOutModeType != HDU_VGA_VGA_75HZ  && byOutModeType != HDU_VGA_SXGA_75HZ &&
			byOutModeType != HDU_VGA_WXGA_1280_768_60HZ  &&
			byOutModeType != HDU_VGA_WXGA_1280_800_60HZ  && 
			byOutModeType != HDU_VGA_WSXGA_60HZ          &&
			byOutModeType != HDU_VGA_SXGAPlus_60HZ       && 
			byOutModeType != HDU_VGA_UXGA_60HZ           &&
			byOutModeType != HDU_WXGA_1280_800_75HZ)
		{
			tChnCfg.SetOutModeType(HDU_VGA_XGA_60HZ);
			tChnCfg.SetZoomRate(HDU_ZOOMRATE_4_3);
			tChnCfg.SetScalingMode(HDU2_SHOWMODE_NONGEOMETRIC);
			Hdu2Print(HDU_LVL_WARNING,  "[CheckChnCfgParam]HDU2 Not support(%d)(%d)!!",tChnCfg.GetOutPortType(),byOutModeType);
			return FALSE;
		}
		//支持HDU2-S
		if (byOutPortType == HDU_OUTPUT_C && 
			tChnCfg.GetOutModeType() != HDU_C_576i_50HZ && 
			tChnCfg.GetOutModeType() != HDU_C_480i_60HZ)
		{
			tChnCfg.SetOutModeType(HDU_C_576i_50HZ);
			tChnCfg.SetZoomRate(HDU_ZOOMRATE_4_3);
			tChnCfg.SetScalingMode(HDU2_SHOWMODE_NONGEOMETRIC);
			Hdu2Print(HDU_LVL_WARNING,  "[CheckChnCfgParam]HDU2-S Not support(%d)(%d)!!",tChnCfg.GetOutPortType(),byOutModeType);
			return FALSE;
		}
	}
	return TRUE;
}
/* --------------------CHdu2ChnMgrGrp类的实现  start-----------------------*/
/*====================================================================
函数  : Create
功能  : 创建KdvMediaRcv,KdvVidDec,KdvAudDec
输入  : u16 wChnId
输出  : 无
返回  : BOOL
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇       创建
2013/03/11   4.7.2       陈兵		  修改(HDU支持多画面)
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::Create(u16 wChnId,BOOL32 bIsHdu2_s)
{
	u16 wRet = 0;
	TVidDecHD3RD tVidDecInit = {0};
	tVidDecInit.dwChnID = wChnId;
	// [2013/03/11 chenbing] 
	tVidDecInit.dwReserved = HDU2_VMP;
	tVidDecInit.dwPlyStd = VID_OUTTYPE_1080P60;
	if (wChnId == 0)
	{
		tVidDecInit.dwPlyPort = VIDIO_HDMI0;
	}
	else if (wChnId == 1) 
	{
		tVidDecInit.dwPlyPort = VIDIO_HDMI1;
	}
	else
	{
		Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::Create]Unknown  PlayPortType!\n");
		return FALSE;
	}

	// [2013/03/11 chenbing] 创建一个视频解码器 
	wRet = m_cVidDecoder.CreateHD3(&tVidDecInit);
	if((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::Create]VDecoder:CreateDecoder Failed!Param(ChnId:%d),error:%d.\n",wChnId,wRet);
		return FALSE;
	}
	wRet = m_cVidDecoder.SetVidPlayPolicy(5, (u32)EN_PLAY_BMP);
	if((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::Create]VDecoder:SetVidPlayPolicy Failed!So Return!!\n");
		return FALSE;
	}
	TDecoder tDecoder;
	memset(&tDecoder, 0, sizeof(TDecoder));
	tDecoder.dwChnID  = wChnId;

	// [2013/03/11 chenbing] 创建一个音频解码器 
	if( bIsHdu2_s )
	{
		wRet = m_cAudDecoder.CreateDecoder(&tDecoder, (u32)en_HDU2_Board_S);
	}
	else
	{
		wRet = m_cAudDecoder.CreateDecoder(&tDecoder, (u32)en_HDU2_Board);
	}
	if((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::Create]ADecoder:CreateDecoder Failed!Param(ChnId:%d),error:%d.\n",wChnId,wRet);
		return FALSE;
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::Create]ADecoder:CreateDecoder Successed!\n");
	}

	// [2013/03/11 chenbing]  
	for (u8 byIndex=0; byIndex<HDU_MODEFOUR_MAX_SUBCHNNUM; byIndex++)
	{
		m_acRcvCB[byIndex].m_byHduSubChnId	 = byIndex;
		m_acRcvCB[byIndex].m_pcHdu2ChnMgrGrp = this;
		// 创建四个视频接收对象 
		wRet = m_acVidMediaRcv[byIndex].Create(MAX_VIDEO_FRAME_SIZE, CBVidRecvFrame, (u32)&m_acRcvCB[byIndex]);
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::Create]VidMediaRcv:Create Failed!error:%d.\n", wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::Create]VidMediaRcv[%d]:Create Successed!\n", byIndex);
		}

		m_acVidMediaRcv[byIndex].SetHDFlag(TRUE);
	} //for u8 byIndex=0 over	
	
	// 创建一个音频接收对象 
 	wRet = m_cAudMediaRcv.Create(MAX_AUDIO_FRAME_SIZE, CBAudRecvFrame, (u32)this);
	if(MEDIANET_NO_ERROR != wRet)
	{
		Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::Create]AudMediaRcv:Create Failed!error:%d.\n", wRet);
		return FALSE;
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::Create]AudMediaRcv:Create Successed!\n");
	}

	return TRUE;
}
/*====================================================================
函数  : StartDecode
功能  : 开启音视频解码器
输入  : u8 byMode        //模式
输出  : 无
返回  : BOOL32
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::StartDecode(u8 byMode/* = MODE_BOTH */)
{
	if (MODE_NONE == byMode)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::StartDecode]byMode == MODE_NONE\n");
		return FALSE;
	}

	u8 byOldMode = MODE_NONE;
	for (u8 bySubChnId = 0; bySubChnId < HDU_MODEFOUR_MAX_SUBCHNNUM; bySubChnId++)
	{
		byOldMode = GetMode(bySubChnId);
		if (MODE_NONE != byOldMode)
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StartDecode] SubChnId(%d) OldMode(%d)\n", bySubChnId, byOldMode);
			break;
		}
	}

	u16 wRet = (u16)Codec_Success;
	Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StartDecode] OldMode(%d)\n", GetMode());
//	if ((MODE_NONE == GetMode() || MODE_VIDEO == GetMode() ) && (MODE_AUDIO == byMode || MODE_BOTH == byMode))
	if( MODE_AUDIO == byMode || MODE_BOTH == byMode )
	{
		// 开启音频前需要先停止音频解码，防止无码流时解码错误 
		wRet = m_cAudDecoder.StopDec();
		if((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::StopDecode]m_cAudDecoder.StaopDec() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StopDecode]m_cAudDecoder.StopDec() Successed!\n");
		}

		if (m_tAudDp.GetRealPayLoad() == MEDIA_TYPE_AACLC || m_tAudDp.GetRealPayLoad() == MEDIA_TYPE_AACLD)
		{
			TAudioDecParam tAudParam = {0};
			tAudParam.m_dwSamRate = (u32)SAMPLE_RATE32000;
			tAudParam.m_dwMediaType = m_tAudDp.GetRealPayLoad();
			tAudParam.m_dwChannelNum = GetAudChnNum();
			m_cAudDecoder.SetAudioDecParam( tAudParam );
			if((u16)Codec_Success != wRet)
			{
				Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::StartDecode]m_cAudDecoder.SetAudioDecParam() Failed,Error:%d\n",wRet);
			}
			else
			{
				Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StartDecode]m_cAudDecoder.SetAudioDecParam(MediaType: %d AudChnNum: %d) Successed!\n",
					tAudParam.m_dwMediaType, tAudParam.m_dwChannelNum);
			}
		}

		wRet = m_cAudDecoder.StartDec();
		if((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::StartDecode]m_cAudDecoder.StartDec() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StartDecode]m_cAudDecoder.StartDec() Successed!\n");
		}
	}

	if ((MODE_NONE == byOldMode || MODE_AUDIO == byOldMode ) && (MODE_VIDEO == byMode || MODE_BOTH == byMode))
	{
		wRet = m_cVidDecoder.StartDec();
		if((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::StartDecode]m_cVidDecoder.StartDec() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StartDecode]m_cVidDecoder.StartDec() Successed!\n");
		}
	}

	return TRUE;
}
/*====================================================================
函数  : StopDecode
功能  : 停止音视频解码器
输入  : u8 byMode        //模式
输出  : 无
返回  : BOOL32
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::StopDecode(u8 byMode/* = MODE_BOTH */ )
{
	if (byMode == MODE_NONE)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::StopDecode]buCurMode == MODE_NONE\n");
		return FALSE;
	}

	u16 wRet = (u16)Codec_Success;
	if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
	{
		wRet = m_cAudDecoder.StopDec();
		if((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::StopDecode]m_cAudDecoder.StopDec() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StopDecode]m_cAudDecoder.StopDec() Successed!\n");
		}
	}
	if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
	{
		wRet = m_cVidDecoder.StopDec();
		if((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::StopDecode]m_cVidDecoder.StopDec() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StopDecode]m_cVidDecoder.StopDec() Successed!\n");
		}
	}

	return TRUE;
}
/*====================================================================
函数  : StartNetRecv
功能  : 停止音视频接收
输入  : u8 byMode        //模式
输出  : 无
返回  : BOOL32
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
2013/03/11   4.7.2       陈兵		   修改(HDU支持多画面)
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::StartNetRecv(u8 byMode/* = MODE_BOTH */, u8 byHduSubChnId)
{
	u16 wRet = MEDIANET_NO_ERROR;
	u8 byOldMode = GetMode(byHduSubChnId);
	if ((MODE_NONE == byOldMode || MODE_VIDEO == byOldMode) && (MODE_AUDIO == byMode || MODE_BOTH == byMode) )
	{
		wRet = m_cAudMediaRcv.StartRcv();
		if((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::StartNetRecv]m_cAudMediaRcv.StartRcv() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StartNetRecv]m_cAudMediaRcv.StartRcv() Successed!\n");
		}
	}

	if ((MODE_NONE == byOldMode || MODE_AUDIO == byOldMode) && (MODE_VIDEO == byMode || MODE_BOTH == byMode) )
	{
		wRet = m_acVidMediaRcv[byHduSubChnId].StartRcv();
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::StartNetRecv]m_acVidMediaRcv.StartRcv() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StartNetRecv]m_acVidMediaRcv.StartRcv() Successed!\n");
		}
	}
	return TRUE;
}
/*====================================================================
函数  : StopDecode
功能  : 停止音视频接收
输入  : u8 byMode        //模式
输出  : 无
返回  : BOOL
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
2013/03/11   4.7.2       陈兵		   修改(HDU支持多画面)
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::StopNetRecv(u8 byMode/* = MODE_BOTH */, u8 byHduSubChnId)
{
	u16 wRet = MEDIANET_NO_ERROR;

	if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
	{
		wRet = m_cAudMediaRcv.StopRcv();
		if((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::StopNetRecv]m_cAudMediaRcv.StopRcv() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StopNetRecv]m_cAudMediaRcv.StopRcv() Successed!\n");
		}
	}

	if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
	{
		wRet = m_acVidMediaRcv[byHduSubChnId].StopRcv();
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::StopNetRecv]m_acVidMediaRcv.StopRcv() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::StopNetRecv]m_acVidMediaRcv.StopRcv() Successed!\n");
		}
	}
	return TRUE;
}
/*====================================================================
函数  : SetVidLocalNetParam
功能  : 保存视频接收参数信息，并设置给相应视频对象，输入为空，则清空
输入  : TLocalNetParam *ptVidLocalNetParam  接收网络参数
输出  : 无
返回  : BOOL32
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
2013/03/11   4.7.2       陈兵		   修改(HDU支持多画面)
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetVidLocalNetParam(TLocalNetParam *ptVidLocalNetParam, u8 byHduSubChnId)
{
	u16 wRet = MEDIANET_NO_ERROR;
	if (ptVidLocalNetParam == NULL)
	{
		memset( &m_atVidLocalNetParam[byHduSubChnId], 0, sizeof(TLocalNetParam) );
		wRet = m_acVidMediaRcv[byHduSubChnId].RemoveNetRcvLocalParam();
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetVidLocalNetParam]m_acVidMediaRcv.RemoveNetRcvLocalParam() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_KEYSTATUS,  "[CHdu2ChnMgrGrp::SetVidLocalNetParam]m_acVidMediaRcv.RemoveNetRcvLocalParam() Successed!\n");
		}
	}
	else
	{
		memcpy( &m_atVidLocalNetParam[byHduSubChnId], ptVidLocalNetParam, sizeof(TLocalNetParam) );
		wRet = m_acVidMediaRcv[byHduSubChnId].SetNetRcvLocalParam(m_atVidLocalNetParam[byHduSubChnId]);
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::SetVidLocalNetParam]m_acVidMediaRcv[byHduSubChnId].SetNetRcvLocalParam() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_KEYSTATUS,  "[CHdu2ChnMgrGrp::SetVidLocalNetParam]m_acVidMediaRcv[byHduSubChnId].SetNetRcvLocalParam() Successed!\n");
		}
	}
	return TRUE;
}
/*====================================================================
函数  : SetAudLocalNetParam
功能  : 保存音频接收参数信息，并设置给相应音频对象，输入为空，则清空
输入  : TLocalNetParam *ptVidLocalNetParam  接收网络参数
输出  : 无
返回  : BOOL32
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetAudLocalNetParam(TLocalNetParam *ptAudLocalNetParam)
{
	u16 wRet = MEDIANET_NO_ERROR;	
	if (ptAudLocalNetParam == NULL)
	{
		memset( &m_tAudLocalNetParam, 0, sizeof(TLocalNetParam) );
		wRet = m_cAudMediaRcv.RemoveNetRcvLocalParam();
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetAudLocalNetParam]m_cAudMediaRcv.RemoveNetRcvLocalParam() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_KEYSTATUS,  "[CHdu2ChnMgrGrp::SetAudLocalNetParam]m_cAudMediaRcv.RemoveNetRcvLocalParam() Successed!\n");
		}
	}
	else
	{
		memcpy( &m_tAudLocalNetParam, ptAudLocalNetParam, sizeof(TLocalNetParam) );
		wRet = m_cAudMediaRcv.SetNetRcvLocalParam(m_tAudLocalNetParam);
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetAudLocalNetParam]m_cAudMediaRcv.SetNetRcvLocalParam() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_KEYSTATUS,  "[CHdu2ChnMgrGrp::SetAudLocalNetParam]m_cAudMediaRcv.SetNetRcvLocalParam() Successed!\n");
		}
	}
	return TRUE;
}
/*====================================================================
函数  : SetNetRecvRsParam
功能  : 设置丢包重传参数
输入  : u8 byMode  模式
		BOOL32 bRepeatSnd 是否开启丢包重传功能
输出  : 无
返回  : BOOL32
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
2013/03/11   4.7.2       陈兵		   修改(HDU支持多画面)
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetNetRecvRsParam(u8 byMode, BOOL32 bRepeatSnd, u8 byHduSubChnId)
{
	u16 wRet = MEDIANET_NO_ERROR;
	TRSParam tRsParam;
	memset(&tRsParam, 0x0, sizeof(tRsParam));
	tRsParam.m_wFirstTimeSpan  = htons(g_cHdu2App.GetPrsTimeSpan().m_wFirstTimeSpan); 
	tRsParam.m_wSecondTimeSpan = htons(g_cHdu2App.GetPrsTimeSpan().m_wSecondTimeSpan); 
	tRsParam.m_wThirdTimeSpan  = htons(g_cHdu2App.GetPrsTimeSpan().m_wThirdTimeSpan); 
	tRsParam.m_wRejectTimeSpan = htons(g_cHdu2App.GetPrsTimeSpan().m_wRejectTimeSpan); 
	if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
	{
		wRet = m_cAudMediaRcv.ResetRSFlag(tRsParam, bRepeatSnd);
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetNetRecvRsParam]m_cAudMediaRcv.ResetRSFlag() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetNetRecvRsParam]m_cAudMediaRcv.ResetRSFlag() Successed!\n");
		}
	}
	if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
	{
		wRet = m_acVidMediaRcv[byHduSubChnId].ResetRSFlag(tRsParam, bRepeatSnd);
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetNetRecvRsParam]m_acVidMediaRcv.ResetRSFlag() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetNetRecvRsParam]m_acVidMediaRcv.ResetRSFlag() Successed!\n");
		}
	}
	return TRUE;
}
/*====================================================================
函数  : SetActivePT
功能  : 设置动态载荷
输入  : u8 byMode  模式
        u8 byHduSubChnId:子通道
输出  : 无
返回  : BOOL32
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
2013/03/11   4.7.2       陈兵		   修改(HDU支持多画面)
====================================================================*/
BOOL32  CHdu2ChnMgrGrp::SetActivePT(u8 byMode,BOOL32 bIsVidAutoAjust /*= TRUE */, u8 byHduSubChnId)
{
	u16 wRet = MEDIANET_NO_ERROR;
	if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
	{
		wRet = m_cAudMediaRcv.SetActivePT(m_tAudDp.GetActivePayload(), m_tAudDp.GetRealPayLoad());
		if((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetActivePT]m_cAudMediaRcv.SetActivePT() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetActivePT]m_cAudMediaRcv.SetActivePT() Successed!\n");
		}
	}
	if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
	{
		if (bIsVidAutoAjust)
		{
			//设置动态载荷参数为自适应，当视频格式发生变化后可自行适应解码
			wRet = m_acVidMediaRcv[byHduSubChnId].SetActivePT(MEDIA_TYPE_H264, MEDIA_TYPE_H264);
		}
		else
		{
			wRet = m_acVidMediaRcv[byHduSubChnId].SetActivePT(m_atVidDp[byHduSubChnId].GetActivePayload(), m_atVidDp[byHduSubChnId].GetRealPayLoad());
		}
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetActivePT]m_acVidMediaRcv.SetActivePT() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetActivePT]m_acVidMediaRcv.SetActivePT() Successed!\n");
		}
	}
	return TRUE;
}
/*====================================================================
函数  : SetDecryptKey
功能  : 设置接收解密参数
输入  : u8 byMode  模式
		u8 byHduSubChnId:子通道
输出  : 无
返回  : BOOL32
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
2013/03/11   4.7.2       陈兵		   修改(HDU支持多画面)
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetDecryptKey(u8 byMode, u8 byHduSubChnId)
{
	u16 wRet = 0;
	u8  byDecryptMode = m_tMediaDec.GetEncryptMode();
	u8  abyKeyBuf[MAXLEN_KEY];
	s32 nKeySize = 0;

	if (byDecryptMode == CONF_ENCRYPTMODE_NONE)
	{
		if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
		{
			wRet = m_cAudMediaRcv.SetDecryptKey((s8*)NULL, (u16)0, byDecryptMode);
			if(MEDIANET_NO_ERROR != wRet)
			{
				Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetDecryptKey]m_cAudMediaRcv.SetDecryptKey() Failed,Error:%d\n",wRet);
				return FALSE;
			}
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetDecryptKey]m_cAudMediaRcv.SetDecryptKey() Successed!\n");
		}
		if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
		{
			wRet = m_acVidMediaRcv[byHduSubChnId].SetDecryptKey((s8*)NULL, (u16)0, byDecryptMode);
			if(MEDIANET_NO_ERROR != wRet)
			{
				Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetDecryptKey]m_acVidMediaRcv.SetDecryptKey() Failed,Error:%d\n",wRet);
				return FALSE;
			}
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetDecryptKey]m_acVidMediaRcv.SetDecryptKey() Successed!\n");
		}
		return TRUE;
	}
	//上层与下层的编解码模式不一致
	if (byDecryptMode == CONF_ENCRYPTMODE_DES)
	{
		byDecryptMode = DES_ENCRYPT_MODE;
	}
	else if (byDecryptMode == CONF_ENCRYPTMODE_AES)
	{
		byDecryptMode = AES_ENCRYPT_MODE;
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetDecryptKey]Unknown DecryptMode(%d),So return!\n",byDecryptMode);
		return FALSE;
	}
	m_tMediaDec.GetEncryptKey(abyKeyBuf,&nKeySize);
	if (byMode == MODE_AUDIO || byMode == MODE_BOTH)
	{
		wRet = m_cAudMediaRcv.SetDecryptKey((s8*)abyKeyBuf, nKeySize, byDecryptMode);
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetDecryptKey]m_cAudMediaRcv.SetDecryptKey() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetDecryptKey]m_cAudMediaRcv.SetDecryptKey() Successed!\n");
		}
	}
	if (byMode == MODE_VIDEO || byMode == MODE_BOTH)
	{
		wRet = m_acVidMediaRcv[byHduSubChnId].SetDecryptKey((s8*)abyKeyBuf, nKeySize, byDecryptMode);
		if(MEDIANET_NO_ERROR != wRet)
		{
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetDecryptKey]m_acVidMediaRcv.SetDecryptKey() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetDecryptKey]m_acVidMediaRcv.SetDecryptKey() Successed!\n");
		}
	}
	return TRUE;
}
/*====================================================================
函数  : SetVidPlyPortType
功能  : 设置视频播放端口类型
输入  : u32 dwType   播放端口类型
输出  : 无
返回  : BOOL32
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetVidPlyPortType(u32 dwType)
{
    u16 wRet = m_cVidDecoder.SetVidPlyPortType(dwType);
	if ((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetVidPlyPortType]m_cVidDecoder.SetVidPlyPortType() Failed,Error:%d\n",wRet);
		return FALSE;
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetVidPlyPortType]m_cVidDecoder.SetVidPlyPortType() Successed!\n");
	}
	return TRUE;
}
/*====================================================================
函数  : SetVideoPlyInfo
功能  : 设置视频源信息
输入  : TVidSrcInfo* ptInfo 视频源信息
输出  : 无
返回  : BOOL32
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetVideoPlyInfo(TVidSrcInfo* ptInfo)
{
    u16 wRet = m_cVidDecoder.SetVideoPlyInfo(ptInfo);
	if ((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetVideoPlyInfo]m_cVidDecoder.SetVideoPlyInfo() Failed,Error:%d\n",wRet);
		return FALSE;
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetVideoPlyInfo]m_cVidDecoder.SetVideoPlyInfo() Successed!\n");
	}
	return TRUE;
}

/*====================================================================
函数  : SetVidPIPParam
功能  : HDU通道模式切换
输入  : 
输出  : 无
返回  : BOOL32
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2013/03/11	 4.7.2		 陈兵		  创建(HDU多画面支持)
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetVidPIPParam(const u8 byHduVmpMode)
{
	enVideoPIPIndex enIndex[3];
	memset(enIndex, 0, sizeof(enIndex));

	if ( byHduVmpMode == HDUCHN_MODE_FOUR )
	{
		u16 wRet = 0;
		m_cVidDecoder.SetVidPIPParam(VIDEO_PIPMODE_FOUR, enIndex);
		if ((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetVidPIPParam]m_cVidDecoder.SetVidPIPParam(%d) Failed, Error:%d\n", byHduVmpMode, wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetVidPIPParam]m_cVidDecoder.SetVidPIPParam(%d) Successed!\n", byHduVmpMode);
		}
	}
	else if (byHduVmpMode == HDUCHN_MODE_ONE)
	{
		u16 wRet = 0;
		m_cVidDecoder.SetVidPIPParam(VIDEO_PIPMODE_ONE, enIndex);
		if ((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetVidPIPParam]m_cVidDecoder.SetVidPIPParam(%d) Failed, Error:%d\n", byHduVmpMode, wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetVidPIPParam]m_cVidDecoder.SetVidPIPParam(%d) Successed!\n", byHduVmpMode);
		}
	}
	else
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetVidPIPParam]m_cVidDecoder.SetVidPIPParam(%d) byHduVmpMode Failed, Error\n", byHduVmpMode);
		return FALSE;
	}

	return TRUE;
}

/*====================================================================
函数  : SetPlayScales
功能  : 设置视频缩放比例
输入  : u16 wWidth  宽
		u16 wHeigh  高
输出  : 无
返回  : BOOL32
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetPlayScales( u16 wWidth, u16 wHeigh )
{
    u16 wRet = m_cVidDecoder.SetPlayScale(wWidth, wHeigh);
	if ((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetPlayScales]m_cVidDecoder.SetPlayScales() Failed,Error:%d\n",wRet);
		return FALSE;
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetPlayScales]m_cVidDecoder.SetPlayScales() Successed!\n");
	}
	return TRUE;
}
/*====================================================================
函数  : SetVidDecResizeMode
功能  : 设置显示策略
输入  : u16 nMode 显示策略
输出  : 无
返回  : BOOL32
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetVidDecResizeMode(u16 nMode)
{
	u8 byDecReszeMode = 0;
	switch(nMode) 
	{
		case HDU2_SHOWMODE_NONGEOMETRIC:
			byDecReszeMode = (u8)EN_ZOOM_SCALE;
			break;
		case HDU2_SHOWMODE_CUTEDGEGEOME:
			byDecReszeMode = (u8)EN_ZOOM_CUT;
			break;
		case HDU2_SHOWMODE_BLACKEDGEGEOME:
			byDecReszeMode = (u8)EN_ZOOM_FILLBLACK;
			break;
		default:
			return FALSE;
	}
    u16 wRet = m_cVidDecoder.SetVidDecResizeMode(byDecReszeMode);
	if ((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetVidDecResizeMode]m_cVidDecoder.SetVidDecResizeMode(%d) Failed,Error:%d\n",byDecReszeMode,wRet);
		return FALSE;
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetVidDecResizeMode]m_cVidDecoder.SetVidDecResizeMode(%d) Successed!\n",byDecReszeMode);
	}
	return TRUE;
}
/*====================================================================
函数  : GetVidDecoderStatus
功能  : 获取视频解码器状态
输入  : u8 byMode
		TKdvDecStatus &tKdvDecStatus
输出  : 视频解码器状态
返回  : BOOL32
注    :
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
20130529	4.7         陈兵		  屏蔽
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::GetDecoderStatus(u8 byMode ,TKdvDecStatus &tKdvDecStatus, u8 byHduSubChnId)
{
// 	if (byMode != MODE_AUDIO && byMode != MODE_VIDEO)
// 	{
// 		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::GetDecoderStatus]Mode(%d) Is Invalide\n",byMode);
// 		return FALSE;
// 	}
// 	u16 wRet = 0;
// 	if (byMode == MODE_AUDIO)
// 	{
// 		wRet = m_cAudDecoder.GetDecoderStatus(tKdvDecStatus);
// 		if ((u16)Codec_Success != wRet)
// 		{
// 			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::GetDecoderStatus]m_cAudDecoder.GetDecoderStatus() Failed,Error:%d\n",wRet);
// 			return FALSE;
// 		}
// 		else
// 		{
// 			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::GetDecoderStatus]m_cAudDecoder.GetDecoderStatus() Successed!\n");
// 		}
// 	}
// 	if (byMode == MODE_VIDEO)
// 	{
// 		wRet = m_cVidDecoder.GetDecoderStatus(tKdvDecStatus, byHduSubChnId);
// 		if ((u16)Codec_Success != wRet)
// 		{
// 			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::GetDecoderStatus]m_cVidDecoder.GetDecoderStatus() Failed,Error:%d\n",wRet);
// 			return FALSE;
// 		}
// 		else
// 		{
// 			Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::GetDecoderStatis]m_cVidDecoder.GetDecoderStatus() Successed!\n");
// 		}
// 	}
	return TRUE;
}
/*====================================================================
函数  : GetVidDecoderStatis
功能  : 获取视频解码器的统计信息
输入  : u8 byMode
		TKdvDecStatis &tKdvDecStatis
输出  : 视频解码器的统计信息
返回  : BOOL32
注    :
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::GetDecoderStatis(u8 byMode ,TKdvDecStatis &tKdvDecStatis, u8 byHduSubChnId)
{
	if (byMode != MODE_AUDIO && byMode != MODE_VIDEO)
	{
		Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::GetDecoderStatis]Mode(%d) Is Invalide\n",byMode);
		return FALSE;
	}
	u16 wRet = 0;
	if (byMode == MODE_AUDIO)
	{
		wRet = m_cAudDecoder.GetDecoderStatis(tKdvDecStatis);
		if ((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::GetDecoderStatis]m_cAudDecoder.GetDecoderStatis() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_GENERAL,  "[CHdu2ChnMgrGrp::GetDecoderStatis]m_cAudDecoder.GetDecoderStatis() Successed!\n");
		}
	}
	if (byMode == MODE_VIDEO)
	{
		wRet = m_cVidDecoder.GetDecoderStatis(tKdvDecStatis, byHduSubChnId);
		if ((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::GetDecoderStatis]m_cVidDecoder.GetDecoderStatis() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
		  Hdu2Print(HDU_LVL_GENERAL,  "[CHdu2ChnMgrGrp::GetDecoderStatis]m_cVidDecoder.GetDecoderStatis() Successed!\n");
		}
	}
	return TRUE;
}
/*====================================================================
函数  : SetData
功能  : 为解码器设置数据
输入  : u8 byMode
		TFrameHeader tFrameHdr
输出  : 
返回  : BOOL32
注    :
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetData(u8 byMode, TFrameHeader tFrameHdr, u8 byHduSubChnId)
{
	if (byMode != MODE_AUDIO && byMode != MODE_VIDEO)
	{
		Hdu2Print(HDU_LVL_GENERAL,  "[CHdu2ChnMgrGrp::SetData]Mode(%d) Is Invalide\n",byMode);
		return FALSE;
	}
	u16 wRet = 0;
	if (byMode == MODE_AUDIO)
	{
		wRet = m_cAudDecoder.SetData(tFrameHdr);
		if ((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::SetData]m_cAudDecoder.SetData() Failed,Error:%d\n",wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_GENERAL,  "[CHdu2ChnMgrGrp::SetData]m_cAudDecoder.SetData() Successed!\n");
		}
	}
	if (byMode == MODE_VIDEO)
	{
		wRet = m_cVidDecoder.SetData(tFrameHdr, byHduSubChnId);
		if ((u16)Codec_Success != wRet)
		{
			Hdu2Print(HDU_LVL_ERROR,  "[CHdu2ChnMgrGrp::SetData]m_cVidDecoder.SetData[SubChnId](%d) Failed,Error:%d\n",byHduSubChnId, wRet);
			return FALSE;
		}
		else
		{
			Hdu2Print(HDU_LVL_GENERAL,  "[CHdu2ChnMgrGrp::SetData]m_cVidDecoder.SetData[SubChnId](%d) Successed!\n", byHduSubChnId);
		}
	}
	return TRUE;
}
/*====================================================================
函数  : SetAudOutPort
功能  : 设置音频播放端口类型 
输入  : u32 dwType
输出  : 无
返回  : BOOL32
注    :
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetAudOutPort(u32 dwAudPort)
{
    u16 wRet = (u16)Codec_Success;
	wRet = m_cAudDecoder.SetAudOutPort(dwAudPort);
	if ((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetAudOutPort]m_cAudDecoder.SetAudOutPort() Failed,Error:%d\n",wRet);
		return FALSE;
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetAudOutPort]m_cAudDecoder.SetAudOutPort() Successed!\n");
	}
	return TRUE;
}
/*====================================================================
函数  : SetVolume
功能  : 设置输出声音音量
输入  : u8 byVolume
输出  : 无
返回  : BOOL32
注    :
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
====================================================================*/
BOOL32  CHdu2ChnMgrGrp::SetVolume(u8 byVolume )
{
    u16 wRet = (u16)Codec_Success;
	wRet = m_cAudDecoder.SetAudioVolume(byVolume);
	if ((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetVolume]m_cAudDecoder.SetAudioVolume() Failed,Error:%d\n",wRet);
		return FALSE;
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetVolume]m_cAudDecoder.SetAudioVolume() Successed!\n");
	}
	m_byVolume = byVolume;
	return TRUE;
} 
/*====================================================================
函数  : GetVolume
功能  : 得到输出声音音量
输入  : u8 byVolume
输出  : 无
返回  : u8
注    :
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
====================================================================*/
u8  CHdu2ChnMgrGrp::GetVolume()
{
	u8 byVolume = 0;
    u16 wRet = m_cAudDecoder.GetAudioVolume(byVolume);
	if ((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::GetVolume]m_cAudDecoder.GetAudioVolume() Failed,Error:%d\n",wRet);
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::GetVolume]m_cAudDecoder.GetAudioVolume() Successed!\n");
	}
	if (byVolume != m_byVolume)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::GetVolume]m_cAudDecoder.GetAudioVolume() m_byVolume(%d) byVolume(%d)\n",m_byVolume,byVolume);
	}
	return m_byVolume;
}
/*====================================================================
函数  : SetIsMute
功能  : 设置静音
输入  : BOOL32 bMute
输出  : 无
返回  : u16
注    :
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
====================================================================*/
BOOL32  CHdu2ChnMgrGrp::SetIsMute(BOOL32 bMute)
{
    u16 wRet = (u16)Codec_Success;
	wRet = m_cAudDecoder.SetAudioMute(bMute);
	if ((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetIsMute]m_cAudDecoder.SetAudioMute() Failed,Error:%d\n",wRet);
		return FALSE;
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetIsMute]m_cAudDecoder.SetAudioMute() Successed!\n");
	}
	m_bIsMute = bMute;
	return TRUE;
}
/*====================================================================
函数  : GetIsMute
功能  : 获取通道是否静音
输入  : 
输出  : 无
返回  : u16
注    :
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
====================================================================*/
BOOL32  CHdu2ChnMgrGrp::GetIsMute()
{
	return m_bIsMute;
}
/*====================================================================
函数  : SetVidPlayPolicy
功能  : 设置空闲通道背景显示策略
输入  : u8 byMode
		TKdvDecStatis &tKdvDecStatis
输出  : 
返回  : BOOL32
注    :
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
====================================================================*/
BOOL32 CHdu2ChnMgrGrp::SetVidPlayPolicy(u8 byShowMode)
{
	enplayMode eIdleChnShowMode = EN_PLAY_BMP;
	switch(byShowMode) 
	{
		case HDU_SHOW_BLACK_MODE:
			eIdleChnShowMode = EN_PLAY_BLACK;
			break;
		case HDU_SHOW_LASTFRAME_MODE:
			eIdleChnShowMode = EN_PLAY_LAST;
			break;
		case HDU_SHOW_DEFPIC_MODE:
			eIdleChnShowMode = EN_PLAY_BMP;
			break;
		case HDU_SHOW_USERDEFPIC_MODE:
			eIdleChnShowMode = EN_PLAY_BMP_USR;
			break;
		default:
			eIdleChnShowMode = EN_PLAY_ERROR;
			Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetVidPlayPolicy]Unknown IdleChnShowMode(%d)\n",byShowMode);
			break;
	}
	m_tIdleChnBackGround = byShowMode;
	u16 wRet = m_cVidDecoder.SetVidPlayPolicy(5,u32(eIdleChnShowMode));
	if ((u16)Codec_Success != wRet)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[CHdu2ChnMgrGrp::SetVidPlayPolicy]m_cVidDecoder.SetVidPlayPolicy() Failed,Error:%d\n",wRet);
		return FALSE;
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[CHdu2ChnMgrGrp::SetVidPlayPolicy]m_cVidDecoder.SetVidPlayPolicy() Successed!\n");
	}
	return TRUE;
}
/* ==================================================================
函数  : GetVidSrcInfoByTypeAndMode
功能  : 根据HDU输出接口和输出制式得到视频源信息
输入  : 无
输出  : 无
返回  : void
注    :
---------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
===================================================================== */
BOOL32 CHdu2ChnMgrGrp::GetVidSrcInfoByTypeAndMode(u8 byOutPortType, u8 byOutModeType, TVidSrcInfo *ptVidSrcInfo,BOOL32 bIsHdu2 = TRUE)
{
    if (NULL == ptVidSrcInfo)
    {
		Hdu2Print(HDU_LVL_WARNING,  "[GetVidSrcInfoByTypeAndMode]NULL == ptVidSrcInfo\n");
        return FALSE;
	}
	switch (byOutPortType)
	{
		case HDU_OUTPUT_DVI:
        case HDU_OUTPUT_HDMI:
		case HDU_OUTPUT_YPbPr:
		case HDU_OUTPUT_SDI:
			switch ( byOutModeType )
			{
			case HDU_YPbPr_1080P_24fps:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 24;
				break;

			case HDU_YPbPr_1080P_25fps:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 25;
				break;

			case HDU_YPbPr_1080P_29970HZ:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 29970;
				break;

			case HDU_YPbPr_1080P_30fps:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 30;
				break;
			
			case HDU_YPbPr_1080P_50fps:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 50;
				break;

			case HDU_YPbPr_1080P_59940HZ:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 59940;
				break;

			case HDU_YPbPr_1080P_60fps:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;

			case HDU_YPbPr_1080i_50HZ:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 0;
				ptVidSrcInfo->m_dwFrameRate = 25;
				break;

			case HDU_YPbPr_1080i_60HZ:
				ptVidSrcInfo->m_wWidth = 1920;
				ptVidSrcInfo->m_wHeight = 1080;
				ptVidSrcInfo->m_bProgressive = 0;
				ptVidSrcInfo->m_dwFrameRate = 30;
				break;

			case HDU_YPbPr_720P_50fps:
				ptVidSrcInfo->m_wWidth = 1280;
				ptVidSrcInfo->m_wHeight = 720;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 50;
				break;

			case HDU_YPbPr_720P_60fps:
				ptVidSrcInfo->m_wWidth = 1280;
				ptVidSrcInfo->m_wHeight = 720;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;
			
			case HDU_YPbPr_576i_50HZ:
				ptVidSrcInfo->m_wWidth = 720;
				ptVidSrcInfo->m_wHeight = 576;
				ptVidSrcInfo->m_bProgressive = 0;
				ptVidSrcInfo->m_dwFrameRate = 25;
				break;
			
			case HDU_YPbPr_480i_60HZ:    
				ptVidSrcInfo->m_wWidth = 720;
				ptVidSrcInfo->m_wHeight = 480;
				ptVidSrcInfo->m_bProgressive = 0;
				ptVidSrcInfo->m_dwFrameRate = 30;
				break;
			
			default:
				if (bIsHdu2)
				{
					ptVidSrcInfo->m_wWidth = 1920;
					ptVidSrcInfo->m_wHeight = 1080;
					ptVidSrcInfo->m_bProgressive = 1;
					ptVidSrcInfo->m_dwFrameRate = 60;
				}
				else
				{
					ptVidSrcInfo->m_wWidth = 1920;
					ptVidSrcInfo->m_wHeight = 1080;
					ptVidSrcInfo->m_bProgressive = 0;
					ptVidSrcInfo->m_dwFrameRate = 25;
				}
				break;
			}
			break;
		
		case HDU_OUTPUT_VGA:
			switch (byOutModeType)
			{
			case HDU_VGA_SXGA_60HZ:
				ptVidSrcInfo->m_wWidth = 1280;
				ptVidSrcInfo->m_wHeight = 1024;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;

			case HDU_VGA_SXGA_75HZ:
				ptVidSrcInfo->m_wWidth = 1280;
				ptVidSrcInfo->m_wHeight = 1024;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 75;
				break;

			case HDU_VGA_XGA_60HZ:
				ptVidSrcInfo->m_wWidth = 1024;
				ptVidSrcInfo->m_wHeight = 768;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;
		
			case HDU_VGA_XGA_75HZ:
				ptVidSrcInfo->m_wWidth = 1024;
				ptVidSrcInfo->m_wHeight = 768;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 75;
				break;
        
			case HDU_VGA_SVGA_60HZ:
				ptVidSrcInfo->m_wWidth = 800;
				ptVidSrcInfo->m_wHeight = 600;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;
			
			case HDU_VGA_SVGA_75HZ:
				ptVidSrcInfo->m_wWidth = 800;
				ptVidSrcInfo->m_wHeight = 600;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 75;
				break;
			
			case HDU_VGA_VGA_60HZ:
				ptVidSrcInfo->m_wWidth = 640;
				ptVidSrcInfo->m_wHeight = 480;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;
			
			case HDU_VGA_VGA_75HZ:
				ptVidSrcInfo->m_wWidth = 640;
				ptVidSrcInfo->m_wHeight = 480;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 75;
				break;

			case HDU_VGA_WXGA_1280_768_60HZ:
				ptVidSrcInfo->m_wWidth = 1280;
				ptVidSrcInfo->m_wHeight = 768;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;

			case HDU_VGA_WXGA_1280_800_60HZ:
				ptVidSrcInfo->m_wWidth = 1280;
				ptVidSrcInfo->m_wHeight = 800;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;

			case HDU_VGA_WSXGA_60HZ:
				ptVidSrcInfo->m_wWidth = 1440;
				ptVidSrcInfo->m_wHeight = 900;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;

			case HDU_VGA_SXGAPlus_60HZ:
				ptVidSrcInfo->m_wWidth = 1680;
				ptVidSrcInfo->m_wHeight = 1050;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;

			case HDU_VGA_UXGA_60HZ:
				ptVidSrcInfo->m_wWidth = 1600;
				ptVidSrcInfo->m_wHeight = 1200;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 60;
				break;

			case HDU_WXGA_1280_800_75HZ:
				ptVidSrcInfo->m_wWidth = 1280;
				ptVidSrcInfo->m_wHeight = 800;
				ptVidSrcInfo->m_bProgressive = 1;
				ptVidSrcInfo->m_dwFrameRate = 75;
				break;

			default:
				if (bIsHdu2)
				{
					ptVidSrcInfo->m_wWidth = 1280;
					ptVidSrcInfo->m_wHeight = 1024;
					ptVidSrcInfo->m_bProgressive = 1;
					ptVidSrcInfo->m_dwFrameRate = 60;
				}
				else
				{
					ptVidSrcInfo->m_wWidth = 1024;
					ptVidSrcInfo->m_wHeight = 768;
					ptVidSrcInfo->m_bProgressive = 1;
					ptVidSrcInfo->m_dwFrameRate = 60;
				}
				break;
			}
			break;
	case HDU_OUTPUT_C:
		switch (byOutModeType)
		{
		case HDU_C_576i_50HZ:
			ptVidSrcInfo->m_wWidth = 720;
			ptVidSrcInfo->m_wHeight = 576;
			ptVidSrcInfo->m_bProgressive = 0;
			ptVidSrcInfo->m_dwFrameRate = 25;
			break;
		case HDU_C_480i_60HZ:    
			ptVidSrcInfo->m_wWidth = 720;
			ptVidSrcInfo->m_wHeight = 480;
			ptVidSrcInfo->m_bProgressive = 0;
			ptVidSrcInfo->m_dwFrameRate = 30;
			break;
		default:
			ptVidSrcInfo->m_wWidth = 720;
			ptVidSrcInfo->m_wHeight = 576;
			ptVidSrcInfo->m_bProgressive = 0;
			ptVidSrcInfo->m_dwFrameRate = 25;
			break;
		}
	    break;
	default:
	    Hdu2Print(HDU_LVL_WARNING,  "[GetVidSrcInfoByTypeAndMode]byOutModeType is not exit!\n");
		return FALSE;
	}
    return TRUE;
}

/*====================================================================
函数  : StatusShow
功能  : 状态显示，显示复合解码器状态
输入  : void
输出  : 无
返回  : void
注    :
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
====================================================================*/
void   CHdu2Instance::StatusShow(void)                                    
{
	static char status[4][10] = {"IDLE", "INIT","READY","RUNNING"};
	u16 wChnId = GetInsID();
	THduCfg tHduCfg = g_cHdu2App.GetHduCfg();
	if (wChnId == CInstance::DAEMON)
	{
		u32 dwMcuNodeA = g_cHdu2App.GetMcuNode();
		u32 dwMcuNodeB = g_cHdu2App.GetMcuNodeB();
		Hdu2Print(HDU_LVL_KEYSTATUS, "\n**********HDU2(EQPID:%d,EQPTYPE:%d) INFO**********\n",
			tHduCfg.byEqpId,tHduCfg.GetEqpType());
		Hdu2Print(HDU_LVL_KEYSTATUS, "Status:[%s]   Alias:[%s]   RcvStartPort:[%d]   NodeA:[%d] NodeB:[%d]\n",
			status[CurState()],tHduCfg.achAlias,tHduCfg.wRcvStartPort,dwMcuNodeA,dwMcuNodeB);
		Hdu2Print(HDU_LVL_KEYSTATUS, "ChnOutPortTypeInCfg(0)[%d] ChnOutPortTypeInCfg(1)[%d]\n",
			g_cBrdAgentApp.GetOutPortTypeInCfg(0),g_cBrdAgentApp.GetOutPortTypeInCfg(1));
	}
	else if ( wChnId <= MAXNUM_HDU_CHANNEL )
    {
		Hdu2Print(HDU_LVL_KEYSTATUS, "**********CHN(%d) INFO**********\n",wChnId);
		Hdu2Print(HDU_LVL_KEYSTATUS, "Conf Info:");
		m_cChnConfId.Print();
		THduModePort tHduModePort;
		tHduCfg.GetHduModePort( (u8)wChnId - 1, tHduModePort );
		Hdu2Print(HDU_LVL_KEYSTATUS, "Status:%s Param(OutType:%d(%s),OutMode:%d(%s),ZoomRate:%d(%s),ScalingMode:%d(%s))\n",
			status[CurState()],tHduModePort.GetOutPortType(),tHduModePort.GetOutTypeStr(),tHduModePort.GetOutModeType(),tHduModePort.GetOutModeStr(),
			tHduModePort.GetZoomRate(),tHduModePort.GetZoomRateStr(),tHduModePort.GetScalingMode(),tHduModePort.GetScalingModeStr());
        m_cHdu2ChnMgrGrp.ShowInfo(wChnId - 1);
    }
	else
	{
		Hdu2Print(HDU_LVL_ERROR, "------------------INVALID CHN(%d)----------------\n",wChnId);
	}
	return;
}
/*====================================================================
函数  : ShowInfo
功能  : 显示信息
输入  : 无
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
====================================================================*/
void  CHdu2ChnMgrGrp::ShowInfo(u8 byChnId)
{
	u8 abyDecKey[64];
	s32 iLen = 0;
	m_tMediaDec.GetEncryptKey(abyDecKey, &iLen);
	abyDecKey[iLen] = 0;
	TKdvRcvStatus tKdvRcvStatus;
	u8 byHduChnCount = 0;

	if ( g_cHdu2App.GetHduChnMode(byChnId) == HDUCHN_MODE_FOUR )
	{
		byHduChnCount = HDU_MODEFOUR_MAX_SUBCHNNUM;
	}
	else if(g_cHdu2App.GetHduChnMode(byChnId) == HDUCHN_MODE_ONE)
	{
		byHduChnCount = 1;
	}

	for (u8 byIndex=0; byIndex<byHduChnCount; byIndex++)
	{
		m_acVidMediaRcv[byIndex].GetStatus ( tKdvRcvStatus );
		BOOL32 bIsVidStartRcv = tKdvRcvStatus.m_bRcvStart;
		u16 wVidLocalRcvPort = tKdvRcvStatus.m_tRcvAddr.m_tLocalNet.m_wRTPPort;
		m_cAudMediaRcv.GetStatus ( tKdvRcvStatus );
		BOOL32 bIsAudStartRcv = tKdvRcvStatus.m_bRcvStart;
		u16 wAudLocalRcvPort = tKdvRcvStatus.m_tRcvAddr.m_tLocalNet.m_wRTPPort;
		TKdvDecStatus tKdvDecStatus;
		m_cVidDecoder.GetDecoderStatus(tKdvDecStatus, byIndex);
		BOOL32 bIsVidDecing = tKdvDecStatus.m_bDecStart;
		m_cAudDecoder.GetDecoderStatus(tKdvDecStatus);
		BOOL32 bIsAudDecing = tKdvDecStatus.m_bDecStart;

		Hdu2Print(HDU_LVL_ERROR, "1.HduVmpMode Info    :(HduMode: %d)\n", g_cHdu2App.GetHduChnMode(byChnId));
		Hdu2Print(HDU_LVL_ERROR, "2.ChnMgrGroup Info   :(Mode:%d,IsNeedRS:%d,IdleChnBackGround:%d)\n",m_abyMode[byIndex],m_bIsNeedRs,m_tIdleChnBackGround);
		Hdu2Print(HDU_LVL_ERROR, "3.DECParam Info      :(Mode:%u,key len:%d,key:%s)\n",m_tMediaDec.GetEncryptMode(),iLen,abyDecKey);
		Hdu2Print(HDU_LVL_ERROR, "4.Vid Info           :(IsDecing:%d,IsRecving:%d,RecvPort:%d,PayLoad(Real:%d,Act:%d))\n",
			bIsVidDecing,bIsVidStartRcv,wVidLocalRcvPort,m_atVidDp[byIndex].GetRealPayLoad(),m_atVidDp[byIndex].GetActivePayload());
		Hdu2Print(HDU_LVL_ERROR, "5.Aud Info           :(IsDecing:%d,IsRecving:%d,RecvPort:%d,PayLoad(Real:%d,Act:%d,),Volume(%d)\n",
			bIsAudDecing,bIsAudStartRcv,wAudLocalRcvPort,m_tAudDp.GetRealPayLoad(),m_tAudDp.GetActivePayload(),m_byAudChnNum);
	} //for u8 byIndex=0 over
	return;
}
/* --------------------CHdu2ChnMgrGrp类的实现  end-----------------------*/


/* ------------------CHdu2Instance类的实现  start----------------------*/
/*====================================================================
函数  : CHdu2Instance
功能  : 构造函数
输入  : 
输出  : 
返回  : 
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
CHdu2Instance::CHdu2Instance()
{
}
/*====================================================================
函数  : ~CHdu2Instance
功能  : 析构函数
输入  : 
输出  : 
返回  : 
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
CHdu2Instance::~CHdu2Instance()
{
}
/*====================================================================
函数  : DaemonInstanceEntry
功能  : 普通实例消息入口
输入  : CMessage* const, CApp*
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
void CHdu2Instance::DaemonInstanceEntry( CMessage* const pMsg, CApp* pcApp )
{
	if ( NULL == pMsg || NULL == pcApp )
	{
		Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonInstanceEntry] pMsg or pcApp is Null!\n");
        return;
	}
    Hdu2Print(HDU_LVL_GENERAL,  "[DaemonInstanceEntry]Receive msg.%d<%s>\n", pMsg->event, ::OspEventDesc(pMsg->event) );
	if ( pMsg->event == EV_HDU_STATUSSHOW )
    {
		StatusShow();
		return;
	}
 	switch (CurState())
 	{
    case emIDLE:
		switch (pMsg->event)
		{
			//上电初始化
			case OSP_POWERON:
				DaemonProcPowerOn(pMsg);
				break;
 			default:
 				Hdu2Print(HDU_LVL_WARNING,  "[DaemonInstanceEntry]wrong message %d<%s> in eIDLE!\n", 
					pMsg->event, OspEventDesc(pMsg->event));
 		}
 		break;
 
 	case emINIT:
 		switch (pMsg->event)
 		{
			// 建链
			case EV_HDU_CONNECT_TIMER:
				DaemonProcConnectTimeOut(TRUE);
				break;
				
			// 建链
			case EV_HDU_CONNECTB_TIMER:
				DaemonProcConnectTimeOut(FALSE);
				break;

			// 注册
			case EV_HDU_REGISTER_TIMER:  	
				DaemonProcRegisterTimeOut(TRUE);
				break;
			// 注册
			case EV_HDU_REGISTERB_TIMER:  	
				DaemonProcRegisterTimeOut(FALSE);
				break;
				
			// MCU 注册应答消息
			case MCU_HDU_REG_ACK:
				DaemonProcMcuRegAck(pMsg);
				break;
				
			// MCU 拒绝注册
			case MCU_HDU_REG_NACK:
				DaemonProcMcuRegNack(pMsg);
				break;
				
			default: 
				Hdu2Print(HDU_LVL_WARNING,  "[DaemonInstanceEntry] wrong message %d<%s> in eINIT\n",
					pMsg->event, OspEventDesc(pMsg->event));
		}
		break;
	case emREADY:
		switch (pMsg->event)
 		{
			// 建链
			case EV_HDU_CONNECT_TIMER:
				DaemonProcConnectTimeOut(TRUE);
				break;
				
			// 建链
			case EV_HDU_CONNECTB_TIMER:
				DaemonProcConnectTimeOut(FALSE);
				break;
			// 注册
			case EV_HDU_REGISTER_TIMER:  	
				DaemonProcRegisterTimeOut(TRUE);
				break;
			// 注册
			case EV_HDU_REGISTERB_TIMER:  	
				DaemonProcRegisterTimeOut(FALSE);
				break;
				
			// MCU 注册应答消息
			case MCU_HDU_REG_ACK:
				DaemonProcMcuRegAck(pMsg);
				break;
				
			// MCU 拒绝注册
			case MCU_HDU_REG_NACK:
				DaemonProcMcuRegNack(pMsg);
				break;
				
			// OSP 断链消息
			case OSP_DISCONNECT:
				DaemonProcOspDisconnect(pMsg);
				break;

			// 开始播放
			case MCU_HDU_START_PLAY_REQ:         
				DaemonProcHduStartPlay(pMsg);
				break;
			
			// 切换HDU通道风格
			case MCU_HDU_CHGHDUVMPMODE_REQ:
				DaemonProcHduChgVmpMode(pMsg);
				break;

			// 停止播放
			case MCU_HDU_STOP_PLAY_REQ:            
				DaemonProcHduStopPlay( pMsg );
				break;
			
			// 设置音频载荷
			case MCU_HDU_FRESHAUDIO_PAYLOAD_CMD:
				DaemonProcHduSetAudioPayLoad(pMsg);
				break;

			// 设置音量或静音
			case MCU_HDU_CHANGEVOLUME_CMD:
				DaemonProcHduChangeVolumeReq( pMsg );
				break;

			//取主备倒换状态
			case EV_HDU_GETMSSTATUS_TIMER:
			case MCU_EQP_GETMSSTATUS_ACK:
				DeamonProcGetMsStatusRsp(pMsg);
				break;
			case MCU_HDU_CHANGEMODEPORT_NOTIF:
				DaemonProcModePortChangeNotif(pMsg);
				break;
			case MCU_HDU_CHANGEMODE_CMD:
				DaemonProcChangeModeCmd(pMsg);
				break;
			case MCU_HDU_CHANGEPLAYPOLICY_NOTIF:
				DaemonProcChangePlayPolicy(pMsg);
				break;
			default: 
				Hdu2Print(HDU_LVL_WARNING,  "[DaemonInstanceEntry] wrong message %d<%s> in eREADY\n",
				pMsg->event, OspEventDesc(pMsg->event));
		}
		break;

 	default:
         Hdu2Print(HDU_LVL_WARNING,  "[DaemonInstanceEntry]wrong status.%d for msg.%d<%s>!\n", 
             CurState(), pMsg->event, OspEventDesc(pMsg->event));
		 break;
 	}
}
/*====================================================================
函数  : InstanceEntry
功能  : 普通实例消息入口
输入  : CMessage* const
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
void CHdu2Instance::InstanceEntry( CMessage* const pMsg )
{
	if (NULL == pMsg)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[InstanceEntry]: The pMsg point can not be Null!\n");
		return;
	}

	CServMsg cServMsg(pMsg->content, pMsg->length);
    cServMsg.SetEventId(pMsg->event);
    if ( pMsg->event != EV_HDU_NEEDIFRAME_TIMER )
    {
        Hdu2Print(HDU_LVL_FRAME,  "[InstanceEntry]Chn(%d) Receive msg.%d<%s>\n",GetInsID() - 1, pMsg->event, ::OspEventDesc(pMsg->event) );
    }
	if ( pMsg->event == EV_HDU_STATUSSHOW )
    {
		StatusShow();
		return;
	}

	switch (CurState())
	{
	case emIDLE:
		switch (pMsg->event)
		{
			//通道初始化
			case EV_HDUCHNMGRGRP_CREATE:
				ProcHduChnGrpCreate();
				break;

			default:
				Hdu2Print(HDU_LVL_WARNING,  "[InstanceEntry]CHN(%d) wrong message %d<%s> in eIDLE\n",GetInsID() - 1, pMsg->event, OspEventDesc(pMsg->event));
				break;
		}
		break;
	case emINIT:
		switch (pMsg->event)
		{
			case EV_HDU_INITCHNCFG:
				ProcInitChnCfg();
				break;

			default:
				Hdu2Print(HDU_LVL_WARNING,  "[InstanceEntry]CHN(%d) wrong message %d<%s> in emINIT\n",GetInsID() - 1, pMsg->event, OspEventDesc(pMsg->event));
				break;
		}
		break;
	case emREADY:
		switch (pMsg->event)
        { 
			//开始播放
			case MCU_HDU_START_PLAY_REQ:
				ProcStartPlayReq( cServMsg );
				break;

			// [2013/03/11 chenbing] 
			// 修改HDU多画面模式
			case MCU_HDU_CHGHDUVMPMODE_REQ:
				ProcChangeHduVmpMode( cServMsg );
				break;

			// 设置音频载荷
			case MCU_HDU_FRESHAUDIO_PAYLOAD_CMD:
				ProcHduSetAudioPayLoad(cServMsg);
				break;

			//修改通道配置
			case MCU_HDU_CHANGEMODEPORT_NOTIF:
				ProcChangeChnCfg(pMsg);
				break;

			// 设置hdu音量或静音
			case MCU_HDU_CHANGEVOLUME_CMD:
				ProcHduChangeVolumeReq( cServMsg );
				break;

			case MCU_HDU_CHANGEPLAYPOLICY_NOTIF:
				ProcChangePlayPolicy(pMsg);
				break;
			//查寻当前模式
			case EVENT_HDU_SHOWMODWE:
				ProcShowMode();
				break;

			//清除当前模式
			case EVENT_HDU_CLEARMODWE:
				ProcClearMode();
				break;

				//断链处理
			case OSP_DISCONNECT:
				ProcDisConnect();
				break;

			default: 
				Hdu2Print(HDU_LVL_WARNING,  "[InstanceEntry]CHN(%d) wrong message %d<%s> in emREADY\n",GetInsID() - 1, pMsg->event, OspEventDesc(pMsg->event));
				break;
        }
		break;
	case emRUNNING:
		switch (pMsg->event)
		{
			//开始播放
			case MCU_HDU_START_PLAY_REQ:
				//Hdu2ChnStopPlay( MODE_BOTH );
				ProcStartPlayReq( cServMsg );
				break;

			// [2013/03/11 chenbing] 
			// 修改HDU多画面模式
			case MCU_HDU_CHGHDUVMPMODE_REQ:
				ProcChangeHduVmpMode( cServMsg );
				break;

			// 设置音频载荷
			case MCU_HDU_FRESHAUDIO_PAYLOAD_CMD:
				ProcHduSetAudioPayLoad(cServMsg);
				break;

			//停止播放
			case MCU_HDU_STOP_PLAY_REQ:
				ProcStopPlayReq( cServMsg );
				break;

			//断链处理
			case OSP_DISCONNECT:
				ProcDisConnect();
				break;

			//Hdu请求Mcu发关键帧
			case EV_HDU_NEEDIFRAME_TIMER:
				ProcTimerNeedIFrame();
				break;

			//修改通道配置
			case MCU_HDU_CHANGEMODEPORT_NOTIF:
				ProcChangeChnCfg(pMsg);
				break;
				
			//设置hdu2通道音量或静音
			case MCU_HDU_CHANGEVOLUME_CMD:
				ProcHduChangeVolumeReq( cServMsg );
				break;

			case MCU_HDU_CHANGEMODE_CMD:
				ProcChangeModeCmd();
				break;

			case MCU_HDU_CHANGEPLAYPOLICY_NOTIF:
				ProcChangePlayPolicy(pMsg);
				break;

			//查寻当前模式
			case EVENT_HDU_SHOWMODWE:
				ProcShowMode();
				break;
				
			//清除当前模式
			case EVENT_HDU_CLEARMODWE:
				ProcClearMode();
				break;

			default:
				Hdu2Print(HDU_LVL_WARNING,  "[InstanceEntry]CHN(%d) wrong message %d<%s> in emRUNNING\n",GetInsID() - 1, pMsg->event, OspEventDesc(pMsg->event));
				break;
		}
		break;
	default:
        Hdu2Print(HDU_LVL_WARNING,  "[InstanceEntry]wrong state %d for msg.%d<%s> \n",
            CurState(), pMsg->event, OspEventDesc(pMsg->event));
        break;
	}

	return;
}
/*====================================================================
函数  : DaemonProcPowerOn
功能  : 初始化
输入  : CMessage* const，CApp*
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
void  CHdu2Instance::DaemonProcPowerOn( CMessage* const pMsg)      
{
	printf( "recv power on\n" );
	THduCfg tHduCfg = g_cHdu2App.GetHduCfg();
	if ( pMsg->length == sizeof(THduCfg) )
    {
        memcpy( (u8*)&tHduCfg, (u8*)pMsg->content, sizeof(THduCfg));
		g_cHdu2App.SetHduCfg(tHduCfg);
    }
	else
	{
		Hdu2Print(HDU_LVL_ERROR,  "[DaemonProcPowerOn]pMsg->length(%d) == sizeof(THduCfg)(%d),So Return!\n",pMsg->length,sizeof(THduCfg));
		return;
	}
	tHduCfg = g_cHdu2App.GetHduCfg();
	s32 nRet = 0;
	//待更新，给底层媒控初始化接口预留
	printf( "start HardMPUInit subtype.%d\n",tHduCfg.GetHduSubType() );
	
	if( tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2_S )
	{
		nRet = HardMPUInit((u32)en_HDU2_Board_S);
	}
	else
	{
		nRet = HardMPUInit((u32)en_HDU2_Board);
	}	
	if ((u16)CODEC_NO_ERROR != nRet )
	{
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcPowerOn]HardMPUInit error:%d\n", nRet);
		return;
	}

	printf( "HardMPUInit success!!!\n" );


    u16 wRet = KdvSocketStartup();
    if ( MEDIANET_NO_ERROR != wRet)
    {
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcPowerOn]KdvSocketStartup Faileded, error: %d\n", wRet);
        return ;
    }
	printf( "KdvSocketStartup success!!!\n" );
	for (u8 byLoop = 1; byLoop <= MAXNUM_HDU_CHANNEL; byLoop++ )
	{
		post(MAKEIID(GetAppID(), byLoop), EV_HDUCHNMGRGRP_CREATE);
	}
    if( 0 != tHduCfg.dwConnectIP )
    {
        SetTimer(EV_HDU_CONNECT_TIMER, HDU_CONNETC_TIMEOUT );
    }
    if( 0 != tHduCfg.dwConnectIpB )
    {
        SetTimer(EV_HDU_CONNECTB_TIMER, HDU_CONNETC_TIMEOUT );
    }
    NextState((u32)emINIT);
	printf( "[DaemonProcPowerOn]PowerOn Successed!So DAEMONINST State Change: IDLE--->INIT!\n");
	Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcPowerOn]PowerOn Successed!So DAEMONINST State Change: IDLE--->INIT!\n");
    return;
}
/*====================================================================
函数  : DaemonProcHduStartPlay
功能  : 管理实例将开启消息分发给对应通道
输入  : CMessage* const
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
void  CHdu2Instance::DaemonProcHduStartPlay( CMessage* const pMsg )
{
	CServMsg cServMsg(pMsg->content, pMsg->length);
    post(MAKEIID(GetAppID(), cServMsg.GetChnIndex()+1), MCU_HDU_START_PLAY_REQ, pMsg->content,pMsg->length);
}

/*====================================================================
函数  : DaemonProcHduChgVmpMode
功能  : HDU通道切换通道模式(多画面或单通道模式)
输入  : CMessage* const
输出  : 无
返回  : 无
注    : HDU多画面接口
----------------------------------------------------------------------
修改记录    ：
日  期       版本         修改人      修改内容
2013/03/11	 4.7.2		  陈兵		   创建(HDU多画面支持)
====================================================================*/
void  CHdu2Instance::DaemonProcHduChgVmpMode( CMessage* const pMsg )
{
	CServMsg cServMsg(pMsg->content, pMsg->length);
    post(MAKEIID(GetAppID(), cServMsg.GetChnIndex()+1), MCU_HDU_CHGHDUVMPMODE_REQ, pMsg->content,pMsg->length);
}

/*====================================================================
函数  : DaemonProcHduSetAudioPayLoad
功能  : 设置音频载荷
输入  : CMessage* const
输出  : 无
返回  : 无
注    : HDU多画面接口
----------------------------------------------------------------------
修改记录    ：
日  期       版本         修改人      修改内容
2013/03/11	 4.7.2		  陈兵		   创建(HDU多画面支持)
====================================================================*/
void  CHdu2Instance::DaemonProcHduSetAudioPayLoad(CMessage* const pMsg)
{
	CServMsg cServMsg(pMsg->content, pMsg->length);
    post(MAKEIID(GetAppID(), cServMsg.GetChnIndex()+1), MCU_HDU_FRESHAUDIO_PAYLOAD_CMD, pMsg->content,pMsg->length);
}

/*====================================================================
函数  : DaemonProcHduStopPlay
功能  : 管理实例将停止消息分发给对应通道
输入  : CMessage* const
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
void  CHdu2Instance::DaemonProcHduStopPlay( CMessage* const pMsg )
{
	CServMsg cServMsg(pMsg->content, pMsg->length);
    post(MAKEIID(GetAppID(), cServMsg.GetChnIndex()+1), MCU_HDU_STOP_PLAY_REQ, pMsg->content,pMsg->length);
	return;

}
/*====================================================================
函数  : DaemonProcOspDisconnect
功能  : 断链处理
输入  : CMessage* const，CApp*
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
void  CHdu2Instance::DaemonProcOspDisconnect( CMessage* const pMsg) 
{
	if ( NULL == pMsg )  
    {
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcOspDisconnect] message's pointer is Null\n");
        return;
    }

    u32 dwNode = *(u32*)pMsg->content;
	u32 dwMcuNodeA = g_cHdu2App.GetMcuNode();
	u32 dwMcuNodeB = g_cHdu2App.GetMcuNodeB();
    if (INVALID_NODE != dwNode)
    {
        OspDisconnectTcpNode(dwNode);
    } 
    if(dwNode == dwMcuNodeA)
    {
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcOspDisconnect] McuNode.A disconnect\n");
        g_cHdu2App.FreeStatusDataA();
        SetTimer(EV_HDU_CONNECT_TIMER, HDU_CONNETC_TIMEOUT);
    }
    else if(dwNode == dwMcuNodeB)
    {
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcOspDisconnect] McuNode.B disconnect\n");
        g_cHdu2App.FreeStatusDataB();
        SetTimer(EV_HDU_CONNECTB_TIMER, HDU_CONNETC_TIMEOUT);
    }
	dwMcuNodeA = g_cHdu2App.GetMcuNode();
	dwMcuNodeB = g_cHdu2App.GetMcuNodeB();
	if (INVALID_NODE != dwMcuNodeA || INVALID_NODE != dwMcuNodeB)
	{
		SetTimer(EV_HDU_GETMSSTATUS_TIMER, 100);
		return;
	}

	if (INVALID_NODE == dwMcuNodeA && INVALID_NODE == dwMcuNodeB)
	{
		for (u8 byLoop = 1; byLoop <= MAXNUM_HDU_CHANNEL; byLoop++ )
		{
			post(MAKEIID(GetAppID(), byLoop), OSP_DISCONNECT);
		}
		g_cHdu2App.SetMcuSSrc(0);
		NextState((u32)emINIT);
		Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcOspDisconnect]DAEMONINST State Change: READY--->INIT!\n");
	}
    return;
}
/*====================================================================
函数  : DaemonProcConnectTimeOut
功能  : 向MCU建链
输入  : BOOL32 连MCUA or MCUB
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
void  CHdu2Instance::DaemonProcConnectTimeOut( BOOL32 bIsConnectA )   
{
	u32 dwConnectNode = INVALID_NODE;
	if (bIsConnectA)
	{
		dwConnectNode = BrdGetDstMcuNode();
	}
	else
	{
		dwConnectNode = BrdGetDstMcuNodeB();
	}
	if (!::OspIsValidTcpNode(dwConnectNode) ||
		OSP_OK != ::OspNodeDiscCBRegQ(dwConnectNode, GetAppID(), CHdu2Instance::DAEMON))
	{
		Hdu2Print(HDU_LVL_WARNING,  "[DaemonProcConnectTimeOut]Connect Mcu %s failed, node: %d!\n", 
				bIsConnectA ? "A" : "B", dwConnectNode);
		if (bIsConnectA)
		{
			SetTimer(EV_HDU_CONNECT_TIMER, HDU_CONNETC_TIMEOUT);
		}
		else
		{
			SetTimer(EV_HDU_CONNECTB_TIMER, HDU_CONNETC_TIMEOUT);
		}
		return;
	}
	if (bIsConnectA)
	{
		KillTimer(EV_HDU_CONNECT_TIMER);
		SetTimer(EV_HDU_REGISTER_TIMER, 10 );
		g_cHdu2App.SetMcuNode(dwConnectNode);
	}
	else
	{
		KillTimer(EV_HDU_CONNECTB_TIMER);
		SetTimer(EV_HDU_REGISTERB_TIMER, 10 );
		g_cHdu2App.SetMcuNodeB(dwConnectNode);
	}
}
/*====================================================================
函数  : DaemonProcRegisterTimeOut
功能  : 向MCU注册
输入  : BOOL32 注册 MCUA or MCUB
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
void  CHdu2Instance::DaemonProcRegisterTimeOut( BOOL32 bIsRegiterA )       
{
	CServMsg       cSvrMsg;
    TPeriEqpRegReq tReg;
	THduCfg tHduCfg = g_cHdu2App.GetHduCfg();

	// 兼容4.7.2HDU2模拟HDU注册到老版本MCU[6/3/2013 chendaiwei]
	u8 byEqpType = GetOldVersionEqpTypeBySubType(tHduCfg.GetHduSubType());
	
    tReg.SetMcuEqp((u8)tHduCfg.wMcuId, tHduCfg.byEqpId, byEqpType);
    tReg.SetPeriEqpIpAddr(htonl(tHduCfg.dwLocalIP));
	tReg.SetChnnlNum(MAXNUM_HDU_CHANNEL);
	if(tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2)
	{
		tReg.SetVersion(DEVVER_HDU2);
	}
	else if (tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2_S)
	{
		tReg.SetVersion(DEVVER_HDU2_S);
	}
	else if (tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2_L)
	{
		tReg.SetVersion(DEVVER_HDU2_L);
	}
	else
	{
		tReg.SetVersion(DEVVER_HDU);
	}
	tReg.SetStartPort(HDU_EQP_STARTPORT);
    tReg.SetHDEqp(TRUE);
    cSvrMsg.SetMsgBody((u8*)&tReg, sizeof(tReg));
    if(bIsRegiterA)
    {
		if (!::OspIsValidTcpNode(g_cHdu2App.GetMcuNode()))
		{
			SetTimer(EV_HDU_CONNECT_TIMER,HDU_CONNETC_TIMEOUT);
			Hdu2Print(HDU_LVL_WARNING,  "[DaemonProcRegisterTimeOut]OspIsValidTcpNode(%d),So Reconnect To Mcu(A)!\n", 
				g_cHdu2App.GetMcuNode());
			return;
		}
		post(MAKEIID(AID_MCU_PERIEQPSSN, tHduCfg.byEqpId),HDU_MCU_REG_REQ,
		cSvrMsg.GetServMsg(),cSvrMsg.GetServMsgLen(),g_cHdu2App.GetMcuNode());
        SetTimer(EV_HDU_REGISTER_TIMER, HDU_REGISTER_TIMEOUT);
    }
    else
    {
		if (!::OspIsValidTcpNode(g_cHdu2App.GetMcuNodeB()))
		{
			SetTimer(EV_HDU_CONNECTB_TIMER,HDU_CONNETC_TIMEOUT);
			Hdu2Print(HDU_LVL_WARNING,  "[DaemonProcRegisterTimeOut]OspIsValidTcpNode(%d),So Reconnect To Mcu(B)!\n", 
				g_cHdu2App.GetMcuNodeB());
			return;
		}
		post(MAKEIID(AID_MCU_PERIEQPSSN, tHduCfg.byEqpId),HDU_MCU_REG_REQ,
		cSvrMsg.GetServMsg(),cSvrMsg.GetServMsgLen(),g_cHdu2App.GetMcuNodeB());
		SetTimer(EV_HDU_REGISTERB_TIMER, HDU_REGISTER_TIMEOUT);
    }
    return;
}
/*====================================================================
函数  : DaemonProcMcuRegAck
功能  : 处理注册ACK回应
输入  : CMessage* const, CApp*
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
void  CHdu2Instance::DaemonProcMcuRegAck(CMessage* const pMsg)      
{
    if(NULL == pMsg)
    {
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcMcuRegAck]Recv Reg Ack Msg, but the msg's pointer is Null\n");
        return;
    }
	u32 dwMcuNodeA = g_cHdu2App.GetMcuNode();
	u32 dwMcuNodeB = g_cHdu2App.GetMcuNodeB();
	if(pMsg->srcnode == dwMcuNodeA)     
    {
		g_cHdu2App.SetMcuIId(pMsg->srcid);
        KillTimer(EV_HDU_REGISTER_TIMER);                
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcMcuRegAck] Regist success to mcu A\n");
    }
    else if(pMsg->srcnode == dwMcuNodeB)
    {
        g_cHdu2App.SetMcuIIdB(pMsg->srcid);
        KillTimer(EV_HDU_REGISTERB_TIMER);
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcMcuRegAck] Regist success to mcu B\n");       
    }
	CServMsg cServMsg(pMsg->content, pMsg->length);
	TPeriEqpRegAck *ptRegAck = NULL;
	ptRegAck = (TPeriEqpRegAck*)cServMsg.GetMsgBody();
	if (NULL == ptRegAck)
	{
		Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcMcuRegAck] ptRegAck is null!\n");
		return;
	}
    if ( g_cHdu2App.GetMcuSSrc() == 0 )
    {
        g_cHdu2App.SetMcuSSrc(ptRegAck->GetMSSsrc());
    }
    else
    {
        if ( g_cHdu2App.GetMcuSSrc()  != ptRegAck->GetMSSsrc() )
        {
            Hdu2Print(HDU_LVL_ERROR,  "[DaemonProcMcuRegAck] MPC SSRC ERROR(%u<-->%u), Disconnect Both Nodes!\n", 
				g_cHdu2App.GetMcuSSrc(), ptRegAck->GetMSSsrc());
            if ( OspIsValidTcpNode(dwMcuNodeA) )
            {
                OspDisconnectTcpNode(dwMcuNodeA);
            }
            if ( OspIsValidTcpNode(dwMcuNodeB) )
            {
                OspDisconnectTcpNode(dwMcuNodeB);
            }      
        }
		else
		{
			StatusNotify();
		}
		return;
    }
	
	TEqpHduCfgInfo *ptCfgInfo = NULL;
	
	ptCfgInfo = (TEqpHduCfgInfo*)(cServMsg.GetMsgBody() + sizeof(TPrsTimeSpan) + sizeof(TPeriEqpRegAck));
    if (NULL == ptCfgInfo)
    {
		Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcMcuRegAck]ptCfgInfo is Null!\n");
		return;
    }
	THduCfg tHduCfg = g_cHdu2App.GetHduCfg();
	if( tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU_M ||
		tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU_L ||
		tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU_H)
	{
		THduChnlModePort tHduChnModePort;
		THduModePort tModePort;
		for (u8 byChnIdx = 0 ;byChnIdx < MAXNUM_HDU_CHANNEL;byChnIdx++)
		{
			memset(&tModePort, 0x0, sizeof(tModePort));
			memset(&tHduChnModePort, 0x0, sizeof(tHduChnModePort));
			ptCfgInfo->GetHduChnlModePort(byChnIdx, tHduChnModePort);
			tModePort.SetOutModeType(tHduChnModePort.GetOutModeType());
			tModePort.SetOutPortType(tHduChnModePort.GetOutPortType());
			tModePort.SetZoomRate(tHduChnModePort.GetZoomRate());
			s8	HDUCFG_FILE[KDV_MAX_PATH] = {0};
			sprintf(HDUCFG_FILE, "%s/mcueqp.ini", DIR_CONFIG);
			s32 nValue;
			::GetRegKeyInt( HDUCFG_FILE, "EQPDEBUG", "DecResizeMode", 0, &nValue );
			tModePort.SetScalingMode(nValue);
			tHduCfg.SetHduModePort(byChnIdx, tModePort);
		}
	}
    
	tHduCfg.wRcvStartPort = ptCfgInfo->GetEqpStartPort();
    memcpy(tHduCfg.achAlias, ptCfgInfo->GetAlias(), MAXLEN_EQP_ALIAS);
	g_cHdu2App.SetHduCfg(tHduCfg);

	for (u8 byLoop = 1; byLoop <= MAXNUM_HDU_CHANNEL; byLoop++ )
	{
		post(MAKEIID(GetAppID(), byLoop), EV_HDU_INITCHNCFG);
		if (tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU_H)
		{
			Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcMcuRegAck]EQP_TYPE_HDU_H Only Has One Chn,So break!\n");
			printf("[DaemonProcMcuRegAck]EQP_TYPE_HDU_H Only Has One Chn,So break!\n");
			break;
		}
		
	}
	NEXTSTATE((u32)emREADY);
	TPrsTimeSpan *ptPrsTime = NULL;
	ptPrsTime = (TPrsTimeSpan*)(cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck));
	g_cHdu2App.SetPrsTimeSpan(*ptPrsTime);
	StatusNotify();
	Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcMcuRegAck]RegAck Successed!DAEMONINST State Change: INIT--->READY!\n");
	return;
}
/*====================================================================
函数  : DaemonProcHduChangeVolumeReq
功能  : 制式改变通知
输入  : CMessage* const
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
void  CHdu2Instance::DaemonProcHduChangeVolumeReq( CMessage* const pMsg )
{
	CServMsg cServMsg(pMsg->content, pMsg->length);
	THduVolumeInfo *ptHduVolumeInfo = (THduVolumeInfo*)cServMsg.GetMsgBody();
	post(MAKEIID(GetAppID(), ptHduVolumeInfo->GetChnlIdx()+1), pMsg->event, pMsg->content,pMsg->length);
}
/*====================================================================
函数  : DaemonProcMcuRegNack
功能  : 制式改变通知
输入  : CMessage* const
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
void  CHdu2Instance::DaemonProcModePortChangeNotif( CMessage* const pMsg )  
{
	if ( NULL == pMsg)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[DaemonProcModePortChangeNotif]NULL == pMsg,So Return!\n");
		return;
	}
	THduChnlModePort *ptHduChnModePort = (THduChnlModePort*)pMsg->content;
    THduModePort tModePort;
	THduCfg tHduCfg = g_cHdu2App.GetHduCfg();
    for (u8 byLoop=0; byLoop<MAXNUM_HDU_CHANNEL; byLoop++)
    {
		tHduCfg.GetHduModePort(byLoop, tModePort);
		tModePort.SetOutModeType(ptHduChnModePort[byLoop].GetOutModeType());
		tModePort.SetOutPortType(ptHduChnModePort[byLoop].GetOutPortType());
		tModePort.SetZoomRate(ptHduChnModePort[byLoop].GetZoomRate());
		tModePort.SetScalingMode(ptHduChnModePort[byLoop].GetScalingMode());
        tHduCfg.SetHduModePort(byLoop, tModePort);
		post(MAKEIID(GetAppID(), byLoop+1), MCU_HDU_CHANGEMODEPORT_NOTIF, &tModePort,sizeof(THduModePort));
		Hdu2Print(HDU_LVL_WARNING,  "[DaemonProcModePortChangeNotif]hdu chn%d's config changed!\n", byLoop);

		if (tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU_H)
		{
			Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcModePortChangeNotif]EQP_TYPE_HDU_H Only Has One Chn,So break!\n");
			printf("[DaemonProcModePortChangeNotif]EQP_TYPE_HDU_H Only Has One Chn,So break!\n");
			break;
		}
    }
}      
/*====================================================================
函数  : DaemonProcMcuRegNack
功能  : 处理注册NACK回应
输入  : CMessage* const
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
void  CHdu2Instance::DaemonProcMcuRegNack(CMessage* const pMsg)               
{
	if( pMsg->srcnode == g_cHdu2App.GetMcuNode())    
    {
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcMcuRegNack]Hdu registe be refused by A.\n");
		SetTimer(EV_HDU_REGISTER_TIMER, HDU_REGISTER_TIMEOUT);
    }
    if( pMsg->srcnode == g_cHdu2App.GetMcuNodeB())    
    {
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[DaemonProcMcuRegNack]Hdu registe be refused by B.\n");
		SetTimer(EV_HDU_REGISTERB_TIMER, HDU_REGISTER_TIMEOUT);
    }
}
/*====================================================================
函数  : DeamonProcGetMsStatusRsp
功能  : 取主备倒换状态
输入  : CMessage* const
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
void  CHdu2Instance::DeamonProcGetMsStatusRsp( CMessage* const pMsg )         
{
	BOOL bSwitchOk = FALSE;
	CServMsg cServMsg(pMsg->content, pMsg->length);
	if( MCU_EQP_GETMSSTATUS_ACK == pMsg->event )
	{
		TMcuMsStatus *ptMsStatus = (TMcuMsStatus *)cServMsg.GetMsgBody();
        KillTimer(EV_HDU_GETMSSTATUS_TIMER);
        Hdu2Print(HDU_LVL_KEYSTATUS,  "[DeamonProcGetMsStatusRsp]receive msg MCU_EQP_GETMSSTATUS_ACK. IsMsSwitchOK :%d\n",ptMsStatus->IsMsSwitchOK());
        if(ptMsStatus->IsMsSwitchOK()) //倒换成功
        {
            bSwitchOk = TRUE;
        }
	}	
	if( !bSwitchOk )//倒换失败或者超时
	{
		u32 dwMcuNodeA = g_cHdu2App.GetMcuNode();
		u32 dwMcuNodeB = g_cHdu2App.GetMcuNodeB();
		if (OspIsValidTcpNode(dwMcuNodeA))
		{
			post( g_cHdu2App.GetMcuIId(), EQP_MCU_GETMSSTATUS_REQ, NULL, 0, dwMcuNodeA );           
			Hdu2Print(HDU_LVL_KEYSTATUS,  "[DeamonProcGetMsStatusRsp]post EQP_MCU_GETMSSTATUS_REQ to McuNode.A\n");
		}
		if (OspIsValidTcpNode(dwMcuNodeB))
		{
			post( g_cHdu2App.GetMcuIIdB(), EQP_MCU_GETMSSTATUS_REQ, NULL, 0, dwMcuNodeB );        
			Hdu2Print(HDU_LVL_KEYSTATUS,  "[DeamonProcGetMsStatusRsp]post EQP_MCU_GETMSSTATUS_REQ to McuNode.B\n");
		}
		SetTimer(EV_HDU_GETMSSTATUS_TIMER, WAITING_MSSTATUS_TIMEOUT);
	}
}
/*====================================================================
函数  : DaemonProcChangeModeCmd
功能  : 改变模式处理
输入  : CMessage* const
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2012/02/24   4.7         田志勇        创建
====================================================================*/
void  CHdu2Instance::DaemonProcChangeModeCmd( CMessage* const pMsg )
{
	CServMsg cServMsg(pMsg->content, pMsg->length);
	post(MAKEIID(GetAppID(), cServMsg.GetChnIndex()+1), pMsg->event, pMsg->content,pMsg->length);
}
/*====================================================================
函数  : ProcHduChnGrpCreate
功能  : 初始化HDU
输入  : void
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
void  CHdu2Instance::ProcHduChnGrpCreate(void)
{
	u16 wChnIdx = GetInsID() - 1;
	
	printf( "[ProcHduChnGrpCreate] create.%d\n",g_cHdu2App.GetHduCfg().GetHduSubType() );

	BOOL32 bRet = m_cHdu2ChnMgrGrp.Create(wChnIdx,g_cHdu2App.GetHduCfg().GetHduSubType() == HDU_SUBTYPE_HDU2_S);
	
	if (!bRet)
	{
		Hdu2Print(HDU_LVL_ERROR, "[ProcHduChnGrpCreate]Chn(%d) Create Failed!So return\n",wChnIdx);
		return;
	}
	NextState((u32)emINIT);
	Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcHduChnGrpCreate]Chn(%d) Create Successed!So Chn State Change: IDLE--->INIT!\n",wChnIdx);
}
/*====================================================================
函数  : ProcInitChn
功能  : 初始化HDU2通道
输入  : void
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
void  CHdu2Instance::ProcInitChnCfg(void)
{
	u16 wChnIdx = GetInsID() - 1;
	THduModePort tChnCfg;
	TVidSrcInfo tVidSrcInfo;
	memset( &tVidSrcInfo, 0x0, sizeof(tVidSrcInfo) );
	memset(&tChnCfg, 0x0, sizeof(THduModePort));
	THduCfg tHduCfg = g_cHdu2App.GetHduCfg();
	tHduCfg.GetHduModePort(wChnIdx, tChnCfg);
	u8 byChnOutPortType = g_cBrdAgentApp.GetOutPortTypeInCfg((u8)wChnIdx);
	if (g_cBrdAgentApp.GetBrdPosition().byBrdID == BRD_TYPE_HDU && byChnOutPortType !=0 )//模拟HDU，先看配置文件中配置的输出接口类型
	{
		tChnCfg.SetOutPortType(byChnOutPortType - 1);
	}
	BOOL32 bIsNotHdu2Brd = g_cBrdAgentApp.GetBrdPosition().byBrdID == BRD_TYPE_HDU2_L ? TRUE : FALSE;
	CheckChnCfgParam(tChnCfg, bIsNotHdu2Brd);
	tHduCfg.SetHduModePort(wChnIdx,tChnCfg);
	g_cHdu2App.SetHduCfg(tHduCfg);

	Hdu2Print(HDU_LVL_WARNING, "[ProcInitChnCfg]new cfg param(%s,%s,%s,%s)\n",
		tChnCfg.GetOutTypeStr(),tChnCfg.GetOutModeStr(),tChnCfg.GetZoomRateStr(),tChnCfg.GetScalingModeStr());

	g_cHdu2App.SetHduCfg(tHduCfg);
	if (!m_cHdu2ChnMgrGrp.SetVidPlyPortType(GetVidPlyPortTypeByChnAndType(wChnIdx,tChnCfg.GetOutPortType())))
	{
		Hdu2Print(HDU_LVL_WARNING, "[ProcInitChnCfg]SetVidPlyPortType Failed!\n");
	}
	BOOL32 bIsHdu2 = (tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2 || tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2_S);
	if (!m_cHdu2ChnMgrGrp.GetVidSrcInfoByTypeAndMode(tChnCfg.GetOutPortType(),tChnCfg.GetOutModeType(), 
			&tVidSrcInfo,bIsHdu2))
	{
		Hdu2Print(HDU_LVL_WARNING, "[ProcInitChnCfg]Chn(%d) GetVidSrcInfoByTypeAndMode() Failed!\n",wChnIdx);
	}
	if (!m_cHdu2ChnMgrGrp.SetVideoPlyInfo(&tVidSrcInfo))
	{
		Hdu2Print(HDU_LVL_WARNING, "[ProcInitChnCfg]Chn(%d) SetVideoPlyInfo() Failed!\n",wChnIdx);
	}
	u16 wWidth, wHeigh;
	switch (tChnCfg.GetZoomRate())
	{
	case HDU_ZOOMRATE_4_3:
		wWidth = 4;
		wHeigh = 3;
		break;
		
	case HDU_ZOOMRATE_16_9:
		wWidth = 16;
		wHeigh = 9;
		break;
		
	default:
		wWidth = 0;
		wHeigh = 0;
	}
	if (!m_cHdu2ChnMgrGrp.SetPlayScales(wWidth, wHeigh))
	{
		Hdu2Print(HDU_LVL_WARNING, "[ProcInitChnCfg]Chn(%d) SetPlayScales() Failed!\n",wChnIdx);
	}
	if (!m_cHdu2ChnMgrGrp.SetVidDecResizeMode(tChnCfg.GetScalingMode()))
	{
		Hdu2Print(HDU_LVL_WARNING, "[ProcInitChnCfg]Chn(%d) SetVidDecResizeMode() Failed!\n",wChnIdx);
	}
	if (!m_cHdu2ChnMgrGrp.SetVolume( HDU_VOLUME_DEFAULT ))
	{
		Hdu2Print(HDU_LVL_WARNING, "[ProcInitChnCfg]Chn(%d) SetAudioVolume() Failed!\n",wChnIdx);
	}
	if (!m_cHdu2ChnMgrGrp.SetIsMute( FALSE ))
	{
		Hdu2Print(HDU_LVL_WARNING, "[ProcInitChnCfg]Chn(%d) SetIsMute() Failed!\n",wChnIdx);
	}

	g_cHdu2App.SetHduChnMode(wChnIdx, HDUCHN_MODE_ONE );

	NextState((u32)emREADY);
	SendChnNotify(TRUE);
	Hdu2Print(HDU_LVL_KEYSTATUS,  "[ProcInitChnCfg]Chn(%d) Create Successed!So Chn State Change: INIT--->READY!\n",wChnIdx);
}
/*====================================================================
函数  : ProcHduChangeVolumeReq
功能  : 设置音量或静音
输入  : CServMsg &cServMsg
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
void  CHdu2Instance::ProcHduChangeVolumeReq( CServMsg &cServMsg )
{
    THduVolumeInfo tHduVolumeInfo = *( THduVolumeInfo* )cServMsg.GetMsgBody();
	BOOL32 bRet;
	bRet = m_cHdu2ChnMgrGrp.SetVolume( tHduVolumeInfo.GetVolume() );
	if (!bRet)
	{
		Hdu2Print(HDU_LVL_WARNING, "[ProcHduChangeVolumeReq]SetAudioVolume Failed!\n" );
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL, "[ProcHduChangeVolumeReq]SetAudioVolume sucessful!\n" );
	}
	bRet = m_cHdu2ChnMgrGrp.SetIsMute( tHduVolumeInfo.GetIsMute() );
	if (!bRet)
	{
		Hdu2Print(HDU_LVL_WARNING, "[ProcHduChangeVolumeReq]SetAudioMute Failed!\n" );
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[ProcHduChangeVolumeReq]SetAudioMute sucessful!\n" );
	}
	SendChnNotify();
}
/*====================================================================
函数  : ProcChangeModeCmd
功能  : 设置模式
输入  : CServMsg &cServMsg
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
2013/03/11	 4.7.2		  陈兵		   修改(HDU多画面支持)
====================================================================*/
void  CHdu2Instance::ProcChangeModeCmd()
{
// 	u8 byIsAdd = *(u8*)cServMsg.GetMsgBody();
// 	u8 byChgMode = *(u8*)(cServMsg.GetMsgBody() + sizeof(u8));
// 
// 	if (byIsAdd >= 1)
// 	{
// 		if (g_cHdu2App.GetHduChnMode(GetInsID() - 1) == HDUCHN_MODE_FOUR)
// 		{
// 			if (!Hdu2ChnStartPlay( byChgMode, 0, TRUE ))
// 			{
// 				Hdu2Print(HDU_LVL_WARNING,  "[ProcChangeModeCmd]Hdu2ChnStartPlay(%d) HduMode:(%d) Failed!\n", byChgMode, HDUCHN_MODE_FOUR);
// 			}
// 		}
// 		else if (g_cHdu2App.GetHduChnMode(GetInsID() - 1) == HDUCHN_MODE_ONE)
// 		{
// 			if (!Hdu2ChnStartPlay( byChgMode ))
// 			{
// 				Hdu2Print(HDU_LVL_WARNING,  "[ProcChangeModeCmd]Hdu2ChnStartPlay(%d)HduMode:(%d) Failed!\n", byChgMode, HDUCHN_MODE_ONE);
// 			}
// 		}
// 		else
// 		{
// 			//
// 		}
// 	} 
// 	else
// 	{
// 		if (g_cHdu2App.GetHduChnMode(GetInsID() - 1) == HDUCHN_MODE_FOUR)
// 		{
// 			if (!Hdu2ChnStopPlay( byChgMode, 0, TRUE ))
// 			{
// 				Hdu2Print(HDU_LVL_WARNING,  "[ProcChangeModeCmd]Hdu2ChnStopPlay(%d) HduMode:(%d) Failed!\n", byChgMode, HDUCHN_MODE_FOUR);
// 			}
// 		}
// 		else if (g_cHdu2App.GetHduChnMode(GetInsID() - 1) == HDUCHN_MODE_ONE)
// 		{
// 			if (!Hdu2ChnStopPlay( byChgMode ))
// 			{
// 				Hdu2Print(HDU_LVL_WARNING,  "[ProcChangeModeCmd]Hdu2ChnStopPlay(%d) HduMode:(%d) Failed!\n", byChgMode, HDUCHN_MODE_ONE);
// 			}
// 		}
// 		else
// 		{
// 			//
// 		}
// 	}
// 	SendChnNotify();
 }
/*====================================================================
函数  : ProcChangeChnCfg
功能  : 修改通道配置
输入  : CMessage* const
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
void CHdu2Instance::ProcChangeChnCfg( CMessage* const pMsg )
{
	u16 wChnIdx = GetInsID() - 1;
	THduModePort tHdu2ChnCfg;
	TVidSrcInfo tVidSrcInfo;
    u16 wWidth;
	u16 wHeigh;
	THduCfg tHduCfg = g_cHdu2App.GetHduCfg();
	tHdu2ChnCfg = *(THduModePort*)pMsg->content;
	u8 byChnOutPortType = g_cBrdAgentApp.GetOutPortTypeInCfg((u8)wChnIdx);
	if (g_cBrdAgentApp.GetBrdPosition().byBrdID == BRD_TYPE_HDU && byChnOutPortType !=0 )//模拟HDU，先看配置文件中配置的输出接口类型
	{
		tHdu2ChnCfg.SetOutPortType(byChnOutPortType - 1);
	}
	BOOL32 bIsNotHdu2Brd = g_cBrdAgentApp.GetBrdPosition().byBrdID == BRD_TYPE_HDU2_L ? TRUE : FALSE;
	CheckChnCfgParam(tHdu2ChnCfg, bIsNotHdu2Brd);
	tHduCfg.SetHduModePort(wChnIdx,tHdu2ChnCfg);
	g_cHdu2App.SetHduCfg(tHduCfg);
	Hdu2Print(HDU_LVL_WARNING,  "[ProcInitChnCfg]new cfg param(%s,%s,%s,%s)\n",
		tHdu2ChnCfg.GetOutTypeStr(),tHdu2ChnCfg.GetOutModeStr(),tHdu2ChnCfg.GetZoomRateStr(),tHdu2ChnCfg.GetScalingModeStr());
	if (!m_cHdu2ChnMgrGrp.SetVidPlyPortType(GetVidPlyPortTypeByChnAndType(wChnIdx,tHdu2ChnCfg.GetOutPortType())))
	{
		Hdu2Print(HDU_LVL_WARNING,  "[ProcChangeChnCfg]SetVidPlyPortType Failed!\n");
	}
	BOOL32 bIsHdu2 =  (tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2 || tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2_S);
	if (m_cHdu2ChnMgrGrp.GetVidSrcInfoByTypeAndMode(tHdu2ChnCfg.GetOutPortType(),tHdu2ChnCfg.GetOutModeType(), 
		&tVidSrcInfo,bIsHdu2))
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[ProcChangeChnCfg]GetVidSrcInfoByTypeAndMode suceeded!\n");
	}
	else
	{
		Hdu2Print(HDU_LVL_WARNING,  "[ProcChangeChnCfg]GetVidSrcInfoByTypeAndMode Failed\n");
	}
	if (!m_cHdu2ChnMgrGrp.SetVideoPlyInfo(&tVidSrcInfo))
	{
		Hdu2Print(HDU_LVL_WARNING,  "[ProcChangeChnCfg]SetVideoPlyInfo Failed!\n");
	}
	switch (tHdu2ChnCfg.GetZoomRate())
	{
		case HDU_ZOOMRATE_4_3:
			wWidth = 4;
			wHeigh = 3;
			break;
		case HDU_ZOOMRATE_16_9:
			wWidth = 16;
			wHeigh = 9;
			break;
		default:
			wWidth = 0;
			wHeigh = 0;
	}
	if (!m_cHdu2ChnMgrGrp.SetPlayScales( wWidth, wHeigh ))
	{
		Hdu2Print(HDU_LVL_WARNING,  "[ProcChangeChnCfg]SetPlayScales Failed!\n");
	}
	if (!m_cHdu2ChnMgrGrp.SetVidDecResizeMode(tHdu2ChnCfg.GetScalingMode()))
	{
		Hdu2Print(HDU_LVL_WARNING,  "[ProcChangeChnCfg]Chn(%d) SetVidDecResizeMode() Failed!\n",wChnIdx);
	}
}
/*====================================================================
函数  : ProcStartPlayReq
功能  : 开始播放
输入  : CServMsg&
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
2013/03/11	 4.7.2		  陈兵		   修改(HDU多画面支持)
=====================================================================*/
void  CHdu2Instance::ProcStartPlayReq( CServMsg &cServMsg )
{
    u8 byChnIdx = (u8)GetInsID() - 1; ////HDU通道Id
	u16 wIndex = 0;

    THduStartPlay tHduStartPlay = *(THduStartPlay*)cServMsg.GetMsgBody();
	wIndex += sizeof(THduStartPlay);
	Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Revc tHduStartPlay.GetMode(%d)!!!\n", tHduStartPlay.GetMode());

	TDoublePayload  tDoubleVidPayload = *(TDoublePayload*)(cServMsg.GetMsgBody() + wIndex);
	wIndex += sizeof(TDoublePayload);
	Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Revc VidRealPayLoad(%d) VidActivePayload(%d)!!!\n", 
		tDoubleVidPayload.GetRealPayLoad(), tDoubleVidPayload.GetActivePayload());

	TDoublePayload  tDoubleAudPayload = *(TDoublePayload*)(cServMsg.GetMsgBody() + wIndex);
	wIndex += sizeof(TDoublePayload);
	Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Revc AudRealPayLoad(%d) AudActivePayload(%d)!!!\n", 
		tDoubleAudPayload.GetRealPayLoad(), tDoubleAudPayload.GetActivePayload());

	THDUExCfgInfo tHDUExCfgInfo;
	tHDUExCfgInfo.m_byIdleChlShowMode = HDU_SHOW_DEFPIC_MODE;// 显示默认图片
	if (cServMsg.GetMsgBodyLen() > wIndex)
	{
		tHDUExCfgInfo = *(THDUExCfgInfo *)(cServMsg.GetMsgBody() + wIndex);
		wIndex += sizeof(THDUExCfgInfo);
		Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Revc tHDUExCfgInfo.m_byIdleChlShowMode(%d)!!!\n", tHDUExCfgInfo.m_byIdleChlShowMode);
	}

	TTransportAddr tAudRemoteAddr;
	memset(&tAudRemoteAddr,0,sizeof(TTransportAddr));
	tAudRemoteAddr = *(TTransportAddr *)(cServMsg.GetMsgBody() + wIndex);
	wIndex += sizeof(TTransportAddr);
	Hdu2Print(HDU_LVL_KEYSTATUS,  "[ProcStartPlayReq] Recv AudRemoteAddr<%x, %d>!\n", tAudRemoteAddr.GetIpAddr(), tAudRemoteAddr.GetPort());
	
	u8 byAudChnNum = *(u8 *)(cServMsg.GetMsgBody() + wIndex);
	m_cHdu2ChnMgrGrp.SetAudChnNum(byAudChnNum);
	wIndex += sizeof(u8);
	Hdu2Print(HDU_LVL_KEYSTATUS,  "[ProcStartPlayReq] Recv byAudChnNum<%d>!\n", byAudChnNum);

	u8 byHduMode = 0;		//HDU通道模式
	u8 byHduSubChnId = 0;	//HDU子通道Id
	if(cServMsg.GetMsgBodyLen() > wIndex)
	{
		// [2013/03/11 chenbing] 解析HDU子通道Id与通道模式
		byHduSubChnId = *(u8 *)(cServMsg.GetMsgBody() + wIndex);
		wIndex += sizeof(u8);
		byHduMode = *(u8 *)(cServMsg.GetMsgBody() + wIndex);
		wIndex += sizeof(u8);
		// 校验Hdu模式是否合法
		if ( g_cHdu2App.IsValidHduVmpMode(byHduMode) )
		{
			if (byHduMode == HDUCHN_MODE_ONE)
			{
				byHduSubChnId = 0;
			}
		}
		else
		{
			SendMsgToMcu(HDU_MCU_START_PLAY_NACK, cServMsg);
			return;
		}

		Hdu2Print(HDU_LVL_KEYSTATUS,  
				"[ProcStartPlayReq] Revc HduChnId(%d) HduMode(%d) HduSubChnId(%d) MediaMode(%d)!!!\n",
				 byChnIdx, byHduMode, byHduSubChnId, tHduStartPlay.GetMode());

		Hdu2Print(HDU_LVL_WARNING,  
				"[ProcStartPlayReq] NewHduVmpMode(%d) OldHduVmpMode(%d) !!!\n", byHduMode, g_cHdu2App.GetHduChnMode(byChnIdx));
		
		//切换的通道风格
		if ( !ChangeHduVmpMode(byChnIdx, byHduMode) )
		{
			Hdu2Print(HDU_LVL_WARNING,  
				"[ProcStartPlayReq] ChangeHduVmpMode Error !!!\n");
			return;
		}
	}

	u8 byMode = MODE_NONE;
	if (tHduStartPlay.GetMode() == HDU_OUTPUTMODE_AUDIO)
	{
		byMode = MODE_AUDIO;
	}
	else if (tHduStartPlay.GetMode() == HDU_OUTPUTMODE_VIDEO)
	{
		byMode = MODE_VIDEO;
	}
	else if (tHduStartPlay.GetMode() == HDU_OUTPUTMODE_BOTH)
	{
		byMode = MODE_BOTH;
	}
	else
	{
		Hdu2Print(HDU_LVL_WARNING,  "[ProcStartPlayReq]byMode(%d) error!!\n",byMode);
		return;
	}

	if ( MODE_VIDEO == byMode || MODE_BOTH == byMode )
	{
		m_cHdu2ChnMgrGrp.SetVidDoublePayload( tDoubleVidPayload, byHduSubChnId);
	}
    
    if( MODE_AUDIO == byMode || MODE_BOTH == byMode )
    {
		m_cHdu2ChnMgrGrp.SetAudDoublePayload(tDoubleAudPayload);
    }

	m_cHdu2ChnMgrGrp.SetVidPlayPolicy(tHDUExCfgInfo.m_byIdleChlShowMode);
	
	if (m_cHdu2ChnMgrGrp.GetAudChnNum() == 0)
	{
		m_cHdu2ChnMgrGrp.SetAudChnNum(1);
	}

	Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Revc IsNeedByPrs(%d) !!!\n", tHduStartPlay.IsNeedByPrs());

	TMediaEncrypt   tMediaEncrypt;
	u8 abyDecKey[64] = {0};
	s32 iLen = 0;
	tMediaEncrypt = tHduStartPlay.GetVideoEncrypt();
	tMediaEncrypt.GetEncryptKey(abyDecKey, &iLen);
	Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Revc EncryptMode(%d) EncKeyLen(%d) EncKey(%s)!!!\n",
	     	tMediaEncrypt.GetEncryptMode(), iLen, abyDecKey);

	m_cHdu2ChnMgrGrp.SetMediaEncrypt(&tMediaEncrypt);
    // 设置加密参数
    m_cHdu2ChnMgrGrp.SetDecryptKey(byMode, byHduSubChnId);
	m_cHdu2ChnMgrGrp.SetActivePT(byMode, TRUE, byHduSubChnId);
	//设丢包重传参数
	m_cHdu2ChnMgrGrp.SetNetRecvRsParam(byMode,tHduStartPlay.IsNeedByPrs(), byHduSubChnId);

	TLocalNetParam tNetParm;
	THduCfg tHduCfg = g_cHdu2App.GetHduCfg();


	u8 byDstChnnl = 0;
	Hdu2Print(HDU_LVL_KEYSTATUS, "tHduCfg.GetHduSubType(%d)\n", tHduCfg.GetHduSubType());
	if(tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2 || tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2_S)
	{
		// HDU2支持多画面1通道的接口端口起始为：42200
		byDstChnnl = (COMPOSECHAN(byChnIdx, byHduSubChnId))*PORTSPAN;
	}
	else
	{
		// 不支持四画面的1通道接口端口起始为：42010
		byDstChnnl = byChnIdx*PORTSPAN;
	}

    //视频
    if( MODE_BOTH  == byMode || MODE_VIDEO == byMode )
    {
		u32 dwRtcpBackIp;
		u16 wRtcpBackPort;
		tHduStartPlay.GetVidRtcpBackAddr(dwRtcpBackIp, wRtcpBackPort);
        memset(&tNetParm, 0, sizeof(tNetParm));

        tNetParm.m_tLocalNet.m_dwRTPAddr  = htonl(tHduCfg.dwLocalIP);
        tNetParm.m_tLocalNet.m_wRTPPort   = tHduCfg.wRcvStartPort + byDstChnnl;
        tNetParm.m_tLocalNet.m_dwRTCPAddr = htonl(tHduCfg.dwLocalIP);
        tNetParm.m_tLocalNet.m_wRTCPPort  = tHduCfg.wRcvStartPort + 1 + byDstChnnl;
        tNetParm.m_dwRtcpBackAddr = htonl(dwRtcpBackIp);
        tNetParm.m_wRtcpBackPort  = wRtcpBackPort;

		Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Video RTPAddr(%s) RTPPort(%d)\n",
			IpToStr(htonl(tNetParm.m_tLocalNet.m_dwRTPAddr)), tNetParm.m_tLocalNet.m_wRTPPort);
		Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Video RTCPAddr(%s) RTCPPort(%d)\n",
			IpToStr(htonl(tNetParm.m_tLocalNet.m_dwRTCPAddr)),	tNetParm.m_tLocalNet.m_wRTCPPort);
		Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Video RtcpBackAddr(%s) RtcpBackPort(%d)!!!\n",
			IpToStr(htonl(tNetParm.m_dwRtcpBackAddr)), tNetParm.m_wRtcpBackPort);

		m_cHdu2ChnMgrGrp.SetVidLocalNetParam(&tNetParm, byHduSubChnId);
    }

    // 音频
    if( MODE_BOTH  == byMode || MODE_AUDIO == byMode )
    {
        memset(&tNetParm, 0, sizeof(tNetParm));
        tNetParm.m_tLocalNet.m_dwRTPAddr  = htonl(tHduCfg.dwLocalIP);
        tNetParm.m_tLocalNet.m_wRTPPort   = tHduCfg.wRcvStartPort + 2 + byDstChnnl;
        tNetParm.m_tLocalNet.m_dwRTCPAddr = htonl(tHduCfg.dwLocalIP);
        tNetParm.m_tLocalNet.m_wRTCPPort  = tHduCfg.wRcvStartPort + 3 + byDstChnnl;
        tNetParm.m_dwRtcpBackAddr = htonl(tAudRemoteAddr.GetIpAddr());
        tNetParm.m_wRtcpBackPort  = tAudRemoteAddr.GetPort();

		Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Audio RTPAddr(%s) RTPPort(%d)\n",
			IpToStr(htonl(tNetParm.m_tLocalNet.m_dwRTPAddr)), tNetParm.m_tLocalNet.m_wRTPPort);
		Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Audio RTCPAddr(%s) RTCPPort(%d)\n",
			IpToStr(htonl(tNetParm.m_tLocalNet.m_dwRTCPAddr)),	tNetParm.m_tLocalNet.m_wRTCPPort);
		Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStartPlayReq] Audio RtcpBackAddr(%s) RtcpBackPort(%d)!!!\n",
			IpToStr(htonl(tNetParm.m_dwRtcpBackAddr)), tNetParm.m_wRtcpBackPort);

		m_cHdu2ChnMgrGrp.SetAudLocalNetParam(&tNetParm);
    }

    if (Hdu2ChnStartPlay( byMode, byHduSubChnId ))
    {
		// [2013/03/11 chenbing]  
        SendMsgToMcu(HDU_MCU_START_PLAY_ACK, cServMsg);
		SendChnNotify(FALSE, byHduSubChnId);
		m_cChnConfId = cServMsg.GetConfId();
		Hdu2Print(HDU_LVL_DETAIL,  "[ProcStartPlayReq] HduChnId(%d) HduSubChnId(%d) Start Succes!!!\n", byChnIdx, byHduSubChnId);
    }
    else
    {
        SendMsgToMcu(HDU_MCU_START_PLAY_NACK, cServMsg);
		Hdu2Print(HDU_LVL_ERROR,  "[ProcStartPlayReq] HduChnId(%d) HduSubChnId(%d) Start Faild!!!\n", byChnIdx, byHduSubChnId);
    }

	Hdu2Print(HDU_LVL_GENERAL, "Mcu_Hdu2 MsgLen: %d\n", wIndex);
}

/*====================================================================
函数  : ProcChangeHduVmpMode
功能  : 切换HDU通道模式
输入  : CServMsg&
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人       修改内容
2013/03/11	 4.7.2		  陈兵		   创建(HDU多画面支持)
=====================================================================*/
void CHdu2Instance::ProcChangeHduVmpMode( CServMsg &cServMsg )
{
	u8 byHduEqpId = cServMsg.GetEqpId();
	u8 byHduChnId = cServMsg.GetChnIndex();
	u8 byHduVmpMode = *(u8 *)cServMsg.GetMsgBody();

	Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcChangeHduVmpMode] Revc HduMode(%d) HduId(%d) HduChnId(%d)!!!\n",
				byHduVmpMode, byHduEqpId, byHduChnId);

	if ( ChangeHduVmpMode(byHduChnId, byHduVmpMode) )
	{
		SendMsgToMcu(HDU_MCU_CHGHDUVMPMODE_ACK, cServMsg);
	}
	else
	{
		SendMsgToMcu(HDU_MCU_CHGHDUVMPMODE_NACK, cServMsg);
	}

	u8 bySubId = 0;
	for (; bySubId < HDU_MODEFOUR_MAX_SUBCHNNUM; bySubId++)
	{
		if (MODE_NONE != m_cHdu2ChnMgrGrp.GetMode(bySubId))
		{
			break;
		}
	}
	
	if (bySubId >= HDU_MODEFOUR_MAX_SUBCHNNUM)
	{
		//恢复到通道的初始态
		NextState((u32)emREADY);
	}
}

/*====================================================================
函数  : ChangeHduVmpMode
功能  : 切换HDU通道模式
输入  : 
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人       修改内容
2013/05/23	 4.7.2		  陈兵		   创建(HDU多画面支持)
=====================================================================*/
BOOL32 CHdu2Instance::ChangeHduVmpMode(const u8 byHduChnId, const u8 byHduVmpMode)
{
	// 校验Hdu模式是否合法
	if ( g_cHdu2App.IsValidHduVmpMode(byHduVmpMode) )
	{
		//需要切换的通道与当前通道模式相同
		if ( byHduVmpMode == g_cHdu2App.GetHduChnMode(byHduChnId) )
		{
			Hdu2Print(HDU_LVL_WARNING,  
				"[ChangeHduVmpMode] NewHduVmpMode and OldHduVmpMode is same!!!\n");
			return TRUE;
		}
		else
		{
			//另一通道已经是四风格
			if ( HDUCHN_MODE_FOUR == g_cHdu2App.GetHduChnMode(byHduChnId, TRUE)
				&& HDUCHN_MODE_FOUR ==  byHduVmpMode)
			{
				Hdu2Print(HDU_LVL_WARNING,  
					"[ChangeHduVmpMode] Other HduChnnl already is HDUCHN_MODE_FOUR!!!\n");
				return FALSE;
			}
			else
			{
				// 从一风格切换到四风格 或 从四风格切换到一风格
				if (  (HDUCHN_MODE_ONE == g_cHdu2App.GetHduChnMode(byHduChnId) && byHduVmpMode == HDUCHN_MODE_FOUR)
					|| (HDUCHN_MODE_FOUR == g_cHdu2App.GetHduChnMode(byHduChnId)&& byHduVmpMode == HDUCHN_MODE_ONE)
					)
				{
					// 切换模式, 设置视频画中画
					if( m_cHdu2ChnMgrGrp.SetVidPIPParam( byHduVmpMode ) )
					{
						g_cHdu2App.SetHduChnMode( byHduChnId, byHduVmpMode );
					}
					else
					{
						Hdu2Print(HDU_LVL_WARNING,  
							"[ChangeHduVmpMode]HDUVMPMODE change Failed!!!\n");
						return FALSE;
					}
				}
			}
		}
	}
	else
	{
		Hdu2Print(HDU_LVL_ERROR,  
			"[ChangeHduVmpMode]byHduVmpMode or HduChnId IsValid!!!\n");
		return FALSE;
	}
	return TRUE;
}

/*====================================================================
函数  : ProcStopPlayReq
功能  : 停止播放
输入  : CServMsg&
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录 ：
日  期       版本        修改人      修改内容
2013/03/11	 4.7.2		  陈兵		   创建(HDU多画面支持)
====================================================================*/
void  CHdu2Instance::ProcStopPlayReq( CServMsg& cServMsg)   		  
{
	u8 byHduChnId = cServMsg.GetChnIndex();
	u8 byHduSubChnId = 0;
	u8 byMode = MODE_BOTH;


	// [2013/03/12 chenbing]
	// 批量轮询时，会先开启HDU同时切换风格
	// 再停止子通道，这时外设风格已经切换，所以，不能判断风格
	if ( cServMsg.GetMsgBodyLen() == (sizeof(TEqp) + sizeof(u8) + sizeof(u8)) )
	{
		Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcStopPlayReq] Revc SerMsgBodyLen(%d) !!!\n", cServMsg.GetMsgBodyLen());
		byHduSubChnId = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TEqp));
		byMode = *(u8 *)(cServMsg.GetMsgBody() + sizeof(TEqp) + sizeof(u8));
	}

	Hdu2Print(HDU_LVL_KEYSTATUS,  
		"[ProcStopPlayReq] Revc OldMediaMode(%d) NewMediaMode(%d) HduChnId(%d) HduSubChnId(%d) HduMode(%d)!!!\n",
				m_cHdu2ChnMgrGrp.GetMode(byHduSubChnId), byMode, byHduChnId, byHduSubChnId, g_cHdu2App.GetHduChnMode(byHduChnId));

	if (Hdu2ChnStopPlay(byMode, byHduSubChnId))
	{
		SendMsgToMcu(HDU_MCU_STOP_PLAY_ACK, cServMsg);
		SendChnNotify(FALSE, byHduSubChnId);
		m_cChnConfId.SetNull();
		Hdu2Print(HDU_LVL_DETAIL,  "[ProcStopPlayReq] HduChnId(%d) HduSubChnId(%d) Stop Success!!!\n", byHduChnId, byHduSubChnId);
	}
	else
	{
		SendMsgToMcu(HDU_MCU_STOP_PLAY_NACK, cServMsg);
		Hdu2Print(HDU_LVL_ERROR,  "[ProcStopPlayReq] HduChnId(%d) HduSubChnId(%d) Stop Faild!!!\n", byHduChnId, byHduSubChnId);
	}
}

/*====================================================================
函数  : ProcHduSetAudioPayLoad
功能  : 设置音频载荷
输入  : CServMsg&
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录 ：
日  期       版本        修改人      修改内容
2013/03/11	 4.7.2		  陈兵		   创建
====================================================================*/
void CHdu2Instance::ProcHduSetAudioPayLoad(CServMsg& cServMsg)
{
	u8 byAudioNum = *(u8*)(cServMsg.GetMsgBody());
	TDoublePayload  tDoubleAudPayload = *(TDoublePayload*)(cServMsg.GetMsgBody() + sizeof(u8));

	Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcHduSetAudioPayLoad] ChnId(%d) CurrMediaMode(%d) AudioNum(%d) ActivePayload(%d) RealPayLoad(%d)\n",
	GetInsID() - 1,	m_cHdu2ChnMgrGrp.GetMode(), byAudioNum, tDoubleAudPayload.GetActivePayload(), tDoubleAudPayload.GetRealPayLoad());

	if ( MODE_BOTH == m_cHdu2ChnMgrGrp.GetMode() 
		|| MODE_AUDIO == m_cHdu2ChnMgrGrp.GetMode())
	{
		Hdu2Print(HDU_LVL_KEYSTATUS, "[ProcHduSetAudioPayLoad] Rfresh AudioPayLoad Success!!!\n");

		Hdu2ChnStopPlay( MODE_AUDIO );

		m_cHdu2ChnMgrGrp.SetAudDoublePayload(tDoubleAudPayload);
	    m_cHdu2ChnMgrGrp.SetActivePT(MODE_AUDIO);
		m_cHdu2ChnMgrGrp.SetAudChnNum(byAudioNum);
		Hdu2ChnStartPlay( MODE_AUDIO );
	}
}

/*====================================================================
函数  : ProcOspDisconnect
功能  : Hdu断链处理
输入  : void
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2013/03/11   4.7.2       陈兵        修改(HDU多画面)
====================================================================*/
void CHdu2Instance::ProcDisConnect(void)
{
	u8 byHduChnId = GetInsID() - 1;
	
	if ( HDUCHN_MODE_FOUR == g_cHdu2App.GetHduChnMode(byHduChnId) )
	{
		for (u8 bySubChnId=0; bySubChnId<HDU_MODEFOUR_MAX_SUBCHNNUM; bySubChnId++)
		{
			Hdu2ChnStopPlay( MODE_BOTH, bySubChnId );
		}

		// Mcu断链,切换模式, 设置视频画中画为一风格
		if( m_cHdu2ChnMgrGrp.SetVidPIPParam( HDUCHN_MODE_ONE ) )
		{
			g_cHdu2App.SetHduChnMode( byHduChnId, HDUCHN_MODE_ONE );
			Hdu2Print(HDU_LVL_KEYSTATUS, "ChangeHduVmpMode Success!!!\n");
		}
		else
		{
			Hdu2Print(HDU_LVL_KEYSTATUS, "ChangeHduVmpMode Falid!!!\n");
		}
	}
	else if ( HDUCHN_MODE_ONE == g_cHdu2App.GetHduChnMode(byHduChnId) )
	{
		Hdu2ChnStopPlay( MODE_BOTH );
	}

	NextState((u32)emINIT);
	Hdu2Print(HDU_LVL_KEYSTATUS,  "[InstanceEntry]OSP_DISCONNECT Chn(%d) State Change: RUNNING--->READY!\n",GetInsID() - 1);
}

/*====================================================================
函数  : ProcTimerNeedIFrame
功能  : Hdu请求Mcu发关键帧
输入  : void
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇      创建
2013/03/11   4.7.2       陈兵        修改(HDU多画面)
====================================================================*/
void  CHdu2Instance::ProcTimerNeedIFrame( )                  
{
	TKdvDecStatis tDecChannelStatis;
	u8 byMaxChnNum = 1;

	// 四风格
	if ( HDUCHN_MODE_FOUR == g_cHdu2App.GetHduChnMode( GetInsID() - 1) )
	{
		byMaxChnNum = HDU_MODEFOUR_MAX_SUBCHNNUM;
	}

	Hdu2Print(HDU_LVL_GENERAL,  
		"[ProcTimerNeedIFrame] Revc HduMode(%d) MaxChnNum(%d)!!!\n",
				g_cHdu2App.GetHduChnMode( GetInsID() - 1), byMaxChnNum);

	// [2013/03/11 chenbing] 循环检测各个子通道是否需要关键帧
	for (u8 byHduSubChnId = 0; byHduSubChnId < byMaxChnNum; byHduSubChnId++)
	{
		if( m_cHdu2ChnMgrGrp.GetMode(byHduSubChnId) == MODE_VIDEO || m_cHdu2ChnMgrGrp.GetMode(byHduSubChnId) == MODE_BOTH)
		{ 
			memset( &tDecChannelStatis, 0, sizeof(TKdvDecStatis) );
			if (!m_cHdu2ChnMgrGrp.GetDecoderStatis(MODE_VIDEO, tDecChannelStatis, byHduSubChnId))
			{
				Hdu2Print(HDU_LVL_FRAME,  "[ProcTimerNeedIFrame] GetVidDecoderStatis Failed!\n");
			}
			if ( tDecChannelStatis.m_bVidCompellingIFrm )
			{	
				THduCfg tHduCfg = g_cHdu2App.GetHduCfg();
				CServMsg cServMsg;
				cServMsg.SetConfId( m_cChnConfId );
				cServMsg.SetChnIndex( GetInsID() - 1 );
				cServMsg.SetSrcSsnId( tHduCfg.byEqpId );
				
				// 四风格的追加子通道 
				if ( HDU_MODEFOUR_MAX_SUBCHNNUM == byMaxChnNum )
				{
					u8 bySubChnId = byHduSubChnId;
					cServMsg.SetMsgBody((u8*)&bySubChnId, sizeof(u8));
				}
				
				SendMsgToMcu( HDU_MCU_NEEDIFRAME_CMD, cServMsg );
 				Hdu2Print(HDU_LVL_FRAME,  "[ProcTimerNeedIFrame]HduChannel(%d) HduSubChnId(%d) request iframe!!\n",
 					GetInsID() - 1, byHduSubChnId);
			}
		}
	}

	SetTimer( EV_HDU_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL );	
}

/*====================================================================
函数  : SendMsgToMcu
功能  : 向MCU发送消息通用函数
输入  : u16 -->消息号, CServMsg&
输出  : 无
返回  : BOOL32
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇      创建
====================================================================*/
void CHdu2Instance::SendMsgToMcu( u16 wEvent, CServMsg& cServMsg ) 
{
	if (GetInsID() != CInstance::DAEMON)
    {
        cServMsg.SetChnIndex((u8)GetInsID() - 1);
    }
	u32 dwMcuNodeA = g_cHdu2App.GetMcuNode();
	u32 dwMcuNodeB = g_cHdu2App.GetMcuNodeB();
    if (OspIsValidTcpNode(dwMcuNodeA)) 
	{
		post(g_cHdu2App.GetMcuIId(), wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), dwMcuNodeA);
		Hdu2Print(HDU_LVL_GENERAL,  "[SendMsgToMcu]Send Message %u(%s) to Mcu A\n",wEvent, ::OspEventDesc(wEvent));
	}
	else
	{
		Hdu2Print(HDU_LVL_GENERAL,  "[SendMsgToMcu]Send Message %u(%s) to Mcu A(Node:%d) Failed\n",wEvent, ::OspEventDesc(wEvent),dwMcuNodeA);
	}
    if(OspIsValidTcpNode(dwMcuNodeB))
    {
		post(g_cHdu2App.GetMcuIIdB(), wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), dwMcuNodeB);
		Hdu2Print(HDU_LVL_GENERAL,  "[SendMsgToMcu]Send Message %u(%s) to Mcu B\n",wEvent, ::OspEventDesc(wEvent));
    }
    else
    {
		Hdu2Print(HDU_LVL_GENERAL,  "[SendMsgToMcu]Send Message %u(%s) to Mcu B(Node:%d) Failed\n",wEvent, ::OspEventDesc(wEvent),dwMcuNodeB);
    }
}
/*====================================================================
函数  : SendChnNotif
功能  : 状态通知
输入  : void
输出  : 无
返回  : void
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
2013/03/11   4.7.2       陈兵          修改(HDU多画面支持)
====================================================================*/
void   CHdu2Instance::SendChnNotify( BOOL32 bFirstNotif/* = FALSE*/, u8 byHduSubChnId /* = 0*/)                                       
{
	 // [2013/03/11 chenbing] 
	 // 目前4.7.2主控侧已经不对此Notif处理
	 // 但为了兼容老的Mcu,此处消息仍需保留
	 u8 byChnIdx = (u8)GetInsID() - 1;
	 CServMsg cServMsg;
	 cServMsg.SetConfId(m_cChnConfId);
	 TEqp tEqp;
	 THduCfg tHduCfg = g_cHdu2App.GetHduCfg();

	// 兼容4.7.2HDU2模拟HDU注册到老版本MCU[6/3/2013 chendaiwei]
	 u8 byEqpType = GetOldVersionEqpTypeBySubType(tHduCfg.GetHduSubType());
	 
	 tEqp.SetEqpType( byEqpType);
	 tEqp.SetEqpId( tHduCfg.GetEqpId() );
	 tEqp.SetMcuId( tHduCfg.GetMcuId() );
	 THduChnStatus tHduChnStatus;
	 tHduChnStatus.SetEqp( tEqp );
	 tHduChnStatus.SetChnIdx(byChnIdx);
	 tHduChnStatus.SetStatus((u8)CurState());  
	 tHduChnStatus.SetVolume( m_cHdu2ChnMgrGrp.GetVolume() );
	 tHduChnStatus.SetIsMute( m_cHdu2ChnMgrGrp.GetIsMute() );
	 cServMsg.SetMsgBody( (u8*)&tHduChnStatus, sizeof(THduChnStatus)  );
	 u8 bFirstNotifTmp = bFirstNotif ? 1:0;
	 cServMsg.CatMsgBody( (u8*)&bFirstNotifTmp, sizeof(bFirstNotifTmp));
	 cServMsg.CatMsgBody( (u8*)&byHduSubChnId, sizeof(u8));
	 

	 SendMsgToMcu(HDU_MCU_CHNNLSTATUS_NOTIF, cServMsg);
	 Hdu2Print(HDU_LVL_DETAIL,  
		 "[SendChnNotif]Send HDU_MCU_CHNNLSTATUS_NOTIF To Mcu HduSubChnId(%d) CutState(%d)\n", byHduSubChnId, (u8)CurState());
 	 return;
}

/*====================================================================
函数  : StatusNotify
功能  : HDU状态通知
输入  : void
输出  : 无
返回  : void
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇        创建
====================================================================*/
void   CHdu2Instance::StatusNotify(void)
{
	TPeriEqpStatus tEqpStatus;
	memset( &tEqpStatus, 0, sizeof(tEqpStatus) );
	THduCfg tHduCfg = g_cHdu2App.GetHduCfg();

	// 兼容4.7.2HDU2模拟HDU注册到老版本MCU[6/3/2013 chendaiwei]
	u8 byEqpType = GetOldVersionEqpTypeBySubType(tHduCfg.GetHduSubType());

	tEqpStatus.SetMcuEqp((u8)tHduCfg.wMcuId, tHduCfg.byEqpId, byEqpType);	
	tEqpStatus.m_byOnline = (CurState() >= (u32)emREADY) ? 1 : 0;
	tEqpStatus.SetAlias(tHduCfg.achAlias);
	if (g_cBrdAgentApp.GetBrdPosition().byBrdID == BRD_TYPE_HDU)
	{
		tEqpStatus.m_tStatus.tHdu.byOutputMode = HDU_OUTPUTMODE_BOTH;
	}
	
	for ( u8 byChnlIdx=0; byChnlIdx<MAXNUM_HDU_CHANNEL; byChnlIdx++ )
	{
		// [2013/03/11 chenbing] 设置当前通道支持四风格 
		if(tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2 || tHduCfg.GetHduSubType() == HDU_SUBTYPE_HDU2_S)
		{
			tEqpStatus.m_tStatus.tHdu.SetChnMaxVmpMode(byChnlIdx, HDU_MODEFOUR_MAX_SUBCHNNUM);
			Hdu2Print(HDU_LVL_DETAIL,  "[StatusNotify]MaxSubChnNum(%d)\n", HDU_MODEFOUR_MAX_SUBCHNNUM);
		}
		tEqpStatus.m_tStatus.tHdu.atHduChnStatus[byChnlIdx].SetVolume( HDU_VOLUME_DEFAULT );
	}

	CServMsg cServMsg;
	cServMsg.SetMsgBody((u8*)&tEqpStatus, sizeof(tEqpStatus));
 	SendMsgToMcu(HDU_MCU_STATUS_NOTIF, cServMsg);
	Hdu2Print(HDU_LVL_DETAIL,  "[StatusNotify]Send HDU_MCU_STATUS_NOTIF To Mcu\n");
	return;
}
/* ==================================================================
函数  : Hdu2ChnPlay
功能  : 通道播放
输入  : 无
输出  : 无
返回  : void
注    :
---------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人      修改内容
2011/11/28  4.7         田志勇        创建
2013/03/11	4.7.2		陈兵		  修改(HDU多画面支持)
===================================================================== */
BOOL32 CHdu2Instance::Hdu2ChnStartPlay( u8 byMode /* = MODE_BOTH */, u8 byHduSubChnId)
{
	// [2013/03/11 chenbing] 遍历子通道是否存在模式
	// 需要提前判定
	u8 bySubChnId = 0;
	for (bySubChnId=0; bySubChnId<HDU_MODEFOUR_MAX_SUBCHNNUM; bySubChnId++)
	{
		Hdu2Print(HDU_LVL_DETAIL,  
			"[ProcStartPlayReq] Revc MediaMode(%d) HduSubChnId(%d)!!!\n",
				byMode, bySubChnId);
		if( MODE_VIDEO == m_cHdu2ChnMgrGrp.GetMode(bySubChnId)
		 ||	MODE_BOTH == m_cHdu2ChnMgrGrp.GetMode(bySubChnId)
		  )
		{
			break;
		}
	}

	if (MODE_NONE == byMode )
	{
		Hdu2Print(HDU_LVL_ERROR,  "[Hdu2ChnStartPlay]GetMode() == MODE_NONE(%d)\n",byMode);
		return FALSE;
	}

    if(!m_cHdu2ChnMgrGrp.StartNetRecv(byMode, byHduSubChnId))
    {
        Hdu2Print(HDU_LVL_ERROR, "[Hdu2ChnStartPlay]StartNetRecv Faileded MediaMode(%d) HduSubChnId(%d)!!!\n",
					byMode, byHduSubChnId);
        return FALSE;
    }

	if(!m_cHdu2ChnMgrGrp.StartDecode(byMode))
	{
		Hdu2Print(HDU_LVL_ERROR,  "[Hdu2ChnStartPlay]StartDecode Faileded MediaMode(%d) HduSubChnId(%d)!!!\n",
					byMode, byHduSubChnId);
		return FALSE;
	}

	//当前子通道中没有视频, 当前开启为视频或音视频, 设置关键帧定时器
	if ( bySubChnId >= HDU_MODEFOUR_MAX_SUBCHNNUM
		&& (MODE_VIDEO == byMode || MODE_BOTH == byMode ))
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[Hdu2ChnStartPlay]All SubChnl is Null, SetTimer EV_HDU_NEEDIFRAME_TIMER!!!\n");
		SetTimer(EV_HDU_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL);
		NextState((u32)emRUNNING);
	}
	else
	{
		Hdu2Print(HDU_LVL_DETAIL,  "[Hdu2ChnStartPlay]SubChnl(%d) OldMode(%d) NewMode(%d) !!!\n",
			bySubChnId, m_cHdu2ChnMgrGrp.GetMode(bySubChnId),  byMode);
	}

	u8 byOldMode = m_cHdu2ChnMgrGrp.GetMode(byHduSubChnId);
	u8 byCurMode = MODE_NONE;
	if (MODE_VIDEO == byOldMode && (MODE_BOTH == byMode || MODE_AUDIO == byMode))
	{
		byCurMode = MODE_BOTH;
	}
	else if (MODE_AUDIO == byOldMode && (MODE_BOTH == byMode || MODE_VIDEO == byMode))
	{
		byCurMode = MODE_BOTH;
	}
	else 
	{
		byCurMode = byMode;
	}

	m_cHdu2ChnMgrGrp.SetMode(byCurMode, byHduSubChnId);
    return TRUE;
}

/* ==================================================================
函数  : Hdu2ChnStopPlay
功能  : 通道停止
输入  : 无
输出  : 无
返回  : void
注    : 添加u8 byIsClearAllSubChn, 判断是否清除子通道
---------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2011/11/28   4.7         田志勇      创建
2013/03/11   4.7.2       陈兵        修改(HDU支持多画面)
===================================================================== */
BOOL32 CHdu2Instance::Hdu2ChnStopPlay( u8 byMode /* = MODE_BOTH */, u8 byHduSubChnId)						                            
{
	if (byMode == MODE_NONE)
	{
		Hdu2Print(HDU_LVL_ERROR,  "[Hdu2ChnStopPlay] byMode(%d)\n",byMode);
		return FALSE;
	}
	
	u8 byOldMode = m_cHdu2ChnMgrGrp.GetMode(byHduSubChnId);
	u8 byCurMode = MODE_NONE;

	if (MODE_BOTH == byOldMode && MODE_VIDEO == byMode)
	{
		byCurMode = MODE_AUDIO;
	}
	else if (MODE_BOTH == byOldMode && MODE_AUDIO == byMode)
	{
		byCurMode = MODE_VIDEO;
	}
	else
	{
		byCurMode = MODE_NONE;
	}

	m_cHdu2ChnMgrGrp.SetMode(byCurMode, byHduSubChnId);
	Hdu2Print(HDU_LVL_DETAIL, "[Hdu2ChnStopPlay] OldMediaMode: %d  StopMediaMode:%d  AfterStopMediaMode(%d) HduSubChnId: %d\n",
		byOldMode, byMode, byCurMode, byHduSubChnId);

	
	if(!m_cHdu2ChnMgrGrp.StopNetRecv(byMode, byHduSubChnId))
	{
		Hdu2Print(HDU_LVL_ERROR,  "[Hdu2ChnStopPlay]StopNetRecv Faileded MediaMode(%d) HduSubChnId(%d)!!!\n",
			byMode, byHduSubChnId);
		return FALSE;
	}
	
	if (MODE_BOTH == byMode || MODE_VIDEO == byMode )
	{
		// [2013/03/11 chenbing] 遍历子通道StopDecode会将byMode设置到m_cHdu2ChnMgrGrp中
		u8 bySubChnId = 0;
		for (bySubChnId=0; bySubChnId<HDU_MODEFOUR_MAX_SUBCHNNUM; bySubChnId++)
		{
			//当前模式中有视频,或音视频,则不能删除请求关键帧定时器
			if(MODE_VIDEO == m_cHdu2ChnMgrGrp.GetMode(bySubChnId)
				|| MODE_BOTH == m_cHdu2ChnMgrGrp.GetMode(bySubChnId)
				)
			{
				break;
			}
		}
		//子通道中没有视频, 取消请求关键帧定时器,关闭解码器
		if ( bySubChnId >= HDU_MODEFOUR_MAX_SUBCHNNUM )
		{
			if(!m_cHdu2ChnMgrGrp.StopDecode(byMode))
			{
				Hdu2Print(HDU_LVL_ERROR,  "[Hdu2ChnStopPlay]StopDecode Faileded MediaMode(%d) HduSubChnId(%d)!!!\n",
					byMode, byHduSubChnId);
				return FALSE;
			}
			Hdu2Print(HDU_LVL_DETAIL,  "[Hdu2ChnStopPlay]All SubChnl MediaMode is Null, KillTimer EV_HDU_NEEDIFRAME_TIMER!!!\n");
			KillTimer(EV_HDU_NEEDIFRAME_TIMER);
			NextState((u32)emREADY);
		}
		else
		{
			Hdu2Print(HDU_LVL_DETAIL,  "[Hdu2ChnStopPlay]SubChnl(%d) OldMode(%d) NewMode(%d) !!!\n",
				bySubChnId, m_cHdu2ChnMgrGrp.GetMode(bySubChnId), byMode);
		}
	}

	if (MODE_AUDIO == byMode)
	{
		if(!m_cHdu2ChnMgrGrp.StopDecode(byMode))
		{
			Hdu2Print(HDU_LVL_ERROR,  "[Hdu2ChnStopPlay]StopDecode Faileded MediaMode(%d) HduSubChnId(%d)!!!\n",
				byMode, byHduSubChnId);
			return FALSE;
		}
	}

    return TRUE;
}

/*====================================================================
函数  : DaemonProcChangePlayPolicy
功能  : 空闲通道显示策略改变处理
输入  : CMessage* const
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2012/09/18   4.7         田志勇        创建
====================================================================*/
void  CHdu2Instance::DaemonProcChangePlayPolicy( CMessage* const pMsg ) 
{
	if ( NULL == pMsg)
	{
		Hdu2Print(HDU_LVL_WARNING,  "[DaemonProcChangePlayPolicy]NULL == pMsg,So Return!\n");
		return;
	}
	u8 byIdleChnPlayPolicy = *(u8*)pMsg->content;
    for (u8 byLoop=0; byLoop<MAXNUM_HDU_CHANNEL; byLoop++)
    {
		post(MAKEIID(GetAppID(), byLoop+1), MCU_HDU_CHANGEPLAYPOLICY_NOTIF, &byIdleChnPlayPolicy,sizeof(u8));
	}
}

/*====================================================================
函数  : ProcShowMode
功能  : 查寻当前通道模式
输入  : CMessage* const
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2013/03/11   4.7.2       陈兵        修改(HDU多画面支持)
====================================================================*/
void CHdu2Instance::ProcShowMode()
{
	OspPrintf(TRUE, FALSE, "\n^^^^^^^^^^^^^^Current HduChnnl (%d)^^^^^^^^^^^^^^\n", GetInsID()-1);
	OspPrintf(TRUE, FALSE, "SubChnnl     Mode\n");
	for (u8 bySubChnId=0; bySubChnId<HDU_MODEFOUR_MAX_SUBCHNNUM; bySubChnId++)
	{
		OspPrintf(TRUE, FALSE, "%4d %11d\n", bySubChnId,  m_cHdu2ChnMgrGrp.GetMode(bySubChnId)	);
	}
}

/*====================================================================
函数  : ProcCLearMode
功能  : 清除当前通道模式
输入  : CMessage* const
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2013/03/11   4.7.2       陈兵        修改(HDU多画面支持)
====================================================================*/
void CHdu2Instance::ProcClearMode()
{
	for (u8 bySubChnId=0; bySubChnId<HDU_MODEFOUR_MAX_SUBCHNNUM; bySubChnId++)
	{
		m_cHdu2ChnMgrGrp.SetMode(MODE_NONE, bySubChnId);
	}
}

/*====================================================================
函数  : ProcChangePlayPolicy
功能  : 空闲通道显示策略改变处理
输入  : CMessage* const
输出  : 无
返回  : 无
注    :
----------------------------------------------------------------------
修改记录    ：
日  期       版本        修改人      修改内容
2012/09/18   4.7         田志勇        创建
====================================================================*/
void  CHdu2Instance::ProcChangePlayPolicy( CMessage* const pMsg ) 
{
	u8 byIdleChnPlayPolicy = *(u8*)pMsg->content;
	m_cHdu2ChnMgrGrp.SetVidPlayPolicy(byIdleChnPlayPolicy);
	Hdu2Print(HDU_LVL_KEYSTATUS,  "[ProcChangePlayPolicy]Chn(%d)  m_cVidDecoder.SetVidPlayPolicy(%d) Successed!\n",GetInsID(),byIdleChnPlayPolicy);
}

u8 CHdu2Instance::GetOldVersionEqpTypeBySubType ( u8 byHduEqpSubType )
{
	// 兼容4.7.2HDU2模拟HDU注册到老版本MCU[6/3/2013 chendaiwei]
	u8 byEqpType = EQP_TYPE_HDU;
	if(byHduEqpSubType == HDU_SUBTYPE_HDU_H)
	{
		byEqpType = 13;//原EQP_TYPE_HDU_H
	}
	else if ( byHduEqpSubType == HDU_SUBTYPE_HDU_L)
	{
		byEqpType = 14;//原EQP_TYPE_HDU_L
	}
	else if ( byHduEqpSubType == HDU_SUBTYPE_HDU2 )
	{
		byEqpType = 15;//原定义，兼容老版本HDU版本卡#define EQP_TYPE_HDU2 15
	}
	else if ( byHduEqpSubType == HDU_SUBTYPE_HDU2_L )
	{
		byEqpType = 12;//原定义，兼容老版本HDU版本卡#define EQP_TYPE_HDU2_L 12
	}

	return byEqpType;
}

/* -------------------CHdu2Instance类的实现  end---------------------*/
/* ---------------------CHdu2Data类的实现  start---------------------*/

CHdu2Data::CHdu2Data()
{
	memset(this, 0, sizeof(CHdu2Data));
}
CHdu2Data::~CHdu2Data()
{
}
/* ---------------------CHdu2Data类的实现  end-----------------------*/
//END OF FILE

