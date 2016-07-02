/*****************************************************************************
   模块名      : mpulib
   文件名      : mpuutility.cpp
   相关文件    : mpuutility.h
   文件实现功能: 工具/通用 类/函数 实现
   作者        : zhangbq
   版本        : V4.5  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期       版本        修改人      修改内容
   2009/3/14    4.6         张宝卿      创建
******************************************************************************/
#include "mpuutility.h"
#include "mpustruct.h"
#include "mpuinst.h"


s32 g_nmpubaslog = 1;

/*=============================================================================
    函 数 名： StrOfIP
    功    能： 把整型IP地址转成字符串
    算法实现： 
    全局变量： 
    参    数： u32 dwIP   主机序IP地址
    返 回 值： char * 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/02/28  3.6			万春雷                  创建
    2009/03/14  4.6         张宝卿                  从mcu移植过来
=============================================================================*/
s8 * StrOfIP( u32 dwIP )
{
    dwIP = htonl(dwIP);
	static s8 strIP[17];  
	u8 *p = (u8 *)&dwIP;
	sprintf(strIP,"%d.%d.%d.%d%c",p[0],p[1],p[2],p[3],0);
	return strIP;
}


/*====================================================================
	函数  : ipStr
	功能  : 取得IP地址字符串
	输入  : dwIp: IP地址
	输出  : 无
	返回  : IP地址对应的字符串
	注    : 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
s8* ipStr(u32 dwIP)
{
	static s8 achIPStr[4][32]={0};
	static u8 byIdx = 0;
    struct in_addr in;

	byIdx++;
	byIdx %= sizeof(achIPStr)/sizeof(achIPStr[0]);
    in.s_addr = dwIP;
	memset(achIPStr[byIdx], 0, sizeof(achIPStr[0]));
	
#ifdef _MSC_VER
	strncpy(achIPStr[byIdx],inet_ntoa(in),sizeof(achIPStr[0]));
#endif //_MSC_VER

#ifdef _LINUX_
	strncpy(achIPStr[byIdx],inet_ntoa(in),sizeof(achIPStr[0]));
#endif //_LINUX_
	
#ifdef _VXWORKS_
	inet_ntoa_b(in, achIPStr[byIdx]);
#endif //_VXWORKS_
	
	achIPStr[byIdx][31] = '\0'; // 字符串结束标志
	return achIPStr[byIdx];
}

/*====================================================================
	函数  : mpulog
	功能  : mpulog
	输入  : u8 byLevel
	输出  : 无
	返回  : 
	注    : 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2009/03/14  4.6         张宝卿        打印级别支持
====================================================================*/
/*lint -save -e628*/
/*lint -save -e526  -e438 -e530*/
void mpulog( u8 byLevel, s8* pszFmt, ... )
{
    s8 * pchLevelName[] = {(s8 *)"[CRIT]", (s8 *)"[WARN]", (s8 *)"[INFO]", "[DETAIL]", ""};

    s8 achPrintBuf[512];
    s32  nBufLen = 0;
    va_list argptr;
    if ( g_nmpubaslog >= byLevel )
    {		  
        nBufLen = sprintf( achPrintBuf, "[Mpu] %s: ", pchLevelName[byLevel] );         
        va_start( argptr, pszFmt );
        vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
        va_end( argptr ); 
        OspPrintf( TRUE, FALSE, achPrintBuf ); 
    }
}
/*lint -restore*/
/*====================================================================
	函数  : mpulog
	功能  : mpulog
	输入  : 
	输出  : 无
	返回  : 
	注    : 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    2009/03/14  4.6         张宝卿        打印级别支持
====================================================================*/
/*lint -save -e438 -e530*/
void mpulogall( s8* pszFmt, ... )
{
    if( g_nmpubaslog >= MPU_DETAIL )
    {
        s8 achPrintBuf[512];
        s32  nBufLen = 0;
        va_list argptr;

        nBufLen = sprintf( achPrintBuf, "[MpuDbg]: " );         
        va_start( argptr, pszFmt );
        vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
        va_end( argptr ); 
        OspPrintf( TRUE, FALSE, achPrintBuf ); 
    }
    return;
}
/*lint -restore*/
/*==============================================================================
函数名    :  ConvertVmpStyle2ChnnlNum
功能      :  VMP style -> 通道数目
算法实现  :  
参数说明  :  
返回值说明:  u8
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-5-9	4.6				薛亮							create
==============================================================================*/
u8 ConvertVmpStyle2ChnnlNum(u8 byVmpStyle)
{
	switch (byVmpStyle)
	{
	case VMP_STYLE_ONE:
		return 1;

	case VMP_STYLE_VTWO:
	case VMP_STYLE_HTWO:
		return 2;

	case VMP_STYLE_THREE:
		return 3;

	case VMP_STYLE_FOUR:
	case VMP_STYLE_SPECFOUR:
		return 4;

	case VMP_STYLE_SIX:
		return 6;

	case VMP_STYLE_SEVEN:
		return 7;

	case VMP_STYLE_EIGHT:
		return 8;

	case VMP_STYLE_NINE:
		return 9;

	case VMP_STYLE_TEN:
		return 10;

	case VMP_STYLE_THIRTEEN:
		return 13;

	case VMP_STYLE_SIXTEEN:
		return 16;

	case VMP_STYLE_TWENTY:
		return 20;

	default:
		return 0;
	}
}
/*==============================================================================
函数名    :  AdjustPrsMaxSpan
功能      :  调整最大prsSpan
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-5-9					薛亮							create
==============================================================================*/
u16 AdjustPrsMaxSpan( u8 byVmpStyle )
{
	u16 wPrsMaxSpan = 480;
	u8 byRealChnnlNum = ConvertVmpStyle2ChnnlNum(byVmpStyle);
	if( byRealChnnlNum == 0)
	{
		OspPrintf(TRUE, FALSE, "[AdjustPrsMaxSpan] unexpected chnnl num:%u!\n", byRealChnnlNum);
		return 0;
	}
	if( byRealChnnlNum <= 4)
	{
		wPrsMaxSpan = 960;
	}
	else if ( byRealChnnlNum <= 9 )
	{
		wPrsMaxSpan = 738;
	}
	else
	{
		wPrsMaxSpan = 480;
	}
// 	if(byVmpStyle != 0)
// 	{
// 		wPrsMaxSpan = wPrsMaxSpan * 20 /byRealChnnlNum;
// 		if(wPrsMaxSpan > 738)
// 		{
// 			wPrsMaxSpan = 738;
// 		}
// 	}
	return wPrsMaxSpan;
	
}

