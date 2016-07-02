/*=============================================================================
模   块   名: 无
文   件   名: IcmpStruct.h
相 关  文 件: 无
文件实现功能: ICMP协议 结构定义
作        者: 王昊
版        本: V4.0  Copyright(C) 2003-2006 Kedacom, All rights reserved.
-------------------------------------------------------------------------------
修改记录:
日      期  版本    修改人      修改内容
2006/05/30  4.0     王昊        创建
=============================================================================*/

#ifndef __ICMPSTRUCT_H__
#define __ICMPSTRUCT_H__

#include "kdvtype.h"

#ifdef WIN32
#include <WINSOCK2.H>
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#endif  //  WIN32

//IP头版本
enum enumIPVersion
{
    emIPv4  = (u8)4 << 4,       //  01000000    4 << 4
    emIpv6  = (u8)6 << 4        //  01100000    6 << 4
};

//IP头最小长度, 按u32计算
#define MIN_IPHEADER_LEN        (u8)5

//ICMP发送消息最大长度, 按u8计算
#define MAX_ICMP_SENDMSG_LEN    (u16)10 * 1024

//IP头优先类型
enum enumPrecedence
{
    emNetworkControl        = (u8)7 << 5,   //  111 - Network Control
    emInternetworkControl   = (u8)6 << 5,   //  110 - Internetwork Control
    emCRITICECP             = (u8)5 << 5,   //  101 - CRITIC/ECP
    emFlashOverride         = (u8)4 << 5,   //  100 - Flash Override
    emFlash                 = (u8)3 << 5,   //  011 - Flash
    emImmediate             = (u8)2 << 5,   //  010 - Immediate
    emPriority              = (u8)1 << 5,   //  001 - Priority
    emRoutine               = (u8)0 << 5,   //  000 - Routine
};

//ICMP消息类型
enum enumIcmpType
{
    emDstUnreachable        = (u8)3,        // 对端不可及
    emTimeExceeded          = (u8)11,       // 超时
    emParamError            = (u8)12,       // 参数错误
    emSourceQuench          = (u8)4,        // Source Quench
    emRedirect              = (u8)5,        // 重定向
    emEcho                  = (u8)8,        // 回应
    emEchoReply             = (u8)0,        // 回应响应
    emTimeStamp             = (u8)13,       // 时间戳
    emTimeStampReply        = (u8)14,       // 时间戳响应
    emInfoReq               = (u8)15,       // 消息请求
    emInfoReply             = (u8)16,       // 消息响应
};

//对端不可及原因
enum enumIcmpCodeUnreachable
{
    emUnreachNet        = (u8)0,// net unreachable
    emUnreachHost       = (u8)1,// host unreachable
    emUnreachProt       = (u8)2,// protocol unreachable
    emUnreachPort       = (u8)3,// port unreachable
    emUnreachFrag       = (u8)4,// fragmentation needed and DF set
    emUnreachRoute      = (u8)5 // source route failed
};

//超时原因
enum enumIcmpCodeTime
{
    emTimeTTL           = (u8)0,// time to live exceeded in transit
    emTimefrag          = (u8)1 // fragment reassembly time exceeded
};

//重定向原因
enum enumIcmpCodeRedirect
{
    emRedirectNet       = (u8)0,// Redirect datagrams for the Network
    emRedirectHost      = (u8)1,// Redirect datagrams for the Host
    emRedirectTOSNet    = (u8)2,// Redirect datagrams for the Type of Service and Network
    emRedirectTOSHost   = (u8)3 // Redirect datagrams for the Type of Service and Host
};

