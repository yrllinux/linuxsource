/*****************************************************************************
   模块名      : KDV draw bmp
   文件名      : drawinterface.cpp
   相关文件    : drawinterface.h
   文件实现功能: 简易画图接口类
   作者        : 
   版本        : V0.9  Copyright(C) 2001-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2006/03/29  4.0         john     创建
******************************************************************************/
#include "drawinterface.h"
#include "drawbmp.h"

/*=============================================================================
  函 数 名： CDrawInterface
  功    能： 构造
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
=============================================================================*/
CDrawInterface::CDrawInterface()
{
    m_pDrawGdi = NULL;
    m_pDrawGdi = new CKdvGDI;
    m_pDrawGdi->InitGDI();
}

/*=============================================================================
  函 数 名： ~CDrawInterface
  功    能： 析构
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
=============================================================================*/
CDrawInterface::~CDrawInterface()
{
    if( NULL == m_pDrawGdi )
    {
        return;
    }
    m_pDrawGdi->QuitGDI();
    delete m_pDrawGdi;
	m_pDrawGdi = NULL;
}

/*=============================================================================
  函 数 名： DrawBmp
  功    能： 生成Bmp图
  算法实现： 
  全局变量： 
  参    数： OUT TPic& tPic 图片
             IN TDrawParam& tDrawParam 制图参数
  返 回 值： u16 
=============================================================================*/
BOOL32 CDrawInterface::DrawBmp( OUT TPic& tPic, IN TDrawParam& tDrawParam )
{
	return m_pDrawGdi->DrawBmp( tPic, tDrawParam );
}