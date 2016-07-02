/*****************************************************************************
   模块名      : mpulib
   文件名      : mpuutility.h
   相关文件    : mpuutility.cpp
   文件实现功能: 工具/通用 类/函数 声明
   作者        : zhangbq
   版本        : V4.5  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期       版本        修改人      修改内容
   2009/3/14    4.6         张宝卿      创建
******************************************************************************/
#ifndef _MPU_UTILITY_H_
#define _MPU_UTILITY_H_

#include "osp.h"
#include "kdvtype.h"
#include "kdvmedianet.h"
#include "mcustruct.h"

//打印级别定义
#define MPU_CRIT                (u8)0       //关键点/错误/异常
#define MPU_WARN                (u8)1       //警告
#define MPU_INFO                (u8)2       //流程
#define MPU_DETAIL              (u8)3       //流程细节/详细调试点


void mpulog( u8 byLevel, s8* pszFmt, ... );
void mpulogall( s8* pszFmt, ... );
s8 * ipStr( u32 dwIP );	
s8 * StrOfIP( u32 dwIP );

u16  AdjustPrsMaxSpan(u8 byVmpStyle);
void GetRSParamByPos(u8 byPos, TRSParam &tRSParam);
u8   ConvertVmpStyle2ChnnlNum(u8 byVmpStyle);
void ConvertToRealFR(THDAdaptParam& tAdptParm);
void ConverToRealParam(u8 byChnId, u8 byOutIdx, THDAdaptParam& tBasParam);



#endif //!_MPU_UTILITY_H_

//END OF FILE