//  IP Header -- RFC 791
//    0                   1                   2                   3   
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |Version|  IHL  |Type of Service|          Total Length         |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |         Identification        |Flags|      Fragment Offset    |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |  Time to Live |    Protocol   |         Header Checksum       |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                       Source Address                          |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                    Destination Address                        |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                    Options                    |    Padding    |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef struct tagIPHDR
{
    /*=============================================================================
    函 数 名:tagIPHDR
    功    能:构造函数
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/15  4.0     王昊    创建
    =============================================================================*/
    tagIPHDR()
    {
        memset( this, 0, sizeof (tagIPHDR) );
    }

    /*=============================================================================
    函 数 名:SetVersion
    功    能:设置版本号
    参    数:enumIPVersion enumVersion      [in]    emIPv4/emIpv6
    注    意:The Version field indicates the format of the internet header
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/15  4.0     王昊    创建
    =============================================================================*/
    inline void SetVersion(enumIPVersion emIPVersion)
    { m_byVIHL = (m_byVIHL & 0x0F) | (emIPVersion); }

    /*=============================================================================
    函 数 名:GetVersion
    功    能:获取版本号
    参    数:无
    注    意:无
    返 回 值:版本号
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/15  4.0     王昊    创建
    =============================================================================*/
    inline enumIPVersion GetVersion(void) const
    { return static_cast<enumIPVersion>(m_byVIHL & 0xF0); }

    /*=============================================================================
    函 数 名:SetIHL
    功    能:设置Internet Header Length
    参    数:u8 byIHL                       [in]    MIN_IPHEADER_LEN
    注    意:The length of the internet header in 32 bit words.
             The minimum value for a correct header is 5
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/15  4.0     王昊    创建
    =============================================================================*/
    inline void SetIHL(u8 byIHL)
    { m_byVIHL = (m_byVIHL & 0xF0) | (byIHL & 0x0F); }

    /*=============================================================================
    函 数 名:GetIHL
    功    能:获取Internet Header Length
    参    数:无
    注    意:无
    返 回 值:Internet Header Length
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/15  4.0     王昊    创建
    =============================================================================*/
    inline u8 GetIHL(void) const { return (m_byVIHL & 0x0F); }

    /*=============================================================================
    函 数 名:SetPrecedence
    功    能:设置优先类型
    参    数:enumPrecedence emPrecedence    [in]    Precedence
    注    意:Several networks offer service precedence, which somehow treats high
               precedence traffic as more important than other traffic (generally
               by accepting only traffic above a certain precedence at time of
               high load).
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/15  4.0     王昊    创建
    =============================================================================*/
    inline void SetPrecedence(enumPrecedence emPrecedence)
    { m_byTOS = ( m_byTOS & 0x1F ) | ( emPrecedence); }

    /*=============================================================================
    函 数 名:SetPrecedence
    功    能:获取优先类型
    参    数:无
    注    意:无
    返 回 值:优先类型
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/15  4.0     王昊    创建
    =============================================================================*/
    inline enumPrecedence GetPrecedence(void) const
    { return static_cast<enumPrecedence>(m_byTOS & ~0x1F); }

    /*=============================================================================
    函 数 名:SetLowDelay
    功    能:设置延时类型
    参    数:BOOL32 bLowDelay               [in]    TRUE: 低延时; FALSE: 正常延时
    注    意:The use of the Delay, Throughput, and Reliability indications may
               increase the cost (in some sense) of the service.
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/15  4.0     王昊    创建
    =============================================================================*/
    inline void SetLowDelay(BOOL32 bLowDelay)
    { bLowDelay ? m_byTOS |= 0x10 : m_byTOS &= ~0x10; }

    /*=============================================================================
    函 数 名:IsLowDelay
    功    能:是否低延时
    参    数:无
    注    意:无
    返 回 值:TRUE: 低延时; FALSE: 正常延时
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/15  4.0     王昊    创建
    =============================================================================*/
    inline BOOL32 IsLowDelay(void) const { return (m_byTOS & 0x10) > 0; }

    /*=============================================================================
    函 数 名:SetHighThroughput
    功    能:设置高吞吐量
    参    数:BOOL32 bHighThroughput         [in]    TRUE: 高吞吐; FALSE: 正常吞吐
    注    意:The use of the Delay, Throughput, and Reliability indications may
               increase the cost (in some sense) of the service.
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/15  4.0     王昊    创建
    =============================================================================*/
    inline void SetHighThroughput(BOOL32 bHighThroughput)
    { bHighThroughput ? m_byTOS |= 0x08 : m_byTOS &= ~0x08; }

    /*=============================================================================
    函 数 名:IsHighThroughput
    功    能:是否高吞吐量
    参    数:无
    注    意:无
    返 回 值:TRUE: 低延时; FALSE: 正常延时
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/15  4.0     王昊    创建
    =============================================================================*/
    inline BOOL32 IsHighThroughput(void) const { return (m_byTOS & 0x08) > 0; }

    /*=============================================================================
    函 数 名:SetHighRelibility
    功    能:设置高可靠性
    参    数:BOOL32 bHighRelibility         [in]    TRUE: 高可靠; FALSE: 正常可靠
    注    意:The use of the Delay, Throughput, and Reliability indications may
               increase the cost (in some sense) of the service.
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/15  4.0     王昊    创建
    =============================================================================*/
    inline void SetHighRelibility(BOOL32 bHighRelibility)
    { bHighRelibility ? m_byTOS |= 0x04 : m_byTOS &= ~0x04; }

    /*=============================================================================
    函 数 名:IsHighRelibility
    功    能:是否高可靠性
    参    数:无
    注    意:无
    返 回 值:TRUE: 高可靠性; FALSE: 正常可靠性
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/15  4.0     王昊    创建
    =============================================================================*/
    inline BOOL32 IsHighRelibility(void) const { return (m_byTOS & 0x04) > 0; }

    /*=============================================================================
    函 数 名:SetTotalLen
    功    能:设置包长
    参    数:u16 wTotalLen                  [in]    包长(主机序)
    注    意:Total Length is the length of the datagram, measured in octets,
               including internet header and data. All hosts must be prepared
               to accept datagrams of up to 576 octets(whether they arrive whole
               or in fragments).
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/16  4.0     王昊    创建
    =============================================================================*/
    inline void SetTotalLen(u16 wTotalLen) { m_wTotalLen = htons( wTotalLen ); }

    /*=============================================================================
    函 数 名:GetTotalLen
    功    能:获取包长(主机序)
    参    数:无
    注    意:无
    返 回 值:包长, 包括头和内容
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/16  4.0     王昊    创建
    =============================================================================*/
    inline u16 GetTotalLen(void) const { return ntohs( m_wTotalLen ); }

    /*=============================================================================
    函 数 名:SetID
    功    能:设置Identification
    参    数:u16 wID                        [in]    ID(主机序)
    注    意:An identifying value assigned by the sender to aid in assembling the
               fragments of a datagram.
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/16  4.0     王昊    创建
    =============================================================================*/
    inline void SetID(u16 wID) { m_wID = htons( wID ); }

    /*=============================================================================
    函 数 名:GetID
    功    能:获取Identification(主机序)
    参    数:无
    注    意:无
    返 回 值:Identification
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/16  4.0     王昊    创建
    =============================================================================*/
    inline u16 GetID(void) const { return ntohs( m_wID ); }

    /*=============================================================================
    函 数 名:SetNoFragment
    功    能:设置不允许分块
    参    数:无
    注    意:会清空Fragment的Offset
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/16  4.0     王昊    创建
    =============================================================================*/
    inline void SetNoFragment(void) { m_wFlagOff = htons( 0x4000 ); }

    /*=============================================================================
    函 数 名:IsNoFragment
    功    能:判断是否不允许分块
    参    数:无
    注    意:无
    返 回 值:不允许分块: TRUE; 允许分块: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/16  4.0     王昊    创建
    =============================================================================*/
    inline BOOL32 IsNoFragment(void) const
    { return ( ntohs( m_wFlagOff ) & 0x4000 ) == 0; }

    /*=============================================================================
    函 数 名:SetLastFragment
    功    能:设置本包为最后一个分块
    参    数:无
    注    意:先设置允许分块, 然后设置本包是Last Fragment
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/16  4.0     王昊    创建
    =============================================================================*/
    inline void SetLastFragment(void)
    {
        u16 wFlagOff = ntohs( m_wFlagOff );
        wFlagOff &= ~0x4000;
        wFlagOff &= ~0x2000;
        m_wFlagOff = htons( wFlagOff );
    }

    /*=============================================================================
    函 数 名:IsLastFragment
    功    能:判断本包是否为最后一个分块
    参    数:无
    注    意:无
    返 回 值:允许分块, 且本包是最后一块: TRUE; 不允许分块, 或本包不是最后一块: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/16  4.0     王昊    创建
    =============================================================================*/
    inline BOOL32 IsLastFragment(void) const
    {
        u16 wFlagOff = ntohs( m_wFlagOff );
        return ((wFlagOff & 0x4000) == 0) && ((wFlagOff & 0x2000) == 0);
    }

    /*=============================================================================
    函 数 名:SetMoreFragment
    功    能:设置本包不是最后一个分块
    参    数:无
    注    意:先设置允许分块, 然后设置本包不是最后一块
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/16  4.0     王昊    创建
    =============================================================================*/
    inline void SetMoreFragment(void)
    {
        u16 wFlagOff = ntohs( m_wFlagOff );
        wFlagOff &= ~0x4000;
        wFlagOff |= 0x2000;
        m_wFlagOff = htons( wFlagOff );
    }

    /*=============================================================================
    函 数 名:IsMoreFragment
    功    能:判断本包是否不是最后一个分块
    参    数:无
    注    意:无
    返 回 值:允许分块, 且本包不是最后一块: TRUE; 不允许分块, 或本包不是最后一块: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/16  4.0     王昊    创建
    =============================================================================*/
    inline BOOL32 IsMoreFragment(void) const
    {
        u16 wFlagOff = ntohs( m_wFlagOff );
        return ((wFlagOff & 0x4000) == 0) && ((wFlagOff & 0x2000) > 0);
    }

    /*=============================================================================
    函 数 名:SetFragmentOffset
    功    能:设置块位移
    参    数:u16 wOffset                    [in]    块位移(只有后13bit有效)
    注    意:Where in the datagram this fragment belongs.
               The fragment offset is measured in units of 8 octets (64 bits).
               The first fragment has offset zero.
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/16  4.0     王昊    创建
    =============================================================================*/
    inline void SetFragmentOffset(u16 wOffset)
    {
        u16 wFlagOff = ntohs( m_wFlagOff );
        wFlagOff |= (wOffset & 0x1FFF);
        m_wFlagOff = htons( wFlagOff );
    }

    /*=============================================================================
    函 数 名:GetFragmentOffset
    功    能:获取块位移
    参    数:无
    注    意:无
    返 回 值:块位移(只有后13bit有效)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/16  4.0     王昊    创建
    =============================================================================*/
    inline u16 GetFragmentOffset(void) const
    {
        return ( ntohs( m_wFlagOff ) & 0x1FFF );
    }

    /*=============================================================================
    函 数 名:SetTimeToLive
    功    能:设置生存时间
    参    数:u8 byTTL                       [in]    生存时间
    注    意:The maximum time the datagram is allowed to remain in the internet system.
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/16  4.0     王昊    创建
    =============================================================================*/
    inline void SetTimeToLive(u8 byTTL) { m_byTTL = byTTL; }

    /*=============================================================================
    函 数 名:GetTimeToLive
    功    能:获取生存时间
    参    数:无
    注    意:无
    返 回 值:生存时间
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/16  4.0     王昊    创建
    =============================================================================*/
    inline u8 GetTimeToLive(void) const { return m_byTTL; }

    /*=============================================================================
    函 数 名:SetProtocol
    功    能:设置协议类型
    参    数:u8 byProtocol                  [in]    协议类型IPPROTO_TCP/IPPROTO_ICMP等
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/25  4.0     王昊    创建
    =============================================================================*/
    inline void SetProtocol(u8 byProtocol) { m_byProtocol = byProtocol; }

    /*=============================================================================
    函 数 名:GetProtocol
    功    能:获取协议类型
    参    数:无
    注    意:无
    返 回 值:协议类型
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/25  4.0     王昊    创建
    =============================================================================*/
    inline u8 GetProtocol(void) const { return m_byProtocol; }

    /*=============================================================================
    函 数 名:SetCheckSum
    功    能:设置校验码
    参    数:u16 wCheckSum                  [in]    校验码(操作系统顺序)
    注    意:使用MakeCheckSum计算校验码
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/25  4.0     王昊    创建
    =============================================================================*/
    inline void SetCheckSum(u16 wCheckSum) { m_wCheckSum = wCheckSum; }

    /*=============================================================================
    函 数 名:GetCheckSum
    功    能:获取校验码
    参    数:无
    注    意:使用MakeCheckSum计算校验码
    返 回 值:校验码(操作系统顺序)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/25  4.0     王昊    创建
    =============================================================================*/
    inline u16 GetCheckSum(void) const { return m_wCheckSum; }

    /*=============================================================================
    函 数 名:SetSrcIPAddr
    功    能:设置源IP地址
    参    数:u32 dwSrcIPAddr            [in]    源IP地址(主机序)
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/25  4.0     王昊    创建
    =============================================================================*/
    inline void SetSrcIPAddr(u32 dwSrcIPAddr)
    { m_dwSrcIPAddr = htonl( dwSrcIPAddr ); }

    /*=============================================================================
    函 数 名:GetSrcIPAddr
    功    能:获取源IP地址
    参    数:无
    注    意:无
    返 回 值:源IP地址(主机序)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/25  4.0     王昊    创建
    =============================================================================*/
    inline u32 GetSrcIPAddr(void) const { return ntohl( m_dwSrcIPAddr ); }

    /*=============================================================================
    函 数 名:SetDstIPAddr
    功    能:设置目的IP地址
    参    数:u32 dwSrcIPAddr            [in]    目的IP地址
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/25  4.0     王昊    创建
    =============================================================================*/
    inline void SetDstIPAddr(u32 dwDstIPAddr)
    { m_dwDstIPAddr = htonl( dwDstIPAddr ); }

    /*=============================================================================
    函 数 名:GetDstIPAddr
    功    能:获取目的IP地址
    参    数:无
    注    意:无
    返 回 值:目的IP地址(主机序)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/25  4.0     王昊    创建
    =============================================================================*/
    inline u32 GetDstIPAddr(void) const { return ntohl( m_dwDstIPAddr ); }

private:
    u8      m_byVIHL;       // Version and IHL

//  Bits 0-2:  Precedence.
//      Bit    3:  0 = Normal Delay,      1 = Low Delay.
//      Bits   4:  0 = Normal Throughput, 1 = High Throughput.
//      Bits   5:  0 = Normal Relibility, 1 = High Relibility.
//      Bit  6-7:  Reserved for Future Use.
//
//         0     1     2     3     4     5     6     7
//      +-----+-----+-----+-----+-----+-----+-----+-----+
//      |                 |     |     |     |     |     |
//      |   PRECEDENCE    |  D  |  T  |  R  |  0  |  0  |
//      |                 |     |     |     |     |     |
//      +-----+-----+-----+-----+-----+-----+-----+-----+
//
//        Precedence
//
//          111 - Network Control
//          110 - Internetwork Control
//          101 - CRITIC/ECP
//          100 - Flash Override
//          011 - Flash
//          010 - Immediate
//          001 - Priority
//          000 - Routine
    u8      m_byTOS;        // Type Of Service
    u16     m_wTotalLen;    // Total Length(网络序)
    u16     m_wID;          // Identification

//  Flags: 3 bits
//      Various Control Flags.
//      Bit 0: reserved, must be zero
//      Bit 1: (DF) 0 = May Fragment, 1 = Don’t Fragment.
//      Bit 2: (MF) 0 = Last Fragment, 1 = More Fragments.
//
//        0   1   2
//      +---+---+---+
//      |   | D | M |
//      | 0 | F | F |
//      +---+---+---+
//Fragment Offset: 13 bits
//      This field indicates where in the datagram this fragment belongs.
//      The fragment offset is measured in units of 8 octets (64 bits). The
//      first fragment has offset zero.
    u16     m_wFlagOff;     // Flags and Fragment Offset

    u8      m_byTTL;        // Time To Live
    u8      m_byProtocol;   // Protocol(IPPROTO_IP/IPPROTO_ICMP/IPPROTO_TCP...)
    u16     m_wCheckSum;    // Checksum(操作系统顺序)
    u32     m_dwSrcIPAddr;  // Internet Address - Source(网络序)
    u32     m_dwDstIPAddr;  // Internet Address - Destination(网络序)
} IIPHDR, *PIIPHDR;

