/*****************************************************************************
   模块名      : mpu2lib
   文件名      : baserr.h
   相关文件    : 
   文件实现功能: mpu2lib错误码定义
   作者        : 倪志俊
   版本        : V4.7  Copyright(C) 2012-2015 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期       版本        修改人      修改内容
   2012/07/26  4.7         倪志俊       创建
******************************************************************************/
#ifndef _BAS_ERR_CODE_H_
#define _BAS_ERR_CODE_H_

#include "errorid.h"

#define ERR_BAS_CHNOTREAD           ( ERR_BAS_BGN + 1 ) //当前通道不在就绪状态
#define ERR_BAS_CHRUNING            ( ERR_BAS_BGN + 2 ) //当前通道正在运行 
#define ERR_BAS_CHREADY             ( ERR_BAS_BGN + 3 ) //当前通道正在运行 
#define ERR_BAS_CHIDLE              ( ERR_BAS_BGN + 4 ) //当前通道为空     
#define ERR_BAS_OPMAP               ( ERR_BAS_BGN + 5 ) //调用底层函数失败
#define ERR_BAS_ERRCONFID           ( ERR_BAS_BGN + 6 ) //错误的会议
#define ERR_BAS_NOENOUGHCHLS        ( ERR_BAS_BGN + 7 ) //通道数不够
#define ERR_BAS_GETCFGFAILED		( ERR_BAS_BGN + 8 ) //读取配置失败
#endif


//END OF FILE

