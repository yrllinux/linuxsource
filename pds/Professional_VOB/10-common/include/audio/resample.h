/*****************************************************************************
模   块   名: resample
文   件   名: resample_4.c, resample_2.c
              resample_1dot5.c, resample_3.c, resample_6.c
相 关  文 件: 
文件实现功能: 采样频率的转换
版        本: V1.0  Copyright(C) 2005-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修 改 记 录:
日       期      版本        修改人      修改内容
2005/11/17       1.0         徐  超      创    建
2006/07/13       2.0         徐  超      增加了与48k采样率相关的变换功能
******************************************************************************/
#ifndef RESAMPLE_H
#define RESAMPLE_H

#ifdef __cplusplus
extern "C" {
#endif

#define RESAMPLE_OK  0
#define RESAMPLE_ERR -1

#define FILTER112_LEN	112

typedef struct tSRCStruct
{
	l32 l32SmallFramLen;                //升采样时输入帧长or降采样时输出帧长
	l32 l32BigFramLen;                  //降采样时输入帧长or升采样时输出帧长

	s16 *ps16LtoHBuf;                   //升采样buf
	s16 *ps16HtoLBuf;                   //降采样buf

	s16 as16LtoHCoef[FILTER112_LEN];    //升采样
	s16 as16HtoLCoef[FILTER112_LEN];    //降采样
} SRCStruct;

/*====================================================================
函   数   名: SRCReSet_4
功        能: 4倍采样变换重置(改变filter缓冲, 重置帧长, 缓冲清零等)
引用全局变量: 无
输入参数说明: 无
返回 值 说明: RESAMPLE_OK 成功 RESAMPLE_ERR 失败
特 殊  说 明: 升降缓冲长度一样,必须大于
              (FILTER112_LEN + l32SmallFramLen *4)*2
			  以字节为单位
======================================================================*/
l32 SRCReSet_4(SRCStruct *pSRCHand, s16 *ps16HtoLBuf, s16 *ps16LtoHBuf, l32 l32SmallFramLen);


/*====================================================================
函   数   名: SRCInit_4
功        能: 4倍采样变换初始化(系数数组的初始化, 缓冲清零)
引用全局变量: 无
输入参数说明: 无
返回 值 说明: RESAMPLE_OK 成功 RESAMPLE_ERR 失败
特 殊  说 明: 升降缓冲长度一样,必须大于
              (FILTER112_LEN + l32SmallFramLen *4)*2
			  以字节为单位
======================================================================*/
l32 SRCInit_4(SRCStruct *pSRCHand, s16 *ps16HtoLBuf, s16 *ps16LtoHBuf, l32 l32SmallFramLen);

void SRC32kTo8k(SRCStruct *pSRCHand, s16 *ps16InData, s16 *ps16OutData);

void SRC8kTo32k(SRCStruct *pSRCHand, s16 *ps16InData, s16 *ps16OutData);


/*====================================================================
函   数   名: SRCReSet_2
功        能: 2倍采样变换重置(改变filter缓冲, 重置帧长, 缓冲清零等)
引用全局变量: 无
输入参数说明: 无
返回 值 说明: RESAMPLE_OK 成功 RESAMPLE_ERR 失败
特 殊  说 明: 升降缓冲长度一样,必须大于
              (FILTER112_LEN + l32SmallFramLen *2)*2
			  以字节为单位
======================================================================*/
l32 SRCReSet_2(SRCStruct *pSRCHand, s16 *ps16HtoLBuf, s16 *ps16LtoHBuf, l32 l32SmallFramLen);

/*====================================================================
函   数   名: SRCInit_2
功        能: 2倍采样变换初始化(系数数组的初始化, 缓冲清零)
引用全局变量: 无
输入参数说明: 无
返回 值 说明: RESAMPLE_OK 成功 RESAMPLE_ERR 失败
特 殊  说 明: 升降缓冲长度一样,必须大于
              (FILTER112_LEN + l32SmallFramLen *2)*2
			  以字节为单位
======================================================================*/
l32 SRCInit_2(SRCStruct *pSRCHand, s16 *ps16HtoLBuf, s16 *ps16LtoHBuf, l32 l32SmallFramLen);

void SRC32kTo16k(SRCStruct *pSRCHand, s16 *ps16InData, s16 *ps16OutData);

void SRC16kTo32k(SRCStruct *pSRCHand, s16 *ps16InData, s16 *ps16OutData);


/*====================================================================
函   数   名: SRCReSet_1dot5
功        能: 采样变换重置(改变filter缓冲, 重置帧长, 缓冲清零等)
引用全局变量: 无
输入参数说明: 无
返回 值 说明: RESAMPLE_OK 成功 RESAMPLE_ERR 失败
特 殊  说 明: 升降缓冲长度一样,必须大于
              (FILTER112_LEN + l32SmallFramLen *1.5)*2
			  以字节为单位
======================================================================*/
l32 SRCReSet_1dot5(SRCStruct *pSRCHand, s16 *ps16HtoLBuf, s16 *ps16LtoHBuf, l32 l32SmallFramLen);
/*====================================================================
函   数   名: SRCInit_1dot5
功        能: 48k-32k降采样滤波
引用全局变量: 无
输入参数说明: 
              1-(i) pSRCHand      句柄
			  2-(i) ps16InData    输入数据buf
			  3-(o) ps16InData    输出数据buf
返回 值 说明: 无
特 殊  说 明: 无
======================================================================*/
l32 SRCInit_1dot5(SRCStruct *pSRCHand, s16 *ps16HtoLBuf, s16 *ps16LtoHBuf, l32 l32SmallFramLen);
void SRC32kTo48k(SRCStruct *pSRCHand, s16 *ps16InData, s16 *ps16OutData);
void SRC48kTo32k(SRCStruct *pSRCHand, s16 *ps16InData, s16 *ps16OutData);


/*====================================================================
函   数   名: SRCReSet_3
功        能: 采样变换重置(改变filter缓冲, 重置帧长, 缓冲清零等)
引用全局变量: 无
输入参数说明: 无
返回 值 说明: RESAMPLE_OK 成功 RESAMPLE_ERR 失败
特 殊  说 明: 升降缓冲长度一样,必须大于
              (FILTER112_LEN + l32SmallFramLen *3)*2
			  以字节为单位
======================================================================*/
l32 SRCReSet_3(SRCStruct *pSRCHand, s16 *ps16HtoLBuf, s16 *ps16LtoHBuf, l32 l32SmallFramLen);

/*====================================================================
函   数   名: SRCInit_3
功        能: 48k-16k降采样滤波
引用全局变量: 无
输入参数说明: 
              1-(i) pSRCHand      句柄
			  2-(i) ps16InData    输入数据buf
			  3-(o) ps16InData    输出数据buf
返回 值 说明: 无
特 殊  说 明: 无
======================================================================*/
l32 SRCInit_3(SRCStruct *pSRCHand, s16 *ps16HtoLBuf, s16 *ps16LtoHBuf, l32 l32SmallFramLen);
void SRC16kTo48k(SRCStruct *pSRCHand, s16 *ps16InData, s16 *ps16OutData);
void SRC48kTo16k(SRCStruct *pSRCHand, s16 *ps16InData, s16 *ps16OutData);



/*====================================================================
函   数   名: SRCReSet_6
功        能: 采样变换重置(改变filter缓冲, 重置帧长, 缓冲清零等)
引用全局变量: 无
输入参数说明: 无
返回 值 说明: RESAMPLE_OK 成功 RESAMPLE_ERR 失败
特 殊  说 明: 升降缓冲长度一样,必须大于
              (FILTER112_LEN + l32SmallFramLen *6)*2
			  以字节为单位
======================================================================*/
l32 SRCReSet_6(SRCStruct *pSRCHand, s16 *ps16HtoLBuf, s16 *ps16LtoHBuf, l32 l32SmallFramLen);

/*====================================================================
函   数   名: SRCInit_6
功        能: 48k-8k降采样滤波
引用全局变量: 无
输入参数说明: 
              1-(i) pSRCHand      句柄
			  2-(i) ps16InData    输入数据buf
			  3-(o) ps16InData    输出数据buf
返回 值 说明: 无
特 殊  说 明: 无
======================================================================*/
l32 SRCInit_6(SRCStruct *pSRCHand, s16 *ps16HtoLBuf, s16 *ps16LtoHBuf, l32 l32SmallFramLen);
void SRC8kTo48k(SRCStruct *pSRCHand, s16 *ps16InData, s16 *ps16OutData);
void SRC48kTo8k(SRCStruct *pSRCHand, s16 *ps16InData, s16 *ps16OutData);

#ifdef __cplusplus
}
#endif

#endif //end of RESAMPLE_H

