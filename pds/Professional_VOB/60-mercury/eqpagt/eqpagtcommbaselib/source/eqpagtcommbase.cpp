/*****************************************************************************
    模块名      : EqpAgt
    文件名      : eqpagtcommbase.cpp
    相关文件    : 
    文件实现功能: EqpAgt共用
    作者        : liaokang
    版本        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    修改记录:
    日  期      版本        修改人          修改内容
    2012/06/18  4.7         liaokang        创建
******************************************************************************/
#include "eqpagtcommbase.h"
#include "eqpagtsnmp.h"
#include "eqpagtcfg.h"
#include "eqpagtscan.h"

CEqpAgtCBList   g_cEqpAgtGetList;
CEqpAgtCBList   g_cEqpAgtSetList;
CEqpAgtTrapList g_cEqpAgtTrapList;

// 信号量保护
extern SEMHANDLE   g_hEqpAgtCfg;

// 构造
CEqpAgtCBList::CEqpAgtCBList():m_wEqpAgtCBSize(0)
{
    m_pEqpAgtCBHead = NULL;
}

// 析构
CEqpAgtCBList::~CEqpAgtCBList()
{
    m_wEqpAgtCBSize = 0;
    m_pEqpAgtCBHead = NULL;
}

/*====================================================================
    函数名      : GetEqpAgtCBEntry
    功能        : 获取对应序号的回调函数结构信息
    算法实现    :
    引用全局变量:
    输入参数说明: u16 wEqpAgtCBIdx                   序号
    输出参数说明: TEqpAgtCBEntry* ptEqpAgtCBntry     对应的回调函数结构信息
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtCBList::GetEqpAgtCBEntry( u16 wEqpAgtCBIdx, TEqpAgtCBEntry* ptEqpAgtCBntry )
{
    if ( NULL == ptEqpAgtCBntry )
    {
        EqpAgtLog( LOG_ERROR, "[GetEqpAgtTrapEntry] The input param is null!\n" );
        return FALSE;
    }
    
    EqpAgtCB *pEqpAgtCB = m_pEqpAgtCBHead;
    while( NULL != pEqpAgtCB ) 
    {
        if( wEqpAgtCBIdx == pEqpAgtCB->m_wEqpAgtCBIdx ) 
        {
            memcpy( ptEqpAgtCBntry, &pEqpAgtCB->m_tEqpAgtCBEntry, sizeof(TEqpAgtCBEntry) );
            return TRUE;
        } 
        pEqpAgtCB = pEqpAgtCB->m_pEqpAgtCBNext; 
    }
    EqpAgtLog( LOG_ERROR, "[GetEqpAgtTrapEntry] Do not find right param match to the index %u !\n", wEqpAgtCBIdx );
    return FALSE;
}

/*====================================================================
    函数名      : Replace
    功能        : 替换链表中对应的回调函数结构信息
    算法实现    :
    引用全局变量:
    输入参数说明: const TEqpAgtTrapEntry& tEqpAgtTrapEntry 回调函数结构信息
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtCBList::Replace( const TEqpAgtCBEntry& tEqpAgtCBEntry )
{
    if( NULL == &tEqpAgtCBEntry )
    {
        EqpAgtLog( LOG_ERROR, "[Replace] The input param is null!\n" );
        return;
    }
    
    EqpAgtCB *pTail = m_pEqpAgtCBHead;
    if ( NULL == pTail )
    {
        EqpAgtLog( LOG_ERROR, "[Replace] No item!\n" );
        return;
    }
    else
    {
        while ( NULL != pTail )
        {
            if( tEqpAgtCBEntry.dwNodeValue == pTail->m_tEqpAgtCBEntry.dwNodeValue ) 
            {                   
                // 替换
                memcpy( &pTail->m_tEqpAgtCBEntry, &tEqpAgtCBEntry, sizeof(pTail->m_tEqpAgtCBEntry));
                return;
            }
            pTail = pTail->m_pEqpAgtCBNext;
        }
    }
    return;    
}

/*====================================================================
    函数名      : PushBack
    功能        : 在链表末尾存储回调函数结构信息
    算法实现    :
    引用全局变量:
    输入参数说明: const TEqpAgtCBEntry& tEqpAgtCBEntry 回调函数结构信息
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtCBList::PushBack( const TEqpAgtCBEntry& tEqpAgtCBEntry )
{
    if( NULL == &tEqpAgtCBEntry )
    {
        EqpAgtLog( LOG_ERROR, "[PushBack] The input param is null!\n" );
        return;
    }

    EqpAgtCB *pNew = new EqpAgtCB[1];
    if( NULL == pNew )
    {
        EqpAgtLog( LOG_ERROR, "[PushBack] Allocate pNew Memory failed!\n" );
        return;
    }
    memcpy( &pNew->m_tEqpAgtCBEntry, &tEqpAgtCBEntry, sizeof(pNew->m_tEqpAgtCBEntry));
    pNew->m_wEqpAgtCBIdx = m_wEqpAgtCBSize;
    m_wEqpAgtCBSize++;
    EqpAgtCB *pTail = m_pEqpAgtCBHead;
    if ( NULL == pTail )
    {
        pTail = pNew;
        pTail->m_pEqpAgtCBNext = NULL;
    }
    else
    {
        while ( NULL != pTail->m_pEqpAgtCBNext )
        {
            pTail = pTail->m_pEqpAgtCBNext;
        }
        pTail->m_pEqpAgtCBNext = pNew;
        pTail = pTail->m_pEqpAgtCBNext;
        pTail->m_pEqpAgtCBNext = NULL;
    }

    if( NULL == m_pEqpAgtCBHead )
    {
        m_pEqpAgtCBHead = pTail;
    }

    return;
}

// 构造
CEqpAgtTrapList::CEqpAgtTrapList():m_wEqpAgtTrapSize(0)
{
    m_pEqpAgtTrapHead = NULL;
}

// 析构
CEqpAgtTrapList::~CEqpAgtTrapList()
{
    m_wEqpAgtTrapSize = 0;
    m_pEqpAgtTrapHead = NULL;
}

/*====================================================================
    函数名      : Replace
    功能        : 替换链表中对应的Trap函数结构信息
    算法实现    :
    引用全局变量:
    输入参数说明: const TEqpAgtTrapEntry& tEqpAgtTrapEntry Trap函数结构信息
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtTrapList::Replace(const TEqpAgtTrapEntry& tEqpAgtTrapEntry)
{
    if( NULL == &tEqpAgtTrapEntry )
    {
        EqpAgtLog( LOG_ERROR, "[Replace] The input param is null!\n" );
        return;
    }

    EqpAgtTrap *pTail = m_pEqpAgtTrapHead;
    if ( NULL == pTail )
    {
        EqpAgtLog( LOG_ERROR, "[Replace] No item!\n" );
        return;
    }
    else
    {
        while ( NULL != pTail )
        {
            if( tEqpAgtTrapEntry.dwNodeValue == pTail->m_tEqpAgtTrapEntry.dwNodeValue ) 
            {                   
                // 替换
                memcpy( &pTail->m_tEqpAgtTrapEntry, &tEqpAgtTrapEntry, sizeof(pTail->m_tEqpAgtTrapEntry));
                return;
            }
            pTail = pTail->m_pEqpAgtTrapNext;
        }
    }
    return;
}

/*====================================================================
    函数名      : PushBack
    功能        : 在链表末尾存储Trap函数结构信息
    算法实现    :
    引用全局变量:
    输入参数说明: const TEqpAgtTrapEntry& tEqpAgtTrapEntry Trap函数结构信息
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void CEqpAgtTrapList::PushBack(const TEqpAgtTrapEntry& tEqpAgtTrapEntry)
{
    if( NULL == &tEqpAgtTrapEntry )
    {
        EqpAgtLog( LOG_ERROR, "[PushBack] The input param is null!\n" );
        return;
    }

    EqpAgtTrap *pNew = new EqpAgtTrap[1];
    if( NULL == pNew )
    {
        EqpAgtLog( LOG_ERROR, "[PushBack] Allocate pNew Memory failed!\n" );
        return;
    }
    memcpy( &pNew->m_tEqpAgtTrapEntry, &tEqpAgtTrapEntry, sizeof(pNew->m_tEqpAgtTrapEntry));
    pNew->m_wEqpAgtTrapIdx = m_wEqpAgtTrapSize;
    m_wEqpAgtTrapSize++;
    EqpAgtTrap *pTail = m_pEqpAgtTrapHead;
    if ( NULL == pTail )
    {
        pTail = pNew;
        pTail->m_pEqpAgtTrapNext = NULL;
    }
    else
    {
        while ( NULL != pTail->m_pEqpAgtTrapNext )
        {
            pTail = pTail->m_pEqpAgtTrapNext;
        }
        pTail->m_pEqpAgtTrapNext = pNew;
        pTail = pTail->m_pEqpAgtTrapNext;
        pTail->m_pEqpAgtTrapNext = NULL;
    }
    
    if( NULL == m_pEqpAgtTrapHead )
    {
        m_pEqpAgtTrapHead = pTail;
    }
    
    return;   
}

/*====================================================================
    函数名      : GetEqpAgtTrapEntry
    功能        : 获取对应序号的trap函数结构信息
    算法实现    :
    引用全局变量:
    输入参数说明: u16 wEqpAgtTrapIdx                   序号
    输出参数说明: TEqpAgtTrapEntry* ptEqpAgtTrapEntry  对应的trap函数结构信息
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CEqpAgtTrapList::GetEqpAgtTrapEntry( u16 wEqpAgtTrapIdx, TEqpAgtTrapEntry* ptEqpAgtTrapEntry )
{
    if ( NULL == ptEqpAgtTrapEntry )
    {
        EqpAgtLog( LOG_ERROR, "[GetEqpAgtTrapEntry] The input param is null!\n" );
        return FALSE;
    }

    EqpAgtTrap *pEqpAgtTrap = m_pEqpAgtTrapHead;
    while( NULL != pEqpAgtTrap ) 
    {
        if( wEqpAgtTrapIdx == pEqpAgtTrap->m_wEqpAgtTrapIdx ) 
        {
            memcpy( ptEqpAgtTrapEntry, &pEqpAgtTrap->m_tEqpAgtTrapEntry, sizeof(TEqpAgtTrapEntry) );
            return TRUE;
        } 
        pEqpAgtTrap = pEqpAgtTrap->m_pEqpAgtTrapNext; 
    }
    EqpAgtLog( LOG_ERROR, "[GetEqpAgtTrapEntry] Do not find right param match to the index %u !\n", wEqpAgtTrapIdx );
    return FALSE;
}

/*====================================================================
    函数名      : EqpAgtGetOper
    功能        : snmp的回调get操作注册
    算法实现    :
    引用全局变量: g_cEqpAgtGetList
    输入参数说明: u32 dwNodeName        结点名，根据结点名区分业务处理函数
                  TEqpAgtCBFunc  pfFunc 回调函数业务处理具体实现
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void EqpAgtGetOper(u32 dwNodeValue, TEqpAgtCBFunc pfFunc )
{
    // 1、判断链表中是否已经存在对应函数,防止反复注册同一函数
    u16 wGetFuncNum = g_cEqpAgtGetList.Size();
    TEqpAgtCBEntry tEqpAgtGetEntry;
    memset( &tEqpAgtGetEntry, 0, sizeof(tEqpAgtGetEntry));
    BOOL32 bRet = TRUE;
    u16 wLoop = 0;
    if ( 0!= wGetFuncNum )
    {
        // 查询所有Trap函数信息，并发送
        for( wLoop = 0; wLoop < wGetFuncNum; wLoop++ )
        {
            bRet = g_cEqpAgtGetList.GetEqpAgtCBEntry( wLoop, &tEqpAgtGetEntry );
            if ( bRet )
            {
                if( dwNodeValue == tEqpAgtGetEntry.dwNodeValue ) 
                {                   
                    // 直接替换
                    TEqpAgtCBEntry tEqpAgtGetEntryNew = { dwNodeValue, pfFunc };
                    g_cEqpAgtGetList.Replace( tEqpAgtGetEntryNew );
                    return;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                return;
            }
        }
    }
    
    
    // 2、不存在，添加
    TEqpAgtCBEntry tEqpAgtGetEntryNew={ dwNodeValue, pfFunc };
    g_cEqpAgtGetList.PushBack( tEqpAgtGetEntryNew );
    return;
}

/*====================================================================
    函数名      : EqpAgtSetOper
    功能        : snmp的回调set操作注册
    算法实现    :
    引用全局变量: g_cEqpAgtSetList
    输入参数说明: u32 dwNodeName        结点名，根据结点名区分业务处理函数
                  TEqpAgtCBFunc  pfFunc 回调函数业务处理具体实现
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void EqpAgtSetOper(u32 dwNodeValue, TEqpAgtCBFunc pfFunc )
{
    // 1、判断链表中是否已经存在对应函数,防止反复注册同一函数
    u16 wSetFuncNum = g_cEqpAgtSetList.Size();
    TEqpAgtCBEntry tEqpAgtSetEntry;
    memset( &tEqpAgtSetEntry, 0, sizeof(tEqpAgtSetEntry));
    BOOL32 bRet = TRUE;
    u16 wLoop = 0;
    if ( 0!= wSetFuncNum )
    {
        // 查询所有Trap函数信息，并发送
        for( wLoop = 0; wLoop < wSetFuncNum; wLoop++ )
        {
            bRet = g_cEqpAgtSetList.GetEqpAgtCBEntry( wLoop, &tEqpAgtSetEntry );
            if ( bRet )
            {
                if( dwNodeValue == tEqpAgtSetEntry.dwNodeValue ) 
                {                   
                    // 直接替换
                    TEqpAgtCBEntry tEqpAgtSetEntryNew = { dwNodeValue, pfFunc };
                    g_cEqpAgtSetList.Replace( tEqpAgtSetEntryNew );
                    return;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                return;
            }
        }
    }


    // 2、不存在，添加
    TEqpAgtCBEntry tEqpAgtSetEntryNew={ dwNodeValue, pfFunc };
    g_cEqpAgtSetList.PushBack( tEqpAgtSetEntryNew );
    return;
}

/*====================================================================
    函数名      : EqpAgtTrapOper
    功能        : snmp的trap操作注册
    算法实现    :
    引用全局变量: g_cEqpAgtTrapList
    输入参数说明: u32 dwNodeName        结点名，根据结点名区分业务处理函数
                  void* pfFunc          Trap函数
                  u32 dwScanTimeSpan    定时检测Trap的时长（单位：秒）
                  BOOL32 bEnable        是否启用
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void EqpAgtTrapOper(u32 dwNodeValue, TEqpAgtTrapFunc pfFunc, u32 dwScanTimeSpan, BOOL32 bEnable )
{
    // 1、判断链表中是否已经存在对应函数,防止反复注册同一函数
    u16 wTrapFuncNum = g_cEqpAgtTrapList.Size();
    TEqpAgtTrapEntry tEqpAgtTrapEntry;
    memset( &tEqpAgtTrapEntry, 0, sizeof(tEqpAgtTrapEntry));
    BOOL32 bRet = TRUE;
    u16 wLoop = 0;
    if ( 0!= wTrapFuncNum )
    {
        // 查询所有Trap函数信息，并发送
        for( wLoop = 0; wLoop < wTrapFuncNum; wLoop++ )
        {
            bRet = g_cEqpAgtTrapList.GetEqpAgtTrapEntry( wLoop, &tEqpAgtTrapEntry );
            if ( bRet )
            {
                if( dwNodeValue == tEqpAgtTrapEntry.dwNodeValue ) 
                {                   
                    // 直接替换
                    TEqpAgtTrapEntry tEqpAgtTrapEntryNew = { dwNodeValue, pfFunc, dwScanTimeSpan, bEnable };
                    g_cEqpAgtTrapList.Replace( tEqpAgtTrapEntryNew );
                    return;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                return;
            }
        }
    }

    // 2、不存在，添加
    TEqpAgtTrapEntry tEqpAgtTrapEntryNew={ dwNodeValue, pfFunc, dwScanTimeSpan, bEnable };
    g_cEqpAgtTrapList.PushBack( tEqpAgtTrapEntryNew );
    return;
}

/*====================================================================
    函数名      : AgentCallBack
    功能        : Snmp回调函数实现
    算法实现    :
    引用全局变量: g_cEqpAgtCfg
                  g_cEqpAgtGetList
                  g_cEqpAgtSetList
    输入参数说明: u32 dwNodeName:    构造节点名
                  u8  byRWFlag:      读写模式
                  void * pBuf:       输入输出数据
                  u16 *pwBufLen:     数据长度
    返回值说明  : SNMP_SUCCESS       成功
                  SNMP_GENERAL_ERROR 错误
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
u16 AgentCallBack(u32 dwNodeName, u8 byRWFlag, void * pBuf, u16* pwBufLen)
{
    u32    dwNodeValue = GET_NODENAME(dwNodeName);
    BOOL32 bRead = ( READ_FLAG == byRWFlag ) ? TRUE : FALSE;
    u16    wResult = SNMP_SUCCESS;
    BOOL32 bRet = TRUE;

    // 1、首先判断是否为设置Trap信息
    switch(dwNodeValue) 
    {
    // Trap Ip信息
    case NODE_COMMTRAPIPS:
        if( bRead ) // get
        {
            bRet = g_cEqpAgtCfg.TrapIpsOfEqpAgt2Nms( pBuf, pwBufLen );
            if ( !bRet )
            {
                wResult = SNMP_GENERAL_ERROR;
            }
            return wResult;
        }
        else // set
        {
            bRet = g_cEqpAgtCfg.TrapIpsOfNms2EqpAgt( pBuf, pwBufLen );            
            if ( !bRet )
            {
                wResult = SNMP_GENERAL_ERROR;
            }
            return wResult;
        }

    // 其余信息
    default:        
        break;
    }	  

    // 2、查找对应函数进行处理
    TEqpAgtCBEntry tEqpAgtCBEntry;
    memset( &tEqpAgtCBEntry, 0, sizeof(tEqpAgtCBEntry));
    u16 wLoop = 0;

    if ( bRead )
    { 
        u16 wGetFuncNum = g_cEqpAgtGetList.Size();
        if ( 0!= wGetFuncNum )
        {
            for( wLoop = 0; wLoop < wGetFuncNum; wLoop++ )
            {
                bRet = g_cEqpAgtGetList.GetEqpAgtCBEntry( wLoop, &tEqpAgtCBEntry );
                if ( bRet )
                {
                    if( dwNodeValue == tEqpAgtCBEntry.dwNodeValue ) 
                    {
                        wResult = (*tEqpAgtCBEntry.pfFunc)( dwNodeName, pBuf, pwBufLen);
                        return wResult;
                    }
                    else
                    {
                        continue;
                    }
                }
                else
                {
                    wResult = SNMP_GENERAL_ERROR;
                    return wResult;
                }
            }
        }
    }
    else
    {
        u16 wSetFuncNum = g_cEqpAgtSetList.Size();
        if ( 0!= wSetFuncNum )
        {
            for( wLoop = 0; wLoop < wSetFuncNum; wLoop++ )
            {
                bRet = g_cEqpAgtSetList.GetEqpAgtCBEntry( wLoop, &tEqpAgtCBEntry );
                if ( bRet )
                {
                    if( dwNodeValue == tEqpAgtCBEntry.dwNodeValue ) 
                    {
                        wResult = (*tEqpAgtCBEntry.pfFunc)( dwNodeName, pBuf, pwBufLen);
                        return wResult;
                    }
                    else
                    {
                        continue;
                    }
                }
                else
                {
                    wResult = SNMP_GENERAL_ERROR;
                    return wResult;
                }
            }
        }
    }

    // 3、找不到对应的处理函数
    EqpAgtLog( LOG_ERROR, "[AgentCallBack] Received node(0x%x) is not supported by operate: %d<1.read-0.write>!\n", dwNodeName, bRead );
    wResult = SNMP_GENERAL_ERROR;
    return wResult;
}

/*====================================================================
    函数名      : EqpAgtInitialize
    功能        : EqpAgt初始化
    算法实现    :
    引用全局对象: g_cEqpAgtCfg
                  g_cEqpAgtSnmp
                  g_cEqpAgtScanApp
    输入参数说明: u16 wAidEqpAgtScan     线程id
                  u8 byAppPri   线程优先级
                  LPCSTR lpszCfgFileFullPath 配置文件全路径
    返回值说明  : 成功则返回 EQPAGT_SUCCESS
                  否则返回错误码，详见eqpagtcommon.h
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
u16 EqpAgtCommBaseInit( u16 wAidEqpAgtScan, u8 byAppPri, LPCSTR lpszCfgPath, LPCSTR lpszCfgName )
{
    u16    wRet = SNMP_SUCCESS;
    s32    nRet = OSP_OK;

    // 1、配置文件初始化
    g_cEqpAgtCfg.EqpAgtCfgInit( lpszCfgPath, lpszCfgName );

    // 2、snmp初始化
    TSnmpAdpParam tSnmpParam;
    memset( &tSnmpParam, 0, sizeof(tSnmpParam) );
	g_cEqpAgtCfg.GetSnmpParam( tSnmpParam );
    wRet = g_cEqpAgtSnmp.Initialize( AgentCallBack, &tSnmpParam );
    if ( SNMP_SUCCESS == wRet )
    {
        EqpAgtLog( LOG_KEYSTATUS, "[EqpAgtInit] snmp init successed!\n" );
    }
    else
    {
        EqpAgtLog( LOG_KEYSTATUS, "[EqpAgtInit] snmp init failed!\n" );
        return EQPAGT_INITERR_SNMPINIT;
    }

    // 3、扫描设备trap信息线程创建及启动
    nRet = g_cEqpAgtScanApp.CreateApp("EqpAgtScan", wAidEqpAgtScan, byAppPri, 50);
    if ( OSP_OK == nRet )
    {
        EqpAgtLog( LOG_KEYSTATUS, "[EqpAgtInit] Creat EqpAgtScan App successed!\n" );
        nRet = OspPost( MAKEIID(wAidEqpAgtScan, 1), EQPAGT_SCAN_POWERON );
        if ( OSP_OK == nRet )
        {
            EqpAgtLog( LOG_KEYSTATUS, "[EqpAgtInit] Post power on msg to EqpAgtScan successed!\n" );
            return EQPAGT_SUCCESS;
        }
        else
        {
            EqpAgtLog( LOG_KEYSTATUS, "[EqpAgtInit] Post power on msg to EqpAgtScan failed!\n" );
            return EQPAGT_INITERR_POSTMSG;
        }
    }
    else
    {      
        EqpAgtLog( LOG_ERROR, "[EqpAgtInit] Creat EqpAgtScan App failed!\n" );
        return EQPAGT_INITERR_CREATAPP;
    }
}

/*====================================================================
    函数名      : EqpAgtQuit
    功能        : EqpAgt退出
    算法实现    :
    引用全局对象: 
    输入参数说明: 
    返回值说明  : 
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void EqpAgtCommBaseQuit(void)
{
    g_cEqpAgtSnmp.Quit();
}