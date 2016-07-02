/*****************************************************************************
模块名      : aec_spe_agc_16k_interface
文件名      : aec_spe_agc_interface.h
相关文件    : aecpack.c
文件实现功能: AEC/SPE/AGC/对外接口函数
版本        : V1.0  Copyright(C) 2006-2008 KDC, All rights reserved.
作者        : 姜莎
-----------------------------------------------------------------------------
修改记录:
日    期      版本        修改人      修改内容
2008/08/04    v100        姜  莎      在16K AEC的基础上集成了SPE和AGC
******************************************************************************/
#ifndef AEC_SPE_AGC_16K_INTERFACE_H
#define AEC_SPE_AGC_16K_INTERFACE_H

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

#define AEC_OK      1
#define AEC_ERR     0

#define AEC_HANDLE void *

/*====================================================================
函   数   名: AEC_SPE_AGC_Proc_Init
功        能：AEC/SPE/AGC的初始化
引用全局变量: 无
输入参数说明: 1.(i)pvAecHandle      句柄指针的指针
              2.(i)pvPackBuf        3个缓冲区buffer的内存空间的指针
              3.(i)pvAdFiltHd       AEC滤波器句柄的指针
			  4.(i)pvSPEHd          SPE句柄指针
			  5.(i)pvAGCHd          AGC句柄指针
			  6.(i)l32AecType       音频编解码的类型
			  返回 值 说明: 成功：返回AEC_OK
              失败：返回AEC_ERR
			  特 殊 说  明: 不管是否开启AEC/SPE/AGC，都先初始化
======================================================================*/
l32 AEC_SPE_AGC_Proc_Init(void *pvAecHandle, void *pvPackBuf, void *pvAdFiltHd, void *pvSPEHd, void *pvAGCHd, l32 l32AecType);

/*====================================================================
函   数   名: AecRefSetNew
功        能：把远端信号放入远端信号的缓存中
引用全局变量: 无
输入参数说明: 1.(i/o)pHandle		  句柄指针
              2.(i)ps16RefBuf         输入的参考信号的指针
              3.(i)l32CapSamples      采集缓冲区样点数
              4.(i)l32NoPlySamples    播放缓冲区样点数
返回 值 说明: AEC_OK：成功
              AEC_ERR：失败
特 殊 说  明: 无
======================================================================*/
l32 AecRefSetNew(void *pHandle, s16 *ps16RefBuf, l32 l32CapSamples, l32 l32NoPlySamples);

/*====================================================================
函   数   名: AEC_SPE_AGC_Proc
功        能：完成回声抵消滤波
引用全局变量: 无
输入参数说明: 1.(i/o)pHandle           句柄指针
              2.(i)ps16EchoSig	       近端信号
              3.(o)ps16AecOutSig       输出信号
              4.(i)l32UseFlag          AEC/SPE/AGC使能的标志
返回 值 说明: 成功：返回AEC_OK
特 殊 说  明: 无
======================================================================*/
l32 AEC_SPE_AGC_Proc(void *pHandle, s16 *ps16EchoSig, s16 *ps16AecOutSig, l32 l32UseFlag);

/*====================================================================
函   数   名:  AecFapFreeNew
功        能： 释放回声抵消的句柄空间
引用全局变量:  无
输入参数说明:  1.(i/0)pHandle          句柄指针
返回 值 说明:  AEC_OK：成功；AEC_ERR：失败
特 殊 说  明:  无
======================================================================*/
l32	AecFapFreeNew(void *pHandle);

/*====================================================================
函   数   名: AecTypeSetNew
功        能：回声抵消的重置
引用全局变量: 无
输入参数说明: 1.(i/o)pHandle       句柄指针
			  2.(i)l32AecType      音频编解码的类型
返回 值 说明: AEC_OK：成功；AEC_ERR：失败
特 殊 说  明: 无
======================================================================*/
l32 AecTypeSetNew(void *pHandle, l32 l32AecType);

/*====================================================================
函   数   名: GetAecVersionNew
功        能：得到AEC/SPE/AGC的版本
引用全局变量: 无
输入参数说明: 1.(o)*Version: 用来取得版本的空间。
			  2.(o)StrLen, 用来取得版本的空间大小。
			  3.(o)*StrLenUsed，版本字符串所占用的空间大小
返回 值 说明: 无
特 殊 说  明: 无
======================================================================*/
void GetAecVersionNew(s8 *Version, l32 StrLen, l32 *pStrLenUsed);

#endif //end of AEC_SPE_AGC_16K_INTERFACE_H

