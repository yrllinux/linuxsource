/*****************************************************************************
   模块名      : eqpbase
   文件名      : eqpbase.h
   相关文件    : eqpbase.cpp
   文件实现功能: 提供了外设的共通功能接口
   作者        : 刘旭
   版本        : V0.9  Copyright(C) 2001-2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2011/06/01  4.6         刘旭	       创建
******************************************************************************/

#ifndef MCU_PERIEQP_BASE_H
#define MCU_PERIEQP_BASE_H
 
#include "kdvtype.h"

struct TMt;

extern BOOL32 IsRecEqp(const TMt&);


// class CSwitchBase
// {
// public:
// 	virtual BOOL32 StartSwitchTo( const TSwitchInfo& );
// 	virtual BOOL32 StartSwitchFrom( const TSwitchInfo& );
// 	virtual BOOL32 ChangeSwitch( const TSwitchInfo& );
// 	virtual BOOL32 StopSwitch( const BOOL32 );
// };
// 
// typedef TMt CKdvObj;
// 
// class CNetObj : CKdvObj
// {
// public:
// 	/** 
// 	* @ 功    能@ : 根据通道号获取前向地址(接收地址)
// 	*/ 
// 	virtual TTransportAddr GetFwdAddr( const u16 wChnnlIdx ) const = 0;
// 
// 	/** 
// 	* @ 功    能@ : 根据通道号获取后向地址(发送地址), 一般无用
// 	*/ 
// 	virtual TTransportAddr GetRearAddr( const u16 wChnnlIdx ) const = 0;
// 
// 	/** 
// 	* @ 功    能@ : 获取前向或后续通道数量
// 	*/ 
// 	virtual u16 GetChnnlNum ( const BOOL32 bFwd = TRUE ) const { return 0; }
// 	
// };
// 
// template< u16 wFwdChnnlNum, u16 wRearChnnlNum >
// class CMediaObj : CNetObj
// {
// public:
// 	/** 
// 	* @ 功    能@ : 根据通道号获取前向逻辑通道(接收通道)
// 	*/ 
// 	virtual TLogicalChannel GetFwdAddr( const u16 wChnnlIdx ) const = 0;
// 
// 	/** 
// 	* @ 功    能@ : 根据通道号获取后向逻辑通道(发送通道), 一般无用
// 	*/ 
// 	virtual TLogicalChannel GetRearAddr( const u16 wChnnlIdx ) const = 0;	
// 
// private:
// 
// 	u16				m_wFwdChnnNum;							// 前向通道个数
// 	u16				m_wRearChnnlNum;						// 后向通道个数
// };

#endif