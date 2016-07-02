/*****************************************************************************
模   块   名: aud_aec
文   件   名: audio_aec_xdm6437.h
相 关  文 件: audio_def.h
文件实现功能: 回声抵消功能
版        本: V1.0  Copyright(C) 2005-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修 改 记 录:
日       期      版本        修改人      修改内容
 2007/09/24      1.0         徐  超      创    建
******************************************************************************/
#ifndef AUDIO_AEC_XDM6437_H
#define AUDIO_AEC_XDM6437_H
#ifdef __cplusplus
extern "C" {
#endif   /* __cplusplus */ 

#define AEC8K
//#define AEC16K
//#define AEC32K

//AEC处理模式
typedef enum
{
	AEC_SETREF_MODE=0,                 //参考信号设置模式
	AEC_PROC_MODE                      //回声处理模式
}TEnumAdAecMode;

/***************************************************/
/*             音频aec Control ID号定义            */
/***************************************************/
enum XDM_AudioAecId
{
	GETAEC_VERSION = 0,                 //得到aec版本号
	GETAEC_STATUS,                      //得到aec状态信息
	RESET_AEC                           //重置aec                  
}; 

/***************************************************/
/*             回声抵消相关结构体类型              */
/***************************************************/
//回声抵消参数结构
typedef struct tagAUDIOAEC_KDC_Params
{
	IAUDENC_Params tAudAecParams;			//MUST be first field of all XDAS algs
	XDAS_UInt32 u32AudAecParamAddr;   //AEC初始化参数结构体首地址(TAecParam)
} TAUDIOAEC_KDC_Params;


//回声抵消输入参数结构
typedef struct tagAUDIOAEC_KDC_InArgs 
{
	IAUDENC_InArgs tAudEncInArgs;			//MUST be first field of all XDAS algs
	TAecPlayInput tAecPlayInput;      //Aec播放端输入数据结构体
	TAecCapInput tAecCapInput;        //Aec采集端输入数据结构体
	TEnumAdAecMode tAdAecMode;        //音频AEC模式
} TAUDIOAEC_KDC_InArgs;

//回声抵消输出参数结构
typedef struct tagAUDIOAEC_KDC_OutArgs 
{
	IAUDENC_OutArgs tAudEncOutArgs;			//MUST be first field of all XDAS algs    
} TAUDIOAEC_KDC_OutArgs;

//回声抵消状态参数结构
typedef struct tagtAUDIOAEC_KDC_Status 
{
	IAUDENC_Status tAudEncStatus;			//MUST be first field of all XDAS algs
} TAUDIOAEC_KDC_Status;

//回声抵消动态参数结构
typedef struct tagAUDIOAEC_KDC_DynamicParams 
{
	IAUDENC_DynamicParams  tAudEncDynamicParams;      // MUST be first field of all XDAS algs 
	XDAS_UInt32 u32AudAecDynParamAddr;                /* GETENC_VERSION时为TAudVerInfoInput结构体首地址，
	                                                     RESET_AEC时为TAecParam结构体首地址，其余可以为空*/
	XDAS_UInt32 u32AudAecStatusParamAddr;             /* GETENC_VERSION时为TAecStatusParam结构体首地址，
	                                                     GETENC_STATUS时为TAudVerInfoOutput结构体首地址,其余可以为空*/
} TAUDIOAEC_KDC_DynamicParams;

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif   //end of AUDIO_AEC_XDM6437_H 



