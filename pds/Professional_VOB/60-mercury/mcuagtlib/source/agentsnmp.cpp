/*****************************************************************************
   模块名      : Mcu Agent
   文件名      : AgentSnmp.cpp
   相关文件    : AgentSnmp.h
   文件实现功能: Snmp功能调用
   作者        : 
   版本        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/08/17  4.0         liuhuiyun     创建
******************************************************************************/
#include "agentsnmp.h"
#include "configureagent.h"

CAgentSnmp::CAgentSnmp()
{
}
CAgentSnmp::~CAgentSnmp()
{
}

/*=============================================================================
  函 数 名： AgentSendTrap
  功    能： 发送Trap
  算法实现： 
  全局变量： 
  参    数： TTarget tTrapRcvAddr
             CNodes & cNodes
  返 回 值： u16 
=============================================================================*/
u16 CAgentSnmp::AgentSendTrap(CNodes & cNodes)
{
    u8      byLoop = 0;
    TTarget tTrapRcvAddr;
    memset( &tTrapRcvAddr, 0, sizeof(tTrapRcvAddr) );
    u16     wRet = SNMP_GENERAL_ERROR;

    for(; byLoop < g_cCfgParse.GetTrapServerNum(); byLoop++)
    {
        g_cCfgParse.GetTrapTarget(byLoop, tTrapRcvAddr);

        if(0 != tTrapRcvAddr.dwIp && 0 != tTrapRcvAddr.wPort)
        {
            wRet = SendTrap(tTrapRcvAddr, cNodes);
        }
    }
#ifdef _LINUX_
    if ( SNMP_SUCCESS == wRet )
    {
        wRet = FreeNodes( cNodes );
    }
    else
    {
        Agtlog(LOG_ERROR, "[AgentSendTrap] send trap failed !\n");
    }
#endif

    return wRet;
}

// END OF FILE
