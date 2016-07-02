/*****************************************************************************
模   块   名: aud_proc
文   件   名: audio_process.h
相 关  文 件: aud_proc.c, aud_proc_private.h
文件实现功能: 音频处理功能
版        本: V1.0  Copyright(C) 2005-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修 改 记 录:
日       期      版本        修改人      修改内容
2007/09/24       1.0         徐  超      创    建
2009/03/26       1.1         陆志洋      增加平均能量统计和语音激励功能
2009/04/20       1.2         陆志洋      语音激励结构体增加平均能量统计成员变量
2009/11/16       1.3         陆志洋      增加音频处理参数设置函数声明
******************************************************************************/
#ifndef AUDIO_PROCESS_H
#define AUDIO_PROCESS_H
#ifdef __cplusplus
extern "C" {
#endif   /* __cplusplus */

#include "audio_define.h"

enum
{
	ERR_AUDPROC_START = ERR_AUDPROCESS_MOD, 
	/*初始化时的错误码*/
	ERR_AUDPROC_PARAM_POINTER_NULL,         //初始化函数输入参数指针为空
	ERR_AUDPROC_MEM_INIT,                   //内存管理初始化失败
	ERR_AUDPROC_SLOWMEM_LACK,               //内存不足
	ERR_AUDPROC_TAP_TIME,                   //时长错误
	
	ERR_AUDPROC_IN_BIT_WIDTH,               //输入码流位宽错误
	ERR_AUDPROC_IN_CHANNEL,                 //输入码流声道数错误
	ERR_AUDPROC_IN_SAMPLE_RATE,             //输入码流采样率错误
	
	ERR_AUDPROC_OUT_BIT_WIDTH,              //输出码流位宽错误
	ERR_AUDPROC_OUT_CHANNEL,                //输出码流声道数错误
	ERR_AUDPROC_OUT_SAMPLE_RATE,            //输出码流采样率错误
	
	ERR_AUDPROC_NO_SURPORT_INIT,            //初始化中不支持的变换
	ERR_AUDPROC_NO_SURPORT_PROC,            //处理中不支持的变换
	ERR_AUDPROC_NO_MODE,                    //不支持的音频处理模式
	
	/*处理时的错误码*/
	ERR_AUDPROC_PROC_PARA_NULL,             //处理函数输入参数指针为空
	ERR_AUDPROC_PROC_INOUT_BUF_NULL,        //处理函数输入输出BUF指针为空
	ERR_AUDPROC_NO_PROCESS,                 //不支持的音频处理
	ERR_AUDPROC_STATUS_NULL,                //获取状态函数输入参数指针为空
	
	ERR_AUDPROC_VER_BUF_SMALL,              //版本buf太小
	ERR_AUDPROC_CLOSE,                      //句柄关闭失败
	
	//v200新添加的错误码
	ERR_AUDPROC_BIT_WIDTH,                 //pcm信号位宽错误
	ERR_AUDPROC_CHANNEL,                   //pcm信号声道数错误
	ERR_AUDPROC_INIT_HANDLE_NULL,           //初始化函数输入参数指针为空
	ERR_AUDPROC_SLOWMEM,                   //片外内存分配失败
	ERR_AUDPROC_BITWIDTH_CHAN_ATTRIB,       //不支持单声道32bit的码流属性,不支持32bit－32bit的变换	
	ERR_AUDPROC_BITWIDTH_CHAN_MODE,         //位宽声道模式错误
	ERR_AUDPROC_INIT_CLOSE,                //采样率变换初始化错误时释放音频处理分配的内存空间错误
	ERR_AUDPROC_PROC_HANDLE_NULL,           //处理函数输入参数指针为空
	ERR_AUDPROC_CLOSE_RESMPLE,              //释放采样率变换模块的失败
	ERR_AUDPSPLIT_INPUTBUF_SMALL,           //一路分四路输入或输出内存空间过小 
	ERR_AUDPSPLIT_INFRAMESIZE,              //输入帧大小必须是8的整数倍
	ERR_AUDPROC_AVG_POW_TAUDPOWERSTAT_NULL, //TAUDPOWERSTAT结构体指针为空
	ERR_AUDPROC_AVG_POW_MODE_CHECK,         //平均能量处理模式检测失败
    ERR_SPHEXICT_OPEN_FAIL,                 //语音激励打开失败
	ERR_AUDPROC_SPH_EXCIT_TAUDSPHEXCIT_NULL,//TAUDSPHEXCIT结构体指针为空
	ERR_AUDPROC_SPH_EXCIT_MODE_CHECK,       //语音激励处理模式检测失败
	ERR_SPHEXICT_CLOSE_FAIL,                //语音激励关闭失败

	//v201新添加的错误码
	ERR_AUDPROC_INVALID_VOLUME_INDEX,       //无效的音量索引
    ERR_AUDPROC_INFRAMESIZE                 //pcm信号帧size的错误    
};

//音频处理模式
typedef enum
{
	AUDPROC_BW_CH_SRC,                      //声道位宽及采样率变换
	AUDPROC_AVG_POW,                        //平均能量统计
	AUDPROC_SPH_EXCIT,                      //语音激励
	AUDPROC_SPIT1TO4                        //一路采集信号分成四路信号
}TEnumAuddProcMode;                  

//音频处理初始化输入参数结构
typedef struct tagAudProcParam
{
	u32 u32Size ;                            //记录结构体空间大小
	//公用参数(三个ID都要设置)
	u32 u32AudioProcMode;                   //输入音频处理模式 TEnumAuddProcMode
	                                        
	l32 l32InFrameSize;                     //输入PCM数据尺寸(单位:字节)

	//u32AudioProcMode为AUDPROC_BWCHSRC时需设置的参数
	TEnumAudSampleRate tInSamRate;          //输入PCM数据采样率模式
	TEnumAudBitWidth tInBitWidth;           //输入PCM位宽模式
	l32 l32InChannelNum;                    //输入声道模式
	
	TEnumAudSampleRate tOutSamRate;         //输出PCM数据采样率模式
	TEnumAudBitWidth tOutBitWidth;          //输出PCM位宽模式
	l32 l32OutChannelNum;                   //输出声道模式

	u32 u32Reserved;                        //保留参数
}TAudProcParam;

//音频处理状态参数结构
#define TAudProcStatusParam TAudProcParam

//音频处理帧处理输入参数结构
typedef struct tagAudProcInput
{
	u32 u32Size ;                            //记录结构体空间大小
	u8 *pu8InFrameBuf;	                  	//指向一帧输入码流指针
	void *pvOutFrameBuf;	                  //指向一帧处理输出结果的指针
	u32 u32InBufSize;                        //输入缓冲的大小
	u32 u32OutBufSize;                       //输出缓冲的大小
	l32 l32InVolIndex;                      //输入音量索引(如果输入码流是32bit位宽,则该音量索引不起作用)
	l32 l32OutVolIndex;                     //输出音量索引
	u32 u32Reserved;                        //保留参数
}TAudProcInput;

//音频处理帧处理输出参数结构
typedef struct tagAudProcOutput
{
	u32 u32Size;                         //记录结构体空间大小
	u32 u32OutFrameSize;	             //一帧输出码流字节数
	l32 l32StatisCycles;                 //效率统计输出
	u32 u32Reserved;                     //保留参数
}TAudProcOutput;

//音频能量统计结构体(该结构体指针通过TAudProcOutput结构体的u32Reserved变量输入)
typedef struct tagAudPowerStat
{
	u32 u32Size;                           //记录结构体空间大小
	u32 u32AudioProcMode;                  //输入平均能量统计处理模式(TEnumAuddProcMode);	         
	l32 l32AvgPower;                       //输出一帧平均能量统计
	u32 u32Reserved;                       //保留参数
}TAudPowerStat;

//音频语音激励结构体(该结构体指针通过TAudProcOutput结构体的u32Reserved变量输入)
typedef struct tagAudSphExcit
{
	u32 u32Size;                           //记录结构体空间大小
	u32 u32AudioProcMode;                  //输入语音激励处理模式(TEnumAuddProcMode);	         
	l32 l32SphExcitFlag;                   //输出当前帧语音激励标志(1表示语音，0表示非语音)
	u32 u32AvgPower;                       //输出当前帧平均能量统计
	u32 u32Reserved;                       //保留参数
}TAudSphExcit;

//音频自动增益控制结构体(该结构体指针通过TAudProcInput结构体的u32Reserved变量输入)
typedef struct tagAudAGC
{
	u32 u32Size;                           //记录结构体空间大小
	u32 u32AgcEnable;                      //是否进行AGC处理(1表示是，0表示否);	         
	u32 u32Reserved;                       //保留参数
}TAudAgc;

/*===============================================================
函   数   名: AudProcOpen
功        能: 音频处理打开
引用全局变量: 
输入参数说明: 无
              ppvAudProcHandle     音频处理句柄指针的指针[out] 
			  ptAudProcParam       初始化参数结构指针[in] 
			  ptAudProcMemParam    内存管理结构体的指针[in]
返回 值 说明: 
              成功: AUDIO_SUCCESS
              失败: 错误码
特 殊  说 明: 无
=================================================================*/
l32 AudProcOpen(void **ppvHandle, TAudProcParam *ptAudProcParam, void *ptAudProcMemParam);
				
/*===============================================================
函   数   名: AudProcess
功        能: 一帧音频处理
引用全局变量: 无
输入参数说明: 无
              pvHandle             音频处理句柄指针的指针[in]
			  ptAudProcInput       输入参数结构指针[in]
			  ptAudProcOutput      输出参数结构指针[out]
返回 值 说明: 
              成功: AUDIO_SUCCESS
              失败: 错误码
特 殊  说 明: 无
=================================================================*/
l32 AudProcess(void *pvHandle, TAudProcInput *ptAudProcInput,
			   TAudProcOutput *ptAudProcOutput);

/*===============================================================
函   数   名: AudProcGetStatus
功        能: 获取状态
引用全局变量: 无
输入参数说明: 无
              pvHandle              音频处理句柄指针的指针[in]
              ptAudProcStatusParam  音频处理状态信息结构的指针[out]
返回 值 说明: 
              成功: AUDIO_SUCCESS
              失败: 错误码
特 殊  说 明: 无
=================================================================*/
l32 AudProcGetStatus(void *pvHandle, TAudProcStatusParam *ptAudProcStatusParam);

/*===============================================================
函   数   名: AudProcGetVersion
功        能: 取得音频处理模块版本号
引用全局变量: 无
输入参数说明: 无
              pvVer      获取版本号输入缓存[in]
			  l32BufLen  输入缓存大小[out]
			  pl32VerLen 版本号字符串大小[in]
返回 值 说明: 
			  成功: AUDIO_SUCCESS
			  失败: 错误码
特 殊  说 明: 无
=================================================================*/
l32 AudProcGetVersion (void *pvVer, l32 l32BufLen, l32 *pl32VerLen);

/*===============================================================
函   数   名: AudProcClose
功        能: 音频处理关闭
引用全局变量: 无
输入参数说明: 无
              pvHandle        音频处理句柄指针的指针[in/out] 
返回 值 说明: 
              成功: AUDIO_SUCCESS
              失败: 错误码
特 殊  说 明: 无
=================================================================*/
l32 AudProcClose(void *pvHandle);

/*====================================================================                        
函数名      : AudProcSetParams                                                                 
功能        : 设置解码器参数                                                                     
算法实现    : 无                                                                              
引用全局变量: 无                                                                              
参数        : pvHandle:         指向音频处理句柄指针[in]
              ptAudProcParam:   音频处理初始化参数结构体指针[in]                          
返回值说明  : 成功: AUDIO_SUCCESS, 失败: 返回错误码                                           
====================================================================*/                        
l32 AudProcSetParams(void *pvHandle, TAudProcParam *ptAudProcParam);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif   //end of AUDIO_PROCESS_H
