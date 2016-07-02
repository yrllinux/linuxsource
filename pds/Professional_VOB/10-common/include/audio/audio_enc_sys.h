/*****************************************************************************
模   块   名: 音频库davinci封装
文   件   名: audio_enc_sys.h
相 关  文 件: 
文件实现功能: 编码类相关结构体类型定义
版        本: V1.0  Copyright(C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
修 改 记 录:
日       期      版本        修改人      修改内容
 2007/01/23      1.0         徐  超      创    建
******************************************************************************/
#ifndef AUDIO_ENC_SYS_H
#define AUDIO_ENC_SYS_H

#include "audio_com_sys.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************/
/*             音频编码Control ID号定义            */
/***************************************************/
enum XDM_AudioEncId{
    AECENC_RESET_BUF=0,                /*AEC 缓存复位*/
	AECENC_SET_TYPE ,                  /*AEC类型重设, 包括缓存复位等*/
	AUDPROCENC_RESET_BWCHSRC,          /*位宽声道采样率重设的id*/
	AUDPROCENC_RESET_AVGPOW,           /*平均能统计重设的id*/
	AUDPROCENC_RESET_SPHEXCIT,         /*语音激励重设的id*/
	GETENCVERSION,                     /*得到编码版本号*/
	GETENC_STATUS                      //2007-07-26 xc add
}; 

//2007-8-13 xc add
#define  AUDPROCENC_BWCHSRC          AUDPROCENC_RESET_BWCHSRC
#define  AUDPROCENC_AVGPOW           AUDPROCENC_RESET_AVGPOW
#define  AUDPROCENC_SPHEXCIT         AUDPROCENC_RESET_SPHEXCIT

/***************************************************/
/*             编码相关结构体类型                  */
/***************************************************/
//编码参数结构
typedef struct tagAUDIOENC_KDC_Params 
{
	IAUDENC_Params tAudEncParams;      //MUST be first field of all XDAS algs
	TEnumAdCodecMode tAudioMode;       //音频模式
	TAudCodecSubType tAudCodecSubType; //音频模式的子类型   2007-7-16 xc add

	XDAS_Int32 l32FrameSize;           //每帧输入PCM数据尺寸(单位:字节)
	TEnumAdSrateMode tSamRate;         //输入PCM数据采样率模式
	TEnumAdBitWthMode tBitWidth;       //输入PCM位宽模式
	XDAS_Int32 l32ChannelNum;          //输入声道模式
  XDAS_Int32 l32BitRate;             //输入编码码率 2008-08-14 lzy add
	//XDAS_Int32 ptExtendStruct;       //2007-07-26 xc add
} TAUDIOENC_KDC_Params;

//编码输入参数结构
typedef struct tagAUDIOENC_KDC_InArgs 
{
	IAUDENC_InArgs tAudEncInArgs;      //MUST be first field of all XDAS algs 

	XDAS_Int32 l32VolIdx;              //调节音量的索引
	XDAS_Int32 l32Mp3HuffEncFlag;      //仅用于mp3
	/*
	  如是mp3huffman编码,则l32Mp3HuffEncFlag = 1否则为0
	  注意: 每次输入码流尺寸为(2304 * 4 = 9216)
	*/
	XDAS_Int32 l32AvgPowOutFlag;	   //平能能量输出标志(1:需要输出; 0:不需要输出) 

} TAUDIOENC_KDC_InArgs;

//编码动态参数结构
typedef struct tagAUDIOENC_KDC_DynamicParams 
{
	IAUDENC_DynamicParams  tAudEncDynamicParams;      // MUST be first field of all XDAS algs 
	XDAS_UInt32             u32VerBuf;                //  the buffer first_address of  the  version string 
	XDAS_Int32              l32InAudStrLen;           // the biggest length  of  the version string 

} TAUDIOENC_KDC_DynamicParams;

