/*******************************************************************************
 *  模块名   :                                                                 *
 *  文件名   : mtmp.h                                                          *
 *  相关文件 :                                                                 *
 *  实现功能 :                                                                 *
 *  作者     : 张明义                                                          *
 *  版本     : V4.0  Copyright(c) 1997-2005 KDC, All rights reserved.          *
 *                                                                             *
 *    =======================================================================  *
 *  修改记录:                                                                  *
 *    日  期      版本        修改人      修改内容                             *
 *  2005/12/5     4.0         张明义      创建                                 *
 *                                                                             *
 *******************************************************************************/


#ifndef _MT_MP_H_
#define _MT_MP_H_

API BOOL InitDSP();


#ifdef _ppc1point2_
API BOOL UnloadDSP();
#endif

#ifdef WIN32
#ifdef SOFT_CODEC

#include "codeclib.h"

//视频解码器的重绘接口
void mtRedrawLastFrame( EmCodecComponent emCodec );

void mtSetShowPreview(BOOL bOpen);

//获取音频功率回调函数指针
void mtGetAudioPowerCallBack(PGETAUDIOPOWER& cb , u32& dwContext);

//设置获取输出音频功率回调函数
void mtSetAudioPowerOutCallBack( PGETAUDIOPOWER cb , u32 dwContext);

//设置获取输入音频功率回调函数
void mtSetAudioPowerInCallBack( PGETAUDIOPOWER cb, u32 dwContext);

//设置解码器显示窗口句柄
void mtSetDecoderHandle( HWND hPreview ,HWND hPrimo ,HWND hSecond );

//对解码器进行抓图
void mtCodecGrapPic( CDrawWnd *pWnd ,EmVideoType emType);

//对编码器进行抓图
void mtCodecGrapPicLocal(CDrawWnd *pWnd);

//设置解码方式
void mtCodecSetDecodMod(EmVideoType emType ,BOOL bIFrameOnly );

//设置视频源
void mtCodecSelectVidSource(s8* pchFilename, u32 dwLength);

//pcmt 多声卡选择,获取输入声卡列表
u16 mtGetWaveInAudioDevice( s8 pszWaveInAudioDevList[MAX_AUDIO_DEV_NUMBER][MAX_AUDIO_DEV_NAME_LEN] );

//pcmt 多声卡选择,获取输出声卡列表
u16 mtGetWaveOutAudioDevice( s8 pszWaveOutAudioDevList[MAX_AUDIO_DEV_NUMBER][MAX_AUDIO_DEV_NAME_LEN] );

//pcmt 多声卡选择,选择输入声卡
void mtSetWaveInAudioDevice( s32 nDeviceID );

//pcmt 多声卡选择,选择输出声卡
void mtSetWaveOutAudioDevice( s32 nDeviceID );

#endif  //SOFT_CODEC

#endif //WIN32

#endif