//ICMP协议下的IP头结构
typedef struct tagIPHDRINICMP
{
public:
    /*=============================================================================
    函 数 名:tagIPHDRINICMP
    功    能:构造函数
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/29  4.0     王昊    创建
    =============================================================================*/
    tagIPHDRINICMP() : m_byType(0), m_byCode(0), m_wIcmpCheckSum(0)
    {
        m_tIPHdr.SetVersion( emIPv4 );
        m_tIPHdr.SetTotalLen( MIN_IPHEADER_LEN );
        m_tIPHdr.SetProtocol( IPPROTO_ICMP );
    }

    /*=============================================================================
    函 数 名:SetType
    功    能:设置类型
    参    数:enumIcmpType emIcmpType    [in]    ICMP消息头类型
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/29  4.0     王昊    创建
    =============================================================================*/
    inline void SetType(enumIcmpType emIcmpType) { m_byType = emIcmpType; }

    /*=============================================================================
    函 数 名:GetType
    功    能:获取类型
    参    数:无 
    注    意:无
    返 回 值:ICMP消息头类型
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/29  4.0     王昊    创建
    =============================================================================*/
    inline enumIcmpType GetType(void) const
    { return static_cast<enumIcmpType>(m_byType); }

    /*=============================================================================
    函 数 名:SetCode
    功    能:设置原因
    参    数:u8 byCode                  [in]    ICMP消息类型产生原因
    注    意:0/enumIcmpCodeUnreachable/enumIcmpCodeTime/enumIcmpCodeRedirect
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/29  4.0     王昊    创建
    =============================================================================*/
    inline void SetCode(u8 byCode) { m_byCode = byCode; }

    /*=============================================================================
    函 数 名:GetCode
    功    能:获取原因
    参    数:无
    注    意:无
    返 回 值:0/enumIcmpCodeUnreachable/enumIcmpCodeTime/enumIcmpCodeRedirect
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/29  4.0     王昊    创建
    =============================================================================*/
    inline u8 GetCode(void) const { return m_byCode; }

    /*=============================================================================
    函 数 名:SetIcmpCheckSum
    功    能:设置ICMP校验码
    参    数:u16 wCheckSum              [in]    校验码(操作系统顺序)
    注    意:参照SetCheckSum()
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/29  4.0     王昊    创建
    =============================================================================*/
    inline void SetIcmpCheckSum(u16 wCheckSum) { m_wIcmpCheckSum = wCheckSum; }

    /*=============================================================================
    函 数 名:GetIcmpCheckSum
    功    能:获取ICMP校验码
    参    数:无
    注    意:参照GetCheckSum()
    返 回 值:校验码(操作系统顺序)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/29  4.0     王昊    创建
    =============================================================================*/
    inline u16 GetIcmpCheckSum(void) const { return m_wIcmpCheckSum; }

public:
    IIPHDR  m_tIPHdr;       // IP头
private:
    u8      m_byType;       // Type
    u8      m_byCode;       // Code
    u16     m_wIcmpCheckSum;// CheckSum(操作系统顺序)
} IIPHDRINICMP, *PIIPHDRINICMP;

