/*=============================================================================
模   块   名: 无
文   件   名: TraceRT.h
相 关  文 件: TraceRT.cpp
文件实现功能: tracert功能
作        者: 王昊
注        意: 最多测试30个网关, 每个网关测试三次
版        本: V4.0  Copyright(C) 2003-2006 Kedacom, All rights reserved.
-------------------------------------------------------------------------------
修改记录:
日      期  版本    修改人      修改内容
2006/06/06  4.0     王昊        创建
=============================================================================*/

#ifndef __TRACERT_H__
#define __TRACERT_H__

#ifdef WIN32
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#endif  //  WIN32

#define MAX_GW_NUM          (s32)30     //最多测试30个网关
#define TEST_PER_GW         (s32)3      //每个网关测试三次

//TraceRt错误类型
enum enumTraceRt
{
    emTraceRtEcho           = (u8)0,    //网关有响应
    emTraceRtGW             = (u8)1,    //网关地址
    emTraceRtUnreachable    = (u8)2,    //对端不可及
    emTraceRtOtherMsg       = (u8)3,    //其他消息, 无需处理, 仅用做中断操作
    emTraceRtEnd            = (u8)4     //全部测试结束
};

//Ping结果返回值
typedef struct tagTraceRtRst
{
    tagTraceRtRst() : m_byTraceRt(0), m_dwEchoTime(0), m_dwGWIP(0) {}

    u8  m_byTraceRt;    //返回结果
    u32 m_dwEchoTime;   //返回时间(主机序), m_byTraceRt为emTraceRtEcho时有效
    u32 m_dwGWIP;       //经过网关IP地址(主机序), m_byTraceRt为emTraceRtGW时有效
} TTraceRtRst, *PTTraceRtRst;

/*=============================================================================
函 数 名:PFNTraceRTResult
功    能:回调函数
参    数:void *pcbData                      [in]    回调数据
         const TTraceRtRst &tTraceRtRst     [in]    返回值
注    意:无
返 回 值:继续进行: TRUE; 停止tracert测试: FALSE
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2006/06/06  4.0     王昊    创建
=============================================================================*/
typedef BOOL32 (*PFNTraceRTResult)(void *pcbData, const TTraceRtRst &tTraceRtRst);

class CTraceRoute
{
public:
    CTraceRoute();
    virtual ~CTraceRoute();

    /*=============================================================================
    函 数 名:SetParam
    功    能:设置参数
    参    数:u32 dwHostIP                   [in]    对端IP地址(主机序)
             PFNTraceRTResult pfnResult     [in]    回调函数
             void *pcbData                  [in]    回调数据
    注    意:无
    返 回 值:参数检查结果
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/06/06  4.0     王昊    创建
    =============================================================================*/
    BOOL32 SetParam(u32 dwHostIP, PFNTraceRTResult pfnResult, void *pcbData);

    /*=============================================================================
    函 数 名:Start
    功    能:开始测试
    参    数:无
    注    意:无
    返 回 值:成功: TRUE; 失败: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/06/06  4.0     王昊    创建
    =============================================================================*/
    BOOL32 Start(void);

private:
    /*=============================================================================
    函 数 名:CheckParam
    功    能:检查参数
    参    数:u32 dwHostIP                   [in]    对端IP地址(主机序)
             PFNTraceRTResult pfnResult     [in]    回调函数
    注    意:无
    返 回 值:参数合法: TRUE; 参数非法: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/06/06  4.0     王昊    创建
    =============================================================================*/
    BOOL32 CheckParam(u32 dwHostIP, PFNTraceRTResult pfnResult);

    /*=============================================================================
    函 数 名:SendReq
    功    能:发送请求
    参    数:SOCKET s                       [in]    socket
             u16 wSN                        [in]    序列号
    注    意:无
    返 回 值:发送成功: TRUE; 发送失败: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/06/06  4.0     王昊    创建
    =============================================================================*/
    BOOL32 SendReq(SOCKET s, u16 wSN);

private:
    u32                 m_dwHostIP; //对端IP地址(主机序)
    PFNTraceRTResult    m_pfnResult;//回调函数
    void                *m_pcbData; //回调数据
};


#endif  //  __TRACERT_H__