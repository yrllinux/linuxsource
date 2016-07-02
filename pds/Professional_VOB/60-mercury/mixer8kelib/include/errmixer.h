/*****************************************************************************
   模块名      : 混音器
   文件名      : errmixinst.h
   创建时间    : 2003年 12月 9日
   实现功能    : 
   作者        : 张明义
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   03/12/09    3.0         zmy         创建
******************************************************************************/
#ifndef _KDV_MIXER_ERROR_CODE_H_
#define _KDV_MIXER_ERROR_CODE_H_
#include "errorid.h"

#define ERR_MIXER_NONE               ( ERR_MIXER_BGN  )     //!无错误 
#define ERR_MIXER_GRPNOTEXIST        ( ERR_MIXER_BGN + 1 )  //!指定的混音组不存在
#define ERR_MIXER_MMBNOTEXIST        ( ERR_MIXER_BGN + 2 )  //!指定的混音成员不存在
#define ERR_MIXER_NOFREEGRP          ( ERR_MIXER_BGN + 3 )  //!没有空闲的混音组
#define ERR_MIXER_NOFREEMMB          ( ERR_MIXER_BGN + 4 )  //!没有空闲的混音通道
#define ERR_MIXER_ILLEGALBODY        ( ERR_MIXER_BGN + 5 )  //!消息中的消息体非法
#define ERR_MIXER_CALLDEVICE         ( ERR_MIXER_BGN + 6 )  //!调用底层函数失败
#define ERR_MIXER_MIXING             ( ERR_MIXER_BGN + 7 )  //!正在混音
#define ERR_MIXER_NOTMIXING          ( ERR_MIXER_BGN + 8 )  //!未混音
#define ERR_MIXER_ERRCONFID          ( ERR_MIXER_BGN + 9 )  //!无效的会议ID
#define ERR_MIXER_GRPINVALID         ( ERR_MIXER_BGN + 10 ) //无效的混音组ID

#endif/*_KDV_MIXER_ERROR_CODE_H_*/