//IMCP消息结构
typedef struct tagICMPMsg
{
    /*=============================================================================
    函 数 名:tagICMPMsg
    功    能:构造函数
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/29  4.0     王昊    创建
    =============================================================================*/
    tagICMPMsg()
    {
        memset( m_abyBuf, 0, sizeof (m_abyBuf) );
    }

    IIPHDRINICMP    m_tIPHdr;       // IP头 + ICMP公用头
    u8              m_abyBuf[MAX_ICMP_SENDMSG_LEN + sizeof (u16) * 2];
} IICMPMsg, *PIICMPMsg;

//ICMP消息无法到达结构
//  If, according to the information in the gateway’s routing 
//tables, the network specified in the internet destination field 
//of a datagram is unreachable, e.g., the distance to the network 
//is infinity, the gateway may send a destination unreachable 
//message to the internet source host of the datagram. In addition, 
//in some networks, the gateway may be able to determine if the 
//internet destination host is unreachable. Gateways in these 
//networks may send destination unreachable messages to the source 
//host when the destination host is unreachable.
//  If, in the destination host, the IP module cannot deliver the
//datagram because the indicated protocol module or process port is
//not active, the destination host may send a destination
//unreachable message to the source host.
//  Another case is when a datagram must be fragmented to be 
//forwarded by a gateway yet the Don’t Fragment flag is on. In this 
//case the gateway must discard the datagram and may return a 
//destination unreachable message.
//  Codes 0, 1, 4, and 5 may be received from a gateway. Codes 2 and
//3 may be received from a host.
// 0                   1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|     Type      |     Code      |          Checksum             |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|                             unused                            |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|      Internet Header + 64 bits of Original Data Datagram      |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef struct tagIcmpUnreachMsg
{
    /*=============================================================================
    函 数 名:tagIcmpUnreachMsg
    功    能:构造函数
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/29  4.0     王昊    创建
    =============================================================================*/
    tagIcmpUnreachMsg() : m_dwReserved(0), m_dwIntHeader(0)
    {
        m_tIPHdr.SetType( emDstUnreachable );
        memset( m_abyDataGram, 0, sizeof (m_abyDataGram) );
    }

    /*=============================================================================
    函 数 名:GetCode
    功    能:获取原因
    参    数:无
    注    意:无
    返 回 值:enumIcmpCodeUnreachable
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/29  4.0     王昊    创建
    =============================================================================*/
    inline enumIcmpCodeUnreachable GetCode(void) const
    { return static_cast<enumIcmpCodeUnreachable>(m_tIPHdr.GetCode()); }

    IIPHDRINICMP    m_tIPHdr;           // IP头 + ICMP公用头
    u32             m_dwReserved;
    u32             m_dwIntHeader;      // Internet Header
    u8              m_abyDataGram[8];   // 64 bits of Original Data Datagram
} IIcmpUnreachMsg, *PIIcmpUnreachMsg;

