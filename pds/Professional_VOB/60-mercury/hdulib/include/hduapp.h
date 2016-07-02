/*****************************************************************************
模块名      : 高清解码卡
文件名      : hduapp.h
相关文件    : 
文件实现功能: 多画面显示实例头文件
作者        : 江乐斌
版本        : V1.0  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
08/11/26    4.6         江乐斌        创建
******************************************************************************/
#ifndef _HDU_APP_H_
#define _HDU_APP_H_

#ifndef __BITSTREAM_ADAPT_SERVER_APP_H__
#define __BITSTREAM_ADAPT_SERVER_APP_H__

#ifdef _LINUX_
#include "nipwrapper.h"
#endif

API void sethdulog(int nlvl);    //设置打印级别
API void hduver(void);           //显示版本号
API void hduhelp(void);          //显示帮助信息
API void hduscreen(void);        //打印屏幕消息
API void hdushow(void);          //HDU状态显示
API	void hduinfo(void);          // 显示配置信息
API void sethdumode(u8 byNum, u8 byPort, u8 byMode, u8 byPlayScale);  
API void showmode(void); 
API void hdutau( LPCSTR lpszUsrName = NULL, LPCSTR lpszPwd = NULL );    // telnet授权
API void setautotest( s32 nIsAutoTest );  //设置是否硬件自动检测
void HduAPIEnableInLinux(void);  //注册linux调试接口
BOOL hduInit(THduCfg* ptCfg);    //HDU初始化

#endif//!__BITSTREAM_ADAPT_SERVER_APP_H__

#endif//_HDU_APP_H
