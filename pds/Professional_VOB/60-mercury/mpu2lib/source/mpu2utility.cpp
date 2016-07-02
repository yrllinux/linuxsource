/*****************************************************************************
   模块名      : mpu2lib
   文件名      : mpu2utility.cpp
   相关文件    : mpu2utility.h
   文件实现功能: 工具/通用 类/函数 实现
   作者        : 周翼亮
   版本        : V4.7  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期       版本        修改人      修改内容
   2011/12/07    4.7         周翼亮      创建
******************************************************************************/
#include "mpu2utility.h"
#include "mpu2struct.h"
#include "mpu2inst.h"


s32 g_nmpubaslog = 1;

u16 g_wKdvlogMid = MID_PUB_ALWAYS;

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

/*lint -save -esym(526, __builtin_va_start)*/
/*lint -save -esym(628, __builtin_va_start)*/
/*====================================================================
	函数  : ErrorLog
	功能  : 打印等级为error的kdvlog
	输入  : pszFmt: 要打印的字符串
	输出  : 无
	返回  : 无
	注    : 
----------------------------------------------------------------------
	修改记录    ：
	日  期         版本        修改人        修改内容
 	2011/12/07     v4.7			周翼亮		create    
====================================================================*/
void ErrorLog(s8* pszFmt,...)
{
	s8 achBuf[1024] = { 0 };
    va_list argptr ;
	memset(&argptr,0,sizeof(va_list));
    va_start( argptr, pszFmt );
    vsprintf( achBuf, pszFmt, argptr );
	LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS,achBuf);
    va_end( argptr );
	
	
}

/*====================================================================
	函数  : KeyLog
	功能  : 打印等级为keystatus的kdvlog
	输入  : pszFmt: 要打印的字符串
	输出  : 无
	返回  : 无
	注    : 
----------------------------------------------------------------------
	修改记录    ：
	日  期         版本        修改人        修改内容
 	2011/12/07     v4.7			周翼亮		create    
====================================================================*/
void KeyLog(s8* pszFmt, ...)
{
	//setmpulog 调log等级起作用
	s8 achBuf[1024] = { 0 };
    va_list argptr ;
	memset(&argptr,0,sizeof(va_list));
	
    va_start( argptr, pszFmt ); 
    vsprintf( achBuf, pszFmt, argptr );
	LogPrint(LOG_LVL_KEYSTATUS,g_wKdvlogMid,achBuf);
    va_end( argptr );
}

/*====================================================================
	函数  : WarningLog
	功能  : 打印等级为warning的kdvlog
	输入  : pszFmt: 要打印的字符串
	输出  : 无
	返回  : 无
	注    : 
----------------------------------------------------------------------
	修改记录    ：
	日  期         版本        修改人        修改内容
 	2011/12/07     v4.7			周翼亮		create    
====================================================================*/
void WarningLog(s8* pszFmt,...)
{
	s8 achBuf[1024] = { 0 };
    va_list argptr;
	memset(&argptr,0,sizeof(va_list));
	
    va_start( argptr, pszFmt );
    vsprintf( achBuf, pszFmt, argptr );
	LogPrint(LOG_LVL_WARNING,MID_PUB_ALWAYS,achBuf);
    va_end( argptr );
}

/*====================================================================
	函数  : DetailLog
	功能  : 打印等级为detail的kdvlog
	输入  : pszFmt: 要打印的字符串
	输出  : 无
	返回  : 无
	注    : 
----------------------------------------------------------------------
	修改记录    ：
	日  期         版本        修改人        修改内容
 	2011/12/07     v4.7			周翼亮		create    
====================================================================*/
void DetailLog(s8* pszFmt,...)
{
	s8 achBuf[1024] = { 0 };
    va_list argptr;
	memset(&argptr,0,sizeof(va_list));
	
    va_start( argptr, pszFmt );
    vsprintf( achBuf, pszFmt, argptr );
	LogPrint(LOG_LVL_DETAIL,g_wKdvlogMid,achBuf);
    va_end( argptr );
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
/*lint -save -e438 -e530*/
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
	case VMP_STYLE_TEN_M:
		return 10;

	case VMP_STYLE_THIRTEEN:
	case VMP_STYLE_THIRTEEN_M:
		return 13;

	case VMP_STYLE_SIXTEEN:
		return 16;

	case VMP_STYLE_TWENTY:
		return 20;
	
	case VMP_STYLE_THREE_2BOTTOMRIGHT:
	case VMP_STYLE_THREE_2BOTTOM2SIDE:
	case VMP_STYLE_THREE_2BOTTOMLEFT:
	case VMP_STYLE_THREE_VERTICAL:
		return 3;

	case VMP_STYLE_FOUR_3BOTTOMMIDDLE:
		return 4;

	case VMP_STYLE_TWENTYFIVE:
		return 25;

	default:
		return 0;
	}
}


