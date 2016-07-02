/*****************************************************************************
   模块名      : 画面复合器配置
   文件名      : VMPCfg.cpp
   创建时间    : 2003年 12月 4日
   实现功能    : 
   作者        : zhangsh
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
******************************************************************************/
#include "bascfg.h"



void CCPParam::GetDefaultParam(u8 byEncType, TVidEncParam& tEncParam)
{
    GetDefaultEncParam(byEncType, tEncParam);
}


void CCPParam::Clear()
{
    memset( &m_tStatus, 0, sizeof(m_tStatus) );
    m_tStatus.m_byVMPStyle = 0xFF;  //invalid
}


CCPParam::CCPParam()
{
    Clear();    
}

CCPParam::~CCPParam()
{
}
