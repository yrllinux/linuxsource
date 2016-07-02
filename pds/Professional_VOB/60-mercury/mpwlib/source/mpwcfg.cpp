/*****************************************************************************
   模块名      : MpwLib多画面复合电视墙
   文件名      : mpwcfg.cpp
   相关文件    : 
   文件实现功能: 实现对配置文件的读取
   作者        : john
   版本        : V4.0  Copyright(C) 2001-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/09/25  1.0         john         创建
******************************************************************************/

#include "mpwcfg.h"

s8	MPWCFG_FILE[KDV_MAX_PATH] = {0};
s8	SECTION_MPWSYS[] = "EQPVMP";


CMpwCfg::CMpwCfg()
{    
}

CMpwCfg::~CMpwCfg()
{
}

/*=============================================================================
  函 数 名： ParseParam
  功    能： 分析参数是否合法
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： BOOL TRUE：合法，FALSE：非法
=============================================================================*/
BOOL32 CMpwCfg::ParseParam()
{
    BOOL32 bRet = TRUE;
    // Map使用标志
    if (m_tMulPicParam.dwTotalMapNum <= INVALID_VALUE)
    {
        OspPrintf(TRUE, FALSE, "Mpw Fatal Error:The Map Number(1<=num<=5) is %u\n",m_tMulPicParam.dwTotalMapNum);
        bRet = FALSE;
    }

    if (INVALID_VALUE == dwMcuIP)
    {
        OspPrintf(TRUE, FALSE, "Mpw Fatal Error:The Mcu Ip is %u\n", dwMcuIP);
        bRet = FALSE;
    }

    if (INVALID_VALUE == wMcuPort)
    {
        OspPrintf(TRUE, FALSE, "Mpw Fatal Error:The Mcu Port is %u\n", wMcuPort);
        bRet = FALSE;
    }

    if (INVALID_VALUE == wMcuId)
    {
        OspPrintf(TRUE, FALSE, "Mpw McuId Error:McuId is %u,Maybe can't register to MCU!\n", wMcuId);
        bRet = FALSE;
    }

    if (INVALID_VALUE == byEqpType)
    {
        OspPrintf(TRUE, FALSE, "Mpw Type Error:Type is %u,Maybe can't register to MCU\n",byEqpType);
        bRet = FALSE;
    }

    if (INVALID_VALUE == byEqpId)
    {
        OspPrintf(TRUE, FALSE, "Mpw Id Error:EqpId is %u,Maybe can't register to MCU\n",byEqpId);
        bRet = FALSE;
    }

    if (INVALID_VALUE == dwLocalIp)
    {
        OspPrintf(TRUE, FALSE, "Mpw Ip Error:IP is %u,Maybe can't receive Video,becase MCU can't has correct switch!\n",dwLocalIp);
    }

    if (INVALID_VALUE == wRecvStartPort)
    {
        OspPrintf(TRUE, FALSE, "Mpw Port Error:Port is %u,Maybe can't receive Video,becase MCU can't has correct switch!\n",wRecvStartPort);
    }
    return bRet;
}

