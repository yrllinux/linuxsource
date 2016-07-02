/*****************************************************************************
模块名      : AecFilter
文件名      : AecFilter.h
相关文件    : AecFilter.c
文件实现功能: 新版ACE自适应滤波
版本        : V1.0  Copyright(C) 2005-2007 KDC, All rights reserved.
作者        : 徐   超
-----------------------------------------------------------------------------
修改记录:
日    期      版本        修改人      修改内容
2005/04/12    1.0         奚荣勇      添加测试接口
2005/05/08    1.0         徐  超      1.添加了宏AEC_FAP_OK及AEC_FAT_ERR
                                      2.头文件中数据类型全部统一至"algorithmtype.h"
									  3.两个主要测试函数添加了具体说明
2005/05/20    1.1         徐  超      实现了AecTypeSet函数
                                      取消了AEC_ADFILTER_TAG结构中的变量bStartFlag
2006/05/18    2.0         宋  兵      提交到版本机上，给测试组测试
2006/10/26    2.0         徐  超      增加对g7221c支持
******************************************************************************/

#ifndef AEC_FILTER_H
#define AEC_FILTER_H

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
函   数   名:  aecInitializationNew
功        能： 回声抵消的初始化
引用全局变量:  无
输入参数说明:
				1.(i/o)ppAecHandle   句柄指针的指针
				2.(i)l32AecType      音频编解码的类型
返回 值 说明:  AEC_OK：初始化成功；AEC_ERR：初始化失败
特 殊 说  明:  无
======================================================================*/
l32 aecInitializationNew(void **ppAecHandle, l32 l32AecType);
/*====================================================================
函   数   名:  AecRefSetNew
功        能： 把远端信号放入远端信号的缓存中
引用全局变量:  无
输入参数说明:
				1.(i/o)pHandle   句柄指针
				2.(i)ps16RefBuf  输入的参考信号的指针
				3.(i)bMute       远端静音标志
				4.(i)bStereo     立体声
				5.(i)s16BuffNum  采集和播放的缓存数
返回 值 说明:  AEC_OK：成功；AEC_ERR：失败
特 殊 说  明:  无
======================================================================*/
l32 AecRefSetNew(void *pHandle, s16 *ps16RefBuf, l32 bMute,l32 bStereo, s16 s16BuffNum);
/*====================================================================
函   数   名:  EchoCancelNew
功        能： 完成回声抵消滤波
引用全局变量:  无
输入参数说明:
				1.(i/0)pHandle          句柄指针
				2.(i)ps16TmExpSig(8k)   近端信号
				3.(o)ps16TmOutSig(8k)   输出信号
返回 值 说明:  AEC_OK 为有语音输出，AEC_ERR 为没有语音输出
特 殊 说  明:  无
======================================================================*/
l32 EchoCancelNew(void  *pHandle, s16 *ps16TmExpSig,s16 *ps16TmOutSig);
/*====================================================================
函   数   名:  AecFapFreeNew
功        能： 释放回声抵消的句柄空间
引用全局变量:  无
输入参数说明:
				1.(i/0)pHandle          句柄指针
返回 值 说明:  AEC_OK：成功；AEC_ERR：失败
特 殊 说  明:  无
======================================================================*/
l32	AecFapFreeNew(void *pHandle);
/*====================================================================
函   数   名:  AecTypeSetNew
功        能： 回声抵消的重置
引用全局变量:  无
输入参数说明:
				1.(i/o)pHandle   句柄指针
				2.(i)l32AecType      音频编解码的类型
返回 值 说明:  AEC_OK：成功；AEC_ERR：失败
特 殊 说  明:  无
======================================================================*/
l32 AecTypeSetNew(void *pHandle,l32 l32AecType);
/*====================================================================
函   数   名:  GetAecVersionNew
功        能： 得到回声抵消的版本
引用全局变量:  无
输入参数说明:
				1.(o)*Version: 用来取得版本的空间。
				2.(o)StrLen, 用来取得版本的空间大小。
				3.(o)*StrLenUsed，版本字符串所占用的空间大小
返回 值 说明:  无
特 殊 说  明:  无
======================================================================*/
void GetAecVersionNew(s8 *Version, l32 StrLen, l32 *pStrLenUsed);

#endif //end of AEC_FILTER_H
