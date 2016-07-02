/*****************************************************************************
模   块   名: aud_mix
文   件   名: audio_mix_xdm6437.h
相 关  文 件: audio_process.h, audio_def.h
文件实现功能: 音频处理功能
版        本: V1.0  Copyright(C) 2005-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修 改 记 录:
日       期      版本        修改人      修改内容
 2007/09/24      1.0         徐  超      创    建
******************************************************************************/
#ifndef AUDIO_MIX_XDM6437_H
#define AUDIO_MIX_XDM6437_H
#ifdef __cplusplus
extern "C" {
#endif   /* __cplusplus */

/***************************************************/
/*             音频混音器Control ID号定义            */
/***************************************************/
enum XDM_AudioMixerId
{
	GETMIXER_VERSION = 0,                 //得到混音器版本号
	GETMIXER_STATUS,                      //得到混音器状态             
}; 

/***************************************************/
/*             混音器相关结构体类型              */
/***************************************************/
//音频混音参数结构
typedef struct tagAUDIOMIX_KDC_Params
{
	IAUDENC_Params tAudMixParams;			//MUST be first field of all XDAS algs

	XDAS_UInt32 u32AudMixParamAddr;
} TAUDIOMIX_KDC_Params;

//音频混音输入参数结构
typedef struct tagAUDIOMIX_KDC_InArgs 
{
	IAUDENC_InArgs tAudEncInArgs;			//MUST be first field of all XDAS algs
	TEnumAudMixMode tAudMixMode;            //音频混音模式

	XDAS_Int32 l32FrameSize;                //每路每帧输入数据尺寸(单位:字节)
	XDAS_Int32 l32MixChNum;                 //混音路数
	XDAS_Int16 as16Val[AUDMIX_MAX_CHANNEL]; //每路音量数组
    u32 u32Reserved;                        //保留参数
	/*
	TAudMixInput  tAudMixInput;
	TEnumAudMixMode tAudMixMode;
	                              //混音模式
	TAudMixBuf **pptInAudBuf;     //指向混音器输入结构体指针数组
	u32 u32InChList;              //混音输入通道列表 
    TAudMixBuf **pptOutAudBuf;    //指向混音器N-1模式输出结构体指针数组
    TAudMixBuf *ptOutNAudBuf;     //N模式输出结构体指针
    u32 u32InSampleCnt;           //每路混音样本数
    u32 u32Reserved;                        //保留参数
	*/

} TAUDIOMIX_KDC_InArgs;

//音频混音输出参数结构
typedef struct tagAUDIOMIX_KDC_OutArgs 
{
	IAUDENC_OutArgs tAudEncOutArgs;			//MUST be first field of all XDAS algs 
    TAudMixOutput  tAudMixOutput;         
} TAUDIOMIX_KDC_OutArgs;

//音频混音状态参数结构
typedef struct tagtAUDIOMIX_KDC_Status 
{
	IAUDENC_Status tAudEncStatus;			//MUST be first field of all XDAS algs

} TAUDIOMIX_KDC_Status;

//音频混音动态参数结构
typedef struct tagAUDIOMIX_KDC_DynamicParams 
{
	IAUDENC_DynamicParams  tAudEncDynamicParams;      // MUST be first field of all XDAS algs 
	XDAS_UInt32 u32AudMixDynParamAddr;
	XDAS_UInt32 u32AudMixStatusParamAddr;
} TAUDIOMIX_KDC_DynamicParams;

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif   //end of AUDIO_MIX_XDM6437_H 



