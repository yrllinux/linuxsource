/*****************************************************************************
模   块   名: 采样率变换模块
文   件   名: resample_48k.h
相 关  文 件: resample_48k.h, resample_48k.c
文件实现功能: 实现8k,16k,32k采样率与48k采样率之间的转换
版        本: V1.0  Copyright(C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
修 改 记 录:
日       期      版本        修改人      修改内容
 2006/06/20      1.0         徐  超      创    建
******************************************************************************/
#ifndef RESAMPLE_48K_H
#define RESAMPLE_48K_H

#ifdef __cplusplus
extern "C" {
#endif

//宏定义
#define MAXFACTOR_RES48K 8                          //Maximum Factor without output buff overflow
#define MAXIBUFFSIZE_RES48K 2048                    //Input buffer size
#define MAXOBUFFSIZE_RES48K (MAXIBUFFSIZE_RES48K*MAXFACTOR_RES48K+2) //Calc'd out buffer size

//结构类型定义
typedef enum  tagENUM_RES
{
	//降采样
	RES_48TO8,
	RES_48TO16,
	RES_48TO32,

	//升采样
	RES_8TO48,
	RES_16TO48,
	RES_32TO48
} ENUM_RES;

typedef void (*PRESFUN)(void *, s16 *, s16 *, l32 *, l32 *);

typedef struct tagRESAMPLE_HANDLE
{
	u32 u32Time; 
	s16 *ps16Imp;
	PRESFUN Resamp;  //函数指针

	//输入输出Buf
	s16 as16X[MAXIBUFFSIZE_RES48K];
	s16 as16Y[MAXOBUFFSIZE_RES48K];

	u16 u16FrameLen;
	u16 u16OutBufLen;
	u16 u16Xp;
	u16 u16XOff;
	u16 u16Xread;
	u16 u16LpScl;
	u16 u16OSkip;
	u16 u16Dhb;      
	u16 u16Dtb;
} RESAMPLE_HANDLE, *PRESAMPLE_HANDLE;

//接口函数声明
void InitRes48k(PRESAMPLE_HANDLE pResH, ENUM_RES eRes, u16 u16FrameLen);
l32 ResFlow48k(PRESAMPLE_HANDLE pResH,s16 *ps16InData, s16 *ps16OutData);

#ifdef __cplusplus
}
#endif

#endif // end of RESAMPLE_48K_H
