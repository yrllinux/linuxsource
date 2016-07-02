/*****************************************************************************
模   块   名: aud_proc
文   件   名: audio_proc_xdm6437.h
相 关  文 件: audio_process.h, audio_def.h
文件实现功能: 音频处理功能
版        本: V1.0  Copyright(C) 2005-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修 改 记 录:
日       期      版本        修改人      修改内容
 2007/09/24      1.0         徐  超      创    建
******************************************************************************/
#ifndef AUDIO_PROC_XDM6437_H
#define AUDIO_PROC_XDM6437_H
#ifdef __cplusplus
extern "C" {
#endif   /* __cplusplus */

/***************************************************/
/*             音频处理Control ID号定义            */
/***************************************************/
enum XDM_AudioProId
{
	GETPRO_VERSION = 0,                 //得到音频处理版本号
	GETPRO_STATUS,                      //得到音频处理状态             
}; 


/***************************************************/
/*             音频变换相关结构体类型              */
/***************************************************/
//音频变换参数结构
typedef struct tagAUDIOPROC_KDC_Params
{
	IAUDENC_Params tAudProcParams;			//MUST be first field of all XDAS algs
	XDAS_UInt32 u32AudProcParamAddr;
} TAUDIOPROC_KDC_Params;

//音频变换输入参数结构
typedef struct tagAUDIOPROC_KDC_InArgs 
{
	IAUDENC_InArgs tAudEncInArgs;			//MUST be first field of all XDAS algs
	TAudProcInput  tAudProcInput;
} TAUDIOPROC_KDC_InArgs;

//音频变换输出参数结构
typedef struct tagAUDIOPROC_KDC_OutArgs 
{
	IAUDENC_OutArgs tAudEncOutArgs;			//MUST be first field of all XDAS algs 
  TAudProcOutput  tAudProcOutput;     //      
} TAUDIOPROC_KDC_OutArgs;

//音频变换状态参数结构
typedef struct tagtAUDIOPROC_KDC_Status 
{
	IAUDENC_Status tAudEncStatus;			//MUST be first field of all XDAS algs
} TAUDIOPROC_KDC_Status;

//音频变换动态参数结构
typedef struct tagAUDIOPROC_KDC_DynamicParams 
{
	IAUDENC_DynamicParams  tAudEncDynamicParams;      // MUST be first field of all XDAS algs 
	XDAS_UInt32 u32AudProcDynParamAddr;
	XDAS_UInt32 u32AudProcStatusParamAddr;
} TAUDIOPROC_KDC_DynamicParams;

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif   //end of AUDIO_PROC_XDM6437_H 



