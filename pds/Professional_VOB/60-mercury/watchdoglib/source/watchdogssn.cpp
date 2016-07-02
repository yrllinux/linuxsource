/*****************************************************************************
   模块名      : WatchDog 实例实现
   文件名      : watchdog.cpp
   相关文件    : 
   文件实现功能: 

   作者        : 顾振华
   版本        : V4.0  Copyright(C) 2003-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/05/26  4.0         顾振华      创建
******************************************************************************/
#include "watchdogssn.h"
#include "watchdogdef.h"

#ifdef _DEBUG
#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWatchDogSsnInst::CWatchDogSsnInst()
{
    g_bWDLog = FALSE;
}

CWatchDogSsnInst::~CWatchDogSsnInst()
{

}

/*====================================================================
    函数名      ：WDLog
    功能        ：打印消息
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2005/05/26  4.0         顾振华      创建
====================================================================*/
/*lint -save -e438 -e550*/
void WDLog( s8 * pszFmt, ... )
{
    if (g_bWDLog)
    {
	    s8 achPrintBuf[255];
        s32 nBufLen;
        va_list argptr;
	    s32 nLen = sprintf( achPrintBuf, "[WDClnt]:" );
        va_start( argptr, pszFmt );    
        nBufLen = vsprintf( achPrintBuf + nLen, pszFmt, argptr );   
        OspPrintf( nLen, FALSE, achPrintBuf );
        va_end(argptr);
    }
}
/*lint -restore*/

/*====================================================================
    函数名      ：ProcSetModuleState
    功能        ：处理发送模块状态
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2005/05/26  4.0         顾振华      创建
====================================================================*/
void CWatchDogSsnInst::ProcSetModuleState( )
{    
    if (m_dwServerNode != INVALID_NODE && 
        m_dwServerIID != INVALID_INS)
    {
        for (u8 byLoop = 0; byLoop < g_cWatchDogSsn.m_byModuleNum; byLoop ++)
        {
            ::OspPost( m_dwServerIID, EV_MODULESETSTATE, 
                (u8*)&g_cWatchDogSsn.m_atModuleParam[byLoop], 
                sizeof( IModuleParameter ), 
                m_dwServerNode, 
                MAKEIID(WD_CLT_APP_ID, 1));

            WDLog( "Sending Module State: %u, %u.\n", 
                g_cWatchDogSsn.m_atModuleParam[byLoop].emType, 
                g_cWatchDogSsn.m_atModuleParam[byLoop].emState );
        }
    }    
}

/*====================================================================
    函数名      ：ProcConnectWDServer
    功能        ：处理连接服务器
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2005/05/26  4.0         顾振华      创建
====================================================================*/
void CWatchDogSsnInst::ProcConnectWDServer()
{    
    if ( INVALID_NODE == m_dwServerNode )
    {
        u32 dwServerNode = ::OspConnectTcpNode(WD_SER_IP, WD_SER_TCP_PORT);
        
        if(INVALID_NODE != dwServerNode)
        {             
            m_dwServerNode = dwServerNode;
        }
        else
        {
            SetTimer( EV_CONNECTWDSERV_TIMER, TIMER_REG_DISC );
            return;
        }
    }

    ::OspNodeDiscCBReg( m_dwServerNode, GetAppID(), GetInsID() );	//注册断链接收实例

    // 发送连接请求消息每个都重新注册一遍
    WDLog( "WD Server TCP Connected, Registing...\n");

    for (u8 byLoop = 0; byLoop < g_cWatchDogSsn.m_byModuleNum; byLoop ++)
    {
        ::OspPost( MAKEIID(WD_SER_APP_ID, 1 ), EV_MODULECONCMD, 
            (u8*)&g_cWatchDogSsn.m_atModuleParam[byLoop], 
            sizeof( IModuleParameter ), 
            m_dwServerNode, 
            MAKEIID(WD_CLT_APP_ID, 1));
    }
    
    // 注册超时定时器
    SetTimer( EV_REGWDSERV_TIMER, TIMER_REG_DISC );
}

/*------------------------------------------------------------------------
	 函数名:   InstanceEntry
	 功  能:   消息入口函数 
	 参  数:   CMessage  * const pMsg           [in]接收的消息     
	 返回值:   void
	  */
void CWatchDogSsnInst::InstanceEntry(CMessage *const pcMsg)
{
	u16 wCurEvent = pcMsg->event;
	switch(wCurEvent)//判断传输的事件类型
	{

    case OSP_POWERON:
    // 启动
        {
            SetTimer( EV_CONNECTWDSERV_TIMER, TIMER_REG_POWERON );
        }
        
        break;

    case OSP_DISCONNECT:
    // 断链
        {
            u32 dwNodeDisconnect = *(u32*)pcMsg->content;
            if ( INVALID_NODE != dwNodeDisconnect )
            {
                OspDisconnectTcpNode( dwNodeDisconnect );
            }
            SetTimer( EV_CONNECTWDSERV_TIMER, TIMER_REG_DISC );
        }
        break;

    // 连接超时        
    case EV_CONNECTWDSERV_TIMER:
        KillTimer( EV_CONNECTWDSERV_TIMER );
        ProcConnectWDServer();
        break; 

    // 注册超时
    case EV_REGWDSERV_TIMER:
        KillTimer( EV_REGWDSERV_TIMER );
        if(INVALID_NODE != m_dwServerNode)
        {
            // 断开连接
            OspDisconnectTcpNode ( m_dwServerNode );
        }
        break;

	// 连接确认
	case EV_MODULECONIND:  
        KillTimer( EV_REGWDSERV_TIMER );
        m_dwServerIID = pcMsg->srcid;
                
        WDLog( "Regist to WatchDog Server Ack!\n");
        
        // 设置本模块状态
        ProcSetModuleState();
        break; 
	default:
        WDLog( "Unknown Event: %s(%u)!\n", OspEventDesc( wCurEvent ),  wCurEvent);
		break;
	}
}
