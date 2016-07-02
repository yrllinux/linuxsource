/*****************************************************************************
   模块名      : 码流适配器
   文件名      : basinst.h
   创建时间    : 2003年 6月 25日
   实现功能    : 错误码头文件
   作者        : 张明义
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
******************************************************************************/
#ifndef _BAS_ERR_CODE_H_
#define _BAS_ERR_CODE_H_

#include "errorid.h"

#define ERR_BAS_CHNOTREAD     ( ERR_BAS_BGN + 1 ) //当前通道不在就绪状态
#define ERR_BAS_CHRUNING      ( ERR_BAS_BGN + 2 ) //当前通道正在运行 
#define ERR_BAS_CHREADY       ( ERR_BAS_BGN + 3 ) //当前通道正在运行 
#define ERR_BAS_CHIDLE        ( ERR_BAS_BGN + 4 ) //当前通道为空     
#define ERR_BAS_OPMAP         ( ERR_BAS_BGN + 5 ) //调用底层函数失败
#define ERR_BAS_ERRCONFID     ( ERR_BAS_BGN + 6 ) //错误的会议
#define ERR_BAS_NOENOUGHCHLS  ( ERR_BAS_BGN + 7 ) //通道数不够

#endif
