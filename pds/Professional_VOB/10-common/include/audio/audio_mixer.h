/*****************************************************************************
模   块   名: 混音器
文   件   名: mixer16.c, mixer32.c
相 关  文 件: 
文件实现功能: 混音器对外接口声明
版        本: V1.0  Copyright(C) 2005-2009 KDC, All rights reserved.
-----------------------------------------------------------------------------
修 改 记 录:
日       期      版本        修改人      修改内容
2003/06/10       1.0         缪春波      创    建
2006/12/20       1.1         徐  超      增加了32位混音接口
******************************************************************************/
#ifndef MIXER_V100_H
#define MIXER_V100_H

#ifdef __cplusplus
extern "C" {
#endif   /* __cplusplus */

#include "audio_define.h"

enum mixer_errcode
{
	ERR_AUDMIX_START = ERR_MIXER,

	ERR_AUDMIX_MODE,              //混音模式错误

	ERR_AUDMIX_MEM_INIT,          //内存管理初始化失败
	ERR_AUDMIX_INIT_PARA_NULL,    //初始化函数输入参数指针为空
	ERR_AUDMIX_SLOWMEM_NULL,      //内存为空或不足

	ERR_AUDMIX_VER_BUF_SMALL,     //版本buf太小
	ERR_AUDMIX_STATUS_NULL,       //获取状态函数输入参数指针为空

	ERR_AUDMIX_PROC_PARM_NULL,    //混音处理函数输入参数指针为空
	ERR_AUDMIX16_SAMPLE_CNT,      //16位混音处理函数输入样本数错误
	ERR_AUDMIX32_SAMPLE_CNT,      //32位混音处理函数输入样本数错误

	ERR_AUDMIX_CLOSE              //句柄关闭失败
};

//一路混音结构体定义
typedef struct
{
    u8 *pu8ChanBuf;               //一路混音码流指针mPCMValue
    u32 u32Volume;                //对应的音量(0~255,如果是主席则可以放大一些)nVolume
}TAudMixBuf;

//混音初始化参数结构体
typedef struct
{
    u32 u32Reserved;              //保留参数
}TAudMixParam;

#define TAudMixStatusParam TAudMixParam

//混音输入结构体
typedef struct
{
	TEnumAudMixMode tAudMixMode;
	                              //混音模式
	TAudMixBuf **pptInAudBuf;     //指向混音器输入结构体指针数组
	u32 u32InChList;              //混音输入通道列表 
    TAudMixBuf **pptOutAudBuf;    //指向混音器N-1模式输出结构体指针数组
    TAudMixBuf *ptOutNAudBuf;     //N模式输出结构体指针
    u32 u32InSampleCnt;           //每路混音样本数
    u32 u32Reserved;              //保留参数
}TAudMixInput;

//混音输出结构体
typedef struct
{       
    u32  u32StatisCycles;         //混音一帧效率统计
    u32  u32Reserved;             //保留参数
}TAudMixOutput;

/*===============================================================
函   数   名: AudMixOpen
功        能: 混音器初始化
引用全局变量: 
输入参数说明: 无
              ppvHandle            指向混音器句柄指针的指针[out]
			  ptAudMixParam        初始化参数结构指针[in]
			  pvAudMemParam        内存管理结构体的指针[in]
返回 值 说明: 
              成功: AUDIO_SUCCESS
              失败: 错误码
特 殊  说 明: 无
=================================================================*/
l32 AudMixOpen(void **ppvHandle,
			   TAudMixParam *ptAudMixParam,
			   void *pvAudMemParam);

/*====================================================================
函   数   名: AudMixProcess
功        能: 实现混音算法
引用全局变量: 无
输入参数说明: 
			 pvHandle            指向混音器句柄指针[in]
			 ptAudMixInput       指向混音输入结构体指针[in/out]
			 ptAudMixOutput      指向混音输出结构体[out]
返回 值 说明: 
              成功-AUDIO_SUCCESS, 失败-错误码
特 殊  说 明: 无
======================================================================*/
l32 AudMixProcess(void *pvHandle,
				  TAudMixInput *ptAudMixInput,
				  TAudMixOutput *ptAudMixOutput);

/*===============================================================
函   数   名: AudMixClose
功        能: 混音器关闭
引用全局变量: 
输入参数说明: 无
              pvHandle             指向混音器句柄的指针[in]
返回 值 说明: 
              成功: AUDIO_SUCCESS
              失败: 错误码
特 殊  说 明: 无
=================================================================*/
l32 AudMixClose(void *pvHandle);

/*===============================================================
函   数   名: AudMixGetStatus
功        能: 获取混音器状态信息
引用全局变量: 
输入参数说明: 无
              pvHandle             指向混音器句柄的指针[in]
			  ptAudMixStatusParam  指向混音状态参数结构体指针[out]
返回 值 说明: 
              成功: AUDIO_SUCCESS
              失败: 错误码
特 殊  说 明: 无
=================================================================*/
l32 AudMixGetStatus(void *pvHandle, 
					TAudMixStatusParam *ptAudMixStatusParam);

/*===============================================================
函   数   名: AudMixGetVersion
功        能: 取得音频混音器模块版本号
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
l32 AudMixGetVersion(void *pvVer, l32 l32BufLen, l32 *pl32VerLen);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif// end of MIXER_V100_H

