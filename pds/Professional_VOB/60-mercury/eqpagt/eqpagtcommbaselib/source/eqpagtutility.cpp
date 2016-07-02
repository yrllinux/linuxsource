/*****************************************************************************
    模块名      : EqpAgt
    文件名      : eqpagtutility.h
    相关文件    : 
    文件实现功能: EqpAgt业务内部使用结构和类
    作者        : liaokang
    版本        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    修改记录:
    日  期      版本        修改人          修改内容
    2012/06/18  4.7         liaokang        创建
******************************************************************************/
#include "eqpagtutility.h"

/*====================================================================
    函数名      : CreateSemHandle
    功能        : 创建信号量
    算法实现    :
    引用全局变量:
    输入参数说明: SEMHANDLE &hSem 信号量
    输出参数说明: 
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 CreateSemHandle( SEMHANDLE &hSem )
{
    BOOL32 bRet = TRUE;    
    // 创建信号量
    if( !OspSemBCreate(&hSem) )
    {
        OspSemDelete( hSem );
        hSem = NULL;
        StaticLog( "[CreateSemHandle] Create semHandle failed!\n");
        bRet = FALSE;
    }
    return bRet;
}

/*====================================================================
    函数名      : FreeSemHandle
    功能        : 释放信号量
    算法实现    :
    引用全局变量:
    输入参数说明: SEMHANDLE &hSem 信号量
    输出参数说明: 
    返回值说明  : BOOL32 成功返回TURE,失败返回FALSE
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
BOOL32 FreeSemHandle( SEMHANDLE &hSem )
{
    // 删除信号量
    if( NULL != hSem )
    {
        OspSemDelete( hSem );
        hSem = NULL;
    }
    return TRUE;
}

/*====================================================================
    函数名      : GetIpFromU32
    功能        : 由U32得到Ip字符串
    算法实现    :
    引用全局变量:
    输入参数说明: u32 dwIpInfo     Ip信息（u32 主机序）
    输出参数说明: LPSTR lpszDstStr Ip字符串
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
void GetIpFromU32( LPSTR lpszDstStr, u32 dwIpInfo )
{
    if( NULL != lpszDstStr )    
    {
        u32 dwTmpIp = dwIpInfo;
        if( 0 != dwTmpIp )
        {
            sprintf(lpszDstStr, "%d.%d.%d.%d%c", (dwTmpIp>>24)&0xFF, (dwTmpIp>>16)&0xFF, (dwTmpIp>>8)&0xFF, dwTmpIp&0xFF, 0);
        }
        else
        {
            memcpy(lpszDstStr, "0", sizeof("0"));
        }
    }    
    return;
}

/*====================================================================
    函数名      : EqpAgtLog
    功能        : 打印实现
    算法实现    :
    引用全局变量:
    输入参数说明: u8 byPrintLvl    打印等级
                  s8* pszFmt, ...  格式
    返回值说明  : void
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人        修改内容
    12/06/18    4.7         liaokang       创建
====================================================================*/
/*lint -save -e530 */
/*lint -save -esym(438, argptr)*/
/*lint -save -esym(526, __builtin_va_start)*/
/*lint -save -esym(628, __builtin_va_start)*/
void EqpAgtLog( u8 byPrintLvl, s8* pszFmt, ... )
{
    s8  achPrintBuf[1024] = { 0 };
    va_list argptr;
    s32 nBufLen = 0;
    nBufLen = sprintf( achPrintBuf, "[EqpAgt]" );
    va_start( argptr, pszFmt );
    vsnprintf( achPrintBuf + nBufLen, 1024-1, pszFmt, argptr );
    va_end(argptr);
    LogPrint( byPrintLvl, MID_SNMP_EQPAGT, achPrintBuf);
    return;
}

// 静态打印
void StaticLog( LPCSTR lpszFmt, ...)
{
    s8 achBuf[1024] = { 0 };
    va_list argptr;    
    va_start( argptr, lpszFmt );
    vsnprintf(achBuf, 1024-1, lpszFmt, argptr );
    LogPrint( LOG_ERROR, MID_PUB_ALWAYS, achBuf);
    va_end( argptr );
}
/*lint -restore*/
