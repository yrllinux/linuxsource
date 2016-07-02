/*****************************************************************************
   模块名      : 多画面显示
   文件名      : twinst.h
   相关文件    : 
   文件实现功能: 多画面显示错误码头文件
   作者        : 李屹
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
 -----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/11/20     			   zhangsh		MCC
******************************************************************************/
#ifndef _TVWALL_ERROR_CODE_H_
#define _TVWALL_ERROR_CODE_H_

#include "errorid.h"

#define ERR_TW_INVALID_CHANNL	   ( ERR_TVWALL_BGN + 1 )  //无效的通道号
#define ERR_TW_INVALID_MEDIATYPE   ( ERR_TVWALL_BGN + 2 )  //无效的媒体类型(必须含视频)
#define ERR_TW_STARTPLAY_FAIL      ( ERR_TVWALL_BGN + 3 )  //开始播放失败
#define ERR_TW_STOPPLAY_FAIL       ( ERR_TVWALL_BGN + 4 )  //开始播放失败
#endif 

// _TVWALL_ERROR_CODE_H_
