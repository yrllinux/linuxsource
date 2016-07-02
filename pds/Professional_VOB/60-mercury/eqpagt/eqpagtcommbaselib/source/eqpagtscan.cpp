/*****************************************************************************
    模块名      : EqpAgt
    文件名      : eqpagtscan.cpp
    相关文件    : 
    文件实现功能: 扫描设备trap信息
    作者        : liaokang
    版本        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    修改记录:
    日  期      版本        修改人          修改内容
    2012/06/18  4.7         liaokang      创建
******************************************************************************/
#include "eqpagtscan.h"
#include "eqpagtsnmp.h"
#include "eqpagtcommbase.h"

CEqpAgtScanApp	g_cEqpAgtScanApp;

// 构造函数
CEqpAgtScan::CEqpAgtScan()
{
    m_dwMaxTimerIdx = 0;
}

// 析构函数
CEqpAgtScan::~CEqpAgtScan()
{
}

/*====================================================================
    函数名      ：InstanceEntry
    功能        ：实例消息处理入口函数，必须override
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：void
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/08/19    1.0         jianghy       创建
====================================================================*/
void CEqpAgtScan::InstanceEntry(CMessage* const pcMsg)
{
	if( NULL == pcMsg )
	{
        EqpAgtLog( LOG_ERROR, "[CEqpAgtGuard::InstanceEntry] Null point!\n" );
		return;
	}
    
	switch( pcMsg->event )
	{
	case EQPAGT_SCAN_POWERON:       // 启动消息
		ProcScanPowerOn();
		break;

    case EQPAGT_SCAN_TIMER:         // 常规定时器
        ProcScanTimer();
		break;

	default:
        ProcTrapInfoScan( pcMsg );  // 各业务模块扫描定时器
		break;
	}	
	return;
}

