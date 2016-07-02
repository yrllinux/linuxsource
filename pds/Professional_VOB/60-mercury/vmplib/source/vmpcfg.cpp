/*****************************************************************************
   模块名      : 画面复合器配置
   文件名      : VMPCfg.cpp
   创建时间    : 2003年 12月 4日
   实现功能    : 
   作者        : zhangsh
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
******************************************************************************/
#include "vmpcfg.h"

void vmplog( s8* pszFmt, ...);
/*lint -save -e765*/
s8      VMPCFG_FILE[KDV_MAX_PATH] = {0};
/*lint -restore*/
const	s8   	SECTION_VMPSYS[] = "EQPVMP";
const	s8      SECTION_VMPDEBUG[] = "EQPDEBUG";

/*====================================================================
	函数名  :ParseParam
	功能    :分析相关参数，若没有设置则设为默认值
	输入    ：
			  
	输出    ：无
	返回值  ：成功返回 TRUE，否则返回FALSE
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   03/12/4	   v1.0	      zhangsh	    create
====================================================================*/
BOOL TVMPCfg::ParseParam()
{
    /*-----------------First is the fatal params,it must give---*/
    //Map使用标志
    if (m_tMulPicParam.dwTotalMapNum < 1)
    {
        OspPrintf(1,0,"VMP Fatal Error:The Map Number(1<=num<=5) is %u\n",m_tMulPicParam.dwTotalMapNum);
        return FALSE;
    }

    if (dwMcuIP == 0)
    {
        OspPrintf(1,0,"VMP Fatal Error:The Mcu Ip Para is %u\n",dwMcuIP);
        return FALSE;
    }

    if (wMcuPort == 0)
    {
        OspPrintf(1,0,"VMP Fatal Error:The Mcu Port Para is %u\n",wMcuPort);
        return FALSE;
    }

    if (byEqpType == 0)
    {
        OspPrintf(1,0,"VMP Type Error:Type is %u,Maybe can't register to MCU\n",byEqpType);
    }

    if (byEqpId == 0)
    {
        OspPrintf(1,0,"VMP Id Error:EqpId is %u,Maybe can't register to MCU\n",byEqpId);
    }

    if (dwLocalIp == 0)
    {
        OspPrintf(1,0,"VMP Ip Error:IP is %u,Maybe can't receive Video,becase MCU can't has correct switch!\n",dwLocalIp);
    }

    if (wRecvStartPort  == 0)
    {
        OspPrintf(1,0,"VMP Port Error:Port is %u,Maybe can't receive Video,becase MCU can't has correct switch!\n",wRecvStartPort);
    }

    if (byMcuId == 0)
    {
        OspPrintf(1,0,"VMP McuId Error:McuId is %u,Maybe can't register to MCU!\n",byMcuId);
    }

    return TRUE;
}

void TVMPCfg::GetDisplayType( u8& byDisplayType )
{
    s32 nValue = DISPLAY_TYPE_PAL;
	if ( ::GetRegKeyInt(VMPCFG_FILE, "MMPOUT", "DisplayType", 0, &nValue) )
    {
        nValue = ( nValue == DISPLAY_TYPE_PAL ) ? DISPLAY_TYPE_PAL : DISPLAY_TYPE_VGA;
		byDisplayType = (u8)nValue;
    }
	return;
}