/*=============================================================================
  函 数 名： ConvertVcStyle2HardStyle
  功    能： 由会控风格得到底层风格
  算法实现： 
  全局变量： 
  参    数： u8 oldstyle
  返 回 值： u8 
=============================================================================*/
u8 ConvertVcStyle2HardStyle( u8 byOldstyle )
{
    u8 byStyle;
    switch ( byOldstyle )
    {
    case VMP_STYLE_ONE:  // 一画面
        byStyle = (u8)MULPIC_TYPE_ONE;  // 单画面
        break;

    case VMP_STYLE_VTWO: // 两画面：左右分
        byStyle = (u8)MULPIC_TYPE_VTWO;  // 两画面：左右分
        break;

    case VMP_STYLE_HTWO:  // 两画面: 一大一小
        byStyle = (u8)MULPIC_TYPE_ITWO; // 两画面： 一大一小（大画面分成9份发送）
        break;

    case VMP_STYLE_THREE: // 三画面
        byStyle = (u8)MULPIC_TYPE_THREE; // 三画面
        break;

    case VMP_STYLE_FOUR: // 四画面
        byStyle = (u8)MULPIC_TYPE_FOUR;  // 四画面
        break;

    case VMP_STYLE_SIX: //六画面
        byStyle = (u8)MULPIC_TYPE_SIX; //六画面
        break;

    case VMP_STYLE_EIGHT: //八画面
        byStyle = (u8)MULPIC_TYPE_EIGHT; //八画面
        break;
    case VMP_STYLE_NINE: //九画面
        byStyle = (u8)MULPIC_TYPE_NINE; //九画面
        break;

    case VMP_STYLE_TEN: //十画面
        byStyle = (u8)MULPIC_TYPE_TEN; //十画面：二大八小（大画面分成4份发送）
        break;

    case VMP_STYLE_THIRTEEN:  //十三画面
        byStyle = (u8)MULPIC_TYPE_THIRTEEN; //十三画面
        break;

    case VMP_STYLE_SIXTEEN: //十六画面
        byStyle = (u8)MULPIC_TYPE_SIXTEEN; //十六画面
        break;

    case VMP_STYLE_TWENTY: //二十画面
        byStyle = (u8)MULPIC_TYPE_TWENTY;
        break;
        
    case VMP_STYLE_SPECFOUR://特殊四画面 
        byStyle = (u8)MULPIC_TYPE_SFOUR; //特殊四画面：一大三小（大画面分成4份发送）
        break;

    case VMP_STYLE_SEVEN: //七画面
        byStyle = (u8)MULPIC_TYPE_SEVEN; //七画面：三大四小（大画面分成4份发送）
        break;

	case VMP_STYLE_THREE_2BOTTOMRIGHT:	   //三画面，2个小的在底部靠右
		byStyle = (u8)MULPIC_TYPE_THREE_2IBR; 
		break;

	case VMP_STYLE_THREE_2BOTTOM2SIDE:	   //三画面，2个小的在底部两边
		byStyle = (u8)MULPIC_TYPE_THREE_2IB; 
		break;

	case VMP_STYLE_THREE_2BOTTOMLEFT:	   //三画面，2个小的在底部靠左
		byStyle = (u8)MULPIC_TYPE_THREE_2IBL; 
		break;

	case VMP_STYLE_THREE_VERTICAL:	   //垂直等大3画面
		byStyle = (u8)MULPIC_TYPE_VTHREE; 
		break;

	case VMP_STYLE_FOUR_3BOTTOMMIDDLE:	   //四画面，3个在底部中央
		byStyle = (u8)MULPIC_TYPE_FOUR_3IB; 
		break;
		
	case VMP_STYLE_TEN_M   :	  //十画面，中间两个，上下各4个
		byStyle = (u8)MULPIC_TYPE_TEN_MID_LR; 
		break;
		
	case VMP_STYLE_THIRTEEN_M  :	  //十三画面，中间一个大的，环绕中间型
		byStyle = (u8)MULPIC_TYPE_THIRTEEN_MID; 
		break;

	case VMP_STYLE_TWENTYFIVE  :	  //二十五画面
		byStyle = (u8)MULPIC_TYPE_TWENTYFIVE; 
		break;
    default:
        byStyle = (u8)MULPIC_TYPE_VTWO;    // 默认两画面：左右分
        break;
    }
    return byStyle;
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
    
    if (byPos >= MAXNUM_MPU2VMP_MEMBER)
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

void ConverToRealParam(u8 byBasIdx, u8 byChnId,u8 byOutIdx,THDAdaptParam& tBasParam)
{
	THDAdaptParam tTempParam = tBasParam;
	u16 	   wWidth = tBasParam.GetWidth(); 
    u16 	   wHeight= tBasParam.GetHeight();
    u8		   byFrameRate= tBasParam.GetFrameRate();
	if ( g_tResBrLimitTable.GetRealResFrameRatebyBitrate(tBasParam.GetVidType(),wWidth,wHeight,byFrameRate,tBasParam.GetBitrate(),tBasParam.GetProfileType()) )
	{
		tBasParam.SetResolution(wWidth,wHeight);
		tBasParam.SetFrameRate(byFrameRate);
		KeyLog("[ConverToRealParam]BasIdx:%d,chnid:%d,outIdx:%d convert width:%d-height:%d framerate:%d to width:%d-height:%d framerate:%d\n",
			   byBasIdx,byChnId, byOutIdx, tTempParam.GetWidth(),tTempParam.GetHeight(),tTempParam.GetFrameRate(),wWidth,wHeight,byFrameRate
			  );
	}
}

//END OF FILE