void GetRSParamByPos(u8 byPos, TRSParam &tRSParam)
{
    memset(&tRSParam, 0, sizeof(tRSParam));
    
    if (byPos >= MAXNUM_MPUSVMP_MEMBER)
    {
        OspPrintf(TRUE, FALSE, "[GetRSParamByPos] unexpected pos.%d, ignore it!\n", byPos);
        return;
    }

#ifndef WIN32
	
#ifndef _MPUB_256_
    if (byPos < 4)
    {
        tRSParam.m_wFirstTimeSpan = 40;
        tRSParam.m_wSecondTimeSpan = 360;
        tRSParam.m_wThirdTimeSpan = 960;
        tRSParam.m_wRejectTimeSpan = 1920;
    }
//     else if (byPos < 9)
//     {
//         tRSParam.m_wFirstTimeSpan = 40;
//         tRSParam.m_wSecondTimeSpan = 240;
//         tRSParam.m_wThirdTimeSpan = 480;
//         tRSParam.m_wRejectTimeSpan = 960;
//     }
    else
    {
        tRSParam.m_wFirstTimeSpan = 40;
        tRSParam.m_wSecondTimeSpan = 100;
        tRSParam.m_wThirdTimeSpan = 200;
        tRSParam.m_wRejectTimeSpan = 400;
    }
#else
	//B 板
	if (byPos < 4)
    {
        tRSParam.m_wFirstTimeSpan = 40;
        tRSParam.m_wSecondTimeSpan = 360;
        tRSParam.m_wThirdTimeSpan = 960;
        tRSParam.m_wRejectTimeSpan = 1920;
    }
	else if (byPos < 9)
    {
         tRSParam.m_wFirstTimeSpan = 40;
         tRSParam.m_wSecondTimeSpan = 240;
         tRSParam.m_wThirdTimeSpan = 480;
         tRSParam.m_wRejectTimeSpan = 960;
    }
    else
    {
        tRSParam.m_wFirstTimeSpan = 40;
        tRSParam.m_wSecondTimeSpan = 120;
        tRSParam.m_wThirdTimeSpan = 240;
        tRSParam.m_wRejectTimeSpan = 480;
    }


#endif

#endif

    return;
}