//编码状态参数结构
typedef struct tagAUDIOENC_KDC_Status 
{
	IAUDENC_Status    tAudEncStatus;                 // MUST be first field of all XDAS algs              
	XDAS_Int32        l32ErrorCode;                  // 错误码
	XDAS_Int32        l32OutAudStrLen;               // length  of the version string in fact
    
	//2007-07-30 xc add
	TEnumAdCodecMode tAudioMode;                     //音频模式
	TAudCodecSubType tAudCodecSubType;               //音频模式的子类型   2007-7-16 xc add

	XDAS_Int32 l32FrameSize;                         //每帧输入PCM数据尺寸(单位:字节)
	TEnumAdSrateMode tSamRate;                       //输入PCM数据采样率模式
	TEnumAdBitWthMode tBitWidth;                     //输入PCM位宽模式
	XDAS_Int32 l32ChannelNum;                        //声道模式

} TAUDIOENC_KDC_Status;

//编码输出参数结构
typedef struct tagAUDIOENC_KDC_OutArgs 
{
	IAUDENC_OutArgs tAudEncOutArgs;                  //MUST be first field of all XDAS algs 

	XDAS_Int32 l32OutSize;                           //输出码流大小 
	XDAS_Int32 l32AvgPow;	                         //平能能量

	XDAS_Int32 l32StatCycles;                        //2007-07-27 xc add

} TAUDIOENC_KDC_OutArgs;

/***************************************************/
/*              混音相关结构体类型                 */
/***************************************************/
//混音参数结构
typedef struct tagAUDIOMIX_KDC_Params
{
	IAUDENC_Params tAudEncParams;      // MUST be first field of all XDAS algs
	XDAS_Int32 l32FrameSize;           //每路每帧输入数据尺寸(单位:字节)
} TAUDIOMIX_KDC_Params;

//混音输入参数结构
typedef struct tagAUDIOMIX_KDC_InArgs 
{
	IAUDENC_InArgs tAudEncInArgs;      //MUST be first field of all XDAS algs 

	TEnumAdMixMode eAudioMixMode;      //音频混音模式
	XDAS_Int32 l32MixChNum;            //混音路数
	XDAS_Int16 as16Val[MAX_MIX_CH_NUM];//每路音量数组

} TAUDIOMIX_KDC_InArgs;

/***************************************************/
/*             音频变换相关结构体类型              */
/***************************************************/
//音频变换参数结构
typedef struct tagAUDIOPROC_KDC_Params
{
	IAUDENC_Params tAudProcParams;     //MUST be first field of all XDAS algs

	//公用参数(三个ID都要设置)
	XDAS_UInt32  u32AudioProcFlag;     /*
									     音频变换掩码标志, 取以下三个宏的值:
										 AUDPROCENC_BWCHSRC  位宽声道采样率处理标志
										 AUDPROCENC_AVGPOW   平均能统计标志
										 AUDPROCENC_SPHEXCIT 语音激励处理标志
	                                   */
	XDAS_Int32 l32InFrameSize;         //输入PCM数据尺寸(单位:字节)

	//u32AudioProcFlag为AUDPROCENC_BWCHSRC时需设置的参数
	TEnumAdSrateMode tInSamRate;       //输入PCM数据采样率模式
	TEnumAdBitWthMode tInBitWidth;     //输入PCM位宽模式
	XDAS_Int32 l32InChannelNum;        //输入声道模式
	
	TEnumAdSrateMode tOutSamRate ;     //输出PCM数据采样率模式
	TEnumAdBitWthMode tOutBitWidth;    //输出PCM位宽模式
	XDAS_Int32 l32OutChannelNum;       //输出声道模式

} TAUDIOPROC_KDC_Params;

//音频变换输入参数结构
typedef struct tagAUDIOPROC_KDC_InArgs 
{
	IAUDENC_InArgs tAudEncInArgs;      //MUST be first field of all XDAS algs 

	XDAS_Int32 l32InVolIndex;          //输入音量索引(如果输入码流是32bit位宽,则该音量索引不起作用)
	XDAS_Int32 l32OutVolIndex;         //输出音量索引
} TAUDIOPROC_KDC_InArgs;

