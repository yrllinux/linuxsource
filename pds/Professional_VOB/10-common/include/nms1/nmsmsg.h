/*=============================================================================
模   块   名: 网管服务器
文   件   名: nmsmsg.h
相 关  文 件: 无
文件实现功能: 消息类
作        者: wjr
版        本: V4.0  Copyright(C) 2003-2005 KDC, All rights reserved.
-------------------------------------------------------------------------------
修改记录:
日      期  版本    修改人      修改内容
2003/06/13  2.0     wjr         创建
2003/10/13  3.0     wjr         修改
2005/11/07  4.0     王昊        代码规范化
=============================================================================*/

#ifndef __NMSMSG_H__20030613
#define __NMSMSG_H__20030613

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _WINSOCK2API_
#pragma comment(lib, "ws2_32.lib")
#endif

static u32 g_dwSerialNo = 1;    // 流水号

const s32 g_cnBufLen = 1024 * 2 + 16;
typedef struct tagTNmsMsgHead
{
    tagTNmsMsgHead()
    {
        ZeroMemory( this, sizeof (tagTNmsMsgHead) );
    }

    u32	m_dwSerialNo;	// 流水号, 网络序
    u16	m_wEventID;		// 事件编号, 网络序
    u16	m_wObjectType;	// 对象类型, 网络序
    u32	m_dwObjectID;	// 对象编号, 网络序
    u32	m_dwTargetID;	// 保留，提供给用户-组关系更改通知, 网络序
    u32	m_dwErrorCode;	// 错误码, 网络序
    u16	m_wTimeout;		// 定时时间长度--单位为秒, 网络序
    u16	m_wMsgLength;	// 消息内容长度, 网络序
} TNmsMsgHead, *PTNmsMsgHead;

#define MSG_BODY_OFFSET     sizeof(TNmsMsgHead) // 消息头为16个子节

// 网管模块消息类
class CNmsMsg
{
protected:
    u8 m_byContent[g_cnBufLen];	// 消息缓冲区，1k大小

public:
    CNmsMsg(LPVOID pbyMsgBody = NULL, u16 wBodyLen = 0)
    {
        ::ZeroMemory( m_byContent, g_cnBufLen );
        PTNmsMsgHead m_ptMsgHead = (PTNmsMsgHead)m_byContent;
        if ( pbyMsgBody != NULL && wBodyLen != 0 )
        {
            ::CopyMemory( &m_byContent[MSG_BODY_OFFSET], pbyMsgBody, wBodyLen );
            m_ptMsgHead->m_wMsgLength = htons( wBodyLen );
        }
        m_ptMsgHead->m_dwSerialNo = htonl( g_dwSerialNo ++ );
        m_ptMsgHead->m_wTimeout = htons( 1 );
    }
    CNmsMsg& operator= (const CNmsMsg &cMsg)
    {
    	::CopyMemory( m_byContent, cMsg.m_byContent, g_cnBufLen );
    	return (*this);
    }

	~CNmsMsg() {}

public:

    /*=============================================================================
    函 数 名:ClearMsgHead
    功    能:清除消息头部
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2003/10/23  2.0     wjr     创建
    =============================================================================*/
    void ClearMsgHead(void)
    {
        ::ZeroMemory( m_byContent, MSG_BODY_OFFSET );
    }

    /*=============================================================================
    函 数 名:ClearMsgBody
    功    能:清除消息内容
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2003/10/23  2.0     wjr     创建
    =============================================================================*/
    void ClearMsgBody(void)
    {
        ::ZeroMemory( &m_byContent[MSG_BODY_OFFSET],
                      g_cnBufLen - MSG_BODY_OFFSET );
    }

    /*=============================================================================
    函 数 名:ClearContent
    功    能:清除消息全部内容
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2003/10/23  2.0     wjr     创建
    =============================================================================*/
    void ClearContent(void)
    {
		::ZeroMemory( m_byContent, g_cnBufLen );
    }

    /*=============================================================================
    函 数 名:GetSerialNo
    功    能:获取流水号
    参    数:无
    注    意:无
    返 回 值:流水号
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2003/10/23  2.0     wjr     创建
    =============================================================================*/
    u32 GetSerialNo(void) const
    {
        PTNmsMsgHead m_ptMsgHead = (PTNmsMsgHead)m_byContent;
		if ( m_ptMsgHead == NULL )
			return 0;
		return ntohl( m_ptMsgHead->m_dwSerialNo );
	}

    /*=============================================================================
    函 数 名:SetSerialNo
    功    能:设置当前消息的流水号
    参    数:u32 dwSerialNo                 [in]    当前的流水号
    注    意:该方法一般不使用
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2003/10/23  2.0     wjr     创建
    =============================================================================*/
	void SetSerialNo(u32 dwSerialNo)
	{
		PTNmsMsgHead m_ptMsgHead = (PTNmsMsgHead)m_byContent;
		if ( m_ptMsgHead != NULL )
		{
			m_ptMsgHead->m_dwSerialNo = htonl( dwSerialNo );
		}
	}