void ConvertToRealFR(THDAdaptParam& tAdptParm)
{
	if (tAdptParm.GetVidType() != MEDIA_TYPE_H264)
	{
		u8 byFrameRate = tAdptParm.GetFrameRate();
		switch(byFrameRate)
		{
		case VIDEO_FPS_2997_1:
			byFrameRate = 30;
			break;
		case VIDEO_FPS_25:
			byFrameRate = 25;
			break;
		case VIDEO_FPS_2997_2:
			byFrameRate = 15;
		    break;
		case VIDEO_FPS_2997_3:
			byFrameRate = 10;
		    break;
		case VIDEO_FPS_2997_4:
		case VIDEO_FPS_2997_5:
			byFrameRate = 6;
			break;
		case VIDEO_FPS_2997_6:
			byFrameRate = 5;
			break;
		case VIDEO_FPS_2997_30:
			byFrameRate = 1;
			break;
		case VIDEO_FPS_2997_7P5:
			byFrameRate = 4;
			break;
		case VIDEO_FPS_2997_10:
			byFrameRate = 3;
			break;
		case VIDEO_FPS_2997_15:
			byFrameRate = 2;
			break;
		default:
			OspPrintf(TRUE, FALSE, "[ConvertToRealFR] undef non264 framerate%d\n", byFrameRate);
		    break;
		}

		tAdptParm.SetFrameRate(byFrameRate);
	}
}

void ConverToRealParam(u8 byChnId, u8 byOutIdx, THDAdaptParam& tBasParam)
{
	THDAdaptParam tTempParam = tBasParam;
	u16 	   wWidth = tBasParam.GetWidth(); 
    u16 	   wHeight= tBasParam.GetHeight();
    u8		   byFrameRate= tBasParam.GetFrameRate();
	if ( g_tResBrLimitTable.GetRealResFrameRatebyBitrate(tBasParam.GetVidType(),wWidth,wHeight,byFrameRate,tBasParam.GetBitrate(),tBasParam.GetProfileType()) )
	{
		do 
		{
			u8 byNewRes = GetResViaWH(wWidth,wHeight);
			u16 wTempWidth = 0;
			u16 wTempHeight = 0;
			if (TYPE_MPUBAS_H == g_cMpuBasApp.m_byWorkMode)
			{
				if ( byOutIdx >5 )
				{
					break;
				}
			
				if ( byOutIdx == 0 )
				{
					GetWHViaRes(VIDEO_FORMAT_XGA,wTempWidth,wTempHeight);
					if ( wWidth*wHeight < wTempWidth*wTempHeight )
					{
						break;
					}
				}
				else if ( byOutIdx == 1 && byNewRes != VIDEO_FORMAT_HD720 )
				{
					break;
				}
				else if ( byOutIdx == 2 && byNewRes != VIDEO_FORMAT_4CIF )
				{
					break;
				}
				else if ( byOutIdx == 3 && byNewRes != VIDEO_FORMAT_CIF )
				{
					break;
				}	
			}
			else
			{
				if ( byOutIdx >1 )
				{
					break;
				}
			
				if ( byOutIdx == 0 )
				{
					GetWHViaRes(VIDEO_FORMAT_4CIF,wTempWidth,wTempHeight);
					if ( wWidth*wHeight < wTempWidth*wTempHeight )
					{
						break;
					}
				}	
			}

			tBasParam.SetResolution(wWidth,wHeight);
			tBasParam.SetFrameRate(byFrameRate);
			mpulog(MPU_INFO,"[ConverToRealParam]chnid:%d,outIdx:%d convert width:%d-height:%d framerate:%d to width:%d-height:%d framerate:%d\n",
				byChnId, byOutIdx, tTempParam.GetWidth(),tTempParam.GetHeight(),tTempParam.GetFrameRate(),wWidth,wHeight,byFrameRate);
		} while (0);
	}
}

//END OF FILE