//ICMP超时消息结构
//  If the gateway processing a datagram finds the time to live 
//field is zero it must discard the datagram. The gateway may also 
//notify the source host via the time exceeded message.
//  If a host reassembling a fragmented datagram cannot complete 
//the reassembly due to missing fragments within its time limit it 
//discards the datagram, and it may send a time exceeded message.
//  If fragment zero is not available then no time exceeded need be 
//sent at all.
//  Code 0 may be received from a gateway. Code 1 may be received 
//from a host.
// 0                   1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|      Type     |     Code      |          Checksum             |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|                             unused                            |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|     Internet Header + 64 bits of Original Data Datagram       |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef struct tagIcmpTimeMsg
{
    /*=============================================================================
    函 数 名:tagIcmpTimeMsg
    功    能:构造函数
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    tagIcmpTimeMsg() : m_dwReserved(0), m_dwIntHeader(0)
    {
        m_tIPHdr.SetType( emTimeExceeded );
        memset( m_abyDataGram, 0, sizeof (m_abyDataGram) );
    }

    /*=============================================================================
    函 数 名:GetCode
    功    能:获取原因
    参    数:无
    注    意:无
    返 回 值:enumIcmpCodeTime
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    inline enumIcmpCodeTime GetCode(void) const
    { return static_cast<enumIcmpCodeTime>(m_tIPHdr.GetCode()); }

    IIPHDRINICMP    m_tIPHdr;           // IP头 + ICMP公用头
    u32             m_dwReserved;
    u32             m_dwIntHeader;      // Internet Header
    u8              m_abyDataGram[8];   // 64 bits of Original Data Datagram
} IIcmpTimeMsg, *PIIcmpTimeMsg;

//ICMP参数错误消息结构
//  If the gateway or host processing a datagram finds a problem 
//with the header parameters such that it cannot complete processing 
//the datagram it must discard the datagram. One potential source of 
//such a problem is with incorrect arguments in an option. The 
//gateway or host may also notify the source host via the parameter 
//problem message. This message is only sent if the error caused 
//the datagram to be discarded.
//  The pointer identifies the octet of the original datagram's 
//header where the error was detected (it may be in the middle of an
//option). For example, 1 indicates something is wrong with the 
//Type of Service, and (if there are options present) 20 indicates 
//something is wrong with the type code of the first option.
//  Code 0 may be received from a gateway or a host.
// 0                   1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|     Type      |     Code      |          Checksum             |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|    Pointer    |                   unused                      |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|      Internet Header + 64 bits of Original Data Datagram      |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef struct tagIcmpParamMsg
{
    /*=============================================================================
    函 数 名:tagIcmpParamMsg
    功    能:构造函数
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    tagIcmpParamMsg() : m_byPtr(0), m_dwIntHeader(0)
    {
        m_tIPHdr.SetType( emParamError );
        memset( m_abyReserved, 0, sizeof (m_abyReserved) );
        memset( m_abyDataGram, 0, sizeof (m_abyDataGram) );
    }

    /*=============================================================================
    函 数 名:GetErrPtr
    功    能:获取错误指针位置
    参    数:无
    注    意:例如: 1: Type of Service; 20: Type; 21: Code
    返 回 值:指针位置
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    inline s32 GetErrPtr(void) const
    { return static_cast<s32>(m_byPtr); }

    IIPHDRINICMP    m_tIPHdr;           // IP头 + ICMP公用头
    u8              m_byPtr;            // 错误指针地址
    u8              m_abyReserved[3];
    u32             m_dwIntHeader;      // Internet Header
    u8              m_abyDataGram[8];   // 64 bits of Original Data Datagram
} IIcmpParamMsg, *PIIcmpParamMsg;

//ICMP源端停止消息结构
//  A gateway may discard internet datagrams if it does not have 
//the buffer space needed to queue the datagrams for output to the 
//next network on the route to the destination network. If a gateway 
//discards a datagram, it may send a source quench message to the 
//internet source host of the datagram. A destination host may also 
//send a source quench message if datagrams arrive too fast to be 
//processed. The source quench message is a request to the host to 
//cut back the rate at which it is sending traffic to the internet 
//destination. The gateway may send a source quench message for 
//every message that it discards. On receipt of a source quench 
//message, the source host should cut back the rate at which it is 
//sending traffic to the specified destination until it no longer 
//receives source quench messages from the gateway. The source host 
//can then gradually increase the rate at which it sends traffic to 
//the destination until it again receives source quench messages.
//  The gateway or host may send the source quench message when it 
//approaches its capacity limit rather than waiting until the 
//capacity is exceeded. This means that the data datagram which 
//triggered the source quench message may be delivered.
//  Code 0 may be received from a gateway or a host.
// 0                   1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|     Type      |     Code      |          Checksum             |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|                             unused                            |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|      Internet Header + 64 bits of Original Data Datagram      |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef struct tagIcmpQuenchMsg
{
    /*=============================================================================
    函 数 名:tagIcmpQuenchMsg
    功    能:构造函数
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    tagIcmpQuenchMsg() : m_dwReserved(0), m_dwIntHeader(0)
    {
        m_tIPHdr.SetType( emSourceQuench );
        memset( m_abyDataGram, 0, sizeof (m_abyDataGram) );
    }

    IIPHDRINICMP    m_tIPHdr;           // IP头 + ICMP公用头
    u32             m_dwReserved;
    u32             m_dwIntHeader;      // Internet Header
    u8              m_abyDataGram[8];   // 64 bits of Original Data Datagram
} IIcmpQuenchMsg, *PIIcmpQuenchMsg;

//ICMP重定向消息结构
//  The gateway sends a redirect message to a host in the following 
//situation. A gateway, G1, receives an internet datagram from a 
//host on a network to which the gateway is attached. The gateway, 
//G1, checks its routing table and obtains the address of the next 
//gateway, G2, on the route to the datagram’s internet destination 
//network, X. If G2 and the host identified by the internet source 
//address of the datagram are on the same network, a redirect 
//message is sent to the host. The redirect message advises the 
//host to send its traffic for network X directly to gateway G2 as 
//this is a shorter path to the destination. The gateway forwards 
//the original datagram’s data to its internet destination.
//  For datagrams with the IP source route options and the gateway 
//address in the destination address field, a redirect message is 
//not sent even if there is a better route to the ultimate 
//destination than the next address in the source route.
//  Codes 0, 1, 2, and 3 may be received from a gateway.
// 0                   1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|     Type      |     Code      |          Checksum             |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|                 Gateway Internet Address                      |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|      Internet Header + 64 bits of Original Data Datagram      |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef struct tagIcmpRedirectMsg
{
    /*=============================================================================
    函 数 名:tagIcmpRedirectMsg
    功    能:构造函数
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    tagIcmpRedirectMsg() : m_dwGatewayAddr(0), m_dwIntHeader(0)
    {
        m_tIPHdr.SetType( emRedirect );
        memset( m_abyDataGram, 0, sizeof (m_abyDataGram) );
    }

    /*=============================================================================
    函 数 名:GetCode
    功    能:获取原因
    参    数:无
    注    意:无
    返 回 值:enumIcmpCodeRedirect
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    inline enumIcmpCodeRedirect GetCode(void) const
    { return static_cast<enumIcmpCodeRedirect>(m_tIPHdr.GetCode()); }

    /*=============================================================================
    函 数 名:GetGatewayAddr
    功    能:获取网关地址
    参    数:无
    注    意:无
    返 回 值:网关地址(主机序)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    inline u32 GetGatewayAddr(void) const
    { return ntohl(m_dwGatewayAddr); }

public:
    IIPHDRINICMP    m_tIPHdr;           // IP头 + ICMP公用头
private:
    u32             m_dwGatewayAddr;    // 网关地址(网络序)
public:
    u32             m_dwIntHeader;      // Internet Header
    u8              m_abyDataGram[8];   // 64 bits of Original Data Datagram
} IIcmpRedirectMsg, *PIIcmpRedirectMsg;

//ICMP回应消息结构
//  The data received in the echo message must be returned in the 
//echo reply message.
//  The identifier and sequence number may be used by the echo 
//sender to aid in matching the replies with the echo requests.
//For example, the identifier might be used like a port in TCP or 
//UDP to identify a session, and the sequence number might be 
//incremented on each echo request sent. The echoer returns these 
//same values in the echo reply.
//  Code 0 may be received from a gateway or a host.
// 0                   1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|     Type      |     Code      |          Checksum             |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|           Identifier          |        Sequence Number        |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|     Data ...
//+-+-+-+-+-
typedef struct tagIcmpEchoMsg
{
    /*=============================================================================
    函 数 名:tagIcmpEchoMsg
    功    能:构造函数
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    tagIcmpEchoMsg() : m_wID(0), m_wSerialNo(0)
    {
        m_tIPHdr.SetType( emEcho );
        memset( m_abyData, 0, sizeof (m_abyData) );
    }

    /*=============================================================================
    函 数 名:GetID
    功    能:获取Identifier
    参    数:无
    注    意:无
    返 回 值:Identifier(主机序)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    inline u16 GetID(void) const { return ntohs(m_wID); }

    /*=============================================================================
    函 数 名:SetID
    功    能:设置Identifier
    参    数:u16 wID                        [in]    Identifier(主机序)
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/06/05  4.0     王昊    创建
    =============================================================================*/
    inline void SetID(u16 wID) { m_wID = htons(wID); }

    /*=============================================================================
    函 数 名:GetSerialNo
    功    能:获取Sequence Number
    参    数:无
    注    意:无
    返 回 值:Sequence Number(主机序)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    inline u16 GetSerialNo(void) const { return ntohs(m_wSerialNo); }

    /*=============================================================================
    函 数 名:SetSerialNo
    功    能:设置Sequence Number
    参    数:u16 wSN                        [in]    Sequence Number(主机序)
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/06/05  4.0     王昊    创建
    =============================================================================*/
    inline u16 SetSerialNo(u16 wSN) { m_wSerialNo = htons(wSN); }

public:
    IIPHDRINICMP    m_tIPHdr;           // IP头 + ICMP公用头
private:
    u16             m_wID;              // Identifier(网络序)
    u16             m_wSerialNo;        // Sequence Number(网络序)
public:
    u8              m_abyData[MAX_ICMP_SENDMSG_LEN];
} IIcmpEchoMsg, *PIIcmpEchoMsg;