    /*=============================================================================
    函 数 名:SetSerialNo
    功    能:设置消息的流水号
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2003/10/23  2.0     wjr     创建
    =============================================================================*/
	void SetSerialNo(void)
	{
		PTNmsMsgHead m_ptMsgHead = (PTNmsMsgHead)m_byContent;
		if ( m_ptMsgHead != NULL )
		{
			m_ptMsgHead->m_dwSerialNo = htonl( g_dwSerialNo ++ );
		}
	}

    /*=============================================================================
    函 数 名:GetObjectID
    功    能:获取对象编号
    参    数:无
    注    意:无
    返 回 值:对象编号
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2003/10/23  2.0     wjr     创建
    =============================================================================*/
	u32 GetObjectID(void) const
	{
		PTNmsMsgHead m_ptMsgHead = (PTNmsMsgHead)m_byContent;
		if ( m_ptMsgHead == NULL )
			return 0;
		return ntohl( m_ptMsgHead->m_dwObjectID );
	}

    /*=============================================================================
    函 数 名:GetObjectType
    功    能:获取对象类型
    参    数:无
    注    意:无
    返 回 值:对象类型
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2003/10/23  2.0     wjr     创建
    =============================================================================*/
	u16 GetObjectType(void) const
	{
		PTNmsMsgHead m_ptMsgHead = (PTNmsMsgHead)m_byContent;
		if ( m_ptMsgHead == NULL )
			return 0;
		return ntohs( m_ptMsgHead->m_wObjectType );
	}

    /*=============================================================================
    函 数 名:GetTargetID
    功    能:获取目的对象编号
    参    数:无
    注    意:该函数仅仅用于用户组关系变更的事件，
             如果对象是用户，则目的对象就是组
             否则，目的对象就是用户
    返 回 值:对象编号
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2003/10/23  2.0     wjr     创建
    =============================================================================*/
	u32 GetTargetID(void) const
	{
		PTNmsMsgHead m_ptMsgHead = (PTNmsMsgHead)m_byContent;
		if ( m_ptMsgHead == NULL )
			return 0;
		return ntohl( m_ptMsgHead->m_dwTargetID );
	}

    /*=============================================================================
    函 数 名:SetObject
    功    能:设置目的对象
    参    数:u32 dwObjectID                 [in]    本次消息的对象编号
             u16 wObjectType                [in]    本次消息的对象类型
             u32 dwTargetID                 [in]    本次消息的对象关联的对象编号
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2003/10/23  2.0     wjr     创建
    =============================================================================*/
	void SetObject(u32 dwObjectID, u16 wObjectType = 0, u32 dwTargetID = -1)
	{
		PTNmsMsgHead m_ptMsgHead = (PTNmsMsgHead)m_byContent;
		if ( m_ptMsgHead == NULL )
			return;
		m_ptMsgHead->m_wObjectType = htons( wObjectType );
		m_ptMsgHead->m_dwObjectID = htonl( dwObjectID );
		m_ptMsgHead->m_dwTargetID = htonl( dwTargetID );
	}

    /*=============================================================================
    函 数 名:SetErrorCode
    功    能:设置错误码
    参    数:u32 dwErrorCode                [in]    本次消息的错误码
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2003/10/23  2.0     wjr     创建
    =============================================================================*/
	void SetErrorCode(u32 dwErrorCode)
	{
		PTNmsMsgHead m_ptMsgHead = (PTNmsMsgHead)m_byContent;
		if ( m_ptMsgHead == NULL )
			return;
		m_ptMsgHead->m_dwErrorCode = htonl( dwErrorCode);
	}

    /*=============================================================================
    函 数 名:GetErrorCode
    功    能:获取本次消息的错误码
    参    数:无
    注    意:无
    返 回 值:错误码
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2003/10/23  2.0     wjr     创建
    =============================================================================*/
	u32 GetErrorCode(void) const
	{
		PTNmsMsgHead m_ptMsgHead = (PTNmsMsgHead)m_byContent;
		if ( m_ptMsgHead == NULL )
			return (u32)~0;
		return ntohl( m_ptMsgHead->m_dwErrorCode );
	}

    /*=============================================================================
    函 数 名:SetEventId
    功    能:设置事件编号
    参    数:u16 wEventId                   [in]    本次消息的事件
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2003/10/23  2.0     wjr     创建
    =============================================================================*/
	void SetEventId(u16 wEventId)
	{
		PTNmsMsgHead m_ptMsgHead = (PTNmsMsgHead)m_byContent;
		if ( m_ptMsgHead == NULL )
			return;
		m_ptMsgHead->m_wEventID = htons( wEventId );
	}

    /*=============================================================================
    函 数 名:GetEventId
    功    能:获取事件
    参    数:无
    注    意:无
    返 回 值:事件号
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2003/10/23  2.0     wjr     创建
    =============================================================================*/
	u16 GetEventId(void) const
	{
		PTNmsMsgHead m_ptMsgHead = (PTNmsMsgHead)m_byContent;
		if ( m_ptMsgHead == NULL )
			return 0;
		return ntohs( m_ptMsgHead->m_wEventID );
	}