/*====================================================================
    函数名      : ProcScanPowerOn
    功能        : 模块启动消息
    算法实现    :
    引用全局变量:
    输入参数说明: CMessage * const pcMsg, 传入的消息
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtScan::ProcScanPowerOn( void )
{
	SetTimer( EQPAGT_SCAN_TIMER, SCAN_STATE_TIME_OUT ); // 等待系统稳态
	return;
}

/*====================================================================
    函数名      : ScanAndSendTrap
    功能        : 扫描获取Trap信息并发送
    算法实现    :
    引用全局变量: g_cEqpAgtSnmp
    输入参数说明: CMessage * const pcMsg, 传入的消息
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtScan::ScanAndSendTrap( TEqpAgtTrapFunc pfTrapFunc )
{
    if ( NULL == pfTrapFunc  )
    {
        EqpAgtLog( LOG_DETAIL, "[ScanAndSendTrap] The input param is null!\n" );
        return;
    }

    u16 wRet = NO_TRAP;
    CNodes cNodes;
    cNodes.Clear();
    wRet = (*pfTrapFunc)( cNodes );
    if ( NO_TRAP == wRet  )
    {
        EqpAgtLog( LOG_DETAIL, "[ScanAndSendTrap] No Trap!\n" );
    }
    else// 获取到Trap信息
    {
        wRet = g_cEqpAgtSnmp.EqpAgtSendTrap( cNodes );
        if ( SNMP_GENERAL_ERROR == wRet ) // 发送错误
        {                    
            EqpAgtLog( LOG_ERROR, "[ScanAndSendTrap] Send Trap failed!\n" );
        }
        else // 发送正确
        {
            EqpAgtLog( LOG_DETAIL, "[ScanAndSendTrap] Send Trap Success!\n" );
        }
    }
    return;
}

/*====================================================================
    函数名      : ProcScanTimer
    功能        : 常规定时器
    算法实现    :
    引用全局变量: g_cEqpAgtTrapList
    输入参数说明: 
    返回值说明  : void
    ----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtScan::ProcScanTimer( void )
{
    u32 dwTimerId = EQPAGT_SCAN_TIMER;
    u16 wLoop = 0;
    u16 wTrapFuncNum = g_cEqpAgtTrapList.Size();
    u32 dwTrapScanTime = 0;
    TEqpAgtTrapEntry tEqpAgtTrapEntry;
    memset( &tEqpAgtTrapEntry, 0, sizeof(tEqpAgtTrapEntry));
    TEqpAgtTrapFunc pfTrapFunc = NULL;
    BOOL32 bRet = TRUE;
    
    // 无业务模块Trap信息定时扫描器加载
    if ( 0 == g_cEqpAgtTrapList.Size() )
    {
        SetTimer( EQPAGT_SCAN_TIMER, SCAN_STATE_TIME_OUT );
        return;
    }

    // 存在业务模块Trap信息定时扫描器没有启动
    if ( 0 == m_dwMaxTimerIdx )
    {
        // 查询所有Trap函数信息，并发送
        for( wLoop = 0; wLoop < wTrapFuncNum; wLoop++ )
        {
            bRet = g_cEqpAgtTrapList.GetEqpAgtTrapEntry( wLoop, &tEqpAgtTrapEntry );
            if ( bRet )
            {
                pfTrapFunc = tEqpAgtTrapEntry.pfFunc;
                // 扫描获取Trap信息并发送
                ScanAndSendTrap( pfTrapFunc );
                // 设立定时器
                dwTimerId++;
                dwTrapScanTime = tEqpAgtTrapEntry.dwScanTimeSpan * 1000;
                SetTimer( dwTimerId, dwTrapScanTime );
            }
        }
        // 记录
        m_dwMaxTimerIdx = dwTimerId;
        SetTimer( EQPAGT_SCAN_TIMER, SCAN_STATE_TIME_OUT );
        return;
    }

    if( ( m_dwMaxTimerIdx - EQPAGT_SCAN_TIMER ) < wTrapFuncNum )
    {      
        // 查询新增Trap函数信息
        dwTimerId = m_dwMaxTimerIdx;
        for( wLoop = (u16)(m_dwMaxTimerIdx - EQPAGT_SCAN_TIMER - 1); wLoop < wTrapFuncNum; wLoop++ )
        {
            bRet = g_cEqpAgtTrapList.GetEqpAgtTrapEntry( wLoop, &tEqpAgtTrapEntry );
            if ( bRet )
            {
                pfTrapFunc = tEqpAgtTrapEntry.pfFunc;
                // 扫描获取Trap信息并发送
                ScanAndSendTrap( pfTrapFunc );
                // 设立定时器
                dwTimerId++;
                dwTrapScanTime = tEqpAgtTrapEntry.dwScanTimeSpan * 1000;
                SetTimer( dwTimerId, dwTrapScanTime );
            }
        }
        // 记录
        m_dwMaxTimerIdx = dwTimerId;
        SetTimer( EQPAGT_SCAN_TIMER, SCAN_STATE_TIME_OUT );
        return;        
    }
}

/*====================================================================
    函数名      : ProcTrapInfoScan
    功能        : 各业务模块Trap信息定时扫描
    算法实现    :
    引用全局变量:
    输入参数说明: CMessage * const pcMsg, 传入的消息
    返回值说明  : void
    ----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtScan::ProcTrapInfoScan( CMessage* const pcMsg )
{
    u32 dwTimerId = pcMsg->event;
    u16 wLoop = 0;
    u32 dwTrapScanTime = 0;
    TEqpAgtTrapEntry tEqpAgtTrapEntry;
    memset( &tEqpAgtTrapEntry, 0, sizeof(tEqpAgtTrapEntry));
    TEqpAgtTrapFunc pfTrapFunc = NULL;
    BOOL32 bRet = TRUE;

    // 某个Trap函数的定时器
    if ( ( EQPAGT_SCAN_TIMER < dwTimerId ) && ( dwTimerId <= m_dwMaxTimerIdx ) )
    {
        wLoop = (u16)( dwTimerId - EQPAGT_SCAN_TIMER ) - 1;
        bRet = g_cEqpAgtTrapList.GetEqpAgtTrapEntry( wLoop, &tEqpAgtTrapEntry );
        if ( bRet )
        {
            if ( tEqpAgtTrapEntry.bEnable )
            {
                EqpAgtLog( LOG_DETAIL, "[ProcTrapInfoScan] scaning node(0x%x) !\n", tEqpAgtTrapEntry.dwNodeValue );
                pfTrapFunc = tEqpAgtTrapEntry.pfFunc;
                // 扫描获取Trap信息并发送
                ScanAndSendTrap( pfTrapFunc );
            }
        }   
        // 设立定时器
        dwTrapScanTime = tEqpAgtTrapEntry.dwScanTimeSpan * 1000;
        SetTimer( dwTimerId, dwTrapScanTime );
    }
    else
    {
        EqpAgtLog( LOG_ERROR, "[ProcTrapInfoScan] wrong message received !\n" );
    }
    return;
}

// END OF FILE