//ICMP回应响应消息结构
//  The data received in the echo message must be returned in the 
//echo reply message.
//  The identifier and sequence number may be used by the echo 
//sender to aid in matching the replies with the echo requests.
//For example, the identifier might be used like a port in TCP or 
//UDP to identify a session, and the sequence number might be 
//incremented on each echo request sent. The echoer returns these 
//same values in the echo reply.
//  Code 0 may be received from a gateway or a host.
// 0                   1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|     Type      |     Code      |          Checksum             |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|           Identifier          |        Sequence Number        |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|     Data ...
//+-+-+-+-+-
typedef struct tagIcmpEchoReplyMsg
{
    /*=============================================================================
    函 数 名:tagIcmpEchoMsg
    功    能:构造函数
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    tagIcmpEchoReplyMsg() : m_wID(0), m_wSerialNo(0)
    {
        m_tIPHdr.SetType( emEchoReply );
        memset( m_abyData, 0, sizeof (m_abyData) );
    }

    /*=============================================================================
    函 数 名:GetID
    功    能:获取Identifier
    参    数:无
    注    意:无
    返 回 值:Identifier(主机序)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    inline u16 GetID(void) const { return ntohs(m_wID); }

    /*=============================================================================
    函 数 名:GetSerialNo
    功    能:获取Sequence Number
    参    数:无
    注    意:无
    返 回 值:Sequence Number(主机序)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    inline u16 GetSerialNo(void) const { return ntohs(m_wSerialNo); }

public:
    IIPHDRINICMP    m_tIPHdr;           // IP头 + ICMP公用头
private:
    u16             m_wID;              // Identifier(网络序)
    u16             m_wSerialNo;        // Sequence Number(网络序)
public:
    u8              m_abyData[MAX_ICMP_SENDMSG_LEN];
} IIcmpEchoReplyMsg, *PIIcmpEchoReplyMsg;

//ICMP时间戳消息结构
//  The data received (a timestamp) in the message is returned in 
//the reply together with an additional timestamp. The timestamp 
//is 32 bits of milliseconds since midnight UT.
//  The Originate Timestamp is the time the sender last touched the 
//message before sending it, the Receive Timestamp is the time the 
//echoer first touched it on receipt, and the Transmit Timestamp is 
//the time the echoer last touched the message on sending it.
//  If the time is not available in milliseconds or cannot be 
//provided with respect to midnight UT then any time can be inserted 
//in a timestamp provided the high order bit of the timestamp is also 
//set to indicate this non-standard value.
//  The identifier and sequence number may be used by the echo sender 
//to aid in matching the replies with the requests. For example, the 
//identifier might be used like a port in TCP or UDP to identify a 
//a session, and the sequence number might be incremented on each 
//request sent. The destination returns these same values in the reply.
//  Code 0 may be received from a gateway or a host.
// 0                   1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|      Type     |      Code     |          Checksum             |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|           Identifier          |        Sequence Number        |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|      Originate Timestamp                                      |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|      Receive Timestamp                                        |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|      Transmit Timestamp                                       |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef struct tagIcmpTimestampMsg
{
    /*=============================================================================
    函 数 名:tagIcmpTimestampMsg
    功    能:构造函数
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    tagIcmpTimestampMsg() : m_wID(0), m_wSerialNo(0), m_dwOrigStamp(0),
                            m_dwRecvStamp(0), m_dwTranStamp(0)
    {
        m_tIPHdr.SetType( emTimeStamp );
    }

    /*=============================================================================
    函 数 名:GetID
    功    能:获取Identifier
    参    数:无
    注    意:无
    返 回 值:Identifier(主机序)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    inline u16 GetID(void) const { return ntohs(m_wID); }

    /*=============================================================================
    函 数 名:GetSerialNo
    功    能:获取Sequence Number
    参    数:无
    注    意:无
    返 回 值:Sequence Number(主机序)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    inline u16 GetSerialNo(void) const { return ntohs(m_wSerialNo); }

    /*=============================================================================
    函 数 名:GetOrigStamp
    功    能:获取Originate Timestamp(毫秒)
    参    数:无
    注    意:无
    返 回 值:Originate Timestamp(主机序)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    inline u32 GetOrigStamp(void) const { return ntohl(m_dwOrigStamp); }

    /*=============================================================================
    函 数 名:GetRecvStamp
    功    能:获取Transmit Timestamp(毫秒)
    参    数:无
    注    意:无
    返 回 值:Transmit Timestamp(主机序)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    inline u32 GetRecvStamp(void) const { return ntohl(m_dwRecvStamp); }

    /*=============================================================================
    函 数 名:GetTranStamp
    功    能:获取Transmit Timestamp(毫秒)
    参    数:无
    注    意:无
    返 回 值:Transmit Timestamp(主机序)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    inline u32 GetTranStamp(void) const { return ntohl(m_dwTranStamp); }

public:
    IIPHDRINICMP    m_tIPHdr;           // IP头 + ICMP公用头
private:
    u16             m_wID;              // Identifier(网络序)
    u16             m_wSerialNo;        // Sequence Number(网络序)
    u32             m_dwOrigStamp;      // Originate Timestamp(毫秒)(网络序)
    u32             m_dwRecvStamp;      // Receive Timestamp(毫秒)(网络序)
    u32             m_dwTranStamp;      // Transmit Timestamp(毫秒)(网络序)
} IIcmpTimestampMsg, *PIIcmpTimestampMsg;

//ICMP时间戳回应消息结构
//  The data received (a timestamp) in the message is returned in 
//the reply together with an additional timestamp. The timestamp 
//is 32 bits of milliseconds since midnight UT.
//  The Originate Timestamp is the time the sender last touched the 
//message before sending it, the Receive Timestamp is the time the 
//echoer first touched it on receipt, and the Transmit Timestamp is 
//the time the echoer last touched the message on sending it.
//  If the time is not available in milliseconds or cannot be 
//provided with respect to midnight UT then any time can be inserted 
//in a timestamp provided the high order bit of the timestamp is also 
//set to indicate this non-standard value.
//  The identifier and sequence number may be used by the echo sender 
//to aid in matching the replies with the requests. For example, the 
//identifier might be used like a port in TCP or UDP to identify a 
//a session, and the sequence number might be incremented on each 
//request sent. The destination returns these same values in the reply.
//  Code 0 may be received from a gateway or a host.
// 0                   1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|      Type     |      Code     |          Checksum             |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|           Identifier          |        Sequence Number        |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|      Originate Timestamp                                      |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|      Receive Timestamp                                        |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|      Transmit Timestamp                                       |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef struct tagIcmpTimestampReplyMsg
{
    /*=============================================================================
    函 数 名:tagIcmpTimestampReplyMsg
    功    能:构造函数
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    tagIcmpTimestampReplyMsg() : m_wID(0), m_wSerialNo(0), m_dwOrigStamp(0),
                                 m_dwRecvStamp(0), m_dwTranStamp(0)
    {
        m_tIPHdr.SetType( emTimeStampReply );
    }

    /*=============================================================================
    函 数 名:GetID
    功    能:获取Identifier
    参    数:无
    注    意:无
    返 回 值:Identifier(主机序)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    inline u16 GetID(void) const { return ntohs(m_wID); }

    /*=============================================================================
    函 数 名:GetSerialNo
    功    能:获取Sequence Number
    参    数:无
    注    意:无
    返 回 值:Sequence Number(主机序)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    inline u16 GetSerialNo(void) const { return ntohs(m_wSerialNo); }

    /*=============================================================================
    函 数 名:GetOrigStamp
    功    能:获取Originate Timestamp(毫秒)
    参    数:无
    注    意:无
    返 回 值:Originate Timestamp(主机序)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    inline u32 GetOrigStamp(void) const { return ntohl(m_dwOrigStamp); }

    /*=============================================================================
    函 数 名:GetRecvStamp
    功    能:获取Transmit Timestamp(毫秒)
    参    数:无
    注    意:无
    返 回 值:Transmit Timestamp(主机序)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    inline u32 GetRecvStamp(void) const { return ntohl(m_dwRecvStamp); }

    /*=============================================================================
    函 数 名:GetTranStamp
    功    能:获取Transmit Timestamp(毫秒)
    参    数:无
    注    意:无
    返 回 值:Transmit Timestamp(主机序)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    inline u32 GetTranStamp(void) const { return ntohl(m_dwTranStamp); }

public:
    IIPHDRINICMP    m_tIPHdr;           // IP头 + ICMP公用头
private:
    u16             m_wID;              // Identifier(网络序)
    u16             m_wSerialNo;        // Sequence Number(网络序)
    u32             m_dwOrigStamp;      // Originate Timestamp(毫秒)(网络序)
    u32             m_dwRecvStamp;      // Receive Timestamp(毫秒)(网络序)
    u32             m_dwTranStamp;      // Transmit Timestamp(毫秒)(网络序)
} IIcmpTimestampReplyMsg, *PIIcmpTimestampReplyMsg;

//ICMP消息请求结构
//  This message may be sent with the source network in the IP 
//header source and destination address fields zero (which means 
//"this" network). The replying IP module should send the reply 
//with the addresses fully specified. This message is a way for 
//a host to find out the number of the network it is on.
//  The identifier and sequence number may be used by the echo 
//sender to aid in matching the replies with the requests. For 
//example, the identifier might be used like a port in TCP or UDP 
//to identify a session, and the sequence number might be 
//incremented on each request sent. The destination returns these 
//same values in the reply.
//  Code 0 may be received from a gateway or a host.
// 0                   1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|     Type      |      Code     |          Checksum             |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|          Identifier           |        Sequence Number        |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef struct tagIcmpInfoReqMsg
{
    /*=============================================================================
    函 数 名:tagIcmpInfoReqMsg
    功    能:构造函数
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    tagIcmpInfoReqMsg() : m_wID(0), m_wSerialNo(0)
    {
        m_tIPHdr.SetType( emInfoReq );
    }

    /*=============================================================================
    函 数 名:GetID
    功    能:获取Identifier
    参    数:无
    注    意:无
    返 回 值:Identifier(主机序)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    inline u16 GetID(void) const { return ntohs(m_wID); }

    /*=============================================================================
    函 数 名:GetSerialNo
    功    能:获取Sequence Number
    参    数:无
    注    意:无
    返 回 值:Sequence Number(主机序)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    inline u16 GetSerialNo(void) const { return ntohs(m_wSerialNo); }

public:
    IIPHDRINICMP    m_tIPHdr;           // IP头 + ICMP公用头
private:
    u16             m_wID;              // Identifier(网络序)
    u16             m_wSerialNo;        // Sequence Number(网络序)
} IIcmpInfoReqMsg, *PIIcmpInfoReqMsg;

//ICMP消息回应结构
//  This message may be sent with the source network in the IP 
//header source and destination address fields zero (which means 
//"this" network). The replying IP module should send the reply 
//with the addresses fully specified. This message is a way for 
//a host to find out the number of the network it is on.
//  The identifier and sequence number may be used by the echo 
//sender to aid in matching the replies with the requests. For 
//example, the identifier might be used like a port in TCP or UDP 
//to identify a session, and the sequence number might be 
//incremented on each request sent. The destination returns these 
//same values in the reply.
//  Code 0 may be received from a gateway or a host.
// 0                   1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|     Type      |      Code     |          Checksum             |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|          Identifier           |        Sequence Number        |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef struct tagIcmpInfoReplyMsg
{
    /*=============================================================================
    函 数 名:tagIcmpInfoReplyMsg
    功    能:构造函数
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    tagIcmpInfoReplyMsg() : m_wID(0), m_wSerialNo(0)
    {
        m_tIPHdr.SetType( emInfoReply );
    }

    /*=============================================================================
    函 数 名:GetID
    功    能:获取Identifier
    参    数:无
    注    意:无
    返 回 值:Identifier(主机序)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    inline u16 GetID(void) const { return ntohs(m_wID); }

    /*=============================================================================
    函 数 名:GetSerialNo
    功    能:获取Sequence Number
    参    数:无
    注    意:无
    返 回 值:Sequence Number(主机序)
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/30  4.0     王昊    创建
    =============================================================================*/
    inline u16 GetSerialNo(void) const { return ntohs(m_wSerialNo); }

public:
    IIPHDRINICMP    m_tIPHdr;           // IP头 + ICMP公用头
private:
    u16             m_wID;              // Identifier(网络序)
    u16             m_wSerialNo;        // Sequence Number(网络序)
} IIcmpInfoReplyMsg, *PIIcmpInfoReplyMsg;


