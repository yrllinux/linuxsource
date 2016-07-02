/*=============================================================================
模   块   名: 无
文   件   名: Ping.h
相 关  文 件: Ping.cpp
文件实现功能: ping功能
作        者: 王昊
版        本: V4.0  Copyright(C) 2003-2006 Kedacom, All rights reserved.
-------------------------------------------------------------------------------
修改记录:
日      期  版本    修改人      修改内容
2006/05/31  4.0     王昊        创建
=============================================================================*/

#ifndef __PING_H__
#define __PING_H__

#ifdef WIN32
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#endif  //  WIN32

//Ping错误类型
enum enumPingRt
{
    emPingRtNoError         = (u8)0,    //成功
    emPingRtUnreachable     = (u8)1,    //对端不可及
    emPingRtTimeOut         = (u8)2,    //超时
    emPingRtEnd             = (u8)3     //全部测试结束
};

//Ping结果返回值
typedef struct tagPingRst
{
    tagPingRst() : m_bEnd(FALSE), m_nTimeCount(0), m_nTotalCount(0),
                   m_nSucCount(0), m_nEchoTime(0), m_nMinEchoTime(0),
                   m_nMaxEchoTime(0), m_nAvrEchoTime(0), m_byPingRt(0),
				   m_byTTL(0),m_nJitter(0)
    {
    }

    BOOL32  m_bEnd;     //TRUE: One Test End; FALSE: Test Begin
    s32     m_nTimeCount;//本次测试次数
    s32     m_nTotalCount;//总共测试次数, m_bEnd为TRUE且m_byPingRt为emPingRtEnd有效
    s32     m_nSucCount;//成功次数, m_bEnd为TRUE且m_byPingRt为emPingRtEnd有效
    s32     m_nEchoTime;//本次响应时间, m_bEnd为TRUE且m_byPingRt为emPingRtNoError有效
    s32     m_nMinEchoTime;//最短响应时间, m_bEnd为TRUE且m_byPingRt为emPingRtEnd有效
    s32     m_nMaxEchoTime;//最长响应时间, m_bEnd为TRUE且m_byPingRt为emPingRtEnd有效
    s32     m_nAvrEchoTime;//平均响应时间, m_bEnd为TRUE且m_byPingRt为emPingRtEnd有效
	s32     m_nJitter;     //抖动,m_bEnd为TRUE且m_byPingRt为emPingRtEnd有效
    u8      m_byPingRt; //enumPingRt, m_bEnd为TRUE有效
    u8      m_byTTL;    //生存时间, m_bEnd为TRUE且m_byPingRt为emPingRtNoError有效
} TPingRst, *PTPingRst;

/*=============================================================================
函 数 名:PFNPingResult
功    能:回调函数
参    数:void *pcbData                      [in]    回调数据
         const TPingRst &tPingRst           [in]    返回值
注    意:无
返 回 值:继续进行: TRUE; 停止ping测试: FALSE
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2006/06/02  4.0     王昊    创建
=============================================================================*/
typedef BOOL32 (*PFNPingResult)(void *pcbData, const TPingRst &tPingRst);

class CPing
{
public:
    /*=============================================================================
    函 数 名:CPing
    功    能:构造函数
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/06/02  4.0     王昊    创建
    =============================================================================*/
    CPing();

    virtual ~CPing();

public:
    /*=============================================================================
    函 数 名:SetParam
    功    能:设置参数
    参    数:u32 dwHostIP                   [in]    对端IP地址(主机序)
             s32 nTimes                     [in]    测试次数
             s32 nPackSize                  [in]    测试包长
             PFNPingResult pfnRst           [in]    回调函数
             void *pcbData                  [in]    回调数据
    注    意:无
    返 回 值:参数检查结果
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/06/05  4.0     王昊    创建
    =============================================================================*/
    BOOL32 SetParam(u32 dwHostIP, s32 nTimes, s32 nPackSize,
                    PFNPingResult pfnRst, void *pcbData);

    /*=============================================================================
    函 数 名:Start
    功    能:开始测试
    参    数:无
    注    意:无
    返 回 值:成功: TRUE; 失败: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/06/05  4.0     王昊    创建
    =============================================================================*/
    BOOL32 Start(void);

    /*=============================================================================
    函 数 名:GetErrStr
    功    能:获取错误描述
    参    数:LPCSTR lptrLan                 [in]    "english"/"chinese"
    注    意:无
    返 回 值:错误描述字符串
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/06/05  4.0     王昊    创建
    =============================================================================*/
    LPCTSTR GetErrStr(LPCTSTR lptrLan);

private:
    /*=============================================================================
    函 数 名:CheckParam
    功    能:检查参数
    参    数:s32 nTimes                     [in]    测试次数
             s32 nPackSize                  [in]    测试包长
             PFNPingResult pfnRst           [in]    回调函数
    注    意:无
    返 回 值:参数合法: TRUE; 参数非法: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/06/05  4.0     王昊    创建
    =============================================================================*/
    BOOL32 CheckParam(s32 nTimes, s32 nPackSize, PFNPingResult pfnRst);

private:
    //  外部设置参数
    u32             m_dwHostIP; //对端IP地址, 主机序
    s32             m_nTimes;   //测试次数
    s32             m_nPackSize;//发送包长
//    u8              m_byTimeout;//超时时长
    PFNPingResult   m_pfnResult;//回调函数
    void            *m_pcbData; //回调数据

    //  内部处理参数
    u8              m_byErrCode;     //错误码
    TCHAR           m_aszErrStr[1024];
};

#endif  //  __PING_H__