//音频变换状态参数结构
typedef struct tagtAUDIOPROC_KDC_Status 
{
	IAUDENC_Status tAudEncStatus;      //MUST be first field of all XDAS algs
	XDAS_Int32 l32ErrorCode;           //错误码
	 
	XDAS_UInt32 u32AudioProcFlag;      //音频变换掩码标志
	XDAS_Int32 l32InFrameSize;         //输入PCM数据尺寸(单位:字节)

	//输入输出码流参数
	TEnumAdSrateMode tInSamRate;       //输入PCM数据采样率模式
	TEnumAdBitWthMode tInBitWidth;     //输入PCM位宽模式
	XDAS_Int32 l32InChannelNum;        //输入声道模式
	
	TEnumAdSrateMode tOutSamRate;      //输出PCM数据采样率模式
	TEnumAdBitWthMode tOutBitWidth;    //输出PCM位宽模式
	XDAS_Int32 l32OutChannelNum;       //输出声道模式

} TAUDIOPROC_KDC_Status;

//音频变换动态参数结构
typedef struct tagAUDIOPROC_KDC_DynamicParams 
{
	IAUDENC_DynamicParams  tAudEncDynamicParams;      // MUST be first field of all XDAS algs 
	//公用参数(三个ID都要设置)
	XDAS_UInt32  u32AudioProcFlag;     /*
									     音频变换掩码标志, 取以下三个宏的值:
										 AUDPROCENC_BWCHSRC  位宽声道采样率处理标志
										 AUDPROCENC_AVGPOW   平均能统计标志
										 AUDPROCENC_SPHEXCIT 语音激励处理标志
	                                   */
	XDAS_Int32 l32InFrameSize;         //输入PCM数据尺寸(单位:字节)

	//ID为AUDPROCENC_RESET_BWCHSRC时需设置的参数
	TEnumAdSrateMode tInSamRate;       //输入PCM数据采样率模式
	TEnumAdBitWthMode tInBitWidth;     //输入PCM位宽模式
	XDAS_Int32 l32InChannelNum;        //输入声道模式
	
	TEnumAdSrateMode tOutSamRate ;     //输出PCM数据采样率模式
	TEnumAdBitWthMode tOutBitWidth;    //输出PCM位宽模式
	XDAS_Int32 l32OutChannelNum;       //输出声道模式

} TAUDIOPROC_KDC_DynamicParams;

//音频变换输出参数结构
typedef struct tagAUDIOPROC_KDC_OutArgs 
{
	IAUDENC_OutArgs tAudEncOutArgs;    //MUST be first field of all XDAS algs 

	XDAS_Int32 l32OutSize;             //输出码流大小 
	XDAS_Int32 l32AvgPow;	           //平能能量
	XDAS_Int32 l32ExciteLev;           //激励级别 
	XDAS_Int32 l32StatCycles;          //2007-08-13 xc add

} TAUDIOPROC_KDC_OutArgs;

/***************************************************/
/*             回声抵消相关结构体类型              */
/***************************************************/
//AEC参数结构
typedef struct tagAUDIOAEC_KDC_Params
{
	IAUDENC_Params tAudAecParams;      // MUST be first field of all XDAS algs
	
	//是否需要,需要时只调节近端采信号吗?
	XDAS_Int32 l32VolIdx;              //调节音量索引

	//AEC处理时编解码类型
	XDAS_Int32 l32AecType;             //eg. AEC_NEW_TYPE_MP3, AEC_NEW_TYPE_G711 etc

	//输入参考信号属性
	XDAS_Int32 l32RefInFrameSize;      //参考信号输入PCM数据尺寸(单位:字节)
	TEnumAdSrateMode tRefInSamRate;    //参考信号输入PCM数据采样率模式
	TEnumAdBitWthMode tRefInBitWidth;  //参考信号输入PCM位宽模式
	XDAS_Int32 l32RefInChannelNum;     //参考信号声道模式
	
	//输入近端信号属性
	XDAS_Int32 l32NearInFrameSize;     //近端信号输入PCM数据尺寸(单位:字节)
	TEnumAdSrateMode tNearInSamRate;   //近端信号输入PCM数据采样率模式
	TEnumAdBitWthMode tNearInBitWidth; //近端信号输入PCM位宽模式
	XDAS_Int32 l32NearInChannelNum;    //近端信号声道模式

}TAUDIOAEC_KDC_Params;