//ICMP自定义发送消息结构定义
typedef struct tagIcmpSendMsg
{
    /*=============================================================================
    函 数 名:tagIcmpSendMsg
    功    能:构造函数
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/06/05  4.0     王昊    创建
    =============================================================================*/
    tagIcmpSendMsg() : m_byType(emEcho), m_byCode(0), m_wCheckSum(0),
                       m_wID(0), m_wSerialNo(0), m_dwTimeStamp(0)
    {
        memset( m_abyBuf, 0, sizeof (m_abyBuf) );
    }

    /*=============================================================================
    函 数 名:SetID
    功    能:设置Identifier
    参    数:u16 wID                [in]    Identifier(主机序)
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/06/05  4.0     王昊    创建
    =============================================================================*/
    inline void SetID(u16 wID) { m_wID = htons(wID); }

    /*=============================================================================
    函 数 名:SetSerialNo
    功    能:设置Sequence Number
    参    数:u16 wSN                [in]    Sequence Number(主机序)
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/06/05  4.0     王昊    创建
    =============================================================================*/
    inline void SetSerialNo(u16 wSN) { m_wSerialNo = htons(wSN); }

    /*=============================================================================
    函 数 名:SetCheckSum
    功    能:设置校验码
    参    数:u16 wCheckSum          [in]    校验码(操作系统顺序)
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/06/05  4.0     王昊    创建
    =============================================================================*/
    inline void SetCheckSum(u16 wCheckSum) { m_wCheckSum = wCheckSum; }

    /*=============================================================================
    函 数 名:SetTimeStamp
    功    能:设置时间戳
    参    数:u32 dwTimeStamp        [in]    时间戳(主机序)
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/06/05  4.0     王昊    创建
    =============================================================================*/
    inline void SetTimeStamp(u32 dwTimeStamp) { m_dwTimeStamp = htonl(dwTimeStamp); }

private:
    u8              m_byType;       //消息类型
    u8              m_byCode;       //0
    u16             m_wCheckSum;    //校验码(操作系统顺序)
    u16             m_wID;          //Identifier(网络序)
    u16             m_wSerialNo;    //Sequence Number(网络序)
    u32             m_dwTimeStamp;  //时间戳, 非标准协议字段
public:
    u8              m_abyBuf[MAX_ICMP_SENDMSG_LEN];
} IIcmpSendMsg, *PIIcmpSendMsg;

/*=============================================================================
函 数 名:MakeCheckSum
功    能:计算校验码
参    数:u8 *pbyStruct              [in]    内存结构
         s32 nStructLen             [in]    结构长度
注    意:无
返 回 值:校验码(操作系统顺序)
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2006/05/30  4.0     王昊    创建
=============================================================================*/
inline u16 MakeCheckSum(u8 *pbyStruct, s32 nStructLen)
{
    register u32 dwSum = 0;
    u16 wCheckSum = 0;
    //  16bit累加
    for ( s32 nIndex = 0; nIndex < nStructLen; nIndex += 2 )
    {
        dwSum += static_cast<u32>(*reinterpret_cast<u16*>(pbyStruct + nIndex));
    }
    //  如果位数为奇数, 还要累加最后一位
    if ( ( nStructLen % 2 ) > 0 )
    {
        dwSum += static_cast<u32>(pbyStruct[nStructLen - 1]);
    }

    dwSum = (dwSum >> 16) + (dwSum & 0xFFFF);
    dwSum += (dwSum >> 16);
    wCheckSum = static_cast<u16>(~dwSum);
    return wCheckSum;
}

#endif  //  __ICMPSTRUCT_H__