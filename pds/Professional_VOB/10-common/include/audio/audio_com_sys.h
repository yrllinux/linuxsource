/*****************************************************************************
模   块   名: 音频库davinci封装
文   件   名: audio_com_sys.h
相 关  文 件: 
文件实现功能: 公用类型定义
版        本: V1.0  Copyright(C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
修 改 记 录:
日       期      版本        修改人      修改内容
 2007/01/23      1.0         徐  超      创    建
******************************************************************************/
#ifndef AUDIO_COM_SYS_H
#define AUDIO_COM_SYS_H

#ifdef __cplusplus
extern "C" {
#endif

//最大混音路数
#define MAX_MIX_CH_NUM      32

/*音频变换的宏定义(掩码)*/         
#define   PROC_BW_MASK          (1)    //位宽变换
#define   PROC_CH_MASK          (1<<1) //声道变换
#define   PROC_RES_MASK         (1<<2) //采样率变换 note
#define   PROC_VOL_MASK         (1<<3) //音量调节
#define   PROC_POW_MASK         (1<<4) //平能能量统计
#define   PROC_EXCIT_MASK       (1<<5) //语音激励 note

/*
//音频变换的采样率变换模式
typedef enum
{
	PROC_SRC_48K_8K=0,
	PROC_SRC_8K_48K,
	PROC_SRC_48K_16K,
	PROC_SRC_16K_48K,
	PROC_SRC_48K_32K,
	PROC_SRC_32K_48K
}TEnumAdSRCMode;
*/
//Aec type
#ifndef AEC_NEW_TYPE_MP3
#define AEC_NEW_TYPE_MP3	2
#endif

#ifndef AEC_NEW_TYPE_G711
#define AEC_NEW_TYPE_G711	3
#endif

#ifndef AEC_NEW_TYPE_G722
#define AEC_NEW_TYPE_G722	4
#endif

#ifndef AEC_NEW_TYPE_G729
#define AEC_NEW_TYPE_G729	5
#endif

#ifndef AEC_NEW_TYPE_G7221C
#define AEC_NEW_TYPE_G7221C	6
#endif

//比特位宽模式
typedef enum
{
	BIT16_MODE = 16,
	BIT32_MODE = 32
}TEnumAdBitWthMode;

//音频采样率模式
typedef enum
{
	SRATE_8K = 8000,
	SRATE_16K = 16000,
	SRATE_32K = 32000,
	SRATE_48K = 48000
}TEnumAdSrateMode;

//音频CODEC模式
typedef enum
{
	G711a_CODEC=0,
	G711u_CODEC,
	G7231_CODEC,
	G728_CODEC,
	G729_CODEC,
	ADPCM_CODEC,
	G722_CODEC,
	G7221C_CODEC,
	MP3_CODEC, 
	AAC_CODEC    //2007-7-17 xc add
}TEnumAdCodecMode;


//2007-7-17 xc add
typedef enum
{
	AAC_16k_MONO = 0,
	AAC_16k_STEREO,
	AAC_32k_MONO,
	AAC_32k_STEREO,
	AAC_48k_MONO,
	AAC_48k_STEREO
}TEnumAACType;

//2007-7-17 xc add
typedef union tagAudCodecSubType
{
	TEnumAACType tAacSubType;
}TAudCodecSubType;

//音频混音模式
typedef enum
{
	MIX_16=0,
	MIX_32
}TEnumAdMixMode;

//AEC处理模式
typedef enum
{
	AEC_SETREF_MODE=0,                 //参考信号设置模式
	AEC_PROC_MODE                      //回声处理模式
}TEnumAdAecMode;


//音频XDM封装模块错语码分配枚举定义
//注:模块错语码占用7个bit(bit24~bit30),最高位bit31为0
typedef enum
{
	ERR_G711ENC = (1<<24),
	ERR_G711DEC = (2<<24),

	ERR_G722ENC = (3<<24),
	ERR_G722DEC = (4<<24),

	ERR_G7221CENC = (5<<24),
	ERR_G7221CDEC = (6<<24),

	ERR_G728ENC = (7<<24),
	ERR_G728DEC = (8<<24),

	ERR_G729ENC = (9<<24),
	ERR_G729DEC = (10<<24),

	ERR_MP3ENC = (11<<24),
	ERR_MP3DEC = (12<<24),

	ERR_ADPCMENC = (13<<24),
	ERR_ADPCMDEC = (14<<24),

	ERR_AEC8K = (15<<24),          //15, 8k回声抵消模块
	ERR_AEC16K = (16<<24),         //16, 16k回声抵消模块
	ERR_DTMF = (17<<24),           //17, 双音多频模块
	ERR_MIXER = (18<<24),          //18, 混音器模块
	ERR_SPLIT = (19<<24),          //19, 一路分解四路语音模块
	ERR_AACENC = (20<<24),         //20   2007-7-17 xc add
	ERR_AACDEC = (21<<24),          //21   2007-7-24 xc add

	ERR_AUDPROC = (22<<24)         //22   2007-8-17 xc add
}TEnumAdErrMode;

#ifdef __cplusplus
}
#endif

#endif