//AEC输入参数结构
typedef struct tagAUDIOAEC_KDC_InArgs 
{
	IAUDENC_InArgs tAudEncInArgs;      //MUST be first field of all XDAS algs 

	TEnumAdAecMode  tAdAecMode;        //音频AEC模式
	XDAS_Int32 l32CapSamples;          //已采集未处理的样本数
	XDAS_Int32 l32PlySamples;          //未播放的样本数
	XDAS_Int32 l32UseFlag;             //aec_spe_agc_16k使用
	                                   //bit2=1表示aec的使用，bit1=1表示spe的使用，bit0=1表示agc的使用
} TAUDIOAEC_KDC_InArgs;

typedef struct tagAUDIOAEC_KDC_DynamicParams 
{
	IAUDENC_DynamicParams  tAudEncDynamicParams;      // MUST be first field of all XDAS algs 
	/*
	Control ID = AECENC_RESET_BUF时,需要设置的参数:
	无
	*/
	//Control ID = AECENC_SET_TYPE,需要设置的参数:
	XDAS_Int32 l32AecType;             //eg. AEC_NEW_TYPE_MP3, AEC_NEW_TYPE_G711 etc

	//输入参考信号属性
	XDAS_Int32 l32RefInFrameSize;      //参考信号输入PCM数据尺寸(单位:字节)
	TEnumAdSrateMode tRefInSamRate;    //参考信号输入PCM数据采样率模式
	TEnumAdBitWthMode tRefInBitWidth;  //参考信号输入PCM位宽模式
	XDAS_Int32 l32RefInChannelNum;     //参考信号声道模式
	
	//输入近端信号属性
	XDAS_Int32 l32NearInFrameSize;     //近端信号输入PCM数据尺寸(单位:字节)
	TEnumAdSrateMode tNearInSamRate;   //近端信号输入PCM数据采样率模式
	TEnumAdBitWthMode tNearInBitWidth; //近端信号输入PCM位宽模式
	XDAS_Int32 l32NearInChannelNum;    //近端信号声道模式
	
	XDAS_UInt32             u32VerBuf;                //  the buffer first_address of  the  version string 
	XDAS_Int32              l32InAudStrLen;               // the biggest length  of  the version string 

} TAUDIOAEC_KDC_DynamicParams;

/***************************************************/
/*             DTMF相关结构体类型                  */
/***************************************************/
//DTMF输入参数结构
typedef struct tagDTMFENC_KDC_InArgs 
{
	IAUDENC_InArgs tAudEncInArgs;      //MUST be first field of all XDAS algs 

    XDAS_Int32 l32DTMFSampleLen;	   //DTMF音持续时间,8K采样的点数, 大于480(即60ms时长)
    XDAS_Int32 l32DTMFZeroLen;		   //DTMF音间隔时间,8K采样的点数, 大于480(即60ms时长)
	XDAS_UInt16 u16DTMFNum;            //需要进行DTMF编码的号码位数
	
} TDTMFENC_KDC_InArgs;


/***************************************************/
/*             SPLIT相关结构体类型                 */
/***************************************************/
typedef struct tagAUDIOSPLIT_KDC_InArgs 
{
	IAUDENC_InArgs tAudEncInArgs;      //MUST be first field of all XDAS algs 

	XDAS_Int32 l32InSize;              //输入码流的大小,以字节为单位,必须为8的倍数
	
} TAUDIOSPLIT_KDC_InArgs;


#ifdef __cplusplus
}
#endif

#endif




