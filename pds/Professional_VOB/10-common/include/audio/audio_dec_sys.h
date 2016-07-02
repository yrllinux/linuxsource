/*****************************************************************************
模   块   名: 音频库davinci封装
文   件   名: audio_dec_sys.h
相 关  文 件: 
文件实现功能: 解码相关结构体类型定义
版        本: V1.0  Copyright(C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
修 改 记 录:
日       期      版本        修改人      修改内容
 2007/01/23      1.0         徐  超      创    建
******************************************************************************/
#ifndef AUDIO_DEC_SYS_H
#define AUDIO_DEC_SYS_H
#include "audio_com_sys.h"
#ifdef __cplusplus
extern "C" {
#endif
	
/***************************************************/
/*             解码相关结构体类型                  */
/***************************************************/
//解码参数结构
typedef struct tagAUDIODEC_KDC_Params
{
	IAUDDEC_Params tAudDecParams;      // MUST be first field of all XDAS algs 
	TEnumAdCodecMode tAudioMode;       //音频CODEC模式
	TAudCodecSubType tAudCodecSubType; //音频模式的子类型   2007-7-24 xc add
	
	XDAS_Int32 l32FrameSize;           //每帧输入码流尺寸(单位:字节)
	TEnumAdSrateMode tSamRate;         //输出PCM数据采样率模式
	TEnumAdBitWthMode tBitWidth;       //输出PCM位宽模式
	XDAS_Int32 l32ChannelNum;          //声道模式
  XDAS_Int32 l32BitRate;             //输入解码码率  2008-08-14 lzy add
	XDAS_Int32 l32AecFlag;             //AEC使能 = 1 否则 = 0;(1:有第二个buf传给process)

}TAUDIODEC_KDC_Params;

//解码输入参数结构
typedef struct tagAUDIODEC_KDC_InArgs 
{
	IAUDDEC_InArgs tAudDecInArgs;      // MUST be first field of all XDAS algs 

	XDAS_Int32 l32VolIdx;              //调节音量的索引

	XDAS_Int32 l32Mp3HuffDecFlag;      //仅用于mp3
	/*
	  如是mp3huffman解码,l32Mp3HuffDecFlag = 1否则为0
	  注意: 每次输出码流尺寸为(2304 * 4 = 9216)
	*/
	XDAS_Int32 l32AvgPowOutFlag;	   //平能能量输出标志(1:需要输出; 0:不需要输出) 

} TAUDIODEC_KDC_InArgs;


//解码动态参数结构
typedef struct tagAUDIODEC_KDC_DynamicParams 
{
	IAUDDEC_DynamicParams  tAudDecDynamicParams;      //MUST be first field of all XDAS algs 
	XDAS_UInt32             u32VerBuf;                //the buffer first_address of  the  version string 
	XDAS_Int32              l32InAudStrLen;           //the biggest length  of  the version string 
	
} TAUDIODEC_KDC_DynamicParams;

//解码状态参数结构
typedef struct tagAUDIODEC_KDC_Status 
{
	IAUDDEC_Status    tAudDecStatus;                 // MUST be first field of all XDAS algs              
	XDAS_Int32        l32ErrorCode;                  //错误码
	XDAS_Int32        l32OutAudStrLen;               //  length  of the version string in fact

	//2007-07-30 xc add
	/*
	*/
	TEnumAdCodecMode tAudioMode;       //音频CODEC模式
	TAudCodecSubType tAudCodecSubType; //音频模式的子类型   2007-7-24 xc add
	
	XDAS_Int32 l32FrameSize;           //每帧输入码流尺寸(单位:字节)
	TEnumAdSrateMode tSamRate;         //输出PCM数据采样率模式
	TEnumAdBitWthMode tBitWidth;       //输出PCM位宽模式
	XDAS_Int32 l32ChannelNum;          //声道模式
	XDAS_Int32 l32AecFlag;             //AEC使能 = 1 否则 = 0;(1:有第二个buf传给process)

} TAUDIODEC_KDC_Status;

//解码输出参数结构
typedef struct tagAUDIODEC_KDC_OutArgs 
{
	IAUDDEC_OutArgs tAudDecOutArgs;                  //MUST be first field of all XDAS algs 

	XDAS_Int32 l32OutSize;                           //输出码流大小 
	XDAS_Int32 l32AvgPow;	                         //平能能量

	XDAS_Int32 l32StatCycles;                        //2007-07-27 xc add

} TAUDIODEC_KDC_OutArgs;

/***************************************************/
/*             音频解码Control ID号定义              */
/***************************************************/
enum XDM_AudioDecId
{
	GETDECVERSION,                     /*得到解码版本号*/
	GETDEC_STATUS                     //2007-07-26 xc add
};
#ifdef __cplusplus
}
#endif

#endif