    /*=============================================================================
    函 数 名:SetMsgBody
    功    能:设置消息的内容
    参    数:LPVOID pbyMsgBody              [in]    本次消息的内容
             u16 wBodyLen                   [in]    本次消息的内容长度
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2003/10/23  2.0     wjr     创建
    =============================================================================*/
	void SetMsgBody(LPVOID pbyMsgBody, u16 wBodyLen)
	{
		if ( pbyMsgBody != NULL && wBodyLen > 0 )
		{
			PTNmsMsgHead m_ptMsgHead = (PTNmsMsgHead)m_byContent;
			if ( m_ptMsgHead == NULL )
				return;
			::CopyMemory( &m_byContent[MSG_BODY_OFFSET], pbyMsgBody, wBodyLen );
			m_ptMsgHead->m_wMsgLength = htons( wBodyLen );
		}
	}

    /*=============================================================================
    函 数 名:CatMsgBody
    功    能:添加消息内容
    参    数:LPVOID pbyMsgBody              [in]    本次消息的内容
             u16 wBodyLen                   [in]    本次消息的内容长度
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2003/10/23  2.0     wjr     创建
    =============================================================================*/
	void CatMsgBody(LPVOID pbyMsgBody, u16 wBodyLen)
	{
		if ( pbyMsgBody != NULL && wBodyLen > 0) 
		{
			PTNmsMsgHead m_ptMsgHead = (PTNmsMsgHead)m_byContent;
			if ( m_ptMsgHead == NULL )
				return;
            s32 nOffset = MSG_BODY_OFFSET + ntohs( m_ptMsgHead->m_wMsgLength );
			::CopyMemory( m_byContent + nOffset, pbyMsgBody, wBodyLen );
			m_ptMsgHead->m_wMsgLength += htons( wBodyLen );
		}
	}

    /*=============================================================================
    函 数 名:GetMsgBody
    功    能:获取消息内容
    参    数:LPVOID pbyMsgBody              [out]   获取得到的消息内容
             u16 wBodyLen                   [in]    消息的长度
             u16 wOffset                    [in]    消息的偏移
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2003/10/23  2.0     wjr     创建
    =============================================================================*/
	u16 GetMsgBody(LPVOID pbyMsgBody, u16 wBodyLen, u16 wOffset = 0)
	{
		if ( pbyMsgBody == NULL )
		{
			return 0;
		}
		PTNmsMsgHead m_ptMsgHead = (PTNmsMsgHead)m_byContent;
		if ( m_ptMsgHead == NULL )
			return 0;
		u16 wLen = __min( wBodyLen, ntohs( m_ptMsgHead->m_wMsgLength ) );
		::CopyMemory( pbyMsgBody, m_byContent + MSG_BODY_OFFSET + wOffset, wLen );
		return wLen;
	}

    /*=============================================================================
    函 数 名:GetMsgBody
    功    能:获取消息内容
    参    数:u16 wOffset                    [in]    消息的偏移
    注    意:直接返回消息内容指针
    返 回 值:消息内容
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2003/10/23  2.0     wjr     创建
    =============================================================================*/
	LPVOID GetMsgBody(u16 wOffset = 0) const
	{
		PTNmsMsgHead m_ptMsgHead = (PTNmsMsgHead)m_byContent;
		if ( m_ptMsgHead == NULL )
			return NULL;
		if ( m_ptMsgHead->m_wMsgLength == 0 )
			return NULL;
		return (LPVOID)(m_byContent + MSG_BODY_OFFSET + wOffset);
	}

    /*=============================================================================
    函 数 名:GetMsgBodyLen
    功    能:获取消息体的长度
    参    数:无
    注    意:无
    返 回 值:消息体的长度
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2003/10/23  2.0     wjr     创建
    =============================================================================*/
	u16 GetMsgBodyLen(void) const
	{
		PTNmsMsgHead m_ptMsgHead = (PTNmsMsgHead)m_byContent;
		if ( m_ptMsgHead == NULL )
			return 0;
		return ntohs( m_ptMsgHead->m_wMsgLength );
	}

    /*=============================================================================
    函 数 名:SetTimeout
    功    能:设置定时时间
    参    数:u16 wTimeout                   [in]    定时的时间长度
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2003/10/23  2.0     wjr     创建
    =============================================================================*/
	void SetTimeout(u16 wTimeout)
	{
		PTNmsMsgHead m_ptMsgHead = (PTNmsMsgHead)m_byContent;
		if ( m_ptMsgHead == NULL )
			return;
		m_ptMsgHead->m_wTimeout = htons( wTimeout );
	}

    /*=============================================================================
    函 数 名:GetTimeout
    功    能:获取定时长度
    参    数:无
    注    意:无
    返 回 值:定时长度
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2003/10/23  2.0     wjr     创建
    =============================================================================*/
	u16 GetTimeout(void) const
	{
		PTNmsMsgHead m_ptMsgHead = (PTNmsMsgHead)m_byContent;
		if ( m_ptMsgHead == NULL )
			return 0;
		return ntohs( m_ptMsgHead->m_wTimeout );
	}
};


#endif // __NMSMSG_H__20030613
