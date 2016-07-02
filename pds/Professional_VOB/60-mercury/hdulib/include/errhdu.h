/*****************************************************************************
   模块名      : 高清解码卡
   文件名      : errhdu.h
   相关文件    : 
   文件实现功能: HDU错误号定义
   作者        : 江乐斌
   版本        : V1.0  Copyright(C) 2008-2011 KDC, All rights reserved.
 -----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2008/12/18     		   江乐斌		创建
******************************************************************************/
#ifndef _HDU_ERROR_CODE_H_
#define _HDU_ERROR_CODE_H_

#include "errorid.h"

#define ERR_HDU_INVALID_CHANNL	    ( ERR_HDU_BGN + 1 )  //无效的通道号
#define ERR_HDU_INVALID_MEDIATYPE   ( ERR_HDU_BGN + 2 )  //无效的媒体类型(必须含视频)
#define ERR_HDU_STARTPLAY_FAIL      ( ERR_HDU_BGN + 3 )  //开始播放失败
#define ERR_HDU_STOPPLAY_FAIL       ( ERR_HDU_BGN + 4 )  //停止播放失败
#endif
