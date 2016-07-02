/*****************************************************************************
模   块   名: 音频库tms320c6437封装
文   件   名: audio_codec_xdm6437.h
相 关  文 件: 
文件实现功能: 编码类相关结构体类型定义
版        本: V1.0  Copyright(C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
修 改 记 录:
日       期      版本        修改人      修改内容
 2007/12/12      1.0         陆志洋     创    建
******************************************************************************/
#ifndef AUDIO_CODEC_XDM6437_H
#define AUDIO_CODEC_XDM6437_H
#ifdef __cplusplus
extern "C" {
#endif

#include "audio_codec.h"
/***************************************************/
/*             音频编码Control ID号定义            */
/***************************************************/
enum XDM_AudioEncId{
	GETENC_VERSION = 0,                  //得到编码版本号
	GETENC_STATUS                       //得到编码状态信息
}; 

/***************************************************/
/*             编码相关结构体类型                  */
/***************************************************/
//编码参数结构
typedef struct tagAUDIOENC_KDC_Params 
{
    IAUDENC_Params tAudEncParams;     //MUST be first field of all XDAS algs
    TAudEncParam tAudEncParam;         //音频编码初使化参数结构体
} TAUDIOENC_KDC_Params;

//编码输入参数结构
typedef struct tagAUDIOENC_KDC_InArgs 
{
    IAUDENC_InArgs tAudEncInArgs;      //MUST be first field of all XDAS algs 
    TAudEncInput  tAudEncInput ;        // 音频编码输入结构体
} TAUDIOENC_KDC_InArgs;

//编码输出参数结构
typedef struct tagAUDIOENC_KDC_OutArgs 
{
    IAUDENC_OutArgs tAudEncOutArgs;       //MUST be first field of all XDAS algs 
    TAudEncOutput  tAudEncOutput;         //音频编码输出结构体
}TAUDIOENC_KDC_OutArgs;

//编码动态参数结构
typedef struct tagAUDIOENC_KDC_DynamicParams 
{
    IAUDENC_DynamicParams  tAudEncDynamicParams;      // MUST be first field of all XDAS algs 
    XDAS_Int8 *             ps8VerBuf;                //  the buffer first_address of  the  version string 
    XDAS_Int32              l32InAudStrLen;           // the biggest length  of  the version string 
    XDAS_Int32              l32OutAudStrLen;          // the real length  of  the version string 
}TAUDIOENC_KDC_DynamicParams;

//编码状态参数结构
typedef struct tagAUDIOENC_KDC_Status 
{
    IAUDENC_Status tAudEncStatus;             // MUST be first field of all XDAS algs              
    TAudEncStatusParam  tAudEncStatusParam;   //音频编码状态结构体
}TAUDIOENC_KDC_Status;

/***************************************************/
/*             音频解码Control ID号定义            */
/***************************************************/
enum XDM_AudioDecId{
    GETDEC_VERSION = 0,                  //得到解码版本号
    GETDEC_STATUS                       //得到解码状态信息
}; 

/***************************************************/
/*             解码相关结构体类型                  */
/***************************************************/
//解码参数结构
typedef struct tagAUDIODEC_KDC_Params
{
    IAUDDEC_Params tAudDecParams;      // MUST be first field of all XDAS algs 
    TAudDecParam  tAudDecParam;         //音频解码初使化参数结构体
}TAUDIODEC_KDC_Params;

//解码输入参数结构
typedef struct tagAUDIODEC_KDC_InArgs 
{
    IAUDDEC_InArgs tAudDecInArgs;      // MUST be first field of all XDAS algs 
    TAudDecInput tAudDecInput;        // 音频解码输入结构体
}TAUDIODEC_KDC_InArgs;

//解码输出参数结构
typedef struct tagAUDIODEC_KDC_OutArgs 
{
    IAUDDEC_OutArgs tAudDecOutArgs;      //MUST be first field of all XDAS algs 
    TAudDecOutput tAudDecOutput;         //音频解码输出结构体
} TAUDIODEC_KDC_OutArgs;

//解码动态参数结构
typedef struct tagAUDIODEC_KDC_DynamicParams 
{
    IAUDDEC_DynamicParams  tAudDECDynamicParams;      // MUST be first field of all XDAS algs 
    XDAS_Int8 *             ps8VerBuf;                 //  the buffer first_address of  the  version string 
    XDAS_Int32              l32InAudStrLen;           // the biggest length  of  the version string 
    XDAS_Int32              l32OutAudStrLen;          // the real length  of  the version string 
}TAUDIODEC_KDC_DynamicParams;

//解码状态参数结构
typedef struct tagAUDIODEC_KDC_Status 
{
    IAUDDEC_Status tAudDecStatus;             // MUST be first field of all XDAS algs              
    TAudDecStatusParam  tAudDecStatusParam ;  //音频解码状态结构体
}TAUDIODEC_KDC_Status;

#ifdef __cplusplus
}
#endif
#endif













