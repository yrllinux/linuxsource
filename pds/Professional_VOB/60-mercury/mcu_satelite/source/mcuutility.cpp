/*****************************************************************************
   模块名      : mcu
   文件名      : mcuutility.cpp
   相关文件    : 
   文件实现功能: MCU业务内部使用结构和类的实现
   作者        : 胡昌威
   版本        : V3.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/11/7   3.0         胡昌威      创建
   2005/02/19  3.6         万春雷      级联修改、与3.5版本合并
******************************************************************************/

#include "mcuvc.h"
#include "evmcu.h"
#include "mcuver.h"
#include "evmcumcs.h"
#include "mcuutility.h"
#include "eqplib.h"
#include "bindmp.h"
#include "bindmtadp.h"
#include "mcupfmlmt.h"
#include "satconst.h"

#include<sys/types.h>   
#include<sys/stat.h>   


SEMHANDLE g_hConfInfoRW = NULL;

//#define SAME_RECVMTPORT_VER
BOOL32 ReadMAPConfig( TMAPParam tMAPParam[], u8 byMapCount );

// Windows 8000B MCU 接口
/*
#ifdef WIN32
#ifdef _MINIMCU_
BOOL32 mixInit (TAudioMixerCfg* tAudioMixerCfg)
{
    return TRUE;
}
BOOL32 vmpinit (TVmpCfg * ptVmpCfg)
{
    return TRUE;
}
BOOL32 basInit (TEqpCfg* ptCfg)
{
    return TRUE;
}

#endif
#endif
*/


/*--------------------------------------------------------------------------*/
/*                                TConfPollParam                            */
/*                               会议轮询参数信息                           */
/*--------------------------------------------------------------------------*/

/*====================================================================
    函数名      ：InitPollParam
    功能        ：设置要被轮询的终端列表及其他参数
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/04/04    4.0         顾振华         创建
====================================================================*/
void TConfPollParam::InitPollParam(u8 byMtNum, TMtPollParam* ptParam)
{
    SetPollList(byMtNum, ptParam) ;

    m_byCurPollPos = 0;
    ClearSpecPos();
}

/*====================================================================
    函数名      SetPollList
    功能        ：设置要被轮询的终端列表
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/04/04    4.0         顾振华         创建
====================================================================*/
void TConfPollParam::SetPollList(u8 byMtNum, TMtPollParam* ptParam)
{
    if ( ptParam == NULL && byMtNum != 0)
    {
        return ;
    }
    m_byPollMtNum = byMtNum > MAXNUM_CONF_MT ? MAXNUM_CONF_MT : byMtNum;
    if ( m_byPollMtNum > 0 )
    {
        memcpy( m_atMtPollParam, ptParam, m_byPollMtNum * sizeof(TMtPollParam) );
    }    
}

/*====================================================================
    函数名      ：GetPolledMtNum
    功能        ：获取轮询列表终端数量
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/04/04    4.0         顾振华         创建
====================================================================*/
u8 TConfPollParam::GetPolledMtNum() const
{
    return m_byPollMtNum;
}

/*====================================================================
    函数名      ：GetPollMtByIdx
    功能        ：获取轮询列表中指定位置的终端参数
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/04/04    4.0         顾振华         创建
====================================================================*/
TMtPollParam* const TConfPollParam::GetPollMtByIdx(u8 byIdx)
{
    if ( byIdx < m_byPollMtNum )
    {
        return &m_atMtPollParam[byIdx];
    }
    else
    {
        return NULL;
    }    
}

/*====================================================================
    函数名      ：SetCurrentIdx
    功能        ：设置当前轮询终端
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/04/04    4.0         顾振华         创建
====================================================================*/
void TConfPollParam::SetCurrentIdx(u8 byIdx)
{
    if ( byIdx < m_byPollMtNum )
    {
        m_byCurPollPos = byIdx;
    }
}


/*====================================================================
    函数名      ：GetCurrentIdx
    功能        ：获取当前在轮询终端索引
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/04/04    4.0         顾振华         创建
====================================================================*/
u8 TConfPollParam::GetCurrentIdx() const
{
    return m_byCurPollPos;
}

/*====================================================================
    函数名      ：GetCurrentMtPolled
    功能        ：获取当前在轮询终端
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/04/04    4.0         顾振华         创建
====================================================================*/
TMtPollParam* const TConfPollParam::GetCurrentMtPolled()
{
    return &m_atMtPollParam[m_byCurPollPos];
}

/*====================================================================
    函数名      ：IsExistMt
    功能        ：在轮询列表中查找指定终端
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/04/04    4.0         顾振华         创建
====================================================================*/
BOOL32 TConfPollParam::IsExistMt(const TMt &tMt, u8 &byIdx)
{
    byIdx = 0xFF;
    u8 byPos = 0;
    for( ; byPos < m_byPollMtNum; byPos++ )
    {
        // guzh [4/5/2007] 这里这么判断是因为相关的其他参数可能不正确
        if ( tMt.GetMcuId() == m_atMtPollParam[byPos].GetMcuId() && 
             tMt.GetMtId() == m_atMtPollParam[byPos].GetMtId() )
        {
            byIdx = byPos;
            return TRUE;
        }
    }

    return FALSE;
}

/*====================================================================
    函数名      ：RemoveMtFromList
    功能        ：从轮询列表中移除指定终端
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/04/04    4.0         顾振华         创建
====================================================================*/
void TConfPollParam::RemoveMtFromList(const TMt &tMt)
{
    u8 byPos = 0;
    for( ; byPos < m_byPollMtNum; byPos++ )
    {
        if ( tMt == m_atMtPollParam[byPos] )
        {            
            // 移动列表
            for( ; byPos < m_byPollMtNum-1; byPos++ )
            {
                m_atMtPollParam[byPos] = m_atMtPollParam[byPos+1];
            }
            m_byPollMtNum--;
            break;
        }
    }
}

/*--------------------------------------------------------------------------*/
/*                              TTvWallPollParam                            */
/*                             电视墙轮询参数信息                           */
/*--------------------------------------------------------------------------*/

/*=============================================================================
函 数 名： GetTvWallEqp
功    能： 获得电视墙设备
算法实现： 
全局变量： 
参    数： void
返 回 值： TEqp* const 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/4/9  4.0			周广程                  创建
=============================================================================*/
TEqp TTvWallPollParam::GetTvWallEqp( void )
{
	return m_tTvWall;
}

/*=============================================================================
函 数 名： SetTvWallEqp
功    能： 设置电视墙设备
算法实现： 
全局变量： 
参    数： TEqp &tTvWall
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/4/9  4.0			周广程                  创建
=============================================================================*/
void TTvWallPollParam::SetTvWallEqp( const TEqp &tTvWall )
{
	m_tTvWall = tTvWall;
}

/*=============================================================================
函 数 名： GetTvWallChnnl
功    能： 获得电视墙通道号
算法实现： 
全局变量： 
参    数： void
返 回 值： u8 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/4/9  4.0			周广程                  创建
=============================================================================*/
u8 TTvWallPollParam::GetTvWallChnnl( void )
{
	return m_byTvWallChnnl;
}

/*=============================================================================
函 数 名： SetTvWallChnnl
功    能： 设置电视墙通道号
算法实现： 
全局变量： 
参    数： u8 byChnIndex
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/4/9  4.0			周广程                  创建
=============================================================================*/
void TTvWallPollParam::SetTvWallChnnl( u8 byChnIndex )
{
	m_byTvWallChnnl = byChnIndex;
}

/*--------------------------------------------------------------------------*/
/*                                TConfMtTable                              */
/*--------------------------------------------------------------------------*/

/*====================================================================
    函数名      ：TConfMtTable
    功能        ：构造函数
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威        创建
  2008/05/06	4.5			张宝卿		  删除: g++编译会因为本实现生成两个汇编构造体，导致异常
====================================================================*/
/*
TConfMtTable::TConfMtTable( void )
{
    m_byMaxNumInUse = 0;
	memset( m_atMtExt, 0, sizeof( m_atMtExt ) );
	memset( m_atMtData, 0, sizeof( m_atMtData ) );	
}*/


/*====================================================================
    函数名      ：SetMtLogicChnnl
    功能        ：设置MT的逻辑通道
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
				  u8 byMediaType, 媒体类型
				  TLogicalChannel * ptChnnl, 逻辑通道配置
				  BOOL32 bForwardChnnl, 流向
    输出参数说明:		无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/04/06    1.0         杨皞昀         创建
====================================================================*/
void TConfMtTable::SetMtLogicChnnl( u8 byMtId, u8 byChannel, const TLogicalChannel * ptChnnl, BOOL32 bForwardChnnl )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptChnnl == NULL )
	{
		return;
	}
	
	switch( byChannel )
	{
	case LOGCHL_VIDEO:
		if( bForwardChnnl )
		{
			m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel = *ptChnnl;
		}
		else
		{
			m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel = *ptChnnl;			
		}
		break;

	case LOGCHL_SECVIDEO:
		if( bForwardChnnl )
		{
			m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel = *ptChnnl;
		}
		else
		{
			m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel = *ptChnnl;			
		}
		break;

	case LOGCHL_AUDIO:
		if( bForwardChnnl )
		{
			m_atMtData[byMtId - 1].m_tFwdChannel.m_tAudioChannel = *ptChnnl;
		}
		else
		{
			m_atMtData[byMtId - 1].m_tRvsChannel.m_tAudioChannel = *ptChnnl;			
		}
		break;

	case LOGCHL_T120DATA:
		if( bForwardChnnl )
		{
			m_atMtData[byMtId - 1].m_tFwdChannel.m_tT120DataChannel = *ptChnnl;
		}
		else
		{
			m_atMtData[byMtId - 1].m_tRvsChannel.m_tT120DataChannel = *ptChnnl;			
		}
		break;

	case LOGCHL_H224DATA:
		if( bForwardChnnl )
		{
			m_atMtData[byMtId - 1].m_tFwdChannel.m_tH224DataChannel = *ptChnnl;
		}
		else
		{
			m_atMtData[byMtId - 1].m_tRvsChannel.m_tH224DataChannel = *ptChnnl;			
		}
		break;
	case LOGCHL_MMCUDATA:
		if( bForwardChnnl )
		{
			m_atMtData[byMtId - 1].m_tFwdChannel.m_tMmcuDataChannel = *ptChnnl;
		}
		else
		{
			m_atMtData[byMtId - 1].m_tRvsChannel.m_tMmcuDataChannel = *ptChnnl;			
		}
		break;

	default:
		OspPrintf( TRUE, FALSE, "Exception - TConfMtData::SetMtLogicChnnl(): wrong byChannel %u!\n", byChannel );
		break;
	}

	return;
}

/*====================================================================
    函数名      ：GetMtLogicChnnl
    功能        ：得到MT的逻辑通道
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
				  u8 byMediaType, 媒体类型
				  TLogicalChannel * ptChnnl, 逻辑通道配置
				  BOOL32 bForwardChnnl, 流向
    输出参数说明:		无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
BOOL32 TConfMtTable::GetMtLogicChnnl( u8 byMtId, u8 byChannel, TLogicalChannel * ptChnnl, BOOL32 bForwardChnnl )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptChnnl == NULL )
	{
		return FALSE;
	}
	
	switch( byChannel )
	{
	case LOGCHL_VIDEO:
		if( bForwardChnnl )
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel;
		}
		else
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel;			
		}
		break;

	case LOGCHL_SECVIDEO:
		if( bForwardChnnl )
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel;
		}
		else
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel;			
		}
		break;

	case LOGCHL_AUDIO:
		if( bForwardChnnl )
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tFwdChannel.m_tAudioChannel;
		}
		else
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tRvsChannel.m_tAudioChannel;			
		}
		break;

	case LOGCHL_T120DATA:
		if( bForwardChnnl )
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tFwdChannel.m_tT120DataChannel;
		}
		else
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tRvsChannel.m_tT120DataChannel;			
		}
		break;

	case LOGCHL_H224DATA:
		if( bForwardChnnl )
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tFwdChannel.m_tH224DataChannel;
		}
		else
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tRvsChannel.m_tH224DataChannel;			
		}
		break;

	case LOGCHL_MMCUDATA:
		if( bForwardChnnl )
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tFwdChannel.m_tMmcuDataChannel;
		}
		else
		{
			*ptChnnl = m_atMtData[byMtId - 1].m_tRvsChannel.m_tMmcuDataChannel;			
		}
		break;

	default:
		OspPrintf( TRUE, FALSE, "Exception - TConfMtTable::GetMtLogicChnnl(): wrong byChannel %u!\n", byChannel );
		return FALSE;
		break;
	}

	return ptChnnl->IsOpened();
}

/*====================================================================
    函数名      ：SetMtSwitchAddr
    功能        ：设置终端的交换地址
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    			  u32 dwRcvIp, 接收Ip
    			  u16 wRcvPort, 接收端口
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/02    2.0         胡昌威         创建
====================================================================*/
void TConfMtTable::SetMtSwitchAddr(u8 byMtId, u32 dwRcvIp, u16 wRcvPort)
{
    if (byMtId == 0 || dwRcvIp == 0 || wRcvPort == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return;
    }

    m_atMtData[byMtId - 1].m_tSwitchAddr.SetIpAddr(dwRcvIp);
    m_atMtData[byMtId - 1].m_tSwitchAddr.SetPort(wRcvPort);

    return;
}

/*====================================================================
    函数名      ：GetMtSwitchAddr
    功能        ：得到终端的交换地址
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    			  u32 &dwRcvIp, 接收Ip
    			  u16 &wRcvPort, 接收端口
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/02    2.0         胡昌威         创建
====================================================================*/
BOOL32 TConfMtTable::GetMtSwitchAddr(u8 byMtId, u32 &dwRcvIp, u16 &wRcvPort)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return FALSE;
    }

    dwRcvIp  = m_atMtData[byMtId - 1].m_tSwitchAddr.GetIpAddr();
    wRcvPort = m_atMtData[byMtId - 1].m_tSwitchAddr.GetPort();

    return (dwRcvIp == 0 || wRcvPort == 0) ? FALSE : TRUE;
}


/*=============================================================================
函 数 名： ClearMtSwitchAddr
功    能： 清空终端交换地址信息
算法实现： 
全局变量： 
参    数： u8 byMtId
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/10/11  4.0			许世林                  创建
=============================================================================*/
void TConfMtTable::ClearMtSwitchAddr(u8 byMtId)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return;
    }

    m_atMtData[byMtId - 1].m_tSwitchAddr.SetIpAddr(0);
    m_atMtData[byMtId - 1].m_tSwitchAddr.SetPort(0);

    return;
}

/*=============================================================================
  函 数 名： SetMtDcsAddr
  功    能： 设置终端的DCS地址
  算法实现： 
  全局变量： 
  参    数：  u8 byMtId
             u32 dwDstIp
             u16 wDstPort
             u8 byMode
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/1/10    4.0			张宝卿                  创建
=============================================================================*/
void TConfMtTable::SetMtDcsAddr( u8 byMtId, u32 dwDstIp, u16 wDstPort, u8 byMode )
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return;
    }
	if (MODE_DATA == byMode)
    {
        m_atMtData[byMtId - 1].m_tDcsAddr.SetIpAddr(dwDstIp);
        m_atMtData[byMtId - 1].m_tDcsAddr.SetPort(wDstPort);
    }
	return;
}

/*=============================================================================
  函 数 名： GetMtDcsAddr
  功    能： 获得终端的DCS地址
  算法实现： 
  全局变量： 
  参    数：  u8 byMtId
             u32 &dwDstIp
             u16 &wDstPort
             u8 byMode
  返 回 值： BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/1/10    4.0			张宝卿                  创建
=============================================================================*/
BOOL32 TConfMtTable::GetMtDcsAddr( u8 byMtId, u32 &dwDstIp, u16 &wDstPort, u8 byMode )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return FALSE;
    }
	
    if (MODE_DATA == byMode)
    {
        dwDstIp = m_atMtData[byMtId - 1].m_tDcsAddr.GetIpAddr();
        wDstPort = m_atMtData[byMtId - 1].m_tDcsAddr.GetPort();
    }

    return (dwDstIp == 0 || wDstPort == 0) ? FALSE : TRUE;
}

/*=============================================================================
  函 数 名： SetMtBRBeLowed
  功    能： 设置当前该终端是否被降速
  算法实现： 
  全局变量： 
  参    数： u8     byMtId
             BOOL32 bLowed
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2008/1/31     4.0			张宝卿                  创建
=============================================================================*/
void TConfMtTable::SetMtBRBeLowed( u8 byMtId, BOOL32 bLowed )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return;
    }
    m_atMtData[byMtId-1].m_bBandWidthLowed = bLowed ? 1 : 0;
    return;
}

/*=============================================================================
  函 数 名： GetMtBRBeLowed
  功    能： 获取当前该终端是否被降速
  算法实现： 
  全局变量： 
  参    数： u8 byMtId
  返 回 值： BOOL32
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2008/1/31     4.0			张宝卿                  创建
=============================================================================*/
BOOL32 TConfMtTable::GetMtBRBeLowed( u8 byMtId )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return FALSE;
    }
    return m_atMtData[byMtId-1].m_bBandWidthLowed == 1 ? TRUE : FALSE;
}


/*=============================================================================
  函 数 名： SetMtTransE1
  功    能： 设置终端线路传输方式是否为E1传输
  算法实现： 
  全局变量： 
  参    数： u8     byMtId
             BOOL32 bTransE1
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2008/1/31     4.0			张宝卿                  创建
=============================================================================*/
void TConfMtTable::SetMtTransE1( u8 byMtId, BOOL32 bTransE1 )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return;
    }
    m_atMtData[byMtId-1].m_bMtE1 = bTransE1 ? 1 : 0;
    return;
}

/*=============================================================================
  函 数 名： GetMtTransE1
  功    能： 获取终端的传输线路是否为E1传输
  算法实现： 
  全局变量： 
  参    数： u8 byMtId
  返 回 值： BOOL32
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2008/1/31     4.0			张宝卿                  创建
=============================================================================*/
BOOL32 TConfMtTable::GetMtTransE1( u8 byMtId )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return FALSE;
    }
    return m_atMtData[byMtId-1].m_bMtE1 == 1 ? TRUE : FALSE;
}


/*=============================================================================
  函 数 名： SetLowedRcvBandWidth
  功    能： 设置终端降速以后的总带宽
  算法实现： 
  全局变量： 
  参    数： u8  byMtId
             u16 wLowedBandWidth
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2008/1/31     4.0			张宝卿                  创建
=============================================================================*/
void TConfMtTable::SetLowedRcvBandWidth( u8 byMtId, u16 wLowedBandWidth )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return;
    }
    m_atMtData[byMtId-1].m_wLowedRcvBandWidth = wLowedBandWidth;
    return;
}

/*=============================================================================
  函 数 名： GetMtTransE1
  功    能： 获取终端降速以后的总带宽
  算法实现： 
  全局变量： 
  参    数： u8 byMtId
  返 回 值： BOOL32
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2008/1/31     4.0			张宝卿                  创建
=============================================================================*/
u16 TConfMtTable::GetLowedRcvBandWidth( u8 byMtId )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return 0;
    }
    return m_atMtData[byMtId-1].m_wLowedRcvBandWidth;
}

/*=============================================================================
  函 数 名： SetRcvBandAdjusted
  功    能： 设置E1终端是否已经调整过了
  算法实现： 
  全局变量： 
  参    数： u8  byMtId
             u16 bAjusted
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2008/9/9     4.0			张宝卿                  创建
=============================================================================*/
void TConfMtTable::SetRcvBandAdjusted(u8 byMtId, BOOL32 bAjusted)
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return;
    }
    m_atMtData[byMtId-1].m_bRcvBandAjusted = bAjusted;
    return;
}

/*=============================================================================
  函 数 名： GetRcvBandAdjusted
  功    能： E1终端是否已经调整过了
  算法实现： 
  全局变量： 
  参    数： u8  byMtId
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2008/9/9     4.0			张宝卿                  创建
=============================================================================*/
BOOL32 TConfMtTable::GetRcvBandAdjusted(u8 byMtId)
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return FALSE;
    }
	return m_atMtData[byMtId-1].m_bRcvBandAjusted;
}

void TConfMtTable::SetMtSigned(u8 byMtId, BOOL32 bSigned)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return;
    }
    m_atMtData[byMtId-1].m_bMtSigned = bSigned;
    return;
}

BOOL32 TConfMtTable::IsMtSigned(u8 byMtId)
{
    return m_atMtData[byMtId-1].m_bMtSigned;
}


void TConfMtTable::SetMtCurrUpLoad(u8 byMtId, BOOL32 bUpLoad)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return;
    }
    m_atMtData[byMtId-1].m_bCurrUpLoad = bUpLoad;
    return;
}

BOOL32 TConfMtTable::IsMtCurrUpLoad(u8 byMtId)
{
    return m_atMtData[byMtId-1].m_bCurrUpLoad;
}

/*====================================================================
    函数名      ：SetMtRtcpDstAddr
    功能        ：设置终端的视频Rtcp目的地址
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    			  u32 dwRcvIp, 接收Ip
    			  u16 wRcvPort, 接收端口
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/10/19    3.0         胡昌威         创建
====================================================================*/
void TConfMtTable::SetMtRtcpDstAddr(u8 byMtId, u32 dwDstIp, u16 wDstPort, u8 byMode)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return;
    }

    if (MODE_VIDEO == byMode)
    {
        m_atMtData[byMtId - 1].m_tRtcpDstAddr.SetIpAddr(dwDstIp);
        m_atMtData[byMtId - 1].m_tRtcpDstAddr.SetPort(wDstPort);
    }
    else if (MODE_AUDIO == byMode)
    {
        m_atMtData[byMtId - 1].m_tAudRtcpDstAddr.SetIpAddr(dwDstIp);
        m_atMtData[byMtId - 1].m_tAudRtcpDstAddr.SetPort(wDstPort);
    }

	return;
}

/*====================================================================
    函数名      ：GetMtRtcpDstAddr
    功能        ：得到终端的视频Rtcp目的地址
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    			  u32 &dwRcvIp, 接收Ip
    			  u16 &wRcvPort, 接收端口
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
     04/10/19    3.0         胡昌威         创建
====================================================================*/
BOOL32 TConfMtTable::GetMtRtcpDstAddr(u8 byMtId, u32 &dwDstIp, u16 &wDstPort, u8 byMode)
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return FALSE;
    }

    if (MODE_VIDEO == byMode)
    {
        dwDstIp = m_atMtData[byMtId - 1].m_tRtcpDstAddr.GetIpAddr();
        wDstPort = m_atMtData[byMtId - 1].m_tRtcpDstAddr.GetPort();
    }
    else if (MODE_AUDIO == byMode)
    {
        dwDstIp = m_atMtData[byMtId - 1].m_tAudRtcpDstAddr.GetIpAddr();
        wDstPort = m_atMtData[byMtId - 1].m_tAudRtcpDstAddr.GetPort();
    }

    return (dwDstIp == 0 || wDstPort == 0) ? FALSE : TRUE;
}

/*====================================================================
    函数名      ：GetSrcSCS
    功能        ：得到终端的源同时能力集
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/18    2.0         胡昌威         创建
====================================================================*/
TSimCapSet TConfMtTable::GetSrcSCS( u8 byMtId )
{
	TSimCapSet tSimCapSet;
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return tSimCapSet;
	}

	if( m_atMtData[byMtId - 1].m_tRvsChannel.m_tAudioChannel.IsOpened() )
	{
		tSimCapSet.SetAudioMediaType( m_atMtData[byMtId - 1].m_tRvsChannel.m_tAudioChannel.GetChannelType() );
	}

	if( m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel.IsOpened() )
	{
		tSimCapSet.SetVideoMediaType( m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel.GetChannelType() );
        tSimCapSet.SetVideoResolution( m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel.GetVideoFormat() );
	}

	return tSimCapSet;
}

/*====================================================================
    函数名      ：GetSrcSCS
    功能        ：得到终端的目地同时能力集
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/18    2.0         胡昌威         创建
====================================================================*/
TSimCapSet TConfMtTable::GetDstSCS( u8 byMtId )
{
	TSimCapSet tSimCapSet;
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return tSimCapSet;
	}

	if( m_atMtData[byMtId - 1].m_tFwdChannel.m_tAudioChannel.IsOpened() )
	{
		tSimCapSet.SetAudioMediaType( m_atMtData[byMtId - 1].m_tFwdChannel.m_tAudioChannel.GetChannelType() );
	}

	if( m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel.IsOpened() )
	{
		tSimCapSet.SetVideoMediaType( m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel.GetChannelType() );
        tSimCapSet.SetVideoResolution( m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel.GetVideoFormat() );
	}

	return tSimCapSet;
}

/*====================================================================
    函数名      ：ClearMtLogicChnnl
    功能        ：清除MT的逻辑通道
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    			  u8 byMediaType, 媒体类型
    			  BOOL32 bForwardChnnl, 流向
 
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/06/04    1.0         JQL         创建
====================================================================*/
void TConfMtTable::ClearMtLogicChnnl( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT  )
	{
		return;
	}

	TLogicalChannel tLogicalChannel;
	m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel = tLogicalChannel;
	m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel = tLogicalChannel;			
	m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel = tLogicalChannel;
	m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel = tLogicalChannel;			
	m_atMtData[byMtId - 1].m_tFwdChannel.m_tAudioChannel = tLogicalChannel;
	m_atMtData[byMtId - 1].m_tRvsChannel.m_tAudioChannel = tLogicalChannel;			
	m_atMtData[byMtId - 1].m_tFwdChannel.m_tT120DataChannel = tLogicalChannel;
	m_atMtData[byMtId - 1].m_tRvsChannel.m_tT120DataChannel = tLogicalChannel;			
	m_atMtData[byMtId - 1].m_tFwdChannel.m_tH224DataChannel = tLogicalChannel;
	m_atMtData[byMtId - 1].m_tRvsChannel.m_tH224DataChannel = tLogicalChannel;			

	return;
}

/*====================================================================
    函数名      ：IsLogicChnnlOpen
    功能        ：判断逻辑通道是否被打开
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
				  u8 byChannel, 通知类型
				  BOOL32 bForwardChnnl 源或者目标
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
BOOL32 TConfMtTable::IsLogicChnnlOpen( u8 byMtId, u8 byChannel, BOOL32 bForwardChnnl )
{
	switch( byChannel )
	{
	case LOGCHL_VIDEO:
		return 	bForwardChnnl ? m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel.IsOpened() 
			                  : m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel.IsOpened();
		break;

	case LOGCHL_SECVIDEO:
		return 	bForwardChnnl ? m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel.IsOpened()
			                  : m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel.IsOpened();
		break;

	case LOGCHL_AUDIO:
		return 	bForwardChnnl ? m_atMtData[byMtId - 1].m_tFwdChannel.m_tAudioChannel.IsOpened()
			                  : m_atMtData[byMtId - 1].m_tRvsChannel.m_tAudioChannel.IsOpened();
		break;

	case LOGCHL_T120DATA:
		return 	bForwardChnnl ? m_atMtData[byMtId - 1].m_tFwdChannel.m_tT120DataChannel.IsOpened()
			                  : m_atMtData[byMtId - 1].m_tRvsChannel.m_tT120DataChannel.IsOpened();
		break;

	case LOGCHL_H224DATA:
		return 	bForwardChnnl ? m_atMtData[byMtId - 1].m_tFwdChannel.m_tH224DataChannel.IsOpened()
			                  : m_atMtData[byMtId - 1].m_tRvsChannel.m_tH224DataChannel.IsOpened();
		break;
		
	default:
		return FALSE;
		break;
	}

	return FALSE;
}

/*====================================================================
    函数名      ：SetMtSrc
    功能        ：设置MT的发送/接收视频或音频源
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端号
				  TMt * ptSrc 终端源
				  u8 byMode 通道类型
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
void TConfMtTable::SetMtSrc( u8 byMtId, const TMt * ptSrc, u8 byMode )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || NULL == ptSrc )
	{
		return;
	}
	
	if( MODE_VIDEO==byMode || MODE_BOTH == byMode )
	{
		m_atMtData[byMtId - 1].m_tSelVSrc = *ptSrc;
		m_atMtData[byMtId - 1].m_tMtStatus.SetVideoMt(*ptSrc);
	}
	
	if( MODE_AUDIO == byMode || MODE_BOTH == byMode )
	{
		m_atMtData[byMtId - 1].m_tSelASrc = *ptSrc;
		m_atMtData[byMtId - 1].m_tMtStatus.SetAudioMt(*ptSrc);
	}
	
	return;
}

/*====================================================================
    函数名      ：GetMtSrc
    功能        ：得到MT的发送/接收视频或音频源
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端号
				  TMt * ptSrc 终端源
				  u8 byMode 通道类型
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
BOOL32 TConfMtTable::GetMtSrc( u8 byMtId, TMt * ptSrc, u8 byMode )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || NULL == ptSrc )
	{
		return FALSE;
	}
	
	if( MODE_VIDEO==byMode )
	{
		*ptSrc = m_atMtData[byMtId - 1].m_tSelVSrc;
	}
	else if( MODE_AUDIO==byMode )
	{
		*ptSrc = m_atMtData[byMtId - 1].m_tSelASrc;
	}
	else
	{
		return FALSE;
	}
	
	return TRUE;
}

/*====================================================================
    函数名      ：SetMtStatus
    功能        ：设置MT状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端号
				  const TMtStatus * ptStatus 终端状态
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
void TConfMtTable::SetMtStatus( u8 byMtId, const TMtStatus * ptStatus )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptStatus == NULL )
	{
		return;
	}
	
	m_atMtData[byMtId - 1].m_tMtStatus = *ptStatus;

	return;	
}

/*====================================================================
    函数名      ：GetMtStatus
    功能        ：得到终端状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
				  TMtStatus * ptStatus, 终端状态
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
BOOL32 TConfMtTable::GetMtStatus( u8 byMtId, TMtStatus * ptStatus )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptStatus == NULL )
	{
		return FALSE;
	}
	
	*ptStatus = m_atMtData[byMtId - 1].m_tMtStatus;
	return TRUE;
}

/*====================================================================
    函数名      ：SetMtCapSupport
    功能        ：设置终端能力集
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
				  const TCapSupport * ptCapSupport, 终端能力集
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
void TConfMtTable::SetMtCapSupport( u8 byMtId, const TCapSupport * ptCapSupport )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptCapSupport == NULL )
	{
		return ;
	}
    
	m_atMtData[byMtId - 1].m_tCapSupport = *ptCapSupport;

	return;
}


/*====================================================================
    函数名      ：GetMtCapSupport
    功能        ：得到终端的能力集
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
				  TCapSupport * ptCapSupport, 终端能力集
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
BOOL32 TConfMtTable::GetMtCapSupport( u8 byMtId, TCapSupport * ptCapSupport )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptCapSupport == NULL )
	{
		return FALSE;
	}
	
	*ptCapSupport = m_atMtData[byMtId - 1].m_tCapSupport;
	return TRUE;
}

/*====================================================================
    函数名      ：SetMtCapSupportEx
    功能        ：获取终端的扩展能力集
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
				  TCapSupportEx * ptCapSupport, 终端扩展能力集
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/10/13    4.0         张宝卿         创建
====================================================================*/
void TConfMtTable::SetMtCapSupportEx( u8 byMtId, const TCapSupportEx * ptCapSupport )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptCapSupport == NULL )
    {
        return ;
    }
    
    m_atMtData[byMtId - 1].m_tCapSupportEx = *ptCapSupport;
    
    return;
}

/*====================================================================
    函数名      ：GetMtCapSupportEx
    功能        ：得到终端的扩展能力集
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
				  TCapSupportEx * ptCapSupport, 终端扩展能力集
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/10/13    4.0         张宝卿         创建
====================================================================*/
BOOL32 TConfMtTable::GetMtCapSupportEx( u8 byMtId, TCapSupportEx * ptCapSupport )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptCapSupport == NULL )
    {
        return FALSE;
    }
    
    *ptCapSupport = m_atMtData[byMtId - 1].m_tCapSupportEx;
    return TRUE;
}
/*====================================================================
    函数名      ：SetMtCapSpecByMCS
    功能        ：设置终端能力级是否由会控干预设定
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId           终端号
	              u8 bySpecByMCS      TRUE: MCS干预终端能力级
				                      FALSE:由终端自己指定
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/10/30    3.0         付秀华         创建
====================================================================*/
void TConfMtTable::SetMtCapSpecByMCS( u8 byMtId, u8 bySpecByMCS)
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT)
	{
		return ;
	}
    
	m_atMtData[byMtId - 1].m_byMTCapSpecByMCS = bySpecByMCS;

	return;
}
/*====================================================================
    函数名      ：IsMtCapSpecByMCS
    功能        ：终端能力级是否由会控来指定
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId           终端号
    返回值说明  ：u8 TRUE: 由会控指定
	                 FALSE:由终端自己指定
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08/10/30    3.0         付秀华         创建
====================================================================*/
u8 TConfMtTable::IsMtCapSpecByMCS( u8 byMtId)
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT)
	{
		return FALSE;
	}
    
	return m_atMtData[byMtId - 1].m_byMTCapSpecByMCS;

}
/*====================================================================
    函数名      ：SetMtAlias
    功能        ：设置终端别名
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
				  const TMtAlias * ptAlias, 终端别名
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
BOOL32 TConfMtTable::SetMtAlias( u8 byMtId, const TMtAlias * ptAlias )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptAlias == NULL )
	{
		return FALSE;
	}

	if( ptAlias->m_AliasType == 0 || ptAlias->m_AliasType > mtAliasTypeOthers)
	{
		return FALSE;
	}

	m_atMtData[byMtId - 1].m_atMtAlias[ptAlias->m_AliasType - 1] = *ptAlias;

	TMtAlias tMtAlias;
	if(!GetMtAlias( byMtId, mtAliasTypeH320Alias, &tMtAlias ))//TMtEx中H320Alias优先于H320Id
	{
        if(!GetMtAlias( byMtId, mtAliasTypeH320ID, &tMtAlias ))
        {
            if(!GetMtAlias( byMtId, mtAliasTypeH323ID, &tMtAlias ))
            {
                if(!GetMtAlias( byMtId, mtAliasTypeE164, &tMtAlias ))
                {
                    GetMtAlias( byMtId, mtAliasTypeTransportAddress, &tMtAlias );
                }
            }
        }		
	}

	if( tMtAlias.m_AliasType == mtAliasTypeH323ID || 
		tMtAlias.m_AliasType == mtAliasTypeE164 || 
		tMtAlias.m_AliasType == mtAliasTypeH320ID ||
        tMtAlias.m_AliasType == mtAliasTypeH320Alias)
	{
		m_atMtExt[byMtId - 1].SetAlias( tMtAlias.m_achAlias );
	}
	else if( tMtAlias.m_AliasType == mtAliasTypeTransportAddress )
	{
		m_atMtExt[byMtId - 1].SetAlias( StrOfIP( tMtAlias.m_tTransportAddr.GetIpAddr() ) );
	}
	else
	{
		return FALSE;
	}

    if( tMtAlias.m_AliasType == mtAliasTypeH320ID ||
        tMtAlias.m_AliasType == mtAliasTypeH320Alias)
    {
        m_atMtExt[byMtId - 1].SetProtocolType(PROTOCOL_TYPE_H320);
    }
    else
    {
        m_atMtExt[byMtId - 1].SetProtocolType(PROTOCOL_TYPE_H323);
    }
	
	return TRUE;
}

/*====================================================================
    函数名      ：GetMtAlias
    功能        ：得到终端别名
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
				  mtAliasType AliasType, 别名类型
				  TMtAlias * ptAlias, 终端别名
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
BOOL32 TConfMtTable::GetMtAlias( u8 byMtId, mtAliasType AliasType, TMtAlias * ptAlias )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptAlias == NULL || 
		AliasType == 0 || AliasType > mtAliasTypeOthers )
	{
		return FALSE;
	}
	
	TMtAlias tMtAlias = m_atMtData[byMtId - 1].m_atMtAlias[ AliasType - 1 ].GetMtAlias();
	if( (tMtAlias.m_AliasType == mtAliasTypeTransportAddress) && 
		(tMtAlias.m_tTransportAddr.GetIpAddr() == 0) )
	{
		return FALSE;
	}

	if( (tMtAlias.m_AliasType != mtAliasTypeTransportAddress) && 
		(tMtAlias.m_achAlias[0] == 0) )
	{
		return FALSE;
	}

	*ptAlias = tMtAlias;
	
	return TRUE;

}


/*====================================================================
    函数名      ：GetMtAlias
    功能        ：得到终端别名
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
				  mtAliasType AliasType, 别名类型
				  TMtAlias * ptAlias, 终端别名
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
s8* TConfMtTable::GetMtAliasFromExt( u8 byMtId ) const
{
	if( byMtId <= 0 || byMtId > MAXNUM_CONF_MT )
	{
		return NULL;
	}
	
    return m_atMtExt[byMtId - 1].GetAlias();
}


/*====================================================================
    函数名      ：SetPuAlias
    功能        ：设置PU别名
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, PU号
				  const TMtAlias * ptAlias, PU别名
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/02/27    3.0         付秀华         创建
====================================================================*/
BOOL32 TConfMtTable::SetPuAlias( u8 byMtId, const TMtAlias * ptAlias )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptAlias == NULL )
	{
		return FALSE;
	}

	if( ptAlias->m_AliasType != puAliasTypeIPPlusAlias ) 
	{
		return FALSE;
	}

	// 对pu该处信息不保存，等待呼叫后刷新
	// m_atMtData[byMtId - 1].m_atMtAlias[ptAlias->m_AliasType - 1] = *ptAlias;

	// PU特殊别名IP+ALIAS
	s8* pbyAlias = (s8*)(ptAlias->m_achAlias);
	m_atMtExt[byMtId - 1].SetIPAddr(ntohl(*(u32*)pbyAlias));
	pbyAlias += sizeof(u32);
	m_atMtExt[byMtId - 1].SetAlias( pbyAlias );

    m_atMtExt[byMtId - 1].SetProtocolType(PROTOCOL_TYPE_H323);
	
	return TRUE;
}
/*====================================================================
    函数名      ：SetNotInvited
    功能        ：设置该终端为未被邀请的
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端号
				  BOOL32 bNotInvited 是否被邀请
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/21    3.0         胡昌威         创建
====================================================================*/
void TConfMtTable::SetNotInvited( u8 byMtId, BOOL32 bNotInvited )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return ;
	}

	m_atMtData[byMtId - 1].m_bNotInvited = bNotInvited;

	return;
}


/*=============================================================================
  函 数 名： SetMtInDataConf
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 byMtId
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/1/9    4.0			张宝卿                  创建
=============================================================================*/
void TConfMtTable::SetMtInDataConf( u8 byMtId, BOOL32 bInDataConf )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return ;
    }

	m_atMtData[byMtId-1].m_bInDataConf = bInDataConf;
}

/*=============================================================================
  函 数 名： IsMtInDataConf
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 byMtId
  返 回 值： BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/1/9    4.0			张宝卿                  创建
=============================================================================*/
BOOL32 TConfMtTable::IsMtInDataConf( u8 byMtId )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return FALSE;
    }

	return m_atMtData[byMtId-1].m_bInDataConf;
}

/*=============================================================================
  函 数 名： SetMtCalling
  功    能： 设置终端是否终端呼叫入会
  算法实现： 
  全局变量： 
  参    数： u8 byMtId
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/1/9    4.0			张宝卿                  创建
=============================================================================*
void TConfMtTable::SetMtCallingIn( u8 byMtId, BOOL32 bCallingIn )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return ;
    }

	m_atMtData[byMtId-1].m_bCallingIn = bCallingIn;
}*/

/*=============================================================================
  函 数 名： IsMtCalling
  功    能： 终端是否终端呼叫入会
  算法实现： 
  全局变量： 
  参    数： u8 byMtId
  返 回 值： BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/1/9    4.0			张宝卿                  创建
=============================================================================*
BOOL32 TConfMtTable::IsMtCallingIn( u8 byMtId )
{	
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return FALSE;
    }

	return m_atMtData[byMtId-1].m_bCallingIn;
}*/

/*====================================================================
    函数名      ：IsNotInvited
    功能        ：得到该终端是否未被邀请
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端号
    返回值说明  ：TRUE - 未被邀请 FALSE - 被邀请
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/21    3.0         胡昌威         创建
====================================================================*/
BOOL32 TConfMtTable::IsNotInvited(  u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

    return m_atMtData[byMtId - 1].m_bNotInvited;
}

/*====================================================================
    函数名      ：SetAddMtMode
    功能        ：设置终端被邀请的方式(1: 终端是被MCS邀请的 2: 终端是被主席邀请的)
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId      --- 终端号
                  u8 byAddMtMode --- 要设置的终端被邀请的方式
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/03/30    3.6          李博         创建
====================================================================*/
void TConfMtTable::SetAddMtMode(u8 byMtId, u8 byAddMtMode)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
	{
		return ;
	}

	m_atMtData[byMtId - 1].m_byAddMtMode = byAddMtMode;

	return;
}

/*====================================================================
    函数名      ：GetAddMtMode
    功能        ：得到该终端被邀请的方式
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId      --- 终端号
    返回值说明  ：1: 终端是被MCS邀请的 2: 终端是被主席邀请的
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/03/30    3.6          李博         创建
====================================================================*/
u8   TConfMtTable::GetAddMtMode(u8 byMtId)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
	{
		return FALSE;
	}

    return m_atMtData[byMtId - 1].m_byAddMtMode;
}

/*====================================================================
    函数名      ：IsMtMulticasting
    功能        ：得到该终端是否正在组播
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端号
    返回值说明  ：TRUE - 组播 FALSE - 未组播
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/27    3.0         胡昌威         创建
====================================================================*/
BOOL32 TConfMtTable::IsMtMulticasting( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

    return m_atMtData[byMtId - 1].m_bMulticasting;
}

/*====================================================================
    函数名      ：SetMtMulticasting
    功能        ：设置该终端是否正在组播
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端号
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/27    3.0         胡昌威         创建
====================================================================*/
void TConfMtTable::SetMtMulticasting( u8 byMtId, BOOL32 bMulticasting )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return ;
	}

	m_atMtData[byMtId - 1].m_bMulticasting = bMulticasting;

	return;
}

/*====================================================================
    函数名      ：AddMonitorMt
    功能        ：增加监控终端
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端号
				  u8 byMonitorMtId 被监控终端号
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/27    3.0         胡昌威         创建
====================================================================*/
void TConfMtTable::AddMonitorSrcMt( u8 byMtId, u8 byMonitorMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return ;
	}

	//查找该终端是否已在监控列表中
	u8 byMtIndex = 0;
	while( byMtIndex < MAXNUM_MT_CHANNL )
	{
		if( m_atMtData[byMtId - 1].m_byMonitorSrcMt[byMtIndex] == byMonitorMtId )
		{
			break;
		}
		else
		{
			byMtIndex++;
		}
	}

	//增加
	if( byMtIndex == MAXNUM_MT_CHANNL )
	{
		byMtIndex = 0;
		while( byMtIndex < MAXNUM_MT_CHANNL )
		{
			if( m_atMtData[byMtId - 1].m_byMonitorSrcMt[byMtIndex] == 0 )
			{
				m_atMtData[byMtId - 1].m_byMonitorSrcMt[byMtIndex] = byMonitorMtId;
				return;
			}
			else
			{
				byMtIndex++;
			}
		}
	}
	
	return;
}

/*====================================================================
    函数名      ：RemoveMonitorMt
    功能        ：移除监控终端
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端号
				  u8 byMonitorMtId 被监控终端号
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/27    3.0         胡昌威         创建
====================================================================*/
void TConfMtTable::RemoveMonitorSrcMt( u8 byMtId, u8 byMonitorMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return ;
	}

	u8 byMtIndex = 0;
	while( byMtIndex < MAXNUM_MT_CHANNL )
	{
		if( m_atMtData[byMtId - 1].m_byMonitorSrcMt[byMtIndex] == byMonitorMtId )
		{
			m_atMtData[byMtId - 1].m_byMonitorSrcMt[byMtIndex] = 0;
			return;
		}
		else
		{
			byMtIndex++;
		}
	}

	return;
}

/*====================================================================
    函数名      ：GetMonitorSrcMtList
    功能        ：得到监控源终端列表
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端号
				  u8 byMtList 终端列表
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/27    3.0         胡昌威         创建
====================================================================*/
u8   TConfMtTable::GetMonitorSrcMtList( u8 byMtId, u8 byMtList[] )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}

	u8 byMtIndex = 0;
	for( u8 byLoop = 0; byLoop < MAXNUM_MT_CHANNL; byLoop++  )
	{
		if( m_atMtData[byMtId - 1].m_byMonitorSrcMt[byLoop] != 0 )
		{
			byMtList[byMtIndex] = m_atMtData[byMtId - 1].m_byMonitorSrcMt[byLoop];
			byMtIndex++;
		}
	}

	return byMtIndex;
}

/*====================================================================
    函数名      ：AddMonitorDstMt
    功能        ：移除监控终端
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端号
				  u8 byDstMtId 监控终端号
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/27    3.0         胡昌威         创建
====================================================================*/
void TConfMtTable::AddMonitorDstMt( u8 byMtId, u8 byDstMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return ;
	}

	m_atMtData[byMtId - 1].m_byMonitorDstMt[(byDstMtId-1) / 8] |= 1 << ( (byDstMtId-1) % 8 );

	return;
}

/*====================================================================
    函数名      ：RemoveMonitorDstMt
    功能        ：移除监控终端
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端号
				  u8 byMonitorMtId 监控终端号
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/27    3.0         胡昌威         创建
====================================================================*/
void TConfMtTable::RemoveMonitorDstMt( u8 byMtId, u8 byDstMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return ;
	}

	m_atMtData[byMtId - 1].m_byMonitorDstMt[(byDstMtId-1) / 8] &= ~( 1 << ( (byDstMtId-1) % 8 ) );

	return;
}

/*====================================================================
    函数名      ：RemoveMonitorMt
    功能        ：移除监控终端
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端号
				  u8 byMonitorMtId 被监控终端号
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/27    3.0         胡昌威         创建
====================================================================*/
BOOL32 TConfMtTable::HasMonitorDstMt( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

	u8 byLoop = 0;
	while( byLoop < MAXNUM_MT_CHANNL )
	{
		if( m_atMtData[byMtId - 1].m_byMonitorDstMt[byLoop] != 0 )
		{
			return TRUE;
		}
		else
		{
			byLoop++;
		}
	}

	return FALSE;
}

/*====================================================================
    函数名      ：ClearMonitorSrcAndDst
    功能        ：移除监控终端
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端号
				  u8 byMonitorMtId 被监控终端号
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/04/27    3.0         胡昌威         创建
====================================================================*/
void TConfMtTable::ClearMonitorSrcAndDst( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return ;
	}

	memset( m_atMtData[byMtId - 1].m_byMonitorDstMt, 0, sizeof(m_atMtData[byMtId - 1].m_byMonitorDstMt) ); 
	memset( m_atMtData[byMtId - 1].m_byMonitorSrcMt, 0, sizeof(m_atMtData[byMtId - 1].m_byMonitorSrcMt) ); 

	return;
}

/*====================================================================
    函数名      ：SetMpId
    功能        ：设置终端Mp号
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
				  u8 byMpId, 终端Mp号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/06/02    3.0         胡昌威         创建
====================================================================*/
void TConfMtTable::SetMpId( u8 byMtId, u8 byMpId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || byMpId > MAXNUM_DRI)
	{
		return;
	}
	m_atMtData[byMtId - 1].m_byMpId = byMpId;

	return;
}

/*====================================================================
    函数名      ：GetMpId
    功能        ：得到终端MP号 
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/06/02    3.0         胡昌威         创建
====================================================================*/
u8 TConfMtTable::GetMpId( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
	return m_atMtData[byMtId - 1].m_byMpId;
}

/*====================================================================
    函数名      ：SetLastTick
    功能        ：设置终端的最近的FastUpdatePic的Tick数
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
				  u32 dwLastTick, Tick数
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/06/02    3.0         胡昌威         创建
====================================================================*/
void TConfMtTable::SetLastTick( u8 byMtId, u32 dwLastTick, BOOL32 bSecVideo )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	if( bSecVideo )
	{
        m_atMtData[byMtId - 1].m_dwSecVideoLastRefreshTick = dwLastTick;
	}
	else
	{
        m_atMtData[byMtId - 1].m_dwLastRefreshTick = dwLastTick;
	}
	
}

/*====================================================================
    函数名      ：GetLastTick
    功能        ：得到终端的最近的FastUpdatePic的Tick数
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/06/02    3.0         胡昌威         创建
====================================================================*/
u32   TConfMtTable::GetLastTick( u8 byMtId, BOOL32 bSecVideo )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}

	if( bSecVideo )
	{
        return m_atMtData[byMtId - 1].m_dwSecVideoLastRefreshTick;
	}
	else
	{
        return m_atMtData[byMtId - 1].m_dwLastRefreshTick;
	}
		
}

/*====================================================================
    函数名      ：SetMtReqBitrate
    功能        ：设置该终端要求接收的码率
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端号
	              u16 wBitRate
	              u8 byChnlMode = LOGCHL_VIDEO_BOTH
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/02/12    3.0         胡昌威         创建
====================================================================*/
void TConfMtTable::SetMtReqBitrate( u8 byMtId, u16 wBitRate, u8 byChnlMode )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	if( LOGCHL_VIDEO_BOTH == byChnlMode || LOGCHL_VIDEO == byChnlMode )
	{
		m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel.SetFlowControl( wBitRate );
	}
	if( LOGCHL_VIDEO_BOTH == byChnlMode || LOGCHL_SECVIDEO == byChnlMode )
	{
		m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel.SetFlowControl( wBitRate );
	}

	return;
}

/*====================================================================
    函数名      ：GetMtReqBitrate
    功能        ：得到该终端要求接收的码率
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端号
	              bPrimary = TRUE
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/02/12    3.0         胡昌威         创建
====================================================================*/
u16  TConfMtTable::GetMtReqBitrate( u8 byMtId, BOOL32 bPrimary )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}

	if( TRUE == bPrimary )
	{
		return m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel.GetFlowControl();
	}
	else
	{
		return m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel.GetFlowControl();
	}
}

void TConfMtTable::SetMtSndBitrate( u8 byMtId, u16 wBitRate )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel.SetFlowControl( wBitRate );

	return;
}

u16  TConfMtTable::GetMtSndBitrate( u8 byMtId, u8 byChannel /*= LOGCHL_VIDEO*/ )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}

    if (LOGCHL_VIDEO == byChannel)
    {
        return m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel.GetFlowControl();
    }
    else if (LOGCHL_SECVIDEO == byChannel)
    {
        return m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel.GetFlowControl();
    }
    else
    {
        OspPrintf(TRUE, FALSE, "[GetMtSndBitrate] get mt sending bitrate error!!!\n");
        return 0;
    }
}

/*====================================================================
    函数名      ：SetMcuId
    功能        ：设置终端的Mcu Id
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端号
				  u8 byMcuId MCU号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
void TConfMtTable::SetMcuId( u8 byMtId, u8 byMcuId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	m_atMtExt[byMtId - 1].SetMcuId( byMcuId );

	return;
}

/*====================================================================
    函数名      ：GetMcuId
    功能        ：得到终端的MCU号
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
u8 TConfMtTable::GetMcuId( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}

	return m_atMtExt[byMtId - 1].GetMcuId();
}

/*====================================================================
    函数名      ：SetDriId
    功能        ：设置终侄DRI号
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
				  u8 byDriId, 终端DRI号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
void TConfMtTable::SetDriId( u8 byMtId, u8 byDriId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || byDriId > MAXNUM_DRI)
	{
		return;
	}
	m_atMtExt[byMtId - 1].SetDriId( byDriId );

	return;
}

/*====================================================================
    函数名      ：GetDriId
    功能        ：得到终端DRI号
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
u8 TConfMtTable::GetDriId( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
	return m_atMtExt[byMtId - 1].GetDriId();
}

/*====================================================================
    函数名      ：SetConfIdx
    功能        ：设置终端会议索引号
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
				  u8 byConfIdx, 终端会议索引号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
void TConfMtTable::SetConfIdx(u8 byMtId, u8 byConfIdx )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	m_atMtExt[byMtId - 1].SetConfIdx( byConfIdx );

	return;
}

/*====================================================================
    函数名      ：GetConfIdx
    功能        ：得到终端会议索引号
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
u8 TConfMtTable::GetConfIdx( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
	return m_atMtExt[byMtId - 1].GetConfIdx();
}

/*====================================================================
    函数名      ：SetMtType
    功能        ：设置终端类型
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
				  u8 byMtType, 终端类型
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
void TConfMtTable::SetMtType( u8 byMtId, u8 byMtType )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	m_atMtExt[byMtId - 1].SetMtType( byMtType );

	return;
}

/*====================================================================
    函数名      ：GetMtType
    功能        ：得到终端类型
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
u8 TConfMtTable::GetMtType( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
	return m_atMtExt[byMtId - 1].GetMtType();
}

/*====================================================================
    函数名      ：SetMainType
    功能        ：设置终端类型
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
				  u8 byType, 终端类型
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/03/20    3.6         魏治兵         创建
====================================================================*/
void TConfMtTable::SetMainType( u8 byMtId, u8 byType )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	m_atMtExt[byMtId - 1].SetType( byType );
	return;
}

/*====================================================================
    函数名      ：GetMainType
    功能        ：得到终端类型
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         魏治兵         创建
====================================================================*/
u8 TConfMtTable::GetMainType( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
	return m_atMtExt[byMtId - 1].GetType();
}

/*====================================================================
    函数名      ：SetManuId
    功能        ：设置终端制造厂商Id
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
				  u8 byManuId, 终端制造厂商Id
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
void TConfMtTable::SetManuId( u8 byMtId, u8 byManuId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	m_atMtExt[byMtId - 1].SetManuId( byManuId );

	return;
}

/*====================================================================
    函数名      ：GetManuId
    功能        ：得到终端制造厂商Id
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
u8 TConfMtTable::GetManuId( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
	
	//FIXME: 总后临时修改，稍后走正常的协商处理流程
#ifdef _SATELITE_
	return MT_MANU_KDC;
#else
	return m_atMtExt[byMtId - 1].GetManuId();
#endif
}

/*=============================================================================
  函 数 名： GetProtocolType
  功    能： 取终端协议类型
  算法实现： 
  全局变量： 
  参    数： u8 byMtId
  返 回 值： u8 
 ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    06/02/17    4.0         john           创建
=============================================================================*/
u8 TConfMtTable::GetProtocolType( u8 byMtId )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
    return m_atMtExt[byMtId - 1].GetProtocolType();
}

/*=============================================================================
  函 数 名： SetProtocolType
  功    能： 设置终端协议类型
  算法实现： 
  全局变量： 
  参    数： u8 byMtId
             u8 byProtocolType
  返 回 值： void 
  ----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    06/02/17    4.0         john           创建
=============================================================================*/
void TConfMtTable::SetProtocolType(u8 byMtId, u8 byProtocolType )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return ;
	}
    m_atMtExt[byMtId - 1].SetProtocolType( byProtocolType );
}
/*====================================================================
    函数名      ：SetCallMode
    功能        ：设置终端呼叫方式
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
				  u8 byCallMode, 终端呼叫方式
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
void TConfMtTable::SetCallMode( u8 byMtId, u8 byCallMode )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	m_atMtExt[byMtId - 1].SetCallMode( byCallMode );

	return;
}

/*====================================================================
    函数名      ：GetCallMode
    功能        ：得到终端呼叫方式
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
u8 TConfMtTable::GetCallMode( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
	return m_atMtExt[byMtId - 1].GetCallMode();
}

/*=============================================================================
    函 数 名： SetCallLeftTimes
    功    能： 设置终端剩余呼叫次数
    算法实现： 
    全局变量： 
    参    数： u8  byMtId
               u32 dwCallTimes
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/03/18  3.6			万春雷                  创建
=============================================================================*/
void TConfMtTable::SetCallLeftTimes( u8 byMtId, u32 dwCallLeftTimes )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	m_atMtExt[byMtId - 1].SetCallLeftTimes( dwCallLeftTimes );

	return;
}

/*=============================================================================
    函 数 名： GetCallLeftTimes
    功    能： 获取终端剩余呼叫次数
    算法实现： 
    全局变量： 
    参    数：u8 byMtId
    返 回 值：u32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/03/18  3.6			万春雷                  创建
=============================================================================*/
u32 TConfMtTable::GetCallLeftTimes( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
	return m_atMtExt[byMtId - 1].GetCallLeftTimes();
}

/*====================================================================
    函数名      ：SetIPAddr
    功能        ：设置终端IP地址
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
				  u32 dwIPAddr, IP地址
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
void TConfMtTable::SetIPAddr( u8 byMtId, u32 dwIPAddr )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	m_atMtExt[byMtId - 1].SetIPAddr( dwIPAddr );

	return;
}

/*====================================================================
    函数名      ：GetIPAddr
    功能        ：得到终端IP地址
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
u32 TConfMtTable::GetIPAddr( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
//zhangs if IP not present,this cause a exception
	return m_atMtExt[byMtId - 1].GetIPAddr();
}

/*====================================================================
    函数名      ：GetMt
    功能        ：得到终端结构
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
TMt TConfMtTable::GetMt( u8 byMtId )
{
    TMt tMt;
	tMt.SetNull();
	
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return tMt;
	}

    tMt = (TMt)m_atMtExt[byMtId - 1];
	return tMt;
}

/*====================================================================
    函数名      ：AddMt
    功能        ：增加终端(根据终端IP)
    算法实现    ：
    引用全局变量：
    输入参数说明：u32 dwIpAddr,        终端IP地址
				  BOOL32  bRepeatFilter  是否过滤别名重复，否则进行强制添加
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
u8 TConfMtTable::AddMt( u32 dwIpAddr, BOOL32 bRepeatFilter )
{
    if( dwIpAddr == 0 )return 0;

	u8 byMtId = GetMtIdByIp( dwIpAddr );

	//已存在 并且 过滤别名重复, 避免反复添加
	if( byMtId > 0 && bRepeatFilter )
	{
		return 0;
	}

	TMtAlias tMtAlias;
	tMtAlias.m_AliasType = mtAliasTypeTransportAddress;
    tMtAlias.m_tTransportAddr.SetIpAddr( dwIpAddr );
	tMtAlias.m_tTransportAddr.SetPort( DEFAULT_Q931_PORT );
  
	byMtId = 1;
	while( byMtId <= MAXNUM_CONF_MT )
	{
	    if( m_atMtExt[byMtId - 1].GetMtId() == 0 && byMtId != LOCAL_MCUID)//找到空位
		{
            m_atMtExt[byMtId - 1].SetMtId( byMtId );
			m_atMtExt[byMtId - 1].SetIPAddr( dwIpAddr );
			SetMtAlias( byMtId, &tMtAlias );
            if(byMtId > m_byMaxNumInUse)
			{
				m_byMaxNumInUse = byMtId;
			}
			break;
		}
	     byMtId++;
	}

	if( byMtId == MAXNUM_CONF_MT + 1 )//会议中终端满
	{
		return 0;
	}
	return byMtId;
}

/*====================================================================
    函数名      ：AddMt
    功能        ：增加终端(根据终端别名)
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMtAlias * ptAlias, 终端别名
				  BOOL32  bRepeatFilter       是否过滤别名重复，否则进行强制添加
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
u8 TConfMtTable::AddMt( TMtAlias * ptAlias, BOOL32 bRepeatFilter )
{
    if( ptAlias == NULL || ptAlias->m_AliasType == 0 || 
		(ptAlias->m_AliasType > mtAliasTypeOthers && 
		 //pu特殊类型
		 ptAlias->m_AliasType != puAliasTypeIPPlusAlias))
	{
		return 0;
	}

	u8 byMtId = GetMtIdByAlias( ptAlias );

	//已存在 并且 过滤别名重复, 避免反复添加
	if( byMtId > 0 && bRepeatFilter )
	{
		return 0;
	}
  
	byMtId = 1;
	while( byMtId <= MAXNUM_CONF_MT )
	{
	    if( m_atMtExt[byMtId - 1].GetMtId() == 0 && byMtId != LOCAL_MCUID )        //找到空位
		{
            m_atMtExt[byMtId - 1].SetMtId( byMtId );
			if( ptAlias->m_AliasType == mtAliasTypeTransportAddress )
			{
				m_atMtExt[byMtId - 1].SetIPAddr( ptAlias->m_tTransportAddr.GetIpAddr() );
				if( 0 == ptAlias->m_tTransportAddr.GetPort() )
				{
					ptAlias->m_tTransportAddr.SetPort( DEFAULT_Q931_PORT );
				}
			}
			
			// pu设置别名,并填写IP
			if ( puAliasTypeIPPlusAlias == ptAlias->m_AliasType )
			{
				SetPuAlias( byMtId, ptAlias );
			}
			else
			{
				SetMtAlias( byMtId, ptAlias );
			}
			if(byMtId > m_byMaxNumInUse)
			{
				m_byMaxNumInUse = byMtId;
			}
			break;
		}
	    byMtId++;
	}

	if( byMtId == MAXNUM_CONF_MT + 1 )//会议中终端满
	{
		return 0;
	}

	return byMtId;
}

/*====================================================================
    函数名      ：SetDialAlias
    功能        ：设置拨号的别名
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMtAlias * ptAlias, 终端别名
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/03/10    3.6        Jason        创建
====================================================================*/
void TConfMtTable::SetDialAlias( u8 byMtId, const TMtAlias * ptAlias )
{
    if( NULL == ptAlias || 0 == ptAlias->m_AliasType || 
		(ptAlias->m_AliasType > mtAliasTypeOthers &&
		 ptAlias->m_AliasType != puAliasTypeIPPlusAlias)||
		byMtId > MAXNUM_CONF_MT || byMtId <= 0 )
	{
		return;
	}

	if (puAliasTypeIPPlusAlias == ptAlias->m_AliasType)
	{
		// 重新整理pu别名
		TMtAlias tMtAlias;
		s8* pachAlias = (s8*)(ptAlias->m_achAlias);
		tMtAlias.m_tTransportAddr.SetIpAddr(ntohl(*(u32*)(pachAlias)));
 		tMtAlias.m_tTransportAddr.SetPort(MCU_Q931_PORT);
		pachAlias += sizeof(u32);
 		tMtAlias.SetH323Alias(pachAlias);
		tMtAlias.m_AliasType = ptAlias->m_AliasType;
		m_atMtData[byMtId - 1].m_tDialAlias = tMtAlias;
	}
	else
	{
		m_atMtData[byMtId - 1].m_tDialAlias = *ptAlias;
	}
}

/*====================================================================
    函数名      ：GetDialAlias
    功能        ：得到拨号的别名
    算法实现    ：
    引用全局变量：
    输入参数说明：const TMtAlias * ptAlias, 终端别名
    返回值说明  ：true or false
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/03/10    3.6        Jason        创建
====================================================================*/
BOOL32 TConfMtTable::GetDialAlias( u8 byMtId, TMtAlias * ptAlias )
{
    if( NULL == ptAlias ||
		byMtId > MAXNUM_CONF_MT || byMtId <= 0 )
	{
		return FALSE;
	}

    *ptAlias = m_atMtData[byMtId - 1].m_tDialAlias.GetMtAlias();
   
	return TRUE;
}

/*=============================================================================
    函 数 名： SetDialBitrate
    功    能： 设置终端的呼叫速率
    算法实现： 
    全局变量： 
    参    数： u8  byMtId
               u16 wDialBitrate （主机序）
    返 回 值： void 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/03/17  3.6			万春雷                  创建
=============================================================================*/
void TConfMtTable::SetDialBitrate( u8 byMtId, u16 wDialBitrate )
{
    if( 0 == wDialBitrate ||
		byMtId > MAXNUM_CONF_MT || byMtId <= 0 )
	{
		return;
	}

	m_atMtExt[byMtId - 1].SetDialBitRate( wDialBitrate );
}

/*=============================================================================
    函 数 名： GetDialBitrate
    功    能： 获取终端的呼叫速率
    算法实现： 
    全局变量： 
    参    数： u8  byMtId
    返 回 值： u16 wDialBitrate （主机序）
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/03/17  3.6			万春雷                  创建
=============================================================================*/
u16 TConfMtTable::GetDialBitrate( u8 byMtId )
{
    if( byMtId > MAXNUM_CONF_MT || byMtId <= 0 )
	{
		return 0;
	}

	return m_atMtExt[byMtId - 1].GetDialBitRate();
}


/*=============================================================================
函 数 名： SetSndBandWidth
功    能： 终端上行带宽
算法实现： 
全局变量： 
参    数： u16 wBandWidth
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/3/21  4.0			许世林                  创建
=============================================================================*/
void TConfMtTable::SetSndBandWidth(u8 byMtId, u16 wBandWidth)
{
    if( byMtId > MAXNUM_CONF_MT || byMtId <= 0 )
    {
        return;
    }

    m_atMtData[byMtId-1].m_wSndBandWidth = wBandWidth;

    return;
}

/*=============================================================================
函 数 名： GetSndBandWidth
功    能： 终端上行带宽
算法实现： 
全局变量： 
参    数： void
返 回 值： u16  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/3/21  4.0			许世林                  创建
=============================================================================*/
u16  TConfMtTable::GetSndBandWidth(u8 byMtId)
{
    if( byMtId > MAXNUM_CONF_MT || byMtId <= 0 )
    {
        return 0;
    }
    
    return m_atMtData[byMtId-1].m_wSndBandWidth;
}


/*=============================================================================
函 数 名： SetRcvBandWidth
功    能： 设置终端接收带宽（视频）
算法实现： 
全局变量： 
参    数： u8 byMtId
           u16 wBandWidth
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/3/25  4.0			许世林                  创建
=============================================================================*/
void TConfMtTable::SetRcvBandWidth(u8 byMtId, u16 wBandWidth)
{
    if (byMtId > MAXNUM_CONF_MT || byMtId <= 0)
    {
        return;
    }

    m_atMtData[byMtId-1].m_wRcvBandWidth = wBandWidth;

    return;
}


/*=============================================================================
函 数 名： GetRcvBandWidth
功    能： 获取终端接收带宽（视频）
算法实现： 
全局变量： 
参    数： u8 byMtId
返 回 值： u16 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/3/25  4.0			许世林                  创建
=============================================================================*/
u16 TConfMtTable::GetRcvBandWidth(u8 byMtId)
{
    if (byMtId > MAXNUM_CONF_MT || byMtId <= 0)
    {
        return 0;
    }

    return m_atMtData[byMtId-1].m_wRcvBandWidth;
}

/*====================================================================
    函数名      ：DelMt
    功能        ：从终端列表中删除终端
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端编号
    返回值说明  ：TRUE - 删除成功 FALSE -删除失败
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
BOOL32 TConfMtTable::DelMt( u8 byMtId )
{
	if( byMtId == 0 || byMtId > m_byMaxNumInUse 
		|| m_atMtExt[byMtId - 1].GetMtId() != byMtId )
	{
		return FALSE;
	}

    memset( &m_atMtExt[byMtId - 1], 0, sizeof(TMtExt) );
	memset( &m_atMtData[byMtId - 1], 0, sizeof(TMtData) ); 
	memset( &m_atMtExt2[byMtId - 1], 0, sizeof(TMtExt2) );
	return TRUE;
}

/*====================================================================
    函数名      ：GetMtIdByIp
    功能        ：根据IP地址查找终端Id
    算法实现    ：
    引用全局变量：
    输入参数说明：u32 dwIpAddr, 终端IP地址
    返回值说明  ：0 - 不存在IP为输入值的终端
	              大于0 - 终端Id
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
u8 TConfMtTable::GetMtIdByIp( u32 dwIpAddr )
{
	if( dwIpAddr == 0 )return 0;

    u8 byMtId = 1;

	//查找
	while( byMtId <= m_byMaxNumInUse )
	{
		if( m_atMtExt[byMtId - 1].GetMtId() != byMtId )
		{
			byMtId++;
		}
		else
		{
            if( m_atMtExt[byMtId - 1].GetIPAddr() == dwIpAddr)
			{
				return byMtId;
			}
			else
			{
				byMtId++;
			}
		}
	}

	return 0;	
}

/*====================================================================
    函数名      ：GetMtIdByAlias
    功能        ：根据别名查找终端Id
    算法实现    ：
    引用全局变量：
    输入参数说明：TMtAlias tMtAlias, 终端别名
    返回值说明  ：0 - 不存在IP为输入值的终端
	              大于0 - 终端Id
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
u8 TConfMtTable::GetMtIdByAlias( const TMtAlias * ptAlias )
{
    if( ptAlias == NULL || ptAlias->m_AliasType == 0 ||
		(ptAlias->m_AliasType > mtAliasTypeOthers && 
		 ptAlias->m_AliasType != puAliasTypeIPPlusAlias))
	{
		return 0;
	}

    u8 byMtId = 1;

	//查找
	while( byMtId <= m_byMaxNumInUse )
	{
		if( m_atMtExt[byMtId - 1].GetMtId() == byMtId )
		{
            // zbq [08/08/2007] 介于终端呼入端口的不确定性，Ip别名不再比较端口
            if ( mtAliasTypeTransportAddress == ptAlias->m_AliasType )
            {
                if ( ptAlias->m_tTransportAddr.GetIpAddr() == 
                     m_atMtData[byMtId-1].m_atMtAlias[ptAlias->m_AliasType-1].GetMtAlias().m_tTransportAddr.GetIpAddr() )
                {
                    return byMtId;
                }
            }
			else if ( puAliasTypeIPPlusAlias == ptAlias->m_AliasType)
			{
				if ( ptAlias->m_tTransportAddr.GetIpAddr() == 
                     m_atMtData[byMtId-1].m_atMtAlias[mtAliasTypeH323ID -1].GetMtAlias().m_tTransportAddr.GetIpAddr() &&
					 strcmp(ptAlias->m_achAlias, m_atMtData[byMtId-1].m_atMtAlias[mtAliasTypeH323ID -1].GetMtAlias().m_achAlias) == 0)
				{
					return byMtId;
				}
			}
            else
            {
                if( *ptAlias == m_atMtData[byMtId-1].m_atMtAlias[ptAlias->m_AliasType-1].GetMtAlias())
                {
                    return byMtId;
                }                
            }
		}
        byMtId++;
	}

	return 0;	
}

/*====================================================================
    函数名      : SetMtRecording
    功能        ：设置终端录像状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
				  const TEqp& tRecEqp, 录像外设 
				  const u8& byRecChannel, 录像通道
    返回值说明  :无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/09/04    1.0				           创建
====================================================================*/
void TConfMtTable::SetMtRecording( u8 byMtId, const TEqp& tRecEqp, const u8& byRecChannel )
{
	if( byMtId != 0 && byMtId <= MAXNUM_CONF_MT )
	{
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState.SetRecording();
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_byRecChannel = byRecChannel;
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecEqp = tRecEqp;

		//同步终端的录像状态
		m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState;
	}

	return;
}

/*====================================================================
    函数名      : GetMtRecordInfo
    功能        ：获取终端的录相机信息
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 
				  TEqp tRecEqp, 录像外设 
				  u8 byRecChannel, 录像通道 
    返回值说明  :成功返回TRUE，否则返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/09/04    1.0         JQL           创建
====================================================================*/
BOOL32 TConfMtTable::GetMtRecordInfo( u8 byMtId, TEqp* ptRecEqp, u8* pbyRecChannel )
{
	if( byMtId != 0 && byMtId <= MAXNUM_CONF_MT && ptRecEqp != NULL && pbyRecChannel != NULL )
	{
		*pbyRecChannel = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_byRecChannel;
		*ptRecEqp = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecEqp;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*====================================================================
    函数名      : GetMtIdFromRecordInfo
    功能        ：获取此录像通道所服务的终端
    算法实现    ：
    引用全局变量：
    输入参数说明：TEqp tRecEqp, 录像外设 
				  u8 byRecChannel, 录像通道 
    返回值说明  :成功返回终端Id，失败返回0
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/09/04    1.0         JQL           创建
====================================================================*/
u8 TConfMtTable::GetMtIdFromRecordInfo( const TEqp& tRecEqp, const u8& byRecChannel )
{
	for( u8 byMtLoop = 1; byMtLoop <= MAXNUM_CONF_MT; byMtLoop++ )
	{
		if( m_atMtData[ byMtLoop -1 ].m_tMtRecInfo.m_byRecChannel == byRecChannel 
			&& m_atMtData[ byMtLoop -1 ].m_tMtRecInfo.m_tRecEqp == tRecEqp )
			return byMtLoop;
	}
	return 0;
}

/*====================================================================
    函数名      : SetMtNoRecording
    功能        ：设置终端未录像状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 
    返回值说明  :无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/09/04    1.0         JQL           创建
====================================================================*/
void TConfMtTable::SetMtNoRecording( u8 byMtId )
{
	if( byMtId != 0 && byMtId <= MAXNUM_CONF_MT )
	{	
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState.SetNoRecording();
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_byRecChannel = 0;
		memset( &m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecEqp , 0, sizeof( TEqp ) );

		//同步终端的录像状态
		m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState;

	}
}

/*====================================================================
    函数名      : SetMtRecPause
    功能        ：设置终端录像暂停状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 
    返回值说明  :无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/09/04    1.0         JQL           创建
====================================================================*/
void TConfMtTable::SetMtRecPause( u8 byMtId )
{
	if( byMtId != 0 && byMtId <= MAXNUM_CONF_MT )
	{
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState.SetRecPause();

		//同步终端的录像状态
		m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState;
	}
}

/*====================================================================
    函数名      : GetMtRecState
    功能        ：获取终端录像状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号  
				  TRecState * ptRecState, 录像机状态
    返回值说明  :无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/09/04    1.0         JQL           创建
====================================================================*/
BOOL32 TConfMtTable::GetMtRecState( u8 byMtId, TRecState * ptRecState )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptRecState == NULL )
		return FALSE;

	*ptRecState = m_atMtData[ byMtId -1 ].m_tMtRecInfo.m_tRecState;
	//同步终端的录像状态
	m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState;

	return TRUE;

}

/*====================================================================
    函数名      : IsMtRecording
    功能        ：查询终端录像是否正在录像
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 
    返回值说明  :TRUE/FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/09/04    1.0         JQL           创建
====================================================================*/
BOOL32 TConfMtTable::IsMtRecording( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
		return FALSE;

	return m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecState.IsRecording();

}

/*====================================================================
    函数名      : IsMtRecPause
    功能        ：查询终端录像是否暂停状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 
    返回值说明  :TRUE/FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/09/04    1.0         JQL           创建
====================================================================*/
BOOL32 TConfMtTable::IsMtRecPause( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
		return FALSE;

	return m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecState.IsRecPause();

}

/*====================================================================
    函数名      : IsMtNoRecording
    功能        ：查询终端录像是否暂停状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 
    返回值说明  :TRUE/FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/09/04    1.0         JQL           创建
====================================================================*/
BOOL32 TConfMtTable::IsMtNoRecording( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
		return FALSE;

	return m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecState.IsNoRecording();

}

/*====================================================================
    函数名      : IsMtRecSkipFrame
    功能        ：终端是否是抽帧录像状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 
    返回值说明  :TRUE/FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/09/04    1.0         JQL           创建
====================================================================*/
BOOL32 TConfMtTable::IsMtRecSkipFrame( u8 byMtId ) const
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
		return FALSE;

	return m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecState.IsRecSkipFrame();

}

/*====================================================================
    函数名      : SetMtRecSkipFrame
    功能        ：设置终端为抽帧录像状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 
				  BOOL32 bSkipFrame, 是否抽帧录像
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/09/04    1.0         JQL           创建
====================================================================*/
void TConfMtTable::SetMtRecSkipFrame( u8 byMtId, BOOL32 bSkipFrame )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecState.SetRecSkipFrame( bSkipFrame );
	//同步终端的录像状态
	m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState;

	return;
}

/*====================================================================
    函数名      : SetMtRecProg
    功能        ：设置终端录像进度
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 
				  const TRecProg & tRecProg, 录像进度
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/09/04    1.0         JQL           创建
====================================================================*/
void TConfMtTable::SetMtRecProg( u8 byMtId, const TRecProg & tRecProg )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	
	m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecProg = tRecProg;

	return;
}

/*====================================================================
    函数名      : GetMtRecProg
    功能        ：得到终端录像进度
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 
				  const TRecProg & tRecProg, 录像进度
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/09/04    1.0         JQL           创建
====================================================================*/
BOOL32 TConfMtTable::GetMtRecProg( u8 byMtId, TRecProg * ptRecProg )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptRecProg == NULL )
		return FALSE;

	*ptRecProg = m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecProg;
	return TRUE;	
}

/*====================================================================
    函数名      : IsMtVideoSending
    功能        ：终端是否正在发送视频
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 		
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/02/24    3.0         胡昌威           创建
====================================================================*/
BOOL32 TConfMtTable::IsMtVideoSending( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

	return m_atMtData[ byMtId - 1].m_tMtStatus.IsSendVideo();
}

/*====================================================================
    函数名      : IsMtVideoSending
    功能        ：终端是否正在发送音频
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 		
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/02/24    3.0         胡昌威           创建
====================================================================*/
BOOL32 TConfMtTable::IsMtAudioSending( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

	return m_atMtData[ byMtId - 1].m_tMtStatus.IsSendAudio();
}

/*====================================================================
    函数名      : IsMtVideo2Sending
    功能        ：终端是否正在发送第二视频
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 		
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/02/24    3.0         胡昌威           创建
====================================================================*/
BOOL32 TConfMtTable::IsMtVideo2Sending( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

	return m_atMtData[ byMtId - 1].m_tMtStatus.IsSndVideo2();
}

/*====================================================================
    函数名      : IsMtVideo2Recving
    功能        ：终端是否正在接收第二视频
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 		
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/02/24    3.0         胡昌威           创建
====================================================================*/
BOOL32 TConfMtTable::IsMtVideo2Recving( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

	return m_atMtData[ byMtId - 1].m_tMtStatus.IsRcvVideo2();
}

/*====================================================================
    函数名      : IsMtInMixing
    功能        ：终端是否正在混音
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 		
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/02/24    3.0         胡昌威           创建
====================================================================*/
BOOL32 TConfMtTable::IsMtInMixing( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

	return m_atMtData[ byMtId - 1].m_tMtStatus.IsInMixing();
}


/*=============================================================================
函 数 名： SetMtInMixGrp
功    能： 设置终端是否在混音通道内状态
算法实现： 
全局变量： 
参    数：  u8 byMtId
           BOOL32 bDiscuss
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/7/29  4.0			许世林                  创建
=============================================================================*/
void TConfMtTable::SetMtInMixGrp( u8 byMtId, BOOL32 bDiscuss )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	m_atMtData[byMtId - 1].m_tMtStatus.SetIsInMixGrp( bDiscuss );
}

/*=============================================================================
函 数 名： IsMtInMixGrp
功    能： 终端是否在混音通道内
算法实现： 
全局变量： 
参    数： u8 byMtId
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/7/29  4.0			许世林                  创建
=============================================================================*/
BOOL32 TConfMtTable::IsMtInMixGrp( u8 byMtId )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

	return m_atMtData[ byMtId - 1].m_tMtStatus.IsInMixGrp();
}

/*====================================================================
    函数名      : SetMtVideoSend
    功能        ：设置终端是否正在发送视频
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 
	              BOOL32 bSending 是否正在发送
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/02/24    3.0         胡昌威           创建
====================================================================*/
void TConfMtTable::SetMtVideoSend( u8 byMtId, BOOL32 bSending )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	m_atMtData[byMtId - 1].m_tMtStatus.SetSendVideo( bSending );

	return;
}

/*====================================================================
    函数名      : SetMtAudioSend
    功能        ：设置终端是否正在发送音频
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 
	              BOOL32 bSending 是否正在发送
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/02/24    3.0         胡昌威           创建
====================================================================*/
void TConfMtTable::SetMtAudioSend( u8 byMtId, BOOL32 bSending )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	//科达终端状态由终端上报，无需设置
	/*if( m_atMtExt[byMtId-1].GetManuId() == MT_MANU_KDC )
	{
		return;
	}*/

	m_atMtData[ byMtId - 1].m_tMtStatus.SetSendAudio( bSending );

	return;
}

/*====================================================================
    函数名      : SetMtVideoRecv
    功能        ：设置终端是否正在接收视频
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 
				  BOOL32 bRecving 是否正在接收
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/02/24    3.0         胡昌威           创建
====================================================================*/
void TConfMtTable::SetMtVideoRecv( u8 byMtId, BOOL32 bRecving )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	m_atMtData[ byMtId - 1].m_tMtStatus.SetReceiveVideo( bRecving );

	return;
}


/*====================================================================
    函数名      : SetMtAudioRecv
    功能        ：设置终端是否正在接收音频
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 
				  BOOL32 bRecving 是否正在接收
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/02/24    3.0         胡昌威           创建
====================================================================*/
void TConfMtTable::SetMtAudioRecv( u8 byMtId, BOOL32 bRecving )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	m_atMtData[ byMtId - 1].m_tMtStatus.SetReceiveAudio( bRecving );

	return;
}

/*====================================================================
    函数名      : IsMtVideoSending
    功能        ：终端是否正在发送音频
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 		
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/17    3.6         Jason        创建
====================================================================*/
BOOL32 TConfMtTable::IsMtAudioRecv( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

	return m_atMtData[ byMtId - 1].m_tMtStatus.IsReceiveAudio();
}

/*====================================================================
    函数名      : IsMtAudioMute
    功能        ：终端是否静音
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 		
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/17    3.6         Jason        创建
====================================================================*/
BOOL32 TConfMtTable::IsMtAudioMute( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

	return m_atMtData[ byMtId - 1].m_tMtStatus.IsDecoderMute();
}


/*====================================================================
    函数名      : IsMtAudioDumb
    功能        ：终端是否哑音
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 		
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/17    3.6         Jason        创建
====================================================================*/
BOOL32 TConfMtTable::IsMtAudioDumb( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

	return m_atMtData[ byMtId - 1].m_tMtStatus.IsCaptureMute();
}



/*====================================================================
    函数名      : SetMtVideo2Send
    功能        ：设置终端是否正在发送第二视频
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 
	              BOOL32 bSending 是否正在发送
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/02/24    3.0         胡昌威           创建
====================================================================*/
void TConfMtTable::SetMtVideo2Send( u8 byMtId, BOOL32 bSending )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	m_atMtData[byMtId - 1].m_tMtStatus.SetSndVideo2( bSending );
}

/*====================================================================
    函数名      : SetMtVideo2Recv
    功能        ：设置终端是否正在接收第二视频
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 
	              BOOL32 bSending 是否正在发送
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/02/24    3.0         胡昌威           创建
====================================================================*/
void TConfMtTable::SetMtVideo2Recv( u8 byMtId, BOOL32 bSending )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	m_atMtData[byMtId - 1].m_tMtStatus.SetRcvVideo2( bSending );
}
/*====================================================================
    函数名      : SetMtInMixing
    功能        ：设置终端是否正在混音
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 
				  BOOL32 bMtInMixing 是否正在混音
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/02/24    3.0         胡昌威           创建
====================================================================*/
void TConfMtTable::SetMtInMixing( u8 byMtId, BOOL32 bMtInMixing )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	//科达终端状态由终端上报，无需设置
	/*if( m_atMtExt[byMtId-1].GetManuId() == MT_MANU_KDC )
	{
		return;
	}*/

	m_atMtData[ byMtId - 1].m_tMtStatus.SetInMixing( bMtInMixing );

	return;
}

/*====================================================================
    函数名      : SetMtInTvWall
    功能        ：设置终端是否正在电视墙中
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 
				  BOOL32 bMtInTvWall 是否正在电视墙中
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/08/12    4.0         libo            创建
====================================================================*/
void TConfMtTable::SetMtInTvWall(u8 byMtId, BOOL32 bMtInTvWall)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return;
    }

    m_atMtData[byMtId - 1].m_tMtStatus.SetInTvWall(bMtInTvWall);
}

/*====================================================================
    函数名      : IsMtInTvWall
    功能        ：终端是否正在电视墙中
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/08/12    4.0         libo            创建
====================================================================*/
BOOL32 TConfMtTable::IsMtInTvWall(u8 byMtId)
{
    if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return FALSE;
    }

    return m_atMtData[byMtId - 1].m_tMtStatus.IsInTvWall();
}

/*====================================================================
    函数名      : SetMtInHdu
    功能        ：终端是否正在电视墙中
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/08/12    4.0         libo            创建
====================================================================*/
void TConfMtTable::SetMtInHdu(u8 byMtId, BOOL32 bMtInHdu /*= TRUE*/)
{
	if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return;
    }
	
    m_atMtData[byMtId - 1].m_tMtStatus.SetInHdu(bMtInHdu);

	return;
}

/*====================================================================
    函数名      : IsMtInHdu
    功能        ：终端是否正在电视墙中
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/08/12    4.0         libo            创建
====================================================================*/
BOOL32 TConfMtTable::IsMtInHdu(u8 byMtId)
{
	if (byMtId == 0 || byMtId > MAXNUM_CONF_MT)
    {
        return FALSE;
    }
	
    return m_atMtData[byMtId - 1].m_tMtStatus.IsInHdu();
}

/*====================================================================
    函数名      : SetMtIsMaster
    功能        ：设置终端是否主终端
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 
				  BOOL32 bMaster 是否主终端
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/14    3.6        Jason           创建
====================================================================*/
void TConfMtTable::SetMtIsMaster( u8 byMtId, BOOL32 bMaster )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	m_atMtData[ byMtId - 1].m_bMaster = bMaster;

	return;
}

/*====================================================================
    函数名      : IsMtIsMaster
    功能        ：终端是否是主终端
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号 		
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/14    3.6         Jason           创建
====================================================================*/
BOOL32 TConfMtTable::IsMtIsMaster( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}

	return m_atMtData[ byMtId - 1].m_bMaster;
}


/*====================================================================
    函数名      : SetCurrVidSrcNo
    功能        ：设置当前视频源号
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
                  u8 byCurrVidNo, 当前视频源号
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/04/05    3.6         libo           创建
====================================================================*/
void TConfMtTable::SetCurrVidSrcNo(u8 byMtId, u8 byCurrVidNo)
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	m_atMtData[byMtId - 1].m_byCurrVidNo = byCurrVidNo;
}

/*====================================================================
    函数名      : GetCurrVidSrcNo
    功能        ：获取当前视频源号
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  : 该终端的当前视频源号
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/04/05    3.6         libo           创建
====================================================================*/
u8 TConfMtTable::GetCurrVidSrcNo(u8 byMtId)
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}

	return m_atMtData[byMtId - 1].m_byCurrVidNo;
}

/*====================================================================
    函数名      : SetHWVerID
    功能        ：设置终端硬件版本号
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  : 
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/12/18    4.5         张宝卿           创建
====================================================================*/
void TConfMtTable::SetHWVerID(u8 byMtId, u8 byHWVer)
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return;
    }
    m_atMtExt2[byMtId-1].SetHWVerId(byHWVer);
    return;
}

/*====================================================================
    函数名      : GetHWVer
    功能        ：获取终端硬件版本号
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  : u8
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/12/18    4.5         张宝卿           创建
====================================================================*/
u8 TConfMtTable::GetHWVerID(u8 byMtId) const
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return 0;
    }
    return m_atMtExt2[byMtId-1].GetHWVerId();
}

/*====================================================================
    函数名      : SetSWVerID
    功能        ：设置终端软件版本号
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
                  LPCSTR lpszSWVer
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/12/18    4.5         张宝卿           创建
====================================================================*/
void TConfMtTable::SetSWVerID(u8 byMtId, LPCSTR lpszSWVer)
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return;
    }
    m_atMtExt2[byMtId-1].SetSWVerId(lpszSWVer);
    return;
}

/*====================================================================
    函数名      : GetSWVerID
    功能        ：获取终端软件版本号
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  : LPCSTR
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/12/18    4.5         张宝卿           创建
====================================================================*/
LPCSTR TConfMtTable::GetSWVerID(u8 byMtId) const
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return NULL;
    }
    return m_atMtExt2[byMtId-1].GetSWVerId();
}

/*====================================================================
    函数名      ：AddSwitchTable
    功能        ：增加Mp交换信息表
    算法实现    ：
    引用全局变量：
    输入参数说明：byMpId Mp编号
	              ptSwitchChannel 交换信道
    返回值说明  ：成功TRUE,失败FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/07    3.0         胡昌威        创建
====================================================================*/
BOOL32 TConfSwitchTable::AddSwitchTable(u8 byMpId,TSwitchChannel *ptSwitchChannel)
{
	if( NULL == ptSwitchChannel )
	{
		return FALSE;
	}

	if (byMpId > MAXNUM_DRI || 0 == byMpId)
	{
		OspPrintf(TRUE, FALSE, "[AddSwitchTable] invalid byMpId<%d> received!!!\n", byMpId);
		return FALSE;
	}
    
#ifdef _MINIMCU_
    if (byMpId > 1)
    {
        byMpId = 2;
    }
#endif

    //若原来此交换信息移除失败，覆盖原有信息
    BOOL32 bFind = FALSE;
    u16 wTableId = 0;
    while( wTableId < MAX_SWITCH_CHANNEL )
    {
        if(m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableId] == *ptSwitchChannel)
        {
            bFind = TRUE;
            break;
        }        
        wTableId++;
    }
	
    if(!bFind)
    {
        //添加新表项
        wTableId = 0;
        while( wTableId < MAX_SWITCH_CHANNEL )
        {
            if( m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableId].IsNull() )
                break;
            wTableId++;
        }
    }	

    if( wTableId < MAX_SWITCH_CHANNEL )
	{
		m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableId] = *ptSwitchChannel;
        return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*====================================================================
    函数名      ：RemoveSwitchTable
    功能        ：移除Mp交换信息表
    算法实现    ：
    引用全局变量：
    输入参数说明：byMpId Mp编号
	              ptSwitchChannel 交换信道
    返回值说明  ：成功TRUE,失败FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/07    3.0         胡昌威        创建
====================================================================*/
BOOL32 TConfSwitchTable::RemoveSwitchTable(u8 byMpId,TSwitchChannel *ptSwitchChannel)
{
	if( NULL == ptSwitchChannel )
	{
		return FALSE;
	}

	if (byMpId > MAXNUM_DRI || 0 == byMpId)
	{
		OspPrintf(TRUE, FALSE, "[RemoveSwitchTable] invalid byMpId<%d> received!!!\n", byMpId);
		return FALSE;
	}

	//移除表项
	TSwitchChannel tSwitchChannel = *ptSwitchChannel;
	if( ptSwitchChannel->IsNull() )
	{
		return FALSE;
	}

#ifdef _MINIMCU_
    if (byMpId > 1)
    {
        byMpId = 2;
    }
#endif

	u16 wTableId = 0;
	while(wTableId<MAX_SWITCH_CHANNEL)
	{
		if( m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableId].GetDstIP() == tSwitchChannel.GetDstIP() && 
			m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableId].GetDstPort() == tSwitchChannel.GetDstPort() )
        {
            break;
        }
		else
		{
			wTableId++;
		}
	}
	
	if(wTableId<MAX_SWITCH_CHANNEL)
	{
		ptSwitchChannel->SetRcvPort( m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableId].GetRcvPort() );
		ptSwitchChannel->SetSrcMt( m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableId].GetSrcMt() );
		m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableId].SetNull();
        return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*=============================================================================
函 数 名： ClearSwitchTable
功    能： 
算法实现： 
全局变量： 
参    数： u8 byMpId
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/14  4.0			许世林                  创建
=============================================================================*/
void TConfSwitchTable::ClearSwitchTable(u8 byMpId)
{
    if (0 == byMpId || byMpId > MAXNUM_DRI)
    {
        return;
    }

#ifdef _MINIMCU_
    if (byMpId > 1)
    {
        byMpId = 2;
    }
#endif
    memset(&m_tSwitchTable[byMpId-1], 0, sizeof(TSwitchTable));
}

/*====================================================================
    函数名      ：ProcMultiToOneSwitch
    功能        ：处理多点到一点的交换
    算法实现    ：
    引用全局变量：
    输入参数说明：ptSwitchChannel 交换信道
				  u8 byAct 1-add 2-remove 3-stop
    返回值说明  ：成功TRUE,失败FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/11/07    3.0         胡昌威        创建
====================================================================*/
void TConfSwitchTable::ProcMultiToOneSwitch( TSwitchChannel *ptSwitchChannel, u8 byAct )
{
	u8 byIdleSlot = 7;
#ifdef _MINIMCU_
    byIdleSlot = 1;
#endif
	switch( byAct ) 
	{
	case 1://增加
		{
			u16 wTableId = 0;
			while( wTableId < MAX_SWITCH_CHANNEL )
			{
				if( m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].IsNull() )
				{
					break;
				}
				else
				{
					wTableId++;
				}
			}

			if( wTableId < MAX_SWITCH_CHANNEL )
			{
				m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId] = *ptSwitchChannel;
			}
		}
		break;

	case 2://移除
		{
			//移除表项
			TSwitchChannel tSwitchChannel = *ptSwitchChannel;
			u16 wTableId = 0;
			while(wTableId<MAX_SWITCH_CHANNEL)
			{
				if( m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].GetRcvIP() == tSwitchChannel.GetRcvIP() && 
					m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].GetRcvPort() == tSwitchChannel.GetRcvPort() && 
					m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].GetDstIP() == tSwitchChannel.GetDstIP() && 
					m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].GetDstPort() == tSwitchChannel.GetDstPort() )
				{
					m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].SetNull();
				}

				wTableId++;
			}
		}
		break;

	case 3://停止
		{
			//移除表项
			TSwitchChannel tSwitchChannel = *ptSwitchChannel;
			u16 wTableId = 0;
			while(wTableId<MAX_SWITCH_CHANNEL)
			{
				if( m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].GetDstIP() == tSwitchChannel.GetDstIP() && 
					m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].GetDstPort() == tSwitchChannel.GetDstPort() )
				{
					m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].SetNull();
				}
				
				wTableId++;		
			}
		}
		break;
		
	default:
		break;
	}

	return;
}

/*====================================================================
    函数名      ：ProcBrdSwitch
    功能        ：处理广播交换
    算法实现    ：
    引用全局变量：
    输入参数说明：byNum 交换通道数量
                  ptSwitchChannel 交换信道指针
				  u8 byAct 1-add brdsrc  2-remove brdsrc
                           11-add brddst
                  u8 byMpId 目标MpId
    返回值说明  ：成功TRUE,失败FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/03/27    4.0         顾振华        创建
====================================================================*/
void TConfSwitchTable::ProcBrdSwitch( u8 byNum, TSwitchChannel *ptSwitchChannel, u8 byAct, u8 byMpId )
{
    // guzh [3/29/2007] 寻找一个空闲的交换表保存广播源，广播目标按照标准交换保存
	u8 byIdleIdx = 0;
#ifndef _MINIMCU_
	byIdleIdx = ( g_cMcuAgent.GetMpcBoardId() == MCU_BOARD_MPC ? MCU_BOARD_MPCD : MCU_BOARD_MPC );  
    byIdleIdx --;
#else
	byIdleIdx = 0;
#endif
    

    TSwitchChannel tSwitchChannel;

    if (0 == byNum || ptSwitchChannel == NULL)
        return;
    if (0 == byMpId)
        return;
    
    switch( byAct ) 
    {
    case 1://增加广播源
        {
            tSwitchChannel = *ptSwitchChannel;

            // 由于增加广播源消息是广播，这里只处理该MT接入的MP的回应
            u32 dwMpIp;
            u32 dwSrcIp;
            u16 wMpRecvPort;
            if ( ! g_cMpManager.GetSwitchInfo(tSwitchChannel.GetSrcMt(), dwMpIp, wMpRecvPort, dwSrcIp) )
            {
                return;
            }

            if ( byMpId != g_cMcuVcApp.FindMp( dwMpIp ) )
            {
                break;
            }

            u16 wTableId = 0;
            while( wTableId < MAX_SWITCH_CHANNEL )
            {
                if( m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wTableId].IsNull() )
                {
                    break;
                }
                else
                {
                    wTableId++;
                }
            }
            
            if( wTableId < MAX_SWITCH_CHANNEL )
            {
                // 标记为广播源                
                tSwitchChannel.SetDstIP(0xffffffff);
                tSwitchChannel.SetDstPort(0x0);
                m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wTableId] = tSwitchChannel;
			}
            else
            {
                OspPrintf(TRUE, FALSE, "[ProcBrdSwitch] Add video broadcast source to switch table failed!\n");
            }
        }
        break;
        
    case 2://移除广播源
        {
            tSwitchChannel = *ptSwitchChannel;
            // 由于删除广播源消息是广播，这里只处理该MT接入的MP的回应
            u32 dwMpIp;
            u32 dwSrcIp;
            u16 wMpRecvPort;
            if ( ! g_cMpManager.GetSwitchInfo(tSwitchChannel.GetSrcMt(), dwMpIp, wMpRecvPort, dwSrcIp) )
            {
                return;
            }
            
            if ( byMpId != g_cMcuVcApp.FindMp( dwMpIp ) )
            {
                break;
            }
            
            u16 wTableId = 0;
            BOOL32 bFound = FALSE;
            while(wTableId<MAX_SWITCH_CHANNEL)
            {
                if( m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wTableId].GetSrcIp() == tSwitchChannel.GetSrcIp() && 
                    m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wTableId].GetRcvPort() == tSwitchChannel.GetRcvPort() && 
                    m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wTableId].GetDstIP() == 0xffffffff && 
                    m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wTableId].GetDstPort() == 0x0 )
                {
                    m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wTableId].SetNull();
                    bFound = TRUE;
                    break;
                }
                
                wTableId++;
            }
            if ( !bFound )
            {
                // OspPrintf(TRUE, FALSE, "[ProcBrdSwitch] Remove video broadcast source to switch table failed!\n");
            }
        }
        break;
        
    case 11://增加广播目标
        {
#ifdef _MINIMCU_
			if (byMpId > 1)
			{
				byMpId = 2;
			}
#endif
			byIdleIdx =	byMpId - 1;

            u16 wTableId = 0;
            u8 bySwitchLoop = 0;
            u16 wInsertedPos;

            while ( bySwitchLoop < byNum )
            {
                tSwitchChannel = ptSwitchChannel[bySwitchLoop];

                //首先查找
                wTableId = 0;
                wInsertedPos = MAX_SWITCH_CHANNEL;
                while ( wTableId<MAX_SWITCH_CHANNEL )
                {
                    if ( m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wTableId].GetDstIP() == tSwitchChannel.GetDstIP() &&
                         m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wTableId].GetDstPort() == tSwitchChannel.GetDstPort() )
                    {
                        // 找到原来的表项，直接替换
                        wInsertedPos = wTableId;
                        break;
                    }
                    else if ( wInsertedPos == MAX_SWITCH_CHANNEL &&
                              m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wTableId].IsNull() )
                    {
                        // 记录下空闲的插入点
                        wInsertedPos = wTableId;
                    }
                    wTableId ++;
                }

                if ( wInsertedPos != MAX_SWITCH_CHANNEL )
                {
                    tSwitchChannel.SetSrcIp(0xffffffff);
                    tSwitchChannel.SetRcvPort(0x0);
                    
                    m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wInsertedPos] = tSwitchChannel;                    
                }
                else
                {
                    OspPrintf(TRUE, FALSE, "[ProcBrdSwitch] m_tSwitchTable Mp.%d full, insert failed!\n", byMpId);
                    break;
                }
                bySwitchLoop ++;
            }
        }
        break;
        
    default:
        break;
    }
    
	return;
}

/*====================================================================
    函数名      ：IsValidSwitchSrcIp
    功能        ：是否是有效的交换源Ip
    算法实现    ：
    引用全局变量：
    输入参数说明：u32 dwSrcIp 源Ip地址
                  u16 wRcvPort 转发端口
                  u8 byMode 音视频模式
    返回值说明  ：TRUE-是,FALSE-否
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	06/09/27    4.0         许世林        创建
====================================================================*/
BOOL32 TConfSwitchTable::IsValidSwitchSrcIp( u32 dwSrcIp, u16 wRcvPort, u8 byMode )
{	
    u8 byMpId;
    for (u8 byLoop = 1; byLoop <= MAXNUM_DRI; byLoop++)
    {
        byMpId = byLoop;
        
        // guzh [3/29/2007] 8/16内部保存了特殊的交换条目
        if ( !g_cMcuVcApp.IsMpConnected(byMpId) &&
             byMpId != MCU_BOARD_MPC &&
             byMpId != MCU_BOARD_MPCD )
            continue;

#ifdef _MINIMCU_
        if (byLoop > 1)
        {
            byMpId = 2;
        }
#endif
        for(u16 wTableIdx = 0; wTableIdx < MAX_SWITCH_CHANNEL; wTableIdx++)
        {
            TSwitchChannel *ptSwitchChnnl = &m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableIdx];
            if (ptSwitchChnnl->IsNull())
                continue;
            
            if( ptSwitchChnnl->GetSrcIp() == dwSrcIp && ptSwitchChnnl->GetRcvPort() == wRcvPort )
            {
                if( (byMode == MODE_VIDEO && ptSwitchChnnl->GetRcvPort() % PORTSPAN == 0 ) ||
                    (byMode == MODE_AUDIO && ptSwitchChnnl->GetRcvPort() % PORTSPAN == 2) )
                {
                    if (ptSwitchChnnl->GetRcvPort() != ptSwitchChnnl->GetDstPort())
                    {
                        return TRUE;
                    }
                    else if (g_cMcuVcApp.FindMp(ptSwitchChnnl->GetDstIP()) == 0)
                    {
                        return TRUE;
                    }
                }
            }
            else if (ptSwitchChnnl->GetDstIP() == 0xffffffff && 
                     ptSwitchChnnl->GetDstPort() == 0 &&
                     byMode == MODE_VIDEO &&
                     //zbq[04/02/2008] 是否当前源建立下的广播
                     ptSwitchChnnl->GetSrcIp() == dwSrcIp )
            {
                return TRUE;
            }
        }
    }

    return FALSE;	
}

/*=============================================================================
函 数 名： IsValidCommonSwitchSrcIp
功    能： 
算法实现： 
全局变量： 
参    数：  u32 dwSrcIp
           u16 wRcvPort
           u8 byMode
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/5/23   4.0		    周广程                  创建
=============================================================================*/
BOOL32 TConfSwitchTable::IsValidCommonSwitchSrcIp( u32 dwSrcIp, u16 wRcvPort, u8 byMode )
{
    u8 byMpId;
    for (u8 byLoop = 1; byLoop <= MAXNUM_DRI; byLoop++)
    {
        byMpId = byLoop;
        
        // guzh [3/29/2007] 8/16内部保存了特殊的交换条目
        if ( !g_cMcuVcApp.IsMpConnected(byMpId) &&
            byMpId != MCU_BOARD_MPC &&
            byMpId != MCU_BOARD_MPCD )
        {
            continue;
        }
        
#ifdef _MINIMCU_
        if (byLoop > 1)
        {
            byMpId = 2;
        }
#endif
        for(u16 wTableIdx = 0; wTableIdx < MAX_SWITCH_CHANNEL; wTableIdx++)
        {
            TSwitchChannel *ptSwitchChnnl = &m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableIdx];
            if (ptSwitchChnnl->IsNull())
            {
                continue;
            }
            
            if ( ptSwitchChnnl->GetSrcIp() != dwSrcIp || ptSwitchChnnl->GetRcvPort() != wRcvPort )
            {
                continue;
            }

            if( (byMode == MODE_VIDEO && ptSwitchChnnl->GetRcvPort() % PORTSPAN == 0 ) ||
                (byMode == MODE_AUDIO && ptSwitchChnnl->GetRcvPort() % PORTSPAN == 2) )
            {
                if (ptSwitchChnnl->GetRcvPort() != ptSwitchChnnl->GetDstPort() && 
                    // 这里是防止把广播源到MP的交换误认为是普通交换, zgc, 2008-08-27
                    ptSwitchChnnl->GetDstPort() != 0 )
                {
                    return TRUE;
                }
                else if (g_cMcuVcApp.FindMp(ptSwitchChnnl->GetDstIP()) == 0 &&
                         // 这里是防止把广播源到MP的交换误认为是普通交换, zgc, 2008-08-27
                         ptSwitchChnnl->GetDstIP() != 0xffffffff )
                {
                    return TRUE;
                }
            }
        }
    }
    
    return FALSE;	
}

/*=============================================================================
函 数 名： IsValidBroaddSwitchSrcIp
功    能： 
算法实现： 
全局变量： 
参    数：  u32 dwSrcIp
           u16 wRcvPort
           u8 byMode
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/5/23   4.0		    周广程                  创建
=============================================================================*/
BOOL32 TConfSwitchTable::IsValidBroadSwitchSrcIp( u32 dwSrcIp, u16 wRcvPort, u8 byMode )
{
    u8 byMpId;
    for (u8 byLoop = 1; byLoop <= MAXNUM_DRI; byLoop++)
    {
        byMpId = byLoop;
        
        // guzh [3/29/2007] 8/16内部保存了特殊的交换条目
        if ( !g_cMcuVcApp.IsMpConnected(byMpId) &&
            byMpId != MCU_BOARD_MPC &&
            byMpId != MCU_BOARD_MPCD )
            continue;
        
#ifdef _MINIMCU_
        if (byLoop > 1)
        {
            byMpId = 2;
        }
#endif
        for(u16 wTableIdx = 0; wTableIdx < MAX_SWITCH_CHANNEL; wTableIdx++)
        {
            TSwitchChannel *ptSwitchChnnl = &m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableIdx];
            if (ptSwitchChnnl->IsNull())
            {
                continue;
            }
            
            if (ptSwitchChnnl->GetDstIP() == 0xffffffff && 
                ptSwitchChnnl->GetDstPort() == 0 &&
                byMode == MODE_VIDEO &&
                ptSwitchChnnl->GetSrcIp() == dwSrcIp )
            {
                return TRUE;
            }
        }
    }
    
    return FALSE;	
}

/*====================================================================
    函数名      : McuGetDebugKeyValue
    功能        : 获取MCU Debug文件key value
    算法实现    :（可选项）
    引用全局变量: 无
    输入参数说明: TMcuDebugVal &tMcuDebugVal: MCU Debug 信息结构
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/13    3.0         胡昌威       创建
====================================================================*/
void McuGetDebugKeyValue(TMcuDebugVal &tMcuDebugVal)
{
	s32 nKeyVal;
	s8  achFullName[64];

	sprintf(achFullName, "%s/%s", DIR_CONFIG, MCUDEBUGFILENAME);
	GetRegKeyInt( achFullName, "mcuNetWork", "mcuListenPort", MCU_LISTEN_PORT, &nKeyVal );
	tMcuDebugVal.m_wMcuListenPort = (u16)nKeyVal;
	GetRegKeyInt( achFullName, "mcuNetWork", "mcuTelnetPort", MCU_TELNET_PORT, &nKeyVal );
	tMcuDebugVal.m_wMcuTelnetPort = (u16)nKeyVal;
    
	GetRegKeyInt( achFullName, "refreshInterval", "mcsRefreshInterval", 3, &nKeyVal );
	tMcuDebugVal.m_dwMcsRefreshInterval = (u32)nKeyVal;

	GetRegKeyInt( achFullName, "mcuWatchDog", "mcuWatchDogEnable", 1, &nKeyVal );
	tMcuDebugVal.m_bWDEnable = (1 == nKeyVal) ? TRUE : FALSE;

#ifdef WIN32
	tMcuDebugVal.m_bMSDetermineType = FALSE;
#else
	GetRegKeyInt( achFullName, "mcuMSExchangeCap", "mcuMSDetermineType", 1, &nKeyVal );
	tMcuDebugVal.m_bMSDetermineType = (1 == nKeyVal) ? TRUE : FALSE;
#endif
	
	// 稳态时主备同步时间, 允许范围在5分钟到60分钟
	GetRegKeyInt( achFullName, "mcuMSExchangeCap", "mcuMSSynTime", MS_SYN_MAX_SPAN_TIMEOUT, &nKeyVal);
	if( nKeyVal < MS_SYN_MAX_SPAN_TIMEOUT || nKeyVal > MS_SYN_MAX_SPAN_TIMEOUT*12 )
	{
		nKeyVal = MS_SYN_MAX_SPAN_TIMEOUT;
	}
	tMcuDebugVal.m_wMsSynTime = (u16)nKeyVal;
    
    GetRegKeyInt(achFullName, "BitrateAdapterParam", "BitrateScale", 0, &nKeyVal);    
    tMcuDebugVal.m_byBitrateScale = nKeyVal;
    
    GetRegKeyInt(achFullName, "cascadeParam", "IsMMcuSpeaker", 0, &nKeyVal);
    tMcuDebugVal.m_byIsMMcuSpeaker = (0 != nKeyVal) ? 1 : 0;

    GetRegKeyInt(achFullName, "cascadeParam", "IsAutoDetectMMcuDupCallIn", 0, &nKeyVal);
    tMcuDebugVal.m_byIsAutoDetectMMcuDupCall = (0 != nKeyVal) ? 1 : 0;

    GetRegKeyInt(achFullName, "cascadeParam", "CascadeAlias", 0, &nKeyVal);
    tMcuDebugVal.m_byCascadeAliasType = (u8)nKeyVal;

    GetRegKeyInt(achFullName, "cascadeParam", "IsShowMMcuMtList", 1, &nKeyVal);
    tMcuDebugVal.m_byShowMMcuMtList = (u8)nKeyVal;
    
    // 默认连接级联SMcu下级端口
    tMcuDebugVal.m_wSMcuCasPort = 3337;

	GetRegKeyInt(achFullName, "mcuGeneralParam", "IsApplyChairToZxMcu", 0, &nKeyVal);
	tMcuDebugVal.m_byIsApplyChairToZxMcu = (0 != nKeyVal) ? 1 : 0;

    // 是否转发终端短消息
    GetRegKeyInt(achFullName, "mcuGeneralParam", "IsTransmitMtShortMsg", 1, &nKeyVal);
	tMcuDebugVal.m_byIsTransmitMtShortMsg = (0 != nKeyVal) ? 1 : 0;

    //是否将终端申请主席或发言人的提示信息告诉主席终端
    GetRegKeyInt(achFullName, "mcuGeneralParam", "IsChairDisplayMtApplyInfo", 1, &nKeyVal);
	tMcuDebugVal.m_byIsChairDisplayMtApplyInfo = (0 != nKeyVal) ? 1 : 0;
    
    //是否允许双格式会议选看
    GetRegKeyInt(achFullName, "mcuGeneralParam", "IsSelInDoubleMediaConf", 0, &nKeyVal);
    tMcuDebugVal.m_byIsSelInDoubleMediaConf = (0 != nKeyVal) ? 1 : 0;

    // guzh [4/30/2007] 以下部分在Debug文件中不可见，除非特殊要求
#ifndef WIN32    
    // 是否根据终端型号（产品类型）限制接入能力
    GetRegKeyInt(achFullName, "mcuGeneralParam", "IsNotLimitAccessByMtModal", 0, &nKeyVal);
    tMcuDebugVal.m_byIsNotLimitAccessByMtModal = (0 != nKeyVal) ? 1 : 0;
#else
    tMcuDebugVal.m_byIsNotLimitAccessByMtModal = 1;
#endif
    
    //是否支持第二双流能力（第一双流为H239/264）
    GetRegKeyInt(achFullName, "mcuGeneralParam", "IsSupportSecDSCap", 0, &nKeyVal);
    tMcuDebugVal.m_byIsSupportSecDSCap = (u8)nKeyVal;

    //是否允许同一终端进HD-VMP多个通道
    GetRegKeyInt(achFullName, "mcuGeneralParam", "IsAllowVmpMemRepeated", 0, &nKeyVal);
    tMcuDebugVal.m_byIsAllowVmpMemRepeated = (u8)nKeyVal;

    //是否紧凑使用当前的适配资源
    GetRegKeyInt(achFullName, "mcuGeneralParam", "IsAdpResourceCompact", 0, &nKeyVal);
    tMcuDebugVal.m_byIsAdpResourceCompact = (u8)nKeyVal;
    
    //MPU-SVMP是否强制编码1080i
    GetRegKeyInt(achFullName, "mcuGeneralParam", "IsSVmpOutput1080i", 0, &nKeyVal);
    tMcuDebugVal.m_byIsSVmpOutput1080i = (u8)nKeyVal;

    //是否支持收工适配会议
    GetRegKeyInt(achFullName, "mcuGeneralParam", "IsConfAdpManually", 1, &nKeyVal);
    tMcuDebugVal.m_byConfAdpManually = (u8)nKeyVal;
	

	//MCU是否运行MP
#ifndef WIN32
	GetRegKeyInt(achFullName, "mcuAbilityParam", "IsMpcRunMp", 0, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byIsMpcRunMp = (u8)nKeyVal;
#else
    // guzh [4/2/2007] Windows 缺省运行MP
    GetRegKeyInt(achFullName, "mcuAbilityParam", "IsMpcRunMp", 1, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byIsMpcRunMp = (u8)nKeyVal;
#endif
	
	//MCU是否运行MtAdp, 8000B/C 才通过debug配置
#ifndef WIN32
	GetRegKeyInt(achFullName, "mcuAbilityParam", "IsMpcRunMtAdp", 0, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byIsMpcRunMtAdp = (u8)nKeyVal;
#else//Windows 缺省运行MtAdp
	GetRegKeyInt(achFullName, "mcuAbilityParam", "IsMpcRunMtAdp", 1, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byIsMpcRunMtAdp = (u8)nKeyVal;
#endif

    //是否限制码流转发板的转发能力
    GetRegKeyInt(achFullName, "mcuAbilityParam", "IsRestrictFlux", 1, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byIsRestrictFlux = (u8)nKeyVal;    

	//MCU上的MP最大转发能力
	GetRegKeyInt(achFullName, "mcuAbilityParam", "mpcMaxMpAbility", MAXLIMIT_MPC_MP, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_wMpcMaxMpAbility = (u16)nKeyVal;
	
	//CRI板上的MP最大转发能力
	GetRegKeyInt(achFullName, "mcuAbilityParam", "criMaxMpAbility", MAXLIMIT_CRI_MP, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_wCriMaxMpAbility = (u16)nKeyVal;

	//是否限制MCU终端适配板的终端接入能力
	GetRegKeyInt(achFullName, "mcuAbilityParam", "IsRestrictMtNum", 1, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byIsRestrictMtNum = (u8)nKeyVal;

	//MCU上终端适配最大终端接入能力
	GetRegKeyInt(achFullName, "mcuAbilityParam", "mpcMaxMtAdpConnMtAbility", MAXLIMIT_MPC_MTADP, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byMpcMaxMtAdpConnMtAbility = (u8)nKeyVal;

	//MCU上终端适配最大下级MCU接入能力
	GetRegKeyInt(achFullName, "mcuAbilityParam", "mpcMaxMtAdpConnSMcuAbility", MAXLIMIT_MPC_MTADP_SMCU, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byMpcMaxMtAdpConnSMcuAbility = (u8)nKeyVal;


	//TUI板上终端适配最大终端接入能力
	GetRegKeyInt(achFullName, "mcuAbilityParam", "tuiMaxMtAdpConnMtAbility", MAXLIMIT_CRI_MTADP, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byTuiMaxMtAdpConnMtAbility = (u8)nKeyVal;

	//TUI板上终端适配最大下级MCU接入能力
	GetRegKeyInt(achFullName, "mcuAbilityParam", "tuiMaxMtAdpConnSMcuAbility", MAXLIMIT_CRI_MTADP_SMCU, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byTuiMaxMtAdpConnSMcuAbility = (u8)nKeyVal;
    
    //HDI板上终端适配最大终端接入能力
    GetRegKeyInt(achFullName, "mcuAbilityParam", "hdiMaxMtAdpConnMtAbility", MAXLIMIT_HDI_MTADP, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byHdiMaxMtAdpConnMtAbility = (u8)nKeyVal;
    
    //是否限制polycom终端的编码发送（用于HD会议，适应HD-VMP的解码局限）
    GetRegKeyInt(achFullName, "mcuAbilityParam", "IsSendFakeCap2Polycom", 0, &nKeyVal);
    tMcuDebugVal.m_byIsSendFakeCap2Polycom = (u8)nKeyVal;

    //是否限制Taide终端的编码发送（用于HD会议，适应tandberg标清终端的能力解析局限）
    GetRegKeyInt(achFullName, "mcuAbilityParam", "IsSendFakeCap2Taide", 0, &nKeyVal);
    tMcuDebugVal.m_byIsSendFakeCap2Taide = (u8)nKeyVal;
    
    //是否增强Taide终端的编码发送（用于HD会议，适应tangberg高清终端的能力解析局限）
    GetRegKeyInt(achFullName, "mcuAbilityParam", "IsSendFakeCap2TaideHD", 0, &nKeyVal);
    tMcuDebugVal.m_byIsSendFakeCap2TaideHD = (u8)nKeyVal;
	
	//是否区分处理高清和标清的终端轮换
	GetRegKeyInt(achFullName, "mcuAbilityParam", "IsDistingtishSDHDMt", 0, &nKeyVal);
	tMcuDebugVal.m_byIsDistingtishSDHDMt = (u8)nKeyVal;

    //调度或选看 polycom是否 走适配或降分辨率
	GetRegKeyInt(achFullName, "mcuAbilityParam", "IsVidAdjustless4Polycom", 0, &nKeyVal);
	tMcuDebugVal.m_byIsVidAdjustless4Polycom = (u8)nKeyVal;

    //直接选看失败，是否强行起适配选看
    GetRegKeyInt(achFullName, "mcuAbilityParam", "IsSelAccord2Adp", 0, &nKeyVal);
    tMcuDebugVal.m_byIsSelAccord2Adp = (u8)nKeyVal;
    
    //是否为HD-BAS作负载预留，单位：Mbps。0：不作负载预留；
    GetRegKeyInt(achFullName, "mcuAbilityParam", "BandWidthReserved4HdBas", 0, &nKeyVal);
    tMcuDebugVal.m_byBandWidthReserved4HdBas = (u8)nKeyVal;

    //是否为HD-Vmp作负载预留，单位：Mbps。0：不作负载预留；
    GetRegKeyInt(achFullName, "mcuAbilityParam", "BandWidthReserved4HdVmp", 0, &nKeyVal);
    tMcuDebugVal.m_byBandWidthReserved4HdVmp = (u8)nKeyVal;

	return;
}

/*====================================================================
    函数名      : McuGetSateliteConfigValue
    功能        : 获取MCU 卫星会配置文件key value
    算法实现    :（可选项）
    引用全局变量: 无
    输入参数说明: TMcuSatInfo &tSatInfo
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/08/28    4.6         张宝卿       创建
====================================================================*/
void McuGetSateliteConfigValue(TMcuSatInfo &tSatInfo)
{
    s32 nKeyVal;
    s8  achFullName[64];
    s8  achReturn[MAX_VALUE_LEN + 1];
    s8  achDefStr[] = "Cannot find the section or key";
    BOOL32 bResult = FALSE;

	sprintf(achFullName, "%s/%s", DIR_CONFIG, MCU_SAT_CFGFILENAME);

    /*向网管申请的IP地址*/	
    bResult = GetRegKeyString( achFullName, "mcuSatNetWork", "mcunetApplyIpAddr", 
                               achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetApplyIpAddr" );
    }
    tSatInfo.SetApplyIp( ntohl( inet_addr( achReturn )));

    /*向网管申请的端口*/
    bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mcunetApplyPort", 
                            0, &nKeyVal );
    if( bResult == FALSE )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetApplyPort" );
    }
    tSatInfo.SetApplyPort( (u16)nKeyVal );
    
    /*网管回应的端口*/
    bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mcunetApplyRcvPort", 0, &nKeyVal );
    if( bResult == FALSE )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetApplyRcvPort" );
    }
	tSatInfo.SetApplyRcvPort( (u16)nKeyVal );

    /*mcu接收终端卫星信令端口*/
    bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mcuRcvMtSignalPort", 0, &nKeyVal );
    if ( !bResult )
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcuRcvMtSignalPort" );
    }
    tSatInfo.SetRcvMtSignalPort( (u16)nKeyVal );

    /*时间更新信令的地址*/
    bResult = GetRegKeyString( achFullName, "mcuSatNetWork", "mcunetTimeIpaddr", 
                               achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetTimeIpaddr" );
        return;
    }
    
    tSatInfo.SetTimeRefreshIpAddr( ntohl( inet_addr( achReturn ) ) );
    
    /*时间更新信令的端口*/
    bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mcunetTimePort", 
                            0, &nKeyVal );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetTimePort" );
        return;
    }
    tSatInfo.SetTImeRefreshPort( ( u16 )nKeyVal );
	

    /* mcuRcvMtMediaStartPort */
    bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mcuRcvMtMediaStartPort",
                            0, &nKeyVal );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcuRcvMtMediaStartPort" );
        return;
    }
    tSatInfo.SetMcuRcvMtMediaStartPort((u16)nKeyVal);
    
    
    /* mtRcvMcuMediaPort */
    bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mtRcvMcuMediaPort", 
                            0, &nKeyVal );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mtRcvMcuMediaPort" );
        return;
    }
	tSatInfo.SetMtRcvMcuMediaPort((u16)nKeyVal);

    // MCU组播信令IP地址(224.0.0.0与239.255.255.255之间)
    bResult = GetRegKeyString( achFullName, "mcuSatNetWork", "mcunetMulticastSignalIpAddr", 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetMulticastSignalIpAddr" );
        return;
    }
	tSatInfo.SetMcuMulitcastSignalIpAddr(ntohl( inet_addr( achReturn ) ));
    
    // MCU组播信令端口(必须是4的整数倍)
    bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mcunetMulticastSignalPort", 
        0, &nKeyVal );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetMulticastSignalPort" );
        return;
    }
	tSatInfo.SetMcuMulticastSignalPort((u16)nKeyVal);
    
    // MCU组播IP地址(224.0.0.0与239.255.255.255之间) 
    bResult = GetRegKeyString( achFullName, "mcuSatNetWork", "mcunetMulticastIpAddr", 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetMulticastIpAddr" );
        return;
    }
	tSatInfo.SetMcuMulticastDataIpAddr(ntohl( inet_addr( achReturn ) ));

    // MCU组播起始端口(必须是4的整数倍) 
    bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mcunetMulticastPort", 
        0, &nKeyVal );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetMulticastPort" );
        return;
    }
	tSatInfo.SetMcuMulticastDataPort((u16)nKeyVal);
	
    
    
    // MCU组播起始端口(必须是4的整数倍)
    bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mcunetMulticastPortCount", 
        0, &nKeyVal );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetMulticastPortCount" );
        return;
    }
	tSatInfo.SetMcuMulticastDataPortNum((u16)nKeyVal);
    
    // MCU组播TTL值
    bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mcunetMulticastTTL", 
        0, &nKeyVal );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetMulticastTTL" );
        return;
    }
	tSatInfo.SetMcuMulticastTTL((u8)nKeyVal);    
    
    // 最大上传路数
    bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mcunetMaxUploadCount", 
        0, &nKeyVal );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetMaxUploadCount" );
        return;
    }
	tSatInfo.SetMcuMaxUploadNum((u8)nKeyVal);

	// MCU接收终端数据起始端口
	bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mcuRcvMtMediaStartPort",
		0, &nKeyVal);
	if (!bResult)
	{
		OspPrintf(TRUE, FALSE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcuRcvMtMediaStartPort");
		return;
	}
	tSatInfo.SetMcuRcvMtMediaStartPort((u16)nKeyVal);

	// 终端接收MCU数据起始端口
	bResult = GetRegKeyInt( achFullName, "mcuSatNetWork", "mtRcvMcuMediaPort",
		0, &nKeyVal);
	if (!bResult)
	{
		OspPrintf(TRUE, FALSE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mtRcvMcuMediaPort");
		return;
	}
	tSatInfo.SetMtRcvMcuMediaPort((u16)nKeyVal);

	// MCU广播下行的第二组播地址（主要用于双视频目的多点会议，多视频目的需要进一步扩展）
    // (224.0.0.0与239.255.255.255之间)
    bResult = GetRegKeyString( achFullName, "mcuSatNetWork", "mcunetSecMulticastIpAddr", 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( !bResult )  
    {
        OspPrintf( TRUE, TRUE, "[McuGetSateliteConfigValue] Wrong profile while reading %s!\n", "mcunetSecMulticastIpAddr" );
        return;
    }
	tSatInfo.SetMcuSecMulticastIpAddr(ntohl( inet_addr( achReturn ) ));

    return;
}

/*=============================================================================
    函 数 名： McuGetMtCallInterfaceInfo
    功    能： 获取MCU Debug文件关于指定终端的协议适配板资源及码流转发板资源的设置
    算法实现： 
    全局变量： 
    参    数： TMtCallInterface *ptMtCallInterface
	           u32 &dwEntryNum
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/25   3.6			万春雷                  创建
    2006/12/20  4.0-R4      王亮                    增加E164号码段支持
=============================================================================*/
BOOL32 McuGetMtCallInterfaceInfo( TMtCallInterface *ptMtCallInterface, u32 &dwEntryNum )
{
	BOOL32  bRet = FALSE;
	
	if( NULL == ptMtCallInterface )
	{
		return bRet;
	}

	char achFullName[64];
    s8   pszE164Start[MAXLEN_E164] = {0};
    s8   pszE164End[MAXLEN_E164] = {0};
	s32  nEntryNum = 0;
	s8   chSeps[] = " \t"; //space or tab as seperators
	s8  *pchToken;
	u32  dwLoop = 0;
	u32  dwValidNum = 0;
	s8 **lpszTable;
	
	sprintf(achFullName, "%s/%s", DIR_CONFIG, MCUDEBUGFILENAME);
    
	//get the number of entry
	bRet = GetRegKeyInt( achFullName, "MtCallInterfaceTable", "EntryNum", 0, &nEntryNum );
	if( FALSE == bRet ) 
	{
		// OspPrintf( TRUE, FALSE, "McuGetMtCallInterfaceInfo Err while reading %s %s!\n", "MtCallInterfaceTable", "EntryNum" );
		return FALSE;
	}
	if( nEntryNum < 0 )
	{
		OspPrintf( TRUE, FALSE, "McuGetMtCallInterfaceInfo Err2 while reading %s %s!\n", "MtCallInterfaceTable", "EntryNum" );
		return FALSE;
	}

	if( nEntryNum > MAXNUM_MCU_MT )
	{
		nEntryNum = MAXNUM_MCU_MT;
	}
    
	if( nEntryNum <= (s32)dwEntryNum )
	{
		dwEntryNum = (u32)nEntryNum;
	}

	//alloc memory
	lpszTable = (s8 **)malloc( dwEntryNum * sizeof( s8* ) );
	for( dwLoop = 0; dwLoop < (u32)dwEntryNum; dwLoop++ )
	{
		lpszTable[dwLoop] = (s8 *)malloc( MAX_VALUE_LEN+1 );
	}
	
	//get the table string
	bRet = GetRegKeyStringTable( achFullName, "MtCallInterfaceTable", "fail", lpszTable, &dwEntryNum, (MAX_VALUE_LEN+1) );
	if( FALSE == bRet ) 
	{
		printf( "McuGetMtCallInterfaceInfo Err while reading %s table!\n", "MtCallInterfaceTable" );
		return bRet;
	}

	for( dwLoop = 0; dwLoop < dwEntryNum; dwLoop++ )
	{
        
        u32 dwStartNetIp = 0;
        u32 dwEndNetIp = 0;
        u8  byAliasType = mtAliasTypeOthers;
		pchToken = strtok( lpszTable[dwValidNum], chSeps );

		//CalledMtMtadpIPAddr		
		if( NULL == pchToken )
		{
			printf( "McuGetMtCallInterfaceInfo Err while reading %s entryID.%d!\n", "CalledMtMtadpIPAddr", dwLoop );
			continue;
		}
		else
		{
			ptMtCallInterface[dwValidNum].m_dwMtadpIpAddr = ntohl( INET_ADDR(pchToken) );
		}

		//CalledMtMPIPAddr
		pchToken = strtok( NULL, chSeps );
		if( NULL == pchToken )
		{
			printf( "McuGetMtCallInterfaceInfo Err while reading %s entryID.%d!\n", "CalledMtMPIPAddr", dwLoop );
			continue;
		}
		else
		{
			ptMtCallInterface[dwValidNum].m_dwMpIpAddr = ntohl( INET_ADDR(pchToken) );
		}

        pchToken = strtok( NULL, chSeps );
        // 起始IP或者是E164号码
		if( NULL == pchToken )
		{
			printf( "McuGetMtCallInterfaceInfo Err while reading %s entryID.%d!\n", "StartIP", dwLoop );
			continue;
		}
		else
		{
			u8 byMtAliasType = GetMtAliasTypeFromString( pchToken );

            switch (byMtAliasType)
            {
            case mtAliasTypeTransportAddress:
                dwStartNetIp = INET_ADDR(pchToken); //要求网络序
                byAliasType = mtAliasTypeTransportAddress;
                //继续解析IP结束地址
                pchToken = strtok(NULL, chSeps);
                if( NULL == pchToken)
                {
			        printf("McuGetMtCallInterfaceInfo Err while reading %s entryID.%d!\n", "StartIp", dwLoop );
                    continue;
                }
                else
                {
                    byMtAliasType = GetMtAliasTypeFromString( pchToken );
                    if( mtAliasTypeTransportAddress != byMtAliasType )
                    {
                        printf( "McuGetMtCallInterfaceInfo Err Invalid IP while reading %s entryID.%d!\n", "EndIp", dwLoop );
                        continue;
                    }
                    dwEndNetIp = INET_ADDR(pchToken); //要求网络序
                    ptMtCallInterface[dwValidNum].SetIpSeg( dwStartNetIp, dwEndNetIp );
                }
                break;
            case mtAliasTypeE164:
                // 读取配置文件中E164号码
                ptMtCallInterface[dwValidNum].SetE164Alias( pchToken );
                break;
            default:
                printf( "McuGetMtCallInterfaceInfo Err Invalid IP while reading %s entryID.%d!\n", "StartIP", dwLoop );
                continue;
            }
        }
        /* del by wangliang 2006-12-20 for E164 seg support start */
        /*
        if ( byAliasType == mtAliasTypeTransportAddress )
        {
            // 结束IP
            pchToken = strtok( NULL, chSeps );
		    if( NULL == pchToken )
		    {
			    printf("McuGetMtCallInterfaceInfo Err while reading %s entryID.%d!\n", "EndIp", dwLoop );
			    continue;
		    }
		    else
		    {
			    u8 byMtAliasType = GetMtAliasTypeFromString( pchToken );
			    
			    if( mtAliasTypeTransportAddress != byMtAliasType )
			    {
				    printf( "McuGetMtCallInterfaceInfo Err Invalid IP while reading %s entryID.%d!\n", "EndIp", dwLoop );
				    continue;
			    }
                dwEndNetIp = INET_ADDR(pchToken); //要求网络序
                ptMtCallInterface[dwValidNum].SetIpSeg( dwStartNetIp, dwEndNetIp );
		    }            
        }
        */
        /* del by wangliang 2006-12-20 for E164 seg support end */
        
		dwValidNum++;
	}

	
	//free memory
	for( dwLoop = 0; dwLoop < dwEntryNum; dwLoop++ )
	{
		if( NULL != lpszTable[dwLoop] )
		{
			free( lpszTable[dwLoop] );
		}
	}
	free( lpszTable );
    
	dwEntryNum = dwValidNum;
	bRet = TRUE;
	return bRet;
}

/*=============================================================================
    函 数 名： GetMtAliasTypeFromString
    功    能： 从待判别的字串判别终端类型
    算法实现： 
    全局变量： 
    参    数： s8* pszAliasString 待判别的字串
    返 回 值： u8 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/25   3.6			万春雷                  创建
=============================================================================*/
u8 GetMtAliasTypeFromString(s8* pszAliasString)
{
	if(IsIPTypeFromString(pszAliasString))
	{	
		//ip
		return mtAliasTypeTransportAddress;
	}
	else if(IsE164TypeFromString(pszAliasString,"0123456789*,#"))
	{
		//e164
		return mtAliasTypeE164;
	}
	else
	{
		//h323
		return mtAliasTypeH323ID;
	}
	
	return mtAliasTypeOthers;
}

/*=============================================================================
    函 数 名： IsIPTypeFromString
    功    能： 从待判别的字串判别是否为ip字串
    算法实现： 
    全局变量： 
    参    数： s8* pszAliasString 待判别的字串
    返 回 值： BOOL32
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/25   3.6			万春雷                  创建
=============================================================================*/
BOOL32 IsIPTypeFromString(s8* pszAliasString)
{
	s32 anValue[4] = {0};
	s32 anLen[4] = {0};
	s32 nDot   = 0;
	s8 *pszTmp = pszAliasString;
	s32 nPos   = 0;
	for(nPos=0; *pszTmp&&nPos<16; nPos++,pszTmp++)
	{
		if( '.' == *pszTmp )
		{
			nDot++;
			//excude 1256.1.1.1.1
			if(nDot > 3)
			{
				return FALSE;
			}
			continue;			
		}
		//excude a.1.1.1
		if( *pszTmp<'0'|| *pszTmp>'9' )
		{
			return FALSE;
		}
		
		anValue[nDot] = anValue[nDot]*10 + (*pszTmp-'0');
		anLen[nDot]++;
	}
	
	//excude 1256.1.1.1234444
	if( nPos >=16 )
	{
		return FALSE;
	}
	
	//excude 0.1.1.1
	if( 0 == anValue[0] )
	{
		return FALSE;
	}
	
	for(nPos=0; nPos<4; nPos++)
	{
		//excude 1256.1.1.1
		if( (0 == anLen[nPos]) || (anLen[nPos] > 3) )
		{
			return FALSE;
		}
		//excude 256.1.1.1
		if(anValue[nPos] > 255)
		{
			return FALSE;
		}
	}

	return TRUE;
}

/*=============================================================================
    函 数 名： IsIPTypeFromString
    功    能： 从待判别的字串判别是否为E164字串
    算法实现： 
    全局变量： 
    参    数： s8* pszAliasString 待判别的字串
    返 回 值： BOOL32
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/25   3.6			万春雷                  创建
=============================================================================*/
BOOL32 IsE164TypeFromString(s8 *pszSrc, s8* pszDst)
{
	s8 *pszSrcTmp = pszSrc;
	s8 *pszDstTmp = pszDst;
	while(*pszSrcTmp)
	{
		pszDstTmp = pszDst;
		while(*pszDstTmp)
		{
			if(*pszSrcTmp == *pszDstTmp) 
			{
				break;
			}
			*pszDstTmp++;
		}
		if( 0 == *pszDstTmp )
		{
			return FALSE;
		}
		pszSrcTmp++;
	}

	return TRUE;
}

/*=============================================================================
函 数 名： IsVidFormatHD
功    能： 是否为高清分辨率
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2007/12/19  4.5			张宝卿                  创建
=============================================================================*/
BOOL32 IsVidFormatHD(u8 byVidFormat)
{
    BOOL32 bFormatHD = FALSE;
    switch( byVidFormat )
    {
    case VIDEO_FORMAT_W4CIF:
    case VIDEO_FORMAT_HD720:
    case VIDEO_FORMAT_SXGA:
    case VIDEO_FORMAT_UXGA:
    case VIDEO_FORMAT_HD1080:
        bFormatHD = TRUE;
        break;
    default:
        break;
    }
    return bFormatHD;
}

/*
    配置会议信息文件读写策略: 一个会议或模板信息对应一个文件中: confinfo_index.dat 
                              (会议 index <- [0, MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE-1])
                              会议头信息及缺省会议信息对应一个独立的文件中: confinfo_head.dat
                              (缺省会议 index == MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLAT)

    用于存储会议信息的文件结构为：
    confinfo_head.dat CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1] + 
	                  TPackConfStore + m_wAliasBufLen字节m_pbyAliasBuf + TTvWallModule(opt) + TVmpModule(opt);
    confinfo_pos.dat  TPackConfStore + m_wAliasBufLen字节m_pbyAliasBuf + TTvWallModule(opt) + TVmpModule(opt);

    实际有效的存储会议数为 (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE)会议或模板 + 一个缺省会议
*/

/*
    guzh [4/3/2007]
    为了避免读写缺省会议导致会议头信息写空,把缺省会议信息保存到confinfo_128(32).dat中,
    不再保存在 confinfo_head.dat 中.
*/


/*====================================================================
    函数名      : BeginRWConfInfoFile
    功能        : 准备读写会议文件信息
    算法实现    :
    引用全局变量: 无
    输入参数说明: 无
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/04/03    4.0         顾振华         创建
====================================================================*/
BOOL32 BeginRWConfInfoFile()
{
    if (OspSemTakeByTime(g_hConfInfoRW, 15000))
    {
        return TRUE;
    }
    else
    {
        printf("[BeginRWConfInfoFile] OspSemTakeByTime failed!\n");
        return FALSE;
    }
}

/*====================================================================
    函数名      : EndRWConfInfoFile
    功能        : 结束读写会议文件信息
    算法实现    :
    引用全局变量: 无
    输入参数说明: 无
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    07/04/03    4.0         顾振华         创建
====================================================================*/
void EndRWConfInfoFile()
{
    OspSemGive(g_hConfInfoRW);
}

/*====================================================================
    函数名      : CreateConfStoreFile
    功能        : 在无存储文件的条件下构造会议文件(本函数只能调用一次)
    算法实现    :
    引用全局变量: 无
    输入参数说明: 无
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/31    3.0         胡昌威         创建
    07/04/03    4.0         顾振华         修改
====================================================================*/
BOOL32 CreateConfStoreFile(void)
{
	//check data files' path
#if defined(WIN32)
	CreateDirectory(DIR_DATA, NULL);
#elif defined(_VXWORKS_)
	mkdir((LPSTR)DIR_DATA);
#elif defined(_LINUX_)
    mkdir((LPSTR)DIR_DATA, 0777);
    chmod((LPSTR)DIR_DATA, 0777);
#endif

    // 创建文件读写信号量
    if (NULL != g_hConfInfoRW)
    {
        OspSemDelete( g_hConfInfoRW );
        g_hConfInfoRW = NULL;
    }
    if( !OspSemBCreate( &g_hConfInfoRW ) )
    {
        OspSemDelete( g_hConfInfoRW );
        g_hConfInfoRW = NULL;
        printf("[CreateConfStoreFile] create conf info readwrite lock failed!\n" );
        return FALSE;
    }
	
	FILE *hFile = NULL;
	s8    achFullName[64];
    if ( ! BeginRWConfInfoFile() )
    {
        return FALSE;
    }

	//读取 会议头信息_存储文件
	sprintf(achFullName, "%s/%s", DIR_DATA, CONFINFOFILENAME_HEADINFO);
	hFile = fopen(achFullName, "rb"); 
	if (NULL == hFile)
	{
		hFile = fopen(achFullName, "wb");
		if (NULL != hFile)
		{
			//写会议头 文件信息
            CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
            memset(acConfId, 0, sizeof(acConfId));
			fwrite(acConfId, sizeof(acConfId), 1, hFile);
		}
	}
	if (NULL != hFile)
	{
		fclose(hFile);
		hFile = NULL;
	}

    EndRWConfInfoFile();

	//会议或模板信息_存储文件不进行预先生成
	/*
	for (s32 nPos = 0; nPos < (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE); nPos++)
	{
		//读取 会议或模板信息_存储文件
		sprintf(achFullName, "%s/%s%d%s", DIR_DATA, CONFINFOFILENAME_PREFIX, nPos, CONFINFOFILENAME_POSTFIX);
		hFile = fopen(achFullName, "r+b"); 
		if (NULL == hFile)
		{
			hFile = fopen(achFullName, "w+b");
			if (NULL != hFile)
			{
				//写会议或模板 文件信息
				fwrite(&tNullConfStore, sizeof(tNullConfStore), 1, hFile);
			}
		}
		if (NULL != hFile)
		{
			fclose(hFile);
			hFile = NULL;
		}
	}
	*/

    return TRUE;
}

/*====================================================================
    函数名      : AddConfToFile
    功能        : 将指定的会议记录存储在文件中待用
    算法实现    :（可选项）
    引用全局变量: 无
    输入参数说明: TConfStore &tConfStore    待存储的会议信息(未进行Pack处理的会议数据)
	              BOOL32 bDefaultConf = FALSE 是否存为缺省会议(如果是，则强制覆盖当前的缺省会议
    返回值说明  : BOOL32
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/13    3.0         胡昌威          创建
====================================================================*/
BOOL32 AddConfToFile( TConfStore &tConfStore, BOOL32 bDefaultConf )
{
	FILE *hHeadFile = NULL;
	FILE *hConfFile = NULL;

	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
	u8 byConfPos = MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE;
	TConfStore tConfStoreBuf;
	TPackConfStore *ptPackConfStore = (TPackConfStore *)&tConfStoreBuf;
	u16 wPackConfDataLen = 0;
	CConfId cDefConfConfId;
	CConfId cConfConfId;
	memset(acConfId, 0, sizeof(acConfId));
	cDefConfConfId.SetNull();

	//1.在头信息记录中 获取 会议或模板的相对位置
	GetAllConfHeadFromFile(acConfId, sizeof(acConfId));

	//查找已有会议：此会议已保存，覆盖，不包含缺省会议位置
	for (s32 nPos = 0; nPos < (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE); nPos++)
	{
		if (acConfId[nPos] == tConfStore.m_tConfInfo.GetConfId())
		{
			byConfPos = (u8)nPos;
			break;
		}
	}
	//未存，找一个空位
	if (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE == byConfPos)
	{
		byConfPos = 0;
		while (!acConfId[byConfPos].IsNull() && byConfPos<(MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE))
		{
			byConfPos++;
		}
	}
	//已满
	if (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE == byConfPos) 
	{
		return FALSE;
	}

	//2.将 未进行Pack处理的会议数据 进行Pack处理
	if (FALSE == PackConfStore(tConfStore, ptPackConfStore, wPackConfDataLen))
	{
		return FALSE;
	}

	//3.保存 该会议或模板信息 到指定索引的存储文件
	cConfConfId = ptPackConfStore->m_tConfInfo.GetConfId();
	if (FALSE == SetConfDataToFile(byConfPos, cConfConfId, (u8*)ptPackConfStore, wPackConfDataLen))
	{
		return FALSE;
	}
	
	//记录该会议或模板头信息
	acConfId[byConfPos] = ptPackConfStore->m_tConfInfo.GetConfId();

	//记录缺省会议头信息
	if (bDefaultConf || acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE].IsNull())
	{
		if (!bDefaultConf)
		{
            //会议ID不存在，创建一个
			cDefConfConfId.CreateConfId(USRGRPID_INVALID);
			cDefConfConfId.SetConfSource(ptPackConfStore->m_tConfInfo.GetConfSource());
			ptPackConfStore->m_tConfInfo.SetConfId(cDefConfConfId);			
		}
		else
		{
            // guzh [8/1/2006] 已存在，则修改为无用户权限限制
			cDefConfConfId = ptPackConfStore->m_tConfInfo.GetConfId();
            cDefConfConfId.SetUsrGrpId(USRGRPID_INVALID);
            ptPackConfStore->m_tConfInfo.SetConfId(cDefConfConfId);            
		}
        ptPackConfStore->m_tConfInfo.m_tStatus.SetOngoing();  // libo [10/31/2005]
	}
	
	//4.将 该会议或模板信息作为缺省会议 保存到指定索引的存储文件
	if (!cDefConfConfId.IsNull())
	{
		//允许保存缺省失败
		if (SetConfDataToFile((MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE), 
			                  cDefConfConfId, (u8*)ptPackConfStore, wPackConfDataLen))
		{
			acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE] = cDefConfConfId;
		}
	}
	
	//5.重新保存 刷新后的所有会议或模板头信息
	if (FALSE == SetAllConfHeadToFile(acConfId, sizeof(acConfId)))
	{
		return FALSE;
	}

	return TRUE;
}

/*====================================================================
    函数名      : DeleteConfFromFile
    功能        : 删除文件中存储的会议记录
    算法实现    :（可选项）
    引用全局变量: 无
    输入参数说明: CConfId cConfId
    返回值说明  : BOOL32
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/13    3.0         胡昌威         创建
    07/04/03    4.0         顾振华         修改
====================================================================*/
BOOL32 DeleteConfFromFile(CConfId cConfId)
{
	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
	u8 byConfPos = MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE;
	memset(acConfId, 0, sizeof(acConfId));
	
	//1. 读取 会议头信息_存储文件
    if ( !GetAllConfHeadFromFile(acConfId, sizeof(acConfId)) )
    {
        return FALSE;
    }
	
	//查找已有会议，不包含缺省会议位置
	for (s32 nPos = 0; nPos < (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE); nPos++)
	{
		if (acConfId[nPos] == cConfId)
		{
			byConfPos = (u8)nPos;
			break;
		}
	}
	
	//未保存
	if (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE == byConfPos)
	{
		return FALSE;
	}

	//2.清除该会议或模板在头信息上的记录信息
	acConfId[byConfPos].SetNull();

    //3.重新保存
    if (FALSE == SetAllConfHeadToFile(acConfId, sizeof(acConfId)))
    {
        return FALSE;
	}    
	
	return TRUE;	
}

/*====================================================================
    函数名      : GetConfFromFile
    功能        : 从文件中获取存储的会议记录
    算法实现    :（可选项）
    引用全局变量: 无
    输入参数说明: [IN]  u8 byIndex, 
	              [IN]  TPackConfStore *ptPackConfStore 大小为预留的 sizeof(TConfStore)
    返回值说明  : BOOL32
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/13    3.0         胡昌威          创建
====================================================================*/
BOOL32 GetConfFromFile(u8 byIndex, TPackConfStore *ptPackConfStore)
{
	//包含缺省会议位置
	if (NULL == ptPackConfStore || 
		byIndex > MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE)
	{
		return FALSE;
	}
	CConfId cConfId = GetOneConfHeadFromFile(byIndex);
	if (cConfId.IsNull()) //未保存
	{
		return FALSE;
	}

	u16 wOutBufLen = 0;
	return GetConfDataFromFile(byIndex, cConfId, (u8*)ptPackConfStore, 
		                       sizeof(TConfStore), wOutBufLen);
}

/*=============================================================================
    函 数 名： GetOneConfHeadFromFile
    功    能： 从文件中获取存储的指定索引的会议或模板头信息记录
    算法实现： 
    全局变量： 
    参    数： [IN]  u8 byIndex
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/29  4.0			万春雷                  创建
=============================================================================*/
CConfId GetOneConfHeadFromFile( u8 byIndex )
{
	CConfId cConfId;
	cConfId.SetNull();

	//包含缺省会议位置
	if (byIndex > MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE)
	{
		return cConfId;
	}
	
	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
	memset(acConfId, 0, sizeof(acConfId));
	
	//读取 会议头信息及缺省会议信息_存储文件
    if ( !GetAllConfHeadFromFile(acConfId, sizeof(acConfId)) )
    {
        return cConfId;
    }
    else
    {
        return acConfId[byIndex];
    }	
}

/*=============================================================================
    函 数 名： GetConfHeadFromFile
    功    能： 从文件中获取存储的所有会议或模板头信息记录
    算法实现： 
    全局变量： 
    参    数： CConfId *pacConfId
	           u32 dwInBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/27  4.0			万春雷                  创建
=============================================================================*/
BOOL32 GetAllConfHeadFromFile( CConfId *pacConfId, u32 dwInBufLen )
{
	FILE *hHeadFile = NULL;
	s8    achFullName[64];
	u32   dwOutBufLen = (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1)*sizeof(CConfId);
	if (NULL == pacConfId || dwInBufLen < dwOutBufLen)
	{
		return FALSE;
	}
	memset((s8*)pacConfId, 0, dwOutBufLen);
	
    if (!BeginRWConfInfoFile())
    {
        return FALSE;
    }

	//读取 会议头信息_存储文件
	sprintf(achFullName, "%s/%s", DIR_DATA, CONFINFOFILENAME_HEADINFO);
	hHeadFile = fopen(achFullName, "rb"); 
	if (NULL != hHeadFile)
	{
        fread((s8*)pacConfId, dwOutBufLen, 1, hHeadFile);				
        fclose(hHeadFile);
	}

    EndRWConfInfoFile();

	return (NULL != hHeadFile);
}

/*=============================================================================
    函 数 名： SetAllConfHeadToFile
    功    能： 将所有会议或模板头记录 存储到指定的会议文件中
    算法实现： 
    全局变量： 
    参    数： CConfId *pacConfId
	           u32 dwInBufLen
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/27  4.0			万春雷                  创建
=============================================================================*/
BOOL32 SetAllConfHeadToFile( CConfId *pacConfId, u32 dwInBufLen )
{
	FILE *hHeadFile = NULL;
	s8    achFullName[64];
	u32   dwOutBufLen = (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1)*sizeof(CConfId);
	if (NULL == pacConfId || dwInBufLen < dwOutBufLen)
	{
		return FALSE;
	}

    // guzh [4/3/2007] 首先尝试读取,判断是否完全相同
    CConfId acTmpConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
    if ( GetAllConfHeadFromFile(acTmpConfId, sizeof(acTmpConfId)) )
    {
        if ( 0 == memcmp( acTmpConfId, pacConfId, dwOutBufLen ) )
        {
            // 完全相同,无需再次保存
            return TRUE;
        }
    }

    sprintf(achFullName, "%s/%s", DIR_DATA, CONFINFOFILENAME_HEADINFO);
    // guzh [4/3/2007] 缺省会议单独保存,这里直接覆盖即可
    if (!BeginRWConfInfoFile())
    {
        return FALSE;
    }
    hHeadFile = fopen(achFullName, "wb");
    if (NULL != hHeadFile)
    {
        fwrite((s8*)pacConfId, dwOutBufLen, 1, hHeadFile);
        fclose(hHeadFile);        
    }

    EndRWConfInfoFile();
	
	return (hHeadFile != NULL);
}

/*=============================================================================
    函 数 名： GetConfDataFromFile
    功    能： 从文件中获取存储的会议记录
    算法实现： 
    全局变量： 
    参    数： [IN]  u8  byIndex
               [IN]  CConfId cConfId
               [IN]  u8 *pbyBuf  已进行Pack处理的会议数据
			         TPackConfStore + m_wAliasBufLen字节m_pbyAliasBuf + TTvWallModule(opt) + TVmpModule(opt);
               [IN]  u32 dwInBufLen 主机序
               [OUT] u16 &wOutBufLen 主机序
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/27  4.0			万春雷                  创建
    07/04/03    4.0         顾振华                  修改
=============================================================================*/
BOOL32 GetConfDataFromFile(u8 byIndex, CConfId cConfId, u8 *pbyBuf, u32 dwInBufLen, u16 &wOutBufLen)
{
	wOutBufLen = 0;

	//可获取缺省会议信息
	if (byIndex > MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE)
	{
		return FALSE;
	}
	//pbyBuf 为已进行Pack处理的会议数据，长度应不小于sizeof(TPackConfStore)
	if (NULL == pbyBuf || dwInBufLen < sizeof(TPackConfStore))
	{
		return FALSE;
	}

	FILE *hConfFile = NULL;
	s8    achFullName[64];
	s32   nFileLen = 0;
	
    // guzh [4/3/2007] 缺省会议单独保存
	//读取 指定索引的会议或模板信息 存储文件 信息
    sprintf(achFullName, "%s/%s%d%s", DIR_DATA, CONFINFOFILENAME_PREFIX, byIndex, CONFINFOFILENAME_POSTFIX);

    if (!BeginRWConfInfoFile())
    {
        return FALSE;
    }

	hConfFile = fopen(achFullName, "rb"); 
	if (NULL == hConfFile)
	{
        EndRWConfInfoFile();
		return FALSE;
	}

	fseek(hConfFile, 0, SEEK_END);
	nFileLen = ftell(hConfFile);
    //重新判断缓冲长度的有效性
//    if (nFileLen < sizeof(TPackConfStore) || dwInBufLen < (u32)nFileLen )
//    {
//        EndRWConfInfoFile();
//        return FALSE;
//    }
    
    wOutBufLen = (u16)nFileLen;	

    fseek(hConfFile, 0, SEEK_SET);

	// confinfo size notify, zgc, 20070524
	s8 achConfFileHead[50] = {0};
	u16 wFileTConfInfoSize = 0;
	u32 dwSpace;

	// 读文件头, 区分是新版本还是老版本
	BOOL32 bIsNewVersion = TRUE;
	fread( achConfFileHead, strlen(CONFFILEHEAD), 1, hConfFile );
	if( 0 == memcmp( achConfFileHead, CONFFILEHEAD, strlen(CONFFILEHEAD) ) )
	{
		fread( &wFileTConfInfoSize, sizeof(u16), 1, hConfFile );
		wFileTConfInfoSize = ntohs(wFileTConfInfoSize);
		fread( &dwSpace, sizeof(u32), 1, hConfFile );
		wOutBufLen = wOutBufLen - strlen(CONFFILEHEAD) - sizeof(u16) - sizeof(u32);
	}
	else
	{
		bIsNewVersion = FALSE;
		fseek(hConfFile, 0, SEEK_SET);		
	}

	if ( bIsNewVersion && 0 == wFileTConfInfoSize )
	{
		OspPrintf( TRUE, FALSE, "[GetConfDataFromFile] In new version, FileTConfInfoSize can't be 0!\n");
		fclose(hConfFile);
		EndRWConfInfoFile();
		hConfFile = NULL;
		return FALSE;
	}

	// 内存保护
	if( !bIsNewVersion && dwInBufLen < wOutBufLen )
	{
		OspPrintf( TRUE, FALSE, "[GetConfDataFromFile] The input buf length(%d) is less than output buf(%d)!\n", dwInBufLen, wOutBufLen );
		fclose(hConfFile);
		EndRWConfInfoFile();
		hConfFile = NULL;
        return FALSE;
	}
	// confinfo size notify end
	
	if( !bIsNewVersion )
	{
		wOutBufLen = ( wOutBufLen <= sizeof(TPackConfStore) ) ? wOutBufLen : sizeof(TPackConfStore);
		fread(pbyBuf, wOutBufLen, 1, hConfFile);
		fclose(hConfFile);
		hConfFile = NULL;
		TPackConfStore *ptTemp = (TPackConfStore *)pbyBuf;
		ptTemp->m_byMtNum = 0;
		ptTemp->m_wAliasBufLen = 0;
	}
	// confinfo size notify, zgc, 20070524
	else
	{
		u16 wMutualLen = min( wFileTConfInfoSize, sizeof(TConfInfo) );
		// 内存保护
		if ( dwInBufLen < (u32)( wOutBufLen - wFileTConfInfoSize + wMutualLen ) )
		{
			OspPrintf( TRUE, FALSE, "[GetConfDataFromFile] The buf length is unenough!\n" );
			EndRWConfInfoFile();
			fclose(hConfFile);
			return FALSE;
		}

		u8 *pbyTemp = new u8[wOutBufLen];
		memset( pbyTemp, 0, wOutBufLen );
		fread( pbyTemp, wOutBufLen, 1, hConfFile );
		fclose( hConfFile );
		hConfFile = NULL;
		
		memcpy( pbyBuf, pbyTemp, wMutualLen );
		memcpy( pbyBuf + wMutualLen, pbyTemp + wFileTConfInfoSize, wOutBufLen - wFileTConfInfoSize );
		
		wOutBufLen = wOutBufLen - wFileTConfInfoSize + wMutualLen;

		delete [] pbyTemp;
	}
	// confinfo size notify end

	//校验 该会议或模板 头信息
	TPackConfStore *ptPackConfStore = (TPackConfStore *)pbyBuf;

	if (!(cConfId == ptPackConfStore->m_tConfInfo.GetConfId()))
	{
		wOutBufLen = 0;
        EndRWConfInfoFile();
		return FALSE;
	}

    EndRWConfInfoFile();
	return TRUE;
}

/*=============================================================================
    函 数 名： SetConfDataToFile
    功    能： 将某一会议记录 存储到指定的会议文件中
    算法实现： 
    全局变量： 
    参    数： [IN]   u8  byIndex
               [OUT]  CConfId &cConfId
               [IN]   u8 *pbyBuf 已进行Pack处理的会议数据
			          TPackConfStore + m_wAliasBufLen字节m_pbyAliasBuf + TTvWallModule(opt) + TVmpModule(opt);
               [IN]   u16 wInBufLen 主机序
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/27  4.0			万春雷                  创建
=============================================================================*/
BOOL32 SetConfDataToFile(u8 byIndex, CConfId &cConfId, u8 *pbyBuf, u16 wInBufLen)
{
	cConfId.SetNull();

	//可保存缺省会议信息
	if (byIndex > MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE)
	{
		return FALSE;
	}
	//pbyBuf 为已进行Pack处理的会议数据，长度应不小于sizeof(TPackConfStore)
	if (NULL == pbyBuf || wInBufLen < sizeof(TPackConfStore))
	{
		return FALSE;
	}
	
	//提取并校验 该会议或模板 头信息
	TPackConfStore *ptPackConfStore = (TPackConfStore *)pbyBuf;
	cConfId = ptPackConfStore->m_tConfInfo.GetConfId();
	if (cConfId.IsNull())
	{
		return FALSE;
	}

    // guzh [4/3/2007] 首先尝试读取,相同则不保存
    TConfStore tStore;
    u16 wOutLen;
    if (GetConfDataFromFile(byIndex, cConfId, (u8*)&tStore, sizeof(tStore), wOutLen))
    {
        if ( memcmp(&tStore, pbyBuf, wOutLen) == 0)
        {
            // 内容相同，不需要再次保存
            return TRUE;
        }
    }

    if ( !BeginRWConfInfoFile() )
    {
        return FALSE;                
    }

    FILE *hConfFile = NULL;
    s8    achFullName[64];	
    sprintf(achFullName, "%s/%s%d%s", DIR_DATA, CONFINFOFILENAME_PREFIX, byIndex, CONFINFOFILENAME_POSTFIX);

	hConfFile = fopen(achFullName, "wb");
	if (NULL != hConfFile)
	{
		// confinfo size notify, zgc, 20070524
		s8 achConfFileHead[50];
		memcpy( achConfFileHead, CONFFILEHEAD, strlen(CONFFILEHEAD) );
		fwrite( achConfFileHead, strlen(CONFFILEHEAD), 1, hConfFile );
		u16 wTConfInfoSize = sizeof(TConfInfo);
		wTConfInfoSize = htons(wTConfInfoSize);
		fwrite( &wTConfInfoSize, sizeof(wTConfInfoSize), 1, hConfFile );
		u32 dwSpace = 0;
		fwrite( &dwSpace, sizeof(dwSpace), 1, hConfFile );
		// confinfo size notify end

        fwrite(pbyBuf, wInBufLen, 1, hConfFile);
	    fclose(hConfFile);
	}	
	
    EndRWConfInfoFile();

	return (hConfFile != NULL);
}
/*=============================================================================
    函 数 名： GetUnProConfDataToFile
    功    能： unprocconfinfo_confindex.dat文件中读取某一会议模板不需要处理的数据
    算法实现： 
    全局变量： 
    参    数： [IN]   u8  byIndex
               [IN/OUT]   u8 *pbyBuf mcu不需要处理的会议模板相关信息(如分组信息)
               [OUT]   u16 wOutBufLen 主机序
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		创建人		走读人    
    08/11/26                付秀华
=============================================================================*/
BOOL32 GetUnProConfDataToFile(u8 byIndex, s8 *pbyBuf, u16& wOutBufLen)
{
	wOutBufLen = 0;

	//可保存缺省会议信息
	if (byIndex > MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE)
	{
		return FALSE;
	}

	if (NULL == pbyBuf)
	{
		return FALSE;
	}
	
    if ( !BeginRWConfInfoFile() )
    {
        return FALSE;                
    }

    FILE *hConfFile = NULL;
    s8    achFullName[64];	
    sprintf(achFullName, "%s/%s%d%s", DIR_DATA, UNPROCCONFINFOFILENAME_PREFIX, byIndex, CONFINFOFILENAME_POSTFIX);

	hConfFile = fopen(achFullName, "rb");
	if (NULL != hConfFile)
	{
		fseek(hConfFile, 0, SEEK_END);
		wOutBufLen = (u16)ftell(hConfFile);
		fseek(hConfFile, 0, SEEK_SET);

		fread(pbyBuf, wOutBufLen, 1, hConfFile);
		u16 dwLen = *(u16*)pbyBuf;
		dwLen = htons(dwLen);
		memcpy(pbyBuf, &dwLen, sizeof(u16));
	    fclose(hConfFile);
	}

    EndRWConfInfoFile();

	return (hConfFile != NULL);
}

/*=============================================================================
    函 数 名： SetUnProConfDataToFile
    功    能： 将某一会议模板不需要处理的数据直接存储到对应的unprocconfinfo_confindex.dat文件中
    算法实现： 
    全局变量： 
    参    数： [IN]   u8  byIndex
               [IN]   s8 *pbyBuf mcu不需要处理的会议模板相关信息(如分组信息)
               [IN]   u16 wInBufLen 主机序
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		创建人		走读人    
    08/11/26                付秀华
=============================================================================*/
BOOL32 SetUnProConfDataToFile(u8 byIndex, s8 *pbyBuf, u16 wInBufLen)
{
	//可保存缺省会议信息
	if (byIndex > MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE)
	{
		return FALSE;
	}

	if (NULL == pbyBuf )
	{
		return FALSE;
	}
	
    if ( !BeginRWConfInfoFile() )
    {
        return FALSE;                
    }

    FILE *hConfFile = NULL;
    s8    achFullName[64];	
    sprintf(achFullName, "%s/%s%d%s", DIR_DATA, UNPROCCONFINFOFILENAME_PREFIX, byIndex, CONFINFOFILENAME_POSTFIX);

	hConfFile = fopen(achFullName, "wb");
	if (NULL != hConfFile)
	{
        fwrite( pbyBuf, wInBufLen, 1, hConfFile);
	    fclose(hConfFile);
	}	
	
    EndRWConfInfoFile();

	return (hConfFile != NULL);
}

/*=============================================================================
    函 数 名： PackConfStore
    功    能： 将 未进行Pack处理的会议数据 进行Pack处理
	           TConfStore -->
	           TPackConfStore + m_wAliasBufLen字节m_pbyAliasBuf + TTvWallModule(opt) + TVmpModule(opt);
    算法实现： 
    全局变量： 
    参    数： [IN]  TConfStore &tConfStore 待Pack处理的会议数据
               [OUT] TPackConfStore *ptPackConfStore Pack后的输出缓冲
               [OUT] u16 &wPackConfDataLen Pack后的输出缓冲长度 主机序
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/29  4.0			万春雷                  创建
=============================================================================*/
BOOL32 PackConfStore( TConfStore &tConfStore, TPackConfStore *ptPackConfStore, u16 &wPackConfDataLen )
{
	u16 wAliasBufLen = 0;
	u8* pbyAliasBuf  = NULL; //终端别名数组打包缓冲
	u8* pbyModuleBuf = NULL; //电视墙//画面合成模板缓冲
	
	wPackConfDataLen = 0;

	//会议属性信息
	memcpy((s8*)ptPackConfStore, (s8*)&tConfStore, sizeof(TPackConfStore));
	wPackConfDataLen += sizeof(TPackConfStore);
	//终端别名数组打包
	pbyAliasBuf = (u8 *)(ptPackConfStore+1);
	PackTMtAliasArray(tConfStore.m_atMtAlias, tConfStore.m_awMtDialBitRate, 
		              tConfStore.m_byMtNum, (s8*)pbyAliasBuf, wAliasBufLen);
	ptPackConfStore->m_wAliasBufLen = wAliasBufLen;
	wPackConfDataLen += wAliasBufLen;
	pbyModuleBuf = pbyAliasBuf+ptPackConfStore->m_wAliasBufLen;
	//电视墙模板 
	if (ptPackConfStore->m_tConfInfo.GetConfAttrb().IsHasTvWallModule())
	{
		memcpy(pbyModuleBuf, (s8*)&tConfStore.m_tMultiTvWallModule, sizeof(TMultiTvWallModule));
		wPackConfDataLen += sizeof(TMultiTvWallModule);
		pbyModuleBuf += sizeof(TMultiTvWallModule);
	}
	//画面合成模板 
	if (ptPackConfStore->m_tConfInfo.GetConfAttrb().IsHasVmpModule())
	{
		memcpy(pbyModuleBuf, (s8*)&tConfStore.m_atVmpModule, sizeof(TVmpModule));
		wPackConfDataLen += sizeof(TVmpModule);
		pbyModuleBuf += sizeof(TVmpModule);
	}
	//如果为VCS模板，增加的信息
	//                  + 1byte(u8: 0 1  是否配置了高清电视墙)
	//                  +(可选, THDTvWall)
	//                  + 1byte(u8: 会议配置HDU的通道总数)
	//                  + (可选, THduModChnlInfo+...)	
	//                  + 1byte(是否为级联调度)
	//                  +(可选，2byte[u16 网络序，配置打包的总长度]+下级mcu配置[1byte(终端类型)+1byte(别名长度)+xbyte(别名字符串)+2byte(呼叫码率)...)])
	//                  + 1byte(是否支持分组)
	//                  + (可选，2byte(u16 网络序，信息总长)+内容(1byte(组数)+n组[1TVCSGroupInfo＋m个TVCSEntryInfo])
	if (VCS_CONF == tConfStore.m_tConfInfo.GetConfSource())
	{
		u8* pbyVCSInfoExBuf = pbyModuleBuf;  //VCS会议模板增加内容缓冲
		*pbyVCSInfoExBuf++  = (u8)tConfStore.IsHDTWCfg();
		wPackConfDataLen += sizeof(u8);
		if (tConfStore.IsHDTWCfg())
		{
			memcpy(pbyVCSInfoExBuf, &tConfStore.m_tHDTWInfo, sizeof(THDTvWall));
			pbyVCSInfoExBuf  += sizeof(THDTvWall);
			wPackConfDataLen += sizeof(THDTvWall);
		}

		u8 byHduChnlNum = tConfStore.m_tHduModule.GetHduChnlNum();
		*pbyVCSInfoExBuf++ = byHduChnlNum;
		wPackConfDataLen += sizeof(u8);
		if (byHduChnlNum)
		{
			memcpy(pbyVCSInfoExBuf, tConfStore.m_tHduModule.GetHduModuleInfo(), byHduChnlNum * sizeof(THduModChnlInfo));
			pbyVCSInfoExBuf  += byHduChnlNum *sizeof(THduModChnlInfo);
			wPackConfDataLen += byHduChnlNum *sizeof(THduModChnlInfo);
		}

		*pbyVCSInfoExBuf++  = (u8)tConfStore.IsVCSMCUCfg();
		wPackConfDataLen += sizeof(u8);
		if (tConfStore.IsVCSMCUCfg())
		{
			wAliasBufLen = 0;
			pbyVCSInfoExBuf += sizeof(u16);
			wPackConfDataLen += sizeof(u16);
			PackTMtAliasArray(tConfStore.m_tVCSSMCUCfg.m_atSMCUAlias, tConfStore.m_tVCSSMCUCfg.m_awSMCUDialBitRate, 
							  tConfStore.m_tVCSSMCUCfg.m_wSMCUNum, (s8*)pbyVCSInfoExBuf, wAliasBufLen);
			*(u16*)(pbyVCSInfoExBuf - sizeof(u16)) = wAliasBufLen;
			pbyVCSInfoExBuf += wAliasBufLen;
			wPackConfDataLen += wAliasBufLen;
		}
		*pbyVCSInfoExBuf++ = tConfStore.m_byMTPackExist;
		wPackConfDataLen += sizeof(u8);
	}	


	return TRUE;
}

/*=============================================================================
    函 数 名： UnPackConfStore
    功    能： 将 已进行Pack处理的会议数据 进行UnPack处理
	           TPackConfStore + m_wAliasBufLen字节m_pbyAliasBuf + TTvWallModule(opt) + TVmpModule(opt) -->
			   TConfStore;
    算法实现： 
    全局变量： 
    参    数： [IN]  TPackConfStore *ptPackConfStore 待UnPack处理的会议数据
               [OUT] TConfStore &tConfStore  UnPack后的输出缓冲
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/29  4.0			万春雷                  创建
=============================================================================*/
BOOL32 UnPackConfStore( TPackConfStore *ptPackConfStore, TConfStore &tConfStore )
{
	u16 wAliasBufLen = 0;
	u8* pbyAliasBuf  = NULL; //终端别名数组打包缓冲
	u8* pbyModuleBuf = NULL; //电视墙//画面合成模板缓冲
	
	//会议属性信息
	memcpy((s8*)&tConfStore, (s8*)ptPackConfStore, sizeof(TPackConfStore));
	//终端别名数组打包
	pbyAliasBuf  = (u8 *)(ptPackConfStore+1);
	wAliasBufLen = ptPackConfStore->m_wAliasBufLen;
	pbyModuleBuf = pbyAliasBuf+ptPackConfStore->m_wAliasBufLen;
    u8 byMtNum = tConfStore.m_byMtNum;
	UnPackTMtAliasArray((s8*)pbyAliasBuf, wAliasBufLen, &ptPackConfStore->m_tConfInfo, 
		                tConfStore.m_atMtAlias, tConfStore.m_awMtDialBitRate, byMtNum);
    tConfStore.m_byMtNum = byMtNum; 
	//电视墙模板 
	if (ptPackConfStore->m_tConfInfo.GetConfAttrb().IsHasTvWallModule())
	{
		memcpy((s8*)&tConfStore.m_tMultiTvWallModule, pbyModuleBuf, sizeof(TMultiTvWallModule));
		pbyModuleBuf += sizeof(TMultiTvWallModule);
	}
	//画面合成模板 
	if (ptPackConfStore->m_tConfInfo.GetConfAttrb().IsHasVmpModule())
	{
		memcpy((s8*)&tConfStore.m_atVmpModule, pbyModuleBuf, sizeof(TVmpModule));
		pbyModuleBuf += sizeof(TVmpModule);
	}
	
	return TRUE;
}

/*=============================================================================
    函 数 名： UpdateConfStoreModule
    功    能： 更新ConfStore中电视墙和画面合成模板的终端索引的映射
    算法实现： 将所有 OldIdx 修改为 NewIdx
    全局变量： 
    参    数： 注意，该函数只修改索引的映射值，其他部分请自己维护
    返 回 值： BOOL32  是否发生了数据更新
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2007/07/05  4.0			顾振华                  创建
=============================================================================*/
BOOL32 UpdateConfStoreModule( const TConfStore &tConfStore, 
                            u8 byOldIdx, u8 byNewIdx,
                            TMultiTvWallModule &tOutTvwallModule, 
                            TVmpModule &tOutVmpModule )
{
    BOOL32 bReplace = FALSE;
    if (tConfStore.m_tConfInfo.GetConfAttrb().IsHasTvWallModule())
    {
        for(u8 byModuleLp = 0; byModuleLp < tConfStore.m_tMultiTvWallModule.GetTvModuleNum() ; byModuleLp++)
        {
            for (u8 byTvChlLp = 0; byTvChlLp < MAXNUM_PERIEQP_CHNNL; byTvChlLp++)
            {
                if(byOldIdx == tConfStore.m_tMultiTvWallModule.m_atTvWallModule[byModuleLp].m_abyTvWallMember[byTvChlLp] )
                {
                    tOutTvwallModule.m_atTvWallModule[byModuleLp].m_abyTvWallMember[byTvChlLp] = byNewIdx;
                    bReplace = TRUE;
                }
            }
        }
    }
    if (tConfStore.m_tConfInfo.GetConfAttrb().IsHasVmpModule())
    {
        for (u8 byVmpIdx = 0; byVmpIdx < MAXNUM_MPUSVMP_MEMBER; byVmpIdx++)
        {
            if( byOldIdx == tConfStore.m_atVmpModule.m_abyVmpMember[byVmpIdx] )
            {
                tOutVmpModule.m_abyVmpMember[byVmpIdx] = byNewIdx;
                bReplace = TRUE;
            }
        }
    }            

    return bReplace;
}

/*====================================================================
    函数名      : GetMediaStr
    功能        : 获取媒体类型字符串
    算法实现    :（可选项）
    引用全局变量: 无
    输入参数说明: byMediaType 媒体类型
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/13    3.0         胡昌威       创建
====================================================================*/
LPCSTR GetMediaStr( u8 byMediaType )
{
    static char szMediaType[16];
	switch( byMediaType ) 
	{
	case MEDIA_TYPE_H261:
		sprintf( szMediaType, "%s%c", "H.261", 0 );
		break;
	case MEDIA_TYPE_H262:
		sprintf( szMediaType, "%s%c", "H.262", 0 );
		break;
	case MEDIA_TYPE_H263:
		sprintf( szMediaType, "%s%c", "H.263", 0 );
		break;
	case MEDIA_TYPE_H263PLUS:
		sprintf( szMediaType, "%s%c", "H.263+", 0 );
		break;
	case MEDIA_TYPE_H264:
		sprintf( szMediaType, "%s%c", "H.264", 0 );
		break;
	case MEDIA_TYPE_MP4:
		sprintf( szMediaType, "%s%c", "MPEG-4", 0 );
		break;
	case MEDIA_TYPE_PCMA:
		sprintf( szMediaType, "%s%c", "PCMA", 0 );
		break;
	case MEDIA_TYPE_PCMU:
		sprintf( szMediaType, "%s%c", "PCMU", 0 );
		break;
	case MEDIA_TYPE_G722:
		sprintf( szMediaType, "%s%c", "G.722", 0 );
		break;
    case MEDIA_TYPE_G7221C:
        sprintf( szMediaType, "%s%c", "G.7221", 0 );
        break;
    case MEDIA_TYPE_AACLC:
        sprintf( szMediaType, "%s%c", "AAC-LC", 0 );
        break;
    case MEDIA_TYPE_AACLD:
        sprintf( szMediaType, "%s%c", "AAC-LD", 0 );
        break;
	case MEDIA_TYPE_G7231:
		sprintf( szMediaType, "%s%c", "G.7231", 0 );
		break;
	case MEDIA_TYPE_G728:
		sprintf( szMediaType, "%s%c", "G.728", 0 );
		break;
	case MEDIA_TYPE_G729:
		sprintf( szMediaType, "%s%c", "G.729", 0 );
		break;
	case MEDIA_TYPE_MP3:
		sprintf( szMediaType, "%s%c", "MP3", 0 );
		break;
	case MEDIA_TYPE_T120:
		sprintf( szMediaType, "%s%c", "T.120", 0 );
		break;
	case MEDIA_TYPE_H224:
		sprintf( szMediaType, "%s%c", "H.224", 0 );
		break;
	case MEDIA_TYPE_H239:
		sprintf( szMediaType, "%s%c", "H.239", 0 );
		break;
	case MEDIA_TYPE_MMCU:
		sprintf( szMediaType, "%s%c", "MMCU", 0 );
		break;
	default:
		sprintf( szMediaType, "%s(%d)%c", "Unknown", byMediaType, 0 );
		break;
	}
	return szMediaType;
}

/*====================================================================
    函数名      : GetResStr
    功能        : 获取分辨率类型字符串
    算法实现    :（可选项）
    引用全局变量: 无
    输入参数说明: byResolution 分辨率类型
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    08-08-09    5.0         周广程        创建
	3/4/2009    4.6			薛亮		  增加非标分辨率 
====================================================================*/
LPCSTR GetResStr( u8 byResolution )
{
    static char szResType[16];
	
    switch( byResolution )
    {
    //视频格式定义
    case VIDEO_FORMAT_SQCIF:
        sprintf( szResType, "%s%c", "SQCIF", 0 );
        break;
    case VIDEO_FORMAT_QCIF:	
        sprintf( szResType, "%s%c", "QCIF", 0 );
        break;
    case VIDEO_FORMAT_CIF:              
        sprintf( szResType, "%s%c", "CIF", 0 );
        break;
    case VIDEO_FORMAT_2CIF:              
        sprintf( szResType, "%s%c", "2CIF", 0 );
        break;
    case VIDEO_FORMAT_4CIF:         
        sprintf( szResType, "%s%c", "4CIF", 0 );
        break;
    case VIDEO_FORMAT_16CIF:			
        sprintf( szResType, "%s%c", "16CIF", 0 );
        break;
    case VIDEO_FORMAT_AUTO:			
        sprintf( szResType, "%s%c", "AUTO", 0 );
        break;
    
    case VIDEO_FORMAT_SIF:             
        sprintf( szResType, "%s%c", "SIF", 0 );
        break;
    case VIDEO_FORMAT_2SIF:               
        sprintf( szResType, "%s%c", "2SIF", 0 );
        break;
    case VIDEO_FORMAT_4SIF:           
        sprintf( szResType, "%s%c", "4SIF", 0 );
        break;
    
    case VIDEO_FORMAT_VGA:            
        sprintf( szResType, "%s%c", "VGA", 0 );
        break;
    case VIDEO_FORMAT_SVGA:             
        sprintf( szResType, "%s%c", "SVGA", 0 );
        break;
    case VIDEO_FORMAT_XGA:             
        sprintf( szResType, "%s%c", "XGA", 0 );
        break;
    
    //仅用于终端分辨率改变
    case VIDEO_FORMAT_SQCIF_112x96:     
        sprintf( szResType, "%s%c", "SQCIF112x96", 0 );
        break;
    case VIDEO_FORMAT_SQCIF_96x80:     
        sprintf( szResType, "%s%c", "SQCIF96x80", 0 );
        break;
    
    // 高清分辨率
    case VIDEO_FORMAT_W4CIF:            
        sprintf( szResType, "%s%c", "W4CIF", 0 );
        break;
    case VIDEO_FORMAT_HD720:           
        sprintf( szResType, "%s%c", "HD720", 0 );
        break;
    case VIDEO_FORMAT_SXGA:             
        sprintf( szResType, "%s%c", "SXGA", 0 );
        break;
    case VIDEO_FORMAT_UXGA:             
        sprintf( szResType, "%s%c", "UXGA", 0 );
        break;
    case VIDEO_FORMAT_HD1080:             
        sprintf( szResType, "%s%c", "HD1080", 0 );
        break;

	//非标分辨率
	case VIDEO_FORMAT_1440x816:
		sprintf( szResType, "%s%c", "1440x816",0 );
		break;
	case VIDEO_FORMAT_1280x720:
		sprintf( szResType, "%s%c", "1280x720",0 );
		break;
	case VIDEO_FORMAT_960x544:
		sprintf( szResType, "%s%c", "960x544",0 );
		break;
	case VIDEO_FORMAT_640x368:
		sprintf( szResType, "%s%c", "640x368",0 );
		break;
	case VIDEO_FORMAT_480x272:
		sprintf( szResType, "%s%c", "480x272",0 );
		break;

	case VIDEO_FORMAT_720_960x544:
		sprintf( szResType, "%s%c", "960x544",0 );
		break;
	case VIDEO_FORMAT_720_864x480:
		sprintf( szResType, "%s%c", "864x480",0 );
		break;
	case VIDEO_FORMAT_720_640x368:
		sprintf( szResType, "%s%c", "640x368",0 );
		break;
	case VIDEO_FORMAT_720_432x240:
		sprintf( szResType, "%s%c", "432x240",0 );
		break;
	case VIDEO_FORMAT_720_320x192:
		sprintf( szResType, "%s%c", "320x192",0 );
		break;
    default:
        sprintf( szResType, "%s(%d)%c", "Unknown", byResolution, 0 );
		break;
    }

	return szResType;
}

/*=============================================================================
函 数 名： IsHDConf
功    能： 
算法实现： 返回适配意义上的HD，即 >= 4CIF 的H264；（真正的HD须 >= HD720p）
全局变量： 
参    数： const TConfInfo &tConfInfo
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/8/9   4.0		    周广程                  创建
=============================================================================*/
BOOL32 IsHDConf( const TConfInfo &tConfInfo )
{
    if ( tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
         ( IsVidFormatHD( tConfInfo.GetMainVideoFormat() ) ||
           tConfInfo.GetMainVideoFormat() == VIDEO_FORMAT_4CIF ) )
    {
        return TRUE;
    }

    if ( tConfInfo.GetSecVideoMediaType() == MEDIA_TYPE_H264 &&
        ( IsVidFormatHD( tConfInfo.GetSecVideoFormat() ) ||
          tConfInfo.GetSecVideoFormat() == VIDEO_FORMAT_4CIF ) )
    {
        return TRUE;
    }

    return FALSE;
}


/*=============================================================================
函 数 名： IsConfDualEqMV
功    能： 判断会议是否 双流同主视频
算法实现： 
全局变量： 
参    数： const TConfInfo &tConfInfo
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/11/17   4.0		张宝卿                  创建
=============================================================================*/
BOOL32 IsConfDualEqMV( const TConfInfo &tConfInfo )
{
    if (!tConfInfo.GetCapSupport().IsDStreamSupportH239() &&
        tConfInfo.GetMainVideoMediaType() == tConfInfo.GetDStreamMediaType() &&
        tConfInfo.GetMainVideoFormat() == tConfInfo.GetDoubleVideoFormat() )
    {
        return TRUE;
    }
    return FALSE;
}

/*=============================================================================
函 数 名： IsConfDualEqMV
功    能： 判断会议是否 两个双流能力
算法实现： 
全局变量： 
参    数： const TConfInfo &tConfInfo
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/11/17   4.0		张宝卿                  创建
=============================================================================*/
BOOL32 IsConfDoubleDual( const TConfInfo &tConfInfo )
{
    if (tConfInfo.GetCapSupport().IsDStreamSupportH239() &&
        MEDIA_TYPE_H264 == tConfInfo.GetDStreamMediaType() &&
        (g_cMcuVcApp.IsSupportSecDSCap() ||
         MEDIA_TYPE_NULL != tConfInfo.GetCapSupportEx().GetSecDSType()))
    {
        return TRUE;
    }
    return FALSE;
}

/*====================================================================
    函数名      : GetManufactureStr
    功能        : 获取制造厂商字符串
    算法实现    :（可选项）
    引用全局变量: 无
    输入参数说明: byMediaType 媒体类型
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/13    3.0         胡昌威       创建
====================================================================*/
LPCSTR GetManufactureStr( u8 byManufacture )
{
    static char szManu[16];
	switch( byManufacture ) 
	{
	case MT_MANU_KDC:
		sprintf( szManu, "%s%c", "Keda", 0 );
		break;
	case MT_MANU_NETMEETING:
		sprintf( szManu, "%s%c", "Netmeeting", 0 );
		break;
	case MT_MANU_POLYCOM:
		sprintf( szManu, "%s%c", "Polycom", 0 );
		break;
	case MT_MANU_HUAWEI:
		sprintf( szManu, "%s%c", "Huawei", 0 );
		break;
	case MT_MANU_TAIDE:
		sprintf( szManu, "%s%c", "Tandberg", 0 );
		break;
	case MT_MANU_SONY:
		sprintf( szManu, "%s%c", "Sony", 0 );
		break;
	case MT_MANU_VCON:
		sprintf( szManu, "%s%c", "Vcon", 0 );
		break;
	case MT_MANU_RADVISION:
		sprintf( szManu, "%s%c", "Radvision", 0 );
		break;
	case MT_MANU_KDCMCU:
		sprintf( szManu, "%s%c", "KedaMCU", 0 );
		break;
	case MT_MANU_PENTEVIEW:
		sprintf( szManu, "%s%c", "Penteviw", 0 );
		break;
    case MT_MANU_CODIAN:
        sprintf( szManu, "%s%c", "CodianMCU", 0 );
        break;
	case MT_MANU_OTHER:
		sprintf( szManu, "%s%c", "Unknown", 0 );
		break;
	default: sprintf( szManu, "%s%c", "Unknown", 0);break;
	}
	return szManu;	
}

/*====================================================================
    函数名      : AddEventStr
    功能        : 添加消息字符串
    算法实现    : 
    引用全局变量: 无
    输入参数说明: 
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/11    3.0         胡昌威       创建
====================================================================*/
void AddEventStr()
{
	#undef OSPEVENT
	#define OSPEVENT( x, y ) OspAddEventDesc( #x, y )
	
#ifdef _EVMCUMCS_H_
	#undef _EVMCUMCS_H_
	#include "evmcumcs.h"
	#define _EVMCUMCS_H_
#else
	#include "evmcumcs.h"
	#undef _EVMCUMCS_H_
#endif

#ifdef _EVMCUVCS_H_
	#undef _EVMCUVCS_H_
	#include "evmcuvcs.h"
	#define _EVMCUVCS_H_
#else
	#include "evmcuvcs.h"
	#undef _EVMCUVCS_H_
#endif

#ifdef _EV_MCUMT_H_
	#undef _EV_MCUMT_H_
	#include "evmcumt.h"
	#define _EV_MCUMT_H_
#else
	#include "evmcumt.h"
	#undef _EV_MCUMT_H_
#endif
	
#ifdef _EV_MCUEQP_H_
	#undef _EV_MCUEQP_H_
	#include "evmcueqp.h"
	#define _EV_MCUEQP_H_
#else
	#include "evmcueqp.h"
	#undef _EV_MCUEQP_H_
#endif

#ifdef _EV_MCU_H_
	#undef _EV_MCU_H_
	#include "evmcu.h"
	#define _EV_MCU_H_
#else
	#include "evmcu.h"
	#undef _EV_MCU_H_
#endif

#ifdef __EVMP_H_
    #undef __EVMP_H_
    #include "evmp.h"
    #define __EVMP_H_
#else
    #include "evmp.h"
    #undef __EVMP_H_
#endif
    
#ifdef _EV_MCUDCS_H_
	#undef _EV_MCUDCS_H_
	#include "evmcudcs.h"
	#define _EV_MCUDCS_H_
#else
	#include "evmcudcs.h"
	#undef _EV_MCUDCS_H_
#endif

#ifdef _EVMCUTEST_H_
	#undef _EVMCUTEST_H_
	#include "evmcutest.h"
	#define _EVMCUTEST_H_
#else
	#include "evmcutest.h"
	#undef _EVMCUTEST_H_
#endif

#ifdef _EV_MODEM_H_
    #undef _EV_MODEM_H_
    #include "evmodem.h"
    #define _EV_MODEM_H_
#else
    #include "evmodem.h"
    #undef _EV_MODEM_H_
#endif
}

/*====================================================================
    函数名      : StartLocalMixer
    功能        : 启动本地的混音器
    算法实现    : 
    引用全局变量: 无
    输入参数说明: u8 byEqpId 设备Id
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/11    3.0         胡昌威       创建
====================================================================*/
void StartLocalMixer( u8 byEqpId )
{
#if defined(_MINIMCU_)
	TAudioMixerCfg tAudioMixerCfg;

    tAudioMixerCfg.byEqpId      = byEqpId;
    tAudioMixerCfg.byEqpType    = EQP_TYPE_MIXER;
    tAudioMixerCfg.dwLocalIP    = g_cMcuAgent.GetMpcIp();
    tAudioMixerCfg.dwConnectIP  = g_cMcuAgent.GetMpcIp();
    tAudioMixerCfg.wConnectPort = PORT_MCU;
    tAudioMixerCfg.wMcuId       = LOCAL_MCUID;
    
	TEqpMixerInfo tMixParam;
	g_cMcuAgent.GetEqpMixerCfg( byEqpId, &tMixParam );

	TMAPParam tMAPParam[3];
    ReadMAPConfig( tMAPParam, 3 );

	tAudioMixerCfg.byMaxMixGroupCount  = 1; //原来在mcu agent中填的1
    tAudioMixerCfg.wRcvStartPort       = tMixParam.GetEqpRecvPort();
	tAudioMixerCfg.byMaxChannelInGroup = tMixParam.GetMaxChnInGrp();
	tAudioMixerCfg.wMAPCount           = tMixParam.GetUsedMapNum();

	for( u8 byMapId = 0; byMapId < tAudioMixerCfg.wMAPCount; byMapId++ )
	{
		tAudioMixerCfg.m_atMap[byMapId].byMapId   = tMixParam.GetUsedMapId( byMapId );
		tAudioMixerCfg.m_atMap[byMapId].dwPort    = tMAPParam[tAudioMixerCfg.m_atMap[byMapId].byMapId].dwPort;
		tAudioMixerCfg.m_atMap[byMapId].dwCoreSpd = tMAPParam[tAudioMixerCfg.m_atMap[byMapId].byMapId].dwCoreSpd;
		tAudioMixerCfg.m_atMap[byMapId].dwMemSize = tMAPParam[tAudioMixerCfg.m_atMap[byMapId].byMapId].dwMemSize;
		tAudioMixerCfg.m_atMap[byMapId].dwMemSpd  = tMAPParam[tAudioMixerCfg.m_atMap[byMapId].byMapId].dwMemSpd;
	}

    memset( tAudioMixerCfg.achAlias, 0, sizeof(tAudioMixerCfg.achAlias));
    memcpy( tAudioMixerCfg.achAlias, tMixParam.GetAlias(), MAXLEN_EQP_ALIAS );
    
	// sprintf( tAudioMixerCfg.achAlias, "Mix%d%c", byEqpId, 0);
    if( mixInit( &tAudioMixerCfg ) )
    {
        OspPrintf( TRUE, FALSE, "Start local mixer success!\n" );
	
    }
    else
    {
        OspPrintf( TRUE, FALSE, "Start local mixer failure!\n");
    }
#endif

	return;
}

#ifdef  _MINIMCU_
//void painthelp(void);
#endif

/*====================================================================
    函数名      : StartLocalVmp
    功能        : 启动本地的画面合成器
    算法实现    : 
    引用全局变量: 无
    输入参数说明: u8 byEqpId 设备Id
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/11    3.0         胡昌威       创建
====================================================================*/
void StartLocalVmp( u8 byEqpId )
{
#if defined(_MINIMCU_)

	// painthelp();

	TVmpCfg tVmpCfg;
    tVmpCfg.byEqpId      = byEqpId;
    tVmpCfg.byEqpType    = EQP_TYPE_VMP;
    tVmpCfg.dwLocalIP    = g_cMcuAgent.GetMpcIp();
    tVmpCfg.dwConnectIP  = g_cMcuAgent.GetMpcIp();
    tVmpCfg.wConnectPort = PORT_MCU;
    tVmpCfg.wMcuId       = LOCAL_MCUID;

	TMAPParam tMAPParam[3];
    ReadMAPConfig( tMAPParam, 3 );
    
	TEqpVMPInfo tVmpParam;
	g_cMcuAgent.GetEqpVMPCfg( byEqpId, &tVmpParam );

    tVmpCfg.wRcvStartPort = tVmpParam.GetEqpRecvPort();
	tVmpCfg.byDbVid = tVmpParam.GetEncodeNum();

	tVmpCfg.wMAPCount = tVmpParam.GetUsedMapNum();
	for( u8 byMapId = 0; byMapId < tVmpCfg.wMAPCount; byMapId++ )
	{
		tVmpCfg.m_atMap[byMapId].byMapId   = tVmpParam.GetUsedMapId(byMapId);
		tVmpCfg.m_atMap[byMapId].dwPort    = tMAPParam[tVmpCfg.m_atMap[byMapId].byMapId].dwPort;
		tVmpCfg.m_atMap[byMapId].dwCoreSpd = tMAPParam[tVmpCfg.m_atMap[byMapId].byMapId].dwCoreSpd;
		tVmpCfg.m_atMap[byMapId].dwMemSize = tMAPParam[tVmpCfg.m_atMap[byMapId].byMapId].dwMemSize;
		tVmpCfg.m_atMap[byMapId].dwMemSpd  = tMAPParam[tVmpCfg.m_atMap[byMapId].byMapId].dwMemSpd;
	}

    memset( tVmpCfg.achAlias, 0, sizeof(tVmpCfg.achAlias));
    memcpy( tVmpCfg.achAlias, tVmpParam.GetAlias(), MAXLEN_EQP_ALIAS );
    
	// sprintf( tVmpCfg.achAlias, "Vmp%d%c", byEqpId, 0);
    vmpinit( &tVmpCfg );
	
#endif

	return;
}

/*====================================================================
    函数名      : GetPrsTimeSpan
    功能        : 获取Prs时间间隔
    算法实现    : 
    引用全局变量: 无
    输入参数说明: u8 byEqpId 设备Id
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/11    3.0         胡昌威       创建
====================================================================*/
void GetPrsTimeSpan( TPrsTimeSpan *ptPrsTimeSpan )
{
	TEqpPrsInfo tPrsParam;
	memset( &tPrsParam, 0, sizeof(tPrsParam) );

	u8  byEqpId;
	u32 dwEqpIp;
	u8  byEqpType;
	for( byEqpId=1; byEqpId < MAXNUM_MCU_PERIEQP; byEqpId++ )
	{
		if( SUCCESS_AGENT == g_cMcuAgent.GetPeriInfo( byEqpId, &dwEqpIp, &byEqpType ) )
		{
			if( byEqpType == EQP_TYPE_PRS )
			{
				g_cMcuAgent.GetEqpPrsCfg( byEqpId, tPrsParam );
				ptPrsTimeSpan->m_wFirstTimeSpan  = tPrsParam.GetFirstTimeSpan();
				ptPrsTimeSpan->m_wSecondTimeSpan = tPrsParam.GetSecondTimeSpan();
				ptPrsTimeSpan->m_wThirdTimeSpan  = tPrsParam.GetThirdTimeSpan();
				ptPrsTimeSpan->m_wRejectTimeSpan = tPrsParam.GetRejectTimeSpan();
			}
		}
	}

	return;
}

/*====================================================================
    函数名      : StartLocalPrs
    功能        : 启动本地的丢包重传
    算法实现    : 
    引用全局变量: 无
    输入参数说明: u8 byEqpId 设备Id
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/11    3.0         胡昌威       创建
====================================================================*/
void StartLocalPrs( u8 byEqpId )
{
#if defined(_MINIMCU_)
	TPrsCfg tPrsCfg;
	tPrsCfg.byEqpId      = byEqpId;
	tPrsCfg.byEqpType    = EQP_TYPE_PRS;
	tPrsCfg.dwConnectIP  = g_cMcuAgent.GetMpcIp();
	tPrsCfg.dwLocalIP    = g_cMcuAgent.GetMpcIp();;
	tPrsCfg.wConnectPort = PORT_MCU;
	tPrsCfg.wMcuId       = LOCAL_MCUID;
	tPrsCfg.wMAPCount    = 0;
	//sprintf( tPrsCfg.achAlias, "Prs%d%c", byEqpId, 0);

	TEqpPrsInfo tPrsParam;
	g_cMcuAgent.GetEqpPrsCfg( byEqpId, tPrsParam );
	tPrsCfg.m_wFirstTimeSpan  = tPrsParam.GetFirstTimeSpan();
	tPrsCfg.m_wSecondTimeSpan = tPrsParam.GetSecondTimeSpan();
	tPrsCfg.m_wThirdTimeSpan  = tPrsParam.GetThirdTimeSpan();
	tPrsCfg.m_wRejectTimeSpan = tPrsParam.GetRejectTimeSpan();
	tPrsCfg.wRcvStartPort     = tPrsParam.GetEqpRecvPort();
    memset( tPrsCfg.achAlias, 0, sizeof(tPrsCfg.achAlias));
    memcpy( tPrsCfg.achAlias, tPrsParam.GetAlias(), MAXLEN_EQP_ALIAS );


	if( prsinit( &tPrsCfg ) )
	{
		OspPrintf( TRUE, FALSE, "Start local prs success!\n" );
	}
	else
	{
		OspPrintf( TRUE, FALSE, "Start local prs failure!\n" );
	}
#endif

	return;
}

/*====================================================================
    函数名      : StartLocalBas
    功能        : 启动本地的码流适配器
    算法实现    : 
    引用全局变量: 无
    输入参数说明: u8 byEqpId 设备Id
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/11    3.0         胡昌威       创建
====================================================================*/
void StartLocalBas( u8 byEqpId )
{
#if defined(_MINIMCU_)

	TEqpCfg	tEqpCfg;
	tEqpCfg.byEqpId = byEqpId;
	tEqpCfg.byEqpType = EQP_TYPE_BAS;
	tEqpCfg.dwConnectIP = g_cMcuAgent.GetMpcIp();
	tEqpCfg.dwLocalIP = g_cMcuAgent.GetMpcIp();
	tEqpCfg.wConnectPort = PORT_MCU;
	tEqpCfg.wMcuId = LOCAL_MCUID;

	TEqpBasInfo tBasParam;
	g_cMcuAgent.GetEqpBasCfg( byEqpId, &tBasParam );
	tEqpCfg.wRcvStartPort = tBasParam.GetEqpRecvPort();

	TMAPParam tMAPParam[3];
    ReadMAPConfig( tMAPParam, 3 );
		
//#ifdef WIN32
//	tEqpCfg.wMAPCount = 2;
//#else    
	tEqpCfg.wMAPCount = tBasParam.GetUsedMapNum();
	for( u8 byMapId = 0; byMapId < tEqpCfg.wMAPCount; byMapId++ )
	{
		tEqpCfg.m_atMap[byMapId].byMapId   = tBasParam.GetUsedMapId(byMapId);
        if (tEqpCfg.m_atMap[byMapId].byMapId == INVALID_MAP_ID)
        {
            // 如果是255,表示不启动音频适配，不填下面的内容
            continue;
        }
        else
        {
		    tEqpCfg.m_atMap[byMapId].dwPort    = tMAPParam[tEqpCfg.m_atMap[byMapId].byMapId].dwPort;
		    tEqpCfg.m_atMap[byMapId].dwCoreSpd = tMAPParam[tEqpCfg.m_atMap[byMapId].byMapId].dwCoreSpd;
		    tEqpCfg.m_atMap[byMapId].dwMemSize = tMAPParam[tEqpCfg.m_atMap[byMapId].byMapId].dwMemSize;
		    tEqpCfg.m_atMap[byMapId].dwMemSpd  = tMAPParam[tEqpCfg.m_atMap[byMapId].byMapId].dwMemSpd;
        }
	}
//#endif
    
    memset( tEqpCfg.achAlias, 0, sizeof(tEqpCfg.achAlias));
    memcpy( tEqpCfg.achAlias, tBasParam.GetAlias(), MAXLEN_EQP_ALIAS );
	// sprintf(tEqpCfg.achAlias, "Bas%d%c", byEqpId, 0);
	
    if (basInit(&tEqpCfg))
    {
        OspPrintf(TRUE, FALSE, "Start local bas success!\n");
    }
    else
    {
        OspPrintf(TRUE, FALSE, "Start local bas failure!\n");
    }

#endif	

	return;
}

/*====================================================================
    函数名      : StartLocalEqp
    功能        : 启动本地的设备
    算法实现    : 
    引用全局变量: 无
    输入参数说明: u8 byEqpId 设备Id
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/11    3.0         胡昌威       创建
====================================================================*/
void StartLocalEqp( void )
{
	u8 byEqpId;
	u32 dwEqpIp;
	u8 byEqpType;

    // 标记是否启动了外设。用于控制会议数量
    u8 byEqpStart = 0;

	for( byEqpId=1 ;byEqpId < MAXNUM_MCU_PERIEQP ;byEqpId++ )
	{
		if( SUCCESS_AGENT == g_cMcuAgent.GetPeriInfo( byEqpId, &dwEqpIp, &byEqpType ) )
		{
			if( dwEqpIp == g_cMcuAgent.GetMpcIp() )
			{
				switch( byEqpType ) 
				{
				case EQP_TYPE_MIXER:
					StartLocalMixer( byEqpId );
                    byEqpStart ++;
					break;
					
				case EQP_TYPE_VMP:
					StartLocalVmp( byEqpId );
                    byEqpStart ++;
					break;

				case EQP_TYPE_BAS:
					StartLocalBas( byEqpId );
                    byEqpStart ++;
					break;
				
				case EQP_TYPE_PRS:
					StartLocalPrs( byEqpId );
                    byEqpStart ++;
					break;

				default:
					break;
				}
			}
		}
	}
#ifdef _VXWORKS
    if (byEqpStart != 0)
    {
        // 启动每个外设应用大概需要2M的内存
        // 如果在Vx下启动了相应的外设，应该至少减去一个会议数量        
        // g_cMcuVcApp.m_byMaxOngoConfSupported --;
    }
#endif
	
	return;
}

/*====================================================================
    函数名      : StartLocalMp
    功能        : 启动本地的Mp
    算法实现    : 
    引用全局变量: 无
    输入参数说明: 无
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/11    3.0         胡昌威       创建
====================================================================*/
void StartLocalMp( void )
{
	u8  byMpcId  = (g_cMcuAgent.GetMpcBoardId() % MCU_BOARD_MPC == 0) ? 
                    MCU_BOARD_MPC : (g_cMcuAgent.GetMpcBoardId()%16);
	u32 dwMcuIp  = g_cMcuAgent.GetMpcIp();
	u16 wMcuPort = g_cMcuVcApp.GetMcuListenPort();

	TMp tMp;
	tMp.SetMcuId( LOCAL_MCUID );
	tMp.SetMpId( byMpcId );
	tMp.SetAttachMode( 1 );
	tMp.SetDoubleLink( 0 );
	tMp.SetIpAddr( ntohl(dwMcuIp) );

	//交换任务
	if( !mpstart( 0, dwMcuIp, wMcuPort, &tMp ) )
	{
		OspPrintf( TRUE, FALSE, "MCU: Start mp failure!\n" );
		return;
	}
	return;
}

/*====================================================================
    函数名      : StartLocalMtadp
    功能        : 启动本地的Mtadp
    算法实现    : 
    引用全局变量: 无
    输入参数说明: 无
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/11    3.0         胡昌威       创建
====================================================================*/
void StartLocalMtadp( void )
{
	u8  byMpcId  = (g_cMcuAgent.GetMpcBoardId()%16 == 0) ? 16 : (g_cMcuAgent.GetMpcBoardId()%16);
	u32 dwMcuIp  = g_cMcuAgent.GetMpcIp();
	u16 wMcuPort = g_cMcuVcApp.GetMcuListenPort();

	if( 0 == dwMcuIp || inet_addr("127.0.0.1") == dwMcuIp ) 
	{
		OspPrintf( TRUE, FALSE, "LocalIP.%x is invalid reset localip start\n", dwMcuIp );

		u32 tdwIPList[5];  //列举到的当前设置的所有有效ip
		u16 dwIPNum;       //列举到ip 数目

		dwIPNum = OspAddrListGet(tdwIPList, 5);
		if( 0 == dwIPNum )
		{
			OspPrintf( TRUE, FALSE, "LocalIP.%x is invalid reset localip err\n", dwMcuIp );
		}
		for(u16 dwAdapter=0; dwAdapter<dwIPNum; dwAdapter++)
		{
			if((inet_addr( "127.0.0.1" ) != tdwIPList[dwAdapter]))
			{
				OspPrintf( TRUE, FALSE, "LocalIP.%x is invalid reset localip.%x ok\n", tdwIPList[dwAdapter] );

				dwMcuIp = tdwIPList[dwAdapter];
				break;
			}
		}
	}

	TMtAdpConnectParam tConnectParam;
	TMtAdp tMtadp;
	
	tConnectParam.m_dwMcuTcpNode = 0;
	tConnectParam.m_wMcuPort = wMcuPort;
	tConnectParam.SetMcuIpAddr( ntohl(dwMcuIp) );
	// zgc, 2008-04-16, WIN32MCU内置mtadp支持高清
#ifdef WIN32
	tConnectParam.m_bSupportHD = TRUE;
#endif

	tMtadp.SetMtadpId( byMpcId );
	tMtadp.SetAttachMode( 1 );
	tMtadp.SetIpAddr( ntohl(dwMcuIp) );

	//终端适配模块（内嵌方式启动该模块）
	if( !MtAdpStart(&tConnectParam, &tMtadp) )
	{
		OspPrintf( TRUE, FALSE, "MCU: Start mtadp failure!\n" );
		return;
	}
	return;
}

/*====================================================================
    函数名      : GetVmpChlNumByStyle
    功能        : 得到画面合成风格的最大成员数
    算法实现    : 
    引用全局变量: 无
    输入参数说明: byVMPStyle 画面合成风格
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/31    3.0         胡昌威       创建
====================================================================*/
u8   GetVmpChlNumByStyle( u8 byVMPStyle )
{
	u8   byMaxMemNum = 0;

	switch( byVMPStyle ) 
	{
	case VMP_STYLE_ONE:
		byMaxMemNum = 1;
		break;
	case VMP_STYLE_VTWO:
	case VMP_STYLE_HTWO:
		byMaxMemNum = 2;
		break;
	case VMP_STYLE_THREE:
		byMaxMemNum = 3;
		break;
	case VMP_STYLE_FOUR:
		byMaxMemNum = 4;
		break;
	case VMP_STYLE_SIX:
		byMaxMemNum = 6;
		break;
	case VMP_STYLE_EIGHT:
		byMaxMemNum = 8;
		break;
	case VMP_STYLE_NINE:
		byMaxMemNum = 9;
		break;
	case VMP_STYLE_TEN:
		byMaxMemNum = 10;
		break;
	case VMP_STYLE_THIRTEEN:
		byMaxMemNum = 13;
		break;
	case VMP_STYLE_SIXTEEN:
		byMaxMemNum = 16;
		break;
	case VMP_STYLE_SPECFOUR:
		 byMaxMemNum = 4;
		break;
	case VMP_STYLE_SEVEN:
		 byMaxMemNum = 7;
		 break;
	case VMP_STYLE_TWENTY:// xliang [2/19/2009] 
		 byMaxMemNum = 20;
		 break;
	default:
		break;
	}

	return byMaxMemNum;
}


/*====================================================================
    函数名      : PackTMtAliasArray
    功能        : 打包终端别名数组（带速率信息）
    算法实现    : 
    引用全局变量: 无
    输入参数说明: TMtAlias *ptMtAlias 输入别名数组
				  u8 &byMtNum 输入别名个数
				  char *pszBuf 输出Buf 
	              u16 wBufLen  输出Buf长度
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/31    3.0         胡昌威       创建
====================================================================*/
BOOL32 PackTMtAliasArray( TMtAlias *ptMtAlias, u16* pawMtDialRate, u8 byMtNum, 
					      char* const pszBuf, u16 &wBufLen )
{
	TMtAlias *ptMtTempAlias = ptMtAlias;
	char *pszTemp = pszBuf;
	u8 byMtCount = 0;

	if( pszBuf == NULL || ptMtAlias == NULL || byMtNum == 0 || byMtNum > MAXNUM_CONF_MT )
	{
        wBufLen = 0;
		return FALSE;
	}

    while( byMtCount < byMtNum )
	{
		if( !ptMtTempAlias->IsNull() )
		{
			*pszTemp++ = ptMtTempAlias->m_AliasType;
			if( ptMtTempAlias->m_AliasType == mtAliasTypeTransportAddress )
			{
				*pszTemp++ = sizeof(TTransportAddr);
				memcpy( pszTemp, &ptMtTempAlias->m_tTransportAddr, sizeof(TTransportAddr) );
				pszTemp = pszTemp + sizeof(TTransportAddr);
			}
			else
			{
				*pszTemp++ = strlen( ptMtTempAlias->m_achAlias );
				memcpy( pszTemp, ptMtTempAlias->m_achAlias, strlen( ptMtTempAlias->m_achAlias ) );
				pszTemp = pszTemp + strlen( ptMtTempAlias->m_achAlias );
			}

			//设置呼叫速率
			*((u16*)pszTemp) = htons(*pawMtDialRate++);
			pszTemp += sizeof(u16);
		}

		ptMtTempAlias++;
		byMtCount++;
	}

	wBufLen = pszTemp - pszBuf;

	return TRUE;
}

/*====================================================================
    函数名      : UnPackTMtAliasArray
    功能        : 解包终端别名数组（带速率信息）
    算法实现    : 
    引用全局变量: 无
    输入参数说明: char *pszBuf 输入Buf 
	              u16 wBufLen  输入Buf长度
				   const TConfInfo *ptConfInfo,  输入的会议信息
				  TMtAlias *ptMtAlias 输出别名数组
				  u8*pawMtDialRate    输出终端呼叫速率数组（主机序）
				  u8 &byMtNum 输出别名个数
    返回值说明  : 无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/03/31    3.0         胡昌威       创建
====================================================================*/
BOOL32 UnPackTMtAliasArray(const char *pszBuf, u16 wBufLen, const TConfInfo *ptConfInfo, 
						  TMtAlias *ptMtAlias, u16* pawMtDialRate, u8 &byMtNum )
{
	u8 byMtCount = 0;
    const char *pszTemp = pszBuf;
	u8 byAliasLen;

	if( NULL == pszBuf || NULL == ptConfInfo || 
		NULL == ptMtAlias || NULL == pawMtDialRate )
	{
		return FALSE;
	}
 
	while( pszTemp - pszBuf < wBufLen )
	{
		ptMtAlias->m_AliasType = *pszTemp++;//得到别名类型
	    if( ptMtAlias->m_AliasType != mtAliasTypeTransportAddress && 
			ptMtAlias->m_AliasType != mtAliasTypeE164 && 
			ptMtAlias->m_AliasType != mtAliasTypeH323ID && 
			ptMtAlias->m_AliasType != mtAliasTypeH320ID &&
			// PU类型
			ptMtAlias->m_AliasType != puAliasTypeIPPlusAlias) //mtAliasTypeH320Alias类型不用作呼叫信息，所以不判断
		{
            byMtNum = byMtCount;
			return FALSE;
		}

		byAliasLen = *pszTemp++;//得到别名长度
		if( byAliasLen > MAXLEN_ALIAS )
		{
            byMtNum = byMtCount;
			return FALSE;
		}

		if( ptMtAlias->m_AliasType == mtAliasTypeTransportAddress )
		{
			if( byAliasLen != sizeof(TTransportAddr) )
			{
                byMtNum = byMtCount;
				return FALSE;
			}
			memcpy( &ptMtAlias->m_tTransportAddr, pszTemp, byAliasLen );
		}
		else
		{
            memset( ptMtAlias->m_achAlias, 0, sizeof( ptMtAlias->m_achAlias) );
			memcpy( ptMtAlias->m_achAlias, pszTemp, byAliasLen );
		}
		pszTemp = pszTemp + byAliasLen;

		*pawMtDialRate = *((u16*)pszTemp);//得到呼叫速率信息;
		*pawMtDialRate = ntohs(*pawMtDialRate);
	
		if (*pawMtDialRate == 0 || *pawMtDialRate > ptConfInfo->GetBitRate())
		{
			*pawMtDialRate = ptConfInfo->GetBitRate();			
		}

		pawMtDialRate++;
		pszTemp += sizeof(u16);

		ptMtAlias++;
		byMtCount++;
	}

	byMtNum = byMtCount;
	return TRUE;
}

/*====================================================================
    函数名      : GetAudioBitrate
    功能        : 得到音频的编码比特率
    算法实现    : 
    引用全局变量: 无
    输入参数说明: byAudioType 音频类型
    返回值说明  : 音频的编码比特率
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/08/06    3.0         胡昌威       创建
====================================================================*/
u16 GetAudioBitrate(u8 byAudioType)
{
	u16 wBitrate = 0;
	switch(byAudioType)
	{
	case MEDIA_TYPE_PCMA:
		wBitrate = AUDIO_BITRATE_G711A;
		break;
	case MEDIA_TYPE_PCMU:
		wBitrate = AUDIO_BITRATE_G711U;
		break;
	case MEDIA_TYPE_G722:
		wBitrate = AUDIO_BITRATE_G722;
		break;
	case MEDIA_TYPE_G7231:
		wBitrate = AUDIO_BITRATE_G7231;
		break;
	case MEDIA_TYPE_G728:
		wBitrate = AUDIO_BITRATE_G728;
		break;
	case MEDIA_TYPE_G729:
		wBitrate = AUDIO_BITRATE_G729;
		break;
	case MEDIA_TYPE_MP3:
		wBitrate = AUDIO_BITRATE_MP3;
		break;
//	case MEDIA_TYPE_G7221:
//		wBitrate = AUDIO_BITRATE_MP3;
//		break;
    case MEDIA_TYPE_G7221C:
        wBitrate = AUDIO_BITRATE_G7221C;
		break;
    case MEDIA_TYPE_AACLC:
    case MEDIA_TYPE_AACLD:
        wBitrate = AUDIO_BITRATE_AAC;
        break;
	default:
		break;
	}
	return wBitrate;
}

/*=============================================================================
    函 数 名： AddMcInfo
    功    能： 将指定级联mcu信息增加置列表中维护，已增加的不再重复
    算法实现： 
    全局变量： 
    参    数： u8 byMcuId
    返 回 值： TConfMcInfo* 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/02/19  3.6			万春雷                  创建
=============================================================================*/
TConfMcInfo* TConfOtherMcTable::AddMcInfo(u8 byMcuId)
{
	s32 nIdle = MAXNUM_SUB_MCU;
	for(s32 nLoop=0; nLoop<MAXNUM_SUB_MCU; nLoop++)
	{
		if(m_atConfOtherMcInfo[nLoop].m_byMcuId == byMcuId)
		{
			return &(m_atConfOtherMcInfo[nLoop]);
		}
		if(nIdle == MAXNUM_SUB_MCU && m_atConfOtherMcInfo[nLoop].IsNull())
		{
			nIdle = nLoop;
		}
	}
	if(nIdle == MAXNUM_SUB_MCU)
	{
		return NULL;
	}
	m_atConfOtherMcInfo[nIdle].SetNull();
	m_atConfOtherMcInfo[nIdle].m_byMcuId = byMcuId;
	m_atConfOtherMcInfo[nIdle].m_tSpyMt.SetNull();
	m_atConfOtherMcInfo[nIdle].m_dwSpyViewId = OspTickGet();
	m_atConfOtherMcInfo[nIdle].m_dwSpyVideoId = OspTickGet()+10;
	
    ///FIXME: guzh [7/4/2007] 权宜之计：如果选择了不请求级联列表，会导致没有该下级MCU的状态
    // 这里就先把该MCU本身的状态设置好，以使得发言、选看、查媒体源等级联操作能够得以进行 
    TMt tMt;
    tMt.SetMcuId(byMcuId);
    tMt.SetMtId(0);
    m_atConfOtherMcInfo[nIdle].m_atMtExt[0].SetMt(tMt);
    m_atConfOtherMcInfo[nIdle].m_atMtStatus[0].SetMt(tMt);
    
	return &(m_atConfOtherMcInfo[nIdle]);
}

/*=============================================================================
    函 数 名： RemoveMcInfo
    功    能： 将指定级联mcu信息移除出列表，清空相应信息
    算法实现： 
    全局变量： 
    参    数：
    返 回 值： BOOL32
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/02/19  3.6			万春雷                  创建
=============================================================================*/
BOOL32 TConfOtherMcTable::RemoveMcInfo(u8 byMcuId)
{
	for(s32 nLoop=0; nLoop<MAXNUM_SUB_MCU; nLoop++)
	{
		if(m_atConfOtherMcInfo[nLoop].m_byMcuId == byMcuId)
		{
			memset( &m_atConfOtherMcInfo[nLoop], 0, sizeof(TConfMcInfo) );
			return TRUE;
		}
	}

	return FALSE;
}

/*=============================================================================
    函 数 名： GetMcInfo
    功    能： 获取指定的级联mcu信息
    算法实现： 
    全局变量： 
    参    数： u8 byMcuId
    返 回 值： TConfMcInfo* 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/02/19  3.6			万春雷                  创建
=============================================================================*/
TConfMcInfo* TConfOtherMcTable::GetMcInfo(u8 byMcuId)
{
	for(s32 nLoop=0; nLoop<MAXNUM_SUB_MCU; nLoop++)
	{
		if(m_atConfOtherMcInfo[nLoop].m_byMcuId == byMcuId)
		{
			return &(m_atConfOtherMcInfo[nLoop]);
		}
	}
	return NULL;
}

/*=============================================================================
函 数 名： SetMtInMixing
功    能： 设置某下级mcu某终端在混音
算法实现： 
全局变量： 
参    数： TMt tMt
返 回 值： void  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/7/28  4.0			许世林                  创建
=============================================================================*/
BOOL32  TConfOtherMcTable::SetMtInMixing(TMt tMt)
{
    TConfMcInfo *ptMcInfo = GetMcInfo(tMt.GetMcuId());
    if (NULL != ptMcInfo)
    {
        TMcMtStatus *ptStatus = ptMcInfo->GetMtStatus(tMt);
        if (NULL != ptStatus)
        {
            ptStatus->SetInMixing(TRUE);
            return TRUE;
        }
    }
    
    return FALSE;
}

/*=============================================================================
函 数 名： IsMtInMixing
功    能： 某下级mcu某终端是否在混音
算法实现： 
全局变量： 
参    数： TMt tMt
返 回 值： BOOL32  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/7/27  4.0			许世林                  创建
=============================================================================*/
BOOL32  TConfOtherMcTable::IsMtInMixing( TMt tMt)
{       
    TConfMcInfo *ptMcInfo = GetMcInfo(tMt.GetMcuId());
    if (NULL != ptMcInfo)
    {
        TMcMtStatus *ptStatus = ptMcInfo->GetMtStatus(tMt);
        if (NULL != ptStatus && ptStatus->IsInMixing())
        {
            return TRUE;
        }        
    }

    return FALSE;
}

/*=============================================================================
函 数 名： ClearMtInMixing
功    能： 清除某mcu下终端混音状态
算法实现： 
全局变量： 
参    数： TMcu tMcu
返 回 值： void  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/7/28  4.0			许世林                  创建
=============================================================================*/
BOOL32   TConfOtherMcTable::ClearMtInMixing(TMcu tMcu)
{
    TMt tMt;
    tMt.SetMcuId(tMcu.GetMcuId());
    TConfMcInfo *ptMcInfo = GetMcInfo(tMcu.GetMcuId());
    if (NULL != ptMcInfo)
    {
        for (u8 byMtId = 1; byMtId <= MAXNUM_CONF_MT; byMtId++)
        {
            tMt.SetMtId(byMtId);
            TMcMtStatus *ptStatus = ptMcInfo->GetMtStatus(tMt);
            if (NULL != ptStatus)
            {
                ptStatus->SetInMixing(FALSE);
            }
        } 

        return TRUE;
    }

    return FALSE;
}

/*====================================================================
    函数名      ：TableMemoryFree
    功能        ：释放动态申请的内存
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/29    3.5         胡昌威         创建
====================================================================*/
void TableMemoryFree( s8 **ppMem, u32 dwEntryNum )
{
    if( ppMem == NULL )
    {
        return;
    }
	
    for(u32 dwLoop = 0; dwLoop < dwEntryNum; dwLoop++ )
    {
        if( ppMem[dwLoop] != NULL )
        {
            free( ppMem[dwLoop] );
        }
    }
	
    free( ppMem );
	
	return;
}

/*====================================================================
    函数名      ：GetMAPMemSpeed
    功能        ：得到map内存主频
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：u32
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/29    3.5         胡昌威         创建
====================================================================*/
u32 GetMAPMemSpeed(u32 dwCoreSpeed)
{
	u16    wLoop=0;
	static u32 adwCoreSpdToMemSpdTable[][2] = 
	{
		{68,68},  {74,74},  {81,81},  {88,88},  {95,95},  {101,101},{108,108}, {115,115},{122,122},
		{128,128},{135,68}, {149,74}, {162,81}, {176,88}, {189,95}, {203,101}, {216,108},{230,115},
		{243,122},{257,128},{270,135},{284,142},{297,99}, {311,104},{324,108}, {338,113},{351,117},
		{365,122},{378,126},{392,131},{405,135},{419,140},{432,144},{446,149}, {459,153},{473,158},
		{486,162},{500,167},{0,0}
	};
	
	while( adwCoreSpdToMemSpdTable[wLoop][0] != 0)
	{
		if( adwCoreSpdToMemSpdTable[wLoop][0] == dwCoreSpeed )
		{
			return adwCoreSpdToMemSpdTable[wLoop][1];
		}
		wLoop++;
	}
	return 0;
}

/*====================================================================
    函数名      ：ReadMAPConfig
    功能        ：得到map配置信息
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：BOOL32
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/01/29    3.5         胡昌威         创建
====================================================================*/
BOOL32 ReadMAPConfig( TMAPParam tMAPParam[], u8 byMapCount )
{
    s8    achProfileName[32];
    s8*   *lpszTable;
    s8    chSeps[] = " \t";       
    s8    *pchToken;
    u32   dwLoop;
    BOOL32  bResult = TRUE;
    u32   dwMemEntryNum;
 
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, "mapcfg.ini");
   
    bResult = GetRegKeyInt( achProfileName, "MAPTable", STR_ENTRY_NUM, 
                            0, (s32*)&dwMemEntryNum );
	if( bResult == FALSE ) 
	{
		memset( tMAPParam, 0, byMapCount*sizeof(TMAPParam) );
		return FALSE;
	}
	if( dwMemEntryNum != byMapCount )
	{
		memset( tMAPParam, 0, byMapCount*sizeof(TMAPParam) );
		return FALSE;
	}

	//alloc memory
    lpszTable = (s8**)malloc( dwMemEntryNum * sizeof( s8* ) );
    for( dwLoop = 0; dwLoop < dwMemEntryNum; dwLoop++ )
    {
        lpszTable[dwLoop] = (s8*)malloc( MAX_VALUE_LEN + 1 );
    }

	//get the map table
    bResult = GetRegKeyStringTable( achProfileName, "MAPTable",
                                    "fail", lpszTable, &dwMemEntryNum, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )
    {
		memset( tMAPParam, 0, byMapCount*sizeof(TMAPParam) );
		return FALSE;
    }
	if( dwMemEntryNum != byMapCount )
	{
		memset( tMAPParam, 0, byMapCount*sizeof(TMAPParam) );
		return FALSE;
	}

    // analyze entry strings
    for( dwLoop = 0; dwLoop < dwMemEntryNum; dwLoop++ )
    {
        pchToken = strtok( lpszTable[dwLoop], chSeps );
        if( pchToken == NULL )
        {
            bResult = FALSE;
        }
        else
        {
            tMAPParam[dwLoop].byMapId = atoi( pchToken );
        }

        // MAP处理器主频
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            bResult = FALSE;
        }
        else
        {
            tMAPParam[dwLoop].dwCoreSpd = atoi( pchToken );
        }

		tMAPParam[dwLoop].dwMemSpd = GetMAPMemSpeed( tMAPParam[dwLoop].dwCoreSpd );

        // MAP处理器内存容量
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            bResult = FALSE;
        }
        else
        {
            tMAPParam[dwLoop].dwMemSize = atoi( pchToken );
        }
        // MAP处理器视频端口
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            bResult = FALSE;
        }
        else
        {
            tMAPParam[dwLoop].dwPort = atoi( pchToken );
        }
    }

    // free memory
    TableMemoryFree( lpszTable, dwMemEntryNum );
    
    return bResult;
}


u8 *GetRandomKey()
{
	static u8 abyKey[MAXLEN_KEY];
	u32 dwTemp;
	memset( abyKey, 0, sizeof( abyKey ) );
    srand(time(NULL));
	//rand
	dwTemp = rand();
	memcpy( abyKey, &dwTemp, sizeof( u32 ) );
    //tick
	dwTemp = OspTickGet();
	memcpy(abyKey + 4, &dwTemp, sizeof(u32));
	
	//tick1
	dwTemp = rand();
	memcpy(abyKey + 8, &dwTemp, sizeof(u32));

	//tick2
	dwTemp = rand();
	memcpy(abyKey + 12, &dwTemp, sizeof(u32));

    return abyKey;
}

/*=============================================================================
    函 数 名： GetActivePayload
    功    能： 
    算法实现： 前向纠错的通道 统一返回 MEDIA_TYPE_FEC,非前向纠错的码流保持最初逻辑
    全局变量： 
    参    数： u32 dwIP   主机序IP地址
    返 回 值： char * 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2007/10/11  4.0			张宝卿                增加 基于会议的FEC类型返回动态载荷
=============================================================================*/
u8 GetActivePayload(const TConfInfo &tConfInfo, u8 byRealPayloadType)
{
    u8 byAPayload = MEDIA_TYPE_NULL;
    u8 byFECType = FECTYPE_NONE;

    switch(byRealPayloadType)
    {
    case MEDIA_TYPE_PCMA:
    case MEDIA_TYPE_PCMU:
    case MEDIA_TYPE_G721:
    case MEDIA_TYPE_G722:
    case MEDIA_TYPE_G7231:
    case MEDIA_TYPE_G728:
    case MEDIA_TYPE_G729:
    case MEDIA_TYPE_MP3:
    case MEDIA_TYPE_G7221C:
    case MEDIA_TYPE_G7221:
    case MEDIA_TYPE_ADPCM:
    case MEDIA_TYPE_AACLC:
    case MEDIA_TYPE_AACLD:

        byFECType = tConfInfo.GetCapSupportEx().GetAudioFECType();
    	break;

    case MEDIA_TYPE_H261:
    case MEDIA_TYPE_H262:
    case MEDIA_TYPE_H263:
    case MEDIA_TYPE_MP4:
    case MEDIA_TYPE_H263PLUS:
    case MEDIA_TYPE_H264:

        byFECType = tConfInfo.GetCapSupportEx().GetVideoFECType();
        break;

    case MEDIA_TYPE_H224:
    case MEDIA_TYPE_MMCU:
        break;
        
    default:
        OspPrintf( TRUE, FALSE, "[GetActivePayload] unexpected media type.%d, ignore it\n", byRealPayloadType );
        break;
    }

    if ( FECTYPE_NONE != byFECType && MEDIA_TYPE_H224 != byRealPayloadType )
    {
        byAPayload = MEDIA_TYPE_FEC;
    }
    else
    {
        BOOL32 bNewActivePT = CONF_ENCRYPTMODE_NONE != tConfInfo.GetConfAttrb().GetEncryptMode() ? TRUE : FALSE;
            
        // guzh [9/26/2007] H264/G7221.C(不加密)/MP4/MP3/ADPCM(不加密) 动态载荷和原载荷一致
	    switch(byRealPayloadType)
	    {
	    case MEDIA_TYPE_PCMA:   byAPayload = ACTIVE_TYPE_PCMA;  break;
	    case MEDIA_TYPE_PCMU:   byAPayload = ACTIVE_TYPE_PCMU;  break;
	    case MEDIA_TYPE_G721:   byAPayload = ACTIVE_TYPE_G721;  break;
	    case MEDIA_TYPE_G722:   byAPayload = ACTIVE_TYPE_G722;  break;
	    case MEDIA_TYPE_G7231:  byAPayload = ACTIVE_TYPE_G7231; break;
	    case MEDIA_TYPE_G728:   byAPayload = ACTIVE_TYPE_G728;  break;
	    case MEDIA_TYPE_G729:   byAPayload = ACTIVE_TYPE_G729;  break;
	    case MEDIA_TYPE_H261:   byAPayload = ACTIVE_TYPE_H261;  break;
	    case MEDIA_TYPE_H262:   byAPayload = ACTIVE_TYPE_H262;  break;
	    case MEDIA_TYPE_H263:   byAPayload = ACTIVE_TYPE_H263;  break;
        case MEDIA_TYPE_ADPCM:  byAPayload = bNewActivePT ? ACTIVE_TYPE_ADPCM : byRealPayloadType;  break;
        case MEDIA_TYPE_G7221C: byAPayload = bNewActivePT ? ACTIVE_TYPE_G7221C : byRealPayloadType; break;
        case MEDIA_TYPE_AACLC:   byAPayload = MEDIA_TYPE_AACLC;  break;
        case MEDIA_TYPE_AACLD:   byAPayload = MEDIA_TYPE_AACLD;  break;
	    default:                byAPayload = byRealPayloadType; break;
	    }
    }

	return byAPayload;
}

// 判断源端分辨率是否大于目的端分辨率,大于则返回TRUE,小于等于则返回FALSE
BOOL32 IsResG(u8 bySrcRes, u8 byDstRes)
{
    if (VIDEO_FORMAT_AUTO == bySrcRes ||
        VIDEO_FORMAT_AUTO == byDstRes)
    {
        return FALSE;
    }

    if (!IsDSResMatched(bySrcRes, byDstRes))
    {
        return FALSE;
    }

    const s32 s_anRes[] = { VIDEO_FORMAT_SQCIF_96x80,
                            VIDEO_FORMAT_SQCIF_112x96,
                            VIDEO_FORMAT_SIF,
                            VIDEO_FORMAT_2SIF,
                            VIDEO_FORMAT_CIF,
                            VIDEO_FORMAT_2CIF,
                            VIDEO_FORMAT_VGA,
                            VIDEO_FORMAT_4SIF,
                            VIDEO_FORMAT_4CIF,
                            VIDEO_FORMAT_SVGA,
                            VIDEO_FORMAT_XGA,
                            VIDEO_FORMAT_HD720,
                            VIDEO_FORMAT_SXGA, 
                            VIDEO_FORMAT_16CIF,
                            VIDEO_FORMAT_HD1080              
                           };

    u8 bySrcResIdx = 0xff;
    u8 byDstResIdx = 0xff;
    u8 byLop = 0;

    // 记录在分辨率数组中的位置，用于比较分辨率大小
    for ( byLop = 0; byLop < sizeof(s_anRes)/sizeof(s32); byLop++ )
    {
        if ( s_anRes[byLop] == bySrcRes )
        {
            if ( bySrcResIdx == 0xff )
            {
                bySrcResIdx = byLop;    
            }
        }
        if ( s_anRes[byLop] == byDstRes )
        {
            if ( byDstResIdx == 0xff )
            {
                byDstResIdx = byLop;
            }
        }
        if ( bySrcResIdx != 0xff && byDstResIdx != 0xff )
        {
            break;
        }
    }

    if (bySrcResIdx == 0xff || byDstResIdx == 0xff)
    {
        OspPrintf(TRUE, FALSE, "[IsResGE] unexpected res <src.%d, dst.%d>\n", bySrcRes, byDstRes);
        return FALSE;
    }

    if (bySrcResIdx <= byDstResIdx)
    {
        return FALSE;
    }
    return TRUE;
}
// 判断源端分辨率是否大于等于目的端分辨率,大于等于则返回TRUE,小于则返回FALSE
BOOL32 IsResGE(u8 bySrcRes, u8 byDstRes)
{
	if (IsResG(bySrcRes, byDstRes) || bySrcRes == byDstRes)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL32 IsDSResMatched(u8 bySrcRes, u8 byDstRes)
{
	BOOL32 bMatched = FALSE;
	if (IsResPresentation(bySrcRes) && IsResPresentation(byDstRes))
	{
		bMatched = TRUE;
	}
	else if (IsResLive(bySrcRes) && IsResLive(byDstRes))
	{
		bMatched = TRUE;
	}
	return bMatched;
}

BOOL32 IsResPresentation(u8 byRes)
{
	BOOL32 bPresentation = FALSE;

	switch(byRes)
	{
	case VIDEO_FORMAT_VGA:
	case VIDEO_FORMAT_SVGA:
	case VIDEO_FORMAT_XGA:
	case VIDEO_FORMAT_SXGA:
	case VIDEO_FORMAT_UXGA:
		bPresentation = TRUE;
		break;
	default:
		break;
	}
	return bPresentation;
}

BOOL32 IsResLive(u8 byRes)
{
	BOOL32 bLive = FALSE;
	
	switch(byRes)
	{
	case VIDEO_FORMAT_CIF:
	case VIDEO_FORMAT_2CIF:
	case VIDEO_FORMAT_4CIF:
	case VIDEO_FORMAT_16CIF:
	
	case VIDEO_FORMAT_SIF:
	case VIDEO_FORMAT_2SIF:
	case VIDEO_FORMAT_4SIF:

	case VIDEO_FORMAT_W4CIF:
	case VIDEO_FORMAT_HD720:
	case VIDEO_FORMAT_HD1080:
		bLive = TRUE;
		break;
	//Auto不解析
	case VIDEO_FORMAT_AUTO:
	//DS不考虑SQCIF
	case VIDEO_FORMAT_SQCIF_112x96:
	case VIDEO_FORMAT_SQCIF_96x80:
		break;
	default:
		break;
	}
	return bLive;
}

u8 GetChnType(const TEqp &tEqp, u8 byChIdx)
{
    u8 byChnType = MAU_CHN_NONE;
    
    if (tEqp.IsNull())
    {
        //OspPrintf(TRUE, FALSE, "[GetChnType] tEqp.IsNull()!\n");
        return byChnType;
    }

    if (!g_cMcuVcApp.IsPeriEqpValid(tEqp.GetEqpId()))
    {
        OspPrintf(TRUE, FALSE, "[GetChnType] Eqp.%d EqpValid = 0!\n", tEqp.GetEqpId());
        return byChnType;
    }
    /*
    if (!g_cMcuVcApp.IsPeriEqpConnected(tEqp.GetEqpId()))
    {
        OspPrintf(TRUE, FALSE, "[GetChnType] Eqp.%d Connected = 0!\n", tEqp.GetEqpId());
        return byChnType;
    }*/
    if (!g_cMcuAgent.IsEqpBasHD(tEqp.GetEqpId()))
    {
        OspPrintf(TRUE, FALSE, "[GetChnType] Eqp.%d IsEqpBasHD = 0!\n", tEqp.GetEqpId());
        return byChnType;
    }
    
    TPeriEqpStatus tStatus;
    if (!g_cMcuVcApp.GetPeriEqpStatus(tEqp.GetEqpId(), &tStatus))
    {
        OspPrintf(TRUE, FALSE, "[GetChnType] GetPeriEqpStatus failed!\n");
        return byChnType;
    }
    
    u8 byMauType = tStatus.m_tStatus.tHdBas.GetEqpType();
    if (TYPE_MAU_NORMAL == byMauType)
    {
        if (0 == byChIdx)
        {
            byChnType = MAU_CHN_NORMAL;
        }
        else if (1 == byChIdx)
        {
            byChnType = MAU_CHN_VGA;
        }
    }
    else if (TYPE_MAU_H263PLUS == byMauType)
    {
        if (0 == byChIdx)
        {
            byChnType = MAU_CHN_263TO264;
        }
        else if (1 == byChIdx)
        {
            byChnType = MAU_CHN_264TO263;
        }
    }
    else if(TYPE_MPU == byMauType)
    {
        if (byChIdx < 4)
        {
            byChnType = MPU_CHN_NORMAL;
        }
    }

    if (MAU_CHN_NONE == byChnType)
    {
        OspPrintf(TRUE, FALSE, "[GetChnType] failed for eqp<%d, %d>!\n", tEqp.GetEqpId(), byChIdx);
    }
    return byChnType;
}


/*=============================================================================
    函 数 名： StrOfIP
    功    能： 把整型IP地址转成字符串
    算法实现： 
    全局变量： 
    参    数： u32 dwIP   主机序IP地址
    返 回 值： char * 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/02/28  3.6			万春雷                  创建
=============================================================================*/
char * StrOfIP( u32 dwIP )
{
    dwIP = htonl(dwIP);
	static char strIP[17];  
	u8 *p = (u8 *)&dwIP;
	sprintf(strIP,"%d.%d.%d.%d%c",p[0],p[1],p[2],p[3],0);
	return strIP;
}


/*=============================================================================
函 数 名： GetAddrBookPath
功    能： 获取地址簿文件路径
算法实现： 
全局变量： 
参    数： void
返 回 值： s8 * 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/3/23  4.0			许世林                  创建
=============================================================================*/
s8 * GetAddrBookPath(void)
{
    static s8 szPath[128];
    sprintf(szPath, "%s/%s", DIR_DATA, MCUADDRFILENAME);
    return szPath;
}

/*=============================================================================
函 数 名： GetMcuType
功    能： 获取MCU产品(软件)类型
算法实现： 
全局变量： 
参    数： void
返 回 值： u8
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/03/08  4.0			顾振华                  创建
=============================================================================*/
u8 GetMcuPdtType(void)
{
    // 按照如下的方式写是为了方便WindowsMCU模拟其他类型
#ifdef _MINIMCU_
    #ifdef _MCU8000C_
        return MCU_TYPE_KDV8000C;
    #else
        if ( !g_cMcuAgent.IsMcu8000BHD() )
        {
            return MCU_TYPE_KDV8000B;
        }
        else
        {
            return MCU_TYPE_KDV8000B_HD;
        }        
    #endif
#else
    #ifdef WIN32
        return MCU_TYPE_WIN32;
    #else
        return MCU_TYPE_KDV8000;
    #endif
#endif
}

/*=============================================================================
函 数 名： GetMtLeftReasonStr
功    能： 获取MCU终端离开的原因
算法实现： 
全局变量： 
参    数： void
返 回 值： u8
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/03/08  4.0			顾振华                  创建
=============================================================================*/
const s8* GetMtLeftReasonStr(u8 byReason)
{
    static s8* gs_szLeftReason[] = {"Unknown", "Exception", "Normal", "RTD", "DRQ", "TypeUnmatch", "Busy",
                                    "Reject", "Unreachable", "Local", "BusyExt" };

    if ( byReason >= MTLEFT_REASON_EXCEPT && byReason <= MTLEFT_REASON_BUSYEXT )
    {
        return gs_szLeftReason[byReason];
    }
    else
    {
        return gs_szLeftReason[0];
    }  
}

/*=============================================================================
函 数 名： GetMcuVersion
功    能： 获取MCU子系统版本号
算法实现： 
全局变量： 
参    数： void
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/09/03  4.0			顾振华                  创建
=============================================================================*/
const s8* GetMcuVersion()
{
    static s8 gs_VersionBuf[128] = {0};
    if ( strlen(gs_VersionBuf) != 0 )
    {
        return gs_VersionBuf;
    }
        
    strcpy(gs_VersionBuf, KDV_MCU_PREFIX);

    /*
    struct stat buf;
    s32 nResult = stat( APPNAME, &buf );

    if( nResult != 0 )
    {
        return gs_VersionBuf;
    }
    else
    {
        // 获取文件生成时间，即为编译时间
        TKdvTime tTime;
        tTime.SetTime(&buf.st_ctime);
        s8 achBuf[16] = {0};
        sprintf(achBuf, ".%04d%02d%02d", tTime.GetYear(), tTime.GetMonth(), tTime.GetDay());
        sprintf(gs_VersionBuf, VERSION_MCU_PREFIX, achBuf);        
    } 
    */
    
    s8 achMon[16] = {0};
    u32 byDay = 0;
    u32 byMonth = 0;
    u32 wYear = 0;
    static s8 achFullDate[24] = {0};
    
    s8 achDate[32] = {0};
    sprintf(achDate, "%s", __DATE__);
    StrUpper(achDate);
    
    sscanf(achDate, "%s %d %d", achMon, &byDay, &wYear );
    
    if ( 0 == strcmp( achMon, "JAN") )		 
        byMonth = 1;
    else if ( 0 == strcmp( achMon, "FEB") )
        byMonth = 2;
    else if ( 0 == strcmp( achMon, "MAR") )
        byMonth = 3;
    else if ( 0 == strcmp( achMon, "APR") )		 
        byMonth = 4;
    else if ( 0 == strcmp( achMon, "MAY") )
        byMonth = 5;
    else if ( 0 == strcmp( achMon, "JUN") )
        byMonth = 6;
    else if ( 0 == strcmp( achMon, "JUL") )
        byMonth = 7;
    else if ( 0 == strcmp( achMon, "AUG") )
        byMonth = 8;
    else if ( 0 == strcmp( achMon, "SEP") )		 
        byMonth = 9;
    else if ( 0 == strcmp( achMon, "OCT") )
        byMonth = 10;
    else if ( 0 == strcmp( achMon, "NOV") )
        byMonth = 11;
    else if ( 0 == strcmp( achMon, "DEC") )
        byMonth = 12;
    else
        byMonth = 0;
    
    if ( byMonth != 0 )
    {
        sprintf(achFullDate, "%04d%02d%02d", wYear, byMonth, byDay);
        sprintf(gs_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
    }
    else
    {
        // for debug information
        sprintf(gs_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
    }
    
    return gs_VersionBuf;
}


//mcu dcs调试打印接口
BOOL32 g_bPrtDcsMsg = FALSE;
API void pdcsmsg( void )
{
	g_bPrtDcsMsg = TRUE;
}

API void npdcsmsg( void )
{
	g_bPrtDcsMsg = FALSE;
}

//mcs调试打印接口
BOOL32 g_bPrintMcsMsg = FALSE;
API void pmcsmsg( void )
{
    g_bPrintMcsMsg = TRUE;
}

API void npmcsmsg( void )
{
    g_bPrintMcsMsg = FALSE;
}

//vcs调试打印接口
BOOL32 g_bPrintVcsMsg = FALSE;
API void pvcsmsg( void )
{
    g_bPrintVcsMsg = TRUE;
	//同时把mcs的消息打应打开
    g_bPrintMcsMsg = TRUE;
}

API void npvcsmsg( void )
{
    g_bPrintVcsMsg = FALSE;
	//同时把mcs的消息打应打开
    g_bPrintMcsMsg = FALSE;
}

//eqp调试打印接口
BOOL32 g_bPrintEqpMsg = FALSE;
API void peqpmsg( void )
{
    g_bPrintEqpMsg = TRUE;
}

API void npeqpmsg( void )
{
    g_bPrintEqpMsg = FALSE;
}

//mmcu调试接口
BOOL32 g_bpMMcuMsg = FALSE;
API void pmmcumsg(void)
{
    g_bpMMcuMsg = TRUE;
}
API void npmmcumsg(void)
{
    g_bpMMcuMsg = FALSE;
}

//mt
BOOL32 g_bPMt2Msg = FALSE;
API void pmt2msg( void )
{
    g_bPMt2Msg = TRUE;
}

API void npmt2msg( void )
{
    g_bPMt2Msg = FALSE;
}

//mt call
BOOL32 g_bPrintCallMsg = FALSE;
API void pcallmsg( void )
{
    g_bPrintCallMsg = TRUE;
}

API void npcallmsg( void )
{
    g_bPrintCallMsg = FALSE;
}

//mp
BOOL32 g_bpMpMgrMsg = FALSE;
API void pmpmgrmsg(void)
{
    g_bpMpMgrMsg = TRUE;
}
API void npmpmgrmsg(void)
{
    g_bpMpMgrMsg = FALSE;
}

//cfg
BOOL32 g_bPrintCfgMsg = FALSE;
API void pcfgmsg( void )
{
    g_bPrintCfgMsg = TRUE;
}

API void npcfgmsg( void )
{
    g_bPrintCfgMsg = FALSE;
}

//mt
BOOL32 g_bPrintMtMsg = FALSE;
API void pmtmsg( void )
{
    g_bPrintMtMsg = TRUE;
}

API void npmtmsg( void )
{
    g_bPrintMtMsg = FALSE;
}

//guard
BOOL32 g_bPrintGdMsg = FALSE;
API void pgdmsg( void )
{
    g_bPrintGdMsg = TRUE;
}

API void npgdmsg( void )
{
    g_bPrintGdMsg = FALSE;
}

//satelite
BOOL32 g_bPrintSatMsg = FALSE;
API void psatmsg( void )
{
    g_bPrintSatMsg = TRUE;
}

API void npsatmsg( void )
{
    g_bPrintSatMsg = FALSE;
}

BOOL32 g_bPrintNPlusMsg = FALSE;

API void pnplusmsg( void )
{
    g_bPrintNPlusMsg = TRUE;
}

API void npnplusmsg( void )
{
    g_bPrintNPlusMsg = FALSE;
}

API void ppfmmsg( void )
{
    g_bPrintPfmMsg = TRUE;
}

API void nppfmmsg( void )
{
    g_bPrintPfmMsg = FALSE;
}

API void sconftotemp( s8* psConfName )
{
	if ( psConfName == NULL )
	{
		OspPrintf( TRUE, FALSE, "ERROR: The name is null!\n" );
		return;
	}

	CConfId cConfId = g_cMcuVcApp.GetConfIdByName( (LPCSTR)psConfName, FALSE );
	if( cConfId.IsNull() )
	{
		OspPrintf( TRUE, FALSE, "ERROR: The conference %s is not exist! The name maybe error!\n", psConfName );
		return;
	}
	u8 byConfIdx = g_cMcuVcApp.GetConfIdx( cConfId );
	CServMsg cServMsg;
	cServMsg.SetMsgBody( &byConfIdx, sizeof(u8) );
	OspPost(MAKEIID( AID_MCU_VC, CInstance::DAEMON ), MCS_MCU_SAVECONFTOTEMPLATE_REQ, 
				cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	return;
}

API void resetlogin( void )
{
	TLoginInfo tLoginInfo;
	tLoginInfo.SetUser("admin");
	tLoginInfo.SetPwd("admin");
	if ( SUCCESS_AGENT != g_cMcuAgent.SetLoginInfo(&tLoginInfo) )
	{
		OspPrintf( TRUE, FALSE, "Reset login info failed!\n" );
	}
	return;
}

/*=============================================================================
函 数 名： NPlusLog
功    能： N+1备份调试打印接口
算法实现： 
全局变量： 
参    数：  s8 * fmt
           ...
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/11/21  4.0			许世林                  创建
=============================================================================*/
void NPlusLog( s8 * fmt, ... )
{
    s8  achBuf[255];
    va_list argptr;    
    if( g_bPrintNPlusMsg )
    {
        s32 nPrefix = sprintf( achBuf, "[McuNPlus]:" );
        va_start( argptr, fmt );    
        vsprintf( achBuf + nPrefix, fmt, argptr );   
        OspPrintf(TRUE, FALSE, achBuf); 
        va_end(argptr); 
    }
}

/*=============================================================================
函 数 名： McsLog
功    能： mcs调试打印接口
算法实现： 
全局变量： 
参    数：  s8 * fmt
           ...
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/3/23  4.0			许世林                  创建
=============================================================================*/
void McsLog( s8 * fmt, ... )
{
    s8  achBuf[255];
    va_list argptr;    
    if( g_bPrintMcsMsg )
    {
        s32 nPrefix = sprintf( achBuf, "[McuMcs]:" );
        va_start( argptr, fmt );    
        vsprintf( achBuf + nPrefix, fmt, argptr );   
        OspPrintf(TRUE, FALSE, achBuf); 
        va_end(argptr); 
    }
}

/*=============================================================================
  函 数 名： DcsLog
  功    能： dcs信息打印
  算法实现： 
  全局变量： 
  参    数： s8 * pszFmt ...
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/13    4.0			张宝卿                  创建
=============================================================================*/
void DcsLog( s8 * fmt, ... )
{
    s8  achBuf[255];
    va_list argptr;	
    if( g_bPrtDcsMsg )
    {
        s32 nPrefix = sprintf( achBuf, "[MCUDCS]:" );
        va_start( argptr, fmt );    
        vsprintf( achBuf + nPrefix, fmt, argptr );   
        OspPrintf(TRUE, FALSE, achBuf); 
        va_end(argptr); 
    }
}

/*====================================================================
    函数名      ：EqpLog
    功能        ：外设打印
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/09/05    4.0         libo          创建
====================================================================*/
void EqpLog(s8 * fmt, ...)
{   
	s8 achBuf[255];
    va_list argptr;
    if (g_bPrintEqpMsg)
    {
        s32 nPrefix = sprintf( achBuf, "[Eqp]: " );
        va_start(argptr, fmt);    
        vsprintf(achBuf + nPrefix, fmt, argptr);   
        OspPrintf(TRUE, FALSE, achBuf);
        va_end(argptr); 	
    }
}

/*=============================================================================
    函 数 名： MMcuLog
    功    能： 级联信息打印
    算法实现： 
    全局变量： 
    参    数：char * fmt
               ...
    返 回 值： void  
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/4/14   3.6			万春雷                  创建
=============================================================================*/
void  MMcuLog(s8 * fmt,...)
{
	s8 achBuf[255];
    va_list argptr;
	if( g_bpMMcuMsg )
	{
		s32 nPrefix = sprintf( achBuf, "[MMCU]:" );
		va_start(argptr, fmt);    
		vsprintf( achBuf+nPrefix, fmt, argptr );   
		OspPrintf(TRUE, FALSE, achBuf); 
		va_end(argptr); 
	}
}

/*====================================================================
    函数名      ：CallLog
    功能        ：呼叫信息打印
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/17    1.0         胡昌威        创建
====================================================================*/	
void  CallLog(s8 * fmt,...)
{
	s8 achBuf[255];
    va_list argptr;
	if( g_bPrintCallMsg )
	{
		s32 nPrefix = sprintf( achBuf, "[CALL]:" );
		va_start(argptr, fmt);    
		vsprintf( achBuf+nPrefix, fmt, argptr );   
		OspPrintf(TRUE, FALSE, achBuf); 
		va_end(argptr); 
	}
}

/*====================================================================
    函数名      ：Mt2Log
    功能        ：与终端通讯的一些附加信息打印
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/17    1.0         胡昌威        创建
====================================================================*/	
void  Mt2Log(s8 * fmt,...)
{
	s8 achBuf[255];
    va_list argptr;
	if( g_bPMt2Msg )
	{
		s32 nPrefix = sprintf( achBuf, "[MTCOM]:" );
		va_start(argptr, fmt);    
		vsprintf( achBuf+nPrefix, fmt, argptr );   
		OspPrintf(TRUE, FALSE, achBuf); 
		va_end(argptr); 
	}
}

/*=============================================================================
函 数 名： MtLog
功    能： 终端适配打印
算法实现： 
全局变量： 
参    数： s8 * fmt
           ...
返 回 值： void  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/3/24  4.0			许世林                  创建
=============================================================================*/
void  MtLog(s8 * fmt,...)
{
    s8 achBuf[255];
    va_list argptr;
    if( g_bPrintMtMsg )
    {
        s32 nPrefix = sprintf( achBuf, "[McuMtadp]:" );
        va_start(argptr, fmt);    
        vsprintf( achBuf+nPrefix, fmt, argptr );   
        OspPrintf(TRUE, FALSE, achBuf); 
        va_end(argptr); 
    }
}

/*====================================================================
    函数名      ：MpManagerLog
    功能        ：Mp信息打印
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/07/17    1.0         胡昌威        创建
====================================================================*/	
void  MpManagerLog(s8 * fmt,...)
{
	s8 achBuf[255];
    va_list argptr;
    if (g_bpMpMgrMsg)
    {
        s32 nPrefix = sprintf( achBuf, "[MpManager]:" );
        va_start(argptr, fmt);
        vsprintf(achBuf+nPrefix,fmt,argptr);          
        OspPrintf(TRUE, FALSE, achBuf);
        va_end(argptr);
    }
}

/*====================================================================
  函 数 名：CfgLog
  功    能：配置信息打印
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void  
====================================================================*/	
void CfgLog(s8 * fmt,...)
{
	s8 szBuf[255];
	va_list argptr;
	if (g_bPrintCfgMsg)
	{
		s32 nPrefix = sprintf( szBuf, "[MCUCFG]:" );
		va_start(argptr, fmt);
		vsprintf( szBuf + nPrefix, fmt, argptr );
		OspPrintf(TRUE, FALSE, szBuf);
		va_end(argptr);
	}
}

/*=============================================================================
函 数 名： GuardLog
功    能： mcu守卫打印
算法实现： 
全局变量： 
参    数： s8 * fmt
           ...
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/3/24  4.0			许世林                  创建
=============================================================================*/
void GuardLog(s8 * fmt,...)
{
    s8 szBuf[255];
    va_list argptr;
    if (g_bPrintGdMsg)
    {
        s32 nPrefix = sprintf( szBuf, "[MCUGuard]:" );
        va_start(argptr, fmt);
        vsprintf( szBuf + nPrefix, fmt, argptr );
        OspPrintf(TRUE, FALSE, szBuf);
        va_end(argptr);
    }    
}

/*=============================================================================
函 数 名： VcsLog
功    能： vcs调试打印接口
算法实现： 
全局变量： 
参    数：  s8 * fmt
           ...
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/11/20  		    付秀华                  创建
=============================================================================*/
void VcsLog( s8 * fmt, ... )
{
    s8  achBuf[255];
    va_list argptr;    
    if( g_bPrintVcsMsg )
    {
        s32 nPrefix = sprintf( achBuf, "[McuVcs]:" );
        va_start( argptr, fmt );    
        vsprintf( achBuf + nPrefix, fmt, argptr );   
        OspPrintf(TRUE, FALSE, achBuf); 
        va_end(argptr); 
    }
}


/*====================================================================
    函数名      ：SatLog
    功能        ：卫星会议信息打印
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	09/08/28    4.6         张宝卿        创建
====================================================================*/	
void  SatLog(s8 * fmt,...)
{
	s8 achBuf[255];
    va_list argptr;
	if( g_bPrintSatMsg )
	{
		s32 nPrefix = sprintf( achBuf, "[Sat]: " );
		va_start(argptr, fmt);    
		vsprintf( achBuf+nPrefix, fmt, argptr );   
		OspPrintf(TRUE, FALSE, achBuf); 
		va_end(argptr); 
	}
}


u8 GetResByWH( u16 wWidth, u16 wHeight )
{
    if ( wWidth == 128 && wHeight == 96 )
    {
        return VIDEO_FORMAT_SQCIF;
    }
    if ( wWidth == 176 && wHeight == 144 )
    {
        return VIDEO_FORMAT_QCIF;
    }
    if ( wWidth == 352 && wHeight == 288 )
    {
        return VIDEO_FORMAT_CIF;
    }
    if ( wWidth == 352 && wHeight == 576 )
    {
        return VIDEO_FORMAT_2CIF;
    }
    if ( (wWidth == 704 && wHeight == 576) ||
         (wWidth == 720 && wHeight == 576) ||
         (wWidth == 720 && wHeight == 480))
    {
        return VIDEO_FORMAT_4CIF;
    }
    if ( wWidth == 1408 && wHeight == 1152 )
    {
        return VIDEO_FORMAT_16CIF;
    }
    if ( wWidth == 352 && wHeight == 240 )
    {
        return VIDEO_FORMAT_SIF;
    }
    if ( wWidth == 352 && wHeight == 480 )
    {
        return VIDEO_FORMAT_2SIF;
    }
    if ( wWidth == 704 && wHeight == 480 )
    {
        return VIDEO_FORMAT_4SIF;
    }
    if ( wWidth == 640 && wHeight == 480 )
    {
        return VIDEO_FORMAT_VGA;
    }
    if ((wWidth == 800 && wHeight == 600) ||
        (wWidth == 800 && wHeight == 608))
    {
        return VIDEO_FORMAT_SVGA;
    }
    if ( wWidth == 1024 && wHeight == 768 )
    {
        return VIDEO_FORMAT_XGA;
    }
    if ( wWidth == 112 && wHeight == 96 )
    {
        return VIDEO_FORMAT_SQCIF_112x96;
    }
    if ( wWidth == 96 && wHeight == 80 )
    {
        return VIDEO_FORMAT_SQCIF_96x80;
    }
    if ( wWidth == 1024 && wHeight == 576 )
    {
        return VIDEO_FORMAT_W4CIF;
    }
    if ( wWidth == 1280 && wHeight == 720 )
    {
        return VIDEO_FORMAT_HD720;
    }
    if ( wWidth == 1280 && wHeight == 1024 )
    {
        return VIDEO_FORMAT_SXGA;
    }
    if ( wWidth == 1600 && wHeight == 1200 )
    {
        return VIDEO_FORMAT_UXGA;
    }
    if ( ( wWidth == 1920 && wHeight == 1088 ) ||
         ( wWidth == 1920 && wHeight == 1080 ) ||
         ( wWidth == 1920 && wHeight == 544 ) )
    {
        return VIDEO_FORMAT_HD1080;
    }
    return 0;
}

void GetWHByRes(u8 byRes, u16 &wWidth, u16 &wHeight)
{
    wWidth = 0;
    wHeight = 0;
    
    switch(byRes)
    {
    case VIDEO_FORMAT_SQCIF_112x96:
        wWidth = 112;
        wHeight = 96;
        break;
        
    case VIDEO_FORMAT_SQCIF_96x80:
        wWidth = 96;
        wHeight = 80;
        break;
        
    case VIDEO_FORMAT_SQCIF:
        wWidth = 128;
        wHeight = 96;
        break;
        
    case VIDEO_FORMAT_QCIF:
        wWidth = 176;
        wHeight = 144;
        break;
        
    case VIDEO_FORMAT_CIF:
        wWidth = 352;
        wHeight = 288;
        break;
        
    case VIDEO_FORMAT_2CIF:
        wWidth = 352;
        wHeight = 576;
        break;
        
    case VIDEO_FORMAT_4CIF:
        
        //基于尽可能播放考虑，这里取大值
        wWidth = 704;
        wHeight = 576;
        break;
        
    case VIDEO_FORMAT_16CIF:
        wWidth = 1048;
        wHeight = 1152;
        break;
        
    case VIDEO_FORMAT_AUTO:
		// 对于auto分辨率解析为cif
        wWidth = 352;
        wHeight = 288;      
        break;
        
    case VIDEO_FORMAT_SIF:
        wWidth = 352;
        wHeight = 240;
        break;
        
    case VIDEO_FORMAT_2SIF:
        wWidth = 352;
        wHeight = 480;
        break;
        
    case VIDEO_FORMAT_4SIF:
        wWidth = 704;
        wHeight = 480;
        break;
        
    case VIDEO_FORMAT_VGA:
        wWidth = 640;
        wHeight = 480;
        break;
        
    case VIDEO_FORMAT_SVGA:
        wWidth = 800;
        wHeight = 608;   //600-->608
        break;
        
    case VIDEO_FORMAT_XGA:
        wWidth = 1024;
        wHeight = 768;
        break;

    case VIDEO_FORMAT_W4CIF:
        wWidth = 1024;
        wHeight = 576;
        break;

    case VIDEO_FORMAT_HD720:
        wWidth = 1280;
        wHeight = 720;
        break;

    case VIDEO_FORMAT_SXGA:
        wWidth = 1280;
        wHeight = 1024;
        break;

    case VIDEO_FORMAT_UXGA:
        wWidth = 1600;
        wHeight = 1200;
        break;

    case VIDEO_FORMAT_HD1080:
        wWidth = 1920;
        wHeight = 1088;
        break;
		//非标分辨率（1080p底图）
	case VIDEO_FORMAT_1440x816:
		wWidth = 1440;
		wHeight = 816;
        break;

	case VIDEO_FORMAT_1280x720:
		wWidth = 1280;
		wHeight = 720;
		break;
	
	case VIDEO_FORMAT_960x544:
		wWidth = 960;
		wHeight = 544;
		break;

	case VIDEO_FORMAT_640x368:
		wWidth = 640;
		wHeight =368;
		break;

	case VIDEO_FORMAT_480x272:
		wWidth = 480;
		wHeight =272;
		break;
	//非标分辨率（720p底图）
	case VIDEO_FORMAT_720_960x544:
		wWidth = 960;
		wHeight =544;
		break;

	case VIDEO_FORMAT_720_864x480:
		wWidth = 864;
		wHeight =480;
		break;

	case VIDEO_FORMAT_720_640x368:
		wWidth = 640;
		wHeight =368;
		break;

	case VIDEO_FORMAT_720_432x240:
		wWidth = 432;
		wHeight =240;
		break;

	case VIDEO_FORMAT_720_320x192:
		wWidth = 320;
		wHeight =192;
		break;
    default:
        break;
    }
    if ( 0 == wHeight || 0 == wWidth )
    {
        OspPrintf( TRUE, FALSE, "[GetWHByRes] unexpected res.%d, ignore it\n", byRes );
    }
    return;
}
/*==============================================================================
函数名    :  IsResCmpNoMeaning
功能      :  
算法实现  :  vga类的分辨率和非vga类的分辨率比较被视为无意义
参数说明  :  u8 bySrcRes	[in]
			 u8 byDstRes	[in]
返回值说明:  BOOL32
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-2-20					薛亮							create
==============================================================================*/
BOOL32 IsResCmpNoMeaning (u8 bySrcRes, u8 byDstRes)
{
	if ( ( (bySrcRes == VIDEO_FORMAT_VGA  || bySrcRes == VIDEO_FORMAT_SVGA 
		|| bySrcRes == VIDEO_FORMAT_XGA || bySrcRes == VIDEO_FORMAT_SXGA 
		|| bySrcRes == VIDEO_FORMAT_UXGA)
		&& (byDstRes != VIDEO_FORMAT_VGA  || byDstRes != VIDEO_FORMAT_SVGA 
		|| byDstRes != VIDEO_FORMAT_XGA || byDstRes != VIDEO_FORMAT_SXGA 
		|| byDstRes != VIDEO_FORMAT_UXGA) ) //src 为VGA类，dst为非vga类
		
		|| ( (bySrcRes != VIDEO_FORMAT_VGA  || bySrcRes != VIDEO_FORMAT_SVGA 
		|| bySrcRes != VIDEO_FORMAT_XGA || bySrcRes != VIDEO_FORMAT_SXGA 
		|| bySrcRes != VIDEO_FORMAT_UXGA)
		&& (byDstRes == VIDEO_FORMAT_VGA  || byDstRes == VIDEO_FORMAT_SVGA 
		|| byDstRes == VIDEO_FORMAT_XGA || byDstRes == VIDEO_FORMAT_SXGA 
		|| byDstRes == VIDEO_FORMAT_UXGA) ) //src 为非VGA类，dst为vga类
		)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*==============================================================================
函数名    :  ResWHCmp
功能      :  
算法实现  :  
参数说明  :  u8 bySrcRes	[in]
			 u8 byDstRes    [in]
返回值说明:  s32 
             宽高均小返回-2；宽小高大返回-1；宽高均相等返回0；宽大高小返回1；宽高均大返回2；
			 两者比较无意义返回3
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-2-20                  薛亮                            创建
==============================================================================*/
s32 ResWHCmp(u8 bySrcRes, u8 byDstRes)
{
	//VGA类的分辨率和非VGA类的分辨率比较没有意义
	if(IsResCmpNoMeaning(bySrcRes,byDstRes))
	{
		return 3;
	}

	u16 wSrcWidth = 0;
	u16 wSrcHeight = 0;
	u16 wDstWidth = 0;
	u16 wDstHeight = 0;
	GetWHByRes(bySrcRes, wSrcWidth, wSrcHeight);
	GetWHByRes(byDstRes, wDstWidth, wDstHeight);

	//特殊调整
	//VIDEO_FORMAT_480x272 ——>比较时统一扩充到480 x 288
	wSrcHeight = (wSrcHeight == 272) ? 288 : wSrcHeight;
	wDstHeight = (wSrcHeight == 272) ? 288 : wDstHeight;
	
	//前者均不大于后者
	if ( (wSrcWidth == wDstWidth) && (wSrcHeight == wDstHeight) )
	{
		return 0;  //宽高均等  (equal in width and height)
	}
	else if( ( wSrcWidth <= wDstWidth ) && (wSrcHeight <= wDstHeight) )
	{
		return -2; //宽高均小，宽等高小，宽小高等 (none bigger in both width and height)
	}
	//前者至少有一项大于后者
	else if ( (wSrcWidth <= wDstWidth) && (wSrcHeight >= wDstHeight) )
	{
		return -1; //宽小高大，宽等高大  (only bigger in height)
	}
	else if ( (wSrcWidth >= wDstWidth) && (wSrcHeight <= wDstHeight) )
	{
		return 1;  //宽大高小，宽大高等  (only bigger in width)
	}
	else 
	{
		return 2; //宽高均大			 (bigger both in width and height)
	}
}

/*==============================================================================
函数名    :  GetVmpOutChnnlByRes
功能      :  获取VMP对应某分辨率的输出通道
算法实现  :  
参数说明  :  u8 byRes			[in]
			 u8 byVmpId			[in]
			 u8 byMediaType		[in] 媒体格式，默认值：MEDIA_TYPE_H264
			 
返回值说明:  u8 VMP输出通道号
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-5-07                  薛亮                            创建
==============================================================================*/
u8 GetVmpOutChnnlByRes(u8 byRes, u8 byVmpId, u8 byMediaType)
{
	u8 byChnnlIdx = ~0;

	TPeriEqpStatus tPeriEqpStatus;	
	g_cMcuVcApp.GetPeriEqpStatus( byVmpId , &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;	
	u8 byBoardVer	= tPeriEqpStatus.m_tStatus.tVmp.m_byBoardVer;

    //zbq[07/27/2007] 不区分A/B板，强制1080I时给720p端取proximal
    if (g_cMcuVcApp.IsSVmpOutput1080i())
    {
        if (VIDEO_FORMAT_HD720 == byRes)
        {
            byRes = VIDEO_FORMAT_4CIF;
        }
    }

	if (VMP == byVmpSubType)		//标清VMP只有一路输出
	{
		byChnnlIdx = 0;				
	}
	else if (MPU_SVMP == byVmpSubType)
	{
		if (MPU_BOARD_A128 == byBoardVer)
		{
			switch (byRes)
			{
			case VIDEO_FORMAT_HD1080:
				byChnnlIdx = 0;
				break;
				
			case VIDEO_FORMAT_HD720:
				byChnnlIdx = 0;
				break;
				
			case VIDEO_FORMAT_4CIF:
				byChnnlIdx = 2;
				break;
				
			case VIDEO_FORMAT_CIF:
				byChnnlIdx = 2;
				break;
			default:
				break;
			}
		}
		else if(MPU_BOARD_B256 == byBoardVer)
		{
			if( byMediaType == MEDIA_TYPE_H264)
			{
				switch (byRes)
				{
				case VIDEO_FORMAT_HD1080:
					byChnnlIdx = 0;
					break;
					
				case VIDEO_FORMAT_HD720:
					byChnnlIdx = 1;
					break;
					
				case VIDEO_FORMAT_4CIF:
					byChnnlIdx = 3;
					break;
					
				case VIDEO_FORMAT_CIF:
					byChnnlIdx = 2;
					break;
					
				default:
					break; 
				}
			}
			else
			{
				byChnnlIdx = 2;
			}
		}
		else
		{
			//Do nothing
			OspPrintf(TRUE, FALSE, "[GetVmpOutChnnlByRes] Unexpected mpu board version!\n");
		}
	}
	else if (EVPU_SVMP == byVmpSubType)
	{
		//后续扩展
	}
	

	return byChnnlIdx;
}

/*==============================================================================
函数名    :  GetResByVmpOutChnnl
功能      :  获取VMP对应某分辨率的输出通道
算法实现  :  
参数说明  :  u8			byChnnl		[in]
			 TConfInfo	tConfInfo	[in]
			 u8			bySubVmpType[in]
			 u8			byMediaType	[out]
			 
返回值说明:  u8			byRes
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-5-07                  薛亮                            创建
==============================================================================*/
u8 GetResByVmpOutChnnl(u8 &byMediaType, u8 byChnnl, const TConfInfo &tConfInfo, u8 byVmpId)
{
	u8 byRes = 0;

	TPeriEqpStatus tPeriEqpStatus;	
	g_cMcuVcApp.GetPeriEqpStatus( byVmpId , &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;	
	u8 byBoardVer	= tPeriEqpStatus.m_tStatus.tVmp.m_byBoardVer;

	//old vmp
	if( byVmpSubType == VMP)
	{
		byMediaType = tConfInfo.GetMainVideoMediaType();	//取会议的主格式
		return byRes;										//这种情况下，该值返回不具备任何参考价值
	}

	//MPU
	if (MPU_BOARD_A128 == byBoardVer)
	{
		//MPU 2出版本(A板)  0    1   2
		/*----------------------------
		1080				1080	cif	
		1080/720			720		cif
		1080/4cif			1080	cif
		1080/cif			1080	cif
		720					720		cif
		720/4cif			720		cif
		720/cif				720		cif
		4cif						cif
		4cif/cif					cif
		cif							cif
		xxx/other			*       other
		other				1080	other (HDU 看0路的，也能看到较好的效果)
		------------------------------*/
		if (MEDIA_TYPE_H264 == tConfInfo.GetMainVideoMediaType())
		{
			byMediaType = MEDIA_TYPE_H264;
			if( 2 == byChnnl )
			{
				if(tConfInfo.GetSecVideoMediaType() == MEDIA_TYPE_H263 || tConfInfo.GetSecVideoMediaType() == MEDIA_TYPE_MP4 )
				{
					byMediaType = tConfInfo.GetSecVideoMediaType();//出other
					byRes =  VIDEO_FORMAT_CIF;
				}
				else
				{
					byRes =  VIDEO_FORMAT_CIF;
				}
			}
			else if( 0 == byChnnl )
			{
				if( VIDEO_FORMAT_HD720 == tConfInfo.GetMainVideoFormat()
					|| ( VIDEO_FORMAT_HD1080 == tConfInfo.GetMainVideoFormat()//1080,720的会议第1出出720
					&& tConfInfo.GetConfAttrbEx().IsResEx720() )
					)
				{
					byRes = VIDEO_FORMAT_HD720;
				}
				else if( VIDEO_FORMAT_HD1080 == tConfInfo.GetMainVideoFormat() ||
					VIDEO_FORMAT_CIF == tConfInfo.GetMainVideoFormat() )
				{
					byRes = VIDEO_FORMAT_HD1080;
				}
				else
				{
					//do nothing
				}
			}
			else
			{
				//do nothing
			}
		}
		else //h263会议和mpeg4会议
		{
			byMediaType = ( 0 == byChnnl )? MEDIA_TYPE_H264: tConfInfo.GetMainVideoMediaType();
			byRes = ( 0 == byChnnl )? VIDEO_FORMAT_HD1080 : VIDEO_FORMAT_CIF;  
		}	
	}
	else
	{
		//MPU 4出版本 （B板）
		if( 0 == byChnnl )	//0出
		{
			byMediaType = MEDIA_TYPE_H264;
			byRes = VIDEO_FORMAT_HD1080;
		}
		else if (1 == byChnnl)	//1出
		{
			byMediaType = MEDIA_TYPE_H264;
			byRes = VIDEO_FORMAT_HD720;
		}
		else if (2 == byChnnl)	//2出
		{
			if( tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_H263 || tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_MP4 )
			{
				byMediaType = tConfInfo.GetMainVideoMediaType(); 
				byRes =  VIDEO_FORMAT_CIF;
			}
			else
			{
				if(tConfInfo.GetSecVideoMediaType() == MEDIA_TYPE_H263 || tConfInfo.GetSecVideoMediaType() == MEDIA_TYPE_MP4)
				{
					byMediaType = tConfInfo.GetSecVideoMediaType(); //出other
					byRes =  VIDEO_FORMAT_CIF;
				}
				else
				{
					byMediaType = MEDIA_TYPE_H264;
					byRes =  VIDEO_FORMAT_CIF;
				}
			}
		}
		else					//3出
		{
			byMediaType = MEDIA_TYPE_H264;
			byRes = VIDEO_FORMAT_4CIF;
		}
	}
	/*
	if(DEVVER_MPU == 4501)
	{

		if (MEDIA_TYPE_H264 == tConfInfo.GetMainVideoMediaType())
		{
			byMediaType = MEDIA_TYPE_H264;
			if( 2 == byChnnl )
			{
				if(tConfInfo.GetSecVideoMediaType() == MEDIA_TYPE_H263 || tConfInfo.GetSecVideoMediaType() == MEDIA_TYPE_MP4 )
				{
					byMediaType = tConfInfo.GetSecVideoMediaType();//出other
					byRes =  VIDEO_FORMAT_CIF;
				}
				else
				{
					byRes =  VIDEO_FORMAT_CIF;
				}
			}
			else if( 0 == byChnnl )
			{
				if( VIDEO_FORMAT_HD720 == tConfInfo.GetMainVideoFormat()
					|| ( VIDEO_FORMAT_HD1080 == tConfInfo.GetMainVideoFormat()//1080,720的会议第1出出720
					&& tConfInfo.GetConfAttrbEx().IsResEx720() )
					)
				{
					byRes = VIDEO_FORMAT_HD720;
				}
				else if( VIDEO_FORMAT_HD1080 == tConfInfo.GetMainVideoFormat() ||
					VIDEO_FORMAT_CIF == tConfInfo.GetMainVideoFormat() )
				{
					byRes = VIDEO_FORMAT_HD1080;
				}
				else
				{
					//do nothing
				}
			}
			else
			{
				//do nothing
			}
		}
		else //h263会议和mpeg4会议
		{
			byMediaType = ( 0 == byChnnl )? MEDIA_TYPE_H264: tConfInfo.GetMainVideoMediaType();
			byRes = ( 0 == byChnnl )? VIDEO_FORMAT_HD1080 : VIDEO_FORMAT_CIF;  
		}
	}
	else if(DEVVER_MPU > 4501)
	{
		//MPU 4出版本 （B板）
		if( 0 == byChnnl )	//0出
		{
			byMediaType = MEDIA_TYPE_H264;
			byRes = VIDEO_FORMAT_HD1080;
		}
		else if (1 == byChnnl)	//1出
		{
			byMediaType = MEDIA_TYPE_H264;
			byRes = VIDEO_FORMAT_HD720;
		}
		else if (2 == byChnnl)	//2出
		{
			if( tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_H263 || tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_MP4 )
			{
				byMediaType = tConfInfo.GetMainVideoMediaType(); 
				byRes =  VIDEO_FORMAT_CIF;
			}
			else
			{
				if(tConfInfo.GetSecVideoMediaType() == MEDIA_TYPE_H263 || tConfInfo.GetSecVideoMediaType() == MEDIA_TYPE_MP4)
				{
					byMediaType = tConfInfo.GetSecVideoMediaType(); //出other
					byRes =  VIDEO_FORMAT_CIF;
				}
				else
				{
					byMediaType = MEDIA_TYPE_H264;
					byRes =  VIDEO_FORMAT_CIF;
				}
			}
		}
		else					//3出
		{
			byMediaType = MEDIA_TYPE_H264;
			byRes = VIDEO_FORMAT_4CIF;
		}
	}
	*/
	return byRes;
}



/*====================================================================
    函数名      ：topoGetAllTopoSubMcu
    功能        ：获取整个拓扑结构中该MCU的所有下级MCU(可选一层或所有层)
    算法实现    ：递归
    引用全局变量：无
    输入参数说明：WORD wMcuId, 用户输入的McuId
				  u16 wMcuTopo[], 数组，用于存放查找的结果
				  u8 bySize, 数组元素个数
				  TMcuTopo atMcuTopo[], MCU拓扑数组
				  u16 wMcuTopoNum, 全网中MCU数目
				  u8 byMode, 访问层数，0表示所有层，1表示一层，缺省为0
    返回值说明  ：实际数目
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/08    1.0         方华平        创建
    02/09/19    1.0         LI Yi         从CMcuAgent中移过来
    02/10/24    1.0         LI Yi         添加参数byMode
====================================================================*/
 u16 topoGetAllTopoSubMcu( u16    wMcuId,
                                 u16    wSubMcu[],
                                 u8  bySize, 
							     TMcuTopo atMcuTopo[],
                                 u16    wMcuTopoNum,
                                 u8  byMode )
{
	u16	wLoop;
	u16	wNum = 0;
	u16    wMMcuNum;
	u8 byLayer = 0;

	ASSERT( atMcuTopo != NULL );

	for( wLoop = 0 ; wLoop < wMcuTopoNum && wNum < bySize; wLoop++ )
	{
		if(byMode != 0)
		{	
		    if( wMcuId == atMcuTopo[wLoop].GetSMcuId() && wMcuId != atMcuTopo[wLoop].GetMcuId() )
			{
			    wSubMcu[wNum] = atMcuTopo[wLoop].GetMcuId();
			    wNum++;
			}
		}
		else
		{
            wMMcuNum = wLoop;
			byLayer = 0;
			while(wMMcuNum != atMcuTopo[wMMcuNum].GetSMcuId() && byLayer<3)
			{
                 wMMcuNum = atMcuTopo[wMMcuNum].GetSMcuId();
				 if(wMMcuNum == wMcuId && wMcuId != atMcuTopo[wLoop].GetMcuId())
				 {
                     wSubMcu[wNum] = atMcuTopo[wLoop].GetMcuId();
			         wNum++;
					 break;
				 }
				 byLayer++;
			}
		}
	}
	
    return( wNum );
}

/*====================================================================
    函数名      ：topoGetMcuTopo
    功能        ：获取指定MCU的自TOP MCU起的拓扑路径
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId, 用户输入的McuId
				  u16 wMcuTopo[], 数组，用于存放查找的结果，从指定MCU
						          到TOP MCU，如"3,2,1"， 0结尾
				  u8 bySize, 数组元素个数，要求不小于5
				  TMcuTopo atMcuTopo[], MCU拓扑数组
				  u16 wMcuTopoNum, 全网中MCU数目
    返回值说明  ：成功返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/02    1.0         方华平        创建
    02/09/19    1.0         LI Yi         从CMcuAgent中移过来
====================================================================*/
  BOOL topoGetMcuTopo( u16     wMcuId,
                            u16     wMcuTopo[],
                            u8   bySize, 
					        TMcuTopo atMcuTopo[],
                            u16     wMcuTopoNum )
{
	u16 wBufLoop;
	u16 wEntLoop;

	ASSERT( atMcuTopo != NULL );

	memset( wMcuTopo, 0, bySize * sizeof( u16 ) );

    //Set the first element of buffer to wMcuId
	wMcuTopo[0] = wMcuId;

	for( wBufLoop = 0; wBufLoop < bySize - 1; wBufLoop++ )
	{
		for( wEntLoop = 0; wEntLoop < wMcuTopoNum; wEntLoop++ )
		{
			if( wMcuTopo[wBufLoop] == atMcuTopo[wEntLoop].GetMcuId() )
			{
				if( wMcuTopo[wBufLoop] != atMcuTopo[wEntLoop].GetSMcuId() )
				{
                    //find the MCU but it's not topMcu
					wMcuTopo[wBufLoop + 1] = atMcuTopo[wEntLoop].GetSMcuId();
					break;
				}
				else
				{
                    //find the topMCU
					return( TRUE );
				}
			}
		}
		if( wMcuTopo[wBufLoop + 1] == 0 )	//cannot find the MCU
		{
			break;
		}
	}	

	//Cannot find the McuId
	printf( "Cannot find Mcu%d!\n", wMcuId );

    return( FALSE );
}

/*====================================================================
    函数名      ：topoGetInterMcu
    功能        ：计算从源MCU出发到目的MCU的下一站直连MCU号
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wSrcMcu, 源MCU号
				  u16 wDstMcu, 目的MCU号
				  TMcuTopo atMcuTopo[], MCU拓扑数组
				  u16 wMcuTopoNum, 全网中MCU数目
    返回值说明  ：返回直连MCU号，0xffff表示下一站为源MCU的上级MCU，
				  失败返回NULL
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/30    1.0         LI Yi         创建
====================================================================*/
  u16 topoGetInterMcu( u16 wSrcMcu, u16 wDstMcu, TMcuTopo atMcuTopo[], u16 wMcuTopoNum )
{
	u16	awBuf[5];
	u8	byLoop;

	ASSERT( atMcuTopo != NULL );

	if( wDstMcu == wSrcMcu )	//self
		return( wDstMcu );

	if( !topoGetMcuTopo( wDstMcu, awBuf, 5, atMcuTopo, wMcuTopoNum ) )	//fail to find it
	{
		printf( "Cannot find the path to MCU%u!\n", wDstMcu );
		return( NULL );
	}

	for( byLoop = 1; byLoop < 5; byLoop++ )
	{
		if( awBuf[byLoop] == wSrcMcu )
			return( awBuf[byLoop - 1] );
	}

	return( ( u16 )-1 );	//是上面MCU，返回0xFFFF
}

/*====================================================================
    函数名      ：topoFindMcuPath
    功能        ：计算从源MCU出发到目的MCU的路
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wSrcMcu, 源MCU号
				  u16 wDstMcu, 目的MCU号
				  u16 awMcuPath[], 返回的路径BUFFER，从源到目的MCU，
						如"1,2,3"，0结尾
				  u8 byBufSize, BUFFER大小
				  TMcuTopo atMcuTopo[], MCU拓扑数组
				  u16 wMcuTopoNum, 全网中MCU数目
    返回值说明  ：成功返回TRUE，失败返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/09/19    1.0         LI Yi         创建
====================================================================*/
  BOOL topoFindMcuPath( u16 wSrcMcu, u16 wDstMcu, u16 awMcuPath[], u8 byBufSize, 
					         TMcuTopo atMcuTopo[], u16 wMcuTopoNum )
{
	u16	wNextMcu = wSrcMcu;
	u8	byLoop, byLoop2;

	ASSERT( atMcuTopo != NULL );

	if( byBufSize < 2 )
		return( FALSE );

	byLoop = 0;
	while( wNextMcu != NULL && byLoop < byBufSize - 1 )
	{
		awMcuPath[byLoop++] = wNextMcu;
		
		if( wNextMcu == wDstMcu )	//finished
		{
			awMcuPath[byLoop] = 0;
			return( TRUE );
		}

		wSrcMcu = wNextMcu;
		wNextMcu = topoGetInterMcu( wSrcMcu, wDstMcu, atMcuTopo, wMcuTopoNum );
		if( wNextMcu == 0xffff )	//to superior MCU
		{
			//find its superior MCU ID
			for( byLoop2 = 0; byLoop2 < wMcuTopoNum; byLoop2++ )
			{
				if( atMcuTopo[byLoop2].GetMcuId() == wSrcMcu )
				{
					wNextMcu = atMcuTopo[byLoop2].GetSMcuId();
					break;
				}
			}
			if( byLoop2 == wMcuTopoNum )	//wrong!
				return( FALSE );
		}
	}

	return( FALSE );
}

/*====================================================================
    函数名      ：topoIsCollide
    功能        ：判断用户正在看或听的终端（或外设）是否与跨MCU正在
					发生的改变有线路冲突
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wCurSrcMcuId, 用户正选看或听的终端或外设所属MCU号
				  u16 wConnMcuId, 用户连接的MCU号
				  u16 wSrcMcuId, 线路改变的源终端或外设所属MCU号
				  u16 wDstMcuId, 线路改变的目的终端或外设所属MCU号
				  TMcuTopo atMcuTopo[], MCU拓扑数组
				  u16 wMcuTopoNum, 全网中MCU数目
    返回值说明  ：有冲突返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/09/19    1.0         LI Yi         创建
====================================================================*/
  BOOL topoIsCollide( u16 wCurSrcMcuId,
                           u16 wConnMcuId,
                           u16 wSrcMcuId, 
					       u16 wDstMcuId,
                           TMcuTopo atMcuTopo[],
                           u16      wMcuTopoNum )
{
	u16	awCurPath[16];		//用户当前路径
	u16	awChangePath[16];	//线路变化的路径
	u8	byLoop1, byLoop2;

	ASSERT( atMcuTopo != NULL );

	//get two paths
	if( !topoFindMcuPath( wCurSrcMcuId, wConnMcuId, awCurPath, 16, atMcuTopo, wMcuTopoNum ) )
	{
		printf( "wrong path from MCU%d to MCU%d!\n", wCurSrcMcuId, wConnMcuId );
		return( FALSE );
	}
	if( !topoFindMcuPath( wSrcMcuId, wDstMcuId, awChangePath, 16, atMcuTopo, wMcuTopoNum ) )
	{
		printf( "wrong path from MCU%d to MCU%d!\n", wSrcMcuId, wDstMcuId);
		return( FALSE );
	}

	//judge two paths if they collide with each other
	byLoop1 = 0;
	while( awCurPath[byLoop1] != 0 )
	{
		byLoop2 = 0;
		while( awChangePath[byLoop2] != 0 )
		{
			if( awCurPath[byLoop1]   == awChangePath[byLoop2] &&
                awCurPath[byLoop1+1] == awChangePath[byLoop2+1] &&
                awCurPath[byLoop1+1] != 0 )
            {
				return TRUE;
            }

			byLoop2++;
		}

		byLoop1++;
	}

	return( FALSE );
}

/*====================================================================
    函数名      ：topoGetMcuInfo
    功能        ：得到所要的Mcu的TMcuTopo结构；
				  用户需自己保证输入的MCU号在数组中实际存在
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId,要求的MCU号
				  TMcuTopo atMcuTopo[]，MCU拓扑数组 
				  u16 wMcuTopoNum，全网中的MCU数目
    返回值说明  ：成功返回TMcuTopo，否则返回一个内部全为0的TMcuTopo结构
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/25    1.0         Liaoweijiang  创建
====================================================================*/
  TMcuTopo topoGetMcuInfo( u16 wMcuId,  
					            TMcuTopo atMcuTopo[], u16 wMcuTopoNum )
{
	TMcuTopo	tMcuTopo;

    ASSERT( atMcuTopo != NULL );

	for( u16 wLoop=0; wLoop<wMcuTopoNum; wLoop++ )
	{
		if( atMcuTopo[wLoop].GetMcuId() == wMcuId )
		{
			return atMcuTopo[wLoop];
		}
	}
	
	memset( &tMcuTopo, 0, sizeof(TMcuTopo) );
	return	tMcuTopo;
}

/*====================================================================
    函数名      ：topoGetMtInfo
    功能        ：得到所要的Mt的TMtTopo结构
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId，MT所在的MCU号
				  u8 byMtId，MT在MCU中的编号
				  TMcuTopo atMtTopo[]，Mt的拓扑数组
				  u16 wMtTopoNum，全网的Mt的数目
    返回值说明  ：成功返回TMtTopo，否则返回一个为全0的TMtTopo结构
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/25    1.0         Liaoweijiang  创建
====================================================================*/
  TMtTopo topoGetMtInfo( u16 wMcuId, u8 byMtId, 
					          TMtTopo atMtTopo[], u16 wMtTopoNum )
{
	TMtTopo	tMtTopo;

    ASSERT( atMtTopo != NULL );

	for(u16 wLoop=0; wLoop<wMtTopoNum; wLoop++)
	{
		if( atMtTopo[wLoop].GetMcuId() == wMcuId && atMtTopo[wLoop].m_byMtId == byMtId )
		{
			return atMtTopo[wLoop];
		}
	}

	memset( &tMtTopo, 0, sizeof(TMtTopo) );
	return	tMtTopo;
}

/*====================================================================
    函数名      ：topoIsSecondaryMt
    功能        ：判断一个Mt是否是次级终端
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId，MT所在的MCU号
				  u8 byMtId，在所给的MCU中的MT号
				  TMtTopo atMtTopo[]，MT的拓扑数组
				  u16 wMtTopoNum，全网MT的数目
    返回值说明  ：如果是返回TRUE, 否则FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/25    1.0         Liaoweijiang  创建
====================================================================*/
  BOOL topoIsSecondaryMt( u16 wMcuId, u8 byMtId, 
					           TMtTopo atMtTopo[], u16 wMtTopoNum )
{
    ASSERT( atMtTopo != NULL );

	for(u16 wLoop=0; wLoop<wMtTopoNum; wLoop++)
	{
		if( atMtTopo[wLoop].GetMcuId() == wMcuId && atMtTopo[wLoop].m_byMtId == byMtId )
		{
			if(atMtTopo[wLoop].m_byPrimaryMtId !=0 )
				return TRUE;
			else
				return FALSE;
		}
	}
    
	return	FALSE;
}

/*====================================================================
    函数名      ：topoHasSecondaryMt
    功能        ：判断一个Mt是否有次级终端
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId，MT所在的MCU号
				  u8 byMtId，MT在此MCU中的号
				  TMtTopo atMtTopo[]，全网的MT拓扑数组
				  u16 wMtTopoNum，全网中的MT数目
    返回值说明  ：成功返回TRUE, 否则FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/25    1.0         Liaoweijiang  创建
====================================================================*/
  BOOL topoHasSecondaryMt( u16 wMcuId, u8 byMtId, 
					            TMtTopo atMtTopo[], u16 wMtTopoNum )
{
    ASSERT( atMtTopo != NULL );

	for(u16 wLoop=0; wLoop<wMtTopoNum; wLoop++)
	{
		if( atMtTopo[wLoop].GetMcuId() == wMcuId && atMtTopo[wLoop].m_byPrimaryMtId == byMtId )
		{
				return TRUE;
		}
	}
    
	return	FALSE;
}
/*====================================================================
    函数名      ：topoGetPrimaryMt
    功能        ：得到一个终端的主终端
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId，MT所在的MCU号
				  u8 byMtId，MT在此MCU中的号
				  TMtTopo atMtTopo[]，全网的MT拓扑数组
				  u16 wMtTopoNum，全网中的MT数目
    返回值说明  ：成功返回它的主终端号,返回0说明它就是主终端，或非法终端
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/25    1.0         Liaoweijiang  创建
====================================================================*/
  u8 topoGetPrimaryMt( u16 wMcuId, u8 byMtId, 
					           TMtTopo atMtTopo[],  u16  wMtTopoNum )
{
    ASSERT( atMtTopo != NULL );

	for(u16 wLoop=0; wLoop < wMtTopoNum; wLoop++)
	{
		if( atMtTopo[wLoop].GetMcuId() == wMcuId && atMtTopo[wLoop].m_byMtId == byMtId )
		{	
			//判断它是否是次级终端
			return atMtTopo[wLoop].m_byPrimaryMtId;
		}
	}
	return 0;
}

/*====================================================================
    函数名      ：topoGetExcludedMt
    功能        ：得到一个终端的所有互斥终端
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId，MT所在的MCU号
				  u8 byMtId，MT在此MCU中的号
				  TMtTopo atMtTopo[]，全网的MT拓扑数组
				  u16 wMtTopoNum，全网中的MT数目
				  u8 MtBuffer[]，传进来准备装互斥终端号的数组
				  u16 BufferSize，传进来数组的大小
    返回值说明  ：成功返回实际的终端数目, 否则NULL
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/25    1.0         Liaoweijiang  创建
====================================================================*/
  u16 topoGetExcludedMt( u16 wMcuId, u8 byMtId, 
					          TMtTopo atMtTopo[], u16 wMtTopoNum,
                              u8 MtBuffer[], u16 wBufferSize)
{
    u16	wLoop, wLoop2=0;
	u8	byTempMtId;
	
	ASSERT( atMtTopo != NULL );
	
	//如果是主终端
	if( !topoIsSecondaryMt( wMcuId, byMtId, atMtTopo, wMtTopoNum ) )
	{
		for( wLoop = 0; wLoop < wMtTopoNum; wLoop++)
		{
			if( atMtTopo[wLoop].GetMcuId () == wMcuId && atMtTopo[wLoop].m_byPrimaryMtId == byMtId )
			{
				MtBuffer[wLoop2++] = atMtTopo[wLoop].m_byMtId;
			}
		}
		
		if(wLoop2 >= min( wBufferSize, wMtTopoNum ) )  
			return wLoop2;
	}
	
	//如果是次级终端，则互斥终端包括所连的主终端和其他的次级终端
	else
	{
		//首先得到次级终端的主终端
		for( wLoop = 0; wLoop < wMtTopoNum; wLoop++)
		{
			if( atMtTopo[wLoop].GetMcuId () == wMcuId && atMtTopo[wLoop].m_byMtId == byMtId )
			{
				byTempMtId = atMtTopo[wLoop].m_byPrimaryMtId;
			}
		}
		
		//得到此次级终端的互斥终端，包括所连的主终端和其他的次级终端
		for( wLoop = 0; wLoop < wMtTopoNum; wLoop++)
		{
			if( ( atMtTopo[wLoop].GetMcuId () == wMcuId && atMtTopo[wLoop].m_byMtId == byTempMtId ) || //所连的主终端
				( atMtTopo[wLoop].GetMcuId () == wMcuId && atMtTopo[wLoop].m_byMtId != byMtId
				&& atMtTopo[wLoop].m_byPrimaryMtId == byTempMtId ) ) //同一主终端下的其他的次级终端
			{
				MtBuffer[wLoop2++]=atMtTopo[wLoop].m_byMtId;
			}
		}
		
		if(wLoop2 >= min( wBufferSize, wMtTopoNum ) )  
			return wLoop2;
	}
    
	return wLoop2;
}

/*====================================================================
    函数名      ：topoGetAllSubMt
    功能        ：得到一个Mcu下的所有Mt
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId : MCU号
				  TMtTopo atMtTopo[] : Mt的拓扑数组
				  u16 wMtTopoNum：全网MT的数目
				  TMt MtBuffer[], 准备放置得到MT的TMt结构的数组
				  u16 wBufferSize, 数组大小
				  byMode 访问模式
					为0,表示只得到本级的MT
					为1,表示可以得到下一级MCU的MT
					为2,表示得到往下二级MCU的MT
    返回值说明  ：实际得到的MT的数目
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/25    1.0         Liaoweijiang  创建
====================================================================*/
  u16 topoGetAllSubMt( u16 wMcuId, TMtTopo atMtTopo[], u16 wMtTopoNum, 
							TMt MtBuffer[], u16 wBufferSize, u8 byMode )
{
	u16	wLoop;
	u16	wNum = 0;

	for( wLoop = 0 ; wLoop < wMtTopoNum; wLoop++ )
	{
		if( atMtTopo[wLoop].GetMcuId() == wMcuId )
		{
			//设置MCU号和MT号，外设号设为0，表示是终端类型
			MtBuffer[wNum].SetMt( (u8)atMtTopo[wLoop].GetMcuId(), 
				 atMtTopo[wLoop].m_byMtId );

            //FIXME:
			//设置别名
			//MtBuffer[wNum].SetAlias( atMtTopo[wLoop].GetAlias() );

			if( wNum >= wBufferSize )
				return wNum;

			wNum++;
		}
	}
    return( wNum );
}

/*====================================================================
    函数名      ：topoGetTopMcuBetweenTwoMcu
    功能        ：得到两个MCU的最低至高点的MCU
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId1 : MCU号
				  u16 wMcuId2 : MCU号
				  TMtTopo atMtTopo[] : Mcu的拓扑数组
				  u16 wMtTopoNum：全网Mcu的数目
    返回值说明  ：两个MCU的最低至高点的MCU号
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/11/07    1.0         Liaoweijiang  创建
====================================================================*/
  u16 topoGetTopMcuBetweenTwoMcu ( u16 wMcuId1, u16 wMcuId2,
										TMcuTopo atMcuTopo[], u16 wMcuTopoNum )
{
	u16 wPath1[5], wPath2[5];

	memset( wPath1, 0, sizeof(wPath1) );
	memset( wPath2, 0, sizeof(wPath2) );
	if( !topoGetMcuTopo( wMcuId1, wPath1, 5, atMcuTopo, wMcuTopoNum ) )
		return 0;
	if( !topoGetMcuTopo( wMcuId2, wPath2, 5, atMcuTopo, wMcuTopoNum ) )
		return 0;

	for( u8 byLoop1=0; byLoop1<5 && wPath1[byLoop1] != 0; byLoop1++)
	{
		for( u8 byLoop2=0; byLoop2<5 && wPath2[byLoop2] != 0; byLoop2++)
		{
			if( wPath1[byLoop1] == wPath2[byLoop2] )
				return wPath1[byLoop1];
		}
	}
	return 0;
}

/*====================================================================
    函数名      ：topoGetTopMcu
    功能        ：得到所有MCU的最低至高点的Mcu
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId[] : MCU号数组
				  u8 byMcuNum : 数组大小
				  TMtTopo atMtTopo[] : Mcu的拓扑数组
				  u16 wMtTopoNum：全网Mcu的数目
    返回值说明  ：最低至高点的Mcu
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/11/07    1.0         Liaoweijiang  创建
====================================================================*/
  u16 topoGetTopMcu( u16 wMcuId[], u8 byMcuNum, 
						  TMcuTopo atMcuTopo[], u16 wMcuTopoNum )
{
	u8 byArrayNum = 0;
	u16  wTopMcuId = 0;
	
	ASSERT( atMcuTopo != NULL );

	byArrayNum = byMcuNum;
	wTopMcuId = wMcuId[ byArrayNum - 1 ];
	//采用从数组最后的两个元素开始比较，将比较结果填入倒数第二个元素位置，
	//如此又重新开始，直到数组只剩下最后一个元素，返回它
	while( byArrayNum > 1 )
	{
		wTopMcuId = topoGetTopMcuBetweenTwoMcu( wMcuId[byArrayNum - 2],
								   wTopMcuId, atMcuTopo, wMcuTopoNum );
		byArrayNum--;
	}

	return wTopMcuId;
}


/*====================================================================
    函数名      ：topoJudgeLegality
    功能        ：拓扑结构合法性判断
    算法实现    ：
    引用全局变量：
    输入参数说明：TMcuTopo atMcuTopo[], Mcu的拓扑数组
				  u16 wMcuTopoNum, 全网Mcu的数目
				  TMtTopo atMtTopo[] : Mt的拓扑数组
				  u16 wMtTopoNum：全网Mt的数目
    返回值说明  ：最低至高点的Mcu
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/11/13    1.0         Liaoweijiang  创建
====================================================================*/
BOOL topoJudgeLegality( TMcuTopo atMcuTopo[], u16 wMcuTopoNum,
							   TMtTopo atMtTopo[], u16 wMtTopoNum )
{
	u16	wLoop, wLoop2;
	u16	wPath1[5];
	u16	wFlag = 0;
	BOOL	bResult = TRUE;

	ASSERT( atMcuTopo != NULL );

	//一个MCU的上级MCU是否存在的判断
	wFlag = 0;	
	for( wLoop = 0; wLoop < wMcuTopoNum; wLoop++ )
	{
		for( wLoop2 = 0; wLoop2 < wMcuTopoNum; wLoop2++ )
		{
			if( atMcuTopo[wLoop].GetSMcuId() == atMcuTopo[wLoop2].GetMcuId() )
			{
				wFlag++;
				break;
			}
		}
		if( wFlag == 0 )	//是0表示不存在
		{
			printf( "MCU%d's Super-MCU MCU%d does not exist!\n", atMcuTopo[wLoop].GetMcuId(),
				atMcuTopo[wLoop].GetSMcuId() );
			bResult = FALSE;
		}
		wFlag = 0;
	}

	//拓扑结构的TOPMCU唯一性判断
	wFlag = 0;	
	for( wLoop = 0; wLoop < wMcuTopoNum; wLoop++ )
	{
		if( atMcuTopo[wLoop].GetMcuId() == atMcuTopo[wLoop].GetSMcuId() )
		{
			wFlag++;
		}
	}
	if( wFlag > 1 )		//多个
	{
		printf( "The TOPMCU is not only one in topology!\n" );
		bResult = FALSE;
	}
	if( wFlag == 0 )	//是0表示不存在
	{
		printf( "The TOPMCU does not exist in topology!\n" );
		bResult = FALSE;
	}

	//一个MT所属MCU是否存在判断
	wFlag = 0;
	for( wLoop = 0; wLoop < wMtTopoNum; wLoop++ )
	{
		for( wLoop2 = 0; wLoop2 < wMcuTopoNum; wLoop2++ )
		{
			if( atMtTopo[wLoop].GetMcuId() == atMcuTopo[wLoop2].GetMcuId() )
			{
				wFlag++;
				break;
			}
		}
		if( wFlag == 0 )//是0表示不存在
		{
			printf( "MCU%d-MT%d's Super-MCU MCU%d does not exist!\n", 
					atMtTopo[wLoop].GetMcuId(), atMtTopo[wLoop].m_byMtId, 
					atMtTopo[wLoop].GetMcuId() );
			bResult = FALSE;
		}
		wFlag = 0;
	}

	//MCU上下级别关系是否冲突判断
	for( wLoop = 0; wLoop < wMcuTopoNum; wLoop++ )
	{
		if( !topoGetMcuTopo( atMcuTopo[wLoop].GetMcuId(), wPath1, 5, atMcuTopo, wMcuTopoNum ) )
		{
			printf( "There could have some mistakes between MCU%u and its superior MCUs!\n", 
				atMcuTopo[wLoop].GetMcuId() );
			continue;
		}
		for( wLoop2 = 1; wLoop2<5 && wPath1[wLoop2] != 0; wLoop2++)
		{
			if( wPath1[wLoop2] == atMcuTopo[wLoop].GetMcuId() )
			{
				printf( "MCU%d Super-MCU or indirect Super-MCU MCU%d is not correct!\n", 
					atMcuTopo[wLoop].GetMcuId(), atMcuTopo[wLoop].GetSMcuId() );
				bResult = FALSE;
				break;
			}
		}
	}

	//MT类型判断
	for( wLoop = 0; wLoop < wMtTopoNum; wLoop++ )
	{
		if( atMtTopo[wLoop].m_byMtType < MTTYPE_RCVTRS || atMtTopo[wLoop].m_byMtType > MTTYPE_RCVTRSADAPT )
		{
			printf( "MCU%d-MT%d's type %u not exist!\n", 
					atMtTopo[wLoop].GetMcuId(), atMtTopo[wLoop].m_byMtId, atMtTopo[wLoop].m_byMtType );
			bResult = FALSE;
		}
	}

	//MT如果是次级终端，则其主终端的主终端必须是0
	for( wLoop = 0; wLoop < wMtTopoNum; wLoop++ )
	{
		if( atMtTopo[wLoop].m_byPrimaryMtId != 0 )
		{
			for( wLoop2 = 0; wLoop2 < wMtTopoNum; wLoop2++ )
			{
				//判断它是所给终端的主终端
				if( atMtTopo[wLoop2].GetMcuId() == atMtTopo[wLoop].GetMcuId() 
					&& atMtTopo[wLoop2].m_byMtId == atMtTopo[wLoop].m_byPrimaryMtId )
				{
					if( atMtTopo[wLoop2].m_byPrimaryMtId !=0 )
					{	
						printf( "MCU%d-MT%d's Primary-MT MCU%d-MT%d is not right!\n", 
							atMtTopo[wLoop].GetMcuId(), atMtTopo[wLoop].m_byMtId, 
							atMtTopo[wLoop].GetMcuId(), atMtTopo[wLoop].m_byPrimaryMtId );
						bResult = FALSE;
						break;
					}
				}
			}
		}
	}

	return bResult;
}

/*====================================================================
    函数名      ：topoIsValidMt
    功能        ：根据终端号，IP地址和假名进行合法性判断
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId, MCU号
				  u8 byMtId, MT号
				  LPCSTR lpszAlias, 终端假名
				  TMtTopo atMtTopo[], Mt的拓扑数组
				  u16 wMtTopoNum, 全网Mt的数目
    返回值说明  ：成功返回TRUE，失败返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/05    1.0         LI Yi         创建
====================================================================*/
BOOL topoIsValidMt( u16 wMcuId, u8 byMtId, LPCSTR lpszAlias, TMtTopo atMtTopo[], u16 wMtTopoNum )
{
	TMtTopo	tMtInfo;

	tMtInfo = topoGetMtInfo( wMcuId, byMtId, atMtTopo, wMtTopoNum );

	//ID
	if( tMtInfo.GetMcuId()!= wMcuId || tMtInfo.m_byMtId != byMtId )
	{
		printf( "Invalid MT%u-%u! Wrong MCU or MT ID!\n", wMcuId, byMtId );
		return( FALSE );
	}

	//Alias
	if( strcmp( tMtInfo.GetAlias(), lpszAlias ) != 0 )
	{
		printf( "Invalid MT%u-%u! Wrong Alias %s!\n", wMcuId, byMtId, lpszAlias );
		return( FALSE );
	}

	return( TRUE );
}

/*====================================================================
    函数名      ：topoIsValidMcu
    功能        ：根据MCU号，IP地址和假名进行合法性判断
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId, MCU号
				  u32 dwIpAddr, 终端IP地址，网络序
				  LPCSTR lpszAlias, 终端假名
				  TMcuTopo atMcuTopo[], Mcu的拓扑数组
				  u16 wMcuTopoNum, 全网Mcu的数目
    返回值说明  ：成功返回TRUE，失败返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/05    1.0         LI Yi         创建
====================================================================*/
BOOL topoIsValidMcu( u16 wMcuId, u32 dwIpAddr, LPCSTR lpszAlias, 
						   TMcuTopo atMcuTopo[], u16 wMcuTopoNum )
{
	TMcuTopo	tMcuInfo;

	ASSERT( atMcuTopo != NULL );

	tMcuInfo = topoGetMcuInfo( wMcuId, atMcuTopo, wMcuTopoNum );

	//ID
	if( tMcuInfo.GetMcuId()!= wMcuId )
	{
		printf( "Invalid Mcu%u! Wrong MCU ID!\n", wMcuId );
		return( FALSE );
	}

	//IP
	if( tMcuInfo.GetIpAddr() != dwIpAddr && tMcuInfo.GetGwIpAddr() != dwIpAddr )
	{
		printf( "Invalid Mcu%u! Wrong IP %#.8x!\n", wMcuId, dwIpAddr );
		return( FALSE );
	}

	//Alias
	if( strcmp( tMcuInfo.GetAlias(), lpszAlias ) != 0 )
	{
		printf( "Invalid Mcu%u! Wrong Alias %s!\n", wMcuId, lpszAlias );
		return( FALSE );
	}

	return( TRUE );
}

/*====================================================================
    函数名      ：topoFindSubMtByIp
    功能        ：根据MCU号，IP地址得到下级终端号
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId, MCU号
				  u32 dwIpAddr, 终端IP地址，网络序
				  TMcuTopo atMtTopo[]，Mt的拓扑数组
				  u16 wMtTopoNum，全网的Mt的数目
    返回值说明  ：成功返回终端号，失败返回0
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/03/12    1.0         LI Yi         创建
====================================================================*/
u8 topoFindSubMtByIp( u16 wMcuId, u32 dwIpAddr, TMtTopo atMtTopo[], u16 wMtTopoNum )
{
    ASSERT( atMtTopo != NULL );

	for( u16 wLoop = 0; wLoop < wMtTopoNum; wLoop++ )
	{
		if( atMtTopo[wLoop].GetMcuId() == wMcuId && atMtTopo[wLoop].GetIpAddr() == dwIpAddr )
		{
			return( atMtTopo[wLoop].m_byMtId );
		}
	}

	return( 0 );
}

/*====================================================================
    函数名      ：topoMtNeedAdapt
    功能        ：判断终端是否需要进行适配
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId, MCU号
				  u32 dwIpAddr, 终端IP地址，网络序
				  TMcuTopo atMtTopo[]，Mt的拓扑数组
				  u16 wMtTopoNum，全网的Mt的数目
    返回值说明  ：成功返回终端号，失败返回0
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/03/12    1.0         LI Yi         创建
====================================================================*/
BOOL topoMtNeedAdapt( u16 wMcuId, u8 byMtId, TMtTopo atMtTopo[], u16 wMtTopoNum )
{
    ASSERT( atMtTopo != NULL );

	for( u16 wLoop = 0; wLoop < wMtTopoNum; wLoop++ )
	{
		if( atMtTopo[wLoop].GetMcuId() == wMcuId && atMtTopo[wLoop].m_byMtId == byMtId )
		{
			if( atMtTopo[wLoop].m_byMtType == MTTYPE_RCVTRSADAPT )	//需要适配
				return( TRUE );
			else
				return( FALSE );
		}
	}

	return( FALSE );
}




/*====================================================================
    函数名      ：ReadMcuTopoTable
    功能        ：读入MCU拓扑信息
    算法实现    ：
    引用全局变量：无
    输入参数说明：LPCSTR lpszProfileName, 文件名（含绝对路径）
                  TMcuTopo atMcuTopoBuf[], MCU信息BUFFER
				  u16 wBufSize, BUFFER大小
    返回值说明  ：拓扑中MCU实际数目
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	02/09/18    1.1         LI Yi         创建
====================================================================*/
u8 ReadMcuTopoTable( LPCSTR lpszProfileName, TMcuTopo atMcuTopo[], u16 wBufSize )
{
	LPSTR   *lpszTable;
    char    chSeps[] = " \t";       /* space or tab as seperators */
    char    *pchToken;
    u32		dwLoop;
    BOOL    bResult = TRUE;
    u32		dwMemEntryNum;
	u32		dwMcuNum;

    ASSERT( atMcuTopo != NULL );
    
    /* get the number of entry */
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mcuTopoTable, STR_ENTRY_NUM, 
                0, ( int * )&dwMemEntryNum );

    dwMemEntryNum = min( dwMemEntryNum, wBufSize );
	lpszTable = (LPSTR *)malloc( dwMemEntryNum * sizeof( LPSTR ) );
    for( dwLoop = 0; dwLoop < dwMemEntryNum; dwLoop++ )
    {
		lpszTable[dwLoop] = (char *)malloc( MAX_VALUE_LEN + 1 );
    }
    dwMcuNum = dwMemEntryNum;

	/* get entry strings */
    bResult = GetRegKeyStringTable( lpszProfileName, SECTION_mcuTopoTable,
                   "fail", lpszTable, &dwMcuNum, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )
    {
        dwMcuNum = 0;
    }
    
	/* analyze entry strings */
    for( dwLoop = 0; dwLoop < dwMcuNum && dwLoop < wBufSize; dwLoop++ )
    {
        /* McuId  */
        pchToken = strtok( lpszTable[dwLoop], chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", FILED_McuId );
            bResult = FALSE;
        }
        else
        {
            atMcuTopo[dwLoop].SetMcuId( atoi( pchToken ) );
        }
        /* MCU的IP地址 */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", FILED_McuIpAddr );
            bResult = FALSE;
        }
        else
        {
           atMcuTopo[dwLoop].SetIpAddr( ntohl(inet_addr( pchToken ) ) );
        }
        /* MCU的网关IP地址 */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", FILED_McuGwIpAddr );
            bResult = FALSE;
        }
        else
        {
           atMcuTopo[dwLoop].SetGwIpAddr( ntohl(inet_addr( pchToken )) );
        }
        /* MCU别名 */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", FILED_McuAlias );
            bResult = FALSE;
        }
        else
        {
           atMcuTopo[dwLoop].SetAlias( pchToken );
        }
    }
	dwMcuNum = dwLoop;

    /* free memory */
    for( dwLoop = 0; dwLoop < dwMemEntryNum; dwLoop++ )
    {
		free( lpszTable[dwLoop] );
    }
	free( lpszTable );

	return( ( u8 )dwMcuNum );
}

/*====================================================================
    函数名      ：ReadMtTopoTable
    功能        ：读入MT拓扑信息
    算法实现    ：
    引用全局变量：无
    输入参数说明：LPCSTR lpszProfileName, 文件名（含绝对路径）
                  TMtTopo atMtTopoBuf[], Mt信息BUFFER
				  u16 wBufSize, BUFFER大小
    返回值说明  ：拓扑中Mt实际数目
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	02/10/24    1.0         liaoweijiang  创建
====================================================================*/
u16 ReadMtTopoTable( LPCSTR lpszProfileName, TMtTopo atMtTopo[], u16 wBufSize )
{
	LPSTR   *lpszTable;
    char    chSeps[] = " \t";       /* space or tab as seperators */
    char    *pchToken;
    u32		dwLoop;
    BOOL    bResult = TRUE;
    u32		dwMemEntryNum;
	u32		dwMtNum;

    ASSERT( atMtTopo != NULL );
    
    /* get the number of entry */
    bResult = GetRegKeyInt( lpszProfileName, SECTION_mtTopoTable, STR_ENTRY_NUM, 
                0, ( int * )&dwMemEntryNum );
    if (!bResult)
    {
        printf("[ReadMtTopoTable] GetRegKeyInt failed with lpszProName.%s!\n", lpszProfileName);
        return 0;
    }
	lpszTable = (LPSTR *)malloc( dwMemEntryNum * sizeof( LPSTR ) );
	dwMemEntryNum = min( dwMemEntryNum, wBufSize );
    for( dwLoop = 0; dwLoop < dwMemEntryNum; dwLoop++ )
    {
		lpszTable[dwLoop] = (char *)malloc( MAX_VALUE_LEN + 1 );
    }
    dwMtNum = dwMemEntryNum;

    printf("[ReadMtTopoTable] mtNum.%d\n", dwMtNum);

	/* get entry strings */
    bResult = GetRegKeyStringTable( lpszProfileName, SECTION_mtTopoTable,
                   "fail", lpszTable, &dwMtNum, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )
    {
        dwMtNum = 0;
    }
    printf("[ReadMtTopoTable] mtNum.%d, getStrRet.%d\n", dwMtNum, bResult);

    
	/* analyze entry strings */
    for( dwLoop = 0; dwLoop < dwMtNum && dwLoop < wBufSize; dwLoop++ )
    {
        /* MtId  */
        pchToken = strtok( lpszTable[dwLoop], chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", FILED_MtId );
            bResult = FALSE;
        }
        else
        {
            atMtTopo[dwLoop].SetMtId( atoi( pchToken ) );
        }
        /* McuId */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", FILED_McuId );
            bResult = FALSE;
        }
        else
        {
           atMtTopo[dwLoop].SetMcuId( atoi( pchToken ) );
        }
        /* Mt的IP地址 */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", FILED_MtIpAddr );
            bResult = FALSE;
        }
        else
        {
           atMtTopo[dwLoop].SetIpAddr( ntohl(inet_addr( pchToken )) );
        }
        /* Mt的网关IP地址 */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", FILED_MtGwIpAddr );
            bResult = FALSE;
        }
        else
        {
           atMtTopo[dwLoop].SetGwIpAddr( ntohl(inet_addr( pchToken )) );
        }
        /* Mt别名 */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", FILED_MtAlias );
            bResult = FALSE;
        }
        else
        {
           atMtTopo[dwLoop].SetAlias( pchToken );
        }
         /* 终端类型 */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", FILED_MtType );
            bResult = FALSE;
        }
        else
        {
           atMtTopo[dwLoop].SetMtType( atoi(pchToken) );
        }
		
		pchToken = strtok( NULL, chSeps );
		if ( pchToken == NULL )
		{
			printf( "Wrong profile while reading %s\n!", FILED_MtConnected );
            bResult = FALSE;
		}
		else
		{
			atMtTopo[dwLoop].SetConnected( atoi(pchToken) );
		}
		/* Mt的MODEM IP地址 */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading Modem Ip\n!");
            bResult = FALSE;
        }
        else
        {
			atMtTopo[dwLoop].m_dwMtModemIp= inet_addr( pchToken );
        }
		/* Mt的MODEM 端口 */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading Modem Port\n!" );
            bResult = FALSE;
        }
        else
        {
			atMtTopo[dwLoop].m_wMtModemPort = htons( atoi( pchToken ) );
        }
		
		pchToken = strtok( NULL, chSeps );
		if ( pchToken == NULL )
		{
			printf( "Wrong profile while reading Modem Type\n!" );
            bResult = FALSE;
		}
		else
		{
			atMtTopo[dwLoop].m_byMtModemType = atoi(pchToken);
		}

		/* Mt 是否高清终端*/
		pchToken = strtok( NULL, chSeps );
		if ( NULL == pchToken)
		{
			printf( "Wrong profile while reading MT high defection\n!" );
			bResult = FALSE;
		}
		else
		{
			BOOL bMtHd = atoi(pchToken) != 0 ? TRUE : FALSE;
			atMtTopo[dwLoop].SetMtHd(bMtHd);
		}

    }
	dwMtNum = dwLoop;

    /* free memory */
    for( dwLoop = 0; dwLoop < dwMemEntryNum; dwLoop++ )
    {
		free( lpszTable[dwLoop] );
    }

	free( lpszTable );

	return( ( u16 )dwMtNum );
}

/*====================================================================
    函数名      :ReadMcuModemTable
    功能        ：读入MCU Modem拓扑信息
    算法实现    ：
    引用全局变量：无
    输入参数说明：LPCSTR lpszProfileName, 文件名（含绝对路径）
                  TMtTopo atMtTopoBuf[], Mt信息BUFFER
				  u16 wBufSize, BUFFER大小
    返回值说明  ：拓扑中Mt实际数目
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	02/10/24    1.0         liaoweijiang  创建
====================================================================*/
u8 ReadMcuModemTable( LPCSTR lpszProfileName, TMcuModemTopo atMcuModemTopo[], u16 wBufSize )
{
	LPSTR   *lpszTable;
    char    chSeps[] = " \t";       /* space or tab as seperators */
    char    *pchToken;
    u32		dwLoop;
    BOOL    bResult = TRUE;
    u32		dwMemEntryNum;
	u32		dwMtNum;

    ASSERT( atMcuModemTopo != NULL );
    
    /* get the number of entry */
    bResult = GetRegKeyInt( lpszProfileName, "mcuModemTable", STR_ENTRY_NUM, 
                0, ( int * )&dwMemEntryNum );

	lpszTable = (LPSTR *)malloc( dwMemEntryNum * sizeof( LPSTR ) );
	dwMemEntryNum = min( dwMemEntryNum, wBufSize );
    for( dwLoop = 0; dwLoop < dwMemEntryNum; dwLoop++ )
    {
		lpszTable[dwLoop] = (char *)malloc( MAX_VALUE_LEN + 1 );
    }
    dwMtNum = dwMemEntryNum;

	/* get entry strings */
    bResult = GetRegKeyStringTable( lpszProfileName, "mcuModemTable",
                   "fail", lpszTable, &dwMtNum, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )
    {
        dwMtNum = 0;
    }
    
	/* analyze entry strings */
    for( dwLoop = 0; dwLoop < dwMtNum && dwLoop < wBufSize; dwLoop++ )
    {
        /* ModemId  */
        pchToken = strtok( lpszTable[dwLoop], chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", "Mcu Modem Id" );
            bResult = FALSE;
        }
        else
        {
            atMcuModemTopo[dwLoop].m_byModemId = ( atoi( pchToken ) );
        }
        /* IP*/
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", "Mcu Modem Ip" );
            bResult = FALSE;
        }
        else
        {
           atMcuModemTopo[dwLoop].m_dwMcuModemIp = inet_addr( pchToken );
        }
        /* Port */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", "Mcu Modem Port" );
            bResult = FALSE;
        }
        else
        {
           atMcuModemTopo[dwLoop].m_wMcuModemPort = htons( atoi( pchToken ) );
        }
        /* Modem Type */
        pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
            printf( "Wrong profile while reading %s\n!", "Mcu Modem type" );
            bResult = FALSE;
        }
        else
        {
           atMcuModemTopo[dwLoop].m_byMcuModemType = atoi( pchToken );
        }
		//S port
		pchToken = strtok( NULL, chSeps );
        if( pchToken == NULL )
        {
           atMcuModemTopo[dwLoop].m_bySPortNum = (u8)dwLoop+1;
        }
        else
        {
           atMcuModemTopo[dwLoop].m_bySPortNum = atoi( pchToken );
        }
    }
	dwMtNum = dwLoop;

    /* free memory */
    for( dwLoop = 0; dwLoop < dwMemEntryNum; dwLoop++ )
    {
		free( lpszTable[dwLoop] );
    }

	free( lpszTable );

	return( ( u8 )dwMtNum );
}


u8 GetSatMediaType(u8 byType)
{
	u8 bySatType = 0;
	switch (byType)
	{
	case MEDIA_TYPE_PCMU:	bySatType = SAT_AUDIO_G711U;	break;
	case MEDIA_TYPE_PCMA:	bySatType = SAT_AUDIO_G711A;	break;
	case MEDIA_TYPE_G722:	bySatType = SAT_AUDIO_G722;		break;
	case MEDIA_TYPE_G7231:	bySatType = SAT_AUDIO_G7231;		break;
	case MEDIA_TYPE_G728:	bySatType = SAT_AUDIO_G728;		break;
	case MEDIA_TYPE_G729:	bySatType = SAT_AUDIO_G729;		break;
	case MEDIA_TYPE_MP3:	bySatType = SAT_AUDIO_MP3;		break;

	case MEDIA_TYPE_H261:	bySatType = SAT_VIDEO_H261;		break;
	case MEDIA_TYPE_H263:	bySatType = SAT_VIDEO_H263;		break;
	case MEDIA_TYPE_H264:	bySatType = SAT_VIDEO_H264;		break;
	case MEDIA_TYPE_H262:	bySatType = SAT_VIDEO_MPEG2;		break;
	case MEDIA_TYPE_MP4:	bySatType = SAT_VIDEO_MPEG4;		break;
	default:
		OspPrintf(TRUE, FALSE, "[GetSatMediaType] unexpected type.%d, ignore it!\n", byType);
		break;
	}

	return bySatType;
}

u8 GetSatRes(u8 byRes)
{
	u8 bySatRes = TYPE_CIF;
	switch (byRes)
	{
	case VIDEO_FORMAT_CIF:	bySatRes = TYPE_CIF;	break;
	case VIDEO_FORMAT_2CIF:	bySatRes = TYPE_2CIF;	break;
	case VIDEO_FORMAT_4CIF:	bySatRes = TYPE_4CIF;	break;
	case VIDEO_FORMAT_16CIF:	bySatRes = TYPE_16CIF;	break;
	case VIDEO_FORMAT_QCIF:		bySatRes = TYPE_QCIF;	break;
	case VIDEO_FORMAT_SQCIF:	bySatRes = TYPE_SQCIF;	break;
	}

	return bySatRes;
}

//END OF FILE
