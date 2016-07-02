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

//#include "evmcumcs.h"
//#include "mcuutility.h"
#include "eqplib.h"
#include "bindmp.h"
#include "bindmtadp.h"
#include "mcupfmlmt.h"


#include<sys/types.h>   
#include<sys/stat.h>   

#define F_WHNONELARGER			-2	// none larger in both
#define F_HLARGER				-1	// only larger in height
#define WHEQUAL					0	// equal
#define F_WLARGER				1	// only larger in width
#define F_WHLARGER				2	// larger in both
//#define F_NOMATCH				3	// no match

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

/*lint -save -e1536*/

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
/*
void TConfPollParam::InitPollParam(u8 byMtNum, TMtPollParam* ptParam)
{
    SetPollList(byMtNum, ptParam) ;

    m_byCurPollPos = 0;
    ClearSpecPos();
}
*/
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
/*
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
*/
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
/*
u8 TConfPollParam::GetPolledMtNum() const
{
    return m_byPollMtNum;
}
*/
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
/*
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
*/
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
/*
void TConfPollParam::SetCurrentIdx(u8 byIdx)
{
    if ( byIdx < m_byPollMtNum )
    {
        m_byCurPollPos = byIdx;
    }
}
*/

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
/*
u8 TConfPollParam::GetCurrentIdx() const
{
    return m_byCurPollPos;
}
*/
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
/*
TMtPollParam* const TConfPollParam::GetCurrentMtPolled()
{
    return &m_atMtPollParam[m_byCurPollPos];
}
*/
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
/*
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
*/
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
/*
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
*/
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
/*
TEqp TTvWallPollParam::GetTvWallEqp( void )
{
	return m_tTvWall;
}
*/
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
/*
void TTvWallPollParam::SetTvWallEqp( const TEqp &tTvWall )
{
	m_tTvWall = tTvWall;
}
*/
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
/*
u8 TTvWallPollParam::GetTvWallChnnl( void )
{
	return m_byTvWallChnnl;
}
*/
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
/*
void TTvWallPollParam::SetTvWallChnnl( u8 byChnIndex )
{
	m_byTvWallChnnl = byChnIndex;
}
*/
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
		LogPrint( LOG_LVL_ERROR, MID_MCU_MT, "Exception - TConfMtData::SetMtLogicChnnl(): wrong byChannel %u!\n", byChannel );
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
    10/01/18	4,6			薛亮			create
====================================================================*/
/*BOOL32 TConfMtTable::GetMtLogicChnnl( const TMt &tMt, u8 byChannel, TLogicalChannel * ptChnnl, BOOL32 bForwardChnnl )
{
	if( tMt.IsNull() )
	{
		return FALSE;
	}

	//zjj20100720 三级级联后getmcuid无法标示mcuid了，所以要保证传进参数就是本地终端
	if( !tMt.IsLocal() )
	{
		return FALSE;
	}

	u8 byMtOrMcuId =  tMt.GetMtId() ;
	return GetMtLogicChnnl( byMtOrMcuId, byChannel, ptChnnl, bForwardChnnl);

	return FALSE;
}*/
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
		LogPrint( LOG_LVL_ERROR, MID_MCU_MT, "Exception - TConfMtTable::GetMtLogicChnnl(): wrong byChannel %u!\n", byChannel );
		return FALSE;
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

	dwDstIp = 0;
	wDstPort = 0;

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
		tSimCapSet.SetVideoMaxBitRate( m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel.GetFlowControl() );
		if (MEDIA_TYPE_H264 == tSimCapSet.GetVideoMediaType())
		{
			tSimCapSet.SetVideoProfileType(m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel.GetProfileAttrb());
			tSimCapSet.SetUserDefFrameRate(m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel.GetChanVidFPS());
		}
		else
		{
			tSimCapSet.SetVideoFrameRate(m_atMtData[byMtId - 1].m_tRvsChannel.m_tPrimaryVideoChannel.GetChanVidFPS());
		}
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
		tSimCapSet.SetVideoMaxBitRate( m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel.GetFlowControl() );
		if (MEDIA_TYPE_H264 == tSimCapSet.GetVideoMediaType())
		{
			tSimCapSet.SetVideoProfileType( m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel.GetProfileAttrb() );
			tSimCapSet.SetUserDefFrameRate(m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel.GetChanVidFPS());
		}
		else
		{
			tSimCapSet.SetVideoFrameRate(m_atMtData[byMtId - 1].m_tFwdChannel.m_tPrimaryVideoChannel.GetChanVidFPS());
		}
	}

	return tSimCapSet;
}

/*====================================================================
    函数名       GetSrcDSSCS
    功能        ：得到终端双流发送能力
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    20110408    4.6         周嘉麟         创建
====================================================================*/
TSimCapSet TConfMtTable::GetSrcDSSCS( u8 byMtId )
{
	TSimCapSet tSimCapSet;
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return tSimCapSet;
	}

	if( m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel.IsOpened() )
	{
		tSimCapSet.SetVideoMediaType( m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel.GetChannelType() );
        tSimCapSet.SetVideoResolution( m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel.GetVideoFormat() );
		tSimCapSet.SetVideoMaxBitRate( m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel.GetFlowControl() );
		if (MEDIA_TYPE_H264 == tSimCapSet.GetVideoMediaType())
		{
			tSimCapSet.SetVideoProfileType(m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel.GetProfileAttrb());
			tSimCapSet.SetUserDefFrameRate(m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel.GetChanVidFPS());
		}
		else
		{
			tSimCapSet.SetVideoFrameRate(m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel.GetChanVidFPS());
		}
	}

	return tSimCapSet;
}
/*====================================================================
    函数名         GetDSSCS
    功能        ：得到目的终端的双流接收能力
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明    TSimCapSet
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    20110408    4.6         周嘉麟         创建
====================================================================*/
TSimCapSet TConfMtTable::GetDstDSSCS(u8 byMtId)
{
	TSimCapSet tSimCapSet;
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return tSimCapSet;
	}
	
	if( m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel.IsOpened() )
	{
		tSimCapSet.SetVideoMediaType( m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel.GetChannelType() );
        tSimCapSet.SetVideoResolution( m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel.GetVideoFormat() );
		tSimCapSet.SetVideoMaxBitRate( m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel.GetFlowControl() );
		if (MEDIA_TYPE_H264 == tSimCapSet.GetVideoMediaType())
		{
			tSimCapSet.SetVideoProfileType(m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel.GetProfileAttrb());
			tSimCapSet.SetUserDefFrameRate(m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel.GetChanVidFPS());
		}
		else
		{
			tSimCapSet.SetVideoFrameRate(m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel.GetChanVidFPS());
		}
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

	case LOGCHL_SECVIDEO:
		return 	bForwardChnnl ? m_atMtData[byMtId - 1].m_tFwdChannel.m_tSecondVideoChannel.IsOpened()
			                  : m_atMtData[byMtId - 1].m_tRvsChannel.m_tSecondVideoChannel.IsOpened();

	case LOGCHL_AUDIO:
		return 	bForwardChnnl ? m_atMtData[byMtId - 1].m_tFwdChannel.m_tAudioChannel.IsOpened()
			                  : m_atMtData[byMtId - 1].m_tRvsChannel.m_tAudioChannel.IsOpened();

	case LOGCHL_T120DATA:
		return 	bForwardChnnl ? m_atMtData[byMtId - 1].m_tFwdChannel.m_tT120DataChannel.IsOpened()
			                  : m_atMtData[byMtId - 1].m_tRvsChannel.m_tT120DataChannel.IsOpened();

	case LOGCHL_H224DATA:
		return 	bForwardChnnl ? m_atMtData[byMtId - 1].m_tFwdChannel.m_tH224DataChannel.IsOpened()
			                  : m_atMtData[byMtId - 1].m_tRvsChannel.m_tH224DataChannel.IsOpened();
		
	default:
		return FALSE;
	}
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
    
//  	m_atMtData[byMtId - 1].m_tCapSupport = *ptCapSupport;
    memcpy(&m_atMtData[byMtId - 1].m_tCapSupport, ptCapSupport, sizeof(TCapSupport));

	return;
}

/*====================================================================
    函数名      ：SetMtMultiCapSupport
    功能        ：设置终端多组能力集
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
				  const TMultiCapSupport * ptCapSupport, 终端多组能力集
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    06/14/11    3.0         朱胜泽         创建
====================================================================*/
void TConfMtTable::SetMtMultiCapSupport( u8 byMtId, const TMultiCapSupport * ptMultiCapSupport )
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptMultiCapSupport == NULL )
	{
		return ;
	}
    
 	m_atMtData[byMtId - 1].m_tCapSupport = *ptMultiCapSupport;

	return;
}
/*====================================================================
    函数名      ：GetMtMultiCapSupport
    功能        ：获得终端多组能力集
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
				  TMultiCapSupport * ptCapSupport, 终端多组能力集
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    06/18/11    3.0         朱胜泽         创建
====================================================================*/
BOOL32 TConfMtTable::GetMtMultiCapSupport(u8 byMtId, TMultiCapSupport * ptMultiCapSupport)
{
    if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptMultiCapSupport == NULL )
    {
        return FALSE;
	}

    *ptMultiCapSupport = m_atMtData[byMtId - 1].m_tCapSupport;

    return TRUE;
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
	
// 	*ptCapSupport = m_atMtData[byMtId - 1].m_tCapSupport;
    memcpy(ptCapSupport, &m_atMtData[byMtId - 1].m_tCapSupport, sizeof(TCapSupport));
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
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
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

	TMtAlias tPuAlias;

	// PU特殊别名IP+ALIAS
	s8* pbyAlias = (s8*)(ptAlias->m_achAlias);
	m_atMtExt[byMtId - 1].SetIPAddr(ntohl(*(u32*)pbyAlias));
	tPuAlias.m_tTransportAddr.SetIpAddr(ntohl(*(u32*)pbyAlias));

	pbyAlias += sizeof(u32);
	m_atMtExt[byMtId - 1].SetAlias( pbyAlias );
    m_atMtExt[byMtId - 1].SetProtocolType(PROTOCOL_TYPE_H323);
	tPuAlias.SetH323Alias(pbyAlias);
		
	m_atMtData[byMtId - 1].m_atMtAlias[tPuAlias.m_AliasType - 1] = tPuAlias;
	
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

// 忽视pclint的错误:Warning -- Expected unsigned type
/*lint -save -e502*/

	m_atMtData[byMtId - 1].m_byMonitorDstMt[(byDstMtId-1) / 8] &= ~( 1 << ( (byDstMtId-1) % 8 ) );
/*lint -restore*/

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
        LogPrint( LOG_LVL_ERROR, MID_MCU_MT, "[GetMtSndBitrate] get mt sending bitrate error!!!\n");
        return 0;
    }
}

/*====================================================================
    函数名      : GetDisconnetReason
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  : LPCSTR
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/09/12    4.5         张宝卿           创建
====================================================================*/
u8 TConfMtTable::GetDisconnectReason(u8 byMtId)
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return 0;
    }
    return m_atMtData[byMtId-1].m_byDisconnetReason;    
}

/*====================================================================
    函数名      : SetDisconnectReason
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  : LPCSTR
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/09/12    4.5         张宝卿           创建
====================================================================*/
void TConfMtTable::SetDisconnectReason(u8 byMtId,u8 byReason)
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return;
    }
    m_atMtData[byMtId-1].m_byDisconnetReason = byReason;    
}

/*====================================================================
    函数名      ：SetRecordName
    功能        ：设置vrs录像文件名
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端号
				  LPCSTR lpszName
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/10/10    4.7         杨怀志         创建
====================================================================*/
void TConfMtTable::SetRecordName( u8 byMtId, LPCSTR lpszName )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || NULL == lpszName)
	{
		return;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		m_atVrsRecChlStatus[byVrsIdx].SetRecordName( lpszName );
	}
	return;
}

/*====================================================================
    函数名      ：GetRecordName
    功能        ：得到vrs录像文件名
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/10/10    4.7         杨怀志         创建
====================================================================*/
BOOL32 TConfMtTable::GetRecordName( u8 byMtId, char *szAlias, u16 wBufLen )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		m_atVrsRecChlStatus[byVrsIdx].GetRecordName(szAlias, wBufLen);
		return TRUE;
	}
	return FALSE;
}

/*====================================================================
    函数名      ：SetRecChlType
    功能        ：设置vrs录像通道类型
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端号
				  u8 byRecChlType
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/10/10    4.7         杨怀志         创建
====================================================================*/
void TConfMtTable::SetRecChlType( u8 byMtId, u8 byRecChlType )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		m_atVrsRecChlStatus[byVrsIdx].m_byType = byRecChlType;
	}
	return;
}

/*====================================================================
    函数名      ：GetRecChlType
    功能        ：得到vrs录像通道类型
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/10/10    4.7         杨怀志         创建
====================================================================*/
u8 TConfMtTable::GetRecChlType( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		return m_atVrsRecChlStatus[byVrsIdx].m_byType;
	}
	return 0;
}

/*====================================================================
    函数名      ：SetRecChlState
    功能        ：设置vrs录像通道状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端号
				  u8 byRecChlState
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/10/10    4.7         杨怀志         创建
====================================================================*/
void TConfMtTable::SetRecChlState( u8 byMtId, u8 byRecChlState )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		m_atVrsRecChlStatus[byVrsIdx].m_byState = byRecChlState;
	}
	return;
}

/*====================================================================
    函数名      ：GetRecChlState
    功能        ：得到vrs录像通道状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/10/10    4.7         杨怀志         创建
====================================================================*/
u8 TConfMtTable::GetRecChlState( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		return m_atVrsRecChlStatus[byVrsIdx].m_byState;
	}
	return 0;
}

/*====================================================================
    函数名      ：SetRecMode
    功能        ：设置vrs录像方式
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端号
				  u8 byRecMode
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/10/10    4.7         杨怀志         创建
====================================================================*/
void TConfMtTable::SetRecMode( u8 byMtId, u8 byRecMode )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		m_atVrsRecChlStatus[byVrsIdx].m_byRecMode = byRecMode;
	}
	return;
}

/*====================================================================
    函数名      ：GetRecMode
    功能        ：得到vrs录像方式
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/10/10    4.7         杨怀志         创建
====================================================================*/
u8 TConfMtTable::GetRecMode( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return 0;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		return m_atVrsRecChlStatus[byVrsIdx].m_byRecMode;
	}
	return 0;
}

/*====================================================================
    函数名      ：SetRecProg
    功能        ：设置vrs录像当前录像或放像进度
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端号
				  TRecProg tRecProg
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/10/10    4.7         杨怀志         创建
====================================================================*/
void TConfMtTable::SetRecProg( u8 byMtId, TRecProg tRecProg )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		m_atVrsRecChlStatus[byVrsIdx].m_tProg = tRecProg;
	}
	return;
}

/*====================================================================
    函数名      ：GetRecProg
    功能        ：得到vrs录像当前录像或放像进度
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/10/10    4.7         杨怀志         创建
====================================================================*/
TRecProg TConfMtTable::GetRecProg( u8 byMtId )
{
	TRecProg tProg;
	memset(&tProg, 0, sizeof(tProg));
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return tProg;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		return m_atVrsRecChlStatus[byVrsIdx].m_tProg;
	}
	return tProg;
}

/*====================================================================
    函数名      ：SetRecSrc
    功能        ：设置vrs录像源
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端号
				  TMtNoConstruct tSrc
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/10/10    4.7         杨怀志         创建
====================================================================*/
void TConfMtTable::SetRecSrc( u8 byMtId, TMtNoConstruct tSrc )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		m_atVrsRecChlStatus[byVrsIdx].m_tSrc = tSrc;
	}
	return;
}

/*====================================================================
    函数名      ：GetRecSrc
    功能        ：得到vrs录像源
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/10/10    4.7         杨怀志         创建
====================================================================*/
TMtNoConstruct TConfMtTable::GetRecSrc( u8 byMtId )
{
	TMtNoConstruct tMt;
	tMt.SetNull();
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return tMt;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		return m_atVrsRecChlStatus[byVrsIdx].m_tSrc;
	}

	return tMt;
}

/*====================================================================
    函数名      ：GetVrsChlIdxbyMtId
    功能        ：得到vrs录像通道全部状态信息
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：u8 索引号
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/10/10    4.7         杨怀志         创建
====================================================================*/
u8 TConfMtTable::GetVrsChlIdxbyMtId( u8 byMtId )
{
	u8 byRet = 0XFF;
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return byRet;
	}
	for (u8 byIdx=0; byIdx<MAX_PRIEQP_NUM; byIdx++)
	{
		if (m_atVrsRecChlStatus[byIdx].GetVrsMtId() == byMtId)
		{
			byRet = byIdx;
			break;
		}
	}

	return byRet;
}

/*====================================================================
    函数名      ：AddVrsRecChlbyMtId
    功能        ：增加一个新的vrs通道信息
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：BOOL32
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/10/10    4.7         杨怀志         创建
====================================================================*/
BOOL32 TConfMtTable::AddVrsRecChlbyMtId( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}
	if (GetVrsChlIdxbyMtId(byMtId) != 0XFF)
	{
		// 已存在，返回TRUE
		return TRUE;
	}
	
	// 分配一个空闲通道
	for (u8 byIdx=0; byIdx<MAX_PRIEQP_NUM; byIdx++)
	{
		if (m_atVrsRecChlStatus[byIdx].GetVrsMtId() == 0)
		{
			m_atVrsRecChlStatus[byIdx].SetVrsMtId(byMtId);
			return TRUE;
		}
	}
	
	return FALSE;
}

/*====================================================================
    函数名      ：GetRecChnnlStatus
    功能        ：得到vrs录像通道全部状态信息
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/10/10    4.7         杨怀志         创建
====================================================================*/
TRecChnnlStatus TConfMtTable::GetRecChnnlStatus( u8 byMtId )
{
	TRecChnnlStatus tStatus;
	memset(&tStatus, 0, sizeof(tStatus));
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return tStatus;
	}

	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		return m_atVrsRecChlStatus[byVrsIdx];
	}

	return tStatus;
}

/*====================================================================
    函数名      ：ResetRecChnnlStatus
    功能        ：重置vrs对应recChnnlStatus通道信息
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/10/10    4.7         杨怀志         创建
====================================================================*/
void TConfMtTable::ResetRecChnnlStatus( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return ;
	}
	u8 byVrsIdx = GetVrsChlIdxbyMtId(byMtId);
	if (byVrsIdx != 0XFF)
	{
		m_atVrsRecChlStatus[byVrsIdx].Clear();
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
void TConfMtTable::SetMcuId( u8 byMtId, u16 wMcuId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	m_atMtExt[byMtId - 1].SetMcuIdx( wMcuId );

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
u16 TConfMtTable::GetMcuId( u8 byMtId )
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
	return m_atMtExt[byMtId - 1].GetManuId();
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
	     
		++byMtId;
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

	TMtAlias tTempAlias;
	if(ptAlias->m_AliasType == puAliasTypeIPPlusAlias)
	{
		// PU特殊别名IP+ALIAS
		tTempAlias.m_AliasType = puAliasTypeIPPlusAlias;
		
		s8* pbyAlias = (s8*)(ptAlias->m_achAlias);
		tTempAlias.m_tTransportAddr.SetIpAddr(ntohl(*(u32*)pbyAlias));
		
		pbyAlias += sizeof(u32);

		u32 dwAliasActualSize = strlen(pbyAlias);
		//u32 dwMaxAliasSize = sizeof(tTempAlias.m_achAlias);
		if( dwAliasActualSize > sizeof(tTempAlias.m_achAlias) )
		{
			dwAliasActualSize = sizeof(tTempAlias.m_achAlias);
		}
		memcpy(tTempAlias.m_achAlias,pbyAlias,dwAliasActualSize);
	}
	else
	{
		tTempAlias = *ptAlias;
	}
	
	u8 byMtId = GetMtIdByAlias( &tTempAlias );

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
		byMtId > MAXNUM_CONF_MT || byMtId == 0 )
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
		byMtId > MAXNUM_CONF_MT || byMtId == 0 )
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
		byMtId > MAXNUM_CONF_MT || byMtId == 0 )
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
    if( byMtId > MAXNUM_CONF_MT || byMtId == 0 )
	{
		return 0;
	}

	u16 wBitRate = 0;
	if( m_atMtData[byMtId - 1].m_tRvsChannel.m_tAudioChannel.IsOpened() )
	{
		wBitRate = m_atMtExt[byMtId - 1].GetDialBitRate() - GetAudioBitrate(m_atMtData[byMtId - 1].m_tRvsChannel.m_tAudioChannel.GetChannelType()) ;
	}
	else
	{
		wBitRate = m_atMtExt[byMtId - 1].GetDialBitRate();
	}

	return wBitRate;
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
    if( byMtId > MAXNUM_CONF_MT || byMtId == 0 )
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
    if( byMtId > MAXNUM_CONF_MT || byMtId == 0 )
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
    if (byMtId > MAXNUM_CONF_MT || byMtId == 0)
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
    if (byMtId > MAXNUM_CONF_MT || byMtId == 0)
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
    函数名      ：GetVrsRecMtIdByAlias
    功能        ：根据别名等信息查找vrs新录播实体Id
    算法实现    ：
    引用全局变量：
    输入参数说明：TMtAlias tMtAlias, vrs服务器别名
				  u8 byType 录放像模式
				  TMt tSrc  源终端
    返回值说明  ：0 - 不存在IP为输入值的终端
	              大于0 - 终端Id
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/05    3.0         胡昌威         创建
====================================================================*/
u8 TConfMtTable::GetVrsRecMtIdByAlias( const TMtAlias * ptAlias , u8 byType, TMt tSrc)
{
    if( ptAlias == NULL || ptAlias->m_AliasType == 0 ||
		(ptAlias->m_AliasType > mtAliasTypeOthers && 
		ptAlias->m_AliasType != puAliasTypeIPPlusAlias))
	{
		return 0;
	}
#ifdef _8KI_
    u8 byMtId = 1;
	
	//查找
	while( byMtId <= m_byMaxNumInUse )
	{
		if( m_atMtExt[byMtId - 1].GetMtId() == byMtId )
		{
            // 介于终端呼入端口的不确定性，Ip别名不再比较端口
            if ( mtAliasTypeTransportAddress == ptAlias->m_AliasType )
            {
                if ( ptAlias->m_tTransportAddr.GetIpAddr() == 
					m_atMtData[byMtId-1].m_atMtAlias[ptAlias->m_AliasType-1].GetMtAlias().m_tTransportAddr.GetIpAddr() )
                {
					if ( m_atVrsRecChlStatus[GetVrsChlIdxbyMtId(byMtId)].m_byType == TRecChnnlStatus::TYPE_UNUSE)
					{
						// 未使用通道可匹配
						return byMtId;
					}
					else if (m_atVrsRecChlStatus[GetVrsChlIdxbyMtId(byMtId)].m_byType == byType)
					{
						if (byType == TRecChnnlStatus::TYPE_PLAY)
						{
							// 匹配会议放像
							return byMtId;
						}
						else if (byType == TRecChnnlStatus::TYPE_RECORD)
						{
							if ( m_atVrsRecChlStatus[GetVrsChlIdxbyMtId(byMtId)].m_tSrc.IsNull() && tSrc.IsNull())
							{
								//匹配会议录像
								return byMtId;
							}
							else if (m_atVrsRecChlStatus[GetVrsChlIdxbyMtId(byMtId)].m_tSrc == tSrc)
							{
								//已使用通道严格匹配录放像模式和源终端
								return byMtId;
							}
						}
					}
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
#endif
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
void TConfMtTable::SetMtRecording( u8 byMtId )
{
	if( byMtId != 0 && byMtId <= MAXNUM_CONF_MT )
	{
		//lukunpeng 2010/07/14 删除录像机信息，由外设和终端状态共同保存
		/*
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState.SetRecording();
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_byRecChannel = byRecChannel;
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecEqp = tRecEqp;

		//同步终端的录像状态
		m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState;
		*/

		m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState.SetRecording();
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

//lukunpeng 2010/07/14 删除录像机信息，由外设和终端状态共同保存
/*
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
*/

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

//lukunpeng 2010/07/14 删除录像机信息，由外设和终端状态共同保存
/*
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
*/
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
		//lukunpeng 2010/07/14 删除录像机信息，由外设和终端状态共同保存
		/*
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState.SetNoRecording();
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_byRecChannel = 0;
		memset( &m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecEqp , 0, sizeof( TEqp ) );
		//同步终端的录像状态
		m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState;
		*/
		m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState.SetNoRecording();

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
		//lukunpeng 2010/07/14 删除录像机信息，由外设和终端状态共同保存
		/*
		m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState.SetRecPause();

		//同步终端的录像状态
		m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState;
		*/
		m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState.SetRecPause();
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
	
	//lukunpeng 2010/07/14 删除录像机信息，由外设和终端状态共同保存
	//*ptRecState = m_atMtData[ byMtId -1 ].m_tMtRecInfo.m_tRecState;
	//同步终端的录像状态
	//m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState;
	*ptRecState = m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState;
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

	//lukunpeng 2010/07/14 删除录像机信息，由外设和终端状态共同保存
	//return m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecState.IsRecording();

	return m_atMtData[ byMtId - 1].m_tMtStatus.m_tRecState.IsRecording();
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

	//lukunpeng 2010/07/14 删除录像机信息，由外设和终端状态共同保存
	//return m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecState.IsRecPause();
	return m_atMtData[ byMtId - 1].m_tMtStatus.m_tRecState.IsRecPause();


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

	//lukunpeng 2010/07/14 删除录像机信息，由外设和终端状态共同保存
	//return m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecState.IsNoRecording();
	return m_atMtData[ byMtId - 1].m_tMtStatus.m_tRecState.IsNoRecording();
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

	//lukunpeng 2010/07/14 删除录像机信息，由外设和终端状态共同保存
	//return m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecState.IsRecSkipFrame();
	return m_atMtData[ byMtId - 1].m_tMtStatus.m_tRecState.IsRecSkipFrame();
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

	//lukunpeng 2010/07/14 删除录像机信息，由外设和终端状态共同保存
	/*
	m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecState.SetRecSkipFrame( bSkipFrame );
	//同步终端的录像状态
	m_atMtData[ byMtId - 1 ].m_tMtStatus.m_tRecState = m_atMtData[ byMtId - 1 ].m_tMtRecInfo.m_tRecState;
	*/
	m_atMtData[ byMtId - 1].m_tMtStatus.m_tRecState.SetRecSkipFrame( bSkipFrame );
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

//lukunpeng 2010/07/14 删除录像机信息，由外设和终端状态共同保存
/*
void TConfMtTable::SetMtRecProg( u8 byMtId, const TRecProg & tRecProg )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}
	
	m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecProg = tRecProg;

	return;
}
*/

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
//lukunpeng 2010/07/14 删除录像机信息，由外设和终端状态共同保存
/*
BOOL32 TConfMtTable::GetMtRecProg( u8 byMtId, TRecProg * ptRecProg )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT || ptRecProg == NULL )
		return FALSE;

	*ptRecProg = m_atMtData[ byMtId - 1].m_tMtRecInfo.m_tRecProg;
	return TRUE;	
}
*/

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

BOOL32 TConfMtTable::IsMtAutoInSpec( u8 byMtId )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return FALSE;
	}
	
	return (m_atMtData[byMtId - 1].m_bySpecInMix == 0) ? TRUE : FALSE;
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
void TConfMtTable::SetMtVideoRecv( u8 byMtId, BOOL32 bRecving, u16 wBitRate/* = 0*/ )
{
	if( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
	{
		return;
	}

	m_atMtData[ byMtId - 1].m_tMtStatus.SetReceiveVideo( bRecving );

	if( bRecving )
	{
		m_atMtData[ byMtId - 1].m_tMtStatus.SetRecvBitRate( wBitRate );
	}
	else
	{
		m_atMtData[ byMtId - 1].m_tMtStatus.SetRecvBitRate( 0 );
	}
	
	
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
void TConfMtTable::SetMtInMixing( u8 byMtId, BOOL32 bMtInMixing, BOOL32 bAutoInSpec )
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

	//lukp 调试时使用
	//LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "SetMtInMixing McuId: %d, MtId%d, bMix:%d, bAutoInSpec: %d\n", m_atMtData[ byMtId - 1].m_tMtStatus.GetMcuId(), byMtId, bMtInMixing, bAutoInSpec);

	//设置终端进定制混音状态时需要判断终端音频逻辑通道是否打开，如果没有打开，则不允许设置
	//Bug00105911_tzy 取消进混音状态时不需要判断终端逻辑通道是否打开，因为可能该异常终端掉线了，此时逻辑通道已经关闭，
	//停掉混音器时也需要清掉该终端进混音状态标识字段
	if (!m_atMtData[byMtId - 1].m_tRvsChannel.m_tAudioChannel.IsOpened() && bMtInMixing)
	{
		return;
	}
	m_atMtData[byMtId - 1].m_tMtStatus.SetInMixing( bMtInMixing );

	m_atMtData[byMtId - 1].m_bySpecInMix = bAutoInSpec ? 0 : 1;
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
    函数名      : GetProductId
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  : LPCSTR
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/09/12    4.5         张宝卿           创建
====================================================================*/
LPCSTR TConfMtTable::GetProductId(u8 byMtId) const
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return NULL;
    }
    return m_atMtExt2[byMtId-1].GetProductId();
}

/*====================================================================
    函数名      : SetProductId
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  : LPCSTR
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/09/12    4.5         张宝卿           创建
====================================================================*/
void TConfMtTable::SetProductId(u8 byMtId, LPCSTR lpszProductId)
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return;
    }
    m_atMtExt2[byMtId-1].SetProductId(lpszProductId);
    return;
}

/*====================================================================
    函数名      : GetVersionId
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  : LPCSTR
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/09/12    4.5         张宝卿           创建
====================================================================*/
LPCSTR TConfMtTable::GetVersionId(u8 byMtId) const
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return NULL;
    }
    return m_atMtExt2[byMtId-1].GetVersionId();
}

/*====================================================================
    函数名      : SetVersionId
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId, 终端号
    返回值说明  : LPCSTR
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    09/09/12    4.5         张宝卿           创建
====================================================================*/
void TConfMtTable::SetVersionId(u8 byMtId, LPCSTR lpszVersionId)
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return;
    }
    m_atMtExt2[byMtId-1].SetVersionId(lpszVersionId);
    return;
}

/*====================================================================
    函数名      : SetDisconnectDRI
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId
				  u8 byDRIId
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
====================================================================*/
void TConfMtTable::SetDisconnectDRI(u8 byMtId, u8 byDRIId)
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return;
    }
    m_atMtData[byMtId-1].m_byDisconnectDRI = byDRIId;
    return;
}

/*====================================================================
    函数名      : GetDisconnectDRI
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId
    返回值说明  : u8 
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
====================================================================*/
u8 TConfMtTable::GetDisconnectDRI(u8 byMtId)
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return 0;
    }
    return m_atMtData[byMtId-1].m_byDisconnectDRI;
}

/*====================================================================
    函数名      ：SetMtVidAlias
    功能        ：设置终端一个视频源的别名
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端Id
	              u8 byVidNum 视频源数量
	              const TVidSrcAliasInfo *ptVidAliasInfo 视频源别名信息
    返回值说明  ：成功TRUE,失败FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	20110412    3.0         彭杰           create
====================================================================*/
BOOL32 TConfMtTable::SetMtVidAlias( u8 byMtId, u8 byVidNum, const TVidSrcAliasInfo *ptVidAliasInfo )
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return FALSE;
    }

	for( u8 byLoop = 0; byLoop < MT_MAXNUM_VIDSOURCE; byLoop++ )
	{
		m_atMtData[byMtId-1].m_tVidAliasInfo[byLoop].Clean();
	}

	memcpy( m_atMtData[byMtId-1].m_tVidAliasInfo, ptVidAliasInfo, \
		min(byVidNum, MT_MAXNUM_VIDSOURCE) * sizeof(TVidSrcAliasInfo) );
	return TRUE;
}

/*====================================================================
    函数名      :InitMtVidAlias
    功能        ：初始化终端视频源别名信息
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端Id
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	20110412    3.0         彭杰           create
====================================================================*/
void TConfMtTable::InitMtVidAlias( u8 byMtId )
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return;
    }

	for( u8 byLoop = 0; byLoop < MT_MAXNUM_VIDSOURCE; byLoop++ )
	{
		m_atMtData[byMtId-1].m_tVidAliasInfo[byLoop].Clean();
	}
}

/*====================================================================
    函数名      ：GetMtSelMtByMode
    功能        ：根据mode取终端的选看源
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端Id
				  u8 byMode 模式
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	2012/09/13  4.7         zhangli       create
====================================================================*/
TMt TConfMtTable::GetMtSelMtByMode(u8 byMtId, u8 byMode)
{
	TMt tMt;
	if(byMtId == 0 || byMtId > MAXNUM_CONF_MT)
	{
		return tMt;
	}
	if (MODE_AUDIO != byMode && MODE_VIDEO != byMode && MODE_VIDEO2SECOND != byMode)
	{
		return tMt;
	}
	
	if (MODE_VIDEO2SECOND == byMode)
	{
		tMt = m_atMtData[byMtId-1].m_tSecSelSrc;
	}
	else if (MODE_AUDIO == byMode || MODE_VIDEO == byMode)
	{
		TMtStatus tMtStatus;
		if (GetMtStatus(byMtId, &tMtStatus))
		{
			tMt = tMtStatus.GetSelectMt(byMode);
		}
	}
	return tMt;
}

/*====================================================================
    函数名      ：SetMtSelMtByMode
    功能        ：根据mode设置终端的选看源
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端Id
				  u8 byMode 模式
				  u8 bySrcId 源终端ID
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	2012/09/13  4.7         zhangli       create
====================================================================*/
void TConfMtTable::SetMtSelMtByMode(u8 byMtId, u8 byMode, TMt &tSrcMt)
{
	if(byMtId == 0 || byMtId > MAXNUM_CONF_MT)
	{
		return;
	}
	if (MODE_AUDIO != byMode && MODE_VIDEO != byMode && MODE_VIDEO2SECOND != byMode && MODE_BOTH != byMode)
	{
		return;
	}
	
	TMtStatus tMtStatus;
	if (!GetMtStatus(byMtId, &tMtStatus))
	{
		return;
	}
	
	if (MODE_VIDEO2SECOND == byMode)
	{
		m_atMtData[byMtId-1].m_tSecSelSrc = tSrcMt;
	}
	else
	{	
		tMtStatus.SetSelectMt(tSrcMt, byMode);
	}
	
	SetMtStatus(byMtId, &tMtStatus);
}

/*====================================================================
    函数名      ：RemoveMtSelMtByMode
    功能        ：根据mode清除终端的选看源
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端Id
				  u8 byMode 模式
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	2012/09/13  4.7         zhangli       create
====================================================================*/
void TConfMtTable::RemoveMtSelMtByMode(u8 byMtId, u8 byMode)
{
	if(byMtId == 0 || byMtId > MAXNUM_CONF_MT)
	{
		return;
	}
	if (MODE_AUDIO != byMode && MODE_VIDEO != byMode && MODE_VIDEO2SECOND != byMode && MODE_BOTH != byMode)
	{
		return;
	}
	
	TMtStatus tMtStatus;
	if (!GetMtStatus(byMtId, &tMtStatus))
	{
		return;
	}
	
	tMtStatus.RemoveSelByMcsMode(byMode);

	TMt tNullMt;
	
	if (MODE_VIDEO2SECOND == byMode)
	{
		m_atMtData[byMtId-1].m_tSecSelSrc = tNullMt;
	}
	else
	{	
		tMtStatus.SetSelectMt(tNullMt, byMode);
	}
	
	SetMtStatus(byMtId, &tMtStatus);
}
/*====================================================================
    函数名      ：GetMtVidPortNum
    功能        ：得到终端的视频源个数
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端Id
    返回值说明  ：终端视频源个数
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	20110412    3.0         彭杰           create
====================================================================*/
u8 TConfMtTable::GetMtVidPortNum( u8 byMtId )
{
    if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT )
    {
        return 0;
    }

	u8 byMtVidPortNum = 0;
	for( u8 byLoop = 0; byLoop < MT_MAXNUM_VIDSOURCE; byLoop++ )
	{
		if( m_atMtData[byMtId-1].m_tVidAliasInfo[byLoop].byVidPortIdx != INVALID_MTVIDSRCPORTIDX )
		{
			byMtVidPortNum++;
		}
	}
	
	return byMtVidPortNum;
}

/*====================================================================
    函数名      ：GetMtVidAliasbyVidIdx
    功能        ：得到终端的一个视频源别名信息
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 byMtId 终端Id
    返回值说明  ：终端视频源别名信息
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	20110412    3.0         彭杰           create
====================================================================*/
TVidSrcAliasInfo TConfMtTable::GetMtVidAliasbyVidIdx( u8 byMtId, u8 byIdx )
{
	if ( byMtId == 0 || byMtId > MAXNUM_CONF_MT || byIdx >= MT_MAXNUM_VIDSOURCE )
    {
		TVidSrcAliasInfo tVidSrcAliasInfo;
        return tVidSrcAliasInfo;
    }

	return m_atMtData[byMtId-1].m_tVidAliasInfo[byIdx];
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
		LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[AddSwitchTable] invalid byMpId<%d> received!!!\n", byMpId);
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
		LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[RemoveSwitchTable] invalid byMpId<%d> received!!!\n", byMpId);
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

/*=============================================================================
函 数 名： ClearRtcpSwitchTable
功    能： 清除byMpId转发板上的RTCP交换
算法实现： 
全局变量： 
参    数： u8 byMpId
			u32 dwMpIp
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2011/08/30  4.6			zhangli                  创建
=============================================================================*/
void TConfSwitchTable::ClearRtcpSwitchTable(u8 byMpId, u32 dwMpIp)
{
	
	if (0 == byMpId || byMpId > MAXNUM_DRI || dwMpIp == 0)
    {
        return;
    }
	
	LogPrint(LOG_LVL_DETAIL, MID_MCU_EQP, "[ClearSwitchTable]mp ip:%s \n", StrOfIP(dwMpIp));

	u8 byIdleSlot = 7;
	for (u16 wTableId = 0; wTableId < MAX_SWITCH_CHANNEL; ++wTableId)
	{
		if (m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].IsNull())
		{
			continue;
		}

		if(m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].GetRcvIP() == dwMpIp
			|| m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].GetSrcIp() == dwMpIp
			/*|| m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].GetDstIP() == dwMpIp
			|| m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].GetDisIp() == dwMpIp*/)
		{
			m_tSwitchTable[byIdleSlot].m_atSwitchChannel[wTableId].SetNull();
		}
	}
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
    输入参数说明：byNum             ：交换通道数量
                  ptSwitchChannel   ：交换信道指针
				  u8 byAct          ：1-add brdsrc；2-remove brdsrc；11-add brddst
                  u8 byMpId         ：目标MpId
                  BOOL32 bForcely   ：是否强制增加或删除，忽略相对应的转发是否在线，一般用于应对板卡掉线导致的时序问题
    返回值说明  ：成功TRUE,失败FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	03/03/27    4.0         顾振华        创建
====================================================================*/
void TConfSwitchTable::ProcBrdSwitch( u8 byNum, TSwitchChannel *ptSwitchChannel, u8 byAct, u8 byMpId, BOOL32 bForcely )
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
				//zjj20100311 第一次增加广播源的ack中带的广播源端口是对的
				//现在要增加多回传(又因为是单广播),所以只会有一个广播源端口,
				//在后面的增加广播目标端口由于mplib中的bug回过来的端口不对,
				//所以这里先记一下，在后面增加广播目标ack来的时候可以设置上
				SetBrdSrcRcvPort( tSwitchChannel.GetRcvPort() );
                m_tSwitchTable[byIdleIdx].m_atSwitchChannel[wTableId] = tSwitchChannel;
			}
            else
            {
                LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[ProcBrdSwitch] Add video broadcast source to switch table failed!\n");
            }
        }
        break;
        
    case 2://移除广播源
        {
            tSwitchChannel = *ptSwitchChannel;
            // 由于删除广播源消息是广播，这里只处理该MT接入的MP的回应
			// 掉线终端交换表仍有残留，会影响后来的带宽计算 [pengguofeng 1/19/2013]
//             u32 dwMpIp;
//             u32 dwSrcIp;
//             u16 wMpRecvPort;
//             if ( ! g_cMpManager.GetSwitchInfo(tSwitchChannel.GetSrcMt(), dwMpIp, wMpRecvPort, dwSrcIp) )
//             {
//                 return;
//             }
//             
//             if ( byMpId != g_cMcuVcApp.FindMp( dwMpIp ) &&
//                  !bForcely )
//             {
//                 break;
//             }
            
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
                // LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[ProcBrdSwitch] Remove video broadcast source to switch table failed!\n");
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
                    LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "[ProcBrdSwitch] m_tSwitchTable Mp.%d full, insert failed!\n", byMpId);
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
	LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[IsValidSwitchSrcIp] dwSrcIp.%0x, Port.%d, Mode.%d!\n",
											 dwSrcIp, wRcvPort, byMode);

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
					LogPrint(LOG_LVL_DETAIL, MID_MCU_MPMGR, "[IsValidSwitchSrcIp] Table<MpId.%d, Idx.%d> Exists Dst<Ip.%0x, Port.%d>\n",
															 byMpId,
															 wTableIdx,
															 ptSwitchChnnl->GetDstIP(),
															 ptSwitchChnnl->GetDstPort());

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
                     ptSwitchChnnl->GetSrcIp() == dwSrcIp &&
					 ptSwitchChnnl->GetRcvPort() == wRcvPort
					 )
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
            
			//源和转发端口不同则过滤
            if ( ptSwitchChnnl->GetSrcIp() != dwSrcIp || ptSwitchChnnl->GetRcvPort() != wRcvPort )
            {
                continue;
            }

            if( (byMode == MODE_VIDEO && ptSwitchChnnl->GetRcvPort() % PORTSPAN == 0 ) ||
                (byMode == MODE_AUDIO && ptSwitchChnnl->GetRcvPort() % PORTSPAN == 2) )
            {
				//分发ip和转发ip都非空 + 目的端口非空 + 非桥交换 标识存在桥交换码流，则不能拆桥
				if (ptSwitchChnnl->GetDisIp() != 0 &&
					ptSwitchChnnl->GetRcvIP() != 0 &&
					ptSwitchChnnl->GetDstPort() != 0 && 
					ptSwitchChnnl->GetRcvPort() != ptSwitchChnnl->GetDstPort())
				{
					LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MPMGR, "[IsValidCommonSwitchSrcIp] Exists Bridge Switch!\n");
					return 	TRUE;
				}


//                 if (ptSwitchChnnl->GetRcvPort() != ptSwitchChnnl->GetDstPort() && 
//                     // 这里是防止把广播源到MP的交换误认为是普通交换, zgc, 2008-08-27
//                     ptSwitchChnnl->GetDstPort() != 0 )
//                 {
//                     return TRUE;
//                 }
//                 else if (g_cMcuVcApp.FindMp(ptSwitchChnnl->GetDstIP()) == 0 &&
//                          // 这里是防止把广播源到MP的交换误认为是普通交换, zgc, 2008-08-27
//                          ptSwitchChnnl->GetDstIP() != 0xffffffff )
//                 {
//                     return TRUE;
//                 }
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
// BOOL32 TConfSwitchTable::IsValidBroadSwitchSrcIp( u32 dwSrcIp, u16 wRcvPort, u8 byMode )
// {
//     u8 byMpId;
//     for (u8 byLoop = 1; byLoop <= MAXNUM_DRI; byLoop++)
//     {
//         byMpId = byLoop;
//         
//         // guzh [3/29/2007] 8/16内部保存了特殊的交换条目
//         if ( !g_cMcuVcApp.IsMpConnected(byMpId) &&
//             byMpId != MCU_BOARD_MPC &&
//             byMpId != MCU_BOARD_MPCD )
//             continue;
//         
// #ifdef _MINIMCU_
//         if (byLoop > 1)
//         {
//             byMpId = 2;
//         }
// #endif
//         for(u16 wTableIdx = 0; wTableIdx < MAX_SWITCH_CHANNEL; wTableIdx++)
//         {
//             TSwitchChannel *ptSwitchChnnl = &m_tSwitchTable[byMpId-1].m_atSwitchChannel[wTableIdx];
//             if (ptSwitchChnnl->IsNull())
//             {
//                 continue;
//             }
//             
//             if (ptSwitchChnnl->GetDstIP() == 0xffffffff && 
//                 ptSwitchChnnl->GetDstPort() == 0 &&
//                 byMode == MODE_VIDEO &&
//                 ptSwitchChnnl->GetSrcIp() == dwSrcIp )
//             {
//                 return TRUE;
//             }
//         }
//     }
//     
//     return FALSE;	
// }

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
    
	
	//是否允许开启telnet
	GetRegKeyInt(achFullName, "mcuNetWork", "EnableTelnet", 1, &nKeyVal);
	tMcuDebugVal.m_byTelnetEn = (u8)nKeyVal;
	
	if( !GetRegKeyInt( achFullName, "refreshInterval", "mcsRefreshInterval", 1, &nKeyVal ))
	{
		SetRegKeyInt( achFullName, "refreshInterval", "mcsRefreshInterval", nKeyVal );
	}

	// [9/28/2011 liuxu] 防止为0, 小于等于0调整为1
	// 以秒为单位
	tMcuDebugVal.m_dwMcsRefreshInterval = nKeyVal > 0 ? nKeyVal : 1;
	// 与原始值不一致, 再次写入文件
	if (tMcuDebugVal.m_dwMcsRefreshInterval != (u32)nKeyVal)
	{
		SetRegKeyInt( achFullName, "refreshInterval", "mcsRefreshInterval", tMcuDebugVal.m_dwMcsRefreshInterval );
	}
	
	// [12/19/2011 liuxu] 转为毫秒
	tMcuDebugVal.m_dwMcsRefreshInterval *= 1000;
	
	// [11/23/2011 liuxu] 缓冲区启用与否的配置控制
	if( !GetRegKeyInt( achFullName, "refreshInterval", "EnableMcuMcsMtInfoBuf", 0, &nKeyVal ) )
	{
		SetRegKeyInt( achFullName, "refreshInterval", "EnableMcuMcsMtInfoBuf", 0 );
	}
	tMcuDebugVal.m_byEnableMcuMcsMtInfoBuf = nKeyVal > 0 ? 1 : 0;
	
	// 是否EnableMcuMcsMtStatusBuf
	if( !GetRegKeyInt( achFullName, "refreshInterval", "EnableMcuMcsMtStatusBuf", 1, &nKeyVal ) )
	{
		SetRegKeyInt( achFullName, "refreshInterval", "EnableMcuMcsMtStatusBuf", 1 );
	}
	tMcuDebugVal.m_byEnableMcuMcsMtStatusBuf = nKeyVal > 0 ? 1 : 0;
	
	// 是否EnableMcuMcuMtStatusBuf
	if( !GetRegKeyInt( achFullName, "refreshInterval", "EnableMcuMcuMtStatusBuf", 1, &nKeyVal ) )
	{
		SetRegKeyInt( achFullName, "refreshInterval", "EnableMcuMcuMtStatusBuf", 1 );
	}
	tMcuDebugVal.m_byEnableMcuMcuMtStatusBuf = nKeyVal > 0 ? 1 : 0;
	
	// 是否EnableMtAdpSMcuListBuf
	if( !GetRegKeyInt( achFullName, "refreshInterval", "EnableMtAdpSMcuListBuf", 0, &nKeyVal ) )
	{
		SetRegKeyInt( achFullName, "refreshInterval", "EnableMtAdpSMcuListBuf", 0 );
	}
	tMcuDebugVal.m_byEnableMtAdpSMcuListBuf = nKeyVal > 0 ? 1 : 0;
	
	
	if( !GetRegKeyInt( achFullName, "refreshInterval", "mtInfoRefreshInterval", 1000, &nKeyVal ))
	{
		SetRegKeyInt( achFullName, "refreshInterval", "mtInfoRefreshInterval", 1000 );
	}
	tMcuDebugVal.m_dwRefressMtInfoInterval = nKeyVal > 0 ? nKeyVal : 1000;
	
	if( !GetRegKeyInt( achFullName, "refreshInterval", "smcuMtRefreshInterval", 1000, &nKeyVal ))
	{
		SetRegKeyInt( achFullName, "refreshInterval", "smcuMtRefreshInterval", 1000 );
	}
	tMcuDebugVal.m_dwRefreshSMcuMtInterval = nKeyVal > 0 ? nKeyVal : 1000;	
	
	if( !GetRegKeyInt( achFullName, "refreshInterval", "mtAdpMtListRefreshInterval", 500, &nKeyVal ))
	{
		SetRegKeyInt( achFullName, "refreshInterval", "mtAdpMtListRefreshInterval", 500 );
	}
	tMcuDebugVal.m_dwRefreshMtAdpSMcuList = nKeyVal > 0 ? nKeyVal : 500;
	
	
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
	if( nKeyVal < (s32)MS_SYN_MAX_SPAN_TIMEOUT || nKeyVal > (s32)MS_SYN_MAX_SPAN_TIMEOUT*12 )
	{
		nKeyVal = (s32)MS_SYN_MAX_SPAN_TIMEOUT;
	}
	tMcuDebugVal.m_wMsSynTime = (u16)nKeyVal;
    
    GetRegKeyInt(achFullName, "BitrateAdapterParam", "BitrateScale", 0, &nKeyVal);    
    tMcuDebugVal.m_byBitrateScale = nKeyVal;
    
	//zjj20100205 取消，移到mcucfg.ini中
    //GetRegKeyInt(achFullName, "cascadeParam", "IsMMcuSpeaker", 0, &nKeyVal);
    //tMcuDebugVal.m_byIsMMcuSpeaker = (0 != nKeyVal) ? 1 : 0;

    GetRegKeyInt(achFullName, "cascadeParam", "IsAutoDetectMMcuDupCallIn", 0, &nKeyVal);
    tMcuDebugVal.m_byIsAutoDetectMMcuDupCall = (0 != nKeyVal) ? 1 : 0;

    GetRegKeyInt(achFullName, "cascadeParam", "CascadeAlias", 0, &nKeyVal);
    tMcuDebugVal.m_byCascadeAliasType = (u8)nKeyVal;

	//[nizhijun 2011/02/14]取消，是否显示上级MCU所挂终端列表，以mcucfg.ini中为准
    //GetRegKeyInt(achFullName, "cascadeParam", "IsShowMMcuMtList", 1, &nKeyVal);
    //tMcuDebugVal.m_byShowMMcuMtList = (u8)nKeyVal;
    
	//[03/04/2010] zjl modify (武警需求代码合并)
	GetRegKeyInt(achFullName, "cascadeParam", "IsStartSwtichToMMcuWithOutCaseBas", 0, &nKeyVal); 
    tMcuDebugVal.m_byIsStartSwitchToMMcuWithoutCasBas = (u8)nKeyVal;

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
    
	//会议帧率作弊
	GetRegKeyInt(achFullName, "mcuGeneralParam", "ConfFPS", 0, &nKeyVal);
    tMcuDebugVal.m_byConfFPS = (u8)nKeyVal;
	
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


	//zhouyiliang 20100702 vcs会议的呼叫超时时间，单位秒,默认10s
	GetRegKeyInt(achFullName, "mcuGeneralParam", "VcsMtOverTimeInterval", DEFAULT_VCS_MTOVERTIME_INTERVAL, &nKeyVal);
    tMcuDebugVal.m_byVcsMtOverTimeInterval = (u8)nKeyVal;
	if( tMcuDebugVal.m_byVcsMtOverTimeInterval < DEFAULT_VCS_MTOVERTIME_INTERVAL )
    {
        tMcuDebugVal.m_byVcsMtOverTimeInterval = DEFAULT_VCS_MTOVERTIME_INTERVAL;
    }
	
	// [1/19/2011 xliang] 开级联通道定时间隔
	if (GetRegKeyInt(achFullName, "mcuGeneralParam", "MmcuOLCTimerOut", TIMESPACE_WAIT_CASCADE_CHECK, &nKeyVal))
	{
		tMcuDebugVal.m_wMmcuOlcTimerOut = (u16)nKeyVal;
	}
	else
	{
		tMcuDebugVal.m_wMmcuOlcTimerOut = TIMESPACE_WAIT_CASCADE_CHECK;
	}

	//是否支持唇音同步
    GetRegKeyInt(achFullName, "mcuGeneralParam", "IsVASimultaneous ", 0, &nKeyVal);
    tMcuDebugVal.m_byIsVASimultaneous = (u8)nKeyVal;

    //配置的行政级别(默认是4级,最大1级)
	//zjj20100108这个配置废除，移到mcucfg.ini
    /*GetRegKeyInt(achFullName, "mcuGeneralParam", "McuAdminLevel", 4, &nKeyVal);
    if (nKeyVal < 1)
    {
        nKeyVal = 1;
    }
    tMcuDebugVal.m_byMcuAdminLevel = nKeyVal;
	*/

	//MCU是否运行MP
#if defined(_MINIMCU_) || defined(WIN32)
    // guzh [4/2/2007] Windows 缺省运行MP
    GetRegKeyInt(achFullName, "mcuAbilityParam", "IsMpcRunMp", 1, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byIsMpcRunMp = (u8)nKeyVal;	
#else
	GetRegKeyInt(achFullName, "mcuAbilityParam", "IsMpcRunMp", 0, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byIsMpcRunMp = (u8)nKeyVal;
#endif
	
	//MCU是否运行MtAdp, 8000B/C 才通过debug配置
#ifndef WIN32
	GetRegKeyInt(achFullName, "mcuAbilityParam", "IsMpcRunMtAdp", 0, &nKeyVal);
    tMcuDebugVal.m_tPerfLimit.m_byIsMpcRunMtAdp = (u8)nKeyVal;
	LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[McuGetDebugKeyValue] tMcuDebugVal.m_tPerfLimit.m_byIsMpcRunMtAdp = %d\n",
		nKeyVal); // [12/30/2009 xliang] add print
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
    GetRegKeyInt(achFullName, "mcuAbilityParam", "IsSendFakeCap2Taide", 1, &nKeyVal);
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
	
    //是否约束华为的标清MCU的2CIF的视频能力到CIF
    //GetRegKeyInt(achFullName, "mcuAbilityParam", "IsSendFakeCap2HuaweiSDMcu", 0, &nKeyVal);
    //tMcuDebugVal.m_bIsSendFakeCap2HuaweiSDMcu = (u8)nKeyVal;

    // guzh [4/17/2007] 最大即时会议数量
	//zjj20100108这个配置废除，改为mcs会议最大即时会议数，移到mcucfg.ini
    /*GetRegKeyInt( achFullName, "mcuMaxConnCap", "mcuMaxOngoingConfNum", MAXNUM_ONGO_CONF, &nKeyVal );

    tMcuDebugVal.m_byMaxOngoingConf = nKeyVal;
    if ( 0 == tMcuDebugVal.m_byMaxOngoingConf )
    {
        tMcuDebugVal.m_byMaxOngoingConf = MAXNUM_ONGO_CONF;
    }
    if ( tMcuDebugVal.m_byMaxOngoingConf > MAXNUM_ONGO_CONF )
    {
        tMcuDebugVal.m_byMaxOngoingConf = MAXNUM_ONGO_CONF;
    }*/

	GetRegKeyInt(achFullName, "mcuAbilityParam", "IsCanConfCapabilityCheat", 0, &nKeyVal);
	tMcuDebugVal.m_byIsCanConfCapabilityCheat = nKeyVal;


	//是否可以向终端会议能力欺骗
	GetRegKeyInt(achFullName, "mcuAbilityParam", "IsCanConfCapabilityCheat", 0, &nKeyVal);
	tMcuDebugVal.m_byIsCanConfCapabilityCheat = nKeyVal;

	//会议能力欺骗的终端类型数
	GetRegKeyInt(achFullName, "mcuAbilityParam", "MTBoardTypeNum ", 0, &nKeyVal);
	tMcuDebugVal.m_byConfCapCheatMtBoardTypeNum = nKeyVal;
	if( tMcuDebugVal.m_byConfCapCheatMtBoardTypeNum > MAXNUM_CONFCAPCHEATMTBOARDNUM )
	{
		tMcuDebugVal.m_byConfCapCheatMtBoardTypeNum = MAXNUM_CONFCAPCHEATMTBOARDNUM;
	}
 
	//可以会议能力欺骗的终端型号
	s8  achKeyName[20];
	u8 byBoardTypeIndex = 0;
	u8 byIndex = 0;
	for( byIndex = 0;byIndex < tMcuDebugVal.m_byConfCapCheatMtBoardTypeNum;byIndex++ )
	{
		sprintf(achKeyName, "MTBoardType%d", byIndex + 1 );
		GetRegKeyInt(achFullName, "mcuAbilityParam", achKeyName, MT_BOARD_UNKNOW, &nKeyVal);
		GetRegKeyString( achFullName,"mcuAbilityParam",achKeyName,"MT_BOARD_UNKNOW",(char*)&tMcuDebugVal.m_pachCheatMtBoardType[byBoardTypeIndex][0],MAXLEN_PRODUCTID);
		if( 0 != strcmp( "MT_BOARD_UNKNOW",(char*)tMcuDebugVal.m_pachCheatMtBoardType[byBoardTypeIndex] ) )
		{
			//strncpy( (char*),achKeyName,MAXLEN_PRODUCTID );
			tMcuDebugVal.m_pachCheatMtBoardType[byBoardTypeIndex][MAXLEN_PRODUCTID-1] = '\0';
		}		
		byBoardTypeIndex++;
	}

	//高清终端型号数量
	GetRegKeyInt(achFullName, "mcuAbilityParam", "HDMTBoardTypeNum ", 0, &nKeyVal);
	tMcuDebugVal.m_byConfHDMtBoardTypeNum = nKeyVal;
	if( tMcuDebugVal.m_byConfHDMtBoardTypeNum > MAXNUM_CONFHDMTBOARDNUM )
	{
		tMcuDebugVal.m_byConfHDMtBoardTypeNum = MAXNUM_CONFHDMTBOARDNUM;
	}

	//高清终端型号读取
	byBoardTypeIndex = 0;
	for( byIndex = 0;byIndex < tMcuDebugVal.m_byConfHDMtBoardTypeNum;byIndex++ )
	{
		sprintf(achKeyName, "HDMTBoardType%d", byIndex + 1 );
		GetRegKeyInt(achFullName, "mcuAbilityParam", achKeyName, MT_BOARD_UNKNOW, &nKeyVal);
		GetRegKeyString( achFullName,"mcuAbilityParam",achKeyName,"MT_BOARD_UNKNOW",(char*)&tMcuDebugVal.m_pachHDMtBoardType[byBoardTypeIndex][0],MAXLEN_PRODUCTID);
		if( 0 != strcmp( "MT_BOARD_UNKNOW",(char*)tMcuDebugVal.m_pachHDMtBoardType[byBoardTypeIndex] ) )
		{
			//strncpy( (char*),achKeyName,MAXLEN_PRODUCTID );
			tMcuDebugVal.m_pachHDMtBoardType[byBoardTypeIndex][MAXLEN_PRODUCTID-1] = '\0';
		}		
		byBoardTypeIndex++;
	}
	
	GetRegKeyInt(achFullName, "mcuAbilityParam", "LowBrCalledMtChlOpenMode", 0, &nKeyVal);
	tMcuDebugVal.m_byLowBrCalledMtChlOpenMode = nKeyVal;

	//是否支持根据终端呼叫码率降终端逻辑通道分辨率
    GetRegKeyInt(achFullName, "mcuAbilityParam", "IsSupportChgLGCResAcdBR", 0, &nKeyVal);
    tMcuDebugVal.m_byIsSupportChgLGCResAcdBR = (u8)nKeyVal;

	//1080P/60/50FPS Bp的码率分界点debug支持：单位Kbps，默认3072Kbps
    GetRegKeyInt(achFullName, "mcuAbilityParam", "1080P6050FPSBpLeastBitRate", 3072, &nKeyVal);
    tMcuDebugVal.m_w1080p6050FPSBpLeastBR = (u16)nKeyVal;
	
	//1080P/30/25FPS Bp的码率分界点debug支持：单位Kbps，默认2048Kbps
    GetRegKeyInt(achFullName, "mcuAbilityParam", "1080P3025FPSBpLeastBitRate", 2560, &nKeyVal);
    tMcuDebugVal.m_w1080p3025FPSBpLeastBR = (u16)nKeyVal;
	
	//720P BP的码率分界点debug支持：单位Kbps，默认1536Kbps
    GetRegKeyInt(achFullName, "mcuAbilityParam", "720P6050FpsBpLeastBitRate", 2560, &nKeyVal);
    tMcuDebugVal.m_w720P6050FpsBpLeastBR = (u16)nKeyVal;

	//720P 30/25 BP的码率分界点debug支持：单位Kbps，默认1024Kbps
    GetRegKeyInt(achFullName, "mcuAbilityParam", "720P3025FpsBpLeastBitRate", 1536, &nKeyVal);
    tMcuDebugVal.m_w720P3025FpsBpLeastBR = (u16)nKeyVal;

	//Cif 的码率分界点debug支持：单位Kbps，默认384Kbps
    GetRegKeyInt(achFullName, "mcuAbilityParam", "CifLeastBitRate", 384, &nKeyVal);
    tMcuDebugVal.m_wCifLeastBR = (u16)nKeyVal;
	
    //4Cif 的码率分界点debug支持：单位Kbps，默认1024Kbps
    GetRegKeyInt(achFullName, "mcuAbilityParam", "4CifLeastBitRate", 1024, &nKeyVal);
    tMcuDebugVal.m_w4CifLeastBR = (u16)nKeyVal;

	//1080P/60/50FPS Hp的码率分界点debug支持：单位Kbps，默认1536Kbps
    GetRegKeyInt(achFullName, "mcuAbilityParam", "1080P6050FPSHpLeastBitRate", 1536, &nKeyVal);
    tMcuDebugVal.m_w1080p6050FPSHpLeastBR = (u16)nKeyVal;
	
	//1080P/30/25FPS Hp的码率分界点debug支持：单位Kbps，默认1024Kbps
    GetRegKeyInt(achFullName, "mcuAbilityParam", "1080P3025FPSHpLeastBitRate", 1024, &nKeyVal);
    tMcuDebugVal.m_w1080p3025FPSHpLeastBR = (u16)nKeyVal;
	
	//720P/60/50Fps HP的码率分界点debug支持：单位Kbps，默认832Kbps
    GetRegKeyInt(achFullName, "mcuAbilityParam", "720P6050FpsHpLeastBitRate", 832, &nKeyVal);
    tMcuDebugVal.m_w720P6050FpsHpLeastBR = (u16)nKeyVal;

	//720P/30/25Fps HP的码率分界点debug支持：单位Kbps，默认512Kbps
    GetRegKeyInt(achFullName, "mcuAbilityParam", "720P3025FpsHpLeastBitRate", 512, &nKeyVal);
    tMcuDebugVal.m_w720P3025FpsHpLeastBR = (u16)nKeyVal;

	//是否支持申请主席自动开启定制混音
    GetRegKeyInt(achFullName, "mcuGeneralParam", "IsChairAutoSpecMix", 0, &nKeyVal);
    tMcuDebugVal.m_byIsChairAutoSpecMix = (0 != nKeyVal) ? 1 : 0; 

	//获取混音器超时间隔设置
	GetRegKeyInt( achFullName, "mcuAbilityParam", "MixerWaitRspTimeout", 6,&nKeyVal );
	tMcuDebugVal.m_byWaitMixerRspTimeout = nKeyVal;

	//获取vcs界面异常断链会议延迟结束的时间
	GetRegKeyInt( achFullName, "mcuGeneralParam", "DelayReleaseConfTime", 30, &nKeyVal );
	tMcuDebugVal.m_byDelayReleaseConfTime = nKeyVal;

    GetRegKeyInt(achFullName, "mcuAbilityParam", "FpsAdp", 0, &nKeyVal);
    tMcuDebugVal.m_byFpsAdp = nKeyVal;

	GetRegKeyInt( achFullName, "mcuAbilityParam", "MtFastUpdateInterval", DEFAULT_MT_MCU_FASTUPDATE_INTERVAL,&nKeyVal );
	tMcuDebugVal.m_byMtFastUpdateInterval = nKeyVal;

	GetRegKeyInt( achFullName, "mcuAbilityParam", "MtFastUpdateNeglectNum", 0,&nKeyVal );
	tMcuDebugVal.m_byMtFastUpdateNeglectNum = nKeyVal;
	
	//获取MCU向发言人请求关键帧次数（1-4次）[4/6/2012 chendaiwei]
	if(!GetRegKeyInt( achFullName, "mcuAbilityParam", "FastUpdateToSpeakerNum", 4,&nKeyVal ) || nKeyVal <1 || nKeyVal >4)
	{
		nKeyVal = 4;
	}

	tMcuDebugVal.m_byFastUpdateToSpeakerNum = nKeyVal;

	GetRegKeyInt( achFullName, "mcuAbilityParam", "IsPolyComMtSendDoubleStream", 1,&nKeyVal );
	tMcuDebugVal.m_byIsPolyComMtSendDoubleStream = nKeyVal;
	
	GetRegKeyInt( achFullName, "mcuAbilityParam", "IsChaoRanMtAllowInVmpChnnl", 0,&nKeyVal );
	tMcuDebugVal.m_byIsChaoRanMtAllowInVmpChnnl = nKeyVal;

	GetRegKeyInt( achFullName, "mcuAbilityParam", "IsSendStaticText", 1,&nKeyVal );
	tMcuDebugVal.m_byIsSendStaticText = nKeyVal;
	
	
	//  作弊的能力集[pengguofeng 7/24/2012]
	GetRegKeyInt(achFullName, "mcuCheatCap", "EntryNum", 0, &nKeyVal);
	if ( nKeyVal > MAXNUM_CHEATMTVIDEOCAP + MAXNUM_CHEATMTAUDIOCAP)//允许20个音频，100个视频
	{
		nKeyVal = MAXNUM_CHEATMTVIDEOCAP + MAXNUM_CHEATMTAUDIOCAP;
	}
	tMcuDebugVal.m_byCheatCapNum = nKeyVal;
	tMcuDebugVal.m_byCheatVideoCapNum = 0;
	u8 byAudioCapIndx = 0;
	u8 abyCapIdxToRealIdx[MAXNUM_CHEATMTVIDEOCAP + MAXNUM_CHEATMTAUDIOCAP]; //capid和能力集作弊的音视频表的idx对应关系
	memset(abyCapIdxToRealIdx,0xFF,sizeof(abyCapIdxToRealIdx));
	for ( byIndex = 0; byIndex < tMcuDebugVal.m_byCheatCapNum; byIndex++)
	{
		sprintf(achKeyName,"Entry%d",byIndex);
		//         achKeyName[5] = '0'+ byIndex;
		s8 achCapContent[100];               
		memset(achCapContent, 0, sizeof(achCapContent));
		
		if ( !GetRegKeyString(achFullName, "mcuCheatCap", achKeyName, 0, achCapContent, sizeof(achCapContent)) )
		{
			continue;
		}
		
		s8 achStore[6][6];
		memset(achStore, 0, sizeof(achStore));
		
		//分割
		sscanf(achCapContent, "%s %s %s %s %s %s\n", achStore[0], achStore[1], achStore[2], achStore[3],achStore[4],achStore[5]);
		printf("[mcuCheatCap]Key:[%s]MediaType:%s RES:%s FPS:%s BR:%s HP:%s,IsSelectH239:%s\n", achKeyName,
			achStore[0], achStore[1], achStore[2], achStore[3],achStore[4],achStore[5]);
		
		u8 byMedaiType = atoi(achStore[0]);
		//根据byMediaType分别存入videoCapSet或者AudioCapSet数组
		if ( tMcuDebugVal.IsAudioType(byMedaiType) )
		{
			if ( byAudioCapIndx >= MAXNUM_CHEATMTAUDIOCAP )//音频最多存20条能力集
			{
				continue;
			}
			u8 byAudioTrackNum = atoi(achStore[1]);
			if ( byAudioTrackNum == 0 ) //不设置则默认单声道
			{
				byAudioTrackNum = 1;
			}
			
			tMcuDebugVal.m_tCheatAudioCapSet[byAudioCapIndx].SetAudioMediaType(byMedaiType);
			tMcuDebugVal.m_tCheatAudioCapSet[byAudioCapIndx].SetAudioTrackNum(byAudioTrackNum);
			abyCapIdxToRealIdx[byIndex] = byAudioCapIndx;
			byAudioCapIndx++;

		}
		else  //视频或者空的能力集
		{
			u8 byRes = atoi(achStore[1]);
			u8 byFps = atoi(achStore[2]);
			u16 wMaxBR = atoi(achStore[3]);
			emProfileAttrb eAttrb = (emProfileAttrb)atoi(achStore[4]);
			u8 bySelectH239 = atoi(achStore[5]);
			if ( byRes == 0 || byFps == 0  || tMcuDebugVal.m_byCheatVideoCapNum >= MAXNUM_CHEATMTVIDEOCAP )//视频最多存100条能力集
			{
				continue;
			}
		
			//保存
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetMediaType(byMedaiType);
			if( MEDIA_TYPE_H264 == byMedaiType )
			{
				
				tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetUserDefFrameRate(byFps);
			}
			else
			{
				tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetFrameRate(byFps);
			}
			
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetResolution(byRes);
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetMaxBitRate(wMaxBR);
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetH264ProfileAttrb(eAttrb);
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetSupportH239( (BOOL32)bySelectH239 );
			abyCapIdxToRealIdx[byIndex] = tMcuDebugVal.m_byCheatVideoCapNum;
			tMcuDebugVal.m_byCheatVideoCapNum++;
		}

	
 	}
	// 作弊的IP地址和厂商 [pengguofeng 7/24/2012]
	GetRegKeyInt(achFullName, "mcuCheatMt", "EntryNum", 0, &nKeyVal);
	if ( nKeyVal > 100)
	{
		nKeyVal = 100;
	}
	tMcuDebugVal.m_byCheatMtNum = nKeyVal;
	
	//     s8 achKeyName[7];                    // 暂时最多是Entry192, 或Cap192
	memset(achKeyName, 0, sizeof(achKeyName));
	for ( byIndex = 0; byIndex < tMcuDebugVal.m_byCheatMtNum; byIndex++ )
	{
		sprintf(achKeyName,"Entry%d",byIndex);
		s8 achCheatIpContent[200];                // 内容最大长度: 15位IP + tab + 3 + tab + 3 + \0
		memset(achCheatIpContent, 0, sizeof(achCheatIpContent));
		
		if ( !GetRegKeyString(achFullName, "mcuCheatMt", achKeyName, 0, achCheatIpContent, sizeof(achCheatIpContent)) )
		{
			continue;
		}
		
		s8 achStore[5][MAXLEN_PRODUCTID];
		memset(achStore, 0, sizeof(achStore));
		
		//分割
		sscanf(achCheatIpContent, "%s %s %s %s %s\n", achStore[0], achStore[1], achStore[2], achStore[3],achStore[4]);
		printf("[mcuCheatIp]key:[%s]MtType:%s MtAlias:%s MainCapId:%s DualCapId:%s AudioCapId:%s\n", achKeyName,
			achStore[0], achStore[1], achStore[2], achStore[3],achStore[4]);
		
		u8 byMtType = atoi(achStore[0]);
		s8 *pszMtAlias = achStore[1];
		u8 byMainCapId = atoi(achStore[2]);  //注意：不要使用字母，字母返回也是0;无效值请用255
		u8 byDualCapId = atoi(achStore[3]);  //同上,界面参数用-1表示,实际程序对应255
		u8 byAudioCapId = atoi(achStore[4]);
		if (  byMtType == 0 || (byMainCapId == 255 && byDualCapId == 255 && byAudioCapId == 255)) //非法条件2:  主流能力和双流能力编号均为255
		{
			continue;
		}
		
		//保存
		tMcuDebugVal.m_tCheatMtParam[byIndex].m_byMtIdType = byMtType;
		//设置了对应的能力集，但是对应的能力集有问题（null）,则让其指向空能力集m_tCheatVideoCapSet[MAXNUM_CHEATMTVIDEOCAP+1]
		//和m_tCheatAudioCapSet[MAXNUM_CHEATMTAUDIOCAP+1]的最后一个，最后一个不赋值，肯定为空
		if (  byMainCapId != 255 && abyCapIdxToRealIdx[byMainCapId] == 255 )
		{
			abyCapIdxToRealIdx[byMainCapId] = MAXNUM_CHEATMTVIDEOCAP;
		}
		if (  byDualCapId != 255 && abyCapIdxToRealIdx[byDualCapId] == 255 )
		{
			abyCapIdxToRealIdx[byDualCapId] = MAXNUM_CHEATMTVIDEOCAP;
		}
		if (  byAudioCapId != 255 && abyCapIdxToRealIdx[byAudioCapId] == 255 )
		{
			abyCapIdxToRealIdx[byAudioCapId] = MAXNUM_CHEATMTAUDIOCAP;
		}
		if( byMainCapId < MAXNUM_CHEATMTVIDEOCAP + MAXNUM_CHEATMTAUDIOCAP )
		{
			tMcuDebugVal.m_tCheatMtParam[byIndex].m_byMainCapId = abyCapIdxToRealIdx[byMainCapId];
		}
		if( byDualCapId < MAXNUM_CHEATMTVIDEOCAP + MAXNUM_CHEATMTAUDIOCAP )
		{
			tMcuDebugVal.m_tCheatMtParam[byIndex].m_byDualCapId = abyCapIdxToRealIdx[byDualCapId];
		}
		if( byAudioCapId < MAXNUM_CHEATMTVIDEOCAP + MAXNUM_CHEATMTAUDIOCAP )
		{
			tMcuDebugVal.m_tCheatMtParam[byIndex].m_byAudioCapId = abyCapIdxToRealIdx[byAudioCapId];
		}		
		
		memcpy(tMcuDebugVal.m_tCheatMtParam[byIndex].m_aszMtAlias,pszMtAlias,sizeof(achStore[1]));
	}

	//兼容sp1_v4r6b2[8/21/2012 chendaiwei]
	GetRegKeyInt(achFullName, "mcuCheatIp", "EntryNum", 0, &nKeyVal);
	if ( nKeyVal > MAXNUM_CONF_MT)
	{
		nKeyVal = MAXNUM_CONF_MT;
	}

	u8 byCheatIpNum = (u8)nKeyVal;
	
	memset(achKeyName, 0, sizeof(achKeyName));
	for ( byIndex = 0; byIndex < tMcuDebugVal.m_byCheatMtNum; byIndex++ )
	{
		sprintf(achKeyName,"Entry%d",byIndex);
		s8 achCheatIpContent[100];			
		memset(achCheatIpContent, 0, sizeof(achCheatIpContent));
		
		if ( !GetRegKeyString(achFullName, "mcuCheatIp", achKeyName, 0, achCheatIpContent, sizeof(achCheatIpContent)) )
		{
			continue;
		}
		
		s8 achStore[3][MAXLEN_PRODUCTID];
		memset(achStore, 0, sizeof(achStore));
		sscanf(achCheatIpContent, "%s %s %s\n", achStore[0], achStore[1], achStore[2]);

		u8 byIdx = 0;
		for( ;byIdx < tMcuDebugVal.m_byCheatMtNum; byIdx++ )
		{
			if( tMcuDebugVal.m_tCheatMtParam[byIdx].m_byMtIdType == 1
				&& INET_ADDR(tMcuDebugVal.m_tCheatMtParam[byIdx].m_aszMtAlias)==  INET_ADDR(achStore[0]))
			{
				break;
			}
		}

		//Cheat列表中已有相同IP的作弊[8/21/2012 chendaiwei]
		u8 byCapIdx = atoi(achStore[1]);
		if( byIdx != tMcuDebugVal.m_byCheatMtNum || byCapIdx == 'N' || tMcuDebugVal.m_byCheatVideoCapNum >= MAXNUM_CHEATMTVIDEOCAP)
		{
			continue;
		}
		else
		{
			memset(achKeyName, 0, sizeof(achKeyName));
			sprintf(achKeyName,"Cap%d",byCapIdx);
			s8 achCheatCapContent[100];			
			memset(achCheatCapContent, 0, sizeof(achCheatCapContent));
				
			if ( !GetRegKeyString(achFullName, "NonKedaCap", achKeyName, 0, achCheatCapContent, sizeof(achCheatCapContent)) )
			{
				continue;
			}

			tMcuDebugVal.m_tCheatMtParam[tMcuDebugVal.m_byCheatMtNum].m_byMtIdType = 1;
			tMcuDebugVal.m_tCheatMtParam[tMcuDebugVal.m_byCheatMtNum].m_byMainCapId = tMcuDebugVal.m_byCheatVideoCapNum;
			tMcuDebugVal.m_tCheatMtParam[tMcuDebugVal.m_byCheatMtNum].m_byDualCapId = 0xFF;
			memcpy(tMcuDebugVal.m_tCheatMtParam[tMcuDebugVal.m_byCheatMtNum].m_aszMtAlias,achStore[0],sizeof(achStore[0]));
			tMcuDebugVal.m_byCheatMtNum ++;

			s8 achCapStore[4][MAXLEN_PRODUCTID];
			memset(achCapStore, 0, sizeof(achCapStore));
			sscanf(achCheatCapContent, "%s %s %s %s\n", achCapStore[0], achCapStore[1], achCapStore[2],achCapStore[3]);
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetMediaType(atoi(achCapStore[0]));
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetResolution(atoi(achCapStore[1]));
			
			if( atoi(achCapStore[0]) == MEDIA_TYPE_H264 )
			{
				tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetUserDefFrameRate(atoi(achCapStore[2]));
			}
			else
			{
				tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetFrameRate(atoi(achCapStore[2]));
			}
			
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetMaxBitRate(atoi(achCapStore[3]));
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetH264ProfileAttrb(emBpAttrb);
			tMcuDebugVal.m_byCheatVideoCapNum++;//视频个数+1
			tMcuDebugVal.m_byCheatCapNum++;
		}
	}

	//兼容Rlease_v4r6b2[8/21/2012 chendaiwei]
	GetRegKeyInt( achFullName, "mcuNoneKedaMtAbilityParam", "MtNum", 0,&nKeyVal );
	tMcuDebugVal.m_byNoneKedaMtAbilityCheatNum = nKeyVal;
	if( tMcuDebugVal.m_byNoneKedaMtAbilityCheatNum > MAXNUM_CONF_MT )
	{
		tMcuDebugVal.m_byNoneKedaMtAbilityCheatNum = MAXNUM_CONF_MT;
	}

	s8  achContent[16];
	for( byIndex = 0;byIndex < tMcuDebugVal.m_byNoneKedaMtAbilityCheatNum ;++byIndex )
	{
		memset( achContent,0,sizeof(achContent) );
		sprintf(achKeyName, "ip%d", byIndex + 1 );
		GetRegKeyString( achFullName,"mcuNoneKedaMtAbilityParam",achKeyName,"0",(char*)&achContent[0],16);
		achContent[sizeof(achContent)-1] = '\0';
		u8 byIdx = 0;
		for( ;byIdx < tMcuDebugVal.m_byCheatMtNum; byIdx++ )
		{
			if( tMcuDebugVal.m_tCheatMtParam[byIdx].m_byMtIdType == 1
				&& INET_ADDR(tMcuDebugVal.m_tCheatMtParam[byIdx].m_aszMtAlias)==  INET_ADDR(&achContent[0]))
			{
				break;
			}
		}

		//Cheat列表中已有相同IP的作弊[8/21/2012 chendaiwei]
		if( byIdx != tMcuDebugVal.m_byCheatMtNum || tMcuDebugVal.m_byCheatVideoCapNum >= MAXNUM_CHEATMTVIDEOCAP )//视频最多存100条能力集
		{
			continue;
		}
		else
		{
			tMcuDebugVal.m_tCheatMtParam[tMcuDebugVal.m_byCheatMtNum].m_byMtIdType = 1;
			tMcuDebugVal.m_tCheatMtParam[tMcuDebugVal.m_byCheatMtNum].m_byMainCapId = tMcuDebugVal.m_byCheatVideoCapNum;
			tMcuDebugVal.m_tCheatMtParam[tMcuDebugVal.m_byCheatMtNum].m_byDualCapId = 0xFF;
			memcpy(tMcuDebugVal.m_tCheatMtParam[tMcuDebugVal.m_byCheatMtNum].m_aszMtAlias,achContent,sizeof(achContent));
			tMcuDebugVal.m_byCheatMtNum ++;

			//  [8/10/2011 chendaiwei]取分辨率
			memset( achContent,0,sizeof(achContent) );
			sprintf(achKeyName, "resolution%d", byIndex + 1 );
			GetRegKeyString( achFullName,"mcuNoneKedaMtAbilityParam",achKeyName,"cif",(char*)&achContent[0],16);
			achContent[sizeof(achContent)-1] = '\0';
			u8 byResolution = VIDEO_FORMAT_INVALID;
			if( strcmp(achContent,"cif") == 0 )
			{
				byResolution = VIDEO_FORMAT_CIF;
			}
			else if( strcmp(achContent,"4cif") == 0 )
			{
				byResolution= VIDEO_FORMAT_4CIF;
			}
			else if( strcmp(achContent,"720") == 0 )
			{
				byResolution = VIDEO_FORMAT_HD720;
			}
			else if( strcmp(achContent,"1080") == 0 )
			{
				byResolution = VIDEO_FORMAT_HD1080;
			}

			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetResolution(byResolution);
			
			//  [8/10/2011 chendaiwei]取媒体格式
			memset( achContent,0,sizeof(achContent) );
			sprintf(achKeyName, "mediatype%d", byIndex + 1 );
			GetRegKeyString( achFullName,"mcuNoneKedaMtAbilityParam",achKeyName,"H264",(char*)&achContent[0],16);
			achContent[sizeof(achContent)-1] = '\0';
			u8 byMediaType = MEDIA_TYPE_NULL;
			if( strcmp(achContent,"H264") == 0 )
			{
				byMediaType = MEDIA_TYPE_H264;
			}
			else if( strcmp(achContent,"H263") == 0 )
			{
				byMediaType = MEDIA_TYPE_H263;
			}
			else if( strcmp(achContent,"H261") == 0 )
			{
				byMediaType = MEDIA_TYPE_H261;
			}
			else if( strcmp(achContent,"MPEG4") == 0 )
			{
				byMediaType = MEDIA_TYPE_MP4;
			}
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetMediaType(byMediaType);
			
			//  [8/10/2011 chendaiwei]取帧率
			sprintf(achKeyName, "fps%d", byIndex + 1 );
			GetRegKeyInt(achFullName, "mcuNoneKedaMtAbilityParam",achKeyName , 25, &nKeyVal);		

			if( byMediaType == MEDIA_TYPE_H264 )
			{
				tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetUserDefFrameRate((u8)nKeyVal);
			}
			else
			{
				tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetFrameRate((u8)nKeyVal);
			}
			
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetMaxBitRate(0);
			tMcuDebugVal.m_tCheatVideoCapSet[tMcuDebugVal.m_byCheatVideoCapNum].SetH264ProfileAttrb(emBpAttrb);
			tMcuDebugVal.m_byCheatVideoCapNum++;
			tMcuDebugVal.m_byCheatCapNum++;
		}
	}

	//20120907 zhouyiliang 需要作假源ip和端口的目的ip,最多100条
	GetRegKeyInt( achFullName, "mcuNeedMapIp", "EntryNum", 0,&nKeyVal );
	tMcuDebugVal.m_byNeedMapIpNum = min(nKeyVal,100);	
	memset(achKeyName, 0, sizeof(achKeyName));
	for (byIndex = 0 ; byIndex< tMcuDebugVal.m_byNeedMapIpNum;byIndex++)
	{
	
		sprintf(achKeyName,"Entry%d",byIndex);
		s8 achMapIpAddr[20];			
		memset(achMapIpAddr, 0, sizeof(achMapIpAddr));
		
		if ( !GetRegKeyString(achFullName, "mcuNeedMapIp", achKeyName, 0, achMapIpAddr, sizeof(achMapIpAddr)) )
		{
			continue;
		}
		tMcuDebugVal.m_adwNeedMappedIp[byIndex] = INET_ADDR(achMapIpAddr);
	}
	


    //接收组播包的终端列表
    GetRegKeyInt( achFullName, "mcuMultiCastMtTable", "EntryNum", 0, &nKeyVal );
    nKeyVal = min( nKeyVal, MAXNUM_MCU_MT );
    nKeyVal = max( nKeyVal, 0 );
    memset(achKeyName, 0, sizeof(achKeyName));
    for ( byIndex = 0; byIndex< nKeyVal; byIndex++ )
    {
        sprintf(achKeyName, "Entry%d", byIndex);
        s8 achMtIp[20] = {0};
        if( !GetRegKeyString( achFullName, "mcuMultiCastMtTable", achKeyName, "0.0.0.0", achMtIp, sizeof(achMtIp)) ||
            INET_ADDR(achMtIp) == 0)
        {
            continue;
        }
        
        tMcuDebugVal.m_adwMultiCastMtTable[tMcuDebugVal.m_byMultiCastMtTableSize] = INET_ADDR(achMtIp);
        tMcuDebugVal.m_byMultiCastMtTableSize++;
        
    }

    //读取打洞时间间隔
    GetRegKeyInt( achFullName, "PinHoldConfig", "PinHoldInterval", 5, &nKeyVal );
    tMcuDebugVal.m_dwPinHoleInterval = nKeyVal;

	//非科达厂商哑音包配置[6/17/2013 chendaiwei]
	nKeyVal = 0;
	GetRegKeyInt( achFullName, "NoKedaMutePackConfig", "EntryNum", 0,&nKeyVal );

	tMcuDebugVal.m_byMutePackManuEntryNum = min(nKeyVal,20);	
	memset(achKeyName, 0, sizeof(achKeyName));
	for (byIndex = 0 ; byIndex< tMcuDebugVal.m_byMutePackManuEntryNum;byIndex++)
	{
		sprintf(achKeyName,"Entry%d",byIndex);
		s8 achSndSelfMutePackMt[140] = {0};	
		if ( !GetRegKeyString(achFullName, "NoKedaMutePackConfig", achKeyName, 0, achSndSelfMutePackMt, sizeof(achSndSelfMutePackMt)) )
		{
			continue;
		}
		s8 achSendSelfManuId[4] = {0};
		//productid可能中间会有空格，配置文件中用|来表示productid的开始
		s8 achdelimChar[] = "|";
		s8 * pTok = strtok(achSndSelfMutePackMt,achdelimChar);
		if (NULL == pTok)
		{
			continue;
		}
		u8 byLen = strlen(pTok);
		memcpy(achSendSelfManuId,pTok,min(byLen,3));
		achSendSelfManuId[3] = '\0';
		pTok = strtok(NULL,achdelimChar);
		if (NULL == pTok)
		{
			continue;
		}
		
		tMcuDebugVal.m_atSndSelfMutePackMt[byIndex].m_byManuId = atoi(achSendSelfManuId);
		byLen = strlen(pTok);
		memcpy(tMcuDebugVal.m_atSndSelfMutePackMt[byIndex].m_achProductId ,pTok,min(byLen,MAXLEN_PRODUCTID));
	}

	nKeyVal = 0;
	GetRegKeyInt( achFullName, "NoKedaMutePackConfig", "MutePackSendInteral", 1,&nKeyVal );
	tMcuDebugVal.m_byMutePackSendInterval = (u8)nKeyVal;

	nKeyVal = 0;
	GetRegKeyInt( achFullName, "NoKedaMutePackConfig", "MutePackNum", 5,&nKeyVal );
	tMcuDebugVal.m_byMutePackNum = (u8)nKeyVal;
	
	//20131114 g7221.c音频格式码流内容奇偶对换终端manuid和productid
	GetRegKeyInt( achFullName, "NoKedaReverseG7221cConfig", "EntryNum", 0,&nKeyVal );
	tMcuDebugVal.m_byReverseG7221cNum = min(nKeyVal,5);	
	memset(achKeyName, 0, sizeof(achKeyName));
	for (byIndex = 0 ; byIndex< tMcuDebugVal.m_byReverseG7221cNum;byIndex++)
	{
		sprintf(achKeyName,"Entry%d",byIndex);
		s8 achReverseG7221cMt[140] = {0};	
		if ( !GetRegKeyString(achFullName, "NoKedaReverseG7221cConfig", achKeyName, 0, achReverseG7221cMt, sizeof(achReverseG7221cMt)) )
		{
			continue;
		}
		s8 achReverseG7221cManuId[4] = {0};
		//productid可能中间会有空格，配置文件中用|来表示productid的开始
		s8 achdelimChar[] = "|";
		s8 * pTok = strtok(achReverseG7221cMt,achdelimChar);
		if (NULL == pTok)
		{
			continue;
		}
		u8 byLen = strlen(pTok);
		memcpy(achReverseG7221cManuId,pTok,min(byLen,3));
		achReverseG7221cManuId[3] = '\0';
		pTok = strtok(NULL,achdelimChar);
		if (NULL == pTok)
		{
			continue;
		}
		
		tMcuDebugVal.m_atReverseG7221cMt[byIndex].m_byManuId = atoi(achReverseG7221cManuId);
		byLen = strlen(pTok);
		memcpy(tMcuDebugVal.m_atReverseG7221cMt[byIndex].m_achProductId ,pTok,min(byLen,MAXLEN_PRODUCTID));
	}

	//20130723 需要按特殊方式解码的产品终端manuid和productid，medianet针对华为码流 设置标志位接口
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	GetRegKeyInt( achFullName, "NoKedaRcvH264DependInMarkConfig", "EntryNum", 0,&nKeyVal );
	tMcuDebugVal.m_byRcvH264DependInMarkNum = min(nKeyVal,5);	
	memset(achKeyName, 0, sizeof(achKeyName));
	for (byIndex = 0 ; byIndex< tMcuDebugVal.m_byRcvH264DependInMarkNum;byIndex++)
	{
		sprintf(achKeyName,"Entry%d",byIndex);
		s8 achRcvH264DependInMarkMt[140] = {0};	
		if ( !GetRegKeyString(achFullName, "NoKedaRcvH264DependInMarkConfig", achKeyName, 0, achRcvH264DependInMarkMt, sizeof(achRcvH264DependInMarkMt)) )
		{
			continue;
		}
		s8 achRcvH264DependInMarkManuId[4] = {0};
		//productid可能中间会有空格，配置文件中用|来表示productid的开始
		s8 achdelimChar[] = "|";
		s8 * pTok = strtok(achRcvH264DependInMarkMt,achdelimChar);
		if (NULL == pTok)
		{
			continue;
		}
		u8 byLen = strlen(pTok);
		memcpy(achRcvH264DependInMarkManuId,pTok,min(byLen,3));
		achRcvH264DependInMarkManuId[3] = '\0';
		pTok = strtok(NULL,achdelimChar);
		if (NULL == pTok)
		{
			continue;
		}
		
		tMcuDebugVal.m_atRcvH264DependInMarkMt[byIndex].m_byManuId = atoi(achRcvH264DependInMarkManuId);
		byLen = strlen(pTok);
		memcpy(tMcuDebugVal.m_atRcvH264DependInMarkMt[byIndex].m_achProductId ,pTok,min(byLen,MAXLEN_PRODUCTID));
	}
#endif

	return;
}

#define INNER_FREE_MEM(p, n)				\
	{									\
		for( u32 dwInnerLoop = 0; dwInnerLoop < n; dwInnerLoop++ )			\
		{															\
			if( p && NULL != p[dwInnerLoop] )			\
			{															\
				free( p[dwInnerLoop] );							\
				p[dwInnerLoop] = NULL;							\
			}															\
		}															\
		free( p );											\
		p = NULL;											\
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
    //s8   pszE164Start[MAXLEN_E164] = {0};
    //s8   pszE164End[MAXLEN_E164] = {0};
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
		// LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "McuGetMtCallInterfaceInfo Err while reading %s %s!\n", "MtCallInterfaceTable", "EntryNum" );
		return FALSE;
	}
	if( nEntryNum < 0 )
	{
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "McuGetMtCallInterfaceInfo Err2 while reading %s %s!\n", "MtCallInterfaceTable", "EntryNum" );
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
	
	// [7/25/2011 liuxu]
	if ( NULL == lpszTable )
	{
		printf("McuGetMtCallInterfaceInfo Err while malloc for lpszTable\n");
		return FALSE;
	}

	for( dwLoop = 0; dwLoop < (u32)dwEntryNum; dwLoop++ )
	{
		lpszTable[dwLoop] = (s8 *)malloc( MAX_VALUE_LEN+1 );
	}
	
	//get the table string
	bRet = GetRegKeyStringTable( achFullName, "MtCallInterfaceTable", "fail", lpszTable, &dwEntryNum, (MAX_VALUE_LEN+1) );
	if( FALSE == bRet ) 
	{
		printf( "McuGetMtCallInterfaceInfo Err while reading %s table!\n", "MtCallInterfaceTable" );

		// [7/25/2011 liuxu] 释放内存
		INNER_FREE_MEM(lpszTable, dwEntryNum);
		return bRet;
	}

	for( dwLoop = 0; dwLoop < dwEntryNum; dwLoop++ )
	{
        
        u32 dwStartNetIp = 0;
        u32 dwEndNetIp = 0;
        //u8  byAliasType = mtAliasTypeOthers;

		pchToken = NULL;
		if (lpszTable && lpszTable[dwValidNum])
		{
			pchToken = strtok( lpszTable[dwValidNum], chSeps );
		}		

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
                //byAliasType = mtAliasTypeTransportAddress;
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

	INNER_FREE_MEM(lpszTable, dwEntryNum);
    
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
	
	//return mtAliasTypeOthers;
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
	// [7/25/2011 liuxu]
	if ( !pszSrc || !pszDst)
	{
		return FALSE;
	}

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
			pszDstTmp++;
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

	//会议头信息_存储文件的备份文件不存在,则进行首次创建
	sprintf(achFullName, "%s/%s", DIR_DATA, CONFHEADINFO_BACKUP);
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
BOOL32 AddConfToFile( TConfStore &tConfStore, BOOL32 bDefaultConf,TConfStore *ptOldConfStore )
{
	//FILE *hHeadFile = NULL;
	//FILE *hConfFile = NULL;

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
	if (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE== byConfPos)
	{
		byConfPos = 0;
		while (!acConfId[byConfPos].IsNull() && byConfPos<(MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE))
		{
			byConfPos++;
		}
	}

	//已满
	if (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE== byConfPos) 
	{
		return FALSE;
	}

	//2.1zjj20091102先将会议额外数据(那些动态增长的数据，现在只存放vcs电视墙预案)拿出来
	u16 wExtraDataLen = 0;
	u8 *pbyExtraDataBuf = NULL; //预案数据[8/16/2012 chendaiwei]

	u16 wExInfoLen = 0;
	u8 *pbyExInfoBuf = NULL;//扩展信息数据[8/16/2012 chendaiwei]

	if( NULL != ptOldConfStore )//修改模板
	{
		GetConfExtraDataLenFromFile( *ptOldConfStore,wExtraDataLen,wExInfoLen );							
	}
	else
	{
		GetConfExtraDataLenFromFile( tConfStore,wExtraDataLen,wExInfoLen );	
	}

	//预案数据长度有可能为0，不包括数据长度的u16.
	//扩展数据数据长度最小为u16，因为包括数据长度的u16[8/16/2012 chendaiwei]

	pbyExtraDataBuf = new u8[wExtraDataLen+1];
	pbyExInfoBuf = new u8[wExInfoLen+1];

	BOOL32 bHasFix2PlanData = FALSE;
	if( NULL != pbyExtraDataBuf && NULL != pbyExInfoBuf )
	{
		if( NULL != ptOldConfStore )
		{
			GetConfExtraDataFromFile( *ptOldConfStore,pbyExtraDataBuf,wExtraDataLen,pbyExInfoBuf,wExInfoLen,bHasFix2PlanData );

		}
		else
		{
			GetConfExtraDataFromFile( tConfStore,pbyExtraDataBuf,wExtraDataLen,pbyExInfoBuf,wExInfoLen,bHasFix2PlanData );
		}				
	}

	u8* pbyActualExtraDataBuf = NULL;
	u16 wActualExtradataLen = 0;
	if(bHasFix2PlanData)
	{
		wActualExtradataLen = GetPlanDataLenByV4R6B2PlanData(pbyExtraDataBuf,wExtraDataLen);
		if( wActualExtradataLen == 0 )
		{
			SAFE_DEL_ARRAY(pbyExtraDataBuf);
			SAFE_DEL_ARRAY(pbyExInfoBuf);
			SAFE_DEL_ARRAY(pbyActualExtraDataBuf);
			LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_CFG,"[AddConfToFile]Conf.%s wActualExtradataLen==0, return!\n",tConfStore.m_tConfInfo.GetConfName());

			return FALSE;
		}

		pbyActualExtraDataBuf = new u8[ wActualExtradataLen ];
		if (!pbyActualExtraDataBuf)
		{
			SAFE_DEL_ARRAY(pbyExtraDataBuf);
			SAFE_DEL_ARRAY(pbyExInfoBuf);
			SAFE_DEL_ARRAY(pbyActualExtraDataBuf);

			return FALSE;
		}
		
		TransferV4R6B2planDataToV4R7(pbyExtraDataBuf,wExtraDataLen,pbyActualExtraDataBuf,wActualExtradataLen);
		
		LogPrint(LOG_LVL_KEYSTATUS,MID_MCU_CFG,"[AddConfToFile]Conf.%s v4r6b2 plan Data transfer from len.%d to len.%d\n",
			tConfStore.m_tConfInfo.GetConfName(),wExtraDataLen,wActualExtradataLen);

	}
	else
	{
		pbyActualExtraDataBuf = new u8[ wExtraDataLen+1 ];
		if (!pbyActualExtraDataBuf)
		{
			SAFE_DEL_ARRAY(pbyExtraDataBuf);
			SAFE_DEL_ARRAY(pbyExInfoBuf);
			SAFE_DEL_ARRAY(pbyActualExtraDataBuf);

			return FALSE;
		}
		
		if(pbyActualExtraDataBuf!=NULL && pbyExtraDataBuf!=NULL)
		{
			memcpy(pbyActualExtraDataBuf,pbyExtraDataBuf,wExtraDataLen);
			wActualExtradataLen = wExtraDataLen;
		}
	}
				
	//////////////////////////////////////////////////////////////////////////
	

	//2.2将 未进行Pack处理的会议数据 进行Pack处理
	if (FALSE == PackConfStore(tConfStore, ptPackConfStore, wPackConfDataLen))
	{
		SAFE_DEL_ARRAY(pbyExtraDataBuf);
		SAFE_DEL_ARRAY(pbyExInfoBuf);
		SAFE_DEL_ARRAY(pbyActualExtraDataBuf);

		return FALSE;
	}

	//3.1保存 该会议或模板信息 到指定索引的存储文件
	cConfConfId = ptPackConfStore->m_tConfInfo.GetConfId();
	if (FALSE == SetConfDataToFile(byConfPos, cConfConfId, (u8*)ptPackConfStore, wPackConfDataLen))
	{
		SAFE_DEL_ARRAY(pbyExtraDataBuf);
		SAFE_DEL_ARRAY(pbyExInfoBuf);
		SAFE_DEL_ARRAY(pbyActualExtraDataBuf);

		return FALSE;
	}


	//////////////////////////////////////////////////////////////////////////
	
	
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
		//缺省会议记录标志位[7/17/2012 chendaiwei]
		TConfAttrbEx tConfAttrbEx = ptPackConfStore->m_tConfInfo.GetConfAttrbEx();
		tConfAttrbEx.SetDefaultConfFlag(TRUE);
		ptPackConfStore->m_tConfInfo.SetConfAttrbEx(tConfAttrbEx);

		//允许保存缺省失败
		if (SetConfDataToFile((MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE), 
			                  cDefConfConfId, (u8*)ptPackConfStore, wPackConfDataLen))
		{
			acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE] = cDefConfConfId;
		}
		else
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_CFG,"save default conf failed!\n");
		}
	}
	
	//5.重新保存 刷新后的所有会议或模板头信息
	if (FALSE == SetAllConfHeadToFile(acConfId, sizeof(acConfId)))
	{
		SAFE_DEL_ARRAY(pbyExtraDataBuf);
		SAFE_DEL_ARRAY(pbyExInfoBuf);
		SAFE_DEL_ARRAY(pbyActualExtraDataBuf);

		return FALSE;
	}

	//6.2zjj20091102 保存会议的额外信息(那些动态增长的数据，现在只存放vcs电视墙预案)
	if( NULL != pbyActualExtraDataBuf && NULL != pbyExInfoBuf)
	{
		SaveConfExtraPlanDataToFile( tConfStore, pbyActualExtraDataBuf,wActualExtradataLen );

		if (!cDefConfConfId.IsNull())
		{
			//缺省会议保存预案和勾选信息[5/28/2013 chendaiwei]
			SaveConfExtraPlanDataToFile( tConfStore, pbyActualExtraDataBuf,wActualExtradataLen,TRUE );
		}
	}

	SAFE_DEL_ARRAY(pbyExtraDataBuf);
	SAFE_DEL_ARRAY(pbyExInfoBuf);
	SAFE_DEL_ARRAY(pbyActualExtraDataBuf);
	
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
	BOOL32 bRet = FALSE;
	if (NULL != hHeadFile)
	{
		bRet = TRUE;
        fread((s8*)pacConfId, dwOutBufLen, 1, hHeadFile);				
        fclose(hHeadFile);
	}

    EndRWConfInfoFile();

	return bRet;
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
	BOOL32 bRet = FALSE;
    if (NULL != hHeadFile)
    {
		bRet = TRUE;
        fwrite((s8*)pacConfId, dwOutBufLen, 1, hHeadFile);
        fclose(hHeadFile);        
    }
	
    EndRWConfInfoFile();
	
	return bRet;
}

/*=============================================================================
  函 数 名： BackConfHeadInfo
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void BackConfHeadInfo( void )
{
	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];
	if (GetAllConfHeadFromFile(acConfId, sizeof(acConfId)))
	{
		FILE *hHeadFile = NULL;
		s8    achFullName[64];

		sprintf(achFullName, "%s/%s", DIR_DATA, CONFHEADINFO_BACKUP);
		// guzh [4/3/2007] 缺省会议单独保存,这里直接覆盖即可
		if (!BeginRWConfInfoFile())
		{
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[BackConfHeadInfo] BeginRWConfInfoFile fail\n");
			return;
		}
		hHeadFile = fopen(achFullName, "wb");
		if (NULL != hHeadFile)
		{
			fwrite((s8*)acConfId, sizeof(acConfId), 1, hHeadFile);
			fclose(hHeadFile);        
		}
		else
		{
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[BackConfHeadInfo] open confback fail\n");
		}
		EndRWConfInfoFile();
		return;
	}

	LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[BackConfHeadInfo]GetAllConfHeadFromFile fail\n");
	return;	

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
			   [BOOL32] bReadAllData TRUE读取所有数据（包括预案），以方便主备数据倒换；FALSE读取除预案以外的数据
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/12/27  4.0			万春雷                  创建
    07/04/03    4.0         顾振华                  修改
=============================================================================*/
BOOL32 GetConfDataFromFile(u8 byIndex, CConfId cConfId, u8 *pbyBuf, u32 dwInBufLen, u16 &wOutBufLen, BOOL32 bReadAllData)
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
	u32 dwSpace = 0;

	fread( achConfFileHead, strlen(CONFFILEHEAD), 1, hConfFile );
	//1)V4R6文件头  [11/19/2011 chendaiwei]
	if( 0 == memcmp( achConfFileHead, CONFFILEHEAD, strlen(CONFFILEHEAD) ) )
	{
		fread( &wFileTConfInfoSize, sizeof(u16), 1, hConfFile );
		wFileTConfInfoSize = ntohs(wFileTConfInfoSize);
		fread( &dwSpace, sizeof(u32), 1, hConfFile );
		wOutBufLen = wOutBufLen - strlen(CONFFILEHEAD) - sizeof(u16) - sizeof(u32);

		// 内存保护
		if ( dwInBufLen < (u32)wOutBufLen || 0 == wFileTConfInfoSize)
		{
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetConfDataFromFile] The buf length is unenough!\n" );
			EndRWConfInfoFile();
			fclose(hConfFile);
			return FALSE;
		}

		if( bReadAllData )
		{
			fread(pbyBuf,wOutBufLen,1,hConfFile);
		}
		else
		{

			//数据 普通 + 预案 + 扩展。跳过预案部分，获取其他信息 普通+扩展 [8/16/2012 chendaiwei]
			TConfStore tTempConfStore;
			TConfStore tUnPackConfstore;
			u16 wRemainFileLenth = (u16)nFileLen;	
			
			u16 wTempBufLen = min(wOutBufLen,sizeof(TConfStore));
			fread((u8*)&tTempConfStore,wTempBufLen,1,hConfFile);
			
			TPackConfStore *ptTempPackConfstore = (TPackConfStore *)&tTempConfStore;
			u16 wUnPackConfDataLen = 0;
			UnPackConfStore(ptTempPackConfstore,tUnPackConfstore,wUnPackConfDataLen,FALSE);
			//wUnPackConfDataLen -= 1;
			fseek(hConfFile,strlen(CONFFILEHEAD) + sizeof(u16) + sizeof(u32),SEEK_SET);
			wRemainFileLenth = wRemainFileLenth - strlen(CONFFILEHEAD) - sizeof(u16) - sizeof(u32);

			BOOL32 bIsFix2or8000HTemplate = FALSE;
			u8 byVcAutoModeExtraLen = 0;
			if(IsV4R6B2VcsTemplate(hConfFile,(u16)nFileLen))
			{
				bIsFix2or8000HTemplate= TRUE;
				byVcAutoModeExtraLen = 1;//传出的pbyBuf应该包括m_byVCAutoMode信息[11/13/2012 chendaiwei]
				wUnPackConfDataLen--;
			}
			
			if( wRemainFileLenth >= wUnPackConfDataLen )
			{
				fread(pbyBuf,wUnPackConfDataLen,1,hConfFile);
				wRemainFileLenth -= wUnPackConfDataLen;

				if(bIsFix2or8000HTemplate)
				{
					pbyBuf[wUnPackConfDataLen] = 0;
				}
			}
			else
			{
				LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetConfDataFromFile] conf.%s wUnPackConfDataLen.%d >wRemainFileLenth.%d,error!\n",tTempConfStore.m_tConfInfo.GetConfName(),wUnPackConfDataLen,wRemainFileLenth );
				EndRWConfInfoFile();
				fclose(hConfFile);
				return FALSE;
			}


			BOOL32 bIgnoreUnpack = FALSE; //标识是否跳出解析[8/17/2012 chendaiwei]
			u16 wPlanLen = 0;
			if( wRemainFileLenth == 0)
			{
				LogPrint( LOG_LVL_WARNING, MID_PUB_ALWAYS, "[GetConfDataFromFile]conf.%s wRemainFileLenth == 0, it must v4r6 template without plan!\n",tTempConfStore.m_tConfInfo.GetConfName());
				*(u16*)(pbyBuf+wUnPackConfDataLen+byVcAutoModeExtraLen) = 0;

				bIgnoreUnpack = TRUE;
			}
			else if( wRemainFileLenth >= sizeof(u16))
			{
				fread(&wPlanLen,sizeof(u16),1,hConfFile);
				wRemainFileLenth -= sizeof(u16);
			}
			else
			{
				LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetConfDataFromFile] conf.%s wRemainFileLenth.%d < sizeof(u16) plan length,error!\n",tTempConfStore.m_tConfInfo.GetConfName(),wRemainFileLenth );
				EndRWConfInfoFile();
				fclose(hConfFile);
				return FALSE;
			}

			if (!bIgnoreUnpack)
			{
				wPlanLen = ntohs(wPlanLen);
				if(wRemainFileLenth >= wPlanLen)
				{
					fseek(hConfFile,wPlanLen,SEEK_CUR);
					wRemainFileLenth -= wPlanLen;
				}
				else
				{
					LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetConfDataFromFile] conf.%s wPlanLen.%d > wRemainFileLenth.%d,error!\n",tTempConfStore.m_tConfInfo.GetConfName(),wPlanLen,wRemainFileLenth );
					EndRWConfInfoFile();
					fclose(hConfFile);
					return FALSE;
				}

				u16 wExInfoLen = 0;
				if( wRemainFileLenth == 0)
				{
					LogPrint( LOG_LVL_WARNING, MID_PUB_ALWAYS, "[GetConfDataFromFile]conf.%s wRemainFileLenth == 0, it must v4r6 template which has plan without Ex-info!\n",tTempConfStore.m_tConfInfo.GetConfName());
				}
				else if(wRemainFileLenth >= sizeof(u16))
				{
					fread(&wExInfoLen,sizeof(u16),1,hConfFile);
					wRemainFileLenth -= sizeof(u16);
				}
				else
				{
					LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetConfDataFromFile]cons.%s wRemainFileLenth.%d < sizeof(u16) ex-info len,error!\n",tTempConfStore.m_tConfInfo.GetConfName(),wRemainFileLenth );
					EndRWConfInfoFile();
					fclose(hConfFile);
					return FALSE;
				}
				
				*(u16*)(pbyBuf+wUnPackConfDataLen+byVcAutoModeExtraLen) = wExInfoLen;
				wExInfoLen = ntohs(wExInfoLen);
				
				if(wRemainFileLenth >= wExInfoLen)
				{
					fread(pbyBuf+wUnPackConfDataLen+sizeof(u16),wExInfoLen,1,hConfFile);
					wRemainFileLenth -= wExInfoLen;
				}
				else
				{
					LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetConfDataFromFile]conf.%s wRemainFileLenth.%d < wExInfoLen.%d,error!\n",tTempConfStore.m_tConfInfo.GetConfName(),wRemainFileLenth,wExInfoLen );
					EndRWConfInfoFile();
					fclose(hConfFile);
					return FALSE;
				}
			}
		}

		fclose(hConfFile);
		hConfFile = NULL;
	}
	//2)老版本
	else
	{
		fseek(hConfFile, 0, SEEK_SET);
		
		// 内存保护
		if(dwInBufLen < wOutBufLen )
		{
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetConfDataFromFile] The input buf length(%d) is less than output buf(%d)!\n", dwInBufLen, wOutBufLen );
			fclose(hConfFile);
			EndRWConfInfoFile();
			hConfFile = NULL;
			return FALSE;
		}

		//读会议信息
		wOutBufLen = ( wOutBufLen <= sizeof(TPackConfStore) ) ? wOutBufLen : sizeof(TPackConfStore);
		fread(pbyBuf, wOutBufLen, 1, hConfFile);
		fclose(hConfFile);
		hConfFile = NULL;
		TPackConfStore *ptTemp = (TPackConfStore *)pbyBuf;
		ptTemp->m_byMtNum = 0;
		ptTemp->m_wAliasBufLen = 0;
	}

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
BOOL32 SetConfDataToFile(u8 byIndex, CConfId &cConfId, u8 *pbyBuf, u16 wInBufLen,BOOL32 bWriteAllData)
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
    if (GetConfDataFromFile(byIndex, cConfId, (u8*)&tStore, sizeof(tStore), wOutLen,bWriteAllData))
    {
		u16 wcompareLen = max(wInBufLen,wOutLen);
        if ( memcmp(&tStore, pbyBuf, wcompareLen) == 0)
        {
            // 内容相同，不需要再次保存
            return TRUE;
        }
		else
		{
			LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[GetConfDataFromFile tick.%d] diff from file,so need to restore\n", OspTickGet());
		}
    }

    if ( !BeginRWConfInfoFile() )
    {
        return FALSE;                
    }

    FILE *hConfFile = NULL;
    s8    achFullName[64];	
    sprintf(achFullName, "%s/%s%d%s", DIR_DATA, CONFINFOFILENAME_PREFIX, byIndex, CONFINFOFILENAME_POSTFIX);

	LogPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[SetConfDataToFile tick.%d] save conf to file:%s\n", OspTickGet(), achFullName);

	hConfFile = fopen(achFullName, "wb");
	BOOL32 bRet = FALSE;
	if (NULL != hConfFile)
	{
		bRet = TRUE;
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

	return bRet;
}
/*=============================================================================
    函 数 名： GetUnProConfDataToFileLen
    功    能： unprocconfinfo_confindex.dat文件中读取某一会议模板不需要处理的数据总长
    算法实现： 
    全局变量： 
    参    数： [IN]   u8  byIndex
    返 回 值： u32
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		创建人		走读人    
    08/11/26                付秀华
=============================================================================*/
u32 GetUnProConfDataToFileLen(u8 byIndex)
{
	//可保存缺省会议信息
	if (byIndex > MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE)
	{
		return 0;
	}
	
    if ( !BeginRWConfInfoFile() )
    {
        return 0;                
    }

	u32 dwFileLen = 0;

    FILE *hConfFile = NULL;
    s8    achFullName[64];	
    sprintf(achFullName, "%s/%s%d%s", DIR_DATA, UNPROCCONFINFOFILENAME_PREFIX, byIndex, CONFINFOFILENAME_POSTFIX);

	hConfFile = fopen(achFullName, "rb");
	if (NULL != hConfFile)
	{
		fseek(hConfFile, 0, SEEK_END);
		dwFileLen = ftell(hConfFile);
	    fclose(hConfFile);
	}
	EndRWConfInfoFile();

	return dwFileLen;
}
/*=============================================================================
    函 数 名： GetUnProConfDataToFile
    功    能： unprocconfinfo_confindex.dat文件中读取某一会议模板不需要处理的数据
    算法实现： 
    全局变量： 
    参    数： [IN]   u8  byIndex
               [IN/OUT]   u8 *pbyBuf mcu不需要处理的会议模板相关信息(如分组信息)
               [IN/OUT]   u32 dwOutBufLen 主机序,传入需要读取的长度;传出实际读取的长度
			   [IN]       u32 dwBeginPos  读取的起始位

    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		创建人		走读人    
    08/11/26                付秀华
=============================================================================*/
BOOL32 GetUnProConfDataToFile(u8 byIndex, s8 *pbyBuf, u32& dwOutBufLen, u32 dwBeginPos)
{
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
	BOOL32 bRet = FALSE;
	if (NULL != hConfFile)
	{
		bRet = TRUE;
		fseek(hConfFile, 0, SEEK_END);
		u32 dwTotalLen = ftell(hConfFile);
		fseek(hConfFile, dwBeginPos, SEEK_SET);

		if (dwBeginPos > dwTotalLen)
		{
			dwOutBufLen = 0;
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetUnProConfDataToFile]beginpos(%d) bigger than totallen(%d)\n",
				      dwBeginPos, dwTotalLen);
		}
		else
		{
			dwOutBufLen = min(dwOutBufLen, dwTotalLen - dwBeginPos);
		}

		fread(pbyBuf, dwOutBufLen, 1, hConfFile);

// 		if (!wBeginPos)
// 		{
// 			u16 dwLen = *(u16*)pbyBuf;
// 			dwLen = htons(dwLen);
// 			memcpy(pbyBuf, &dwLen, sizeof(u16));
// 		}
	    fclose(hConfFile);
	}

    EndRWConfInfoFile();

	return bRet;
}

/*=============================================================================
    函 数 名： SetUnProConfDataToFile
    功    能： 将某一会议模板不需要处理的数据直接存储到对应的unprocconfinfo_confindex.dat文件中
    算法实现： 
    全局变量： 
    参    数： [IN]   u8  byIndex
               [IN]   s8 *pbyBuf mcu不需要处理的会议模板相关信息(如分组信息)
               [IN]   u32 dwInBufLen 主机序
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		创建人		走读人    
    08/11/26                付秀华
=============================================================================*/
BOOL32 SetUnProConfDataToFile(u8 byIndex, s8 *pbyBuf, u32 dwInBufLen, u8 byOverWrite)
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

	if (byOverWrite)
	{
		hConfFile = fopen(achFullName, "wb");
	}
	else
	{
		hConfFile = fopen(achFullName, "a+b");
	}

	BOOL32 bRet = FALSE;

	if (NULL != hConfFile)
	{
		bRet = TRUE;
        fwrite( pbyBuf, dwInBufLen, 1, hConfFile);
	    fclose(hConfFile);
	}	
	
    EndRWConfInfoFile();

	return bRet;
}

/*=============================================================================
    函 数 名： UnPackConfInfoEx
    功    能： Un pack数据，存储到TConfInfoEx中
    算法实现： 
    全局变量： 
    参    数： [IN][OUT]  TConfInfoEx &tConfInfoEx 保存unpack后的数据
               [IN] const u8 *pbyBuf 待unpack的数据
               [IN][OUT] u16 &wUnPackConfExLen unPack后的数据缓冲长度 主机序
			   [IN][OUT] bool32 &bExistUnknowInfo 存在本MCU无法解析的信息，置为TRUE
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2011/12/29  4.0			chendaiwei                  创建
=============================================================================*/
void   UnPackConfInfoEx(TConfInfoEx & tConfInfoEx, const u8 *pbyInBuf,
						u16 &wUnPackConfExLen, BOOL32 &bExistUnknowInfo,
						THduVmpModuleOrTHDTvWall *ptHduVmpModule ,
						TVmpModuleInfo *ptVmpModule,
						u8 *pbyEncoding)
{
	//扩展信息解析  u16扩展信息总长度+ u8信息类型A + u16信息类型A字节数 + 信息类型A数据（前面标识字节数大小）+ u16信息类型B......[12/29/2011 chendaiwei]
	u16 wConfExSize = ntohs( *(u16*)pbyInBuf);
	wUnPackConfExLen = wConfExSize + sizeof(u16);
	pbyInBuf = pbyInBuf + sizeof(u16);
	bExistUnknowInfo = FALSE;

	s32 nConfExSize = wConfExSize;

	THduVmpChannlInfo m_atHduVmpChnInfo[(HDUID_MAX-HDUID_MIN+1)*MAXNUM_HDU_CHANNEL];
	memset(&m_atHduVmpChnInfo[0],0,sizeof(m_atHduVmpChnInfo));
	u8 byHduVmpChnlCount = 0;

	// lang [pengguofeng 4/12/2013]
	if ( pbyEncoding != NULL )
	{
		*pbyEncoding = emenCoding_GBK; //默认是GBK编码
	}

	while( nConfExSize > 0)
	{
		u16 wLength = 0;
		emCommunicateType etype = (emCommunicateType)(*pbyInBuf);
		pbyInBuf ++;
		
		switch(etype)
		{
		case emEncodingType:
			// 参数为空，不作拷贝，但是需要正常偏移 [pengguofeng 5/21/2013]
// 			if ( pbyEncoding != NULL )
			{
				wLength = ntohs(*(u16*)pbyInBuf); //长度
				pbyInBuf = pbyInBuf + sizeof(u16);
				LogPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[UnPackConfInfoEx]emEncording length:%d\n", wLength);

				if ( pbyEncoding != NULL ) //拷贝
				{
					memcpy(pbyEncoding, pbyInBuf, wLength);
				}
				pbyInBuf = pbyInBuf + wLength;
			}			
			break;
		case emMainStreamCapEx:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			memcpy(&tConfInfoEx,pbyInBuf,wLength);
			pbyInBuf = pbyInBuf + wLength;
			
			break;
		case emDoubleStreamCapEx:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tConfInfoEx.SetDoubleStreamCapEx((TVideoStreamCap *)pbyInBuf,wLength/sizeof(TVideoStreamCap));
			pbyInBuf = pbyInBuf +  wLength;
			
			break;
		case emMainAudioTypeDesc:
			wLength = ntohs(*(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tConfInfoEx.SetAudioTypeDesc((TAudioTypeDesc*)pbyInBuf,wLength/sizeof(TAudioTypeDesc));
			pbyInBuf = pbyInBuf + wLength;
			
			break;
		case emScheduleDurationDateEx:
			wLength = ntohs(*(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tConfInfoEx.SetDurationDate((TDurationDate*)pbyInBuf);
			pbyInBuf = pbyInBuf + wLength;
			
			break;
		case emScheduleNextStartTimeEx:
			wLength = ntohs(*(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16);
			tConfInfoEx.SetNextStartTime((TKdvTime*)pbyInBuf);
			pbyInBuf = pbyInBuf + wLength;
			
			break;

		case emVmpChnnlMember:
			{
				wLength = ntohs( *(u16*)pbyInBuf);
				pbyInBuf = pbyInBuf + sizeof(u16);
				
				TVmpChnnlMember aTmpChnlMember[MAXNUM_MPU2VMP_MEMBER - MAXNUM_MPUSVMP_MEMBER];
				memset(aTmpChnlMember,0,sizeof(aTmpChnlMember));
				
				if( wLength/sizeof(TVmpChnnlMember) <= (MAXNUM_MPU2VMP_MEMBER - MAXNUM_MPUSVMP_MEMBER)
					&& ptVmpModule != NULL )
				{
					memcpy(aTmpChnlMember,(TVmpChnnlMember *)pbyInBuf,wLength);
					
					ptVmpModule->SetVmpChnnlMember(&aTmpChnlMember[0],wLength/sizeof(TVmpChnnlMember));
				}
				
				pbyInBuf = pbyInBuf + wLength;
			}

			break;

		case emHduVmpChnnl:
			{
				wLength = ntohs(*(u16*)pbyInBuf);
				pbyInBuf = pbyInBuf + sizeof(u16);
				THduVmpChnnl tVmpChnnl[(HDUID_MAX-HDUID_MIN+1)*MAXNUM_HDU_CHANNEL];
				u8 byVmpChnlNum = wLength/sizeof(THduVmpChnnl);
				if( byVmpChnlNum <= (HDUID_MAX-HDUID_MIN+1)*MAXNUM_HDU_CHANNEL)
				{
					u8 byIdx = 0;
					memcpy(&tVmpChnnl[0],pbyInBuf,wLength);
					for(byIdx = 0; byIdx < byVmpChnlNum; byIdx++)
					{
						if( byHduVmpChnlCount <(HDUID_MAX-HDUID_MIN+1)*MAXNUM_HDU_CHANNEL)
						{
							m_atHduVmpChnInfo[byHduVmpChnlCount].m_byHduEqpId = tVmpChnnl[byIdx].m_byHduEqpId;
							m_atHduVmpChnInfo[byHduVmpChnlCount].m_byChIdx = tVmpChnnl[byIdx].m_byChnnlIdx;
							m_atHduVmpChnInfo[byHduVmpChnlCount].m_byVmpStyle = tVmpChnnl[byIdx].m_byVmpStyle;
							
							byHduVmpChnlCount++;
						}
					}
					
					pbyInBuf = pbyInBuf + wLength;
					
					s32 nTmpConfExSize = nConfExSize - (s32)sizeof(u8) - (s32)sizeof(u16) - (s32)wLength;
					
					if(nTmpConfExSize > 0)
					{
						emCommunicateType eSubtype = (emCommunicateType)(*pbyInBuf);
						if (eSubtype == emHduVmpSubChnnl)
						{
							pbyInBuf ++;

							wLength = ntohs(*(u16*)pbyInBuf);
							pbyInBuf = pbyInBuf + sizeof(u16);
							
							THduVmpSubChnnl atHduVmpSubChnnl[HDU_MODEFOUR_MAX_SUBCHNNUM];
							u8 byVmpSubChnlNum = wLength/sizeof(THduVmpSubChnnl);
							if(byVmpSubChnlNum <= HDU_MODEFOUR_MAX_SUBCHNNUM && byHduVmpChnlCount >=1)
							{
								memcpy(&atHduVmpSubChnnl[0],pbyInBuf,wLength);
								for( byIdx = 0; byIdx < byVmpSubChnlNum; byIdx++)
								{
									m_atHduVmpChnInfo[byHduVmpChnlCount-1].m_abyMemberType[atHduVmpSubChnnl[byIdx].m_bySubChnnlIdx] = atHduVmpSubChnnl[byIdx].m_byMemberType;
									m_atHduVmpChnInfo[byHduVmpChnlCount-1].m_abyTvWallMember[atHduVmpSubChnnl[byIdx].m_bySubChnnlIdx] = atHduVmpSubChnnl[byIdx].m_byMember;
								}
							}

							pbyInBuf = pbyInBuf + wLength;

							nConfExSize = nTmpConfExSize;
						}
					}
				}
			}

			break;

		default:
			wLength = ntohs( *(u16*)pbyInBuf);
			pbyInBuf = pbyInBuf + sizeof(u16) +wLength;
			bExistUnknowInfo = TRUE;
			
			break;
		}
		
		nConfExSize = nConfExSize - (s32)sizeof(u8) - (s32)sizeof(u16) - (s32)wLength;
	}

	if(byHduVmpChnlCount != 0 && ptHduVmpModule != NULL)
	{
		ptHduVmpModule->SetHduVmpChnnlInfo(&m_atHduVmpChnInfo[0],byHduVmpChnlCount);
	}

	return;
}

/*=============================================================================
    函 数 名： PackConfInfoEx
    功    能： Pack数据，存储到pbybuf中
    算法实现： 
    全局变量： 
    参    数： [IN]  TConfInfoEx &tConfInfoEx 待pack数据
               [IN][OUT]u8 *pbyBuf pack后数据存储位置
               [IN][OUT] u16 &wPackConfDataLen Pack后的数据长度 主机序
    返 回 值： BOOL32 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2011/12/29  4.0			chendaiwei                  创建
=============================================================================*/
void   PackConfInfoEx(const TConfInfoEx &tConfInfoEx,u8 * pbyOutbuf, u16 &wPackConfDataLen,
					  THduVmpModuleOrTHDTvWall *ptHduVmpModule,
					  TVmpModuleInfo *ptVmpModule,
					  u8 *pbyEncoding)
{
	//扩展信息解析  u16扩展信息总长度+ u8信息类型A + u16信息类型A字节数 + 信息类型A数据（前面标识字节数大小）+ u16信息类型B......[12/29/2011 chendaiwei]
	wPackConfDataLen = 0;
	u8 *pbyConfExSizeBuf = pbyOutbuf;
	u8 *pbyConfExBuf = pbyConfExSizeBuf + 2;

	// 多国语言:保存4个字节 [pengguofeng 4/12/2013]
	if ( pbyEncoding != NULL )
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[PackConfInfoEx Tick.%d]Add Mcu EncodingType to pbyOutbuf\n", OspTickGet());
		*pbyConfExBuf = (u8)emEncodingType;
		pbyConfExBuf++;
		wPackConfDataLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(u8));//长度，方便解析:net order
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackConfDataLen = wPackConfDataLen + sizeof(u16);
		
		memcpy(pbyConfExBuf, pbyEncoding, sizeof(u8));
		pbyConfExBuf = pbyConfExBuf + sizeof(u8);
		wPackConfDataLen = wPackConfDataLen + sizeof(u8);
	}
	else
	{
		LogPrint(LOG_LVL_WARNING, MID_MCU_CONF, "[PackConfInfoEx Tick.%d]param pbyEncoding is NULL\n", OspTickGet());
	}

	//主流勾选
	TVideoStreamCap tMSCap[MAX_CONF_CAP_EX_NUM];
	u8 byActualNum = MAX_CONF_CAP_EX_NUM;
	tConfInfoEx.GetMainStreamCapEx(tMSCap,byActualNum);

	if(byActualNum !=0)
	{
		*pbyConfExBuf = (u8)emMainStreamCapEx;
		pbyConfExBuf++;
		wPackConfDataLen++;
		
		*(u16*)pbyConfExBuf = htons(byActualNum*sizeof(TVideoStreamCap));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackConfDataLen = wPackConfDataLen + sizeof(u16);
		
		memcpy(pbyConfExBuf,tMSCap,byActualNum*sizeof(TVideoStreamCap));
		pbyConfExBuf = pbyConfExBuf + byActualNum*sizeof(TVideoStreamCap);
		wPackConfDataLen = wPackConfDataLen + byActualNum*sizeof(TVideoStreamCap);
	}

	//双流勾选
	TVideoStreamCap tDSCap[MAX_CONF_CAP_EX_NUM];
	u8 byActualDSNum = MAX_CONF_CAP_EX_NUM;
	tConfInfoEx.GetDoubleStreamCapEx(tDSCap,byActualDSNum);

	if(byActualDSNum !=0)
	{
		*pbyConfExBuf = (u8)emDoubleStreamCapEx;
		pbyConfExBuf++;
		wPackConfDataLen++;
		
		*(u16*)pbyConfExBuf = htons(byActualDSNum*sizeof(TVideoStreamCap));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackConfDataLen = wPackConfDataLen + sizeof(u16);
		
		memcpy(pbyConfExBuf,tDSCap,byActualDSNum*sizeof(TVideoStreamCap));
		pbyConfExBuf = pbyConfExBuf + byActualDSNum*sizeof(TVideoStreamCap);
		wPackConfDataLen = wPackConfDataLen + byActualDSNum*sizeof(TVideoStreamCap);
	}
	
	//音频能力描述
	TAudioTypeDesc  atAudioCap[MAXNUM_CONF_AUDIOTYPE];
	u8 byActualAudioNum = tConfInfoEx.GetAudioTypeDesc(atAudioCap);
	if( byActualAudioNum > MAXNUM_CONF_AUDIOTYPE )
	{
		byActualAudioNum = MAXNUM_CONF_AUDIOTYPE;
	}	 
	if(byActualAudioNum > 0)
	{
		*pbyConfExBuf = (u8)emMainAudioTypeDesc;
		pbyConfExBuf++;
		wPackConfDataLen++;
		
		*(u16*)pbyConfExBuf = htons(byActualAudioNum*sizeof(TAudioTypeDesc));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackConfDataLen = wPackConfDataLen + sizeof(u16);
		
		memcpy(pbyConfExBuf,atAudioCap,byActualAudioNum*sizeof(TAudioTypeDesc));
		pbyConfExBuf = pbyConfExBuf + byActualAudioNum*sizeof(TAudioTypeDesc);
		wPackConfDataLen = wPackConfDataLen + byActualAudioNum*sizeof(TAudioTypeDesc);
	}
	//TDuraDate有设置才打包，无设置，TConfInfoEx默认构造的TDuraDate就是空
	TDurationDate tDuraDate = tConfInfoEx.GetDurationDate();
	if ( !tDuraDate.IsDuraStartTimeNull() || !tDuraDate.IsDuraEndTimeNull() )
	{
		*pbyConfExBuf = (u8)emScheduleDurationDateEx;
		pbyConfExBuf++;
		wPackConfDataLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(TDurationDate));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackConfDataLen = wPackConfDataLen + sizeof(u16);
		
		memcpy(pbyConfExBuf,&tDuraDate,sizeof(TDurationDate));
		pbyConfExBuf = pbyConfExBuf + sizeof(TDurationDate);
		wPackConfDataLen = wPackConfDataLen + sizeof(TDurationDate);
	}
	//TKdvTime有设置才打包，无设置，TConfInfoEx默认构造的TKdvTime就是空
	TKdvTime tNextStartTime = tConfInfoEx.GetNextStartTime();
	if ( !tConfInfoEx.IsNextStartTimeNull() )
	{
		*pbyConfExBuf = (u8)emScheduleNextStartTimeEx;
		pbyConfExBuf++;
		wPackConfDataLen++;
		
		*(u16*)pbyConfExBuf = htons(sizeof(TKdvTime));
		pbyConfExBuf = pbyConfExBuf + sizeof(u16);
		wPackConfDataLen = wPackConfDataLen + sizeof(u16);
		
		memcpy(pbyConfExBuf,&tNextStartTime,sizeof(TKdvTime));
		pbyConfExBuf = pbyConfExBuf + sizeof(TKdvTime);
		wPackConfDataLen = wPackConfDataLen + sizeof(TKdvTime);
	}

	if(ptHduVmpModule != NULL)
	{
		if(ptHduVmpModule->IsUnionStoreHduVmpInfo())
		{
			for( u8 byLoopChIdx = 0; byLoopChIdx < (HDUID_MAX-HDUID_MIN+1)*MAXNUM_HDU_CHANNEL;byLoopChIdx++)
			{
				if(ptHduVmpModule->m_union.m_atHduVmpChnInfo[byLoopChIdx].IsValid())
				{
					*pbyConfExBuf = (u8)emHduVmpChnnl;
					pbyConfExBuf++;
					wPackConfDataLen++;
					
					*(u16*)pbyConfExBuf = htons(sizeof(THduVmpChnnl));
					pbyConfExBuf = pbyConfExBuf + sizeof(u16);
					wPackConfDataLen = wPackConfDataLen + sizeof(u16);
					
					THduVmpChnnl tHduVmpChnl;
					tHduVmpChnl.m_byHduEqpId = ptHduVmpModule->m_union.m_atHduVmpChnInfo[byLoopChIdx].m_byHduEqpId;
					tHduVmpChnl.m_byVmpStyle = ptHduVmpModule->m_union.m_atHduVmpChnInfo[byLoopChIdx].m_byVmpStyle;
					tHduVmpChnl.m_byChnnlIdx = ptHduVmpModule->m_union.m_atHduVmpChnInfo[byLoopChIdx].m_byChIdx;
					memcpy(pbyConfExBuf,&tHduVmpChnl,sizeof(THduVmpChnnl));
					pbyConfExBuf = pbyConfExBuf + sizeof(THduVmpChnnl);
					wPackConfDataLen = wPackConfDataLen + sizeof(THduVmpChnnl);
					
					THduVmpSubChnnl atSubChnnl[HDU_MODEFOUR_MAX_SUBCHNNUM];
					memset(&atSubChnnl[0],0,sizeof(atSubChnnl));
					u8 byCount = 0;
					for( u8 byLoopSubIdx = 0; byLoopSubIdx <HDU_MODEFOUR_MAX_SUBCHNNUM; byLoopSubIdx++)
					{
						u8 byMemberType = ptHduVmpModule->m_union.m_atHduVmpChnInfo[byLoopChIdx].m_abyMemberType[byLoopSubIdx];
						u8 byMember = ptHduVmpModule->m_union.m_atHduVmpChnInfo[byLoopChIdx].m_abyTvWallMember[byLoopSubIdx];
						//有合法终端ID和跟随类型
						if(byMemberType != TW_MEMBERTYPE_NULL || byMember != 0)
						{
							atSubChnnl[byCount].m_bySubChnnlIdx = byLoopSubIdx;
							atSubChnnl[byCount].m_byMember = byMember;
							atSubChnnl[byCount].m_byMemberType = byMemberType;
							
							byCount++;
						}
					}
					
					if(byCount != 0)
					{
						*pbyConfExBuf = (u8)emHduVmpSubChnnl;
						pbyConfExBuf++;
						wPackConfDataLen++;
						
						*(u16*)pbyConfExBuf = htons(sizeof(THduVmpSubChnnl)*byCount);
						pbyConfExBuf = pbyConfExBuf + sizeof(u16);
						wPackConfDataLen = wPackConfDataLen + sizeof(u16);
						
						memcpy(pbyConfExBuf,&atSubChnnl[0],sizeof(THduVmpSubChnnl)*byCount);
						pbyConfExBuf = pbyConfExBuf + byCount*sizeof(THduVmpSubChnnl);
						wPackConfDataLen = wPackConfDataLen + byCount*sizeof(THduVmpSubChnnl);
					}
				}
			}
		}
	}

	//VMP模板20风格之后的后通道信息
	if(ptVmpModule != NULL)
	{
		TVmpChnnlMember atVmpChnl[MAXNUM_MPU2VMP_MEMBER-MAXNUM_MPUSVMP_MEMBER];
		memset(atVmpChnl,0,sizeof(atVmpChnl));
		u8 byVmpChnlCount = 0;
		for( u8 byIdx = MAXNUM_MPUSVMP_MEMBER; byIdx < MAXNUM_MPU2VMP_MEMBER;byIdx++)
		{
			if(ptVmpModule->m_abyMemberType[byIdx] != VMP_MEMBERTYPE_NULL
				|| ptVmpModule->m_abyVmpMember[byIdx] != 0)
			{
				atVmpChnl[byVmpChnlCount].m_byChnIdx = byIdx;
				atVmpChnl[byVmpChnlCount].m_byMemberType = ptVmpModule->m_abyMemberType[byIdx];
				atVmpChnl[byVmpChnlCount].m_byVmpMember = ptVmpModule->m_abyVmpMember[byIdx];
				byVmpChnlCount++;
			}
		}
		
		if ( byVmpChnlCount > 0 )
		{
			*pbyConfExBuf = (u8)emVmpChnnlMember;
			pbyConfExBuf++;
			wPackConfDataLen++;
			
			*(u16*)pbyConfExBuf = htons(byVmpChnlCount*sizeof(TVmpChnnlMember));
			pbyConfExBuf = pbyConfExBuf + sizeof(u16);
			wPackConfDataLen = wPackConfDataLen + sizeof(u16);
			
			memcpy(pbyConfExBuf,atVmpChnl,byVmpChnlCount*sizeof(TVmpChnnlMember));
			pbyConfExBuf = pbyConfExBuf + byVmpChnlCount*sizeof(TVmpChnnlMember);
			wPackConfDataLen = wPackConfDataLen + byVmpChnlCount*sizeof(TVmpChnnlMember);
		}
	}


	//Buffer总大小
	*(u16*)pbyConfExSizeBuf = htons(wPackConfDataLen);
	wPackConfDataLen = wPackConfDataLen + sizeof(u16); //加上首部两个字节

	return;
}

/*====================================================================
函数名       GetConfInfoExFromConfAttrb
功能        ：从会议属性信息中获取扩展分辨率勾选信息，存放在新的结构中
算法实现    ：
引用全局变量：
输入参数说明：[IN] TConfInfo 会议信息结构描述 
返回值说明  ：[OUT]TConfInfoEx 扩展能力集勾选信息
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
12/06/07    4.7         陈代伟          创建
====================================================================*/
TConfInfoEx GetConfInfoExFromConfAttrb(const TConfInfo& tConfInfo)
{
	TConfInfoEx tConfInfoEx;

	TConfAttrbEx tTConfAttrbEx = tConfInfo.GetConfAttrbEx();
	if(!tTConfAttrbEx.IsImaxModeConf())
	{
		TVideoStreamCap tCapEx[4];
		u8 byIdx = 0;
		
		//8000H勾选1080/30/25帧情况[2/14/2012 chendaiwei]
		if(tTConfAttrbEx.IsResEx1080())
		{
			u8 byfps = tConfInfo.GetMainVidUsrDefFPS();
			if(byfps > 30 )
			{
				byfps = tConfInfo.GetMainVidUsrDefFPS()/2;
			}
			TVideoStreamCap tTempCap(MEDIA_TYPE_H264,VIDEO_FORMAT_HD1080,byfps,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate());
			tCapEx[byIdx] = tTempCap;
			byIdx++;
		}

		if(tTConfAttrbEx.IsResEx720())
		{
			TVideoStreamCap tTempCap(MEDIA_TYPE_H264,VIDEO_FORMAT_HD720,tConfInfo.GetMainVidUsrDefFPS(),emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate());
			tCapEx[byIdx] = tTempCap;
			byIdx++;
		}
		
		if(tTConfAttrbEx.IsResEx4Cif())
		{	
			u8 byfps = tConfInfo.GetMainVidUsrDefFPS();
			if(byfps >= 30 )
			{
				byfps = 25;
			}
			
			TVideoStreamCap tTempCap(MEDIA_TYPE_H264,VIDEO_FORMAT_4CIF,byfps,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate());
			tCapEx[byIdx] = tTempCap;
			byIdx++;
		}
		
		if(tTConfAttrbEx.IsResExCif())
		{
			u8 byfps = tConfInfo.GetMainVidUsrDefFPS();
			if(byfps >= 30 )
			{
				byfps = 25;
			}
			
			TVideoStreamCap tTempCap(MEDIA_TYPE_H264,VIDEO_FORMAT_CIF,byfps,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate());
			tCapEx[byIdx] = tTempCap;
			byIdx++;
		}
		
		tConfInfoEx.SetMainStreamCapEx(&tCapEx[0],byIdx);

	}
	//IMax会议
	else
	{
		//主流
		TVideoStreamCap tImaxCapEx[5];
		u8 byIdx = 0;

		TVideoStreamCap t1080Cap(MEDIA_TYPE_H264,VIDEO_FORMAT_HD1080,30,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate());
		tImaxCapEx[byIdx] = t1080Cap;
		byIdx++;

		TVideoStreamCap t720Cap(MEDIA_TYPE_H264,VIDEO_FORMAT_HD720,60,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate());
		tImaxCapEx[byIdx] = t720Cap;
		byIdx++;

		TVideoStreamCap t72030Cap(MEDIA_TYPE_H264,VIDEO_FORMAT_HD720,30,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate());
		tImaxCapEx[byIdx] = t72030Cap;
		byIdx++;

		TVideoStreamCap t4cifCap(MEDIA_TYPE_H264,VIDEO_FORMAT_4CIF,25,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate());
		tImaxCapEx[byIdx] = t4cifCap;
		byIdx++;
		
		TVideoStreamCap tcifCap(MEDIA_TYPE_H264,VIDEO_FORMAT_CIF,25,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate());
		tImaxCapEx[byIdx] = tcifCap;
		byIdx++;

		tConfInfoEx.SetMainStreamCapEx(&tImaxCapEx[0],byIdx);

		//双流
		TVideoStreamCap tImaxDSCapEx[4];
		byIdx = 0;
		
		TVideoStreamCap t720DSCap(MEDIA_TYPE_H264,VIDEO_FORMAT_HD720,30,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate()*tConfInfo.GetDStreamScale()/100);
		tImaxDSCapEx[byIdx] = t720DSCap;
		byIdx++;
		
		TVideoStreamCap tXGADSCap(MEDIA_TYPE_H264,VIDEO_FORMAT_XGA,5,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate()*tConfInfo.GetDStreamScale()/100);
		tImaxDSCapEx[byIdx] = tXGADSCap;
		byIdx++;
		
		TVideoStreamCap t4cifDSCap(MEDIA_TYPE_H264,VIDEO_FORMAT_4CIF,25,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate()*tConfInfo.GetDStreamScale()/100);
		tImaxDSCapEx[byIdx] = t4cifDSCap;
		byIdx++;
		
		TVideoStreamCap tcifDSCap(MEDIA_TYPE_H264,VIDEO_FORMAT_CIF,25,emBpAttrb,tConfInfo.GetMainSimCapSet().GetVideoMaxBitRate()*tConfInfo.GetDStreamScale()/100);
		tImaxDSCapEx[byIdx] = tcifDSCap;
		byIdx++;

		tConfInfoEx.SetDoubleStreamCapEx(&tImaxDSCapEx[0],byIdx);
	}


	//对于老模板只有一种音频类型，并且只有单声道
	TAudioTypeDesc tMainAudioType(tConfInfo.GetMainAudioMediaType(),1);
	tConfInfoEx.SetAudioTypeDesc(&tMainAudioType,1);

	return tConfInfoEx;
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

	TConfAttrb tConfAttrb = ptPackConfStore->m_tConfInfo.GetConfAttrb();
	//电视墙模板 
	if (tConfAttrb.IsHasTvWallModule())
	{
		memcpy(pbyModuleBuf, (s8*)&tConfStore.m_tMultiTvWallModule, sizeof(TMultiTvWallModule));
		wPackConfDataLen += sizeof(TMultiTvWallModule);
		pbyModuleBuf += sizeof(TMultiTvWallModule);
	}
	//画面合成模板 
	if (tConfAttrb.IsHasVmpModule())
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
	//                  + 1 TMtAlias (本地终端备份)
	u8* pbyVCSInfoExBuf = pbyModuleBuf;  //VCS会议模板增加内容缓冲

	if (VCS_CONF == tConfStore.m_tConfInfo.GetConfSource())
	{

		*pbyVCSInfoExBuf++  = (u8)tConfStore.IsHDTWCfg();
		wPackConfDataLen += sizeof(u8);
		if (tConfStore.IsHDTWCfg())
		{
			memcpy(pbyVCSInfoExBuf, &tConfStore.m_tHDTWInfo, sizeof(THDTvWall));
			pbyVCSInfoExBuf  += sizeof(THDTvWall);
			wPackConfDataLen += sizeof(THDTvWall);
		}

		u8 byHduChnlNum = tConfStore.m_tHduModule.GetHduChnlNum( FALSE );
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
		
		//zhouyiliang 20100820 VCS 1+1 备份保存本地备份终端
		memcpy( pbyVCSInfoExBuf,&tConfStore.m_tVCSBackupChairMan,sizeof(TMtAlias) );
		wPackConfDataLen += sizeof(TMtAlias);
		
		// [4/20/2011 xliang] VCS VCAutoMode
		pbyVCSInfoExBuf += sizeof(TMtAlias);
		*pbyVCSInfoExBuf = tConfStore.m_byVCAutoMode;
		wPackConfDataLen += sizeof(u8);
		pbyVCSInfoExBuf += sizeof(u8);
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
BOOL32 UnPackConfStore( TPackConfStore *ptPackConfStore, TConfStore &tConfStore, u16& wPackConfDataLen, BOOL32 bUnPackExInfo )
{
	u16 wAliasBufLen = 0;
	u8* pbyAliasBuf  = NULL; //终端别名数组打包缓冲
	u8* pbyModuleBuf = NULL; //电视墙//画面合成模板缓冲
	
	wPackConfDataLen = 0;
	u8 *pbyInitPos = (u8*)ptPackConfStore; //记录指针初始位置[8/16/2012 chendaiwei]

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
	TConfAttrb tConfAttrb = ptPackConfStore->m_tConfInfo.GetConfAttrb();
	if (tConfAttrb.IsHasTvWallModule())
	{
		memcpy((s8*)&tConfStore.m_tMultiTvWallModule, pbyModuleBuf, sizeof(TMultiTvWallModule));
		pbyModuleBuf += sizeof(TMultiTvWallModule);
	}
	//画面合成模板 
	if (tConfAttrb.IsHasVmpModule())
	{
		memcpy((s8*)&tConfStore.m_atVmpModule, pbyModuleBuf, sizeof(TVmpModule));
		pbyModuleBuf += sizeof(TVmpModule);
	}

	//VCS信息[12/27/2011 chendaiwei]
	u8 * pbyVCSInfoBuf = pbyModuleBuf;
	if (VCS_CONF == ptPackConfStore->m_tConfInfo.GetConfSource())
	{	
		u8 byIsSupportHDTW = *pbyVCSInfoBuf++;
		if (byIsSupportHDTW)
		{
			pbyVCSInfoBuf += sizeof(THDTvWall);
		}
		
		u8 byHduNum = *pbyVCSInfoBuf++;
		if (byHduNum)
		{
			pbyVCSInfoBuf += byHduNum * sizeof(THduModChnlInfo);
		}
		
		u8 bySMCUExist = *pbyVCSInfoBuf++;
		if (bySMCUExist)
		{
			wAliasBufLen = htons(*(u16*)pbyVCSInfoBuf);
			pbyVCSInfoBuf += sizeof(u16);
			wAliasBufLen = ntohs(wAliasBufLen);
			pbyVCSInfoBuf += wAliasBufLen;					
		}
		
		// 会议模板中不包含分组信息
		pbyVCSInfoBuf++;

		//zhouyiliang 20100820 增加备份终端
		pbyVCSInfoBuf += sizeof(TMtAlias);
		
		// [4/20/2011 xliang] VCAutoMode
		pbyVCSInfoBuf ++;
	}

	if(bUnPackExInfo)
	{
		u16 wConfInfoExLength = ntohs(*(u16*)pbyVCSInfoBuf) + sizeof(u16);
		if(wConfInfoExLength <= CONFINFO_EX_BUFFER_LENGTH)
		{
			memcpy(tConfStore.m_byConInfoExBuf,pbyVCSInfoBuf,wConfInfoExLength);
			pbyVCSInfoBuf += wConfInfoExLength;
		}
		else
		{
			LogPrint(LOG_LVL_ERROR,MID_MCU_CONF,"[UnPackConfStore] %s buffer is out of memory, error!\n",tConfStore.m_tConfInfo.GetConfName());

			return FALSE;
		}
	}

	wPackConfDataLen = pbyVCSInfoBuf - pbyInitPos;

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
                            TVmpModule &tOutVmpModule,
							THduVmpModuleOrTHDTvWall &tHduVmpModule,
							TVmpModuleInfo &tVmpModuleEx)
{
    BOOL32 bReplace = FALSE;
	TConfAttrb tConfattrb = tConfStore.m_tConfInfo.GetConfAttrb();
		
	TConfInfoEx tConfInfoEx;
	u16 wPackConfExInfoLength = 0;
	BOOL32 bUnkownConfInfo = FALSE;
	THduVmpModuleOrTHDTvWall tOrgiHduVmpModule;
	TVmpModuleInfo tOrigVmpModuleEx25;
	UnPackConfInfoEx(tConfInfoEx,tConfStore.m_byConInfoExBuf, wPackConfExInfoLength,
						bUnkownConfInfo, &tOrgiHduVmpModule, &tOrigVmpModuleEx25);

    if (tConfattrb.IsHasTvWallModule())
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
    if (tConfattrb.IsHasVmpModule())
    {
        for (u8 byVmpIdx = 0; byVmpIdx < MAXNUM_MPUSVMP_MEMBER; byVmpIdx++)
        {
            if( byOldIdx == tConfStore.m_atVmpModule.m_abyVmpMember[byVmpIdx] )
            {
                tOutVmpModule.m_abyVmpMember[byVmpIdx] = byNewIdx;
                bReplace = TRUE;
            }
        }

		for ( u8 byExVmpIdx = MAXNUM_MPUSVMP_MEMBER; byExVmpIdx < MAXNUM_MPU2VMP_MEMBER;byExVmpIdx ++ )
		{
			if( byOldIdx == tOrigVmpModuleEx25.m_abyVmpMember[byExVmpIdx])
			{
				tVmpModuleEx.m_abyVmpMember[byExVmpIdx] = byNewIdx;
				bReplace = TRUE;
			}
		}
    }

	if (tOrgiHduVmpModule.IsUnionStoreHduVmpInfo())
	{
		for(u8 byLoopChnIdx = 0; byLoopChnIdx < (HDUID_MAX-HDUID_MIN+1)*MAXNUM_HDU_CHANNEL; byLoopChnIdx++)
		{
			if(tOrgiHduVmpModule.m_union.m_atHduVmpChnInfo[byLoopChnIdx].IsValid())
			{
				for (u8 bySubChnlIdx = 0; bySubChnlIdx < HDU_MODEFOUR_MAX_SUBCHNNUM; bySubChnlIdx++)
				{
					u8 byMtIndex =  tOrgiHduVmpModule.m_union.m_atHduVmpChnInfo[byLoopChnIdx].m_abyTvWallMember[bySubChnlIdx];
					u8 byMemberType = tOrgiHduVmpModule.m_union.m_atHduVmpChnInfo[byLoopChnIdx].m_abyMemberType[bySubChnlIdx];
					
					if( byMtIndex == byOldIdx )
					{
						tHduVmpModule.m_union.m_atHduVmpChnInfo[byLoopChnIdx].m_abyTvWallMember[bySubChnlIdx] = byNewIdx;

						bReplace = TRUE;
					}
				}
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
	case MEDIA_TYPE_G719:
		sprintf( szMediaType, "%s%c", "G.719", 0 );
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
	case VIDEO_FORMAT_384x272:
		sprintf( szResType, "%s%c", "384x272",0 );
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
	case VIDEO_FORMAT_640x544:
		sprintf( szResType, "%s%c", "640x544",0 );
		break;
	case VIDEO_FORMAT_320x272:
		sprintf( szResType, "%s%c", "320x272",0 );
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
	if (tConfInfo.GetCapSupport().GetDStreamType() == VIDEO_DSTREAM_MAIN ||
		tConfInfo.GetCapSupport().GetDStreamType() == VIDEO_DSTREAM_MAIN_H239 )
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
/*==============================================================================
函数名    :  IsConfFrm50OR60
功能      :  判断会议帧率是否是50 or 60
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
20100510                   周嘉麟                            创建
==============================================================================*/
BOOL32 IsConfFR50OR60(const TConfInfo &tConfInfo)
{
	u8 byConfFrm = 0;
	if(MEDIA_TYPE_H264 == tConfInfo.GetMainVideoMediaType())
	{
		byConfFrm = tConfInfo.GetMainVidUsrDefFPS();
	}
	else
	{
		byConfFrm = tConfInfo.GetMainVidFrameRate();
	}
	
	LogPrint( LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[IsConfFrm50OR60] ConfFrm is %d!\n", byConfFrm);
	
	if (byConfFrm == 50 || byConfFrm == 60)
	{
		return TRUE;	
	}
	return FALSE;
}


/*==============================================================================
函数名    :  IsDSFR50OR60
功能      :  判断双流帧率是否高帧率
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
20121009                   倪志俊                           创建
==============================================================================*/
BOOL32 IsDSFR50OR60(const TConfInfo &tConfInfo)
{
	u8 byConfFrm = tConfInfo.GetDStreamUsrDefFPS();
	
	LogPrint( LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[IsDSFR50OR60] DSFrm is %d!\n", byConfFrm);
	
	if (byConfFrm == 50 || byConfFrm == 60)
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
SEMHANDLE	g_hGetManuStr = NULL;						// GetManufactureStr专用

LPCSTR GetManufactureStr( u8 byManufacture )
{
	if (NULL == g_hGetManuStr)
	{
		if( !OspSemBCreate(&g_hGetManuStr))
		{
			OspSemDelete( g_hGetManuStr );
			g_hGetManuStr = NULL;
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetManufactureStr] create g_hGetManuStr failed!\n" );
		}
	}

	if (NULL != g_hGetManuStr)
	{
		OspSemTake(g_hGetManuStr);
	}

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
	case MT_MANU_AETHRA:
		sprintf( szManu, "%s%c", "Aethra", 0 );
        break;
	case MT_MANU_CHAORAN:
		sprintf( szManu, "%s%c", "ChaoRan", 0 );
        break;
	case MT_MANU_OTHER:
		sprintf( szManu, "%s%c", "Unknown", 0 );
		break;
	default: sprintf( szManu, "%s%c", "Unknown", 0);break;
	}

	if (NULL != g_hGetManuStr)
	{
		OspSemGive(g_hGetManuStr);
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
	/*lint -save -esym(529, EV_MCUMT_SAT_END)*/
	/*lint -save -e537*/

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

	/*lint -restore*/
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
        LogPrint( LOG_LVL_ERROR, MID_MCU_MIXER, "Start local mixer success!\n" );
	
    }
    else
    {
        LogPrint( LOG_LVL_ERROR, MID_MCU_MIXER, "Start local mixer failure!\n");
    }
#endif

	LogPrint(LOG_LVL_DETAIL,MID_MCU_MIXER,"[StartLocalMixer] byEqpId.%d\n",byEqpId);

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

	LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[StartLocalVmp] byEqpId.%d\n",byEqpId);
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

	// [9/26/2010 liuxu] 此处需要优化，PRS id在PRSID_MIN和PRSID_MAX之间
	// 另外，如果没有PRS设备连上去，应该有个标记，此处如果有标记，则省略以下for循环

	for( byEqpId=1; byEqpId <= MAXNUM_MCU_PERIEQP; byEqpId++ )
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
	tPrsCfg.dwLocalIP    = g_cMcuAgent.GetMpcIp();

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
	g_cMcuAgent.WritePrsTable(1, &tPrsParam);


	if( prsinit( &tPrsCfg ) )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_PRS, "Start local prs success!\n" );
	}
	else
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_PRS, "Start local prs failure!\n" );
	}
#endif
	LogPrint(LOG_LVL_DETAIL,MID_MCU_PRS,"[StartLocalPrs] byEqpId.%d\n",byEqpId);
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
        LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, "Start local bas success!\n");
    }
    else
    {
        LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, "Start local bas failure!\n");
    }

#endif	
	LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"[StartLocalBas] byEqpId.%d\n",byEqpId);
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
	// [7/25/2011 liuxu] 由于这四个接口内在8000a下都是空的,会导致pclint报522的错误,这里消除此错误
	/*lint -save -esym(522, StartLocalMixer, StartLocalVmp, StartLocalBas, StartLocalPrs)*/

#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	//[nizhijun 2011/02/17] 8ke prs外置化
	//StartLocalPrs(PRSID_MIN);
#else	
	u8 byEqpId;
	u32 dwEqpIp;
	u8 byEqpType;

    // 标记是否启动了外设。用于控制会议数量
   // u8 byEqpStart = 0;

	for( byEqpId=1 ;byEqpId <= MAXNUM_MCU_PERIEQP ;byEqpId++ )
	{
		if( SUCCESS_AGENT == g_cMcuAgent.GetPeriInfo( byEqpId, &dwEqpIp, &byEqpType ) )
		{
			if( dwEqpIp == g_cMcuAgent.GetMpcIp() )
			{
				switch( byEqpType ) 
				{
				case EQP_TYPE_MIXER:
					{
						StartLocalMixer( byEqpId );
						//++byEqpStart;
					}
					break;
					
				case EQP_TYPE_VMP:
					{
						StartLocalVmp( byEqpId );
					//	++byEqpStart;
					}
					break;

				case EQP_TYPE_BAS:
					{
						StartLocalBas( byEqpId );
						//++byEqpStart;
					}
					break;
				
				case EQP_TYPE_PRS:
					{
						StartLocalPrs( byEqpId );
						//++byEqpStart;
					}
					break;

				default:
					break;
				}
			}
		}
	}
#endif

#ifdef _VXWORKS
   // if (byEqpStart != 0)
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
    04/03/11    3.0         胡昌威        创建
	10/03/01	4.6			薛亮		  整合8000E	
====================================================================*/
void StartLocalMp( void )
{
	/*
#ifdef _8KE_
	// [3/1/2010 xliang]  FIXME: 多网口
	TMcu8KECfg tMcu8KECfg;
	g_cMcuAgent.GetMcuEqpCfg(&tMcu8KECfg);
	
	
	TNetAdaptInfo * ptNetAdapt = tMcu8KECfg.m_tLinkNetAdap.GetValue(0);	
	if( ptNetAdapt == NULL)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "GetNetAdaptInfo error! Local Mp start failed!\n");
		return;
	}
	TNetParam *ptNetParam = ptNetAdapt->m_tLinkIpAddr.GetValue(0);			//host ip, mask
	
	u16 wMcuPort = g_cMcuVcApp.GetMcuListenPort();
	u32 dwMcuIp = 0;
	TMp tMp;
	
	if( ptNetParam->IsValid() )
	{
		dwMcuIp = ptNetParam->GetIpAddr();
		
		tMp.SetMcuId( LOCAL_MCUID );
		tMp.SetMpId( MCU_BOARD_MPC );
		tMp.SetAttachMode( 1 );
		tMp.SetDoubleLink( 0 );
		tMp.SetIpAddr( dwMcuIp );
	}

#else
  */
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
//#endif

	//交换任务
	if( !mpstart(dwMcuIp, wMcuPort, &tMp ) )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_MPMGR, "MCU: Start mp failure!\n" );
		return;
	}
	return;
}

void StopLocalMp( void )
{
	mpstop();
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
	10/03/01	4.6			薛亮		  整合8000E	
====================================================================*/
void StartLocalMtadp( void )
{
	/*
#ifdef _8KE_
	// [3/1/2010 xliang] FIXME: 多网口
	TMcu8KECfg tMcu8KECfg;
	g_cMcuAgent.GetMcuEqpCfg(&tMcu8KECfg);	
	TNetAdaptInfo * ptNetAdapt = tMcu8KECfg.m_tLinkNetAdap.GetValue(0);		
	// [3/30/2010 xliang] FIXME: 并非一直是0口
	TNetAdaptInfo * ptNetAdapt = tMcu8KECfg.m_tLinkNetAdap.GetValue(0);	
	
	if( ptNetAdapt == NULL)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, "GetNetAdaptInfo error! Local Mtadp start failed!\n");
		printf("GetNetAdaptInfo error! Local Mtadp start failed!\n");
		return;
	}
	TNetParam *ptNetParam = ptNetAdapt->m_tLinkIpAddr.GetValue(0);			//host ip, mask
	
	u16 wMcuPort = g_cMcuVcApp.GetMcuListenPort();
	u32 dwMcuIp = 0;
	TMtAdp tMtadp;
	
	if( ptNetParam->IsValid() )
	{
		dwMcuIp = ptNetParam->GetIpAddr();
		
		tMtadp.SetMtadpId( MCU_BOARD_MPC );
		tMtadp.SetAttachMode( 1 );
		tMtadp.SetIpAddr( dwMcuIp );
	}
	
	
	TMtAdpConnectParam tConnectParam;
	
	tConnectParam.m_dwMcuTcpNode = 0;
	tConnectParam.m_wMcuPort = wMcuPort;
	tConnectParam.SetMcuIpAddr( dwMcuIp );
	
	tConnectParam.m_bSupportHD = TRUE;//内置mtadp支持高清

#else
*/

	//u8  byMpcId  = (g_cMcuAgent.GetMpcBoardId()%16 == 0) ? 16 : (g_cMcuAgent.GetMpcBoardId()%16);
	u8  byMpcId  = g_cMcuAgent.GetMpcBoardId();
	u32 dwMcuIp  = g_cMcuAgent.GetMpcIp();
	u16 wMcuPort = g_cMcuVcApp.GetMcuListenPort();

	if( 0 == dwMcuIp || inet_addr("127.0.0.1") == dwMcuIp ) 
	{
		LogPrint( LOG_LVL_ERROR, MID_MCULIB_MTADP, "LocalIP.%x is invalid reset localip start\n", dwMcuIp );

		u32 tdwIPList[5];  //列举到的当前设置的所有有效ip
		u16 dwIPNum;       //列举到ip 数目

		dwIPNum = OspAddrListGet(tdwIPList, 5);
		if( 0 == dwIPNum )
		{
			LogPrint( LOG_LVL_ERROR, MID_MCULIB_MTADP, "LocalIP.%x is invalid reset localip err\n", dwMcuIp );
		}
		for(u16 dwAdapter=0; dwAdapter<dwIPNum; dwAdapter++)
		{
			if((inet_addr( "127.0.0.1" ) != tdwIPList[dwAdapter]))
			{
				LogPrint( LOG_LVL_ERROR, MID_MCULIB_MTADP, "LocalIP.%x is invalid reset localip.%x ok\n", dwMcuIp, tdwIPList[dwAdapter] );

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

//#endif	//!8KE 

	//终端适配模块（内嵌方式启动该模块）
	if( !MtAdpStart(&tConnectParam, &tMtadp) )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCULIB_MTADP, "MCU: Start mtadp failure!\n" );
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
	case VMP_STYLE_TEN_M:
		byMaxMemNum = 10;
		break;
	case VMP_STYLE_THIRTEEN:
	case VMP_STYLE_THIRTEEN_M:
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
	case VMP_STYLE_FIFTEEN:
		 byMaxMemNum = 15;
		 break;
	case VMP_STYLE_TWENTYFIVE:
		byMaxMemNum = 25;
		break;
	default:
		break;
	}

	return byMaxMemNum;
}

/*====================================================================
    函数名      : IsValidVmpId
    功能        : 判断是否是有效vmpid
    算法实现    : 
    引用全局变量: 无
    输入参数说明: byVmpId 
    返回值说明  : BOOL32
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    13/04/12    4.7       yanghuaizhi       创建
====================================================================*/
BOOL32 IsValidVmpId(u8 byVmpId)
{
	if (byVmpId >= VMPID_MIN &&
		byVmpId <= VMPID_MAX)
	{
		return TRUE;
	}
	return FALSE;
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
				if( ptMtTempAlias->m_AliasType == puAliasTypeIPPlusAlias )
				{
					*pszTemp++ = sizeof(u32) + strlen( &ptMtTempAlias->m_achAlias[sizeof(u32)] );
					memcpy( pszTemp, ptMtTempAlias->m_achAlias, sizeof(u32) + strlen( &ptMtTempAlias->m_achAlias[sizeof(u32)] ) );
					pszTemp = pszTemp + sizeof(u32) + strlen( &ptMtTempAlias->m_achAlias[sizeof(u32)] );
				}
				else
				{
					*pszTemp++ = strlen( ptMtTempAlias->m_achAlias );
					memcpy( pszTemp, ptMtTempAlias->m_achAlias, strlen( ptMtTempAlias->m_achAlias ) );
					pszTemp = pszTemp + strlen( ptMtTempAlias->m_achAlias );
				}
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
 
	while( (pszTemp - pszBuf < wBufLen) &&
		    // fxh超过MAXNUM_CONF_MT的终端信息忽略，避免越界
		    byMtCount < MAXNUM_CONF_MT )
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
	case MEDIA_TYPE_G719:
		wBitrate = AUDIO_BITRATE_G719;
		break;
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
函 数 名： GetMaxAudioBiterate
功    能： 
算法实现： 
全局变量： 
参    数：const TConfInfoEx &tConInfoEx 
返 回 值：u16
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2013/04/09  4.7			倪志俊                 创建
=============================================================================*/
u16 GetMaxAudioBiterate(const TConfInfoEx &tConInfoEx)
{
	TAudioTypeDesc tAudDesc[MAXNUM_CONF_AUDIOTYPE];
	u8 byAudNum = tConInfoEx.GetAudioTypeDesc(tAudDesc);
	u16 wMaxAudBiterate = 0;

	for ( u8 byIdx =0 ; byIdx<byAudNum; byIdx++ )
	{
		if ( GetAudioBitrate( tAudDesc[byIdx].GetAudioMediaType()) > wMaxAudBiterate )
		{
			wMaxAudBiterate = GetAudioBitrate( tAudDesc[byIdx].GetAudioMediaType());
		}
	}
	
	return wMaxAudBiterate;
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
	2010/12/31  4.6			 刘旭                  修改
=============================================================================*/
TConfMcInfo* TConfOtherMcTable::AddMcInfo(const u16 wMcuIdx)
{
	// 192保留给local mcuid
	if (!IsValidSubMcuId(wMcuIdx))
	{
		return NULL;
	}

	if( m_atConfOtherMcInfo[wMcuIdx].GetMcuIdx() == wMcuIdx )
	{
		return &(m_atConfOtherMcInfo[wMcuIdx]);
	}

	m_atConfOtherMcInfo[wMcuIdx].SetNull();
	m_atConfOtherMcInfo[wMcuIdx].SetMcuIdx( wMcuIdx );//.m_wMcuIdx = wMcuIdx;
	m_atConfOtherMcInfo[wMcuIdx].m_tSpyMt.SetNull();
	m_atConfOtherMcInfo[wMcuIdx].m_dwSpyViewId = OspTickGet();
	m_atConfOtherMcInfo[wMcuIdx].m_dwSpyVideoId = OspTickGet()+10;
	
    ///FIXME: guzh [7/4/2007] 权宜之计：如果选择了不请求级联列表，会导致没有该下级MCU的状态
    // 这里就先把该MCU本身的状态设置好，以使得发言、选看、查媒体源等级联操作能够得以进行 
    TMt tMt;
    tMt.SetMcuIdx(wMcuIdx);
    tMt.SetMtId(0);
    m_atConfOtherMcInfo[wMcuIdx].m_atMtExt[0].SetMt(tMt);
    m_atConfOtherMcInfo[wMcuIdx].m_atMtStatus[0].SetMt(tMt);
    
	return &(m_atConfOtherMcInfo[wMcuIdx]);
}


/*=============================================================================
    函 数 名： SetMcInfo
    功    能： 根据McuIdx设置它的TConfMcInfo
    算法实现： 
    全局变量： 
    参    数： wMcuIdx, Id号; 
	           tMcInfo, TConfMcInfo
    返 回 值： BOOL32
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2010/12/31  4.6			liuxu                   创建
=============================================================================*/
BOOL32 TConfOtherMcTable::SetMcInfo(const u16 wMcuIdx, const TConfMcInfo& tMcInfo)
{
	// 非法的子mcu id
	if (!IsValidSubMcuId(wMcuIdx))
	{
		return FALSE;
	}

	// 无论何时都强制填充
	memcpy(&m_atConfOtherMcInfo[wMcuIdx], &tMcInfo, sizeof(TConfMcInfo));

	return TRUE;
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
    2010/12/31  4.6			liuxu                   修改
=============================================================================*/
BOOL32 TConfOtherMcTable::RemoveMcInfo( const u16 wMcuIdx )
{
	if (!IsValidSubMcuId(wMcuIdx))
	{
		return FALSE;
	}

	if(m_atConfOtherMcInfo[wMcuIdx].GetMcuIdx() == wMcuIdx)
	{
		memset( &m_atConfOtherMcInfo[wMcuIdx], 0, sizeof(TConfMcInfo) );
		m_atConfOtherMcInfo[wMcuIdx].SetNull();
		return TRUE;
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
	2010/12/31  4.6			 刘旭                   创建
=============================================================================*/
TConfMcInfo* TConfOtherMcTable::GetMcInfo(const u16 wMcuIdx)
{
	if (!IsValidSubMcuId(wMcuIdx))
	{
		// 只打印非192的非法idx
		if (!IsLocalMcuId(wMcuIdx))
		{
			LogPrint( LOG_LVL_WARNING, MID_MCU_MMCU, "[TConfOtherMcTable::GetMcInfo] wMcuIdx:%d is not a invalid sub mcu id!\n", wMcuIdx);
		}

		return NULL;
	}

	if(m_atConfOtherMcInfo[wMcuIdx].GetMcuIdx() == wMcuIdx)
	{
		return &(m_atConfOtherMcInfo[wMcuIdx]);
	}
	else
	{
		return NULL;
	}
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
BOOL32  TConfOtherMcTable::SetMtInMixing(const TMt& tMt)
{
    TConfMcInfo *ptMcInfo = GetMcInfo(tMt.GetMcuIdx());
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
BOOL32  TConfOtherMcTable::IsMtInMixing( const TMt& tMt)  
{       
    TConfMcInfo *ptMcInfo = GetMcInfo(tMt.GetMcuIdx());
    if (NULL != ptMcInfo)
    {
        const TMcMtStatus *ptStatus = ptMcInfo->GetMtStatus(tMt);
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
BOOL32   TConfOtherMcTable::ClearMtInMixing(const TMcu& tMcu)
{
    TMt tMt;
    tMt.SetMcuIdx(tMcu.GetMcuIdx());
    TConfMcInfo *ptMcInfo = GetMcInfo(tMcu.GetMcuIdx());
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

/*=============================================================================
函 数 名： GetIsMcuSupMultSpy
功    能： 判断某个下级mcu是否支持多回传
算法实现： 
全局变量： 
参    数： u8 byMcuId 
返 回 值： void  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
20100106  4.0			pengjie                create
=============================================================================*/
BOOL32  TConfOtherMcTable::GetIsMcuSupMultSpy( const u16 wMcuIdx)
{
	if( IsValidSubMcuId(wMcuIdx) )
	{
		if( wMcuIdx == m_atConfOtherMcInfo[wMcuIdx].GetMcuIdx() )
		{
			return m_atConfOtherMcInfo[wMcuIdx].m_byIsSupMultSpy;
		}
	}

	return FALSE;	
}

BOOL32  TConfOtherMcTable::GetMultiSpyBW( const u16 wMcuIdx, u32 &dwMaxSpyBW, s32 &nRemainSpyBW)
{
	if( IsValidSubMcuId(wMcuIdx) )
	{
		if( wMcuIdx == m_atConfOtherMcInfo[wMcuIdx].GetMcuIdx() )
		{
			if (m_atConfOtherMcInfo[wMcuIdx].m_byIsSupMultSpy == 1)
			{
				dwMaxSpyBW = m_atConfOtherMcInfo[wMcuIdx].m_dwMaxSpyBW;
				nRemainSpyBW = m_atConfOtherMcInfo[wMcuIdx].m_nRemainSpyBW;
				return TRUE;
			}
			else
			{
				return FALSE;
			}
				
		}
	}

	return FALSE;	
}

BOOL32  TConfOtherMcTable::SetMcuSupMultSpyMaxBW( const u16 wMcuIdx, const u32 dwMaxSpyBW)
{
	if( IsValidSubMcuId(wMcuIdx) )
	{
		if( wMcuIdx == m_atConfOtherMcInfo[wMcuIdx].GetMcuIdx() )
		{
			if (m_atConfOtherMcInfo[wMcuIdx].m_byIsSupMultSpy == 1)
			{
				m_atConfOtherMcInfo[wMcuIdx].m_dwMaxSpyBW = dwMaxSpyBW;
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}

	return FALSE;
}

BOOL32  TConfOtherMcTable::SetMcuSupMultSpyRemainBW( const u16 wMcuIdx, const s32 nRemainSpyBW)
{
	if( IsValidSubMcuId(wMcuIdx) )
	{
		if( wMcuIdx == m_atConfOtherMcInfo[wMcuIdx].GetMcuIdx() )
		{
			if (m_atConfOtherMcInfo[wMcuIdx].m_byIsSupMultSpy == 1)
			{
				m_atConfOtherMcInfo[wMcuIdx].m_nRemainSpyBW = nRemainSpyBW;
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}

	return FALSE;
}

/*=============================================================================
函 数 名： SetMcuSupMultSpy
功    能： 标志某个下级的mcu支持多回传
算法实现： 
全局变量： 
参    数： u8 byMcuId 
返 回 值： void  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
20100106  4.0			pengjie                create
=============================================================================*/
BOOL32  TConfOtherMcTable::SetMcuSupMultSpy( const u16 wMcuIdx )
{
	if( IsValidSubMcuId(wMcuIdx) )
	{
		if( wMcuIdx == m_atConfOtherMcInfo[wMcuIdx].GetMcuIdx() )
		{
			m_atConfOtherMcInfo[wMcuIdx].m_byIsSupMultSpy = TRUE;
			return TRUE;
		}
	}

	return FALSE;	
}

/*=============================================================================
函 数 名： IsMcuSupMultCas
功    能： 判断某个下级mcu是否支持多级联
算法实现： 
全局变量： 
参    数： u16 wMcuIdx 
返 回 值： BOOL32  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010/07/09  4.6			xueliang                create
=============================================================================*/
BOOL32 TConfOtherMcTable::IsMcuSupMultCas( const u16 wMcuIdx )
{
	if( IsValidSubMcuId(wMcuIdx) )
	{
		if( wMcuIdx == m_atConfOtherMcInfo[wMcuIdx].GetMcuIdx() )
		{
			return  ( m_atConfOtherMcInfo[wMcuIdx].m_byIsSupMultCas == 1);
		}
	}

	return FALSE;
}

/*=============================================================================
函 数 名： SetMcuSupMultCas
功    能： 标志某个下级的mcu支持多级联
算法实现： 
全局变量： 
参    数： u16 wMcuIdx 
返 回 值： BOOL32  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010/07/09  4.6			xueliang                create
=============================================================================*/
BOOL32  TConfOtherMcTable::SetMcuSupMultCas( const u16 wMcuIdx )
{
	if( IsValidSubMcuId(wMcuIdx) )
	{
		if( wMcuIdx == m_atConfOtherMcInfo[wMcuIdx].GetMcuIdx() )
		{
			m_atConfOtherMcInfo[wMcuIdx].m_byIsSupMultCas = 1;
			return TRUE;
		}
		
		return FALSE;
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
	if (!lpszTable)
	{
		memset( tMAPParam, 0, byMapCount*sizeof(TMAPParam) );
		return FALSE;
	}

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
		INNER_FREE_MEM(lpszTable, dwMemEntryNum);
		return FALSE;
    }
	if( dwMemEntryNum != byMapCount )
	{
		memset( tMAPParam, 0, byMapCount*sizeof(TMAPParam) );
		INNER_FREE_MEM(lpszTable, dwMemEntryNum);
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
	INNER_FREE_MEM(lpszTable, dwMemEntryNum);
    
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
	case MEDIA_TYPE_G719:

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
        LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetActivePayload] unexpected media type.%d, ignore it\n", byRealPayloadType );
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
	    case MEDIA_TYPE_PCMA:   byAPayload = bNewActivePT ? ACTIVE_TYPE_PCMA : byRealPayloadType;  break;
	    case MEDIA_TYPE_PCMU:   byAPayload = bNewActivePT ? ACTIVE_TYPE_PCMU : byRealPayloadType;  break;
	    case MEDIA_TYPE_G721:   byAPayload = ACTIVE_TYPE_G721;  break;
	    case MEDIA_TYPE_G722:   byAPayload = bNewActivePT ? ACTIVE_TYPE_G722 : byRealPayloadType;  break;
	    case MEDIA_TYPE_G7231:  byAPayload = ACTIVE_TYPE_G7231; break;
	    case MEDIA_TYPE_G728:   byAPayload = bNewActivePT ? ACTIVE_TYPE_G728 : byRealPayloadType;  break;
	    case MEDIA_TYPE_G729:   byAPayload = bNewActivePT ? ACTIVE_TYPE_G729 : byRealPayloadType;  break;
	    case MEDIA_TYPE_G719:   byAPayload = bNewActivePT ? ACTIVE_TYPE_G719 : byRealPayloadType;  break;
		// Bug00121515 h261格式编码还是采用原来的载荷
		//zjj20131120 再次修改加密使用ACTIVE_TYPE_H261
		case MEDIA_TYPE_H261:   byAPayload = bNewActivePT ? ACTIVE_TYPE_H261 : byRealPayloadType;  break;	//
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

/*=============================================================================
函 数 名： GetActivePL
功    能： 适配管理类 获取动态载荷封装
算法实现： 
全局变量： 
参    数：
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
08/11/18    4.6         周嘉麟        创建
=============================================================================*/
u8 GetActivePL(const TConfInfo &tConfInfo, u8 byMediaType)
{
	// [7/25/2011 liuxu] 无用,删除
    // u8 byConfIdx = g_cMcuVcApp.GetConfIdx(tConfInfo.GetConfId());

    return GetActivePayload(tConfInfo, byMediaType);
}

// 判断源端分辨率是否大于目的端分辨率,大于则返回TRUE,小于等于则返回FALSE
BOOL32 IsResG(u8 bySrcRes, u8 byDstRes)
{
    if (VIDEO_FORMAT_AUTO == bySrcRes ||
        VIDEO_FORMAT_AUTO == byDstRes)
    {
        return FALSE;
    }

	//20110503 zjl 双流不区分动静态
//     if (!IsDSResMatched(bySrcRes, byDstRes))
//     {
//         return FALSE;
//     }

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
							VIDEO_FORMAT_UXGA,
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
        LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[IsResGE] unexpected res <src.%d, dst.%d>\n", bySrcRes, byDstRes);
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
	//Auto不解析
	case VIDEO_FORMAT_AUTO:
		bLive = TRUE;
		break;
	//DS不考虑SQCIF
	case VIDEO_FORMAT_SQCIF_112x96:
	case VIDEO_FORMAT_SQCIF_96x80:
		break;
	default:
		break;
	}
	return bLive;
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
    2011/01/21  4.6			 liuxu                  添加lock
=============================================================================*/
SEMHANDLE	g_hStrOfIP = NULL;						// StrOfIP专用

char * StrOfIP( u32 dwIP )
{
	if (NULL == g_hStrOfIP)
	{
		if( !OspSemBCreate(&g_hStrOfIP))
		{
			OspSemDelete( g_hStrOfIP );
			g_hStrOfIP = NULL;
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[StrOfIP] create g_hStrOfIP failed!\n" );
		}
	}

	if (NULL != g_hStrOfIP)
	{
		OspSemTake(g_hStrOfIP);
	}	

    dwIP = htonl(dwIP);
	static char strIP[17];  
	u8 *p = (u8 *)&dwIP;
	sprintf(strIP,"%d.%d.%d.%d%c",p[0],p[1],p[2],p[3],0);

	if (NULL != g_hStrOfIP)
	{
		OspSemGive(g_hStrOfIP);
	}

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
#elif defined(_8KE_) 
	return MCU_TYPE_KDV8000E;	// [3/1/2010 xliang] 增加8000E型号
#elif defined(_8KH_)	
	#ifdef _800L_
		return MCU_TYPE_KDV800L;
	#elif defined(_8KH_M_)
		return MCU_TYPE_KDV8000H_M;
	#else
		return MCU_TYPE_KDV8000H;
	#endif

#elif defined(_8KI_)
	return MCU_TYPE_KDV8000I;
#else
//     #ifdef WIN32
//         return MCU_TYPE_WIN32;
//     #else
//         return MCU_TYPE_KDV8000;
//     #endif
	return MCU_TYPE_KDV8000;
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
	// [pengjie 2010/6/3] 添加离线原因
    static s8* gs_szLeftReason[] = {"Unknown", "Exception", "Normal", "RTD", "DRQ", "TypeUnmatch", "Busy",
                                    "Reject", "Unreachable", "Local", "BusyExt", "remotereconnect", "confholding",
	                                "hascascaded"};

    if ( byReason >= MTLEFT_REASON_EXCEPT && byReason <= MTLEFT_REASON_REMOTEHASCASCADED )
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
	logenablemod(MID_MCU_DCS);
	//g_bPrtDcsMsg = TRUE;
}

API void npdcsmsg( void )
{
	logdisablemod(MID_MCU_DCS);
	//g_bPrtDcsMsg = FALSE;
}

//mcs调试打印接口
BOOL32 g_bPrintMcsMsg = FALSE;
API void pmcsmsg( void )
{
	logenablemod(MID_MCU_MCS);
    g_bPrintMcsMsg = TRUE;
}

API void npmcsmsg( void )
{
	logdisablemod(MID_MCU_MCS);
    g_bPrintMcsMsg = FALSE;
}

//vcs调试打印接口
BOOL32 g_bPrintVcsMsg = FALSE;
API void pvcsmsg( void )
{
	logenablemod(MID_MCU_MCS);
	logenablemod(MID_MCU_VCS);

    g_bPrintVcsMsg = TRUE;
	//同时把mcs的消息打应打开
    g_bPrintMcsMsg = TRUE;
}

API void npvcsmsg( void )
{
	logdisablemod(MID_MCU_MCS);
	logdisablemod(MID_MCU_VCS);
    g_bPrintVcsMsg = FALSE;
	//同时把mcs的消息打应打开
    g_bPrintMcsMsg = FALSE;
}

//eqp调试打印接口
BOOL32 g_bPrintEqpMsg = FALSE;
API void peqpmsg( void )
{
	logenablemod(MID_MCU_EQP);
	logenablemod(MID_MCU_REC);
	logenablemod(MID_MCU_HDU);
	
	logenablemod(MID_MCU_VMP);
	logenablemod(MID_MCU_BAS);
	logenablemod(MID_MCU_PRS);

	logenablemod(MID_MCU_MIXER);
	
    g_bPrintEqpMsg = TRUE;
}

API void npeqpmsg( void )
{
	logdisablemod(MID_MCU_EQP);
	logdisablemod(MID_MCU_REC);
	logdisablemod(MID_MCU_HDU);

	logdisablemod(MID_MCU_VMP);
	logdisablemod(MID_MCU_BAS);
	logdisablemod(MID_MCU_PRS);
	
	logdisablemod(MID_MCU_MIXER);

    g_bPrintEqpMsg = FALSE;
}

//mmcu调试接口
BOOL32 g_bpMMcuMsg = FALSE;
API void pmmcumsg(void)
{
	logenablemod(MID_MCU_MMCU);
	logenablemod(MID_MCU_SPY);
    g_bpMMcuMsg = TRUE;
}
API void npmmcumsg(void)
{
	logdisablemod(MID_MCU_MMCU);
	logdisablemod(MID_MCU_SPY);
    g_bpMMcuMsg = FALSE;
}

//mt
BOOL32 g_bPMt2Msg = FALSE;
API void pmt2msg( void )
{
	logenablemod(MID_MCU_MT2);
    g_bPMt2Msg = TRUE;
}

API void npmt2msg( void )
{
	logdisablemod(MID_MCU_MT2);
    g_bPMt2Msg = FALSE;
}

//mt call
BOOL32 g_bPrintCallMsg = FALSE;
API void pcallmsg( void )
{
	logenablemod(MID_MCU_CALL);
    g_bPrintCallMsg = TRUE;
}

API void npcallmsg( void )
{
	logdisablemod(MID_MCU_CALL);
    g_bPrintCallMsg = FALSE;
}

//mp
BOOL32 g_bpMpMgrMsg = FALSE;
API void pmpmgrmsg(void)
{
	logenablemod(MID_MCU_MPMGR);
    g_bpMpMgrMsg = TRUE;
}
API void npmpmgrmsg(void)
{
	logdisablemod(MID_MCU_MPMGR);
    g_bpMpMgrMsg = FALSE;
}

//cfg
BOOL32 g_bPrintCfgMsg = FALSE;
API void pcfgmsg( void )
{
	logenablemod(MID_MCU_CFG);
    g_bPrintCfgMsg = TRUE;
}

API void npcfgmsg( void )
{
	logdisablemod(MID_MCU_CFG);
    g_bPrintCfgMsg = FALSE;
}

//mt
BOOL32 g_bPrintMtMsg = FALSE;
API void pmtmsg( void )
{
	logenablemod(MID_MCU_MT);
    g_bPrintMtMsg = TRUE;
}

API void npmtmsg( void )
{
	logdisablemod(MID_MCU_MT);
    g_bPrintMtMsg = FALSE;
}

//guard
BOOL32 g_bPrintGdMsg = FALSE;
API void pgdmsg( void )
{
	logenablemod(MID_MCU_GUARD);
    g_bPrintGdMsg = TRUE;
}

API void npgdmsg( void )
{
	logdisablemod(MID_MCU_GUARD);
    g_bPrintGdMsg = FALSE;
}

BOOL32 g_bPrintNPlusMsg = FALSE;

API void pnplusmsg( void )
{
	logenablemod(MID_MCU_NPLUS);
    g_bPrintNPlusMsg = TRUE;
}

API void npnplusmsg( void )
{
	logdisablemod(MID_MCU_NPLUS);
    g_bPrintNPlusMsg = FALSE;
}

API void ppfmmsg( void )
{
	logenablemod(MID_MCU_PFM);
    g_bPrintPfmMsg = TRUE;
}

API void nppfmmsg( void )
{
	logdisablemod(MID_MCU_PFM);
    g_bPrintPfmMsg = FALSE;
}

API void sconftotemp( s8* psConfName )
{
	if ( psConfName == NULL )
	{
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "ERROR: The name is null!\n" );
		return;
	}

	CConfId cConfId = g_cMcuVcApp.GetConfIdByName( (LPCSTR)psConfName, FALSE,TRUE );
	if( cConfId.IsNull() )
	{
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "ERROR: The conference %s is not exist! The name maybe error!\n", psConfName );
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
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "Reset login info failed!\n" );
	}
	return;
}

//多回传调试打印
BOOL32 g_bPrintCascadeSpyMsg = 0;

API void pspymsg( void )
{
	logenablemod(MID_MCU_SPY);
	g_bPrintCascadeSpyMsg = LOG_LVL_DETAIL+1;
}

API void npspymsg( void )
{
	logdisablemod(MID_MCU_SPY);
	g_bPrintCascadeSpyMsg = 0;
}

u8 FrameRateMac2Real(u8 byFrameRate)
{
	switch (byFrameRate)
	{
	case VIDEO_FPS_2997_1:	return 30;
	case VIDEO_FPS_25:		return 25;
	case VIDEO_FPS_2997_2:	return 15;
	case VIDEO_FPS_2997_3:	return 10;
	case VIDEO_FPS_2997_4:	return 8;
	case VIDEO_FPS_2997_5:	return 6;
	case VIDEO_FPS_2997_6:	return 5;
	case VIDEO_FPS_2997_30: return 1;
	case VIDEO_FPS_2997_7P5:return 4;
	case VIDEO_FPS_2997_10:	return 3;
	case VIDEO_FPS_2997_15:	return 2;

	default:				return 1;
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

	if ( wWidth == 1440 && wHeight == 816 )
	{
		return VIDEO_FORMAT_1440x816;
	}
	
	if ( wWidth == 960 && wHeight == 544 )
	{
		return VIDEO_FORMAT_960x544;
	}
	
	if ( wWidth == 640 && wHeight == 368 )
	{
		return VIDEO_FORMAT_640x368;
	}
	
	if ( wWidth == 480 && wHeight == 272 )
	{
		return VIDEO_FORMAT_480x272;
	}
	
	if ( wWidth == 384 && wHeight == 272 )
	{
		return VIDEO_FORMAT_384x272;
	}
	
	//非标分辨率（720p底图）
	if ( wWidth == 864 && wHeight == 480 )
	{
		return VIDEO_FORMAT_720_864x480;
	}
	
	if( wWidth == 432 && wHeight == 240 )
	{
		return VIDEO_FORMAT_720_432x240;
	}
	
	if( wWidth == 320 && wHeight == 192 )
	{
		return VIDEO_FORMAT_720_320x192;
	}

	if( wWidth == 640 && wHeight == 544 )
	{
		return VIDEO_FORMAT_640x544;
	}

	if( wWidth == 320 && wHeight == 272 )
	{
		return VIDEO_FORMAT_320x272;
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
		
	case VIDEO_FORMAT_384x272:
		wWidth = 384;
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
	case VIDEO_FORMAT_640x544:
		wWidth = 640;
		wHeight =544;
		break;
	case VIDEO_FORMAT_320x272:
		wWidth = 320;
		wHeight =272;
		break;
    default:
        break;
    }
    if ( 0 == wHeight || 0 == wWidth )
    {
        LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetWHByRes] unexpected res.%d, ignore it\n", byRes );
    }
    return;
}

/*====================================================================
函数名           GetFormatByBitrate
功能        ：根据码率得到分辨率
算法实现    ：
引用全局变量：
输入参数说明：u8 byRes原分辨率
			  u16 wBitrate 码率
返回值说明  ：u8 对应分辨率
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2011/06/30    4.0         zhangli        创建
====================================================================*/
u8 GetAutoResByBitrate(u8 byRes, u16 wBitrate)
{
	if (byRes != VIDEO_FORMAT_AUTO)
	{
		return VIDEO_FORMAT_INVALID;
	}
	if (wBitrate > 3072)
	{
		return VIDEO_FORMAT_4CIF;
	}
	else if (wBitrate > 1536)
	{
		return VIDEO_FORMAT_2CIF;
	}
	else
	{
		return VIDEO_FORMAT_CIF;
	}
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
函数名    :  WHCmp
功能      :  某高宽分别同另一高宽比较
算法实现  :  
参数说明  :  
返回值说明:  s32 
             宽高均小返回 F_WHNONELARGER
			 宽小高大返回 F_HLARGER
			 宽高均相等返回 WHEQUAL
			 宽大高小返回 F_WLARGER
			 宽高均大返回 F_WHLARGER
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-10-20                  薛亮                            创建
==============================================================================*/
s32 WHCmp(u16 wSrcWidth, u16 wSrcHeight, u16 wDstWidth, u16 wDstHeight)
{
	//特殊调整
	//VIDEO_FORMAT_480x272 ——>比较时统一扩充到480 x 288
	wSrcHeight = (wSrcHeight == 272) ? 288 : wSrcHeight;
	wDstHeight = (wDstHeight == 272) ? 288 : wDstHeight;
	
	//前者均不大于后者
	if ( (wSrcWidth == wDstWidth) && (wSrcHeight == wDstHeight) )
	{
		return WHEQUAL;  //宽高均等  (equal in width and height)
	}
	else if( ( wSrcWidth <= wDstWidth ) && (wSrcHeight <= wDstHeight) )
	{
		return F_WHNONELARGER; //宽高均小，宽等高小，宽小高等 (none bigger in both width and height)
	}
	//前者至少有一项大于后者
	else if ( (wSrcWidth <= wDstWidth) && (wSrcHeight >= wDstHeight) )
	{
		return F_HLARGER; //宽小高大，宽等高大  (only bigger in height)
	}
	else if ( (wSrcWidth >= wDstWidth) && (wSrcHeight <= wDstHeight) )
	{
		return F_WLARGER;  //宽大高小，宽大高等  (only bigger in width)
	}
	else 
	{
		return F_WHLARGER; //宽高均大			 (bigger both in width and height)
	}
}
/*==============================================================================
函数名    :  ResWHCmp
功能      :  
算法实现  :  
参数说明  :  u8 bySrcRes	[in]
			 u8 byDstRes    [in]
返回值说明: s32 
			宽高均小返回 F_WHNONELARGER
			宽小高大返回 F_HLARGER
			宽高均相等返回 WHEQUAL
			宽大高小返回 F_WLARGER
			宽高均大返回 F_WHLARGER
			两者比较无意义返回 F_NOMATCH
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-2-20                  薛亮                            创建
==============================================================================*/
s32 ResWHCmp(u8 bySrcRes, u8 byDstRes)
{
	// vmp支持双流跟随,双流分辨率同样需要比较
	/*VGA类的分辨率和非VGA类的分辨率比较没有意义
	if(IsResCmpNoMeaning(bySrcRes,byDstRes))
	{
// 		return 3;
		return F_NOMATCH;
	}*/
	
	u16 wSrcWidth = 0;
	u16 wSrcHeight = 0;
	u16 wDstWidth = 0;
	u16 wDstHeight = 0;
	GetWHByRes(bySrcRes, wSrcWidth, wSrcHeight);
	GetWHByRes(byDstRes, wDstWidth, wDstHeight);
	
	return WHCmp(wSrcWidth, wSrcHeight, wDstWidth, wDstHeight);
}

/*==============================================================================
函数名    :  IsSrcResThanDst
功能      :  
算法实现  :  
参数说明  :  u8 bySrcRes	[in]
			 u8 byDstRes    [in]
返回值说明: BOOL32 TRUE:源大于目的，FALSE：源小于目的
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
20130419                  chenbing                            创建
==============================================================================*/
BOOL32 IsSrcResThanDst(u8 bySrcRes, u8 byDstRes)
{
	if (  -1 == ResWHCmp(bySrcRes, byDstRes)
		|| 1 == ResWHCmp(bySrcRes, byDstRes)
		|| 2 == ResWHCmp(bySrcRes, byDstRes)
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
函数名    :  GetSuitableRes
功能      :  
算法实现  :  
参数说明  :  u8 byLmtRes	[i]	需要的分辨率上限
u8 byOrgRes	[i] 原始分辨率
返回值说明:  u8					合适的分辨率
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-10-20                   薛亮
==============================================================================*/
u8 GetSuitableRes(u8 byLmtRes, u8 byOrgRes)
{
	//1.
	u16 wLmtWidth = 0;
	u16 wLmtHeight = 0;
	u16 wOrgWidth = 0;
	u16 wOrgHeight = 0;
	GetWHByRes(byLmtRes, wLmtWidth, wLmtHeight);
	GetWHByRes(byOrgRes, wOrgWidth, wOrgHeight);
	
	//4.
	return GetSuitableResByWH(wLmtWidth, wLmtHeight, wOrgWidth, wOrgHeight);	
}

/*==============================================================================
函数名    :  GetSuitableResByWH
功能      :  
算法实现  :  
参数说明  :  u8 byLmtRes	[i]	需要的分辨率上限
u8 byOrgRes	[i] 原始分辨率
返回值说明:  u8					合适的分辨率
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2012-09-13                 yanghuaizhi
==============================================================================*/
u8 GetSuitableResByWH(u16 wLmtWidth, u16 wLmtHeight, u16 wOrgWidth, u16 wOrgHeight)
{
	u16 wMinWidth  = min(wLmtWidth, wOrgWidth);
	u16 wMinHeight = min(wLmtHeight,wOrgHeight);
	
	return GetMcuSupportedRes(wMinWidth, wMinHeight);
}

/*==============================================================================
函数名    :  GetDecAbility
功能      :  根据同时能力集获取与之适应的解码能力
算法实现  :  
参数说明  :  [in]TSimCapSet, 终端或外设的同时能力集
			 
返回值说明:  TBasChnData::DecAbility
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010/11/12                 liuxu                            创建
2011/02/09   4.7		   倪志俊							修改
==============================================================================*/
const s16 GetDecAbility(const TSimCapSet& tSimCapSet)
{
	if ( tSimCapSet.GetVideoProfileType() == emHpAttrb )
	{
		return CBasChn::emDecHp;
	}
	else if(tSimCapSet.GetUserDefFrameRate() >= 50)
	{
		return CBasChn::emDecHighFps;
	}
	else if (MEDIA_TYPE_H264 == tSimCapSet.GetVideoMediaType())
	{
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
		if(IsResG(tSimCapSet.GetVideoResolution(), VIDEO_FORMAT_4CIF))
#else
		if(IsResGE(tSimCapSet.GetVideoResolution(), VIDEO_FORMAT_4CIF))
#endif
		{
			return CBasChn::emDecHD;
		}
		else
		{
			return CBasChn::emDecSD;
		}		
	}
	else
	{
		return CBasChn::emDecSD;
	}
}

/*==============================================================================
函数名    :  GetSwitchSndBindIp
功能      :  取组播的发送地址，用于设给DataSwitch
注意	  :  只在8000H-M下有效
算法实现  :  根据route -n 里的UG取默认路由的IP地址
参数说明  :  无
返回值说明:  0: 不影响以前结果
		   非0: 当前的发包 网卡的IP地址
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2013/06/5                  彭国锋                            创建
==============================================================================*/
u32 GetSwitchSndBindIp()
{
	//1. 非8000H-M，一律返回0，和原来一样
#ifdef _8KH_
	if ( !g_cMcuAgent.Is8000HmMcu() )
	{
		LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS, "[sorry it it not 8000H-M]\n");
		return 0;
	}

	// 只显示第一个默认路由[pengguofeng 6/5/2013]
#ifndef _LINUX_
#define __func__    "GetSwitchSndBindIp"
#define popen		_popen
#define pclose		_pclose
#endif
	s8 *pCmd = "ifconfig `route -n | grep UG | awk '{print $8}' | sed -n 1p` | grep \"inet addr\" | cut -d':' -f2 | cut -d' ' -f1";
	LogPrint(LOG_LVL_DETAIL, MID_PUB_ALWAYS, "[%s] run cmd is [%s]\n", __func__, pCmd);
	FILE *fp = popen(pCmd, "r");
	if ( fp)
	{
		s8 achIp[16] = {0}; //IP的长度
		while(fgets(achIp, 16, fp) )
		{
			u8 byLen = strlen(achIp);
			if ( byLen == 0 )
			{
				LogPrint(LOG_LVL_KEYSTATUS, MID_PUB_ALWAYS, "[%s]get ip content is Null\n", __func__);
				continue;
			}
			if ( achIp[byLen-1] == '\n')
			{
				achIp[byLen-1] = 0;
			}
			LogPrint(LOG_LVL_DETAIL, MID_PUB_ALWAYS, "[%s] get fist UG ip: [%s]\n", __func__, achIp);
			break;
		}
		pclose(fp);
		return ntohl(INET_ADDR(achIp));
	}
	else
	{
		LogPrint(LOG_LVL_ERROR, MID_PUB_ALWAYS, "[%s]seach IP failed\n", __func__);
		return 0;
	}
#else
	return 0; //非8000H平台不用考虑，也返回0
#endif
}

/*==============================================================================
函数名    :  GetMtPosInMtArray
功能      :  从终端列表ptMtList中获取具体终端tSpecMt所处的位置
算法实现  :  
参数说明  :  [in]tSpecMt  -- 待寻找的终端
			 [in]ptMtList -- 终端列表
			 [in]byMtNum  -- 终端列表数目
返回值说明:  终端tSpecMt所在ptMtList中的索引位置[0，byMtNum), 没找到返回-1
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2011/06/13                 liuxu                            创建
==============================================================================*/
const s16 GetMtPosInMtArray( const TMtAlias& tSpecMt, const TMtAlias* ptMtList, const u8 byMtNum )
{
	s16 swRet = -1;									// 原始返回值
	
	if ( !ptMtList || byMtNum == 0 )
	{
		return swRet;
	}

	for ( u8 byLoop = 0; byLoop < byMtNum; ++byLoop)
	{
		if (tSpecMt == ptMtList[byLoop])
		{
			swRet = byLoop;
			break;
		}
	}

	return swRet;
}

/*==============================================================================
函数名    :  GetConfExtraDataLenFromFile
功能      :  获得会议文件额外数据大小，现在只是针对vcs会议中的电视墙预案
算法实现  :  
参数说明  :  u8			byChnnl		[in]
			 TConfInfo	tConfInfo	[in]
			 u8			bySubVmpType[in]
			 u8			byMediaType	[out]
			 
返回值说明:  u8			byRes
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-11-02                  周晶晶                            创建
==============================================================================*/
BOOL32 GetConfExtraDataLenFromFile( TConfStore tConfStore, u16 &wPlanDataLen,  u16 &wExInfoLen, BOOL32 bIsDefaultConf )
{
	FILE *hConfFile = NULL;
	s8    achFullName[64];
	s32   nFileLen = 0;
	u16	  wOutBufLen = 0;
	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];


	u8 byConfPos = MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE;
	//缺省会议直接取MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE作为Idx[7/17/2012 chendaiwei]
	if(!bIsDefaultConf)
	{
		//1.在头信息记录中 获取 会议或模板的相对位置
		GetAllConfHeadFromFile(acConfId, sizeof(acConfId));

		//查找已有会议：此会议已保存，覆盖，不包含缺省会议位置
		// modify 包括缺省会议位置 [7/16/2012 chendaiwei]
		for (s32 nPos = 0; nPos < (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE); nPos++)
		{
			if (acConfId[nPos] == tConfStore.m_tConfInfo.GetConfId())
			{
				byConfPos = (u8)nPos;
				break;
			}
		}

		if( (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE) == byConfPos)
		{
			return FALSE;
		}
	}

	//读取 指定索引的会议或模板信息 存储文件 信息
    sprintf(achFullName, "%s/%s%d%s", DIR_DATA, CONFINFOFILENAME_PREFIX, byConfPos, CONFINFOFILENAME_POSTFIX);

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
		//wFileTConfInfoSize = ntohs(wFileTConfInfoSize);
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
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetConfDataFromFile] In new version, FileTConfInfoSize can't be 0!\n");
		fclose(hConfFile);
		EndRWConfInfoFile();
		hConfFile = NULL;
		return FALSE;
	}

	u8 *pbyBuf = NULL;

	if( !bIsNewVersion )
	{		
		fclose(hConfFile);
		hConfFile = NULL;		
		wPlanDataLen = 0;
		wExInfoLen = 0;
		EndRWConfInfoFile();
		return FALSE;
	}
	// confinfo size notify, zgc, 20070524
	else
	{
		
		TConfStore tBufConfStore;
		u16 wPackConfDataLen = 0;
		if (FALSE == PackConfStore(tConfStore, (TPackConfStore*)&tBufConfStore, wPackConfDataLen))
		{
			fclose(hConfFile);
			hConfFile = NULL;		
			wPlanDataLen = 0;
			wExInfoLen = 0;
			EndRWConfInfoFile();
			return FALSE;
		}

		if(IsV4R6B2VcsTemplate(hConfFile,(u16)nFileLen))
		{
			// fix2模板中少个u8 [11/13/2012 chendaiwei]
			wPackConfDataLen--;
		}

		if( wOutBufLen <= wPackConfDataLen )
		{
			fclose(hConfFile);
			hConfFile = NULL;		
			wPlanDataLen = 0;
			wExInfoLen = 0;
			EndRWConfInfoFile();
			return FALSE;
		}

		pbyBuf = new u8[ sizeof(TPackConfStore) ];
		memset( pbyBuf,0,sizeof( pbyBuf ) );

		u16 wRemainFileLen = wOutBufLen;

		if( wRemainFileLen >= wPackConfDataLen && wPackConfDataLen >= sizeof(TPackConfStore))
		{
			fread( pbyBuf,sizeof(TPackConfStore),1,hConfFile );
			fseek(hConfFile,wPackConfDataLen - sizeof(TPackConfStore),SEEK_CUR);
			wRemainFileLen -= wPackConfDataLen;
		}
		else
		{
			fclose(hConfFile);
			hConfFile = NULL;		
			wPlanDataLen = 0;
			wExInfoLen = 0;
			EndRWConfInfoFile();
			SAFE_DEL_ARRAY(pbyBuf);

			return FALSE;
		}

		BOOL32 bIgnoreUnpack = FALSE;
		if( wRemainFileLen == 0)
		{
			LogPrint( LOG_LVL_WARNING, MID_PUB_ALWAYS, "[GetConfExtraDataLenFromFile] wRemainFileLen == 0, v4r6 template(%s) without plan!\n",tConfStore.m_tConfInfo.GetConfName());

			bIgnoreUnpack = TRUE;
		}
		else if( wRemainFileLen >= sizeof(u16) )
		{
			fread(&wPlanDataLen,sizeof(u16),1,hConfFile);
			wRemainFileLen-= sizeof(u16);
		}
		else
		{
			fclose(hConfFile);
			hConfFile = NULL;		
			wPlanDataLen = 0;
			wExInfoLen = 0;
			EndRWConfInfoFile();
			SAFE_DEL_ARRAY(pbyBuf);

			return FALSE;
		}

		if( !bIgnoreUnpack )
		{
			wPlanDataLen = ntohs(wPlanDataLen);
			if( wRemainFileLen >= wPlanDataLen )
			{
				fseek(hConfFile,wPlanDataLen,SEEK_CUR);
				wRemainFileLen-= wPlanDataLen;
			}
			else
			{
				fclose(hConfFile);
				hConfFile = NULL;		
				wPlanDataLen = 0;
				wExInfoLen = 0;
				EndRWConfInfoFile();
				SAFE_DEL_ARRAY(pbyBuf);

				return FALSE;
			}
			
			if( wRemainFileLen == 0)
			{
				LogPrint( LOG_LVL_WARNING, MID_PUB_ALWAYS, "[GetConfExtraDataLenFromFile] wRemainFileLen == 0, v4r6 template(%s) has plan without ex-info!\n",tConfStore.m_tConfInfo.GetConfName());
				
				bIgnoreUnpack = TRUE;
			}
			else if( wRemainFileLen >= sizeof(u16) )
			{
				fread(&wExInfoLen,sizeof(u16),1,hConfFile);
				wRemainFileLen -= sizeof(u16);
			}
			else
			{
				fclose(hConfFile);
				hConfFile = NULL;		
				wExInfoLen = 0;
				EndRWConfInfoFile();
				SAFE_DEL_ARRAY(pbyBuf);

				return FALSE;
			}
			
			if( !bIgnoreUnpack )
			{
				wExInfoLen = ntohs(wExInfoLen);
				if(wRemainFileLen >= wExInfoLen )
				{
					wExInfoLen = wExInfoLen + sizeof(u16);
				}
				else
				{
					fclose(hConfFile);
					hConfFile = NULL;		
					wExInfoLen = 0;
					EndRWConfInfoFile();
					SAFE_DEL_ARRAY(pbyBuf);

					return FALSE;
				}
			}

		}
		
		fclose( hConfFile );
		hConfFile = NULL;			
	}
	// confinfo size notify end

	//校验 该会议或模板 头信息
	TPackConfStore *ptPackConfStore = (TPackConfStore *)pbyBuf;

	if (!(tConfStore.m_tConfInfo.GetConfId() == ptPackConfStore->m_tConfInfo.GetConfId()))
	{
		SAFE_DEL_ARRAY(pbyBuf);	
		wPlanDataLen = 0;
		wExInfoLen = 0;
        EndRWConfInfoFile();
		return FALSE;
	}

	SAFE_DEL_ARRAY(pbyBuf);
    EndRWConfInfoFile();

	return TRUE;
}
/*==============================================================================
函数名    :  GetConfExtraDataFromFile
功能      :  获得会议文件额外数据，现在只是针对vcs会议中的电视墙预案
算法实现  :  
参数说明  :  u8			byChnnl		[in]
			 TConfInfo	tConfInfo	[in]
			 u8			bySubVmpType[in]
			 u8			byMediaType	[out]
			 
返回值说明:  u8			byRes
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-11-02                  周晶晶                            创建
==============================================================================*/
BOOL32 GetConfExtraDataFromFile( TConfStore tConfStore, u8 *pbyBuf,u16 wBufInLen, u8 *pbyExInfoBuf, u16 wExInfoBufLen,BOOL32 &bHasFix2PlanData,BOOL32 bIsDefaultConf)
{
/*lint -save -esym(429, pbyBuf)*/
/*lint -save -esym(429, pbyExInfoBuf)*/
	
	FILE *hConfFile = NULL;
	s8    achFullName[64];
	s32   nFileLen = 0;
	u16	  wOutBufLen = 0;
	u16   wDataLen = 0;
	u16   wExInfoLen = 0; //扩展信息长度 [8/16/2012 chendaiwei]

	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];

	//缺省会议直接取MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE作为Idx[7/17/2012 chendaiwei]
	u8 byConfPos = MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE;
	if( !bIsDefaultConf )
	{
		//1.在头信息记录中 获取 会议或模板的相对位置
		GetAllConfHeadFromFile(acConfId, sizeof(acConfId));

		//查找已有会议：此会议已保存，覆盖，不包含缺省会议位置
		for (s32 nPos = 0; nPos <(MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE); nPos++)
		{
			if (acConfId[nPos] == tConfStore.m_tConfInfo.GetConfId())
			{
				byConfPos = (u8)nPos;
				break;
			}
		}

		if( (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE) == byConfPos)
		{
			return FALSE;
		}
	}

	//读取 指定索引的会议或模板信息 存储文件 信息
    sprintf(achFullName, "%s/%s%d%s", DIR_DATA, CONFINFOFILENAME_PREFIX, byConfPos, CONFINFOFILENAME_POSTFIX);

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
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetConfDataFromFile] In new version, FileTConfInfoSize can't be 0!\n");
		fclose(hConfFile);
		EndRWConfInfoFile();
		hConfFile = NULL;
		wDataLen = 0;
		return FALSE;
	}


	// confinfo size notify end
	u8 *pbyConfStoreBuf = NULL;
	if( !bIsNewVersion )
	{		
		fclose(hConfFile);
		hConfFile = NULL;		
		wDataLen = 0;
		EndRWConfInfoFile();
		return FALSE;
	}
	// confinfo size notify, zgc, 20070524
	else
	{
		TConfStore tBufConfStore;
		u16 wPackConfDataLen = 0;
		if (FALSE == PackConfStore(tConfStore, (TPackConfStore*)&tBufConfStore, wPackConfDataLen))
		{
			fclose(hConfFile);
			hConfFile = NULL;		
			wDataLen = 0;
			EndRWConfInfoFile();
			return FALSE;
		}

		BOOL32 bIsFix2Or8000HTemplate = FALSE;
		if(IsV4R6B2VcsTemplate(hConfFile,(u16)nFileLen))
		{
			// fix2模板中少个u8 [11/13/2012 chendaiwei]
			wPackConfDataLen--;
			bIsFix2Or8000HTemplate = TRUE;
		}

		//没有额外数据
		if( wOutBufLen <= wPackConfDataLen )
		{
			fclose(hConfFile);
			hConfFile = NULL;		
			wDataLen = 0;
			EndRWConfInfoFile();
			return FALSE;
		}

		//进去预案数据头的2字节的buf大小
		//wDataLen = wOutBufLen - wPackConfDataLen - sizeof(u16);

		pbyConfStoreBuf = new u8[ sizeof(TPackConfStore) ];
		memset( pbyConfStoreBuf,0,sizeof( pbyConfStoreBuf ) );
		fread( pbyConfStoreBuf,sizeof(TPackConfStore),1,hConfFile );

		if( 0 != fseek( hConfFile,wPackConfDataLen - sizeof(TPackConfStore),SEEK_CUR ) )
		{
			SAFE_DEL_ARRAY(pbyConfStoreBuf);
			fclose(hConfFile);
			hConfFile = NULL;		
			wDataLen = 0;
			EndRWConfInfoFile();
			return FALSE;
		}

		u16 wRemainFileLen = 0;
		if( wOutBufLen >= wPackConfDataLen )
		{
			wRemainFileLen = wOutBufLen - wPackConfDataLen;
		}
		else
		{
			SAFE_DEL_ARRAY(pbyConfStoreBuf);
			fclose(hConfFile);
			hConfFile = NULL;		
			wDataLen = 0;
			EndRWConfInfoFile();
			return FALSE;
		}

		BOOL32 bUnPackData = FALSE;
		if( wRemainFileLen == 0 )
		{
			bUnPackData = TRUE;
			LogPrint( LOG_LVL_WARNING, MID_PUB_ALWAYS, "[GetConfExtraDataFromFile] wRemainFileLen == 0, v4r6 template(%s) without plan!\n",tConfStore.m_tConfInfo.GetConfName());
		}
		else if( wRemainFileLen >= sizeof(u16))
		{
			fread(&wDataLen,sizeof(u16),1,hConfFile);
			wDataLen = ntohs(wDataLen);
			wRemainFileLen -= sizeof(u16);
		}
		else
		{
			SAFE_DEL_ARRAY(pbyConfStoreBuf);
			fclose(hConfFile);
			hConfFile = NULL;		
			wDataLen = 0;
			EndRWConfInfoFile();
			return FALSE;
		}
		
		//保护
		if( wDataLen >  wBufInLen)
		{
			SAFE_DEL_ARRAY(pbyConfStoreBuf);
			fclose(hConfFile);
			hConfFile = NULL;		
			wDataLen = 0;
			EndRWConfInfoFile();
			return FALSE;
		}
		
		if( wRemainFileLen >= wDataLen)
		{
			fread(pbyBuf,wDataLen,1,hConfFile);
			wRemainFileLen -= wDataLen;
			//标识是否有Fix2模板的预案数据[11/14/2012 chendaiwei]
			bHasFix2PlanData = bIsFix2Or8000HTemplate;
		}
		else
		{
			SAFE_DEL_ARRAY(pbyConfStoreBuf);
			fclose(hConfFile);
			hConfFile = NULL;		
			wDataLen = 0;
			EndRWConfInfoFile();
			return FALSE;
		}

		if( !bUnPackData )
		{
			if ( wRemainFileLen == 0 )
			{
				bUnPackData = TRUE;
				LogPrint( LOG_LVL_WARNING, MID_PUB_ALWAYS, "[GetConfExtraDataFromFile] wRemainFileLen == 0, v4r6 template(%s) has plan without ex-info!\n",tConfStore.m_tConfInfo.GetConfName());
			}
			else if( wRemainFileLen >= sizeof(u16))
			{
				fread(&wExInfoLen,sizeof(u16),1,hConfFile);
				wRemainFileLen -= sizeof(u16);
				wExInfoLen = ntohs(wExInfoLen);
			}
			else
			{
				SAFE_DEL_ARRAY(pbyConfStoreBuf);
				fclose(hConfFile);
				hConfFile = NULL;		
				wDataLen = 0;
				EndRWConfInfoFile();
				return FALSE;
			}
			
			//保护
			if(  wExInfoLen!= 0 && wExInfoLen + sizeof(u16) >  wExInfoBufLen )
			{
				SAFE_DEL_ARRAY(pbyConfStoreBuf);
				fclose(hConfFile);
				hConfFile = NULL;		
				EndRWConfInfoFile();
				return FALSE;
			}
			
			if( !bUnPackData )
			{
				if(wRemainFileLen >= wExInfoLen)
				{
					memcpy(pbyExInfoBuf,&wExInfoLen,sizeof(u16));
					fread(pbyExInfoBuf+sizeof(u16),wExInfoLen,1,hConfFile);
				}
				else
				{
					SAFE_DEL_ARRAY(pbyConfStoreBuf);
					fclose(hConfFile);
					hConfFile = NULL;		
					wDataLen = 0;
					EndRWConfInfoFile();
					return FALSE;
				}
			}
		}

		fclose( hConfFile );
		hConfFile = NULL;
		
	}
	// confinfo size notify end

	//校验 该会议或模板 头信息
	TPackConfStore *ptPackConfStore = (TPackConfStore *)pbyConfStoreBuf;

	if (!(tConfStore.m_tConfInfo.GetConfId() == ptPackConfStore->m_tConfInfo.GetConfId()))
	{
		SAFE_DEL_ARRAY(pbyConfStoreBuf);
		wOutBufLen = 0;
        EndRWConfInfoFile();
		return FALSE;
	}

	SAFE_DEL_ARRAY(pbyConfStoreBuf);
	
    EndRWConfInfoFile();

	return TRUE;

/*lint -restore*/
/*lint -restore*/
}

/*==============================================================================
函数名    :  SaveConfExtraTvWallPlanDataToFile
功能      :  获得会议文件额外数据，现在只是针对vcs会议中的电视墙预案
算法实现  :  
参数说明  :  u8			byChnnl		[in]
			 TConfInfo	tConfInfo	[in]
			 u8			bySubVmpType[in]
			 u8			byMediaType	[out]
			 
返回值说明:  u8			byRes
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-11-02                  周晶晶                            创建
==============================================================================*/
BOOL32 SaveConfExtraPlanDataToFile( TConfStore &tConfStore, u8 *pbyBuf,u16 wDataLen, BOOL32 bIsDefaultConf )
{
/*lint -save -esym(593, pbyBuf)*/

	FILE *hConfFile = NULL;
	s8    achFullName[64];
	s32   nFileLen = 0;
	u16	  wOutBufLen = 0;
//	u16	  wExtraDataLen = 0;

	CConfId acConfId[MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE+1];


	u8 byConfPos = MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE;
	if( !bIsDefaultConf )
	{
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

		if( (MAXNUM_MCU_CONF+MAXNUM_MCU_TEMPLATE) == byConfPos)
		{
			return FALSE;
		}
	}

	//读取 指定索引的会议或模板信息 存储文件 信息
    sprintf(achFullName, "%s/%s%d%s", DIR_DATA, CONFINFOFILENAME_PREFIX, byConfPos, CONFINFOFILENAME_POSTFIX);

	LogPrint(LOG_LVL_DETAIL, MID_MCU_CONF, "[SaveConfExtraPlanDataToFile tick.%d] save conf to file:%s\n", OspTickGet(), achFullName);

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
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetConfDataFromFile] In new version, FileTConfInfoSize can't be 0!\n");
		fclose(hConfFile);
		EndRWConfInfoFile();
		hConfFile = NULL;
		return FALSE;
	}

	
	// confinfo size notify end
	//u8 *pbyConfStoreBuf = NULL;
	if( !bIsNewVersion )
	{		
		fclose(hConfFile);
		hConfFile = NULL;		
		EndRWConfInfoFile();
		return FALSE;
	}
	// confinfo size notify, zgc, 20070524
	else
	{

		TConfStore tBufConfStore;
		u16 wPackConfDataLen = 0;

		if (FALSE == PackConfStore(tConfStore, (TPackConfStore*)&tBufConfStore, wPackConfDataLen))
		{
			fclose(hConfFile);
			hConfFile = NULL;		
			EndRWConfInfoFile();
			return FALSE;
		}


// 		if( wOutBufLen > wPackConfDataLen )
// 		{
// 			wExtraDataLen = wOutBufLen - wPackConfDataLen;
// 		}		


		u16 wPlanBufLen = wDataLen;

		{
			if( 0 != fseek(hConfFile, 0, SEEK_SET) )
			{
				//delete []pbyConfStoreBuf;
				fclose( hConfFile );
				hConfFile = NULL;
				return FALSE;
			}

			u16 byOrginalDataLen = wPackConfDataLen+strlen(CONFFILEHEAD) + sizeof(u16) + sizeof(u32);

			u8 *pbyOrginalBuf = new u8[byOrginalDataLen];
			fread( pbyOrginalBuf,byOrginalDataLen,1,hConfFile );
			fclose( hConfFile );
			
			hConfFile = fopen(achFullName, "wb"); 
			if (NULL == hConfFile)
			{
				//delete []pbyConfStoreBuf;
				delete[] pbyOrginalBuf;
				wOutBufLen = 0;
				EndRWConfInfoFile();
				return FALSE;
			}
			fclose( hConfFile );

			hConfFile = fopen(achFullName, "ab");
			if (NULL == hConfFile)
			{
				//delete []pbyConfStoreBuf;
				delete[] pbyOrginalBuf;
				wOutBufLen = 0;
				EndRWConfInfoFile();
				return FALSE;
			}

			fwrite( pbyOrginalBuf,byOrginalDataLen,1,hConfFile );

			delete []pbyOrginalBuf;

			//始终写入预案长度u16（可能是0）[8/16/2012 chendaiwei]
			wPlanBufLen = htons( wPlanBufLen );
			fwrite( &wPlanBufLen,sizeof(wPlanBufLen),1,hConfFile );
			if( wDataLen > 0  && NULL != pbyBuf )
			{
				fwrite( pbyBuf,wDataLen,1,hConfFile );
			}

			//写入扩展信息[8/16/2012 chendaiwei]
			u16 wExInfoLen = ntohs(*(u16*)&tConfStore.m_byConInfoExBuf[0]);
			if( wExInfoLen + sizeof(u16) <= CONFINFO_EX_BUFFER_LENGTH)
			{
				fwrite( &tConfStore.m_byConInfoExBuf[0],wExInfoLen+sizeof(u16),1,hConfFile);
			}
			else
			{
				fclose( hConfFile );
				EndRWConfInfoFile();
				return FALSE;
			}
		}
		
		fclose( hConfFile );
		hConfFile = NULL;
		
	}
	// confinfo size notify end

	//校验 该会议或模板 头信息
	/*TPackConfStore *ptPackConfStore = (TPackConfStore *)pbyConfStoreBuf;

	if (!(tConfStore.m_tConfInfo.GetConfId() == ptPackConfStore->m_tConfInfo.GetConfId()))
	{
		delete []pbyConfStoreBuf;
		wOutBufLen = 0;
        EndRWConfInfoFile();
		return FALSE;
	}
	*/

	//delete []pbyConfStoreBuf;
	
    EndRWConfInfoFile();

	return TRUE;

/*lint -restore*/
}

/*==============================================================================
函数名    : GetMtVidLmtForHd 
功能      : 得到终端参与VMP分辨率最高限制值（对于高清终端） 
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-2-20	4.6				薛亮							create
==============================================================================*/
BOOL32 CVmpMemVidLmt::GetMtVidLmtForHd ( u8 byStyle, u8 byMemPos, u8 &byMtRes, u16 &wMtBandWidth)
{
	
	if( byStyle >= VMPSTYLE_NUMBER || byMemPos >= MAXNUM_VMP_MEMBER)
	{
		return FALSE;
	}

	BOOL32 bRet = TRUE;
	byMtRes = m_atMtVidLmtHd[byStyle][byMemPos].GetMtVidRes();
	wMtBandWidth = m_atMtVidLmtHd[byStyle][byMemPos].GetMtVidBw();
	if(byMtRes == 0 || wMtBandWidth == 0)
	{
		bRet = FALSE;
	}
	return bRet;
}

/*==============================================================================
函数名    : GetMtVidLmtForStd 
功能      : 得到终端参与VMP分辨率最高限制值（对于标清终端）
算法实现  :  
参数说明  :  
返回值说明:  BOOL32
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-2-20	4.6				薛亮							create
==============================================================================*/
BOOL32 CVmpMemVidLmt::GetMtVidLmtForStd ( u8 byStyle, u8 byMemPos, u8 &byMtRes, u16 &wMtBandWidth)
{

	if( byStyle >= VMPSTYLE_NUMBER || byMemPos >= MAXNUM_VMP_MEMBER)
	{
		return FALSE;
	}

	BOOL32 bRet = TRUE;
	byMtRes = m_atMtVidLmtStd[byStyle][byMemPos].GetMtVidRes();
	wMtBandWidth = m_atMtVidLmtStd[byStyle][byMemPos].GetMtVidBw();
	if(byMtRes == 0 || wMtBandWidth == 0)
	{
		bRet = FALSE;
	}
	return bRet;
}

/*==============================================================================
类名    :  CSmsControl
功能    :  短消息发送控制
主要接口:  
备注    :  
-------------------------------------------------------------------------------
修改记录:  
日  期     版本          修改人          走读人          修改记录
2011-1-19                薛亮							 create
==============================================================================*/
CSmsControl::CSmsControl()
{
	Init();
}

void CSmsControl::Init()
{
	m_cServMsg.Init();
	m_wTimerSpan = 200;// 200ms
	m_wPos = 0;
	m_byState = IDLE;
}

void CSmsControl::SetMtPos(u16 wPos )
{
	m_wPos = wPos;
}

u16	CSmsControl::GetMtPos(void) const
{
	return m_wPos;
}

void CSmsControl::SetState(u8 byState)
{
	m_byState = byState;
}

BOOL32 CSmsControl::IsStateIdle(void) const
{
	return (m_byState == IDLE);
}

void CSmsControl::SetServMsg(const CServMsg &cServMsg)
{
	memcpy(&m_cServMsg, &cServMsg, sizeof(cServMsg));
}

CServMsg * CSmsControl::GetServMsg(void)
{
	return &m_cServMsg;
}

void CSmsControl::SetTimerSpan(u16 wTimerSpan)
{
	m_wTimerSpan = wTimerSpan;
}

u16 CSmsControl::GetTimerSpan(void) const
{
	return m_wTimerSpan;
}

/*==============================================================================
类名    :  CVmpMemVidLmt
功能    :  画面合成成员视频分辨率等能力限制
主要接口:  
备注    :  
-------------------------------------------------------------------------------
修改记录:  
日  期     版本          修改人          走读人          修改记录
2009-2-26
==============================================================================*/
void CVmpMemVidLmt::Init( void )
{
	u8 byStyle = 0;
	u8 byLoop = 0;
	memset( m_atMtVidLmtHd, 0, sizeof( m_atMtVidLmtHd ) );
	memset( m_atMtVidLmtStd, 0, sizeof( m_atMtVidLmtStd ) );
	const u16 wBW_8M = 8192;
	const u16 wBW_3M = 3072;
	const u16 wBW_1d5M = 1536;

#if !defined(_8KE_)
	byStyle = VMP_STYLE_ONE;	//1画面
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_HD1080);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);
	
	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4SIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);
	
	byStyle = VMP_STYLE_VTWO;	//左右2画面
	for(byLoop = 0; byLoop < 2;byLoop ++)
	{
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_960x544);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_8M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_4SIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_8M);
	}
	
	
	byStyle = VMP_STYLE_HTWO;	//一大一小2画面
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_HD1080);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);
	m_atMtVidLmtHd[byStyle][1].SetMtVidRes(VIDEO_FORMAT_640x368);
	m_atMtVidLmtHd[byStyle][1].SetMtVidBW(wBW_3M);
	
	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4SIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);
	m_atMtVidLmtStd[byStyle][1].SetMtVidRes(VIDEO_FORMAT_CIF);
	m_atMtVidLmtStd[byStyle][1].SetMtVidBW(wBW_1d5M);
	
	byStyle = VMP_STYLE_THREE;	//3画面
	for(byLoop = 0; byLoop < 3; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_960x544);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_8M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_4SIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_8M);
	}
	
	
	byStyle = VMP_STYLE_FOUR;	//4画面
	for(byLoop = 0; byLoop < 4; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_960x544);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_8M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_4SIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_8M);
	}
	
	
	byStyle = VMP_STYLE_SIX;	//6画面
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_HD720);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);
	
	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4SIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);
	
	for(byLoop = 1; byLoop < 6; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_640x368);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_3M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	
	
	byStyle = VMP_STYLE_EIGHT;	//8画面
	// 之前因mpu性能将8画面0通道分辨率改为720会影响到其它合成外设，不在初始化时做此处理
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_1440x816);
	//m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_HD720);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);
	
	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4SIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);
	
	for(byLoop = 1; byLoop < 8; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_480x272);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	
	
	byStyle = VMP_STYLE_NINE;	//9画面
	for(byLoop = 0; byLoop < 9; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_640x368);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_3M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	
				
	byStyle = VMP_STYLE_TEN;	//10画面
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_960x544);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);
	m_atMtVidLmtHd[byStyle][5].SetMtVidRes(VIDEO_FORMAT_960x544);
	m_atMtVidLmtHd[byStyle][5].SetMtVidBW(wBW_8M);
	
	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4SIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);
	m_atMtVidLmtStd[byStyle][5].SetMtVidRes(VIDEO_FORMAT_4SIF);
	m_atMtVidLmtStd[byStyle][5].SetMtVidBW(wBW_8M);
	
	for(byLoop = 1; byLoop < 5; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_480x272);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	for(byLoop = 6; byLoop < 10; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_480x272);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	
	byStyle = VMP_STYLE_THIRTEEN;//13画面
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_960x544);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);
	
	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4SIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);
	
	for(byLoop = 1; byLoop < 13; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_480x272);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	
				
	byStyle = VMP_STYLE_SIXTEEN;//16画面
	for(byLoop = 0; byLoop < 16; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_480x272);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	
	
	byStyle = VMP_STYLE_SPECFOUR;//特殊4画面
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_HD720);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);
	
	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4SIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);
	
	for(byLoop = 1; byLoop < 4; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_640x368);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_3M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	
	
	byStyle = VMP_STYLE_SEVEN;	//7画面
	for(byLoop = 0; byLoop < 3; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_960x544);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_8M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_4SIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_8M);
	}
	for(byLoop = 3; byLoop < 7; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_480x272);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	
	
	byStyle = VMP_STYLE_TWENTY; //20画面
	for(byLoop = 0; byLoop < 20; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_480x272);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}

	byStyle = VMP_STYLE_FIFTEEN;//15画面
	for(byLoop = 0; byLoop < 3; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_640x544);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_3M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_640x544);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_3M);
	}

	for(byLoop = 3; byLoop < 15; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_320x272);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_320x272);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	
	byStyle = VMP_STYLE_TEN_M; //10画面(中间左右)
	for(byLoop = 0; byLoop < 2; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_960x544);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_8M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_4SIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_8M);
	}
	for(byLoop = 2; byLoop < 10; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_480x272);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	
	byStyle = VMP_STYLE_THIRTEEN_M; //13画面(一大在中间)
	for(byLoop = 0; byLoop < 1; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_960x544);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_8M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_4SIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_8M);
	}
	for(byLoop = 1; byLoop < 13; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_480x272);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}

	byStyle = VMP_STYLE_TWENTYFIVE; //25画面
	for(byLoop = 0; byLoop < 25; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
		
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}


#else
	byStyle = VMP_STYLE_ONE;	//1画面
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_HD720);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);
	
	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4CIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);

	byStyle = VMP_STYLE_VTWO;	//左右2画面
	for(byLoop = 0; byLoop < 2;byLoop ++)
	{
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_640x368);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_8M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_4CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_8M);
	}

	
	byStyle = VMP_STYLE_HTWO;	//一大一小2画面
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_HD720);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);
	m_atMtVidLmtHd[byStyle][1].SetMtVidRes(VIDEO_FORMAT_720_432x240);
	m_atMtVidLmtHd[byStyle][1].SetMtVidBW(wBW_3M);
	
	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4CIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);
	m_atMtVidLmtStd[byStyle][1].SetMtVidRes(VIDEO_FORMAT_4CIF);
	m_atMtVidLmtStd[byStyle][1].SetMtVidBW(wBW_1d5M);

	byStyle = VMP_STYLE_THREE;	//3画面
	for(byLoop = 0; byLoop < 3; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_640x368);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_8M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_4CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_8M);
	}
	
	
	byStyle = VMP_STYLE_FOUR;	//4画面
	for(byLoop = 0; byLoop < 4; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_640x368);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_8M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_4CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_8M);
	}

	
	byStyle = VMP_STYLE_SIX;	//6画面
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_720_864x480);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);

	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4CIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);

	for(byLoop = 1; byLoop < 6; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_432x240);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_3M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}

	
	byStyle = VMP_STYLE_EIGHT;	//8画面
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_720_960x544);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);

	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4CIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);

	for(byLoop = 1; byLoop < 8; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_320x192);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);

 		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
 		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}

	
	byStyle = VMP_STYLE_NINE;	//9画面
	for(byLoop = 0; byLoop < 9; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_432x240);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_3M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}

			
	byStyle = VMP_STYLE_TEN;	//10画面
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_720_640x368);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);
	m_atMtVidLmtHd[byStyle][5].SetMtVidRes(VIDEO_FORMAT_720_640x368);
	m_atMtVidLmtHd[byStyle][5].SetMtVidBW(wBW_8M);

	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4CIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);
	m_atMtVidLmtStd[byStyle][5].SetMtVidRes(VIDEO_FORMAT_4CIF);
	m_atMtVidLmtStd[byStyle][5].SetMtVidBW(wBW_8M);

	for(byLoop = 1; byLoop < 5; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_320x192);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}
	for(byLoop = 6; byLoop < 10; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_320x192);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}

    	
	byStyle = VMP_STYLE_THIRTEEN;//13画面
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_720_640x368);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);

	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4CIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);

	for(byLoop = 1; byLoop < 13; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_320x192);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}

			
	byStyle = VMP_STYLE_SIXTEEN;//16画面
	for(byLoop = 0; byLoop < 16; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_320x192);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}

	
	byStyle = VMP_STYLE_SPECFOUR;//特殊4画面
	m_atMtVidLmtHd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_720_864x480);
	m_atMtVidLmtHd[byStyle][0].SetMtVidBW(wBW_8M);

	m_atMtVidLmtStd[byStyle][0].SetMtVidRes(VIDEO_FORMAT_4CIF);
	m_atMtVidLmtStd[byStyle][0].SetMtVidBW(wBW_8M);

	for(byLoop = 1; byLoop < 4; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_432x240);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_3M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_4CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}


	byStyle = VMP_STYLE_SEVEN;	//7画面
	for(byLoop = 0; byLoop < 3; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_640x368);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_8M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_4CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_8M);
	}
	for(byLoop = 3; byLoop < 7; byLoop ++)
	{	
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_720_320x192);
		m_atMtVidLmtHd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);

		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidRes(VIDEO_FORMAT_CIF);
		m_atMtVidLmtStd[byStyle][byLoop].SetMtVidBW(wBW_1d5M);
	}

#endif
	
}

/*==============================================================================
函数名    :  GetMacStrSegmentedbySep
功能      :  用指定分割符得到mac地址
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
20100601                   薛亮                            创建
==============================================================================*/
void GetMacStrSegmentedbySep(u8 *pbyBufIn, s8 *pchBufOut, s8 *pchSep)
{
	sprintf(pchBufOut, "%02X%s%02X%s%02X%s%02X%s%02X%s%02X", 
		pbyBufIn[0], pchSep,
		pbyBufIn[1], pchSep,
		pbyBufIn[2], pchSep,
		pbyBufIn[3], pchSep,
		pbyBufIn[4], pchSep,
		pbyBufIn[5]
		);
	
	return;
	
}


/************************************************************************/
/*                                                                      */
/*                            TApplySpeakQue                            */
/*                                                                      */
/************************************************************************/

//构造
TApplySpeakQue::TApplySpeakQue() : 
                       m_byLen(MAXNUM_CONF_MT),
                       m_nHead(0),
                       m_nTail(0),
                       m_bPrtChgInfo(FALSE)
{
						   memset(m_atMtList, 0, sizeof(m_atMtList));
}

//析构
TApplySpeakQue::~TApplySpeakQue()
{
/*lint -save -e1551*/
    Quit();
/*lint -restore*/
}

/*=============================================================================
  函 数 名： Init
  功    能： 队列初始内存分配
  算法实现： 
  全局变量： 
  参    数： u8 byQueueLen
  返 回 值： void
 -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/04/05    4.0			张宝卿                  创建
=============================================================================*/
void TApplySpeakQue::Init( void )
{
	/*
    if ( 0 == byQueueLen ) 
    {
        LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[TApplySpeakQue] err: Len.%d, Init failed !\n", byQueueLen );
        return;
    }*/

    //m_ptMtList = new TMt[byQueueLen];
	
	memset(m_atMtList, 0, sizeof(m_atMtList));
	m_byLen = MAXNUM_CONF_MT;
	/*
    if ( NULL != m_ptMtList )
    {
        m_byLen = byQueueLen;
        memset(m_ptMtList, 0, sizeof(TMt) * byQueueLen);
    }
    else
    {
        LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[TApplySpeakQue] Init failed !\n" );
    }*/
    return;    
}

/*=============================================================================
  函 数 名： Quit
  功    能： 队列内存释放
  算法实现： 
  全局变量： 
  参    数：
  返 回 值： void
 -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/04/05    4.0			张宝卿                  创建
=============================================================================*/
void TApplySpeakQue::Quit()
{
	/*
    if ( NULL != m_ptMtList ) 
    {
        delete [] m_ptMtList;
        m_ptMtList = NULL;
    }*/
	memset(m_atMtList, 0, sizeof(m_atMtList));
    m_byLen = 0;
    m_nHead = 0;
    m_nTail = 0;
    m_bPrtChgInfo = FALSE;
}

/*=============================================================================
  函 数 名： ProcQueueInfo
  功    能： 处理队列
  算法实现： 本队列 不 遵循严格的 FIFO 规则
  全局变量： 
  参    数： TMt &tMt:
             BOOL32      bInc      : TRUE 入列；    FALSE 随机出列 或 正常出列
             BOOL32      bDel      : TRUE 随机出列；FALSE 正常出列
  返 回 值： BOOL32 
 -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/04/05    4.0			张宝卿                  创建
=============================================================================*/
BOOL32 TApplySpeakQue::ProcQueueInfo( TMt &tMt, BOOL32 &bIsSendToChairman, BOOL32 bInc, BOOL32 bDel )
{
	bIsSendToChairman = FALSE;
	u8 byCount = 0;
    if ( bInc )
    {
        if ( IsQueueFull() )
        {
			QueueLog("[ProcQueueInfo] Buffer Full!\n");
            return FALSE;
        }
        if ( tMt.IsNull() ) 
        {
			QueueLog("[ProcQueueInfo] tMt Null!\n");
            return FALSE;
        }
        if ( IsMtInQueue(tMt) )
        {
			QueueLog("[ProcQueueInfo] Mt<%d, %d> exist already!\n",
                      tMt.GetMcuId(), tMt.GetMtId());


            return FALSE;
        }

        memcpy(&m_atMtList[m_nTail], &tMt, sizeof(tMt));
        m_nTail = (m_nTail + 1) % m_byLen;

		byCount = m_nTail > m_nHead ? m_nTail - m_nHead  : ( m_byLen - m_nHead) + m_nTail ;
		if( byCount <= MAXNUM_ANSWERINSTANTLY_MT )
		{
			bIsSendToChairman = TRUE;
		}
		
        QueueLog("[ProcQueueInfo] Mt <%d, %d> INC queue !\n",
                  tMt.GetMcuId(), tMt.GetMtId());

        if ( m_bPrtChgInfo ) 
        {
            ShowQueue();
        }

        return TRUE;
    }
    else
    {
        if ( IsQueueNull() )
        {
			QueueLog("[ProcQueueInfo] Buffer NULL!\n");
            return FALSE;
        }

        if ( bDel ) 
        {
            if ( tMt.IsNull() ) 
            {
                QueueLog("[ProcQueueInfo] tCallInfo Null<Del>!\n");
                return FALSE;
            }
            
            u8 byQue = 0;
            if ( !IsMtInQueue(tMt, &byQue) )
            {

                QueueLog( "[ProcQueueInfo] Mt <%d, %d> to be DE unexist !\n",
                           tMt.GetMcuId(), tMt.GetMtId());

                return FALSE;
            }
            m_atMtList[byQue].SetNull();
            
            // 删除的MT不在队列头，整理队列
            if ( m_nHead != byQue )
            {
                s32 nPos = 0;
                s32 nAdjustPos = byQue > m_nHead ? byQue : byQue + m_byLen;
				s32 nQueNew = 0;
				s32 nQueNewFr = 0;
				
                
                for( nPos = nAdjustPos; nPos > m_nHead; nPos-- )
                {
                    nQueNew   = nPos;
                    nQueNewFr = nQueNew - 1;
                    nQueNew   = nQueNew % m_byLen;
                    nQueNewFr = nQueNewFr % m_byLen;
                    m_atMtList[nQueNew] = m_atMtList[nQueNewFr];
					++byCount;
                }
				if( byCount < MAXNUM_ANSWERINSTANTLY_MT )
				{
					bIsSendToChairman = TRUE;
				}
                m_atMtList[m_nHead].SetNull();
            }
			else
			{
				bIsSendToChairman = TRUE;
			}
            m_nHead = (m_nHead + 1) % m_byLen;
        }
        else
        {
            if ( NULL != m_atMtList &&
				!m_atMtList[m_nHead].IsNull() )
            {
                tMt = m_atMtList[m_nHead];
                m_atMtList[m_nHead].SetNull();
                m_nHead = (m_nHead + 1) % m_byLen;
				bIsSendToChairman = TRUE;
            }
            else
            {
                QueueLog("[ProcQueueInfo] None member in queue !\n");
            }
        }


        QueueLog("[ProcQueueInfo] Mt <%d, %d> DE queue !\n",
                  tMt.GetMcuId(), tMt.GetMtId());

        
        if ( m_bPrtChgInfo ) 
        {
            ShowQueue();
        }

        return TRUE;
    }
}

/*=============================================================================
  函 数 名： IsQueueNull
  功    能： 出队头
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： BOOL32 
 -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2010/06/01    4.0			周晶晶                  创建
=============================================================================*/
BOOL32 TApplySpeakQue::PopQueueHead( TMt &tMt )
{
	if ( IsQueueNull() )
    {
		QueueLog("[PopQueueHead] Buffer NULL!\n");
		tMt.SetNull();
        return FALSE;
    }

	tMt = m_atMtList[m_nHead];	
	m_atMtList[m_nHead].SetNull();
	m_nHead = (m_nHead + 1) % m_byLen;

	return TRUE;	
}

/*=============================================================================
  函 数 名： PopQueue
  功    能： 弹出队列中的任意一个终端
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： BOOL32 
 -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2010/06/01    4.0			周晶晶                  创建
=============================================================================*/
BOOL32 TApplySpeakQue::GetQueueHead( TMt &tMt )
{
	if ( IsQueueNull() )
    {
		QueueLog("[PopQueueHead] Buffer NULL!\n");
		tMt.SetNull();
        return FALSE;
    }

	tMt = m_atMtList[m_nHead];		

	return TRUE;	
}

/*=============================================================================
  函 数 名： GetQueueNextMt
  功    能： 弹出队列中的某个终端后的一个终端
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： BOOL32 
 -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2010/06/01    4.0			周晶晶                  创建
=============================================================================*/
BOOL32 TApplySpeakQue::GetQueueNextMt( const TMt tCurMt,TMt &tNextMt )
{
	if ( IsQueueNull() ) 
    {
        return FALSE;
    }

	u8 byPos = 0;
	if( IsMtInQueue( tCurMt,&byPos ) )
	{
		byPos += 1;		
		byPos = byPos % m_byLen;		
		tNextMt = m_atMtList[byPos];
		if( !tNextMt.IsNull() )
		{
			return TRUE;
		}		
	}
	return FALSE;
}

/*=============================================================================
  函 数 名： RemoveMtByMcuIdx
  功    能： 删除某个mcu下的终端
  算法实现： 
  全局变量： 
  参    数： wMcuIdx mcu索引号
  返 回 值： BOOL32 
 -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/04/05    4.0			张宝卿                  创建
=============================================================================*/
BOOL32 TApplySpeakQue::RemoveMtByMcuIdx( u16 wMcuIdx,BOOL32 &bIsSendToChairman )
{
	TMt tMt;
	u8 byPos = 0;
	u8 byNum = 0;
	bIsSendToChairman = FALSE;
	BOOL32 bSend = FALSE;

	BOOL32 bLoopFlag = TRUE;
	while( bLoopFlag )
	{
		tMt.SetMcuId( wMcuIdx ); 
		if( !IsMcuInQueue( tMt,&byPos ) )
		{
			break;
		}
		tMt = m_atMtList[byPos];
		ProcQueueInfo( tMt,bSend,FALSE );
		bIsSendToChairman = bSend ? TRUE:bIsSendToChairman;
		++byNum;
	}
	return ( byNum > 0 );
}

/*=============================================================================
  函 数 名： RemoveMtByMcuIdx
  功    能： 删除某个mcu下的终端
  算法实现： 
  全局变量： 
  参    数： wMcuIdx mcu索引号
  返 回 值： BOOL32 
 -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/04/05    4.0			张宝卿                  创建
=============================================================================*/
BOOL32 TApplySpeakQue::IsMcuInQueue( TMt &tMt, u8* pbyPos )
{
	if ( IsQueueNull() ) 
    {
        return FALSE;
    }
    if ( INVALID_MCUIDX == tMt.GetMcuId() ) 
    {
        return FALSE;
    }

    s32 nQue  = m_nHead;
    s32 nTail = m_nTail > m_nHead ? m_nTail : m_nTail + m_byLen;

    for( ; nQue < nTail; nQue++ )
    {
        s32 nPos = nQue;
        nPos = nPos % m_byLen;

        if ( tMt.GetMcuId() == m_atMtList[nPos].GetMcuId() )
        {
            if ( NULL != pbyPos )
            {
                *pbyPos = (u8)nPos;
            }
            return TRUE;
        }
    }
    return FALSE;
}
/*=============================================================================
  函 数 名： IsQueueNull
  功    能： 队列是否为空
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： BOOL32 
 -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/04/05    4.0			张宝卿                  创建
=============================================================================*/
BOOL32 TApplySpeakQue::IsQueueNull()
{
	/*
	if( NULL == m_atMtList )
	{
		return TRUE;
	}
	*/
    if ( m_atMtList[m_nHead].IsNull() )
    {
        return TRUE;
    }
    return FALSE;
}

/*=============================================================================
  函 数 名： IsQueueFull
  功    能： 队列是否满
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： BOOL32 
 -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/04/05    4.0			张宝卿                  创建
=============================================================================*/
BOOL32 TApplySpeakQue::IsQueueFull()
{
	/*
	if( NULL == m_ptMtList )
	{
		return FALSE;
	}
	*/
    if ( !m_atMtList[m_nTail].IsNull() )
    {
        return TRUE;
    }
    return FALSE;
}

/*=============================================================================
  函 数 名： IsMtInQueue
  功    能： 查询是否在列
  算法实现： 
  全局变量： 
  参    数： u8 byConfIdx
             u8 byMtId
             u8 byPos   :具体位置
  返 回 值： BOOL32 
 -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/04/05    4.0			张宝卿                  创建
=============================================================================*/
BOOL32 TApplySpeakQue::IsMtInQueue( const TMt &tMt, u8* pbyPos )
{
    if ( IsQueueNull() ) 
    {
        return FALSE;
    }
    if ( tMt.IsNull() ) 
    {
        return FALSE;
    }

    s32 nQue  = m_nHead;
    s32 nTail = m_nTail > m_nHead ? m_nTail : m_nTail + m_byLen;

    for( ; nQue < nTail; nQue++ )
    {
        s32 nPos = nQue;
        nPos = nPos % m_byLen;

        if ( tMt.GetMcuId() == m_atMtList[nPos].GetMcuId() &&
               tMt.GetMtId()  == m_atMtList[nPos].GetMtId())
        {
            if ( NULL != pbyPos )
            {
                *pbyPos = (u8)nPos;
            }
            return TRUE;
        }
    }
    return FALSE;
}

/*=============================================================================
  函 数 名： ClearQueue
  功    能： 清空队列成员
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
 -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/05/22    4.0			张宝卿                  创建
=============================================================================*/
void TApplySpeakQue::ClearQueue()
{
    memset( m_atMtList, 0, sizeof(m_atMtList) );
    m_nHead = 0;
    m_nTail = 0;
    m_bPrtChgInfo = FALSE;

    return;
}

/*=============================================================================
  函 数 名： ShowQueue
  功    能： 显示当前的呼叫队列和等待队列
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： BOOL32 
 -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/04/05    4.0			张宝卿                  创建
=============================================================================*/
void TApplySpeakQue::ShowQueue()
{
    // zbq [06/12/2007] 异常保护
    if ( m_byLen == 0 )
    {
        return;
    }
    
    BOOL32 bNull  = IsQueueNull();

    s32 nPos        = 0;
    s32 nAdjustTail = 0;

    if ( !bNull ) 
    {
        StaticLog("\n--------------------- Apply Queue ----------------------\n" );
        
        StaticLog("Head.%d, Tail.%d, Len.%d \n", m_nHead, m_nTail,
                                 m_nTail>m_nHead ? m_nTail-m_nHead : m_nTail+m_byLen-m_nHead );

        nAdjustTail = m_nHead < m_nTail ? m_nTail : m_nTail + m_byLen;

        for( nPos = m_nHead; nPos < nAdjustTail; nPos++ )
        {
            s32 nRealPos = nPos;
            nRealPos = nRealPos % m_byLen;

            StaticLog( "   queue[%d]: < McuId.%d, MtId.%d> \n",
                                        nRealPos,
                                        m_atMtList[nRealPos].GetMcuId(),
                                        m_atMtList[nRealPos].GetMtId());
                        
        }
        StaticLog("--------------------- Apply Queue End ------------------\n\n" );
    }
    else
    {
        StaticLog("\n********************* Apply Queue NULL *****************\n\n" );
    }
    return;
}

/*=============================================================================
  函 数 名： PrtQueueInfo
  功    能： 实时打印呼叫队列和等待队列的出入列信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void
 -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/04/06    4.0			张宝卿                  创建
=============================================================================*/
void TApplySpeakQue::PrtQueueInfo( BOOL32 bPrt /* = TRUE */)
{
    m_bPrtChgInfo = bPrt;
}


/*=============================================================================
  函 数 名： QueueLog
  功    能： 
  算法实现： 
  全局变量： 
  参    数：  s8* pszStr...
  返 回 值： void 
----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/4/17  4.0			周广程                  创建
=============================================================================*/
void TApplySpeakQue::QueueLog( s8* pszStr, ... )
{
	s8 achBuf[1024];
    va_list argptr;
    if ( m_bPrtChgInfo )
    {
        s32 nPrefix = sprintf( achBuf, "[MtQueue]:" );
        va_start( argptr, pszStr );
		vsnprintf(achBuf + nPrefix, 1024 - nPrefix - 1, pszStr, argptr );
        //vsprintf( achBuf + nPrefix, pszStr, argptr );
        LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, achBuf );
        va_end( argptr );
    }
}

/*=============================================================================
  函 数 名： GetQueueList
  功    能： 
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2010/5/13     4.6			张宝卿                  创建
=============================================================================*/
void TApplySpeakQue::GetQueueList(IN OUT TMt *ptMtList, IN OUT u8 &byLen)
{
    if (IsQueueNull())
    {
        byLen = 0;
        return;
    }

    u8 byListLen = 0;
    if (m_nTail > m_nHead)
    {
        byListLen = m_nTail - m_nHead;
    }
    else
    {
        byListLen = m_nTail + m_byLen - m_nHead;
    }

    if (byListLen > byLen)
    {
		byLen = 0;
        LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[GetQueueList] Non enough memery<ListLen.%d, MemLen.%d> applyed, ignore it!\n", byListLen, byLen);
        return;
    }

    s32 nQue  = m_nHead;
    s32 nTail = m_nTail > m_nHead ? m_nTail : m_nTail + m_byLen;

    byLen = 0;

    for( ; nQue < nTail; nQue++ )
    {
        s32 nPos = nQue;
        nPos = nPos % m_byLen;
        ptMtList[byLen] = m_atMtList[nPos];
        byLen ++;
    }

    return;
}

/*====================================================================
函数名      : IsConfSupportMainCap
功能        : 会议是否有相应的勾选支持主格式能力
算法实现    ：
引用全局变量：
输入参数说明：const TConfInfoEx& tConfinfo 会议勾选信息
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2011/01/09  4.7         倪志俊         创建
====================================================================*/
BOOL32 IsConfSupportMainCap(const TConfInfoEx& tConfInfoEx, const TSimCapSet &tMainSimCapSet)
{
	//获得模板勾选
	TVideoStreamCap tVideoCap[MAX_CONF_CAP_EX_NUM];
	u8 byCapNUm = MAX_CONF_CAP_EX_NUM;
	if ( tConfInfoEx.GetMainStreamCapEx(tVideoCap,byCapNUm) )
	{
		if ( byCapNUm == 0 )
		{
			return FALSE;
		}

		for ( u8 byIdx = 0; byIdx<byCapNUm; byIdx++)
		{
			if ( tVideoCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL )
			{
				continue;
			}
			
			//媒体类型一致
			if ( tMainSimCapSet.GetVideoMediaType() != tVideoCap[byIdx].GetMediaType()  )
			{
				continue;
			}

			//profile能力一致
			if ( tMainSimCapSet.GetVideoProfileType() != tVideoCap[byIdx].GetH264ProfileAttrb() )
			{
				continue;
			}

			//勾选分辨率小于主格式分辨率
			if ( tMainSimCapSet.GetVideoResolution() < tVideoCap[byIdx].GetResolution() )
			{
				continue;
			}

			//勾选的帧率小于主格式帧率
			if ( tMainSimCapSet.GetUserDefFrameRate() < tVideoCap[byIdx].GetUserDefFrameRate() )
			{
				continue;
			}

			return TRUE;
		}
	}

	return FALSE;
}

/*====================================================================
函数名      : IsConfSupportDSCap
功能        : 会议是否有相应的勾选支持能力小于某种能力
算法实现    ：
引用全局变量：
输入参数说明：const TConfInfoEx& tConfinfo 会议勾选信息
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2011/01/09  4.7         倪志俊         创建
====================================================================*/
BOOL32 IsConfSupportDSCap(const TConfInfoEx& tConfInfoEx, const TDStreamCap &tSimCapSet)
{
	//获得模板勾选
	TVideoStreamCap tVideoCap[MAX_CONF_CAP_EX_NUM];
	u8 byCapNUm = MAX_CONF_CAP_EX_NUM;
	if ( tConfInfoEx.GetDoubleStreamCapEx(tVideoCap,byCapNUm) )
	{
		if ( byCapNUm == 0 )
		{
			return FALSE;
		}

		for ( u8 byIdx = 0; byIdx<byCapNUm; byIdx++)
		{
			if ( tVideoCap[byIdx].GetMediaType() == MEDIA_TYPE_NULL )
			{
				continue;
			}
			
			//媒体类型
			if ( tSimCapSet.GetMediaType() != tVideoCap[byIdx].GetMediaType()  )
			{
				continue;
			}

			//profile能力
			if ( tSimCapSet.GetH264ProfileAttrb() != tVideoCap[byIdx].GetH264ProfileAttrb() )
			{
				continue;
			}

			//分辨率
			if ( tSimCapSet.GetResolution() < tVideoCap[byIdx].GetResolution() )
			{
				continue;
			}

			//帧率
			if ( tSimCapSet.GetUserDefFrameRate() < tVideoCap[byIdx].GetUserDefFrameRate() )
			{
				continue;
			}

			return TRUE;
		}
	}

	return FALSE;
}

/*====================================================================
函数名      : IsConfCanCompactAdapt
功能        : 判断会议是否能紧凑适配
算法实现    ：
引用全局变量：
输入参数说明：const TConfInfo& tConfinfo 会议信息
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
03/01/2010              周嘉麟         创建
====================================================================*/
BOOL32 IsConfCanCompactAdapt(const TConfInfo& tConfinfo, const TConfInfoEx &tConfInfoEx)
{
	if (tConfinfo.GetMainVideoMediaType() != MEDIA_TYPE_H264)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[IsConfCanCompactAdapt] conf's Main Media is not h264!\n");
		return FALSE;
	}
	
	//双速
	if ( 0 == tConfinfo.GetSecBitRate())
	{
		return FALSE;
	}
	//主格式多分辨率时紧凑适配
	return !tConfInfoEx.IsMainHasCapEx();
}

u8 TransFRtoReal(u8 byMediaType, u8 byFrame)
{
	if (byMediaType == 0 || byMediaType == MEDIA_TYPE_H264)
	{
		return 0;
	}

	u8 byFrameRate = byFrame;

	if (byMediaType != MEDIA_TYPE_H264)
	{		
		switch(byFrameRate)
		{
		case VIDEO_FPS_2997_1:
			byFrameRate = 30;
			break;
		case VIDEO_FPS_25:
			byFrameRate = 25;
			break;
		case VIDEO_FPS_2997_2:
			byFrameRate = 15;
			break;
		case VIDEO_FPS_2997_3:
			byFrameRate = 10;
			break;
		case VIDEO_FPS_2997_4:
		case VIDEO_FPS_2997_5:
			byFrameRate = 6;
			break;
		case VIDEO_FPS_2997_6:
			byFrameRate = 5;
			break;
		case VIDEO_FPS_2997_30:
			byFrameRate = 1;
			break;
		case VIDEO_FPS_2997_7P5:
			byFrameRate = 4;
			break;
		case VIDEO_FPS_2997_10:
			byFrameRate = 3;
			break;
		case VIDEO_FPS_2997_15:
			byFrameRate = 2;
			break;
		default:
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[TransFRtoReal] undef non264 framerate%d\n", byFrameRate);
			break;
		}
	}
	return byFrameRate;
}

/*====================================================================
函数名      : GetDownStandRes
功能        : 获取比当前分辨率小的一个标准分辨率
算法实现    ：
引用全局变量：
输入参数说明：u8 byRes 当前分辨率
返回值说明  ：向下匹配到的标准分辨率
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
05/21/2010              周嘉麟         创建
====================================================================*/
u8 GetDownStandRes(u8 byRes)
{
	if (VIDEO_FORMAT_INVALID == byRes || 0 == byRes)
	{
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetDownStandRes] byRes is unexpected %d!\n", byRes) ;
		return VIDEO_FORMAT_INVALID;
	}
	
	u16 wOrgWidth  = 0;
	u16 wOrgHeight = 0;
	GetWHByRes(byRes, wOrgWidth, wOrgHeight);
	
	if (wOrgWidth == 0 || wOrgHeight == 0)
	{
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetDownStandRes]wOrgWidth or wOrgHeight = 0!\n");
		return VIDEO_FORMAT_INVALID;
	}
	
    u8 abyStandRes[4] = {VIDEO_FORMAT_HD1080, 
		VIDEO_FORMAT_HD720,
		VIDEO_FORMAT_4CIF,
		VIDEO_FORMAT_CIF};
	
	u16 wStandWidth  = 0;
	u16 wStandHeight = 0;
	for(u8 byId = 0; byId < sizeof(abyStandRes)/sizeof(u8); byId++)
	{
		wStandWidth  = 0;
		wStandHeight = 0;
		GetWHByRes(abyStandRes[byId], wStandWidth, wStandHeight);
		if ((wOrgWidth * wOrgHeight) > (wStandWidth * wStandHeight))
		{
			return abyStandRes[byId];
		}
		
		//小于cif的非标分辨率都以cif处理
		if (byId == ((sizeof(abyStandRes)/sizeof(u8)) -1))
		{
			if ((wOrgWidth * wOrgHeight) < (wStandWidth * wStandHeight))
			{
				return abyStandRes[byId];
			}			
		}
	}
	return VIDEO_FORMAT_INVALID;
}

/*====================================================================
函数名      : GetUpStandRes
功能        : 获取比当前分辨率大的一个标准分辨率
算法实现    ：
引用全局变量：
输入参数说明：u8 byRes 当前分辨率
返回值说明  ：向下匹配到的标准分辨率
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2011/08/23  4.6         zhangli         创建
====================================================================*/
u8 GetUpStandRes(u8 byRes)
{
	if (VIDEO_FORMAT_INVALID == byRes || 0 == byRes)
	{
		return VIDEO_FORMAT_INVALID;
	}
	
	u16 wOrgWidth  = 0;
	u16 wOrgHeight = 0;
	GetWHByRes(byRes, wOrgWidth, wOrgHeight);
	
	if (wOrgWidth == 0 || wOrgHeight == 0)
	{
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetUpStandRes]wOrgWidth or wOrgHeight = 0!\n");
		return VIDEO_FORMAT_INVALID;
	}
	
	u8 abyStandRes[4] = {VIDEO_FORMAT_CIF, VIDEO_FORMAT_4CIF, VIDEO_FORMAT_HD720, VIDEO_FORMAT_HD1080};
	
	u16 wStandWidth  = 0;
	u16 wStandHeight = 0;
	for(u8 byId = 0; byId < sizeof(abyStandRes)/sizeof(u8); byId++)
	{
		wStandWidth  = 0;
		wStandHeight = 0;
		GetWHByRes(abyStandRes[byId], wStandWidth, wStandHeight);
		if ((wOrgWidth * wOrgHeight) < (wStandWidth * wStandHeight))
		{
			return abyStandRes[byId];
		}
	}
	return VIDEO_FORMAT_INVALID;
}

/*====================================================================
函数名      : GetMinResAcdWHProduct
功能        : 根据
算法实现    ：
引用全局变量：
输入参数说明：u8 byRes 当前分辨率
返回值说明  ：向下匹配到的标准分辨率
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
05/21/2010              周嘉麟         创建
====================================================================*/
u8 GetMinResAcdWHProduct(u8 bySrcRes, u8 byDstRes)
{
	if (VIDEO_FORMAT_INVALID == bySrcRes || 0 == bySrcRes ||
		VIDEO_FORMAT_INVALID == byDstRes || 0 == byDstRes)
	{
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetMinResAcdWHProduct] bySrcRes %d or byDstRes %d is unexpected!\n",
			bySrcRes, byDstRes) ;
		return VIDEO_FORMAT_INVALID;
	}
	
	u16 wSrcWidth  = 0;
	u16 wSrcHeight = 0;
	u16 wDstWidth  = 0;
	u16 wDstHeight = 0;
	
	//获取源分辨率
	GetWHByRes(bySrcRes, wSrcWidth, wSrcHeight);
	if (wSrcWidth == 0 || wSrcHeight == 0)
	{
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetMinResAcdWHProduct]wSrcWidth or wSrcHeight = 0!\n");
		return VIDEO_FORMAT_INVALID;
	}
	
	//获取目的分辨率
	GetWHByRes(byDstRes, wDstWidth, wDstHeight);
	if (wDstWidth == 0 || wDstHeight == 0)
	{
		LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetMinResAcdWHProduct]wDstWidth or wDstHeight = 0!\n");
		return VIDEO_FORMAT_INVALID;
	}
	
	if ((wSrcWidth * wSrcHeight) >= (wDstWidth * wDstHeight))
	{
		return byDstRes;
	}
	else
	{
		return bySrcRes;
	} 
}

/*====================================================================
函数名      : GetNormalRes
功能        : 根据宽高,获得一个通用的分辨率,调分辨率用
算法实现    ：
引用全局变量：
输入参数说明：u16 wResW 宽,u16 wResH 高
返回值说明  ：v4r7前mcu支持的分辨率
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
09/13/2012              yanghuaizhi     创建
====================================================================*/
u8 GetNormalRes(u16 wResW, u16 wResH)
{
	// 支持调分辨率的通用分辨率列表,旧mcu支持
	u8 abyAcceptableRes[MAXNUM_ACCEPTABLE_RES] = 
	{
		VIDEO_FORMAT_HD1080,
		VIDEO_FORMAT_1440x816,
		VIDEO_FORMAT_HD720,
		VIDEO_FORMAT_960x544,
		VIDEO_FORMAT_720_864x480,	//TS Vmp 会需要该分辨率
		VIDEO_FORMAT_4CIF,
		VIDEO_FORMAT_4SIF,
		VIDEO_FORMAT_640x368,
		VIDEO_FORMAT_480x272,
		VIDEO_FORMAT_CIF,
		VIDEO_FORMAT_720_432x240,	// need by 8000G vmp 
		VIDEO_FORMAT_720_320x192,	// need by 8000G vmp
		VIDEO_FORMAT_QCIF,			//old vmp 可能会需要QCIF
		VIDEO_FORMAT_2CIF
	}; 

	u8 byExistRes = VIDEO_FORMAT_INVALID;	//已存在分辨率
	u8 bySuitableRes = VIDEO_FORMAT_INVALID;	//最接近的分辨率,宽高乘积小于等于此分辨率的能力
	u16 wWidth = 0;
	u16 wHeight = 0;
	for(u8 byLoop = 0; byLoop < MAXNUM_ACCEPTABLE_RES; byLoop ++)
	{
		if( abyAcceptableRes[byLoop] == 0 )
		{
			break;
		}
		GetWHByRes( abyAcceptableRes[byLoop], wWidth, wHeight);
		// 在列表中有定义,直接使用
		if (WHEQUAL == WHCmp (wWidth, wHeight, wResW, wResH))
		{
			byExistRes = abyAcceptableRes[byLoop];
			break;
		}
		// 宽高成绩小于此分辨率,存入最接近的那个能力
		if (wWidth * wHeight < wResW * wResH)
		{
			if (VIDEO_FORMAT_INVALID == bySuitableRes)
			{
				bySuitableRes = abyAcceptableRes[byLoop];
			}
		}
	}

	// 有存在的分辨率,返回存在的,若无,返回合适的
	if (VIDEO_FORMAT_INVALID != byExistRes)
	{
		return byExistRes;
	}
	else
	{
		return bySuitableRes;
	}
}


/*====================================================================
函数名      : GetSutiableRes
功能        : 根据宽高,获得相应分辨率。如果该宽高MCU不支持，向下获得最接近的分辨率
算法实现    ：
引用全局变量：
输入参数说明：u16 wResW 宽,u16 wResH 高
返回值说明  ：mcu支持的分辨率
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
09/17/2012              chendaiwei     创建
====================================================================*/
u8 GetMcuSupportedRes(u16 wResW, u16 wResH)
{
	u8 abyAcceptableRes[MAXNUM_ACCEPTABLE_RES] = 
	{
		VIDEO_FORMAT_HD1080,
		VIDEO_FORMAT_1440x816,
		VIDEO_FORMAT_HD720,
		VIDEO_FORMAT_960x544,
		VIDEO_FORMAT_720_864x480,	//TS Vmp 会需要该分辨率
		VIDEO_FORMAT_4CIF,
		VIDEO_FORMAT_4SIF,
		VIDEO_FORMAT_640x544,
		VIDEO_FORMAT_640x368,
		VIDEO_FORMAT_480x272,
		VIDEO_FORMAT_384x272,		//mpu2 20风格会需要384x272
		VIDEO_FORMAT_CIF,
		VIDEO_FORMAT_720_432x240,	// need by 8000G vmp 
		VIDEO_FORMAT_720_320x192,	// need by 8000G vmp
		VIDEO_FORMAT_320x272,
		VIDEO_FORMAT_QCIF,			//old vmp 可能会需要QCIF
		VIDEO_FORMAT_2CIF
	}; 

	u8 byExistRes = VIDEO_FORMAT_INVALID;	//已存在分辨率
	u8 bySuitableRes = VIDEO_FORMAT_INVALID;	//最接近的分辨率,宽高乘积小于等于此分辨率的能力
	u16 wWidth = 0;
	u16 wHeight = 0;
	for(u8 byLoop = 0; byLoop < MAXNUM_ACCEPTABLE_RES; byLoop ++)
	{
		if( abyAcceptableRes[byLoop] == 0 )
		{
			break;
		}
		GetWHByRes( abyAcceptableRes[byLoop], wWidth, wHeight);
		// 在列表中有定义,直接使用
		if (WHEQUAL == WHCmp (wWidth, wHeight, wResW, wResH))
		{
			byExistRes = abyAcceptableRes[byLoop];
			break;
		}
		// 宽高成绩小于此分辨率,存入最接近的那个能力
		if (wWidth * wHeight < wResW * wResH)
		{
			if (VIDEO_FORMAT_INVALID == bySuitableRes)
			{
				bySuitableRes = abyAcceptableRes[byLoop];
			}
		}
	}

	// 有存在的分辨率,返回存在的,若无,返回合适的
	if (VIDEO_FORMAT_INVALID != byExistRes)
	{
		return byExistRes;
	}
	else
	{
		return bySuitableRes;
	}
}



/*====================================================================
函数名      : GetBasNeedResDataByCap
功能        : 通过能力获得编解码所需数据
算法实现    ：
引用全局变量：
输入参数说明：const TSimCapSet &tSimCapSet  能力集
			  TNeedBasResData &tBasResData  所需BAS资源
返回值说明  ：成功返回TRUE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2011/12/07  4.7         倪志俊         创建
====================================================================*/
BOOL32	GetBasNeedResDataByCap(const TSimCapSet &tSrcCap,const TSimCapSet &tDstCap, u8 byMediaMode, TNeedVidAdaptData &tBasResData)
{
	if (MODE_AUDIO == byMediaMode)
	{
		tBasResData.m_byMediaMode = MODE_AUDIO;
		tBasResData.m_byRealEncNum = 1;
		tBasResData.m_byDecNeedRes = (u8)1 <<CBasChn::emDecAud;
	}
	else if (MODE_VIDEO == byMediaMode || MODE_SECVIDEO == byMediaMode)
	{
		tBasResData.m_byMediaMode = byMediaMode;
		tBasResData.m_byRealEncNum = 1;
		
		//获得解码能力
		if ( tSrcCap.GetVideoMediaType() == MEDIA_TYPE_H261 )
		{
			tBasResData.m_byDecNeedRes = (u8)1 << CBasChn::emDecH261;
		}
		else if ( tSrcCap.GetVideoCap().IsSupportHP() )
		{
			tBasResData.m_byDecNeedRes = (u8)1 << CBasChn::emDecHp;
		}
		else
		{
			u16 wSrcWidth  = 0;
			u16 wSrcHeight = 0;
			GetWHByRes(tSrcCap.GetVideoResolution(), wSrcWidth, wSrcHeight);
			u16 w720Width  = 0;
			u16 w720Height = 0;
			GetWHByRes(VIDEO_FORMAT_HD720, w720Width, w720Height);
			BOOL32 bDecHighFps = tSrcCap.GetUserDefFrameRate() >= 50 &&
				((wSrcWidth * wSrcHeight) > (w720Width * w720Height));
			//解码是否高清解码
			BOOL32 bDecHDRes   = MEDIA_TYPE_H264 == tSrcCap.GetVideoMediaType()
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
				&& IsResG(tSrcCap.GetVideoResolution(), VIDEO_FORMAT_4CIF);
#else
			&& IsResGE(tSrcCap.GetVideoResolution(), VIDEO_FORMAT_4CIF);
#endif
			if (bDecHighFps)
			{
				tBasResData.m_byDecNeedRes  = (u8)1 << CBasChn::emDecHighFps;
			}
			else if(bDecHDRes)
			{
				tBasResData.m_byDecNeedRes  = (u8)1 << CBasChn::emDecHD;
			}
			else
			{
				tBasResData.m_byDecNeedRes  = (u8)1 << CBasChn::emDecSD;
			}
		}
		
		//获得编码能力
		tBasResData.m_atSimCapSet[0] = tDstCap.GetVideoCap();
	}
	else
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[GetBasNeedResDataByCap]byMediaMode:%d is illegal!\n");
		return FALSE;
	}
	return TRUE;
}

/*====================================================================
函数名      : GetResourceUnitAcd2Cap
功能        : 根据格式、分辨率、帧率获得编解码是对应占用的资源单位
算法实现    ：
引用全局变量：
输入参数说明：u8 byMediaType  格式
			  u8 byRes		  分辨率
			  u8 byFrameRate  帧率
返回值说明  ：返回所需资源单位
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2011/12/07  4.7            倪志俊         创建
====================================================================*/
u16 GetResourceUnitAcd2Cap(u8 byMediaType, u8 byRes, u8 byFrameRate)
{
	u16 wRetVal = 0;
	if ( MEDIA_TYPE_H264 != byMediaType ||
		 VIDEO_FORMAT_INVALID == byRes  ||
		 0 == byFrameRate
		)
	{
		LogPrint( LOG_LVL_DETAIL, MID_PUB_ALWAYS, "[GetResourceUnitAcd2Cap]byMediaType:%d,byRes:%d,byFrameRate:%d is Error!\n",
				 byMediaType, byRes, byFrameRate
				);
		return INVALID_RESOUCEUNIT_VALUE;
	}

	switch (byRes)
	{
	case VIDEO_FORMAT_HD1080:
		{
			if ( byFrameRate == 60 || byFrameRate == 50 )
			{
				wRetVal = 1000;
			} 
			else if (byFrameRate ==30  || byFrameRate ==25 )
			{
				wRetVal = 500;
			}
			else
			{
				LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetResourceUnitAcd2Cap] byRes:%d,byFrameRate:%d is Error!\n",
						  byRes, byFrameRate
						);
				wRetVal = INVALID_RESOUCEUNIT_VALUE;
			}
		}
		break;
	case VIDEO_FORMAT_HD720:
		{
			if ( byFrameRate == 60 || byFrameRate == 50 )
			{
				wRetVal = 444;
			} 
			else if (byFrameRate ==30  || byFrameRate ==25 )
			{
				wRetVal = 222;
			}
			else
			{
				LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetResourceUnitAcd2Cap] byRes:%d,byFrameRate:%d is Error!\n",
					byRes, byFrameRate
					);
				wRetVal = INVALID_RESOUCEUNIT_VALUE;
			}
		}
		break;
	case VIDEO_FORMAT_4CIF:
		{
			wRetVal = 81;
		}
		break;
	case VIDEO_FORMAT_CIF:
		{
			wRetVal = 2;
		}
		break;
	case VIDEO_FORMAT_UXGA:
		{
			if ( byFrameRate == 60  )
			{
				wRetVal = 926;
			} 
			else if ( byFrameRate == 30 )
			{
				wRetVal = 463;
			}
			else if ( byFrameRate == 20 )
			{
				wRetVal = 309;
			}
			else if ( byFrameRate == 15 )
			{
				wRetVal = 232;
			}
			else if ( byFrameRate == 10 )
			{
				wRetVal = 154;
			}
			else if ( byFrameRate == 5 )
			{
				wRetVal = 77;
			}
			else
			{
				LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetResourceUnitAcd2Cap] byRes:%d,byFrameRate:%d is Error!\n",
					byRes, byFrameRate
					);
				wRetVal = INVALID_RESOUCEUNIT_VALUE;
			}
		}
		break;
	case VIDEO_FORMAT_SXGA:
		{
			if ( byFrameRate == 60  )
			{
				wRetVal = 632;
			} 
			else if ( byFrameRate== 30)
			{
				wRetVal = 316;
			}
			else if ( byFrameRate == 20)
			{
				wRetVal = 211;
			}
			else if ( byFrameRate == 15)
			{
				wRetVal = 158;
			}
			else if ( byFrameRate == 10)
			{
				wRetVal = 106;
			}
			else if ( byFrameRate ==5 )
			{
				wRetVal = 53;
			}
			else
			{
				LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetResourceUnitAcd2Cap] byRes:%d,byFrameRate:%d is Error!\n",
					byRes, byFrameRate
					);
				wRetVal = INVALID_RESOUCEUNIT_VALUE;
			}
		}
		break;
// 	case VIDEO_FORMAT_XGA:
// 		{
// 			if ( byFrameRate == 5  )
// 			{
// 				wRetVal = 32;
// 			} 
// 			else
// 			{
// 				LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetResourceUnitAcd2Cap] byRes:%d,byFrameRate:%d is Error!\n"
// 						 byRes, byFrameRate
// 						);
// 				wRetVal = INVALID_RESOUCEUNIT_VALUE;
// 			}
// 		}
// 		break;
	default:
		{
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetResourceUnitAcd2Cap] byRes:%d,byFrameRate:%d is Error!\n",
					byRes, byFrameRate
					);
			wRetVal = INVALID_RESOUCEUNIT_VALUE;
		}
		break;
	}

	return wRetVal;
}

/*====================================================================
函数名      : GetResourceUnitAcd2Cap
功能        : 根据TVideoStreamCap获得编解码是对应占用的资源单位
算法实现    ：
引用全局变量：
输入参数说明：const TVideoStreamCap &tSimCapSet 能力集
返回值说明  ：返回所需资源单位
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2011/12/07  4.7            倪志俊         创建
====================================================================*/
u16 GetResourceUnitAcd2Cap(const TVideoStreamCap &tSimCapSet)
{
	u8 byMediaType = tSimCapSet.GetMediaType();
	u8 byRes	   = tSimCapSet.GetResolution();
	u8 byFrameRate = tSimCapSet.GetUserDefFrameRate();

	return GetResourceUnitAcd2Cap(byMediaType, byRes, byFrameRate);
}

/*====================================================================
函数名      : GetResourceUnitAcd2Cap
功能        : 根据会议勾选idx获得对应资源单位
算法实现    ：
引用全局变量：
输入参数说明：u8 *pbyIdx 会议勾选idx数组指针
			  u8 byNum	 数组个数
返回值说明  ：返回所需资源单位，0XFFFF表示该能力暂时还不能获得对应所需资源单位
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2011/12/07  4.7            倪志俊         创建
====================================================================*/
u16 GetResourceUnitAcd2CapArray(TSimCapSet *tSimCapSet,u8 byNum)
{
	u16  wResUnit = 0;

	for ( u8 byIdx= 0; byIdx< byNum; byIdx++)
	{
		u16 wTempResUnit =  GetResourceUnitAcd2Cap( tSimCapSet[byIdx].GetVideoCap() );
		if ( wTempResUnit != INVALID_RESOUCEUNIT_VALUE )
		{
			wResUnit += wTempResUnit;
		}
	}

	return wResUnit;
}

/*====================================================================
函数名      : IsNeedAdjustCapBySrc
功能        : 如果源能力小于目的能力，那么按照源能力设置目的能力
算法实现    ：
引用全局变量：
输入参数说明：const TVideoStreamCap &tVidSrcCap 源能力
			  TVideoStreamCap &tVidDstCap 目的能力，如果需要按照源调，那么通过
			  该变量返回新的能力
			  u8 byMediaMode 模式
返回值说明  ：返回所需资源单位
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/04/23  4.7            倪志俊         创建
====================================================================*/
BOOL32 IsNeedAdjustCapBySrc(const TVideoStreamCap &tVidSrcCap,TVideoStreamCap &tVidDstCap)
{
	BOOL32 bIsAdjustBySrc = FALSE;
	//是否需要调帧率，目前调帧率只有目的是H264的情况
	if ( tVidDstCap.GetMediaType() == MEDIA_TYPE_H264 )
	{
		u8 byDstFrameRate = tVidDstCap.GetUserDefFrameRate();
		u8 bySrcFramRate =0;
		if ( tVidSrcCap.GetMediaType() != MEDIA_TYPE_H264 )
		{
			bySrcFramRate= FrameRateMac2Real(tVidSrcCap.GetFrameRate());
		}
		else
		{
			bySrcFramRate = tVidSrcCap.GetUserDefFrameRate();
		}

		if ( bySrcFramRate < byDstFrameRate )
		{
			tVidDstCap.SetUserDefFrameRate(bySrcFramRate);
			bIsAdjustBySrc= TRUE;
		}
	}

	//是否需要调分辨率
	u16 wSrcWidth=0;
	u16 wSrcHeight=0;
	if ( tVidSrcCap.GetMediaType() == MEDIA_TYPE_MP4 && tVidSrcCap.GetResolution() == VIDEO_FORMAT_AUTO )
	{
		GetWHByRes( GetAutoResByBitrate( tVidSrcCap.GetResolution(),tVidSrcCap.GetMaxBitRate() ),wSrcWidth,wSrcHeight );
		u8 byTempMpe4Res= GetResByWH(wSrcWidth,wSrcHeight);
		//如果源是MPE4 2CIF，调整成CIF来比较
		if ( byTempMpe4Res == VIDEO_FORMAT_2CIF )
		{
			GetWHByRes( VIDEO_FORMAT_CIF, wSrcWidth,wSrcHeight);
		}
	}
	else
	{
		GetWHByRes( tVidSrcCap.GetResolution(), wSrcWidth,wSrcHeight);
	}

	u16 wDstWidth=0;
	u16 wDstHeight=0;
	if ( tVidDstCap.GetMediaType() == MEDIA_TYPE_MP4 && tVidDstCap.GetResolution() == VIDEO_FORMAT_AUTO )
	{
		GetWHByRes( GetAutoResByBitrate( tVidDstCap.GetResolution(),tVidDstCap.GetMaxBitRate() ),wDstWidth,wDstHeight );
		u8 byTempMpe4Res= GetResByWH(wDstWidth,wDstHeight);
		//如果源是MPE4 2CIF，调整成CIF来比较
		if ( byTempMpe4Res == VIDEO_FORMAT_2CIF )
		{
			GetWHByRes( VIDEO_FORMAT_CIF, wDstWidth,wDstHeight);
		}
	}
	else
	{
		GetWHByRes( tVidDstCap.GetResolution(), wDstWidth,wDstHeight);
	}

	if ( wSrcWidth*wSrcHeight < wDstWidth*wDstHeight )
	{
		u8 byNewRes=GetResByWH(wSrcWidth,wSrcHeight);
		tVidDstCap.SetResolution(byNewRes);
		bIsAdjustBySrc = TRUE;
	}
	
	return bIsAdjustBySrc;
}

/*====================================================================
函数名      : ConstructMsgToVmp
功能        : 组织给画面合成器的参数
算法实现    ：
引用全局变量：
输入参数说明：CServMsg &cServMsg	[o]
返回值说明  ：void
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
02/16/2011              薛亮          创建
====================================================================*/
void CParamToOldVmp::ConstructMsgToVmp(CServMsg &cServMsg)
{
	CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(m_byConfIdx);
	if ( NULL == pVcInst )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ConstructMsgToVmp]ConfIdx.%d doesn't exist!\n", m_byConfIdx );
		return;
    }

	TConfInfo tConfInfo = pVcInst->m_tConf;
	cServMsg.SetConfId(tConfInfo.GetConfId());


	CKDVVMPParam cKDVVMPParam;
	memset(&cKDVVMPParam, 0, sizeof(cKDVVMPParam));

	//style
	cKDVVMPParam.m_byVMPStyle = m_tVmpCommonAttrb.m_byVmpStyle;
	
	//enctype
	cKDVVMPParam.m_byEncType = tConfInfo.GetMainVideoMediaType();

	//member number
	cKDVVMPParam.m_byMemberNum  = m_tVmpCommonAttrb.m_byMemberNum;

	//payload and encypt key
	memcpy(cKDVVMPParam.m_atMtMember, m_tVmpCommonAttrb.m_atMtMember, sizeof(cKDVVMPParam.m_atMtMember));
	memcpy(cKDVVMPParam.m_tVideoEncrypt, m_tVmpCommonAttrb.m_tVideoEncrypt, sizeof(cKDVVMPParam.m_tVideoEncrypt));
	memcpy(cKDVVMPParam.m_tDoublePayload, m_tVmpCommonAttrb.m_tDoublePayload, sizeof(cKDVVMPParam.m_tDoublePayload));

	//fps: no

	//resolution, BR info for 1st Encode Channel-----------------------
	u16 wMinMtReqBitrate = 0;
    if (0 != tConfInfo.GetSecBitRate() && 
        MEDIA_TYPE_NULL == tConfInfo.GetSecVideoMediaType())
    {
		//先判断双速单格式会议的情况：第一通道为会议码率，第二通道为非会议码率的最低码率（>=会议辅助码率）
        wMinMtReqBitrate = tConfInfo.GetBitRate();
    }
    else
    {
        wMinMtReqBitrate = pVcInst->GetLeastMtReqBitrate(TRUE, cKDVVMPParam.m_byEncType);
    }
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( m_byVmpId );
	g_cMcuVcApp.SetVMPOutChlBitrate(tVmpEqp, 0, wMinMtReqBitrate);
    
    cKDVVMPParam.m_wBitRate = htons(wMinMtReqBitrate);
	
    u16 wWidth = 0;
    u16 wHeight = 0;
    tConfInfo.GetVideoScale(cKDVVMPParam.m_byEncType,
		wWidth,
		wHeight);
    cKDVVMPParam.m_wVideoWidth = wWidth;
    cKDVVMPParam.m_wVideoHeight = wHeight;
	
    //现在约定16CIF即是2CIF,规则按照AUTO处理, xsl [8/11/2006] mpeg2 4cif按2cif处理
	u8 byRes = tConfInfo.GetVideoFormat(cKDVVMPParam.m_byEncType);
    if( VIDEO_FORMAT_16CIF == byRes ||
        (MEDIA_TYPE_H262 == cKDVVMPParam.m_byEncType && VIDEO_FORMAT_4CIF == byRes))
    {
        cKDVVMPParam.m_wVideoWidth  = 352;
        cKDVVMPParam.m_wVideoHeight = 576;
    }
    
	cKDVVMPParam.m_wVideoWidth  = htons(cKDVVMPParam.m_wVideoWidth);
	cKDVVMPParam.m_wVideoHeight = htons(cKDVVMPParam.m_wVideoHeight);

	cServMsg.SetMsgBody((u8*)&cKDVVMPParam, sizeof(cKDVVMPParam));

	// resolution, BR info for 2nd Encode Channel if any-----------------------
    if (0 != tConfInfo.GetSecBitRate() && 
        MEDIA_TYPE_NULL == tConfInfo.GetSecVideoMediaType())
    {
		//双速单格式会议时第二通道填主媒体格式
        cKDVVMPParam.m_byEncType = tConfInfo.GetMainVideoMediaType();
    }
    else
    {
        cKDVVMPParam.m_byEncType = tConfInfo.GetSecVideoMediaType();
        
        //xsl [8/11/2006] mpeg2 4cif按2cif处理
        tConfInfo.GetVideoScale(cKDVVMPParam.m_byEncType, wWidth, wHeight);
        cKDVVMPParam.m_wVideoWidth = wWidth;
        cKDVVMPParam.m_wVideoHeight = wHeight;
        byRes = tConfInfo.GetVideoFormat(cKDVVMPParam.m_byEncType);
        if( VIDEO_FORMAT_16CIF == byRes ||
            (MEDIA_TYPE_H262 == cKDVVMPParam.m_byEncType && VIDEO_FORMAT_4CIF == byRes))
        {
            cKDVVMPParam.m_wVideoWidth  = 352;
            cKDVVMPParam.m_wVideoHeight = 576;
        }        
        cKDVVMPParam.m_wVideoWidth  = htons(cKDVVMPParam.m_wVideoWidth);
        cKDVVMPParam.m_wVideoHeight = htons(cKDVVMPParam.m_wVideoHeight);
    }
    
    //modify bas 2
    // 单速单格式会议，码率清0, zgc, 2008-03-19
    if ( MEDIA_TYPE_NULL == cKDVVMPParam.m_byEncType )
    {
        wMinMtReqBitrate = 0;
    }
    else
    {
        wMinMtReqBitrate = pVcInst->GetLeastMtReqBitrate(TRUE, cKDVVMPParam.m_byEncType);
//         if (wMinMtReqBitrate < m_wBasBitrate || 0 == m_wBasBitrate)
//         {
//             m_wBasBitrate = wMinMtReqBitrate;
//         }
    }
    cKDVVMPParam.m_wBitRate = htons(wMinMtReqBitrate);
	
	cServMsg.CatMsgBody((u8 *)&cKDVVMPParam, sizeof(cKDVVMPParam));


	//need PRS
	u8 byNeedPrs = m_tVmpCommonAttrb.m_byNeedPrs;
	cServMsg.CatMsgBody((u8 *)&byNeedPrs, sizeof(byNeedPrs));

	//画面合成风格
    TVmpStyleCfgInfo tMcuVmpStyle = m_tVmpCommonAttrb.m_tVmpStyleCfgInfo;    
    cServMsg.CatMsgBody((u8*)&tMcuVmpStyle, sizeof(tMcuVmpStyle));

}

/*====================================================================
函数名      : ConstructMsgToVmp
功能        : 组织给画面合成器的参数
算法实现    ：
引用全局变量：
输入参数说明：CServMsg &cServMsg	[o]
返回值说明  ：void
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2011-12-1              yanghuaizhi       创建
====================================================================*/
void CParamToKDVVmp::ConstructMsgToVmp(CServMsg &cServMsg)
{
	CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(m_byConfIdx);
	if ( NULL == pVcInst )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ConstructMsgToVmp]ConfIdx.%d doesn't exist!\n", m_byConfIdx );
		return;
    }
	
	TConfInfo tConfInfo = pVcInst->m_tConf;
	cServMsg.SetConfId(tConfInfo.GetConfId());
	// 设置画面合成参数通用部分
	// 按照mpu2格式存储通用参数
	TMPU2CommonAttrb tMpu2CommonAttrb;
	tMpu2CommonAttrb.m_byVmpStyle	= m_tVmpCommonAttrb.m_byVmpStyle;	//合成风格
	tMpu2CommonAttrb.m_byMemberNum	= m_tVmpCommonAttrb.m_byMemberNum;	//成员数量
	tMpu2CommonAttrb.m_byNeedPrs	= m_tVmpCommonAttrb.m_byNeedPrs;	//是否需要PRS
	memcpy(tMpu2CommonAttrb.m_atMtMember, m_tVmpCommonAttrb.m_atMtMember, sizeof(TVMPMemberEx)*MAXNUM_MPU2VMP_MEMBER);
	memcpy(tMpu2CommonAttrb.m_tVideoEncrypt, m_tVmpCommonAttrb.m_tVideoEncrypt, sizeof(TMediaEncrypt)*MAXNUM_MPU2VMP_MEMBER);
	memcpy(tMpu2CommonAttrb.m_tDoublePayload, m_tVmpCommonAttrb.m_tDoublePayload, sizeof(TDoublePayload)*MAXNUM_MPU2VMP_MEMBER);
	tMpu2CommonAttrb.m_tVmpStyleCfgInfo = m_tVmpCommonAttrb.m_tVmpStyleCfgInfo;		//配置信息
	tMpu2CommonAttrb.m_tVmpExCfgInfo = m_tVmpCommonAttrb.m_tVmpExCfgInfo;		//扩展配置信息
	// 设置通用信息
	cServMsg.SetMsgBody((u8*)&tMpu2CommonAttrb, sizeof(tMpu2CommonAttrb));

	CKDVVMPOutMember cVMPOutMember;
	u8 byChnnlRes  = 0;
	u16 wWidth		= 0;	//宽
	u16 wHeight		= 0;	//高
	u16 wMinMtReqBitrate = 0;
	u8 byFrameRate = 0;
	TVideoStreamCap tStrCap;
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( m_byVmpId );
	for (u8 byChnIdx=0; byChnIdx<MAXNUM_MPU2_OUTCHNNL; byChnIdx++)
	{
		cVMPOutMember.Clear();
		// 获得视频能力
		tStrCap = m_tVMPOutParam.GetVmpOutCapIdx(byChnIdx);
		if (MEDIA_TYPE_NULL != tStrCap.GetMediaType())
		{
			//获得Idx对应能力集
			cVMPOutMember.SetEncType(tStrCap.GetMediaType());
			byChnnlRes = tStrCap.GetResolution();
			wMinMtReqBitrate = pVcInst->GetMinMtRcvBitByVmpChn(m_byVmpId, TRUE,byChnIdx);
			//mp4_auto 按照码率编对应分辨率
			if ( MEDIA_TYPE_MP4 == tStrCap.GetMediaType()
				&& VIDEO_FORMAT_AUTO == byChnnlRes )
			{
				if (wMinMtReqBitrate > 3072)
				{
					byChnnlRes = VIDEO_FORMAT_4CIF;
				} 
				else if(wMinMtReqBitrate > 1536)
				{
					byChnnlRes = VIDEO_FORMAT_2CIF;
				}
				else
				{
					byChnnlRes = VIDEO_FORMAT_CIF;
				}
			}
			// mp4 16cif 下参为mpeg4 4cif
			else if (MEDIA_TYPE_MP4 == tStrCap.GetMediaType()
				&& VIDEO_FORMAT_16CIF == byChnnlRes )
			{
				byChnnlRes = VIDEO_FORMAT_4CIF;
			}
			// h264 auto 下参为cif
			else if (MEDIA_TYPE_H264 == tStrCap.GetMediaType()
				&& VIDEO_FORMAT_AUTO == byChnnlRes )
			{
				byChnnlRes = VIDEO_FORMAT_CIF;
			}
			//根据能力集获得分辨率
			GetWHByRes(byChnnlRes , wWidth, wHeight);
			// 对MP4和4cif 严格区分给宽高
			if( MEDIA_TYPE_MP4 == tStrCap.GetMediaType() && VIDEO_FORMAT_4CIF == tStrCap.GetResolution())
			{
				wWidth  = 720;
				wHeight = 576;
			}

			//fps:
			byFrameRate = tStrCap.GetUserDefFrameRate();
			if( tStrCap.GetMediaType() == MEDIA_TYPE_H264)
			{
				//h264: the value is real, so maintain the value
			}
			else
			{
				byFrameRate = FrameRateMac2Real(byFrameRate);
			}

			cVMPOutMember.SetVideoWidth(wWidth);
			cVMPOutMember.SetVideoHeight(wHeight);
			cVMPOutMember.SetProfileType(tStrCap.GetH264ProfileAttrb());
			cVMPOutMember.SetFrameRate(byFrameRate);
			g_cMcuVcApp.SetVMPOutChlBitrate(tVmpEqp, byChnIdx, wMinMtReqBitrate);
			cVMPOutMember.SetBitRate(wMinMtReqBitrate);
		}
		// 设置VMP后适配参数
		cServMsg.CatMsgBody((u8*)&cVMPOutMember, sizeof(cVMPOutMember));
	}

    // 追加消息 [7/4/2013 liaokang]
    // 结构：u8 （type num）+ 
    //       u8 （type）    +  u16 （type length，net order） + content + …
    u8  byTypeNum = 1;
    cServMsg.CatMsgBody(&byTypeNum, sizeof(byTypeNum));

    // 追加Vmp成员别名 [7/4/2013 liaokang] type + type length + 【  ChnNum + ( ChnNo + Len + content + …)  】
    s8  achMbAlias[1 + 2 + 1 + (1 + 1 + MAXLEN_ALIAS)*MAXNUM_MPU2VMP_MEMBER] = {0};
    u8 *pchMbAlias = (u8*)achMbAlias;
    u8 byChnNum = 0;
    u16 wTypeLen = (1 + 2 + 1);
    pchMbAlias += wTypeLen; 
    s8  achAlias[MAXLEN_ALIAS] = {0}; 
    u8 byAliasLen = 0;
    for(u8 byChnNo=0; byChnNo<m_tVmpCommonAttrb.m_byMemberNum; byChnNo++)
    {
        if(m_tVmpCommonAttrb.m_atMtMember[byChnNo].IsNull())
        {
            continue;
        }

        memset(achAlias,0,sizeof(achAlias));
        if( !( pVcInst->GetMtAliasToVmp(m_tVmpCommonAttrb.m_atMtMember[byChnNo],achAlias) ) )
        {
            continue;
        }
        byChnNum++;
        byAliasLen = strlen(achAlias);
        *pchMbAlias = byChnNo;
        pchMbAlias++;
        *pchMbAlias = byAliasLen;
        pchMbAlias++;
        memcpy(pchMbAlias,achAlias,byAliasLen);
        pchMbAlias += byAliasLen;
        wTypeLen += (2 + byAliasLen);
    }
    pchMbAlias -= wTypeLen;
    *pchMbAlias = (u8)emVmpMbAlias;
    pchMbAlias++;
    *(u16*)pchMbAlias = htons(wTypeLen - 1 - 2);
    pchMbAlias += 2;
    *pchMbAlias = byChnNum;
    cServMsg.CatMsgBody((u8*)&achMbAlias[0], wTypeLen);
}

/*====================================================================
函数名      : ConstructMsgToVmp
功能        : 组织给画面合成器的参数
算法实现    ：
引用全局变量：
输入参数说明：CServMsg &cServMsg	[o]
返回值说明  ：void
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
02/16/2011              薛亮          创建
====================================================================*/
void CParamToMpuVmp::ConstructMsgToVmp(CServMsg &cServMsg)
{
	CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(m_byConfIdx);
	if ( NULL == pVcInst )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ConstructMsgToVmp]ConfIdx.%d doesn't exist!\n", m_byConfIdx );
		return;
    }

	TConfInfo tConfInfo = pVcInst->m_tConf;
	cServMsg.SetConfId(tConfInfo.GetConfId());


	CKDVNewVMPParam cKDVVMPParam;
	memset(&cKDVVMPParam, 0, sizeof(cKDVVMPParam));

	//style
	cKDVVMPParam.m_byVMPStyle = m_tVmpCommonAttrb.m_byVmpStyle;
	
	//enctype
	cKDVVMPParam.m_byEncType = tConfInfo.GetMainVideoMediaType();

	//member number
	cKDVVMPParam.m_byMemberNum  = m_tVmpCommonAttrb.m_byMemberNum;

	//将合成成员更新到TPeriEqpStatus中//is there need

	memcpy(cKDVVMPParam.m_atMtMember, m_tVmpCommonAttrb.m_atMtMember, sizeof(cKDVVMPParam.m_atMtMember));

	//payload and encypt key
	memcpy(cKDVVMPParam.m_tVideoEncrypt, m_tVmpCommonAttrb.m_tVideoEncrypt, sizeof(cKDVVMPParam.m_tVideoEncrypt));
	memcpy(cKDVVMPParam.m_tDoublePayload, m_tVmpCommonAttrb.m_tDoublePayload, sizeof(cKDVVMPParam.m_tDoublePayload));

	//fps:
	u8 byFrameRate = tConfInfo.GetMainVidUsrDefFPS();
	if( tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_H264)
	{
		//h264: the value is real, so maintain the value
	}
	else
	{
		byFrameRate = FrameRateMac2Real(byFrameRate);
	}
	cKDVVMPParam.SetFrameRate(byFrameRate);


	//resolution, BR info
	u16 wMinMtReqBitrate = 0;
	u16 wWidth = 0;
	u16 wHeight = 0;
	u8 byChnnlRes  = 0;
	u8 byMediaType = 0;
	u8 byVmpOutChnnl = 0;
	u8 byFRate = 0;
	u8 byAttrb = emBpAttrb;		//临时参数,仅用于传入
	TPeriEqpStatus tVmpStatus;	//临时参数,仅用于传入
	g_cMcuVcApp.GetPeriEqpStatus(m_byVmpId, &tVmpStatus);
    //u8 byVmpSubType = tVmpStatus.m_tStatus.tVmp.m_bySubType;	
	u8 byBoardVer	= tVmpStatus.m_tStatus.tVmp.m_byBoardVer;
	
	for (byVmpOutChnnl = 0; byVmpOutChnnl < MAXNUM_MPU_OUTCHNNL; byVmpOutChnnl++)
	{
		
		if (byBoardVer == MPU_BOARD_A128 && byVmpOutChnnl == 3) //A板只填3路
		{
			break;
		}
		byChnnlRes = pVcInst->GetResByVmpOutChnnl(m_byVmpId, byMediaType, byFRate, byAttrb, byVmpOutChnnl);
		
		if (VIDEO_FORMAT_INVALID == byChnnlRes)
		{
			cKDVVMPParam.m_byEncType = MEDIA_TYPE_NULL;
			cKDVVMPParam.SetBitRate(0);
			cKDVVMPParam.SetVideoWidth(0);
			cKDVVMPParam.SetVideoHeight(0);
		}
		else
		{
			wMinMtReqBitrate = pVcInst->GetMinMtRcvBitByVmpChn(m_byVmpId, TRUE,byVmpOutChnnl);//songkun,20110530
			//mp4_auto 按照码率编对应分辨率
			if ( MEDIA_TYPE_MP4 == byMediaType
				&& VIDEO_FORMAT_AUTO == byChnnlRes )
			{
				if (wMinMtReqBitrate > 3072)
				{
					byChnnlRes = VIDEO_FORMAT_4CIF;
				} 
				else if(wMinMtReqBitrate > 1536)
				{
					byChnnlRes = VIDEO_FORMAT_2CIF;
				}
				else
				{
					byChnnlRes = VIDEO_FORMAT_CIF;
				}
			}
			GetWHByRes(byChnnlRes, wWidth, wHeight);
			
			// [pengjie 2010/1/20] Modify 对MP4和4cif 严格区分给宽高
			if( VIDEO_FORMAT_4CIF == byChnnlRes && MEDIA_TYPE_MP4 == byMediaType)
			{
				wWidth  = 720;
				wHeight = 576;
			}
			// End Modify	
			
			cKDVVMPParam.SetVideoWidth(wWidth);
			cKDVVMPParam.SetVideoHeight(wHeight);
			cKDVVMPParam.m_byEncType = byMediaType;
			//	wMinMtReqBitrate = pVcInst->GetLstRcvMediaResMtBr(TRUE, byMediaType, byChnnlRes);
			g_cMcuVcApp.SetVMPOutChlBitrate((TEqp)tVmpStatus, byVmpOutChnnl, wMinMtReqBitrate);
			cKDVVMPParam.SetBitRate(wMinMtReqBitrate);
			
			//zbq[07/27/2009] 是否强制编码输出1080i, 放弃720p
			if (g_cMcuVcApp.IsSVmpOutput1080i())
			{
				if (VIDEO_FORMAT_HD1080 == byChnnlRes)
				{
					cKDVVMPParam.SetVideoHeight(544);
				}
				else if (VIDEO_FORMAT_HD720 == byChnnlRes)
				{
					cKDVVMPParam.SetBitRate(0);
				}
			}
			
			// xliang [7/28/2009] 会议如果没有高清能力，则把SVMP当EVPU用
			if( VIDEO_FORMAT_HD1080 != tConfInfo.GetMainVideoFormat() 
				&& VIDEO_FORMAT_HD720 != tConfInfo.GetMainVideoFormat() )
			{
				if( VIDEO_FORMAT_HD1080 == byChnnlRes || VIDEO_FORMAT_HD720 == byChnnlRes )
				{
					cKDVVMPParam.SetBitRate(0);
				}
			}
		}
		LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "[ConstructMsgToVmp]set BR.%d for chn.%d\n", 
			cKDVVMPParam.GetBitRate(), byVmpOutChnnl);
		cServMsg.CatMsgBody((u8 *)&cKDVVMPParam, sizeof(cKDVVMPParam));
	}

	//need PRS
	u8 byNeedPrs = m_tVmpCommonAttrb.m_byNeedPrs;
	cServMsg.CatMsgBody((u8 *)&byNeedPrs, sizeof(byNeedPrs));

	//画面合成风格
    TVmpStyleCfgInfo tMcuVmpStyle = m_tVmpCommonAttrb.m_tVmpStyleCfgInfo;    
    cServMsg.CatMsgBody((u8*)&tMcuVmpStyle, sizeof(tMcuVmpStyle));

	//设置画面合成空闲通道的背景
	TEqpExCfgInfo tEqpExCfgInfo;
	if( SUCCESS_AGENT != g_cMcuAgent.GetEqpExCfgInfo( tEqpExCfgInfo ) )
	{
		tEqpExCfgInfo.Init();
	}
	
	TVMPExCfgInfo tVMPExCfgInfo;
	tVMPExCfgInfo = tEqpExCfgInfo.m_tVMPExCfgInfo;
    LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP,  "Set Vmp IdleChlShowMode: %d \n", tVMPExCfgInfo.m_byIdleChlShowMode );	
	cServMsg.CatMsgBody( (u8 *)&tVMPExCfgInfo, sizeof(TVMPExCfgInfo) );

}


/*====================================================================
函数名      : ConstructMsgToVmp
功能        : 组织给画面合成器的参数
算法实现    ：
引用全局变量：
输入参数说明：CServMsg &cServMsg	[o]
返回值说明  ：void
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
02/16/2011              薛亮          创建
====================================================================*/
void CParamTo8kg8khVmp::ConstructMsgToVmp(CServMsg &cServMsg)
{
	CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(m_byConfIdx);
	if ( NULL == pVcInst )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ConstructMsgToVmp]ConfIdx.%d doesn't exist!\n", m_byConfIdx );
		return;
    }

	TConfInfo tConfInfo = pVcInst->m_tConf;
	cServMsg.SetConfId(tConfInfo.GetConfId());
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( m_byVmpId, &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;

	C8KEVMPParam cKDVVMPParam;
	memset(&cKDVVMPParam, 0, sizeof(cKDVVMPParam));

	//style
	cKDVVMPParam.m_byVMPStyle = m_tVmpCommonAttrb.m_byVmpStyle;
	
	//enctype
	cKDVVMPParam.m_byEncType = tConfInfo.GetMainVideoMediaType();

	//member number
	cKDVVMPParam.m_byMemberNum  = m_tVmpCommonAttrb.m_byMemberNum;

	memcpy(cKDVVMPParam.m_atMtMember, m_tVmpCommonAttrb.m_atMtMember, sizeof(cKDVVMPParam.m_atMtMember));

	//payload and encypt key
	memcpy(cKDVVMPParam.m_tVideoEncrypt, m_tVmpCommonAttrb.m_tVideoEncrypt, sizeof(cKDVVMPParam.m_tVideoEncrypt));
	memcpy(cKDVVMPParam.m_tDoublePayload, m_tVmpCommonAttrb.m_tDoublePayload, sizeof(cKDVVMPParam.m_tDoublePayload));

	//获取画面合成空闲通道的背景
	TEqpExCfgInfo tEqpExCfgInfo;
	if( SUCCESS_AGENT != g_cMcuAgent.GetEqpExCfgInfo( tEqpExCfgInfo ) )
	{
		tEqpExCfgInfo.Init();
	}
	
	TVMPExCfgInfo tVMPExCfgInfo;
	tVMPExCfgInfo = tEqpExCfgInfo.m_tVMPExCfgInfo;
	
	//设置合成图像是否叠加合成成员别名
    cKDVVMPParam.m_byIsDisplayMmbAlias = tVMPExCfgInfo.m_byIsDisplayMmbAlias;	
	
	LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP,  "[ConstructMsgToVmp]Vmp IdleChlShowMode: %d, Vmp IsDisplayMmbAlias: %d \n", 
		tVMPExCfgInfo.m_byIdleChlShowMode,
		tVMPExCfgInfo.m_byIsDisplayMmbAlias);

	//fps: no by now

	//resolution, BR info
	u16 wMinMtReqBitrate = 0;
	u16 wWidth = 0;
	u16 wHeight = 0;
	u8 byChnnlRes  = 0;
	u8 byMediaType = 0;
	u8 byVmpOutChnnl = 0;
	TPeriEqpStatus tVmpStatus;
	g_cMcuVcApp.GetPeriEqpStatus(m_byVmpId, &tVmpStatus);
    //u8 byVmpSubType = tVmpStatus.m_tStatus.tVmp.m_bySubType;	
	//u8 byBoardVer	= tVmpStatus.m_tStatus.tVmp.m_byBoardVer;
	
	u8 byMaxOutputChnnlNum = MAXNUM_MPU_OUTCHNNL;
	u8 abyChnlResEx[MAXNUM_MPU_OUTCHNNL] = { 0 };	//通道对应的分辨率是否勾选了
	u8 abyOutChnlActive[MAXNUM_MPU_OUTCHNNL];		//输出通道是否Active,1:Active,0:非Active
	u8 abyOutChnlAttrb[MAXNUM_MPU_OUTCHNNL];		//8kh输出通道画质属性(HP/BP),emBpAttrb/emHpAttrb
	memset(abyOutChnlActive,emVmpOutChnlNotChange,sizeof(abyOutChnlActive));
	memset(abyOutChnlAttrb, emBpAttrb, sizeof(abyOutChnlAttrb));//默认emBpAttrb
	if (VMP_8KH == byVmpSubType)
	{
		if ( pVcInst->IsMSSupportCapEx(VIDEO_FORMAT_HD720) )
		{
			abyChnlResEx[0] = 1;//1080
			abyChnlResEx[1] = 1;//720
		}
		if ( pVcInst->IsMSSupportCapEx(VIDEO_FORMAT_4CIF) )
		{
			abyChnlResEx[2] = 1;//4cif
		}
		if ( pVcInst->IsMSSupportCapEx(VIDEO_FORMAT_CIF) )
		{
			abyChnlResEx[3] = 1;//cif
		}
		if( MEDIA_TYPE_H264 == tConfInfo.GetMainVideoMediaType() ) 
		{
			if (VIDEO_FORMAT_HD1080 == tConfInfo.GetMainVideoFormat() ) //主格式1080
			{
				abyChnlResEx[0] = 1;//1080
				// 1080p60fps会议时,第1路可能出720p
				if (tConfInfo.GetMainVidUsrDefFPS() >= 50)
				{
					abyChnlResEx[1] = 1;//720
				}
			}
			if (VIDEO_FORMAT_HD720 == tConfInfo.GetMainVideoFormat() ) //主格式720
			{
				abyChnlResEx[1] = 1;//720
				// 720HP会议时,第0路出HP
				if (emHpAttrb == tConfInfo.GetProfileAttrb())
				{
					abyChnlResEx[0] = 1;//720 HP
				}
			}
			if (VIDEO_FORMAT_4CIF == tConfInfo.GetMainVideoFormat() ) //主格式4cif
			{
				abyChnlResEx[2] = 1;//4cif
			}
			if (VIDEO_FORMAT_CIF == tConfInfo.GetMainVideoFormat() ) //主格式cif
			{
				abyChnlResEx[3] = 1;//cif
			}
		}
		else if( MEDIA_TYPE_H264 == tConfInfo.GetSecVideoMediaType() ) //辅格式是h264
		{
			
			if (VIDEO_FORMAT_HD1080 == tConfInfo.GetSecVideoFormat() ) //辅格式1080
			{
				abyChnlResEx[0] = 1;//1080
			}
			if (VIDEO_FORMAT_HD720 == tConfInfo.GetSecVideoFormat() ) //辅格式720
			{
				abyChnlResEx[1] = 1;//720
			}
			if (VIDEO_FORMAT_4CIF == tConfInfo.GetSecVideoFormat() ) //辅格式4cif
			{
				abyChnlResEx[2] = 1;//4cif
			}
			if (VIDEO_FORMAT_CIF == tConfInfo.GetSecVideoFormat() ) //辅格式cif
			{
				abyChnlResEx[3] = 1;//cif
			}		
		}
		abyChnlResEx[MAXNUM_MPU_OUTCHNNL-1] = 1;//other肯定出

		// 1080p30fps适配会议vmp非广播时,vmp后适配1080p30要非Active
		if (pVcInst->IsConfExcludeDDSUseAdapt() && !tPeriEqpStatus.m_tStatus.tVmp.GetVmpParam().IsVMPBrdst())
		{
			//判断主格式
			if (tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
				tConfInfo.GetMainVideoFormat() == VIDEO_FORMAT_HD1080 &&
				tConfInfo.GetMainVidUsrDefFPS() >= 25 &&
				tConfInfo.GetMainVidUsrDefFPS() <= 30)
			{
				//vmp后适配第0路1080p30要非Active
				abyOutChnlActive[0] = emVmpOutChnlInactive;
			}
			if (tConfInfo.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
				tConfInfo.GetMainVideoFormat() == VIDEO_FORMAT_HD720 &&
				tConfInfo.GetMainVidUsrDefFPS() >= 25 &&
				tConfInfo.GetMainVidUsrDefFPS() <= 30 &&
				tConfInfo.GetProfileAttrb() == emHpAttrb)
			{
				//vmp后适配第0路720p30HP要非Active
				abyOutChnlActive[0] = emVmpOutChnlInactive;
			}
			//判断辅格式
			if (tConfInfo.GetSecVideoMediaType() == MEDIA_TYPE_H264 &&
				tConfInfo.GetSecVideoFormat() == VIDEO_FORMAT_HD1080 &&
				tConfInfo.GetSecVidUsrDefFPS() >= 25 &&
				tConfInfo.GetSecVidUsrDefFPS() <= 30)
			{
				//vmp后适配第0路1080p30要非Active
				abyOutChnlActive[0] = emVmpOutChnlInactive;
			}
		}
	}

	u8 byFrameRate = 0;
	for (byVmpOutChnnl = 0; byVmpOutChnnl < byMaxOutputChnnlNum; byVmpOutChnnl ++)
	{
		
		byChnnlRes = pVcInst->GetResByVmpOutChnnl(m_byVmpId, byMediaType, byFrameRate, abyOutChnlAttrb[byVmpOutChnnl], byVmpOutChnnl);
		//zhouyiliang20110228 H264 auto高码率是以MPEG4 16cif发过来的,此时下参应该用mpeg4 4cif
// 		if (MEDIA_TYPE_MP4 == byMediaType && VIDEO_FORMAT_16CIF == byChnnlRes ) 
// 		{
// 			byChnnlRes = VIDEO_FORMAT_4CIF;
// 		}
		if(VMP_8KH == byVmpSubType)
		{
			//会议帧率
			//byFrameRate = tConfInfo.GetMainVidUsrDefFPS();
			if( byMediaType == MEDIA_TYPE_H264)
			{
				//h264: the value is real, so maintain the value
			}
			else
			{
				byFrameRate = FrameRateMac2Real(byFrameRate);
			}
			
			//单格式会议 50/60的都要减半
			if (MEDIA_TYPE_H264 == tConfInfo.GetMainVideoMediaType()  && byFrameRate >= 50 ) 
			{
				byFrameRate = byFrameRate/2;
			}
			cKDVVMPParam.m_byFrameRate = byFrameRate;
			LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"Set outchnl:%d Vmp frame rate to %d\n", byVmpOutChnnl,byFrameRate);
			
			if ( !pVcInst->IsConfExcludeDDSUseAdapt() )
			{
				//8000H单格式会议只出一路,其余四路下空的参数
				if (byMediaType != tConfInfo.GetMainVideoMediaType() || byChnnlRes != tConfInfo.GetMainVideoFormat() )
				{
					if (emHpAttrb == tConfInfo.GetProfileAttrb() && 0 == byVmpOutChnnl)
					{
						// 单格式HP会议,第0路出720p/HP,分辨率不一致,不下空
					}
					else if (VIDEO_FORMAT_16CIF == tConfInfo.GetMainVideoFormat() && 4 == byVmpOutChnnl)
					{
						// 武警MPEG4 16CIF会议,第4路出MPEG4/4CIF,分辨率不一致,不下空
					} 
					else
					{
						byChnnlRes = VIDEO_FORMAT_INVALID;
					}
				}
			}
			else//适配会议720 50/60会议不出1080 那一路。其余情况勾了什么就出哪路，双other出两路
			{
				if ( MEDIA_TYPE_H264 == byMediaType && 1 != abyChnlResEx[byVmpOutChnnl])
				{
					byChnnlRes = VIDEO_FORMAT_INVALID;
				}
				
				if (MEDIA_TYPE_H264 != tConfInfo.GetMainVideoMediaType() &&  MEDIA_TYPE_NULL != tConfInfo.GetMainVideoMediaType() 
					&&  MEDIA_TYPE_H264 != tConfInfo.GetSecVideoMediaType() && MEDIA_TYPE_NULL != tConfInfo.GetSecVideoMediaType()) //双other
				{
					if (byMediaType != tConfInfo.GetMainVideoMediaType() && byMediaType != tConfInfo.GetSecVideoMediaType())
					{
						byChnnlRes = VIDEO_FORMAT_INVALID;
					}
				}
				
			}
		}
		if( byChnnlRes == VIDEO_FORMAT_INVALID)
		{
			cKDVVMPParam.m_byEncType = MEDIA_TYPE_NULL;
			cKDVVMPParam.m_wBitRate = 0;
			cKDVVMPParam.m_wVideoHeight = 0;
			cKDVVMPParam.m_wVideoWidth = 0;
		}
		else
		{
			//wMinMtReqBitrate = pVcInst->GetLstRcvMediaResMtBr(TRUE, byMediaType, byChnnlRes);
            wMinMtReqBitrate = pVcInst->GetMinMtRcvBitByVmpChn(m_byVmpId, TRUE,byVmpOutChnnl);//songkun,20110530
			//mp4_auto 按照码率编对应分辨率
			if ( MEDIA_TYPE_MP4 == byMediaType
				&& VIDEO_FORMAT_AUTO == byChnnlRes )
			{
				if (wMinMtReqBitrate > 3072)
				{
					byChnnlRes = VIDEO_FORMAT_4CIF;
				} 
				else if(wMinMtReqBitrate > 1536)
				{
					byChnnlRes = VIDEO_FORMAT_2CIF;
				}
				else
				{
					byChnnlRes = VIDEO_FORMAT_CIF;
				}
			}
			GetWHByRes(byChnnlRes, wWidth, wHeight);
			// 对MP4和4cif 严格区分给宽高
			if( VIDEO_FORMAT_4CIF == byChnnlRes && MEDIA_TYPE_MP4 == byMediaType)
			{
				wWidth  = 720;
				wHeight = 576;
			}

			cKDVVMPParam.SetVideoWidth(wWidth);
			cKDVVMPParam.SetVideoHeight(wHeight);
			cKDVVMPParam.m_byEncType = byMediaType;
			g_cMcuVcApp.SetVMPOutChlBitrate((TEqp)tVmpStatus, byVmpOutChnnl, wMinMtReqBitrate);
			cKDVVMPParam.SetBitRate(wMinMtReqBitrate);
		}
		
		cServMsg.CatMsgBody((u8 *)&cKDVVMPParam, sizeof(cKDVVMPParam));
	}

	//need PRS
	u8 byNeedPrs = m_tVmpCommonAttrb.m_byNeedPrs;
	cServMsg.CatMsgBody((u8 *)&byNeedPrs, sizeof(byNeedPrs));

	//画面合成风格
    TVmpStyleCfgInfo tMcuVmpStyle = m_tVmpCommonAttrb.m_tVmpStyleCfgInfo;    
    cServMsg.CatMsgBody((u8*)&tMcuVmpStyle, sizeof(tMcuVmpStyle));

	//设置画面合成空闲通道的背景
	cServMsg.CatMsgBody( (u8 *)&tVMPExCfgInfo, sizeof(TVMPExCfgInfo) );
	
	// 追加解码通道RcvH264DependInMark属性,8kg/8kh/8ki都支持
	for (u8 byIdx=0; byIdx<MAXNUM_SDVMP_MEMBER; byIdx++)
	{
		LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"VMP chnl[%d] RcvH264DependInMark: %d.\n", 
			byIdx, m_tVmpCommonAttrb.m_abyRcvH264DependInMark[byIdx]);
		cServMsg.CatMsgBody( (u8 *)&m_tVmpCommonAttrb.m_abyRcvH264DependInMark[byIdx],sizeof(u8));
	}

    // 追加Vmp成员别名 [7/4/2013 liaokang]
    // 结构：(u8) ChnNum + ( (u8)ChnNo + (u8)Len + (s8*)content + …)
    s8  achMbAlias[1 + (1 + 1 + MAXLEN_ALIAS)*MAXNUM_SDVMP_MEMBER] = {0};
    u8 *pchMbAlias = (u8*)achMbAlias;
    u8  byChnNum = 0;
    u16 wTypeLen = 1;
    pchMbAlias += wTypeLen; 
    s8  achAlias[MAXLEN_ALIAS] = {0}; 
    u8  byAliasLen = 0;
    for(u8 byChnNo=0; byChnNo<m_tVmpCommonAttrb.m_byMemberNum; byChnNo++)
    {
        if(m_tVmpCommonAttrb.m_atMtMember[byChnNo].IsNull())
        {
            continue;
        }
        
        memset(achAlias,0,sizeof(achAlias));
        if( !( pVcInst->GetMtAliasToVmp(m_tVmpCommonAttrb.m_atMtMember[byChnNo],achAlias) ) )
        {
            continue;
        }
        byChnNum++;
        byAliasLen = strlen(achAlias);
        *pchMbAlias = byChnNo;
        pchMbAlias++;
        *pchMbAlias = byAliasLen;
        pchMbAlias++;
        memcpy(pchMbAlias,achAlias,byAliasLen);
        pchMbAlias += byAliasLen;
        wTypeLen += (2 + byAliasLen);
    }
    pchMbAlias -= wTypeLen;
    *pchMbAlias = byChnNum;
    cServMsg.CatMsgBody((u8*)&achMbAlias[0], wTypeLen);

	if (VMP_8KH == byVmpSubType)
	{
		for (u8 byIdx=0; byIdx<sizeof(abyOutChnlActive); byIdx++)
		{
			LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"VMP Outchnl[%d] Active: %d, ProfileAttrb: %d.\n", 
				byIdx, abyOutChnlActive[byIdx], abyOutChnlAttrb[byIdx]);
		}
		//8kh画面合成带上通道是否Active数组
		cServMsg.CatMsgBody( abyOutChnlActive,sizeof(abyOutChnlActive));
		//8kh画面合成带上通道HB/BP数组
		cServMsg.CatMsgBody( abyOutChnlAttrb,sizeof(abyOutChnlAttrb));
	}
}

/*====================================================================
函数名      : ConstructMsgToVmp
功能        : 组织给画面合成器的参数(下参重构准备)
算法实现    ：
引用全局变量：
输入参数说明：CServMsg &cServMsg	[o]
返回值说明  ：void
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
08/05/2013            yanghuaizhi       创建
====================================================================*/
/*void CParamTo8kiVmp::ConstructMsgToVmp(CServMsg &cServMsg)
{
	CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(m_byConfIdx);
	if ( NULL == pVcInst )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ConstructMsgToVmp]ConfIdx.%d doesn't exist!\n", m_byConfIdx );
		return;
    }

	TConfInfo tConfInfo = pVcInst->m_tConf;
	cServMsg.SetConfId(tConfInfo.GetConfId());
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( m_byVmpId, &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;

	T8KEVmpCommonAttrb t8keVmpCommonAttrb;

	//style
	t8keVmpCommonAttrb.m_byVmpStyle = m_tVmpCommonAttrb.m_byVmpStyle;
	
	//member number
	t8keVmpCommonAttrb.m_byMemberNum  = m_tVmpCommonAttrb.m_byMemberNum;

	u8 byIdx;
    s8  achAlias[MAXLEN_ALIAS] = {0}; 
    u8  byAliasLen = 0;
	for (byIdx = 0; byIdx < MAXNUM_SDVMP_MEMBER; byIdx++)
	{
		// 设置成员信息
		t8keVmpCommonAttrb.m_atMtMember[byIdx].SetMember(m_tVmpCommonAttrb.m_atMtMember[byIdx]);
		// 设置成员别名，128长度
		memset(achAlias, 0, sizeof(achAlias));
        if( pVcInst->GetMtAliasToVmp(m_tVmpCommonAttrb.m_atMtMember[byIdx],achAlias))
        {
            t8keVmpCommonAttrb.m_atMtMember[byIdx].SetMbAlias(strlen(achAlias), achAlias);
        }
		//payload and encypt key
		t8keVmpCommonAttrb.m_tVideoEncrypt[byIdx] = m_tVmpCommonAttrb.m_tVideoEncrypt[byIdx];
		t8keVmpCommonAttrb.m_tDoublePayload[byIdx] = m_tVmpCommonAttrb.m_tDoublePayload[byIdx];
		t8keVmpCommonAttrb.m_abyRcvH264DependInMark[byIdx] = m_tVmpCommonAttrb.m_abyRcvH264DependInMark[byIdx];
	}
	t8keVmpCommonAttrb.m_tVmpStyleCfgInfo = m_tVmpCommonAttrb.m_tVmpStyleCfgInfo;		//配置信息
	t8keVmpCommonAttrb.m_tVmpExCfgInfo = m_tVmpCommonAttrb.m_tVmpExCfgInfo;		//扩展配置信息
	// 设置通用信息
	cServMsg.SetMsgBody((u8*)&t8keVmpCommonAttrb, sizeof(t8keVmpCommonAttrb));

	CKDVVMPOutMember cVMPOutMember;
	u8 byChnnlRes  = 0;
	u16 wWidth		= 0;	//宽
	u16 wHeight		= 0;	//高
	u16 wMinMtReqBitrate = 0;
	u8 byFrameRate = 0;
	TVideoStreamCap tStrCap;
	TEqp tVmpEqp = g_cMcuVcApp.GetEqp( m_byVmpId );
	u8 byMaxOutputChnnlNum = MAXNUM_MPU_OUTCHNNL;
	u8 abyOutChnlActive[MAXNUM_MPU_OUTCHNNL];		//输出通道是否Active,1:Active,0:非Active
	memset(abyOutChnlActive,emVmpOutChnlNotChange,sizeof(abyOutChnlActive));
	for (u8 byVmpOutChnnl = 0; byVmpOutChnnl < byMaxOutputChnnlNum; byVmpOutChnnl ++)
	{
		cVMPOutMember.Clear();
		// 获得视频能力
		tStrCap = m_tVMPOutParam.GetVmpOutCapIdx(byVmpOutChnnl);
		
		if (MEDIA_TYPE_NULL != tStrCap.GetMediaType())
		{
			//获得Idx对应能力集
			cVMPOutMember.SetEncType(tStrCap.GetMediaType());
			byChnnlRes = tStrCap.GetResolution();
			wMinMtReqBitrate = pVcInst->GetMinMtRcvBitByVmpChn(m_byVmpId, TRUE,byVmpOutChnnl);
			//mp4_auto 按照码率编对应分辨率
			if ( MEDIA_TYPE_MP4 == tStrCap.GetMediaType()
				&& VIDEO_FORMAT_AUTO == byChnnlRes )
			{
				if (wMinMtReqBitrate > 3072)
				{
					byChnnlRes = VIDEO_FORMAT_4CIF;
				} 
				else if(wMinMtReqBitrate > 1536)
				{
					byChnnlRes = VIDEO_FORMAT_2CIF;
				}
				else
				{
					byChnnlRes = VIDEO_FORMAT_CIF;
				}
			}
			// mp4 16cif 下参为mpeg4 4cif
			else if (MEDIA_TYPE_MP4 == tStrCap.GetMediaType()
				&& VIDEO_FORMAT_16CIF == byChnnlRes )
			{
				byChnnlRes = VIDEO_FORMAT_4CIF;
			}
			// h264 auto 下参为cif
			else if (MEDIA_TYPE_H264 == tStrCap.GetMediaType()
				&& VIDEO_FORMAT_AUTO == byChnnlRes )
			{
				byChnnlRes = VIDEO_FORMAT_CIF;
			}
			//根据能力集获得分辨率
			GetWHByRes(byChnnlRes , wWidth, wHeight);
			// 对MP4和4cif 严格区分给宽高
			if( MEDIA_TYPE_MP4 == tStrCap.GetMediaType() && VIDEO_FORMAT_4CIF == tStrCap.GetResolution())
			{
				wWidth  = 720;
				wHeight = 576;
			}
			
			//fps:
			byFrameRate = tStrCap.GetUserDefFrameRate();
			if( tStrCap.GetMediaType() == MEDIA_TYPE_H264)
			{
				//h264: the value is real, so maintain the value
			}
			else
			{
				byFrameRate = FrameRateMac2Real(byFrameRate);
			}

			cVMPOutMember.SetVideoWidth(wWidth);
			cVMPOutMember.SetVideoHeight(wHeight);
			cVMPOutMember.SetProfileType(tStrCap.GetH264ProfileAttrb());
			cVMPOutMember.SetFrameRate(byFrameRate);
			if (tStrCap.GetMaxBitRate() > 0)
			{
				// 码率不为0时才更新对应通道码率，8kivmp非广播可能停止某路编码
				g_cMcuVcApp.SetVMPOutChlBitrate((TEqp)tPeriEqpStatus, byVmpOutChnnl, wMinMtReqBitrate);
			}
			else
			{
				// 若码率为0，该路不编码
				abyOutChnlActive[byVmpOutChnnl] = emVmpOutChnlInactive;
			}
			cVMPOutMember.SetBitRate(wMinMtReqBitrate);//照常下参
		}
		else
		{
			cVMPOutMember.SetEncType(MEDIA_TYPE_NULL);
		}
		
		cServMsg.CatMsgBody((u8 *)&cVMPOutMember, sizeof(cVMPOutMember));
	}

}*/

/*====================================================================
函数名      : ConstructMsgToVmp
功能        : 组织给画面合成器的参数
算法实现    ：
引用全局变量：
输入参数说明：CServMsg &cServMsg	[o]
返回值说明  ：void
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
08/05/2013            yanghuaizhi       创建
====================================================================*/
void CParamTo8kiVmp::ConstructMsgToVmp(CServMsg &cServMsg)
{
	CMcuVcInst* pVcInst = g_cMcuVcApp.GetConfInstHandle(m_byConfIdx);
	if ( NULL == pVcInst )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[ConstructMsgToVmp]ConfIdx.%d doesn't exist!\n", m_byConfIdx );
		return;
    }

	TConfInfo tConfInfo = pVcInst->m_tConf;
	cServMsg.SetConfId(tConfInfo.GetConfId());
	TPeriEqpStatus tPeriEqpStatus; 
	g_cMcuVcApp.GetPeriEqpStatus( m_byVmpId, &tPeriEqpStatus );
	u8 byVmpSubType = tPeriEqpStatus.m_tStatus.tVmp.m_bySubType;

	C8KEVMPParam cKDVVMPParam;
	memset(&cKDVVMPParam, 0, sizeof(cKDVVMPParam));

	//style
	cKDVVMPParam.m_byVMPStyle = m_tVmpCommonAttrb.m_byVmpStyle;
	
	//enctype
	cKDVVMPParam.m_byEncType = tConfInfo.GetMainVideoMediaType();

	//member number
	cKDVVMPParam.m_byMemberNum  = m_tVmpCommonAttrb.m_byMemberNum;

	memcpy(cKDVVMPParam.m_atMtMember, m_tVmpCommonAttrb.m_atMtMember, sizeof(cKDVVMPParam.m_atMtMember));

	//payload and encypt key
	memcpy(cKDVVMPParam.m_tVideoEncrypt, m_tVmpCommonAttrb.m_tVideoEncrypt, sizeof(cKDVVMPParam.m_tVideoEncrypt));
	memcpy(cKDVVMPParam.m_tDoublePayload, m_tVmpCommonAttrb.m_tDoublePayload, sizeof(cKDVVMPParam.m_tDoublePayload));

	//获取画面合成空闲通道的背景
	TEqpExCfgInfo tEqpExCfgInfo;
	if( SUCCESS_AGENT != g_cMcuAgent.GetEqpExCfgInfo( tEqpExCfgInfo ) )
	{
		tEqpExCfgInfo.Init();
	}
	
	TVMPExCfgInfo tVMPExCfgInfo;
	tVMPExCfgInfo = tEqpExCfgInfo.m_tVMPExCfgInfo;
	
	//设置合成图像是否叠加合成成员别名
    cKDVVMPParam.m_byIsDisplayMmbAlias = tVMPExCfgInfo.m_byIsDisplayMmbAlias;	
	
	LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP,  "[ConstructMsgToVmp]Vmp IdleChlShowMode: %d, Vmp IsDisplayMmbAlias: %d \n", 
		tVMPExCfgInfo.m_byIdleChlShowMode,
		tVMPExCfgInfo.m_byIsDisplayMmbAlias);

	//fps: no by now

	//resolution, BR info
	u16 wMinMtReqBitrate = 0;
	u16 wWidth = 0;
	u16 wHeight = 0;
	u8 byChnnlRes  = 0;
	u8 byVmpOutChnnl = 0;
	TPeriEqpStatus tVmpStatus;
	g_cMcuVcApp.GetPeriEqpStatus(m_byVmpId, &tVmpStatus);
	
	u8 byMaxOutputChnnlNum = MAXNUM_MPU_OUTCHNNL;
	//u8 abyChnlResEx[MAXNUM_MPU_OUTCHNNL] = { 0 };	//通道对应的分辨率是否勾选了
	u8 abyOutChnlActive[MAXNUM_MPU_OUTCHNNL];		//输出通道是否Active,1:Active,0:非Active
	u8 abyOutChnlAttrb[MAXNUM_MPU_OUTCHNNL];		//8kh输出通道画质属性(HP/BP),emBpAttrb/emHpAttrb
	memset(abyOutChnlActive,emVmpOutChnlNotChange,sizeof(abyOutChnlActive));
	memset(abyOutChnlAttrb, emBpAttrb, sizeof(abyOutChnlAttrb));//默认emBpAttrb
	
	u8 byFrameRate = 0;
	TVideoStreamCap tStrCap;
	for (byVmpOutChnnl = 0; byVmpOutChnnl < byMaxOutputChnnlNum; byVmpOutChnnl ++)
	{
		// 获得视频能力
		tStrCap = m_tVMPOutParam.GetVmpOutCapIdx(byVmpOutChnnl);

		if (MEDIA_TYPE_NULL != tStrCap.GetMediaType())
		{
			//获得Idx对应能力集
			byChnnlRes = tStrCap.GetResolution();
			wMinMtReqBitrate = pVcInst->GetMinMtRcvBitByVmpChn(m_byVmpId, TRUE,byVmpOutChnnl);
			//mp4_auto 按照码率编对应分辨率
			if ( MEDIA_TYPE_MP4 == tStrCap.GetMediaType()
				&& VIDEO_FORMAT_AUTO == byChnnlRes )
			{
				if (wMinMtReqBitrate > 3072)
				{
					byChnnlRes = VIDEO_FORMAT_4CIF;
				} 
				else if(wMinMtReqBitrate > 1536)
				{
					byChnnlRes = VIDEO_FORMAT_2CIF;
				}
				else
				{
					byChnnlRes = VIDEO_FORMAT_CIF;
				}
			}
			// mp4 16cif 下参为mpeg4 4cif
			else if (MEDIA_TYPE_MP4 == tStrCap.GetMediaType()
				&& VIDEO_FORMAT_16CIF == byChnnlRes )
			{
				byChnnlRes = VIDEO_FORMAT_4CIF;
			}
			// h264 auto 下参为cif
			else if (MEDIA_TYPE_H264 == tStrCap.GetMediaType()
				&& VIDEO_FORMAT_AUTO == byChnnlRes )
			{
				byChnnlRes = VIDEO_FORMAT_CIF;
			}
			//根据能力集获得分辨率
			GetWHByRes(byChnnlRes , wWidth, wHeight);
			// 对MP4和4cif 严格区分给宽高
			if( MEDIA_TYPE_MP4 == tStrCap.GetMediaType() && VIDEO_FORMAT_4CIF == tStrCap.GetResolution())
			{
				wWidth  = 720;
				wHeight = 576;
			}
			
			//fps:
			byFrameRate = tStrCap.GetUserDefFrameRate();
			if( tStrCap.GetMediaType() == MEDIA_TYPE_H264)
			{
				//h264: the value is real, so maintain the value
			}
			else
			{
				byFrameRate = FrameRateMac2Real(byFrameRate);
			}

			cKDVVMPParam.SetVideoWidth(wWidth);
			cKDVVMPParam.SetVideoHeight(wHeight);
			cKDVVMPParam.m_byEncType = tStrCap.GetMediaType();
			if (tStrCap.GetMaxBitRate() > 0)
			{
				// 码率不为0时才更新对应通道码率，8kivmp非广播可能停止某路编码
				g_cMcuVcApp.SetVMPOutChlBitrate((TEqp)tVmpStatus, byVmpOutChnnl, wMinMtReqBitrate);
			}
			else
			{
				// 若码率为0，该路不编码
				abyOutChnlActive[byVmpOutChnnl] = emVmpOutChnlInactive;
			}
			cKDVVMPParam.SetBitRate(wMinMtReqBitrate);//照常下参
			abyOutChnlAttrb[byVmpOutChnnl] = tStrCap.GetH264ProfileAttrb();//记录HP/BP属性
			cKDVVMPParam.m_byFrameRate = byFrameRate;
		}
		else
		{
			cKDVVMPParam.m_byEncType = MEDIA_TYPE_NULL;
			cKDVVMPParam.m_wBitRate = 0;
			cKDVVMPParam.m_wVideoHeight = 0;
			cKDVVMPParam.m_wVideoWidth = 0;
			cKDVVMPParam.m_byFrameRate = 0;
		}
		
		cServMsg.CatMsgBody((u8 *)&cKDVVMPParam, sizeof(cKDVVMPParam));
	}

	//need PRS
	u8 byNeedPrs = m_tVmpCommonAttrb.m_byNeedPrs;
	cServMsg.CatMsgBody((u8 *)&byNeedPrs, sizeof(byNeedPrs));

	//画面合成风格
    TVmpStyleCfgInfo tMcuVmpStyle = m_tVmpCommonAttrb.m_tVmpStyleCfgInfo;    
    cServMsg.CatMsgBody((u8*)&tMcuVmpStyle, sizeof(tMcuVmpStyle));

	//设置画面合成空闲通道的背景
	cServMsg.CatMsgBody( (u8 *)&tVMPExCfgInfo, sizeof(TVMPExCfgInfo) );
	
	// 追加解码通道RcvH264DependInMark属性,8kg/8kh/8ki都支持
	for (u8 byIdx=0; byIdx<MAXNUM_SDVMP_MEMBER; byIdx++)
	{
		LogPrint(LOG_LVL_DETAIL,MID_MCU_VMP,"VMP chnl[%d] RcvH264DependInMark: %d.\n", 
			byIdx, m_tVmpCommonAttrb.m_abyRcvH264DependInMark[byIdx]);
		cServMsg.CatMsgBody( (u8 *)&m_tVmpCommonAttrb.m_abyRcvH264DependInMark[byIdx],sizeof(u8));
	}

    // 追加Vmp成员别名 [7/4/2013 liaokang]
    // 结构：(u8) ChnNum + ( (u8)ChnNo + (u8)Len + (s8*)content + …)
    s8  achMbAlias[1 + (1 + 1 + MAXLEN_ALIAS)*MAXNUM_SDVMP_MEMBER] = {0};
    u8 *pchMbAlias = (u8*)achMbAlias;
    u8  byChnNum = 0;
    u16 wTypeLen = 1;
    pchMbAlias += wTypeLen; 
    s8  achAlias[MAXLEN_ALIAS] = {0}; 
    u8  byAliasLen = 0;
    for(u8 byChnNo=0; byChnNo<m_tVmpCommonAttrb.m_byMemberNum; byChnNo++)
    {
        if(m_tVmpCommonAttrb.m_atMtMember[byChnNo].IsNull())
        {
            continue;
        }
        
        memset(achAlias,0,sizeof(achAlias));
        if( !( pVcInst->GetMtAliasToVmp(m_tVmpCommonAttrb.m_atMtMember[byChnNo],achAlias) ) )
        {
            continue;
        }
        byChnNum++;
        byAliasLen = strlen(achAlias);
        *pchMbAlias = byChnNo;
        pchMbAlias++;
        *pchMbAlias = byAliasLen;
        pchMbAlias++;
        memcpy(pchMbAlias,achAlias,byAliasLen);
        pchMbAlias += byAliasLen;
        wTypeLen += (2 + byAliasLen);
    }
    pchMbAlias -= wTypeLen;
    *pchMbAlias = byChnNum;
    cServMsg.CatMsgBody((u8*)&achMbAlias[0], wTypeLen);
	
	//8ki画面合成带上通道是否Active数组
	cServMsg.CatMsgBody( abyOutChnlActive,sizeof(abyOutChnlActive));
	//8ki画面合成带上通道HB/BP数组
	cServMsg.CatMsgBody( abyOutChnlAttrb,sizeof(abyOutChnlAttrb));

}

/*====================================================================
函数名      : IsConfAdpOptimization
功能        : 当前会议是否适配资源利用最大化
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
11/04/25                周嘉麟          创建
====================================================================*/
BOOL32 IsConfAdpOptimization(const TConfInfo& tConfinfo)
{
	TConfAttrbEx tConfAttrbEx = tConfinfo.GetConfAttrbEx();
	return tConfAttrbEx.IsImaxModeConf();
}

/*====================================================================
函数名      : IsConfNeedReserveMainCap
功能        : 当前会议是否需要预留主流主格式
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/01/31              倪志俊           创建
====================================================================*/
BOOL32	IsConfNeedReserveMainCap(const TConfInfo& tConfinfo, const TConfInfoEx &tConfInfoEx)
{
#if defined(_8KH_) || defined(_8KI_)
	if ( tConfinfo.GetProfileAttrb() == emHpAttrb )
	{
		//1080HP,勾选1080BP，则不预留HP
		if ( tConfinfo.GetMainVideoFormat() == VIDEO_FORMAT_HD1080 &&
			 tConfInfoEx.IsMSSupportCapEx(VIDEO_FORMAT_HD1080) 
			)
		{
			return FALSE;
		}

		//单速，无勾选，无辅格式会议，不预留，其他情况全部预留 720 30 HP
		//双速紧凑也必须预留
		if (  tConfinfo.GetSecBitRate() ==0 && 
			  !tConfInfoEx.IsMainHasCapEx() &&
			  tConfinfo.GetSecVideoMediaType() == MEDIA_TYPE_NULL
			)
		{
			return FALSE;
		}

		
#ifndef _8KI_
		return TRUE;
#endif

	}
	else
	{
#ifndef _8KI_
		//8000H,主格式如果是720 60/50 BP，并且勾选720 30/25 BP 只预留一路,此处不预留
		if ( tConfInfoEx.IsMainHasCapEx() &&
			tConfinfo.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
			tConfinfo.GetMainVideoFormat() == VIDEO_FORMAT_HD720 &&
			(tConfinfo.GetMainVidUsrDefFPS() == 60 || tConfinfo.GetMainVidUsrDefFPS() == 50) &&
			( tConfInfoEx.IsMSSupportCapEx(VIDEO_FORMAT_HD720,tConfinfo.GetMainVidUsrDefFPS()/2,emBpAttrb) == TRUE )
			)
		{
			return FALSE;
		}
#endif
		
		//8000H,主格式如果是1080 60，并且勾选1080 30/25 只预留一路
		if ( tConfInfoEx.IsMainHasCapEx() &&
			tConfinfo.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
			tConfinfo.GetMainVideoFormat() == VIDEO_FORMAT_HD1080 &&
			(tConfinfo.GetMainVidUsrDefFPS() == 60 || tConfinfo.GetMainVidUsrDefFPS() == 50) &&
			( tConfInfoEx.IsMSSupportCapEx(VIDEO_FORMAT_HD1080) == TRUE )
			)
		{
			return FALSE;
		}
	}
#endif

	//8ke 不支持1080编码
#ifdef _8KE_
	if ( 	tConfinfo.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
			tConfinfo.GetMainVideoFormat() == VIDEO_FORMAT_HD1080  
		)
	{
		return FALSE;
	}

#endif

#ifdef _MINIMCU_
	//主辅格式有H264和other，并且主格式无勾选，则需要预留,8000B,比较特殊，只有1出，故H264 CIF不预留
	if (  (!tConfInfoEx.IsMainHasCapEx() &&
		tConfinfo.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
		tConfinfo.GetMainVideoFormat() != VIDEO_FORMAT_CIF &&
		tConfinfo.GetSecVideoMediaType() != MEDIA_TYPE_NULL &&
		tConfinfo.GetSecVideoMediaType() != MEDIA_TYPE_H264) 
		||
		(!tConfInfoEx.IsMainHasCapEx() &&
		tConfinfo.GetMainVideoMediaType() != MEDIA_TYPE_H264 &&
		tConfinfo.GetMainVideoMediaType() != MEDIA_TYPE_NULL &&
		tConfinfo.GetSecVideoMediaType()  == MEDIA_TYPE_H264&&
		tConfinfo.GetSecVideoFormat() != VIDEO_FORMAT_CIF 
		)
	   )
	{
		return TRUE;
	}
#else
	//主辅格式有H264和other，并且主格式无勾选，则需要预留
	if (  (!tConfInfoEx.IsMainHasCapEx() &&
		tConfinfo.GetMainVideoMediaType() == MEDIA_TYPE_H264 &&
		tConfinfo.GetSecVideoMediaType() != MEDIA_TYPE_NULL &&
		tConfinfo.GetSecVideoMediaType() != MEDIA_TYPE_H264) 
		||
		(!tConfInfoEx.IsMainHasCapEx() &&
		tConfinfo.GetMainVideoMediaType() != MEDIA_TYPE_H264 &&
		tConfinfo.GetMainVideoMediaType() != MEDIA_TYPE_NULL &&
		tConfinfo.GetSecVideoMediaType()  == MEDIA_TYPE_H264)
		)
	{
		return TRUE;
	}
#endif

	//非双速不预留
	if ( 0 == tConfinfo.GetSecBitRate())
	{
		return FALSE;
	}

	//紧凑时必须有多格式，才不预留
	if ( g_cMcuVcApp.IsAdpResourceCompact() && tConfInfoEx.IsMainHasCapEx() )
	{
		return FALSE;
	}

	return TRUE;
}

/*====================================================================
函数名      : Is8KINeedDsAdaptH264SXGA
功能        : 8ki会议是否要预留264sxga20能力
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/08/14              周晶晶          创建
====================================================================*/
BOOL32 Is8KINeedDsAdaptH264SXGA( const TConfInfo& tConfinfo )
{
	if( (tConfinfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetResolution() > VIDEO_FORMAT_SXGA &&
			tConfinfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetUserDefFrameRate() >= 20) ||
			(tConfinfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetResolution() == VIDEO_FORMAT_SXGA &&
			tConfinfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetUserDefFrameRate() > 20)
		)
	{
		return TRUE;
	}

	return FALSE;
}

/*====================================================================
函数名      : IsConfHasBpCapOrBpExCap
功能        : 当前会议主流是否是bp能力或有勾选bp能力
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/09/02              周晶晶          创建
====================================================================*/
BOOL32 IsConfHasBpCapOrBpExCap( const TConfInfo& tConfinfo,const TConfInfoEx& tConfEx )
{
	TCapSupport tCapSupport = tConfinfo.GetCapSupport();
	if( tCapSupport.GetMainVideoType() == MEDIA_TYPE_H264 )
	{
		if( tCapSupport.GetMainStreamProfileAttrb() == emHpAttrb )
		{
			TVideoStreamCap atMStreamCapEx[MAX_CONF_CAP_EX_NUM];
			u8 byCapExNum = MAX_CONF_CAP_EX_NUM;
			tConfEx.GetMainStreamCapEx(atMStreamCapEx,byCapExNum);

			for( u8 byIdx = 0; byIdx < MAX_CONF_CAP_EX_NUM; byIdx++ )
			{
				if( atMStreamCapEx[byIdx].GetMediaType() == MEDIA_TYPE_H264 
					&& atMStreamCapEx[byIdx].GetH264ProfileAttrb() == emBpAttrb)
				{
					return TRUE;					
				}
			}
		}
		else
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*====================================================================
函数名      : IsConfNeedReserveDSMainCap
功能        : 当前会议是否需要预留双流主格式
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/01/31              倪志俊          创建
====================================================================*/
BOOL32 IsConfNeedReserveDSMainCap(const TConfInfo& tConfinfo, const TConfInfoEx &tConfInfoEx)
{
// 	if ( tConfinfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetMediaType() != MEDIA_TYPE_H264)
// 	{
// 		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[IsNeedReserveMainCap] conf's Main Media is not h264!\n");
// 		return FALSE;
// 	}

	//双流主格式为空也不需要预留
	if ( tConfinfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetMediaType() == MEDIA_TYPE_NULL ||
		tConfinfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetResolution() == VIDEO_FORMAT_INVALID	
		)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_CONF, "[IsNeedReserveMainCap] MediaType() or Resolution is invalid!\n");
		return FALSE;
	}


	if ( IsConfDoubleDual(tConfinfo)  && 
		!tConfInfoEx.IsDoubleHasCapEx()&&
		tConfinfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetMediaType() == MEDIA_TYPE_H264  &&
		tConfinfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetResolution() != VIDEO_FORMAT_XGA
		)
	{
		return TRUE;
	}

	//主格式UXA情况下，如果勾选1080，则需要预留主格式，因为UXGA的分辨率小于1080
	if ( tConfinfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetMediaType() == MEDIA_TYPE_H264  &&
		tConfinfo.GetCapSupport().GetDStreamCapSet().GetVideoStremCap().GetResolution() == VIDEO_FORMAT_UXGA &&
		tConfInfoEx.IsDSSupportCapEx(VIDEO_FORMAT_HD1080)
		)
	{
		return TRUE;
	}

	//8000E,8000H会组织勾选，故对于有勾选，则直接预留H264 XGA
#if	defined(_8KE_) || 	defined(_8KH_) || defined(_8KI_)
	if ( tConfInfoEx.IsDoubleHasCapEx() )
	{
		return TRUE;
	}
#endif

	if ( 0 == tConfinfo.GetSecBitRate())
	{
		return FALSE;
	}
	
	//紧凑时必须有多格式
	if ( g_cMcuVcApp.IsAdpResourceCompact() && tConfInfoEx.IsDoubleHasCapEx() )
	{
		return FALSE;
	}
	
	return TRUE;
}

/*====================================================================
函数名      : GetAACChnlTypeByAudioTrackNum
功能        : 通过声道数获取AACLD/AACLC相应通道类型
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/10/10              chendaiwei    创建
====================================================================*/
u8 GetAACChnlTypeByAudioTrackNum( u8 byAudioTrackNum )
{
	u8 byChnlType = 0;
	switch ( byAudioTrackNum )
	{
	//单声道
	case 1:
		byChnlType = AAC_CHNL_TYPE_SINGLE;
		break;
	//双声道
	case 2:
		byChnlType = AAC_CHNL_TYPE_DOUBLE;
		break;
	default:
		byChnlType = byAudioTrackNum;
		LogPrint(LOG_LVL_ERROR,MID_MCU_CONF,"[GetAACChnlTypeByAudioTrackNum] AudioTrackNum.%d, not support,error!\n");
		break;
	}

	return byChnlType;
}

/*====================================================================
函数名      : IsV4R6B2VcsTemplate
功能        : 判断是否是fix2或者8000H封闭版本的VCS模板
算法实现    ：fix2, 8000h的结构体TConfStore中无m_byVCAutoMode，解析文件
			  数据时需要特殊处理，跳过该字段
引用全局变量：
输入参数说明：File *hConfFile 文件
			  u16 wFileLenth 文件长度
返回值说明   是返回TRUE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/11/13              chendaiwei    创建
====================================================================*/
BOOL32 IsV4R6B2VcsTemplate ( FILE * hConfFile, u16 wFileLenth)
{
	BOOL32 bRet = FALSE;

	if(hConfFile == NULL || wFileLenth == 0)
	{
		return bRet;
	}

	//数据 普通 + 预案 + 扩展。跳过预案部分，获取其他信息 普通+扩展 [8/16/2012 chendaiwei]
	TConfStore tTempConfStore;
	TConfStore tUnPackConfstore;

	u16 wRemainFileLenth = wFileLenth - (strlen(CONFFILEHEAD) + sizeof(u16) + sizeof(u32));
	u16 wTempBufLen = min(wRemainFileLenth,sizeof(TConfStore));

	fseek(hConfFile,strlen(CONFFILEHEAD) + sizeof(u16) + sizeof(u32),SEEK_SET);
	fread((u8*)&tTempConfStore,wTempBufLen,1,hConfFile);
	u16 wPackConfDataLen = 0;
	TPackConfStore *ptTempPackConfstore = (TPackConfStore *)&tTempConfStore;
	UnPackConfStore(ptTempPackConfstore,tUnPackConfstore,wPackConfDataLen,FALSE);

	//对于fix2或8000H版本TConfStore结构体中无m_byVCAutoMode，导致一个U8的误差，此处特殊处理
	u8 byVcAutoModeExtraLen = 0;
	BOOL32 bIsFix2or8000HTemplate = FALSE;
	if( tTempConfStore.m_tConfInfo.GetConfSource() == VCS_CONF )
	{
		u16 wOldTemplateUnPackConfDataLen = wPackConfDataLen - 1;
		u16 wOldTEmplateRemainFileLenth = wRemainFileLenth - wOldTemplateUnPackConfDataLen;
		u16 wOldwPlanLen = 0;
		if( wOldTEmplateRemainFileLenth >= sizeof(u16))
		{
			fseek(hConfFile,strlen(CONFFILEHEAD) + sizeof(u16) + sizeof(u32) + wOldTemplateUnPackConfDataLen ,SEEK_SET);
			fread(&wOldwPlanLen,sizeof(u16),1,hConfFile);
			wOldwPlanLen = htons(wOldwPlanLen);
			wOldTEmplateRemainFileLenth -= sizeof(u16);
		}

		if( wOldwPlanLen == wOldTEmplateRemainFileLenth )
		{
			wPackConfDataLen--;
			bRet = TRUE;
			LogPrint( LOG_LVL_ERROR, MID_PUB_ALWAYS, "[GetConfDataFromFile] conf.%s is Fix2 or 8000H template without m_byVCAutoMode\n",tTempConfStore.m_tConfInfo.GetConfName());
		}
	}

	fseek(hConfFile,strlen(CONFFILEHEAD) + sizeof(u16) + sizeof(u32),SEEK_SET);

	return bRet;
}

/*====================================================================
函数名      : GetPlanDataLenByV4R6B2PlanData
功能        : 通过fix2的预案数据解析出若本地表示预案数据实际长度
算法实现    ：
引用全局变量：
输入参数说明：u8 *pbyV4R6B2PlanData fix2预案数据长度
			  u16 wV4R6B2Length 预案数据长度
			 
返回值说明   u16 返回预案数据长度
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/11/13              chendaiwei    创建
====================================================================*/
u16 GetPlanDataLenByV4R6B2PlanData( u8*pbyV4R6B2PlanData, u16 wV4R6B2Length)
{
	/*lint -save -esym(429, pbyV4R6B2PlanData)*/
	//保护
	if( wV4R6B2Length < 3 || pbyV4R6B2PlanData == NULL)
	{
		return 0;
	}
	
	u16 bRetLen = 0;
	u8 *pbyBuf = pbyV4R6B2PlanData;
	
	//第一个字节是预案个数
	u8 byPlanNum = *pbyBuf;
	pbyBuf++;
	bRetLen++;
	
	//预案个数0 返回
	if( 0 == byPlanNum )
	{
		return 0;
	}
	
	u8 byTemp = 0;
	for( u8 idx = 0;idx < byPlanNum;idx++ )
	{
		//现在只支持到8个电视墙预案
		if( idx >= VCS_MAXNUM_PLAN )
		{
			byPlanNum = VCS_MAXNUM_PLAN;
			break;
		}
		//预案名字长度u8，不包括\0
		byTemp = *pbyBuf;
		pbyBuf++;
		bRetLen++;

		//预案名字数据[11/14/2012 chendaiwei]
		pbyBuf += byTemp;
		bRetLen += byTemp;
		
		//终端别名个数u8
		byTemp = *pbyBuf;
		pbyBuf++;
		bRetLen++;

		pbyBuf += sizeof( TMtVCSPlanAliasV4R6B2 ) * byTemp;
		
		bRetLen += sizeof( TMtVCSPlanAlias ) * byTemp;
		
		if( ( wV4R6B2Length + pbyV4R6B2PlanData ) <= pbyBuf )
		{
			byPlanNum = idx + 1;
			break;
		}
	}

	return bRetLen;
	/*lint -restore*/
}

/*====================================================================
函数名      : TransferV4R6B2planDataToV4R7
功能        : v4r6b2或8000H的预案数据转化成v4r7预案数据
算法实现    ：
引用全局变量：
输入参数说明：u8 *pbyV4R6B2PlanData fix2预案数据
			  u16 wV4R6B2Length fix2预案数据长度
			  u8 *pbyV4R7PlanData v4r7预案数据首地址
			  u16 wV4R7DataLen v4r7预案数据长度
返回值说明   是返回TRUE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/11/13              chendaiwei    创建
====================================================================*/
BOOL32 TransferV4R6B2planDataToV4R7(u8*pbyV4R6B2PlanData,u16 wV4R6B2DataLen,u8*pbyV4R7PlanData,u16 wV4R7DataLen )
{
	/*lint -save -esym(429, pbyV4R6B2PlanData)*/
	/*lint -save -esym(429, pbyV4R7PlanData)*/

	if( wV4R6B2DataLen == 0 || wV4R7DataLen == 0 || pbyV4R7PlanData == NULL || pbyV4R6B2PlanData == NULL)
	{
		return FALSE;
	}
	
	u16 bRetLen = 0;
	u8 *pbyBuf = pbyV4R6B2PlanData;
	
	//第一个字节是预案个数
	u8 byPlanNum = *pbyBuf;
	*pbyV4R7PlanData = *pbyBuf;
	pbyBuf++;
	pbyV4R7PlanData++;

	//预案个数0 返回
	if( 0 == byPlanNum )
	{
		return FALSE;
	}
	
	u8 byTemp = 0;
	for( u8 idx = 0;idx < byPlanNum;idx++ )
	{
		//现在只支持到8个电视墙预案
		if( idx >= VCS_MAXNUM_PLAN )
		{
			byPlanNum = VCS_MAXNUM_PLAN;
			break;
		}
		//预案名字长度u8，不包括\0
		byTemp = *pbyBuf;
		*pbyV4R7PlanData = byTemp;
		pbyBuf++;
		pbyV4R7PlanData++;
		
		//预案名字数据[11/14/2012 chendaiwei]
		memcpy(pbyV4R7PlanData,pbyBuf,byTemp);
		pbyBuf += byTemp;
		pbyV4R7PlanData += byTemp;
		
		//终端别名个数u8
		byTemp = *pbyBuf;
		*pbyV4R7PlanData = byTemp;
		pbyBuf++;
		pbyV4R7PlanData++;

		u8 byV4R6AlaisHeadLen = 33;
		u8 byV4R6AliasTrailLen = sizeof(TTransportAddr)+sizeof(u8);
		for( u8 byIdx = 0; byIdx < byTemp; byIdx++)
		{
			memcpy(pbyV4R7PlanData,pbyBuf,byV4R6AlaisHeadLen);
			pbyV4R7PlanData += byV4R6AlaisHeadLen;
			pbyBuf += byV4R6AlaisHeadLen;

			memset(pbyV4R7PlanData,0,VCS_MAXLEN_ALIAS-32);
			pbyV4R7PlanData += (VCS_MAXLEN_ALIAS-32);

			memcpy(pbyV4R7PlanData,pbyBuf,byV4R6AliasTrailLen);
			pbyBuf+= byV4R6AliasTrailLen;
			pbyV4R7PlanData+= byV4R6AliasTrailLen;
		}
		
		if( ( wV4R6B2DataLen + pbyV4R6B2PlanData ) <= pbyBuf )
		{
			break;
		}
	}
	
	return TRUE;
	/*lint -restore*/
	/*lint -restore*/
}

//END OF FILE
