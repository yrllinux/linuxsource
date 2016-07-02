/*****************************************************************************
    模块名      : EqpAgt
    文件名      : eqpagtsnmp.cpp
    相关文件    : 
    文件实现功能: Snmp功能调用
    作者        : liaokang
    版本        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    修改记录:
    日  期      版本        修改人        修改内容
    2012/06/18  4.7         liaokang      创建
******************************************************************************/
#include "eqpagtsnmp.h"
#include "eqpagtcfg.h"

CEqpAgtSnmp g_cEqpAgtSnmp;

// 构造
CEqpAgtSnmp::CEqpAgtSnmp()
{
}

// 析构
CEqpAgtSnmp::~CEqpAgtSnmp()
{
}

/*====================================================================
    函数名      : EqpAgtSendTrap
    功能        : 发送Trap
    算法实现    :
    引用全局变量: g_cEqpAgtCfg
    输入参数说明: CNodes& cNodes Trap节点信息
    返回值说明  : u16
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
u16 CEqpAgtSnmp::EqpAgtSendTrap(CNodes& cNodes)
{  
    u16 wRet = SNMP_GENERAL_ERROR;
    TTarget tTrapRcvAddr;
    memset( &tTrapRcvAddr, 0, sizeof(tTrapRcvAddr) );

    if ( !g_cEqpAgtCfg.HasSnmpNms() )
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtSendTrap] no NMS!\n");
        return wRet;
    }

    // 1、发送Trap
    for( u8 byLoop = 0; byLoop < g_cEqpAgtCfg.GetTrapServerNum(); byLoop++ )
    {
        g_cEqpAgtCfg.GetTrapTarget( byLoop, tTrapRcvAddr );

        if( ( 0 != tTrapRcvAddr.dwIp ) && ( 0 != tTrapRcvAddr.wPort ) )
        {
            wRet = SendTrap(tTrapRcvAddr, cNodes);
        }
    }

    // 2、释放
#ifdef _LINUX_

    if ( SNMP_SUCCESS != wRet )
    {
        EqpAgtLog( LOG_ERROR, "[EqpAgtSendTrap] send trap failed !\n");
    }
    else
    {
        EqpAgtLog( LOG_KEYSTATUS, "[EqpAgtSendTrap] send trap successed !\n");
        wRet = FreeNodes( cNodes );
        if ( SNMP_SUCCESS != wRet )
        {
            EqpAgtLog( LOG_ERROR, "[EqpAgtSendTrap] Free Nodes failed !\n");
        }
        else
        {
            EqpAgtLog( LOG_KEYSTATUS, "[EqpAgtSendTrap] Free Nodes successed !\n");
        }
    }

#endif
    
    return wRet;
}

// END OF FILE
