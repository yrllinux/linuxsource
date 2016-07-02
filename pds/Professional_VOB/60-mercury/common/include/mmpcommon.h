
#ifndef _MMP_COMMON_H_
#define _MMP_COMMON_H_

#define SOFT_CODEC
#define TYPE_SCODE
#include "codecwrapper_common.h"


const u16 g_tblVidEncParam[6][6]=
{
    //类型               压缩码率控制参数 最大关键帧 最大量化 最小量化    图像质量
    
    //mpeg4
    MEDIA_TYPE_MP4,           0,            3000,        31,      2,       QUALITY_PRIOR,
    //261
    MEDIA_TYPE_H261,          0,            300,        31,      2,       QUALITY_PRIOR,
    //262(mpeg-2)
    MEDIA_TYPE_H262,          0,            300,        31,      2,       QUALITY_PRIOR,
    //263
    MEDIA_TYPE_H263,          1,            3000,        31,      3,       QUALITY_PRIOR,
    //h263+
    MEDIA_TYPE_H263PLUS,      0,            75,        31,      2,       QUALITY_PRIOR,
    //h264
    MEDIA_TYPE_H264,          0,            3000,       45,      20,      SPEED_PRIOR
};

inline void GetDefaultEncParam(u8 byMediaType,TVidEncParam& tEncParam)
{
    s8 CFG_FILE[KDV_MAX_PATH] = {0};
    s8 SECTION_SYS[]      = "EQP";
    s8 SECTION_DEBUG[]    = "EQPDEBUG";

    sprintf(CFG_FILE, "%s/mcueqp.ini", DIR_CONFIG);

    s32 nValue;
	
    switch( byMediaType )
    {
    case MEDIA_TYPE_MP4:
        tEncParam.m_byEncType = (u8)g_tblVidEncParam[0][0];

        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "MPEG4BitrateControlMode", 0, &nValue))
        {
            tEncParam.m_byEncQuality = (u8)g_tblVidEncParam[0][1];
        }
        else
        {
            tEncParam.m_byEncQuality = (u8)nValue;
        }

        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "MPEG4VideoEcdIKeyRate", 0, &nValue))
        {
            tEncParam.m_dwMaxKeyFrameInterval = g_tblVidEncParam[0][2];
        }
        else
        {
            tEncParam.m_dwMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "MPEG4VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncParam.m_byMaxQuant = (u8)g_tblVidEncParam[0][3];
        }
        else
        {
            tEncParam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "MPEG4VideoEcdQualMinValu", 0, &nValue))
        {
            tEncParam.m_byMinQuant = (u8)g_tblVidEncParam[0][4];
        }
        else
        {
            tEncParam.m_byMinQuant = (u8)nValue;
        }

        tEncParam.m_byEncQuality = 0;

        break;

    case MEDIA_TYPE_H261: 
        tEncParam.m_byEncType = (u8)g_tblVidEncParam[1][0];
        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "261BitrateControlMode", 0, &nValue))
        {
            tEncParam.m_byEncQuality = (u8)g_tblVidEncParam[1][1];
        }
        else
        {
            tEncParam.m_byEncQuality = (u8)nValue;
        }

        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "261VideoEcdIKeyRate", 0, &nValue))
        {
            tEncParam.m_dwMaxKeyFrameInterval = g_tblVidEncParam[1][2];
        }
        else
        {
            tEncParam.m_dwMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "261VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncParam.m_byMaxQuant = (u8)g_tblVidEncParam[1][3];
        }
        else
        {
            tEncParam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "261VideoEcdQualMinValu", 0, &nValue))
        {
            tEncParam.m_byMinQuant = (u8)g_tblVidEncParam[1][4];
        }
        else
        {
            tEncParam.m_byMinQuant = (u8)nValue;
        }

        tEncParam.m_byEncQuality = 0;

        break;

    case MEDIA_TYPE_H262:  
        tEncParam.m_byEncType = (u8)g_tblVidEncParam[2][0];
        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "262BitrateControlMode", 0, &nValue))
        {
            tEncParam.m_byEncQuality = (u8)g_tblVidEncParam[2][1];
        }
        else
        {
            tEncParam.m_byEncQuality = (u8)nValue;
        }

        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "262VideoEcdIKeyRate", 0, &nValue))
        {
            tEncParam.m_dwMaxKeyFrameInterval = g_tblVidEncParam[2][2];
        }
        else
        {
            tEncParam.m_dwMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "262VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncParam.m_byMaxQuant = (u8)g_tblVidEncParam[2][3];
        }
        else
        {
            tEncParam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "262VideoEcdQualMinValu", 0, &nValue))
        {
            tEncParam.m_byMinQuant = (u8)g_tblVidEncParam[2][4];
        }
        else
        {
            tEncParam.m_byMinQuant = (u8)nValue;
        }

        tEncParam.m_byEncQuality = 0;

        break;

    case MEDIA_TYPE_H263:  
        tEncParam.m_byEncType = (u8)g_tblVidEncParam[3][0];
        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "263BitrateControlMode", 0, &nValue))
        {
            tEncParam.m_byEncQuality = (u8)g_tblVidEncParam[3][1];
        }
        else
        {
            tEncParam.m_byEncQuality = (u8)nValue;
        }

        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "263VideoEcdIKeyRate", 0, &nValue))
        {
            tEncParam.m_dwMaxKeyFrameInterval = g_tblVidEncParam[3][2];
        }
        else
        {
            tEncParam.m_dwMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "263VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncParam.m_byMaxQuant = (u8)g_tblVidEncParam[3][3];
        }
        else
        {
            tEncParam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "263VideoEcdQualMinValu", 0, &nValue))
        {
            tEncParam.m_byMinQuant = (u8)g_tblVidEncParam[3][4];
        }
        else
        {
            tEncParam.m_byMinQuant = (u8)nValue;
        }

        tEncParam.m_byEncQuality = 0;

        break;

    case MEDIA_TYPE_H263PLUS:  
        tEncParam.m_byEncType = (u8)g_tblVidEncParam[4][0];
        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "263PLUSBitrateControlMode", 0, &nValue))
        {
            tEncParam.m_byEncQuality = (u8)g_tblVidEncParam[4][1];
        }
        else
        {
            tEncParam.m_byEncQuality = (u8)nValue;
        }

        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "263PLUSVideoEcdIKeyRate", 0, &nValue))
        {
            tEncParam.m_dwMaxKeyFrameInterval = g_tblVidEncParam[4][2];
        }
        else
        {
            tEncParam.m_dwMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "263PLUSVideoEcdQualMaxValu", 0, &nValue))
        {
            tEncParam.m_byMaxQuant = (u8)g_tblVidEncParam[4][3];
        }
        else
        {
            tEncParam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "263PLUSVideoEcdQualMinValu", 0, &nValue))
        {
            tEncParam.m_byMinQuant = (u8)g_tblVidEncParam[4][4];
        }
        else
        {
            tEncParam.m_byMinQuant = (u8)nValue;
        }

        tEncParam.m_byEncQuality = 0;

        break;

    case MEDIA_TYPE_H264:  
        tEncParam.m_byEncType = (u8)g_tblVidEncParam[5][0];

        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "264VideoEcdIKeyRate", 0, &nValue))
        {
            tEncParam.m_dwMaxKeyFrameInterval = g_tblVidEncParam[5][2];
        }
        else
        {
            tEncParam.m_dwMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "264VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncParam.m_byMaxQuant = (u8)g_tblVidEncParam[5][3];
        }
        else
        {
            tEncParam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "264VideoEcdQualMinValu", 0, &nValue))
        {
            tEncParam.m_byMinQuant = (u8)g_tblVidEncParam[5][4];
        }
        else
        {
            tEncParam.m_byMinQuant = (u8)nValue;
        }      
    
        if (!::GetRegKeyInt(CFG_FILE, SECTION_SYS, "264SVideoImageQuality", SPEED_PRIOR, &nValue))
        {
            tEncParam.m_byEncQuality = (u8)g_tblVidEncParam[5][5];
        }
        else
        {
            tEncParam.m_byEncQuality = (u8)nValue;
        }

        break;

    default:
        break;
    }

    return;
}

#endif  //_MMP_COMMON_H_