/*=============================================================================
  函 数 名： ReadConfigureFile
  功    能： 读配置文件
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CMpwCfg::ReadConfigureFile(void)
{
    sprintf(MPWCFG_FILE, "%s/mcueqp.ini", DIR_CONFIG);

    s32 nValue;
    tEncparamMp4.m_byEncType = (u8)wEnVidFmt[0][0];
    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "MPEG4BitrateControlMode", 0, &nValue)) // 读配置文件失败
    {
        nValue = (s32)wEnVidFmt[0][1];
    }
    tEncparamMp4.m_byRcMode = (u8)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "MPEG4VideoEcdIKeyRate", 0, &nValue))
    {
        nValue = wEnVidFmt[0][2];
    }
    tEncparamMp4.m_byMaxKeyFrameInterval = (u8)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "MPEG4VideoEcdQualMaxValu", 0, &nValue))
    {
        nValue = (u8)wEnVidFmt[0][3];
    }
    tEncparamMp4.m_byMaxQuant = (u8)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "MPEG4VideoEcdQualMinValu", 0, &nValue))
    {
        nValue = (u8)wEnVidFmt[0][4];
    }
    tEncparamMp4.m_byMinQuant = (u8)nValue;



    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "MPEG4EncodeBitRate", 0, &nValue))
    {
        nValue = wEnVidFmt[0][5];    
    }
    tEncparamMp4.m_wBitRate = (u16)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "MPEG4BandWidth", 0, &nValue))
    {
        nValue = 0;
    }
    tEncparamMp4.m_dwSndNetBand = (u32)nValue;

#ifdef SOFT_CODEC
        tEncparamMp4.m_byQcMode = 0;
        tEncparamMp4.m_byQI     = 5;
        tEncparamMp4.m_byQP     = 13;
#else
        tEncparamMp4.m_byPalNtsc = (u8)wEnVidFmt[0][6];
        tEncparamMp4.m_byCapPort = (u8)wEnVidFmt[0][7];
    
    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "MPEG4FrameRate", 0, &nValue))
    {
        nValue = (u8)wEnVidFmt[0][8];
    }
    tEncparamMp4.m_byFrameRate = (u8)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "MPEG4VideoImageQuality", 0, &nValue))
    {
        nValue = (u8)wEnVidFmt[0][9];
    }
    tEncparamMp4.m_byImageQulity = (u8)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "MPEG4VideoHeight", 0, &nValue))
    {
        nValue = wEnVidFmt[0][10];
    }
    tEncparamMp4.m_wVideoHeight = (u16)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "MPEG4VideoWidth", 0, &nValue))
    {
        nValue = wEnVidFmt[0][11];
    }
    tEncparamMp4.m_wVideoWidth = (u16)nValue;
#endif
    // 261
    tEncparamH261.m_byEncType = (u8)wEnVidFmt[1][0];
    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "261BitrateControlMode", 0, &nValue))
    {
        nValue = (u8)wEnVidFmt[1][1];
    }
    tEncparamH261.m_byRcMode = (u8)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "261VideoEcdIKeyRate", 0, &nValue))
    {
        nValue = wEnVidFmt[1][2];
    }
    tEncparamH261.m_byMaxKeyFrameInterval = nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "261VideoEcdQualMaxValu", 0, &nValue))
    {
        nValue = (u8)wEnVidFmt[1][3];
    }
    tEncparamH261.m_byMaxQuant = (u8)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "261VideoEcdQualMinValu", 0, &nValue))
    {
        nValue = (u8)wEnVidFmt[1][4];
    }
    tEncparamH261.m_byMinQuant = (u8)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "261EncodeBitRate", 0, &nValue))
    {
        nValue = wEnVidFmt[1][5];
    }
    tEncparamH261.m_wBitRate = (u16)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "261BandWidth", 0, &nValue))
    {
        nValue = 0;
    }
    tEncparamH261.m_dwSndNetBand = (u32)nValue;

#ifdef SOFT_CODEC
    tEncparamH261.m_byQcMode = 0;
    tEncparamH261.m_byQI     = 5;
    tEncparamH261.m_byQP     = 13;
#else
    tEncparamH261.m_byPalNtsc = (u8)wEnVidFmt[1][6];
    tEncparamH261.m_byCapPort = (u8)wEnVidFmt[1][7];

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "261FrameRate", 0, &nValue))
    {
        nValue = (u8)wEnVidFmt[1][8];
    }
    tEncparamH261.m_byFrameRate = (u8)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "261VideoImageQuality", 0, &nValue))
    {
        nValue = (u8)wEnVidFmt[1][9];
    }
    tEncparamH261.m_byImageQulity = (u8)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "261VideoHeight", 0, &nValue))
    {
        nValue	= wEnVidFmt[1][10];
    }
    tEncparamH261.m_wVideoHeight = (u16)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "261VideoWidth", 0, &nValue))
    {
        nValue = wEnVidFmt[1][11];
    }
    tEncparamH261.m_wVideoWidth = (u16)nValue;
#endif

    // h262
    // OspPrintf(TRUE, FALSE, "[GetDefaultParam] GetDefaultParam MEDIA_TYPE_H262！\n");
    tEncparamH262.m_byEncType = (u8)wEnVidFmt[2][0];
    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "262BitrateControlMode", 0, &nValue))
    {
        nValue = (u8)wEnVidFmt[2][1];
    }
    tEncparamH262.m_byRcMode = (u8)nValue;
                
    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "262VideoEcdIKeyRate", 0, &nValue))       
    {           
        nValue = wEnVidFmt[2][2];       
    }            
    tEncparamH262.m_byMaxKeyFrameInterval = nValue;
        
    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "262VideoEcdQualMaxValu", 0, &nValue))        
    {            
        nValue = (u8)wEnVidFmt[2][3];        
    }            
    tEncparamH262.m_byMaxQuant = (u8)nValue;
       
    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "262VideoEcdQualMinValu", 0, &nValue))      
    {           
        nValue = (u8)wEnVidFmt[2][4];     
    }            
    tEncparamH262.m_byMinQuant = (u8)nValue;
       
    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "262EncodeBitRate", 0, &nValue))     
    {         
        nValue = wEnVidFmt[2][5];
    }         
    tEncparamH262.m_wBitRate = (u16)nValue;

    if (!::GetRegKeyInt( MPWCFG_FILE, SECTION_MPWSYS, "262BandWidth", 0, &nValue))
    {
        nValue = 0;
    }
    tEncparamH262.m_dwSndNetBand = (u32)nValue;
  
#ifdef SOFT_CODEC
    tEncparamH262.m_byQcMode = 0;
    tEncparamH262.m_byQI     = 5;
    tEncparamH262.m_byQP     = 13;
#else
    tEncparamH262.m_byPalNtsc = (u8)wEnVidFmt[2][6];
    tEncparamH262.m_byCapPort = (u8)wEnVidFmt[2][7];
        
    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "262FrameRate", 0, &nValue))
    {
        nValue = (u8)wEnVidFmt[2][8];
    }
    tEncparamH262.m_byFrameRate = (u8)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "262VideoImageQuality", 0, &nValue))
    {
        nValue = (u8) wEnVidFmt[2][9];
    }           
    tEncparamH262.m_byImageQulity = (u8)nValue;
        
    if (!::GetRegKeyInt( MPWCFG_FILE, SECTION_MPWSYS, "262VideoHeight", 0, &nValue))
       
    {
        nValue = wEnVidFmt[2][10];
    }
    tEncparamH262.m_wVideoHeight = (u16)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "262VideoWidth", 0, &nValue))
    {
        nValue = wEnVidFmt[2][11];
    }
    tEncparamH262.m_wVideoWidth = (u16)nValue;
   
#endif

    // h263
    tEncparamH263.m_byEncType = (u8)wEnVidFmt[3][0];
    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "263BitrateControlMode", 0, &nValue))
    {
        nValue = wEnVidFmt[3][1];
    }
    tEncparamH263.m_byRcMode = (u8)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "263VideoEcdIKeyRate", 0, &nValue))
    {
        nValue = wEnVidFmt[3][2];
    }
    tEncparamH263.m_byMaxKeyFrameInterval = (u8)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "263VideoEcdQualMaxValu", 0, &nValue))
    {
        nValue = (u8)wEnVidFmt[3][3];
    }
    tEncparamH263.m_byMaxQuant = (u8)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "263VideoEcdQualMinValu", 0, &nValue))
    {
        nValue = (u8)wEnVidFmt[3][4];
    }
    tEncparamH263.m_byMinQuant = (u8)nValue;
        
    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "263EncodeBitRate", 0, &nValue))
    {
        nValue = wEnVidFmt[3][5];
    }
    tEncparamH263.m_wBitRate = (u16)nValue;

    if (!::GetRegKeyInt( MPWCFG_FILE, SECTION_MPWSYS, "263BandWidth", 0, &nValue))
    {
        nValue = 0;
    }
    tEncparamH263.m_dwSndNetBand = (u32)nValue;

#ifdef SOFT_CODEC
    tEncparamH263.m_byQcMode = 0;
    tEncparamH263.m_byQI     = 5;
    tEncparamH263.m_byQP     = 13;
#else
    tEncparamH263.m_byPalNtsc = (u8)wEnVidFmt[3][6];
    tEncparamH263.m_byCapPort = (u8)wEnVidFmt[3][7];
        
    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "263FrameRate", 0, &nValue))
    {
        nValue = (u8)wEnVidFmt[3][8];
    }
    tEncparamH263.m_byFrameRate = (u8)nValue;
        
    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "263VideoImageQuality", 0, &nValue))
    {
        nValue = (u8) wEnVidFmt[3][9];
    }
    tEncparamH263.m_byImageQulity = (u8)nValue;

    if (!::GetRegKeyInt( MPWCFG_FILE, SECTION_MPWSYS, "263VideoHeight", 0, &nValue))
    {
        nValue = wEnVidFmt[3][10];
    }
    tEncparamH263.m_wVideoHeight = (u16)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "263VideoWidth", 0, &nValue))
    {
        nValue = wEnVidFmt[3][11];
    }
    tEncparamH263.m_wVideoWidth = (u16)nValue;

    #endif


    // H263+
    tEncparamH263plus.m_byEncType = (u8)wEnVidFmt[4][0];
        
    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "263PLUSBitrateControlMode", 0, &nValue))
    {
        nValue = (u8)wEnVidFmt[4][1];
    }
    tEncparamH263plus.m_byRcMode = (u8)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "263PLUSVideoEcdIKeyRate", 0, &nValue))
    {
        nValue = wEnVidFmt[4][2];
    }
    tEncparamH263plus.m_byMaxKeyFrameInterval = nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "263PLUSVideoEcdQualMaxValu", 0, &nValue))
    {
        nValue = (u8)wEnVidFmt[4][3];
    }
    tEncparamH263plus.m_byMaxQuant = (u8)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "263PLUSVideoEcdQualMinValu", 0, &nValue))
    {
        nValue = (u8)wEnVidFmt[4][4];
    }
    tEncparamH263plus.m_byMinQuant = (u8)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "263PLUSEncodeBitRate", 0, &nValue))
    {
        nValue = wEnVidFmt[4][5];
    }
    tEncparamH263plus.m_wBitRate = (u16)nValue;

    if (!::GetRegKeyInt( MPWCFG_FILE, SECTION_MPWSYS, "263PLUSBandWidth", 0, &nValue))
    {
        nValue = 0;
    }
    tEncparamH263plus.m_dwSndNetBand = (u32)nValue;

#ifdef SOFT_CODEC
    tEncparamH263plus.m_byQcMode = 0;
    tEncparamH263plus.m_byQI     = 5;
    tEncparamH263plus.m_byQP     = 13;
#else
    tEncparamH263plus.m_byPalNtsc = (u8)wEnVidFmt[4][6];
    tEncparamH263plus.m_byCapPort = (u8)wEnVidFmt[4][7];
        
    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "263PLUSFrameRate", 0, &nValue))
    {
        nValue = (u8)wEnVidFmt[4][8];
    }
    tEncparamH263plus.m_byFrameRate = (u8)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "263PLUSVideoImageQuality", 0, &nValue))
    {
        nValue = (u8) wEnVidFmt[4][9];
    }
    tEncparamH263plus.m_byImageQulity = (u8)nValue;

    if (!::GetRegKeyInt( MPWCFG_FILE, SECTION_MPWSYS, "263PLUSVideoHeight", 0, &nValue))
    {
        nValue	= wEnVidFmt[4][10];
    }
    tEncparamH263plus.m_wVideoHeight = (u16)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "263PLUSVideoWidth", 0, &nValue))
    {
        nValue = wEnVidFmt[4][11];
    }
    tEncparamH263plus.m_wVideoWidth = (u16)nValue;
#endif

    // h264
    tEncparamH264.m_byEncType = (u8)wEnVidFmt[5][0];
    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "264SBitrateControlMode", 0, &nValue))
    {
        nValue = (u8)wEnVidFmt[5][1];
    }
    tEncparamH264.m_byRcMode = (u8)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "264VideoEcdIKeyRate", 0, &nValue))
    {
        nValue = wEnVidFmt[5][2];
    }
    tEncparamH264.m_byMaxKeyFrameInterval = nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "264VideoEcdQualMaxValu", 0, &nValue))
    {
        nValue = (u8)wEnVidFmt[5][3];
    }
    tEncparamH264.m_byMaxQuant = (u8)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "264VideoEcdQualMinValu", 0, &nValue))
    {
        nValue = (u8)wEnVidFmt[5][4];
    }
    tEncparamH264.m_byMinQuant = (u8)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "264EncodeBitRate", 0, &nValue))
    {
        nValue = wEnVidFmt[5][5];
    }
    tEncparamH264.m_wBitRate = (u16)nValue;

    if (!::GetRegKeyInt( MPWCFG_FILE, SECTION_MPWSYS, "264BandWidth", 0, &nValue))
    {
        nValue = 0;
    }
    tEncparamH264.m_dwSndNetBand = (u32)nValue;

#ifdef SOFT_CODEC
    tEncparamH264.m_byQcMode = 0;
    tEncparamH264.m_byQI     = 5;
    tEncparamH264.m_byQP     = 13;
#else
    tEncparamH264.m_byPalNtsc = (u8)wEnVidFmt[5][6];
    tEncparamH264.m_byCapPort = (u8)wEnVidFmt[5][7];

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "264FrameRate", 0, &nValue))
    {
        nValue = (u8)wEnVidFmt[5][8];
    }
    tEncparamH264.m_byFrameRate = (u8)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "264SVideoImageQuality", 0, &nValue))
    {
        nValue = 0; // (u8) wEnVidFmt[5][9];  // h264目前只支持速度优先
    }
    tEncparamH264.m_byImageQulity = (u8)nValue;

    if (!::GetRegKeyInt( MPWCFG_FILE, SECTION_MPWSYS, "264VideoHeight", 0, &nValue))
    {
        nValue = wEnVidFmt[5][10];
    }
    tEncparamH264.m_wVideoHeight = (u16)nValue;

    if (!::GetRegKeyInt(MPWCFG_FILE, SECTION_MPWSYS, "264VideoWidth", 0, &nValue))
    {
        nValue = wEnVidFmt[5][11];
    }
    tEncparamH264.m_wVideoWidth = (u16)nValue;
#endif

    return;
}

/*=============================================================================
  函 数 名： GetDefaultParam
  功    能： 取对应的参数
  算法实现： 
  全局变量： 
  参    数： u8 byEnctype：媒体类型
             TVideoEncParam& tEncparam
  返 回 值： void 
=============================================================================*/
void CMpwCfg::GetDefaultParam(u8 byEnctype, TVideoEncParam& tEncparam)
{
    byEnctype = MEDIA_TYPE_H261; // 强制编成H261
    switch( byEnctype )
    {
    case MEDIA_TYPE_MP4:
        memcpy(&tEncparam, &tEncparamMp4, sizeof(TVideoEncParam));
        break;

    case MEDIA_TYPE_H261: //261
        memcpy(&tEncparam, &tEncparamH261, sizeof(TVideoEncParam));
        break;

    case MEDIA_TYPE_H262:   //262(mepg2)
        memcpy(&tEncparam, &tEncparamH262, sizeof(TVideoEncParam));
        break;

    case MEDIA_TYPE_H263:  //263
        memcpy(&tEncparam, &tEncparamH263, sizeof(TVideoEncParam));
        break;

    case MEDIA_TYPE_H263PLUS:  //263+
        memcpy(&tEncparam, &tEncparamH263plus, sizeof(TVideoEncParam));
        break;

    case MEDIA_TYPE_H264:  //264
        memcpy(&tEncparam, &tEncparamH264, sizeof(TVideoEncParam));
        break;

    default:
        OspPrintf(TRUE, FALSE, "[MpwCfg] Unknown media type\n");
        break;
    }

    return;
}