void TVMPCfg::GetDefaultParam(u8 byEnctype, TVideoEncParam& tEncparam)
{
    s32 nValue;
	// 读是否打开平滑发送, zgc, 2007-09-28
	if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPDEBUG, "IsUseSmoothSend", 1, &nValue))
	{
		m_byIsUseSmoothSend = VMP_SMOOTH_OPEN;
	}
	else
	{
		m_byIsUseSmoothSend = (u8)nValue;
	}
	
    switch( byEnctype )
    {
    //case 97: (mepg4)
    case MEDIA_TYPE_MP4:
        // OspPrintf(TRUE, FALSE, "[Vmp Encode]The Encode format: MP4, %d\n", MEDIA_TYPE_MP4);
        tEncparam.m_byEncType = (u8)EnVidFmt[0][0];

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "MPEG4BitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[0][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "MPEG4VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[0][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "MPEG4VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[0][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "MPEG4VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[0][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "MPEG4EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[0][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "MPEG4BandWidth", 0, &nValue))
        {
            tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
            tEncparam.m_dwSndNetBand = (u32)nValue;
        }
/*
        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "MPEG4VideoEncLostFrameOnOff", 0, &nValue))
        {
            encparam.m_byAutoDelFrm = 2;
        }
        else
        {
            encparam.m_byAutoDelFrm = (u32)nValue;
        }
*/
    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
        tEncparam.m_byPalNtsc = (u8)EnVidFmt[0][6];
        tEncparam.m_byCapPort = (u8)EnVidFmt[0][7];
    
        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "MPEG4FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[0][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "MPEG4VideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8)EnVidFmt[0][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "MPEG4VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[0][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "MPEG4VideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[0][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H261: //261
        // OspPrintf(TRUE, FALSE, "[Vmp Encode]The Encode format: H261, %d\n", MEDIA_TYPE_H261);
        tEncparam.m_byEncType = (u8)EnVidFmt[1][0];
        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "261BitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[1][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "261VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[1][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "261VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[1][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "261VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[1][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "261EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[1][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "261BandWidth", 0, &nValue))
        {
            tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
            tEncparam.m_dwSndNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
        tEncparam.m_byPalNtsc = (u8)EnVidFmt[1][6];
        tEncparam.m_byCapPort = (u8)EnVidFmt[1][7];
        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "261FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[1][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "261VideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8)EnVidFmt[1][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "261VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[1][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "261VideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[1][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H262:   //262(mepg2)
        // OspPrintf(TRUE, FALSE, "[Vmp Encode]The Encode format: H262, %d\n", MEDIA_TYPE_H262);
        tEncparam.m_byEncType = (u8)EnVidFmt[2][0];
        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "262BitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[2][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "262VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[2][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "262VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[2][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "262VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[2][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "262EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[2][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt( VMPCFG_FILE, SECTION_VMPSYS, "262BandWidth", 0, &nValue))
        {
            tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
            tEncparam.m_dwSndNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
        tEncparam.m_byPalNtsc = (u8)EnVidFmt[2][6];
        tEncparam.m_byCapPort = (u8)EnVidFmt[2][7];
        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "262FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[2][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "262VideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8) EnVidFmt[2][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt( VMPCFG_FILE, SECTION_VMPSYS, "262VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[2][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "262VideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[2][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H263:  //263
        // OspPrintf(TRUE, FALSE, "[Vmp Encode]The Encode format: H263, %d\n", MEDIA_TYPE_H263);
        tEncparam.m_byEncType = (u8)EnVidFmt[3][0];
        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "263BitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[3][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "263VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[3][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "263VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[3][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "263VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[3][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "263EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[3][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt( VMPCFG_FILE, SECTION_VMPSYS, "263BandWidth", 0, &nValue))
        {
            tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
            tEncparam.m_dwSndNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
        tEncparam.m_byPalNtsc = (u8)EnVidFmt[3][6];
        tEncparam.m_byCapPort = (u8)EnVidFmt[3][7];
        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "263FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[3][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "263VideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8) EnVidFmt[3][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt( VMPCFG_FILE, SECTION_VMPSYS, "263VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[3][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "263VideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[3][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H263PLUS:  //263+
        // OspPrintf(TRUE, FALSE, "[Vmp Encode]The Encode format: H263+, %d\n", MEDIA_TYPE_H263PLUS);
        tEncparam.m_byEncType = (u8)EnVidFmt[4][0];
        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "263PLUSBitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[4][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "263PLUSVideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[4][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "263PLUSVideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[4][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "263PLUSVideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[4][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "263PLUSEncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[4][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt( VMPCFG_FILE, SECTION_VMPSYS, "263PLUSBandWidth", 0, &nValue))
        {
            tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
            tEncparam.m_dwSndNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
        tEncparam.m_byPalNtsc = (u8)EnVidFmt[4][6];
        tEncparam.m_byCapPort = (u8)EnVidFmt[4][7];
        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "263PLUSFrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[4][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "263PLUSVideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8) EnVidFmt[4][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt( VMPCFG_FILE, SECTION_VMPSYS, "263PLUSVideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[4][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "263PLUSVideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[4][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H264:  //264
        // OspPrintf(TRUE, FALSE, "[Vmp Encode]The Encode format: H264, %d\n", MEDIA_TYPE_H264);
        tEncparam.m_byEncType = (u8)EnVidFmt[5][0];
        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "264SBitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[5][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "264VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[5][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "264VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[5][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "264VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[5][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "264EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[5][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt( VMPCFG_FILE, SECTION_VMPSYS, "264BandWidth", 0, &nValue))
        {
            tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
            tEncparam.m_dwSndNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
        tEncparam.m_byPalNtsc = (u8)EnVidFmt[5][6];
        tEncparam.m_byCapPort = (u8)EnVidFmt[5][7];
        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "264FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[5][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "264SVideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = 0; // (u8) EnVidFmt[5][9];  // 目前h264只支持速度优先
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt( VMPCFG_FILE, SECTION_VMPSYS, "264VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[5][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "264VideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[5][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    default:
        break;
    }

    return;
}

void TVMPCfg::Clear()
{
    dwMaxSendBand = 0;
    byEqpId   = 0;
    byEqpType = 0;
    dwLocalIp = 0;
    byMcuId   = 0;
    dwMcuIP   = 0;
    wMcuPort  = 0;
    wRecvStartPort = 0;
    memset( m_tVideoEncParam, 0, 2 * sizeof(TVideoEncParam) );
    memset( szAlias, 0, sizeof(szAlias) );
	m_byIsUseSmoothSend = VMP_SMOOTH_OPEN;
}

void TVMPCfg::PrintMulPicParam(void) const
{
    vmplog("\tThe type(defined in kdvmulpic.h,<=11):%u\n", m_tMulPicParam.byType );
    vmplog("\tTotal use map number(<=5):%u\n", m_tMulPicParam.dwTotalMapNum );
    vmplog("\tCore Speed(0(default) or other(392 maybe)) :%u\n", m_tMulPicParam.dwCoreSpd );
    vmplog("\tMem Speed(0(default) or other(131 maybe)) :%u\n", m_tMulPicParam.dwMemSpd );
    vmplog("\tMem Size (Must have,normal is 64M):%u\n", m_tMulPicParam.dwMemSize );	
}

void TVMPCfg::PrintEncParam( u8 byVideoIdx ) const
{
	if( 0 == byVideoIdx || 1 == byVideoIdx )
    {
		vmplog("\tEncType: \t%u \n\tRcMode: \t%u \n\tMaxKeyFrameInterval: \t%d \n\tMaxQuant: \t%u \n\tMinQuant: \t%u\n",
                               m_tVideoEncParam[byVideoIdx].m_byEncType,
							   m_tVideoEncParam[byVideoIdx].m_byRcMode,
                               m_tVideoEncParam[byVideoIdx].m_byMaxKeyFrameInterval,
							   m_tVideoEncParam[byVideoIdx].m_byMaxQuant,
                               m_tVideoEncParam[byVideoIdx].m_byMinQuant );

		vmplog("\tBitRate: \t%u \n\tSndNetBand: \t%u \n\tFrameRate: \t%u \n\tImageQulity: \t%u \n\tVideoWidth: \t%u \n\tVideoHeight: \t%u\n",
                               m_tVideoEncParam[byVideoIdx].m_wBitRate,
							   m_tVideoEncParam[byVideoIdx].m_dwSndNetBand,
                               m_tVideoEncParam[byVideoIdx].m_byFrameRate,
							   m_tVideoEncParam[byVideoIdx].m_byImageQulity,
                               m_tVideoEncParam[byVideoIdx].m_wVideoWidth,
							   m_tVideoEncParam[byVideoIdx].m_wVideoHeight );
	}
}

TVMPCfg::TVMPCfg()
{
//    Clear();
    dwMaxSendBand = 0;
    byEqpId   = 0;
    byEqpType = 0;
    dwLocalIp = 0;
    byMcuId   = 0;
    dwMcuIP   = 0;
    wMcuPort  = 0;
    wRecvStartPort = 0;
    memset( m_tVideoEncParam, 0, 2 * sizeof(TVideoEncParam) );
    memset( szAlias, 0, sizeof(szAlias) );
	m_byIsUseSmoothSend = VMP_SMOOTH_OPEN;
    sprintf(VMPCFG_FILE, "%s/mcueqp.ini", DIR_CONFIG);
}
/*lint -save -e1551*/
TVMPCfg::~TVMPCfg()
{
    Clear();
}
/*lint -restore*/
/*=============================================================================
函 数 名： ReadDebugValues
功    能： 从配置文件读取码率作弊值
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
=============================================================================*/
void TVMPCfg::ReadBitrateCheatValues()
{
    s32 nValue;
//    BOOL32 bRet = FALSE;
	// 默认放开码率作弊, zgc, 2008-03-19
    /*bRet = */::GetRegKeyInt( VMPCFG_FILE, SECTION_VMPDEBUG, "IsEnableRateAutoDec", 1, &nValue );
    m_tDebugVal.SetEnableBitrateCheat( (nValue==0) ? FALSE : TRUE );
    
    if ( !m_tDebugVal.IsEnableBitrateCheat() )
        return;

    /*bRet = */::GetRegKeyInt( VMPCFG_FILE, SECTION_VMPDEBUG, "DefaultTargetRatePct", 85, &nValue );
    m_tDebugVal.SetDefaultRate((u16)nValue);

    return;
}

/*=============================================================================
函 数 名： IsNeedBackBrdOut
功    能： 从配置文件读取是否要设置vpu背板输出
算法实现： 
全局变量： 
参    数： void
返 回 值： BOOL32 
日  期        版本        修改人        修改内容
20100111	   v4.6	        彭杰	      create
=============================================================================*/
BOOL32 TVMPCfg::ReadIsNeedBackBrdOut( void )
{
	s32 nValue = 0;
	
	::GetRegKeyInt( VMPCFG_FILE, SECTION_VMPDEBUG, "IsNeedBackBrdOut", 0, &nValue );
	
	return (nValue == 1) ? 1 : 0;
}


//END OF FILE