/*=============================================================================
  函 数 名： MpwGetActivePayload
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 byRealPayloadType
  返 回 值： u8 
=============================================================================*/
//u8 MpwGetActivePayload(u8 byRealPayloadType)
//{
//	u8 byAPayload;
//	switch(byRealPayloadType)
//	{
//	case MEDIA_TYPE_PCMA: 
//        byAPayload = ACTIVE_TYPE_PCMA;
//        break;
//
//	case MEDIA_TYPE_PCMU: 
//        byAPayload = ACTIVE_TYPE_PCMU;
//        break;
//
//	case MEDIA_TYPE_G721: 
//        byAPayload = ACTIVE_TYPE_G721;
//        break;
//
//	case MEDIA_TYPE_G722: 
//        byAPayload = ACTIVE_TYPE_G722;
//        break;
//
//	case MEDIA_TYPE_G7231:
//        byAPayload = ACTIVE_TYPE_G7231;
//        break;
//
//	case MEDIA_TYPE_G728: 
//        byAPayload = ACTIVE_TYPE_G728;
//        break;
//
//	case MEDIA_TYPE_G729: 
//        byAPayload = ACTIVE_TYPE_G729;
//        break;
//
//	case MEDIA_TYPE_G7221:
//        byAPayload = ACTIVE_TYPE_G7221;
//        break;
//
//	case MEDIA_TYPE_H261: 
//        byAPayload = ACTIVE_TYPE_H261;
//        break;
//
//	case MEDIA_TYPE_H262: 
//        byAPayload = ACTIVE_TYPE_H262;
//        break;
//
//	case MEDIA_TYPE_H263: 
//        byAPayload = ACTIVE_TYPE_H263;
//        break;
//
//	default:
//        byAPayload = byRealPayloadType; 
//        break;
//	}
//	return byAPayload;
//}
